/*
 *  Copyright (c) 2019 Rockchip Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef _SENSOR_HW_BASE_H_
#define _SENSOR_HW_BASE_H_

#include <map>
#include <list>
#include "v4l2_device.h"
#include "rk_aiq_pool.h"

/* test hdr function */
/*
* struct hdrae_exp_s - hdrae exposure
*
*/
struct hdrae_exp_s {
    unsigned int long_exp_reg;
    unsigned int long_gain_reg;
    unsigned int middle_exp_reg;
    unsigned int middle_gain_reg;
    unsigned int short_exp_reg;
    unsigned int short_gain_reg;
    unsigned int long_exp_val;
    unsigned int long_gain_val;
    unsigned int middle_exp_val;
    unsigned int middle_gain_val;
    unsigned int short_exp_val;
    unsigned int short_gain_val;
};

/**
 * struct rkmodule_base_inf - module base information
 *
 */
 #define RKMODULE_NAME_LEN   32
struct rkmodule_base_inf {
    char sensor[RKMODULE_NAME_LEN];
    char module[RKMODULE_NAME_LEN];
    char lens[RKMODULE_NAME_LEN];
} __attribute__ ((packed));
#if 0
/**
 * struct rkmodule_fac_inf - module factory information
 *
 */
struct rkmodule_fac_inf {
    unsigned int flag;

    char module[RKMODULE_NAME_LEN];
    char lens[RKMODULE_NAME_LEN];
    unsigned int year;
    unsigned int month;
    unsigned int day;
} __attribute__ ((packed));

/**
 * struct rkmodule_awb_inf - module awb information
 *
 */
struct rkmodule_awb_inf {
    unsigned int flag;

    unsigned int r_value;
    unsigned int b_value;
    unsigned int gr_value;
    unsigned int gb_value;

    unsigned int golden_r_value;
    unsigned int golden_b_value;
    unsigned int golden_gr_value;
    unsigned int golden_gb_value;
} __attribute__ ((packed));

/**
 * struct rkmodule_lsc_inf - module lsc information
 *
 */
struct rkmodule_lsc_inf {
    unsigned int flag;

    unsigned short lsc_w;
    unsigned short lsc_h;
    unsigned short decimal_bits;

    unsigned short lsc_r[RKMODULE_LSCDATA_LEN];
    unsigned short lsc_b[RKMODULE_LSCDATA_LEN];
    unsigned short lsc_gr[RKMODULE_LSCDATA_LEN];
    unsigned short lsc_gb[RKMODULE_LSCDATA_LEN];
} __attribute__ ((packed));

/**
 * struct rkmodule_af_inf - module af information
 *
 */
struct rkmodule_af_inf {
    unsigned int flag;

    unsigned int vcm_start;
    unsigned int vcm_end;
    unsigned int vcm_dir;
} __attribute__ ((packed));
#endif
/**
 * struct rkmodule_inf - module information
 *
 */
struct rkmodule_inf {
    struct rkmodule_base_inf base;
    //struct rkmodule_fac_inf fac;
    //struct rkmodule_awb_inf awb;
    //struct rkmodule_lsc_inf lsc;
    //struct rkmodule_af_inf af;
} __attribute__ ((packed));

#define SENSOR_CMD_SET_HDRAE_EXP        \
	_IOW('V', BASE_VIDIOC_PRIVATE + 0, struct hdrae_exp_s)
#define RKMODULE_GET_MODULE_INFO    \
    _IOR('V', BASE_VIDIOC_PRIVATE + 1, struct rkmodule_inf)

using namespace XCam;

namespace RkCam {

class SensorHw : public V4l2SubDevice {
public:
    explicit SensorHw(const char* name);
    virtual ~SensorHw();

    XCamReturn setExposureParams(SmartPtr<RkAiqExpParamsProxy>& expPar);
    XCamReturn getSensorModeData(const char* sns_ent_name,
                                 rk_aiq_exposure_sensor_descriptor& sns_des);

    XCamReturn handle_sof(int64_t time, int frameid);
    int get_pixel(rk_aiq_exposure_sensor_descriptor* sns_des);
    int get_blank(rk_aiq_exposure_sensor_descriptor* sns_des);
    int get_exposure_range(rk_aiq_exposure_sensor_descriptor* sns_des);
    int get_format(rk_aiq_exposure_sensor_descriptor* sns_des);

    XCamReturn get_sensor_descriptor (rk_aiq_exposure_sensor_descriptor* sns_des);
    XCamReturn get_sensor_mode_data (struct isp_supplemental_sensor_mode_data &sensor_mode_data,
                                     int frame_id = -1);
    XCamReturn getEffectiveParams(SmartPtr<RkAiqExpParamsProxy>& ExpParams, int frame_id);

private:
    XCAM_DEAD_COPY (SensorHw);
    Mutex _mutex;
    const int _exp_delay = 3;
    std::list<SmartPtr<RkAiqExpParamsProxy>> _exp_list;
    std::map<int, SmartPtr<RkAiqExpParamsProxy>> _effecting_exp_map;
    bool _first;
    int _frame_sequence;
    rk_aiq_exposure_sensor_descriptor _sensor_desc;

    int get_sensor_fps(float& fps);
    XCamReturn setLinearSensorExposure(SmartPtr<RkAiqExpParamsProxy>& expPar);
    XCamReturn setHdrSensorExposure(SmartPtr<RkAiqExpParamsProxy>& expPar);
    XCamReturn setExposure(int frameid);
    uint32_t get_v4l2_pixelformat(uint32_t pixelcode);
};

}; //namespace RkCam

#endif
