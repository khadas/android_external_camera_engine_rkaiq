/*
 * rk_aiq_algo_acprc_itf.h
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

#ifndef __AGAMMA_H__
#define __AGAMMA_H__

#include "rk_aiq_comm.h"
#include "../../iq_parser/RkAiqCalibDbTypes.h"

RKAIQ_BEGIN_DECLARE



typedef struct RKAiqAgammaHtmlConfig_s
{
   
    int gamma_en;
	int gamma_out_mode; //0:log segment ; 1:equal segment ;
    int gamma_out_segnum ;

	int gamma_out_offset;

	int gamma_nomal[45];
	int gamma_hdr[45];
	int gamma_night[45];
	int gamma_user1[45];
	int gamma_user2[45];




} RKAiqAgammaHtmlConfig_t;



typedef struct RKAiqAgammaHtmlConfig_Select_s {
 
	int gamma_en;
	int gamma_out_mode; //0:log segment ; 1:equal segment ;
    int gamma_out_segnum ;

	int gamma_out_offset;

	int gamma_table[45];
}  RKAiqAgammaHtmlConfig_Select_t;

typedef struct RKAiqAgammaHwConfig_s {
    int gamma_en;
	int gamma_out_mode; //0:log segment ; 1:equal segment ;
    int gamma_out_segnum ;

	int gamma_out_offset;

	int gamma_table[45];
}  RKAiqAgammaHwConfig_t;

enum {
    GAMMA_OUT_LOG_SEGMENT = 0,
    GAMMA_OUT_EQ_SEGMENT = 1
};

enum {
    GAMMA_OUT_NORMAL = 0,
    GAMMA_OUT_HDR = 1,
    GAMMA_OUT_NIGHT = 2,
	GAMMA_OUT_USR1 = 3,
    GAMMA_OUT_USR2 = 4
};


typedef struct AgammaHandle_s {
    RKAiqAgammaHwConfig_t    agamma_config;
    RKAiqAgammaHtmlConfig_t  AgammaHtmlPara;
    CalibDb_Gamma_t *calib_gamma;
} AgammaHandle_t;

XCamReturn AgammaInitV200(AgammaHandle_t** para);
XCamReturn AgammaReleaseV200(AgammaHandle_t* para);
XCamReturn AgammaConfigV200(const CalibDb_Gamma_t *  AgammaHtmlPara, AgammaHandle_t* para);
XCamReturn AgammaPreProcV200(AgammaHandle_t* para);
XCamReturn AgammaProcessingV200(AgammaHandle_t* para);
XCamReturn  AgammaReConfigV200(AgammaHandle_t *para,const CalibDb_Gamma_t * gamma_calib);


RKAIQ_END_DECLARE

#endif 
