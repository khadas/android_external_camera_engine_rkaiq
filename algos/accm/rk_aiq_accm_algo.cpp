/*
* rk_aiq_accm_algo.cpp

* for rockchip v2.0.0
*
*  Copyright (c) 2019 Rockchip Corporation
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/
/* for rockchip v2.0.0*/

#include "accm/rk_aiq_accm_algo.h"
#include "xcam_log.h"
#include "interpolation.h"
RKAIQ_BEGIN_DECLARE

static XCamReturn illuminant_index_estimation(int light_num, const CalibDb_AccmCof_ill_t illAll[], float awbGain[2], int* illuminant_index)
{

    LOGI_ACCM( "%s: (enter)\n", __FUNCTION__);
    float minDist = 9999999;
    float dist[8];
    float nRG, nBG;
    *illuminant_index = 0;
    XCamReturn ret = XCAM_RETURN_ERROR_FAILED;
    for(int i = 0; i < light_num; i++)
    {
        nRG = awbGain[0];
        nBG = awbGain[1];
        dist[i] = sqrt((nRG - illAll[i].awbGain[0]) * (nRG -  illAll[i].awbGain[0])
                       + (nBG -  illAll[i].awbGain[1]) * (nBG -  illAll[i].awbGain[1]));
        if(dist[i] < minDist)
        {
            minDist = dist[i];
            *illuminant_index = i;
            ret = XCAM_RETURN_NO_ERROR;
        }
    }
    if(ret != XCAM_RETURN_NO_ERROR){
      LOGE_ACCM("fail to estimate illuminant!!!\n");
    }

    LOGD_ACCM( "wbGain:%f,%f, estimation illuminant  is %s(%d) \n",awbGain[0],awbGain[1],
               illAll[*illuminant_index].illuName, *illuminant_index);

    LOGI_ACCM( "%s: (exit)\n", __FUNCTION__);
    return ret;
}


static XCamReturn AwbOrderCcmProfilesBySaturation
(
    const CalibDb_CcmMatrixProfile_t* pCcmProfiles[],
    const int32_t   cnt
) {
    int32_t i, j;

    for (i = 0; i < (cnt - 1); ++i) {
        for (j = 0; j < (cnt - i - 1); ++j) {
            if (pCcmProfiles[j]->saturation< pCcmProfiles[j + 1]->saturation) {
                const CalibDb_CcmMatrixProfile_t* temp   = pCcmProfiles[j];
                pCcmProfiles[j]         = pCcmProfiles[j + 1];
                pCcmProfiles[j + 1]       = temp;
            }
        }
    }

    return (XCAM_RETURN_NO_ERROR);
}

static XCamReturn SatSelectCcmProfiles
(
    const float     fSaturation,
    int32_t         no_ccm,
    const CalibDb_CcmMatrixProfile_t* pCcmProfiles[],
    const CalibDb_CcmMatrixProfile_t** pCcmProfile1,
    const CalibDb_CcmMatrixProfile_t** pCcmProfile2
) {
    XCamReturn XCamReturn = XCAM_RETURN_NO_ERROR;

    if ((no_ccm == 0) || (pCcmProfiles == NULL)
            || (pCcmProfile1 == NULL) || (pCcmProfile2 == NULL)) {
        return (XCAM_RETURN_ERROR_PARAM);
    }

    if (fSaturation >= pCcmProfiles[0]->saturation) {
        *pCcmProfile1 = pCcmProfiles[0];
        *pCcmProfile2 = NULL;
        LOGD_ACCM( "select:%s \n", (*pCcmProfile1)->name);
        XCamReturn = XCAM_RETURN_ERROR_OUTOFRANGE;
    } else {
        int32_t nLast = no_ccm - 1;
        if (fSaturation <= pCcmProfiles[nLast]->saturation) {
            *pCcmProfile1 = pCcmProfiles[nLast];
            *pCcmProfile2 = NULL;
            LOGD_ACCM( "select:%s \n", (*pCcmProfile1)->name);
            XCamReturn = XCAM_RETURN_ERROR_OUTOFRANGE;
        } else {
            uint16_t n = 0;

            /* find the segment */
            while ((fSaturation <= pCcmProfiles[n]->saturation) && (n <= nLast)) {
                n++;
            }
            n--;

            *pCcmProfile1 = pCcmProfiles[n];
            *pCcmProfile2 = pCcmProfiles[n + 1];

        }
    }

    return (XCamReturn);
}



/******************************************************************************
 * InterpolateMatrices
 *****************************************************************************/
static XCamReturn SatInterpolateMatrices
(
    const float             fSat,
    const CalibDb_CcmMatrixProfile_t*   pCcProfileA,
    const CalibDb_CcmMatrixProfile_t*   pCcProfileB,
    Cam3x3FloatMatrix_t*          pResMatrix
) {
    XCamReturn iXCamReturn = XCAM_RETURN_ERROR_PARAM;

    if ((pCcProfileA != NULL) && (pCcProfileA != NULL) && (pResMatrix != NULL)) {
        const Cam3x3FloatMatrix_t *pMatrixA = &pCcProfileA->CrossTalkCoeff;
        const Cam3x3FloatMatrix_t *pMatrixB = &pCcProfileB->CrossTalkCoeff;

        float fSatA = pCcProfileA->saturation;
        float fSatB = pCcProfileB->saturation;

        float f1 = ( fSatB - fSat ) / ( fSatB - fSatA ); // test: if fSat == fSatA => f1 = 1 => choose A: ok
        float f2 = 1.0f - f1;

        int i;

        for ( i=0; i<9; i++)
        {
            pResMatrix->fCoeff[i] = f1 * pMatrixA->fCoeff[i] + f2 * pMatrixB->fCoeff[i];
        }

        iXCamReturn = XCAM_RETURN_NO_ERROR;

    }

    return (iXCamReturn);
}

/******************************************************************************
 * InterpolateMatrices
 *****************************************************************************/
static XCamReturn SatInterpolateOffset
(
    const float             fSat,
    const CalibDb_CcmMatrixProfile_t*   pCcProfileA,
    const CalibDb_CcmMatrixProfile_t*   pCcProfileB,
    Cam1x3FloatMatrix_t     *pResOffset
) {
    XCamReturn result = XCAM_RETURN_ERROR_PARAM;

    if ( (pCcProfileA != NULL) && (pCcProfileB != NULL) && (pResOffset != NULL) )
    {
        const Cam1x3FloatMatrix_t *pOffsetA = &pCcProfileA->CrossTalkOffset;
        const Cam1x3FloatMatrix_t *pOffsetB = &pCcProfileB->CrossTalkOffset;

        float fSatA = pCcProfileA->saturation;
        float fSatB = pCcProfileB->saturation;

        float f1 = ( fSatB - fSat ) / ( fSatB - fSatA ); // test: if fSat == fSatA => f1 = 1 => choose A: ok
        float f2 = 1.0f - f1;

        pResOffset->fCoeff[CAM_3CH_COLOR_COMPONENT_RED]
                = f1 * pOffsetA->fCoeff[CAM_3CH_COLOR_COMPONENT_RED] + f2 * pOffsetB->fCoeff[CAM_3CH_COLOR_COMPONENT_RED];
        pResOffset->fCoeff[CAM_3CH_COLOR_COMPONENT_GREEN]
                = f1 * pOffsetA->fCoeff[CAM_3CH_COLOR_COMPONENT_GREEN] + f2 * pOffsetB->fCoeff[CAM_3CH_COLOR_COMPONENT_GREEN];
        pResOffset->fCoeff[CAM_3CH_COLOR_COMPONENT_BLUE]
                = f1 * pOffsetA->fCoeff[CAM_3CH_COLOR_COMPONENT_BLUE] + f2 * pOffsetB->fCoeff[CAM_3CH_COLOR_COMPONENT_BLUE];

        result = XCAM_RETURN_NO_ERROR;
    }

    return ( result );

}

/******************************************************************************
 * Damping
 *****************************************************************************/
static XCamReturn Damping
(
    const float         damp,                /**< damping coefficient */
    Cam3x3FloatMatrix_t *pMatrixUndamped,   /**< undamped new computed matrices */
    Cam3x3FloatMatrix_t *pMatrixDamped,     /**< old matrices and result */
    Cam1x3FloatMatrix_t *pOffsetUndamped,   /**< undamped new computed offsets */
    Cam1x3FloatMatrix_t *pOffsetDamped      /**< old offset and result */
)
{
    XCamReturn result = XCAM_RETURN_ERROR_PARAM;

    if ( (pMatrixUndamped != NULL) && (pMatrixDamped != NULL)
            && (pOffsetUndamped != NULL) && (pOffsetDamped != NULL) )
    {
        int32_t i;
        float f = (1.0f - damp);

        /* calc. damped cc matrix */
        for( i=0; i<9; i++ )
        {
            pMatrixDamped->fCoeff[i] = (damp * pMatrixDamped->fCoeff[i]) + (f *  pMatrixUndamped->fCoeff[i]);
        }

        /* calc. damped cc offsets */
        pOffsetDamped->fCoeff[CAM_3CH_COLOR_COMPONENT_RED]
                        = (damp * pOffsetDamped->fCoeff[CAM_3CH_COLOR_COMPONENT_RED])
                        + (f * pOffsetUndamped->fCoeff[CAM_3CH_COLOR_COMPONENT_RED]);
        pOffsetDamped->fCoeff[CAM_3CH_COLOR_COMPONENT_GREEN]
                        = (damp * pOffsetDamped->fCoeff[CAM_3CH_COLOR_COMPONENT_GREEN])
                        + (f * pOffsetUndamped->fCoeff[CAM_3CH_COLOR_COMPONENT_GREEN]);
        pOffsetDamped->fCoeff[CAM_3CH_COLOR_COMPONENT_BLUE]
                        = (damp * pOffsetDamped->fCoeff[CAM_3CH_COLOR_COMPONENT_BLUE])
                        + (f * pOffsetUndamped->fCoeff[CAM_3CH_COLOR_COMPONENT_BLUE]);

        result = XCAM_RETURN_NO_ERROR;
    }
    LOGD_ACCM( "dampfactor:%f",damp);
    LOGD_ACCM( " undampedCcmMatrix: %f,%f,%f,%f,%f,%f,%f,%f,%f  undampedCcOffset:%f,%f,%f  \n",
        pMatrixUndamped->fCoeff[0],
        pMatrixUndamped->fCoeff[1],
        pMatrixUndamped->fCoeff[2],
        pMatrixUndamped->fCoeff[3],
        pMatrixUndamped->fCoeff[4],
        pMatrixUndamped->fCoeff[5],
        pMatrixUndamped->fCoeff[6],
        pMatrixUndamped->fCoeff[7],
        pMatrixUndamped->fCoeff[8],
        pOffsetUndamped->fCoeff[0],
        pOffsetUndamped->fCoeff[1],
        pOffsetUndamped->fCoeff[2]
        );
    LOGD_ACCM( " dampedCcmMatrix: %f,%f,%f,%f,%f,%f,%f,%f,%f  dampedCcOffset:%f,%f,%f  \n",
        pMatrixDamped->fCoeff[0],
        pMatrixDamped->fCoeff[1],
        pMatrixDamped->fCoeff[2],
        pMatrixDamped->fCoeff[3],
        pMatrixDamped->fCoeff[4],
        pMatrixDamped->fCoeff[5],
        pMatrixDamped->fCoeff[6],
        pMatrixDamped->fCoeff[7],
        pMatrixDamped->fCoeff[8],
        pOffsetDamped->fCoeff[0],
        pOffsetDamped->fCoeff[1],
        pOffsetDamped->fCoeff[2]
        );


    return ( result );
}



static XCamReturn CamCalibDbGetCcmProfileByName(const CalibDb_Ccm_t *calibCcm, char* name, const CalibDb_CcmMatrixProfile_t **pCcmMatrixProfile)
{
    LOGI_ACCM("%s: (enter)\n", __FUNCTION__);

    XCamReturn ret = XCAM_RETURN_ERROR_FAILED;

    for(int i=0;i<calibCcm->matrixAllNum;i++){
        if(strcmp(calibCcm->matrixAll[i].name,name)==0){
            *pCcmMatrixProfile = &calibCcm->matrixAll[i];
            ret = XCAM_RETURN_NO_ERROR;
            break;
        }
    }
    if(ret!= XCAM_RETURN_NO_ERROR){
       LOGE_ACCM("can't find %s in matrixAll\n",name);
   }
    LOGI_ACCM("%s: (exit)\n", __FUNCTION__);

    return ret;
}



XCamReturn AccmAutoConfig
(
    accm_handle_t hAccm
) {

    LOGI_ACCM("%s: (enter) count:%d\n", __FUNCTION__, hAccm->count);

    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    const CalibDb_AccmCof_ill_t* pDomIlluProfile = NULL;
    const CalibDb_CcmMatrixProfile_t* pCcmProfile1 = NULL;
    const CalibDb_CcmMatrixProfile_t* pCcmProfile2 = NULL;

    if (hAccm == NULL) {
        return XCAM_RETURN_ERROR_PARAM;
    }

    int dominateIlluProfileIdx;
    int resIdx;
    //1)
    ret = illuminant_index_estimation(hAccm->calibCcm->aCcmCof.illuNum, hAccm->calibCcm->aCcmCof.illAll,
                                      hAccm->accmSwInfo.awbGain, &dominateIlluProfileIdx);
    RETURN_RESULT_IF_DIFFERENT(ret, XCAM_RETURN_NO_ERROR);
    hAccm->accmRest.dominateIlluProfileIdx = dominateIlluProfileIdx;

    // 2)
    float sensorGain =  hAccm->accmSwInfo.sensorGain;
    float fSaturation;
    pDomIlluProfile = &hAccm->calibCcm->aCcmCof.illAll[dominateIlluProfileIdx];

    interpolation(pDomIlluProfile->saturationCurve.pSensorGain,
                  pDomIlluProfile->saturationCurve.pSaturation,
                  pDomIlluProfile->saturationCurve.arraySize,
                  sensorGain, &fSaturation);
    hAccm->accmRest.fSaturation =  fSaturation;
    //3)
    ret = SatSelectCcmProfiles(fSaturation, pDomIlluProfile->matrixUsedNO, hAccm->pCcmMatrixAll[dominateIlluProfileIdx],
                                &pCcmProfile1, &pCcmProfile2);
    if (ret == XCAM_RETURN_NO_ERROR) {
        if (pCcmProfile1 && pCcmProfile2)
            LOGD_ACCM("fSaturation: %f (%f .. %f)\n",  fSaturation, pCcmProfile1->saturation, pCcmProfile2->saturation);
        ret = SatInterpolateMatrices(fSaturation, pCcmProfile1, pCcmProfile2,
                                      &hAccm->accmRest.undampedCcmMatrix);

        ret = SatInterpolateOffset(fSaturation, pCcmProfile1, pCcmProfile2,
                                      &hAccm->accmRest.undampedCcOffset);
        if (ret != XCAM_RETURN_NO_ERROR) {
            return (ret);
        }
    } else if (ret == XCAM_RETURN_ERROR_OUTOFRANGE) {
        /* we don't need to interpolate */
        LOGD_ACCM("fSaturation: %f (%f)\n",  fSaturation, pCcmProfile1->saturation);
        memcpy(&hAccm->accmRest.undampedCcmMatrix, &pCcmProfile1->CrossTalkCoeff, sizeof(Cam3x3FloatMatrix_t));
        memcpy(&hAccm->accmRest.undampedCcOffset, &pCcmProfile1->CrossTalkOffset, sizeof(Cam1x3FloatMatrix_t));
    } else {
        return (ret);
    }
    // 5) . Damping
    ret = Damping((hAccm->calibCcm->damp_enable && hAccm->count>1) ? hAccm->accmSwInfo.awbIIRDampCoef : 0,
                  &hAccm->accmRest.undampedCcmMatrix, &hAccm->accmRest.dampedCcmMatrix,
                  &hAccm->accmRest.undampedCcOffset, &hAccm->accmRest.dampedCcOffset);

    hAccm->accmRest.pCcmProfile1 = pCcmProfile1;
    hAccm->accmRest.pCcmProfile2 = pCcmProfile2;
    //6 calc scale for y_alpha_curve
    float fScale = 1.0;
#if 0
    //real use
    interpolation(hAccm->calibCcm->luma_ccm.alpha_gain,
                  hAccm->calibCcm->luma_ccm.alpha_scale,
                  hAccm->calibCcm->luma_ccm.gain_scale_cure_size,
                  sensorGain, &fScale);
#else
    //for test, to be same with demo
    for( int i = 0; i < 9; i++)
    {
        int j = uint16_t(sensorGain);
        j = (j > (1<<8)) ? (1<<8) : j;

        if(j <= (1<<i))
        {
            fScale = hAccm->calibCcm->luma_ccm.alpha_scale[i];
            break;
        }
    }
#endif

   // 7 set to ic  to do bit check

    //hAccm->ccmHwConf.ccmEnable = hAccm->calibCcm->enable;
    memcpy(hAccm->ccmHwConf.matrix,&hAccm->accmRest.dampedCcmMatrix,sizeof(Cam3x3FloatMatrix_t));
    memcpy(hAccm->ccmHwConf.offs,&hAccm->accmRest.dampedCcOffset,sizeof(Cam1x3FloatMatrix_t));
    for(int i=0; i<CCM_CURVE_DOT_NUM;i++){
        hAccm->ccmHwConf.alp_y[i] = fScale * hAccm->calibCcm->luma_ccm.y_alpha_curve[i];
    }


    LOGI_ACCM("%s: (exit)\n", __FUNCTION__);

    return (ret);
}

XCamReturn AccmManualConfig
(
    accm_handle_t hAccm
){
    LOGI_ACCM("%s: (enter)\n", __FUNCTION__);

    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    memcpy(hAccm->ccmHwConf.matrix,hAccm->mCurAtt.stManual.matrix,sizeof(hAccm->mCurAtt.stManual.matrix));
    memcpy(hAccm->ccmHwConf.offs,hAccm->mCurAtt.stManual.offs,sizeof(hAccm->mCurAtt.stManual.offs));
    memcpy(hAccm->ccmHwConf.alp_y,hAccm->mCurAtt.stManual.alp_y,sizeof(hAccm->mCurAtt.stManual.alp_y));

    LOGI_ACCM("%s: (exit)\n", __FUNCTION__);
    return ret;

}

XCamReturn AccmConfig
(
    accm_handle_t hAccm
){
    LOGI_ACCM("%s: (enter)\n", __FUNCTION__);

    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    LOGD_ACCM("%s: updateAtt: %d\n", __FUNCTION__,hAccm->updateAtt);
    if(hAccm->updateAtt){
        hAccm->mCurAtt = hAccm->mNewAtt;
    }
    LOGD_ACCM("%s: byPass: %d  mode:%d \n", __FUNCTION__,hAccm->mCurAtt.byPass,hAccm->mCurAtt.mode);
    if(hAccm->mCurAtt.byPass != true){
         hAccm->ccmHwConf.ccmEnable = true;
        if(hAccm->mCurAtt.mode == RK_AIQ_CCM_MODE_AUTO){
            AccmAutoConfig(hAccm);
        }else if(hAccm->mCurAtt.mode == RK_AIQ_CCM_MODE_MANUAL){
            AccmManualConfig(hAccm);
        }else{
            LOGE_ACCM("%s: hAccm->mCurAtt.mode(%d) is invalid \n", __FUNCTION__,hAccm->mCurAtt.mode);
        }
        memcpy(hAccm->mCurAtt.stManual.matrix,hAccm->ccmHwConf.matrix,sizeof(hAccm->ccmHwConf.matrix));
        memcpy(hAccm->mCurAtt.stManual.offs,hAccm->ccmHwConf.offs,sizeof(hAccm->ccmHwConf.offs));
        memcpy(hAccm->mCurAtt.stManual.alp_y,hAccm->ccmHwConf.alp_y,sizeof(hAccm->ccmHwConf.alp_y));

    }else{
        hAccm->ccmHwConf.ccmEnable = false;
    }

    LOGD_ACCM( " set to ic ccmEnable :%d  bound_bit:%f\n",hAccm->ccmHwConf.ccmEnable,hAccm->ccmHwConf.bound_bit);

    LOGD_ACCM( " set to ic CcmMatrix: %f,%f,%f,%f,%f,%f,%f,%f,%f  CcOffset:%f,%f,%f  \n",
         hAccm->ccmHwConf.matrix[0],
         hAccm->ccmHwConf.matrix[1],
         hAccm->ccmHwConf.matrix[2],
         hAccm->ccmHwConf.matrix[3],
         hAccm->ccmHwConf.matrix[4],
         hAccm->ccmHwConf.matrix[5],
         hAccm->ccmHwConf.matrix[6],
         hAccm->ccmHwConf.matrix[7],
         hAccm->ccmHwConf.matrix[8],
         hAccm->ccmHwConf.offs[0],
         hAccm->ccmHwConf.offs[1],
         hAccm->ccmHwConf.offs[2]
    );

    LOGD_ACCM( " set to ic rgb2y_para: %f,%f,%f\n",
          hAccm->ccmHwConf.rgb2y_para[0],
          hAccm->ccmHwConf.rgb2y_para[1],
          hAccm->ccmHwConf.rgb2y_para[2]

     );

    LOGD_ACCM( " set to ic alp_y: %f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f  \n",
          hAccm->ccmHwConf.alp_y[0],
          hAccm->ccmHwConf.alp_y[1],
          hAccm->ccmHwConf.alp_y[2],
          hAccm->ccmHwConf.alp_y[3],
          hAccm->ccmHwConf.alp_y[4],
          hAccm->ccmHwConf.alp_y[5],
          hAccm->ccmHwConf.alp_y[6],
          hAccm->ccmHwConf.alp_y[7],
          hAccm->ccmHwConf.alp_y[8],
          hAccm->ccmHwConf.alp_y[9],
          hAccm->ccmHwConf.alp_y[10],
          hAccm->ccmHwConf.alp_y[11],
          hAccm->ccmHwConf.alp_y[12],
          hAccm->ccmHwConf.alp_y[13],
          hAccm->ccmHwConf.alp_y[14],
          hAccm->ccmHwConf.alp_y[15],
          hAccm->ccmHwConf.alp_y[16]
     );


     hAccm->count = ((hAccm->count+2) >(65536))? 2 : (hAccm->count+1);

    LOGI_ACCM("%s: (exit)\n", __FUNCTION__);
    return ret;

}


XCamReturn AccmInit(accm_handle_t *hAccm,const CamCalibDbContext_t* calib)
{
    LOGI_ACCM("%s: (enter)\n", __FUNCTION__);

    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    *hAccm = (accm_context_t*)malloc(sizeof(accm_context_t));
    accm_context_t* accm_context = *hAccm;
    memset(accm_context, 0 ,sizeof(accm_context_t));

    if(calib == NULL) {
        return XCAM_RETURN_ERROR_FAILED;
    }
    const CalibDb_Ccm_t *calib_ccm = &calib->ccm;
    accm_context->calibCcm= calib_ccm;
    accm_context->accmSwInfo.sensorGain=1.0;
    accm_context->accmSwInfo.awbIIRDampCoef = 0;
    bool lsFound;
    for(int i = 0; i < calib_ccm->aCcmCof.illuNum; i++) {
        if(strcmp(calib_ccm->aCcmCof.illAll[i].illuName,calib->awb.stategy_cfg.lsForFirstFrame)==0){
            memcpy(accm_context->accmSwInfo.awbGain, calib_ccm->aCcmCof.illAll[i].awbGain,
                sizeof(accm_context->accmSwInfo.awbGain));
            lsFound = true;
            LOGD_ACCM("%s: accm lsForFirstFrame:%s", __FUNCTION__, calib_ccm->aCcmCof.illAll[i].illuName);
            break;
        }
    }
    if(calib_ccm->aCcmCof.illuNum>0 && lsFound == false){
        memcpy(accm_context->accmSwInfo.awbGain, calib_ccm->aCcmCof.illAll[0].awbGain,
        sizeof(accm_context->accmSwInfo.awbGain));
        LOGD_ACCM("%s: accm lsForFirstFrame:%s", __FUNCTION__, calib_ccm->aCcmCof.illAll[0].illuName);
    }

    LOGI_ACCM("%s: accm illunum:%d", __FUNCTION__, calib_ccm->aCcmCof.illuNum);
    // 1) gtet and reorder para
    for(int i = 0; i < calib_ccm->aCcmCof.illuNum; i++) {
            for (int j = 0; j < calib_ccm->aCcmCof.illAll[i].matrixUsedNO; j++) {
                char name[CCM_PROFILE_NAME];
                sprintf(name,"%s", calib_ccm->aCcmCof.illAll[i].matrixUsed[j]);
                const CalibDb_CcmMatrixProfile_t* pCcmMatrixProfile = NULL;
                // get a ccm-profile from database
                ret = CamCalibDbGetCcmProfileByName(calib_ccm, name, &pCcmMatrixProfile);
                RETURN_RESULT_IF_DIFFERENT(ret, XCAM_RETURN_NO_ERROR);
                // store ccm-profile in pointer array
                accm_context->pCcmMatrixAll[i][j] = pCcmMatrixProfile;
                LOGD_ACCM("CCM name  %s coef:%f,%f,%f  %f,%f,%f  %f,%f,%f  \n",name,
                    accm_context->pCcmMatrixAll[i][j]->CrossTalkCoeff.fCoeff[0],
                    accm_context->pCcmMatrixAll[i][j]->CrossTalkCoeff.fCoeff[1],
                    accm_context->pCcmMatrixAll[i][j]->CrossTalkCoeff.fCoeff[2],
                    accm_context->pCcmMatrixAll[i][j]->CrossTalkCoeff.fCoeff[3],
                    accm_context->pCcmMatrixAll[i][j]->CrossTalkCoeff.fCoeff[4],
                    accm_context->pCcmMatrixAll[i][j]->CrossTalkCoeff.fCoeff[5],
                    accm_context->pCcmMatrixAll[i][j]->CrossTalkCoeff.fCoeff[6],
                    accm_context->pCcmMatrixAll[i][j]->CrossTalkCoeff.fCoeff[7],
                    accm_context->pCcmMatrixAll[i][j]->CrossTalkCoeff.fCoeff[8]);
                LOGD_ACCM("CCM name  %s off:%f,%f,%f  \n",name,
                    accm_context->pCcmMatrixAll[i][j]->CrossTalkOffset.fCoeff[0],
                    accm_context->pCcmMatrixAll[i][j]->CrossTalkOffset.fCoeff[1],
                    accm_context->pCcmMatrixAll[i][j]->CrossTalkOffset.fCoeff[2]);

            }
            // order ccm-profiles by saturation
            ret = AwbOrderCcmProfilesBySaturation(accm_context->pCcmMatrixAll[i],
            calib_ccm->aCcmCof.illAll[i].matrixUsedNO);
            //RETURN_RESULT_IF_DIFFERENT(result, XCAM_RETURN_NO_ERROR);

    }
    accm_context->count = 0;

    accm_context->ccmHwConf.bound_bit = accm_context->calibCcm->luma_ccm.low_bound_pos_bit;
    memcpy( accm_context->ccmHwConf.rgb2y_para,accm_context->calibCcm->luma_ccm.rgb2y_para,
        sizeof(accm_context->calibCcm->luma_ccm.rgb2y_para));

    accm_context->mCurAtt.byPass = !(calib_ccm->enable);
    accm_context->mCurAtt.mode = RK_AIQ_CCM_MODE_AUTO;

    LOGI_ACCM("%s: (exit)\n", __FUNCTION__);
    return(ret);


}

XCamReturn AccmRelease(accm_handle_t hAccm)
{
    LOGI_ACCM("%s: (enter)\n", __FUNCTION__);

    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    free(hAccm);

    LOGI_ACCM("%s: (exit)\n", __FUNCTION__);
    return(ret);

}

XCamReturn AccmPrepare(accm_handle_t hAccm)
{
    LOGI_ACCM("%s: (enter)\n", __FUNCTION__);

    XCamReturn ret = XCAM_RETURN_NO_ERROR;


    LOGI_ACCM("%s: (exit)\n", __FUNCTION__);
    return ret;
}
XCamReturn AccmPreProc(accm_handle_t hAccm)
{

    LOGI_ACCM("%s: (enter)\n", __FUNCTION__);

    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    LOGI_ACCM("%s: (exit)\n", __FUNCTION__);
    return(ret);

}
XCamReturn AccmProcessing(accm_handle_t hAccm)
{
    LOGI_ACCM("%s: (enter)\n", __FUNCTION__);

    XCamReturn ret = XCAM_RETURN_NO_ERROR;


    LOGI_ACCM("%s: (exit)\n", __FUNCTION__);
    return(ret);
}




RKAIQ_END_DECLARE


