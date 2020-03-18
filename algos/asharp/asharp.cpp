
#include "asharp.h"
#include "rk_aiq_algo_asharp_itf.h"

#define ASHARP_FIX_VALUE_PRINTF 1

//anr inint
AsharpResult_t AsharpInit(AsharpContext_t **ppAsharpCtx)
{
	AsharpContext_t * pAsharpCtx;

	LOGI_ASHARP("%s(%d): enter!\n", __FUNCTION__, __LINE__);
	
	pAsharpCtx = (AsharpContext_t *)malloc(sizeof(AsharpContext_t));
	if(pAsharpCtx == NULL){
		LOGE_ASHARP("%s(%d): malloc fail\n", __FUNCTION__, __LINE__);
		return ASHARP_RET_NULL_POINTER;
	}

	memset(pAsharpCtx, 0x00, sizeof(AsharpContext_t));
	pAsharpCtx->eState = ASHARP_STATE_INITIALIZED;

	//AsharpConfig(pAsharpCtx, pAsharpConfig);
	*ppAsharpCtx = pAsharpCtx;
	
	LOGI_ASHARP("%s(%d): exit!\n", __FUNCTION__, __LINE__);
	return ASHARP_RET_SUCCESS;
}

//anr release
AsharpResult_t AsharpRelease(AsharpContext_t *pAsharpCtx)
{
	LOGI_ASHARP("%s(%d): enter!\n", __FUNCTION__, __LINE__);
	if(pAsharpCtx == NULL){
		LOGE_ASHARP("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ASHARP_RET_NULL_POINTER;
	}

	memset(pAsharpCtx, 0x00, sizeof(AsharpContext_t));
	free(pAsharpCtx);

	LOGI_ASHARP("%s(%d): exit!\n", __FUNCTION__, __LINE__);
	return ASHARP_RET_SUCCESS;
}

//anr config
AsharpResult_t AsharpConfig(AsharpContext_t *pAsharpCtx, AsharpConfig_t* pAsharpConfig)
{
	LOGI_ASHARP("%s(%d): enter!\n", __FUNCTION__, __LINE__);
	
	if(pAsharpCtx == NULL){
		LOGE_ASHARP("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ASHARP_RET_INVALID_PARM;
	}

	if(pAsharpConfig == NULL){
		LOGE_ASHARP("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ASHARP_RET_INVALID_PARM;
	}

	if(pAsharpConfig->eMode == ASHARP_OP_MODE_AUTO){
		pAsharpCtx->stAuto.sharpEn = pAsharpConfig->stSharpCalib.enable;
		init_sharp_params_v1(&pAsharpCtx->stAuto.stSharpParam.rk_sharpen_params_V1, &pAsharpConfig->stSharpCalib);
		pAsharpCtx->stAuto.edgeFltEn = pAsharpConfig->stEdgeFltCalib.enable;
		init_edgefilter_params(&pAsharpCtx->stAuto.stEdgefilterParams, &pAsharpConfig->stEdgeFltCalib);
		//memcpy(&pAsharpCtx->stAuto.stSharpParam, &pAsharpConfig->stHtmlParams.stSharpParam, sizeof(RKAsharp_Sharp_Params_t));
		//memcpy(&pAsharpCtx->stAuto.stEdgefilterParams, &pAsharpConfig->stHtmlParams.stEdgefilterParams, sizeof(RKAsharp_EdgeFilter_Params_t));
		LOGD_ASHARP("%s(%d): sharp %f %f %f %f %f %f", __FUNCTION__, __LINE__, 
			pAsharpCtx->stAuto.stSharpParam.rk_sharpen_params_V1.hratio[0],
			pAsharpCtx->stAuto.stSharpParam.rk_sharpen_params_V1.lratio[0],
			pAsharpCtx->stAuto.stSharpParam.rk_sharpen_params_V1.H_ratio[0],
			pAsharpCtx->stAuto.stSharpParam.rk_sharpen_params_V1.M_ratio[0],
			pAsharpCtx->stAuto.stSharpParam.rk_sharpen_params_V1.hbf_gain[0],
			pAsharpCtx->stAuto.stSharpParam.rk_sharpen_params_V1.hbf_ratio[0]);
		
	}

	pAsharpCtx->eMode = pAsharpConfig->eMode;
	pAsharpCtx->eState = pAsharpConfig->eState;

	LOGI_ASHARP("%s(%d): exit!\n", __FUNCTION__, __LINE__);
	return ASHARP_RET_SUCCESS;
}

//anr reconfig
AsharpResult_t AsharpReConfig(AsharpContext_t *pAsharpCtx, AsharpConfig_t* pAsharpConfig)
{
	LOGI_ASHARP("%s(%d): enter!\n", __FUNCTION__, __LINE__);
	//need todo what?

	LOGI_ASHARP("%s(%d): exit!\n", __FUNCTION__, __LINE__);
	return ASHARP_RET_SUCCESS;
}

//anr preprocess
AsharpResult_t AsharpPreProcess(AsharpContext_t *pAsharpCtx)
{
	LOGI_ASHARP("%s(%d): enter!\n", __FUNCTION__, __LINE__);
	//need todo what?

	LOGI_ASHARP("%s(%d): exit!\n", __FUNCTION__, __LINE__);
	return ASHARP_RET_SUCCESS;
}

//anr process
AsharpResult_t AsharpProcess(AsharpContext_t *pAsharpCtx, int ISO)
{	
	LOGI_ASHARP("%s(%d): enter!\n", __FUNCTION__, __LINE__);

	if(pAsharpCtx == NULL){
		LOGE_ASHARP("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ASHARP_RET_INVALID_PARM;
	}
	
	if(pAsharpCtx->eMode == ASHARP_OP_MODE_AUTO){

		LOGD_ASHARP("%s(%d): ISO:%d \n", __FUNCTION__, __LINE__, ISO);
		//select param
		select_sharpen_params_by_ISO(&pAsharpCtx->stAuto.stSharpParam, &pAsharpCtx->stAuto.stSharpParamSelect, ISO);
		select_edgefilter_params_by_ISO(&pAsharpCtx->stAuto.stEdgefilterParams, &pAsharpCtx->stAuto.stEdgefilterParamSelect, ISO);

		#if ASHARP_FIX_VALUE_PRINTF
		ASharp_fix_transfer(&pAsharpCtx->stAuto.stSharpParamSelect, &pAsharpCtx->stAuto.stEdgefilterParamSelect);
		#endif
	}else if(pAsharpCtx->eMode == ASHARP_OP_MODE_MANUAL){
		//TODO
	}
	
	LOGI_ASHARP("%s(%d): exit!\n", __FUNCTION__, __LINE__);
	return ASHARP_RET_SUCCESS;

}

//anr get result
AsharpResult_t AsharpGetProcResult(AsharpContext_t *pAsharpCtx, AsharpProcResult_t* pAsharpResult)
{
	LOGI_ASHARP("%s(%d): enter!\n", __FUNCTION__, __LINE__);

	if(pAsharpCtx == NULL){
		LOGE_ASHARP("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ASHARP_RET_INVALID_PARM;
	}

	if(pAsharpResult == NULL){
		LOGE_ASHARP("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ASHARP_RET_INVALID_PARM;
	}
	
	if(pAsharpCtx->eMode == ASHARP_OP_MODE_AUTO){
		pAsharpResult->sharpEn = pAsharpCtx->stAuto.sharpEn;
		pAsharpResult->stSharpParamSelect = pAsharpCtx->stAuto.stSharpParamSelect;
		pAsharpResult->edgeFltEn = pAsharpCtx->stAuto.edgeFltEn;
		pAsharpResult->stEdgefilterParamSelect = pAsharpCtx->stAuto.stEdgefilterParamSelect;		
	}else if(pAsharpCtx->eMode == ASHARP_OP_MODE_MANUAL){
		//TODO
		pAsharpResult->sharpEn = pAsharpCtx->stManual.sharpEn;
		pAsharpResult->stSharpParamSelect = pAsharpCtx->stManual.stSharpParamSelect;
		pAsharpResult->edgeFltEn = pAsharpCtx->stManual.edgeFltEn;
		pAsharpResult->stEdgefilterParamSelect = pAsharpCtx->stManual.stEdgefilterParamSelect;
	}
	
	LOGI_ASHARP("%s(%d): exit!\n", __FUNCTION__, __LINE__);
	return ASHARP_RET_SUCCESS;
}


#define ROUND_F(x) (int)(((float)x)+(((x) > 0) ? 0.5 : (-0.5)))
#define	FLOOR(a)   (int)( ((double)(a) < (int)(a)) ? (int)((a)-1) : (int)(a) )
//fix
#define reg_sharpenHW_lratio_fix_bits				8
#define reg_sharpenHW_hratio_fix_bits				8
#define reg_sharpenHW_M_ratio_fix_bits				2
#define reg_sharpenHW_H_ratio_fix_bits				2
#define reg_sharpenHW_pbf_ratio_fix_bits			7
#define reg_sharpenHW_lum_min_m_fix_bits			4
#define reg_sharpenHW_hbf_ratio_fix_bits			8
#define reg_sharpenHW_pPBfCoeff_fix_bits			6
#define reg_sharpenHW_pMRfCoeff_fix_bits			7
#define reg_sharpenHW_pMBfCoeff_fix_bits			6
#define reg_sharpenHW_pHRfCoeff_fix_bits			7
#define reg_sharpenHW_pHBfCoeff_fix_bits			6

//fix
#define RK_EDGEFILTER_COEF_BIT      				(6)          // coef Quantilize bits
#define	reg_dir_min_fix_bits             		RK_EDGEFILTER_COEF_BIT
#define	reg_l_alpha_fix_bits             		8
#define	reg_g_alpha_fix_bits             		8 
#define	reg_detail_alpha_dog_fix_bits    		6
#define	reg_h0_h_coef_5x5_fix_bits       		6
#define	reg_h_coef_5x5_fix_bits          		6
#define	reg_gf_coef_3x3_fix_bits         		4
#define	reg_dog_kernel_fix_bits          		6
#define	reg_smoth4_fix_bits						8	


void ASharp_fix_transfer(RKAsharp_Sharp_Params_Select_t* sharp, RKAsharp_EdgeFilter_Params_Select_t* edgeflt)
{
	int i = 0;
	int k = 0;
	int tmp = 0;
	int sum_coeff, offset;
	float sum_coeff_float;
	rkispp_sharp_fix_t* pSharpCfg = (rkispp_sharp_fix_t *)malloc(sizeof(rkispp_sharp_fix_t));
	RKAsharp_Sharp_HW_Params_Select_t *pSharpV1 = &sharp->rk_sharpen_params_selected_V1;

	//0x0080
	pSharpCfg->alpha_adp_en = edgeflt->alpha_adp_en;
	pSharpCfg->yin_flt_en = 0;
	pSharpCfg->edge_avg_en = 0;
	
	//0x0084
	pSharpCfg->hbf_ratio = (unsigned short)ROUND_F(pSharpV1->hbf_ratio * (1 << reg_sharpenHW_hbf_ratio_fix_bits)); 
	pSharpCfg->ehf_th = (unsigned short)ROUND_F(pSharpV1->ehf_th); 
	pSharpCfg->pbf_ratio = (unsigned short)ROUND_F(pSharpV1->pbf_ratio * (1 << reg_sharpenHW_pbf_ratio_fix_bits));

	//0x0088
	pSharpCfg->edge_thed = edgeflt->edge_thed;
	pSharpCfg->dir_min = (unsigned char)ROUND_F(edgeflt->dir_min * (1 << reg_dir_min_fix_bits));	
	pSharpCfg->smoth_th4 = (unsigned short)ROUND_F(edgeflt->smoth4 * (1 << reg_smoth4_fix_bits));

	//0x008c
	pSharpCfg->l_alpha = (unsigned short)ROUND_F(edgeflt->l_alpha * (1 << reg_l_alpha_fix_bits));	
	pSharpCfg->g_alpha = (unsigned short)ROUND_F(edgeflt->g_alpha * (1 << reg_g_alpha_fix_bits)); 
	

	//0x0090
	sum_coeff = 0;
	sum_coeff_float = 0;
	pSharpCfg->pbf_k[0] = (unsigned char)ROUND_F(pSharpV1->kernel_pbf[4] * (1 << reg_sharpenHW_pPBfCoeff_fix_bits));
	pSharpCfg->pbf_k[1] = (unsigned char)ROUND_F(pSharpV1->kernel_pbf[1] * (1 << reg_sharpenHW_pPBfCoeff_fix_bits));
	pSharpCfg->pbf_k[2] = (unsigned char)ROUND_F(pSharpV1->kernel_pbf[0] * (1 << reg_sharpenHW_pPBfCoeff_fix_bits));	
	for(k = 0; k <RKSHAPRENHW_PBF_DIAM * RKSHAPRENHW_PBF_DIAM; k ++)
	{
		sum_coeff += ROUND_F(pSharpV1->kernel_pbf[k] * (1 << reg_sharpenHW_pPBfCoeff_fix_bits));
		sum_coeff_float += pSharpV1->kernel_pbf[k];
	}
	offset = int(sum_coeff_float * (1 << reg_sharpenHW_pPBfCoeff_fix_bits)) - sum_coeff;
	pSharpCfg->pbf_k[0] = pSharpCfg->pbf_k[0] + offset;
	

	//0x0094 - 0x0098
	sum_coeff = 0;
	sum_coeff_float = 0;
	pSharpCfg->mrf_k[0] = (unsigned char)ROUND_F(pSharpV1->h_rf_m[12] * (1 << reg_sharpenHW_pMRfCoeff_fix_bits));
	pSharpCfg->mrf_k[1] = (unsigned char)ROUND_F(pSharpV1->h_rf_m[7] * (1 << reg_sharpenHW_pMRfCoeff_fix_bits));
	pSharpCfg->mrf_k[2] = (unsigned char)ROUND_F(pSharpV1->h_rf_m[6] * (1 << reg_sharpenHW_pMRfCoeff_fix_bits));
	pSharpCfg->mrf_k[3] = (unsigned char)ROUND_F(pSharpV1->h_rf_m[2] * (1 << reg_sharpenHW_pMRfCoeff_fix_bits));
	pSharpCfg->mrf_k[4] = (unsigned char)ROUND_F(pSharpV1->h_rf_m[1] * (1 << reg_sharpenHW_pMRfCoeff_fix_bits));
	pSharpCfg->mrf_k[5] = (unsigned char)ROUND_F(pSharpV1->h_rf_m[0] * (1 << reg_sharpenHW_pMRfCoeff_fix_bits));
	for(k = 0; k <RKSHAPRENHW_MRF_DIAM * RKSHAPRENHW_MRF_DIAM; k ++)
	{
		sum_coeff += ROUND_F(pSharpV1->h_rf_m[k] * (1 << reg_sharpenHW_pMRfCoeff_fix_bits));
		sum_coeff_float += pSharpV1->h_rf_m[k];
	}
	offset = int(sum_coeff_float * (1 << reg_sharpenHW_pMRfCoeff_fix_bits)) - sum_coeff;
	pSharpCfg->mrf_k[0] = pSharpCfg->mrf_k[0] + offset;

	//0x009c -0x00a4
	sum_coeff = 0;
	sum_coeff_float = 0;
	pSharpCfg->mbf_k[0] = (unsigned char)ROUND_F(pSharpV1->kernel_mbf[110] * (1 << reg_sharpenHW_pMBfCoeff_fix_bits));
	pSharpCfg->mbf_k[1] = (unsigned char)ROUND_F(pSharpV1->kernel_mbf[93] * (1 << reg_sharpenHW_pMBfCoeff_fix_bits));
	pSharpCfg->mbf_k[2] = (unsigned char)ROUND_F(pSharpV1->kernel_mbf[92] * (1 << reg_sharpenHW_pMBfCoeff_fix_bits));
	pSharpCfg->mbf_k[3] = (unsigned char)ROUND_F(pSharpV1->kernel_mbf[76] * (1 << reg_sharpenHW_pMBfCoeff_fix_bits));
	pSharpCfg->mbf_k[4] = (unsigned char)ROUND_F(pSharpV1->kernel_mbf[58] * (1 << reg_sharpenHW_pMBfCoeff_fix_bits));
	pSharpCfg->mbf_k[5] = (unsigned char)ROUND_F(pSharpV1->kernel_mbf[56] * (1 << reg_sharpenHW_pMBfCoeff_fix_bits));
	pSharpCfg->mbf_k[6] = (unsigned char)ROUND_F(pSharpV1->kernel_mbf[25] * (1 << reg_sharpenHW_pMBfCoeff_fix_bits));
	pSharpCfg->mbf_k[7] = (unsigned char)ROUND_F(pSharpV1->kernel_mbf[23] * (1 << reg_sharpenHW_pMBfCoeff_fix_bits));
	pSharpCfg->mbf_k[8] = (unsigned char)ROUND_F(pSharpV1->kernel_mbf[38] * (1 << reg_sharpenHW_pMBfCoeff_fix_bits));
	pSharpCfg->mbf_k[9] = (unsigned char)ROUND_F(pSharpV1->kernel_mbf[4] * (1 << reg_sharpenHW_pMBfCoeff_fix_bits));
	pSharpCfg->mbf_k[10] = (unsigned char)ROUND_F(pSharpV1->kernel_mbf[69] * (1 << reg_sharpenHW_pMBfCoeff_fix_bits));
	pSharpCfg->mbf_k[11] = (unsigned char)ROUND_F(pSharpV1->kernel_mbf[102] * (1 << reg_sharpenHW_pMBfCoeff_fix_bits));
	for(k = 0; k <RKSHAPRENHW_MBF_DIAM_Y * RKSHAPRENHW_MBF_DIAM_X; k ++)
	{
		sum_coeff += ROUND_F(pSharpV1->kernel_mbf[k] * (1 << reg_sharpenHW_pMBfCoeff_fix_bits));
		sum_coeff_float += pSharpV1->kernel_mbf[k];
	}
	offset = int(sum_coeff_float * (1 << reg_sharpenHW_pMBfCoeff_fix_bits)) - sum_coeff;
	pSharpCfg->mbf_k[0] = pSharpCfg->mbf_k[0] + offset;
	
	//0x00a8 -0x00ac
	sum_coeff = 0;
	sum_coeff_float = 0;
	pSharpCfg->hrf_k[0] = (unsigned char)ROUND_F(pSharpV1->h_rf_h[12] * (1 << reg_sharpenHW_pHRfCoeff_fix_bits));
	pSharpCfg->hrf_k[1] = (unsigned char)ROUND_F(pSharpV1->h_rf_h[7] * (1 << reg_sharpenHW_pHRfCoeff_fix_bits));
	pSharpCfg->hrf_k[2] = (unsigned char)ROUND_F(pSharpV1->h_rf_h[6] * (1 << reg_sharpenHW_pHRfCoeff_fix_bits));
	pSharpCfg->hrf_k[3] = (unsigned char)ROUND_F(pSharpV1->h_rf_h[2] * (1 << reg_sharpenHW_pHRfCoeff_fix_bits));
	pSharpCfg->hrf_k[4] = (unsigned char)ROUND_F(pSharpV1->h_rf_h[1] * (1 << reg_sharpenHW_pHRfCoeff_fix_bits));
	pSharpCfg->hrf_k[5] = (unsigned char)ROUND_F(pSharpV1->h_rf_h[0] * (1 << reg_sharpenHW_pHRfCoeff_fix_bits));
	for(k = 0; k < RKSHAPRENHW_HRF_DIAM * RKSHAPRENHW_HRF_DIAM; k ++)
	{
		sum_coeff += ROUND_F(pSharpV1->h_rf_h[k] * (1 << reg_sharpenHW_pHRfCoeff_fix_bits));
		sum_coeff_float += pSharpV1->h_rf_h[k];
	}
	offset = int(sum_coeff_float * (1 << reg_sharpenHW_pHRfCoeff_fix_bits)) - sum_coeff;
	pSharpCfg->hrf_k[0] = pSharpCfg->hrf_k[0] + offset;

	//0x00b0
	sum_coeff = 0;
	sum_coeff_float = 0;
	pSharpCfg->hbf_k[0] = (unsigned char)ROUND_F(pSharpV1->kernel_hbf[4] * (1 << reg_sharpenHW_pHBfCoeff_fix_bits));
	pSharpCfg->hbf_k[1] = (unsigned char)ROUND_F(pSharpV1->kernel_hbf[1] * (1 << reg_sharpenHW_pHBfCoeff_fix_bits));
	pSharpCfg->hbf_k[2] = (unsigned char)ROUND_F(pSharpV1->kernel_hbf[0] * (1 << reg_sharpenHW_pHBfCoeff_fix_bits));
	for(k = 0; k < RKSHAPRENHW_HBF_DIAM * RKSHAPRENHW_HBF_DIAM; k ++)
	{
		sum_coeff += ROUND_F(pSharpV1->kernel_hbf[k] * (1 << reg_sharpenHW_pHBfCoeff_fix_bits));
		sum_coeff_float += pSharpV1->kernel_hbf[k];
	}
	offset = int(sum_coeff_float * (1 << reg_sharpenHW_pHBfCoeff_fix_bits)) - sum_coeff;
	pSharpCfg->hbf_k[0] = pSharpCfg->hbf_k[0] + offset;
	
	//0x00b4
	pSharpCfg->eg_coef[0] = (char)ROUND_F(edgeflt->gf_coef_3x3[4] * (1 << reg_gf_coef_3x3_fix_bits));
	pSharpCfg->eg_coef[1] = (char)ROUND_F(edgeflt->gf_coef_3x3[1] * (1 << reg_gf_coef_3x3_fix_bits));
	pSharpCfg->eg_coef[2] = (char)ROUND_F(edgeflt->gf_coef_3x3[0] * (1 << reg_gf_coef_3x3_fix_bits));	
	sum_coeff = 0;
	sum_coeff_float = 0;
	for(int k = 0; k <RKEDGEFILTER_SHRP_DIAM 		* RKEDGEFILTER_SHRP_DIAM; k ++)
	{
		sum_coeff += ROUND_F(edgeflt->gf_coef_3x3[k] * (1 << reg_gf_coef_3x3_fix_bits));
		sum_coeff_float += edgeflt->gf_coef_3x3[k];
	}
	offset = int(sum_coeff_float * (1 << reg_gf_coef_3x3_fix_bits)) - sum_coeff;
	pSharpCfg->eg_coef[0] = pSharpCfg->eg_coef[0] + offset;
	
	//0x00b8
	pSharpCfg->eg_smoth[0] = (unsigned char)ROUND_F(edgeflt->h0_h_coef_5x5[2 * RKEDGEFILTER_DIR_SMTH_DIAM + 2] * (1 << reg_h0_h_coef_5x5_fix_bits));
	pSharpCfg->eg_smoth[1] = (unsigned char)ROUND_F(edgeflt->h0_h_coef_5x5[2 * RKEDGEFILTER_DIR_SMTH_DIAM + 1] * (1 << reg_h0_h_coef_5x5_fix_bits));
	pSharpCfg->eg_smoth[2] = (unsigned char)ROUND_F(edgeflt->h0_h_coef_5x5[2 * RKEDGEFILTER_DIR_SMTH_DIAM + 0] * (1 << reg_h0_h_coef_5x5_fix_bits));
	sum_coeff = 0;
	sum_coeff_float = 0;
	for(int k = 0; k <RKEDGEFILTER_DIR_SMTH_DIAM * RKEDGEFILTER_DIR_SMTH_DIAM; k ++)
	{
		sum_coeff += ROUND_F(edgeflt->h0_h_coef_5x5[k] * (1 << reg_h0_h_coef_5x5_fix_bits));
		sum_coeff_float += edgeflt->h0_h_coef_5x5[k];
	}
	offset = int(sum_coeff_float * (1 << reg_h0_h_coef_5x5_fix_bits)) - sum_coeff;
	pSharpCfg->eg_smoth[0] = pSharpCfg->eg_smoth[0] + offset;

	//0x00bc - 0x00c0
	pSharpCfg->eg_gaus[0] = (unsigned char)ROUND_F(edgeflt->h_coef_5x5[12] * (1 << reg_h_coef_5x5_fix_bits));
	pSharpCfg->eg_gaus[1] = (unsigned char)ROUND_F(edgeflt->h_coef_5x5[7] * (1 << reg_h_coef_5x5_fix_bits));
	pSharpCfg->eg_gaus[2] = (unsigned char)ROUND_F(edgeflt->h_coef_5x5[6] * (1 << reg_h_coef_5x5_fix_bits));
	pSharpCfg->eg_gaus[3] = (unsigned char)ROUND_F(edgeflt->h_coef_5x5[2] * (1 << reg_h_coef_5x5_fix_bits));
	pSharpCfg->eg_gaus[4] = (unsigned char)ROUND_F(edgeflt->h_coef_5x5[1] * (1 << reg_h_coef_5x5_fix_bits));
	pSharpCfg->eg_gaus[5] = (unsigned char)ROUND_F(edgeflt->h_coef_5x5[0] * (1 << reg_h_coef_5x5_fix_bits));
	sum_coeff = 0;
	sum_coeff_float = 0;
	for(int k = 0; k <RKEDGEFILTER_GAUS_DIAM * RKEDGEFILTER_GAUS_DIAM; k ++)
	{
		sum_coeff += ROUND_F(edgeflt->h_coef_5x5[k] * (1 << reg_h_coef_5x5_fix_bits));
		sum_coeff_float += edgeflt->h_coef_5x5[k];
	}
	offset = int(sum_coeff_float * (1 << reg_h_coef_5x5_fix_bits)) - sum_coeff;
	pSharpCfg->eg_gaus[0] = pSharpCfg->eg_gaus[0] + offset;
	
	//0x00c4 - 0x00c8
	pSharpCfg->dog_k[0] = ( char)ROUND_F(edgeflt->dog_kernel[12] * (1 << reg_dog_kernel_fix_bits));
	pSharpCfg->dog_k[1] = ( char)ROUND_F(edgeflt->dog_kernel[7] * (1 << reg_dog_kernel_fix_bits));
	pSharpCfg->dog_k[2] = ( char)ROUND_F(edgeflt->dog_kernel[6] * (1 << reg_dog_kernel_fix_bits));
	pSharpCfg->dog_k[3] = ( char)ROUND_F(edgeflt->dog_kernel[2] * (1 << reg_dog_kernel_fix_bits));
	pSharpCfg->dog_k[4] = ( char)ROUND_F(edgeflt->dog_kernel[1] * (1 << reg_dog_kernel_fix_bits));
	pSharpCfg->dog_k[5] = ( char)ROUND_F(edgeflt->dog_kernel[0] * (1 << reg_dog_kernel_fix_bits));
	sum_coeff = 0;
	sum_coeff_float = 0;
	for(int k = 0; k <RKEDGEFILTER_DOG_DIAM * RKEDGEFILTER_DOG_DIAM; k ++)
	{
		sum_coeff += ROUND_F(edgeflt->dog_kernel[k] * (1 << reg_dog_kernel_fix_bits));
		sum_coeff_float += edgeflt->dog_kernel[k];
	}
	offset = int(sum_coeff_float * (1 << reg_dog_kernel_fix_bits)) - sum_coeff;
	pSharpCfg->dog_k[0] = pSharpCfg->dog_k[0] + offset;

	//0x00cc - 0x00d0
	for(i=0; i<6; i++){
		pSharpCfg->lum_point[i] = (unsigned char)ROUND_F(pSharpV1->luma_point[i+1]);
	}
	
	//0x00d4
	//pbf
	int sigma_deci_bits = 7;
	int sigma_inte_bits = 1;
	int max_val			= 0;
	int min_val			= 65536;
	int shf_bits		= 0;
	short sigma_bits[3];
	for(int i = 0; i < 8; i++)
	{
		int cur_sigma = FLOOR((pSharpV1->luma_sigma[i] * pSharpV1->pbf_gain + pSharpV1->pbf_add));
		if(max_val < cur_sigma)
			max_val = cur_sigma;
		if(min_val > cur_sigma)
			min_val = cur_sigma;
	}
	sigma_bits[0] = FLOOR(log((float)min_val)/log((float)2));
	sigma_bits[1] = MAX(sigma_inte_bits - sigma_bits[0], 0);
	sigma_bits[2] = sigma_deci_bits + sigma_bits[0];
	pSharpCfg->pbf_shf_bits = sigma_bits[2] - 5;

	// mf bf sigma inv
	max_val = 0;
	min_val = 65536;
	shf_bits = 0;
	for(int i = 0; i < 8; i++)
	{
		int cur_sigma = FLOOR(pSharpV1->luma_sigma[i] 
						* pSharpV1->mbf_gain 
						+ pSharpV1->mbf_add);
		if(max_val < cur_sigma)
			max_val = cur_sigma;
		if(min_val > cur_sigma)
			min_val = cur_sigma;
	}
	sigma_bits[0] = FLOOR(log((float)min_val)/log((float)2));
	sigma_bits[1] = MAX(sigma_inte_bits - sigma_bits[0], 0);
	sigma_bits[2] = sigma_deci_bits + sigma_bits[0];
	pSharpCfg->mbf_shf_bits = (unsigned char)(sigma_bits[2] - 5);

	// hf bf sigma inv
	max_val = 0;
	min_val = 65536;
	shf_bits = 0;
	for(int i = 0; i < 8; i++)
	{
		int cur_sigma = FLOOR(pSharpV1->luma_sigma[i] * pSharpV1->hbf_gain + pSharpV1->hbf_add);
		if(max_val < cur_sigma)
			max_val = cur_sigma;
		if(min_val > cur_sigma)
			min_val = cur_sigma;
	}
	sigma_bits[0] = FLOOR(log((float)min_val)/log((float)2));
	sigma_bits[1] = MAX(sigma_inte_bits - sigma_bits[0], 0);
	sigma_bits[2] = sigma_deci_bits + sigma_bits[0];
	pSharpCfg->hbf_shf_bits = (unsigned char)(sigma_bits[2] - 5);
	

	//0x00d8 - 0x00dc
	// pre bf sigma inv
	sigma_deci_bits = 7;
	sigma_inte_bits = 1;
	max_val			= 0;
	min_val			= 65536;
	shf_bits		= 0;
	for(int i = 0; i < RK_SHARPFILTER_LUMA_POINT_NUM; i++)
	{
		int cur_sigma = FLOOR((pSharpV1->luma_sigma[i] * pSharpV1->pbf_gain + pSharpV1->pbf_add));
		if(max_val < cur_sigma)
			max_val = cur_sigma;
		if(min_val > cur_sigma)
			min_val = cur_sigma;
	}
	sigma_bits[0] = FLOOR(log((float)min_val)/log((float)2));
	sigma_bits[1] = MAX(sigma_inte_bits - sigma_bits[0], 0);
	sigma_bits[2] = sigma_deci_bits + sigma_bits[0];

	for(i=0; i<8; i++){
		pSharpCfg->pbf_sigma[i] = (unsigned char)ROUND_F((float)1 
								/ (pSharpV1->luma_sigma[i] * pSharpV1->pbf_gain + pSharpV1->pbf_add)  
								* (1 << sigma_bits[2]));
	}

	//0x00e0 - 0x00e4
	for(i=0; i<8; i++){
		pSharpCfg->lum_clp_m[i] = (unsigned char)(pSharpV1->lum_clp_m[i]);
	}

	//0x00e8 - 0x00ec
	for(i=0; i<8; i++){
		pSharpCfg->lum_min_m[i] = (char)ROUND_F(pSharpV1->lum_min_m[i] * (1 << reg_sharpenHW_lum_min_m_fix_bits));
	}

	//0x00f0 - 0x00f4
	// mf bf sigma inv
	max_val = 0;
	min_val = 65536;
	shf_bits = 0;
	for(int i = 0; i < RK_SHARPFILTER_LUMA_POINT_NUM; i++)
	{
		int cur_sigma = FLOOR(pSharpV1->luma_sigma[i] * pSharpV1->mbf_gain + pSharpV1->mbf_add);
		if(max_val < cur_sigma)
			max_val = cur_sigma;
		if(min_val > cur_sigma)
			min_val = cur_sigma;
	}
	sigma_bits[0] = FLOOR(log((float)min_val)/log((float)2));
	sigma_bits[1] = MAX(sigma_inte_bits - sigma_bits[0], 0);
	sigma_bits[2] = sigma_deci_bits + sigma_bits[0];

	for(i=0; i<8; i++){
		pSharpCfg->mbf_sigma[i] = (unsigned char)ROUND_F((float)1
								/ (pSharpV1->luma_sigma[i] * pSharpV1->mbf_gain + pSharpV1->mbf_add) 
								* (1 << sigma_bits[2]));
	}

	//0x00f8 - 0x00fc
	for(i=0; i<8; i++){
		pSharpCfg->lum_clp_h[i] = (unsigned char)(pSharpV1->lum_clp_h[i]);
	}

	//0x0100 - 0x0104
	//hbf
	max_val = 0;
	min_val = 65536;
	shf_bits = 0;
	for(int i = 0; i < RK_SHARPFILTER_LUMA_POINT_NUM; i++)
	{
		int cur_sigma = FLOOR(pSharpV1->luma_sigma[i] * pSharpV1->hbf_gain + pSharpV1->hbf_add);
		if(max_val < cur_sigma)
			max_val = cur_sigma;
		if(min_val > cur_sigma)
			min_val = cur_sigma;
	}
	sigma_bits[0] = FLOOR(log((float)min_val)/log((float)2));
	sigma_bits[1] = MAX(sigma_inte_bits - sigma_bits[0], 0);
	sigma_bits[2] = sigma_deci_bits + sigma_bits[0];

	for(i=0; i<8; i++){
		pSharpCfg->hbf_sigma[i] = (unsigned char)ROUND_F((float)1 
								/ (pSharpV1->luma_sigma[i] * pSharpV1->hbf_gain + pSharpV1->hbf_add) 
								* (1 << sigma_bits[2]));
	}

	//0x0108 - 0x010c
	for(i=0; i<8; i++){
		pSharpCfg->edge_lum_thed[i] = (unsigned char)edgeflt->edge_thed_1[i];
	}

	//0x0110 - 0x0114
	for(i=0; i<8; i++){
		pSharpCfg->clamp_pos[i] = (unsigned char)edgeflt->clamp_pos_dog[i];
	}

	//0x0118 - 0x011c
	for(i=0; i<8; i++){
		pSharpCfg->clamp_neg[i] = (unsigned char)edgeflt->clamp_neg_dog[i];
	}

	//0x0120 - 0x0124
	for(i=0; i<8; i++){
		pSharpCfg->detail_alpha[i] = (unsigned char)ROUND_F(edgeflt->detail_alpha_dog[i] * (1 << reg_detail_alpha_dog_fix_bits));
	}

	//0x0128
	pSharpCfg->rfl_ratio = (unsigned short)ROUND_F(pSharpV1->lratio * (1 << reg_sharpenHW_lratio_fix_bits));
	pSharpCfg->rfh_ratio = (unsigned short)ROUND_F(pSharpV1->hratio * (1 << reg_sharpenHW_hratio_fix_bits)); 

	//0x012C
	pSharpCfg->m_ratio = (unsigned char)ROUND_F(pSharpV1->M_ratio * (1 << reg_sharpenHW_M_ratio_fix_bits)); 
	pSharpCfg->h_ratio = (unsigned char)ROUND_F(pSharpV1->H_ratio * (1 << reg_sharpenHW_H_ratio_fix_bits));

	Asharp_fix_Printf(pSharpCfg);
}

void Asharp_fix_Printf(rkispp_sharp_fix_t  * pSharpCfg)
{
	printf("%s:(%d) enter \n", __FUNCTION__, __LINE__);
	int i = 0;
	
	//0x0080
	printf("(0x0080) alpha_adp_en:%d yin_flt_en:%d edge_avg_en:%d\n",
			pSharpCfg->alpha_adp_en,
			pSharpCfg->yin_flt_en,
			pSharpCfg->edge_avg_en);
	
	
	//0x0084
	printf("(0x0084) hbf_ratio:%d ehf_th:%d pbf_ratio:%d\n",
			pSharpCfg->hbf_ratio,
			pSharpCfg->ehf_th,
			pSharpCfg->pbf_ratio);

	//0x0088
	printf("(0x0088) edge_thed:%d dir_min:%d smoth_th4:%d\n",
			pSharpCfg->edge_thed,
			pSharpCfg->dir_min,
			pSharpCfg->smoth_th4);

	//0x008c
	printf("(0x008c) l_alpha:%d g_alpha:%d \n",
			pSharpCfg->l_alpha,
			pSharpCfg->g_alpha);
	

	//0x0090
	for(i = 0; i < 3; i++){
		printf("(0x0090) pbf_k[%d]:%d  \n",
			i, pSharpCfg->pbf_k[i]);
	}

	

	//0x0094 - 0x0098
	for(i = 0; i < 6; i++){
		printf("(0x0094 - 0x0098) mrf_k[%d]:%d  \n",
			i, pSharpCfg->mrf_k[i]);
	}
	

	//0x009c -0x00a4
	for(i = 0; i < 12; i++){
		printf("(0x009c -0x00a4) mbf_k[%d]:%d  \n",
			i, pSharpCfg->mbf_k[i]);
	}
	
	
	//0x00a8 -0x00ac
	for(i = 0; i < 6; i++){
		printf("(0x00a8 -0x00ac) hrf_k[%d]:%d  \n",
			i, pSharpCfg->hrf_k[i]);
	}
	

	//0x00b0
	for(i = 0; i < 3; i++){
		printf("(0x00b0) hbf_k[%d]:%d  \n",
			i, pSharpCfg->hbf_k[i]);
	}

	
	//0x00b4
	for(i = 0; i < 3; i++){
		printf("(0x00b4) eg_coef[%d]:%d  \n",
			i, pSharpCfg->eg_coef[i]);
	}

	//0x00b8
	for(i = 0; i < 3; i++){
		printf("(0x00b8) eg_smoth[%d]:%d  \n",
			i, pSharpCfg->eg_smoth[i]);
	}


	//0x00bc - 0x00c0
	for(i = 0; i < 6; i++){
		printf("(0x00bc - 0x00c0) eg_gaus[%d]:%d  \n",
			i, pSharpCfg->eg_gaus[i]);
	}
	

	//0x00c4 - 0x00c8
	for(i = 0; i < 6; i++){
		printf("(0x00c4 - 0x00c8) dog_k[%d]:%d  \n",
			i, pSharpCfg->dog_k[i]);
	}


	//0x00cc - 0x00d0
	for(i=0; i<6; i++){
		printf("(0x00cc - 0x00d0) lum_point[%d]:%d  \n",
			i, pSharpCfg->lum_point[i]);
	}
	
	//0x00d4
	printf("(0x00d4) pbf_shf_bits:%d  mbf_shf_bits:%d hbf_shf_bits:%d\n",
		pSharpCfg->pbf_shf_bits,
		pSharpCfg->mbf_shf_bits,
		pSharpCfg->hbf_shf_bits);
	

	//0x00d8 - 0x00dc
	for(i=0; i<8; i++){
		printf("(0x00d8 - 0x00dc) pbf_sigma[%d]:%d  \n",
			i, pSharpCfg->pbf_sigma[i]);
	}

	//0x00e0 - 0x00e4
	for(i=0; i<8; i++){
		printf("(0x00e0 - 0x00e4) lum_clp_m[%d]:%d  \n",
			i, pSharpCfg->lum_clp_m[i]);
	}

	//0x00e8 - 0x00ec
	for(i=0; i<8; i++){
		printf("(0x00e8 - 0x00ec) lum_min_m[%d]:%d  \n",
			i, pSharpCfg->lum_min_m[i]);	
	}

	//0x00f0 - 0x00f4
	for(i=0; i<8; i++){
		printf("(0x00f0 - 0x00f4) mbf_sigma[%d]:%d  \n",
			i, pSharpCfg->mbf_sigma[i]);	
	}

	//0x00f8 - 0x00fc
	for(i=0; i<8; i++){
		printf("(0x00f8 - 0x00fc) lum_clp_h[%d]:%d  \n",
			i, pSharpCfg->lum_clp_h[i]);	
	}

	//0x0100 - 0x0104
	for(i=0; i<8; i++){
		printf("(0x0100 - 0x0104) hbf_sigma[%d]:%d  \n",
			i, pSharpCfg->hbf_sigma[i]);
	}

	//0x0108 - 0x010c
	for(i=0; i<8; i++){
		printf("(0x0108 - 0x010c) edge_lum_thed[%d]:%d  \n",
			i, pSharpCfg->edge_lum_thed[i]);
	}

	//0x0110 - 0x0114
	for(i=0; i<8; i++){
		printf("(0x0110 - 0x0114) clamp_pos[%d]:%d  \n",
			i, pSharpCfg->clamp_pos[i]);
	}

	//0x0118 - 0x011c
	for(i=0; i<8; i++){
		printf("(0x0118 - 0x011c) clamp_neg[%d]:%d  \n",
			i, pSharpCfg->clamp_neg[i]);
	}

	//0x0120 - 0x0124
	for(i=0; i<8; i++){
		printf("(0x0120 - 0x0124) detail_alpha[%d]:%d  \n",
			i, pSharpCfg->detail_alpha[i]);
	}

	//0x0128
	printf("(0x0128) rfl_ratio:%d  rfh_ratio:%d\n",
			pSharpCfg->rfl_ratio, pSharpCfg->rfh_ratio);
	
	// mf/hf ratio

	//0x012C
	printf("(0x012C) m_ratio:%d  h_ratio:%d\n",
			pSharpCfg->m_ratio, pSharpCfg->h_ratio);
	
	printf("%s:(%d) exit \n", __FUNCTION__, __LINE__);
}


