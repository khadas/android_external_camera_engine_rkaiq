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

#include <cstring>
#include "Isp20Params.h"

namespace RkCam {

void
Isp20Params::convertAiqAeToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
                                       const rk_aiq_isp_aec_meas_t& aec_meas)
{
    // TODO
    memcpy(&isp_cfg.meas.rawae3, &aec_meas.rawae3, sizeof(aec_meas.rawae3));
    memcpy(&isp_cfg.meas.rawae1, &aec_meas.rawae1, sizeof(aec_meas.rawae1));
    memcpy(&isp_cfg.meas.rawae2, &aec_meas.rawae2, sizeof(aec_meas.rawae2));
    memcpy(&isp_cfg.meas.rawae0, &aec_meas.rawae0, sizeof(aec_meas.rawae0));
    memcpy(&isp_cfg.meas.yuvae, &aec_meas.yuvae, sizeof(aec_meas.yuvae));
    /*
     *     LOGD("xuhf-debug: hist_meas-isp_cfg size: [%dx%d]-[%dx%d]-[%dx%d]-[%dx%d]\n",
     *                     sizeof(aec_meas.rawae3),
     *                     sizeof(isp_cfg.meas.rawae3),
     *                     sizeof(aec_meas.rawae1),
     *                     sizeof(isp_cfg.meas.rawae1),
     *                     sizeof(aec_meas.rawae2),
     *                     sizeof(isp_cfg.meas.rawae2),
     *                     sizeof(aec_meas.rawae0),
     *                     sizeof(isp_cfg.meas.rawae0));
     *
     *     LOGD("xuhf-debug: aec_meas: win size: [%dx%d]-[%dx%d]-[%dx%d]-[%dx%d]\n",
     *            aec_meas.rawae0.win.h_size,
     *            aec_meas.rawae0.win.v_size,
     *            aec_meas.rawae3.win.h_size,
     *            aec_meas.rawae3.win.v_size,
     *            aec_meas.rawae1.win.h_size,
     *            aec_meas.rawae1.win.v_size,
     *            aec_meas.rawae2.win.h_size,
     *            aec_meas.rawae2.win.v_size);
     *
     *     LOGD("xuhf-debug: isp_cfg: win size: [%dx%d]-[%dx%d]-[%dx%d]-[%dx%d]\n",
     *            isp_cfg.meas.rawae0.win.h_size,
     *            isp_cfg.meas.rawae0.win.v_size,
     *            isp_cfg.meas.rawae3.win.h_size,
     *            isp_cfg.meas.rawae3.win.v_size,
     *            isp_cfg.meas.rawae1.win.h_size,
     *            isp_cfg.meas.rawae1.win.v_size,
     *            isp_cfg.meas.rawae2.win.h_size,
     *            isp_cfg.meas.rawae2.win.v_size);
     */
}

void
Isp20Params::convertAiqBigHstWndSize(struct isp2x_rawhistbig_cfg& hst_big)
{
    u32 wnd_num_idx = 0;
    const u32 hst_wnd_num[] = {
        5, 5, 15, 15
    };
    wnd_num_idx = hst_big.wnd_num;

    hst_big.win.h_size = hst_big.win.h_size / hst_wnd_num[wnd_num_idx] - 1;
    hst_big.win.v_size = hst_big.win.v_size / hst_wnd_num[wnd_num_idx] - 1;

}

void
Isp20Params::convertAiqLiteHstWndSize(struct isp2x_rawhistlite_cfg& hst_lite)
{

    hst_lite.win.h_size = hst_lite.win.h_size / 5 - 1;
    hst_lite.win.v_size = hst_lite.win.v_size / 5 - 1;

}

void
Isp20Params::convertAiqSiHstWndSize(struct isp2x_sihst_cfg& sihst)
{
    u32 wnd_num_idx = 0;
    const u32 hst_wnd_num[] = {
        5, 9, 15, 15
    };
    wnd_num_idx = sihst.wnd_num;
    for(int i = 0; i < ISP2X_SIHIST_WIN_NUM; i++) {
        sihst.win_cfg[i].win.h_size = sihst.win_cfg[i].win.h_size / hst_wnd_num[wnd_num_idx] - 1;
        sihst.win_cfg[i].win.v_size = sihst.win_cfg[i].win.v_size / hst_wnd_num[wnd_num_idx] - 1;
    }

}


void
Isp20Params::convertAiqHistToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
        const rk_aiq_isp_hist_meas_t& hist_meas)
{
    memcpy(&isp_cfg.meas.rawhist3, &hist_meas.rawhist3, sizeof(hist_meas.rawhist3));
    // convertAiqBigHstWndSize(isp_cfg.meas.rawhist3);
    memcpy(&isp_cfg.meas.rawhist1, &hist_meas.rawhist1, sizeof(hist_meas.rawhist1));
    // convertAiqBigHstWndSize(isp_cfg.meas.rawhist1);
    memcpy(&isp_cfg.meas.rawhist2, &hist_meas.rawhist2, sizeof(hist_meas.rawhist2));
    // convertAiqBigHstWndSize(isp_cfg.meas.rawhist2);
    memcpy(&isp_cfg.meas.rawhist0, &hist_meas.rawhist0, sizeof(hist_meas.rawhist0));
    // convertAiqLiteHstWndSize(isp_cfg.meas.rawhist0);
    memcpy(&isp_cfg.meas.sihst, &hist_meas.sihist, sizeof(hist_meas.sihist));
    // convertAiqSiHstWndSize(isp_cfg.meas.sihst);

    /*
     *     LOGD("xuhf-debug: hist_meas-isp_cfg size: [%dx%d]-[%dx%d]-[%dx%d]-[%dx%d]\n",
     *                     sizeof(hist_meas.rawhist3),
     *                     sizeof(isp_cfg.meas.rawhist3),
     *                     sizeof(hist_meas.rawhist1),
     *                     sizeof(isp_cfg.meas.rawhist1),
     *                     sizeof(hist_meas.rawhist2),
     *                     sizeof(isp_cfg.meas.rawhist2),
     *                     sizeof(hist_meas.rawhist0),
     *                     sizeof(isp_cfg.meas.rawhist0));
     *
     *     LOGD("xuhf-debug: hist_meas: hist win size: [%dx%d]-[%dx%d]-[%dx%d]-[%dx%d]\n",
     *                     hist_meas.rawhist0.win.h_size,
     *                     hist_meas.rawhist0.win.v_size,
     *                     hist_meas.rawhist3.win.h_size,
     *                     hist_meas.rawhist3.win.v_size,
     *                     hist_meas.rawhist1.win.h_size,
     *                     hist_meas.rawhist1.win.v_size,
     *                     hist_meas.rawhist2.win.h_size,
     *                     hist_meas.rawhist2.win.v_size);
     *
     *     LOGD("xuhf-debug: isp_cfg: hist win size: [%dx%d]-[%dx%d]-[%dx%d]-[%dx%d]\n",
     *                     isp_cfg.meas.rawhist0.win.h_size,
     *                     isp_cfg.meas.rawhist0.win.v_size,
     *                     isp_cfg.meas.rawhist3.win.h_size,
     *                     isp_cfg.meas.rawhist3.win.v_size,
     *                     isp_cfg.meas.rawhist1.win.h_size,
     *                     isp_cfg.meas.rawhist1.win.v_size,
     *                     isp_cfg.meas.rawhist2.win.h_size,
     *                     isp_cfg.meas.rawhist2.win.v_size);
     */
}



























void
Isp20Params::convertAiqAwbToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
                                        const rk_aiq_awb_stat_cfg_v200_t& awb_meas)
{

    if(awb_meas.awbEnable) {
        isp_cfg.module_ens |= ISP2X_MODULE_RAWAWB;
    }
    isp_cfg.module_en_update |= ISP2X_MODULE_RAWAWB;
    isp_cfg.module_cfg_update |= ISP2X_MODULE_RAWAWB;

    struct isp2x_rawawb_meas_cfg * awb_cfg_v200 = &isp_cfg.meas.rawawb;
    awb_cfg_v200->rawawb_sel                        =    awb_meas.frameChoose;
    awb_cfg_v200->sw_rawawb_xy_en                   =    awb_meas.xyDetectionEnable;
    awb_cfg_v200->sw_rawawb_uv_en                   =    awb_meas.uvDetectionEnable;
    /* TODO: bypass the lsc modude before the rawawb*/
    awb_cfg_v200->sw_rawlsc_bypass_en               =     true;
    awb_cfg_v200->sw_rawawb_3dyuv_ls_idx0           =     awb_meas.threeDyuvIllu[0];
    awb_cfg_v200->sw_rawawb_3dyuv_ls_idx1           =     awb_meas.threeDyuvIllu[1];
    awb_cfg_v200->sw_rawawb_3dyuv_ls_idx2           =     awb_meas.threeDyuvIllu[2];
    awb_cfg_v200->sw_rawawb_3dyuv_ls_idx3           =     awb_meas.threeDyuvIllu[3];
    awb_cfg_v200->sw_rawawb_blk_measure_mode        =     awb_meas.blkMeasureMode;
    awb_cfg_v200->sw_rawawb_light_num               =    awb_meas.lightNum;
    awb_cfg_v200->sw_rawawb_h_offs                  =    awb_meas.windowSet[0];
    awb_cfg_v200->sw_rawawb_v_offs                  =    awb_meas.windowSet[1];
    awb_cfg_v200->sw_rawawb_h_size                  =    awb_meas.windowSet[2];
    awb_cfg_v200->sw_rawawb_v_size                  =    awb_meas.windowSet[3];
    switch(awb_meas.dsMode) {
    case RK_AIQ_AWB_DS_4X4:
        awb_cfg_v200->sw_rawawb_wind_size = 0;
        break;
    default:
        awb_cfg_v200->sw_rawawb_wind_size = 1;
    }
    awb_cfg_v200->sw_rawawb_r_max                   =    awb_meas.maxR;
    awb_cfg_v200->sw_rawawb_g_max                   =    awb_meas.maxG;
    awb_cfg_v200->sw_rawawb_b_max                   =    awb_meas.maxB;
    awb_cfg_v200->sw_rawawb_y_max                   =    awb_meas.maxY;
    awb_cfg_v200->sw_rawawb_r_min                   =    awb_meas.minR;
    awb_cfg_v200->sw_rawawb_g_min                   =    awb_meas.minG;
    awb_cfg_v200->sw_rawawb_b_min                   =    awb_meas.minB;
    awb_cfg_v200->sw_rawawb_y_min                   =    awb_meas.minY;
    awb_cfg_v200->sw_rawawb_c_range                 =     awb_meas.rgb2yuv_c_range;
    awb_cfg_v200->sw_rawawb_y_range                 =    awb_meas.rgb2yuv_y_range;
    awb_cfg_v200->sw_rawawb_coeff_y_r               =    awb_meas.rgb2yuv_matrix[0];
    awb_cfg_v200->sw_rawawb_coeff_y_g               =    awb_meas.rgb2yuv_matrix[1];
    awb_cfg_v200->sw_rawawb_coeff_y_b               =    awb_meas.rgb2yuv_matrix[2];
    awb_cfg_v200->sw_rawawb_coeff_u_r               =    awb_meas.rgb2yuv_matrix[3];
    awb_cfg_v200->sw_rawawb_coeff_u_g               =    awb_meas.rgb2yuv_matrix[4];
    awb_cfg_v200->sw_rawawb_coeff_u_b               =    awb_meas.rgb2yuv_matrix[5];
    awb_cfg_v200->sw_rawawb_coeff_v_r               =    awb_meas.rgb2yuv_matrix[6];
    awb_cfg_v200->sw_rawawb_coeff_v_g               =    awb_meas.rgb2yuv_matrix[7];
    awb_cfg_v200->sw_rawawb_coeff_v_b               =    awb_meas.rgb2yuv_matrix[8];
    //uv
    awb_cfg_v200->sw_rawawb_vertex0_u_0             =    awb_meas.uvRange_param[0].pu_region[0];
    awb_cfg_v200->sw_rawawb_vertex0_v_0             =    awb_meas.uvRange_param[0].pv_region[0];
    awb_cfg_v200->sw_rawawb_vertex1_u_0             =    awb_meas.uvRange_param[0].pu_region[1];
    awb_cfg_v200->sw_rawawb_vertex1_v_0             =    awb_meas.uvRange_param[0].pv_region[1];
    awb_cfg_v200->sw_rawawb_vertex2_u_0             =    awb_meas.uvRange_param[0].pu_region[2];
    awb_cfg_v200->sw_rawawb_vertex2_v_0             =    awb_meas.uvRange_param[0].pv_region[2];
    awb_cfg_v200->sw_rawawb_vertex3_u_0             =    awb_meas.uvRange_param[0].pu_region[3];
    awb_cfg_v200->sw_rawawb_vertex3_v_0             =    awb_meas.uvRange_param[0].pv_region[3];
    awb_cfg_v200->sw_rawawb_islope01_0              =    awb_meas.uvRange_param[0].slope_inv[0];
    awb_cfg_v200->sw_rawawb_islope12_0              =    awb_meas.uvRange_param[0].slope_inv[1];
    awb_cfg_v200->sw_rawawb_islope23_0              =    awb_meas.uvRange_param[0].slope_inv[2];
    awb_cfg_v200->sw_rawawb_islope30_0              =    awb_meas.uvRange_param[0].slope_inv[3];
    awb_cfg_v200->sw_rawawb_vertex0_u_1             =    awb_meas.uvRange_param[1].pu_region[0];
    awb_cfg_v200->sw_rawawb_vertex0_v_1             =    awb_meas.uvRange_param[1].pv_region[0];
    awb_cfg_v200->sw_rawawb_vertex1_u_1             =    awb_meas.uvRange_param[1].pu_region[1];
    awb_cfg_v200->sw_rawawb_vertex1_v_1             =    awb_meas.uvRange_param[1].pv_region[1];
    awb_cfg_v200->sw_rawawb_vertex2_u_1             =    awb_meas.uvRange_param[1].pu_region[2];
    awb_cfg_v200->sw_rawawb_vertex2_v_1             =    awb_meas.uvRange_param[1].pv_region[2];
    awb_cfg_v200->sw_rawawb_vertex3_u_1             =    awb_meas.uvRange_param[1].pu_region[3];
    awb_cfg_v200->sw_rawawb_vertex3_v_1             =    awb_meas.uvRange_param[1].pv_region[3];
    awb_cfg_v200->sw_rawawb_islope01_1              =    awb_meas.uvRange_param[1].slope_inv[0];
    awb_cfg_v200->sw_rawawb_islope12_1              =    awb_meas.uvRange_param[1].slope_inv[1];
    awb_cfg_v200->sw_rawawb_islope23_1              =    awb_meas.uvRange_param[1].slope_inv[2];
    awb_cfg_v200->sw_rawawb_islope30_1              =    awb_meas.uvRange_param[1].slope_inv[3];
    awb_cfg_v200->sw_rawawb_vertex0_u_2             =    awb_meas.uvRange_param[2].pu_region[0];
    awb_cfg_v200->sw_rawawb_vertex0_v_2             =    awb_meas.uvRange_param[2].pv_region[0];
    awb_cfg_v200->sw_rawawb_vertex1_u_2             =    awb_meas.uvRange_param[2].pu_region[1];
    awb_cfg_v200->sw_rawawb_vertex1_v_2             =    awb_meas.uvRange_param[2].pv_region[1];
    awb_cfg_v200->sw_rawawb_vertex2_u_2             =    awb_meas.uvRange_param[2].pu_region[2];
    awb_cfg_v200->sw_rawawb_vertex2_v_2             =    awb_meas.uvRange_param[2].pv_region[2];
    awb_cfg_v200->sw_rawawb_vertex3_u_2             =    awb_meas.uvRange_param[2].pu_region[3];
    awb_cfg_v200->sw_rawawb_vertex3_v_2             =    awb_meas.uvRange_param[2].pv_region[3];
    awb_cfg_v200->sw_rawawb_islope01_2              =    awb_meas.uvRange_param[2].slope_inv[0];
    awb_cfg_v200->sw_rawawb_islope12_2              =    awb_meas.uvRange_param[2].slope_inv[1];
    awb_cfg_v200->sw_rawawb_islope23_2              =    awb_meas.uvRange_param[2].slope_inv[2];
    awb_cfg_v200->sw_rawawb_islope30_2              =    awb_meas.uvRange_param[2].slope_inv[3];
    awb_cfg_v200->sw_rawawb_vertex0_u_3             =    awb_meas.uvRange_param[3].pu_region[0];
    awb_cfg_v200->sw_rawawb_vertex0_v_3             =    awb_meas.uvRange_param[3].pv_region[0];
    awb_cfg_v200->sw_rawawb_vertex1_u_3             =    awb_meas.uvRange_param[3].pu_region[1];
    awb_cfg_v200->sw_rawawb_vertex1_v_3             =    awb_meas.uvRange_param[3].pv_region[1];
    awb_cfg_v200->sw_rawawb_vertex2_u_3             =    awb_meas.uvRange_param[3].pu_region[2];
    awb_cfg_v200->sw_rawawb_vertex2_v_3             =    awb_meas.uvRange_param[3].pv_region[2];
    awb_cfg_v200->sw_rawawb_vertex3_u_3             =    awb_meas.uvRange_param[3].pu_region[3];
    awb_cfg_v200->sw_rawawb_vertex3_v_3             =    awb_meas.uvRange_param[3].pv_region[3];
    awb_cfg_v200->sw_rawawb_islope01_3              =    awb_meas.uvRange_param[3].slope_inv[0];
    awb_cfg_v200->sw_rawawb_islope12_3              =    awb_meas.uvRange_param[3].slope_inv[1];
    awb_cfg_v200->sw_rawawb_islope23_3              =    awb_meas.uvRange_param[3].slope_inv[2];
    awb_cfg_v200->sw_rawawb_islope30_3              =    awb_meas.uvRange_param[3].slope_inv[3];
    awb_cfg_v200->sw_rawawb_vertex0_u_4             =    awb_meas.uvRange_param[4].pu_region[0];
    awb_cfg_v200->sw_rawawb_vertex0_v_4             =    awb_meas.uvRange_param[4].pv_region[0];
    awb_cfg_v200->sw_rawawb_vertex1_u_4             =    awb_meas.uvRange_param[4].pu_region[1];
    awb_cfg_v200->sw_rawawb_vertex1_v_4             =    awb_meas.uvRange_param[4].pv_region[1];
    awb_cfg_v200->sw_rawawb_vertex2_u_4             =    awb_meas.uvRange_param[4].pu_region[2];
    awb_cfg_v200->sw_rawawb_vertex2_v_4             =    awb_meas.uvRange_param[4].pv_region[2];
    awb_cfg_v200->sw_rawawb_vertex3_u_4             =    awb_meas.uvRange_param[4].pu_region[3];
    awb_cfg_v200->sw_rawawb_vertex3_v_4             =    awb_meas.uvRange_param[4].pv_region[3];
    awb_cfg_v200->sw_rawawb_islope01_4              =    awb_meas.uvRange_param[4].slope_inv[0];
    awb_cfg_v200->sw_rawawb_islope12_4              =    awb_meas.uvRange_param[4].slope_inv[1];
    awb_cfg_v200->sw_rawawb_islope23_4              =    awb_meas.uvRange_param[4].slope_inv[2];
    awb_cfg_v200->sw_rawawb_islope30_4              =    awb_meas.uvRange_param[4].slope_inv[3];
    awb_cfg_v200->sw_rawawb_vertex0_u_5             =    awb_meas.uvRange_param[5].pu_region[0];
    awb_cfg_v200->sw_rawawb_vertex0_v_5             =    awb_meas.uvRange_param[5].pv_region[0];
    awb_cfg_v200->sw_rawawb_vertex1_u_5             =    awb_meas.uvRange_param[5].pu_region[1];
    awb_cfg_v200->sw_rawawb_vertex1_v_5             =    awb_meas.uvRange_param[5].pv_region[1];
    awb_cfg_v200->sw_rawawb_vertex2_u_5             =    awb_meas.uvRange_param[5].pu_region[2];
    awb_cfg_v200->sw_rawawb_vertex2_v_5             =    awb_meas.uvRange_param[5].pv_region[2];
    awb_cfg_v200->sw_rawawb_vertex3_u_5             =    awb_meas.uvRange_param[5].pu_region[3];
    awb_cfg_v200->sw_rawawb_vertex3_v_5             =    awb_meas.uvRange_param[5].pv_region[3];
    awb_cfg_v200->sw_rawawb_islope01_5              =    awb_meas.uvRange_param[5].slope_inv[0];
    awb_cfg_v200->sw_rawawb_islope12_5              =    awb_meas.uvRange_param[5].slope_inv[1];
    awb_cfg_v200->sw_rawawb_islope23_5              =    awb_meas.uvRange_param[5].slope_inv[2];
    awb_cfg_v200->sw_rawawb_islope30_5              =    awb_meas.uvRange_param[5].slope_inv[3];
    awb_cfg_v200->sw_rawawb_vertex0_u_6             =    awb_meas.uvRange_param[6].pu_region[0];
    awb_cfg_v200->sw_rawawb_vertex0_v_6             =    awb_meas.uvRange_param[6].pv_region[0];
    awb_cfg_v200->sw_rawawb_vertex1_u_6             =    awb_meas.uvRange_param[6].pu_region[1];
    awb_cfg_v200->sw_rawawb_vertex1_v_6             =    awb_meas.uvRange_param[6].pv_region[1];
    awb_cfg_v200->sw_rawawb_vertex2_u_6             =    awb_meas.uvRange_param[6].pu_region[2];
    awb_cfg_v200->sw_rawawb_vertex2_v_6             =    awb_meas.uvRange_param[6].pv_region[2];
    awb_cfg_v200->sw_rawawb_vertex3_u_6             =    awb_meas.uvRange_param[6].pu_region[3];
    awb_cfg_v200->sw_rawawb_vertex3_v_6             =    awb_meas.uvRange_param[6].pv_region[3];
    awb_cfg_v200->sw_rawawb_islope01_6              =    awb_meas.uvRange_param[6].slope_inv[0];
    awb_cfg_v200->sw_rawawb_islope12_6              =    awb_meas.uvRange_param[6].slope_inv[1];
    awb_cfg_v200->sw_rawawb_islope23_6              =    awb_meas.uvRange_param[6].slope_inv[2];
    awb_cfg_v200->sw_rawawb_islope30_6              =    awb_meas.uvRange_param[6].slope_inv[3];
    //yuv
    awb_cfg_v200->sw_rawawb_b_uv_0                  =    awb_meas.yuvRange_param[0].b_uv;
    awb_cfg_v200->sw_rawawb_slope_ydis_0            =    awb_meas.yuvRange_param[0].slope_ydis;
    awb_cfg_v200->sw_rawawb_b_ydis_0                =    awb_meas.yuvRange_param[0].b_ydis;
    awb_cfg_v200->sw_rawawb_slope_vtcuv_0           =    awb_meas.yuvRange_param[0].slope_inv_neg_uv;
    awb_cfg_v200->sw_rawawb_inv_dslope_0            =    awb_meas.yuvRange_param[0].slope_factor_uv;
    awb_cfg_v200->sw_rawawb_b_uv_1                  =    awb_meas.yuvRange_param[1].b_uv;
    awb_cfg_v200->sw_rawawb_slope_ydis_1            =    awb_meas.yuvRange_param[1].slope_ydis;
    awb_cfg_v200->sw_rawawb_b_ydis_1                =    awb_meas.yuvRange_param[1].b_ydis;
    awb_cfg_v200->sw_rawawb_slope_vtcuv_1           =    awb_meas.yuvRange_param[1].slope_inv_neg_uv;
    awb_cfg_v200->sw_rawawb_inv_dslope_1            =    awb_meas.yuvRange_param[1].slope_factor_uv;
    awb_cfg_v200->sw_rawawb_b_uv_2                  =    awb_meas.yuvRange_param[2].b_uv;
    awb_cfg_v200->sw_rawawb_slope_ydis_2            =    awb_meas.yuvRange_param[2].slope_ydis;
    awb_cfg_v200->sw_rawawb_b_ydis_2                =    awb_meas.yuvRange_param[2].b_ydis;
    awb_cfg_v200->sw_rawawb_slope_vtcuv_2           =    awb_meas.yuvRange_param[2].slope_inv_neg_uv;
    awb_cfg_v200->sw_rawawb_inv_dslope_2            =    awb_meas.yuvRange_param[2].slope_factor_uv;
    awb_cfg_v200->sw_rawawb_b_uv_3                  =    awb_meas.yuvRange_param[3].b_uv;
    awb_cfg_v200->sw_rawawb_slope_ydis_3            =    awb_meas.yuvRange_param[3].slope_ydis;
    awb_cfg_v200->sw_rawawb_b_ydis_3                =    awb_meas.yuvRange_param[3].b_ydis;
    awb_cfg_v200->sw_rawawb_slope_vtcuv_3           =    awb_meas.yuvRange_param[3].slope_inv_neg_uv;
    awb_cfg_v200->sw_rawawb_inv_dslope_3            =    awb_meas.yuvRange_param[3].slope_factor_uv;
    awb_cfg_v200->sw_rawawb_ref_u                   =    awb_meas.yuvRange_param[0].ref_u;
    awb_cfg_v200->sw_rawawb_ref_v_0                 =    awb_meas.yuvRange_param[0].ref_v;
    awb_cfg_v200->sw_rawawb_ref_v_1                 =    awb_meas.yuvRange_param[1].ref_v;
    awb_cfg_v200->sw_rawawb_ref_v_2                 =    awb_meas.yuvRange_param[2].ref_v;
    awb_cfg_v200->sw_rawawb_ref_v_3                 =    awb_meas.yuvRange_param[3].ref_v;
    awb_cfg_v200->sw_rawawb_dis0_0                  =    awb_meas.yuvRange_param[0].dis[0];
    awb_cfg_v200->sw_rawawb_dis1_0                  =    awb_meas.yuvRange_param[0].dis[1];
    awb_cfg_v200->sw_rawawb_dis2_0                  =    awb_meas.yuvRange_param[0].dis[2];
    awb_cfg_v200->sw_rawawb_dis3_0                  =    awb_meas.yuvRange_param[0].dis[3];
    awb_cfg_v200->sw_rawawb_dis4_0                  =    awb_meas.yuvRange_param[0].dis[4];
    awb_cfg_v200->sw_rawawb_dis5_0                  =    awb_meas.yuvRange_param[0].dis[5];
    awb_cfg_v200->sw_rawawb_th0_0                   =    awb_meas.yuvRange_param[0].th[0];
    awb_cfg_v200->sw_rawawb_th1_0                   =    awb_meas.yuvRange_param[0].th[1];
    awb_cfg_v200->sw_rawawb_th2_0                   =    awb_meas.yuvRange_param[0].th[2];
    awb_cfg_v200->sw_rawawb_th3_0                   =    awb_meas.yuvRange_param[0].th[3];
    awb_cfg_v200->sw_rawawb_th4_0                   =    awb_meas.yuvRange_param[0].th[4];
    awb_cfg_v200->sw_rawawb_th5_0                   =    awb_meas.yuvRange_param[0].th[5];
    awb_cfg_v200->sw_rawawb_dis0_1                  =    awb_meas.yuvRange_param[1].dis[0];
    awb_cfg_v200->sw_rawawb_dis1_1                  =    awb_meas.yuvRange_param[1].dis[1];
    awb_cfg_v200->sw_rawawb_dis2_1                  =    awb_meas.yuvRange_param[1].dis[2];
    awb_cfg_v200->sw_rawawb_dis3_1                  =    awb_meas.yuvRange_param[1].dis[3];
    awb_cfg_v200->sw_rawawb_dis4_1                  =    awb_meas.yuvRange_param[1].dis[4];
    awb_cfg_v200->sw_rawawb_dis5_1                  =    awb_meas.yuvRange_param[1].dis[5];
    awb_cfg_v200->sw_rawawb_th0_1                   =    awb_meas.yuvRange_param[1].th[0];
    awb_cfg_v200->sw_rawawb_th1_1                   =    awb_meas.yuvRange_param[1].th[1];
    awb_cfg_v200->sw_rawawb_th2_1                   =    awb_meas.yuvRange_param[1].th[2];
    awb_cfg_v200->sw_rawawb_th3_1                   =    awb_meas.yuvRange_param[1].th[3];
    awb_cfg_v200->sw_rawawb_th4_1                   =    awb_meas.yuvRange_param[1].th[4];
    awb_cfg_v200->sw_rawawb_th5_1                   =    awb_meas.yuvRange_param[1].th[5];
    awb_cfg_v200->sw_rawawb_dis0_2                  =    awb_meas.yuvRange_param[2].dis[0];
    awb_cfg_v200->sw_rawawb_dis1_2                  =    awb_meas.yuvRange_param[2].dis[1];
    awb_cfg_v200->sw_rawawb_dis2_2                  =    awb_meas.yuvRange_param[2].dis[2];
    awb_cfg_v200->sw_rawawb_dis3_2                  =    awb_meas.yuvRange_param[2].dis[3];
    awb_cfg_v200->sw_rawawb_dis4_2                  =    awb_meas.yuvRange_param[2].dis[4];
    awb_cfg_v200->sw_rawawb_dis5_2                  =    awb_meas.yuvRange_param[2].dis[5];
    awb_cfg_v200->sw_rawawb_th0_2                   =    awb_meas.yuvRange_param[2].th[0];
    awb_cfg_v200->sw_rawawb_th1_2                   =    awb_meas.yuvRange_param[2].th[1];
    awb_cfg_v200->sw_rawawb_th2_2                   =    awb_meas.yuvRange_param[2].th[2];
    awb_cfg_v200->sw_rawawb_th3_2                   =    awb_meas.yuvRange_param[2].th[3];
    awb_cfg_v200->sw_rawawb_th4_2                   =    awb_meas.yuvRange_param[2].th[4];
    awb_cfg_v200->sw_rawawb_th5_2                   =    awb_meas.yuvRange_param[2].th[5];
    awb_cfg_v200->sw_rawawb_dis0_3                  =    awb_meas.yuvRange_param[3].dis[0];
    awb_cfg_v200->sw_rawawb_dis1_3                  =    awb_meas.yuvRange_param[3].dis[1];
    awb_cfg_v200->sw_rawawb_dis2_3                  =    awb_meas.yuvRange_param[3].dis[2];
    awb_cfg_v200->sw_rawawb_dis3_3                  =    awb_meas.yuvRange_param[3].dis[3];
    awb_cfg_v200->sw_rawawb_dis4_3                  =    awb_meas.yuvRange_param[3].dis[4];
    awb_cfg_v200->sw_rawawb_dis5_3                  =    awb_meas.yuvRange_param[3].dis[5];
    awb_cfg_v200->sw_rawawb_th0_3                   =    awb_meas.yuvRange_param[3].th[0];
    awb_cfg_v200->sw_rawawb_th1_3                   =    awb_meas.yuvRange_param[3].th[1];
    awb_cfg_v200->sw_rawawb_th2_3                   =    awb_meas.yuvRange_param[3].th[2];
    awb_cfg_v200->sw_rawawb_th3_3                   =    awb_meas.yuvRange_param[3].th[3];
    awb_cfg_v200->sw_rawawb_th4_3                   =    awb_meas.yuvRange_param[3].th[4];
    awb_cfg_v200->sw_rawawb_th5_3                   =    awb_meas.yuvRange_param[3].th[5];
    //xy
    awb_cfg_v200->sw_rawawb_wt0                     =    awb_meas.rgb2xy_param.pseudoLuminanceWeight[0];
    awb_cfg_v200->sw_rawawb_wt1                     =    awb_meas.rgb2xy_param.pseudoLuminanceWeight[1];
    awb_cfg_v200->sw_rawawb_wt2                     =    awb_meas.rgb2xy_param.pseudoLuminanceWeight[2];
    awb_cfg_v200->sw_rawawb_mat0_x                  =    awb_meas.rgb2xy_param.rotationMat[0];
    awb_cfg_v200->sw_rawawb_mat1_x                  =    awb_meas.rgb2xy_param.rotationMat[1];
    awb_cfg_v200->sw_rawawb_mat2_x                  =    awb_meas.rgb2xy_param.rotationMat[2];
    awb_cfg_v200->sw_rawawb_mat0_y                  =    awb_meas.rgb2xy_param.rotationMat[3];
    awb_cfg_v200->sw_rawawb_mat1_y                  =    awb_meas.rgb2xy_param.rotationMat[4];
    awb_cfg_v200->sw_rawawb_mat2_y                  =    awb_meas.rgb2xy_param.rotationMat[5];
    awb_cfg_v200->sw_rawawb_nor_x0_0                =    awb_meas.xyRange_param[0].NorrangeX[0];
    awb_cfg_v200->sw_rawawb_nor_x1_0                =    awb_meas.xyRange_param[0].NorrangeX[1];
    awb_cfg_v200->sw_rawawb_nor_y0_0                =    awb_meas.xyRange_param[0].NorrangeY[0];
    awb_cfg_v200->sw_rawawb_nor_y1_0                =    awb_meas.xyRange_param[0].NorrangeY[1];
    awb_cfg_v200->sw_rawawb_big_x0_0                =    awb_meas.xyRange_param[0].SperangeX[0];
    awb_cfg_v200->sw_rawawb_big_x1_0                =    awb_meas.xyRange_param[0].SperangeX[1];
    awb_cfg_v200->sw_rawawb_big_y0_0                =    awb_meas.xyRange_param[0].SperangeY[0];
    awb_cfg_v200->sw_rawawb_big_y1_0                =    awb_meas.xyRange_param[0].SperangeY[1];
    awb_cfg_v200->sw_rawawb_sma_x0_0                =    awb_meas.xyRange_param[0].SmalrangeX[0];
    awb_cfg_v200->sw_rawawb_sma_x1_0                =    awb_meas.xyRange_param[0].SmalrangeX[1];
    awb_cfg_v200->sw_rawawb_sma_y0_0                =    awb_meas.xyRange_param[0].SmalrangeY[0];
    awb_cfg_v200->sw_rawawb_sma_y1_0                =    awb_meas.xyRange_param[0].SmalrangeY[1];
    awb_cfg_v200->sw_rawawb_nor_x0_1                =    awb_meas.xyRange_param[1].NorrangeX[0];
    awb_cfg_v200->sw_rawawb_nor_x1_1                =    awb_meas.xyRange_param[1].NorrangeX[1];
    awb_cfg_v200->sw_rawawb_nor_y0_1                =    awb_meas.xyRange_param[1].NorrangeY[0];
    awb_cfg_v200->sw_rawawb_nor_y1_1                =    awb_meas.xyRange_param[1].NorrangeY[1];
    awb_cfg_v200->sw_rawawb_big_x0_1                =    awb_meas.xyRange_param[1].SperangeX[0];
    awb_cfg_v200->sw_rawawb_big_x1_1                =    awb_meas.xyRange_param[1].SperangeX[1];
    awb_cfg_v200->sw_rawawb_big_y0_1                =    awb_meas.xyRange_param[1].SperangeY[0];
    awb_cfg_v200->sw_rawawb_big_y1_1                =    awb_meas.xyRange_param[1].SperangeY[1];
    awb_cfg_v200->sw_rawawb_sma_x0_1                =    awb_meas.xyRange_param[1].SmalrangeX[0];
    awb_cfg_v200->sw_rawawb_sma_x1_1                =    awb_meas.xyRange_param[1].SmalrangeX[1];
    awb_cfg_v200->sw_rawawb_sma_y0_1                =    awb_meas.xyRange_param[1].SmalrangeY[0];
    awb_cfg_v200->sw_rawawb_sma_y1_1                =    awb_meas.xyRange_param[1].SmalrangeY[1];
    awb_cfg_v200->sw_rawawb_nor_x0_2                =    awb_meas.xyRange_param[2].NorrangeX[0];
    awb_cfg_v200->sw_rawawb_nor_x1_2                =    awb_meas.xyRange_param[2].NorrangeX[1];
    awb_cfg_v200->sw_rawawb_nor_y0_2                =    awb_meas.xyRange_param[2].NorrangeY[0];
    awb_cfg_v200->sw_rawawb_nor_y1_2                =    awb_meas.xyRange_param[2].NorrangeY[1];
    awb_cfg_v200->sw_rawawb_big_x0_2                =    awb_meas.xyRange_param[2].SperangeX[0];
    awb_cfg_v200->sw_rawawb_big_x1_2                =    awb_meas.xyRange_param[2].SperangeX[1];
    awb_cfg_v200->sw_rawawb_big_y0_2                =    awb_meas.xyRange_param[2].SperangeY[0];
    awb_cfg_v200->sw_rawawb_big_y1_2                =    awb_meas.xyRange_param[2].SperangeY[1];
    awb_cfg_v200->sw_rawawb_sma_x0_2                =    awb_meas.xyRange_param[2].SmalrangeX[0];
    awb_cfg_v200->sw_rawawb_sma_x1_2                =    awb_meas.xyRange_param[2].SmalrangeX[1];
    awb_cfg_v200->sw_rawawb_sma_y0_2                =    awb_meas.xyRange_param[2].SmalrangeY[0];
    awb_cfg_v200->sw_rawawb_sma_y1_2                =    awb_meas.xyRange_param[2].SmalrangeY[1];
    awb_cfg_v200->sw_rawawb_nor_x0_3                =    awb_meas.xyRange_param[3].NorrangeX[0];
    awb_cfg_v200->sw_rawawb_nor_x1_3                =    awb_meas.xyRange_param[3].NorrangeX[1];
    awb_cfg_v200->sw_rawawb_nor_y0_3                =    awb_meas.xyRange_param[3].NorrangeY[0];
    awb_cfg_v200->sw_rawawb_nor_y1_3                =    awb_meas.xyRange_param[3].NorrangeY[1];
    awb_cfg_v200->sw_rawawb_big_x0_3                =    awb_meas.xyRange_param[3].SperangeX[0];
    awb_cfg_v200->sw_rawawb_big_x1_3                =    awb_meas.xyRange_param[3].SperangeX[1];
    awb_cfg_v200->sw_rawawb_big_y0_3                =    awb_meas.xyRange_param[3].SperangeY[0];
    awb_cfg_v200->sw_rawawb_big_y1_3                =    awb_meas.xyRange_param[3].SperangeY[1];
    awb_cfg_v200->sw_rawawb_sma_x0_3                =    awb_meas.xyRange_param[3].SmalrangeX[0];
    awb_cfg_v200->sw_rawawb_sma_x1_3                =    awb_meas.xyRange_param[3].SmalrangeX[1];
    awb_cfg_v200->sw_rawawb_sma_y0_3                =    awb_meas.xyRange_param[3].SmalrangeY[0];
    awb_cfg_v200->sw_rawawb_sma_y1_3                =    awb_meas.xyRange_param[3].SmalrangeY[1];
    awb_cfg_v200->sw_rawawb_nor_x0_4                =    awb_meas.xyRange_param[4].NorrangeX[0];
    awb_cfg_v200->sw_rawawb_nor_x1_4                =    awb_meas.xyRange_param[4].NorrangeX[1];
    awb_cfg_v200->sw_rawawb_nor_y0_4                =    awb_meas.xyRange_param[4].NorrangeY[0];
    awb_cfg_v200->sw_rawawb_nor_y1_4                =    awb_meas.xyRange_param[4].NorrangeY[1];
    awb_cfg_v200->sw_rawawb_big_x0_4                =    awb_meas.xyRange_param[4].SperangeX[0];
    awb_cfg_v200->sw_rawawb_big_x1_4                =    awb_meas.xyRange_param[4].SperangeX[1];
    awb_cfg_v200->sw_rawawb_big_y0_4                =    awb_meas.xyRange_param[4].SperangeY[0];
    awb_cfg_v200->sw_rawawb_big_y1_4                =    awb_meas.xyRange_param[4].SperangeY[1];
    awb_cfg_v200->sw_rawawb_sma_x0_4                =    awb_meas.xyRange_param[4].SmalrangeX[0];
    awb_cfg_v200->sw_rawawb_sma_x1_4                =    awb_meas.xyRange_param[4].SmalrangeX[1];
    awb_cfg_v200->sw_rawawb_sma_y0_4                =    awb_meas.xyRange_param[4].SmalrangeY[0];
    awb_cfg_v200->sw_rawawb_sma_y1_4                =    awb_meas.xyRange_param[4].SmalrangeY[1];
    awb_cfg_v200->sw_rawawb_nor_x0_5                =    awb_meas.xyRange_param[5].NorrangeX[0];
    awb_cfg_v200->sw_rawawb_nor_x1_5                =    awb_meas.xyRange_param[5].NorrangeX[1];
    awb_cfg_v200->sw_rawawb_nor_y0_5                =    awb_meas.xyRange_param[5].NorrangeY[0];
    awb_cfg_v200->sw_rawawb_nor_y1_5                =    awb_meas.xyRange_param[5].NorrangeY[1];
    awb_cfg_v200->sw_rawawb_big_x0_5                =    awb_meas.xyRange_param[5].SperangeX[0];
    awb_cfg_v200->sw_rawawb_big_x1_5                =    awb_meas.xyRange_param[5].SperangeX[1];
    awb_cfg_v200->sw_rawawb_big_y0_5                =    awb_meas.xyRange_param[5].SperangeY[0];
    awb_cfg_v200->sw_rawawb_big_y1_5                =    awb_meas.xyRange_param[5].SperangeY[1];
    awb_cfg_v200->sw_rawawb_sma_x0_5                =    awb_meas.xyRange_param[5].SmalrangeX[0];
    awb_cfg_v200->sw_rawawb_sma_x1_5                =    awb_meas.xyRange_param[5].SmalrangeX[1];
    awb_cfg_v200->sw_rawawb_sma_y0_5                =    awb_meas.xyRange_param[5].SmalrangeY[0];
    awb_cfg_v200->sw_rawawb_sma_y1_5                =    awb_meas.xyRange_param[5].SmalrangeY[1];
    awb_cfg_v200->sw_rawawb_nor_x0_6                =    awb_meas.xyRange_param[6].NorrangeX[0];
    awb_cfg_v200->sw_rawawb_nor_x1_6                =    awb_meas.xyRange_param[6].NorrangeX[1];
    awb_cfg_v200->sw_rawawb_nor_y0_6                =    awb_meas.xyRange_param[6].NorrangeY[0];
    awb_cfg_v200->sw_rawawb_nor_y1_6                =    awb_meas.xyRange_param[6].NorrangeY[1];
    awb_cfg_v200->sw_rawawb_big_x0_6                =    awb_meas.xyRange_param[6].SperangeX[0];
    awb_cfg_v200->sw_rawawb_big_x1_6                =    awb_meas.xyRange_param[6].SperangeX[1];
    awb_cfg_v200->sw_rawawb_big_y0_6                =    awb_meas.xyRange_param[6].SperangeY[0];
    awb_cfg_v200->sw_rawawb_big_y1_6                =    awb_meas.xyRange_param[6].SperangeY[1];
    awb_cfg_v200->sw_rawawb_sma_x0_6                =    awb_meas.xyRange_param[6].SmalrangeX[0];
    awb_cfg_v200->sw_rawawb_sma_x1_6                =    awb_meas.xyRange_param[6].SmalrangeX[1];
    awb_cfg_v200->sw_rawawb_sma_y0_6                =    awb_meas.xyRange_param[6].SmalrangeY[0];
    awb_cfg_v200->sw_rawawb_sma_y1_6                =    awb_meas.xyRange_param[6].SmalrangeY[1];
    //multiwindow
    awb_cfg_v200->sw_rawawb_multiwindow_en          =      awb_meas.multiwindowEn;
    awb_cfg_v200->sw_rawawb_multiwindow0_h_offs     =      awb_meas.multiwindow[0][0];
    awb_cfg_v200->sw_rawawb_multiwindow0_v_offs     =      awb_meas.multiwindow[0][1];
    awb_cfg_v200->sw_rawawb_multiwindow0_h_size     =      awb_meas.multiwindow[0][2];
    awb_cfg_v200->sw_rawawb_multiwindow0_v_size     =      awb_meas.multiwindow[0][3];
    awb_cfg_v200->sw_rawawb_multiwindow1_h_offs     =      awb_meas.multiwindow[1][0];
    awb_cfg_v200->sw_rawawb_multiwindow1_v_offs     =      awb_meas.multiwindow[1][1];
    awb_cfg_v200->sw_rawawb_multiwindow1_h_size     =      awb_meas.multiwindow[1][2];
    awb_cfg_v200->sw_rawawb_multiwindow1_v_size     =      awb_meas.multiwindow[1][3];
    awb_cfg_v200->sw_rawawb_multiwindow2_h_offs     =      awb_meas.multiwindow[2][0];
    awb_cfg_v200->sw_rawawb_multiwindow2_v_offs     =      awb_meas.multiwindow[2][1];
    awb_cfg_v200->sw_rawawb_multiwindow2_h_size     =      awb_meas.multiwindow[2][2];
    awb_cfg_v200->sw_rawawb_multiwindow2_v_size     =      awb_meas.multiwindow[2][3];
    awb_cfg_v200->sw_rawawb_multiwindow3_h_offs     =      awb_meas.multiwindow[3][0];
    awb_cfg_v200->sw_rawawb_multiwindow3_v_offs     =      awb_meas.multiwindow[3][1];
    awb_cfg_v200->sw_rawawb_multiwindow3_h_size     =      awb_meas.multiwindow[3][2];
    awb_cfg_v200->sw_rawawb_multiwindow3_v_size     =      awb_meas.multiwindow[3][3];
    awb_cfg_v200->sw_rawawb_multiwindow4_h_offs     =      awb_meas.multiwindow[4][0];
    awb_cfg_v200->sw_rawawb_multiwindow4_v_offs     =      awb_meas.multiwindow[4][1];
    awb_cfg_v200->sw_rawawb_multiwindow4_h_size     =      awb_meas.multiwindow[4][2];
    awb_cfg_v200->sw_rawawb_multiwindow4_v_size     =      awb_meas.multiwindow[4][3];
    awb_cfg_v200->sw_rawawb_multiwindow5_h_offs     =      awb_meas.multiwindow[5][0];
    awb_cfg_v200->sw_rawawb_multiwindow5_v_offs     =      awb_meas.multiwindow[5][1];
    awb_cfg_v200->sw_rawawb_multiwindow5_h_size     =      awb_meas.multiwindow[5][2];
    awb_cfg_v200->sw_rawawb_multiwindow5_v_size     =      awb_meas.multiwindow[5][3];
    awb_cfg_v200->sw_rawawb_multiwindow6_h_offs     =      awb_meas.multiwindow[6][0];
    awb_cfg_v200->sw_rawawb_multiwindow6_v_offs     =      awb_meas.multiwindow[6][1];
    awb_cfg_v200->sw_rawawb_multiwindow6_h_size     =      awb_meas.multiwindow[6][2];
    awb_cfg_v200->sw_rawawb_multiwindow6_v_size     =      awb_meas.multiwindow[6][3];
    awb_cfg_v200->sw_rawawb_multiwindow7_h_offs     =      awb_meas.multiwindow[7][0];
    awb_cfg_v200->sw_rawawb_multiwindow7_v_offs     =      awb_meas.multiwindow[7][1];
    awb_cfg_v200->sw_rawawb_multiwindow7_h_size     =      awb_meas.multiwindow[7][2];
    awb_cfg_v200->sw_rawawb_multiwindow7_v_size     =      awb_meas.multiwindow[7][3];
    //exc range

    awb_cfg_v200->sw_rawawb_exc_wp_region0_excen    =     awb_meas.excludeWpRange[0].excludeEnable;
    awb_cfg_v200->sw_rawawb_exc_wp_region0_measen   =     awb_meas.excludeWpRange[0].measureEnable;
    switch(awb_meas.excludeWpRange[0].domain) {
    case RK_AIQ_AWB_EXC_RANGE_DOMAIN_UV:
        awb_cfg_v200->sw_rawawb_exc_wp_region0_domain  =     0;
        break;
    default:
        awb_cfg_v200->sw_rawawb_exc_wp_region0_domain  =     1;
    }
    awb_cfg_v200->sw_rawawb_exc_wp_region0_xu0      =     awb_meas.excludeWpRange[0].xu[0];
    awb_cfg_v200->sw_rawawb_exc_wp_region0_xu1      =     awb_meas.excludeWpRange[0].xu[1];
    awb_cfg_v200->sw_rawawb_exc_wp_region0_yv0      =     awb_meas.excludeWpRange[0].yv[0];
    awb_cfg_v200->sw_rawawb_exc_wp_region0_yv1      =     awb_meas.excludeWpRange[0].yv[1];
    awb_cfg_v200->sw_rawawb_exc_wp_region1_excen    =     awb_meas.excludeWpRange[1].excludeEnable;
    awb_cfg_v200->sw_rawawb_exc_wp_region1_measen   =     awb_meas.excludeWpRange[1].measureEnable;
    awb_cfg_v200->sw_rawawb_exc_wp_region1_domain   =     awb_meas.excludeWpRange[1].domain;
    switch(awb_meas.excludeWpRange[1].domain) {
    case RK_AIQ_AWB_EXC_RANGE_DOMAIN_UV:
        awb_cfg_v200->sw_rawawb_exc_wp_region1_domain  =     0;
        break;
    default:
        awb_cfg_v200->sw_rawawb_exc_wp_region1_domain  =     1;
    }
    awb_cfg_v200->sw_rawawb_exc_wp_region1_xu0      =     awb_meas.excludeWpRange[1].xu[0];
    awb_cfg_v200->sw_rawawb_exc_wp_region1_xu1      =     awb_meas.excludeWpRange[1].xu[1];
    awb_cfg_v200->sw_rawawb_exc_wp_region1_yv0      =     awb_meas.excludeWpRange[1].yv[0];
    awb_cfg_v200->sw_rawawb_exc_wp_region1_yv1      =     awb_meas.excludeWpRange[1].yv[1];
    awb_cfg_v200->sw_rawawb_exc_wp_region2_excen    =     awb_meas.excludeWpRange[2].excludeEnable;
    awb_cfg_v200->sw_rawawb_exc_wp_region2_measen   =     awb_meas.excludeWpRange[2].measureEnable;
    switch(awb_meas.excludeWpRange[2].domain) {
    case RK_AIQ_AWB_EXC_RANGE_DOMAIN_UV:
        awb_cfg_v200->sw_rawawb_exc_wp_region2_domain  =     0;
        break;
    default:
        awb_cfg_v200->sw_rawawb_exc_wp_region2_domain  =     1;
    }
    awb_cfg_v200->sw_rawawb_exc_wp_region2_xu0      =     awb_meas.excludeWpRange[2].xu[0];
    awb_cfg_v200->sw_rawawb_exc_wp_region2_xu1      =     awb_meas.excludeWpRange[2].xu[1];
    awb_cfg_v200->sw_rawawb_exc_wp_region2_yv0      =     awb_meas.excludeWpRange[2].yv[0];
    awb_cfg_v200->sw_rawawb_exc_wp_region2_yv1      =     awb_meas.excludeWpRange[2].yv[1];
    awb_cfg_v200->sw_rawawb_exc_wp_region3_excen    =     awb_meas.excludeWpRange[3].excludeEnable;
    awb_cfg_v200->sw_rawawb_exc_wp_region3_measen   =     awb_meas.excludeWpRange[3].measureEnable;
    awb_cfg_v200->sw_rawawb_exc_wp_region3_domain   =     awb_meas.excludeWpRange[3].domain;
    switch(awb_meas.excludeWpRange[3].domain) {
    case RK_AIQ_AWB_EXC_RANGE_DOMAIN_UV:
        awb_cfg_v200->sw_rawawb_exc_wp_region3_domain  =     0;
        break;
    default:
        awb_cfg_v200->sw_rawawb_exc_wp_region3_domain  =     1;
    }
    awb_cfg_v200->sw_rawawb_exc_wp_region3_xu0      =     awb_meas.excludeWpRange[3].xu[0];
    awb_cfg_v200->sw_rawawb_exc_wp_region3_xu1      =     awb_meas.excludeWpRange[3].xu[1];
    awb_cfg_v200->sw_rawawb_exc_wp_region3_yv0      =     awb_meas.excludeWpRange[3].yv[0];
    awb_cfg_v200->sw_rawawb_exc_wp_region3_yv1      =     awb_meas.excludeWpRange[3].yv[1];
    awb_cfg_v200->sw_rawawb_exc_wp_region4_excen    =     awb_meas.excludeWpRange[4].excludeEnable;
    awb_cfg_v200->sw_rawawb_exc_wp_region4_measen   =     awb_meas.excludeWpRange[4].measureEnable;
    switch(awb_meas.excludeWpRange[4].domain) {
    case RK_AIQ_AWB_EXC_RANGE_DOMAIN_UV:
        awb_cfg_v200->sw_rawawb_exc_wp_region4_domain  =     0;
        break;
    default:
        awb_cfg_v200->sw_rawawb_exc_wp_region4_domain  =     1;
    }
    awb_cfg_v200->sw_rawawb_exc_wp_region4_xu0      =     awb_meas.excludeWpRange[4].xu[0];
    awb_cfg_v200->sw_rawawb_exc_wp_region4_xu1      =     awb_meas.excludeWpRange[4].xu[1];
    awb_cfg_v200->sw_rawawb_exc_wp_region4_yv0      =     awb_meas.excludeWpRange[4].yv[0];
    awb_cfg_v200->sw_rawawb_exc_wp_region4_yv1      =     awb_meas.excludeWpRange[4].yv[1];
    awb_cfg_v200->sw_rawawb_exc_wp_region5_excen    =     awb_meas.excludeWpRange[5].excludeEnable;
    awb_cfg_v200->sw_rawawb_exc_wp_region5_measen   =     awb_meas.excludeWpRange[5].measureEnable;
    switch(awb_meas.excludeWpRange[5].domain) {
    case RK_AIQ_AWB_EXC_RANGE_DOMAIN_UV:
        awb_cfg_v200->sw_rawawb_exc_wp_region5_domain  =     0;
        break;
    default:
        awb_cfg_v200->sw_rawawb_exc_wp_region5_domain  =     1;
    }
    awb_cfg_v200->sw_rawawb_exc_wp_region5_xu0      =     awb_meas.excludeWpRange[5].xu[0];
    awb_cfg_v200->sw_rawawb_exc_wp_region5_xu1      =     awb_meas.excludeWpRange[5].xu[1];
    awb_cfg_v200->sw_rawawb_exc_wp_region5_yv0      =     awb_meas.excludeWpRange[5].yv[0];
    awb_cfg_v200->sw_rawawb_exc_wp_region5_yv1      =     awb_meas.excludeWpRange[5].yv[1];
    awb_cfg_v200->sw_rawawb_exc_wp_region6_excen    =     awb_meas.excludeWpRange[6].excludeEnable;
    awb_cfg_v200->sw_rawawb_exc_wp_region6_measen   =     awb_meas.excludeWpRange[6].measureEnable;
    switch(awb_meas.excludeWpRange[6].domain) {
    case RK_AIQ_AWB_EXC_RANGE_DOMAIN_UV:
        awb_cfg_v200->sw_rawawb_exc_wp_region6_domain  =     0;
        break;
    default:
        awb_cfg_v200->sw_rawawb_exc_wp_region6_domain  =     1;
    }
    awb_cfg_v200->sw_rawawb_exc_wp_region6_xu0      =     awb_meas.excludeWpRange[6].xu[0];
    awb_cfg_v200->sw_rawawb_exc_wp_region6_xu1      =     awb_meas.excludeWpRange[6].xu[1];
    awb_cfg_v200->sw_rawawb_exc_wp_region6_yv0      =     awb_meas.excludeWpRange[6].yv[0];
    awb_cfg_v200->sw_rawawb_exc_wp_region6_yv1      =     awb_meas.excludeWpRange[6].yv[1];

    awb_cfg_v200->sw_rawawb_store_wp_flag_ls_idx0   =     awb_meas.storeWpFlagIllu[0];
    awb_cfg_v200->sw_rawawb_store_wp_flag_ls_idx1   =     awb_meas.storeWpFlagIllu[1];
    awb_cfg_v200->sw_rawawb_store_wp_flag_ls_idx2   =     awb_meas.storeWpFlagIllu[2];
}


#define ISP2X_WBGAIN_FIXSCALE_BIT  8

void Isp20Params::convertAiqAhdrToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
        const rk_aiq_isp_hdr_t& ahdr_data)
{

    //merge register
    isp_cfg.others.hdrmge_cfg.mode         = ahdr_data.MgeProcRes.sw_hdrmge_mode;
    isp_cfg.others.hdrmge_cfg.gain0_inv    = ahdr_data.MgeProcRes.sw_hdrmge_gain0_inv;
    isp_cfg.others.hdrmge_cfg.gain0         = ahdr_data.MgeProcRes.sw_hdrmge_gain0;
    isp_cfg.others.hdrmge_cfg.gain1_inv    = ahdr_data.MgeProcRes.sw_hdrmge_gain1_inv;
    isp_cfg.others.hdrmge_cfg.gain1        = ahdr_data.MgeProcRes.sw_hdrmge_gain1;
    isp_cfg.others.hdrmge_cfg.gain2        = ahdr_data.MgeProcRes.sw_hdrmge_gain2;
    isp_cfg.others.hdrmge_cfg.lm_dif_0p15  = ahdr_data.MgeProcRes.sw_hdrmge_lm_dif_0p15;
    isp_cfg.others.hdrmge_cfg.lm_dif_0p9   = ahdr_data.MgeProcRes.sw_hdrmge_lm_dif_0p9;
    isp_cfg.others.hdrmge_cfg.ms_diff_0p15 = ahdr_data.MgeProcRes.sw_hdrmge_ms_dif_0p15;
    isp_cfg.others.hdrmge_cfg.ms_dif_0p8   = ahdr_data.MgeProcRes.sw_hdrmge_ms_dif_0p8;
    for(int i = 0; i < 17; i++)
    {
        isp_cfg.others.hdrmge_cfg.curve.curve_0[i] = ahdr_data.MgeProcRes.sw_hdrmge_l0_y[i];
        isp_cfg.others.hdrmge_cfg.curve.curve_1[i] = ahdr_data.MgeProcRes.sw_hdrmge_l1_y[i];
        isp_cfg.others.hdrmge_cfg.e_y[i]           = ahdr_data.MgeProcRes.sw_hdrmge_e_y[i];
    }

    //tmo register
    isp_cfg.others.hdrtmo_cfg.cnt_vsize     = ahdr_data.TmoProcRes.sw_hdrtmo_cnt_vsize;
    isp_cfg.others.hdrtmo_cfg.gain_ld_off2  = ahdr_data.TmoProcRes.sw_hdrtmo_gain_ld_off2;
    isp_cfg.others.hdrtmo_cfg.gain_ld_off1  = ahdr_data.TmoProcRes.sw_hdrtmo_gain_ld_off1;
    isp_cfg.others.hdrtmo_cfg.big_en        = ahdr_data.TmoProcRes.sw_hdrtmo_big_en;
    isp_cfg.others.hdrtmo_cfg.nobig_en      = ahdr_data.TmoProcRes.sw_hdrtmo_nobig_en;
    isp_cfg.others.hdrtmo_cfg.newhst_en     = ahdr_data.TmoProcRes.sw_hdrtmo_newhist_en;
    isp_cfg.others.hdrtmo_cfg.cnt_mode      = ahdr_data.TmoProcRes.sw_hdrtmo_cnt_mode;
    isp_cfg.others.hdrtmo_cfg.expl_lgratio  = ahdr_data.TmoProcRes.sw_hdrtmo_expl_lgratio;
    isp_cfg.others.hdrtmo_cfg.lgscl_ratio   = ahdr_data.TmoProcRes.sw_hdrtmo_lgscl_ratio;
    isp_cfg.others.hdrtmo_cfg.cfg_alpha     = ahdr_data.TmoProcRes.sw_hdrtmo_cfg_alpha;
    isp_cfg.others.hdrtmo_cfg.set_gainoff   = ahdr_data.TmoProcRes.sw_hdrtmo_set_gainoff;
    isp_cfg.others.hdrtmo_cfg.set_palpha    = ahdr_data.TmoProcRes.sw_hdrtmo_set_palpha;
    isp_cfg.others.hdrtmo_cfg.set_lgmax     = ahdr_data.TmoProcRes.sw_hdrtmo_set_lgmax;
    isp_cfg.others.hdrtmo_cfg.set_lgmin     = ahdr_data.TmoProcRes.sw_hdrtmo_set_lgmin;
    isp_cfg.others.hdrtmo_cfg.set_weightkey = ahdr_data.TmoProcRes.sw_hdrtmo_set_weightkey;
    isp_cfg.others.hdrtmo_cfg.set_lgmean    = ahdr_data.TmoProcRes.sw_hdrtmo_set_lgmean;
    isp_cfg.others.hdrtmo_cfg.set_lgrange1  = ahdr_data.TmoProcRes.sw_hdrtmo_set_lgrange1;
    isp_cfg.others.hdrtmo_cfg.set_lgrange0  = ahdr_data.TmoProcRes.sw_hdrtmo_set_lgrange0;
    isp_cfg.others.hdrtmo_cfg.set_lgavgmax  = ahdr_data.TmoProcRes.sw_hdrtmo_set_lgavgmax;
    isp_cfg.others.hdrtmo_cfg.clipgap1_i    = ahdr_data.TmoProcRes.sw_hdrtmo_clipgap1;
    isp_cfg.others.hdrtmo_cfg.clipgap0_i    = ahdr_data.TmoProcRes.sw_hdrtmo_clipgap0;
    isp_cfg.others.hdrtmo_cfg.clipratio1    = ahdr_data.TmoProcRes.sw_hdrtmo_clipratio1;
    isp_cfg.others.hdrtmo_cfg.clipratio0    = ahdr_data.TmoProcRes.sw_hdrtmo_clipratio0;
    isp_cfg.others.hdrtmo_cfg.ratiol        = ahdr_data.TmoProcRes.sw_hdrtmo_ratiol;
    isp_cfg.others.hdrtmo_cfg.lgscl_inv     = ahdr_data.TmoProcRes.sw_hdrtmo_lgscl_inv;
    isp_cfg.others.hdrtmo_cfg.lgscl         = ahdr_data.TmoProcRes.sw_hdrtmo_lgscl;
    isp_cfg.others.hdrtmo_cfg.lgmax         = ahdr_data.TmoProcRes.sw_hdrtmo_lgmax;
    isp_cfg.others.hdrtmo_cfg.hist_low      = ahdr_data.TmoProcRes.sw_hdrtmo_hist_low;
    isp_cfg.others.hdrtmo_cfg.hist_min      = ahdr_data.TmoProcRes.sw_hdrtmo_hist_min;
    isp_cfg.others.hdrtmo_cfg.hist_shift    = ahdr_data.TmoProcRes.sw_hdrtmo_hist_shift;
    isp_cfg.others.hdrtmo_cfg.hist_0p3      = ahdr_data.TmoProcRes.sw_hdrtmo_hist_0p3;
    isp_cfg.others.hdrtmo_cfg.hist_high     = ahdr_data.TmoProcRes.sw_hdrtmo_hist_high;
    isp_cfg.others.hdrtmo_cfg.palpha_lwscl  = ahdr_data.TmoProcRes.sw_hdrtmo_palpha_lwscl;
    isp_cfg.others.hdrtmo_cfg.palpha_lw0p5  = ahdr_data.TmoProcRes.sw_hdrtmo_palpha_lw0p5;
    isp_cfg.others.hdrtmo_cfg.palpha_0p18   = ahdr_data.TmoProcRes.sw_hdrtmo_palpha_0p18;
    isp_cfg.others.hdrtmo_cfg.maxgain       = ahdr_data.TmoProcRes.sw_hdrtmo_maxgain;
    isp_cfg.others.hdrtmo_cfg.maxpalpha     = ahdr_data.TmoProcRes.sw_hdrtmo_maxpalpha;

    //api setting
    ahdr_data.hdrAttr.bEnable == false;
    //update merge data in Auto mode
    if(ahdr_data.hdrAttr.bEnable == true && ahdr_data.hdrAttr.stAuto.bUpdateMge == true )
    {
        //get oe cruve
        float slope = 0;
        float ofst = 0;
        slope = LIMIT_PARA(ahdr_data.hdrAttr.stAuto.stMgeAuto.stOECurve.stCoef, ahdr_data.hdrAttr.stAuto.stMgeAuto.stOECurve.stSmthMax,
                           ahdr_data.hdrAttr.stAuto.stMgeAuto.stOECurve.stSmthMin, ahdr_data.hdrAttr.stAuto.stMgeAuto.stOECurve.stCoefMax,
                           ahdr_data.hdrAttr.stAuto.stMgeAuto.stOECurve.stCoefMin);
        ofst = LIMIT_PARA(ahdr_data.hdrAttr.stAuto.stMgeAuto.stOECurve.stCoef, ahdr_data.hdrAttr.stAuto.stMgeAuto.stOECurve.stOfstMax,
                          ahdr_data.hdrAttr.stAuto.stMgeAuto.stOECurve.stOfstMin, ahdr_data.hdrAttr.stAuto.stMgeAuto.stOECurve.stCoefMax,
                          ahdr_data.hdrAttr.stAuto.stMgeAuto.stOECurve.stCoefMin);
        int step = 32 ;
        float curve = 0;
        int OECurve[17];
        float k = 511;
        for(int i = 0; i < 17; ++i)
        {
            curve = 1 + exp(-slope * (k / 1023.0 - ofst / 256.0));
            curve = 1024.0 / curve ;
            OECurve[i] = round(curve) ;
            OECurve[i] = MIN(OECurve[i], 1023);
            isp_cfg.others.hdrmge_cfg.e_y[i] = OECurve[i];
            k += step ;
        }

        //get md cruve ms
        slope = 0;
        ofst = 0;
        slope = LIMIT_PARA(ahdr_data.hdrAttr.stAuto.stMgeAuto.stMDCurveMS.stCoef, ahdr_data.hdrAttr.stAuto.stMgeAuto.stMDCurveMS.stSmthMax,
                           ahdr_data.hdrAttr.stAuto.stMgeAuto.stMDCurveMS.stSmthMin, ahdr_data.hdrAttr.stAuto.stMgeAuto.stMDCurveMS.stCoefMax,
                           ahdr_data.hdrAttr.stAuto.stMgeAuto.stMDCurveMS.stCoefMin);
        ofst = LIMIT_PARA(ahdr_data.hdrAttr.stAuto.stMgeAuto.stMDCurveMS.stCoef, ahdr_data.hdrAttr.stAuto.stMgeAuto.stMDCurveMS.stOfstMax,
                          ahdr_data.hdrAttr.stAuto.stMgeAuto.stMDCurveMS.stOfstMin, ahdr_data.hdrAttr.stAuto.stMgeAuto.stMDCurveMS.stCoefMax,
                          ahdr_data.hdrAttr.stAuto.stMgeAuto.stMDCurveMS.stCoefMin);
        isp_cfg.others.hdrmge_cfg.ms_diff_0p15   = (int)(ofst + 0.5);
        step = 16;
        curve = 0;
        int MDCurveMS[17];
        k = 0;
        for (int i = 0; i < 17; ++i)
        {
            curve = 1 + exp(-slope * (k / 1023.0 - ofst / 256.0));
            curve = 1024.0 / curve ;
            MDCurveMS[i] = round(curve) ;
            MDCurveMS[i] = MIN(MDCurveMS[i], 1023);
            isp_cfg.others.hdrmge_cfg.curve.curve_0[i] = MDCurveMS[i];
            k += step ;
        }

        //get md cruve lm
        slope = 0;
        ofst = 0;
        slope = LIMIT_PARA(ahdr_data.hdrAttr.stAuto.stMgeAuto.stMDCurveLM.stCoef, ahdr_data.hdrAttr.stAuto.stMgeAuto.stMDCurveLM.stSmthMax,
                           ahdr_data.hdrAttr.stAuto.stMgeAuto.stMDCurveLM.stSmthMin, ahdr_data.hdrAttr.stAuto.stMgeAuto.stMDCurveLM.stCoefMax,
                           ahdr_data.hdrAttr.stAuto.stMgeAuto.stMDCurveLM.stCoefMin);
        ofst = LIMIT_PARA(ahdr_data.hdrAttr.stAuto.stMgeAuto.stMDCurveLM.stCoef, ahdr_data.hdrAttr.stAuto.stMgeAuto.stMDCurveLM.stOfstMax,
                          ahdr_data.hdrAttr.stAuto.stMgeAuto.stMDCurveLM.stOfstMin, ahdr_data.hdrAttr.stAuto.stMgeAuto.stMDCurveLM.stCoefMax,
                          ahdr_data.hdrAttr.stAuto.stMgeAuto.stMDCurveLM.stCoefMin);
        isp_cfg.others.hdrmge_cfg.lm_dif_0p15   = (int)(ofst + 0.5);
        step = 16;
        curve = 0;
        int MDCurveLM[17];
        k = 0;

        for (int i = 0; i < 17; ++i)
        {
            curve = 1 + exp(-slope * (k / 1023.0 - ofst / 256.0));
            curve = 1024.0 / curve ;
            MDCurveLM[i] = round(curve) ;
            MDCurveLM[i] = MIN(MDCurveLM[i], 1023.0);
            isp_cfg.others.hdrmge_cfg.curve.curve_1[i] = MDCurveLM[i];
            k += step ;
        }
    }

    //update tmo data in Auto mode
    if(ahdr_data.hdrAttr.bEnable == true && ahdr_data.hdrAttr.stAuto.bUpdateTmo == true )
    {
        float value = 0;
        value = LIMIT_PARA(ahdr_data.hdrAttr.stAuto.stTmoAuto.stDtlsLL.stCoef, ahdr_data.hdrAttr.stAuto.stTmoAuto.stDtlsLL.stMax, ahdr_data.hdrAttr.stAuto.stTmoAuto.stDtlsLL.stMin,
                           ahdr_data.hdrAttr.stAuto.stTmoAuto.stDtlsLL.stCoefMax, ahdr_data.hdrAttr.stAuto.stTmoAuto.stDtlsLL.stMin);
        isp_cfg.others.hdrtmo_cfg.palpha_lwscl  = (int)(value + 0.5);

        value = 0;
        value = LIMIT_PARA(ahdr_data.hdrAttr.stAuto.stTmoAuto.stDtlsHL.stCoef, ahdr_data.hdrAttr.stAuto.stTmoAuto.stDtlsHL.stMax, ahdr_data.hdrAttr.stAuto.stTmoAuto.stDtlsHL.stMin,
                           ahdr_data.hdrAttr.stAuto.stTmoAuto.stDtlsHL.stCoefMax, ahdr_data.hdrAttr.stAuto.stTmoAuto.stDtlsHL.stMin);
        isp_cfg.others.hdrtmo_cfg.palpha_lw0p5  = (int)(value + 0.5);

        value = 0;
        value = LIMIT_PARA(ahdr_data.hdrAttr.stAuto.stTmoAuto.stGlobeLuma.stCoef, ahdr_data.hdrAttr.stAuto.stTmoAuto.stGlobeLuma.stMax, ahdr_data.hdrAttr.stAuto.stTmoAuto.stGlobeLuma.stMin,
                           ahdr_data.hdrAttr.stAuto.stTmoAuto.stGlobeLuma.stCoefMax, ahdr_data.hdrAttr.stAuto.stTmoAuto.stGlobeLuma.stMin);
        isp_cfg.others.hdrtmo_cfg.palpha_0p18   = (int)(value + 0.5);

        value = 0;
        value = LIMIT_PARA(ahdr_data.hdrAttr.stAuto.stTmoAuto.stGlobeMaxLuma.stCoef, ahdr_data.hdrAttr.stAuto.stTmoAuto.stGlobeMaxLuma.stMax, ahdr_data.hdrAttr.stAuto.stTmoAuto.stGlobeMaxLuma.stMin,
                           ahdr_data.hdrAttr.stAuto.stTmoAuto.stGlobeMaxLuma.stCoefMax, ahdr_data.hdrAttr.stAuto.stTmoAuto.stGlobeMaxLuma.stMin);
        isp_cfg.others.hdrtmo_cfg.maxpalpha     = (int)(value + 0.5);

        value = 0;
        value = value = LIMIT_PARA(ahdr_data.hdrAttr.stAuto.stTmoAuto.stSmthCtrlCoef.stCoef, ahdr_data.hdrAttr.stAuto.stTmoAuto.stSmthCtrlCoef.stMax, ahdr_data.hdrAttr.stAuto.stTmoAuto.stSmthCtrlCoef.stMin,
                                   ahdr_data.hdrAttr.stAuto.stTmoAuto.stSmthCtrlCoef.stCoefMax, ahdr_data.hdrAttr.stAuto.stTmoAuto.stSmthCtrlCoef.stMin);
        isp_cfg.others.hdrtmo_cfg.set_weightkey = (int)(value + 0.5);

    }

    //update merge data in manual mode
    if(ahdr_data.hdrAttr.bEnable == true && ahdr_data.hdrAttr.stManual.bUpdateMge == true )
    {
        isp_cfg.others.hdrmge_cfg.mode = ahdr_data.hdrAttr.opMode;
        for(int i = 0; i < 17; i++)
        {
            isp_cfg.others.hdrmge_cfg.curve.curve_0[i] = (int)(ahdr_data.hdrAttr.stManual.stMgeManual.MDCurveMS[i]);
            isp_cfg.others.hdrmge_cfg.curve.curve_1[i] = (int)(ahdr_data.hdrAttr.stManual.stMgeManual.MDCurveLM[i]);
            isp_cfg.others.hdrmge_cfg.e_y[i]           = (int)(ahdr_data.hdrAttr.stManual.stMgeManual.OECurve[i]);
        }
    }

    //update tmo data in manual mode
    if(ahdr_data.hdrAttr.bEnable == true && ahdr_data.hdrAttr.stManual.bUpdateTmo == true )
    {
        isp_cfg.others.hdrtmo_cfg.palpha_lwscl  = (int)(ahdr_data.hdrAttr.stManual.stTmoManual.stDtlsLL + 0.5);
        isp_cfg.others.hdrtmo_cfg.palpha_lw0p5  = (int)(ahdr_data.hdrAttr.stManual.stTmoManual.stDtlsHL + 0.5);
        isp_cfg.others.hdrtmo_cfg.palpha_0p18   = (int)(ahdr_data.hdrAttr.stManual.stTmoManual.stGlobeLuma + 0.5);
        isp_cfg.others.hdrtmo_cfg.set_weightkey = (int)(ahdr_data.hdrAttr.stManual.stTmoManual.stSmthCtrlCoef + 0.5);
        isp_cfg.others.hdrtmo_cfg.maxpalpha     = (int)(ahdr_data.hdrAttr.stManual.stTmoManual.stGlobeMaxLuma + 0.5);
    }

#if 0
    LOGE("%d: gain0_inv %d", __LINE__, isp_cfg.others.hdrmge_cfg.gain0_inv);
    LOGE("%d: gain0 %d", __LINE__, isp_cfg.others.hdrmge_cfg.gain0);
    LOGE("%d: gain1_inv %d", __LINE__, isp_cfg.others.hdrmge_cfg.gain1_inv);
    LOGE("%d: gain1 %d", __LINE__, isp_cfg.others.hdrmge_cfg.gain1);
    LOGE("%d: gain2 %d", __LINE__, isp_cfg.others.hdrmge_cfg.gain2);
    LOGE("%d: lm_dif_0p15 %d", __LINE__, isp_cfg.others.hdrmge_cfg.lm_dif_0p15);
    LOGE("%d: lm_dif_0p9 %d", __LINE__, isp_cfg.others.hdrmge_cfg.lm_dif_0p9);
    LOGE("%d: ms_diff_0p15 %d", __LINE__, isp_cfg.others.hdrmge_cfg.ms_diff_0p15);
    LOGE("%d: ms_dif_0p8 %d", __LINE__, isp_cfg.others.hdrmge_cfg.ms_dif_0p8);
    for(int i = 0 ; i < 17; i++)
    {
        LOGE("%d: curve_0[%d] %d", __LINE__, i, isp_cfg.others.hdrmge_cfg.curve.curve_0[i]);
        LOGE("%d: curve_1[%d] %d", __LINE__, i, isp_cfg.others.hdrmge_cfg.curve.curve_1[i]);
        LOGE("%d: e_y[%d] %d", __LINE__, i, isp_cfg.others.hdrmge_cfg.e_y[i]);
    }

    LOGE("%d: cnt_vsize %d", __LINE__, isp_cfg.others.hdrtmo_cfg.cnt_vsize);
    LOGE("%d: gain_ld_off2 %d", __LINE__, isp_cfg.others.hdrtmo_cfg.gain_ld_off2);
    LOGE("%d: gain_ld_off1 %d", __LINE__, isp_cfg.others.hdrtmo_cfg.gain_ld_off1);
    LOGE("%d: big_en %d", __LINE__, isp_cfg.others.hdrtmo_cfg.big_en);
    LOGE("%d: nobig_en %d", __LINE__, isp_cfg.others.hdrtmo_cfg.nobig_en);
    LOGE("%d: newhst_en %d", __LINE__, isp_cfg.others.hdrtmo_cfg.newhst_en);
    LOGE("%d: cnt_mode %d", __LINE__, isp_cfg.others.hdrtmo_cfg.cnt_mode);
    LOGE("%d: expl_lgratio %d", __LINE__, isp_cfg.others.hdrtmo_cfg.expl_lgratio);
    LOGE("%d: lgscl_ratio %d", __LINE__, isp_cfg.others.hdrtmo_cfg.lgscl_ratio);
    LOGE("%d: cfg_alpha %d", __LINE__, isp_cfg.others.hdrtmo_cfg.cfg_alpha);
    LOGE("%d: set_gainoff %d", __LINE__, isp_cfg.others.hdrtmo_cfg.set_gainoff);
    LOGE("%d: set_palpha %d", __LINE__, isp_cfg.others.hdrtmo_cfg.set_palpha);
    LOGE("%d: set_lgmax %d", __LINE__, isp_cfg.others.hdrtmo_cfg.set_lgmax);
    LOGE("%d: set_lgmin %d", __LINE__, isp_cfg.others.hdrtmo_cfg.set_lgmin);
    LOGE("%d: set_weightkey %d", __LINE__, isp_cfg.others.hdrtmo_cfg.set_weightkey);
    LOGE("%d: set_lgmean %d", __LINE__, isp_cfg.others.hdrtmo_cfg.set_lgmean);
    LOGE("%d: set_lgrange1 %d", __LINE__, isp_cfg.others.hdrtmo_cfg.set_lgrange1);
    LOGE("%d: set_lgrange0 %d", __LINE__, isp_cfg.others.hdrtmo_cfg.set_lgrange0);
    LOGE("%d: set_lgavgmax %d", __LINE__, isp_cfg.others.hdrtmo_cfg.set_lgavgmax);
    LOGE("%d: clipgap1_i %d", __LINE__, isp_cfg.others.hdrtmo_cfg.clipgap1_i);
    LOGE("%d: clipgap0_i %d", __LINE__, isp_cfg.others.hdrtmo_cfg.clipgap0_i);
    LOGE("%d: clipratio1 %d", __LINE__, isp_cfg.others.hdrtmo_cfg.clipratio1);
    LOGE("%d: clipratio0 %d", __LINE__, isp_cfg.others.hdrtmo_cfg.clipratio0);
    LOGE("%d: ratiol %d", __LINE__, isp_cfg.others.hdrtmo_cfg.ratiol);
    LOGE("%d: lgscl_inv %d", __LINE__, isp_cfg.others.hdrtmo_cfg.lgscl_inv);
    LOGE("%d: lgscl %d", __LINE__, isp_cfg.others.hdrtmo_cfg.lgscl);
    LOGE("%d: lgmax %d", __LINE__, isp_cfg.others.hdrtmo_cfg.lgmax);
    LOGE("%d: hist_low %d", __LINE__, isp_cfg.others.hdrtmo_cfg.hist_low);
    LOGE("%d: hist_min %d", __LINE__, isp_cfg.others.hdrtmo_cfg.hist_min);
    LOGE("%d: hist_shift %d", __LINE__, isp_cfg.others.hdrtmo_cfg.hist_shift);
    LOGE("%d: hist_0p3 %d", __LINE__, isp_cfg.others.hdrtmo_cfg.hist_0p3);
    LOGE("%d: hist_high %d", __LINE__, isp_cfg.others.hdrtmo_cfg.hist_high);
    LOGE("%d: palpha_lwscl %d", __LINE__, isp_cfg.others.hdrtmo_cfg.palpha_lwscl);
    LOGE("%d: palpha_lw0p5 %d", __LINE__, isp_cfg.others.hdrtmo_cfg.palpha_lw0p5);
    LOGE("%d: palpha_0p18 %d", __LINE__, isp_cfg.others.hdrtmo_cfg.palpha_0p18);
    LOGE("%d: maxgain %d", __LINE__, isp_cfg.others.hdrtmo_cfg.maxgain);
    LOGE("%d: maxpalpha %d", __LINE__, isp_cfg.others.hdrtmo_cfg.maxpalpha);
    if(ahdr_data.hdrAttr.bEnable == true)
    {
        LOGE("%d:Ahdr API is ON!!!", __LINE__);
        if(ahdr_data.hdrAttr.opMode == 1)
        {
            LOGE("%d:Ahdr API use Atuo Mode!!!", __LINE__);
            if(ahdr_data.hdrAttr.stAuto.bUpdateMge == true)
                LOGE("%d:Merge datas are updated!!!", __LINE__);
            else
                LOGE("%d:Merge datas stay the same!!!", __LINE__);
            if(ahdr_data.hdrAttr.stAuto.bUpdateTmo == true)
                LOGE("%d:TMO datas are updated!!!", __LINE__);
            else
                LOGE("%d:TMO datas stay the same!!!", __LINE__);
        }
        else if(ahdr_data.hdrAttr.opMode == 2)
        {
            LOGE("%d:Ahdr API use Manual Mode!!!", __LINE__);
            if(ahdr_data.hdrAttr.stManual.bUpdateMge == true)
                LOGE("%d:Merge datas are updated!!!", __LINE__);
            else
                LOGE("%d:Merge datas stay the same!!!", __LINE__);
            if(ahdr_data.hdrAttr.stManual.bUpdateTmo == true)
                LOGE("%d:TMO datas are updated!!!", __LINE__);
            else
                LOGE("%d:TMO datas stay the same!!!", __LINE__);
        }

    }
    else
        LOGE("%d:Ahdr API is OFF!!!", __LINE__);
#endif
}

void
Isp20Params::convertAiqAfToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
                                       const rk_aiq_isp_af_meas_t& af_data)
{
    isp_cfg.meas.rawaf.rawaf_sel = af_data.gaus_flt_en;
    isp_cfg.meas.rawaf.gamma_en = af_data.gamma_flt_en;
    isp_cfg.meas.rawaf.gaus_en = af_data.gaus_flt_en;
    isp_cfg.meas.rawaf.afm_thres = af_data.afm_thres;
    isp_cfg.meas.rawaf.gaus_coe_h0 = af_data.gaus_h0;
    isp_cfg.meas.rawaf.gaus_coe_h1 = af_data.gaus_h1;
    isp_cfg.meas.rawaf.gaus_coe_h2 = af_data.gaus_h2;
    memcpy(isp_cfg.meas.rawaf.lum_var_shift,
           af_data.lum_var_shift, ISP2X_RAWAF_WIN_NUM * sizeof(unsigned char));
    memcpy(isp_cfg.meas.rawaf.afm_var_shift,
           af_data.afm_var_shift, ISP2X_RAWAF_WIN_NUM * sizeof(unsigned char));
    memcpy(isp_cfg.meas.rawaf.line_en,
           af_data.line_en, ISP2X_RAWAF_LINE_NUM * sizeof(unsigned char));
    memcpy(isp_cfg.meas.rawaf.line_num,
           af_data.line_num, ISP2X_RAWAF_LINE_NUM * sizeof(unsigned char));
    memcpy(isp_cfg.meas.rawaf.gamma_y,
           af_data.gamma_y, ISP2X_RAWAF_GAMMA_NUM * sizeof(unsigned short));

    isp_cfg.meas.rawaf.num_afm_win = af_data.window_num;
    isp_cfg.meas.rawaf.win[0].h_offs = af_data.wina_h_offs;
    isp_cfg.meas.rawaf.win[0].v_offs = af_data.wina_v_offs;
    isp_cfg.meas.rawaf.win[0].h_size = af_data.wina_h_size;
    isp_cfg.meas.rawaf.win[0].v_size = af_data.wina_v_size;
    isp_cfg.meas.rawaf.win[1].h_offs = af_data.winb_h_offs;
    isp_cfg.meas.rawaf.win[1].v_offs = af_data.winb_v_offs;
    isp_cfg.meas.rawaf.win[1].h_size = af_data.winb_h_size;
    isp_cfg.meas.rawaf.win[1].v_size = af_data.winb_v_size;
}

void
Isp20Params::convertAiqAwbGainToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
        const rk_aiq_wb_gain_t& awb_gain)
{
    struct isp2x_awb_gain_cfg *  cfg = &isp_cfg.others.awb_gain_cfg;
    cfg->gain_red       = (uint16_t)(0.5 + awb_gain.rgain * (1 << ISP2X_WBGAIN_FIXSCALE_BIT));
    cfg->gain_blue      = (uint16_t)(0.5 + awb_gain.bgain * (1 << ISP2X_WBGAIN_FIXSCALE_BIT));
    cfg->gain_green_r   = (uint16_t)(0.5 + awb_gain.grgain * (1 << ISP2X_WBGAIN_FIXSCALE_BIT));
    cfg->gain_green_b   = (uint16_t)(0.5 + awb_gain.gbgain * (1 << ISP2X_WBGAIN_FIXSCALE_BIT));
}
/*


        void Isp20Params::convertAiqAdehazeToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
                const isp2x_dhaz_cfg& dhaze                     )
        {
        int i;

             int rawWidth = 1920;
             int rawHeight =1080;
             struct isp2x_dhaz_cfg *  cfg = &isp_cfg.others.dhaz_cfg;



                 //cfg->       = int(rk_aiq_dehaze_cfg_t->dehaze_en[0]);  //0~1  , (1bit) dehaze_en
                 cfg->dc_en    = int(dhaze.dc_en);  //0~1  , (1bit) dc_en
                 cfg->hist_en          = int(dhaze.hist_en);  //0~1  , (1bit) hist_en
                 cfg->hist_chn         = int(dhaze.hist_chn);  //0~1  , (1bit) hist_channel
                 cfg->gain_en          = int(dhaze.gain_en);  //0~1  , (1bit) gain_en
                 cfg->dc_min_th    = int(dhaze.dc_min_th); //0~255, (8bit) dc_min_th
                 cfg->dc_max_th    = int(dhaze.dc_max_th                );  //0~255, (8bit) dc_max_th
                 cfg->yhist_th    = int(dhaze.yhist_th              );  //0~255, (8bit) yhist_th
                 cfg->yblk_th    = int(dhaze.yblk_th*((rawWidth+15) / 16)*((rawHeight+15) / 16)); //default:28,(9bit) yblk_th
                 cfg->dark_th    = int(dhaze.dark_th            );  //0~255, (8bit) dark_th
                 cfg->bright_min   = int(dhaze.bright_min               );  //0~255, (8bit) bright_min
                 cfg->bright_max   = int(dhaze.bright_max               );  //0~255, (8bit) bright_max
                 cfg->wt_max   = int(dhaze.wt_max*256           );  //0~255, (9bit) wt_max
                 cfg->air_min   = int(dhaze.air_min         );  //0~255, (8bit) air_min
                 cfg->air_max   = int(dhaze.air_max             );  //0~256, (8bit) air_max
                 cfg->tmax_base   = int(dhaze.tmax_base             );  //0~255, (8bit) tmax_base
                 cfg->tmax_off   = int(dhaze.tmax_off*1024          );  //0~1024,(11bit) tmax_off
                 cfg->tmax_max   = int(dhaze.tmax_max*1024          );  //0~1024,(11bit) tmax_max
                 cfg->hist_gratio   = int(dhaze.hist_gratio*8           );  //       (8bit) hist_gratio
                 cfg->hist_th_off   = int(dhaze.hist_th_off             );  //       (8bit) hist_th_off
                 cfg->hist_k   = int(dhaze.hist_k * 4 + 0.5 );  //0~7    (5bit),3bit+2bit, hist_k
                 cfg->hist_min   = int(dhaze.hist_min * 256     );  //       (9bit) hist_min
                 cfg->enhance_en       = int(dhaze.enhance_en               );  //0~1  , (1bit) enhance_en
                 cfg->enhance_value    = int(dhaze.enhance_value * 1024 + 0.5   );  //       (14bit),4bit + 10bit, enhance_value
                 cfg->hpara_en     = int(dhaze.hpara_en             );  //0~1  , (1bit) sw_hist_para_en
                 cfg->hist_scale       = int(dhaze.hist_scale * 256 + 0.5   );  //       (13bit),5bit + 8bit, sw_hist_scale
                 cfg->stab_fnum = int(dhaze.stab_fnum           );  //1~31,  (5bit) stab_fnum
                 cfg->iir_sigma = int(dhaze.iir_sigma           );  //0~255, (8bit) sigma
                 cfg->iir_wt_sigma = int(dhaze.iir_wt_sigma*8+0.5       );  //       (11bit),8bit+3bit, wt_sigma
                 cfg->iir_air_sigma = int(dhaze.iir_air_sigma           );  //       (8bit) air_sigma
                 cfg->iir_tmax_sigma = int(dhaze.iir_tmax_sigma * 1024 + 0.5);  //       (11bit) tmax_sigma
                 cfg->cfg_alpha = int(MIN(dhaze.cfg_alpha*256,255));    //0~255, (8bit) cfg_alpha
                 cfg->cfg_wt = int(dhaze.cfg_wt*256         );  //0~256, (9bit) cfg_wt
                 cfg->cfg_air = int(dhaze.cfg_air           );  //0~255, (8bit) cfg_air
                 cfg->cfg_tmax = int(dhaze.cfg_tmax*1024        );  //0~1024,(11bit) cfg_tmax
                 cfg->cfg_gratio = int(dhaze.cfg_gratio*256     );  //       (13bit),5bit+8bit, cfg_gratio
                 cfg->dc_thed      = int(dhaze.dc_thed              );  //0~255, (8bit) dc_thed
                 cfg->dc_weitcur       = int(dhaze.dc_weitcur * 256 + 0.5   );  //0~256, (9bit) dc_weitcur
                 cfg->air_thed     = int(dhaze.air_thed             );  //0~255, (8bit) air_thed
                 cfg->air_weitcur      = int(dhaze.air_weitcur * 256 + 0.5  );  //0~256, (9bit) air_weitcur


                     cfg->sw_dhaz_dc_bf_h0   = int(dhaze.sw_dhaz_dc_bf_h0);//h0~h5  从大到小
                     cfg->sw_dhaz_dc_bf_h1   = int(dhaze.sw_dhaz_dc_bf_h1);
                     cfg->sw_dhaz_dc_bf_h2   = int(dhaze.sw_dhaz_dc_bf_h2);
                     cfg->sw_dhaz_dc_bf_h3   = int(dhaze.sw_dhaz_dc_bf_h3);
                     cfg->sw_dhaz_dc_bf_h4   = int(dhaze.sw_dhaz_dc_bf_h4);
                     cfg->sw_dhaz_dc_bf_h5   = int(dhaze.sw_dhaz_dc_bf_h5);


                     cfg->air_bf_h0  = int(dhaze.air_bf_h0);//h0~h2  从大到小
                     cfg->air_bf_h1  = int(dhaze.air_bf_h1);
                     cfg->air_bf_h2  = int(dhaze.air_bf_h2);

                     cfg->gaus_h0    = int(dhaze.gaus_h0);//h0~h2   从大到小
                     cfg->gaus_h1    = int(dhaze.gaus_h1);
                     cfg->gaus_h2    = int(dhaze.gaus_h2);

               for (int i = 0; i < 6; i++)
                 {
                     cfg->conv_t0[i]     = int(dhaze.conv_t0[i]);
                     cfg->conv_t1[i]     = int(dhaze.conv_t1[i]);
                     cfg->conv_t2[i]     = int(dhaze.conv_t2[i]);
                 }




        }*/



void Isp20Params::convertAiqAgammaToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
        const rk_aiq_gamma_cfg_t& gamma_out_cfg)
{
    if(gamma_out_cfg.gamma_en) {
        isp_cfg.module_ens |= ISP2X_MODULE_GOC;
    }
    isp_cfg.module_en_update |= ISP2X_MODULE_GOC;
    isp_cfg.module_cfg_update |= ISP2X_MODULE_GOC;

    struct isp2x_gammaout_cfg* cfg = &isp_cfg.others.gammaout_cfg;
    cfg->offset = gamma_out_cfg.gamma_out_offset;
    cfg->equ_segm = gamma_out_cfg.gamma_out_segnum;
    for (int i = 0; i < 45 + 1; i++)
    {
        cfg->gamma_y[i] = gamma_out_cfg.gamma_table[i];
    }
}


void Isp20Params::convertAiqAdehazeToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
        const rk_aiq_dehaze_cfg_t& dhaze                     )
{
    int i;

    int rawWidth = 1920;
    int rawHeight = 1080;
    struct isp2x_dhaz_cfg *  cfg = &isp_cfg.others.dhaz_cfg;

    //cfg->       = int(rk_aiq_dehaze_cfg_t->dehaze_en[0]);  //0~1  , (1bit) dehaze_en
    cfg->dc_en    = int(dhaze.dehaze_en[1]);  //0~1  , (1bit) dc_en
    cfg->hist_en          = int(dhaze.dehaze_en[2]);  //0~1  , (1bit) hist_en
    cfg->hist_chn         = int(dhaze.dehaze_en[3]);  //0~1  , (1bit) hist_channel
    cfg->big_en           = int(dhaze.dehaze_en[4]);  //0~1  , (1bit) gain_en
    cfg->dc_min_th    = int(dhaze.dehaze_self_adp[0]); //0~255, (8bit) dc_min_th
    cfg->dc_max_th    = int(dhaze.dehaze_self_adp[1]               );  //0~255, (8bit) dc_max_th
    cfg->yhist_th    = int(dhaze.dehaze_self_adp[2]                );  //0~255, (8bit) yhist_th
    cfg->yblk_th    = int(dhaze.dehaze_self_adp[3] * ((rawWidth + 15) / 16) * ((rawHeight + 15) / 16)); //default:28,(9bit) yblk_th
    cfg->dark_th    = int(dhaze.dehaze_self_adp[4]             );  //0~255, (8bit) dark_th
    cfg->bright_min   = int(dhaze.dehaze_self_adp[5]               );  //0~255, (8bit) bright_min
    cfg->bright_max   = int(dhaze.dehaze_self_adp[6]               );  //0~255, (8bit) bright_max
    cfg->wt_max   = int(dhaze.dehaze_range_adj[0] * 256          ); //0~255, (9bit) wt_max
    cfg->air_min   = int(dhaze.dehaze_range_adj[1]             );  //0~255, (8bit) air_min
    cfg->air_max   = int(dhaze.dehaze_range_adj[2]             );  //0~256, (8bit) air_max
    cfg->tmax_base   = int(dhaze.dehaze_range_adj[3]               );  //0~255, (8bit) tmax_base
    cfg->tmax_off   = int(dhaze.dehaze_range_adj[4] * 1024           ); //0~1024,(11bit) tmax_off
    cfg->tmax_max   = int(dhaze.dehaze_range_adj[5] * 1024           ); //0~1024,(11bit) tmax_max
    cfg->hist_gratio   = int(dhaze.dehaze_hist_para[0] * 8           ); //       (8bit) hist_gratio
    cfg->hist_th_off   = int(dhaze.dehaze_hist_para[1]             );  //       (8bit) hist_th_off
    cfg->hist_k   = int(dhaze.dehaze_hist_para[2] * 4 + 0.5    );  //0~7    (5bit),3bit+2bit, hist_k
    cfg->hist_min   = int(dhaze.dehaze_hist_para[3] * 256      );  //       (9bit) hist_min
    cfg->enhance_en       = int(dhaze.dehaze_enhance[0]                );  //0~1  , (1bit) enhance_en
    cfg->enhance_value    = int(dhaze.dehaze_enhance[1] * 1024 + 0.5   );  //       (14bit),4bit + 10bit, enhance_value
    cfg->hpara_en     = int(dhaze.dehaze_enhance[2]                );  //0~1  , (1bit) sw_hist_para_en
    cfg->hist_scale       = int(dhaze.dehaze_enhance[3] *  256 + 0.5   );  //       (13bit),5bit + 8bit, sw_hist_scale
    cfg->stab_fnum = int(dhaze.dehaze_iir_control[0]           );  //1~31,  (5bit) stab_fnum
    cfg->iir_sigma = int(dhaze.dehaze_iir_control[1]           );  //0~255, (8bit) sigma
    cfg->iir_wt_sigma = int(dhaze.dehaze_iir_control[2] * 8 + 0.5      ); //       (11bit),8bit+3bit, wt_sigma
    cfg->iir_air_sigma = int(dhaze.dehaze_iir_control[3]           );  //       (8bit) air_sigma
    cfg->iir_tmax_sigma = int(dhaze.dehaze_iir_control[4] * 1024 + 0.5);   //       (11bit) tmax_sigma
    cfg->cfg_alpha = int(MIN(dhaze.dehaze_user_config[0] * 256, 255)); //0~255, (8bit) cfg_alpha
    cfg->cfg_air = int(dhaze.dehaze_user_config[1] * 256         ); //0~256, (9bit) cfg_wt
    cfg->cfg_air = int(dhaze.dehaze_user_config[2]             );  //0~255, (8bit) cfg_air
    cfg->cfg_tmax = int(dhaze.dehaze_user_config[3] * 1024       ); //0~1024,(11bit) cfg_tmax
    cfg->cfg_gratio = int(dhaze.dehaze_user_config[4] * 256      ); //       (13bit),5bit+8bit, cfg_gratio
    cfg->dc_thed      = int(dhaze.dehaze_bi_para[0]                );  //0~255, (8bit) dc_thed
    cfg->dc_weitcur       = int(dhaze.dehaze_bi_para[1] * 256 + 0.5    );  //0~256, (9bit) dc_weitcur
    cfg->air_thed     = int(dhaze.dehaze_bi_para[2]                );  //0~255, (8bit) air_thed
    cfg->air_weitcur      = int(dhaze.dehaze_bi_para[3] * 256 + 0.5    );  //0~256, (9bit) air_weitcur


    cfg->sw_dhaz_dc_bf_h0   = int(dhaze.dehaze_dc_bf_h[12]);//h0~h5  从大到小
    cfg->sw_dhaz_dc_bf_h1   = int(dhaze.dehaze_dc_bf_h[7]);
    cfg->sw_dhaz_dc_bf_h2   = int(dhaze.dehaze_dc_bf_h[6]);
    cfg->sw_dhaz_dc_bf_h3   = int(dhaze.dehaze_dc_bf_h[2]);
    cfg->sw_dhaz_dc_bf_h4   = int(dhaze.dehaze_dc_bf_h[1]);
    cfg->sw_dhaz_dc_bf_h5   = int(dhaze.dehaze_dc_bf_h[0]);


    cfg->air_bf_h0  = int(dhaze.dehaze_air_bf_h[4]);//h0~h2  从大到小
    cfg->air_bf_h1  = int(dhaze.dehaze_air_bf_h[1]);
    cfg->air_bf_h2  = int(dhaze.dehaze_air_bf_h[0]);

    cfg->gaus_h0    = int(dhaze.dehaze_gaus_h[4]);//h0~h2  从大到小
    cfg->gaus_h1    = int(dhaze.dehaze_gaus_h[1]);
    cfg->gaus_h2    = int(dhaze.dehaze_gaus_h[0]);

    for (int i = 0; i < 6; i++)
    {
        cfg->conv_t0[i]     = int(dhaze.dehaze_hist_t0[i]);
        cfg->conv_t1[i]     = int(dhaze.dehaze_hist_t1[i]);
        cfg->conv_t2[i]     = int(dhaze.dehaze_hist_t2[i]);
    }

}


void
Isp20Params::convertAiqBlcToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
                                        SmartPtr<RkAiqIspParamsProxy> aiq_results)
{
    LOGD("%s:(%d) enter \n", __FUNCTION__, __LINE__);

	if(aiq_results->data()->blc.stResult.enable){
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

    isp_cfg.others.bls_cfg.fixed_val.r = aiq_results->data()->blc.stResult.blc_gr;
    isp_cfg.others.bls_cfg.fixed_val.gr = aiq_results->data()->blc.stResult.blc_gr;
    isp_cfg.others.bls_cfg.fixed_val.gb = aiq_results->data()->blc.stResult.blc_gr;
    isp_cfg.others.bls_cfg.fixed_val.b = aiq_results->data()->blc.stResult.blc_gr;

    LOGD("%s:(%d) exit \n", __FUNCTION__, __LINE__);
}


void
Isp20Params::convertAiqDpccToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
        SmartPtr<RkAiqIspParamsProxy> aiq_results)
{
    LOGD("%s:(%d) enter \n", __FUNCTION__, __LINE__);

    struct isp2x_dpcc_cfg * pDpccCfg = &isp_cfg.others.dpcc_cfg;
    rk_aiq_isp_dpcc_t *pDpccRst = &aiq_results->data()->dpcc;

    if(pDpccRst->stBasic.enable) {
        isp_cfg.module_ens |= ISP2X_MODULE_DPCC;
    }
    isp_cfg.module_en_update |= ISP2X_MODULE_DPCC;
    isp_cfg.module_cfg_update |= ISP2X_MODULE_DPCC;

    //mode 0x0000
    pDpccCfg->stage1_enable = pDpccRst->stBasic.stage1_enable;
    pDpccCfg->grayscale_mode = pDpccRst->stBasic.grayscale_mode;
    //pDpccCfg->enable = pDpccRst->stBasic.enable;

    //output_mode 0x0004
    pDpccCfg->sw_rk_out_sel = pDpccRst->stBasic.sw_rk_out_sel;
    pDpccCfg->sw_dpcc_output_sel = pDpccRst->stBasic.sw_dpcc_output_sel;
    pDpccCfg->stage1_rb_3x3 = pDpccRst->stBasic.stage1_rb_3x3;
    pDpccCfg->stage1_g_3x3 = pDpccRst->stBasic.stage1_g_3x3;
    pDpccCfg->stage1_incl_rb_center = pDpccRst->stBasic.stage1_incl_rb_center;
    pDpccCfg->stage1_incl_green_center = pDpccRst->stBasic.stage1_incl_green_center;

    //set_use 0x0008
    pDpccCfg->stage1_use_fix_set = pDpccRst->stBasic.stage1_use_fix_set;
    pDpccCfg->stage1_use_set_3 = pDpccRst->stBasic.stage1_use_set_3;
    pDpccCfg->stage1_use_set_2 = pDpccRst->stBasic.stage1_use_set_2;
    pDpccCfg->stage1_use_set_1 = pDpccRst->stBasic.stage1_use_set_1;

    //methods_set_1 0x000c
    pDpccCfg->sw_rk_red_blue1_en = pDpccRst->stBasic.sw_rk_red_blue1_en;
    pDpccCfg->rg_red_blue1_enable = pDpccRst->stBasic.rg_red_blue1_enable;
    pDpccCfg->rnd_red_blue1_enable = pDpccRst->stBasic.rnd_red_blue1_enable;
    pDpccCfg->ro_red_blue1_enable = pDpccRst->stBasic.ro_red_blue1_enable;
    pDpccCfg->lc_red_blue1_enable = pDpccRst->stBasic.lc_red_blue1_enable;
    pDpccCfg->pg_red_blue1_enable = pDpccRst->stBasic.pg_red_blue1_enable;
    pDpccCfg->sw_rk_green1_en = pDpccRst->stBasic.sw_rk_green1_en;
    pDpccCfg->rg_green1_enable = pDpccRst->stBasic.rg_green1_enable;
    pDpccCfg->rnd_green1_enable = pDpccRst->stBasic.rnd_green1_enable;
    pDpccCfg->ro_green1_enable = pDpccRst->stBasic.ro_green1_enable;
    pDpccCfg->lc_green1_enable = pDpccRst->stBasic.lc_green1_enable;
    pDpccCfg->pg_green1_enable = pDpccRst->stBasic.pg_green1_enable;

    //methods_set_2 0x0010
    pDpccCfg->sw_rk_red_blue2_en = pDpccRst->stBasic.sw_rk_red_blue2_en;
    pDpccCfg->rg_red_blue2_enable = pDpccRst->stBasic.rg_red_blue2_enable;
    pDpccCfg->rnd_red_blue2_enable = pDpccRst->stBasic.rnd_red_blue2_enable;
    pDpccCfg->ro_red_blue2_enable = pDpccRst->stBasic.ro_red_blue2_enable;
    pDpccCfg->lc_red_blue2_enable = pDpccRst->stBasic.lc_red_blue2_enable;
    pDpccCfg->pg_red_blue2_enable = pDpccRst->stBasic.pg_red_blue2_enable;
    pDpccCfg->sw_rk_green2_en = pDpccRst->stBasic.sw_rk_green2_en;
    pDpccCfg->rg_green2_enable = pDpccRst->stBasic.rg_green2_enable;
    pDpccCfg->rnd_green2_enable = pDpccRst->stBasic.rnd_green2_enable;
    pDpccCfg->ro_green2_enable = pDpccRst->stBasic.ro_green2_enable;
    pDpccCfg->lc_green2_enable = pDpccRst->stBasic.lc_green2_enable;
    pDpccCfg->pg_green2_enable = pDpccRst->stBasic.pg_green2_enable;

    //methods_set_3 0x0014
    pDpccCfg->sw_rk_red_blue3_en = pDpccRst->stBasic.sw_rk_red_blue3_en;
    pDpccCfg->rg_red_blue3_enable = pDpccRst->stBasic.rg_red_blue3_enable;
    pDpccCfg->rnd_red_blue3_enable = pDpccRst->stBasic.rnd_red_blue3_enable;
    pDpccCfg->ro_red_blue3_enable = pDpccRst->stBasic.ro_red_blue3_enable;
    pDpccCfg->lc_red_blue3_enable = pDpccRst->stBasic.lc_red_blue3_enable;
    pDpccCfg->pg_red_blue3_enable = pDpccRst->stBasic.pg_red_blue3_enable;
    pDpccCfg->sw_rk_green3_en = pDpccRst->stBasic.sw_rk_green3_en;
    pDpccCfg->rg_green3_enable = pDpccRst->stBasic.rg_green3_enable;
    pDpccCfg->rnd_green3_enable = pDpccRst->stBasic.rnd_green3_enable;
    pDpccCfg->ro_green3_enable = pDpccRst->stBasic.ro_green3_enable;
    pDpccCfg->lc_green3_enable = pDpccRst->stBasic.lc_green3_enable;
    pDpccCfg->pg_green3_enable = pDpccRst->stBasic.pg_green3_enable;

    //line_thresh_1 0x0018
    pDpccCfg->sw_mindis1_rb = pDpccRst->stBasic.sw_mindis1_rb;
    pDpccCfg->sw_mindis1_g = pDpccRst->stBasic.sw_mindis1_g;
    pDpccCfg->line_thr_1_rb = pDpccRst->stBasic.line_thr_1_rb;
    pDpccCfg->line_thr_1_g = pDpccRst->stBasic.line_thr_1_g;

    //line_mad_fac_1 0x001c
    pDpccCfg->sw_dis_scale_min1 = pDpccRst->stBasic.sw_dis_scale_min1;
    pDpccCfg->sw_dis_scale_max1 = pDpccRst->stBasic.sw_dis_scale_max1;
    pDpccCfg->line_mad_fac_1_rb = pDpccRst->stBasic.line_mad_fac_1_rb;
    pDpccCfg->line_mad_fac_1_g = pDpccRst->stBasic.line_mad_fac_1_g;

    //pg_fac_1 0x0020
    pDpccCfg->pg_fac_1_rb = pDpccRst->stBasic.pg_fac_1_rb;
    pDpccCfg->pg_fac_1_g = pDpccRst->stBasic.pg_fac_1_g;

    //rnd_thresh_1 0x0024
    pDpccCfg->rnd_thr_1_rb = pDpccRst->stBasic.rnd_thr_1_rb;
    pDpccCfg->rnd_thr_1_g = pDpccRst->stBasic.rnd_thr_1_g;

    //rg_fac_1 0x0028
    pDpccCfg->rg_fac_1_rb = pDpccRst->stBasic.rg_fac_1_rb;
    pDpccCfg->rg_fac_1_g = pDpccRst->stBasic.rg_fac_1_g;


    //line_thresh_2 0x002c
    pDpccCfg->sw_mindis2_rb = pDpccRst->stBasic.sw_mindis2_rb;
    pDpccCfg->sw_mindis2_g = pDpccRst->stBasic.sw_mindis2_g;
    pDpccCfg->line_thr_2_rb = pDpccRst->stBasic.line_thr_2_rb;
    pDpccCfg->line_thr_2_g = pDpccRst->stBasic.line_thr_2_g;

    //line_mad_fac_2 0x0030
    pDpccCfg->sw_dis_scale_min2 = pDpccRst->stBasic.sw_dis_scale_min2;
    pDpccCfg->sw_dis_scale_max2 = pDpccRst->stBasic.sw_dis_scale_max2;
    pDpccCfg->line_mad_fac_2_rb = pDpccRst->stBasic.line_mad_fac_2_rb;
    pDpccCfg->line_mad_fac_2_g = pDpccRst->stBasic.line_mad_fac_2_g;

    //pg_fac_2 0x0034
    pDpccCfg->pg_fac_2_rb = pDpccRst->stBasic.pg_fac_2_rb;
    pDpccCfg->pg_fac_2_g = pDpccRst->stBasic.pg_fac_2_g;

    //rnd_thresh_2 0x0038
    pDpccCfg->rnd_thr_2_rb = pDpccRst->stBasic.rnd_thr_2_rb;
    pDpccCfg->rnd_thr_2_g = pDpccRst->stBasic.rnd_thr_2_g;

    //rg_fac_2 0x003c
    pDpccCfg->rg_fac_2_rb = pDpccRst->stBasic.rg_fac_2_rb;
    pDpccCfg->rg_fac_2_g = pDpccRst->stBasic.rg_fac_2_g;


    //line_thresh_3 0x0040
    pDpccCfg->sw_mindis3_rb = pDpccRst->stBasic.sw_mindis3_rb;
    pDpccCfg->sw_mindis3_g = pDpccRst->stBasic.sw_mindis3_g;
    pDpccCfg->line_thr_3_rb = pDpccRst->stBasic.line_thr_3_rb;
    pDpccCfg->line_thr_3_g = pDpccRst->stBasic.line_thr_3_g;

    //line_mad_fac_3 0x0044
    pDpccCfg->sw_dis_scale_min3 = pDpccRst->stBasic.sw_dis_scale_min3;
    pDpccCfg->sw_dis_scale_max3 = pDpccRst->stBasic.sw_dis_scale_max3;
    pDpccCfg->line_mad_fac_3_rb = pDpccRst->stBasic.line_mad_fac_3_rb;
    pDpccCfg->line_mad_fac_3_g = pDpccRst->stBasic.line_mad_fac_3_g;

    //pg_fac_3 0x0048
    pDpccCfg->pg_fac_3_rb = pDpccRst->stBasic.pg_fac_3_rb;
    pDpccCfg->pg_fac_3_g = pDpccRst->stBasic.pg_fac_3_g;

    //rnd_thresh_3 0x004c
    pDpccCfg->rnd_thr_3_rb = pDpccRst->stBasic.rnd_thr_3_rb;
    pDpccCfg->rnd_thr_3_g = pDpccRst->stBasic.rnd_thr_3_g;

    //rg_fac_3 0x0050
    pDpccCfg->rg_fac_3_rb = pDpccRst->stBasic.rg_fac_3_rb;
    pDpccCfg->rg_fac_3_g = pDpccRst->stBasic.rg_fac_3_g;

    //ro_limits 0x0054
    pDpccCfg->ro_lim_3_rb = pDpccRst->stBasic.ro_lim_3_rb;
    pDpccCfg->ro_lim_3_g = pDpccRst->stBasic.ro_lim_3_g;
    pDpccCfg->ro_lim_2_rb = pDpccRst->stBasic.ro_lim_2_rb;;
    pDpccCfg->ro_lim_2_g = pDpccRst->stBasic.ro_lim_2_g;
    pDpccCfg->ro_lim_1_rb = pDpccRst->stBasic.ro_lim_1_rb;
    pDpccCfg->ro_lim_1_g = pDpccRst->stBasic.ro_lim_1_g;

    //rnd_offs 0x0058
    pDpccCfg->rnd_offs_3_rb = pDpccRst->stBasic.rnd_offs_3_rb;
    pDpccCfg->rnd_offs_3_g = pDpccRst->stBasic.rnd_offs_3_g;
    pDpccCfg->rnd_offs_2_rb = pDpccRst->stBasic.rnd_offs_2_rb;
    pDpccCfg->rnd_offs_2_g = pDpccRst->stBasic.rnd_offs_2_g;
    pDpccCfg->rnd_offs_1_rb = pDpccRst->stBasic.rnd_offs_1_rb;
    pDpccCfg->rnd_offs_1_g = pDpccRst->stBasic.rnd_offs_1_g;

    //bpt_ctrl 0x005c
    pDpccCfg->bpt_rb_3x3 = pDpccRst->stBpt.bpt_rb_3x3;
    pDpccCfg->bpt_g_3x3 = pDpccRst->stBpt.bpt_g_3x3;
    pDpccCfg->bpt_incl_rb_center = pDpccRst->stBpt.bpt_incl_rb_center;
    pDpccCfg->bpt_incl_green_center = pDpccRst->stBpt.bpt_incl_green_center;
    pDpccCfg->bpt_use_fix_set = pDpccRst->stBpt.bpt_use_fix_set;
    pDpccCfg->bpt_use_set_3 = pDpccRst->stBpt.bpt_use_set_3;
    pDpccCfg->bpt_use_set_2 = pDpccRst->stBpt.bpt_use_set_2;
    pDpccCfg->bpt_use_set_1 = pDpccRst->stBpt.bpt_use_set_1;
    pDpccCfg->bpt_cor_en = pDpccRst->stBpt.bpt_cor_en;
    pDpccCfg->bpt_det_en = pDpccRst->stBpt.bpt_det_en;

    //bpt_number 0x0060
    pDpccCfg->bp_number = pDpccRst->stBpt.bp_number;

    //bpt_addr 0x0064
    pDpccCfg->bp_table_addr = pDpccRst->stBpt.bp_table_addr;

    //bpt_data 0x0068
    pDpccCfg->bpt_v_addr = pDpccRst->stBpt.bpt_v_addr;
    pDpccCfg->bpt_h_addr = pDpccRst->stBpt.bpt_h_addr;

    //bp_cnt 0x006c
    pDpccCfg->bp_cnt = pDpccRst->stBpt.bp_cnt;

    //pdaf_en 0x0070
    pDpccCfg->sw_pdaf_en = pDpccRst->stPdaf.sw_pdaf_en;

    //pdaf_point_en 0x0074
    for(int i = 0; i < ISP2X_DPCC_PDAF_POINT_NUM; i++) {
        pDpccCfg->pdaf_point_en[i] = pDpccRst->stPdaf.pdaf_point_en[i];
    }

    //pdaf_offset 0x0078
    pDpccCfg->pdaf_offsety = pDpccRst->stPdaf.pdaf_offsety;
    pDpccCfg->pdaf_offsetx = pDpccRst->stPdaf.pdaf_offsetx;

    //pdaf_wrap 0x007c
    pDpccCfg->pdaf_wrapy = pDpccRst->stPdaf.pdaf_wrapy;
    pDpccCfg->pdaf_wrapx = pDpccRst->stPdaf.pdaf_wrapx;

    //pdaf_scope 0x0080
    pDpccCfg->pdaf_wrapy_num = pDpccRst->stPdaf.pdaf_wrapy_num;
    pDpccCfg->pdaf_wrapx_num = pDpccRst->stPdaf.pdaf_wrapx_num;

    //pdaf_point_0 0x0084
    for(int i = 0; i < ISP2X_DPCC_PDAF_POINT_NUM; i++) {
        pDpccCfg->point[i].x = pDpccRst->stPdaf.point[i].x;
        pDpccCfg->point[i].y = pDpccRst->stPdaf.point[i].y;
    }

    //pdaf_forward_med 0x00a4
    pDpccCfg->pdaf_forward_med = pDpccRst->stPdaf.pdaf_forward_med;


    LOGD("%s:(%d) exit \n", __FUNCTION__, __LINE__);
}


void
Isp20Params::convertAiqLscToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
                                        const rk_aiq_lsc_cfg_t& lsc)
{

    if(lsc.lsc_en) {
        isp_cfg.module_ens |= ISP2X_MODULE_LSC;
    }
    isp_cfg.module_en_update |= ISP2X_MODULE_LSC;
    isp_cfg.module_cfg_update |= ISP2X_MODULE_LSC;

    struct isp2x_lsc_cfg *  cfg = &isp_cfg.others.lsc_cfg;
    memcpy(cfg->x_size_tbl, lsc.x_size_tbl, sizeof(lsc.x_size_tbl));
    memcpy(cfg->y_size_tbl, lsc.y_size_tbl, sizeof(lsc.y_size_tbl));
    memcpy(cfg->x_grad_tbl, lsc.x_grad_tbl, sizeof(lsc.x_grad_tbl));
    memcpy(cfg->y_grad_tbl, lsc.y_grad_tbl, sizeof(lsc.y_grad_tbl));

    memcpy(cfg->r_data_tbl, lsc.r_data_tbl, sizeof(lsc.r_data_tbl));
    memcpy(cfg->gr_data_tbl, lsc.gr_data_tbl, sizeof(lsc.gr_data_tbl));
    memcpy(cfg->gb_data_tbl, lsc.gb_data_tbl, sizeof(lsc.gb_data_tbl));
    memcpy(cfg->b_data_tbl, lsc.b_data_tbl, sizeof(lsc.b_data_tbl));
}
void Isp20Params::convertAiqCcmToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
        const rk_aiq_ccm_cfg_t& ccm)
{
    if(ccm.ccmEnable) {
        isp_cfg.module_ens |= ISP2X_MODULE_CCM;
    }
    isp_cfg.module_en_update |= ISP2X_MODULE_CCM;
    isp_cfg.module_cfg_update |= ISP2X_MODULE_CCM;

    struct isp2x_ccm_cfg *  cfg = &isp_cfg.others.ccm_cfg;
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


    cfg->offset_r = offset[0] > 0 ? (short)(offset[0] + 0.5) : (short)(offset[0] - 0.5);//check, for 12bit?
    cfg->offset_g = offset[1] > 0 ? (short)(offset[1] + 0.5) : (int)(offset[1] - 0.5);
    cfg->offset_b = offset[2] > 0 ? (short)(offset[2] + 0.5) : (short)(offset[2] - 0.5);

    cfg->coeff0_y = (u16 )ccm.rgb2y_para[0];
    cfg->coeff1_y = (u16 )ccm.rgb2y_para[1];
    cfg->coeff2_y = (u16 )ccm.rgb2y_para[2];
    cfg->bound_bit = (u8)ccm.bound_bit;//check

    for( int i = 0; i < 17; i++)
    {
        cfg->alp_y[i] = (u16)(ccm.alp_y[i]);
    }


}
void Isp20Params::convertAiqA3dlutToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
        const rk_aiq_lut3d_cfg_t& lut3d_cfg)
{
    if(lut3d_cfg.enable) {
        isp_cfg.module_ens |= ISP2X_MODULE_3DLUT;
    }
    isp_cfg.module_en_update |= ISP2X_MODULE_3DLUT;
    isp_cfg.module_cfg_update |= ISP2X_MODULE_3DLUT;

    struct isp2x_3dlut_cfg* cfg = &isp_cfg.others.isp3dlut_cfg;
    cfg->bypass_en = lut3d_cfg.bypass_en;
    cfg->actual_size = lut3d_cfg.lut3d_lut_wsize;
    memcpy(cfg->lut_r, lut3d_cfg.look_up_table_r, sizeof(cfg->lut_r));
    memcpy(cfg->lut_g, lut3d_cfg.look_up_table_g, sizeof(cfg->lut_g));
    memcpy(cfg->lut_b, lut3d_cfg.look_up_table_b, sizeof(cfg->lut_b));
}

void
Isp20Params::convertAiqRawnrToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
        rk_aiq_isp_rawnr_t& rawnr)
{

    LOGD("%s:(%d) enter \n", __FUNCTION__, __LINE__);

    struct isp2x_rawnr_cfg * pRawnrCfg = &isp_cfg.others.rawnr_cfg;

	if(rawnr.rawnr_en){
    	isp_cfg.module_ens |= ISP2X_MODULE_RAWNR;
	}
    isp_cfg.module_en_update |= ISP2X_MODULE_RAWNR;
    isp_cfg.module_cfg_update |= ISP2X_MODULE_RAWNR;

    int rawbit = 12;//rawBit;
    float tmp;

    //(0x0004)
    pRawnrCfg->gauss_en = rawnr.gauss_en;
    pRawnrCfg->log_bypass = rawnr.log_bypass;

    //(0x0008 - 0x0010)
    pRawnrCfg->filtpar0 = rawnr.filtpar0;
    pRawnrCfg->filtpar1 = rawnr.filtpar1;
    pRawnrCfg->filtpar2 = rawnr.filtpar2;

    //(0x0014 - 0x0001c)
    pRawnrCfg->dgain0 = rawnr.dgain0;
    pRawnrCfg->dgain1 = rawnr.dgain1;
    pRawnrCfg->dgain2 = rawnr.dgain2;

    //(0x0020 - 0x0002c)
    for(int i = 0; i < ISP2X_RAWNR_LUMA_RATION_NUM; i++) {
        pRawnrCfg->luration[i] = rawnr.luration[i];
    }

    //(0x0030 - 0x0003c)
    for(int i = 0; i < ISP2X_RAWNR_LUMA_RATION_NUM; i++) {
        pRawnrCfg->lulevel[i] = rawnr.lulevel[i];
    }

    //(0x0040)
    pRawnrCfg->gauss = rawnr.gauss;

    //(0x0044)
    pRawnrCfg->sigma = rawnr.sigma;

    //(0x0048)
    pRawnrCfg->pix_diff = rawnr.pix_diff;

    //(0x004c)
    pRawnrCfg->thld_diff = rawnr.thld_diff;

    //(0x0050)
    pRawnrCfg->gas_weig_scl1 = rawnr.gas_weig_scl1;
    pRawnrCfg->gas_weig_scl2 = rawnr.gas_weig_scl2;
    pRawnrCfg->thld_chanelw = rawnr.thld_chanelw;

    //(0x0054)
    pRawnrCfg->lamda = rawnr.lamda;

    //(0x0058 - 0x0005c)
    pRawnrCfg->fixw0 = rawnr.fixw0;
    pRawnrCfg->fixw1 = rawnr.fixw1;
    pRawnrCfg->fixw2 = rawnr.fixw2;
    pRawnrCfg->fixw3 = rawnr.fixw3;

    //(0x0060 - 0x00068)
    pRawnrCfg->wlamda0 = rawnr.wlamda0;
    pRawnrCfg->wlamda1 = rawnr.wlamda1;
    pRawnrCfg->wlamda2 = rawnr.wlamda2;

    //(0x006c)
    pRawnrCfg->rgain_filp = rawnr.rgain_filp;
    pRawnrCfg->bgain_filp = rawnr.bgain_filp;

    LOGD("%s:(%d) exit \n", __FUNCTION__, __LINE__);
}

void
Isp20Params::convertAiqTnrToIsp20Params(struct rkispp_params_cfg& pp_cfg,
                                        rk_aiq_isp_tnr_t& tnr)
{
    LOGD("%s:(%d) enter \n", __FUNCTION__, __LINE__);
    int i = 0;

	if(tnr.tnr_en){
    	pp_cfg.module_ens |= ISPP_MODULE_TNR;
	}
    pp_cfg.module_en_update |= ISPP_MODULE_TNR;
    pp_cfg.module_cfg_update |= ISPP_MODULE_TNR;

    struct rkispp_tnr_config  * pTnrCfg = &pp_cfg.tnr_cfg;

    //0x0080
    pTnrCfg->mode = tnr.mode;
    pTnrCfg->opty_en = tnr.opty_en;
    pTnrCfg->optc_en = tnr.optc_en;
    pTnrCfg->gain_en = tnr.gain_en;

    //0x0088
    pTnrCfg->pk0_y = tnr.pk0_y;
    pTnrCfg->pk1_y = tnr.pk1_y;
    pTnrCfg->pk0_c = tnr.pk0_c;
    pTnrCfg->pk1_c = tnr.pk1_c;

    //0x008c
    pTnrCfg->glb_gain_cur = tnr.glb_gain_cur;
    pTnrCfg->glb_gain_nxt = tnr.glb_gain_nxt;

    //0x0090
    pTnrCfg->glb_gain_cur_div = tnr.glb_gain_cur_div;
    pTnrCfg->glb_gain_cur_sqrt = tnr.glb_gain_cur_sqrt;

    //0x0094 - 0x0098
    for(i = 0; i < TNR_SIGMA_CURVE_SIZE - 1; i++) {
        pTnrCfg->sigma_x[i] = tnr.sigma_x[i];
    }

    //0x009c - 0x00bc
    for(i = 0; i < TNR_SIGMA_CURVE_SIZE; i++) {
        pTnrCfg->sigma_y[i] = tnr.sigma_y[i];
    }

    //0x00c4 - 0x00cc
    for(i = 0; i < TNR_LUMA_CURVE_SIZE; i++) {
        pTnrCfg->luma_curve[i] = tnr.luma_curve[i];
    }

    //0x00d0
    pTnrCfg->txt_th0_y = tnr.txt_th0_y;
    pTnrCfg->txt_th1_y = tnr.txt_th1_y;

    //0x00d4
    pTnrCfg->txt_th0_c = tnr.txt_th0_c;
    pTnrCfg->txt_th1_c = tnr.txt_th1_c;

    //0x00d8
    pTnrCfg->txt_thy_dlt = tnr.txt_thy_dlt;
    pTnrCfg->txt_thc_dlt = tnr.txt_thc_dlt;

    //0x00dc - 0x00ec
    for(i = 0; i < TNR_GFCOEF6_SIZE; i++) {
        pTnrCfg->gfcoef_y0[i] = tnr.gfcoef_y0[i];
    }
    for(i = 0; i < TNR_GFCOEF3_SIZE; i++) {
        pTnrCfg->gfcoef_y1[i] = tnr.gfcoef_y1[i];
    }
    for(i = 0; i < TNR_GFCOEF3_SIZE; i++) {
        pTnrCfg->gfcoef_y2[i] = tnr.gfcoef_y2[i];
    }
    for(i = 0; i < TNR_GFCOEF3_SIZE; i++) {
        pTnrCfg->gfcoef_y3[i] = tnr.gfcoef_y3[i];
    }

    //0x00f0 - 0x0100
    for(i = 0; i < TNR_GFCOEF6_SIZE; i++) {
        pTnrCfg->gfcoef_yg0[i] = tnr.gfcoef_yg0[i];
    }
    for(i = 0; i < TNR_GFCOEF3_SIZE; i++) {
        pTnrCfg->gfcoef_yg1[i] = tnr.gfcoef_yg1[i];
    }
    for(i = 0; i < TNR_GFCOEF3_SIZE; i++) {
        pTnrCfg->gfcoef_yg2[i] = tnr.gfcoef_yg2[i];
    }
    for(i = 0; i < TNR_GFCOEF3_SIZE; i++) {
        pTnrCfg->gfcoef_yg3[i] = tnr.gfcoef_yg3[i];
    }

    //0x0104 - 0x0110
    for(i = 0; i < TNR_GFCOEF6_SIZE; i++) {
        pTnrCfg->gfcoef_yl0[i] = tnr.gfcoef_yl0[i];
    }
    for(i = 0; i < TNR_GFCOEF3_SIZE; i++) {
        pTnrCfg->gfcoef_yl1[i] = tnr.gfcoef_yl1[i];
    }
    for(i = 0; i < TNR_GFCOEF3_SIZE; i++) {
        pTnrCfg->gfcoef_yl2[i] = tnr.gfcoef_yl2[i];
    }

    //0x0114 - 0x0120
    for(i = 0; i < TNR_GFCOEF6_SIZE; i++) {
        pTnrCfg->gfcoef_cg0[i] = tnr.gfcoef_cg0[i];
    }
    for(i = 0; i < TNR_GFCOEF3_SIZE; i++) {
        pTnrCfg->gfcoef_cg1[i] = tnr.gfcoef_cg1[i];
    }
    for(i = 0; i < TNR_GFCOEF3_SIZE; i++) {
        pTnrCfg->gfcoef_cg2[i] = tnr.gfcoef_cg2[i];
    }

    //0x0124 - 0x012c
    for(i = 0; i < TNR_GFCOEF6_SIZE; i++) {
        pTnrCfg->gfcoef_cl0[i] = tnr.gfcoef_cl0[i];
    }
    for(i = 0; i < TNR_GFCOEF3_SIZE; i++) {
        pTnrCfg->gfcoef_cl1[i] = tnr.gfcoef_cl1[i];
    }

    //0x0130 - 0x0134
    for(i = 0; i < TNR_SCALE_YG_SIZE; i++) {
        pTnrCfg->scale_yg[i] = tnr.scale_yg[i];
    }

    //0x0138 - 0x013c
    for(i = 0; i < TNR_SCALE_YL_SIZE; i++) {
        pTnrCfg->scale_yl[i] = tnr.scale_yl[i];
    }

    //0x0140 - 0x0148
    for(i = 0; i < TNR_SCALE_CG_SIZE; i++) {
        pTnrCfg->scale_cg[i] = tnr.scale_cg[i];
        pTnrCfg->scale_y2cg[i] = tnr.scale_y2cg[i];
    }

    //0x014c - 0x0154
    for(i = 0; i < TNR_SCALE_CL_SIZE; i++) {
        pTnrCfg->scale_cl[i] = tnr.scale_cl[i];
    }
    for(i = 0; i < TNR_SCALE_Y2CL_SIZE; i++) {
        pTnrCfg->scale_y2cl[i] = tnr.scale_y2cl[i];
    }

    //0x0158
    for(i = 0; i < TNR_WEIGHT_Y_SIZE; i++) {
        pTnrCfg->weight_y[i] = tnr.weight_y[i];
    }

    LOGD("%s:(%d) exit \n", __FUNCTION__, __LINE__);
}


void
Isp20Params::convertAiqUvnrToIsp20Params(struct rkispp_params_cfg& pp_cfg,
        rk_aiq_isp_uvnr_t& uvnr)
{
    LOGD("%s:(%d) enter \n", __FUNCTION__, __LINE__);

    int i = 0;
    struct rkispp_nr_config  * pNrCfg = &pp_cfg.nr_cfg;

	if(uvnr.uvnr_en){
    	pp_cfg.module_ens |= ISPP_MODULE_NR;
	}
    pp_cfg.module_en_update |= ISPP_MODULE_NR;
    pp_cfg.module_cfg_update |= ISPP_MODULE_NR;

    //0x0080
    pNrCfg->uvnr_step1_en = uvnr.uvnr_step1_en;
    pNrCfg->uvnr_step2_en = uvnr.uvnr_step2_en;
    pNrCfg->nr_gain_en = uvnr.nr_gain_en;
    pNrCfg->uvnr_nobig_en = uvnr.uvnr_nobig_en;
    pNrCfg->uvnr_big_en = uvnr.uvnr_big_en;


    //0x0084
    pNrCfg->uvnr_gain_1sigma = uvnr.uvnr_gain_1sigma;

    //0x0088
    pNrCfg->uvnr_gain_offset = uvnr.uvnr_gain_offset;

    //0x008c
    pNrCfg->uvnr_gain_uvgain[0] = uvnr.uvnr_gain_uvgain[0];
    pNrCfg->uvnr_gain_uvgain[1] = uvnr.uvnr_gain_uvgain[1];
    pNrCfg->uvnr_gain_t2gen = uvnr.uvnr_gain_t2gen;
    // no need set
    //pNrCfg->uvnr_gain_iso = uvnr.bfRatio1;

    //0x0090
    pNrCfg->uvnr_t1gen_m3alpha = uvnr.uvnr_t1gen_m3alpha;

    //0x0094
    pNrCfg->uvnr_t1flt_mode = uvnr.uvnr_t1flt_mode;

    //0x0098
    pNrCfg->uvnr_t1flt_msigma = uvnr.uvnr_t1flt_msigma;

    //0x009c
    pNrCfg->uvnr_t1flt_wtp = uvnr.uvnr_t1flt_wtp;

    //0x00a0-0x00a4
    for(i = 0; i < NR_UVNR_T1FLT_WTQ_SIZE; i++) {
        pNrCfg->uvnr_t1flt_wtq[i] = uvnr.uvnr_t1flt_wtq[i];
    }

    //0x00a8
    pNrCfg->uvnr_t2gen_m3alpha = uvnr.uvnr_t2gen_m3alpha;

    //0x00ac
    pNrCfg->uvnr_t2gen_msigma = uvnr.uvnr_t2gen_msigma;

    //0x00b0
    pNrCfg->uvnr_t2gen_wtp = uvnr.uvnr_t2gen_wtp;

    //0x00b4
    for(i = 0; i < NR_UVNR_T2GEN_WTQ_SIZE; i++) {
        pNrCfg->uvnr_t2gen_wtq[i] = uvnr.uvnr_t2gen_wtq[i];
    }

    //0x00b8
    pNrCfg->uvnr_t2flt_msigma = uvnr.uvnr_t2flt_msigma;

    //0x00bc
    pNrCfg->uvnr_t2flt_wtp = uvnr.uvnr_t2flt_wtp;
    for(i = 0; i < NR_UVNR_T2FLT_WT_SIZE; i++) {
        pNrCfg->uvnr_t2flt_wt[i] = uvnr.uvnr_t2flt_wt[i];
    }


    LOGD("%s:(%d) exit \n", __FUNCTION__, __LINE__);
}


void
Isp20Params::convertAiqYnrToIsp20Params(struct rkispp_params_cfg& pp_cfg,
                                        rk_aiq_isp_ynr_t& ynr)
{
    LOGD("%s:(%d) enter \n", __FUNCTION__, __LINE__);

    int i = 0;
    struct rkispp_nr_config  * pNrCfg = &pp_cfg.nr_cfg;

	if(ynr.ynr_en){
    	pp_cfg.module_ens |= ISPP_MODULE_NR;
	}
    pp_cfg.module_en_update |= ISPP_MODULE_NR;
    pp_cfg.module_cfg_update |= ISPP_MODULE_NR;

    //0x0104 - 0x0108
    for(i = 0; i < NR_YNR_SGM_DX_SIZE; i++) {
        pNrCfg->ynr_sgm_dx[i] = ynr.ynr_sgm_dx[i];
    }

    //0x010c - 0x012c
    for(i = 0; i < NR_YNR_SGM_Y_SIZE; i++) {
        pNrCfg->ynr_lsgm_y[i] = ynr.ynr_lsgm_y[i];
    }


    //0x0130
    for(i = 0; i < NR_YNR_CI_SIZE; i++) {
        pNrCfg->ynr_lci[i] = ynr.ynr_lci[i];
    }

    //0x0134
    for(i = 0; i < NR_YNR_LGAIN_MIN_SIZE; i++) {
        pNrCfg->ynr_lgain_min[i] = ynr.ynr_lgain_min[i];
    }

    //0x0138
    pNrCfg->ynr_lgain_max = ynr.ynr_lgain_max;


    //0x013c
    pNrCfg->ynr_lmerge_bound = ynr.ynr_lmerge_bound;
    pNrCfg->ynr_lmerge_ratio = ynr.ynr_lmerge_ratio;

    //0x0140
    for(i = 0; i < NR_YNR_LWEIT_FLT_SIZE; i++) {
        pNrCfg->ynr_lweit_flt[i] = ynr.ynr_lweit_flt[i];
    }

    //0x0144 - 0x0164
    for(i = 0; i < NR_YNR_SGM_Y_SIZE; i++) {
        pNrCfg->ynr_hsgm_y[i] = ynr.ynr_hsgm_y[i];
    }

    //0x0168
    for(i = 0; i < NR_YNR_CI_SIZE; i++) {
        pNrCfg->ynr_hlci[i] = ynr.ynr_hlci[i];
    }

    //0x016c
    for(i = 0; i < NR_YNR_CI_SIZE; i++) {
        pNrCfg->ynr_lhci[i] = ynr.ynr_lhci[i];
    }

    //0x0170
    for(i = 0; i < NR_YNR_CI_SIZE; i++) {
        pNrCfg->ynr_hhci[i] = ynr.ynr_hhci[i];
    }

    //0x0174
    for(i = 0; i < NR_YNR_HGAIN_SGM_SIZE; i++) {
        pNrCfg->ynr_hgain_sgm[i] = ynr.ynr_hgain_sgm[i];
    }

    //0x0178 - 0x0188
    for(i = 0; i < NR_YNR_HWEIT_D_SIZE; i++) {
        pNrCfg->ynr_hweit_d[i] = ynr.ynr_hweit_d[i];
    }


    //0x018c - 0x01a0
    for(i = 0; i < NR_YNR_HGRAD_Y_SIZE; i++) {
        pNrCfg->ynr_hgrad_y[i] = ynr.ynr_hgrad_y[i];
    }

    //0x01a4 -0x01a8
    for(i = 0; i < NR_YNR_HWEIT_SIZE; i++) {
        pNrCfg->ynr_hweit[i] = ynr.ynr_hweit[i];
    }

    //0x01b0
    pNrCfg->ynr_hmax_adjust = ynr.ynr_hmax_adjust;

    //0x01b4
    pNrCfg->ynr_hstrength = ynr.ynr_hstrength;

    //0x01b8
    pNrCfg->ynr_lweit_cmp[0] = ynr.ynr_lweit_cmp[0];
    pNrCfg->ynr_lweit_cmp[1] = ynr.ynr_lweit_cmp[1];


    //0x01bc
    pNrCfg->ynr_lmaxgain_lv4 = ynr.ynr_lmaxgain_lv4;

    //0x01c0 - 0x01e0
    for(i = 0; i < NR_YNR_HSTV_Y_SIZE; i++) {
        pNrCfg->ynr_hstv_y[i] = ynr.ynr_hstv_y[i];
    }

    //0x01e4  - 0x01e8
    for(i = 0; i < NR_YNR_ST_SCALE_SIZE; i++) {
        pNrCfg->ynr_st_scale[i] = ynr.ynr_st_scale[i];
    }

    LOGD("%s:(%d) exit \n", __FUNCTION__, __LINE__);

}


void
Isp20Params::convertAiqSharpenToIsp20Params(struct rkispp_params_cfg& pp_cfg,
        rk_aiq_isp_sharpen_t& sharp, rk_aiq_isp_edgeflt_t& edgeflt)
{
    int i = 0;
    struct rkispp_sharp_config  * pSharpCfg = &pp_cfg.shp_cfg;
    RKAsharp_Sharp_HW_Fix_t *pSharpV1 = &sharp.stSharpFixV1;

	if(pSharpV1->sharp_en || edgeflt.edgeflt_en){
    	pp_cfg.module_ens |= ISPP_MODULE_SHP;
	}
    pp_cfg.module_en_update |= ISPP_MODULE_SHP;
    pp_cfg.module_cfg_update |= ISPP_MODULE_SHP;

    //0x0080
    pSharpCfg->alpha_adp_en = edgeflt.alpha_adp_en;
    pSharpCfg->yin_flt_en = pSharpV1->yin_flt_en;
    pSharpCfg->edge_avg_en = pSharpV1->edge_avg_en;

    //0x0084
    pSharpCfg->hbf_ratio = pSharpV1->hbf_ratio;
    pSharpCfg->ehf_th = pSharpV1->ehf_th;
    pSharpCfg->pbf_ratio = pSharpV1->pbf_ratio;

    //0x0088
    pSharpCfg->edge_thed = edgeflt.edge_thed;
    pSharpCfg->dir_min = edgeflt.dir_min;
    pSharpCfg->smoth_th4 = edgeflt.smoth_th4;

    //0x008c
    pSharpCfg->l_alpha = edgeflt.l_alpha;
    pSharpCfg->g_alpha = edgeflt.g_alpha;


    //0x0090
    for(i = 0; i < SHP_PBF_KERNEL_SIZE; i++) {
        pSharpCfg->pbf_k[i] = pSharpV1->pbf_k[i];
    }

    //0x0094 - 0x0098
    for(i = 0; i < SHP_MRF_KERNEL_SIZE; i++) {
        pSharpCfg->mrf_k[i] = pSharpV1->mrf_k[i];
    }

    //0x009c -0x00a4
    for(i = 0; i < SHP_MBF_KERNEL_SIZE; i++) {
        pSharpCfg->mbf_k[i] = pSharpV1->mbf_k[i];
    }

    //0x00a8 -0x00ac
    for(i = 0; i < SHP_HRF_KERNEL_SIZE; i++) {
        pSharpCfg->hrf_k[i] = pSharpV1->hrf_k[i];
    }

    //0x00b0
    for(i = 0; i < SHP_HBF_KERNEL_SIZE; i++) {
        pSharpCfg->hbf_k[i] = pSharpV1->hbf_k[i];
    }

    //0x00b4
    for(i = 0; i < SHP_EDGE_COEF_SIZE; i++) {
        pSharpCfg->eg_coef[i] = edgeflt.eg_coef[i];
    }

    //0x00b8
    for(i = 0; i < SHP_EDGE_SMOTH_SIZE; i++) {
        pSharpCfg->eg_smoth[i] = edgeflt.eg_smoth[i];
    }

    //0x00bc - 0x00c0
    for(i = 0; i < SHP_EDGE_GAUS_SIZE; i++) {
        pSharpCfg->eg_gaus[i] = edgeflt.eg_gaus[i];
    }

    //0x00c4 - 0x00c8
    for(i = 0; i < SHP_DOG_KERNEL_SIZE; i++) {
        pSharpCfg->dog_k[i] = edgeflt.dog_k[i];
    }

    //0x00cc - 0x00d0
    for(i = 0; i < 6; i++) {
        pSharpCfg->lum_point[i] = pSharpV1->lum_point[i];
    }

    //0x00d4
    pSharpCfg->pbf_shf_bits = pSharpV1->pbf_shf_bits;
    pSharpCfg->mbf_shf_bits = pSharpV1->mbf_shf_bits;
    pSharpCfg->hbf_shf_bits = pSharpV1->hbf_shf_bits;


    //0x00d8 - 0x00dc
    for(i = 0; i < 8; i++) {
        pSharpCfg->pbf_sigma[i] = pSharpV1->pbf_sigma[i];
    }

    //0x00e0 - 0x00e4
    for(i = 0; i < 8; i++) {
        pSharpCfg->lum_clp_m[i] = pSharpV1->lum_clp_m[i];
    }

    //0x00e8 - 0x00ec
    for(i = 0; i < 8; i++) {
        pSharpCfg->lum_min_m[i] = pSharpV1->lum_min_m[i];
    }

    //0x00f0 - 0x00f4
    for(i = 0; i < 8; i++) {
        pSharpCfg->mbf_sigma[i] = pSharpV1->mbf_sigma[i];
    }

    //0x00f8 - 0x00fc
    for(i = 0; i < 8; i++) {
        pSharpCfg->lum_clp_h[i] = pSharpV1->lum_clp_h[i];
    }

    //0x0100 - 0x0104
    for(i = 0; i < 8; i++) {
        pSharpCfg->hbf_sigma[i] = pSharpV1->hbf_sigma[i];
    }

    //0x0108 - 0x010c
    for(i = 0; i < 8; i++) {
        pSharpCfg->edge_lum_thed[i] = edgeflt.edge_lum_thed[i];
    }

    //0x0110 - 0x0114
    for(i = 0; i < 8; i++) {
        pSharpCfg->clamp_pos[i] = edgeflt.clamp_pos[i];
    }

    //0x0118 - 0x011c
    for(i = 0; i < 8; i++) {
        pSharpCfg->clamp_neg[i] = edgeflt.clamp_neg[i];
    }

    //0x0120 - 0x0124
    for(i = 0; i < 8; i++) {
        pSharpCfg->detail_alpha[i] = edgeflt.detail_alpha[i];
    }

    //0x0128
    pSharpCfg->rfl_ratio = pSharpV1->rfl_ratio;
    pSharpCfg->rfh_ratio = pSharpV1->rfh_ratio;
    //0x012C
    pSharpCfg->m_ratio = pSharpV1->m_ratio;
    pSharpCfg->h_ratio = pSharpV1->h_ratio;

}




XCamReturn
Isp20Params::convertAiqResultsToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
        SmartPtr<RkAiqIspParamsProxy> aiq_results,
        SmartPtr<RkAiqIspParamsProxy>& last_aiq_results)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    convertAiqHistToIsp20Params(isp_cfg, aiq_results->data()->hist_meas);
    convertAiqAeToIsp20Params(isp_cfg, aiq_results->data()->aec_meas);
    convertAiqAhdrToIsp20Params(isp_cfg, aiq_results->data()->ahdr_proc_res);
    convertAiqAwbGainToIsp20Params(isp_cfg, aiq_results->data()->awb_gain);
    convertAiqAwbToIsp20Params(isp_cfg, aiq_results->data()->awb_cfg_v200);
    convertAiqLscToIsp20Params(isp_cfg, aiq_results->data()->lsc);
    convertAiqCcmToIsp20Params(isp_cfg, aiq_results->data()->ccm);
    convertAiqAgammaToIsp20Params(isp_cfg, aiq_results->data()->agamma_config);
    convertAiqBlcToIsp20Params(isp_cfg, aiq_results);
    convertAiqDpccToIsp20Params(isp_cfg, aiq_results);
    convertAiqRawnrToIsp20Params(isp_cfg, aiq_results->data()->rawnr);

    /*
     * enable the modules that has been verified to work properly on the board
     * TODO: enable all modules after validation in isp
     */
#if 0
    convertAiqAfToIsp20Params(isp_cfg, aiq_results->data()->af_meas);
    convertAiqAdehazeToIsp20Params(isp_cfg, aiq_results->data()->adhaz_config);
    convertAiqAdemosaicToIsp20Params(isp_cfg, aiq_results);
    convertAiqCpToIsp20Params(isp_cfg, aiq_results->data()->cp);
    convertAiqIeToIsp20Params(isp_cfg, aiq_results->data()->ie);
#endif

    return ret;
}

XCamReturn
Isp20Params::convertAiqResultsToIsp20PpParams(struct rkispp_params_cfg& pp_cfg,
        SmartPtr<RkAiqIsppParamsProxy> aiq_results)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    convertAiqTnrToIsp20Params(pp_cfg, aiq_results->data()->tnr);
    convertAiqUvnrToIsp20Params(pp_cfg, aiq_results->data()->uvnr);
    convertAiqYnrToIsp20Params(pp_cfg, aiq_results->data()->ynr);
    convertAiqSharpenToIsp20Params(pp_cfg, aiq_results->data()->sharpen,
                                   aiq_results->data()->edgeflt);

    return ret;
}

XCamReturn
Isp20Params::checkIsp20Params(struct isp2x_isp_params_cfg& isp_cfg)
{
    //TODO
    return XCAM_RETURN_NO_ERROR;
}

void
Isp20Params::convertAiqAdemosaicToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
        SmartPtr<RkAiqIspParamsProxy> aiq_results)
{
    isp_cfg.others.debayer_cfg.clip_en = aiq_results->data()->demosaic.clip_en;
    isp_cfg.others.debayer_cfg.filter_c_en = aiq_results->data()->demosaic.filter_c_en;
    isp_cfg.others.debayer_cfg.filter_g_en = aiq_results->data()->demosaic.filter_g_en;
    isp_cfg.others.debayer_cfg.gain_offset = aiq_results->data()->demosaic.gain_offset;
    isp_cfg.others.debayer_cfg.offset = aiq_results->data()->demosaic.offset;
    isp_cfg.others.debayer_cfg.hf_offset = aiq_results->data()->demosaic.hf_offset;
    isp_cfg.others.debayer_cfg.thed0 = aiq_results->data()->demosaic.thed0;
    isp_cfg.others.debayer_cfg.thed1 = aiq_results->data()->demosaic.thed1;
    isp_cfg.others.debayer_cfg.dist_scale = aiq_results->data()->demosaic.dist_scale;
    isp_cfg.others.debayer_cfg.shift_num = aiq_results->data()->demosaic.shift_num;
    isp_cfg.others.debayer_cfg.filter1_coe1 = aiq_results->data()->demosaic.filter1_coe[0];
    isp_cfg.others.debayer_cfg.filter1_coe2 = aiq_results->data()->demosaic.filter1_coe[1];
    isp_cfg.others.debayer_cfg.filter1_coe3 = aiq_results->data()->demosaic.filter1_coe[2];
    isp_cfg.others.debayer_cfg.filter1_coe4 = aiq_results->data()->demosaic.filter1_coe[3];
    isp_cfg.others.debayer_cfg.filter1_coe5 = aiq_results->data()->demosaic.filter1_coe[4];
    isp_cfg.others.debayer_cfg.filter2_coe1 = aiq_results->data()->demosaic.filter2_coe[0];
    isp_cfg.others.debayer_cfg.filter2_coe2 = aiq_results->data()->demosaic.filter2_coe[1];
    isp_cfg.others.debayer_cfg.filter2_coe3 = aiq_results->data()->demosaic.filter2_coe[2];
    isp_cfg.others.debayer_cfg.filter2_coe4 = aiq_results->data()->demosaic.filter2_coe[3];
    isp_cfg.others.debayer_cfg.filter2_coe5 = aiq_results->data()->demosaic.filter2_coe[4];
    isp_cfg.others.debayer_cfg.max_ratio = aiq_results->data()->demosaic.max_ratio;
    isp_cfg.others.debayer_cfg.order_max = aiq_results->data()->demosaic.order_max;
    isp_cfg.others.debayer_cfg.order_min = aiq_results->data()->demosaic.order_min;
}

void
Isp20Params::convertAiqCpToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
                                       const rk_aiq_acp_params_t& cp_cfg)
{
    struct isp2x_cproc_cfg* cproc_cfg = &isp_cfg.others.cproc_cfg;

    // TODO: set range
    /* cproc_cfg->y_in_range = ; */
    /* cproc_cfg->y_out_range =; */
    /* cproc_cfg->c_out_range = ; */

    // do float to fix
    cproc_cfg->contrast = (uint8_t)(cp_cfg.contrast * (1 << 7) + 0.5);
    cproc_cfg->sat = (uint8_t)(cp_cfg.saturation * (1 << 7) + 0.5);
    cproc_cfg->brightness = cp_cfg.brightness > 0.0f ? \
                            (uint8_t)(cp_cfg.brightness + 0.5) :
                            (uint8_t)(-cp_cfg.brightness + 0.5);
    cproc_cfg->hue = cp_cfg.hue < 0.0f ? \
                     (uint8_t)(cp_cfg.hue * -128.0f / 90.0f) :
                     (uint8_t)(cp_cfg.hue * 128.0f / 90.0f);
}

void
Isp20Params::convertAiqIeToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
                                       const rk_aiq_isp_ie_t& ie_cfg)
{
    struct isp2x_ie_cfg* ie_config = &isp_cfg.others.ie_cfg;

    switch (ie_cfg.base.mode) {
    case RK_AIQ_IE_EFFECT_BW:
        ie_config->effect = V4L2_COLORFX_BW;
        break;
    case RK_AIQ_IE_EFFECT_NEGATIVE:
        ie_config->effect = V4L2_COLORFX_NEGATIVE;
        break;
    case RK_AIQ_IE_EFFECT_SEPIA:
        ie_config->effect = V4L2_COLORFX_SEPIA;
        break;
    case RK_AIQ_IE_EFFECT_EMBOSS:
    {
        ie_config->effect = V4L2_COLORFX_EMBOSS;
        ie_config->eff_mat_1 = (uint16_t)(ie_cfg.extra.mode_coeffs[0])
                               | ((uint16_t)(ie_cfg.extra.mode_coeffs[1]) << 0x4)
                               | ((uint16_t)(ie_cfg.extra.mode_coeffs[2]) << 0x8)
                               | ((uint16_t)(ie_cfg.extra.mode_coeffs[3]) << 0xc);
        ie_config->eff_mat_2 = (uint16_t)(ie_cfg.extra.mode_coeffs[4])
                               | ((uint16_t)(ie_cfg.extra.mode_coeffs[5]) << 0x4)
                               | ((uint16_t)(ie_cfg.extra.mode_coeffs[6]) << 0x8)
                               | ((uint16_t)(ie_cfg.extra.mode_coeffs[7]) << 0xc);
        ie_config->eff_mat_3 = (ie_cfg.extra.mode_coeffs[8]);
        /*not used for this effect*/
        ie_config->eff_mat_4 = 0;
        ie_config->eff_mat_5 = 0;
        ie_config->color_sel = 0;
        ie_config->eff_tint = 0;
    }
    break;
    case RK_AIQ_IE_EFFECT_SKETCH:
    {
        ie_config->effect = V4L2_COLORFX_SKETCH;
        ie_config->eff_mat_3 = ((uint16_t)(ie_cfg.extra.mode_coeffs[0]) << 0x4)
                               | ((uint16_t)(ie_cfg.extra.mode_coeffs[1]) << 0x8)
                               | ((uint16_t)(ie_cfg.extra.mode_coeffs[2]) << 0xc);
        /*not used for this effect*/
        ie_config->eff_mat_4 = (uint16_t)(ie_cfg.extra.mode_coeffs[3])
                               | ((uint16_t)(ie_cfg.extra.mode_coeffs[4]) << 0x4)
                               | ((uint16_t)(ie_cfg.extra.mode_coeffs[5]) << 0x8)
                               | ((uint16_t)(ie_cfg.extra.mode_coeffs[6]) << 0xc);
        ie_config->eff_mat_5 = (uint16_t)(ie_cfg.extra.mode_coeffs[7])
                               | ((uint16_t)(ie_cfg.extra.mode_coeffs[8]) << 0x4);

        /*not used for this effect*/
        ie_config->eff_mat_1 = 0;
        ie_config->eff_mat_2 = 0;
        ie_config->color_sel = 0;
        ie_config->eff_tint = 0;
    }
    break;
    case RK_AIQ_IE_EFFECT_SHARPEN:
    {
        /* TODO: can't find related mode in v4l2_colorfx*/
        //ie_config->effect =
        //  V4L2_COLORFX_EMBOSS;
        ie_config->eff_mat_1 = (uint16_t)(ie_cfg.extra.mode_coeffs[0])
                               | ((uint16_t)(ie_cfg.extra.mode_coeffs[1]) << 0x4)
                               | ((uint16_t)(ie_cfg.extra.mode_coeffs[2]) << 0x8)
                               | ((uint16_t)(ie_cfg.extra.mode_coeffs[3]) << 0xc);
        ie_config->eff_mat_2 = (uint16_t)(ie_cfg.extra.mode_coeffs[4])
                               | ((uint16_t)(ie_cfg.extra.mode_coeffs[5]) << 0x4)
                               | ((uint16_t)(ie_cfg.extra.mode_coeffs[6]) << 0x8)
                               | ((uint16_t)(ie_cfg.extra.mode_coeffs[7]) << 0xc);
        ie_config->eff_mat_3 = (ie_cfg.extra.mode_coeffs[8]);
        /*not used for this effect*/
        ie_config->eff_mat_4 = 0;
        ie_config->eff_mat_5 = 0;
        ie_config->color_sel = 0;
        ie_config->eff_tint = 0;
    }
    break;
    default:
        break;
    }
}

}; //namspace RkCam
