#include "rk_aiq_uapi_agamma_int.h"
#include "agamma/rk_aiq_types_agamma_algo_prvt.h"

XCamReturn
rk_aiq_uapi_agamma_SetAttrib(RkAiqAlgoContext *ctx,
                             rk_aiq_gamma_attrib_t attr,
                             bool need_sync)
{
    AgammaHandle_t *gamma_handle = (AgammaHandle_t *)ctx;
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    gamma_handle->agamma_config.gamma_en = attr.gamma_en;
    gamma_handle->agamma_config.gamma_out_mode = attr.gamma_out_mode;
    gamma_handle->agamma_config.gamma_out_segnum = attr.gamma_out_segnum;
    gamma_handle->agamma_config.gamma_out_offset = attr.gamma_out_offset;
    if (attr.gamma_out_mode == GAMMA_OUT_NORMAL){
        memcpy(gamma_handle->normal_table, attr.gamma_table, sizeof(attr.gamma_table));
    }else if (attr.gamma_out_mode == GAMMA_OUT_HDR){
        memcpy(gamma_handle->hdr_table, attr.gamma_table, sizeof(attr.gamma_table));
    }else if (attr.gamma_out_mode == GAMMA_OUT_NIGHT){
        memcpy(gamma_handle->night_table, attr.gamma_table, sizeof(attr.gamma_table));
    }else if (attr.gamma_out_mode == GAMMA_OUT_USR1){
        memcpy(gamma_handle->user1_table, attr.gamma_table, sizeof(attr.gamma_table));
    }else if (attr.gamma_out_mode == GAMMA_OUT_USR2){
        memcpy(gamma_handle->user2_table, attr.gamma_table, sizeof(attr.gamma_table));
    }else {
        LOGE_AGAMMA("%s(%d): gammaout mode is not supported!\n", __FUNCTION__, __LINE__);
        ret = XCAM_RETURN_ERROR_PARAM;
    }
    return ret;
}

XCamReturn
rk_aiq_uapi_agamma_GetAttrib(const RkAiqAlgoContext *ctx,
                             rk_aiq_gamma_attrib_t *attr)
{

    AgammaHandle_t* gamma_handle = (AgammaHandle_t*)ctx;

    memcpy(attr, &gamma_handle->agamma_config, sizeof(rk_aiq_gamma_attrib_t));

    return XCAM_RETURN_NO_ERROR;
}




