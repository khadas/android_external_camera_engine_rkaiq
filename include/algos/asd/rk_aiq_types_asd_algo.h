#ifndef _RK_AIQ_ASD_ALGOS_H_
#define _RK_AIQ_ASD_ALGOS_H_

typedef struct {
    unsigned char valid;
    unsigned int pos;
    unsigned int hsize;
    unsigned int vsize;
} asd_target_info_t;

typedef struct {
    unsigned char asd_scene_changed;
    asd_target_info_t asd_target_info;
} asd_preprocess_result_t;


#endif
