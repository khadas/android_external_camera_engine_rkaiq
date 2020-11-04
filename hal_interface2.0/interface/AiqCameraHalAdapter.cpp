#include "AiqCameraHalAdapter.h"

#include <base/xcam_log.h>
#include <hwi/isp20/Isp20StatsBuffer.h>
#include <hwi/isp20/rkisp2-config.h>
#include <aiq_core/RkAiqHandleInt.h>

#include "common/rk_aiq_pool.h"
#include "rkaiq.h"

#include "rk_aiq_user_api_imgproc.h"
#include "rk_aiq_user_api_sysctl.h"
#include "rk_aiq_user_api_ae.h"
#include "rk_aiq_user_api_af.h"
#include "rk_aiq_user_api_awb.h"
#include "rk_aiq_user_api_accm.h"

#include "rkcamera_vendor_tags.h"
#include "settings_processor.h"

#define DEFAULT_ENTRY_CAP 64
#define DEFAULT_DATA_CAP 1024


AiqCameraHalAdapter::AiqCameraHalAdapter(SmartPtr<RkAiqManager> rkAiqManager,SmartPtr<RkAiqCore> analyzer,SmartPtr<ICamHw> camHw)
: _rkAiqManager(rkAiqManager)
, _analyzer(analyzer)
, _camHw(camHw)
, _delay_still_capture(false)
, _aiq_ctx(NULL)
, _meta (NULL)
, _metadata (NULL)
{
    int32_t status = OK;
    LOGD("@%s %d:", __FUNCTION__, __LINE__);
    _settingsProcessor = new SettingsProcessor();
    _settings.clear();
    _fly_settings.clear();

    mAeState = new RkAEStateMachine();
    mAfState = new RkAFStateMachine();
    mAwbState = new RkAWBStateMachine();
    _meta = allocate_camera_metadata(DEFAULT_ENTRY_CAP, DEFAULT_DATA_CAP);
    XCAM_ASSERT (_meta);
    _metadata = new CameraMetadata(_meta);
    _aiq_ctx_mutex = PTHREAD_MUTEX_INITIALIZER;

}

AiqCameraHalAdapter::~AiqCameraHalAdapter()
{
    int32_t status = OK;
    LOGD("@%s %d:", __FUNCTION__, __LINE__);

    deInit();
    if(_settingsProcessor)
        delete _settingsProcessor;
    _settings.clear();
    _fly_settings.clear();
     delete _metadata;
     _metadata = NULL;
     /*free_camera_metadata(_meta);*/
    _meta = NULL;
    LOGD("@%s deinit done", __FUNCTION__);
}

void
AiqCameraHalAdapter::init(const cl_result_callback_ops_t* callbacks){
    LOGD("@%s %d:", __FUNCTION__, __LINE__);
    if(_rkAiqManager.ptr() &&_analyzer.ptr() && callbacks){
        this->_analyzer->setAnalyzeResultCb(this);
        this->_RkAiqAnalyzerCb = _rkAiqManager;
        this->mCallbackOps = callbacks;
        this->_camHw->setIspStatsListener(this);
        this->_IspStatsListener = _rkAiqManager;
    }
}

void
AiqCameraHalAdapter::deInit(){
    LOGD("@%s %d:", __FUNCTION__, __LINE__);
    this->_rkAiqManager = nullptr;
    this->_analyzer = nullptr;
    this->_camHw = nullptr;
    this->_RkAiqAnalyzerCb = nullptr;
    this->_IspStatsListener = nullptr;
}

SmartPtr<AiqInputParams> AiqCameraHalAdapter:: getAiqInputParams()
{
    LOGD("@%s %d:", __FUNCTION__, __LINE__);
    LOGD("@%s %d: _fly_settings.size():%d, _settings.size():%d.", __FUNCTION__, __LINE__, _fly_settings.size(), _settings.size());
    SmartLock lock(_settingsMutex);
    // use new setting when no flying settings to make sure
    // same settings used for 3A stats of one frame
    if (!_settings.empty() && _fly_settings.empty()) {
        _cur_settings = *_settings.begin();
        _settings.erase(_settings.begin());
        _fly_settings.push_back(_cur_settings);
    }
    LOGD("@%s %d: _fly_settings.size():%d, _settings.size():%d.", __FUNCTION__, __LINE__, _fly_settings.size(), _settings.size());
    return _cur_settings;
}

XCamReturn 
AiqCameraHalAdapter::ispStatsCb(SmartPtr<VideoBuffer>& ispStats){
    LOGD("@%s %d:", __FUNCTION__, __LINE__);

    setAiqInputParams(this->getAiqInputParams());
    LOGD("@%s : reqId %d", __FUNCTION__, _inputParams.ptr() ? _inputParams->reqId : -1);
    // update 3A states
    pre_process_3A_states(_inputParams);
    updateMetaParams(ispStats);
    //TODO
    //set_sensor_mode_data()
    /*QJ old method, for Reference， not using now*/
    //updateParams(ispStats);

    //TODO, Bypass 3A Algorithm when states locked
    if(this->_IspStatsListener.ptr()){
        return this->_IspStatsListener->ispStatsCb(ispStats);
    }
    return XCAM_RETURN_NO_ERROR;
}

void
AiqCameraHalAdapter::pre_process_3A_states(SmartPtr<AiqInputParams> inputParams) {
    // we'll use the latest inputparams if no new one is comming,
    // so should ignore the processed triggers

    static int _procReqId = -1;
    if (inputParams.ptr()) {
        if (_procReqId == inputParams->reqId) {
            if (inputParams->aaaControls.ae.aePreCaptureTrigger == ANDROID_CONTROL_AE_PRECAPTURE_TRIGGER_START)
                inputParams->aaaControls.ae.aePreCaptureTrigger = 0;
            if (inputParams->aaaControls.af.afTrigger == ANDROID_CONTROL_AF_TRIGGER_START)
                inputParams->aaaControls.af.afTrigger = 0;
            /* inputParams->stillCapSyncCmd = 0; */
        } else
            _procReqId = inputParams->reqId;
        mAeState->processState(inputParams->aaaControls.controlMode,
                                            inputParams->aaaControls.ae);
        mAwbState->processState(inputParams->aaaControls.controlMode,
                                              inputParams->aaaControls.awb);
        mAfState->processTriggers(inputParams->aaaControls.af.afTrigger,
                                               inputParams->aaaControls.af.afMode, 0,
                                               inputParams->afInputParams.afParams);
    }
}

XCamReturn
AiqCameraHalAdapter::set_control_params(const int request_frame_id,
                              const camera_metadata_t *metas)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    static bool stillcap_sync_cmd_end_delay = false;

    XCam::SmartPtr<AiqInputParams> inputParams = new AiqInputParams();
    inputParams->reqId = request_frame_id;
    inputParams->settings = metas;
    inputParams->staticMeta = &AiqCameraHalAdapter::staticMeta;
    if(_settingsProcessor) {
        SmartPtr<RkCam::RkAiqManager> rk_aiq_manager = _aiq_ctx->_rkAiqManager; //current not use
        rk_aiq_exposure_sensor_descriptor sns_des;
        ret = _aiq_ctx->_camHw->getSensorModeData(_aiq_ctx->_sensor_entity_name, sns_des);
        inputParams->sensorOutputWidth = sns_des.sensor_output_width;
        inputParams->sensorOutputHeight = sns_des.sensor_output_height;
        _settingsProcessor->processRequestSettings(inputParams->settings, *inputParams.ptr());
    } else {
        LOGE("@%s %d: _settingsProcessor is null , is a bug, fix me", __FUNCTION__, __LINE__);
        return XCAM_RETURN_ERROR_UNKNOWN;
    }
    XCamAeParam aeparams = inputParams->aeInputParams.aeParams;
    AeControls aectl = inputParams->aaaControls.ae;
    AfControls afctl = inputParams->aaaControls.af;
    LOGI("@%s: request %d: aeparms: mode-%d, metering_mode-%d, flicker_mode-%d,"
         "ex_min-%" PRId64 ",ex_max-%" PRId64 ", manual_exp-%" PRId64 ", manual_gain-%f,"
         "aeControls: mode-%d, lock-%d, preTrigger-%d, antibanding-%d,"
         "evCompensation-%d, fpsrange[%d, %d]", __FUNCTION__, request_frame_id,
         aeparams.mode, aeparams.metering_mode, aeparams.flicker_mode,
         aeparams.exposure_time_min, aeparams.exposure_time_max,
         aeparams.manual_exposure_time, aeparams.manual_analog_gain,
         aectl.aeMode, aectl.aeLock, aectl.aePreCaptureTrigger,
         aectl.aeAntibanding, aectl.evCompensation,
         aectl.aeTargetFpsRange[0], aectl.aeTargetFpsRange[1]);
    LOGI("@%s : reqId %d, afMode %d, afTrigger %d", __FUNCTION__, request_frame_id, afctl.afMode, afctl.afTrigger);
    LOGI("@%s : reqId %d, frame usecase %d, flash_mode %d, stillCapSyncCmd %d",
         __FUNCTION__, request_frame_id, inputParams->frameUseCase,
         aeparams.flash_mode, inputParams->stillCapSyncCmd);
    {
        SmartLock lock(_settingsMutex);
        // to speed up flash off routine
        if (inputParams->stillCapSyncCmd == RKCAMERA3_PRIVATEDATA_STILLCAP_SYNC_CMD_SYNCEND) {
            float power[2] = {0.0f, 0.0f};

            //TODO
            //Flash control
//            if (_isp_controller.ptr()) {
//                _isp_controller->set_3a_fl (RKISP_FLASH_MODE_OFF, power, 0, 0);
//                LOGD("reqId %d, stillCapSyncCmd %d, flash off",
//                     request_frame_id, inputParams->stillCapSyncCmd);
//            }
//      RkAiqCpslParamsProxy include rk_aiq_flash_setting_t
//      _aiq_ctx->_camHw->setCpslParams(SmartPtr < RkAiqCpslParamsProxy > & cpsl_params)
        }

        // we use id -1 request to do special work, eg. flash stillcap sync
        if (request_frame_id != -1) {
            if (stillcap_sync_cmd_end_delay) {
                stillcap_sync_cmd_end_delay = false;
                inputParams->stillCapSyncCmd = RKCAMERA3_PRIVATEDATA_STILLCAP_SYNC_CMD_SYNCEND;
            }
            _settings.push_back(inputParams);
        } else {
            // merged to next params
            if (inputParams->stillCapSyncCmd == RKCAMERA3_PRIVATEDATA_STILLCAP_SYNC_CMD_SYNCEND) {
                if (!_settings.empty()) {
                    XCam::SmartPtr<AiqInputParams> settings = *_settings.begin();
                    settings->stillCapSyncCmd = RKCAMERA3_PRIVATEDATA_STILLCAP_SYNC_CMD_SYNCEND;
                } else {
                    stillcap_sync_cmd_end_delay = true;
                }
            }
            if (inputParams->aaaControls.ae.aePreCaptureTrigger == ANDROID_CONTROL_AE_PRECAPTURE_TRIGGER_START) {
                if (!_settings.empty()) {
                    XCam::SmartPtr<AiqInputParams> settings = *_settings.begin();
                    settings->aaaControls.ae.aePreCaptureTrigger = ANDROID_CONTROL_AE_PRECAPTURE_TRIGGER_START;
                    settings->reqId = -1;
                } else {
                    _cur_settings->aaaControls.ae.aePreCaptureTrigger = ANDROID_CONTROL_AE_PRECAPTURE_TRIGGER_START;
                    _cur_settings->reqId = -1;
                }
            }
        }
    }
    return ret;
}

void
AiqCameraHalAdapter::updateAeParams(XCamAeParam *aeParams){
    LOGD("@%s %d:", __FUNCTION__, __LINE__);

    RkCam::RkAiqAeHandleInt* handle =
    dynamic_cast<RkCam::RkAiqAeHandleInt*>(const_cast<RkCam::RkAiqHandle*>(_analyzer->getAiqAlgoHandle(RK_AIQ_ALGO_TYPE_AE)));

    Uapi_HdrExpAttr_t HdrExpAttr;

    handle->getHdrExpAttr(&HdrExpAttr);

    //HdrExpAttr.Evbias = -((50 - 1) * 3.0f);
    handle->setHdrExpAttr(HdrExpAttr);
    Uapi_HdrAeRouteAttr_t HdrAeDayRouteAttr;
    handle->getHdrAeDayRouteAttr(&HdrAeDayRouteAttr);

    handle->setHdrAeDayRouteAttr(HdrAeDayRouteAttr);
    Uapi_ExpSwAttr_t ExpSwAttr;
    handle->getExpSwAttr(&ExpSwAttr);
    ExpSwAttr.stAuto.stHdrAeRange.stExpTimeRange->Max = aeParams->exposure_time_max;
    ExpSwAttr.stAuto.stHdrAeRange.stExpTimeRange->Min = aeParams->exposure_time_min;

    if(aeParams->flicker_mode == XCAM_AE_FLICKER_MODE_50HZ){
        ExpSwAttr.stAntiFlicker.Frequency = AEC_FLICKER_FREQUENCY_50HZ;
    }else if(aeParams->flicker_mode == XCAM_AE_FLICKER_MODE_60HZ){
        ExpSwAttr.stAntiFlicker.Frequency = AEC_FLICKER_FREQUENCY_60HZ;
    }else if (XCAM_AE_FLICKER_MODE_OFF){
        ExpSwAttr.stAntiFlicker.Frequency = AEC_FLICKER_FREQUENCY_OFF;
    }else {
        ExpSwAttr.stAntiFlicker.Frequency = AEC_FLICKER_FREQUENCY_50HZ;
    }

    if(aeParams->mode == XCAM_AE_MODE_AUTO){
        ExpSwAttr.stAntiFlicker.Mode = AEC_ANTIFLICKER_AUTO_MODE;
    }else if(aeParams->mode == XCAM_AE_MODE_MANUAL){
        ExpSwAttr.stAntiFlicker.Mode = AEC_ANTIFLICKER_NORMAL_MODE;
    }

    handle->setExpSwAttr(ExpSwAttr);

    if(1){
        int status =  mAeState->getState();
        //ANDROID_CONTROL_AE_STATE_LOCKED = 3
        if(status == 3){
            handle->setEnable(false);
        }else{
            handle->setEnable(true);
        }
    }

}

void
AiqCameraHalAdapter::updateAwbParams(XCamAwbParam *awbParams){
    LOGD("@%s %d:", __FUNCTION__, __LINE__);
    RkCam::RkAiqAwbHandleInt* awbhandle =
    dynamic_cast<RkCam::RkAiqAwbHandleInt*>(const_cast<RkCam::RkAiqHandle*>(_analyzer->getAiqAlgoHandle(RK_AIQ_ALGO_TYPE_AWB)));
    rk_aiq_wb_attrib_t attr;
    awbhandle->getAttrib(&attr);
    ALOGD("awbParams->mode:%d",awbParams->mode);
    if(awbParams->mode == XCAM_AWB_MODE_AUTO){
        attr.mode = RK_AIQ_WB_MODE_AUTO;
    }else if(awbParams->mode == XCAM_AWB_MODE_MANUAL){
        attr.mode = RK_AIQ_WB_MODE_MANUAL;
        attr.stManual.mode = RK_AIQ_MWB_MODE_SCENE;
        attr.stManual.para.scene =  RK_AIQ_WBCT_DAYLIGHT;
    }else if(awbParams->mode == XCAM_AWB_MODE_WARM_INCANDESCENT){
        attr.mode = RK_AIQ_WB_MODE_MANUAL;
        attr.stManual.mode = RK_AIQ_MWB_MODE_SCENE;
        attr.stManual.para.scene =  RK_AIQ_WBCT_INCANDESCENT;
    }else if(awbParams->mode == XCAM_AWB_MODE_FLUORESCENT){
        attr.mode = RK_AIQ_WB_MODE_MANUAL;
        attr.stManual.mode = RK_AIQ_MWB_MODE_SCENE;
        attr.stManual.para.scene =  RK_AIQ_WBCT_FLUORESCENT;
    }else if(awbParams->mode == XCAM_AWB_MODE_WARM_FLUORESCENT){
        attr.mode = RK_AIQ_WB_MODE_MANUAL;
        attr.stManual.mode = RK_AIQ_MWB_MODE_SCENE;
        attr.stManual.para.scene =  RK_AIQ_WBCT_WARM_FLUORESCENT;
    }else if(awbParams->mode == XCAM_AWB_MODE_DAYLIGHT){
        attr.mode = RK_AIQ_WB_MODE_MANUAL;
        attr.stManual.mode = RK_AIQ_MWB_MODE_SCENE;
        attr.stManual.para.scene =  RK_AIQ_WBCT_DAYLIGHT;
    }else if(awbParams->mode == XCAM_AWB_MODE_CLOUDY){
        attr.mode = RK_AIQ_WB_MODE_MANUAL;
        attr.stManual.mode = RK_AIQ_MWB_MODE_SCENE;
        attr.stManual.para.scene =  RK_AIQ_WBCT_CLOUDY_DAYLIGHT;
    }else if(awbParams->mode == XCAM_AWB_MODE_SHADOW){
        attr.mode = RK_AIQ_WB_MODE_MANUAL;
        attr.stManual.mode = RK_AIQ_MWB_MODE_SCENE;
        attr.stManual.para.scene =  RK_AIQ_WBCT_SHADE;
    }

    awbhandle->setAttrib(attr);

}

void
AiqCameraHalAdapter::updateAfParams(XCamAfParam *afParams){
    LOGD("@%s %d:", __FUNCTION__, __LINE__);
    RkCam::RkAiqAfHandleInt* afhandle =
    dynamic_cast<RkCam::RkAiqAfHandleInt*>(const_cast<RkCam::RkAiqHandle*>(_analyzer->getAiqAlgoHandle(RK_AIQ_ALGO_TYPE_AF)));
    rk_aiq_af_attrib_t att;
    afhandle->getAttrib(&att);

    if(afParams->focus_mode == AF_MODE_CONTINUOUS_VIDEO){
        att.AfMode = RKAIQ_AF_MODE_CONTINUOUS_VIDEO;
    }else if(afParams->focus_mode == AF_MODE_CONTINUOUS_PICTURE){
        att.AfMode = RKAIQ_AF_MODE_CONTINUOUS_PICTURE;
    }else if(afParams->focus_mode == AF_MODE_MACRO){
        att.AfMode = RKAIQ_AF_MODE_MACRO;
    }else if(afParams->focus_mode == AF_MODE_EDOF){
        att.AfMode = RKAIQ_AF_MODE_EDOF;
    }else if(afParams->focus_mode == AF_MODE_EDOF){
        att.AfMode = RKAIQ_AF_MODE_EDOF;
    }else if(afParams->focus_mode == AF_MODE_AUTO){
        att.AfMode = RKAIQ_AF_MODE_AUTO;
    }

    afhandle->setAttrib(&att);
}

void
AiqCameraHalAdapter::updateParams(SmartPtr<VideoBuffer>& ispStats){
    LOGD("@%s %d:", __FUNCTION__, __LINE__);

    SmartPtr<AiqInputParams> inputParams =  getAiqInputParams();
    int id = -1;
    if(inputParams.ptr()){
        id = inputParams->reqId ;
    }else{
        ALOGE("inputParams NULL");
        return ;
    }

    AeControls *aeCtrl = &inputParams->aaaControls.ae;

    XCamAeParam *aeParams = &inputParams->aeInputParams.aeParams;
    XCamAwbParam *awbParams = &inputParams->awbInputParams.awbParams;
    XCamAfParam *afParams = &inputParams->afInputParams.afParams;
    if (ispStats->get_video_info().format == V4L2_META_FMT_RK_ISP1_STAT_3A) {
        LOGD("@%s %d:", __FUNCTION__, __LINE__);
           const SmartPtr<Isp20StatsBuffer> buf =
        ispStats.dynamic_cast_ptr<Isp20StatsBuffer>();
        struct rkisp_isp2x_stat_buffer *stats;

        SmartPtr<RkAiqExpParamsProxy> expParams = buf->get_exp_params();
        SmartPtr<RkAiqIspParamsProxy> ispParams = buf->get_isp_params();
        SmartPtr<RkAiqAfInfoProxy> af_Params = buf->get_af_params();

        stats = (struct rkisp_isp2x_stat_buffer *)(buf->get_v4l2_userptr());
        if(stats){
            LOGD("stats->frame_id:%d",stats->frame_id);
        }
        if(aeParams){
            updateAeParams(aeParams);
        }
        if(awbParams){
            updateAwbParams(awbParams);
        }
        if(afParams){
            updateAfParams(afParams);
        }
    }
}

void
AiqCameraHalAdapter::updateMetaParams(SmartPtr<VideoBuffer>& ispStats){
    LOGI("@%s %d: enter", __FUNCTION__, __LINE__);
    SmartPtr<AiqInputParams> inputParams =  getAiqInputParams_simple();
    XCamAeParam *aeParams = &inputParams->aeInputParams.aeParams;
    XCamAfParam *afParams = &inputParams->afInputParams.afParams;
    XCamAwbParam *awbParams = &inputParams->awbInputParams.awbParams;

    LOGI("@%s %d: enter, inputParams.ptr()(%p)", __FUNCTION__, __LINE__, inputParams.ptr());
    if(!inputParams.ptr()){
        LOGE("@%s inputParams NULL\n", __FUNCTION__);
        return ;
    }


    if (aeParams)
        updateAeMetaParams(aeParams);
    if (afParams)
        updateAfMetaParams(afParams);
    if (awbParams)
        updateAwbMetaParams(awbParams);
    updateOtherMetaParams();

}

void
AiqCameraHalAdapter::updateAeMetaParams(XCamAeParam *aeParams){
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    LOGI("@%s %d: enter", __FUNCTION__, __LINE__);
    Uapi_ExpSwAttr_t stExpSwAttr;
    Uapi_ExpWin_t stExpWin;

    if (_aiq_ctx == NULL){
        LOGE("@%s %d: _aiq_ctx is NULL!\n", __FUNCTION__, __LINE__);
        return;
    }

    LOGI("@%s %d: aeParams pointer (%p) \n", __FUNCTION__, __LINE__, aeParams);

    ret = rk_aiq_user_api_ae_getExpSwAttr(_aiq_ctx, &stExpSwAttr);
    if (ret) {
        LOGE("%s(%d) getExpSwAttr failed!\n", __FUNCTION__, __LINE__);
    }

    ret = rk_aiq_user_api_ae_getExpWinAttr(_aiq_ctx, &stExpWin);
    if (ret) {
        LOGE("%s(%d) getExpWinAttr failed!\n", __FUNCTION__, __LINE__);
    }

    /* Aemode */
    if (aeParams->mode == XCAM_AE_MODE_MANUAL) {
        stExpSwAttr.AecOpType = RK_AIQ_OP_MODE_MANUAL;
        stExpSwAttr.stAntiFlicker.Mode = AEC_ANTIFLICKER_NORMAL_MODE;
        stExpSwAttr.stManual.stLinMe.ManualTimeEn = true;
        stExpSwAttr.stManual.stLinMe.ManualGainEn = true;
    }
    else {
        stExpSwAttr.AecOpType = RK_AIQ_OP_MODE_AUTO;
        stExpSwAttr.stAntiFlicker.Mode = AEC_ANTIFLICKER_AUTO_MODE;
    }

    /* aeAntibanding mode */
    uint8_t flickerMode = aeParams->flicker_mode;
    switch (aeParams->flicker_mode) {
    case XCAM_AE_FLICKER_MODE_OFF:
        stExpSwAttr.stAntiFlicker.Frequency = AEC_FLICKER_FREQUENCY_OFF;
        break;
    case XCAM_AE_FLICKER_MODE_50HZ:
        stExpSwAttr.stAntiFlicker.Frequency = AEC_FLICKER_FREQUENCY_50HZ;
        break;
    case XCAM_AE_FLICKER_MODE_60HZ:
        stExpSwAttr.stAntiFlicker.Frequency = AEC_FLICKER_FREQUENCY_60HZ;
        break;
    case XCAM_AE_FLICKER_MODE_AUTO:
        //No AUTO
        stExpSwAttr.stAntiFlicker.Frequency = AEC_FLICKER_FREQUENCY_50HZ;
        break;
    default:
        stExpSwAttr.stAntiFlicker.Frequency = AEC_FLICKER_FREQUENCY_50HZ;
        LOGE("ERROR @%s: Unknow flicker mode %d", __FUNCTION__, flickerMode);
    }
    stExpSwAttr.stAntiFlicker.enable = true;

    //TODO Flash mode
    switch (aeParams->flash_mode) {
    case AE_FLASH_MODE_AUTO:
        break;
    case AE_FLASH_MODE_ON:
        break;
    case AE_FLASH_MODE_TORCH:
        break;
    case AE_FLASH_MODE_OFF:
        break;
    default:
        //stExpSwAttr.stAntiFlicker.Frequency = AEC_FLICKER_FREQUENCY_50HZ;
        LOGD("@%s: flash mode need TODO %d", __FUNCTION__, aeParams->flash_mode);
    }

    /*auto ExpTimeRange & ExpGainRange*/
    stExpSwAttr.stAuto.stLinAeRange.stExpTimeRange.Max = (float) aeParams->exposure_time_max / 1000.0 / 1000.0 / 1000.0;
    stExpSwAttr.stAuto.stLinAeRange.stExpTimeRange.Min = (float) aeParams->exposure_time_min /  1000.0 / 1000.0 / 1000.0;
    stExpSwAttr.stAuto.stLinAeRange.stGainRange.Max = aeParams->max_analog_gain;
    for(int i = 0; i < 3; i++) {
        stExpSwAttr.stAuto.stHdrAeRange.stExpTimeRange[i].Max = (float) aeParams->exposure_time_max / 1000.0 / 1000.0 / 1000.0;
        stExpSwAttr.stAuto.stHdrAeRange.stExpTimeRange[i].Min = (float) aeParams->exposure_time_min / 1000.0 / 1000.0 / 1000.0;
        stExpSwAttr.stAuto.stHdrAeRange.stGainRange[i].Max = (float) aeParams->max_analog_gain;
    }

    /*Manual exposure time & gain*/
    stExpSwAttr.stManual.stLinMe.TimeValue = (float)aeParams->manual_exposure_time / 1000.0 / 1000.0 / 1000.0;
    stExpSwAttr.stManual.stLinMe.GainValue = (float)aeParams->manual_analog_gain;
    stExpSwAttr.stManual.stHdrMe.TimeValue.fCoeff[0] = (float)aeParams->manual_exposure_time / 1000.0 / 1000.0 / 1000.0;
    stExpSwAttr.stManual.stHdrMe.GainValue.fCoeff[0] = (float)aeParams->manual_analog_gain;

    /* AE region */
    //stExpSwAttr.metering_mode =  aeParams->metering_mode
    if (aeParams->window.x_end - aeParams->window.x_start > 0) {
        stExpWin.h_offs = aeParams->window.x_start;
        stExpWin.v_offs = aeParams->window.y_start;
        stExpWin.h_size = aeParams->window.x_end - aeParams->window.x_start;
        stExpWin.v_size = aeParams->window.y_end - aeParams->window.y_start;
    }

    pthread_mutex_lock(&_aiq_ctx_mutex);
    //when in Locked state, not run AE Algorithm
    //TODO: Need lock/unlock set api
    if (mAeState->getState() != ANDROID_CONTROL_AE_STATE_LOCKED) {
        LOGD("%s(%d) AE_STATE_UNLOCKED !\n", __FUNCTION__, __LINE__);
        ret = rk_aiq_user_api_ae_setExpSwAttr(_aiq_ctx, stExpSwAttr);
        if (ret) {
            LOGE("%s(%d) setExpSwAttr failed!\n", __FUNCTION__, __LINE__);
        }
        ret = rk_aiq_user_api_ae_setExpWinAttr(_aiq_ctx, stExpWin);
        if (ret) {
            LOGE("%s(%d) setExpWinAttr failed!\n", __FUNCTION__, __LINE__);
        }
    }
    pthread_mutex_unlock(&_aiq_ctx_mutex);
}

void
AiqCameraHalAdapter::updateAfMetaParams(XCamAfParam *afParams){
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    LOGI("@%s %d: enter", __FUNCTION__, __LINE__);
    rk_aiq_af_attrib_t stAfttr;

    if (_aiq_ctx == NULL){
        LOGE("@%s %d: _aiq_ctx is NULL!\n", __FUNCTION__, __LINE__);
        return;
    }

    ret = rk_aiq_user_api_af_GetAttrib(_aiq_ctx, &stAfttr);
    if (ret) {
        LOGE("%s(%d) Af GetAttrib failed!\n", __FUNCTION__, __LINE__);
    }

    //TODO  afParams->trigger_new_search 
//    if (afParams->trigger_new_search) {
//        //TODO ANDROID_CONTROL_AF_TRIGGER_START
//    }
//    else {
//        //TODO ANDROID_CONTROL_AF_TRIGGER_CANCEL
        //or ANDROID_CONTROL_AF_TRIGGER_IDLE
//    }

    XCamAfOperationMode afMode = afParams->focus_mode;
    switch (afMode) {
        case AF_MODE_CONTINUOUS_VIDEO:
            stAfttr.AfMode = RKAIQ_AF_MODE_CONTINUOUS_VIDEO;
            //TODO 
            //stAfttr.focus_range = AF_RANGE_NORMAL;
            break;
        case AF_MODE_CONTINUOUS_PICTURE:
            stAfttr.AfMode = RKAIQ_AF_MODE_CONTINUOUS_PICTURE;
            //TODO 
            //stAfttr.focus_range = AF_RANGE_NORMAL;
            break;
        case AF_MODE_MACRO:
            stAfttr.AfMode = RKAIQ_AF_MODE_MACRO;
            //TODO 
            //stAfttr.focus_range = AF_RANGE_MACRO;
            break;
        case AF_MODE_EDOF:
            stAfttr.AfMode = RKAIQ_AF_MODE_EDOF;
            //TODO 
            //stAfttr.focus_range = AF_RANGE_EXTENDED;
            break;
        case AF_MODE_AUTO:
            // TODO: switch to operation_mode_auto, similar to MACRO AF
            stAfttr.AfMode = RKAIQ_AF_MODE_AUTO;
            //TODO 
            //stAfttr.focus_range = AF_RANGE_EXTENDED;
            break;
        default:
            LOGE("ERROR @%s: Unknown focus mode %d- using auto",
                    __FUNCTION__, afMode);
            stAfttr.AfMode = RKAIQ_AF_MODE_AUTO;
            //TODO 
            //stAfttr.focus_range = AF_RANGE_EXTENDED;
            break;
    }

    /* metering mode */
    //TODO

    /* AF region */
    if (afParams->focus_rect[0].right_width > 0 && afParams->focus_rect[0].bottom_height > 0) {
        stAfttr.h_offs = afParams->focus_rect[0].left_hoff;
        stAfttr.v_offs = afParams->focus_rect[0].top_voff;
        stAfttr.h_size= afParams->focus_rect[0].right_width;
        stAfttr.v_size = afParams->focus_rect[0].bottom_height;
    }

    pthread_mutex_lock(&_aiq_ctx_mutex);
    //when in Locked state, not run AF Algorithm
    if (mAfState->getState() != ANDROID_CONTROL_AF_STATE_FOCUSED_LOCKED) {
        ret = rk_aiq_uapi_lockFocus(_aiq_ctx);
        if (ret) {
            LOGE("%s(%d) Af set unlock failed!\n", __FUNCTION__, __LINE__);
        }

        ret = rk_aiq_user_api_af_SetAttrib(_aiq_ctx, &stAfttr);
        if (ret) {
            LOGE("%s(%d) Af SetAttrib failed!\n", __FUNCTION__, __LINE__);
        }
    } else {
        ret = rk_aiq_uapi_unlockFocus(_aiq_ctx);
        if (ret) {
            LOGE("%s(%d) Af set lock failed!\n", __FUNCTION__, __LINE__);
        }
    }
    pthread_mutex_unlock(&_aiq_ctx_mutex);

    return;
}

void
AiqCameraHalAdapter::updateAwbMetaParams(XCamAwbParam *awbParams){
    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    LOGI("@%s %d: enter", __FUNCTION__, __LINE__);
    rk_aiq_wb_attrib_t stAwbttr;
    rk_aiq_ccm_attrib_t setCcm;

    if (_aiq_ctx == NULL){
        LOGE("@%s %d: _aiq_ctx is NULL!\n", __FUNCTION__, __LINE__);
        return;
    }

    ret = rk_aiq_user_api_accm_GetAttrib(_aiq_ctx, &setCcm);
    if (ret) {
        LOGE("%s(%d) Awb GetAttrib failed!\n", __FUNCTION__, __LINE__);
    }

    ret = rk_aiq_user_api_awb_GetAttrib(_aiq_ctx, &stAwbttr);
    if (ret) {
        LOGE("%s(%d) Awb GetAttrib failed!\n", __FUNCTION__, __LINE__);
    }
    switch (awbParams->mode) {
    case XCAM_AWB_MODE_MANUAL:
        stAwbttr.mode = RK_AIQ_WB_MODE_MANUAL;
        break;
    case XCAM_AWB_MODE_AUTO:
        stAwbttr.mode = RK_AIQ_WB_MODE_AUTO;
        break;
    case XCAM_AWB_MODE_WARM_INCANDESCENT:
        stAwbttr.mode = RK_AIQ_WB_MODE_MANUAL;
        stAwbttr.stManual.mode = RK_AIQ_MWB_MODE_SCENE;
        stAwbttr.stManual.para.scene = RK_AIQ_WBCT_INCANDESCENT;
        break;
    case XCAM_AWB_MODE_FLUORESCENT:
        stAwbttr.mode = RK_AIQ_WB_MODE_MANUAL;
        stAwbttr.stManual.mode = RK_AIQ_MWB_MODE_SCENE;
        stAwbttr.stManual.para.scene = RK_AIQ_WBCT_FLUORESCENT;
        break;
    case XCAM_AWB_MODE_WARM_FLUORESCENT:
        stAwbttr.mode = RK_AIQ_WB_MODE_MANUAL;
        stAwbttr.stManual.mode = RK_AIQ_MWB_MODE_SCENE;
        stAwbttr.stManual.para.scene = RK_AIQ_WBCT_WARM_FLUORESCENT;
        break;
    case XCAM_AWB_MODE_DAYLIGHT:
        stAwbttr.mode = RK_AIQ_WB_MODE_MANUAL;
        stAwbttr.stManual.mode = RK_AIQ_MWB_MODE_SCENE;
        stAwbttr.stManual.para.scene = RK_AIQ_WBCT_DAYLIGHT;
        break;
    case XCAM_AWB_MODE_CLOUDY:
        stAwbttr.mode = RK_AIQ_WB_MODE_MANUAL;
        stAwbttr.stManual.mode = RK_AIQ_MWB_MODE_SCENE;
        stAwbttr.stManual.para.scene = RK_AIQ_WBCT_CLOUDY_DAYLIGHT;
        break;
    case XCAM_AWB_MODE_SHADOW:
        stAwbttr.mode = RK_AIQ_WB_MODE_MANUAL;
        stAwbttr.stManual.mode = RK_AIQ_MWB_MODE_SCENE;
        stAwbttr.stManual.para.scene = RK_AIQ_WBCT_SHADE;
        break;
    default :
        stAwbttr.mode = RK_AIQ_WB_MODE_AUTO;
        break;
    }

    /* AWB region */
    /* TODO no struct contain the awb region,  need add */
    if (awbParams->window.x_end - awbParams->window.x_start > 0) {
//        stAwbttr.region.x_start = awbParams->window.x_start;
//        stAwbttr.region.y_start = awbParams->window.y_start;
//        stAwbttr.region.x_end = awbParams->window.x_end;
//        stAwbttr.region.y_end = awbParams->window.y_end;
//        stAwbttr.region.weight = awbParams->window.weight;
    }

    /* colorCorrection.gains */
    if (awbParams->mode == XCAM_AWB_MODE_MANUAL) {
        if (awbParams->r_gain) {
            stAwbttr.mode = RK_AIQ_WB_MODE_MANUAL;
            stAwbttr.stManual.mode = RK_AIQ_MWB_MODE_WBGAIN;
            stAwbttr.stManual.para.gain.rgain = awbParams->r_gain;
            stAwbttr.stManual.para.gain.grgain = awbParams->gr_gain;
            stAwbttr.stManual.para.gain.gbgain = awbParams->gb_gain;
            stAwbttr.stManual.para.gain.bgain = awbParams->b_gain;
        }

        /* MANUAL COLOR CORRECTION */
        /* normal auto according color Temperature */
        if (awbParams->is_ccm_valid) {
            setCcm.mode = RK_AIQ_CCM_MODE_MANUAL;
            setCcm.byPass = false;
            memcpy(setCcm.stManual.matrix,  awbParams->ccm_matrix, sizeof(float)*9);
        } else {
            setCcm.mode = RK_AIQ_CCM_MODE_AUTO;
        }
    }

    pthread_mutex_lock(&_aiq_ctx_mutex);
    //when in Locked state, not run AWB Algorithm
    if (mAwbState->getState() != ANDROID_CONTROL_AWB_STATE_LOCKED) {
        ret = rk_aiq_uapi_unlockAWB(_aiq_ctx);
        if (ret) {
            LOGE("%s(%d) Awb Set unlock failed!\n", __FUNCTION__, __LINE__);
        }

        ret = rk_aiq_user_api_accm_SetAttrib(_aiq_ctx, setCcm);
        if (ret) {
            LOGE("%s(%d) accm SetAttrib failed!\n", __FUNCTION__, __LINE__);
        }
        ret = rk_aiq_user_api_awb_SetAttrib(_aiq_ctx, stAwbttr);
        if (ret) {
            LOGE("%s(%d) Awb SetAttrib failed!\n", __FUNCTION__, __LINE__);
        }

    } else {
        ret = rk_aiq_uapi_lockAWB(_aiq_ctx);
        if (ret) {
            LOGE("%s(%d) Awb Set lock failed!\n", __FUNCTION__, __LINE__);
        }
    }
    pthread_mutex_unlock(&_aiq_ctx_mutex);
    return;

}

void
AiqCameraHalAdapter::updateOtherMetaParams(){
    LOGI("@%s %d: enter", __FUNCTION__, __LINE__);
    if (_aiq_ctx == NULL){
        LOGE("@%s %d: _aiq_ctx is NULL!\n", __FUNCTION__, __LINE__);
        return;
    }

}

bool
AiqCameraHalAdapter::set_sensor_mode_data (rk_aiq_exposure_sensor_descriptor *sensor_mode,
                                      bool first)
{
    SmartPtr<AiqInputParams> inputParams = getAiqInputParams_simple();
    static enum USE_CASE old_usecase = UC_PREVIEW;

    if(_inputParams.ptr()) {
        uint8_t new_aestate = mAeState->getState();
        enum USE_CASE cur_usecase = old_usecase;
        enum USE_CASE new_usecase = old_usecase;
        AiqFrameUseCase frameUseCase = _inputParams->frameUseCase;
        if (new_aestate == ANDROID_CONTROL_AE_STATE_PRECAPTURE &&
            _inputParams->aeInputParams.aeParams.flash_mode != AE_FLASH_MODE_TORCH &&
            /* ignore the video snapshot case */
            _inputParams->aeInputParams.aeParams.flash_mode != AE_FLASH_MODE_OFF
            ) {
            new_usecase = UC_PRE_CAPTRUE;
            if (frameUseCase == AIQ_FRAME_USECASE_STILL_CAPTURE)
                _delay_still_capture = true;
        } else {
            switch (cur_usecase ) {
            case UC_PREVIEW:
                // TODO: preview to capture directly, don't change usecase now
                /* if (frameUseCase == AIQ_FRAME_USECASE_STILL_CAPTURE) */
                /*     new_usecase = UC_CAPTURE; */
                if (frameUseCase == AIQ_FRAME_USECASE_VIDEO_RECORDING)
                    new_usecase = UC_RECORDING;
                break;
            case UC_PRE_CAPTRUE:
                if ((new_aestate == ANDROID_CONTROL_AE_STATE_CONVERGED ||
                    new_aestate == ANDROID_CONTROL_AE_STATE_LOCKED ||
                    new_aestate == ANDROID_CONTROL_AE_STATE_FLASH_REQUIRED) &&
                    (frameUseCase == AIQ_FRAME_USECASE_STILL_CAPTURE ||
                     first || _delay_still_capture)) {
                    _delay_still_capture = false;
                    new_usecase = UC_CAPTURE;
                // cancel precap
                if (new_aestate == ANDROID_CONTROL_AE_STATE_INACTIVE)
                    new_usecase = UC_PREVIEW;
                break;
            case UC_CAPTURE:
                break;
            case UC_RECORDING:
                if (frameUseCase == AIQ_FRAME_USECASE_PREVIEW)
                    new_usecase = UC_PREVIEW;
                break;
            case UC_RAW:
                break;
            default:
                new_usecase = UC_PREVIEW;
                LOGE("wrong usecase %d", cur_usecase);
            }
        }
        LOGD("@%s (%d) usecase %d -> %d, frameUseCase %d, new_aestate %d \n",
             __FUNCTION__, __LINE__, cur_usecase, new_usecase, frameUseCase, new_aestate);
        old_usecase = new_usecase;
    }
        AAAControls *aaaControls = &_inputParams->aaaControls;
        // update flash mode
        XCamAeParam* aeParam = &_inputParams->aeInputParams.aeParams;

        if (aeParam->flash_mode == AE_FLASH_MODE_AUTO) {
         //TODO Control Flash
        }
        else if (aeParam->flash_mode == AE_FLASH_MODE_ON)
        {
         //TODO Control Flash
        }
        else if (aeParam->flash_mode == AE_FLASH_MODE_TORCH)
        {
         //TODO Control Flash
        }
        else
        {
         //TODO Control Flash
        }
    }
    return true;
}

void
AiqCameraHalAdapter::processResults(SmartPtr<RkAiqFullParamsProxy> &results){
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    LOGD("@%s %d: enter", __FUNCTION__, __LINE__);
    RkAiqFullParams* aiqParams = results->data().ptr();
    SmartPtr<rk_aiq_exposure_params_wrapper_t> exp_param = aiqParams->mExposureParams->data();
    SmartPtr<rk_aiq_focus_params_t> focus_param = aiqParams->mFocusParams->data();
    SmartPtr<rk_aiq_isp_params_t> isp_param = aiqParams->mIspParams->data();
    SmartPtr<AiqInputParams> inputParams =  _inputParams;

    int id = inputParams.ptr() ? inputParams->reqId : -1;
    LOGI("@%s %d: inputParams.ptr() (%p) id (%d)", __FUNCTION__, __LINE__, inputParams.ptr(), id);

    //when id = -1, means no inputParams set;
    if( mCallbackOps && id != -1){

        LOGD("@%s %d: workingMode(%s)", __FUNCTION__, __LINE__, _work_mode == RK_AIQ_WORKING_MODE_NORMAL? "MODE_NORMAL":"MODE_HDR");

        rk_aiq_ae_results ae_results;
        ret = getAeResults(ae_results);
        if (ret) {
            LOGE("%s(%d) getAeResults failed, ae meta is invalid!\n", __FUNCTION__, __LINE__);
        } else {
            if (_inputParams.ptr()) {
                processAeMetaResults(ae_results, _metadata);
            }
        }

        //convert to af_results, need zyc TODO query interface
        rk_aiq_af_results af_results;
        ret = getAfResults(af_results);
        if (ret) {
            LOGE("%s(%d) getAeResults failed, ae meta is invalid!\n", __FUNCTION__, __LINE__);
        } else {
            if (_inputParams.ptr()) {
                processAfMetaResults(af_results, _metadata);
            }
        }

        //convert to awb_results
        rk_aiq_awb_results awb_results;
        ret = getAwbResults(awb_results);
        if (ret) {
            LOGE("%s(%d) getAeResults failed, ae meta is invalid!\n", __FUNCTION__, __LINE__);
        } else {
            if (_inputParams.ptr()) {
                processAwbMetaResults(awb_results, _metadata);
            }
        }

        if (_inputParams.ptr()) {
            processMiscMetaResults(_metadata);
        }
    }
    setAiqInputParams(NULL);
    camera_metadata_entry entry;
    entry = _metadata->find(ANDROID_REQUEST_ID);
    if (entry.count == 1)
        id = entry.data.i32[0];

    LOGI("@%s %d:_metadata ANDROID_REQUEST_ID (%d)", __FUNCTION__, __LINE__, id);
    /* meta_result->dump(); */
    {
        SmartLock lock(_settingsMutex);
        if (!_fly_settings.empty())
            LOGI("@%s %d: flying id %d", __FUNCTION__, __LINE__, (*_fly_settings.begin())->reqId);
        if (!_fly_settings.empty() && (id == (*_fly_settings.begin())->reqId)) {
            _fly_settings.erase(_fly_settings.begin());
            LOGD("_fly_settings.size():%d,  _settings.size():%d",_fly_settings.size(), _settings.size());
        } else {
            // return every meta result, we use meta to do extra work, eg.
            // flash stillcap synchronization
            id = -1;
        }
    }

    rkisp_cl_frame_metadata_s cb_result;
    cb_result.id = id;
    cb_result.metas = _metadata->getAndLock();
    if (mCallbackOps)
        mCallbackOps->metadata_result_callback(mCallbackOps, &cb_result);
    _metadata->unlock(cb_result.metas);
}

void
AiqCameraHalAdapter::getAeResultsDebug(rk_aiq_ae_results &ae_results,
                     SmartPtr<rk_aiq_exposure_params_wrapper_t> exp_param)
{
    LOGD("@%s %d: enter", __FUNCTION__, __LINE__);
    RkAiqAlgoProcResAeInt* ae_proc_res = (RkAiqAlgoProcResAeInt*)_analyzer->mAlogsSharedParams.procResComb.ae_proc_res;

    //need zyc TODO get interface
    if(_work_mode == RK_AIQ_WORKING_MODE_NORMAL /*linear*/) {
        ae_results.exposure.exposure_time_us = exp_param->aecExpInfo.LinearExp.exp_real_params.integration_time * 1000 * 1000; //us or ns?
        ae_results.exposure.analog_gain = exp_param->aecExpInfo.LinearExp.exp_real_params.analog_gain;
        ae_results.exposure.iso = exp_param->aecExpInfo.LinearExp.exp_real_params.iso;

        //current useless, for future use
        ae_results.exposure.digital_gain = exp_param->aecExpInfo.LinearExp.exp_real_params.digital_gain;
        //sensor exposure params
        ae_results.sensor_exposure.coarse_integration_time = exp_param->aecExpInfo.LinearExp.exp_sensor_params.coarse_integration_time;
        ae_results.sensor_exposure.analog_gain_code_global = exp_param->aecExpInfo.LinearExp.exp_sensor_params.analog_gain_code_global;
        ae_results.sensor_exposure.fine_integration_time =  exp_param->aecExpInfo.LinearExp.exp_sensor_params.fine_integration_time;
        ae_results.sensor_exposure.digital_gain_global =  exp_param->aecExpInfo.LinearExp.exp_sensor_params.digital_gain_global;
    } else {
        /* Hdr need zyc TODO */
        //ae_results.exposure.exposure_time_us = exp_param->aecExpInfo.HdrExp[3];
        ae_results.exposure.exposure_time_us = exp_param->aecExpInfo.HdrExp[0].exp_real_params.integration_time * 1000 * 1000; //us or ns?
        ae_results.exposure.analog_gain = exp_param->aecExpInfo.HdrExp[0].exp_real_params.analog_gain;
        ae_results.exposure.iso = exp_param->aecExpInfo.HdrExp[0].exp_real_params.iso;

        //current useless, for future use
        ae_results.exposure.digital_gain = exp_param->aecExpInfo.HdrExp[0].exp_real_params.digital_gain;
        //sensor exposure params
        ae_results.sensor_exposure.coarse_integration_time = exp_param->aecExpInfo.HdrExp[0].exp_sensor_params.coarse_integration_time;
        ae_results.sensor_exposure.analog_gain_code_global = exp_param->aecExpInfo.HdrExp[0].exp_sensor_params.analog_gain_code_global;
        ae_results.sensor_exposure.fine_integration_time =  exp_param->aecExpInfo.HdrExp[0].exp_sensor_params.fine_integration_time;
        ae_results.sensor_exposure.digital_gain_global =  exp_param->aecExpInfo.HdrExp[0].exp_sensor_params.digital_gain_global;
    }
    ae_results.sensor_exposure.frame_length_lines =  exp_param->aecExpInfo.frame_length_lines;
    ae_results.sensor_exposure.line_length_pixels =  exp_param->aecExpInfo.line_length_pixels;
    //need zyc RKAIQ TODO
    //ae_results.converged = exp_param...
    ae_results.converged = ae_proc_res->ae_proc_res_rk.IsConverged;
    LOGE("@%s ae_results.converged:%d",__FUNCTION__, ae_results.converged);
}

XCamReturn
AiqCameraHalAdapter::getAeResults(rk_aiq_ae_results &ae_results)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    LOGD("@%s %d: enter", __FUNCTION__, __LINE__);
    Uapi_ExpQueryInfo_t gtExpResInfo;

    ret = rk_aiq_user_api_ae_queryExpResInfo(_aiq_ctx, &gtExpResInfo);
    if (ret) {
        LOGE("%s(%d) queryExpResInfo failed!\n", __FUNCTION__, __LINE__);
        return ret;
    }

    if(_work_mode == RK_AIQ_WORKING_MODE_NORMAL /*linear*/) {
        ae_results.exposure.exposure_time_us = gtExpResInfo.CurExpInfo.LinearExp.exp_real_params.integration_time * 1000 * 1000; //us or ns?
        ae_results.exposure.analog_gain = gtExpResInfo.CurExpInfo.LinearExp.exp_real_params.analog_gain;
        ae_results.exposure.iso = gtExpResInfo.CurExpInfo.LinearExp.exp_real_params.iso;

        //current useless, for future use
        ae_results.exposure.digital_gain = gtExpResInfo.CurExpInfo.LinearExp.exp_real_params.digital_gain;
        //sensor exposure params
        ae_results.sensor_exposure.coarse_integration_time = gtExpResInfo.CurExpInfo.LinearExp.exp_sensor_params.coarse_integration_time;
        ae_results.sensor_exposure.analog_gain_code_global = gtExpResInfo.CurExpInfo.LinearExp.exp_sensor_params.analog_gain_code_global;
        ae_results.sensor_exposure.fine_integration_time =  gtExpResInfo.CurExpInfo.LinearExp.exp_sensor_params.fine_integration_time;
        ae_results.sensor_exposure.digital_gain_global =  gtExpResInfo.CurExpInfo.LinearExp.exp_sensor_params.digital_gain_global;
    } else {
        /* Hdr need zyc TODO */
        ae_results.exposure.exposure_time_us = gtExpResInfo.CurExpInfo.HdrExp[0].exp_real_params.integration_time * 1000 * 1000; //us or ns?
        ae_results.exposure.analog_gain = gtExpResInfo.CurExpInfo.HdrExp[0].exp_real_params.analog_gain;
        ae_results.exposure.iso = gtExpResInfo.CurExpInfo.HdrExp[0].exp_real_params.iso;

        //current useless, for future use
        ae_results.exposure.digital_gain =gtExpResInfo.CurExpInfo.HdrExp[0].exp_real_params.digital_gain;
        //sensor exposure params
        ae_results.sensor_exposure.coarse_integration_time = gtExpResInfo.CurExpInfo.HdrExp[0].exp_sensor_params.coarse_integration_time;
        ae_results.sensor_exposure.analog_gain_code_global = gtExpResInfo.CurExpInfo.HdrExp[0].exp_sensor_params.analog_gain_code_global;
        ae_results.sensor_exposure.fine_integration_time = gtExpResInfo.CurExpInfo.HdrExp[0].exp_sensor_params.fine_integration_time;
        ae_results.sensor_exposure.digital_gain_global = gtExpResInfo.CurExpInfo.HdrExp[0].exp_sensor_params.digital_gain_global;
    }
    ae_results.sensor_exposure.frame_length_lines = gtExpResInfo.CurExpInfo.frame_length_lines;
    ae_results.sensor_exposure.line_length_pixels = gtExpResInfo.CurExpInfo.line_length_pixels;
    ae_results.converged = gtExpResInfo.IsConverged;
    LOGE("@%s ae_results.converged:%d",__FUNCTION__, ae_results.converged);
    return ret;
}

void
AiqCameraHalAdapter::getAfResultsDebug(rk_aiq_af_results &af_results,
                        SmartPtr<rk_aiq_focus_params_t> focus_param)
{
    LOGD("@%s %d:", __FUNCTION__, __LINE__);
    RkAiqAlgoProcResAfInt* af_pre_res_int = (RkAiqAlgoProcResAfInt*)_analyzer->mAlogsSharedParams.procResComb.af_proc_res;
    RkAiqAlgoProcResAf* af_com = _analyzer->mAlogsSharedParams.procResComb.af_proc_res;

    //need zyc TODO get interface
    af_results.next_lens_position = focus_param->next_lens_pos;
    /* // RKAIQ 接口还缺如下参数， TODO
    af_results.afc_config = focus_param->afc_config;
    af_results.current_focus_distance = focus_param->current_focus_distance;
    //主要以下两个
    af_results.status = focus_param->status;
    af_results.final_lens_position_reached = focus_param->final_lens_position_reached; */
}

XCamReturn
AiqCameraHalAdapter::getAfResults(rk_aiq_af_results &af_results)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    LOGD("@%s %d:", __FUNCTION__, __LINE__);
    rk_aiq_af_sec_path_t gtAfPath;

    ret = rk_aiq_user_api_af_GetSearchPath(_aiq_ctx, &gtAfPath);
    if (ret) {
        LOGE("%s(%d) GetSearchPath failed!\n", __FUNCTION__, __LINE__);
        return ret;
    }
    af_results.next_lens_position = gtAfPath.search_num;
    switch (gtAfPath.stat) {
        case RK_AIQ_AF_SEARCH_RUNNING:
            af_results.status = rk_aiq_af_status_local_search;
            af_results.final_lens_position_reached = false;
            break;
        case RK_AIQ_AF_SEARCH_END:
            af_results.status = rk_aiq_af_status_success;
            af_results.final_lens_position_reached = true;
            break;
        case RK_AIQ_AF_SEARCH_INVAL:
            af_results.status = rk_aiq_af_status_success;
            af_results.final_lens_position_reached = true;
            break;
        default:
            LOGE("ERROR @%s: Unknown af status %d- using idle",
                    __FUNCTION__, af_results.status);
             af_results.status = rk_aiq_af_status_idle;
            break;
    }


    return ret;
}

void
AiqCameraHalAdapter::getAwbResultsDebug(rk_aiq_awb_results &awb_results,
                        SmartPtr<rk_aiq_isp_params_t> isp_param)
{
    LOGD("@%s %d: enter", __FUNCTION__, __LINE__);
    RkAiqAlgoProcResAwbInt* awb_pre_res_int = (RkAiqAlgoProcResAwbInt*)_analyzer->mAlogsSharedParams.procResComb.awb_proc_res;

    //need zyc TODO get interface
    awb_results.awb_gain_cfg.enabled = true; //isp_param->awb_gain.awb_gain_update;
    awb_results.awb_gain_cfg.awb_gains.red_gain = isp_param->awb_gain.rgain == 0 ? 394 : isp_param->awb_gain.rgain;
    awb_results.awb_gain_cfg.awb_gains.green_b_gain= isp_param->awb_gain.gbgain == 0 ? 256 : isp_param->awb_gain.gbgain;
    awb_results.awb_gain_cfg.awb_gains.green_r_gain= isp_param->awb_gain.grgain == 0 ? 256 : isp_param->awb_gain.grgain;
    awb_results.awb_gain_cfg.awb_gains.blue_gain= isp_param->awb_gain.bgain == 0 ? 296 : isp_param->awb_gain.bgain;
    memcpy(awb_results.ctk_config.ctk_matrix.coeff, isp_param->ccm.matrix, sizeof(unsigned int)*9);

    //need RKAIQ TODO
    //awb_results.converged = isp_param...
    awb_results.converged = awb_pre_res_int->awbConverged;
    LOGE("@%s awb_results.converged:%d", __FUNCTION__, awb_results.converged);
}

XCamReturn
AiqCameraHalAdapter::getAwbResults(rk_aiq_awb_results &awb_results)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    LOGD("@%s %d: enter", __FUNCTION__, __LINE__);
    rk_aiq_wb_querry_info_t query_info;
    rk_aiq_ccm_querry_info_t ccm_querry_info;

    ret = rk_aiq_user_api_awb_QueryWBInfo(_aiq_ctx, &query_info);
    if (ret) {
        LOGE("%s(%d) QueryWBInfo failed!\n", __FUNCTION__, __LINE__);
        return ret;
    }


    awb_results.awb_gain_cfg.enabled = true; //isp_param->awb_gain.awb_gain_update;
    awb_results.awb_gain_cfg.awb_gains.red_gain = query_info.gain.rgain == 0 ? 394 : query_info.gain.rgain;
    awb_results.awb_gain_cfg.awb_gains.green_b_gain= query_info.gain.gbgain == 0 ? 256 : query_info.gain.gbgain ;
    awb_results.awb_gain_cfg.awb_gains.green_r_gain= query_info.gain.grgain  == 0 ? 256 : query_info.gain.grgain;
    awb_results.awb_gain_cfg.awb_gains.blue_gain= query_info.gain.bgain == 0 ? 296 : query_info.gain.bgain;
    awb_results.converged = query_info.awbConverged;
    LOGD("@%s awb_results.converged:%d", __FUNCTION__, awb_results.converged);

    ret = rk_aiq_user_api_accm_QueryCcmInfo(_aiq_ctx, &ccm_querry_info);
    if (ret) {
        LOGE("%s(%d) QueryCcmInfo failed!\n", __FUNCTION__, __LINE__);
        return ret;
    }
    memcpy(awb_results.ctk_config.ctk_matrix.coeff, ccm_querry_info.matrix, sizeof(float)*9);
    LOGD("@%s ccm_en:%s", __FUNCTION__, ccm_querry_info.ccm_en ? "true":"false");

    return ret;
}

XCamReturn
AiqCameraHalAdapter::processAeMetaResults(rk_aiq_ae_results &ae_result, CameraMetadata *metadata){
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    bool is_first_param = false;
    camera_metadata_entry entry;
    SmartPtr<AiqInputParams> inputParams = _inputParams;
    CameraMetadata* staticMeta  = inputParams->staticMeta;

    LOGI("@%s %d: enter", __FUNCTION__, __LINE__);
    XCamAeParam &aeParams = inputParams->aeInputParams.aeParams;
    uint8_t sceneFlickerMode = ANDROID_STATISTICS_SCENE_FLICKER_NONE;
    switch (aeParams.flicker_mode) {
    case XCAM_AE_FLICKER_MODE_50HZ:
        sceneFlickerMode = ANDROID_STATISTICS_SCENE_FLICKER_50HZ;
        break;
    case XCAM_AE_FLICKER_MODE_60HZ:
        sceneFlickerMode = ANDROID_STATISTICS_SCENE_FLICKER_60HZ;
        break;
    default:
        sceneFlickerMode = ANDROID_STATISTICS_SCENE_FLICKER_NONE;
    }
    //# ANDROID_METADATA_Dynamic android.statistics.sceneFlicker done
    metadata->update(ANDROID_STATISTICS_SCENE_FLICKER,
                                    &sceneFlickerMode, 1);

    rk_aiq_exposure_sensor_descriptor sns_des;
    ret = _aiq_ctx->_camHw->getSensorModeData(_aiq_ctx->_sensor_entity_name, sns_des);

    /* exposure in sensor_desc is the actual effective, and the one in
     * aec_results is the latest result calculated from 3a stats and
     * will be effective in future
     */
//    if (!is_first_param) {
//        ae_result.exposure.exposure_time_us = sns_des.exp_time_seconds * 1000 * 1000;
//        ae_result.exposure.analog_gain = sns_des.gains;
//    }
    LOGD("%s exp_time=%d gain=%f, sensor_exposure.frame_length_lines=%d, is_first_parms %d", __FUNCTION__,
            ae_result.exposure.exposure_time_us,
            ae_result.exposure.analog_gain,
            ae_result.sensor_exposure.frame_length_lines,
            is_first_param);

    ret = mAeState->processResult(ae_result, *metadata,
                            inputParams->reqId);

    //# ANDROID_METADATA_Dynamic android.control.aeRegions done
    entry = inputParams->settings.find(ANDROID_CONTROL_AE_REGIONS);
    if (entry.count == 5)
        metadata->update(ANDROID_CONTROL_AE_REGIONS, inputParams->aeInputParams.aeRegion, entry.count);

    //# ANDROID_METADATA_Dynamic android.control.aeExposureCompensation done
    // TODO get step size (currently 1/3) from static metadata
    int32_t exposureCompensation =
            round((aeParams.ev_shift) * 3);

    metadata->update(ANDROID_CONTROL_AE_EXPOSURE_COMPENSATION,
                                    &exposureCompensation,
                                    1);


    int64_t exposureTime = 0;
    uint16_t pixels_per_line = 0;
    uint16_t lines_per_frame = 0;
    int64_t manualExpTime = 1;

        // Calculate frame duration from AE results and sensor descriptor
        pixels_per_line = ae_result.sensor_exposure.line_length_pixels;
        //lines_per_frame = ae_result.sensor_exposure.frame_length_lines;
        /*
         * Android wants the frame duration in nanoseconds
         */
         lines_per_frame  =
                (sns_des.line_periods_per_field < ae_result.sensor_exposure.frame_length_lines) ?
                ae_result.sensor_exposure.frame_length_lines : sns_des.line_periods_per_field;
        int64_t frameDuration = (pixels_per_line * lines_per_frame) /
                                sns_des.pixel_clock_freq_mhz;
        frameDuration *= 1000;
        //TO DO
        //metadata->update(ANDROID_SENSOR_FRAME_DURATION, &frameDuration, 1);
    exposureTime = ae_result.exposure.exposure_time_us *1000;
    metadata->update(ANDROID_SENSOR_EXPOSURE_TIME,
                                     &exposureTime, 1);

    int32_t ExposureGain = ae_result.exposure.analog_gain * 100; //use iso instead
    int32_t Iso = ae_result.exposure.iso;
    /* The sensitivity is the standard ISO sensitivity value, as defined in ISO 12232:2006. */
    metadata->update(ANDROID_SENSOR_SENSITIVITY,
                                     &Iso, 1);

    int32_t value = ANDROID_SENSOR_TEST_PATTERN_MODE_OFF;
    
    entry = staticMeta->find(ANDROID_SENSOR_TEST_PATTERN_MODE);
    if (entry.count == 1)
        value = entry.data.i32[0];

    metadata->update(ANDROID_SENSOR_TEST_PATTERN_MODE,
                                     &value, 1);

    // update expousre range
    int64_t exptime_range_us[2];
    entry = staticMeta->find(ANDROID_SENSOR_INFO_EXPOSURE_TIME_RANGE);
    if (entry.count == 2) {
        exptime_range_us[0] = entry.data.i64[0];
        exptime_range_us[1] = entry.data.i64[1];
        metadata->update(ANDROID_SENSOR_INFO_EXPOSURE_TIME_RANGE, exptime_range_us, 2);
    }

    int32_t sensitivity_range[2];
    entry = staticMeta->find(ANDROID_SENSOR_INFO_SENSITIVITY_RANGE);
    if (entry.count == 2) {
        sensitivity_range[0] = entry.data.i32[0];
        sensitivity_range[1] = entry.data.i32[1];
        metadata->update(ANDROID_SENSOR_INFO_SENSITIVITY_RANGE, sensitivity_range, 2);
    }


    entry = staticMeta->find(ANDROID_CONTROL_AE_MODE);
    if (entry.count == 1 /*&&
        aec_results.flashModeState != AEC_FLASH_PREFLASH &&
        aec_results.flashModeState != AEC_FLASH_MAINFLASH*/) {
        uint8_t stillcap_sync = false;
        if (entry.data.u8[0] == ANDROID_CONTROL_AE_MODE_ON_ALWAYS_FLASH ||
            (entry.data.u8[0] == ANDROID_CONTROL_AE_MODE_ON_AUTO_FLASH /*&&
            aec_results.require_flash*/))
            stillcap_sync = true;
        else
            stillcap_sync = false;
        metadata->update(RKCAMERA3_PRIVATEDATA_STILLCAP_SYNC_NEEDED,
                         &stillcap_sync, 1);
    }
    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
AiqCameraHalAdapter::processAfMetaResults(rk_aiq_af_results &af_result, CameraMetadata *metadata){
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    SmartPtr<AiqInputParams> inputParams = _inputParams;
    camera_metadata_entry entry;
    LOGI("@%s %d: enter", __FUNCTION__, __LINE__);

    XCamAfParam &afParams = inputParams->afInputParams.afParams;
    entry = inputParams->settings.find(ANDROID_CONTROL_AF_REGIONS);
    if (entry.count == 5)
        metadata->update(ANDROID_CONTROL_AF_REGIONS, inputParams->afInputParams.afRegion, entry.count);

    ret = mAfState->processResult(af_result, afParams, *metadata);
    return ret;
}

XCamReturn
AiqCameraHalAdapter::processAwbMetaResults(rk_aiq_awb_results &awb_result, CameraMetadata *metadata){
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    SmartPtr<AiqInputParams> inputParams = _inputParams;
    camera_metadata_entry entry;
    LOGI("@%s %d: enter", __FUNCTION__, __LINE__);

    ret = mAwbState->processResult(awb_result, *metadata);

    metadata->update(ANDROID_COLOR_CORRECTION_MODE,
                  &inputParams->aaaControls.awb.colorCorrectionMode,
                  1);
    metadata->update(ANDROID_COLOR_CORRECTION_ABERRATION_MODE,
                  &inputParams->aaaControls.awb.colorCorrectionAberrationMode,
                  1);

    /*
     * TODO: Consider moving this to common code in 3A class
     */
    float gains[4] = {1.0, 1.0, 1.0, 1.0};
    gains[0] = awb_result.awb_gain_cfg.awb_gains.red_gain;
    gains[1] = awb_result.awb_gain_cfg.awb_gains.green_r_gain;
    gains[2] = awb_result.awb_gain_cfg.awb_gains.green_b_gain;
    gains[3] = awb_result.awb_gain_cfg.awb_gains.blue_gain;
    metadata->update(ANDROID_COLOR_CORRECTION_GAINS, gains, 4);

    //# ANDROID_METADATA_Dynamic android.control.awbRegions done
    entry = inputParams->settings.find(ANDROID_CONTROL_AWB_REGIONS);
    if (entry.count == 5)
        metadata->update(ANDROID_CONTROL_AWB_REGIONS, inputParams->awbInputParams.awbRegion, entry.count);
    /*
     * store the results in row major order
     */
    if (mAwbState->getState() != ANDROID_CONTROL_AWB_STATE_LOCKED) {
        camera_metadata_rational_t transformMatrix[9];
        const int32_t COLOR_TRANSFORM_PRECISION = 10000;
        for (int i = 0; i < 9; i++) {
            transformMatrix[i].numerator =
                (int32_t)(awb_result.ctk_config.ctk_matrix.coeff[i] * COLOR_TRANSFORM_PRECISION);
            transformMatrix[i].denominator = COLOR_TRANSFORM_PRECISION;
        }
        metadata->update(ANDROID_COLOR_CORRECTION_TRANSFORM,
                         transformMatrix, 9);
    } else {
        entry = inputParams->settings.find(ANDROID_COLOR_CORRECTION_TRANSFORM);
        if (entry.count == 9) {
            metadata->update(ANDROID_COLOR_CORRECTION_TRANSFORM, entry.data.r, entry.count);
        }
    }
    return ret;
}

XCamReturn
AiqCameraHalAdapter::processMiscMetaResults(CameraMetadata *metadata){
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    SmartPtr<AiqInputParams> inputParams = _inputParams;
    camera_metadata_entry entry;
    LOGI("@%s %d: enter", __FUNCTION__, __LINE__);

    int reqId = inputParams.ptr() ? inputParams->reqId : -1;
    metadata->update(ANDROID_REQUEST_ID, &reqId, 1);
    // update flash states
    CameraMetadata* staticMeta  = inputParams->staticMeta;
    entry = staticMeta->find(ANDROID_FLASH_INFO_AVAILABLE);
    if (entry.count == 1) {
        if (entry.data.u8[0] == ANDROID_FLASH_INFO_AVAILABLE_TRUE) {
            const CameraMetadata* settings  =
                &inputParams->settings;

            /*flash mode*/
            uint8_t flash_mode = ANDROID_FLASH_MODE_OFF;
            camera_metadata_ro_entry entry_flash =
                settings->find(ANDROID_FLASH_MODE);

            if (entry_flash.count == 1) {
                flash_mode = entry_flash.data.u8[0];
            }
            metadata->update(ANDROID_FLASH_MODE, &flash_mode, 1);

            /* flash Staet*/
            uint8_t flashState = ANDROID_FLASH_STATE_READY;
            //TODO
            //rk_aiq_flash_setting_t fl_setting = rk_aiq_user_api_getFlSwAttr();
            rk_aiq_flash_setting_t fl_setting;
            if (fl_setting.frame_status == RK_AIQ_FRAME_STATUS_EXPOSED ||
                fl_setting.flash_mode == RK_AIQ_FLASH_MODE_TORCH ||
                /* CTS required */
                flash_mode == ANDROID_FLASH_MODE_SINGLE||
                flash_mode == ANDROID_FLASH_MODE_TORCH)
                flashState = ANDROID_FLASH_STATE_FIRED;
            else if (fl_setting.frame_status == RK_AIQ_FRAME_STATUS_PARTIAL)
                flashState = ANDROID_FLASH_STATE_PARTIAL;
            metadata->update(ANDROID_FLASH_STATE, &flashState, 1);

            entry = staticMeta->find(ANDROID_FLASH_INFO_AVAILABLE);
            //TODO
//            // sync needed and main flash on
//            if ((_stillcap_sync_state == STILLCAP_SYNC_STATE_START &&
//                fl_setting.frame_status == RK_AIQ_FRAME_STATUS_EXPOSED)) {
//                uint8_t stillcap_sync = RKCAMERA3_PRIVATEDATA_STILLCAP_SYNC_CMD_SYNCDONE;
//                metadata->update(RKCAMERA3_PRIVATEDATA_STILLCAP_SYNC_CMD, &stillcap_sync, 1);
//                //_stillcap_sync_state = STILLCAP_SYNC_STATE_WAITING_END;
//                LOGD("%s:%d, stillcap_sync done", __FUNCTION__, __LINE__);
//            }
        }
    }

    return ret;

}

void AiqCameraHalAdapter::rkAiqCalcDone(SmartPtr<RkAiqFullParamsProxy> &results){
    LOGD("@%s %d:", __FUNCTION__, __LINE__);
    if(this->_RkAiqAnalyzerCb.ptr()){
        this->processResults(results);
        this->_RkAiqAnalyzerCb->rkAiqCalcDone(results);
    }
}

void AiqCameraHalAdapter::rkAiqCalcFailed(const char* msg){
    LOGD("@%s %d:", __FUNCTION__, __LINE__);
    if(this->_RkAiqAnalyzerCb.ptr()){
        this->_RkAiqAnalyzerCb->rkAiqCalcFailed(msg);
    }
}

