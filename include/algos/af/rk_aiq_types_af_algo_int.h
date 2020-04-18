#ifndef __RK_AIQ_TYPES_AF_ALGO_INT_H__
#define __RK_AIQ_TYPES_AF_ALGO_INT_H__

#include "rk_aiq_types_af_algo.h"

typedef struct {
    unsigned char valid;
    int id;
    int depth;
} af_depth_info_t;

typedef struct {
    unsigned char valid;
    unsigned int af_target_pos;
    unsigned int af_target_width;
    unsigned int af_target_height;
} af_target_info_t;

typedef struct {
    unsigned char af_scene_changed;
    af_target_info_t af_target_info;
    af_depth_info_t af_depth_info[RKAIQ_RAWAF_SUMDATA_NUM];
    unsigned int global_sharpness_compensated[RKAIQ_RAWAF_SUMDATA_NUM];
} af_preprocess_result_t;

#endif
