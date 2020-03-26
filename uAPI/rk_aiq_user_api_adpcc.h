#ifndef _RK_AIQ_USER_API_ADPCC_H_
#define _RK_AIQ_USER_API_ADPCC_H_

#include "adpcc/rk_aiq_uapi_adpcc_int.h"

XCamReturn
rk_aiq_user_api_adpcc_SetAttrib(rk_aiq_sys_ctx_t* sys_ctx, rk_aiq_dpcc_attrib_t *attr);
XCamReturn
rk_aiq_user_api_adpcc_GetAttrib(rk_aiq_sys_ctx_t* sys_ctx, rk_aiq_dpcc_attrib_t *attr);

#endif

