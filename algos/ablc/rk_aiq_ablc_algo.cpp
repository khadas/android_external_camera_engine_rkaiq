#include "rk_aiq_algo_ablc_itf.h"
#include "rk_aiq_ablc_algo.h"


AblcResult_t Ablc_html_params_init(AblcParams_t *pParams)
{
	AblcResult_t ret = ABLC_RET_SUCCESS;

	LOGI_ABLC("%s(%d): enter!\n", __FUNCTION__, __LINE__);

	if(pParams == NULL){
		ret = ABLC_RET_NULL_POINTER;
		LOGE_ADPCC("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ret;
	}

	int isoBase = 50;

	pParams->enable = 1;
	for(int i=0; i<BLC_MAX_ISO_LEVEL; i++){
		pParams->iso[i] = isoBase * (1 << i);
		pParams->blc_r[i] = 200;
		pParams->blc_gr[i] = 200;
		pParams->blc_gb[i] = 200;
		pParams->blc_b[i] = 200;
	}

	LOGI_ABLC("%s(%d): exit!\n", __FUNCTION__, __LINE__);
    return ret;
}

AblcResult_t Ablc_xml_params_init(AblcParams_t *pParams, CalibDb_Blc_t* pBlcCalib)
{
	AblcResult_t ret = ABLC_RET_SUCCESS;

	LOGI_ABLC("%s(%d): enter!\n", __FUNCTION__, __LINE__);

	if(pParams == NULL){
		ret = ABLC_RET_NULL_POINTER;
		LOGE_ABLC("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ret;
	}

	if(pBlcCalib == NULL){
		ret = ABLC_RET_NULL_POINTER;
		LOGE_ABLC("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ret;
	}
	
	int isoBase = 50;

	pParams->enable = pBlcCalib->enable;
	for(int i=0; i<BLC_MAX_ISO_LEVEL; i++){
		pParams->iso[i] = isoBase * (1 << i);
		pParams->blc_r[i] = (short int)(pBlcCalib->level[0]);
		pParams->blc_gr[i] = (short int)(pBlcCalib->level[1]);
		pParams->blc_gb[i] = (short int)(pBlcCalib->level[2]);
		pParams->blc_b[i] = (short int)(pBlcCalib->level[3]);
	}

	LOGI_ABLC("%s(%d): exit!\n", __FUNCTION__, __LINE__);
    return ret;
}


AblcResult_t Ablc_Select_Params_By_ISO(AblcParams_t *pParams, AblcParamsSelect_t *pSelect, int isoValue)
{
	int isoLowlevel = 0;
	int isoHighlevel = 0;
	int lowIso = 0;
	int highIso = 0;
	float ratio =0.0f;

	LOGI_ABLC("%s(%d): enter!\n", __FUNCTION__, __LINE__);

	if(pParams == NULL){
		LOGE_ABLC("%s(%d): NULL pointer\n", __FUNCTION__, __LINE__);
		return ABLC_RET_NULL_POINTER;
	}

	if(pSelect == NULL){
		LOGE_ABLC("%s(%d): NULL pointer\n", __FUNCTION__, __LINE__);
		return ABLC_RET_NULL_POINTER;
	}

	for(int i=0; i<BLC_MAX_ISO_LEVEL-1; i++)
	{
		if(isoValue >= pParams->iso[i] && isoValue <= pParams->iso[i+1])
		{
			isoLowlevel = i;
			isoHighlevel = i+1;
			//ratio = (isoValue -lowIso ) / (float)(highIso - lowIso);
			ratio = 0;
			break;
		}
	}

	if(isoValue < pParams->iso[0])
	{
		isoLowlevel = 0;
		isoHighlevel = 1;
		ratio = 0;
	}
	
	if(isoValue > pParams->iso[BLC_MAX_ISO_LEVEL -1])
	{
		isoLowlevel = BLC_MAX_ISO_LEVEL - 1;
		isoHighlevel = BLC_MAX_ISO_LEVEL - 1;
		ratio = 0;
	}

	pSelect->enable = pParams->enable;
		
	pSelect->blc_r = ratio *(pParams->blc_r[isoHighlevel] - pParams->blc_r[isoLowlevel]) 
					+ pParams->blc_r[isoLowlevel];
	pSelect->blc_gr = ratio *(pParams->blc_gr[isoHighlevel] - pParams->blc_gr[isoLowlevel]) 
					+ pParams->blc_gr[isoLowlevel];
	pSelect->blc_gb = ratio *(pParams->blc_gb[isoHighlevel] - pParams->blc_gb[isoLowlevel]) 
					+ pParams->blc_gb[isoLowlevel];
	pSelect->blc_b = ratio *(pParams->blc_b[isoHighlevel] - pParams->blc_b[isoLowlevel]) 
					+ pParams->blc_b[isoLowlevel];

	LOGD_ABLC("%s:(%d) Ablc iso:%d rggb: %d %d %d %d \n", 
		__FUNCTION__, __LINE__, isoValue,
		pSelect->blc_r, pSelect->blc_gr,
		pSelect->blc_gb, pSelect->blc_b);

	LOGI_ABLC("%s(%d): exit!\n", __FUNCTION__, __LINE__);
	return ABLC_RET_SUCCESS;
}

AblcResult_t AblcInit(AblcContext_t **ppAblcCtx, CamCalibDbContext_t *pCalibDb)
{
	AblcContext_t * pAblcCtx;

	LOGI_ABLC("%s(%d): enter!\n", __FUNCTION__, __LINE__);
	
	pAblcCtx = (AblcContext_t *)malloc(sizeof(AblcContext_t));
	if(pAblcCtx == NULL){
		LOGE_ABLC("%s(%d): NULL pointer\n", __FUNCTION__, __LINE__);
		return ABLC_RET_NULL_POINTER;
	}

	memset(pAblcCtx, 0x00, sizeof(AblcContext_t));
	pAblcCtx->eState = ABLC_STATE_INITIALIZED;

	*ppAblcCtx = pAblcCtx;

	//init params for algo work
	pAblcCtx->eMode = ABLC_OP_MODE_AUTO;
	
	#if 1
	//xml param
	pAblcCtx->stBlcCalib = pCalibDb->blc;
	Ablc_xml_params_init(&pAblcCtx->stAuto.stParams, &pAblcCtx->stBlcCalib);
	#else
	//static init params
	Ablc_html_params_init(&pAblcCtx->stAuto.stParams);
	#endif
	
	LOGE_ABLC("%s(%d): Ablc en:%d blc:%d %d %d %d \n", 
		__FUNCTION__, __LINE__, 
		pAblcCtx->stAuto.stParams.enable,
		pAblcCtx->stAuto.stParams.blc_r[0],
		pAblcCtx->stAuto.stParams.blc_gr[0],
		pAblcCtx->stAuto.stParams.blc_gb[0],
		pAblcCtx->stAuto.stParams.blc_gb[0]);

	
	LOGI_ABLC("%s(%d): exit!\n", __FUNCTION__, __LINE__);
	return ABLC_RET_SUCCESS;
}

AblcResult_t AblcRelease(AblcContext_t *pAblcCtx)
{
	LOGI_ABLC("%s(%d): enter!\n", __FUNCTION__, __LINE__);
	if(pAblcCtx == NULL){
		LOGE_ABLC("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ABLC_RET_NULL_POINTER;
	}

	memset(pAblcCtx, 0x00, sizeof(AblcContext_t));
	free(pAblcCtx);

	LOGI_ABLC("%s(%d): exit!\n", __FUNCTION__, __LINE__);
	return ABLC_RET_SUCCESS;

}

AblcResult_t AblcConfig(AblcContext_t *pAblcCtx, AblcConfig_t* pAblcConfig)
{
	LOGI_ABLC("%s(%d): enter!\n", __FUNCTION__, __LINE__);
		
	if(pAblcCtx == NULL){
		LOGE_ABLC("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ABLC_RET_NULL_POINTER;
	}

	if(pAblcConfig == NULL){
		LOGE_ABLC("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ABLC_RET_NULL_POINTER;
	}

	//pAblcCtx->eMode = pAblcConfig->eMode;
	//pAblcCtx->eState = pAblcConfig->eState;
		

	LOGI_ABLC("%s(%d): exit!\n", __FUNCTION__, __LINE__);
	return ABLC_RET_SUCCESS;

}

AblcResult_t AblcReConfig(AblcContext_t *pAblcCtx, AblcConfig_t* pAblcConfig)
{
	LOGI_ABLC("%s(%d): enter!\n", __FUNCTION__, __LINE__);
		
	//TODO

	LOGI_ABLC("%s(%d): exit!\n", __FUNCTION__, __LINE__);
	return ABLC_RET_SUCCESS;
}

AblcResult_t AblcPreProcess(AblcContext_t *pAblcCtx)
{
	LOGI_ABLC("%s(%d): enter!\n", __FUNCTION__, __LINE__);
	//need todo what?

	LOGI_ABLC("%s(%d): exit!\n", __FUNCTION__, __LINE__);
	return ABLC_RET_SUCCESS;

}

AblcResult_t AblcProcess(AblcContext_t *pAblcCtx, int isoValue)
{
	LOGI_ABLC("%s(%d): enter!\n", __FUNCTION__, __LINE__);
	AblcResult_t ret = ABLC_RET_SUCCESS;

	if(pAblcCtx == NULL){
		LOGE_ABLC("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ABLC_RET_NULL_POINTER;
	}

	if(isoValue < 0){
		LOGE_ABLC("%s(%d): invalid param\n", __FUNCTION__, __LINE__);
		return ABLC_RET_INVALID_PARM;
	}

	if(pAblcCtx->eMode == ABLC_OP_MODE_AUTO){
		ret = Ablc_Select_Params_By_ISO(&pAblcCtx->stAuto.stParams, &pAblcCtx->stAuto.stSelect, isoValue);
	}
	
	LOGI_ABLC("%s(%d): exit!\n", __FUNCTION__, __LINE__);
	return ABLC_RET_SUCCESS;
}

AblcResult_t AblcGetProcResult(AblcContext_t *pAblcCtx, AblcProcResult_t* pAblcResult)
{
	LOGI_ABLC("%s(%d): enter!\n", __FUNCTION__, __LINE__);

	if(pAblcCtx == NULL){
		LOGE_ABLC("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ABLC_RET_NULL_POINTER;
	}

	if(pAblcResult == NULL){
		LOGE_ABLC("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ABLC_RET_NULL_POINTER;
	}

	if(pAblcCtx->eMode == ABLC_OP_MODE_AUTO){
		pAblcResult->stResult.enable = pAblcCtx->stAuto.stSelect.enable;
		pAblcResult->stResult.blc_r = pAblcCtx->stAuto.stSelect.blc_r;
		pAblcResult->stResult.blc_gr = pAblcCtx->stAuto.stSelect.blc_gr;
		pAblcResult->stResult.blc_gb = pAblcCtx->stAuto.stSelect.blc_gb;
		pAblcResult->stResult.blc_b = pAblcCtx->stAuto.stSelect.blc_b;
		
	}else if(pAblcCtx->eMode == ABLC_OP_MODE_MANUAL){
		pAblcResult->stResult.enable = pAblcCtx->stManual.stSelect.enable;
		pAblcResult->stResult.blc_r = pAblcCtx->stManual.stSelect.blc_r;
		pAblcResult->stResult.blc_gr = pAblcCtx->stManual.stSelect.blc_gr;
		pAblcResult->stResult.blc_gb = pAblcCtx->stManual.stSelect.blc_gb;
		pAblcResult->stResult.blc_b = pAblcCtx->stManual.stSelect.blc_b;
	}

	LOGD_ABLC("%s:(%d) Ablc rggb: %d %d %d %d \n", 
		__FUNCTION__, __LINE__, 
		pAblcResult->stResult.blc_r, pAblcResult->stResult.blc_gr,
		pAblcResult->stResult.blc_gb, pAblcResult->stResult.blc_gr);

	LOGI_ABLC("%s(%d): exit!\n", __FUNCTION__, __LINE__);
	return ABLC_RET_SUCCESS;
}

