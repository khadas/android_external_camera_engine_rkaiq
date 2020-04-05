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

#define ADD_LOCK

namespace RkCam {

SensorHw::SensorHw(const char* name)
    : V4l2SubDevice (name)
    , _first(true)
    , _working_mode(RK_AIQ_WORKING_MODE_NORMAL)
{
    ENTER_CAMHW_FUNCTION();
    EXIT_CAMHW_FUNCTION();
}

SensorHw::~SensorHw()
{
    ENTER_CAMHW_FUNCTION();
    EXIT_CAMHW_FUNCTION();
}

XCamReturn
SensorHw::setHdrSensorExposure(SmartPtr<RkAiqExpParamsProxy>& expPar)
{
    ENTER_CAMHW_FUNCTION();
    struct hdrae_exp_s hdrExp;

    XCAM_LOG_DEBUG("lexp: 0x%x-0x%x, mexp: 0x%x-0x%x, sexp: 0x%x-0x%x\n",
	expPar->data()->HdrExp[2].exp_sensor_params.analog_gain_code_global,
	expPar->data()->HdrExp[2].exp_sensor_params.coarse_integration_time,
	expPar->data()->HdrExp[1].exp_sensor_params.analog_gain_code_global,
	expPar->data()->HdrExp[1].exp_sensor_params.coarse_integration_time,
	expPar->data()->HdrExp[0].exp_sensor_params.analog_gain_code_global,
	expPar->data()->HdrExp[0].exp_sensor_params.coarse_integration_time);

    memset(&hdrExp, 0, sizeof(hdrExp));
    hdrExp.long_exp_reg =
	expPar->data()->HdrExp[2].exp_sensor_params.coarse_integration_time;
    hdrExp.long_gain_reg =
	expPar->data()->HdrExp[2].exp_sensor_params.analog_gain_code_global;
    hdrExp.middle_exp_reg =
	expPar->data()->HdrExp[1].exp_sensor_params.coarse_integration_time;
    hdrExp.middle_gain_reg =
	expPar->data()->HdrExp[1].exp_sensor_params.analog_gain_code_global;
    hdrExp.short_exp_reg =
	expPar->data()->HdrExp[0].exp_sensor_params.coarse_integration_time;
    hdrExp.short_gain_reg =
	expPar->data()->HdrExp[0].exp_sensor_params.analog_gain_code_global;

    if (io_control(SENSOR_CMD_SET_HDRAE_EXP, &hdrExp) < 0) {
	    XCAM_LOG_ERROR ("failed to set hdrExp exp");
	    return XCAM_RETURN_ERROR_IOCTL;
    }

    EXIT_CAMHW_FUNCTION();
    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
SensorHw::setLinearSensorExposure(SmartPtr<RkAiqExpParamsProxy>& expPar)
{
    ENTER_CAMHW_FUNCTION();
    int frame_line_length;
    struct v4l2_control ctrl;
    rk_aiq_exposure_sensor_descriptor sensor_desc;

    XCAM_LOG_DEBUG("exp: a-gain: %d, time: %d\n",
	   expPar->data()->LinearExp.exp_sensor_params.analog_gain_code_global,
	   expPar->data()->LinearExp.exp_sensor_params.coarse_integration_time);

    // set vts before exposure time firstly
    get_sensor_descriptor (&sensor_desc);

    frame_line_length = sensor_desc.line_periods_per_field;
    memset(&ctrl, 0, sizeof(ctrl));
    ctrl.id = V4L2_CID_VBLANK;
    ctrl.value = frame_line_length - sensor_desc.sensor_output_height;
    if (io_control(VIDIOC_S_CTRL, &ctrl) < 0) {
        XCAM_LOG_ERROR ("failed to set vblank result(val: %d)", ctrl.value);
        return XCAM_RETURN_ERROR_IOCTL;
    }

    if (expPar->data()->LinearExp.exp_sensor_params.analog_gain_code_global >= 0) {
        memset(&ctrl, 0, sizeof(ctrl));
        ctrl.id = V4L2_CID_ANALOGUE_GAIN;
        ctrl.value = expPar->data()->LinearExp.exp_sensor_params.analog_gain_code_global;
        if (io_control(VIDIOC_S_CTRL, &ctrl) < 0) {
            XCAM_LOG_ERROR ("failed to  set again result(val: %d)", ctrl.value);
            return XCAM_RETURN_ERROR_IOCTL;
        }
    }

    if (expPar->data()->LinearExp.exp_sensor_params.digital_gain_global != 0) {
        memset(&ctrl, 0, sizeof(ctrl));
        ctrl.id = V4L2_CID_GAIN;
        ctrl.value = expPar->data()->LinearExp.exp_sensor_params.digital_gain_global;
        if (io_control(VIDIOC_S_CTRL, &ctrl) < 0) {
            XCAM_LOG_ERROR ("failed to set dgain result(val: %d)", ctrl.value);
            return XCAM_RETURN_ERROR_IOCTL;
        }
    }

    if (expPar->data()->LinearExp.exp_sensor_params.coarse_integration_time != 0) {
        memset(&ctrl, 0, sizeof(ctrl));
        ctrl.id = V4L2_CID_EXPOSURE;
        ctrl.value = expPar->data()->LinearExp.exp_sensor_params.coarse_integration_time;
        if (io_control(VIDIOC_S_CTRL, &ctrl) < 0) {
            XCAM_LOG_ERROR ("failed to set dgain result(val: %d)", ctrl.value);
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
#ifdef ADD_LOCK
    SmartLock locker (_mutex);
#endif

    if (_first) {
	if (_working_mode == RK_AIQ_WORKING_MODE_NORMAL)
            setLinearSensorExposure(expPar);
	else
	    setHdrSensorExposure(expPar);

	_effecting_exp_map[2] = expPar;
	_first = false;
    } else {
        _exp_list.push_back(expPar);
    }

    EXIT_CAMHW_FUNCTION();
    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
SensorHw::getEffectiveParams(SmartPtr<RkAiqExpParamsProxy>& expParams, int frame_id)
{
    ENTER_CAMHW_FUNCTION();

    std::map<int, SmartPtr<RkAiqExpParamsProxy>>::iterator it;
    int num = _effecting_exp_map.size();
    int search_id = frame_id < 0 ? 0 : frame_id;
#ifdef ADD_LOCK
    SmartLock locker (_mutex);
#endif

    do {
        it = _effecting_exp_map.find(search_id);
        if (it != _effecting_exp_map.end()) {
            expParams = _effecting_exp_map[search_id];
            break;
        }
    } while (--num > 0 && --search_id >= 0);

    // havn't found
    if (it == _effecting_exp_map.end()) {
          /* use the latest */
          std::map<int, SmartPtr<RkAiqExpParamsProxy>>::reverse_iterator rit;

	  if (_effecting_exp_map.size() < 1) {
              expParams = _effecting_exp_map[2];
	      /*
	       * printf("debug-hdr: %d: get 2, lexp: %d from map\n",
	       *        search_id,
	       *        expParams->data()->HdrExp[2].exp_sensor_params.coarse_integration_time);
	       */
	  } else {
	      int i = 0;

              rit = _effecting_exp_map.rbegin();
	      /* while (i++ < 3 && rit->first > search_id) */
                  /* rit++; */
	      expParams = rit->second;
	      /*
	       * printf("debug-hdr: %d: get-last %d, lexp: %d from map\n",
	       *        search_id, rit->first,
	       *        expParams->data()->HdrExp[2].exp_sensor_params.coarse_integration_time);
	       */
	  }
    } else {
	    /*
	     * printf("debug-hdr: %d: get-find %d, lexp: %d from map\n",
	     *        search_id, it->first,
	     *        expParams->data()->HdrExp[2].exp_sensor_params.coarse_integration_time);
	     */
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

    XCAM_LOG_DEBUG("vts-hts-pclk: %d-%d-%d-%f, rect: [%dx%d]\n",
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

#ifdef ADD_LOCK
    _mutex.lock();
#endif
    if (!_exp_list.empty()) {
        SmartPtr<RkAiqExpParamsProxy> exp = nullptr;

        if (_effecting_exp_map.size() > 10)
            _effecting_exp_map.erase(_effecting_exp_map.begin());

        exp = _exp_list.front();
#ifdef ADD_LOCK
	_mutex.unlock();
#endif

	if (_working_mode == RK_AIQ_WORKING_MODE_NORMAL)
            setLinearSensorExposure(exp);
	else
	    setHdrSensorExposure(exp);

#ifdef ADD_LOCK
        _mutex.lock();
#endif
        _exp_list.pop_front();
        effecting_frame_id = frameid + _exp_delay - 1;
        _effecting_exp_map[effecting_frame_id] = exp;
	/*
	 * printf("debug-hdr: add %d, lexp: %d to map\n",
	 *        effecting_frame_id,
	 *        exp->data()->HdrExp[2].exp_sensor_params.coarse_integration_time);
	 */
#ifdef ADD_LOCK
	_mutex.unlock();
#endif
    } else {
#ifdef ADD_LOCK
	_mutex.unlock();
#endif
    }

    EXIT_CAMHW_FUNCTION();
    return XCAM_RETURN_NO_ERROR;
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
            XCAM_LOG_ERROR ("%s no support pixelcode:0x%x\n",
                __func__, pixelcode);
    }
    return pixelformat;
}

XCamReturn
SensorHw::set_working_mode(int mode)
{
    rkmodule_hdr_cfg hdr_cfg;
    __u32 hdr_mode;

    xcam_mem_clear(hdr_cfg);
    if (mode == RK_AIQ_WORKING_MODE_NORMAL) {
        hdr_mode = NO_HDR;
    } else if (mode == RK_AIQ_ISP_HDR_MODE_2_FRAME_HDR ||
               mode == RK_AIQ_ISP_HDR_MODE_2_LINE_HDR) {
        hdr_mode = HDR_X2;
    } else if (mode == RK_AIQ_ISP_HDR_MODE_3_FRAME_HDR ||
               mode == RK_AIQ_ISP_HDR_MODE_3_LINE_HDR) {
        hdr_mode = HDR_X3;
    }
    hdr_cfg.hdr_mode = hdr_mode;
    if (io_control(RKMODULE_SET_HDR_CFG, &hdr_cfg) < 0) {
        XCAM_LOG_ERROR ("failed to set hdr mode");
        return XCAM_RETURN_ERROR_IOCTL;
    }

    _working_mode = mode;

    XCAM_LOG_DEBUG ("%s _working_mode: %d\n",
                    __func__, _working_mode);

    return XCAM_RETURN_NO_ERROR;
}

}; //namespace RkCam
