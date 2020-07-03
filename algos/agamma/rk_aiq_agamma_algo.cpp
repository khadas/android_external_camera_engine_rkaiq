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
#include "xcam_common.h"
#include "rk_aiq_agamma_algo.h"

RKAIQ_BEGIN_DECLARE

XCamReturn AgammaInitV200(AgammaHandle_t** para)
{
    LOGD_AGAMMA("ENTER: %s \n", __func__);

    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    *para = (AgammaHandle_t*)malloc(sizeof(AgammaHandle_t));
    return(ret);

}

XCamReturn AgammaReleaseV200(AgammaHandle_t* para)
{
    LOGD_AGAMMA("ENTER: %s \n", __func__);

    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    free(para);
    return(ret);

}

XCamReturn AgammaConfigInit(AgammaHandle_t* para)
{

    LOGD_AGAMMA("ENTER: %s \n", __func__);
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    int i;

    CalibDb_Gamma_t *gamma_calib = &para->pCalibDb->gamma;
    para->agamma_config.gamma_en = gamma_calib->gamma_en;
    para->agamma_config.gamma_out_mode = gamma_calib->gamma_out_mode;
    para->agamma_config.gamma_out_offset = gamma_calib->gamma_out_offset;
    para->agamma_config.gamma_out_segnum = gamma_calib->gamma_out_segnum;

    if(gamma_calib->gamma_out_mode == GAMMA_OUT_NORMAL)
    {
        for (i = 0; i < 45; i++)
        {
            para->agamma_config.gamma_table[i] = gamma_calib->curve_normal[i];
        }
    }
    else if(gamma_calib->gamma_out_mode == GAMMA_OUT_HDR)
    {
        for (i = 0; i < 45; i++)
        {
            para->agamma_config.gamma_table[i] = gamma_calib->curve_hdr[i];
        }
    }
    else if(gamma_calib->gamma_out_mode == GAMMA_OUT_NIGHT)
    {
        for (i = 0; i < 45; i++)
        {
            para->agamma_config.gamma_table[i] = gamma_calib->curve_night[i];
        }
    }
    else if(gamma_calib->gamma_out_mode == GAMMA_OUT_USR1)
    {
        for (i = 0; i < 45; i++)
        {
            para->agamma_config.gamma_table[i] = gamma_calib->curve_user1[i];
        }
    }
    else if(gamma_calib->gamma_out_mode == GAMMA_OUT_USR2)
    {
        for (i = 0; i < 45; i++)
        {
            para->agamma_config.gamma_table[i] = gamma_calib->curve_user2[i];
        }
    }
    else {
        LOGE_AGAMMA("%s: gamma out mode is not support!\n", __func__);
        ret = XCAM_RETURN_ERROR_PARAM;
    }
    return ret;
}

XCamReturn AgammaPreProcV200(AgammaHandle_t* para)
{
    LOGD_AGAMMA("ENTER: %s \n", __func__);

    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    return(ret);

}

RKAIQ_END_DECLARE


