#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <linux/v4l2-subdev.h>
#include <string>
#include "rk_aiq_types.h"
#include "rk_aiq_offline_raw.h"
#include "rk_vi_user_api2_stream_cfg.h"
#include "uAPI2/rk_aiq_user_api2_sysctl.h"
#include "rk_aiq_offline_raw.h"
#include <iostream>
#include <getopt.h>

#define USE_LIBAIQ

#define SENSOR_ENTNAME "m00_b_imx415 5-001a"

#define VIDEO_DEVNAME "/dev/video8"
#define VIDEO_WIDTH 3840
#define VIDEO_HEIGHT 2160
#define VIDEO_WORKMODE RK_AIQ_WORKING_MODE_NORMAL

#define VIDEO_WIDTH_2 3840
#define VIDEO_HEIGHT_2 2160
#define VIDEO_WORKMODE_2 RK_AIQ_WORKING_MODE_NORMAL

#define RAWCAP_SAVE_NUM    30
#define STREAM_SAVE_NUM    30

struct rk_aiq_vbuf_info {
    uint32_t frame_id;
    uint32_t timestamp;
    float    exp_time;
    float    exp_gain;
    uint32_t exp_time_reg;
    uint32_t exp_gain_reg;
    uint32_t data_fd;
    uint8_t *data_addr;
    uint32_t data_length;
    bool valid;
};

struct rk_aiq_vbuf {
    void *base_addr;
    uint32_t frame_width;
    uint32_t frame_height;
    struct rk_aiq_vbuf_info buf_info[3];/*index: 0-short,1-medium,2-long*/
};


static rk_aiq_rawbuf_type_t _rawbuf_type;
static struct _st_addrinfo _st_addr[3];
static struct _raw_format _rawfmt;
static rk_aiq_frame_info_t _finfo;

static rkraw_vi_ctx_t *g_vi_ctx;

static int g_quit = 0;
static int g_aiq_quit = 0;
static int g_aiq_pause = 0;
static int g_aiq_test_mode = 0;

static int saved_num = 0;
static char g_sns_entity_name[64] = {0};
static char g_sns_name[64] = {0};

static bool use_isp_driver = false;
static int raw_width = 3840;
static int raw_height = 2160;
static int video_width = 640;
static int video_height = 480;
static int modul_select = 0;
static int hdr_mode = 0;
static const char *isp_vir;
static const char *real_sns;
static const char *iq_file_name;
static const char *video_name;
static uint32_t pix_fmt = V4L2_PIX_FMT_SBGGR10;

static FILE* g_rkraw_fp;
static uint8_t* g_rkraw_buffer;
static uint32_t g_rkraw_size;
static int g_rkraw_index;

static struct timeval start_t, cur_t, prev_t;

static rk_aiq_sys_ctx_t* g_aiq_ctx;

// enum v4l2_memory _tx_memory_type;
// enum v4l2_memory _rx_memory_type;
// SmartPtr<V4l2Device> g_vi_ctx->_mRawProcUnit->_mipi_rx_devs[3];
// SmartPtr<V4l2Device> _mipi_tx_devs[3];

void debug_init_time(){
    gettimeofday(&start_t,NULL);
    gettimeofday(&cur_t,NULL);
    gettimeofday(&prev_t,NULL);
}

void debug_update_time(){
    gettimeofday(&prev_t,NULL);
}

void debug_print_time(std::string tag){
    unsigned  long tmp;
    double process_ms, total_ms,cur_ms;
    struct timespec kernel_t;

    gettimeofday(&cur_t,NULL);
    tmp = 1000000 * (cur_t.tv_sec-prev_t.tv_sec)+ cur_t.tv_usec-prev_t.tv_usec;
    prev_t = cur_t;
    process_ms = 1.0*tmp/1000;

    tmp = 1000000 * (cur_t.tv_sec-start_t.tv_sec)+ cur_t.tv_usec-start_t.tv_usec;
    total_ms = 1.0*tmp/1000;

    clock_gettime(CLOCK_BOOTTIME , &kernel_t);
    tmp = 1000000000 * kernel_t.tv_sec + kernel_t.tv_nsec;
    cur_ms = 1.0*tmp/1000000000;
    printf("[TIME_DEBUG %.1f] %s process_ms %d, total_ms %d\n", cur_ms, tag.c_str(), (int)process_ms, (int)total_ms);
}

int prepare_rkraw()
{
    int ret;
    if(g_rkraw_fp){
        fclose(g_rkraw_fp);
        g_rkraw_fp = NULL;
    }
    if(g_rkraw_buffer){
        free(g_rkraw_buffer);
        g_rkraw_buffer = NULL;
    }

    char filname[32];
    sprintf(filname, "mnt/0.raw");
    g_rkraw_fp = fopen(filname,"rb");
    if(!g_rkraw_fp){
        printf("can't open %s\n", filname);
        return -1;
    }

    fseek(g_rkraw_fp, 0, SEEK_END);
    g_rkraw_size=ftell(g_rkraw_fp);
    fseek(g_rkraw_fp, 0, SEEK_SET);

    g_rkraw_buffer = (uint8_t *)malloc(g_rkraw_size);
    if(!g_rkraw_buffer){
        printf("prepare_rkraw malloc failed!\n");
        return -1;
    }

    ret = fread(g_rkraw_buffer, 1, g_rkraw_size, g_rkraw_fp);
    printf("prepare_rkraw: ret %d, addr 0x%lx\n", ret, (uint64_t)g_rkraw_buffer);

    if (g_rkraw_index > 20) {
        return -1;
    }

    g_rkraw_index++;
    return 0;
}

void parse_rk_rawdata(uint8_t *rawdata, uint32_t rkraw_len, struct rk_aiq_vbuf *vbuf)
{
    unsigned short tag = 0;
    struct _block_header header;
    uint8_t *p = (uint8_t *)rawdata;
    bool bExit = false;

    struct _st_addrinfo_stream _st_addr[3];
    struct _raw_format _rawfmt;

    while(!bExit){
        tag = *((unsigned short*)p);
        switch (tag)
        {
            case START_TAG:
                p = p+TAG_BYTE_LEN;
                memset(_st_addr, 0, sizeof(_st_addr));
                memset(&_rawfmt, 0, sizeof(_rawfmt));
                memset(&_finfo, 0, sizeof(_finfo));
                break;
            case NORMAL_RAW_TAG:
            {
                header = *((struct _block_header *)p);
                p = p + sizeof(struct _block_header);
                if (header.block_length == sizeof(struct _st_addrinfo_stream)) {
                    _st_addr[0] = *((struct _st_addrinfo_stream*)p);
                }
                p = p + header.block_length;

                break;
            }
            case HDR_S_RAW_TAG:
            {
                header = *((struct _block_header *)p);
                p = p + sizeof(struct _block_header);
                if (header.block_length == sizeof(struct _st_addrinfo_stream)) {
                    _st_addr[0] = *((struct _st_addrinfo_stream*)p);
                }
                p = p + header.block_length;
                break;
            }
            case HDR_M_RAW_TAG:
            {
                header = *((struct _block_header *)p);
                p = p + sizeof(struct _block_header);
                if (header.block_length == sizeof(struct _st_addrinfo_stream)) {
                    _st_addr[1] = *((struct _st_addrinfo_stream*)p);
                }
                p = p + header.block_length;
                break;
            }
            case HDR_L_RAW_TAG:
            {
                header = *((struct _block_header *)p);
                p = p + sizeof(struct _block_header);
                if (header.block_length == sizeof(struct _st_addrinfo_stream)) {
                    _st_addr[2] = *((struct _st_addrinfo_stream*)p);
                }
                p = p + header.block_length;
                break;
            }
            case FORMAT_TAG:
            {
                _rawfmt = *((struct _raw_format *)p);
                p = p + sizeof(struct _block_header) + _rawfmt.size;
                break;
            }
            case STATS_TAG:
            {
                _finfo = *((rk_aiq_frame_info_t *)p);
                p = p + sizeof(struct _block_header) + _finfo.size;
                break;
            }
            case ISP_REG_FMT_TAG:
            {
                header = *((struct _block_header *)p);
                p += sizeof(struct _block_header);
                p = p + header.block_length;
                break;
            }
            case ISP_REG_TAG:
            {
                header = *((struct _block_header *)p);
                p += sizeof(struct _block_header);
                p = p + header.block_length;
                break;
            }
            case ISPP_REG_FMT_TAG:
            {
                header = *((struct _block_header *)p);
                p += sizeof(struct _block_header);
                p = p + header.block_length;
                break;
            }
            case ISPP_REG_TAG:
            {
                header = *((struct _block_header *)p);
                p += sizeof(struct _block_header);
                p = p + header.block_length;
                break;
            }
            case PLATFORM_TAG:
            {
                header = *((struct _block_header *)p);
                p += sizeof(struct _block_header);
                p = p + header.block_length;
                break;
            }
            case END_TAG:
            {
                //uint32_t _len = p - (uint8_t *)rawdata;
                //printf("_len %d rkraw_len %d\n", _len, rkraw_len);
                bExit = true;
                break;
            }
            default:
            {
                printf("Not support TAG(0x%04x)\n", tag);
                bExit = true;
                break;
            }
        }
    }

    printf("frame_id %d, size %d %d %d, format %dx%d\n",
            _rawfmt.frame_id,
            _st_addr[0].size,_st_addr[1].size,_st_addr[2].size,
            _rawfmt.width, _rawfmt.height
            );
    //if(_st_addr[0].fd){
     //   printf("get haddr %x\n",_st_addr[0].haddr);
    //    printf("get laddr %x\n",_st_addr[0].laddr);
    //}

    uint64_t uptr;
    uptr = _st_addr[0].haddr;
    uptr = uptr << 32;
    uptr = uptr | _st_addr[0].laddr;

    vbuf->buf_info[0].data_addr = (uint8_t *)uptr;
    vbuf->buf_info[0].data_length = _st_addr[0].size;

    if(_st_addr[1].size > 0){
        uptr = _st_addr[1].haddr;
        uptr = uptr << 32;
        uptr = uptr | _st_addr[1].laddr;

        vbuf->buf_info[1].data_addr = (uint8_t *)uptr;
        vbuf->buf_info[1].data_length = _st_addr[1].size;
    }
}

static int capture_raw(uint8_t *rkraw_data, uint32_t rkraw_len)
{
    rk_aiq_vbuf buf;
    struct _block_header _header;
    _header.block_id = NORMAL_RAW_TAG;
    uint16_t end_tag = END_TAG;
    //print_v4l2_info(vbuf, vfmt, state);

    memset(&buf, 0, sizeof(rk_aiq_vbuf));

    parse_rk_rawdata(rkraw_data, rkraw_len, &buf);

    char filname[32];
    sprintf(filname, "/tmp/raw%d", saved_num);
    FILE *fp = fopen(filname,"wb");
    if(fp){
        int ret = 0;
        _header.block_length = buf.buf_info[0].data_length;

        ret += fwrite(rkraw_data, 1, sizeof(struct _raw_format) + 2, fp);

        if(buf.buf_info[1].data_length > 0)
            _header.block_id = HDR_S_RAW_TAG;
        ret += fwrite(&_header, 1, sizeof(struct _block_header), fp);
        ret += fwrite((void *)buf.buf_info[0].data_addr, 1, buf.buf_info[0].data_length, fp);

        if(buf.buf_info[1].data_length > 0){
            _header.block_id = HDR_M_RAW_TAG;
            ret += fwrite(&_header, 1, sizeof(struct _block_header), fp);
            ret += fwrite((void *)buf.buf_info[0].data_addr, 1, buf.buf_info[0].data_length, fp);
        }
        ret += fwrite(&end_tag, 1, 2, fp);
        printf("%s:write file ret %d\n", __func__, ret);
        fclose(fp);
    }

    return 0;
}
static int on_frame_ready_capture(uint8_t *rkraw_data, uint32_t rkraw_len)
{
    debug_print_time("on_frame_ready_capture");
    if(g_quit == 0){

        capture_raw(rkraw_data, rkraw_len);
        saved_num++;
        if(saved_num == RAWCAP_SAVE_NUM)
            g_quit = 1;
    }
    return 0;
}

static int on_frame_ready_streaming(uint8_t *rkraw_data, uint32_t rkraw_len)
{
    //capture_raw(rkraw_data, rkraw_len);
    rkrawstream_vicap_buf_take(g_vi_ctx);
    rkrawstream_readback_set_buffer(g_vi_ctx, rkraw_data);

    saved_num++;
    if(saved_num == STREAM_SAVE_NUM)
        g_quit = 1;
    return 0;
}

static void on_isp_process_done_streaming(int dev_index)
{
    printf("on_isp_process_done_streaming\n");
    rkrawstream_vicap_buf_return(g_vi_ctx, dev_index);
}

static void on_isp_process_done_offline(int dev_index)
{
    printf("%s:index %d\n", __func__, dev_index);
}

void print_sensor_info(rk_aiq_static_info_t *s_info)
{
    int i = 0;
    printf("\n\n****** sensor %s infos: *********\n",s_info->sensor_info.sensor_name);
    for(i=0; i<10; i++){
        if(s_info->sensor_info.support_fmt[i].width >0)
         printf("format%d: %dx%d, format 0x%x, fps %d hdr %d\n",i,
            s_info->sensor_info.support_fmt[i].width,
            s_info->sensor_info.support_fmt[i].height,
            s_info->sensor_info.support_fmt[i].format,
            s_info->sensor_info.support_fmt[i].fps,
            s_info->sensor_info.support_fmt[i].hdr_mode);
    }
}

#include "rkvi_demo_isp.c_part"

int rawcap_test(void)
{
    strcpy(g_sns_entity_name, SENSOR_ENTNAME);
    printf("g_sns_entity_name:%s\n", g_sns_entity_name);
    sscanf(&g_sns_entity_name[6], "%s", g_sns_name);
    printf("sns_name:%s\n", g_sns_name);

    rkraw_vi_init_params_t init_p;
    rkraw_vi_prepare_params_t prepare_p;

    /* init rkraw_vi user api */
    g_vi_ctx = rkrawstream_uapi_init();
    init_p.sns_ent_name = g_sns_entity_name;
    init_p.use_offline = false;
    rkrawstream_vicap_init(g_vi_ctx, &init_p);

    prepare_p.width = VIDEO_WIDTH;
    prepare_p.height = VIDEO_HEIGHT;
    prepare_p.pix_fmt = V4L2_PIX_FMT_SGBRG10;
    prepare_p.hdr_mode = VIDEO_WORKMODE;
    rkrawstream_vicap_prepare(g_vi_ctx, &prepare_p);

    rkrawstream_vicap_start(g_vi_ctx, on_frame_ready_capture);

    while(g_quit == 0)
        usleep(10000);

    g_aiq_quit = 1;
    rkrawstream_vicap_stop(g_vi_ctx);
    /* clean up library. */
    rkrawstream_uapi_deinit(g_vi_ctx);
    return 0;
}


int stream_test(void)
{
    pthread_t isp_tid;
    strcpy(g_sns_entity_name, SENSOR_ENTNAME);
    printf("g_sns_entity_name:%s\n", g_sns_entity_name);
    sscanf(&g_sns_entity_name[6], "%s", g_sns_name);
    printf("sns_name:%s\n", g_sns_name);

    rkraw_vi_init_params_t init_p;
    rkraw_vi_prepare_params_t prepare_p;
    int param[2];

    g_ispfd = open(VIDEO_DEVNAME, O_RDWR /* required */ /*| O_NONBLOCK*/, 0);
    if (-1 == g_ispfd) {
        printf("Cannot open '%s'\n", VIDEO_DEVNAME);
        return 0;
    }

    /* init rkraw_vi user api */
    g_vi_ctx = rkrawstream_uapi_init();
    init_p.sns_ent_name = g_sns_entity_name;
    init_p.use_offline = false;
    rkrawstream_vicap_init(g_vi_ctx, &init_p);
    rkrawstream_readback_init(g_vi_ctx, &init_p);

    prepare_p.width = VIDEO_WIDTH;
    prepare_p.height = VIDEO_HEIGHT;
    prepare_p.pix_fmt = V4L2_PIX_FMT_SGBRG10;
    prepare_p.hdr_mode = VIDEO_WORKMODE;
    rkrawstream_vicap_prepare(g_vi_ctx, &prepare_p);
    rkrawstream_readback_prepare(g_vi_ctx, &prepare_p);

    g_aiq_ctx = rk_aiq_uapi2_sysctl_init(g_sns_entity_name, "/etc/iqfiles", NULL, NULL);
    rkrawstream_readback_set_aiqctx(g_vi_ctx, g_aiq_ctx);

    g_aiq_pause = 1;
    g_aiq_quit = 0;
    param[0] = VIDEO_WIDTH;
    param[1] = VIDEO_HEIGHT;
    pthread_create(&isp_tid, NULL, isp_thread, param);
    sleep(1);

    rk_aiq_uapi2_sysctl_prepare(g_aiq_ctx, VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_WORKMODE);
    rk_aiq_uapi2_sysctl_start(g_aiq_ctx);

    printf("rkisp_init_device %d %d\n", param[0], param[1]);
    rkisp_init_device(param[0], param[1]);
    rkisp_start_capturing();

    rkrawstream_vicap_start(g_vi_ctx, on_frame_ready_streaming);
    rkrawstream_readback_start(g_vi_ctx, on_isp_process_done_streaming);
    g_aiq_pause = 0;

    while(g_quit == 0)
        usleep(10000);

    g_aiq_quit = 1;
    rkrawstream_vicap_stop(g_vi_ctx);
    rkrawstream_readback_stop(g_vi_ctx);
    rk_aiq_uapi2_sysctl_stop(g_aiq_ctx, false);
    /* clean up library. */
    rkrawstream_uapi_deinit(g_vi_ctx);

    pthread_join(isp_tid, NULL);

    rkisp_stop_capturing();
    rkisp_uninit_device();
    rk_aiq_uapi2_sysctl_deinit(g_aiq_ctx);

    close(g_ispfd);
    return 0;
}

void release_buffer(void *addr) {
    printf("release buffer called: addr=%p\n", addr);
}

void parse_rk_rawdata_exp(uint8_t *rawdata)
{
    unsigned short tag = 0;
    struct _block_header header;
    uint8_t *p = (uint8_t *)rawdata;
    bool bExit = false;

    struct _st_addrinfo_stream _st_addr[3];
    struct _raw_format _rawfmt;

    while(!bExit){
        tag = *((unsigned short*)p);
        switch (tag)
        {
            case START_TAG:
                p = p+TAG_BYTE_LEN;
                memset(_st_addr, 0, sizeof(_st_addr));
                memset(&_rawfmt, 0, sizeof(_rawfmt));
                memset(&_finfo, 0, sizeof(_finfo));
                break;
            case NORMAL_RAW_TAG:
            {
                header = *((struct _block_header *)p);
                p = p + sizeof(struct _block_header);
                if (header.block_length == sizeof(struct _st_addrinfo_stream)) {
                    _st_addr[0] = *((struct _st_addrinfo_stream*)p);
                }
                p = p + header.block_length;

                break;
            }
            case HDR_S_RAW_TAG:
            {
                header = *((struct _block_header *)p);
                p = p + sizeof(struct _block_header);
                if (header.block_length == sizeof(struct _st_addrinfo_stream)) {
                    _st_addr[0] = *((struct _st_addrinfo_stream*)p);
                }
                p = p + header.block_length;
                break;
            }
            case HDR_M_RAW_TAG:
            {
                header = *((struct _block_header *)p);
                p = p + sizeof(struct _block_header);
                if (header.block_length == sizeof(struct _st_addrinfo_stream)) {
                    _st_addr[1] = *((struct _st_addrinfo_stream*)p);
                }
                p = p + header.block_length;
                break;
            }
            case HDR_L_RAW_TAG:
            {
                header = *((struct _block_header *)p);
                p = p + sizeof(struct _block_header);
                if (header.block_length == sizeof(struct _st_addrinfo_stream)) {
                    _st_addr[2] = *((struct _st_addrinfo_stream*)p);
                }
                p = p + header.block_length;
                break;
            }
            case FORMAT_TAG:
            {
                _rawfmt = *((struct _raw_format *)p);
                p = p + sizeof(struct _block_header) + _rawfmt.size;
                break;
            }
            case STATS_TAG:
            {
                _finfo = *((rk_aiq_frame_info_t *)p);
                p = p + sizeof(struct _block_header) + _finfo.size;
                break;
            }
            case ISP_REG_FMT_TAG:
            {
                header = *((struct _block_header *)p);
                p += sizeof(struct _block_header);
                p = p + header.block_length;
                break;
            }
            case ISP_REG_TAG:
            {
                header = *((struct _block_header *)p);
                p += sizeof(struct _block_header);
                p = p + header.block_length;
                break;
            }
            case ISPP_REG_FMT_TAG:
            {
                header = *((struct _block_header *)p);
                p += sizeof(struct _block_header);
                p = p + header.block_length;
                break;
            }
            case ISPP_REG_TAG:
            {
                header = *((struct _block_header *)p);
                p += sizeof(struct _block_header);
                p = p + header.block_length;
                break;
            }
            case PLATFORM_TAG:
            {
                header = *((struct _block_header *)p);
                p += sizeof(struct _block_header);
                p = p + header.block_length;
                break;
            }
            case END_TAG:
            {
                //uint32_t _len = p - (uint8_t *)rawdata;
                //printf("_len %d rkraw_len %d\n", _len, rkraw_len);
                bExit = true;
                break;
            }
            default:
            {
                printf("Not support TAG(0x%04x)\n", tag);
                bExit = true;
                break;
            }
        }
    }

    printf("frame_id %d, size %d %d %d, format %dx%d\n",
            _rawfmt.frame_id,
            _st_addr[0].size,_st_addr[1].size,_st_addr[2].size,
            _rawfmt.width, _rawfmt.height
            );
}

int offline_test(void)
{
    pthread_t isp_tid;

    if (use_isp_driver) {
        prepare_rkraw();
        g_rkraw_index = 0;
        parse_rk_rawdata_exp(g_rkraw_buffer);
        rk_aiq_frame_info_t offline_finfo[2];
        offline_finfo[0] = _finfo;
        offline_finfo[1] = _finfo;
        if (!rk_aiq_uapi2_sysctl_rawReproc_preInit(isp_vir, real_sns, offline_finfo)) {
            printf("%s is no exit\n", isp_vir);
            return -1;
        }
        strcpy(g_sns_entity_name, rk_aiq_uapi2_sysctl_getBindedSnsEntNmByVd(video_name));
    }else{
        strcpy(g_sns_entity_name, rk_aiq_uapi2_sysctl_getBindedSnsEntNmByVd(VIDEO_DEVNAME));
    }
    printf("g_sns_entity_name:%s\n", g_sns_entity_name);
    sscanf(&g_sns_entity_name[6], "%s", g_sns_name);
    printf("sns_name:%s\n", g_sns_name);

    rkraw_vi_init_params_t init_p;
    rkraw_vi_prepare_params_t prepare_p;
    int param[2];

    if (use_isp_driver) {
        g_ispfd = open(video_name, O_RDWR /* required */ /*| O_NONBLOCK*/, 0);
        if (-1 == g_ispfd) {
            printf("Cannot open '%s'\n", video_name);
            return 0;
        }
    } else {
        g_ispfd = open(VIDEO_DEVNAME, O_RDWR /* required */ /*| O_NONBLOCK*/, 0);
        if (-1 == g_ispfd) {
            printf("Cannot open '%s'\n", VIDEO_DEVNAME);
            return 0;
        }
    }

    /* init rkraw_vi user api */
    if (use_isp_driver){
        g_vi_ctx = rkrawstream_uapi_init(isp_vir, real_sns);
    } else {
        g_vi_ctx = rkrawstream_uapi_init();
    }
    init_p.sns_ent_name = g_sns_entity_name;
    init_p.use_offline = true;
    rkrawstream_readback_init(g_vi_ctx, &init_p);
    if (use_isp_driver) {
        rk_aiq_uapi2_sysctl_preInit(g_sns_entity_name, RK_AIQ_WORKING_MODE_NORMAL, iq_file_name);
    }
    g_aiq_ctx = rk_aiq_uapi2_sysctl_init(g_sns_entity_name, "/etc/iqfiles", NULL, NULL);
    rkrawstream_readback_set_aiqctx(g_vi_ctx, g_aiq_ctx);

    rk_aiq_raw_prop_t prop;
    if (pix_fmt == V4L2_PIX_FMT_SBGGR10)
        prop.format = RK_PIX_FMT_SBGGR10;
    else if (pix_fmt == V4L2_PIX_FMT_SGBRG10)
        prop.format = RK_PIX_FMT_SGBRG10;
    else if (pix_fmt == V4L2_PIX_FMT_SRGGB10)
        prop.format = RK_PIX_FMT_SRGGB10;
    else if (pix_fmt == V4L2_PIX_FMT_SGRBG10)
        prop.format = RK_PIX_FMT_SGRBG10;
    else if (pix_fmt == V4L2_PIX_FMT_SBGGR12)
        prop.format = RK_PIX_FMT_SBGGR12;
    else if (pix_fmt == V4L2_PIX_FMT_SGBRG12)
        prop.format = RK_PIX_FMT_SGBRG12;
    else if (pix_fmt == V4L2_PIX_FMT_SRGGB12)
        prop.format = RK_PIX_FMT_SRGGB12;
    else if (pix_fmt == V4L2_PIX_FMT_SGRBG12)
        prop.format = RK_PIX_FMT_SGRBG12;
    else if (pix_fmt == V4L2_PIX_FMT_SBGGR14)
        prop.format = RK_PIX_FMT_SBGGR14;
    else if (pix_fmt == V4L2_PIX_FMT_SGBRG14)
        prop.format = RK_PIX_FMT_SGBRG14;
    else if (pix_fmt == V4L2_PIX_FMT_SRGGB14)
        prop.format = RK_PIX_FMT_SRGGB14;
    else if (pix_fmt == V4L2_PIX_FMT_SGRBG14)
        prop.format = RK_PIX_FMT_SGRBG14;
    else
        prop.format = RK_PIX_FMT_SBGGR10;
    prop.frame_width = raw_width;
    prop.frame_height = raw_height;
    prop.rawbuf_type = RK_AIQ_RAW_FILE;
    rk_aiq_uapi2_sysctl_prepareRkRaw(g_aiq_ctx, prop);


    if(use_isp_driver) {
        prepare_p.width = raw_width;
        prepare_p.height = raw_height;
        prepare_p.pix_fmt = pix_fmt;
        prepare_p.hdr_mode = hdr_mode;
    } else {
        prepare_p.width = VIDEO_WIDTH;
        prepare_p.height = VIDEO_HEIGHT;
        prepare_p.pix_fmt = V4L2_PIX_FMT_SGBRG10;
        prepare_p.hdr_mode = VIDEO_WORKMODE;
    }
    rkrawstream_readback_prepare(g_vi_ctx, &prepare_p);

    g_aiq_pause = 1;
    g_aiq_quit = 0;
    param[0] = video_width;
    param[1] = video_height;
    pthread_create(&isp_tid, NULL, isp_thread, param);
    sleep(2);

    rk_aiq_uapi2_sysctl_prepare(g_aiq_ctx, video_width, video_height, VIDEO_WORKMODE);
    //rk_aiq_uapi2_sysctl_registRkRawCb(g_aiq_ctx, release_buffer);
    rk_aiq_uapi2_sysctl_start(g_aiq_ctx);

    printf("rkisp_init_device %d %d\n", param[0], param[1]);
    rkisp_init_device(param[0], param[1]);
    rkisp_start_capturing();

    rkrawstream_readback_start(g_vi_ctx, on_isp_process_done_offline);
    g_aiq_pause = 0;

    while(g_quit == 0){
        /* in offline mode, feed frames to isp.*/
        printf("\n");
        if(prepare_rkraw() < 0){
            break;
        }else{
            usleep(30000);
        }
        rkrawstream_readback_set_buffer(g_vi_ctx, g_rkraw_buffer);
        usleep(100000);
    }

    g_aiq_quit = 1;
    rkrawstream_readback_stop(g_vi_ctx);
    rk_aiq_uapi2_sysctl_stop(g_aiq_ctx, false);
    /* clean up library. */
    rkrawstream_uapi_deinit(g_vi_ctx);

    pthread_join(isp_tid, NULL);

    rkisp_stop_capturing();
    rkisp_uninit_device();
    rk_aiq_uapi2_sysctl_deinit(g_aiq_ctx);

    close(g_ispfd);
    return 0;
}

void print_help()
{
    printf("\n\n****** librkraw_vi demo by ydb *********\n\n");
    printf("--rawcap      capture rkraw file in /tmp.\n");
    printf("--stream      live stream to isp.\n");
    printf("--offaiq      run aiq and isp offline.\n");
    printf("you must choose one from rawcap, stream and offaiq, and you just can choose one of them\n");
    printf("\nif you choose --offaiq, there are some params you can config\n");
    printf("--width       video width, default value 640\n");
    printf("--height      video height, default value 480\n");
    printf("--rawfmt      raw width and height, config it such as 640x480, use 'x', don't use 'X'\n");
    printf("--iqfile      enter iqfile name, iq file can be found at /etc/iqfiles/\n");
    printf("--ispdriver   isp driver module name, such as rkisp0-vir0\n");
    printf("--video       mainpath video on the isp driver you choosed, such as /dev/video8\n");
    printf("--sensor      what sensor name of raw, such as imx415\n");
    printf("--pix         raw pix fmt, such as GB10(SGBRG10), default value BG10(SBGGR10)\n");
    printf("              all pix fmt BG10, GB10, BA10, RG10, BG12, GB12, RG12, BA12, BG14, GB14, RG14, BA14\n");
    printf("--har         optional, hdr mode, val 2 means hdrx2, 3 means hdrx3, default value 0(normal mode)");
    printf("--help        print help info\n");
}

static void parse_args(int argc, char **argv)
{
    int c;
    int digit_optind = 0;
    optind = 0;
    while (1) {
        int this_option_optind = optind ? optind : 1;
        int option_index = 0;
        static struct option long_options[] = {
            {"width",    required_argument, 0, 'w' },
            {"height",   required_argument, 0, 'h' },
            {"offaiq",   no_argument, 0, 'f' },
            {"video",    required_argument, 0, 'v' },
            {"stream",   no_argument, 0, 'o' },
            {"rawcap",   no_argument, 0, 'r' },
            {"help",     no_argument,       0, 'p' },
            {"iqfile",   required_argument, 0, 'i' },
            {"sensor",   required_argument, 0, 's' },
            {"ispdriver", required_argument, 0, 'd' },
            {"hdr",      required_argument, 0, 'a'},
            {"pix",      required_argument, 0, 'x'},
            {"rawfmt",   required_argument, 0, 't'},
            {0,          0,                 0,  0  }
        };

        //c = getopt_long(argc, argv, "w:h:f:i:d:o:c:ps",
        c = getopt_long(argc, argv, "w:h:v:d:i:s:t:orfp",
                        long_options, &option_index);
        if (c == -1)
            break;

        switch (c)
        {
        case 'w':
            video_width = atoi(optarg);
            break;
        case 'h':
            video_height = atoi(optarg);
            break;
        case 'f':
        {
            use_isp_driver = true;
            modul_select = 1;
        }
            break;
        case 'v':
            video_name = optarg;
            break;
        case 'd':
            isp_vir = optarg;
            break;
        case 'i':
            iq_file_name = optarg;
            break;
        case 's':
            real_sns = optarg;
            break;
        case 'o':
            modul_select = 2;
            break;
        case 'r':
            modul_select = 3;
            break;
        case 'p':
            print_help();
            break;
        case 'a':
        {
            int mode_val = atoi(optarg);
            if (mode_val == 2) {
                hdr_mode = RK_AIQ_WORKING_MODE_ISP_HDR2;
            }else if (mode_val == 3) {
                hdr_mode = RK_AIQ_WORKING_MODE_ISP_HDR3;
            } else {
                hdr_mode = RK_AIQ_WORKING_MODE_NORMAL;
            }

        }
            break;
        case 'x':
        {
            if (strcmp(optarg, "BG10") == 0)
                    pix_fmt = V4L2_PIX_FMT_SBGGR10;
                else if (strcmp(optarg, "GB10") == 0)
                    pix_fmt = V4L2_PIX_FMT_SGBRG10;
                else if (strcmp(optarg, "RG10") == 0)
                    pix_fmt = V4L2_PIX_FMT_SRGGB10;
                else if (strcmp(optarg, "BA10") == 0)
                    pix_fmt = V4L2_PIX_FMT_SGRBG10;
                else if (strcmp(optarg, "BG12") == 0)
                    pix_fmt = V4L2_PIX_FMT_SBGGR12;
                else if (strcmp(optarg, "GB12") == 0)
                    pix_fmt = V4L2_PIX_FMT_SGBRG12;
                else if (strcmp(optarg, "RG12") == 0)
                    pix_fmt = V4L2_PIX_FMT_SRGGB12;
                else if (strcmp(optarg, "BA12") == 0)
                    pix_fmt = V4L2_PIX_FMT_SGRBG12;
                else if (strcmp(optarg, "BG14") == 0)
                    pix_fmt = V4L2_PIX_FMT_SBGGR14;
                else if (strcmp(optarg, "GB14") == 0)
                    pix_fmt = V4L2_PIX_FMT_SGBRG14;
                else if (strcmp(optarg, "RG14") == 0)
                    pix_fmt = V4L2_PIX_FMT_SRGGB14;
                else if (strcmp(optarg, "BA14") == 0)
                    pix_fmt = V4L2_PIX_FMT_SGRBG14;
                else
                    pix_fmt = V4L2_PIX_FMT_SBGGR10;
        }
            break;
        case 't':
        {
            raw_width = atoi(optarg);
            char *raw_fmt = strstr(optarg, "x");
            raw_height = atoi(raw_fmt + 1);
        }
        default:
            break;
        }
    }
}

int main(int argc, char **argv)
{
    bool use_aiq = false;
    bool use_rawcap = false;
    bool use_rawstream = false;
    bool test_switch = false;
    bool use_offline = false;
    pthread_t isp_tid;
    g_aiq_test_mode = 0;

    //rkraw_vi_sensor_info_t m_info = {0};
    rk_aiq_static_info_t s_info;

    parse_args(argc, argv);
    if (!modul_select)
        return 0;
    switch (modul_select)
    {
    case 1:
        offline_test();
        break;
    case 2:
        stream_test();
        break;
    case 3:
        rawcap_test();
        break;
    default:
        break;
    }
    return 0;
}

