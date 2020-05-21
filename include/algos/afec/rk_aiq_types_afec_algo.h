#ifndef __RK_AIQ_TYPES_AFEC_ALGO_H__
#define __RK_AIQ_TYPES_AFEC_ALGO_H__

#define FEC_MESH_XY_NUM_ALGO 524288

typedef struct {
    unsigned char update;
    unsigned char sw_fec_en;
    unsigned char crop_en;
    unsigned int crop_width;
    unsigned int crop_height;
    unsigned char mesh_density;
    unsigned int mesh_size;
    unsigned short meshxi[FEC_MESH_XY_NUM_ALGO];
    unsigned char meshxf[FEC_MESH_XY_NUM_ALGO];
    unsigned short meshyi[FEC_MESH_XY_NUM_ALGO];
    unsigned char meshyf[FEC_MESH_XY_NUM_ALGO];
} fec_preprocess_result_t;

#endif