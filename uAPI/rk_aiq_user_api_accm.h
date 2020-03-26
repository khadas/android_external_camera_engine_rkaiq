#ifndef _RK_AIQ_USER_API_ACCM_H_
#define _RK_AIQ_USER_API_ACCM_H_

#include "accm/rk_aiq_uapi_accm_int.h"

XCamReturn
rk_aiq_user_api_accm_SetAttrib(rk_aiq_sys_ctx_t* sys_ctx, rk_aiq_ccm_attrib_t attr);
XCamReturn
rk_aiq_user_api_accm_GetAttrib(rk_aiq_sys_ctx_t* sys_ctx, rk_aiq_ccm_attrib_t *attr);
XCamReturn
rk_aiq_user_api_accm_QueryCcmInfo(rk_aiq_sys_ctx_t* sys_ctx, rk_aiq_ccm_querry_info_t *ccm_querry_info);

#endif
