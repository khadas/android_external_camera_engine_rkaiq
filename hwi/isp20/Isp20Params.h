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

#ifndef _CAM_HW_ISP20_PARAMS_H_
#define _CAM_HW_ISP20_PARAMS_H_

#include "rk_aiq_pool.h"
#include "rkisp2-config.h"
#include "rkispp-config.h"

#define LIMIT_PARA(a,b,c,d,e)      (c+(a-e)*(b-c)/(d -e))


namespace RkCam {


class Isp20Params {
public:
    explicit Isp20Params() {};
    virtual ~Isp20Params() {};

    virtual XCamReturn checkIsp20Params(struct isp2x_isp_params_cfg& isp_cfg);
    virtual XCamReturn convertAiqResultsToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
            SmartPtr<RkAiqIspParamsProxy> aiq_results,
            SmartPtr<RkAiqIspParamsProxy>& last_aiq_results);
    virtual XCamReturn convertAiqResultsToIsp20PpParams(struct rkispp_params_cfg& pp_cfg,
            SmartPtr<RkAiqIsppParamsProxy> aiq_results);

private:
    XCAM_DEAD_COPY(Isp20Params);
    void convertAiqLiteHstWndSize(struct isp2x_rawhistlite_cfg & hst_lite);
    void convertAiqBigHstWndSize(struct isp2x_rawhistbig_cfg & hst_big);
    void convertAiqSiHstWndSize(struct isp2x_sihst_cfg & sihst);
    void convertAiqAeToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
                                   const rk_aiq_isp_aec_meas_t& aec_meas);
    void convertAiqHistToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
                                     const rk_aiq_isp_hist_meas_t& hist_meas);
    void convertAiqAwbToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
                                    const rk_aiq_awb_stat_cfg_v200_t& awb_meas);
    void convertAiqAwbGainToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
                                        const rk_aiq_wb_gain_t& awb_gain);
    void convertAiqAhdrToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
                                     const rk_aiq_isp_hdr_t& ahdr_data);
    void convertAiqAdehazeToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
                                        const rk_aiq_dehaze_cfg_t& dhaze                     );
    void convertAiqAgammaToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
                                       const rk_aiq_gamma_cfg_t& gamma_out_cfg);
    void convertAiqAdemosaicToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
                                          SmartPtr<RkAiqIspParamsProxy> aiq_results);
    void convertAiqLscToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
                                    const rk_aiq_lsc_cfg_t& lsc);
    void convertAiqBlcToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
                                    SmartPtr<RkAiqIspParamsProxy> aiq_results);
    void convertAiqDpccToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
                                     SmartPtr<RkAiqIspParamsProxy> aiq_results);
    void convertAiqCcmToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
                                    const rk_aiq_ccm_cfg_t& ccm);
    void convertAiqA3dlutToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
                                       const rk_aiq_lut3d_cfg_t& lut3d_cfg);
    void convertAiqCpToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
                                       const rk_aiq_acp_params_t& lut3d_cfg);
    void convertAiqIeToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
                                       const rk_aiq_isp_ie_t& ie_cfg);
	void convertAiqRawnrToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
        	rk_aiq_isp_rawnr_t& rawnr);
	void convertAiqTnrToIsp20Params(struct rkispp_params_cfg& pp_cfg,
        rk_aiq_isp_tnr_t& tnr);
	void convertAiqUvnrToIsp20Params(struct rkispp_params_cfg& pp_cfg,
        rk_aiq_isp_uvnr_t& uvnr);
	void convertAiqYnrToIsp20Params(struct rkispp_params_cfg& pp_cfg,
        rk_aiq_isp_ynr_t& ynr);
	void convertAiqSharpenToIsp20Params(struct rkispp_params_cfg& pp_cfg,
        rk_aiq_isp_sharpen_t& sharp, rk_aiq_isp_edgeflt_t& edgeflt);
    void convertAiqAfToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
                                   const rk_aiq_isp_af_meas_t& af_data);
};
};
#endif
