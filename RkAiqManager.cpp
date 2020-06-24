/*
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

#include "RkAiqManager.h"
#include "isp20/Isp20_module_dbg.h"

using namespace XCam;
namespace RkCam {

#define RKAIQMNG_CHECK_RET(ret, format, ...) \
    if (ret) { \
        LOGE(format, ##__VA_ARGS__); \
        return ret; \
    }

bool
RkAiqRstApplyThread::loop ()
{
    ENTER_XCORE_FUNCTION();

    const static int32_t timeout = -1;
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    SmartPtr<RkAiqFullParamsProxy> results = mAiqRstQueue.pop (timeout);

    XCAM_ASSERT (mAiqMng);

    if (!results.ptr()) {
        XCAM_LOG_WARNING("RkAiqRstApplyThread got empty result, stop thread");
        return false;
    }

#ifdef RUNTIME_MODULE_DEBUG
#ifndef RK_SIMULATOR_HW
    if (g_apply_init_params_only)
        goto out;
#endif
#endif
    ret = mAiqMng->applyAnalyzerResult(results);
    if (ret == XCAM_RETURN_NO_ERROR)
        return true;

    EXIT_XCORE_FUNCTION();
out:
    // always true
    return true;
}

RkAiqManager::RkAiqManager(const char* sns_ent_name,
                           rk_aiq_error_cb err_cb,
                           rk_aiq_metas_cb metas_cb)
    : mCamHw(NULL)
    , mRkAiqAnalyzer(NULL)
    , mRkLumaAnalyzer(NULL)
    , mAiqRstAppTh(new RkAiqRstApplyThread(this))
    , mErrCb(err_cb)
    , mMetasCb(metas_cb)
    , mSnsEntName(sns_ent_name)
    , mWorkingMode(RK_AIQ_WORKING_MODE_NORMAL)
    , mCalibDb(NULL)
{
    ENTER_XCORE_FUNCTION();
    EXIT_XCORE_FUNCTION();
}

RkAiqManager::~RkAiqManager()
{
    ENTER_XCORE_FUNCTION();
    EXIT_XCORE_FUNCTION();
}

void
RkAiqManager::setCamHw(SmartPtr<ICamHw>& camhw)
{
    ENTER_XCORE_FUNCTION();
    XCAM_ASSERT (!mCamHw.ptr());
    mCamHw = camhw;
    EXIT_XCORE_FUNCTION();
}

void
RkAiqManager::setAnalyzer(SmartPtr<RkAiqCore> analyzer)
{
    ENTER_XCORE_FUNCTION();
    XCAM_ASSERT (!mRkAiqAnalyzer.ptr());
    mRkAiqAnalyzer = analyzer;
    EXIT_XCORE_FUNCTION();
}

void
RkAiqManager::setLumaAnalyzer(SmartPtr<RkLumaCore> analyzer)
{
    ENTER_XCORE_FUNCTION();
    XCAM_ASSERT (!mRkLumaAnalyzer.ptr());
    mRkLumaAnalyzer = analyzer;
    EXIT_XCORE_FUNCTION();
}

void
RkAiqManager::setAiqCalibDb(const CamCalibDbContext_t* calibDb)
{
    ENTER_XCORE_FUNCTION();
    XCAM_ASSERT (!mCalibDb);
    mCalibDb = calibDb;
    EXIT_XCORE_FUNCTION();
}

XCamReturn
RkAiqManager::init()
{
    ENTER_XCORE_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    XCAM_ASSERT (mSnsEntName);
    XCAM_ASSERT (mRkAiqAnalyzer.ptr());
    XCAM_ASSERT (mCamHw.ptr());
    XCAM_ASSERT (mCalibDb);

    mRkAiqAnalyzer->setAnalyzeResultCb(this);
    ret = mRkAiqAnalyzer->init(mSnsEntName, mCalibDb);
    RKAIQMNG_CHECK_RET(ret, "analyzer init error %d !", ret);

    mRkLumaAnalyzer->setAnalyzeResultCb(this);
    ret = mRkLumaAnalyzer->init();
    RKAIQMNG_CHECK_RET(ret, "luma analyzer init error %d !", ret);

    mCamHw->setIspLumaListener(this);
    mCamHw->setIspStatsListener(this);
    mCamHw->setEvtsListener(NULL);
    ret = mCamHw->init(mSnsEntName);
    RKAIQMNG_CHECK_RET(ret, "camHw init error %d !", ret);

    EXIT_XCORE_FUNCTION();

    return ret;
}

XCamReturn
RkAiqManager::prepare(uint32_t width, uint32_t height, rk_aiq_working_mode_t mode)
{
    ENTER_XCORE_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    rk_aiq_exposure_sensor_descriptor sensor_des;

    XCAM_ASSERT (mCalibDb);
#ifdef RUNTIME_MODULE_DEBUG
#ifndef RK_SIMULATOR_HW
    get_dbg_force_disable_mods_env();
#endif
#endif
    int working_mode_hw;
    if (mode == RK_AIQ_WORKING_MODE_NORMAL) {
        working_mode_hw = mode;
    } else {
        if (mode != RK_AIQ_HDR_GET_WORKING_MODE(mCalibDb->sysContrl.hdr_mode)) {
            ret = XCAM_RETURN_ERROR_PARAM;
            RKAIQMNG_CHECK_RET(ret, "Not supported HDR mode!");
        } else {
            working_mode_hw = mCalibDb->sysContrl.hdr_mode;
        }
    }
    mCamHw->setCalib(mCalibDb);
    ret = mCamHw->prepare(width, height, working_mode_hw, mCalibDb->sysContrl.time_delay, mCalibDb->sysContrl.gain_delay);
    RKAIQMNG_CHECK_RET(ret, "camhw prepare error %d", ret);

    xcam_mem_clear(sensor_des);
    ret = mCamHw->getSensorModeData(mSnsEntName, sensor_des);

    ret = mRkLumaAnalyzer->prepare(working_mode_hw);

    RKAIQMNG_CHECK_RET(ret, "getSensorModeData error %d", ret);
    ret = mRkAiqAnalyzer->prepare(&sensor_des, working_mode_hw);
    RKAIQMNG_CHECK_RET(ret, "analyzer prepare error %d", ret);

    SmartPtr<RkAiqFullParamsProxy> initParams = mRkAiqAnalyzer->getAiqFullParams();

    ret = applyAnalyzerResult(initParams);
    RKAIQMNG_CHECK_RET(ret, "set initial params error %d", ret);

    EXIT_XCORE_FUNCTION();

    return ret;
}

XCamReturn
RkAiqManager::start()
{
    ENTER_XCORE_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    bool bret = mAiqRstAppTh->start();
    ret = bret ? XCAM_RETURN_NO_ERROR : XCAM_RETURN_ERROR_FAILED;
    RKAIQMNG_CHECK_RET(ret, "apply result thread start error");

    ret = mRkAiqAnalyzer->start();
    RKAIQMNG_CHECK_RET(ret, "analyzer start error %d", ret);

    ret = mRkLumaAnalyzer->start();
    RKAIQMNG_CHECK_RET(ret, "luma analyzer start error %d", ret);

    ret = mCamHw->start();
    RKAIQMNG_CHECK_RET(ret, "camhw start error %d", ret);

    EXIT_XCORE_FUNCTION();

    return ret;
}

XCamReturn
RkAiqManager::stop()
{
    ENTER_XCORE_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    mAiqRstAppTh->triger_stop();
    bool bret = mAiqRstAppTh->stop();
    ret = bret ? XCAM_RETURN_NO_ERROR : XCAM_RETURN_ERROR_FAILED;
    RKAIQMNG_CHECK_RET(ret, "apply result thread stop error");

    ret = mRkAiqAnalyzer->stop();
    RKAIQMNG_CHECK_RET(ret, "analyzer stop error %d", ret);

    ret = mRkLumaAnalyzer->stop();
    RKAIQMNG_CHECK_RET(ret, "luma analyzer stop error %d", ret);

    ret = mCamHw->stop();
    RKAIQMNG_CHECK_RET(ret, "camhw stop error %d", ret);

    EXIT_XCORE_FUNCTION();

    return ret;
}

XCamReturn
RkAiqManager::deInit()
{
    ENTER_XCORE_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    ret = mRkAiqAnalyzer->deInit();
    RKAIQMNG_CHECK_RET(ret, "analyzer deinit error %d", ret);

    ret = mRkLumaAnalyzer->deInit();
    RKAIQMNG_CHECK_RET(ret, "luma analyzer deinit error %d", ret);

    ret = mCamHw->deInit();
    RKAIQMNG_CHECK_RET(ret, "camhw deinit error %d", ret);

    EXIT_XCORE_FUNCTION();

    return ret;
}

XCamReturn
RkAiqManager::isppStatsCb(SmartPtr<VideoBuffer>& isppStats)
{
    ENTER_XCORE_FUNCTION();
    return ispStatsCb(isppStats);
    EXIT_XCORE_FUNCTION();
}

XCamReturn
RkAiqManager::ispLumaCb(SmartPtr<VideoBuffer>& ispLuma)
{
    ENTER_XCORE_FUNCTION();
    //TODO::create luma detection thread,analyze luma change,and decide
    //number of frames should HDR to process.
    XCamReturn ret = mRkLumaAnalyzer->pushStats(ispLuma);
    EXIT_XCORE_FUNCTION();

    return ret;
}

XCamReturn
RkAiqManager::ispStatsCb(SmartPtr<VideoBuffer>& ispStats)
{
    ENTER_XCORE_FUNCTION();
    XCamReturn ret = mRkAiqAnalyzer->pushStats(ispStats);
#ifndef RK_SIMULATOR_HW
    if (get_rkaiq_runtime_dbg() > 0) {
        if (ispStats->get_video_info().format == V4L2_META_FMT_RK_ISP1_STAT_3A) {
            XCAM_STATIC_FPS_CALCULATION(ISP_STATS_FPS, 60);
        } else {
            XCAM_STATIC_FPS_CALCULATION(PP_STATS_FPS, 60);
        }
    }
#endif
    EXIT_XCORE_FUNCTION();

    return ret;
}

XCamReturn
RkAiqManager::ispEvtsCb(ispHwEvt_t* evt)
{
    //TODO
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    return ret;
}

XCamReturn
RkAiqManager::applyAnalyzerResult(SmartPtr<RkAiqFullParamsProxy>& results)
{
    ENTER_XCORE_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    RkAiqFullParams* aiqParams = NULL;

    if (!results.ptr()) {
        LOGW_ANALYZER("empty aiq params results!");
        return ret;
    }

    // TODO: couldn't get dynamic debug env now
#if 0//def RUNTIME_MODULE_DEBUG
    get_dbg_force_disable_mods_env();
#endif

    aiqParams = results->data().ptr();

#ifdef RUNTIME_MODULE_DEBUG
#ifndef RK_SIMULATOR_HW
    if (g_bypass_exp_params)
        goto set_exp_end;
#endif
#endif
    if (aiqParams->mExposureParams.ptr()) {
//#define DEBUG_FIXED_EXPOSURE
#ifdef DEBUG_FIXED_EXPOSURE
        /* test aec with fixed sensor exposure */
        int cnt = aiqParams->mIspParams->data()->frame_id ;
        if (aiqParams->mExposureParams->data()->algo_id == 0) {
            aiqParams->mExposureParams->data()->exp_tbl_size = 1;
            RKAiqAecExpInfo_t* exp_tbl = &aiqParams->mExposureParams->data()->exp_tbl[0];
            if(cnt % 40 <= 19) {
                exp_tbl->HdrExp[2].exp_sensor_params.coarse_integration_time = 984;
                exp_tbl->HdrExp[2].exp_sensor_params.analog_gain_code_global = 48;
                exp_tbl->HdrExp[1].exp_sensor_params.coarse_integration_time = 984;
                exp_tbl->HdrExp[1].exp_sensor_params.analog_gain_code_global = 48;
                exp_tbl->HdrExp[0].exp_sensor_params.coarse_integration_time = 246;
                exp_tbl->HdrExp[0].exp_sensor_params.analog_gain_code_global = 16;

                exp_tbl->HdrExp[2].exp_real_params.integration_time = 0.02;
                exp_tbl->HdrExp[2].exp_real_params.analog_gain = 3;
                exp_tbl->HdrExp[1].exp_real_params.integration_time = 0.02;
                exp_tbl->HdrExp[1].exp_real_params.analog_gain = 3;
                exp_tbl->HdrExp[0].exp_real_params.integration_time = 0.005;
                exp_tbl->HdrExp[0].exp_real_params.analog_gain = 1;
            } else {
                exp_tbl->HdrExp[2].exp_sensor_params.coarse_integration_time = 1475;
                exp_tbl->HdrExp[2].exp_sensor_params.analog_gain_code_global = 144;
                exp_tbl->HdrExp[1].exp_sensor_params.coarse_integration_time = 1475;
                exp_tbl->HdrExp[1].exp_sensor_params.analog_gain_code_global = 144;
                exp_tbl->HdrExp[0].exp_sensor_params.coarse_integration_time = 492;
                exp_tbl->HdrExp[0].exp_sensor_params.analog_gain_code_global = 48;

                exp_tbl->HdrExp[2].exp_real_params.integration_time = 0.03;
                exp_tbl->HdrExp[2].exp_real_params.analog_gain = 9;
                exp_tbl->HdrExp[1].exp_real_params.integration_time = 0.03;
                exp_tbl->HdrExp[1].exp_real_params.analog_gain = 9;
                exp_tbl->HdrExp[0].exp_real_params.integration_time = 0.01;
                exp_tbl->HdrExp[0].exp_real_params.analog_gain = 3;
            }
        }
        ret = mCamHw->setExposureParams(aiqParams->mExposureParams);
        if (ret)
            LOGE_ANALYZER("setExposureParams error %d", ret);
#else
        ret = mCamHw->setExposureParams(aiqParams->mExposureParams);
        if (ret)
            LOGE_ANALYZER("setExposureParams error %d", ret);
#endif
    }
set_exp_end:

#ifdef RUNTIME_MODULE_DEBUG
#ifndef RK_SIMULATOR_HW
    if (g_bypass_isp_params)
        goto set_isp_end;
#endif
#endif

    if (aiqParams->mIspParams.ptr()) {
        ret = mCamHw->setIspParams(aiqParams->mIspParams);
        if (ret)
            LOGE_ANALYZER("setIspParams error %d", ret);
    }
set_isp_end:

#ifdef RUNTIME_MODULE_DEBUG
#ifndef RK_SIMULATOR_HW
    if (g_bypass_ispp_params)
        goto set_ispp_end;
#endif
#endif

#ifndef DISABLE_PP
    if (aiqParams->mIsppParams.ptr()) {
        ret = mCamHw->setIsppParams(aiqParams->mIsppParams);
        if (ret)
            LOGE_ANALYZER("setIsppParams error %d", ret);
    }
#endif
set_ispp_end:

    if (aiqParams->mFocusParams.ptr()) {
        ret = mCamHw->setFocusParams(aiqParams->mFocusParams);
        if (ret)
            LOGE_ANALYZER("setFocusParams error %d", ret);
    }

    EXIT_XCORE_FUNCTION();
out:
    return ret;
}

void
RkAiqManager::rkAiqCalcDone(SmartPtr<RkAiqFullParamsProxy> &results)
{
    ENTER_XCORE_FUNCTION();

    XCAM_ASSERT (mAiqRstAppTh.ptr());
    mAiqRstAppTh->push_results(results);

    EXIT_XCORE_FUNCTION();
}

void
RkAiqManager::rkAiqCalcFailed(const char* msg)
{
    ENTER_XCORE_FUNCTION();
    // TODO
    EXIT_XCORE_FUNCTION();
    return ;
}

void
RkAiqManager::rkLumaCalcDone(int frame_id, int count)
{
    ENTER_XCORE_FUNCTION();
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    ret = mCamHw->setHdrProcessCount(frame_id, count);
    EXIT_XCORE_FUNCTION();
}

void
RkAiqManager::rkLumaCalcFailed(const char* msg)
{
    ENTER_XCORE_FUNCTION();
    // TODO
    EXIT_XCORE_FUNCTION();
    return ;
}

XCamReturn
RkAiqManager::setModuleCtl(rk_aiq_module_id_t mId, bool mod_en)
{
    ENTER_XCORE_FUNCTION();
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    ret = mCamHw->setModuleCtl(mId, mod_en);
    EXIT_XCORE_FUNCTION();
    return ret;
}

XCamReturn
RkAiqManager::getModuleCtl(rk_aiq_module_id_t mId, bool& mod_en)
{
    ENTER_XCORE_FUNCTION();
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    ret = mCamHw->getModuleCtl(mId, mod_en);
    EXIT_XCORE_FUNCTION();
    return ret;
}
}; //namespace RkCam
