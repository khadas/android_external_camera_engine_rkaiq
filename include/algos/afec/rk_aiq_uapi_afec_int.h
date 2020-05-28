#ifndef _RK_AIQ_UAPI_AFEC_INT_H_
#define _RK_AIQ_UAPI_AFEC_INT_H_

#include "base/xcam_common.h"
#include "rk_aiq_algo_des.h"
#include "afec/rk_aiq_types_afec_algo_int.h"

XCamReturn rk_aiq_uapi_afec_enable(RkAiqAlgoContext *ctx);
XCamReturn rk_aiq_uapi_afec_disable(RkAiqAlgoContext *ctx);

#endif
