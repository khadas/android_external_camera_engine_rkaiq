#include "rk_aiq_uapi_agamma_int.h"
#include "agamma/rk_aiq_types_agamma_algo_prvt.h"
XCamReturn
rk_aiq_uapi_agamma_SetAttrib(RkAiqAlgoContext *ctx,
                          rk_aiq_gamma_attrib_t attr,
                          bool need_sync)
{

    agamma_context_t* gamma_contex = (agamma_context_t*)ctx;
    gamma_contex->mNewAtt = attr;
    gamma_contex->updateAtt = true;

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
rk_aiq_uapi_agamma_GetAttrib(const RkAiqAlgoContext *ctx,
                          rk_aiq_gamma_attrib_t *attr)
{

    agamma_context_t* gamma_contex = (agamma_context_t*)ctx;

    memcpy(attr,&gamma_contex->mCurAtt,sizeof(rk_aiq_gamma_attrib_t));

    return XCAM_RETURN_NO_ERROR;
}


						

