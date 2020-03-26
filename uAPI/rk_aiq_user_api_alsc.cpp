#include "rk_aiq_user_api_alsc.h"
#include "RkAiqHandleInt.h"

XCamReturn
rk_aiq_user_api_alsc_SetAttrib(rk_aiq_sys_ctx_t* sys_ctx, rk_aiq_lsc_attrib_t attr)
{
    RkAiqAlscHandleInt* algo_handle =
        algoHandle<RkAiqAlscHandleInt>(sys_ctx, RK_AIQ_ALGO_TYPE_ALSC);

    if (algo_handle) {
        return algo_handle->setAttrib(attr);
    }

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
rk_aiq_user_api_alsc_GetAttrib(rk_aiq_sys_ctx_t* sys_ctx, rk_aiq_lsc_attrib_t *attr)
{
    RkAiqAlscHandleInt* algo_handle =
        algoHandle<RkAiqAlscHandleInt>(sys_ctx, RK_AIQ_ALGO_TYPE_ALSC);

    if (algo_handle) {
        return algo_handle->getAttrib(attr);
    }

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
rk_aiq_user_api_alsc_QueryLscInfo(rk_aiq_sys_ctx_t* sys_ctx, rk_aiq_lsc_querry_info_t *lsc_querry_info)
{
    RkAiqAlscHandleInt* algo_handle =
        algoHandle<RkAiqAlscHandleInt>(sys_ctx, RK_AIQ_ALGO_TYPE_ALSC);

    if (algo_handle) {
        return algo_handle->queryLscInfo(lsc_querry_info);
    }

    return XCAM_RETURN_NO_ERROR;
}

