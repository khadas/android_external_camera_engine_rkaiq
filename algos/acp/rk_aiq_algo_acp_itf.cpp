/*
 * rk_aiq_algo_acp_itf.c
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
#include "acp/rk_aiq_algo_acp_itf.h"
#include "xcam_log.h"
#include "acp/rk_aiq_types_algo_acp_prvt.h"
RKAIQ_BEGIN_DECLARE

static XCamReturn
create_context(RkAiqAlgoContext **context, const AlgoCtxInstanceCfg* cfg)
{
    const AlgoCtxInstanceCfgInt* cfg_int = (const AlgoCtxInstanceCfgInt*)cfg;
    RkAiqAlgoContext *ctx = new RkAiqAlgoContext();
    if (ctx == NULL) {
        LOGE_ACP( "%s: create acp context fail!\n", __FUNCTION__);
        return XCAM_RETURN_ERROR_MEM;
    }
    ctx->acpCtx.calib = cfg_int->calib;
    rk_aiq_acp_params_t* params = &ctx->acpCtx.params;
    params->brightness = 128;
    params->hue = 128;
    params->saturation = 128;
    params->contrast = 128;

    *context = ctx;

    return XCAM_RETURN_NO_ERROR;
}

static XCamReturn
destroy_context(RkAiqAlgoContext *context)
{
    delete context;
    return XCAM_RETURN_NO_ERROR;
}

static XCamReturn
prepare(RkAiqAlgoCom* params)
{
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
    RkAiqAlgoProcResAcp* res_com = (RkAiqAlgoProcResAcp*)outparams;
    RkAiqAlgoProcResAcpInt* res_int = (RkAiqAlgoProcResAcpInt*)outparams;
    RkAiqAlgoContext* ctx = inparams->ctx;

    res_com->acp_res = ctx->acpCtx.params;

    return XCAM_RETURN_NO_ERROR;
}

static XCamReturn
post_process(const RkAiqAlgoCom* inparams, RkAiqAlgoResCom* outparams)
{
    return XCAM_RETURN_NO_ERROR;
}

RkAiqAlgoDescription g_RkIspAlgoDescAcp = {
    .common = {
        .version = RKISP_ALGO_ACP_VERSION,
        .vendor  = RKISP_ALGO_ACP_VENDOR,
        .description = RKISP_ALGO_ACP_DESCRIPTION,
        .type    = RK_AIQ_ALGO_TYPE_ACP,
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
