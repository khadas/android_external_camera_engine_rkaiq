/*
 * RkAiqHandleIntV1.h
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

#ifndef _RK_AIQ_HANDLE_INT_V1_H_
#define _RK_AIQ_HANDLE_INT_V1_H_

#include "RkAiqHandleInt.h"

namespace RkCam {

class RkAiqAdrcV1HandleInt:
    virtual public RkAiqAdrcHandle,
    virtual public RkAiqHandleIntCom {
public:
    explicit RkAiqAdrcV1HandleInt(RkAiqAlgoDesComm* des, RkAiqCore* aiqCore)
        : RkAiqHandle(des, aiqCore)
        , RkAiqAdrcHandle(des, aiqCore)
        , RkAiqHandleIntCom(des, aiqCore) {};
    virtual ~RkAiqAdrcV1HandleInt() {
        RkAiqAdrcHandle::deInit();
    };
    virtual XCamReturn updateConfig(bool needSync);
    virtual XCamReturn prepare();
    virtual XCamReturn preProcess();
    virtual XCamReturn processing();
    virtual XCamReturn postProcess();
    //XCamReturn setAttrib(adrcV1_attrib_t att);
    //XCamReturn getAttrib(adrcV1_attrib_t *att);
protected:
    virtual void init();
    virtual void deInit() {
        RkAiqAdrcHandle::deInit();
    };
private:
    //adrcV1_attrib_t mCurAtt;
    //adrcV1_attrib_t mNewAtt;
};


}; //namespace RkCam

#endif
