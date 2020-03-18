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

#ifndef __RK_AIQ_UAPI_AWB_H__
#define __RK_AIQ_UAPI_AWB_H__

#include "rk_aiq_awb_api_hw_v200.h"
#include "rk_aiq_awb_api_hw_v201.h"
#include "rk_aiq_awb_api_algo.h"
#include "common.h"
#include "base/xcam_common.h"
#include "rk_aiq.h"


typedef enum wbCTScene_e {
    WBCT_INDOOR = 0,
    WBCT_OUTDOOR = 1,
    WBCT_FLUORESCENT = 2,
    WBCT_SODIUMWLGHT = 3,
    WBCT_WARMLIGHT = 4,
    WBCT_DAYLIGHT = 5,
    WBCT_INVAL
} wbCTScene_t;

typedef struct wbGain_s{
    float r_gain;
    float gr_gain;
    float gb_gain;
    float b_gain;
} wbGain_t;

typedef enum awbRange_e {
    AWB_RANGE_0 = 0,
    AWB_RANGE_1 = 1,
    AWB_RANGE_INVAL,
} awbRange_t;

typedef struct mwbAttr_s {
    unsigned int active;
    union MWBPara_u {
        wbGain_t gain;
        float CCT;  //used in AWB_MODE_MANUAL_CT
        float CCRI;  //used in AWB_MODE_MANUAL_CT
        int illIndex;  //light source index
    } para;
} mwbAttr_t;

typedef enum awbAlgType_s {
    AWB_ALG_TYPE_INVAILD =0,
    AWB_ALG_TYPE_GLOABL =1,
    AWB_ALG_TYPE_BLOCK =2,
} AWBAlgType_t;


typedef struct awbAttr_s {
    bool En;
    AWBAlgType_t AlgType;
    unsigned int RunInterval; 
    bool ColorBalanceEn;
    bool SceneAdjEn;
} awbAttr_t;

typedef struct wbAttr_s{
    opMode_t mode;
    MWBAttr_t stManual;
    AWBAttr_t stAuto;
} wbAttr_t;

typedef struct wbInfo_s{
    WBGain_t gain;
    AWB_CCT_INFO_T ct;
    AWB_CCT_INFO_T ct[4];    
    AWB_SGC_RES_T sinColorResult;
    bool converged;
} wbInfo_t;

XCamReturn rk_aiq_uapi_getCCT(const rk_aiq_sys_ctx_t* ctx, float *CCT, float *CCRI );

XCamReturn rk_aiq_uapi_calcWbGainbyCCT(const rk_aiq_sys_ctx_t* ctx, float CCT, float CCRI, wbGain_t *gain);

XCamReturn rk_aiq_uapi_setWbAttr(const rk_aiq_sys_ctx_t* ctx, wbAttr_t *attr);

XCamReturn rk_aiq_uapi_getWbAttr(const rk_aiq_sys_ctx_t* ctx, wbAttr_t *attr);

XCamReturn rk_aiq_uapi_queryWBInfo(const rk_aiq_sys_ctx_t* ctx, wbInfo_t *info);

XCamReturn rk_aiq_uapi_lockAwb(const rk_aiq_sys_ctx_t* ctx);

XCamReturn rk_aiq_uapi_unlockAwb(const rk_aiq_sys_ctx_t* ctx);


#endif

