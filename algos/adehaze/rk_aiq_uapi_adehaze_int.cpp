#include "rk_aiq_uapi_adehaze_int.h"
#include "rk_aiq_types_adehaze_algo_prvt.h"
#include "xcam_log.h"

XCamReturn
rk_aiq_uapi_adehaze_SetAttrib(RkAiqAlgoContext *ctx,
                              adehaze_sw_t attr,
                              bool need_sync)
{
    AdehazeHandle_t * AdehazeHandle = (AdehazeHandle_t *)ctx;
    LOGD_ADEHAZE("%s: setMode:%d", __func__, attr.mode);
    AdehazeHandle->AdehazeAtrr.byPass = attr.byPass;
    AdehazeHandle->AdehazeAtrr.mode = attr.mode;
    if(attr.mode == RK_AIQ_DEHAZE_MODE_AUTO)
        memcpy(&AdehazeHandle->AdehazeAtrr.stAuto, &attr.stAuto, sizeof(rk_aiq_dehaze_A_attrib_t));
    else if(attr.mode == RK_AIQ_DEHAZE_MODE_MANUAL)
        memcpy(&AdehazeHandle->AdehazeAtrr.stManual, &attr.stManual, sizeof(rk_aiq_dehaze_M_attrib_t));
    else if(attr.mode == RK_AIQ_DEHAZE_MODE_OFF)
        memcpy(&AdehazeHandle->AdehazeAtrr.stEnhance, &attr.stEnhance, sizeof(rk_aiq_dehaze_enhance_t));
    else {
        LOGE_ADEHAZE("invalid mode!");
    }
    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
rk_aiq_uapi_adehaze_GetAttrib(RkAiqAlgoContext *ctx, adehaze_sw_t *attr)
{
    AdehazeHandle_t * AdehazeHandle = (AdehazeHandle_t *)ctx;
    attr->mode = AdehazeHandle->AdehazeAtrr.mode;
    if (AdehazeHandle->AdehazeAtrr.mode == RK_AIQ_DEHAZE_MODE_AUTO) {
        if (AdehazeHandle->Dehaze_Scene_mode == 0) {
        } else if (AdehazeHandle->Dehaze_Scene_mode == 1) {
        } else if (AdehazeHandle->Dehaze_Scene_mode == 2) {
        }
    } else if (AdehazeHandle->AdehazeAtrr.mode == RK_AIQ_DEHAZE_MODE_MANUAL) {
        attr->stManual.strength = AdehazeHandle->strength;
        if (AdehazeHandle->Dehaze_Scene_mode == 0) {
        } else if (AdehazeHandle->Dehaze_Scene_mode == 1) {
        } else if (AdehazeHandle->Dehaze_Scene_mode == 2) {
        }
    } else if (AdehazeHandle->AdehazeAtrr.mode == RK_AIQ_DEHAZE_MODE_OFF) {
        if (AdehazeHandle->Dehaze_Scene_mode == 0) {
        } else if (AdehazeHandle->Dehaze_Scene_mode == 1) {
        } else if (AdehazeHandle->Dehaze_Scene_mode == 2) {
        }
    }
    return XCAM_RETURN_NO_ERROR;
}

