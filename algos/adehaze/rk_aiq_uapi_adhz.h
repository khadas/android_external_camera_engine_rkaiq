/*
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

#ifndef __RK_AIQ_UAPI_ADHZ_H__
#define __RK_AIQ_UAPI_ADHZ_H__

#include "base/xcam_common.h"
#include "rk_aiq.h"

typedef enum opMode_s{
	opMode_OFF = 0,
	opMode_Auto,
	opMode_MANU
}opMode_t;







/*
typedef struct Dehaze_MODE_s{
	Dehaze_MODE_OFF,
	Dehaze_MODE_Auto,
	Dehaze_MODE_MANU,
}Dehaze_MODE_t;

typedef struct  HistEqual_MODE_s{
	HistEqual_MODE_OFF
	HistEqual_MODE_Auto,
	HistEqual_MODE_Manu,
	
}HistEqual_MODE_t;

typedef struct  Enhance_MODE_s{
  Enhance_MODE_OFF
	Enhance_MODE_Auto,//
	Enhance_MODE_Manu,//
}Enhance_MODE_t;*/






typedef struct adehazeAttr_s{
	int sw_dhaz_en = 1;
	int cfg_alpha = 0;
	int enchance_en = 0;
		
}adehazeAttr_t;


typedef struct mdehazeAttr_s{
	int sw_dhaz_en = 1;
	int cfg_alpha = 1;
	int enchance_en = 0;
}mdehazeAttr_t;

typedef struct dehazeAttr_s{
	bool bEnable;
	opMode_t opMode;
	adehazeAttr_t  stAuto;
	mdehazeAttr_t stManual;
}dehazeAttr_t;



typedef struct ahistEqualAttr_s{
     int sw_dhaze_hist_en;
		
}ahistEqualAttr_t;


typedef struct mhistEqualAttr_s{

}mhistEqualAttr_t;

typedef struct histEqualAttr_s{
	bool bEnable;
	opMode_t opMode;
	ahistEqualAttr_t  stAuto;
	mhistEqualAttr_t stManual;
}histEqualAttr_t;


typedef struct aenhanceAttr_s{

		
}aenhanceAttr_t;


typedef struct menhanceAttr_s{

}menhanceAttr_t;

typedef struct enhanceAttr_s{
	bool bEnable;
	opMode_t opMode;
	aenhanceAttr_t  stAuto;
	menhanceAttr_t stManual;
}enhanceAttr_t;





typedef  int  rk_aiq_sys_ctx_t ;

XCamReturn rk_aiq_uapi_setDhzMode(const rk_aiq_sys_ctx_t* ctx, opMode_t mode);

XCamReturn rk_aiq_uapi_getDhzMode(const rk_aiq_sys_ctx_t* ctx, opMode_t *mode);

XCamReturn rk_aiq_uapi_setDehazeManu(const rk_aiq_sys_ctx_t* ctx, float wt, int air, float tmax, float gratio);

XCamReturn rk_aiq_uapi_getDehazeManu(const rk_aiq_sys_ctx_t* ctx, float *wt, int *air, float *tmax, float *gratio);

XCamReturn rk_aiq_uapi_setIircontral(const rk_aiq_sys_ctx_t* ctx, int fnum, int sigma, float wt, float air, float tmax);//(fnum,sigma,wt,air,tmax) 

XCamReturn rk_aiq_uapi_getIircontral(const rk_aiq_sys_ctx_t* ctx, int *fnum, int *sigma, float *wt, float *air, float *tmax);

XCamReturn rk_aiq_uapi_setHisEqMode(const rk_aiq_sys_ctx_t* ctx, opMode_t mode);

XCamReturn rk_aiq_uapi_getHisEqMode(const rk_aiq_sys_ctx_t* ctx, opMode_t *mode);

XCamReturn rk_aiq_uapi_setHisEqualManu(const rk_aiq_sys_ctx_t* ctx, float strength);//

XCamReturn rk_aiq_uapi_getHisEqualManu(const rk_aiq_sys_ctx_t* ctx, float *strength);

XCamReturn rk_aiq_uapi_enableEnhanceMode(const rk_aiq_sys_ctx_t* ctx, opMode_t mode);//

XCamReturn rk_aiq_uapi_disableEnhanceMode(const rk_aiq_sys_ctx_t* ctx, opMode_t* mode);

XCamReturn rk_aiq_uapi_setEnhance(const rk_aiq_sys_ctx_t* ctx, float strength);//

XCamReturn rk_aiq_uapi_getEnhance(const rk_aiq_sys_ctx_t* ctx, float *strength);

XCamReturn rk_aiq_uapi_getDehazeAdpOut(const rk_aiq_sys_ctx_t* ctx, float *wt, float *air, float *tmax, float *gratio);//(wt,air_base,tmax,gratio)//dehaze


#endif

