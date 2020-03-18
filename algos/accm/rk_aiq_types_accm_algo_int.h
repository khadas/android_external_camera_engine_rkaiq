/*
 * rk_aiq_types_accm_int.h
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

#ifndef _RK_AIQ_TYPES_ACCM_ALGO_INT_H_
#define _RK_AIQ_TYPES_ACCM_ALGO_INT_H_

#include "accm/rk_aiq_types_accm_algo.h"

RKAIQ_BEGIN_DECLARE

//add more here
typedef struct accm_sw_info_s {
    float sensorGain;
    float awbGain[2];
    float awbIIRDampCoef;
} accm_sw_info_t;

RKAIQ_END_DECLARE

#endif

