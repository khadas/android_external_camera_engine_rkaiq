#include "rk_aiq_uapi_asharp_int.h"
#include "asharp/rk_aiq_types_asharp_algo_prvt.h"


XCamReturn
rk_aiq_uapi_asharp_SetAttrib(RkAiqAlgoContext *ctx,
                             rk_aiq_sharp_attrib_t *attr,
                             bool need_sync)
{

    AsharpContext_t* pAsharpCtx = (AsharpContext_t*)ctx;

    pAsharpCtx->eMode = attr->eMode;
    pAsharpCtx->stAuto = attr->stAuto;
    pAsharpCtx->stManual = attr->stManual;

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
rk_aiq_uapi_asharp_GetAttrib(const RkAiqAlgoContext *ctx,
                             rk_aiq_sharp_attrib_t *attr)
{

    AsharpContext_t* pAsharpCtx = (AsharpContext_t*)ctx;

    attr->eMode = pAsharpCtx->eMode;
    memcpy(&attr->stAuto, &pAsharpCtx->stAuto, sizeof(Asharp_Auto_Attr_t));
    memcpy(&attr->stManual, &pAsharpCtx->stManual, sizeof(Asharp_Manual_Attr_t));

    return XCAM_RETURN_NO_ERROR;
}


