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
#include "../../xcore/base/xcam_common.h"
#include "rk_aiq_agamma_algo.h"

RKAIQ_BEGIN_DECLARE




void select_RkGamma_params(const CalibDb_Gamma_t * stRKGammaParam, RKAiqAgammaHtmlConfig_Select_t *stRKgammaParamSelected, int workMode)
{
    int i;

    stRKgammaParamSelected->gamma_en = stRKGammaParam->gamma_en;
    stRKgammaParamSelected->gamma_out_mode = stRKGammaParam->gamma_out_mode;
    stRKgammaParamSelected->gamma_out_offset = stRKGammaParam->gamma_out_offset;
    stRKgammaParamSelected->gamma_out_segnum = stRKGammaParam->gamma_out_segnum;

    if(workMode == GAMMA_OUT_NORMAL)
    {
        for (i = 0; i < 45; i++)
        {
            stRKgammaParamSelected->gamma_table[i] = stRKGammaParam->curve_normal[i];
        }
    }
    else if(workMode == GAMMA_OUT_HDR)
    {
        for (i = 0; i < 45; i++)
        {
            stRKgammaParamSelected->gamma_table[i] = stRKGammaParam->curve_hdr[i];
        }
    }
    else if(workMode == GAMMA_OUT_NIGHT)
    {
        for (i = 0; i < 45; i++)
        {
            stRKgammaParamSelected->gamma_table[i] = stRKGammaParam->curve_night[i];
        }
    }
    else if(workMode == GAMMA_OUT_USR1)
    {
        for (i = 0; i < 45; i++)
        {
            stRKgammaParamSelected->gamma_table[i] = stRKGammaParam->curve_user1[i];
        }
    }
    else if(workMode == GAMMA_OUT_USR2)
    {
        for (i = 0; i < 45; i++)
        {
            stRKgammaParamSelected->gamma_table[i] = stRKGammaParam->curve_user2[i];
        }
    }
    else return;




}




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
XCamReturn AgammaConfigV200(const CalibDb_Gamma_t * AgammaHtmlPara, AgammaHandle_t* para)
{

    LOGD_AGAMMA("ENTER: %s \n", __func__);
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    int workmode = AgammaHtmlPara->gamma_out_mode;


    select_RkGamma_params(AgammaHtmlPara, (RKAiqAgammaHtmlConfig_Select_t*)&para->agamma_config,  workmode);
    para->calib_gamma = (CalibDb_Gamma_t *)AgammaHtmlPara;

    return ret;
}
XCamReturn AgammaPreProcV200(AgammaHandle_t* para)
{
    LOGD_AGAMMA("ENTER: %s \n", __func__);

    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    return(ret);

}
/*
XCamReturn AgammaProcessingV200(AgammaHandle_t* para)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    LOGD_AGAMMA("ENTER: %s \n", __func__);

    ret = AgammaReConfigV200(para);

    return(ret);
}
*/

XCamReturn  AgammaReConfigV200(AgammaHandle_t *para, const CalibDb_Gamma_t * gamma_calib)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    LOGD_AGAMMA("ENTER: %s \n", __func__);

    int workmode = gamma_calib->gamma_out_mode;


    select_RkGamma_params(gamma_calib, (RKAiqAgammaHtmlConfig_Select_t*)&para->agamma_config,   workmode);


    return(ret);
}


RKAIQ_END_DECLARE


