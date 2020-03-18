/******************************************************************************
 *
 * Copyright 2019, Fuzhou Rockchip Electronics Co.Ltd . All rights reserved.
 * No part of this work may be reproduced, modified, distributed, transmitted,
 * transcribed, or translated into any language or computer format, in any form
 * or by any means without written permission of:
 * Fuzhou Rockchip Electronics Co.Ltd .
 *
 *
 *****************************************************************************/
#ifndef __RK_AIQ_UAPI_AE_H__
#define __RK_AIQ_UAPI_AE_H__
#include "base/xcam_common.h"
#include "rk_aiq.h"
#include "rk_aiq_ae_api_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum aeMode_e {
    AE_AUTO = 0,
    AE_IRIS_PRIOR = 1,
    AE_SHUTTER_PRIOR = 2
} aeMode_t;

typedef enum expPwrLineFreq_e {
	EXP_PWR_LINE_FREQ_DIS	= 0,
	EXP_PWR_LINE_FREQ_50HZ	= 1,
	EXP_PWR_LINE_FREQ_60HZ	= 2,
	EXP_PWR_LINE_FREQ_AUTO	= 3,
} expPwrLineFreq_t;



XCamReturn rk_aiq_uapi_setAeHWAttr(const rk_aiq_sys_ctx_t* ctx, const RkAiqApiAecHw_Attr_t*pAeHwAttr);
XCamReturn rk_aiq_uapi_getAeHWAttr(const rk_aiq_sys_ctx_t* ctx, RkAiqApiAecHw_Attr_t* pAeHwAttr);
XCamReturn rk_aiq_uapi_setAeSWMainCtrlAttr(const rk_aiq_sys_ctx_t* ctx, const RkAiqApiAecMainCtrl_Attr_t* pMainCtrlAttr);
XCamReturn rk_aiq_uapi_getAeSWMainCtrlAttr(const rk_aiq_sys_ctx_t* ctx, RkAiqApiAecMainCtrl_Attr_t* pMainCtrlAttr);
XCamReturn rk_aiq_uapi_setLinearAeRouteAttr(const rk_aiq_sys_ctx_t* ctx, const RkAiqApiLinAeRoute_Attr_t* pLinAeRouteAttr);
XCamReturn rk_aiq_uapi_getLinearAeRouteAttr(const rk_aiq_sys_ctx_t* ctx, RkAiqApiLinAeRoute_Attr_t* pLinAeRouteAttr);
XCamReturn rk_aiq_uapi_setHdrAeRouteAttr(const rk_aiq_sys_ctx_t* ctx, const RkAiqApiHdrAeRoute_Attr_t* pHdrAeRouteAttr);
XCamReturn rk_aiq_uapi_getHdrAeRouteAttr(const rk_aiq_sys_ctx_t* ctx, RkAiqApiHdrAeRoute_Attr_t* pHdrAeRouteAttr);
XCamReturn rk_aiq_uapi_QueryExposureInfo(const rk_aiq_sys_ctx_t* ctx, RKAiqApiExpInfo_t* pExpInfo);

XCamReturn rk_aiq_uapi_setLinearAEAttr(const rk_aiq_sys_ctx_t* ctx, const RkAiqApiLinearAE_Attr_t* pLinAeAttr);
XCamReturn rk_aiq_uapi_getLinearAEAttr(const rk_aiq_sys_ctx_t* ctx, RkAiqApiLinearAE_Attr_t* pLinAeAttr);
XCamReturn rk_aiq_uapi_setHdrAEAttr(const rk_aiq_sys_ctx_t* ctx, const RkAiqApiHdrAE_Attr_t* pHdrAeAttr);
XCamReturn rk_aiq_uapi_getHdrAEAtt(const rk_aiq_sys_ctx_t* ctx, RkAiqApiHdrAE_Attr_t* pHdrAeAttr);


#ifdef __cplusplus
}
#endif




#endif /*__RK_AIQ_UAPI_AE_H__*/
