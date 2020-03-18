#ifndef _RK_AIQ_UAPI_AE_TYPES_H_
#define _RK_AIQ_UAPI_AE_TYPES_H_
#include <string>
#include "list.h"
#include "rk_aiq_algo_des.h"

/*****************************************************************************/
/**
 * @brief   ISP2.0 AEC Algo Params
 */
/*****************************************************************************/

#define AEC_ECM_SCHEME_NAME         ( 20U )
typedef char                        AecEcmSchemeName_t[AEC_ECM_SCHEME_NAME];

#define AEC_ECM_PROFILE_NAME        ( 20U )
typedef char                        AecEcmProfileName_t[AEC_ECM_PROFILE_NAME];

#define AEC_DYNAMIC_SETPOINT_NAME         ( 20U )
typedef char                        AecDynamicSetpointName_t[AEC_ECM_SCHEME_NAME];

#define AEC_EXP_SEPARATE_NAME         ( 20U )
typedef char                        AecExpSeparateName_t[AEC_ECM_SCHEME_NAME];

typedef enum RkAiqApiAeOpType_e {
    RKAIQ_AEOPTYPE_MODE_INVALID   = 0,
    RKAIQ_AEOPTYPE_MODE_AUTO      = 1,
    RKAIQ_AEOPTYPE_MODE_MANUAL    = 2,
    RKAIQ_AEOPTYPE_MODE_MAX
} RkAiqApiAeOpType_t;

typedef enum RkAiqApiHdrAeRatioType_e {
    RKAIQ_HDRAE_RATIOTYPE_MODE_INVALID    = 0,
    RKAIQ_HDRAE_RATIOTYPE_MODE_AUTO   = 1,
    RKAIQ_HDRAE_RATIOTYPE_MODE_FIX    = 2,
    RKAIQ_HDRAE_RATIOTYPE_MODE_MAX
} RkAiqApiHdrAeRatioType_t;

typedef enum RkAiqApiHdrAeStrategyMode_e {
    RKAIQ_HDRAE_STRATEGY_MODE_INVALID    = 0,        /* invalid (only used for initialization) */
    RKAIQ_HDRAE_STRATEGY_MODE_LOWLIGHT_PRIOR     = 1,
    RKAIQ_HDRAE_STRATEGY_MODE_HIGHLIGHT_PRIOR    = 2,
    RKAIQ_HDRAE_STRATEGY_MODE_AUTO = 3,
    RKAIQ_HDRAE_STRATEGY_MODE_MAX
} RkAiqApiHdrAeStrategyMode_t;

enum AEC_LIGHT_MODE {
    AEC_LIGHT_MODE_MIN = -1,
    AEC_LIGHT_MODE_DAY = 0,
    AEC_LIGHT_MODE_NIGHT = 1,
    AEC_LIGHT_MODE_MAX = 2,
};

/*****************************************************************************/
/**
 *          AecSemMode_t
 *
 * @brief   mode type of AEC Scene Evaluation
 *
 */
/*****************************************************************************/
typedef enum AecSemMode_e {
    AEC_SCENE_EVALUATION_INVALID    = 0,        /* invalid (only used for initialization) */
    AEC_SCENE_EVALUATION_DISABLED   = 1,        /* Scene Evaluation disabled (fix setpoint) */
    AEC_SCENE_EVALUATION_FIX        = 2,        /* Scene Evaluation fix (static ROI) */
    AEC_SCENE_EVALUATION_ADAPTIVE   = 3,        /* Scene Evaluation adaptive (ROI caluclated by Scene Evaluation */
    AEC_SCENE_EVALUATION_MAX
} AecSemMode_t;
/*****************************************************************************/
/**
 *          AecBackLitMode_t
 *
 * @brief   mode type of AEC-BackLit
 *
 */
/*****************************************************************************/
typedef enum AecBackLitMode_e {
    AEC_BACKLIT_MODE_INVALID    = 0,        /* invalid (only used for initialization) */
    AEC_BACKLIT_MODE_WEIGHT_METHOD = 1,        /* Backlit based on Weight-Method MODE (only one integration time value) */
    AEC_BACKLIT_MODE_DARKROI_METHOD    = 2,        /* Backlit based on DarkROI-Method MODE (independent integration time value) */
    AEC_BACKLIT_MODE_MAX
} AecBackLitMode_t;

/*****************************************************************************/
typedef enum AecECMMode_e {
    AEC_ECM_MODE_INVALID     = 0,    /**< invalid exposure conversion module */
    AEC_ECM_MODE_1   = 1,  /* the original exposure conversion module ,   minimize the value of gain   */
    AEC_ECM_MODE_2    = 2,    /* the newl exposure conversion module ,imcrease the frame rate */
    AEC_ECM_MODE_MAX,
} AecECMMode_t;

/*****************************************************************************/
/**
 * @brief   mode type of Hdr-AEC
 *
 ****************************************************************************/
typedef enum AecHdrMode_e {
    AEC_HDR_MODE_INVALID    = 0,        /* invalid (only used for initialization) */
    AEC_HDR_MODE_STAGGER   = 1,     /* HDR STAGGER MODE (independent integration time value) */
    AEC_HDR_MODE_DCG    = 2,        /* HDR DCG MODE (only one integration time value) */
    AEC_HDR_MODE_MAX
} AecHdrMode_t;

/*****************************************************************************/
/**
 * @brief   Enumeration type to configure CamerIC ISP exposure measuring mode.
 *
 *****************************************************************************/
typedef enum CamExpMeasuringMode_e {
    CAM_EXP_MEASURING_MODE_INVALID    = 0,    /**< invalid histogram measuring mode   */
    CAM_EXP_MEASURING_MODE_1          = 1,    /**< Y = (R + G + B) x (85/256)         */
    CAM_EXP_MEASURING_MODE_2          = 2,    /**< Y = 16 + 0.25R + 0.5G + 0.1094B    */
    CAM_EXP_MEASURING_MODE_MAX,
} CamExpMeasuringMode_t;

typedef enum CamHistMode_e {
    CAM_HIST_MODE_INVALID       = 0,    /**< lower border (only for an internal evaluation) */
    CAM_HIST_MODE_RGB_COMBINED  = 1,    /**< RGB combined histogram */
    CAM_HIST_MODE_R             = 2,    /**< R histogram */
    CAM_HIST_MODE_G             = 3,    /**< G histogram */
    CAM_HIST_MODE_B             = 4,    /**< B histogram */
    CAM_HIST_MODE_Y             = 5,    /**< luminance histogram */
    CAM_HIST_MODE_MAX,                                  /**< upper border (only for an internal evaluation) */
} CamHistMode_t;

typedef struct Aec_Win_s{
  unsigned short h_offs;
  unsigned short v_offs;
  unsigned short h_size;
  unsigned short v_size;
}Aec_Win_t;

typedef struct RkAiqApiAecGainRange_s {
    int array_size;
    float *pGainRange;
} RkAiqApiAecGainRange_t;

typedef struct RkAiqApiAecDynamicSetpoint_s {
    void*                    p_next;                    /**< for adding to a list */
    AecDynamicSetpointName_t name;                       /**name */
    uint8_t                  enable;
    float*                   pExpValue;
    float*                   pDySetpoint;
    int                      array_size;
} RkAiqApiAecDynamicSetpoint_t;

typedef struct RkAiqApiLinAeRoute_Attr_s {
    void*                    p_next;                    /**< for adding to a list */
    AecExpSeparateName_t     name;                       /**name */
    float*                   pTimeDot;
    float*                   pGainDot;
    float*                   pIspgainDot;
    float*                   pPIrisDot;
    int                      array_size;
} RkAiqApiLinAeRoute_Attr_t;

typedef struct RkAiqApiHdrAeRoute_Attr_s {
    void*                    p_next;                    /**< for adding to a list */
    AecExpSeparateName_t     name;                       /**name */
    float*                   pLTimeDot;
    float*                   pMTimeDot;
    float*                   pSTimeDot;
    float*                   pLGainDot;
    float*                   pMGainDot;
    float*                   pSGainDot;
    float*                   pLDGainDot;
    float*                   pMDGainDot;
    float*                   pSDGainDot;
    float*                   pLIspgainDot;
    float*                   pMIspgainDot;
    float*                   pSIspgainDot;
    float*                   pPIrisDot;
    int                      array_size;
} RkAiqApiHdrAeRoute_Attr_t;

typedef struct RkAiqApiLinearAeManuCtrl_s {
    uint8_t                 ManualTimeEn;
    uint8_t                 ManualGainEn;
    uint8_t                 ManualIspDgainEn;
    uint8_t                 ManualPIrisEn;
    float                   TimeValue;
    float                   GainValue;
    float                   IspDGainValue;
    float                   PIrisValue;
} RkAiqApiLinearAeManuCtrl_t;

typedef struct RkAiqApiLinearAeInitExp_s {
    float                   InitTimeValue;
    float                   InitGainValue;
    float                   InitIspDGainValue;
    float                   InitPIrisValue;
} RkAiqApiLinearAeInitExp_t;

typedef struct RkAiqApiHdrAeManuCtrl_s {
    uint8_t                 ManualTimeEn;
    uint8_t                 ManualGainEn;
    uint8_t                 ManualDGainEn;
    uint8_t                 ManualIspDgainEn;
    uint8_t                 ManualPIrisEn;
    Cam3x1FloatMatrix_t     TimeValue;
    Cam3x1FloatMatrix_t     GainValue;
    Cam3x1FloatMatrix_t     DGainValue;
    Cam3x1FloatMatrix_t     IspDGainValue;
    float                   PIrisValue;
} RkAiqApiHdrAeManuCtrl_t;

typedef struct RkAiqApiHdrAeInitExp_s {
    Cam3x1FloatMatrix_t     InitTimeValue;
    Cam3x1FloatMatrix_t     InitGainValue;
    Cam3x1FloatMatrix_t     InitDGainValue;
    Cam3x1FloatMatrix_t     InitIspDGainValue;
    float                   InitPIrisValue;
} RkAiqApiHdrAeInitExp_t;

typedef struct RkAiqApiAecMode_s {
    uint8_t                 LinearAeEn;
    uint8_t                 LinearAeOpType;
    RkAiqApiLinearAeManuCtrl_t  LinearAeManuCtrl;
    RkAiqApiLinearAeInitExp_t  LinearAeInitExp;
    uint8_t                 HdrAeEn;
    uint8_t                 HdrAeOpType;
    RkAiqApiHdrAeManuCtrl_t HdrAeManuCtrl;
    RkAiqApiHdrAeInitExp_t  HdrAeInitExp;
} RkAiqApiAecMode_t;

typedef struct RkAiqApiAecSpeed_s {
    float                   DampOverStill;              /**< damping coefficient for still image mode */
    float                   DampUnderStill;             /**< damping coefficient for still image mode */
    float                   DampDark2BrightStill;
    float                   DampBright2DarkStill;
    float                   DampOverVideo;              /**< damping coefficient for video mode */
    float                   DampUnderVideo;             /**< damping coefficient for video mode */
} RkAiqApiAecSpeed_t;

typedef struct RkAiqApiAecMainCtrl_Attr_s {
    uint8_t                 Enable;
    //measure type
    CamHistMode_t           CamerIcIspHistMode;//cxf
    CamExpMeasuringMode_t   CamerIcIspExpMeasuringMode;//cxf
    uint8_t                 AeRunInterval;
    //aemode: manual/auto
    RkAiqApiAecMode_t       AecMode;
    //ae speed
    RkAiqApiAecSpeed_t      AeSpeed;
    //GridWeight
    Cam5x5UCharMatrix_t     NormalGridWeights;//cxf
    Cam5x5UCharMatrix_t     NightGridWeights;//cxf
    //DelayFrmNum
    uint8_t                 BlackDelay;
    uint8_t                 WhiteDelay;
    //FrameRateMode
    uint8_t                 isFpsFix;
    float                   *pFpsFixLinTimeDot;
    int                     LinFps_arraysize;
    float                   *pFpsFixHdrTimeDot;
    int                     HdrFps_arraysize;
} RkAiqApiAecMainCtrl_Attr_t;

typedef struct RkAiqApiLinearAE_Attr_s {
    float                   SetPoint;                   /**< set point to hit by the ae control system */
    float                   NightSetPoint;
    float                   ClmTolerance;
    uint8_t                 StrategyMode;
    List                    DySetpointList;
    RkAiqApiAecDynamicSetpoint_t *pDySetpoint[AEC_LIGHT_MODE_MAX];
} RkAiqApiLinearAE_Attr_t;

typedef struct RkAiqApiHdrAE_Attr_s {
    uint8_t                 SensorMode;
    uint8_t                 FrameNum;
    float                   DCG_Ratio;
    float                   ClmTolerance;
    uint8_t                 StrategyMode;
    //ExpRatioCtrl
    uint8_t                 ExpRatioType;
    Cam1x6FloatMatrix_t     RatioTimeDot;
    Cam1x6FloatMatrix_t     M2SRatioFix;
    Cam1x6FloatMatrix_t     L2MRatioFix;
    Cam1x6FloatMatrix_t     M2SRatioMax;
    Cam1x6FloatMatrix_t     L2MRatioMax;
    //ExpConvertCtrl
    Cam2x1FloatMatrix_t     RegSumLimitFac;
    uint8_t                 RegUnEqualLimitEn;
    uint8_t                 RegTimeMin;
    Cam2x1FloatMatrix_t     RegTimeMax;
    uint8_t                 EachGainSetEn;
    uint8_t                 EachTimeSetEn;
} RkAiqApiHdrAE_Attr_t;

typedef struct RkAiqApiAecHw_Attr_s{
    Aec_Win_t AeBig1;
    Aec_Win_t AeBig2;
    Aec_Win_t AeBig3;
    Aec_Win_t AeLite;
    Aec_Win_t YuvAe;
    Aec_Win_t HistBig1;
    Aec_Win_t HistBig2;
    Aec_Win_t HistBig3;
    Aec_Win_t HistLite;
}RkAiqApiAecHw_Attr_t;

typedef struct RkAiqApiExpParam_s {
    float integration_time;
    float analog_gain;
    float digital_gain;
    float isp_dgain;
    int   iso;
} RkAiqApiExpParam_t;

typedef struct RkAiqApiExpSensorParam_s {
    unsigned short fine_integration_time;
    unsigned short coarse_integration_time;
    unsigned short analog_gain_code_global;
    unsigned short digital_gain_global;
    unsigned short isp_digital_gain;
} RkAiqApiExpSensorParam_t;

typedef struct {
    RkAiqApiExpParam_t exp_real_params; //real value
    RkAiqApiExpSensorParam_t exp_sensor_params;//reg value
} RkAiqApiExpParamComb_t;

typedef struct RKAiqApiExpInfo_s {
    RkAiqApiExpParamComb_t LinearExp;
    RkAiqApiExpParamComb_t HdrExp[3];
    unsigned short Piris;
    unsigned short line_length_pixels;
    unsigned short frame_length_lines;
    bool converged;
} RKAiqApiExpInfo_t;


#endif
