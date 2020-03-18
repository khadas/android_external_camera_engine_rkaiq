#ifndef __RKAIQ_ASHARP_SHARP_H__
#define __RKAIQ_ASHARP_SHARP_H__

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "math.h"
#include "base/xcam_log.h"
#include "rk_aiq_comm.h"
#include "RkAiqCalibDbTypes.h"


#define MAX(a,b)  ((a) >= (b) ? (a):(b))
#define MIN(a,b)  ((a) <= (b) ? (a):(b))
#define SHARP_ROUND_F(x) (int)(((float)x)+(((x) > 0) ? 0.5 : (-0.5)))
#define INTERP1(x0, x1, ratio)			((ratio) * ((x1) - (x0)) + x0)

#define MAX_ISO_STEP 				CALIBDB_NR_SHARP_MAX_ISO_LEVEL 		//iso class num, from 50 to 12800,multiplied by 2
//sharp
#define	IDX_SIGN							0
#define	IDX_INTE							1
#define	IDX_DECI							2
#define IDX_NUM								3

#define RK_EDGEFILTER_LUMA_POINT_NUM		8

#define RK_SHARPEN_HW_PARAMS_NUM			28

#define RK_SHARPFILTER_LUMA_POINT_NUM		8
#define RK_SHARPEN_HW_SHARP_PAR_NUM         (23)

#define	ISP_PP_SHARP_BLK_WIDTH				32
#define ISP_PP_SHARP_BLK_HEIGHT				128

#define	RKSHAPRENHW_GAU_DIAM							3
#define	RKSHAPRENHW_PBF_DIAM							3
#define	RKSHAPRENHW_MRF_DIAM							5
#define	RKSHAPRENHW_MBF_DIAM_X							17
#define	RKSHAPRENHW_MBF_DIAM_Y							13
#define	RKSHAPRENHW_HRF_DIAM							5
#define	RKSHAPRENHW_HBF_DIAM							3
#define	RKSHAPRENHW_AVG_DIAM							3

#define	RKSHAPRENHW_V2_GAU_DIAM							3
#define	RKSHAPRENHW_V2_PBF_DIAM							3
#define	RKSHAPRENHW_V2_HRF_DIAM							5
#define	RKSHAPRENHW_V2_HBF_DIAM_X						9
#define	RKSHAPRENHW_V2_HBF_DIAM_Y						9
#define	RKSHAPRENHW_V2_AVG_DIAM							3

#define	RKSHAPRENHW_V3_RF_DIAM							5
#define	RKSHAPRENHW_V3_AVG_DIAM							5


//用于存储配置文件参数的结构体定义
typedef struct RKAsharp_Sharp_HW_Params_s
{

	float lratio		[MAX_ISO_STEP];
	float hratio		[MAX_ISO_STEP];
	float M_ratio		[MAX_ISO_STEP];
	float H_ratio		[MAX_ISO_STEP];
	short luma_point	[RK_SHARPFILTER_LUMA_POINT_NUM];
	float luma_sigma	[MAX_ISO_STEP][RK_SHARPFILTER_LUMA_POINT_NUM];
	float pbf_gain		[MAX_ISO_STEP];
	float pbf_add		[MAX_ISO_STEP];
	float pbf_ratio		[MAX_ISO_STEP];
	short lum_clp_m		[MAX_ISO_STEP][RK_SHARPFILTER_LUMA_POINT_NUM];
	float lum_min_m		[MAX_ISO_STEP][RK_SHARPFILTER_LUMA_POINT_NUM];
	float mbf_gain		[MAX_ISO_STEP];
	float mbf_add		[MAX_ISO_STEP];
	short lum_clp_h		[MAX_ISO_STEP][RK_SHARPFILTER_LUMA_POINT_NUM];
	float hbf_ratio		[MAX_ISO_STEP];
	float hbf_gain		[MAX_ISO_STEP];
	float hbf_add		[MAX_ISO_STEP];
	short ehf_th		[MAX_ISO_STEP];

	float gaus_luma_kernel	[MAX_ISO_STEP][RKSHAPRENHW_PBF_DIAM * RKSHAPRENHW_PBF_DIAM];
	float kernel_pbf	[MAX_ISO_STEP][RKSHAPRENHW_PBF_DIAM * RKSHAPRENHW_PBF_DIAM];
	float h_rf_m		[MAX_ISO_STEP][RKSHAPRENHW_MRF_DIAM * RKSHAPRENHW_MRF_DIAM];
	float kernel_mbf	[MAX_ISO_STEP][RKSHAPRENHW_MBF_DIAM_Y * RKSHAPRENHW_MBF_DIAM_X];
	float h_rf_h		[MAX_ISO_STEP][RKSHAPRENHW_HRF_DIAM * RKSHAPRENHW_HRF_DIAM];
	float kernel_hbf	[MAX_ISO_STEP][RKSHAPRENHW_HBF_DIAM * RKSHAPRENHW_HBF_DIAM];



}RKAsharp_Sharp_HW_Params_t;

typedef struct RKAsharp_Sharp_HW_Params_Select_s
{
	float lratio		;
	float hratio		;
	float M_ratio		;
	float H_ratio		;
	short luma_point	[RK_SHARPFILTER_LUMA_POINT_NUM];
	float luma_sigma	[RK_SHARPFILTER_LUMA_POINT_NUM];
	float pbf_gain		;
	float pbf_add		;
	float pbf_ratio;
	short lum_clp_m		[RK_SHARPFILTER_LUMA_POINT_NUM];
	float lum_min_m		[RK_SHARPFILTER_LUMA_POINT_NUM];
	float mbf_gain		;
	float mbf_add		;
	short lum_clp_h		[RK_SHARPFILTER_LUMA_POINT_NUM];
	float hbf_ratio		;
	float hbf_gain		;
	float hbf_add		;
	short ehf_th		;
	float gaus_luma_kernel	[RKSHAPRENHW_GAU_DIAM * RKSHAPRENHW_GAU_DIAM];
	float kernel_pbf	[RKSHAPRENHW_PBF_DIAM * RKSHAPRENHW_PBF_DIAM];
	float h_rf_m		[RKSHAPRENHW_MRF_DIAM * RKSHAPRENHW_MRF_DIAM];
	float kernel_mbf	[RKSHAPRENHW_MBF_DIAM_Y * RKSHAPRENHW_MBF_DIAM_X];
	float h_rf_h		[RKSHAPRENHW_HRF_DIAM * RKSHAPRENHW_HRF_DIAM];
	float kernel_hbf	[RKSHAPRENHW_HBF_DIAM * RKSHAPRENHW_HBF_DIAM];



}RKAsharp_Sharp_HW_Params_Select_t;

typedef struct RKAsharp_Sharp_HW_V2_Params_s
{
	short luma_point		[RK_SHARPFILTER_LUMA_POINT_NUM];
	short luma_sigma		[MAX_ISO_STEP][RK_SHARPFILTER_LUMA_POINT_NUM];
	float pbf_gain			[MAX_ISO_STEP];
	float pbf_add			[MAX_ISO_STEP];
	float pbf_ratio			[MAX_ISO_STEP];
	float lratio			[MAX_ISO_STEP];
	float hratio			[MAX_ISO_STEP];
	float sharp_ratio		[MAX_ISO_STEP];
	short lum_clip_h		[MAX_ISO_STEP][RK_SHARPFILTER_LUMA_POINT_NUM];	
	float hbf_gain			[MAX_ISO_STEP];
	float hbf_add			[MAX_ISO_STEP];
	float hbf_ratio			[MAX_ISO_STEP];
	short ehf_th			[MAX_ISO_STEP];

	short gaus_luma_kernel		[MAX_ISO_STEP][RKSHAPRENHW_V2_PBF_DIAM * RKSHAPRENHW_V2_PBF_DIAM];
	short kernel_pre_bf			[MAX_ISO_STEP][RKSHAPRENHW_V2_PBF_DIAM * RKSHAPRENHW_V2_PBF_DIAM];
	short kernel_range_filter	[MAX_ISO_STEP][RKSHAPRENHW_V2_HRF_DIAM * RKSHAPRENHW_V2_HRF_DIAM];
	short kernel_hf_bf			[MAX_ISO_STEP][RKSHAPRENHW_V2_HBF_DIAM_Y * RKSHAPRENHW_V2_HBF_DIAM_X];

}RKAsharp_Sharp_HW_V2_Params_t;

typedef struct RKAsharp_Sharp_HW_V2_Params_Select_s
{
	short luma_point	[RK_SHARPFILTER_LUMA_POINT_NUM];
	short luma_sigma	[RK_SHARPFILTER_LUMA_POINT_NUM];
	float pbf_gain		;
	float pbf_add		;
	float pbf_ratio		;
	float lratio		;
	float hratio		;
	float sharp_ratio	;
	short lum_clip_h	[RK_SHARPFILTER_LUMA_POINT_NUM];	
	float hbf_gain		;
	float hbf_add		;
	float hbf_ratio		;
	short ehf_th		;

	short gaus_luma_kernel		[RKSHAPRENHW_V2_PBF_DIAM * RKSHAPRENHW_V2_PBF_DIAM];
	short kernel_pre_bf			[RKSHAPRENHW_V2_PBF_DIAM * RKSHAPRENHW_V2_PBF_DIAM];
	short kernel_range_filter	[RKSHAPRENHW_V2_HRF_DIAM * RKSHAPRENHW_V2_HRF_DIAM];
	short kernel_hf_bf			[RKSHAPRENHW_V2_HBF_DIAM_Y * RKSHAPRENHW_V2_HBF_DIAM_X];

}RKAsharp_Sharp_HW_V2_Params_Select_t;

typedef struct RKAsharp_Sharp_HW_V3_Params_s
{
	short luma_point		[RK_SHARPFILTER_LUMA_POINT_NUM];
	float lratio			[MAX_ISO_STEP];
	float hratio			[MAX_ISO_STEP];
	float sharp_ratio		[MAX_ISO_STEP];
	short lum_clip_h		[MAX_ISO_STEP][RK_SHARPFILTER_LUMA_POINT_NUM];	
	short ehf_th			[MAX_ISO_STEP];

	short kernel_range_filter	[MAX_ISO_STEP][RKSHAPRENHW_V3_RF_DIAM * RKSHAPRENHW_V3_RF_DIAM];
}RKAsharp_Sharp_HW_V3_Params_t;

typedef struct RKAsharp_Sharp_HW_V3_Params_Select_s
{
	short luma_point	[RK_SHARPFILTER_LUMA_POINT_NUM];
	float lratio		;
	float hratio		;
	float sharp_ratio	;
	short lum_clip_h	[RK_SHARPFILTER_LUMA_POINT_NUM];	
	short ehf_th		;

	short kernel_range_filter	[RKSHAPRENHW_V3_RF_DIAM * RKSHAPRENHW_V3_RF_DIAM];
}RKAsharp_Sharp_HW_V3_Params_Select_t;

typedef struct RKAsharp_Sharp_Params_s
{
	RKAsharp_Sharp_HW_Params_t			rk_sharpen_params_V1;
	RKAsharp_Sharp_HW_V2_Params_t		rk_sharpen_params_V2;
	RKAsharp_Sharp_HW_V3_Params_t		rk_sharpen_params_V3;

}RKAsharp_Sharp_Params_t;

typedef struct RKAsharp_Sharp_Params_Select_s
{
	RKAsharp_Sharp_HW_Params_Select_t			rk_sharpen_params_selected_V1;
	RKAsharp_Sharp_HW_V2_Params_Select_t		rk_sharpen_params_selected_V2;
	RKAsharp_Sharp_HW_V3_Params_Select_t		rk_sharpen_params_selected_V3;

}RKAsharp_Sharp_Params_Select_t;



RKAiqResult_t init_sharp_params_v1(RKAsharp_Sharp_HW_Params_t *pParams, CalibDb_Sharp_t *pCalibdb);

void select_rk_sharpen_hw_params_by_ISO				(RKAsharp_Sharp_HW_Params_t *strksharpenParams, 	RKAsharp_Sharp_HW_Params_Select_t *strksharpenParamsSelected, int iso);

void select_rk_sharpen_hw_v2_params_by_ISO			(RKAsharp_Sharp_HW_V2_Params_t *strksharpenParams, RKAsharp_Sharp_HW_V2_Params_Select_t *strksharpenParamsSelected, int iso);

void select_rk_sharpen_hw_v3_params_by_ISO 			(RKAsharp_Sharp_HW_V3_Params_t *strksharpenParams, RKAsharp_Sharp_HW_V3_Params_Select_t *strksharpenParamsSelected, int iso);

void select_sharpen_params_by_ISO					(RKAsharp_Sharp_Params_t *strksharpenParams,		RKAsharp_Sharp_Params_Select_t *strksharpenParamsSelected, int iso);


#endif

