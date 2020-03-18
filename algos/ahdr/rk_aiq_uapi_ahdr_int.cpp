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

    pAhdrCtx->bEnable = attr.bEnable;
    pAhdrCtx->Mode = attr.opMode;

    pAhdrCtx->stAuto.stMgeAuto.stMDCurveLM.stCoef = attr.stAuto.stMgeAuto.stMDCurveLM.stCoef;
    pAhdrCtx->stAuto.stMgeAuto.stMDCurveLM.stCoefMax = attr.stAuto.stMgeAuto.stMDCurveLM.stCoefMax;
    pAhdrCtx->stAuto.stMgeAuto.stMDCurveLM.stCoefMin = attr.stAuto.stMgeAuto.stMDCurveLM.stCoefMin;
    pAhdrCtx->stAuto.stMgeAuto.stMDCurveLM.stOfstMax = attr.stAuto.stMgeAuto.stMDCurveLM.stOfstMax;
    pAhdrCtx->stAuto.stMgeAuto.stMDCurveLM.stOfstMin = attr.stAuto.stMgeAuto.stMDCurveLM.stOfstMin;
    pAhdrCtx->stAuto.stMgeAuto.stMDCurveLM.stSmthMax = attr.stAuto.stMgeAuto.stMDCurveLM.stSmthMax;
    pAhdrCtx->stAuto.stMgeAuto.stMDCurveLM.stSmthMin = attr.stAuto.stMgeAuto.stMDCurveLM.stSmthMin;

    pAhdrCtx->stAuto.stMgeAuto.stMDCurveMS.stCoef = attr.stAuto.stMgeAuto.stMDCurveMS.stCoef;
    pAhdrCtx->stAuto.stMgeAuto.stMDCurveMS.stCoefMax = attr.stAuto.stMgeAuto.stMDCurveMS.stCoefMax;
    pAhdrCtx->stAuto.stMgeAuto.stMDCurveMS.stCoefMin = attr.stAuto.stMgeAuto.stMDCurveMS.stCoefMin;
    pAhdrCtx->stAuto.stMgeAuto.stMDCurveMS.stOfstMax = attr.stAuto.stMgeAuto.stMDCurveMS.stOfstMax;
    pAhdrCtx->stAuto.stMgeAuto.stMDCurveMS.stOfstMin = attr.stAuto.stMgeAuto.stMDCurveMS.stOfstMin;
    pAhdrCtx->stAuto.stMgeAuto.stMDCurveMS.stSmthMax = attr.stAuto.stMgeAuto.stMDCurveMS.stSmthMax;
    pAhdrCtx->stAuto.stMgeAuto.stMDCurveMS.stSmthMin = attr.stAuto.stMgeAuto.stMDCurveMS.stSmthMin;

    pAhdrCtx->stAuto.stMgeAuto.stOECurve.stCoef = attr.stAuto.stMgeAuto.stOECurve.stCoef;
    pAhdrCtx->stAuto.stMgeAuto.stOECurve.stCoefMax = attr.stAuto.stMgeAuto.stOECurve.stCoefMax;
    pAhdrCtx->stAuto.stMgeAuto.stOECurve.stCoefMin = attr.stAuto.stMgeAuto.stOECurve.stCoefMin;
    pAhdrCtx->stAuto.stMgeAuto.stOECurve.stOfstMax = attr.stAuto.stMgeAuto.stOECurve.stOfstMax;
    pAhdrCtx->stAuto.stMgeAuto.stOECurve.stOfstMin = attr.stAuto.stMgeAuto.stOECurve.stOfstMin;
    pAhdrCtx->stAuto.stMgeAuto.stOECurve.stSmthMax = attr.stAuto.stMgeAuto.stOECurve.stSmthMax;
    pAhdrCtx->stAuto.stMgeAuto.stOECurve.stSmthMin = attr.stAuto.stMgeAuto.stOECurve.stSmthMin;

    pAhdrCtx->stAuto.stTmoAuto.stGlobeMaxLuma.stCoef = attr.stAuto.stTmoAuto.stGlobeMaxLuma.stCoef;
    pAhdrCtx->stAuto.stTmoAuto.stGlobeMaxLuma.stCoefMax = attr.stAuto.stTmoAuto.stGlobeMaxLuma.stCoefMax;
    pAhdrCtx->stAuto.stTmoAuto.stGlobeMaxLuma.stCoefMin = attr.stAuto.stTmoAuto.stGlobeMaxLuma.stCoefMin;
    pAhdrCtx->stAuto.stTmoAuto.stGlobeMaxLuma.stMax = attr.stAuto.stTmoAuto.stGlobeMaxLuma.stMax;
    pAhdrCtx->stAuto.stTmoAuto.stGlobeMaxLuma.stMin = attr.stAuto.stTmoAuto.stGlobeMaxLuma.stMin;

    pAhdrCtx->stAuto.stTmoAuto.stGlobeLuma.stCoef = attr.stAuto.stTmoAuto.stGlobeLuma.stCoef;
    pAhdrCtx->stAuto.stTmoAuto.stGlobeLuma.stCoefMax = attr.stAuto.stTmoAuto.stGlobeLuma.stCoefMax;
    pAhdrCtx->stAuto.stTmoAuto.stGlobeLuma.stCoefMin = attr.stAuto.stTmoAuto.stGlobeLuma.stCoefMin;
    pAhdrCtx->stAuto.stTmoAuto.stGlobeLuma.stMax = attr.stAuto.stTmoAuto.stGlobeLuma.stMax;
    pAhdrCtx->stAuto.stTmoAuto.stGlobeLuma.stMin = attr.stAuto.stTmoAuto.stGlobeLuma.stMin;

    pAhdrCtx->stAuto.stTmoAuto.stDtlsLL.stCoef = attr.stAuto.stTmoAuto.stDtlsLL.stCoef;
    pAhdrCtx->stAuto.stTmoAuto.stDtlsLL.stCoefMax = attr.stAuto.stTmoAuto.stDtlsLL.stCoefMax;
    pAhdrCtx->stAuto.stTmoAuto.stDtlsLL.stCoefMin = attr.stAuto.stTmoAuto.stDtlsLL.stCoefMin;
    pAhdrCtx->stAuto.stTmoAuto.stDtlsLL.stMax = attr.stAuto.stTmoAuto.stDtlsLL.stMax;
    pAhdrCtx->stAuto.stTmoAuto.stDtlsLL.stMin = attr.stAuto.stTmoAuto.stDtlsLL.stMin;

    pAhdrCtx->stAuto.stTmoAuto.stDtlsHL.stCoef = attr.stAuto.stTmoAuto.stDtlsHL.stCoef;
    pAhdrCtx->stAuto.stTmoAuto.stDtlsHL.stCoefMax = attr.stAuto.stTmoAuto.stDtlsHL.stCoefMax;
    pAhdrCtx->stAuto.stTmoAuto.stDtlsHL.stCoefMin = attr.stAuto.stTmoAuto.stDtlsHL.stCoefMin;
    pAhdrCtx->stAuto.stTmoAuto.stDtlsHL.stMax = attr.stAuto.stTmoAuto.stDtlsHL.stMax;
    pAhdrCtx->stAuto.stTmoAuto.stDtlsHL.stMin = attr.stAuto.stTmoAuto.stDtlsHL.stMin;

    pAhdrCtx->stAuto.stTmoAuto.stNosLL.stCoef = attr.stAuto.stTmoAuto.stNosLL.stCoef;
    pAhdrCtx->stAuto.stTmoAuto.stNosLL.stCoefMax = attr.stAuto.stTmoAuto.stNosLL.stCoefMax;
    pAhdrCtx->stAuto.stTmoAuto.stNosLL.stCoefMin = attr.stAuto.stTmoAuto.stNosLL.stCoefMin;
    pAhdrCtx->stAuto.stTmoAuto.stNosLL.stMax = attr.stAuto.stTmoAuto.stNosLL.stMax;
    pAhdrCtx->stAuto.stTmoAuto.stNosLL.stMin = attr.stAuto.stTmoAuto.stNosLL.stMin;

    pAhdrCtx->stAuto.stTmoAuto.enSmthCtrl = attr.stAuto.stTmoAuto.enSmthCtrl;

    pAhdrCtx->stAuto.stTmoAuto.stSmthCtrlCoef1.stCoef = attr.stAuto.stTmoAuto.stSmthCtrlCoef1.stCoef;
    pAhdrCtx->stAuto.stTmoAuto.stSmthCtrlCoef1.stCoefMax = attr.stAuto.stTmoAuto.stSmthCtrlCoef1.stCoefMax;
    pAhdrCtx->stAuto.stTmoAuto.stSmthCtrlCoef1.stCoefMin = attr.stAuto.stTmoAuto.stSmthCtrlCoef1.stCoefMin;
    pAhdrCtx->stAuto.stTmoAuto.stSmthCtrlCoef1.stMax = attr.stAuto.stTmoAuto.stSmthCtrlCoef1.stMax;
    pAhdrCtx->stAuto.stTmoAuto.stSmthCtrlCoef1.stMin = attr.stAuto.stTmoAuto.stSmthCtrlCoef1.stMin;

    pAhdrCtx->stAuto.stTmoAuto.stSmthCtrlCoef2.stCoef = attr.stAuto.stTmoAuto.stSmthCtrlCoef2.stCoef;
    pAhdrCtx->stAuto.stTmoAuto.stSmthCtrlCoef2.stCoefMax = attr.stAuto.stTmoAuto.stSmthCtrlCoef2.stCoefMax;
    pAhdrCtx->stAuto.stTmoAuto.stSmthCtrlCoef2.stCoefMin = attr.stAuto.stTmoAuto.stSmthCtrlCoef2.stCoefMin;
    pAhdrCtx->stAuto.stTmoAuto.stSmthCtrlCoef2.stMax = attr.stAuto.stTmoAuto.stSmthCtrlCoef2.stMax;
    pAhdrCtx->stAuto.stTmoAuto.stSmthCtrlCoef2.stMin = attr.stAuto.stTmoAuto.stSmthCtrlCoef2.stMin;

    pAhdrCtx->stManual.stMgeManual.stMDCurveLM_ofst = attr.stManual.stMgeManual.stMDCurveLM_ofst;
    pAhdrCtx->stManual.stMgeManual.stMDCurveLM_smth = attr.stManual.stMgeManual.stMDCurveLM_smth;
    pAhdrCtx->stManual.stMgeManual.stMDCurveMS_ofst = attr.stManual.stMgeManual.stMDCurveMS_ofst;
    pAhdrCtx->stManual.stMgeManual.stMDCurveMS_smth = attr.stManual.stMgeManual.stMDCurveMS_smth;
    pAhdrCtx->stManual.stMgeManual.stOECurve_smth = attr.stManual.stMgeManual.stOECurve_smth;
    pAhdrCtx->stManual.stMgeManual.stOECurve_ofst = attr.stManual.stMgeManual.stOECurve_ofst;

    pAhdrCtx->stManual.stTmoManual.stGlobeMaxLuma = attr.stManual.stTmoManual.stGlobeMaxLuma;
    pAhdrCtx->stManual.stTmoManual.stGlobeLuma = attr.stManual.stTmoManual.stGlobeLuma;
    pAhdrCtx->stManual.stTmoManual.stDtlsLL = attr.stManual.stTmoManual.stDtlsLL;
    pAhdrCtx->stManual.stTmoManual.stDtlsHL = attr.stManual.stTmoManual.stDtlsHL;
    pAhdrCtx->stManual.stTmoManual.stNosLL = attr.stManual.stTmoManual.stNosLL;
    pAhdrCtx->stManual.stTmoManual.enSmthCtrl = attr.stManual.stTmoManual.enSmthCtrl;
    pAhdrCtx->stManual.stTmoManual.stSmthCtrlCoef1 = attr.stManual.stTmoManual.stSmthCtrlCoef1;
    pAhdrCtx->stManual.stTmoManual.stSmthCtrlCoef2 = attr.stManual.stTmoManual.stSmthCtrlCoef2;

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

    pAhdrCtx->bEnable = attr->bEnable;
    pAhdrCtx->Mode = attr->opMode;

    pAhdrCtx->stAuto.stMgeAuto.stMDCurveLM.stCoef = attr->stAuto.stMgeAuto.stMDCurveLM.stCoef;
    pAhdrCtx->stAuto.stMgeAuto.stMDCurveLM.stCoefMax = attr->stAuto.stMgeAuto.stMDCurveLM.stCoefMax;
    pAhdrCtx->stAuto.stMgeAuto.stMDCurveLM.stCoefMin = attr->stAuto.stMgeAuto.stMDCurveLM.stCoefMin;
    pAhdrCtx->stAuto.stMgeAuto.stMDCurveLM.stOfstMax = attr->stAuto.stMgeAuto.stMDCurveLM.stOfstMax;
    pAhdrCtx->stAuto.stMgeAuto.stMDCurveLM.stOfstMin = attr->stAuto.stMgeAuto.stMDCurveLM.stOfstMin;
    pAhdrCtx->stAuto.stMgeAuto.stMDCurveLM.stSmthMax = attr->stAuto.stMgeAuto.stMDCurveLM.stSmthMax;
    pAhdrCtx->stAuto.stMgeAuto.stMDCurveLM.stSmthMin = attr->stAuto.stMgeAuto.stMDCurveLM.stSmthMin;

    pAhdrCtx->stAuto.stMgeAuto.stMDCurveMS.stCoef = attr->stAuto.stMgeAuto.stMDCurveMS.stCoef;
    pAhdrCtx->stAuto.stMgeAuto.stMDCurveMS.stCoefMax = attr->stAuto.stMgeAuto.stMDCurveMS.stCoefMax;
    pAhdrCtx->stAuto.stMgeAuto.stMDCurveMS.stCoefMin = attr->stAuto.stMgeAuto.stMDCurveMS.stCoefMin;
    pAhdrCtx->stAuto.stMgeAuto.stMDCurveMS.stOfstMax = attr->stAuto.stMgeAuto.stMDCurveMS.stOfstMax;
    pAhdrCtx->stAuto.stMgeAuto.stMDCurveMS.stOfstMin = attr->stAuto.stMgeAuto.stMDCurveMS.stOfstMin;
    pAhdrCtx->stAuto.stMgeAuto.stMDCurveMS.stSmthMax = attr->stAuto.stMgeAuto.stMDCurveMS.stSmthMax;
    pAhdrCtx->stAuto.stMgeAuto.stMDCurveMS.stSmthMin = attr->stAuto.stMgeAuto.stMDCurveMS.stSmthMin;

    pAhdrCtx->stAuto.stMgeAuto.stOECurve.stCoef = attr->stAuto.stMgeAuto.stOECurve.stCoef;
    pAhdrCtx->stAuto.stMgeAuto.stOECurve.stCoefMax = attr->stAuto.stMgeAuto.stOECurve.stCoefMax;
    pAhdrCtx->stAuto.stMgeAuto.stOECurve.stCoefMin = attr->stAuto.stMgeAuto.stOECurve.stCoefMin;
    pAhdrCtx->stAuto.stMgeAuto.stOECurve.stOfstMax = attr->stAuto.stMgeAuto.stOECurve.stOfstMax;
    pAhdrCtx->stAuto.stMgeAuto.stOECurve.stOfstMin = attr->stAuto.stMgeAuto.stOECurve.stOfstMin;
    pAhdrCtx->stAuto.stMgeAuto.stOECurve.stSmthMax = attr->stAuto.stMgeAuto.stOECurve.stSmthMax;
    pAhdrCtx->stAuto.stMgeAuto.stOECurve.stSmthMin = attr->stAuto.stMgeAuto.stOECurve.stSmthMin;

    pAhdrCtx->stAuto.stTmoAuto.stGlobeMaxLuma.stCoef = attr->stAuto.stTmoAuto.stGlobeMaxLuma.stCoef;
    pAhdrCtx->stAuto.stTmoAuto.stGlobeMaxLuma.stCoefMax = attr->stAuto.stTmoAuto.stGlobeMaxLuma.stCoefMax;
    pAhdrCtx->stAuto.stTmoAuto.stGlobeMaxLuma.stCoefMin = attr->stAuto.stTmoAuto.stGlobeMaxLuma.stCoefMin;
    pAhdrCtx->stAuto.stTmoAuto.stGlobeMaxLuma.stMax = attr->stAuto.stTmoAuto.stGlobeMaxLuma.stMax;
    pAhdrCtx->stAuto.stTmoAuto.stGlobeMaxLuma.stMin = attr->stAuto.stTmoAuto.stGlobeMaxLuma.stMin;

    pAhdrCtx->stAuto.stTmoAuto.stGlobeLuma.stCoef = attr->stAuto.stTmoAuto.stGlobeLuma.stCoef;
    pAhdrCtx->stAuto.stTmoAuto.stGlobeLuma.stCoefMax = attr->stAuto.stTmoAuto.stGlobeLuma.stCoefMax;
    pAhdrCtx->stAuto.stTmoAuto.stGlobeLuma.stCoefMin = attr->stAuto.stTmoAuto.stGlobeLuma.stCoefMin;
    pAhdrCtx->stAuto.stTmoAuto.stGlobeLuma.stMax = attr->stAuto.stTmoAuto.stGlobeLuma.stMax;
    pAhdrCtx->stAuto.stTmoAuto.stGlobeLuma.stMin = attr->stAuto.stTmoAuto.stGlobeLuma.stMin;

    pAhdrCtx->stAuto.stTmoAuto.stDtlsLL.stCoef = attr->stAuto.stTmoAuto.stDtlsLL.stCoef;
    pAhdrCtx->stAuto.stTmoAuto.stDtlsLL.stCoefMax = attr->stAuto.stTmoAuto.stDtlsLL.stCoefMax;
    pAhdrCtx->stAuto.stTmoAuto.stDtlsLL.stCoefMin = attr->stAuto.stTmoAuto.stDtlsLL.stCoefMin;
    pAhdrCtx->stAuto.stTmoAuto.stDtlsLL.stMax = attr->stAuto.stTmoAuto.stDtlsLL.stMax;
    pAhdrCtx->stAuto.stTmoAuto.stDtlsLL.stMin = attr->stAuto.stTmoAuto.stDtlsLL.stMin;

    pAhdrCtx->stAuto.stTmoAuto.stDtlsHL.stCoef = attr->stAuto.stTmoAuto.stDtlsHL.stCoef;
    pAhdrCtx->stAuto.stTmoAuto.stDtlsHL.stCoefMax = attr->stAuto.stTmoAuto.stDtlsHL.stCoefMax;
    pAhdrCtx->stAuto.stTmoAuto.stDtlsHL.stCoefMin = attr->stAuto.stTmoAuto.stDtlsHL.stCoefMin;
    pAhdrCtx->stAuto.stTmoAuto.stDtlsHL.stMax = attr->stAuto.stTmoAuto.stDtlsHL.stMax;
    pAhdrCtx->stAuto.stTmoAuto.stDtlsHL.stMin = attr->stAuto.stTmoAuto.stDtlsHL.stMin;

    pAhdrCtx->stAuto.stTmoAuto.stNosLL.stCoef = attr->stAuto.stTmoAuto.stNosLL.stCoef;
    pAhdrCtx->stAuto.stTmoAuto.stNosLL.stCoefMax = attr->stAuto.stTmoAuto.stNosLL.stCoefMax;
    pAhdrCtx->stAuto.stTmoAuto.stNosLL.stCoefMin = attr->stAuto.stTmoAuto.stNosLL.stCoefMin;
    pAhdrCtx->stAuto.stTmoAuto.stNosLL.stMax = attr->stAuto.stTmoAuto.stNosLL.stMax;
    pAhdrCtx->stAuto.stTmoAuto.stNosLL.stMin = attr->stAuto.stTmoAuto.stNosLL.stMin;

    pAhdrCtx->stAuto.stTmoAuto.enSmthCtrl = attr->stAuto.stTmoAuto.enSmthCtrl;

    pAhdrCtx->stAuto.stTmoAuto.stSmthCtrlCoef1.stCoef = attr->stAuto.stTmoAuto.stSmthCtrlCoef1.stCoef;
    pAhdrCtx->stAuto.stTmoAuto.stSmthCtrlCoef1.stCoefMax = attr->stAuto.stTmoAuto.stSmthCtrlCoef1.stCoefMax;
    pAhdrCtx->stAuto.stTmoAuto.stSmthCtrlCoef1.stCoefMin = attr->stAuto.stTmoAuto.stSmthCtrlCoef1.stCoefMin;
    pAhdrCtx->stAuto.stTmoAuto.stSmthCtrlCoef1.stMax = attr->stAuto.stTmoAuto.stSmthCtrlCoef1.stMax;
    pAhdrCtx->stAuto.stTmoAuto.stSmthCtrlCoef1.stMin = attr->stAuto.stTmoAuto.stSmthCtrlCoef1.stMin;

    pAhdrCtx->stAuto.stTmoAuto.stSmthCtrlCoef2.stCoef = attr->stAuto.stTmoAuto.stSmthCtrlCoef2.stCoef;
    pAhdrCtx->stAuto.stTmoAuto.stSmthCtrlCoef2.stCoefMax = attr->stAuto.stTmoAuto.stSmthCtrlCoef2.stCoefMax;
    pAhdrCtx->stAuto.stTmoAuto.stSmthCtrlCoef2.stCoefMin = attr->stAuto.stTmoAuto.stSmthCtrlCoef2.stCoefMin;
    pAhdrCtx->stAuto.stTmoAuto.stSmthCtrlCoef2.stMax = attr->stAuto.stTmoAuto.stSmthCtrlCoef2.stMax;
    pAhdrCtx->stAuto.stTmoAuto.stSmthCtrlCoef2.stMin = attr->stAuto.stTmoAuto.stSmthCtrlCoef2.stMin;

    pAhdrCtx->stManual.stMgeManual.stMDCurveLM_ofst = attr->stManual.stMgeManual.stMDCurveLM_ofst;
    pAhdrCtx->stManual.stMgeManual.stMDCurveLM_smth = attr->stManual.stMgeManual.stMDCurveLM_smth;
    pAhdrCtx->stManual.stMgeManual.stMDCurveMS_ofst = attr->stManual.stMgeManual.stMDCurveMS_ofst;
    pAhdrCtx->stManual.stMgeManual.stMDCurveMS_smth = attr->stManual.stMgeManual.stMDCurveMS_smth;
    pAhdrCtx->stManual.stMgeManual.stOECurve_smth = attr->stManual.stMgeManual.stOECurve_smth;
    pAhdrCtx->stManual.stMgeManual.stOECurve_ofst = attr->stManual.stMgeManual.stOECurve_ofst;

    pAhdrCtx->stManual.stTmoManual.stGlobeMaxLuma = attr->stManual.stTmoManual.stGlobeMaxLuma;
    pAhdrCtx->stManual.stTmoManual.stGlobeLuma = attr->stManual.stTmoManual.stGlobeLuma;
    pAhdrCtx->stManual.stTmoManual.stDtlsLL = attr->stManual.stTmoManual.stDtlsLL;
    pAhdrCtx->stManual.stTmoManual.stDtlsHL = attr->stManual.stTmoManual.stDtlsHL;
    pAhdrCtx->stManual.stTmoManual.stNosLL = attr->stManual.stTmoManual.stNosLL;
    pAhdrCtx->stManual.stTmoManual.enSmthCtrl = attr->stManual.stTmoManual.enSmthCtrl;
    pAhdrCtx->stManual.stTmoManual.stSmthCtrlCoef1 = attr->stManual.stTmoManual.stSmthCtrlCoef1;
    pAhdrCtx->stManual.stTmoManual.stSmthCtrlCoef2 = attr->stManual.stTmoManual.stSmthCtrlCoef2;


    return XCAM_RETURN_NO_ERROR;
}

