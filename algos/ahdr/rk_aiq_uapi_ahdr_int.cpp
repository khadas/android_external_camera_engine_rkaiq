#include "rk_aiq_uapi_ahdr_int.h"
#include "rk_aiq_types_ahdr_algo_prvt.h"


XCamReturn
rk_aiq_uapi_ahdr_SetAttrib
(
    RkAiqAlgoContext* ctx,
    ahdr_attrib_t attr,
    bool need_sync
)
{
    if(ctx == NULL) {
        LOGE_AHDR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
        return XCAM_RETURN_ERROR_PARAM;
    }

    AhdrContext_t* pAhdrCtx = (AhdrContext_t*)(ctx->AhdrInstConfig.hAhdr);

	//Todo
	pAhdrCtx->hdrAttr.bEnable = attr.bEnable;
	pAhdrCtx->hdrAttr.bEnable = false;

    if (need_sync == true)
    {
        pAhdrCtx->hdrAttr.stAuto.bUpdateMge = attr.stAuto.bUpdateMge;
        pAhdrCtx->hdrAttr.stAuto.bUpdateTmo = attr.stAuto.bUpdateTmo;
        memcpy(&pAhdrCtx->hdrAttr.stAuto.stMgeAuto, &attr.stAuto.stMgeAuto, sizeof(amgeAttr_t));
        memcpy(&pAhdrCtx->hdrAttr.stAuto.stTmoAuto, &attr.stAuto.stMgeAuto, sizeof(atmoAttr_t));

        pAhdrCtx->hdrAttr.stManual.bUpdateMge = attr.stManual.bUpdateMge;
        pAhdrCtx->hdrAttr.stManual.bUpdateTmo = attr.stManual.bUpdateTmo;
        memcpy(&pAhdrCtx->hdrAttr.stManual.stMgeManual, &attr.stManual.stMgeManual, sizeof(mmgeAttr_t));
        memcpy(&pAhdrCtx->hdrAttr.stManual.stTmoManual, &attr.stManual.stTmoManual, sizeof(mtmoAttr_t));
    }
    else
    {
        pAhdrCtx->hdrAttr.stAuto.bUpdateMge = need_sync;
        pAhdrCtx->hdrAttr.stAuto.bUpdateTmo = need_sync;
        pAhdrCtx->hdrAttr.stManual.bUpdateMge = need_sync;
        pAhdrCtx->hdrAttr.stManual.bUpdateTmo = need_sync;
    }

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
rk_aiq_uapi_ahdr_GetAttrib
(
    RkAiqAlgoContext*  ctx,
    ahdr_attrib_t* attr
)
{
    if(ctx == NULL || attr == NULL) {
        LOGE_AHDR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
        return XCAM_RETURN_ERROR_PARAM;
    }

    AhdrContext_t* pAhdrCtx = (AhdrContext_t*)ctx->AhdrInstConfig.hAhdr;

    attr->bEnable = pAhdrCtx->hdrAttr.bEnable;
    attr->opMode = pAhdrCtx->hdrAttr.opMode;
    attr->stAuto.bUpdateMge = false;
    attr->stAuto.bUpdateTmo = false;
    attr->stManual.bUpdateMge = false;
    attr->stManual.bUpdateTmo = false;

    memcpy(&attr->stAuto.stMgeAuto, &pAhdrCtx->hdrAttr.stAuto.stMgeAuto, sizeof(amgeAttr_t));
    memcpy(&attr->stAuto.stTmoAuto, &pAhdrCtx->hdrAttr.stAuto.stTmoAuto, sizeof(atmoAttr_t));

    memcpy(&attr->stManual.stMgeManual, &pAhdrCtx->hdrAttr.stManual.stMgeManual, sizeof(mmgeAttr_t));
    memcpy(&attr->stManual.stTmoManual, &pAhdrCtx->hdrAttr.stManual.stTmoManual, sizeof(mtmoAttr_t));

    return XCAM_RETURN_NO_ERROR;
}

