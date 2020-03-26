/*
 * rk_aiq_algo_anr_itf.c
 *
 *  Copyright (c) 2019 Rockchip Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "rk_aiq_algo_types_int.h"
#include "anr/rk_aiq_algo_anr_itf.h"
#include "anr/rk_aiq_anr_algo.h"

RKAIQ_BEGIN_DECLARE

typedef struct _RkAiqAlgoContext {
    void* place_holder[0];
} RkAiqAlgoContext;

static RkAiqAlgoContext ctx;

static XCamReturn
create_context(RkAiqAlgoContext **context, const AlgoCtxInstanceCfg* cfg)
{
	 
	XCamReturn result = XCAM_RETURN_NO_ERROR;
	AlgoCtxInstanceCfgInt *cfgInt = (AlgoCtxInstanceCfgInt*)cfg;
	LOGI_ANR("%s: (enter)\n", __FUNCTION__ );
	 
#if 1
	ANRContext_t* pAnrCtx = NULL;
    ANRresult_t ret = ANRInit(&pAnrCtx, cfgInt->calib);
	if(ret != ANR_RET_SUCCESS){
		result = XCAM_RETURN_ERROR_FAILED;
		LOGE_ANR("%s: Initializaion ANR failed (%d)\n", __FUNCTION__, ret);
	}else{
		*context = (RkAiqAlgoContext *)(pAnrCtx);
	}
#endif

	LOGI_ANR("%s: (exit)\n", __FUNCTION__ );
	return result;
}

static XCamReturn
destroy_context(RkAiqAlgoContext *context)
{	
	XCamReturn result = XCAM_RETURN_NO_ERROR;
	
	LOGI_ANR("%s: (enter)\n", __FUNCTION__ );
	
#if 1
	ANRContext_t* pAnrCtx = (ANRContext_t*)context;
    ANRresult_t ret = ANRRelease(pAnrCtx);
	if(ret != ANR_RET_SUCCESS){
		result = XCAM_RETURN_ERROR_FAILED;
		LOGE_ANR("%s: release ANR failed (%d)\n", __FUNCTION__, ret);
	}
#endif

	LOGI_ANR("%s: (exit)\n", __FUNCTION__ );
    return result;
}

static XCamReturn
prepare(RkAiqAlgoCom* params)
{
	XCamReturn result = XCAM_RETURN_NO_ERROR;

	LOGI_ANR("%s: (enter)\n", __FUNCTION__ );
	
    ANRContext_t* pAnrCtx = (ANRContext_t *)params->ctx;
    RkAiqAlgoConfigAnrInt* pCfgParam = (RkAiqAlgoConfigAnrInt*)params;

	ANRresult_t ret = ANRPrepare(pAnrCtx, &pCfgParam->stANRConfig);
	if(ret != ANR_RET_SUCCESS){
		result = XCAM_RETURN_ERROR_FAILED;
		LOGE_ANR("%s: config ANR failed (%d)\n", __FUNCTION__, ret);
	}

	LOGI_ANR("%s: (exit)\n", __FUNCTION__ );
    return result;
}

static XCamReturn
pre_process(const RkAiqAlgoCom* inparams, RkAiqAlgoResCom* outparams)
{
	XCamReturn result = XCAM_RETURN_NO_ERROR;
	static int framecnt = 0;

	LOGI_ANR("%s: (enter)\n", __FUNCTION__ );
	
	//nothing todo now

	LOGI_ANR("%s: (exit)\n", __FUNCTION__ );
    return result;
}

static XCamReturn
processing(const RkAiqAlgoCom* inparams, RkAiqAlgoResCom* outparams)
{
	XCamReturn result = XCAM_RETURN_NO_ERROR;
	int iso;

	LOGI_ANR("%s: (enter)\n", __FUNCTION__ );
	
#if 1
	RkAiqAlgoProcAnrInt* pAnrProcParams = (RkAiqAlgoProcAnrInt*)inparams;
	RkAiqAlgoProcResAnrInt* pAnrProcResParams = (RkAiqAlgoProcResAnrInt*)outparams;
	ANRContext_t* pAnrCtx = (ANRContext_t *)inparams->ctx;

	LOGD_ANR("%s:%d init:%d \n", __FUNCTION__, __LINE__, inparams->u.proc.init);

	#if 0
	if(inparams->u.proc.init)
		iso = 50;
	else
		iso = pAnrProcParams->rk_com.u.proc.iso;
	#else
		iso = 50;
	#endif
	
	ANRresult_t ret = ANRProcess(pAnrCtx, iso);
	if(ret != ANR_RET_SUCCESS){
		result = XCAM_RETURN_ERROR_FAILED;
		LOGE_ANR("%s: processing ANR failed (%d)\n", __FUNCTION__, ret);
	}

	ANRGetProcResult(pAnrCtx, &pAnrProcResParams->stAnrProcResult);	
#endif

	LOGI_ANR("%s: (exit)\n", __FUNCTION__ );
    return XCAM_RETURN_NO_ERROR;
}

static XCamReturn
post_process(const RkAiqAlgoCom* inparams, RkAiqAlgoResCom* outparams)
{
	LOGI_ANR("%s: (enter)\n", __FUNCTION__ );
	
	//nothing todo now

	LOGI_ANR("%s: (exit)\n", __FUNCTION__ );
    return XCAM_RETURN_NO_ERROR;	
}

RkAiqAlgoDescription g_RkIspAlgoDescAnr = {
    .common = {
        .version = RKISP_ALGO_ANR_VERSION,
        .vendor  = RKISP_ALGO_ANR_VENDOR,
        .description = RKISP_ALGO_ANR_DESCRIPTION,
        .type    = RK_AIQ_ALGO_TYPE_ANR,
        .id      = 0,
        .create_context  = create_context,
        .destroy_context = destroy_context,
    },
    .prepare = prepare,
    .pre_process = pre_process,
    .processing = processing,
    .post_process = post_process,
};

RKAIQ_END_DECLARE
