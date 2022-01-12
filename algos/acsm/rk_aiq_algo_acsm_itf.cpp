/*
 * rk_aiq_algo_acsm_itf.c
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
#include "acsm/rk_aiq_algo_acsm_itf.h"
#include "rk_aiq_types_algo_acsm_prvt.h"
#include "xcam_log.h"
RKAIQ_BEGIN_DECLARE

static rk_aiq_acsm_params_t g_csm_def = {
    .op_mode = RK_AIQ_OP_MODE_AUTO,
    .full_range = true,
    .y_offset = 0,
    .c_offset = 0,
    .coeff = {
            0x021, 0x040, 0x00d,
            0x1ed, 0x1db, 0x038,
            0x038, 0x1d1, 0x1f7
    }
};

static XCamReturn
create_context(RkAiqAlgoContext **context, const AlgoCtxInstanceCfg* cfg)
{
    RkAiqAlgoContext *ctx = new RkAiqAlgoContext();
    if (ctx == NULL) {
        LOGE_ACSM( "%s: create acsm context fail!\n", __FUNCTION__);
        return XCAM_RETURN_ERROR_MEM;
    }

    rk_aiq_acsm_params_t* params = &ctx->acsmCtx.params;
    memset(params, 0, sizeof(*params));
    if (ctx->acsmCtx.calibv2) {
        Csm_Param_t* csm =
            (Csm_Param_t*)(CALIBDBV2_GET_MODULE_PTR(ctx->acsmCtx.calibv2, csm));
        if (csm) *params = *csm;
    } else {
        // auto means using chip reset valuse
        *params = g_csm_def;
    }

    *context = ctx;
    return XCAM_RETURN_NO_ERROR;
}

static XCamReturn
destroy_context(RkAiqAlgoContext *context)
{
    delete context;
    context = NULL;
    return XCAM_RETURN_NO_ERROR;
}

static XCamReturn
prepare(RkAiqAlgoCom* params)
{
    rk_aiq_acsm_params_t* acsm_params = &params->ctx->acsmCtx.params;
    RkAiqAlgoConfigAcsm* pCfgParam = (RkAiqAlgoConfigAcsm*)params;

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
    RkAiqAlgoProcResAcsm* res_com = (RkAiqAlgoProcResAcsm*)outparams;
    RkAiqAlgoContext* ctx = inparams->ctx;

    if (ctx->acsmCtx.params.op_mode == RK_AIQ_OP_MODE_AUTO) {
        ctx->acsmCtx.params = g_csm_def;
    }
    res_com->acsm_res = ctx->acsmCtx.params;

    return XCAM_RETURN_NO_ERROR;
}

static XCamReturn
post_process(const RkAiqAlgoCom* inparams, RkAiqAlgoResCom* outparams)
{
    return XCAM_RETURN_NO_ERROR;
}

RkAiqAlgoDescription g_RkIspAlgoDescAcsm = {
    .common = {
        .version = RKISP_ALGO_ACSM_VERSION,
        .vendor  = RKISP_ALGO_ACSM_VENDOR,
        .description = RKISP_ALGO_ACSM_DESCRIPTION,
        .type    = RK_AIQ_ALGO_TYPE_ACSM,
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
