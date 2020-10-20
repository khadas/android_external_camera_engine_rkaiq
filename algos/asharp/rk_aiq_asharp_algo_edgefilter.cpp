#include "rk_aiq_asharp_algo_edgefilter.h"

RKAIQ_BEGIN_DECLARE

AsharpResult_t edgefilter_get_mode_cell_idx_by_name(CalibDb_EdgeFilter_t *pCalibdb, char *name, int *mode_idx)
{
	int i = 0;
	AsharpResult_t res = ASHARP_RET_SUCCESS;

	if(pCalibdb == NULL){
		LOGE_ASHARP("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ASHARP_RET_NULL_POINTER;
	}

	if(name == NULL){
		LOGE_ASHARP("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ASHARP_RET_NULL_POINTER;
	}

	if(mode_idx == NULL){
		LOGE_ASHARP("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ASHARP_RET_NULL_POINTER;
	}

	for(i=0; i<CALIBDB_NR_SHARP_SETTING_LEVEL; i++){
		if(strncmp(name, pCalibdb->mode_cell[i].name, sizeof(pCalibdb->mode_cell[i].name)) == 0){
			break;
		}
	}

	if(i<CALIBDB_MAX_MODE_NUM){
		*mode_idx = i;
		res = ASHARP_RET_SUCCESS;
	}else{
		*mode_idx = 0;
		res = ASHARP_RET_FAILURE;
	}

	LOGD_ASHARP("%s:%d mode_name:%s  mode_idx:%d i:%d \n", __FUNCTION__, __LINE__, name, *mode_idx, i);
	return res;

}

AsharpResult_t edgefilter_get_setting_idx_by_name(CalibDb_EdgeFilter_t *pCalibdb, char *name, int mode_idx, int *setting_idx)
{
	int i = 0;
	AsharpResult_t res = ASHARP_RET_SUCCESS;

	if(pCalibdb == NULL){
		LOGE_ASHARP("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ASHARP_RET_NULL_POINTER;
	}

	if(name == NULL){
		LOGE_ASHARP("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ASHARP_RET_NULL_POINTER;
	}

	if(setting_idx == NULL){
		LOGE_ASHARP("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ASHARP_RET_NULL_POINTER;
	}

	for(i=0; i<CALIBDB_NR_SHARP_SETTING_LEVEL; i++){
		if(strncmp(name, pCalibdb->mode_cell[mode_idx].setting[i].snr_mode, sizeof(pCalibdb->mode_cell[mode_idx].setting[i].snr_mode)) == 0){
			break;
		}
	}

	if(i<CALIBDB_NR_SHARP_SETTING_LEVEL){
		*setting_idx = i;
		res = ASHARP_RET_SUCCESS;
	}else{
		*setting_idx = 0;
		res = ASHARP_RET_FAILURE;
	}

	LOGD_ASHARP("%s:%d snr_name:%s  snr_idx:%d i:%d \n", __FUNCTION__, __LINE__, name, *setting_idx, i);
	return res;

}

AsharpResult_t edgefilter_config_setting_param(RKAsharp_EdgeFilter_Params_t *pParams, CalibDb_EdgeFilter_t *pCalibdb, char* param_mode, char* snr_name)
{
	AsharpResult_t res = ASHARP_RET_SUCCESS;
	int mode_idx = 0;
	int setting_idx = 0;

	if(pParams == NULL){
		LOGE_ASHARP("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ASHARP_RET_NULL_POINTER;
	}

	if(pCalibdb == NULL){
		LOGE_ASHARP("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ASHARP_RET_NULL_POINTER;
	}

	res = edgefilter_get_mode_cell_idx_by_name(pCalibdb, param_mode, &mode_idx);
	if(res != ASHARP_RET_SUCCESS){
		LOGW_ASHARP("%s(%d): error!!!  can't find mode name in iq files, use 0 instead\n", __FUNCTION__, __LINE__);
	}

	res = edgefilter_get_setting_idx_by_name(pCalibdb, snr_name, mode_idx,  &setting_idx);
	if(res != ASHARP_RET_SUCCESS){
		LOGW_ASHARP("%s(%d): error!!!  can't find setting in iq files, use 0 instead\n", __FUNCTION__, __LINE__);
	}

	res = init_edgefilter_params(pParams, pCalibdb, mode_idx, setting_idx);

	return res;

}
AsharpResult_t init_edgefilter_params(RKAsharp_EdgeFilter_Params_t *pParams, CalibDb_EdgeFilter_t *pCalibdb, int mode_idx, int setting_idx)
{
    AsharpResult_t res = ASHARP_RET_SUCCESS;
    int i = 0;
    int j = 0;
    int max_iso_step = MAX_ISO_STEP;

    if(pParams == NULL) {
        LOGE_ASHARP("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
        return ASHARP_RET_NULL_POINTER;
    }

    if(pCalibdb == NULL) {
        LOGE_ASHARP("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
        return ASHARP_RET_NULL_POINTER;
    }

	CalibDb_EdgeFilter_Setting_t *pSetting = &pCalibdb->mode_cell[mode_idx].setting[setting_idx];
    for(i = 0; i < max_iso_step; i++) {
#ifndef RK_SIMULATOR_HW
		pParams->iso[i] = pSetting->edgeFilter_iso[i].iso;
		#endif
		pParams->edge_thed[i] = pSetting->edgeFilter_iso[i].edge_thed;
		pParams->smoth4[i] = pSetting->edgeFilter_iso[i].src_wgt;
		pParams->alpha_adp_en[i] = pSetting->edgeFilter_iso[i].alpha_adp_en;
		pParams->l_alpha[i] = pSetting->edgeFilter_iso[i].local_alpha;
		pParams->g_alpha[i] = pSetting->edgeFilter_iso[i].global_alpha;
	}

	for(j=0; j < RK_EDGEFILTER_LUMA_POINT_NUM; j++){
		pParams->enhance_luma_point[j] = pCalibdb->luma_point[j];
	}

	for (i=0; i<max_iso_step; i++){
        for(j = 0; j < RK_EDGEFILTER_LUMA_POINT_NUM; j++){
			pParams->edge_thed_1[i][j] = pSetting->edgeFilter_iso[i].noise_clip[j];
			pParams->clamp_pos_dog[i][j] = pSetting->edgeFilter_iso[i].dog_clip_pos[j];
			pParams->clamp_neg_dog[i][j] = pSetting->edgeFilter_iso[i].dog_clip_neg[j];
			pParams->detail_alpha_dog[i][j] = pSetting->edgeFilter_iso[i].dog_alpha[j];
		}
    }

	for (i=0; i<max_iso_step; i++){
		pParams->dir_min[i] = 0.94;
	}

	// init filter params
	float gaus_luma_coef[RKEDGEFILTER_LUMA_GAU_DIAM * RKEDGEFILTER_LUMA_GAU_DIAM] =
    {
        0.062500, 0.125000, 0.062500,
        0.125000, 0.25, 0.125000,
        0.062500, 0.125000, 0.062500
    };
	float hf_filter_coef[RKEDGEFILTER_GAUS_DIAM * RKEDGEFILTER_GAUS_DIAM] =
    {
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.062500, 0.125000, 0.062500, 0.0,
        0.0, 0.125000, 0.25, 0.125000, 0.0,
        0.0, 0.062500, 0.125000, 0.062500, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0,

    };
	float sharp_guide_filter_coef[RKEDGEFILTER_SHRP_DIAM * RKEDGEFILTER_SHRP_DIAM] =
    {
        -0.062500, -0.125000, -0.062500,
        -0.125000, 1.75, -0.125000,
        -0.062500, -0.125000, -0.062500
    };

	for (i=0; i<max_iso_step; i++){
        int h           = RKEDGEFILTER_LUMA_GAU_DIAM;
        int w           = RKEDGEFILTER_LUMA_GAU_DIAM;
        for(int m = 0; m < h; m++){
            for(int n = 0; n < w; n++)
                pParams->gaus_luma_kernel[i][m * w + n] = gaus_luma_coef[m * w + n];
        }
    }

	memset(&pParams->h0_h_coef_5x5[0][0], 0, sizeof(pParams->h0_h_coef_5x5));
    for (i=0; i<max_iso_step; i++){
        int h = RKEDGEFILTER_DIR_SMTH_DIAM;
        int w = RKEDGEFILTER_DIR_SMTH_DIAM;
        for(int n = 0; n < w; n++)
            pParams->h0_h_coef_5x5[i][2 * w + n] = pSetting->edgeFilter_iso[i].direct_filter_coeff[n];
    }

	for (i=0; i<max_iso_step; i++){
        int h = RKEDGEFILTER_GAUS_DIAM;
        int w = RKEDGEFILTER_GAUS_DIAM;
        for(int m = 0; m < h; m++){
            for(int n = 0; n < w; n++){
                pParams->h_coef_5x5[i][m * w + n] = hf_filter_coef[m * w + n];
            }
        }
    }

	for (i=0; i<max_iso_step; i++){
        int h = RKEDGEFILTER_SHRP_DIAM;
        int w = RKEDGEFILTER_SHRP_DIAM;
        for(int m = 0; m < h; m++){
            for(int n = 0; n < w; n++)
                pParams->gf_coef_3x3[i][m * w + n] = sharp_guide_filter_coef[m * w + n];
        }
    }

	for (i=0;i<max_iso_step;i++)
    {
        int h = RKEDGEFILTER_DOG_DIAM;
        int w = RKEDGEFILTER_DOG_DIAM;
        int iso         = (1 << i) * 50;
        int gain        = (1 << i);

        for(int n = 0; n < w; n++){
            pParams->dog_kernel[i][0* w + n] = pSetting->edgeFilter_iso[i].dog_kernel_row0[n];
			pParams->dog_kernel[i][1* w + n] = pSetting->edgeFilter_iso[i].dog_kernel_row1[n];
			pParams->dog_kernel[i][2* w + n] = pSetting->edgeFilter_iso[i].dog_kernel_row2[n];
			pParams->dog_kernel[i][3* w + n] = pSetting->edgeFilter_iso[i].dog_kernel_row3[n];
			pParams->dog_kernel[i][4* w + n] = pSetting->edgeFilter_iso[i].dog_kernel_row4[n];
        }


    }

	return ASHARP_RET_SUCCESS;
}


AsharpResult_t select_edgefilter_params_by_ISO(RKAsharp_EdgeFilter_Params_t *strkedgefilterParams, RKAsharp_EdgeFilter_Params_Select_t *strkedgefilterParamsSelected, AsharpExpInfo_t *pExpInfo)
{
    int i;
	int gain_high = 0, gain_low=0;
	float ratio = 0.0f;
	int iso_div 			= 50;
    int max_iso_step        = MAX_ISO_STEP;
	int iso = 50;
	int iso_low = iso, iso_high = iso;

	AsharpResult_t res = ASHARP_RET_SUCCESS;

	if(strkedgefilterParams == NULL){
        LOGE_ASHARP("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
        return ASHARP_RET_NULL_POINTER;
    }

    if(strkedgefilterParamsSelected == NULL) {
        LOGE_ASHARP("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
        return ASHARP_RET_NULL_POINTER;
    }

    if(pExpInfo == NULL) {
        LOGE_ASHARP("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
        return ASHARP_RET_NULL_POINTER;
    }

    iso = pExpInfo->arIso[pExpInfo->hdr_mode];

#ifndef RK_SIMULATOR_HW
    for (i = 0; i < max_iso_step - 1; i++) {
        if (iso >= strkedgefilterParams->iso[i] && iso <= strkedgefilterParams->iso[i + 1] ) {
            iso_low = strkedgefilterParams->iso[i];
            iso_high = strkedgefilterParams->iso[i + 1];
            gain_low = i ;
            gain_high = i + 1;
            ratio = (float)(iso - iso_low) / (iso_high - iso_low);
            break;
        }
    }

    if(iso < strkedgefilterParams->iso[0]) {
        iso_low = strkedgefilterParams->iso[0];
        iso_high = strkedgefilterParams->iso[1];
        gain_low = 0 ;
        gain_high = 1;
        ratio = 0;
    }

    if(iso >  strkedgefilterParams->iso[max_iso_step - 1]) {
        iso_low = strkedgefilterParams->iso[max_iso_step - 2];
        iso_high = strkedgefilterParams->iso[max_iso_step - 1];
        gain_low = max_iso_step - 2 ;
        gain_high = max_iso_step - 1;
        ratio = 1;
    }
#else
    for (i = max_iso_step - 1; i >= 0; i--)
    {
        if (iso < iso_div * (2 << i))
        {
            iso_low = iso_div * (2 << (i)) / 2;
            iso_high = iso_div * (2 << i);
        }
    }

    ratio = (float)(iso - iso_low) / (iso_high - iso_low);
    if (iso_low == iso)
    {
        iso_high = iso;
        ratio = 0;
    }
    if (iso_high == iso )
    {
        iso_low = iso;
        ratio = 1;
    }
    gain_high       = (int)(log((float)iso_high / 50) / log((float)2));
    gain_low        = (int)(log((float)iso_low / 50) / log((float)2));

    gain_low        = MIN(MAX(gain_low, 0), max_iso_step - 1);
    gain_high       = MIN(MAX(gain_high, 0), max_iso_step - 1);
#endif

    strkedgefilterParamsSelected->edge_thed    = (short)ROUND_F(INTERP1(strkedgefilterParams->edge_thed   [gain_low], strkedgefilterParams->edge_thed   [gain_high], ratio));
    strkedgefilterParamsSelected->dir_min      = INTERP1(strkedgefilterParams->dir_min     [gain_low], strkedgefilterParams->dir_min     [gain_high], ratio);
    strkedgefilterParamsSelected->smoth4       = INTERP1(strkedgefilterParams->smoth4     [gain_low], strkedgefilterParams->smoth4     [gain_high], ratio);
    strkedgefilterParamsSelected->alpha_adp_en = INTERP1(strkedgefilterParams->alpha_adp_en[gain_low], strkedgefilterParams->alpha_adp_en[gain_high], ratio);
    strkedgefilterParamsSelected->l_alpha      = INTERP1(strkedgefilterParams->l_alpha     [gain_low], strkedgefilterParams->l_alpha     [gain_high], ratio);
    strkedgefilterParamsSelected->g_alpha      = INTERP1(strkedgefilterParams->g_alpha     [gain_low], strkedgefilterParams->g_alpha     [gain_high], ratio);
    for(int i = 0; i < RK_EDGEFILTER_LUMA_POINT_NUM; i++)
    {
        strkedgefilterParamsSelected->enhance_luma_point[i] = strkedgefilterParams->enhance_luma_point[i];
        strkedgefilterParamsSelected->edge_thed_1[i]        = (short)ROUND_F(INTERP1(strkedgefilterParams->edge_thed_1[gain_low][i],           strkedgefilterParams->edge_thed_1[gain_high][i],        ratio));
        strkedgefilterParamsSelected->clamp_pos_gaus[i]     = INTERP1(strkedgefilterParams->clamp_pos_gaus[gain_low][i],        strkedgefilterParams->clamp_pos_gaus[gain_high][i],     ratio);
        strkedgefilterParamsSelected->clamp_neg_gaus[i]     = INTERP1(strkedgefilterParams->clamp_neg_gaus[gain_low][i],        strkedgefilterParams->clamp_neg_gaus[gain_high][i],     ratio);
        strkedgefilterParamsSelected->clamp_pos_dog[i]      = (short)ROUND_F(INTERP1(strkedgefilterParams->clamp_pos_dog[gain_low][i],         strkedgefilterParams->clamp_pos_dog[gain_high][i],      ratio));
        strkedgefilterParamsSelected->clamp_neg_dog[i]      = (short)ROUND_F(INTERP1(strkedgefilterParams->clamp_neg_dog[gain_low][i],         strkedgefilterParams->clamp_neg_dog[gain_high][i],      ratio));
        strkedgefilterParamsSelected->detail_alpha_gaus[i]  = INTERP1(strkedgefilterParams->detail_alpha_gaus[gain_low][i],     strkedgefilterParams->detail_alpha_gaus[gain_high][i],  ratio);
        strkedgefilterParamsSelected->detail_alpha_dog[i]   = INTERP1(strkedgefilterParams->detail_alpha_dog[gain_low][i],      strkedgefilterParams->detail_alpha_dog[gain_high][i],   ratio);
    }
    for(unsigned int i = 0; i < sizeof(strkedgefilterParamsSelected->gaus_luma_kernel)   / sizeof(strkedgefilterParamsSelected->gaus_luma_kernel[0]); i++)
        strkedgefilterParamsSelected->gaus_luma_kernel[i]   = INTERP1(strkedgefilterParams->gaus_luma_kernel [gain_low][i],     strkedgefilterParams->gaus_luma_kernel  [gain_high][i], ratio);
    for(unsigned int i = 0; i < sizeof(strkedgefilterParamsSelected->h0_h_coef_5x5)      / sizeof(strkedgefilterParamsSelected->h0_h_coef_5x5[0]); i++)
        strkedgefilterParamsSelected->h0_h_coef_5x5[i]      = INTERP1(strkedgefilterParams->h0_h_coef_5x5 [gain_low][i],        strkedgefilterParams->h0_h_coef_5x5     [gain_high][i], ratio);
    for(unsigned int i = 0; i < sizeof(strkedgefilterParamsSelected->h1_v_coef_5x5)      / sizeof(strkedgefilterParamsSelected->h1_v_coef_5x5[0]); i++)
        strkedgefilterParamsSelected->h1_v_coef_5x5[i]      = INTERP1(strkedgefilterParams->h1_v_coef_5x5 [gain_low][i],        strkedgefilterParams->h1_v_coef_5x5     [gain_high][i], ratio);
    for(unsigned int i = 0; i < sizeof(strkedgefilterParamsSelected->h2_m_coef_5x5)      / sizeof(strkedgefilterParamsSelected->h2_m_coef_5x5[0]); i++)
        strkedgefilterParamsSelected->h2_m_coef_5x5[i]      = INTERP1(strkedgefilterParams->h2_m_coef_5x5 [gain_low][i],        strkedgefilterParams->h2_m_coef_5x5     [gain_high][i], ratio);
    for(unsigned int i = 0; i < sizeof(strkedgefilterParamsSelected->h3_p_coef_5x5)      / sizeof(strkedgefilterParamsSelected->h3_p_coef_5x5[0]); i++)
        strkedgefilterParamsSelected->h3_p_coef_5x5[i]      = INTERP1(strkedgefilterParams->h3_p_coef_5x5 [gain_low][i],        strkedgefilterParams->h3_p_coef_5x5     [gain_high][i], ratio);
    for(unsigned int i = 0; i < sizeof(strkedgefilterParamsSelected->h_coef_5x5)         / sizeof(strkedgefilterParamsSelected->h_coef_5x5[0]); i++)
        strkedgefilterParamsSelected->h_coef_5x5[i]         = INTERP1(strkedgefilterParams->h_coef_5x5 [gain_low][i],           strkedgefilterParams->h_coef_5x5        [gain_high][i], ratio);
    for(unsigned int i = 0; i < sizeof(strkedgefilterParamsSelected->gf_coef_3x3)            / sizeof(strkedgefilterParamsSelected->gf_coef_3x3[0]); i++)
        strkedgefilterParamsSelected->gf_coef_3x3[i]        = INTERP1(strkedgefilterParams->gf_coef_3x3 [gain_low][i],          strkedgefilterParams->gf_coef_3x3       [gain_high][i], ratio);
    for(unsigned int i = 0; i < sizeof(strkedgefilterParamsSelected->dog_kernel)         / sizeof(strkedgefilterParamsSelected->dog_kernel[0]); i++)
        strkedgefilterParamsSelected->dog_kernel[i]         = INTERP1(strkedgefilterParams->dog_kernel [gain_low][i],           strkedgefilterParams->dog_kernel        [gain_high][i], ratio);

    return res;
}




AsharpResult_t edgefilter_fix_transfer(RKAsharp_EdgeFilter_Params_Select_t* edgeflt, RKAsharp_Edgefilter_Fix_t* pEdgefilterCfg , float fPercent)
{
    int i = 0;
    int k = 0;
    int tmp = 0;
    int sum_coeff, offset;
    float sum_coeff_float;
    AsharpResult_t res = ASHARP_RET_SUCCESS;

    if(edgeflt == NULL) {
        LOGE_ASHARP("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
        return ASHARP_RET_NULL_POINTER;
    }

    if(pEdgefilterCfg == NULL) {
        LOGE_ASHARP("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
        return ASHARP_RET_NULL_POINTER;
    }

	if(fPercent <= 0.0f){
		fPercent = 0.000001;
	}
	LOGD_ASHARP("%s(%d): fPercent:%f \n", __FUNCTION__, __LINE__, fPercent);

    //0x0080
    pEdgefilterCfg->alpha_adp_en = edgeflt->alpha_adp_en;

    //0x0084

    //0x0088
    tmp = ROUND_F(edgeflt->edge_thed / fPercent );
	if(tmp > 0xff){
		tmp = 0xff;
	}
	pEdgefilterCfg->edge_thed  = tmp;
	pEdgefilterCfg->dir_min = ROUND_F((edgeflt->dir_min ) * (1 << reg_dir_min_fix_bits));
     tmp = ROUND_F((edgeflt->smoth4 + (fPercent -1.0)/(SHARP_MAX_STRENGTH_PERCENT)*1.0 )* (1 << reg_smoth4_fix_bits));
	 if(tmp > 0xff){
		tmp = 0xff;
	 }
	 pEdgefilterCfg->smoth_th4 = (unsigned short)tmp;

    //0x008c
    tmp = ROUND_F(edgeflt->l_alpha / fPercent * (1 << reg_l_alpha_fix_bits));
	if(tmp > 0x1ff){
		tmp = 0x1ff;
	}
	pEdgefilterCfg->l_alpha = (unsigned short)tmp;
	
    tmp = ROUND_F(edgeflt->g_alpha / fPercent * (1 << reg_g_alpha_fix_bits));
	if(tmp > 0x1ff){
		tmp = 0x1ff;
	}
	pEdgefilterCfg->g_alpha = (unsigned short)tmp;

    //0x0090 - 0x00b0

    //0x00b4
    pEdgefilterCfg->eg_coef[0] = (char)ROUND_F(edgeflt->gf_coef_3x3[4] * (1 << reg_gf_coef_3x3_fix_bits));
    pEdgefilterCfg->eg_coef[1] = (char)ROUND_F(edgeflt->gf_coef_3x3[1] * (1 << reg_gf_coef_3x3_fix_bits));
    pEdgefilterCfg->eg_coef[2] = (char)ROUND_F(edgeflt->gf_coef_3x3[0] * (1 << reg_gf_coef_3x3_fix_bits));
    sum_coeff = 0;
    sum_coeff_float = 0;
    for(int k = 0; k < RKEDGEFILTER_SHRP_DIAM        * RKEDGEFILTER_SHRP_DIAM; k ++)
    {
        sum_coeff += ROUND_F(edgeflt->gf_coef_3x3[k] * (1 << reg_gf_coef_3x3_fix_bits));
        sum_coeff_float += edgeflt->gf_coef_3x3[k];
    }
    offset = int(sum_coeff_float * (1 << reg_gf_coef_3x3_fix_bits)) - sum_coeff;
    pEdgefilterCfg->eg_coef[0] = pEdgefilterCfg->eg_coef[0] + offset;

    //0x00b8
    pEdgefilterCfg->eg_smoth[0] = (unsigned char)ROUND_F(edgeflt->h0_h_coef_5x5[2 * RKEDGEFILTER_DIR_SMTH_DIAM + 2] * (1 << reg_h0_h_coef_5x5_fix_bits));
    pEdgefilterCfg->eg_smoth[1] = (unsigned char)ROUND_F(edgeflt->h0_h_coef_5x5[2 * RKEDGEFILTER_DIR_SMTH_DIAM + 1] * (1 << reg_h0_h_coef_5x5_fix_bits));
    pEdgefilterCfg->eg_smoth[2] = (unsigned char)ROUND_F(edgeflt->h0_h_coef_5x5[2 * RKEDGEFILTER_DIR_SMTH_DIAM + 0] * (1 << reg_h0_h_coef_5x5_fix_bits));
    sum_coeff = 0;
    sum_coeff_float = 0;
    for(int k = 0; k < RKEDGEFILTER_DIR_SMTH_DIAM * RKEDGEFILTER_DIR_SMTH_DIAM; k ++)
    {
        sum_coeff += ROUND_F(edgeflt->h0_h_coef_5x5[k] * (1 << reg_h0_h_coef_5x5_fix_bits));
        sum_coeff_float += edgeflt->h0_h_coef_5x5[k];
    }
    offset = int(sum_coeff_float * (1 << reg_h0_h_coef_5x5_fix_bits)) - sum_coeff;
    pEdgefilterCfg->eg_smoth[0] = pEdgefilterCfg->eg_smoth[0] + offset;

    //0x00bc - 0x00c0
    pEdgefilterCfg->eg_gaus[0] = (unsigned char)ROUND_F(edgeflt->h_coef_5x5[12] * (1 << reg_h_coef_5x5_fix_bits));
    pEdgefilterCfg->eg_gaus[1] = (unsigned char)ROUND_F(edgeflt->h_coef_5x5[7] * (1 << reg_h_coef_5x5_fix_bits));
    pEdgefilterCfg->eg_gaus[2] = (unsigned char)ROUND_F(edgeflt->h_coef_5x5[6] * (1 << reg_h_coef_5x5_fix_bits));
    pEdgefilterCfg->eg_gaus[3] = (unsigned char)ROUND_F(edgeflt->h_coef_5x5[2] * (1 << reg_h_coef_5x5_fix_bits));
    pEdgefilterCfg->eg_gaus[4] = (unsigned char)ROUND_F(edgeflt->h_coef_5x5[1] * (1 << reg_h_coef_5x5_fix_bits));
    pEdgefilterCfg->eg_gaus[5] = (unsigned char)ROUND_F(edgeflt->h_coef_5x5[0] * (1 << reg_h_coef_5x5_fix_bits));
    sum_coeff = 0;
    sum_coeff_float = 0;
    for(int k = 0; k < RKEDGEFILTER_GAUS_DIAM * RKEDGEFILTER_GAUS_DIAM; k ++)
    {
        sum_coeff += ROUND_F(edgeflt->h_coef_5x5[k] * (1 << reg_h_coef_5x5_fix_bits));
        sum_coeff_float += edgeflt->h_coef_5x5[k];
    }
    offset = int(sum_coeff_float * (1 << reg_h_coef_5x5_fix_bits)) - sum_coeff;
    pEdgefilterCfg->eg_gaus[0] = pEdgefilterCfg->eg_gaus[0] + offset;

    //0x00c4 - 0x00c8
    pEdgefilterCfg->dog_k[0] = ( char)ROUND_F(edgeflt->dog_kernel[12] * (1 << reg_dog_kernel_fix_bits));
    pEdgefilterCfg->dog_k[1] = ( char)ROUND_F(edgeflt->dog_kernel[7] * (1 << reg_dog_kernel_fix_bits));
    pEdgefilterCfg->dog_k[2] = ( char)ROUND_F(edgeflt->dog_kernel[6] * (1 << reg_dog_kernel_fix_bits));
    pEdgefilterCfg->dog_k[3] = ( char)ROUND_F(edgeflt->dog_kernel[2] * (1 << reg_dog_kernel_fix_bits));
    pEdgefilterCfg->dog_k[4] = ( char)ROUND_F(edgeflt->dog_kernel[1] * (1 << reg_dog_kernel_fix_bits));
    pEdgefilterCfg->dog_k[5] = ( char)ROUND_F(edgeflt->dog_kernel[0] * (1 << reg_dog_kernel_fix_bits));
    sum_coeff = 0;
    sum_coeff_float = 0;
    for(int k = 0; k < RKEDGEFILTER_DOG_DIAM * RKEDGEFILTER_DOG_DIAM; k ++)
    {
        sum_coeff += ROUND_F(edgeflt->dog_kernel[k] * (fPercent -1.0)/SHARP_MAX_STRENGTH_PERCENT * 2 * (1 << reg_dog_kernel_fix_bits));
        sum_coeff_float += edgeflt->dog_kernel[k];
    }
    offset = int(sum_coeff_float * (1 << reg_dog_kernel_fix_bits)) - sum_coeff;
    pEdgefilterCfg->dog_k[0] = pEdgefilterCfg->dog_k[0] + offset;

    //0x00cc -  0x0104

    //0x0108 - 0x010c
    for(i = 0; i < 8; i++) {
		 tmp = ROUND_F(edgeflt->edge_thed_1[i] * fPercent);
		 if(tmp > 0xff){
			tmp = 0xff;
		 }
        pEdgefilterCfg->edge_lum_thed[i] = (unsigned char)tmp;
    }

    //0x0110 - 0x0114
    for(i = 0; i < 8; i++) {
         tmp = ROUND_F(edgeflt->clamp_pos_dog[i] * fPercent);
		 if(tmp > 0xff){
			tmp = 0xff;
		 }
		 pEdgefilterCfg->clamp_pos[i] = (unsigned char)tmp;
    }

    //0x0118 - 0x011c
    for(i = 0; i < 8; i++) {
		tmp = ROUND_F(edgeflt->clamp_neg_dog[i] * fPercent);
		 if(tmp > 0xff){
			tmp = 0xff;
		 }
        pEdgefilterCfg->clamp_neg[i] = (unsigned char)tmp;
    }

    //0x0120 - 0x0124
    for(i = 0; i < 8; i++) {
		 if(fPercent > 1.0){
         	tmp = ROUND_F((edgeflt->detail_alpha_dog[i] + (fPercent - 1.0)/SHARP_MAX_STRENGTH_PERCENT * 3 ) * (1 << reg_detail_alpha_dog_fix_bits));
		 }else{
			tmp = ROUND_F((edgeflt->detail_alpha_dog[i] ) * (1 << reg_detail_alpha_dog_fix_bits));
		 }
		 if(tmp > 0xff){
			tmp = 0xff;
		 }
		 pEdgefilterCfg->detail_alpha[i] = (unsigned char)tmp;
    }

    return res;
}

RKAIQ_END_DECLARE


