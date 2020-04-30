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
    unsigned int pic_width;
    unsigned int pic_height;
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
    LDCHHandle_t hLDCH = (LDCHHandle_t)context->hLDCH;
    LDCHContext_t* ldchCtx = (LDCHContext_t*)hLDCH;

    if (ldchCtx->lut_mapxy != NULL) {
        free(ldchCtx->lut_mapxy);
        ldchCtx->lut_mapxy = NULL;
    }
    context = NULL;
    return XCAM_RETURN_NO_ERROR;
}

static XCamReturn
prepare(RkAiqAlgoCom* params)
{
    LDCHHandle_t hLDCH = (LDCHHandle_t)params->ctx->hLDCH;
    LDCHContext_t* ldchCtx = (LDCHContext_t*)hLDCH;
    RkAiqAlgoConfigAldchInt* rkaiqAldchConfig = (RkAiqAlgoConfigAldchInt*)params;

    ldchCtx->ldch_en = rkaiqAldchConfig->aldch_calib_cfg.ldch_en;
    LOGI_ALDCH("ldch en from xml file: %d", rkaiqAldchConfig->aldch_calib_cfg.ldch_en);

    ldchCtx->pic_width = params->u.prepare.sns_op_width;
    ldchCtx->pic_height = params->u.prepare.sns_op_height;

    FILE* ofp;
    ofp = fopen("/userdata/meshxy.bin", "rb");
    if (ofp != NULL) {
        unsigned short hpic, vpic, hsize, vsize, hstep, vstep = 0;

        fread(&hpic, sizeof(unsigned short), 1, ofp);
        fread(&vpic, sizeof(unsigned short), 1, ofp);
        fread(&hsize, sizeof(unsigned short), 1, ofp);
        fread(&vsize, sizeof(unsigned short), 1, ofp);
        fread(&hstep, sizeof(unsigned short), 1, ofp);
        fread(&vstep, sizeof(unsigned short), 1, ofp);
        //fseek(ofp, 0, SEEK_SET);
        LOGI_ALDCH("lut info: [%d-%d-%d-%d-%d-%d]", hpic, vpic, hsize, vsize, hstep, vstep);

        ldchCtx->lut_h_size = hsize;
        ldchCtx->lut_v_size = vsize;
        ldchCtx->lut_mapxy_size = ldchCtx->lut_h_size * ldchCtx->lut_v_size * sizeof(unsigned short);
        ldchCtx->lut_mapxy = (unsigned short*)malloc(ldchCtx->lut_mapxy_size);
        ldchCtx->lut_h_size = hsize / 2; //word unit

        unsigned int num = fread(ldchCtx->lut_mapxy, 1, ldchCtx->lut_mapxy_size, ofp);
        fclose(ofp);

        if (num != ldchCtx->lut_mapxy_size) {
            ldchCtx->ldch_en = 0;
            LOGE_ALDCH("mismatched lut calib file");
        }
        LOGE_ALDCH("check calib file, size: %d, num: %d", ldchCtx->lut_mapxy_size, num);
    } else {
        LOGW_ALDCH("lut calib file not exist");
        ldchCtx->ldch_en = 0;
    }

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
    ldchPreOut->ldch_result.sw_ldch_en = ldchCtx->ldch_en;
    // TODO: add update flag for ldch
    if (ldchCtx->ldch_en) {
        ldchPreOut->ldch_result.lut_h_size = ldchCtx->lut_h_size;
        ldchPreOut->ldch_result.lut_v_size = ldchCtx->lut_v_size;
        ldchPreOut->ldch_result.lut_map_size = ldchCtx->lut_mapxy_size;
        if (ldchCtx->lut_mapxy != NULL) {
            memcpy(ldchPreOut->ldch_result.lut_mapxy, ldchCtx->lut_mapxy,
                ldchCtx->lut_mapxy_size);
        }
        ldchCtx->ldch_en = 0;
    }

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
