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

/******************************************************************************
 * AhdrStart()
 *****************************************************************************/
RESULT AhdrStart
(
    AhdrHandle_t pAhdrCtx
) {

    LOGI_AHDR( "%s:enter!\n", __FUNCTION__);

    // initial checks
    if (pAhdrCtx == NULL) {
        return (AHDR_RET_WRONG_HANDLE);
    }

    if ((AHDR_STATE_RUNNING == pAhdrCtx->state)
            || (AHDR_STATE_LOCKED == pAhdrCtx->state)) {
        return (AHDR_RET_WRONG_STATE);
    }

    pAhdrCtx->state = AHDR_STATE_RUNNING;

    LOGI_AHDR( "%s:exit!\n", __FUNCTION__);
    return (AHDR_RET_SUCCESS);
}
/******************************************************************************
 * AhdrStop()
 *****************************************************************************/
RESULT AhdrStop
(
    AhdrHandle_t pAhdrCtx
) {

    LOGI_AHDR( "%s:enter!\n", __FUNCTION__);

    // initial checks
    if (pAhdrCtx == NULL) {
        return (AHDR_RET_WRONG_HANDLE);
    }

    // before stopping, unlock the AHDR if locked
    if (AHDR_STATE_LOCKED == pAhdrCtx->state) {
        return (AHDR_RET_WRONG_STATE);
    }

    pAhdrCtx->state = AHDR_STATE_STOPPED;

    LOGI_AHDR( "%s:exit!\n", __FUNCTION__);

    return (AHDR_RET_SUCCESS);
}
/******************************************************************************
 * SetFirstPara()
 *****************************************************************************/
void SetFirstPara
(
    AhdrHandle_t pAhdrCtx
) {

    LOGI_AHDR( "%s:enter!\n", __FUNCTION__);
    //default setting
    float OECurve_smooth = 80;
    float OECurve_offset = 205;
    float MDCurveLM_smooth = 80;
    float MDCurveLM_offset = 38;
    float MDCurveMS_smooth = 80;
    float MDCurveMS_offset = 38;

    float GlobeMaxLuma = 307;
    float GlobeLuma = 184;
    float DetailsHighLight = 512;
    float DetailsLowLight  = 16;
    float TmoContrast = 76;

    //merge
    if(pAhdrCtx->hdr_mode == 3)
        pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_mode = 2;
    else if(pAhdrCtx->hdr_mode == 2)
        pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_mode = 1;;

    pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_gain0 = 1;
    pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_gain0_inv = 4095;
    pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_gain1 = 1;
    pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_gain1_inv = 4095;
    pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_gain2 = 1;
    pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_ms_dif_0p8 = 205;
    pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_lm_dif_0p9 = 230;
    pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_ms_dif_0p15 = MDCurveMS_offset;
    pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_lm_dif_0p15 = MDCurveLM_offset;
    CalibrateOECurve(OECurve_smooth, OECurve_offset, pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_e_y);
    CalibrateMDCurve(MDCurveLM_smooth, MDCurveLM_offset, pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_l1_y);
    CalibrateMDCurve(MDCurveMS_smooth, MDCurveMS_offset, pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_l0_y);

    //tmo
    pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_clipratio0 = 64;
    pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_clipratio1 = 166;
    pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_clipgap0 = 12;
    pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_clipgap1 = 12;
    pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_ratiol = 32;
    pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_hist_min = 0;
    pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_hist_low = 0;
    pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_hist_high = (int)(pAhdrCtx->height * pAhdrCtx->width * 0.01 / 16);
    pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_hist_0p3 = 614;
    pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_hist_shift = 3;
    pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_palpha_0p18 = (int)(GlobeLuma + 0.5);
    pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_palpha_lw0p5 = (int)(DetailsHighLight + 0.5);
    pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_palpha_lwscl = (int)(DetailsLowLight + 0.5);
    pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_maxpalpha = (int)(GlobeMaxLuma + 0.5);
    pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_maxgain = 4096;
    pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_cfg_alpha = 255;
    pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_set_gainoff = 1;
    pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_set_lgmin = 1;
    pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_set_lgmax = 1;
    pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_set_lgmean = 1;
    pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_set_weightkey = (int)(TmoContrast + 0.5);
    pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_set_lgrange0 = 1;
    pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_set_lgrange1 = 1;
    pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_set_lgavgmax = 1;
    pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_set_palpha = 1;
    pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_gain_ld_off1 = 10;
    pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_gain_ld_off2 = 5;

    pAhdrCtx->frameCnt = 0;

    //get pre data from next
    pAhdrCtx->AhdrPrevData.PrevMergeHandleData.OECurve_smooth = OECurve_smooth;
    pAhdrCtx->AhdrPrevData.PrevMergeHandleData.OECurve_offset = OECurve_offset;
    pAhdrCtx->AhdrPrevData.PrevMergeHandleData.MDCurveLM_smooth = MDCurveLM_smooth;
    pAhdrCtx->AhdrPrevData.PrevMergeHandleData.MDCurveLM_offset = MDCurveLM_offset;
    pAhdrCtx->AhdrPrevData.PrevMergeHandleData.MDCurveMS_smooth = MDCurveMS_smooth;
    pAhdrCtx->AhdrPrevData.PrevMergeHandleData.MDCurveMS_offset = MDCurveMS_offset;

    pAhdrCtx->AhdrPrevData.PrevTmoHandleData.GlobeLuma = GlobeLuma;
    pAhdrCtx->AhdrPrevData.PrevTmoHandleData.GlobeMaxLuma = GlobeMaxLuma;
    pAhdrCtx->AhdrPrevData.PrevTmoHandleData.DetailsHighLight = DetailsHighLight;
    pAhdrCtx->AhdrPrevData.PrevTmoHandleData.DetailsLowLight = DetailsLowLight;
    pAhdrCtx->AhdrPrevData.PrevTmoHandleData.TmoContrast = TmoContrast;

    pAhdrCtx->AhdrPrevData.PreL2S_ratio = 1;
    pAhdrCtx->AhdrPrevData.PreLExpo = 1 * 0.03;
    pAhdrCtx->AhdrPrevData.PreEnvlv = 1;
    pAhdrCtx->AhdrPrevData.PreMoveCoef = 1;
    pAhdrCtx->AhdrPrevData.PreOEPdf = 0.5;
    pAhdrCtx->AhdrPrevData.PreTotalFocusLuma = 50;
    pAhdrCtx->AhdrPrevData.PreDarkPdf = 0.5;
    pAhdrCtx->AhdrPrevData.PreDynamicRange = 1;

#if LRK_DEBUG_LOG
    LOGD_AHDR("%s:lrk  Merge set IOdata to register:\n", __FUNCTION__);
    LOGD_AHDR("%s:lrk  sw_hdrmge_mode:%d \n", __FUNCTION__, pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_mode);
    LOGD_AHDR("%s:lrk  sw_hdrmge_ms_dif_0p8:%d \n", __FUNCTION__, pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_ms_dif_0p8);
    LOGD_AHDR("%s:lrk  sw_hdrmge_lm_dif_0p9:%d \n", __FUNCTION__, pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_lm_dif_0p9);
    LOGD_AHDR("%s:lrk  sw_hdrmge_ms_dif_0p15:%d \n", __FUNCTION__, pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_ms_dif_0p15);
    LOGD_AHDR("%s:lrk  sw_hdrmge_lm_dif_0p15:%d \n", __FUNCTION__, pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_lm_dif_0p15);
    LOGD_AHDR("%s:lrk  sw_hdrmge_gain0:%d \n", __FUNCTION__, pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_gain0);
    LOGD_AHDR("%s:lrk  sw_hdrmge_gain0_inv:%d \n", __FUNCTION__, pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_gain0_inv);
    LOGD_AHDR("%s:lrk  sw_hdrmge_gain1:%d \n", __FUNCTION__, pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_gain1);
    LOGD_AHDR("%s:lrk  sw_hdrmge_gain1_inv:%d \n", __FUNCTION__, pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_gain1_inv);
    LOGD_AHDR("%s:lrk  sw_hdrmge_gain2:%d \n", __FUNCTION__, pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_gain2);
    for(int i = 0; i < 17; i++)
        LOGD_AHDR("%s:lrk  sw_hdrmge_e_y[%d]:%d \n", __FUNCTION__, i, pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_e_y[i]);
    for(int i = 0; i < 17; i++)
        LOGD_AHDR("%s:lrk  sw_hdrmge_l0_y[%d]:%d \n", __FUNCTION__, i, pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_l0_y[i]);
    for(int i = 0; i < 17; i++)
        LOGD_AHDR("%s:lrk  sw_hdrmge_l1_y[%d]:%d \n", __FUNCTION__, i, pAhdrCtx->AhdrProcRes.MgeProcRes.sw_hdrmge_l1_y[i]);

    LOGD_AHDR("%s:lrk  Tmo set IOdata to register:\n", __FUNCTION__);
    LOGD_AHDR("%s:lrk  sw_hdrtmo_lgmax:%d\n", __FUNCTION__, pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_lgmax);
    LOGD_AHDR("%s:lrk  sw_hdrtmo_lgscl:%d\n", __FUNCTION__, pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_lgscl);
    LOGD_AHDR("%s:lrk  sw_hdrtmo_lgscl_inv:%d\n", __FUNCTION__, pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_lgscl_inv);
    LOGD_AHDR("%s:lrk  sw_hdrtmo_clipratio0:%d\n", __FUNCTION__, pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_clipratio0);
    LOGD_AHDR("%s:lrk  sw_hdrtmo_clipratio1:%d\n", __FUNCTION__, pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_clipratio1);
    LOGD_AHDR("%s:lrk  sw_hdrtmo_clipgap0:%d\n", __FUNCTION__, pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_clipgap0);
    LOGD_AHDR("%s:lrk  sw_hdrtmo_clipgap:%d\n", __FUNCTION__, pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_clipgap1);
    LOGD_AHDR("%s:lrk  sw_hdrtmo_ratiol:%d\n", __FUNCTION__, pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_ratiol);
    LOGD_AHDR("%s:lrk  sw_hdrtmo_hist_min:%d\n", __FUNCTION__, pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_hist_min);
    LOGD_AHDR("%s:lrk  sw_hdrtmo_hist_low:%d\n", __FUNCTION__, pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_hist_low);
    LOGD_AHDR("%s:lrk  sw_hdrtmo_hist_high:%d\n", __FUNCTION__, pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_hist_high);
    LOGD_AHDR("%s:lrk  sw_hdrtmo_hist_0p3:%d\n", __FUNCTION__, pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_hist_0p3);
    LOGD_AHDR("%s:lrk  sw_hdrtmo_hist_shift:%d\n", __FUNCTION__, pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_hist_shift);
    LOGD_AHDR("%s:lrk  sw_hdrtmo_palpha_0p18:%d\n", __FUNCTION__, pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_palpha_0p18);
    LOGD_AHDR("%s:lrk  sw_hdrtmo_palpha_lw0p5:%d\n", __FUNCTION__, pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_palpha_lw0p5);
    LOGD_AHDR("%s:lrk  sw_hdrtmo_palpha_lwscl:%d\n", __FUNCTION__, pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_palpha_lwscl);
    LOGD_AHDR("%s:lrk  sw_hdrtmo_maxpalpha:%d\n", __FUNCTION__, pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_maxpalpha);
    LOGD_AHDR("%s:lrk  sw_hdrtmo_maxgain:%d\n", __FUNCTION__, pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_maxgain);
    LOGD_AHDR("%s:lrk  sw_hdrtmo_cfg_alpha:%d\n", __FUNCTION__, pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_cfg_alpha);
    LOGD_AHDR("%s:lrk  sw_hdrtmo_set_gainoff:%d\n", __FUNCTION__, pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_set_gainoff);
    LOGD_AHDR("%s:lrk  sw_hdrtmo_set_lgmin:%d\n", __FUNCTION__, pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_set_lgmin);
    LOGD_AHDR("%s:lrk  sw_hdrtmo_set_lgmax:%d\n", __FUNCTION__, pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_set_lgmax);
    LOGD_AHDR("%s:lrk  sw_hdrtmo_set_lgmean:%d\n", __FUNCTION__, pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_set_lgmean);
    LOGD_AHDR("%s:lrk  sw_hdrtmo_set_weightkey:%d\n", __FUNCTION__, pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_set_weightkey);
    LOGD_AHDR("%s:lrk  sw_hdrtmo_set_lgrange0:%d\n", __FUNCTION__, pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_set_lgrange0);
    LOGD_AHDR("%s:lrk  sw_hdrtmo_set_lgrange1:%d\n", __FUNCTION__, pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_set_lgrange1);
    LOGD_AHDR("%s:lrk  sw_hdrtmo_set_lgavgmax:%d\n", __FUNCTION__, pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_set_lgavgmax);
    LOGD_AHDR("%s:lrk  sw_hdrtmo_set_palpha:%d\n", __FUNCTION__, pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_set_palpha);
    LOGD_AHDR("%s:lrk  sw_hdrtmo_big_en:%d\n", __FUNCTION__, pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_big_en);
    LOGD_AHDR("%s:lrk  sw_hdrtmo_nobig_en:%d\n", __FUNCTION__, pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_nobig_en);
    LOGD_AHDR("%s:lrk  sw_hdrtmo_newhist_en:%d\n", __FUNCTION__, pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_newhist_en);
    LOGD_AHDR("%s:lrk  sw_hdrtmo_cnt_mode:%d\n", __FUNCTION__, pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_cnt_mode);
    LOGD_AHDR("%s:lrk  sw_hdrtmo_cnt_vsize:%d\n", __FUNCTION__, pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_cnt_vsize);
    LOGD_AHDR("%s:lrk  sw_hdrtmo_expl_lgratio:%d\n", __FUNCTION__, pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_expl_lgratio);
    LOGD_AHDR("%s:lrk  sw_hdrtmo_lgscl_ratio:%d\n", __FUNCTION__, pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_lgscl_ratio);
    LOGD_AHDR("%s:lrk  sw_hdrtmo_gain_ld_off1:%d\n", __FUNCTION__, pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_gain_ld_off1);
    LOGD_AHDR("%s:lrk  sw_hdrtmo_gain_ld_off2:%d\n", __FUNCTION__, pAhdrCtx->AhdrProcRes.TmoProcRes.sw_hdrtmo_gain_ld_off2);

#endif


    LOGI_AHDR( "%s:exit!\n", __FUNCTION__);
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
    LOGI_AHDR( "%s:enter!\n", __FUNCTION__);
    float x1;
    float x2;
    float value1;
    float value2;
    float outPara;

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
        LOGD_AHDR( "%s:lrk  wrong para!!!\n", __FUNCTION__);

    return outPara;
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
    pAhdrCtx->AhdrConfig.tmo_para.EnvLv[0] = 0 ;
    pAhdrCtx->AhdrConfig.tmo_para.EnvLv[1] = 0.1 ;
    pAhdrCtx->AhdrConfig.tmo_para.EnvLv[2] = 0.2 ;
    pAhdrCtx->AhdrConfig.tmo_para.EnvLv[3] = 0.3 ;
    pAhdrCtx->AhdrConfig.tmo_para.EnvLv[4] = 0.9 ;
    pAhdrCtx->AhdrConfig.tmo_para.EnvLv[5] = 1.0 ;
    pAhdrCtx->AhdrConfig.tmo_para.EnvLvTolerance = 0 ;
    pAhdrCtx->AhdrConfig.tmo_para.OEPdf[0] = 0;
    pAhdrCtx->AhdrConfig.tmo_para.OEPdf[1] = 0.1;
    pAhdrCtx->AhdrConfig.tmo_para.OEPdf[2] = 0.2;
    pAhdrCtx->AhdrConfig.tmo_para.OEPdf[3] = 0.3;
    pAhdrCtx->AhdrConfig.tmo_para.OEPdf[4] = 0.4;
    pAhdrCtx->AhdrConfig.tmo_para.OEPdf[5] = 0.5;
    pAhdrCtx->AhdrConfig.tmo_para.OETolerance = 0;
    pAhdrCtx->AhdrConfig.tmo_para.enFocusLuma = 0;
    pAhdrCtx->AhdrConfig.tmo_para.DTPdfTolerance = 0;
    pAhdrCtx->AhdrConfig.tmo_para.FocusLuma[0] = 0;
    pAhdrCtx->AhdrConfig.tmo_para.FocusLuma[1] = 0.1;
    pAhdrCtx->AhdrConfig.tmo_para.FocusLuma[2] = 0.2;
    pAhdrCtx->AhdrConfig.tmo_para.FocusLuma[3] = 0.3;
    pAhdrCtx->AhdrConfig.tmo_para.FocusLuma[4] = 0.4;
    pAhdrCtx->AhdrConfig.tmo_para.FocusLuma[5] = 0.1;
    pAhdrCtx->AhdrConfig.tmo_para.DarkPdf[0] = 0;
    pAhdrCtx->AhdrConfig.tmo_para.DarkPdf[1] = 0.1;
    pAhdrCtx->AhdrConfig.tmo_para.DarkPdf[2] = 0.2;
    pAhdrCtx->AhdrConfig.tmo_para.DarkPdf[3] = 0.3;
    pAhdrCtx->AhdrConfig.tmo_para.DarkPdf[4] = 0.4;
    pAhdrCtx->AhdrConfig.tmo_para.DarkPdf[5] = 0.5;
    pAhdrCtx->AhdrConfig.tmo_para.DRTolerance = 0;
    pAhdrCtx->AhdrConfig.tmo_para.DynamicRange[0] = 1;
    pAhdrCtx->AhdrConfig.tmo_para.DynamicRange[1] = 1;
    pAhdrCtx->AhdrConfig.tmo_para.DynamicRange[2] = 1;
    pAhdrCtx->AhdrConfig.tmo_para.DynamicRange[3] = 1;
    pAhdrCtx->AhdrConfig.tmo_para.DynamicRange[4] = 1;
    pAhdrCtx->AhdrConfig.tmo_para.DynamicRange[5] = 1;
    pAhdrCtx->AhdrConfig.tmo_para.DayTh = 0.7;
    for(int i = 0; i < 6; ++i)
    {
        pAhdrCtx->AhdrConfig.tmo_para.GlobeLuma[i] = 0.25 ;
        pAhdrCtx->AhdrConfig.tmo_para.DetailsHighLight[i] = 0.5;
        pAhdrCtx->AhdrConfig.tmo_para.DetailsLowLight[i] = 1;
        pAhdrCtx->AhdrConfig.tmo_para.TmoContrast[i] = 0.5;
    }

    //config default AhdrPrevData data
    for(int i = 0; i < 16; i++)
        pAhdrCtx->AhdrPrevData.ro_hdrtmo_lgmean_all[i] = 128;

    LOGI_AHDR( "%s:exit!\n", __FUNCTION__);
}
void AhdrGetROData
(
    AhdrHandle_t           pAhdrCtx,
    rkisp_ahdr_stats_t*         ROData
) {
    LOGI_AHDR( "%s:enter!\n", __FUNCTION__);

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

#if LRK_DEBUG_LOG
    LOGD_AHDR("%s:lrk  Ahdr RO data from register:\n", __FUNCTION__);
    LOGD_AHDR("%s:lrk  ro_hdrtmo_lglow:%d:\n", __FUNCTION__, pAhdrCtx->CurrStatsData.tmo_stats.ro_hdrtmo_lglow);
    LOGD_AHDR("%s:lrk  ro_hdrtmo_lgmin:%d:\n", __FUNCTION__, pAhdrCtx->CurrStatsData.tmo_stats.ro_hdrtmo_lgmin);
    LOGD_AHDR("%s:lrk  ro_hdrtmo_lgmax:%d:\n", __FUNCTION__, pAhdrCtx->CurrStatsData.tmo_stats.ro_hdrtmo_lgmax);
    LOGD_AHDR("%s:lrk  ro_hdrtmo_lghigh:%d:\n", __FUNCTION__, pAhdrCtx->CurrStatsData.tmo_stats.ro_hdrtmo_lghigh);
    LOGD_AHDR("%s:lrk  ro_hdrtmo_weightkey:%d:\n", __FUNCTION__, pAhdrCtx->CurrStatsData.tmo_stats.ro_hdrtmo_weightkey);
    LOGD_AHDR("%s:lrk  ro_hdrtmo_lgmean:%d:\n", __FUNCTION__, pAhdrCtx->CurrStatsData.tmo_stats.ro_hdrtmo_lgmean);
    LOGD_AHDR("%s:lrk  ro_hdrtmo_lgrange0:%d:\n", __FUNCTION__, pAhdrCtx->CurrStatsData.tmo_stats.ro_hdrtmo_lgrange0);
    LOGD_AHDR("%s:lrk  ro_hdrtmo_lgrange1:%d:\n", __FUNCTION__, pAhdrCtx->CurrStatsData.tmo_stats.ro_hdrtmo_lgrange1);
    LOGD_AHDR("%s:lrk  ro_hdrtmo_lgavgmax:%d:\n", __FUNCTION__, pAhdrCtx->CurrStatsData.tmo_stats.ro_hdrtmo_lgavgmax);
    LOGD_AHDR("%s:lrk  ro_hdrtmo_palpha:%d:\n", __FUNCTION__, pAhdrCtx->CurrStatsData.tmo_stats.ro_hdrtmo_palpha);
    LOGD_AHDR("%s:lrk  ro_hdrtmo_linecnt:%d:\n", __FUNCTION__, pAhdrCtx->CurrStatsData.tmo_stats.ro_hdrtmo_linecnt);
    for(int i = 0; i < 32; i++)
        LOGD_AHDR("%s:lrk  ro_array_min_max[%d]:%d:\n", __FUNCTION__, i, pAhdrCtx->CurrStatsData.tmo_stats.ro_array_min_max[i]);

#endif

    LOGI_AHDR( "%s:exit!\n", __FUNCTION__);
}

/******************************************************************************
 * AhdrGetXmlParas()
 *transfer html parameter into handle
 ***************************************************************************/
void AhdrGetXmlParas
(
    AhdrHandle_t           pAhdrCtx,
    const CamCalibDbContext_t*         pCalibDb
) {
    LOGI_AHDR( "%s:enter!\n", __FUNCTION__);

    // initial checks
    DCT_ASSERT(pAhdrCtx != NULL);
    DCT_ASSERT(pCalibDb != NULL);

    //merge
    pAhdrCtx->AhdrConfig.merge_para.OECurve_damp = LIMIT_VALUE(pCalibDb->ahdr.merge.oeCurve_damp, DAMPMAX, DAMPMIN);
    pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_damp = LIMIT_VALUE(pCalibDb->ahdr.merge.mdCurveLm_damp, DAMPMAX, DAMPMIN);
    pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_damp = LIMIT_VALUE(pCalibDb->ahdr.merge.mdCurveMs_damp, DAMPMAX, DAMPMIN);
    for (int i = 0; i < 6; i++ )
    {
        pAhdrCtx->AhdrConfig.merge_para.EnvLv[i] = LIMIT_VALUE(pCalibDb->ahdr.merge.envLevel[i], ENVLVMAX, ENVLVMIN);
        pAhdrCtx->AhdrConfig.merge_para.MoveCoef[i] = LIMIT_VALUE(pCalibDb->ahdr.merge.moveCoef[i], MOVECOEFMAX, MOVECOEFMIN);
        pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[i] = LIMIT_VALUE(pCalibDb->ahdr.merge.oeCurve_smooth[i], IQPARAMAX, IQPARAMIN);
        pAhdrCtx->AhdrConfig.merge_para.OECurve_offset[i] = LIMIT_VALUE(pCalibDb->ahdr.merge.oeCurve_offset[i], IQPARAMAX, IQPARAMIN);
        pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[i] = LIMIT_VALUE(pCalibDb->ahdr.merge.mdCurveLm_smooth[i], IQPARAMAX, IQPARAMIN);
        pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[i] = LIMIT_VALUE(pCalibDb->ahdr.merge.mdCurveLm_offset[i], IQPARAMAX, IQPARAMIN);
        pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[i] = LIMIT_VALUE(pCalibDb->ahdr.merge.mdCurveMs_smooth[i], IQPARAMAX, IQPARAMIN);
        pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[i] = LIMIT_VALUE(pCalibDb->ahdr.merge.mdCurveMs_offset[i], IQPARAMAX, IQPARAMIN);
    }

    //TMO
    pAhdrCtx->AhdrConfig.tmo_para.enFocusLuma = LIMIT_VALUE(pCalibDb->ahdr.tmo.enFocusLuma, ENMAX, ENMIN);
    pAhdrCtx->AhdrConfig.tmo_para.DayTh = LIMIT_VALUE(pCalibDb->ahdr.tmo.DayTh, DARKPDFTHMAX, DARKPDFTHMIN);
    pAhdrCtx->AhdrConfig.tmo_para.damp = LIMIT_VALUE(pCalibDb->ahdr.tmo.damp, DAMPMAX, DAMPMIN);
    pAhdrCtx->AhdrConfig.tmo_para.EnvLvTolerance = LIMIT_VALUE(pCalibDb->ahdr.tmo.EnvLvTolerance, TOLERANCEMAX, TOLERANCEMIN);
    pAhdrCtx->AhdrConfig.tmo_para.OETolerance = LIMIT_VALUE(pCalibDb->ahdr.tmo.OETolerance, TOLERANCEMAX, TOLERANCEMIN);
    pAhdrCtx->AhdrConfig.tmo_para.DTPdfTolerance = LIMIT_VALUE(pCalibDb->ahdr.tmo.DTPdfTolerance, TOLERANCEMAX, TOLERANCEMIN);
    pAhdrCtx->AhdrConfig.tmo_para.DRTolerance = LIMIT_VALUE(pCalibDb->ahdr.tmo.DRTolerance, TOLERANCEMAX, TOLERANCEMIN);
    for(int i = 0; i < 6; i++)
    {
        pAhdrCtx->AhdrConfig.tmo_para.EnvLv[i] = LIMIT_VALUE(pCalibDb->ahdr.tmo.envLevel[i], ENVLVMAX, ENVLVMIN);
        pAhdrCtx->AhdrConfig.tmo_para.GlobeLuma[i] = LIMIT_VALUE(pCalibDb->ahdr.tmo.globalLuma[i], IQPARAMAX, IQPARAMIN) ;

        pAhdrCtx->AhdrConfig.tmo_para.OEPdf[i] = LIMIT_VALUE(pCalibDb->ahdr.tmo.OEPdf[i], OEPDFMAX, OEPDFMIN) ;
        pAhdrCtx->AhdrConfig.tmo_para.DetailsHighLight[i] = LIMIT_VALUE(pCalibDb->ahdr.tmo.detailsHighLight[i], IQPARAMAX, IQPARAMIN) ;

        pAhdrCtx->AhdrConfig.tmo_para.FocusLuma[i] = LIMIT_VALUE(pCalibDb->ahdr.tmo.FocusLuma[i], FOCUSLUMAMAX, FOCUSLUMAMIN) ;
        pAhdrCtx->AhdrConfig.tmo_para.DarkPdf[i] = LIMIT_VALUE(pCalibDb->ahdr.tmo.DarkPdf[i], DARKPDFMAX, DARKPDFMIN) ;
        pAhdrCtx->AhdrConfig.tmo_para.DetailsLowLight[i] = LIMIT_VALUE(pCalibDb->ahdr.tmo.detailsLowLight[i], IQDETAILSLOWLIGHTMAX, IQDETAILSLOWLIGHTMIN) ;

        pAhdrCtx->AhdrConfig.tmo_para.DynamicRange[i] = LIMIT_VALUE(pCalibDb->ahdr.tmo.DynamicRange[i], DYNAMICRANGEMAX, DYNAMICRANGEMIN) ;
        pAhdrCtx->AhdrConfig.tmo_para.TmoContrast[i] = LIMIT_VALUE(pCalibDb->ahdr.tmo.TmoContrast[i], IQPARAMAX, IQPARAMIN) ;
    }

#if LRK_DEBUG_LOG
    LOGD_AHDR("%s:lrk  Ahdr comfig data from xml:\n", __FUNCTION__);
    LOGD_AHDR("%s:lrk  Merge MergeMode:%d:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.MergeMode);
    LOGD_AHDR("%s:lrk  Merge EnvLv:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.EnvLv[0], pAhdrCtx->AhdrConfig.merge_para.EnvLv[1], pAhdrCtx->AhdrConfig.merge_para.EnvLv[2],
              pAhdrCtx->AhdrConfig.merge_para.EnvLv[3], pAhdrCtx->AhdrConfig.merge_para.EnvLv[4], pAhdrCtx->AhdrConfig.merge_para.EnvLv[5]);
    LOGD_AHDR("%s:lrk  Merge OECurve_smooth:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[0], pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[1], pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[2],
              pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[3], pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[4], pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[5]);
    LOGD_AHDR("%s:lrk  Merge OECurve_offset:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.OECurve_offset[0], pAhdrCtx->AhdrConfig.merge_para.OECurve_offset[1], pAhdrCtx->AhdrConfig.merge_para.OECurve_offset[2],
              pAhdrCtx->AhdrConfig.merge_para.OECurve_offset[3], pAhdrCtx->AhdrConfig.merge_para.OECurve_offset[4], pAhdrCtx->AhdrConfig.merge_para.OECurve_offset[5]);
    LOGD_AHDR("%s:lrk  Merge MoveCoef:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.MoveCoef[0], pAhdrCtx->AhdrConfig.merge_para.MoveCoef[1], pAhdrCtx->AhdrConfig.merge_para.MoveCoef[2],
              pAhdrCtx->AhdrConfig.merge_para.MoveCoef[3], pAhdrCtx->AhdrConfig.merge_para.MoveCoef[4], pAhdrCtx->AhdrConfig.merge_para.MoveCoef[5]);
    LOGD_AHDR("%s:lrk  Merge MDCurveLM_smooth:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[0], pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[1], pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[2],
              pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[3], pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[4], pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[5]);
    LOGD_AHDR("%s:lrk  Merge MDCurveLM_offset:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[0], pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[1], pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[2],
              pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[3], pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[4], pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[5]);
    LOGD_AHDR("%s:lrk  Merge MDCurveMS_smooth:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[0], pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[1], pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[2],
              pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[3], pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[4], pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[5]);
    LOGD_AHDR("%s:lrk  Merge MDCurveMS_offset:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[0], pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[1], pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[2],
              pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[3], pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[4], pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[5]);
    LOGD_AHDR("%s:lrk  Merge OECurve_damp:%f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.OECurve_damp);
    LOGD_AHDR("%s:lrk  Merge MDCurveLM_damp:%f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_damp);
    LOGD_AHDR("%s:lrk  Merge MDCurveMS_damp:%f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_damp);

    LOGD_AHDR("%s:lrk  Tmo EnvLv:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.EnvLv[0], pAhdrCtx->AhdrConfig.tmo_para.EnvLv[1], pAhdrCtx->AhdrConfig.tmo_para.EnvLv[2]
              , pAhdrCtx->AhdrConfig.tmo_para.EnvLv[3], pAhdrCtx->AhdrConfig.tmo_para.EnvLv[4], pAhdrCtx->AhdrConfig.tmo_para.EnvLv[5]);
    LOGD_AHDR("%s:lrk  Tmo EnvLvTolerance:%f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.EnvLvTolerance);
    LOGD_AHDR("%s:lrk  Tmo GlobeLuma:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.GlobeLuma[0], pAhdrCtx->AhdrConfig.tmo_para.GlobeLuma[1], pAhdrCtx->AhdrConfig.tmo_para.GlobeLuma[2]
              , pAhdrCtx->AhdrConfig.tmo_para.GlobeLuma[3], pAhdrCtx->AhdrConfig.tmo_para.GlobeLuma[4], pAhdrCtx->AhdrConfig.tmo_para.GlobeLuma[5]);
    LOGD_AHDR("%s:lrk  Tmo OEPdf:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.OEPdf[0], pAhdrCtx->AhdrConfig.tmo_para.OEPdf[1], pAhdrCtx->AhdrConfig.tmo_para.OEPdf[2]
              , pAhdrCtx->AhdrConfig.tmo_para.OEPdf[3], pAhdrCtx->AhdrConfig.tmo_para.OEPdf[4], pAhdrCtx->AhdrConfig.tmo_para.OEPdf[5]);
    LOGD_AHDR("%s:lrk  Tmo OETolerance:%f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.OETolerance);
    LOGD_AHDR("%s:lrk  Tmo DetailsHighLight:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.DetailsHighLight[0], pAhdrCtx->AhdrConfig.tmo_para.DetailsHighLight[1], pAhdrCtx->AhdrConfig.tmo_para.DetailsHighLight[2]
              , pAhdrCtx->AhdrConfig.tmo_para.DetailsHighLight[3], pAhdrCtx->AhdrConfig.tmo_para.DetailsHighLight[4], pAhdrCtx->AhdrConfig.tmo_para.DetailsHighLight[5]);
    LOGD_AHDR("%s:lrk  Tmo enFocusLuma:%f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.enFocusLuma);
    LOGD_AHDR("%s:lrk  Tmo FocusLuma:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.FocusLuma[0], pAhdrCtx->AhdrConfig.tmo_para.FocusLuma[1], pAhdrCtx->AhdrConfig.tmo_para.FocusLuma[2]
              , pAhdrCtx->AhdrConfig.tmo_para.FocusLuma[3], pAhdrCtx->AhdrConfig.tmo_para.FocusLuma[4], pAhdrCtx->AhdrConfig.tmo_para.FocusLuma[5]);
    LOGD_AHDR("%s:lrk  Tmo DarkPdf:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.DarkPdf[0], pAhdrCtx->AhdrConfig.tmo_para.DarkPdf[1], pAhdrCtx->AhdrConfig.tmo_para.DarkPdf[2]
              , pAhdrCtx->AhdrConfig.tmo_para.DarkPdf[3], pAhdrCtx->AhdrConfig.tmo_para.DarkPdf[4], pAhdrCtx->AhdrConfig.tmo_para.DarkPdf[5]);
    LOGD_AHDR("%s:lrk  Tmo DTPdfTolerance:%f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.DTPdfTolerance);
    LOGD_AHDR("%s:lrk  Tmo DetailsLowLight:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.DetailsLowLight[0], pAhdrCtx->AhdrConfig.tmo_para.DetailsLowLight[1], pAhdrCtx->AhdrConfig.tmo_para.DetailsLowLight[2]
              , pAhdrCtx->AhdrConfig.tmo_para.DetailsLowLight[3], pAhdrCtx->AhdrConfig.tmo_para.DetailsLowLight[4], pAhdrCtx->AhdrConfig.tmo_para.DetailsLowLight[5]);
    LOGD_AHDR("%s:lrk  Tmo DynamicRange:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.DynamicRange[0], pAhdrCtx->AhdrConfig.tmo_para.DynamicRange[1], pAhdrCtx->AhdrConfig.tmo_para.DynamicRange[2]
              , pAhdrCtx->AhdrConfig.tmo_para.DynamicRange[3], pAhdrCtx->AhdrConfig.tmo_para.DynamicRange[4], pAhdrCtx->AhdrConfig.tmo_para.DynamicRange[5]);
    LOGD_AHDR("%s:lrk  Tmo DRTolerance:%f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.DRTolerance);
    LOGD_AHDR("%s:lrk  Tmo DayTh:%f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.DayTh);
    LOGD_AHDR("%s:lrk  Tmo TmoContrast:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.TmoContrast[0], pAhdrCtx->AhdrConfig.tmo_para.TmoContrast[1], pAhdrCtx->AhdrConfig.tmo_para.TmoContrast[2]
              , pAhdrCtx->AhdrConfig.tmo_para.TmoContrast[3], pAhdrCtx->AhdrConfig.tmo_para.TmoContrast[4], pAhdrCtx->AhdrConfig.tmo_para.TmoContrast[5]);
    LOGD_AHDR("%s:lrk  Tmo Damp:%f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.damp);
#endif

    //turn the IQ paras into algo paras
    for(int i = 0; i < 6; i++)
    {
        pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[i] = pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[i] * OECURVESMOOTHMAX;
        pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[i] = LIMIT_VALUE(pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[i], OECURVESMOOTHMAX, OECURVESMOOTHMIN) ;
        pAhdrCtx->AhdrConfig.merge_para.OECurve_offset[i] = pAhdrCtx->AhdrConfig.merge_para.OECurve_offset[i] * OECURVEOFFSETMAX;
        pAhdrCtx->AhdrConfig.merge_para.OECurve_offset[i] = LIMIT_VALUE(pAhdrCtx->AhdrConfig.merge_para.OECurve_offset[i], OECURVEOFFSETMAX, OECURVEOFFSETMIN) ;

        pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[i] = pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[i] * MDCURVESMOOTHMAX;
        pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[i] = LIMIT_VALUE(pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[i], MDCURVESMOOTHMAX, MDCURVESMOOTHMIN) ;
        pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[i] = pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[i] * MDCURVEOFFSETMAX;
        pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[i] = LIMIT_VALUE(pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[i], MDCURVEOFFSETMAX, MDCURVEOFFSETMIN) ;

        pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[i] = pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[i] * MDCURVESMOOTHMAX;
        pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[i] = LIMIT_VALUE(pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[i], MDCURVESMOOTHMAX, MDCURVESMOOTHMIN) ;
        pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[i] = pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[i] * MDCURVEOFFSETMAX;
        pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[i] = LIMIT_VALUE(pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[i], MDCURVEOFFSETMAX, MDCURVEOFFSETMIN) ;

        pAhdrCtx->AhdrConfig.tmo_para.GlobeLuma[i] = pAhdrCtx->AhdrConfig.tmo_para.GlobeLuma[i] * GLOBELUMAMAX;
        pAhdrCtx->AhdrConfig.tmo_para.GlobeLuma[i] = LIMIT_VALUE(pAhdrCtx->AhdrConfig.tmo_para.GlobeLuma[i], GLOBELUMAMAX, GLOBELUMAMIN) ;

        pAhdrCtx->AhdrConfig.tmo_para.DetailsHighLight[i] = pAhdrCtx->AhdrConfig.tmo_para.DetailsHighLight[i] * DETAILSHIGHLIGHTMAX;
        pAhdrCtx->AhdrConfig.tmo_para.DetailsHighLight[i] = LIMIT_VALUE(pAhdrCtx->AhdrConfig.tmo_para.DetailsHighLight[i], DETAILSHIGHLIGHTMAX, DETAILSHIGHLIGHTMIN) ;

        pAhdrCtx->AhdrConfig.tmo_para.DetailsLowLight[i] = pAhdrCtx->AhdrConfig.tmo_para.DetailsLowLight[i] * DETAILSLOWLIGHTMIN;
        pAhdrCtx->AhdrConfig.tmo_para.DetailsLowLight[i] = LIMIT_VALUE(pAhdrCtx->AhdrConfig.tmo_para.DetailsLowLight[i], DETAILSLOWLIGHTMAX, DETAILSLOWLIGHTMIN) ;

        pAhdrCtx->AhdrConfig.tmo_para.TmoContrast[i] = pAhdrCtx->AhdrConfig.tmo_para.TmoContrast[i] * TMOCONTRASTMAX;
        pAhdrCtx->AhdrConfig.tmo_para.TmoContrast[i] = LIMIT_VALUE(pAhdrCtx->AhdrConfig.tmo_para.TmoContrast[i], TMOCONTRASTMAX, TMOCONTRASTMIN) ;

    }


#if LRK_DEBUG_LOG
    LOGD_AHDR("%s:lrk  Merge algo OECurve_smooth:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[0], pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[1], pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[2],
              pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[3], pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[4], pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[5]);
    LOGD_AHDR("%s:lrk  Merge algo OECurve_offset:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.OECurve_offset[0], pAhdrCtx->AhdrConfig.merge_para.OECurve_offset[1], pAhdrCtx->AhdrConfig.merge_para.OECurve_offset[2],
              pAhdrCtx->AhdrConfig.merge_para.OECurve_offset[3], pAhdrCtx->AhdrConfig.merge_para.OECurve_offset[4], pAhdrCtx->AhdrConfig.merge_para.OECurve_offset[5]);
    LOGD_AHDR("%s:lrk  Merge algo MDCurveLM_smooth:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[0], pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[1], pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[2],
              pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[3], pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[4], pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[5]);
    LOGD_AHDR("%s:lrk  Merge algo MDCurveLM_offset:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[0], pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[1], pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[2],
              pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[3], pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[4], pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[5]);
    LOGD_AHDR("%s:lrk  Merge algo MDCurveMS_smooth:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[0], pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[1], pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[2],
              pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[3], pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[4], pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[5]);
    LOGD_AHDR("%s:lrk  Merge algo MDCurveMS_offset:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[0], pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[1], pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[2],
              pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[3], pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[4], pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[5]);
    LOGD_AHDR("%s:lrk  Tmo algo GlobeLuma:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.GlobeLuma[0], pAhdrCtx->AhdrConfig.tmo_para.GlobeLuma[1], pAhdrCtx->AhdrConfig.tmo_para.GlobeLuma[2]
              , pAhdrCtx->AhdrConfig.tmo_para.GlobeLuma[3], pAhdrCtx->AhdrConfig.tmo_para.GlobeLuma[4], pAhdrCtx->AhdrConfig.tmo_para.GlobeLuma[5]);
    LOGD_AHDR("%s:lrk  Tmo algo DetailsHighLight:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.DetailsHighLight[0], pAhdrCtx->AhdrConfig.tmo_para.DetailsHighLight[1], pAhdrCtx->AhdrConfig.tmo_para.DetailsHighLight[2]
              , pAhdrCtx->AhdrConfig.tmo_para.DetailsHighLight[3], pAhdrCtx->AhdrConfig.tmo_para.DetailsHighLight[4], pAhdrCtx->AhdrConfig.tmo_para.DetailsHighLight[5]);
    LOGD_AHDR("%s:lrk  Tmo algo DetailsLowLight:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.DetailsLowLight[0], pAhdrCtx->AhdrConfig.tmo_para.DetailsLowLight[1], pAhdrCtx->AhdrConfig.tmo_para.DetailsLowLight[2]
              , pAhdrCtx->AhdrConfig.tmo_para.DetailsLowLight[3], pAhdrCtx->AhdrConfig.tmo_para.DetailsLowLight[4], pAhdrCtx->AhdrConfig.tmo_para.DetailsLowLight[5]);
    LOGD_AHDR("%s:lrk  Tmo algo TmoContrast:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.TmoContrast[0], pAhdrCtx->AhdrConfig.tmo_para.TmoContrast[1], pAhdrCtx->AhdrConfig.tmo_para.TmoContrast[2]
              , pAhdrCtx->AhdrConfig.tmo_para.TmoContrast[3], pAhdrCtx->AhdrConfig.tmo_para.TmoContrast[4], pAhdrCtx->AhdrConfig.tmo_para.TmoContrast[5]);
#endif

    LOGI_AHDR( "%s:exit!\n", __FUNCTION__);
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
    LOGI_AHDR("%s:enter!\n", __FUNCTION__);

    //get current ae data from AecPreRes
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

    //Normalize the current envLv for AEC
    float maxEnvLuma = 65 / (1 * 0.01);
    float minEnvLuma = 65 / (16 * 0.03);
    pAhdrCtx->CurrAeResult.GlobalEnvLv = (pAhdrCtx->CurrAeResult.GlobalEnvLv - minEnvLuma) / maxEnvLuma;
    pAhdrCtx->CurrAeResult.GlobalEnvLv = LIMIT_VALUE(pAhdrCtx->CurrAeResult.GlobalEnvLv, 1, 0);
    pAhdrCtx->CurrAeResult.GlobalEnvLv = 1 - pAhdrCtx->CurrAeResult.GlobalEnvLv;

    //transfer CurrAeResult data into AhdrHandle
    //get Curren hdr mode
    pAhdrCtx->CurrHandleData.MergeMode = pAhdrCtx->hdr_mode - 1;

    switch (pAhdrCtx->CurrHandleData.MergeMode)
    {
    case 1:
        pAhdrCtx->CurrAeResult.L2M_Ratio = 1;
        pAhdrCtx->CurrHandleData.CurrL2S_Ratio = pAhdrCtx->CurrAeResult.M2S_Ratio;
        pAhdrCtx->CurrHandleData.CurrL2M_Ratio = pAhdrCtx->CurrAeResult.L2M_Ratio;
        pAhdrCtx->CurrHandleData.CurrL2L_Ratio = 1;
        pAhdrCtx->CurrHandleData.CurrLExpo = AecHdrPreResult.HdrExp[1].exp_real_params.analog_gain * AecHdrPreResult.HdrExp[1].exp_real_params.integration_time;
        break;
    case 2:
        pAhdrCtx->CurrHandleData.CurrL2S_Ratio = pAhdrCtx->CurrAeResult.L2M_Ratio * pAhdrCtx->CurrAeResult.M2S_Ratio;
        pAhdrCtx->CurrHandleData.CurrL2M_Ratio = pAhdrCtx->CurrAeResult.L2M_Ratio;
        pAhdrCtx->CurrHandleData.CurrL2L_Ratio = 1;
        pAhdrCtx->CurrHandleData.CurrLExpo = AecHdrPreResult.HdrExp[2].exp_real_params.analog_gain * AecHdrPreResult.HdrExp[2].exp_real_params.integration_time;
        break;
    default:
        LOGE_AHDR("%s:lrk  Wrong frame number in HDR mode!!!\n", __FUNCTION__);
        break;
    }

    //get Current merge OECurve
    pAhdrCtx->CurrHandleData.CurrEnvLv = pAhdrCtx->CurrAeResult.GlobalEnvLv;
    pAhdrCtx->CurrHandleData.CurrEnvLv = LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrEnvLv, ENVLVMAX, ENVLVMIN);
    pAhdrCtx->CurrHandleData.CurrMergeHandleData.OECurve_smooth = GetCurrPara(pAhdrCtx->CurrHandleData.CurrEnvLv,
            pAhdrCtx->AhdrConfig.merge_para.EnvLv,
            pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth);
    pAhdrCtx->CurrHandleData.CurrMergeHandleData.OECurve_offset = GetCurrPara(pAhdrCtx->CurrHandleData.CurrEnvLv,
            pAhdrCtx->AhdrConfig.merge_para.EnvLv,
            pAhdrCtx->AhdrConfig.merge_para.OECurve_offset);
    //get Current merge MDCurve
    pAhdrCtx->CurrHandleData.CurrMoveCoef = 1;
    pAhdrCtx->CurrHandleData.CurrMoveCoef = LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrMoveCoef, MOVECOEFMAX, MOVECOEFMIN);
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

    //get Current tmo GlobeLuma GlobeMaxLuma
    pAhdrCtx->CurrHandleData.CurrEnvLv = pAhdrCtx->CurrAeResult.GlobalEnvLv;
    pAhdrCtx->CurrHandleData.CurrEnvLv = LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrEnvLv, ENVLVMAX, ENVLVMIN);
    pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeLuma = GetCurrPara(pAhdrCtx->CurrHandleData.CurrEnvLv,
            pAhdrCtx->AhdrConfig.tmo_para.EnvLv,
            pAhdrCtx->AhdrConfig.tmo_para.GlobeLuma);
    float GlobeLuma = pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeLuma;
    pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeMaxLuma = MAXLUMAK * GlobeLuma + MAXLUMAB;
    pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeMaxLuma = LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeMaxLuma, GLOBEMAXLUMAMAX, GLOBEMAXLUMAMIN);

    //get Current tmo TmoContrast
    pAhdrCtx->CurrHandleData.DayTh = pAhdrCtx->AhdrConfig.tmo_para.DayTh;
    if(pAhdrCtx->CurrHandleData.CurrEnvLv > pAhdrCtx->CurrHandleData.DayTh)
    {
        pAhdrCtx->CurrHandleData.CurrDynamicRange = pAhdrCtx->CurrAeResult.DynamicRange;
        pAhdrCtx->CurrHandleData.CurrDynamicRange = LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrDynamicRange, DYNAMICRANGEMAX, DYNAMICRANGEMIN);
        pAhdrCtx->CurrHandleData.CurrTmoHandleData.TmoContrast = GetCurrPara(pAhdrCtx->CurrHandleData.CurrDynamicRange,
                pAhdrCtx->AhdrConfig.tmo_para.DynamicRange,
                pAhdrCtx->AhdrConfig.tmo_para.TmoContrast);
    }
    else
        pAhdrCtx->CurrHandleData.CurrTmoHandleData.TmoContrast = 76;

    //get Current tmo DetailsHighLight
    pAhdrCtx->CurrHandleData.CurrOEPdf = pAhdrCtx->CurrAeResult.OEPdf;
    pAhdrCtx->CurrHandleData.CurrOEPdf = LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrOEPdf, OEPDFMAX, OEPDFMIN);
    pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsHighLight = GetCurrPara(pAhdrCtx->CurrHandleData.CurrOEPdf,
            pAhdrCtx->AhdrConfig.tmo_para.OEPdf,
            pAhdrCtx->AhdrConfig.tmo_para.DetailsHighLight);

    //get Current tmo DetailsLowLight
    if (pAhdrCtx->AhdrConfig.tmo_para.enFocusLuma == 1)
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
                pAhdrCtx->AhdrConfig.tmo_para.FocusLuma,
                pAhdrCtx->AhdrConfig.tmo_para.DetailsLowLight);

#endif
    }
    else
    {
        pAhdrCtx->CurrHandleData.CurrDarkPdf = pAhdrCtx->CurrAeResult.DarkPdf;
        pAhdrCtx->CurrHandleData.CurrDarkPdf = LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrDarkPdf, 0.5, 0);
        pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsLowLight = GetCurrPara(pAhdrCtx->CurrHandleData.CurrDarkPdf,
                pAhdrCtx->AhdrConfig.tmo_para.DarkPdf,
                pAhdrCtx->AhdrConfig.tmo_para.DetailsLowLight);
    }

    //get Current tmo TmoDamp
    pAhdrCtx->CurrHandleData.TmoDamp = pAhdrCtx->AhdrConfig.tmo_para.damp;

    //read curren rodata
    int data[16];
    data[0] = pAhdrCtx->CurrStatsData.tmo_stats.ro_hdrtmo_lgmean;
    for(int j = 0; j < 15; j++)
        data[j + 1] = pAhdrCtx->AhdrPrevData.ro_hdrtmo_lgmean_all[j];
    for(int k = 0; k < 16; k++)
        pAhdrCtx->AhdrPrevData.ro_hdrtmo_lgmean_all[k] = data[k];

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


#if LRK_DEBUG_LOG
    LOGD_AHDR("%s:lrk  Ahdr Current frame cnt:%d:\n",  __FUNCTION__, pAhdrCtx->frameCnt);
    LOGD_AHDR("%s:lrk  Ahdr Current Handle data:\n", __FUNCTION__);
    LOGD_AHDR("%s:lrk  Current MergeMode:         %d \n", __FUNCTION__, pAhdrCtx->CurrHandleData.MergeMode);
    LOGD_AHDR("%s:lrk  Current CurrL2S_Ratio:     %f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrL2S_Ratio);
    LOGD_AHDR("%s:lrk  Current CurrL2M_Ratio:     %f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrL2M_Ratio);
    LOGD_AHDR("%s:lrk  Current CurrL2L_Ratio:     %f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrL2L_Ratio);
    LOGD_AHDR("%s:lrk  Current CurrLExpo:         %f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrLExpo);
    LOGD_AHDR("%s:lrk  Current CurrEnvLv:         %f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrEnvLv);
    LOGD_AHDR("%s:lrk  Current CurrTotalFocusLuma:%f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrTotalFocusLuma);
    LOGD_AHDR("%s:lrk  Current CurrMoveCoef:      %f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrMoveCoef);
    LOGD_AHDR("%s:lrk  Current CurrDynamicRange:  %f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrDynamicRange);
    LOGD_AHDR("%s:lrk  Current CurrOEPdf:         %f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrOEPdf);
    LOGD_AHDR("%s:lrk  Current CurrDarkPdf:       %f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrDarkPdf);
    LOGD_AHDR("%s:lrk  Current CurrTotalFocusLuma:%f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrTotalFocusLuma);
    LOGD_AHDR("%s:lrk  Current OECurve_smooth:    %f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrMergeHandleData.OECurve_smooth);
    LOGD_AHDR("%s:lrk  Current OECurve_offset:    %f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrMergeHandleData.OECurve_offset);
    LOGD_AHDR("%s:lrk  Current MDCurveLM_smooth:  %f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveLM_smooth);
    LOGD_AHDR("%s:lrk  Current MDCurveLM_offset:  %f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveLM_offset);
    LOGD_AHDR("%s:lrk  Current MDCurveMS_smooth:  %f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveMS_smooth);
    LOGD_AHDR("%s:lrk  Current MDCurveMS_offset:  %f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveMS_offset);
    LOGD_AHDR("%s:lrk  Current GlobeLuma:         %f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeLuma);
    LOGD_AHDR("%s:lrk  Current GlobeMaxLuma:      %f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeMaxLuma);
    LOGD_AHDR("%s:lrk  Current DetailsHighLight:  %f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsHighLight);
    LOGD_AHDR("%s:lrk  Current DetailsLowLight:   %f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsLowLight);
    LOGD_AHDR("%s:lrk  Current TmoContrast:%f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrTmoHandleData.TmoContrast);
    LOGD_AHDR("%s:lrk  preFrame lg Merge mean Luma:%f \n", __FUNCTION__, lgmean);
    LOGD_AHDR("%s:lrk  preFrame Merge mean Luma(8bit):%f \n", __FUNCTION__, MergeLuma);
    LOGD_AHDR("%s:lrk  preFrame tmo mean Luma(8bit):%d \n", __FUNCTION__, tmo_mean);
    LOGD_AHDR("%s:lrk  preFrame short mean Luma(8bit):%d \n", __FUNCTION__, short_mean);
    LOGD_AHDR("%s:lrk  preFrame middle mean Luma(8bit):%d \n", __FUNCTION__, middle_mean);
    LOGD_AHDR("%s:lrk  preFrame long mean Luma(8bit):%d \n", __FUNCTION__, long_mean);
#endif

    LOGI_AHDR( "%s:exit!\n", __FUNCTION__);
}

/******************************************************************************
 * AhdrInit()
 *****************************************************************************/
RESULT AhdrInit
(
    AhdrInstanceConfig_t* pInstConfig
) {

    AhdrContext_s *pAhdrCtx;

    LOGI_AHDR("%s:enter!\n", __FUNCTION__);

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

    LOGI_AHDR( "%s:exit!\n", __FUNCTION__);

    return (AHDR_RET_SUCCESS);
}
/******************************************************************************
 * AhdrRelease()
 *****************************************************************************/
RESULT AhdrRelease
(
    AhdrHandle_t pAhdrCtx
) {

    LOGI_AHDR( "%s:enter!\n", __FUNCTION__);
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

    LOGI_AHDR( "%s:exit!\n", __FUNCTION__);

    return (AHDR_RET_SUCCESS);
}
/******************************************************************************
 * AhdrRelease()
 *****************************************************************************/
void AhdrProcessing
(
    AhdrHandle_t pAhdrCtx
) {

    LOGI_AHDR( "%s:enter!\n", __FUNCTION__);

    MergeProcessing(pAhdrCtx);
    TmoProcessing(pAhdrCtx);

    // store current handle data to pre data for next loop
    pAhdrCtx->AhdrPrevData.PreL2S_ratio = pAhdrCtx->CurrHandleData.CurrL2S_Ratio;
    pAhdrCtx->AhdrPrevData.PreLExpo = pAhdrCtx->CurrHandleData.CurrLExpo;
    pAhdrCtx->AhdrPrevData.PreEnvlv = pAhdrCtx->CurrHandleData.CurrEnvLv;
    pAhdrCtx->AhdrPrevData.PreMoveCoef = pAhdrCtx->CurrHandleData.CurrMoveCoef;
    pAhdrCtx->AhdrPrevData.PreOEPdf = pAhdrCtx->CurrHandleData.CurrOEPdf;
    pAhdrCtx->AhdrPrevData.PreTotalFocusLuma = pAhdrCtx->CurrHandleData.CurrTotalFocusLuma;
    pAhdrCtx->AhdrPrevData.PreDarkPdf = pAhdrCtx->CurrHandleData.CurrDarkPdf;
    pAhdrCtx->AhdrPrevData.PreDynamicRange = pAhdrCtx->CurrHandleData.CurrDynamicRange;
    memcpy(&pAhdrCtx->AhdrPrevData.PrevMergeHandleData, &pAhdrCtx->CurrHandleData.CurrMergeHandleData, sizeof(MergeHandleData_s));
    memcpy(&pAhdrCtx->AhdrPrevData.PrevTmoHandleData, &pAhdrCtx->CurrHandleData.CurrTmoHandleData, sizeof(TmoHandleData_s));
    ++pAhdrCtx->frameCnt;

    LOGI_AHDR( "%s:exit!\n", __FUNCTION__);
}
