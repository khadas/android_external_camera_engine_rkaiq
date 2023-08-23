/*
 * AiqCameraHalAdapter.h - main rkaiq CameraHal Adapter
 *
 *  Copyright (c) 2020 Rockchip Corporation
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

#ifndef _AIQ_CAMERA_HAL_ADAPTER_H_
#define _AIQ_CAMERA_HAL_ADAPTER_H_

#include "RkAiqCore.h"
#include <RkAiqManager.h>

#include "rkisp_control_loop.h"
#include "rkaiq.h"

#ifdef ANDROID_VERSION_ABOVE_8_X
#include <CameraMetadata.h>
using ::android::hardware::camera::common::V1_0::helper::CameraMetadata;
#include <cutils/properties.h>
#define CAM_IQ_PROPERTY_KEY  "vendor.cam.librkisp.iqfile.name"
#else
#include <camera/CameraMetadata.h>
#endif
#include <pthread.h>

#include "include/ae_state_machine.h"
#include "include/af_state_machine.h"
#include "include/awb_state_machine.h"

#include "settings_processor.h"
#include "MessageQueue.h"
#include "MessageThread.h"

/*
 ***************** AIQ ADAPTER LIB VERSION NOTE *****************
* v1.0.0 init
*  - match rkaiq v1.3.0
*  - add properties to show rkaiq & aiqAdapter lib version
* v1.0.1
*  - remove unnecessary code
* v1.0.2
*  - add Message Thread deal with parameter setting & resultcb
* v1.0.3
*  - workaround awb state not converged issue.
*  - fix some mismatched debug info
* v2.0.1
*  - adapt to aiq_v5.0x1.3-rc1.
*  - replace uAPI1 with uAPI2
*/

#define CONFIG_AIQ_ADAPTER_LIB_VERSION "v2.0.1"

using namespace RkCam;
using namespace XCam;
using namespace android::camera2;

typedef struct rk_aiq_sys_ctx_s rk_aiq_sys_ctx_t;

struct rkisp_cl_frame_rkaiq_s {
    //frame id
    int id;
    // rkaiq input parameters
    const void *rkaiq;
};

struct rkisp_cl_frame_rkaiq_result_s {
    //frame id
    int id;
    rk_aiq_ae_results  *ae_results;
    rk_aiq_awb_results *awb_results;
    rk_aiq_af_results  *af_results;
};

enum USE_CASE {
  UC_PREVIEW,
  UC_PRE_CAPTRUE,
  UC_CAPTURE,
  UC_RECORDING,
  UC_RAW
};

enum MessageId {
    MESSAGE_ID_EXIT = 0,            // call requestExitAndWait
    MESSAGE_ID_ISP_STAT_DONE,
    MESSAGE_ID_ISP_SOF_DONE,
    MESSAGE_ID_RKAIQ_CAL_DONE,       // partial metadata
    MESSAGE_ID_FLUSH,
    // max number of messages
    MESSAGE_ID_MAX
};

struct MessageMetadataDone {
    int resultIndex;    /*!> Index from the partial result array that is
                             being returned */
 };

struct MessageShutterDone {
    int64_t time;
 };

union MessageData {
    MessageMetadataDone meta;
    MessageShutterDone shutter;
};

/**
 * \struct
 * Result processor message structure
 */
struct Message {
    MessageId id;
    MessageData data;
};

typedef enum StillLockState {
    STILL_CAP_3A_UNLOCK = 0,
    STILL_CAP_3A_LOCK
} StillLockState_t ;

//namespace RkCam {
class AiqCameraHalAdapter:
    public IMessageHandler
{
private:

    const cl_result_callback_ops_t *mCallbackOps;

    Mutex _settingsMutex;
    // push_back when set_control_params, erase when calculationd done
    std::vector<SmartPtr<AiqInputParams>>  _settings;
    std::vector<SmartPtr<AiqInputParams>>  _fly_settings;
    SmartPtr<AiqInputParams>  _cur_settings;
    SettingsProcessor*            _settingsProcessor;
    SmartPtr<AiqInputParams> _inputParams;
    SmartPtr<RkAEStateMachine>     mAeState;      /**< AE state machine */
    SmartPtr<RkAFStateMachine>      mAfState;     /**< AF state machine */
    SmartPtr<RkAWBStateMachine>    mAwbState;     /**< AWB state machine */
    rk_aiq_exposure_sensor_descriptor _sns_des;

    /* MetaData*/
    CameraMetadata *_metadata;
    camera_metadata_t *_meta;
    //TODO for Capture or Flash
    bool _delay_still_capture;
    rk_aiq_working_mode_t _work_mode;
    int32_t _exposureCompensation;
    StillLockState_t mAelockstate;
    StillLockState_t mAflockstate;
    StillLockState_t mAwblockstate;
    XCamAeParam mLastAeParam;
    XCamAwbParam mLastAwbParam;
    XCamAfParam mLastAfParam;

private:  /* Members */
    /**
     * Thread control members
     */
    std::atomic<bool> mThreadRunning;
    MessageQueue<Message, MessageId> mMessageQueue;
    std::unique_ptr<android::camera2::MessageThread> mMessageThread;
    float mMeanLuma = 1.0f;
    enum aiq_state_e {
        AIQ_ADAPTER_STATE_INVALID,
        AIQ_ADAPTER_STATE_INITED,
        AIQ_ADAPTER_STATE_PREPARED,
        AIQ_ADAPTER_STATE_STARTED,
        AIQ_ADAPTER_STATE_STOPED,
    };

    int _state;
private:  /* Methods */
    /* IMessageHandler overloads */
    status_t requestExitAndWait();
    virtual void messageThreadLoop();
    status_t handleMessageExit(Message &msg);
    status_t handleIspSofCb(Message &msg);
    status_t handleMessageFlush(Message &msg);

public:
    rk_aiq_sys_ctx_t* _aiq_ctx;
    static CameraMetadata staticMeta;
    pthread_mutex_t _aiq_ctx_mutex;
    camera_metadata_rational_t _transformMatrix[9];

public:
    explicit AiqCameraHalAdapter();
    virtual ~AiqCameraHalAdapter();
    void init(const cl_result_callback_ops_t* callbacks);
    void start();
    void stop();
    void deInit();
    void processResults();
    void syncSofToHal(rk_aiq_metas_t* metas);
    void processResultsDebug(SmartPtr<RkAiqFullParamsProxy> &results);
    XCamReturn getAeResults(rk_aiq_ae_results &ae_results);
    void getAeResultsDebug(rk_aiq_ae_results &ae_results, SmartPtr<rk_aiq_exposure_params_wrapper_t> exp_param);
    XCamReturn getAfResults(rk_aiq_af_results &af_results);
    void getAfResultsDebug(rk_aiq_af_results &af_results, SmartPtr<rk_aiq_focus_params_t> focus_param);
    XCamReturn getAwbResults(rk_aiq_awb_results &awb_results);
    void getAwbResultsDebug(rk_aiq_awb_results &awb_results, SmartPtr<rk_aiq_isp_stats_t> isp_param);
    void updateParams(SmartPtr<VideoBuffer>& ispStats);
    void updateAeParams(XCamAeParam *aeParams);
    void updateAwbParams(XCamAwbParam *awbParams);
    void updateAfParams(XCamAfParam *afParams);
    void setFrameParams(const struct rkisp_cl_frame_rkaiq_s* frame_params);

    virtual XCamReturn metaCallback(rk_aiq_metas_t* metas);

    SmartPtr<AiqInputParams> getAiqInputParams();
    void set_aiq_ctx(rk_aiq_sys_ctx_t* aiq_ctx) { _aiq_ctx = aiq_ctx; };
    rk_aiq_sys_ctx_t* get_aiq_ctx() { return _aiq_ctx; };
    void set_static_metadata(const camera_metadata_t *metas) { staticMeta = metas; };
    static CameraMetadata& get_static_metadata() { return staticMeta; };
    XCamReturn set_control_params(const int request_frame_id,
                                  const camera_metadata_t *metas);
    void setAiqInputParams(SmartPtr<AiqInputParams> inputParams) { _inputParams  = inputParams; };
    SmartPtr<AiqInputParams> getAiqInputParams_simple() { return _inputParams; };

    void updateMetaParams(SmartPtr<AiqInputParams> inputParams);
    void updateAeMetaParams(XCamAeParam *aeParams, int reqId);
    void updateAfMetaParams(XCamAfParam *afParams, int reqId);
    void updateAwbV21MetaParams(XCamAwbParam *awbParams, int reqId);
    void updateAwbV30MetaParams(XCamAwbParam *awbParams, int reqId);
    void updateAwbV32MetaParams(XCamAwbParam *awbParams, int reqId);
    void updateAwbMetaParams(XCamAwbParam *awbParams, int reqId);
    void updateOtherMetaParams();

    void pre_process_3A_states(SmartPtr<AiqInputParams> inputParams);
    bool set_sensor_mode_data (rk_aiq_exposure_sensor_descriptor *sensor_mode,
                               bool first);
    XCamReturn processAeMetaResults(rk_aiq_ae_results &ae_result, CameraMetadata *metadata);
    XCamReturn processAfMetaResults(rk_aiq_af_results &af_result, CameraMetadata *metadata);
    XCamReturn processAwbMetaResults(rk_aiq_awb_results &awb_result, CameraMetadata *metadata);
    XCamReturn processMiscMetaResults(CameraMetadata *metadata);
    void set_working_mode(rk_aiq_working_mode_t work_mode) { _work_mode = work_mode; };
};
//}; //namespace RkCam


#endif //_AIQ_CAMERA_HAL_ADAPTER_H_
