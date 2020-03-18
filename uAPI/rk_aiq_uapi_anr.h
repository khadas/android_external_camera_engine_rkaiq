#ifndef __RK_AIQ_UAPI_ANR_H_
#define __RK_AIQ_UAPI_ANR_H_
#include "base/xcam_common.h"
#include "rk_aiq.h"

#define MAX_ISO_STEP 				9 

/*******************************bayernr************************************/

//用于bayernr存储配置文件参数的结构体定义
typedef struct RKISP20_Bayernr_Params_s
{
	char bayernr_ver_char[64]; //version
    
	float a[MAX_ISO_STEP];//@iso50,100,200,400,800,1600,3200,6400,12800,共9个等级
	float b[MAX_ISO_STEP];

	int halfpatch;
	int halfblock;
	float filtpar[MAX_ISO_STEP];
	float ctrPit[7];

	//根据像素亮度调整降噪强度,9档ISO，8档亮度
	float luLevel[8];
	float luRatio[MAX_ISO_STEP][8];

	//去peak noise，9档ISO,3档去peak noise等级（lamda、2lamda、3lamda, 4lamda）
	float w[MAX_ISO_STEP][4];
    
    int peaknoisesigma;
    int sw_rawnr_gauss_en;
    
    int rgain_offs;
    int rgain_filp;
    int bgain_offs;
    int bgain_filp;

    float bayernr_edgesoftness;
    float bayernr_gauss_weight0;
    float bayernr_gauss_weight1;
    
    int sw_bayernr_edge_filter_en;
    float sw_bayernr_edge_filter_lumapoint[8];
    float sw_bayernr_edge_filter_wgt[MAX_ISO_STEP][8];
    float sw_bayernr_filter_strength[MAX_ISO_STEP];
    int   sw_bayernr_filter_lumapoint[16];
    int   sw_bayernr_filter_sigma[MAX_ISO_STEP][16];
    float sw_bayernr_filter_edgesofts[MAX_ISO_STEP];
    float sw_bayernr_filter_soft_threshold_ratio[MAX_ISO_STEP];
    float sw_bayernr_filter_out_wgt[MAX_ISO_STEP];
}RKISP20_Bayernr_Params_t;

typedef struct RKISP20_Bayernr_Params_Select_s
{
	char bayernr_ver_char[64]; //version
    
	float a;
	float b;
	float t0;
	int halfPatch;
	int halfBlock;
	float filtPar;
	float ctrPit[7];

	float luLevel[8];
	float luRatio[8];

	float w[4];
	
    int peaknoisesigma;
    int sw_rawnr_gauss_en;
	
	//oyyf add
	int rgain_offs;
    int rgain_filp;
    int bgain_offs;
    int bgain_filp;
	
	char bayernr_ver_char[64]; //version
	
    float bayernr_edgesoftness;
    float bayernr_gauss_weight0;
    float bayernr_gauss_weight1;
	
    int sw_bayernr_edge_filter_en;
    float sw_bayernr_edge_filter_lumapoint[8];    
    float sw_bayernr_edge_filter_wgt[8];
    float sw_bayernr_filter_strength;
    int   sw_bayernr_filter_lumapoint[16];
    int   sw_bayernr_filter_sigma[16];
    float sw_bayernr_filter_edgesofts;
    float sw_bayernr_filter_soft_threshold_ratio;
    float sw_bayernr_filter_out_wgt;
}RKISP20_Bayernr_Params_Select_t;

/*******************************mfnr************************************/
#define WAVELET_LEVEL_NUM 4
#define MAX_INTEPORATATION_LUMAPOINT 	17
#define POLT_ORDER      4
#define MAX_BACK_REF_NUM			1
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

//用于存储配置文件参数的结构体定义
typedef struct RKISP20_Mfnr_Params_s
{
	char mfnr_ver_char;
	
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
	
}RKISP20_Mfnr_Params_t;

typedef struct RKISP20_Mfnr_Params_Select_s
{
	char mfnr_ver_char;
	
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
}RKISP20_Mfnr_Params_Select_t;

/*******************************ynr************************************/
//噪声曲线点数
#define ISO_CURVE_POINT_BIT 			4
#define ISO_CURVE_POINT_NUM 			((1 << ISO_CURVE_POINT_BIT)+1)
#define RKAIQ_YNR_SIGMA_BITS      			10
//小波分解层数
#define WAVELET_LEVEL_NUM 4
#define 	GAIN_YNR_ENABLE_FLG				(GAIN_ENABLE_FLG & 1)
#define		GAIN_YNR_FIX_BITS_DECI			4   //(GAIN_ISP_PP_BITS_DECI + 6)
#define		GAIN_YUVNR_SOURCE_FLG			0	// contron pp source from mfnr or conv422, 1 from conv422, 0 from mfnr

typedef struct RKISP20_Ynr_Params_s
{
	char ynr_ver_char[64];
	//小波高频层标定参数ci和亮度噪声曲线
	float ciISO[MAX_ISO_STEP][12];                              //小波高频层标定参数ci和亮度噪声曲线
    float noiseSigma[MAX_ISO_STEP][ISO_CURVE_POINT_NUM];
	short lumaPoints[MAX_ISO_STEP][ISO_CURVE_POINT_NUM];

	float loFreqNoiseCi[MAX_ISO_STEP][WAVELET_LEVEL_NUM];       //小波低频层去噪tuning参数
	float loFreqDenoiseWeight[MAX_ISO_STEP][WAVELET_LEVEL_NUM];
	float loFreqBfScale[MAX_ISO_STEP][WAVELET_LEVEL_NUM];
	float loFreqLumaNrCurvePoint[MAX_ISO_STEP][6];
	float loFreqLumaNrCurveRatio[MAX_ISO_STEP][6];
	float loFreqDenoiseStrength[MAX_ISO_STEP][2];
	float loFreqDirectionStrength[MAX_ISO_STEP];

	float hiFreqDenoiseWeight[MAX_ISO_STEP][WAVELET_LEVEL_NUM]; //小波高频层去噪tuning参数
	float hiFreqBfScale[MAX_ISO_STEP][WAVELET_LEVEL_NUM];
	float hiFreqEdgeSoftness[MAX_ISO_STEP][WAVELET_LEVEL_NUM];
	float hiFreqLumaNrCurvePoint[MAX_ISO_STEP][6];
	float hiFreqLumaNrCurveRatio[MAX_ISO_STEP][6];
	float hiFreqDenoiseStrength[MAX_ISO_STEP];
	float hiFreqSoftThresholdScale[MAX_ISO_STEP][WAVELET_LEVEL_NUM];

	short radialNoiseCtrPoint[MAX_ISO_STEP][7];                 //径向去噪tuning参数
	float radialNoiseCtrRatio[MAX_ISO_STEP][7];

	float lscGainRatioAdjust[MAX_ISO_STEP][WAVELET_LEVEL_NUM];  //lsc gain调节高频层denoise weight参数

	//根据纹理信息(梯度值)调节每层的hiFreqDenoiseWeight对应参数
	float detailThre[MAX_ISO_STEP][6];
	float detailThreRatioLevel[MAX_ISO_STEP][3][6];
	float detailMinAdjDnW[MAX_ISO_STEP];

	//第四层高频系数根据纹理信息(梯度值)调节软阈值力度参数
	float detailThreLevel4[MAX_ISO_STEP][6];
	float detailThreRatioLevel4[MAX_ISO_STEP][6];

	short waveLetCoeffDeltaHi[MAX_ISO_STEP];                    //小波第四层LL递归参数
	short waveLetCoeffDeltaLo[MAX_ISO_STEP];
	short hiValueThre[MAX_ISO_STEP];
	short loValueThre[MAX_ISO_STEP];
    int   ynr_level4_max_gain[MAX_ISO_STEP];
}RKISP20_Ynr_Params_t;


typedef struct RKISP20_Ynr_Params_Select_s
{
	char ynr_ver_char[64];
	//小波高频层标定参数ci和亮度噪声曲线
	float ciISO[12];                              //小波高频层标定参数ci和亮度噪声曲线
    float noiseSigma[ISO_CURVE_POINT_NUM];
	short lumaPoints[ISO_CURVE_POINT_NUM];

	float loFreqNoiseCi[WAVELET_LEVEL_NUM];       //小波低频层去噪tuning参数
	float loFreqDenoiseWeight[WAVELET_LEVEL_NUM];
	float loFreqBfScale[WAVELET_LEVEL_NUM];
	float loFreqLumaNrCurvePoint[6];
	float loFreqLumaNrCurveRatio[6];
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

	//根据纹理信息(梯度值)调节每层的hiFreqDenoiseWeight对应参数
	float detailThre[6];
	float detailThreRatioLevel[3][6];
	float detailMinAdjDnW;

	//第四层高频系数根据纹理信息(梯度值)调节软阈值力度参数
	float detailThreLevel4[6];
	float detailThreRatioLevel4[6];

	short waveLetCoeffDeltaHi;                    //小波第四层LL递归参数
	short waveLetCoeffDeltaLo;
	short hiValueThre;
	short loValueThre;
    int   ynr_level4_max_gain;

	char ynr_ver_char[64];
}RKISP20_Ynr_Params_Select_t;

/*****************************uvnr*****************************/
typedef struct RKISP20_Uvnr_Params_s
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

}RKISP20_Uvnr_Params_t;



//用于存储根据iso选定参数的结构体定义
typedef struct RKISP20_Uvnr_Params_Select_s
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

}RKISP20_Uvnr_Params_Select_t;

/*******************************sharp**********************/
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
typedef struct RKISP20_Sharp_HW_Params_s
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



}RKISP20_Sharp_HW_Params_t;

typedef struct RKISP20_Sharp_HW_Params_Select_s
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



}RKISP20_Sharp_HW_Params_Select_t;

typedef struct RKISP20_Sharp_HW_V2_Params_s
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

}RKISP20_Sharp_HW_V2_Params_t;

typedef struct RKISP20_Sharp_HW_V2_Params_Select_s
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

}RKISP20_Sharp_HW_V2_Params_Select_t;

typedef struct RKISP20_Sharp_HW_V3_Params_s
{
	short luma_point		[RK_SHARPFILTER_LUMA_POINT_NUM];
	float lratio			[MAX_ISO_STEP];
	float hratio			[MAX_ISO_STEP];
	float sharp_ratio		[MAX_ISO_STEP];
	short lum_clip_h		[MAX_ISO_STEP][RK_SHARPFILTER_LUMA_POINT_NUM];	
	short ehf_th			[MAX_ISO_STEP];

	short kernel_range_filter	[MAX_ISO_STEP][RKSHAPRENHW_V3_RF_DIAM * RKSHAPRENHW_V3_RF_DIAM];
}RKISP20_Sharp_HW_V3_Params_t;

typedef struct RKISP20_Sharp_HW_V3_Params_Select_s
{
	short luma_point	[RK_SHARPFILTER_LUMA_POINT_NUM];
	float lratio		;
	float hratio		;
	float sharp_ratio	;
	short lum_clip_h	[RK_SHARPFILTER_LUMA_POINT_NUM];	
	short ehf_th		;

	short kernel_range_filter	[RKSHAPRENHW_V3_RF_DIAM * RKSHAPRENHW_V3_RF_DIAM];
}RKISP20_Sharp_HW_V3_Params_Select_t;

typedef struct RKISP20_Sharp_Params_s
{
	RKISP20_Sharp_HW_Params_t			rk_sharpen_params_V1;
	RKISP20_Sharp_HW_V2_Params_t		rk_sharpen_params_V2;
	RKISP20_Sharp_HW_V3_Params_t		rk_sharpen_params_V3;

}RKISP20_Sharp_Params_t;

typedef struct RKISP20_Sharp_Params_Select_s
{
	RKISP20_Sharp_HW_Params_Select_t		rk_sharpen_params_selected_V1;
	RKISP20_Sharp_HW_V2_Params_Select_t		rk_sharpen_params_selected_V2;
	RKISP20_Sharp_HW_V3_Params_Select_t		rk_sharpen_params_selected_V3;

}RKISP20_Sharp_Params_Select_t;


/********************************************Edgefilter*****************************************/
#define RK_EDGEFILTER_LUMA_POINT_NUM		8
#define RK_EDGEFILTER_PARAMS_NUM			15//21
#define	RKEDGEFILTER_LUMA_GAU_DIAM						3
#define	RKEDGEFILTER_DIR_SMTH_DIAM						5
#define	RKEDGEFILTER_GAUS_DIAM							5
#define	RKEDGEFILTER_SHRP_DIAM							3
#define	RKEDGEFILTER_DOG_DIAM							5

typedef struct RKISP20_EdgeFilter_Params_s
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
}RKISP20_EdgeFilter_Params_t;

typedef struct RKISP20_EdgeFilter_Params_Select_sS
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

}RKISP20_EdgeFilter_Params_Select_t;


/*************************** anr contxt*************************************/

typedef enum NROPMode_e {
  NR_OP_MODE_INVALID           = 0,                   /**< initialization value */
  NR_OP_MODE_AUTO       		= 1,                   /**< instance is created, but not initialized */
  NR_OP_MODE_MANUAL           	= 2,                   /**< instance is confiured (ready to start) or stopped */
  NR_OP_MODE_MAX                                      /**< max */
} NROPState_t;

typedef enum SHARPOPMode_e {
  SHARP_OP_MODE_INVALID           = 0,                   /**< initialization value */
  SHARP_OP_MODE_AUTO       		= 1,                   /**< instance is created, but not initialized */
  SHARP_OP_MODE_MANUAL           	= 2,                   /**< instance is confiured (ready to start) or stopped */
  SHARP_OP_MODE_MAX                                      /**< max */
} SHARPOPMode_t;

typedef struct RKISP20_NR_Manual_Attr_s
{
	RKISP20_Bayernr_Params_Select_t stBayernrParamSelect;
	RKISP20_Mfnr_Params_Select_t stMfnrParamSelect;
	RKISP20_Ynr_Params_Select_t stYnrParamSelect;
	RKISP20_Uvnr_Params_Select_t stUvnrParamSelect;
	
}RKISP20_NR_Manual_Attr_t;


typedef struct RKISP20_NR_Auto_Attr_s
{
	//all ISO params 
	RKISP20_Bayernr_Params_t stBayernrParams;
	RKISP20_Mfnr_Params_t stMfnrParams;
	RKISP20_Ynr_Params_t stYnrParams;	
	RKISP20_Uvnr_Params_t stUvnrParams;
	
}RKISP20_NR_Auto_Attr_t;

//nr context
typedef struct RKISP20_NR_Attr_s{
	bool bEnable;
	NROPState_t eMode;
	RKISP20_NR_Auto_Attr_t stNrAuto;
	RKISP20_NR_Manual_Attr_t stNrManual;

}RKISP20_NR_Attr_t;




typedef struct RKISP20_SHARP_Manual_Attr_s
{
	RKISP20_Sharp_Params_Select_t stSharpParamSelect;
	RKISP20_EdgeFilter_Params_Select_t stEdgefilterParamSelect;
	
}RKISP20_SHARP_Manual_Attr_t;

typedef struct RKISP20_SHARP_Auto_Attr_s
{
	//sharp
	RKISP20_Sharp_Params_t stSharpParams;
	
	//edgefilter
	RKISP20_EdgeFilter_Params_t stEdgefilterParams;
	
}RKISP20_SHARP_Auto_Attr_t;

typedef struct RKISP20_SHARP_Attr_s{
	bool bEnable;
	SHARPOPMode_t eMode;
	RKISP20_SHARP_Auto_Attr_t stSharpAuto;
	RKISP20_SHARP_Manual_Attr_t stSharpManual;
}RKISP20_SHARP_Attr_t;


XCamReturn rk_aiq_uapi_setNRAttr(const rk_aiq_sys_ctx_t* ctx, RKISP20_NR_Attr_t *pNRAttr);
XCamReturn rk_aiq_uapi_getNRAttr(const rk_aiq_sys_ctx_t* ctx, RKISP20_NR_Attr_t *pNRAttr);

XCamReturn rk_aiq_uapi_setSharpAttr(const rk_aiq_sys_ctx_t* ctx, RKISP20_SHARP_Attr_t *pSHARPAttr);
XCamReturn rk_aiq_uapi_getSharpAttr(const rk_aiq_sys_ctx_t* ctx, RKISP20_SHARP_Attr_t *pSHARPAttr);

#endif







