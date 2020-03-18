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
 * @file tmo.cpp
 *
 * @brief
 *   ADD_DESCRIPTION_HERE
 *
 *****************************************************************************/
#include "math.h"
#include "rk_aiq_types_ahdr_algo_int.h"
#include "rk_aiq_ahdr_algo_tmo.h"

/******************************************************************************
 * GetSetLgmean()
 *****************************************************************************/
unsigned short GetSetLgmean(AhdrHandle_t pAhdrCtx)
{
    LOGI_AHDR( "%s:enter!\n", __FUNCTION__);

    float weight[16] = {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    float value = 0;
    float lgmean_all[16];
    unsigned short returnValue;

    for(int i = 0; i < 16; i++)
        lgmean_all[i] = pAhdrCtx->AhdrPrevData.ro_hdrtmo_lgmean_all[i] / 2048.0;

    if(pAhdrCtx->frameCnt >= 16)
    {
        for(int i = 0; i < 16; i++)
            value = value + weight[i] * lgmean_all[i];
        value = value / 136;
    }

    else
    {
        for(int i = 0; i < pAhdrCtx->frameCnt; i++)
            value = value + weight[i] * lgmean_all[i];
        value = value / ((pAhdrCtx->frameCnt + 1) * (32 - pAhdrCtx->frameCnt) / 2.0);
    }

    returnValue = (int)SHIFT11BIT(value) ;

#if LRK_DEBUG_LOG
    LOGE_AHDR( "%s:lrk frameCnt:%d value:%f returnValue:%d\n", __FUNCTION__, pAhdrCtx->frameCnt, value, returnValue);
#endif

    return returnValue;

    LOGI_AHDR( "%s:exit!\n", __FUNCTION__);

}
/******************************************************************************
 * GetSetLgAvgMax()
 *****************************************************************************/
unsigned short GetSetLgAvgMax(AhdrHandle_t pAhdrCtx, float set_lgmin, float set_lgmax)
{
    LOGI_AHDR("%s:Enter!\n", __FUNCTION__);
    float WeightKey = 0.0;
    float value = 0.0;
    float set_lgmean = pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_set_lgmean / 2048.0;
    float lgrange1 = pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_set_lgrange1 / 2048.0;
    unsigned short returnValue;

    WeightKey = MIN(2 / (set_lgmax - set_lgmean), 2 / (1 - set_lgmin));
    WeightKey = MIN((1 - WeightKey), 0.6);
    WeightKey = MAX(WeightKey, 0);
    value = WeightKey * set_lgmax + (1 - WeightKey) * set_lgmean;
    value = MIN(value, lgrange1);
    returnValue = (int)SHIFT11BIT(value);

#if LRK_DEBUG_LOG
    LOGE_AHDR( "%s:lrk set_lgmin:%f set_lgmax:%f set_lgmean:%f lgrange1:%f value:%f returnValue:%d\n", __FUNCTION__, set_lgmin, set_lgmax, set_lgmean, lgrange1, value, returnValue);
#endif

    return returnValue;
    LOGI_AHDR("%s:Eixt!\n", __FUNCTION__);

}
/******************************************************************************
 * GetSetLgRange0()
 *****************************************************************************/
unsigned short GetSetLgRange0(AhdrHandle_t pAhdrCtx, float set_lgmin, float set_lgmax)
{
    LOGI_AHDR("%s:Enter!\n", __FUNCTION__);
    float value = 0.0;
    float clipratio0 = (float)(pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_clipratio0) / 256.0;
    float clipgap0 = pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_clipgap0 / 4.0;
    unsigned short returnValue;


    value = set_lgmin * (1 - clipratio0) + set_lgmax * clipratio0;
    value = MIN(value, (set_lgmin + clipgap0));
    returnValue = (int)SHIFT11BIT(value);

#if LRK_DEBUG_LOG
    LOGE_AHDR( "%s:lrk set_lgmin:%f set_lgmax:%f clipratio0:%f clipgap0:%f value:%f returnValue:%d\n", __FUNCTION__, set_lgmin, set_lgmax, clipratio0, clipgap0, value, returnValue);
#endif

    return returnValue;
    LOGI_AHDR("%s:Eixt!\n", __FUNCTION__);

}
/******************************************************************************
 * GetSetLgRange1()
 *****************************************************************************/
unsigned short GetSetLgRange1(AhdrHandle_t pAhdrCtx, float set_lgmin, float set_lgmax)
{
    LOGI_AHDR("%s:Enter!\n", __FUNCTION__);
    float value = 0.0;
    float clipratio1 = pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_clipratio1 / 256.0;
    float clipgap1 = pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_clipgap1 / 4.0;
    unsigned short returnValue;

    value = set_lgmin * (1 - clipratio1) + set_lgmax * clipratio1;
    value = MAX(value, (set_lgmax - clipgap1));
    returnValue = (int)SHIFT11BIT(value);

#if LRK_DEBUG_LOG
    LOGE_AHDR( "%s:lrk set_lgmin:%f set_lgmax:%f clipratio1:%f clipgap1:%f value:%f returnValue:%d\n", __FUNCTION__, set_lgmin, set_lgmax, clipratio1, clipgap1, value, returnValue);
#endif

    return returnValue;
    LOGI_AHDR("%s:Eixt!\n", __FUNCTION__);

}
/******************************************************************************
 * GetSetPalhpa()
 *****************************************************************************/
unsigned short GetSetPalhpa(AhdrHandle_t pAhdrCtx, float set_lgmin, float set_lgmax)
{
    LOGI_AHDR("%s:Enter!\n", __FUNCTION__);

    float index = 0.0;
    float value = 0.0;
    float set_lgmean = pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_set_lgmean / 2048.0;
    float palpha_0p18 = pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_palpha_0p18 / 1024.0;
    unsigned short returnValue;

    index = 2 * set_lgmean - set_lgmin - set_lgmax;
    index = index / (set_lgmax - set_lgmin);
    value = palpha_0p18 * pow(4, index);
    returnValue = (int)SHIFT10BIT(value);

#if LRK_DEBUG_LOG
    LOGE_AHDR( "%s:lrk set_lgmin:%f set_lgmax:%f set_lgmean:%f palpha_0p18:%f value:%f returnValue:%d\n", __FUNCTION__, set_lgmin, set_lgmax, set_lgmean, palpha_0p18, value, returnValue);
#endif

    return returnValue;
    LOGI_AHDR("%s:Eixt!\n", __FUNCTION__);

}
/******************************************************************************
 * GetCurrIOData()
 *****************************************************************************/
void TmoGetCurrIOData
(
    AhdrHandle_t pAhdrCtx
)
{
    LOGI_AHDR("%s:Enter!\n", __FUNCTION__);

    //default IO data
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_clipratio0 = 64;
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_clipratio1 = 166;
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_clipgap0 = 12;
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_clipgap1 = 12;
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_ratiol = 32;
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_hist_high = (int)(pAhdrCtx->height * pAhdrCtx->width * 0.01 / 16);
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_hist_min = (int)(pAhdrCtx->height * pAhdrCtx->width * 0.01 / 16);
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_hist_0p3 = 614;
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_hist_shift = 3;
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_gain_ld_off1 = 10;
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_gain_ld_off2 = 5;

    //IO data from config
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_maxpalpha = (int)(pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeMaxLuma + 0.5);
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_palpha_0p18 =  (int)(pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeLuma + 0.5);
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_palpha_lw0p5 = (int)(pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsHighLight + 0.5);
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_palpha_lwscl = (int)(pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsLowLight + 0.5);
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_hist_low = (int)(pAhdrCtx->CurrHandleData.CurrTmoHandleData.NoiseLowLight * pAhdrCtx->height * pAhdrCtx->width / (16 * 1000) + 0.5);

    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_cfg_alpha = (int)(pAhdrCtx->CurrHandleData.CurrTmoHandleData.SmoothControlCoef1 + 0.5);
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_set_weightkey = (int)(pAhdrCtx->CurrHandleData.CurrTmoHandleData.SmoothControlCoef2 + 0.5);

    //calc other IO data
    float lgmax = 12 + log(pAhdrCtx->CurrHandleData.CurrL2S_Ratio) / log(2);
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_lgmax = (int)SHIFT11BIT(lgmax);
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_lgscl = (int)SHIFT12BIT(16 / lgmax);
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_lgscl_inv = (int)SHIFT12BIT(lgmax / 16);
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_maxgain = 4096;

    if (pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_cfg_alpha > 0 && pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_cfg_alpha <= 255)
    {
        float set_lgmin = 0;
        float set_lgmax = lgmax;
        pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_set_lgmin = (int)SHIFT11BIT(set_lgmin) ;
        pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_set_lgmax = pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_lgmax;
        pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_set_gainoff = 0;
        pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_set_lgmean = GetSetLgmean(pAhdrCtx);
        pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_set_lgrange0 = GetSetLgRange0(pAhdrCtx, set_lgmin, set_lgmax);
        pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_set_lgrange1 = GetSetLgRange1(pAhdrCtx, set_lgmin, set_lgmax);
        pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_set_lgavgmax = GetSetLgAvgMax(pAhdrCtx, set_lgmin, set_lgmax);
        pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_set_palpha = GetSetPalhpa(pAhdrCtx, set_lgmin, set_lgmax);

    }
    else
    {
        pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_set_lgmin = 1;
        pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_set_lgmax = 1;
        pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_set_gainoff = 1;
        pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_set_lgmean = 1;
        pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_set_lgrange0 = 1;
        pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_set_lgrange1 = 1;
        pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_set_lgavgmax = 1;
        pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_set_palpha = 1;
    }
#if 1
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_big_en = pAhdrCtx->height >= 2560 ? 1 : 0;
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_nobig_en = (int)(1 - pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_big_en);
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_newhist_en = 0;
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_cnt_mode = 0;
    pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_cnt_vsize = (int)(pAhdrCtx->height * 0.9);
    if (pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_newhist_en == 1)
    {
        pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_expl_lgratio = (int)SHIFT11BIT(log(pAhdrCtx->AhdrPrevData.PreLExpo / pAhdrCtx->CurrHandleData.CurrLExpo) / log(2));
        pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_lgscl_ratio = (int)SHIFT7BIT(log(pAhdrCtx->CurrHandleData.CurrL2S_Ratio) / log(pAhdrCtx->AhdrPrevData.PreL2S_ratio));
    }
    else
    {
        pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_expl_lgratio = 1;
        pAhdrCtx->AhdrIOData.TmoData.sw_hdrtmo_lgscl_ratio = 1;
    }
#endif
#if LRK_DEBUG_LOG
    LOGE_AHDR("%s:lrk  Tmo set IOdata to register:\n", __FUNCTION__);
    LOGE_AHDR("%s:lrk lrk  float lgmax:%f\n", __FUNCTION__, lgmax);
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

    LOGI_AHDR("%s:Eixt!\n", __FUNCTION__);
}

/******************************************************************************
 * TMOProcessing()
 *****************************************************************************/
void TmoProcessing
(
    AhdrHandle_t pAhdrCtx
)
{
    LOGI_AHDR("%s:Enter!\n", __FUNCTION__);
    float tmo_damp = pAhdrCtx->CurrHandleData.TmoDamp;

    //get finnal cfg data by damp
    pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeMaxLuma = tmo_damp * pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeMaxLuma + (1 - tmo_damp) * pAhdrCtx->AhdrPrevData.PrevTmoHandleData.GlobeMaxLuma;
    pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeLuma = tmo_damp * pAhdrCtx->CurrHandleData.CurrTmoHandleData.GlobeLuma + (1 - tmo_damp) * pAhdrCtx->AhdrPrevData.PrevTmoHandleData.GlobeLuma;
    pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsHighLight = tmo_damp * pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsHighLight + (1 - tmo_damp) * pAhdrCtx->AhdrPrevData.PrevTmoHandleData.DetailsHighLight;
    pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsLowLight = tmo_damp * pAhdrCtx->CurrHandleData.CurrTmoHandleData.DetailsLowLight + (1 - tmo_damp) * pAhdrCtx->AhdrPrevData.PrevTmoHandleData.DetailsLowLight;
    pAhdrCtx->CurrHandleData.CurrTmoHandleData.NoiseLowLight = tmo_damp * pAhdrCtx->CurrHandleData.CurrTmoHandleData.NoiseLowLight + (1 - tmo_damp) * pAhdrCtx->AhdrPrevData.PrevTmoHandleData.NoiseLowLight;
    pAhdrCtx->CurrHandleData.CurrTmoHandleData.SmoothControlCoef1 = tmo_damp * pAhdrCtx->CurrHandleData.CurrTmoHandleData.SmoothControlCoef1 + (1 - tmo_damp) * pAhdrCtx->AhdrPrevData.PrevTmoHandleData.SmoothControlCoef1;
    pAhdrCtx->CurrHandleData.CurrTmoHandleData.SmoothControlCoef2 = tmo_damp * pAhdrCtx->CurrHandleData.CurrTmoHandleData.SmoothControlCoef2 + (1 - tmo_damp) * pAhdrCtx->AhdrPrevData.PrevTmoHandleData.SmoothControlCoef2;

    //get current IO data
    TmoGetCurrIOData(pAhdrCtx);

    LOGI_AHDR("%s:Eixt!\n", __FUNCTION__);
}
