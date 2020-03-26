/*
 * rk_aiq_algo_ablc_itf.c
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
#include "ablc/rk_aiq_algo_ablc_itf.h"
#include "ablc/rk_aiq_ablc_algo.h"

RKAIQ_BEGIN_DECLARE

typedef struct _RkAiqAlgoContext {
    void* place_holder[0];
} RkAiqAlgoContext;

static RkAiqAlgoContext ctx;

static XCamReturn
create_context(RkAiqAlgoContext **context, const AlgoCtxInstanceCfg* cfg)
{XCamReturn result = XCAM_RETURN_NO_ERROR;

	LOGI_ABLC("%s: (enter)\n", __FUNCTION__ );
	AlgoCtxInstanceCfgInt *cfgInt = (AlgoCtxInstanceCfgInt*)cfg;
#if 1
	AblcContext_t* pAblcCtx = NULL;
    AblcResult_t ret = AblcInit(&pAblcCtx, cfgInt->calib);
	if(ret != ABLC_RET_SUCCESS){
		result = XCAM_RETURN_ERROR_FAILED;
		LOGE_ABLC("%s: Initializaion Ablc failed (%d)\n", __FUNCTION__, ret);
	}else{
		*context = (RkAiqAlgoContext *)(pAblcCtx);
	}
#endif

	LOGI_ABLC("%s: (exit)\n", __FUNCTION__ );
	return result;
    *context = &ctx;
    return XCAM_RETURN_NO_ERROR;
}

static XCamReturn
destroy_context(RkAiqAlgoContext *context)
{
	XCamReturn result = XCAM_RETURN_NO_ERROR;
	
	LOGI_ABLC("%s: (enter)\n", __FUNCTION__ );
	
#if 1
	AblcContext_t* pAblcCtx = (AblcContext_t*)context;
	AblcResult_t ret = AblcRelease(pAblcCtx);
	if(ret != ABLC_RET_SUCCESS){
		result = XCAM_RETURN_ERROR_FAILED;
		LOGE_ABLC("%s: release Ablc failed (%d)\n", __FUNCTION__, ret);
	}
#endif

	LOGI_ABLC("%s: (exit)\n", __FUNCTION__ );
	return result;

}

static XCamReturn
prepare(RkAiqAlgoCom* params)
{
 	XCamReturn result = XCAM_RETURN_NO_ERROR;

	LOGI_ABLC("%s: (enter)\n", __FUNCTION__ );
	
#if 1
    AblcContext_t* pAblcCtx = (AblcContext_t *)params->ctx;
    RkAiqAlgoConfigAblcInt* pCfgParam = (RkAiqAlgoConfigAblcInt*)params;
	AblcConfig_t *pAblc_config = &pCfgParam->ablc_config;
	
	AblcResult_t ret = AblcConfig(pAblcCtx, pAblc_config);
	if(ret != ABLC_RET_SUCCESS){
		result = XCAM_RETURN_ERROR_FAILED;
		LOGE_ABLC("%s: config Ablc failed (%d)\n", __FUNCTION__, ret);
	}
	
#endif

	LOGI_ABLC("%s: (exit)\n", __FUNCTION__ );
    return result;
}

static XCamReturn
pre_process(const RkAiqAlgoCom* inparams, RkAiqAlgoResCom* outparams)
{
    return XCAM_RETURN_NO_ERROR;
}

static XCamReturn
processing(const RkAiqAlgoCom* inparams, RkAiqAlgoResCom* outparams)
{
    XCamReturn result = XCAM_RETURN_NO_ERROR;
	int iso;

	LOGI_ABLC("%s: (enter)\n", __FUNCTION__ );
	
#if 1
	RkAiqAlgoProcAblcInt* pAblcProcParams = (RkAiqAlgoProcAblcInt*)inparams;
	RkAiqAlgoProcResAblcInt* pAblcProcResParams = (RkAiqAlgoProcResAblcInt*)outparams;
	AblcContext_t* pAblcCtx = (AblcContext_t *)inparams->ctx;

	LOGI_ABLC("%s:%d init:%d \n", __FUNCTION__, __LINE__, inparams->u.proc.init);

	#if 0
	if(inparams->u.proc.init)
		iso = 50;
	else
		iso = pAblcProcParams->rk_com.u.proc.iso;
	#else
		iso = 50;
	#endif
	
	AblcResult_t ret = AblcProcess(pAblcCtx, iso);
	if(ret != ABLC_RET_SUCCESS){
		result = XCAM_RETURN_ERROR_FAILED;
		LOGE_ABLC("%s: processing ANR failed (%d)\n", __FUNCTION__, ret);
	}

	AblcGetProcResult(pAblcCtx, &pAblcProcResParams->ablc_proc_res);	
#endif

	LOGI_ABLC("%s: (exit)\n", __FUNCTION__ );
    return XCAM_RETURN_NO_ERROR;
}

static XCamReturn
post_process(const RkAiqAlgoCom* inparams, RkAiqAlgoResCom* outparams)
{
    return XCAM_RETURN_NO_ERROR;
}

RkAiqAlgoDescription g_RkIspAlgoDescAblc = {
    .common = {
        .version = RKISP_ALGO_ABLC_VERSION,
        .vendor  = RKISP_ALGO_ABLC_VENDOR,
        .description = RKISP_ALGO_ABLC_DESCRIPTION,
        .type    = RK_AIQ_ALGO_TYPE_ABLC,
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
