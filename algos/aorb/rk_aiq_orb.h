#ifndef __RK_AIQ_ORB_H__
#define __RK_AIQ_ORB_H__

#include "orb.h"

typedef struct {
    unsigned int num_points;
	unsigned short pXs[MAX_POINTS];
	unsigned short pYs[MAX_POINTS];
	unsigned char pDescriptors[MAX_POINTS * DESCRIPTOR_SIZE];
	unsigned char pScores;
} orb_stat_t;

typedef struct {
    unsigned char valid;
    double homographyMat[9];
} orb_preprocess_result_t;

#endif
