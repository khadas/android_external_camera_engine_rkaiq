#ifndef __RK_ISP20_MFNR_PARSE_PARAMS_
#define __RK_ISP20_MFNR_PARSE_PARAMS_

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "base/xcam_log.h"
#include "rk_aiq_comm.h"
#include "RkAiqCalibDbTypes.h"
#include "anr/rk_aiq_types_anr_algo_prvt.h"

RKAIQ_BEGIN_DECLARE

ANRresult_t init_mfnr_params(RKAnr_Mfnr_Params_t *pParams, CalibDb_MFNR_t *pCalibdb);

ANRresult_t select_mfnr_params_by_ISO(RKAnr_Mfnr_Params_t *stmfnrParams, 	RKAnr_Mfnr_Params_Select_t *stmfnrParamsSelected, ANRExpInfo_t *pExpInfo, int bits_proc);

ANRresult_t mfnr_fix_transfer(RKAnr_Mfnr_Params_Select_t* tnr, RKAnr_Mfnr_Fix_t *pMfnrCfg, ANRExpInfo_t *pExpInfo);

ANRresult_t mfnr_fix_Printf(RKAnr_Mfnr_Fix_t  * pMfnrCfg);

RKAIQ_END_DECLARE

#endif


