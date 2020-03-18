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
    memcpy(&isp_cfg.meas.rawaebig1, &aec_meas.rawaebig1, sizeof(aec_meas.rawaebig1));
    memcpy(&isp_cfg.meas.rawaebig2, &aec_meas.rawaebig2, sizeof(aec_meas.rawaebig2));
    memcpy(&isp_cfg.meas.rawaebig3, &aec_meas.rawaebig3, sizeof(aec_meas.rawaebig3));
    memcpy(&isp_cfg.meas.rawaelite, &aec_meas.rawaelite, sizeof(aec_meas.rawaelite));
    memcpy(&isp_cfg.meas.yuvae, &aec_meas.yuvae, sizeof(aec_meas.yuvae));

    /*
     *     printf("xuhf-debug: hist_meas-isp_cfg size: [%dx%d]-[%dx%d]-[%dx%d]-[%dx%d]\n",
     *                     sizeof(aec_meas.rawaebig1),
     *                     sizeof(isp_cfg.meas.rawaebig1),
     *                     sizeof(aec_meas.rawaebig2),
     *                     sizeof(isp_cfg.meas.rawaebig2),
     *                     sizeof(aec_meas.rawaebig3),
     *                     sizeof(isp_cfg.meas.rawaebig3),
     *                     sizeof(aec_meas.rawaelite),
     *                     sizeof(isp_cfg.meas.rawaelite));
     *
     *     printf("xuhf-debug: aec_meas: win size: [%dx%d]-[%dx%d]-[%dx%d]-[%dx%d]\n",
     *            aec_meas.rawaelite.win.h_size,
     *            aec_meas.rawaelite.win.v_size,
     *            aec_meas.rawaebig1.win.h_size,
     *            aec_meas.rawaebig1.win.v_size,
     *            aec_meas.rawaebig2.win.h_size,
     *            aec_meas.rawaebig2.win.v_size,
     *            aec_meas.rawaebig3.win.h_size,
     *            aec_meas.rawaebig3.win.v_size);
     *
     *     printf("xuhf-debug: isp_cfg: win size: [%dx%d]-[%dx%d]-[%dx%d]-[%dx%d]\n",
     *            isp_cfg.meas.rawaelite.win.h_size,
     *            isp_cfg.meas.rawaelite.win.v_size,
     *            isp_cfg.meas.rawaebig1.win.h_size,
     *            isp_cfg.meas.rawaebig1.win.v_size,
     *            isp_cfg.meas.rawaebig2.win.h_size,
     *            isp_cfg.meas.rawaebig2.win.v_size,
     *            isp_cfg.meas.rawaebig3.win.h_size,
     *            isp_cfg.meas.rawaebig3.win.v_size);
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
    memcpy(&isp_cfg.meas.rawhstbig1, &hist_meas.rawhstbig1, sizeof(hist_meas.rawhstbig1));
    // convertAiqBigHstWndSize(isp_cfg.meas.rawhstbig1);
    memcpy(&isp_cfg.meas.rawhstbig2, &hist_meas.rawhstbig2, sizeof(hist_meas.rawhstbig2));
    // convertAiqBigHstWndSize(isp_cfg.meas.rawhstbig2);
    memcpy(&isp_cfg.meas.rawhstbig3, &hist_meas.rawhstbig3, sizeof(hist_meas.rawhstbig3));
    // convertAiqBigHstWndSize(isp_cfg.meas.rawhstbig3);
    memcpy(&isp_cfg.meas.rawhstlite, &hist_meas.rawhstlite, sizeof(hist_meas.rawhstlite));
    // convertAiqLiteHstWndSize(isp_cfg.meas.rawhstlite);
    memcpy(&isp_cfg.meas.sihst, &hist_meas.sihist, sizeof(hist_meas.sihist));
    // convertAiqSiHstWndSize(isp_cfg.meas.sihst);

    /*
     *     printf("xuhf-debug: hist_meas-isp_cfg size: [%dx%d]-[%dx%d]-[%dx%d]-[%dx%d]\n",
     *                     sizeof(hist_meas.rawhstbig1),
     *                     sizeof(isp_cfg.meas.rawhstbig1),
     *                     sizeof(hist_meas.rawhstbig2),
     *                     sizeof(isp_cfg.meas.rawhstbig2),
     *                     sizeof(hist_meas.rawhstbig3),
     *                     sizeof(isp_cfg.meas.rawhstbig3),
     *                     sizeof(hist_meas.rawhstlite),
     *                     sizeof(isp_cfg.meas.rawhstlite));
     *
     *     printf("xuhf-debug: hist_meas: hist win size: [%dx%d]-[%dx%d]-[%dx%d]-[%dx%d]\n",
     *                     hist_meas.rawhstlite.win.h_size,
     *                     hist_meas.rawhstlite.win.v_size,
     *                     hist_meas.rawhstbig1.win.h_size,
     *                     hist_meas.rawhstbig1.win.v_size,
     *                     hist_meas.rawhstbig2.win.h_size,
     *                     hist_meas.rawhstbig2.win.v_size,
     *                     hist_meas.rawhstbig3.win.h_size,
     *                     hist_meas.rawhstbig3.win.v_size);
     *
     *     printf("xuhf-debug: isp_cfg: hist win size: [%dx%d]-[%dx%d]-[%dx%d]-[%dx%d]\n",
     *                     isp_cfg.meas.rawhstlite.win.h_size,
     *                     isp_cfg.meas.rawhstlite.win.v_size,
     *                     isp_cfg.meas.rawhstbig1.win.h_size,
     *                     isp_cfg.meas.rawhstbig1.win.v_size,
     *                     isp_cfg.meas.rawhstbig2.win.h_size,
     *                     isp_cfg.meas.rawhstbig2.win.v_size,
     *                     isp_cfg.meas.rawhstbig3.win.h_size,
     *                     isp_cfg.meas.rawhstbig3.win.v_size);
     */
}



























void
Isp20Params::convertAiqAwbToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
                                        const rk_aiq_awb_stat_cfg_v200_t& awb_meas)
{

    if(awb_meas.awbEnable){
        isp_cfg.module_ens |= ISP2X_MODULE_RAWAWB;
    }
    isp_cfg.module_en_update |= ISP2X_MODULE_RAWAWB;
    isp_cfg.module_cfg_update |= ISP2X_MODULE_RAWAWB;

    struct isp2x_rawawb_meas_cfg * awb_cfg_v200 = &isp_cfg.meas.rawawb;
    awb_cfg_v200->rawawb_sel                        =    awb_meas.frameChoose;
    awb_cfg_v200->sw_rawawb_xy_en                   =    awb_meas.xyDetectionEnable;
    awb_cfg_v200->sw_rawawb_uv_en                   =    awb_meas.uvDetectionEnable;
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
    awb_cfg_v200->sw_rawawb_y_range                 =    awb_meas.rgb2yuv_c_range;
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
    isp_cfg.others.hdrmge_cfg.mode         = ahdr_data.MergeData.sw_hdrmge_mode;
    isp_cfg.others.hdrmge_cfg.gain0_inv    = ahdr_data.MergeData.sw_hdrmge_gain0_inv;
    isp_cfg.others.hdrmge_cfg.gain0         = ahdr_data.MergeData.sw_hdrmge_gain0;
    isp_cfg.others.hdrmge_cfg.gain1_inv    = ahdr_data.MergeData.sw_hdrmge_gain1_inv;
    isp_cfg.others.hdrmge_cfg.gain1        = ahdr_data.MergeData.sw_hdrmge_gain1;
    isp_cfg.others.hdrmge_cfg.gain2        = ahdr_data.MergeData.sw_hdrmge_gain2;
    isp_cfg.others.hdrmge_cfg.lm_dif_0p15  = ahdr_data.MergeData.sw_hdrmge_lm_dif_0p15;
    isp_cfg.others.hdrmge_cfg.lm_dif_0p9   = ahdr_data.MergeData.sw_hdrmge_lm_dif_0p9;
    isp_cfg.others.hdrmge_cfg.ms_diff_0p15 = ahdr_data.MergeData.sw_hdrmge_ms_dif_0p15;
    isp_cfg.others.hdrmge_cfg.ms_dif_0p8   = ahdr_data.MergeData.sw_hdrmge_ms_dif_0p8;
    for(int i = 0; i < 17; i++)
    {
        isp_cfg.others.hdrmge_cfg.curve.curve_0[i] = ahdr_data.MergeData.sw_hdrmge_l0_y[i];
        isp_cfg.others.hdrmge_cfg.curve.curve_1[i] = ahdr_data.MergeData.sw_hdrmge_l1_y[i];
        isp_cfg.others.hdrmge_cfg.e_y[i]           = ahdr_data.MergeData.sw_hdrmge_e_y[i];
    }

    //tmo register
    isp_cfg.others.hdrtmo_cfg.cnt_vsize     = ahdr_data.TmoData.sw_hdrtmo_cnt_vsize;
    isp_cfg.others.hdrtmo_cfg.gain_ld_off2  = ahdr_data.TmoData.sw_hdrtmo_gain_ld_off2;
    isp_cfg.others.hdrtmo_cfg.gain_ld_off1  = ahdr_data.TmoData.sw_hdrtmo_gain_ld_off1;
    isp_cfg.others.hdrtmo_cfg.big_en        = ahdr_data.TmoData.sw_hdrtmo_big_en;
    isp_cfg.others.hdrtmo_cfg.nobig_en      = ahdr_data.TmoData.sw_hdrtmo_nobig_en;
    isp_cfg.others.hdrtmo_cfg.newhst_en     = ahdr_data.TmoData.sw_hdrtmo_newhist_en;
    isp_cfg.others.hdrtmo_cfg.cnt_mode      = ahdr_data.TmoData.sw_hdrtmo_cnt_mode;
    isp_cfg.others.hdrtmo_cfg.expl_lgratio  = ahdr_data.TmoData.sw_hdrtmo_expl_lgratio;
    isp_cfg.others.hdrtmo_cfg.lgscl_ratio   = ahdr_data.TmoData.sw_hdrtmo_lgscl_ratio;
    isp_cfg.others.hdrtmo_cfg.cfg_alpha     = ahdr_data.TmoData.sw_hdrtmo_cfg_alpha;
    isp_cfg.others.hdrtmo_cfg.set_gainoff   = ahdr_data.TmoData.sw_hdrtmo_set_gainoff;
    isp_cfg.others.hdrtmo_cfg.set_palpha    = ahdr_data.TmoData.sw_hdrtmo_set_palpha;
    isp_cfg.others.hdrtmo_cfg.set_lgmax     = ahdr_data.TmoData.sw_hdrtmo_set_lgmax;
    isp_cfg.others.hdrtmo_cfg.set_lgmin     = ahdr_data.TmoData.sw_hdrtmo_set_lgmin;
    isp_cfg.others.hdrtmo_cfg.set_weightkey = ahdr_data.TmoData.sw_hdrtmo_set_weightkey;
    isp_cfg.others.hdrtmo_cfg.set_lgmean    = ahdr_data.TmoData.sw_hdrtmo_set_lgmean;
    isp_cfg.others.hdrtmo_cfg.set_lgrange1  = ahdr_data.TmoData.sw_hdrtmo_set_lgrange1;
    isp_cfg.others.hdrtmo_cfg.set_lgrange0  = ahdr_data.TmoData.sw_hdrtmo_set_lgrange0;
    isp_cfg.others.hdrtmo_cfg.set_lgavgmax  = ahdr_data.TmoData.sw_hdrtmo_set_lgavgmax;
    isp_cfg.others.hdrtmo_cfg.clipgap1_i    = ahdr_data.TmoData.sw_hdrtmo_clipgap1;
    isp_cfg.others.hdrtmo_cfg.clipgap0_i    = ahdr_data.TmoData.sw_hdrtmo_clipgap0;
    isp_cfg.others.hdrtmo_cfg.clipratio1    = ahdr_data.TmoData.sw_hdrtmo_clipratio1;
    isp_cfg.others.hdrtmo_cfg.clipratio0    = ahdr_data.TmoData.sw_hdrtmo_clipratio0;
    isp_cfg.others.hdrtmo_cfg.ratiol        = ahdr_data.TmoData.sw_hdrtmo_ratiol;
    isp_cfg.others.hdrtmo_cfg.lgscl_inv     = ahdr_data.TmoData.sw_hdrtmo_lgscl_inv;
    isp_cfg.others.hdrtmo_cfg.lgscl         = ahdr_data.TmoData.sw_hdrtmo_lgscl;
    isp_cfg.others.hdrtmo_cfg.lgmax         = ahdr_data.TmoData.sw_hdrtmo_lgmax;
    isp_cfg.others.hdrtmo_cfg.hist_low      = ahdr_data.TmoData.sw_hdrtmo_hist_low;
    isp_cfg.others.hdrtmo_cfg.hist_min      = ahdr_data.TmoData.sw_hdrtmo_hist_min;
    isp_cfg.others.hdrtmo_cfg.hist_shift    = ahdr_data.TmoData.sw_hdrtmo_hist_shift;
    isp_cfg.others.hdrtmo_cfg.hist_0p3      = ahdr_data.TmoData.sw_hdrtmo_hist_0p3;
    isp_cfg.others.hdrtmo_cfg.hist_high     = ahdr_data.TmoData.sw_hdrtmo_hist_high;
    isp_cfg.others.hdrtmo_cfg.palpha_lwscl  = ahdr_data.TmoData.sw_hdrtmo_palpha_lwscl;
    isp_cfg.others.hdrtmo_cfg.palpha_lw0p5  = ahdr_data.TmoData.sw_hdrtmo_palpha_lw0p5;
    isp_cfg.others.hdrtmo_cfg.palpha_0p18   = ahdr_data.TmoData.sw_hdrtmo_palpha_0p18;
    isp_cfg.others.hdrtmo_cfg.maxgain       = ahdr_data.TmoData.sw_hdrtmo_maxgain;
    isp_cfg.others.hdrtmo_cfg.maxpalpha     = ahdr_data.TmoData.sw_hdrtmo_maxpalpha;
    LOGE("%d: gain_ld_off2 %d", __LINE__, isp_cfg.others.hdrtmo_cfg.gain_ld_off2);
    LOGE("%d: gain_ld_off1 %d", __LINE__, isp_cfg.others.hdrtmo_cfg.gain_ld_off1);
    LOGE("%d: hist_low %d", __LINE__, isp_cfg.others.hdrtmo_cfg.hist_low);

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
#endif
}

void
Isp20Params::convertAiqAfToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
        const rk_aiq_isp_af_meas_t& af_data)
{
    isp_cfg.meas.rawaf.rawaf_sel= af_data.gaus_flt_en;
    isp_cfg.meas.rawaf.gamma_en = af_data.gamma_flt_en;
    isp_cfg.meas.rawaf.gaus_en = af_data.gaus_flt_en;
    isp_cfg.meas.rawaf.afm_thres = af_data.afm_thres;
    isp_cfg.meas.rawaf.gaus_coe_h0= af_data.gaus_h0;
    isp_cfg.meas.rawaf.gaus_coe_h1= af_data.gaus_h1;
    isp_cfg.meas.rawaf.gaus_coe_h2= af_data.gaus_h2;
    memcpy(isp_cfg.meas.rawaf.lum_var_shift,
        af_data.lum_var_shift, ISP2X_RAWAF_WIN_NUM*sizeof(unsigned char));
    memcpy(isp_cfg.meas.rawaf.afm_var_shift,
        af_data.afm_var_shift, ISP2X_RAWAF_WIN_NUM*sizeof(unsigned char));
    memcpy(isp_cfg.meas.rawaf.line_en,
        af_data.line_en, ISP2X_RAWAF_LINE_NUM*sizeof(unsigned char));
    memcpy(isp_cfg.meas.rawaf.line_num,
        af_data.line_num, ISP2X_RAWAF_LINE_NUM*sizeof(unsigned char));
    memcpy(isp_cfg.meas.rawaf.gamma_y,
        af_data.gamma_y, ISP2X_RAWAF_GAMMA_NUM*sizeof(unsigned short));

    isp_cfg.meas.rawaf.num_afm_win = af_data.window_num;
    isp_cfg.meas.rawaf.win[0].h_offs = af_data.wina_h_offs;
    isp_cfg.meas.rawaf.win[0].v_offs = af_data.wina_v_offs;
    isp_cfg.meas.rawaf.win[0].h_size= af_data.wina_h_size;
    isp_cfg.meas.rawaf.win[0].v_size= af_data.wina_v_size;
    isp_cfg.meas.rawaf.win[1].h_offs = af_data.winb_h_offs;
    isp_cfg.meas.rawaf.win[1].v_offs = af_data.winb_v_offs;
    isp_cfg.meas.rawaf.win[1].h_size= af_data.winb_h_size;
    isp_cfg.meas.rawaf.win[1].v_size= af_data.winb_v_size;
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



                 //cfg->       = int(RKAiqAdhazConfig_t->dehaze_en[0]);  //0~1  , (1bit) dehaze_en
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
        const rk_aiq_isp_agamma_t& gamma_out_cfg)
{
    if(gamma_out_cfg.gamma_en){
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
        const RKAiqAdhazConfig_t& dhaze                     )
{
    int i;

    int rawWidth = 1920;
    int rawHeight = 1080;
    struct isp2x_dhaz_cfg *  cfg = &isp_cfg.others.dhaz_cfg;



    //cfg->       = int(RKAiqAdhazConfig_t->dehaze_en[0]);  //0~1  , (1bit) dehaze_en
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
    printf("%s:(%d) enter \n", __FUNCTION__, __LINE__);

	
	isp_cfg.module_ens |= ISP2X_MODULE_BLS;
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

    printf("%s:(%d) exit \n", __FUNCTION__, __LINE__);
}


void
Isp20Params::convertAiqDpccToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
        SmartPtr<RkAiqIspParamsProxy> aiq_results)
{
    printf("%s:(%d) enter \n", __FUNCTION__, __LINE__);

    struct isp2x_dpcc_cfg * pDpccCfg = &isp_cfg.others.dpcc_cfg;
    rk_aiq_isp_dpcc_t *pDpccRst = &aiq_results->data()->dpcc;

	if(pDpccRst->stBasic.enable){
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


    printf("%s:(%d) exit \n", __FUNCTION__, __LINE__);
}


void
Isp20Params::convertAiqLscToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
                                        const rk_aiq_lsc_cfg_t& lsc)
{

    if(lsc.lsc_en){
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
    if(ccm.ccmEnable){
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
    if(lut3d_cfg.enable){
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

u16 rawnr_get_trans(int tmpfix)
{
	int logtablef[65] = {0,1465,2909,4331,5731,7112,8472,9813,11136,12440,
		13726,14995,16248,17484,18704,19908,21097,22272,23432,24578,25710,
		26829,27935,29028,30109,31177,32234,33278,34312,35334,36345,37346,
		38336,39315,40285,41245,42195,43136,44068,44990,45904,46808,47704,
		48592,49472,50343,51207,52062,52910,53751,54584,55410,56228,57040,
		57844,58642,59433,60218,60996,61768,62534,63293,64047,64794,65536};
	int logprecision = 6;
    int logfixbit = 16;
    int logtblbit = 16;
    int logscalebit = 12;
	int logfixmul = (1<<logfixbit);
	long long x8, one = 1;
	long long gx, n, ix1, ix2, dp;
	long long lt1, lt2, dx, fx;
	int i, j = 1;

    x8 = tmpfix + (1<<8);
    // find highest bit
	for (i = 0; i<32; i++)
	{
		if (x8&j)
		{
			n = i;
		}
		j = j << 1;
	}

    gx = x8 - (one<<n);
    gx = gx * (one<<logprecision) * logfixmul;
    gx = gx/(one<<n);

    ix1 = gx >> logfixbit;
    dp = gx - ix1 * logfixmul;

    ix2 = ix1 + 1;

    lt1 = logtablef[ix1];
    lt2 = logtablef[ix2];

    dx = lt1*(logfixmul-dp) + lt2*dp;

    fx = dx + (n-8)*(one<<(logfixbit+logtblbit));
    fx = fx + (one<<(logfixbit+logtblbit-logscalebit-1));
    fx = fx >> (logfixbit+logtblbit-logscalebit);

	return fx;
}

void
Isp20Params::convertAiqRawnrToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
        rk_aiq_isp_rawnr_t& rawnr)
{

	printf("%s:(%d) enter \n", __FUNCTION__, __LINE__);

	struct isp2x_rawnr_cfg * pRawnrCfg = &isp_cfg.others.rawnr_cfg;

	isp_cfg.module_ens |= ISP2X_MODULE_RAWNR;
	isp_cfg.module_en_update |= ISP2X_MODULE_RAWNR;
	isp_cfg.module_cfg_update |= ISP2X_MODULE_RAWNR;

	int rawbit = 12;//rawBit;
	float tmp;

	//(0x0004)
	pRawnrCfg->gauss_en = rawnr.sw_rawnr_gauss_en;
	pRawnrCfg->log_bypass = rawnr.log_bypass;

	//(0x0008 - 0x0010)
	pRawnrCfg->filtpar0 = u16(rawnr.filtPar[0] * (1<<FIXNLMCALC));
	pRawnrCfg->filtpar1 = u16(rawnr.filtPar[1] * (1<<FIXNLMCALC));
	pRawnrCfg->filtpar2 = u16(rawnr.filtPar[2] * (1<<FIXNLMCALC));

	//(0x0014 - 0x0001c)
	pRawnrCfg->dgain0 = u32(rawnr.sw_dgain[0] * (1<<FIXNLMCALC));
	pRawnrCfg->dgain1 = u32(rawnr.sw_dgain[1] * (1<<FIXNLMCALC));
	pRawnrCfg->dgain2 = u32(rawnr.sw_dgain[2] * (1<<FIXNLMCALC));

	//(0x0020 - 0x0002c)
	for(int i=0; i<ISP2X_RAWNR_LUMA_RATION_NUM; i++){
		pRawnrCfg->luration[i] = u16(rawnr.luRatio[i] * (1<<FIXNLMCALC));
	}

	//(0x0030 - 0x0003c)
	for(int i=0; i<ISP2X_RAWNR_LUMA_RATION_NUM; i++){
		tmp = rawnr.luLevel[i] * ( 1<<(rawbit - 8) );
		pRawnrCfg->lulevel[i] = rawnr_get_trans((int)tmp);
	}
	tmp = (float)((1<<rawbit) - 1);
    pRawnrCfg->lulevel[ISP2X_RAWNR_LUMA_RATION_NUM - 1] = rawnr_get_trans((int)tmp);

	//(0x0040)
	pRawnrCfg->gauss = rawnr.gausskparsq;

	//(0x0044)
	pRawnrCfg->sigma = rawnr.sigmaPar;

	//(0x0048)
	pRawnrCfg->pix_diff = rawnr.pix_diff;

	//(0x004c)
	pRawnrCfg->thld_diff = rawnr.thld_diff;

	//(0x0050)
	pRawnrCfg->gas_weig_scl1 = rawnr.bayernr_gauss_weight0 * (1<<8);
	pRawnrCfg->gas_weig_scl2 = rawnr.bayernr_gauss_weight1 * (1<<8);
	pRawnrCfg->thld_chanelw = rawnr.thld_chanelw;

	//(0x0054)
	pRawnrCfg->lamda = rawnr.peaknoisesigma;

	//(0x0058 - 0x0005c)
	pRawnrCfg->fixw0 = u16(rawnr.w[0] * (1<<FIXNLMCALC));
	pRawnrCfg->fixw1 = u16(rawnr.w[1] * (1<<FIXNLMCALC));
	pRawnrCfg->fixw2 = u16(rawnr.w[2] * (1<<FIXNLMCALC));
	pRawnrCfg->fixw3 = u16(rawnr.w[3] * (1<<FIXNLMCALC));

	//(0x0060 - 0x00068)
	pRawnrCfg->wlamda0 = (pRawnrCfg->fixw0 * pRawnrCfg->lamda)>>FIXNLMCALC;
	pRawnrCfg->wlamda1 = (pRawnrCfg->fixw1 * pRawnrCfg->lamda)>>FIXNLMCALC;
	pRawnrCfg->wlamda2 = (pRawnrCfg->fixw3 * pRawnrCfg->lamda)>>FIXNLMCALC;

	//(0x006c)
	pRawnrCfg->rgain_filp = rawnr.rgain_filp;
	pRawnrCfg->bgain_filp = rawnr.bgain_filp;

	printf("%s:(%d) exit \n", __FUNCTION__, __LINE__);
}

template<typename T1, typename T2>
T1 FIX_FLOAT(T1 data0,  int bits, T2 &dst, int flag = 0)
{
    if(flag == 0)
        dst = ROUND_D(data0 * (1 << bits));
    else
        dst = FLOOR_INT64(data0 * (1 << bits));
    return  ((T1)dst / (1 << bits));

}

int get_matrix_idx(int i, int j, int rad)
{
    int src_i = 0;
    int i_act, j_act;
    if(i > rad)
        i_act = 2 * rad - i;
    else
        i_act = i;

    if(j > rad)
        j_act = 2 * rad - j;
    else
        j_act = j;
    if(j_act < i_act)
    {
        int tmp;
        tmp = j_act;
        j_act = i_act;
        i_act = tmp;
    }
    for(int ii = rad; ii >= 0; ii--)
        for(int jj = rad; jj >= ii; jj--)
        {
            if(i_act == ii && j_act == jj)
                return src_i;
            src_i++;
        }
}


void tnr_gfcoef_fix(int rad, double *gfcoef, u8* gfcoef_fix)
{
	double sum_d2;
    double *h;
	unsigned long tmp;

    h = (double*)malloc((rad * 2 + 1) * (rad * 2 + 1) * sizeof(double));
    sum_d2 = 0;
    for(int i = 0; i <  (rad * 2 + 1); i++){
        for(int j = 0; j < (rad * 2 + 1); j++){
            int src_i = get_matrix_idx(i, j, rad);
            h[i * (rad * 2 + 1) + j] = FIX_FLOAT((double)gfcoef[src_i], F_DECI_PIXEL_SIGMA_CONV_WEIGHT, tmp);
			gfcoef_fix[src_i] = tmp;
            sum_d2 += h[i * (rad * 2 + 1) + j];
        }
    }
    int idx = ((rad * 2 + 1) * rad + rad);
    h[idx]  = FIX_FLOAT(h[idx] + (1 - sum_d2), F_DECI_PIXEL_SIGMA_CONV_WEIGHT, tmp);
	gfcoef_fix[0] = tmp;
    free(h);

}


void
Isp20Params::convertAiqTnrToIsp20Params(struct rkispp_params_cfg& pp_cfg,
        rk_aiq_isp_tnr_t& tnr)
{
	printf("%s:(%d) enter \n", __FUNCTION__, __LINE__);

	pp_cfg.module_ens |= ISPP_MODULE_TNR;
	pp_cfg.module_en_update |= ISPP_MODULE_TNR;
	pp_cfg.module_cfg_update |= ISPP_MODULE_TNR;

	struct rkispp_tnr_config  * pTnrCfg = &pp_cfg.tnr_cfg;
	int i = 0;
	unsigned long tmp = 0;
	int mIso_last = 50;
	int mIso = 50;
	double gain_glb_filt;
    double gain_glb_ref1;
    double gain_glb_filt_sqrt;
    double gain_glb_filt_sqrt_inv;
    double scale_l_y				[MFNR_DIR_NUM][MFNR_MAX_LVL];
    double scale_l_y_uv				[MFNR_DIR_NUM][MFNR_MAX_LVL_UV];
    double scale_l_uv				[MFNR_DIR_NUM][MFNR_MAX_LVL_UV];
	double ratio_frq_sub_div		[2];
    int max_lvl         = MFNR_MAX_LVL;
    int max_lvl_uv      = MFNR_MAX_LVL_UV;
    int dir_num         = MFNR_DIR_NUM;


	uint32_t rad_isp20[2][5]                = {{2, 1, 1, 1, 1}, {2, 1, 1, 1, 1}};
    uint32_t rad_uv_isp20[2][5]             = {{2, 1, 1, 1, 1}, {2, 1, 1, 1, 1}};
    gain_glb_filt                           = (mIso_last / 50);
    gain_glb_ref1                           = (mIso / 50);//ref1
    gain_glb_filt_sqrt                      = sqrt(gain_glb_filt);
    gain_glb_filt_sqrt_inv                  = 1 / sqrt(gain_glb_filt);

	for(int dir_idx = 0; dir_idx < dir_num; dir_idx++){
		for(int lvl = 0; lvl < max_lvl; lvl++)
			scale_l_y[dir_idx][lvl] = tnr.ci[dir_idx][lvl] * tnr.scale[dir_idx][lvl] / tnr.dnstr[dir_idx];
	}
	for(int dir_idx = 0; dir_idx < dir_num; dir_idx++){
		for(int lvl = 0; lvl < max_lvl_uv; lvl++){
			scale_l_y_uv[dir_idx][lvl] = tnr.ci[dir_idx][lvl] * tnr.scale_uv[dir_idx][lvl] / tnr.dnstr_uv[dir_idx];
			scale_l_uv[dir_idx][lvl] = tnr.ci_uv[dir_idx][lvl] * tnr.scale_uv[dir_idx][lvl] / tnr.dnstr_uv[dir_idx];
		}
	}

	//0x0080
	pTnrCfg->opty_en = 1;
	pTnrCfg->optc_en = 1;
	pTnrCfg->gain_en = 1;

	//0x0088
	pTnrCfg->pk0_y = tnr.weight_limit_y[0];
	pTnrCfg->pk1_y = tnr.weight_limit_y[max_lvl - 1];
	pTnrCfg->pk0_c = tnr.weight_limit_uv[0];
	pTnrCfg->pk1_c = tnr.weight_limit_uv[max_lvl_uv - 1];

	//0x008c
	FIX_FLOAT(gain_glb_filt, F_DECI_GAIN, tmp, 0);
	pTnrCfg->glb_gain_cur = tmp;
    FIX_FLOAT(gain_glb_ref1, F_DECI_GAIN, tmp);
	pTnrCfg->glb_gain_nxt = tmp;

	//0x0090
	FIX_FLOAT(gain_glb_filt_sqrt_inv, F_DECI_GAIN_GLB_SQRT_INV, tmp);
	pTnrCfg->glb_gain_cur_div = tmp;
	FIX_FLOAT(gain_glb_filt_sqrt, F_DECI_GAIN_GLB_SQRT, tmp);
	pTnrCfg->glb_gain_cur_sqrt = tmp;

	//0x0094 - 0x0098
	for(i=0; i<TNR_SIGMA_CURVE_SIZE - 1; i++){
		pTnrCfg->sigma_x[i] = log((double)(tnr.fix_x_pos[i + 1] - tnr.fix_x_pos[i])) / log((double)2) - 2;
	}

	//0x009c - 0x00bc
	for(i=0; i<TNR_SIGMA_CURVE_SIZE; i++){
		FIX_FLOAT(tnr.noise_sigma_sample[i], F_DECI_SIGMA, tmp);
		pTnrCfg->sigma_y[i] = tmp;
	}

	//0x00c4 - 0x00cc
	//dir_idx = 0;
	for(i=0; i<TNR_LUMA_CURVE_SIZE; i++){
		FIX_FLOAT(tnr.lumanrcurve[0][i], F_DECI_LUMASCALE, tmp);
		pTnrCfg->luma_curve[i] = tmp;
	}

	//0x00d0
	FIX_FLOAT(tnr.ratio_frq[0], F_DECI_TXT_THRD_RATIO, tmp);
	pTnrCfg->txt_th0_y = tmp;
	FIX_FLOAT(tnr.ratio_frq[1], F_DECI_TXT_THRD_RATIO, tmp);
	pTnrCfg->txt_th1_y = tmp;

	//0x00d4
	FIX_FLOAT(tnr.ratio_frq[2], F_DECI_TXT_THRD_RATIO, tmp);
	pTnrCfg->txt_th0_c = tmp;
	FIX_FLOAT(tnr.ratio_frq[3], F_DECI_TXT_THRD_RATIO, tmp);
	pTnrCfg->txt_th1_c = tmp;

	//0x00d8
	ratio_frq_sub_div[0] = 1 / (tnr.ratio_frq[1] - tnr.ratio_frq[0]);
    ratio_frq_sub_div[1] = 1 / (tnr.ratio_frq[3] - tnr.ratio_frq[2]);
	FIX_FLOAT(ratio_frq_sub_div[0], F_DECI_TXT_THRD_RATIO, tmp, 1);
	pTnrCfg->txt_thy_dlt = tmp;
	FIX_FLOAT(ratio_frq_sub_div[1], F_DECI_TXT_THRD_RATIO, tmp, 1);
	pTnrCfg->txt_thc_dlt = tmp;

	//0x00dc - 0x00ec
	//dir_idx = 0; lvl 0-3;
	//rad_isp20[dir][levl];   gfsigma[lvl]
	tnr_gfcoef_fix(rad_isp20[0][0], tnr.gfsigma[0], pTnrCfg->gfcoef_y0);
	tnr_gfcoef_fix(rad_isp20[0][1], tnr.gfsigma[1], pTnrCfg->gfcoef_y1);
	tnr_gfcoef_fix(rad_isp20[0][2], tnr.gfsigma[2], pTnrCfg->gfcoef_y2);
	tnr_gfcoef_fix(rad_isp20[0][3], tnr.gfsigma[3], pTnrCfg->gfcoef_y3);

	//0x00f0 - 0x0100
	//dir_idx = 0; lvl 0-3;
	//rad_isp20[dir][levl];   gfdelta[[dir_idx][lvl]
	tnr_gfcoef_fix(rad_isp20[0][0], tnr.gfdelta[0][0], pTnrCfg->gfcoef_yg0);
	tnr_gfcoef_fix(rad_isp20[0][1], tnr.gfdelta[0][1], pTnrCfg->gfcoef_yg1);
	tnr_gfcoef_fix(rad_isp20[0][2], tnr.gfdelta[0][2], pTnrCfg->gfcoef_yg2);
	tnr_gfcoef_fix(rad_isp20[0][3], tnr.gfdelta[0][3], pTnrCfg->gfcoef_yg3);

	//0x0104 - 0x0110
	//dir_idx = 1; lvl 0-3;
	//rad_isp20[dir][levl];   gfdelta[[dir_idx][lvl]  ;
	tnr_gfcoef_fix(rad_isp20[1][0], tnr.gfdelta[1][0], pTnrCfg->gfcoef_yl0);
	tnr_gfcoef_fix(rad_isp20[1][1], tnr.gfdelta[1][1], pTnrCfg->gfcoef_yl1);
	tnr_gfcoef_fix(rad_isp20[1][2], tnr.gfdelta[1][2], pTnrCfg->gfcoef_yl2);

	//0x0114 - 0x0120
	//dir_idx = 0; lvl 0-2;
	//rad_isp20[dir][levl];   gfdelta_uv[[dir_idx][lvl]  ;
	tnr_gfcoef_fix(rad_uv_isp20[0][0], tnr.gfdelta_uv[0][0], pTnrCfg->gfcoef_cg0);
	tnr_gfcoef_fix(rad_uv_isp20[0][1], tnr.gfdelta_uv[0][1], pTnrCfg->gfcoef_cg1);
	tnr_gfcoef_fix(rad_uv_isp20[0][2], tnr.gfdelta_uv[0][2], pTnrCfg->gfcoef_cg2);

	//0x0124 - 0x012c
	//dir_idx = 1; lvl 0-1;
	//rad_isp20[dir][levl];   gfdelta_uv[[dir_idx][lvl]  ;
	tnr_gfcoef_fix(rad_uv_isp20[1][0], tnr.gfdelta_uv[1][0], pTnrCfg->gfcoef_cl0);
	tnr_gfcoef_fix(rad_uv_isp20[1][1], tnr.gfdelta_uv[1][1], pTnrCfg->gfcoef_cl1);

	//0x0130 - 0x0134
	//dir_idx = 0;  i = lvl;
	for(i=0; i<TNR_SCALE_YG_SIZE; i++){
		FIX_FLOAT(scale_l_y[0][i], F_DECI_SCALE_L, tmp);
		pTnrCfg->scale_yg[i] = tmp;
	}

	//0x0138 - 0x013c
	//dir_idx = 1;  i = lvl;
	for(i=0; i<TNR_SCALE_YL_SIZE; i++){
		FIX_FLOAT(scale_l_y[1][i], F_DECI_SCALE_L, tmp);
		pTnrCfg->scale_yl[i] = tmp;
	}

	//0x0140 - 0x0148
	//dir_idx = 0;  i = lvl;
	for(i=0; i<TNR_SCALE_CG_SIZE; i++){
		FIX_FLOAT(scale_l_uv[0][i], F_DECI_SCALE_L_UV, tmp);
		pTnrCfg->scale_cg[i] = tmp;
		FIX_FLOAT(scale_l_y_uv[0][i], F_DECI_SCALE_L_UV, tmp);
		pTnrCfg->scale_y2cg[i] = tmp;
	}

	//0x014c - 0x0154
	//dir_idx = 1;  i = lvl;
	for(i=0; i<TNR_SCALE_CL_SIZE; i++){
		FIX_FLOAT(scale_l_uv[1][i], F_DECI_SCALE_L_UV, tmp);
		pTnrCfg->scale_cl[i] = tmp;
	}
	for(i=0; i<TNR_SCALE_Y2CL_SIZE; i++){
		FIX_FLOAT(scale_l_y_uv[1][i], F_DECI_SCALE_L_UV, tmp);
		pTnrCfg->scale_y2cl[i] = tmp;
	}

	//0x0158
	for(i=0; i<TNR_WEIGHT_Y_SIZE; i++){
		FIX_FLOAT(tnr.luma_w_in_chroma[i], F_DECI_LUMA_W_IN_CHROMA, tmp);
		pTnrCfg->weight_y[i] = tmp;
	}

	printf("%s:(%d) exit \n", __FUNCTION__, __LINE__);
}


void
Isp20Params::convertAiqUvnrToIsp20Params(struct rkispp_params_cfg& pp_cfg,
        rk_aiq_isp_uvnr_t& uvnr)
{
	printf("%s:(%d) enter \n", __FUNCTION__, __LINE__);

	int i=0;
	struct rkispp_nr_config  * pNrCfg = &pp_cfg.nr_cfg;

	pp_cfg.module_ens |= ISPP_MODULE_NR;
	pp_cfg.module_en_update |= ISPP_MODULE_NR;
	pp_cfg.module_cfg_update |= ISPP_MODULE_NR;

	//0x0080
	pNrCfg->uvnr_step1_en = 0;
	pNrCfg->uvnr_step2_en = 0;
	pNrCfg->nr_gain_en = 0;
	pNrCfg->uvnr_nobig_en = 0;
	pNrCfg->uvnr_big_en = 0;


	//0x0084
	pNrCfg->uvnr_gain_1sigma = u8(uvnr.ratio * (1 << RKUVNR_ratio));

	//0x0088
	pNrCfg->uvnr_gain_offset = u8(uvnr.offset * (1 << RKUVNR_offset));

	//0x008c
	pNrCfg->uvnr_gain_uvgain[0] = u8(uvnr.uvgain1 * (1 << RKUVNR_uvgain));
	pNrCfg->uvnr_gain_uvgain[1] = u8(uvnr.uvgain3 * (1 << RKUVNR_uvgain));
	pNrCfg->uvnr_gain_t2gen = u8(uvnr.uvgain2 * (1 << RKUVNR_uvgain));
	// no need set
	//pNrCfg->uvnr_gain_iso = uvnr.bfRatio1;

	//0x0090
	pNrCfg->uvnr_t1gen_m3alpha = (uvnr.medRatio1 * (1 << RKUVNR_medRatio));

	//0x0094
	pNrCfg->uvnr_t1flt_mode = uvnr.kernel_9x9_num;

	//0x0098
	pNrCfg->uvnr_t1flt_msigma = u16(uvnr.sigmaR1 * (1 << RKUVNR_sigmaR));

	//0x009c
	pNrCfg->uvnr_t1flt_wtp = u8(uvnr.bfRatio1 * (1 << RKUVNR_bfRatio));

	//0x00a0-0x00a4
	for(i=0; i<NR_UVNR_T1FLT_WTQ_SIZE; i++){
		pNrCfg->uvnr_t1flt_wtq[i] = u8(uvnr.kernel_9x9_table[i] * (1 << RKUVNR_kernels));
	}

	//0x00a8
	pNrCfg->uvnr_t2gen_m3alpha = u8(uvnr.medRatio2 * (1 << RKUVNR_medRatio));

	//0x00ac
	pNrCfg->uvnr_t2gen_msigma = u16(uvnr.sigmaR2 * (1 << RKUVNR_sigmaR));

	//0x00b0
	pNrCfg->uvnr_t2gen_wtp = u8(uvnr.bfRatio2 * (1 << RKUVNR_bfRatio));

	//0x00b4
	for(i=0; i<NR_UVNR_T2GEN_WTQ_SIZE; i++){
		pNrCfg->uvnr_t2gen_wtq[i] = u8(uvnr.kernel_5x5_table[i] * (1 << RKUVNR_kernels));
	}

	//0x00b8
	pNrCfg->uvnr_t2flt_msigma = u16(uvnr.sigmaR3 * (1 << RKUVNR_sigmaR));

	//0x00bc
	pNrCfg->uvnr_t2flt_wtp = u8(uvnr.bfRatio3 * (1 << RKUVNR_bfRatio));
	for(i=0; i<NR_UVNR_T2FLT_WT_SIZE; i++){
		pNrCfg->uvnr_t2flt_wt[i] = u8(uvnr.kernel_3x3_table[i] * (1 << RKUVNR_kernels));
	}


	printf("%s:(%d) exit \n", __FUNCTION__, __LINE__);
}



int  find_top_one_pos(int data)
{
    int i, j = 1;
    int pos = 0;
    for(i=0; i<32; i++)
    {
        if(data&j)
        {
            pos = i+1;
        }
        j = j << 1;
    }
    return pos;
}


void
Isp20Params::convertAiqYnrToIsp20Params(struct rkispp_params_cfg& pp_cfg,
        rk_aiq_isp_ynr_t& ynr)
{
	printf("%s:(%d) enter \n", __FUNCTION__, __LINE__);

	int i = 0;
	int j = 0;
	int tmp = 0;
	struct rkispp_nr_config  * pNrCfg = &pp_cfg.nr_cfg;

	pp_cfg.module_ens |= ISPP_MODULE_NR;
	pp_cfg.module_en_update |= ISPP_MODULE_NR;
	pp_cfg.module_cfg_update |= ISPP_MODULE_NR;
	
	//0x0104 - 0x0108
	for(i=0; i<NR_YNR_SGM_DX_SIZE; i++){
		pNrCfg->ynr_sgm_dx[i] = find_top_one_pos(ynr.lumaPoints[i+1] - ynr.lumaPoints[i]) - 2;
	}

	//0x010c - 0x012c
	for(i = 0; i < NR_YNR_SGM_Y_SIZE; i++){
		float rate;
		for(j = 0; j < 6; j++){
			if(ynr.lumaPoints[i] <= ynr.loFreqLumaNrCurvePoint[j])
				break;
		}

		if(j <= 0)
			rate = ynr.loFreqLumaNrCurveRatio[0];
		else if(j >= 6)
			rate = ynr.loFreqLumaNrCurveRatio[5];
		else{
			rate = ((float)ynr.lumaPoints[i] - ynr.loFreqLumaNrCurvePoint[j-1]) / (ynr.loFreqLumaNrCurvePoint[j] - ynr.loFreqLumaNrCurvePoint[j-1]);
			rate = ynr.loFreqLumaNrCurveRatio[j-1] + rate * (ynr.loFreqLumaNrCurveRatio[j] - ynr.loFreqLumaNrCurveRatio[j-1]);
		}
		tmp = ynr.noiseSigma[i] * (1 << FIX_BIT_NOISE_SIGMA);
		tmp = (int)(rate * tmp);
		//clip sigma be 10bit;
		pNrCfg->ynr_lsgm_y[i] = tmp /(1<<(12 - YNR_SIGMA_BITS));
	}


	//0x0130
	for(i=0; i<NR_YNR_CI_SIZE; i++){
		pNrCfg->ynr_lci[i] = u8(ynr.loFreqNoiseCi[i] * (1 << FIX_BIT_CI));
	}

	//0x0134
	for(i=0; i<NR_YNR_LGAIN_MIN_SIZE; i++){
		pNrCfg->ynr_lgain_min[i] = (u8)(ynr.loFreqBfScale[i] * (1 << FIX_BIT_BF_SCALE));
	}

	//0x0138
	pNrCfg->ynr_lgain_max = (u8)(ynr.loFreqDirectionStrength * (1 << FIX_BIT_DIRECTION_STRENGTH));


	//0x013c
	pNrCfg->ynr_lmerge_bound = (u8)((ynr.loFreqDenoiseStrength[0]) * (1 << FIX_BIT_DENOISE_STRENGTH));
	pNrCfg->ynr_lmerge_ratio = (u8)((ynr.loFreqDenoiseStrength[1]) * (1 << FIX_BIT_DENOISE_STRENGTH));

	//0x0140
	for(i=0; i<NR_YNR_LWEIT_FLT_SIZE; i++){
		pNrCfg->ynr_lweit_flt[i] = (u8)(ynr.loFreqDenoiseWeight[i] * (1 << FIX_BIT_DENOISE_WEIGHT));
	}

	//0x0144 - 0x0164
	for(i = 0; i < NR_YNR_SGM_Y_SIZE; i++){
        float rate;
		for(j = 0; j < 6; j++){
    		if(ynr.lumaPoints[i] <= ynr.hiFreqLumaNrCurvePoint[j])
                break;
    	}

        if(j <= 0)
            rate = ynr.hiFreqLumaNrCurveRatio[0];
        else if(j >= 6)
            rate = ynr.hiFreqLumaNrCurveRatio[5];
        else{
            rate = ((float)ynr.lumaPoints[i] - ynr.hiFreqLumaNrCurvePoint[j-1])
				/ (ynr.hiFreqLumaNrCurvePoint[j] - ynr.hiFreqLumaNrCurvePoint[j-1]);
            rate = ynr.hiFreqLumaNrCurveRatio[j-1]
				+ rate * (ynr.hiFreqLumaNrCurveRatio[j] - ynr.hiFreqLumaNrCurveRatio[j-1]);
        }
		tmp = ynr.noiseSigma[i] * (1 << FIX_BIT_NOISE_SIGMA);
		tmp = (int)(rate * tmp);
		//clip sigma be 10bit;
		pNrCfg->ynr_hsgm_y[i] = tmp /(1<<(12 - YNR_SIGMA_BITS));
	}

	//0x0168
	for(i=0; i<NR_YNR_CI_SIZE; i++){
		pNrCfg->ynr_hlci[i] = u8(ynr.ciISO[i*3+1] * (1 << FIX_BIT_CI));
	}

	//0x016c
	for(i=0; i<NR_YNR_CI_SIZE; i++){
		pNrCfg->ynr_lhci[i] = u8(ynr.ciISO[i*3+0] * (1 << FIX_BIT_CI));
	}

	//0x0170
	for(i=0; i<NR_YNR_CI_SIZE; i++){
		pNrCfg->ynr_hhci[i] = u8(ynr.ciISO[i*3+2] * (1 << FIX_BIT_CI));
	}

	//0x0174
	for(i=0; i<NR_YNR_HGAIN_SGM_SIZE; i++){
		pNrCfg->ynr_hgain_sgm[i] = (u8)(ynr.hiFreqBfScale[i] * (1 << FIX_BIT_BF_SCALE));
	}

	//0x0178 - 0x0188
	int wavelvl = 0;
    int EdgeSoftness = 0;
    for(i=0; i<4; i++){
        if(i == 0)wavelvl = WAVELET_LEVEL_1;
        if(i == 1)wavelvl = WAVELET_LEVEL_2;
        if(i == 2)wavelvl = WAVELET_LEVEL_3;
        if(i == 3)wavelvl = WAVELET_LEVEL_4;
		EdgeSoftness = (int)(ynr.hiFreqEdgeSoftness[wavelvl] * (1 << FIX_BIT_EDGE_SOFTNESS));
        pNrCfg->ynr_hweit_d[0*4 + i] = (int)((exp(-(((0 * 0 + 1 * 1) * (1 << (FIX_BIT_EDGE_SOFTNESS + FIX_BIT_EDGE_SOFTNESS))) / (float)(2 * EdgeSoftness * EdgeSoftness)))) * (1 << YNR_exp_lut_y));
        pNrCfg->ynr_hweit_d[1*4 + i] = (int)((exp(-(((1 * 1 + 1 * 1) * (1 << (FIX_BIT_EDGE_SOFTNESS + FIX_BIT_EDGE_SOFTNESS))) / (float)(2 * EdgeSoftness * EdgeSoftness)))) * (1 << YNR_exp_lut_y));
        pNrCfg->ynr_hweit_d[2*4 + i] = (int)((exp(-(((0 * 0 + 2 * 2) * (1 << (FIX_BIT_EDGE_SOFTNESS + FIX_BIT_EDGE_SOFTNESS))) / (float)(2 * EdgeSoftness * EdgeSoftness)))) * (1 << YNR_exp_lut_y));
        pNrCfg->ynr_hweit_d[3*4 + i] = (int)((exp(-(((1 * 1 + 2 * 2) * (1 << (FIX_BIT_EDGE_SOFTNESS + FIX_BIT_EDGE_SOFTNESS))) / (float)(2 * EdgeSoftness * EdgeSoftness)))) * (1 << YNR_exp_lut_y));
        pNrCfg->ynr_hweit_d[4*4 + i] = (int)((exp(-(((2 * 2 + 2 * 2) * (1 << (FIX_BIT_EDGE_SOFTNESS + FIX_BIT_EDGE_SOFTNESS))) / (float)(2 * EdgeSoftness * EdgeSoftness)))) * (1 << YNR_exp_lut_y));
    }


	//0x018c - 0x01a0
	for(i = 0; i < 6; i++){
		pNrCfg->ynr_hgrad_y[i*4 + 0] = (int)(ynr.detailThreRatioLevel[0][i] * (1 << FIX_BIT_GRAD_ADJUST_CURVE));
		pNrCfg->ynr_hgrad_y[i*4 + 1] = (int)(ynr.detailThreRatioLevel[1][i] * (1 << FIX_BIT_GRAD_ADJUST_CURVE));
		pNrCfg->ynr_hgrad_y[i*4 + 2] = (int)(ynr.detailThreRatioLevel[2][i] * (1 << FIX_BIT_GRAD_ADJUST_CURVE));
		pNrCfg->ynr_hgrad_y[i*4 + 3] = (int)(ynr.detailThreRatioLevel4[i] * (1 << FIX_BIT_GRAD_ADJUST_CURVE));
	}

	//0x01a4 -0x01a8
	for(i=0; i<NR_YNR_HWEIT_SIZE; i++){
		pNrCfg->ynr_hweit[i] = (u16)(ynr.hiFreqDenoiseWeight[i] * (1 << FIX_BIT_DENOISE_WEIGHT));
	}

	//0x01b0
	pNrCfg->ynr_hmax_adjust = (u8)(ynr.detailMinAdjDnW * (1 << FIX_BIT_GRAD_ADJUST_CURVE));

	//0x01b4
	pNrCfg->ynr_hstrength = (u8)(ynr.hiFreqDenoiseStrength * (1 << FIX_BIT_DENOISE_STRENGTH));

	//0x01b8
	pNrCfg->ynr_lweit_cmp[0] = (int)(0.1f * (1 << YNR_exp_lut_y) + 0.5f);//13
	pNrCfg->ynr_lweit_cmp[1] = (int)(0.1f * (1 << YNR_exp_lut_y) + 0.5f);//13


	//0x01bc
	pNrCfg->ynr_lmaxgain_lv4 = ynr.ynr_level4_max_gain;

	//0x01c0 - 0x01e0
	for(i=0; i<NR_YNR_HSTV_Y_SIZE; i++){
		tmp = (int)(ynr.noiseSigma[i] * (1 << FIX_BIT_NOISE_SIGMA));
		pNrCfg->ynr_hstv_y[i] = tmp / (1<<(12 - YNR_SIGMA_BITS));
	}

	//0x01e4  - 0x01e8
	if (strcmp(ynr.ynr_ver_char, "V2") == 0){
		// Lite Version
		for(i=0; i<NR_YNR_ST_SCALE_SIZE; i++){
			pNrCfg->ynr_st_scale[i] = (u16)(ynr.hiFreqSoftThresholdScale[i] * (1 << FIX_BIT_SOFT_THRESHOLD_SCALE_V2));
		}
	}else{
		// old v1 version
		for(i=0; i<NR_YNR_ST_SCALE_SIZE; i++){
			pNrCfg->ynr_st_scale[i] = (u16)(ynr.hiFreqSoftThresholdScale[i] * (1 << FIX_BIT_SOFT_THRESHOLD_SCALE));
		}
	}

	printf("%s:(%d) exit \n", __FUNCTION__, __LINE__);

}


void
Isp20Params::convertAiqSharpenToIsp20Params(struct rkispp_params_cfg& pp_cfg,
        rk_aiq_isp_sharpen_t& sharp, rk_aiq_isp_edgeflt_t& edgeflt)
{
	int i = 0;
	int k = 0;
	int tmp = 0;
	int sum_coeff, offset;
	float sum_coeff_float;
	RKAsharp_Sharp_HW_Params_Select_t *pSharpV1 = &sharp.rk_sharpen_params_selected_V1;
	struct rkispp_sharp_config  * pSharpCfg = &pp_cfg.shp_cfg;

	pp_cfg.module_ens |= ISPP_MODULE_SHP;
	pp_cfg.module_en_update |= ISPP_MODULE_SHP;
	pp_cfg.module_cfg_update |= ISPP_MODULE_SHP;
	
	//0x0080
	pSharpCfg->alpha_adp_en = edgeflt.alpha_adp_en;
	pSharpCfg->yin_flt_en = 0;
	pSharpCfg->edge_avg_en = 0;

	//0x0084
	pSharpCfg->hbf_ratio = (unsigned short)ROUND_F(pSharpV1->hbf_ratio * (1 << reg_sharpenHW_hbf_ratio_fix_bits));
	pSharpCfg->ehf_th = (unsigned short)ROUND_F(pSharpV1->ehf_th);
	pSharpCfg->pbf_ratio = (unsigned short)ROUND_F(pSharpV1->pbf_ratio * (1 << reg_sharpenHW_pbf_ratio_fix_bits));

	//0x0088
	pSharpCfg->edge_thed = edgeflt.edge_thed;
	pSharpCfg->dir_min = (unsigned char)ROUND_F(edgeflt.dir_min * (1 << reg_dir_min_fix_bits));
	pSharpCfg->smoth_th4 = (unsigned short)ROUND_F(edgeflt.smoth4 * (1 << reg_smoth4_fix_bits));

	//0x008c
	pSharpCfg->l_alpha = (unsigned short)ROUND_F(edgeflt.l_alpha * (1 << reg_l_alpha_fix_bits));
	pSharpCfg->g_alpha = (unsigned short)ROUND_F(edgeflt.g_alpha * (1 << reg_g_alpha_fix_bits));


	//0x0090
	sum_coeff = 0;
	sum_coeff_float = 0;
	pSharpCfg->pbf_k[0] = (unsigned char)ROUND_F(pSharpV1->kernel_pbf[4] * (1 << reg_sharpenHW_pPBfCoeff_fix_bits));
	pSharpCfg->pbf_k[1] = (unsigned char)ROUND_F(pSharpV1->kernel_pbf[1] * (1 << reg_sharpenHW_pPBfCoeff_fix_bits));
	pSharpCfg->pbf_k[2] = (unsigned char)ROUND_F(pSharpV1->kernel_pbf[0] * (1 << reg_sharpenHW_pPBfCoeff_fix_bits));
	for(k = 0; k <RKSHAPRENHW_PBF_DIAM * RKSHAPRENHW_PBF_DIAM; k ++)
	{
		sum_coeff += ROUND_F(pSharpV1->kernel_pbf[k] * (1 << reg_sharpenHW_pPBfCoeff_fix_bits));
		sum_coeff_float += pSharpV1->kernel_pbf[k];
	}
	offset = int(sum_coeff_float * (1 << reg_sharpenHW_pPBfCoeff_fix_bits)) - sum_coeff;
	pSharpCfg->pbf_k[0] = pSharpCfg->pbf_k[0] + offset;


	//0x0094 - 0x0098
	sum_coeff = 0;
	sum_coeff_float = 0;
	pSharpCfg->mrf_k[0] = (unsigned char)ROUND_F(pSharpV1->h_rf_m[12] * (1 << reg_sharpenHW_pMRfCoeff_fix_bits));
	pSharpCfg->mrf_k[1] = (unsigned char)ROUND_F(pSharpV1->h_rf_m[7] * (1 << reg_sharpenHW_pMRfCoeff_fix_bits));
	pSharpCfg->mrf_k[2] = (unsigned char)ROUND_F(pSharpV1->h_rf_m[6] * (1 << reg_sharpenHW_pMRfCoeff_fix_bits));
	pSharpCfg->mrf_k[3] = (unsigned char)ROUND_F(pSharpV1->h_rf_m[2] * (1 << reg_sharpenHW_pMRfCoeff_fix_bits));
	pSharpCfg->mrf_k[4] = (unsigned char)ROUND_F(pSharpV1->h_rf_m[1] * (1 << reg_sharpenHW_pMRfCoeff_fix_bits));
	pSharpCfg->mrf_k[5] = (unsigned char)ROUND_F(pSharpV1->h_rf_m[0] * (1 << reg_sharpenHW_pMRfCoeff_fix_bits));
	for(k = 0; k <RKSHAPRENHW_MRF_DIAM * RKSHAPRENHW_MRF_DIAM; k ++)
	{
		sum_coeff += ROUND_F(pSharpV1->h_rf_m[k] * (1 << reg_sharpenHW_pMRfCoeff_fix_bits));
		sum_coeff_float += pSharpV1->h_rf_m[k];
	}
	offset = int(sum_coeff_float * (1 << reg_sharpenHW_pMRfCoeff_fix_bits)) - sum_coeff;
	pSharpCfg->mrf_k[0] = pSharpCfg->mrf_k[0] + offset;

	//0x009c -0x00a4
	sum_coeff = 0;
	sum_coeff_float = 0;
	pSharpCfg->mbf_k[0] = (unsigned char)ROUND_F(pSharpV1->kernel_mbf[110] * (1 << reg_sharpenHW_pMBfCoeff_fix_bits));
	pSharpCfg->mbf_k[1] = (unsigned char)ROUND_F(pSharpV1->kernel_mbf[93] * (1 << reg_sharpenHW_pMBfCoeff_fix_bits));
	pSharpCfg->mbf_k[2] = (unsigned char)ROUND_F(pSharpV1->kernel_mbf[92] * (1 << reg_sharpenHW_pMBfCoeff_fix_bits));
	pSharpCfg->mbf_k[3] = (unsigned char)ROUND_F(pSharpV1->kernel_mbf[76] * (1 << reg_sharpenHW_pMBfCoeff_fix_bits));
	pSharpCfg->mbf_k[4] = (unsigned char)ROUND_F(pSharpV1->kernel_mbf[58] * (1 << reg_sharpenHW_pMBfCoeff_fix_bits));
	pSharpCfg->mbf_k[5] = (unsigned char)ROUND_F(pSharpV1->kernel_mbf[56] * (1 << reg_sharpenHW_pMBfCoeff_fix_bits));
	pSharpCfg->mbf_k[6] = (unsigned char)ROUND_F(pSharpV1->kernel_mbf[25] * (1 << reg_sharpenHW_pMBfCoeff_fix_bits));
	pSharpCfg->mbf_k[7] = (unsigned char)ROUND_F(pSharpV1->kernel_mbf[23] * (1 << reg_sharpenHW_pMBfCoeff_fix_bits));
	pSharpCfg->mbf_k[8] = (unsigned char)ROUND_F(pSharpV1->kernel_mbf[38] * (1 << reg_sharpenHW_pMBfCoeff_fix_bits));
	pSharpCfg->mbf_k[9] = (unsigned char)ROUND_F(pSharpV1->kernel_mbf[4] * (1 << reg_sharpenHW_pMBfCoeff_fix_bits));
	pSharpCfg->mbf_k[10] = (unsigned char)ROUND_F(pSharpV1->kernel_mbf[69] * (1 << reg_sharpenHW_pMBfCoeff_fix_bits));
	pSharpCfg->mbf_k[11] = (unsigned char)ROUND_F(pSharpV1->kernel_mbf[102] * (1 << reg_sharpenHW_pMBfCoeff_fix_bits));
	for(k = 0; k <RKSHAPRENHW_MBF_DIAM_Y * RKSHAPRENHW_MBF_DIAM_X; k ++)
	{
		sum_coeff += ROUND_F(pSharpV1->kernel_mbf[k] * (1 << reg_sharpenHW_pMBfCoeff_fix_bits));
		sum_coeff_float += pSharpV1->kernel_mbf[k];
	}
	offset = int(sum_coeff_float * (1 << reg_sharpenHW_pMBfCoeff_fix_bits)) - sum_coeff;
	pSharpCfg->mbf_k[0] = pSharpCfg->mbf_k[0] + offset;

	//0x00a8 -0x00ac
	sum_coeff = 0;
	sum_coeff_float = 0;
	pSharpCfg->hrf_k[0] = (unsigned char)ROUND_F(pSharpV1->h_rf_h[12] * (1 << reg_sharpenHW_pHRfCoeff_fix_bits));
	pSharpCfg->hrf_k[1] = (unsigned char)ROUND_F(pSharpV1->h_rf_h[7] * (1 << reg_sharpenHW_pHRfCoeff_fix_bits));
	pSharpCfg->hrf_k[2] = (unsigned char)ROUND_F(pSharpV1->h_rf_h[6] * (1 << reg_sharpenHW_pHRfCoeff_fix_bits));
	pSharpCfg->hrf_k[3] = (unsigned char)ROUND_F(pSharpV1->h_rf_h[2] * (1 << reg_sharpenHW_pHRfCoeff_fix_bits));
	pSharpCfg->hrf_k[4] = (unsigned char)ROUND_F(pSharpV1->h_rf_h[1] * (1 << reg_sharpenHW_pHRfCoeff_fix_bits));
	pSharpCfg->hrf_k[5] = (unsigned char)ROUND_F(pSharpV1->h_rf_h[0] * (1 << reg_sharpenHW_pHRfCoeff_fix_bits));
	for(k = 0; k < RKSHAPRENHW_HRF_DIAM * RKSHAPRENHW_HRF_DIAM; k ++)
	{
		sum_coeff += ROUND_F(pSharpV1->h_rf_h[k] * (1 << reg_sharpenHW_pHRfCoeff_fix_bits));
		sum_coeff_float += pSharpV1->h_rf_h[k];
	}
	offset = int(sum_coeff_float * (1 << reg_sharpenHW_pHRfCoeff_fix_bits)) - sum_coeff;
	pSharpCfg->hrf_k[0] = pSharpCfg->hrf_k[0] + offset;

	//0x00b0
	sum_coeff = 0;
	sum_coeff_float = 0;
	pSharpCfg->hbf_k[0] = (unsigned char)ROUND_F(pSharpV1->kernel_hbf[4] * (1 << reg_sharpenHW_pHBfCoeff_fix_bits));
	pSharpCfg->hbf_k[1] = (unsigned char)ROUND_F(pSharpV1->kernel_hbf[1] * (1 << reg_sharpenHW_pHBfCoeff_fix_bits));
	pSharpCfg->hbf_k[2] = (unsigned char)ROUND_F(pSharpV1->kernel_hbf[0] * (1 << reg_sharpenHW_pHBfCoeff_fix_bits));
	for(k = 0; k < RKSHAPRENHW_HBF_DIAM * RKSHAPRENHW_HBF_DIAM; k ++)
	{
		sum_coeff += ROUND_F(pSharpV1->kernel_hbf[k] * (1 << reg_sharpenHW_pHBfCoeff_fix_bits));
		sum_coeff_float += pSharpV1->kernel_hbf[k];
	}
	offset = int(sum_coeff_float * (1 << reg_sharpenHW_pHBfCoeff_fix_bits)) - sum_coeff;
	pSharpCfg->hbf_k[0] = pSharpCfg->hbf_k[0] + offset;

	//0x00b4
	pSharpCfg->eg_coef[0] = (char)ROUND_F(edgeflt.gf_coef_3x3[4] * (1 << reg_gf_coef_3x3_fix_bits));
	pSharpCfg->eg_coef[1] = (char)ROUND_F(edgeflt.gf_coef_3x3[1] * (1 << reg_gf_coef_3x3_fix_bits));
	pSharpCfg->eg_coef[2] = (char)ROUND_F(edgeflt.gf_coef_3x3[0] * (1 << reg_gf_coef_3x3_fix_bits));
	sum_coeff = 0;
	sum_coeff_float = 0;
	for(int k = 0; k <RKEDGEFILTER_SHRP_DIAM 		* RKEDGEFILTER_SHRP_DIAM; k ++)
	{
		sum_coeff += ROUND_F(edgeflt.gf_coef_3x3[k] * (1 << reg_gf_coef_3x3_fix_bits));
		sum_coeff_float += edgeflt.gf_coef_3x3[k];
	}
	offset = int(sum_coeff_float * (1 << reg_gf_coef_3x3_fix_bits)) - sum_coeff;
	pSharpCfg->eg_coef[0] = pSharpCfg->eg_coef[0] + offset;

	//0x00b8
	pSharpCfg->eg_smoth[0] = (unsigned char)ROUND_F(edgeflt.h0_h_coef_5x5[2 * RKEDGEFILTER_DIR_SMTH_DIAM + 2] * (1 << reg_h0_h_coef_5x5_fix_bits));
	pSharpCfg->eg_smoth[1] = (unsigned char)ROUND_F(edgeflt.h0_h_coef_5x5[2 * RKEDGEFILTER_DIR_SMTH_DIAM + 1] * (1 << reg_h0_h_coef_5x5_fix_bits));
	pSharpCfg->eg_smoth[2] = (unsigned char)ROUND_F(edgeflt.h0_h_coef_5x5[2 * RKEDGEFILTER_DIR_SMTH_DIAM + 0] * (1 << reg_h0_h_coef_5x5_fix_bits));
	sum_coeff = 0;
	sum_coeff_float = 0;
	for(int k = 0; k <RKEDGEFILTER_DIR_SMTH_DIAM * RKEDGEFILTER_DIR_SMTH_DIAM; k ++)
	{
		sum_coeff += ROUND_F(edgeflt.h0_h_coef_5x5[k] * (1 << reg_h0_h_coef_5x5_fix_bits));
		sum_coeff_float += edgeflt.h0_h_coef_5x5[k];
	}
	offset = int(sum_coeff_float * (1 << reg_h0_h_coef_5x5_fix_bits)) - sum_coeff;
	pSharpCfg->eg_smoth[0] = pSharpCfg->eg_smoth[0] + offset;

	//0x00bc - 0x00c0
	pSharpCfg->eg_gaus[0] = (unsigned char)ROUND_F(edgeflt.h_coef_5x5[12] * (1 << reg_h_coef_5x5_fix_bits));
	pSharpCfg->eg_gaus[1] = (unsigned char)ROUND_F(edgeflt.h_coef_5x5[7] * (1 << reg_h_coef_5x5_fix_bits));
	pSharpCfg->eg_gaus[2] = (unsigned char)ROUND_F(edgeflt.h_coef_5x5[6] * (1 << reg_h_coef_5x5_fix_bits));
	pSharpCfg->eg_gaus[3] = (unsigned char)ROUND_F(edgeflt.h_coef_5x5[2] * (1 << reg_h_coef_5x5_fix_bits));
	pSharpCfg->eg_gaus[4] = (unsigned char)ROUND_F(edgeflt.h_coef_5x5[1] * (1 << reg_h_coef_5x5_fix_bits));
	pSharpCfg->eg_gaus[5] = (unsigned char)ROUND_F(edgeflt.h_coef_5x5[0] * (1 << reg_h_coef_5x5_fix_bits));
	sum_coeff = 0;
	sum_coeff_float = 0;
	for(int k = 0; k <RKEDGEFILTER_GAUS_DIAM * RKEDGEFILTER_GAUS_DIAM; k ++)
	{
		sum_coeff += ROUND_F(edgeflt.h_coef_5x5[k] * (1 << reg_h_coef_5x5_fix_bits));
		sum_coeff_float += edgeflt.h_coef_5x5[k];
	}
	offset = int(sum_coeff_float * (1 << reg_h_coef_5x5_fix_bits)) - sum_coeff;
	pSharpCfg->eg_gaus[0] = pSharpCfg->eg_gaus[0] + offset;

	//0x00c4 - 0x00c8
	pSharpCfg->dog_k[0] = ( char)ROUND_F(edgeflt.dog_kernel[12] * (1 << reg_dog_kernel_fix_bits));
	pSharpCfg->dog_k[1] = ( char)ROUND_F(edgeflt.dog_kernel[7] * (1 << reg_dog_kernel_fix_bits));
	pSharpCfg->dog_k[2] = ( char)ROUND_F(edgeflt.dog_kernel[6] * (1 << reg_dog_kernel_fix_bits));
	pSharpCfg->dog_k[3] = ( char)ROUND_F(edgeflt.dog_kernel[2] * (1 << reg_dog_kernel_fix_bits));
	pSharpCfg->dog_k[4] = ( char)ROUND_F(edgeflt.dog_kernel[1] * (1 << reg_dog_kernel_fix_bits));
	pSharpCfg->dog_k[5] = ( char)ROUND_F(edgeflt.dog_kernel[0] * (1 << reg_dog_kernel_fix_bits));
	sum_coeff = 0;
	sum_coeff_float = 0;
	for(int k = 0; k <RKEDGEFILTER_DOG_DIAM * RKEDGEFILTER_DOG_DIAM; k ++)
	{
		sum_coeff += ROUND_F(edgeflt.dog_kernel[k] * (1 << reg_dog_kernel_fix_bits));
		sum_coeff_float += edgeflt.dog_kernel[k];
	}
	offset = int(sum_coeff_float * (1 << reg_dog_kernel_fix_bits)) - sum_coeff;
	pSharpCfg->dog_k[0] = pSharpCfg->dog_k[0] + offset;

	//0x00cc - 0x00d0
	for(i=0; i<6; i++){
		pSharpCfg->lum_point[i] = (unsigned char)ROUND_F(pSharpV1->luma_point[i+1]);
	}

	//0x00d4
	//pbf
	int sigma_deci_bits = 7;
	int sigma_inte_bits = 1;
	int max_val			= 0;
	int min_val			= 65536;
	int shf_bits		= 0;
	short sigma_bits[3];
	for(int i = 0; i < 8; i++)
	{
		int cur_sigma = FLOOR((pSharpV1->luma_sigma[i] * pSharpV1->pbf_gain + pSharpV1->pbf_add));
		if(max_val < cur_sigma)
			max_val = cur_sigma;
		if(min_val > cur_sigma)
			min_val = cur_sigma;
	}
	sigma_bits[0] = FLOOR(log((float)min_val)/log((float)2));
	sigma_bits[1] = MAX(sigma_inte_bits - sigma_bits[0], 0);
	sigma_bits[2] = sigma_deci_bits + sigma_bits[0];
	pSharpCfg->pbf_shf_bits = sigma_bits[2] - 5;

	// mf bf sigma inv
	max_val = 0;
	min_val = 65536;
	shf_bits = 0;
	for(int i = 0; i < 8; i++)
	{
		int cur_sigma = FLOOR(pSharpV1->luma_sigma[i]
						* pSharpV1->mbf_gain
						+ pSharpV1->mbf_add);
		if(max_val < cur_sigma)
			max_val = cur_sigma;
		if(min_val > cur_sigma)
			min_val = cur_sigma;
	}
	sigma_bits[0] = FLOOR(log((float)min_val)/log((float)2));
	sigma_bits[1] = MAX(sigma_inte_bits - sigma_bits[0], 0);
	sigma_bits[2] = sigma_deci_bits + sigma_bits[0];
	pSharpCfg->mbf_shf_bits = (unsigned char)(sigma_bits[2] - 5);

	// hf bf sigma inv
	max_val = 0;
	min_val = 65536;
	shf_bits = 0;
	for(int i = 0; i < 8; i++)
	{
		int cur_sigma = FLOOR(pSharpV1->luma_sigma[i] * pSharpV1->hbf_gain + pSharpV1->hbf_add);
		if(max_val < cur_sigma)
			max_val = cur_sigma;
		if(min_val > cur_sigma)
			min_val = cur_sigma;
	}
	sigma_bits[0] = FLOOR(log((float)min_val)/log((float)2));
	sigma_bits[1] = MAX(sigma_inte_bits - sigma_bits[0], 0);
	sigma_bits[2] = sigma_deci_bits + sigma_bits[0];
	pSharpCfg->hbf_shf_bits = (unsigned char)(sigma_bits[2] - 5);


	//0x00d8 - 0x00dc
	// pre bf sigma inv
	sigma_deci_bits = 7;
	sigma_inte_bits = 1;
	max_val			= 0;
	min_val			= 65536;
	shf_bits		= 0;
	for(int i = 0; i < RK_SHARPFILTER_LUMA_POINT_NUM; i++)
	{
		int cur_sigma = FLOOR((pSharpV1->luma_sigma[i] * pSharpV1->pbf_gain + pSharpV1->pbf_add));
		if(max_val < cur_sigma)
			max_val = cur_sigma;
		if(min_val > cur_sigma)
			min_val = cur_sigma;
	}
	sigma_bits[0] = FLOOR(log((float)min_val)/log((float)2));
	sigma_bits[1] = MAX(sigma_inte_bits - sigma_bits[0], 0);
	sigma_bits[2] = sigma_deci_bits + sigma_bits[0];

	for(i=0; i<8; i++){
		pSharpCfg->pbf_sigma[i] = (unsigned char)ROUND_F((float)1
								/ (pSharpV1->luma_sigma[i] * pSharpV1->pbf_gain + pSharpV1->pbf_add)
								* (1 << sigma_bits[2]));
	}

	//0x00e0 - 0x00e4
	for(i=0; i<8; i++){
		pSharpCfg->lum_clp_m[i] = (unsigned char)(pSharpV1->lum_clp_m[i]);
	}

	//0x00e8 - 0x00ec
	for(i=0; i<8; i++){
		pSharpCfg->lum_min_m[i] = (char)ROUND_F(pSharpV1->lum_min_m[i] * (1 << reg_sharpenHW_lum_min_m_fix_bits));
	}

	//0x00f0 - 0x00f4
	// mf bf sigma inv
	max_val = 0;
	min_val = 65536;
	shf_bits = 0;
	for(int i = 0; i < RK_SHARPFILTER_LUMA_POINT_NUM; i++)
	{
		int cur_sigma = FLOOR(pSharpV1->luma_sigma[i] * pSharpV1->mbf_gain + pSharpV1->mbf_add);
		if(max_val < cur_sigma)
			max_val = cur_sigma;
		if(min_val > cur_sigma)
			min_val = cur_sigma;
	}
	sigma_bits[0] = FLOOR(log((float)min_val)/log((float)2));
	sigma_bits[1] = MAX(sigma_inte_bits - sigma_bits[0], 0);
	sigma_bits[2] = sigma_deci_bits + sigma_bits[0];

	for(i=0; i<8; i++){
		pSharpCfg->mbf_sigma[i] = (unsigned char)ROUND_F((float)1
								/ (pSharpV1->luma_sigma[i] * pSharpV1->mbf_gain + pSharpV1->mbf_add)
								* (1 << sigma_bits[2]));
	}

	//0x00f8 - 0x00fc
	for(i=0; i<8; i++){
		pSharpCfg->lum_clp_h[i] = (unsigned char)(pSharpV1->lum_clp_h[i]);
	}

	//0x0100 - 0x0104
	//hbf
	max_val = 0;
	min_val = 65536;
	shf_bits = 0;
	for(int i = 0; i < RK_SHARPFILTER_LUMA_POINT_NUM; i++)
	{
		int cur_sigma = FLOOR(pSharpV1->luma_sigma[i] * pSharpV1->hbf_gain + pSharpV1->hbf_add);
		if(max_val < cur_sigma)
			max_val = cur_sigma;
		if(min_val > cur_sigma)
			min_val = cur_sigma;
	}
	sigma_bits[0] = FLOOR(log((float)min_val)/log((float)2));
	sigma_bits[1] = MAX(sigma_inte_bits - sigma_bits[0], 0);
	sigma_bits[2] = sigma_deci_bits + sigma_bits[0];

	for(i=0; i<8; i++){
		pSharpCfg->hbf_sigma[i] = (unsigned char)ROUND_F((float)1
								/ (pSharpV1->luma_sigma[i] * pSharpV1->hbf_gain + pSharpV1->hbf_add)
								* (1 << sigma_bits[2]));
	}

	//0x0108 - 0x010c
	for(i=0; i<8; i++){
		pSharpCfg->edge_lum_thed[i] = (unsigned char)edgeflt.edge_thed_1[i];
	}

	//0x0110 - 0x0114
	for(i=0; i<8; i++){
		pSharpCfg->clamp_pos[i] = (unsigned char)edgeflt.clamp_pos_dog[i];
	}

	//0x0118 - 0x011c
	for(i=0; i<8; i++){
		pSharpCfg->clamp_neg[i] = (unsigned char)edgeflt.clamp_neg_dog[i];
	}

	//0x0120 - 0x0124
	for(i=0; i<8; i++){
		pSharpCfg->detail_alpha[i] = (unsigned char)ROUND_F(edgeflt.detail_alpha_dog[i] * (1 << reg_detail_alpha_dog_fix_bits));
	}

	//0x0128
	pSharpCfg->rfl_ratio = (unsigned short)ROUND_F(pSharpV1->lratio * (1 << reg_sharpenHW_lratio_fix_bits));
	pSharpCfg->rfh_ratio = (unsigned short)ROUND_F(pSharpV1->hratio * (1 << reg_sharpenHW_hratio_fix_bits));

	//0x012C
	pSharpCfg->m_ratio = (unsigned char)ROUND_F(pSharpV1->M_ratio * (1 << reg_sharpenHW_M_ratio_fix_bits));
	pSharpCfg->h_ratio = (unsigned char)ROUND_F(pSharpV1->H_ratio * (1 << reg_sharpenHW_H_ratio_fix_bits));

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
    convertAiqAhdrToIsp20Params(isp_cfg, aiq_results->data()->ahdr_proc_res);
    convertAiqAfToIsp20Params(isp_cfg, aiq_results->data()->af_meas);
    convertAiqAdehazeToIsp20Params(isp_cfg, aiq_results->data()->adhaz_config);
    convertAiqLscToIsp20Params(isp_cfg, aiq_results->data()->lsc);
    convertAiqCcmToIsp20Params(isp_cfg, aiq_results->data()->ccm);
    convertAiqAgammaToIsp20Params(isp_cfg, aiq_results->data()->agamma_config);
    convertAiqAdemosaicToIsp20Params(isp_cfg, aiq_results);
    convertAiqBlcToIsp20Params(isp_cfg, aiq_results);
    convertAiqDpccToIsp20Params(isp_cfg, aiq_results);
	convertAiqRawnrToIsp20Params(isp_cfg, aiq_results->data()->rawnr);

    return ret;
}

XCamReturn
Isp20Params::checkIsp20Params(struct isp2x_isp_params_cfg& isp_cfg)
{

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

}; //namspace RkCam
