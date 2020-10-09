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
    if(attr.mode == RK_AIQ_DEHAZE_MODE_AUTO) {
        if (attr.mode != AdehazeHandle->mode) {
            AdehazeHandle->calib_dehaz = AdehazeHandle->pCalibDb->dehaze;
            AdehazeHandle->mode = attr.mode;
        }

        AdehazeHandle->calib_dehaz.en = attr.stAuto.sw_dhaz_en;
        AdehazeHandle->calib_dehaz.cfg_alpha_normal = attr.stAuto.cfg_alpha;
        AdehazeHandle->calib_dehaz.cfg_alpha_hdr = attr.stAuto.cfg_alpha;
        AdehazeHandle->calib_dehaz.cfg_alpha_night = attr.stAuto.cfg_alpha;


        for(int i = 0; i < 3; i++)
            AdehazeHandle->calib_dehaz.enhance_setting[i].en = attr.stAuto.enhance_en;


    } else if(attr.mode == RK_AIQ_DEHAZE_MODE_MANUAL) {
        if (attr.mode != AdehazeHandle->mode) {
            AdehazeHandle->calib_dehaz = AdehazeHandle->pCalibDb->dehaze;
            AdehazeHandle->mode = attr.mode;
        }
        AdehazeHandle->strength = attr.stManual.strength;

        AdehazeHandle->calib_dehaz.en = attr.stManual.sw_dhaz_en;
        //AdehazeHandle->calib_dehaz.cfg_alpha_normal = attr.stManual.cfg_alpha;
        //AdehazeHandle->calib_dehaz.cfg_alpha_hdr = attr.stManual.cfg_alpha;
        //AdehazeHandle->calib_dehaz.cfg_alpha_night = attr.stManual.cfg_alpha;

        for(int i = 0; i < 3; i++)
        {
            AdehazeHandle->calib_dehaz.enhance_setting[i].en = attr.stManual.enhance_en;
            for(int j = 0; j < 9; j++) {
                AdehazeHandle->calib_dehaz.dehaze_setting[i].cfg_wt[j] = attr.stManual.sw_dhaz_cfg_wt;
                AdehazeHandle->calib_dehaz.dehaze_setting[i].cfg_air[j] = attr.stManual.sw_dhaz_cfg_air;
                AdehazeHandle->calib_dehaz.dehaze_setting[i].cfg_tmax[j] = attr.stManual.sw_dhaz_cfg_tmax;
            }
        }
    } else if(attr.mode == RK_AIQ_DEHAZE_MODE_ENHANCE) {
        if (attr.mode != AdehazeHandle->mode) {
            AdehazeHandle->calib_dehaz = AdehazeHandle->pCalibDb->dehaze;
            AdehazeHandle->mode = attr.mode;
        }

        AdehazeHandle->calib_dehaz.en = attr.stEnhance.sw_dhaz_en;
        //AdehazeHandle->calib_dehaz.cfg_alpha_normal = attr.stManual.cfg_alpha;
        //AdehazeHandle->calib_dehaz.cfg_alpha_hdr = attr.stManual.cfg_alpha;
        //AdehazeHandle->calib_dehaz.cfg_alpha_night = attr.stManual.cfg_alpha;
        for(int i = 0; i < 3; i++)
        {
            AdehazeHandle->calib_dehaz.enhance_setting[i].en = attr.stEnhance.enhance_en;
            for(int j = 0; j < 9; i++)
                AdehazeHandle->calib_dehaz.enhance_setting[i].enhance_value[j] = attr.stEnhance.level;
        }
    } else if(attr.mode == RK_AIQ_DEHAZE_MODE_DEFAULT) {
        for(int i = 0; i < 3; i++)
        {
            //todo//AdehazeHandle->calib_dehaz.dehaze_setting[i].dc_en = attr.stManual.dc_en;
            //todo//AdehazeHandle->calib_dehaz.enhance_setting[i].enhance_en = attr.stManual.enhance_en;
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
        } else if (AdehazeHandle->Dehaze_ISO_mode == 1) {
        } else if (AdehazeHandle->Dehaze_ISO_mode == 2) {
        }
    } else if (AdehazeHandle->mode == RK_AIQ_DEHAZE_MODE_MANUAL) {
        attr->stManual.strength = AdehazeHandle->strength;
        if (AdehazeHandle->Dehaze_ISO_mode == 0) {
        } else if (AdehazeHandle->Dehaze_ISO_mode == 1) {
        } else if (AdehazeHandle->Dehaze_ISO_mode == 2) {
        }
    } else if (AdehazeHandle->mode == RK_AIQ_DEHAZE_MODE_ENHANCE) {
        if (AdehazeHandle->Dehaze_ISO_mode == 0) {
        } else if (AdehazeHandle->Dehaze_ISO_mode == 1) {
        } else if (AdehazeHandle->Dehaze_ISO_mode == 2) {
        }
    } else if (AdehazeHandle->mode == RK_AIQ_DEHAZE_MODE_DEFAULT) {
    }
    return XCAM_RETURN_NO_ERROR;
}

