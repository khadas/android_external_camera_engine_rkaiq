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
#include "asharp/rk_aiq_algo_asharp_itf.h"
#include "asharp/asharp.h"

RKAIQ_BEGIN_DECLARE

typedef struct _RkAiqAlgoContext {
    void* place_holder[0];
} RkAiqAlgoContext;

static RkAiqAlgoContext ctx;

static XCamReturn
create_context(RkAiqAlgoContext **context, const AlgoCtxInstanceCfg* cfg)
{
	 
	XCamReturn result = XCAM_RETURN_NO_ERROR;

	LOGI_ASHARP("%s: (enter)\n", __FUNCTION__ );
	 
#if 1
	AsharpContext_t* pAsharpCtx = NULL;
    AsharpResult_t ret = AsharpInit(&pAsharpCtx);
	if(ret != ASHARP_RET_SUCCESS){
		result = XCAM_RETURN_ERROR_FAILED;
		LOGE_ASHARP("%s: Initializaion Asharp failed (%d)\n", __FUNCTION__, ret);
	}else{
		*context = (RkAiqAlgoContext *)(pAsharpCtx);
	}
#endif

	LOGI_ASHARP("%s: (exit)\n", __FUNCTION__ );
	return result;
}

static XCamReturn
destroy_context(RkAiqAlgoContext *context)
{	
	XCamReturn result = XCAM_RETURN_NO_ERROR;
	
	LOGI_ASHARP("%s: (enter)\n", __FUNCTION__ );
	
#if 1
	AsharpContext_t* pAsharpCtx = (AsharpContext_t*)context;
    AsharpResult_t ret = AsharpRelease(pAsharpCtx);
	if(ret != ASHARP_RET_SUCCESS){
		result = XCAM_RETURN_ERROR_FAILED;
		LOGE_ASHARP("%s: release Asharp failed (%d)\n", __FUNCTION__, ret);
	}
#endif

	LOGI_ASHARP("%s: (exit)\n", __FUNCTION__ );
    return result;
}

static XCamReturn
prepare(RkAiqAlgoCom* params)
{
	XCamReturn result = XCAM_RETURN_NO_ERROR;

	LOGI_ASHARP("%s: (enter)\n", __FUNCTION__ );
	
#if 1
    AsharpContext_t* pAsharpCtx = (AsharpContext_t *)params->ctx;
    RkAiqAlgoConfigAsharpInt* pCfgParam = (RkAiqAlgoConfigAsharpInt*)params;
	AsharpConfig_t* pAsharpConfig = &pCfgParam->stAsharpConfig;

	pAsharpConfig->stSharpCalib = pCfgParam->rk_com.u.prepare.calib->sharp;
	pAsharpConfig->stEdgeFltCalib = pCfgParam->rk_com.u.prepare.calib->edgeFilter;
	
	AsharpResult_t ret = AsharpConfig(pAsharpCtx, &pCfgParam->stAsharpConfig);
	if(ret != ASHARP_RET_SUCCESS){
		result = XCAM_RETURN_ERROR_FAILED;
		LOGE_ASHARP("%s: config Asharp failed (%d)\n", __FUNCTION__, ret);
	}
	
#endif

	LOGI_ASHARP("%s: (exit)\n", __FUNCTION__ );
    return result;
}

static XCamReturn
pre_process(const RkAiqAlgoCom* inparams, RkAiqAlgoResCom* outparams)
{
	XCamReturn result = XCAM_RETURN_NO_ERROR;
	static int framecnt = 0;

	LOGI_ASHARP("%s: (enter)\n", __FUNCTION__ );
	
	//nothing todo now

	LOGI_ASHARP("%s: (exit)\n", __FUNCTION__ );
    return result;
}

static XCamReturn
processing(const RkAiqAlgoCom* inparams, RkAiqAlgoResCom* outparams)
{
	XCamReturn result = XCAM_RETURN_NO_ERROR;
	int iso;

	LOGI_ASHARP("%s: (enter)\n", __FUNCTION__ );
	
#if 1
	RkAiqAlgoProcAsharpInt* pAsharpProcParams = (RkAiqAlgoProcAsharpInt*)inparams;
	RkAiqAlgoProcResAsharpInt* pAsharpProcResParams = (RkAiqAlgoProcResAsharpInt*)outparams;
	AsharpContext_t* pAsharpCtx = (AsharpContext_t *)inparams->ctx;

	LOGD_ASHARP("%s:%d init:%d \n", __FUNCTION__, __LINE__, inparams->u.proc.init);

	if(inparams->u.proc.init)
		iso = 50;
	else
		iso = pAsharpProcParams->rk_com.u.proc.iso;
	
	AsharpResult_t ret = AsharpProcess(pAsharpCtx, iso);
	if(ret != ASHARP_RET_SUCCESS){
		result = XCAM_RETURN_ERROR_FAILED;
		LOGE_ASHARP("%s: processing Asharp failed (%d)\n", __FUNCTION__, ret);
	}

	AsharpGetProcResult(pAsharpCtx, &pAsharpProcResParams->stAsharpProcResult);	
#endif

	LOGI_ASHARP("%s: (exit)\n", __FUNCTION__ );
    return XCAM_RETURN_NO_ERROR;
}

static XCamReturn
post_process(const RkAiqAlgoCom* inparams, RkAiqAlgoResCom* outparams)
{
	LOGI_ASHARP("%s: (enter)\n", __FUNCTION__ );
	
	//nothing todo now

	LOGI_ASHARP("%s: (exit)\n", __FUNCTION__ );
    return XCAM_RETURN_NO_ERROR;	
}

RkAiqAlgoDescription g_RkIspAlgoDescAsharp = {
    .common = {
        .version = RKISP_ALGO_ASHARP_VERSION,
        .vendor  = RKISP_ALGO_ASHARP_VENDOR,
        .description = RKISP_ALGO_ASHARP_DESCRIPTION,
        .type    = RK_AIQ_ALGO_TYPE_ASHARP,
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
