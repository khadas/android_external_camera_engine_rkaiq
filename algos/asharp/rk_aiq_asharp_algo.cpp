
#include "rk_aiq_asharp_algo.h"
#include "rk_aiq_algo_asharp_itf.h"

RKAIQ_BEGIN_DECLARE

AsharpResult_t AsharpInit(AsharpContext_t **ppAsharpCtx, CamCalibDbContext_t *pCalibDb)
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
	*ppAsharpCtx = pAsharpCtx;

	//init params config
	pAsharpCtx->eMode = ASHARP_OP_MODE_AUTO;
	
	#if ASHARP_USE_XML_FILE
	//get v1 params from xml file
	pAsharpCtx->stSharpCalib = pCalibDb->sharp;
	pAsharpCtx->stEdgeFltCalib = pCalibDb->edgeFilter;
	#endif

	#ifdef RK_SIMULATOR_HW
	//get v2 params from html file
    FILE *fp2 = fopen("rkaiq_asharp_html_params.bin", "rb");
    if(fp2 != NULL) {
        memset(&pAsharpCtx->stAuto.stSharpParam, 0, sizeof(RKAsharp_Sharp_Params_t));
        fread(&pAsharpCtx->stAuto.stSharpParam, 1, sizeof(RKAsharp_Sharp_Params_t), fp2);
        memset(&pAsharpCtx->stAuto.stEdgefilterParams, 0, sizeof(RKAsharp_EdgeFilter_Params_t));
        fread(&pAsharpCtx->stAuto.stEdgefilterParams, 1, sizeof(RKAsharp_EdgeFilter_Params_t), fp2);
        fclose(fp2);
        LOGD_ASHARP("oyyf: %s:(%d) read sharp html param file sucess! \n", __FUNCTION__, __LINE__);
    } else {
        LOGE_ASHARP("oyyf: %s:(%d) read sharp html param file failed! \n", __FUNCTION__, __LINE__);
        return ASHARP_RET_FAILURE;
    }
	#endif

	
	#if ASHARP_USE_XML_FILE
	//convert V1 params from xml file & cover html v1 params
	pAsharpCtx->stAuto.sharpEn = pAsharpCtx->stSharpCalib.enable;
	init_sharp_params_v1(&pAsharpCtx->stAuto.stSharpParam.rk_sharpen_params_V1, &pAsharpCtx->stSharpCalib);
	pAsharpCtx->stAuto.edgeFltEn = pAsharpCtx->stEdgeFltCalib.enable;
	init_edgefilter_params(&pAsharpCtx->stAuto.stEdgefilterParams, &pAsharpCtx->stEdgeFltCalib);
	#endif

	printf("%s(%d): sharp %f %f %f %f %f %f", __FUNCTION__, __LINE__, 
			pAsharpCtx->stAuto.stSharpParam.rk_sharpen_params_V1.hratio[0],
			pAsharpCtx->stAuto.stSharpParam.rk_sharpen_params_V1.lratio[0],
			pAsharpCtx->stAuto.stSharpParam.rk_sharpen_params_V1.H_ratio[0],
			pAsharpCtx->stAuto.stSharpParam.rk_sharpen_params_V1.M_ratio[0],
			pAsharpCtx->stAuto.stSharpParam.rk_sharpen_params_V1.hbf_gain[0],
			pAsharpCtx->stAuto.stSharpParam.rk_sharpen_params_V1.hbf_ratio[0]);
	
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
AsharpResult_t AsharpPrepare(AsharpContext_t *pAsharpCtx, AsharpConfig_t* pAsharpConfig)
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

	//pAsharpCtx->eMode = pAsharpConfig->eMode;
	//pAsharpCtx->eState = pAsharpConfig->eState;

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
		pAsharpResult->edgeFltEn = pAsharpCtx->stAuto.edgeFltEn;
		pAsharpResult->stSharpParamSelect = pAsharpCtx->stAuto.stSharpParamSelect;	
		pAsharpResult->stEdgefilterParamSelect = pAsharpCtx->stAuto.stEdgefilterParamSelect;
		
	}else if(pAsharpCtx->eMode == ASHARP_OP_MODE_MANUAL){
		//TODO
		pAsharpResult->sharpEn = pAsharpCtx->stManual.sharpEn;
		pAsharpResult->stSharpParamSelect = pAsharpCtx->stManual.stSharpParamSelect;
		pAsharpResult->edgeFltEn = pAsharpCtx->stManual.edgeFltEn;
		pAsharpResult->stEdgefilterParamSelect = pAsharpCtx->stManual.stEdgefilterParamSelect;
	}

	rk_Sharp_fix_transfer(&pAsharpResult->stSharpParamSelect, &pAsharpResult->stSharpFix);
	edgefilter_fix_transfer(&pAsharpResult->stEdgefilterParamSelect, &pAsharpResult->stEdgefltFix);
	pAsharpResult->stSharpFix.stSharpFixV1.sharp_en = pAsharpResult->sharpEn ;
	pAsharpResult->stEdgefltFix.edgeflt_en = pAsharpResult->edgeFltEn;
	#if ASHARP_FIX_VALUE_PRINTF
	Asharp_fix_Printf(&pAsharpResult->stSharpFix.stSharpFixV1, &pAsharpResult->stEdgefltFix);
	#endif
	
	LOGI_ASHARP("%s(%d): exit!\n", __FUNCTION__, __LINE__);
	return ASHARP_RET_SUCCESS;
}

AsharpResult_t Asharp_fix_Printf(RKAsharp_Sharp_HW_Fix_t  * pSharpCfg, RKAsharp_Edgefilter_Fix_t *pEdgefltCfg)
{
	printf("%s:(%d) enter \n", __FUNCTION__, __LINE__);
	int i = 0;
	AsharpResult_t res = ASHARP_RET_SUCCESS;

	if(pSharpCfg == NULL){
		LOGE_ASHARP("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ASHARP_RET_INVALID_PARM;
	}

	if(pEdgefltCfg == NULL){
		LOGE_ASHARP("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ASHARP_RET_INVALID_PARM;
	}
	
	//0x0080
	printf("(0x0080) alpha_adp_en:%d yin_flt_en:%d edge_avg_en:%d\n",
			pEdgefltCfg->alpha_adp_en,
			pSharpCfg->yin_flt_en,
			pSharpCfg->edge_avg_en);
	
	
	//0x0084
	printf("(0x0084) hbf_ratio:%d ehf_th:%d pbf_ratio:%d\n",
			pSharpCfg->hbf_ratio,
			pSharpCfg->ehf_th,
			pSharpCfg->pbf_ratio);

	//0x0088
	printf("(0x0088) edge_thed:%d dir_min:%d smoth_th4:%d\n",
			pEdgefltCfg->edge_thed,
			pEdgefltCfg->dir_min,
			pEdgefltCfg->smoth_th4);

	//0x008c
	printf("(0x008c) l_alpha:%d g_alpha:%d \n",
			pEdgefltCfg->l_alpha,
			pEdgefltCfg->g_alpha);
	

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
			i, pEdgefltCfg->eg_coef[i]);
	}

	//0x00b8
	for(i = 0; i < 3; i++){
		printf("(0x00b8) eg_smoth[%d]:%d  \n",
			i, pEdgefltCfg->eg_smoth[i]);
	}


	//0x00bc - 0x00c0
	for(i = 0; i < 6; i++){
		printf("(0x00bc - 0x00c0) eg_gaus[%d]:%d  \n",
			i, pEdgefltCfg->eg_gaus[i]);
	}
	

	//0x00c4 - 0x00c8
	for(i = 0; i < 6; i++){
		printf("(0x00c4 - 0x00c8) dog_k[%d]:%d  \n",
			i, pEdgefltCfg->dog_k[i]);
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
			i, pEdgefltCfg->edge_lum_thed[i]);
	}

	//0x0110 - 0x0114
	for(i=0; i<8; i++){
		printf("(0x0110 - 0x0114) clamp_pos[%d]:%d  \n",
			i, pEdgefltCfg->clamp_pos[i]);
	}

	//0x0118 - 0x011c
	for(i=0; i<8; i++){
		printf("(0x0118 - 0x011c) clamp_neg[%d]:%d  \n",
			i, pEdgefltCfg->clamp_neg[i]);
	}

	//0x0120 - 0x0124
	for(i=0; i<8; i++){
		printf("(0x0120 - 0x0124) detail_alpha[%d]:%d  \n",
			i, pEdgefltCfg->detail_alpha[i]);
	}

	//0x0128
	printf("(0x0128) rfl_ratio:%d  rfh_ratio:%d\n",
			pSharpCfg->rfl_ratio, pSharpCfg->rfh_ratio);
	
	// mf/hf ratio

	//0x012C
	printf("(0x012C) m_ratio:%d  h_ratio:%d\n",
			pSharpCfg->m_ratio, pSharpCfg->h_ratio);
	
	printf("%s:(%d) exit \n", __FUNCTION__, __LINE__);

	return res;
}

RKAIQ_END_DECLARE

