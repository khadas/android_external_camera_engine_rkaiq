#ifndef __RKAIQ_UVNR_H_
#define __RKAIQ_UVNR_H_

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "math.h"
#include "base/xcam_log.h"
#include "rk_aiq_comm.h"
#include "RkAiqCalibDbTypes.h"


#define INTERP1(x0, x1, ratio)			((ratio) * ((x1) - (x0)) + x0)

#define MAX_ISO_STEP CALIBDB_NR_SHARP_MAX_ISO_LEVEL 

typedef struct RKAnr_Uvnr_Params_s
{
	char	rkuvnrISO[256];//@iso50,100,200,400,800,1600,3200,6400,12800
	
	//step0:uvgain预处理
	float	ratio[MAX_ISO_STEP];
	float	offset[MAX_ISO_STEP];

	//step1-下采样1
	//均值1
	int		wStep1[MAX_ISO_STEP];
	int		hStep1[MAX_ISO_STEP];
	int		meanSize1[MAX_ISO_STEP];

	//中值1
	int		nonMed1[4];
	int		medSize1[MAX_ISO_STEP];
	float	medRatio1[MAX_ISO_STEP];
	int		isMedIIR1[MAX_ISO_STEP];

	//双边1
	int		nonBf1[4];
	int		bfSize1[MAX_ISO_STEP];//或外部kernel
	float	sigmaR1[MAX_ISO_STEP];
	float	sigmaD1[MAX_ISO_STEP];
	float	uvgain1[MAX_ISO_STEP];
	float	bfRatio1[MAX_ISO_STEP];
	int		isRowIIR1[MAX_ISO_STEP];
	int		isYcopy1[MAX_ISO_STEP];

	//step2-下采样2
	int		block2_ext[4];
	//均值2
	int		wStep2[MAX_ISO_STEP];
	int		hStep2[MAX_ISO_STEP];
	int		meanSize2[MAX_ISO_STEP];

	//中值2
	int		nonMed2[4];
	int		medSize2[MAX_ISO_STEP];
	float	medRatio2[MAX_ISO_STEP];
	int		isMedIIR2[MAX_ISO_STEP];

	//双边2
	int		nonBf2[4];
	int		bfSize2[MAX_ISO_STEP];
	float	sigmaR2[MAX_ISO_STEP];
	float	sigmaD2[MAX_ISO_STEP];
	float	uvgain2[MAX_ISO_STEP];
	float	bfRatio2[MAX_ISO_STEP];
	int		isRowIIR2[MAX_ISO_STEP];
	int		isYcopy2[MAX_ISO_STEP];

	//step3
	int		nonBf3[4];
	//双边3
	int		bfSize3[MAX_ISO_STEP];
	float	sigmaR3[MAX_ISO_STEP];
	float	sigmaD3[MAX_ISO_STEP];
	float	uvgain3[MAX_ISO_STEP];
	float	bfRatio3[MAX_ISO_STEP];
	int		isRowIIR3[MAX_ISO_STEP];
	int		isYcopy3[MAX_ISO_STEP];

	//kernels
	float	kernel_3x3_table[3];
	float	kernel_5x5_talbe[5];
	float	kernel_9x9_table[8];
	int		kernel_9x9_num;

	//curves
	int		sigmaAdj_x[MAX_ISO_STEP];
	float	sigamAdj_y[MAX_ISO_STEP];

	int		threAdj_x[MAX_ISO_STEP];
	int		threAjd_y[MAX_ISO_STEP];

}RKAnr_Uvnr_Params_t;



//用于存储根据iso选定参数的结构体定义
typedef struct RKAnr_Uvnr_Params_Select_s
{
	char	select_iso[256];
	
	//step0:uvgain预处理
	float	ratio;
	float	offset;

	//step1-下采样1
	//均值1
	int		wStep1;
	int		hStep1;
	int		meanSize1;

	//中值1
	int		nonMed1[4];
	int		medSize1;
	float	medRatio1;
	int		isMedIIR1;

	//双边1
	int		nonBf1[4];
	int		bfSize1;//或外部kernel
	float	sigmaR1;
	float	sigmaD1;
	float	uvgain1;
	float	bfRatio1;
	int		isRowIIR1;
	int		isYcopy1;

	//step2-下采样2
	int		block2_ext[4];
	//均值2
	int		wStep2;
	int		hStep2;
	int		meanSize2;

	//中值2
	int		nonMed2[4];
	int		medSize2;
	float	medRatio2;
	int		isMedIIR2;

	//双边2
	int		nonBf2[4];
	int		bfSize2;
	float	sigmaR2;
	float	sigmaD2;
	float	uvgain2;
	float	bfRatio2;
	int		isRowIIR2;
	int		isYcopy2;

	//step3
	int		nonBf3[4];
	//双边3
	int		bfSize3;
	float	sigmaR3;
	float	sigmaD3;
	float	uvgain3;
	float	bfRatio3;
	int		isRowIIR3;
	int		isYcopy3;

	//kernels
	float	kernel_3x3_table[3];
	float	kernel_5x5_table[5];
	float	kernel_9x9_table[8];
	int		kernel_9x9_num;

	//curves
	int		sigmaAdj_x[9];
	float	sigmaAdj_y[9];

	int		threAdj_x[9];
	int		threAdj_y[9];

}RKAnr_Uvnr_Params_Select_t;

typedef struct rkispp_nr_fix_s {
	unsigned char uvnr_step1_en;
	unsigned char uvnr_step2_en;
	unsigned char nr_gain_en;
	unsigned char uvnr_nobig_en;
	unsigned char uvnr_big_en;

	unsigned char uvnr_gain_1sigma;
	unsigned char uvnr_gain_offset;
	unsigned char uvnr_gain_uvgain[2];
	unsigned char uvnr_gain_t2gen;
	unsigned char uvnr_gain_iso;
	unsigned char uvnr_t1gen_m3alpha;
	unsigned char uvnr_t1flt_mode;
	unsigned short uvnr_t1flt_msigma;
	unsigned char uvnr_t1flt_wtp;
	unsigned char uvnr_t1flt_wtq[8];
	unsigned char uvnr_t2gen_m3alpha;
	unsigned short uvnr_t2gen_msigma;
	unsigned char uvnr_t2gen_wtp;
	unsigned char uvnr_t2gen_wtq[5];
	unsigned short uvnr_t2flt_msigma;

	unsigned char uvnr_t2flt_wtp;
	unsigned char uvnr_t2flt_wt[3];

} rkispp_nr_fix_t;

RKAiqResult_t init_uvnr_params(RKAnr_Uvnr_Params_t *pParams, CalibDb_UVNR_t *pCalibdb);

void select_uvnr_params_by_ISO(RKAnr_Uvnr_Params_t *stRKUVNrParams, RKAnr_Uvnr_Params_Select_t *stRKUVNrParamsSelected, int iso);

float interpISO(int ISO_low, int ISO_high, float value_low, float value_high, int ISO, float value);

void uvnr_fix_transfer(RKAnr_Uvnr_Params_Select_t *uvnr);

void uvnr_fix_Printf(rkispp_nr_fix_t  * pNrCfg);


#endif

