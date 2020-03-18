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

#ifndef _RK_AIQ_CALIB_PARSER_H_
#define _RK_AIQ_CALIB_PARSER_H_
#include <string.h>
#include "tinyxml2.h"

#include "RkAiqCalibDbTypes.h"
#include "xmltags.h"
using namespace tinyxml2;

#if defined(__linux__)
#include "smartptr.h"
#include <xcam_common.h>
#include "xcam_log.h"

#ifdef DCT_ASSERT
#undef DCT_ASSERT
#endif
#define DCT_ASSERT  assert

#elif defined(_WIN32)

#ifdef DCT_ASSERT
#undef DCT_ASSERT
#endif
#define DCT_ASSERT(x) if(!(x))return false

#define LOGI printf
#define LOGD printf
#define LOGE printf
#define LOG1 printf

#endif

#define XML_PARSER_READ    0
#define XML_PARSER_WRITE   1


namespace RkCam {

class RkAiqCalibParser
{
private:
    typedef bool (RkAiqCalibParser::*parseCellContent)(const XMLElement*, void* param);

    // parse helper
    bool parseEntryCell(const XMLElement*, int, parseCellContent, void* param = NULL);

	// parse read/write control
	bool xmlParseReadWrite;
	char autoTabStr[128];
	int  autoTabIdx;
	void parseReadWriteCtrl(bool ctrl);
	void autoTabForward();
	void autoTabBackward();

	// parse Kit
	int ParseCharToHex(XmlTag* tag, uint32_t* reg_value);
	int ParseByteArray(const XMLNode *pNode, uint8_t* values, const int num);
	int ParseFloatArray(const XMLNode *pNode, float* values, const int num, int printAccuracy = 4);
	int ParseDoubleArray(const XMLNode *pNode, double* values, const int num);
	int ParseUintArray(const XMLNode *pNode, uint32_t* values, const int num);
	int ParseIntArray(const XMLNode *pNode, int32_t* values, const int num);
	int ParseUcharArray(const XMLNode *pNode, uint8_t* values, const int num);
	int ParseCharArray(const XMLNode *pNode, int8_t* values, const int num);
	int ParseUshortArray(const XMLNode *pNode, uint16_t* values, const int num);
	int ParseShortArray(const XMLNode *pNode, int16_t* values, const int num);
	int ParseString(const XMLNode *pNode, char* values, const int size);
	int ParseLscProfileArray(const XMLNode *pNode, CalibDb_Lsc_ProfileName_t values[], const int num);


    // parse Header
    bool parseEntryHeader(const XMLElement*, void* param = NULL);
    bool parseEntrySensor(const XMLElement*, void* param = NULL);
    bool parseEntrySensorAwb(const XMLElement*, void* param = NULL);
    bool parseEntrySensorAwbMeasureParaV200(const XMLElement*, void* param = NULL);
    bool parseEntrySensorAwbMeasureParaV201(const XMLElement*, void* param = NULL);
    bool parseEntrySensorAwbStategyPara(const XMLElement*, void* param = NULL);
    bool parseEntrySensorAwbMeasureGlobalsV200(const XMLElement*, void* param = NULL);
    bool parseEntrySensorAwbMeasureLightSourcesV200(const XMLElement*, void* param = NULL);
    bool parseEntrySensorAwbMeasureGlobalsV201(const XMLElement*, void* param = NULL);
    bool parseEntrySensorAwbMeasureLightSourcesV201(const XMLElement*, void* param = NULL);
    bool parseEntrySensorAwbStategyLightSources(const XMLElement*, void* param = NULL);
    bool parseEntrySensorAwbStategyGlobals(const XMLElement*, void* param = NULL);
    bool parseEntrySensorAwbLsForYuvDet(const XMLElement*, void* param = NULL);
    bool parseEntrySensorAwbLsForYuvDetV201(const XMLElement*, void* param = NULL);
    bool parseEntrySensorAwbWindowV201(const XMLElement*, void* param = NULL);
    bool parseEntrySensorAwbMeasureWindowV201(const XMLElement*, void* param = NULL);
    bool parseEntrySensorAwbFrameChoose(const XMLElement*, void* param = NULL);
    bool parseEntrySensorAwbMeasureWindow(const XMLElement*, void* param = NULL);
    bool parseEntrySensorAwbWindow(const XMLElement*, void* param = NULL);
    bool parseEntrySensorAwbSingleColor(const XMLElement*, void* param = NULL);
    bool parseEntrySensorAwbColBlk(const XMLElement*, void* param = NULL);
    bool parseEntrySensorAwbLsForEstimation(const XMLElement*, void* param = NULL);
    bool parseEntrySensorAwbGlobalsExclude(const XMLElement*, void* param = NULL);
    bool parseEntrySensorAwbGlobalsExcludeV201(const XMLElement*, void* param = NULL);
    bool parseEntrySensorAwbLightSources(const XMLElement*, void* param = NULL);
    bool parseEntrySensorAec(const XMLElement*, void* param = NULL);
    bool parseEntrySensorAecLinearAeRoute(const XMLElement*, void* param = NULL);
    bool parseEntrySensorAecHdrAeRoute(const XMLElement*, void* param = NULL);
    bool parseEntrySensorAecLinearAeDynamicPoint(const XMLElement*, void* param = NULL);
    bool parseEntrySensorAhdr(const XMLElement* pelement, void* param = NULL);
    bool parseEntrySensorAhdrMerge(const XMLElement* pelement, void* param = NULL);
    bool parseEntrySensorAhdrTmo(const XMLElement* pelement, void* param = NULL);
    bool parseEntrySensorBlc(const XMLElement* pelement, void* param = NULL);
    bool parseEntrySensorLut3d(const XMLElement* pelement, void* param = NULL);
    bool parseEntrySensorDpcc(const XMLElement* pelement, void* param = NULL);
	bool parseEntrySensorDpccSetCell(const XMLElement* pelement, void* param = NULL);
	bool parseEntrySensorDpccPdaf(const XMLElement* pelement, void* param = NULL);
    bool parseEntrySensorBayerNr(const XMLElement* pelement, void* param = NULL);
    bool parseEntrySensorLsc(const XMLElement* pelement, void* param = NULL);
    bool parseEntrySensorLscAlscCof(const XMLElement* pelement, void* param = NULL);
    bool parseEntrySensorLscAlscCofResAll(const XMLElement* pelement, void* param = NULL);
    bool parseEntrySensorLscAlscCofIllAll(const XMLElement* pelement, void* param = NULL);
    bool parseEntrySensorLscTableAll(const XMLElement* pelement, void* param = NULL);
    bool parseEntrySensorRKDM(const XMLElement* pelement, void* param = NULL);
    bool parseEntrySensorCCM(const XMLElement* pelement, void* param = NULL);
    bool parseEntrySensorCcmAccmCof(const XMLElement* pelement, void* param = NULL);
    bool parseEntrySensorCcmAccmCofIllAll(const XMLElement* pelement, void* param = NULL);
    bool parseEntrySensorCcmMatrixAll(const XMLElement* pelement, void* param = NULL);
    bool parseEntrySensorUVNR(const XMLElement* pelement, void* param = NULL);
    bool parseEntrySensorGamma(const XMLElement* pelement, void* param = NULL);
    bool parseEntrySensorYnr(const XMLElement* pelement, void* param = NULL);
	bool parseEntrySensorYnrISO(const XMLElement* pelement, void* param = NULL);
    bool parseEntrySensorGic(const XMLElement* pelement, void* param = NULL);
    bool parseEntrySensorGicISO(const XMLElement* pelement, void* param = NULL);
    bool parseEntrySensorMFNR(const XMLElement* pelement, void* param = NULL);
    bool parseEntrySensorMFNRAwbUvRatio(const XMLElement* pelement, void* param = NULL);
    bool parseEntrySensorMFNRISO(const XMLElement* pelement, void* param = NULL);
    bool parseEntrySensorSharp(const XMLElement* pelement, void* param = NULL);
    bool parseEntrySensorSharpISO(const XMLElement* pelement, void* param = NULL);
    bool parseEntrySensorEdgeFilter(const XMLElement* pelement, void* param = NULL);
    bool parseEntrySensorEdgeFilterISO(const XMLElement* pelement, void* param = NULL);
    bool parseEntrySensorDehaze(const XMLElement* pelement, void* param = NULL);
    bool parseEntrySensorDehazeISO(const XMLElement* pelement, void* param = NULL);
    bool parseEntrySensorAf(const XMLElement* pelement, void* param = NULL);
    bool parseEntrySensorInfo(const XMLElement* pelement, void* param = NULL);
    bool parseEntrySystem(const XMLElement*, void* param = NULL);


public:
    explicit RkAiqCalibParser(CamCalibDbContext_t *pCalibDb);
    virtual ~RkAiqCalibParser();
    bool doParse(const char* device);
	bool doGenerate(const char* deviceRef, const char* deviceOutput);


private:
    CamCalibDbContext_t *mCalibDb;

#if defined(__linux__)
    XCAM_DEAD_COPY (RkAiqCalibParser);
#endif
};

}; //namespace RkCam

#endif
