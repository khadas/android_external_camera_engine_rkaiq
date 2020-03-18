#include "edgefilter.h"



RKAiqResult_t init_edgefilter_params(RKAsharp_EdgeFilter_Params_t *pParams, CalibDb_EdgeFilter_t *pCalibdb)
{
	RKAiqResult_t res = RK_AIQ_RET_SUCCESS;
	int i = 0;
	int j = 0;
	int max_iso_step = MAX_ISO_STEP;
	
	if(pParams == NULL){
		LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return RK_AIQ_RET_NULL_POINTER;
	}

	if(pCalibdb == NULL){
		LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return RK_AIQ_RET_NULL_POINTER;
	}

	for(i=0; i<max_iso_step; i++){
		pParams->edge_thed[i] = pCalibdb->edgeFilter_iso[i].edge_thed;
		pParams->smoth4[i] = pCalibdb->edgeFilter_iso[i].src_wgt;
		pParams->alpha_adp_en[i] = pCalibdb->edgeFilter_iso[i].alpha_adp_en;
		pParams->l_alpha[i] = pCalibdb->edgeFilter_iso[i].local_alpha;
		pParams->g_alpha[i] = pCalibdb->edgeFilter_iso[i].global_alpha;
	}

	for(j=0; j < RK_EDGEFILTER_LUMA_POINT_NUM; j++){
		pParams->enhance_luma_point[j] = pCalibdb->luma_point[j];
	}

	for (i=0; i<max_iso_step; i++){
        for(j = 0; j < RK_EDGEFILTER_LUMA_POINT_NUM; j++){
			pParams->edge_thed_1[i][j] = pCalibdb->edgeFilter_iso[i].noise_clip[j];
			pParams->clamp_pos_dog[i][j] = pCalibdb->edgeFilter_iso[i].dog_clip_pos[j];
			pParams->clamp_neg_dog[i][j] = pCalibdb->edgeFilter_iso[i].dog_clip_neg[j];
			pParams->detail_alpha_dog[i][j] = pCalibdb->edgeFilter_iso[i].dog_alpha[j];
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
            pParams->h0_h_coef_5x5[i][2 * w + n] = pCalibdb->edgeFilter_iso[i].direct_filter_coeff[n]; 
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
            pParams->dog_kernel[i][0* w + n] = pCalibdb->edgeFilter_iso[i].dog_kernel_row0[n];
			pParams->dog_kernel[i][1* w + n] = pCalibdb->edgeFilter_iso[i].dog_kernel_row1[n];
			pParams->dog_kernel[i][2* w + n] = pCalibdb->edgeFilter_iso[i].dog_kernel_row2[n];
			pParams->dog_kernel[i][3* w + n] = pCalibdb->edgeFilter_iso[i].dog_kernel_row3[n];
			pParams->dog_kernel[i][4* w + n] = pCalibdb->edgeFilter_iso[i].dog_kernel_row4[n];
        }
			
      
    }

	return RK_AIQ_RET_SUCCESS;
}


void select_edgefilter_params_by_ISO(RKAsharp_EdgeFilter_Params_t *strkedgefilterParams, RKAsharp_EdgeFilter_Params_Select_t *strkedgefilterParamsSelected, int iso)
{
    int i;
	int iso_low, iso_high;
	int gain_high, gain_low;
	float ratio;
	int iso_div 			= 50;
    int max_iso_step        = MAX_ISO_STEP;
	for (i = max_iso_step - 1; i >= 0; i--)
	{
		if (iso < iso_div * (2 << i))
		{
			iso_low = iso_div * (2 << (i)) / 2;
			iso_high = iso_div * (2 << i);
		}
	}
	ratio = (float)(iso - iso_low)/(iso_high-iso_low);
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
	gain_high 		= (int)(log((float)iso_high / 50) /log((float)2));
	gain_low 		= (int)(log((float)iso_low / 50) / log((float)2));

	gain_low		= MIN(MAX(gain_low, 0), 8);
	gain_high		= MIN(MAX(gain_high, 0), 8);

    strkedgefilterParamsSelected->edge_thed    = (short)EDGEFILTER_ROUND_F(INTERP1(strkedgefilterParams->edge_thed   [gain_low], strkedgefilterParams->edge_thed   [gain_high], ratio));
    strkedgefilterParamsSelected->dir_min      = INTERP1(strkedgefilterParams->dir_min     [gain_low], strkedgefilterParams->dir_min     [gain_high], ratio);
	strkedgefilterParamsSelected->smoth4	   = INTERP1(strkedgefilterParams->smoth4     [gain_low], strkedgefilterParams->smoth4     [gain_high], ratio);
    strkedgefilterParamsSelected->alpha_adp_en = INTERP1(strkedgefilterParams->alpha_adp_en[gain_low], strkedgefilterParams->alpha_adp_en[gain_high], ratio);
    strkedgefilterParamsSelected->l_alpha      = INTERP1(strkedgefilterParams->l_alpha     [gain_low], strkedgefilterParams->l_alpha     [gain_high], ratio);
    strkedgefilterParamsSelected->g_alpha      = INTERP1(strkedgefilterParams->g_alpha     [gain_low], strkedgefilterParams->g_alpha     [gain_high], ratio);
    for(int i = 0; i < RK_EDGEFILTER_LUMA_POINT_NUM; i++)
    {
        strkedgefilterParamsSelected->enhance_luma_point[i] = strkedgefilterParams->enhance_luma_point[i];
        strkedgefilterParamsSelected->edge_thed_1[i]        = (short)EDGEFILTER_ROUND_F(INTERP1(strkedgefilterParams->edge_thed_1[gain_low][i],           strkedgefilterParams->edge_thed_1[gain_high][i],        ratio));
        strkedgefilterParamsSelected->clamp_pos_gaus[i]     = INTERP1(strkedgefilterParams->clamp_pos_gaus[gain_low][i],        strkedgefilterParams->clamp_pos_gaus[gain_high][i],     ratio);
        strkedgefilterParamsSelected->clamp_neg_gaus[i]     = INTERP1(strkedgefilterParams->clamp_neg_gaus[gain_low][i],        strkedgefilterParams->clamp_neg_gaus[gain_high][i],     ratio);
        strkedgefilterParamsSelected->clamp_pos_dog[i]      = (short)EDGEFILTER_ROUND_F(INTERP1(strkedgefilterParams->clamp_pos_dog[gain_low][i],         strkedgefilterParams->clamp_pos_dog[gain_high][i],      ratio));
        strkedgefilterParamsSelected->clamp_neg_dog[i]      = (short)EDGEFILTER_ROUND_F(INTERP1(strkedgefilterParams->clamp_neg_dog[gain_low][i],         strkedgefilterParams->clamp_neg_dog[gain_high][i],      ratio));
        strkedgefilterParamsSelected->detail_alpha_gaus[i]  = INTERP1(strkedgefilterParams->detail_alpha_gaus[gain_low][i],     strkedgefilterParams->detail_alpha_gaus[gain_high][i],  ratio);
        strkedgefilterParamsSelected->detail_alpha_dog[i]   = INTERP1(strkedgefilterParams->detail_alpha_dog[gain_low][i],      strkedgefilterParams->detail_alpha_dog[gain_high][i],   ratio);
    }
    for(int i = 0;i < sizeof(strkedgefilterParamsSelected->gaus_luma_kernel)	/ sizeof(strkedgefilterParamsSelected->gaus_luma_kernel[0]); i++)
        strkedgefilterParamsSelected->gaus_luma_kernel[i]   = INTERP1(strkedgefilterParams->gaus_luma_kernel [gain_low][i],     strkedgefilterParams->gaus_luma_kernel  [gain_high][i], ratio);
    for(int i = 0;i < sizeof(strkedgefilterParamsSelected->h0_h_coef_5x5)	    / sizeof(strkedgefilterParamsSelected->h0_h_coef_5x5[0]); i++)
        strkedgefilterParamsSelected->h0_h_coef_5x5[i]      = INTERP1(strkedgefilterParams->h0_h_coef_5x5 [gain_low][i],        strkedgefilterParams->h0_h_coef_5x5     [gain_high][i], ratio);
    for(int i = 0;i < sizeof(strkedgefilterParamsSelected->h1_v_coef_5x5)	    / sizeof(strkedgefilterParamsSelected->h1_v_coef_5x5[0]); i++)
        strkedgefilterParamsSelected->h1_v_coef_5x5[i]      = INTERP1(strkedgefilterParams->h1_v_coef_5x5 [gain_low][i],        strkedgefilterParams->h1_v_coef_5x5     [gain_high][i], ratio);
    for(int i = 0;i < sizeof(strkedgefilterParamsSelected->h2_m_coef_5x5)	    / sizeof(strkedgefilterParamsSelected->h2_m_coef_5x5[0]); i++)
        strkedgefilterParamsSelected->h2_m_coef_5x5[i]      = INTERP1(strkedgefilterParams->h2_m_coef_5x5 [gain_low][i],        strkedgefilterParams->h2_m_coef_5x5     [gain_high][i], ratio);
    for(int i = 0;i < sizeof(strkedgefilterParamsSelected->h3_p_coef_5x5)	    / sizeof(strkedgefilterParamsSelected->h3_p_coef_5x5[0]); i++)
        strkedgefilterParamsSelected->h3_p_coef_5x5[i]      = INTERP1(strkedgefilterParams->h3_p_coef_5x5 [gain_low][i],        strkedgefilterParams->h3_p_coef_5x5     [gain_high][i], ratio);
    for(int i = 0;i < sizeof(strkedgefilterParamsSelected->h_coef_5x5)	        / sizeof(strkedgefilterParamsSelected->h_coef_5x5[0]); i++)
        strkedgefilterParamsSelected->h_coef_5x5[i]         = INTERP1(strkedgefilterParams->h_coef_5x5 [gain_low][i],           strkedgefilterParams->h_coef_5x5        [gain_high][i], ratio);
    for(int i = 0;i < sizeof(strkedgefilterParamsSelected->gf_coef_3x3)	        / sizeof(strkedgefilterParamsSelected->gf_coef_3x3[0]); i++)
        strkedgefilterParamsSelected->gf_coef_3x3[i]        = INTERP1(strkedgefilterParams->gf_coef_3x3 [gain_low][i],          strkedgefilterParams->gf_coef_3x3       [gain_high][i], ratio);
    for(int i = 0;i < sizeof(strkedgefilterParamsSelected->dog_kernel)			/ sizeof(strkedgefilterParamsSelected->dog_kernel[0]); i++)
        strkedgefilterParamsSelected->dog_kernel[i]         = INTERP1(strkedgefilterParams->dog_kernel [gain_low][i],           strkedgefilterParams->dog_kernel        [gain_high][i], ratio);


}

