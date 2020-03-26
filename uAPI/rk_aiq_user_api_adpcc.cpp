#include "rk_aiq_user_api_adpcc.h"
#include "RkAiqHandleInt.h"

XCamReturn
rk_aiq_user_api_adpcc_SetAttrib(rk_aiq_sys_ctx_t* sys_ctx, rk_aiq_dpcc_attrib_t *attr)
{
    RkAiqAdpccHandleInt* algo_handle =
        algoHandle<RkAiqAdpccHandleInt>(sys_ctx, RK_AIQ_ALGO_TYPE_ADPCC);

    if (algo_handle) {
        return algo_handle->setAttrib(attr);
    }

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
rk_aiq_user_api_adpcc_GetAttrib(rk_aiq_sys_ctx_t* sys_ctx, rk_aiq_dpcc_attrib_t *attr)
{
    RkAiqAdpccHandleInt* algo_handle =
        algoHandle<RkAiqAdpccHandleInt>(sys_ctx, RK_AIQ_ALGO_TYPE_ADPCC);

    if (algo_handle) {
        return algo_handle->getAttrib(attr);
    }

    return XCAM_RETURN_NO_ERROR;
}

