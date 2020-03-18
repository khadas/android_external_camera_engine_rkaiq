#ifndef __RK_ISP20_MFNR_PARSE_PARAMS_
#define __RK_ISP20_MFNR_PARSE_PARAMS_

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "base/xcam_log.h"
#include "rk_aiq_comm.h"
#include "RkAiqCalibDbTypes.h"


#define MAX(a,b)  ((a) >= (b) ? (a):(b))
#define MIN(a,b)  ((a) <= (b) ? (a):(b))
#define MFNR_ROUND_F(x) (int)(((float)x)+(((x) > 0) ? 0.5 : (-0.5)))




#define WAVELET_LEVEL_NUM 4


#define MAX_INTEPORATATION_LUMAPOINT 	17
#define POLT_ORDER      4

#define MAX_BACK_REF_NUM			1
#define MAX_ISO_STEP 				CALIBDB_NR_SHARP_MAX_ISO_LEVEL 		//iso class num, from 50 to 12800,multiplied by 2
#define LIMIT_RANGE_LEFT_BIT8		16
#define LIMIT_RANGE_RIGHT_BIT8	    236
#define Y_CALIBRATION_BITS  		12 	//assign to 12bits, confirmed by calibration program output
#define Y_SIGMA_TABLE_BITS			8 	//assign to 12bits, confirmed by calibration program output, 8 for matlab_3dnr
#define Y_SIGMA_TABLE_BITS_DEHAZE	8 	//assign to 12bits, confirmed by calibration program output
#define MFNR_POLYORDER 				POLT_ORDER		//noise cure order
#define MFNR_MAX_GAUS_SIZE			10		// 10 for 7x7, 6 for 5x5 3 for 3x3
#define MFNR_DIR_NUM 				2		//direction num in every frequency. 4 in wavelet, 2 in gaussian pyramid and average downscale
#define DIR_LO 						0
#define DIR_HI 						1
#define MFNR_MAX_LVL_UV				3		//max uv decomposition depth
#define MFNR_MAX_LVL 			    4		//max y decomposition depth
#define MFNR_MAX_LVL_DEPTH  		((MFNR_MAX_LVL_UV + 1 > MFNR_MAX_LVL) ? (MFNR_MAX_LVL_UV + 1) : MFNR_MAX_LVL) //max depth for uv
#define	MFNR_DELTA_SQR_EXP_POINT	17
#define LUMANRCURVE_STEP 			6		//luma nr curve class

#define	MFNR_TILE_W					16
#define	MFNR_TILE_H					128



#define INTERP1(x0, x1, ratio)			((ratio) * ((x1) - (x0)) + x0)

//用于存储配置文件参数的结构体定义
typedef struct RKAnr_Mfnr_Params_s
{
	int back_ref_num;

	int weight_limit_y	[MAX_ISO_STEP][MFNR_MAX_LVL_DEPTH];
	int weight_limit_uv	[MAX_ISO_STEP][MFNR_MAX_LVL_UV];
	double ratio_frq		[MAX_ISO_STEP][4];
	double luma_w_in_chroma[MAX_ISO_STEP][MFNR_MAX_LVL_UV];
	double awb_uv_ratio	[4][2];
	double curve		[MAX_ISO_STEP][MFNR_POLYORDER + 1];
	double curve_x0		[MAX_ISO_STEP];

	double ci			[MAX_ISO_STEP][MFNR_DIR_NUM][MFNR_MAX_LVL_DEPTH];
	double dnweight		[MAX_ISO_STEP][MFNR_DIR_NUM][MFNR_MAX_LVL_DEPTH];
	double scale			[MAX_ISO_STEP][MFNR_DIR_NUM][MFNR_MAX_LVL_DEPTH];
	double lumanrpoint	[MAX_ISO_STEP][MFNR_DIR_NUM][LUMANRCURVE_STEP];
	double lumanrcurve	[MAX_ISO_STEP][MFNR_DIR_NUM][LUMANRCURVE_STEP];
	double dnstr			[MAX_ISO_STEP][MFNR_DIR_NUM];
	double gfdelta		[MAX_ISO_STEP][MFNR_DIR_NUM][MFNR_MAX_LVL_DEPTH][MFNR_MAX_GAUS_SIZE];

	double ci_uv			[MAX_ISO_STEP][MFNR_DIR_NUM][MFNR_MAX_LVL_UV];
	double dnweight_uv	[MAX_ISO_STEP][MFNR_DIR_NUM][MFNR_MAX_LVL_UV];
	double scale_uv		[MAX_ISO_STEP][MFNR_DIR_NUM][MFNR_MAX_LVL_UV];
	double lumanrpoint_uv[MAX_ISO_STEP][MFNR_DIR_NUM][LUMANRCURVE_STEP];
	double lumanrcurve_uv[MAX_ISO_STEP][MFNR_DIR_NUM][LUMANRCURVE_STEP];
	double dnstr_uv		[MAX_ISO_STEP][MFNR_DIR_NUM];
	double gfdelta_uv	[MAX_ISO_STEP][MFNR_DIR_NUM][MFNR_MAX_LVL_UV][MFNR_MAX_GAUS_SIZE];

	double gfsigma		[MAX_ISO_STEP][MFNR_MAX_LVL][MFNR_MAX_GAUS_SIZE];

	double noise_sigma	[MAX_ISO_STEP][1 << Y_CALIBRATION_BITS];
	char mfnr_ver_char;
}RKAnr_Mfnr_Params_t;

typedef struct RKAnr_Mfnr_Params_Select_s
{
	int back_ref_num;

	int weight_limit_y[MFNR_MAX_LVL_DEPTH];
	int weight_limit_uv[MFNR_MAX_LVL_UV];
	double ratio_frq[4];
	double luma_w_in_chroma[MFNR_MAX_LVL_UV];
	double awb_uv_ratio[2];

	double ci					[MFNR_DIR_NUM][MFNR_MAX_LVL_DEPTH];
	double dnweight				[MFNR_DIR_NUM][MFNR_MAX_LVL_DEPTH];
	double scale					[MFNR_DIR_NUM][MFNR_MAX_LVL_DEPTH];
	double lumanrpoint			[MFNR_DIR_NUM][LUMANRCURVE_STEP];
	double lumanrcurve			[MFNR_DIR_NUM][LUMANRCURVE_STEP];
	double dnstr					[MFNR_DIR_NUM];
	double gfdelta				[MFNR_DIR_NUM][MFNR_MAX_LVL_DEPTH][MFNR_MAX_GAUS_SIZE];

	double ci_uv					[MFNR_DIR_NUM][MFNR_MAX_LVL_UV];
	double dnweight_uv			[MFNR_DIR_NUM][MFNR_MAX_LVL_UV];
	double scale_uv				[MFNR_DIR_NUM][MFNR_MAX_LVL_UV];
	double lumanrpoint_uv		[MFNR_DIR_NUM][LUMANRCURVE_STEP];
	double lumanrcurve_uv		[MFNR_DIR_NUM][LUMANRCURVE_STEP];
	double dnstr_uv				[MFNR_DIR_NUM];
	double gfdelta_uv			[MFNR_DIR_NUM][MFNR_MAX_LVL_UV][MFNR_MAX_GAUS_SIZE];

	double gfsigma				[MFNR_MAX_LVL][MFNR_MAX_GAUS_SIZE];

	double noise_sigma			[1 << Y_CALIBRATION_BITS];
	double noise_sigma_sample	[MAX_INTEPORATATION_LUMAPOINT];
	double noise_sigma_dehaze	[MAX_INTEPORATATION_LUMAPOINT];
	unsigned short  fix_x_pos			[MAX_INTEPORATATION_LUMAPOINT];
	unsigned short  fix_x_pos_dehaze	[MAX_INTEPORATATION_LUMAPOINT];


}RKAnr_Mfnr_Params_Select_t;

typedef struct rkispp_mfnr_fix_s {
	unsigned char opty_en;
	unsigned char optc_en;
	unsigned char gain_en;

	unsigned char pk0_y;
	unsigned char pk1_y;
	unsigned char pk0_c;
	unsigned char pk1_c;

	unsigned short glb_gain_cur;
	unsigned short glb_gain_nxt;
	unsigned short glb_gain_cur_div;
	unsigned char glb_gain_cur_sqrt;

	unsigned char sigma_x[16];
	unsigned short sigma_y[17];

	unsigned short luma_curve[6];

	unsigned short txt_th0_y;
	unsigned short txt_th1_y;
	unsigned short txt_th0_c;
	unsigned short txt_th1_c;
	unsigned short txt_thy_dlt;
	unsigned short txt_thc_dlt;

	unsigned char gfcoef_y0[6];
	unsigned char gfcoef_y1[3];
	unsigned char gfcoef_y2[3];
	unsigned char gfcoef_y3[3];

	unsigned char gfcoef_yg0[6];
	unsigned char gfcoef_yg1[3];
	unsigned char gfcoef_yg2[3];
	unsigned char gfcoef_yg3[3];

	unsigned char gfcoef_yl0[6];
	unsigned char gfcoef_yl1[3];
	unsigned char gfcoef_yl2[3];

	unsigned char gfcoef_cg0[6];
	unsigned char gfcoef_cg1[3];
	unsigned char gfcoef_cg2[3];

	unsigned char gfcoef_cl0[6];
	unsigned char gfcoef_cl1[3];

	unsigned short scale_yg[4];
	unsigned short scale_yl[3];
	unsigned short scale_cg[3];
	unsigned short scale_y2cg[3];
	unsigned short scale_cl[2];
	unsigned short scale_y2cl[3];

	unsigned char weight_y[3];
}rkispp_mfnr_fix_t;

RKAiqResult_t init_mfnr_params(RKAnr_Mfnr_Params_t *pParams, CalibDb_MFNR_t *pCalibdb);

void select_mfnr_params_by_ISO(RKAnr_Mfnr_Params_t *stmfnrParams, 	RKAnr_Mfnr_Params_Select_t *stmfnrParamsSelected, int iso, int bits_proc);

void mfnr_fix_transfer(RKAnr_Mfnr_Params_Select_t* mfnr);

void mfnr_fix_Printf(rkispp_mfnr_fix_t  * pMfnrCfg);


#endif


