/*
 * rk_aiq_adehaze_algo.h
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

#ifndef __RK_AIQ_ADEHAZE_ALGO_H__
#define __K_AIQ_ADEHAZE_ALGO_H__

#include "rk_aiq_comm.h"
#include "RkAiqCalibDbTypes.h"
#include "adehaze/rk_aiq_types_adehaze_algo_prvt.h"


RKAIQ_BEGIN_DECLARE

   XCamReturn AdehazeInitV200(AdehazeHandle_t** para);
	XCamReturn AdehazeReleaseV200(AdehazeHandle_t* para);
	XCamReturn AdehazeConfigV200(const CalibDb_Dehaze_t * adhaz_html_para, AdehazeHandle_t* para,int iso);
	XCamReturn AdehazePreProcV200(AdehazeHandle_t* para);
	XCamReturn AdehazeProcessingV200(AdehazeHandle_t* para);
	XCamReturn  AdehazeReConfigV200(AdehazeHandle_t *para,const CalibDb_Dehaze_t *calib_dehaze,int iso);

RKAIQ_END_DECLARE

#endif //_RK_AIQ_ALGO_ACPRC_ITF_H_
