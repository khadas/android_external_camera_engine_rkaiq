/*
 * rk_aiq_algo_acprc_itf.h
 *
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

#ifndef __ADHAZ_H__
#define __ADHAZ_H__

#include "rk_aiq_comm.h"
#include "RkAiqCalibDbTypes.h"

RKAIQ_BEGIN_DECLARE

#define RK_DEHAZE_ISO_NUM 9

typedef struct RKAiqAdhazHtmlConfig_s
{
    float iso_idx              [RK_DEHAZE_ISO_NUM];
    float dehaze_en         [5][RK_DEHAZE_ISO_NUM];
    float dehaze_self_adp   [7][RK_DEHAZE_ISO_NUM]; //-
    float dehaze_range_adj  [6][RK_DEHAZE_ISO_NUM];
    float dehaze_hist_para  [4][RK_DEHAZE_ISO_NUM];
    float dehaze_enhance    [4][RK_DEHAZE_ISO_NUM];
    float dehaze_iir_control[5][RK_DEHAZE_ISO_NUM];
    float dehaze_user_config[5][RK_DEHAZE_ISO_NUM];
    float dehaze_bi_para    [4][RK_DEHAZE_ISO_NUM]; //-
    float dehaze_dc_bf_h   [25];
    float dehaze_air_bf_h   [9];
    float dehaze_gaus_h     [9];
    float dehaze_hist_t0    [6]; //-
    float dehaze_hist_t1    [6]; //-
    float dehaze_hist_t2    [6]; //-

} RKAiqAdhazHtmlConfig_t;

typedef struct RKAiqAdhazConfig_s
{
    float dehaze_en         [5];
    float dehaze_self_adp   [7]; //-
    float dehaze_range_adj  [6];
    float dehaze_hist_para  [4];
    float dehaze_enhance    [4];
    float dehaze_iir_control[5];
    float dehaze_user_config[5];
    float dehaze_bi_para    [4]; //-
    float dehaze_dc_bf_h   [25];
    float dehaze_air_bf_h   [9];
    float dehaze_gaus_h     [9];
    float dehaze_hist_t0    [6]; //-
    float dehaze_hist_t1    [6]; //-
    float dehaze_hist_t2    [6]; //-
} RKAiqAdhazConfig_t;


typedef struct RKAiqAdhazHwConfig_s
{
    // dehaze
    int            rkdehaze_en         [5];
    int            rkdehaze_self_adp   [7]; //-
    int            rkdehaze_range_adj  [6];
    int            rkdehaze_hist_para  [4];
    int            rkdehaze_enhance    [4];
    int            rkdehaze_iir_control[5];
    int            rkdehaze_user_config[5];
    int            rkdehaze_bi_para    [4]; //-
    unsigned short rkdehaze_dc_bf_h   [25];
    unsigned short rkdehaze_air_bf_h   [9];
    unsigned short rkdehaze_gaus_h     [9];
    int            rkdehaze_hist_t0    [6]; //-
    int            rkdehaze_hist_t1    [6]; //-
    int            rkdehaze_hist_t2    [6]; //-
} RKAiqAdhazHwConfig_t;


typedef struct RKAiqAdhazHwConfig_Select_s {
    int sw_dhaz_en;                     //0~1, (1 bit)
    int sw_dhaz_dc_en;                  //0~1, (1 bit)
    int sw_dhaz_hist_en;                //0~1, (1 bit)
    int sw_dhaz_hist_chn;               //0~1, (1 bit)
    int sw_dhaz_gain_en;                //0~1, (1 bit)
    int sw_dhaz_dc_min_th;              //range:16~120, default:64, (8 bit)
    int sw_dhaz_dc_max_th;              //range:170~255, default:192, (8 bit)
    int sw_dhaz_yhist_th;               //range:170~255, default:249, (8 bit)
    int sw_dhaz_yblk_th;                //range:(0.002~0.1)*col_blk*row_blk, default:0.002*col_blk*row_blk, (8 bit)
    int sw_dark_th;                     //range:230~250, default:250, (8 bit)
    int sw_dhaz_bright_min;             //range:160~200, default:180, (8 bit)
    int sw_dhaz_bright_max;             //range:210~250, default:240, (8 bit)
    int sw_dhaz_wt_max;                 //range:(0.75~0.9)*256, default:230, (9 bit)
    int sw_dhaz_air_min;                //range:200~220, default:200, (8 bit)
    int sw_dhaz_air_max;                //range:230~250, default:250, (8 bit)
    int sw_dhaz_tmax_base;              //range:131~105, default:125, (8 bit)
    int sw_dhaz_tmax_off;               //range:0.1*1024~0.5*1024, default:0.1*1024, (11 bit)
    int sw_dhaz_tmax_max;               //range:0.1*1024~0.5*1024, default:0.5*1024, (11 bit)
    int sw_dhaz_hist_gratio;            //(8 bit)
    int sw_dhaz_hist_th_off;            //default:64, (8 bit)
    int sw_dhaz_hist_k;                 //default:2, (5 bit)
    int sw_dhaz_hist_min;               //default:0.016*256, (9 bit)
    int sw_dhaz_enhance_en;             //0~1, (1 bit)
    int sw_dhaz_enhance_value;          //(14 bit)
    int sw_dhaz_hpara_en;               //0~1, (1 bit)
    int sw_dhaz_hist_scale;             //(13 bit)
    int sw_dhaz_hist_stab_fnum;         //(5 bit)
    int sw_dhaz_iir_sigma;              //max:255, default:6, (8 bit)
    int sw_dhaz_iir_wt_sigma;           //(11 bit)
    int sw_dhaz_iir_air_sigma;          //(8 bit)
    int sw_dhaz_iir_tmax_sigma;         //(11 bit)
    int sw_dhaz_cfg_alpha;              //range:0~255, default:0, (8 bit)
    int sw_dhaz_cfg_wt;                 //range:0~256, (9 bit)
    int sw_dhaz_cfg_air;                //range:0~255, (8 bit)
    int sw_dhaz_cfg_tmax;               //range:0~1024, (11 bit)
    int sw_dhaz_cfg_gratio;             //(13 bit)
    int sw_dhaz_dc_thed;                //range:0~255, default:31, (8 bit)
    int sw_dhaz_dc_weicur;              //range:0~256, default:1, (9 bit)
    int sw_dhaz_air_thed;               //range:0~255, default:31, (8 bit)
    int sw_dhaz_air_weitcur;            //range:0~256, default:0.14*256, (9 bit)
    unsigned short sw_dhaz_dc_bf_h[25];
    unsigned short sw_dhaz_air_bf_h[9];
    unsigned short sw_dhaz_gaus_h[9];
    int sw_dhaz_hist_conv_t0[6];
    int sw_dhaz_hist_conv_t1[6];
    int sw_dhaz_hist_conv_t2[6];
} RKAiqAdhazHwConfig_Select_t;

typedef struct AdehazeHandle_s {
    RKAiqAdhazConfig_t adhaz_config;
    RKAiqAdhazHtmlConfig_t adhaz_html_para;
    CalibDb_Dehaze_t* calib_dehaz;
} AdehazeHandle_t;

XCamReturn AdehazeInitV200(AdehazeHandle_t** para);
XCamReturn AdehazeReleaseV200(AdehazeHandle_t* para);
XCamReturn AdehazeConfigV200(const CalibDb_Dehaze_t * adhaz_html_para, AdehazeHandle_t* para);
XCamReturn AdehazePreProcV200(AdehazeHandle_t* para);
XCamReturn AdehazeProcessingV200(AdehazeHandle_t* para);
XCamReturn  AdehazeReConfigV200(AdehazeHandle_t *para,const CalibDb_Dehaze_t *calib_dehaze);


RKAIQ_END_DECLARE

#endif //_RK_AIQ_ALGO_ACPRC_ITF_H_
