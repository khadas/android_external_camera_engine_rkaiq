
#ifndef __RKAIQ_ANR_H__
#define __RKAIQ_ANR_H__

#include "stdio.h"
#include "string.h"
#include "math.h"

#include "rk_aiq_anr_algo_uvnr.h"
#include "rk_aiq_anr_algo_ynr.h"
#include "rk_aiq_anr_algo_mfnr.h"
#include "rk_aiq_anr_algo_bayernr.h"
#include "rk_aiq_types_anr_algo_prvt.h"

RKAIQ_BEGIN_DECLARE

//anr inint
ANRresult_t ANRInit(ANRContext_t **ppANRCtx, CamCalibDbContext_t *pCalibDb);

//anr release
ANRresult_t ANRRelease(ANRContext_t *pANRCtx);

//anr config
ANRresult_t ANRPrepare(ANRContext_t *pANRCtx, ANRConfig_t* pANRConfig);

//anr reconfig
ANRresult_t ANRReConfig(ANRContext_t *pANRCtx, ANRConfig_t* pANRConfig);

//anr preprocess
ANRresult_t ANRPreProcess(ANRContext_t *pANRCtx);

//anr process
ANRresult_t ANRProcess(ANRContext_t *pANRCtx, ANRExpInfo_t *pExpInfo);

//anr get result
ANRresult_t ANRGetProcResult(ANRContext_t *pANRCtx, ANRProcResult_t* pANRResult);

RKAIQ_END_DECLARE

#endif
