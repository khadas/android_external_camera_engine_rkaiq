#include "rk_aiq_uapi_anr_int.h"
#include "anr/rk_aiq_types_anr_algo_prvt.h"


XCamReturn
rk_aiq_uapi_anr_SetAttrib(RkAiqAlgoContext *ctx,
                          rk_aiq_nr_attrib_t *attr,
                          bool need_sync)
{

    ANRContext_t* pAnrCtx = (ANRContext_t*)ctx;

    pAnrCtx->eMode = attr->eMode;
    pAnrCtx->stAuto = attr->stAuto;
    pAnrCtx->stManual = attr->stManual;

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
rk_aiq_uapi_anr_GetAttrib(const RkAiqAlgoContext *ctx,
                          rk_aiq_nr_attrib_t *attr)
{

    ANRContext_t* pAnrCtx = (ANRContext_t*)ctx;

    attr->eMode = pAnrCtx->eMode;
    memcpy(&attr->stAuto, &pAnrCtx->stAuto, sizeof(ANR_Auto_Attr_t));
    memcpy(&attr->stManual, &pAnrCtx->stManual, sizeof(ANR_Manual_Attr_t));

    return XCAM_RETURN_NO_ERROR;
}


