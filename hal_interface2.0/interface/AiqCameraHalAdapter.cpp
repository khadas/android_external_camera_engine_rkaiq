#include "AiqCameraHalAdapter.h"

#include <base/xcam_log.h>
#include <hwi/isp20/Isp20StatsBuffer.h>
#include <hwi/isp20/rkisp2-config.h>
#include <aiq_core/RkAiqHandleInt.h>

AiqCameraHalAdapter::AiqCameraHalAdapter(SmartPtr<RkAiqManager> rkAiqManager,SmartPtr<RkAiqCore> analyzer,SmartPtr<ICamHw> camHw)
:_rkAiqManager(rkAiqManager),_analyzer(analyzer),_camHw(camHw)
{
    LOGD("@%s %d:", __FUNCTION__, __LINE__);
    _settings.clear();
    _fly_settings.clear();
}

AiqCameraHalAdapter::~AiqCameraHalAdapter()
{
    LOGD("@%s %d:", __FUNCTION__, __LINE__);
    _settings.clear();
    _fly_settings.clear();
}

void AiqCameraHalAdapter::init(const cl_result_callbacks_ops* callbacks){
    LOGD("@%s %d:", __FUNCTION__, __LINE__);
    if(_rkAiqManager.ptr()&&_analyzer.ptr()&& callbacks){
        this->_analyzer->setAnalyzeResultCb(this);
        this->_RkAiqAnalyzerCb = _rkAiqManager;
        this->mCallbacks_ops = callbacks;
        this->_camHw->setIspStatsListener(this);
        this->_IspStatsListener = _rkAiqManager;
    }
}

SmartPtr<AiqInputParams> AiqCameraHalAdapter:: getAiqInputParams()
{
    LOGD("@%s %d:", __FUNCTION__, __LINE__);
    SmartLock lock(_settingsMutex);
    // use new setting when no flying settings to make sure
    // same settings used for 3A stats of one frame
    if (!_settings.empty() && _fly_settings.empty()) {
        _cur_settings = *_settings.begin();
        _settings.erase(_settings.begin());
        _fly_settings.push_back(_cur_settings);
    }
    return _cur_settings;
}

void AiqCameraHalAdapter::processAeResults(rk_aiq_ae_results &ae_results){
    LOGD("@%s %d:", __FUNCTION__, __LINE__);
    RkAiqAlgoProcResAeInt* ae_proc_res = (RkAiqAlgoProcResAeInt*)_analyzer->mAlogsSharedParams.procResComb.ae_proc_res;
    ae_results.converged = ae_proc_res->ae_proc_res_rk.IsConverged;
    LOGE("ae_results.converged:%d",ae_results.converged);
}

void AiqCameraHalAdapter::processAwbResults(rk_aiq_awb_results &awb_results){
    LOGD("@%s %d:", __FUNCTION__, __LINE__);
    RkAiqAlgoPreResAwbInt* awb_pre_res_int = (RkAiqAlgoPreResAwbInt*)_analyzer->mAlogsSharedParams.preResComb.awb_pre_res;
    awb_results.converged = awb_pre_res_int->awbConverged;
    LOGE("awb_results.converged:%d",awb_results.converged);
}

void AiqCameraHalAdapter::processAfResults(rk_aiq_af_results &af_results){
    LOGD("@%s %d:", __FUNCTION__, __LINE__);
    RkAiqAlgoPreResAfInt* af_pre_res_int = (RkAiqAlgoPreResAfInt*)_analyzer->mAlogsSharedParams.preResComb.af_pre_res;
}

void AiqCameraHalAdapter::processResults(SmartPtr<RkAiqFullParamsProxy> &results){
    LOGD("@%s %d:", __FUNCTION__, __LINE__);
    SmartPtr<rk_aiq_isp_params_t> isp_params = results->data()->mIspParams->data();

    if(this->mCallbacks_ops){
        LOGD("AeStatus:%d",mCallbacks_ops->getAeStatus(mCallbacks_ops));
        LOGD("AwbStatus:%d",mCallbacks_ops->getAwbStatus(mCallbacks_ops));
        LOGD("AfStatus:%d",mCallbacks_ops->getAfStatus(mCallbacks_ops));
    }

    int id = -1;
    {
        SmartLock lock(_settingsMutex);
        if(_fly_settings.size()){
            id = (*_fly_settings.begin())->reqId;
        }else{
            LOGD("_fly_settings.size():%d",_fly_settings.size());
            return ;
        }
        if (_fly_settings.size() && (id == (*_fly_settings.begin())->reqId)) {
            _fly_settings.erase(_fly_settings.begin());
        } else {
            // return every meta result, we use meta to do extra work, eg.
            // flash stillcap synchronization
            id = -1;
        }
    }

    if(mCallbacks_ops && id != -1){

        struct rkisp_cl_frame_rkaiq_result_s result;
        result.id = id;

        rk_aiq_ae_results ae_results;
        processAeResults(ae_results);

        rk_aiq_awb_results awb_results;
        processAwbResults(awb_results);

        rk_aiq_af_results af_results;
        processAfResults(af_results);

        result.ae_results = &ae_results;
        result.awb_results = &awb_results;
        result.af_results = &af_results;

        mCallbacks_ops->rkaiq_result_callback(mCallbacks_ops,&result);
    }
}

void AiqCameraHalAdapter::setFrameParams(const struct rkisp_cl_frame_rkaiq_s* frame_params){
    LOGD("@%s %d:", __FUNCTION__, __LINE__);
    SmartPtr<AiqInputParams> inputParams = new AiqInputParams();
    *inputParams.ptr() = *(AiqInputParams*)frame_params->rkaiq;

    if(inputParams.ptr()){
        inputParams->reqId = frame_params->id;
        LOGD("inputParams->afParams.focus_mode:%d",inputParams->afInputParams.afParams.focus_mode);
        LOGD("inputParams->aeCtrl->aeTargetFpsRange:[%d]-[%d]",inputParams->aaaControls.ae.aeTargetFpsRange[0],inputParams->aaaControls.ae.aeTargetFpsRange[1]);
        LOGD("inputParams->awb.colorCorrectionMode.:%d",inputParams->aaaControls.awb.colorCorrectionMode);
    }
    SmartLock lock(_settingsMutex);
    static bool stillcap_sync_cmd_end_delay = false;
        // we use id -1 request to do special work, eg. flash stillcap sync
        if (frame_params->id != -1) {
            if (stillcap_sync_cmd_end_delay) {
                stillcap_sync_cmd_end_delay = false;
                //inputParams->stillCapSyncCmd = RKCAMERA3_PRIVATEDATA_STILLCAP_SYNC_CMD_SYNCEND;
            }

            _settings.push_back(inputParams);
        } else {
            // merged to next params
            // if (inputParams->stillCapSyncCmd == RKCAMERA3_PRIVATEDATA_STILLCAP_SYNC_CMD_SYNCEND) {
            //     if (!_settings.empty()) {
            //         XCam::SmartPtr<AiqInputParams> settings = *_settings.begin();
            //         settings->stillCapSyncCmd = RKCAMERA3_PRIVATEDATA_STILLCAP_SYNC_CMD_SYNCEND;
            //     } else {
            //         stillcap_sync_cmd_end_delay = true;
            //     }
            // }
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

void AiqCameraHalAdapter::updateAeParams(XCamAeParam *aeParams){
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
    }else if(aeParams->flicker_mode == XCAM_AE_FLICKER_MODE_50HZ){
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

    if(this->mCallbacks_ops){
        int status =  mCallbacks_ops->getAeStatus(mCallbacks_ops);
        ALOGD("AeStatus:%d",status);
        //ANDROID_CONTROL_AE_STATE_LOCKED = 3
        if(status == 3){
            handle->setEnable(false);
        }else{
            handle->setEnable(true);
        }
    }

}

void AiqCameraHalAdapter::updateAwbParams(XCamAwbParam *awbParams){
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

    if(this->mCallbacks_ops){
        int status =  mCallbacks_ops->getAwbStatus(mCallbacks_ops);
        ALOGD("AwbStatus:%d",status);
        //CONTROL_AWB_STATE_LOCKED = 3
        if(status == 3){
            awbhandle->setEnable(false);
        }else{
            awbhandle->setEnable(true);
        }
    }
}

void AiqCameraHalAdapter::updateAfParams(XCamAfParam *afParams){
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

    afhandle->setAttrib(att);
}

void AiqCameraHalAdapter::updateParams(SmartPtr<VideoBuffer>& ispStats){
    LOGD("@%s %d:", __FUNCTION__, __LINE__);

    SmartPtr<AiqInputParams> inputParams =  getAiqInputParams();
    int id = -1;
    if(inputParams.ptr()){
        id = inputParams->reqId ;
    }else{
        ALOGE("inputParams NULL");
        return ;
    }
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

XCamReturn AiqCameraHalAdapter::ispStatsCb(SmartPtr<VideoBuffer>& ispStats){
    LOGD("@%s %d:", __FUNCTION__, __LINE__);
    updateParams(ispStats);
    if(this->_IspStatsListener.ptr()){
        return this->_IspStatsListener->ispStatsCb(ispStats);
    }
    return XCAM_RETURN_NO_ERROR;
}
