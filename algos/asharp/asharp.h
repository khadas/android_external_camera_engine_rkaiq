
#ifndef __RKAIQ_ASHARP_H__
#define __RKAIQ_ASHARP_H__

#include "stdio.h"
#include "string.h"
#include "math.h"
#include "sharp.h"
#include "edgefilter.h"

typedef enum AsharpResult_e {
    ASHARP_RET_SUCCESS				= 0,   // this has to be 0, if clauses rely on it
    ASHARP_RET_FAILURE				= 1,   // process failure
    ASHARP_RET_INVALID_PARM	    = 2,   // invalid parameter
    ASHARP_RET_WRONG_CONFIG		= 3,   // feature not supported
    ASHARP_RET_BUSY				= 4,   // there's already something going on...
    ASHARP_RET_CANCELED			= 5,   // operation canceled
    ASHARP_RET_OUTOFMEM			= 6,   // out of memory
    ASHARP_RET_OUTOFRANGE			= 7,   // parameter/value out of range
    ASHARP_RET_NULL_POINTER		= 8,   // the/one/all parameter(s) is a(are) NULL pointer(s)
    ASHARP_RET_DIVISION_BY_ZERO	= 9,   // a divisor equals ZERO
    ASHARP_RET_NO_INPUTIMAGE		= 10   // no input image
} AsharpResult_t;

typedef enum AsharpState_e {
  ASHARP_STATE_INVALID           = 0,                   /**< initialization value */
  ASHARP_STATE_INITIALIZED       = 1,                   /**< instance is created, but not initialized */
  ASHARP_STATE_STOPPED           = 2,                   /**< instance is confiured (ready to start) or stopped */
  ASHARP_STATE_RUNNING           = 3,                   /**< instance is running (processes frames) */
  ASHARP_STATE_LOCKED            = 4,                   /**< instance is locked (for taking snapshots) */
  ASHARP_STATE_MAX                                      /**< max */
} AsharpState_t;

typedef enum AsharpOPMode_e {
  ASHARP_OP_MODE_INVALID           = 0,                   /**< initialization value */
  ASHARP_OP_MODE_AUTO       		= 1,                   /**< instance is created, but not initialized */
  ASHARP_OP_MODE_MANUAL           	= 2,                   /**< instance is confiured (ready to start) or stopped */
  ASHARP_OP_MODE_MAX                                      /**< max */
} AsharpOPMode_t;

typedef struct Asharp_Manual_Attr_s
{
	int sharpEn;
	RKAsharp_Sharp_Params_Select_t stSharpParamSelect;

	int edgeFltEn;
	RKAsharp_EdgeFilter_Params_Select_t stEdgefilterParamSelect;
	
}Asharp_Manual_Attr_t;

typedef struct Asharp_Auto_Attr_s
{
	//all ISO params and select param	
	//sharp
	int sharpEn;
	RKAsharp_Sharp_Params_t stSharpParam;
	RKAsharp_Sharp_Params_Select_t stSharpParamSelect;
	
	//edgefilter
	int edgeFltEn;
	RKAsharp_EdgeFilter_Params_t stEdgefilterParams;
	RKAsharp_EdgeFilter_Params_Select_t stEdgefilterParamSelect;
}Asharp_Auto_Attr_t;


//anr context
typedef struct AsharpContext_s{
	float fGain;
	float fEnvLight;
	AsharpState_t eState;
	AsharpOPMode_t eMode;

	Asharp_Auto_Attr_t stAuto;
	Asharp_Manual_Attr_t stManual;

}AsharpContext_t;

//anr result
typedef struct AsharpProcResult_s{
	int sharpEn;
	RKAsharp_Sharp_Params_Select_t stSharpParamSelect;

	int edgeFltEn;
	RKAsharp_EdgeFilter_Params_Select_t stEdgefilterParamSelect;
	
}AsharpProcResult_t;

typedef struct AsharpHtmlParams_s{
	//auto mode get params from html and config to anrContext->stAuto
	RKAsharp_Sharp_Params_t stSharpParam;
	RKAsharp_EdgeFilter_Params_t stEdgefilterParams;
	int iso;
	
}AsharpHtmlParams_t;

//anr config or param
typedef struct AsharpConfig_s{
	
	AsharpState_t eState;
	AsharpOPMode_t eMode;

	//html
	AsharpHtmlParams_t stHtmlParams;

	//xml
	CalibDb_Sharp_t stSharpCalib;
	CalibDb_EdgeFilter_t stEdgeFltCalib;
	
}AsharpConfig_t;


#if 1
typedef RKAsharp_Sharp_Params_Select_t rk_aiq_isp_sharpen_t;
typedef RKAsharp_EdgeFilter_Params_Select_t rk_aiq_isp_edgeflt_t;
typedef AsharpHtmlParams_t rkaiq_asharp_htmlparmas_t;
typedef AsharpProcResult_t rkaiq_asharp_procRes_t;
#endif

typedef struct rkispp_sharp_fix_s {
	unsigned char alpha_adp_en;
	unsigned char yin_flt_en;
	unsigned char edge_avg_en;

	unsigned short hbf_ratio;
	unsigned char ehf_th;
	unsigned char pbf_ratio;

	unsigned char edge_thed;
	unsigned char dir_min;
	unsigned short smoth_th4;

	unsigned short l_alpha;
	unsigned short g_alpha;

	unsigned char pbf_k[3];
	unsigned char mrf_k[6];
	unsigned char mbf_k[12];
	unsigned char hrf_k[6];
	unsigned char hbf_k[3];

	char eg_coef[3];
	unsigned char eg_smoth[3];
	unsigned char eg_gaus[6];
	char dog_k[6];
	unsigned char lum_point[6];

	unsigned char pbf_shf_bits;
	unsigned char mbf_shf_bits;
	unsigned char hbf_shf_bits;

	unsigned char pbf_sigma[8];
	unsigned char lum_clp_m[8];
	char lum_min_m[8];
	unsigned char mbf_sigma[8];
	unsigned char lum_clp_h[8];
	unsigned char hbf_sigma[8];
	unsigned char edge_lum_thed[8];
	unsigned char clamp_pos[8];
	unsigned char clamp_neg[8];
	unsigned char detail_alpha[8];

	unsigned short rfl_ratio;
	unsigned short rfh_ratio;

	unsigned char m_ratio;
	unsigned char h_ratio;
}rkispp_sharp_fix_t;


AsharpResult_t AsharpInit(AsharpContext_t **ppAsharpCtx);

AsharpResult_t AsharpRelease(AsharpContext_t *pAsharpCtx);

AsharpResult_t AsharpConfig(AsharpContext_t *pAsharpCtx, AsharpConfig_t* pAsharpConfig);

AsharpResult_t AsharpReConfig(AsharpContext_t *pAsharpCtx, AsharpConfig_t* pAsharpConfig);

AsharpResult_t AsharpPreProcess(AsharpContext_t *pAsharpCtx);

AsharpResult_t AsharpProcess(AsharpContext_t *pAsharpCtx, int ISO);

AsharpResult_t AsharpGetProcResult(AsharpContext_t *pAsharpCtx, AsharpProcResult_t* pAsharpResult);

void ASharp_fix_transfer(RKAsharp_Sharp_Params_Select_t* sharp, RKAsharp_EdgeFilter_Params_Select_t* edgeflt);

void Asharp_fix_Printf(rkispp_sharp_fix_t  * pSharpCfg);

#endif
