/*
 *rk_aiq_types_afec_algo_prvt.h
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

#ifndef _RK_AIQ_TYPES_AFEC_ALGO_PRVT_H_
#define _RK_AIQ_TYPES_AFEC_ALGO_PRVT_H_

#include "RkAiqCalibDbTypes.h"

RKAIQ_BEGIN_DECLARE

typedef enum {
    FEC_CORRECT_LEVEL0,          // 100%
    FEC_CORRECT_LEVEL1,          // 75%
    FEC_CORRECT_LEVEL2,          // 50%
    FEC_CORRECT_LEVEL3,          // 25%
    FEC_BYPASS
} FECCorrectLevel;

typedef struct FECContext_s {
    unsigned char initialized;
    unsigned int fec_en;
    unsigned int mesh_density; //0:16x8 1:32x16
    unsigned int fec_mesh_h_size;
    unsigned int fec_mesh_v_size;
    unsigned int fec_mesh_size;
    unsigned int pic_width;
    unsigned int pic_height;
    unsigned int sw_rd_vir_stride;
    unsigned int sw_wr_yuv_format; //0:YUV420 1:YUV422
    unsigned int sw_wr_vir_stride;
    unsigned int sw_fec_wr_fbce_mode; //0:normal 1:fbec
    unsigned short* meshxi;
    unsigned char* meshxf;
    unsigned short* meshyi;
    unsigned char* meshyf;
    char meshfile[256];
    unsigned char correct_level;
    unsigned int user_en;
    bool bypass;
} FECContext_t;

typedef struct FECContext_s* FECHandle_t;

typedef struct _RkAiqAlgoContext {
    FECHandle_t hFEC;
    void* place_holder[0];
} RkAiqAlgoContext;

RKAIQ_END_DECLARE

#endif

