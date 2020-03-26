#ifndef _RK_AIQ_USER_API_AGAMMA_H_
#define _RK_AIQ_USER_API_AGAMMA_H_

#include "agamma/rk_aiq_uapi_agamma_int.h"

XCamReturn
rk_aiq_user_api_agamma_SetAttrib(rk_aiq_sys_ctx_t* sys_ctx, rk_aiq_gamma_attrib_t attr);
XCamReturn
rk_aiq_user_api_agamma_GetAttrib(rk_aiq_sys_ctx_t* sys_ctx, rk_aiq_gamma_attrib_t *attr);

#endif
