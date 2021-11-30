/******************************************************************************
 *
 * Copyright 2019, Fuzhou Rockchip Electronics Co.Ltd. All rights reserved.
 * No part of this work may be reproduced, modified, distributed, transmitted,
 * transcribed, or translated into any language or computer format, in any form
 * or by any means without written permission of:
 * Fuzhou Rockchip Electronics Co.Ltd .
 *
 *
 *****************************************************************************/
/**
 * @file adrc.cpp
 *
 * @brief
 *   ADD_DESCRIPTION_HERE
 *
 *****************************************************************************/
#include "math.h"
#include "rk_aiq_algo_types_int.h"
#include "rk_aiq_types_adrc_algo_int.h"
#include "xcam_log.h"
#include "rk_aiq_types_adrc_algo_prvt.h"


/******************************************************************************
 * AdrcStart()
 *****************************************************************************/
XCamReturn AdrcStart
(
    AdrcContext_t* pAdrcCtx
) {

    LOG1_ATMO( "%s:enter!\n", __FUNCTION__);

    // initial checks
    if (pAdrcCtx == NULL) {
        return (XCAM_RETURN_ERROR_MEM);
    }

    if ((ADRC_STATE_RUNNING == pAdrcCtx->state)
            || (ADRC_STATE_LOCKED == pAdrcCtx->state)) {
        return (XCAM_RETURN_ERROR_PARAM);
    }

    pAdrcCtx->state = ADRC_STATE_RUNNING;

    LOG1_ATMO( "%s:exit!\n", __FUNCTION__);
    return (XCAM_RETURN_NO_ERROR);
}

/******************************************************************************
 * DrcGetCurrPara()
 *****************************************************************************/
float DrcGetCurrPara
(
    float           inPara,
    float*         inMatrixX,
    float*         inMatrixY,
    int Max_Knots
) {
    LOG1_AMERGE( "%s:enter!\n", __FUNCTION__);
    float x1 = 0.0f;
    float x2 = 0.0f;
    float value1 = 0.0f;
    float value2 = 0.0f;
    float outPara = 0.0f;

    if(inPara < inMatrixX[0])
        outPara = inMatrixY[0];
    else if (inPara >= inMatrixX[Max_Knots - 1])
        outPara = inMatrixY[Max_Knots - 1];
    else
        for(int i = 0; i < Max_Knots - 1; i++)
        {
            if(inPara >= inMatrixX[i] && inPara < inMatrixX[i + 1])
            {
                x1 = inMatrixX[i];
                x2 = inMatrixX[i + 1];
                value1 = inMatrixY[i];
                value2 = inMatrixY[i + 1];
                outPara = value1 + (inPara - x1) * (value1 - value2) / (x1 - x2);
                break;
            }
            else
                continue;
        }

    return outPara;
    LOG1_AMERGE( "%s:exit!\n", __FUNCTION__);
}

/******************************************************************************
 * DrcGetCurrPara()
 *****************************************************************************/
int DrcGetCurrParaInt
(
    float           inPara,
    float*         inMatrixX,
    int*         inMatrixY,
    int Max_Knots
) {
    LOG1_AMERGE( "%s:enter!\n", __FUNCTION__);
    float x1 = 0.0f;
    float x2 = 0.0f;
    float value1 = 0.0f;
    float value2 = 0.0f;
    int outPara = 0;

    if(inPara < inMatrixX[0])
        outPara = inMatrixY[0];
    else if (inPara >= inMatrixX[Max_Knots - 1])
        outPara = inMatrixY[Max_Knots - 1];
    else
        for(int i = 0; i < Max_Knots - 1; i++)
        {
            if(inPara >= inMatrixX[i] && inPara < inMatrixX[i + 1])
            {
                x1 = inMatrixX[i];
                x2 = inMatrixX[i + 1];
                value1 = inMatrixY[i];
                value2 = inMatrixY[i + 1];
                outPara = (int)(value1 + (inPara - x1) * (value1 - value2) / (x1 - x2));
                break;
            }
            else
                continue;
        }

    return outPara;
    LOG1_AMERGE( "%s:exit!\n", __FUNCTION__);
}

/******************************************************************************
 * AdrcStop()
 *****************************************************************************/
XCamReturn AdrcStop
(
    AdrcContext_t* pAdrcCtx
) {

    LOG1_ATMO( "%s:enter!\n", __FUNCTION__);

    // initial checks
    if (pAdrcCtx == NULL) {
        return (XCAM_RETURN_ERROR_MEM);
    }

    // before stopping, unlock the AHDR if locked
    if (ADRC_STATE_LOCKED == pAdrcCtx->state) {
        return (XCAM_RETURN_ERROR_PARAM);
    }

    pAdrcCtx->state = ADRC_STATE_STOPPED;

    LOG1_ATMO( "%s:exit!\n", __FUNCTION__);

    return (XCAM_RETURN_NO_ERROR);
}
/******************************************************************************
 * DrcNewMalloc()
 ***************************************************************************/
void DrcNewMalloc
(
    AdrcConfig_t*           pConfig,
    DrcCalibDB_t*         pCalibDb
) {
    LOG1_ATMO( "%s:enter!\n", __FUNCTION__);

    // initial checks
    DCT_ASSERT(pConfig != NULL);
    DCT_ASSERT(pCalibDb != NULL);

    if(CHECK_ISP_HW_V21()) {
        LOG1_ATMO( "%s: Pre DrcGain len:%d, Cur DrcGain len:%d\n", __FUNCTION__, pConfig->Drc_v20.DrcGain.len, pCalibDb->Drc_v20.DrcTuningPara.DrcGain.EnvLv_len);
        LOG1_ATMO( "%s: Pre HiLit len:%d, Cur HiLit len:%d\n", __FUNCTION__, pConfig->Drc_v20.HiLit.len, pCalibDb->Drc_v20.DrcTuningPara.HiLight.EnvLv_len);
        LOG1_ATMO( "%s: Pre Local len:%d, Cur Local len:%d\n", __FUNCTION__, pConfig->Drc_v20.Local.len, pCalibDb->Drc_v20.DrcTuningPara.LocalTMOSetting.LocalTMOData.EnvLv_len);

        //drc gain
        if(pConfig->Drc_v20.DrcGain.len != pCalibDb->Drc_v20.DrcTuningPara.DrcGain.EnvLv_len) {
            free(pConfig->Drc_v20.DrcGain.EnvLv);
            free(pConfig->Drc_v20.DrcGain.DrcGain);
            free(pConfig->Drc_v20.DrcGain.Alpha);
            free(pConfig->Drc_v20.DrcGain.Clip);
            pConfig->Drc_v20.DrcGain.len = pCalibDb->Drc_v20.DrcTuningPara.DrcGain.EnvLv_len;
            pConfig->Drc_v20.DrcGain.EnvLv = (float*)malloc(sizeof(float) * (pCalibDb->Drc_v20.DrcTuningPara.DrcGain.EnvLv_len));
            pConfig->Drc_v20.DrcGain.DrcGain = (float*)malloc(sizeof(float) * (pCalibDb->Drc_v20.DrcTuningPara.DrcGain.EnvLv_len));
            pConfig->Drc_v20.DrcGain.Alpha = (float*)malloc(sizeof(float) * (pCalibDb->Drc_v20.DrcTuningPara.DrcGain.EnvLv_len));
            pConfig->Drc_v20.DrcGain.Clip = (float*)malloc(sizeof(float) * (pCalibDb->Drc_v20.DrcTuningPara.DrcGain.EnvLv_len));
        }

        //hilit
        if(pConfig->Drc_v20.HiLit.len != pCalibDb->Drc_v20.DrcTuningPara.HiLight.EnvLv_len) {
            free(pConfig->Drc_v20.HiLit.EnvLv);
            free(pConfig->Drc_v20.HiLit.Strength);
            pConfig->Drc_v20.HiLit.len = pCalibDb->Drc_v20.DrcTuningPara.HiLight.EnvLv_len;
            pConfig->Drc_v20.HiLit.EnvLv = (float*)malloc(sizeof(float) * (pCalibDb->Drc_v20.DrcTuningPara.HiLight.EnvLv_len));
            pConfig->Drc_v20.HiLit.Strength = (float*)malloc(sizeof(float) * (pCalibDb->Drc_v20.DrcTuningPara.HiLight.EnvLv_len));
        }

        //local
        if(pConfig->Drc_v20.Local.len != pCalibDb->Drc_v20.DrcTuningPara.LocalTMOSetting.LocalTMOData.EnvLv_len) {
            free(pConfig->Drc_v20.Local.EnvLv);
            free(pConfig->Drc_v20.Local.LocalWeit);
            free(pConfig->Drc_v20.Local.GlobalContrast);
            free(pConfig->Drc_v20.Local.LoLitContrast);
            pConfig->Drc_v20.Local.len = pCalibDb->Drc_v20.DrcTuningPara.LocalTMOSetting.LocalTMOData.EnvLv_len;
            pConfig->Drc_v20.Local.EnvLv = (float*)malloc(sizeof(float) * (pCalibDb->Drc_v20.DrcTuningPara.LocalTMOSetting.LocalTMOData.EnvLv_len));
            pConfig->Drc_v20.Local.LocalWeit = (float*)malloc(sizeof(float) * (pCalibDb->Drc_v20.DrcTuningPara.LocalTMOSetting.LocalTMOData.EnvLv_len));
            pConfig->Drc_v20.Local.GlobalContrast = (float*)malloc(sizeof(float) * (pCalibDb->Drc_v20.DrcTuningPara.LocalTMOSetting.LocalTMOData.EnvLv_len));
            pConfig->Drc_v20.Local.LoLitContrast = (float*)malloc(sizeof(float) * (pCalibDb->Drc_v20.DrcTuningPara.LocalTMOSetting.LocalTMOData.EnvLv_len));
        }
    }
    else if(CHECK_ISP_HW_V30()) {
        LOG1_ATMO( "%s: Pre DrcGain len:%d, Cur DrcGain len:%d\n", __FUNCTION__, pConfig->Drc_v21.DrcGain.len, pCalibDb->Drc_v21.DrcTuningPara.DrcGain.EnvLv_len);
        LOG1_ATMO( "%s: Pre HiLit len:%d, Cur HiLit len:%d\n", __FUNCTION__, pConfig->Drc_v21.HiLit.len, pCalibDb->Drc_v21.DrcTuningPara.HiLight.EnvLv_len);
        LOG1_ATMO( "%s: Pre Local len:%d, Cur Local len:%d\n", __FUNCTION__, pConfig->Drc_v21.Local.len, pCalibDb->Drc_v21.DrcTuningPara.LocalSetting.LocalData.EnvLv_len);

        //drc gain
        if(pConfig->Drc_v21.DrcGain.len != pCalibDb->Drc_v21.DrcTuningPara.DrcGain.EnvLv_len) {
            free(pConfig->Drc_v21.DrcGain.EnvLv);
            free(pConfig->Drc_v21.DrcGain.DrcGain);
            free(pConfig->Drc_v21.DrcGain.Alpha);
            free(pConfig->Drc_v21.DrcGain.Clip);
            pConfig->Drc_v21.DrcGain.len = pCalibDb->Drc_v21.DrcTuningPara.DrcGain.EnvLv_len;
            pConfig->Drc_v21.DrcGain.EnvLv = (float*)malloc(sizeof(float) * (pCalibDb->Drc_v21.DrcTuningPara.DrcGain.EnvLv_len));
            pConfig->Drc_v21.DrcGain.DrcGain = (float*)malloc(sizeof(float) * (pCalibDb->Drc_v21.DrcTuningPara.DrcGain.EnvLv_len));
            pConfig->Drc_v21.DrcGain.Alpha = (float*)malloc(sizeof(float) * (pCalibDb->Drc_v21.DrcTuningPara.DrcGain.EnvLv_len));
            pConfig->Drc_v21.DrcGain.Clip = (float*)malloc(sizeof(float) * (pCalibDb->Drc_v21.DrcTuningPara.DrcGain.EnvLv_len));
        }

        //hilit
        if(pConfig->Drc_v21.HiLit.len != pCalibDb->Drc_v21.DrcTuningPara.HiLight.EnvLv_len) {
            free(pConfig->Drc_v21.HiLit.EnvLv);
            free(pConfig->Drc_v21.HiLit.Strength);
            pConfig->Drc_v21.HiLit.len = pCalibDb->Drc_v21.DrcTuningPara.HiLight.EnvLv_len;
            pConfig->Drc_v21.HiLit.EnvLv = (float*)malloc(sizeof(float) * (pCalibDb->Drc_v21.DrcTuningPara.HiLight.EnvLv_len));
            pConfig->Drc_v21.HiLit.Strength = (float*)malloc(sizeof(float) * (pCalibDb->Drc_v21.DrcTuningPara.HiLight.EnvLv_len));
        }

        //local
        if(pConfig->Drc_v21.Local.len != pCalibDb->Drc_v21.DrcTuningPara.LocalSetting.LocalData.EnvLv_len) {
            free(pConfig->Drc_v21.Local.EnvLv);
            free(pConfig->Drc_v21.Local.LocalWeit);
            free(pConfig->Drc_v21.Local.LocalAutoEnable);
            free(pConfig->Drc_v21.Local.LocalAutoWeit);
            free(pConfig->Drc_v21.Local.GlobalContrast);
            free(pConfig->Drc_v21.Local.LoLitContrast);
            pConfig->Drc_v21.Local.len = pCalibDb->Drc_v21.DrcTuningPara.LocalSetting.LocalData.EnvLv_len;
            pConfig->Drc_v21.Local.EnvLv = (float*)malloc(sizeof(float) * (pCalibDb->Drc_v21.DrcTuningPara.LocalSetting.LocalData.EnvLv_len));
            pConfig->Drc_v21.Local.LocalWeit = (float*)malloc(sizeof(float) * (pCalibDb->Drc_v21.DrcTuningPara.LocalSetting.LocalData.EnvLv_len));
            pConfig->Drc_v21.Local.LocalAutoEnable = (int*)malloc(sizeof(float) * (pCalibDb->Drc_v21.DrcTuningPara.LocalSetting.LocalData.EnvLv_len));
            pConfig->Drc_v21.Local.LocalAutoWeit = (float*)malloc(sizeof(float) * (pCalibDb->Drc_v21.DrcTuningPara.LocalSetting.LocalData.EnvLv_len));
            pConfig->Drc_v21.Local.GlobalContrast = (float*)malloc(sizeof(float) * (pCalibDb->Drc_v21.DrcTuningPara.LocalSetting.LocalData.EnvLv_len));
            pConfig->Drc_v21.Local.LoLitContrast = (float*)malloc(sizeof(float) * (pCalibDb->Drc_v21.DrcTuningPara.LocalSetting.LocalData.EnvLv_len));
        }
    }

    LOG1_ATMO( "%s:exit!\n", __FUNCTION__);
}

/******************************************************************************
* CalibrateDrcGainY()
*****************************************************************************/
void CalibrateDrcGainY( DrcProcRes_t *para, float DraGain, float alpha)
{
    LOG1_ATMO("%s:Enter!\n", __FUNCTION__);

    float tmp = 0;
    float luma[17] = { 0, 256, 512, 768, 1024, 1280, 1536, 1792, 2048, 2304, 2560, 2816, 3072, 3328, 3584, 3840, 4096 };
    float gainTable[17];

    for(int i = 0; i < 17; ++i) {
        tmp = luma[i];
        tmp = 1 - alpha * pow((1 - tmp / 4096), 2);
        gainTable[i] = 1024 * pow(DraGain, tmp);
        if(CHECK_ISP_HW_V21())
            para->Drc_v20.sw_drc_gain_y[i] = (int)(gainTable[i]) ;
        else if(CHECK_ISP_HW_V30())
            para->Drc_v21.gain_y[i] = (int)(gainTable[i]) ;
    }

    LOG1_ATMO("%s:Eixt!\n", __FUNCTION__);

}

/******************************************************************************
 * AdrcApiOffConfig()
 *set default Config data
 *****************************************************************************/
void AdrcApiOffConfig
(
    AdrcContext_t*           pAdrcCtx
) {
    LOG1_ATMO( "%s:enter!\n", __FUNCTION__);




    LOGI_ATMO( "%s:exit!\n", __FUNCTION__);
}

/******************************************************************************
 * Config()
 *set default Config data
 *****************************************************************************/
void Config
(
    AdrcContext_t*           pAdrcCtx
) {
    LOGI_ATMO( "%s:enter!\n", __FUNCTION__);

    // initial checks
    DCT_ASSERT(pAdrcCtx != NULL);

    if(pAdrcCtx->drcAttr.opMode == DRC_OPMODE_API_OFF)
    {
        AdrcApiOffConfig(pAdrcCtx);
        LOGD_ATMO("%s: Ahdr Api is OFF!!!:\n", __FUNCTION__);
    }
    else
        LOGD_ATMO("%s: Ahdr Api is ON!!!:\n", __FUNCTION__);

    //config default PrevData data
    pAdrcCtx->PrevData.frameCnt = 0;
    pAdrcCtx->PrevData.EnvLv = 0;
    pAdrcCtx->PrevData.ApiMode = DRC_OPMODE_API_OFF;
    pAdrcCtx->PrevData.HandleData.Drc_v20.DrcGain = 4.0;
    pAdrcCtx->PrevData.HandleData.Drc_v20.Alpha = 0.2;
    pAdrcCtx->PrevData.HandleData.Drc_v20.Clip = 16.0 ;
    pAdrcCtx->PrevData.HandleData.Drc_v20.Strength = 0.01 ;
    pAdrcCtx->PrevData.HandleData.Drc_v20.LocalWeit = 1.00 ;
    pAdrcCtx->PrevData.HandleData.Drc_v20.GlobalContrast = 0 ;
    pAdrcCtx->PrevData.HandleData.Drc_v20.LoLitContrast = 0;

    LOG1_ATMO( "%s:exit!\n", __FUNCTION__);
}

/******************************************************************************
 * Config()
 *set default Config data
 *****************************************************************************/
void ConfigV30
(
    AdrcContext_t*           pAdrcCtx
) {
    LOGI_ATMO( "%s:enter!\n", __FUNCTION__);

    // initial checks
    DCT_ASSERT(pAdrcCtx != NULL);

    if(pAdrcCtx->drcAttr.opMode == DRC_OPMODE_API_OFF)
    {
        AdrcApiOffConfig(pAdrcCtx);
        LOGD_ATMO("%s: Ahdr Api is OFF!!!:\n", __FUNCTION__);
    }
    else
        LOGD_ATMO("%s: Ahdr Api is ON!!!:\n", __FUNCTION__);

    //config default PrevData data
    pAdrcCtx->PrevData.frameCnt = 0;
    pAdrcCtx->PrevData.EnvLv = 0;
    pAdrcCtx->PrevData.MotionCoef = 0;
    pAdrcCtx->PrevData.ApiMode = DRC_OPMODE_API_OFF;
    pAdrcCtx->PrevData.HandleData.Drc_v21.DrcGain = 4.0;
    pAdrcCtx->PrevData.HandleData.Drc_v21.Alpha = 0.2;
    pAdrcCtx->PrevData.HandleData.Drc_v21.Clip = 16.0 ;
    pAdrcCtx->PrevData.HandleData.Drc_v21.Strength = 0.01 ;
    pAdrcCtx->PrevData.HandleData.Drc_v21.LocalWeit = 1.00 ;
    pAdrcCtx->PrevData.HandleData.Drc_v21.LocalAutoEnable = 1 ;
    pAdrcCtx->PrevData.HandleData.Drc_v21.LocalAutoWeit = 0.037477 ;
    pAdrcCtx->PrevData.HandleData.Drc_v21.GlobalContrast = 0 ;
    pAdrcCtx->PrevData.HandleData.Drc_v21.LoLitContrast = 0;

    LOG1_ATMO( "%s:exit!\n", __FUNCTION__);
}

void AdrcGetAeResult
(
    AdrcContext_t*           pAdrcCtx,
    AecPreResult_t  AecHdrPreResult
) {
    LOG1_ATMO( "%s:enter!\n", __FUNCTION__);

    //get Ae Pre Result
    pAdrcCtx->CurrAeResult.GlobalEnvLv = AecHdrPreResult.GlobalEnvLv[AecHdrPreResult.NormalIndex];
    pAdrcCtx->CurrAeResult.M2S_Ratio = AecHdrPreResult.M2S_ExpRatio;
    pAdrcCtx->CurrAeResult.M2S_Ratio = pAdrcCtx->CurrAeResult.M2S_Ratio < 1 ? 1 : pAdrcCtx->CurrAeResult.M2S_Ratio;
    pAdrcCtx->CurrAeResult.L2M_Ratio = AecHdrPreResult.L2M_ExpRatio;
    pAdrcCtx->CurrAeResult.L2M_Ratio = pAdrcCtx->CurrAeResult.L2M_Ratio < 1 ? 1 : pAdrcCtx->CurrAeResult.L2M_Ratio;

    //transfer CurrAeResult data into AhdrHandle
    switch (pAdrcCtx->FrameNumber)
    {
    case 1:
        pAdrcCtx->CurrData.Ratio = 1;
        pAdrcCtx->CurrAeResult.ISO = AecHdrPreResult.LinearExp.exp_real_params.analog_gain * 50.0;
        pAdrcCtx->CurrAeResult.GlobalEnvLv = AecHdrPreResult.GlobalEnvLv[0];
        pAdrcCtx->CurrAeResult.AEMaxRatio = 1.0;
        break;
    case 2:
        pAdrcCtx->CurrData.Ratio = pAdrcCtx->CurrAeResult.M2S_Ratio;
        pAdrcCtx->CurrAeResult.ISO = AecHdrPreResult.HdrExp[1].exp_real_params.analog_gain * 50.0;
        pAdrcCtx->CurrAeResult.GlobalEnvLv = AecHdrPreResult.GlobalEnvLv[1];
        //todo
        pAdrcCtx->CurrAeResult.AEMaxRatio = 64.0;
        break;
    case 3:
        if(CHECK_ISP_HW_V30()) {
            pAdrcCtx->CurrData.Ratio = pAdrcCtx->CurrAeResult.M2S_Ratio * pAdrcCtx->CurrAeResult.L2M_Ratio;
            pAdrcCtx->CurrAeResult.ISO = AecHdrPreResult.HdrExp[1].exp_real_params.analog_gain * 50.0;
            pAdrcCtx->CurrAeResult.GlobalEnvLv = AecHdrPreResult.GlobalEnvLv[1];
            //todo
            pAdrcCtx->CurrAeResult.AEMaxRatio = 64.0;
        }
        break;
    default:
        LOGE_ATMO("%s:  Wrong frame number in HDR mode!!!\n", __FUNCTION__);
        break;
    }

    //Normalize the current envLv for AEC
    float maxEnvLuma = 65 / 10;
    float minEnvLuma = 0;
    pAdrcCtx->CurrData.EnvLv = (pAdrcCtx->CurrAeResult.GlobalEnvLv  - minEnvLuma) / (maxEnvLuma - minEnvLuma);
    pAdrcCtx->CurrData.EnvLv = LIMIT_VALUE(pAdrcCtx->CurrData.EnvLv, 1, 0);

    LOGD_ATMO("%s:  Current Ratio:%f\n", __FUNCTION__, pAdrcCtx->CurrData.Ratio);

    LOG1_ATMO( "%s:exit!\n", __FUNCTION__);
}

void AdrcGetSensorInfo
(
    AdrcContext_t*     pAdrcCtx,
    AecProcResult_t  AecHdrProcResult
) {
    LOG1_ATMO( "%s:enter!\n", __FUNCTION__);

    pAdrcCtx->SensorInfo.LongFrmMode = AecHdrProcResult.LongFrmMode;

    for(int i = 0; i < 3; i++)
    {
#if 0 // TODO Merge
        pAdrcCtx->SensorInfo.HdrMinGain[i] = AecHdrProcResult.HdrExpRange.stGainRange[i].Min;
        pAdrcCtx->SensorInfo.HdrMaxGain[i] = AecHdrProcResult.HdrExpRange.stGainRange[i].Max;
        pAdrcCtx->SensorInfo.HdrMinIntegrationTime[i] = AecHdrProcResult.HdrExpRange.stExpTimeRange[i].Min;
        pAdrcCtx->SensorInfo.HdrMaxIntegrationTime[i] = AecHdrProcResult.HdrExpRange.stExpTimeRange[i].Max;
#endif
    }

    if(pAdrcCtx->FrameNumber == 1) {
        pAdrcCtx->SensorInfo.MaxExpoL = 0;
        pAdrcCtx->SensorInfo.MinExpoL = 0;
    }
    else if(pAdrcCtx->FrameNumber == 2) {
        pAdrcCtx->SensorInfo.MaxExpoL = pAdrcCtx->SensorInfo.HdrMaxGain[1] * pAdrcCtx->SensorInfo.HdrMaxIntegrationTime[1];
        pAdrcCtx->SensorInfo.MinExpoL = pAdrcCtx->SensorInfo.HdrMinGain[1] * pAdrcCtx->SensorInfo.HdrMinIntegrationTime[1];
    }
    else if(pAdrcCtx->FrameNumber == 3) {
        pAdrcCtx->SensorInfo.MaxExpoL = pAdrcCtx->SensorInfo.HdrMaxGain[2] * pAdrcCtx->SensorInfo.HdrMaxIntegrationTime[2];
        pAdrcCtx->SensorInfo.MinExpoL = pAdrcCtx->SensorInfo.HdrMinGain[2] * pAdrcCtx->SensorInfo.HdrMinIntegrationTime[2];
    }

    pAdrcCtx->SensorInfo.MaxExpoS = pAdrcCtx->SensorInfo.HdrMaxGain[0] * pAdrcCtx->SensorInfo.HdrMaxIntegrationTime[0];
    pAdrcCtx->SensorInfo.MinExpoS = pAdrcCtx->SensorInfo.HdrMinGain[0] * pAdrcCtx->SensorInfo.HdrMinIntegrationTime[0];

    LOG1_ATMO( "%s:exit!\n", __FUNCTION__);
}

/******************************************************************************
 * AdrcIQUpdate()
 *
 *****************************************************************************/
void AdrcIQUpdate
(
    AdrcContext_t*     pAdrcCtx
)
{
    LOG1_ATMO("%s:enter!\n", __FUNCTION__);

    //get Drc gain
    pAdrcCtx->CurrData.HandleData.Drc_v20.DrcGain = DrcGetCurrPara(pAdrcCtx->CurrData.EnvLv,
            pAdrcCtx->Config.Drc_v20.DrcGain.EnvLv, pAdrcCtx->Config.Drc_v20.DrcGain.DrcGain, pAdrcCtx->Config.Drc_v20.DrcGain.len);
    pAdrcCtx->CurrData.HandleData.Drc_v20.Alpha = DrcGetCurrPara(pAdrcCtx->CurrData.EnvLv,
            pAdrcCtx->Config.Drc_v20.DrcGain.EnvLv, pAdrcCtx->Config.Drc_v20.DrcGain.Alpha, pAdrcCtx->Config.Drc_v20.DrcGain.len);
    pAdrcCtx->CurrData.HandleData.Drc_v20.Clip = DrcGetCurrPara(pAdrcCtx->CurrData.EnvLv,
            pAdrcCtx->Config.Drc_v20.DrcGain.EnvLv, pAdrcCtx->Config.Drc_v20.DrcGain.Clip, pAdrcCtx->Config.Drc_v20.DrcGain.len);

    //get hi lit
    pAdrcCtx->CurrData.HandleData.Drc_v20.Strength = DrcGetCurrPara(pAdrcCtx->CurrData.EnvLv,
            pAdrcCtx->Config.Drc_v20.HiLit.EnvLv, pAdrcCtx->Config.Drc_v20.HiLit.Strength, pAdrcCtx->Config.Drc_v20.HiLit.len);

    //get local
    pAdrcCtx->CurrData.HandleData.Drc_v20.LocalWeit = DrcGetCurrPara(pAdrcCtx->CurrData.EnvLv,
            pAdrcCtx->Config.Drc_v20.Local.EnvLv, pAdrcCtx->Config.Drc_v20.Local.LocalWeit, pAdrcCtx->Config.Drc_v20.Local.len);
    pAdrcCtx->CurrData.HandleData.Drc_v20.GlobalContrast = DrcGetCurrPara(pAdrcCtx->CurrData.EnvLv,
            pAdrcCtx->Config.Drc_v20.Local.EnvLv, pAdrcCtx->Config.Drc_v20.Local.GlobalContrast, pAdrcCtx->Config.Drc_v20.Local.len);
    pAdrcCtx->CurrData.HandleData.Drc_v20.LoLitContrast = DrcGetCurrPara(pAdrcCtx->CurrData.EnvLv,
            pAdrcCtx->Config.Drc_v20.Local.EnvLv, pAdrcCtx->Config.Drc_v20.Local.LoLitContrast, pAdrcCtx->Config.Drc_v20.Local.len);

    //compress
    pAdrcCtx->CurrData.HandleData.Drc_v20.Mode = pAdrcCtx->Config.Drc_v20.Compress.Mode;
    for(int i = 0; i < ADRC_Y_NUM; i++)
        pAdrcCtx->CurrData.HandleData.Drc_v20.Manual_curve[i] = pAdrcCtx->Config.Drc_v20.Compress.Manual_curve[i];

    //others
    if(0 != memcmp(&pAdrcCtx->CurrData.Others, &pAdrcCtx->Config.Drc_v20.Others, sizeof(DrcOhters_t)))
        memcpy(&pAdrcCtx->CurrData.Others, &pAdrcCtx->Config.Drc_v20.Others, sizeof(DrcOhters_t));//load iq paras

    LOG1_ATMO( "%s:exit!\n", __FUNCTION__);
}

/******************************************************************************
 * AdrcIQUpdate()
 *
 *****************************************************************************/
void AdrcIQUpdateV30
(
    AdrcContext_t*     pAdrcCtx
)
{
    LOG1_ATMO("%s:enter!\n", __FUNCTION__);

    //get Drc gain
    pAdrcCtx->CurrData.HandleData.Drc_v21.DrcGain = DrcGetCurrPara(pAdrcCtx->CurrData.EnvLv,
            pAdrcCtx->Config.Drc_v21.DrcGain.EnvLv, pAdrcCtx->Config.Drc_v21.DrcGain.DrcGain, pAdrcCtx->Config.Drc_v21.DrcGain.len);
    pAdrcCtx->CurrData.HandleData.Drc_v21.Alpha = DrcGetCurrPara(pAdrcCtx->CurrData.EnvLv,
            pAdrcCtx->Config.Drc_v21.DrcGain.EnvLv, pAdrcCtx->Config.Drc_v21.DrcGain.Alpha, pAdrcCtx->Config.Drc_v21.DrcGain.len);
    pAdrcCtx->CurrData.HandleData.Drc_v21.Clip = DrcGetCurrPara(pAdrcCtx->CurrData.EnvLv,
            pAdrcCtx->Config.Drc_v21.DrcGain.EnvLv, pAdrcCtx->Config.Drc_v21.DrcGain.Clip, pAdrcCtx->Config.Drc_v21.DrcGain.len);

    //get hi lit
    pAdrcCtx->CurrData.HandleData.Drc_v21.Strength = DrcGetCurrPara(pAdrcCtx->CurrData.EnvLv,
            pAdrcCtx->Config.Drc_v21.HiLit.EnvLv, pAdrcCtx->Config.Drc_v21.HiLit.Strength, pAdrcCtx->Config.Drc_v21.HiLit.len);

    //get local
    pAdrcCtx->CurrData.HandleData.Drc_v21.LocalWeit = DrcGetCurrPara(pAdrcCtx->CurrData.EnvLv,
            pAdrcCtx->Config.Drc_v21.Local.EnvLv, pAdrcCtx->Config.Drc_v21.Local.LocalWeit, pAdrcCtx->Config.Drc_v21.Local.len);
    pAdrcCtx->CurrData.HandleData.Drc_v21.GlobalContrast = DrcGetCurrPara(pAdrcCtx->CurrData.EnvLv,
            pAdrcCtx->Config.Drc_v21.Local.EnvLv, pAdrcCtx->Config.Drc_v21.Local.GlobalContrast, pAdrcCtx->Config.Drc_v21.Local.len);
    pAdrcCtx->CurrData.HandleData.Drc_v21.LoLitContrast = DrcGetCurrPara(pAdrcCtx->CurrData.EnvLv,
            pAdrcCtx->Config.Drc_v21.Local.EnvLv, pAdrcCtx->Config.Drc_v21.Local.LoLitContrast, pAdrcCtx->Config.Drc_v21.Local.len);
    pAdrcCtx->CurrData.HandleData.Drc_v21.LocalAutoEnable = DrcGetCurrParaInt(pAdrcCtx->CurrData.EnvLv,
            pAdrcCtx->Config.Drc_v21.Local.EnvLv, pAdrcCtx->Config.Drc_v21.Local.LocalAutoEnable, pAdrcCtx->Config.Drc_v21.Local.len);
    pAdrcCtx->CurrData.HandleData.Drc_v21.LocalAutoWeit = DrcGetCurrPara(pAdrcCtx->CurrData.EnvLv,
            pAdrcCtx->Config.Drc_v21.Local.EnvLv, pAdrcCtx->Config.Drc_v21.Local.LocalAutoWeit, pAdrcCtx->Config.Drc_v21.Local.len);

    //compress
    pAdrcCtx->CurrData.HandleData.Drc_v21.Mode = pAdrcCtx->Config.Drc_v21.Compress.Mode;
    for(int i = 0; i < ADRC_Y_NUM; i++)
        pAdrcCtx->CurrData.HandleData.Drc_v21.Manual_curve[i] = pAdrcCtx->Config.Drc_v21.Compress.Manual_curve[i];

    //others
    if(0 != memcmp(&pAdrcCtx->CurrData.Others, &pAdrcCtx->Config.Drc_v21.Others, sizeof(DrcOhters_t)))
        memcpy(&pAdrcCtx->CurrData.Others, &pAdrcCtx->Config.Drc_v21.Others, sizeof(DrcOhters_t));//load iq paras

    LOG1_ATMO( "%s:exit!\n", __FUNCTION__);
}

/******************************************************************************
 * AdrcTranferData2Api()
 *
 *****************************************************************************/
void AdrcTranferData2Api
(
    AdrcContext_t*     pAdrcCtx
)
{
    LOG1_ATMO("%s:enter!\n", __FUNCTION__);

    //info
    pAdrcCtx->drcAttr.Info.EnvLv = pAdrcCtx->CurrData.EnvLv;

    //reg
    pAdrcCtx->drcAttr.stManual.DrcGain.DrcGain = pAdrcCtx->CurrData.HandleData.Drc_v20.DrcGain;
    pAdrcCtx->drcAttr.stManual.DrcGain.Alpha = pAdrcCtx->CurrData.HandleData.Drc_v20.Alpha;
    pAdrcCtx->drcAttr.stManual.DrcGain.Clip = pAdrcCtx->CurrData.HandleData.Drc_v20.Clip;

    pAdrcCtx->drcAttr.stManual.HiLit.Strength = pAdrcCtx->CurrData.HandleData.Drc_v20.Strength;

    pAdrcCtx->drcAttr.stManual.Local.LocalWeit = pAdrcCtx->CurrData.HandleData.Drc_v20.LocalWeit;
    pAdrcCtx->drcAttr.stManual.Local.GlobalContrast = pAdrcCtx->CurrData.HandleData.Drc_v20.GlobalContrast;
    pAdrcCtx->drcAttr.stManual.Local.LoLitContrast = pAdrcCtx->CurrData.HandleData.Drc_v20.LoLitContrast;


    LOG1_ATMO( "%s:exit!\n", __FUNCTION__);
}

/******************************************************************************
 * AdrcTranferData2Api()
 *
 *****************************************************************************/
void AdrcTranferData2ApiV30
(
    AdrcContext_t*     pAdrcCtx
)
{
    LOG1_ATMO("%s:enter!\n", __FUNCTION__);

    //info
    pAdrcCtx->drcAttr.Info.EnvLv = pAdrcCtx->CurrData.EnvLv;

    //reg
    pAdrcCtx->drcAttr.stManual.DrcGain.DrcGain = pAdrcCtx->CurrData.HandleData.Drc_v21.DrcGain;
    pAdrcCtx->drcAttr.stManual.DrcGain.Alpha = pAdrcCtx->CurrData.HandleData.Drc_v21.Alpha;
    pAdrcCtx->drcAttr.stManual.DrcGain.Clip = pAdrcCtx->CurrData.HandleData.Drc_v21.Clip;
    pAdrcCtx->drcAttr.stManual.HiLit.Strength = pAdrcCtx->CurrData.HandleData.Drc_v21.Strength;
    pAdrcCtx->drcAttr.stManual.Local.LocalWeit = pAdrcCtx->CurrData.HandleData.Drc_v21.LocalWeit;
    pAdrcCtx->drcAttr.stManual.Local.GlobalContrast = pAdrcCtx->CurrData.HandleData.Drc_v21.GlobalContrast;
    pAdrcCtx->drcAttr.stManual.Local.LoLitContrast = pAdrcCtx->CurrData.HandleData.Drc_v21.LoLitContrast;

    LOG1_ATMO( "%s:exit!\n", __FUNCTION__);
}

/******************************************************************************
 * AdrcApiManualUpdate()
 *
 *****************************************************************************/
void AdrcApiManualUpdate
(
    CurrData_t*     pAdrcCurr,
    mdrcAttr_t*  pStManual

)
{
    LOG1_ATMO("%s:enter!\n", __FUNCTION__);

    //get Drc gain
    pAdrcCurr->HandleData.Drc_v20.DrcGain = pStManual->DrcGain.DrcGain;
    pAdrcCurr->HandleData.Drc_v20.Alpha = pStManual->DrcGain.Alpha;
    pAdrcCurr->HandleData.Drc_v20.Clip = pStManual->DrcGain.Clip;

    //get hi lit
    pAdrcCurr->HandleData.Drc_v20.Strength = pStManual->HiLit.Strength;

    //get local
    pAdrcCurr->HandleData.Drc_v20.LocalWeit = pStManual->Local.LocalWeit;
    pAdrcCurr->HandleData.Drc_v20.GlobalContrast = pStManual->Local.GlobalContrast;
    pAdrcCurr->HandleData.Drc_v20.LoLitContrast = pStManual->Local.LoLitContrast;

    //compress
    pAdrcCurr->HandleData.Drc_v20.Mode = pStManual->Compress.Mode;
    for(int i = 0; i < ADRC_Y_NUM; i++)
        pAdrcCurr->HandleData.Drc_v20.Manual_curve[i] = pStManual->Compress.Manual_curve[i];

    LOG1_ATMO( "%s:exit!\n", __FUNCTION__);
}

/******************************************************************************
 * AdrcUpdateConfig()
 *transfer html parameter into handle
 ***************************************************************************/
void AdrcUpdateConfig
(
    AdrcContext_t*           pAdrcCtx,
    DrcCalibDB_t*         pCalibDb
) {
    LOG1_ATMO( "%s:enter!\n", __FUNCTION__);

    // initial checks
    DCT_ASSERT(pAdrcCtx != NULL);
    DCT_ASSERT(pCalibDb != NULL);

    if(CHECK_ISP_HW_V21()) {
        //enbale
        pAdrcCtx->Config.Drc_v20.Enable  = pCalibDb->Drc_v20.DrcTuningPara.Enable;
        pAdrcCtx->Config.Drc_v20.OutPutLongFrame = pCalibDb->Drc_v20.DrcTuningPara.OutPutLongFrame;

        //drc gain
        for (int i = 0; i < pAdrcCtx->Config.Drc_v20.DrcGain.len; i++ ) {
            pAdrcCtx->Config.Drc_v20.DrcGain.EnvLv[i] = LIMIT_VALUE(pCalibDb->Drc_v20.DrcTuningPara.DrcGain.EnvLv[i], ADRCNORMALIZEMAX, ADRCNORMALIZEMIN);
            pAdrcCtx->Config.Drc_v20.DrcGain.DrcGain[i] = LIMIT_VALUE(pCalibDb->Drc_v20.DrcTuningPara.DrcGain.DrcGain[i], DRCGAINMAX, DRCGAINMIN);
            pAdrcCtx->Config.Drc_v20.DrcGain.Alpha[i] = LIMIT_VALUE(pCalibDb->Drc_v20.DrcTuningPara.DrcGain.Alpha[i], ADRCNORMALIZEMAX, ADRCNORMALIZEMIN);
            pAdrcCtx->Config.Drc_v20.DrcGain.Clip[i] = LIMIT_VALUE(pCalibDb->Drc_v20.DrcTuningPara.DrcGain.Clip[i], CLIPMAX, CLIPMIN);
        }
        //hight ligjt
        for (int i = 0; i < pAdrcCtx->Config.Drc_v20.HiLit.len; i++ ) {
            pAdrcCtx->Config.Drc_v20.HiLit.EnvLv[i] = LIMIT_VALUE(pCalibDb->Drc_v20.DrcTuningPara.HiLight.EnvLv[i], ADRCNORMALIZEMAX, ADRCNORMALIZEMIN);
            pAdrcCtx->Config.Drc_v20.HiLit.Strength[i] = LIMIT_VALUE(pCalibDb->Drc_v20.DrcTuningPara.HiLight.Strength[i], ADRCNORMALIZEMAX, ADRCNORMALIZEMIN);
        }
        // local
        for (int i = 0; i < pAdrcCtx->Config.Drc_v20.Local.len; i++ ) {
            pAdrcCtx->Config.Drc_v20.Local.EnvLv[i] = LIMIT_VALUE(pCalibDb->Drc_v20.DrcTuningPara.LocalTMOSetting.LocalTMOData.EnvLv[i], ADRCNORMALIZEMAX, ADRCNORMALIZEMIN);
            pAdrcCtx->Config.Drc_v20.Local.LocalWeit[i] = LIMIT_VALUE(pCalibDb->Drc_v20.DrcTuningPara.LocalTMOSetting.LocalTMOData.LocalWeit[i], ADRCNORMALIZEMAX, ADRCNORMALIZEMIN);
            pAdrcCtx->Config.Drc_v20.Local.GlobalContrast[i] = LIMIT_VALUE(pCalibDb->Drc_v20.DrcTuningPara.LocalTMOSetting.LocalTMOData.GlobalContrast[i], ADRCNORMALIZEMAX, ADRCNORMALIZEMIN);
            pAdrcCtx->Config.Drc_v20.Local.LoLitContrast[i] = LIMIT_VALUE(pCalibDb->Drc_v20.DrcTuningPara.LocalTMOSetting.LocalTMOData.LoLitContrast[i], ADRCNORMALIZEMAX, ADRCNORMALIZEMIN);
        }
        //others
        pAdrcCtx->Config.Drc_v20.Others.curPixWeit = LIMIT_VALUE(pCalibDb->Drc_v20.DrcTuningPara.LocalTMOSetting.curPixWeit, ADRCNORMALIZEMAX, ADRCNORMALIZEMIN);
        pAdrcCtx->Config.Drc_v20.Others.preFrameWeit = LIMIT_VALUE(pCalibDb->Drc_v20.DrcTuningPara.LocalTMOSetting.preFrameWeit, ADRCNORMALIZEMAX, ADRCNORMALIZEMIN);
        pAdrcCtx->Config.Drc_v20.Others.Range_force_sgm = LIMIT_VALUE(pCalibDb->Drc_v20.DrcTuningPara.LocalTMOSetting.Range_force_sgm, ADRCNORMALIZEMAX, ADRCNORMALIZEMIN);
        pAdrcCtx->Config.Drc_v20.Others.Range_sgm_cur = LIMIT_VALUE(pCalibDb->Drc_v20.DrcTuningPara.LocalTMOSetting.Range_sgm_cur, ADRCNORMALIZEMAX, ADRCNORMALIZEMIN);
        pAdrcCtx->Config.Drc_v20.Others.Range_sgm_pre = LIMIT_VALUE(pCalibDb->Drc_v20.DrcTuningPara.LocalTMOSetting.Range_sgm_pre, ADRCNORMALIZEMAX, ADRCNORMALIZEMIN);
        pAdrcCtx->Config.Drc_v20.Others.Space_sgm_cur = LIMIT_VALUE(pCalibDb->Drc_v20.DrcTuningPara.LocalTMOSetting.Space_sgm_cur, SPACESGMMAX, SPACESGMMIN);
        pAdrcCtx->Config.Drc_v20.Others.Space_sgm_pre = LIMIT_VALUE(pCalibDb->Drc_v20.DrcTuningPara.LocalTMOSetting.Space_sgm_pre, SPACESGMMAX, SPACESGMMIN);
        //compress and scale y
        pAdrcCtx->Config.Drc_v20.Compress.Mode = pCalibDb->Drc_v20.DrcTuningPara.CompressSetting.Mode;
        for(int i = 0; i < ADRC_Y_NUM; i++ ) {
            pAdrcCtx->Config.Drc_v20.Others.Scale_y[i] = LIMIT_VALUE(pCalibDb->Drc_v20.DrcTuningPara.Scale_y[i], SCALEYMAX, SCALEYMIN);
            pAdrcCtx->Config.Drc_v20.Compress.Manual_curve[i] = LIMIT_VALUE(pCalibDb->Drc_v20.DrcTuningPara.CompressSetting.Manual_curve[i], MANUALCURVEMAX, MANUALCURVEMIN);
        }
        pAdrcCtx->Config.Drc_v20.Others.ByPassThr = LIMIT_VALUE(pCalibDb->Drc_v20.DrcTuningPara.ByPassThr, ADRCNORMALIZEMAX, ADRCNORMALIZEMIN);
        pAdrcCtx->Config.Drc_v20.Others.Edge_Weit = LIMIT_VALUE(pCalibDb->Drc_v20.DrcTuningPara.Edge_Weit, ADRCNORMALIZEMAX, ADRCNORMALIZEMIN);
        pAdrcCtx->Config.Drc_v20.Others.IIR_frame = LIMIT_VALUE(pCalibDb->Drc_v20.DrcTuningPara.IIR_frame, IIRFRAMEMAX, IIRFRAMEMIN);
        pAdrcCtx->Config.Drc_v20.Others.Tolerance = LIMIT_VALUE(pCalibDb->Drc_v20.DrcTuningPara.Tolerance, IIRFRAMEMAX, IIRFRAMEMIN);
        pAdrcCtx->Config.Drc_v20.Others.damp = LIMIT_VALUE(pCalibDb->Drc_v20.DrcTuningPara.damp, ADRCNORMALIZEMAX, ADRCNORMALIZEMIN);
    }
    else if(CHECK_ISP_HW_V30()) {
        //enbale
        pAdrcCtx->Config.Drc_v21.Enable  = pCalibDb->Drc_v21.DrcTuningPara.Enable;
        pAdrcCtx->Config.Drc_v21.OutPutLongFrame = pCalibDb->Drc_v21.DrcTuningPara.OutPutLongFrame;

        //drc gain
        for (int i = 0; i < pAdrcCtx->Config.Drc_v21.DrcGain.len; i++ ) {
            pAdrcCtx->Config.Drc_v21.DrcGain.EnvLv[i] = LIMIT_VALUE(pCalibDb->Drc_v21.DrcTuningPara.DrcGain.EnvLv[i], ADRCNORMALIZEMAX, ADRCNORMALIZEMIN);
            pAdrcCtx->Config.Drc_v21.DrcGain.DrcGain[i] = LIMIT_VALUE(pCalibDb->Drc_v21.DrcTuningPara.DrcGain.DrcGain[i], DRCGAINMAX, DRCGAINMIN);
            pAdrcCtx->Config.Drc_v21.DrcGain.Alpha[i] = LIMIT_VALUE(pCalibDb->Drc_v21.DrcTuningPara.DrcGain.Alpha[i], ADRCNORMALIZEMAX, ADRCNORMALIZEMIN);
            pAdrcCtx->Config.Drc_v21.DrcGain.Clip[i] = LIMIT_VALUE(pCalibDb->Drc_v21.DrcTuningPara.DrcGain.Clip[i], CLIPMAX, CLIPMIN);
        }
        //hight ligjt
        for (int i = 0; i < pAdrcCtx->Config.Drc_v21.HiLit.len; i++ ) {
            pAdrcCtx->Config.Drc_v21.HiLit.EnvLv[i] = LIMIT_VALUE(pCalibDb->Drc_v21.DrcTuningPara.HiLight.EnvLv[i], ADRCNORMALIZEMAX, ADRCNORMALIZEMIN);
            pAdrcCtx->Config.Drc_v21.HiLit.Strength[i] = LIMIT_VALUE(pCalibDb->Drc_v21.DrcTuningPara.HiLight.Strength[i], ADRCNORMALIZEMAX, ADRCNORMALIZEMIN);
        }
        // local
        for (int i = 0; i < pAdrcCtx->Config.Drc_v21.Local.len; i++ ) {
            pAdrcCtx->Config.Drc_v21.Local.EnvLv[i] = LIMIT_VALUE(pCalibDb->Drc_v21.DrcTuningPara.LocalSetting.LocalData.EnvLv[i], ADRCNORMALIZEMAX, ADRCNORMALIZEMIN);
            pAdrcCtx->Config.Drc_v21.Local.LocalWeit[i] = LIMIT_VALUE(pCalibDb->Drc_v21.DrcTuningPara.LocalSetting.LocalData.LocalWeit[i], ADRCNORMALIZEMAX, ADRCNORMALIZEMIN);
            pAdrcCtx->Config.Drc_v21.Local.LocalAutoEnable[i] = LIMIT_VALUE(pCalibDb->Drc_v21.DrcTuningPara.LocalSetting.LocalData.LocalAutoEnable[i], ADRCNORMALIZEINTMAX, ADRCNORMALIZEINTMIN);
            pAdrcCtx->Config.Drc_v21.Local.LocalAutoWeit[i] = LIMIT_VALUE(pCalibDb->Drc_v21.DrcTuningPara.LocalSetting.LocalData.LocalAutoWeit[i], ADRCNORMALIZEMAX, ADRCNORMALIZEMIN);
            pAdrcCtx->Config.Drc_v21.Local.GlobalContrast[i] = LIMIT_VALUE(pCalibDb->Drc_v21.DrcTuningPara.LocalSetting.LocalData.GlobalContrast[i], ADRCNORMALIZEMAX, ADRCNORMALIZEMIN);
            pAdrcCtx->Config.Drc_v21.Local.LoLitContrast[i] = LIMIT_VALUE(pCalibDb->Drc_v21.DrcTuningPara.LocalSetting.LocalData.LoLitContrast[i], ADRCNORMALIZEMAX, ADRCNORMALIZEMIN);
        }
        //others
        pAdrcCtx->Config.Drc_v21.Others.curPixWeit = LIMIT_VALUE(pCalibDb->Drc_v21.DrcTuningPara.LocalSetting.curPixWeit, ADRCNORMALIZEMAX, ADRCNORMALIZEMIN);
        pAdrcCtx->Config.Drc_v21.Others.preFrameWeit = LIMIT_VALUE(pCalibDb->Drc_v21.DrcTuningPara.LocalSetting.preFrameWeit, ADRCNORMALIZEMAX, ADRCNORMALIZEMIN);
        pAdrcCtx->Config.Drc_v21.Others.Range_force_sgm = LIMIT_VALUE(pCalibDb->Drc_v21.DrcTuningPara.LocalSetting.Range_force_sgm, ADRCNORMALIZEMAX, ADRCNORMALIZEMIN);
        pAdrcCtx->Config.Drc_v21.Others.Range_sgm_cur = LIMIT_VALUE(pCalibDb->Drc_v21.DrcTuningPara.LocalSetting.Range_sgm_cur, ADRCNORMALIZEMAX, ADRCNORMALIZEMIN);
        pAdrcCtx->Config.Drc_v21.Others.Range_sgm_pre = LIMIT_VALUE(pCalibDb->Drc_v21.DrcTuningPara.LocalSetting.Range_sgm_pre, ADRCNORMALIZEMAX, ADRCNORMALIZEMIN);
        pAdrcCtx->Config.Drc_v21.Others.Space_sgm_cur = LIMIT_VALUE(pCalibDb->Drc_v21.DrcTuningPara.LocalSetting.Space_sgm_cur, SPACESGMMAX, SPACESGMMIN);
        pAdrcCtx->Config.Drc_v21.Others.Space_sgm_pre = LIMIT_VALUE(pCalibDb->Drc_v21.DrcTuningPara.LocalSetting.Space_sgm_pre, SPACESGMMAX, SPACESGMMIN);
        //compress and scale y
        pAdrcCtx->Config.Drc_v21.Compress.Mode = pCalibDb->Drc_v21.DrcTuningPara.CompressSetting.Mode;
        for(int i = 0; i < ADRC_Y_NUM; i++ ) {
            pAdrcCtx->Config.Drc_v21.Others.Scale_y[i] = LIMIT_VALUE(pCalibDb->Drc_v21.DrcTuningPara.Scale_y[i], SCALEYMAX, SCALEYMIN);
            pAdrcCtx->Config.Drc_v21.Compress.Manual_curve[i] = LIMIT_VALUE(pCalibDb->Drc_v21.DrcTuningPara.CompressSetting.Manual_curve[i], MANUALCURVEMAX, MANUALCURVEMIN);
        }
        pAdrcCtx->Config.Drc_v21.Others.ByPassThr = LIMIT_VALUE(pCalibDb->Drc_v21.DrcTuningPara.ByPassThr, ADRCNORMALIZEMAX, ADRCNORMALIZEMIN);
        pAdrcCtx->Config.Drc_v21.Others.Edge_Weit = LIMIT_VALUE(pCalibDb->Drc_v21.DrcTuningPara.Edge_Weit, ADRCNORMALIZEMAX, ADRCNORMALIZEMIN);
        pAdrcCtx->Config.Drc_v21.Others.IIR_frame = LIMIT_VALUE(pCalibDb->Drc_v21.DrcTuningPara.IIR_frame, IIRFRAMEMAX, IIRFRAMEMIN);
        pAdrcCtx->Config.Drc_v21.Others.Tolerance = LIMIT_VALUE(pCalibDb->Drc_v21.DrcTuningPara.Tolerance, IIRFRAMEMAX, IIRFRAMEMIN);
        pAdrcCtx->Config.Drc_v21.Others.damp = LIMIT_VALUE(pCalibDb->Drc_v21.DrcTuningPara.damp, ADRCNORMALIZEMAX, ADRCNORMALIZEMIN);
    }


    LOG1_ATMO( "%s:exit!\n", __FUNCTION__);
}
/******************************************************************************
 * AdrcDamping()
 *****************************************************************************/
void AdrcDamping
(
    CurrData_t* pCurrData,
    AdrcPrevData_t* pPreData,
    drc_OpMode_t opMode,
    int FrameCnt
)
{
    LOG1_ATMO("%s:Enter!\n", __FUNCTION__);
    float Drc_damp = pCurrData->Others.damp;
    bool enDamp;
    bool ifHDRModeChange = pCurrData->frameCnt == pPreData->frameCnt ? false : true;

    if((opMode == DRC_OPMODE_API_OFF || opMode == DRC_OPMODE_AUTO) && FrameCnt != 0 && !ifHDRModeChange)
    {
        float diff = 0.0;

        diff = ABS(pCurrData->EnvLv - pPreData->EnvLv);
        diff = diff / pPreData->EnvLv;
        if (diff < pCurrData->Others.Tolerance)
            enDamp = false;
        else
            enDamp = true;

        //get finnal cfg data by damp
        if (enDamp) {
            pCurrData->HandleData.Drc_v20.DrcGain = Drc_damp * pCurrData->HandleData.Drc_v20.DrcGain
                                                    + (1 - Drc_damp) * pPreData->HandleData.Drc_v20.DrcGain;
            pCurrData->HandleData.Drc_v20.Alpha = Drc_damp * pCurrData->HandleData.Drc_v20.Alpha
                                                  + (1 - Drc_damp) * pPreData->HandleData.Drc_v20.Alpha;
            pCurrData->HandleData.Drc_v20.Clip = Drc_damp * pCurrData->HandleData.Drc_v20.Clip
                                                 + (1 - Drc_damp) * pPreData->HandleData.Drc_v20.Clip;
            pCurrData->HandleData.Drc_v20.Strength = Drc_damp * pCurrData->HandleData.Drc_v20.Strength
                    + (1 - Drc_damp) * pPreData->HandleData.Drc_v20.Strength;
            pCurrData->HandleData.Drc_v20.LocalWeit = Drc_damp * pCurrData->HandleData.Drc_v20.LocalWeit
                    + (1 - Drc_damp) * pPreData->HandleData.Drc_v20.LocalWeit;
            pCurrData->HandleData.Drc_v20.GlobalContrast = Drc_damp * pCurrData->HandleData.Drc_v20.GlobalContrast
                    + (1 - Drc_damp) * pPreData->HandleData.Drc_v20.GlobalContrast;
            pCurrData->HandleData.Drc_v20.LoLitContrast = Drc_damp * pCurrData->HandleData.Drc_v20.LoLitContrast
                    + (1 - Drc_damp) * pPreData->HandleData.Drc_v20.LoLitContrast;
        }


    }

    LOG1_ATMO("%s:Eixt!\n", __FUNCTION__);
}
/******************************************************************************
 * AdrcDamping()
 *****************************************************************************/
void AdrcDampingV30
(
    CurrData_t* pCurrData,
    AdrcPrevData_t* pPreData,
    drc_OpMode_t opMode,
    int FrameCnt
)
{
    LOG1_ATMO("%s:Enter!\n", __FUNCTION__);
    float Drc_damp = pCurrData->Others.damp;
    bool enDamp;
    bool ifHDRModeChange = pCurrData->frameCnt == pPreData->frameCnt ? false : true;

    if((opMode == DRC_OPMODE_API_OFF || opMode == DRC_OPMODE_AUTO) && FrameCnt != 0 && !ifHDRModeChange)
    {
        float diff = 0.0;

        diff = ABS(pCurrData->EnvLv - pPreData->EnvLv);
        diff = diff / pPreData->EnvLv;
        if (diff < pCurrData->Others.Tolerance)
            enDamp = false;
        else
            enDamp = true;

        //get finnal cfg data by damp
        if (enDamp) {
            pCurrData->HandleData.Drc_v21.DrcGain = Drc_damp * pCurrData->HandleData.Drc_v21.DrcGain
                                                    + (1 - Drc_damp) * pPreData->HandleData.Drc_v21.DrcGain;
            pCurrData->HandleData.Drc_v21.Alpha = Drc_damp * pCurrData->HandleData.Drc_v21.Alpha
                                                  + (1 - Drc_damp) * pPreData->HandleData.Drc_v21.Alpha;
            pCurrData->HandleData.Drc_v21.Clip = Drc_damp * pCurrData->HandleData.Drc_v21.Clip
                                                 + (1 - Drc_damp) * pPreData->HandleData.Drc_v21.Clip;
            pCurrData->HandleData.Drc_v21.Strength = Drc_damp * pCurrData->HandleData.Drc_v21.Strength
                    + (1 - Drc_damp) * pPreData->HandleData.Drc_v21.Strength;
            pCurrData->HandleData.Drc_v21.LocalWeit = Drc_damp * pCurrData->HandleData.Drc_v21.LocalWeit
                    + (1 - Drc_damp) * pPreData->HandleData.Drc_v21.LocalWeit;
            pCurrData->HandleData.Drc_v21.LocalAutoWeit = Drc_damp * pCurrData->HandleData.Drc_v21.LocalAutoWeit
                    + (1 - Drc_damp) * pPreData->HandleData.Drc_v21.LocalAutoWeit;
            pCurrData->HandleData.Drc_v21.GlobalContrast = Drc_damp * pCurrData->HandleData.Drc_v21.GlobalContrast
                    + (1 - Drc_damp) * pPreData->HandleData.Drc_v21.GlobalContrast;
            pCurrData->HandleData.Drc_v21.LoLitContrast = Drc_damp * pCurrData->HandleData.Drc_v21.LoLitContrast
                    + (1 - Drc_damp) * pPreData->HandleData.Drc_v21.LoLitContrast;
        }
    }

    LOG1_ATMO("%s:Eixt!\n", __FUNCTION__);
}

/******************************************************************************
 * AdrcGetCurrProcRes()
 *****************************************************************************/
void AdrcGetCurrProcRes
(
    AdrcProcResData_t*           pAdrcProcRes,
    CurrData_t*          pCurrData,
    bool LongFrmMode,
    int FrameNumber,
    int FrameCnt,
    bool OutPutLongFrame
) {
    LOG1_ATMO( "%s:enter!\n", __FUNCTION__);

    //enable
    if(FrameNumber == 2 || FrameNumber == 3) {
        pAdrcProcRes->bTmoEn = true;
        pAdrcProcRes->isLinearTmo = false;
    }
    else if(FrameNumber == 1) {
        pAdrcProcRes->bTmoEn = pCurrData->Enable;
        pAdrcProcRes->isLinearTmo = pAdrcProcRes->bTmoEn;

    }

    //Long Frame mode
    pAdrcProcRes->LongFrameMode = LongFrmMode;

    //Global tmo
    pAdrcProcRes->isHdrGlobalTmo = pCurrData->HandleData.Drc_v20.LocalWeit == 0 ? true : false;

    //compress mode
    pAdrcProcRes->CompressMode = pCurrData->HandleData.Drc_v20.Mode;

    //DrcProcRes
    pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_offset_pow2 = SW_DRC_OFFSET_POW2_FIX;
    pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_position = (int)(SHIFT8BIT(pCurrData->HandleData.Drc_v20.Clip) + 0.5);
    pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_hpdetail_ratio = (int)(SHIFT12BIT(pCurrData->HandleData.Drc_v20.LoLitContrast) + 0.5);
    pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_lpdetail_ratio = (int)(SHIFT12BIT(pCurrData->HandleData.Drc_v20.GlobalContrast) + 0.5);
    pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_weicur_pix = (int)(SHIFT8BIT(pCurrData->Others.curPixWeit) + 0.5);
    pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_weipre_frame = (int)(SHIFT8BIT(pCurrData->Others.preFrameWeit) + 0.5);
    pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_weipre_frame = LIMIT_VALUE(pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_weipre_frame, 255, 0);
    pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_force_sgm_inv0 = (int)(SHIFT13BIT(pCurrData->Others.Range_force_sgm) + 0.5);
    pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_edge_scl = (int)(SHIFT8BIT(pCurrData->Others.Edge_Weit) + 0.5);
    pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_motion_scl = SW_DRC_MOTION_SCL_FIX;
    pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_space_sgm_inv1 = (int)(pCurrData->Others.Space_sgm_cur);
    pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_space_sgm_inv0 = (int)(pCurrData->Others.Space_sgm_pre);
    pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_range_sgm_inv1 = (int)(SHIFT13BIT(pCurrData->Others.Range_sgm_cur) + 0.5);
    pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_range_sgm_inv0 = (int)(SHIFT13BIT(pCurrData->Others.Range_sgm_pre) + 0.5);
    pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_weig_maxl = (int)(SHIFT4BIT(pCurrData->HandleData.Drc_v20.Strength) + 0.5);
    pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_adrc_gain = pCurrData->HandleData.Drc_v20.DrcGain;
    pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_weig_bilat = (int)(SHIFT4BIT(pCurrData->HandleData.Drc_v20.LocalWeit) + 0.5);
    pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_min_ogain = OutPutLongFrame ? 1 : 0;
    for(int i = 0; i < ADRC_Y_NUM; ++i) {
        pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_scale_y[i] = (int)(pCurrData->Others.Scale_y[i]) ;
        pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_compres_y[i] = pCurrData->HandleData.Drc_v20.Manual_curve[i] ;
    }

    //get sw_drc_gain_y
    CalibrateDrcGainY(&pAdrcProcRes->DrcProcRes, pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_adrc_gain, pCurrData->HandleData.Drc_v20.Alpha) ;

    float iir_frame = (float)(MIN(FrameCnt + 1, pCurrData->Others.IIR_frame));
    pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_iir_weight = (int)(SHIFT6BIT((iir_frame - 1) / iir_frame) + 0.5);

    pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_compres_scl =  20000.0;

    //sw_drc_delta_scalein FIX
    int ISP_RAW_BIT = 12;
    int MFHDR_LOG_Q_BITS = 11;
    int dstbits = ISP_RAW_BIT << MFHDR_LOG_Q_BITS;
    int offsetbits_int = SW_DRC_OFFSET_POW2_FIX;
    int offsetbits = offsetbits_int << MFHDR_LOG_Q_BITS;
    int validbits = dstbits - offsetbits;
    int delta_scalein = (256 << MFHDR_LOG_Q_BITS) / validbits;
    pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_delta_scalein = delta_scalein;

    LOGD_ATMO("%s:Current damp Enable:%d DrcGain:%f Alpha:%f Clip:%f Strength:%f LocalWeit:%f GlobalContrast:%f LoLitContrast:%f CompressMode:%d\n", __FUNCTION__, pAdrcProcRes->bTmoEn,
              pCurrData->HandleData.Drc_v20.DrcGain, pCurrData->HandleData.Drc_v20.Alpha, pCurrData->HandleData.Drc_v20.Clip, pCurrData->HandleData.Drc_v20.Strength,
              pCurrData->HandleData.Drc_v20.LocalWeit, pCurrData->HandleData.Drc_v20.GlobalContrast, pCurrData->HandleData.Drc_v20.LoLitContrast, pCurrData->HandleData.Drc_v20.Mode);

    LOGV_ATMO("%s:  sw_drc_adrc_gain:%f Alpha:%f\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_adrc_gain, pCurrData->HandleData.Drc_v20.Alpha);
    LOGV_ATMO("%s:  sw_drc_offset_pow2:%d\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_offset_pow2);
    LOGV_ATMO("%s:  sw_drc_compres_scl:%d\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_compres_scl);
    LOGV_ATMO("%s:  sw_drc_position:%d\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_position);
    LOGV_ATMO("%s:  sw_drc_delta_scalein:%d\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_delta_scalein);
    LOGV_ATMO("%s:  sw_drc_hpdetail_ratio:%d\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_hpdetail_ratio);
    LOGV_ATMO("%s:  sw_drc_lpdetail_ratio:%d\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_lpdetail_ratio);
    LOGV_ATMO("%s:  sw_drc_weicur_pix:%d\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_weicur_pix);
    LOGV_ATMO("%s:  sw_drc_weipre_frame:%d\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_weipre_frame);
    LOGV_ATMO("%s:  sw_drc_force_sgm_inv0:%d\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_force_sgm_inv0);
    LOGV_ATMO("%s:  sw_drc_edge_scl:%d\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_edge_scl);
    LOGV_ATMO("%s:  sw_drc_space_sgm_inv1:%d\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_space_sgm_inv1);
    LOGV_ATMO("%s:  sw_drc_space_sgm_inv0:%d\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_space_sgm_inv0);
    LOGV_ATMO("%s:  sw_drc_range_sgm_inv1:%d\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_range_sgm_inv1);
    LOGV_ATMO("%s:  sw_drc_range_sgm_inv0:%d\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_range_sgm_inv0);
    LOGV_ATMO("%s:  sw_drc_weig_maxl:%d\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_weig_maxl);
    LOGV_ATMO("%s:  sw_drc_weig_bilat:%d\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_weig_bilat);
    LOGV_ATMO("%s:  sw_drc_adrc_gain:%f\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_adrc_gain);
    LOGV_ATMO("%s:  sw_drc_iir_weight:%d\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_iir_weight);
    LOGV_ATMO("%s:  sw_drc_min_ogain:%d\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_min_ogain);
    for(int i = 0; i < 17; i++)
        LOGV_ATMO("%s:  sw_drc_gain_y[%d]:%d\n", __FUNCTION__, i, pAdrcProcRes->DrcProcRes.Drc_v20.sw_drc_gain_y[i]);

    LOG1_ATMO( "%s:exit!\n", __FUNCTION__);
}

/******************************************************************************
 * AdrcGetCurrProcRes()
 *****************************************************************************/
void AdrcGetCurrProcResV30
(
    AdrcProcResData_t*           pAdrcProcRes,
    CurrData_t*          pCurrData,
    bool LongFrmMode,
    int FrameNumber,
    int FrameCnt,
    bool OutPutLongFrame
) {
    LOG1_ATMO( "%s:enter!\n", __FUNCTION__);

    //enable
    if(FrameNumber == 2 || FrameNumber == 3) {
        pAdrcProcRes->bTmoEn = true;
        pAdrcProcRes->isLinearTmo = false;
    }
    else if(FrameNumber == 1) {
        pAdrcProcRes->bTmoEn = pCurrData->Enable;
        pAdrcProcRes->isLinearTmo = pAdrcProcRes->bTmoEn;

    }

    //Long Frame mode
    pAdrcProcRes->LongFrameMode = LongFrmMode;

    //Global tmo
    pAdrcProcRes->isHdrGlobalTmo = pCurrData->HandleData.Drc_v21.LocalWeit == 0 ? true : false;

    //compress mode
    pAdrcProcRes->CompressMode = pCurrData->HandleData.Drc_v21.Mode;

    //DrcProcRes
    pAdrcProcRes->DrcProcRes.Drc_v21.bypass_en = 0;
    pAdrcProcRes->DrcProcRes.Drc_v21.offset_pow2 = SW_DRC_OFFSET_POW2_FIX;
    pAdrcProcRes->DrcProcRes.Drc_v21.position = (int)(SHIFT8BIT(pCurrData->HandleData.Drc_v21.Clip) + 0.5);
    pAdrcProcRes->DrcProcRes.Drc_v21.hpdetail_ratio = (int)(SHIFT12BIT(pCurrData->HandleData.Drc_v21.LoLitContrast) + 0.5);
    pAdrcProcRes->DrcProcRes.Drc_v21.lpdetail_ratio = (int)(SHIFT12BIT(pCurrData->HandleData.Drc_v21.GlobalContrast) + 0.5);
    pAdrcProcRes->DrcProcRes.Drc_v21.weicur_pix = (int)(SHIFT8BIT(pCurrData->Others.curPixWeit) + 0.5);
    pAdrcProcRes->DrcProcRes.Drc_v21.weipre_frame = (int)(SHIFT8BIT(pCurrData->Others.preFrameWeit) + 0.5);
    pAdrcProcRes->DrcProcRes.Drc_v21.weipre_frame = LIMIT_VALUE(pAdrcProcRes->DrcProcRes.Drc_v21.weipre_frame, INT8BITMAX, 0);
    pAdrcProcRes->DrcProcRes.Drc_v21.bilat_wt_off = SW_DRC_BILAT_WT_OFF_FIX;//LIMIT_VALUE(pCurrData->HandleData.Drc_v21.MotionStr * INT8BITMAX, INT8BITMAX, 0);
    pAdrcProcRes->DrcProcRes.Drc_v21.force_sgm_inv0 = (int)(SHIFT13BIT(pCurrData->Others.Range_force_sgm) + 0.5);
    pAdrcProcRes->DrcProcRes.Drc_v21.edge_scl = (int)(SHIFT8BIT(pCurrData->Others.Edge_Weit) + 0.5);
    pAdrcProcRes->DrcProcRes.Drc_v21.motion_scl = SW_DRC_MOTION_SCL_FIX;
    pAdrcProcRes->DrcProcRes.Drc_v21.space_sgm_inv1 = (int)(pCurrData->Others.Space_sgm_cur);
    pAdrcProcRes->DrcProcRes.Drc_v21.space_sgm_inv0 = (int)(pCurrData->Others.Space_sgm_pre);
    pAdrcProcRes->DrcProcRes.Drc_v21.range_sgm_inv1 = (int)(SHIFT13BIT(pCurrData->Others.Range_sgm_cur) + 0.5);
    pAdrcProcRes->DrcProcRes.Drc_v21.range_sgm_inv0 = (int)(SHIFT13BIT(pCurrData->Others.Range_sgm_pre) + 0.5);
    pAdrcProcRes->DrcProcRes.Drc_v21.weig_maxl = (int)(SHIFT4BIT(pCurrData->HandleData.Drc_v21.Strength) + 0.5);
    pAdrcProcRes->DrcProcRes.Drc_v21.adrc_gain = pCurrData->HandleData.Drc_v21.DrcGain;
    pAdrcProcRes->DrcProcRes.Drc_v21.weig_bilat = (int)(SHIFT4BIT(pCurrData->HandleData.Drc_v21.LocalWeit) + 0.5);
    pAdrcProcRes->DrcProcRes.Drc_v21.enable_soft_thd = pCurrData->HandleData.Drc_v21.LocalAutoEnable;
    pAdrcProcRes->DrcProcRes.Drc_v21.bilat_soft_thd = (int)(SHIFT14BIT(pCurrData->HandleData.Drc_v21.LocalAutoWeit) + 0.5);
    pAdrcProcRes->DrcProcRes.Drc_v21.bilat_soft_thd = LIMIT_VALUE(pAdrcProcRes->DrcProcRes.Drc_v21.bilat_soft_thd, INT14BITMAX, 0);
    pAdrcProcRes->DrcProcRes.Drc_v21.min_ogain = OutPutLongFrame ? 1 : 0;
    for(int i = 0; i < ADRC_Y_NUM; ++i) {
        pAdrcProcRes->DrcProcRes.Drc_v21.scale_y[i] = (int)(pCurrData->Others.Scale_y[i]) ;
        pAdrcProcRes->DrcProcRes.Drc_v21.compres_y[i] = pCurrData->HandleData.Drc_v21.Manual_curve[i] ;
    }

    //get sw_drc_gain_y
    CalibrateDrcGainY(&pAdrcProcRes->DrcProcRes, pAdrcProcRes->DrcProcRes.Drc_v21.adrc_gain, pCurrData->HandleData.Drc_v21.Alpha) ;

    pAdrcProcRes->DrcProcRes.Drc_v21.wr_cycle = 0;
    float iir_frame = (float)(MIN(FrameCnt + 1, pCurrData->Others.IIR_frame));
    pAdrcProcRes->DrcProcRes.Drc_v21.iir_weight = (int)(SHIFT6BIT((iir_frame - 1) / iir_frame) + 0.5);

    pAdrcProcRes->DrcProcRes.Drc_v21.compres_scl =  20000.0;

    //sw_drc_delta_scalein FIX
    int ISP_RAW_BIT = 12;
    int MFHDR_LOG_Q_BITS = 11;
    int dstbits = ISP_RAW_BIT << MFHDR_LOG_Q_BITS;
    int offsetbits_int = SW_DRC_OFFSET_POW2_FIX;
    int offsetbits = offsetbits_int << MFHDR_LOG_Q_BITS;
    int validbits = dstbits - offsetbits;
    int delta_scalein = (256 << MFHDR_LOG_Q_BITS) / validbits;
    pAdrcProcRes->DrcProcRes.Drc_v21.delta_scalein = delta_scalein;

    LOGD_ATMO("%s:Current damp Enable:%d DrcGain:%f Alpha:%f Clip:%f Strength:%f LocalWeit:%f GlobalContrast:%f LoLitContrast:%f CompressMode:%d\n", __FUNCTION__, pAdrcProcRes->bTmoEn,
              pCurrData->HandleData.Drc_v21.DrcGain, pCurrData->HandleData.Drc_v21.Alpha, pCurrData->HandleData.Drc_v21.Clip, pCurrData->HandleData.Drc_v21.Strength,
              pCurrData->HandleData.Drc_v21.LocalWeit, pCurrData->HandleData.Drc_v21.GlobalContrast, pCurrData->HandleData.Drc_v21.LoLitContrast, pCurrData->HandleData.Drc_v21.Mode);

    LOGV_ATMO("%s:  sw_drc_adrc_gain:%f Alpha:%f\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v21.adrc_gain, pCurrData->HandleData.Drc_v21.Alpha);
    LOGV_ATMO("%s:  sw_drc_offset_pow2:%d\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v21.offset_pow2);
    LOGV_ATMO("%s:  sw_drc_compres_scl:%d\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v21.compres_scl);
    LOGV_ATMO("%s:  sw_drc_position:%d\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v21.position);
    LOGV_ATMO("%s:  sw_drc_delta_scalein:%d\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v21.delta_scalein);
    LOGV_ATMO("%s:  sw_drc_hpdetail_ratio:%d\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v21.hpdetail_ratio);
    LOGV_ATMO("%s:  sw_drc_lpdetail_ratio:%d\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v21.lpdetail_ratio);
    LOGV_ATMO("%s:  sw_drc_weicur_pix:%d\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v21.weicur_pix);
    LOGV_ATMO("%s:  sw_drc_weipre_frame:%d\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v21.weipre_frame);
    LOGV_ATMO("%s:  sw_drc_force_sgm_inv0:%d\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v21.force_sgm_inv0);
    LOGV_ATMO("%s:  sw_drc_edge_scl:%d\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v21.edge_scl);
    LOGV_ATMO("%s:  sw_drc_space_sgm_inv1:%d\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v21.space_sgm_inv1);
    LOGV_ATMO("%s:  sw_drc_space_sgm_inv0:%d\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v21.space_sgm_inv0);
    LOGV_ATMO("%s:  sw_drc_range_sgm_inv1:%d\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v21.range_sgm_inv1);
    LOGV_ATMO("%s:  sw_drc_range_sgm_inv0:%d\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v21.range_sgm_inv0);
    LOGV_ATMO("%s:  sw_drc_weig_maxl:%d\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v21.weig_maxl);
    LOGV_ATMO("%s:  sw_drc_weig_bilat:%d\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v21.weig_bilat);
    LOGV_ATMO("%s:  sw_drc_adrc_gain:%f\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v21.adrc_gain);
    LOGV_ATMO("%s:  sw_drc_enable_soft_thd:%d\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v21.enable_soft_thd);
    LOGV_ATMO("%s:  sw_drc_bilat_soft_thd:%d\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v21.bilat_soft_thd);
    LOGV_ATMO("%s:  sw_drc_iir_weight:%d\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v21.iir_weight);
    LOGV_ATMO("%s:  sw_drc_min_ogain:%d\n", __FUNCTION__, pAdrcProcRes->DrcProcRes.Drc_v21.min_ogain);
    for(int i = 0; i < 17; i++)
        LOGV_ATMO("%s:  sw_drc_gain_y[%d]:%d\n", __FUNCTION__, i, pAdrcProcRes->DrcProcRes.Drc_v21.gain_y[i]);

    LOG1_ATMO( "%s:exit!\n", __FUNCTION__);
}

/******************************************************************************
 * AdrcGetCurrData()
 *****************************************************************************/
void AdrcGetCurrData
(
    AdrcContext_t* pAdrcCtx
) {

    LOG1_ATMO( "%s:enter!\n", __FUNCTION__);

    //damp
    AdrcDamping(&pAdrcCtx->CurrData, &pAdrcCtx->PrevData, pAdrcCtx->drcAttr.opMode, pAdrcCtx->frameCnt);

    //get io data
    AdrcGetCurrProcRes(&pAdrcCtx->AdrcProcRes, &pAdrcCtx->CurrData, pAdrcCtx->SensorInfo.LongFrmMode,
                       pAdrcCtx->FrameNumber, pAdrcCtx->frameCnt, pAdrcCtx->Config.Drc_v20.OutPutLongFrame);

    // store current handle data to pre data for next loop
    pAdrcCtx->PrevData.EnvLv = pAdrcCtx->CurrData.EnvLv;
    pAdrcCtx->PrevData.ISO = pAdrcCtx->CurrData.ISO;
    if(0 != memcmp(&pAdrcCtx->PrevData.HandleData, &pAdrcCtx->CurrData.HandleData, sizeof(DrcHandleData_t)))
        memcpy(&pAdrcCtx->PrevData.HandleData, &pAdrcCtx->CurrData.HandleData, sizeof(DrcHandleData_t));

    LOG1_ATMO( "%s:exit!\n", __FUNCTION__);
}

/******************************************************************************
 * AdrcGetCurrData()
 *****************************************************************************/
void AdrcGetCurrDataV30
(
    AdrcContext_t* pAdrcCtx
) {

    LOG1_ATMO( "%s:enter!\n", __FUNCTION__);

    //damp
    AdrcDampingV30(&pAdrcCtx->CurrData, &pAdrcCtx->PrevData, pAdrcCtx->drcAttr.opMode, pAdrcCtx->frameCnt);

    //get io data
    AdrcGetCurrProcResV30(&pAdrcCtx->AdrcProcRes, &pAdrcCtx->CurrData, pAdrcCtx->SensorInfo.LongFrmMode,
                          pAdrcCtx->FrameNumber, pAdrcCtx->frameCnt, pAdrcCtx->Config.Drc_v21.OutPutLongFrame);

    // store current handle data to pre data for next loop
    pAdrcCtx->PrevData.EnvLv = pAdrcCtx->CurrData.EnvLv;
    pAdrcCtx->PrevData.ISO = pAdrcCtx->CurrData.ISO;
    pAdrcCtx->PrevData.MotionCoef = pAdrcCtx->CurrData.MotionCoef;
    if(0 != memcmp(&pAdrcCtx->PrevData.HandleData, &pAdrcCtx->CurrData.HandleData, sizeof(DrcHandleData_t)))
        memcpy(&pAdrcCtx->PrevData.HandleData, &pAdrcCtx->CurrData.HandleData, sizeof(DrcHandleData_t));

    LOG1_ATMO( "%s:exit!\n", __FUNCTION__);
}

/******************************************************************************
 * AdrcProcessing()
 *get handle para by config and current variate
 *****************************************************************************/
void AdrcProcessingV20
(
    AdrcContext_t*     pAdrcCtx
)
{
    LOG1_ATMO("%s:enter!\n", __FUNCTION__);
    pAdrcCtx->CurrData.frameCnt = pAdrcCtx->frameCnt;

    //enable setting
    if(pAdrcCtx->drcAttr.Enable)
        pAdrcCtx->CurrData.Enable = true;
    else
        pAdrcCtx->CurrData.Enable = pAdrcCtx->Config.Drc_v20.Enable ;

    //para setting
    AdrcIQUpdate(pAdrcCtx);

    //api
    if(pAdrcCtx->drcAttr.opMode == DRC_OPMODE_API_OFF)
        LOGD_ATMO("%s:  Adrc api OFF!! Current Handle data:\n", __FUNCTION__);
    else if(pAdrcCtx->drcAttr.opMode == DRC_OPMODE_MANU) {
        LOGD_ATMO("%s:  Adrc api Manual!! Current Handle data:\n", __FUNCTION__);
        AdrcApiManualUpdate(&pAdrcCtx->CurrData, &pAdrcCtx->drcAttr.stManual);
    } else if(pAdrcCtx->drcAttr.opMode == DRC_OPMODE_AUTO)
        LOGD_ATMO("%s:  Adrc api ATUO!! Current Handle data:\n", __FUNCTION__);
    else if(pAdrcCtx->drcAttr.opMode == DRC_OPMODE_DRC_GAIN) {
        LOGD_ATMO("%s:  Adrc api GDRC Gain!! Current Handle data:\n", __FUNCTION__);
        pAdrcCtx->CurrData.HandleData.Drc_v20.DrcGain = pAdrcCtx->drcAttr.stManual.DrcGain.DrcGain;
        pAdrcCtx->CurrData.HandleData.Drc_v20.Alpha = pAdrcCtx->drcAttr.stManual.DrcGain.Alpha;
        pAdrcCtx->CurrData.HandleData.Drc_v20.Clip = pAdrcCtx->drcAttr.stManual.DrcGain.Clip;
    }
    else if(pAdrcCtx->drcAttr.opMode == DRC_OPMODE_HILIT) {
        LOGD_ATMO("%s:  Adrc api HiLit!! Current Handle data:\n", __FUNCTION__);
        pAdrcCtx->CurrData.HandleData.Drc_v20.Strength = pAdrcCtx->drcAttr.stManual.HiLit.Strength;
    }
    else if(pAdrcCtx->drcAttr.opMode == DRC_OPMODE_LOCAL_TMO) {
        LOGD_ATMO("%s:  Adrc api Local TMO!! Current Handle data:\n", __FUNCTION__);
        pAdrcCtx->CurrData.HandleData.Drc_v20.LocalWeit = pAdrcCtx->drcAttr.stManual.Local.LocalWeit;
        pAdrcCtx->CurrData.HandleData.Drc_v20.GlobalContrast = pAdrcCtx->drcAttr.stManual.Local.GlobalContrast;
        pAdrcCtx->CurrData.HandleData.Drc_v20.LoLitContrast = pAdrcCtx->drcAttr.stManual.Local.LoLitContrast;
    }
    else if(pAdrcCtx->drcAttr.opMode == DRC_OPMODE_COMPRESS) {
        LOGD_ATMO("%s:  Adrc api Compress!! Current Handle data:\n", __FUNCTION__);
        pAdrcCtx->CurrData.HandleData.Drc_v20.Mode = pAdrcCtx->drcAttr.stManual.Compress.Mode;
        for(int i = 0; i < ADRC_Y_NUM; i++)
            pAdrcCtx->CurrData.HandleData.Drc_v20.Manual_curve[i] = pAdrcCtx->drcAttr.stManual.Compress.Manual_curve[i];
    }
    else
        LOGE_ATMO("%s:  Adrc wrong mode!!!\n", __FUNCTION__);

    //clip drc gain
    if(pAdrcCtx->CurrAeResult.AEMaxRatio * pAdrcCtx->CurrData.HandleData.Drc_v20.DrcGain > MAX_AE_DRC_GAIN) {
        LOGE_ATMO("%s:  AERatio*DrcGain > 256!!!\n", __FUNCTION__);
        pAdrcCtx->CurrData.HandleData.Drc_v20.DrcGain = MAX_AE_DRC_GAIN / pAdrcCtx->CurrAeResult.AEMaxRatio;
    }

    LOGD_ATMO("%s:Current Enable:%d DrcGain:%f Alpha:%f Clip:%f Strength:%f LocalWeit:%f GlobalContrast:%f LoLitContrast:%f CompressMode:%d\n", __FUNCTION__, pAdrcCtx->CurrData.Enable,
              pAdrcCtx->CurrData.HandleData.Drc_v20.DrcGain, pAdrcCtx->CurrData.HandleData.Drc_v20.Alpha, pAdrcCtx->CurrData.HandleData.Drc_v20.Clip, pAdrcCtx->CurrData.HandleData.Drc_v20.Strength,
              pAdrcCtx->CurrData.HandleData.Drc_v20.LocalWeit, pAdrcCtx->CurrData.HandleData.Drc_v20.GlobalContrast, pAdrcCtx->CurrData.HandleData.Drc_v20.LoLitContrast
              , pAdrcCtx->CurrData.HandleData.Drc_v20.Mode);

    //transfer data to api
    AdrcTranferData2Api(pAdrcCtx);

    //get io and store data in proc res
    AdrcGetCurrData(pAdrcCtx);


    LOG1_ATMO( "%s:exit!\n", __FUNCTION__);
}

/******************************************************************************
 * AdrcProcessing()
 *get handle para by config and current variate
 *****************************************************************************/
void AdrcProcessingV30
(
    AdrcContext_t*     pAdrcCtx
)
{
    LOG1_ATMO("%s:enter!\n", __FUNCTION__);
    pAdrcCtx->CurrData.frameCnt = pAdrcCtx->frameCnt;


    //enable setting
    if(pAdrcCtx->drcAttr.Enable)
        pAdrcCtx->CurrData.Enable = true;
    else
        pAdrcCtx->CurrData.Enable = pAdrcCtx->Config.Drc_v21.Enable ;

    //para setting
    AdrcIQUpdateV30(pAdrcCtx);

    //api
    if(pAdrcCtx->drcAttr.opMode == DRC_OPMODE_API_OFF)
        LOG1_ATMO("%s: Adrc api OFF!! Current Handle data:\n", __FUNCTION__);
    else if(pAdrcCtx->drcAttr.opMode == DRC_OPMODE_MANU) {
        LOG1_ATMO("%s: Adrc api Manual!! Current Handle data:\n", __FUNCTION__);
        //AdrcApiManualUpdate(&pAdrcCtx->CurrData, &pAdrcCtx->drcAttr.stManual);
    } else if(pAdrcCtx->drcAttr.opMode == DRC_OPMODE_AUTO)
        LOG1_ATMO("%s: Adrc api ATUO!! Current Handle data:\n", __FUNCTION__);
    else if(pAdrcCtx->drcAttr.opMode == DRC_OPMODE_DRC_GAIN) {
        LOGD_ATMO("%s: Data from api: DrcGain:%f Alpha:% Clip:%f\n", __FUNCTION__, pAdrcCtx->drcAttr.stManual.DrcGain.DrcGain,
                  pAdrcCtx->drcAttr.stManual.DrcGain.Alpha, pAdrcCtx->drcAttr.stManual.DrcGain.Clip);
        pAdrcCtx->CurrData.HandleData.Drc_v21.DrcGain = pAdrcCtx->drcAttr.stManual.DrcGain.DrcGain;
        pAdrcCtx->CurrData.HandleData.Drc_v21.Alpha = pAdrcCtx->drcAttr.stManual.DrcGain.Alpha;
        pAdrcCtx->CurrData.HandleData.Drc_v21.Clip = pAdrcCtx->drcAttr.stManual.DrcGain.Clip;
    }
    else if(pAdrcCtx->drcAttr.opMode == DRC_OPMODE_HILIT) {
        LOGD_ATMO("%s: Data from api: HiLitStrength:%f\n", __FUNCTION__, pAdrcCtx->drcAttr.stManual.HiLit.Strength);
        pAdrcCtx->CurrData.HandleData.Drc_v21.Strength = pAdrcCtx->drcAttr.stManual.HiLit.Strength;
    }
    else if(pAdrcCtx->drcAttr.opMode == DRC_OPMODE_LOCAL_TMO) {
        LOGD_ATMO("%s: Data from api: LocalWeit::%f GlobalContrast:%f LoLitContrast:%f\n", __FUNCTION__, pAdrcCtx->drcAttr.stManual.Local.LocalWeit,
                  pAdrcCtx->drcAttr.stManual.Local.GlobalContrast, pAdrcCtx->drcAttr.stManual.Local.LoLitContrast);
        pAdrcCtx->CurrData.HandleData.Drc_v21.LocalWeit = pAdrcCtx->drcAttr.stManual.Local.LocalWeit;
        pAdrcCtx->CurrData.HandleData.Drc_v21.GlobalContrast = pAdrcCtx->drcAttr.stManual.Local.GlobalContrast;
        pAdrcCtx->CurrData.HandleData.Drc_v21.LoLitContrast = pAdrcCtx->drcAttr.stManual.Local.LoLitContrast;
    }
    else if(pAdrcCtx->drcAttr.opMode == DRC_OPMODE_COMPRESS) {
        LOGD_ATMO("%s:Data from api: CompressMode::%d\n", __FUNCTION__, pAdrcCtx->drcAttr.stManual.Compress.Mode);
        pAdrcCtx->CurrData.HandleData.Drc_v21.Mode = pAdrcCtx->drcAttr.stManual.Compress.Mode;
        for(int i = 0; i < ADRC_Y_NUM; i++)
            pAdrcCtx->CurrData.HandleData.Drc_v21.Manual_curve[i] = pAdrcCtx->drcAttr.stManual.Compress.Manual_curve[i];
    }
    else
        LOGE_ATMO("%s:  Adrc wrong mode!!!\n", __FUNCTION__);

    //clip drc gain
    if(pAdrcCtx->CurrAeResult.AEMaxRatio * pAdrcCtx->CurrData.HandleData.Drc_v21.DrcGain > MAX_AE_DRC_GAIN) {
        LOGE_ATMO("%s:  AERatio*DrcGain > 256!!!\n", __FUNCTION__);
        pAdrcCtx->CurrData.HandleData.Drc_v21.DrcGain = MAX_AE_DRC_GAIN / pAdrcCtx->CurrAeResult.AEMaxRatio;
    }

    LOGD_ATMO("%s:Current Enable:%d DrcGain:%f Alpha:%f Clip:%f Strength:%f LocalWeit:%f GlobalContrast:%f LoLitContrast:%f CompressMode:%d\n", __FUNCTION__, pAdrcCtx->CurrData.Enable,
              pAdrcCtx->CurrData.HandleData.Drc_v21.DrcGain, pAdrcCtx->CurrData.HandleData.Drc_v21.Alpha, pAdrcCtx->CurrData.HandleData.Drc_v21.Clip, pAdrcCtx->CurrData.HandleData.Drc_v21.Strength,
              pAdrcCtx->CurrData.HandleData.Drc_v21.LocalWeit, pAdrcCtx->CurrData.HandleData.Drc_v21.GlobalContrast, pAdrcCtx->CurrData.HandleData.Drc_v21.LoLitContrast
              , pAdrcCtx->CurrData.HandleData.Drc_v21.Mode);

    //transfer data to api
    AdrcTranferData2ApiV30(pAdrcCtx);

    //get io and store data in proc res
    AdrcGetCurrDataV30(pAdrcCtx);

    LOG1_ATMO( "%s:exit!\n", __FUNCTION__);
}

/******************************************************************************
 * AdrcProcessing()
 *get handle para by config and current variate
 *****************************************************************************/
void AdrcProcessing
(
    AdrcContext_t*     pAdrcCtx
)
{
    LOG1_ATMO("%s:enter!\n", __FUNCTION__);
    pAdrcCtx->CurrData.frameCnt = pAdrcCtx->frameCnt;

    if(CHECK_ISP_HW_V21())
        AdrcProcessingV20(pAdrcCtx);
    else if(CHECK_ISP_HW_V30())
        AdrcProcessingV30(pAdrcCtx);

    LOG1_ATMO( "%s:exit!\n", __FUNCTION__);
}

/******************************************************************************
 * AdrcByPassProcessing()
 *get handle para by config and current variate
 *****************************************************************************/
bool AdrcByPassProcessing
(
    AdrcContext_t*     pAdrcCtx,
    AecPreResult_t  AecHdrPreResult
)
{
    LOG1_ATMO("%s:enter!\n", __FUNCTION__);

    bool bypass = false;
    float diff = 0.0;

    float ByPassThr = 0.0f;
    if(CHECK_ISP_HW_V21())
        ByPassThr = pAdrcCtx->Config.Drc_v20.Others.ByPassThr;
    else if(CHECK_ISP_HW_V30())
        ByPassThr = pAdrcCtx->Config.Drc_v21.Others.ByPassThr;

    if(pAdrcCtx->frameCnt <= 2)//start frame
        bypass = false;
    else if(pAdrcCtx->drcAttr.opMode > DRC_OPMODE_API_OFF)//api
        bypass = false;
    else if(pAdrcCtx->drcAttr.opMode !=  pAdrcCtx->PrevData.ApiMode)//api change
        bypass = false;
    else { //EnvLv change
        //get current ae data from AecPreRes
        AdrcGetAeResult(pAdrcCtx, AecHdrPreResult);

        //motion coef
        pAdrcCtx->CurrData.MotionCoef = 0;

        //transfer ae data to CurrHandle
        pAdrcCtx->CurrData.EnvLv = LIMIT_VALUE(pAdrcCtx->CurrData.EnvLv, ADRCNORMALIZEMAX, ADRCNORMALIZEMIN);

        pAdrcCtx->CurrData.ISO = pAdrcCtx->CurrAeResult.ISO;
        pAdrcCtx->CurrData.ISO = LIMIT_VALUE(pAdrcCtx->CurrData.ISO, ISOMAX, ISOMIN);

        //use Envlv for now
        diff = pAdrcCtx->PrevData.EnvLv - pAdrcCtx->CurrData.EnvLv;
        if(pAdrcCtx->PrevData.EnvLv == 0.0) {
            diff = pAdrcCtx->CurrData.EnvLv;
            if(diff == 0.0)
                bypass = true;
            else
                bypass = false;
        }
        else {
            diff /= pAdrcCtx->PrevData.EnvLv;
            if(diff >= ByPassThr || diff <= (0 - ByPassThr))
                bypass = false;
            else
                bypass = true;
        }
    }

    LOGD_ATMO( "%s: FrameID:%d HDRFrameNum:%d DRCApiMode:%d EnvLv:%f bypass:%d H\n", __FUNCTION__, pAdrcCtx->frameCnt, pAdrcCtx->FrameNumber, pAdrcCtx->drcAttr.opMode,
               pAdrcCtx->CurrData.EnvLv, bypass);

    LOG1_ATMO( "%s:CtrlEnvLv:%f PrevEnvLv:%f diff:%f ByPassThr:%f opMode:%d bypass:%d!\n", __FUNCTION__, pAdrcCtx->CurrData.EnvLv,
               pAdrcCtx->PrevData.EnvLv, diff, ByPassThr, pAdrcCtx->drcAttr.opMode, bypass);

    LOG1_ATMO( "%s:exit!\n", __FUNCTION__);
    return bypass;
}

/******************************************************************************
 * AdrcInit()
 *****************************************************************************/
XCamReturn AdrcInit
(
    AdrcContext_t **ppAdrcCtx,
    CamCalibDbV2Context_t *pCalibDb
) {
    LOG1_ATMO("%s:enter!\n", __FUNCTION__);
    XCamReturn result = XCAM_RETURN_NO_ERROR;

    AdrcContext_t * pAdrcCtx;
    pAdrcCtx = (AdrcContext_t *)malloc(sizeof(AdrcContext_t));
    if(pAdrcCtx == NULL) {
        LOGE_ATMO("%s(%d): invalid inputparams\n", __FUNCTION__, __LINE__);
        return XCAM_RETURN_ERROR_MEM;
    }

    memset(pAdrcCtx, 0x00, sizeof(AdrcContext_t));
    *ppAdrcCtx = pAdrcCtx;
    pAdrcCtx->state = ADRC_STATE_INITIALIZED;

    if(CHECK_ISP_HW_V21()) {
        CalibDbV2_drc_t* calibv2_adrc_calib =
            (CalibDbV2_drc_t*)(CALIBDBV2_GET_MODULE_PTR(pCalibDb, adrc_calib));

        // pre-initialize context
        memset(pAdrcCtx, 0x00, sizeof(*pAdrcCtx));

        //malloc DrcGain
        pAdrcCtx->Config.Drc_v20.DrcGain.len = calibv2_adrc_calib->DrcTuningPara.DrcGain.EnvLv_len;
        pAdrcCtx->Config.Drc_v20.DrcGain.EnvLv = (float*)malloc(sizeof(float) * (calibv2_adrc_calib->DrcTuningPara.DrcGain.EnvLv_len));
        pAdrcCtx->Config.Drc_v20.DrcGain.DrcGain = (float*)malloc(sizeof(float) * (calibv2_adrc_calib->DrcTuningPara.DrcGain.EnvLv_len));
        pAdrcCtx->Config.Drc_v20.DrcGain.Alpha = (float*)malloc(sizeof(float) * (calibv2_adrc_calib->DrcTuningPara.DrcGain.EnvLv_len));
        pAdrcCtx->Config.Drc_v20.DrcGain.Clip = (float*)malloc(sizeof(float) * (calibv2_adrc_calib->DrcTuningPara.DrcGain.EnvLv_len));

        //malloc HiLit
        pAdrcCtx->Config.Drc_v20.HiLit.len = calibv2_adrc_calib->DrcTuningPara.HiLight.EnvLv_len;
        pAdrcCtx->Config.Drc_v20.HiLit.EnvLv = (float*)malloc(sizeof(float) * (calibv2_adrc_calib->DrcTuningPara.HiLight.EnvLv_len));
        pAdrcCtx->Config.Drc_v20.HiLit.Strength = (float*)malloc(sizeof(float) * (calibv2_adrc_calib->DrcTuningPara.HiLight.EnvLv_len));

        //malloc local
        pAdrcCtx->Config.Drc_v20.Local.len = calibv2_adrc_calib->DrcTuningPara.LocalTMOSetting.LocalTMOData.EnvLv_len;
        pAdrcCtx->Config.Drc_v20.Local.EnvLv = (float*)malloc(sizeof(float) * (calibv2_adrc_calib->DrcTuningPara.LocalTMOSetting.LocalTMOData.EnvLv_len));
        pAdrcCtx->Config.Drc_v20.Local.LocalWeit = (float*)malloc(sizeof(float) * (calibv2_adrc_calib->DrcTuningPara.LocalTMOSetting.LocalTMOData.EnvLv_len));
        pAdrcCtx->Config.Drc_v20.Local.GlobalContrast = (float*)malloc(sizeof(float) * (calibv2_adrc_calib->DrcTuningPara.LocalTMOSetting.LocalTMOData.EnvLv_len));
        pAdrcCtx->Config.Drc_v20.Local.LoLitContrast = (float*)malloc(sizeof(float) * (calibv2_adrc_calib->DrcTuningPara.LocalTMOSetting.LocalTMOData.EnvLv_len));

        pAdrcCtx->drcAttr.Enable = false;
        pAdrcCtx->drcAttr.opMode = DRC_OPMODE_API_OFF;
        Config(pAdrcCtx); //set default para
        memcpy(&pAdrcCtx->pCalibDB.Drc_v20, calibv2_adrc_calib, sizeof(CalibDbV2_drc_t));//load iq paras

    }
    else if(CHECK_ISP_HW_V30()) {
        CalibDbV2_drc_V2_t* calibv2_adrc_calib =
            (CalibDbV2_drc_V2_t*)(CALIBDBV2_GET_MODULE_PTR(pCalibDb, adrc_calib));

        // pre-initialize context
        memset(pAdrcCtx, 0x00, sizeof(*pAdrcCtx));

        //malloc DrcGain
        pAdrcCtx->Config.Drc_v21.DrcGain.len = calibv2_adrc_calib->DrcTuningPara.DrcGain.EnvLv_len;
        pAdrcCtx->Config.Drc_v21.DrcGain.EnvLv = (float*)malloc(sizeof(float) * (calibv2_adrc_calib->DrcTuningPara.DrcGain.EnvLv_len));
        pAdrcCtx->Config.Drc_v21.DrcGain.DrcGain = (float*)malloc(sizeof(float) * (calibv2_adrc_calib->DrcTuningPara.DrcGain.EnvLv_len));
        pAdrcCtx->Config.Drc_v21.DrcGain.Alpha = (float*)malloc(sizeof(float) * (calibv2_adrc_calib->DrcTuningPara.DrcGain.EnvLv_len));
        pAdrcCtx->Config.Drc_v21.DrcGain.Clip = (float*)malloc(sizeof(float) * (calibv2_adrc_calib->DrcTuningPara.DrcGain.EnvLv_len));

        //malloc HiLit
        pAdrcCtx->Config.Drc_v21.HiLit.len = calibv2_adrc_calib->DrcTuningPara.HiLight.EnvLv_len;
        pAdrcCtx->Config.Drc_v21.HiLit.EnvLv = (float*)malloc(sizeof(float) * (calibv2_adrc_calib->DrcTuningPara.HiLight.EnvLv_len));
        pAdrcCtx->Config.Drc_v21.HiLit.Strength = (float*)malloc(sizeof(float) * (calibv2_adrc_calib->DrcTuningPara.HiLight.EnvLv_len));

        //malloc local
        pAdrcCtx->Config.Drc_v21.Local.len = calibv2_adrc_calib->DrcTuningPara.LocalSetting.LocalData.EnvLv_len;
        pAdrcCtx->Config.Drc_v21.Local.EnvLv = (float*)malloc(sizeof(float) * (calibv2_adrc_calib->DrcTuningPara.LocalSetting.LocalData.EnvLv_len));
        pAdrcCtx->Config.Drc_v21.Local.LocalAutoEnable = (int*)malloc(sizeof(float) * (calibv2_adrc_calib->DrcTuningPara.LocalSetting.LocalData.EnvLv_len));
        pAdrcCtx->Config.Drc_v21.Local.LocalAutoWeit = (float*)malloc(sizeof(float) * (calibv2_adrc_calib->DrcTuningPara.LocalSetting.LocalData.EnvLv_len));
        pAdrcCtx->Config.Drc_v21.Local.LocalWeit = (float*)malloc(sizeof(float) * (calibv2_adrc_calib->DrcTuningPara.LocalSetting.LocalData.EnvLv_len));
        pAdrcCtx->Config.Drc_v21.Local.GlobalContrast = (float*)malloc(sizeof(float) * (calibv2_adrc_calib->DrcTuningPara.LocalSetting.LocalData.EnvLv_len));
        pAdrcCtx->Config.Drc_v21.Local.LoLitContrast = (float*)malloc(sizeof(float) * (calibv2_adrc_calib->DrcTuningPara.LocalSetting.LocalData.EnvLv_len));

        pAdrcCtx->drcAttr.Enable = false;
        pAdrcCtx->drcAttr.opMode = DRC_OPMODE_API_OFF;
        ConfigV30(pAdrcCtx); //set default para
        memcpy(&pAdrcCtx->pCalibDB.Drc_v21, calibv2_adrc_calib, sizeof(CalibDbV2_drc_V2_t));//load iq paras
    }

    LOG1_ATMO( "%s:exit!\n", __FUNCTION__);

    return (XCAM_RETURN_NO_ERROR);
}
/******************************************************************************
 * AhdrRelease()
 *****************************************************************************/
XCamReturn AdrcRelease
(
    AdrcContext_t* pAdrcCtx
) {

    LOG1_ATMO( "%s:enter!\n", __FUNCTION__);
    XCamReturn result = XCAM_RETURN_NO_ERROR;

    // initial checks
    if (NULL == pAdrcCtx) {
        return (XCAM_RETURN_ERROR_PARAM);
    }

    result = AdrcStop(pAdrcCtx);
    if (result != XCAM_RETURN_NO_ERROR) {
        LOGE_ATMO( "%s: AHDRStop() failed!\n", __FUNCTION__);
        return (result);
    }

    // check state
    if ((ADRC_STATE_RUNNING == pAdrcCtx->state)
            || (ADRC_STATE_LOCKED == pAdrcCtx->state)) {
        return (XCAM_RETURN_ERROR_MEM);
    }

    if(CHECK_ISP_HW_V21()) {
        free(pAdrcCtx->Config.Drc_v20.DrcGain.EnvLv);
        free(pAdrcCtx->Config.Drc_v20.DrcGain.DrcGain);
        free(pAdrcCtx->Config.Drc_v20.DrcGain.Alpha);
        free(pAdrcCtx->Config.Drc_v20.DrcGain.Clip);
        free(pAdrcCtx->Config.Drc_v20.HiLit.EnvLv);
        free(pAdrcCtx->Config.Drc_v20.HiLit.Strength);
        free(pAdrcCtx->Config.Drc_v20.Local.EnvLv);
        free(pAdrcCtx->Config.Drc_v20.Local.LocalWeit);
        free(pAdrcCtx->Config.Drc_v20.Local.GlobalContrast);
        free(pAdrcCtx->Config.Drc_v20.Local.LoLitContrast);
    }
    else if(CHECK_ISP_HW_V30()) {
        free(pAdrcCtx->Config.Drc_v21.DrcGain.EnvLv);
        free(pAdrcCtx->Config.Drc_v21.DrcGain.DrcGain);
        free(pAdrcCtx->Config.Drc_v21.DrcGain.Alpha);
        free(pAdrcCtx->Config.Drc_v21.DrcGain.Clip);
        free(pAdrcCtx->Config.Drc_v21.HiLit.EnvLv);
        free(pAdrcCtx->Config.Drc_v21.HiLit.Strength);
        free(pAdrcCtx->Config.Drc_v21.Local.EnvLv);
        free(pAdrcCtx->Config.Drc_v21.Local.LocalAutoEnable);
        free(pAdrcCtx->Config.Drc_v21.Local.LocalAutoWeit);
        free(pAdrcCtx->Config.Drc_v21.Local.LocalWeit);
        free(pAdrcCtx->Config.Drc_v21.Local.GlobalContrast);
        free(pAdrcCtx->Config.Drc_v21.Local.LoLitContrast);
    }
    memset(pAdrcCtx, 0, sizeof(AdrcContext_t));
    free(pAdrcCtx);
    pAdrcCtx = NULL;

    LOG1_ATMO( "%s:exit!\n", __FUNCTION__);

    return (XCAM_RETURN_NO_ERROR);
}
