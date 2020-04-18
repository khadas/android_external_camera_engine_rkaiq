/*
 * V4L2 video capture example
 * AUTHOT : Jacob Chen
 * DATA : 2018-02-25
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h> /* getopt_long() */
#include <fcntl.h> /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <dlfcn.h>
#include <signal.h>
#include <linux/videodev2.h>

#include "drmDsp.h"
#include "rk_aiq_user_api_sysctl.h"

#define CLEAR(x) memset(&(x), 0, sizeof(x))
#define FMT_NUM_PLANES 1

#define BUFFER_COUNT 4

struct buffer {
        void *start;
        size_t length;
        int export_fd;
	int sequence;
};

static char out_file[255];
static char dev_name[255];
static int width = 640;
static int height = 480;
static int format = V4L2_PIX_FMT_NV12;
static int fd = -1;
static enum v4l2_buf_type buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
struct buffer *buffers;
static unsigned int n_buffers;
static int frame_count = -1;
FILE *fp = NULL;
static rk_aiq_sys_ctx_t* aiq_ctx = NULL;
static int silent = 0;
static int vop = 0;
static int rkaiq = 0;
static int writeFile = 0;
static int pponeframe = 0;
static int hdrmode = 0;
static char sensor[64];

static int fd_pp_input = -1;
static int fd_isp_mp = -1;
struct buffer *buffers_mp;
static int outputCnt = 3;
static int skipCnt = 30;

//TODO: get active sensor from driver

#define DBG(...) do { if(!silent) printf(__VA_ARGS__); } while(0)
#define ERR(...) do { fprintf(stderr, __VA_ARGS__); } while (0)

static void errno_exit(const char *s)
{
        ERR("%s error %d, %s\n", s, errno, strerror(errno));
        exit(EXIT_FAILURE);
}

static int xioctl(int fh, int request, void *arg)
{
        int r;
        do {
                r = ioctl(fh, request, arg);
        } while (-1 == r && EINTR == errno);
        return r;
}

static void process_image(const void *p, int sequence,int size)
{
	if (fp && sequence > skipCnt && outputCnt-- > 0) {
	    printf(">\n");
	    fwrite(p, size, 1, fp);
	    fflush(fp);
	}
}

static int read_frame()
{
        struct v4l2_buffer buf;
        int i, bytesused;

        CLEAR(buf);

        buf.type = buf_type;
        buf.memory = V4L2_MEMORY_MMAP;

        struct v4l2_plane planes[FMT_NUM_PLANES];
        if (V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE == buf_type) {
            buf.m.planes = planes;
            buf.length = FMT_NUM_PLANES;
        }

        if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf))
                errno_exit("VIDIOC_DQBUF");

        i = buf.index;

        if (V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE == buf_type)
            bytesused = buf.m.planes[0].bytesused;
        else
            bytesused = buf.bytesused;

	if (vop) {
	    drmDspFrame(width, height, buffers[i].start, DRM_FORMAT_NV12);
	}

	process_image(buffers[i].start,  buf.sequence, bytesused);

        if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
            errno_exit("VIDIOC_QBUF");

        return 1;
}

static int read_frame_pp_oneframe()
{
        struct v4l2_buffer buf;
        struct v4l2_buffer buf_pp;
        int i,ii, bytesused;
        static int first_time = 1;

        CLEAR(buf);
        // dq one buf from isp mp
        printf("------ dq 1 from isp mp --------------\n");
        buf.type = buf_type;
        buf.memory = V4L2_MEMORY_MMAP;

        struct v4l2_plane planes[FMT_NUM_PLANES];
        if (V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE == buf_type) {
            buf.m.planes = planes;
            buf.length = FMT_NUM_PLANES;
        }

        if (-1 == xioctl(fd_isp_mp, VIDIOC_DQBUF, &buf))
                errno_exit("VIDIOC_DQBUF");

        i = buf.index;

        if (first_time ) {
            printf("------ dq 2 from isp mp --------------\n");
            if (-1 == xioctl(fd_isp_mp, VIDIOC_DQBUF, &buf))
                    errno_exit("VIDIOC_DQBUF");

            ii = buf.index;
        }

        if (V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE == buf_type)
            bytesused = buf.m.planes[0].bytesused;
        else
            bytesused = buf.bytesused;

        // queue to ispp input 
        printf("------ queue 1 index %d to ispp input --------------\n", i);
        CLEAR(buf_pp);
        buf_pp.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
        buf_pp.memory = V4L2_MEMORY_DMABUF;
        buf_pp.index = i;

        struct v4l2_plane planes_pp[FMT_NUM_PLANES];
        memset(planes_pp, 0, sizeof(planes_pp));
        buf_pp.m.planes = planes_pp;
        buf_pp.length = FMT_NUM_PLANES;
        buf_pp.m.planes[0].m.fd = buffers_mp[i].export_fd;

        if (-1 == xioctl(fd_pp_input, VIDIOC_QBUF, &buf_pp))
                errno_exit("VIDIOC_QBUF");

        if (first_time ) {
            printf("------ queue 2 index %d to ispp input --------------\n", ii);
            buf_pp.index = ii;
            buf_pp.m.planes[0].m.fd = buffers_mp[ii].export_fd;
            if (-1 == xioctl(fd_pp_input, VIDIOC_QBUF, &buf_pp))
                    errno_exit("VIDIOC_QBUF");
        }
        // read frame from ispp sharp/scale
        printf("------ readframe from output --------------\n");
        read_frame();
        // dq from pp input
        printf("------ dq 1 from ispp input--------------\n");
        if (-1 == xioctl(fd_pp_input, VIDIOC_DQBUF, &buf_pp))
                errno_exit("VIDIOC_DQBUF");
        // queue back to mp
        printf("------ queue 1 index %d back to isp mp--------------\n", buf_pp.index);
        buf.index = buf_pp.index;
        if (-1 == xioctl(fd_isp_mp, VIDIOC_QBUF, &buf))
            errno_exit("VIDIOC_QBUF");

        first_time = 0;
        return 1;
}

static void mainloop(void)
{
    bool loop_inf = frame_count == -1 ? true : false;

    while (loop_inf || (frame_count-- > 0)) {
        if (pponeframe)
            read_frame_pp_oneframe();
        else
            read_frame();
    }
}

static void stop_capturing(void)
{
        enum v4l2_buf_type type;

        type = buf_type;
        if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type))
            errno_exit("VIDIOC_STREAMOFF");
}

static void stop_capturing_pp_oneframe(void)
{
        enum v4l2_buf_type type;

        type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
        if (-1 == xioctl(fd_pp_input, VIDIOC_STREAMOFF, &type))
            errno_exit("VIDIOC_STREAMOFF ppinput");
        type = buf_type;
        if (-1 == xioctl(fd_isp_mp, VIDIOC_STREAMOFF, &type))
            errno_exit("VIDIOC_STREAMOFF ispmp");
}

static void start_capturing(void)
{
        unsigned int i;
        enum v4l2_buf_type type;

        for (i = 0; i < n_buffers; ++i) {
                struct v4l2_buffer buf;

                CLEAR(buf);
                buf.type = buf_type;
                buf.memory = V4L2_MEMORY_MMAP;
                buf.index = i;

                if (V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE == buf_type) {
                    struct v4l2_plane planes[FMT_NUM_PLANES];

                    buf.m.planes = planes;
                    buf.length = FMT_NUM_PLANES;
                }
                if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
                        errno_exit("VIDIOC_QBUF");
        }
        type = buf_type;
        printf("-------- stream on output -------------\n");
        if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
                errno_exit("VIDIOC_STREAMON");
}

static void start_capturing_pp_oneframe(void)
{
        unsigned int i;
        enum v4l2_buf_type type;

        type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
        printf("-------- stream on pp input -------------\n");
        if (-1 == xioctl(fd_pp_input, VIDIOC_STREAMON, &type))
                errno_exit("VIDIOC_STREAMON pp input");

        type = buf_type;
        for (i = 0; i < n_buffers; ++i) {
                struct v4l2_buffer buf;

                CLEAR(buf);
                buf.type = buf_type;
                buf.memory = V4L2_MEMORY_MMAP;
                buf.index = i;

                if (V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE == buf_type) {
                    struct v4l2_plane planes[FMT_NUM_PLANES];

                    buf.m.planes = planes;
                    buf.length = FMT_NUM_PLANES;
                }
                if (-1 == xioctl(fd_isp_mp, VIDIOC_QBUF, &buf))
                        errno_exit("VIDIOC_QBUF");
        }
        printf("-------- stream on isp mp -------------\n");
        if (-1 == xioctl(fd_isp_mp, VIDIOC_STREAMON, &type))
                errno_exit("VIDIOC_STREAMON ispmp");
}


static void uninit_device(void)
{
        unsigned int i;

        for (i = 0; i < n_buffers; ++i) {
            if (-1 == munmap(buffers[i].start, buffers[i].length))
                    errno_exit("munmap");

            close(buffers[i].export_fd);
        }

        free(buffers);
}

static void uninit_device_pp_oneframe(void)
{
        unsigned int i;

        for (i = 0; i < n_buffers; ++i) {
            if (-1 == munmap(buffers_mp[i].start, buffers_mp[i].length))
                    errno_exit("munmap");

            close(buffers_mp[i].export_fd);
        }

        free(buffers_mp);
}

static void init_mmap(int pp_onframe)
{
        struct v4l2_requestbuffers req;
        int fd_tmp = -1;

        CLEAR(req);

        if (pp_onframe)
            fd_tmp = fd_isp_mp ;
        else
            fd_tmp = fd;

        req.count = BUFFER_COUNT;
        req.type = buf_type;
        req.memory = V4L2_MEMORY_MMAP;

        struct buffer *tmp_buffers = NULL;

        if (-1 == xioctl(fd_tmp, VIDIOC_REQBUFS, &req)) {
                if (EINVAL == errno) {
                        ERR("%s does not support "
                                 "memory mapping\n", dev_name);
                        exit(EXIT_FAILURE);
                } else {
                        errno_exit("VIDIOC_REQBUFS");
                }
        }

        if (req.count < 2) {
                ERR("Insufficient buffer memory on %s\n",
                         dev_name);
                exit(EXIT_FAILURE);
        }

        tmp_buffers = (struct buffer*)calloc(req.count, sizeof(struct buffer));

        if (!tmp_buffers) {
                ERR("Out of memory\n");
                exit(EXIT_FAILURE);
        }

        if (pp_onframe)
            buffers_mp = tmp_buffers;
        else
            buffers = tmp_buffers;

        for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
                struct v4l2_buffer buf;
                struct v4l2_plane planes[FMT_NUM_PLANES];
                CLEAR(buf);
                CLEAR(planes);

                buf.type = buf_type;
                buf.memory = V4L2_MEMORY_MMAP;
                buf.index = n_buffers;

                if (V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE == buf_type) {
                    buf.m.planes = planes;
                    buf.length = FMT_NUM_PLANES;
                }

                if (-1 == xioctl(fd_tmp, VIDIOC_QUERYBUF, &buf))
                        errno_exit("VIDIOC_QUERYBUF");

                if (V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE == buf_type) {
                    tmp_buffers[n_buffers].length = buf.m.planes[0].length;
                    tmp_buffers[n_buffers].start =
                        mmap(NULL /* start anywhere */,
                              buf.m.planes[0].length,
                              PROT_READ | PROT_WRITE /* required */,
                              MAP_SHARED /* recommended */,
                              fd_tmp, buf.m.planes[0].m.mem_offset);
                } else {
                    tmp_buffers[n_buffers].length = buf.length;
                    tmp_buffers[n_buffers].start =
                        mmap(NULL /* start anywhere */,
                              buf.length,
                              PROT_READ | PROT_WRITE /* required */,
                              MAP_SHARED /* recommended */,
                              fd_tmp, buf.m.offset);
                }

                if (MAP_FAILED == tmp_buffers[n_buffers].start)
                        errno_exit("mmap");

                // export buf dma fd
                struct v4l2_exportbuffer expbuf;
                xcam_mem_clear (expbuf);
                expbuf.type = buf_type;
                expbuf.index = n_buffers;
                expbuf.flags = O_CLOEXEC;
                if (xioctl(fd_tmp, VIDIOC_EXPBUF, &expbuf) < 0) {
                    errno_exit("get dma buf failed\n");
                } else {
                    printf("get dma buf(%d)-fd: %d\n", n_buffers, expbuf.fd);
                }
                tmp_buffers[n_buffers].export_fd = expbuf.fd;
        }
}

static void init_input_dmabuf_oneframe(void) {
        struct v4l2_requestbuffers req;

        CLEAR(req);

        printf("-------- request pp input buffer -------------\n");
        req.count = BUFFER_COUNT;
        req.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
        req.memory = V4L2_MEMORY_DMABUF;

        if (-1 == xioctl(fd_pp_input, VIDIOC_REQBUFS, &req)) {
                if (EINVAL == errno) {
                        ERR("does not support "
                                 "DMABUF\n");
                        exit(EXIT_FAILURE);
                } else {
                        errno_exit("VIDIOC_REQBUFS");
                }
        }

        if (req.count < 2) {
                ERR("Insufficient buffer memory on %s\n",
                         dev_name);
                exit(EXIT_FAILURE);
        }
    printf("-------- request isp mp buffer -------------\n");
    init_mmap(true);
}

static void init_device(void)
{
        struct v4l2_capability cap;
        struct v4l2_format fmt;

        if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) {
                if (EINVAL == errno) {
                        ERR("%s is no V4L2 device\n",
                                 dev_name);
                        exit(EXIT_FAILURE);
                } else {
                        errno_exit("VIDIOC_QUERYCAP");
                }
        }

        if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) &&
                !(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE_MPLANE)) {
            ERR("%s is not a video capture device, capabilities: %x\n",
                         dev_name, cap.capabilities);
                exit(EXIT_FAILURE);
        }

        if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
                ERR("%s does not support streaming i/o\n",
                    dev_name);
                exit(EXIT_FAILURE);
        }

        if (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)
            buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        else if (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE_MPLANE)
            buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;

        CLEAR(fmt);
        fmt.type = buf_type;
        fmt.fmt.pix.width = width;
        fmt.fmt.pix.height = height;
        fmt.fmt.pix.pixelformat = format;
        fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

        if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt))
                errno_exit("VIDIOC_S_FMT");

	init_mmap(false);
}

static void init_device_pp_oneframe(void)
{
    // TODO, set format and link, now do with setup_link.sh    
    init_input_dmabuf_oneframe();
}

static void close_device(void)
{
        if (-1 == close(fd))
                errno_exit("close");

        fd = -1;
}

static void open_device(void)
{
        printf("-------- open output dev_name -------------\n");
        fd = open(dev_name, O_RDWR /* required */ /*| O_NONBLOCK*/, 0);

        if (-1 == fd) {
                ERR("Cannot open '%s': %d, %s\n",
                         dev_name, errno, strerror(errno));
                exit(EXIT_FAILURE);
        }
}

static void close_device_pp_oneframe(void)
{
        if (-1 == close(fd_pp_input))
                errno_exit("close");

        fd_pp_input = -1;

        if (-1 == close(fd_isp_mp))
                errno_exit("close");

        fd_isp_mp = -1;
}

static void open_device_pp_oneframe(void)
{
        printf("-------- open pp input(video13) -------------\n");
        fd_pp_input = open("/dev/video13", O_RDWR /* required */ /*| O_NONBLOCK*/, 0);

        if (-1 == fd_pp_input) {
                ERR("Cannot open '%s': %d, %s\n",
                         dev_name, errno, strerror(errno));
                exit(EXIT_FAILURE);
        }

        printf("-------- open isp mp(video0) -------------\n");
        fd_isp_mp = open("/dev/video0", O_RDWR /* required */ /*| O_NONBLOCK*/, 0);

        if (-1 == fd_isp_mp ) {
                ERR("Cannot open '%s': %d, %s\n",
                         dev_name, errno, strerror(errno));
                exit(EXIT_FAILURE);
        }
}

static void uninit_device_pp_onframe(void)
{
        unsigned int i;

        for (i = 0; i < n_buffers; ++i) {
                if (-1 == munmap(buffers_mp[i].start, buffers_mp[i].length))
                        errno_exit("munmap");
                close(buffers_mp[i].export_fd);
        }

        free(buffers_mp);
}

void parse_args(int argc, char **argv)
{
   int c;
   int digit_optind = 0;

   while (1) {
       int this_option_optind = optind ? optind : 1;
       int option_index = 0;
       static struct option long_options[] = {
           {"width",    required_argument, 0, 'w' },
           {"height",   required_argument, 0, 'h' },
           {"format",   required_argument, 0, 'f' },
           {"device",   required_argument, 0, 'd' },
           {"stream-to",   required_argument, 0, 'o' },
           {"stream-count",   required_argument, 0, 'n' },
           {"stream-skip",   required_argument, 0, 'k' },
           {"count",    required_argument, 0, 'c' },
           {"help",     no_argument,       0, 'p' },
           {"silent",   no_argument,       0, 's' },
           {"vop",   no_argument,       0, 'v' },
           {"rkaiq",   no_argument,       0, 'r' },
           {"pponeframe",   no_argument,       0, 'm' },
           {"hdr",   no_argument,       0, 'a' },
           {"sensor",   required_argument,       0, 'b' },
           {0,          0,                 0,  0  }
       };

       //c = getopt_long(argc, argv, "w:h:f:i:d:o:c:ps",
       c = getopt_long(argc, argv, "w:h:f:i:d:o:c:n:k:m:ps",
           long_options, &option_index);
       if (c == -1)
           break;

       switch (c) {
       case 'c':
           frame_count = atoi(optarg);
           break;
       case 'w':
           width = atoi(optarg);
           break;
       case 'h':
           height = atoi(optarg);
           break;
       case 'f':
           format = v4l2_fourcc(optarg[0], optarg[1], optarg[2], optarg[3]);
           break;
       case 'd':
           strcpy(dev_name, optarg);
           break;
       case 'o':
           strcpy(out_file, optarg);
           writeFile = 1;
	   break;
       case 'n':
           outputCnt = atoi(optarg);
	   break;
       case 'k':
           skipCnt = atoi(optarg);
	   break;
       case 's':
           silent = 1;
           break;
       case 'v':
           vop = 1;
           break;
       case 'r':
           rkaiq = 1;
           break;
       case 'm':
           pponeframe = 1;
           break;
       case 'a':
           hdrmode = 1;
           break;
       case 'b':
           strcpy(sensor, optarg);
           break;
       case '?':
       case 'p':
           ERR("Usage: %s to capture rkisp1 frames\n"
                  "         --width,  default 640,             optional, width of image\n"
                  "         --height, default 480,             optional, height of image\n"
                  "         --format, default NV12,            optional, fourcc of format\n"
                  "         --count,  default 1000,            optional, how many frames to capture\n"
                  "         --device,                          required, path of video device\n"
                  "         --stream-to,                       optional, output file path, if <file> is '-', then the data is written to stdout\n"
                  "         --stream-count, default 3		   optional, how many frames to write files\n"
                  "         --stream-skip, default 30		   optional, how many frames to skip befor writing file\n"
                  "         --vop,                             optional, drm display\n"
                  "         --rkaiq,                           optional, auto image quality\n",
                  "         --silent,                          optional, subpress debug log\n",
                  "         --pponeframe,                      optional, pp oneframe readback mode\n",
                  "         --hdr,                             optional, hdr mode\n",
                  "         --sensor,  default os04a10,        optional, sensor names\n",
                  argv[0]);
           exit(-1);

       default:
           ERR("?? getopt returned character code 0%o ??\n", c);
       }
   }

   if (strlen(dev_name) == 0) {
        ERR("arguments --output and --device are required\n");
        exit(-1);
   }

}

static void deinit() 
{
    stop_capturing();
    if (pponeframe)
        stop_capturing_pp_oneframe();
	if (aiq_ctx) {
        printf("-------- stop aiq -------------\n");
		rk_aiq_uapi_sysctl_stop(aiq_ctx);
        printf("-------- deinit aiq -------------\n");
		rk_aiq_uapi_sysctl_deinit(aiq_ctx);
        printf("-------- deinit aiq end -------------\n");
	}

    //stop_capturing();
    uninit_device();
    if (pponeframe)
        uninit_device_pp_oneframe();
    close_device();
    if (pponeframe)
        close_device_pp_oneframe();

    if (fp)
        fclose(fp);
}
static void signal_handle(int signo)
{
    printf("force exit signo %d !!!\n",signo);
    deinit();
    exit(0);
}

int main(int argc, char **argv)
{
    signal(SIGINT, signal_handle);
    signal(SIGQUIT, signal_handle);
    signal(SIGTERM, signal_handle);

    strcpy(sensor, "os04a10");
    parse_args(argc, argv);

    printf("-------- open output dev -------------\n");
    open_device();
    if (pponeframe)
        open_device_pp_oneframe();
    init_device();
    if (pponeframe)
        init_device_pp_oneframe();
    if (writeFile) {
        fp = fopen(out_file, "w+");
        if (fp == NULL) {
            ERR("fopen output file %s failed!\n", out_file);
            exit(-1);
        }
    }

    const char* sns_entity_name = NULL;
    rk_aiq_working_mode_t work_mode = RK_AIQ_WORKING_MODE_NORMAL;

    if (strcasecmp(sensor, "ov4689") == 0) {
        if (hdrmode)
            work_mode = RK_AIQ_WORKING_MODE_ISP_HDR3;
        sns_entity_name = "m01_f_ov4689 1-0036";
    } else if (strcasecmp(sensor, "os04a10") == 0) {
        if (hdrmode)
            work_mode = RK_AIQ_WORKING_MODE_ISP_HDR2;
        sns_entity_name = "m01_f_os04a10 1-0036";
    } else if (strcasecmp(sensor, "gc4c33") == 0) {
        if (hdrmode)
            work_mode = RK_AIQ_WORKING_MODE_ISP_HDR2;
        sns_entity_name = "m01_f_gc4c33 1-0029";
    } else if (strcasecmp(sensor, "imx347") == 0) {
        if (hdrmode)
            work_mode = RK_AIQ_WORKING_MODE_ISP_HDR2;
        sns_entity_name = "m01_f_imx347 1-0037";
    }

    printf("sns_entity_name %s\n", sns_entity_name);
	if (rkaiq) {
		aiq_ctx = rk_aiq_uapi_sysctl_init(sns_entity_name, NULL, NULL, NULL);

		if (aiq_ctx) {
            printf("-------- init mipi tx/rx -------------\n");
			XCamReturn ret = rk_aiq_uapi_sysctl_prepare(aiq_ctx, width, height, work_mode);
			if (ret != XCAM_RETURN_NO_ERROR)
				ERR("rk_aiq_uapi_sysctl_prepare failed: %d\n", ret);
			else {
                /* printf("-------- stream on mipi tx/rx -------------\n"); */
				/* ret = rk_aiq_uapi_sysctl_start(aiq_ctx ); */
				start_capturing();
                if (pponeframe)
                    start_capturing_pp_oneframe();
                printf("-------- stream on mipi tx/rx -------------\n");
				ret = rk_aiq_uapi_sysctl_start(aiq_ctx );
			}

			if (vop) {
				if (initDrmDsp() < 0) {
					printf("DRM display init failed\n");
					exit(0);
				}
			}

			usleep(500 * 1000);
		}
	} else {
		if (vop) {
			if (initDrmDsp() < 0) {
				printf("DRM display init failed\n");
				exit(0);
			}
		}

		usleep(500 * 1000);

		start_capturing();
        if (pponeframe)
            start_capturing_pp_oneframe();
	}

    mainloop();

    deinit();

    return 0;
}
