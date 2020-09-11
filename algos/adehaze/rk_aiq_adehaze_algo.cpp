/*
 * rk_aiq_algo_acprc_itf.c
 *
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
#include <string.h>
#include "rk_aiq_adehaze_algo.h"
#include "xcam_log.h"

RKAIQ_BEGIN_DECLARE

static void LinearInterp(const float *pX, const float *pY, float posx, float *yOut, int XSize)
{
    int index;

    if (posx >= pX[XSize - 1])
    {
        *yOut = pY[XSize - 1];
    }
    else if (posx <= pX[0])
    {
        *yOut = pY[0];
    }
    else
    {
        index = 0;
        while((posx >= pX[index]) && (index < XSize))
        {
            index++;
        }
        index -= 1;
        *yOut = (((float)pY[index + 1] - (float)pY[index]) / ((float)pX[index + 1] - (float)pX[index]) * ((float)posx - (float)pX[index]))
                + pY[index];
    }

}
static void LinearInterpEnable(const float *pX, const unsigned char *pY, float posx, float *yOut, int XSize)
{
    int index;
    float out;
    if (posx >= pX[XSize - 1])
    {
        out = (float)pY[XSize - 1];
    }
    else if (posx <= pX[0])
    {
        out = pY[0];
    }
    else
    {
        index = 0;
        while((posx >= pX[index]) && (index < XSize))
        {
            index++;
        }
        index -= 1;
        out = (((float)pY[index + 1] - (float)pY[index]) / ((float)pX[index + 1] - (float)pX[index]) * ((float)posx - (float)pX[index]))
              + pY[index];
    }
    *yOut = out > 0.5 ? 1 : 0;

}


static void select_RkDehaze_params_algo(const CalibDb_Dehaze_t * stRKDehazeParam, rk_aiq_dehaze_cfg_t *stRKDehazeParamSelected, int iso, int mode)
{
    int i;
    if(mode == 0) {
        LinearInterpEnable(stRKDehazeParam->normal.iso, stRKDehazeParam->normal.dehaze_en, iso, &stRKDehazeParamSelected->dehaze_en[0], RK_DEHAZE_ISO_NUM);
        LinearInterpEnable(stRKDehazeParam->normal.iso, stRKDehazeParam->normal.dc_en, iso, &stRKDehazeParamSelected->dehaze_en[1], RK_DEHAZE_ISO_NUM);
        LinearInterpEnable(stRKDehazeParam->normal.iso, stRKDehazeParam->normal.hist_en, iso, &stRKDehazeParamSelected->dehaze_en[2], RK_DEHAZE_ISO_NUM);
        LinearInterpEnable(stRKDehazeParam->normal.iso, stRKDehazeParam->normal.hist_channel, iso, &stRKDehazeParamSelected->dehaze_en[3], RK_DEHAZE_ISO_NUM);
        LinearInterpEnable(stRKDehazeParam->normal.iso, stRKDehazeParam->normal.gain_en, iso, &stRKDehazeParamSelected->dehaze_en[4], RK_DEHAZE_ISO_NUM);

        /*
              LinearInterp(stRKDehazeParam->normal.iso, stRKDehazeParam->normal.dehaze_en, iso, &stRKDehazeParamSelected->dehaze_en[0], RK_DEHAZE_ISO_NUM);

            LinearInterp(stRKDehazeParam.normal.iso, stRKDehazeParam.normal.dc_en, iso, &stRKDehazeParamSelected->dehaze_en[1], RK_DEHAZE_ISO_NUM);
            LinearInterp(stRKDehazeParam.normal.iso, stRKDehazeParam.normal.hist_en, iso, &stRKDehazeParamSelected->dehaze_en[2], RK_DEHAZE_ISO_NUM);
            LinearInterp(stRKDehazeParam.normal.iso, stRKDehazeParam.normal.hist_channel, iso, &stRKDehazeParamSelected->dehaze_en[3], RK_DEHAZE_ISO_NUM);
            LinearInterp(stRKDehazeParam.normal.iso, stRKDehazeParam.normal.gain_en, iso, &stRKDehazeParamSelected->dehaze_en[4], RK_DEHAZE_ISO_NUM);*/

        // dehaze_self_adp[7]

        LinearInterp(stRKDehazeParam->normal.iso, stRKDehazeParam->normal.dc_min_th, iso, &stRKDehazeParamSelected->dehaze_self_adp[0], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->normal.iso, stRKDehazeParam->normal.dc_max_th, iso, &stRKDehazeParamSelected->dehaze_self_adp[1], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->normal.iso, stRKDehazeParam->normal.yhist_th, iso, &stRKDehazeParamSelected->dehaze_self_adp[2], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->normal.iso, stRKDehazeParam->normal.yblk_th, iso, &stRKDehazeParamSelected->dehaze_self_adp[3], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->normal.iso, stRKDehazeParam->normal.dark_th, iso, &stRKDehazeParamSelected->dehaze_self_adp[4], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->normal.iso, stRKDehazeParam->normal.bright_min, iso, &stRKDehazeParamSelected->dehaze_self_adp[5], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->normal.iso, stRKDehazeParam->normal.bright_max, iso, &stRKDehazeParamSelected->dehaze_self_adp[6], RK_DEHAZE_ISO_NUM);

        // dehaze_range_adj[6]

        LinearInterp(stRKDehazeParam->normal.iso, stRKDehazeParam->normal.wt_max, iso, &stRKDehazeParamSelected->dehaze_range_adj[0], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->normal.iso, stRKDehazeParam->normal.air_max, iso, &stRKDehazeParamSelected->dehaze_range_adj[1], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->normal.iso, stRKDehazeParam->normal.air_min, iso, &stRKDehazeParamSelected->dehaze_range_adj[2], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->normal.iso, stRKDehazeParam->normal.tmax_base, iso, &stRKDehazeParamSelected->dehaze_range_adj[3], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->normal.iso, stRKDehazeParam->normal.tmax_off, iso, &stRKDehazeParamSelected->dehaze_range_adj[4], RK_DEHAZE_ISO_NUM);

        LinearInterp(stRKDehazeParam->normal.iso, stRKDehazeParam->normal.tmax_max, iso, &stRKDehazeParamSelected->dehaze_range_adj[5], RK_DEHAZE_ISO_NUM);

        // dehaze_hist_para[4]

        LinearInterp(stRKDehazeParam->normal.iso, stRKDehazeParam->normal.hist_gratio, iso, &stRKDehazeParamSelected->dehaze_hist_para[0], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->normal.iso, stRKDehazeParam->normal.hist_th_off, iso, &stRKDehazeParamSelected->dehaze_hist_para[1], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->normal.iso, stRKDehazeParam->normal.hist_k, iso, &stRKDehazeParamSelected->dehaze_hist_para[2], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->normal.iso, stRKDehazeParam->normal.hist_min, iso, &stRKDehazeParamSelected->dehaze_hist_para[3], RK_DEHAZE_ISO_NUM);

        // dehaze_enhance[4]

        LinearInterpEnable(stRKDehazeParam->normal.iso, stRKDehazeParam->normal.enhance_en, iso, &stRKDehazeParamSelected->dehaze_enhance[0], RK_DEHAZE_ISO_NUM);

        //stRKDehazeParamSelected->dehaze_enhance[0] = stRKDehazeParam->normal.enhance_en;
        LinearInterp(stRKDehazeParam->normal.iso, stRKDehazeParam->normal.enhance_value, iso, &stRKDehazeParamSelected->dehaze_enhance[1], RK_DEHAZE_ISO_NUM);
        LinearInterpEnable(stRKDehazeParam->normal.iso, stRKDehazeParam->normal.hist_para_en, iso, &stRKDehazeParamSelected->dehaze_enhance[2], RK_DEHAZE_ISO_NUM);
        //stRKDehazeParamSelected->dehaze_enhance[2] = stRKDehazeParam->normal.hist_para_en;
        LinearInterp(stRKDehazeParam->normal.iso, stRKDehazeParam->normal.hist_scale, iso, &stRKDehazeParamSelected->dehaze_enhance[3], RK_DEHAZE_ISO_NUM);

        // dehaze_iir_control[5]

        LinearInterp(stRKDehazeParam->normal.iso, stRKDehazeParam->normal.stab_fnum, iso, &stRKDehazeParamSelected->dehaze_iir_control[0], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->normal.iso, stRKDehazeParam->normal.sigma, iso, &stRKDehazeParamSelected->dehaze_iir_control[1], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->normal.iso, stRKDehazeParam->normal.wt_sigma, iso, &stRKDehazeParamSelected->dehaze_iir_control[2], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->normal.iso, stRKDehazeParam->normal.air_sigma, iso, &stRKDehazeParamSelected->dehaze_iir_control[3], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->normal.iso, stRKDehazeParam->normal.tmax_sigma, iso, &stRKDehazeParamSelected->dehaze_iir_control[4], RK_DEHAZE_ISO_NUM);

        // dehaze_user_config[5]

        LinearInterp(stRKDehazeParam->normal.iso, stRKDehazeParam->normal.cfg_alpha, iso, &stRKDehazeParamSelected->dehaze_user_config[0], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->normal.iso, stRKDehazeParam->normal.cfg_wt, iso, &stRKDehazeParamSelected->dehaze_user_config[1], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->normal.iso, stRKDehazeParam->normal.cfg_air, iso, &stRKDehazeParamSelected->dehaze_user_config[2], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->normal.iso, stRKDehazeParam->normal.cfg_tmax, iso, &stRKDehazeParamSelected->dehaze_user_config[3], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->normal.iso, stRKDehazeParam->normal.cfg_gratio, iso, &stRKDehazeParamSelected->dehaze_user_config[4], RK_DEHAZE_ISO_NUM);

        // dehaze_bi_para[4]

        LinearInterp(stRKDehazeParam->normal.iso, stRKDehazeParam->normal.dc_thed, iso, &stRKDehazeParamSelected->dehaze_bi_para[0], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->normal.iso, stRKDehazeParam->normal.dc_weitcur, iso, &stRKDehazeParamSelected->dehaze_bi_para[1], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->normal.iso, stRKDehazeParam->normal.air_thed, iso, &stRKDehazeParamSelected->dehaze_bi_para[2], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->normal.iso, stRKDehazeParam->normal.air_weitcur, iso, &stRKDehazeParamSelected->dehaze_bi_para[3], RK_DEHAZE_ISO_NUM);
    }
    else if(mode == 1) {
        LinearInterpEnable(stRKDehazeParam->hdr.iso, stRKDehazeParam->hdr.dehaze_en, iso, &stRKDehazeParamSelected->dehaze_en[0], RK_DEHAZE_ISO_NUM);
        LinearInterpEnable(stRKDehazeParam->hdr.iso, stRKDehazeParam->hdr.dc_en, iso, &stRKDehazeParamSelected->dehaze_en[1], RK_DEHAZE_ISO_NUM);
        LinearInterpEnable(stRKDehazeParam->hdr.iso, stRKDehazeParam->hdr.hist_en, iso, &stRKDehazeParamSelected->dehaze_en[2], RK_DEHAZE_ISO_NUM);
        LinearInterpEnable(stRKDehazeParam->hdr.iso, stRKDehazeParam->hdr.hist_channel, iso, &stRKDehazeParamSelected->dehaze_en[3], RK_DEHAZE_ISO_NUM);
        LinearInterpEnable(stRKDehazeParam->hdr.iso, stRKDehazeParam->hdr.gain_en, iso, &stRKDehazeParamSelected->dehaze_en[4], RK_DEHAZE_ISO_NUM);

        /*
              LinearInterp(stRKDehazeParam->hdr.iso, stRKDehazeParam->hdr.dehaze_en, iso, &stRKDehazeParamSelected->dehaze_en[0], RK_DEHAZE_ISO_NUM);

            LinearInterp(stRKDehazeParam.hdr.iso, stRKDehazeParam.hdr.dc_en, iso, &stRKDehazeParamSelected->dehaze_en[1], RK_DEHAZE_ISO_NUM);
            LinearInterp(stRKDehazeParam.hdr.iso, stRKDehazeParam.hdr.hist_en, iso, &stRKDehazeParamSelected->dehaze_en[2], RK_DEHAZE_ISO_NUM);
            LinearInterp(stRKDehazeParam.hdr.iso, stRKDehazeParam.hdr.hist_channel, iso, &stRKDehazeParamSelected->dehaze_en[3], RK_DEHAZE_ISO_NUM);
            LinearInterp(stRKDehazeParam.hdr.iso, stRKDehazeParam.hdr.gain_en, iso, &stRKDehazeParamSelected->dehaze_en[4], RK_DEHAZE_ISO_NUM);*/

        // dehaze_self_adp[7]

        LinearInterp(stRKDehazeParam->hdr.iso, stRKDehazeParam->hdr.dc_min_th, iso, &stRKDehazeParamSelected->dehaze_self_adp[0], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->hdr.iso, stRKDehazeParam->hdr.dc_max_th, iso, &stRKDehazeParamSelected->dehaze_self_adp[1], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->hdr.iso, stRKDehazeParam->hdr.yhist_th, iso, &stRKDehazeParamSelected->dehaze_self_adp[2], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->hdr.iso, stRKDehazeParam->hdr.yblk_th, iso, &stRKDehazeParamSelected->dehaze_self_adp[3], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->hdr.iso, stRKDehazeParam->hdr.dark_th, iso, &stRKDehazeParamSelected->dehaze_self_adp[4], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->hdr.iso, stRKDehazeParam->hdr.bright_min, iso, &stRKDehazeParamSelected->dehaze_self_adp[5], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->hdr.iso, stRKDehazeParam->hdr.bright_max, iso, &stRKDehazeParamSelected->dehaze_self_adp[6], RK_DEHAZE_ISO_NUM);

        // dehaze_range_adj[6]

        LinearInterp(stRKDehazeParam->hdr.iso, stRKDehazeParam->hdr.wt_max, iso, &stRKDehazeParamSelected->dehaze_range_adj[0], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->hdr.iso, stRKDehazeParam->hdr.air_max, iso, &stRKDehazeParamSelected->dehaze_range_adj[1], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->hdr.iso, stRKDehazeParam->hdr.air_min, iso, &stRKDehazeParamSelected->dehaze_range_adj[2], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->hdr.iso, stRKDehazeParam->hdr.tmax_base, iso, &stRKDehazeParamSelected->dehaze_range_adj[3], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->hdr.iso, stRKDehazeParam->hdr.tmax_off, iso, &stRKDehazeParamSelected->dehaze_range_adj[4], RK_DEHAZE_ISO_NUM);

        LinearInterp(stRKDehazeParam->hdr.iso, stRKDehazeParam->hdr.tmax_max, iso, &stRKDehazeParamSelected->dehaze_range_adj[5], RK_DEHAZE_ISO_NUM);

        // dehaze_hist_para[4]

        LinearInterp(stRKDehazeParam->hdr.iso, stRKDehazeParam->hdr.hist_gratio, iso, &stRKDehazeParamSelected->dehaze_hist_para[0], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->hdr.iso, stRKDehazeParam->hdr.hist_th_off, iso, &stRKDehazeParamSelected->dehaze_hist_para[1], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->hdr.iso, stRKDehazeParam->hdr.hist_k, iso, &stRKDehazeParamSelected->dehaze_hist_para[2], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->hdr.iso, stRKDehazeParam->hdr.hist_min, iso, &stRKDehazeParamSelected->dehaze_hist_para[3], RK_DEHAZE_ISO_NUM);

        // dehaze_enhance[4]

        LinearInterpEnable(stRKDehazeParam->hdr.iso, stRKDehazeParam->hdr.enhance_en, iso, &stRKDehazeParamSelected->dehaze_enhance[0], RK_DEHAZE_ISO_NUM);

        //stRKDehazeParamSelected->dehaze_enhance[0] = stRKDehazeParam->hdr.enhance_en;
        LinearInterp(stRKDehazeParam->hdr.iso, stRKDehazeParam->hdr.enhance_value, iso, &stRKDehazeParamSelected->dehaze_enhance[1], RK_DEHAZE_ISO_NUM);
        LinearInterpEnable(stRKDehazeParam->hdr.iso, stRKDehazeParam->hdr.hist_para_en, iso, &stRKDehazeParamSelected->dehaze_enhance[2], RK_DEHAZE_ISO_NUM);
        //stRKDehazeParamSelected->dehaze_enhance[2] = stRKDehazeParam->hdr.hist_para_en;
        LinearInterp(stRKDehazeParam->hdr.iso, stRKDehazeParam->hdr.hist_scale, iso, &stRKDehazeParamSelected->dehaze_enhance[3], RK_DEHAZE_ISO_NUM);

        // dehaze_iir_control[5]

        LinearInterp(stRKDehazeParam->hdr.iso, stRKDehazeParam->hdr.stab_fnum, iso, &stRKDehazeParamSelected->dehaze_iir_control[0], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->hdr.iso, stRKDehazeParam->hdr.sigma, iso, &stRKDehazeParamSelected->dehaze_iir_control[1], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->hdr.iso, stRKDehazeParam->hdr.wt_sigma, iso, &stRKDehazeParamSelected->dehaze_iir_control[2], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->hdr.iso, stRKDehazeParam->hdr.air_sigma, iso, &stRKDehazeParamSelected->dehaze_iir_control[3], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->hdr.iso, stRKDehazeParam->hdr.tmax_sigma, iso, &stRKDehazeParamSelected->dehaze_iir_control[4], RK_DEHAZE_ISO_NUM);

        // dehaze_user_config[5]

        LinearInterp(stRKDehazeParam->hdr.iso, stRKDehazeParam->hdr.cfg_alpha, iso, &stRKDehazeParamSelected->dehaze_user_config[0], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->hdr.iso, stRKDehazeParam->hdr.cfg_wt, iso, &stRKDehazeParamSelected->dehaze_user_config[1], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->hdr.iso, stRKDehazeParam->hdr.cfg_air, iso, &stRKDehazeParamSelected->dehaze_user_config[2], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->hdr.iso, stRKDehazeParam->hdr.cfg_tmax, iso, &stRKDehazeParamSelected->dehaze_user_config[3], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->hdr.iso, stRKDehazeParam->hdr.cfg_gratio, iso, &stRKDehazeParamSelected->dehaze_user_config[4], RK_DEHAZE_ISO_NUM);

        // dehaze_bi_para[4]

        LinearInterp(stRKDehazeParam->hdr.iso, stRKDehazeParam->hdr.dc_thed, iso, &stRKDehazeParamSelected->dehaze_bi_para[0], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->hdr.iso, stRKDehazeParam->hdr.dc_weitcur, iso, &stRKDehazeParamSelected->dehaze_bi_para[1], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->hdr.iso, stRKDehazeParam->hdr.air_thed, iso, &stRKDehazeParamSelected->dehaze_bi_para[2], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->hdr.iso, stRKDehazeParam->hdr.air_weitcur, iso, &stRKDehazeParamSelected->dehaze_bi_para[3], RK_DEHAZE_ISO_NUM);
    }
    else if(mode == 2) {
        LinearInterpEnable(stRKDehazeParam->night.iso, stRKDehazeParam->night.dehaze_en, iso, &stRKDehazeParamSelected->dehaze_en[0], RK_DEHAZE_ISO_NUM);
        LinearInterpEnable(stRKDehazeParam->night.iso, stRKDehazeParam->night.dc_en, iso, &stRKDehazeParamSelected->dehaze_en[1], RK_DEHAZE_ISO_NUM);
        LinearInterpEnable(stRKDehazeParam->night.iso, stRKDehazeParam->night.hist_en, iso, &stRKDehazeParamSelected->dehaze_en[2], RK_DEHAZE_ISO_NUM);
        LinearInterpEnable(stRKDehazeParam->night.iso, stRKDehazeParam->night.hist_channel, iso, &stRKDehazeParamSelected->dehaze_en[3], RK_DEHAZE_ISO_NUM);
        LinearInterpEnable(stRKDehazeParam->night.iso, stRKDehazeParam->night.gain_en, iso, &stRKDehazeParamSelected->dehaze_en[4], RK_DEHAZE_ISO_NUM);

        /*
              LinearInterp(stRKDehazeParam->night.iso, stRKDehazeParam->night.dehaze_en, iso, &stRKDehazeParamSelected->dehaze_en[0], RK_DEHAZE_ISO_NUM);

            LinearInterp(stRKDehazeParam.night.iso, stRKDehazeParam.night.dc_en, iso, &stRKDehazeParamSelected->dehaze_en[1], RK_DEHAZE_ISO_NUM);
            LinearInterp(stRKDehazeParam.night.iso, stRKDehazeParam.night.hist_en, iso, &stRKDehazeParamSelected->dehaze_en[2], RK_DEHAZE_ISO_NUM);
            LinearInterp(stRKDehazeParam.night.iso, stRKDehazeParam.night.hist_channel, iso, &stRKDehazeParamSelected->dehaze_en[3], RK_DEHAZE_ISO_NUM);
            LinearInterp(stRKDehazeParam.night.iso, stRKDehazeParam.night.gain_en, iso, &stRKDehazeParamSelected->dehaze_en[4], RK_DEHAZE_ISO_NUM);*/

        // dehaze_self_adp[7]

        LinearInterp(stRKDehazeParam->night.iso, stRKDehazeParam->night.dc_min_th, iso, &stRKDehazeParamSelected->dehaze_self_adp[0], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->night.iso, stRKDehazeParam->night.dc_max_th, iso, &stRKDehazeParamSelected->dehaze_self_adp[1], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->night.iso, stRKDehazeParam->night.yhist_th, iso, &stRKDehazeParamSelected->dehaze_self_adp[2], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->night.iso, stRKDehazeParam->night.yblk_th, iso, &stRKDehazeParamSelected->dehaze_self_adp[3], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->night.iso, stRKDehazeParam->night.dark_th, iso, &stRKDehazeParamSelected->dehaze_self_adp[4], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->night.iso, stRKDehazeParam->night.bright_min, iso, &stRKDehazeParamSelected->dehaze_self_adp[5], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->night.iso, stRKDehazeParam->night.bright_max, iso, &stRKDehazeParamSelected->dehaze_self_adp[6], RK_DEHAZE_ISO_NUM);

        // dehaze_range_adj[6]

        LinearInterp(stRKDehazeParam->night.iso, stRKDehazeParam->night.wt_max, iso, &stRKDehazeParamSelected->dehaze_range_adj[0], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->night.iso, stRKDehazeParam->night.air_max, iso, &stRKDehazeParamSelected->dehaze_range_adj[1], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->night.iso, stRKDehazeParam->night.air_min, iso, &stRKDehazeParamSelected->dehaze_range_adj[2], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->night.iso, stRKDehazeParam->night.tmax_base, iso, &stRKDehazeParamSelected->dehaze_range_adj[3], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->night.iso, stRKDehazeParam->night.tmax_off, iso, &stRKDehazeParamSelected->dehaze_range_adj[4], RK_DEHAZE_ISO_NUM);

        LinearInterp(stRKDehazeParam->night.iso, stRKDehazeParam->night.tmax_max, iso, &stRKDehazeParamSelected->dehaze_range_adj[5], RK_DEHAZE_ISO_NUM);

        // dehaze_hist_para[4]

        LinearInterp(stRKDehazeParam->night.iso, stRKDehazeParam->night.hist_gratio, iso, &stRKDehazeParamSelected->dehaze_hist_para[0], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->night.iso, stRKDehazeParam->night.hist_th_off, iso, &stRKDehazeParamSelected->dehaze_hist_para[1], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->night.iso, stRKDehazeParam->night.hist_k, iso, &stRKDehazeParamSelected->dehaze_hist_para[2], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->night.iso, stRKDehazeParam->night.hist_min, iso, &stRKDehazeParamSelected->dehaze_hist_para[3], RK_DEHAZE_ISO_NUM);

        // dehaze_enhance[4]

        LinearInterpEnable(stRKDehazeParam->night.iso, stRKDehazeParam->night.enhance_en, iso, &stRKDehazeParamSelected->dehaze_enhance[0], RK_DEHAZE_ISO_NUM);

        //stRKDehazeParamSelected->dehaze_enhance[0] = stRKDehazeParam->night.enhance_en;
        LinearInterp(stRKDehazeParam->night.iso, stRKDehazeParam->night.enhance_value, iso, &stRKDehazeParamSelected->dehaze_enhance[1], RK_DEHAZE_ISO_NUM);
        LinearInterpEnable(stRKDehazeParam->night.iso, stRKDehazeParam->night.hist_para_en, iso, &stRKDehazeParamSelected->dehaze_enhance[2], RK_DEHAZE_ISO_NUM);
        //stRKDehazeParamSelected->dehaze_enhance[2] = stRKDehazeParam->night.hist_para_en;
        LinearInterp(stRKDehazeParam->night.iso, stRKDehazeParam->night.hist_scale, iso, &stRKDehazeParamSelected->dehaze_enhance[3], RK_DEHAZE_ISO_NUM);

        // dehaze_iir_control[5]

        LinearInterp(stRKDehazeParam->night.iso, stRKDehazeParam->night.stab_fnum, iso, &stRKDehazeParamSelected->dehaze_iir_control[0], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->night.iso, stRKDehazeParam->night.sigma, iso, &stRKDehazeParamSelected->dehaze_iir_control[1], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->night.iso, stRKDehazeParam->night.wt_sigma, iso, &stRKDehazeParamSelected->dehaze_iir_control[2], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->night.iso, stRKDehazeParam->night.air_sigma, iso, &stRKDehazeParamSelected->dehaze_iir_control[3], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->night.iso, stRKDehazeParam->night.tmax_sigma, iso, &stRKDehazeParamSelected->dehaze_iir_control[4], RK_DEHAZE_ISO_NUM);

        // dehaze_user_config[5]

        LinearInterp(stRKDehazeParam->night.iso, stRKDehazeParam->night.cfg_alpha, iso, &stRKDehazeParamSelected->dehaze_user_config[0], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->night.iso, stRKDehazeParam->night.cfg_wt, iso, &stRKDehazeParamSelected->dehaze_user_config[1], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->night.iso, stRKDehazeParam->night.cfg_air, iso, &stRKDehazeParamSelected->dehaze_user_config[2], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->night.iso, stRKDehazeParam->night.cfg_tmax, iso, &stRKDehazeParamSelected->dehaze_user_config[3], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->night.iso, stRKDehazeParam->night.cfg_gratio, iso, &stRKDehazeParamSelected->dehaze_user_config[4], RK_DEHAZE_ISO_NUM);

        // dehaze_bi_para[4]

        LinearInterp(stRKDehazeParam->night.iso, stRKDehazeParam->night.dc_thed, iso, &stRKDehazeParamSelected->dehaze_bi_para[0], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->night.iso, stRKDehazeParam->night.dc_weitcur, iso, &stRKDehazeParamSelected->dehaze_bi_para[1], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->night.iso, stRKDehazeParam->night.air_thed, iso, &stRKDehazeParamSelected->dehaze_bi_para[2], RK_DEHAZE_ISO_NUM);
        LinearInterp(stRKDehazeParam->night.iso, stRKDehazeParam->night.air_weitcur, iso, &stRKDehazeParamSelected->dehaze_bi_para[3], RK_DEHAZE_ISO_NUM);
    }
    // dehaze_dc_bf_h[25]
    for (i = 0; i < 25; i++)
    {
        stRKDehazeParamSelected->dehaze_dc_bf_h[i] = stRKDehazeParam->dc_bf_h[i];
    }
    // dehaze_air_bf_h[9],dehaze_gaus_h[9]
    for (i = 0; i < 9; i++)
    {
        stRKDehazeParamSelected->dehaze_air_bf_h[i] = stRKDehazeParam->air_bf_h[i];
        stRKDehazeParamSelected->dehaze_gaus_h[i] = stRKDehazeParam->gaus_h[i];
    }
    // dehaze_hist_t0[6],dehaze_hist_t1[6],dehaze_hist_t2[6]
    for (i = 0; i < 6; i++)
    {
        stRKDehazeParamSelected->dehaze_hist_t0[i] = stRKDehazeParam->hist_conv_t0[i];
        stRKDehazeParamSelected->dehaze_hist_t1[i] = stRKDehazeParam->hist_conv_t1[i];
        stRKDehazeParamSelected->dehaze_hist_t2[i] = stRKDehazeParam->hist_conv_t2[i];
    }

}

XCamReturn AdehazeInit(AdehazeHandle_t** para, CamCalibDbContext_t* calib)
{
    LOGD_ADEHAZE("ENTER: %s \n", __func__);
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    AdehazeHandle_t *handle = (AdehazeHandle_t*)malloc(sizeof(AdehazeHandle_t));
    if (NULL == handle)
        return XCAM_RETURN_ERROR_MEM;
    memset(handle, 0, sizeof(AdehazeHandle_t));
    handle->pCalibDb = calib;
    handle->calib_dehaz = calib->dehaze;
    handle->mode = RK_AIQ_DEHAZE_MODE_DEFAULT;
    *para = handle;
    LOGD_ADEHAZE("EXIT: %s \n", __func__);
    return(ret);
}

XCamReturn AdehazeRelease(AdehazeHandle_t* para)
{
    LOGD_ADEHAZE("ENTER: %s \n", __func__);
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    if (para)
        free(para);
    LOGD_ADEHAZE("EXIT: %s \n", __func__);
    return(ret);
}

XCamReturn AdehazeProcess(AdehazeHandle_t* para, int iso, int mode)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    LOGD_ADEHAZE("ENTER: %s \n", __func__);
    select_RkDehaze_params_algo(&para->calib_dehaz, &para->adhaz_config, iso, mode);
    LOGD_ADEHAZE("EXIT: %s \n", __func__);
    return ret;
}

RKAIQ_END_DECLARE


