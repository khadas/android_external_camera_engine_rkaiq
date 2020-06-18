/*
 *rk_aiq_types_agamma_algo_int.h
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

#ifndef _RK_AIQ_TYPE_AGAMMA_ALGO_INT_H_
#define _RK_AIQ_TYPE_AGAMMA_ALGO_INT_H_
#include "agamma/rk_aiq_types_agamma_algo.h"




RKAIQ_BEGIN_DECLARE

//add more here  api


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

typedef struct agamma_sw_info_s {
    float wokemode;

} agamma_sw_info_t;



typedef struct rk_aiq_lsc_mgamma_attrib_s {
    int gamma_en;
    int gamma_out_mode; //0:log segment ; 1:equal segment ;
    int gamma_out_segnum ;

    int gamma_out_offset;

    int gamma_table[45];


} rk_aiq_gamma_mgamma_attrib_t;

typedef enum rk_aiq_gamma_op_mode_s {
    RK_AIQ_GAMMA_MODE_INVALID                     = 0,        /**< initialization value */
    RK_AIQ_GAMMA_MODE_MANUAL                      = 1,        /**< run manual lens shading correction */
    RK_AIQ_GAMMA_MODE_AUTO                        = 2,        /**< run auto lens shading correction */

} rk_aiq_gamma_op_mode_t;


typedef struct rk_aiq_gamma_attrib_s {
    bool byPass;
    rk_aiq_gamma_op_mode_t mode;
    rk_aiq_gamma_mgamma_attrib_t stManual;

} rk_aiq_gamma_attrib_t;

RKAIQ_END_DECLARE

#endif

