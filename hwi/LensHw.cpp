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
#include "LensHw.h"

namespace RkCam {

uint16_t LensHw::DEFAULT_POOL_SIZE = 20;

LensHw::LensHw(const char* name)
    : V4l2SubDevice (name)
{
    ENTER_CAMHW_FUNCTION();
	_rec_sof_idx = 0;
	memset(_frame_time, 0, sizeof(_frame_time));
	memset(_frame_sequence, 0, sizeof(_frame_sequence));
	_afInfoPool = new RkAiqAfInfoPool("LensLocalAfInfoParams", LensHw::DEFAULT_POOL_SIZE);
    EXIT_CAMHW_FUNCTION();
}

LensHw::~LensHw()
{
    ENTER_CAMHW_FUNCTION();
    EXIT_CAMHW_FUNCTION();
}

XCamReturn
LensHw::start()
{
    ENTER_CAMHW_FUNCTION();
	_rec_sof_idx = 0;
	memset(_frame_time, 0, sizeof(_frame_time));
	memset(_frame_sequence, 0, sizeof(_frame_sequence));
    EXIT_CAMHW_FUNCTION();
    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
LensHw::stop()
{
    ENTER_CAMHW_FUNCTION();
    EXIT_CAMHW_FUNCTION();
    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
LensHw::setFocusParams(int position)
{
    ENTER_CAMHW_FUNCTION();
    SmartLock locker (_mutex);

    struct v4l2_control control;

    xcam_mem_clear (control);
    control.id = V4L2_CID_FOCUS_ABSOLUTE;
    control.value = position;

    LOGD_CAMHW("|||set focus result: %d", position);
    if (io_control (VIDIOC_S_CTRL, &control) < 0) {
        LOGE_CAMHW("set focus result failed to device");
        return XCAM_RETURN_ERROR_IOCTL;
    }

    struct rk_cam_vcm_tim tim;
    if (io_control (RK_VIDIOC_VCM_TIMEINFO, &tim) < 0) {
        LOGE_CAMHW("set focus result failed to device");
        return XCAM_RETURN_ERROR_IOCTL;
    }
    _vcm_tim = tim;

    EXIT_CAMHW_FUNCTION();
    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
LensHw::handle_sof(int64_t time, int frameid)
{
    ENTER_CAMHW_FUNCTION();
    SmartLock locker (_mutex);

    XCamReturn ret = XCAM_RETURN_NO_ERROR;
	int idx;

    idx = (_rec_sof_idx + 1) % LENSHW_RECORD_SOF_NUM;
    _frame_sequence[idx] = frameid;
	_frame_time[idx] = time;
	_rec_sof_idx = idx;

	LOGD_CAMHW("%s: frm_id %d, time %lld\n", __func__, frameid, time);

    EXIT_CAMHW_FUNCTION();
    return ret;
}

XCamReturn
LensHw::getAfInfoParams(SmartPtr<RkAiqAfInfoProxy>& afInfo, int frame_id)
{
    ENTER_CAMHW_FUNCTION();
    SmartLock locker (_mutex);

	int i;

    afInfo = NULL;
    if (_afInfoPool->has_free_items()) {
        afInfo = (SmartPtr<RkAiqAfInfoProxy>)_afInfoPool->get_item();
    } else {
        LOGE_CAMHW("%s: no free params buffer!\n", __FUNCTION__);
        return XCAM_RETURN_ERROR_MEM;
    }

	for (i = 0; i < LENSHW_RECORD_SOF_NUM; i++) {
        if (frame_id == _frame_sequence[i])
			break;
	}

    afInfo->data()->focusStartTim = _vcm_tim.vcm_start_t;
    afInfo->data()->focusEndTim = _vcm_tim.vcm_end_t;
    if (i < LENSHW_RECORD_SOF_NUM) {
	    afInfo->data()->sofTime = _frame_time[i];
    } else {
        LOGE_CAMHW("%s: frame_id %d, can not find sof time!\n", __FUNCTION__, frame_id);
        return  XCAM_RETURN_ERROR_PARAM;
	}

	LOGD_CAMHW("%s: frm_id %d, time %lld\n", __func__, frame_id, afInfo->data()->sofTime);

    EXIT_CAMHW_FUNCTION();

    return XCAM_RETURN_NO_ERROR;
}

}; //namespace RkCam
