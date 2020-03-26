
#include "rk_aiq_anr_algo.h"
#include "rk_aiq_algo_anr_itf.h"

RKAIQ_BEGIN_DECLARE

//anr inint
ANRresult_t ANRInit(ANRContext_t **ppANRCtx, CamCalibDbContext_t *pCalibDb)
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
	*ppANRCtx = pANRCtx;
	
	pANRCtx->refYuvBit = 8;
	pANRCtx->eMode = ANR_OP_MODE_AUTO;

	#if ANR_USE_XML_FILE
	//read v1 params from xml
	pANRCtx->stBayernrCalib = pCalibDb->bayerNr;
	pANRCtx->stUvnrCalib = pCalibDb->uvnr;
	pANRCtx->stYnrCalib = pCalibDb->ynr;
	pANRCtx->stMfnrCalib = pCalibDb->mfnr;
	#endif

	#if RK_SIMULATOR_HW
	//just for v2 params from html
    FILE *fp2 = fopen("rkaiq_anr_html_params.bin", "rb");
    if(fp2 != NULL) {
        memset(&pANRCtx->stAuto.stBayernrParams, 0, sizeof(RKAnr_Bayernr_Params_t));
        fread(&pANRCtx->stAuto.stBayernrParams, 1, sizeof(RKAnr_Bayernr_Params_t), fp2);
        memset(&pANRCtx->stAuto.stMfnrParams, 0, sizeof(RKAnr_Mfnr_Params_t));
        fread(&pANRCtx->stAuto.stMfnrParams, 1, sizeof(RKAnr_Mfnr_Params_t), fp2);
        memset(&pANRCtx->stAuto.stUvnrParams, 0, sizeof(RKAnr_Uvnr_Params_t));
        fread(&pANRCtx->stAuto.stUvnrParams, 1, sizeof(RKAnr_Uvnr_Params_t), fp2);
        memset(&pANRCtx->stAuto.stYnrParams, 0, sizeof(RKAnr_Ynr_Params_t));
        fread(&pANRCtx->stAuto.stYnrParams, 1, sizeof(RKAnr_Ynr_Params_t), fp2);
        LOGD_ANR("oyyf: %s:(%d) read anr param html file sucess! \n", __FUNCTION__, __LINE__);
    } else {
        LOGE_ANR("oyyf: %s:(%d) read anr param html file failed! \n", __FUNCTION__, __LINE__);
        return ANR_RET_FAILURE;
    }   
	#endif

	#if ANR_USE_XML_FILE
	//cover v1 params
	pANRCtx->stAuto.bayernrEn = pANRCtx->stBayernrCalib.enable;
	init_bayernr_params(&pANRCtx->stAuto.stBayernrParams, &pANRCtx->stBayernrCalib);
	pANRCtx->stAuto.uvnrEn = pANRCtx->stUvnrCalib.enable;
	init_uvnr_params(&pANRCtx->stAuto.stUvnrParams, &pANRCtx->stUvnrCalib);
	pANRCtx->stAuto.ynrEn = pANRCtx->stYnrCalib.enable;
	init_ynr_params(&pANRCtx->stAuto.stYnrParams, &pANRCtx->stYnrCalib);
	pANRCtx->stAuto.ynrEn = pANRCtx->stMfnrCalib.enable;
	init_mfnr_params(&pANRCtx->stAuto.stMfnrParams, &pANRCtx->stMfnrCalib);
	#endif

	LOGD_ANR("%s(%d): bayernr %f %f %f %d %d %f", __FUNCTION__, __LINE__, 
			pANRCtx->stAuto.stBayernrParams.filtpar[0],
			pANRCtx->stAuto.stBayernrParams.filtpar[4],
			pANRCtx->stAuto.stBayernrParams.filtpar[8],
			pANRCtx->stAuto.stBayernrParams.peaknoisesigma,
			pANRCtx->stAuto.stBayernrParams.sw_bayernr_edge_filter_en,
			pANRCtx->stAuto.stBayernrParams.sw_bayernr_filter_strength[0]);
		
	
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
ANRresult_t ANRPrepare(ANRContext_t *pANRCtx, ANRConfig_t* pANRConfig)
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

	//pANRCtx->eMode = pANRConfig->eMode;
	//pANRCtx->eState = pANRConfig->eState;
	//pANRCtx->refYuvBit = pANRConfig->refYuvBit;

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

		pANRCtx->ISO = ISO;
		
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

		pANRResult->stBayernrParamSelect = pANRCtx->stAuto.stBayernrParamSelect;		
		pANRResult->stMfnrParamSelect = pANRCtx->stAuto.stMfnrParamSelect;		
		pANRResult->stYnrParamSelect = pANRCtx->stAuto.stYnrParamSelect;		
		pANRResult->stUvnrParamSelect = pANRCtx->stAuto.stUvnrParamSelect;

		pANRResult->bayernrEn = pANRCtx->stAuto.bayernrEn;
		pANRResult->mfnrEn = pANRCtx->stAuto.mfnrEn;
		pANRResult->ynrEN = pANRCtx->stAuto.ynrEn;
		pANRResult->uvnrEn = pANRCtx->stAuto.uvnrEn;
		
		
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

	//transfer to reg value
	bayernr_fix_tranfer(&pANRResult->stBayernrParamSelect, &pANRResult->stBayernrFix);
	mfnr_fix_transfer(&pANRResult->stMfnrParamSelect, &pANRResult->stMfnrFix);
	ynr_fix_transfer(&pANRResult->stYnrParamSelect, &pANRResult->stYnrFix);
	uvnr_fix_transfer(&pANRResult->stUvnrParamSelect, &pANRResult->stUvnrFix);
	pANRResult->stBayernrFix.rawnr_en = pANRResult->bayernrEn;
	pANRResult->stMfnrFix.tnr_en = pANRResult->mfnrEn;
	pANRResult->stYnrFix.ynr_en = pANRResult->ynrEN;
	pANRResult->stUvnrFix.uvnr_en = pANRResult->uvnrEn;
	
	LOGI_ANR("%s(%d): exit!\n", __FUNCTION__, __LINE__);
	return ANR_RET_SUCCESS;
}

RKAIQ_END_DECLARE


