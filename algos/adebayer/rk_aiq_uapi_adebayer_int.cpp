#include "rk_aiq_uapi_adebayer_int.h"
#include "rk_aiq_types_algo_adebayer_prvt.h"

XCamReturn
rk_aiq_uapi_adebayer_SetAttrib
(
    RkAiqAlgoContext* ctx,
    adebayer_attrib_t attr,
    bool need_sync
)
{
    if(ctx == NULL) {
        LOGE_ADEBAYER("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
        return XCAM_RETURN_ERROR_PARAM;
    }

    AdebayerContext_t* pAdebayerCtx = (AdebayerContext_t*)&ctx->adebayerCtx;
    pAdebayerCtx->full_param.clip_en = attr.clip_enable;
    pAdebayerCtx->full_param.filter_c_en = attr.chroma_chan_en;
    pAdebayerCtx->full_param.filter_g_en = attr.main_chan_en;
    pAdebayerCtx->full_param.thed0 = attr.thed0;
    pAdebayerCtx->full_param.thed1 = attr.thed1;
    pAdebayerCtx->full_param.dist_scale = attr.dist_scale;
    pAdebayerCtx->full_param.gain_offset = attr.gian_offset;
    pAdebayerCtx->full_param.offset = attr.offset;
    memcpy(pAdebayerCtx->full_param.hf_offset, attr.hf_offset, sizeof(attr.hf_offset));
    memcpy(pAdebayerCtx->full_param.sharp_strength, attr.sharp_strength, sizeof(attr.sharp_strength));
    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
rk_aiq_uapi_adebayer_GetAttrib
(
    RkAiqAlgoContext*  ctx,
    adebayer_attrib_t* attr
)
{
    if(ctx == NULL || attr == NULL) {
        LOGE_ADEBAYER("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
        return XCAM_RETURN_ERROR_PARAM;
    }

    AdebayerContext_t* pAdebayerCtx = (AdebayerContext_t*)&ctx->adebayerCtx;
    attr->clip_enable = pAdebayerCtx->full_param.clip_en;
    attr->chroma_chan_en = pAdebayerCtx->full_param.filter_c_en;
    attr->main_chan_en = pAdebayerCtx->full_param.filter_g_en;
    attr->thed0 = pAdebayerCtx->full_param.thed0;
    attr->thed1 = pAdebayerCtx->full_param.thed1;
    attr->dist_scale = pAdebayerCtx->full_param.dist_scale;
    attr->gian_offset = pAdebayerCtx->full_param.gain_offset;
    attr->offset = pAdebayerCtx->full_param.offset;
    memcpy(attr->hf_offset, pAdebayerCtx->full_param.hf_offset, sizeof(attr->hf_offset));
    memcpy(attr->sharp_strength, pAdebayerCtx->full_param.sharp_strength, sizeof(attr->sharp_strength));

    return XCAM_RETURN_NO_ERROR;
}

