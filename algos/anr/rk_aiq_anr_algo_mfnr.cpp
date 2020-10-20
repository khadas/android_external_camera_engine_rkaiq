#include "rk_aiq_anr_algo_mfnr.h"

ANRresult_t mfnr_get_mode_cell_idx_by_name(CalibDb_MFNR_t *pCalibdb, char *name, int *mode_idx)
{
	int i = 0;
	ANRresult_t res = ANR_RET_SUCCESS;

	if(pCalibdb == NULL){
		LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ANR_RET_NULL_POINTER;
	}

	if(name == NULL){
		LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ANR_RET_NULL_POINTER;
	}

	if(mode_idx == NULL){
		LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ANR_RET_NULL_POINTER;
	}

	for(i=0; i<CALIBDB_NR_SHARP_SETTING_LEVEL; i++){
		if(strncmp(name, pCalibdb->mode_cell[i].name, sizeof(pCalibdb->mode_cell[i].name)) == 0){
			break;
		}
	}

	if(i<CALIBDB_MAX_MODE_NUM){
		*mode_idx = i;
		res = ANR_RET_SUCCESS;
	}else{
		*mode_idx = 0;
		res = ANR_RET_FAILURE;
	}

	LOGD_ANR("%s:%d mode_name:%s  mode_idx:%d i:%d \n",__FUNCTION__, __LINE__, name, *mode_idx, i);
	return res;

}


ANRresult_t mfnr_get_setting_idx_by_name(CalibDb_MFNR_t *pCalibdb, char *name, int mode_idx, int *setting_idx)
{
	int i = 0;
	ANRresult_t res = ANR_RET_SUCCESS;

	if(pCalibdb == NULL){
		LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ANR_RET_NULL_POINTER;
	}

	if(name == NULL){
		LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ANR_RET_NULL_POINTER;
	}

	if(setting_idx == NULL){
		LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ANR_RET_NULL_POINTER;
	}

	for(i=0; i<CALIBDB_NR_SHARP_SETTING_LEVEL; i++){
		if(strncmp(name, pCalibdb->mode_cell[mode_idx].setting[i].snr_mode, sizeof(pCalibdb->mode_cell[mode_idx].setting[i].snr_mode)) == 0){
			break;
		}
	}

	if(i<CALIBDB_NR_SHARP_SETTING_LEVEL){
		*setting_idx = i;
		res = ANR_RET_SUCCESS;
	}else{
		*setting_idx = 0;
		res = ANR_RET_FAILURE;
	}

	LOGD_ANR("%s:%d snr_name:%s  snr_idx:%d i:%d \n",__FUNCTION__, __LINE__, name, *setting_idx, i);
	return res;

}

ANRresult_t init_mfnr_dynamic_params(RKAnr_Mfnr_Dynamic_t *pDynamic, CalibDb_MFNR_t *pCalibdb, int mode_idx)
{
    ANRresult_t res = ANR_RET_SUCCESS;
    int i = 0;
    int j = 0;

    if(pDynamic == NULL) {
        LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
        return ANR_RET_NULL_POINTER;
    }

    if(pCalibdb == NULL) {
        LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
        return ANR_RET_NULL_POINTER;
    }

   pDynamic->enable = pCalibdb->mode_cell[mode_idx].dynamic.enable;
   pDynamic->lowth_iso = pCalibdb->mode_cell[mode_idx].dynamic.lowth_iso;
   pDynamic->lowth_time = pCalibdb->mode_cell[mode_idx].dynamic.lowth_time;
   pDynamic->highth_iso = pCalibdb->mode_cell[mode_idx].dynamic.highth_iso;
   pDynamic->highth_time = pCalibdb->mode_cell[mode_idx].dynamic.highth_time;

   LOGD_ANR("dynamic final param mode:%d \n", mode_idx);
   return res;

}

ANRresult_t mfnr_config_dynamic_param(RKAnr_Mfnr_Dynamic_t *pDynamic,  CalibDb_MFNR_t *pCalibdb, char* param_mode)
{
	
	ANRresult_t res = ANR_RET_SUCCESS;
	int mode_idx = 0;
	int setting_idx = 0;

	if(pDynamic == NULL){
		LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ANR_RET_NULL_POINTER;
	}

	if(pCalibdb == NULL){
		LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ANR_RET_NULL_POINTER;
	}

	if(param_mode == NULL){
		LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ANR_RET_NULL_POINTER;
	}
	
	res = mfnr_get_mode_cell_idx_by_name(pCalibdb, param_mode, &mode_idx);
	if(res != ANR_RET_SUCCESS){
		LOGW_ANR("%s(%d): error!!!	can't find mode name in iq files, use 0 instead\n", __FUNCTION__, __LINE__);
	}

	res = init_mfnr_dynamic_params(pDynamic, pCalibdb, mode_idx);

	LOGD_ANR("final param mode:%d snr_mode:%d\n", mode_idx);
	return res;
	
}

ANRresult_t mfnr_config_setting_param(RKAnr_Mfnr_Params_t *pParams, CalibDb_MFNR_t *pCalibdb, char* param_mode, char* snr_name)
{
	ANRresult_t res = ANR_RET_SUCCESS;
	int mode_idx = 0;
	int setting_idx = 0;

	if(pParams == NULL){
		LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ANR_RET_NULL_POINTER;
	}

	if(pCalibdb == NULL){
		LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ANR_RET_NULL_POINTER;
	}

	if(param_mode == NULL){
		LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ANR_RET_NULL_POINTER;
	}

	if(snr_name == NULL){
		LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ANR_RET_NULL_POINTER;
	}
	
	res = mfnr_get_mode_cell_idx_by_name(pCalibdb, param_mode, &mode_idx);
	if(res != ANR_RET_SUCCESS){
		LOGW_ANR("%s(%d): error!!!	can't find mode name in iq files, use 0 instead\n", __FUNCTION__, __LINE__);
	}

	res = mfnr_get_setting_idx_by_name(pCalibdb, snr_name, mode_idx, &setting_idx);
	if(res != ANR_RET_SUCCESS){
		LOGW_ANR("%s(%d): error!!!  can't find setting in iq files, use 0 instead\n", __FUNCTION__, __LINE__);
	}

	res = init_mfnr_params(pParams, pCalibdb, mode_idx, setting_idx);

	LOGD_ANR("final param mode:%d snr_mode:%d\n", mode_idx, setting_idx);
	return res;

}

ANRresult_t init_mfnr_params(RKAnr_Mfnr_Params_t *pParams, CalibDb_MFNR_t *pCalibdb, int mode_idx, int setting_idx)
{
    ANRresult_t res = ANR_RET_SUCCESS;
    int i = 0;
    int j = 0;

    if(pParams == NULL) {
        LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
        return ANR_RET_NULL_POINTER;
    }

    if(pCalibdb == NULL) {
        LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
        return ANR_RET_NULL_POINTER;
    }

    int max_iso_step        = MAX_ISO_STEP;
    int dir_num             = MFNR_DIR_NUM;
    int polyorder           = MFNR_POLYORDER;
    int max_lvl             = MFNR_MAX_LVL;
    int max_lvl_uv          = MFNR_MAX_LVL_UV;
    int lumancurve_step     = LUMANRCURVE_STEP;
    int range               = 1 << Y_CALIBRATION_BITS;
    int dir_lo              = DIR_LO;
    int dir_hi              = DIR_HI;


    max_lvl = pCalibdb->max_level;
    max_lvl_uv = pCalibdb->max_level_uv;
    pParams->back_ref_num = pCalibdb->back_ref_num;

	CalibDb_MFNR_Setting_t *pSetting = &pCalibdb->mode_cell[mode_idx].setting[setting_idx];
    for(int lvl = 0; lvl < max_lvl; lvl++) {
        for (i = 0; i < max_iso_step; i++)
            pParams->weight_limit_y[i][lvl] = pSetting->mfnr_iso[i].weight_limit_y[lvl];
    }

	for(int lvl = 0; lvl < max_lvl_uv; lvl++){
        for (i=0;i<max_iso_step;i++)
            pParams->weight_limit_uv[i][lvl] = pSetting->mfnr_iso[i].weight_limit_uv[lvl];
    }

	for(int j = 0; j < 4; j++){
        for (i=0;i<max_iso_step;i++)
            pParams->ratio_frq[i][j] = pSetting->mfnr_iso[i].ratio_frq[j];
    }

	for(int lvl = 0; lvl < max_lvl_uv; lvl++){
        for (i=0;i<max_iso_step;i++)
            pParams->luma_w_in_chroma[i][lvl] = pSetting->mfnr_iso[i].luma_w_in_chroma[lvl];
    }

	for(j=0; j<4; j++){
		for(i=0; i<2; i++)
			pParams->awb_uv_ratio[j][i] = pCalibdb->uv_ratio[j].ratio[i];
	}

	for(int j = 0; j < polyorder + 1; j++){
        for (i=0;i<max_iso_step;i++)
            pParams->curve[i][j] = pSetting->mfnr_iso[i].noise_curve[j];
    }

	for (i=0;i<max_iso_step;i++){
        pParams->curve_x0[i] = pSetting->mfnr_iso[i].noise_curve_x00;
    }


    for (j = 0; j < max_lvl; j++){
        for (i=0;i<max_iso_step;i++){
            pParams->ci[i][0][j] = pSetting->mfnr_iso[i].y_lo_noiseprofile[j];
			pParams->ci[i][1][j] = pSetting->mfnr_iso[i].y_hi_noiseprofile[j];
            pParams->scale[i][0][j] = pSetting->mfnr_iso[i].y_lo_bfscale[j];
			pParams->scale[i][1][j] = pSetting->mfnr_iso[i].y_hi_bfscale[j];
        }
    }

	for (j = 0; j < lumancurve_step; j++){
        for (i=0;i<max_iso_step;i++){
            pParams->lumanrpoint[i][dir_lo][j] = pSetting->mfnr_iso[i].y_lumanrpoint[j];
			pParams->lumanrcurve[i][dir_lo][j] = pSetting->mfnr_iso[i].y_lumanrcurve[j];
			pParams->lumanrpoint[i][dir_hi][j] = pSetting->mfnr_iso[i].y_lumanrpoint[j];
			pParams->lumanrcurve[i][dir_hi][j] = pSetting->mfnr_iso[i].y_lumanrcurve[j];
        }
    }

	for (i=0;i<max_iso_step;i++){
        pParams->dnstr[i][dir_lo] = pSetting->mfnr_iso[i].y_denoisestrength;
		pParams->dnstr[i][dir_hi] = pParams->dnstr[i][dir_lo];
    }


    for(int j = 0; j < 6; j++){
        for (i=0;i<max_iso_step;i++){
            pParams->gfdelta[i][0][0][j] = pSetting->mfnr_iso[i].y_lo_lvl0_gfdelta[j];
			pParams->gfdelta[i][1][0][j] = pSetting->mfnr_iso[i].y_hi_lvl0_gfdelta[j];
        }
    }

	for(int j = 0; j < 3; j++){
        for (i=0;i<max_iso_step;i++){
			pParams->gfdelta[i][0][1][j] = pSetting->mfnr_iso[i].y_lo_lvl1_gfdelta[j];
			pParams->gfdelta[i][0][2][j] = pSetting->mfnr_iso[i].y_lo_lvl2_gfdelta[j];
			pParams->gfdelta[i][0][3][j] = pSetting->mfnr_iso[i].y_lo_lvl3_gfdelta[j];

			pParams->gfdelta[i][1][1][j] = pSetting->mfnr_iso[i].y_hi_lvl1_gfdelta[j];
			pParams->gfdelta[i][1][2][j] = pSetting->mfnr_iso[i].y_hi_lvl2_gfdelta[j];
			pParams->gfdelta[i][1][3][j] = pSetting->mfnr_iso[i].y_hi_lvl3_gfdelta[j];
        }
    }

    for (j = 0; j < max_lvl_uv; j++){
        for (i=0;i<max_iso_step;i++){
            pParams->ci_uv[i][0][j] = pSetting->mfnr_iso[i].uv_lo_noiseprofile[j];
			pParams->ci_uv[i][1][j] = pSetting->mfnr_iso[i].uv_hi_noiseprofile[j];
            pParams->scale_uv[i][0][j] = pSetting->mfnr_iso[i].uv_lo_bfscale[j];
			pParams->scale_uv[i][1][j] = pSetting->mfnr_iso[i].uv_hi_bfscale[j];

			LOGI_ANR("j:%d i:%d ci:%f %f scale:%f %f\n", j, i,
				pParams->ci_uv[i][0][j],
				pParams->ci_uv[i][1][j],
				pParams->scale_uv[i][0][j],
				pParams->scale_uv[i][1][j]);
        }
    }

	for (j = 0; j < lumancurve_step; j++){
        for (i=0;i<max_iso_step;i++){
            pParams->lumanrpoint_uv[i][dir_lo][j] = pSetting->mfnr_iso[i].uv_lumanrpoint[j];
			pParams->lumanrcurve_uv[i][dir_lo][j] = pSetting->mfnr_iso[i].uv_lumanrcurve[j];
			pParams->lumanrpoint_uv[i][dir_hi][j] = pSetting->mfnr_iso[i].uv_lumanrpoint[j];
			pParams->lumanrcurve_uv[i][dir_hi][j] = pSetting->mfnr_iso[i].uv_lumanrcurve[j];
        }
    }

	for (i=0;i<max_iso_step;i++){
        pParams->dnstr_uv[i][dir_lo] = pSetting->mfnr_iso[i].uv_denoisestrength;
		pParams->dnstr_uv[i][dir_hi] = pParams->dnstr_uv[i][dir_lo];
		LOGI_ANR("%d: dnstr_uv:%f %f\n", i,
			pParams->dnstr_uv[i][dir_lo],
			pParams->dnstr_uv[i][dir_hi]);
    }

	for(int j = 0; j < 6; j++){
        for (i=0;i<max_iso_step;i++){
            pParams->gfdelta_uv[i][0][0][j] = pSetting->mfnr_iso[i].uv_lo_lvl0_gfdelta[j];
			pParams->gfdelta_uv[i][1][0][j] = pSetting->mfnr_iso[i].uv_hi_lvl0_gfdelta[j];
        }
    }

	for(int j = 0; j < 3; j++){
		for (i=0;i<max_iso_step;i++){
			pParams->gfdelta_uv[i][0][1][j] = pSetting->mfnr_iso[i].uv_lo_lvl1_gfdelta[j];
			pParams->gfdelta_uv[i][0][2][j] = pSetting->mfnr_iso[i].uv_lo_lvl2_gfdelta[j];
			pParams->gfdelta_uv[i][1][1][j] = pSetting->mfnr_iso[i].uv_hi_lvl1_gfdelta[j];
			pParams->gfdelta_uv[i][1][2][j] = pSetting->mfnr_iso[i].uv_hi_lvl2_gfdelta[j];
		}
	}


    for(int j = 0; j < 6; j++){
        for (i=0;i<max_iso_step;i++){
            pParams->gfsigma[i][0][j] = pSetting->mfnr_iso[i].lvl0_gfsigma[j];
        }
    }

	for(int j = 0; j < 3; j++){
        for (i=0;i<max_iso_step;i++){
            pParams->gfsigma[i][1][j] = pSetting->mfnr_iso[i].lvl1_gfsigma[j];
			pParams->gfsigma[i][2][j] = pSetting->mfnr_iso[i].lvl2_gfsigma[j];
			pParams->gfsigma[i][3][j] = pSetting->mfnr_iso[i].lvl3_gfsigma[j];
        }
    }

	for (i=0;i<max_iso_step;i++){
        int64_t curve_x0 = (int)pParams->curve_x0[i];
        for (j = 0; j < range; j++){
            int64_t X[5];
            double y;

            X[0] = 1;
            y = pParams->curve[i][polyorder];
            for(int order = 1; order < polyorder + 1; order++){
                X[order] = X[order - 1] * j;
                y = y + (double)X[order] * pParams->curve[i][polyorder-order];
            }

            if(j > curve_x0)
                pParams->noise_sigma[i][j] = pParams->noise_sigma[i][curve_x0];
            else
                pParams->noise_sigma[i][j] = y;
        }
    }

	#ifndef RK_SIMULATOR_HW
	for (i=0;i<max_iso_step;i++){
         pParams->iso[i] = pSetting->mfnr_iso[i].iso;
    	}
	#endif

    //memcpy(pParams->mfnr_ver_char, pCalibdb->version, sizeof(pParams->mfnr_ver_char));
    LOGI_ANR("%s(%d): exit!\n", __FUNCTION__, __LINE__);

    return res;
}


ANRresult_t select_mfnr_params_by_ISO(RKAnr_Mfnr_Params_t *stmfnrParams,    RKAnr_Mfnr_Params_Select_t *stmfnrParamsSelected, ANRExpInfo_t *pExpInfo, int bits_proc)
{
    ANRresult_t res = ANR_RET_SUCCESS;
    int iso = 50;

    if(stmfnrParams == NULL) {
        LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
        return ANR_RET_NULL_POINTER;
    }

    if(stmfnrParamsSelected == NULL) {
        LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
        return ANR_RET_NULL_POINTER;
    }

    if(pExpInfo == NULL) {
        LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
        return ANR_RET_NULL_POINTER;
    }

    iso = pExpInfo->arIso[pExpInfo->hdr_mode];

    int i, j;
    int iso_low = iso, iso_high = iso;
    int gain_high = 0, gain_low = 0;
    double ratio = 0.0f;
    int iso_div             = 50;
    int dir_num             = MFNR_DIR_NUM;
    int polyorder           = MFNR_POLYORDER;
    int max_lvl             = MFNR_MAX_LVL;
    int max_lvl_uv          = MFNR_MAX_LVL_UV;
    int lumancurve_step     = LUMANRCURVE_STEP;
    int range               = (1 << Y_CALIBRATION_BITS);
    int step                = (1 << (Y_CALIBRATION_BITS - bits_proc));
    int step_x              = (1 << (Y_CALIBRATION_BITS - Y_SIGMA_TABLE_BITS));
    int step_x1             = (1 << (Y_CALIBRATION_BITS - bits_proc));
    int step_y              = (1 << (Y_CALIBRATION_BITS - bits_proc));
    int step_x_dehz         = (1 << (Y_CALIBRATION_BITS - Y_SIGMA_TABLE_BITS_DEHAZE));
    double noise_sigma_tmp[(1 << Y_CALIBRATION_BITS) + 1];

#ifndef RK_SIMULATOR_HW
    for (i = 0; i < MAX_ISO_STEP - 1; i++) {
        if(iso >= stmfnrParams->iso[i] && iso_low <= stmfnrParams->iso[i + 1]) {
            iso_low = stmfnrParams->iso[i];
            iso_high = stmfnrParams->iso[i + 1];
            gain_low = i;
            gain_high = i + 1;
            ratio = (double)(iso - iso_low) / (iso_high - iso_low);
            break;
        }
    }

    if(iso < stmfnrParams->iso[0]) {
        iso_low = stmfnrParams->iso[0];
        iso_high = stmfnrParams->iso[1];
        gain_low = 0;
        gain_high = 1;
        ratio = 0;
    }

    if(iso > stmfnrParams->iso[MAX_ISO_STEP - 1]) {
        iso_low = stmfnrParams->iso[MAX_ISO_STEP - 2];
        iso_high = stmfnrParams->iso[MAX_ISO_STEP - 1];
        gain_low = MAX_ISO_STEP - 2;
        gain_high = MAX_ISO_STEP - 1;
        ratio = 1;
    }
#else
    for (i = MAX_ISO_STEP - 1; i >= 0; i--)
    {
        if (iso < iso_div * (2 << i))
        {
            iso_low = iso_div * (2 << (i)) / 2;
            iso_high = iso_div * (2 << i);
        }
    }
    ratio = (double)(iso - iso_low) / (iso_high - iso_low);
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
    gain_high       = (int)(log((double)iso_high / 50) / log((double)2));
    gain_low        = (int)(log((double)iso_low / 50) / log((double)2));
#endif


    LOGD_ANR("%s:%d iso:%d high:%d low:%d ratio:%f \n",
             __FUNCTION__, __LINE__,
             iso, iso_high, iso_low, ratio);


    stmfnrParamsSelected->back_ref_num              = stmfnrParams->back_ref_num;
    for(int lvl = 0; lvl < max_lvl; lvl++)
        stmfnrParamsSelected->weight_limit_y[lvl]   = ratio * (stmfnrParams->weight_limit_y[gain_high][lvl]   - stmfnrParams->weight_limit_y[gain_low][lvl])   + stmfnrParams->weight_limit_y[gain_low][lvl];;
    for(int lvl = 0; lvl < max_lvl_uv; lvl++)
        stmfnrParamsSelected->weight_limit_uv[lvl]  = ratio * (stmfnrParams->weight_limit_uv[gain_high][lvl]  - stmfnrParams->weight_limit_uv[gain_low][lvl])  + stmfnrParams->weight_limit_uv[gain_low][lvl];;
    for(int i = 0; i < 4; i++)
        stmfnrParamsSelected->ratio_frq[i]          = ratio * (stmfnrParams->ratio_frq[gain_high][i]          - stmfnrParams->ratio_frq[gain_low][i])          + stmfnrParams->ratio_frq[gain_low][i];

    for(int lvl = 0; lvl < max_lvl_uv; lvl++)
        stmfnrParamsSelected->luma_w_in_chroma[lvl] = ratio * (stmfnrParams->luma_w_in_chroma[gain_high][lvl] - stmfnrParams->luma_w_in_chroma[gain_low][lvl]) + stmfnrParams->luma_w_in_chroma[gain_low][lvl];
    for (int dir_idx = 0; dir_idx < dir_num; dir_idx++)
        for (j = 0; j < max_lvl; j++)
        {
            stmfnrParamsSelected->ci[dir_idx][j]            = ratio * (1 / stmfnrParams->ci[gain_high][dir_idx][j] - 1 / stmfnrParams->ci[gain_low][dir_idx][j]) + 1 / stmfnrParams->ci[gain_low][dir_idx][j];
        }
    for (int dir_idx = 0; dir_idx < dir_num; dir_idx++)
        for (j = 0; j < max_lvl; j++)
        {
            stmfnrParamsSelected->dnweight[dir_idx][j]  = ratio * (stmfnrParams->dnweight[gain_high][dir_idx][j] - stmfnrParams->dnweight[gain_low][dir_idx][j]) + stmfnrParams->dnweight[gain_low][dir_idx][j];
        }

    for (int dir_idx = 0; dir_idx < dir_num; dir_idx++)
        for (j = 0; j < max_lvl; j++)
        {
            stmfnrParamsSelected->scale[dir_idx][j]         = ratio * (stmfnrParams->scale[gain_high][dir_idx][j] - stmfnrParams->scale[gain_low][dir_idx][j]) + stmfnrParams->scale[gain_low][dir_idx][j];
        }
    for (int dir_idx = 0; dir_idx < dir_num; dir_idx++)
        for (j = 0; j < lumancurve_step; j++)
        {
            stmfnrParamsSelected->lumanrpoint[dir_idx][j]   = ratio * (stmfnrParams->lumanrpoint[gain_high][dir_idx][j] - stmfnrParams->lumanrpoint[gain_low][dir_idx][j]) + stmfnrParams->lumanrpoint[gain_low][dir_idx][j];
            stmfnrParamsSelected->lumanrcurve[dir_idx][j]   = ratio * (stmfnrParams->lumanrcurve[gain_high][dir_idx][j] - stmfnrParams->lumanrcurve[gain_low][dir_idx][j]) + stmfnrParams->lumanrcurve[gain_low][dir_idx][j];
        }
    for (int dir_idx = 0; dir_idx < dir_num; dir_idx++)
        stmfnrParamsSelected->dnstr[dir_idx]                = ratio * (stmfnrParams->dnstr[gain_high][dir_idx] - stmfnrParams->dnstr[gain_low][dir_idx]) + stmfnrParams->dnstr[gain_low][dir_idx];
    for (int dir_idx = 0; dir_idx < dir_num; dir_idx++)
        for (int lvl = 0; lvl < max_lvl; lvl++)
        {
            for(int i = 0; i < MFNR_MAX_GAUS_SIZE; i++)
                stmfnrParamsSelected->gfdelta[dir_idx][lvl][i]  = ratio * (stmfnrParams->gfdelta[gain_high][dir_idx][lvl][i] - stmfnrParams->gfdelta[gain_low][dir_idx][lvl][i]) + stmfnrParams->gfdelta[gain_low][dir_idx][lvl][i];
        }

    for (int dir_idx = 0; dir_idx < dir_num; dir_idx++)
        for (j = 0; j < max_lvl_uv; j++)
        {
            stmfnrParamsSelected->ci_uv[dir_idx][j]         = ratio * (1 / stmfnrParams->ci_uv[gain_high][dir_idx][j] - 1 / stmfnrParams->ci_uv[gain_low][dir_idx][j]) + 1 / stmfnrParams->ci_uv[gain_low][dir_idx][j];
            stmfnrParamsSelected->dnweight_uv[dir_idx][j]   = ratio * (stmfnrParams->dnweight_uv[gain_high][dir_idx][j] - stmfnrParams->dnweight_uv[gain_low][dir_idx][j]) + stmfnrParams->dnweight_uv[gain_low][dir_idx][j];
            stmfnrParamsSelected->scale_uv[dir_idx][j]      = ratio * (stmfnrParams->scale_uv[gain_high][dir_idx][j] - stmfnrParams->scale_uv[gain_low][dir_idx][j]) + stmfnrParams->scale_uv[gain_low][dir_idx][j];
        }
    for (int dir_idx = 0; dir_idx < dir_num; dir_idx++)
        for (j = 0; j < lumancurve_step; j++)
        {
            stmfnrParamsSelected->lumanrpoint_uv[dir_idx][j]    = ratio * (stmfnrParams->lumanrpoint_uv[gain_high][dir_idx][j] - stmfnrParams->lumanrpoint_uv[gain_low][dir_idx][j]) + stmfnrParams->lumanrpoint_uv[gain_low][dir_idx][j];
            stmfnrParamsSelected->lumanrcurve_uv[dir_idx][j]    = ratio * (stmfnrParams->lumanrcurve_uv[gain_high][dir_idx][j] - stmfnrParams->lumanrcurve_uv[gain_low][dir_idx][j]) + stmfnrParams->lumanrcurve_uv[gain_low][dir_idx][j];
        }
    for (int dir_idx = 0; dir_idx < dir_num; dir_idx++)
        stmfnrParamsSelected->dnstr_uv[dir_idx]                 = ratio * (stmfnrParams->dnstr_uv[gain_high][dir_idx] - stmfnrParams->dnstr_uv[gain_low][dir_idx]) + stmfnrParams->dnstr_uv[gain_low][dir_idx];

    for (int dir_idx = 0; dir_idx < dir_num; dir_idx++)
        for (int lvl = 0; lvl < max_lvl; lvl++)
        {
            for(int i = 0; i < MFNR_MAX_GAUS_SIZE; i++)
                stmfnrParamsSelected->gfdelta_uv[dir_idx][lvl][i]  = ratio * (stmfnrParams->gfdelta_uv[gain_high][dir_idx][lvl][i] - stmfnrParams->gfdelta_uv[gain_low][dir_idx][lvl][i]) + stmfnrParams->gfdelta_uv[gain_low][dir_idx][lvl][i];
        }




    for (int lvl = 0; lvl < max_lvl; lvl++)
    {
        for(int i = 0; i < MFNR_MAX_GAUS_SIZE; i++)
            stmfnrParamsSelected->gfsigma[lvl][i]           = ratio * (stmfnrParams->gfsigma[gain_high][lvl][i] - stmfnrParams->gfsigma[gain_low][lvl][i]) + stmfnrParams->gfsigma[gain_low][lvl][i];
    }

    for (i = 0; i < range; i++)
        noise_sigma_tmp[i]                                  = (ratio * (stmfnrParams->noise_sigma[gain_high][i] - stmfnrParams->noise_sigma[gain_low][i]) + stmfnrParams->noise_sigma[gain_low][i]);
    noise_sigma_tmp[range]                                  = noise_sigma_tmp[range - 1];

    int max_pos[2];
    int min_pos[2];
    int intep_pos_flg[POLT_ORDER + 2];
    int intep_pos[POLT_ORDER + 2];
    int fix_x_pos[MAX_INTEPORATATION_LUMAPOINT];
    int max_idx = 0;
    int min_idx = 0;
    int intep_num = 0;
    int intep_num_1 = 0;
#if 1
    for(i = 1; i < MAX(stmfnrParams->curve_x0[gain_high], stmfnrParams->curve_x0[gain_low]); i++)
    {
        if(noise_sigma_tmp[i] > noise_sigma_tmp[i - 1] && noise_sigma_tmp[i] > noise_sigma_tmp[i + 1])
        {
            max_pos[max_idx]    = i;
            max_idx++;
        }
        if(noise_sigma_tmp[i] < noise_sigma_tmp[i - 1] && noise_sigma_tmp[i] < noise_sigma_tmp[i + 1])
        {
            min_pos[min_idx]    = i;
            min_idx++;
        }
    }
    intep_pos[intep_num++]    = 0;
    intep_pos[intep_num++]    = MAX(stmfnrParams->curve_x0[gain_high], stmfnrParams->curve_x0[gain_low]);
    for(i = 0; i < max_idx; i++)
        intep_pos[intep_num++]    = max_pos[i];
    for(i = 0; i < MIN(1, min_idx); i++)// only 1 min point is allowed
        intep_pos[intep_num++]    = min_pos[i];
    for(i = 0; i < intep_num; i++) {
        for(j = i; j < intep_num; j++)
        {
            if(intep_pos[i] > intep_pos[j])
            {
                int temp;
                temp            = intep_pos[i];
                intep_pos[i]    = intep_pos[j];
                intep_pos[j]    = temp;

            }
        }
    }

    intep_pos[intep_num++]      = range;
#endif

#if 1
#if 1
    for (i = 1; i < intep_num; i++)
    {
        if(i == 1)
            intep_pos[i]        = ROUND_F(intep_pos[i] / (step_x1 * 4))  * (4   * step_x1);
        else
            intep_pos[i]        = ROUND_F(intep_pos[i] / (step_x1 * 16)) * (16  * step_x1);
    }
    memset(intep_pos_flg, 0, sizeof(intep_pos_flg));
    for (i = 1; i < intep_num; i++)
    {
        if(intep_pos[i - 1] == intep_pos[i])
            intep_pos_flg[i] = 1;
    }
    for (int i = 0, j = 0; i < intep_num; i++)
    {
        if(intep_pos_flg[i] == 0)
        {
            intep_pos[j] = intep_pos[i];
            j++;
        }
    }
    intep_num = j;


    int off = 0;
    int st;
    int idx;
    int off4    = (0x4 * step_x1);
    int off8    = (0x8 * step_x1);
    int off16   = (0x10 * step_x1);
    int off32   = (0x20 * step_x1);
    for (int i = 0, idx = 1; i < MAX_INTEPORATATION_LUMAPOINT - 1; i++)
    {
        int cur_pos;
        if(idx >= intep_num && off >= range)
        {

            fix_x_pos[intep_num_1++] = range;
            continue;
        }
        cur_pos                     = intep_pos[idx];
        fix_x_pos[intep_num_1++]  = off;

        if(off & off4)
            st = off4;
        else if(off & off8)
            st = off8;
        else if((off < 64 * step_x1) || (off & off16))
            st = off16;
        else
            st = off32;
        if(off + st > cur_pos && off != cur_pos)
        {
            if((cur_pos - off)  & off4)
                st = off4;
            else if((cur_pos - off)  & off8)
                st = off8;
            else if((cur_pos - off)  & off16)
                st = off16;
            else
                st = off32;
        }
        if(off + st == cur_pos)
            idx++;

        off += st;

    }
    fix_x_pos[intep_num_1++]        = range;
#else
    for(int i = 0, idx = 1; i < MAX_INTEPORATATION_LUMAPOINT; i++)
        fix_x_pos[intep_num_1++]        = i * (range / (MAX_INTEPORATATION_LUMAPOINT - 1));
#endif



    for (i = 0; i < range; i += step_x)
        stmfnrParamsSelected->noise_sigma[i / step_x]             = noise_sigma_tmp[i]                    / step_y;

    for (i = 0; i < MAX_INTEPORATATION_LUMAPOINT; i++) {
        stmfnrParamsSelected->noise_sigma_sample[i]             = noise_sigma_tmp[fix_x_pos[i]]       / step_y;
        stmfnrParamsSelected->noise_sigma_sample[i] = ABS(stmfnrParamsSelected->noise_sigma_sample[i]);
    }
    for (i = 0; i < MAX_INTEPORATATION_LUMAPOINT; i++)
        stmfnrParamsSelected->fix_x_pos[i]                    = fix_x_pos[i]                        / step_x;


    for (i = 0; i < MAX_INTEPORATATION_LUMAPOINT; i++) {
        stmfnrParamsSelected->noise_sigma_dehaze[i]             = noise_sigma_tmp[fix_x_pos[i]]       / step_y;
        stmfnrParamsSelected->noise_sigma_dehaze[i] = ABS(stmfnrParamsSelected->noise_sigma_dehaze[i]);
    }
    for (i = 0; i < MAX_INTEPORATATION_LUMAPOINT; i++)
        stmfnrParamsSelected->fix_x_pos_dehaze[i]               = fix_x_pos[i]                        / step_x_dehz;


#else
    for (i = 0; i < range; i += step)
        stmfnrParamsSelected->noise_sigma[i / step]   = noise_sigma_tmp[i] / step;

    int gap_num[POLT_ORDER] = {6, 6, 2, 2};
    int gap_piece = 8;
    int gap_rat[POLT_ORDER][5] = {{1, 2, 4, 6, 7}, {1, 2, 4, 6, 7}, {4, 8, 4, 4, 4}, {8, 8, 8, 8, 8}};

    for (i = 0; i < intep_num - 1; i++)
    {
        int left_pos                        = intep_pos[i];
        int right_pos                       = intep_pos[i + 1];
        int gap_num_cur                     = gap_num[i];
        intep_pos_1[intep_num_1++]          = intep_pos[i];
        for (j = 0; j < gap_num_cur - 1; j++)
        {
            intep_pos_1[intep_num_1++]      = ceil(left_pos + (double)(right_pos - left_pos) * gap_rat[i][j] / gap_piece);
        }
    }

    for (int idx = intep_num_1; idx < MAX_INTEPORATATION_LUMAPOINT - 1; idx++)
    {
        intep_pos_1[idx]              = intep_pos[intep_num - 1];
    }

    intep_pos_1[MAX_INTEPORATATION_LUMAPOINT - 1]                   = range;

    for (int idx = 0; idx < MAX_INTEPORATATION_LUMAPOINT; idx++)
    {
        intep_pos_1[idx]              = ROUND_F((double)intep_pos_1[idx] / step) * step;
    }


    for (int idx = 0; idx < intep_num; idx++)
    {
        intep_pos[idx]              = ROUND_F((double)intep_pos[idx] / step) * step;
    }


    for (i = 0; i < intep_num_1 - 1; i++)
    {
        int left_pos  = intep_pos_1[i] ;
        int right_pos = intep_pos_1[i + 1];
        double left_val  = noise_sigma_tmp[intep_pos_1[i]];
        double right_val = noise_sigma_tmp[intep_pos_1[i + 1]];
        for(j = left_pos; j < right_pos; j++)
        {
            if(j == 112)
                j = j;
            if(j != left_pos && j != right_pos)
                noise_sigma_tmp[j] = (left_val * (right_pos - j) + right_val * (j - left_pos)) / (right_pos - left_pos);
        }
    }

    for (i = 0; i < range; i += step)
        stmfnrParamsSelected->noise_sigma_intep[i / step]         = noise_sigma_tmp[i]                    / step;

    for (i = 0; i < MAX_INTEPORATATION_LUMAPOINT; i++)
        stmfnrParamsSelected->intep_pos_1[i]                    = intep_pos_1[i]                        / step;

    for (i = 0; i < MAX_INTEPORATATION_LUMAPOINT; i++)
        stmfnrParamsSelected->noise_sigma_dehaze[i]             = noise_sigma_tmp[intep_pos_1[i] ]      / step;

#endif

//for (i = 0; i < range; i++)
//    noise_sigma_tmp[i]                          = ROUND_F(noise_sigma_tmp[i] * (1 << 12)) >>  ;


    double noise_sigma_max = 0;
    double noise_sigma_limit = 1 << MFNR_F_INTE_SIGMA;
    double sigma_ratio = 1.0;
    double sigma_ratio_inv = 1.0;
    for(int i = 0; i < MAX_INTEPORATATION_LUMAPOINT;      i++)
        noise_sigma_max = MAX(stmfnrParamsSelected->noise_sigma_sample[i], noise_sigma_max);

    if(noise_sigma_max > noise_sigma_limit)
    {
        if(noise_sigma_max <= noise_sigma_limit)
        {
            sigma_ratio       = 1;
            sigma_ratio_inv   = 1;
        }
        else if(noise_sigma_max <= noise_sigma_limit * 2)
        {
            sigma_ratio       = 2;
            sigma_ratio_inv   = 1.0 / 2;
        }
        else if(noise_sigma_max <= noise_sigma_limit * 4)
        {
            sigma_ratio       = 4;
            sigma_ratio_inv   = 1.0 / 4;
        }
        else
        {
            LOGE_ANR("%s:%d noise_sigma is too big\n", __FUNCTION__, __LINE__);
        }
        if(sigma_ratio !=  1)
        {
            for(int i = 0; i < MAX_INTEPORATATION_LUMAPOINT;       i++)
                stmfnrParamsSelected->noise_sigma_sample[i]         = stmfnrParamsSelected->noise_sigma_sample[i]   * sigma_ratio_inv;
            for(int i = 0; i < MAX_INTEPORATATION_LUMAPOINT;       i++)
                stmfnrParamsSelected->noise_sigma_dehaze[i]         = stmfnrParamsSelected->noise_sigma_dehaze[i]   * sigma_ratio_inv;
            for(int dir_idx = 0; dir_idx < dir_num; dir_idx++)
                for(int lvl = 0; lvl < max_lvl; lvl++)
                    stmfnrParamsSelected->scale[dir_idx][lvl]       = stmfnrParamsSelected->scale[dir_idx][lvl]     * sigma_ratio;
            for(int dir_idx = 0; dir_idx < dir_num; dir_idx++)
                for(int lvl = 0; lvl < max_lvl_uv; lvl++)
                    stmfnrParamsSelected->scale_uv[dir_idx][lvl]    = stmfnrParamsSelected->scale_uv[dir_idx][lvl]  * sigma_ratio;
        }
    }

    return res;

}


template<typename T1, typename T2>
T1 FIX_FLOAT(T1 data0,  int bits, T2 &dst, int flag = 0)
{
    if(flag == 0)
        dst = (T2)ROUND_D(data0 * (1 << bits));
    else
        dst = (T2)FLOOR_INT64(data0 * (1 << bits));
    return  ((T1)dst / (1 << bits));

}


template<typename T1>
T1 FX_CLP(T1 data0, int inte_bit, int deci_bit)
{
    int64_t tmp0;
    int64_t max_val;
    int64_t min_val;
    int64_t out;
    tmp0        = (int64_t)(data0 * (1 << deci_bit));
    max_val     = (((int64_t)1 << (deci_bit + inte_bit)) - 1);
    min_val     = (-((int64_t)1 << (deci_bit + inte_bit)) + 1);
    out         = MIN(MAX(min_val, tmp0), max_val);
    return (T1)out / (1 << deci_bit);//->dst_shr_off64, round_val64, and_val64, round_bits);

}


int get_matrix_idx(int i, int j, int rad)
{
    int src_i = 0;
    int i_act, j_act;
    if(i > rad)
        i_act = 2 * rad - i;
    else
        i_act = i;

    if(j > rad)
        j_act = 2 * rad - j;
    else
        j_act = j;

    if(j_act < i_act) {
        int tmp;
        tmp = j_act;
        j_act = i_act;
        i_act = tmp;
    }

    for(int ii = rad; ii >= 0; ii--) {
        for(int jj = rad; jj >= ii; jj--) {
            if(i_act == ii && j_act == jj) {
                return src_i;
            }
            src_i++;
        }
    }

    return -1;
}


void mfnr_gfcoef_fix(int rad, double *gfcoef, unsigned char* gfcoef_fix)
{
    double sum_d2;
    double *h;
    unsigned long tmp;

    h = (double*)malloc((rad * 2 + 1) * (rad * 2 + 1) * sizeof(double));
    sum_d2 = 0;
    for(int i = 0; i <  (rad * 2 + 1); i++) {
        for(int j = 0; j < (rad * 2 + 1); j++) {
            int src_i = get_matrix_idx(i, j, rad);
            if(src_i == -1) {
                LOGE_ANR("get_matrix_idx is error \n");
            }
            h[i * (rad * 2 + 1) + j] = FIX_FLOAT((double)gfcoef[src_i], F_DECI_PIXEL_SIGMA_CONV_WEIGHT, tmp);
            gfcoef_fix[src_i] = tmp;
            sum_d2 += h[i * (rad * 2 + 1) + j];
        }
    }
    int idx = ((rad * 2 + 1) * rad + rad);
    h[idx]  = FIX_FLOAT(h[idx] + (1 - sum_d2), F_DECI_PIXEL_SIGMA_CONV_WEIGHT, tmp);
    gfcoef_fix[0] = tmp;
    free(h);

}


ANRresult_t mfnr_fix_transfer(RKAnr_Mfnr_Params_Select_t* tnr, RKAnr_Mfnr_Fix_t *pMfnrCfg, ANRExpInfo_t *pExpInfo, float gain_ratio, float fLumaStrength, float fChromaStrength)
{
    LOGI_ANR("%s:(%d) enter \n", __FUNCTION__, __LINE__);

    ANRresult_t res = ANR_RET_SUCCESS;

    if(tnr == NULL) {
        LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
        return ANR_RET_NULL_POINTER;
    }

    if(pMfnrCfg == NULL) {
        LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
        return ANR_RET_NULL_POINTER;
    }

    if(pExpInfo == NULL) {
        LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
        return ANR_RET_NULL_POINTER;
    }

	LOGD_ANR("%s:%d iso:%d strength:%f\n", __FUNCTION__, __LINE__, pExpInfo->arIso[pExpInfo->hdr_mode], fLumaStrength);
	if(fLumaStrength <= 0.0){
		fLumaStrength = 0.000001;
	}
	
    int i = 0;
    unsigned long tmp = 0;
    int mIso_last = pExpInfo->arIso[pExpInfo->hdr_mode];
    int mIso = pExpInfo->arIso[pExpInfo->hdr_mode];
    double gain_glb_filt;
    double gain_glb_ref1;
    double gain_glb_filt_sqrt;
    double gain_glb_filt_sqrt_inv;
    double scale_l_y                [MFNR_DIR_NUM][MFNR_MAX_LVL];
    double scale_l_y_uv             [MFNR_DIR_NUM][MFNR_MAX_LVL_UV];
    double scale_l_uv               [MFNR_DIR_NUM][MFNR_MAX_LVL_UV];
    double ratio_frq_sub_div        [2];
    int max_lvl         = MFNR_MAX_LVL;
    int max_lvl_uv      = MFNR_MAX_LVL_UV;
    int dir_num         = MFNR_DIR_NUM;


    int rad_isp20[2][5]                = {{2, 1, 1, 1, 1}, {2, 1, 1, 1, 1}};
    int rad_uv_isp20[2][5]             = {{2, 1, 1, 1, 1}, {2, 1, 1, 1, 1}};
    gain_glb_filt                           = (mIso_last / 50 * gain_ratio);
    gain_glb_ref1                           = (mIso / 50 * gain_ratio);//ref1
    gain_glb_filt_sqrt                      = sqrt(gain_glb_filt);
    gain_glb_filt_sqrt_inv                  = 1 / sqrt(gain_glb_filt);

    for(int dir_idx = 0; dir_idx < dir_num; dir_idx++) {
        for(int lvl = 0; lvl < max_lvl; lvl++)
            scale_l_y[dir_idx][lvl] = tnr->ci[dir_idx][lvl] * tnr->scale[dir_idx][lvl] / (tnr->dnstr[dir_idx] * fLumaStrength);
    }
    for(int dir_idx = 0; dir_idx < dir_num; dir_idx++) {
        for(int lvl = 0; lvl < max_lvl_uv; lvl++) {
            scale_l_y_uv[dir_idx][lvl] = tnr->ci[dir_idx][lvl] * tnr->scale_uv[dir_idx][lvl] / (tnr->dnstr_uv[dir_idx] * fChromaStrength);
            scale_l_uv[dir_idx][lvl] = tnr->ci_uv[dir_idx][lvl] * tnr->scale_uv[dir_idx][lvl] / (tnr->dnstr_uv[dir_idx] * fChromaStrength);
        }
    }

    //0x0080
    pMfnrCfg->mode = 0;
    pMfnrCfg->opty_en = 1;
    pMfnrCfg->optc_en = 1;
    pMfnrCfg->gain_en = 1;

    //0x0088   
    tmp = (tnr->weight_limit_y[0] / fLumaStrength);
	if(tmp > 0xff){
		tmp = 0xff;
	}
	pMfnrCfg->pk0_y = (unsigned char)tmp;

	tmp = (tnr->weight_limit_y[max_lvl - 1] / fLumaStrength);
	if(tmp > 0xff){
		tmp = 0xff;
	}
    pMfnrCfg->pk1_y = (unsigned char)tmp;
	tmp = (tnr->weight_limit_uv[0] / fChromaStrength );
	if(tmp > 0xff){
		tmp = 0xff;
	}
    pMfnrCfg->pk0_c = (unsigned char)tmp;
	tmp = (tnr->weight_limit_uv[max_lvl_uv - 1] / fChromaStrength);
	if(tmp > 0xff){
		tmp = 0xff;
	}
    pMfnrCfg->pk1_c = (unsigned char)tmp;

    //0x008c
    FIX_FLOAT(gain_glb_filt, F_DECI_GAIN, tmp, 0);
    if(tmp > 0xffff) {
        tmp = 0xffff;
    }
    pMfnrCfg->glb_gain_cur = tmp;
    FIX_FLOAT(gain_glb_ref1, F_DECI_GAIN, tmp);
    if(tmp > 0xffff) {
        tmp = 0xffff;
    }
    pMfnrCfg->glb_gain_nxt = tmp;

    //0x0090
    gain_glb_filt_sqrt_inv = FX_CLP(gain_glb_filt_sqrt_inv, F_INTE_GAIN_GLB_SQRT_INV, F_DECI_GAIN_GLB_SQRT_INV);
    FIX_FLOAT(gain_glb_filt_sqrt_inv, F_DECI_GAIN_GLB_SQRT_INV, tmp);
    if(tmp > 0x1fff) {
        tmp = 0x1fff;
    }
    pMfnrCfg->glb_gain_cur_div = tmp;
    FIX_FLOAT(gain_glb_filt_sqrt, F_DECI_GAIN_GLB_SQRT, tmp);
    if(tmp > 0xff) {
        tmp = 0xff;
    }
    pMfnrCfg->glb_gain_cur_sqrt = tmp;

    //0x0094 - 0x0098
    for(i = 0; i < 17 - 1; i++) {
        pMfnrCfg->sigma_x[i] = log((double)(tnr->fix_x_pos[i + 1] - tnr->fix_x_pos[i])) / log((double)2) - 2;
    }

    //0x009c - 0x00bc
    for(i = 0; i < 17; i++) {
        FIX_FLOAT(tnr->noise_sigma_sample[i], F_DECI_SIGMA, tmp);
        pMfnrCfg->sigma_y[i] = tmp;
    }

    //0x00c4 - 0x00cc
    //dir_idx = 0;
    for(i = 0; i < 6; i++) {
        FIX_FLOAT(tnr->lumanrcurve[0][i], F_DECI_LUMASCALE, tmp);
        pMfnrCfg->luma_curve[i] = tmp;
    }

    //0x00d0
    FIX_FLOAT(tnr->ratio_frq[0], F_DECI_TXT_THRD_RATIO, tmp);
    pMfnrCfg->txt_th0_y = tmp;
    FIX_FLOAT(tnr->ratio_frq[1], F_DECI_TXT_THRD_RATIO, tmp);
    pMfnrCfg->txt_th1_y = tmp;

    //0x00d4
    FIX_FLOAT(tnr->ratio_frq[2], F_DECI_TXT_THRD_RATIO, tmp);
    pMfnrCfg->txt_th0_c = tmp;
    FIX_FLOAT(tnr->ratio_frq[3], F_DECI_TXT_THRD_RATIO, tmp);
    pMfnrCfg->txt_th1_c = tmp;

    //0x00d8
    ratio_frq_sub_div[0] = 1 / (tnr->ratio_frq[1] - tnr->ratio_frq[0]);
    ratio_frq_sub_div[1] = 1 / (tnr->ratio_frq[3] - tnr->ratio_frq[2]);
    FIX_FLOAT(ratio_frq_sub_div[0], F_DECI_TXT_THRD_RATIO, tmp, 1);
    pMfnrCfg->txt_thy_dlt = tmp;
    FIX_FLOAT(ratio_frq_sub_div[1], F_DECI_TXT_THRD_RATIO, tmp, 1);
    pMfnrCfg->txt_thc_dlt = tmp;

    //0x00dc - 0x00ec
    //dir_idx = 0; lvl 0-3;
    //rad_isp20[dir][levl];   gfsigma[lvl]
    mfnr_gfcoef_fix(rad_isp20[0][0], tnr->gfsigma[0], pMfnrCfg->gfcoef_y0);
    mfnr_gfcoef_fix(rad_isp20[0][1], tnr->gfsigma[1], pMfnrCfg->gfcoef_y1);
    mfnr_gfcoef_fix(rad_isp20[0][2], tnr->gfsigma[2], pMfnrCfg->gfcoef_y2);
    mfnr_gfcoef_fix(rad_isp20[0][3], tnr->gfsigma[3], pMfnrCfg->gfcoef_y3);

    //0x00f0 - 0x0100
    //dir_idx = 0; lvl 0-3;
    //rad_isp20[dir][levl];   gfdelta[[dir_idx][lvl]
    mfnr_gfcoef_fix(rad_isp20[0][0], tnr->gfdelta[0][0], pMfnrCfg->gfcoef_yg0);
    mfnr_gfcoef_fix(rad_isp20[0][1], tnr->gfdelta[0][1], pMfnrCfg->gfcoef_yg1);
    mfnr_gfcoef_fix(rad_isp20[0][2], tnr->gfdelta[0][2], pMfnrCfg->gfcoef_yg2);
    mfnr_gfcoef_fix(rad_isp20[0][3], tnr->gfdelta[0][3], pMfnrCfg->gfcoef_yg3);

    //0x0104 - 0x0110
    //dir_idx = 1; lvl 0-3;
    //rad_isp20[dir][levl];   gfdelta[[dir_idx][lvl]  ;
    mfnr_gfcoef_fix(rad_isp20[1][0], tnr->gfdelta[1][0], pMfnrCfg->gfcoef_yl0);
    mfnr_gfcoef_fix(rad_isp20[1][1], tnr->gfdelta[1][1], pMfnrCfg->gfcoef_yl1);
    mfnr_gfcoef_fix(rad_isp20[1][2], tnr->gfdelta[1][2], pMfnrCfg->gfcoef_yl2);

    //0x0114 - 0x0120
    //dir_idx = 0; lvl 0-2;
    //rad_isp20[dir][levl];   gfdelta_uv[[dir_idx][lvl]  ;
    mfnr_gfcoef_fix(rad_uv_isp20[0][0], tnr->gfdelta_uv[0][0], pMfnrCfg->gfcoef_cg0);
    mfnr_gfcoef_fix(rad_uv_isp20[0][1], tnr->gfdelta_uv[0][1], pMfnrCfg->gfcoef_cg1);
    mfnr_gfcoef_fix(rad_uv_isp20[0][2], tnr->gfdelta_uv[0][2], pMfnrCfg->gfcoef_cg2);

    //0x0124 - 0x012c
    //dir_idx = 1; lvl 0-1;
    //rad_isp20[dir][levl];   gfdelta_uv[[dir_idx][lvl]  ;
    mfnr_gfcoef_fix(rad_uv_isp20[1][0], tnr->gfdelta_uv[1][0], pMfnrCfg->gfcoef_cl0);
    mfnr_gfcoef_fix(rad_uv_isp20[1][1], tnr->gfdelta_uv[1][1], pMfnrCfg->gfcoef_cl1);

    //0x0130 - 0x0134
    //dir_idx = 0;  i = lvl;
    for(i = 0; i < 4; i++) {
        FIX_FLOAT(scale_l_y[0][i], F_DECI_SCALE_L, tmp);
        pMfnrCfg->scale_yg[i] = tmp;
    }

    //0x0138 - 0x013c
    //dir_idx = 1;  i = lvl;
    for(i = 0; i < 3; i++) {
        FIX_FLOAT(scale_l_y[1][i], F_DECI_SCALE_L, tmp);
        pMfnrCfg->scale_yl[i] = tmp;
    }

    //0x0140 - 0x0148
    //dir_idx = 0;  i = lvl;
    for(i = 0; i < 3; i++) {
        FIX_FLOAT(scale_l_uv[0][i], F_DECI_SCALE_L_UV, tmp);
        pMfnrCfg->scale_cg[i] = tmp;
        FIX_FLOAT(scale_l_y_uv[0][i], F_DECI_SCALE_L_UV, tmp);
        pMfnrCfg->scale_y2cg[i] = tmp;
    }

    //0x014c - 0x0154
    //dir_idx = 1;  i = lvl;
    for(i = 0; i < 2; i++) {
        FIX_FLOAT(scale_l_uv[1][i], F_DECI_SCALE_L_UV, tmp);
        pMfnrCfg->scale_cl[i] = tmp;
    }
    for(i = 0; i < 3; i++) {
        FIX_FLOAT(scale_l_y_uv[1][i], F_DECI_SCALE_L_UV, tmp);
        pMfnrCfg->scale_y2cl[i] = tmp;
    }

    //0x0158
    for(i = 0; i < 3; i++) {
        FIX_FLOAT(tnr->luma_w_in_chroma[i], F_DECI_LUMA_W_IN_CHROMA, tmp);
        pMfnrCfg->weight_y[i] = tmp;
    }

#if MFNR_FIX_VALUE_PRINTF
    mfnr_fix_Printf(pMfnrCfg);
#endif


    LOGI_ANR("%s:(%d) exit \n", __FUNCTION__, __LINE__);

    return res;
}



ANRresult_t mfnr_fix_Printf(RKAnr_Mfnr_Fix_t  * pMfnrCfg)
{
    int i = 0;
    ANRresult_t res = ANR_RET_SUCCESS;

    if(pMfnrCfg == NULL) {
        LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
        return ANR_RET_NULL_POINTER;
    }

    LOGD_ANR("%s:(%d) enter \n", __FUNCTION__, __LINE__);
    //0x0080
    LOGD_ANR("(0x0080) mode:%d opty_en:%d optc_en:%d gain_en:%d\n",
             pMfnrCfg->mode,
             pMfnrCfg->opty_en,
             pMfnrCfg->optc_en,
             pMfnrCfg->gain_en);

    //0x0088
    LOGD_ANR("(0x0088) pk0_y:%d pk1_y:%d pk0_c:%d pk1_c:%d \n",
             pMfnrCfg->pk0_y,
             pMfnrCfg->pk1_y,
             pMfnrCfg->pk0_c,
             pMfnrCfg->pk1_c);

    //0x008c
    LOGD_ANR("(0x008c) glb_gain_cur:%d glb_gain_nxt:%d \n",
             pMfnrCfg->glb_gain_cur,
             pMfnrCfg->glb_gain_nxt);

    //0x0090
    LOGD_ANR("(0x0090) glb_gain_cur_div:%d gain_glb_filt_sqrt:%d \n",
             pMfnrCfg->glb_gain_cur_div,
             pMfnrCfg->glb_gain_cur_sqrt);

    //0x0094 - 0x0098
    for(i = 0; i < 17 - 1; i++) {
        LOGD_ANR("(0x0094 - 0x0098) sigma_x[%d]:%d \n",
                 i, pMfnrCfg->sigma_x[i]);
    }

    //0x009c - 0x00bc
    for(i = 0; i < 17; i++) {
        LOGD_ANR("(0x009c - 0x00bc) sigma_y[%d]:%d \n",
                 i, pMfnrCfg->sigma_y[i]);
    }

    //0x00c4 - 0x00cc
    //dir_idx = 0;
    for(i = 0; i < 6; i++) {
        LOGD_ANR("(0x00c4 - 0x00cc) luma_curve[%d]:%d \n",
                 i, pMfnrCfg->luma_curve[i]);
    }

    //0x00d0
    LOGD_ANR("(0x00d0) txt_th0_y:%d txt_th1_y:%d \n",
             pMfnrCfg->txt_th0_y,
             pMfnrCfg->txt_th1_y);

    //0x00d4
    LOGD_ANR("(0x00d0) txt_th0_c:%d txt_th1_c:%d \n",
             pMfnrCfg->txt_th0_c,
             pMfnrCfg->txt_th1_c);

    //0x00d8
    LOGD_ANR("(0x00d8) txt_thy_dlt:%d txt_thc_dlt:%d \n",
             pMfnrCfg->txt_thy_dlt,
             pMfnrCfg->txt_thc_dlt);

    //0x00dc - 0x00ec
    for(i = 0; i < 6; i++) {
        LOGD_ANR("(0x00dc - 0x00ec) gfcoef_y0[%d]:%d \n",
                 i, pMfnrCfg->gfcoef_y0[i]);
    }
    for(i = 0; i < 3; i++) {
        LOGD_ANR("(0x00dc - 0x00ec) gfcoef_y1[%d]:%d \n",
                 i, pMfnrCfg->gfcoef_y1[i]);
    }
    for(i = 0; i < 3; i++) {
        LOGD_ANR("(0x00dc - 0x00ec) gfcoef_y2[%d]:%d \n",
                 i, pMfnrCfg->gfcoef_y2[i]);
    }
    for(i = 0; i < 3; i++) {
        LOGD_ANR("(0x00dc - 0x00ec) gfcoef_y3[%d]:%d \n",
                 i, pMfnrCfg->gfcoef_y3[i]);
    }

    //0x00f0 - 0x0100
    for(i = 0; i < 6; i++) {
        LOGD_ANR("(0x00f0 - 0x0100) gfcoef_yg0[%d]:%d \n",
                 i, pMfnrCfg->gfcoef_yg0[i]);
    }
    for(i = 0; i < 3; i++) {
        LOGD_ANR("(0x00f0 - 0x0100) gfcoef_yg1[%d]:%d \n",
                 i, pMfnrCfg->gfcoef_yg1[i]);
    }
    for(i = 0; i < 3; i++) {
        LOGD_ANR("(0x00f0 - 0x0100) gfcoef_yg2[%d]:%d \n",
                 i, pMfnrCfg->gfcoef_yg2[i]);
    }
    for(i = 0; i < 3; i++) {
        LOGD_ANR("(0x00f0 - 0x0100) gfcoef_yg3[%d]:%d \n",
                 i, pMfnrCfg->gfcoef_yg3[i]);
    }


    //0x0104 - 0x0110
    for(i = 0; i < 6; i++) {
        LOGD_ANR("(0x0104 - 0x0110) gfcoef_yl0[%d]:%d \n",
                 i, pMfnrCfg->gfcoef_yl0[i]);
    }
    for(i = 0; i < 3; i++) {
        LOGD_ANR("(0x0104 - 0x0110) gfcoef_yl1[%d]:%d \n",
                 i, pMfnrCfg->gfcoef_yl1[i]);
    }
    for(i = 0; i < 3; i++) {
        LOGD_ANR("(0x0104 - 0x0110) gfcoef_yl2[%d]:%d \n",
                 i, pMfnrCfg->gfcoef_yl2[i]);
    }

    //0x0114 - 0x0120
    for(i = 0; i < 6; i++) {
        LOGD_ANR("(0x0114 - 0x0120) gfcoef_cg0[%d]:%d \n",
                 i, pMfnrCfg->gfcoef_cg0[i]);
    }
    for(i = 0; i < 3; i++) {
        LOGD_ANR("(0x0114 - 0x0120) gfcoef_cg1[%d]:%d \n",
                 i, pMfnrCfg->gfcoef_cg1[i]);
    }
    for(i = 0; i < 3; i++) {
        LOGD_ANR("(0x0114 - 0x0120) gfcoef_cg2[%d]:%d \n",
                 i, pMfnrCfg->gfcoef_cg2[i]);
    }


    //0x0124 - 0x012c
    for(i = 0; i < 6; i++) {
        LOGD_ANR("(0x0124 - 0x012c) gfcoef_cl0[%d]:%d \n",
                 i, pMfnrCfg->gfcoef_cl0[i]);
    }
    for(i = 0; i < 3; i++) {
        LOGD_ANR("(0x0124 - 0x012c) gfcoef_cl1[%d]:%d \n",
                 i, pMfnrCfg->gfcoef_cl1[i]);
    }


    //0x0130 - 0x0134
    //dir_idx = 0;  i = lvl;
    for(i = 0; i < 4; i++) {
        LOGD_ANR("(0x0130 - 0x0134) scale_yg[%d]:%d \n",
                 i, pMfnrCfg->scale_yg[i]);
    }

    //0x0138 - 0x013c
    //dir_idx = 1;  i = lvl;
    for(i = 0; i < 3; i++) {
        LOGD_ANR("(0x0138 - 0x013c) scale_yl[%d]:%d \n",
                 i, pMfnrCfg->scale_yl[i]);
    }

    //0x0140 - 0x0148
    //dir_idx = 0;  i = lvl;
    for(i = 0; i < 3; i++) {
        LOGD_ANR("(0x0140 - 0x0148) scale_cg[%d]:%d \n",
                 i, pMfnrCfg->scale_cg[i]);
        LOGD_ANR("(0x0140 - 0x0148) scale_y2cg[%d]:%d \n",
                 i, pMfnrCfg->scale_y2cg[i]);
    }

    //0x014c - 0x0154
    //dir_idx = 1;  i = lvl;
    for(i = 0; i < 2; i++) {
        LOGD_ANR("(0x014c - 0x0154) scale_cl[%d]:%d \n",
                 i, pMfnrCfg->scale_cl[i]);
    }
    for(i = 0; i < 3; i++) {
        LOGD_ANR("(0x014c - 0x0154) scale_y2cl[%d]:%d \n",
                 i, pMfnrCfg->scale_y2cl[i]);
    }

    //0x0158
    for(i = 0; i < 3; i++) {
        LOGD_ANR("(0x0158) weight_y[%d]:%d \n",
                 i, pMfnrCfg->weight_y[i]);
    }

    LOGD_ANR("%s:(%d) exit \n", __FUNCTION__, __LINE__);

    return res;
}


ANRresult_t mfnr_dynamic_calc(RKAnr_Mfnr_Dynamic_t  * pDynamic, ANRExpInfo_t *pExpInfo)
{
    LOGI_ANR("%s:(%d) enter \n", __FUNCTION__, __LINE__);

    ANRresult_t res = ANR_RET_SUCCESS;
    float time = pExpInfo->arTime[pExpInfo->hdr_mode];
    float iso = pExpInfo->arIso[pExpInfo->hdr_mode];
    float exp = time * iso;
	
    if(pDynamic == NULL) {
        LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
        return ANR_RET_NULL_POINTER;
    }

    if(pExpInfo == NULL) {
        LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
        return ANR_RET_NULL_POINTER;
    }

	
    if(iso >= pDynamic->highth_iso && time >= pDynamic->highth_time){
	 pDynamic->mfnr_enable_state = 1;
    }else if(iso <= pDynamic->lowth_iso && time <= pDynamic->lowth_time){
	 pDynamic->mfnr_enable_state = 0;
    }

	LOGD_ANR("%s:%d mfnr: cur:%f %f  highth:%f %f  lowth:%f %f  finnal:%d\n", 
		__FUNCTION__, __LINE__, 
		iso, time, 
		pDynamic->highth_iso, pDynamic->highth_time,
		pDynamic->lowth_iso, pDynamic->lowth_time,
		pDynamic->mfnr_enable_state);
	
    return res;
}


