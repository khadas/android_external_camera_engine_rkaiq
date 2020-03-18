/*
 *  Copyright (c) 2019 Rockchip Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef _RK_AIQ_TYPES_H_
#define _RK_AIQ_TYPES_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "rk_aiq_comm.h"
#include "anr/anr.h"
#include "asharp/asharp.h"
#include "adpcc/adpcc.h"
#include "adebayer/rk_aiq_types_algo_adebayer.h"
#include "ablc/ablc.h"
#include "ae/rk_aiq_types_ae_algo.h"
#include "awb/rk_aiq_types_awb_algo.h"
#include "alsc/rk_aiq_types_alsc_algo.h"
#include "accm/rk_aiq_types_accm_algo.h"
#include "a3dlut/rk_aiq_types_a3dlut_algo.h"
#include "ahdr/rk_aiq_types_ahdr_algo_int.h"
#include "ahdr/rk_aiq_types_ahdr_algo.h"


#ifdef RK_SIMULATOR_HW
#include "simulator/isp20_hw_simulator.h"
#include "af/rk_aiq_af_hw_v200.h"
#else
#include "af/rk_aiq_types_af_algo.h"
#include "aorb/rk_aiq_orb.h"
#include "adehaze/dehaze.h"
#include "agamma/agamma.h"
#endif

typedef enum {
    RK_AIQ_WORKING_MODE_NORMAL,
    RK_AIQ_WORKING_MODE_ISP_HDR    = 1,
    RK_AIQ_WORKING_MODE_SENSOR_HDR = 10, // sensor built-in hdr mode
} rk_aiq_working_mode_t;

typedef enum {
    RK_AIQ_ISP_HDR_MODE_3_FRAME_HDR = RK_AIQ_WORKING_MODE_ISP_HDR + 1,
    RK_AIQ_ISP_HDR_MODE_2_FRAME_HDR,
    RK_AIQ_ISP_HDR_MODE_3_LINE_HDR,
    RK_AIQ_ISP_HDR_MODE_2_LINE_HDR,
} rk_aiq_isp_hdr_mode_t;

typedef enum {
    RK_AIQ_SENSOR_HDR_LINE_MODE_DCG,
    RK_AIQ_SENSOR_HDR_LINE_MODE_STAGGER,
} rk_aiq_sensor_hdr_line_mode_t;

// static cap infos
typedef struct {

} rk_aiq_sensor_info_t;

typedef struct {

} rk_aiq_lens_info_t;

typedef struct {
    rk_aiq_sensor_info_t    sensor_info;
    rk_aiq_lens_info_t      lens_info;
    // supported Antibanding modes
    // supported lock modes
    // supported ae compensation range/step
    // supported ae measure mode
    // supported af modes
    // other from iq
} rk_aiq_static_info_t;

// sensor
typedef struct {
    unsigned short line_periods_vertical_blanking;
    unsigned short fine_integration_time_min;
    unsigned short fine_integration_time_max_margin;
    unsigned short coarse_integration_time_min;
    unsigned short coarse_integration_time_max_margin;
    unsigned int frame_length_lines;
    unsigned int line_length_pck;
    unsigned int vt_pix_clk_freq_hz;
    float pixel_clock_freq_mhz;
    unsigned short pixel_periods_per_line;
    unsigned short line_periods_per_field;
    uint32_t sensor_output_width;
    uint32_t sensor_output_height;
    uint32_t sensor_pixelformat;
} rk_aiq_exposure_sensor_descriptor;

// exposure
typedef RkAiqExpParam_t rk_aiq_exposure_real_params;
typedef RkAiqExpSensorParam_t rk_aiq_exposure_sensor_params;
typedef RkAiqExpParamComb_t rk_aiq_exposure_params_comb_t;
typedef RKAiqAecExpInfo_t rk_aiq_exposure_params_t;

// focus
typedef struct
{
    unsigned int next_lens_pos;
} rk_aiq_focus_params_t;

// isp

struct rk_aiq_isp_window {
    uint16_t h_offs;
    uint16_t v_offs;
    uint16_t h_size;
    uint16_t v_size;
} __attribute__ ((packed));


typedef RKAiqAecStats_t rk_aiq_isp_aec_stats_t;


#ifdef RK_SIMULATOR_HW
typedef rawaf_isp_af_stat_t rk_aiq_isp_af_stats_t;
typedef rawaf_isp_af_meas_t rk_aiq_isp_af_meas_t;
typedef rawaf_focus_pos_meas_t rk_aiq_af_focus_pos_meas_t;
#else
typedef rk_aiq_af_algo_stat_t rk_aiq_isp_af_stats_t;
typedef rk_aiq_af_algo_meas_t rk_aiq_isp_af_meas_t;
typedef rk_aiq_af_algo_focus_pos_t rk_aiq_af_focus_pos_meas_t;
#endif

typedef rk_aiq_ae_meas_params_t rk_aiq_isp_aec_meas_t;
typedef rk_aiq_hist_meas_params_t rk_aiq_isp_hist_meas_t;

/**wb gain **/

#ifdef RK_SIMULATOR_HW
typedef orb_stat_t rk_aiq_isp_orb_stats_t;
#else
typedef orb_stat_t rk_aiq_isp_orb_stats_t;
/*
typedef struct {
    unsigned int num_points;
    unsigned short pXs[MAX_POINTS];
    unsigned short pYs[MAX_POINTS];
    unsigned char pDescriptors[MAX_POINTS * DESCRIPTOR_SIZE];
    unsigned char pScores;
} rk_aiq_isp_orb_stats_t;*/
#endif
typedef struct {
    int orb_en;
    int limit_value;
    int max_feature;
} rk_aiq_isp_orb_t;

#if 0
typedef struct {

} rk_aiq_isp_blc_t;
#endif

#if 0
typedef struct {

} rk_aiq_isp_dpcc_t;
#endif

typedef struct {

} rk_aiq_isp_drc_t;

typedef struct {

} rk_aiq_isp_gic_t;

/*typedef struct {

} rk_aiq_isp_lsc_t;*/

/*
typedef struct {
    unsigned char filter_c_en;
    unsigned char filter_g_en;

    unsigned char gain_offset;
    unsigned short hf_offset;
    unsigned char thed1;
    unsigned char thed0;
    unsigned char dist_scale;
    unsigned char max_ratio;
    unsigned char clip_en;

    signed char filter1_coe[5];
    signed char filter2_coe[5];

    unsigned char offset;
    unsigned char shift_num;
    unsigned char order_max;
    unsigned char order_min;
} rk_aiq_isp_demosaic_t;
*/
typedef AdebayerConfig_t rk_aiq_isp_demosaic_t;

/*
typedef struct {

    int enhance_en;
    int hist_chn;
    int hpara_en;
    int hist_en;
    int dc_en;
    int big_en;
    int nobig_en;

    unsigned char yblk_th;
    unsigned charyhist_th;
    unsigned char dc_max_th;
    unsigned char dc_min_th;

    unsigned char wt_max;
    unsigned char bright_max;
    unsigned char bright_min;

    unsigned char tmax_base;
    unsigned char chardark_th;
    unsigned char air_max;
    unsigned char air_min;

    unsigned short tmax_max;
    unsigned short tmax_off;

    unsigned char hist_th_off;
    unsigned char hist_gratio;

    unsigned short hist_min;
    unsigned short hist_k;

    unsigned short enhance_value;
    unsigned short hist_scale;

    unsigned short iir_wt_sigma;
    unsigned short iir_sigma;
    unsigned short stab_fnum;

    unsigned short iir_tmax_sigma;
    unsigned short iir_air_sigma;

    unsigned short cfg_wt;
    unsigned short cfg_air;
    unsigned short cfg_alpha;

    unsigned short cfg_gratio;
    unsigned short cfg_tmax;

    unsigned short dc_weitcur;
    unsigned short dc_thed;

    unsigned short sw_dhaz_dc_bf_h3;
    unsigned short sw_dhaz_dc_bf_h2;
    unsigned short sw_dhaz_dc_bf_h1;
    unsigned short sw_dhaz_dc_bf_h0;

    unsigned short sw_dhaz_dc_bf_h5;
    unsigned short sw_dhaz_dc_bf_h4;

    int air_weitcur;
    int air_thed;

    unsigned short air_bf_h2;
    unsigned short air_bf_h1;
    unsigned short air_bf_h0;

    unsigned short gaus_h2;
    unsigned short gaus_h1;
    unsigned short gaus_h0;

    unsigned short conv_t0[6];
    unsigned short conv_t1[6];
    unsigned short conv_t2[6];

    int dhaz_adp_air_base;
    int dhaz_adp_wt;

    int dhaz_adp_gratio;
    int dhaz_adp_tmax;

    int h_r_iir[64];
    int h_g_iir[64];
    int h_b_iir[64];


} rk_aiq_isp_dehaze_t;*/




typedef RKAiqAdhazHwConfig_t rk_aiq_isp_dehaze_t;

typedef RKAiqAgammaHwConfig_t rk_aiq_isp_agamma_t;




typedef struct {


    unsigned char  equ_segm;
    unsigned short offset;
    unsigned short gamma_y[45];



} rk_aiq_isp_goc_t;

typedef struct {

} rk_aiq_isp_wdr_t;

typedef struct {

} rk_aiq_isp_csm_t;

typedef struct {

} rk_aiq_isp_conv422_t;

typedef struct {

} rk_aiq_isp_yuvconv_t;

typedef struct {
    int ldch_en;
    int lut_h_size;
    int lut_v_size;
} rk_aiq_isp_ldch_t;

typedef struct {
    int fec_en;
    int pic_width;
    int pic_height;
    int sw_rd_vir_stride;
    int sw_wr_yuv_format;
    int sw_wr_vir_stride;
    unsigned short sw_mesh_xi[512];
    unsigned char sw_mesh_xf[512];
    unsigned short sw_mesh_yi[1024];
    unsigned char sw_mesh_yf[1024];
} rk_aiq_isp_fec_t;

#if 0
typedef struct {

} rk_aiq_isp_tnr_t;

typedef struct {

} rk_aiq_isp_ynr_t;


typedef struct {

} rk_aiq_isp_uvnr_t;

typedef struct {

} rk_aiq_isp_sharpen_t;

typedef struct {

} rk_aiq_isp_edgeflt_t;
#endif

typedef struct {

} rk_aiq_isp_cgc_t;

#if 0
typedef struct {

} rk_aiq_isp_rawnr_t;
#endif


typedef struct {
    rk_aiq_isp_aec_stats_t* aec_stats;
    rk_aiq_awb_stat_res_v200_t* awb_stats_v200;
    rk_aiq_isp_af_stats_t*  af_stats;
} rk_aiq_isp_stats_t;

typedef RkAiqAhdrProcResult_t rk_aiq_isp_hdr_t;

#if 0
typedef struct {
    uint32_t module_enable_mask;
    rk_aiq_isp_aec_meas_t*   aec_meas;
    rk_aiq_isp_awb_meas_t*   awb_meas;
    rk_aiq_isp_af_meas_t*    af_meas;
    rk_aiq_isp_blc_t*        blc;
    rk_aiq_isp_dpcc_t*       dpcc;
    rk_aiq_isp_hdr_t*        hdr;
    rk_aiq_isp_rawnr_t*      rawnr;
    rk_aiq_isp_drc_t*        drc;
    rk_aiq_isp_gic_t*        gic;
    rk_aiq_isp_lsc_t*        lsc;
    rk_aiq_isp_demosaic_t*   demosaic;
    rk_aiq_isp_ldch_t*       ldch;
    rk_aiq_isp_lut3d_t*      lut3d;
    rk_aiq_isp_dehaze_t*     dehaze;
    rk_aiq_isp_ccm_t*        ccm;
    rk_aiq_isp_goc_t*        goc;
    rk_aiq_isp_wdr_t*        wdr;
    rk_aiq_isp_csm_t*        csm;
    rk_aiq_isp_tnr_t*        tnr;
    rk_aiq_isp_ynr_t*        ynr;
    rk_aiq_isp_uvnr_t*       uvnr;
    rk_aiq_isp_sharpen_t*    sharpen;
    rk_aiq_isp_edgeflt_t*    edgeflt;
    rk_aiq_isp_cgc_t*        cgc;
    rk_aiq_isp_conv422_t*    conv22;
    rk_aiq_isp_yuvconv_t*    yuvconv;
} rk_aiq_isp_partial_params_t;
#endif




typedef struct {
    uint32_t module_enable_mask;
    rk_aiq_isp_aec_meas_t   aec_meas;
    rk_aiq_awb_stat_cfg_v200_t   awb_cfg_v200;
    rk_aiq_awb_stat_cfg_v201_t   awb_cfg_v201;
    rk_aiq_wb_gain_t       awb_gain;
    rk_aiq_isp_af_meas_t    af_meas;
    rk_aiq_isp_hist_meas_t  hist_meas;
    rk_aiq_isp_blc_t        blc;
    rk_aiq_isp_dpcc_t       dpcc;
    RkAiqAhdrProcResult_t   ahdr_proc_res;//porc data for hw/simulator
    rk_aiq_isp_rawnr_t      rawnr;
    rk_aiq_isp_drc_t        drc;
    rk_aiq_isp_gic_t        gic;
    rk_aiq_lsc_cfg_t        lsc;
    rk_aiq_isp_demosaic_t   demosaic;
    rk_aiq_isp_ldch_t       ldch;
    rk_aiq_isp_fec_t        fec;
    rk_aiq_isp_orb_t        orb;
    rk_aiq_lut3d_cfg_t      lut3d;
    //rk_aiq_isp_dehaze_t     dehaze;
    RKAiqAdhazConfig_t adhaz_config;
    rk_aiq_ccm_cfg_t        ccm;
    //rk_aiq_isp_goc_t        goc;
    RKAiqAgammaHwConfig_t    agamma_config;
    rk_aiq_isp_wdr_t        wdr;
    rk_aiq_isp_csm_t        csm;
    rk_aiq_isp_tnr_t        tnr;
    rk_aiq_isp_ynr_t        ynr;
    rk_aiq_isp_uvnr_t       uvnr;
    rk_aiq_isp_sharpen_t    sharpen;
    rk_aiq_isp_edgeflt_t    edgeflt;
    rk_aiq_isp_cgc_t        cgc;
    rk_aiq_isp_conv422_t    conv22;
    rk_aiq_isp_yuvconv_t    yuvconv;
    //anr result
    rkaiq_anr_procRes_t     rkaiq_anr_proc_res;
    rkaiq_asharp_procRes_t  rkaiq_asharp_proc_res;
#ifdef RK_SIMULATOR_HW
    //rk_aiq_awb_stat_cfg_v200_t  awb_hw0_para;
    // rk_aiq_awb_stat_cfg_v201_t  awb_hw1_para;
    //rk_aiq_wb_gain_t        awb_gain_algo;
    // float                   awb_smooth_factor;
    //adhaz
    RKAiqAdhazHwConfig_t AdhazHwConfig;

    //agamma
    //RKAiqAgammaHwConfig_t  agamma_config;



    //ahdr
    //RkAiqAhdrProcResult_t   ahdr_proc_res;

    //adpcc
    //rkaiq_adpcc_procRes_t rkaiq_adpcc_proc_res;

    //adebayer
    //AdebayerConfig_t adebayer_config;

    //ablc
    //rk_aiq_isp_blc_t rkaiq_ablc_proc_res;
#endif
} rk_aiq_isp_params_t;

#endif
