
#ifndef __RKAIQ_ASHARP_EDGEFILTER_H__
#define __RKAIQ_ASHARP_EDGEFILTER_H__

#include "stdio.h"
#include "string.h"
#include "math.h"
#include "base/xcam_log.h"
#include "rk_aiq_comm.h"
#include "RkAiqCalibDbTypes.h"


#define MAX(a,b)  ((a) >= (b) ? (a):(b))
#define MIN(a,b)  ((a) <= (b) ? (a):(b))
#define EDGEFILTER_ROUND_F(x) (int)(((float)x)+(((x) > 0) ? 0.5 : (-0.5)))



#define INTERP1(x0, x1, ratio)			((ratio) * ((x1) - (x0)) + x0)
#define MAX_ISO_STEP 				CALIBDB_NR_SHARP_MAX_ISO_LEVEL 		//iso class num, from 50 to 12800,multiplied by 2
#define RK_EDGEFILTER_LUMA_POINT_NUM		8
#define RK_EDGEFILTER_PARAMS_NUM			15//21
#define	RKEDGEFILTER_LUMA_GAU_DIAM						3
#define	RKEDGEFILTER_DIR_SMTH_DIAM						5
#define	RKEDGEFILTER_GAUS_DIAM							5
#define	RKEDGEFILTER_SHRP_DIAM							3
#define	RKEDGEFILTER_DOG_DIAM							5

typedef struct RKAsharp_EdgeFilter_Params_s
{
	short edge_thed				[MAX_ISO_STEP];
	float dir_min				[MAX_ISO_STEP];
	float smoth4				[MAX_ISO_STEP];
	short alpha_adp_en			[MAX_ISO_STEP];
	float l_alpha				[MAX_ISO_STEP];
	float g_alpha				[MAX_ISO_STEP];
	short enhance_luma_point	[RK_EDGEFILTER_LUMA_POINT_NUM];
	short edge_thed_1			[MAX_ISO_STEP][RK_EDGEFILTER_LUMA_POINT_NUM];
	float clamp_pos_gaus		[MAX_ISO_STEP][RK_EDGEFILTER_LUMA_POINT_NUM];
	float clamp_neg_gaus		[MAX_ISO_STEP][RK_EDGEFILTER_LUMA_POINT_NUM];
	short clamp_pos_dog			[MAX_ISO_STEP][RK_EDGEFILTER_LUMA_POINT_NUM];
	short clamp_neg_dog			[MAX_ISO_STEP][RK_EDGEFILTER_LUMA_POINT_NUM];
	float detail_alpha_gaus		[MAX_ISO_STEP][RK_EDGEFILTER_LUMA_POINT_NUM];
	float detail_alpha_dog		[MAX_ISO_STEP][RK_EDGEFILTER_LUMA_POINT_NUM];

	float gaus_luma_kernel		[MAX_ISO_STEP][RKEDGEFILTER_LUMA_GAU_DIAM * RKEDGEFILTER_LUMA_GAU_DIAM];
	float h0_h_coef_5x5    		[MAX_ISO_STEP][RKEDGEFILTER_DIR_SMTH_DIAM 	* RKEDGEFILTER_DIR_SMTH_DIAM];
	float h1_v_coef_5x5    		[MAX_ISO_STEP][RKEDGEFILTER_DIR_SMTH_DIAM 	* RKEDGEFILTER_DIR_SMTH_DIAM];
	float h2_m_coef_5x5    		[MAX_ISO_STEP][RKEDGEFILTER_DIR_SMTH_DIAM 	* RKEDGEFILTER_DIR_SMTH_DIAM];
	float h3_p_coef_5x5    		[MAX_ISO_STEP][RKEDGEFILTER_DIR_SMTH_DIAM 	* RKEDGEFILTER_DIR_SMTH_DIAM];
	float h_coef_5x5    		[MAX_ISO_STEP][RKEDGEFILTER_GAUS_DIAM 		* RKEDGEFILTER_GAUS_DIAM];
	float gf_coef_3x3   		[MAX_ISO_STEP][RKEDGEFILTER_SHRP_DIAM 		* RKEDGEFILTER_SHRP_DIAM];
	float dog_kernel			[MAX_ISO_STEP][RKEDGEFILTER_DOG_DIAM 		* RKEDGEFILTER_DOG_DIAM];
}RKAsharp_EdgeFilter_Params_t;

typedef struct RKAsharp_EdgeFilter_Params_Select_s
{

	short edge_thed;
	float dir_min;
	float smoth4;
	short alpha_adp_en;
	float l_alpha;
	float g_alpha;
	short enhance_luma_point	[RK_EDGEFILTER_LUMA_POINT_NUM];
	short edge_thed_1			[RK_EDGEFILTER_LUMA_POINT_NUM];
	float clamp_pos_gaus		[RK_EDGEFILTER_LUMA_POINT_NUM];
	float clamp_neg_gaus		[RK_EDGEFILTER_LUMA_POINT_NUM];
	short clamp_pos_dog			[RK_EDGEFILTER_LUMA_POINT_NUM];
	short clamp_neg_dog			[RK_EDGEFILTER_LUMA_POINT_NUM];
	float detail_alpha_gaus		[RK_EDGEFILTER_LUMA_POINT_NUM];
	float detail_alpha_dog		[RK_EDGEFILTER_LUMA_POINT_NUM];

	float gaus_luma_kernel		[RKEDGEFILTER_LUMA_GAU_DIAM 	* RKEDGEFILTER_LUMA_GAU_DIAM];
	float h0_h_coef_5x5    		[RKEDGEFILTER_DIR_SMTH_DIAM 	* RKEDGEFILTER_DIR_SMTH_DIAM];
	float h1_v_coef_5x5    		[RKEDGEFILTER_DIR_SMTH_DIAM 	* RKEDGEFILTER_DIR_SMTH_DIAM];
	float h2_m_coef_5x5    		[RKEDGEFILTER_DIR_SMTH_DIAM 	* RKEDGEFILTER_DIR_SMTH_DIAM];
	float h3_p_coef_5x5    		[RKEDGEFILTER_DIR_SMTH_DIAM 	* RKEDGEFILTER_DIR_SMTH_DIAM];
	float h_coef_5x5    		[RKEDGEFILTER_GAUS_DIAM 		* RKEDGEFILTER_GAUS_DIAM];
	float gf_coef_3x3   		[RKEDGEFILTER_SHRP_DIAM 		* RKEDGEFILTER_SHRP_DIAM];
	float dog_kernel			[RKEDGEFILTER_DOG_DIAM 			* RKEDGEFILTER_DOG_DIAM];

}RKAsharp_EdgeFilter_Params_Select_t;

RKAiqResult_t init_edgefilter_params(RKAsharp_EdgeFilter_Params_t *pParams, CalibDb_EdgeFilter_t *pCalibdb);

void select_edgefilter_params_by_ISO(RKAsharp_EdgeFilter_Params_t *strkedgefilterParams,		RKAsharp_EdgeFilter_Params_Select_t *strkedgefilterParamsSelected, int iso);

#endif

