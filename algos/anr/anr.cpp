
#include "anr.h"
#include "rk_aiq_algo_anr_itf.h"


//anr inint
ANRresult_t ANRInit(ANRContext_t **ppANRCtx)
{
	ANRContext_t * pANRCtx;

	LOGI_ANR("%s(%d): enter!\n", __FUNCTION__, __LINE__);
	
	pANRCtx = (ANRContext_t *)malloc(sizeof(ANRContext_t));
	if(pANRCtx == NULL){
		LOGE_ANR("%s(%d): malloc fail\n", __FUNCTION__, __LINE__);
		return ANR_RET_NULL_POINTER;
	}

	memset(pANRCtx, 0x00, sizeof(ANRContext_t));
	pANRCtx->eState = ANR_STATE_INITIALIZED;

	//ANRConfig(pANRCtx, pANRConfig);
	*ppANRCtx = pANRCtx;
	
	LOGI_ANR("%s(%d): exit!\n", __FUNCTION__, __LINE__);
	return ANR_RET_SUCCESS;
}

//anr release
ANRresult_t ANRRelease(ANRContext_t *pANRCtx)
{
	LOGI_ANR("%s(%d): enter!\n", __FUNCTION__, __LINE__);
	if(pANRCtx == NULL){
		LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ANR_RET_NULL_POINTER;
	}

	memset(pANRCtx, 0x00, sizeof(ANRContext_t));
	free(pANRCtx);

	LOGI_ANR("%s(%d): exit!\n", __FUNCTION__, __LINE__);
	return ANR_RET_SUCCESS;
}

//anr config
ANRresult_t ANRConfig(ANRContext_t *pANRCtx, ANRConfig_t* pANRConfig)
{
	LOGI_ANR("%s(%d): enter!\n", __FUNCTION__, __LINE__);
	
	if(pANRCtx == NULL){
		LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ANR_RET_INVALID_PARM;
	}

	if(pANRConfig == NULL){
		LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ANR_RET_INVALID_PARM;
	}

	if(pANRConfig->eMode == ANR_OP_MODE_AUTO){
		//xml 
		pANRCtx->stAuto.bayernrEn = pANRConfig->stBayernrCalib.enable;
		init_bayernr_params(&pANRCtx->stAuto.stBayernrParams, &pANRConfig->stBayernrCalib);

		pANRCtx->stAuto.uvnrEn = pANRConfig->stUvnrCalib.enable;
		init_uvnr_params(&pANRCtx->stAuto.stUvnrParams, &pANRConfig->stUvnrCalib);

		pANRCtx->stAuto.ynrEn = pANRConfig->stYnrCalib.enable;
		init_ynr_params(&pANRCtx->stAuto.stYnrParams, &pANRConfig->stYnrCalib);

		pANRCtx->stAuto.ynrEn = pANRConfig->stMfnrCalib.enable;
		init_mfnr_params(&pANRCtx->stAuto.stMfnrParams, &pANRConfig->stMfnrCalib);

		//html
		//memcpy(&pANRCtx->stAuto.stBayernrParams, &pANRConfig->stHtmlParams.stBayernrParams, sizeof(RKAnr_Bayernr_Params_t));
		//memcpy(&pANRCtx->stAuto.stMfnrParams, &pANRConfig->stHtmlParams.stMfnrParams, sizeof(RKAnr_Mfnr_Params_t));	
		//memcpy(&pANRCtx->stAuto.stYnrParams, &pANRConfig->stHtmlParams.stYnrParams, sizeof(RKAnr_Ynr_Params_t));
		//memcpy(&pANRCtx->stAuto.stUvnrParams, &pANRConfig->stHtmlParams.stUvnrParams, sizeof(RKAnr_Uvnr_Params_t));

		LOGD_ANR("%s(%d): bayernr %f %f %f %d %d %f", __FUNCTION__, __LINE__, 
			pANRCtx->stAuto.stBayernrParams.filtpar[0],
			pANRCtx->stAuto.stBayernrParams.filtpar[4],
			pANRCtx->stAuto.stBayernrParams.filtpar[8],
			pANRCtx->stAuto.stBayernrParams.peaknoisesigma,
			pANRCtx->stAuto.stBayernrParams.sw_bayernr_edge_filter_en,
			pANRCtx->stAuto.stBayernrParams.sw_bayernr_filter_strength[0]);
		
	}

	#if 0
	if(pANRConfig->eMode == ANR_OP_MODE_MANUAL){
		pANRCtx->stManual.u32NrLevel = pANRConfig->u32NrLevel;
		pANRCtx->stManual.u32SharpLevel = pANRConfig->u32SharpLevel;
	}
	#endif

	pANRCtx->eMode = pANRConfig->eMode;
	pANRCtx->eState = pANRConfig->eState;
	pANRCtx->refYuvBit = pANRConfig->refYuvBit;

	LOGI_ANR("%s(%d): exit!\n", __FUNCTION__, __LINE__);
	return ANR_RET_SUCCESS;
}

//anr reconfig
ANRresult_t ANRReConfig(ANRContext_t *pANRCtx, ANRConfig_t* pANRConfig)
{
	LOGI_ANR("%s(%d): enter!\n", __FUNCTION__, __LINE__);
	//need todo what?

	LOGI_ANR("%s(%d): exit!\n", __FUNCTION__, __LINE__);
	return ANR_RET_SUCCESS;
}

//anr preprocess
ANRresult_t ANRPreProcess(ANRContext_t *pANRCtx)
{
	LOGI_ANR("%s(%d): enter!\n", __FUNCTION__, __LINE__);
	//need todo what?

	LOGI_ANR("%s(%d): exit!\n", __FUNCTION__, __LINE__);
	return ANR_RET_SUCCESS;
}

//anr process
ANRresult_t ANRProcess(ANRContext_t *pANRCtx, int ISO)
{	
	LOGI_ANR("%s(%d): enter!\n", __FUNCTION__, __LINE__);

	if(pANRCtx == NULL){
		LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ANR_RET_INVALID_PARM;
	}
	
	if(pANRCtx->eMode == ANR_OP_MODE_AUTO){

		LOGD_ANR("%s(%d): ISO:%d refYuvBit:%d\n", __FUNCTION__, __LINE__,
			ISO, pANRCtx->refYuvBit);
		//select param
		select_bayernr_params_by_ISO(&pANRCtx->stAuto.stBayernrParams, &pANRCtx->stAuto.stBayernrParamSelect, ISO);
		select_mfnr_params_by_ISO(&pANRCtx->stAuto.stMfnrParams, &pANRCtx->stAuto.stMfnrParamSelect, ISO, pANRCtx->refYuvBit);
		select_ynr_params_by_ISO(&pANRCtx->stAuto.stYnrParams, &pANRCtx->stAuto.stYnrParamSelect, ISO, pANRCtx->refYuvBit);
		select_uvnr_params_by_ISO(&pANRCtx->stAuto.stUvnrParams, &pANRCtx->stAuto.stUvnrParamSelect, ISO);
		
	}else if(pANRCtx->eMode == ANR_OP_MODE_MANUAL){
		//TODO
	}
	
	LOGI_ANR("%s(%d): exit!\n", __FUNCTION__, __LINE__);
	return ANR_RET_SUCCESS;

}

//anr get result
ANRresult_t ANRGetProcResult(ANRContext_t *pANRCtx, ANRProcResult_t* pANRResult)
{
	LOGI_ANR("%s(%d): enter!\n", __FUNCTION__, __LINE__);

	if(pANRCtx == NULL){
		LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ANR_RET_INVALID_PARM;
	}

	if(pANRResult == NULL){
		LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ANR_RET_INVALID_PARM;
	}
	
	if(pANRCtx->eMode == ANR_OP_MODE_AUTO){	
		pANRResult->bayernrEn = pANRCtx->stAuto.bayernrEn;
		pANRResult->stBayernrParamSelect = pANRCtx->stAuto.stBayernrParamSelect;
		pANRResult->mfnrEn = pANRCtx->stAuto.mfnrEn;
		pANRResult->stMfnrParamSelect = pANRCtx->stAuto.stMfnrParamSelect;
		pANRResult->ynrEN = pANRCtx->stAuto.ynrEn;
		pANRResult->stYnrParamSelect = pANRCtx->stAuto.stYnrParamSelect;
		pANRResult->uvnrEn = pANRCtx->stAuto.uvnrEn;
		pANRResult->stUvnrParamSelect = pANRCtx->stAuto.stUvnrParamSelect;
		
	}else if(pANRCtx->eMode == ANR_OP_MODE_MANUAL){
		//TODO
		pANRResult->bayernrEn = pANRCtx->stManual.bayernrEn;
		pANRResult->stBayernrParamSelect = pANRCtx->stManual.stBayernrParamSelect;
		pANRResult->mfnrEn = pANRCtx->stManual.mfnrEn;
		pANRResult->stMfnrParamSelect = pANRCtx->stManual.stMfnrParamSelect;
		pANRResult->ynrEN = pANRCtx->stManual.ynrEn;
		pANRResult->stYnrParamSelect = pANRCtx->stManual.stYnrParamSelect;
		pANRResult->uvnrEn = pANRCtx->stManual.uvnrEn;
		pANRResult->stUvnrParamSelect = pANRCtx->stManual.stUvnrParamSelect;
	}
	
	LOGI_ANR("%s(%d): exit!\n", __FUNCTION__, __LINE__);
	return ANR_RET_SUCCESS;
}



