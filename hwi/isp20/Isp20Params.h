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

#ifndef _CAM_HW_ISP20_PARAMS_H_
#define _CAM_HW_ISP20_PARAMS_H_

#include "rk_aiq_pool.h"
#include "rkisp2-config.h"
#include "rkispp-config.h"

namespace RkCam {

#define ROUND_F(x) (int)(((float)x)+(((x) > 0) ? 0.5 : (-0.5)))
#define ROUND_D(x) (long)(((double)x)+(((x) > 0) ? 0.5 : (-0.5)))
#define	FLOOR(a)   (int)( ((double)(a) < (int)(a)) ? (int)((a)-1) : (int)(a) )
#define	FLOOR_INT64(a) (long)( ((double)(a) < (long)(a)) ? (long)((a)-1) : (long)(a) )

// rawnr fix define
#define FIXVSTINV       10	    // VST fix
#define FIXSQRTBIT      30	    // SQRT fix bits
#define FIXSQRTDIV      1	    // SQRT fix
#define FIXNLMCALC      10	    // NLM calc fix
#define FIXNLMSQRT      6	    // NLM SQRT fix, FIXNLMSQRT*2 nee less FIXNLMCALC
#define FIXEXPBIT      20	    // EXP fix bits
#define FIXEXPDIV      1	    // EXP fix
#define FIXMERGEWG      10	    // HDR merge weight
#define FIXTMOWG        12	    // HDR TMO weight
#define FIXDIFMAX       ((long long)1<<(14))     // DIFF USE 12bits
#define DIVSUBBIT       1                        // div sub bits
#define FIXCURVE        8           // noise curve interp
#define FIXDIVBIT       20          // DIV MAX BITS
#define FIXDAOBIT       49          // DAO MAX BITS
#define FIXSHTBIT       2           // div result shift bits.
#define FIXMINBIT       8           // div min bits.
#define LUTMAX_FIX      30.0
#define LUTMAXM1_FIX    300.0 //29.0
#define LUTPRECISION_FIX (1<<FIXNLMCALC)
#define EXPPRECISION_FIX (1<<(FIXNLMCALC+5))

//tnr fix define
#define MFNR_MATALAB_FLG					0
#define FIX_VAL								(1 && !MFNR_MATALAB_FLG)//  1 for rtl_3dnr, 0 for matlab_3dnr
#define ACCURATE_LOW						0
#define F_ACCURATE_FLG_SCALE_L				ACCURATE_LOW
#if FIX_VAL
#define FIX_ENABLE_GAIN_IN					FIX_ENABLE							//FIX_DISABLE for rtl_3dnr
#define FIX_ENABLE_DELTA_SCALE				FIX_ENABLE
#define FIX_ENABLE_DELTA_CALC				FIX_ENABLE
#define FIX_ENABLE_DELTA_TO_DELTA_SQR		FIX_ENABLE
#define FIX_ENABLE_DELTA_CONV				FIX_ENABLE
#define FIX_ENABLE_PK_CALC					FIX_ENABLE
#define F_ACCURATE_FLG_SCALE_L				ACCURATE_LOW						//ACCURATE_HIGH for rtl_3dnr
#define F_ACCURATE_FLG_DELTA_SCALE_L		ACCURATE_LOW
#define F_ACCURATE_FLG_LUMA_W_IN_CHROMA		ACCURATE_LOW
#define F_ACCURATE_FLG_CONV_WEIGHT			ACCURATE_LOW
#define F_ACCURATE_FLG_CONV_OUT				ACCURATE_LOW
#define F_ACCURATE_FLG_CONV1_OUT			ACCURATE_LOW
#define F_ACCURATE_FLG_SIGMA				ACCURATE_LOW
#define F_ACCURATE_FLG_FRQ_DELTA			ACCURATE_LOW
#define F_ACCURATE_FLG_TXT_THRD_RATIO		ACCURATE_LOW
#define F_ACCURATE_FLG_TXT_THRD  			ACCURATE_LOW
#define F_ACCURATE_FLG_TXT_RATIO  			ACCURATE_LOW
#define F_ACCURATE_FLG_DELTA				ACCURATE_LOW
#define F_ACCURATE_FLG_EXP_VAL				ACCURATE_LOW						//ACCURATE_HIGH for rtl_3dnr
#define F_ACCURATE_FLG_PK_MID				ACCURATE_LOW
#define F_ACCURATE_FLG_EXP_SIGMA			ACCURATE_LOW
#define F_ACCURATE_FLG_EXP_SIGMA_RATIO		ACCURATE_LOW
#define F_ACCURATE_FLG_GAIN					ACCURATE_LOW
#define F_ACCURATE_FLG_PIXEL_RECON  		ACCURATE_LOW
#define F_ACCURATE_FLG_PIXEL_ORI	  		ACCURATE_LOW
#else
#define FIX_ENABLE_GAIN_IN					FIX_DISABLE
#define FIX_ENABLE_DELTA_SCALE				FIX_DISABLE
#define FIX_ENABLE_DELTA_CALC				FIX_DISABLE
#define FIX_ENABLE_DELTA_TO_DELTA_SQR		FIX_DISABLE
#define FIX_ENABLE_DELTA_CONV				FIX_DISABLE
#define FIX_ENABLE_PK_CALC					FIX_DISABLE
#define F_ACCURATE_FLG_SCALE_L				ACCURATE_HIGH
#define F_ACCURATE_FLG_DELTA_SCALE_L		ACCURATE_HIGH
#define F_ACCURATE_FLG_LUMA_W_IN_CHROMA		ACCURATE_HIGH
#define F_ACCURATE_FLG_CONV_WEIGHT			ACCURATE_HIGH
#define F_ACCURATE_FLG_CONV_OUT				ACCURATE_HIGH
#define F_ACCURATE_FLG_CONV1_OUT			ACCURATE_HIGH
#define F_ACCURATE_FLG_SIGMA				ACCURATE_HIGH
#define F_ACCURATE_FLG_FRQ_DELTA			ACCURATE_HIGH
#define F_ACCURATE_FLG_TXT_THRD_RATIO		ACCURATE_HIGH
#define F_ACCURATE_FLG_TXT_THRD  			ACCURATE_HIGH
#define F_ACCURATE_FLG_TXT_RATIO  			ACCURATE_HIGH
#define F_ACCURATE_FLG_DELTA				ACCURATE_HIGH
#define F_ACCURATE_FLG_EXP_VAL				ACCURATE_HIGH
#define F_ACCURATE_FLG_PK_MID				ACCURATE_HIGH
#define F_ACCURATE_FLG_EXP_SIGMA			ACCURATE_HIGH
#define F_ACCURATE_FLG_EXP_SIGMA_RATIO		ACCURATE_HIGH
#define F_ACCURATE_FLG_GAIN					ACCURATE_HIGH
#define F_ACCURATE_FLG_PIXEL_RECON  		ACCURATE_HIGH
#define F_ACCURATE_FLG_PIXEL_ORI	  		ACCURATE_HIGH
#endif

#define F_DECI_CONV_WEIGHT_ACCURATE					13
#define F_DECI_CONV_WEIGHT_REAL						8
#define F_DECI_CONV_WEIGHT							(F_ACCURATE_FLG_CONV_WEIGHT ? F_DECI_CONV_WEIGHT_ACCURATE : F_DECI_CONV_WEIGHT_REAL)
#define F_DECI_PIXEL_SIGMA_CONV_WEIGHT              F_DECI_CONV_WEIGHT
#define F_DECI_GAIN_ACCURATE                        (16)
#define F_DECI_GAIN_REAL                            8 //(MAX(F_DECI_GAIN_IN*2, 8))
#define F_DECI_GAIN                                 (F_ACCURATE_FLG_GAIN ? (F_DECI_GAIN_ACCURATE) : (F_DECI_GAIN_REAL))
#define F_DECI_GAIN_SQRT                            4// 8 for rtl_sqrt(F_DECI_GAIN / 2)
#define F_DECI_GAIN_GLB_SQRT                    	F_DECI_GAIN_SQRT
#define F_DECI_GAIN_GLB_SQRT_INV                    13 // (F_INTE_GAIN_SQRT + F_DECI_GAIN_SQRT)    	// 13   // (F_INTE_GAIN_SQRT + F_DECI_GAIN_SQRT + 4) is better jmj_3dnr
#define	F_DECI_LUMASCALE						    6		//8 for rtl_3dnr
#define F_DECI_SCALE_L_ACCURATE               		18
#define F_DECI_SCALE_L_REAL               			8
#define F_DECI_SCALE_L                        		(F_ACCURATE_FLG_SCALE_L ? F_DECI_SCALE_L_ACCURATE : F_DECI_SCALE_L_REAL)
#define F_DECI_SCALE_L_UV_ACCURATE               	18
#define F_DECI_SCALE_L_UV_REAL               		6
#define F_DECI_SCALE_L_UV                     		(F_ACCURATE_FLG_SCALE_L ? F_DECI_SCALE_L_UV_ACCURATE : F_DECI_SCALE_L_UV_REAL)
#define F_DECI_LUMA_W_IN_CHROMA_ACCURATE            16
#define F_DECI_LUMA_W_IN_CHROMA_REAL               	5
#define F_DECI_LUMA_W_IN_CHROMA                     (F_ACCURATE_FLG_LUMA_W_IN_CHROMA ? F_DECI_LUMA_W_IN_CHROMA_ACCURATE : F_DECI_LUMA_W_IN_CHROMA_REAL)
#define	F_DECI_SIGMA_ACCURATE						(24)
#define	F_DECI_SIGMA_REAL							(6 )
#define F_DECI_SIGMA                                (F_ACCURATE_FLG_SIGMA ? F_DECI_SIGMA_ACCURATE : F_DECI_SIGMA_REAL)
#define F_DECI_TXT_THRD_RATIO_ACCURATE              (16)
#define F_DECI_TXT_THRD_RATIO_REAL                  (6)		//(8)	 for rtl_3dnr
#define F_DECI_TXT_THRD_RATIO                       (F_ACCURATE_FLG_TXT_THRD_RATIO ? F_DECI_TXT_THRD_RATIO_ACCURATE : F_DECI_TXT_THRD_RATIO_REAL)


//uvnr fix define
#define RKUVNR_ratio 0
#define RKUVNR_offset 4
#define RKUVNR_kernels 7
#define RKUVNR_medRatio 4  //
#define RKUVNR_sigmaR 0  //
#define RKUVNR_uvgain 4  //
#define RKUVNR_exp2_lut_y 7 //
#define RKUVNR_bfRatio RKUVNR_exp2_lut_y


//ynr fix define
#define FIX_BIT_CI                     5//7
#define FIX_BIT_NOISE_SIGMA            5//7
#define FIX_BIT_DENOISE_WEIGHT         7
#define FIX_BIT_BF_SCALE               4//7
#define FIX_BIT_LUMA_CURVE             4//7
#define FIX_BIT_EDGE_SOFTNESS          7
#define FIX_BIT_GRAD_ADJUST_CURVE      4//7
#define FIX_BIT_LSC_ADJUST_RATIO       4
#define FIX_BIT_RADIAL_ADJUST_CURVE    4
#define FIX_BIT_VAR_TEXTURE            4
#define FIX_BIT_BF_W                   7
#define FIX_BIT_DENOISE_STRENGTH       4//7
#define FIX_BIT_SOFT_THRESHOLD_SCALE   8
#define FIX_BIT_SOFT_THRESHOLD_SCALE_V2   4
#define FIX_BIT_DIRECTION_STRENGTH     FIX_BIT_BF_SCALE
//#define FIX_BIT_LSCGAIN                4
#define FIX_BIT_4                      4
#define FIX_BIT_6                      6
#define FIX_BIT_7                      7
#define FIX_COEF_BIT                   2
#define YNR_FILT_MODE0                 0
#define YNR_FILT_MODE1                 1
#define YNR_DMA_NUM                    4

//exp(-x^2/2)
#define YNR_exp_lut_num 16 //
#define YNR_exp_lut_x 7 //
#define YNR_exp_lut_y 7 //
#define CLIPVALUE
#define CLIPBIT(a,b) ((a)>((1<<(b))-1)?((1<<(b))-1):(a))
#define WAVELET_LEVEL_1 0
#define WAVELET_LEVEL_2 1
#define WAVELET_LEVEL_3 2
#define WAVELET_LEVEL_4 3
#define YNR_SIGMA_BITS  10


//sharp fix define
#define reg_sharpenHW_lratio_fix_bits				8
#define reg_sharpenHW_hratio_fix_bits				8
#define reg_sharpenHW_M_ratio_fix_bits				2
#define reg_sharpenHW_H_ratio_fix_bits				2
#define reg_sharpenHW_pbf_ratio_fix_bits			7
#define reg_sharpenHW_lum_min_m_fix_bits			4
#define reg_sharpenHW_hbf_ratio_fix_bits			8
#define reg_sharpenHW_pPBfCoeff_fix_bits			6
#define reg_sharpenHW_pMRfCoeff_fix_bits			7
#define reg_sharpenHW_pMBfCoeff_fix_bits			6
#define reg_sharpenHW_pHRfCoeff_fix_bits			7
#define reg_sharpenHW_pHBfCoeff_fix_bits			6

#define RK_EDGEFILTER_COEF_BIT      				(6)          // coef Quantilize bits
#define	reg_dir_min_fix_bits             		RK_EDGEFILTER_COEF_BIT
#define	reg_l_alpha_fix_bits             		8
#define	reg_g_alpha_fix_bits             		8
#define	reg_detail_alpha_dog_fix_bits    		6
#define	reg_h0_h_coef_5x5_fix_bits       		6
#define	reg_h_coef_5x5_fix_bits          		6
#define	reg_gf_coef_3x3_fix_bits         		4
#define	reg_dog_kernel_fix_bits          		6
#define	reg_smoth4_fix_bits						8



class Isp20Params {
public:
    explicit Isp20Params() {};
    virtual ~Isp20Params() {};

    virtual XCamReturn checkIsp20Params(struct isp2x_isp_params_cfg& isp_cfg);
    virtual XCamReturn convertAiqResultsToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
            SmartPtr<RkAiqIspParamsProxy> aiq_results,
            SmartPtr<RkAiqIspParamsProxy>& last_aiq_results);

private:
    XCAM_DEAD_COPY(Isp20Params);
    void convertAiqLiteHstWndSize(struct isp2x_rawhistlite_cfg & hst_lite);
    void convertAiqBigHstWndSize(struct isp2x_rawhistbig_cfg & hst_big);
    void convertAiqSiHstWndSize(struct isp2x_sihst_cfg & sihst);
    void convertAiqAeToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
                                   const rk_aiq_isp_aec_meas_t& aec_meas);
    void convertAiqHistToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
                                     const rk_aiq_isp_hist_meas_t& hist_meas);
    void convertAiqAwbToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
                                    const rk_aiq_awb_stat_cfg_v200_t& awb_meas);
    void convertAiqAwbGainToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
                                        const rk_aiq_wb_gain_t& awb_gain);
    void convertAiqAhdrToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
                                     const rk_aiq_isp_hdr_t& ahdr_data);
    void convertAiqAdehazeToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
                                        const RKAiqAdhazConfig_t& dhaze                     );
    void convertAiqAgammaToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
                                       const rk_aiq_isp_agamma_t& gamma_out_cfg);
    void convertAiqAdemosaicToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
                                          SmartPtr<RkAiqIspParamsProxy> aiq_results);
    void convertAiqLscToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
                                    const rk_aiq_lsc_cfg_t& lsc);
    void convertAiqBlcToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
                                    SmartPtr<RkAiqIspParamsProxy> aiq_results);
    void convertAiqDpccToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
                                     SmartPtr<RkAiqIspParamsProxy> aiq_results);
    void convertAiqCcmToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
                                    const rk_aiq_ccm_cfg_t& ccm);
    void convertAiqA3dlutToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
                                       const rk_aiq_lut3d_cfg_t& lut3d_cfg);
	void convertAiqRawnrToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
        	rk_aiq_isp_rawnr_t& rawnr);
	void convertAiqTnrToIsp20Params(struct rkispp_params_cfg& pp_cfg,
        rk_aiq_isp_tnr_t& tnr);
	void convertAiqUvnrToIsp20Params(struct rkispp_params_cfg& pp_cfg,
        rk_aiq_isp_uvnr_t& uvnr);
	void convertAiqYnrToIsp20Params(struct rkispp_params_cfg& pp_cfg,
        rk_aiq_isp_ynr_t& ynr);
	void convertAiqSharpenToIsp20Params(struct rkispp_params_cfg& pp_cfg,
        rk_aiq_isp_sharpen_t& sharp, rk_aiq_isp_edgeflt_t& edgeflt);
    void convertAiqAfToIsp20Params(struct isp2x_isp_params_cfg& isp_cfg,
                                   const rk_aiq_isp_af_meas_t& af_data);
};
};
#endif
