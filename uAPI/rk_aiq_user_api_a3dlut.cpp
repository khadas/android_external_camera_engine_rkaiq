#include "rk_aiq_user_api_a3dlut.h"
#include "RkAiqHandleInt.h"

XCamReturn
rk_aiq_user_api_a3dlut_SetAttrib(rk_aiq_sys_ctx_t* sys_ctx, rk_aiq_lut3d_attrib_t attr)
{
    RkAiqA3dlutHandleInt* algo_handle =
        algoHandle<RkAiqA3dlutHandleInt>(sys_ctx, RK_AIQ_ALGO_TYPE_A3DLUT);

    if (algo_handle) {
        return algo_handle->setAttrib(attr);
    }

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
rk_aiq_user_api_a3dlut_GetAttrib(rk_aiq_sys_ctx_t* sys_ctx, rk_aiq_lut3d_attrib_t *attr)
{
    RkAiqA3dlutHandleInt* algo_handle =
        algoHandle<RkAiqA3dlutHandleInt>(sys_ctx, RK_AIQ_ALGO_TYPE_A3DLUT);

    if (algo_handle) {
        return algo_handle->getAttrib(attr);
    }

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
rk_aiq_user_api_a3dlut_Query3dlutInfo(rk_aiq_sys_ctx_t* sys_ctx, rk_aiq_lut3d_querry_info_t *lut3d_querry_info)
{
    RkAiqA3dlutHandleInt* algo_handle =
        algoHandle<RkAiqA3dlutHandleInt>(sys_ctx, RK_AIQ_ALGO_TYPE_A3DLUT);

    if (algo_handle) {
        return algo_handle->query3dlutInfo(lut3d_querry_info);
    }

    return XCAM_RETURN_NO_ERROR;
}

