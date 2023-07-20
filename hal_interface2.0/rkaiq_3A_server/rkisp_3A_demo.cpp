#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h> /* getopt_long() */
#include <fcntl.h> /* low-level i/o */
#include <inttypes.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>

#include <mediactl.h>
#include <mediactl/mediactl-priv.h>
#include <linux/videodev2.h>
#include "rkisp_control_loop.h"
#include <utils/Log.h>
#include "rkisp_control_aiq.h"
#include <vector>
#include <pthread.h>
#include "tinyxml2.h"
#include <map>

#include <time.h>
#include <mutex>
#include <chrono>
#include <condition_variable> 
#include<sys/time.h>
#include "rk_aiq_user_api_sysctl.h"
#include "rk_aiq_user_api_imgproc.h"
//#include "rk_aiq_user_api_debug.h"
#include "AiqCameraHalAdapter.h"
//local
#include "rkisp_control_aiq.h"
#include "rkisp_control_loop.h"
#include "rk_aiq_user_api_sysctl.h"
#include "rkcamera_vendor_tags.h"


#include "rkaiq.h"
#include "AiqCameraHalAdapter.h"

#include "isp20/CamHwIsp20.h"

#define CLEAR(x) memset(&(x), 0, sizeof(x))
#define DBG(...) do { if(!silent) printf("DBG: " __VA_ARGS__);} while(0)
#define ERR(...) do { fprintf(stderr, "ERR: " __VA_ARGS__); } while (0)

/* Private v4l2 event */
#define CIFISP_V4L2_EVENT_STREAM_START  \
                    (V4L2_EVENT_PRIVATE_START + 1)
#define CIFISP_V4L2_EVENT_STREAM_STOP   \
                    (V4L2_EVENT_PRIVATE_START + 2)

#define FILE_PATH_LEN                       64
#define CAMS_NUM_MAX                        8
#define FLASH_NUM_MAX                       8


#define MAX_MEDIA_DEV_NUM  10

/* The SensorDriverDescriptor instance that describes sensor device and
 * connected sub-device informations.
 * @sensor_entity_name,
 * @sd_isp_path, the isp sub-device path, e.g. /dev/v4l-subdev0
 * @vd_params_path, the params video device path
 * @vd_stats_path, the stats video device path
 * @cams, multipe cameras can attache to isp, but only one can be active
 * @sd_sensor_path, the sensor sub-device path
 * @sd_lens_path, the lens sub-device path that attached to sensor
 * @sd_flash_path, the flash sub-device path that attached to sensor,
 *      can be two or more.
 * @link_enabled, the link status of this sensor
 */
struct SensorDriverDescriptor {
    /* sensor entity name format:
     * m01_b_ov13850 1-0010, where 'm01' means
     * module index number, 'b' means
     * back or front, 'ov13850' is real
     * sensor name, '1-0010' means the i2c bus
     * and sensor i2c slave address
     */
    int module_idx;
    char sensor_entity_name[FILE_PATH_LEN];
    char sd_sensor_path[FILE_PATH_LEN];
    char sd_lens_path[FILE_PATH_LEN];
    char sd_flash_path[FLASH_NUM_MAX][FILE_PATH_LEN];
    bool link_enabled;
    bool sensorLinkedToCIF;
    char linkedModelName[32];
    bool isSupportHdr;
    char iq_file_full_name[256];

    std::string mSensorName;
    std::string mDeviceName;
    std::string mParentMediaDev;
    std::string mModuleRealSensorName; //parsed frome sensor entity name
    std::string mModuleIndexStr; // parsed from sensor entity name
    char mPhyModuleOrient; // parsed from sensor entity name
};

std::vector<struct SensorDriverDescriptor> mSensorInfos;
std::map<uint32_t, const char*> mHdrModeConfigs;
static int threadId = 0;
static bool aiq_state = false;

/* The media topology instance that describes video device and
 * sub-device informations.
 *
 * @sd_isp_path, the isp sub-device path, e.g. /dev/v4l-subdev0
 * @vd_params_path, the params video device path
 * @vd_stats_path, the stats video device path
 * @cams, multipe cameras can attache to isp, but only one can be active
 * @sd_sensor_path, the sensor sub-device path
 * @sd_lens_path, the lens sub-device path that attached to sensor
 * @sd_flash_path, the flash sub-device path that attached to sensor,
 *      can be two or more.
 * @link_enabled, the link status of this sensor
 */
struct rkisp_media_info {
    char sd_isp_path[FILE_PATH_LEN];
    char vd_params_path[FILE_PATH_LEN];
    char vd_stats_path[FILE_PATH_LEN];
    SensorDriverDescriptor sensorInfo;
    const char *hdrmode;
    rk_aiq_working_mode_t work_mode;

    char mdev_path[32];
    int available;
    void* cl_ctx;
    pthread_t pid;
    pthread_mutex_t _cl_ctx_mutex;
};

static struct rkisp_media_info media_infos[CAMS_NUM_MAX];
static void* rkisp_engine;
static int sensor_index = -1;
static int silent = 0;
static const char *hdrmode = "NORMAL";
static int width = 2688;
static int height = 1520;
static const char *mdev_path = NULL;
static std::mutex mThreadListLock;
static std::condition_variable mThreadCond; 
static const int kWaitTimeoutMs = 500;   // 500ms
static const int kWaitTimesMax = 30;    // 33ms * 30 ~= 1 sec

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

static int
rkisp_get_devname(struct media_device *device, const char *name, char *dev_name)
{
    const char *devname;

    media_entity *entity =  NULL;

    entity = media_get_entity_by_name(device, name, strlen(name));
    if (!entity)
        return -1;

    devname = media_entity_get_devname(entity);
    if (!devname) {
        ERR("can't find %s device path!", name);
        return -1;
    }

    strncpy(dev_name, devname, FILE_PATH_LEN);

    DBG("get %s devname: %s\n", name, dev_name);

    return 0;
}

static int parseModuleInfo(const std::string &entity_name,
                                       SensorDriverDescriptor &drv_info)
{
    int ret = 0;

    // sensor entity name format SHOULD be like this:
    // m00_b_ov13850 1-0010
    if (entity_name.empty())
        return -1;

    int parse_index = 0;

    if (entity_name.at(parse_index) != 'm') {
        ERR("%d:parse sensor entity name %s error at %d, please check sensor driver !\n",
             __LINE__, entity_name.c_str(), parse_index);
        return -1;
    }

    std::string index_str = entity_name.substr (parse_index, 3);
    drv_info.mModuleIndexStr = index_str;

    parse_index += 3;

    if (entity_name.at(parse_index) != '_') {
        ERR("%d:parse sensor entity name %s error at %d, please check sensor driver !\n",
             __LINE__, entity_name.c_str(), parse_index);
        return -1;
    }

    parse_index++;

    if (entity_name.at(parse_index) != 'b' &&
        entity_name.at(parse_index) != 'f') {
        ERR("%d:parse sensor entity name %s error at %d, please check sensor driver !\n",
             __LINE__, entity_name.c_str(), parse_index);
        return -1;
    }
    drv_info.mPhyModuleOrient = entity_name.at(parse_index);

    parse_index++;

    if (entity_name.at(parse_index) != '_') {
        ERR("%d:parse sensor entity name %s error at %d, please check sensor driver !\n",
             __LINE__, entity_name.c_str(), parse_index);
        return -1;
    }

    parse_index++;

    std::size_t real_name_end = std::string::npos;
    if ((real_name_end = entity_name.find(' ')) == std::string::npos) {
        ERR("%d:parse sensor entity name %s error at %d, please check sensor driver !\n",
             __LINE__, entity_name.c_str(), parse_index);
        return -1;
    }

    std::string real_name_str = entity_name.substr(parse_index, real_name_end - parse_index);
    drv_info.mModuleRealSensorName = real_name_str;

    DBG("%s:%d, real sensor name %s, module ori %c, module id %s\n",
         __FUNCTION__, __LINE__, drv_info.mModuleRealSensorName.c_str(),
         drv_info.mPhyModuleOrient, drv_info.mModuleIndexStr.c_str());

    return ret;
}

static int
rkisp_enumrate_modules (struct media_device *device)
{
    uint32_t nents, i;
    const char* subdev_path = NULL;
    int active_sensor = -1;
    SensorDriverDescriptor drvInfo;
    int ret;
    const struct media_device_info *info = NULL;

    CLEAR(drvInfo);
    drvInfo.mSensorName = "none";
    drvInfo.link_enabled = false;
    nents = media_get_entities_count (device);
    for (i = 0; i < nents; ++i) {
        int module_idx = -1;
        struct media_entity *e;
        const struct media_entity_desc *ef;
        const struct media_link *link;

        e = media_get_entity(device, i);
        ef = media_entity_get_info(e);
        if (ef->type != MEDIA_ENT_T_V4L2_SUBDEV_SENSOR &&
            ef->type != MEDIA_ENT_T_V4L2_SUBDEV_FLASH &&
            ef->type != MEDIA_ENT_T_V4L2_SUBDEV_LENS)
            continue;

        if (ef->name[0] != 'm' && ef->name[3] != '_') {
            ERR("sensor/lens/flash entity name format is incorrect,"
                "pls check driver version !\n");
            return -1;
        }

        /* Retrive the sensor index from sensor name,
         * which is indicated by two characters after 'm',
         *   e.g.  m00_b_ov13850 1-0010
         *          ^^, 00 is the module index
         */
        module_idx = atoi(ef->name + 1);
        if (module_idx >= CAMS_NUM_MAX) {
            ERR("multiple sensors more than two not supported, %s\n", ef->name);
            continue;
        }
        subdev_path = media_entity_get_devname (e);
        switch (ef->type) {
        case MEDIA_ENT_T_V4L2_SUBDEV_SENSOR:
            drvInfo.module_idx = module_idx;
            drvInfo.mSensorName = ef->name;
            strncpy(drvInfo.sd_sensor_path,
                    subdev_path, FILE_PATH_LEN);
            strcpy(drvInfo.sensor_entity_name, ef->name);
            DBG("%s(%d) sensor_entity_name(%s)\n",
                __FUNCTION__, __LINE__, drvInfo.sensor_entity_name);
            ret = parseModuleInfo(drvInfo.mSensorName, drvInfo);

            link = media_entity_get_link(e, 0);
            if (link && (link->flags & MEDIA_LNK_FL_ENABLED)) {
                drvInfo.link_enabled = true;
            } else {
                DBG("%s(%d) sensor(%s) not linked!\n",
                    __FUNCTION__, __LINE__, drvInfo.mSensorName.c_str());
            }
            break;
        case MEDIA_ENT_T_V4L2_SUBDEV_FLASH:
            // TODO, support multiple flashes attached to one module
            strncpy(drvInfo.sd_flash_path[0],
                    subdev_path, FILE_PATH_LEN);
            break;
        case MEDIA_ENT_T_V4L2_SUBDEV_LENS:
            strncpy(drvInfo.sd_lens_path,
                    subdev_path, FILE_PATH_LEN);
            break;
        default:
            break;
        }
    }

    if (drvInfo.mSensorName != "none" && drvInfo.link_enabled) {
        info = media_get_info(device);
        if (info && !strncmp(info->driver, "rkcif", strlen("rkcif"))) {
            drvInfo.sensorLinkedToCIF = true;
        } else {
            drvInfo.sensorLinkedToCIF = false;
        }
        strncpy(drvInfo.linkedModelName, info->model, sizeof(info->model));
        DBG("module_idx(%d) sensor_entity_name(%s), linkedModelName(%s).\n",
             drvInfo.module_idx, drvInfo.sensor_entity_name, drvInfo.linkedModelName);

        mSensorInfos.push_back(drvInfo);
        //media_infos[drvInfo.module_idx].sensorInfo = drvInfo;
    } else {
        DBG("media path: %s, no camera sensor found!\n", device->devnode);
    }

    return 0;
}

static const bool compareFuncForSensorInfo(struct SensorDriverDescriptor  s1, struct SensorDriverDescriptor s2) {
    return (s1.mModuleIndexStr < s2.mModuleIndexStr);
}

#undef RK_3A_TUNING_FILE_PATH
#define RK_3A_TUNING_FILE_PATH "/vendor/etc/camera/rkisp2/"

static int rkisp_select_iqfile(const struct rkmodule_inf* mod_info, char* iq_file_full_name)
{

    char iq_file_name[128];

    if (!mod_info || !iq_file_full_name)
        return -1;

    CLEAR(iq_file_name);

    const struct rkmodule_base_inf* base_inf = &mod_info->base;
    const char *sensor_name, *module_name, *lens_name;
    char sensor_name_full[32];

    if (!strlen(base_inf->module) || !strlen(base_inf->sensor) ||
        !strlen(base_inf->lens)) {
        LOGE("no camera module fac info, check the drv !");
        return -1;
    }

    sensor_name = base_inf->sensor;
    strncpy(sensor_name_full, sensor_name, 32);
    module_name = base_inf->module;
    lens_name = base_inf->lens;

    sprintf(iq_file_name, "%s_%s_%s.xml",
            sensor_name_full, module_name, lens_name);
    strcpy(iq_file_full_name, RK_3A_TUNING_FILE_PATH);
    strcat(iq_file_full_name, iq_file_name);

    return 0;
}

static int querySensorInfo(SensorDriverDescriptor &drvInfo)
{
    int pad = 0;
    int sensorFd;
    struct v4l2_subdev_mbus_code_enum code_enum;
    struct v4l2_subdev_frame_interval_enum fintval_enum;
    struct rkmodule_inf mod_info;

    DBG("@%s: sensor_entity_name(%s)\n",
        __FUNCTION__, drvInfo.sensor_entity_name);

    CLEAR(mod_info);
    CLEAR(drvInfo.iq_file_full_name);
    CLEAR(code_enum);
    code_enum.pad = pad;
    code_enum.index = 0;

    drvInfo.isSupportHdr = false;
    sensorFd = open(drvInfo.sd_sensor_path, O_RDWR);
    if (sensorFd < 0) {
        ERR("open %s failed %s\n", drvInfo.sd_sensor_path, strerror(errno));
        return NULL;
    }

	code_enum.index = 0;
	if (xioctl(sensorFd, VIDIOC_SUBDEV_ENUM_MBUS_CODE, &code_enum) < 0){
		ERR("enum mbus code failed!");
		return -1;
	}

	memset(&fintval_enum, 0, sizeof(fintval_enum));
	fintval_enum.pad = 0;
	fintval_enum.index = 0;
	fintval_enum.code = code_enum.code;

    while (xioctl(sensorFd, VIDIOC_SUBDEV_ENUM_FRAME_INTERVAL, &fintval_enum) == 0) {
        DBG("@%s device: %s, fintval_enum.reserved[0](%u). \n", __FUNCTION__, drvInfo.sensor_entity_name, fintval_enum.reserved[0]);
        if (fintval_enum.reserved[0] != NO_HDR)
            drvInfo.isSupportHdr = true;
        fintval_enum.index++;
    };
    //drvInfo.isSupportHdr = false;
    if (xioctl(sensorFd, RKMODULE_GET_MODULE_INFO, &mod_info) < 0){
        ERR("failed to get camera module info!");
        return -1;
    }
    rkisp_select_iqfile(&mod_info, drvInfo.iq_file_full_name);
    DBG("@%s iq_file_full_name: %s.\n", __FUNCTION__, drvInfo.iq_file_full_name);

    close(sensorFd);
    DBG("@%s device: %s, isSupportHdr?(%s). \n", __FUNCTION__, drvInfo.sensor_entity_name, drvInfo.isSupportHdr?"true":"false");
    return NO_ERROR;
}

/*get sensor media info*/
int rkisp_get_sensor_info() {
    int ret = 0;
    unsigned int i, index = 0;
    char sys_path[64];
    int find_sensor = 0;
    int find_isp = 0;
    int linked_sensor = -1;
    struct media_device *device = NULL;

    while (index < MAX_MEDIA_DEV_NUM) {
        snprintf(sys_path, 64, "/dev/media%d", index++);
        DBG("media get sys_path: %s\n", sys_path);
        if(access(sys_path,F_OK))
          continue;

        device = media_device_new(sys_path);
        if (device == NULL) {
          ERR("Failed to create media %s\n", sys_path);
          continue;
        }

        ret = media_device_enumerate(device);
        if (ret < 0) {
          ERR("Failed to enumerate %s (%d)\n", sys_path, ret);
          media_device_unref(device);
          continue;
        }
        ret = rkisp_enumrate_modules(device);
        if (ret < 0) {
          ERR("Failed to enumerate modules%s (%d)\n", sys_path, ret);
          media_device_unref(device);
          continue;
        }
        media_device_unref(device);
    }

    std:sort(mSensorInfos.begin(), mSensorInfos.end(), compareFuncForSensorInfo);
    if (mSensorInfos.size() == 0) {
        // No registered drivers found
        ERR("ERROR no sensor driver registered in medias!\n");
        ret = -1;
    }

    DBG("%s:%d, find available camera num:%d!\n", __FUNCTION__, __LINE__, mSensorInfos.size());
    for (size_t i = 0; i < mSensorInfos.size(); i++) {
        querySensorInfo(mSensorInfos[i]);
        media_infos[i].sensorInfo = mSensorInfos[i];
        DBG("media_infos: module_idx(%d) sensor_entity_name(%s)\n",
             media_infos[i].sensorInfo.module_idx, media_infos[i].sensorInfo.sensor_entity_name);
        DBG("media_infos: sd_sensor_path(%s)\n", media_infos[i].sensorInfo.sd_sensor_path);
    }

    return ret;
}

/* Get sensor binded isp subdevs*/
static int
rkisp_enumrate_ispdev_info (struct media_device *device)
{
    uint32_t nents, i;
    const char* subdev_path = NULL;
    int active_sensor = -1;
    SensorDriverDescriptor drvInfo;
    int ret;
    const struct media_device_info *info = NULL;
    char Temp[FILE_PATH_LEN];

    CLEAR(drvInfo);
    drvInfo.mSensorName = "none";
    for (size_t i = 0; i < mSensorInfos.size(); i++) {
        drvInfo = media_infos[i].sensorInfo;
        DBG("@%s, Target sensor name: %s, candidate media: %s.\n",
            __FUNCTION__, drvInfo.mSensorName.c_str(), device->devnode);
        if (drvInfo.sensorLinkedToCIF) {
            ret = rkisp_get_devname(device, drvInfo.linkedModelName, Temp);
            if (ret < 0) {
                DBG("%s is direct linked to sensor, not linked to cif!\n", device->devnode);
                continue;
            }
            ret = rkisp_get_devname(device, "rkisp-isp-subdev", media_infos[i].sd_isp_path);
            ret |= rkisp_get_devname(device, "rkisp-input-params", media_infos[i].vd_params_path);
            ret |= rkisp_get_devname(device, "rkisp-statistics", media_infos[i].vd_stats_path);
            strncpy(media_infos[i].mdev_path, device->devnode, sizeof(media_infos[i].mdev_path));
        } else {
            ret = rkisp_get_devname(device, drvInfo.sensor_entity_name, Temp);
            if (ret < 0) {
                DBG("%s not direct linked to sensor!\n", device->devnode);
                continue;
            }
            ret = rkisp_get_devname(device, "rkisp-isp-subdev", media_infos[i].sd_isp_path);
            ret |= rkisp_get_devname(device, "rkisp-input-params", media_infos[i].vd_params_path);
            ret |= rkisp_get_devname(device, "rkisp-statistics", media_infos[i].vd_stats_path);
            strncpy(media_infos[i].mdev_path, device->devnode, sizeof(media_infos[i].mdev_path));
        }
    }

    return 0;
}

/*get sensor binded isp info*/
int rkisp_get_cif_linked_info() {
    int ret = 0;
    unsigned int i, index = 0;
    char sys_path[64];
    int find_sensor = 0;
    int find_isp = 0;
    int linked_sensor = -1;
    struct media_device *device = NULL;
    const struct media_device_info *info = NULL;
    char model[64] = "\0";

    while (index < MAX_MEDIA_DEV_NUM) {
        info = NULL;
        snprintf(sys_path, 64, "/dev/media%d", index++);
        DBG("media get sys_path: %s\n", sys_path);
        if(access(sys_path,F_OK))
          continue;

        device = media_device_new(sys_path);
        if (device == NULL) {
          ERR("Failed to create media %s\n", sys_path);
          continue;
        }

        ret = media_device_enumerate(device);
        if (ret < 0) {
          ERR("Failed to enumerate %s (%d)\n", sys_path, ret);
          media_device_unref(device);
          continue;
        }
        info = media_get_info(device);
        if (info && !strncmp(info->driver, "rkcif", strlen("rkcif"))) {
            DBG("media: %s is cif node, skip!\n", sys_path);
            media_device_unref(device);
            continue;
        }

        ret = rkisp_enumrate_ispdev_info(device);
        if (ret < 0) {
          ERR("Failed to enumerate ispdev info:%s (%d)\n", sys_path, ret);
          media_device_unref(device);
          continue;
        }
        media_device_unref(device);
    }

    return ret;
}

static void init_engine(struct rkisp_media_info *media_info)
{
    int ret = 0;

    if (!media_info->sensorInfo.link_enabled) {
        DBG("Link disabled, skipped\n");
    }

    ret = rkisp_cl_rkaiq_init(&(media_info->cl_ctx), NULL, NULL, media_info->sensorInfo.sensor_entity_name);
    if (ret) {
        ERR("rkisp engine init failed !\n");
        exit(-1);
    }
    setMulCamConc(media_info->cl_ctx,true);

}

static void prepare_engine(struct rkisp_media_info *media_info)
{
    struct rkisp_cl_prepare_params_s params = {0};
    int ret = 0;

    params.isp_sd_node_path = media_info->sd_isp_path;
    params.isp_vd_params_path = media_info->vd_params_path;
    params.isp_vd_stats_path = media_info->vd_stats_path;
    params.staticMeta = NULL;
    params.width = width;
    params.height = height;
    params.work_mode = media_info->hdrmode;
    DBG("%s--set workingmode(%s)\n", __FUNCTION__, params.work_mode);

    if (!media_info->sensorInfo.link_enabled) {
        DBG("Link disabled, skipped\n");
    }

    DBG("%s - %s: link enabled : %d\n", media_info->sensorInfo.sd_sensor_path,
        media_info->sensorInfo.sensor_entity_name,
        media_info->sensorInfo.link_enabled);
    if (strlen(media_info->sensorInfo.sd_lens_path))
        params.lens_sd_node_path = media_info->sensorInfo.sd_lens_path;
    if (strlen(media_info->sensorInfo.sd_flash_path[0]))
        params.flashlight_sd_node_path[0] = media_info->sensorInfo.sd_flash_path[0];

    params.sensor_sd_node_path = media_info->sensorInfo.sd_sensor_path;

    if (rkisp_cl_prepare(media_info->cl_ctx, &params)) {
        ERR("rkisp engine prepare failed !\n");
        exit(-1);
    }

}

static void start_engine(struct rkisp_media_info *media_info)
{
    DBG("rkaiq start\n");
    rkisp_cl_start(media_info->cl_ctx);
    if (media_info->cl_ctx == NULL) {
        ERR("rkaiq_start engine failed\n");
        exit(-1);
    } else {
        DBG("rkaiq_start engine succeed\n");
    }
}

static void stop_engine(struct rkisp_media_info *media_info)
{
    rkisp_cl_stop(media_info->cl_ctx);
}

static void deinit_engine(struct rkisp_media_info *media_info)
{
    rkisp_cl_deinit(media_info->cl_ctx);
}

// blocked func
static int wait_stream_event(int fd, unsigned int event_type, int time_out_ms)
{
    int ret;
    struct v4l2_event event;

    CLEAR(event);

    do {
	/*
	 * xioctl instead of poll.
	 * Since poll() cannot wait for input before stream on,
	 * it will return an error directly. So, use ioctl to
	 * dequeue event and block until sucess.
	 */
	ret = xioctl(fd, VIDIOC_DQEVENT, &event);
	if (ret == 0 && event.type == event_type) {
		return 0;
	}
    } while (true);

    return -1;

}

static int subscrible_stream_event(struct rkisp_media_info *media_info, int fd, bool subs)
{
    struct v4l2_event_subscription sub;
    int ret = 0;

    DBG("subscribe events from %s !\n", media_info->vd_params_path);
    CLEAR(sub);
    sub.type = CIFISP_V4L2_EVENT_STREAM_START;
    ret = xioctl(fd,
                 subs ? VIDIOC_SUBSCRIBE_EVENT : VIDIOC_UNSUBSCRIBE_EVENT,
                 &sub);
    if (ret) {
        ERR("can't subscribe %s start event!\n", media_info->vd_params_path);
        exit(EXIT_FAILURE);
    }

    CLEAR(sub);
    sub.type = CIFISP_V4L2_EVENT_STREAM_STOP;
    ret = xioctl(fd,
                 subs ? VIDIOC_SUBSCRIBE_EVENT : VIDIOC_UNSUBSCRIBE_EVENT,
                 &sub);
    if (ret) {
        ERR("can't subscribe %s stop event!\n", media_info->vd_params_path);
        exit(EXIT_FAILURE);
    }

    DBG("subscribe events from %s success !\n", media_info->vd_params_path);

    return 0;
}

void parse_args(int argc, char **argv)
{
   int c;
   int digit_optind = 0;

   while (1) {
       int this_option_optind = optind ? optind : 1;
       int option_index = 0;
       static struct option long_options[] = {
           {"silent",    no_argument,       0, 's' },
           {"help",      no_argument,       0, '?' },
           {"hdrmode",   optional_argument, 0, 'r' },
           {0,           0,                 0,  0  }
       };

       c = getopt_long(argc, argv, "m::shr:", long_options, &option_index);
       if (c == -1)
           break;

       switch (c) {
           case 'd':
               sensor_index = atoi(optarg);
               break;
           case 'm':
               mdev_path = optarg;
               break;
           case 'w':
               width = atoi(optarg);
               break;
           case 'h':
               height = atoi(optarg);
               break;
           case 's':
               silent = 1;
               break;
           case 'r':
               hdrmode = optarg;
               break;
           case '?':
               ERR("Usage: %s to start 3A engine\n"
                   "         --silent,        optional, subpress debug log\n",
                   "         --hdrmode,       optional, NORMAL/HDR2/HDR3 \n",
                   argv[0]);
               exit(-1);

           default:
               ERR("?? getopt returned character code %c ??\n", c);
       }
   }
}

#define AIQ_CONTEXT_CAST(context)  ((AiqCameraHalAdapter*)(context))

static void set_af_manual_meascfg(const rk_aiq_sys_ctx_t* ctx)
{
    rk_aiq_af_attrib_t attr;
    uint16_t gamma_y[RKAIQ_RAWAF_GAMMA_NUM] =
             {0, 45, 108, 179, 245, 344, 409, 459, 500, 567, 622, 676, 759, 833, 896, 962, 1023};

    rk_aiq_user_api_af_GetAttrib(ctx, &attr);
    attr.AfMode = RKAIQ_AF_MODE_FIXED;

    attr.manual_meascfg.contrast_af_en = 1;
    attr.manual_meascfg.rawaf_sel = 0; // normal = 0; hdr = 1

    attr.manual_meascfg.window_num = 2;
    attr.manual_meascfg.wina_h_offs = 2;
    attr.manual_meascfg.wina_v_offs = 2;
    attr.manual_meascfg.wina_h_size = 1920;
    attr.manual_meascfg.wina_v_size = 1080;

    attr.manual_meascfg.winb_h_offs = 500;
    attr.manual_meascfg.winb_v_offs = 600;
    attr.manual_meascfg.winb_h_size = 300;
    attr.manual_meascfg.winb_v_size = 300;

    attr.manual_meascfg.gamma_flt_en = 1;
    attr.manual_meascfg.gamma_y[RKAIQ_RAWAF_GAMMA_NUM];
    memcpy(attr.manual_meascfg.gamma_y, gamma_y, RKAIQ_RAWAF_GAMMA_NUM * sizeof(uint16_t));

    attr.manual_meascfg.gaus_flt_en = 1;
    attr.manual_meascfg.gaus_h0 = 0x20;
    attr.manual_meascfg.gaus_h1 = 0x10;
    attr.manual_meascfg.gaus_h2 = 0x08;

    attr.manual_meascfg.afm_thres = 4;

    attr.manual_meascfg.lum_var_shift[0] = 0;
    attr.manual_meascfg.afm_var_shift[0] = 0;
    attr.manual_meascfg.lum_var_shift[1] = 4;
    attr.manual_meascfg.afm_var_shift[1] = 4;
    rk_aiq_user_api_af_SetAttrib(ctx, &attr);
}

static void print_af_stats(rk_aiq_isp_stats_t *stats_ref)
{
    unsigned long sof_time;

    sof_time = stats_ref->af_stats.sof_tim / 1000000LL;
    printf("sof_tim %ld, sharpness roia: 0x%llx-0x%08x roib: 0x%x-0x%08x\n",
           sof_time,
           stats_ref->af_stats.roia_sharpness,
           stats_ref->af_stats.roia_luminance,
           stats_ref->af_stats.roib_sharpness,
           stats_ref->af_stats.roib_luminance);
#if 0
    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 15; j++) {
            printf("0x%08x, ", stats_ref->af_stats.global_sharpness[15 * i + j]);
        }
        printf("\n");
    }
#endif
}

static void print_ae_stats(rk_aiq_isp_stats_t *stats_ref)
{

	RkAiqAecHwStatsRes_t *aec_hw_stats = &(stats_ref->aec_stats.ae_data);
	rawaebig_stat_t *ae_chan_stat = &(aec_hw_stats->chn[0].rawae_big);
	unsigned int i = 0;
	unsigned long long sum_r = 0;
	unsigned long long sum_g = 0;
	unsigned long long sum_b = 0;
	unsigned long long sum_y = 0;
	unsigned long long sum_all = 0;

	for(i = 0; i < RAWAEBIG_WIN_NUM ; i++) {
	sum_r += ae_chan_stat->channelr_xy[i];
	sum_g += ae_chan_stat->channelg_xy[i];
	sum_b += ae_chan_stat->channelb_xy[i];
	sum_y += ae_chan_stat->channely_xy[i];
	}

	printf("ae [%llu %llu %llu %llu ] \n", sum_r / RAWAEBIG_WIN_NUM, sum_g / RAWAEBIG_WIN_NUM,
	sum_b / RAWAEBIG_WIN_NUM, sum_y / RAWAEBIG_WIN_NUM);
}

static const float exp_table[] = {
	0.001,
#if 0
	0.005,
	0.010,
	0.015,
	0.020,
	0.025,
	0.030,
	0.033,
	0.030,
	0.025,
	0.020,
	0.015,
	0.010,
	0.005,
#endif
	0.033,
};

static XCamReturn set_ae_manual_meascfg(const rk_aiq_sys_ctx_t* ctx)
{
	XCamReturn ret;
	int i;
	static bool test = false;

	if (test) {
		ret = rk_aiq_uapi_setManualExp(ctx, 2.0, 0.001);
		printf("set manual exposure param: gain:2.0, exposure time: 0.001ms.\n");
	}
	else {
		ret = rk_aiq_uapi_setManualExp(ctx, 2.0, 0.033);
		printf("set manual exposure param: gain:2.0, exposure time: 0.033ms.\n");
	}
	test = !test;
	sleep(3);
	return ret;
}

/* stats thread */
static void* stats_thread(void* args) {
    struct rkisp_media_info *media_info;
    XCamReturn ret;
    char name[128] = {0};

    pthread_detach (pthread_self());
    printf("begin stats thread\n");
    media_info = (struct rkisp_media_info *) args;
    sprintf(name, "stats_thread%d", threadId);
    pthread_setname_np(pthread_self(), name);
    threadId++;

    AiqCameraHalAdapter * gAiqCameraHalAdapter = AIQ_CONTEXT_CAST (media_info->cl_ctx);
    rk_aiq_sys_ctx_t *aiq_ctx = gAiqCameraHalAdapter->get_aiq_ctx();

#if 1
    //set_af_manual_meascfg(aiq_ctx);
    while(1) {
        rk_aiq_isp_stats_t *stats_ref = NULL;

        ret = rk_aiq_uapi_sysctl_get3AStatsBlk(aiq_ctx, &stats_ref, -1);
        if (ret == XCAM_RETURN_NO_ERROR && stats_ref != NULL) {
            printf("get one stats frame id %d \n", stats_ref->frame_id);
            //printf("aec pixel_clock_freq_mhz %f.\n", stats_ref->aec_stats.ae_exp.pixel_clock_freq_mhz);
            //print_af_stats(stats_ref);
            print_ae_stats(stats_ref);
            rk_aiq_uapi_sysctl_release3AStatsRef(aiq_ctx, stats_ref);
			aiq_state = true;

        } else {
            if (ret == XCAM_RETURN_NO_ERROR) {
                printf("aiq has stopped !\n");
                break;
            } else if (ret == XCAM_RETURN_ERROR_TIMEOUT) {
                printf("aiq timeout!\n");
                continue;
            } else if (ret == XCAM_RETURN_ERROR_FAILED) {
                printf("aiq failed!\n");
                break;
            }
			aiq_state = false;
        }
    }
#endif
    printf("end stats thread\n");
    return 0;
}


/* munual ae thread */
static void* manualAe_thread(void* args) {
    struct rkisp_media_info *media_info;
    XCamReturn ret;
    char name[128] = {0};

    pthread_detach (pthread_self());
    printf("begin munual ae thread\n");
    media_info = (struct rkisp_media_info *) args;
    sprintf(name, "manualAe_thread%d", threadId);
    pthread_setname_np(pthread_self(), name);
    threadId++;

    AiqCameraHalAdapter * gAiqCameraHalAdapter = AIQ_CONTEXT_CAST (media_info->cl_ctx);
    rk_aiq_sys_ctx_t *aiq_ctx = gAiqCameraHalAdapter->get_aiq_ctx();

    //set_af_manual_meascfg(aiq_ctx);
    while(1) {
        rk_aiq_isp_stats_t *stats_ref = NULL;

        if (aiq_state) {
            ret = set_ae_manual_meascfg(aiq_ctx);
            if (ret == XCAM_RETURN_NO_ERROR) {
                printf("set gain & exposure.\n");
            }
        }
    }
    printf("end manual thread\n");
    return 0;
}

//#define TEST_BLOCKED_STATS_FUNC
//#define TEST_BLOCKED_MUNUAL_AE

static void getSensorCurMode(struct rkisp_media_info *media_info)
{
    if (strcmp(media_info->hdrmode, "NORMAL") == 0) {
        media_info->work_mode = RK_AIQ_WORKING_MODE_NORMAL;
    } else if (strcmp(media_info->hdrmode, "HDR2") == 0) {
        media_info->work_mode = RK_AIQ_WORKING_MODE_ISP_HDR2;
    } else if (strcmp(media_info->hdrmode, "HDR3") == 0) {
        media_info->work_mode = RK_AIQ_WORKING_MODE_ISP_HDR3;
    } else {
        //default
        media_info->work_mode = RK_AIQ_WORKING_MODE_NORMAL;
    }
}

/* engine thread */
static void *engine_thread(void *arg)
{
    int ret = 0;
    int isp_fd;
    unsigned int stream_event = -1;
    struct rkisp_media_info *media_info;
    pthread_t tid;
    std::unique_lock<std::mutex> lk(mThreadListLock);
    char name[128] = {0};

    DBG("%s current thread id:%lu \n", __FUNCTION__, pthread_self());
    sprintf(name, "engine_thread%d", threadId);
    pthread_setname_np(pthread_self(), name);
    threadId++;

    media_info = (struct rkisp_media_info *) arg;

    pthread_mutex_lock(&(media_info->_cl_ctx_mutex));

    for (;;) {
        isp_fd = open(media_info->vd_params_path, O_RDWR);
        if (isp_fd < 0) {
            ERR("open %s failed %s\n", media_info->vd_params_path, strerror(errno));
            return NULL;
        }
        subscrible_stream_event(media_info, isp_fd, true);
        //getSensorCurMode(media_info);
        init_engine(media_info);
        /* notify init ok*/
        lk.unlock();
        mThreadCond.notify_one();
        DBG("%s: init engine success...\n", media_info->mdev_path);

        prepare_engine(media_info);
        DBG("%s: prepare engine success...\n", media_info->mdev_path);

        start_engine(media_info);
        DBG("%s: wait stream start event...\n", media_info->mdev_path);
        wait_stream_event(isp_fd, CIFISP_V4L2_EVENT_STREAM_START, -1);
        DBG("%s: wait stream start event success ...\n", media_info->mdev_path);

        DBG("%s: wait stream stop event...\n", media_info->mdev_path);

#ifdef TEST_BLOCKED_STATS_FUNC
        pthread_t stats_tid;
        pthread_create(&stats_tid, NULL, stats_thread, media_info);
#endif

#ifdef TEST_BLOCKED_MUNUAL_AE
		pthread_t manual_ae;
		pthread_create(&manual_ae, NULL, manualAe_thread, media_info);
#endif

        wait_stream_event(isp_fd, CIFISP_V4L2_EVENT_STREAM_STOP, -1);
        DBG("%s: wait stream stop event success ...\n", media_info->mdev_path);
        stop_engine(media_info);
        if (!strncmp(media_info->hdrmode, "NORMAL", sizeof("NORMAL"))) {
            if (media_info->sensorInfo.isSupportHdr) {
                DBG("current mode is normal, switch to hdr.\n");
                media_info->hdrmode = "HDR2";
            } else {
                DBG("sensor not support hdr mode, keep normal.\n");
            }
        } else {
            DBG("current mode is hdr, switch to normal.\n");
            media_info->hdrmode = "NORMAL";
        }
        lk.lock();
        deinit_engine(media_info);
        subscrible_stream_event(media_info, isp_fd, false);
        close(isp_fd);
    }

    pthread_mutex_unlock(&(media_info->_cl_ctx_mutex));
    return NULL;
}

const char* kDefaultCfgPath = "/vendor/etc/multi_camera_config.xml";

bool updateModeList(tinyxml2::XMLElement* modeList,
        std::map<uint32_t, const char*>& modeConfigs) {
    using namespace tinyxml2;

    DBG("@%s enter!\n", __FUNCTION__);
    XMLElement* row = modeList->FirstChildElement("CameraId");
    while (row != nullptr) {

        uint32_t moduleId;
        const char * mode = nullptr;
        moduleId = row->UnsignedAttribute("moduleId"),
        mode = row->Attribute("hdrmode");
        //DBG("update mode: %s\n", mode);
        if (mode == nullptr) {
            ERR("%s: mode Config list must config mode!", __FUNCTION__);
            return false;
        }
        modeConfigs.insert(std::make_pair(moduleId, mode));
        row = row->NextSiblingElement("CameraId");
    }
    return true;
}

static int loadFromCfg(const char* cfgPath) {
    using namespace tinyxml2;
    XMLDocument configXml;
    int errorID;

    errorID = configXml.LoadFile(cfgPath);
    if (errorID != XML_SUCCESS) {
        ERR("%s: Unable to load aiq camera config file: %s. ErrorID: %d\n",
                __FUNCTION__, cfgPath, errorID);
        return errorID;
    } else {
        DBG("%s: load aiq camera config succeed!\n", __FUNCTION__);
    }

    XMLElement *aiqCamConfig = configXml.FirstChildElement("AiqCameraConfig");
    if (aiqCamConfig == nullptr) {
        DBG("%s: no aiq camera config specified\n", __FUNCTION__);
        return -1;
    }
    XMLElement *modelist = aiqCamConfig->FirstChildElement("modeConfigList");
    if (modelist == nullptr) {
        DBG("%s: no mode list specified\n", __FUNCTION__);
    } else {
        if (!updateModeList(modelist, mHdrModeConfigs)) {
            return -1;
        }
    }

    return 0;
}

static int waitForNextInit()
{
    std::unique_lock<std::mutex> lk(mThreadListLock);
    std::chrono::milliseconds timeout = std::chrono::milliseconds(kWaitTimeoutMs);
    int waitTimes = 0;
    struct timeval old, now;
    time_t oldLt, nowlt;
    int ret = 0;

    oldLt =time(NULL);  //sec
    gettimeofday(&old, NULL);

    auto st = mThreadCond.wait_for(lk, timeout);
    if (st == std::cv_status::timeout) {
        DBG("---------wait aiq init timeout-------\n\n");
        ret = -1;
    }

    nowlt =time(NULL);  //sec
    gettimeofday(&now, NULL);
    DBG("wait for aiq init time: %ld ms\n", (nowlt - oldLt) * 1000 + (now.tv_usec / 1000 - old.tv_usec / 1000));
    return ret;

}

int main(int argc, char **argv)
{
    int i, ret = 0;
    int isp_fd;
    unsigned int stream_event = -1;
    int threads = 0;

    /* Line buffered so that printf can flash every line if redirected to
     * no-interactive device.
     */
    setlinebuf(stdout);
    parse_args(argc, argv);

    DBG("----------------------------------------------\n\n");
    CLEAR(media_infos);
    mSensorInfos.clear();
    mHdrModeConfigs.clear();

    /* get physical sensor infos*/
    if (rkisp_get_sensor_info())
        errno_exit("Bad media topology\n");

    /* get sensor binded isp infos*/
    if (rkisp_get_cif_linked_info())
        errno_exit("Bad media infos.\n");

    /* sensor mode config & mutex init*/
    loadFromCfg(kDefaultCfgPath);
    if (mHdrModeConfigs.empty()) {
        ERR("Using default hdr configs!\n");
        for (i = 0; i < mSensorInfos.size(); i++) {
            media_infos[i]._cl_ctx_mutex = PTHREAD_MUTEX_INITIALIZER;
            media_infos[i].hdrmode = hdrmode;
        }
    } else {
        for (i = 0; i < mSensorInfos.size(); i++) {
            media_infos[i]._cl_ctx_mutex = PTHREAD_MUTEX_INITIALIZER;
            media_infos[i].hdrmode = mHdrModeConfigs[i];
            DBG("camera:%d, hdrmode: %s.\n", i, media_infos[i].hdrmode);
        }
    }
    for (i = 0; i < mSensorInfos.size(); i++) {
        DBG("-------------pthread_create start------------------\n\n");
        ret = pthread_create(&media_infos[i].pid, NULL, engine_thread, &media_infos[i]);
        if (ret) {
            media_infos[i].pid = 0;
            ERR("Failed to create camera engine thread for: %s\n", media_infos[i].mdev_path);
            errno_exit("Create thread failed");
        }
        /*wait for every aiq ctx init finished*/
        waitForNextInit();
        DBG("-------------pthread_create success------------------\n\n");
    }

    DBG("-------------pthread_join start------------------\n\n");
    if (media_infos[mSensorInfos.size() -1].pid == 0)
        DBG("-----media_infos[mSensorInfos.size() -1].pid--is 0---\n\n");
    pthread_join(media_infos[mSensorInfos.size()-1].pid, NULL);
    DBG("-------------pthread_join success------------------\n\n");

    DBG("----------------------exit----------------------\n\n");
    return 0;
}
