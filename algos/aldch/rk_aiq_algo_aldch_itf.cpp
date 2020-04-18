/*
 * rk_aiq_algo_aldch_itf.c
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
#include "aldch/rk_aiq_algo_aldch_itf.h"
#include "xcam_log.h"

RKAIQ_BEGIN_DECLARE

typedef struct LDCHContext_s {
    unsigned char initialized;
    unsigned int ldch_en;
    unsigned int lut_h_size;
    unsigned int lut_v_size;
    unsigned int lut_mapxy_size;
    unsigned short* lut_mapxy;
} LDCHContext_t;

typedef struct LDCHContext_s* LDCHHandle_t;
LDCHContext_t gLDCHCtx;

typedef struct _RkAiqAlgoContext {
    LDCHHandle_t hLDCH;
    void* place_holder[0];
} RkAiqAlgoContext;

static RkAiqAlgoContext ctx;

static XCamReturn
create_context(RkAiqAlgoContext **context, const AlgoCtxInstanceCfg* cfg)
{
    XCamReturn result = XCAM_RETURN_NO_ERROR;

    LOGI_AHDR("%s: (enter)\n", __FUNCTION__ );

    /* setup config */
    memset( &gLDCHCtx, 0, sizeof(gLDCHCtx) );
    ctx.hLDCH = &gLDCHCtx;

    /* return handle */
    *context = &ctx;

    return XCAM_RETURN_NO_ERROR;
}

static XCamReturn
destroy_context(RkAiqAlgoContext *context)
{
    return XCAM_RETURN_NO_ERROR;
}

static XCamReturn
prepare(RkAiqAlgoCom* params)
{
    LDCHHandle_t hLDCH = (LDCHHandle_t)params->ctx->hLDCH;
    LDCHContext_t* ldchCtx = (LDCHContext_t*)hLDCH;
    RkAiqAlgoConfigAldchInt* rkaiqAldchConfig = (RkAiqAlgoConfigAldchInt*)params;

    ldchCtx->ldch_en = rkaiqAldchConfig->aldch_calib_cfg.ldch_en;
    ldchCtx->lut_h_size = rkaiqAldchConfig->aldch_calib_cfg.lut_h_size;
    ldchCtx->lut_v_size = rkaiqAldchConfig->aldch_calib_cfg.lut_v_size;
    ldchCtx->lut_mapxy_size = rkaiqAldchConfig->aldch_calib_cfg.lut_mapxy_size;
    memcpy(ldchCtx->lut_mapxy, rkaiqAldchConfig->aldch_calib_cfg.lut_mapxy,
        ldchCtx->lut_mapxy_size*sizeof(unsigned short));

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
    LDCHHandle_t hLDCH = (LDCHHandle_t)inparams->ctx->hLDCH;
    LDCHContext_t* ldchCtx = (LDCHContext_t*)hLDCH;

    RkAiqAlgoProcResAldchInt* ldchPreOut = (RkAiqAlgoProcResAldchInt*)outparams;
    ldchPreOut->sw_ldch_en = ldchCtx->ldch_en;
    ldchPreOut->lut_h_size = ldchCtx->lut_h_size;
    ldchPreOut->lut_v_size = ldchCtx->lut_v_size;
    ldchPreOut->lut_mapxy_size = ldchCtx->lut_mapxy_size;
    memcpy(ldchPreOut->lut_mapxy, ldchCtx->lut_mapxy,
        ldchCtx->lut_mapxy_size*sizeof(unsigned short));

    return XCAM_RETURN_NO_ERROR;
}

static XCamReturn
post_process(const RkAiqAlgoCom* inparams, RkAiqAlgoResCom* outparams)
{
    return XCAM_RETURN_NO_ERROR;
}

RkAiqAlgoDescription g_RkIspAlgoDescAldch = {
    .common = {
        .version = RKISP_ALGO_ALDCH_VERSION,
        .vendor  = RKISP_ALGO_ALDCH_VENDOR,
        .description = RKISP_ALGO_ALDCH_DESCRIPTION,
        .type    = RK_AIQ_ALGO_TYPE_ALDCH,
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
