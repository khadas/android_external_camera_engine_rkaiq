/*
 * rkisp_aiq_core.h
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

#include "RkAiqHandleInt.h"
#include "RkAiqCore.h"
#include "v4l2_buffer_proxy.h"
#include "acp/rk_aiq_algo_acp_itf.h"
#include "ae/rk_aiq_algo_ae_itf.h"
#include "awb/rk_aiq_algo_awb_itf.h"
#include "af/rk_aiq_algo_af_itf.h"
#include "anr/rk_aiq_algo_anr_itf.h"
#include "asd/rk_aiq_algo_asd_itf.h"
#include "ahdr/rk_aiq_algo_ahdr_itf.h"
#include "adrc/rk_aiq_algo_adrc_itf.h"
#include "asharp/rk_aiq_algo_asharp_itf.h"
#include "adehaze/rk_aiq_algo_adhaz_itf.h"
#include "ablc/rk_aiq_algo_ablc_itf.h"
#include "adpcc/rk_aiq_algo_adpcc_itf.h"
#include "alsc/rk_aiq_algo_alsc_itf.h"
#include "agic/rk_aiq_algo_agic_itf.h"
#include "adebayer/rk_aiq_algo_adebayer_itf.h"
#include "accm/rk_aiq_algo_accm_itf.h"
#include "agamma/rk_aiq_algo_agamma_itf.h"
#include "awdr/rk_aiq_algo_awdr_itf.h"
#include "a3dlut/rk_aiq_algo_a3dlut_itf.h"
#include "aldch/rk_aiq_algo_aldch_itf.h"
#include "ar2y/rk_aiq_algo_ar2y_itf.h"
#include "aie/rk_aiq_algo_aie_itf.h"
#include "aorb/rk_aiq_algo_aorb_itf.h"
#include "afec/rk_aiq_algo_afec_itf.h"
#include "acgc/rk_aiq_algo_acgc_itf.h"
#ifdef RK_SIMULATOR_HW
#include "simulator/isp20_hw_simulator.h"
#else
#include "isp20/Isp20StatsBuffer.h"
#include "isp20/rkisp2-config.h"
#include "isp20/rkispp-config.h"
#endif
#include <fcntl.h>
#include <unistd.h>

namespace RkCam {
#define EPSINON 0.0000001

/*
 * isp/ispp pipeline algos ordered array, and the analyzer
 * will run these algos one by one.
 */
static struct RkAiqAlgoDesCommExt g_default_3a_des[] = {
    { (RkAiqAlgoDesComm*)&g_RkIspAlgoDescAe, 0, 0 },
    { (RkAiqAlgoDesComm*)&g_RkIspAlgoDescAwb, 0, 0 },
    { (RkAiqAlgoDesComm*)&g_RkIspAlgoDescAf, 0, 0 },
    { (RkAiqAlgoDesComm*)&g_RkIspAlgoDescAblc, 0, 0 },
    { (RkAiqAlgoDesComm*)&g_RkIspAlgoDescAdpcc, 0, 0 },
    { (RkAiqAlgoDesComm*)&g_RkIspAlgoDescAhdr, 0, 0 },
    { (RkAiqAlgoDesComm*)&g_RkIspAlgoDescAnr, 0, 0 },
    { (RkAiqAlgoDesComm*)&g_RkIspAlgoDescAlsc, 0, 0 },
    { (RkAiqAlgoDesComm*)&g_RkIspAlgoDescAgic, 0, 0 },
    { (RkAiqAlgoDesComm*)&g_RkIspAlgoDescAdebayer, 0, 0 },
    { (RkAiqAlgoDesComm*)&g_RkIspAlgoDescAccm, 0, 0 },
    { (RkAiqAlgoDesComm*)&g_RkIspAlgoDescAgamma, 0, 0 },
    { (RkAiqAlgoDesComm*)&g_RkIspAlgoDescAwdr, 0, 0 },
    { (RkAiqAlgoDesComm*)&g_RkIspAlgoDescAdhaz, 0, 0 },
    { (RkAiqAlgoDesComm*)&g_RkIspAlgoDescA3dlut, 0, 0 },
    { (RkAiqAlgoDesComm*)&g_RkIspAlgoDescAldch, 0, 0 },
    { (RkAiqAlgoDesComm*)&g_RkIspAlgoDescAr2y, 0, 0 },
    { (RkAiqAlgoDesComm*)&g_RkIspAlgoDescAcp, 0, 0 },
    { (RkAiqAlgoDesComm*)&g_RkIspAlgoDescAie, 0, 0 },
    { (RkAiqAlgoDesComm*)&g_RkIspAlgoDescAsharp, 0, 0 },
    { (RkAiqAlgoDesComm*)&g_RkIspAlgoDescAorb, 0, 0 },
    { (RkAiqAlgoDesComm*)&g_RkIspAlgoDescAfec, 0, 0 },
    { (RkAiqAlgoDesComm*)&g_RkIspAlgoDescAcgc, 0, 0 },
    { (RkAiqAlgoDesComm*)&g_RkIspAlgoDescAsd, 0, 0 },
    // { (RkAiqAlgoDesComm*)&g_RkIspAlgoDescAdrc, 0,0 },
    { NULL, 0, 0 },
};

bool
RkAiqCoreThread::loop()
{
    ENTER_ANALYZER_FUNCTION();

    const static int32_t timeout = -1;
    SmartPtr<VideoBuffer> stats = mStatsQueue.pop (timeout);

    if (!stats.ptr()) {
        LOGW_ANALYZER("RkAiqCoreThread got empty stats, stop thread");
        return false;
    }

    XCamReturn ret = mRkAiqCore->analyze (stats);
    if (ret == XCAM_RETURN_NO_ERROR || ret == XCAM_RETURN_BYPASS)
        return true;

    LOGE_ANALYZER("RkAiqCoreThread failed to analyze 3a stats");

    EXIT_ANALYZER_FUNCTION();

    return false;
}

// notice that some pool shared items may be cached by other
// modules(e.g. CamHwIsp20), so here should consider the cached number
uint16_t RkAiqCore::DEFAULT_POOL_SIZE = 15;

RkAiqCore::RkAiqCore()
    : mRkAiqCoreTh(new RkAiqCoreThread(this))
    , mRkAiqCorePpTh(new RkAiqCoreThread(this))
    , mState(RK_AIQ_CORE_STATE_INVALID)
    , mCb(NULL)
    , mAiqParamsPool(new RkAiqFullParamsPool("RkAiqFullParams", 4))
    , mAiqExpParamsPool(new RkAiqExpParamsPool("RkAiqExpParams", 4))
    , mAiqIrisParamsPool(new RkAiqIrisParamsPool("RkAiqIrisParams", 4))
    //, mAiqIspParamsPool(new RkAiqIspParamsPool("RkAiqIspParams", RkAiqCore::DEFAULT_POOL_SIZE))
    , mAiqFocusParamsPool(new RkAiqFocusParamsPool("RkAiqFocusParams", 4))
    , mAiqCpslParamsPool(new RkAiqCpslParamsPool("RkAiqCpslParamsPool", 4))
    , mAiqStatsPool(new RkAiqStatsPool("RkAiqStatsPool", 4))
{
    ENTER_ANALYZER_FUNCTION();
    mAlogsSharedParams.reset();
    xcam_mem_clear(mHwInfo);
    mCurCpslOn = false;
    mStrthLed = 0.0f;
    mStrthIr = 0.0f;
    mGrayMode = RK_AIQ_GRAY_MODE_CPSL;
    firstStatsReceived = false;

    SmartPtr<RkAiqFullParams> fullParam = new RkAiqFullParams();
    mAiqCurParams = new RkAiqFullParamsProxy(fullParam );
    mAiqIspParamsPool = new RkAiqIspParamsV20Pool("RkAiqIspParamsV20", RkAiqCore::DEFAULT_POOL_SIZE);
    mAiqIspParamsPool->init(RkAiqCore::DEFAULT_POOL_SIZE);
    mHasPp = true;
    mAlgosDesArray = g_default_3a_des;
    mIspHwVer  = 0;

    EXIT_ANALYZER_FUNCTION();
}

RkAiqCore::~RkAiqCore()
{
    ENTER_ANALYZER_FUNCTION();
    if (mAlogsSharedParams.resourcePath) {
        xcam_free((void*)(mAlogsSharedParams.resourcePath));
        mAlogsSharedParams.resourcePath = NULL;
    }
    EXIT_ANALYZER_FUNCTION();
}

void RkAiqCore::initCpsl()
{
    queryCpsLtCap(mCpslCap);

    rk_aiq_cpsl_cfg_t* cfg = &mAlogsSharedParams.cpslCfg;
    const CamCalibDbContext_t* aiqCalib = mAlogsSharedParams.calib;
    // TODO: something from calib
    if (mCpslCap.modes_num > 0 && aiqCalib->cpsl.support_en) {
        if (aiqCalib->cpsl.mode == 0) {
            cfg->mode = RK_AIQ_OP_MODE_AUTO;
        } else if (aiqCalib->cpsl.mode == 1) {
            cfg->mode = RK_AIQ_OP_MODE_MANUAL;
        } else {
            cfg->mode = RK_AIQ_OP_MODE_INVALID;
        }

        if (aiqCalib->cpsl.lght_src == 0) {
            cfg->lght_src = RK_AIQ_CPSLS_LED;
        } else if (aiqCalib->cpsl.lght_src == 1) {
            cfg->lght_src = RK_AIQ_CPSLS_IR;
        } else if (aiqCalib->cpsl.lght_src == 2) {
            cfg->lght_src = RK_AIQ_CPSLS_MIX;
        } else {
            cfg->lght_src = RK_AIQ_CPSLS_INVALID;
        }
        cfg->gray_on = aiqCalib->cpsl.gray;
        if (cfg->mode == RK_AIQ_OP_MODE_AUTO) {
            cfg->u.a.sensitivity = aiqCalib->cpsl.ajust_sens;
            cfg->u.a.sw_interval = aiqCalib->cpsl.sw_interval;
            LOGI_ANALYZER("mode sensitivity %f, interval time %d s\n",
                          cfg->u.a.sensitivity, cfg->u.a.sw_interval);
        } else {
            cfg->u.m.on = aiqCalib->cpsl.cpsl_on;
            cfg->u.m.strength_ir = aiqCalib->cpsl.strength;
            cfg->u.m.strength_led = aiqCalib->cpsl.strength;
            LOGI_ANALYZER("on %d, strength_led %f, strength_ir %f \n",
                          cfg->u.m.on, cfg->u.m.strength_led, cfg->u.m.strength_ir);
        }
    } else {
        cfg->mode = RK_AIQ_OP_MODE_INVALID;
        LOGI_ANALYZER("not support light compensation \n");
    }
}

XCamReturn
RkAiqCore::init(const char* sns_ent_name, const CamCalibDbContext_t* aiqCalib)
{
    ENTER_ANALYZER_FUNCTION();

    if (mState != RK_AIQ_CORE_STATE_INVALID) {
        LOGE_ANALYZER("wrong state %d\n", mState);
        return XCAM_RETURN_ERROR_ANALYZER;
    }

    if (mHasPp)
        mAiqIsppParamsPool = new RkAiqIsppParamsPool("RkAiqIsppParams", 4);

    mAlogsSharedParams.calib = aiqCalib;

    addDefaultAlgos(mAlgosDesArray);
    initCpsl();

    mState = RK_AIQ_CORE_STATE_INITED;
    return XCAM_RETURN_NO_ERROR;

    EXIT_ANALYZER_FUNCTION();
}

XCamReturn
RkAiqCore::deInit()
{
    ENTER_ANALYZER_FUNCTION();

    if (mState == RK_AIQ_CORE_STATE_STARTED || mState == RK_AIQ_CORE_STATE_RUNNING) {
        LOGE_ANALYZER("wrong state %d\n", mState);
        return XCAM_RETURN_ERROR_ANALYZER;
    }

    mState = RK_AIQ_CORE_STATE_INVALID;

    EXIT_ANALYZER_FUNCTION();

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
RkAiqCore::start()
{
    ENTER_ANALYZER_FUNCTION();

    if ((mState != RK_AIQ_CORE_STATE_PREPARED) &&
            (mState != RK_AIQ_CORE_STATE_STOPED)) {
        LOGE_ANALYZER("wrong state %d\n", mState);
        return XCAM_RETURN_ERROR_ANALYZER;
    }

    mRkAiqCoreTh->triger_start();
    mRkAiqCoreTh->start();
    if (mHasPp) {
        mRkAiqCorePpTh->triger_start();
        mRkAiqCorePpTh->start();
    }
    mState = RK_AIQ_CORE_STATE_STARTED;

    EXIT_ANALYZER_FUNCTION();

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
RkAiqCore::stop()
{
    ENTER_ANALYZER_FUNCTION();

    if (mState != RK_AIQ_CORE_STATE_STARTED && mState != RK_AIQ_CORE_STATE_RUNNING) {
        LOGW_ANALYZER("in state %d\n", mState);
        return XCAM_RETURN_NO_ERROR;
    }

    mRkAiqCoreTh->triger_stop();
    mRkAiqCoreTh->stop();

    if (mHasPp) {
        mRkAiqCorePpTh->triger_stop();
        mRkAiqCorePpTh->stop();
    }
    mAiqStatsCachedList.clear();
    mAiqStatsOutMap.clear();
    mState = RK_AIQ_CORE_STATE_STOPED;
    firstStatsReceived = false;
    mIspStatsCond.broadcast ();
    EXIT_ANALYZER_FUNCTION();

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
RkAiqCore::prepare(const rk_aiq_exposure_sensor_descriptor* sensor_des,
                   int mode)
{
    ENTER_ANALYZER_FUNCTION();
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    // check state
    if ((mState == RK_AIQ_CORE_STATE_STARTED) ||
            (mState == RK_AIQ_CORE_STATE_INVALID) ||
            (mState == RK_AIQ_CORE_STATE_RUNNING)) {
        LOGW_ANALYZER("in state %d\n", mState);
        return XCAM_RETURN_NO_ERROR;
    }

    mAlogsSharedParams.snsDes = *sensor_des;
    mAlogsSharedParams.working_mode = mode;

    if ((mAlogsSharedParams.snsDes.sensor_pixelformat == V4L2_PIX_FMT_GREY) ||
            (mAlogsSharedParams.snsDes.sensor_pixelformat == V4L2_PIX_FMT_Y10) ||
            (mAlogsSharedParams.snsDes.sensor_pixelformat == V4L2_PIX_FMT_Y12)) {
        mAlogsSharedParams.is_bw_sensor = true;
        mGrayMode = RK_AIQ_GRAY_MODE_ON;
        mAlogsSharedParams.gray_mode = true;
    } else {
        mAlogsSharedParams.is_bw_sensor = false;
        if (mAlogsSharedParams.calib->colorAsGrey.enable) {
            mAlogsSharedParams.gray_mode = true;
            mGrayMode = RK_AIQ_GRAY_MODE_ON;
        }
    }

    for (auto algoHdl : mCurIspAlgoHandleList) {
        if (algoHdl.ptr() && algoHdl->getEnable()) {
            /* update user initial params */ \
            ret = algoHdl->updateConfig(true);
            \
            RKAIQCORE_CHECK_BYPASS(ret, "algoHdl %d update initial user params failed", algoHdl->getAlgoType());
            \
            algoHdl->setReConfig(mState == RK_AIQ_CORE_STATE_STOPED);
            \
            ret = algoHdl->prepare();
            \
            RKAIQCORE_CHECK_BYPASS(ret, "algoHdl %d prepare failed", algoHdl->getAlgoType());
            \
        }
    }

    for (auto algoHdl : mCurIsppAlgoHandleList) {
        if (algoHdl.ptr() && algoHdl->getEnable()) {
            /* update user initial params */ \
            ret = algoHdl->updateConfig(true);
            \
            RKAIQCORE_CHECK_BYPASS(ret, "algoHdl %d update initial user params failed", algoHdl->getAlgoType());
            \
            algoHdl->setReConfig(mState == RK_AIQ_CORE_STATE_STOPED);
            \
            ret = algoHdl->prepare();
            \
            RKAIQCORE_CHECK_BYPASS(ret, "algoHdl %d prepare failed", algoHdl->getAlgoType());
            \
        }
    }

    mAlogsSharedParams.init = true;
    analyzeInternal();
    if (mHasPp)
        analyzeInternalPp();

    if (mAiqCurParams->data()->mIspParams.ptr()) {
        mAiqCurParams->data()->mIspParams->data()->frame_id = 0;
    }

    if (mAiqCurParams->data()->mIsppParams.ptr()) {
        mAiqCurParams->data()->mIsppParams->data()->frame_id = 0;
    }

    mState = RK_AIQ_CORE_STATE_PREPARED;

    EXIT_ANALYZER_FUNCTION();

    return XCAM_RETURN_NO_ERROR;
}

SmartPtr<RkAiqFullParamsProxy>
RkAiqCore::analyzeInternal()
{
    ENTER_ANALYZER_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    if (mAlogsSharedParams.init) {
        // run algos without stats to generate
        // initial params
        mAlogsSharedParams.ispStats.aec_stats_valid = false;
        mAlogsSharedParams.ispStats.awb_stats_valid = false;
        mAlogsSharedParams.ispStats.awb_cfg_effect_valid = false;
        mAlogsSharedParams.ispStats.af_stats_valid = false;
        mAlogsSharedParams.ispStats.ahdr_stats_valid = false;
    }

    SmartPtr<RkAiqFullParamsProxy> aiqParamProxy = mAiqParamsPool->get_item();

    if (!aiqParamProxy.ptr()) {
        LOGE_ANALYZER("no free aiq params buffer!");
        return NULL;
    }

    RkAiqFullParams* aiqParams = aiqParamProxy->data().ptr();
    aiqParams->reset();

    if (mAiqIspParamsPool->has_free_items()) {
        SmartPtr<rk_aiq_isp_params_t> data =
            mAiqIspParamsPool->get_item()->data();
        aiqParams->mIspParams = new RkAiqIspParamsProxy(data);
    } else {
        LOGE_ANALYZER("no free isp params buffer!");
        return NULL;
    }

    if (mAiqExpParamsPool->has_free_items()) {
        aiqParams->mExposureParams = mAiqExpParamsPool->get_item();
    } else {
        LOGE_ANALYZER("no free exposure params buffer!");
        return NULL;
    }

    if (mAiqIrisParamsPool->has_free_items()) {
        aiqParams->mIrisParams = mAiqIrisParamsPool->get_item();
    } else {
        LOGE_ANALYZER("no free iris params buffer!");
        return NULL;
    }

    if (mAiqFocusParamsPool->has_free_items()) {
        aiqParams->mFocusParams = mAiqFocusParamsPool->get_item();
    } else {
        LOGE_ANALYZER("no free focus params buffer!");
        return NULL;
    }

    if (mHasPp) {
        if (mAiqIsppParamsPool->has_free_items()) {
            aiqParams->mIsppParams = mAiqIsppParamsPool->get_item();
            aiqParams->mIsppParams->data()->update_mask = 0;
        } else {
            LOGE_ANALYZER("no free ispp params buffer!");
            //return NULL;
        }
    }

#if 0
    // for test
    int fd = open("/tmp/cpsl", O_RDWR);
    if (fd != -1) {
        char c;
        read(fd, &c, 1);
        int enable = atoi(&c);

        rk_aiq_cpsl_cfg_t cfg;

        cfg.mode = (RKAiqOPMode_t)enable;
        cfg.lght_src = RK_AIQ_CPSLS_LED;
        if (cfg.mode == RK_AIQ_OP_MODE_AUTO) {
            cfg.u.a.sensitivity = 100;
            cfg.u.a.sw_interval = 60;
            cfg.gray_on = false;
            LOGI_ANALYZER("mode sensitivity %f, interval time %d s\n",
                          cfg.u.a.sensitivity, cfg.u.a.sw_interval);
        } else {
            cfg.gray_on = true;
            cfg.u.m.on = true;
            cfg.u.m.strength_ir = 100;
            cfg.u.m.strength_led = 100;
            LOGI_ANALYZER("on %d, strength_led %f, strength_ir %f\n",
                          cfg.u.m.on, cfg.u.m.strength_led, cfg.u.m.strength_ir);
        }
        close(fd);
        setCpsLtCfg(cfg);
    }
#endif
    ret = preProcess();
    RKAIQCORE_CHECK_RET_NULL(ret, "preprocess failed");
    genCpslResult(aiqParams);

    ret = processing();
    RKAIQCORE_CHECK_RET_NULL(ret, "processing failed");

    ret = postProcess();
    RKAIQCORE_CHECK_RET_NULL(ret, "post process failed");

    genIspAeResult(aiqParams);
    genIspAwbResult(aiqParams);
    genIspAfResult(aiqParams);
    genIspAhdrResult(aiqParams);
    genIspAnrResult(aiqParams);
    genIspAdhazResult(aiqParams);
    genIspAsdResult(aiqParams);
    genIspAcpResult(aiqParams);
    genIspAieResult(aiqParams);
    genIspAsharpResult(aiqParams);
    genIspA3dlutResult(aiqParams);
    genIspAblcResult(aiqParams);
    genIspAccmResult(aiqParams);
    genIspAcgcResult(aiqParams);
    genIspAdebayerResult(aiqParams);
    genIspAdpccResult(aiqParams);
    genIspAfecResult(aiqParams);
    //genIspAorbResult(aiqParams);
    genIspAgammaResult(aiqParams);
    genIspAgicResult(aiqParams);
    genIspAldchResult(aiqParams);
    genIspAlscResult(aiqParams);
    genIspAr2yResult(aiqParams);
    genIspAwdrResult(aiqParams);
    genIspArawnrResult(aiqParams);
    genIspAmfnrResult(aiqParams);
    genIspAynrResult(aiqParams);
    genIspAcnrResult(aiqParams);
    genIspAdrcResult(aiqParams);
    mAiqCurParams->data()->mIspParams = aiqParams->mIspParams;
    mAiqCurParams->data()->mExposureParams = aiqParams->mExposureParams;
    mAiqCurParams->data()->mIrisParams = aiqParams->mIrisParams;
    mAiqCurParams->data()->mFocusParams = aiqParams->mFocusParams;
    mAiqCurParams->data()->mIsppParams = aiqParams->mIsppParams;
    mAiqCurParams->data()->mCpslParams = aiqParams->mCpslParams;

    EXIT_ANALYZER_FUNCTION();

    return aiqParamProxy;
}

SmartPtr<RkAiqFullParamsProxy>
RkAiqCore::analyzeInternalPp()
{
    ENTER_ANALYZER_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    if (mAlogsSharedParams.init) {
        // run algos without stats to generate
        // initial params
        mAlogsSharedParams.ispStats.orb_stats_valid = false;
    }

    SmartPtr<RkAiqFullParamsProxy> aiqParamProxy = mAiqParamsPool->get_item();

    if (!aiqParamProxy.ptr()) {
        LOGE_ANALYZER("no free aiq params buffer!");
        return NULL;
    }

    RkAiqFullParams* aiqParams = aiqParamProxy->data().ptr();
    aiqParams->reset();

    if (mAiqIsppParamsPool->has_free_items()) {
        aiqParams->mIsppParams = mAiqIsppParamsPool->get_item();
        if (!aiqParams->mIsppParams.ptr()) {
            LOGE_ANALYZER("no free ispp params buffer!");
            return NULL;
        }
        aiqParams->mIsppParams->data()->update_mask = 0;
    } else {
        LOGE_ANALYZER("no free ispp params buffer!");
        return NULL;
    }

    ret = preProcessPp();
    RKAIQCORE_CHECK_RET_NULL(ret, "preprocessPp failed");

    ret = processingPp();
    RKAIQCORE_CHECK_RET_NULL(ret, "processingPp failed");

    ret = postProcessPp();
    RKAIQCORE_CHECK_RET_NULL(ret, "post processPp failed");

    genIspAorbResult(aiqParams);

    if (!mAiqCurParams->data()->mIsppParams.ptr()) {
        mAiqCurParams->data()->mIsppParams = aiqParams->mIsppParams;
    } else {
        if (aiqParams->mIsppParams->data()->update_mask & RKAIQ_ISPP_ORB_ID) {
            mAiqCurParams->data()->mIsppParams->data()->update_mask |= RKAIQ_ISPP_ORB_ID;
            mAiqCurParams->data()->mIsppParams->data()->orb =
                aiqParams->mIsppParams->data()->orb;
        }
    }

    EXIT_ANALYZER_FUNCTION();

    return aiqParamProxy;
}

XCamReturn
RkAiqCore::genIspAeResult(RkAiqFullParams* params)
{
    ENTER_ANALYZER_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    RkAiqAlgoProcResAe* ae_proc =
        mAlogsSharedParams.procResComb.ae_proc_res;
    RkAiqAlgoPostResAe* ae_post =
        mAlogsSharedParams.postResComb.ae_post_res;

    rk_aiq_isp_params_v20_t* isp_param =
        static_cast<rk_aiq_isp_params_v20_t*>(params->mIspParams->data().ptr());
    SmartPtr<rk_aiq_exposure_params_wrapper_t> exp_param =
        params->mExposureParams->data();
    SmartPtr<rk_aiq_iris_params_wrapper_t> iris_param =
        params->mIrisParams->data();

    if (!ae_proc) {
        LOGD_ANALYZER("no ae_proc result");
        return XCAM_RETURN_NO_ERROR;
    }

    if (!ae_post) {
        LOGD_ANALYZER("no ae_post result");
        return XCAM_RETURN_NO_ERROR;
    }

    // TODO: gen ae common result
    SmartPtr<RkAiqHandle>* handle = getCurAlgoTypeHandle(RK_AIQ_ALGO_TYPE_AE);
    int algo_id = (*handle)->getAlgoId();
    // gen common result

    exp_param->aecExpInfo.LinearExp = ae_proc->new_ae_exp.LinearExp;
    memcpy(exp_param->aecExpInfo.HdrExp, ae_proc->new_ae_exp.HdrExp, sizeof(ae_proc->new_ae_exp.HdrExp));
    exp_param->aecExpInfo.frame_length_lines = ae_proc->new_ae_exp.frame_length_lines;
    exp_param->aecExpInfo.line_length_pixels = ae_proc->new_ae_exp.line_length_pixels;
    exp_param->aecExpInfo.pixel_clock_freq_mhz = ae_proc->new_ae_exp.pixel_clock_freq_mhz;
    exp_param->aecExpInfo.Iris.PIris = ae_proc->new_ae_exp.Iris.PIris;

    iris_param->IrisType = ae_proc->new_ae_exp.Iris.IrisType;
    iris_param->PIris.step = ae_proc->new_ae_exp.Iris.PIris.step;
    iris_param->PIris.update = ae_proc->new_ae_exp.Iris.PIris.update;

    isp_param->aec_meas = ae_proc->ae_meas;
    isp_param->hist_meas = ae_proc->hist_meas;

    // gen rk ae result
    if (algo_id == 0) {
        RkAiqAlgoProcResAeInt* ae_rk = (RkAiqAlgoProcResAeInt*)ae_proc;
        memcpy(exp_param->exp_tbl, ae_rk->ae_proc_res_rk.exp_set_tbl, sizeof(exp_param->exp_tbl));
        exp_param->exp_tbl_size = ae_rk->ae_proc_res_rk.exp_set_cnt;
        exp_param->algo_id = algo_id;

        RkAiqAlgoPostResAeInt* ae_post_rk = (RkAiqAlgoPostResAeInt*)ae_post;
        iris_param->DCIris.update = ae_post_rk->ae_post_res_rk.DCIris.update;
        iris_param->DCIris.pwmDuty = ae_post_rk->ae_post_res_rk.DCIris.pwmDuty;
    }

    EXIT_ANALYZER_FUNCTION();

    return ret;
}

XCamReturn
RkAiqCore::genIspAwbResult(RkAiqFullParams* params)
{
    ENTER_ANALYZER_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    RkAiqAlgoProcResAwb* awb_com =
        mAlogsSharedParams.procResComb.awb_proc_res;
    rk_aiq_isp_params_v20_t* isp_param =
        static_cast<rk_aiq_isp_params_v20_t*>(params->mIspParams->data().ptr());

    if (!awb_com) {
        LOGD_ANALYZER("no awb result");
        return XCAM_RETURN_NO_ERROR;
    }

    // TODO: gen awb common result
    RkAiqAlgoProcResAwb* awb_rk = (RkAiqAlgoProcResAwb*)awb_com;
    isp_param->awb_gain_update = awb_rk->awb_gain_update;
    isp_param->awb_cfg_update = awb_rk->awb_cfg_update;
    isp_param->awb_gain = awb_rk->awb_gain_algo;
    isp_param->awb_cfg = awb_rk->awb_hw0_para;
    //isp_param->awb_cfg_v201 = awb_rk->awb_hw1_para;

    SmartPtr<RkAiqHandle>* handle = getCurAlgoTypeHandle(RK_AIQ_ALGO_TYPE_AWB);
    int algo_id = (*handle)->getAlgoId();

    // gen rk awb result
    if (algo_id == 0) {
        RkAiqAlgoProcResAwbInt* awb_rk_int = (RkAiqAlgoProcResAwbInt*)awb_com;
    }

    EXIT_ANALYZER_FUNCTION();

    return ret;
}

XCamReturn
RkAiqCore::genIspAfResult(RkAiqFullParams* params)
{
    ENTER_ANALYZER_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    RkAiqAlgoProcResAf* af_com =
        mAlogsSharedParams.procResComb.af_proc_res;

    rk_aiq_isp_params_v20_t* isp_param =
        static_cast<rk_aiq_isp_params_v20_t*>(params->mIspParams->data().ptr());
    SmartPtr<rk_aiq_focus_params_t> focus_param =
        params->mFocusParams->data();

    isp_param->af_cfg_update = false;
    focus_param->lens_pos_valid = false;
    focus_param->zoom_pos_valid = false;
    if (!af_com) {
        LOGD_ANALYZER("no af result");
        return XCAM_RETURN_NO_ERROR;
    }

    // TODO: gen af common result

    SmartPtr<RkAiqHandle>* handle = getCurAlgoTypeHandle(RK_AIQ_ALGO_TYPE_AF);
    int algo_id = (*handle)->getAlgoId();

    // gen rk af result
    if (algo_id == 0) {
        RkAiqAlgoProcResAfInt* af_rk = (RkAiqAlgoProcResAfInt*)af_com;

        isp_param->af_meas = af_rk->af_proc_res_com.af_isp_param;
        isp_param->af_cfg_update = af_rk->af_proc_res_com.af_cfg_update;

        focus_param->next_lens_pos = af_rk->af_proc_res_com.af_focus_param.next_lens_pos;
        focus_param->next_zoom_pos = af_rk->af_proc_res_com.af_focus_param.next_zoom_pos;
        focus_param->lens_pos_valid = af_rk->af_proc_res_com.af_focus_param.lens_pos_valid;
        focus_param->zoom_pos_valid = af_rk->af_proc_res_com.af_focus_param.zoom_pos_valid;
    }

    EXIT_ANALYZER_FUNCTION();

    return ret;
}

XCamReturn
RkAiqCore::genIspAhdrResult(RkAiqFullParams* params)
{
    ENTER_ANALYZER_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    RkAiqAlgoProcResAhdr* ahdr_com =
        mAlogsSharedParams.procResComb.ahdr_proc_res;
    rk_aiq_isp_params_v20_t* isp_param =
        static_cast<rk_aiq_isp_params_v20_t*>(params->mIspParams->data().ptr());

    if (!ahdr_com) {
        LOGD_ANALYZER("no ahdr result");
        return XCAM_RETURN_NO_ERROR;
    }

    // TODO: gen ahdr common result

    SmartPtr<RkAiqHandle>* handle = getCurAlgoTypeHandle(RK_AIQ_ALGO_TYPE_AHDR);
    int algo_id = (*handle)->getAlgoId();

    // gen rk ahdr result
    if (algo_id == 0) {
        RkAiqAlgoProcResAhdrInt* ahdr_rk = (RkAiqAlgoProcResAhdrInt*)ahdr_com;

        isp_param->ahdr_proc_res.MgeProcRes.sw_hdrmge_mode =
            ahdr_rk->AhdrProcRes.MgeProcRes.sw_hdrmge_mode;
        isp_param->ahdr_proc_res.MgeProcRes.sw_hdrmge_lm_dif_0p9 =
            ahdr_rk->AhdrProcRes.MgeProcRes.sw_hdrmge_lm_dif_0p9;
        isp_param->ahdr_proc_res.MgeProcRes.sw_hdrmge_ms_dif_0p8 =
            ahdr_rk->AhdrProcRes.MgeProcRes.sw_hdrmge_ms_dif_0p8;
        isp_param->ahdr_proc_res.MgeProcRes.sw_hdrmge_lm_dif_0p15 =
            ahdr_rk->AhdrProcRes.MgeProcRes.sw_hdrmge_lm_dif_0p15;
        isp_param->ahdr_proc_res.MgeProcRes.sw_hdrmge_ms_dif_0p15 =
            ahdr_rk->AhdrProcRes.MgeProcRes.sw_hdrmge_ms_dif_0p15;
        isp_param->ahdr_proc_res.MgeProcRes.sw_hdrmge_gain0 =
            ahdr_rk->AhdrProcRes.MgeProcRes.sw_hdrmge_gain0;
        isp_param->ahdr_proc_res.MgeProcRes.sw_hdrmge_gain0_inv =
            ahdr_rk->AhdrProcRes.MgeProcRes.sw_hdrmge_gain0_inv;
        isp_param->ahdr_proc_res.MgeProcRes.sw_hdrmge_gain1 =
            ahdr_rk->AhdrProcRes.MgeProcRes.sw_hdrmge_gain1;
        isp_param->ahdr_proc_res.MgeProcRes.sw_hdrmge_gain1_inv =
            ahdr_rk->AhdrProcRes.MgeProcRes.sw_hdrmge_gain1_inv;
        isp_param->ahdr_proc_res.MgeProcRes.sw_hdrmge_gain2 =
            ahdr_rk->AhdrProcRes.MgeProcRes.sw_hdrmge_gain2;
        for(int i = 0; i < 17; i++)
        {
            isp_param->ahdr_proc_res.MgeProcRes.sw_hdrmge_e_y[i] =
                ahdr_rk->AhdrProcRes.MgeProcRes.sw_hdrmge_e_y[i];
            isp_param->ahdr_proc_res.MgeProcRes.sw_hdrmge_l1_y[i] =
                ahdr_rk->AhdrProcRes.MgeProcRes.sw_hdrmge_l1_y[i];
            isp_param->ahdr_proc_res.MgeProcRes.sw_hdrmge_l0_y[i] =
                ahdr_rk->AhdrProcRes.MgeProcRes.sw_hdrmge_l0_y[i];
        }

        isp_param->ahdr_proc_res.TmoProcRes.sw_hdrtmo_lgmax =
            ahdr_rk->AhdrProcRes.TmoProcRes.sw_hdrtmo_lgmax;
        isp_param->ahdr_proc_res.TmoProcRes.sw_hdrtmo_lgscl =
            ahdr_rk->AhdrProcRes.TmoProcRes.sw_hdrtmo_lgscl;
        isp_param->ahdr_proc_res.TmoProcRes.sw_hdrtmo_lgscl_inv =
            ahdr_rk->AhdrProcRes.TmoProcRes.sw_hdrtmo_lgscl_inv;
        isp_param->ahdr_proc_res.TmoProcRes.sw_hdrtmo_clipratio0 =
            ahdr_rk->AhdrProcRes.TmoProcRes.sw_hdrtmo_clipratio0;
        isp_param->ahdr_proc_res.TmoProcRes.sw_hdrtmo_clipratio1 =
            ahdr_rk->AhdrProcRes.TmoProcRes.sw_hdrtmo_clipratio1;
        isp_param->ahdr_proc_res.TmoProcRes.sw_hdrtmo_clipgap0 =
            ahdr_rk->AhdrProcRes.TmoProcRes.sw_hdrtmo_clipgap0;
        isp_param->ahdr_proc_res.TmoProcRes.sw_hdrtmo_clipgap1 =
            ahdr_rk->AhdrProcRes.TmoProcRes.sw_hdrtmo_clipgap1;
        isp_param->ahdr_proc_res.TmoProcRes.sw_hdrtmo_ratiol =
            ahdr_rk->AhdrProcRes.TmoProcRes.sw_hdrtmo_ratiol;
        isp_param->ahdr_proc_res.TmoProcRes.sw_hdrtmo_hist_min =
            ahdr_rk->AhdrProcRes.TmoProcRes.sw_hdrtmo_hist_min;
        isp_param->ahdr_proc_res.TmoProcRes.sw_hdrtmo_hist_low =
            ahdr_rk->AhdrProcRes.TmoProcRes.sw_hdrtmo_hist_low;
        isp_param->ahdr_proc_res.TmoProcRes.sw_hdrtmo_hist_high =
            ahdr_rk->AhdrProcRes.TmoProcRes.sw_hdrtmo_hist_high;
        isp_param->ahdr_proc_res.TmoProcRes.sw_hdrtmo_hist_0p3 =
            ahdr_rk->AhdrProcRes.TmoProcRes.sw_hdrtmo_hist_0p3;
        isp_param->ahdr_proc_res.TmoProcRes.sw_hdrtmo_hist_shift =
            ahdr_rk->AhdrProcRes.TmoProcRes.sw_hdrtmo_hist_shift;
        isp_param->ahdr_proc_res.TmoProcRes.sw_hdrtmo_palpha_0p18 =
            ahdr_rk->AhdrProcRes.TmoProcRes.sw_hdrtmo_palpha_0p18;
        isp_param->ahdr_proc_res.TmoProcRes.sw_hdrtmo_palpha_lw0p5 =
            ahdr_rk->AhdrProcRes.TmoProcRes.sw_hdrtmo_palpha_lw0p5;
        isp_param->ahdr_proc_res.TmoProcRes.sw_hdrtmo_palpha_lwscl =
            ahdr_rk->AhdrProcRes.TmoProcRes.sw_hdrtmo_palpha_lwscl;
        isp_param->ahdr_proc_res.TmoProcRes.sw_hdrtmo_maxpalpha =
            ahdr_rk->AhdrProcRes.TmoProcRes.sw_hdrtmo_maxpalpha;
        isp_param->ahdr_proc_res.TmoProcRes.sw_hdrtmo_maxgain =
            ahdr_rk->AhdrProcRes.TmoProcRes.sw_hdrtmo_maxgain;
        isp_param->ahdr_proc_res.TmoProcRes.sw_hdrtmo_cfg_alpha =
            ahdr_rk->AhdrProcRes.TmoProcRes.sw_hdrtmo_cfg_alpha;
        isp_param->ahdr_proc_res.TmoProcRes.sw_hdrtmo_set_gainoff =
            ahdr_rk->AhdrProcRes.TmoProcRes.sw_hdrtmo_set_gainoff;
        isp_param->ahdr_proc_res.TmoProcRes.sw_hdrtmo_set_lgmin =
            ahdr_rk->AhdrProcRes.TmoProcRes.sw_hdrtmo_set_lgmin;
        isp_param->ahdr_proc_res.TmoProcRes.sw_hdrtmo_set_lgmax =
            ahdr_rk->AhdrProcRes.TmoProcRes.sw_hdrtmo_set_lgmax;
        isp_param->ahdr_proc_res.TmoProcRes.sw_hdrtmo_set_lgmean =
            ahdr_rk->AhdrProcRes.TmoProcRes.sw_hdrtmo_set_lgmean;
        isp_param->ahdr_proc_res.TmoProcRes.sw_hdrtmo_set_weightkey =
            ahdr_rk->AhdrProcRes.TmoProcRes.sw_hdrtmo_set_weightkey;
        isp_param->ahdr_proc_res.TmoProcRes.sw_hdrtmo_set_lgrange0 =
            ahdr_rk->AhdrProcRes.TmoProcRes.sw_hdrtmo_set_lgrange0;
        isp_param->ahdr_proc_res.TmoProcRes.sw_hdrtmo_set_lgrange1 =
            ahdr_rk->AhdrProcRes.TmoProcRes.sw_hdrtmo_set_lgrange1;
        isp_param->ahdr_proc_res.TmoProcRes.sw_hdrtmo_set_lgavgmax =
            ahdr_rk->AhdrProcRes.TmoProcRes.sw_hdrtmo_set_lgavgmax;
        isp_param->ahdr_proc_res.TmoProcRes.sw_hdrtmo_set_palpha =
            ahdr_rk->AhdrProcRes.TmoProcRes.sw_hdrtmo_set_palpha;
        isp_param->ahdr_proc_res.TmoProcRes.sw_hdrtmo_gain_ld_off1 =
            ahdr_rk->AhdrProcRes.TmoProcRes.sw_hdrtmo_gain_ld_off1;
        isp_param->ahdr_proc_res.TmoProcRes.sw_hdrtmo_gain_ld_off2 =
            ahdr_rk->AhdrProcRes.TmoProcRes.sw_hdrtmo_gain_ld_off2;
        isp_param->ahdr_proc_res.TmoProcRes.sw_hdrtmo_cnt_vsize =
            ahdr_rk->AhdrProcRes.TmoProcRes.sw_hdrtmo_cnt_vsize;
        isp_param->ahdr_proc_res.TmoProcRes.sw_hdrtmo_big_en =
            ahdr_rk->AhdrProcRes.TmoProcRes.sw_hdrtmo_big_en;
        isp_param->ahdr_proc_res.TmoProcRes.sw_hdrtmo_nobig_en =
            ahdr_rk->AhdrProcRes.TmoProcRes.sw_hdrtmo_nobig_en;
        isp_param->ahdr_proc_res.TmoProcRes.sw_hdrtmo_newhist_en =
            ahdr_rk->AhdrProcRes.TmoProcRes.sw_hdrtmo_newhist_en;
        isp_param->ahdr_proc_res.TmoProcRes.sw_hdrtmo_cnt_mode =
            ahdr_rk->AhdrProcRes.TmoProcRes.sw_hdrtmo_cnt_mode;
        isp_param->ahdr_proc_res.TmoProcRes.sw_hdrtmo_expl_lgratio =
            ahdr_rk->AhdrProcRes.TmoProcRes.sw_hdrtmo_expl_lgratio;
        isp_param->ahdr_proc_res.TmoProcRes.sw_hdrtmo_lgscl_ratio =
            ahdr_rk->AhdrProcRes.TmoProcRes.sw_hdrtmo_lgscl_ratio;

        isp_param->ahdr_proc_res.isHdrGlobalTmo =
            ahdr_rk->AhdrProcRes.isHdrGlobalTmo;

        isp_param->ahdr_proc_res.bTmoEn =
            ahdr_rk->AhdrProcRes.bTmoEn;

        isp_param->ahdr_proc_res.isLinearTmo =
            ahdr_rk->AhdrProcRes.isLinearTmo;

    }

    EXIT_ANALYZER_FUNCTION();

    return ret;
}

XCamReturn
RkAiqCore::genIspAnrResult(RkAiqFullParams* params)
{
    ENTER_ANALYZER_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    RkAiqAlgoProcResAnr* anr_com =
        mAlogsSharedParams.procResComb.anr_proc_res;
    rk_aiq_isp_params_v20_t* isp_param =
        static_cast<rk_aiq_isp_params_v20_t*>(params->mIspParams->data().ptr());

    if (!anr_com || !params->mIsppParams.ptr()) {
        LOGD_ANALYZER("no anr result");
        return XCAM_RETURN_NO_ERROR;
    }

    SmartPtr<rk_aiq_ispp_params_t> ispp_param =
        params->mIsppParams->data();

    // TODO: gen anr common result

    SmartPtr<RkAiqHandle>* handle = getCurAlgoTypeHandle(RK_AIQ_ALGO_TYPE_ANR);
    int algo_id = (*handle)->getAlgoId();

    // gen rk anr result
    if (algo_id == 0) {
        RkAiqAlgoProcResAnrInt* anr_rk = (RkAiqAlgoProcResAnrInt*)anr_com;

#ifdef RK_SIMULATOR_HW
        LOGD_ANR("oyyf: %s:%d output isp param start\n", __FUNCTION__, __LINE__);
        memcpy(&isp_param->rkaiq_anr_proc_res.stBayernrParamSelect,
               &anr_rk->stAnrProcResult.stBayernrParamSelect,
               sizeof(RKAnr_Bayernr_Params_Select_t));
        memcpy(&isp_param->rkaiq_anr_proc_res.stUvnrParamSelect,
               &anr_rk->stAnrProcResult.stUvnrParamSelect,
               sizeof(RKAnr_Uvnr_Params_Select_t));

        memcpy(&isp_param->rkaiq_anr_proc_res.stMfnrParamSelect,
               &anr_rk->stAnrProcResult.stMfnrParamSelect,
               sizeof(RKAnr_Mfnr_Params_Select_t));

        memcpy(&isp_param->rkaiq_anr_proc_res.stYnrParamSelect,
               &anr_rk->stAnrProcResult.stYnrParamSelect,
               sizeof(RKAnr_Ynr_Params_Select_t));

        LOGD_ANR("oyyf: %s:%d output isp param end \n", __FUNCTION__, __LINE__);
#else
        LOGD_ANR("oyyf: %s:%d output isp param start\n", __FUNCTION__, __LINE__);
        memcpy(&isp_param->rawnr,
               &anr_rk->stAnrProcResult.stBayernrFix,
               sizeof(rk_aiq_isp_rawnr_t));

        ispp_param->update_mask |= RKAIQ_ISPP_NR_ID;
        memcpy(&ispp_param->uvnr,
               &anr_rk->stAnrProcResult.stUvnrFix,
               sizeof(RKAnr_Uvnr_Fix_t));

        memcpy(&ispp_param->ynr,
               &anr_rk->stAnrProcResult.stYnrFix,
               sizeof(RKAnr_Ynr_Fix_t));

        ispp_param->update_mask |= RKAIQ_ISPP_TNR_ID;
        memcpy(&ispp_param->tnr,
               &anr_rk->stAnrProcResult.stMfnrFix,
               sizeof(RKAnr_Mfnr_Fix_t));

        memcpy(&isp_param->gain_config,
               &anr_rk->stAnrProcResult.stGainFix,
               sizeof(rk_aiq_isp_gain_t));

        LOGD_ANR("oyyf: %s:%d output isp param end \n", __FUNCTION__, __LINE__);

#endif
    }

    EXIT_ANALYZER_FUNCTION();

    return ret;
}

XCamReturn
RkAiqCore::genIspAdhazResult(RkAiqFullParams* params)
{
    ENTER_ANALYZER_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    RkAiqAlgoProcResAdhaz* adhaz_com =
        mAlogsSharedParams.procResComb.adhaz_proc_res;
    rk_aiq_isp_params_v20_t* isp_param =
        static_cast<rk_aiq_isp_params_v20_t*>(params->mIspParams->data().ptr());

    if (!adhaz_com) {
        LOGD_ANALYZER("no adhaz result");
        return XCAM_RETURN_NO_ERROR;
    }

    // TODO: gen adhaz common result
    RkAiqAlgoProcResAdhaz* adhaz_rk = (RkAiqAlgoProcResAdhaz*)adhaz_com;

    isp_param->adhaz = adhaz_rk->AdehzeProcRes.ProcResV20;

    SmartPtr<RkAiqHandle>* handle = getCurAlgoTypeHandle(RK_AIQ_ALGO_TYPE_ADHAZ);
    int algo_id = (*handle)->getAlgoId();

    // gen rk adhaz result
    if (algo_id == 0) {
        RkAiqAlgoProcResAdhazInt* adhaz_rk = (RkAiqAlgoProcResAdhazInt*)adhaz_com;

        isp_param->adhaz.enhance_en     = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.enhance_en;
        isp_param->adhaz.hist_chn   = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.hist_chn;
        isp_param->adhaz.hpara_en   = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.hpara_en;
        isp_param->adhaz.hist_en    = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.hist_en;
        isp_param->adhaz.dc_en  = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.dc_en;
        isp_param->adhaz.big_en     = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.big_en;
        isp_param->adhaz.nobig_en   = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.nobig_en;
        isp_param->adhaz.yblk_th    = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.yblk_th;
        isp_param->adhaz.yhist_th   = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.yhist_th;
        isp_param->adhaz.dc_max_th  = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.dc_max_th;
        isp_param->adhaz.dc_min_th  = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.dc_min_th;
        isp_param->adhaz.wt_max     = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.wt_max;
        isp_param->adhaz.bright_max     = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.bright_max;
        isp_param->adhaz.bright_min     = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.bright_min;
        isp_param->adhaz.tmax_base  = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.tmax_base;
        isp_param->adhaz.dark_th    = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.dark_th;
        isp_param->adhaz.air_max    = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.air_max;
        isp_param->adhaz.air_min    = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.air_min;
        isp_param->adhaz.tmax_max   = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.tmax_max;
        isp_param->adhaz.tmax_off   = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.tmax_off;
        isp_param->adhaz.hist_k     = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.hist_k;
        isp_param->adhaz.hist_th_off    = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.hist_th_off;
        isp_param->adhaz.hist_min   = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.hist_min;
        isp_param->adhaz.hist_gratio    = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.hist_gratio;
        isp_param->adhaz.hist_scale     = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.hist_scale;
        isp_param->adhaz.enhance_value  = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.enhance_value;
        isp_param->adhaz.iir_wt_sigma   = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.iir_wt_sigma;
        isp_param->adhaz.iir_sigma  = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.iir_sigma;
        isp_param->adhaz.stab_fnum  = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.stab_fnum;
        isp_param->adhaz.iir_tmax_sigma     = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.iir_tmax_sigma;
        isp_param->adhaz.iir_air_sigma  = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.iir_air_sigma;
        isp_param->adhaz.cfg_wt     = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.cfg_wt;
        isp_param->adhaz.cfg_air    = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.cfg_air;
        isp_param->adhaz.cfg_alpha  = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.cfg_alpha;
        isp_param->adhaz.cfg_gratio     = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.cfg_gratio;
        isp_param->adhaz.cfg_tmax   = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.cfg_tmax;
        isp_param->adhaz.dc_weitcur     = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.dc_weitcur;
        isp_param->adhaz.dc_thed    = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.dc_thed;
        isp_param->adhaz.sw_dhaz_dc_bf_h0   = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.sw_dhaz_dc_bf_h0;
        isp_param->adhaz.sw_dhaz_dc_bf_h1   = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.sw_dhaz_dc_bf_h1;
        isp_param->adhaz.sw_dhaz_dc_bf_h2   = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.sw_dhaz_dc_bf_h2;
        isp_param->adhaz.sw_dhaz_dc_bf_h3   = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.sw_dhaz_dc_bf_h3;
        isp_param->adhaz.sw_dhaz_dc_bf_h4   = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.sw_dhaz_dc_bf_h4;
        isp_param->adhaz.sw_dhaz_dc_bf_h5   = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.sw_dhaz_dc_bf_h5;
        isp_param->adhaz.air_weitcur    = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.air_weitcur;
        isp_param->adhaz.air_thed   = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.air_thed;
        isp_param->adhaz.air_bf_h0  = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.air_bf_h0;
        isp_param->adhaz.air_bf_h1  = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.air_bf_h1;
        isp_param->adhaz.air_bf_h2  = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.air_bf_h2;
        isp_param->adhaz.gaus_h0    = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.gaus_h0;
        isp_param->adhaz.gaus_h1    = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.gaus_h1;
        isp_param->adhaz.gaus_h2    = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.gaus_h2;

        for(int i = 0; i < 6; i++) {
            isp_param->adhaz.conv_t0[i]   = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.conv_t0[i];
            isp_param->adhaz.conv_t1[i]   = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.conv_t1[i];
            isp_param->adhaz.conv_t2[i]   = adhaz_rk->adhaz_proc_res_com.AdehzeProcRes.ProcResV20.conv_t2[i];
        }

    }

    EXIT_ANALYZER_FUNCTION();

    return ret;
}

XCamReturn
RkAiqCore::genIspAsdResult(RkAiqFullParams* params)
{
    ENTER_ANALYZER_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    RkAiqAlgoProcResAsd* asd_com =
        mAlogsSharedParams.procResComb.asd_proc_res;
    rk_aiq_isp_params_v20_t* isp_param =
        static_cast<rk_aiq_isp_params_v20_t*>(params->mIspParams->data().ptr());

    if (!asd_com) {
        LOGD_ANALYZER("no asd result");
        return XCAM_RETURN_NO_ERROR;
    }

    // TODO: gen asd common result

    SmartPtr<RkAiqHandle>* handle = getCurAlgoTypeHandle(RK_AIQ_ALGO_TYPE_ASD);
    int algo_id = (*handle)->getAlgoId();

    // gen rk asd result
    if (algo_id == 0) {
        RkAiqAlgoProcResAsdInt* asd_rk = (RkAiqAlgoProcResAsdInt*)asd_com;

#if 0 // flash test
        RkAiqAlgoPreResAsdInt* asd_pre_rk = (RkAiqAlgoPreResAsdInt*)mAlogsSharedParams.preResComb.asd_pre_res;
        if (asd_pre_rk->asd_result.fl_on) {
            fl_param->flash_mode = RK_AIQ_FLASH_MODE_TORCH;
            fl_param->power[0] = 1000;
            fl_param->strobe = true;
        } else {
            fl_param->flash_mode = RK_AIQ_FLASH_MODE_OFF;
            fl_param->power[0] = 0;
            fl_param->strobe = false;
        }
#endif
    }

    EXIT_ANALYZER_FUNCTION();

    return ret;
}

XCamReturn
RkAiqCore::genIspAcpResult(RkAiqFullParams* params)
{
    ENTER_ANALYZER_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    RkAiqAlgoProcResAcp* acp_com =
        mAlogsSharedParams.procResComb.acp_proc_res;
    rk_aiq_isp_params_v20_t* isp_param =
        static_cast<rk_aiq_isp_params_v20_t*>(params->mIspParams->data().ptr());

    if (!acp_com) {
        LOGD_ANALYZER("no acp result");
        return XCAM_RETURN_NO_ERROR;
    }

    // TODO: gen acp common result
    rk_aiq_acp_params_t* isp_cp = &isp_param->cp;

    *isp_cp = acp_com->acp_res;

    SmartPtr<RkAiqHandle>* handle = getCurAlgoTypeHandle(RK_AIQ_ALGO_TYPE_ACP);
    int algo_id = (*handle)->getAlgoId();

    // gen rk acp result
    if (algo_id == 0) {
        RkAiqAlgoProcResAcpInt* acp_rk = (RkAiqAlgoProcResAcpInt*)acp_com;

#ifdef RK_SIMULATOR_HW
#else
#endif
    }

    EXIT_ANALYZER_FUNCTION();

    return ret;
}

XCamReturn
RkAiqCore::genIspAieResult(RkAiqFullParams* params)
{
    ENTER_ANALYZER_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    RkAiqAlgoProcResAie* aie_com =
        mAlogsSharedParams.procResComb.aie_proc_res;
    rk_aiq_isp_params_v20_t* isp_param =
        static_cast<rk_aiq_isp_params_v20_t*>(params->mIspParams->data().ptr());

    if (!aie_com) {
        LOGD_ANALYZER("no aie result");
        return XCAM_RETURN_NO_ERROR;
    }

    // TODO: gen aie common result
    rk_aiq_isp_ie_t* isp_ie = &isp_param->ie;
    isp_ie->base = aie_com->params;
    SmartPtr<RkAiqHandle>* handle = getCurAlgoTypeHandle(RK_AIQ_ALGO_TYPE_AIE);
    int algo_id = (*handle)->getAlgoId();

    // gen rk aie result
    if (algo_id == 0) {
        RkAiqAlgoProcResAieInt* aie_rk = (RkAiqAlgoProcResAieInt*)aie_com;

        isp_ie->extra = aie_rk->params;
#ifdef RK_SIMULATOR_HW
#else
#endif
    }

    EXIT_ANALYZER_FUNCTION();

    return ret;
}

XCamReturn
RkAiqCore::genIspAsharpResult(RkAiqFullParams* params)
{
    ENTER_ANALYZER_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    RkAiqAlgoProcResAsharp* asharp_com =
        mAlogsSharedParams.procResComb.asharp_proc_res;
    rk_aiq_isp_params_v20_t* isp_param =
        static_cast<rk_aiq_isp_params_v20_t*>(params->mIspParams->data().ptr());

    if (!asharp_com || !params->mIsppParams.ptr()) {
        LOGD_ANALYZER("no asharp result");
        return XCAM_RETURN_NO_ERROR;
    }

    SmartPtr<rk_aiq_ispp_params_t> ispp_param =
        params->mIsppParams->data();

    // TODO: gen asharp common result

    SmartPtr<RkAiqHandle>* handle = getCurAlgoTypeHandle(RK_AIQ_ALGO_TYPE_ASHARP);
    int algo_id = (*handle)->getAlgoId();

    // gen rk asharp result
    if (algo_id == 0) {
        RkAiqAlgoProcResAsharpInt* asharp_rk = (RkAiqAlgoProcResAsharpInt*)asharp_com;

#ifdef RK_SIMULATOR_HW
        LOGD_ASHARP("oyyf: %s:%d output isp param start\n", __FUNCTION__, __LINE__);
        memcpy(&isp_param->rkaiq_asharp_proc_res.stSharpParamSelect.rk_sharpen_params_selected_V1,
               &asharp_rk->stAsharpProcResult.stSharpParamSelect.rk_sharpen_params_selected_V1,
               sizeof(RKAsharp_Sharp_HW_Params_Select_t));

        memcpy(&isp_param->rkaiq_asharp_proc_res.stSharpParamSelect.rk_sharpen_params_selected_V2,
               &asharp_rk->stAsharpProcResult.stSharpParamSelect.rk_sharpen_params_selected_V2,
               sizeof(RKAsharp_Sharp_HW_V2_Params_Select_t));

        memcpy(&isp_param->rkaiq_asharp_proc_res.stSharpParamSelect.rk_sharpen_params_selected_V3,
               &asharp_rk->stAsharpProcResult.stSharpParamSelect.rk_sharpen_params_selected_V3,
               sizeof(RKAsharp_Sharp_HW_V3_Params_Select_t));

        memcpy(&isp_param->rkaiq_asharp_proc_res.stEdgefilterParamSelect,
               &asharp_rk->stAsharpProcResult.stEdgefilterParamSelect,
               sizeof(RKAsharp_EdgeFilter_Params_Select_t));

        LOGD_ASHARP("oyyf: %s:%d output isp param end \n", __FUNCTION__, __LINE__);
#else
        LOGD_ASHARP("oyyf: %s:%d output isp param start\n", __FUNCTION__, __LINE__);

        ispp_param->update_mask |= RKAIQ_ISPP_SHARP_ID;
        memcpy(&ispp_param->sharpen,
               &asharp_rk->stAsharpProcResult.stSharpFix,
               sizeof(rk_aiq_isp_sharpen_t));

        memcpy(&ispp_param->edgeflt,
               &asharp_rk->stAsharpProcResult.stEdgefltFix,
               sizeof(rk_aiq_isp_edgeflt_t));

        LOGD_ASHARP("oyyf: %s:%d output isp param end \n", __FUNCTION__, __LINE__);
#endif
    }

    EXIT_ANALYZER_FUNCTION();

    return ret;
}

XCamReturn
RkAiqCore::genIspA3dlutResult(RkAiqFullParams* params)
{
    ENTER_ANALYZER_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    RkAiqAlgoProcResA3dlut* a3dlut_com =
        mAlogsSharedParams.procResComb.a3dlut_proc_res;
    rk_aiq_isp_params_v20_t* isp_param =
        static_cast<rk_aiq_isp_params_v20_t*>(params->mIspParams->data().ptr());

    if (!a3dlut_com) {
        LOGD_ANALYZER("no a3dlut result");
        return XCAM_RETURN_NO_ERROR;
    }

    // TODO: gen a3dlut common result
    RkAiqAlgoProcResA3dlut* a3dlut_rk = (RkAiqAlgoProcResA3dlut*)a3dlut_com;
    isp_param->lut3d = a3dlut_rk->lut3d_hw_conf;

    SmartPtr<RkAiqHandle>* handle = getCurAlgoTypeHandle(RK_AIQ_ALGO_TYPE_A3DLUT);
    int algo_id = (*handle)->getAlgoId();

    // gen rk a3dlut result
    if (algo_id == 0) {
        RkAiqAlgoProcResA3dlutInt* a3dlut_rk_int = (RkAiqAlgoProcResA3dlutInt*)a3dlut_com;

#ifdef RK_SIMULATOR_HW
#else
#endif
    }

    EXIT_ANALYZER_FUNCTION();

    return ret;
}

XCamReturn
RkAiqCore::genIspAblcResult(RkAiqFullParams* params)
{
    ENTER_ANALYZER_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    RkAiqAlgoProcResAblc* ablc_com =
        mAlogsSharedParams.procResComb.ablc_proc_res;
    rk_aiq_isp_params_v20_t* isp_param =
        static_cast<rk_aiq_isp_params_v20_t*>(params->mIspParams->data().ptr());

    if (!ablc_com) {
        LOGD_ANALYZER("no ablc result");
        return XCAM_RETURN_NO_ERROR;
    }

    // TODO: gen ablc common result

    SmartPtr<RkAiqHandle>* handle = getCurAlgoTypeHandle(RK_AIQ_ALGO_TYPE_ABLC);
    int algo_id = (*handle)->getAlgoId();

    // gen rk ablc result
    if (algo_id == 0) {
        RkAiqAlgoProcResAblcInt* ablc_rk = (RkAiqAlgoProcResAblcInt*)ablc_com;

        memcpy(&isp_param->blc, &ablc_rk->ablc_proc_res,
               sizeof(rk_aiq_isp_blc_t));
    }

    EXIT_ANALYZER_FUNCTION();

    return ret;
}

XCamReturn
RkAiqCore::genIspAccmResult(RkAiqFullParams* params)
{
    ENTER_ANALYZER_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    RkAiqAlgoProcResAccm* accm_com =
        mAlogsSharedParams.procResComb.accm_proc_res;
    rk_aiq_isp_params_v20_t* isp_param =
        static_cast<rk_aiq_isp_params_v20_t*>(params->mIspParams->data().ptr());

    if (!accm_com) {
        LOGD_ANALYZER("no accm result");
        return XCAM_RETURN_NO_ERROR;
    }

    // TODO: gen accm common result
    RkAiqAlgoProcResAccm* accm_rk = (RkAiqAlgoProcResAccm*)accm_com;
    isp_param->ccm = accm_rk->accm_hw_conf;

    SmartPtr<RkAiqHandle>* handle = getCurAlgoTypeHandle(RK_AIQ_ALGO_TYPE_ACCM);
    int algo_id = (*handle)->getAlgoId();

    // gen rk accm result
    if (algo_id == 0) {
        RkAiqAlgoProcResAccmInt* accm_rk_int = (RkAiqAlgoProcResAccmInt*)accm_com;
    }

    EXIT_ANALYZER_FUNCTION();

    return ret;
}

XCamReturn
RkAiqCore::genIspAcgcResult(RkAiqFullParams* params)
{
    ENTER_ANALYZER_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    RkAiqAlgoProcResAcgc* acgc_com =
        mAlogsSharedParams.procResComb.acgc_proc_res;
    rk_aiq_isp_params_v20_t* isp_param =
        static_cast<rk_aiq_isp_params_v20_t*>(params->mIspParams->data().ptr());

    if (!acgc_com) {
        LOGD_ANALYZER("no acgc result");
        return XCAM_RETURN_NO_ERROR;
    }

    // TODO: gen acgc common result

    SmartPtr<RkAiqHandle>* handle = getCurAlgoTypeHandle(RK_AIQ_ALGO_TYPE_ACGC);
    int algo_id = (*handle)->getAlgoId();

    // gen rk acgc result
    if (algo_id == 0) {
        RkAiqAlgoProcResAcgcInt* acgc_rk = (RkAiqAlgoProcResAcgcInt*)acgc_com;

#ifdef RK_SIMULATOR_HW
#else
#endif
    }

    EXIT_ANALYZER_FUNCTION();

    return ret;
}

XCamReturn
RkAiqCore::genIspAdebayerResult(RkAiqFullParams* params)
{
    ENTER_ANALYZER_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    RkAiqAlgoProcResAdebayer* adebayer_com =
        mAlogsSharedParams.procResComb.adebayer_proc_res;
    rk_aiq_isp_params_v20_t* isp_param =
        static_cast<rk_aiq_isp_params_v20_t*>(params->mIspParams->data().ptr());

    if (!adebayer_com) {
        LOGD_ANALYZER("no adebayer result");
        return XCAM_RETURN_NO_ERROR;
    }

    // TODO: gen adebayer common result

    SmartPtr<RkAiqHandle>* handle = getCurAlgoTypeHandle(RK_AIQ_ALGO_TYPE_ADEBAYER);
    int algo_id = (*handle)->getAlgoId();

    // gen rk adebayer result
    if (algo_id == 0) {
        RkAiqAlgoProcResAdebayerInt* adebayer_rk = (RkAiqAlgoProcResAdebayerInt*)adebayer_com;
        memcpy(&isp_param->demosaic, &adebayer_rk->debayerRes.config, sizeof(AdebayerConfig_t));
    }

    EXIT_ANALYZER_FUNCTION();

    return ret;
}

XCamReturn
RkAiqCore::genIspAdpccResult(RkAiqFullParams* params)
{
    ENTER_ANALYZER_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    RkAiqAlgoProcResAdpcc* adpcc_com =
        mAlogsSharedParams.procResComb.adpcc_proc_res;
    rk_aiq_isp_params_v20_t* isp_param =
        static_cast<rk_aiq_isp_params_v20_t*>(params->mIspParams->data().ptr());
    SmartPtr<rk_aiq_exposure_params_wrapper_t> exp_param =
        params->mExposureParams->data();

    if (!adpcc_com) {
        LOGD_ANALYZER("no adpcc result");
        return XCAM_RETURN_NO_ERROR;
    }

    // TODO: gen adpcc common result

    SmartPtr<RkAiqHandle>* handle = getCurAlgoTypeHandle(RK_AIQ_ALGO_TYPE_ADPCC);
    int algo_id = (*handle)->getAlgoId();

    // gen rk adpcc result
    exp_param->SensorDpccInfo.enable = adpcc_com->SenDpccRes.enable;
    exp_param->SensorDpccInfo.cur_single_dpcc = adpcc_com->SenDpccRes.cur_single_dpcc;
    exp_param->SensorDpccInfo.cur_multiple_dpcc = adpcc_com->SenDpccRes.cur_multiple_dpcc;
    exp_param->SensorDpccInfo.total_dpcc = adpcc_com->SenDpccRes.total_dpcc;

    if (algo_id == 0) {
        RkAiqAlgoProcResAdpccInt* adpcc_rk = (RkAiqAlgoProcResAdpccInt*)adpcc_com;

        LOGD_ADPCC("oyyf: %s:%d output dpcc param start\n", __FUNCTION__, __LINE__);
        memcpy(&isp_param->dpcc,
               &adpcc_rk->stAdpccProcResult,
               sizeof(rk_aiq_isp_dpcc_t));
        LOGD_ADPCC("oyyf: %s:%d output dpcc param end\n", __FUNCTION__, __LINE__);
    }

    EXIT_ANALYZER_FUNCTION();

    return ret;
}

XCamReturn
RkAiqCore::genIspAfecResult(RkAiqFullParams* params)
{
    ENTER_ANALYZER_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    RkAiqAlgoProcResAfec* afec_com =
        mAlogsSharedParams.procResComb.afec_proc_res;

    if (!afec_com || !params->mIsppParams.ptr()) {
        LOGD_ANALYZER("no afec result");
        return XCAM_RETURN_NO_ERROR;
    }

    SmartPtr<rk_aiq_ispp_params_t> ispp_param =
        params->mIsppParams->data();

    // TODO: gen afec common result

    SmartPtr<RkAiqHandle>* handle = getCurAlgoTypeHandle(RK_AIQ_ALGO_TYPE_AFEC);
    int algo_id = (*handle)->getAlgoId();
    // gen rk afec result
    if (algo_id == 0) {
        RkAiqAlgoProcResAfecInt* afec_rk = (RkAiqAlgoProcResAfecInt*)afec_com;

        if (afec_rk->afec_result.update) {
            ispp_param->update_mask |= RKAIQ_ISPP_FEC_ID;
            ispp_param->fec.fec_en = afec_rk->afec_result.sw_fec_en;
            if (ispp_param->fec.fec_en) {
                ispp_param->fec.crop_en = afec_rk->afec_result.crop_en;
                ispp_param->fec.crop_width = afec_rk->afec_result.crop_width;
                ispp_param->fec.crop_height = afec_rk->afec_result.crop_height;
                ispp_param->fec.mesh_density = afec_rk->afec_result.mesh_density;
                ispp_param->fec.mesh_size = afec_rk->afec_result.mesh_size;
                ispp_param->fec.mesh_buf_fd = afec_rk->afec_result.mesh_buf_fd;
                //memcpy(ispp_param->fec.sw_mesh_xi, afec_rk->afec_result.meshxi, sizeof(ispp_param->fec.sw_mesh_xi));
                //memcpy(ispp_param->fec.sw_mesh_xf, afec_rk->afec_result.meshxf, sizeof(ispp_param->fec.sw_mesh_xf));
                //memcpy(ispp_param->fec.sw_mesh_yi, afec_rk->afec_result.meshyi, sizeof(ispp_param->fec.sw_mesh_yi));
                //memcpy(ispp_param->fec.sw_mesh_yf, afec_rk->afec_result.meshyf, sizeof(ispp_param->fec.sw_mesh_yf));
            }
        } else
            ispp_param->update_mask &= ~RKAIQ_ISPP_FEC_ID;
    }

    EXIT_ANALYZER_FUNCTION();

    return ret;
}

XCamReturn
RkAiqCore::genIspAgammaResult(RkAiqFullParams* params)
{
    ENTER_ANALYZER_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    RkAiqAlgoProcResAgamma* agamma_com =
        mAlogsSharedParams.procResComb.agamma_proc_res;
    rk_aiq_isp_params_v20_t* isp_param =
        static_cast<rk_aiq_isp_params_v20_t*>(params->mIspParams->data().ptr());

    if (!agamma_com) {
        LOGD_ANALYZER("no agamma result");
        return XCAM_RETURN_NO_ERROR;
    }

    // TODO: gen agamma common result
    RkAiqAlgoProcResAgamma* agamma_rk = (RkAiqAlgoProcResAgamma*)agamma_com;

    isp_param->agamma = agamma_rk->agamma_proc_res;

    SmartPtr<RkAiqHandle>* handle = getCurAlgoTypeHandle(RK_AIQ_ALGO_TYPE_AGAMMA);
    int algo_id = (*handle)->getAlgoId();

    // gen rk agamma result
    if (algo_id == 0) {
        RkAiqAlgoProcResAgammaInt* agamma_rk = (RkAiqAlgoProcResAgammaInt*)agamma_com;
    }

    EXIT_ANALYZER_FUNCTION();

    return ret;
}

XCamReturn
RkAiqCore::genIspAgicResult(RkAiqFullParams* params)
{
    ENTER_ANALYZER_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    RkAiqAlgoProcResAgic* agic_com =
        mAlogsSharedParams.procResComb.agic_proc_res;
    rk_aiq_isp_params_v20_t* isp_param =
        static_cast<rk_aiq_isp_params_v20_t*>(params->mIspParams->data().ptr());

    if (!agic_com) {
        LOGD_ANALYZER("no agic result");
        return XCAM_RETURN_NO_ERROR;
    }

    // TODO: gen agic common result

    SmartPtr<RkAiqHandle>* handle = getCurAlgoTypeHandle(RK_AIQ_ALGO_TYPE_AGIC);
    int algo_id = (*handle)->getAlgoId();

    // gen rk agic result
    if (algo_id == 0) {
        RkAiqAlgoProcResAgicInt* agic_rk = (RkAiqAlgoProcResAgicInt*)agic_com;
        memcpy(&isp_param->gic, &agic_rk->gicRes, sizeof(AgicProcResult_t));
    }

    EXIT_ANALYZER_FUNCTION();

    return ret;
}

XCamReturn
RkAiqCore::genIspAldchResult(RkAiqFullParams* params)
{
    ENTER_ANALYZER_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    RkAiqAlgoProcResAldch* aldch_com =
        mAlogsSharedParams.procResComb.aldch_proc_res;
    rk_aiq_isp_params_v20_t* isp_param =
        static_cast<rk_aiq_isp_params_v20_t*>(params->mIspParams->data().ptr());

    if (!aldch_com) {
        LOGD_ANALYZER("no aldch result");
        return XCAM_RETURN_NO_ERROR;
    }

    RkAiqAlgoProcResAldchInt* aldch_rk = (RkAiqAlgoProcResAldchInt*)aldch_com;
    if (aldch_rk->ldch_result.update) {
        isp_param->update_mask |= RKAIQ_ISP_LDCH_ID;
        isp_param->ldch.ldch_en = aldch_rk->ldch_result.sw_ldch_en;
        if (isp_param->ldch.ldch_en) {
            isp_param->ldch.lut_h_size = aldch_rk->ldch_result.lut_h_size;
            isp_param->ldch.lut_v_size = aldch_rk->ldch_result.lut_v_size;
            isp_param->ldch.lut_size = aldch_rk->ldch_result.lut_map_size;
            isp_param->ldch.lut_mem_fd = aldch_rk->ldch_result.lut_mapxy_buf_fd;
        }
    } else {
        isp_param->update_mask &= ~RKAIQ_ISP_LDCH_ID;
    }

    SmartPtr<RkAiqHandle>* handle = getCurAlgoTypeHandle(RK_AIQ_ALGO_TYPE_ALDCH);
    int algo_id = (*handle)->getAlgoId();

    // gen rk aldch result
    if (algo_id == 0) {
        RkAiqAlgoProcResAldchInt* aldch_rk = (RkAiqAlgoProcResAldchInt*)aldch_com;
    }

    EXIT_ANALYZER_FUNCTION();

    return ret;
}

XCamReturn
RkAiqCore::genIspAlscResult(RkAiqFullParams* params)
{
    ENTER_ANALYZER_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    RkAiqAlgoProcResAlsc* alsc_com =
        mAlogsSharedParams.procResComb.alsc_proc_res;
    rk_aiq_isp_params_v20_t* isp_param =
        static_cast<rk_aiq_isp_params_v20_t*>(params->mIspParams->data().ptr());

    if (!alsc_com) {
        LOGD_ANALYZER("no alsc result");
        return XCAM_RETURN_NO_ERROR;
    }
    // TODO: gen alsc common result
    RkAiqAlgoProcResAlsc* alsc_rk = (RkAiqAlgoProcResAlsc*)alsc_com;
    isp_param->lsc = alsc_rk->alsc_hw_conf;
    if(mAlogsSharedParams.sns_mirror) {
        for(int i = 0; i < LSC_DATA_TBL_V_SIZE; i++) {
            for(int j = 0; j < LSC_DATA_TBL_H_SIZE; j++) {
                SWAP(unsigned short, isp_param->lsc.r_data_tbl[i * LSC_DATA_TBL_H_SIZE + j], isp_param->lsc.r_data_tbl[i * LSC_DATA_TBL_H_SIZE + (LSC_DATA_TBL_H_SIZE - 1 - j)]);
                SWAP(unsigned short, isp_param->lsc.gr_data_tbl[i * LSC_DATA_TBL_H_SIZE + j], isp_param->lsc.gr_data_tbl[i * LSC_DATA_TBL_H_SIZE + (LSC_DATA_TBL_H_SIZE - 1 - j)]);
                SWAP(unsigned short, isp_param->lsc.gb_data_tbl[i * LSC_DATA_TBL_H_SIZE + j], isp_param->lsc.gb_data_tbl[i * LSC_DATA_TBL_H_SIZE + (LSC_DATA_TBL_H_SIZE - 1 - j)]);
                SWAP(unsigned short, isp_param->lsc.b_data_tbl[i * LSC_DATA_TBL_H_SIZE + j], isp_param->lsc.b_data_tbl[i * LSC_DATA_TBL_H_SIZE + (LSC_DATA_TBL_H_SIZE - 1 - j)]);
            }
        }
    }
    if(mAlogsSharedParams.sns_flip) {
        for(int i = 0; i < LSC_DATA_TBL_V_SIZE; i++) {
            for(int j = 0; j < LSC_DATA_TBL_H_SIZE; j++) {
                SWAP(unsigned short, isp_param->lsc.r_data_tbl[i * LSC_DATA_TBL_H_SIZE + j], isp_param->lsc.r_data_tbl[(LSC_DATA_TBL_V_SIZE - 1 - i)*LSC_DATA_TBL_H_SIZE + j]);
                SWAP(unsigned short, isp_param->lsc.gr_data_tbl[i * LSC_DATA_TBL_H_SIZE + j], isp_param->lsc.gr_data_tbl[(LSC_DATA_TBL_V_SIZE - 1 - i)*LSC_DATA_TBL_H_SIZE + j]);
                SWAP(unsigned short, isp_param->lsc.gb_data_tbl[i * LSC_DATA_TBL_H_SIZE + j], isp_param->lsc.gb_data_tbl[(LSC_DATA_TBL_V_SIZE - 1 - i)*LSC_DATA_TBL_H_SIZE + j]);
                SWAP(unsigned short, isp_param->lsc.b_data_tbl[i * LSC_DATA_TBL_H_SIZE + j], isp_param->lsc.b_data_tbl[(LSC_DATA_TBL_V_SIZE - 1 - i)*LSC_DATA_TBL_H_SIZE + j]);
            }
        }
    }
    SmartPtr<RkAiqHandle>* handle = getCurAlgoTypeHandle(RK_AIQ_ALGO_TYPE_ALSC);
    int algo_id = (*handle)->getAlgoId();

    // gen rk alsc result
    if (algo_id == 0) {
        RkAiqAlgoProcResAlscInt* alsc_rk_int = (RkAiqAlgoProcResAlscInt*)alsc_com;
    }

    EXIT_ANALYZER_FUNCTION();

    return ret;
}

XCamReturn
RkAiqCore::genIspAorbResult(RkAiqFullParams* params)
{
    ENTER_ANALYZER_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    RkAiqAlgoProcResAorb* aorb_com =
        mAlogsSharedParams.procResComb.aorb_proc_res;

    if (!aorb_com || !params->mIsppParams.ptr()) {
        LOGD_ANALYZER("no aorb result");
        return XCAM_RETURN_NO_ERROR;
    }

    SmartPtr<rk_aiq_ispp_params_t> ispp_param =
        params->mIsppParams->data();

    // TODO: gen aorb common result

    SmartPtr<RkAiqHandle>* handle = getCurAlgoTypeHandle(RK_AIQ_ALGO_TYPE_AORB);
    int algo_id = (*handle)->getAlgoId();

    // gen rk aorb result
    if (algo_id == 0) {
        RkAiqAlgoProcResAorbInt* aorb_rk = (RkAiqAlgoProcResAorbInt*)aorb_com;

        if (aorb_rk->aorb_meas.update) {
            ispp_param->update_mask |= RKAIQ_ISPP_ORB_ID;
            ispp_param->orb.orb_en = aorb_rk->aorb_meas.orb_en;
            if (ispp_param->orb.orb_en) {
                ispp_param->orb.limit_value = aorb_rk->aorb_meas.limit_value;
                ispp_param->orb.max_feature = aorb_rk->aorb_meas.max_feature;
            }
        } else {
            ispp_param->update_mask &= ~RKAIQ_ISPP_ORB_ID;
        }
    }

    EXIT_ANALYZER_FUNCTION();

    return ret;
}

XCamReturn
RkAiqCore::genIspAr2yResult(RkAiqFullParams* params)
{
    ENTER_ANALYZER_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    RkAiqAlgoProcResAr2y* ar2y_com =
        mAlogsSharedParams.procResComb.ar2y_proc_res;
    SmartPtr<rk_aiq_isp_params_t> isp_param =
        params->mIspParams->data();

    if (!ar2y_com) {
        LOGD_ANALYZER("no ar2y result");
        return XCAM_RETURN_NO_ERROR;
    }

    // TODO: gen ar2y common result

    SmartPtr<RkAiqHandle>* handle = getCurAlgoTypeHandle(RK_AIQ_ALGO_TYPE_AR2Y);
    int algo_id = (*handle)->getAlgoId();

    // gen rk ar2y result
    if (algo_id == 0) {
        RkAiqAlgoProcResAr2yInt* ar2y_rk = (RkAiqAlgoProcResAr2yInt*)ar2y_com;

#ifdef RK_SIMULATOR_HW
#else
#endif
    }

    EXIT_ANALYZER_FUNCTION();

    return ret;
}

XCamReturn
RkAiqCore::genIspAwdrResult(RkAiqFullParams* params)
{
    ENTER_ANALYZER_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    RkAiqAlgoProcResAwdr* awdr_com =
        mAlogsSharedParams.procResComb.awdr_proc_res;
    rk_aiq_isp_params_v20_t* isp_param =
        static_cast<rk_aiq_isp_params_v20_t*>(params->mIspParams->data().ptr());

    if (!awdr_com) {
        LOGD_ANALYZER("no awdr result");
        return XCAM_RETURN_NO_ERROR;
    }

    // TODO: gen awdr common result

    SmartPtr<RkAiqHandle>* handle = getCurAlgoTypeHandle(RK_AIQ_ALGO_TYPE_ADEBAYER);
    int algo_id = (*handle)->getAlgoId();

    // gen rk awdr result
    if (algo_id == 0) {
        RkAiqAlgoProcResAwdrInt* awdr_rk = (RkAiqAlgoProcResAwdrInt*)awdr_com;

#ifdef RK_SIMULATOR_HW
#else
#endif
    }

    EXIT_ANALYZER_FUNCTION();

    return ret;
}

XCamReturn
RkAiqCore::genIspArawnrResult(RkAiqFullParams* params)
{
    ENTER_ANALYZER_FUNCTION();

    EXIT_ANALYZER_FUNCTION();

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
RkAiqCore::genIspAmfnrResult(RkAiqFullParams* params)
{
    ENTER_ANALYZER_FUNCTION();

    EXIT_ANALYZER_FUNCTION();

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
RkAiqCore::genIspAynrResult(RkAiqFullParams* params)
{
    ENTER_ANALYZER_FUNCTION();

    EXIT_ANALYZER_FUNCTION();

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
RkAiqCore::genIspAcnrResult(RkAiqFullParams* params)
{
    ENTER_ANALYZER_FUNCTION();

    EXIT_ANALYZER_FUNCTION();

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
RkAiqCore::genIspAdrcResult(RkAiqFullParams* params)
{
    ENTER_ANALYZER_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    RkAiqAlgoProcResAdrc* adrc_com =
        mAlogsSharedParams.procResComb.adrc_proc_res;
    rk_aiq_isp_params_v20_t* isp_param =
        static_cast<rk_aiq_isp_params_v20_t*>(params->mIspParams->data().ptr());

    if (!adrc_com) {
        LOGD_ANALYZER("no adrc result");
        return XCAM_RETURN_NO_ERROR;
    }

    // TODO: gen ahdr common result

    SmartPtr<RkAiqHandle>* handle = getCurAlgoTypeHandle(RK_AIQ_ALGO_TYPE_ADRC);
    int algo_id = (*handle)->getAlgoId();

    // gen rk ahdr result
    if (algo_id == 0) {
        RkAiqAlgoProcResAdrcInt* ahdr_rk = (RkAiqAlgoProcResAdrcInt*)adrc_com;


        isp_param->drc.DrcProcRes.sw_drc_offset_pow2     = ahdr_rk->AdrcProcRes.DrcProcRes.sw_drc_offset_pow2;
        isp_param->drc.DrcProcRes.sw_drc_compres_scl  = ahdr_rk->AdrcProcRes.DrcProcRes.sw_drc_compres_scl;
        isp_param->drc.DrcProcRes.sw_drc_position  = ahdr_rk->AdrcProcRes.DrcProcRes.sw_drc_position;
        isp_param->drc.DrcProcRes.sw_drc_delta_scalein        = ahdr_rk->AdrcProcRes.DrcProcRes.sw_drc_delta_scalein;
        isp_param->drc.DrcProcRes.sw_drc_hpdetail_ratio      = ahdr_rk->AdrcProcRes.DrcProcRes.sw_drc_hpdetail_ratio;
        isp_param->drc.DrcProcRes.sw_drc_lpdetail_ratio     = ahdr_rk->AdrcProcRes.DrcProcRes.sw_drc_lpdetail_ratio;
        isp_param->drc.DrcProcRes.sw_drc_weicur_pix      = ahdr_rk->AdrcProcRes.DrcProcRes.sw_drc_weicur_pix;
        isp_param->drc.DrcProcRes.sw_drc_weipre_frame  = ahdr_rk->AdrcProcRes.DrcProcRes.sw_drc_weipre_frame;
        isp_param->drc.DrcProcRes.sw_drc_force_sgm_inv0   = ahdr_rk->AdrcProcRes.DrcProcRes.sw_drc_force_sgm_inv0;
        isp_param->drc.DrcProcRes.sw_drc_motion_scl     = ahdr_rk->AdrcProcRes.DrcProcRes.sw_drc_motion_scl;
        isp_param->drc.DrcProcRes.sw_drc_edge_scl   = ahdr_rk->AdrcProcRes.DrcProcRes.sw_drc_edge_scl;
        isp_param->drc.DrcProcRes.sw_drc_space_sgm_inv1    = ahdr_rk->AdrcProcRes.DrcProcRes.sw_drc_space_sgm_inv1;
        isp_param->drc.DrcProcRes.sw_drc_space_sgm_inv0     = ahdr_rk->AdrcProcRes.DrcProcRes.sw_drc_range_sgm_inv1;
        isp_param->drc.DrcProcRes.sw_drc_range_sgm_inv1     = ahdr_rk->AdrcProcRes.DrcProcRes.sw_drc_range_sgm_inv0;
        isp_param->drc.DrcProcRes.sw_drc_range_sgm_inv0 = ahdr_rk->AdrcProcRes.DrcProcRes.sw_drc_space_sgm_inv0;
        isp_param->drc.DrcProcRes.sw_drc_weig_maxl    = ahdr_rk->AdrcProcRes.DrcProcRes.sw_drc_weig_maxl;
        isp_param->drc.DrcProcRes.sw_drc_weig_bilat  = ahdr_rk->AdrcProcRes.DrcProcRes.sw_drc_weig_bilat;
        isp_param->drc.DrcProcRes.sw_drc_iir_weight  = ahdr_rk->AdrcProcRes.DrcProcRes.sw_drc_iir_weight;
        isp_param->drc.DrcProcRes.sw_drc_min_ogain  = ahdr_rk->AdrcProcRes.DrcProcRes.sw_drc_min_ogain;
        for(int i = 0; i < 17; i++) {
            isp_param->drc.DrcProcRes.sw_drc_gain_y[i]    = ahdr_rk->AdrcProcRes.DrcProcRes.sw_drc_gain_y[i];
            isp_param->drc.DrcProcRes.sw_drc_compres_y[i]    = ahdr_rk->AdrcProcRes.DrcProcRes.sw_drc_compres_y[i];
            isp_param->drc.DrcProcRes.sw_drc_scale_y[i]    = ahdr_rk->AdrcProcRes.DrcProcRes.sw_drc_scale_y[i];
        }


        isp_param->drc.isHdrGlobalTmo =
            ahdr_rk->AdrcProcRes.isHdrGlobalTmo;

        isp_param->drc.bTmoEn =
            ahdr_rk->AdrcProcRes.bTmoEn;

        isp_param->drc.isLinearTmo =
            ahdr_rk->AdrcProcRes.isLinearTmo;

        LOGE("sw_drc_offset_pow2 %d", isp_param->drc.DrcProcRes.sw_drc_offset_pow2);
        LOGE("sw_drc_compres_scl %d", isp_param->drc.DrcProcRes.sw_drc_compres_scl);
        LOGE("sw_drc_position %d", isp_param->drc.DrcProcRes.sw_drc_position);
        LOGE("sw_drc_delta_scalein %d", isp_param->drc.DrcProcRes.sw_drc_delta_scalein);
        LOGE("sw_drc_hpdetail_ratio %d", isp_param->drc.DrcProcRes.sw_drc_hpdetail_ratio);
        LOGE("sw_drc_lpdetail_ratio %d", isp_param->drc.DrcProcRes.sw_drc_lpdetail_ratio);
        LOGE("sw_drc_weicur_pix %d", isp_param->drc.DrcProcRes.sw_drc_weicur_pix);


    }

    EXIT_ANALYZER_FUNCTION();

    return ret;
}


XCamReturn
RkAiqCore::pushStats(SmartPtr<VideoBuffer> &buffer)
{
    ENTER_ANALYZER_FUNCTION();

    XCAM_ASSERT(buffer.ptr());
    if (buffer->get_video_info().format == V4L2_META_FMT_RK_ISP1_STAT_3A)
        mRkAiqCoreTh->push_stats(buffer);
    else
        mRkAiqCorePpTh->push_stats(buffer);

    EXIT_ANALYZER_FUNCTION();

    return XCAM_RETURN_NO_ERROR;
}

const RkAiqHandle*
RkAiqCore::getAiqAlgoHandle(const int algo_type)
{
    SmartPtr<RkAiqHandle>* handlePtr = getCurAlgoTypeHandle(algo_type);

    return (*handlePtr).ptr();
}

SmartPtr<RkAiqHandle>*
RkAiqCore::getCurAlgoTypeHandle(int algo_type)
{
    std::map<int, SmartPtr<RkAiqHandle>>::iterator it =
                                          mCurAlgoHandleMaps.find(algo_type);

    if (it == mCurAlgoHandleMaps.end()) {
        return NULL;
    }

    return &mCurAlgoHandleMaps[algo_type];
}

std::map<int, SmartPtr<RkAiqHandle>>*
                                  RkAiqCore::getAlgoTypeHandleMap(int algo_type)
{
    std::map<int, map<int, SmartPtr<RkAiqHandle>>>::iterator it =
        mAlgoHandleMaps.find(algo_type);

    if (it == mAlgoHandleMaps.end()) {
        LOGE("can't find algo %d", algo_type);
        return NULL;
    }

    return &mAlgoHandleMaps[algo_type];
}

void
RkAiqCore::addDefaultAlgos(struct RkAiqAlgoDesCommExt* algoDes)
{
    map<int, SmartPtr<RkAiqHandle>> algoMap;
    for (int i = 0; i < RK_AIQ_ALGO_TYPE_MAX; i++) {
        mAlogsSharedParams.ctxCfigs[i].calib =
            const_cast<CamCalibDbContext_t*>(mAlogsSharedParams.calib);
        mAlogsSharedParams.ctxCfigs[i].cfg_com.isp_hw_version = mIspHwVer;
        mAlogsSharedParams.ctxCfigs[i].cfg_com.module_hw_version = algoDes[i].module_hw_ver;
    }

    for (size_t i = 0; algoDes[i].des != NULL; i++) {
        int algo_type = algoDes[i].des->type;
        mAlogsSharedParams.ctxCfigs[algo_type].calib =
            const_cast<CamCalibDbContext_t*>(mAlogsSharedParams.calib);
        mAlogsSharedParams.ctxCfigs[algo_type].cfg_com.module_hw_version = algoDes[i].module_hw_ver;
        algoMap[0] = newAlgoHandle(algoDes[i].des, false, algoDes[i].algo_ver);
        if (!algoMap[0].ptr()) {
            LOGE_ANALYZER("new algo_type %d handle failed", algo_type);
            continue;
        }
        mAlgoHandleMaps[algo_type] = algoMap;
        if (algo_type == RK_AIQ_ALGO_TYPE_AORB)
            mCurIsppAlgoHandleList.push_back(algoMap[0]);
        else
            mCurIspAlgoHandleList.push_back(algoMap[0]);
        mCurAlgoHandleMaps[algo_type] = algoMap[0];
        enableAlgo(algo_type, 0, true);
        algoMap.clear();
    }
}

SmartPtr<RkAiqHandle>
RkAiqCore::newAlgoHandle(RkAiqAlgoDesComm* algo, bool generic, int version)
{
#define NEW_ALGO_HANDLE(lc, BC) \
    if (algo->type == RK_AIQ_ALGO_TYPE_##BC) { \
        if (generic) { \
            if (version == 0) \
                return new RkAiq##lc##Handle(algo, this); \
        } else { \
            if (version == 0) \
                return new RkAiq##lc##HandleInt(algo, this); \
        }\
    }\

    NEW_ALGO_HANDLE(Ae, AE);
    NEW_ALGO_HANDLE(Awb, AWB);
    NEW_ALGO_HANDLE(Af, AF);
    NEW_ALGO_HANDLE(Ahdr, AHDR);
    NEW_ALGO_HANDLE(Anr, ANR);
    NEW_ALGO_HANDLE(Adhaz, ADHAZ);
    NEW_ALGO_HANDLE(Asd, ASD);
    NEW_ALGO_HANDLE(Acp, ACP);
    NEW_ALGO_HANDLE(Asharp, ASHARP);
    NEW_ALGO_HANDLE(A3dlut, A3DLUT);
    NEW_ALGO_HANDLE(Ablc, ABLC);
    NEW_ALGO_HANDLE(Accm, ACCM);
    NEW_ALGO_HANDLE(Acgc, ACGC);
    NEW_ALGO_HANDLE(Adebayer, ADEBAYER);
    NEW_ALGO_HANDLE(Adpcc, ADPCC);
    NEW_ALGO_HANDLE(Afec, AFEC);
    NEW_ALGO_HANDLE(Agamma, AGAMMA);
    NEW_ALGO_HANDLE(Agic, AGIC);
    NEW_ALGO_HANDLE(Aie, AIE);
    NEW_ALGO_HANDLE(Aldch, ALDCH);
    NEW_ALGO_HANDLE(Alsc, ALSC);
    NEW_ALGO_HANDLE(Aorb, AORB);
    NEW_ALGO_HANDLE(Ar2y, AR2Y);
    NEW_ALGO_HANDLE(Awdr, AWDR);
    NEW_ALGO_HANDLE(Arawnr, ARAWNR);
    NEW_ALGO_HANDLE(Amfnr, AMFNR);
    NEW_ALGO_HANDLE(Aynr, AYNR);
    NEW_ALGO_HANDLE(Acnr, ACNR);
    NEW_ALGO_HANDLE(Adrc, ADRC);

    return NULL;
}

XCamReturn
RkAiqCore::addAlgo(RkAiqAlgoDesComm& algo)
{
    ENTER_ANALYZER_FUNCTION();

    std::map<int, SmartPtr<RkAiqHandle>>* algo_map = getAlgoTypeHandleMap(algo.type);

    if (!algo_map) {
        LOGE_ANALYZER("do not support this algo type %d !", algo.type);
        return XCAM_RETURN_ERROR_FAILED;
    }
    // TODO, check if exist befor insert ?
    std::map<int, SmartPtr<RkAiqHandle>>::reverse_iterator rit = algo_map->rbegin();

    algo.id = rit->first + 1;

    int i = 0;
    struct RkAiqAlgoDesCommExt* def_des = NULL;
    while (mAlgosDesArray[i].des != NULL) {
        if (mAlgosDesArray[i].des->type == algo.type) {
            def_des = &mAlgosDesArray[i];
            break;
        }
        i++;
    }

    // add to map
    SmartPtr<RkAiqHandle> new_hdl = newAlgoHandle(&algo, true, def_des->algo_ver);
    new_hdl->setEnable(false);
    (*algo_map)[algo.id] = new_hdl;

    EXIT_ANALYZER_FUNCTION();

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
RkAiqCore::enableAlgo(int algoType, int id, bool enable)
{
    ENTER_ANALYZER_FUNCTION();
    // get current algotype handle, get id
    SmartPtr<RkAiqHandle>* cur_algo_hdl = getCurAlgoTypeHandle(algoType);
    std::map<int, SmartPtr<RkAiqHandle>>* algo_map = getAlgoTypeHandleMap(algoType);
    std::map<int, SmartPtr<RkAiqHandle>>::iterator it = algo_map->find(id);

    if (it == algo_map->end()) {
        LOGE_ANALYZER("can't find type id <%d, %d> algo", algoType, id);
        return XCAM_RETURN_ERROR_FAILED;
    }

    if (!cur_algo_hdl) {
        LOGE_ANALYZER("can't find current type %d algo", algoType);
        return XCAM_RETURN_ERROR_FAILED;
    }

    it->second->setEnable(enable);
    /* WARNING:
     * Be careful when use SmartPtr<RkAiqxxxHandle> = SmartPtr<RkAiqHandle>
     * if RkAiqxxxHandle is derived from multiple RkAiqHandle,
     * the ptr of RkAiqxxxHandle and RkAiqHandle IS NOT the same
     * (RkAiqHandle ptr = RkAiqxxxHandle ptr + offset), but seams like
     * SmartPtr do not deal with this correctly.
     */
    if (enable) {
        *cur_algo_hdl = it->second;
        for (auto& algo : mCurIspAlgoHandleList) {
            if (algo->getAlgoType() == algoType)
                algo = it->second;
        }
        for (auto& algo : mCurIsppAlgoHandleList) {
            if (algo->getAlgoType() == algoType)
                algo = it->second;
        }
    }

    EXIT_ANALYZER_FUNCTION();

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
RkAiqCore::rmAlgo(int algoType, int id)
{
    ENTER_ANALYZER_FUNCTION();

    // can't remove default algos
    if (id == 0)
        return XCAM_RETURN_NO_ERROR;

    SmartPtr<RkAiqHandle>* cur_algo_hdl = getCurAlgoTypeHandle(algoType);
    std::map<int, SmartPtr<RkAiqHandle>>* algo_map = getAlgoTypeHandleMap(algoType);
    std::map<int, SmartPtr<RkAiqHandle>>::iterator it = algo_map->find(id);

    if (it == algo_map->end()) {
        LOGE_ANALYZER("can't find type id <%d, %d> algo", algoType, id);
        return XCAM_RETURN_ERROR_FAILED;
    }

    if (!cur_algo_hdl) {
        LOGE_ANALYZER("can't find current type %d algo", algoType);
        return XCAM_RETURN_ERROR_FAILED;
    }

    // if it's the current algo handle, clear it
    if ((*cur_algo_hdl).ptr() == it->second.ptr()) {
        (*cur_algo_hdl).release();
        for (auto& algo : mCurIspAlgoHandleList) {
            if (algo->getAlgoType() == algoType)
                algo.release();
        }
        for (auto& algo : mCurIsppAlgoHandleList) {
            if (algo->getAlgoType() == algoType)
                algo.release();
        }
    }

    algo_map->erase(it);

    EXIT_ANALYZER_FUNCTION();

    return XCAM_RETURN_NO_ERROR;
}

bool
RkAiqCore::getAxlibStatus(int algoType, int id)
{
    std::map<int, SmartPtr<RkAiqHandle>>* algo_map = getAlgoTypeHandleMap(algoType);
    std::map<int, SmartPtr<RkAiqHandle>>::iterator it = algo_map->find(id);

    if (it == algo_map->end()) {
        LOGE_ANALYZER("can't find type id <%d, %d> algo", algoType, id);
        return false;
    }

    LOGD_ANALYZER("algo type id <%d,%d> status %s", algoType, id,
                  it->second->getEnable() ? "enable" : "disable");

    return it->second->getEnable();
}

const RkAiqAlgoContext*
RkAiqCore::getEnabledAxlibCtx(const int algo_type)
{
    if (algo_type <= RK_AIQ_ALGO_TYPE_NONE ||
            algo_type >= RK_AIQ_ALGO_TYPE_MAX)
        return NULL;

    SmartPtr<RkAiqHandle>* algo_handle = getCurAlgoTypeHandle(algo_type);

    if ((*algo_handle).ptr())
        return (*algo_handle)->getAlgoCtx();
    else
        return NULL;
}

void
RkAiqCore::copyIspStats(RkAiqIspStats* from, rk_aiq_isp_stats_t* to)
{
    to->aec_stats = from->aec_stats;
    to->awb_hw_ver = 0;
    to->awb_stats_v200 = from->awb_stats;
    to->af_stats = from->af_stats;
    to->frame_id = from->frame_id;
}

void
RkAiqCore::cacheIspStatsToList()
{
    SmartLock locker (ispStatsListMutex);
    SmartPtr<RkAiqStatsProxy> stats = NULL;
    if (mAiqStatsPool->has_free_items()) {
        stats = mAiqStatsPool->get_item();
    } else {
        if(mAiqStatsCachedList.empty()) {
            LOGW_ANALYZER("no free or cached stats, user may hold all stats buf !");
            return ;
        }
        stats = mAiqStatsCachedList.front();
        mAiqStatsCachedList.pop_front();
    }

    stats->data()->frame_id = mAlogsSharedParams.frameId;
    copyIspStats(&mAlogsSharedParams.ispStats, stats->data().ptr());

    mAiqStatsCachedList.push_back(stats);
    mIspStatsCond.broadcast ();
}

XCamReturn RkAiqCore::get3AStatsFromCachedList(rk_aiq_isp_stats_t **stats, int timeout_ms)
{
    SmartLock locker (ispStatsListMutex);
    int code = 0;
    while (mState != RK_AIQ_CORE_STATE_STOPED &&
            mAiqStatsCachedList.empty() &&
            code == 0) {
        if (timeout_ms < 0)
            code = mIspStatsCond.wait(ispStatsListMutex);
        else
            code = mIspStatsCond.timedwait(ispStatsListMutex, timeout_ms * 1000);
    }

    if (mState == RK_AIQ_CORE_STATE_STOPED) {
        *stats = NULL;
        return XCAM_RETURN_NO_ERROR;
    }

    if (mAiqStatsCachedList.empty()) {
        if (code == ETIMEDOUT) {
            *stats = NULL;
            return XCAM_RETURN_ERROR_TIMEOUT;
        } else {
            *stats = NULL;
            return XCAM_RETURN_ERROR_FAILED;
        }
    }
    SmartPtr<RkAiqStatsProxy> stats_proxy = mAiqStatsCachedList.front();
    mAiqStatsCachedList.pop_front();
    *stats = stats_proxy->data().ptr();
    mAiqStatsOutMap[*stats] = stats_proxy;
    stats_proxy.release();

    return XCAM_RETURN_NO_ERROR;
}

void RkAiqCore::release3AStatsRef(rk_aiq_isp_stats_t *stats)
{
    SmartLock locker (ispStatsListMutex);

    std::map<rk_aiq_isp_stats_t*, SmartPtr<RkAiqStatsProxy>>::iterator it;
    it = mAiqStatsOutMap.find(stats);
    if (it != mAiqStatsOutMap.end()) {
        mAiqStatsOutMap.erase(it);
    }
}

XCamReturn RkAiqCore::get3AStatsFromCachedList(rk_aiq_isp_stats_t &stats)
{
    SmartLock locker (ispStatsListMutex);
    if(!mAiqStatsCachedList.empty()) {
        SmartPtr<RkAiqStatsProxy> stats_proxy = mAiqStatsCachedList.front();
        mAiqStatsCachedList.pop_front();
        stats = *(stats_proxy->data().ptr());
        stats_proxy.release();
        return XCAM_RETURN_NO_ERROR;
    } else {
        return XCAM_RETURN_ERROR_FAILED;
    }
}

XCamReturn
RkAiqCore::convertIspstatsToAlgo(const SmartPtr<VideoBuffer> &buffer)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
#ifndef RK_SIMULATOR_HW
    const SmartPtr<Isp20StatsBuffer> buf =
        buffer.dynamic_cast_ptr<Isp20StatsBuffer>();
    struct rkisp_isp2x_stat_buffer *stats;

    SmartPtr<RkAiqExpParamsProxy> expParams = buf->get_exp_params();
    rk_aiq_isp_params_v20_t* ispParams =
        static_cast<rk_aiq_isp_params_v20_t*>(buf->get_isp_params()->data().ptr());
    SmartPtr<RkAiqAfInfoProxy> afParams = buf->get_af_params();
    SmartPtr<RkAiqIrisParamsProxy> irisParams = buf->get_iris_params();

    stats = (struct rkisp_isp2x_stat_buffer *)(buf->get_v4l2_userptr());
    if(stats == NULL) {
        LOGE("fail to get stats ,ignore\n");
        return XCAM_RETURN_BYPASS;
    }
    LOGI_ANALYZER("stats: frame_id: %d,  meas_type; 0x%x",
                  stats->frame_id, stats->meas_type);

    mAlogsSharedParams.frameId = stats->frame_id;

    if(ispParams == NULL) {
        LOGE("fail to get ispParams ,ignore\n");
        return XCAM_RETURN_BYPASS;
    }

    //awb2.0

    mAlogsSharedParams.ispStats.awb_cfg_effect_v200 = ispParams->awb_cfg;
    mAlogsSharedParams.ispStats.awb_cfg_effect_valid = true;

    for(int i = 0; i < mAlogsSharedParams.ispStats.awb_cfg_effect_v200.lightNum; i++) {
        mAlogsSharedParams.ispStats.awb_stats.light[i].xYType[RK_AIQ_AWB_XY_TYPE_NORMAL_V200].Rvalue =
            stats->params.rawawb.ro_rawawb_sum_r_nor[i];
        mAlogsSharedParams.ispStats.awb_stats.light[i].xYType[RK_AIQ_AWB_XY_TYPE_NORMAL_V200].Gvalue =
            stats->params.rawawb.ro_rawawb_sum_g_nor[i];
        mAlogsSharedParams.ispStats.awb_stats.light[i].xYType[RK_AIQ_AWB_XY_TYPE_NORMAL_V200].Bvalue =
            stats->params.rawawb.ro_rawawb_sum_b_nor[i];
        mAlogsSharedParams.ispStats.awb_stats.light[i].xYType[RK_AIQ_AWB_XY_TYPE_NORMAL_V200].WpNo =
            stats->params.rawawb.ro_rawawb_wp_num_nor[i];
        mAlogsSharedParams.ispStats.awb_stats.light[i].xYType[RK_AIQ_AWB_XY_TYPE_BIG_V200].Rvalue =
            stats->params.rawawb.ro_rawawb_sum_r_big[i];
        mAlogsSharedParams.ispStats.awb_stats.light[i].xYType[RK_AIQ_AWB_XY_TYPE_BIG_V200].Gvalue =
            stats->params.rawawb.ro_rawawb_sum_g_big[i];
        mAlogsSharedParams.ispStats.awb_stats.light[i].xYType[RK_AIQ_AWB_XY_TYPE_BIG_V200].Bvalue =
            stats->params.rawawb.ro_rawawb_sum_b_big[i];
        mAlogsSharedParams.ispStats.awb_stats.light[i].xYType[RK_AIQ_AWB_XY_TYPE_BIG_V200].WpNo =
            stats->params.rawawb.ro_rawawb_wp_num_big[i];
        mAlogsSharedParams.ispStats.awb_stats.light[i].xYType[RK_AIQ_AWB_XY_TYPE_SMALL_V200].Rvalue =
            stats->params.rawawb.ro_rawawb_sum_r_sma[i];
        mAlogsSharedParams.ispStats.awb_stats.light[i].xYType[RK_AIQ_AWB_XY_TYPE_SMALL_V200].Gvalue =
            stats->params.rawawb.ro_rawawb_sum_g_sma[i];
        mAlogsSharedParams.ispStats.awb_stats.light[i].xYType[RK_AIQ_AWB_XY_TYPE_SMALL_V200].Bvalue =
            stats->params.rawawb.ro_rawawb_sum_b_sma[i];
        mAlogsSharedParams.ispStats.awb_stats.light[i].xYType[RK_AIQ_AWB_XY_TYPE_SMALL_V200].WpNo =
            stats->params.rawawb.ro_rawawb_wp_num_sma[i];
    }
    for(int i = 0; i < mAlogsSharedParams.ispStats.awb_cfg_effect_v200.lightNum; i++) {
        mAlogsSharedParams.ispStats.awb_stats.multiwindowLightResult[i].xYType[RK_AIQ_AWB_XY_TYPE_NORMAL_V200].Rvalue =
            stats->params.rawawb.ro_sum_r_nor_multiwindow[i];
        mAlogsSharedParams.ispStats.awb_stats.multiwindowLightResult[i].xYType[RK_AIQ_AWB_XY_TYPE_NORMAL_V200].Gvalue =
            stats->params.rawawb.ro_sum_g_nor_multiwindow[i];
        mAlogsSharedParams.ispStats.awb_stats.multiwindowLightResult[i].xYType[RK_AIQ_AWB_XY_TYPE_NORMAL_V200].Bvalue =
            stats->params.rawawb.ro_sum_b_nor_multiwindow[i];
        mAlogsSharedParams.ispStats.awb_stats.multiwindowLightResult[i].xYType[RK_AIQ_AWB_XY_TYPE_NORMAL_V200].WpNo =
            stats->params.rawawb.ro_wp_nm_nor_multiwindow[i];
        mAlogsSharedParams.ispStats.awb_stats.multiwindowLightResult[i].xYType[RK_AIQ_AWB_XY_TYPE_BIG_V200].Rvalue =
            stats->params.rawawb.ro_sum_r_big_multiwindow[i];
        mAlogsSharedParams.ispStats.awb_stats.multiwindowLightResult[i].xYType[RK_AIQ_AWB_XY_TYPE_BIG_V200].Gvalue =
            stats->params.rawawb.ro_sum_g_big_multiwindow[i];
        mAlogsSharedParams.ispStats.awb_stats.multiwindowLightResult[i].xYType[RK_AIQ_AWB_XY_TYPE_BIG_V200].Bvalue =
            stats->params.rawawb.ro_sum_b_big_multiwindow[i];
        mAlogsSharedParams.ispStats.awb_stats.multiwindowLightResult[i].xYType[RK_AIQ_AWB_XY_TYPE_BIG_V200].WpNo =
            stats->params.rawawb.ro_wp_nm_big_multiwindow[i];
        mAlogsSharedParams.ispStats.awb_stats.multiwindowLightResult[i].xYType[RK_AIQ_AWB_XY_TYPE_SMALL_V200].Rvalue =
            stats->params.rawawb.ro_sum_r_sma_multiwindow[i];
        mAlogsSharedParams.ispStats.awb_stats.multiwindowLightResult[i].xYType[RK_AIQ_AWB_XY_TYPE_SMALL_V200].Gvalue =
            stats->params.rawawb.ro_sum_g_sma_multiwindow[i];
        mAlogsSharedParams.ispStats.awb_stats.multiwindowLightResult[i].xYType[RK_AIQ_AWB_XY_TYPE_SMALL_V200].Bvalue =
            stats->params.rawawb.ro_sum_b_sma_multiwindow[i];
        mAlogsSharedParams.ispStats.awb_stats.multiwindowLightResult[i].xYType[RK_AIQ_AWB_XY_TYPE_SMALL_V200].WpNo =
            stats->params.rawawb.ro_wp_nm_sma_multiwindow[i];
    }
    for(int i = 0; i < RK_AIQ_AWB_STAT_WP_RANGE_NUM_V200; i++) {
        mAlogsSharedParams.ispStats.awb_stats.excWpRangeResult[i].Rvalue = stats->params.rawawb.ro_sum_r_exc[i];
        mAlogsSharedParams.ispStats.awb_stats.excWpRangeResult[i].Gvalue = stats->params.rawawb.ro_sum_g_exc[i];
        mAlogsSharedParams.ispStats.awb_stats.excWpRangeResult[i].Bvalue = stats->params.rawawb.ro_sum_b_exc[i];
        mAlogsSharedParams.ispStats.awb_stats.excWpRangeResult[i].WpNo =    stats->params.rawawb.ro_wp_nm_exc[i];

    }
    for(int i = 0; i < RK_AIQ_AWB_GRID_NUM_TOTAL; i++) {
        mAlogsSharedParams.ispStats.awb_stats.blockResult[i].Rvalue = stats->params.rawawb.ramdata[i].r;
        mAlogsSharedParams.ispStats.awb_stats.blockResult[i].Gvalue = stats->params.rawawb.ramdata[i].g;
        mAlogsSharedParams.ispStats.awb_stats.blockResult[i].Bvalue = stats->params.rawawb.ramdata[i].b;
        mAlogsSharedParams.ispStats.awb_stats.blockResult[i].isWP[2] = stats->params.rawawb.ramdata[i].wp & 0x1;
        mAlogsSharedParams.ispStats.awb_stats.blockResult[i].isWP[1] = (stats->params.rawawb.ramdata[i].wp >> 1) & 0x1;
        mAlogsSharedParams.ispStats.awb_stats.blockResult[i].isWP[0] = (stats->params.rawawb.ramdata[i].wp >> 2) & 0x1;
    }
    //mAlogsSharedParams.ispStats.awb_stats_valid = ISP2X_STAT_RAWAWB(stats->meas_type)? true:false;
    mAlogsSharedParams.ispStats.awb_stats_valid = stats->meas_type >> 5 & 1;

    //ahdr
    mAlogsSharedParams.ispStats.ahdr_stats_valid = stats->meas_type >> 16 & 1;
    mAlogsSharedParams.ispStats.ahdr_stats.tmo_stats.ro_hdrtmo_lglow = stats->params.hdrtmo.lglow;
    mAlogsSharedParams.ispStats.ahdr_stats.tmo_stats.ro_hdrtmo_lgmin = stats->params.hdrtmo.lgmin;
    mAlogsSharedParams.ispStats.ahdr_stats.tmo_stats.ro_hdrtmo_lghigh = stats->params.hdrtmo.lghigh;
    mAlogsSharedParams.ispStats.ahdr_stats.tmo_stats.ro_hdrtmo_lgmax = stats->params.hdrtmo.lgmax;
    mAlogsSharedParams.ispStats.ahdr_stats.tmo_stats.ro_hdrtmo_weightkey = stats->params.hdrtmo.weightkey;
    mAlogsSharedParams.ispStats.ahdr_stats.tmo_stats.ro_hdrtmo_lgmean = stats->params.hdrtmo.lgmean;
    mAlogsSharedParams.ispStats.ahdr_stats.tmo_stats.ro_hdrtmo_lgrange0 = stats->params.hdrtmo.lgrange0;
    mAlogsSharedParams.ispStats.ahdr_stats.tmo_stats.ro_hdrtmo_lgrange1 = stats->params.hdrtmo.lgrange1;
    mAlogsSharedParams.ispStats.ahdr_stats.tmo_stats.ro_hdrtmo_palpha = stats->params.hdrtmo.palpha;
    mAlogsSharedParams.ispStats.ahdr_stats.tmo_stats.ro_hdrtmo_lgavgmax = stats->params.hdrtmo.lgavgmax;
    mAlogsSharedParams.ispStats.ahdr_stats.tmo_stats.ro_hdrtmo_linecnt = stats->params.hdrtmo.linecnt;
    for(int i = 0; i < 32; i++)
        mAlogsSharedParams.ispStats.ahdr_stats.tmo_stats.ro_array_min_max[i] = stats->params.hdrtmo.min_max[i];

    //dehaze
    mAlogsSharedParams.ispStats.adehaze_stats_valid = stats->meas_type >> 17 & 1;
    mAlogsSharedParams.ispStats.adehaze_stats.dehaze_stats_v20.dhaz_adp_air_base = stats->params.dhaz.dhaz_adp_air_base;
    mAlogsSharedParams.ispStats.adehaze_stats.dehaze_stats_v20.dhaz_adp_wt = stats->params.dhaz.dhaz_adp_wt;
    mAlogsSharedParams.ispStats.adehaze_stats.dehaze_stats_v20.dhaz_adp_gratio = stats->params.dhaz.dhaz_adp_gratio;
    mAlogsSharedParams.ispStats.adehaze_stats.dehaze_stats_v20.dhaz_adp_wt = stats->params.dhaz.dhaz_adp_wt;
    for(int i = 0; i < 64; i++) {
        mAlogsSharedParams.ispStats.adehaze_stats.dehaze_stats_v20.h_b_iir[i] = stats->params.dhaz.h_b_iir[i];
        mAlogsSharedParams.ispStats.adehaze_stats.dehaze_stats_v20.h_g_iir[i] = stats->params.dhaz.h_g_iir[i];
        mAlogsSharedParams.ispStats.adehaze_stats.dehaze_stats_v20.h_r_iir[i] = stats->params.dhaz.h_r_iir[i];
    }

    //ae
    mAlogsSharedParams.ispStats.aec_stats_valid = (stats->meas_type >> 11) & (0x01) ? true : false;

    /*rawae stats*/
    uint8_t AeSwapMode, AeSelMode;
    AeSwapMode = ispParams->aec_meas.rawae0.rawae_sel;
    AeSelMode = ispParams->aec_meas.rawae3.rawae_sel;

    switch(AeSwapMode) {
    case AEC_RAWSWAP_MODE_S_LITE:
        for(int i = 0; i < ISP2X_RAWAEBIG_MEAN_NUM; i++) {
            if(i < ISP2X_RAWAELITE_MEAN_NUM) {
                mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[0].rawae_lite.channelr_xy[i] = stats->params.rawae0.data[i].channelr_xy;
                mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[0].rawae_lite.channelg_xy[i] = stats->params.rawae0.data[i].channelg_xy;
                mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[0].rawae_lite.channelb_xy[i] = stats->params.rawae0.data[i].channelb_xy;
            }
            mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[1].rawae_big.channelr_xy[i] = stats->params.rawae1.data[i].channelr_xy;
            mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[1].rawae_big.channelg_xy[i] = stats->params.rawae1.data[i].channelg_xy;
            mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[1].rawae_big.channelb_xy[i] = stats->params.rawae1.data[i].channelb_xy;
            mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[2].rawae_big.channelr_xy[i] = stats->params.rawae2.data[i].channelr_xy;
            mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[2].rawae_big.channelg_xy[i] = stats->params.rawae2.data[i].channelg_xy;
            mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[2].rawae_big.channelb_xy[i] = stats->params.rawae2.data[i].channelb_xy;
            if(i < ISP2X_RAWAEBIG_SUBWIN_NUM) {
                mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[1].rawae_big.wndx_sumr[i] = stats->params.rawae1.sumr[i];
                mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[1].rawae_big.wndx_sumg[i] = stats->params.rawae1.sumg[i];
                mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[1].rawae_big.wndx_sumb[i] = stats->params.rawae1.sumb[i];
                mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[2].rawae_big.wndx_sumr[i] = stats->params.rawae2.sumr[i];
                mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[2].rawae_big.wndx_sumg[i] = stats->params.rawae2.sumg[i];
                mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[2].rawae_big.wndx_sumb[i] = stats->params.rawae2.sumb[i];
            }
        }
        memcpy(mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[0].rawhist_lite.bins, stats->params.rawhist0.hist_bin, ISP2X_HIST_BIN_N_MAX * sizeof(u32));
        memcpy(mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[1].rawhist_big.bins, stats->params.rawhist1.hist_bin, ISP2X_HIST_BIN_N_MAX * sizeof(u32));
        memcpy(mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[2].rawhist_big.bins, stats->params.rawhist2.hist_bin, ISP2X_HIST_BIN_N_MAX * sizeof(u32));
        break;

    case AEC_RAWSWAP_MODE_M_LITE:
        for(int i = 0; i < ISP2X_RAWAEBIG_MEAN_NUM; i++) {
            if(i < ISP2X_RAWAELITE_MEAN_NUM) {
                mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[1].rawae_lite.channelr_xy[i] = stats->params.rawae0.data[i].channelr_xy;
                mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[1].rawae_lite.channelg_xy[i] = stats->params.rawae0.data[i].channelg_xy;
                mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[1].rawae_lite.channelb_xy[i] = stats->params.rawae0.data[i].channelb_xy;
            }
            mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[0].rawae_big.channelr_xy[i] = stats->params.rawae1.data[i].channelr_xy;
            mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[0].rawae_big.channelg_xy[i] = stats->params.rawae1.data[i].channelg_xy;
            mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[0].rawae_big.channelb_xy[i] = stats->params.rawae1.data[i].channelb_xy;
            mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[2].rawae_big.channelr_xy[i] = stats->params.rawae2.data[i].channelr_xy;
            mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[2].rawae_big.channelg_xy[i] = stats->params.rawae2.data[i].channelg_xy;
            mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[2].rawae_big.channelb_xy[i] = stats->params.rawae2.data[i].channelb_xy;

            if(i < ISP2X_RAWAEBIG_SUBWIN_NUM) {
                mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[0].rawae_big.wndx_sumr[i] = stats->params.rawae1.sumr[i];
                mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[0].rawae_big.wndx_sumg[i] = stats->params.rawae1.sumg[i];
                mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[0].rawae_big.wndx_sumb[i] = stats->params.rawae1.sumb[i];
                mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[2].rawae_big.wndx_sumr[i] = stats->params.rawae2.sumr[i];
                mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[2].rawae_big.wndx_sumg[i] = stats->params.rawae2.sumg[i];
                mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[2].rawae_big.wndx_sumb[i] = stats->params.rawae2.sumb[i];
            }
        }
        memcpy(mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[0].rawhist_big.bins, stats->params.rawhist1.hist_bin, ISP2X_HIST_BIN_N_MAX * sizeof(u32));
        memcpy(mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[1].rawhist_lite.bins, stats->params.rawhist0.hist_bin, ISP2X_HIST_BIN_N_MAX * sizeof(u32));
        memcpy(mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[2].rawhist_big.bins, stats->params.rawhist2.hist_bin, ISP2X_HIST_BIN_N_MAX * sizeof(u32));
        break;

    case AEC_RAWSWAP_MODE_L_LITE:
        for(int i = 0; i < ISP2X_RAWAEBIG_MEAN_NUM; i++) {
            if(i < ISP2X_RAWAELITE_MEAN_NUM) {
                mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[2].rawae_lite.channelr_xy[i] = stats->params.rawae0.data[i].channelr_xy;
                mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[2].rawae_lite.channelg_xy[i] = stats->params.rawae0.data[i].channelg_xy;
                mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[2].rawae_lite.channelb_xy[i] = stats->params.rawae0.data[i].channelb_xy;
            }
            mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[0].rawae_big.channelr_xy[i] = stats->params.rawae2.data[i].channelr_xy;
            mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[0].rawae_big.channelg_xy[i] = stats->params.rawae2.data[i].channelg_xy;
            mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[0].rawae_big.channelb_xy[i] = stats->params.rawae2.data[i].channelb_xy;
            mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[1].rawae_big.channelr_xy[i] = stats->params.rawae1.data[i].channelr_xy;
            mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[1].rawae_big.channelg_xy[i] = stats->params.rawae1.data[i].channelg_xy;
            mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[1].rawae_big.channelb_xy[i] = stats->params.rawae1.data[i].channelb_xy;

            if(i < ISP2X_RAWAEBIG_SUBWIN_NUM) {
                mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[0].rawae_big.wndx_sumr[i] = stats->params.rawae2.sumr[i];
                mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[0].rawae_big.wndx_sumg[i] = stats->params.rawae2.sumg[i];
                mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[0].rawae_big.wndx_sumb[i] = stats->params.rawae2.sumb[i];
                mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[1].rawae_big.wndx_sumr[i] = stats->params.rawae1.sumr[i];
                mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[1].rawae_big.wndx_sumg[i] = stats->params.rawae1.sumg[i];
                mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[1].rawae_big.wndx_sumb[i] = stats->params.rawae1.sumb[i];
            }
        }
        memcpy(mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[0].rawhist_big.bins, stats->params.rawhist2.hist_bin, ISP2X_HIST_BIN_N_MAX * sizeof(u32));
        memcpy(mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[1].rawhist_big.bins, stats->params.rawhist1.hist_bin, ISP2X_HIST_BIN_N_MAX * sizeof(u32));
        memcpy(mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[2].rawhist_lite.bins, stats->params.rawhist0.hist_bin, ISP2X_HIST_BIN_N_MAX * sizeof(u32));
        break;

    default:
        LOGE("wrong AeSwapMode=%d\n", AeSwapMode);
        return XCAM_RETURN_ERROR_PARAM;
        break;
    }

    switch(AeSelMode) {
    case AEC_RAWSEL_MODE_CHN_0:
        for(int i = 0; i < ISP2X_RAWAEBIG_MEAN_NUM; i++) {

            mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[0].rawae_big.channelr_xy[i] = stats->params.rawae3.data[i].channelr_xy;
            mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[0].rawae_big.channelg_xy[i] = stats->params.rawae3.data[i].channelg_xy;
            mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[0].rawae_big.channelb_xy[i] = stats->params.rawae3.data[i].channelb_xy;

            if(i < ISP2X_RAWAEBIG_SUBWIN_NUM) {
                mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[0].rawae_big.wndx_sumr[i] = stats->params.rawae3.sumr[i];
                mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[0].rawae_big.wndx_sumg[i] = stats->params.rawae3.sumg[i];
                mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[0].rawae_big.wndx_sumb[i] = stats->params.rawae3.sumb[i];
            }
        }
        memcpy(mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[0].rawhist_big.bins, stats->params.rawhist3.hist_bin, ISP2X_HIST_BIN_N_MAX * sizeof(u32));
        break;

    case AEC_RAWSEL_MODE_CHN_1:
        for(int i = 0; i < ISP2X_RAWAEBIG_MEAN_NUM; i++) {

            mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[1].rawae_big.channelr_xy[i] = stats->params.rawae3.data[i].channelr_xy;
            mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[1].rawae_big.channelg_xy[i] = stats->params.rawae3.data[i].channelg_xy;
            mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[1].rawae_big.channelb_xy[i] = stats->params.rawae3.data[i].channelb_xy;

            if(i < ISP2X_RAWAEBIG_SUBWIN_NUM) {
                mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[1].rawae_big.wndx_sumr[i] = stats->params.rawae3.sumr[i];
                mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[1].rawae_big.wndx_sumg[i] = stats->params.rawae3.sumg[i];
                mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[1].rawae_big.wndx_sumb[i] = stats->params.rawae3.sumb[i];
            }
        }
        memcpy(mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[1].rawhist_big.bins, stats->params.rawhist3.hist_bin, ISP2X_HIST_BIN_N_MAX * sizeof(u32));
        break;

    case AEC_RAWSEL_MODE_CHN_2:
        for(int i = 0; i < ISP2X_RAWAEBIG_MEAN_NUM; i++) {

            mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[2].rawae_big.channelr_xy[i] = stats->params.rawae3.data[i].channelr_xy;
            mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[2].rawae_big.channelg_xy[i] = stats->params.rawae3.data[i].channelg_xy;
            mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[2].rawae_big.channelb_xy[i] = stats->params.rawae3.data[i].channelb_xy;

            if(i < ISP2X_RAWAEBIG_SUBWIN_NUM) {
                mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[2].rawae_big.wndx_sumr[i] = stats->params.rawae3.sumr[i];
                mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[2].rawae_big.wndx_sumg[i] = stats->params.rawae3.sumg[i];
                mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[2].rawae_big.wndx_sumb[i] = stats->params.rawae3.sumb[i];
            }
        }
        memcpy(mAlogsSharedParams.ispStats.aec_stats.ae_data.chn[2].rawhist_big.bins, stats->params.rawhist3.hist_bin, ISP2X_HIST_BIN_N_MAX * sizeof(u32));
        break;

    case AEC_RAWSEL_MODE_TMO:
        for(int i = 0; i < ISP2X_RAWAEBIG_MEAN_NUM; i++) {

            mAlogsSharedParams.ispStats.aec_stats.ae_data.extra.rawae_big.channelr_xy[i] = stats->params.rawae3.data[i].channelr_xy;
            mAlogsSharedParams.ispStats.aec_stats.ae_data.extra.rawae_big.channelg_xy[i] = stats->params.rawae3.data[i].channelg_xy;
            mAlogsSharedParams.ispStats.aec_stats.ae_data.extra.rawae_big.channelb_xy[i] = stats->params.rawae3.data[i].channelb_xy;

            if(i < ISP2X_RAWAEBIG_SUBWIN_NUM) {
                mAlogsSharedParams.ispStats.aec_stats.ae_data.extra.rawae_big.wndx_sumr[i] = stats->params.rawae3.sumr[i];
                mAlogsSharedParams.ispStats.aec_stats.ae_data.extra.rawae_big.wndx_sumg[i] = stats->params.rawae3.sumg[i];
                mAlogsSharedParams.ispStats.aec_stats.ae_data.extra.rawae_big.wndx_sumb[i] = stats->params.rawae3.sumb[i];
            }
        }
        memcpy(mAlogsSharedParams.ispStats.aec_stats.ae_data.extra.rawhist_big.bins, stats->params.rawhist3.hist_bin, ISP2X_HIST_BIN_N_MAX * sizeof(u32));
        break;

    default:
        LOGE("wrong AeSelMode=%d\n", AeSelMode);
        return XCAM_RETURN_ERROR_PARAM;
    }

    for(int i = 0; i < ISP2X_YUVAE_MEAN_NUM; i++) {
        mAlogsSharedParams.ispStats.aec_stats.ae_data.yuvae.mean[i] = stats->params.yuvae.mean[i];
        if(i < ISP2X_YUVAE_SUBWIN_NUM)
            mAlogsSharedParams.ispStats.aec_stats.ae_data.yuvae.ro_yuvae_sumy[i] = stats->params.yuvae.ro_yuvae_sumy[i];
    }

    if (expParams.ptr()) {

        mAlogsSharedParams.ispStats.aec_stats.ae_exp = expParams->data()->aecExpInfo;
        /*
         * printf("%s: L: [0x%x-0x%x], M: [0x%x-0x%x], S: [0x%x-0x%x]\n",
         *        __func__,
         *        expParams->data()->aecExpInfo.HdrExp[2].exp_sensor_params.coarse_integration_time,
         *        expParams->data()->aecExpInfo.HdrExp[2].exp_sensor_params.analog_gain_code_global,
         *        expParams->data()->aecExpInfo.HdrExp[1].exp_sensor_params.coarse_integration_time,
         *        expParams->data()->aecExpInfo.HdrExp[1].exp_sensor_params.analog_gain_code_global,
         *        expParams->data()->aecExpInfo.HdrExp[0].exp_sensor_params.coarse_integration_time,
         *        expParams->data()->aecExpInfo.HdrExp[0].exp_sensor_params.analog_gain_code_global);
         */
    }

    if (irisParams.ptr()) {

        float sof_time = (float)irisParams->data()->sofTime / 1000000000.0f;
        float start_time = (float)irisParams->data()->PIris.StartTim.tv_sec + (float)irisParams->data()->PIris.StartTim.tv_usec / 1000000.0f;
        float end_time = (float)irisParams->data()->PIris.EndTim.tv_sec + (float)irisParams->data()->PIris.EndTim.tv_usec / 1000000.0f;
        float frm_intval = 1 / (mAlogsSharedParams.ispStats.aec_stats.ae_exp.pixel_clock_freq_mhz * 1000000.0f /
                                (float)mAlogsSharedParams.ispStats.aec_stats.ae_exp.line_length_pixels / (float)mAlogsSharedParams.ispStats.aec_stats.ae_exp.frame_length_lines);

        /*printf("%s: step=%d,last-step=%d,start-tim=%f,end-tim=%f,sof_tim=%f\n",
            __func__,
            mAlogsSharedParams.ispStats.aec_stats.ae_exp.Iris.PIris.step,
            irisParams->data()->PIris.laststep,start_time,end_time,sof_time);
        */

        if(sof_time < end_time + frm_intval)
            mAlogsSharedParams.ispStats.aec_stats.ae_exp.Iris.PIris.step = irisParams->data()->PIris.laststep;
        else
            mAlogsSharedParams.ispStats.aec_stats.ae_exp.Iris.PIris.step = irisParams->data()->PIris.step;
    }

    //af
    {
        mAlogsSharedParams.ispStats.af_stats_valid =
            (stats->meas_type >> 6) & (0x01) ? true : false;
        mAlogsSharedParams.ispStats.af_stats.roia_luminance =
            stats->params.rawaf.afm_lum[0];
        mAlogsSharedParams.ispStats.af_stats.roib_sharpness =
            stats->params.rawaf.afm_sum[1];
        mAlogsSharedParams.ispStats.af_stats.roib_luminance =
            stats->params.rawaf.afm_lum[1];
        memcpy(mAlogsSharedParams.ispStats.af_stats.global_sharpness,
               stats->params.rawaf.ramdata, ISP2X_RAWAF_SUMDATA_NUM * sizeof(u32));

        mAlogsSharedParams.ispStats.af_stats.roia_sharpness = 0LL;
        for (int i = 0; i < ISP2X_RAWAF_SUMDATA_NUM; i++)
            mAlogsSharedParams.ispStats.af_stats.roia_sharpness += stats->params.rawaf.ramdata[i];

        if(afParams.ptr()) {
            mAlogsSharedParams.ispStats.af_stats.focus_endtim = afParams->data()->focusEndTim;
            mAlogsSharedParams.ispStats.af_stats.focus_starttim = afParams->data()->focusStartTim;
            mAlogsSharedParams.ispStats.af_stats.zoom_endtim = afParams->data()->zoomEndTim;
            mAlogsSharedParams.ispStats.af_stats.zoom_starttim = afParams->data()->zoomStartTim;
            mAlogsSharedParams.ispStats.af_stats.sof_tim = afParams->data()->sofTime;
        }
    }
#endif
    return ret;
}

XCamReturn
RkAiqCore::analyze(const SmartPtr<VideoBuffer> &buffer)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    SmartPtr<RkAiqFullParamsProxy> fullParam;
    SmartPtr<RkAiqFullParamsProxy> fullPparam;
    bool has_orb_stats = false;
    bool has_3a_stats = false;

    if (!firstStatsReceived) {
        firstStatsReceived = true;
        mState = RK_AIQ_CORE_STATE_RUNNING;
    }

    mAlogsSharedParams.init = false;

#ifdef RK_SIMULATOR_HW
    has_orb_stats = true;
    has_3a_stats = true;
#else
    if (buffer->get_video_info().format == V4L2_META_FMT_RK_ISP1_STAT_3A) {
        has_3a_stats = true;
    } else {
        has_orb_stats = true;
    }
#endif

#ifdef RK_SIMULATOR_HW
    const SmartPtr<V4l2BufferProxy> buf =
        buffer.dynamic_cast_ptr<V4l2BufferProxy>();
    //SmartPtr<RkAiqIspParamsProxy> ispParams = buf->get_isp_params();

    rk_sim_isp_v200_stats_t* stats =
        (rk_sim_isp_v200_stats_t*)(buf->get_v4l2_userptr());
    // copy directly for simulator
    mAlogsSharedParams.ispStats.awb_stats = stats->awb;
    mAlogsSharedParams.ispStats.awb_stats_valid = stats->valid_awb;
    //mAlogsSharedParams.ispStats.awb_cfg_effect_v200 = ispParams->data()->awb_cfg_v200;
    mAlogsSharedParams.ispStats.awb_cfg_effect_valid = true;
    mAlogsSharedParams.ispStats.awb_stats_v201 = stats->awb_v201;
    //mAlogsSharedParams.ispStats.awb_cfg_effect_v201 = ispParams->data()->awb_cfg_v201;

    mAlogsSharedParams.ispStats.aec_stats = stats->ae;
    mAlogsSharedParams.ispStats.aec_stats_valid = stats->valid_ae;

    mAlogsSharedParams.ispStats.af_stats = stats->af;
    mAlogsSharedParams.ispStats.af_stats_valid = stats->valid_af;


#if 1
    LOGD_ANR("oyyf: %s:%d input stats param start\n", __FUNCTION__, __LINE__);
    mAlogsSharedParams.iso = stats->iso;
    LOGD_ANR("oyyf: %s:%d input stats param end\n", __FUNCTION__, __LINE__);
#endif

    //Ahdr
    mAlogsSharedParams.ispStats.ahdr_stats_valid = true;
    mAlogsSharedParams.ispStats.ahdr_stats = stats->ahdr;

    mAlogsSharedParams.ispStats.orb_stats = stats->orb;
    mAlogsSharedParams.ispStats.orb_stats_valid = stats->valid_orb;
#else
    if (has_3a_stats) {
        ret = convertIspstatsToAlgo(buffer);
        if (ret)
            return XCAM_RETURN_BYPASS;
        cacheIspStatsToList();
    } else if (has_orb_stats) {
        const SmartPtr<V4l2BufferProxy> buf =
            buffer.dynamic_cast_ptr<V4l2BufferProxy>();
        struct rkispp_stats_buffer *ppstats =
            (struct rkispp_stats_buffer *)(buf->get_v4l2_userptr());

        mAlogsSharedParams.ispStats.orb_stats_valid =
            (ppstats->meas_type >> 4) & (0x01) ? true : false;
        mAlogsSharedParams.ispStats.orb_stats.num_points =
            ppstats->total_num;
        for (u32 i = 0; i < ppstats->total_num; i++) {
            mAlogsSharedParams.ispStats.orb_stats.points[i].x =
                ppstats->data[i].x;
            mAlogsSharedParams.ispStats.orb_stats.points[i].y =
                ppstats->data[i].y;
            memcpy(mAlogsSharedParams.ispStats.orb_stats.points[i].brief,
                   ppstats->data[i].brief, 15);
        }
    } else {
        LOGW_ANALYZER("no orb or 3a stats !", __FUNCTION__, __LINE__);
    }
#endif

    if (has_3a_stats)
        fullParam = analyzeInternal();
    if (has_orb_stats)
        fullPparam = analyzeInternalPp();

#ifdef RK_SIMULATOR_HW
    // merge results for simulator
    fullParam->data()->mIsppParams->data()->orb =
        fullPparam->data()->mIsppParams->data()->orb;
#endif

    if (fullParam.ptr() && mCb) {
        fullParam->data()->mIspParams->data()->frame_id = buffer->get_sequence() + 1;
        if (fullParam->data()->mIsppParams.ptr())
            fullParam->data()->mIsppParams->data()->frame_id = buffer->get_sequence() + 1;
        mCb->rkAiqCalcDone(fullParam);
    } else if (fullPparam.ptr() && mCb) {
        fullPparam->data()->mIsppParams->data()->frame_id = buffer->get_sequence() + 1;
        mCb->rkAiqCalcDone(fullPparam);
    }

    return ret;
}

XCamReturn
RkAiqCore::preProcessPp()
{
    ENTER_ANALYZER_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    for (auto algoHdl : mCurIsppAlgoHandleList) {
        if (algoHdl.ptr() && algoHdl->getEnable()) {
            /* TODO, should be called before all algos preProcess ? */ \
            ret = algoHdl->updateConfig(true);
            RKAIQCORE_CHECK_BYPASS(ret, "algoHdl %d updateConfig failed", algoHdl->getAlgoType());
            \
            ret = algoHdl->preProcess();
            RKAIQCORE_CHECK_BYPASS(ret, "algoHdl %d preProcess failed", algoHdl->getAlgoType());
            \
        }
    }

    EXIT_ANALYZER_FUNCTION();

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
RkAiqCore::preProcess()
{
    ENTER_ANALYZER_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    for (auto algoHdl : mCurIspAlgoHandleList) {
        if (algoHdl.ptr() && algoHdl->getEnable()) {
            /* TODO, should be called before all algos preProcess ? */ \
            ret = algoHdl->updateConfig(true);
            RKAIQCORE_CHECK_BYPASS(ret, "algoHdl %d updateConfig failed", algoHdl->getAlgoType());
            \
            ret = algoHdl->preProcess();
            RKAIQCORE_CHECK_BYPASS(ret, "algoHdl %d processing failed", algoHdl->getAlgoType());
            \
        }
    }

    EXIT_ANALYZER_FUNCTION();

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
RkAiqCore::processingPp()
{
    ENTER_ANALYZER_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    for (auto algoHdl : mCurIsppAlgoHandleList) {
        if (algoHdl.ptr() && algoHdl->getEnable()) {
            ret = algoHdl->processing();
            RKAIQCORE_CHECK_BYPASS(ret, "algoHdl %d processing failed", algoHdl->getAlgoType());
            \
        }
    }

    EXIT_ANALYZER_FUNCTION();

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
RkAiqCore::processing()
{
    ENTER_ANALYZER_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    for (auto algoHdl : mCurIspAlgoHandleList) {
        if (algoHdl.ptr() && algoHdl->getEnable()) {
            ret = algoHdl->processing();
            RKAIQCORE_CHECK_BYPASS(ret, "algoHdl %d processing failed", algoHdl->getAlgoType());
            \
        }
    }

    EXIT_ANALYZER_FUNCTION();

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
RkAiqCore::postProcessPp()
{
    ENTER_ANALYZER_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    for (auto algoHdl : mCurIsppAlgoHandleList) {
        if (algoHdl.ptr() && algoHdl->getEnable()) {
            ret = algoHdl->postProcess();
            RKAIQCORE_CHECK_BYPASS(ret, "algoHdl %d postProcess failed", algoHdl->getAlgoType());
            \
        }
    }

    EXIT_ANALYZER_FUNCTION();

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
RkAiqCore::postProcess()
{
    ENTER_ANALYZER_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    for (auto algoHdl : mCurIspAlgoHandleList) {
        if (algoHdl.ptr() && algoHdl->getEnable()) {
            ret = algoHdl->postProcess();
            RKAIQCORE_CHECK_BYPASS(ret, "algoHdl %d postProcess failed", algoHdl->getAlgoType());
            \
        }
    }

    EXIT_ANALYZER_FUNCTION();

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
RkAiqCore::setHwInfos(struct RkAiqHwInfo &hw_info)
{
    ENTER_ANALYZER_FUNCTION();
    mHwInfo = hw_info;
    EXIT_ANALYZER_FUNCTION();

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
RkAiqCore::setCpsLtCfg(rk_aiq_cpsl_cfg_t &cfg)
{
    ENTER_ANALYZER_FUNCTION();
    if (mState < RK_AIQ_CORE_STATE_INITED) {
        LOGE_ANALYZER("should call afer init");
        return XCAM_RETURN_ERROR_FAILED;
    }

    if (mCpslCap.modes_num == 0)
        return XCAM_RETURN_ERROR_PARAM;

    int i = 0;
    for (; i < mCpslCap.modes_num; i++) {
        if (mCpslCap.supported_modes[i] == cfg.mode)
            break;
    }

    if (i == mCpslCap.modes_num) {
        return XCAM_RETURN_ERROR_PARAM;
    }

    if (cfg.mode == RK_AIQ_OP_MODE_AUTO) {
        mAlogsSharedParams.cpslCfg.u.a = cfg.u.a;
    } else if (cfg.mode == RK_AIQ_OP_MODE_MANUAL) {
        mAlogsSharedParams.cpslCfg.u.m = cfg.u.m;
    } else {
        return XCAM_RETURN_ERROR_PARAM;
    }

    mAlogsSharedParams.cpslCfg.mode = cfg.mode;

    for (i = 0; i < mCpslCap.lght_src_num; i++) {
        if (mCpslCap.supported_lght_src[i] == cfg.lght_src)
            break;
    }

    if (i == mCpslCap.lght_src_num) {
        return XCAM_RETURN_ERROR_PARAM;
    }

    mAlogsSharedParams.cpslCfg = cfg;
    LOGD("set cpsl: mode %d", cfg.mode);
    EXIT_ANALYZER_FUNCTION();

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
RkAiqCore::getCpsLtInfo(rk_aiq_cpsl_info_t &info)
{
    ENTER_ANALYZER_FUNCTION();
    if (mState < RK_AIQ_CORE_STATE_INITED) {
        LOGE_ANALYZER("should call afer init");
        return XCAM_RETURN_ERROR_FAILED;
    }

    info.mode = mAlogsSharedParams.cpslCfg.mode;
    if (info.mode == RK_AIQ_OP_MODE_MANUAL) {
        info.on = mAlogsSharedParams.cpslCfg.u.m.on;
        info.strength_led = mAlogsSharedParams.cpslCfg.u.m.strength_led;
        info.strength_ir = mAlogsSharedParams.cpslCfg.u.m.strength_ir;
    } else {
        info.on = mCurCpslOn;
        info.gray = mAlogsSharedParams.gray_mode;
    }

    info.lght_src = mAlogsSharedParams.cpslCfg.lght_src;
    EXIT_ANALYZER_FUNCTION();

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
RkAiqCore::queryCpsLtCap(rk_aiq_cpsl_cap_t &cap)
{
    ENTER_ANALYZER_FUNCTION();
    if (mHwInfo.fl_supported || mHwInfo.irc_supported) {
        cap.supported_modes[0] = RK_AIQ_OP_MODE_AUTO;
        cap.supported_modes[1] = RK_AIQ_OP_MODE_MANUAL;
        cap.modes_num = 2;
    } else {
        cap.modes_num = 0;
    }

    cap.lght_src_num = 0;
    if (mHwInfo.fl_supported) {
        cap.supported_lght_src[0] = RK_AIQ_CPSLS_LED;
        cap.lght_src_num++;
    }

    if (mHwInfo.irc_supported) {
        cap.supported_lght_src[cap.lght_src_num] = RK_AIQ_CPSLS_IR;
        cap.lght_src_num++;
    }

    if (cap.lght_src_num > 1) {
        cap.supported_lght_src[cap.lght_src_num] = RK_AIQ_CPSLS_MIX;
        cap.lght_src_num++;
    }

    cap.strength_led.min = 0;
    cap.strength_led.max = 100;
    if (!mHwInfo.fl_strth_adj)
        cap.strength_led.step = 100;
    else
        cap.strength_led.step = 1;

    cap.strength_ir.min = 0;
    cap.strength_ir.max = 100;
    if (!mHwInfo.fl_ir_strth_adj)
        cap.strength_ir.step = 100;
    else
        cap.strength_ir.step = 1;

    cap.sensitivity.min = 0;
    cap.sensitivity.max = 100;
    cap.sensitivity.step = 1;

    LOGI("cpsl cap: light_src_num %d, led_step %f, ir_step %f",
         cap.lght_src_num, cap.strength_led.step, cap.strength_ir.step);

    EXIT_ANALYZER_FUNCTION();

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
RkAiqCore::genCpslResult(RkAiqFullParams* params)
{
    rk_aiq_cpsl_cfg_t* cpsl_cfg = &mAlogsSharedParams.cpslCfg;

    if (cpsl_cfg->mode == RK_AIQ_OP_MODE_INVALID)
        return XCAM_RETURN_NO_ERROR;

    if (mAiqCpslParamsPool->has_free_items()) {
        params->mCpslParams = mAiqCpslParamsPool->get_item();
    } else {
        LOGW_ANALYZER("no free cpsl params buffer!");
        return XCAM_RETURN_NO_ERROR;
    }

    RKAiqCpslInfoWrapper_t* cpsl_param = params->mCpslParams->data().ptr();
    xcam_mem_clear(*cpsl_param);

    LOGD_ANALYZER("cpsl mode %d, light src %d", cpsl_cfg->mode, cpsl_cfg->lght_src);
    bool cpsl_on = false;
    bool need_update = false;

    if (cpsl_cfg->mode == RK_AIQ_OP_MODE_MANUAL) {
        if ((mCurCpslOn != cpsl_cfg->u.m.on) ||
                (fabs(mStrthLed - cpsl_cfg->u.m.strength_led) > EPSINON) ||
                (fabs(mStrthIr - cpsl_cfg->u.m.strength_ir) > EPSINON)) {
            need_update = true;
            cpsl_on = cpsl_cfg->u.m.on;
            cpsl_param->fl.power[0] = cpsl_cfg->u.m.strength_led / 100.0f;
            cpsl_param->fl_ir.power[0] = cpsl_cfg->u.m.strength_ir / 100.0f;
        }
    } else {
        RkAiqAlgoPreResAsdInt* asd_pre_rk = (RkAiqAlgoPreResAsdInt*)mAlogsSharedParams.preResComb.asd_pre_res;
        if (asd_pre_rk) {
            asd_preprocess_result_t* asd_result = &asd_pre_rk->asd_result;
            if (mCurCpslOn != asd_result->cpsl_on) {
                need_update = true;
                cpsl_on = asd_result->cpsl_on;
            }
        }
        cpsl_param->fl.power[0] = 1.0f;
        cpsl_param->fl_ir.power[0] = 1.0f;
    }

    // need to init cpsl status, cause the cpsl driver state
    // may be not correct
    if (mState == RK_AIQ_CORE_STATE_INITED)
        need_update = true;

    if (need_update) {
        if (cpsl_cfg->lght_src & RK_AIQ_CPSLS_LED) {
            cpsl_param->update_fl = true;
            if (cpsl_on)
                cpsl_param->fl.flash_mode = RK_AIQ_FLASH_MODE_TORCH;
            else
                cpsl_param->fl.flash_mode = RK_AIQ_FLASH_MODE_OFF;
            if (cpsl_on ) {
                cpsl_param->fl.strobe = true;
                mAlogsSharedParams.fill_light_on = true;
            } else {
                cpsl_param->fl.strobe = false;
                mAlogsSharedParams.fill_light_on = false;
            }
            LOGD_ANALYZER("cpsl fl mode %d, strength %f, strobe %d",
                          cpsl_param->fl.flash_mode, cpsl_param->fl.power[0],
                          cpsl_param->fl.strobe);
        }

        if (cpsl_cfg->lght_src & RK_AIQ_CPSLS_IR) {
            cpsl_param->update_ir = true;
            if (cpsl_on) {
                cpsl_param->ir.irc_on = true;
                cpsl_param->fl_ir.flash_mode = RK_AIQ_FLASH_MODE_TORCH;
                cpsl_param->fl_ir.strobe = true;
                mAlogsSharedParams.fill_light_on = true;
            } else {
                cpsl_param->ir.irc_on = false;
                cpsl_param->fl_ir.flash_mode = RK_AIQ_FLASH_MODE_OFF;
                cpsl_param->fl_ir.strobe = false;
                mAlogsSharedParams.fill_light_on = false;
            }
            LOGD_ANALYZER("cpsl irc on %d, fl_ir: mode %d, strength %f, strobe %d",
                          cpsl_param->ir.irc_on, cpsl_param->fl_ir.flash_mode, cpsl_param->fl_ir.power[0],
                          cpsl_param->fl_ir.strobe);
        }

        if (mGrayMode == RK_AIQ_GRAY_MODE_CPSL) {
            if (mAlogsSharedParams.fill_light_on && cpsl_cfg->gray_on) {
                mAlogsSharedParams.gray_mode = true;
            } else
                mAlogsSharedParams.gray_mode = false;

        } else {
            /* no mutex lock protection for gray_mode with setGrayMode,
             * so need set again here
             */
            if (mGrayMode == RK_AIQ_GRAY_MODE_OFF)
                mAlogsSharedParams.gray_mode = false;
            else if (mGrayMode == RK_AIQ_GRAY_MODE_ON)
                mAlogsSharedParams.gray_mode = true;
        }
        mCurCpslOn = cpsl_on;
        mStrthLed = cpsl_cfg->u.m.strength_led;
        mStrthIr = cpsl_cfg->u.m.strength_ir;
    } else {
        cpsl_param->update_ir = false;
        cpsl_param->update_fl = false;
    }

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
RkAiqCore::setGrayMode(rk_aiq_gray_mode_t mode)
{
    LOGD_ANALYZER("%s: gray mode %d", __FUNCTION__, mode);

    if (mAlogsSharedParams.is_bw_sensor) {
        LOGE_ANALYZER("%s: not support for black&white sensor", __FUNCTION__);
        return XCAM_RETURN_ERROR_PARAM;
    }
    if (mAlogsSharedParams.calib->colorAsGrey.enable) {
        LOGE_ANALYZER("%s: not support,since color_as_grey is enabled in xml", __FUNCTION__);
        return XCAM_RETURN_ERROR_PARAM;
    }

    mGrayMode = mode;
    if (mode == RK_AIQ_GRAY_MODE_OFF)
        mAlogsSharedParams.gray_mode = false;
    else if (mode == RK_AIQ_GRAY_MODE_ON)
        mAlogsSharedParams.gray_mode = true;
    else if (mode == RK_AIQ_GRAY_MODE_CPSL)
        ; // do nothing
    else
        LOGE_ANALYZER("%s: gray mode %d error", __FUNCTION__, mode);

    return XCAM_RETURN_NO_ERROR;
}

rk_aiq_gray_mode_t
RkAiqCore::getGrayMode()
{
    LOGD_ANALYZER("%s: gray mode %d", __FUNCTION__, mGrayMode);
    return mGrayMode;
}

void
RkAiqCore::setSensorFlip(bool mirror, bool flip)
{
    mAlogsSharedParams.sns_mirror = mirror;
    mAlogsSharedParams.sns_flip = flip;
}



} //namespace RkCam
