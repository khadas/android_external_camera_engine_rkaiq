/*
 * rk_aiq_algo_agamma_itf.c
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

#include "rk_aiq_algo_types_int.h"
#include "agamma/rk_aiq_algo_agamma_itf.h"
#include "agamma/rk_aiq_agamma_algo.h"
RKAIQ_BEGIN_DECLARE

typedef struct _RkAiqAlgoContext {
    void* place_holder[0];
} RkAiqAlgoContext;

static RkAiqAlgoContext ctx;

static XCamReturn
create_context(RkAiqAlgoContext **context, const AlgoCtxInstanceCfg* cfg)
{
    AgammaHandle_t*AgammaHandle = NULL;
    XCamReturn ret;
    ret = AgammaInitV200(&AgammaHandle);
    *context = (RkAiqAlgoContext *)(AgammaHandle);
    return XCAM_RETURN_NO_ERROR;
}

static XCamReturn
destroy_context(RkAiqAlgoContext *context)
{
    AgammaHandle_t*AgammaHandle = (AgammaHandle_t*)context;
    XCamReturn ret;

    ret = AgammaReleaseV200(AgammaHandle);

    return XCAM_RETURN_NO_ERROR;
}

static XCamReturn
prepare(RkAiqAlgoCom* params)
{

    XCamReturn ret;
    RkAiqAlgoConfigAgammaInt* config = (RkAiqAlgoConfigAgammaInt*)params;

    int work_mode;
    AgammaHandle_t * AgammaHandle = (AgammaHandle_t *)params->ctx;
    CamCalibDbContext_t* calib = config->rk_com.u.prepare.calib;
    const CalibDb_Gamma_t *calib_gamma = &calib->gamma;

    ret = AgammaConfigV200(calib_gamma, AgammaHandle);
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
    AgammaHandle_t * AgammaHandle = (AgammaHandle_t *)inparams->ctx;
    RkAiqAlgoProcAgammaInt* procPara = (RkAiqAlgoProcAgammaInt*)inparams;
    RkAiqAlgoProcResAgamma* procResPara = (RkAiqAlgoProcResAgamma*)outparams;
    rk_aiq_gamma_cfg_t* agamma_config;
    agamma_config = (rk_aiq_gamma_cfg_t*)&procResPara->agamma_config;
    const CalibDb_Gamma_t *calib_gamma = AgammaHandle->calib_gamma;

    if(inparams->u.proc.init == false) {
        ret = AgammaReConfigV200(AgammaHandle, calib_gamma);
    }
    memcpy(agamma_config, &AgammaHandle->agamma_config, sizeof(rk_aiq_gamma_cfg_t));
    return XCAM_RETURN_NO_ERROR;
}

static XCamReturn
post_process(const RkAiqAlgoCom* inparams, RkAiqAlgoResCom* outparams)
{
    return XCAM_RETURN_NO_ERROR;
}

RkAiqAlgoDescription g_RkIspAlgoDescAgamma = {
    .common = {
        .version = RKISP_ALGO_AGAMMA_VERSION,
        .vendor  = RKISP_ALGO_AGAMMA_VENDOR,
        .description = RKISP_ALGO_AGAMMA_DESCRIPTION,
        .type    = RK_AIQ_ALGO_TYPE_AGAMMA,
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
