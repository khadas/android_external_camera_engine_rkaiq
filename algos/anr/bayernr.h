/*

 */

#ifndef _RKAIQ_BAYERNR_H_
#define _RKAIQ_BAYERNR_H_

//在此处包含其它头文件
#include "stdio.h"
#include "math.h"
#include "stdlib.h"
#include "string.h"
#include "base/xcam_log.h"
#include "rk_aiq_comm.h"
#include "RkAiqCalibDbTypes.h"


#define MAX(a,b)  ((a) >= (b) ? (a):(b))
#define BAYERNR_MAX_ISO_LEVEL CALIBDB_NR_SHARP_MAX_ISO_LEVEL


///// LUT tables
#define LUTMAX 30.0
#define LUTMAXM1 29.0
#define LUTPRECISION 1000.0


typedef struct RKAnr_Bayernr_Params_s
{
	float a[9];//@iso50,100,200,400,800,1600,3200,6400,12800,共9个等级
	float b[9];

	int halfpatch;
	int halfblock;
	float filtpar[9];
	float ctrPit[7];

	//根据像素亮度调整降噪强度,9档ISO，8档亮度
	float luLevel[8];
	float luRatio[9][8];

	//去peak noise，9档ISO,3档去peak noise等级（lamda、2lamda、3lamda, 4lamda）
	float w[9][4];
    
    int peaknoisesigma;
    int sw_rawnr_gauss_en;
    
    int rgain_offs;
    int rgain_filp;
    int bgain_offs;
    int bgain_filp;

    char bayernr_ver_char[64];
    
    float bayernr_edgesoftness;
    float bayernr_gauss_weight0;
    float bayernr_gauss_weight1;
    
    int sw_bayernr_edge_filter_en;
    float sw_bayernr_edge_filter_lumapoint[8];
    float sw_bayernr_edge_filter_wgt[9][8];
    float sw_bayernr_filter_strength[9];
    int   sw_bayernr_filter_lumapoint[16];
    int   sw_bayernr_filter_sigma[9][16];
    float sw_bayernr_filter_edgesofts[9];
    float sw_bayernr_filter_soft_threshold_ratio[9];
    float sw_bayernr_filter_out_wgt[9];
}RKAnr_Bayernr_Params_t;



typedef struct RKAnr_Bayernr_Params_Select_s
{
	float a[3];//@iso50,100,200,400,800,1600,3200,6400,12800,共9个等级
	float b[3];
	float t0[3];
	int halfPatch;
	int halfBlock;
	float filtPar[3];
	float ctrPit[7];

	//根据像素亮度调整降噪强度,9档ISO，8档亮度
	float luLevel[8];
	float luRatio[8];

	//去peak noise，9档ISO,3档去peak noise等级（lamda、2lamda、3lamda）
	float w[4];
    int peaknoisesigma;
    int sw_rawnr_gauss_en;

	int rgain_offs;
    int rgain_filp;
    int bgain_offs;
    int bgain_filp;

	char bayernr_ver_char[64];
	 
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

	float sw_dgain[3];

	int gausskparsq;
	int sigmaPar;
	int thld_diff;
	int thld_chanelw;
	int pix_diff;
	int log_bypass;	//0 is none, 1 is G and RB all en,  2 only en G, 3 only RB;
}RKAnr_Bayernr_Params_Select_t;

typedef struct isp2x_rawnr_reg_s {
	unsigned char gauss_en;
	unsigned char log_bypass;

	unsigned short filtpar0;
	unsigned short filtpar1;
	unsigned short filtpar2;

	unsigned int dgain0;
	unsigned int dgain1;
	unsigned int dgain2;

	unsigned short luration[8];
	unsigned short lulevel[8];

	unsigned int gauss;
	unsigned short sigma;
	unsigned short pix_diff;

	unsigned int thld_diff;

	unsigned char gas_weig_scl2;
	unsigned char gas_weig_scl1;
	unsigned short thld_chanelw;

	unsigned short lamda;

	unsigned short fixw0;
	unsigned short fixw1;
	unsigned short fixw2;
	unsigned short fixw3;

	unsigned int wlamda0;
	unsigned int wlamda1;
	unsigned int wlamda2;

	unsigned short rgain_filp;
	unsigned short bgain_filp;
}isp2x_rawnr_reg_t;

RKAiqResult_t init_bayernr_params(RKAnr_Bayernr_Params_t *pParams, CalibDb_BayerNr_t *pCalibdb);
void select_bayernr_params_by_ISO(RKAnr_Bayernr_Params_t *stBayerNrParams, RKAnr_Bayernr_Params_Select_t *stBayerNrParamsSelected,int iso);

unsigned short bayernr_get_trans(int tmpfix);
void bayernr_fix_tranfer(RKAnr_Bayernr_Params_Select_t* rawnr);
void bayernr_fix_printf(isp2x_rawnr_reg_t * pRawnrCfg);


#endif  // BAYERNR_READ_PARM_H_

