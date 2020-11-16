/*
 * RkAiqHandleIntV21.cpp
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

#include "RkAiqHandleIntV21.h"
#include "RkAiqCore.h"

namespace RkCam {

XCamReturn 
RkAiqAdrcV1HandleInt::updateConfig(bool needSync)
{
    ENTER_ANALYZER_FUNCTION();

    EXIT_ANALYZER_FUNCTION();

    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
RkAiqAdrcV1HandleInt::prepare()
{
    ENTER_ANALYZER_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    ret = RkAiqAdrcHandle::prepare();
    RKAIQCORE_CHECK_RET(ret, "adrcV1 handle prepare failed");

    RkAiqAlgoConfigAdrcV1Int* adrcV1_config_int = (RkAiqAlgoConfigAdrcV1Int*)mConfig;
    RkAiqCore::RkAiqAlgosShared_t* shared = &mAiqCore->mAlogsSharedParams;

    RkAiqAlgoDescription* des = (RkAiqAlgoDescription*)mDes;
    ret = des->prepare(mConfig);
    RKAIQCORE_CHECK_RET(ret, "adrcV1 algo prepare failed");

    EXIT_ANALYZER_FUNCTION();
    return XCAM_RETURN_NO_ERROR;
}

void
RkAiqAdrcV1HandleInt::init()
{
    ENTER_ANALYZER_FUNCTION();

    RkAiqAdrcHandle::deInit();
    mConfig       = (RkAiqAlgoCom*)(new RkAiqAlgoConfigAdrcV1Int());
    mPreInParam   = (RkAiqAlgoCom*)(new RkAiqAlgoPreAdrcV1Int());
    mPreOutParam  = (RkAiqAlgoResCom*)(new RkAiqAlgoPreResAdrcV1Int());
    mProcInParam  = (RkAiqAlgoCom*)(new RkAiqAlgoProcAdrcV1Int());
    mProcOutParam = (RkAiqAlgoResCom*)(new RkAiqAlgoProcResAdrcV1Int());
    mPostInParam  = (RkAiqAlgoCom*)(new RkAiqAlgoPostAdrcV1Int());
    mPostOutParam = (RkAiqAlgoResCom*)(new RkAiqAlgoPostResAdrcV1Int());

    EXIT_ANALYZER_FUNCTION();
}

XCamReturn
RkAiqAdrcV1HandleInt::preProcess()
{
    ENTER_ANALYZER_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    RkAiqAlgoPreAdrcV1Int* adrcV1_pre_int = (RkAiqAlgoPreAdrcV1Int*)mPreInParam;
    RkAiqAlgoPreResAdrcV1Int* adrcV1_pre_res_int = (RkAiqAlgoPreResAdrcV1Int*)mPreOutParam;
    RkAiqCore::RkAiqAlgosShared_t* shared = &mAiqCore->mAlogsSharedParams;
    RkAiqIspStats* ispStats = &shared->ispStats;
    RkAiqPreResComb* comb = &shared->preResComb;

    ret = RkAiqAdrcHandle::preProcess();
    if (ret) {
        comb->adrc_pre_res = NULL;
        RKAIQCORE_CHECK_RET(ret, "adrcV1 handle preProcess failed");
    }

    comb->adrc_pre_res = NULL;

    RkAiqAlgoDescription* des = (RkAiqAlgoDescription*)mDes;
    ret = des->pre_process(mPreInParam, mPreOutParam);
    RKAIQCORE_CHECK_RET(ret, "adrcV1 algo pre_process failed");

    // set result to mAiqCore
    comb->adrc_pre_res = (RkAiqAlgoPreResAdrc*)adrcV1_pre_res_int;

    EXIT_ANALYZER_FUNCTION();
    return XCAM_RETURN_NO_ERROR;
}

XCamReturn
RkAiqAdrcV1HandleInt::processing()
{
    ENTER_ANALYZER_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    RkAiqAlgoProcAdrcV1Int* adrcV1_proc_int = (RkAiqAlgoProcAdrcV1Int*)mProcInParam;
    RkAiqAlgoProcResAdrcV1Int* adrcV1_proc_res_int = (RkAiqAlgoProcResAdrcV1Int*)mProcOutParam;
    RkAiqCore::RkAiqAlgosShared_t* shared = &mAiqCore->mAlogsSharedParams;
    RkAiqProcResComb* comb = &shared->procResComb;
    RkAiqIspStats* ispStats = &shared->ispStats;

    ret = RkAiqAdrcHandle::processing();
    if (ret) {
        comb->adrc_proc_res = NULL;
        RKAIQCORE_CHECK_RET(ret, "adrcV1 handle processing failed");
    }

    comb->adrc_proc_res = NULL;

    RkAiqAlgoDescription* des = (RkAiqAlgoDescription*)mDes;
    ret = des->processing(mProcInParam, mProcOutParam);
    RKAIQCORE_CHECK_RET(ret, "adrcV1 algo processing failed");

    comb->adrc_proc_res = (RkAiqAlgoProcResAdrc*)adrcV1_proc_res_int;

    EXIT_ANALYZER_FUNCTION();
    return ret;
}

XCamReturn
RkAiqAdrcV1HandleInt::postProcess()
{
    ENTER_ANALYZER_FUNCTION();

    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    RkAiqAlgoPostAdrcV1Int* adrcV1_post_int = (RkAiqAlgoPostAdrcV1Int*)mPostInParam;
    RkAiqAlgoPostResAdrcV1Int* adrcV1_post_res_int = (RkAiqAlgoPostResAdrcV1Int*)mPostOutParam;
    RkAiqCore::RkAiqAlgosShared_t* shared = &mAiqCore->mAlogsSharedParams;
    RkAiqPostResComb* comb = &shared->postResComb;
    RkAiqIspStats* ispStats = &shared->ispStats;

    ret = RkAiqAdrcHandle::postProcess();
    if (ret) {
        comb->adrc_post_res = NULL;
        RKAIQCORE_CHECK_RET(ret, "adrcV1 handle postProcess failed");
        return ret;
    }

    comb->adrc_post_res = NULL;
    RkAiqAlgoDescription* des = (RkAiqAlgoDescription*)mDes;
    ret = des->post_process(mPostInParam, mPostOutParam);
    RKAIQCORE_CHECK_RET(ret, "adrcV1 algo post_process failed");
    // set result to mAiqCore
    comb->adrc_post_res = (RkAiqAlgoPostResAdrc*)adrcV1_post_res_int ;

    EXIT_ANALYZER_FUNCTION();
    return ret;
}

}; //namespace RkCam
