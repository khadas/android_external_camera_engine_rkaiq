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

#ifndef _RK_AIQ_MANAGER_H_
#define _RK_AIQ_MANAGER_H_

#include "ICamHw.h"
#include "RkAiqCore.h"
#include "RkAiqCalibDb.h"
#include "RkLumaCore.h"
#include "rk_aiq.h"

namespace RkCam {

class RkAiqManager;
class RkAiqRstApplyThread
    : public Thread {
public:
    RkAiqRstApplyThread(RkAiqManager* aiqMng)
        : Thread("RkAiqRstApplyThread")
        , mAiqMng(aiqMng) {};
    ~RkAiqRstApplyThread() {};

    void triger_stop() {
        mAiqRstQueue.pause_pop ();
    };

    void triger_start() {
        mAiqRstQueue.resume_pop ();
    };

    bool push_results (SmartPtr<RkAiqFullParamsProxy> aiqRst) {
        return mAiqRstQueue.push(aiqRst);
    };

protected:
    virtual void stopped () {
        mAiqRstQueue.clear ();
    };

    virtual bool loop ();

private:
    RkAiqManager* mAiqMng;
    SafeList<RkAiqFullParamsProxy>  mAiqRstQueue;
};

class RkAiqManager
    : public IspStatsListener
    , public IsppStatsListener
    , public IspLumaListener
    , public IspEvtsListener
    , public RkAiqAnalyzerCb
    , public RkLumaAnalyzerCb {
    friend RkAiqRstApplyThread;
public:
    explicit RkAiqManager(const char* sns_ent_name,
                          rk_aiq_error_cb err_cb,
                          rk_aiq_metas_cb metas_cb);
    virtual ~RkAiqManager();
    void setCamHw(SmartPtr<ICamHw>& camhw);
    void setAnalyzer(SmartPtr<RkAiqCore> analyzer);
    void setAiqCalibDb(const CamCalibDbContext_t* calibDb);
    void setLumaAnalyzer(SmartPtr<RkLumaCore> analyzer);
    XCamReturn init();
    XCamReturn prepare(uint32_t width, uint32_t height, rk_aiq_working_mode_t mode);
    XCamReturn start();
    XCamReturn stop();
    XCamReturn deInit();
    // from IsppStatsListener
    XCamReturn isppStatsCb(SmartPtr<VideoBuffer>& isppStats);
    // from IspLumaListener
    XCamReturn ispLumaCb(SmartPtr<VideoBuffer>& ispLuma);
    // from IspStatsListener
    XCamReturn ispStatsCb(SmartPtr<VideoBuffer>& ispStats);
    // from IspEvtsListener
    XCamReturn ispEvtsCb(ispHwEvt_t* evt);
    // from RkAiqAnalyzerCb
    void rkAiqCalcDone(SmartPtr<RkAiqFullParamsProxy>& results);
    void rkAiqCalcFailed(const char* msg);
    // from RkLumaAnalyzerCb
    void rkLumaCalcDone(int frame_id, int count);
    void rkLumaCalcFailed(const char* msg);
    XCamReturn setModuleCtl(rk_aiq_module_id_t mId, bool mod_en);
    XCamReturn getModuleCtl(rk_aiq_module_id_t mId, bool& mod_en);
    XCamReturn enqueueBuffer(struct rk_aiq_vbuf *vbuf);
    XCamReturn offlineRdJobPrepare();
    XCamReturn offlineRdJobDone();
    XCamReturn setSharpFbcRotation(rk_aiq_rotation_t rot);
    XCamReturn setMirrorFlip(bool mirror, bool flip);
    XCamReturn getMirrorFlip(bool& mirror, bool& flip);
    void setDefMirrorFlip();
    XCamReturn swWorkingModeDyn(rk_aiq_working_mode_t mode);
protected:
    XCamReturn applyAnalyzerResult(SmartPtr<RkAiqFullParamsProxy>& results);
private:
    enum aiq_state_e {
        AIQ_STATE_INVALID,
        AIQ_STATE_INITED,
        AIQ_STATE_PREPARED,
        AIQ_STATE_STARTED,
        AIQ_STATE_STOPED,
    };
    XCAM_DEAD_COPY (RkAiqManager);
private:
    SmartPtr<ICamHw> mCamHw;
    SmartPtr<RkAiqCore> mRkAiqAnalyzer;
    SmartPtr<RkAiqRstApplyThread> mAiqRstAppTh;
    SmartPtr<RkLumaCore> mRkLumaAnalyzer;
    rk_aiq_error_cb mErrCb;
    rk_aiq_metas_cb mMetasCb;
    const char* mSnsEntName;
    const CamCalibDbContext_t* mCalibDb;
    rk_aiq_working_mode_t mWorkingMode;
    uint32_t mWidth;
    uint32_t mHeight;
    int _state;
    bool mCurMirror;
    bool mCurFlip;
};

}; //namespace RkCam

#endif //_RK_AIQ_MANAGER_H_
