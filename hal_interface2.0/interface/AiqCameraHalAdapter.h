#ifndef _AIQ_CAMERA_HAL_ADAPTER_H_
#define _AIQ_CAMERA_HAL_ADAPTER_H_

#include "RkAiqCore.h"
#include <RkAiqManager.h>

#include "rkisp_control_loop.h"
#include "rkaiq.h"

using namespace RkCam;
using namespace XCam;

class AiqCameraHalAdapter:
    public RkAiqAnalyzerCb,
    public IspStatsListener
{
private:
    SmartPtr<RkAiqManager> _rkAiqManager;
    SmartPtr<RkAiqCore> _analyzer;
    SmartPtr<ICamHw> _camHw;
    SmartPtr<RkAiqAnalyzerCb> _RkAiqAnalyzerCb;
    SmartPtr<IspStatsListener> _IspStatsListener;

    const cl_result_callbacks_ops* mCallbacks_ops;

    Mutex _settingsMutex;
    // push_back when set_control_params, erase when calculationd done
    std::vector<SmartPtr<AiqInputParams>>  _settings;
    std::vector<SmartPtr<AiqInputParams>>  _fly_settings;
    SmartPtr<AiqInputParams>  _cur_settings;
public:
    AiqCameraHalAdapter(SmartPtr<RkAiqManager> _rkAiqManager,SmartPtr<RkAiqCore> _analyzer,SmartPtr<ICamHw> _camHw);
    ~AiqCameraHalAdapter();
    void init(const cl_result_callbacks_ops* callbacks);
    void processResults(SmartPtr<RkAiqFullParamsProxy> &results);
    void processAeResults(rk_aiq_ae_results &ae_results);
    void processAwbResults(rk_aiq_awb_results &awb_results);
    void processAfResults(rk_aiq_af_results &af_results);
    void updateParams(SmartPtr<VideoBuffer>& ispStats);
    void updateAeParams(XCamAeParam *aeParams);
    void updateAwbParams(XCamAwbParam *awbParams);
    void updateAfParams(XCamAfParam *afParams);
    void setFrameParams(const struct rkisp_cl_frame_rkaiq_s* frame_params);

    virtual void rkAiqCalcDone(SmartPtr<RkAiqFullParamsProxy> &results);
    virtual void rkAiqCalcFailed(const char* msg);

    virtual XCamReturn ispStatsCb(SmartPtr<VideoBuffer>& ispStats);

    SmartPtr<AiqInputParams> getAiqInputParams();
};

#endif //_AIQ_CAMERA_HAL_ADAPTER_H_
