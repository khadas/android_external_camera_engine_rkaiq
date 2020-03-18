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


#include "RkAiqCalibDb.h"

#define FUNC_ENTER LOG1("%s enter", __FUNCTION__);
#define FUNC_EXIT LOG1("%s exit", __FUNCTION__);


namespace RkCam {

map<string, CamCalibDbContext_t*> RkAiqCalibDb::mCalibDbsMap;

CamCalibDbContext_t* RkAiqCalibDb::createCalibDb(char* iqFile)
{
    map<string, CamCalibDbContext_t*>::iterator it;

    string str(iqFile);

    it = mCalibDbsMap.find(str);
    if (it != mCalibDbsMap.end()) {
        LOGD("use cached calibdb for %s!", iqFile);
        return it->second;
    } else {
        CamCalibDbContext_t *pCalibDb = new CamCalibDbContext_t;
        if (pCalibDb) {
            RkAiqCalibParser  parser(pCalibDb);
            if (parser.doParse(iqFile)) {
                mCalibDbsMap[str] = pCalibDb;
                LOGD("create calibdb from %s success.", iqFile);
                return pCalibDb;

            } else {
                LOGE("parse %s failed.", iqFile);
            }
        } else {
            LOGE("alloc calibdb memory failed.");
        }
    }
    return NULL;
}

bool RkAiqCalibDb::generateCalibDb(char* iqFileRef, char* iqFileOutput, CamCalibDbContext_t* pCalibDb)
{
	//map<string, CamCalibDbContext_t*>::iterator it;

	string str(iqFileRef);

	//it = mCalibDbsMap.find(str);
	if (pCalibDb) {
		RkAiqCalibParser  parser(pCalibDb);
		if (parser.doGenerate(iqFileRef, iqFileOutput)) {
			mCalibDbsMap[str] = pCalibDb;
			LOGD("generate calibdb from %s to %s success.", iqFileRef, iqFileOutput);
			return true;
		}
		else {
			LOGE("generate %s to %s failed.", iqFileRef, iqFileOutput);
		}
	}
	else {
		LOGE("alloc generate memory failed.");
	}
	return false;
}

void RkAiqCalibDb::releaseCalibDb()
{
    std::map<string, CamCalibDbContext_t*>::iterator it;
    for (it = mCalibDbsMap.begin(); it != mCalibDbsMap.end(); it++) {
        CamCalibDbContext_t *pCalibDb = it->second;
        if(pCalibDb) {
            if(pCalibDb->af.contrast_af.FullRangeTbl != NULL) {
                free(pCalibDb->af.contrast_af.FullRangeTbl);
            }
            if(pCalibDb->af.contrast_af.AdaptRangeTbl!= NULL) {
                free(pCalibDb->af.contrast_af.AdaptRangeTbl);
            }
            if(pCalibDb->sensor.GainRange.pGainRange != NULL)
                free(pCalibDb->sensor.GainRange.pGainRange);

            LOGE("releaseCalibDb!");
            delete pCalibDb;
        }
    }
    mCalibDbsMap.clear();
}

CamCalibDbContext_t* RkAiqCalibDb::getCalibDb(char* iqFile)
{
    std::map<string, CamCalibDbContext_t*>::iterator it;

    std::string str(iqFile);

    it = mCalibDbsMap.find(str);
    if (it != mCalibDbsMap.end()) {
        return it->second;
    } else {
        LOGE("calibDb not found!");
        return NULL;
    }
}

void RkAiqCalibDb::InitCalibDatabase(CamCalibDbContext_t *pCalibDb, RkAiqCalibParser *pCalibDbParser)
{

}

}; //namespace RkCam


