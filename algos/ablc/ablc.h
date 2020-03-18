
#ifndef _RKAIQ_ABLC_H_
#define _RKAIQ_ABLC_H_

#include "stdio.h"
#include "string.h"
#include "math.h"
#include "base/xcam_log.h"
#include "rk_aiq_comm.h"
#include "RkAiqCalibDbTypes.h"


#define BLC_MAX_ISO_LEVEL (9)

typedef enum AblcResult_e {
    ABLC_RET_SUCCESS				= 0,   // this has to be 0, if clauses rely on it
    ABLC_RET_FAILURE				= 1,   // process failure
    ABLC_RET_INVALID_PARM	    = 2,   // invalid parameter
    ABLC_RET_WRONG_CONFIG		= 3,   // feature not supported
    ABLC_RET_BUSY				= 4,   // there's already something going on...
    ABLC_RET_CANCELED			= 5,   // operation canceled
    ABLC_RET_OUTOFMEM			= 6,   // out of memory
    ABLC_RET_OUTOFRANGE			= 7,   // parameter/value out of range
    ABLC_RET_NULL_POINTER		= 8,   // the/one/all parameter(s) is a(are) NULL pointer(s)
    ABLC_RET_DIVISION_BY_ZERO	= 9,   // a divisor equals ZERO
    ABLC_RET_NO_INPUTIMAGE		= 10   // no input image
} AblcResult_t;

typedef enum AblcState_e {
  ABLC_STATE_INVALID           = 0,                   /**< initialization value */
  ABLC_STATE_INITIALIZED       = 1,                   /**< instance is created, but not initialized */
  ABLC_STATE_STOPPED           = 2,                   /**< instance is confiured (ready to start) or stopped */
  ABLC_STATE_RUNNING           = 3,                   /**< instance is running (processes frames) */
  ABLC_STATE_LOCKED            = 4,                   /**< instance is locked (for taking snapshots) */
  ABLC_STATE_MAX                                      /**< max */
} AblcState_t;

typedef enum AblcOPMode_e {
  ABLC_OP_MODE_INVALID           = 0,                   /**< initialization value */
  ABLC_OP_MODE_AUTO       		= 1,                   /**< instance is created, but not initialized */
  ABLC_OP_MODE_MANUAL           	= 2,                   /**< instance is confiured (ready to start) or stopped */
  ABLC_OP_MODE_MAX                                      /**< max */
} AblcOPMode_t;

typedef struct AblcParams_s{
	int iso[BLC_MAX_ISO_LEVEL];
	short int blc_r[BLC_MAX_ISO_LEVEL];
	short int blc_gr[BLC_MAX_ISO_LEVEL];
	short int blc_gb[BLC_MAX_ISO_LEVEL];
	short int blc_b[BLC_MAX_ISO_LEVEL];
}AblcParams_t;

typedef struct AblcParamsSelect_s{
	short int blc_r;
	short int blc_gr;
	short int blc_gb;
	short int blc_b;
}AblcParamsSelect_t;

typedef struct AblcManualAttr_s{
	AblcParamsSelect_t stSelect;
}AblcManualAttr_t;

typedef struct AblcAutoAttr_s{
	AblcParams_t stParams;
	AblcParamsSelect_t stSelect;
}AblcAutoAttr_t;

typedef struct AblcProcResult_s{
	AblcParamsSelect_t stResult;
}AblcProcResult_t;

typedef struct AblcHtmlParams_s{
	AblcParams_t stParams;
}AblcHtmlParams_t;

typedef struct AblcConfig_s{
	AblcState_t eState;
	AblcOPMode_t eMode;
	AblcHtmlParams_t stHtmlParams;

	CalibDb_Blc_t stBlcCalib;
}AblcConfig_t;

typedef struct AblcContext_s{	
	int iso;
	AblcState_t eState;
	AblcOPMode_t eMode;

	AblcAutoAttr_t stAuto;
	AblcManualAttr_t stManual;
}AblcContext_t;


//typedef AblcProcResult_t rkaiq_ablc_procRes_t;
typedef AblcProcResult_t rk_aiq_isp_blc_t;

AblcResult_t AblcInit(AblcContext_t **ppAblcCtx);

AblcResult_t AblcRelease(AblcContext_t *pAblcCtx);

AblcResult_t AblcConfig(AblcContext_t *pAblcCtx, AblcConfig_t* pAblcConfig);

AblcResult_t AblcReConfig(AblcContext_t *pAblcCtx, AblcConfig_t* pAblcConfig);

AblcResult_t AblcPreProcess(AblcContext_t *pAblcCtx);

AblcResult_t AblcProcess(AblcContext_t *pAblcCtx, int isoValue);

AblcResult_t AblcGetProcResult(AblcContext_t *pAblcCtx, AblcProcResult_t* pAblcResult);



#endif

