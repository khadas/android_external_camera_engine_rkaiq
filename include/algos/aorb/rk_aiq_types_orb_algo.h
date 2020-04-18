#ifndef __RK_AIQ_ORB_ALGO_H__
#define __RK_AIQ_ORB_ALGO_H__

#define MAX_POINTS 40000
#define DESCRIPTOR_SIZE 15
#define ORB_FEATURE_DESCRIPTOR_BITS 120

typedef struct {
    unsigned int num_points;
	unsigned short pXs[MAX_POINTS];
	unsigned short pYs[MAX_POINTS];
	unsigned char pDescriptors[MAX_POINTS * DESCRIPTOR_SIZE];
	unsigned char pScores;
} rk_aiq_orb_algo_stat_t;

typedef struct {
    unsigned char orb_en;
    unsigned char limit_value;
    unsigned short max_feature;
} rk_aiq_orb_algo_meas_t;

typedef struct {
    int row;
    int col;
    bool descriptor[ORB_FEATURE_DESCRIPTOR_BITS];
} orb_point_t;

typedef struct {
    unsigned short row1;
    unsigned short col1;
    unsigned short row2;
    unsigned short col2;
    unsigned short score;
} orb_matched_point_t;

typedef struct{
    unsigned int left;
    unsigned int top;
    unsigned int right;
    unsigned int bottom;
    unsigned int width;
    unsigned int height;
} orb_rect_t;

typedef struct {
    unsigned char valid;
    orb_rect_t tracking_roi_rect;
    unsigned int matched_keypoints_num;
    orb_matched_point_t matched_keypoints[MAX_POINTS];
} orb_preprocess_result_t;

#endif
