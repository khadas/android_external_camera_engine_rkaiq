/*
 * RkAiqHandleInt.h
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

#ifndef _RK_AIQ_HANDLE_INT_H_
#define _RK_AIQ_HANDLE_INT_H_

#include "RkAiqHandle.h"
#include "rk_aiq_algo_types_int.h"
#include "ae/rk_aiq_uapi_ae_int.h"
#include "awb/rk_aiq_uapi_awb_int.h"
#include "adebayer/rk_aiq_uapi_adebayer_int.h"
#include "ahdr/rk_aiq_uapi_ahdr_int.h"
#include "xcam_mutex.h"

namespace RkCam {

class RkAiqCore;

class RkAiqHandleIntCom : virtual public RkAiqHandle
{
public:
    explicit RkAiqHandleIntCom(RkAiqAlgoDesComm* des, RkAiqCore* aiqCore)
        : RkAiqHandle(des, aiqCore)
        , updateAtt(false) {};
    virtual ~RkAiqHandleIntCom() {};
protected:
    XCamReturn configInparamsCom(RkAiqAlgoCom* com, int type);
    XCam::Mutex mCfgMutex;
    bool updateAtt;
};

#define RKAIQHANDLEINT(algo) \
    class RkAiq##algo##HandleInt: \
            virtual public RkAiq##algo##Handle, \
            virtual public RkAiqHandleIntCom { \
    public: \
        explicit RkAiq##algo##HandleInt(RkAiqAlgoDesComm* des, RkAiqCore* aiqCore) \
                    : RkAiqHandle(des, aiqCore) \
                    , RkAiq##algo##Handle(des, aiqCore) \
                    , RkAiqHandleIntCom(des, aiqCore) {}; \
        virtual ~RkAiq##algo##HandleInt() { RkAiq##algo##Handle::deInit(); }; \
        virtual XCamReturn prepare(); \
        virtual XCamReturn preProcess(); \
        virtual XCamReturn processing(); \
        virtual XCamReturn postProcess(); \
    protected: \
        virtual void init(); \
        virtual void deInit() { RkAiq##algo##Handle::deInit(); }; \
    }

// define
//RKAIQHANDLEINT(Ae);
//RKAIQHANDLEINT(Awb);
RKAIQHANDLEINT(Af);
//RKAIQHANDLEINT(Ahdr);
RKAIQHANDLEINT(Anr);
RKAIQHANDLEINT(Alsc);
RKAIQHANDLEINT(Asharp);
RKAIQHANDLEINT(Adhaz);
RKAIQHANDLEINT(Asd);
RKAIQHANDLEINT(Acp);
RKAIQHANDLEINT(A3dlut);
RKAIQHANDLEINT(Ablc);
RKAIQHANDLEINT(Accm);
RKAIQHANDLEINT(Acgc);
//RKAIQHANDLEINT(Adebayer);
RKAIQHANDLEINT(Adpcc);
RKAIQHANDLEINT(Afec);
RKAIQHANDLEINT(Agamma);
RKAIQHANDLEINT(Agic);
RKAIQHANDLEINT(Aie);
RKAIQHANDLEINT(Aldch);
RKAIQHANDLEINT(Ar2y);
RKAIQHANDLEINT(Awdr);
RKAIQHANDLEINT(Aorb);
// ae
class RkAiqAeHandleInt:
    virtual public RkAiqAeHandle,
    virtual public RkAiqHandleIntCom {
public:
    explicit RkAiqAeHandleInt(RkAiqAlgoDesComm* des, RkAiqCore* aiqCore)
        : RkAiqHandle(des, aiqCore)
        , RkAiqAeHandle(des, aiqCore)
        , RkAiqHandleIntCom(des, aiqCore) {};
    virtual ~RkAiqAeHandleInt() {
        RkAiqAeHandle::deInit();
    };
    virtual XCamReturn updateConfig();
    virtual XCamReturn prepare();
    virtual XCamReturn preProcess();
    virtual XCamReturn processing();
    virtual XCamReturn postProcess();
    // TODO add algo specific methods, this is a sample
    XCamReturn setExpSwAttr(Uapi_ExpSwAttr_t pExpSwAttr);
    XCamReturn getExpSwAttr(Uapi_ExpSwAttr_t* pExpSwAttr);
    XCamReturn setLinExpAttr(Uapi_LinExpAttr_t pLinExpAttr);
    XCamReturn getLinExpAttr(Uapi_LinExpAttr_t* pLinExpAttr);
    XCamReturn setHdrExpAttr(Uapi_HdrExpAttr_t pHdrExpAttr);
    XCamReturn getHdrExpAttr (Uapi_HdrExpAttr_t* pHdrExpAttr);
    XCamReturn setLinAeRouteAttr(Uapi_LinAeRouteAttr_t pLinAeRouteAttr);
    XCamReturn getLinAeRouteAttr(Uapi_LinAeRouteAttr_t* pLinAeRouteAttr);
    XCamReturn setHdrAeRouteAttr(Uapi_HdrAeRouteAttr_t pHdrAeRouteAttr);
    XCamReturn getHdrAeRouteAttr(Uapi_HdrAeRouteAttr_t* pHdrAeRouteAttr);
    XCamReturn queryExpInfo(Uapi_ExpQueryInfo_t* pExpQueryInfo);

protected:
    virtual void init();
    virtual void deInit() {
        RkAiqAeHandle::deInit();
    };
private:
    // TODO
    XCam::Mutex mCfgMutex;
    Uapi_ExpSwAttr_t  mCurExpSwAttr;
    Uapi_ExpSwAttr_t  mNewExpSwAttr;
    Uapi_LinExpAttr_t mCurLinExpAttr;
    Uapi_LinExpAttr_t mNewLinExpAttr;
    Uapi_HdrExpAttr_t mCurHdrExpAttr;
    Uapi_HdrExpAttr_t mNewHdrExpAttr;
    Uapi_LinAeRouteAttr_t mCurLinAeRouteAttr;
    Uapi_LinAeRouteAttr_t mNewLinAeRouteAttr;
    Uapi_HdrAeRouteAttr_t mCurHdrAeRouteAttr;
    Uapi_HdrAeRouteAttr_t mNewHdrAeRouteAttr;
    bool updateExpSwAttr = false;
    bool updateExpHwAttr = false;
    bool updateLinExpAttr = false;
    bool updateHdrExpAttr = false;
    bool updateLinAeRouteAttr = false;
    bool updateHdrAeRouteAttr = false;
};

// awb
class RkAiqAwbHandleInt:
    virtual public RkAiqAwbHandle,
    virtual public RkAiqHandleIntCom {
public:
    explicit RkAiqAwbHandleInt(RkAiqAlgoDesComm* des, RkAiqCore* aiqCore)
        : RkAiqHandle(des, aiqCore)
        , RkAiqAwbHandle(des, aiqCore)
        , RkAiqHandleIntCom(des, aiqCore) {};
    virtual ~RkAiqAwbHandleInt() {
        RkAiqAwbHandle::deInit();
    };
    virtual XCamReturn updateConfig();
    virtual XCamReturn prepare();
    virtual XCamReturn preProcess();
    virtual XCamReturn processing();
    virtual XCamReturn postProcess();
    // TODO add algo specific methords, this is a sample
    XCamReturn setAttrib(rk_aiq_wb_attrib_t att);
    XCamReturn getAttrib(rk_aiq_wb_attrib_t *att);
    XCamReturn getCct(rk_aiq_wb_cct_t *cct);
    XCamReturn queryWBInfo(rk_aiq_wb_querry_info_t *wb_querry_info );

protected:
    virtual void init();
    virtual void deInit() {
        RkAiqAwbHandle::deInit();
    };
private:
    // TODO
    rk_aiq_wb_attrib_t mCurAtt;
    rk_aiq_wb_attrib_t mNewAtt;
};

class RkAiqAdebayerHandleInt:
    virtual public RkAiqAdebayerHandle,
    virtual public RkAiqHandleIntCom {
public:
    explicit RkAiqAdebayerHandleInt(RkAiqAlgoDesComm* des, RkAiqCore* aiqCore)
        : RkAiqHandle(des, aiqCore)
        , RkAiqAdebayerHandle(des, aiqCore)
        , RkAiqHandleIntCom(des, aiqCore) {};
    virtual ~RkAiqAdebayerHandleInt() {
        RkAiqAdebayerHandle::deInit();
    };
    virtual XCamReturn updateConfig();
    virtual XCamReturn prepare();
    virtual XCamReturn preProcess();
    virtual XCamReturn processing();
    virtual XCamReturn postProcess();
    // TODO add algo specific methords, this is a sample
    XCamReturn setAttrib(adebayer_attrib_t att);
protected:
    virtual void init();
    virtual void deInit() {
        RkAiqAdebayerHandle::deInit();
    };
private:
    // TODO
    XCam::Mutex mCfgMutex;
    adebayer_attrib_t mCurAtt;
    adebayer_attrib_t mNewAtt;
    bool updateAtt;
};


// ahdr
class RkAiqAhdrHandleInt:
    virtual public RkAiqAhdrHandle,
    virtual public RkAiqHandleIntCom {
public:
    explicit RkAiqAhdrHandleInt(RkAiqAlgoDesComm* des, RkAiqCore* aiqCore)
        : RkAiqHandle(des, aiqCore)
        , RkAiqAhdrHandle(des, aiqCore)
        , RkAiqHandleIntCom(des, aiqCore)
        , updateAtt(false) {};
    virtual ~RkAiqAhdrHandleInt() {
        RkAiqAhdrHandle::deInit();
    };
    virtual XCamReturn updateConfig();
    virtual XCamReturn prepare();
    virtual XCamReturn preProcess();
    virtual XCamReturn processing();
    virtual XCamReturn postProcess();
    // TODO add algo specific methords, this is a sample
    XCamReturn setAttrib(ahdr_attrib_t att);
protected:
    virtual void init();
    virtual void deInit() {
        RkAiqAhdrHandle::deInit();
    };
private:
    // TODO
    XCam::Mutex mCfgMutex;
    ahdr_attrib_t mCurAtt;
    ahdr_attrib_t mNewAtt;
    bool updateAtt;
};

}; //namespace RkCam

#endif
