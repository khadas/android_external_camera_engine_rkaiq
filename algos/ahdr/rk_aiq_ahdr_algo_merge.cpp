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
 * @file merge.cpp
 *
 * @brief
 *   ADD_DESCRIPTION_HERE
 *
 *****************************************************************************/
#include "math.h"
#include "rk_aiq_types_ahdr_algo_int.h"
#include "rk_aiq_ahdr_algo_merge.h"

/******************************************************************************
* CalibrateOECurve()
*****************************************************************************/
void CalibrateOECurve
(
    float smooth, float offset, unsigned short *OECurve
)
{
    LOGI_AHDR("%s:Enter!\n", __FUNCTION__);

    int step = 32 ;
    float curve = 0;
    float k = 511;

    for(int i = 0; i < 17; ++i)
    {
        curve = 1 + exp(-smooth * (k / 1023 - offset / 256));
        curve = 1024 / curve ;
        OECurve[i] = round(curve) ;
        OECurve[i] = MIN(OECurve[i], 1023);
        k += step ;
    }

    LOGI_AHDR("%s:Eixt!\n", __FUNCTION__);

}
/******************************************************************************
* CalibrateMDCurve()
*****************************************************************************/
void CalibrateMDCurve
(
    float smooth, float offset, unsigned short *MDCurve
)
{
    LOGI_AHDR("%s:Enter!\n", __FUNCTION__);

    int step = 16;
    float curve;
    float k = 0;

    for (int i = 0; i < 17; ++i)
    {
        curve = 1 + exp(-smooth * (k / 1023 - offset / 256));
        curve = 1024 / curve ;
        MDCurve[i] = round(curve) ;
        MDCurve[i] = MIN(MDCurve[i], 1023);
        k += step ;
    }

    LOGI_AHDR("%s:Eixt!\n", __FUNCTION__);
}
/******************************************************************************
 * GetCurrIOData()
 *****************************************************************************/
RESULT MergeGetCurrIOData
(
    AhdrHandle_t pAhdrCtx
)
{
    LOGI_AHDR("%s:Enter!\n", __FUNCTION__);
    RESULT result = AHDR_RET_SUCCESS;
    int OECurve[17];

    pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_mode = pAhdrCtx->CurrHandleData.MergeMode;
    CalibrateOECurve(pAhdrCtx->CurrHandleData.CurrMergeHandleData.OECurve_smooth,
                     pAhdrCtx->CurrHandleData.CurrMergeHandleData.OECurve_offset, pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_e_y) ;
    CalibrateMDCurve(pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveLM_smooth,
                     pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveLM_offset, pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_l1_y);
    CalibrateMDCurve(pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveMS_smooth,
                     pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveMS_offset, pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_l0_y);
    pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_lm_dif_0p9 = 230;
    pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_ms_dif_0p8 = 205;
    pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_lm_dif_0p15 = (int)pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveLM_offset;
    pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_ms_dif_0p15 = (int)pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveMS_offset;


#if 1
    //when gainX = 1, gainX_inv = 1/gainX -1
    pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_gain0 = (int)SHIFT6BIT(pAhdrCtx->CurrHandleData.CurrL2S_Ratio);
    if(pAhdrCtx->CurrHandleData.CurrL2S_Ratio == 1)
        pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_gain0_inv = (int)(SHIFT12BIT(1 / pAhdrCtx->CurrHandleData.CurrL2S_Ratio) - 1);
    else
        pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_gain0_inv = (int)SHIFT12BIT(1 / pAhdrCtx->CurrHandleData.CurrL2S_Ratio);

    pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_gain1 = (int)SHIFT6BIT(pAhdrCtx->CurrHandleData.CurrL2M_Ratio);
    if(pAhdrCtx->CurrHandleData.CurrL2M_Ratio == 1)
        pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_gain1_inv = (int)(SHIFT12BIT(1 / pAhdrCtx->CurrHandleData.CurrL2M_Ratio) - 1);
    else
        pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_gain1_inv = (int)SHIFT12BIT(1 / pAhdrCtx->CurrHandleData.CurrL2M_Ratio);

    pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_gain2 = (int)SHIFT6BIT(pAhdrCtx->CurrHandleData.CurrL2L_Ratio);
#endif

#if LRK_DEBUG_LOG
    LOGE_AHDR("%s:lrk  Merge set IOdata to register:\n", __FUNCTION__);
    LOGE_AHDR("%s:lrk  sw_hdrmge_mode:%d \n", __FUNCTION__, pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_mode);
    LOGE_AHDR("%s:lrk  sw_hdrmge_ms_dif_0p8:%d \n", __FUNCTION__, pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_ms_dif_0p8);
    LOGE_AHDR("%s:lrk  sw_hdrmge_lm_dif_0p9:%d \n", __FUNCTION__, pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_lm_dif_0p9);
    LOGE_AHDR("%s:lrk  sw_hdrmge_ms_dif_0p15:%d \n", __FUNCTION__, pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_ms_dif_0p15);
    LOGE_AHDR("%s:lrk  sw_hdrmge_gain1:%d \n", __FUNCTION__, pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_gain1);
    LOGE_AHDR("%s:lrk  sw_hdrmge_gain1_inv:%d \n", __FUNCTION__, pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_gain1_inv);
    LOGE_AHDR("%s:lrk  sw_hdrmge_gain2:%d \n", __FUNCTION__, pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_gain2);
    LOGE_AHDR("%s:lrk  sw_hdrmge_gain0:%d \n", __FUNCTION__, pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_gain0);
    LOGE_AHDR("%s:lrk  sw_hdrmge_gain0_inv:%d \n", __FUNCTION__, pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_gain0_inv);
    LOGE_AHDR("%s:lrk  sw_hdrmge_lm_dif_0p15:%d \n", __FUNCTION__, pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_lm_dif_0p15);
    for(int i = 0; i < 17; i++)
        LOGE_AHDR("%s:lrk  sw_hdrmge_e_y[%d]:%d \n", __FUNCTION__, i, pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_e_y[i]);
    for(int i = 0; i < 17; i++)
        LOGE_AHDR("%s:lrk  sw_hdrmge_l0_y[%d]:%d \n", __FUNCTION__, i, pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_l0_y[i]);
    for(int i = 0; i < 17; i++)
        LOGE_AHDR("%s:lrk  sw_hdrmge_l1_y[%d]:%d \n", __FUNCTION__, i, pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_l1_y[i]);
#endif

    LOGI_AHDR("%s:Eixt!\n", __FUNCTION__);
    return(result);
}

/******************************************************************************
 * MergeProcess()
 *****************************************************************************/
RESULT MergeProcessing
(
    AhdrHandle_t pAhdrCtx
)
{
    LOGI_AHDR("%s:Enter!\n", __FUNCTION__);
    RESULT result = AHDR_RET_SUCCESS;

    float OEDamp = pAhdrCtx->CurrHandleData.MergeOEDamp;
    float MDDampLM = pAhdrCtx->CurrHandleData.MergeMDDampLM;
    float MDDampMS = pAhdrCtx->CurrHandleData.MergeMDDampMS;

    //get finnal current data
    if (pAhdrCtx->frameCnt != 0)
    {
        pAhdrCtx->CurrHandleData.CurrMergeHandleData.OECurve_smooth = OEDamp * pAhdrCtx->CurrHandleData.CurrMergeHandleData.OECurve_smooth
                + (1 - OEDamp) * pAhdrCtx->AhdrPrevData.PrevMergeHandleData.OECurve_smooth;
        pAhdrCtx->CurrHandleData.CurrMergeHandleData.OECurve_offset = OEDamp * pAhdrCtx->CurrHandleData.CurrMergeHandleData.OECurve_offset
                + (1 - OEDamp) * pAhdrCtx->AhdrPrevData.PrevMergeHandleData.OECurve_offset;
        pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveLM_smooth = MDDampLM * pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveLM_smooth
                + (1 - MDDampLM) * pAhdrCtx->AhdrPrevData.PrevMergeHandleData.MDCurveLM_smooth;
        pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveLM_offset = MDDampLM * pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveLM_offset
                + (1 - MDDampLM) * pAhdrCtx->AhdrPrevData.PrevMergeHandleData.MDCurveLM_offset;
        pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveMS_smooth = MDDampMS * pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveMS_smooth
                + (1 - MDDampMS) * pAhdrCtx->AhdrPrevData.PrevMergeHandleData.MDCurveMS_smooth;
        pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveMS_offset = MDDampMS * pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveMS_offset
                + (1 - MDDampMS) * pAhdrCtx->AhdrPrevData.PrevMergeHandleData.MDCurveMS_offset;
    }

    //get current IO data
    result = MergeGetCurrIOData(pAhdrCtx);

#if LRK_DEBUG_LOG
    LOGE_AHDR("%s:lrk  Current damp OECurve_smooth:%f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrMergeHandleData.OECurve_smooth);
    LOGE_AHDR("%s:lrk  Current damp OECurve_offset:%f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrMergeHandleData.OECurve_offset);
    LOGE_AHDR("%s:lrk  Current damp MDCurveLM_smooth:%f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveLM_smooth);
    LOGE_AHDR("%s:lrk  Current damp MDCurveLM_offset:%f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveLM_offset);
    LOGE_AHDR("%s:lrk  Current damp MDCurveMS_smooth:%f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveMS_smooth);
    LOGE_AHDR("%s:lrk  Current damp MDCurveMS_offset:%f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveMS_offset);
#endif

    LOGI_AHDR("%s:Eixt!\n", __FUNCTION__);
    return(result);
}
