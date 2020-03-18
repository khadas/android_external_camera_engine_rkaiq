#ifndef _RK_AIQ_USER_API_AWB_H_
#define _RK_AIQ_USER_API_AWB_H_

#include "awb/rk_aiq_uapi_awb_int.h"

XCamReturn
rk_aiq_user_api_awb_SetAttrib(rk_aiq_sys_ctx_t* sys_ctx, rk_aiq_wb_attrib_t attr);
XCamReturn
rk_aiq_user_api_awb_GetAttrib(rk_aiq_sys_ctx_t* sys_ctx, rk_aiq_wb_attrib_t *attr);
XCamReturn
rk_aiq_user_api_awb_GetCCT(rk_aiq_sys_ctx_t* sys_ctx, rk_aiq_wb_cct_t *cct);
XCamReturn
rk_aiq_user_api_awb_QueryWBInfo(rk_aiq_sys_ctx_t* sys_ctx, rk_aiq_wb_querry_info_t *wb_querry_info);

#endif
