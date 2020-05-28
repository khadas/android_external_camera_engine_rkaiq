/*
 *  Copyright (c) 2019 Rockchip Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "rk_aiq_user_api_imgproc.h"


#define RKAIQ_IMGPROC_CHECK_RET(ret, format, ...) \
    if (ret) { \
        LOGE(format, ##__VA_ARGS__); \
        return ret; \
    }

RKAIQ_BEGIN_DECLARE

/*
*****************************
*
* Desc: set exposure control mode
* Argument:
*   mode:  auto: auto exposure
*          manual: manual exposure
*****************************
*/
XCamReturn rk_aiq_uapi_setExpMode(const rk_aiq_sys_ctx_t* ctx, opMode_t mode)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    Uapi_ExpSwAttr_t expSwAttr;
    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error!");
    }

    ret = rk_aiq_user_api_ae_getExpSwAttr(ctx, &expSwAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "get exp attr failed!\nset exp mode failed!");
    if (mode == OP_AUTO) {
        expSwAttr.AecOpType = RK_AIQ_OP_MODE_AUTO;
    } else if (mode == OP_MANUALl) {
        expSwAttr.AecOpType = RK_AIQ_OP_MODE_MANUAL;
        //}else if (mode == OP_SEMI_AUTO) {
    } else {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "mode is invalid!");
    }
    ret = rk_aiq_user_api_ae_setExpSwAttr(ctx, expSwAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "set exp attr failed!\nset exp mode failed!");
    return (ret);
}

XCamReturn rk_aiq_uapi_getExpMode(const rk_aiq_sys_ctx_t* ctx, opMode_t *mode)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    Uapi_ExpSwAttr_t expSwAttr;
    if ((ctx == NULL) || (mode == NULL)) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error!");
    }

    ret = rk_aiq_user_api_ae_getExpSwAttr(ctx, &expSwAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "get exp attr failed!\nget exp mode failed!");
    if (expSwAttr.AecOpType == RK_AIQ_OP_MODE_AUTO) {
        *mode = OP_AUTO;
    } else if (expSwAttr.AecOpType == RK_AIQ_OP_MODE_MANUAL) {
        *mode = OP_MANUALl;
    }
    return (ret);
}

/*
*****************************
*
* Desc: set auto exposure mode
* Argument:
*   mode:
*
*****************************
*/
XCamReturn rk_aiq_uapi_setAeMode(const rk_aiq_sys_ctx_t* ctx, aeMode_t mode)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    return ret;
}

XCamReturn rk_aiq_uapi_getAeMode(const rk_aiq_sys_ctx_t* ctx, aeMode_t *mode)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    return ret;
}


/*
*****************************
*
* Desc: set exposure parameter
* Argument:
*    auto exposure mode:
*      exposure gain will be adjust between [gain->min, gain->max]；
*    manual exposure mode:
*      gain->min == gain->max
*
*****************************
*/
XCamReturn rk_aiq_uapi_setExpGainRange(const rk_aiq_sys_ctx_t* ctx, paRange_t *gain)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    Uapi_ExpSwAttr_t expSwAttr;
    if ((ctx == NULL) || (gain == NULL)) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error!");
    }
    ret = rk_aiq_user_api_ae_getExpSwAttr(ctx, &expSwAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "get exp attr failed!\nsetExpGainRange failed!");
    expSwAttr.stAuto.stLinAeRange.stGainRange.Max = gain->max;
    expSwAttr.stAuto.stLinAeRange.stGainRange.Min = gain->min;
    expSwAttr.stAuto.stHdrAeRange.stGainRange[0].Max = gain->max;
    expSwAttr.stAuto.stHdrAeRange.stGainRange[0].Min = gain->min;
    expSwAttr.stAuto.stHdrAeRange.stGainRange[1].Max = gain->max;
    expSwAttr.stAuto.stHdrAeRange.stGainRange[1].Min = gain->min;
    expSwAttr.stAuto.stHdrAeRange.stGainRange[2].Max = gain->max;
    expSwAttr.stAuto.stHdrAeRange.stGainRange[2].Min = gain->min;
    ret = rk_aiq_user_api_ae_setExpSwAttr(ctx, expSwAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "set exp attr failed!\nsetExpGainRange failed!");
    return (ret);
}

XCamReturn rk_aiq_uapi_getExpGainRange(const rk_aiq_sys_ctx_t* ctx, paRange_t *gain)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    Uapi_ExpSwAttr_t expSwAttr;
    if ((ctx == NULL) || (gain == NULL)) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error!");
    }
    ret = rk_aiq_user_api_ae_getExpSwAttr(ctx, &expSwAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "get exp attr failed!\ngetExpGainRange failed!");
    gain->max = expSwAttr.stAuto.stLinAeRange.stGainRange.Max;
    gain->min = expSwAttr.stAuto.stLinAeRange.stGainRange.Min;
    //hdr?long-frame?short-frame?
    return (ret);
}
/*
*****************************
*
* Desc: set exposure parameter
* Argument:
*    auto exposure mode:
*       exposure time will be adjust between [time->min, time->max]；
*    manual exposure mode:
*       exposure time will be set gain->min == gain->max;
*
*****************************
*/
XCamReturn rk_aiq_uapi_setExpTimeRange(const rk_aiq_sys_ctx_t* ctx, paRange_t *time)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    Uapi_ExpSwAttr_t expSwAttr;
    if ((ctx == NULL) || (time == NULL)) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error!");
    }
    ret = rk_aiq_user_api_ae_getExpSwAttr(ctx, &expSwAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "get exp attr failed!\nsetExpTimeRange failed!");
    expSwAttr.stAuto.stLinAeRange.stExpTimeRange.Max = time->max;
    expSwAttr.stAuto.stLinAeRange.stExpTimeRange.Min = time->min;
    expSwAttr.stAuto.stHdrAeRange.stExpTimeRange[0].Max = time->max;
    expSwAttr.stAuto.stHdrAeRange.stExpTimeRange[0].Min = time->min;
    expSwAttr.stAuto.stHdrAeRange.stExpTimeRange[1].Max = time->max;
    expSwAttr.stAuto.stHdrAeRange.stExpTimeRange[1].Min = time->min;
    expSwAttr.stAuto.stHdrAeRange.stExpTimeRange[2].Max = time->max;
    expSwAttr.stAuto.stHdrAeRange.stExpTimeRange[2].Min = time->min;
    ret = rk_aiq_user_api_ae_setExpSwAttr(ctx, expSwAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "set exp attr failed!\nsetExpTimeRange failed!");
    return (ret);
}

XCamReturn rk_aiq_uapi_getExpTimeRange(const rk_aiq_sys_ctx_t* ctx, paRange_t *time)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    Uapi_ExpSwAttr_t expSwAttr;
    if ((ctx == NULL) || (time == NULL)) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error!");
    }
    ret = rk_aiq_user_api_ae_getExpSwAttr(ctx, &expSwAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "get exp attr failed!\ngetExpTimeRange failed!");
    time->max = expSwAttr.stAuto.stLinAeRange.stExpTimeRange.Max;
    time->min = expSwAttr.stAuto.stLinAeRange.stExpTimeRange.Min;
    //hdr?
    return (ret);
}

/*
**********************************************************
* Auto exposure advanced features
**********************************************************
*/
/*
*****************************
*
* Desc: blacklight compensation
* Argument:
*      on:  1  on
*           0  off
*      rect: blacklight compensation area
*
*****************************
*/
XCamReturn rk_aiq_uapi_setBLCMode(const rk_aiq_sys_ctx_t* ctx, bool on, paRect_t *rect)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    if ((ctx == NULL) || (rect == NULL)) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error!");
    }
    return ret;
}

/*
*****************************
*
* Desc: highlight compensation
* Argument:
*      on:  1  on
*           0  off
*      rect: blacklight compensation area
*
*****************************
*/
XCamReturn rk_aiq_uapi_setHLCMode(const rk_aiq_sys_ctx_t* ctx, bool on, paRect_t *rect)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    if ((ctx == NULL) || (rect == NULL)) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error!");
    }
    Uapi_LinExpAttr_t linExpAttr;
    ret = rk_aiq_user_api_ae_getLinExpAttr(ctx, &linExpAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "get exp attr failed!\n getLinExpAttr failed!");
    if (on)
        linExpAttr.StrategyMode = RKAIQ_AEC_STRATEGY_MODE_HIGHLIGHT_PRIOR;
    else
        linExpAttr.StrategyMode = RKAIQ_AEC_STRATEGY_MODE_AUTO;
    ret = rk_aiq_user_api_ae_setLinExpAttr(ctx, linExpAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "get exp attr failed!\n setLinExpAttr failed!");

    return ret;

}
/*
*****************************
*
* Desc: set lowlight exposure mode
* Argument:
*    mode:
*       auto: auto lowlight mode
*       manual: manual lowlight mode
*
*****************************
*/
XCamReturn rk_aiq_uapi_setLExpMode(const rk_aiq_sys_ctx_t* ctx, opMode_t mode)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error!");
    }
    Uapi_LinExpAttr_t linExpAttr;
    ret = rk_aiq_user_api_ae_getLinExpAttr(ctx, &linExpAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "setLExpMode failed!");
    if (mode == OP_MANUALl) {
        linExpAttr.StrategyMode = RKAIQ_AEC_STRATEGY_MODE_LOWLIGHT_PRIOR;
    } else if (mode == OP_AUTO) {
        linExpAttr.StrategyMode = RKAIQ_AEC_STRATEGY_MODE_AUTO;
    } else {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "mode is invalid!");
    }
    ret = rk_aiq_user_api_ae_setLinExpAttr(ctx, linExpAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "setLExpMode failed!");

    return ret;
}

XCamReturn rk_aiq_uapi_getLExpMode(const rk_aiq_sys_ctx_t* ctx, opMode_t *mode)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    if ((ctx == NULL) || (mode == NULL)) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error!");
    }
    Uapi_LinExpAttr_t linExpAttr;
    ret = rk_aiq_user_api_ae_getLinExpAttr(ctx, &linExpAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "getLExpMode failed!");
    if (linExpAttr.StrategyMode == RKAIQ_AEC_STRATEGY_MODE_LOWLIGHT_PRIOR)
        *mode = OP_MANUALl;
    else if (linExpAttr.StrategyMode == RKAIQ_AEC_STRATEGY_MODE_AUTO)
        *mode = OP_AUTO;

    return ret;

}


/*
*****************************
*
* Desc: set manual lowlight exposure time ratio
* Argument:
*    ratio:  [1.0, 128.0]
*
*****************************
*/
XCamReturn rk_aiq_uapi_setMLExp(const rk_aiq_sys_ctx_t* ctx, unsigned int ratio)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    return ret;
}

XCamReturn rk_aiq_uapi_getMLExp(const rk_aiq_sys_ctx_t* ctx, unsigned int *ratio)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    return ret;
}

XCamReturn rk_aiq_uapi_setAntiFlickerMode(const rk_aiq_sys_ctx_t* ctx, antiFlickerMode_t mode)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    Uapi_ExpSwAttr_t expSwAttr;
    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error!");
    }

    ret = rk_aiq_user_api_ae_getExpSwAttr(ctx, &expSwAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "get exp attr failed!\nsetAntiFlickerMode failed!");
    if (mode == ANTIFLICKER_AUTO_MODE) {
        expSwAttr.stAntiFlicker.enable = true;
        expSwAttr.stAntiFlicker.Mode = AEC_ANTIFLICKER_AUTO_MODE;
    } else if(mode == ANTIFLICKER_NORMAL_MODE) {
        expSwAttr.stAntiFlicker.enable = true;
        expSwAttr.stAntiFlicker.Mode = AEC_ANTIFLICKER_NORMAL_MODE;
    }
    ret = rk_aiq_user_api_ae_setExpSwAttr(ctx, expSwAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "set exp attr failed!\nsetAntiFlickerMode failed!");

    return ret;
}

XCamReturn rk_aiq_uapi_getAntiFlickerMode(const rk_aiq_sys_ctx_t* ctx, antiFlickerMode_t *mode)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    Uapi_ExpSwAttr_t expSwAttr;
    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error!");
    }

    ret = rk_aiq_user_api_ae_getExpSwAttr(ctx, &expSwAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "get exp attr failed!\ngetAntiFlickerMode!");
    if (expSwAttr.stAntiFlicker.Mode == AEC_ANTIFLICKER_AUTO_MODE)
        *mode = ANTIFLICKER_AUTO_MODE;
    else if (expSwAttr.stAntiFlicker.Mode == AEC_ANTIFLICKER_NORMAL_MODE)
        *mode = ANTIFLICKER_NORMAL_MODE;

    return ret;
}

/*
*****************************
*
* Desc: set power line frequence
* Argument:
*    freq
*
*****************************
*/
XCamReturn rk_aiq_uapi_setExpPwrLineFreqMode(const rk_aiq_sys_ctx_t* ctx, expPwrLineFreq_t freq)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    Uapi_ExpSwAttr_t expSwAttr;
    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error!");
    }

    ret = rk_aiq_user_api_ae_getExpSwAttr(ctx, &expSwAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "get exp attr failed!\nsetExpPwrLineFreqMode failed!");
    if (freq == EXP_PWR_LINE_FREQ_50HZ) {
        expSwAttr.stAntiFlicker.enable = true;
        //expSwAttr.stAntiFlicker.Mode = AEC_ANTIFLICKER_AUTO_MODE;
        expSwAttr.stAntiFlicker.Frequency = AEC_FLICKER_FREQUENCY_50HZ;
    } else if (freq == EXP_PWR_LINE_FREQ_60HZ) {
        expSwAttr.stAntiFlicker.enable = true;
        //expSwAttr.stAntiFlicker.Mode = AEC_ANTIFLICKER_AUTO_MODE;
        expSwAttr.stAntiFlicker.Frequency = AEC_FLICKER_FREQUENCY_60HZ;
    } else if (freq == EXP_PWR_LINE_FREQ_DIS) {
        expSwAttr.stAntiFlicker.enable = true;
        //expSwAttr.stAntiFlicker.Mode = AEC_ANTIFLICKER_AUTO_MODE;
        expSwAttr.stAntiFlicker.Frequency = AEC_FLICKER_FREQUENCY_OFF;
    } else {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "freq is invalid!");
    }
    ret = rk_aiq_user_api_ae_setExpSwAttr(ctx, expSwAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "set exp attr failed!\nsetExpPwrLineFreqMode failed!");
    return (ret);
}

XCamReturn rk_aiq_uapi_getExpPwrLineFreqMode(const rk_aiq_sys_ctx_t* ctx, expPwrLineFreq_t *freq)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    Uapi_ExpSwAttr_t expSwAttr;
    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error!");
    }

    ret = rk_aiq_user_api_ae_getExpSwAttr(ctx, &expSwAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "get exp attr failed!\nsetExpPwrLineFreqMode failed!");
    if (expSwAttr.stAntiFlicker.Frequency == AEC_FLICKER_FREQUENCY_50HZ) {
        *freq = EXP_PWR_LINE_FREQ_50HZ;
    } else if (expSwAttr.stAntiFlicker.Frequency == AEC_FLICKER_FREQUENCY_60HZ) {
        *freq = EXP_PWR_LINE_FREQ_60HZ;
    } else if (expSwAttr.stAntiFlicker.Frequency == AEC_FLICKER_FREQUENCY_OFF) {
        *freq = EXP_PWR_LINE_FREQ_DIS;
    } else {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "freq is invalid!");
    }

    return (ret);
}



/*
*****************************
*
* Desc: set day night switch mode
* Argument:
*    mode
*
*****************************
*/
XCamReturn rk_aiq_uapi_setDayNSwMode(const rk_aiq_sys_ctx_t* ctx, opMode_t mode)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    Uapi_ExpSwAttr_t expSwAttr;

    if (mode == OP_AUTO) {
        expSwAttr.DNTrigger = 1;
        rk_aiq_user_api_ae_setExpSwAttr(ctx, expSwAttr);
    } else if (mode == OP_MANUALl) {
        expSwAttr.DNTrigger = 0;
        expSwAttr.DNMode = AEC_DNMODE_DAY;
        rk_aiq_user_api_ae_setExpSwAttr(ctx, expSwAttr);
    }
    return ret;
}


XCamReturn rk_aiq_uapi_getDayNSwMode(const rk_aiq_sys_ctx_t* ctx, opMode_t *mode)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    Uapi_ExpSwAttr_t expSwAttr;
    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error!");
    }

    ret = rk_aiq_user_api_ae_getExpSwAttr(ctx, &expSwAttr);
    if (expSwAttr.DNTrigger == 0)
        *mode = OP_MANUALl;
    else if (expSwAttr.DNTrigger == 1 || expSwAttr.DNTrigger == 2)
        *mode = OP_AUTO;

    return ret;
}

/*
*****************************
*
* Desc: set manual day night scene
*    this function is active for DayNSw is manual mode
* Argument:
*    scene
*
*****************************
*/
XCamReturn rk_aiq_uapi_setMDNScene(const rk_aiq_sys_ctx_t* ctx, dayNightScene_t scene)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    Uapi_ExpSwAttr_t expSwAttr;
    if (scene < DAYNIGHT_SCENE_DAY || scene > DAYNIGHT_SCENE_NIGHT)
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error!");
    ret = rk_aiq_user_api_ae_getExpSwAttr(ctx, &expSwAttr);
    if (expSwAttr.DNTrigger == 0) {
        if (scene == DAYNIGHT_SCENE_DAY)
            expSwAttr.DNMode = AEC_DNMODE_DAY;
        else if(scene == DAYNIGHT_SCENE_NIGHT)
            expSwAttr.DNMode = AEC_DNMODE_NIGHT;
        rk_aiq_user_api_ae_setExpSwAttr(ctx, expSwAttr);
    } else if (expSwAttr.DNTrigger == 1 || expSwAttr.DNTrigger == 2) {
        RKAIQ_IMGPROC_CHECK_RET(ret, "mode error, auto mode now!");
    }
    return ret;
}

XCamReturn rk_aiq_uapi_getMDNScene(const rk_aiq_sys_ctx_t* ctx, dayNightScene_t *scene)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    Uapi_ExpSwAttr_t expSwAttr;
    ret = rk_aiq_user_api_ae_getExpSwAttr(ctx, &expSwAttr);
    if (expSwAttr.DNMode == AEC_DNMODE_DAY)
        *scene = DAYNIGHT_SCENE_DAY;
    else if(expSwAttr.DNMode == AEC_DNMODE_NIGHT)
        *scene = DAYNIGHT_SCENE_NIGHT;

    return ret;
}


/*
*****************************
*
* Desc: set auto day night switch sensitivity
*    this function is active for DayNSw is auto mode
* Argument:
*    level: [1, 3]
*
*****************************
*/
XCamReturn rk_aiq_uapi_setADNSens(const rk_aiq_sys_ctx_t* ctx, unsigned int level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    return ret;
}

XCamReturn rk_aiq_uapi_getADNSens(const rk_aiq_sys_ctx_t* ctx, unsigned int *level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    return ret;
}

/*
*****************************
*
* Desc: set fill light mode
* Argument:
*    mode
*
*****************************
*/
XCamReturn rk_aiq_uapi_setFLightMode(const rk_aiq_sys_ctx_t* ctx, opMode_t mode)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    return ret;
}

XCamReturn rk_aiq_uapi_getFLightMode(const rk_aiq_sys_ctx_t* ctx, opMode_t *mode)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    return ret;
}



/*
*****************************
*
* Desc: set maual fill light mode
* Argument:
*    on:  1: on
*         0: off
*
*****************************
*/
XCamReturn rk_aiq_uapi_setMFLight(const rk_aiq_sys_ctx_t* ctx, bool on)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    return ret;
}

XCamReturn rk_aiq_uapi_getMFLight(const rk_aiq_sys_ctx_t* ctx, bool *on)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    return ret;
}



/*
**********************************************************
* White balance & Color
**********************************************************
*/

/*
*****************************
*
* Desc: set white balance mode
* Argument:
*   mode:  auto: auto white balance
*          manual: manual white balance
*****************************
*/
XCamReturn rk_aiq_uapi_setWBMode(const rk_aiq_sys_ctx_t* ctx, opMode_t mode)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    rk_aiq_wb_attrib_t attr;
    ret = rk_aiq_user_api_awb_GetAttrib(ctx, &attr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "setWBMode failed!");
    if (mode == OP_AUTO) {
        attr.mode = RK_AIQ_WB_MODE_AUTO;
    } else if (mode == OP_MANUALl) {
        attr.mode = RK_AIQ_WB_MODE_MANUAL;
        attr.stManual.mode = RK_AIQ_MWB_MODE_SCENE;
        attr.stManual.para.scene =  RK_AIQ_WBCT_DAYLIGHT;
    } else {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "Not supported mode!");
    }
    ret = rk_aiq_user_api_awb_SetAttrib(ctx, attr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "setWbMode failed!");
    return ret;
}

XCamReturn rk_aiq_uapi_getWBMode(const rk_aiq_sys_ctx_t* ctx, opMode_t *mode)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    rk_aiq_wb_attrib_t attr;
    ret = rk_aiq_user_api_awb_GetAttrib(ctx, &attr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "getWBMode failed!");
    if (attr.mode == RK_AIQ_WB_MODE_AUTO) {
        *mode = OP_AUTO;
    } else if (attr.mode == RK_AIQ_WB_MODE_MANUAL) {
        *mode = OP_MANUALl;
    } else {
        *mode = OP_INVAL;
    }

    return ret;
}



/*
*****************************
*
* Desc: lock/unlock auto white balance
* Argument:
*
*
*****************************
*/
XCamReturn rk_aiq_uapi_lockAWB(const rk_aiq_sys_ctx_t* ctx)
{
    return rk_aiq_user_api_awb_Lock(ctx);
}

XCamReturn rk_aiq_uapi_unlockAWB(const rk_aiq_sys_ctx_t* ctx)
{
    return rk_aiq_user_api_awb_Unlock(ctx);
}



/*
*****************************
*
* Desc: set auto white balance mode
* Argument:
*
*
*****************************
*/

XCamReturn rk_aiq_uapi_setAWBRange(const rk_aiq_sys_ctx_t* ctx, awbRange_t range)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    return ret;
}

XCamReturn rk_aiq_uapi_getAWBRange(const rk_aiq_sys_ctx_t* ctx, awbRange_t *range)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    return ret;
}


/*
*****************************
*
* Desc: set manual white balance scene mode
* Argument:
*   ct_scene:
*
*****************************
*/
XCamReturn rk_aiq_uapi_setMWBScene(const rk_aiq_sys_ctx_t* ctx, rk_aiq_wb_scene_t scene)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    rk_aiq_wb_attrib_t attr;
    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error, setMWBScene failed!");
    }

    if (scene < RK_AIQ_WBCT_INCANDESCENT || scene > RK_AIQ_WBCT_SHADE) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "invalid scene mode, setMWBScene failed!");
    }

    ret = rk_aiq_user_api_awb_GetAttrib(ctx, &attr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "setMWBScene failed!");
    attr.mode = RK_AIQ_WB_MODE_MANUAL;
    attr.stManual.mode = RK_AIQ_MWB_MODE_SCENE;
    attr.stManual.para.scene = scene;
    ret = rk_aiq_user_api_awb_SetAttrib(ctx, attr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "setMWBScene failed!");
    return ret;
}

XCamReturn rk_aiq_uapi_getMWBScene(const rk_aiq_sys_ctx_t* ctx, rk_aiq_wb_scene_t *scene)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    rk_aiq_wb_attrib_t attr;
    if ((ctx == NULL) || (scene == NULL)) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error, getMWBScene failed!");
    }
    //attr.mode = RK_AIQ_WB_MODE_MANUAL;
    attr.stManual.mode = RK_AIQ_MWB_MODE_SCENE;
    ret = rk_aiq_user_api_awb_GetAttrib(ctx, &attr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "getMWBScene failed!");
    *scene = attr.stManual.para.scene;

    return ret;
}


/*
*****************************
*
* Desc: set manual white balance r/b gain
* Argument:
*   ct_scene:
*
*****************************
*/
XCamReturn rk_aiq_uapi_setMWBGain(const rk_aiq_sys_ctx_t* ctx, rk_aiq_wb_gain_t *gain)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    rk_aiq_wb_attrib_t attr;
    if ((ctx == NULL) || (gain == NULL)) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error, setMWBGain failed!");
    }

    ret = rk_aiq_user_api_awb_GetAttrib(ctx, &attr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "getMWBGain failed!");
    attr.mode = RK_AIQ_WB_MODE_MANUAL;
    attr.stManual.mode = RK_AIQ_MWB_MODE_WBGAIN;
    attr.stManual.para.gain = *gain;
    ret = rk_aiq_user_api_awb_SetAttrib(ctx, attr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "setMWBGain failed!");
    return ret;
}

XCamReturn rk_aiq_uapi_getMWBGain(const rk_aiq_sys_ctx_t* ctx, rk_aiq_wb_gain_t *gain)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    rk_aiq_wb_querry_info_t query_info;
    if ((ctx == NULL) || (gain == NULL)) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error, getMWBGain failed!");
    }
    ret = rk_aiq_user_api_awb_QueryWBInfo(ctx, &query_info);
    RKAIQ_IMGPROC_CHECK_RET(ret, "getMWBGain failed!");
    *gain = query_info.gain;

    return ret;
}

/*
*****************************
*
* Desc: set manual white balance color temperature
* Argument:
*   ct: color temperature value [2800, 7500]K
*
*****************************
*/
XCamReturn rk_aiq_uapi_setMWBCT(const rk_aiq_sys_ctx_t* ctx, rk_aiq_wb_cct_t ct)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    rk_aiq_wb_attrib_t attr;
    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error, setMWBCT failed!");
    }

    ret = rk_aiq_user_api_awb_GetAttrib(ctx, &attr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "getMWBCT failed!");
    attr.mode = RK_AIQ_WB_MODE_MANUAL;
    attr.stManual.mode = RK_AIQ_MWB_MODE_CCT;
    attr.stManual.para.cct = ct;
    ret = rk_aiq_user_api_awb_SetAttrib(ctx, attr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "setMWBCT failed!");
    return ret;

}

XCamReturn rk_aiq_uapi_getMWBCT(const rk_aiq_sys_ctx_t* ctx, rk_aiq_wb_cct_t *ct)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    if ((ctx == NULL) || (ct == NULL)) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error, getMWBCT failed!");
    }

    ret = rk_aiq_user_api_awb_GetCCT(ctx, ct);
    RKAIQ_IMGPROC_CHECK_RET(ret, "getMWBCT failed!");

    return ret;
}


/*
*****************************
*
* Desc: set color supperssion level
* Argument:
*   level: [0, 100]
*
*****************************
*/
XCamReturn rk_aiq_uapi_setCrSuppsn(const rk_aiq_sys_ctx_t* ctx, unsigned int level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    rk_aiq_ccm_attrib_t ccm;
    rk_aiq_user_api_accm_GetAttrib(ctx, &ccm);
    for(int i = 0; i < RK_AIQ_ACCM_COLOR_GAIN_NUM; i++) {
        ccm.stAuto.color_inhibition.sensorGain[i] = 2.0f;
        ccm.stAuto.color_inhibition.level[i] = level;
    }
    return rk_aiq_user_api_accm_SetAttrib(ctx, ccm);
}


XCamReturn rk_aiq_uapi_getCrSuppsn(const rk_aiq_sys_ctx_t* ctx, unsigned int *level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    rk_aiq_ccm_querry_info_t ccm_querry_info;
    rk_aiq_user_api_accm_QueryCcmInfo(ctx, &ccm_querry_info);
    *level = (int)ccm_querry_info.color_inhibition_level;
    return ret;
}


/*
**********************************************************
* Focus & Zoom
**********************************************************
*/
/*
*****************************
*
* Desc: set focus mode
* Argument:
*   mode:  auto: auto focus
*          manual: manual focus
*          semi-auto: semi-auto focus
*****************************
*/
XCamReturn rk_aiq_uapi_setFocusMode(const rk_aiq_sys_ctx_t* ctx, opMode_t mode)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    return ret;
}

XCamReturn rk_aiq_uapi_getFocusMode(const rk_aiq_sys_ctx_t* ctx, opMode_t *mode)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    return ret;
}



/*
*****************************
*
* Desc: set minimum focus distance
* Argument:
*   disrance:  unint is cm
*****************************
*/
XCamReturn rk_aiq_uapi_setMinFocusDis(const rk_aiq_sys_ctx_t* ctx, unsigned int distance)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    return ret;
}

XCamReturn rk_aiq_uapi_getMinFocusDis(const rk_aiq_sys_ctx_t* ctx, unsigned int *distance)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    return ret;
}


/*
*****************************
*
* Desc: set optical zoom range
* Argument:
*   range:  [1.0, 100.0]
*
*****************************
*/
XCamReturn rk_aiq_uapi_setOpZoomRange(const rk_aiq_sys_ctx_t* ctx, paRange_t *range)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    return ret;
}

XCamReturn rk_aiq_uapi_getOpZoomRange(const rk_aiq_sys_ctx_t* ctx, paRange_t *range)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    return ret;
}

/*
*****************************
*
* Desc: set optical zoom speed
* Argument:
*   level:  [1, 10]
*
*****************************
*/
XCamReturn rk_aiq_uapi_setOpZoomSpeed(const rk_aiq_sys_ctx_t* ctx, unsigned int level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    return ret;
}

XCamReturn rk_aiq_uapi_getOpZoomSpeed(const rk_aiq_sys_ctx_t* ctx, unsigned int *level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    return ret;
}

/*
**********************************************************
* HDR
**********************************************************
*/
/*
*****************************
*
* Desc: set hdr mode
* Argument:
*   mode:
*     auto: auto hdr mode
*     manual：manual hdr mode
*
*****************************
*/
XCamReturn rk_aiq_uapi_setHDRMode(const rk_aiq_sys_ctx_t* ctx, opMode_t mode)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    ahdr_attrib_t attr;
    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error, setHDRMode failed!");
    }
    //rk_aiq_user_api_ahdr_GetAttrib(ctx, &attr);
    if (mode == OP_AUTO) {
        attr.opMode = OpMode_Auto;
    } else if (mode == OP_MANUALl) {
        attr.opMode = OpMode_MANU;
    } else {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "Not supported mode!");
    }

    return rk_aiq_user_api_ahdr_SetAttrib(ctx, attr);
}

XCamReturn rk_aiq_uapi_getHDRMode(const rk_aiq_sys_ctx_t* ctx, opMode_t *mode)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    return ret;
}

/*
*****************************
*
* Desc: set manual hdr strength
*    this function is active for HDR is manual mode
* Argument:
*   on:   1: on
*         0: off
*   level: [0, 10]
*
*****************************
*/
XCamReturn rk_aiq_uapi_setMHDRStrth(const rk_aiq_sys_ctx_t* ctx, bool on, unsigned int level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    return ret;
}

XCamReturn rk_aiq_uapi_getMHDRStrth(const rk_aiq_sys_ctx_t* ctx, bool *on, unsigned int *level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    return ret;
}

/*
**********************************************************
* Noise reduction
**********************************************************
*/
/*
*****************************
*
* Desc: set noise reduction mode
* Argument:
*   mode:
*     auto: auto noise reduction
*     manual：manual noise reduction
*
*****************************
*/
XCamReturn rk_aiq_uapi_setNRMode(const rk_aiq_sys_ctx_t* ctx, opMode_t mode)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    rk_aiq_nr_attrib_t attr;
    ret = rk_aiq_user_api_anr_GetAttrib(ctx, &attr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "get anr attrib failed!,ret=%d", ret);
    if (mode == OP_AUTO) {
        attr.eMode = ANR_OP_MODE_AUTO;
    } else if (mode == OP_MANUALl) {
        attr.eMode = ANR_OP_MODE_MANUAL;
    } else {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "Not supported mode!");
    }
    return rk_aiq_user_api_anr_SetAttrib(ctx, &attr);
}

XCamReturn rk_aiq_uapi_getNRMode(const rk_aiq_sys_ctx_t* ctx, opMode_t *mode)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    rk_aiq_nr_attrib_t attr;
    ret = rk_aiq_user_api_anr_GetAttrib(ctx, &attr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "get anr attrib failed!,ret=%d", ret);
    if (attr.eMode == ANR_OP_MODE_AUTO) {
        *mode = OP_AUTO;
    } else if (attr.eMode == ANR_OP_MODE_MANUAL) {
        *mode = OP_MANUALl;
    } else if (attr.eMode == ANR_OP_MODE_INVALID) {
        *mode = OP_INVAL;
    }
    return ret;
}

/*
*****************************
*
* Desc: set auto noise reduction strength
* Argument:
*   level: [0, 10]
*
*****************************
*/
XCamReturn rk_aiq_uapi_setANRStrth(const rk_aiq_sys_ctx_t* ctx, unsigned int level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    return ret;
}

XCamReturn rk_aiq_uapi_getANRStrth(const rk_aiq_sys_ctx_t* ctx, unsigned int *level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    return ret;
}

/*
*****************************
*
* Desc: set manual spatial noise reduction strength
*    this function is active for NR is manual mode
* Argument:
*   on: 1:on
*      0: off
*   level: [0, 10]
*
*****************************
*/
XCamReturn rk_aiq_uapi_setMSpaNRStrth(const rk_aiq_sys_ctx_t* ctx, bool on, unsigned int level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    return ret;
}

XCamReturn rk_aiq_uapi_getMSpaNRStrth(const rk_aiq_sys_ctx_t* ctx, bool *on, unsigned int *level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    return ret;
}

/*
*****************************
*
* Desc: set manual time noise reduction strength
*     this function is active for NR is manual mode
* Argument:
*   level: [0, 10]
*
*****************************
*/
XCamReturn rk_aiq_uapi_setMTNRStrth(const rk_aiq_sys_ctx_t* ctx, bool on, unsigned int level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    return ret;
}

XCamReturn rk_aiq_uapi_getMTNRStrth(const rk_aiq_sys_ctx_t* ctx, bool *on, unsigned int *level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    return ret;
}

/*
**********************************************************
* Dehazer
**********************************************************
*/
/*
*****************************
*
* Desc: set dehaze mode
* Argument:
*   mode:
*     auto: auto dehaze
*     manual：manual dehaze
*
*****************************
*/
XCamReturn rk_aiq_uapi_setDhzMode(const rk_aiq_sys_ctx_t* ctx, opMode_t mode)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error, setDhzMode failed!");
    }
    adehaze_sw_t attr;
    ret = rk_aiq_user_api_adehaze_getSwAttrib(ctx, &attr);
    if (mode == OP_AUTO)
        attr.mode = RK_AIQ_DEHAZE_MODE_AUTO;
    else if (mode == OP_MANUALl) {
        attr.mode = RK_AIQ_DEHAZE_MODE_MANUAL;
        attr.stManual.sw_dhaz_en = 1;
        attr.stManual.cfg_alpha = 1;
        attr.stManual.enhance_en = 0;
        attr.stManual.sw_dhaz_cfg_wt = 0.5;
        attr.stManual.sw_dhaz_cfg_air = 100;
        attr.stManual.sw_dhaz_cfg_tmax = 0.5;
        attr.stManual.sw_dhaz_cfg_gratio = 5;
    } else
        ret = XCAM_RETURN_ERROR_PARAM;
    RKAIQ_IMGPROC_CHECK_RET(ret, "param error, setDhzMode failed!");
    ret = rk_aiq_user_api_adehaze_setSwAttrib(ctx, attr);
    return ret;
}

XCamReturn rk_aiq_uapi_getDhzMode(const rk_aiq_sys_ctx_t* ctx, opMode_t *mode)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error, setDhzMode failed!");
    }
    adehaze_sw_t attr;
    ret = rk_aiq_user_api_adehaze_getSwAttrib(ctx, &attr);
    if(attr.mode == RK_AIQ_DEHAZE_MODE_AUTO)
        *mode = OP_AUTO;
    else if(attr.mode == RK_AIQ_DEHAZE_MODE_MANUAL)
        *mode = OP_MANUALl;
    return ret;
}

/*
*****************************
*
* Desc: set manual dehaze strength
*     this function is active for dehaze is manual mode
* Argument:
*   level: [0, 10]
*
*****************************
*/
XCamReturn rk_aiq_uapi_setMDhzStrth(const rk_aiq_sys_ctx_t* ctx, bool on, unsigned int level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    if (level < 0 || level > 10) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error, setMDhzStrth failed!");
    }
    switch (level) {
    case 0:
        break;
    case 1:
        break;
    case 2:
        break;
    //3,4,5
    default:
        break;
    }
    return ret;
}

XCamReturn rk_aiq_uapi_getMDhzStrth(const rk_aiq_sys_ctx_t* ctx, bool *on, unsigned int *level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    return ret;
}

/*
**********************************************************
* Image adjust
**********************************************************
*/

/*
*****************************
*
* Desc: Adjust image contrast level
* Argument:
*    level: contrast level, [0, 100]
*****************************
*/
XCamReturn rk_aiq_uapi_setContrast(const rk_aiq_sys_ctx_t* ctx, unsigned int level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    return ret;
}

XCamReturn rk_aiq_uapi_getContrast(const rk_aiq_sys_ctx_t* ctx, unsigned int *level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    return ret;
}

/*
*****************************
*
* Desc: Adjust image brightness level
* Argument:
*    level: contrast level, [0, 100]
*****************************
*/
XCamReturn rk_aiq_uapi_setBrightness(const rk_aiq_sys_ctx_t* ctx, unsigned int level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    return ret;
}

XCamReturn rk_aiq_uapi_getBrightness(const rk_aiq_sys_ctx_t* ctx, unsigned int *level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    return ret;
}

/*
*****************************
*
* Desc: Adjust image saturation level
* Argument:
*    level: contrast level, [0, 100]
*****************************
*/
XCamReturn rk_aiq_uapi_setSaturation(const rk_aiq_sys_ctx_t* ctx, float level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    rk_aiq_ccm_attrib_t ccm;
    rk_aiq_user_api_accm_GetAttrib(ctx, &ccm);
    for(int i = 0; i < RK_AIQ_ACCM_COLOR_GAIN_NUM; i++) {
        ccm.stAuto.color_saturation.sensorGain[i] = 2.0f;
        ccm.stAuto.color_saturation.level[i] = level;
    }
    return rk_aiq_user_api_accm_SetAttrib(ctx, ccm);
}

XCamReturn rk_aiq_uapi_getSaturation(const rk_aiq_sys_ctx_t* ctx, float* level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    rk_aiq_ccm_querry_info_t ccm_querry_info;
    rk_aiq_user_api_accm_QueryCcmInfo(ctx, &ccm_querry_info);
    *level = ccm_querry_info.color_saturation_level;

    return ret;
}

/*
*****************************
*
* Desc: Adjust image sharpness level
* Argument:
*    level: contrast level, [0, 100]
*****************************
*/
XCamReturn rk_aiq_uapi_setSharpness(const rk_aiq_sys_ctx_t* ctx, unsigned int level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    rk_aiq_sharp_attrib_t attr;
    rk_aiq_user_api_asharp_SetAttrib(ctx, &attr);
    return ret;
}

XCamReturn rk_aiq_uapi_getSharpness(const rk_aiq_sys_ctx_t* ctx, unsigned int *level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    return ret;
}

RKAIQ_END_DECLARE
