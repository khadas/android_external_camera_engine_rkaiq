/*
 * rk_aiq_algo_orb_itf.c
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

#include "rk_aiq_algo_types_int.h"
#include "rk_aiq_algo_aorb_itf.h"

#if OPENCV_SUPPORT
#include <vector>
#include "opencv2/opencv.hpp"
#endif

RKAIQ_BEGIN_DECLARE

typedef struct ORBContext_s {
    unsigned char initialized;
    int orb_en;
    int limit_value;
    int max_feature;
    orb_stat_t orb_stats;
    double homographyMat[9];
} ORBContext_t;

typedef struct ORBContext_s* ORBHandle_t;
ORBContext_t gORBCtx;

typedef struct _RkAiqAlgoContext {
    ORBHandle_t hORB;
    void* place_holder[0];
} RkAiqAlgoContext;

static RkAiqAlgoContext ctx;

#define ORB_LIMIT_VALUE_DEF 50
#define ORB_MAX_FEATURE_DEF 200

#if OPENCV_SUPPORT
using namespace cv;
using namespace std;
#endif

XCamReturn ORBCreateContext(RkAiqAlgoContext **context, const AlgoCtxInstanceCfg* cfg)
{
    XCamReturn result = XCAM_RETURN_NO_ERROR;

    LOGI_ORB("%s: (enter)\n", __FUNCTION__ );

    /* setup config */
    memset( &gORBCtx, 0, sizeof(gORBCtx) );
    ctx.hORB = &gORBCtx;

    /* return handle */
    *context = &ctx;

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn ORBDestroyContext(RkAiqAlgoContext *context)
{
    XCamReturn result = XCAM_RETURN_NO_ERROR;
    LOGI_ORB("%s: (enter)\n", __FUNCTION__ );

    DCT_ASSERT( context != NULL );

    if ( NULL != context )
    {
        context = NULL;
    }

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn ORBPrepare(RkAiqAlgoCom* params)
{
    XCamReturn result = XCAM_RETURN_NO_ERROR;

    ORBHandle_t hORB = (ORBHandle_t)params->ctx->hORB;
    ORBContext_t* ORBctx = (ORBContext_t*)hORB;
    RkAiqAlgoConfigAorbInt* rkaiqAorbConfig = (RkAiqAlgoConfigAorbInt*)params;

    ORBctx->orb_en = rkaiqAorbConfig->orb_calib_cfg.orb_en;
    ORBctx->limit_value = ORB_LIMIT_VALUE_DEF;
    ORBctx->max_feature = ORB_MAX_FEATURE_DEF;

    LOGI_ORB("%s: (enter)\n", __FUNCTION__ );

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn ORBPreProcess(const RkAiqAlgoCom* inparams, RkAiqAlgoResCom* outparams)
{
    XCamReturn result = XCAM_RETURN_NO_ERROR;

    ORBHandle_t hORB = (ORBHandle_t)inparams->ctx->hORB;
    ORBContext_t* ORBctx = (ORBContext_t*)hORB;
    RkAiqAlgoPreAorbInt* rkaiqAorbPreParam = (RkAiqAlgoPreAorbInt*)inparams;
    RkAiqAlgoPreResAorbInt* rkaiqAorbPreOut = (RkAiqAlgoPreResAorbInt*)outparams;

#if OPENCV_SUPPORT
    Point2f queryPoints, trainPoints;
    vector<Point2f> m_queryPoints, m_trainPoints;
    double reprojectionThreshold = 10;

    m_queryPoints.clear();
    m_trainPoints.clear();
    vector<Point2f>(m_queryPoints).swap(m_queryPoints);
    vector<Point2f>(m_trainPoints).swap(m_trainPoints);
#endif

/*
    LOGI_ORB("%s: (%d)feature point num: %d-%d\n", __FUNCTION__,
        ORBctx->initialized,
        ORBctx->orb_stats.num_points,
        rkaiqAorbPreParam->orb_stats.num_points);
*/
    if (!ORBctx->initialized) {
        rkaiqAorbPreOut->orb_pre_result.valid = 0;
        ORBctx->initialized = 1;
    } else {
#if OPENCV_SUPPORT
        if (ORBctx->orb_stats.num_points > 0 &&
                rkaiqAorbPreParam->orb_stats.num_points > 0) {
            for (int i = 0; i < rkaiqAorbPreParam->orb_stats.num_points; i++) {
                queryPoints.x = rkaiqAorbPreParam->orb_stats.pXs[i];
                queryPoints.y = rkaiqAorbPreParam->orb_stats.pYs[i];

                trainPoints.x = ORBctx->orb_stats.pXs[i];
                trainPoints.y = ORBctx->orb_stats.pYs[i];

                m_queryPoints.push_back(queryPoints);
                m_trainPoints.push_back(trainPoints);
            }

            Mat homography = findHomography(m_queryPoints, m_trainPoints, RANSAC,
                                reprojectionThreshold, noArray(), 2000, 0.995);
            double* data = (double*)homography.data;
            for (int i = 0; i < 9; i++) {
                rkaiqAorbPreOut->orb_pre_result.homographyMat[i] = data[i];
                //LOGI_ORB("%s: feature point match ret: %f\n", __FUNCTION__, data[i]);
            }

            rkaiqAorbPreOut->orb_pre_result.valid = 1;
        }
#endif
    }

    ORBctx->orb_stats = rkaiqAorbPreParam->orb_stats;

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn ORBProcessing(const RkAiqAlgoCom* inparams, RkAiqAlgoResCom* outparams)
{
    XCamReturn result = XCAM_RETURN_NO_ERROR;

    ORBHandle_t hORB = (ORBHandle_t)inparams->ctx->hORB;
    ORBContext_t* ORBctx = (ORBContext_t*)hORB;
    RkAiqAlgoProcAorbInt* rkaiqAorbProcParam = (RkAiqAlgoProcAorbInt*)inparams;
    RkAiqAlgoProcResAorbInt* rkaiqAorbProcOut = (RkAiqAlgoProcResAorbInt*)outparams;

    rkaiqAorbProcParam->orb_stats;
    rkaiqAorbProcOut->orb_en = ORBctx->orb_en;
    rkaiqAorbProcOut->limit_value = ORBctx->limit_value;
    rkaiqAorbProcOut->max_feature = ORBctx->max_feature;

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn ORBPostProcess(const RkAiqAlgoCom* inparams, RkAiqAlgoResCom* outparams)
{
    XCamReturn result = XCAM_RETURN_NO_ERROR;

    ORBHandle_t hORB = (ORBHandle_t)inparams->ctx->hORB;
    ORBContext_t* ORBctx = (ORBContext_t*)hORB;

    LOGI_ORB("%s: (enter)\n", __FUNCTION__ );

    return XCAM_RETURN_NO_ERROR;
}

RkAiqAlgoDescription g_RkIspAlgoDescAorb = {
    .common = {
        .version = RKISP_ALGO_ORB_VERSION,
        .vendor  = RKISP_ALGO_ORB_VENDOR,
        .description = RKISP_ALGO_ORB_DESCRIPTION,
        .type    = RK_AIQ_ALGO_TYPE_AORB,
        .id      = 0,
        .create_context  = ORBCreateContext,
        .destroy_context = ORBDestroyContext,
    },
    .prepare = ORBPrepare,
    .pre_process = ORBPreProcess,
    .processing = ORBProcessing,
    .post_process = ORBPostProcess,
};

RKAIQ_END_DECLARE
