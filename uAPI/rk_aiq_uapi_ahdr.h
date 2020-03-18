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

#ifndef _RK_AIQ_UAPI_AHDR_H_
#define _RK_AIQ_UAPI_AHDR_H_
#include "base/xcam_common.h"
#include "rk_aiq.h"


typedef struct tmoCtrlData_s
{
    float stCoef;
    float stCoefMax;
    float stCoefMin;
    int   stMax;
    int   stMin;
} tmoCtrlData_t;

typedef struct mgeCtrlData_S
{
    float stCoef;
    float stCoefMax;
    float stCoefMin;
    int   stSmthMax;
    int   stSmthMin;
    int   stOfstMax;
    int   stOfstMin;
} mgeCtrlData_t;

typedef struct amgeAttr_s
{
    mgeCtrlData_t stMDCurveLM;
    mgeCtrlData_t stMDCurveMS;
    mgeCtrlData_t stOECurve;
} amgeAttr_t;

typedef struct atmoAttr_S
{
    tmoCtrlData_t stGlobeMaxLuma;
    tmoCtrlData_t stGlobeLuma;
    tmoCtrlData_t stDtlsLL;
    tmoCtrlData_t stDtlsHL;
    tmoCtrlData_t stNosLL;
    bool  enSmthCtrl;
    tmoCtrlData_t stSmthCtrlCoef1;
    tmoCtrlData_t stSmthCtrlCoef2;
} atmoAttr_t;

typedef struct mmgeAttr_S
{
    float stMDCurveLM_smth;
    float stMDCurveLM_ofst;
    float stMDCurveMS_smth;
    float stMDCurveMS_ofst;
    float stOECurve_smth;
    float stOECurve_ofst;

} mmgeAttr_t;

typedef struct mtmoAttr_S
{
    float stGlobeMaxLuma;
    float stGlobeLuma;
    float stDtlsHL;
    float stDtlsLL;
    float stNosLL;
    bool  enSmthCtrl;
    int   stSmthCtrlCoef1;
    int   stSmthCtrlCoef2;

} mtmoAttr_t;

typedef struct ahdrAttr_S
{
    amgeAttr_t stMgeAuto;
    atmoAttr_t stTmoAuto;

} ahdrAttr_t;

typedef struct mhdrAttr_s
{
    mmgeAttr_t stMgeManual;
    mtmoAttr_t stTmoManual;
} mhdrAttr_t;

typedef struct hdrAttr_s
{
    bool      bEnable;
    opMode_t  opMode;
    ahdrAttr_t    stAuto;
    mhdrAttr_t stManual;
} hdrAttr_t;


XCamReturn rk_aiq_uapi_setHDRAttr(const rk_aiq_sys_ctx_t* ctx, hdrAttr_t *pstHDRAttr);

XCamReturn rk_aiq_uapi_getHDRAttr(const rk_aiq_sys_ctx_t* ctx, hdrAttr_t *pstHDRAttr);



