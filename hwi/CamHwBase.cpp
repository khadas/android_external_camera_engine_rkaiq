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

#include "CamHwBase.h"
#include "SensorHw.h"

namespace RkCam {

std::map<std::string, SmartPtr<rk_aiq_static_info_t>> CamHwBase::mCamHwInfos;

CamHwBase::CamHwBase()
{}

CamHwBase::~CamHwBase()
{}

//static
XCamReturn
CamHwBase::getCamHwInfos()
{
    // TODO
    // (1) all sensor info
    // (2) all pipeline entity infos belonged to
    //     the sensor
    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
CamHwBase::init(const char* sns_ent_name)
{
    if (mCamHwInfos.empty())
        getCamHwInfos();

    std::string sensor_name(sns_ent_name);
    if (mCamHwInfos.find(sensor_name) != mCamHwInfos.end()) {
        LOGE("can't find sensor %s", sns_ent_name);
        return XCAM_RETURN_ERROR_SENSOR;
    }
    // TODO: new all subdevices and open
    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
CamHwBase::deInit()
{
    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
CamHwBase::prepare(uint32_t width, uint32_t height, int mode)
{
    // TODO
    // check sensor's output width,height
    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
CamHwBase::start()
{
    // TODO
    // subscribe events
    // start devices
    // start pollthread
    mPollthread->start();
    mPollLumathread->start();

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
CamHwBase::stop()
{
    // TODO
    // unsubscribe events
    // stop pollthread
    // stop devices
    mPollthread->stop();
    mPollLumathread->stop();

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
CamHwBase::getSensorModeData(const char* sns_ent_name,
                             rk_aiq_exposure_sensor_descriptor& sns_des)
{
    // TODO
    // get from SensorHw
    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
CamHwBase::setIspParams(SmartPtr<RkAiqIspParamsProxy>& ispParams)
{
    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
CamHwBase::setExposureParams(SmartPtr<RkAiqExpParamsProxy>& expPar)
{
    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
CamHwBase::setFocusParams(SmartPtr<RkAiqFocusParamsProxy>& focus_params)
{
    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
CamHwBase::setIspLumaListener(IspLumaListener* lumaListener)
{
    mIspLumaListener = lumaListener;

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
CamHwBase::setIspStatsListener(IspStatsListener* statsListener)
{
    mIspStatsLintener = statsListener;

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
CamHwBase::setEvtsListener(IspEvtsListener* evtListener)
{
    mIspEvtsListener = evtListener;

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
CamHwBase::poll_buffer_ready (SmartPtr<VideoBuffer> &buf, int type)
{
    if (type == ISP_POLL_3A_STATS && mIspStatsLintener) {
        return mIspStatsLintener->ispStatsCb(buf);
    }

    if (type == ISP_POLL_LUMA && mIspLumaListener) {
        return mIspLumaListener->ispLumaCb(buf);
    }

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
CamHwBase::poll_buffer_failed (int64_t timestamp, const char *msg)
{
    // TODO
    return XCAM_RETURN_ERROR_FAILED;
}

}; //namspace RkCam
