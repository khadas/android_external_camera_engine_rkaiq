#ifndef __RK_AIQ_USER_API_AE_H__
#define __RK_AIQ_USER_API_AE_H__

#include "ae/rk_aiq_uapi_ae_int.h"

#ifdef __cplusplus
extern "C"
{
#endif

XCamReturn rk_aiq_user_api_ae_setExpSwAttr(const rk_aiq_sys_ctx_t* ctx, const Uapi_ExpSwAttr_t* pExpSwAttr);
XCamReturn rk_aiq_user_api_ae_getExpSwAttr(const rk_aiq_sys_ctx_t* ctx, Uapi_ExpSwAttr_t* pExpSwAttr);
XCamReturn rk_aiq_user_api_ae_setLinAeRouteAttr(const rk_aiq_sys_ctx_t* ctx, const Uapi_LinAeRouteAttr_t* pLinAeRouteAttr);
XCamReturn rk_aiq_user_api_ae_getLinAeRouteAttr(const rk_aiq_sys_ctx_t* ctx, Uapi_LinAeRouteAttr_t* pLinAeRouteAttr);
XCamReturn rk_aiq_user_api_ae_setHdrAeRouteAttr(const rk_aiq_sys_ctx_t* ctx, const Uapi_HdrAeRouteAttr_t* pHdrAeRouteAttr);
XCamReturn rk_aiq_user_api_ae_getHdrAeRouteAttr(const rk_aiq_sys_ctx_t* ctx, Uapi_HdrAeRouteAttr_t* pHdrAeRouteAttr);
XCamReturn rk_aiq_user_api_ae_queryExpResInfo(const rk_aiq_sys_ctx_t* ctx, Uapi_ExpQueryInfo_t* pExpResInfo);

XCamReturn rk_aiq_user_api_ae_setLinExpAttr(const rk_aiq_sys_ctx_t* ctx, const Uapi_LinExpAttr_t* pLinExpAttr);
XCamReturn rk_aiq_user_api_ae_getLinExpAttr(const rk_aiq_sys_ctx_t* ctx, Uapi_LinExpAttr_t* pLinExpAttr);
XCamReturn rk_aiq_user_api_ae_setHdrExpAttr(const rk_aiq_sys_ctx_t* ctx, const Uapi_HdrExpAttr_t* pHdrExpAttr);
XCamReturn rk_aiq_user_api_ae_getHdrExpAttr(const rk_aiq_sys_ctx_t* ctx, Uapi_HdrExpAttr_t* pHdrExpAttr);


#ifdef __cplusplus
}
#endif





#endif /*__RK_AIQ_USER_API_AE_H__*/
