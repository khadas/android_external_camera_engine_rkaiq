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
#include "aldch/rk_aiq_types_aldch_algo_prvt.h"
#include "xcam_log.h"

#define EPSINON 0.0000001

RKAIQ_BEGIN_DECLARE


static XCamReturn
create_context(RkAiqAlgoContext **context, const AlgoCtxInstanceCfg* cfg)
{
    XCamReturn result = XCAM_RETURN_NO_ERROR;

    LOG1_AHDR("%s: (enter)\n", __FUNCTION__ );
    RkAiqAlgoContext *ctx = new RkAiqAlgoContext();
    if (ctx == NULL) {
        LOGE_ALDCH( "%s: create aldch context fail!\n", __FUNCTION__);
        return XCAM_RETURN_ERROR_MEM;
    }
    ctx->hLDCH = new LDCHContext_t;
    if (ctx->hLDCH == NULL) {
        LOGE_ALDCH( "%s: create aldch handle fail!\n", __FUNCTION__);
        return XCAM_RETURN_ERROR_MEM;
    }
    /* setup config */
    memset((void *)(ctx->hLDCH), 0, sizeof(LDCHContext_t) );

    /* return handle */
    *context = ctx;

#if GENMESH_ONLINE
    ctx->hLDCH->isAttribUpdated = false;
    ctx->hLDCH->aldchReadMeshThread = new RKAiqAldchThread(ctx->hLDCH);
    ctx->hLDCH->aldchReadMeshThread->triger_start();
    ctx->hLDCH->aldchReadMeshThread->start();
#endif
    return XCAM_RETURN_NO_ERROR;
}

static XCamReturn
destroy_context(RkAiqAlgoContext *context)
{
    LDCHHandle_t hLDCH = (LDCHHandle_t)context->hLDCH;
    LDCHContext_t* ldchCtx = (LDCHContext_t*)hLDCH;

#if GENMESH_ONLINE
    ldchCtx->aldchReadMeshThread->triger_stop();
    ldchCtx->aldchReadMeshThread->stop();
#endif
    if (ldchCtx->lut_mapxy != NULL) {
        free(ldchCtx->lut_mapxy);
        ldchCtx->lut_mapxy = NULL;
    }
#if GENMESH_ONLINE
	genLdchMeshDeInit(ldchCtx->ldchParams);
#endif
    delete context->hLDCH;
    context->hLDCH = NULL;
    delete context;
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
    memcpy(ldchCtx->meshfile, rkaiqAldchConfig->aldch_calib_cfg.meshfile, sizeof(ldchCtx->meshfile));
    ldchCtx->resource_path = rkaiqAldchConfig->resource_path;
#if GENMESH_ONLINE
    ldchCtx->camCoeff.cx = rkaiqAldchConfig->aldch_calib_cfg.light_center[0];
    ldchCtx->camCoeff.cy = rkaiqAldchConfig->aldch_calib_cfg.light_center[1];
    ldchCtx->camCoeff.a0 = rkaiqAldchConfig->aldch_calib_cfg.coefficient[0];
    ldchCtx->camCoeff.a2 = rkaiqAldchConfig->aldch_calib_cfg.coefficient[1];
    ldchCtx->camCoeff.a3 = rkaiqAldchConfig->aldch_calib_cfg.coefficient[2];
    ldchCtx->camCoeff.a4 = rkaiqAldchConfig->aldch_calib_cfg.coefficient[3];
    LOGI_ALDCH("(%s) len light center(%.16f, %.16f)\n",
            __FUNCTION__,
            ldchCtx->camCoeff.cx, ldchCtx->camCoeff.cy);
    LOGI_ALDCH("(%s) len coefficient(%.16f, %.16f, %.16f, %.16f)\n",
            __FUNCTION__,
            ldchCtx->camCoeff.a0, ldchCtx->camCoeff.a2,
            ldchCtx->camCoeff.a3, ldchCtx->camCoeff.a4);
#endif

    double correct_level = rkaiqAldchConfig->aldch_calib_cfg.correct_level;
    LOGI_ALDCH("ldch en %d, meshfile: %s, correct_level: %d-%f from xml file",
               rkaiqAldchConfig->aldch_calib_cfg.ldch_en,
               ldchCtx->meshfile,
               ldchCtx->correct_level,
               correct_level);

    ldchCtx->src_width = params->u.prepare.sns_op_width;
    ldchCtx->src_height = params->u.prepare.sns_op_height;
    ldchCtx->dst_width = params->u.prepare.sns_op_width;
    ldchCtx->dst_height = params->u.prepare.sns_op_height;
#if GENMESH_ONLINE
    if (ldchCtx->isAttribUpdated) {
        ldchCtx->ldch_en = ldchCtx->user_config.en;
        ldchCtx->correct_level = ldchCtx->user_config.correct_level;
        ldchCtx->isAttribUpdated = false;
    } else {
        ldchCtx->correct_level = correct_level;
        ldchCtx->user_config.en = ldchCtx->ldch_en;
        ldchCtx->user_config.correct_level = ldchCtx->correct_level;
    }

	genLdchMeshInit(ldchCtx->src_width, ldchCtx->src_height,
                    ldchCtx->dst_width, ldchCtx->dst_height,
                    ldchCtx->ldchParams, ldchCtx->camCoeff);

    ldchCtx->lut_h_size = (ldchCtx->ldchParams.meshSizeW + 1) / 2; //word unit
    ldchCtx->lut_v_size = ldchCtx->ldchParams.meshSizeH;
    ldchCtx->lut_mapxy_size = ldchCtx->ldchParams.meshSize * sizeof(unsigned short);

    // need re-alloc ?
    if (ldchCtx->lut_mapxy) {
        free(ldchCtx->lut_mapxy);
        ldchCtx->lut_mapxy = NULL;
    }
    ldchCtx->lut_mapxy = (unsigned short*)malloc(ldchCtx->lut_mapxy_size);

    LOGI_ALDCH("ldch en %d, h/v size(%dx%d), mapxy size(%d), correct_level: %d",
               ldchCtx->ldch_en,
               ldchCtx->lut_h_size,
               ldchCtx->lut_v_size,
               ldchCtx->lut_mapxy_size ,
               ldchCtx->correct_level);

    if (!ldchCtx->ldch_en)
        return XCAM_RETURN_NO_ERROR;

    bool success = genLDCMeshNLevel(ldchCtx->ldchParams, ldchCtx->camCoeff,
                                    ldchCtx->correct_level, ldchCtx->lut_mapxy);
    if (!success) {
        LOGW_ALDCH("lut is not exist");
        ldchCtx->ldch_en = 0;
    }
#else
    if (!ldchCtx->ldch_en)
        return XCAM_RETURN_NO_ERROR;

    if (fabs(correct_level) <= fabs(EPSINON)) {
        ldchCtx->correct_level = LDCH_BYPASS;
    } else if (correct_level >= 1.0) {
        ldchCtx->correct_level = LDCH_CORRECT_LEVEL0;
    } else if (1 - correct_level <= 0.25) {
        ldchCtx->correct_level = LDCH_CORRECT_LEVEL1;
    } else if (1 - correct_level <= 0.50) {
        ldchCtx->correct_level = LDCH_CORRECT_LEVEL2;
    } else if (1 - correct_level <= 0.75) {
        ldchCtx->correct_level = LDCH_CORRECT_LEVEL3;
    } else {
        ldchCtx->correct_level = LDCH_BYPASS;
    }

    FILE* ofp;
    char filename[512];
    sprintf(filename, "%s/%s/mesh_level%d.bin",
            ldchCtx->resource_path,
            ldchCtx->meshfile,
            ldchCtx->correct_level);
    ofp = fopen(filename, "rb");
    if (ofp != NULL) {
        unsigned short hpic, vpic, hsize, vsize, hstep, vstep = 0;

        fread(&hpic, sizeof(unsigned short), 1, ofp);
        fread(&vpic, sizeof(unsigned short), 1, ofp);
        fread(&hsize, sizeof(unsigned short), 1, ofp);
        fread(&vsize, sizeof(unsigned short), 1, ofp);
        fread(&hstep, sizeof(unsigned short), 1, ofp);
        fread(&vstep, sizeof(unsigned short), 1, ofp);
        //fseek(ofp, 0, SEEK_SET);
        LOGE_ALDCH("lut info: [%d-%d-%d-%d-%d-%d]", hpic, vpic, hsize, vsize, hstep, vstep);

        ldchCtx->lut_h_size = hsize;
        ldchCtx->lut_v_size = vsize;
        ldchCtx->lut_mapxy_size = ldchCtx->lut_h_size * ldchCtx->lut_v_size * sizeof(unsigned short);
        // need re-alloc ?
        if (ldchCtx->lut_mapxy) {
            free(ldchCtx->lut_mapxy);
            ldchCtx->lut_mapxy = NULL;
        }
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
#endif

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

    if (inparams->u.proc.init) {
        ldchPreOut->ldch_result.update = 1;
    } else {
        if (ldchCtx->isAttribUpdated) {
            ldchCtx->ldch_en = ldchCtx->user_config.en;
            ldchCtx->correct_level = ldchCtx->user_config.correct_level;
            ldchCtx->isAttribUpdated = false;
            ldchPreOut->ldch_result.update = 1;
        } else {
            ldchPreOut->ldch_result.update = 0;
        }

        LOGV_ALDCH("(%s) en(%d), level(%d), user en(%d), level(%d), result update(%d)\n",
                __FUNCTION__,
                ldchCtx->ldch_en,
                ldchCtx->correct_level,
                ldchCtx->user_config.en,
                ldchCtx->user_config.correct_level,
                ldchPreOut->ldch_result.update);
    }

    if (ldchPreOut->ldch_result.update) {
        ldchPreOut->ldch_result.sw_ldch_en = ldchCtx->ldch_en;
        ldchPreOut->ldch_result.lut_h_size = ldchCtx->lut_h_size;
        ldchPreOut->ldch_result.lut_v_size = ldchCtx->lut_v_size;
        ldchPreOut->ldch_result.lut_map_size = ldchCtx->lut_mapxy_size;
        if (ldchCtx->lut_mapxy != NULL && ldchCtx->ldch_en) {
            memcpy(ldchPreOut->ldch_result.lut_mapxy, ldchCtx->lut_mapxy,
                   ldchCtx->lut_mapxy_size);
        }
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


bool RKAiqAldchThread::loop()
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    ENTER_ANALYZER_FUNCTION();

    const static int32_t timeout = -1;
    SmartPtr<rk_aiq_ldch_cfg_t> attrib = mAttrQueue.pop (timeout);

    if (!attrib.ptr()) {
        LOGW_ANALYZER("RKAiqAldchThread got empty attrib, stop thread");
        return false;
    }

#if GENMESH_ONLINE
    if (attrib->en && (hLDCH->ldch_en != attrib->en || \
	    attrib->correct_level != hLDCH->correct_level)) {
        bool success = genLDCMeshNLevel(hLDCH->ldchParams, hLDCH->camCoeff,
                attrib->correct_level, hLDCH->lut_mapxy);
        if (!success)
            LOGW_ALDCH("lut is not exist");
    }
#endif
    if (ret == XCAM_RETURN_NO_ERROR) {
        hLDCH->isAttribUpdated = true;
        LOGV_ANALYZER("ldch en(%d), level(%d)\n", hLDCH->ldch_en, hLDCH->correct_level);
        return true;
    }

    LOGE_ANALYZER("RKAiqAldchThread failed to read mesh table!");

    EXIT_ANALYZER_FUNCTION();

    return false;
}
