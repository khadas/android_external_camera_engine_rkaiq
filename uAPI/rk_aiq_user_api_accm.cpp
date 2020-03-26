#include "rk_aiq_user_api_accm.h"
#include "RkAiqHandleInt.h"

XCamReturn
rk_aiq_user_api_accm_SetAttrib(rk_aiq_sys_ctx_t* sys_ctx, rk_aiq_ccm_attrib_t attr)
{
    RkAiqAccmHandleInt* algo_handle =
        algoHandle<RkAiqAccmHandleInt>(sys_ctx, RK_AIQ_ALGO_TYPE_ACCM);

    if (algo_handle) {
        return algo_handle->setAttrib(attr);
    }

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
rk_aiq_user_api_accm_GetAttrib(rk_aiq_sys_ctx_t* sys_ctx, rk_aiq_ccm_attrib_t *attr)
{
    RkAiqAccmHandleInt* algo_handle =
        algoHandle<RkAiqAccmHandleInt>(sys_ctx, RK_AIQ_ALGO_TYPE_ACCM);

    if (algo_handle) {
        return algo_handle->getAttrib(attr);
    }

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
rk_aiq_user_api_accm_QueryCcmInfo(rk_aiq_sys_ctx_t* sys_ctx, rk_aiq_ccm_querry_info_t *ccm_querry_info)
{
    RkAiqAccmHandleInt* algo_handle =
        algoHandle<RkAiqAccmHandleInt>(sys_ctx, RK_AIQ_ALGO_TYPE_ACCM);

    if (algo_handle) {
        return algo_handle->queryCcmInfo(ccm_querry_info);
    }

    return XCAM_RETURN_NO_ERROR;
}

