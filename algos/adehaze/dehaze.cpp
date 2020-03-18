/*
 * rk_aiq_algo_acprc_itf.c
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
#include <string.h>
#include "../../xcore/base/xcam_common.h"
#include "dehaze.h"
#include "rk_aiq_uapi_adhz.h"
RKAIQ_BEGIN_DECLARE
/*
//useless
void select_dehzae_params_by_ISO(RKAiqAdhazHtmlConfig_t stRKdehazePara, RKAiqAdhazHwConfig_Select_t  *stRKdehazeParamsSelected, int iso)
{
    //确定iso等级
    //共有7个iso等级：50 100 200 400 800 1600 3200  6400 12800
    //       isogain: 1   2   4   8   16   32  64  128  256
    //      isolevel: 0   1   2   3   4    5   6   7    8
    int isoGainStd[9] = {1, 2, 4, 8, 16, 32, 64, 128, 256};
    int isoGain = MAX(int(iso / 50), 1);
    int isoGainLow = 0; //向下一个isoGain,用做参数插值：y=float(isoGainHig-isoGain)/float(isoGainHig-isoGainLow)*y[isoLevelLow]
    //                                  +float(isoGain-isoGainLow)/float(isoGainHig-isoGainLow)*y[isoLevelHig];
    int isoGainHig = 0; //向上一个isoGain
    int isoGainCorrect = 1; //选择最近的一档iso的配置

    int isoLevelLow = 0;
    int isoLevelHig = 0;
    int isoLevelCorrect = 0;
    int i, j;

    for (i = 0; i < 8; i++)
    {
        if (isoGain >= isoGainStd[i] && isoGain <= isoGainStd[i + 1])
        {
            isoGainLow = isoGainStd[i];
            isoGainHig = isoGainStd[i + 1];
            isoLevelLow = i;
            isoLevelHig = i + 1;
            isoGainCorrect = ((isoGain - isoGainStd[i]) <= (isoGainStd[i + 1] - isoGain)) ? isoGainStd[i] : isoGainStd[i + 1];
            isoLevelCorrect = ((isoGain - isoGainStd[i]) <= (isoGainStd[i + 1] - isoGain)) ? i : (i + 1);
        }
    }
//使能参数赋值
    stRKdehazeParamsSelected->sw_dhaz_en = stRKdehazePara.dehaze_en[0][isoLevelCorrect];
    stRKdehazeParamsSelected->sw_dhaz_dc_en = stRKdehazePara.dehaze_en[1][isoLevelCorrect];
    stRKdehazeParamsSelected->sw_dhaz_hist_en = stRKdehazePara.dehaze_en[2][isoLevelCorrect];
    stRKdehazeParamsSelected->sw_dhaz_hist_chn = stRKdehazePara.dehaze_en[3][isoLevelCorrect];
    stRKdehazeParamsSelected->sw_dhaz_gain_en = stRKdehazePara.dehaze_en[4][isoLevelCorrect];
    //阈值赋值
    stRKdehazeParamsSelected->sw_dhaz_dc_min_th = stRKdehazePara.dehaze_self_adp[0][isoLevelCorrect];
    stRKdehazeParamsSelected->sw_dhaz_dc_max_th = stRKdehazePara.dehaze_self_adp[1][isoLevelCorrect];
    stRKdehazeParamsSelected->sw_dhaz_yhist_th = stRKdehazePara.dehaze_self_adp[2][isoLevelCorrect];
    stRKdehazeParamsSelected->sw_dhaz_yblk_th = stRKdehazePara.dehaze_self_adp[3][isoLevelCorrect];
    stRKdehazeParamsSelected->sw_dark_th = stRKdehazePara.dehaze_self_adp[4][isoLevelCorrect];
    stRKdehazeParamsSelected->sw_dhaz_bright_min = stRKdehazePara.dehaze_self_adp[5][isoLevelCorrect];
    stRKdehazeParamsSelected->sw_dhaz_bright_max = stRKdehazePara.dehaze_self_adp[6][isoLevelCorrect];


    stRKdehazeParamsSelected->sw_dhaz_wt_max = stRKdehazePara.dehaze_range_adj[0][isoLevelCorrect];
    stRKdehazeParamsSelected->sw_dhaz_air_min = stRKdehazePara.dehaze_range_adj[1][isoLevelCorrect];
    stRKdehazeParamsSelected->sw_dhaz_air_max = stRKdehazePara.dehaze_range_adj[2][isoLevelCorrect];
    stRKdehazeParamsSelected->sw_dhaz_tmax_base = stRKdehazePara.dehaze_range_adj[3][isoLevelCorrect];
    stRKdehazeParamsSelected->sw_dhaz_tmax_off = stRKdehazePara.dehaze_range_adj[4][isoLevelCorrect];
    stRKdehazeParamsSelected->sw_dhaz_tmax_max = stRKdehazePara.dehaze_range_adj[5][isoLevelCorrect];

    //直方图阈值
    stRKdehazeParamsSelected->sw_dhaz_hist_gratio = stRKdehazePara.dehaze_hist_para[0][isoLevelCorrect];
    stRKdehazeParamsSelected->sw_dhaz_hist_th_off = stRKdehazePara.dehaze_hist_para[1][isoLevelCorrect];
    stRKdehazeParamsSelected->sw_dhaz_hist_k = stRKdehazePara.dehaze_hist_para[2][isoLevelCorrect];
    stRKdehazeParamsSelected->sw_dhaz_hist_min = stRKdehazePara.dehaze_hist_para[3][isoLevelCorrect];
    //对比度增强使能与阈值
    stRKdehazeParamsSelected->sw_dhaz_enhance_en = stRKdehazePara.dehaze_enhance[0][isoLevelCorrect];
    stRKdehazeParamsSelected->sw_dhaz_enhance_value = stRKdehazePara.dehaze_enhance[1][isoLevelCorrect];
    stRKdehazeParamsSelected->sw_dhaz_hpara_en = stRKdehazePara.dehaze_enhance[2][isoLevelCorrect];
    stRKdehazeParamsSelected->sw_dhaz_hist_scale = stRKdehazePara.dehaze_enhance[3][isoLevelCorrect];

    //iir
    stRKdehazeParamsSelected->sw_dhaz_hist_stab_fnum = stRKdehazePara.dehaze_iir_control[0][isoLevelCorrect];
    stRKdehazeParamsSelected->sw_dhaz_iir_sigma = stRKdehazePara.dehaze_iir_control[1][isoLevelCorrect];
    stRKdehazeParamsSelected->sw_dhaz_iir_wt_sigma = stRKdehazePara.dehaze_iir_control[2][isoLevelCorrect];
    stRKdehazeParamsSelected->sw_dhaz_iir_air_sigma = stRKdehazePara.dehaze_iir_control[3][isoLevelCorrect];
    stRKdehazeParamsSelected->sw_dhaz_iir_tmax_sigma = stRKdehazePara.dehaze_iir_control[4][isoLevelCorrect];

    //用户配置
    stRKdehazeParamsSelected->sw_dhaz_cfg_alpha = stRKdehazePara.dehaze_user_config[0][isoLevelCorrect];
    stRKdehazeParamsSelected->sw_dhaz_cfg_wt = stRKdehazePara.dehaze_user_config[1][isoLevelCorrect];
    stRKdehazeParamsSelected->sw_dhaz_cfg_air = stRKdehazePara.dehaze_user_config[2][isoLevelCorrect];
    stRKdehazeParamsSelected->sw_dhaz_cfg_tmax = stRKdehazePara.dehaze_user_config[3][isoLevelCorrect];
    stRKdehazeParamsSelected->sw_dhaz_cfg_gratio = stRKdehazePara.dehaze_user_config[4][isoLevelCorrect];

    stRKdehazeParamsSelected->sw_dhaz_dc_thed = stRKdehazePara.dehaze_bi_para[0][isoLevelCorrect];
    stRKdehazeParamsSelected->sw_dhaz_dc_weicur = stRKdehazePara.dehaze_bi_para[1][isoLevelCorrect];
    stRKdehazeParamsSelected->sw_dhaz_air_thed = stRKdehazePara.dehaze_bi_para[2][isoLevelCorrect];
    stRKdehazeParamsSelected->sw_dhaz_air_weitcur = stRKdehazePara.dehaze_bi_para[3][isoLevelCorrect];


    //矩阵赋值
    memcpy(stRKdehazeParamsSelected->sw_dhaz_dc_bf_h, stRKdehazePara.dehaze_dc_bf_h, sizeof(stRKdehazePara.dehaze_dc_bf_h));
    memcpy(stRKdehazeParamsSelected->sw_dhaz_air_bf_h, stRKdehazePara.dehaze_air_bf_h, sizeof(stRKdehazePara.dehaze_air_bf_h));
    memcpy(stRKdehazeParamsSelected->sw_dhaz_gaus_h, stRKdehazePara.dehaze_gaus_h, sizeof(stRKdehazePara.dehaze_gaus_h));
    memcpy(stRKdehazeParamsSelected->sw_dhaz_hist_conv_t0, stRKdehazePara.dehaze_hist_t0, sizeof(stRKdehazePara.dehaze_hist_t0));
    memcpy(stRKdehazeParamsSelected->sw_dhaz_hist_conv_t1, stRKdehazePara.dehaze_hist_t1, sizeof(stRKdehazePara.dehaze_hist_t1));
    memcpy(stRKdehazeParamsSelected->sw_dhaz_hist_conv_t2, stRKdehazePara.dehaze_hist_t2, sizeof( stRKdehazePara.dehaze_hist_t2));


}

*/

void LinearInterp(const float *pX, const float *pY, float posx, float *yOut, int XSize)
{
    int index;

    if (posx >= pX[XSize - 1])
    {
        *yOut = pY[XSize - 1];
    }
    else if (posx <= pX[0])
    {
        *yOut = pY[0];
    }
    else
    {
        index = 0;
        while((posx >= pX[index]) && (index < XSize))
        {
            index++;
        }
        index -= 1;
        *yOut = (((float)pY[index + 1] - (float)pY[index]) / ((float)pX[index + 1] - (float)pX[index]) * ((float)posx - (float)pX[index]))
                + pY[index];
    }

}

void select_RkDehaze_params(const CalibDb_Dehaze_t * stRKDehazeParam, RKAiqAdhazConfig_t *stRKDehazeParamSelected, int iso)
{
    int i;

    // dehaze_en[5]

	stRKDehazeParamSelected->dehaze_en[0] = stRKDehazeParam->dehaze_iso->dehaze_en;
	stRKDehazeParamSelected->dehaze_en[1] = stRKDehazeParam->dehaze_iso->dc_en;
	stRKDehazeParamSelected->dehaze_en[2] = stRKDehazeParam->dehaze_iso->hist_en;
	stRKDehazeParamSelected->dehaze_en[3] = stRKDehazeParam->dehaze_iso->hist_channel;
	stRKDehazeParamSelected->dehaze_en[4] = stRKDehazeParam->dehaze_iso->gain_en;
  /*
        LinearInterp(&stRKDehazeParam->dehaze_iso->iso, &stRKDehazeParam->dehaze_iso->dehaze_en, iso, &stRKDehazeParamSelected->dehaze_en[0], RK_DEHAZE_ISO_NUM);

	    LinearInterp(stRKDehazeParam.dehaze_iso.iso, stRKDehazeParam.dehaze_iso.dc_en, iso, &stRKDehazeParamSelected->dehaze_en[1], RK_DEHAZE_ISO_NUM);
	    LinearInterp(stRKDehazeParam.dehaze_iso.iso, stRKDehazeParam.dehaze_iso.hist_en, iso, &stRKDehazeParamSelected->dehaze_en[2], RK_DEHAZE_ISO_NUM);
	    LinearInterp(stRKDehazeParam.dehaze_iso.iso, stRKDehazeParam.dehaze_iso.hist_channel, iso, &stRKDehazeParamSelected->dehaze_en[3], RK_DEHAZE_ISO_NUM);
	    LinearInterp(stRKDehazeParam.dehaze_iso.iso, stRKDehazeParam.dehaze_iso.gain_en, iso, &stRKDehazeParamSelected->dehaze_en[4], RK_DEHAZE_ISO_NUM);*/

    // dehaze_self_adp[7]

        LinearInterp(&stRKDehazeParam->dehaze_iso->iso, &stRKDehazeParam->dehaze_iso->dc_min_th, iso, &stRKDehazeParamSelected->dehaze_self_adp[0], RK_DEHAZE_ISO_NUM);
	    LinearInterp(&stRKDehazeParam->dehaze_iso->iso, &stRKDehazeParam->dehaze_iso->dc_max_th, iso, &stRKDehazeParamSelected->dehaze_self_adp[1], RK_DEHAZE_ISO_NUM);
	    LinearInterp(&stRKDehazeParam->dehaze_iso->iso, &stRKDehazeParam->dehaze_iso->yhist_th, iso, &stRKDehazeParamSelected->dehaze_self_adp[2], RK_DEHAZE_ISO_NUM);
	    LinearInterp(&stRKDehazeParam->dehaze_iso->iso, &stRKDehazeParam->dehaze_iso->yblk_th, iso, &stRKDehazeParamSelected->dehaze_self_adp[3], RK_DEHAZE_ISO_NUM);
	    LinearInterp(&stRKDehazeParam->dehaze_iso->iso, &stRKDehazeParam->dehaze_iso->dark_th, iso, &stRKDehazeParamSelected->dehaze_self_adp[4], RK_DEHAZE_ISO_NUM);
	    LinearInterp(&stRKDehazeParam->dehaze_iso->iso, &stRKDehazeParam->dehaze_iso->bright_min, iso, &stRKDehazeParamSelected->dehaze_self_adp[5], RK_DEHAZE_ISO_NUM);
		LinearInterp(&stRKDehazeParam->dehaze_iso->iso, &stRKDehazeParam->dehaze_iso->bright_max, iso, &stRKDehazeParamSelected->dehaze_self_adp[6], RK_DEHAZE_ISO_NUM);

    // dehaze_range_adj[6]

        LinearInterp(&stRKDehazeParam->dehaze_iso->iso, &stRKDehazeParam->dehaze_iso->wt_max, iso, &stRKDehazeParamSelected->dehaze_range_adj[0], RK_DEHAZE_ISO_NUM);
		LinearInterp(&stRKDehazeParam->dehaze_iso->iso, &stRKDehazeParam->dehaze_iso->air_max, iso, &stRKDehazeParamSelected->dehaze_range_adj[1], RK_DEHAZE_ISO_NUM);
		LinearInterp(&stRKDehazeParam->dehaze_iso->iso, &stRKDehazeParam->dehaze_iso->air_min, iso, &stRKDehazeParamSelected->dehaze_range_adj[2], RK_DEHAZE_ISO_NUM);
		LinearInterp(&stRKDehazeParam->dehaze_iso->iso, &stRKDehazeParam->dehaze_iso->tmax_base, iso, &stRKDehazeParamSelected->dehaze_range_adj[3], RK_DEHAZE_ISO_NUM);
		LinearInterp(&stRKDehazeParam->dehaze_iso->iso, &stRKDehazeParam->dehaze_iso->tmax_off, iso, &stRKDehazeParamSelected->dehaze_range_adj[4], RK_DEHAZE_ISO_NUM);

		LinearInterp(&stRKDehazeParam->dehaze_iso->iso, &stRKDehazeParam->dehaze_iso->tmax_max, iso, &stRKDehazeParamSelected->dehaze_range_adj[5], RK_DEHAZE_ISO_NUM);

    // dehaze_hist_para[4]

        LinearInterp(&stRKDehazeParam->dehaze_iso->iso, &stRKDehazeParam->dehaze_iso->hist_gratio, iso, &stRKDehazeParamSelected->dehaze_hist_para[0], RK_DEHAZE_ISO_NUM);
		LinearInterp(&stRKDehazeParam->dehaze_iso->iso, &stRKDehazeParam->dehaze_iso->hist_th_off, iso, &stRKDehazeParamSelected->dehaze_hist_para[1], RK_DEHAZE_ISO_NUM);
		LinearInterp(&stRKDehazeParam->dehaze_iso->iso, &stRKDehazeParam->dehaze_iso->hist_k, iso, &stRKDehazeParamSelected->dehaze_hist_para[2], RK_DEHAZE_ISO_NUM);
		LinearInterp(&stRKDehazeParam->dehaze_iso->iso, &stRKDehazeParam->dehaze_iso->hist_min, iso, &stRKDehazeParamSelected->dehaze_hist_para[3], RK_DEHAZE_ISO_NUM);

    // dehaze_enhance[4]

        //LinearInterp(stRKDehazeParam.dehaze_iso.iso, stRKDehazeParam.dehaze_iso.enhance_en, iso, &stRKDehazeParamSelected->dehaze_enhance[0], RK_DEHAZE_ISO_NUM);

	    stRKDehazeParamSelected->dehaze_enhance[0] = stRKDehazeParam->dehaze_iso->enhance_en;
		LinearInterp(&stRKDehazeParam->dehaze_iso->iso, &stRKDehazeParam->dehaze_iso->enhance_value, iso, &stRKDehazeParamSelected->dehaze_enhance[1], RK_DEHAZE_ISO_NUM);
		//LinearInterp(stRKDehazeParam.dehaze_iso.iso, stRKDehazeParam.dehaze_iso.hist_para_en, iso, &stRKDehazeParamSelected->dehaze_enhance[2], RK_DEHAZE_ISO_NUM);
		stRKDehazeParamSelected->dehaze_enhance[2] = stRKDehazeParam->dehaze_iso->hist_para_en;
		LinearInterp(&stRKDehazeParam->dehaze_iso->iso, &stRKDehazeParam->dehaze_iso->hist_scale, iso, &stRKDehazeParamSelected->dehaze_enhance[3], RK_DEHAZE_ISO_NUM);

    // dehaze_iir_control[5]

        LinearInterp(&stRKDehazeParam->dehaze_iso->iso, &stRKDehazeParam->dehaze_iso->stab_fnum, iso, &stRKDehazeParamSelected->dehaze_iir_control[0], RK_DEHAZE_ISO_NUM);
		LinearInterp(&stRKDehazeParam->dehaze_iso->iso, &stRKDehazeParam->dehaze_iso->sigma, iso, &stRKDehazeParamSelected->dehaze_iir_control[1], RK_DEHAZE_ISO_NUM);
		LinearInterp(&stRKDehazeParam->dehaze_iso->iso, &stRKDehazeParam->dehaze_iso->wt_sigma, iso, &stRKDehazeParamSelected->dehaze_iir_control[2], RK_DEHAZE_ISO_NUM);
		LinearInterp(&stRKDehazeParam->dehaze_iso->iso, &stRKDehazeParam->dehaze_iso->air_sigma, iso, &stRKDehazeParamSelected->dehaze_iir_control[3], RK_DEHAZE_ISO_NUM);
		LinearInterp(&stRKDehazeParam->dehaze_iso->iso, &stRKDehazeParam->dehaze_iso->tmax_sigma, iso, &stRKDehazeParamSelected->dehaze_iir_control[4], RK_DEHAZE_ISO_NUM);

    // dehaze_user_config[5]

        LinearInterp(&stRKDehazeParam->dehaze_iso->iso, &stRKDehazeParam->dehaze_iso->cfg_alpha, iso, &stRKDehazeParamSelected->dehaze_user_config[0], RK_DEHAZE_ISO_NUM);
		LinearInterp(&stRKDehazeParam->dehaze_iso->iso, &stRKDehazeParam->dehaze_iso->cfg_wt, iso, &stRKDehazeParamSelected->dehaze_user_config[1], RK_DEHAZE_ISO_NUM);
		LinearInterp(&stRKDehazeParam->dehaze_iso->iso, &stRKDehazeParam->dehaze_iso->cfg_air, iso, &stRKDehazeParamSelected->dehaze_user_config[2], RK_DEHAZE_ISO_NUM);
		LinearInterp(&stRKDehazeParam->dehaze_iso->iso, &stRKDehazeParam->dehaze_iso->cfg_tmax, iso, &stRKDehazeParamSelected->dehaze_user_config[3], RK_DEHAZE_ISO_NUM);
		LinearInterp(&stRKDehazeParam->dehaze_iso->iso, &stRKDehazeParam->dehaze_iso->cfg_gratio, iso, &stRKDehazeParamSelected->dehaze_user_config[4], RK_DEHAZE_ISO_NUM);

    // dehaze_bi_para[4]

        LinearInterp(&stRKDehazeParam->dehaze_iso->iso, &stRKDehazeParam->dehaze_iso->dc_thed, iso, &stRKDehazeParamSelected->dehaze_bi_para[0], RK_DEHAZE_ISO_NUM);
	    LinearInterp(&stRKDehazeParam->dehaze_iso->iso, &stRKDehazeParam->dehaze_iso->dc_weitcur, iso, &stRKDehazeParamSelected->dehaze_bi_para[1], RK_DEHAZE_ISO_NUM);
		LinearInterp(&stRKDehazeParam->dehaze_iso->iso, &stRKDehazeParam->dehaze_iso->air_thed, iso, &stRKDehazeParamSelected->dehaze_bi_para[2], RK_DEHAZE_ISO_NUM);
		LinearInterp(&stRKDehazeParam->dehaze_iso->iso, &stRKDehazeParam->dehaze_iso->air_weitcur, iso, &stRKDehazeParamSelected->dehaze_bi_para[3], RK_DEHAZE_ISO_NUM);

    // dehaze_dc_bf_h[25]
    for (i = 0; i < 25; i++)
    {
        stRKDehazeParamSelected->dehaze_dc_bf_h[i] = stRKDehazeParam->dc_bf_h[i];
    }
    // dehaze_air_bf_h[9],dehaze_gaus_h[9]
    for (i = 0; i < 9; i++)
    {
        stRKDehazeParamSelected->dehaze_air_bf_h[i] = stRKDehazeParam->air_bf_h[i];
        stRKDehazeParamSelected->dehaze_gaus_h[i] = stRKDehazeParam->gaus_h[i];
    }
    // dehaze_hist_t0[6],dehaze_hist_t1[6],dehaze_hist_t2[6]
    for (i = 0; i < 6; i++)
    {
        stRKDehazeParamSelected->dehaze_hist_t0[i] = stRKDehazeParam->hist_conv_t0[i];
        stRKDehazeParamSelected->dehaze_hist_t1[i] = stRKDehazeParam->hist_conv_t1[i];
        stRKDehazeParamSelected->dehaze_hist_t2[i] = stRKDehazeParam->hist_conv_t2[i];
    }

}

XCamReturn AdehazeInitV200(AdehazeHandle_t** para)
{
    printf("ENTER: %s \n", __func__);

    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    *para = (AdehazeHandle_t*)malloc(sizeof(AdehazeHandle_t));
    return(ret);

}

XCamReturn AdehazeReleaseV200(AdehazeHandle_t* para)
{
    printf("ENTER: %s \n", __func__);

    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    free(para);
    return(ret);

}
XCamReturn AdehazeConfigV200(const CalibDb_Dehaze_t * adhaz_html_para, AdehazeHandle_t* para)
{

    printf("ENTER: %s \n", __func__);
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
#if 0
   // memset(&adhaz_html_para, 0, sizeof(RKAiqAdhazHtmlConfig_t));
#include "dhazhtmlpata.h"
#endif
    //TO DO
    //memcpy(&para->adhaz_html_para, &adhaz_html_para, sizeof(adhaz_html_para) );

    int iso = 50;//TODO
    //select_RkDehaze_params(para->adhaz_html_para, &para->adhaz_config, iso);
	select_RkDehaze_params(adhaz_html_para, &para->adhaz_config, iso);
    para->calib_dehaz = (CalibDb_Dehaze_t *)adhaz_html_para;

    return ret;
}
XCamReturn AdehazePreProcV200(AdehazeHandle_t* para)
{
    printf("ENTER: %s \n", __func__);

    XCamReturn ret = XCAM_RETURN_NO_ERROR;

    return(ret);

}
/*
XCamReturn AdehazeProcessingV200(AdehazeHandle_t* para)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    printf("ENTER: %s \n", __func__);

    ret = AdehazeReConfigV200(para);

    return(ret);
}
*/

XCamReturn AdehazeReConfigV200(AdehazeHandle_t* para,const CalibDb_Dehaze_t *calib_dehaze)
{
    XCamReturn ret = XCAM_RETURN_NO_ERROR;
    printf("ENTER: %s \n", __func__);


    int iso = 50;//TODO



    select_RkDehaze_params(calib_dehaze, &para->adhaz_config, iso);

    return(ret);
}



/*XCamReturn rk_aiq_uapi_setDhzMode(const rk_aiq_sys_ctx_t* ctx, opMode_t mode)
{


    XCamReturn  ret = XCAM_RETURN_NO_ERROR;

    if(mode == 0)//off
    {
    }
    else if(mode == 1)//auto
    {
    }
    else if(mode == 2)//manu
    {
    }
    else return(ret);

    return(ret);


}


XCamReturn rk_aiq_uapi_getDhzMode(const rk_aiq_sys_ctx_t* ctx, opMode_t *mode)
{
    XCamReturn  ret = XCAM_RETURN_NO_ERROR;
    printf("ENTER: %s \n", __func__);

    //TO DO

    return(ret);
}

XCamReturn rk_aiq_uapi_setDehazeManu(const rk_aiq_sys_ctx_t* ctx, float wt, int air, float tmax, float gratio)
{
    XCamReturn  ret = XCAM_RETURN_NO_ERROR;

    printf("ENTER: %s \n", __func__);

    //TO DO

    return(ret);
}


XCamReturn rk_aiq_uapi_getDehazeManu(const rk_aiq_sys_ctx_t* ctx, float *wt, int *air, float *tmax, float *gratio)
{
    XCamReturn  ret = XCAM_RETURN_NO_ERROR;
    printf("ENTER: %s \n", __func__);

    //TO DO

    return(ret);
}


XCamReturn rk_aiq_uapi_setIircontral(const rk_aiq_sys_ctx_t* ctx, int fnum, int sigma, float wt, float air, float tmax)//(fnum,sigma,wt,air,tmax)
{
    XCamReturn  ret = XCAM_RETURN_NO_ERROR;
    printf("ENTER: %s \n", __func__);

    //TO DO

    return(ret);
}


XCamReturn rk_aiq_uapi_getIircontral(const rk_aiq_sys_ctx_t* ctx, int *fnum, int *sigma, float *wt, float *air, float *tmax)
{
    XCamReturn  ret = XCAM_RETURN_NO_ERROR;
    printf("ENTER: %s \n", __func__);

    //TO DO

    return(ret);
}


XCamReturn rk_aiq_uapi_setHisEqMode(const rk_aiq_sys_ctx_t* ctx, opMode_t mode)
{
    XCamReturn  ret = XCAM_RETURN_NO_ERROR;
    printf("ENTER: %s \n", __func__);

    //TO DO

    return(ret);
}


XCamReturn rk_aiq_uapi_getHisEqMode(const rk_aiq_sys_ctx_t* ctx, opMode_t *mode)
{
    XCamReturn  ret = XCAM_RETURN_NO_ERROR;
    printf("ENTER: %s \n", __func__);

    //TO DO

    return(ret);
}


XCamReturn rk_aiq_uapi_setHisEqualManu(const rk_aiq_sys_ctx_t* ctx, float strength)//
{
    XCamReturn  ret = XCAM_RETURN_NO_ERROR;
    printf("ENTER: %s \n", __func__);

    //TO DO

    return(ret);
}


XCamReturn rk_aiq_uapi_getHisEqualManu(const rk_aiq_sys_ctx_t* ctx, float *strength)
{
    XCamReturn  ret = XCAM_RETURN_NO_ERROR;
    printf("ENTER: %s \n", __func__);

    //TO DO

    return(ret);
}


XCamReturn rk_aiq_uapi_enableEnhanceMode(const rk_aiq_sys_ctx_t* ctx, opMode_t mode)//
{
    XCamReturn  ret = XCAM_RETURN_NO_ERROR;
    printf("ENTER: %s \n", __func__);

    //TO DO

    return(ret);
}


XCamReturn rk_aiq_uapi_disableEnhanceMode(const rk_aiq_sys_ctx_t* ctx, opMode_t* mode)
{
    XCamReturn  ret = XCAM_RETURN_NO_ERROR;
    printf("ENTER: %s \n", __func__);

    //TO DO

    return(ret);
}


XCamReturn rk_aiq_uapi_setEnhance(const rk_aiq_sys_ctx_t* ctx, float strength)//
{
    XCamReturn  ret = XCAM_RETURN_NO_ERROR;
    printf("ENTER: %s \n", __func__);

    //TO DO

    return(ret);
}


XCamReturn rk_aiq_uapi_getEnhance(const rk_aiq_sys_ctx_t* ctx, float *strength)
{
    XCamReturn  ret = XCAM_RETURN_NO_ERROR;
    printf("ENTER: %s \n", __func__);

    //TO DO

    return(ret);
}


XCamReturn rk_aiq_uapi_getDehazeAdpOut(const rk_aiq_sys_ctx_t* ctx, float *wt, float *air, float *tmax, float *gratio)//(wt,air_base,tmax,gratio)//dehaze
{
    XCamReturn  ret = XCAM_RETURN_NO_ERROR;
    printf("ENTER: %s \n", __func__);

    //TO DO

    return(ret);
}*/

RKAIQ_END_DECLARE


