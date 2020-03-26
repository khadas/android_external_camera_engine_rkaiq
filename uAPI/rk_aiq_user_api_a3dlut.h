#ifndef _RK_AIQ_USER_API_A3DLUT_H_
#define _RK_AIQ_USER_API_A3DLUT_H_

#include "a3dlut/rk_aiq_uapi_a3dlut_int.h"

XCamReturn
rk_aiq_user_api_a3dlut_SetAttrib(rk_aiq_sys_ctx_t* sys_ctx, rk_aiq_lut3d_attrib_t attr);
XCamReturn
rk_aiq_user_api_a3dlut_GetAttrib(rk_aiq_sys_ctx_t* sys_ctx, rk_aiq_lut3d_attrib_t *attr);
XCamReturn
rk_aiq_user_api_a3dlut_Query3dlutInfo(rk_aiq_sys_ctx_t* sys_ctx, rk_aiq_lut3d_querry_info_t *lut3d_querry_info);

#endif
