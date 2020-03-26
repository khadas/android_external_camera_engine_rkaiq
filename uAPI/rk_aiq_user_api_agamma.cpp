#include "rk_aiq_user_api_agamma.h"
#include "RkAiqHandleInt.h"

XCamReturn
rk_aiq_user_api_agamma_SetAttrib(rk_aiq_sys_ctx_t* sys_ctx, rk_aiq_gamma_attrib_t attr)
{
    RkAiqAgammaHandleInt* algo_handle =
        algoHandle<RkAiqAgammaHandleInt>(sys_ctx, RK_AIQ_ALGO_TYPE_AGAMMA);

    if (algo_handle) {
        return algo_handle->setAttrib(attr);
    }

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
rk_aiq_user_api_agamma_GetAttrib(rk_aiq_sys_ctx_t* sys_ctx, rk_aiq_gamma_attrib_t *attr)
{
    RkAiqAgammaHandleInt* algo_handle =
        algoHandle<RkAiqAgammaHandleInt>(sys_ctx, RK_AIQ_ALGO_TYPE_AGAMMA);

    if (algo_handle) {
        return algo_handle->getAttrib(attr);
    }

    return XCAM_RETURN_NO_ERROR;
}
