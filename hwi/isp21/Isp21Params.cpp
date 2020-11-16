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

#include "Isp21Params.h"

namespace RkCam {

#define ISP2X_WBGAIN_FIXSCALE_BIT  8//check
#define ISP2X_BLC_BIT_MAX 12
void
Isp21Params::convertAiqAwbGainToIsp21Params(struct isp21_isp_params_cfg& isp_cfg,
        const rk_aiq_wb_gain_t& awb_gain, SmartPtr<RkAiqIspParamsProxy> aiq_results, bool awb_gain_update)
{

    if(awb_gain_update) {
        isp_cfg.module_ens |= 1LL << RK_ISP2X_AWB_GAIN_ID;
        isp_cfg.module_cfg_update |= 1LL << RK_ISP2X_AWB_GAIN_ID;
        isp_cfg.module_en_update |= 1LL << RK_ISP2X_AWB_GAIN_ID;
    } else {
        return;
    }

    struct isp21_awb_gain_cfg *  cfg = &isp_cfg.others.awb_gain_cfg;
    uint16_t max_wb_gain = (1 << (ISP2X_WBGAIN_FIXSCALE_BIT + 3)) - 1;
    rk_aiq_wb_gain_t awb_gain1 = awb_gain;
    rk_aiq_isp_params_v21_t* isp21_result =
        static_cast<rk_aiq_isp_params_v21_t*>(aiq_results->data().ptr());
    if(isp21_result->blc.v0.stResult.enable) {
        awb_gain1.bgain *= (float)((1 << ISP2X_BLC_BIT_MAX) - 1) / ((1 << ISP2X_BLC_BIT_MAX) - 1 - isp21_result->blc.v0.stResult.blc_b);
        awb_gain1.gbgain *= (float)((1 << ISP2X_BLC_BIT_MAX) - 1) / ((1 << ISP2X_BLC_BIT_MAX) - 1 - isp21_result->blc.v0.stResult.blc_gb);
        awb_gain1.rgain *= (float)((1 << ISP2X_BLC_BIT_MAX) - 1) / ((1 << ISP2X_BLC_BIT_MAX) - 1 - isp21_result->blc.v0.stResult.blc_r);
        awb_gain1.grgain *= (float)((1 << ISP2X_BLC_BIT_MAX) - 1) / ((1 << ISP2X_BLC_BIT_MAX) - 1 - isp21_result->blc.v0.stResult.blc_gr);
    }
    uint16_t R = (uint16_t)(0.5 + awb_gain1.rgain * (1 << ISP2X_WBGAIN_FIXSCALE_BIT));
    uint16_t B = (uint16_t)(0.5 + awb_gain1.bgain * (1 << ISP2X_WBGAIN_FIXSCALE_BIT));
    uint16_t Gr = (uint16_t)(0.5 + awb_gain1.grgain * (1 << ISP2X_WBGAIN_FIXSCALE_BIT));
    uint16_t Gb = (uint16_t)(0.5 + awb_gain1.gbgain * (1 << ISP2X_WBGAIN_FIXSCALE_BIT));
    cfg->gain0_red       = R > max_wb_gain ? max_wb_gain : R;
    cfg->gain0_blue      = B > max_wb_gain ? max_wb_gain : B;
    cfg->gain0_green_r   = Gr > max_wb_gain ? max_wb_gain : Gr ;
    cfg->gain0_green_b   = Gb > max_wb_gain ? max_wb_gain : Gb;
    cfg->gain1_red       = R > max_wb_gain ? max_wb_gain : R;
    cfg->gain1_blue      = B > max_wb_gain ? max_wb_gain : B;
    cfg->gain1_green_r   = Gr > max_wb_gain ? max_wb_gain : Gr ;
    cfg->gain1_green_b   = Gb > max_wb_gain ? max_wb_gain : Gb;
    cfg->gain2_red       = R > max_wb_gain ? max_wb_gain : R;
    cfg->gain2_blue      = B > max_wb_gain ? max_wb_gain : B;
    cfg->gain2_green_r   = Gr > max_wb_gain ? max_wb_gain : Gr ;
    cfg->gain2_green_b   = Gb > max_wb_gain ? max_wb_gain : Gb;

}

void
Isp21Params::convertAiqBlcToIsp21Params(struct isp21_isp_params_cfg& isp_cfg,
                                        SmartPtr<RkAiqIspParamsProxy> aiq_results)
{
    LOGD_CAMHW_SUBM(ISP20PARAM_SUBM, "%s:(%d) enter \n", __FUNCTION__, __LINE__);

    rk_aiq_isp_params_v21_t* isp21_result =
        static_cast<rk_aiq_isp_params_v21_t*>(aiq_results->data().ptr());

    if(isp21_result->blc.v0.stResult.enable) {
        isp_cfg.module_ens |= ISP2X_MODULE_BLS;
    }
    isp_cfg.module_en_update |= ISP2X_MODULE_BLS;
    isp_cfg.module_cfg_update |= ISP2X_MODULE_BLS;

    isp_cfg.others.bls_cfg.enable_auto = 0;
    isp_cfg.others.bls_cfg.en_windows = 0;

    isp_cfg.others.bls_cfg.bls_window1.h_offs = 0;
    isp_cfg.others.bls_cfg.bls_window1.v_offs = 0;
    isp_cfg.others.bls_cfg.bls_window1.h_size = 0;
    isp_cfg.others.bls_cfg.bls_window1.v_size = 0;

    isp_cfg.others.bls_cfg.bls_window2.h_offs = 0;
    isp_cfg.others.bls_cfg.bls_window2.v_offs = 0;
    isp_cfg.others.bls_cfg.bls_window2.h_size = 0;
    isp_cfg.others.bls_cfg.bls_window2.v_size = 0;

    isp_cfg.others.bls_cfg.bls_samples = 0;

    isp_cfg.others.bls_cfg.fixed_val.r = isp21_result->blc.v0.stResult.blc_r;
    isp_cfg.others.bls_cfg.fixed_val.gr = isp21_result->blc.v0.stResult.blc_gr;
    isp_cfg.others.bls_cfg.fixed_val.gb = isp21_result->blc.v0.stResult.blc_gb;
    isp_cfg.others.bls_cfg.fixed_val.b = isp21_result->blc.v0.stResult.blc_b;

    //TODO bls1 params
    isp_cfg.others.bls_cfg.bls1_en = 0;

    LOGD_CAMHW_SUBM(ISP20PARAM_SUBM, "%s:(%d) exit \n", __FUNCTION__, __LINE__);

}

void
Isp21Params::convertAiqAdehazeToIsp21Params(struct isp21_isp_params_cfg& isp_cfg,
        const rk_aiq_isp_dhaz_cfg_v21_t& dhaze)
{

}

void
Isp21Params::convertAiqCcmToIsp21Params(struct isp21_isp_params_cfg& isp_cfg,
                                        const rk_aiq_ccm_cfg_t& ccm)
{
    if(ccm.ccmEnable) {
        isp_cfg.module_ens |= ISP2X_MODULE_CCM;
    }
    isp_cfg.module_en_update |= ISP2X_MODULE_CCM;
    isp_cfg.module_cfg_update |= ISP2X_MODULE_CCM;

    struct isp21_ccm_cfg *  cfg = &isp_cfg.others.ccm_cfg;
    const float *coeff = ccm.matrix;
    const float *offset = ccm.offs;

    cfg->coeff0_r =  (coeff[0] - 1) > 0 ? (short)((coeff[0] - 1) * 128 + 0.5) : (short)((coeff[0] - 1) * 128 - 0.5); //check -128?
    cfg->coeff1_r =  coeff[1] > 0 ? (short)(coeff[1] * 128 + 0.5) : (short)(coeff[1] * 128 - 0.5);
    cfg->coeff2_r =  coeff[2] > 0 ? (short)(coeff[2] * 128 + 0.5) : (short)(coeff[2] * 128 - 0.5);
    cfg->coeff0_g =  coeff[3] > 0 ? (short)(coeff[3] * 128 + 0.5) : (short)(coeff[3] * 128 - 0.5);
    cfg->coeff1_g =  (coeff[4] - 1) > 0 ? (short)((coeff[4] - 1) * 128 + 0.5) : (short)((coeff[4] - 1) * 128 - 0.5);
    cfg->coeff2_g =  coeff[5] > 0 ? (short)(coeff[5] * 128 + 0.5) : (short)(coeff[5] * 128 - 0.5);
    cfg->coeff0_b =  coeff[6] > 0 ? (short)(coeff[6] * 128 + 0.5) : (short)(coeff[6] * 128 - 0.5);
    cfg->coeff1_b =  coeff[7] > 0 ? (short)(coeff[7] * 128 + 0.5) : (short)(coeff[7] * 128 - 0.5);
    cfg->coeff2_b =  (coeff[8] - 1) > 0 ? (short)((coeff[8] - 1) * 128 + 0.5) : (short)((coeff[8] - 1) * 128 - 0.5);

    cfg->offset_r = offset[0] > 0 ? (short)(offset[0] + 0.5) : (short)(offset[0] - 0.5);// for 12bit
    cfg->offset_g = offset[1] > 0 ? (short)(offset[1] + 0.5) : (int)(offset[1] - 0.5);
    cfg->offset_b = offset[2] > 0 ? (short)(offset[2] + 0.5) : (short)(offset[2] - 0.5);

    cfg->coeff0_y = (u16 )ccm.rgb2y_para[0];
    cfg->coeff1_y = (u16 )ccm.rgb2y_para[1];
    cfg->coeff2_y = (u16 )ccm.rgb2y_para[2];
    cfg->bound_bit = (u8)ccm.bound_bit;//check
    cfg->highy_adjust_dis = 1;

    for( int i = 0; i < 17; i++)
    {
        cfg->alp_y[i] = (u16)(ccm.alp_y[i]);
    }

}


void
Isp21Params::convertAiqTmoToIsp21Params(struct isp21_isp_params_cfg& isp_cfg,
                                        const rk_aiq_isp_drc_v21_t& tmo_data)
{

}

void
Isp21Params::convertAiqGicToIsp21Params(struct isp21_isp_params_cfg& isp_cfg,
                                        const rk_aiq_isp_gic_v21_t& gic_cfg)
{

}

void
Isp21Params::convertAiqAwbToIsp21Params(struct isp21_isp_params_cfg& isp_cfg,
                                        const rk_aiq_awb_stat_cfg_v201_t& awb_meas,
                                        bool awb_cfg_udpate)
{

    if(awb_cfg_udpate) {
        if(awb_meas.awbEnable) {
            isp_cfg.module_ens |= ISP2X_MODULE_RAWAWB;
            isp_cfg.module_cfg_update |= ISP2X_MODULE_RAWAWB;
            isp_cfg.module_en_update |= ISP2X_MODULE_RAWAWB;
        }
    } else {
        return;
    }
    struct isp21_rawawb_meas_cfg * awb_cfg_v201 = &isp_cfg.meas.rawawb;
    awb_cfg_v201->sw_rawawb_xy_en0          =  awb_meas.xyDetectionEnable[RK_AIQ_AWB_XY_TYPE_NORMAL_V201];
    awb_cfg_v201->sw_rawawb_uv_en0          =  awb_meas.uvDetectionEnable[RK_AIQ_AWB_XY_TYPE_NORMAL_V201];
    awb_cfg_v201->sw_rawawb_3dyuv_en0          =  awb_meas.threeDyuvEnable[RK_AIQ_AWB_XY_TYPE_NORMAL_V201];
    awb_cfg_v201->sw_rawawb_xy_en1          =  awb_meas.xyDetectionEnable[RK_AIQ_AWB_XY_TYPE_BIG_V201];
    awb_cfg_v201->sw_rawawb_uv_en1          =  awb_meas.uvDetectionEnable[RK_AIQ_AWB_XY_TYPE_BIG_V201];
    awb_cfg_v201->sw_rawawb_3dyuv_en1          =  awb_meas.threeDyuvEnable[RK_AIQ_AWB_XY_TYPE_BIG_V201];
    awb_cfg_v201->sw_rawawb_wp_blk_wei_en0    =  awb_meas.blkWeightEnable[RK_AIQ_AWB_XY_TYPE_NORMAL_V201];
    awb_cfg_v201->sw_rawawb_wp_blk_wei_en1    =  awb_meas.blkWeightEnable[RK_AIQ_AWB_XY_TYPE_BIG_V201];
    awb_cfg_v201->sw_rawlsc_bypass_en    =  awb_meas.lscBypEnable;//check
    awb_cfg_v201->sw_rawawb_blk_measure_enable    =  awb_meas.blkStatisticsEnable;
    awb_cfg_v201->sw_rawawb_blk_measure_mode     =  awb_meas.blkMeasureMode;
    awb_cfg_v201->sw_rawawb_blk_measure_xytype     =  awb_meas.xyRangeTypeForBlkStatistics;
    awb_cfg_v201->sw_rawawb_blk_measure_illu_idx     =  awb_meas.illIdxForBlkStatistics;
    awb_cfg_v201->sw_rawawb_blk_with_luma_wei_en  =  awb_meas.blkStatisticsWithLumaWeightEn;
    awb_cfg_v201->sw_rawawb_wp_luma_wei_en0   =  awb_meas.wpDiffWeiEnable[RK_AIQ_AWB_XY_TYPE_NORMAL_V201];
    awb_cfg_v201->sw_rawawb_wp_luma_wei_en1   =  awb_meas.wpDiffWeiEnable[RK_AIQ_AWB_XY_TYPE_BIG_V201];
    awb_cfg_v201->sw_rawawb_wp_hist_xytype    =  awb_meas.xyRangeTypeForWpHist;
    awb_cfg_v201->sw_rawawb_3dyuv_ls_idx0       =  awb_meas.threeDyuvIllu[0];
    awb_cfg_v201->sw_rawawb_3dyuv_ls_idx1       =  awb_meas.threeDyuvIllu[1];
    awb_cfg_v201->sw_rawawb_3dyuv_ls_idx2       =  awb_meas.threeDyuvIllu[2];
    awb_cfg_v201->sw_rawawb_3dyuv_ls_idx3       =  awb_meas.threeDyuvIllu[3];
    awb_cfg_v201->sw_rawawb_light_num      =  awb_meas.lightNum;
    awb_cfg_v201->sw_rawawb_h_offs         =  awb_meas.windowSet[0];
    awb_cfg_v201->sw_rawawb_v_offs         =  awb_meas.windowSet[1];
    awb_cfg_v201->sw_rawawb_h_size         =  awb_meas.windowSet[2];
    awb_cfg_v201->sw_rawawb_v_size         =  awb_meas.windowSet[3];
    awb_cfg_v201->sw_rawawb_wind_size = awb_meas.dsMode;
    awb_cfg_v201->sw_rawawb_r_max          =  awb_meas.maxR;
    awb_cfg_v201->sw_rawawb_g_max          =  awb_meas.maxG;
    awb_cfg_v201->sw_rawawb_b_max          =  awb_meas.maxB;
    awb_cfg_v201->sw_rawawb_y_max          =  awb_meas.maxY;
    awb_cfg_v201->sw_rawawb_r_min          =  awb_meas.minR;
    awb_cfg_v201->sw_rawawb_g_min          =  awb_meas.minG;
    awb_cfg_v201->sw_rawawb_b_min          =  awb_meas.minB;
    awb_cfg_v201->sw_rawawb_y_min          =  awb_meas.minY;
    awb_cfg_v201->sw_rawawb_vertex0_u_0    =  awb_meas.uvRange_param[0].pu_region[0];
    awb_cfg_v201->sw_rawawb_vertex0_v_0    =  awb_meas.uvRange_param[0].pv_region[0];
    awb_cfg_v201->sw_rawawb_vertex1_u_0    =  awb_meas.uvRange_param[0].pu_region[1];
    awb_cfg_v201->sw_rawawb_vertex1_v_0    =  awb_meas.uvRange_param[0].pv_region[1];
    awb_cfg_v201->sw_rawawb_vertex2_u_0    =  awb_meas.uvRange_param[0].pu_region[2];
    awb_cfg_v201->sw_rawawb_vertex2_v_0    =  awb_meas.uvRange_param[0].pv_region[2];
    awb_cfg_v201->sw_rawawb_vertex3_u_0    =  awb_meas.uvRange_param[0].pu_region[3];
    awb_cfg_v201->sw_rawawb_vertex3_v_0    =  awb_meas.uvRange_param[0].pv_region[3];
    awb_cfg_v201->sw_rawawb_islope01_0      =  awb_meas.uvRange_param[0].slope_inv[0];
    awb_cfg_v201->sw_rawawb_islope12_0      =  awb_meas.uvRange_param[0].slope_inv[1];
    awb_cfg_v201->sw_rawawb_islope23_0      =  awb_meas.uvRange_param[0].slope_inv[2];
    awb_cfg_v201->sw_rawawb_islope30_0      =  awb_meas.uvRange_param[0].slope_inv[3];
    awb_cfg_v201->sw_rawawb_vertex0_u_1    =  awb_meas.uvRange_param[1].pu_region[0];
    awb_cfg_v201->sw_rawawb_vertex0_v_1    =  awb_meas.uvRange_param[1].pv_region[0];
    awb_cfg_v201->sw_rawawb_vertex1_u_1    =  awb_meas.uvRange_param[1].pu_region[1];
    awb_cfg_v201->sw_rawawb_vertex1_v_1    =  awb_meas.uvRange_param[1].pv_region[1];
    awb_cfg_v201->sw_rawawb_vertex2_u_1    =  awb_meas.uvRange_param[1].pu_region[2];
    awb_cfg_v201->sw_rawawb_vertex2_v_1    =  awb_meas.uvRange_param[1].pv_region[2];
    awb_cfg_v201->sw_rawawb_vertex3_u_1    =  awb_meas.uvRange_param[1].pu_region[3];
    awb_cfg_v201->sw_rawawb_vertex3_v_1    =  awb_meas.uvRange_param[1].pv_region[3];
    awb_cfg_v201->sw_rawawb_islope01_1      =  awb_meas.uvRange_param[1].slope_inv[0];
    awb_cfg_v201->sw_rawawb_islope12_1      =  awb_meas.uvRange_param[1].slope_inv[1];
    awb_cfg_v201->sw_rawawb_islope23_1      =  awb_meas.uvRange_param[1].slope_inv[2];
    awb_cfg_v201->sw_rawawb_islope30_1      =  awb_meas.uvRange_param[1].slope_inv[3];
    awb_cfg_v201->sw_rawawb_vertex0_u_2    =  awb_meas.uvRange_param[2].pu_region[0];
    awb_cfg_v201->sw_rawawb_vertex0_v_2    =  awb_meas.uvRange_param[2].pv_region[0];
    awb_cfg_v201->sw_rawawb_vertex1_u_2    =  awb_meas.uvRange_param[2].pu_region[1];
    awb_cfg_v201->sw_rawawb_vertex1_v_2    =  awb_meas.uvRange_param[2].pv_region[1];
    awb_cfg_v201->sw_rawawb_vertex2_u_2    =  awb_meas.uvRange_param[2].pu_region[2];
    awb_cfg_v201->sw_rawawb_vertex2_v_2    =  awb_meas.uvRange_param[2].pv_region[2];
    awb_cfg_v201->sw_rawawb_vertex3_u_2    =  awb_meas.uvRange_param[2].pu_region[3];
    awb_cfg_v201->sw_rawawb_vertex3_v_2    =  awb_meas.uvRange_param[2].pv_region[3];
    awb_cfg_v201->sw_rawawb_islope01_2      =  awb_meas.uvRange_param[2].slope_inv[0];
    awb_cfg_v201->sw_rawawb_islope12_2      =  awb_meas.uvRange_param[2].slope_inv[1];
    awb_cfg_v201->sw_rawawb_islope23_2      =  awb_meas.uvRange_param[2].slope_inv[2];
    awb_cfg_v201->sw_rawawb_islope30_2      =  awb_meas.uvRange_param[2].slope_inv[3];
    awb_cfg_v201->sw_rawawb_vertex0_u_3    =  awb_meas.uvRange_param[3].pu_region[0];
    awb_cfg_v201->sw_rawawb_vertex0_v_3    =  awb_meas.uvRange_param[3].pv_region[0];
    awb_cfg_v201->sw_rawawb_vertex1_u_3    =  awb_meas.uvRange_param[3].pu_region[1];
    awb_cfg_v201->sw_rawawb_vertex1_v_3    =  awb_meas.uvRange_param[3].pv_region[1];
    awb_cfg_v201->sw_rawawb_vertex2_u_3    =  awb_meas.uvRange_param[3].pu_region[2];
    awb_cfg_v201->sw_rawawb_vertex2_v_3    =  awb_meas.uvRange_param[3].pv_region[2];
    awb_cfg_v201->sw_rawawb_vertex3_u_3    =  awb_meas.uvRange_param[3].pu_region[3];
    awb_cfg_v201->sw_rawawb_vertex3_v_3    =  awb_meas.uvRange_param[3].pv_region[3];
    awb_cfg_v201->sw_rawawb_islope01_3      =  awb_meas.uvRange_param[3].slope_inv[0];
    awb_cfg_v201->sw_rawawb_islope12_3      =  awb_meas.uvRange_param[3].slope_inv[1];
    awb_cfg_v201->sw_rawawb_islope23_3      =  awb_meas.uvRange_param[3].slope_inv[2];
    awb_cfg_v201->sw_rawawb_islope30_3      =  awb_meas.uvRange_param[3].slope_inv[3];
    awb_cfg_v201->sw_rawawb_vertex0_u_4    =  awb_meas.uvRange_param[4].pu_region[0];
    awb_cfg_v201->sw_rawawb_vertex0_v_4    =  awb_meas.uvRange_param[4].pv_region[0];
    awb_cfg_v201->sw_rawawb_vertex1_u_4    =  awb_meas.uvRange_param[4].pu_region[1];
    awb_cfg_v201->sw_rawawb_vertex1_v_4    =  awb_meas.uvRange_param[4].pv_region[1];
    awb_cfg_v201->sw_rawawb_vertex2_u_4    =  awb_meas.uvRange_param[4].pu_region[2];
    awb_cfg_v201->sw_rawawb_vertex2_v_4    =  awb_meas.uvRange_param[4].pv_region[2];
    awb_cfg_v201->sw_rawawb_vertex3_u_4    =  awb_meas.uvRange_param[4].pu_region[3];
    awb_cfg_v201->sw_rawawb_vertex3_v_4    =  awb_meas.uvRange_param[4].pv_region[3];
    awb_cfg_v201->sw_rawawb_islope01_4      =  awb_meas.uvRange_param[4].slope_inv[0];
    awb_cfg_v201->sw_rawawb_islope12_4      =  awb_meas.uvRange_param[4].slope_inv[1];
    awb_cfg_v201->sw_rawawb_islope23_4      =  awb_meas.uvRange_param[4].slope_inv[2];
    awb_cfg_v201->sw_rawawb_islope30_4      =  awb_meas.uvRange_param[4].slope_inv[3];
    awb_cfg_v201->sw_rawawb_vertex0_u_5    =  awb_meas.uvRange_param[5].pu_region[0];
    awb_cfg_v201->sw_rawawb_vertex0_v_5    =  awb_meas.uvRange_param[5].pv_region[0];
    awb_cfg_v201->sw_rawawb_vertex1_u_5    =  awb_meas.uvRange_param[5].pu_region[1];
    awb_cfg_v201->sw_rawawb_vertex1_v_5    =  awb_meas.uvRange_param[5].pv_region[1];
    awb_cfg_v201->sw_rawawb_vertex2_u_5    =  awb_meas.uvRange_param[5].pu_region[2];
    awb_cfg_v201->sw_rawawb_vertex2_v_5    =  awb_meas.uvRange_param[5].pv_region[2];
    awb_cfg_v201->sw_rawawb_vertex3_u_5    =  awb_meas.uvRange_param[5].pu_region[3];
    awb_cfg_v201->sw_rawawb_vertex3_v_5    =  awb_meas.uvRange_param[5].pv_region[3];
    awb_cfg_v201->sw_rawawb_islope01_5      =  awb_meas.uvRange_param[5].slope_inv[0];
    awb_cfg_v201->sw_rawawb_islope12_5      =  awb_meas.uvRange_param[5].slope_inv[1];
    awb_cfg_v201->sw_rawawb_islope23_5      =  awb_meas.uvRange_param[5].slope_inv[2];
    awb_cfg_v201->sw_rawawb_islope30_5      =  awb_meas.uvRange_param[5].slope_inv[3];
    awb_cfg_v201->sw_rawawb_vertex0_u_6    =  awb_meas.uvRange_param[6].pu_region[0];
    awb_cfg_v201->sw_rawawb_vertex0_v_6    =  awb_meas.uvRange_param[6].pv_region[0];
    awb_cfg_v201->sw_rawawb_vertex1_u_6    =  awb_meas.uvRange_param[6].pu_region[1];
    awb_cfg_v201->sw_rawawb_vertex1_v_6    =  awb_meas.uvRange_param[6].pv_region[1];
    awb_cfg_v201->sw_rawawb_vertex2_u_6    =  awb_meas.uvRange_param[6].pu_region[2];
    awb_cfg_v201->sw_rawawb_vertex2_v_6    =  awb_meas.uvRange_param[6].pv_region[2];
    awb_cfg_v201->sw_rawawb_vertex3_u_6    =  awb_meas.uvRange_param[6].pu_region[3];
    awb_cfg_v201->sw_rawawb_vertex3_v_6    =  awb_meas.uvRange_param[6].pv_region[3];
    awb_cfg_v201->sw_rawawb_islope01_6      =  awb_meas.uvRange_param[6].slope_inv[0];
    awb_cfg_v201->sw_rawawb_islope12_6      =  awb_meas.uvRange_param[6].slope_inv[1];
    awb_cfg_v201->sw_rawawb_islope23_6      =  awb_meas.uvRange_param[6].slope_inv[2];
    awb_cfg_v201->sw_rawawb_islope30_6      =  awb_meas.uvRange_param[6].slope_inv[3];
    awb_cfg_v201->sw_rawawb_rgb2ryuvmat0_u =  awb_meas.icrgb2RYuv_matrix[0];
    awb_cfg_v201->sw_rawawb_rgb2ryuvmat1_u =  awb_meas.icrgb2RYuv_matrix[1];
    awb_cfg_v201->sw_rawawb_rgb2ryuvmat2_u =  awb_meas.icrgb2RYuv_matrix[2];
    awb_cfg_v201->sw_rawawb_rgb2ryuvofs_u =  awb_meas.icrgb2RYuv_matrix[3];
    awb_cfg_v201->sw_rawawb_rgb2ryuvmat0_v =  awb_meas.icrgb2RYuv_matrix[4];
    awb_cfg_v201->sw_rawawb_rgb2ryuvmat1_v =  awb_meas.icrgb2RYuv_matrix[5];
    awb_cfg_v201->sw_rawawb_rgb2ryuvmat2_v =  awb_meas.icrgb2RYuv_matrix[6];
    awb_cfg_v201->sw_rawawb_rgb2ryuvofs_v =  awb_meas.icrgb2RYuv_matrix[7];
    awb_cfg_v201->sw_rawawb_rgb2ryuvmat0_y =  awb_meas.icrgb2RYuv_matrix[8];
    awb_cfg_v201->sw_rawawb_rgb2ryuvmat1_y =  awb_meas.icrgb2RYuv_matrix[9];
    awb_cfg_v201->sw_rawawb_rgb2ryuvmat2_y =  awb_meas.icrgb2RYuv_matrix[10];
    awb_cfg_v201->sw_rawawb_rgb2ryuvofs_y =  awb_meas.icrgb2RYuv_matrix[11];
    awb_cfg_v201->sw_rawawb_rotu0_ls0 =  awb_meas.ic3Dyuv2Range_param[0].thcurve_u[0];
    awb_cfg_v201->sw_rawawb_rotu1_ls0 =  awb_meas.ic3Dyuv2Range_param[0].thcurve_u[1];
    awb_cfg_v201->sw_rawawb_rotu2_ls0 =  awb_meas.ic3Dyuv2Range_param[0].thcurve_u[2];
    awb_cfg_v201->sw_rawawb_rotu3_ls0 =  awb_meas.ic3Dyuv2Range_param[0].thcurve_u[3];
    awb_cfg_v201->sw_rawawb_rotu4_ls0 =  awb_meas.ic3Dyuv2Range_param[0].thcurve_u[4];
    awb_cfg_v201->sw_rawawb_rotu5_ls0 =  awb_meas.ic3Dyuv2Range_param[0].thcurve_u[5];
    awb_cfg_v201->sw_rawawb_th0_ls0 =  awb_meas.ic3Dyuv2Range_param[0].thcure_th[0];
    awb_cfg_v201->sw_rawawb_th1_ls0 =  awb_meas.ic3Dyuv2Range_param[0].thcure_th[1];
    awb_cfg_v201->sw_rawawb_th2_ls0 =  awb_meas.ic3Dyuv2Range_param[0].thcure_th[2];
    awb_cfg_v201->sw_rawawb_th3_ls0 =  awb_meas.ic3Dyuv2Range_param[0].thcure_th[3];
    awb_cfg_v201->sw_rawawb_th4_ls0 =  awb_meas.ic3Dyuv2Range_param[0].thcure_th[4];
    awb_cfg_v201->sw_rawawb_th5_ls0 =  awb_meas.ic3Dyuv2Range_param[0].thcure_th[5];
    awb_cfg_v201->sw_rawawb_coor_x1_ls0_u =  awb_meas.ic3Dyuv2Range_param[0].lineP1[0];
    awb_cfg_v201->sw_rawawb_coor_x1_ls0_v =  awb_meas.ic3Dyuv2Range_param[0].lineP1[1];
    awb_cfg_v201->sw_rawawb_coor_x1_ls0_y =  awb_meas.ic3Dyuv2Range_param[0].lineP1[2];
    awb_cfg_v201->sw_rawawb_vec_x21_ls0_u =  awb_meas.ic3Dyuv2Range_param[0].vP1P2[0];
    awb_cfg_v201->sw_rawawb_vec_x21_ls0_v =  awb_meas.ic3Dyuv2Range_param[0].vP1P2[1];
    awb_cfg_v201->sw_rawawb_vec_x21_ls0_y =  awb_meas.ic3Dyuv2Range_param[0].vP1P2[2];
    awb_cfg_v201->sw_rawawb_dis_x1x2_ls0 =  awb_meas.ic3Dyuv2Range_param[0].disP1P2;
    awb_cfg_v201->sw_rawawb_rotu0_ls1 =  awb_meas.ic3Dyuv2Range_param[1].thcurve_u[0];
    awb_cfg_v201->sw_rawawb_rotu1_ls1 =  awb_meas.ic3Dyuv2Range_param[1].thcurve_u[1];
    awb_cfg_v201->sw_rawawb_rotu2_ls1 =  awb_meas.ic3Dyuv2Range_param[1].thcurve_u[2];
    awb_cfg_v201->sw_rawawb_rotu3_ls1 =  awb_meas.ic3Dyuv2Range_param[1].thcurve_u[3];
    awb_cfg_v201->sw_rawawb_rotu4_ls1 =  awb_meas.ic3Dyuv2Range_param[1].thcurve_u[4];
    awb_cfg_v201->sw_rawawb_rotu5_ls1 =  awb_meas.ic3Dyuv2Range_param[1].thcurve_u[5];
    awb_cfg_v201->sw_rawawb_th0_ls1 =  awb_meas.ic3Dyuv2Range_param[1].thcure_th[0];
    awb_cfg_v201->sw_rawawb_th1_ls1 =  awb_meas.ic3Dyuv2Range_param[1].thcure_th[1];
    awb_cfg_v201->sw_rawawb_th2_ls1 =  awb_meas.ic3Dyuv2Range_param[1].thcure_th[2];
    awb_cfg_v201->sw_rawawb_th3_ls1 =  awb_meas.ic3Dyuv2Range_param[1].thcure_th[3];
    awb_cfg_v201->sw_rawawb_th4_ls1 =  awb_meas.ic3Dyuv2Range_param[1].thcure_th[4];
    awb_cfg_v201->sw_rawawb_th5_ls1 =  awb_meas.ic3Dyuv2Range_param[1].thcure_th[5];
    awb_cfg_v201->sw_rawawb_coor_x1_ls1_u =  awb_meas.ic3Dyuv2Range_param[1].lineP1[0];
    awb_cfg_v201->sw_rawawb_coor_x1_ls1_v =  awb_meas.ic3Dyuv2Range_param[1].lineP1[1];
    awb_cfg_v201->sw_rawawb_coor_x1_ls1_y =  awb_meas.ic3Dyuv2Range_param[1].lineP1[2];
    awb_cfg_v201->sw_rawawb_vec_x21_ls1_u =  awb_meas.ic3Dyuv2Range_param[1].vP1P2[0];
    awb_cfg_v201->sw_rawawb_vec_x21_ls1_v =  awb_meas.ic3Dyuv2Range_param[1].vP1P2[1];
    awb_cfg_v201->sw_rawawb_vec_x21_ls1_y =  awb_meas.ic3Dyuv2Range_param[1].vP1P2[2];
    awb_cfg_v201->sw_rawawb_dis_x1x2_ls1 =  awb_meas.ic3Dyuv2Range_param[1].disP1P2;
    awb_cfg_v201->sw_rawawb_rotu0_ls2 =  awb_meas.ic3Dyuv2Range_param[2].thcurve_u[0];
    awb_cfg_v201->sw_rawawb_rotu1_ls2 =  awb_meas.ic3Dyuv2Range_param[2].thcurve_u[1];
    awb_cfg_v201->sw_rawawb_rotu2_ls2 =  awb_meas.ic3Dyuv2Range_param[2].thcurve_u[2];
    awb_cfg_v201->sw_rawawb_rotu3_ls2 =  awb_meas.ic3Dyuv2Range_param[2].thcurve_u[3];
    awb_cfg_v201->sw_rawawb_rotu4_ls2 =  awb_meas.ic3Dyuv2Range_param[2].thcurve_u[4];
    awb_cfg_v201->sw_rawawb_rotu5_ls2 =  awb_meas.ic3Dyuv2Range_param[2].thcurve_u[5];
    awb_cfg_v201->sw_rawawb_th0_ls2 =  awb_meas.ic3Dyuv2Range_param[2].thcure_th[0];
    awb_cfg_v201->sw_rawawb_th1_ls2 =  awb_meas.ic3Dyuv2Range_param[2].thcure_th[1];
    awb_cfg_v201->sw_rawawb_th2_ls2 =  awb_meas.ic3Dyuv2Range_param[2].thcure_th[2];
    awb_cfg_v201->sw_rawawb_th3_ls2 =  awb_meas.ic3Dyuv2Range_param[2].thcure_th[3];
    awb_cfg_v201->sw_rawawb_th4_ls2 =  awb_meas.ic3Dyuv2Range_param[2].thcure_th[4];
    awb_cfg_v201->sw_rawawb_th5_ls2 =  awb_meas.ic3Dyuv2Range_param[2].thcure_th[5];
    awb_cfg_v201->sw_rawawb_coor_x1_ls2_u =  awb_meas.ic3Dyuv2Range_param[2].lineP1[0];
    awb_cfg_v201->sw_rawawb_coor_x1_ls2_v =  awb_meas.ic3Dyuv2Range_param[2].lineP1[1];
    awb_cfg_v201->sw_rawawb_coor_x1_ls2_y =  awb_meas.ic3Dyuv2Range_param[2].lineP1[2];
    awb_cfg_v201->sw_rawawb_vec_x21_ls2_u =  awb_meas.ic3Dyuv2Range_param[2].vP1P2[0];
    awb_cfg_v201->sw_rawawb_vec_x21_ls2_v =  awb_meas.ic3Dyuv2Range_param[2].vP1P2[1];
    awb_cfg_v201->sw_rawawb_vec_x21_ls2_y =  awb_meas.ic3Dyuv2Range_param[2].vP1P2[2];
    awb_cfg_v201->sw_rawawb_dis_x1x2_ls2 =  awb_meas.ic3Dyuv2Range_param[2].disP1P2;

    awb_cfg_v201->sw_rawawb_rotu0_ls3 =  awb_meas.ic3Dyuv2Range_param[3].thcurve_u[0];
    awb_cfg_v201->sw_rawawb_rotu1_ls3 =  awb_meas.ic3Dyuv2Range_param[3].thcurve_u[1];
    awb_cfg_v201->sw_rawawb_rotu2_ls3 =  awb_meas.ic3Dyuv2Range_param[3].thcurve_u[2];
    awb_cfg_v201->sw_rawawb_rotu3_ls3 =  awb_meas.ic3Dyuv2Range_param[3].thcurve_u[3];
    awb_cfg_v201->sw_rawawb_rotu4_ls3 =  awb_meas.ic3Dyuv2Range_param[3].thcurve_u[4];
    awb_cfg_v201->sw_rawawb_rotu5_ls3 =  awb_meas.ic3Dyuv2Range_param[3].thcurve_u[5];
    awb_cfg_v201->sw_rawawb_th0_ls3 =  awb_meas.ic3Dyuv2Range_param[3].thcure_th[0];
    awb_cfg_v201->sw_rawawb_th1_ls3 =  awb_meas.ic3Dyuv2Range_param[3].thcure_th[1];
    awb_cfg_v201->sw_rawawb_th2_ls3 =  awb_meas.ic3Dyuv2Range_param[3].thcure_th[2];
    awb_cfg_v201->sw_rawawb_th3_ls3 =  awb_meas.ic3Dyuv2Range_param[3].thcure_th[3];
    awb_cfg_v201->sw_rawawb_th4_ls3 =  awb_meas.ic3Dyuv2Range_param[3].thcure_th[4];
    awb_cfg_v201->sw_rawawb_th5_ls3 =  awb_meas.ic3Dyuv2Range_param[3].thcure_th[5];
    awb_cfg_v201->sw_rawawb_coor_x1_ls3_u =  awb_meas.ic3Dyuv2Range_param[3].lineP1[0];
    awb_cfg_v201->sw_rawawb_coor_x1_ls3_v =  awb_meas.ic3Dyuv2Range_param[3].lineP1[1];
    awb_cfg_v201->sw_rawawb_coor_x1_ls3_y =  awb_meas.ic3Dyuv2Range_param[3].lineP1[2];
    awb_cfg_v201->sw_rawawb_vec_x21_ls3_u =  awb_meas.ic3Dyuv2Range_param[3].vP1P2[0];
    awb_cfg_v201->sw_rawawb_vec_x21_ls3_v =  awb_meas.ic3Dyuv2Range_param[3].vP1P2[1];
    awb_cfg_v201->sw_rawawb_vec_x21_ls3_y =  awb_meas.ic3Dyuv2Range_param[3].vP1P2[2];
    awb_cfg_v201->sw_rawawb_dis_x1x2_ls3 =  awb_meas.ic3Dyuv2Range_param[3].disP1P2;
    awb_cfg_v201->sw_rawawb_wt0            =  awb_meas.rgb2xy_param.pseudoLuminanceWeight[0];
    awb_cfg_v201->sw_rawawb_wt1            =  awb_meas.rgb2xy_param.pseudoLuminanceWeight[1];
    awb_cfg_v201->sw_rawawb_wt2            =  awb_meas.rgb2xy_param.pseudoLuminanceWeight[2];
    awb_cfg_v201->sw_rawawb_mat0_x         =  awb_meas.rgb2xy_param.rotationMat[0];
    awb_cfg_v201->sw_rawawb_mat1_x         =  awb_meas.rgb2xy_param.rotationMat[1];
    awb_cfg_v201->sw_rawawb_mat2_x         =  awb_meas.rgb2xy_param.rotationMat[2];
    awb_cfg_v201->sw_rawawb_mat0_y         =  awb_meas.rgb2xy_param.rotationMat[3];
    awb_cfg_v201->sw_rawawb_mat1_y         =  awb_meas.rgb2xy_param.rotationMat[4];
    awb_cfg_v201->sw_rawawb_mat2_y         =  awb_meas.rgb2xy_param.rotationMat[5];
    awb_cfg_v201->sw_rawawb_nor_x0_0       =  awb_meas.xyRange_param[0].NorrangeX[0];
    awb_cfg_v201->sw_rawawb_nor_x1_0       =  awb_meas.xyRange_param[0].NorrangeX[1];
    awb_cfg_v201->sw_rawawb_nor_y0_0       =  awb_meas.xyRange_param[0].NorrangeY[0];
    awb_cfg_v201->sw_rawawb_nor_y1_0       =  awb_meas.xyRange_param[0].NorrangeY[1];
    awb_cfg_v201->sw_rawawb_big_x0_0       =  awb_meas.xyRange_param[0].SperangeX[0];
    awb_cfg_v201->sw_rawawb_big_x1_0       =  awb_meas.xyRange_param[0].SperangeX[1];
    awb_cfg_v201->sw_rawawb_big_y0_0       =  awb_meas.xyRange_param[0].SperangeY[0];
    awb_cfg_v201->sw_rawawb_big_y1_0       =  awb_meas.xyRange_param[0].SperangeY[1];
    awb_cfg_v201->sw_rawawb_nor_x0_1       =  awb_meas.xyRange_param[1].NorrangeX[0];
    awb_cfg_v201->sw_rawawb_nor_x1_1       =  awb_meas.xyRange_param[1].NorrangeX[1];
    awb_cfg_v201->sw_rawawb_nor_y0_1       =  awb_meas.xyRange_param[1].NorrangeY[0];
    awb_cfg_v201->sw_rawawb_nor_y1_1       =  awb_meas.xyRange_param[1].NorrangeY[1];
    awb_cfg_v201->sw_rawawb_big_x0_1       =  awb_meas.xyRange_param[1].SperangeX[0];
    awb_cfg_v201->sw_rawawb_big_x1_1       =  awb_meas.xyRange_param[1].SperangeX[1];
    awb_cfg_v201->sw_rawawb_big_y0_1       =  awb_meas.xyRange_param[1].SperangeY[0];
    awb_cfg_v201->sw_rawawb_big_y1_1       =  awb_meas.xyRange_param[1].SperangeY[1];
    awb_cfg_v201->sw_rawawb_nor_x0_2       =  awb_meas.xyRange_param[2].NorrangeX[0];
    awb_cfg_v201->sw_rawawb_nor_x1_2       =  awb_meas.xyRange_param[2].NorrangeX[1];
    awb_cfg_v201->sw_rawawb_nor_y0_2       =  awb_meas.xyRange_param[2].NorrangeY[0];
    awb_cfg_v201->sw_rawawb_nor_y1_2       =  awb_meas.xyRange_param[2].NorrangeY[1];
    awb_cfg_v201->sw_rawawb_big_x0_2       =  awb_meas.xyRange_param[2].SperangeX[0];
    awb_cfg_v201->sw_rawawb_big_x1_2       =  awb_meas.xyRange_param[2].SperangeX[1];
    awb_cfg_v201->sw_rawawb_big_y0_2       =  awb_meas.xyRange_param[2].SperangeY[0];
    awb_cfg_v201->sw_rawawb_big_y1_2       =  awb_meas.xyRange_param[2].SperangeY[1];
    awb_cfg_v201->sw_rawawb_nor_x0_3       =  awb_meas.xyRange_param[3].NorrangeX[0];
    awb_cfg_v201->sw_rawawb_nor_x1_3       =  awb_meas.xyRange_param[3].NorrangeX[1];
    awb_cfg_v201->sw_rawawb_nor_y0_3       =  awb_meas.xyRange_param[3].NorrangeY[0];
    awb_cfg_v201->sw_rawawb_nor_y1_3       =  awb_meas.xyRange_param[3].NorrangeY[1];
    awb_cfg_v201->sw_rawawb_big_x0_3       =  awb_meas.xyRange_param[3].SperangeX[0];
    awb_cfg_v201->sw_rawawb_big_x1_3       =  awb_meas.xyRange_param[3].SperangeX[1];
    awb_cfg_v201->sw_rawawb_big_y0_3       =  awb_meas.xyRange_param[3].SperangeY[0];
    awb_cfg_v201->sw_rawawb_big_y1_3       =  awb_meas.xyRange_param[3].SperangeY[1];
    awb_cfg_v201->sw_rawawb_nor_x0_4       =  awb_meas.xyRange_param[4].NorrangeX[0];
    awb_cfg_v201->sw_rawawb_nor_x1_4       =  awb_meas.xyRange_param[4].NorrangeX[1];
    awb_cfg_v201->sw_rawawb_nor_y0_4       =  awb_meas.xyRange_param[4].NorrangeY[0];
    awb_cfg_v201->sw_rawawb_nor_y1_4       =  awb_meas.xyRange_param[4].NorrangeY[1];
    awb_cfg_v201->sw_rawawb_big_x0_4       =  awb_meas.xyRange_param[4].SperangeX[0];
    awb_cfg_v201->sw_rawawb_big_x1_4       =  awb_meas.xyRange_param[4].SperangeX[1];
    awb_cfg_v201->sw_rawawb_big_y0_4       =  awb_meas.xyRange_param[4].SperangeY[0];
    awb_cfg_v201->sw_rawawb_big_y1_4       =  awb_meas.xyRange_param[4].SperangeY[1];
    awb_cfg_v201->sw_rawawb_nor_x0_5       =  awb_meas.xyRange_param[5].NorrangeX[0];
    awb_cfg_v201->sw_rawawb_nor_x1_5       =  awb_meas.xyRange_param[5].NorrangeX[1];
    awb_cfg_v201->sw_rawawb_nor_y0_5       =  awb_meas.xyRange_param[5].NorrangeY[0];
    awb_cfg_v201->sw_rawawb_nor_y1_5       =  awb_meas.xyRange_param[5].NorrangeY[1];
    awb_cfg_v201->sw_rawawb_big_x0_5       =  awb_meas.xyRange_param[5].SperangeX[0];
    awb_cfg_v201->sw_rawawb_big_x1_5       =  awb_meas.xyRange_param[5].SperangeX[1];
    awb_cfg_v201->sw_rawawb_big_y0_5       =  awb_meas.xyRange_param[5].SperangeY[0];
    awb_cfg_v201->sw_rawawb_big_y1_5       =  awb_meas.xyRange_param[5].SperangeY[1];
    awb_cfg_v201->sw_rawawb_nor_x0_6       =  awb_meas.xyRange_param[6].NorrangeX[0];
    awb_cfg_v201->sw_rawawb_nor_x1_6       =  awb_meas.xyRange_param[6].NorrangeX[1];
    awb_cfg_v201->sw_rawawb_nor_y0_6       =  awb_meas.xyRange_param[6].NorrangeY[0];
    awb_cfg_v201->sw_rawawb_nor_y1_6       =  awb_meas.xyRange_param[6].NorrangeY[1];
    awb_cfg_v201->sw_rawawb_big_x0_6       =  awb_meas.xyRange_param[6].SperangeX[0];
    awb_cfg_v201->sw_rawawb_big_x1_6       =  awb_meas.xyRange_param[6].SperangeX[1];
    awb_cfg_v201->sw_rawawb_big_y0_6       =  awb_meas.xyRange_param[6].SperangeY[0];
    awb_cfg_v201->sw_rawawb_big_y1_6       =  awb_meas.xyRange_param[6].SperangeY[1];
    awb_cfg_v201->sw_rawawb_pre_wbgain_inv_r       =  awb_meas.pre_wbgain_inv_r;
    awb_cfg_v201->sw_rawawb_pre_wbgain_inv_g       =  awb_meas.pre_wbgain_inv_g;
    awb_cfg_v201->sw_rawawb_pre_wbgain_inv_b       =  awb_meas.pre_wbgain_inv_b;
    awb_cfg_v201->sw_rawawb_exc_wp_region0_excen0     =  awb_meas.excludeWpRange[0].excludeEnable[RK_AIQ_AWB_XY_TYPE_NORMAL_V201];
    awb_cfg_v201->sw_rawawb_exc_wp_region0_excen1     =  awb_meas.excludeWpRange[0].excludeEnable[RK_AIQ_AWB_XY_TYPE_BIG_V201];
    //awb_cfg_v201->sw_rawawb_exc_wp_region0_measen   =  awb_meas.excludeWpRange[0].measureEnable;
    awb_cfg_v201->sw_rawawb_exc_wp_region0_domain        =  awb_meas.excludeWpRange[0].domain;
    awb_cfg_v201->sw_rawawb_exc_wp_region0_xu0          =  awb_meas.excludeWpRange[0].xu[0];
    awb_cfg_v201->sw_rawawb_exc_wp_region0_xu1          =  awb_meas.excludeWpRange[0].xu[1];
    awb_cfg_v201->sw_rawawb_exc_wp_region0_yv0          =  awb_meas.excludeWpRange[0].yv[0];
    awb_cfg_v201->sw_rawawb_exc_wp_region0_yv1          =  awb_meas.excludeWpRange[0].yv[1];
    awb_cfg_v201->sw_rawawb_exc_wp_region1_excen0     =  awb_meas.excludeWpRange[1].excludeEnable[RK_AIQ_AWB_XY_TYPE_NORMAL_V201];
    awb_cfg_v201->sw_rawawb_exc_wp_region1_excen1     =  awb_meas.excludeWpRange[1].excludeEnable[RK_AIQ_AWB_XY_TYPE_BIG_V201];
    //awb_cfg_v201->sw_rawawb_exc_wp_region1_measen   =  awb_meas.excludeWpRange[1].measureEnable;
    awb_cfg_v201->sw_rawawb_exc_wp_region1_domain        =  awb_meas.excludeWpRange[1].domain;
    awb_cfg_v201->sw_rawawb_exc_wp_region1_xu0          =  awb_meas.excludeWpRange[1].xu[0];
    awb_cfg_v201->sw_rawawb_exc_wp_region1_xu1          =  awb_meas.excludeWpRange[1].xu[1];
    awb_cfg_v201->sw_rawawb_exc_wp_region1_yv0          =  awb_meas.excludeWpRange[1].yv[0];
    awb_cfg_v201->sw_rawawb_exc_wp_region1_yv1          =  awb_meas.excludeWpRange[1].yv[1];
    awb_cfg_v201->sw_rawawb_exc_wp_region2_excen0     =  awb_meas.excludeWpRange[2].excludeEnable[RK_AIQ_AWB_XY_TYPE_NORMAL_V201];
    awb_cfg_v201->sw_rawawb_exc_wp_region2_excen1     =  awb_meas.excludeWpRange[2].excludeEnable[RK_AIQ_AWB_XY_TYPE_BIG_V201];
    //awb_cfg_v201->sw_rawawb_exc_wp_region2_measen   =  awb_meas.excludeWpRange[2].measureEnable;
    awb_cfg_v201->sw_rawawb_exc_wp_region2_domain        =  awb_meas.excludeWpRange[2].domain;
    awb_cfg_v201->sw_rawawb_exc_wp_region2_xu0          =  awb_meas.excludeWpRange[2].xu[0];
    awb_cfg_v201->sw_rawawb_exc_wp_region2_xu1          =  awb_meas.excludeWpRange[2].xu[1];
    awb_cfg_v201->sw_rawawb_exc_wp_region2_yv0          =  awb_meas.excludeWpRange[2].yv[0];
    awb_cfg_v201->sw_rawawb_exc_wp_region2_yv1          =  awb_meas.excludeWpRange[2].yv[1];
    awb_cfg_v201->sw_rawawb_exc_wp_region3_excen0     =  awb_meas.excludeWpRange[3].excludeEnable[RK_AIQ_AWB_XY_TYPE_NORMAL_V201];
    awb_cfg_v201->sw_rawawb_exc_wp_region3_excen1     =  awb_meas.excludeWpRange[3].excludeEnable[RK_AIQ_AWB_XY_TYPE_BIG_V201];
    //awb_cfg_v201->sw_rawawb_exc_wp_region3_measen   =  awb_meas.excludeWpRange[3].measureEnable;
    awb_cfg_v201->sw_rawawb_exc_wp_region3_domain        =  awb_meas.excludeWpRange[3].domain;
    awb_cfg_v201->sw_rawawb_exc_wp_region3_xu0          =  awb_meas.excludeWpRange[3].xu[0];
    awb_cfg_v201->sw_rawawb_exc_wp_region3_xu1          =  awb_meas.excludeWpRange[3].xu[1];
    awb_cfg_v201->sw_rawawb_exc_wp_region3_yv0          =  awb_meas.excludeWpRange[3].yv[0];
    awb_cfg_v201->sw_rawawb_exc_wp_region3_yv1          =  awb_meas.excludeWpRange[3].yv[1];
    awb_cfg_v201->sw_rawawb_exc_wp_region4_excen0     =  awb_meas.excludeWpRange[4].excludeEnable[RK_AIQ_AWB_XY_TYPE_NORMAL_V201];
    awb_cfg_v201->sw_rawawb_exc_wp_region4_excen1     =  awb_meas.excludeWpRange[4].excludeEnable[RK_AIQ_AWB_XY_TYPE_BIG_V201];
    //awb_cfg_v201->sw_rawawb_exc_wp_region4_measen   =  awb_meas.excludeWpRange[4].measureEnable;
    awb_cfg_v201->sw_rawawb_exc_wp_region4_domain        =  awb_meas.excludeWpRange[4].domain;
    awb_cfg_v201->sw_rawawb_exc_wp_region4_xu0          =  awb_meas.excludeWpRange[4].xu[0];
    awb_cfg_v201->sw_rawawb_exc_wp_region4_xu1          =  awb_meas.excludeWpRange[4].xu[1];
    awb_cfg_v201->sw_rawawb_exc_wp_region4_yv0          =  awb_meas.excludeWpRange[4].yv[0];
    awb_cfg_v201->sw_rawawb_exc_wp_region4_yv1          =  awb_meas.excludeWpRange[4].yv[1];
    awb_cfg_v201->sw_rawawb_exc_wp_region5_excen0     =  awb_meas.excludeWpRange[5].excludeEnable[RK_AIQ_AWB_XY_TYPE_NORMAL_V201];
    awb_cfg_v201->sw_rawawb_exc_wp_region5_excen1     =  awb_meas.excludeWpRange[5].excludeEnable[RK_AIQ_AWB_XY_TYPE_BIG_V201];
    //awb_cfg_v201->sw_rawawb_exc_wp_region5_measen   =  awb_meas.excludeWpRange[5].measureEnable;
    awb_cfg_v201->sw_rawawb_exc_wp_region5_domain        =  awb_meas.excludeWpRange[5].domain;
    awb_cfg_v201->sw_rawawb_exc_wp_region5_xu0          =  awb_meas.excludeWpRange[5].xu[0];
    awb_cfg_v201->sw_rawawb_exc_wp_region5_xu1          =  awb_meas.excludeWpRange[5].xu[1];
    awb_cfg_v201->sw_rawawb_exc_wp_region5_yv0          =  awb_meas.excludeWpRange[5].yv[0];
    awb_cfg_v201->sw_rawawb_exc_wp_region5_yv1          =  awb_meas.excludeWpRange[5].yv[1];
    awb_cfg_v201->sw_rawawb_exc_wp_region6_excen0     =  awb_meas.excludeWpRange[6].excludeEnable[RK_AIQ_AWB_XY_TYPE_NORMAL_V201];
    awb_cfg_v201->sw_rawawb_exc_wp_region6_excen1     =  awb_meas.excludeWpRange[6].excludeEnable[RK_AIQ_AWB_XY_TYPE_BIG_V201];
    //awb_cfg_v201->sw_rawawb_exc_wp_region6_measen   =  awb_meas.excludeWpRange[6].measureEnable;
    awb_cfg_v201->sw_rawawb_exc_wp_region6_domain        =  awb_meas.excludeWpRange[6].domain;
    awb_cfg_v201->sw_rawawb_exc_wp_region6_xu0          =  awb_meas.excludeWpRange[6].xu[0];
    awb_cfg_v201->sw_rawawb_exc_wp_region6_xu1          =  awb_meas.excludeWpRange[6].xu[1];
    awb_cfg_v201->sw_rawawb_exc_wp_region6_yv0          =  awb_meas.excludeWpRange[6].yv[0];
    awb_cfg_v201->sw_rawawb_exc_wp_region6_yv1          =  awb_meas.excludeWpRange[6].yv[1];
    awb_cfg_v201->sw_rawawb_wp_luma_weicurve_y0     =   awb_meas.wpDiffwei_y[0];
    awb_cfg_v201->sw_rawawb_wp_luma_weicurve_y1     =   awb_meas.wpDiffwei_y[1];
    awb_cfg_v201->sw_rawawb_wp_luma_weicurve_y2     =   awb_meas.wpDiffwei_y[2];
    awb_cfg_v201->sw_rawawb_wp_luma_weicurve_y3     =   awb_meas.wpDiffwei_y[3];
    awb_cfg_v201->sw_rawawb_wp_luma_weicurve_y4     =   awb_meas.wpDiffwei_y[4];
    awb_cfg_v201->sw_rawawb_wp_luma_weicurve_y5     =   awb_meas.wpDiffwei_y[5];
    awb_cfg_v201->sw_rawawb_wp_luma_weicurve_y6     =   awb_meas.wpDiffwei_y[6];
    awb_cfg_v201->sw_rawawb_wp_luma_weicurve_y7     =   awb_meas.wpDiffwei_y[7];
    awb_cfg_v201->sw_rawawb_wp_luma_weicurve_y8     =   awb_meas.wpDiffwei_y[8];
    awb_cfg_v201->sw_rawawb_wp_luma_weicurve_w0     = awb_meas.wpDiffwei_w[0];
    awb_cfg_v201->sw_rawawb_wp_luma_weicurve_w1     = awb_meas.wpDiffwei_w[1];
    awb_cfg_v201->sw_rawawb_wp_luma_weicurve_w2     = awb_meas.wpDiffwei_w[2];
    awb_cfg_v201->sw_rawawb_wp_luma_weicurve_w3     = awb_meas.wpDiffwei_w[3];
    awb_cfg_v201->sw_rawawb_wp_luma_weicurve_w4     = awb_meas.wpDiffwei_w[4];
    awb_cfg_v201->sw_rawawb_wp_luma_weicurve_w5     = awb_meas.wpDiffwei_w[5];
    awb_cfg_v201->sw_rawawb_wp_luma_weicurve_w6     = awb_meas.wpDiffwei_w[6];
    awb_cfg_v201->sw_rawawb_wp_luma_weicurve_w7     = awb_meas.wpDiffwei_w[7];
    awb_cfg_v201->sw_rawawb_wp_luma_weicurve_w8     = awb_meas.wpDiffwei_w[8];

    for (int i = 0; i < RK_AIQ_AWB_GRID_NUM_TOTAL; i++) {
        awb_cfg_v201->sw_rawawb_wp_blk_wei_w[i]          = awb_meas.blkWeight[i];
    }

    awb_cfg_v201->sw_rawawb_blk_rtdw_measure_en =  awb_meas.blk_rtdw_measure_en;






}

void
Isp21Params::convertAiqRawnrToIsp21Params(struct isp21_isp_params_cfg& isp_cfg,
        rk_aiq_isp_baynr_v21_t& rawnr)
{

}

void
Isp21Params::convertAiqTnrToIsp21Params(struct isp21_isp_params_cfg& isp_cfg,
                                        rk_aiq_isp_bay3d_v21_t& tnr)
{

}

void
Isp21Params::convertAiqUvnrToIsp21Params(struct isp21_isp_params_cfg& isp_cfg,
        rk_aiq_isp_cnr_v21_t& uvnr)
{

}

void
Isp21Params::convertAiqYnrToIsp21Params(struct isp21_isp_params_cfg& isp_cfg,
                                        rk_aiq_isp_ynr_v21_t& ynr)
{

}

void
Isp21Params::convertAiqSharpenToIsp21Params(struct isp21_isp_params_cfg& isp_cfg,
        rk_aiq_isp_sharp_v21_t& sharp)
{

}

XCamReturn
Isp21Params::convertAiqResultsToIsp21Params(struct isp21_isp_params_cfg& isp_cfg,
        SmartPtr<RkAiqIspParamsProxy> aiq_results,
        SmartPtr<RkAiqIspParamsProxy>& last_aiq_results)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    rk_aiq_isp_params_v21_t* isp21_result =
        static_cast<rk_aiq_isp_params_v21_t*>(aiq_results->data().ptr());

    convertAiqHistToIsp20Params<struct isp21_isp_params_cfg>(isp_cfg, isp21_result->hist_meas);
    convertAiqAeToIsp20Params<struct isp21_isp_params_cfg>(isp_cfg, isp21_result->aec_meas);

    // TODO: merge params should be split from ahdr
    /* convertAiqMergeToIsp20Params<struct isp21_isp_params_cfg>(isp_cfg, isp21_result->ahdr_proc_res); */
    convertAiqTmoToIsp21Params(isp_cfg, isp21_result->drc);
    // TODO: blc type has changed
    convertAiqAwbGainToIsp21Params(isp_cfg, isp21_result->awb_gain, aiq_results,
                                   isp21_result->awb_gain_update);
    convertAiqAwbToIsp21Params(isp_cfg, isp21_result->awb_cfg, isp21_result->awb_cfg_update);
    convertAiqLscToIsp20Params<struct isp21_isp_params_cfg>(isp_cfg, isp21_result->lsc);
    convertAiqCcmToIsp21Params(isp_cfg, isp21_result->ccm);
    convertAiqAgammaToIsp20Params<struct isp21_isp_params_cfg>(isp_cfg, isp21_result->agamma);
    convertAiqBlcToIsp21Params(isp_cfg, aiq_results);
    convertAiqDpccToIsp20Params<struct isp21_isp_params_cfg>(isp_cfg, aiq_results);
    convertAiqRawnrToIsp21Params(isp_cfg, isp21_result->rawnr);
    convertAiqTnrToIsp21Params(isp_cfg, isp21_result->tnr);
    convertAiqYnrToIsp21Params(isp_cfg, isp21_result->ynr);
    convertAiqUvnrToIsp21Params(isp_cfg, isp21_result->uvnr);
    convertAiqSharpenToIsp21Params(isp_cfg, isp21_result->sharp);
    convertAiqAfToIsp20Params<struct isp21_isp_params_cfg>(isp_cfg, isp21_result->af_meas, isp21_result->af_cfg_update);
    convertAiqAdehazeToIsp21Params(isp_cfg, isp21_result->adhaz_config);
    convertAiqA3dlutToIsp20Params<struct isp21_isp_params_cfg>(isp_cfg, isp21_result->lut3d);
    if(isp21_result->update_mask & RKAIQ_ISP_LDCH_ID)
        convertAiqAldchToIsp20Params<struct isp21_isp_params_cfg>(isp_cfg, isp21_result->ldch);

    //must be at the end of isp module
    /* convertAiqGainToIsp20Params<struct isp21_isp_params_cfg>(isp_cfg, isp21_result->gain_config); */
    convertAiqGicToIsp21Params(isp_cfg, isp21_result->gic);
    convertAiqAdemosaicToIsp20Params<struct isp21_isp_params_cfg>(isp_cfg, aiq_results);
    convertAiqIeToIsp20Params<struct isp21_isp_params_cfg>(isp_cfg, isp21_result->ie);
    convertAiqCpToIsp20Params<struct isp21_isp_params_cfg>(isp_cfg, isp21_result->cp);
    last_aiq_results = aiq_results;

    return ret;

}

};
