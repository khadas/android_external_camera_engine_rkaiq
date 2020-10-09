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

static void select_Dehaze_params_algo(const CalibDb_Dehaze_t * stRKDehazeParam, rk_aiq_dehaze_cfg_t *stRKDehazeParamSelected, int iso, int mode)
{
    LOG1_ADEHAZE("ENTER: %s \n", __func__);

    // dehaze_self_adp[7]
    LinearInterp(stRKDehazeParam->dehaze_setting[mode].iso, stRKDehazeParam->dehaze_setting[mode].dc_min_th, iso, &stRKDehazeParamSelected->dehaze_self_adp[0], RK_DEHAZE_ISO_NUM);
    LinearInterp(stRKDehazeParam->dehaze_setting[mode].iso, stRKDehazeParam->dehaze_setting[mode].dc_max_th, iso, &stRKDehazeParamSelected->dehaze_self_adp[1], RK_DEHAZE_ISO_NUM);
    LinearInterp(stRKDehazeParam->dehaze_setting[mode].iso, stRKDehazeParam->dehaze_setting[mode].yhist_th, iso, &stRKDehazeParamSelected->dehaze_self_adp[2], RK_DEHAZE_ISO_NUM);
    LinearInterp(stRKDehazeParam->dehaze_setting[mode].iso, stRKDehazeParam->dehaze_setting[mode].yblk_th, iso, &stRKDehazeParamSelected->dehaze_self_adp[3], RK_DEHAZE_ISO_NUM);
    LinearInterp(stRKDehazeParam->dehaze_setting[mode].iso, stRKDehazeParam->dehaze_setting[mode].dark_th, iso, &stRKDehazeParamSelected->dehaze_self_adp[4], RK_DEHAZE_ISO_NUM);
    LinearInterp(stRKDehazeParam->dehaze_setting[mode].iso, stRKDehazeParam->dehaze_setting[mode].bright_min, iso, &stRKDehazeParamSelected->dehaze_self_adp[5], RK_DEHAZE_ISO_NUM);
    LinearInterp(stRKDehazeParam->dehaze_setting[mode].iso, stRKDehazeParam->dehaze_setting[mode].bright_max, iso, &stRKDehazeParamSelected->dehaze_self_adp[6], RK_DEHAZE_ISO_NUM);

    // dehaze_range_adj[6]
    LinearInterp(stRKDehazeParam->dehaze_setting[mode].iso, stRKDehazeParam->dehaze_setting[mode].wt_max, iso, &stRKDehazeParamSelected->dehaze_range_adj[0], RK_DEHAZE_ISO_NUM);
    LinearInterp(stRKDehazeParam->dehaze_setting[mode].iso, stRKDehazeParam->dehaze_setting[mode].air_max, iso, &stRKDehazeParamSelected->dehaze_range_adj[1], RK_DEHAZE_ISO_NUM);
    LinearInterp(stRKDehazeParam->dehaze_setting[mode].iso, stRKDehazeParam->dehaze_setting[mode].air_min, iso, &stRKDehazeParamSelected->dehaze_range_adj[2], RK_DEHAZE_ISO_NUM);
    LinearInterp(stRKDehazeParam->dehaze_setting[mode].iso, stRKDehazeParam->dehaze_setting[mode].tmax_base, iso, &stRKDehazeParamSelected->dehaze_range_adj[3], RK_DEHAZE_ISO_NUM);
    LinearInterp(stRKDehazeParam->dehaze_setting[mode].iso, stRKDehazeParam->dehaze_setting[mode].tmax_off, iso, &stRKDehazeParamSelected->dehaze_range_adj[4], RK_DEHAZE_ISO_NUM);
    LinearInterp(stRKDehazeParam->dehaze_setting[mode].iso, stRKDehazeParam->dehaze_setting[mode].tmax_max, iso, &stRKDehazeParamSelected->dehaze_range_adj[5], RK_DEHAZE_ISO_NUM);

    // dehaze_iir_control[5]
    stRKDehazeParamSelected->dehaze_iir_control[0] = stRKDehazeParam->dehaze_setting[mode].IIR_setting.stab_fnum;
    stRKDehazeParamSelected->dehaze_iir_control[1] = stRKDehazeParam->dehaze_setting[mode].IIR_setting.sigma;
    stRKDehazeParamSelected->dehaze_iir_control[2] = stRKDehazeParam->dehaze_setting[mode].IIR_setting.wt_sigma;
    stRKDehazeParamSelected->dehaze_iir_control[3] = stRKDehazeParam->dehaze_setting[mode].IIR_setting.air_sigma;
    stRKDehazeParamSelected->dehaze_iir_control[4] = stRKDehazeParam->dehaze_setting[mode].IIR_setting.tmax_sigma;

    // dehaze_user_config[5]
    LinearInterp(stRKDehazeParam->dehaze_setting[mode].iso, stRKDehazeParam->dehaze_setting[mode].cfg_wt, iso, &stRKDehazeParamSelected->dehaze_user_config[1], RK_DEHAZE_ISO_NUM);
    LinearInterp(stRKDehazeParam->dehaze_setting[mode].iso, stRKDehazeParam->dehaze_setting[mode].cfg_air, iso, &stRKDehazeParamSelected->dehaze_user_config[2], RK_DEHAZE_ISO_NUM);
    LinearInterp(stRKDehazeParam->dehaze_setting[mode].iso, stRKDehazeParam->dehaze_setting[mode].cfg_tmax, iso, &stRKDehazeParamSelected->dehaze_user_config[3], RK_DEHAZE_ISO_NUM);

    // dehaze_bi_para[4]
    LinearInterp(stRKDehazeParam->dehaze_setting[mode].iso, stRKDehazeParam->dehaze_setting[mode].dc_thed, iso, &stRKDehazeParamSelected->dehaze_bi_para[0], RK_DEHAZE_ISO_NUM);
    LinearInterp(stRKDehazeParam->dehaze_setting[mode].iso, stRKDehazeParam->dehaze_setting[mode].dc_weitcur, iso, &stRKDehazeParamSelected->dehaze_bi_para[1], RK_DEHAZE_ISO_NUM);
    LinearInterp(stRKDehazeParam->dehaze_setting[mode].iso, stRKDehazeParam->dehaze_setting[mode].air_thed, iso, &stRKDehazeParamSelected->dehaze_bi_para[2], RK_DEHAZE_ISO_NUM);
    LinearInterp(stRKDehazeParam->dehaze_setting[mode].iso, stRKDehazeParam->dehaze_setting[mode].air_weitcur, iso, &stRKDehazeParamSelected->dehaze_bi_para[3], RK_DEHAZE_ISO_NUM);

    LOGD_ADEHAZE("%s dehaze_en:%f dc_en:%f\n", __func__, stRKDehazeParamSelected->dehaze_en[0], stRKDehazeParamSelected->dehaze_en[1]);
    LOGD_ADEHAZE("%s dc_min_th:%f dc_max_th:%f yhist_th:%f yblk_th:%f dark_th:%f bright_min:%f bright_max:%f\n", __func__, stRKDehazeParamSelected->dehaze_self_adp[0], stRKDehazeParamSelected->dehaze_self_adp[1],
                 stRKDehazeParamSelected->dehaze_self_adp[2], stRKDehazeParamSelected->dehaze_self_adp[3], stRKDehazeParamSelected->dehaze_self_adp[4], stRKDehazeParamSelected->dehaze_self_adp[5]
                 , stRKDehazeParamSelected->dehaze_self_adp[6]);
    LOGD_ADEHAZE("%s wt_max:%f air_max:%f air_min:%f tmax_base:%f tmax_off:%f tmax_max:%f\n", __func__, stRKDehazeParamSelected->dehaze_range_adj[0], stRKDehazeParamSelected->dehaze_range_adj[1],
                 stRKDehazeParamSelected->dehaze_range_adj[2], stRKDehazeParamSelected->dehaze_range_adj[3], stRKDehazeParamSelected->dehaze_range_adj[4], stRKDehazeParamSelected->dehaze_range_adj[5]);
    LOGD_ADEHAZE("%s stab_fnum:%f sigma:%f wt_sigma:%f air_sigma:%f tmax_sigma:%f\n", __func__, stRKDehazeParamSelected->dehaze_iir_control[0], stRKDehazeParamSelected->dehaze_iir_control[1],
                 stRKDehazeParamSelected->dehaze_iir_control[2], stRKDehazeParamSelected->dehaze_iir_control[3], stRKDehazeParamSelected->dehaze_iir_control[4]);
    LOGD_ADEHAZE("%s cfg_alpha:%f cfg_wt:%f cfg_air:%f cfg_tmax:%f\n", __func__, stRKDehazeParamSelected->dehaze_user_config[0], stRKDehazeParamSelected->dehaze_user_config[1],
                 stRKDehazeParamSelected->dehaze_user_config[2], stRKDehazeParamSelected->dehaze_user_config[3]);
    LOGD_ADEHAZE("%s dc_thed:%f dc_weitcur:%f air_thed:%f air_weitcur:%f\n", __func__, stRKDehazeParamSelected->dehaze_bi_para[0], stRKDehazeParamSelected->dehaze_bi_para[1],
                 stRKDehazeParamSelected->dehaze_bi_para[2], stRKDehazeParamSelected->dehaze_bi_para[3]);

    // dehaze_dc_bf_h[25]
    for (int i = 0; i < 25; i++)
        stRKDehazeParamSelected->dehaze_dc_bf_h[i] = stRKDehazeParam->dehaze_setting[mode].dc_bf_h[i];

    // dehaze_air_bf_h[9],dehaze_gaus_h[9]
    for (int i = 0; i < 9; i++)
    {
        stRKDehazeParamSelected->dehaze_air_bf_h[i] = stRKDehazeParam->dehaze_setting[mode].air_bf_h[i];
        stRKDehazeParamSelected->dehaze_gaus_h[i] = stRKDehazeParam->dehaze_setting[mode].gaus_h[i];
    }

    LOG1_ADEHAZE("EIXT: %s \n", __func__);
}

static void select_Enhance_params_algo(const CalibDb_Dehaze_t * stRKDehazeParam, rk_aiq_dehaze_cfg_t *stRKDehazeParamSelected, int iso, int mode)
{
    LOG1_ADEHAZE("ENTER: %s \n", __func__);

    LinearInterp(stRKDehazeParam->enhance_setting[mode].iso, stRKDehazeParam->enhance_setting[mode].enhance_value, iso, &stRKDehazeParamSelected->dehaze_enhance[1], RK_DEHAZE_ISO_NUM);


    LOGD_ADEHAZE("%s enhance_en:%f enhance_value:%f\n", __func__, stRKDehazeParamSelected->dehaze_enhance[0], stRKDehazeParamSelected->dehaze_enhance[1]);

    LOG1_ADEHAZE("EIXT: %s \n", __func__);
}

static void select_Hist_params_algo(const CalibDb_Dehaze_t * stRKDehazeParam, rk_aiq_dehaze_cfg_t *stRKDehazeParamSelected, int iso, int mode)
{
    LOG1_ADEHAZE("ENTER: %s \n", __func__);

    LinearInterpEnable(stRKDehazeParam->hist_setting[mode].iso, stRKDehazeParam->hist_setting[mode].hist_channel, iso, &stRKDehazeParamSelected->dehaze_en[3], RK_DEHAZE_ISO_NUM);
    LinearInterpEnable(stRKDehazeParam->hist_setting[mode].iso, stRKDehazeParam->hist_setting[mode].hist_para_en, iso, &stRKDehazeParamSelected->dehaze_enhance[2], RK_DEHAZE_ISO_NUM);
    LinearInterp(stRKDehazeParam->hist_setting[mode].iso, stRKDehazeParam->hist_setting[mode].hist_gratio, iso, &stRKDehazeParamSelected->dehaze_hist_para[0], RK_DEHAZE_ISO_NUM);
    LinearInterp(stRKDehazeParam->hist_setting[mode].iso, stRKDehazeParam->hist_setting[mode].hist_th_off, iso, &stRKDehazeParamSelected->dehaze_hist_para[1], RK_DEHAZE_ISO_NUM);
    LinearInterp(stRKDehazeParam->hist_setting[mode].iso, stRKDehazeParam->hist_setting[mode].hist_k, iso, &stRKDehazeParamSelected->dehaze_hist_para[2], RK_DEHAZE_ISO_NUM);
    LinearInterp(stRKDehazeParam->hist_setting[mode].iso, stRKDehazeParam->hist_setting[mode].hist_min, iso, &stRKDehazeParamSelected->dehaze_hist_para[3], RK_DEHAZE_ISO_NUM);
    LinearInterp(stRKDehazeParam->hist_setting[mode].iso, stRKDehazeParam->hist_setting[mode].hist_scale, iso, &stRKDehazeParamSelected->dehaze_enhance[3], RK_DEHAZE_ISO_NUM);
    LinearInterp(stRKDehazeParam->hist_setting[mode].iso, stRKDehazeParam->hist_setting[mode].cfg_gratio, iso, &stRKDehazeParamSelected->dehaze_user_config[4], RK_DEHAZE_ISO_NUM);

    // dehaze_hist_t0[6],dehaze_hist_t1[6],dehaze_hist_t2[6]
    for (int i = 0; i < 6; i++)
    {
        stRKDehazeParamSelected->dehaze_hist_t0[i] = stRKDehazeParam->hist_setting[mode].hist_conv_t0[i];
        stRKDehazeParamSelected->dehaze_hist_t1[i] = stRKDehazeParam->hist_setting[mode].hist_conv_t1[i];
        stRKDehazeParamSelected->dehaze_hist_t2[i] = stRKDehazeParam->hist_setting[mode].hist_conv_t2[i];
    }

    LOGD_ADEHAZE("%s dehaze_en:%f hist_en:%f hist_channel:%f hist_para_en:%f\n", __func__, stRKDehazeParamSelected->dehaze_en[0], stRKDehazeParamSelected->dehaze_en[2], stRKDehazeParamSelected->dehaze_en[3],
                 stRKDehazeParamSelected->dehaze_en[4], stRKDehazeParamSelected->dehaze_enhance[2]);
    LOGD_ADEHAZE("%s hist_gratio:%f hist_th_off:%f hist_k:%f hist_min:%f\n", __func__, stRKDehazeParamSelected->dehaze_hist_para[0], stRKDehazeParamSelected->dehaze_hist_para[1],
                 stRKDehazeParamSelected->dehaze_hist_para[2], stRKDehazeParamSelected->dehaze_hist_para[3]);
    LOGD_ADEHAZE("%s hist_scale:%f cfg_alpha:%f cfg_gratio:%f\n", __func__, stRKDehazeParamSelected->dehaze_enhance[3], stRKDehazeParamSelected->dehaze_user_config[0],
                 stRKDehazeParamSelected->dehaze_user_config[4]);

    LOG1_ADEHAZE("EIXT: %s \n", __func__);

}

XCamReturn AdehazeInit(AdehazeHandle_t** para, CamCalibDbContext_t* calib)
{
    LOG1_ADEHAZE("ENTER: %s \n", __func__);
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    AdehazeHandle_t *handle = (AdehazeHandle_t*)malloc(sizeof(AdehazeHandle_t));
    if (NULL == handle)
        return XCAM_RETURN_ERROR_MEM;
    memset(handle, 0, sizeof(AdehazeHandle_t));
    handle->pCalibDb = calib;
    handle->calib_dehaz = calib->dehaze;
    handle->mode = RK_AIQ_DEHAZE_MODE_DEFAULT;
    *para = handle;
    LOG1_ADEHAZE("EXIT: %s \n", __func__);
    return(ret);
}

XCamReturn AdehazeRelease(AdehazeHandle_t* para)
{
    LOG1_ADEHAZE("ENTER: %s \n", __func__);
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    if (para)
        free(para);
    LOG1_ADEHAZE("EXIT: %s \n", __func__);
    return(ret);
}

XCamReturn AdehazeProcess(AdehazeHandle_t* para, int iso, int mode)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    LOG1_ADEHAZE("ENTER: %s \n", __func__);

    //fuction enable
    if(para->calib_dehaz.en == 0)
        para->adhaz_config.dehaze_en[0] = 0;
    else
    {
        para->adhaz_config.dehaze_en[0] = 1;
        if(para->calib_dehaz.dehaze_setting[mode].en != 0 && para->calib_dehaz.enhance_setting[mode].en != 0)
        {
            para->adhaz_config.dehaze_en[1] = 1;
            para->adhaz_config.dehaze_enhance[0] = 1;
        }
        else if(para->calib_dehaz.dehaze_setting[mode].en != 0 && para->calib_dehaz.enhance_setting[mode].en == 0)
        {
            para->adhaz_config.dehaze_en[1] = 1;
            para->adhaz_config.dehaze_enhance[0] = 0;
        }
        else if(para->calib_dehaz.dehaze_setting[mode].en == 0 && para->calib_dehaz.enhance_setting[mode].en != 0)
        {
            para->adhaz_config.dehaze_en[1] = 1;
            para->adhaz_config.dehaze_enhance[0] = 1;
        }
        else
        {
            para->adhaz_config.dehaze_en[1] = 0;
            para->adhaz_config.dehaze_enhance[0] = 0;
            LOGD_ADEHAZE("%s: Dehaze and Enhance do not open!!!\n", __func__);
        }

        if(para->calib_dehaz.hist_setting[mode].en != 0)
            para->adhaz_config.dehaze_en[2] = 1;
        else
            para->adhaz_config.dehaze_en[2] = 0;

    }

    //others
    para->adhaz_config.dehaze_en[4] = para->calib_dehaz.gain_en;
    if(mode == 0)
        para->adhaz_config.dehaze_user_config[0] = para->calib_dehaz.cfg_alpha_normal;
    else if(mode == 1)
        para->adhaz_config.dehaze_user_config[0] = para->calib_dehaz.cfg_alpha_hdr;
    else if(mode == 2)
        para->adhaz_config.dehaze_user_config[0] = para->calib_dehaz.cfg_alpha_night;
    else
        LOGE_ADEHAZE("%s Wrong mode in Dehaze!!!\n", __func__);
    LOGD_ADEHAZE("%s ISO:%d mode:%d\n", __func__, iso, mode);

    //dehaze setting
    select_Dehaze_params_algo(&para->calib_dehaz, &para->adhaz_config, iso, mode);

    //enhance setting
    select_Enhance_params_algo(&para->calib_dehaz, &para->adhaz_config, iso, mode);

    //hist setting
    select_Hist_params_algo(&para->calib_dehaz, &para->adhaz_config, iso, mode);


    LOG1_ADEHAZE("EXIT: %s \n", __func__);
    return ret;
}

RKAIQ_END_DECLARE


