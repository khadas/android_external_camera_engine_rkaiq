
#ifndef __RKAIQ_ASHARP_EDGEFILTER_H__
#define __RKAIQ_ASHARP_EDGEFILTER_H__

#include "stdio.h"
#include "string.h"
#include "math.h"
#include "base/xcam_log.h"
#include "rk_aiq_comm.h"
#include "RkAiqCalibDbTypes.h"
#include "rk_aiq_types_asharp_algo_prvt.h"


RKAIQ_BEGIN_DECLARE

AsharpResult_t init_edgefilter_params(RKAsharp_EdgeFilter_Params_t *pParams, CalibDb_EdgeFilter_t *pCalibdb);

AsharpResult_t select_edgefilter_params_by_ISO(RKAsharp_EdgeFilter_Params_t *strkedgefilterParams,		RKAsharp_EdgeFilter_Params_Select_t *strkedgefilterParamsSelected, int iso);

AsharpResult_t edgefilter_fix_transfer(RKAsharp_EdgeFilter_Params_Select_t* edgeflt, RKAsharp_Edgefilter_Fix_t* pEdgefilterCfg);

RKAIQ_END_DECLARE

#endif

