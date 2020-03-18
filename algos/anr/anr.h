
#ifndef __RKAIQ_ANR_H__
#define __RKAIQ_ANR_H__

#include "stdio.h"
#include "string.h"
#include "math.h"

#include "uvnr.h"
#include "ynr.h"
#include "mfnr.h"
#include "bayernr.h"

typedef enum ANRresult_e {
    ANR_RET_SUCCESS				= 0,   // this has to be 0, if clauses rely on it
    ANR_RET_FAILURE				= 1,   // process failure
    ANR_RET_INVALID_PARM	    = 2,   // invalid parameter
    ANR_RET_WRONG_CONFIG		= 3,   // feature not supported
    ANR_RET_BUSY				= 4,   // there's already something going on...
    ANR_RET_CANCELED			= 5,   // operation canceled
    ANR_RET_OUTOFMEM			= 6,   // out of memory
    ANR_RET_OUTOFRANGE			= 7,   // parameter/value out of range
    ANR_RET_NULL_POINTER		= 8,   // the/one/all parameter(s) is a(are) NULL pointer(s)
    ANR_RET_DIVISION_BY_ZERO	= 9,   // a divisor equals ZERO
    ANR_RET_NO_INPUTIMAGE		= 10   // no input image
} ANRresult_t;

typedef enum ANRState_e {
  ANR_STATE_INVALID           = 0,                   /**< initialization value */
  ANR_STATE_INITIALIZED       = 1,                   /**< instance is created, but not initialized */
  ANR_STATE_STOPPED           = 2,                   /**< instance is confiured (ready to start) or stopped */
  ANR_STATE_RUNNING           = 3,                   /**< instance is running (processes frames) */
  ANR_STATE_LOCKED            = 4,                   /**< instance is locked (for taking snapshots) */
  ANR_STATE_MAX                                      /**< max */
} ANRState_t;

typedef enum ANROPMode_e {
  ANR_OP_MODE_INVALID           = 0,                   /**< initialization value */
  ANR_OP_MODE_AUTO       		= 1,                   /**< instance is created, but not initialized */
  ANR_OP_MODE_MANUAL           	= 2,                   /**< instance is confiured (ready to start) or stopped */
  ANR_OP_MODE_MAX                                      /**< max */
} ANROPState_t;

typedef struct ANR_Manual_Attr_s
{
	int bayernrEn;
	RKAnr_Bayernr_Params_Select_t stBayernrParamSelect;

	int mfnrEn;
	RKAnr_Mfnr_Params_Select_t stMfnrParamSelect;

	int ynrEn;
	RKAnr_Ynr_Params_Select_t stYnrParamSelect;

	int uvnrEn;
	RKAnr_Uvnr_Params_Select_t stUvnrParamSelect;
	
}ANR_Manual_Attr_t;

typedef struct ANR_Auto_Attr_s
{
	//all ISO params and select param
	//bayernr
	int bayernrEn;
	RKAnr_Bayernr_Params_t stBayernrParams;
	RKAnr_Bayernr_Params_Select_t stBayernrParamSelect;

	//mfnr
	int mfnrEn;
	RKAnr_Mfnr_Params_t stMfnrParams;
	RKAnr_Mfnr_Params_Select_t stMfnrParamSelect;

	//ynr
	int ynrEn;
	RKAnr_Ynr_Params_t stYnrParams;
	RKAnr_Ynr_Params_Select_t stYnrParamSelect;
	
	//uvnr
	int uvnrEn;
	RKAnr_Uvnr_Params_t stUvnrParams;
	RKAnr_Uvnr_Params_Select_t stUvnrParamSelect;
	
}ANR_Auto_Attr_t;


//anr context
typedef struct ANRContext_s{
	float fISO;
	float fEnvLight;
	ANRState_t eState;
	ANROPState_t eMode;
	
	ANR_Auto_Attr_t stAuto;
	ANR_Manual_Attr_t stManual;

	int refYuvBit;

}ANRContext_t;

//anr result
typedef struct ANRProcResult_s{
	int bayernrEn;
	RKAnr_Bayernr_Params_Select_t stBayernrParamSelect;

	int mfnrEn;
	RKAnr_Mfnr_Params_Select_t stMfnrParamSelect;

	int ynrEN;
	RKAnr_Ynr_Params_Select_t stYnrParamSelect;

	int uvnrEn;
	RKAnr_Uvnr_Params_Select_t stUvnrParamSelect;
	
}ANRProcResult_t;

typedef struct ANRHtmlParams_s{
	//auto mode get params from html and config to anrContext->stAuto
	RKAnr_Bayernr_Params_t stBayernrParams;
	RKAnr_Mfnr_Params_t stMfnrParams;
	RKAnr_Ynr_Params_t stYnrParams;
	RKAnr_Uvnr_Params_t stUvnrParams;
	int refYuvBit;
	int iso;
	int calibyuvBit;
}ANRHtmlParams_t;

//anr config or param
typedef struct ANRConfig_s{
	
	ANRState_t eState;
	ANROPState_t eMode;

	//html
	ANRHtmlParams_t stHtmlParams;
	int refYuvBit;
	

	//manual mode get nr & sharp levle from config to anrContext
	unsigned int u32NrLevel;
	unsigned int u32SharpLevel;

	//manual anr attr

	//xml params
	CalibDb_BayerNr_t stBayernrCalib;
	CalibDb_MFNR_t stMfnrCalib;
	CalibDb_UVNR_t stUvnrCalib;
	CalibDb_YNR_t stYnrCalib;
	
}ANRConfig_t;


#if 1
typedef RKAnr_Bayernr_Params_Select_t rk_aiq_isp_rawnr_t;
typedef RKAnr_Mfnr_Params_Select_t rk_aiq_isp_tnr_t;
typedef RKAnr_Ynr_Params_Select_t rk_aiq_isp_ynr_t;
typedef RKAnr_Uvnr_Params_Select_t rk_aiq_isp_uvnr_t;
typedef ANRHtmlParams_t rkaiq_anr_htmlparmas_t;
typedef ANRProcResult_t rkaiq_anr_procRes_t;
#endif


//后续加入sharp和noise的等级，给用户层调节的


//anr inint
ANRresult_t ANRInit(ANRContext_t **ppANRCtx);

//anr release
ANRresult_t ANRRelease(ANRContext_t *pANRCtx);

//anr config
ANRresult_t ANRConfig(ANRContext_t *pANRCtx, ANRConfig_t* pANRConfig);

//anr reconfig
ANRresult_t ANRReConfig(ANRContext_t *pANRCtx, ANRConfig_t* pANRConfig);

//anr preprocess
ANRresult_t ANRPreProcess(ANRContext_t *pANRCtx);

//anr process
ANRresult_t ANRProcess(ANRContext_t *pANRCtx, int ISO);

//anr get result
ANRresult_t ANRGetProcResult(ANRContext_t *pANRCtx, ANRProcResult_t* pANRResult);


#endif
