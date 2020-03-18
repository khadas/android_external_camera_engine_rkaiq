/*
 * rk_aiq_algo_adhaz_itf.c
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
/* for rockchip v2.0.0*/

#include "rk_aiq_algo_types_int.h"
#include "rk_aiq_algo_adhaz_itf.h"
#include "RkAiqCalibDbTypes.h"
#include "adehaze/dehaze.h"
#include "RkAiqCalibDbTypes.h"

RKAIQ_BEGIN_DECLARE

static XCamReturn
create_context(RkAiqAlgoContext **context, const AlgoCtxInstanceCfg* cfg)
{
    AdehazeHandle_t*AdehazeHandle = NULL;
    XCamReturn ret;
    ret = AdehazeInitV200(&AdehazeHandle);
    *context = (RkAiqAlgoContext *)(AdehazeHandle);
    return XCAM_RETURN_NO_ERROR;

}

static XCamReturn
destroy_context(RkAiqAlgoContext *context)
{
    AdehazeHandle_t*AdehazeHandle = (AdehazeHandle_t*)context;
    XCamReturn ret;

    ret = AdehazeReleaseV200(AdehazeHandle);

    return XCAM_RETURN_NO_ERROR;
}

static XCamReturn
prepare(RkAiqAlgoCom* params)
{
    XCamReturn ret;
	RkAiqAlgoConfigAdhazInt* config = (RkAiqAlgoConfigAdhazInt*)params;
    AdehazeHandle_t * AdehazeHandle = (AdehazeHandle_t *)params->ctx;
    RKAiqAdhazHtmlConfig_t adhaz_html_para;
    CamCalibDbContext_t* calib = config->rk_com.u.prepare.calib;
	const CalibDb_Dehaze_t *calib_dehaze = &calib->dehaze;
    //TO DO

    ret = AdehazeConfigV200(calib_dehaze, AdehazeHandle);
    return XCAM_RETURN_NO_ERROR;

}

static XCamReturn
pre_process(const RkAiqAlgoCom* inparams, RkAiqAlgoResCom* outparams)
{
    return XCAM_RETURN_NO_ERROR;
}

static XCamReturn
processing(const RkAiqAlgoCom* inparams, RkAiqAlgoResCom* outparams)
{
    XCamReturn ret;
    AdehazeHandle_t * AdehazeHandle = (AdehazeHandle_t *)inparams->ctx;
    RkAiqAlgoProcAdhazInt* procPara = (RkAiqAlgoProcAdhazInt*)inparams;
    RkAiqAlgoProcResAdhazInt* procResPara = (RkAiqAlgoProcResAdhazInt*)outparams;
    RKAiqAdhazConfig_t* adhaz_config;
	const CalibDb_Dehaze_t *calib_dehaze = AdehazeHandle->calib_dehaz;
    adhaz_config = (RKAiqAdhazConfig_t*)&procResPara->adhaz_config;

    if(inparams->u.proc.init == false) {
        ret = AdehazeReConfigV200(AdehazeHandle, calib_dehaze);
    }
    memcpy(adhaz_config, &AdehazeHandle->adhaz_config, sizeof(RKAiqAdhazConfig_t));
    return XCAM_RETURN_NO_ERROR;

}

static XCamReturn
post_process(const RkAiqAlgoCom* inparams, RkAiqAlgoResCom* outparams)
{
    return XCAM_RETURN_NO_ERROR;
}

RkAiqAlgoDescription g_RkIspAlgoDescAdhaz = {
    .common = {
        .version = RKISP_ALGO_ADHAZ_VERSION,
        .vendor  = RKISP_ALGO_ADHAZ_VENDOR,
        .description = RKISP_ALGO_ADHAZ_DESCRIPTION,
        .type    = RK_AIQ_ALGO_TYPE_ADHAZ,
        .id      = 0,
        .create_context  = create_context,
        .destroy_context = destroy_context,
    },
    .prepare = prepare,
    .pre_process = pre_process,
    .processing = processing,
    .post_process = post_process,
};

RKAIQ_END_DECLARE
