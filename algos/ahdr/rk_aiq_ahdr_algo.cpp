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
 * @file ahdr.cpp
 *
 * @brief
 *   ADD_DESCRIPTION_HERE
 *
 *****************************************************************************/
#include "math.h"
#include "rk_aiq_types_ahdr_algo_int.h"
#include "rk_aiq_ahdr_algo_merge.h"
#include "rk_aiq_ahdr_algo_tmo.h"
#include "xcam_log.h"

/******************************************************************************
 * AhdrStart()
 *****************************************************************************/
RESULT AhdrStart
(
    AhdrHandle_t pAhdrCtx
) {

    LOG1_AHDR( "%s:enter!\n", __FUNCTION__);

    // initial checks
    if (pAhdrCtx == NULL) {
        return (AHDR_RET_WRONG_HANDLE);
    }

    if ((AHDR_STATE_RUNNING == pAhdrCtx->state)
            || (AHDR_STATE_LOCKED == pAhdrCtx->state)) {
        return (AHDR_RET_WRONG_STATE);
    }

    pAhdrCtx->state = AHDR_STATE_RUNNING;

    LOG1_AHDR( "%s:exit!\n", __FUNCTION__);
    return (AHDR_RET_SUCCESS);
}
/******************************************************************************
 * AhdrStop()
 *****************************************************************************/
RESULT AhdrStop
(
    AhdrHandle_t pAhdrCtx
) {

    LOG1_AHDR( "%s:enter!\n", __FUNCTION__);

    // initial checks
    if (pAhdrCtx == NULL) {
        return (AHDR_RET_WRONG_HANDLE);
    }

    // before stopping, unlock the AHDR if locked
    if (AHDR_STATE_LOCKED == pAhdrCtx->state) {
        return (AHDR_RET_WRONG_STATE);
    }

    pAhdrCtx->state = AHDR_STATE_STOPPED;

    LOG1_AHDR( "%s:exit!\n", __FUNCTION__);

    return (AHDR_RET_SUCCESS);
}
/******************************************************************************
 * GetCurrPara()
 *****************************************************************************/
float GetCurrPara
(
    float           inPara,
    float         inMatrixX[6],
    float         inMatrixY[6]
) {
    LOG1_AHDR( "%s:enter!\n", __FUNCTION__);
    float x1 = 0.0f;
    float x2 = 0.0f;
    float value1 = 0.0f;
    float value2 = 0.0f;
    float outPara = 0.0f;

    if(inPara < inMatrixX[0])
        outPara = inMatrixY[0];
    else if (inPara >= inMatrixX[5])
        outPara = inMatrixY[5];
    else if (inPara >= inMatrixX[0] && inPara < inMatrixX[1])
    {
        x1 = inMatrixX[0];
        x2 = inMatrixX[1];
        value1 = inMatrixY[0];
        value2 = inMatrixY[1];
        outPara = value1 + (inPara - x1) * (value1 - value2) / (x1 - x2);
    }
    else if (inPara >= inMatrixX[1] && inPara < inMatrixX[2])
    {
        x1 = inMatrixX[1];
        x2 = inMatrixX[2];
        value1 = inMatrixY[1];
        value2 = inMatrixY[2];
        outPara = value1 + (inPara - x1) * (value1 - value2) / (x1 - x2);
    }
    else if (inPara >= inMatrixX[2] && inPara < inMatrixX[3])
    {
        x1 = inMatrixX[2];
        x2 = inMatrixX[3];
        value1 = inMatrixY[2];
        value2 = inMatrixY[3];
        outPara = value1 + (inPara - x1) * (value1 - value2) / (x1 - x2);
    }
    else if (inPara >= inMatrixX[3] && inPara < inMatrixX[4])
    {
        x1 = inMatrixX[3];
        x2 = inMatrixX[4];
        value1 = inMatrixY[3];
        value2 = inMatrixY[4];
        outPara = value1 + (inPara - x1) * (value1 - value2) / (x1 - x2);
    }
    else if (inPara >= inMatrixX[4] && inPara < inMatrixX[5])
    {
        x1 = inMatrixX[4];
        x2 = inMatrixX[5];
        value1 = inMatrixY[4];
        value2 = inMatrixY[5];
        outPara = value1 + (inPara - x1) * (value1 - value2) / (x1 - x2);
    }
    else
        LOGE_AHDR( "%s:  wrong para!!!\n", __FUNCTION__);

    return outPara;
    LOG1_AHDR( "%s:exit!\n", __FUNCTION__);
}
/******************************************************************************
 * AhdrApiOffConfig()
 *set default AhdrConfig data
 *****************************************************************************/
void AhdrApiOffConfig
(
    AhdrHandle_t           pAhdrCtx
) {
    LOG1_AHDR( "%s:enter!\n", __FUNCTION__);

    //merge
    pAhdrCtx->AhdrConfig.merge_para.MergeMode = 1;
    pAhdrCtx->AhdrConfig.merge_para.EnvLv[0] = 0;
    pAhdrCtx->AhdrConfig.merge_para.EnvLv[1] = 0.2 ;
    pAhdrCtx->AhdrConfig.merge_para.EnvLv[2] = 0.3 ;
    pAhdrCtx->AhdrConfig.merge_para.EnvLv[3] = 0.4 ;
    pAhdrCtx->AhdrConfig.merge_para.EnvLv[4] = 0.9 ;
    pAhdrCtx->AhdrConfig.merge_para.EnvLv[5] = 1.0 ;
    pAhdrCtx->AhdrConfig.merge_para.MoveCoef[0] = 0;
    pAhdrCtx->AhdrConfig.merge_para.MoveCoef[1] = 0.1;
    pAhdrCtx->AhdrConfig.merge_para.MoveCoef[2] = 0.2;
    pAhdrCtx->AhdrConfig.merge_para.MoveCoef[3] = 0.3;
    pAhdrCtx->AhdrConfig.merge_para.MoveCoef[4] = 0.4;
    pAhdrCtx->AhdrConfig.merge_para.MoveCoef[5] = 1;
    for(int i = 0; i < 6; ++i)
    {
        pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[i] = 80 ;
        pAhdrCtx->AhdrConfig.merge_para.OECurve_offset[i] = 205 ;
        pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[i] = 80 ;
        pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[i] = 38 ;
        pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[i] = 80 ;
        pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[i] = 38 ;
    }
    pAhdrCtx->AhdrConfig.merge_para.OECurve_damp = 0.7;
    pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_damp = 0.7;
    pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_damp = 0.7;

    //TMO
    pAhdrCtx->AhdrConfig.tmo_para.damp = 0.7 ;
    pAhdrCtx->AhdrConfig.tmo_para.Luma.EnvLv[0] = 0 ;
    pAhdrCtx->AhdrConfig.tmo_para.Luma.EnvLv[1] = 0.1 ;
    pAhdrCtx->AhdrConfig.tmo_para.Luma.EnvLv[2] = 0.2 ;
    pAhdrCtx->AhdrConfig.tmo_para.Luma.EnvLv[3] = 0.3 ;
    pAhdrCtx->AhdrConfig.tmo_para.Luma.EnvLv[4] = 0.9 ;
    pAhdrCtx->AhdrConfig.tmo_para.Luma.EnvLv[5] = 1.0 ;
    pAhdrCtx->AhdrConfig.tmo_para.Luma.Tolerance = 0 ;
    pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.DetailsHighLightMode = 0;
    pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.OEPdf[0] = 0;
    pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.OEPdf[1] = 0.1;
    pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.OEPdf[2] = 0.2;
    pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.OEPdf[3] = 0.3;
    pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.OEPdf[4] = 0.4;
    pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.OEPdf[5] = 0.5;
    pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.EnvLv[0] = 0;
    pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.EnvLv[1] = 0.1;
    pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.EnvLv[2] = 0.2;
    pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.EnvLv[3] = 0.3;
    pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.EnvLv[4] = 0.4;
    pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.EnvLv[5] = 1;
    pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.Tolerance = 0;
    pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DetailsLowLightMode = 0;
    pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.Tolerance = 0;
    pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.FocusLuma[0] = 0;
    pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.FocusLuma[1] = 0.1;
    pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.FocusLuma[2] = 0.2;
    pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.FocusLuma[3] = 0.3;
    pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.FocusLuma[4] = 0.4;
    pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.FocusLuma[5] = 0.1;
    pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DarkPdf[0] = 0;
    pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DarkPdf[1] = 0.1;
    pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DarkPdf[2] = 0.2;
    pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DarkPdf[3] = 0.3;
    pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DarkPdf[4] = 0.4;
    pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DarkPdf[5] = 0.5;
    pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.ISO[0] = 50;
    pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.ISO[1] = 50;
    pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.ISO[2] = 50;
    pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.ISO[3] = 50;
    pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.ISO[4] = 50;
    pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.ISO[5] = 50;
    pAhdrCtx->AhdrConfig.tmo_para.local.localtmoMode = 0;
    pAhdrCtx->AhdrConfig.tmo_para.local.Tolerance = 0;
    pAhdrCtx->AhdrConfig.tmo_para.local.DynamicRange[0] = 1;
    pAhdrCtx->AhdrConfig.tmo_para.local.DynamicRange[1] = 1;
    pAhdrCtx->AhdrConfig.tmo_para.local.DynamicRange[2] = 1;
    pAhdrCtx->AhdrConfig.tmo_para.local.DynamicRange[3] = 1;
    pAhdrCtx->AhdrConfig.tmo_para.local.DynamicRange[4] = 1;
    pAhdrCtx->AhdrConfig.tmo_para.local.DynamicRange[5] = 1;
    pAhdrCtx->AhdrConfig.tmo_para.local.EnvLv[0] = 0;
    pAhdrCtx->AhdrConfig.tmo_para.local.EnvLv[1] = 0.2;
    pAhdrCtx->AhdrConfig.tmo_para.local.EnvLv[2] = 0.4;
    pAhdrCtx->AhdrConfig.tmo_para.local.EnvLv[3] = 0.6;
    pAhdrCtx->AhdrConfig.tmo_para.local.EnvLv[4] = 0.8;
    pAhdrCtx->AhdrConfig.tmo_para.local.EnvLv[5] = 1;
    for(int i = 0; i < 6; ++i)
    {
        pAhdrCtx->AhdrConfig.tmo_para.Luma.GlobeLuma[i] = 0.25 ;
        pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.DetailsHighLight[i] = 0.5;
        pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DetailsLowLight[i] = 1;
        pAhdrCtx->AhdrConfig.tmo_para.local.LocalTmoStrength[i] = 0.5;
    }

    pAhdrCtx->AhdrConfig.tmo_para.global.iir = 64;

    LOGI_AHDR( "%s:exit!\n", __FUNCTION__);
}

/******************************************************************************
 * AhdrConfig()
 *set default AhdrConfig data
 *****************************************************************************/
void AhdrConfig
(
    AhdrHandle_t           pAhdrCtx
) {
    LOGI_AHDR( "%s:enter!\n", __FUNCTION__);

    // initial checks
    DCT_ASSERT(pAhdrCtx != NULL);

    if(pAhdrCtx->hdrAttr.bEnable == false)
    {
        AhdrApiOffConfig(pAhdrCtx);
        LOGD_AHDR("%s: Ahdr Api is OFF!!!:\n", __FUNCTION__);
    }
    else

        LOGD_AHDR("%s: Ahdr Api is ON!!!:\n", __FUNCTION__);

    //config default AhdrPrevData data
    pAhdrCtx->AhdrPrevData.ro_hdrtmo_lgmean = 20000;
    pAhdrCtx->AhdrPrevData.PrevMergeHandleData.OECurve_smooth = 80;
    pAhdrCtx->AhdrPrevData.PrevMergeHandleData.OECurve_offset = 210;
    pAhdrCtx->AhdrPrevData.PrevMergeHandleData.MDCurveLM_smooth = 80;
    pAhdrCtx->AhdrPrevData.PrevMergeHandleData.MDCurveLM_offset = 38;
    pAhdrCtx->AhdrPrevData.PrevMergeHandleData.MDCurveMS_smooth = 80;
    pAhdrCtx->AhdrPrevData.PrevMergeHandleData.MDCurveMS_offset = 38;
    pAhdrCtx->AhdrPrevData.PrevTmoHandleData.GlobeLuma = 0.18;
    pAhdrCtx->AhdrPrevData.PrevTmoHandleData.GlobeMaxLuma = 0.3;
    pAhdrCtx->AhdrPrevData.PrevTmoHandleData.DetailsHighLight = 0.5;
    pAhdrCtx->AhdrPrevData.PrevTmoHandleData.DetailsLowLight = 1;
    pAhdrCtx->AhdrPrevData.PrevTmoHandleData.LocalTmoStrength = 0.3;
    pAhdrCtx->AhdrPrevData.PrevTmoHandleData.GlobalTmoStrength = 0.5;

    LOG1_AHDR( "%s:exit!\n", __FUNCTION__);
}
void AhdrGetStats
(
    AhdrHandle_t           pAhdrCtx,
    rkisp_ahdr_stats_t*         ROData
) {
    LOG1_AHDR( "%s:enter!\n", __FUNCTION__);

    pAhdrCtx->CurrStatsData.tmo_stats.ro_hdrtmo_lglow = ROData->tmo_stats.ro_hdrtmo_lglow;
    pAhdrCtx->CurrStatsData.tmo_stats.ro_hdrtmo_lgmin = ROData->tmo_stats.ro_hdrtmo_lgmin;
    pAhdrCtx->CurrStatsData.tmo_stats.ro_hdrtmo_lgmax = ROData->tmo_stats.ro_hdrtmo_lgmax;
    pAhdrCtx->CurrStatsData.tmo_stats.ro_hdrtmo_lghigh = ROData->tmo_stats.ro_hdrtmo_lghigh;
    pAhdrCtx->CurrStatsData.tmo_stats.ro_hdrtmo_lgmean = ROData->tmo_stats.ro_hdrtmo_lgmean;
    pAhdrCtx->CurrStatsData.tmo_stats.ro_hdrtmo_weightkey = ROData->tmo_stats.ro_hdrtmo_weightkey;
    pAhdrCtx->CurrStatsData.tmo_stats.ro_hdrtmo_lgrange0 = ROData->tmo_stats.ro_hdrtmo_lgrange0;
    pAhdrCtx->CurrStatsData.tmo_stats.ro_hdrtmo_lgrange1 = ROData->tmo_stats.ro_hdrtmo_lgrange1;
    pAhdrCtx->CurrStatsData.tmo_stats.ro_hdrtmo_lgavgmax = ROData->tmo_stats.ro_hdrtmo_lgavgmax;
    pAhdrCtx->CurrStatsData.tmo_stats.ro_hdrtmo_palpha = ROData->tmo_stats.ro_hdrtmo_palpha;
    pAhdrCtx->CurrStatsData.tmo_stats.ro_hdrtmo_linecnt = ROData->tmo_stats.ro_hdrtmo_linecnt;
    for(int i = 0; i < 32; i++)
        pAhdrCtx->CurrStatsData.tmo_stats.ro_array_min_max[i] = ROData->tmo_stats.ro_array_min_max[i];

    //get other stats from stats
    for(int i = 0; i < 225; i++)
    {
        pAhdrCtx->CurrStatsData.other_stats.short_luma[i] = ROData->other_stats.short_luma[i];
        pAhdrCtx->CurrStatsData.other_stats.long_luma[i] = ROData->other_stats.long_luma[i];
        pAhdrCtx->CurrStatsData.other_stats.tmo_luma[i] = ROData->other_stats.tmo_luma[i];
    }

    if(pAhdrCtx->hdr_mode == 3)
    {
        for(int i = 0; i < 25; i++)
            pAhdrCtx->CurrStatsData.other_stats.middle_luma[i] = ROData->other_stats.middle_luma[i];
    }

    LOGV_AHDR("%s:  Ahdr RO data from register:\n", __FUNCTION__);
    LOGV_AHDR("%s:  ro_hdrtmo_lglow:%d:\n", __FUNCTION__, pAhdrCtx->CurrStatsData.tmo_stats.ro_hdrtmo_lglow);
    LOGV_AHDR("%s:  ro_hdrtmo_lgmin:%d:\n", __FUNCTION__, pAhdrCtx->CurrStatsData.tmo_stats.ro_hdrtmo_lgmin);
    LOGV_AHDR("%s:  ro_hdrtmo_lgmax:%d:\n", __FUNCTION__, pAhdrCtx->CurrStatsData.tmo_stats.ro_hdrtmo_lgmax);
    LOGV_AHDR("%s:  ro_hdrtmo_lghigh:%d:\n", __FUNCTION__, pAhdrCtx->CurrStatsData.tmo_stats.ro_hdrtmo_lghigh);
    LOGV_AHDR("%s:  ro_hdrtmo_weightkey:%d:\n", __FUNCTION__, pAhdrCtx->CurrStatsData.tmo_stats.ro_hdrtmo_weightkey);
    LOGV_AHDR("%s:  ro_hdrtmo_lgmean:%d:\n", __FUNCTION__, pAhdrCtx->CurrStatsData.tmo_stats.ro_hdrtmo_lgmean);
    LOGV_AHDR("%s:  ro_hdrtmo_lgrange0:%d:\n", __FUNCTION__, pAhdrCtx->CurrStatsData.tmo_stats.ro_hdrtmo_lgrange0);
    LOGV_AHDR("%s:  ro_hdrtmo_lgrange1:%d:\n", __FUNCTION__, pAhdrCtx->CurrStatsData.tmo_stats.ro_hdrtmo_lgrange1);
    LOGV_AHDR("%s:  ro_hdrtmo_lgavgmax:%d:\n", __FUNCTION__, pAhdrCtx->CurrStatsData.tmo_stats.ro_hdrtmo_lgavgmax);
    LOGV_AHDR("%s:  ro_hdrtmo_palpha:%d:\n", __FUNCTION__, pAhdrCtx->CurrStatsData.tmo_stats.ro_hdrtmo_palpha);
    LOGV_AHDR("%s:  ro_hdrtmo_linecnt:%d:\n", __FUNCTION__, pAhdrCtx->CurrStatsData.tmo_stats.ro_hdrtmo_linecnt);
    for(int i = 0; i < 32; i++)
        LOGV_AHDR("%s:  ro_array_min_max[%d]:%d:\n", __FUNCTION__, i, pAhdrCtx->CurrStatsData.tmo_stats.ro_array_min_max[i]);

    LOG1_AHDR( "%s:exit!\n", __FUNCTION__);
}
void AhdrGetAeResult
(
    AhdrHandle_t           pAhdrCtx,
    AecPreResult_t  AecHdrPreResult
) {
    LOG1_AHDR( "%s:enter!\n", __FUNCTION__);

    //get Ae Pre Result
    pAhdrCtx->CurrAeResult.GlobalEnvLv = AecHdrPreResult.GlobalEnvLv[AecHdrPreResult.NormalIndex];
    pAhdrCtx->CurrAeResult.M2S_Ratio = AecHdrPreResult.M2S_ExpRatio;
    pAhdrCtx->CurrAeResult.M2S_Ratio = pAhdrCtx->CurrAeResult.M2S_Ratio < 1 ? 1 : pAhdrCtx->CurrAeResult.M2S_Ratio;
    pAhdrCtx->CurrAeResult.L2M_Ratio = AecHdrPreResult.L2M_ExpRatio;
    pAhdrCtx->CurrAeResult.L2M_Ratio = pAhdrCtx->CurrAeResult.L2M_Ratio < 1 ? 1 : pAhdrCtx->CurrAeResult.L2M_Ratio;
    pAhdrCtx->CurrAeResult.DynamicRange = AecHdrPreResult.DynamicRange;
    pAhdrCtx->CurrAeResult.OEPdf = AecHdrPreResult.OverExpROIPdf[1];
    pAhdrCtx->CurrAeResult.DarkPdf = AecHdrPreResult.LowLightROIPdf[1];
    for(int i = 0; i < 225; i++)
    {
        pAhdrCtx->CurrAeResult.BlockLumaS[i] = pAhdrCtx->CurrStatsData.other_stats.short_luma[i];
        pAhdrCtx->CurrAeResult.BlockLumaL[i] = pAhdrCtx->CurrStatsData.other_stats.long_luma[i];
    }
    if(pAhdrCtx->hdr_mode == 3)
        for(int i = 0; i < 25; i++)
            pAhdrCtx->CurrAeResult.BlockLumaM[i] = pAhdrCtx->CurrStatsData.other_stats.middle_luma[i];
    else
        for(int i = 0; i < 25; i++)
            pAhdrCtx->CurrAeResult.BlockLumaM[i] = 0;

    //transfer CurrAeResult data into AhdrHandle
    switch (pAhdrCtx->CurrHandleData.MergeMode)
    {
    case 0:
        pAhdrCtx->CurrHandleData.CurrLExpo = AecHdrPreResult.LinearExp.exp_real_params.analog_gain * AecHdrPreResult.LinearExp.exp_real_params.integration_time;
        pAhdrCtx->CurrHandleData.CurrL2S_Ratio = 1;
        pAhdrCtx->CurrAeResult.ISO = AecHdrPreResult.LinearExp.exp_real_params.analog_gain * 50.0;
        pAhdrCtx->CurrAeResult.GlobalEnvLv = AecHdrPreResult.GlobalEnvLv[0];
        pAhdrCtx->CurrAeResult.OEPdf = AecHdrPreResult.OverExpROIPdf[0];
        pAhdrCtx->CurrAeResult.DarkPdf = AecHdrPreResult.LowLightROIPdf[0];
        break;
    case 1:
        pAhdrCtx->CurrAeResult.L2M_Ratio = 1;
        pAhdrCtx->CurrHandleData.CurrL2S_Ratio = pAhdrCtx->CurrAeResult.M2S_Ratio;
        pAhdrCtx->CurrHandleData.CurrL2M_Ratio = pAhdrCtx->CurrAeResult.L2M_Ratio;
        pAhdrCtx->CurrHandleData.CurrL2L_Ratio = 1;
        pAhdrCtx->CurrHandleData.CurrLExpo = AecHdrPreResult.HdrExp[1].exp_real_params.analog_gain * AecHdrPreResult.HdrExp[1].exp_real_params.integration_time;
        pAhdrCtx->CurrAeResult.ISO = AecHdrPreResult.HdrExp[1].exp_real_params.analog_gain * 50.0;
        pAhdrCtx->CurrAeResult.GlobalEnvLv = AecHdrPreResult.GlobalEnvLv[1];
        break;
    case 2:
        pAhdrCtx->CurrHandleData.CurrL2S_Ratio = pAhdrCtx->CurrAeResult.L2M_Ratio * pAhdrCtx->CurrAeResult.M2S_Ratio;
        pAhdrCtx->CurrHandleData.CurrL2M_Ratio = pAhdrCtx->CurrAeResult.L2M_Ratio;
        pAhdrCtx->CurrHandleData.CurrL2L_Ratio = 1;
        pAhdrCtx->CurrHandleData.CurrLExpo = AecHdrPreResult.HdrExp[2].exp_real_params.analog_gain * AecHdrPreResult.HdrExp[2].exp_real_params.integration_time;
        pAhdrCtx->CurrAeResult.ISO = AecHdrPreResult.HdrExp[2].exp_real_params.analog_gain * 50.0;
        pAhdrCtx->CurrAeResult.GlobalEnvLv = AecHdrPreResult.GlobalEnvLv[2];
        break;
    default:
        LOGE_AHDR("%s:  Wrong frame number in HDR mode!!!\n", __FUNCTION__);
        break;
    }

    //Normalize the current envLv for AEC
    float maxEnvLuma = 65 / 10;
    float minEnvLuma = 0;
    pAhdrCtx->CurrHandleData.CurrEnvLv = (pAhdrCtx->CurrAeResult.GlobalEnvLv  - minEnvLuma) / (maxEnvLuma - minEnvLuma);
    pAhdrCtx->CurrHandleData.CurrEnvLv = LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrEnvLv, 1, 0);

    LOGD_AHDR("%s:  Current CurrL2S_Ratio:%f CurrL2M_Ratio:%f CurrL2L_Ratio:%f\n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrL2S_Ratio,
              pAhdrCtx->CurrHandleData.CurrL2M_Ratio, pAhdrCtx->CurrHandleData.CurrL2L_Ratio);

    LOG1_AHDR( "%s:exit!\n", __FUNCTION__);
}

void AhdrGetCurrMergeData
(
    AhdrHandle_t           pAhdrCtx
) {
    LOG1_AHDR( "%s:enter!\n", __FUNCTION__);

    //get Current merge OECurve
    pAhdrCtx->CurrHandleData.CurrMergeHandleData.OECurve_smooth = GetCurrPara(pAhdrCtx->CurrHandleData.CurrEnvLv,
            pAhdrCtx->AhdrConfig.merge_para.EnvLv,
            pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth);
    pAhdrCtx->CurrHandleData.CurrMergeHandleData.OECurve_offset = GetCurrPara(pAhdrCtx->CurrHandleData.CurrEnvLv,
            pAhdrCtx->AhdrConfig.merge_para.EnvLv,
            pAhdrCtx->AhdrConfig.merge_para.OECurve_offset);

    //get Current merge MDCurve
    pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveLM_smooth = GetCurrPara(pAhdrCtx->CurrHandleData.CurrMoveCoef,
            pAhdrCtx->AhdrConfig.merge_para.MoveCoef,
            pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth);
    pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveLM_offset = GetCurrPara(pAhdrCtx->CurrHandleData.CurrMoveCoef,
            pAhdrCtx->AhdrConfig.merge_para.MoveCoef,
            pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset);
    pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveMS_smooth = GetCurrPara(pAhdrCtx->CurrHandleData.CurrMoveCoef,
            pAhdrCtx->AhdrConfig.merge_para.MoveCoef,
            pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth);
    pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveMS_offset = GetCurrPara(pAhdrCtx->CurrHandleData.CurrMoveCoef,
            pAhdrCtx->AhdrConfig.merge_para.MoveCoef,
            pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset);

    pAhdrCtx->CurrHandleData.MergeOEDamp = pAhdrCtx->AhdrConfig.merge_para.OECurve_damp;
    pAhdrCtx->CurrHandleData.MergeMDDampLM = pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_damp;
    pAhdrCtx->CurrHandleData.MergeMDDampMS = pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_damp;

    LOG1_AHDR( "%s:exit!\n", __FUNCTION__);
}

void AhdrGetCurrTmoData
(
    AhdrHandle_t           pAhdrCtx
) {
    LOG1_AHDR( "%s:enter!\n", __FUNCTION__);

    //get Current tmo GlobeLuma GlobeMaxLuma
    int GlobalLuma_mode = (int)pAhdrCtx->AhdrConfig.tmo_para.Luma.globalLumaMode;
    if(GlobalLuma_mode == 0)
        pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeLuma = GetCurrPara(pAhdrCtx->CurrHandleData.CurrEnvLv,
                pAhdrCtx->AhdrConfig.tmo_para.Luma.EnvLv,
                pAhdrCtx->AhdrConfig.tmo_para.Luma.GlobeLuma);
    else if(GlobalLuma_mode == 1)
        pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeLuma = GetCurrPara(pAhdrCtx->CurrHandleData.CurrISO,
                pAhdrCtx->AhdrConfig.tmo_para.Luma.ISO,
                pAhdrCtx->AhdrConfig.tmo_para.Luma.GlobeLuma);

    float GlobeLuma = pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeLuma;
    pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeMaxLuma = MAXLUMAK * GlobeLuma + MAXLUMAB;
    pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeMaxLuma = LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeMaxLuma, GLOBEMAXLUMAMAX, GLOBEMAXLUMAMIN);

    //get Current tmo strength
    int LocalMode = (int)pAhdrCtx->AhdrConfig.tmo_para.local.localtmoMode;
    if(LocalMode == 0)
        pAhdrCtx->CurrHandleData.CurrTmoHandleData.LocalTmoStrength = GetCurrPara(pAhdrCtx->CurrHandleData.CurrDynamicRange,
                pAhdrCtx->AhdrConfig.tmo_para.local.DynamicRange,
                pAhdrCtx->AhdrConfig.tmo_para.local.LocalTmoStrength);
    else if(LocalMode == 1)
        pAhdrCtx->CurrHandleData.CurrTmoHandleData.LocalTmoStrength = GetCurrPara(pAhdrCtx->CurrHandleData.CurrEnvLv,
                pAhdrCtx->AhdrConfig.tmo_para.local.EnvLv,
                pAhdrCtx->AhdrConfig.tmo_para.local.LocalTmoStrength);

    if(pAhdrCtx->AhdrConfig.tmo_para.global.isHdrGlobalTmo)
        pAhdrCtx->CurrHandleData.CurrTmoHandleData.LocalTmoStrength = 0;

    //get Current tmo BabdPrior
    int GlobalMode = (int)pAhdrCtx->AhdrConfig.tmo_para.global.mode;
    if(GlobalMode == 0)
        pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobalTmoStrength = GetCurrPara(pAhdrCtx->CurrHandleData.CurrDynamicRange,
                pAhdrCtx->AhdrConfig.tmo_para.global.DynamicRange,
                pAhdrCtx->AhdrConfig.tmo_para.global.GlobalTmoStrength);
    else if(GlobalMode == 1)
        pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobalTmoStrength = GetCurrPara(pAhdrCtx->CurrHandleData.CurrEnvLv,
                pAhdrCtx->AhdrConfig.tmo_para.global.EnvLv,
                pAhdrCtx->AhdrConfig.tmo_para.global.GlobalTmoStrength);

    //get Current tmo DetailsHighLight
    int DetailsHighLight_mode = (int)pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.DetailsHighLightMode;
    if(DetailsHighLight_mode == 0)
        pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsHighLight = GetCurrPara(pAhdrCtx->CurrHandleData.CurrOEPdf,
                pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.OEPdf,
                pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.DetailsHighLight);
    else if(DetailsHighLight_mode == 1)
        pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsHighLight = GetCurrPara(pAhdrCtx->CurrHandleData.CurrEnvLv,
                pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.EnvLv,
                pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.DetailsHighLight);

    //get Current tmo DetailsLowLight
    int DetailsLowLight_mode = (int)pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DetailsLowLightMode;
    if (DetailsLowLight_mode == 0)
    {
#if 0
        int focs = pAhdrCtx->CurrAfResult.CurrAfTargetPos;
        int focs_width = pAhdrCtx->CurrAfResult.CurrAfTargetWidth / (pAhdrCtx->width / 15);
        int focs_height = pAhdrCtx->CurrAfResult.CurrAfTargetHeight / (pAhdrCtx->height / 15);
        float focs_luma = 0;
        for(int i = 0; i < focs_height; i++)
            for(int j = 0; j < focs_width; j++)
                focs_luma += pAhdrCtx->CurrAeResult.BlockLumaL[focs + i + 15 * j];
        focs_luma = focs_luma / (focs_width * focs_height);
        pAhdrCtx->CurrHandleData.CurrTotalFocusLuma = focs_luma / 15;
        pAhdrCtx->CurrHandleData.CurrTotalFocusLuma = LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrTotalFocusLuma, FOCUSLUMAMAX, FOCUSLUMAMIN);
        pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsLowLight = GetCurrPara(pAhdrCtx->CurrHandleData.CurrTotalFocusLuma,
                pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.FocusLuma,
                pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DetailsLowLight);

#endif
        pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsLowLight = pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DetailsLowLight[0];

    }
    else if (DetailsLowLight_mode == 1)
        pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsLowLight = GetCurrPara(pAhdrCtx->CurrHandleData.CurrDarkPdf,
                pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DarkPdf,
                pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DetailsLowLight);
    else if (DetailsLowLight_mode == 2)
        pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsLowLight = GetCurrPara(pAhdrCtx->CurrHandleData.CurrISO,
                pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.ISO,
                pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DetailsLowLight);

    LOG1_AHDR( "%s:exit!\n", __FUNCTION__);
}

void AhdrGetSensorInfo
(
    AhdrHandle_t     pAhdrCtx,
    AecProcResult_t  AecHdrProcResult
) {
    LOG1_AHDR( "%s:enter!\n", __FUNCTION__);

    pAhdrCtx->SensorInfo.LongFrmMode = AecHdrProcResult.LongFrmMode;

    for(int i = 0; i < 3; i++)
    {
        pAhdrCtx->SensorInfo.HdrMinGain[i] = AecHdrProcResult.HdrMinGain[i];
        pAhdrCtx->SensorInfo.HdrMaxGain[i] = AecHdrProcResult.HdrMaxGain[i];
        pAhdrCtx->SensorInfo.HdrMinIntegrationTime[i] = AecHdrProcResult.HdrMinIntegrationTime[i];
        pAhdrCtx->SensorInfo.HdrMaxIntegrationTime[i] = AecHdrProcResult.HdrMaxIntegrationTime[i];
    }

    if(pAhdrCtx->hdr_mode == 2)
    {
        pAhdrCtx->SensorInfo.MaxExpoL = pAhdrCtx->SensorInfo.HdrMaxGain[1] * pAhdrCtx->SensorInfo.HdrMaxIntegrationTime[1];
        pAhdrCtx->SensorInfo.MinExpoL = pAhdrCtx->SensorInfo.HdrMinGain[1] * pAhdrCtx->SensorInfo.HdrMinIntegrationTime[1];
        pAhdrCtx->SensorInfo.MaxExpoM = 0;
        pAhdrCtx->SensorInfo.MinExpoM = 0;

    }
    else if(pAhdrCtx->hdr_mode == 3)
    {
        pAhdrCtx->SensorInfo.MaxExpoL = pAhdrCtx->SensorInfo.HdrMaxGain[2] * pAhdrCtx->SensorInfo.HdrMaxIntegrationTime[2];
        pAhdrCtx->SensorInfo.MinExpoL = pAhdrCtx->SensorInfo.HdrMinGain[2] * pAhdrCtx->SensorInfo.HdrMinIntegrationTime[2];
        pAhdrCtx->SensorInfo.MaxExpoM = pAhdrCtx->SensorInfo.HdrMaxGain[1] * pAhdrCtx->SensorInfo.HdrMaxIntegrationTime[1];
        pAhdrCtx->SensorInfo.MinExpoM = pAhdrCtx->SensorInfo.HdrMinGain[1] * pAhdrCtx->SensorInfo.HdrMinIntegrationTime[1];
    }


    pAhdrCtx->SensorInfo.MaxExpoS = pAhdrCtx->SensorInfo.HdrMaxGain[0] * pAhdrCtx->SensorInfo.HdrMaxIntegrationTime[0];
    pAhdrCtx->SensorInfo.MinExpoS = pAhdrCtx->SensorInfo.HdrMinGain[0] * pAhdrCtx->SensorInfo.HdrMinIntegrationTime[0];


    LOG1_AHDR( "%s:exit!\n", __FUNCTION__);
}

/******************************************************************************
 * AhdrApiOffUpdate()
 *
 *****************************************************************************/
void AhdrApiFastMode
(
    AhdrHandle_t     pAhdrCtx
)
{
    LOG1_AHDR("%s:enter!\n", __FUNCTION__);

    //tmo data
    pAhdrCtx->hdrAttr.level = LIMIT_VALUE(pAhdrCtx->hdrAttr.level, FASTMODELEVELMAX, FASTMODELEVELMIN);

    float level = ((float)(pAhdrCtx->hdrAttr.level)) / FASTMODELEVELMAX;
    float level_default = 0.5;
    float level_diff = level - level_default;

    pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeLuma *= 1 + level_diff;
    float GlobeLuma = pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeLuma;
    pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeMaxLuma = MAXLUMAK * GlobeLuma + MAXLUMAB;
    pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeMaxLuma =
        LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeMaxLuma, GLOBEMAXLUMAMAX, GLOBEMAXLUMAMIN);

    pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsHighLight *= 1 + level_diff;
    pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsHighLight =
        LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsHighLight, DETAILSHIGHLIGHTMAX, DETAILSHIGHLIGHTMIN);

    pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsLowLight *= 1 + level_diff;
    pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsLowLight =
        LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsLowLight, DETAILSLOWLIGHTMAX, DETAILSLOWLIGHTMIN);

    /*
        pAhdrCtx->CurrHandleData.CurrTmoHandleData.TmoContrast *= 1 + level_diff;
        pAhdrCtx->CurrHandleData.CurrTmoHandleData.TmoContrast =
            LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrTmoHandleData.LocalTmoStrength, TMOCONTRASTMAX, TMOCONTRASTMIN);
    */
    //paras after updating
    LOGD_AHDR("%s:  Ahdr api off!! Current Handle data:\n", __FUNCTION__);
    LOGD_AHDR("%s:  API mode is HDR_OpMode_Fast, set level:%d\n", __FUNCTION__, pAhdrCtx->hdrAttr.level);
    LOGD_AHDR("%s:  After fast mode GlobeLuma:%f GlobeMaxLuma:%f DetailsHighLight:%f DetailsLowLight:%f LocalTmoStrength:%f \n", __FUNCTION__,
              pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeLuma, pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeMaxLuma,
              pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsHighLight, pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsLowLight,
              pAhdrCtx->CurrHandleData.CurrTmoHandleData.LocalTmoStrength);


    LOG1_AHDR( "%s:exit!\n", __FUNCTION__);
}

/******************************************************************************
 * AhdrApiOffUpdate()
 *
 *****************************************************************************/
void AhdrApiOffUpdate
(
    AhdrHandle_t     pAhdrCtx,
    AecPreResult_t  AecHdrPreResult,
    af_preprocess_result_t AfPreResult
)
{
    LOG1_AHDR("%s:enter!\n", __FUNCTION__);
    LOGD_AHDR("%s:  Ahdr api off!! Current Handle data:\n", __FUNCTION__);

    //get current merge data
    AhdrGetCurrMergeData(pAhdrCtx);

    //get cuurrent tmo data
    AhdrGetCurrTmoData(pAhdrCtx);

    //log after updating

    LOGD_AHDR("%s:	Current CurrEnvLv:%f OECurve_smooth:%f OECurve_offset:%f \n", __FUNCTION__,  pAhdrCtx->CurrHandleData.CurrEnvLv,
              pAhdrCtx->CurrHandleData.CurrMergeHandleData.OECurve_smooth, pAhdrCtx->CurrHandleData.CurrMergeHandleData.OECurve_offset);
    LOGD_AHDR("%s:	Current CurrMoveCoef:%f MDCurveLM_smooth:%f MDCurveLM_offset:%f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrMoveCoef,
              pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveLM_smooth, pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveLM_offset);
    LOGD_AHDR("%s:	Current CurrMoveCoef:%f MDCurveMS_smooth:%f MDCurveMS_offset:%f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrMoveCoef,
              pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveMS_smooth, pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveMS_offset);
    LOGD_AHDR("%s:  GlobalLumaMode:%f CurrEnvLv:%f CurrISO:%f GlobeLuma:%f GlobeMaxLuma:%f \n", __FUNCTION__,  pAhdrCtx->AhdrConfig.tmo_para.Luma.globalLumaMode,
              pAhdrCtx->CurrHandleData.CurrEnvLv, pAhdrCtx->CurrHandleData.CurrISO, pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeLuma, pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeMaxLuma);
    LOGD_AHDR("%s:  DetailsHighLightMode:%f CurrOEPdf:%f CurrEnvLv:%f DetailsHighLight:%f\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.DetailsHighLightMode, pAhdrCtx->CurrHandleData.CurrOEPdf
              , pAhdrCtx->CurrHandleData.CurrEnvLv, pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsHighLight);
    LOGD_AHDR("%s:  DetailsLowLightMode:%f CurrTotalFocusLuma:%f CurrDarkPdf:%f CurrISO:%f DetailsLowLight:%f\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DetailsLowLightMode,
              pAhdrCtx->CurrHandleData.CurrTotalFocusLuma, pAhdrCtx->CurrHandleData.CurrDarkPdf, pAhdrCtx->CurrHandleData.CurrISO, pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsLowLight);
    LOGD_AHDR("%s:  localtmoMode:%f CurrDynamicRange:%f CurrEnvLv:%f LocalTmoStrength:%f\n", __FUNCTION__,  pAhdrCtx->AhdrConfig.tmo_para.local.localtmoMode, pAhdrCtx->CurrHandleData.CurrDynamicRange,
              pAhdrCtx->CurrHandleData.CurrEnvLv, pAhdrCtx->CurrHandleData.CurrTmoHandleData.LocalTmoStrength);
    LOGD_AHDR("%s:  GlobalTMO en:%d mode:%f CurrDynamicRange:%f CurrEnvLv:%f Strength:%f\n", __FUNCTION__,  pAhdrCtx->AhdrConfig.tmo_para.global.isHdrGlobalTmo, pAhdrCtx->AhdrConfig.tmo_para.global.mode, pAhdrCtx->CurrHandleData.CurrDynamicRange,
              pAhdrCtx->CurrHandleData.CurrEnvLv, pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobalTmoStrength);


    //clip hdr level by fast mode
    if(pAhdrCtx->hdrAttr.opMode == HDR_OpMode_Fast)
        AhdrApiFastMode(pAhdrCtx);

    //add linear tmo UI control
    if(pAhdrCtx->hdrAttr.opMode == HDR_OpMode_LINEAR)
    {
        pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsLowLight *= 1 + (float)(pAhdrCtx->hdrAttr.level_Linear_Dark) * 0.4;
        pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsLowLight =
            LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsLowLight, DETAILSLOWLIGHTMAX, DETAILSLOWLIGHTMIN);
        LOGD_AHDR("%s:  Linear Tmo On\n", __FUNCTION__);
        LOGD_AHDR("%s:  DetailsLowLightMode:%f CurrTotalFocusLuma:%f CurrDarkPdf:%f CurrISO:%f DetailsLowLight:%f\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DetailsLowLightMode,
                  pAhdrCtx->CurrHandleData.CurrTotalFocusLuma, pAhdrCtx->CurrHandleData.CurrDarkPdf, pAhdrCtx->CurrHandleData.CurrISO, pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsLowLight);
    }

    //get Current tmo TmoDamp
    pAhdrCtx->CurrHandleData.TmoDamp = pAhdrCtx->AhdrConfig.tmo_para.damp;


    pAhdrCtx->hdrAttr.RegInfo.OECurve_smooth = pAhdrCtx->CurrHandleData.CurrMergeHandleData.OECurve_smooth;
    pAhdrCtx->hdrAttr.RegInfo.OECurve_offset = pAhdrCtx->CurrHandleData.CurrMergeHandleData.OECurve_offset;
    pAhdrCtx->hdrAttr.RegInfo.MDCurveLM_smooth = pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveLM_smooth;
    pAhdrCtx->hdrAttr.RegInfo.MDCurveLM_offset = pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveLM_offset;
    pAhdrCtx->hdrAttr.RegInfo.MDCurveMS_smooth = pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveMS_smooth;
    pAhdrCtx->hdrAttr.RegInfo.MDCurveMS_offset = pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveMS_offset;

    pAhdrCtx->hdrAttr.RegInfo.GlobalLuma = pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeLuma;
    pAhdrCtx->hdrAttr.RegInfo.DetailsLowlight = pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsLowLight;
    pAhdrCtx->hdrAttr.RegInfo.DetailsHighlight = pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsHighLight;
    pAhdrCtx->hdrAttr.RegInfo.LocalTmoStrength = pAhdrCtx->CurrHandleData.CurrTmoHandleData.LocalTmoStrength;
    pAhdrCtx->hdrAttr.RegInfo.GlobaltmoStrength = pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobalTmoStrength;


    LOG1_AHDR( "%s:exit!\n", __FUNCTION__);
}
/******************************************************************************
 * AhdrApiOnUpdate()
 *
 *****************************************************************************/
void AhdrApiOnUpdate
(
    AhdrHandle_t     pAhdrCtx
)
{
    LOG1_AHDR("%s:enter!\n", __FUNCTION__);

    if (pAhdrCtx->hdrAttr.opMode == HDR_OpMode_Auto)
    {
        if (pAhdrCtx->hdrAttr.stAuto.bUpdateMge == true)
        {
            //get oe cruve
            pAhdrCtx->CurrHandleData.CurrMergeHandleData.OECurve_smooth =
                LIMIT_PARA(pAhdrCtx->hdrAttr.stAuto.stMgeAuto.stOECurve.stCoef, pAhdrCtx->hdrAttr.stAuto.stMgeAuto.stOECurve.stSmthMax,
                           pAhdrCtx->hdrAttr.stAuto.stMgeAuto.stOECurve.stSmthMin, pAhdrCtx->hdrAttr.stAuto.stMgeAuto.stOECurve.stCoefMax,
                           pAhdrCtx->hdrAttr.stAuto.stMgeAuto.stOECurve.stCoefMin);
            pAhdrCtx->CurrHandleData.CurrMergeHandleData.OECurve_offset =
                LIMIT_PARA(pAhdrCtx->hdrAttr.stAuto.stMgeAuto.stOECurve.stCoef, pAhdrCtx->hdrAttr.stAuto.stMgeAuto.stOECurve.stOfstMax,
                           pAhdrCtx->hdrAttr.stAuto.stMgeAuto.stOECurve.stOfstMin, pAhdrCtx->hdrAttr.stAuto.stMgeAuto.stOECurve.stCoefMax,
                           pAhdrCtx->hdrAttr.stAuto.stMgeAuto.stOECurve.stCoefMin);

            //get md cruve ms
            pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveMS_smooth =
                LIMIT_PARA(pAhdrCtx->hdrAttr.stAuto.stMgeAuto.stMDCurveMS.stCoef, pAhdrCtx->hdrAttr.stAuto.stMgeAuto.stMDCurveMS.stSmthMax,
                           pAhdrCtx->hdrAttr.stAuto.stMgeAuto.stMDCurveMS.stSmthMin, pAhdrCtx->hdrAttr.stAuto.stMgeAuto.stMDCurveMS.stCoefMax,
                           pAhdrCtx->hdrAttr.stAuto.stMgeAuto.stMDCurveMS.stCoefMin);
            pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveMS_offset =
                LIMIT_PARA(pAhdrCtx->hdrAttr.stAuto.stMgeAuto.stMDCurveMS.stCoef, pAhdrCtx->hdrAttr.stAuto.stMgeAuto.stMDCurveMS.stOfstMax,
                           pAhdrCtx->hdrAttr.stAuto.stMgeAuto.stMDCurveMS.stOfstMin, pAhdrCtx->hdrAttr.stAuto.stMgeAuto.stMDCurveMS.stCoefMax,
                           pAhdrCtx->hdrAttr.stAuto.stMgeAuto.stMDCurveMS.stCoefMin);

            //get md cruve lm
            pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveLM_smooth =
                LIMIT_PARA(pAhdrCtx->hdrAttr.stAuto.stMgeAuto.stMDCurveLM.stCoef, pAhdrCtx->hdrAttr.stAuto.stMgeAuto.stMDCurveLM.stSmthMax,
                           pAhdrCtx->hdrAttr.stAuto.stMgeAuto.stMDCurveLM.stSmthMin, pAhdrCtx->hdrAttr.stAuto.stMgeAuto.stMDCurveLM.stCoefMax,
                           pAhdrCtx->hdrAttr.stAuto.stMgeAuto.stMDCurveLM.stCoefMin);
            pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveLM_offset =
                LIMIT_PARA(pAhdrCtx->hdrAttr.stAuto.stMgeAuto.stMDCurveLM.stCoef, pAhdrCtx->hdrAttr.stAuto.stMgeAuto.stMDCurveLM.stOfstMax,
                           pAhdrCtx->hdrAttr.stAuto.stMgeAuto.stMDCurveLM.stOfstMin, pAhdrCtx->hdrAttr.stAuto.stMgeAuto.stMDCurveLM.stCoefMax,
                           pAhdrCtx->hdrAttr.stAuto.stMgeAuto.stMDCurveLM.stCoefMin);

        }
        else
            AhdrGetCurrMergeData(pAhdrCtx);

        //update tmo data in Auto mode
        if (pAhdrCtx->hdrAttr.stAuto.bUpdateTmo == true)
        {

            pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsLowLight =
                LIMIT_PARA(pAhdrCtx->hdrAttr.stAuto.stTmoAuto.stDtlsLL.stCoef, pAhdrCtx->hdrAttr.stAuto.stTmoAuto.stDtlsLL.stMax, pAhdrCtx->hdrAttr.stAuto.stTmoAuto.stDtlsLL.stMin,
                           pAhdrCtx->hdrAttr.stAuto.stTmoAuto.stDtlsLL.stCoefMax, pAhdrCtx->hdrAttr.stAuto.stTmoAuto.stDtlsLL.stMin);
            pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsLowLight = LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsLowLight
                    , IQDETAILSLOWLIGHTMAX, IQDETAILSLOWLIGHTMIN);

            pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsHighLight =
                LIMIT_PARA(pAhdrCtx->hdrAttr.stAuto.stTmoAuto.stDtlsHL.stCoef, pAhdrCtx->hdrAttr.stAuto.stTmoAuto.stDtlsHL.stMax, pAhdrCtx->hdrAttr.stAuto.stTmoAuto.stDtlsHL.stMin,
                           pAhdrCtx->hdrAttr.stAuto.stTmoAuto.stDtlsHL.stCoefMax, pAhdrCtx->hdrAttr.stAuto.stTmoAuto.stDtlsHL.stMin);
            pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsHighLight = LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsHighLight
                    , DETAILSHIGHLIGHTMAX, DETAILSHIGHLIGHTMIN);

            pAhdrCtx->CurrHandleData.CurrTmoHandleData.LocalTmoStrength =
                LIMIT_PARA(pAhdrCtx->hdrAttr.stAuto.stTmoAuto.stLocalTMO.stCoef, pAhdrCtx->hdrAttr.stAuto.stTmoAuto.stLocalTMO.stMax, pAhdrCtx->hdrAttr.stAuto.stTmoAuto.stLocalTMO.stMin,
                           pAhdrCtx->hdrAttr.stAuto.stTmoAuto.stLocalTMO.stCoefMax, pAhdrCtx->hdrAttr.stAuto.stTmoAuto.stLocalTMO.stMin);
            pAhdrCtx->CurrHandleData.CurrTmoHandleData.LocalTmoStrength = LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrTmoHandleData.LocalTmoStrength
                    , TMOCONTRASTMAX, TMOCONTRASTMIN);

            pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeLuma =
                LIMIT_PARA(pAhdrCtx->hdrAttr.stAuto.stTmoAuto.stGlobeLuma.stCoef, pAhdrCtx->hdrAttr.stAuto.stTmoAuto.stGlobeLuma.stMax, pAhdrCtx->hdrAttr.stAuto.stTmoAuto.stGlobeLuma.stMin,
                           pAhdrCtx->hdrAttr.stAuto.stTmoAuto.stGlobeLuma.stCoefMax, pAhdrCtx->hdrAttr.stAuto.stTmoAuto.stGlobeLuma.stMin);
            pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeLuma = LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeLuma
                    , GLOBELUMAMAX, GLOBELUMAMIN);

            float GlobeLuma = pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeLuma;
            pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeMaxLuma = MAXLUMAK * GlobeLuma + MAXLUMAB;
            pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeMaxLuma = LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeMaxLuma,
                    GLOBEMAXLUMAMAX, GLOBEMAXLUMAMIN);

            if(pAhdrCtx->hdrAttr.stAuto.stTmoAuto.stGlobalTMO.en)
            {
                float strength = LIMIT_PARA(pAhdrCtx->hdrAttr.stAuto.stTmoAuto.stGlobalTMO.stCoef, pAhdrCtx->hdrAttr.stAuto.stTmoAuto.stGlobalTMO.stMax, pAhdrCtx->hdrAttr.stAuto.stTmoAuto.stGlobalTMO.stMin,
                                            pAhdrCtx->hdrAttr.stAuto.stTmoAuto.stGlobalTMO.stCoefMax, pAhdrCtx->hdrAttr.stAuto.stTmoAuto.stGlobalTMO.stMin);
                pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobalTmoStrength = strength;
            }
            else
                pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobalTmoStrength = 0.5;

        }
        else
            AhdrGetCurrTmoData(pAhdrCtx);

        //paras after updating
        LOGD_AHDR("%s:	Ahdr api on!! Current Handle data:\n", __FUNCTION__);
        LOGD_AHDR("%s:	API mode is HDR_OpMode_Auto\n", __FUNCTION__);
        LOGD_AHDR("%s:	Current MDCurveMS_smooth:%f MDCurveMS_offset:%f MDCurveLM_smooth:%f MDCurveLM_offset:%f OECurve_smooth:%f OECurve_offset:%f\n", __FUNCTION__,
                  pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveMS_smooth, pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveMS_offset,
                  pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveLM_smooth, pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveLM_offset,
                  pAhdrCtx->CurrHandleData.CurrMergeHandleData.OECurve_smooth, pAhdrCtx->CurrHandleData.CurrMergeHandleData.OECurve_offset);
        LOGD_AHDR("%s:	Current GlobeLuma:%f GlobeMaxLuma:%f DetailsHighLight:%f DetailsLowLight:%f LocalTmoStrength:%f BandPriorStrength:%f\n", __FUNCTION__,
                  pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeLuma, pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeMaxLuma,
                  pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsHighLight, pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsLowLight,
                  pAhdrCtx->CurrHandleData.CurrTmoHandleData.LocalTmoStrength, pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobalTmoStrength);
    }
    else if (pAhdrCtx->hdrAttr.opMode == HDR_OpMode_MANU)
    {
        //update merge data in manual mode
        if (pAhdrCtx->hdrAttr.stManual.bUpdateMge == true)
        {
            pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveMS_smooth = pAhdrCtx->hdrAttr.stManual.stMgeManual.MDCurveMS_smooth * MDCURVESMOOTHMAX;
            pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveMS_smooth = LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveMS_smooth,
                    MDCURVESMOOTHMAX, MDCURVESMOOTHMIN);
            pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveMS_offset = pAhdrCtx->hdrAttr.stManual.stMgeManual.MDCurveMS_offset * MDCURVEOFFSETMAX;
            pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveMS_offset = LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveMS_offset,
                    MDCURVEOFFSETMAX, MDCURVEOFFSETMIN);

            pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveLM_smooth = pAhdrCtx->hdrAttr.stManual.stMgeManual.MDCurveLM_smooth * MDCURVESMOOTHMAX;
            pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveLM_smooth = LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveLM_smooth,
                    MDCURVESMOOTHMAX, MDCURVESMOOTHMIN);
            pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveLM_offset = pAhdrCtx->hdrAttr.stManual.stMgeManual.MDCurveLM_offset * MDCURVEOFFSETMAX;
            pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveLM_offset = LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveLM_offset,
                    MDCURVEOFFSETMAX, MDCURVEOFFSETMIN);

            pAhdrCtx->CurrHandleData.CurrMergeHandleData.OECurve_smooth = pAhdrCtx->hdrAttr.stManual.stMgeManual.OECurve_smooth * OECURVESMOOTHMAX;
            pAhdrCtx->CurrHandleData.CurrMergeHandleData.OECurve_smooth = LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrMergeHandleData.OECurve_smooth,
                    OECURVESMOOTHMAX, OECURVESMOOTHMIN);
            pAhdrCtx->CurrHandleData.CurrMergeHandleData.OECurve_offset = pAhdrCtx->hdrAttr.stManual.stMgeManual.OECurve_offset;
            pAhdrCtx->CurrHandleData.CurrMergeHandleData.OECurve_offset = LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrMergeHandleData.OECurve_offset,
                    OECURVEOFFSETMAX, OECURVEOFFSETMIN);


            pAhdrCtx->CurrHandleData.MergeOEDamp = pAhdrCtx->hdrAttr.stManual.stMgeManual.dampOE;
            pAhdrCtx->CurrHandleData.MergeMDDampLM = pAhdrCtx->hdrAttr.stManual.stMgeManual.dampMDLM;
            pAhdrCtx->CurrHandleData.MergeMDDampMS = pAhdrCtx->hdrAttr.stManual.stMgeManual.dampMDMS;

        }
        else
            AhdrGetCurrMergeData(pAhdrCtx);

        //update tmo data in manual mode
        if (pAhdrCtx->hdrAttr.stManual.bUpdateTmo == true)
        {
            pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsLowLight = pAhdrCtx->hdrAttr.stManual.stTmoManual.stDtlsLL ;
            pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsLowLight = LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsLowLight
                    , IQDETAILSLOWLIGHTMAX, IQDETAILSLOWLIGHTMIN);

            pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsHighLight = pAhdrCtx->hdrAttr.stManual.stTmoManual.stDtlsHL ;
            pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsHighLight = LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsHighLight
                    , DETAILSHIGHLIGHTMAX, DETAILSHIGHLIGHTMIN);

            pAhdrCtx->CurrHandleData.CurrTmoHandleData.LocalTmoStrength = pAhdrCtx->hdrAttr.stManual.stTmoManual.stLocalTMOStrength;
            pAhdrCtx->CurrHandleData.CurrTmoHandleData.LocalTmoStrength = LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrTmoHandleData.LocalTmoStrength
                    , TMOCONTRASTMAX, TMOCONTRASTMIN);

            pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeLuma = pAhdrCtx->hdrAttr.stManual.stTmoManual.stGlobeLuma;
            pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeLuma = LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeLuma
                    , GLOBELUMAMAX, GLOBELUMAMIN);

            float GlobeLuma = pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeLuma;
            pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeMaxLuma = MAXLUMAK * GlobeLuma + MAXLUMAB;
            pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeMaxLuma = LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeMaxLuma,
                    GLOBEMAXLUMAMAX, GLOBEMAXLUMAMIN);

            pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobalTmoStrength = LIMIT_VALUE(pAhdrCtx->hdrAttr.stManual.stTmoManual.stGlobalTMOStrength,
                    1, 0);

            pAhdrCtx->CurrHandleData.TmoDamp = pAhdrCtx->hdrAttr.stManual.stTmoManual.damp;



        }
        else
            AhdrGetCurrTmoData(pAhdrCtx);

        //paras after updating
        LOGD_AHDR("%s:  Ahdr api on!! Current Handle data:\n", __FUNCTION__);
        LOGD_AHDR("%s:  API mode is HDR_OpMode_MANU\n", __FUNCTION__);
        LOGD_AHDR("%s:	Current MDCurveMS_smooth:%f MDCurveMS_offset:%f MDCurveLM_smooth:%f MDCurveLM_offset:%f OECurve_smooth:%f OECurve_offset:%f\n", __FUNCTION__,
                  pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveMS_smooth, pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveMS_offset,
                  pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveLM_smooth, pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveLM_offset,
                  pAhdrCtx->CurrHandleData.CurrMergeHandleData.OECurve_smooth, pAhdrCtx->CurrHandleData.CurrMergeHandleData.OECurve_offset);
        LOGD_AHDR("%s:  Current GlobeLuma:%f GlobeMaxLuma:%f DetailsHighLight:%f DetailsLowLight:%f TmoContrast:%f BandPriorStrength:%f\n", __FUNCTION__,
                  pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeLuma, pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeMaxLuma,
                  pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsHighLight, pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsLowLight,
                  pAhdrCtx->CurrHandleData.CurrTmoHandleData.LocalTmoStrength, pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobalTmoStrength);

    }

    //transfer control data to api
    pAhdrCtx->hdrAttr.CtlInfo.Envlv = pAhdrCtx->CurrHandleData.CurrEnvLv;
    pAhdrCtx->hdrAttr.CtlInfo.MoveCoef = pAhdrCtx->CurrHandleData.CurrMoveCoef;
    pAhdrCtx->hdrAttr.CtlInfo.ISO = pAhdrCtx->CurrHandleData.CurrISO;
    pAhdrCtx->hdrAttr.CtlInfo.OEPdf = pAhdrCtx->CurrHandleData.CurrOEPdf;
    pAhdrCtx->hdrAttr.CtlInfo.DarkPdf = pAhdrCtx->CurrHandleData.CurrDarkPdf;
    pAhdrCtx->hdrAttr.CtlInfo.FocusLuma = pAhdrCtx->CurrHandleData.CurrTotalFocusLuma;
    pAhdrCtx->hdrAttr.CtlInfo.DynamicRange = pAhdrCtx->CurrHandleData.CurrDynamicRange;

    //transfer register data to api
    pAhdrCtx->hdrAttr.RegInfo.OECurve_smooth = pAhdrCtx->CurrHandleData.CurrMergeHandleData.OECurve_smooth;
    pAhdrCtx->hdrAttr.RegInfo.OECurve_offset = pAhdrCtx->CurrHandleData.CurrMergeHandleData.OECurve_offset;
    pAhdrCtx->hdrAttr.RegInfo.MDCurveLM_smooth = pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveLM_smooth;
    pAhdrCtx->hdrAttr.RegInfo.MDCurveLM_offset = pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveLM_offset;
    pAhdrCtx->hdrAttr.RegInfo.MDCurveMS_smooth = pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveMS_smooth;
    pAhdrCtx->hdrAttr.RegInfo.MDCurveMS_offset = pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveMS_offset;

    pAhdrCtx->hdrAttr.RegInfo.GlobalLuma = pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeLuma;
    pAhdrCtx->hdrAttr.RegInfo.DetailsLowlight = pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsLowLight;
    pAhdrCtx->hdrAttr.RegInfo.DetailsHighlight = pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsHighLight;
    pAhdrCtx->hdrAttr.RegInfo.LocalTmoStrength = pAhdrCtx->CurrHandleData.CurrTmoHandleData.LocalTmoStrength;
    pAhdrCtx->hdrAttr.RegInfo.GlobaltmoStrength = pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobalTmoStrength;

    LOG1_AHDR( "%s:exit!\n", __FUNCTION__);
}

/******************************************************************************
 * AhdrSelectMode()
 *transfer html parameter into handle
 ***************************************************************************/
void AhdrSelectMode
(
    AhdrHandle_t           pAhdrCtx,
    CalibDb_Ahdr_Para_t*         pCalibDb,
    int mode
) {
    LOG1_AHDR( "%s:enter!\n", __FUNCTION__);

    // initial checks
    DCT_ASSERT(pAhdrCtx != NULL);
    DCT_ASSERT(pCalibDb != NULL);

    //merge
    pAhdrCtx->AhdrConfig.merge_para.OECurve_damp = LIMIT_VALUE(pCalibDb->merge.oeCurve_damp, DAMPMAX, DAMPMIN);
    pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_damp = LIMIT_VALUE(pCalibDb->merge.mdCurveLm_damp, DAMPMAX, DAMPMIN);
    pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_damp = LIMIT_VALUE(pCalibDb->merge.mdCurveMs_damp, DAMPMAX, DAMPMIN);
    for (int i = 0; i < 6; i++ )
    {
        pAhdrCtx->AhdrConfig.merge_para.EnvLv[i] = LIMIT_VALUE(pCalibDb->merge.envLevel[i], ENVLVMAX, ENVLVMIN);
        pAhdrCtx->AhdrConfig.merge_para.MoveCoef[i] = LIMIT_VALUE(pCalibDb->merge.moveCoef[i], MOVECOEFMAX, MOVECOEFMIN);
        pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[i] = LIMIT_VALUE(pCalibDb->merge.oeCurve_smooth[i], IQPARAMAX, IQPARAMIN);
        pAhdrCtx->AhdrConfig.merge_para.OECurve_offset[i] = LIMIT_VALUE(pCalibDb->merge.oeCurve_offset[i], OECURVEOFFSETMAX, OECURVEOFFSETMIN);
        pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[i] = LIMIT_VALUE(pCalibDb->merge.mdCurveLm_smooth[i], IQPARAMAX, IQPARAMIN);
        pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[i] = LIMIT_VALUE(pCalibDb->merge.mdCurveLm_offset[i], IQPARAMAX, IQPARAMIN);
        pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[i] = LIMIT_VALUE(pCalibDb->merge.mdCurveMs_smooth[i], IQPARAMAX, IQPARAMIN);
        pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[i] = LIMIT_VALUE(pCalibDb->merge.mdCurveMs_offset[i], IQPARAMAX, IQPARAMIN);
    }

    //TMO
    pAhdrCtx->AhdrConfig.tmo_para.Luma.globalLumaMode = LIMIT_VALUE(pCalibDb->tmo.luma[mode].GlobalLumaMode, GLOBALLUMAMODEMAX, GLOBALLUMAMODEMIN);
    pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DetailsLowLightMode = LIMIT_VALUE(pCalibDb->tmo.LowLight[mode].DetailsLowLightMode, DETAILSLOWLIGHTMODEMAX, DETAILSLOWLIGHTMODEMIN);
    pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.DetailsHighLightMode = LIMIT_VALUE(pCalibDb->tmo.HighLight[mode].DetailsHighLightMode, DETAILSHIGHLIGHTMODEMAX, DETAILSHIGHLIGHTMODEMIN);
    pAhdrCtx->AhdrConfig.tmo_para.local.localtmoMode = LIMIT_VALUE(pCalibDb->tmo.LocalTMO[mode].LocalTMOMode, TMOCONTRASTMODEMAX, TMOCONTRASTMODEMIN);
    pAhdrCtx->AhdrConfig.tmo_para.damp = LIMIT_VALUE(pCalibDb->tmo.damp, DAMPMAX, DAMPMIN);
    pAhdrCtx->AhdrConfig.tmo_para.Luma.Tolerance = LIMIT_VALUE(pCalibDb->tmo.luma[mode].Tolerance, TOLERANCEMAX, TOLERANCEMIN);
    pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.Tolerance = LIMIT_VALUE(pCalibDb->tmo.HighLight[mode].Tolerance, TOLERANCEMAX, TOLERANCEMIN);
    pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.Tolerance = LIMIT_VALUE(pCalibDb->tmo.LowLight[mode].Tolerance, TOLERANCEMAX, TOLERANCEMIN);
    pAhdrCtx->AhdrConfig.tmo_para.local.Tolerance = LIMIT_VALUE(pCalibDb->tmo.LocalTMO[mode].Tolerance, TOLERANCEMAX, TOLERANCEMIN);
    for(int i = 0; i < 6; i++)
    {
        pAhdrCtx->AhdrConfig.tmo_para.Luma.EnvLv[i] = LIMIT_VALUE(pCalibDb->tmo.luma[mode].envLevel[i], ENVLVMAX, ENVLVMIN);
        pAhdrCtx->AhdrConfig.tmo_para.Luma.ISO[i] = LIMIT_VALUE(pCalibDb->tmo.luma[mode].ISO[i], ISOMAX, ISOMIN);
        pAhdrCtx->AhdrConfig.tmo_para.Luma.GlobeLuma[i] = LIMIT_VALUE(pCalibDb->tmo.luma[mode].globalLuma[i], IQPARAMAX, IQPARAMIN) ;

        pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.OEPdf[i] = LIMIT_VALUE(pCalibDb->tmo.HighLight[mode].OEPdf[i], OEPDFMAX, OEPDFMIN) ;
        pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.EnvLv[i] = LIMIT_VALUE(pCalibDb->tmo.HighLight[mode].EnvLv[i], ENVLVMAX, ENVLVMIN);
        pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.DetailsHighLight[i] = LIMIT_VALUE(pCalibDb->tmo.HighLight[mode].detailsHighLight[i], IQPARAMAX, IQPARAMIN) ;

        pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.FocusLuma[i] = LIMIT_VALUE(pCalibDb->tmo.LowLight[mode].FocusLuma[i], FOCUSLUMAMAX, FOCUSLUMAMIN) ;
        pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DarkPdf[i] = LIMIT_VALUE(pCalibDb->tmo.LowLight[mode].DarkPdf[i], DARKPDFMAX, DARKPDFMIN) ;
        pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.ISO[i] = LIMIT_VALUE(pCalibDb->tmo.LowLight[mode].ISO[i], ISOMAX, ISOMIN) ;
        pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DetailsLowLight[i] = LIMIT_VALUE(pCalibDb->tmo.LowLight[mode].detailsLowLight[i], IQDETAILSLOWLIGHTMAX, IQDETAILSLOWLIGHTMIN) ;

        pAhdrCtx->AhdrConfig.tmo_para.local.DynamicRange[i] = LIMIT_VALUE(pCalibDb->tmo.LocalTMO[mode].DynamicRange[i], DYNAMICRANGEMAX, DYNAMICRANGEMIN) ;
        pAhdrCtx->AhdrConfig.tmo_para.local.EnvLv[i] = LIMIT_VALUE(pCalibDb->tmo.LocalTMO[mode].EnvLv[i], ENVLVMAX, ENVLVMIN) ;
        pAhdrCtx->AhdrConfig.tmo_para.local.LocalTmoStrength[i] = LIMIT_VALUE(pCalibDb->tmo.LocalTMO[mode].Strength[i], IQPARAMAX, IQPARAMIN) ;
    }

    //band prior
    pAhdrCtx->AhdrConfig.tmo_para.global.isHdrGlobalTmo =
        pCalibDb->tmo.GlobaTMO[mode].en == 0 ? false : true;
    pAhdrCtx->AhdrConfig.tmo_para.global.mode = LIMIT_VALUE(pCalibDb->tmo.GlobaTMO[mode].mode, TMOCONTRASTMODEMAX, TMOCONTRASTMODEMIN);
    pAhdrCtx->AhdrConfig.tmo_para.global.Tolerance = LIMIT_VALUE(pCalibDb->tmo.GlobaTMO[mode].Tolerance, TOLERANCEMAX, TOLERANCEMIN);
    for(int i = 0; i < 6; i++)
    {
        pAhdrCtx->AhdrConfig.tmo_para.global.DynamicRange[i] = LIMIT_VALUE(pCalibDb->tmo.GlobaTMO[mode].DynamicRange[i], DYNAMICRANGEMAX, DYNAMICRANGEMIN) ;
        pAhdrCtx->AhdrConfig.tmo_para.global.EnvLv[i] = LIMIT_VALUE(pCalibDb->tmo.GlobaTMO[mode].EnvLv[i], ENVLVMAX, ENVLVMIN) ;
        pAhdrCtx->AhdrConfig.tmo_para.global.GlobalTmoStrength[i] = LIMIT_VALUE(pCalibDb->tmo.GlobaTMO[mode].Strength[i], IQPARAMAX, IQPARAMIN) ;
    }
    pAhdrCtx->AhdrConfig.tmo_para.global.iir = LIMIT_VALUE(pCalibDb->tmo.GlobaTMO[mode].iir, IIRMAX, IIRMIN);

    //linear tmo
    pAhdrCtx->AhdrConfig.tmo_para.isLinearTmoOn =
        (pCalibDb->tmo.isLinearTmoOn != 0 && pAhdrCtx->hdr_mode == 1) ? true : false;
    pAhdrCtx->AhdrProcRes.isLinearTmoOn = pAhdrCtx->AhdrConfig.tmo_para.isLinearTmoOn;

    LOG1_AHDR("%s:  Ahdr comfig data from xml:\n", __FUNCTION__);
    LOG1_AHDR("%s:  Merge MergeMode:%d:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.MergeMode);
    LOG1_AHDR("%s:  Merge EnvLv:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.EnvLv[0], pAhdrCtx->AhdrConfig.merge_para.EnvLv[1], pAhdrCtx->AhdrConfig.merge_para.EnvLv[2],
              pAhdrCtx->AhdrConfig.merge_para.EnvLv[3], pAhdrCtx->AhdrConfig.merge_para.EnvLv[4], pAhdrCtx->AhdrConfig.merge_para.EnvLv[5]);
    LOG1_AHDR("%s:  Merge OECurve_smooth:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[0], pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[1], pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[2],
              pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[3], pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[4], pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[5]);
    LOG1_AHDR("%s:  Merge OECurve_offset:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.OECurve_offset[0], pAhdrCtx->AhdrConfig.merge_para.OECurve_offset[1], pAhdrCtx->AhdrConfig.merge_para.OECurve_offset[2],
              pAhdrCtx->AhdrConfig.merge_para.OECurve_offset[3], pAhdrCtx->AhdrConfig.merge_para.OECurve_offset[4], pAhdrCtx->AhdrConfig.merge_para.OECurve_offset[5]);
    LOG1_AHDR("%s:  Merge MoveCoef:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.MoveCoef[0], pAhdrCtx->AhdrConfig.merge_para.MoveCoef[1], pAhdrCtx->AhdrConfig.merge_para.MoveCoef[2],
              pAhdrCtx->AhdrConfig.merge_para.MoveCoef[3], pAhdrCtx->AhdrConfig.merge_para.MoveCoef[4], pAhdrCtx->AhdrConfig.merge_para.MoveCoef[5]);
    LOG1_AHDR("%s:  Merge MDCurveLM_smooth:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[0], pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[1], pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[2],
              pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[3], pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[4], pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[5]);
    LOG1_AHDR("%s:  Merge MDCurveLM_offset:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[0], pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[1], pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[2],
              pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[3], pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[4], pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[5]);
    LOG1_AHDR("%s:  Merge MDCurveMS_smooth:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[0], pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[1], pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[2],
              pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[3], pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[4], pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[5]);
    LOG1_AHDR("%s:  Merge MDCurveMS_offset:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[0], pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[1], pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[2],
              pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[3], pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[4], pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[5]);
    LOG1_AHDR("%s:  Merge OECurve_damp:%f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.OECurve_damp);
    LOG1_AHDR("%s:  Merge MDCurveLM_damp:%f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_damp);
    LOG1_AHDR("%s:  Merge MDCurveMS_damp:%f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_damp);

    LOGE_AHDR("%s:  Tmo Linear Tmo en:%d\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.isLinearTmoOn);
    LOG1_AHDR("%s:  Tmo GlobalLuma EnvLv:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.Luma.EnvLv[0], pAhdrCtx->AhdrConfig.tmo_para.Luma.EnvLv[1], pAhdrCtx->AhdrConfig.tmo_para.Luma.EnvLv[2]
              , pAhdrCtx->AhdrConfig.tmo_para.Luma.EnvLv[3], pAhdrCtx->AhdrConfig.tmo_para.Luma.EnvLv[4], pAhdrCtx->AhdrConfig.tmo_para.Luma.EnvLv[5]);
    LOG1_AHDR("%s:  Tmo GlobalLuma ISO:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.Luma.ISO[0], pAhdrCtx->AhdrConfig.tmo_para.Luma.ISO[1], pAhdrCtx->AhdrConfig.tmo_para.Luma.ISO[2]
              , pAhdrCtx->AhdrConfig.tmo_para.Luma.ISO[3], pAhdrCtx->AhdrConfig.tmo_para.Luma.ISO[4], pAhdrCtx->AhdrConfig.tmo_para.Luma.ISO[5]);
    LOG1_AHDR("%s:  Tmo EnvLvTolerance:%f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.Luma.Tolerance);
    LOG1_AHDR("%s:  Tmo GlobeLuma:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.Luma.GlobeLuma[0], pAhdrCtx->AhdrConfig.tmo_para.Luma.GlobeLuma[1], pAhdrCtx->AhdrConfig.tmo_para.Luma.GlobeLuma[2]
              , pAhdrCtx->AhdrConfig.tmo_para.Luma.GlobeLuma[3], pAhdrCtx->AhdrConfig.tmo_para.Luma.GlobeLuma[4], pAhdrCtx->AhdrConfig.tmo_para.Luma.GlobeLuma[5]);
    LOG1_AHDR("%s:  Tmo DetailsHighLightMode:%f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.DetailsHighLightMode);
    LOG1_AHDR("%s:  Tmo OEPdf:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.OEPdf[0], pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.OEPdf[1], pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.OEPdf[2]
              , pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.OEPdf[3], pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.OEPdf[4], pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.OEPdf[5]);
    LOG1_AHDR("%s:  Tmo DetailsHighLight EnvLv:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.EnvLv[0], pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.EnvLv[1], pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.EnvLv[2]
              , pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.EnvLv[3], pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.EnvLv[4], pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.EnvLv[5]);
    LOG1_AHDR("%s:  Tmo OETolerance:%f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.Tolerance);
    LOG1_AHDR("%s:  Tmo DetailsHighLight:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.DetailsHighLight[0], pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.DetailsHighLight[1], pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.DetailsHighLight[2]
              , pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.DetailsHighLight[3], pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.DetailsHighLight[4], pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.DetailsHighLight[5]);
    LOG1_AHDR("%s:  Tmo DetailsLowLightMode:%f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DetailsLowLightMode);
    LOG1_AHDR("%s:  Tmo FocusLuma:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.FocusLuma[0], pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.FocusLuma[1], pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.FocusLuma[2]
              , pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.FocusLuma[3], pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.FocusLuma[4], pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.FocusLuma[5]);
    LOG1_AHDR("%s:  Tmo DarkPdf:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DarkPdf[0], pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DarkPdf[1], pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DarkPdf[2]
              , pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DarkPdf[3], pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DarkPdf[4], pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DarkPdf[5]);
    LOG1_AHDR("%s:  Tmo ISO:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.ISO[0], pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.ISO[1], pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.ISO[2]
              , pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.ISO[3], pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.ISO[4], pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.ISO[5]);
    LOG1_AHDR("%s:  Tmo DTPdfTolerance:%f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.Tolerance);
    LOG1_AHDR("%s:  Tmo DetailsLowLight:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DetailsLowLight[0], pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DetailsLowLight[1], pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DetailsLowLight[2]
              , pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DetailsLowLight[3], pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DetailsLowLight[4], pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DetailsLowLight[5]);
    LOG1_AHDR("%s:  Tmo LocalTMOMode:%f\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.local.localtmoMode);
    LOG1_AHDR("%s:  Tmo LocalTMO DynamicRange:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.local.DynamicRange[0], pAhdrCtx->AhdrConfig.tmo_para.local.DynamicRange[1], pAhdrCtx->AhdrConfig.tmo_para.local.DynamicRange[2]
              , pAhdrCtx->AhdrConfig.tmo_para.local.DynamicRange[3], pAhdrCtx->AhdrConfig.tmo_para.local.DynamicRange[4], pAhdrCtx->AhdrConfig.tmo_para.local.DynamicRange[5]);
    LOG1_AHDR("%s:  Tmo LocalTMO EnvLv:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.local.EnvLv[0], pAhdrCtx->AhdrConfig.tmo_para.local.EnvLv[1], pAhdrCtx->AhdrConfig.tmo_para.local.EnvLv[2]
              , pAhdrCtx->AhdrConfig.tmo_para.local.EnvLv[3], pAhdrCtx->AhdrConfig.tmo_para.local.EnvLv[4], pAhdrCtx->AhdrConfig.tmo_para.local.EnvLv[5]);
    LOG1_AHDR("%s:  Tmo LocalTMO Tolerance:%f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.local.Tolerance);
    LOG1_AHDR("%s:  Tmo LocalTMO:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.local.LocalTmoStrength[0], pAhdrCtx->AhdrConfig.tmo_para.local.LocalTmoStrength[1], pAhdrCtx->AhdrConfig.tmo_para.local.LocalTmoStrength[2]
              , pAhdrCtx->AhdrConfig.tmo_para.local.LocalTmoStrength[3], pAhdrCtx->AhdrConfig.tmo_para.local.LocalTmoStrength[4], pAhdrCtx->AhdrConfig.tmo_para.local.LocalTmoStrength[5]);
    LOG1_AHDR("%s:  Tmo GlobalTMO en%d\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.global.isHdrGlobalTmo);
    LOG1_AHDR("%s:  Tmo GlobalTMO mode:%f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.global.mode);
    LOG1_AHDR("%s:  Tmo GlobalTMO DynamicRange:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.global.DynamicRange[0], pAhdrCtx->AhdrConfig.tmo_para.global.DynamicRange[1], pAhdrCtx->AhdrConfig.tmo_para.global.DynamicRange[2]
              , pAhdrCtx->AhdrConfig.tmo_para.global.DynamicRange[3], pAhdrCtx->AhdrConfig.tmo_para.global.DynamicRange[4], pAhdrCtx->AhdrConfig.tmo_para.global.DynamicRange[5]);
    LOG1_AHDR("%s:  Tmo GlobalTMO EnvLv:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.global.EnvLv[0], pAhdrCtx->AhdrConfig.tmo_para.global.EnvLv[1], pAhdrCtx->AhdrConfig.tmo_para.global.EnvLv[2]
              , pAhdrCtx->AhdrConfig.tmo_para.global.EnvLv[3], pAhdrCtx->AhdrConfig.tmo_para.global.EnvLv[4], pAhdrCtx->AhdrConfig.tmo_para.global.EnvLv[5]);
    LOG1_AHDR("%s:  Tmo GlobalTMO Tolerance:%f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.global.Tolerance);
    LOG1_AHDR("%s:  Tmo GlobalTMO Strength:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.global.GlobalTmoStrength[0], pAhdrCtx->AhdrConfig.tmo_para.global.GlobalTmoStrength[1], pAhdrCtx->AhdrConfig.tmo_para.global.GlobalTmoStrength[2]
              , pAhdrCtx->AhdrConfig.tmo_para.global.GlobalTmoStrength[3], pAhdrCtx->AhdrConfig.tmo_para.global.GlobalTmoStrength[4], pAhdrCtx->AhdrConfig.tmo_para.global.GlobalTmoStrength[5]);
    LOG1_AHDR("%s:  Tmo Damp:%f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.damp);

    //turn the IQ paras into algo paras
    for(int i = 0; i < 6; i++)
    {
        pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[i] = pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[i] * OECURVESMOOTHMAX;
        pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[i] = LIMIT_VALUE(pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[i], OECURVESMOOTHMAX, OECURVESMOOTHMIN) ;

        pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[i] = pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[i] * MDCURVESMOOTHMAX;
        pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[i] = LIMIT_VALUE(pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[i], MDCURVESMOOTHMAX, MDCURVESMOOTHMIN) ;
        pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[i] = pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[i] * MDCURVEOFFSETMAX;
        pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[i] = LIMIT_VALUE(pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[i], MDCURVEOFFSETMAX, MDCURVEOFFSETMIN) ;

        pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[i] = pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[i] * MDCURVESMOOTHMAX;
        pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[i] = LIMIT_VALUE(pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[i], MDCURVESMOOTHMAX, MDCURVESMOOTHMIN) ;
        pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[i] = pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[i] * MDCURVEOFFSETMAX;
        pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[i] = LIMIT_VALUE(pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[i], MDCURVEOFFSETMAX, MDCURVEOFFSETMIN) ;

        pAhdrCtx->AhdrConfig.tmo_para.Luma.GlobeLuma[i] = pAhdrCtx->AhdrConfig.tmo_para.Luma.GlobeLuma[i] * GLOBELUMAMAX;
        pAhdrCtx->AhdrConfig.tmo_para.Luma.GlobeLuma[i] = LIMIT_VALUE(pAhdrCtx->AhdrConfig.tmo_para.Luma.GlobeLuma[i], GLOBELUMAMAX, GLOBELUMAMIN) ;

        pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.DetailsHighLight[i] = pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.DetailsHighLight[i] * DETAILSHIGHLIGHTMAX;
        pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.DetailsHighLight[i] = LIMIT_VALUE(pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.DetailsHighLight[i], DETAILSHIGHLIGHTMAX, DETAILSHIGHLIGHTMIN) ;

        pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DetailsLowLight[i] = pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DetailsLowLight[i] * DETAILSLOWLIGHTMIN;
        pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DetailsLowLight[i] = LIMIT_VALUE(pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DetailsLowLight[i], DETAILSLOWLIGHTMAX, DETAILSLOWLIGHTMIN) ;

        pAhdrCtx->AhdrConfig.tmo_para.local.LocalTmoStrength[i] = pAhdrCtx->AhdrConfig.tmo_para.local.LocalTmoStrength[i] * TMOCONTRASTMAX;
        pAhdrCtx->AhdrConfig.tmo_para.local.LocalTmoStrength[i] = LIMIT_VALUE(pAhdrCtx->AhdrConfig.tmo_para.local.LocalTmoStrength[i], TMOCONTRASTMAX, TMOCONTRASTMIN) ;

    }


    LOG1_AHDR("%s:  Merge algo OECurve_smooth:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[0], pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[1], pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[2],
              pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[3], pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[4], pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[5]);
    LOG1_AHDR("%s:  Merge algo MDCurveLM_smooth:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[0], pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[1], pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[2],
              pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[3], pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[4], pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[5]);
    LOG1_AHDR("%s:  Merge algo MDCurveLM_offset:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[0], pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[1], pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[2],
              pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[3], pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[4], pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[5]);
    LOG1_AHDR("%s:  Merge algo MDCurveMS_smooth:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[0], pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[1], pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[2],
              pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[3], pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[4], pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[5]);
    LOG1_AHDR("%s:  Merge algo MDCurveMS_offset:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[0], pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[1], pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[2],
              pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[3], pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[4], pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[5]);
    LOG1_AHDR("%s:  Tmo algo GlobeLuma:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.Luma.GlobeLuma[0], pAhdrCtx->AhdrConfig.tmo_para.Luma.GlobeLuma[1], pAhdrCtx->AhdrConfig.tmo_para.Luma.GlobeLuma[2]
              , pAhdrCtx->AhdrConfig.tmo_para.Luma.GlobeLuma[3], pAhdrCtx->AhdrConfig.tmo_para.Luma.GlobeLuma[4], pAhdrCtx->AhdrConfig.tmo_para.Luma.GlobeLuma[5]);
    LOG1_AHDR("%s:  Tmo algo DetailsHighLight:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.DetailsHighLight[0], pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.DetailsHighLight[1], pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.DetailsHighLight[2]
              , pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.DetailsHighLight[3], pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.DetailsHighLight[4], pAhdrCtx->AhdrConfig.tmo_para.DtsHiLit.DetailsHighLight[5]);
    LOG1_AHDR("%s:  Tmo algo DetailsLowLight:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DetailsLowLight[0], pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DetailsLowLight[1], pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DetailsLowLight[2]
              , pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DetailsLowLight[3], pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DetailsLowLight[4], pAhdrCtx->AhdrConfig.tmo_para.DtsLoLit.DetailsLowLight[5]);
    LOG1_AHDR("%s:  Tmo algo TmoContrast:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.local.LocalTmoStrength[0], pAhdrCtx->AhdrConfig.tmo_para.local.LocalTmoStrength[1], pAhdrCtx->AhdrConfig.tmo_para.local.LocalTmoStrength[2]
              , pAhdrCtx->AhdrConfig.tmo_para.local.LocalTmoStrength[3], pAhdrCtx->AhdrConfig.tmo_para.local.LocalTmoStrength[4], pAhdrCtx->AhdrConfig.tmo_para.local.LocalTmoStrength[5]);

    LOG1_AHDR( "%s:exit!\n", __FUNCTION__);
}
/******************************************************************************
 * AhdrUpdateConfig()
 *get handle para by config and current variate
 *****************************************************************************/
void AhdrUpdateConfig
(
    AhdrHandle_t     pAhdrCtx,
    AecPreResult_t  AecHdrPreResult,
    af_preprocess_result_t AfPreResult
)
{
    LOG1_AHDR("%s:enter!\n", __FUNCTION__);

    LOGD_AHDR("%s:  Ahdr Current frame cnt:%d:\n",  __FUNCTION__, pAhdrCtx->frameCnt);

    //get Current hdr mode
    pAhdrCtx->CurrHandleData.MergeMode = pAhdrCtx->hdr_mode - 1;
    LOGD_AHDR("%s:  Current MergeMode: %d \n", __FUNCTION__, pAhdrCtx->CurrHandleData.MergeMode);

    //get current ae data from AecPreRes
    AhdrGetAeResult(pAhdrCtx, AecHdrPreResult);

    //transfer ae data to CurrHandle
    pAhdrCtx->CurrHandleData.CurrEnvLv = LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrEnvLv, ENVLVMAX, ENVLVMIN);

    pAhdrCtx->CurrHandleData.CurrMoveCoef = 1;
    pAhdrCtx->CurrHandleData.CurrMoveCoef = LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrMoveCoef, MOVECOEFMAX, MOVECOEFMIN);

    pAhdrCtx->CurrHandleData.CurrISO = pAhdrCtx->CurrAeResult.ISO;
    pAhdrCtx->CurrHandleData.CurrISO = LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrISO, ISOMAX, ISOMIN);

    pAhdrCtx->CurrHandleData.CurrOEPdf = pAhdrCtx->CurrAeResult.OEPdf;
    pAhdrCtx->CurrHandleData.CurrOEPdf = LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrOEPdf, OEPDFMAX, OEPDFMIN);

    pAhdrCtx->CurrHandleData.CurrTotalFocusLuma = 1;
    pAhdrCtx->CurrHandleData.CurrTotalFocusLuma = LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrTotalFocusLuma, FOCUSLUMAMAX, FOCUSLUMAMIN);

    pAhdrCtx->CurrHandleData.CurrDarkPdf = pAhdrCtx->CurrAeResult.DarkPdf;
    pAhdrCtx->CurrHandleData.CurrDarkPdf = LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrDarkPdf, 0.5, 0);

    pAhdrCtx->CurrHandleData.CurrDynamicRange = pAhdrCtx->CurrAeResult.DynamicRange;
    pAhdrCtx->CurrHandleData.CurrDynamicRange = LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrDynamicRange, DYNAMICRANGEMAX, DYNAMICRANGEMIN);


    if(pAhdrCtx->hdrAttr.bEnable == false || pAhdrCtx->hdrAttr.opMode == HDR_OpMode_Fast
            || pAhdrCtx->hdrAttr.opMode == HDR_OpMode_LINEAR) //api close
        AhdrApiOffUpdate(pAhdrCtx, AecHdrPreResult, AfPreResult);
    else //api on
        AhdrApiOnUpdate(pAhdrCtx);

    //read current rodata
    pAhdrCtx->CurrHandleData.CurrLgMean = pAhdrCtx->CurrStatsData.tmo_stats.ro_hdrtmo_lgmean / 2048.0;

    //calc the current merge luma
    float MergeLuma = (float)pAhdrCtx->CurrStatsData.tmo_stats.ro_hdrtmo_lgmean;
    MergeLuma = MergeLuma / 2048.0;
    float lgmean = MergeLuma;
    MergeLuma = pow(2, MergeLuma);
    MergeLuma =  MergeLuma / 16;

    //get pre frame tmo mean luma
    unsigned long tmo_mean = 0;
    for(int i = 0; i < 225; i++) {
        tmo_mean += pAhdrCtx->CurrStatsData.other_stats.tmo_luma[i];
    }
    tmo_mean = tmo_mean / 225;
    tmo_mean = tmo_mean / 16;

    //calc short middle long frame mean luma
    unsigned long short_mean = 0, middle_mean = 0, long_mean = 0;
    for (int i = 0; i < 225; i++)
    {
        short_mean += pAhdrCtx->CurrAeResult.BlockLumaS[i];
        long_mean += pAhdrCtx->CurrAeResult.BlockLumaL[i];
    }
    short_mean = short_mean / 225;
    long_mean = long_mean / 225;
    short_mean = short_mean / 16;
    long_mean = long_mean / 16;

    for(int i = 0; i < 25; i++)
        middle_mean += pAhdrCtx->CurrAeResult.BlockLumaM[i];
    middle_mean = middle_mean / 25;
    middle_mean = middle_mean / 16;

    LOGD_AHDR("%s:  preFrame lgMergeLuma:%f MergeLuma(8bit):%f TmoLuma(8bit):%d\n", __FUNCTION__, lgmean, MergeLuma, tmo_mean);
    LOGD_AHDR("%s:  preFrame SLuma(8bit):%d MLuma(8bit):%d LLuma(8bit):%d\n", __FUNCTION__, short_mean, middle_mean, long_mean);

    LOG1_AHDR( "%s:exit!\n", __FUNCTION__);
}
/******************************************************************************
 * AhdrRelease()
 *****************************************************************************/
bool BandPrior
(
    AhdrHandle_t pAhdrCtx
) {

    LOG1_AHDR( "%s:enter!\n", __FUNCTION__);

    bool returnValue = false;

    if(pAhdrCtx->AhdrConfig.tmo_para.global.isHdrGlobalTmo == true) {
        returnValue = true;
        pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_set_weightkey = 0;
    }

    else
        returnValue = false;

    LOGD_AHDR("%s: BandPrior:%d\n", __FUNCTION__, returnValue);

    return returnValue;

    LOG1_AHDR( "%s:exit!\n", __FUNCTION__);
}

/******************************************************************************
 * AhdrInit()
 *****************************************************************************/
RESULT AhdrInit
(
    AhdrInstanceConfig_t* pInstConfig
) {

    AhdrContext_s *pAhdrCtx;

    LOG1_AHDR("%s:enter!\n", __FUNCTION__);

    RESULT result = AHDR_RET_SUCCESS;

    // initial checks
    if (pInstConfig == NULL)
        return (AHDR_RET_INVALID_PARM);

    // allocate AHDR control context
    pAhdrCtx = (AhdrContext_s*)malloc(sizeof(AhdrContext_s));
    if (NULL == pAhdrCtx) {
        LOGE_AHDR( "%s: Can't allocate AHDR context\n",  __FUNCTION__);
        return (AHDR_RET_OUTOFMEM);
    }

    // pre-initialize context
    memset(pAhdrCtx, 0x00, sizeof(*pAhdrCtx));

    pAhdrCtx->state = AHDR_STATE_INITIALIZED;

    // memory check
    if (NULL == pAhdrCtx) {
        return (AHDR_RET_WRONG_HANDLE);
    }
    // state check
    if ((AHDR_STATE_INITIALIZED != pAhdrCtx->state)
            && (AHDR_STATE_STOPPED != pAhdrCtx->state)) {
        return (AHDR_RET_WRONG_STATE);
    }

    pInstConfig->hAhdr = (AhdrHandle_t)pAhdrCtx;

    LOG1_AHDR( "%s:exit!\n", __FUNCTION__);

    return (AHDR_RET_SUCCESS);
}
/******************************************************************************
 * AhdrRelease()
 *****************************************************************************/
RESULT AhdrRelease
(
    AhdrHandle_t pAhdrCtx
) {

    LOG1_AHDR( "%s:enter!\n", __FUNCTION__);
    RESULT result = AHDR_RET_SUCCESS;

    // initial checks
    if (NULL == pAhdrCtx) {
        return (AHDR_RET_WRONG_HANDLE);
    }

    result = AhdrStop(pAhdrCtx);
    if (result != AHDR_RET_SUCCESS) {
        LOGE_AHDR( "%s: AHDRStop() failed!\n", __FUNCTION__);
        return (result);
    }

    // check state
    if ((AHDR_STATE_RUNNING == pAhdrCtx->state)
            || (AHDR_STATE_LOCKED == pAhdrCtx->state)) {
        return (AHDR_RET_BUSY);
    }

    memset(pAhdrCtx, 0, sizeof(AhdrContext_s));
    free(pAhdrCtx);
    pAhdrCtx = NULL;

    LOG1_AHDR( "%s:exit!\n", __FUNCTION__);

    return (AHDR_RET_SUCCESS);
}
/******************************************************************************
 * AhdrRelease()
 *****************************************************************************/
void AhdrProcessing
(
    AhdrHandle_t pAhdrCtx
) {

    LOG1_AHDR( "%s:enter!\n", __FUNCTION__);

    MergeProcessing(pAhdrCtx);
    TmoProcessing(pAhdrCtx);

    //Band prior
    pAhdrCtx->AhdrProcRes.isHdrGlobalTmo = BandPrior(pAhdrCtx);

    // store current handle data to pre data for next loop
    pAhdrCtx->AhdrPrevData.ro_hdrtmo_lgmean = pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_set_lgmean;
    pAhdrCtx->AhdrPrevData.PreL2S_ratio = pAhdrCtx->CurrHandleData.CurrL2S_Ratio;
    pAhdrCtx->AhdrPrevData.PreLExpo = pAhdrCtx->CurrHandleData.CurrLExpo;
    pAhdrCtx->AhdrPrevData.PreEnvlv = pAhdrCtx->CurrHandleData.CurrEnvLv;
    pAhdrCtx->AhdrPrevData.PreMoveCoef = pAhdrCtx->CurrHandleData.CurrMoveCoef;
    pAhdrCtx->AhdrPrevData.PreOEPdf = pAhdrCtx->CurrHandleData.CurrOEPdf;
    pAhdrCtx->AhdrPrevData.PreTotalFocusLuma = pAhdrCtx->CurrHandleData.CurrTotalFocusLuma;
    pAhdrCtx->AhdrPrevData.PreDarkPdf = pAhdrCtx->CurrHandleData.CurrDarkPdf;
    pAhdrCtx->AhdrPrevData.PreISO = pAhdrCtx->CurrHandleData.CurrISO;
    pAhdrCtx->AhdrPrevData.PreDynamicRange = pAhdrCtx->CurrHandleData.CurrDynamicRange;
    memcpy(&pAhdrCtx->AhdrPrevData.PrevMergeHandleData, &pAhdrCtx->CurrHandleData.CurrMergeHandleData, sizeof(MergeHandleData_s));
    memcpy(&pAhdrCtx->AhdrPrevData.PrevTmoHandleData, &pAhdrCtx->CurrHandleData.CurrTmoHandleData, sizeof(TmoHandleData_s));
    ++pAhdrCtx->frameCnt;

    LOG1_AHDR( "%s:exit!\n", __FUNCTION__);
}
