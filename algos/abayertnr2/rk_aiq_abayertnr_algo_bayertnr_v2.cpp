

#include "rk_aiq_abayertnr_algo_bayertnr_v2.h"


RKAIQ_BEGIN_DECLARE

Abayertnr_result_V2_t bayertnr_select_params_by_ISO_V2(RK_Bayertnr_Params_V2_t *pParams, RK_Bayertnr_Params_V2_Select_t *pSelect, Abayertnr_ExpInfo_V2_t *pExpInfo)
{
    Abayertnr_result_V2_t res = ABAYERTNRV2_RET_SUCCESS;
    int iso = 50;

    if(pParams == NULL) {
        LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
        return ABAYERTNRV2_RET_NULL_POINTER;
    }

    if(pSelect == NULL) {
        LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
        return ABAYERTNRV2_RET_NULL_POINTER;
    }

    if(pExpInfo == NULL) {
        LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
        return ABAYERTNRV2_RET_NULL_POINTER;
    }

    iso = pExpInfo->arIso[pExpInfo->hdr_mode];

    LOGD_ANR("%s:%d iso:%d \n", __FUNCTION__, __LINE__, iso);

    int isoGainStd[RK_BAYERNR_V2_MAX_ISO_NUM];
    int isoGain = MAX(int(iso / 50), 1);
    int isoGainLow = 0;
    int isoGainHig = 0;
    int isoGainCorrect = 1;
    int isoLevelLow = 0;
    int isoLevelHig = 0;
    int isoLevelCorrect = 0;
    int i, j;
    float tmpf;

#ifndef RK_SIMULATOR_HW
    for(int i = 0; i < RK_BAYERNR_V2_MAX_ISO_NUM; i++) {
        isoGainStd[i] = pParams->iso[i] / 50;
    }
#else
    for(int i = 0; i < RK_BAYERNR_V2_MAX_ISO_NUM; i++) {
        isoGainStd[i] = 1 * (1 << i);
    }
#endif

    for (i = 0; i < RK_BAYERNR_V2_MAX_ISO_NUM - 1; i++) {
        if (isoGain >= isoGainStd[i] && isoGain <= isoGainStd[i + 1]) {
            isoGainLow = isoGainStd[i];
            isoGainHig = isoGainStd[i + 1];
            isoLevelLow = i;
            isoLevelHig = i + 1;
            isoGainCorrect = ((isoGain - isoGainStd[i]) <= (isoGainStd[i + 1] - isoGain)) ? isoGainStd[i] : isoGainStd[i + 1];
            isoLevelCorrect = ((isoGain - isoGainStd[i]) <= (isoGainStd[i + 1] - isoGain)) ? i : (i + 1);
            break;
        }
    }

    if(iso > pParams->iso[RK_BAYERNR_V2_MAX_ISO_NUM - 1]) {
        isoGainLow = isoGainStd[RK_BAYERNR_V2_MAX_ISO_NUM - 2];
        isoGainHig = isoGainStd[RK_BAYERNR_V2_MAX_ISO_NUM - 1];
        isoLevelLow = RK_BAYERNR_V2_MAX_ISO_NUM - 2;
        isoLevelHig = RK_BAYERNR_V2_MAX_ISO_NUM - 1;
        isoGainCorrect = isoGainStd[RK_BAYERNR_V2_MAX_ISO_NUM - 1];
        isoLevelCorrect = RK_BAYERNR_V2_MAX_ISO_NUM - 1;
    }

    if(iso < pParams->iso[0]) {
        isoGainLow = isoGainStd[0];
        isoGainHig = isoGainStd[1];
        isoLevelLow = 0;
        isoLevelHig = 1;
        isoGainCorrect = isoGainStd[0];
        isoLevelCorrect = 0;
    }

    LOGD_ANR("%s:%d iso:%d high:%d low:%d\n",
             __FUNCTION__, __LINE__,
             isoGain, isoGainHig, isoGainLow);

    pSelect->bayertnrv2_tnr_enable = pParams->bayertnrv2_tnr_enable;

    pSelect->bayertnrv2_lo_filter_strength =
        float(isoGainHig - isoGain) / float(isoGainHig - isoGainLow) * pParams->bayertnrv2_lo_filter_strength_r[isoLevelLow]
        + float(isoGain - isoGainLow) / float(isoGainHig - isoGainLow) * pParams->bayertnrv2_lo_filter_strength_r[isoLevelHig];

    pSelect->bayertnrv2_hi_filter_strength =
        float(isoGainHig - isoGain) / float(isoGainHig - isoGainLow) * pParams->bayertnrv2_hi_filter_strength_r[isoLevelLow]
        + float(isoGain - isoGainLow) / float(isoGainHig - isoGainLow) * pParams->bayertnrv2_hi_filter_strength_r[isoLevelHig];

    pSelect->bayertnrv2_filter_soft_threshold_ratio =
        float(isoGainHig - isoGain) / float(isoGainHig - isoGainLow) * pParams->bayertnrv2_filter_soft_threshold_ratio_r[isoLevelLow]
        + float(isoGain - isoGainLow) / float(isoGainHig - isoGainLow) * pParams->bayertnrv2_filter_soft_threshold_ratio_r[isoLevelHig];


    tmpf =
        float(isoGainHig - isoGain) / float(isoGainHig - isoGainLow) * pParams->bayertnrv2_thumbds_r[isoLevelLow]
        + float(isoGain - isoGainLow) / float(isoGainHig - isoGainLow) * pParams->bayertnrv2_thumbds_r[isoLevelHig];
    pSelect->bayertnrv2_tnr_sw_thumbds = ((int)tmpf) & (~0x7);

    tmpf =
        float(isoGainHig - isoGain) / float(isoGainHig - isoGainLow) * pParams->bayertnrv2_lo_ena_r[isoLevelLow]
        + float(isoGain - isoGainLow) / float(isoGainHig - isoGainLow) * pParams->bayertnrv2_lo_ena_r[isoLevelHig];
    pSelect->bayertnrv2_tnr_sw_lo_ena = ((int)tmpf) != 0;

    tmpf =
        float(isoGainHig - isoGain) / float(isoGainHig - isoGainLow) * pParams->bayertnrv2_hi_ena_r[isoLevelLow]
        + float(isoGain - isoGainLow) / float(isoGainHig - isoGainLow) * pParams->bayertnrv2_hi_ena_r[isoLevelHig];
    pSelect->bayertnrv2_tnr_sw_hi_ena = ((int)tmpf) != 0;


    for (i = 0; i < 16; i++)
    {
        pSelect->bayertnrv2_tnr_luma_point[i] = pParams->bayertnrv2_lumapoint_r[i];
        pSelect->bayertnrv2_tnr_sigma[i] =
            float(isoGainHig - isoGain) / float(isoGainHig - isoGainLow) * pParams->bayertnrv2_sigma_r[isoLevelLow][i]
            + float(isoGain - isoGainLow) / float(isoGainHig - isoGainLow) * pParams->bayertnrv2_sigma_r[isoLevelHig][i];
    }

    for (i = 0; i < 16; i++)
    {
        pSelect->bayertnrv2_tnr_sw_wgt_lum[i] = pParams->bayertnrv2_lumapoint2_r[i];
        pSelect->bayertnrv2_tnr_sw_lo_sig[i] =
            float(isoGainHig - isoGain) / float(isoGainHig - isoGainLow) * pParams->bayertnrv2_lo_sigma_r[isoLevelLow][i]
            + float(isoGain - isoGainLow) / float(isoGainHig - isoGainLow) * pParams->bayertnrv2_lo_sigma_r[isoLevelHig][i];
        pSelect->bayertnrv2_tnr_sw_hi_sig[i] =
            float(isoGainHig - isoGain) / float(isoGainHig - isoGainLow) * pParams->bayertnrv2_hi_sigma_r[isoLevelLow][i]
            + float(isoGain - isoGainLow) / float(isoGainHig - isoGainLow) * pParams->bayertnrv2_hi_sigma_r[isoLevelHig][i];
    }
    return res;
}


unsigned short bayertnr_get_trans_V2(int tmpfix)
{
    int logtablef[65] = {0, 1465, 2909, 4331, 5731, 7112, 8472, 9813, 11136, 12440,
                         13726, 14995, 16248, 17484, 18704, 19908, 21097, 22272, 23432, 24578, 25710,
                         26829, 27935, 29028, 30109, 31177, 32234, 33278, 34312, 35334, 36345, 37346,
                         38336, 39315, 40285, 41245, 42195, 43136, 44068, 44990, 45904, 46808, 47704,
                         48592, 49472, 50343, 51207, 52062, 52910, 53751, 54584, 55410, 56228, 57040,
                         57844, 58642, 59433, 60218, 60996, 61768, 62534, 63293, 64047, 64794, 65536
                        };
    int logprecision = 6;
    int logfixbit = 16;
    int logtblbit = 16;
    int logscalebit = 12;
    int logfixmul = (1 << logfixbit);
    long long x8, one = 1;
    long long gx, n = 0, ix1, ix2, dp;
    long long lt1, lt2, dx, fx;
    int i, j = 1;

    x8 = tmpfix + (1 << 8);
    // find highest bit
    for (i = 0; i < 32; i++)
    {
        if (x8 & j)
        {
            n = i;
        }
        j = j << 1;
    }

    gx = x8 - (one << n);
    gx = gx * (one << logprecision) * logfixmul;
    gx = gx / (one << n);

    ix1 = gx >> logfixbit;
    dp = gx - ix1 * logfixmul;

    ix2 = ix1 + 1;

    lt1 = logtablef[ix1];
    lt2 = logtablef[ix2];

    dx = lt1 * (logfixmul - dp) + lt2 * dp;

    fx = dx + (n - 8) * (one << (logfixbit + logtblbit));
    fx = fx + (one << (logfixbit + logtblbit - logscalebit - 1));
    fx = fx >> (logfixbit + logtblbit - logscalebit);

    return fx;
}

Abayertnr_result_V2_t bayertnr_fix_transfer_V2(RK_Bayertnr_Params_V2_Select_t* pSelect, RK_Bayertnr_Fix_V2_t *pFix, float fStrength, Abayertnr_ExpInfo_V2_t *pExpInfo)
{
    int i = 0;
    int tmp;

    if(fStrength <= 0.0f) {
        fStrength = 0.000001;
    }

    // BAY3D_BAY3D_CTRL 0x2c00
    pFix->bay3d_soft_st = 0;
    pFix->bay3d_soft_mode = 0;
    pFix->bay3d_bwsaving_en = 0;
    pFix->bay3d_loswitch_protect = 0;
    pFix->bay3d_glbpk_en = 0;
    pFix->bay3d_logaus3_bypass_en = 0;
    pFix->bay3d_logaus5_bypass_en = 0;
    pFix->bay3d_lomed_bypass_en = 0;
    pFix->bay3d_hichnsplit_en = pSelect->bayertnrv2_tnr_sw_lo_ena == 0;
    pFix->bay3d_hiabs_pssel = pSelect->bayertnrv2_tnr_sw_lo_ena == 0;
    pFix->bay3d_higaus_bypass_en = 0;
    pFix->bay3d_himed_bypass_en = 0;
    pFix->bay3d_lobypass_en = !pSelect->bayertnrv2_tnr_sw_lo_ena;
    pFix->bay3d_hibypass_en = !pSelect->bayertnrv2_tnr_sw_hi_ena;
    pFix->bay3d_bypass_en = !pSelect->bayertnrv2_tnr_enable;;
    pFix->bay3d_en_i = pSelect->bayertnrv2_tnr_enable;

    // BAY3D_BAY3D_KALRATIO 0x2c04

    tmp = (int)(pSelect->bayertnrv2_filter_soft_threshold_ratio * (1 << 10) );
    pFix->bay3d_softwgt = CLIP(tmp, 0, 0x3ff);
    pFix->bay3d_hidif_th = 32767;

    // BAY3D_BAY3D_GLBPK2 0x2c08
    pFix->bay3d_glbpk2 = 1024;

    // BAY3D_BAY3D_WGTLMT 0x2c10
    pFix->bay3d_wgtlmt = (int)(((float)1 - (float)0.03215) * (1 << FIXTNRWGT));
    pFix->bay3d_wgtratio = pSelect->bayertnrv2_tnr_sw_hi_ena ? (int)(((float)0.16) * (1 << 10)) : 0;


    for(int i = 0; i < 16; i++) {
        // BAY3D_BAY3D_SIG_X0  0x2c14 - 0x2c30
        tmp = bayertnr_get_trans_V2(pSelect->bayertnrv2_tnr_luma_point[i]);
        pFix->bay3d_sig0_x[i] = CLIP(tmp, 0, 0xffff);

        // BAY3D_BAY3D_SIG0_Y0 0x2c34 - 0x2c50
        tmp = pSelect->bayertnrv2_tnr_sigma[i];
        pFix->bay3d_sig0_y[i] = CLIP(tmp, 0, 0x3fff);

        // BAY3D_BAY3D_SIG_X0  0x2c54 - 0x2c70
        tmp = pSelect->bayertnrv2_tnr_sw_wgt_lum[i];
        pFix->bay3d_sig1_x[i] = CLIP(tmp, 0, 0xffff);

        // BAY3D_BAY3D_SIG1_Y0 0x2c74 - 0x2c90
        tmp = pSelect->bayertnrv2_tnr_sw_hi_sig[i] * pSelect->bayertnrv2_hi_filter_strength;
        pFix->bay3d_sig1_y[i] = CLIP(tmp, 0, (1 << 14) - 1);

        // BAY3D_BAY3D_SIG2_Y0 0x2c94 - 0x2cb0
        tmp = pSelect->bayertnrv2_tnr_sw_lo_sig[i] * pSelect->bayertnrv2_lo_filter_strength;
        pFix->bay3d_sig2_y[i] = CLIP(tmp, 0, (1 << 10) - 1);
    }

    //BAY3D_BAY3D_LODIF_STAT0 0x2cb4 -0x2cb8
    //pFix->ro_sum_lodif = ;

    //BAY3D_BAY3D_LODIF_STAT0 0x2cbc -0x2cc0
    //pFix->ro_sum_hidif0;

    //BAY3D_BAY3D_MI_ST 0x2CC8
    //pFix->sw_bay3dmi_st_linemode;
    //pFix->sw_bay3d_mi2cur_linecnt;

    bayertnr_fix_printf_V2(pFix);

    return ABAYERTNRV2_RET_SUCCESS;
}


Abayertnr_result_V2_t bayertnr_fix_printf_V2(RK_Bayertnr_Fix_V2_t * pFix)
{
    //FILE *fp = fopen("bayernr_regsiter.dat", "wb+");
    Abayertnr_result_V2_t res = ABAYERTNRV2_RET_SUCCESS;

    if(pFix == NULL) {
        LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
        return ABAYERTNRV2_RET_NULL_POINTER;
    }

    LOGD_ANR("%s:(%d) ############# bayertnr enter######################## \n", __FUNCTION__, __LINE__);


    // BAY3D_BAY3D_CTRL (0x002c00)
    LOGD_ANR("(0x2c00) soft_st:0x%x soft_mode:0x%x bwsaving_en:0x%x loswitch_protect:0x%x glbpk_en:0x%x\n",
             pFix->bay3d_soft_st,
             pFix->bay3d_soft_mode,
             pFix->bay3d_bwsaving_en,
             pFix->bay3d_loswitch_protect,
             pFix->bay3d_glbpk_en);

    LOGD_ANR("(0x2c00) logaus3_bypass_en:0x%x logaus5_bypass_en:0x%x lomed_bypass_en:0x%x hichnsplit_en:0x%x glbpk_en:0x%x\n",
             pFix->bay3d_logaus3_bypass_en,
             pFix->bay3d_logaus5_bypass_en,
             pFix->bay3d_lomed_bypass_en,
             pFix->bay3d_hichnsplit_en,
             pFix->bay3d_hiabs_pssel);

    LOGD_ANR("(0x2c00) higaus_bypass_en:0x%x himed_bypass_en:0x%x lobypass_en,:0x%x hibypass_en:0x%x bypass_en:0x%x en:0x%x\n",
             pFix->bay3d_higaus_bypass_en,
             pFix->bay3d_himed_bypass_en,
             pFix->bay3d_lobypass_en,
             pFix->bay3d_hibypass_en,
             pFix->bay3d_bypass_en,
             pFix->bay3d_en_i);

    // BAY3D_BAY3D_KALRATIO 0x2c04
    LOGD_ANR("(0x2c04) bay3d_softwgt:0x%x bay3d_hidif_th:0x%x \n",
             pFix->bay3d_softwgt,
             pFix->bay3d_hidif_th);

    // BAY3D_BAY3D_GLBPK2 0x2c08
    LOGD_ANR("(0x2c08) bay3d_glbpk2:0x%x \n",
             pFix->bay3d_glbpk2);

    // BAY3D_BAY3D_WGTLMT 0x2c10
    LOGD_ANR("(0x2c10) bay3d_wgtlmt:0x%x bay3d_wgtratio:0x%x \n",
             pFix->bay3d_wgtlmt,
             pFix->bay3d_wgtratio);

    // BAY3D_BAY3D_SIG_X0  0x2c14 - 0x2c30
    for(int i = 0; i < 16; i++) {
        LOGD_ANR("(0x2c14 - 0x2c30) bay3d_sig0_x[%d]:0x%x \n",
                 i, pFix->bay3d_sig0_x[i]);
    }

    // BAY3D_BAY3D_SIG0_Y0 0x2c34 - 0x2c50
    for(int i = 0; i < 16; i++) {
        LOGD_ANR("(0x2c34 - 0x2c50) bay3d_sig0_y[%d]:0x%x \n",
                 i, pFix->bay3d_sig0_y[i]);
    }

    // BAY3D_BAY3D_SIG_X0  0x2c54 - 0x2c70
    for(int i = 0; i < 16; i++) {
        LOGD_ANR("(0x2c54 - 0x2c70) bay3d_sig1_x[%d]:0x%x \n",
                 i, pFix->bay3d_sig1_x[i]);
    }

    // // BAY3D_BAY3D_SIG1_Y0 0x2c74 - 0x2c90
    for(int i = 0; i < 16; i++) {
        LOGD_ANR("(0x2c74 - 0x2c90) bay3d_sig1_y[%d]:0x%x \n",
                 i, pFix->bay3d_sig1_y[i]);
    }

    // BAY3D_BAY3D_SIG2_Y0 0x2c94 - 0x2cb0
    for(int i = 0; i < 16; i++) {
        LOGD_ANR("(0x2c94 - 0x2cb0) bay3d_sig2_y[%d]:0x%x \n",
                 i, pFix->bay3d_sig2_y[i]);
    }

    LOGD_ANR("%s:(%d) ############# bayertnr exit ######################## \n", __FUNCTION__, __LINE__);
    return res;
}



Abayertnr_result_V2_t bayertnr_get_setting_by_name_json_V2(CalibDbV2_BayerTnr_V2_t* pCalibdb, char *name, int *calib_idx, int *tuning_idx)
{
    int i = 0;
    Abayertnr_result_V2_t res = ABAYERTNRV2_RET_SUCCESS;

    if(pCalibdb == NULL || name == NULL || calib_idx == NULL || tuning_idx == NULL) {
        LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
        return ABAYERTNRV2_RET_NULL_POINTER;
    }

    for(i = 0; i < pCalibdb->TuningPara.Setting_len; i++) {
        if(strncmp(name, pCalibdb->TuningPara.Setting[i].SNR_Mode, strlen(name)*sizeof(char)) == 0) {
            break;
        }
    }

    if(i < pCalibdb->TuningPara.Setting_len) {
        *tuning_idx = i;
    } else {
        *tuning_idx = 0;
    }

    for(i = 0; i < pCalibdb->CalibPara.Setting_len; i++) {
        if(strncmp(name, pCalibdb->CalibPara.Setting[i].SNR_Mode, strlen(name)*sizeof(char)) == 0) {
            break;
        }
    }

    if(i < pCalibdb->CalibPara.Setting_len) {
        *calib_idx = i;
    } else {
        *calib_idx = 0;
    }

    LOGD_ANR("%s:%d snr_name:%s  snr_idx:%d i:%d \n", __FUNCTION__, __LINE__, name, *calib_idx, i);
    return res;


}


Abayertnr_result_V2_t bayertnr_init_params_json_V2(RK_Bayertnr_Params_V2_t *pParams, CalibDbV2_BayerTnr_V2_t* pCalibdb, int calib_idx, int tuning_idx)
{
    Abayertnr_result_V2_t res = ABAYERTNRV2_RET_SUCCESS;
    CalibDbV2_BayerTnr_V2_CalibPara_Setting_ISO_t *pCalibIso = NULL;
    CalibDbV2_BayerTnr_V2_TuningPara_Setting_ISO_t *pTuningIso = NULL;

    LOGI_ANR("%s:(%d) oyyf bayerner xml config start\n", __FUNCTION__, __LINE__);
    if(pParams == NULL || pCalibdb == NULL) {
        LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
        return ABAYERTNRV2_RET_NULL_POINTER;
    }

    for(int i = 0; i < pCalibdb->CalibPara.Setting[calib_idx].Calib_ISO_len && i < RK_BAYERNR_V2_MAX_ISO_NUM; i++) {
        pCalibIso = &pCalibdb->CalibPara.Setting[calib_idx].Calib_ISO[i];
        pParams->iso[i] = pCalibIso->iso;
        for(int k = 0; k < 16; k++) {
            pParams->bayertnrv2_lumapoint_r[k] = pCalibIso->lumapoint[k];
            pParams->bayertnrv2_sigma_r[i][k] = pCalibIso->sigma[k];
            pParams->bayertnrv2_lumapoint2_r[k] = pCalibIso->lumapoint2[k];
            pParams->bayertnrv2_lo_sigma_r[i][k] = pCalibIso->lo_sigma[k];
            pParams->bayertnrv2_hi_sigma_r[i][k] = pCalibIso->hi_sigma[k];
        }
    }

    for(int i = 0; i < pCalibdb->TuningPara.Setting[tuning_idx].Tuning_ISO_len && i < RK_BAYERNR_V2_MAX_ISO_NUM; i++) {
        pTuningIso = &pCalibdb->TuningPara.Setting[tuning_idx].Tuning_ISO[i];
        pParams->iso[i] = pTuningIso->iso;
        pParams->bayertnrv2_thumbds_r[i] = pTuningIso->thumbds;
        pParams->bayertnrv2_lo_ena_r[i] = pTuningIso->lo_enable;
        pParams->bayertnrv2_hi_ena_r[i] = pTuningIso->hi_enable;
        pParams->bayertnrv2_lo_filter_strength_r[i] = pTuningIso->lo_filter_strength;
        pParams->bayertnrv2_hi_filter_strength_r[i] = pTuningIso->hi_filter_strength;
        pParams->bayertnrv2_filter_soft_threshold_ratio_r[i] = pTuningIso->soft_threshold_ratio;

    }

    LOGI_ANR("%s:(%d) oyyf bayerner xml config end!   \n", __FUNCTION__, __LINE__);

    return res;
}


Abayertnr_result_V2_t bayertnr_config_setting_param_json_V2(RK_Bayertnr_Params_V2_t *pParams, CalibDbV2_BayerTnr_V2_t* pCalibdbV2, char* param_mode, char * snr_name)
{
    Abayertnr_result_V2_t res = ABAYERTNRV2_RET_SUCCESS;
    int calib_idx = 0;
    int tuning_idx = 0;

    if(pParams == NULL || pCalibdbV2 == NULL
            || param_mode == NULL || snr_name == NULL) {
        LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
        return ABAYERTNRV2_RET_NULL_POINTER;
    }

    res = bayertnr_get_setting_by_name_json_V2(pCalibdbV2, snr_name, &calib_idx, &tuning_idx);
    if(res != ABAYERTNRV2_RET_SUCCESS) {
        LOGW_ANR("%s(%d): error!!!  can't find setting in iq files, use 0 instead\n", __FUNCTION__, __LINE__);

    }

    res = bayertnr_init_params_json_V2(pParams, pCalibdbV2, calib_idx, tuning_idx);
    pParams->bayertnrv2_tnr_enable = pCalibdbV2->TuningPara.enable;
    return res;

}

RKAIQ_END_DECLARE
