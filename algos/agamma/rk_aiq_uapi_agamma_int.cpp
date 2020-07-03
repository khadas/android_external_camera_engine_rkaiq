#include "rk_aiq_uapi_agamma_int.h"
#include "agamma/rk_aiq_types_agamma_algo_prvt.h"

XCamReturn
rk_aiq_uapi_agamma_SetAttrib(RkAiqAlgoContext *ctx,
                             rk_aiq_gamma_attrib_t attr,
                             bool need_sync)
{
    AgammaHandle_t *gamma_handle = (AgammaHandle_t *)ctx;

    memcpy(&gamma_handle->agamma_config, &attr, sizeof(rk_aiq_gamma_attrib_t));
    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
rk_aiq_uapi_agamma_GetAttrib(const RkAiqAlgoContext *ctx,
                             rk_aiq_gamma_attrib_t *attr)
{

    AgammaHandle_t* gamma_handle = (AgammaHandle_t*)ctx;

    memcpy(attr, &gamma_handle->agamma_config, sizeof(rk_aiq_gamma_attrib_t));

    return XCAM_RETURN_NO_ERROR;
}




