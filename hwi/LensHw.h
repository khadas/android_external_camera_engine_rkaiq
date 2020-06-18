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

#ifndef _LENS_HW_BASE_H_
#define _LENS_HW_BASE_H_

#include <map>
#include <list>
#include "v4l2_device.h"
#include "rk_aiq_pool.h"
#include "linux/rk-camera-module.h"

struct rk_cam_vcm_tim {
    struct timeval vcm_start_t;
    struct timeval vcm_end_t;
};

#define RK_VIDIOC_VCM_TIMEINFO \
    _IOR('V', BASE_VIDIOC_PRIVATE + 0, struct rk_cam_vcm_tim)

#define LENSHW_RECORD_SOF_NUM   8

using namespace XCam;

namespace RkCam {

class LensHw : public V4l2SubDevice {
public:
    explicit LensHw(const char* name);
    virtual ~LensHw();

    XCamReturn start();
    XCamReturn stop();
    XCamReturn setFocusParams(int position);
    XCamReturn handle_sof(int64_t time, int frameid);
    XCamReturn getAfInfoParams(SmartPtr<RkAiqAfInfoProxy>& afInfo, int frame_id);

private:
    XCAM_DEAD_COPY (LensHw);
    Mutex _mutex;
    SmartPtr<RkAiqAfInfoPool> _afInfoPool;
    static uint16_t DEFAULT_POOL_SIZE;
    struct rk_cam_vcm_tim _vcm_tim;
    int64_t _frame_time[LENSHW_RECORD_SOF_NUM];
    int _frame_sequence[LENSHW_RECORD_SOF_NUM];
    int _rec_sof_idx;
};

}; //namespace RkCam

#endif
