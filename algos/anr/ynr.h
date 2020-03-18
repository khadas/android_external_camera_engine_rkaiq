#ifndef _RK_ISP20_YNR_H_
#define _RK_ISP20_YNR_H_

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "math.h"
#include "base/xcam_log.h"
#include "rk_aiq_comm.h"
#include "RkAiqCalibDbTypes.h"


#ifdef __cplusplus
extern "C" {
#endif

#define YNR_ROUND_F(x) (int)(((float)x)+(((x) > 0) ? 0.5 : (-0.5)))

#define MAX_ISO_STEP					CALIBDB_NR_SHARP_MAX_ISO_LEVEL
//噪声曲线点数
#define ISO_CURVE_POINT_BIT 			4
#define ISO_CURVE_POINT_NUM 			((1 << ISO_CURVE_POINT_BIT)+1)
#define RKAIQ_YNR_SIGMA_BITS      			10

//小波分解层数
#define WAVELET_LEVEL_NUM 4
#define 	GAIN_ENABLE_FLG					1
#define 	GAIN_YNR_ENABLE_FLG				(GAIN_ENABLE_FLG & 1)
#define		GAIN_YNR_FIX_BITS_DECI			4   //(GAIN_ISP_PP_BITS_DECI + 6)
#define		GAIN_YUVNR_SOURCE_FLG			0	// contron pp source from mfnr or conv422, 1 from conv422, 0 from mfnr


typedef struct RKAnr_Ynr_Params_Select_s
{
	//小波高频层标定参数ci和亮度噪声曲线
	float ciISO[12];                              //小波高频层标定参数ci和亮度噪声曲线
    float noiseSigma[ISO_CURVE_POINT_NUM];
	short lumaPoints[ISO_CURVE_POINT_NUM];

	float loFreqNoiseCi[WAVELET_LEVEL_NUM];       //小波低频层去噪tuning参数
	float loFreqDenoiseWeight[WAVELET_LEVEL_NUM];
	float loFreqBfScale[WAVELET_LEVEL_NUM];
	float loFreqLumaNrCurvePoint[6];
	float loFreqLumaNrCurveRatio[6];
	//float loFreqDenoiseStrength;
	float loFreqDenoiseStrength[2];
	float loFreqDirectionStrength;

	float hiFreqDenoiseWeight[WAVELET_LEVEL_NUM]; //小波高频层去噪tuning参数
	float hiFreqBfScale[WAVELET_LEVEL_NUM];
	float hiFreqEdgeSoftness[WAVELET_LEVEL_NUM];
	float hiFreqLumaNrCurvePoint[6];
	float hiFreqLumaNrCurveRatio[6];
	float hiFreqDenoiseStrength;
	float hiFreqSoftThresholdScale[WAVELET_LEVEL_NUM];

	short radialNoiseCtrPoint[7];                 //径向去噪tuning参数
	float radialNoiseCtrRatio[7];

	float lscGainRatioAdjust[WAVELET_LEVEL_NUM];  //lsc gain调节高频层denoise weight参数

	float detailThre[6];
	float detailThreRatioLevel[3][6];
	float detailMinAdjDnW;

	float detailThreLevel4[6];
	float detailThreRatioLevel4[6];

	short waveLetCoeffDeltaHi;                    //小波第四层LL递归参数
	short waveLetCoeffDeltaLo;
	short hiValueThre;
	short loValueThre;
    int   ynr_level4_max_gain;

	char ynr_ver_char[64];
}RKAnr_Ynr_Params_Select_t;

//
typedef struct RKAnr_Ynr_Params_s
{
	RKAnr_Ynr_Params_Select_t aYnrParamsISO[MAX_ISO_STEP];

	short rawBit;
	short isoValue;

	char ynr_ver_char[64];
}RKAnr_Ynr_Params_t;

typedef struct rkispp_ynr_fix_s {
	unsigned char ynr_sgm_dx[16];
	unsigned short ynr_lsgm_y[17];
	unsigned char ynr_lci[4];
	unsigned char ynr_lgain_min[4];
	unsigned char ynr_lgain_max;

	unsigned char ynr_lmerge_bound;
	unsigned char ynr_lmerge_ratio;

	unsigned char ynr_lweit_flt[4];
	unsigned short ynr_hsgm_y[17];
	unsigned char ynr_hlci[4];
	unsigned char ynr_lhci[4];
	unsigned char ynr_hhci[4];
	unsigned char ynr_hgain_sgm[4];
	unsigned char ynr_hweit_d[20];
	unsigned char ynr_hgrad_y[24];
	unsigned short ynr_hweit[4];
	unsigned char ynr_hmax_adjust;
	unsigned char ynr_hstrength;
	unsigned char ynr_lweit_cmp[2];
	unsigned char ynr_lmaxgain_lv4;
	unsigned short ynr_hstv_y[17];
	unsigned short ynr_st_scale[3];
} rkispp_ynr_fix_t;


RKAiqResult_t init_ynr_params(RKAnr_Ynr_Params_s *pYnrParams, CalibDb_YNR_t* pYnrCalib);

void select_ynr_params_by_ISO(RKAnr_Ynr_Params_t *stYnrParam, RKAnr_Ynr_Params_Select_t *stYnrParamSelected, short isoValue, short bitValue);

void ynr_fix_transfer(RKAnr_Ynr_Params_Select_t* ynr);

void ynr_fix_printf(rkispp_ynr_fix_t * pNrCfg);


#ifdef __cplusplus
}
#endif

#endif  // INITAL_ALG_PARAMS_YNR_H_
