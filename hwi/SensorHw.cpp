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

#include <linux/v4l2-subdev.h>
#include "SensorHw.h"

namespace RkCam {

SensorHw::SensorHw(const char* name)
    : V4l2SubDevice (name)
    , _first(true)
    , _working_mode(RK_AIQ_WORKING_MODE_NORMAL)
{
    ENTER_CAMHW_FUNCTION();
    _last_exp_time = nullptr;
    _last_exp_gain = nullptr;
    _gain_delay = 0;
    _time_delay = 0;
    _gain_delayed = false;
    _frame_sequence = 0;
    _expParamsPool = new RkAiqExpParamsPool("SensorLocalExpParams", 20);
    EXIT_CAMHW_FUNCTION();
}

SensorHw::~SensorHw()
{
    ENTER_CAMHW_FUNCTION();
    EXIT_CAMHW_FUNCTION();
}

XCamReturn
SensorHw::setHdrSensorExposure(RKAiqAecExpInfo_t* expPar)
{
    ENTER_CAMHW_FUNCTION();
    struct hdrae_exp_s hdrExp;

    LOGD_CAMHW("%s: lexp: 0x%x-0x%x, mexp: 0x%x-0x%x, sexp: 0x%x-0x%x\n",
               __FUNCTION__,
               expPar->HdrExp[2].exp_sensor_params.analog_gain_code_global,
               expPar->HdrExp[2].exp_sensor_params.coarse_integration_time,
               expPar->HdrExp[1].exp_sensor_params.analog_gain_code_global,
               expPar->HdrExp[1].exp_sensor_params.coarse_integration_time,
               expPar->HdrExp[0].exp_sensor_params.analog_gain_code_global,
               expPar->HdrExp[0].exp_sensor_params.coarse_integration_time);

    memset(&hdrExp, 0, sizeof(hdrExp));
    hdrExp.long_exp_reg =
        expPar->HdrExp[2].exp_sensor_params.coarse_integration_time;
    hdrExp.long_gain_reg =
        expPar->HdrExp[2].exp_sensor_params.analog_gain_code_global;
    hdrExp.middle_exp_reg =
        expPar->HdrExp[1].exp_sensor_params.coarse_integration_time;
    hdrExp.middle_gain_reg =
        expPar->HdrExp[1].exp_sensor_params.analog_gain_code_global;
    hdrExp.short_exp_reg =
        expPar->HdrExp[0].exp_sensor_params.coarse_integration_time;
    hdrExp.short_gain_reg =
        expPar->HdrExp[0].exp_sensor_params.analog_gain_code_global;

    if (io_control(SENSOR_CMD_SET_HDRAE_EXP, &hdrExp) < 0) {
        LOGE_CAMHW ("failed to set hdrExp exp");
        return XCAM_RETURN_ERROR_IOCTL;
    }

    EXIT_CAMHW_FUNCTION();
    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
SensorHw::setLinearSensorExposure(RKAiqAecExpInfo_t* expPar)
{
    ENTER_CAMHW_FUNCTION();
    int frame_line_length;
    struct v4l2_control ctrl;
    rk_aiq_exposure_sensor_descriptor sensor_desc;

    LOGD_CAMHW("%s: frameid: %d, a-gain: %d, time: %d\n",
               __FUNCTION__,
               _frame_sequence,
               expPar->LinearExp.exp_sensor_params.analog_gain_code_global,
               expPar->LinearExp.exp_sensor_params.coarse_integration_time);

    // set vts before exposure time firstly
    get_sensor_descriptor (&sensor_desc);

    frame_line_length = sensor_desc.line_periods_per_field;
    memset(&ctrl, 0, sizeof(ctrl));
    ctrl.id = V4L2_CID_VBLANK;
    ctrl.value = frame_line_length - sensor_desc.sensor_output_height;
    if (io_control(VIDIOC_S_CTRL, &ctrl) < 0) {
        LOGE_CAMHW("failed to set vblank result(val: %d)", ctrl.value);
        return XCAM_RETURN_ERROR_IOCTL;
    }

    if (expPar->LinearExp.exp_sensor_params.analog_gain_code_global >= 0) {
        memset(&ctrl, 0, sizeof(ctrl));
        ctrl.id = V4L2_CID_ANALOGUE_GAIN;
        ctrl.value = expPar->LinearExp.exp_sensor_params.analog_gain_code_global;
        if (io_control(VIDIOC_S_CTRL, &ctrl) < 0) {
            LOGE_CAMHW ("failed to  set again result(val: %d)", ctrl.value);
            return XCAM_RETURN_ERROR_IOCTL;
        }
    }

    if (expPar->LinearExp.exp_sensor_params.digital_gain_global != 0) {
        memset(&ctrl, 0, sizeof(ctrl));
        ctrl.id = V4L2_CID_GAIN;
        ctrl.value = expPar->LinearExp.exp_sensor_params.digital_gain_global;
        if (io_control(VIDIOC_S_CTRL, &ctrl) < 0) {
            LOGE_CAMHW ("failed to set dgain result(val: %d)", ctrl.value);
            return XCAM_RETURN_ERROR_IOCTL;
        }
    }

    if (expPar->LinearExp.exp_sensor_params.coarse_integration_time != 0) {
        memset(&ctrl, 0, sizeof(ctrl));
        ctrl.id = V4L2_CID_EXPOSURE;
        ctrl.value = expPar->LinearExp.exp_sensor_params.coarse_integration_time;
        if (io_control(VIDIOC_S_CTRL, &ctrl) < 0) {
            LOGE_CAMHW ("failed to set dgain result(val: %d)", ctrl.value);
            return XCAM_RETURN_ERROR_IOCTL;
        }
    }

    EXIT_CAMHW_FUNCTION();
    return XCAM_RETURN_NO_ERROR;
}

int
SensorHw::get_blank(rk_aiq_exposure_sensor_descriptor* sns_des)
{
    struct v4l2_queryctrl ctrl;
    int horzBlank, vertBlank;

    memset(&ctrl, 0, sizeof(ctrl));
    ctrl.id = V4L2_CID_HBLANK;
    if (io_control(VIDIOC_QUERYCTRL, &ctrl) < 0) {
        return -errno;
    }
    horzBlank = ctrl.minimum;

    memset(&ctrl, 0, sizeof(ctrl));
    ctrl.id = V4L2_CID_VBLANK;
    if (io_control(VIDIOC_QUERYCTRL, &ctrl) < 0) {
        return -errno;
    }
    vertBlank = ctrl.minimum;

    sns_des->pixel_periods_per_line = horzBlank + sns_des->sensor_output_width;
    sns_des->line_periods_per_field = vertBlank + sns_des->sensor_output_height;

    return 0;
}

int
SensorHw::get_pixel(rk_aiq_exposure_sensor_descriptor* sns_des)
{
    struct v4l2_ext_controls controls;
    struct v4l2_ext_control ext_control;
    signed long pixel;

    memset(&controls, 0, sizeof(controls));
    memset(&ext_control, 0, sizeof(ext_control));

    ext_control.id = V4L2_CID_PIXEL_RATE;
    controls.ctrl_class = V4L2_CTRL_ID2CLASS(ext_control.id);
    controls.count = 1;
    controls.controls = &ext_control;

    if (io_control(VIDIOC_G_EXT_CTRLS, &controls) < 0)
        return -errno;

    pixel = ext_control.value64;

    sns_des->pixel_clock_freq_mhz = (float)pixel / 1000000;

    return 0;
}

int
SensorHw::get_sensor_fps(float& fps)
{
    struct v4l2_subdev_frame_interval finterval;

    memset(&finterval, 0, sizeof(finterval));
    finterval.pad = 0;

    if (io_control(VIDIOC_SUBDEV_G_FRAME_INTERVAL, &finterval) < 0)
        return -errno;

    fps = (float)(finterval.interval.denominator) / finterval.interval.numerator;

    return 0;
}

int
SensorHw::get_format(rk_aiq_exposure_sensor_descriptor* sns_des)
{
    struct v4l2_subdev_format fmt;
    uint32_t format_code;

    memset(&fmt, 0, sizeof(fmt));
    fmt.pad = 0;
    fmt.which = V4L2_SUBDEV_FORMAT_ACTIVE;

    if (io_control(VIDIOC_SUBDEV_G_FMT, &fmt) < 0)
        return -errno;

    sns_des->sensor_output_width = fmt.format.width;
    sns_des->sensor_output_height = fmt.format.height;
    sns_des->sensor_pixelformat = get_v4l2_pixelformat(fmt.format.code);

    return 0;
}

int
SensorHw::get_exposure_range(rk_aiq_exposure_sensor_descriptor* sns_des)
{
    struct v4l2_queryctrl ctrl;

    memset(&ctrl, 0, sizeof(ctrl));
    ctrl.id = V4L2_CID_EXPOSURE;

    if (io_control(VIDIOC_QUERYCTRL, &ctrl) < 0)
        return -errno;

    sns_des->coarse_integration_time_min = ctrl.minimum;
    sns_des->coarse_integration_time_max_margin = 10;

    return 0;
}

XCamReturn
SensorHw::get_sensor_descriptor(rk_aiq_exposure_sensor_descriptor *sns_des)
{
    memset(sns_des, 0, sizeof(rk_aiq_exposure_sensor_descriptor));

    if (get_format(sns_des))
        return XCAM_RETURN_ERROR_IOCTL;

    if (get_blank(sns_des))
        return XCAM_RETURN_ERROR_IOCTL;

    /*
     * pixel rate is not equal to pclk sometimes
     * prefer to use pclk = ppl * lpp * fps
     */
    float fps = 0;
    if (get_sensor_fps(fps) == 0)
        sns_des->pixel_clock_freq_mhz =
            (float)(sns_des->pixel_periods_per_line) *
            sns_des->line_periods_per_field * fps / 1000000.0;
    else if (get_pixel(sns_des))
        return XCAM_RETURN_ERROR_IOCTL;

    if (get_exposure_range(sns_des))
        return XCAM_RETURN_ERROR_IOCTL;

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
SensorHw::setExposureParams(SmartPtr<RkAiqExpParamsProxy>& expPar)
{
    ENTER_CAMHW_FUNCTION();
    SmartLock locker (_mutex);
    if (_first) {
        if (expPar->data()->exp_cnt) {
            int lastIdx = expPar->data()->exp_cnt - 1;
            expPar->data()->LinearExp = expPar->data()->exp_tbl[lastIdx].LinearExp;
            expPar->data()->HdrExp[0] = expPar->data()->exp_tbl[lastIdx].HdrExp[0];
            expPar->data()->HdrExp[1] = expPar->data()->exp_tbl[lastIdx].HdrExp[1];
            expPar->data()->HdrExp[2] = expPar->data()->exp_tbl[lastIdx].HdrExp[2];
        }
        if (_working_mode == RK_AIQ_WORKING_MODE_NORMAL)
            setLinearSensorExposure(expPar->data().ptr());
        else
            setHdrSensorExposure(expPar->data().ptr());

        _effecting_exp_map[0] = expPar;
        _first = false;
        _last_exp_time = expPar;
        _last_exp_gain = expPar;
        LOGD_CAMHW("exp-sync: first set exp, add id[0] to the effected exp map\n");
    } else {
        if (expPar->data()->exp_cnt) {
            SmartPtr<RkAiqExpParamsProxy> expParamsProxy = NULL;

            LOGD_CAMHW("%s: exp_cnt:%d, exp_list remain:%d\n", __FUNCTION__, expPar->data()->exp_cnt, _exp_list.size());
            /* when new exp-table comes, remove elem until meet the first one of last exp-table */
            if(!_exp_list.empty()) {
                while(!_exp_list.back().second) {
                    _exp_list.pop_back();
                    if(_exp_list.empty())
                        break;
                }
            }

            for(int i = 0; i < expPar->data()->exp_cnt; i++) {
                if (_expParamsPool->has_free_items()) {
                    expParamsProxy = _expParamsPool->get_item();
                } else {
                    LOGE_CAMHW("%s: no free params buffer!\n", __FUNCTION__);
                    return XCAM_RETURN_ERROR_MEM;
                }
                memcpy(expParamsProxy->data().ptr(), expPar->data().ptr(), sizeof(RKAiqAecExpInfo_t));
                expParamsProxy->data()->LinearExp = expParamsProxy->data()->exp_tbl[i].LinearExp;
                expParamsProxy->data()->HdrExp[0] = expParamsProxy->data()->exp_tbl[i].HdrExp[0];
                expParamsProxy->data()->HdrExp[1] = expParamsProxy->data()->exp_tbl[i].HdrExp[1];
                expParamsProxy->data()->HdrExp[2] = expParamsProxy->data()->exp_tbl[i].HdrExp[2];
                /* set a flag when it's fisrt elem of exp-table*/
                _exp_list.push_back(std::make_pair(expParamsProxy, (i == 0 ? true : false)));

                if (_working_mode == RK_AIQ_WORKING_MODE_NORMAL) {
                    LOGD_CAMHW("%s:add tbl[%d] to list: a-gain: %d, time: %d\n",
                               __FUNCTION__, i,
                               expParamsProxy->data()->LinearExp.exp_sensor_params.analog_gain_code_global,
                               expParamsProxy->data()->LinearExp.exp_sensor_params.coarse_integration_time);
                } else {
                    LOGD_CAMHW("%s:add tbl[%d] to list: lexp: 0x%x-0x%x, mexp: 0x%x-0x%x, sexp: 0x%x-0x%x\n",
                               __FUNCTION__, i,
                               expParamsProxy->data()->HdrExp[2].exp_sensor_params.analog_gain_code_global,
                               expParamsProxy->data()->HdrExp[2].exp_sensor_params.coarse_integration_time,
                               expParamsProxy->data()->HdrExp[1].exp_sensor_params.analog_gain_code_global,
                               expParamsProxy->data()->HdrExp[1].exp_sensor_params.coarse_integration_time,
                               expParamsProxy->data()->HdrExp[0].exp_sensor_params.analog_gain_code_global,
                               expParamsProxy->data()->HdrExp[0].exp_sensor_params.coarse_integration_time);
                }

            }
        } else {
#if 0//TEST: default run smoother strategy
            _exp_list.push_back(std::make_pair(expPar, true));

            if (_working_mode == RK_AIQ_WORKING_MODE_NORMAL) {
                LOGD_CAMHW("%s:add to list: a-gain: %d, time: %d\n",
                           __FUNCTION__,
                           expPar->data()->LinearExp.exp_sensor_params.analog_gain_code_global,
                           expPar->data()->LinearExp.exp_sensor_params.coarse_integration_time);
            } else {
                LOGD_CAMHW("%s:add to list: lexp: 0x%x-0x%x, mexp: 0x%x-0x%x, sexp: 0x%x-0x%x\n",
                           __FUNCTION__,
                           expPar->data()->HdrExp[2].exp_sensor_params.analog_gain_code_global,
                           expPar->data()->HdrExp[2].exp_sensor_params.coarse_integration_time,
                           expPar->data()->HdrExp[1].exp_sensor_params.analog_gain_code_global,
                           expPar->data()->HdrExp[1].exp_sensor_params.coarse_integration_time,
                           expPar->data()->HdrExp[0].exp_sensor_params.analog_gain_code_global,
                           expPar->data()->HdrExp[0].exp_sensor_params.coarse_integration_time);
            }
#endif
        }
    }
    EXIT_CAMHW_FUNCTION();
    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
SensorHw::getEffectiveExpParams(SmartPtr<RkAiqExpParamsProxy>& expParams, int frame_id)
{
    ENTER_CAMHW_FUNCTION();

    std::map<int, SmartPtr<RkAiqExpParamsProxy>>::iterator it;
    int search_id = frame_id < 0 ? 0 : frame_id;
#ifdef ADD_LOCK
    SmartLock locker (_mutex);
#endif

    it = _effecting_exp_map.find(search_id);

    // havn't found
    if (it == _effecting_exp_map.end()) {
        /* use the latest */
        std::map<int, SmartPtr<RkAiqExpParamsProxy>>::reverse_iterator rit;

        for (rit = _effecting_exp_map.rbegin(); rit != _effecting_exp_map.rend(); rit++) {
            if (rit->first <= search_id)
                break;
        }

        if (rit == _effecting_exp_map.rend()) {
            XCAM_LOG_ERROR("can't find the latest effecting exposure for id %d, impossible case !", search_id);
            return  XCAM_RETURN_ERROR_PARAM;
        }

        expParams = rit->second;
        if (_working_mode == RK_AIQ_WORKING_MODE_NORMAL) {
            XCAM_LOG_DEBUG("%s: search_id: %d, get-last %d, a-gain: %d, time: %d\n",
                           __FUNCTION__, search_id, rit->first,
                           expParams->data()->LinearExp.exp_sensor_params.analog_gain_code_global,
                           expParams->data()->LinearExp.exp_sensor_params.coarse_integration_time);
        } else {
            XCAM_LOG_DEBUG("%s: search_id: %d, get-last %d, lexp: 0x%x-0x%x, mexp: 0x%x-0x%x, sexp: 0x%x-0x%x\n",
                           __FUNCTION__, search_id, rit->first,
                           expParams->data()->HdrExp[2].exp_sensor_params.analog_gain_code_global,
                           expParams->data()->HdrExp[2].exp_sensor_params.coarse_integration_time,
                           expParams->data()->HdrExp[1].exp_sensor_params.analog_gain_code_global,
                           expParams->data()->HdrExp[1].exp_sensor_params.coarse_integration_time,
                           expParams->data()->HdrExp[0].exp_sensor_params.analog_gain_code_global,
                           expParams->data()->HdrExp[0].exp_sensor_params.coarse_integration_time);
        }
    } else {
        expParams = it->second;
        if (_working_mode == RK_AIQ_WORKING_MODE_NORMAL) {
            XCAM_LOG_DEBUG("%s: search_id: %d, get-find %d, a-gain: %d, time: %d\n",
                           __FUNCTION__, search_id, it->first,
                           expParams->data()->LinearExp.exp_sensor_params.analog_gain_code_global,
                           expParams->data()->LinearExp.exp_sensor_params.coarse_integration_time);
        } else {
            XCAM_LOG_DEBUG("%s: search_id: %d, get-find %d, lexp: 0x%x-0x%x, mexp: 0x%x-0x%x, sexp: 0x%x-0x%x\n",
                           __FUNCTION__, search_id, it->first,
                           expParams->data()->HdrExp[2].exp_sensor_params.analog_gain_code_global,
                           expParams->data()->HdrExp[2].exp_sensor_params.coarse_integration_time,
                           expParams->data()->HdrExp[1].exp_sensor_params.analog_gain_code_global,
                           expParams->data()->HdrExp[1].exp_sensor_params.coarse_integration_time,
                           expParams->data()->HdrExp[0].exp_sensor_params.analog_gain_code_global,
                           expParams->data()->HdrExp[0].exp_sensor_params.coarse_integration_time);
        }
    }

    EXIT_CAMHW_FUNCTION();

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
SensorHw::getSensorModeData(const char* sns_ent_name,
                            rk_aiq_exposure_sensor_descriptor& sns_des)
{
    rk_aiq_exposure_sensor_descriptor sensor_desc;

    get_sensor_descriptor (&sensor_desc);

    sns_des.coarse_integration_time_min =
        sensor_desc.coarse_integration_time_min;
    sns_des.coarse_integration_time_max_margin =
        sensor_desc.coarse_integration_time_max_margin;
    sns_des.fine_integration_time_min =
        sensor_desc.fine_integration_time_min;
    sns_des.fine_integration_time_max_margin =
        sensor_desc.fine_integration_time_max_margin;

    sns_des.frame_length_lines = sensor_desc.line_periods_per_field;
    sns_des.line_length_pck = sensor_desc.pixel_periods_per_line;
    sns_des.vt_pix_clk_freq_hz = sensor_desc.pixel_clock_freq_mhz/*  * 1000000 */;
    sns_des.pixel_clock_freq_mhz = sensor_desc.pixel_clock_freq_mhz/* * 1000000 */;

    sns_des.sensor_output_width = sensor_desc.sensor_output_width;
    sns_des.sensor_output_height = sensor_desc.sensor_output_height;

    LOGD_CAMHW("vts-hts-pclk: %d-%d-%d-%f, rect: [%dx%d]\n",
               sns_des.frame_length_lines,
               sns_des.line_length_pck,
               sns_des.vt_pix_clk_freq_hz,
               sns_des.pixel_clock_freq_mhz,
               sns_des.sensor_output_width,
               sns_des.sensor_output_height);

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
SensorHw::handle_sof(int64_t time, int frameid)
{
    ENTER_CAMHW_FUNCTION();
    int effecting_frame_id = 0;
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    _mutex.lock();
    if (frameid - _frame_sequence > 1)
        LOGE_CAMHW("!!!!frame losed,last frameid:%d,current farmeid:%d!!!!\n", _frame_sequence, frameid);

    _frame_sequence = frameid;
    LOGD_CAMHW("%s: frameid=%d, exp_list size=%d, gain_list size=%d",
               __FUNCTION__, frameid, _exp_list.size(), _delayed_gain_list.size());

    SmartPtr<RkAiqExpParamsProxy> exp_time = nullptr;
    SmartPtr<RkAiqExpParamsProxy> exp_gain = nullptr;
    bool set_time = false, set_gain = false;

    while (_effecting_exp_map.size() > 10)
        _effecting_exp_map.erase(_effecting_exp_map.begin());

    if(!_exp_list.empty()) {
        exp_time = _last_exp_time = _exp_list.front().first;
        set_time = true;
        _exp_list.pop_front();
    } else {
        exp_time = _last_exp_time;
    }

    if(!_delayed_gain_list.empty()) {
        exp_gain = _last_exp_gain = _delayed_gain_list.front();
        set_gain = true;
        _delayed_gain_list.pop_front();
    } else {
        exp_gain = _last_exp_gain;
    }

    _mutex.unlock();
    LOGD_CAMHW("%s: working_mode=%d,frameid=%d, status: set_time=%d,set_gain=%d\n",
               __FUNCTION__, _working_mode, frameid, set_time, set_gain);

    if (set_time || set_gain) {
        RKAiqAecExpInfo_t *ptr_new_exp = NULL, new_exp;
        if (_gain_delayed) {
            composeExpParam(exp_time, exp_gain, &new_exp);
            ptr_new_exp = &new_exp;
        } else {
            ptr_new_exp = exp_time->data().ptr();
        }
        if(_working_mode == RK_AIQ_WORKING_MODE_NORMAL) {
            ret = setLinearSensorExposure(ptr_new_exp);
        } else {
            ret = setHdrSensorExposure(ptr_new_exp);
        }
    }

    if (ret != XCAM_RETURN_NO_ERROR) {
        LOGE_CAMHW("%s: sof_id[%d]: set exposure failed!!!\n",
                   __FUNCTION__,
                   frameid);
    }

    if(set_time) {
        _mutex.lock();

        if(_gain_delayed) {
            _delayed_gain_list.push_back(exp_time);
        }
        effecting_frame_id = frameid + _time_delay;
        _effecting_exp_map[effecting_frame_id] = exp_time;

        if (_working_mode == RK_AIQ_WORKING_MODE_NORMAL) {
            LOGD_CAMHW("%s: sof_id[%d], _effecting_exp_map: add %d, a-gain: %d, time: %d\n",
                       __FUNCTION__, frameid, effecting_frame_id,
                       exp_time->data()->LinearExp.exp_sensor_params.analog_gain_code_global,
                       exp_time->data()->LinearExp.exp_sensor_params.coarse_integration_time);
        } else {
            LOGD_CAMHW("%s: sof_id[%d], _effecting_exp_map: add %d, lexp: 0x%x-0x%x, mexp: 0x%x-0x%x, sexp: 0x%x-0x%x\n",
                       __FUNCTION__,  frameid, effecting_frame_id,
                       exp_time->data()->HdrExp[2].exp_sensor_params.analog_gain_code_global,
                       exp_time->data()->HdrExp[2].exp_sensor_params.coarse_integration_time,
                       exp_time->data()->HdrExp[1].exp_sensor_params.analog_gain_code_global,
                       exp_time->data()->HdrExp[1].exp_sensor_params.coarse_integration_time,
                       exp_time->data()->HdrExp[0].exp_sensor_params.analog_gain_code_global,
                       exp_time->data()->HdrExp[0].exp_sensor_params.coarse_integration_time);
        }

        _mutex.unlock();
    }

    EXIT_CAMHW_FUNCTION();
    return ret;
}

uint32_t
SensorHw::get_v4l2_pixelformat(uint32_t pixelcode)
{
    uint32_t pixelformat = -1;

    switch (pixelcode) {
    case MEDIA_BUS_FMT_SRGGB8_1X8:
        pixelformat = V4L2_PIX_FMT_SRGGB8;
        break;
    case MEDIA_BUS_FMT_SBGGR8_1X8:
        pixelformat = V4L2_PIX_FMT_SBGGR8;
        break;
    case MEDIA_BUS_FMT_SGBRG8_1X8:
        pixelformat = V4L2_PIX_FMT_SGBRG8;
        break;
    case MEDIA_BUS_FMT_SGRBG8_1X8:
        pixelformat = V4L2_PIX_FMT_SGRBG8;
        break;
    case MEDIA_BUS_FMT_SBGGR10_1X10:
        pixelformat = V4L2_PIX_FMT_SBGGR10;
        break;
    case MEDIA_BUS_FMT_SRGGB10_1X10:
        pixelformat = V4L2_PIX_FMT_SRGGB10;
        break;
    case MEDIA_BUS_FMT_SGBRG10_1X10:
        pixelformat = V4L2_PIX_FMT_SGBRG10;
        break;
    case MEDIA_BUS_FMT_SGRBG10_1X10:
        pixelformat = V4L2_PIX_FMT_SGRBG10;
        break;
    case MEDIA_BUS_FMT_SRGGB12_1X12:
        pixelformat = V4L2_PIX_FMT_SRGGB12;
        break;
    case MEDIA_BUS_FMT_SBGGR12_1X12:
        pixelformat = V4L2_PIX_FMT_SBGGR12;
        break;
    case MEDIA_BUS_FMT_SGBRG12_1X12:
        pixelformat = V4L2_PIX_FMT_SGBRG12;
        break;
    case MEDIA_BUS_FMT_SGRBG12_1X12:
        pixelformat = V4L2_PIX_FMT_SGRBG12;
        break;
    default:
        //TODO add other
        LOGE_CAMHW ("%s no support pixelcode:0x%x\n",
                    __func__, pixelcode);
    }
    return pixelformat;
}

XCamReturn
SensorHw::set_working_mode(int mode)
{
    rkmodule_hdr_cfg hdr_cfg;
    __u32 hdr_mode = NO_HDR;

    xcam_mem_clear(hdr_cfg);
    if (mode == RK_AIQ_WORKING_MODE_NORMAL) {
        hdr_mode = NO_HDR;
    } else if (mode == RK_AIQ_ISP_HDR_MODE_2_FRAME_HDR ||
               mode == RK_AIQ_ISP_HDR_MODE_2_LINE_HDR) {
        hdr_mode = HDR_X2;
    } else if (mode == RK_AIQ_ISP_HDR_MODE_3_FRAME_HDR ||
               mode == RK_AIQ_ISP_HDR_MODE_3_LINE_HDR) {
        hdr_mode = HDR_X3;
    } else {
        LOGE_CAMHW("failed to set hdr mode to %d", mode);
        return XCAM_RETURN_ERROR_FAILED;
    }
    hdr_cfg.hdr_mode = hdr_mode;
    if (io_control(RKMODULE_SET_HDR_CFG, &hdr_cfg) < 0) {
        LOGE_CAMHW("failed to set hdr mode");
        return XCAM_RETURN_ERROR_IOCTL;
    }

    _working_mode = mode;

    LOGD_CAMHW("%s _working_mode: %d\n",
               __func__, _working_mode);

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
SensorHw::set_exp_delay_info(int time_delay, int gain_delay)
{
    _time_delay = time_delay;
    _gain_delay = gain_delay;

    LOGD_CAMHW ("%s _time_delay: %d, _gain_delay:%d\n",
                __func__, _time_delay, _gain_delay);
    if (_time_delay > _gain_delay) {
        _gain_delayed = true;
    } else if (_time_delay == _gain_delay) {
        _gain_delayed = false;
    } else {
        LOGE_CAMHW("Not support gain's delay greater than time's delay!");
        return XCAM_RETURN_ERROR_PARAM;
    }

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
SensorHw::start()
{
    ENTER_CAMHW_FUNCTION();
    EXIT_CAMHW_FUNCTION();
    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
SensorHw::stop()
{
    ENTER_CAMHW_FUNCTION();
    _exp_list.clear();
    _effecting_exp_map.clear();
    _delayed_gain_list.clear();
    EXIT_CAMHW_FUNCTION();
    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
SensorHw::composeExpParam
(
    SmartPtr<RkAiqExpParamsProxy>& timeValid,
    SmartPtr<RkAiqExpParamsProxy>& gainValid,
    RKAiqAecExpInfo_t* newExp
)
{
    if(_working_mode == RK_AIQ_WORKING_MODE_NORMAL) {
        newExp->LinearExp.exp_sensor_params.analog_gain_code_global =
            gainValid->data()->LinearExp.exp_sensor_params.analog_gain_code_global;
        newExp->LinearExp.exp_sensor_params.coarse_integration_time =
            timeValid->data()->LinearExp.exp_sensor_params.coarse_integration_time;
    } else {
        newExp->HdrExp[2].exp_sensor_params.analog_gain_code_global =
            gainValid->data()->HdrExp[2].exp_sensor_params.analog_gain_code_global;
        newExp->HdrExp[2].exp_sensor_params.coarse_integration_time =
            timeValid->data()->HdrExp[2].exp_sensor_params.coarse_integration_time;
        newExp->HdrExp[1].exp_sensor_params.analog_gain_code_global =
            gainValid->data()->HdrExp[1].exp_sensor_params.analog_gain_code_global;
        newExp->HdrExp[1].exp_sensor_params.coarse_integration_time =
            timeValid->data()->HdrExp[1].exp_sensor_params.coarse_integration_time;
        newExp->HdrExp[0].exp_sensor_params.analog_gain_code_global =
            gainValid->data()->HdrExp[0].exp_sensor_params.analog_gain_code_global;
        newExp->HdrExp[0].exp_sensor_params.coarse_integration_time =
            timeValid->data()->HdrExp[0].exp_sensor_params.coarse_integration_time;
    }
    return XCAM_RETURN_NO_ERROR;
}

}; //namespace RkCam
