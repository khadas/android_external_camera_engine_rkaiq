#ifndef _RK_AIQ_CALIB_TYPES_H_
#define _RK_AIQ_CALIB_TYPES_H_
#include "rk_aiq_algo_des.h"

#define CALIBDB_MAX_ISO_LEVEL 9
#define CALIBDB_NR_SHARP_MAX_ISO_LEVEL CALIBDB_MAX_ISO_LEVEL
#define CALIBDB_DPCC_MAX_ISO_LEVEL CALIBDB_MAX_ISO_LEVEL

/*****************************************************************************/
/**
 * @brief   ISP2.0 AEC Algo Params
 */
/*****************************************************************************/

#define AEC_ECM_SCHEME_NAME         ( 20U )
typedef char                        AecEcmSchemeName_t[AEC_ECM_SCHEME_NAME];

#define AEC_ECM_PROFILE_NAME        ( 20U )
typedef char                        AecEcmProfileName_t[AEC_ECM_PROFILE_NAME];

#define AEC_DYNAMIC_SETPOINT_NAME   ( 20U )
typedef char                        AecDynamicSetpointName_t[AEC_DYNAMIC_SETPOINT_NAME];

#define AEC_EXP_SEPARATE_NAME       ( 20U )
typedef char                        AecExpSeparateName_t[AEC_EXP_SEPARATE_NAME];

typedef enum _CalibDb_AeOpType_e {
    RKAIQ_AEOPTYPE_MODE_INVALID   = 0,
    RKAIQ_AEOPTYPE_MODE_AUTO      = 1,
    RKAIQ_AEOPTYPE_MODE_MANUAL    = 2,
    RKAIQ_AEOPTYPE_MODE_MAX
} CalibDb_AeOpType_t;

typedef enum _CalibDb_HdrAeRatioType_e {
    RKAIQ_HDRAE_RATIOTYPE_MODE_INVALID    = 0,
    RKAIQ_HDRAE_RATIOTYPE_MODE_AUTO   = 1,
    RKAIQ_HDRAE_RATIOTYPE_MODE_FIX    = 2,
    RKAIQ_HDRAE_RATIOTYPE_MODE_MAX
} CalibDb_HdrAeRatioType_t;

typedef enum _CalibDb_AeStrategyMode_e {
    RKAIQ_AEC_STRATEGY_MODE_AUTO    = 0,
    RKAIQ_AEC_STRATEGY_MODE_LOWLIGHT_PRIOR     = 1,
    RKAIQ_AEC_STRATEGY_MODE_HIGHLIGHT_PRIOR    = 2,
    RKAIQ_AEC_STRATEGY_MODE_MAX
} CalibDb_AeStrategyMode_t;

typedef enum _CalibDb_AecDayNightMode_e {
    AEC_DNMODE_MIN = -1,
    AEC_DNMODE_DAY = 0,
    AEC_DNMODE_NIGHT = 1, /*night mode or IR night-vision mode*/
    AEC_DNMODE_MAX = 2,
} CalibDb_AecDayNightMode_t;

/*****************************************************************************/
/**
 *          AecSemMode_t
 *
 * @brief   mode type of AEC Scene Evaluation
 *
 */
/*****************************************************************************/
typedef enum _CalibDb_AecSemMode_e {
    AEC_SCENE_EVALUATION_INVALID    = 0,        /* invalid (only used for initialization) */
    AEC_SCENE_EVALUATION_DISABLED   = 1,        /* Scene Evaluation disabled (fix setpoint) */
    AEC_SCENE_EVALUATION_FIX        = 2,        /* Scene Evaluation fix (static ROI) */
    AEC_SCENE_EVALUATION_ADAPTIVE   = 3,        /* Scene Evaluation adaptive (ROI caluclated by Scene Evaluation */
    AEC_SCENE_EVALUATION_MAX
} CalibDb_AecSemMode_t;
/*****************************************************************************/
/**
 *          AecBackLitMode_t
 *
 * @brief   mode type of AEC-BackLit
 *
 */
/*****************************************************************************/
typedef enum _CalibDb_AecBackLitMode_e {
    AEC_BACKLIT_MODE_INVALID    = 0,        /* invalid (only used for initialization) */
    AEC_BACKLIT_MODE_WEIGHT_METHOD = 1,        /* Backlit based on Weight-Method MODE (only one integration time value) */
    AEC_BACKLIT_MODE_DARKROI_METHOD    = 2,        /* Backlit based on DarkROI-Method MODE (independent integration time value) */
    AEC_BACKLIT_MODE_MAX
} CalibDb_AecBackLitMode_t;

/*****************************************************************************/
typedef enum _CalibDb_AecECMMode_e {
    AEC_ECM_MODE_INVALID     = 0,    /**< invalid exposure conversion module */
    AEC_ECM_MODE_1   = 1,  /* the original exposure conversion module ,   minimize the value of gain   */
    AEC_ECM_MODE_2    = 2,    /* the newl exposure conversion module ,imcrease the frame rate */
    AEC_ECM_MODE_MAX,
} CalibDb_AecECMMode_t;

/*****************************************************************************/
/**
 * @brief   Enumeration type to configure CamerIC ISP exposure measuring mode.
 *
 *****************************************************************************/
typedef enum _CalibDb_CamYRangeMode_e {
    CAM_YRANGE_MODE_INVALID    = 0,    /**< invalid y range mode   */
    CAM_YRANGE_MODE_FULL          = 1,    /**< Y = 0.299 * R + 0.587 * G + 0.114 * B,full range BT601*/
    CAM_YRANGE_MODE_LIMITED          = 2,    /**< Y = 16 + 0.25R + 0.5G + 0.1094B,limited range*/
    CAM_YRANGE_MODE_MAX,
} CalibDb_CamYRangeMode_t;

typedef enum _CalibDb_CamRawStatsMode_e {
    CAM_RAWSTATS_MODE_INVALID       = 0,    /**< lower border (only for an internal evaluation) */
    CAM_RAWSTATS_MODE_R             = 1,    /**< R channel */
    CAM_RAWSTATS_MODE_G             = 2,    /**< G channel */
    CAM_RAWSTATS_MODE_B             = 3,    /**< B channel */
    CAM_RAWSTATS_MODE_Y             = 4,    /**< luminance channel */
    CAM_RAWSTATS_MODE_MAX,                                  /**< upper border (only for an internal evaluation) */
} CalibDb_CamRawStatsMode_t;

typedef enum _CalibDb_CamHistStatsMode_e {
    CAM_HIST_MODE_INVALID       = 0,    /**< lower border (only for an internal evaluation) */
    CAM_HIST_MODE_RGB_COMBINED  = 1,    /**< RGB combined histogram */
    CAM_HIST_MODE_R             = 2,    /**< R histogram */
    CAM_HIST_MODE_G             = 3,    /**< G histogram */
    CAM_HIST_MODE_B             = 4,    /**< B histogram */
    CAM_HIST_MODE_Y             = 5,    /**< luminance histogram */
    CAM_HIST_MODE_MAX,                                  /**< upper border (only for an internal evaluation) */
} CalibDb_CamHistStatsMode_t;

typedef struct CalibDb_Aec_Win_s {
    unsigned short h_offs;
    unsigned short v_offs;
    unsigned short h_size;
    unsigned short v_size;
} CalibDb_Aec_Win_t;

/*****************************************************************************/
/**
 * @brief   ISP2.0 AEC API Exp-Route Params
 */
/*****************************************************************************/
#define AEC_ROUTE_MAX_NODES (10)
typedef struct CalibDb_LinAeRoute_Attr_s {
    AecExpSeparateName_t     name;                       /**name */
    float                    TimeDot[AEC_ROUTE_MAX_NODES];
    float                    AGainDot[AEC_ROUTE_MAX_NODES];
    float                    DGainDot[AEC_ROUTE_MAX_NODES];
    float                    IspgainDot[AEC_ROUTE_MAX_NODES];
    float                    PIrisDot[AEC_ROUTE_MAX_NODES];
    int                      array_size;
} CalibDb_LinAeRoute_Attr_t;

typedef struct CalibDb_HdrAeRoute_Attr_s {
    AecExpSeparateName_t     name;                       /**name */
    float                    HdrTimeDot[3][AEC_ROUTE_MAX_NODES];
    float                    HdrAGainDot[3][AEC_ROUTE_MAX_NODES];
    float                    HdrDGainDot[3][AEC_ROUTE_MAX_NODES];
    float                    HdrIspDGainDot[3][AEC_ROUTE_MAX_NODES];
    float                    PIrisDot[AEC_ROUTE_MAX_NODES];
    int                      array_size;
} CalibDb_HdrAeRoute_Attr_t;

typedef struct CalibDb_AeRoute_Attr_s {
    CalibDb_LinAeRoute_Attr_t LinAeSeperate[AEC_DNMODE_MAX];
    CalibDb_HdrAeRoute_Attr_t HdrAeSeperate[AEC_DNMODE_MAX];
} CalibDb_AeRoute_Attr_t;

//2). Auto exposure
typedef struct CalibDb_AeSpeed_s {
    float                   DampOverStill;              /**< damping coefficient for still image mode */
    float                   DampUnderStill;             /**< damping coefficient for still image mode */
    float                   DampDark2BrightStill;
    float                   DampBright2DarkStill;
    float                   DampOverVideo;              /**< damping coefficient for video mode */
    float                   DampUnderVideo;             /**< damping coefficient for video mode */
} CalibDb_AeSpeed_t;

typedef struct CalibDb_AeRange_s {
    float                   Min;
    float                   Max;
} CalibDb_AeRange_t;

typedef struct CalibDb_LinAeRange_s {
    CalibDb_AeRange_t      stExpTimeRange;
    CalibDb_AeRange_t      stAGainRange;
    CalibDb_AeRange_t      stDGainRange;
    CalibDb_AeRange_t      stIspDGainRange;
    CalibDb_AeRange_t      stPIrisRange;
} CalibDb_LinAeRange_t;

typedef struct CalibDb_HdrAeRange_s {
    CalibDb_AeRange_t      stExpTimeRange[3];
    CalibDb_AeRange_t      stAGainRange[3];
    CalibDb_AeRange_t      stDGainRange[3];
    CalibDb_AeRange_t      stIspDGainRange[3];
    CalibDb_AeRange_t      stPIrisRange;
} CalibDb_HdrAeRange_t;

typedef struct CalibDb_AeFrmRateAttr_s {
    bool             isFpsFix;
    uint8_t          FpsValue;
    float            FpsFixLinTimeDot[AEC_ROUTE_MAX_NODES];
    int              LinFps_arraysize;
    float            FpsFixHdrTimeDot[AEC_ROUTE_MAX_NODES];
    int              HdrFps_arraysize;
} CalibDb_AeFrmRateAttr_t;


//3.) Init expsore
typedef struct CalibDb_LinExpInitExp_s {
    float                   InitTimeValue;
    float                   InitAGainValue;
    float                   InitDGainValue;
    float                   InitIspDGainValue;
    float                   InitPIrisValue;
    int                     array_size;
} CalibDb_LinExpInitExp_t;

typedef struct CalibDb_HdrExpInitExp_s {
    Cam3x1FloatMatrix_t     InitTimeValue;
    Cam3x1FloatMatrix_t     InitAGainValue;
    Cam3x1FloatMatrix_t     InitDGainValue;
    Cam3x1FloatMatrix_t     InitIspDGainValue;
    float                   InitPIrisValue;
    int                     array_size;
} CalibDb_HdrExpInitExp_t;

typedef struct CalibDb_ExpInitExp_s {
    CalibDb_LinExpInitExp_t         stLinExpInitExp;
    CalibDb_HdrExpInitExp_t         stHdrExpInitExp;
} CalibDb_ExpInitExp_t;

typedef struct CalibDb_AeAttr_s {

    CalibDb_AeSpeed_t        stAeSpeed;
    //DelayFrmNum
    uint8_t                  BlackDelayFrame;
    uint8_t                  WhiteDelayFrame;
    //AeRange
    bool                     SetAeRangeEn;
    CalibDb_LinAeRange_t     stLinAeRange;
    CalibDb_HdrAeRange_t     stHdrAeRange;
    //Auto/Fixed fps
    CalibDb_AeFrmRateAttr_t  stFrmRate;

} CalibDb_AeAttr_t;
//manual exposure
typedef struct CalibDb_LinMeAttr_s {
    bool                 ManualTimeEn;
    bool                 ManualAGainEn;
    bool                 ManualDGainEn;
    bool                 ManualIspDgainEn;
    bool                 ManualPIrisEn;
    float                TimeValue;
    float                AGainValue;
    float                DGainValue;
    float                IspDGainValue;
    float                PIrisValue;
} CalibDb_LinMeAttr_t;

typedef struct CalibDb_HdrMeAttr_s {
    bool                    ManualTimeEn;
    bool                    ManualAGainEn;
    bool                    ManualDGainEn;
    bool                    ManualIspDgainEn;
    bool                    ManualPIrisEn;
    Cam3x1FloatMatrix_t     TimeValue;
    Cam3x1FloatMatrix_t     AGainValue;
    Cam3x1FloatMatrix_t     DGainValue;
    Cam3x1FloatMatrix_t     IspDGainValue;
    float                   PIrisValue;
} CalibDb_HdrMeAttr_t;

typedef struct CalibDb_MeAttr_s {
    CalibDb_LinMeAttr_t     stLinMe;
    CalibDb_HdrMeAttr_t     stHdrMe;
} CalibDb_MeAttr_t;

// DayorNight Switch
typedef struct CalibDb_IRNightMode_s {
    uint8_t                 enable;
    float                   IRRgain;
    float                   IRBgain;
    float                   MaxWbDis;
    uint8_t                 Night2DayFrmCnt;
    float                   Night2DayFacTh;
    float                   VbPercent;
} CalibDb_IRNightMode_t;

typedef struct CalibDb_VBNightMode_s {
    uint8_t                 enable;
    float                   Night2DayFacTh;
    uint8_t                 Night2DayFrmCnt;
} CalibDb_VBNightMode_t;

typedef struct CalibDb_DNSwitch_Attr_s {
    uint8_t                     DNTrigger;
    CalibDb_AecDayNightMode_t   DNMode;
    uint8_t                     NightMode;
    float                       Day2NightFacTh;
    uint8_t                     Day2NightFrmCnt;
    CalibDb_VBNightMode_t       stVBNightMode;
    CalibDb_IRNightMode_t       stIRNightMode;
} CalibDb_DNSwitch_Attr_t;

typedef struct CalibDb_AecCommon_Attr_s {
    bool                   Bypass;
    CalibDb_CamRawStatsMode_t        RawStatsMode;
    CalibDb_CamHistStatsMode_t       HistStatsMode;
    CalibDb_CamYRangeMode_t          YRangeMode;
    uint8_t                          AecRunInterval;
    CalibDb_AeOpType_t               AecOpType;
    //GridWeight
    Cam5x5UCharMatrix_t              DayGridWeights;
    Cam5x5UCharMatrix_t              NightGridWeights;

    CalibDb_ExpInitExp_t             stInitExp;
    //DayOrNight switch
    CalibDb_DNSwitch_Attr_t          stDNSwitch;
    //ExpSeperate
    CalibDb_AeRoute_Attr_t           stAeRoute;
    //manual expvalue
    CalibDb_MeAttr_t                 stManual;
    CalibDb_AeAttr_t                 stAuto;
} CalibDb_AecCommon_Attr_t;

/*****************************************************************************/
/**
 * @brief   ISP2.0 AEC API algo Config Params(private)
 */
/*****************************************************************************/
typedef struct CalibDb_AecInterAdjust_s {
    uint8_t     enable;
    float       dluma_high_th;
    float       dluma_low_th;
    uint8_t     trigger_frame;
} CalibDb_AecInterAdjust_t;

typedef struct CalibDb_AecOverExpControl_s {
    AecDynamicSetpointName_t      name;                       /**name */
    float K1;
    float K2;
    float K3;
    float OEC_Pdf_max;
    float OEC_Pdf_th;
    float OEC_Tolerance_max;
    float* pGainLevel;
    float* pDyMaxLuma;
    int   array_size;
} CalibDb_AecOverExpControl_t;

#define AEC_SETPOINT_MAX_NODES 10
typedef struct CalibDb_AecDynamicSetpoint_s {
    AecDynamicSetpointName_t      name;                       /**name */
    float ExpValue[AEC_SETPOINT_MAX_NODES];
    float DySetpoint[AEC_SETPOINT_MAX_NODES];
    int   array_size;
} CalibDb_AecDynamicSetpoint_t;

typedef struct CalibDb_AecBacklight_s {
    uint8_t             enable;
    uint8_t             Mode;
    float               lumaHighTh;
    float               lumaLowTh;
    float               LvTh;
    float               weightMinTh;
    float               weightMaxTh;

    float               OEROILowTh;
    float               LvLowTh;
    float               LvHightTh;
    Cam1x6FloatMatrix_t NonOEPdfTh;
    Cam1x6FloatMatrix_t LowLightPdfTh;
    Cam1x6FloatMatrix_t ExpLevel;
    Cam1x6FloatMatrix_t TargetLLLuma;
    float               LumaDistTh;
} CalibDb_AecBacklight_t;

typedef struct CalibDb_AecHist2Hal_s {
    uint8_t             enable;
    uint8_t             lowHistBinTh;
} CalibDb_AecHist2Hal_t;

typedef struct CalibDb_LinearAE_Attr_s {
    float                   SetPoint;                   /**< set point to hit by the ae control system */
    float                   NightSetPoint;
    float                   Tolerance;
    float                   Evbias;
    CalibDb_AeStrategyMode_t        StrategyMode;
    bool                    DySetPointEn;
    CalibDb_AecDynamicSetpoint_t DySetpoint[AEC_DNMODE_MAX];
    //AOE
    bool                    AOE_Enable;
    float                   AOE_Max_point;
    float                   AOE_Min_point;
    float                   AOE_Y_Max_th;
    float                   AOE_Y_Min_th;
    float                   AOE_Step_Inc;
    float                   AOE_Step_Dec;

    CalibDb_AecBacklight_t  BackLightConf;
    CalibDb_AecHist2Hal_t   Hist2Hal; //TODO
    CalibDb_AecInterAdjust_t InterAdjustStrategy;//TODO
} CalibDb_LinearAE_Attr_t;

typedef struct CalibDb_LFrameCtrl_s
{
    //LframeCtrl
    float                   OEROILowTh;
    float                   LvLowTh;
    float                   LvHighTh;
    Cam1x6FloatMatrix_t     LExpLevel;
    Cam1x6FloatMatrix_t     LSetPoint;
    Cam1x6FloatMatrix_t     TargetLLLuma;
    Cam1x6FloatMatrix_t     NonOEPdfTh;
    Cam1x6FloatMatrix_t     LowLightPdfTh;
} CalibDb_LFrameCtrl_t;

typedef struct CalibDb_MFrameCtrl_s
{
    //MframeCtrl
    Cam1x6FloatMatrix_t     MExpLevel;
    Cam1x6FloatMatrix_t     MSetPoint;
} CalibDb_MFrameCtrl_t;

typedef struct CalibDb_SFrameCtrl_s
{
    //SframeCtrl
    float                   HLLumaTolerance;
    Cam1x6FloatMatrix_t     SExpLevel;
    Cam1x6FloatMatrix_t     TargetHLLuma;
    Cam1x6FloatMatrix_t     SSetPoint;
} CalibDb_SFrameCtrl_t;

typedef struct CalibDb_HdrAE_Attr_s {
    float                           Tolerance;
    CalibDb_AeStrategyMode_t        StrategyMode;
    float                           Evbias;
    //ExpRatioCtrl
    CalibDb_HdrAeRatioType_t        ExpRatioType;
    Cam1x6FloatMatrix_t             RatioExpDot;
    Cam1x6FloatMatrix_t             M2SRatioFix;
    Cam1x6FloatMatrix_t             L2MRatioFix;
    Cam1x6FloatMatrix_t             M2SRatioMax;
    Cam1x6FloatMatrix_t             L2MRatioMax;

    float                           LumaDistTh; //used for area-growing
    CalibDb_LFrameCtrl_t            LframeCtrl;
    CalibDb_MFrameCtrl_t            MframeCtrl;
    CalibDb_SFrameCtrl_t            SframeCtrl;

} CalibDb_HdrAE_Attr_t;

/*****************************************************************************/
/**
 * @brief   Global AEC calibration structure of isp2.0
 */
/*****************************************************************************/
typedef struct CalibDb_Aec_Para_s {
    CalibDb_AecCommon_Attr_t      CommCtrl;
    CalibDb_LinearAE_Attr_t       LinearAeCtrl;
    CalibDb_HdrAE_Attr_t          HdrAeCtrl;
} CalibDb_Aec_Para_t;

typedef struct CalibDb_AecGainRange_s {
    int array_size;
    float *pGainRange;
} CalibDb_AecGainRange_t;

typedef struct CalibDb_Sensor_Para_s {
    //Exp RealValue 2 RegValue
    CalibDb_AecGainRange_t  GainRange;
    float                   TimeFactor[4];
    //ExpSeperateCtrl
    float                   DCG_Ratio;
    Cam2x1FloatMatrix_t     CISTimeRegSumFac;
    Cam2x1FloatMatrix_t     CISTimeRegOdevity;
    uint8_t                 CISTimeRegUnEqualEn;
    uint8_t                 CISTimeRegMin;
    CalibDb_AeRange_t       CISAgainRange;
    CalibDb_AeRange_t       CISExtraAgainRange; //add for HDR-DCG MODE, which Lgain & Sgain have different CISAgainRange respectively
    CalibDb_AeRange_t       CISDgainRange;
    CalibDb_AeRange_t       CISIspDgainRange;
    uint8_t                 EachGainSetEn;
    uint8_t                 EachTimeSetEn;
} CalibDb_Sensor_Para_t;


#define CALD_AWB_LS_NUM_MAX 7
#define CALD_AWB_WINDOW_NUM_MAX 8
#define CALD_AWB_EXCRANGE_NUM_MAX 7
#define CALD_AWB_RRES_NUM_MAX 5
#define CALD_AWB_LV_NUM_MAX 16
#define CALD_AWB_SGC_NUM_MAX 16
#define CALD_AWB_ILLUMINATION_NAME       ( 20U )
#define CALD_AWB_RES_NAME       ( 20U )
#define CALD_AWB_TEMPORAL_GAIN_SIZE_MAX 4
#define CALD_AWB_XY_TYPE_MAX_V201 2
#define CALD_AWB_GRID_NUM_TOTAL 225

typedef struct _CalibDb_ExcRange_s {
    unsigned char domain;/*0uv domain,1 xy domain*/
    unsigned char mode;
    int xu[2];//left right
    int yv[2];//top bottom
} CalibDb_ExcRange_t;

typedef struct _CalibDb_Rgb2TcsParam_s {
    float pseudoLuminanceWeight[3];
    float rotationMat[9];
} CalibDb_Rgb2TcsParam_t;

typedef struct CalibDb_Yuv3D_Range_Ill_s {
    int b_uv;           //17+0 s
    int slope_inv_neg_uv; //9+10s //-1/k
    int slope_factor_uv; //9+10s // 1/(-1/k+k)
    int slope_ydis;     //9+10s //k
    int b_ydis;         //17+0s
    unsigned char ref_u;          //8u
    unsigned char ref_v;            //8u
    unsigned short  dis[6];//8+4 u
    unsigned char th[6]; //8u
} CalibDb_Yuv3D_Range_Ill_t;

typedef struct CalibDb_Yuv3D_2_Range_Ill_s {
    float thcurve_u[6];
    float thcure_th[6];
    float line[6];
    //float lineP1[3];
    //float lineP2[3];
} CalibDb_Yuv3D_2_Range_Ill_t;

typedef struct CalibDb_UV_Range_Ill_s {
    float  pu_region[5];
    float  pv_region[5];
} CalibDb_UV_Range_Ill_t;

typedef struct CalibDb_Tcs_Range_Ill_s {
    float xleft;
    float xright;
    float ytop;
    float ybottom;
} CalibDb_Tcs_Range_Ill_t;

typedef struct CalibDb_Awb_Light_Info_s {
    char light_name[16];
    CalibDb_UV_Range_Ill_t uvRange_param;
    CalibDb_UV_Range_Ill_t uvRange_param_small;
    CalibDb_Tcs_Range_Ill_t normalRange;
    CalibDb_Tcs_Range_Ill_t bigRange;
    CalibDb_Tcs_Range_Ill_t smallRange;
    CalibDb_Yuv3D_Range_Ill_t   yuv3DRange_param;
    CalibDb_Yuv3D_2_Range_Ill_t yuv3D2Range_param;
} CalibDb_Awb_Light_Info_t;

typedef struct CalibDb_Awb_Light_Info2_s {
    char light_name[CALD_AWB_ILLUMINATION_NAME];
    unsigned char doorType;
    float standardGainValue[4];//rggb
    unsigned char staWeight[CALD_AWB_LV_NUM_MAX];
    unsigned int spatialGain_LV_THH;//threshold for  spatial gain calculation in different illuminant
    unsigned int spatialGain_LV_THL;//
    unsigned char xyType2Enable;
    float weightCurve_ratio[4];
    float weightCurve_weight[4];
} CalibDb_Awb_Light_Info2_t;

typedef struct CalibDb_StatWindow_s {
    int mode;
    int  resNum;
    char resName[CALD_AWB_RRES_NUM_MAX][CALD_AWB_RES_NAME];
    uint16_t window[CALD_AWB_RRES_NUM_MAX][4];
} CalibDb_StatWindow_t;
typedef struct CalibDb_tcs_range_ill_s {
    float NorrangeX[2];//left right
    float NorrangeY[2];//top bottom
    float SperangeX[2];
    float SperangeY[2];
    float SmalrangeX[2];
    float SmalrangeY[2];
} CalibDb_tcs_range_ill_t;

typedef struct CalibDb_Awb_Measure_Para_V200_s {

    unsigned char       hdrFrameChooseMode;
    unsigned char       hdrFrameChoose;
    bool                lscBypEnable;
    bool                uvDetectionEnable;
    bool                xyDetectionEnable;
    bool                yuvDetectionEnable;
    bool                wpDiffWeiEnable;
    bool                blkWeightEnable;//the different weight in WP sum
    bool                blkStatisticsEnable;

    int                 lsUsedForYuvDetNum;
    char                lsUsedForYuvDet[CALD_AWB_LS_NUM_MAX][CALD_AWB_ILLUMINATION_NAME];
    unsigned char       dsMode;
    unsigned char       blkMeasureMode;
    CalibDb_StatWindow_t measeureWindow;
    bool                multiwindow_en;
    unsigned char       lightNum;
    char                lightName[CALD_AWB_LS_NUM_MAX][CALD_AWB_ILLUMINATION_NAME];
    unsigned short      maxR;
    unsigned short      minR;
    unsigned short      maxG;
    unsigned short      minG;
    unsigned short      maxB;
    unsigned short      minB;
    unsigned short      maxY;
    unsigned short      minY;
    //uv_range_ill_t        uvRange_param[7];//big range
    //uv_range_ill_t        uvRange_param_small[7];//small range
    CalibDb_Rgb2TcsParam_t     rgb2tcs_param;
    //tcs_range_ill_t     xyRangeLight[7];//HZ A TL84 CWF D50 D65 D75
    //yuv3d_range_ill_t yuv3DRange_param[7];
    //yuv3d_2_range_ill_t yuv3D2Range_param[7];
    //several window in pixel domain
    //CalibDb_Awb_Light_Info_t    awb_light_info[CALD_AWB_LS_NUM_MAX];
    CalibDb_UV_Range_Ill_t uvRange_param[CALD_AWB_LS_NUM_MAX];
    CalibDb_UV_Range_Ill_t uvRange_param_small[CALD_AWB_LS_NUM_MAX];
    CalibDb_tcs_range_ill_t xyRangeLight[CALD_AWB_LS_NUM_MAX];
    CalibDb_Yuv3D_Range_Ill_t   yuv3DRange_param[CALD_AWB_LS_NUM_MAX];


    unsigned short multiwindow[CALD_AWB_WINDOW_NUM_MAX][4];//8  windows in pixel domain ,hOffset,vOffser,hSize,vSize;
    //several winow in uv or xy domain
    CalibDb_ExcRange_t excludeWpRange[CALD_AWB_EXCRANGE_NUM_MAX];
} CalibDb_Awb_Measure_Para_V200_t;

typedef struct CalibDb_Awb_Measure_Para_V201_s {

    bool                lscBypEnable;
    bool                uvDetectionEnable;
    bool                xyDetectionEnable;
    bool                yuvDetectionEnable;
    int                 lsUsedForYuvDetNum;
    char                lsUsedForYuvDet[CALD_AWB_LS_NUM_MAX][CALD_AWB_ILLUMINATION_NAME];
    bool                wpDiffWeiEnable;
    bool                blkWeightEnable;
    bool                blkStatisticsEnable;
    unsigned char       dsMode;
    unsigned char       blkMeasureMode;
    bool                multiwindow_en;
    unsigned char       hdrFrameChooseMode;
    unsigned char       hdrFrameChoose;
    CalibDb_StatWindow_t measeureWindow;
    unsigned char       lightNum;
    char                lightName[CALD_AWB_LS_NUM_MAX][CALD_AWB_ILLUMINATION_NAME];
    unsigned short      maxR;
    unsigned short      minR;
    unsigned short      maxG;
    unsigned short      minG;
    unsigned short      maxB;
    unsigned short      minB;
    unsigned short      maxY;
    unsigned short      minY;
    CalibDb_Rgb2TcsParam_t     rgb2tcs_param;
    float               rgb2RYuv_matrix[16];
    CalibDb_UV_Range_Ill_t uvRange_param[CALD_AWB_LS_NUM_MAX];
    CalibDb_UV_Range_Ill_t uvRange_param_small[CALD_AWB_LS_NUM_MAX];
    CalibDb_tcs_range_ill_t xyRangeLight[CALD_AWB_LS_NUM_MAX];
    CalibDb_Yuv3D_2_Range_Ill_t yuv3D2Range_param[CALD_AWB_LS_NUM_MAX];


    unsigned short multiwindow[CALD_AWB_WINDOW_NUM_MAX][4];//8  windows in pixel domain ,hOffset,vOffser,hSize,vSize;
    //several winow in uv or xy domain
    CalibDb_ExcRange_t excludeWpRange[CALD_AWB_EXCRANGE_NUM_MAX];
    //with differernt luma ,the different weight in WP sum
    float wpDiffNoTh;
    unsigned int   wpDiffLvValueTh;
    unsigned char wpDiffwei_y[9];
    unsigned char  perfectBin[8];// ture means the luma is appropriate
    float wpDiffweiSet_w_LvValueTh[2];
    float wpDiffWeiRatioTh[3];
    float wpDiffweiSet_w_HigLV[3][9];
    float wpDiffweiSet_w_LowLV[3][9];
    unsigned short blkWeight[CALD_AWB_GRID_NUM_TOTAL];
} CalibDb_Awb_Measure_Para_V201_t;

typedef struct CalibDb_Awb_line_s {
    float a;
    float b;
    float c;
} CalibDb_Awb_line_t;


typedef struct CalibDb_Awb_Stategy_Para_s {
    unsigned char lightNum;
    bool ca_enable;
    char lsForFirstFrame[CALD_AWB_ILLUMINATION_NAME];
    //multiwindow
    unsigned char multiwindowMode;
    bool uvRange_small_enable;

    //wbgain damp
    float dFStep;
    float dFMin;
    float dFMax;
    int LvIIRsize;
    float LvVarTh;

    int LV_NUM;
    unsigned int LVMatrix[CALD_AWB_LV_NUM_MAX];
    unsigned int LV_THL;
    unsigned int LV_THL2;
    unsigned int LV_THH;
    unsigned int LV_THH2;
    unsigned int WP_THH;
    unsigned int WP_THL;
    float proDis_THL; //threshold for distance probality calculation
    float proDis_THH;
    unsigned int proLV_Indoor_THL;// threshold for luminance probality calculation
    unsigned int proLV_Indoor_THH;
    unsigned int proLV_Outdoor_THL;
    unsigned int proLV_Outdoor_THH;
    float spatialGain_H[4];//spatial gain
    float spatialGain_L[4];
    float temporalDefaultGain[4];
    unsigned char temporalCalGainSetSize;
    unsigned char temporalGainSetWeight[CALD_AWB_TEMPORAL_GAIN_SIZE_MAX];//四帧的比例0-100总和为100//gainPer[0]为-1帧，gainPer[1]is -2,gainPer[2] is -3 gainPer[3] is -4
    float  wpNumPercTh;//无效白点阈值，白点数量少时认为该白点无效
    unsigned char tempWeight[CALD_AWB_LV_NUM_MAX];


    //color adaptation
    float ca_targetGain[4];
    float ca_LACalcFactor;

    //single color
    unsigned short      sSelColorNUM;
    unsigned short      sIndSelColor[CALD_AWB_SGC_NUM_MAX];
    float               sMeanCh[2][CALD_AWB_SGC_NUM_MAX];
    float               srGain[CALD_AWB_LS_NUM_MAX];
    float               sbGain[CALD_AWB_LS_NUM_MAX];
    unsigned short      sIllEstNum;
    char                sNameIllEst[CALD_AWB_LS_NUM_MAX][CALD_AWB_ILLUMINATION_NAME];
    float               sAlpha;


    float convergedVarTh;
    float lineRgBg[3];
    float lineRgProjCCT[3];

    //wbgain shift to do

    CalibDb_Awb_Light_Info2_t    awb_light_info[CALD_AWB_LS_NUM_MAX];

    bool xyType2ForColBalEnable;// to do for awb2.1
} CalibDb_Awb_Stategy_Para_t;


typedef struct CalibDb_Awb_Para_s
{
    bool                wbBypass;
    bool                awbEnable;
    CalibDb_Awb_Measure_Para_V200_t measure_para_v200;
    CalibDb_Awb_Measure_Para_V201_t measure_para_v201;
    CalibDb_Awb_Stategy_Para_t stategy_cfg;
} CalibDb_Awb_Para_t;

typedef struct CalibDb_HdrMerge_s
{
    float envLevel[6];
    float oeCurve_smooth[6];
    float oeCurve_offset[6];
    float moveCoef[6];
    float mdCurveLm_smooth[6];
    float mdCurveLm_offset[6];
    float mdCurveMs_smooth[6];
    float mdCurveMs_offset[6];
    float oeCurve_damp;
    float mdCurveLm_damp;
    float mdCurveMs_damp;
} CalibDb_HdrMerge_t;

typedef struct CalibDb_HdrTmo_s
{
    float envLevel[6];
    float EnvLvTolerance;
    float globalLuma[6];
    float OEPdf[6];
    float OETolerance;
    float detailsHighLight[6];
    float enFocusLuma;
    float FocusLuma[6];
    float DarkPdf[6];
    float DTPdfTolerance;
    float detailsLowLight[6];
    float DynamicRange[6];
    float DRTolerance;
    float DayTh;
    float TmoContrast[6];
    float damp;
} CalibDb_HdrTmo_t;

typedef struct CalibDb_Ahdr_Para_s {
    CalibDb_HdrMerge_t merge;
    CalibDb_HdrTmo_t tmo;
} CalibDb_Ahdr_Para_t;

typedef struct CalibDb_Blc_s {
    int enable;
    float level[4];
} CalibDb_Blc_t;

typedef struct CalibDb_Dpcc_set_s {
    unsigned char red_blue_enable[6][CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char green_enable[6][CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char line_thresh[4][CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char line_mad_fac[4][CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char pg_fac[2][CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char rnd_thresh[2][CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char rg_fac[2][CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char rg_lim[2][CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char rnd_offs[2][CALIBDB_DPCC_MAX_ISO_LEVEL];
} CalibDb_Dpcc_set_t;

typedef struct CalibDb_Dpcc_Pdaf_s {
    unsigned char en;
    unsigned char point_en[16];
    unsigned short int offsetx;
    unsigned short int offsety;
    unsigned char wrapx;
    unsigned char wrapy;
    unsigned short int wrapx_num;
    unsigned short int wrapy_num;
    unsigned char point_x[16];
    unsigned char point_y[16];
    unsigned char forward_med;
} CalibDb_Dpcc_Pdaf_t;

typedef struct CalibDb_Dpcc_s {
    int enable;
    char version[64];
    float iso[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char stage1_Enable[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char grayscale_mode[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char rk_out_sel[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char dpcc_out_sel[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char stage1_rb_3x3[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char stage1_g_3x3[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char stage1_inc_rb_center[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char stage1_inc_g_center[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char stage1_use_fix_set[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char stage1_use_set3[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char stage1_use_set2[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char stage1_use_set1[CALIBDB_DPCC_MAX_ISO_LEVEL];
    CalibDb_Dpcc_set_t set[3];
    CalibDb_Dpcc_Pdaf_t pdaf;
} CalibDb_Dpcc_t;

typedef struct CalibDb_BayerNr_s {
    int enable;
    char version[64];
    float iso[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float filtPara[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float luLevel[8];
    float luLevelVal[8];
    float luRatio[8][CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float fixW[4][CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float lamda;
    unsigned char gauss_en;
    float RGainOff;
    float RGainFilp;
    float BGainOff;
    float BGainFilp;
    float edgeSoftness;
    float gaussWeight0;
    float gaussWeight1;
    float bilEdgeFilter;
    float bilFilterStreng[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float bilEdgeSoft;
    float bilEdgeSoftRatio;
    float bilRegWgt;
} CalibDb_BayerNr_t;


#define CIFISP_LSC_DATA_TBL_SIZE           289
#define CIFISP_LSC_GRAD_TBL_SIZE           8
#define CIFISP_LSC_SIZE_TBL_SIZE           8
#define LSC_GRAD_TBL_SIZE                  8
#define LSC_ILLUMINATION_MAX               7
#define LSC_RESOLUTION_NAME         ( 15U )
typedef char                        CalibDb_ResolutionName_t[LSC_RESOLUTION_NAME];
#define LSC_PROFILE_NAME            ( 25U )
typedef char                        CalibDb_Lsc_ProfileName_t[LSC_PROFILE_NAME];
#define LSC_ILLUMINATION_NAME       ( 20U )
typedef char                        CalibDb_IlluminationName_t[LSC_ILLUMINATION_NAME];
#define LSC_PROFILES_NUM_MAX         ( 5 )
#define LSC_RESOLUTIONS_NUM_MAX      ( 4 )
typedef enum Cam4ChColorComponent_e
{
    CAM_4CH_COLOR_COMPONENT_RED     = 0,
    CAM_4CH_COLOR_COMPONENT_GREENR  = 1,
    CAM_4CH_COLOR_COMPONENT_GREENB  = 2,
    CAM_4CH_COLOR_COMPONENT_BLUE    = 3,
    CAM_4CH_COLOR_COMPONENT_MAX
} Cam4ChColorComponent_t;

typedef struct CamLscMatrix_s
{
    Cam17x17UShortMatrix_t  LscMatrix[CAM_4CH_COLOR_COMPONENT_MAX];
} CamLscMatrix_t;

typedef struct CamVignettingCurve_s {
    uint16_t    arraySize;
    float      pSensorGain[4];//change to pointer
    float      pVignetting[4];
} CamVignettingCurve_t;

typedef struct CalibDb_AlscCof_ill_s {
    char illuName[25];
    float awbGain[2];
    int tableUsedNO;
    CalibDb_Lsc_ProfileName_t tableUsed[LSC_PROFILES_NUM_MAX];
    CamVignettingCurve_t vignettingCurve;
} CalibDb_AlscCof_ill_t;

typedef struct CalibDb_AlscCof_s {
    int   lscResNum;
    CalibDb_ResolutionName_t  lscResName[LSC_RESOLUTIONS_NUM_MAX];// type CalibDb_ResolutionName_t
    int   illuNum;
    CalibDb_AlscCof_ill_t illAll[LSC_ILLUMINATION_MAX];
} CalibDb_AlscCof_t;

typedef struct CalibDb_LscTableProfile_s {
    CalibDb_Lsc_ProfileName_t     name;                                   /**< profile name */
    CalibDb_ResolutionName_t     resolution;                             /**< resolution link */
    CalibDb_IlluminationName_t   illumination;                           /**< illumination link */
    float                   vignetting;                             /**< vignetting value */

    uint16_t                LscSectors;
    uint16_t                LscNo;
    uint16_t                LscXo;
    uint16_t                LscYo;

    uint16_t                LscXGradTbl[LSC_GRAD_TBL_SIZE];
    uint16_t                LscYGradTbl[LSC_GRAD_TBL_SIZE];
    uint16_t                LscXSizeTbl[CIFISP_LSC_SIZE_TBL_SIZE];
    uint16_t                LscYSizeTbl[CIFISP_LSC_SIZE_TBL_SIZE];

    Cam17x17UShortMatrix_t  LscMatrix[CAM_4CH_COLOR_COMPONENT_MAX];     /**< matrix for different color channels */
} CalibDb_LscTableProfile_t;

typedef struct CalibDb_Lsc_s {
    bool enable;
    bool damp_enable;
    CalibDb_AlscCof_t aLscCof;
    int tableAllNum;
    CalibDb_LscTableProfile_t tableAll[LSC_RESOLUTIONS_NUM_MAX * LSC_ILLUMINATION_MAX * LSC_PROFILES_NUM_MAX]; //type  CalibDb_LscTableProfile_t;
} CalibDb_Lsc_t;


typedef struct CalibDb_RKDM_s {
    signed char debayer_filter1[5];
    signed char debayer_filter2[5];
    unsigned char debayer_gain_offset;
    int ISO[9];
    unsigned char sharp_strength[9];
    unsigned short debayer_hf_offset[9];
    unsigned char debayer_offset;
    unsigned char debayer_clip_en;
    unsigned char debayer_filter_g_en;
    unsigned char debayer_filter_c_en;
    unsigned char debayer_thed0;
    unsigned char debayer_thed1;
    unsigned char debayer_dist_scale;
    unsigned char debayer_cnr_strength;
    unsigned char debayer_shift_num;
} CalibDb_RKDM_t;

#define CCM_ILLUMINATION_MAX               7
#define CCM_PROFILE_NAME            ( 25U )
typedef char                        CalibDb_Ccm_ProfileName_t[CCM_PROFILE_NAME];
#define CCM_ILLUMINATION_NAME       ( 20U )
typedef char                        CalibDb_IlluminationName_t[CCM_ILLUMINATION_NAME];
#define CCM_PROFILES_NUM_MAX         ( 5 )
#define CCM_RESOLUTIONS_NUM_MAX      ( 4 )

typedef enum Cam3ChColorComponent_e
{
    CAM_3CH_COLOR_COMPONENT_RED     = 0,
    CAM_3CH_COLOR_COMPONENT_GREEN   = 1,
    CAM_3CH_COLOR_COMPONENT_BLUE    = 2,
    CAM_3CH_COLOR_COMPONENT_MAX
} Cam3ChColorComponent_t;

typedef struct CamSaturationCurve_s {
    uint16_t    arraySize;
    float      pSensorGain[4];//change to pointer
    float      pSaturation[4];
} CamSaturationCurve_t;

typedef struct CalibDb_AccmCof_ill_s {
    char illuName[25];
    float awbGain[2];
    int matrixUsedNO;
    CalibDb_Ccm_ProfileName_t matrixUsed[CCM_PROFILES_NUM_MAX];
    CamSaturationCurve_t saturationCurve;
} CalibDb_AccmCof_ill_t;

typedef struct CalibDb_AccmCof_s {
    int   illuNum;
    CalibDb_AccmCof_ill_t illAll[CCM_ILLUMINATION_MAX];
} CalibDb_AccmCof_t;

typedef struct CalibDb_CcmMatrixProfile_s
{
    CalibDb_Ccm_ProfileName_t  name;                   /**< profile name */
    CalibDb_IlluminationName_t illumination;
    float                   saturation;             /**< saturation value */
    Cam3x3FloatMatrix_t     CrossTalkCoeff;         /**< CrossTalk matrix coefficients */
    Cam1x3FloatMatrix_t     CrossTalkOffset;        /**< CrossTalk offsets */
} CalibDb_CcmMatrixProfile_t;

typedef struct CalibDb_LUMA_CCM_s {
    float rgb2y_para[3];
    float low_bound_pos_bit;
    float y_alpha_curve[17];
    int gain_scale_cure_size;
    float alpha_gain[9];//change to pointer
    float alpha_scale[9];
} CalibDb_LUMA_CCM_t;

typedef struct CalibDb_Ccm_s {
    bool enable;
    bool  damp_enable;
    CalibDb_AccmCof_t aCcmCof;
    CalibDb_LUMA_CCM_t   luma_ccm;
    int matrixAllNum;
    CalibDb_CcmMatrixProfile_t matrixAll[CCM_RESOLUTIONS_NUM_MAX * CCM_ILLUMINATION_MAX * CCM_PROFILES_NUM_MAX]; //type  CalibDb_CcmMatrixProfile_t;
} CalibDb_Ccm_t;

typedef struct CalibDb_UVNR_s {
    int enable;
    char version[64];
    float ISO[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step0_uvgrad_ratio[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step0_uvgrad_offset[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step1_nonMed1[4];
    float step1_nonBf1[4];
    float step1_downSample_w[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step1_downSample_h[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step1_downSample_meansize[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step1_median_ratio[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step1_median_size[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step1_median_IIR[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step1_bf_sigmaR[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step1_bf_uvgain[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step1_bf_ratio[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step1_bf_size[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step1_bf_sigmaD[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step1_bf_isRowIIR[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step1_bf_isYcopy[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step2_nonExt_block[4];
    float step2_nonMed[4];
    float step2_nonBf[4];
    float step2_downSample_w[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step2_downSample_h[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step2_downSample_meansize[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step2_median_ratio[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step2_median_size[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step2_median_IIR[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step2_bf_sigmaR[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step2_bf_uvgain[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step2_bf_ratio[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step2_bf_size[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step2_bf_sigmaD[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step2_bf_isRowIIR[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step2_bf_isYcopy[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step3_nonBf3[4];
    float step3_bf_sigmaR[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step3_bf_uvgain[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step3_bf_ratio[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step3_bf_size[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step3_bf_sigmaD[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step3_bf_isRowIIR[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step3_bf_isYcopy[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float kernel_3x3[3];
    float kernel_5x5[5];
    float kernel_9x9[8];
    float kernel_9x9_num;
    float sigma_adj_luma[9];
    float sigma_adj_ratio[9];
    float threshold_adj_luma[9];
    float threshold_adj_thre[9];
} CalibDb_UVNR_t;

typedef struct CalibDb_Gamma_s {
    unsigned char gamma_en;
    unsigned char gamma_out_mode;
    unsigned char gamma_out_segnum;
    unsigned char gamma_out_offset;
    float curve_normal[45];
    float curve_hdr[45];
    float curve_night[45];
    float curve_user1[45];
    float curve_user2[45];
} CalibDb_Gamma_t;

typedef struct CalibDb_YNR_ISO_s {
    float iso;
    double sigma_curve[5];
    float ynr_lci[4];
    float ynr_lhci[4];
    float ynr_hlci[4];
    float ynr_hhci[4];
    float lo_lumaPoint[6];
    float lo_lumaRatio[6];
    float lo_directionStrength;
    float lo_bfScale[4];
    float imerge_ratio;
    float imerge_bound;
    float denoise_weight[4];
    float hi_lumaPoint[6];
    float hi_lumaRatio[6];
    float hi_bfScale[4];
    float hwith_d[4];
    float hi_denoiseStrength;
    float hi_detailMinAdjDnW;
    float hi_denoiseWeight[4];
    float y_luma_point[6];
    float hgrad_y_level1[6];
    float hgrad_y_level2[6];
    float hgrad_y_level3[6];
    float hgrad_y_level4[6];
    float hi_soft_thresh_scale[4];
} CalibDb_YNR_ISO_t;

typedef struct CalibDb_YNR_s {
    int enable;
    char version[64];
    CalibDb_YNR_ISO_t ynr_iso[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
} CalibDb_YNR_t;

struct CalibDb_Gic_ISO_s {
    float iso;
    float min_busy_thre;
    float min_grad_thr1;
    float min_grad_thr2 ;
    float k_grad1;
    float k_grad2 ;
    float smoothness_gb;
    float smoothness_gb_weak;
    float gb_thre ;
    float maxCorV ;
    float maxCorVboth;
    float maxCutV ;
    float dark_thre;
    float dark_threHi;
    float k_grad1_dark;
    float k_grad2_dark ;
    float min_grad_thr_dark1;
    float min_grad_thr_dark2 ;
    float GValueLimitLo ;
    float GValueLimitHi;
    float textureStrength ;
    float ScaleLo ;
    float ScaleHi ;
    float noiseCurve_0 ;
    float noiseCurve_1 ;
    float globalStrength;
    float noise_coea ;
    float noise_coeb;
    float diff_clip;
};

typedef struct CalibDb_Gic_s {
    unsigned char edge_en;
    unsigned char gr_ration;
    unsigned char noise_cut_en;
    struct CalibDb_Gic_ISO_s gic_iso[9];
} CalibDb_Gic_t;

struct CalibDb_awb_uv_ratio_s {
    char illum[9];
    float ratio[2];
};

struct CalibDb_MFNR_ISO_s {
    float iso;
    float weight_limit_y[4];
    float weight_limit_uv[3];
    float ratio_frq[4];
    float luma_w_in_chroma[3];
    double noise_curve[5];
    double noise_curve_x00;
    float y_lo_noiseprofile[4];
    float y_hi_noiseprofile[4];
    float y_lo_denoiseweight[4];
    float y_hi_denoiseweight[4];
    float y_lo_bfscale[4];
    float y_hi_bfscale[4];
    float y_lumanrpoint[6];
    float y_lumanrcurve[6];
    float y_denoisestrength;
    float y_lo_lvl0_gfdelta[6];
    float y_hi_lvl0_gfdelta[6];
    float y_lo_lvl1_gfdelta[3];
    float y_hi_lvl1_gfdelta[3];
    float y_lo_lvl2_gfdelta[3];
    float y_hi_lvl2_gfdelta[3];
    float y_lo_lvl3_gfdelta[3];
    float y_hi_lvl3_gfdelta[3];
    float uv_lo_noiseprofile[3];
    float uv_hi_noiseprofile[3];
    float uv_lo_denoiseweight[3];
    float uv_hi_denoiseweight[3];
    float uv_lo_bfscale[3];
    float uv_hi_bfscale[3];
    float uv_lumanrpoint[6];
    float uv_lumanrcurve[6];
    float uv_denoisestrength;
    float uv_lo_lvl0_gfdelta[6];
    float uv_hi_lvl0_gfdelta[6];
    float uv_lo_lvl1_gfdelta[3];
    float uv_hi_lvl1_gfdelta[3];
    float uv_lo_lvl2_gfdelta[3];
    float uv_hi_lvl2_gfdelta[3];
    float lvl0_gfsigma[6];
    float lvl1_gfsigma[3];
    float lvl2_gfsigma[3];
    float lvl3_gfsigma[3];
};

typedef struct CalibDb_MFNR_s {
    int enable;
    char version[64];
    unsigned char max_level;
    unsigned char max_level_uv;
    unsigned char back_ref_num;
    struct CalibDb_awb_uv_ratio_s uv_ratio[4];
    struct CalibDb_MFNR_ISO_s mfnr_iso[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
} CalibDb_MFNR_t;

struct CalibDb_Sharp_ISO_s {
    float iso;
    float hratio;
    float lratio;
    float mf_sharp_ratio;
    float hf_sharp_ratio;
    float luma_sigma[8];
    float pbf_gain;
    float pbf_ratio;
    float pbf_add;
    float mf_clip_pos[8];
    float mf_clip_neg[8];
    float hf_clip[8];
    float mbf_gain;
    float hbf_gain;
    float hbf_ratio;
    float mbf_add;
    float hbf_add;
    float local_sharp_strength;
};

typedef struct CalibDb_Sharp_s {
    int enable;
    char version[64];
    float luma_point[8];
    struct CalibDb_Sharp_ISO_s sharp_iso[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
} CalibDb_Sharp_t;

struct CalibDb_EdgeFilter_ISO_s {
    float iso;
    float edge_thed;
    float src_wgt;
    unsigned char alpha_adp_en;
    float local_alpha;
    float global_alpha;
    float noise_clip[8];
    float dog_clip_pos[8];
    float dog_clip_neg[8];
    float dog_alpha[8];
    float direct_filter_coeff[5];
    float dog_kernel_row0[5];
    float dog_kernel_row1[5];
    float dog_kernel_row2[5];
    float dog_kernel_row3[5];
    float dog_kernel_row4[5];
};

typedef struct CalibDb_EdgeFilter_s {
    int enable;
    char version[64];
    float luma_point[8];
    struct CalibDb_EdgeFilter_ISO_s edgeFilter_iso[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
} CalibDb_EdgeFilter_t;

struct CalibDb_Dehaze_ISO_s {
    float iso[9];
    unsigned char dehaze_en[9];
    unsigned char  dc_en[9];
    unsigned char  hist_en[9];
    unsigned char  hist_channel[9];
    unsigned char  gain_en[9];
    float dc_min_th[9];
    float dc_max_th[9];
    float yhist_th[9];
    float yblk_th[9];
    float dark_th[9];
    float bright_min[9];
    float bright_max[9];
    float wt_max[9];
    float air_min[9];
    float air_max[9];
    float tmax_base[9];
    float tmax_off[9];
    float tmax_max[9];
    float hist_gratio[9];
    float hist_th_off[9];
    float hist_k[9];
    float hist_min[9];
    unsigned char  enhance_en[9];
    float enhance_value[9];
    unsigned char  hist_para_en[9];
    float hist_scale[9];
    float stab_fnum[9];
    float sigma[9];
    float wt_sigma[9];
    float air_sigma[9];
    float tmax_sigma[9];
    float cfg_alpha[9];
    float cfg_wt[9];
    float cfg_air[9];
    float cfg_tmax[9];
    float cfg_gratio[9];
    float dc_thed[9];
    float dc_weitcur[9];
    float air_thed[9];
    float air_weitcur[9];
};

typedef struct CalibDb_Dehaze_s {
    float dc_bf_h[25];
    float air_bf_h[9];
    float gaus_h[9];
    float hist_conv_t0[6];
    float hist_conv_t1[6];
    float hist_conv_t2[6];
    struct CalibDb_Dehaze_ISO_s dehaze_iso;
} CalibDb_Dehaze_t;

typedef enum CalibDb_Af_SearchDir_s {
    CAM_AFM_POSITIVE_SEARCH     = 0,
    CAM_AFM_NEGATIVE_SEARCH     = 1,
    CAM_AFM_ADAPTIVE_SEARCH     = 2
} CalibDb_Af_SearchDir_t;

typedef enum CalibDb_Af_SS_s {
    CAM_AFM_FSS_INVALID         = 0,
    CAM_AFM_FSS_FULLRANGE       = 1,    /**< scan the full focus range to find the point of best focus */
    CAM_AFM_FSS_HILLCLIMBING    = 2,    /**< use hillclimbing search */
    CAM_AFM_FSS_ADAPTIVE_RANGE  = 3,    /**< similar to full range search, but with multiple subsequent scans
                                         with decreasing range and step size will be performed. */
    CAM_AFM_FSS_MAX
} CalibDb_Af_SS_t;

typedef struct CalibDb_Af_Contrast_s {
    unsigned char           enable;
    CalibDb_Af_SS_t         Afss;                         /**< enumeration type for search strategy */
    CalibDb_Af_SearchDir_t  FullDir;
    unsigned char           FullSteps;
    unsigned short*         FullRangeTbl;                 /**< full range search table*/
    CalibDb_Af_SearchDir_t  AdaptiveDir;
    unsigned char           AdaptiveSteps;
    unsigned short*         AdaptRangeTbl;                /**< adaptive range search table*/
    float                   TrigThers;                    /**< AF trigger threshold */

    float                   StableThers;                  /**< AF stable threshold */
    unsigned short          StableFrames;                 /**< AF stable  status must hold frames */
    unsigned short          StableTime;                   /**< AF stable status must hold time */

    float                   OutFocusValue;                /**< out of focus vlaue*/
    unsigned short          OutFocusPos;                  /**< out of focus position*/

    unsigned short          gammaY[17];
    unsigned char           gaussWeight[3];
} CalibDb_Af_Contrast_t;

typedef struct CalibDb_Af_Laser_s {
    unsigned char enable;
    float vcmDot[7];
    float distanceDot[7];
} CalibDb_Af_Laser_t;

typedef struct CalibDb_Af_Pdaf_s {
    unsigned char enable;
} CalibDb_Af_Pdaf_t;

typedef struct CalibDb_Af_DefCode_s {
    unsigned char code;
} CalibDb_Af_DefCode_t;

typedef struct CalibDb_AF_s {
    unsigned short win_h_offs;
    unsigned short win_v_offs;
    unsigned short win_h_size;
    unsigned short win_v_size;
    CalibDb_Af_DefCode_t fixed_mode;
    CalibDb_Af_DefCode_t macro_mode;
    CalibDb_Af_DefCode_t infinity_mode;
    CalibDb_Af_Contrast_t contrast_af;
    CalibDb_Af_Laser_t laser_af;
    CalibDb_Af_Pdaf_t pdaf;
} CalibDb_AF_t;

typedef struct CalibDb_ORB_s {
    int orb_en;
} CalibDb_ORB_t;

typedef struct CalibDb_FEC_s {
    unsigned int fec_en;
    unsigned int mesh_density; //0:16x8 1:32x16
    unsigned int yuv_format; //0:YUV420 1:YUV422
    unsigned int fbce_mode; //0:normal 1:fbec
    unsigned int fec_mesh_h_size;
    unsigned int fec_mesh_v_size;
    unsigned short* meshxi;
    unsigned char* meshxf;
    unsigned short* meshyi;
    unsigned char* meshyf;
} CalibDb_FEC_t;

typedef struct CalibDb_LDCH_s {
    unsigned int ldch_en;
    unsigned int lut_h_size;
    unsigned int lut_v_size;
    unsigned int lut_mapxy_size;
    unsigned short* lut_mapxy;
} CalibDb_LDCH_t;
typedef struct {
    bool enable;
    unsigned short look_up_table_r[729];
    unsigned short look_up_table_g[729];
    unsigned short look_up_table_b[729];
} CalibDb_Lut3d_t;

typedef struct {
#define HDR_MODE_2_FRAME_STR        "MODE_2_FRAME"
#define HDR_MODE_2_LINE_STR         "MODE_2_LINE"
#define HDR_MODE_3_FRAME_STR        "MODE_3_FRAME"
#define HDR_MODE_3_LINE_STR         "MODE_3_LINE"
#define HDR_LINE_MODE_DCG_STR       "DCG"
#define HDR_LINE_MODE_STAGGER_STR   "STAGGER"

    unsigned char hdr_en;
    rk_aiq_isp_hdr_mode_t hdr_mode;
    rk_aiq_sensor_hdr_line_mode_t line_mode;
} CalibDb_System_t;

typedef struct CamCalibDbContext_s {
    CalibDb_Awb_Para_t awb;
    CalibDb_Lut3d_t lut3d;
    CalibDb_Aec_Para_t aec;
    CalibDb_AF_t af;
    CalibDb_Ahdr_Para_t ahdr;
    CalibDb_Blc_t blc;
    CalibDb_Dpcc_t dpcc;
    CalibDb_BayerNr_t bayerNr;
    CalibDb_Lsc_t lsc;
    CalibDb_RKDM_t dm;
    CalibDb_Ccm_t ccm;
    CalibDb_UVNR_t uvnr;
    CalibDb_Gamma_t gamma;
    CalibDb_YNR_t ynr;
    CalibDb_Gic_t gic;
    CalibDb_MFNR_t mfnr;
    CalibDb_Sharp_t sharp;
    CalibDb_EdgeFilter_t edgeFilter;
    CalibDb_Dehaze_t dehaze;
    CalibDb_FEC_t afec;
    CalibDb_LDCH_t aldch;
    CalibDb_ORB_t orb;
    CalibDb_Sensor_Para_t sensor;
    CalibDb_System_t  sysContrl;
} CamCalibDbContext_t;

#endif
