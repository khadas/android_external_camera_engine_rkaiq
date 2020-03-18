/******************************************************************************
 *
 * Copyright 2019, Fuzhou Rockchip Electronics Co.Ltd . All rights reserved.
 * No part of this work may be reproduced, modified, distributed, transmitted,
 * transcribed, or translated into any language or computer format, in any form
 * or by any means without written permission of:
 * Fuzhou Rockchip Electronics Co.Ltd .
 *
 *
 *****************************************************************************/
#ifndef __RK_AIQ_TYPES_AHDR_ALGO_PRVT_H__
#define __RK_AIQ_TYPES_AHDR_ALGO_PRVT_H__

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "ae/rk_aiq_types_ae_algo_int.h"
#include "af/rk_aiq_types_af_algo_int.h"
#include "rk_aiq_algo_types.h"
#include "RkAiqCalibDbTypes.h"

#include "ae/rk_aiq_types_ae_algo_int.h"
#include "af/rk_aiq_af_hw_v200.h"
#include "rk_aiq_types_ahdr_stat_v200.h"
#include "rk_aiq_uapi_ahdr_int.h"


#define MIN(a,b)             ((a) <= (b) ? (a):(b))
#define MAX(a,b)             ((a) >= (b) ? (a):(b))

#define LIMIT_VALUE(value,max_value,min_value)      (value > max_value? max_value : value < min_value ? min_value : value)
#define SHIFT6BIT(A)         (A*64)
#define SHIFT7BIT(A)         (A*128)
#define SHIFT10BIT(A)         (A*1024)
#define SHIFT11BIT(A)         (A*2048)
#define SHIFT12BIT(A)         (A*4096)

#define LRK_DEBUG_LOG  (0)

#define AHDR_RET_SUCCESS             0   //!< this has to be 0, if clauses rely on it
#define AHDR_RET_FAILURE             1   //!< general failure
#define AHDR_RET_NOTSUPP             2   //!< feature not supported
#define AHDR_RET_BUSY                3   //!< there's already something going on...
#define AHDR_RET_CANCELED            4   //!< operation canceled
#define AHDR_RET_OUTOFMEM            5   //!< out of memory
#define AHDR_RET_OUTOFRANGE          6   //!< parameter/value out of range
#define AHDR_RET_IDLE                7   //!< feature/subsystem is in idle state
#define AHDR_RET_WRONG_HANDLE        8   //!< handle is wrong
#define AHDR_RET_NULL_POINTER        9   //!< the/one/all parameter(s) is a(are) NULL pointer(s)
#define AHDR_RET_NOTAVAILABLE       10   //!< profile not available
#define AHDR_RET_DIVISION_BY_ZERO   11   //!< a divisor equals ZERO
#define AHDR_RET_WRONG_STATE        12   //!< state machine in wrong state
#define AHDR_RET_INVALID_PARM       13   //!< invalid parameter
#define AHDR_RET_PENDING            14   //!< command pending
#define AHDR_RET_WRONG_CONFIG       15   //!< given configuration is invalid

#define ENVLVMAX     (1.0)
#define ENVLVMIN     (0.0)
#define MOVECOEFMAX     (1.0)
#define MOVECOEFMIN     (0.0)
#define OEPDFMAX     (0.5)
#define OEPDFMIN     (0.0)
#define OECURVESMOOTHMAX     (200)
#define OECURVESMOOTHMIN     (20)
#define OECURVEOFFSETMAX     (230)
#define OECURVEOFFSETMIN     (154)
#define MDCURVESMOOTHMAX     (200)
#define MDCURVESMOOTHMIN     (20)
#define MDCURVEOFFSETMAX     (103)
#define MDCURVEOFFSETMIN     (26)
#define NOISELOWLIGHTMAX    (100)
#define NOISELOWLIGHTMIN     (1)

#define DAMPMAX     (1.0)
#define DAMPMIN     (0.0)
#define ENMAX     (1.0)
#define ENMIN     (0.0)
#define DAYTHMAX     (1.0)
#define DAYTHMIN     (0.0)
#define DARKPDFTHMAX     (1.0)
#define DARKPDFTHMIN     (0.0)

#define GLOBEMAXLUMAMAX     (737)
#define GLOBEMAXLUMAMIN     (46)
#define GLOBELUMAMAX     (737)
#define GLOBELUMAMIN     (46)
#define DETAILSHIGHLIGHTMAX     (737)
#define DETAILSHIGHLIGHTMIN     (46)
#define DARKTEXTUREPDFMAX     (1)
#define DARKTEXTUREPDFMIN     (0)

#define DETAILSLOWLIGHTMAX     (63)
#define DETAILSLOWLIGHTMIN     (16)
#define POSCOEFMAX     (1)
#define POSCOEFMIN     (0)
#define DYNAMICRANGEMAX     (84)
#define DYNAMICRANGEMIN     (1)
#define POSWEIGHTMAX     (1)
#define POSWEIGHTMIN     (0)
#define LUMAWEIGHTMAX     (1)
#define LUMAWEIGHTMIN     (0)

#define SMOOTHCONTROLCOEFMAX     (255)
#define SMOOTHCONTROLCOEFMIN     (0)

typedef enum AhdrState_e {
    AHDR_STATE_INVALID       = 0,
    AHDR_STATE_INITIALIZED   = 1,
    AHDR_STATE_STOPPED       = 2,
    AHDR_STATE_RUNNING       = 3,
    AHDR_STATE_LOCKED        = 4,
    AHDR_STATE_MAX
} AhdrState_t;

typedef struct TmoHandleData_s
{
    float GlobeMaxLuma;
    float GlobeLuma;
    float DetailsHighLight;
    float DetailsLowLight;
    float NoiseLowLight;
    float SmoothControlCoef1;
    float SmoothControlCoef2;

} TmoHandleData_t;

typedef struct MergeHandleData_s
{
    float OECurve_smooth;
    float OECurve_offset;
    float MDCurveLM_smooth;
    float MDCurveLM_offset;
    float MDCurveMS_smooth;
    float MDCurveMS_offset;
} MergeHandleData_t;

typedef struct AhdrPrevData_s
{
    float PreL2S_ratio;
    float PreLExpo;
    MergeHandleData_t PrevMergeHandleData;
    TmoHandleData_t PrevTmoHandleData;
    unsigned short ro_hdrtmo_lgmean_all[16];
} AhdrPrevData_t;

typedef struct CurrAeResult_s {
    //TODO
    float MeanLuma[3];
    float LfrmDarkLuma;
    float LfrmDarkPdf;
    float LfrmOverExpPdf;
    float SfrmMaxLuma;
    float SfrmMaxLumaPdf;
    float GlobalEnvLv;
    float L2M_Ratio;
    float M2S_Ratio;
    float DynamicRange;
    float OEPdf;

    float Lv_fac;
    float DarkPdf_fac;
    float Contrast_fac;
} CurrAeResult_t;

typedef struct {
    unsigned char valid;
    int id;
    int depth;
} AfDepthInfo_t;

typedef struct CurrAfResult_s {
    unsigned int CurrAfTargetPos;
    unsigned int CurrAfTargetWidth;
    unsigned int CurrAfTargetHeight;
    AfDepthInfo_t AfDepthInfo[225];
    unsigned int GlobalSharpnessCompensated[225];
} CurrAfResult_t;

typedef struct CurrHandleData_s
{
    int MergeMode;
    float CurrL2S_Ratio;
    float CurrL2M_Ratio;
    float CurrL2L_Ratio;
    float CurrLExpo;
    float CurrEnvLv;
    float CurrTotalPosWeight;
    float CurrMoveCoef;
    float CurrDynamicRange;
    float CurrOEPdf;
    float CurrDarkPdf;
    float DayTh;
    float DarkPdfTH;
    float CurrTotalSharpness;
    float TmoDamp;
    float LumaWeight[225];
    float PosWeight[225];
    float MergeOEDamp;
    float MergeMDDampLM;
    float MergeMDDampMS;
    MergeHandleData_t CurrMergeHandleData;
    TmoHandleData_t CurrTmoHandleData;
} CurrHandleData_t;


typedef struct AhdrContext_s
{
    //api
    bool      bEnable;
    OpMode_t Mode;
    ahdrAttr_t    stAuto;
    mhdrAttr_t stManual;

    AhdrState_t state;
    AhdrConfig_t AhdrConfig;
    AhdrPrevData_t AhdrPrevData ;
    AhdrIOData_t AhdrIOData;
    CurrAeResult_t CurrAeResult;
    CurrAfResult_t CurrAfResult;
    CurrHandleData_t CurrHandleData;
    rkisp_ahdr_stats_t CurrStatsData;
    uint32_t width;
    uint32_t height;
    int frameCnt;
} AhdrContext_t;

typedef struct AhdrContext_s* AhdrHandle_t;

typedef struct AhdrInstanceConfig_s {
    AhdrHandle_t              hAhdr;
} AhdrInstanceConfig_t;

typedef struct _RkAiqAlgoContext {
    AhdrInstanceConfig_t AhdrInstConfig;
    //void* place_holder[0];
} RkAiqAlgoContext;


#endif
