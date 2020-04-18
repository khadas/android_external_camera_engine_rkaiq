/*
 * rk_aiq_algo_adhaz_itf.c
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
/* for rockchip v2.0.0*/

#include "rk_aiq_algo_types_int.h"


#include "rk_aiq_algo_adhaz_itf.h"
#include "RkAiqCalibDbTypes.h"
#include "adehaze/rk_aiq_adehaze_algo.h"
#include "RkAiqCalibDbTypes.h"
#include "xcam_log.h"

RKAIQ_BEGIN_DECLARE

static XCamReturn
create_context(RkAiqAlgoContext **context, const AlgoCtxInstanceCfg* cfg)
{
    AdehazeHandle_t*AdehazeHandle = NULL;
    XCamReturn ret;
    ret = AdehazeInitV200(&AdehazeHandle);
    *context = (RkAiqAlgoContext *)(AdehazeHandle);
    return XCAM_RETURN_NO_ERROR;

}

static XCamReturn
destroy_context(RkAiqAlgoContext *context)
{
    AdehazeHandle_t*AdehazeHandle = (AdehazeHandle_t*)context;
    XCamReturn ret;

    ret = AdehazeReleaseV200(AdehazeHandle);

    return XCAM_RETURN_NO_ERROR;
}

static XCamReturn
	prepare(RkAiqAlgoCom* params)
	{

		XCamReturn ret;
		int iso;

		RkAiqAlgoConfigAdhazInt* config = (RkAiqAlgoConfigAdhazInt*)params;
		AdehazeHandle_t * AdehazeHandle = (AdehazeHandle_t *)params->ctx;
		RKAiqAdhazHtmlConfig_t adhaz_html_para;
		CamCalibDbContext_t* calib = config->rk_com.u.prepare.calib;
		const CalibDb_Dehaze_t *calib_dehaze = &calib->dehaze;
		//TO DO
		iso = 50;
		ret = AdehazeConfigV200(calib_dehaze,AdehazeHandle, iso);
		
		return XCAM_RETURN_NO_ERROR;

	}

	static XCamReturn
	pre_process(const RkAiqAlgoCom* inparams, RkAiqAlgoResCom* outparams)
	{
		return XCAM_RETURN_NO_ERROR;
	}

	static XCamReturn
	processing(const RkAiqAlgoCom* inparams, RkAiqAlgoResCom* outparams)
	{

		XCamReturn ret;
		int iso;

		AdehazeHandle_t * AdehazeHandle = (AdehazeHandle_t *)inparams->ctx;
		RkAiqAlgoProcAdhazInt* procPara = (RkAiqAlgoProcAdhazInt*)inparams;
		RkAiqAlgoProcResAdhaz* procResPara = (RkAiqAlgoProcResAdhaz*)outparams;
		rk_aiq_dehaze_cfg_t* adhaz_config;
		const CalibDb_Dehaze_t *calib_dehaze = AdehazeHandle->calib_dehaz;
		adhaz_config = (rk_aiq_dehaze_cfg_t*)&procResPara->adhaz_config;




#if 1
			
			AdehazeExpInfo_t stExpInfo;
			memset(&stExpInfo, 0x00, sizeof(AdehazeExpInfo_t));
		
			LOGD_ADEHAZE("%s:%d init:%d hdr mode:%d  \n", 
				__FUNCTION__, __LINE__, 
				inparams->u.proc.init, 
				procPara->hdr_mode);
		
			stExpInfo.hdr_mode = 0; //pAnrProcParams->hdr_mode;
			for(int i=0; i<3; i++){
				stExpInfo.arIso[i] = 50;
				stExpInfo.arAGain[i] = 1.0;
				stExpInfo.arDGain[i] = 1.0;
				stExpInfo.arTime[i] = 0.01;
			}
		
			if(procPara->hdr_mode == RK_AIQ_WORKING_MODE_NORMAL){
					stExpInfo.hdr_mode = 0;
			}else if(procPara->hdr_mode == RK_AIQ_ISP_HDR_MODE_2_FRAME_HDR 
				|| procPara->hdr_mode == RK_AIQ_ISP_HDR_MODE_2_LINE_HDR ){
				stExpInfo.hdr_mode = 1; 
			}else if(procPara->hdr_mode == RK_AIQ_ISP_HDR_MODE_3_FRAME_HDR 
				|| procPara->hdr_mode == RK_AIQ_ISP_HDR_MODE_3_LINE_HDR ){
				stExpInfo.hdr_mode = 2;
			}
			
	#if 1	
			if(!inparams->u.proc.init){ 
				RkAiqAlgoPreResAeInt* pAEPreRes =
					(RkAiqAlgoPreResAeInt*)(procPara->rk_com.u.proc.pre_res_comb->ae_pre_res);
		
				if(pAEPreRes != NULL){
					if(procPara->hdr_mode == RK_AIQ_WORKING_MODE_NORMAL){
						stExpInfo.arAGain[0] = pAEPreRes->ae_pre_res_rk.LinearExp.exp_real_params.analog_gain;
						stExpInfo.arDGain[0] = pAEPreRes->ae_pre_res_rk.LinearExp.exp_real_params.digital_gain;
						stExpInfo.arTime[0] = pAEPreRes->ae_pre_res_rk.LinearExp.exp_real_params.integration_time;
						stExpInfo.arIso[0] = stExpInfo.arAGain[0]* 50;
					}else{		
						for(int i=0; i<3; i++){ 			
							stExpInfo.arAGain[i] = pAEPreRes->ae_pre_res_rk.HdrExp[i].exp_real_params.analog_gain;
							stExpInfo.arDGain[i] = pAEPreRes->ae_pre_res_rk.HdrExp[i].exp_real_params.digital_gain;
							stExpInfo.arTime[i] = pAEPreRes->ae_pre_res_rk.HdrExp[i].exp_real_params.integration_time;
							stExpInfo.arIso[i] = stExpInfo.arAGain[i] * 50;
		
							LOGD_ADEHAZE("%s:%d index:%d again:%f dgain:%f time:%f iso:%d hdr_mode:%d\n",
								__FUNCTION__, __LINE__,
								i,
								stExpInfo.arAGain[i],
								stExpInfo.arDGain[i],
								stExpInfo.arTime[i],
								stExpInfo.arIso[i],
								stExpInfo.hdr_mode);
						}	
					}
				}else{
					//LOGE_ADEHAZE("%s:%d pAEPreRes is NULL, so use default instead \n", __FUNCTION__, __LINE__);
				}	
			}
	#endif






	//if(inparams->u.proc.init == false) {
		iso = stExpInfo.arIso[stExpInfo.hdr_mode];
		//todo
			LOGD_ADEHAZE("============hdr_mode=%d,iso=%d========\n",stExpInfo.hdr_mode,iso);
		ret = AdehazeReConfigV200(AdehazeHandle,calib_dehaze,iso);
	//}
	
	memcpy(adhaz_config, &AdehazeHandle->adhaz_config, sizeof(rk_aiq_dehaze_cfg_t));
	return XCAM_RETURN_NO_ERROR;
	


	

#endif




	}

static XCamReturn
post_process(const RkAiqAlgoCom* inparams, RkAiqAlgoResCom* outparams)
{
    return XCAM_RETURN_NO_ERROR;
}

RkAiqAlgoDescription g_RkIspAlgoDescAdhaz = {
    .common = {
        .version = RKISP_ALGO_ADHAZ_VERSION,
        .vendor  = RKISP_ALGO_ADHAZ_VENDOR,
        .description = RKISP_ALGO_ADHAZ_DESCRIPTION,
        .type    = RK_AIQ_ALGO_TYPE_ADHAZ,
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
