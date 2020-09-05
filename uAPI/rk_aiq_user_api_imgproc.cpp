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

#define IMGPROC_FUNC_ENTER LOGD("%s: enter", __FUNCTION__);
#define IMGPROC_FUNC_EXIT LOGD("%s: exit", __FUNCTION__);

RKAIQ_BEGIN_DECLARE

static bool isHDRmode(const rk_aiq_sys_ctx_t* ctx)
{
    int mode = ctx->_analyzer->mAlogsSharedParams.working_mode;
    if (RK_AIQ_WORKING_MODE_NORMAL == mode)
        return false;
    else
        return true;
}

static int getHDRFrameNum(const rk_aiq_sys_ctx_t* ctx)
{
    int FrameNum = 1;
    switch (ctx->_analyzer->mAlogsSharedParams.working_mode)
    {
    case RK_AIQ_WORKING_MODE_NORMAL:
        FrameNum = 1;
        break;
    case RK_AIQ_ISP_HDR_MODE_2_FRAME_HDR:
    case RK_AIQ_ISP_HDR_MODE_2_LINE_HDR:
        FrameNum = 2;
        break;
    case RK_AIQ_ISP_HDR_MODE_3_FRAME_HDR:
    case RK_AIQ_ISP_HDR_MODE_3_LINE_HDR:
        FrameNum = 3;
        break;
    default:
        FrameNum = 1;
        break;
    }
    return FrameNum;
}

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
    IMGPROC_FUNC_ENTER
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
    } else if (mode == OP_MANUAL) {
        expSwAttr.AecOpType = RK_AIQ_OP_MODE_MANUAL;
        //if not set, use default value in iq; if set, use user value
        expSwAttr.stManual.stLinMe.GainValue = 4.0f;
        expSwAttr.stManual.stLinMe.TimeValue = 0.02f;

    } else {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "mode is not supported!");
    }
    ret = rk_aiq_user_api_ae_setExpSwAttr(ctx, expSwAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "set exp attr failed!\nset exp mode failed!");
    IMGPROC_FUNC_EXIT
    return (ret);
}

XCamReturn rk_aiq_uapi_getExpMode(const rk_aiq_sys_ctx_t* ctx, opMode_t *mode)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    Uapi_ExpSwAttr_t expSwAttr;
    IMGPROC_FUNC_ENTER
    if ((ctx == NULL) || (mode == NULL)) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error!");
    }

    ret = rk_aiq_user_api_ae_getExpSwAttr(ctx, &expSwAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "get exp attr failed!\nget exp mode failed!");
    if (expSwAttr.AecOpType == RK_AIQ_OP_MODE_AUTO) {
        *mode = OP_AUTO;
    } else if (expSwAttr.AecOpType == RK_AIQ_OP_MODE_MANUAL) {
        *mode = OP_MANUAL;
    }
    IMGPROC_FUNC_EXIT
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
    IMGPROC_FUNC_ENTER
    if ((ctx == NULL) || (gain == NULL)) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error!");
    }
    LOGD("set range: [%f, %f]", gain->min, gain->max);
    //if (gain->min < 1.0f) gain->min = 1.0f;
    //if (gain->max < 1.0f) gain->max = 1.0f;
    ret = rk_aiq_user_api_ae_getExpSwAttr(ctx, &expSwAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "get exp attr failed!\nsetExpGainRange failed!");
    LOGD(">>gainRange[0]: [%f, %f]", expSwAttr.stAuto.stHdrAeRange.stGainRange[0].Min, expSwAttr.stAuto.stHdrAeRange.stGainRange[0].Max);
    LOGD(">>gainRange[1]: [%f, %f]", expSwAttr.stAuto.stHdrAeRange.stGainRange[1].Min, expSwAttr.stAuto.stHdrAeRange.stGainRange[1].Max);
    LOGD(">>gainRange[2]: [%f, %f]", expSwAttr.stAuto.stHdrAeRange.stGainRange[2].Min, expSwAttr.stAuto.stHdrAeRange.stGainRange[2].Max);

    expSwAttr.stAuto.SetAeRangeEn = true;
    if (isHDRmode(ctx)) {
        expSwAttr.stAuto.stHdrAeRange.stGainRange[0].Max = gain->max;
        //expSwAttr.stAuto.stHdrAeRange.stGainRange[0].Min = gain->min;
        expSwAttr.stAuto.stHdrAeRange.stGainRange[1].Max = gain->max;
        //expSwAttr.stAuto.stHdrAeRange.stGainRange[1].Min = gain->min;
        expSwAttr.stAuto.stHdrAeRange.stGainRange[2].Max = gain->max;
        //expSwAttr.stAuto.stHdrAeRange.stGainRange[2].Min = gain->min;
    } else {
        expSwAttr.stAuto.stLinAeRange.stGainRange.Max = gain->max;
        //expSwAttr.stAuto.stLinAeRange.stGainRange.Min = gain->min;
    }

    ret = rk_aiq_user_api_ae_setExpSwAttr(ctx, expSwAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "set exp attr failed!\nsetExpGainRange failed!");
    IMGPROC_FUNC_EXIT
    return (ret);
}

XCamReturn rk_aiq_uapi_getExpGainRange(const rk_aiq_sys_ctx_t* ctx, paRange_t *gain)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    Uapi_ExpSwAttr_t expSwAttr;
    IMGPROC_FUNC_ENTER
    if ((ctx == NULL) || (gain == NULL)) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error!");
    }

    ret = rk_aiq_user_api_ae_getExpSwAttr(ctx, &expSwAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "get exp attr failed!\ngetExpGainRange failed!");
    if (isHDRmode(ctx)) {
        int index = getHDRFrameNum(ctx);
        gain->max = expSwAttr.stAuto.stHdrAeRange.stGainRange[index - 1].Max;
        gain->min = expSwAttr.stAuto.stHdrAeRange.stGainRange[index - 1].Min;
    } else {
        gain->max = expSwAttr.stAuto.stLinAeRange.stGainRange.Max;
        gain->min = expSwAttr.stAuto.stLinAeRange.stGainRange.Min;
    }

    IMGPROC_FUNC_EXIT
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
    IMGPROC_FUNC_ENTER
    if ((ctx == NULL) || (time == NULL)) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error!");
    }
    LOGD("set range: [%f, %f]", time->min, time->max);
    ret = rk_aiq_user_api_ae_getExpSwAttr(ctx, &expSwAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "get exp attr failed!\nsetExpTimeRange failed!");
    LOGD(">>timeRange[0]: [%f, %f]", expSwAttr.stAuto.stHdrAeRange.stExpTimeRange[0].Min, expSwAttr.stAuto.stHdrAeRange.stExpTimeRange[0].Max);
    LOGD(">>timeRange[1]: [%f, %f]", expSwAttr.stAuto.stHdrAeRange.stExpTimeRange[1].Min, expSwAttr.stAuto.stHdrAeRange.stExpTimeRange[1].Max);
    LOGD(">>timeRange[2]: [%f, %f]", expSwAttr.stAuto.stHdrAeRange.stExpTimeRange[2].Min, expSwAttr.stAuto.stHdrAeRange.stExpTimeRange[2].Max);

    expSwAttr.stAuto.SetAeRangeEn = true;
    if (isHDRmode(ctx)) {
        expSwAttr.stAuto.stHdrAeRange.stExpTimeRange[0].Max = time->max;
        //expSwAttr.stAuto.stHdrAeRange.stExpTimeRange[0].Min = time->min;
        expSwAttr.stAuto.stHdrAeRange.stExpTimeRange[1].Max = time->max;
        //expSwAttr.stAuto.stHdrAeRange.stExpTimeRange[1].Min = time->min;
        expSwAttr.stAuto.stHdrAeRange.stExpTimeRange[2].Max = time->max;
        //expSwAttr.stAuto.stHdrAeRange.stExpTimeRange[2].Min = time->min;
    } else {
        expSwAttr.stAuto.stLinAeRange.stExpTimeRange.Max = time->max;
        //expSwAttr.stAuto.stLinAeRange.stExpTimeRange.Min = time->min;
    }
    ret = rk_aiq_user_api_ae_setExpSwAttr(ctx, expSwAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "set exp attr failed!\nsetExpTimeRange failed!");
    IMGPROC_FUNC_EXIT
    return (ret);
}

XCamReturn rk_aiq_uapi_getExpTimeRange(const rk_aiq_sys_ctx_t* ctx, paRange_t *time)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    Uapi_ExpSwAttr_t expSwAttr;
    IMGPROC_FUNC_ENTER
    if ((ctx == NULL) || (time == NULL)) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error!");
    }

    ret = rk_aiq_user_api_ae_getExpSwAttr(ctx, &expSwAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "get exp attr failed!\ngetExpTimeRange failed!");
    if (isHDRmode(ctx)) {
        int index = getHDRFrameNum(ctx);
        time->max = expSwAttr.stAuto.stHdrAeRange.stExpTimeRange[index - 1].Max;
        time->min = expSwAttr.stAuto.stHdrAeRange.stExpTimeRange[index - 1].Min;
    } else {
        time->max = expSwAttr.stAuto.stLinAeRange.stExpTimeRange.Max;
        time->min = expSwAttr.stAuto.stLinAeRange.stExpTimeRange.Min;
    }

    IMGPROC_FUNC_EXIT
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
* Desc: backlight compensation
* Argument:
*      on:  1  on
*           0  off
*      areaType: backlight compensation area
*
*****************************
*/
XCamReturn rk_aiq_uapi_setBLCMode(const rk_aiq_sys_ctx_t* ctx, bool on, aeMeasAreaType_t areaType)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    Uapi_LinExpAttr_t lineExpAttr;
    IMGPROC_FUNC_ENTER
    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error!");
    }
    if (isHDRmode(ctx)) {
        ret = XCAM_RETURN_ERROR_FAILED;
        RKAIQ_IMGPROC_CHECK_RET(ret, "Not support in HDR mode!");
    }
    ret = rk_aiq_user_api_ae_getLinExpAttr(ctx, &lineExpAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "getLinExpAttr error!");
    lineExpAttr.BackLightConf.enable = on ? 1 : 0;
    lineExpAttr.BackLightConf.MeasArea = (CalibDb_AecMeasAreaMode_t)areaType;
    lineExpAttr.BackLightConf.StrBias = 0;
    ret = rk_aiq_user_api_ae_setLinExpAttr(ctx, lineExpAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "setBLCMode error!");
    IMGPROC_FUNC_EXIT
    return ret;
}

/*
*****************************
*
* Desc: backlight compensation strength,only available in normal mode
* Argument:
*      strength:  [1,100]
*****************************
*/
XCamReturn rk_aiq_uapi_setBLCStrength(const rk_aiq_sys_ctx_t* ctx, int strength)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    Uapi_LinExpAttr_t lineExpAttr;
    IMGPROC_FUNC_ENTER
    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error!");
    }
    if (isHDRmode(ctx)) {
        ret = XCAM_RETURN_ERROR_FAILED;
        RKAIQ_IMGPROC_CHECK_RET(ret, "Not support in HDR mode!");
    }else {
        ret = rk_aiq_user_api_ae_getLinExpAttr(ctx, &lineExpAttr);
        RKAIQ_IMGPROC_CHECK_RET(ret, "getLinExpAttr error!");
        if(0 == lineExpAttr.BackLightConf.enable)
            RKAIQ_IMGPROC_CHECK_RET(ret, "blc mode is not enabled!");
        lineExpAttr.BackLightConf.StrBias = strength;
        ret = rk_aiq_user_api_ae_setLinExpAttr(ctx, lineExpAttr);
        RKAIQ_IMGPROC_CHECK_RET(ret, "setBLCStrength error!");
    }

    IMGPROC_FUNC_EXIT
    return ret;
}

/*
*****************************
*
* Desc: highlight compensation
* Argument:
*      on:  1  on
*           0  off
*****************************
*/
XCamReturn rk_aiq_uapi_setHLCMode(const rk_aiq_sys_ctx_t* ctx, bool on)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    Uapi_LinExpAttr_t linExpAttr;
    IMGPROC_FUNC_ENTER
    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error!");
    }

    if (isHDRmode(ctx)) {
        ret = XCAM_RETURN_ERROR_FAILED;
        RKAIQ_IMGPROC_CHECK_RET(ret, "Not support in HDR mode!");
    }else {
        ret = rk_aiq_user_api_ae_getLinExpAttr(ctx, &linExpAttr);
        RKAIQ_IMGPROC_CHECK_RET(ret, "get exp attr failed!\n setHLCMode failed!");
        linExpAttr.OverExpCtrl.enable = on ? 1 : 0;
        linExpAttr.OverExpCtrl.StrBias = 0;
        ret = rk_aiq_user_api_ae_setLinExpAttr(ctx, linExpAttr);
        RKAIQ_IMGPROC_CHECK_RET(ret, "set exp attr failed!\n setHLCMode failed!");
    }
    IMGPROC_FUNC_EXIT
    return ret;

}

/*
*****************************
*
* Desc: highlight compensation strength
* Argument:
*      strength:  [1,100]
*****************************
*/
XCamReturn rk_aiq_uapi_setHLCStrength(const rk_aiq_sys_ctx_t* ctx, int strength)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    Uapi_LinExpAttr_t lineExpAttr;
    IMGPROC_FUNC_ENTER
    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error!");
    }
    if (isHDRmode(ctx)) {
        ret = XCAM_RETURN_ERROR_FAILED;
        RKAIQ_IMGPROC_CHECK_RET(ret, "Not support in HDR mode!");
    }else {
        ret = rk_aiq_user_api_ae_getLinExpAttr(ctx, &lineExpAttr);
        RKAIQ_IMGPROC_CHECK_RET(ret, "getLinExpAttr error!");
        if(0 == lineExpAttr.OverExpCtrl.enable)
            RKAIQ_IMGPROC_CHECK_RET(ret, "hlc mode is not enabled!");
        lineExpAttr.OverExpCtrl.StrBias = strength;
        ret = rk_aiq_user_api_ae_setLinExpAttr(ctx, lineExpAttr);
        RKAIQ_IMGPROC_CHECK_RET(ret, "setHLCStrength error!");
    }
    IMGPROC_FUNC_EXIT
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
*****************************
*/
XCamReturn rk_aiq_uapi_setLExpMode(const rk_aiq_sys_ctx_t* ctx, opMode_t mode)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    IMGPROC_FUNC_ENTER
    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error!");
    }
    Uapi_LinExpAttr_t linExpAttr;
    ret = rk_aiq_user_api_ae_getLinExpAttr(ctx, &linExpAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "setLExpMode failed!");
    if (mode == OP_MANUAL) {
        linExpAttr.StrategyMode = RKAIQ_AEC_STRATEGY_MODE_LOWLIGHT_PRIOR;
    } else if (mode == OP_AUTO) {
        linExpAttr.StrategyMode = RKAIQ_AEC_STRATEGY_MODE_AUTO;
    } else {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "mode is invalid!");
    }
    ret = rk_aiq_user_api_ae_setLinExpAttr(ctx, linExpAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "setLExpMode failed!");
    IMGPROC_FUNC_EXIT
    return ret;
}

XCamReturn rk_aiq_uapi_getLExpMode(const rk_aiq_sys_ctx_t* ctx, opMode_t *mode)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    IMGPROC_FUNC_ENTER
    if ((ctx == NULL) || (mode == NULL)) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error!");
    }
    Uapi_LinExpAttr_t linExpAttr;
    ret = rk_aiq_user_api_ae_getLinExpAttr(ctx, &linExpAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "getLExpMode failed!");
    if (linExpAttr.StrategyMode == RKAIQ_AEC_STRATEGY_MODE_LOWLIGHT_PRIOR)
        *mode = OP_MANUAL;
    else if (linExpAttr.StrategyMode == RKAIQ_AEC_STRATEGY_MODE_AUTO)
        *mode = OP_AUTO;
    IMGPROC_FUNC_EXIT
    return ret;

}


/*
*****************************
*
* Desc: set manual lowlight exposure time ratio
* Argument:
*    ratio:  [1.0, 128.0]
* Not support
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
    IMGPROC_FUNC_ENTER
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
    } else {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "mode is invalid!");
    }
    ret = rk_aiq_user_api_ae_setExpSwAttr(ctx, expSwAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "set exp attr failed!\nsetAntiFlickerMode failed!");
    IMGPROC_FUNC_EXIT
    return ret;
}

XCamReturn rk_aiq_uapi_getAntiFlickerMode(const rk_aiq_sys_ctx_t* ctx, antiFlickerMode_t *mode)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    Uapi_ExpSwAttr_t expSwAttr;
    IMGPROC_FUNC_ENTER
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
    IMGPROC_FUNC_EXIT
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
    IMGPROC_FUNC_ENTER
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
    IMGPROC_FUNC_EXIT
    return (ret);
}

XCamReturn rk_aiq_uapi_getExpPwrLineFreqMode(const rk_aiq_sys_ctx_t* ctx, expPwrLineFreq_t *freq)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    Uapi_ExpSwAttr_t expSwAttr;
    IMGPROC_FUNC_ENTER
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
    IMGPROC_FUNC_EXIT
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
    IMGPROC_FUNC_ENTER
    if (mode == OP_AUTO) {
        expSwAttr.DNTrigger = 1;
        rk_aiq_user_api_ae_setExpSwAttr(ctx, expSwAttr);
    } else if (mode == OP_MANUAL) {
        expSwAttr.DNTrigger = 0;
        expSwAttr.DNMode = AEC_DNMODE_DAY;
        rk_aiq_user_api_ae_setExpSwAttr(ctx, expSwAttr);
    }
    IMGPROC_FUNC_EXIT
    return ret;
}


XCamReturn rk_aiq_uapi_getDayNSwMode(const rk_aiq_sys_ctx_t* ctx, opMode_t *mode)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    Uapi_ExpSwAttr_t expSwAttr;
    IMGPROC_FUNC_ENTER
    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error!");
    }

    ret = rk_aiq_user_api_ae_getExpSwAttr(ctx, &expSwAttr);
    if (expSwAttr.DNTrigger == 0)
        *mode = OP_MANUAL;
    else if (expSwAttr.DNTrigger == 1 || expSwAttr.DNTrigger == 2)
        *mode = OP_AUTO;
    IMGPROC_FUNC_EXIT
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
    IMGPROC_FUNC_ENTER
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
    IMGPROC_FUNC_EXIT
    return ret;
}

XCamReturn rk_aiq_uapi_getMDNScene(const rk_aiq_sys_ctx_t* ctx, dayNightScene_t *scene)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    Uapi_ExpSwAttr_t expSwAttr;
    IMGPROC_FUNC_ENTER
    ret = rk_aiq_user_api_ae_getExpSwAttr(ctx, &expSwAttr);
    if (expSwAttr.DNMode == AEC_DNMODE_DAY)
        *scene = DAYNIGHT_SCENE_DAY;
    else if(expSwAttr.DNMode == AEC_DNMODE_NIGHT)
        *scene = DAYNIGHT_SCENE_NIGHT;
    IMGPROC_FUNC_EXIT
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
    IMGPROC_FUNC_ENTER
    ret = rk_aiq_user_api_awb_GetAttrib(ctx, &attr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "setWBMode failed!");
    if (mode == OP_AUTO) {
        attr.mode = RK_AIQ_WB_MODE_AUTO;
    } else if (mode == OP_MANUAL) {
        attr.mode = RK_AIQ_WB_MODE_MANUAL;
        attr.stManual.mode = RK_AIQ_MWB_MODE_SCENE;
        attr.stManual.para.scene =  RK_AIQ_WBCT_DAYLIGHT;
    } else {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "Not supported mode!");
    }
    ret = rk_aiq_user_api_awb_SetAttrib(ctx, attr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "setWbMode failed!");
    IMGPROC_FUNC_EXIT
    return ret;
}

XCamReturn rk_aiq_uapi_getWBMode(const rk_aiq_sys_ctx_t* ctx, opMode_t *mode)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    rk_aiq_wb_attrib_t attr;
    IMGPROC_FUNC_ENTER
    ret = rk_aiq_user_api_awb_GetAttrib(ctx, &attr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "getWBMode failed!");
    if (attr.mode == RK_AIQ_WB_MODE_AUTO) {
        *mode = OP_AUTO;
    } else if (attr.mode == RK_AIQ_WB_MODE_MANUAL) {
        *mode = OP_MANUAL;
    } else {
        *mode = OP_INVAL;
    }
    IMGPROC_FUNC_EXIT
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
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    IMGPROC_FUNC_ENTER
    ret = rk_aiq_user_api_awb_Lock(ctx);
    IMGPROC_FUNC_EXIT
    return ret;
}

XCamReturn rk_aiq_uapi_unlockAWB(const rk_aiq_sys_ctx_t* ctx)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    IMGPROC_FUNC_ENTER
    ret = rk_aiq_user_api_awb_Unlock(ctx);
    IMGPROC_FUNC_EXIT
    return ret;
}



/*
*****************************
*
* Desc: set auto white balance mode
* Argument:
*
* Not support
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
    IMGPROC_FUNC_ENTER
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
    IMGPROC_FUNC_EXIT
    return ret;
}

XCamReturn rk_aiq_uapi_getMWBScene(const rk_aiq_sys_ctx_t* ctx, rk_aiq_wb_scene_t *scene)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    rk_aiq_wb_attrib_t attr;
    IMGPROC_FUNC_ENTER
    if ((ctx == NULL) || (scene == NULL)) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error, getMWBScene failed!");
    }
    //attr.mode = RK_AIQ_WB_MODE_MANUAL;
    attr.stManual.mode = RK_AIQ_MWB_MODE_SCENE;
    ret = rk_aiq_user_api_awb_GetAttrib(ctx, &attr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "getMWBScene failed!");
    *scene = attr.stManual.para.scene;
    IMGPROC_FUNC_EXIT
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
    IMGPROC_FUNC_ENTER
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
    IMGPROC_FUNC_EXIT
    return ret;
}

XCamReturn rk_aiq_uapi_getMWBGain(const rk_aiq_sys_ctx_t* ctx, rk_aiq_wb_gain_t *gain)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    rk_aiq_wb_querry_info_t query_info;
    IMGPROC_FUNC_ENTER
    if ((ctx == NULL) || (gain == NULL)) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error, getMWBGain failed!");
    }
    ret = rk_aiq_user_api_awb_QueryWBInfo(ctx, &query_info);
    RKAIQ_IMGPROC_CHECK_RET(ret, "getMWBGain failed!");
    *gain = query_info.gain;
    IMGPROC_FUNC_EXIT

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
XCamReturn rk_aiq_uapi_setMWBCT(const rk_aiq_sys_ctx_t* ctx, unsigned int ct)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    rk_aiq_wb_attrib_t attr;
    IMGPROC_FUNC_ENTER
    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error, setMWBCT failed!");
    }

    ret = rk_aiq_user_api_awb_GetAttrib(ctx, &attr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "getMWBCT failed!");
    attr.mode = RK_AIQ_WB_MODE_MANUAL;
    attr.stManual.mode = RK_AIQ_MWB_MODE_CCT;
    attr.stManual.para.cct.CCT = (float)ct;
    attr.stManual.para.cct.CCRI = 0.0f;
    ret = rk_aiq_user_api_awb_SetAttrib(ctx, attr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "setMWBCT failed!");
    IMGPROC_FUNC_EXIT
    return ret;

}

XCamReturn rk_aiq_uapi_getMWBCT(const rk_aiq_sys_ctx_t* ctx, unsigned int *ct)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    rk_aiq_wb_cct_t cct;
    IMGPROC_FUNC_ENTER
    if ((ctx == NULL) || (ct == NULL)) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error, getMWBCT failed!");
    }

    ret = rk_aiq_user_api_awb_GetCCT(ctx, &cct);
    RKAIQ_IMGPROC_CHECK_RET(ret, "getMWBCT failed!");
    *ct = (unsigned int)cct.CCT;
    IMGPROC_FUNC_EXIT
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
    IMGPROC_FUNC_ENTER
    rk_aiq_ccm_attrib_t ccm;
    rk_aiq_user_api_accm_GetAttrib(ctx, &ccm);
    for(int i = 0; i < RK_AIQ_ACCM_COLOR_GAIN_NUM; i++) {
        ccm.stAuto.color_inhibition.sensorGain[i] = 2.0f;
        ccm.stAuto.color_inhibition.level[i] = level;
    }
    ret = rk_aiq_user_api_accm_SetAttrib(ctx, ccm);
    IMGPROC_FUNC_EXIT
    return ret;
}


XCamReturn rk_aiq_uapi_getCrSuppsn(const rk_aiq_sys_ctx_t* ctx, unsigned int *level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    IMGPROC_FUNC_ENTER
    rk_aiq_ccm_querry_info_t ccm_querry_info;
    ret = rk_aiq_user_api_accm_QueryCcmInfo(ctx, &ccm_querry_info);
    *level = (int)ccm_querry_info.color_inhibition_level;
    IMGPROC_FUNC_EXIT
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

    rk_aiq_af_attrib_t attr;
    ret = rk_aiq_user_api_af_GetAttrib(ctx, &attr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "setFocusMode failed!");
    if (mode == OP_AUTO) {
        attr.AfMode = RKAIQ_AF_MODE_CONTINUOUS_PICTURE;
    } else if (mode == OP_MANUAL) {
        attr.AfMode = RKAIQ_AF_MODE_FIXED;
    } else {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "Not supported mode!");
    }

    ret = rk_aiq_user_api_af_SetAttrib(ctx, attr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "setFocusMode failed!");
    return ret;
}

XCamReturn rk_aiq_uapi_getFocusMode(const rk_aiq_sys_ctx_t* ctx, opMode_t *mode)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    rk_aiq_af_attrib_t attr;
    ret = rk_aiq_user_api_af_GetAttrib(ctx, &attr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "getFocusMode failed!");
    if (attr.AfMode == RKAIQ_AF_MODE_FIXED) {
        *mode = OP_MANUAL;
    } else if (attr.AfMode == RKAIQ_AF_MODE_NOT_SET) {
        *mode = OP_INVAL;
    } else {
        *mode = OP_AUTO;
    }

    return ret;
}

/*
*****************************
*
* Desc: set fixed mode code
* Argument:
*
*****************************
*/
XCamReturn rk_aiq_uapi_setFixedModeCode(const rk_aiq_sys_ctx_t* ctx, unsigned short code)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    rk_aiq_af_attrib_t attr;

    IMGPROC_FUNC_ENTER
    ret = rk_aiq_user_api_af_GetAttrib(ctx, &attr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "setFixedModeCode failed!");
    attr.fixedModeDefCode = code;
    ret = rk_aiq_user_api_af_SetAttrib(ctx, attr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "setFixedModeCode failed!");
    IMGPROC_FUNC_EXIT
    return ret;
}

XCamReturn rk_aiq_uapi_getFixedModeCode(const rk_aiq_sys_ctx_t* ctx, unsigned short *code)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    rk_aiq_af_attrib_t attr;
    IMGPROC_FUNC_ENTER
    ret = rk_aiq_user_api_af_GetAttrib(ctx, &attr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "getFixedModeCode failed!");
    *code = attr.fixedModeDefCode;
    IMGPROC_FUNC_EXIT
    return ret;
}

/*
*****************************
*
* Desc: set focus window
* Argument:
*
*****************************
*/

XCamReturn rk_aiq_uapi_setFocusWin(const rk_aiq_sys_ctx_t* ctx, paRect_t *rect)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    rk_aiq_af_attrib_t attr;
    IMGPROC_FUNC_ENTER
    ret = rk_aiq_user_api_af_GetAttrib(ctx, &attr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "setFocusWin failed!");

    attr.h_offs = rect->x;
    attr.v_offs = rect->y;
    attr.h_size = rect->w;
    attr.v_size = rect->h;
    ret = rk_aiq_user_api_af_SetAttrib(ctx, attr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "setFocusWin failed!");
    IMGPROC_FUNC_EXIT
    return ret;
}

XCamReturn rk_aiq_uapi_getFocusWin(const rk_aiq_sys_ctx_t* ctx, paRect_t *rect)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    rk_aiq_af_attrib_t attr;
    IMGPROC_FUNC_ENTER
    ret = rk_aiq_user_api_af_GetAttrib(ctx, &attr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "getFocusWin failed!");

    rect->x = attr.h_offs;
    rect->y = attr.v_offs;
    rect->w = attr.h_size;
    rect->h = attr.v_size;
    IMGPROC_FUNC_EXIT
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
    IMGPROC_FUNC_ENTER
    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error, setHDRMode failed!");
    }
    if (!isHDRmode(ctx)) {
        ret = XCAM_RETURN_ERROR_FAILED;
        RKAIQ_IMGPROC_CHECK_RET(ret, "not in HDR mode!");
    }
    ret = rk_aiq_user_api_ahdr_GetAttrib(ctx, &attr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "get hdr attr failed!");
    attr.bEnable = true;
    if (mode == OP_AUTO) {
        attr.opMode = HDR_OpMode_Auto;
    } else if (mode == OP_MANUAL) {
        attr.opMode = HDR_OpMode_MANU;
    } else {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "Not supported mode!");
    }
    IMGPROC_FUNC_EXIT
    return rk_aiq_user_api_ahdr_SetAttrib(ctx, attr);
}

XCamReturn rk_aiq_uapi_getHDRMode(const rk_aiq_sys_ctx_t* ctx, opMode_t *mode)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    ahdr_attrib_t attr;
    IMGPROC_FUNC_ENTER
    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error, getHDRMode failed!");
    }
    if (!isHDRmode(ctx)) {
        ret = XCAM_RETURN_ERROR_FAILED;
        RKAIQ_IMGPROC_CHECK_RET(ret, "not in HDR mode!");
    }
    ret = rk_aiq_user_api_ahdr_GetAttrib(ctx, &attr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "get hdr attr failed!");

    if (attr.opMode == HDR_OpMode_Auto)
        *mode =  OP_AUTO;
    else if (attr.opMode == HDR_OpMode_MANU)
        *mode = OP_MANUAL;
    IMGPROC_FUNC_EXIT
    return ret;
}

/*
*****************************
*
* Desc: set manual hdr strength
*    this function is active for HDR is manual mode
* Argument:
*   level: [1, 100]
*
*****************************
*/
XCamReturn rk_aiq_uapi_setMHDRStrth(const rk_aiq_sys_ctx_t* ctx, bool on, unsigned int level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    ahdr_attrib_t attr;
    IMGPROC_FUNC_ENTER
    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "ctx is null, setMHDRStrth failed!");
    }

    if (!isHDRmode(ctx)) {
        ret = XCAM_RETURN_ERROR_FAILED;
        RKAIQ_IMGPROC_CHECK_RET(ret, "not in HDR mode!");
    }
    if (level < 1 || level > 100) {
        ret = XCAM_RETURN_ERROR_OUTOFRANGE;
        RKAIQ_IMGPROC_CHECK_RET(ret, "level(%d) is out of range, setMHDRStrth failed!");
    }

    attr.level = level;
    attr.opMode = HDR_OpMode_Fast;
    attr.bEnable = true;//enable api set
    ret = rk_aiq_user_api_ahdr_SetAttrib(ctx, attr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "setMHDRStrth failed!");
    IMGPROC_FUNC_EXIT
    return ret;
}


XCamReturn rk_aiq_uapi_getMHDRStrth(const rk_aiq_sys_ctx_t* ctx, bool *on, unsigned int *level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    ahdr_attrib_t attr;
    IMGPROC_FUNC_ENTER
    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "ctx is null, getMHDRStrth failed!");
    }
    if (!isHDRmode(ctx)) {
        ret = XCAM_RETURN_ERROR_FAILED;
        RKAIQ_IMGPROC_CHECK_RET(ret, "not in HDR mode!");
    }
    ret = rk_aiq_user_api_ahdr_GetAttrib(ctx, &attr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "get hdr attr failed!");

    *level = attr.level;
    IMGPROC_FUNC_EXIT
    return ret;
}

/*
*****************************
*
* Desc: set dark area boost strength
*    this function is active for normal mode
* Argument:
*   level: [1, 10]
*
*****************************
*/
XCamReturn rk_aiq_uapi_getDarkAreaBoostStrth(const rk_aiq_sys_ctx_t* ctx, unsigned int *level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    ahdr_attrib_t attr;
    IMGPROC_FUNC_ENTER
    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "ctx is null, getMHDRStrth failed!");
    }
    if (isHDRmode(ctx)) {
        ret = XCAM_RETURN_ERROR_FAILED;
        RKAIQ_IMGPROC_CHECK_RET(ret, "Not valid in HDR mode!");
    }
    ret = rk_aiq_user_api_ahdr_GetAttrib(ctx, &attr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "get hdr attr failed!");
    if (attr.opMode == HDR_OpMode_LINEAR)
        *level = attr.level_Linear_Dark;
    else
        *level = 0;
    IMGPROC_FUNC_EXIT
    return ret;
}

/*
*****************************
*
* Desc: get dark area boost strength
*    this function is active for normal mode
* Argument:
*   level: [1, 10]
*
*****************************
*/
XCamReturn rk_aiq_uapi_setDarkAreaBoostStrth(const rk_aiq_sys_ctx_t* ctx, unsigned int level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    ahdr_attrib_t attr;
    IMGPROC_FUNC_ENTER
    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "ctx is null, setMHDRStrth failed!");
    }

    if (isHDRmode(ctx)) {
        ret = XCAM_RETURN_ERROR_FAILED;
        RKAIQ_IMGPROC_CHECK_RET(ret, "Not valid in HDR mode!");
    }
    if (level > 10) {
        ret = XCAM_RETURN_ERROR_OUTOFRANGE;
        RKAIQ_IMGPROC_CHECK_RET(ret, "level(%d) is out of range, setDarkAreaBoostStrth failed!");
    }
    attr.bEnable = true;
    attr.level_Linear_Dark = level;
    attr.opMode = HDR_OpMode_LINEAR;
    ret = rk_aiq_user_api_ahdr_SetAttrib(ctx, attr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "setDarkAreaBoostStrth failed!");
    IMGPROC_FUNC_EXIT
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
    IMGPROC_FUNC_ENTER
    ret = rk_aiq_user_api_anr_GetAttrib(ctx, &attr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "get anr attrib failed!,ret=%d", ret);
    if (mode == OP_AUTO) {
        attr.eMode = ANR_OP_MODE_AUTO;
    } else if (mode == OP_MANUAL) {
        attr.eMode = ANR_OP_MODE_MANUAL;
    } else {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "Not supported mode!");
    }
    ret = rk_aiq_user_api_anr_SetAttrib(ctx, &attr);
    IMGPROC_FUNC_EXIT
    return ret;
}

XCamReturn rk_aiq_uapi_getNRMode(const rk_aiq_sys_ctx_t* ctx, opMode_t *mode)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    rk_aiq_nr_attrib_t attr;
    IMGPROC_FUNC_ENTER
    ret = rk_aiq_user_api_anr_GetAttrib(ctx, &attr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "get anr attrib failed!,ret=%d", ret);
    if (attr.eMode == ANR_OP_MODE_AUTO) {
        *mode = OP_AUTO;
    } else if (attr.eMode == ANR_OP_MODE_MANUAL) {
        *mode = OP_MANUAL;
    } else if (attr.eMode == ANR_OP_MODE_INVALID) {
        *mode = OP_INVAL;
    }
    IMGPROC_FUNC_EXIT
    return ret;
}

/*
*****************************
*
* Desc: set normal noise reduction strength
* Argument:
*   level: [0, 100]
* Normal mode
*****************************
*/
XCamReturn rk_aiq_uapi_setANRStrth(const rk_aiq_sys_ctx_t* ctx, unsigned int level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    IMGPROC_FUNC_ENTER
    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "ctx is null, setANRStrth failed!");
    }
    ret = rk_aiq_user_api_anr_SetLumaSFStrength(ctx, level / 100.0);
    ret = rk_aiq_user_api_anr_SetLumaTFStrength(ctx, level / 100.0);
    RKAIQ_IMGPROC_CHECK_RET(ret, "setANRStrth failed!", ret);
    IMGPROC_FUNC_EXIT
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
*   level: [0, 100]
*
*****************************
*/
XCamReturn rk_aiq_uapi_setMSpaNRStrth(const rk_aiq_sys_ctx_t* ctx, bool on, unsigned int level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    IMGPROC_FUNC_ENTER

    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "ctx is null, setMSpaNRStrth failed!");
    }
    ret = rk_aiq_user_api_anr_SetLumaSFStrength(ctx, level / 100.0);
    IMGPROC_FUNC_EXIT
    return ret;
}

/*
*****************************
*
* Desc: get manual spatial noise reduction strength
*    this function is active for NR is manual mode
* Argument:
*   level: [0, 100]
*
*****************************
*/
XCamReturn rk_aiq_uapi_getMSpaNRStrth(const rk_aiq_sys_ctx_t* ctx, bool *on, unsigned int *level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    float percent;
    IMGPROC_FUNC_ENTER
    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "ctx is null, getMSpaNRStrth failed!");
    }
    ret = rk_aiq_user_api_anr_GetLumaSFStrength(ctx, &percent);
    *level = (unsigned int)(percent * 100);
    IMGPROC_FUNC_EXIT
    return ret;
}

/*
*****************************
*
* Desc: set manual time noise reduction strength
*     this function is active for NR is manual mode
* Argument:
*   level: [0, 100]
*
*****************************
*/
XCamReturn rk_aiq_uapi_setMTNRStrth(const rk_aiq_sys_ctx_t* ctx, bool on, unsigned int level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    IMGPROC_FUNC_ENTER
    LOGD("level=%d", level);
    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "ctx is null, setMTNRStrth failed!");
    }
    ret = rk_aiq_user_api_anr_SetLumaTFStrength(ctx, level / 100.0);
    IMGPROC_FUNC_EXIT
    return ret;
}

/*
*****************************
*
* Desc: get manual time noise reduction strength
*     this function is active for NR is manual mode
* Argument:
*   level: [0, 100]
*
*****************************
*/
XCamReturn rk_aiq_uapi_getMTNRStrth(const rk_aiq_sys_ctx_t* ctx, bool *on, unsigned int *level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    float percent;
    IMGPROC_FUNC_ENTER
    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "ctx is null, getMTNRStrth failed!");
    }
    ret = rk_aiq_user_api_anr_GetLumaTFStrength(ctx, &percent);
    *level = (unsigned int)(percent * 100);
    IMGPROC_FUNC_EXIT
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
    adehaze_sw_t attr;
    IMGPROC_FUNC_ENTER
    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error, setDhzMode failed!");
    }

    if (mode == OP_AUTO) {
        attr.mode = RK_AIQ_DEHAZE_MODE_AUTO;
        attr.stAuto.sw_dhaz_en = 1;
        attr.stAuto.enhance_en = 0;
        attr.stAuto.cfg_alpha = 0;
    } else if (mode == OP_MANUAL) {
        rk_aiq_uapi_setMDhzStrth(ctx, true, 1);
    } else
        ret = XCAM_RETURN_ERROR_PARAM;
    RKAIQ_IMGPROC_CHECK_RET(ret, "param error, setDhzMode failed!");
    ret = rk_aiq_user_api_adehaze_setSwAttrib(ctx, attr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "setDhzMode failed!");
    IMGPROC_FUNC_EXIT
    return ret;
}

//not implemet
XCamReturn rk_aiq_uapi_getDhzMode(const rk_aiq_sys_ctx_t* ctx, opMode_t *mode)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error, getDhzMode failed!");
    }

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
    adehaze_sw_t attr;
    IMGPROC_FUNC_ENTER
    if (level < 0 || level > 10) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error, setMDhzStrth failed!");
    }
    attr.mode = RK_AIQ_DEHAZE_MODE_MANUAL;
    attr.stManual.sw_dhaz_en = 1;
    attr.stManual.enhance_en = 0;
    attr.stManual.cfg_alpha = 1;

    switch (level) {
    case 0:
        attr.stManual.sw_dhaz_cfg_wt = 0.1f;
        attr.stManual.sw_dhaz_cfg_air = 210;
        attr.stManual.sw_dhaz_cfg_tmax = 0.2f;
        attr.stManual.sw_dhaz_cfg_gratio = 2;
        break;
    case 1:
        attr.stManual.sw_dhaz_cfg_wt = 0.2f;
        attr.stManual.sw_dhaz_cfg_air = 220;
        attr.stManual.sw_dhaz_cfg_tmax = 0.2f;
        attr.stManual.sw_dhaz_cfg_gratio = 2;
        break;
    case 2:
        attr.stManual.sw_dhaz_cfg_wt = 0.3f;
        attr.stManual.sw_dhaz_cfg_air = 230;
        attr.stManual.sw_dhaz_cfg_tmax = 0.2f;
        attr.stManual.sw_dhaz_cfg_gratio = 2;
        break;
    case 3:
        attr.stManual.sw_dhaz_cfg_wt = 0.4f;
        attr.stManual.sw_dhaz_cfg_air = 240;
        attr.stManual.sw_dhaz_cfg_tmax = 0.2f;
        attr.stManual.sw_dhaz_cfg_gratio = 2;
        break;
    case 4:
        attr.stManual.sw_dhaz_cfg_wt = 0.5f;
        attr.stManual.sw_dhaz_cfg_air = 250;
        attr.stManual.sw_dhaz_cfg_tmax = 0.2f;
        attr.stManual.sw_dhaz_cfg_gratio = 2;
        break;
    case 5:
        attr.stManual.sw_dhaz_cfg_wt = 0.6f;
        attr.stManual.sw_dhaz_cfg_air = 260;
        attr.stManual.sw_dhaz_cfg_tmax = 0.2f;
        attr.stManual.sw_dhaz_cfg_gratio = 2;
        break;
    case 6:
        attr.stManual.sw_dhaz_cfg_wt = 0.7f;
        attr.stManual.sw_dhaz_cfg_air = 270;
        attr.stManual.sw_dhaz_cfg_tmax = 0.2f;
        attr.stManual.sw_dhaz_cfg_gratio = 2;
        break;
    case 7:
        attr.stManual.sw_dhaz_cfg_wt = 0.75f;
        attr.stManual.sw_dhaz_cfg_air = 275;
        attr.stManual.sw_dhaz_cfg_tmax = 0.2f;
        attr.stManual.sw_dhaz_cfg_gratio = 2;
        break;
    case 8:
        attr.stManual.sw_dhaz_cfg_wt = 0.8f;
        attr.stManual.sw_dhaz_cfg_air = 280;
        attr.stManual.sw_dhaz_cfg_tmax = 0.2f;
        attr.stManual.sw_dhaz_cfg_gratio = 2;
        break;
    case 9:
        attr.stManual.sw_dhaz_cfg_wt = 0.9f;
        attr.stManual.sw_dhaz_cfg_air = 290;
        attr.stManual.sw_dhaz_cfg_tmax = 0.2f;
        attr.stManual.sw_dhaz_cfg_gratio = 2;
        break;
    case 10:
        attr.stManual.sw_dhaz_cfg_wt = 0.95f;
        attr.stManual.sw_dhaz_cfg_air = 295;
        attr.stManual.sw_dhaz_cfg_tmax = 0.2f;
        attr.stManual.sw_dhaz_cfg_gratio = 2;
        break;
    default:
        attr.stManual.sw_dhaz_cfg_wt = 0.1f;
        attr.stManual.sw_dhaz_cfg_air = 210;
        attr.stManual.sw_dhaz_cfg_tmax = 0.2f;
        attr.stManual.sw_dhaz_cfg_gratio = 2;
        break;
    }
    ret = rk_aiq_user_api_adehaze_setSwAttrib(ctx, attr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "setMDhzStrth failed!");
    IMGPROC_FUNC_EXIT
    return ret;
}

XCamReturn rk_aiq_uapi_getMDhzStrth(const rk_aiq_sys_ctx_t* ctx, bool *on, unsigned int *level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    return ret;
}

XCamReturn rk_aiq_uapi_enableDhz(const rk_aiq_sys_ctx_t* ctx)
{
    return rk_aiq_uapi_setMDhzStrth(ctx, true, 1);
}

XCamReturn rk_aiq_uapi_disableDhz(const rk_aiq_sys_ctx_t* ctx)
{
    return rk_aiq_uapi_setMDhzStrth(ctx, true, 0);
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
    IMGPROC_FUNC_ENTER
    LOGD("setContrast enter, level=%d\n", level);
    adehaze_sw_t attr;
    if (level < 0 || level > 100) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error, setContrast failed!");
    }
    attr.mode = RK_AIQ_DEHAZE_MODE_ENHANCE;
    attr.stEnhance.sw_dhaz_en = 1;
    attr.stEnhance.enhance_en = 1;
    attr.stEnhance.cfg_alpha = 0;
    //level range is [1.0,2.5],we use [1.0,2.0]
    attr.stEnhance.level = 1.0 + level / 100.0f;
    ret = rk_aiq_user_api_adehaze_setSwAttrib(ctx, attr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "set contrast failed!");
    IMGPROC_FUNC_EXIT

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
*    level: brightness level, [0, 100]
*****************************
*/
XCamReturn rk_aiq_uapi_setBrightness(const rk_aiq_sys_ctx_t* ctx, unsigned int level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    Uapi_LinExpAttr_t linExpAttr;
    Uapi_HdrExpAttr_t hdrExpAttr;
    IMGPROC_FUNC_ENTER

    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "ctx is null, setBrightness failed!");
    }

    if (isHDRmode(ctx)) {
        ret = rk_aiq_user_api_ae_getHdrExpAttr(ctx, &hdrExpAttr);
        if (level > 50) {
            hdrExpAttr.Evbias = (level - 50) * 3.0f; //0~150%
        } else if(level < 50) {
            hdrExpAttr.Evbias = -((50 - level) * 3.0f); //-150%~0
        } else {
            hdrExpAttr.Evbias = 0.0f;
        }
        LOGD("hdr: evbias=%f", hdrExpAttr.Evbias);
        ret = rk_aiq_user_api_ae_setHdrExpAttr(ctx, hdrExpAttr);
    } else {
        ret = rk_aiq_user_api_ae_getLinExpAttr(ctx, &linExpAttr);
        if (level > 50) {
            linExpAttr.Evbias = (level - 50) * 3.0f; //0~150%
        } else if(level < 50) {
            linExpAttr.Evbias = -((50 - level) * 3.0f); //-150%~0
        } else {
            linExpAttr.Evbias = 0.0f;
        }
        ret = rk_aiq_user_api_ae_setLinExpAttr(ctx, linExpAttr);
    }
    IMGPROC_FUNC_EXIT
    return ret;
}

XCamReturn rk_aiq_uapi_getBrightness(const rk_aiq_sys_ctx_t* ctx, unsigned int *level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    Uapi_LinExpAttr_t linExpAttr;
    Uapi_HdrExpAttr_t hdrExpAttr;
    IMGPROC_FUNC_ENTER
    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "ctx is null, getBrightness failed!");
    }

    if (isHDRmode(ctx)) {
        ret = rk_aiq_user_api_ae_getHdrExpAttr(ctx, &hdrExpAttr);
        RKAIQ_IMGPROC_CHECK_RET(ret, "get hdr expattr failed!");
        //if (hdrExpAttr.Evbias < 0)
        //hdrExpAttr.Evbias = -hdrExpAttr.Evbias;
        *level = (unsigned int)(hdrExpAttr.Evbias / 3 + 50);
    } else {
        ret = rk_aiq_user_api_ae_getLinExpAttr(ctx, &linExpAttr);
        RKAIQ_IMGPROC_CHECK_RET(ret, "get line expattr failed!");
        //if (linExpAttr.Evbias < 0)
        //linExpAttr.Evbias = -linExpAttr.Evbias;
        *level = (unsigned int)(linExpAttr.Evbias / 3 + 50);
    }
    IMGPROC_FUNC_EXIT
    return ret;
}

/*
*****************************
*
* Desc: Adjust image saturation level
* Argument:
*    level: saturation level, [0, 100]
*****************************
*/
XCamReturn rk_aiq_uapi_setSaturation(const rk_aiq_sys_ctx_t* ctx, unsigned int level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    rk_aiq_ccm_attrib_t ccm;
    IMGPROC_FUNC_ENTER
    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "ctx is null, setSaturation failed!");
    }
    LOGD("setSaturation enter, level=%d\n", level);
    ret = rk_aiq_user_api_accm_GetAttrib(ctx, &ccm);
    RKAIQ_IMGPROC_CHECK_RET(ret, "get ccm attrib failed!");
    for(int i = 0; i < RK_AIQ_ACCM_COLOR_GAIN_NUM; i++) {
        ccm.stAuto.color_saturation.sensorGain[i] = 2.0f;
        ccm.stAuto.color_saturation.level[i] = (float)level;
    }
    IMGPROC_FUNC_EXIT
    ret = rk_aiq_user_api_accm_SetAttrib(ctx, ccm);
    RKAIQ_IMGPROC_CHECK_RET(ret, "set ccm attrib failed!");
    return ret;
}

XCamReturn rk_aiq_uapi_getSaturation(const rk_aiq_sys_ctx_t* ctx, unsigned int* level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    rk_aiq_ccm_querry_info_t ccm_querry_info;
    float flevel = 0.0f;
    IMGPROC_FUNC_ENTER
    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "ctx is null, getSaturation failed!");
    }
    ret = rk_aiq_user_api_accm_QueryCcmInfo(ctx, &ccm_querry_info);
    RKAIQ_IMGPROC_CHECK_RET(ret, "query ccm info failed!");
    flevel = ccm_querry_info.color_saturation_level;
    LOGD("getSaturation enter, flevel=%f\n", flevel);
    *level = (unsigned int)flevel;
    IMGPROC_FUNC_EXIT
    return ret;
}

/*
*****************************
*
* Desc: Adjust image sharpness level
* Argument:
*    level: sharpness level, [0, 100]
*****************************
*/
XCamReturn rk_aiq_uapi_setSharpness(const rk_aiq_sys_ctx_t* ctx, unsigned int level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    float fPercent = 0.0f;
    IMGPROC_FUNC_ENTER
    fPercent = level / 100.0f;
    ret = rk_aiq_user_api_asharp_SetStrength(ctx, fPercent);
    IMGPROC_FUNC_EXIT

    return ret;
}

XCamReturn rk_aiq_uapi_getSharpness(const rk_aiq_sys_ctx_t* ctx, unsigned int *level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    float fPercent = 0.0f;
    IMGPROC_FUNC_ENTER
    ret = rk_aiq_user_api_asharp_GetStrength(ctx, &fPercent);
    *level = (unsigned int)(fPercent * 100);
    IMGPROC_FUNC_EXIT
    return ret;
}

/*
*****************************
*
* Desc: Adjust image gamma level
* Argument:
*    level: gamma level, [0, 100]
*****************************
*/
XCamReturn rk_aiq_uapi_setGammaCoef(const rk_aiq_sys_ctx_t* ctx, unsigned int level)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    rk_aiq_gamma_attrib_t gammaAttr;
    float flevel = 100.0f / level, maxval = 4096.0f;
    int tmp_gamma = 0;
    IMGPROC_FUNC_ENTER
    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "ctx is null, setGammaCoef failed!");
    }
    LOGD("%s: flevel:%f", __FUNCTION__, flevel);
    ret = rk_aiq_user_api_agamma_GetAttrib(ctx, &gammaAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "get gamma attrib failed!");
    for(int i = 0; i < 33; i++) {
        tmp_gamma = (int)((pow((64 * i / maxval), flevel)) * 4096) + gammaAttr.gamma_out_offset;
        tmp_gamma = (tmp_gamma > 4095) ? 4095 : tmp_gamma;
        tmp_gamma = (tmp_gamma < 0) ? 0 : tmp_gamma;
        gammaAttr.gamma_table[i] = (uint16_t)(tmp_gamma);
        LOGD("gamma_table[%d]:%d", i, gammaAttr.gamma_table[i]);
    }

    gammaAttr.gamma_out_segnum = GAMMA_OUT_EQ_SEGMENT;
    ret = rk_aiq_user_api_agamma_SetAttrib(ctx, gammaAttr);
    IMGPROC_FUNC_EXIT
    return ret;
}

XCamReturn rk_aiq_uapi_setGrayMode(const rk_aiq_sys_ctx_t* ctx, rk_aiq_gray_mode_t mode)
{
    return ctx->_analyzer->setGrayMode(mode);
}

rk_aiq_gray_mode_t rk_aiq_uapi_getGrayMode(const rk_aiq_sys_ctx_t* ctx)
{
    return ctx->_analyzer->getGrayMode();
}

XCamReturn rk_aiq_uapi_setFrameRate(const rk_aiq_sys_ctx_t* ctx, frameRateInfo_t info)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    Uapi_ExpSwAttr_t expSwAttr;
    IMGPROC_FUNC_ENTER
    if ((ctx == NULL) || (info.mode < OP_AUTO || info.mode >= OP_INVAL)) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error!");
    }
    ret = rk_aiq_user_api_ae_getExpSwAttr(ctx, &expSwAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "get exp attr failed!\n setFrameRate failed!");
    if(info.mode == OP_AUTO) {
        expSwAttr.stAuto.stFrmRate.isFpsFix = false;
    } else if (info.mode == OP_MANUAL) {
        expSwAttr.stAuto.stFrmRate.isFpsFix = true;
        expSwAttr.stAuto.stFrmRate.FpsValue = info.fps;
    }
    ret = rk_aiq_user_api_ae_setExpSwAttr(ctx, expSwAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "set exp attr failed!\n setFrameRate failed!");
    IMGPROC_FUNC_EXIT
    return ret;
}

XCamReturn rk_aiq_uapi_getFrameRate(const rk_aiq_sys_ctx_t* ctx, frameRateInfo_t *info)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    Uapi_ExpSwAttr_t expSwAttr;
    IMGPROC_FUNC_ENTER
    if ((ctx == NULL) || (info == NULL)) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error!");
    }
    ret = rk_aiq_user_api_ae_getExpSwAttr(ctx, &expSwAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "get exp attr failed!\n getFrameRate failed!");
    if(expSwAttr.stAuto.stFrmRate.isFpsFix) {
        info->mode = OP_MANUAL;
        info->fps = expSwAttr.stAuto.stFrmRate.FpsValue;
    } else {
        info->mode = OP_AUTO;
        info->fps = expSwAttr.stAuto.stFrmRate.FpsValue;
    }
    IMGPROC_FUNC_EXIT
    return ret;
}

XCamReturn rk_aiq_uapi_setMirroFlip(const rk_aiq_sys_ctx_t* ctx, bool mirror, bool flip)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    IMGPROC_FUNC_ENTER
    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error!");
    }
    return ctx->_rkAiqManager->setMirrorFlip(mirror, flip);
}

XCamReturn rk_aiq_uapi_getMirrorFlip(const rk_aiq_sys_ctx_t* ctx, bool* mirror, bool* flip)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    IMGPROC_FUNC_ENTER
    if (ctx == NULL || mirror == NULL || flip == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error!");
    }
    return ctx->_rkAiqManager->getMirrorFlip(*mirror, *flip);
}

XCamReturn rk_aiq_uapi_setFecEn(const rk_aiq_sys_ctx_t* ctx, bool en)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    IMGPROC_FUNC_ENTER
    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error!");
    }
    rk_aiq_fec_attrib_t fecAttr;
    ret = rk_aiq_user_api_afec_GetAttrib(ctx, &fecAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "get fec attrib failed!");
    fecAttr.en = en;
    ret = rk_aiq_user_api_afec_SetAttrib(ctx, fecAttr);
    IMGPROC_FUNC_EXIT
    return ret;
}

XCamReturn rk_aiq_uapi_setFecBypass(const rk_aiq_sys_ctx_t* ctx, bool bypass)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    IMGPROC_FUNC_ENTER
    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error!");
    }
    rk_aiq_fec_attrib_t fecAttr;
    ret = rk_aiq_user_api_afec_GetAttrib(ctx, &fecAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "get fec attrib failed!");
    fecAttr.bypass = bypass;
    ret = rk_aiq_user_api_afec_SetAttrib(ctx, fecAttr);
    IMGPROC_FUNC_EXIT
    return ret;
}

XCamReturn rk_aiq_uapi_setFecCorrectLevel(const rk_aiq_sys_ctx_t* ctx, int correctLevel)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    IMGPROC_FUNC_ENTER
    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error!");
    }
    rk_aiq_fec_attrib_t fecAttr;
    ret = rk_aiq_user_api_afec_GetAttrib(ctx, &fecAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "get fec attrib failed!");
    fecAttr.correct_level = correctLevel;
    ret = rk_aiq_user_api_afec_SetAttrib(ctx, fecAttr);
    IMGPROC_FUNC_EXIT
    return ret;
}

XCamReturn rk_aiq_uapi_setLdchEn(const rk_aiq_sys_ctx_t* ctx, bool en)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    IMGPROC_FUNC_ENTER
    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error!");
    }
    rk_aiq_ldch_attrib_t ldchAttr;
    ret = rk_aiq_user_api_aldch_GetAttrib(ctx, &ldchAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "get ldch attrib failed!");
    ldchAttr.en = en;
    ret = rk_aiq_user_api_aldch_SetAttrib(ctx, ldchAttr);
    IMGPROC_FUNC_EXIT
    return ret;
}

XCamReturn rk_aiq_uapi_setLdchCorrectLevel(const rk_aiq_sys_ctx_t* ctx, int correctLevel)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    IMGPROC_FUNC_ENTER
    if (ctx == NULL) {
        ret = XCAM_RETURN_ERROR_PARAM;
        RKAIQ_IMGPROC_CHECK_RET(ret, "param error!");
    }
    rk_aiq_ldch_attrib_t ldchAttr;
    ret = rk_aiq_user_api_aldch_GetAttrib(ctx, &ldchAttr);
    RKAIQ_IMGPROC_CHECK_RET(ret, "get ldch attrib failed!");
    ldchAttr.correct_level = correctLevel;
    ret = rk_aiq_user_api_aldch_SetAttrib(ctx, ldchAttr);
    IMGPROC_FUNC_EXIT
    return ret;
}

RKAIQ_END_DECLARE
