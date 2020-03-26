#ifndef __RKAIQ_ASHARP_SHARP_H__
#define __RKAIQ_ASHARP_SHARP_H__

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "math.h"
#include "base/xcam_log.h"
#include "rk_aiq_comm.h"
#include "RkAiqCalibDbTypes.h"
#include "rk_aiq_types_asharp_algo_prvt.h"


RKAIQ_BEGIN_DECLARE

AsharpResult_t init_sharp_params_v1(RKAsharp_Sharp_HW_Params_t *pParams, CalibDb_Sharp_t *pCalibdb);

AsharpResult_t select_rk_sharpen_hw_params_by_ISO(RKAsharp_Sharp_HW_Params_t *strksharpenParams, RKAsharp_Sharp_HW_Params_Select_t *strksharpenParamsSelected, int iso);

AsharpResult_t select_rk_sharpen_hw_v2_params_by_ISO(RKAsharp_Sharp_HW_V2_Params_t *strksharpenParams, RKAsharp_Sharp_HW_V2_Params_Select_t *strksharpenParamsSelected, int iso);

AsharpResult_t select_rk_sharpen_hw_v3_params_by_ISO (RKAsharp_Sharp_HW_V3_Params_t *strksharpenParams, RKAsharp_Sharp_HW_V3_Params_Select_t *strksharpenParamsSelected, int iso);

void select_sharpen_params_by_ISO(RKAsharp_Sharp_Params_t *strksharpenParams, RKAsharp_Sharp_Params_Select_t *strksharpenParamsSelected, int iso);

AsharpResult_t rk_Sharp_V1_fix_transfer(RKAsharp_Sharp_HW_Params_Select_t *pSharpV1, RKAsharp_Sharp_HW_Fix_t* pSharpCfg);

AsharpResult_t rk_Sharp_fix_transfer(RKAsharp_Sharp_Params_Select_t* sharp, RKAsharp_Sharp_Fix_t* pSharpCfg);

RKAIQ_END_DECLARE

#endif

