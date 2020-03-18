#include "rk_aiq_user_api_ahdr.h"
#include "RkAiqHandleInt.h"

XCamReturn
rk_aiq_user_api_ahdr_SetAttrib(rk_aiq_sys_ctx_t* sys_ctx, ahdr_attrib_t attr)
{
    RkAiqAhdrHandleInt* algo_handle =
        algoHandle<RkAiqAhdrHandleInt>(sys_ctx, RK_AIQ_ALGO_TYPE_AHDR);

    if (algo_handle) {
        return algo_handle->setAttrib(attr);
    }

    return XCAM_RETURN_NO_ERROR;
}
