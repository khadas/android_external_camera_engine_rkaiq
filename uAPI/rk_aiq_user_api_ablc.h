#ifndef _RK_AIQ_USER_API_ABLC_H_
#define _RK_AIQ_USER_API_ABLC_H_

#include "ablc/rk_aiq_uapi_ablc_int.h"

XCamReturn
rk_aiq_user_api_ablc_SetAttrib(rk_aiq_sys_ctx_t* sys_ctx, rk_aiq_blc_attrib_t *attr);
XCamReturn
rk_aiq_user_api_ablc_GetAttrib(rk_aiq_sys_ctx_t* sys_ctx, rk_aiq_blc_attrib_t *attr);

#endif

