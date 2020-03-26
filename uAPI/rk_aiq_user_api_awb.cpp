#include "rk_aiq_user_api_awb.h"
#include "RkAiqHandleInt.h"

XCamReturn
rk_aiq_user_api_awb_SetAttrib(rk_aiq_sys_ctx_t* sys_ctx, rk_aiq_wb_attrib_t attr)
{
    RkAiqAwbHandleInt* algo_handle =
        algoHandle<RkAiqAwbHandleInt>(sys_ctx, RK_AIQ_ALGO_TYPE_AWB);

    if (algo_handle) {
        return algo_handle->setAttrib(attr);
    }

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
rk_aiq_user_api_awb_GetAttrib(rk_aiq_sys_ctx_t* sys_ctx, rk_aiq_wb_attrib_t *attr)
{
    RkAiqAwbHandleInt* algo_handle =
        algoHandle<RkAiqAwbHandleInt>(sys_ctx, RK_AIQ_ALGO_TYPE_AWB);

    if (algo_handle) {
        return algo_handle->getAttrib(attr);
    }

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
rk_aiq_user_api_awb_GetCCT(rk_aiq_sys_ctx_t* sys_ctx, rk_aiq_wb_cct_t *cct)
{
    RkAiqAwbHandleInt* algo_handle =
        algoHandle<RkAiqAwbHandleInt>(sys_ctx, RK_AIQ_ALGO_TYPE_AWB);

    if (algo_handle) {
        return algo_handle->getCct(cct);
    }

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
rk_aiq_user_api_awb_QueryWBInfo(rk_aiq_sys_ctx_t* sys_ctx, rk_aiq_wb_querry_info_t *wb_querry_info)
{
    RkAiqAwbHandleInt* algo_handle =
        algoHandle<RkAiqAwbHandleInt>(sys_ctx, RK_AIQ_ALGO_TYPE_AWB);

    if (algo_handle) {
        return algo_handle->queryWBInfo(wb_querry_info);
    }

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
rk_aiq_user_api_awb_Lock(rk_aiq_sys_ctx_t* sys_ctx)
{
    RkAiqAwbHandleInt* algo_handle =
        algoHandle<RkAiqAwbHandleInt>(sys_ctx, RK_AIQ_ALGO_TYPE_AWB);

    if (algo_handle) {
        return algo_handle->lock();
    }

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
rk_aiq_user_api_awb_Unlock(rk_aiq_sys_ctx_t* sys_ctx)
{
    RkAiqAwbHandleInt* algo_handle =
        algoHandle<RkAiqAwbHandleInt>(sys_ctx, RK_AIQ_ALGO_TYPE_AWB);

    if (algo_handle) {
        return algo_handle->unlock();
    }

    return XCAM_RETURN_NO_ERROR;
}


