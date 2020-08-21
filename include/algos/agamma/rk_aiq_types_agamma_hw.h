/*
 *rk_aiq_types_agamma_hw.h
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

#ifndef _RK_AIQ_TYPE_AGAMMA_HW_H_
#define _RK_AIQ_TYPE_AGAMMA_HW_H_

#include "rk_aiq_comm.h"



RKAIQ_BEGIN_DECLARE



typedef struct rk_aiq_gamma_cfg_s {
    int gamma_en;
    int gamma_out_mode;
    int gamma_out_segnum;//0:log segment ; 1:equal segment ;
    int gamma_out_offset;
    int gamma_table[45];
}  rk_aiq_gamma_cfg_t;


RKAIQ_END_DECLARE

#endif

