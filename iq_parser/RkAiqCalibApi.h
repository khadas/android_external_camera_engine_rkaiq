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

#ifndef _RK_AIQ_CALIB_API_H_
#define _RK_AIQ_CALIB_API_H_
#include <string.h>
#include "tinyxml2.h"

#include "RkAiqCalibDbTypes.h"

#if defined(_MSC_VER)
#define strcasecmp _stricmp
#define snprintf(buf,len, format,...) _snprintf_s(buf, len, len-1, format, __VA_ARGS__)
#endif
#ifndef container_of
#define container_of(ptr, type, member) \
  ((type *)(((unsigned long)(unsigned char *)(ptr)) - (unsigned long)(&((type *)0)->member)))

#endif
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


bool AddAwbCalibV200Profile2AwbCalibV200List(list_head *profile_list, CalibDb_Awb_Calib_Para_V200_t* pAddAwb);
bool GetAwbProfileFromAwbCalibV200ListBySceneName(const list_head* profile_list, const char* name, CalibDb_Awb_Calib_Para_V200_t** pAwbProfile, int *name_index = NULL);
bool GetAwbProfileFromAwbCalibV200ListByIdx(const list_head* profile_list, int idx,const CalibDb_Awb_Calib_Para_V200_t** pAwbProfile) ;
bool AddAwbCalibV201Profile2AwbCalibV201List(list_head *profile_list, CalibDb_Awb_Calib_Para_V201_t* pAddAwb);
bool GetAwbProfileFromAwbCalibV201ListBySceneName(const list_head* profile_list, const char* name, CalibDb_Awb_Calib_Para_V201_t** pAwbProfile, int *name_index = NULL);
bool GetAwbProfileFromAwbCalibV201ListByIdx(const list_head* profile_list, int idx,const CalibDb_Awb_Calib_Para_V201_t** pAwbProfile) ;
bool AddAwbAdjustProfile2AwbAdjustList(list_head *profile_list, CalibDb_Awb_Adjust_Para_t* pAddAwb);
bool GetAwbProfileFromAwbAdjustListBySceneName(const list_head* profile_list, const char* name, CalibDb_Awb_Adjust_Para_t** pAwbProfile);
bool GetAwbProfileFromAwbAdjustListByIdx(const list_head* profile_list, int idx,const CalibDb_Awb_Adjust_Para_t** pAwbProfile) ;

#endif
