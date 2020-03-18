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

    //merge
#ifndef RK_SIMULATOR_HW
    pAhdrCtx->AhdrIOData.MergeData.sw_hdrmge_mode = 2;
#else
    pAhdrCtx->AhdrIOData.MergeData.sw_hdrmge_mode = 1;;
#endif
    pAhdrCtx->AhdrIOData.MergeData.sw_hdrmge_gain0 = 1;
    pAhdrCtx->AhdrIOData.MergeData.sw_hdrmge_gain0_inv = 4095;
    pAhdrCtx->AhdrIOData.MergeData.sw_hdrmge_gain1 = 1;
    pAhdrCtx->AhdrIOData.MergeData.sw_hdrmge_gain1_inv = 4095;
    pAhdrCtx->AhdrIOData.MergeData.sw_hdrmge_gain2 = 1;
    pAhdrCtx->AhdrIOData.MergeData.sw_hdrmge_ms_dif_0p8 = 204;
    pAhdrCtx->AhdrIOData.MergeData.sw_hdrmge_lm_dif_0p9 = 230;
    pAhdrCtx->AhdrIOData.MergeData.sw_hdrmge_ms_dif_0p15 = 38;
    pAhdrCtx->AhdrIOData.MergeData.sw_hdrmge_lm_dif_0p15 = 38;
    int curveOE[17] = {0, 0, 0, 0, 0, 0, 2, 5, 18, 62, 187, 448, 749, 926, 994, 1015, 1023};
    int curveMDLM[17] = {0, 0, 0, 0, 1, 3, 11, 38, 122, 329, 637, 872, 975, 1010, 1020, 1023, 1023};
    int curveMDMS[17] = {0, 0, 0, 0, 1, 3, 11, 38, 122, 329, 637, 872, 975, 1010, 1020, 1023, 1023};
    for(int i = 0; i < 17; i++)
    {
        pAhdrCtx->AhdrIOData.MergeData.sw_hdrmge_e_y[i] = curveOE[i];
        pAhdrCtx->AhdrIOData.MergeData.sw_hdrmge_l0_y[i] = curveMDLM[i];
        pAhdrCtx->AhdrIOData.MergeData.sw_hdrmge_l1_y[i] = curveMDLM[i];
    }

    //tmo
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_clipratio0 = 64;
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_clipratio1 = 166;
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_clipgap0 = 12;
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_clipgap1 = 12;
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_ratiol = 32;
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_hist_min = 0;
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_hist_low = 1296;
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_hist_high = 130;
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_hist_0p3 = 0;
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_hist_shift = 3;
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_palpha_0p18 = 184;
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_palpha_lw0p5 = 512;
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_palpha_lwscl = 16;
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_maxpalpha = 307;
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_maxgain = 4096;
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_cfg_alpha = 0;
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_set_gainoff = 1024;
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_set_lgmin = 20480;
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_set_lgmax = 32153;
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_set_lgmean = 30105;
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_set_weightkey = 76;
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_set_lgrange0 = 23398;
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_set_lgrange1 = 29234;
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_set_lgavgmax = 29234;
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_set_palpha = 307;
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_gain_ld_off1 = 10;
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_gain_ld_off2 = 5;

    pAhdrCtx->frameCnt = 0;

#if LRK_DEBUG_LOG
    LOGE_AHDR("%s:lrk  Merge set IOdata to register:\n", __FUNCTION__);
    LOGE_AHDR("%s:lrk  sw_hdrmge_mode:%d \n", __FUNCTION__, pAhdrCtx->AhdrIOData.MergeData.sw_hdrmge_mode);
    LOGE_AHDR("%s:lrk  sw_hdrmge_ms_dif_0p8:%d \n", __FUNCTION__, pAhdrCtx->AhdrIOData.MergeData.sw_hdrmge_ms_dif_0p8);
    LOGE_AHDR("%s:lrk  sw_hdrmge_lm_dif_0p9:%d \n", __FUNCTION__, pAhdrCtx->AhdrIOData.MergeData.sw_hdrmge_lm_dif_0p9);
    LOGE_AHDR("%s:lrk  sw_hdrmge_ms_dif_0p15:%d \n", __FUNCTION__, pAhdrCtx->AhdrIOData.MergeData.sw_hdrmge_ms_dif_0p15);
    LOGE_AHDR("%s:lrk  sw_hdrmge_lm_dif_0p15:%d \n", __FUNCTION__, pAhdrCtx->AhdrIOData.MergeData.sw_hdrmge_lm_dif_0p15);
    LOGE_AHDR("%s:lrk  sw_hdrmge_gain0:%d \n", __FUNCTION__, pAhdrCtx->AhdrIOData.MergeData.sw_hdrmge_gain0);
    LOGE_AHDR("%s:lrk  sw_hdrmge_gain0_inv:%d \n", __FUNCTION__, pAhdrCtx->AhdrIOData.MergeData.sw_hdrmge_gain0_inv);
    LOGE_AHDR("%s:lrk  sw_hdrmge_gain1:%d \n", __FUNCTION__, pAhdrCtx->AhdrIOData.MergeData.sw_hdrmge_gain1);
    LOGE_AHDR("%s:lrk  sw_hdrmge_gain1_inv:%d \n", __FUNCTION__, pAhdrCtx->AhdrIOData.MergeData.sw_hdrmge_gain1_inv);
    LOGE_AHDR("%s:lrk  sw_hdrmge_gain2:%d \n", __FUNCTION__, pAhdrCtx->AhdrIOData.MergeData.sw_hdrmge_gain2);
    for(int i = 0; i < 17; i++)
        LOGE_AHDR("%s:lrk  sw_hdrmge_e_y[%d]:%d \n", __FUNCTION__, i, pAhdrCtx->AhdrIOData.MergeData.sw_hdrmge_e_y[i]);
    for(int i = 0; i < 17; i++)
        LOGE_AHDR("%s:lrk  sw_hdrmge_l0_y[%d]:%d \n", __FUNCTION__, i, pAhdrCtx->AhdrIOData.MergeData.sw_hdrmge_l0_y[i]);
    for(int i = 0; i < 17; i++)
        LOGE_AHDR("%s:lrk  sw_hdrmge_l1_y[%d]:%d \n", __FUNCTION__, i, pAhdrCtx->AhdrIOData.MergeData.sw_hdrmge_l1_y[i]);

    LOGE_AHDR("%s:lrk  Tmo set IOdata to register:\n", __FUNCTION__);
    LOGE_AHDR("%s:lrk  sw_hdrtmo_lgmax:%d\n", __FUNCTION__, pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_lgmax);
    LOGE_AHDR("%s:lrk  sw_hdrtmo_lgscl:%d\n", __FUNCTION__, pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_lgscl);
    LOGE_AHDR("%s:lrk  sw_hdrtmo_lgscl_inv:%d\n", __FUNCTION__, pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_lgscl_inv);
    LOGE_AHDR("%s:lrk  sw_hdrtmo_clipratio0:%d\n", __FUNCTION__, pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_clipratio0);
    LOGE_AHDR("%s:lrk  sw_hdrtmo_clipratio1:%d\n", __FUNCTION__, pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_clipratio1);
    LOGE_AHDR("%s:lrk  sw_hdrtmo_clipgap0:%d\n", __FUNCTION__, pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_clipgap0);
    LOGE_AHDR("%s:lrk  sw_hdrtmo_clipgap:%d\n", __FUNCTION__, pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_clipgap1);
    LOGE_AHDR("%s:lrk  sw_hdrtmo_ratiol:%d\n", __FUNCTION__, pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_ratiol);
    LOGE_AHDR("%s:lrk  sw_hdrtmo_hist_min:%d\n", __FUNCTION__, pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_hist_min);
    LOGE_AHDR("%s:lrk  sw_hdrtmo_hist_low:%d\n", __FUNCTION__, pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_hist_low);
    LOGE_AHDR("%s:lrk  sw_hdrtmo_hist_high:%d\n", __FUNCTION__, pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_hist_high);
    LOGE_AHDR("%s:lrk  sw_hdrtmo_hist_0p3:%d\n", __FUNCTION__, pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_hist_0p3);
    LOGE_AHDR("%s:lrk  sw_hdrtmo_hist_shift:%d\n", __FUNCTION__, pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_hist_shift);
    LOGE_AHDR("%s:lrk  sw_hdrtmo_palpha_0p18:%d\n", __FUNCTION__, pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_palpha_0p18);
    LOGE_AHDR("%s:lrk  sw_hdrtmo_palpha_lw0p5:%d\n", __FUNCTION__, pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_palpha_lw0p5);
    LOGE_AHDR("%s:lrk  sw_hdrtmo_palpha_lwscl:%d\n", __FUNCTION__, pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_palpha_lwscl);
    LOGE_AHDR("%s:lrk  sw_hdrtmo_maxpalpha:%d\n", __FUNCTION__, pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_maxpalpha);
    LOGE_AHDR("%s:lrk  sw_hdrtmo_maxgain:%d\n", __FUNCTION__, pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_maxgain);
    LOGE_AHDR("%s:lrk  sw_hdrtmo_cfg_alpha:%d\n", __FUNCTION__, pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_cfg_alpha);
    LOGE_AHDR("%s:lrk  sw_hdrtmo_set_gainoff:%d\n", __FUNCTION__, pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_set_gainoff);
    LOGE_AHDR("%s:lrk  sw_hdrtmo_set_lgmin:%d\n", __FUNCTION__, pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_set_lgmin);
    LOGE_AHDR("%s:lrk  sw_hdrtmo_set_lgmax:%d\n", __FUNCTION__, pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_set_lgmax);
    LOGE_AHDR("%s:lrk  sw_hdrtmo_set_lgmean:%d\n", __FUNCTION__, pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_set_lgmean);
    LOGE_AHDR("%s:lrk  sw_hdrtmo_set_weightkey:%d\n", __FUNCTION__, pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_set_weightkey);
    LOGE_AHDR("%s:lrk  sw_hdrtmo_set_lgrange0:%d\n", __FUNCTION__, pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_set_lgrange0);
    LOGE_AHDR("%s:lrk  sw_hdrtmo_set_lgrange1:%d\n", __FUNCTION__, pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_set_lgrange1);
    LOGE_AHDR("%s:lrk  sw_hdrtmo_set_lgavgmax:%d\n", __FUNCTION__, pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_set_lgavgmax);
    LOGE_AHDR("%s:lrk  sw_hdrtmo_set_palpha:%d\n", __FUNCTION__, pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_set_palpha);
    LOGE_AHDR("%s:lrk  sw_hdrtmo_big_en:%d\n", __FUNCTION__, pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_big_en);
    LOGE_AHDR("%s:lrk  sw_hdrtmo_nobig_en:%d\n", __FUNCTION__, pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_nobig_en);
    LOGE_AHDR("%s:lrk  sw_hdrtmo_newhist_en:%d\n", __FUNCTION__, pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_newhist_en);
    LOGE_AHDR("%s:lrk  sw_hdrtmo_cnt_mode:%d\n", __FUNCTION__, pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_cnt_mode);
    LOGE_AHDR("%s:lrk  sw_hdrtmo_cnt_vsize:%d\n", __FUNCTION__, pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_cnt_vsize);
    LOGE_AHDR("%s:lrk  sw_hdrtmo_expl_lgratio:%d\n", __FUNCTION__, pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_expl_lgratio);
    LOGE_AHDR("%s:lrk  sw_hdrtmo_lgscl_ratio:%d\n", __FUNCTION__, pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_lgscl_ratio);
    LOGE_AHDR("%s:lrk  sw_hdrtmo_gain_ld_off1:%d\n", __FUNCTION__, pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_gain_ld_off1);
    LOGE_AHDR("%s:lrk  sw_hdrtmo_gain_ld_off2:%d\n", __FUNCTION__, pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_gain_ld_off2);

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
        LOGE_AHDR( "%s:lrk  wrong para!!!\n", __FUNCTION__);

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
    pAhdrCtx->AhdrConfig.tmo_para.OEPdf[0] = 0;
    pAhdrCtx->AhdrConfig.tmo_para.OEPdf[1] = 0.1;
    pAhdrCtx->AhdrConfig.tmo_para.OEPdf[2] = 0.2;
    pAhdrCtx->AhdrConfig.tmo_para.OEPdf[3] = 0.3;
    pAhdrCtx->AhdrConfig.tmo_para.OEPdf[4] = 0.4;
    pAhdrCtx->AhdrConfig.tmo_para.OEPdf[5] = 0.5;
    pAhdrCtx->AhdrConfig.tmo_para.enLumaWeight = 0;
    pAhdrCtx->AhdrConfig.tmo_para.DarkPdfTH = 0.7;
    pAhdrCtx->AhdrConfig.tmo_para.DarkTexturePdf[0] = 0;
    pAhdrCtx->AhdrConfig.tmo_para.DarkTexturePdf[1] = 0.1;
    pAhdrCtx->AhdrConfig.tmo_para.DarkTexturePdf[2] = 0.2;
    pAhdrCtx->AhdrConfig.tmo_para.DarkTexturePdf[3] = 0.3;
    pAhdrCtx->AhdrConfig.tmo_para.DarkTexturePdf[4] = 0.4;
    pAhdrCtx->AhdrConfig.tmo_para.DarkTexturePdf[5] = 0.1;
    pAhdrCtx->AhdrConfig.tmo_para.enPosWeight = 0;
    pAhdrCtx->AhdrConfig.tmo_para.PosCoef[0] = 0;
    pAhdrCtx->AhdrConfig.tmo_para.PosCoef[1] = 0.1;
    pAhdrCtx->AhdrConfig.tmo_para.PosCoef[2] = 0.2;
    pAhdrCtx->AhdrConfig.tmo_para.PosCoef[3] = 0.3;
    pAhdrCtx->AhdrConfig.tmo_para.PosCoef[4] = 0.4;
    pAhdrCtx->AhdrConfig.tmo_para.PosCoef[5] = 0.1;
    pAhdrCtx->AhdrConfig.tmo_para.DynamicRange[0] = 1;
    pAhdrCtx->AhdrConfig.tmo_para.DynamicRange[1] = 1;
    pAhdrCtx->AhdrConfig.tmo_para.DynamicRange[2] = 1;
    pAhdrCtx->AhdrConfig.tmo_para.DynamicRange[3] = 1;
    pAhdrCtx->AhdrConfig.tmo_para.DynamicRange[4] = 1;
    pAhdrCtx->AhdrConfig.tmo_para.DynamicRange[5] = 1;
    pAhdrCtx->AhdrConfig.tmo_para.DayTh = 0.7;
    for(int i = 0; i < 6; ++i)
    {
        pAhdrCtx->AhdrConfig.tmo_para.GlobeMaxLuma[i] = 307;
        pAhdrCtx->AhdrConfig.tmo_para.GlobeLuma[i] = 184 ;
        pAhdrCtx->AhdrConfig.tmo_para.DetailsHighLight[i] = 512;
        pAhdrCtx->AhdrConfig.tmo_para.DetailsLowLight[i] = 16;
        pAhdrCtx->AhdrConfig.tmo_para.NoiseLowLight[i] = 10;//lrk need to check
        pAhdrCtx->AhdrConfig.tmo_para.SmoothControlCoef1[i] = 0 ;
        pAhdrCtx->AhdrConfig.tmo_para.SmoothControlCoef2[i] = 76;
    }
    for(int i = 0; i < 225; i++)
    {
        pAhdrCtx->AhdrConfig.tmo_para.PosWeight[i] = 1;
        pAhdrCtx->AhdrConfig.tmo_para.LumaWeight[i] = 1;
    }

    //config default AhdrPrevData data
    pAhdrCtx->AhdrPrevData.PreL2S_ratio = 8;
    pAhdrCtx->AhdrPrevData.PreLExpo = 0.01 * 8;
    pAhdrCtx->AhdrPrevData.PrevMergeHandleData.OECurve_smooth = 80;
    pAhdrCtx->AhdrPrevData.PrevMergeHandleData.OECurve_offset = 205;
    pAhdrCtx->AhdrPrevData.PrevMergeHandleData.MDCurveLM_smooth = 80;
    pAhdrCtx->AhdrPrevData.PrevMergeHandleData.MDCurveLM_offset = 38;
    pAhdrCtx->AhdrPrevData.PrevMergeHandleData.MDCurveMS_smooth = 80;
    pAhdrCtx->AhdrPrevData.PrevMergeHandleData.MDCurveMS_offset = 38;

    pAhdrCtx->AhdrPrevData.PrevTmoHandleData.GlobeMaxLuma = 307;
    pAhdrCtx->AhdrPrevData.PrevTmoHandleData.GlobeLuma = 184 ;
    pAhdrCtx->AhdrPrevData.PrevTmoHandleData.DetailsHighLight = 512;
    pAhdrCtx->AhdrPrevData.PrevTmoHandleData.DetailsLowLight = 16;
    pAhdrCtx->AhdrPrevData.PrevTmoHandleData.NoiseLowLight = 10;
    pAhdrCtx->AhdrPrevData.PrevTmoHandleData.SmoothControlCoef1 = 0 ;
    pAhdrCtx->AhdrPrevData.PrevTmoHandleData.SmoothControlCoef2 = 76;

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

    pAhdrCtx->CurrStatsData.ro_hdrtmo_lglow = ROData->ro_hdrtmo_lglow;
    pAhdrCtx->CurrStatsData.ro_hdrtmo_lgmin = ROData->ro_hdrtmo_lgmin;
    pAhdrCtx->CurrStatsData.ro_hdrtmo_lgmax = ROData->ro_hdrtmo_lgmax;
    pAhdrCtx->CurrStatsData.ro_hdrtmo_lghigh = ROData->ro_hdrtmo_lghigh;
    pAhdrCtx->CurrStatsData.ro_hdrtmo_lgmean = ROData->ro_hdrtmo_lgmean;
    pAhdrCtx->CurrStatsData.ro_hdrtmo_weightkey = ROData->ro_hdrtmo_weightkey;
    pAhdrCtx->CurrStatsData.ro_hdrtmo_lgrange0 = ROData->ro_hdrtmo_lgrange0;
    pAhdrCtx->CurrStatsData.ro_hdrtmo_lgrange1 = ROData->ro_hdrtmo_lgrange1;
    pAhdrCtx->CurrStatsData.ro_hdrtmo_lgavgmax = ROData->ro_hdrtmo_lgavgmax;
    pAhdrCtx->CurrStatsData.ro_hdrtmo_palpha = ROData->ro_hdrtmo_palpha;
    pAhdrCtx->CurrStatsData.ro_hdrtmo_linecnt = ROData->ro_hdrtmo_linecnt;
    for(int i = 0; i < 32; i++)
        pAhdrCtx->CurrStatsData.ro_array_min_max[i] = ROData->ro_array_min_max[i];

#if LRK_DEBUG_LOG
    LOGE_AHDR("%s:lrk  Ahdr RO data from register:\n", __FUNCTION__);
    LOGE_AHDR("%s:lrk  ro_hdrtmo_lglow:%d:\n", __FUNCTION__, pAhdrCtx->CurrStatsData.ro_hdrtmo_lglow);
    LOGE_AHDR("%s:lrk  ro_hdrtmo_lgmin:%d:\n", __FUNCTION__, pAhdrCtx->CurrStatsData.ro_hdrtmo_lgmin);
    LOGE_AHDR("%s:lrk  ro_hdrtmo_lgmax:%d:\n", __FUNCTION__, pAhdrCtx->CurrStatsData.ro_hdrtmo_lgmax);
    LOGE_AHDR("%s:lrk  ro_hdrtmo_lghigh:%d:\n", __FUNCTION__, pAhdrCtx->CurrStatsData.ro_hdrtmo_lghigh);
    LOGE_AHDR("%s:lrk  ro_hdrtmo_weightkey:%d:\n", __FUNCTION__, pAhdrCtx->CurrStatsData.ro_hdrtmo_weightkey);
    LOGE_AHDR("%s:lrk  ro_hdrtmo_lgmean:%d:\n", __FUNCTION__, pAhdrCtx->CurrStatsData.ro_hdrtmo_lgmean);
    LOGE_AHDR("%s:lrk  ro_hdrtmo_lgrange0:%d:\n", __FUNCTION__, pAhdrCtx->CurrStatsData.ro_hdrtmo_lgrange0);
    LOGE_AHDR("%s:lrk  ro_hdrtmo_lgrange1:%d:\n", __FUNCTION__, pAhdrCtx->CurrStatsData.ro_hdrtmo_lgrange1);
    LOGE_AHDR("%s:lrk  ro_hdrtmo_lgavgmax:%d:\n", __FUNCTION__, pAhdrCtx->CurrStatsData.ro_hdrtmo_lgavgmax);
    LOGE_AHDR("%s:lrk  ro_hdrtmo_palpha:%d:\n", __FUNCTION__, pAhdrCtx->CurrStatsData.ro_hdrtmo_palpha);
    LOGE_AHDR("%s:lrk  ro_hdrtmo_linecnt:%d:\n", __FUNCTION__, pAhdrCtx->CurrStatsData.ro_hdrtmo_linecnt);
    for(int i = 0; i < 32; i++)
        LOGE_AHDR("%s:lrk  ro_array_min_max[%d]:%d:\n", __FUNCTION__, i, pAhdrCtx->CurrStatsData.ro_array_min_max[i]);

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
    pAhdrCtx->AhdrConfig.merge_para.MergeMode = pCalibDb->ahdr.merge.mode;
    pAhdrCtx->AhdrConfig.merge_para.OECurve_damp = LIMIT_VALUE(pCalibDb->ahdr.merge.oeCurve_damp, DAMPMAX, DAMPMIN);
    pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_damp = LIMIT_VALUE(pCalibDb->ahdr.merge.mdCurveLm_damp, DAMPMAX, DAMPMIN);
    pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_damp = LIMIT_VALUE(pCalibDb->ahdr.merge.mdCurveMs_damp, DAMPMAX, DAMPMIN);
    for (int i = 0; i < 6; i++ )
    {
        pAhdrCtx->AhdrConfig.merge_para.EnvLv[i] = LIMIT_VALUE(pCalibDb->ahdr.merge.envLevel[i], ENVLVMAX, ENVLVMIN);
        pAhdrCtx->AhdrConfig.merge_para.MoveCoef[i] = LIMIT_VALUE(pCalibDb->ahdr.merge.moveCoef[i], MOVECOEFMAX, MOVECOEFMIN);
        pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[i] = LIMIT_VALUE(pCalibDb->ahdr.merge.oeCurve_smooth[i], OECURVESMOOTHMAX, OECURVESMOOTHMIN);
        pAhdrCtx->AhdrConfig.merge_para.OECurve_offset[i] = LIMIT_VALUE(pCalibDb->ahdr.merge.oeCurve_offset[i], OECURVEOFFSETMAX, OECURVEOFFSETMIN);
        pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[i] = LIMIT_VALUE(pCalibDb->ahdr.merge.mdCurveLm_smooth[i], MDCURVESMOOTHMAX, MDCURVESMOOTHMIN);
        pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[i] = LIMIT_VALUE(pCalibDb->ahdr.merge.mdCurveLm_offset[i], MDCURVEOFFSETMAX, MDCURVEOFFSETMIN);
        pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[i] = LIMIT_VALUE(pCalibDb->ahdr.merge.mdCurveMs_smooth[i], MDCURVESMOOTHMAX, MDCURVESMOOTHMIN);
        pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[i] = LIMIT_VALUE(pCalibDb->ahdr.merge.mdCurveMs_offset[i], MDCURVEOFFSETMAX, MDCURVEOFFSETMIN);
    }

    //TMO
    pAhdrCtx->AhdrConfig.tmo_para.enLumaWeight = LIMIT_VALUE(pCalibDb->ahdr.tmo.enLumaWeight, ENMAX, ENMIN);
    pAhdrCtx->AhdrConfig.tmo_para.enPosWeight = LIMIT_VALUE(pCalibDb->ahdr.tmo.enPosWeight, ENMAX, ENMIN);
    pAhdrCtx->AhdrConfig.tmo_para.DarkPdfTH = LIMIT_VALUE(pCalibDb->ahdr.tmo.DarkPdfTH, ENMAX, ENMIN);
    pAhdrCtx->AhdrConfig.tmo_para.DayTh = LIMIT_VALUE(pCalibDb->ahdr.tmo.DayTh, DARKPDFTHMAX, DARKPDFTHMIN);
    pAhdrCtx->AhdrConfig.tmo_para.damp = LIMIT_VALUE(pCalibDb->ahdr.tmo.damp, DAMPMAX, DAMPMIN);
    for(int i = 0; i < 6; i++)
    {
        pAhdrCtx->AhdrConfig.tmo_para.EnvLv[i] = LIMIT_VALUE(pCalibDb->ahdr.tmo.envLevel[i], ENVLVMAX, ENVLVMIN);
        pAhdrCtx->AhdrConfig.tmo_para.GlobeLuma[i] = LIMIT_VALUE(pCalibDb->ahdr.tmo.globalLuma[i], GLOBELUMAMAX, GLOBELUMAMIN) ;
        pAhdrCtx->AhdrConfig.tmo_para.GlobeMaxLuma[i] = LIMIT_VALUE(pCalibDb->ahdr.tmo.globalMaxLuma[i], GLOBEMAXLUMAMAX, GLOBEMAXLUMAMIN) ;

        pAhdrCtx->AhdrConfig.tmo_para.OEPdf[i] = LIMIT_VALUE(pCalibDb->ahdr.tmo.OEPdf[i], OEPDFMAX, OEPDFMIN) ;
        pAhdrCtx->AhdrConfig.tmo_para.DetailsHighLight[i] = LIMIT_VALUE(pCalibDb->ahdr.tmo.detailsHighLight[i], DETAILSHIGHLIGHTMAX, DETAILSHIGHLIGHTMIN) ;

        pAhdrCtx->AhdrConfig.tmo_para.DarkTexturePdf[i] = LIMIT_VALUE(pCalibDb->ahdr.tmo.DarkTexturePdf[i], DARKTEXTUREPDFMAX, DARKTEXTUREPDFMIN) ;
        pAhdrCtx->AhdrConfig.tmo_para.DetailsLowLight[i] = LIMIT_VALUE(pCalibDb->ahdr.tmo.detailsLowLight[i], DETAILSLOWLIGHTMAX, DETAILSLOWLIGHTMIN) ;

        pAhdrCtx->AhdrConfig.tmo_para.PosCoef[i] = LIMIT_VALUE(pCalibDb->ahdr.tmo.PosCoef[i], POSCOEFMAX, POSCOEFMIN) ;
        pAhdrCtx->AhdrConfig.tmo_para.NoiseLowLight[i] = LIMIT_VALUE(pCalibDb->ahdr.tmo.noiseLowLight[i], NOISELOWLIGHTMAX, NOISELOWLIGHTMIN) ;

        pAhdrCtx->AhdrConfig.tmo_para.DynamicRange[i] = LIMIT_VALUE(pCalibDb->ahdr.tmo.DynamicRange[i], DYNAMICRANGEMAX, DYNAMICRANGEMIN) ;
        pAhdrCtx->AhdrConfig.tmo_para.SmoothControlCoef1[i] = LIMIT_VALUE(pCalibDb->ahdr.tmo.smoothControlCoef1[i], SMOOTHCONTROLCOEFMAX, SMOOTHCONTROLCOEFMIN) ;
        pAhdrCtx->AhdrConfig.tmo_para.SmoothControlCoef2[i] = LIMIT_VALUE(pCalibDb->ahdr.tmo.smoothControlCoef2[i], SMOOTHCONTROLCOEFMAX, SMOOTHCONTROLCOEFMIN) ;
    }
    for(int i = 0; i < 225; i++)
    {
        pAhdrCtx->AhdrConfig.tmo_para.PosWeight[i] = LIMIT_VALUE(pCalibDb->ahdr.tmo.PosWeight[i], POSWEIGHTMAX, POSWEIGHTMIN) ;
        pAhdrCtx->AhdrConfig.tmo_para.LumaWeight[i] = LIMIT_VALUE(pCalibDb->ahdr.tmo.LumaWeight[i], LUMAWEIGHTMAX, LUMAWEIGHTMIN) ;
    }

    //check hdr frame mode


#if LRK_DEBUG_LOG
    LOGE_AHDR("%s:lrk  Ahdr comfig data from xml:\n", __FUNCTION__);
    LOGE_AHDR("%s:lrk  Merge MergeMode:%d:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.MergeMode);
    LOGE_AHDR("%s:lrk  Merge EnvLv:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.EnvLv[0], pAhdrCtx->AhdrConfig.merge_para.EnvLv[1], pAhdrCtx->AhdrConfig.merge_para.EnvLv[2],
              pAhdrCtx->AhdrConfig.merge_para.EnvLv[3], pAhdrCtx->AhdrConfig.merge_para.EnvLv[4], pAhdrCtx->AhdrConfig.merge_para.EnvLv[5]);
    LOGE_AHDR("%s:lrk  Merge OECurve_smooth:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[0], pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[1], pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[2],
              pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[3], pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[4], pAhdrCtx->AhdrConfig.merge_para.OECurve_smooth[5]);
    LOGE_AHDR("%s:lrk  Merge OECurve_offset:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.OECurve_offset[0], pAhdrCtx->AhdrConfig.merge_para.OECurve_offset[1], pAhdrCtx->AhdrConfig.merge_para.OECurve_offset[2],
              pAhdrCtx->AhdrConfig.merge_para.OECurve_offset[3], pAhdrCtx->AhdrConfig.merge_para.OECurve_offset[4], pAhdrCtx->AhdrConfig.merge_para.OECurve_offset[5]);
    LOGE_AHDR("%s:lrk  Merge MoveCoef:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.MoveCoef[0], pAhdrCtx->AhdrConfig.merge_para.MoveCoef[1], pAhdrCtx->AhdrConfig.merge_para.MoveCoef[2],
              pAhdrCtx->AhdrConfig.merge_para.MoveCoef[3], pAhdrCtx->AhdrConfig.merge_para.MoveCoef[4], pAhdrCtx->AhdrConfig.merge_para.MoveCoef[5]);
    LOGE_AHDR("%s:lrk  Merge MDCurveLM_smooth:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[0], pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[1], pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[2],
              pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[3], pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[4], pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_smooth[5]);
    LOGE_AHDR("%s:lrk  Merge MDCurveLM_offset:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[0], pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[1], pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[2],
              pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[3], pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[4], pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_offset[5]);
    LOGE_AHDR("%s:lrk  Merge MDCurveMS_smooth:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[0], pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[1], pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[2],
              pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[3], pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[4], pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_smooth[5]);
    LOGE_AHDR("%s:lrk  Merge MDCurveMS_offset:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[0], pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[1], pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[2],
              pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[3], pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[4], pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_offset[5]);
    LOGE_AHDR("%s:lrk  Merge OECurve_damp:%f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.OECurve_damp);
    LOGE_AHDR("%s:lrk  Merge MDCurveLM_damp:%f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.MDCurveLM_damp);
    LOGE_AHDR("%s:lrk  Merge MDCurveMS_damp:%f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.merge_para.MDCurveMS_damp);

    LOGE_AHDR("%s:lrk  Tmo EnvLv:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.EnvLv[0], pAhdrCtx->AhdrConfig.tmo_para.EnvLv[1], pAhdrCtx->AhdrConfig.tmo_para.EnvLv[2]
              , pAhdrCtx->AhdrConfig.tmo_para.EnvLv[3], pAhdrCtx->AhdrConfig.tmo_para.EnvLv[4], pAhdrCtx->AhdrConfig.tmo_para.EnvLv[5]);
    LOGE_AHDR("%s:lrk  Tmo GlobeMaxLuma:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.GlobeMaxLuma[0], pAhdrCtx->AhdrConfig.tmo_para.GlobeMaxLuma[1], pAhdrCtx->AhdrConfig.tmo_para.GlobeMaxLuma[2]
              , pAhdrCtx->AhdrConfig.tmo_para.GlobeMaxLuma[3], pAhdrCtx->AhdrConfig.tmo_para.GlobeMaxLuma[4], pAhdrCtx->AhdrConfig.tmo_para.GlobeMaxLuma[5]);
    LOGE_AHDR("%s:lrk  Tmo GlobeLuma:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.GlobeLuma[0], pAhdrCtx->AhdrConfig.tmo_para.GlobeLuma[1], pAhdrCtx->AhdrConfig.tmo_para.GlobeLuma[2]
              , pAhdrCtx->AhdrConfig.tmo_para.GlobeLuma[3], pAhdrCtx->AhdrConfig.tmo_para.GlobeLuma[4], pAhdrCtx->AhdrConfig.tmo_para.GlobeLuma[5]);
    LOGE_AHDR("%s:lrk  Tmo OEPdf:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.OEPdf[0], pAhdrCtx->AhdrConfig.tmo_para.OEPdf[1], pAhdrCtx->AhdrConfig.tmo_para.OEPdf[2]
              , pAhdrCtx->AhdrConfig.tmo_para.OEPdf[3], pAhdrCtx->AhdrConfig.tmo_para.OEPdf[4], pAhdrCtx->AhdrConfig.tmo_para.OEPdf[5]);
    LOGE_AHDR("%s:lrk  Tmo DetailsHighLight:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.DetailsHighLight[0], pAhdrCtx->AhdrConfig.tmo_para.DetailsHighLight[1], pAhdrCtx->AhdrConfig.tmo_para.DetailsHighLight[2]
              , pAhdrCtx->AhdrConfig.tmo_para.DetailsHighLight[3], pAhdrCtx->AhdrConfig.tmo_para.DetailsHighLight[4], pAhdrCtx->AhdrConfig.tmo_para.DetailsHighLight[5]);
    LOGE_AHDR("%s:lrk  Tmo enLumaWeight:%f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.enLumaWeight);
    LOGE_AHDR("%s:lrk  Tmo DarkPdfTH:%f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.DarkPdfTH);
    LOGE_AHDR("%s:lrk  Tmo DarkTexturePdf:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.DarkTexturePdf[0], pAhdrCtx->AhdrConfig.tmo_para.DarkTexturePdf[1], pAhdrCtx->AhdrConfig.tmo_para.DarkTexturePdf[2]
              , pAhdrCtx->AhdrConfig.tmo_para.DarkTexturePdf[3], pAhdrCtx->AhdrConfig.tmo_para.DarkTexturePdf[4], pAhdrCtx->AhdrConfig.tmo_para.DarkTexturePdf[5]);
    LOGE_AHDR("%s:lrk  Tmo DetailsLowLight:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.DetailsLowLight[0], pAhdrCtx->AhdrConfig.tmo_para.DetailsLowLight[1], pAhdrCtx->AhdrConfig.tmo_para.DetailsLowLight[2]
              , pAhdrCtx->AhdrConfig.tmo_para.DetailsLowLight[3], pAhdrCtx->AhdrConfig.tmo_para.DetailsLowLight[4], pAhdrCtx->AhdrConfig.tmo_para.DetailsLowLight[5]);
    LOGE_AHDR("%s:lrk  Tmo enPosWeight:%f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.enPosWeight);
    LOGE_AHDR("%s:lrk  Tmo PosCoef:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.PosCoef[0], pAhdrCtx->AhdrConfig.tmo_para.PosCoef[1], pAhdrCtx->AhdrConfig.tmo_para.PosCoef[2]
              , pAhdrCtx->AhdrConfig.tmo_para.PosCoef[3], pAhdrCtx->AhdrConfig.tmo_para.PosCoef[4], pAhdrCtx->AhdrConfig.tmo_para.PosCoef[5]);
    LOGE_AHDR("%s:lrk  Tmo NoiseLowLight:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.NoiseLowLight[0], pAhdrCtx->AhdrConfig.tmo_para.NoiseLowLight[1], pAhdrCtx->AhdrConfig.tmo_para.NoiseLowLight[2]
              , pAhdrCtx->AhdrConfig.tmo_para.NoiseLowLight[3], pAhdrCtx->AhdrConfig.tmo_para.NoiseLowLight[4], pAhdrCtx->AhdrConfig.tmo_para.NoiseLowLight[5]);
    LOGE_AHDR("%s:lrk  Tmo DynamicRange:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.DynamicRange[0], pAhdrCtx->AhdrConfig.tmo_para.DynamicRange[1], pAhdrCtx->AhdrConfig.tmo_para.DynamicRange[2]
              , pAhdrCtx->AhdrConfig.tmo_para.DynamicRange[3], pAhdrCtx->AhdrConfig.tmo_para.DynamicRange[4], pAhdrCtx->AhdrConfig.tmo_para.DynamicRange[5]);
    LOGE_AHDR("%s:lrk  Tmo DayTh:%f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.DayTh);
    LOGE_AHDR("%s:lrk  Tmo SmoothControlCoef1:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.SmoothControlCoef1[0], pAhdrCtx->AhdrConfig.tmo_para.SmoothControlCoef1[1], pAhdrCtx->AhdrConfig.tmo_para.SmoothControlCoef1[2]
              , pAhdrCtx->AhdrConfig.tmo_para.SmoothControlCoef1[3], pAhdrCtx->AhdrConfig.tmo_para.SmoothControlCoef1[4], pAhdrCtx->AhdrConfig.tmo_para.SmoothControlCoef1[5]);
    LOGE_AHDR("%s:lrk  Tmo SmoothControlCoef2:%f %f %f %f %f %f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.SmoothControlCoef2[0], pAhdrCtx->AhdrConfig.tmo_para.SmoothControlCoef2[1], pAhdrCtx->AhdrConfig.tmo_para.SmoothControlCoef2[2]
              , pAhdrCtx->AhdrConfig.tmo_para.SmoothControlCoef2[3], pAhdrCtx->AhdrConfig.tmo_para.SmoothControlCoef2[4], pAhdrCtx->AhdrConfig.tmo_para.SmoothControlCoef2[5]);
    LOGE_AHDR("%s:lrk  Tmo Damp:%f:\n", __FUNCTION__, pAhdrCtx->AhdrConfig.tmo_para.damp);
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
    pAhdrCtx->CurrAeResult.GlobalEnvLv = AecHdrPreResult.GlobalEnvLv;
    pAhdrCtx->CurrAeResult.M2S_Ratio = AecHdrPreResult.M2S_ExpRatio;
    pAhdrCtx->CurrAeResult.M2S_Ratio = pAhdrCtx->CurrAeResult.M2S_Ratio < 1 ? 1 : pAhdrCtx->CurrAeResult.M2S_Ratio;
    pAhdrCtx->CurrAeResult.L2M_Ratio = AecHdrPreResult.L2M_ExpRatio;
    pAhdrCtx->CurrAeResult.L2M_Ratio = pAhdrCtx->CurrAeResult.L2M_Ratio < 1 ? 1 : pAhdrCtx->CurrAeResult.L2M_Ratio;
    pAhdrCtx->CurrAeResult.DynamicRange = AecHdrPreResult.DynamicRange;
    pAhdrCtx->CurrAeResult.OEPdf = AecHdrPreResult.MaxLumaROIPdf;

    //Normalize the current envLv for AEC
    float maxEnvLuma = 65 / (1 * 0.01);
    float minEnvLuma = 65 / (16 * 0.03);
    pAhdrCtx->CurrAeResult.GlobalEnvLv = (pAhdrCtx->CurrAeResult.GlobalEnvLv - minEnvLuma) / maxEnvLuma;
    pAhdrCtx->CurrAeResult.GlobalEnvLv = LIMIT_VALUE(pAhdrCtx->CurrAeResult.GlobalEnvLv, 1, 0);
    pAhdrCtx->CurrAeResult.GlobalEnvLv = 1 - pAhdrCtx->CurrAeResult.GlobalEnvLv;

    //transfer CurrAeResult data into AhdrHandle
    //get Curren hdr mode
    pAhdrCtx->CurrHandleData.MergeMode = pAhdrCtx->AhdrConfig.merge_para.MergeMode;

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
    pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeMaxLuma = GetCurrPara(pAhdrCtx->CurrHandleData.CurrEnvLv,
            pAhdrCtx->AhdrConfig.tmo_para.EnvLv,
            pAhdrCtx->AhdrConfig.tmo_para.GlobeMaxLuma);
    //get Current tmo SmoothControlCoef1 SmoothControlCoef2
    pAhdrCtx->CurrHandleData.DayTh = pAhdrCtx->AhdrConfig.tmo_para.DayTh;
    if(pAhdrCtx->CurrHandleData.CurrEnvLv > pAhdrCtx->CurrHandleData.DayTh)
    {
        pAhdrCtx->CurrHandleData.CurrDynamicRange = pAhdrCtx->CurrAeResult.DynamicRange;
        pAhdrCtx->CurrHandleData.CurrDynamicRange = LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrDynamicRange, DYNAMICRANGEMAX, DYNAMICRANGEMIN);
        pAhdrCtx->CurrHandleData.CurrTmoHandleData.SmoothControlCoef1 = GetCurrPara(pAhdrCtx->CurrHandleData.CurrDynamicRange,
                pAhdrCtx->AhdrConfig.tmo_para.DynamicRange,
                pAhdrCtx->AhdrConfig.tmo_para.SmoothControlCoef1);
        pAhdrCtx->CurrHandleData.CurrTmoHandleData.SmoothControlCoef2 = GetCurrPara(pAhdrCtx->CurrHandleData.CurrDynamicRange,
                pAhdrCtx->AhdrConfig.tmo_para.DynamicRange,
                pAhdrCtx->AhdrConfig.tmo_para.SmoothControlCoef2);

    }
    else
    {
        pAhdrCtx->CurrHandleData.CurrTmoHandleData.SmoothControlCoef1 = 0;
        pAhdrCtx->CurrHandleData.CurrTmoHandleData.SmoothControlCoef2 = 76;
    }

    //get Current tmo DetailsHighLight
    pAhdrCtx->CurrHandleData.CurrOEPdf = pAhdrCtx->CurrAeResult.OEPdf;
    pAhdrCtx->CurrHandleData.CurrOEPdf = LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrOEPdf, OEPDFMAX, OEPDFMIN);
    pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsHighLight = GetCurrPara(pAhdrCtx->CurrHandleData.CurrOEPdf,
            pAhdrCtx->AhdrConfig.tmo_para.OEPdf,
            pAhdrCtx->AhdrConfig.tmo_para.DetailsHighLight);
    //get Current tmo NoiseLowLight
#if 0
    if(pAhdrCtx->AhdrConfig.tmo_para.enPosWeight > 0)
    {

        int Block_Width = (int)(pAhdrCtx->width / 15);
        int Block_Height = (int)(pAhdrCtx->height / 15);

        pAhdrCtx->CurrAfResult.CurrAfTargetPos = AfPreResult.af_target_info.af_target_pos;
        pAhdrCtx->CurrAfResult.CurrAfTargetHeight = AfPreResult.af_target_info.af_target_height;
        pAhdrCtx->CurrAfResult.CurrAfTargetWidth = AfPreResult.af_target_info.af_target_width;
        int Block_Cnt_W = (int)(pAhdrCtx->CurrAfResult.CurrAfTargetWidth / Block_Width);
        int Block_Cnt_H = (int)(pAhdrCtx->CurrAfResult.CurrAfTargetWidth / Block_Height);
        int TargetCnt = Block_Cnt_H * Block_Cnt_W;
        int Af_Matrix[225] = {0};
        if(Block_Cnt_W == 1 && Block_Cnt_H == 1 )
            Af_Matrix[AfPreResult.af_target_info.af_target_pos] = 1;
        else if (Block_Cnt_W == 1 && Block_Cnt_H == 2 )
        {
            Af_Matrix[AfPreResult.af_target_info.af_target_pos] = 1;
            Af_Matrix[AfPreResult.af_target_info.af_target_pos + 15] = 1;
        }
        else if (Block_Cnt_W == 1 && Block_Cnt_H == 3 )
        {
            Af_Matrix[AfPreResult.af_target_info.af_target_pos] = 1;
            Af_Matrix[AfPreResult.af_target_info.af_target_pos + 15] = 1;
            Af_Matrix[AfPreResult.af_target_info.af_target_pos + 15 * 2] = 1;
        }
        else if (Block_Cnt_W == 2 && Block_Cnt_H == 1 )
        {
            Af_Matrix[AfPreResult.af_target_info.af_target_pos] = 1;
            Af_Matrix[AfPreResult.af_target_info.af_target_pos + 1] = 1;
        }
        else if (Block_Cnt_W == 2 && Block_Cnt_H == 2 )
        {
            Af_Matrix[AfPreResult.af_target_info.af_target_pos] = 1;
            Af_Matrix[AfPreResult.af_target_info.af_target_pos + 1] = 1;
            Af_Matrix[AfPreResult.af_target_info.af_target_pos + 15] = 1;
            Af_Matrix[AfPreResult.af_target_info.af_target_pos + 15 + 1] = 1;
        }
        else if (Block_Cnt_W == 2 && Block_Cnt_H == 3 )
        {
            Af_Matrix[AfPreResult.af_target_info.af_target_pos] = 1;
            Af_Matrix[AfPreResult.af_target_info.af_target_pos + 1] = 1;
            Af_Matrix[AfPreResult.af_target_info.af_target_pos + 15] = 1;
            Af_Matrix[AfPreResult.af_target_info.af_target_pos + 15 + 1] = 1;
            Af_Matrix[AfPreResult.af_target_info.af_target_pos + 15 * 2] = 1;
            Af_Matrix[AfPreResult.af_target_info.af_target_pos + 15 * 2 + 1] = 1;
        }
        else if (Block_Cnt_W == 3 && Block_Cnt_H == 1 )
        {
            Af_Matrix[AfPreResult.af_target_info.af_target_pos] = 1;
            Af_Matrix[AfPreResult.af_target_info.af_target_pos + 1] = 1;
            Af_Matrix[AfPreResult.af_target_info.af_target_pos + 2] = 1;
        }
        else if (Block_Cnt_W == 3 && Block_Cnt_H == 2 )
        {
            Af_Matrix[AfPreResult.af_target_info.af_target_pos] = 1;
            Af_Matrix[AfPreResult.af_target_info.af_target_pos + 1] = 1;
            Af_Matrix[AfPreResult.af_target_info.af_target_pos + 2] = 1;
            Af_Matrix[AfPreResult.af_target_info.af_target_pos + 15] = 1;
            Af_Matrix[AfPreResult.af_target_info.af_target_pos + 15 + 1] = 1;
            Af_Matrix[AfPreResult.af_target_info.af_target_pos + 15 + 2] = 1;
        }
        else if (Block_Cnt_W == 3 && Block_Cnt_H == 3 )
        {
            Af_Matrix[AfPreResult.af_target_info.af_target_pos] = 1;
            Af_Matrix[AfPreResult.af_target_info.af_target_pos + 1] = 1;
            Af_Matrix[AfPreResult.af_target_info.af_target_pos + 2] = 1;
            Af_Matrix[AfPreResult.af_target_info.af_target_pos + 15] = 1;
            Af_Matrix[AfPreResult.af_target_info.af_target_pos + 15 + 1] = 1;
            Af_Matrix[AfPreResult.af_target_info.af_target_pos + 15 + 2] = 1;
            Af_Matrix[AfPreResult.af_target_info.af_target_pos + 15 * 2] = 1;
            Af_Matrix[AfPreResult.af_target_info.af_target_pos + 15 * 2 + 1] = 1;
            Af_Matrix[AfPreResult.af_target_info.af_target_pos + 15 * 2 + 2] = 1;
        }
        float total_weight = 0;
        float total_PosWeight = 0;
        for(int i = 0; i < 225; i++)
        {
            pAhdrCtx->CurrHandleData.PosWeight[i] = pAhdrCtx->AhdrConfig.tmo_para.PosWeight[i];
            total_weight += pAhdrCtx->CurrHandleData.PosWeight[i];
        }
        for(int i = 0; i < 225; i++)
        {
            pAhdrCtx->CurrHandleData.PosWeight[i] = pAhdrCtx->CurrHandleData.PosWeight[i] / total_weight;
            total_PosWeight += pAhdrCtx->CurrHandleData.PosWeight[i] * Af_Matrix[i];
        }
        pAhdrCtx->CurrHandleData.CurrTotalPosWeight = total_PosWeight;
        else
            pAhdrCtx->CurrHandleData.CurrTotalPosWeight = 1;
    }
#endif
    pAhdrCtx->CurrHandleData.CurrTotalPosWeight = 10;
    pAhdrCtx->CurrHandleData.CurrTotalPosWeight = LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrTotalPosWeight, 100, 1);
    pAhdrCtx->CurrHandleData.CurrTmoHandleData.NoiseLowLight = GetCurrPara(pAhdrCtx->CurrHandleData.CurrTotalPosWeight,
            pAhdrCtx->AhdrConfig.tmo_para.PosCoef,
            pAhdrCtx->AhdrConfig.tmo_para.NoiseLowLight);
    //get Current tmo DetailsLowLight
#if 0
    pAhdrCtx->CurrHandleData.DarkPdfTH = pAhdrCtx->AhdrConfig.tmo_para.DarkPdfTH;
    if(pAhdrCtx->CurrHandleData.CurrDarkPdf > pAhdrCtx->CurrHandleData.DarkPdfTH )
    {
        float TotalLumaWeight;
        float Sharpness = 0;
        for(int i = 0; i < 225; i++)
        {
            pAhdrCtx.CurrHandleData.LumaWeight[i] = pAhdrCtx->AhdrConfig.tmo_para.LumaWeight[i];
            TotalLumaWeight += pAhdrCtx.CurrHandleData.LumaWeight[i];
        }
        for(int i = 0; i < 225; i++)
        {
            pAhdrCtx.CurrHandleData.LumaWeight[i] = pAhdrCtx.CurrHandleData.LumaWeight[i] / TotalLumaWeight;
            pAhdrCtx->CurrAfResult.GlobalSharpnessCompensated[i] = AfPreResult.global_sharpness_compensated[i];
            Sharpness += pAhdrCtx.CurrHandleData.LumaWeight[i] * pAhdrCtx->CurrAfResult.GlobalSharpnessCompensated[i];
        }
        pAhdrCtx->CurrHandleData.CurrTotalSharpness = Sharpness;
    }
    else
        pAhdrCtx->CurrHandleData.CurrTotalSharpness = 1;
#endif
    pAhdrCtx->CurrHandleData.CurrTotalSharpness = 0;
    pAhdrCtx->CurrHandleData.CurrTotalSharpness = LIMIT_VALUE(pAhdrCtx->CurrHandleData.CurrTotalSharpness, 1, 0);
    pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsLowLight = GetCurrPara(pAhdrCtx->CurrHandleData.CurrTotalSharpness,
            pAhdrCtx->AhdrConfig.tmo_para.DarkTexturePdf,
            pAhdrCtx->AhdrConfig.tmo_para.DetailsLowLight);

    //get Current tmo TmoDamp
    pAhdrCtx->CurrHandleData.TmoDamp = pAhdrCtx->AhdrConfig.tmo_para.damp;

    //read curren rodata
#if 1
    if (pAhdrCtx->CurrHandleData.CurrTmoHandleData.SmoothControlCoef1 > 0 && pAhdrCtx->CurrHandleData.CurrTmoHandleData.SmoothControlCoef1 <= 255)
    {
        int data[16];
        data[0] = pAhdrCtx->CurrStatsData.ro_hdrtmo_lgmean;
        for(int j = 0; j < 15; j++)
            data[j + 1] = pAhdrCtx->AhdrPrevData.ro_hdrtmo_lgmean_all[j];
        for(int k = 0; k < 16; k++)
            pAhdrCtx->AhdrPrevData.ro_hdrtmo_lgmean_all[k] = data[k];
    }
#endif

    //calc the current merge luma
    float MergeLuma = (float)pAhdrCtx->CurrStatsData.ro_hdrtmo_lgmean;
    MergeLuma = MergeLuma / 2048.0;
    MergeLuma = pow(2, MergeLuma);
    MergeLuma = 256 * MergeLuma / (4096 * pAhdrCtx->CurrHandleData.CurrL2S_Ratio);


#if LRK_DEBUG_LOG
    LOGE_AHDR("%s:lrk  Ahdr Current frame cnt:%d:\n",  __FUNCTION__, pAhdrCtx->frameCnt);
    LOGE_AHDR("%s:lrk  Ahdr Current Handle data:\n", __FUNCTION__);
    LOGE_AHDR("%s:lrk  Current MergeMode:         %d \n", __FUNCTION__, pAhdrCtx->CurrHandleData.MergeMode);
    LOGE_AHDR("%s:lrk  Current CurrL2S_Ratio:     %f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrL2S_Ratio);
    LOGE_AHDR("%s:lrk  Current CurrL2M_Ratio:     %f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrL2M_Ratio);
    LOGE_AHDR("%s:lrk  Current CurrL2L_Ratio:     %f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrL2L_Ratio);
    LOGE_AHDR("%s:lrk  Current CurrLExpo:         %f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrLExpo);
    LOGE_AHDR("%s:lrk  Current CurrEnvLv:         %f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrEnvLv);
    LOGE_AHDR("%s:lrk  Current CurrTotalPosWeight:%f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrTotalPosWeight);
    LOGE_AHDR("%s:lrk  Current CurrMoveCoef:      %f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrMoveCoef);
    LOGE_AHDR("%s:lrk  Current CurrDynamicRange:  %f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrDynamicRange);
    LOGE_AHDR("%s:lrk  Current CurrOEPdf:         %f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrOEPdf);
    LOGE_AHDR("%s:lrk  Current CurrDarkPdf:       %f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrDarkPdf);
    LOGE_AHDR("%s:lrk  Current CurrTotalSharpness:%f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrTotalSharpness);
    LOGE_AHDR("%s:lrk  Current OECurve_smooth:    %f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrMergeHandleData.OECurve_smooth);
    LOGE_AHDR("%s:lrk  Current OECurve_offset:    %f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrMergeHandleData.OECurve_offset);
    LOGE_AHDR("%s:lrk  Current MDCurveLM_smooth:  %f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveLM_smooth);
    LOGE_AHDR("%s:lrk  Current MDCurveLM_offset:  %f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveLM_offset);
    LOGE_AHDR("%s:lrk  Current MDCurveMS_smooth:  %f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveMS_smooth);
    LOGE_AHDR("%s:lrk  Current MDCurveMS_offset:  %f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrMergeHandleData.MDCurveMS_offset);
    LOGE_AHDR("%s:lrk  Current GlobeLuma:         %f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeLuma);
    LOGE_AHDR("%s:lrk  Current GlobeMaxLuma:      %f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeMaxLuma);
    LOGE_AHDR("%s:lrk  Current DetailsHighLight:  %f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsHighLight);
    LOGE_AHDR("%s:lrk  Current DetailsLowLight:   %f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsLowLight);
    LOGE_AHDR("%s:lrk  Current NoiseLowLight:     %f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrTmoHandleData.NoiseLowLight);
    LOGE_AHDR("%s:lrk  Current SmoothControlCoef1:%f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrTmoHandleData.SmoothControlCoef1);
    LOGE_AHDR("%s:lrk  Current SmoothControlCoef2:%f \n", __FUNCTION__, pAhdrCtx->CurrHandleData.CurrTmoHandleData.SmoothControlCoef2);
    LOGE_AHDR("%s:lrk  preFrame Merge mean Luma:%f \n", __FUNCTION__, MergeLuma);
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
    memcpy(&pAhdrCtx->AhdrPrevData.PrevMergeHandleData, &pAhdrCtx->CurrHandleData.CurrMergeHandleData, sizeof(MergeHandleData_s));
    memcpy(&pAhdrCtx->AhdrPrevData.PrevTmoHandleData, &pAhdrCtx->CurrHandleData.CurrTmoHandleData, sizeof(TmoHandleData_s));
    ++pAhdrCtx->frameCnt;

    LOGI_AHDR( "%s:exit!\n", __FUNCTION__);
}
