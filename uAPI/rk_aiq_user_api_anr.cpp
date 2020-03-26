#include "rk_aiq_user_api_anr.h"
#include "RkAiqHandleInt.h"

XCamReturn
rk_aiq_user_api_anr_SetAttrib(rk_aiq_sys_ctx_t* sys_ctx, rk_aiq_nr_attrib_t *attr)
{
    RkAiqAnrHandleInt* algo_handle =
        algoHandle<RkAiqAnrHandleInt>(sys_ctx, RK_AIQ_ALGO_TYPE_ANR);

    if (algo_handle) {
        return algo_handle->setAttrib(attr);
    }

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
rk_aiq_user_api_anr_GetAttrib(rk_aiq_sys_ctx_t* sys_ctx, rk_aiq_nr_attrib_t *attr)
{
    RkAiqAnrHandleInt* algo_handle =
        algoHandle<RkAiqAnrHandleInt>(sys_ctx, RK_AIQ_ALGO_TYPE_ANR);

    if (algo_handle) {
        return algo_handle->getAttrib(attr);
    }

    return XCAM_RETURN_NO_ERROR;
}

