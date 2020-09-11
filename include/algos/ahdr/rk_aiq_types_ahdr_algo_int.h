/******************************************************************************
 *
 * Copyright 2019, Fuzhou Rockchip Electronics Co.Ltd . All rights reserved.
 * No part of this work may be reproduced, modified, distributed, transmitted,
 * transcribed, or translated into any language or computer format, in any form
 * or by any means without written permission of:
 * Fuzhou Rockchip Electronics Co.Ltd .
 *
 *
 *****************************************************************************/
#ifndef __RK_AIQ_TYPES_AHDR_ALGO_INT_H__
#define __RK_AIQ_TYPES_AHDR_ALGO_INT_H__

//#include "rk_aiq_types.h"

typedef struct globalLuma_s
{
    float globalLumaMode;
    float EnvLv[6];
    float ISO[6];
    float Tolerance;
    float GlobeLuma[6];

} globalLuma_t ;

typedef struct detailsHighLight_s
{
    float DetailsHighLightMode;
    float OEPdf[6];
    float EnvLv[6];
    float Tolerance;
    float DetailsHighLight[6];

} detailsHighLight_t ;

typedef struct detailsLowLight_s
{
    float DetailsLowLightMode;
    float FocusLuma[6];
    float DarkPdf[6];
    float ISO[6];//use long frame
    float Tolerance;
    float DetailsLowLight[6];

} detailsLowLight_t ;

typedef struct tmoContrast_s
{
    float TmoContrastMode;
    float DynamicRange[6];
    float EnvLv[6];
    float Tolerance;
    float TmoContrast[6];

} tmoContrast_t ;

typedef struct Band_Prior_s
{
    bool isHdrGlobalTmo;
    float mode;
    float DynamicRange[6];
    float EnvLv[6];
    float Tolerance;
    float Strength[6];
} Band_Prior_t;

typedef struct moreSetting_s
{
    float clipgap0;
    float clipgap1;
    float clipratio0;
    float clipratio1;
    float damp;

} moreSetting_t ;

typedef struct tmo_config_s
{
    bool isLinearTmoOn;
    globalLuma_t Luma;
    detailsHighLight_t DtsHiLit;
    detailsLowLight_t DtsLoLit;
    tmoContrast_t Contrast;
    Band_Prior_t Band;
    moreSetting_t More;

} tmo_config_t ;

typedef struct merge_config_s {
    int MergeMode;
    float EnvLv[6]; //0: dark 1:bright
    float MoveCoef[6];
    float OECurve_smooth[6];  //current over exposure curve slope
    float OECurve_offset[6];  //current over exposure curve offset
    float MDCurveLM_smooth[6];  //Move Detect curve slope betwwen long frame and middle frame
    float MDCurveLM_offset[6];  //Move Detect curve offset betwwen long frame and middle frame
    float MDCurveMS_smooth[6];  //Move Detect curve slope betwwen middle frame and short frame
    float MDCurveMS_offset[6];  //Move Detect curve slope betwwen middle frame and short frame
    float OECurve_damp;
    float MDCurveLM_damp;
    float MDCurveMS_damp ;
} merge_config_t;

typedef struct AhdrConfig_s {
    merge_config_t merge_para;
    tmo_config_t   tmo_para;
} AhdrConfig_t;

typedef struct tmoCtrlData_s
{
    float stCoef;
    float stCoefMax;
    float stCoefMin;
    int   stMax;
    int   stMin;
} tmoCtrlData_t;

typedef struct AtmoBandPriorData_s
{
    bool en;
    float stCoef;
    float stCoefMax;
    float stCoefMin;
    int   stMax;
    int   stMin;
} AtmoBandPriorData_t;

typedef struct mgeCtrlData_S
{
    float stCoef;
    float stCoefMax;
    float stCoefMin;
    int   stSmthMax;
    int   stSmthMin;
    int   stOfstMax;
    int   stOfstMin;
} mgeCtrlData_t;

typedef struct amgeAttr_s
{
    mgeCtrlData_t stMDCurveLM;
    mgeCtrlData_t stMDCurveMS;
    mgeCtrlData_t stOECurve;
} amgeAttr_t;

typedef struct atmoAttr_S
{
    tmoCtrlData_t stGlobeLuma;
    tmoCtrlData_t stDtlsLL;
    tmoCtrlData_t stDtlsHL;
    tmoCtrlData_t stTmoContrast;
    AtmoBandPriorData_t stTmoBandPrior;
} atmoAttr_t;

typedef struct mmgeAttr_S
{
    unsigned short OECurve[17];
    unsigned short MDCurveLM[17];
    unsigned short MDCurveMS[17];
} mmgeAttr_t;

typedef struct mtmoAttr_S
{
    float stGlobeLuma;
    float stDtlsHL;
    float stDtlsLL;
    float stTmoContrast;
    float stBandPriorStrength;
} mtmoAttr_t;

typedef struct ahdrAttr_S
{
    bool      bUpdateTmo;
    bool      bUpdateMge;
    amgeAttr_t stMgeAuto;
    atmoAttr_t stTmoAuto;
} ahdrAttr_t;

typedef struct mhdrAttr_s
{
    bool      bUpdateTmo;
    bool      bUpdateMge;
    mmgeAttr_t stMgeManual;
    mtmoAttr_t stTmoManual;
} mhdrAttr_t;

typedef enum hdr_OpMode_s {
    HDR_OpMode_Auto = 1,
    HDR_OpMode_MANU = 2,
    HDR_OpMode_Fast = 3,
    HDR_OpMode_LINEAR = 4,
} hdr_OpMode_t;

typedef struct CurrCtlData_s
{
    float Envlv;
    float MoveCoef;
    float ISO;
    float OEPdf;
    float FocusLuma;
    float DarkPdf;
    float DynamicRange;
} CurrCtlData_t;

typedef struct CurrRegData_s
{
    float GlobalLuma;
    float DetailsLowlight;
    float DetailsHighlight;
    float TmoContrast;
    float BandPriorStrength;
} CurrRegData_t;

typedef struct hdrAttr_s
{
    bool      bEnable;
    hdr_OpMode_t    opMode;
    ahdrAttr_t    stAuto;
    mhdrAttr_t stManual;
    int level;
    int level_Linear_Dark;
    CurrCtlData_t CtlInfo;
    CurrRegData_t RegInfo;
} hdrAttr_t;

typedef struct MgeProcRes_s
{
    unsigned char  sw_hdrmge_mode;
    unsigned short sw_hdrmge_gain0_inv;
    unsigned short sw_hdrmge_gain0;
    unsigned short sw_hdrmge_gain1_inv;
    unsigned short sw_hdrmge_gain1;
    unsigned char  sw_hdrmge_gain2;
    unsigned char  sw_hdrmge_ms_dif_0p8;
    unsigned char  sw_hdrmge_lm_dif_0p9;
    unsigned char  sw_hdrmge_ms_dif_0p15;
    unsigned char  sw_hdrmge_lm_dif_0p15;
    unsigned short sw_hdrmge_l0_y[17];
    unsigned short sw_hdrmge_l1_y[17];
    unsigned short sw_hdrmge_e_y[17];
} MgeProcRes_t;

typedef struct TmoProcRes_s
{
    unsigned short sw_hdrtmo_cnt_vsize;
    unsigned char  sw_hdrtmo_gain_ld_off2;
    unsigned char  sw_hdrtmo_gain_ld_off1;
    unsigned char  sw_hdrtmo_big_en;
    unsigned char  sw_hdrtmo_nobig_en;
    unsigned char  sw_hdrtmo_newhist_en;
    unsigned char  sw_hdrtmo_cnt_mode;
    unsigned short sw_hdrtmo_expl_lgratio;
    unsigned char  sw_hdrtmo_lgscl_ratio;
    unsigned char  sw_hdrtmo_cfg_alpha;
    unsigned short sw_hdrtmo_set_gainoff;
    unsigned short sw_hdrtmo_set_palpha;
    unsigned short sw_hdrtmo_set_lgmax;
    unsigned short sw_hdrtmo_set_lgmin;
    unsigned char  sw_hdrtmo_set_weightkey;
    unsigned short sw_hdrtmo_set_lgmean;
    unsigned short sw_hdrtmo_set_lgrange1;
    unsigned short sw_hdrtmo_set_lgrange0;
    unsigned short sw_hdrtmo_set_lgavgmax;
    unsigned char  sw_hdrtmo_clipgap1;
    unsigned char  sw_hdrtmo_clipgap0;
    unsigned char  sw_hdrtmo_clipratio1;
    unsigned char  sw_hdrtmo_clipratio0;
    unsigned char  sw_hdrtmo_ratiol;
    unsigned short sw_hdrtmo_lgscl_inv;
    unsigned short sw_hdrtmo_lgscl;
    unsigned short sw_hdrtmo_lgmax;
    unsigned short sw_hdrtmo_hist_low;
    unsigned short sw_hdrtmo_hist_min;
    unsigned char  sw_hdrtmo_hist_shift;
    unsigned short sw_hdrtmo_hist_0p3;
    unsigned short sw_hdrtmo_hist_high;
    unsigned short sw_hdrtmo_palpha_lwscl;
    unsigned short sw_hdrtmo_palpha_lw0p5;
    unsigned short sw_hdrtmo_palpha_0p18;
    unsigned short sw_hdrtmo_maxgain;
    unsigned short sw_hdrtmo_maxpalpha;
} TmoProcRes_t;

typedef struct RkAiqAhdrProcResult_s
{
    TmoProcRes_t TmoProcRes;
    MgeProcRes_t MgeProcRes;
    hdrAttr_t hdrAttr;
    bool LongFrameMode;
    bool isHdrGlobalTmo;
    bool isLinearTmoOn;
} RkAiqAhdrProcResult_t;


#endif
