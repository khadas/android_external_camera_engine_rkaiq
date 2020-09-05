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

XCamReturn AgammaInit(AgammaHandle_t** para, CamCalibDbContext_t* calib)
{
    LOGD_AGAMMA("ENTER: %s \n", __func__);
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    AgammaHandle_t* handle = (AgammaHandle_t*)malloc(sizeof(AgammaHandle_t));
    if (NULL == handle)
        return XCAM_RETURN_ERROR_MEM;
    memset(handle, 0, sizeof(AgammaHandle_t));
    handle->pCalibDb = calib;
    ret = AgammaConfigInit(handle);
    *para = handle;
    LOGD_AGAMMA("EXIT: %s \n", __func__);
    return(ret);

}

XCamReturn AgammaRelease(AgammaHandle_t* para)
{
    LOGD_AGAMMA("ENTER: %s \n", __func__);
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    if (para)
        free(para);
    LOGD_AGAMMA("EXIT: %s \n", __func__);
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

    for (i = 0; i < 45; i++)
    {
        para->normal_table[i] = gamma_calib->curve_normal[i];
        para->hdr_table[i] = gamma_calib->curve_hdr[i];
        para->night_table[i] = gamma_calib->curve_night[i];
        para->user1_table[i] = gamma_calib->curve_user1[i];
        para->user2_table[i] = gamma_calib->curve_user2[i];
    }

    if(gamma_calib->gamma_out_mode == GAMMA_OUT_NORMAL)
    {
        memcpy(para->agamma_config.gamma_table, para->normal_table, sizeof(para->normal_table));
    }
    else if(gamma_calib->gamma_out_mode == GAMMA_OUT_HDR)
    {
        memcpy(para->agamma_config.gamma_table, para->hdr_table, sizeof(para->hdr_table));
    }
    else if(gamma_calib->gamma_out_mode == GAMMA_OUT_NIGHT)
    {
        memcpy(para->agamma_config.gamma_table, para->night_table, sizeof(para->night_table));
    }
    else if(gamma_calib->gamma_out_mode == GAMMA_OUT_USR1)
    {
        memcpy(para->agamma_config.gamma_table, para->user1_table, sizeof(para->user1_table));
    }
    else if(gamma_calib->gamma_out_mode == GAMMA_OUT_USR2)
    {
        memcpy(para->agamma_config.gamma_table, para->user2_table, sizeof(para->user2_table));
    }
    else {
        LOGE_AGAMMA("%s: gamma out mode is not support!\n", __func__);
        ret = XCAM_RETURN_ERROR_PARAM;
    }
    LOGD_AGAMMA("EXIT: %s \n", __func__);
    return ret;
}

XCamReturn AgammaPreProc(AgammaHandle_t* para)
{
    LOGD_AGAMMA("ENTER: %s \n", __func__);

    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    LOGD_AGAMMA("EXIT: %s \n", __func__);
    return(ret);

}

RKAIQ_END_DECLARE


