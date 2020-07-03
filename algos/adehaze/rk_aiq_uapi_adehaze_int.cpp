#include "rk_aiq_uapi_adehaze_int.h"
#include "rk_aiq_types_adehaze_algo_prvt.h"
#include "xcam_log.h"

XCamReturn
rk_aiq_uapi_adehaze_SetAttrib(RkAiqAlgoContext *ctx,
                              adehaze_sw_t attr,
                              bool need_sync)
{
    AdehazeHandle_t * AdehazeHandle = (AdehazeHandle_t *)ctx;
    if(attr.mode == RK_AIQ_DEHAZE_MODE_AUTO) {
        for(int i=0; i<9; i++) {
            AdehazeHandle->calib_dehaz->dehaze_iso.dehaze_en[i] = attr.stAuto.sw_dhaz_en;
            AdehazeHandle->calib_dehaz->dehaze_iso.enhance_en[i] = attr.stAuto.enhance_en;
            AdehazeHandle->calib_dehaz->dehaze_iso.cfg_alpha[i] = attr.stAuto.cfg_alpha;
        }
    }else if(attr.mode == RK_AIQ_DEHAZE_MODE_MANUAL) {
        for(int i=0; i<9; i++) {
            AdehazeHandle->calib_dehaz->dehaze_iso.dehaze_en[i] = attr.stManual.sw_dhaz_en;
            AdehazeHandle->calib_dehaz->dehaze_iso.enhance_en[i] = attr.stManual.enhance_en;
            AdehazeHandle->calib_dehaz->dehaze_iso.cfg_alpha[i] = attr.stManual.cfg_alpha;
            AdehazeHandle->calib_dehaz->dehaze_iso.cfg_wt[i] = attr.stManual.sw_dhaz_cfg_wt;
            AdehazeHandle->calib_dehaz->dehaze_iso.cfg_air[i] = attr.stManual.sw_dhaz_cfg_air;
            AdehazeHandle->calib_dehaz->dehaze_iso.cfg_tmax[i] = attr.stManual.sw_dhaz_cfg_tmax;
            AdehazeHandle->calib_dehaz->dehaze_iso.cfg_gratio[i] = attr.stManual.sw_dhaz_cfg_gratio;
        }
    }else if(attr.mode == RK_AIQ_DEHAZE_MODE_ENHANCE){
        for(int i=0; i<9; i++) {
            AdehazeHandle->calib_dehaz->dehaze_iso.dehaze_en[i] = attr.stEnhance.sw_dhaz_en;
            AdehazeHandle->calib_dehaz->dehaze_iso.enhance_en[i] = attr.stEnhance.enhance_en;
            AdehazeHandle->calib_dehaz->dehaze_iso.cfg_alpha[i] = attr.stEnhance.cfg_alpha;
            AdehazeHandle->calib_dehaz->dehaze_iso.enhance_value[i] = attr.stEnhance.level;
        }
    }else{
        LOGE_ADEHAZE("invalid mode!");
    }
    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
rk_aiq_uapi_adehaze_GetAttrib(RkAiqAlgoContext *ctx,
                              adehaze_sw_t *attr)
{
    return XCAM_RETURN_NO_ERROR;
}

