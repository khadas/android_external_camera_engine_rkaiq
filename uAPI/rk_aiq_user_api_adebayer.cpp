#include "rk_aiq_user_api_adebayer.h"
#include "RkAiqHandleInt.h"

XCamReturn
rk_aiq_user_api_adebayer_SetAttrib(rk_aiq_sys_ctx_t* sys_ctx, adebayer_attrib_t attr)
{
    RkAiqAdebayerHandleInt* algo_handle =
        algoHandle<RkAiqAdebayerHandleInt>(sys_ctx, RK_AIQ_ALGO_TYPE_ADEBAYER);

    if (algo_handle) {
        return algo_handle->setAttrib(attr);
    }

    return XCAM_RETURN_NO_ERROR;
}
