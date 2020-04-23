#include "rk_aiq_user_api_agic.h"
#include "RkAiqHandleInt.h"

XCamReturn
rk_aiq_user_api_agic_SetAttrib(rk_aiq_sys_ctx_t* sys_ctx, agic_attrib_t attr)
{
    RkAiqAgicHandleInt* algo_handle =
        algoHandle<RkAiqAgicHandleInt>(sys_ctx, RK_AIQ_ALGO_TYPE_AGIC);

    if (algo_handle) {
        return algo_handle->setAttrib(attr);
    }

    return XCAM_RETURN_NO_ERROR;
}
