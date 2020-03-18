
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

#ifndef _CAM_HW_ISP20_H_
#define _CAM_HW_ISP20_H_

#include "CamHwBase.h"
#include "Isp20Params.h"

namespace RkCam {
class CamHwIsp20
    : public CamHwBase, public Isp20Params, public V4l2Device {
public:
    explicit CamHwIsp20();
    virtual ~CamHwIsp20();

    // from CamHwBase
    virtual XCamReturn init(const char* sns_ent_name);
    virtual XCamReturn deInit();
    virtual XCamReturn prepare(uint32_t width, uint32_t height, int mode);
    virtual XCamReturn start();
    virtual XCamReturn stop();
    virtual XCamReturn getSensorModeData(const char* sns_ent_name,
                                         rk_aiq_exposure_sensor_descriptor& sns_des);
    virtual XCamReturn setIspParams(SmartPtr<RkAiqIspParamsProxy>& ispParams);
    virtual XCamReturn setExposureParams(SmartPtr<RkAiqExpParamsProxy>& expPar);
    virtual XCamReturn setFocusParams(SmartPtr<RkAiqFocusParamsProxy>& focus_params);
    virtual XCamReturn setHdrProcessCount(int frame_id, int count);

private:
    XCAM_DEAD_COPY(CamHwIsp20);

    Mutex _mutex;
    bool _first;
    volatile bool _is_exit;
    SmartPtr<RkAiqIspParamsProxy> _last_aiq_results;
    struct isp2x_isp_params_cfg _full_active_isp_params;
    std::list<SmartPtr<RkAiqIspParamsProxy>> _pending_ispparams_queue;
    std::map<int, SmartPtr<RkAiqIspParamsPool>> _effecting_ispparm_map;

    void gen_full_isp_params(const struct isp2x_isp_params_cfg* update_params,
                             struct isp2x_isp_params_cfg* full_params);
    void dump_isp_config(struct isp2x_isp_params_cfg* isp_params,
                         SmartPtr<RkAiqIspParamsProxy> aiq_results);
    XCamReturn setIspParamsSync();
	void dumpRawnrFixValue(struct isp2x_rawnr_cfg * pRawnrCfg );
	void dumpTnrFixValue(struct rkispp_tnr_config  * pTnrCfg);
	void dumpUvnrFixValue(struct rkispp_nr_config  * pNrCfg);
	void dumpYnrFixValue(struct rkispp_nr_config  * pNrCfg);
	void dumpSharpFixValue(struct rkispp_sharp_config  * pSharpCfg);
};

};
#endif
