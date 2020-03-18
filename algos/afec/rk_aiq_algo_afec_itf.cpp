/*
 * rk_aiq_algo_afec_itf.c
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
#include "afec/rk_aiq_algo_afec_itf.h"

RKAIQ_BEGIN_DECLARE

typedef struct FECContext_s {
    unsigned char initialized;
    unsigned int fec_en;
    unsigned int mesh_density; //0:16x8 1:32x16
    unsigned int fec_mesh_h_size;
    unsigned int fec_mesh_v_size;
    unsigned int fec_mesh_size;
    unsigned int pic_width;
    unsigned int pic_height;
    unsigned int sw_rd_vir_stride;
    unsigned int sw_wr_yuv_format; //0:YUV420 1:YUV422
    unsigned int sw_wr_vir_stride;
    unsigned int sw_fec_wr_fbce_mode; //0:normal 1:fbec
    unsigned short* meshxi;
    unsigned char* meshxf;
    unsigned short* meshyi;
    unsigned char* meshyf;
    float k1;
    float k2;
    float p1;
    float p2;
    float k3;
} FECContext_t;

typedef struct FECContext_s* FECHandle_t;
static FECContext_t gFECCtx;

typedef struct _RkAiqAlgoContext {
    FECHandle_t hFEC;
    void* place_holder[0];
} RkAiqAlgoContext;

static RkAiqAlgoContext ctx;

static XCamReturn
gen_mesh_table(
        float k1, float k2, float p1, float p2, float k3,
        int pic_width, int pic_height,
        int mesh_h_size, int mesh_v_size,
        int* meshxi, int* meshxf, int* meshyi, int* meshyf) {
    //TODO::implement mesh table generation function
    return XCAM_RETURN_NO_ERROR;
}

static XCamReturn
create_context(RkAiqAlgoContext **context, const AlgoCtxInstanceCfg* cfg)
{
    ctx.hFEC = &gFECCtx;
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
    FECHandle_t hFEC = (FECHandle_t)params->ctx->hFEC;
    FECContext_t* fecCtx = (FECContext_t*)hFEC;
    RkAiqAlgoConfigAfecInt* rkaiqAfecConfig = (RkAiqAlgoConfigAfecInt*)params;
    int mesh_block_h_size, mesh_block_v_size;

    fecCtx->fec_en = rkaiqAfecConfig->afec_calib_cfg.fec_en;
    fecCtx->mesh_density = rkaiqAfecConfig->afec_calib_cfg.mesh_density;

    fecCtx->sw_wr_yuv_format = rkaiqAfecConfig->afec_calib_cfg.yuv_format;
    fecCtx->sw_fec_wr_fbce_mode = rkaiqAfecConfig->afec_calib_cfg.fbce_mode;

    if (fecCtx->mesh_density == 0) {
        mesh_block_h_size = 16;
        mesh_block_v_size = 8;
    } else {
        mesh_block_h_size = 32;
        mesh_block_v_size = 16;
    }

    fecCtx->pic_width = rkaiqAfecConfig->output_width;
    fecCtx->pic_height = rkaiqAfecConfig->output_height;

    fecCtx->fec_mesh_h_size = rkaiqAfecConfig->afec_calib_cfg.fec_mesh_h_size;
    fecCtx->fec_mesh_v_size = rkaiqAfecConfig->afec_calib_cfg.fec_mesh_v_size;
    fecCtx->fec_mesh_size = fecCtx->fec_mesh_h_size * fecCtx->fec_mesh_v_size;

    fecCtx->sw_rd_vir_stride = 16*((fecCtx->pic_width + 15)/16);
    fecCtx->sw_wr_vir_stride = 16*((fecCtx->pic_width + 15)/16);

    memcpy(fecCtx->meshxi, rkaiqAfecConfig->afec_calib_cfg.meshxi,
        fecCtx->fec_mesh_size*sizeof(unsigned short));
    memcpy(fecCtx->meshxi, rkaiqAfecConfig->afec_calib_cfg.meshxf,
        fecCtx->fec_mesh_size*sizeof(unsigned char));
    memcpy(fecCtx->meshxi, rkaiqAfecConfig->afec_calib_cfg.meshyi,
        fecCtx->fec_mesh_size*sizeof(unsigned short));
    memcpy(fecCtx->meshxi, rkaiqAfecConfig->afec_calib_cfg.meshyi,
        fecCtx->fec_mesh_size*sizeof(unsigned char));
    /*
    gen_mesh_table(fecCtx->k1, fecCtx->k2, fecCtx->p1, fecCtx->p2, fecCtx->k3,
        fecCtx->pic_width, fecCtx->pic_height,
        fecCtx->meshxi, fecCtx->meshxf,
        fecCtx->meshyi, fecCtx->meshyf);
    */
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
    FECHandle_t hFEC = (FECHandle_t)inparams->ctx->hFEC;
    FECContext_t* fecCtx = (FECContext_t*)hFEC;
    RkAiqAlgoProcAfecInt* fecProcParams = (RkAiqAlgoProcAfecInt*)inparams;

    RkAiqAlgoProcResAfecInt* fecPreOut = (RkAiqAlgoProcResAfecInt*)outparams;
    fecPreOut->sw_fec_en = fecCtx->fec_en;
    fecPreOut->pic_width = fecProcParams->output_width;
    fecPreOut->pic_height = fecProcParams->output_height;
    fecPreOut->sw_rd_vir_stride = fecCtx->sw_rd_vir_stride;
    fecPreOut->sw_wr_vir_stride = fecCtx->sw_wr_vir_stride;
    fecPreOut->sw_wr_yuv_format = fecCtx->sw_wr_yuv_format;
    memcpy(fecPreOut->meshxi, fecCtx->meshxi,
        fecCtx->fec_mesh_size*sizeof(unsigned short));
    memcpy(fecPreOut->meshxf, fecCtx->meshxf,
        fecCtx->fec_mesh_size*sizeof(unsigned char));
    memcpy(fecPreOut->meshyi, fecCtx->meshyi,
        fecCtx->fec_mesh_size*sizeof(unsigned short));
    memcpy(fecPreOut->meshyf, fecCtx->meshyf,
        fecCtx->fec_mesh_size*sizeof(unsigned char));

    return XCAM_RETURN_NO_ERROR;
}

static XCamReturn
post_process(const RkAiqAlgoCom* inparams, RkAiqAlgoResCom* outparams)
{
    return XCAM_RETURN_NO_ERROR;
}

RkAiqAlgoDescription g_RkIspAlgoDescAfec = {
    .common = {
        .version = RKISP_ALGO_AFEC_VERSION,
        .vendor  = RKISP_ALGO_AFEC_VENDOR,
        .description = RKISP_ALGO_AFEC_DESCRIPTION,
        .type    = RK_AIQ_ALGO_TYPE_AFEC,
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
