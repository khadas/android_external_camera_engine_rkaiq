#ifndef _RK_AIQ_UAPI_AE_INT_TYPES_H_
#define _RK_AIQ_UAPI_AE_INT_TYPES_H_
#include "rk_aiq_algo_des.h"
#include "rk_aiq_types_ae_algo_int.h"


/*****************************************************************************/
/**
 * @brief   ISP2.0 AEC API ExpSwAttr_t Params
 */
/*****************************************************************************/
typedef CalibDb_LinAeRoute_Attr_t Uapi_LinAeRouteAttr_t;

typedef CalibDb_HdrAeRoute_Attr_t Uapi_HdrAeRouteAttr_t;

typedef CalibDb_AeRoute_Attr_t Uapi_AeRouteAttr_t;

typedef CalibDb_AeSpeed_t Uapi_AeSpeed_t;

typedef CalibDb_AeRange_t Uapi_AeRange_t;

typedef CalibDb_LinAeRange_t Uapi_LinAeRange_t;

typedef CalibDb_HdrAeRange_t Uapi_HdrAeRange_t;

typedef CalibDb_AeFrmRateAttr_t Uapi_AeFpsAttr_t;

typedef CalibDb_LinExpInitExp_t Uapi_LinExpInitExp_t;

typedef CalibDb_HdrExpInitExp_t Uapi_HdrExpInitExp_t;

typedef CalibDb_ExpInitExp_t Uapi_ExpInitExp_t;

typedef CalibDb_AntiFlickerAttr_t Uapi_AntiFlicker_t;

typedef CalibDb_AeAttr_t Uapi_AeAttr_t;

typedef CalibDb_LinMeAttr_t Uapi_LinMeAttr_t;

typedef CalibDb_HdrMeAttr_t Uapi_HdrMeAttr_t;

typedef CalibDb_MeAttr_t Uapi_MeAttr_t;

typedef struct Uapi_ExpSwAttr_s {
    bool                     Bypass;
    int                      RawStatsMode;
    int                      HistStatsMode;
    int                      YRangeMode;
    uint8_t                  AecRunInterval;
    int                      AecOpType;
    //GridWeight
    Cam5x5UCharMatrix_t      DayGridWeights;
    Cam5x5UCharMatrix_t      NightGridWeights;

    //DayOrNight Switch
    uint8_t                  DNTrigger;
    CalibDb_AecDayNightMode_t   DNMode;
    uint8_t                  FillLightMode;

    Uapi_AntiFlicker_t       stAntiFlicker;
    Uapi_AeAttr_t            stAuto;
    Uapi_MeAttr_t            stManual;
    Uapi_ExpInitExp_t        stInitExp;
} Uapi_ExpSwAttr_t;

/*****************************************************************************/
/**
 * @brief   ISP2.0 AEC API LinExpAttr/HdrExpAttr Params
 */
/*****************************************************************************/
typedef CalibDb_AecDynamicSetpoint_t Uapi_AeDySetpoint_t;

typedef struct Uapi_LinExpAttr_s {
    float                   SetPoint;                   /**< set point to hit by the ae control system */
    float                   NightSetPoint;
    float                   EvBias;
    float                   Tolerance;
    int                     StrategyMode;
    bool                    DySetPointEn;
    Uapi_AeDySetpoint_t     DySetpoint[AEC_DNMODE_MAX];
} Uapi_LinExpAttr_t;

typedef struct Uapi_HdrExpAttr_s {
    float                   Tolerance;
    int                     StrategyMode;
    float                   Evbias;
    //ExpRatioCtrl
    int                     ExpRatioType;
    Cam1x6FloatMatrix_t     RatioExpDot;
    Cam1x6FloatMatrix_t     M2SRatioFix;
    Cam1x6FloatMatrix_t     L2MRatioFix;
    Cam1x6FloatMatrix_t     M2SRatioMax;
    Cam1x6FloatMatrix_t     L2MRatioMax;
} Uapi_HdrExpAttr_t;

/*****************************************************************************/
/**
 * @brief   ISP2.0 AEC API ExpHwAttr Params
 */
/*****************************************************************************/
typedef struct window Aec_Win_t;
typedef struct Uapi_ExpHwAttr_s {
#if 0
    Aec_Win_t AeBig1;
    Aec_Win_t AeBig2;
    Aec_Win_t AeBig3;
    Aec_Win_t AeLite;
    Aec_Win_t YuvAe;
    Aec_Win_t HistBig1;
    Aec_Win_t HistBig2;
    Aec_Win_t HistBig3;
    Aec_Win_t HistLite;
#else
    Aec_Win_t Chn_wnd[3];
    Aec_Win_t Extra_wnd;
    Aec_Win_t YuvAe_wnd;
#endif
} Uapi_ExpHwAttr_t;

/*****************************************************************************/
/**
 * @brief   ISP2.0 AEC API ExpQueryInfo Params
 */
/*****************************************************************************/
typedef struct Uapi_ExpQueryInfo_s {

    bool              IsConverged;
    bool              IsExpMax;
    float             LumaDeviation;
    float             HdrLumaDeviation[3];

    float             MeanLuma;
    float             HdrMeanLuma[3];

    RKAiqAecExpInfo_t CurExpInfo;
    unsigned short    Piris;
    float             LinePeriodsPerField;
    float             PixelPeriodsPerLine;
    float             PixelClockFreqMHZ;

} Uapi_ExpQueryInfo_t;


#endif
