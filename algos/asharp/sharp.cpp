
#include "sharp.h"


RKAiqResult_t init_sharp_params_v1(RKAsharp_Sharp_HW_Params_t *pParams, CalibDb_Sharp_t *pCalibdb)
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

	for (i=0;i<max_iso_step;i++){
		pParams->lratio[i] = pCalibdb->sharp_iso[i].lratio;
		pParams->hratio[i] = pCalibdb->sharp_iso[i].hratio;
		pParams->M_ratio[i] = pCalibdb->sharp_iso[i].mf_sharp_ratio;
		pParams->H_ratio[i] = pCalibdb->sharp_iso[i].hf_sharp_ratio;
	}

	for(int j = 0; j < RK_EDGEFILTER_LUMA_POINT_NUM; j++){
		pParams->luma_point[j] = pCalibdb->luma_point[j];
	}

	for (i=0;i<max_iso_step;i++){
		for(int j = 0; j < RK_EDGEFILTER_LUMA_POINT_NUM; j++){
            pParams->luma_sigma[i][j] = pCalibdb->sharp_iso[i].luma_sigma[j];
        }		
		pParams->pbf_gain[i] = pCalibdb->sharp_iso[i].pbf_gain;
		pParams->pbf_ratio[i] = pCalibdb->sharp_iso[i].pbf_ratio;
		pParams->pbf_add[i] = pCalibdb->sharp_iso[i].pbf_add;

		for(int j = 0; j < RK_EDGEFILTER_LUMA_POINT_NUM; j++){
			pParams->lum_clp_m[i][j] = pCalibdb->sharp_iso[i].mf_clip_pos[j];
		}	
			
		for(int j = 0; j < RK_EDGEFILTER_LUMA_POINT_NUM; j++){
			pParams->lum_min_m[i][j] = pCalibdb->sharp_iso[i].mf_clip_neg[j];
		}

		for(int j = 0; j < RK_SHARPFILTER_LUMA_POINT_NUM; j++){
			pParams->lum_clp_h[i][j] = pCalibdb->sharp_iso[i].hf_clip[j];
		}
		
		pParams->mbf_gain[i] = pCalibdb->sharp_iso[i].mbf_gain;
		pParams->hbf_gain[i] = pCalibdb->sharp_iso[i].hbf_gain;
		pParams->hbf_ratio[i] = pCalibdb->sharp_iso[i].hbf_ratio;
		pParams->mbf_add[i] = pCalibdb->sharp_iso[i].mbf_add;
		pParams->hbf_add[i] = pCalibdb->sharp_iso[i].hbf_add;
        pParams->ehf_th[i] = pCalibdb->sharp_iso[i].local_sharp_strength;       
    }

	//////////////////////////////////////////////////////////////////////////
	// init filter params
	// Gaussian filter params
	float gaus_luma_coeff[RKSHAPRENHW_PBF_DIAM * RKSHAPRENHW_PBF_DIAM] = 
	{
		0.0625, 0.125, 0.0625,
		0.125, 0.25, 0.125,
		0.0625, 0.125, 0.0625
	};
	// pre bf params
	float pbf_coeff[RKSHAPRENHW_PBF_DIAM * RKSHAPRENHW_PBF_DIAM] =
	{
        0.156250, 0.25, 0.156250, 
        0.25, 0.375000, 0.25, 
        0.156250, 0.25, 0.156250
	};
	// mf range filter params
	float rf_m_coeff[RKSHAPRENHW_MRF_DIAM * RKSHAPRENHW_MRF_DIAM] =
    {
        0.023438, 0.031250, 0.039063, 0.031250, 0.023438, 
        0.031250, 0.046875, 0.054688, 0.046875, 0.031250, 
        0.039063, 0.054688, 0.093750, 0.054688, 0.039063, 
        0.031250, 0.046875, 0.054688, 0.046875, 0.031250,
        0.023438, 0.031250, 0.039063, 0.031250, 0.023438 

    };
	// mf bf params
	float mbf_coeff[RKSHAPRENHW_MBF_DIAM_Y * RKSHAPRENHW_MBF_DIAM_X] =
    {
        0.0, 0.0, 0.0, 0.0, 0.406250, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.406250, 0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.343750, 0.0, 0.328125, 0.0, 0.343750, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.406250, 0.0, 0.359375, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.359375, 0.0, 0.406250, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 0.0, 0.296875, 0.0, 0.234375, 0.0, 0.234375, 0.0, 0.296875, 0.0, 0.0, 0.0, 0.0, 0.0, 
        0.0, 0.406250, 0.0, 0.343750, 0.0, 0.0, 0.0, 0.0, 0.171875, 0.0, 0.0, 0.0, 0.0, 0.343750, 0.0, 0.406250, 0.0, 
        0.0, 0.0, 0.0, 0.0, 0.0, 0.234375, 0.0, 0.140625, 0.109375, 0.140625, 0.0, 0.234375, 0.0, 0.0, 0.0, 0.0, 0.0, 
        0.437500, 0.0, 0.0, 0.328125, 0.0, 0.0, 0.171875, 0.109375, 0.0, 0.109375, 0.171875, 0.0, 0.0, 0.328125, 0.0, 0.0, 0.437500, 
        0.0, 0.0, 0.0, 0.0, 0.0, 0.234375, 0.0, 0.140625, 0.109375, 0.140625, 0.0, 0.234375, 0.0, 0.0, 0.0, 0.0, 0.0, 
        0.0, 0.406250, 0.0, 0.343750, 0.0, 0.0, 0.0, 0.0, 0.171875, 0.0, 0.0, 0.0, 0.0, 0.343750, 0.0, 0.406250, 0.0, 
        0.0, 0.0, 0.0, 0.0, 0.0, 0.296875, 0.0, 0.234375, 0.0, 0.234375, 0.0, 0.296875, 0.0, 0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.406250, 0.0, 0.359375, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.359375, 0.0, 0.406250, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.343750, 0.0, 0.328125, 0.0, 0.343750, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.406250, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.406250, 0.0, 0.0, 0.0, 0.0  
    };
	// hf range filter params
	float rf_h_coeff[RKSHAPRENHW_HRF_DIAM * RKSHAPRENHW_HRF_DIAM] =
    {
        0.0, 0.015625, 0.023438, 0.015625, 0.0, 
        0.015625, 0.062500, 0.101563, 0.062500, 0.015625, 
        0.023438, 0.101563, 0.125000, 0.101563, 0.023438, 
        0.015625, 0.062500, 0.101563, 0.062500, 0.015625,
		0.0, 0.015625, 0.023438, 0.015625, 0.0
    };
	// hf bf params
	float hbf_coeff[RKSHAPRENHW_HBF_DIAM * RKSHAPRENHW_HBF_DIAM] = 
    {
        0.156250, 0.25, 0.156250, 
        0.25, 0.375000, 0.25, 
        0.156250, 0.25, 0.156250 
    };

	for (i=0; i<max_iso_step; i++){
        int h = RKSHAPRENHW_GAU_DIAM;
        int w = RKSHAPRENHW_GAU_DIAM;
        for(int m = 0; m < h; m++){
            for(int n = 0; n < w; n++){
                pParams->gaus_luma_kernel[i][m * w + n] = gaus_luma_coeff[m * w + n];
			}
        }
    }

    for (i=0; i<max_iso_step; i++){
        int h = RKSHAPRENHW_PBF_DIAM;
        int w = RKSHAPRENHW_PBF_DIAM;
        for(int m = 0; m < h; m++){
            for(int n = 0; n < w; n++)
                pParams->kernel_pbf[i][m * w + n] = pbf_coeff[m * w + n];
        }
    }
	
    for (i=0; i<max_iso_step; i++){
        int h = RKSHAPRENHW_MRF_DIAM;
        int w = RKSHAPRENHW_MRF_DIAM;
        for(int m = 0; m < h; m++){
            for(int n = 0; n < w; n++)
                pParams->h_rf_m[i][m * w + n] = rf_m_coeff[m * w + n];
        }
    }

	for (i=0; i<max_iso_step; i++){
        int h = RKSHAPRENHW_MBF_DIAM_Y;
        int w = RKSHAPRENHW_MBF_DIAM_X;
        for(int m = 0; m < h; m++){
            for(int n = 0; n < w; n++)
                pParams->kernel_mbf[i][m * w + n] = mbf_coeff[m * w + n];
        }
    }
	
    for (i=0; i<max_iso_step; i++){
        int h = RKSHAPRENHW_HRF_DIAM;
        int w = RKSHAPRENHW_HRF_DIAM;
        for(int m = 0; m < h; m++){
            for(int n = 0; n < w; n++)
                pParams->h_rf_h[i][m * w + n] = rf_h_coeff[m * w + n];
        }
    }
	
    for (i=0; i<max_iso_step; i++){
        int h = RKSHAPRENHW_HBF_DIAM;
        int w = RKSHAPRENHW_HBF_DIAM;
        for(int m = 0; m < h; m++){
            for(int n = 0; n < w; n++)
                pParams->kernel_hbf[i][m * w + n] = hbf_coeff[m * w + n];
        }
    }

	return RK_AIQ_RET_SUCCESS;
	
}

void select_rk_sharpen_hw_params_by_ISO(				
	RKAsharp_Sharp_HW_Params_t *strksharpenParams, 	
	RKAsharp_Sharp_HW_Params_Select_t *strksharpenParamsSelected, 
	int iso
)
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



    strksharpenParamsSelected->lratio               = INTERP1(strksharpenParams->lratio     [gain_low],     strksharpenParams->lratio       [gain_high],    ratio);
    strksharpenParamsSelected->hratio               = INTERP1(strksharpenParams->hratio     [gain_low],     strksharpenParams->hratio       [gain_high],    ratio);
    strksharpenParamsSelected->M_ratio              = INTERP1(strksharpenParams->M_ratio    [gain_low],     strksharpenParams->M_ratio      [gain_high],    ratio);
    strksharpenParamsSelected->H_ratio              = INTERP1(strksharpenParams->H_ratio    [gain_low],     strksharpenParams->H_ratio      [gain_high],    ratio);
	strksharpenParamsSelected->pbf_ratio            = INTERP1(strksharpenParams->pbf_ratio  [gain_low],     strksharpenParams->pbf_ratio    [gain_high],    ratio);
    strksharpenParamsSelected->hbf_ratio            = INTERP1(strksharpenParams->hbf_ratio  [gain_low],     strksharpenParams->hbf_ratio    [gain_high],    ratio);
    strksharpenParamsSelected->ehf_th               = (short)SHARP_ROUND_F(INTERP1(strksharpenParams->ehf_th     [gain_low],     strksharpenParams->ehf_th       [gain_high],    ratio));
    for(int i = 0; i < RK_EDGEFILTER_LUMA_POINT_NUM; i++)
    {
        strksharpenParamsSelected->luma_point[i]    = strksharpenParams->luma_point[i];
        strksharpenParamsSelected->luma_sigma[i]    = INTERP1(strksharpenParams->luma_sigma [gain_low][i],  strksharpenParams->luma_sigma   [gain_high][i], ratio);
        
        strksharpenParamsSelected->lum_clp_m[i]     = (short)SHARP_ROUND_F(INTERP1(strksharpenParams->lum_clp_m  [gain_low][i],  strksharpenParams->lum_clp_m    [gain_high][i], ratio));
        strksharpenParamsSelected->lum_min_m[i]     = INTERP1(strksharpenParams->lum_min_m  [gain_low][i],  strksharpenParams->lum_min_m    [gain_high][i], ratio);

        strksharpenParamsSelected->lum_clp_h[i]     = (short)SHARP_ROUND_F(INTERP1(strksharpenParams->lum_clp_h  [gain_low][i],  strksharpenParams->lum_clp_h    [gain_high][i], ratio));
    }
	strksharpenParamsSelected->pbf_gain      = INTERP1(strksharpenParams->pbf_gain   [gain_low],  strksharpenParams->pbf_gain     [gain_high], ratio);
	strksharpenParamsSelected->pbf_add       = INTERP1(strksharpenParams->pbf_add    [gain_low],  strksharpenParams->pbf_add      [gain_high], ratio);

	strksharpenParamsSelected->mbf_gain      = INTERP1(strksharpenParams->mbf_gain   [gain_low],  strksharpenParams->mbf_gain     [gain_high], ratio);
	strksharpenParamsSelected->mbf_add       = INTERP1(strksharpenParams->mbf_add    [gain_low],  strksharpenParams->mbf_add      [gain_high], ratio);

	strksharpenParamsSelected->hbf_gain      = INTERP1(strksharpenParams->hbf_gain   [gain_low],  strksharpenParams->hbf_gain     [gain_high], ratio);
	strksharpenParamsSelected->hbf_add       = INTERP1(strksharpenParams->hbf_add    [gain_low],  strksharpenParams->hbf_add      [gain_high], ratio);

    for(int i = 0;i < sizeof(strksharpenParamsSelected->gaus_luma_kernel) / sizeof(strksharpenParamsSelected->gaus_luma_kernel[0]); i++)
        strksharpenParamsSelected->gaus_luma_kernel[i]    = INTERP1(strksharpenParams->gaus_luma_kernel [gain_low][i],  strksharpenParams->gaus_luma_kernel   [gain_high][i], ratio);
    for(int i = 0;i < sizeof(strksharpenParamsSelected->kernel_pbf) / sizeof(strksharpenParamsSelected->kernel_pbf[0]); i++)
        strksharpenParamsSelected->kernel_pbf[i]    = INTERP1(strksharpenParams->kernel_pbf [gain_low][i],  strksharpenParams->kernel_pbf   [gain_high][i], ratio);
    for(int i = 0;i < sizeof(strksharpenParamsSelected->h_rf_m) / sizeof(strksharpenParamsSelected->h_rf_m[0]); i++)
        strksharpenParamsSelected->h_rf_m[i]        = INTERP1(strksharpenParams->h_rf_m [gain_low][i],      strksharpenParams->h_rf_m       [gain_high][i], ratio);
    for(int i = 0;i < sizeof(strksharpenParamsSelected->kernel_mbf) / sizeof(strksharpenParamsSelected->kernel_mbf[0]); i++)
        strksharpenParamsSelected->kernel_mbf[i]    = INTERP1(strksharpenParams->kernel_mbf [gain_low][i],  strksharpenParams->kernel_mbf   [gain_high][i], ratio);
    for(int i = 0;i < sizeof(strksharpenParamsSelected->h_rf_h) / sizeof(strksharpenParamsSelected->h_rf_h[0]); i++)
        strksharpenParamsSelected->h_rf_h[i]        = INTERP1(strksharpenParams->h_rf_h [gain_low][i],      strksharpenParams->h_rf_h       [gain_high][i], ratio);
    for(int i = 0;i < sizeof(strksharpenParamsSelected->kernel_hbf) / sizeof(strksharpenParamsSelected->kernel_hbf[0]); i++)
        strksharpenParamsSelected->kernel_hbf[i]    = INTERP1(strksharpenParams->kernel_hbf [gain_low][i],  strksharpenParams->kernel_hbf   [gain_high][i], ratio);




}

void select_rk_sharpen_hw_v2_params_by_ISO(
	RKAsharp_Sharp_HW_V2_Params_t *strksharpenParams, 
	RKAsharp_Sharp_HW_V2_Params_Select_t *strksharpenParamsSelected, 
	int iso
)
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

	strksharpenParamsSelected->pbf_gain				= INTERP1(strksharpenParams->pbf_gain	[gain_low],		strksharpenParams->pbf_gain 	[gain_high], 	ratio);
	strksharpenParamsSelected->pbf_add				= INTERP1(strksharpenParams->pbf_add	[gain_low],		strksharpenParams->pbf_add		[gain_high], 	ratio);
	strksharpenParamsSelected->pbf_ratio            = INTERP1(strksharpenParams->pbf_ratio  [gain_low],     strksharpenParams->pbf_ratio    [gain_high],    ratio);
	strksharpenParamsSelected->lratio               = INTERP1(strksharpenParams->lratio     [gain_low],     strksharpenParams->lratio       [gain_high],    ratio);
	strksharpenParamsSelected->hratio               = INTERP1(strksharpenParams->hratio     [gain_low],     strksharpenParams->hratio       [gain_high],    ratio);
	strksharpenParamsSelected->sharp_ratio          = INTERP1(strksharpenParams->sharp_ratio[gain_low],     strksharpenParams->sharp_ratio  [gain_high],    ratio);	
	strksharpenParamsSelected->hbf_gain      		= INTERP1(strksharpenParams->hbf_gain   [gain_low],  	strksharpenParams->hbf_gain     [gain_high], 	ratio);
	strksharpenParamsSelected->hbf_add       		= INTERP1(strksharpenParams->hbf_add    [gain_low],  	strksharpenParams->hbf_add      [gain_high], 	ratio);
	strksharpenParamsSelected->hbf_ratio            = INTERP1(strksharpenParams->hbf_ratio  [gain_low],     strksharpenParams->hbf_ratio    [gain_high],    ratio);
	strksharpenParamsSelected->ehf_th               = (short)SHARP_ROUND_F(INTERP1(strksharpenParams->ehf_th     [gain_low],     strksharpenParams->ehf_th       [gain_high],    ratio));
	for(int i = 0; i < RK_EDGEFILTER_LUMA_POINT_NUM; i++)
	{
		strksharpenParamsSelected->luma_point[i]    = strksharpenParams->luma_point[i];
		strksharpenParamsSelected->luma_sigma[i]    = INTERP1(strksharpenParams->luma_sigma [gain_low][i],  strksharpenParams->luma_sigma   [gain_high][i], ratio);		
		strksharpenParamsSelected->lum_clip_h[i]    = (short)SHARP_ROUND_F(INTERP1(strksharpenParams->lum_clip_h  [gain_low][i],  strksharpenParams->lum_clip_h    [gain_high][i], ratio));
	}
	for(int i = 0;i < RKSHAPRENHW_V2_GAU_DIAM * RKSHAPRENHW_V2_GAU_DIAM; i++)
	{
		strksharpenParamsSelected->gaus_luma_kernel[i] = INTERP1(strksharpenParams->gaus_luma_kernel [gain_low][i],  strksharpenParams->gaus_luma_kernel[gain_high][i], ratio);
	}

	for(int i = 0;i < RKSHAPRENHW_V2_PBF_DIAM * RKSHAPRENHW_V2_PBF_DIAM; i++)
	{
		strksharpenParamsSelected->kernel_pre_bf[i] = INTERP1(strksharpenParams->kernel_pre_bf [gain_low][i], strksharpenParams->kernel_pre_bf[gain_high][i], ratio);
	}
	for(int i = 0;i < RKSHAPRENHW_V2_HRF_DIAM * RKSHAPRENHW_V2_HRF_DIAM; i++)
	{
		strksharpenParamsSelected->kernel_range_filter[i] = INTERP1(strksharpenParams->kernel_range_filter[gain_low][i], strksharpenParams->kernel_range_filter[gain_high][i], ratio);
	}
	for(int i = 0;i < RKSHAPRENHW_V2_HBF_DIAM_Y * RKSHAPRENHW_V2_HBF_DIAM_X; i++)
	{
		strksharpenParamsSelected->kernel_hf_bf[i] = INTERP1(strksharpenParams->kernel_hf_bf[gain_low][i],  strksharpenParams->kernel_hf_bf[gain_high][i], ratio);
	}
}

void select_rk_sharpen_hw_v3_params_by_ISO(
	RKAsharp_Sharp_HW_V3_Params_t *strksharpenParams, 
	RKAsharp_Sharp_HW_V3_Params_Select_t *strksharpenParamsSelected, 
	int iso
)
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

	strksharpenParamsSelected->lratio				= INTERP1(strksharpenParams->lratio		[gain_low],		strksharpenParams->lratio 		[gain_high], 	ratio);
	strksharpenParamsSelected->hratio				= INTERP1(strksharpenParams->hratio		[gain_low],		strksharpenParams->hratio		[gain_high], 	ratio);
	strksharpenParamsSelected->sharp_ratio          = INTERP1(strksharpenParams->sharp_ratio[gain_low],     strksharpenParams->sharp_ratio  [gain_high],    ratio);	
	strksharpenParamsSelected->ehf_th               = (short)SHARP_ROUND_F(INTERP1(strksharpenParams->ehf_th     [gain_low],     strksharpenParams->ehf_th       [gain_high],    ratio));
	for(int i = 0; i < RK_EDGEFILTER_LUMA_POINT_NUM; i++)
	{
		strksharpenParamsSelected->luma_point[i]    = strksharpenParams->luma_point[i];
		strksharpenParamsSelected->lum_clip_h[i]    = (short)SHARP_ROUND_F(INTERP1(strksharpenParams->lum_clip_h  [gain_low][i],  strksharpenParams->lum_clip_h    [gain_high][i], ratio));
	}
	for(int i = 0;i < RKSHAPRENHW_V3_RF_DIAM * RKSHAPRENHW_V3_RF_DIAM; i++)
	{
		strksharpenParamsSelected->kernel_range_filter[i] = INTERP1(strksharpenParams->kernel_range_filter [gain_low][i], strksharpenParams->kernel_range_filter[gain_high][i], ratio);
	}
}

void select_sharpen_params_by_ISO	(
	RKAsharp_Sharp_Params_t *strksharpenParams,		
	RKAsharp_Sharp_Params_Select_t *strksharpenParamsSelected, 
	int iso
)
{
	select_rk_sharpen_hw_params_by_ISO(&strksharpenParams->rk_sharpen_params_V1, &strksharpenParamsSelected->rk_sharpen_params_selected_V1, iso);
	
	select_rk_sharpen_hw_v2_params_by_ISO(&strksharpenParams->rk_sharpen_params_V2, &strksharpenParamsSelected->rk_sharpen_params_selected_V2, iso);
	
	select_rk_sharpen_hw_v3_params_by_ISO(&strksharpenParams->rk_sharpen_params_V3, &strksharpenParamsSelected->rk_sharpen_params_selected_V3, iso);
}

