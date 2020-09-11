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
        if (attr.mode != AdehazeHandle->mode) {
            AdehazeHandle->calib_dehaz = AdehazeHandle->pCalibDb->dehaze;
            AdehazeHandle->mode = attr.mode;
        }
        for(int i = 0; i < 9; i++) {
            AdehazeHandle->calib_dehaz.night.dehaze_en[i] = attr.stAuto.sw_dhaz_en;
            AdehazeHandle->calib_dehaz.night.enhance_en[i] = attr.stAuto.enhance_en;
            AdehazeHandle->calib_dehaz.night.cfg_alpha[i] = attr.stAuto.cfg_alpha;

            AdehazeHandle->calib_dehaz.normal.dehaze_en[i] = attr.stAuto.sw_dhaz_en;
            AdehazeHandle->calib_dehaz.normal.enhance_en[i] = attr.stAuto.enhance_en;
            AdehazeHandle->calib_dehaz.normal.cfg_alpha[i] = attr.stAuto.cfg_alpha;

            AdehazeHandle->calib_dehaz.hdr.dehaze_en[i] = attr.stAuto.sw_dhaz_en;
            AdehazeHandle->calib_dehaz.hdr.enhance_en[i] = attr.stAuto.enhance_en;
            AdehazeHandle->calib_dehaz.hdr.cfg_alpha[i] = attr.stAuto.cfg_alpha;
        }
    } else if(attr.mode == RK_AIQ_DEHAZE_MODE_MANUAL) {
        if (attr.mode != AdehazeHandle->mode) {
            AdehazeHandle->calib_dehaz = AdehazeHandle->pCalibDb->dehaze;
            AdehazeHandle->mode = attr.mode;
        }
        AdehazeHandle->strength = attr.stManual.strength;
        for(int i = 0; i < 9; i++) {
            AdehazeHandle->calib_dehaz.night.dehaze_en[i] = attr.stManual.sw_dhaz_en;
            AdehazeHandle->calib_dehaz.night.enhance_en[i] = attr.stManual.enhance_en;
            //AdehazeHandle->calib_dehaz.night.cfg_alpha[i] = attr.stManual.cfg_alpha;
            AdehazeHandle->calib_dehaz.night.cfg_wt[i] = attr.stManual.sw_dhaz_cfg_wt;
            AdehazeHandle->calib_dehaz.night.cfg_air[i] = attr.stManual.sw_dhaz_cfg_air;
            AdehazeHandle->calib_dehaz.night.cfg_tmax[i] = attr.stManual.sw_dhaz_cfg_tmax;
            AdehazeHandle->calib_dehaz.night.cfg_gratio[i] = attr.stManual.sw_dhaz_cfg_gratio;

            AdehazeHandle->calib_dehaz.normal.dehaze_en[i] = attr.stManual.sw_dhaz_en;
            AdehazeHandle->calib_dehaz.normal.enhance_en[i] = attr.stManual.enhance_en;
            //AdehazeHandle->calib_dehaz.normal.cfg_alpha[i] = attr.stManual.cfg_alpha;
            AdehazeHandle->calib_dehaz.normal.cfg_wt[i] = attr.stManual.sw_dhaz_cfg_wt;
            AdehazeHandle->calib_dehaz.normal.cfg_air[i] = attr.stManual.sw_dhaz_cfg_air;
            AdehazeHandle->calib_dehaz.normal.cfg_tmax[i] = attr.stManual.sw_dhaz_cfg_tmax;
            AdehazeHandle->calib_dehaz.normal.cfg_gratio[i] = attr.stManual.sw_dhaz_cfg_gratio;

            AdehazeHandle->calib_dehaz.hdr.dehaze_en[i] = attr.stManual.sw_dhaz_en;
            AdehazeHandle->calib_dehaz.hdr.enhance_en[i] = attr.stManual.enhance_en;
            //AdehazeHandle->calib_dehaz.hdr.cfg_alpha[i] = attr.stManual.cfg_alpha;
            AdehazeHandle->calib_dehaz.hdr.cfg_wt[i] = attr.stManual.sw_dhaz_cfg_wt;
            AdehazeHandle->calib_dehaz.hdr.cfg_air[i] = attr.stManual.sw_dhaz_cfg_air;
            AdehazeHandle->calib_dehaz.hdr.cfg_tmax[i] = attr.stManual.sw_dhaz_cfg_tmax;
            AdehazeHandle->calib_dehaz.hdr.cfg_gratio[i] = attr.stManual.sw_dhaz_cfg_gratio;
        }
    } else if(attr.mode == RK_AIQ_DEHAZE_MODE_ENHANCE) {
        if (attr.mode != AdehazeHandle->mode) {
            AdehazeHandle->calib_dehaz = AdehazeHandle->pCalibDb->dehaze;
            AdehazeHandle->mode = attr.mode;
        }
        for(int i = 0; i < 9; i++) {
            AdehazeHandle->calib_dehaz.night.dehaze_en[i] = attr.stEnhance.sw_dhaz_en;
            AdehazeHandle->calib_dehaz.night.enhance_en[i] = attr.stEnhance.enhance_en;
            //AdehazeHandle->calib_dehaz->night.cfg_alpha[i] = attr.stEnhance.cfg_alpha;
            AdehazeHandle->calib_dehaz.night.enhance_value[i] = attr.stEnhance.level;

            AdehazeHandle->calib_dehaz.normal.dehaze_en[i] = attr.stEnhance.sw_dhaz_en;
            AdehazeHandle->calib_dehaz.normal.enhance_en[i] = attr.stEnhance.enhance_en;
            //AdehazeHandle->calib_dehaz.normal.cfg_alpha[i] = attr.stEnhance.cfg_alpha;
            AdehazeHandle->calib_dehaz.normal.enhance_value[i] = attr.stEnhance.level;

            AdehazeHandle->calib_dehaz.hdr.dehaze_en[i] = attr.stEnhance.sw_dhaz_en;
            AdehazeHandle->calib_dehaz.hdr.enhance_en[i] = attr.stEnhance.enhance_en;
            //AdehazeHandle->calib_dehaz.hdr.cfg_alpha[i] = attr.stEnhance.cfg_alpha;
            AdehazeHandle->calib_dehaz.hdr.enhance_value[i] = attr.stEnhance.level;
        }
    }else if(attr.mode == RK_AIQ_DEHAZE_MODE_DEFAULT) {
        for(int i = 0; i < 9; i++) {
            AdehazeHandle->calib_dehaz.night.dc_en[i] = attr.stManual.dc_en;
            AdehazeHandle->calib_dehaz.night.enhance_en[i] = attr.stManual.enhance_en;
            AdehazeHandle->calib_dehaz.normal.dc_en[i] = attr.stManual.dc_en;
            AdehazeHandle->calib_dehaz.normal.enhance_en[i] = attr.stManual.enhance_en;
            AdehazeHandle->calib_dehaz.hdr.dc_en[i] = attr.stManual.dc_en;
            AdehazeHandle->calib_dehaz.hdr.enhance_en[i] = attr.stManual.enhance_en;
        }
    } else {
        LOGE_ADEHAZE("invalid mode!");
    }
    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
rk_aiq_uapi_adehaze_GetAttrib(RkAiqAlgoContext *ctx, adehaze_sw_t *attr)
{
    AdehazeHandle_t * AdehazeHandle = (AdehazeHandle_t *)ctx;
    attr->mode = AdehazeHandle->mode;
    if (AdehazeHandle->mode == RK_AIQ_DEHAZE_MODE_AUTO) {
        if (AdehazeHandle->Dehaze_ISO_mode == 0) {
        }else if (AdehazeHandle->Dehaze_ISO_mode == 1) {
        }else if (AdehazeHandle->Dehaze_ISO_mode == 2) {
        }
    }else if (AdehazeHandle->mode == RK_AIQ_DEHAZE_MODE_MANUAL) {
        attr->stManual.strength = AdehazeHandle->strength;
        if (AdehazeHandle->Dehaze_ISO_mode == 0) {
        }else if (AdehazeHandle->Dehaze_ISO_mode == 1) {
        }else if (AdehazeHandle->Dehaze_ISO_mode == 2) {
        }
    }else if (AdehazeHandle->mode == RK_AIQ_DEHAZE_MODE_ENHANCE) {
        if (AdehazeHandle->Dehaze_ISO_mode == 0) {
        }else if (AdehazeHandle->Dehaze_ISO_mode == 1) {
        }else if (AdehazeHandle->Dehaze_ISO_mode == 2) {
        }
    }else if (AdehazeHandle->mode == RK_AIQ_DEHAZE_MODE_DEFAULT) {
    }
    return XCAM_RETURN_NO_ERROR;
}

