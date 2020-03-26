#include "rk_aiq_user_api_ablc.h"
#include "RkAiqHandleInt.h"

XCamReturn
rk_aiq_user_api_alsc_SetAttrib(rk_aiq_sys_ctx_t* sys_ctx, rk_aiq_blc_attrib_t *attr)
{
    RkAiqAblcHandleInt* algo_handle =
        algoHandle<RkAiqAblcHandleInt>(sys_ctx, RK_AIQ_ALGO_TYPE_ABLC);

    if (algo_handle) {
        return algo_handle->setAttrib(attr);
    }

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
rk_aiq_user_api_alsc_GetAttrib(rk_aiq_sys_ctx_t* sys_ctx, rk_aiq_blc_attrib_t *attr)
{
    RkAiqAblcHandleInt* algo_handle =
        algoHandle<RkAiqAblcHandleInt>(sys_ctx, RK_AIQ_ALGO_TYPE_ABLC);

    if (algo_handle) {
        return algo_handle->getAttrib(attr);
    }

    return XCAM_RETURN_NO_ERROR;
}



