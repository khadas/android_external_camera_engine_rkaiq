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

typedef enum rk_aiq_gamma_op_mode_s {
    RK_AIQ_GAMMA_MODE_INVALID                     = 0,        /**< initialization value */
    RK_AIQ_GAMMA_MODE_MANUAL                      = 1,        /**< run manual lens shading correction */
    RK_AIQ_GAMMA_MODE_AUTO                        = 2,        /**< run auto lens shading correction */
} rk_aiq_gamma_op_mode_t;

RKAIQ_END_DECLARE

#endif

