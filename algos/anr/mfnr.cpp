#include "mfnr.h"

#define MFNR_FIX_VALUE_PRINTF 1

RKAiqResult_t init_mfnr_params(RKAnr_Mfnr_Params_t *pParams, CalibDb_MFNR_t *pCalibdb)
{
	RKAiqResult_t res = RK_AIQ_RET_SUCCESS;
	int i = 0;
	int j = 0;

	if(pParams == NULL){
		LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return RK_AIQ_RET_NULL_POINTER;
	}

	if(pCalibdb == NULL){
		LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return RK_AIQ_RET_NULL_POINTER;
	}

	int max_iso_step        = MAX_ISO_STEP;
    int dir_num             = MFNR_DIR_NUM;
    int polyorder           = MFNR_POLYORDER;
    int max_lvl             = MFNR_MAX_LVL;
    int max_lvl_uv          = MFNR_MAX_LVL_UV;
    int lumancurve_step     = LUMANRCURVE_STEP;
    int range               = 1 << Y_CALIBRATION_BITS;
    int dir_lo              = DIR_LO;
    int dir_hi              = DIR_HI;

	
	max_lvl = pCalibdb->max_level;
	max_lvl_uv = pCalibdb->max_level_uv;
	pParams->back_ref_num = pCalibdb->back_ref_num;
	
	for(int lvl = 0; lvl < max_lvl; lvl++){
        for (i=0;i<max_iso_step;i++)
            pParams->weight_limit_y[i][lvl] = pCalibdb->mfnr_iso[i].weight_limit_y[lvl];
    }
	
	for(int lvl = 0; lvl < max_lvl_uv; lvl++){
        for (i=0;i<max_iso_step;i++)
            pParams->weight_limit_uv[i][lvl] = pCalibdb->mfnr_iso[i].weight_limit_uv[lvl];
    }

	for(int j = 0; j < 4; j++){
        for (i=0;i<max_iso_step;i++)
            pParams->ratio_frq[i][j] = pCalibdb->mfnr_iso[i].ratio_frq[j];
    }
	
	for(int lvl = 0; lvl < max_lvl_uv; lvl++){
        for (i=0;i<max_iso_step;i++)
            pParams->luma_w_in_chroma[i][lvl] = pCalibdb->mfnr_iso[i].luma_w_in_chroma[lvl];
    }

	for(j=0; j<4; j++){
		for(i=0; i<2; i++)
			pParams->awb_uv_ratio[j][i] = pCalibdb->uv_ratio[j].ratio[i];
	}

	for(int j = 0; j < polyorder + 1; j++){
        for (i=0;i<max_iso_step;i++)
            pParams->curve[i][j] = pCalibdb->mfnr_iso[i].noise_curve[j];
    }

	for (i=0;i<max_iso_step;i++){
        pParams->curve_x0[i] = pCalibdb->mfnr_iso[i].noise_curve_x00;
    }


    for (j = 0; j < max_lvl; j++){
        for (i=0;i<max_iso_step;i++){
            pParams->ci[i][0][j] = pCalibdb->mfnr_iso[i].y_lo_noiseprofile[j];
			pParams->ci[i][1][j] = pCalibdb->mfnr_iso[i].y_hi_noiseprofile[j];			
            pParams->scale[i][0][j] = pCalibdb->mfnr_iso[i].y_lo_bfscale[j];
			pParams->scale[i][1][j] = pCalibdb->mfnr_iso[i].y_hi_bfscale[j];
        }
    }

	for (j = 0; j < lumancurve_step; j++){
        for (i=0;i<max_iso_step;i++){
            pParams->lumanrpoint[i][dir_lo][j] = pCalibdb->mfnr_iso[i].y_lumanrpoint[j];
			pParams->lumanrcurve[i][dir_lo][j] = pCalibdb->mfnr_iso[i].y_lumanrcurve[j];
			pParams->lumanrpoint[i][dir_hi][j] = pCalibdb->mfnr_iso[i].y_lumanrpoint[j];
			pParams->lumanrcurve[i][dir_hi][j] = pCalibdb->mfnr_iso[i].y_lumanrcurve[j];
        }
    }

	for (i=0;i<max_iso_step;i++){
        pParams->dnstr[i][dir_lo] = pCalibdb->mfnr_iso[i].y_denoisestrength;
		pParams->dnstr[i][dir_hi] = pParams->dnstr[i][dir_lo];
    }


    for(int j = 0; j < 6; j++){
        for (i=0;i<max_iso_step;i++){
            pParams->gfdelta[i][0][0][j] = pCalibdb->mfnr_iso[i].y_lo_lvl0_gfdelta[j];
			pParams->gfdelta[i][1][0][j] = pCalibdb->mfnr_iso[i].y_hi_lvl0_gfdelta[j];
        }
    }

	for(int j = 0; j < 3; j++){
        for (i=0;i<max_iso_step;i++){
			pParams->gfdelta[i][0][1][j] = pCalibdb->mfnr_iso[i].y_lo_lvl1_gfdelta[j];
			pParams->gfdelta[i][0][2][j] = pCalibdb->mfnr_iso[i].y_lo_lvl2_gfdelta[j];
			pParams->gfdelta[i][0][3][j] = pCalibdb->mfnr_iso[i].y_lo_lvl3_gfdelta[j];
			
			pParams->gfdelta[i][1][1][j] = pCalibdb->mfnr_iso[i].y_hi_lvl1_gfdelta[j];
			pParams->gfdelta[i][1][2][j] = pCalibdb->mfnr_iso[i].y_hi_lvl2_gfdelta[j];
			pParams->gfdelta[i][1][3][j] = pCalibdb->mfnr_iso[i].y_hi_lvl3_gfdelta[j];
        }
    }

    for (j = 0; j < max_lvl_uv; j++){
        for (i=0;i<max_iso_step;i++){
            pParams->ci_uv[i][0][j] = pCalibdb->mfnr_iso[i].uv_lo_noiseprofile[j];
			pParams->ci_uv[i][1][j] = pCalibdb->mfnr_iso[i].uv_hi_noiseprofile[j];
            pParams->scale_uv[i][0][j] = pCalibdb->mfnr_iso[i].uv_lo_bfscale[j];
			pParams->scale_uv[i][1][j] = pCalibdb->mfnr_iso[i].uv_hi_bfscale[j];

			printf("j:%d i:%d ci:%f %f scale:%f %f\n", j, i,
				pParams->ci_uv[i][0][j],
				pParams->ci_uv[i][1][j],
				pParams->scale_uv[i][0][j],
				pParams->scale_uv[i][1][j]);
        }
    }

	for (j = 0; j < lumancurve_step; j++){
        for (i=0;i<max_iso_step;i++){
            pParams->lumanrpoint_uv[i][dir_lo][j] = pCalibdb->mfnr_iso[i].uv_lumanrpoint[j];
			pParams->lumanrcurve_uv[i][dir_lo][j] = pCalibdb->mfnr_iso[i].uv_lumanrcurve[j];
			pParams->lumanrpoint_uv[i][dir_hi][j] = pCalibdb->mfnr_iso[i].uv_lumanrpoint[j];
			pParams->lumanrcurve_uv[i][dir_hi][j] = pCalibdb->mfnr_iso[i].uv_lumanrcurve[j];
        }
    }
	
	for (i=0;i<max_iso_step;i++){
        pParams->dnstr_uv[i][dir_lo] = pCalibdb->mfnr_iso[i].uv_denoisestrength;
		pParams->dnstr_uv[i][dir_hi] = pParams->dnstr_uv[i][dir_lo];
		printf("%d: dnstr_uv:%f %f\n", i,
			pParams->dnstr_uv[i][dir_lo],
			pParams->dnstr_uv[i][dir_hi]);
    }

	for(int j = 0; j < 6; j++){
        for (i=0;i<max_iso_step;i++){
            pParams->gfdelta_uv[i][0][0][j] = pCalibdb->mfnr_iso[i].uv_lo_lvl0_gfdelta[j];
			pParams->gfdelta_uv[i][1][0][j] = pCalibdb->mfnr_iso[i].uv_hi_lvl0_gfdelta[j];
        }
    }
	
	for(int j = 0; j < 3; j++){
		for (i=0;i<max_iso_step;i++){
			pParams->gfdelta_uv[i][0][1][j] = pCalibdb->mfnr_iso[i].uv_lo_lvl1_gfdelta[j];
			pParams->gfdelta_uv[i][0][2][j] = pCalibdb->mfnr_iso[i].uv_lo_lvl2_gfdelta[j];		
			pParams->gfdelta_uv[i][1][1][j] = pCalibdb->mfnr_iso[i].uv_hi_lvl1_gfdelta[j];
			pParams->gfdelta_uv[i][1][2][j] = pCalibdb->mfnr_iso[i].uv_hi_lvl2_gfdelta[j];
		}
	}


    for(int j = 0; j < 6; j++){
        for (i=0;i<max_iso_step;i++){
            pParams->gfsigma[i][0][j] = pCalibdb->mfnr_iso[i].lvl0_gfsigma[j];
        }
    }

	for(int j = 0; j < 3; j++){
        for (i=0;i<max_iso_step;i++){
            pParams->gfsigma[i][1][j] = pCalibdb->mfnr_iso[i].lvl1_gfsigma[j];
			pParams->gfsigma[i][2][j] = pCalibdb->mfnr_iso[i].lvl2_gfsigma[j];
			pParams->gfsigma[i][3][j] = pCalibdb->mfnr_iso[i].lvl3_gfsigma[j];
        }
    }

	for (i=0;i<max_iso_step;i++){
        int64_t curve_x0 = (int)pParams->curve_x0[i];
        for (j = 0; j < range; j++){
            int64_t X[5];
            double y;

            X[0] = 1;
            y = pParams->curve[i][polyorder];
            for(int order = 1; order < polyorder + 1; order++){
                X[order] = X[order - 1] * j;
                y = y + (double)X[order] * pParams->curve[i][polyorder-order];
            }

            if(j > curve_x0)
                pParams->noise_sigma[i][j] = pParams->noise_sigma[i][curve_x0];
            else
                pParams->noise_sigma[i][j] = y;
        }
    }

	//memcpy(pParams->mfnr_ver_char, pCalibdb->version, sizeof(pParams->mfnr_ver_char));
	LOGI_ANR("%s(%d): exit!\n", __FUNCTION__, __LINE__);
	return RK_AIQ_RET_SUCCESS;
}


void select_mfnr_params_by_ISO(RKAnr_Mfnr_Params_t *stmfnrParams, 	RKAnr_Mfnr_Params_Select_t *stmfnrParamsSelected, int iso, int bits_proc)
{
    int i, j;
	int iso_low, iso_high;
	int gain_high, gain_low;
	double ratio;
	int iso_div 			= 50;
	int dir_num 			= MFNR_DIR_NUM;
	int polyorder 			= MFNR_POLYORDER;
	int max_lvl 		    = MFNR_MAX_LVL;
	int max_lvl_uv 			= MFNR_MAX_LVL_UV;
	int lumancurve_step 	= LUMANRCURVE_STEP;
	int range 			    = (1 << Y_CALIBRATION_BITS);
    int step                = (1 << (Y_CALIBRATION_BITS - bits_proc));
    int step_x              = (1 << (Y_CALIBRATION_BITS - Y_SIGMA_TABLE_BITS));
    int step_x1             = (1 << (Y_CALIBRATION_BITS - bits_proc));
    int step_y              = (1 << (Y_CALIBRATION_BITS - bits_proc));
	int step_x_dehz         = (1 << (Y_CALIBRATION_BITS - Y_SIGMA_TABLE_BITS_DEHAZE));
	double noise_sigma_tmp[(1 << Y_CALIBRATION_BITS) + 1];
	for (i = MAX_ISO_STEP - 1; i >= 0; i--)
	{
		if (iso < iso_div * (2 << i))
		{
			iso_low = iso_div * (2 << (i)) / 2;
			iso_high = iso_div * (2 << i);
		}
	}
	ratio = (double)(iso - iso_low)/(iso_high-iso_low);
	if (iso_low == iso)
	{
		iso_high = iso;
		ratio = 0;
	}
	if (iso_high == iso )
	{
		iso_low = iso;
	    	ratio = 1;
	}
	gain_high 		= (int)(log((double)iso_high / 50) /log((double)2));

	gain_low 		= (int)(log((double)iso_low / 50) / log((double)2));

	stmfnrParamsSelected->back_ref_num 				= stmfnrParams->back_ref_num;
	for(int lvl = 0; lvl < max_lvl; lvl++)
		stmfnrParamsSelected->weight_limit_y[lvl]	= ratio * (stmfnrParams->weight_limit_y[gain_high][lvl]   - stmfnrParams->weight_limit_y[gain_low][lvl])   + stmfnrParams->weight_limit_y[gain_low][lvl];;
	for(int lvl = 0; lvl < max_lvl_uv; lvl++)
		stmfnrParamsSelected->weight_limit_uv[lvl] 	= ratio * (stmfnrParams->weight_limit_uv[gain_high][lvl]  - stmfnrParams->weight_limit_uv[gain_low][lvl])  + stmfnrParams->weight_limit_uv[gain_low][lvl];;
	for(int i = 0; i < 4; i++)
		stmfnrParamsSelected->ratio_frq[i]          = ratio * (stmfnrParams->ratio_frq[gain_high][i]          - stmfnrParams->ratio_frq[gain_low][i])          + stmfnrParams->ratio_frq[gain_low][i];

	for(int lvl = 0; lvl < max_lvl_uv; lvl++)
		stmfnrParamsSelected->luma_w_in_chroma[lvl] = ratio * (stmfnrParams->luma_w_in_chroma[gain_high][lvl] - stmfnrParams->luma_w_in_chroma[gain_low][lvl]) + stmfnrParams->luma_w_in_chroma[gain_low][lvl];
	for (int dir_idx = 0; dir_idx < dir_num; dir_idx++)
		for (j = 0; j < max_lvl; j++)
		{
			stmfnrParamsSelected->ci[dir_idx][j] 			= ratio * (1 / stmfnrParams->ci[gain_high][dir_idx][j] - 1 / stmfnrParams->ci[gain_low][dir_idx][j]) + 1 / stmfnrParams->ci[gain_low][dir_idx][j];
		}
	for (int dir_idx = 0; dir_idx < dir_num; dir_idx++)
		for (j = 0; j < max_lvl; j++)
		{
			stmfnrParamsSelected->dnweight[dir_idx][j] 	= ratio * (stmfnrParams->dnweight[gain_high][dir_idx][j] - stmfnrParams->dnweight[gain_low][dir_idx][j]) + stmfnrParams->dnweight[gain_low][dir_idx][j];
		}

	for (int dir_idx = 0; dir_idx < dir_num; dir_idx++)
		for (j = 0; j < max_lvl; j++)
		{
			stmfnrParamsSelected->scale[dir_idx][j] 	    = ratio * (stmfnrParams->scale[gain_high][dir_idx][j] - stmfnrParams->scale[gain_low][dir_idx][j]) + stmfnrParams->scale[gain_low][dir_idx][j];
		}
	for (int dir_idx = 0; dir_idx < dir_num; dir_idx++)
		for (j = 0; j < lumancurve_step; j++)
		{
			stmfnrParamsSelected->lumanrpoint[dir_idx][j]   = ratio * (stmfnrParams->lumanrpoint[gain_high][dir_idx][j] - stmfnrParams->lumanrpoint[gain_low][dir_idx][j]) + stmfnrParams->lumanrpoint[gain_low][dir_idx][j];
			stmfnrParamsSelected->lumanrcurve[dir_idx][j]   = ratio * (stmfnrParams->lumanrcurve[gain_high][dir_idx][j] - stmfnrParams->lumanrcurve[gain_low][dir_idx][j]) + stmfnrParams->lumanrcurve[gain_low][dir_idx][j];
		}
    for (int dir_idx = 0; dir_idx < dir_num; dir_idx++)
	    stmfnrParamsSelected->dnstr[dir_idx] 			    = ratio * (stmfnrParams->dnstr[gain_high][dir_idx] - stmfnrParams->dnstr[gain_low][dir_idx]) + stmfnrParams->dnstr[gain_low][dir_idx];
	for (int dir_idx = 0; dir_idx < dir_num; dir_idx++)
		for (int lvl = 0; lvl < max_lvl; lvl++)
		{
		    for(int i = 0; i < MFNR_MAX_GAUS_SIZE; i++)
			    stmfnrParamsSelected->gfdelta[dir_idx][lvl][i]  = ratio * (stmfnrParams->gfdelta[gain_high][dir_idx][lvl][i] - stmfnrParams->gfdelta[gain_low][dir_idx][lvl][i]) + stmfnrParams->gfdelta[gain_low][dir_idx][lvl][i];
		}

	for (int dir_idx = 0; dir_idx < dir_num; dir_idx++)
		for (j = 0; j < max_lvl_uv; j++)
		{
			stmfnrParamsSelected->ci_uv[dir_idx][j] 		= ratio * (1 / stmfnrParams->ci_uv[gain_high][dir_idx][j] - 1 / stmfnrParams->ci_uv[gain_low][dir_idx][j]) + 1 / stmfnrParams->ci_uv[gain_low][dir_idx][j];
			stmfnrParamsSelected->dnweight_uv[dir_idx][j]   = ratio * (stmfnrParams->dnweight_uv[gain_high][dir_idx][j] - stmfnrParams->dnweight_uv[gain_low][dir_idx][j]) + stmfnrParams->dnweight_uv[gain_low][dir_idx][j];
			stmfnrParamsSelected->scale_uv[dir_idx][j]      = ratio * (stmfnrParams->scale_uv[gain_high][dir_idx][j] - stmfnrParams->scale_uv[gain_low][dir_idx][j]) + stmfnrParams->scale_uv[gain_low][dir_idx][j];
		}
    for (int dir_idx = 0; dir_idx < dir_num; dir_idx++)
		for (j = 0; j < lumancurve_step; j++)
		{
			stmfnrParamsSelected->lumanrpoint_uv[dir_idx][j]    = ratio * (stmfnrParams->lumanrpoint_uv[gain_high][dir_idx][j] - stmfnrParams->lumanrpoint_uv[gain_low][dir_idx][j]) + stmfnrParams->lumanrpoint_uv[gain_low][dir_idx][j];
			stmfnrParamsSelected->lumanrcurve_uv[dir_idx][j]    = ratio * (stmfnrParams->lumanrcurve_uv[gain_high][dir_idx][j] - stmfnrParams->lumanrcurve_uv[gain_low][dir_idx][j]) + stmfnrParams->lumanrcurve_uv[gain_low][dir_idx][j];
		}
	for (int dir_idx = 0; dir_idx < dir_num; dir_idx++)
		stmfnrParamsSelected->dnstr_uv[dir_idx] 			    = ratio * (stmfnrParams->dnstr_uv[gain_high][dir_idx] - stmfnrParams->dnstr_uv[gain_low][dir_idx]) + stmfnrParams->dnstr_uv[gain_low][dir_idx];

	for (int dir_idx = 0; dir_idx < dir_num; dir_idx++)
		for (int lvl = 0; lvl < max_lvl; lvl++)
		{
		    for(int i = 0; i < MFNR_MAX_GAUS_SIZE; i++)
			    stmfnrParamsSelected->gfdelta_uv[dir_idx][lvl][i]  = ratio * (stmfnrParams->gfdelta_uv[gain_high][dir_idx][lvl][i] - stmfnrParams->gfdelta_uv[gain_low][dir_idx][lvl][i]) + stmfnrParams->gfdelta_uv[gain_low][dir_idx][lvl][i];
		}




	for (int lvl = 0; lvl < max_lvl; lvl++)
	{
	    for(int i = 0; i < MFNR_MAX_GAUS_SIZE; i++)
		    stmfnrParamsSelected->gfsigma[lvl][i]           = ratio * (stmfnrParams->gfsigma[gain_high][lvl][i] - stmfnrParams->gfsigma[gain_low][lvl][i]) + stmfnrParams->gfsigma[gain_low][lvl][i];
	}

	for (i = 0; i < range; i++)
		noise_sigma_tmp[i]                                  = (ratio * (stmfnrParams->noise_sigma[gain_high][i] - stmfnrParams->noise_sigma[gain_low][i]) + stmfnrParams->noise_sigma[gain_low][i]);
	noise_sigma_tmp[range]							        = noise_sigma_tmp[range - 1];

    int max_pos[2];
    int min_pos[2];
    int intep_pos_flg[POLT_ORDER + 2];
    int intep_pos[POLT_ORDER + 2];
    int fix_x_pos[MAX_INTEPORATATION_LUMAPOINT];
    int max_idx = 0;
    int min_idx = 0;
    int intep_num = 0;
    int intep_num_1 = 0;
    for(i = 1; i < MAX(stmfnrParams->curve_x0[gain_high], stmfnrParams->curve_x0[gain_low]); i++)
    {
        if(noise_sigma_tmp[i] > noise_sigma_tmp[i - 1] && noise_sigma_tmp[i] > noise_sigma_tmp[i + 1])
        {
            max_pos[max_idx]    = i;
            max_idx++;
        }
        if(noise_sigma_tmp[i] < noise_sigma_tmp[i - 1] && noise_sigma_tmp[i] < noise_sigma_tmp[i + 1])
        {
            min_pos[min_idx]    = i;
            min_idx++;
        }
    }
    intep_pos[intep_num++]    = 0;
    intep_pos[intep_num++]    = MAX(stmfnrParams->curve_x0[gain_high], stmfnrParams->curve_x0[gain_low]);
    for(i = 0; i < max_idx; i++)
        intep_pos[intep_num++]    = max_pos[i];
    for(i = 0; i < MIN(1, min_idx); i++)// only 1 min point is allowed
        intep_pos[intep_num++]    = min_pos[i];
    for(i = 0; i < intep_num; i++)
        for(j = i; j < intep_num; j++)
        {
            if(intep_pos[i] > intep_pos[j])
            {
                int temp;
                temp            = intep_pos[i];
                intep_pos[i]    = intep_pos[j];
                intep_pos[j]    = temp;

            }
        }

	intep_pos[intep_num++]		= range;


#if 1
    for (i = 1; i < intep_num; i++)
    {
        if(i == 1)
            intep_pos[i]        = MFNR_ROUND_F(intep_pos[i] / (step_x1 * 4))  * (4   * step_x1);
        else
            intep_pos[i]        = MFNR_ROUND_F(intep_pos[i] / (step_x1 * 16)) * (16  * step_x1);
    }
	memset(intep_pos_flg, 0, sizeof(intep_pos_flg));
    for (i = 1; i < intep_num; i++)
    {
		if(intep_pos[i - 1] == intep_pos[i])
			intep_pos_flg[i] = 1;
    }
    for (int i = 0, j = 0; i < intep_num; i++)
    {
		if(intep_pos_flg[i] == 0)
		{
			intep_pos[j] = intep_pos[i];
			j++;
		}
    }
	intep_num = j;


    int off = 0;
    int st;
    int idx;
    int off4    = (0x4 * step_x1);
    int off8    = (0x8 * step_x1);
    int off16   = (0x10 * step_x1);
    int off32   = (0x20 * step_x1);
    for (int i = 0, idx = 1; i < MAX_INTEPORATATION_LUMAPOINT - 1; i++)
    {
		int cur_pos;
		if(idx >= intep_num&&off>=range)
		{

			fix_x_pos[intep_num_1++] = range;
			continue;
		}
		cur_pos                     = intep_pos[idx];
        fix_x_pos[intep_num_1++]  = off;

        if(off & off4)
            st = off4;
        else if(off & off8)
            st = off8;
        else if((off < 64 * step_x1) || (off & off16))
            st = off16;
        else
            st = off32;
        if(off + st > cur_pos && off != cur_pos)
        {
            if((cur_pos - off)  & off4)
                st = off4;
            else if((cur_pos - off)  & off8)
                st = off8;
            else if((cur_pos - off)  & off16)
                st = off16;
			else
                st = off32;
        }
        if(off + st == cur_pos)
            idx++;

        off+=st;

    }
	fix_x_pos[intep_num_1++]		= range;




    for (i = 0; i < range; i+= step_x)
        stmfnrParamsSelected->noise_sigma[i/step_x]             = noise_sigma_tmp[i]                    / step_y;

    for (i = 0; i < MAX_INTEPORATATION_LUMAPOINT; i++)
        stmfnrParamsSelected->noise_sigma_sample[i]             = noise_sigma_tmp[fix_x_pos[i]]       / step_y;
    for (i = 0; i < MAX_INTEPORATATION_LUMAPOINT; i++)
        stmfnrParamsSelected->fix_x_pos[i]                    = fix_x_pos[i]                        / step_x;


    for (i = 0; i < MAX_INTEPORATATION_LUMAPOINT; i++)
        stmfnrParamsSelected->noise_sigma_dehaze[i]             = noise_sigma_tmp[fix_x_pos[i]]       / step_y;
    for (i = 0; i < MAX_INTEPORATATION_LUMAPOINT; i++)
        stmfnrParamsSelected->fix_x_pos_dehaze[i]               = fix_x_pos[i]                        / step_x_dehz;


#else
    for (i = 0; i < range; i+= step)
        stmfnrParamsSelected->noise_sigma[i/step]   = noise_sigma_tmp[i] / step;

    int gap_num[POLT_ORDER] = {6, 6, 2, 2};
    int gap_piece = 8;
	int gap_rat[POLT_ORDER][5] = {{1,2,4,6,7},{1,2,4,6,7},{4,8,4,4,4},{8,8,8,8,8}};

    for (i = 0; i < intep_num - 1; i++)
    {
        int left_pos                        = intep_pos[i];
        int right_pos                       = intep_pos[i + 1];
        int gap_num_cur                     = gap_num[i];
        intep_pos_1[intep_num_1++]          = intep_pos[i];
        for (j = 0; j < gap_num_cur - 1; j++)
        {
            intep_pos_1[intep_num_1++]      = ceil(left_pos + (double)(right_pos-left_pos) * gap_rat[i][j] / gap_piece);
        }
    }

    for (int idx = intep_num_1; idx < MAX_INTEPORATATION_LUMAPOINT - 1; idx++)
    {
        intep_pos_1[idx]              = intep_pos[intep_num - 1];
    }

	intep_pos_1[MAX_INTEPORATATION_LUMAPOINT - 1]					= range;

    for (int idx = 0; idx < MAX_INTEPORATATION_LUMAPOINT; idx++)
    {
        intep_pos_1[idx]              = ROUND_F((double)intep_pos_1[idx] / step) * step;
    }


    for (int idx = 0; idx < intep_num; idx++)
    {
        intep_pos[idx]              = ROUND_F((double)intep_pos[idx] / step) * step;
    }


    for (i = 0; i < intep_num_1 - 1; i++)
    {
        int left_pos  = intep_pos_1[i] ;
        int right_pos = intep_pos_1[i + 1];
        double left_val  = noise_sigma_tmp[intep_pos_1[i]];
        double right_val = noise_sigma_tmp[intep_pos_1[i + 1]];
        for(j = left_pos; j < right_pos; j++)
        {
			if(j==112)
				j=j;
			if(j != left_pos && j != right_pos)
				noise_sigma_tmp[j] = (left_val * (right_pos - j) + right_val * (j - left_pos)) / (right_pos - left_pos);
        }
    }

    for (i = 0; i < range; i+= step)
        stmfnrParamsSelected->noise_sigma_intep[i/step]         = noise_sigma_tmp[i]                    / step;

    for (i = 0; i < MAX_INTEPORATATION_LUMAPOINT; i++)
        stmfnrParamsSelected->intep_pos_1[i]                    = intep_pos_1[i]                        / step;

    for (i = 0; i < MAX_INTEPORATATION_LUMAPOINT; i++)
        stmfnrParamsSelected->noise_sigma_dehaze[i]             = noise_sigma_tmp[intep_pos_1[i] ]      / step;

#endif

//for (i = 0; i < range; i++)
//    noise_sigma_tmp[i]                          = ROUND_F(noise_sigma_tmp[i] * (1 << 12)) >>  ;

	#if MFNR_FIX_VALUE_PRINTF
	mfnr_fix_transfer(stmfnrParamsSelected);
	#endif
}


#define ROUND_D(x) (long)(((double)x)+(((x) > 0) ? 0.5 : (-0.5)))
#define	FLOOR_INT64(a) (long)( ((double)(a) < (long)(a)) ? (long)((a)-1) : (long)(a) )

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



template<typename T1, typename T2>
T1 FIX_FLOAT(T1 data0,  int bits, T2 &dst, int flag = 0)
{
    if(flag == 0)
        dst = ROUND_D(data0 * (1 << bits));
    else
        dst = FLOOR_INT64(data0 * (1 << bits));
    return  ((T1)dst / (1 << bits));

}

int get_matrix_idx(int i, int j, int rad)
{
    int src_i = 0;
    int i_act, j_act;
    if(i > rad)
        i_act = 2 * rad - i;
    else
        i_act = i;

    if(j > rad)
        j_act = 2 * rad - j;
    else
        j_act = j;
    if(j_act < i_act)
    {
        int tmp;
        tmp = j_act;
        j_act = i_act;
        i_act = tmp;
    }
    for(int ii = rad; ii >= 0; ii--)
        for(int jj = rad; jj >= ii; jj--)
        {
            if(i_act == ii && j_act == jj)
                return src_i;
            src_i++;
        }
}


void mfnr_gfcoef_fix(int rad, double *gfcoef, unsigned char* gfcoef_fix)
{
	double sum_d2;
    double *h;
	unsigned long tmp;
	
    h = (double*)malloc((rad * 2 + 1) * (rad * 2 + 1) * sizeof(double));
    sum_d2 = 0;
    for(int i = 0; i <  (rad * 2 + 1); i++){
        for(int j = 0; j < (rad * 2 + 1); j++){
            int src_i = get_matrix_idx(i, j, rad);
            h[i * (rad * 2 + 1) + j] = FIX_FLOAT((double)gfcoef[src_i], F_DECI_PIXEL_SIGMA_CONV_WEIGHT, tmp);
			gfcoef_fix[src_i] = tmp;
            sum_d2 += h[i * (rad * 2 + 1) + j];
        }
    }
    int idx = ((rad * 2 + 1) * rad + rad);
    h[idx]  = FIX_FLOAT(h[idx] + (1 - sum_d2), F_DECI_PIXEL_SIGMA_CONV_WEIGHT, tmp);
	gfcoef_fix[0] = tmp;
    free(h);
	
}


void mfnr_fix_transfer(RKAnr_Mfnr_Params_Select_t* tnr)
{
	printf("%s:(%d) enter \n", __FUNCTION__, __LINE__);
	rkispp_mfnr_fix_t *pMfnrCfg = (rkispp_mfnr_fix_t *)malloc(sizeof(rkispp_mfnr_fix_t));
	
	int i = 0;
	unsigned long tmp = 0;
	int mIso_last = 50;
	int mIso = 50;	
	double gain_glb_filt;
    double gain_glb_ref1;
    double gain_glb_filt_sqrt;
    double gain_glb_filt_sqrt_inv;
    double scale_l_y				[MFNR_DIR_NUM][MFNR_MAX_LVL];
    double scale_l_y_uv				[MFNR_DIR_NUM][MFNR_MAX_LVL_UV];
    double scale_l_uv				[MFNR_DIR_NUM][MFNR_MAX_LVL_UV];
	double ratio_frq_sub_div		[2];
    int max_lvl         = MFNR_MAX_LVL;
    int max_lvl_uv      = MFNR_MAX_LVL_UV;
    int dir_num         = MFNR_DIR_NUM;
	

	int rad_isp20[2][5]                = {{2, 1, 1, 1, 1}, {2, 1, 1, 1, 1}};
    int rad_uv_isp20[2][5]             = {{2, 1, 1, 1, 1}, {2, 1, 1, 1, 1}};
    gain_glb_filt                           = (mIso_last / 50);
    gain_glb_ref1                           = (mIso / 50);//ref1
    gain_glb_filt_sqrt                      = sqrt(gain_glb_filt);
    gain_glb_filt_sqrt_inv                  = 1 / sqrt(gain_glb_filt);

	for(int dir_idx = 0; dir_idx < dir_num; dir_idx++){
		for(int lvl = 0; lvl < max_lvl; lvl++)
			scale_l_y[dir_idx][lvl] = tnr->ci[dir_idx][lvl] * tnr->scale[dir_idx][lvl] / tnr->dnstr[dir_idx];
	}
	for(int dir_idx = 0; dir_idx < dir_num; dir_idx++){
		for(int lvl = 0; lvl < max_lvl_uv; lvl++){
			scale_l_y_uv[dir_idx][lvl] = tnr->ci[dir_idx][lvl] * tnr->scale_uv[dir_idx][lvl] / tnr->dnstr_uv[dir_idx];
			scale_l_uv[dir_idx][lvl] = tnr->ci_uv[dir_idx][lvl] * tnr->scale_uv[dir_idx][lvl] / tnr->dnstr_uv[dir_idx];
		}
	}

	//0x0080
	pMfnrCfg->opty_en = 1;
	pMfnrCfg->optc_en = 1;
	pMfnrCfg->gain_en = 1;

	//0x0088
	pMfnrCfg->pk0_y = tnr->weight_limit_y[0];
	pMfnrCfg->pk1_y = tnr->weight_limit_y[max_lvl - 1];
	pMfnrCfg->pk0_c = tnr->weight_limit_uv[0];
	pMfnrCfg->pk1_c = tnr->weight_limit_uv[max_lvl_uv - 1];
		
	//0x008c
	FIX_FLOAT(gain_glb_filt, F_DECI_GAIN, tmp, 0);
	pMfnrCfg->glb_gain_cur = tmp;
    FIX_FLOAT(gain_glb_ref1, F_DECI_GAIN, tmp);
	pMfnrCfg->glb_gain_nxt = tmp;

	//0x0090
	FIX_FLOAT(gain_glb_filt_sqrt_inv, F_DECI_GAIN_GLB_SQRT_INV, tmp);
	pMfnrCfg->glb_gain_cur_div = tmp;
	FIX_FLOAT(gain_glb_filt_sqrt, F_DECI_GAIN_GLB_SQRT, tmp);
	pMfnrCfg->glb_gain_cur_sqrt = tmp;
	
	//0x0094 - 0x0098
	for(i=0; i<17 - 1; i++){
		pMfnrCfg->sigma_x[i] = log((double)(tnr->fix_x_pos[i + 1] - tnr->fix_x_pos[i])) / log((double)2) - 2;	
	}

	//0x009c - 0x00bc
	for(i=0; i<17; i++){
		FIX_FLOAT(tnr->noise_sigma_sample[i], F_DECI_SIGMA, tmp);
		pMfnrCfg->sigma_y[i] = tmp;
	}

	//0x00c4 - 0x00cc
	//dir_idx = 0;
	for(i=0; i<6; i++){
		FIX_FLOAT(tnr->lumanrcurve[0][i], F_DECI_LUMASCALE, tmp);
		pMfnrCfg->luma_curve[i] = tmp;
	}

	//0x00d0
	FIX_FLOAT(tnr->ratio_frq[0], F_DECI_TXT_THRD_RATIO, tmp);
	pMfnrCfg->txt_th0_y = tmp;
	FIX_FLOAT(tnr->ratio_frq[1], F_DECI_TXT_THRD_RATIO, tmp);	
	pMfnrCfg->txt_th1_y = tmp;
	
	//0x00d4
	FIX_FLOAT(tnr->ratio_frq[2], F_DECI_TXT_THRD_RATIO, tmp);
	pMfnrCfg->txt_th0_c = tmp;
	FIX_FLOAT(tnr->ratio_frq[3], F_DECI_TXT_THRD_RATIO, tmp);
	pMfnrCfg->txt_th1_c = tmp;

	//0x00d8
	ratio_frq_sub_div[0] = 1 / (tnr->ratio_frq[1] - tnr->ratio_frq[0]);
    ratio_frq_sub_div[1] = 1 / (tnr->ratio_frq[3] - tnr->ratio_frq[2]);	
	FIX_FLOAT(ratio_frq_sub_div[0], F_DECI_TXT_THRD_RATIO, tmp, 1);
	pMfnrCfg->txt_thy_dlt = tmp;
	FIX_FLOAT(ratio_frq_sub_div[1], F_DECI_TXT_THRD_RATIO, tmp, 1);
	pMfnrCfg->txt_thc_dlt = tmp;

	//0x00dc - 0x00ec
	//dir_idx = 0; lvl 0-3;
	//rad_isp20[dir][levl];   gfsigma[lvl]  
	mfnr_gfcoef_fix(rad_isp20[0][0], tnr->gfsigma[0], pMfnrCfg->gfcoef_y0);
	mfnr_gfcoef_fix(rad_isp20[0][1], tnr->gfsigma[1], pMfnrCfg->gfcoef_y1);
	mfnr_gfcoef_fix(rad_isp20[0][2], tnr->gfsigma[2], pMfnrCfg->gfcoef_y2);
	mfnr_gfcoef_fix(rad_isp20[0][3], tnr->gfsigma[3], pMfnrCfg->gfcoef_y3);

	//0x00f0 - 0x0100
	//dir_idx = 0; lvl 0-3;
	//rad_isp20[dir][levl];   gfdelta[[dir_idx][lvl]   
	mfnr_gfcoef_fix(rad_isp20[0][0], tnr->gfdelta[0][0], pMfnrCfg->gfcoef_yg0);
	mfnr_gfcoef_fix(rad_isp20[0][1], tnr->gfdelta[0][1], pMfnrCfg->gfcoef_yg1);
	mfnr_gfcoef_fix(rad_isp20[0][2], tnr->gfdelta[0][2], pMfnrCfg->gfcoef_yg2);
	mfnr_gfcoef_fix(rad_isp20[0][3], tnr->gfdelta[0][3], pMfnrCfg->gfcoef_yg3);

	//0x0104 - 0x0110
	//dir_idx = 1; lvl 0-3;
	//rad_isp20[dir][levl];   gfdelta[[dir_idx][lvl]  ;   
	mfnr_gfcoef_fix(rad_isp20[1][0], tnr->gfdelta[1][0], pMfnrCfg->gfcoef_yl0);
	mfnr_gfcoef_fix(rad_isp20[1][1], tnr->gfdelta[1][1], pMfnrCfg->gfcoef_yl1);
	mfnr_gfcoef_fix(rad_isp20[1][2], tnr->gfdelta[1][2], pMfnrCfg->gfcoef_yl2);

	//0x0114 - 0x0120
	//dir_idx = 0; lvl 0-2;
	//rad_isp20[dir][levl];   gfdelta_uv[[dir_idx][lvl]  ;   
	mfnr_gfcoef_fix(rad_uv_isp20[0][0], tnr->gfdelta_uv[0][0], pMfnrCfg->gfcoef_cg0);
	mfnr_gfcoef_fix(rad_uv_isp20[0][1], tnr->gfdelta_uv[0][1], pMfnrCfg->gfcoef_cg1);
	mfnr_gfcoef_fix(rad_uv_isp20[0][2], tnr->gfdelta_uv[0][2], pMfnrCfg->gfcoef_cg2);

	//0x0124 - 0x012c
	//dir_idx = 1; lvl 0-1;
	//rad_isp20[dir][levl];   gfdelta_uv[[dir_idx][lvl]  ;   
	mfnr_gfcoef_fix(rad_uv_isp20[1][0], tnr->gfdelta_uv[1][0], pMfnrCfg->gfcoef_cl0);
	mfnr_gfcoef_fix(rad_uv_isp20[1][1], tnr->gfdelta_uv[1][1], pMfnrCfg->gfcoef_cl1);

	//0x0130 - 0x0134
	//dir_idx = 0;  i = lvl;
	for(i=0; i<4; i++){
		FIX_FLOAT(scale_l_y[0][i], F_DECI_SCALE_L, tmp);	
		pMfnrCfg->scale_yg[i] = tmp;
	}

	//0x0138 - 0x013c
	//dir_idx = 1;  i = lvl;
	for(i=0; i<3; i++){
		FIX_FLOAT(scale_l_y[1][i], F_DECI_SCALE_L, tmp);	
		pMfnrCfg->scale_yl[i] = tmp;
	}

	//0x0140 - 0x0148
	//dir_idx = 0;  i = lvl;
	for(i=0; i<3; i++){
		FIX_FLOAT(scale_l_uv[0][i], F_DECI_SCALE_L_UV, tmp);
		pMfnrCfg->scale_cg[i] = tmp;
		FIX_FLOAT(scale_l_y_uv[0][i], F_DECI_SCALE_L_UV, tmp);
		pMfnrCfg->scale_y2cg[i] = tmp;
	}

	//0x014c - 0x0154
	//dir_idx = 1;  i = lvl;
	for(i=0; i<2; i++){
		FIX_FLOAT(scale_l_uv[1][i], F_DECI_SCALE_L_UV, tmp);
		pMfnrCfg->scale_cl[i] = tmp;
	}
	for(i=0; i<3; i++){
		FIX_FLOAT(scale_l_y_uv[1][i], F_DECI_SCALE_L_UV, tmp);  
		pMfnrCfg->scale_y2cl[i] = tmp;
	}

	//0x0158 
	for(i=0; i<3; i++){
		FIX_FLOAT(tnr->luma_w_in_chroma[i], F_DECI_LUMA_W_IN_CHROMA, tmp);
		pMfnrCfg->weight_y[i] = tmp;
	}
	
	mfnr_fix_Printf(pMfnrCfg);

	free(pMfnrCfg);
	
	printf("%s:(%d) exit \n", __FUNCTION__, __LINE__);
}



void mfnr_fix_Printf(rkispp_mfnr_fix_t  * pMfnrCfg)
{
	int i = 0;
	printf("%s:(%d) enter \n", __FUNCTION__, __LINE__);
	//0x0080
	printf("(0x0080) opty_en:%d optc_en:%d gain_en:%d\n", 
		pMfnrCfg->opty_en,
		pMfnrCfg->optc_en,
		pMfnrCfg->gain_en);

	//0x0088
	printf("(0x0088) pk0_y:%d pk1_y:%d pk0_c:%d pk1_c:%d \n", 
		pMfnrCfg->pk0_y,
		pMfnrCfg->pk1_y,
		pMfnrCfg->pk0_c,
		pMfnrCfg->pk1_c);
		
	//0x008c
	printf("(0x008c) glb_gain_cur:%d glb_gain_nxt:%d \n", 
		pMfnrCfg->glb_gain_cur,
		pMfnrCfg->glb_gain_nxt);

	//0x0090
	printf("(0x0090) glb_gain_cur_div:%d gain_glb_filt_sqrt:%d \n", 
		pMfnrCfg->glb_gain_cur_div,
		pMfnrCfg->glb_gain_cur_sqrt);
	
	//0x0094 - 0x0098
	for(i=0; i<17 - 1; i++){
		printf("(0x0094 - 0x0098) sigma_x[%d]:%d \n", 
			i, pMfnrCfg->sigma_x[i]);
	}

	//0x009c - 0x00bc
	for(i=0; i<17; i++){
		printf("(0x009c - 0x00bc) sigma_y[%d]:%d \n", 
			i, pMfnrCfg->sigma_y[i]);
	}

	//0x00c4 - 0x00cc
	//dir_idx = 0;
	for(i=0; i<6; i++){
		printf("(0x00c4 - 0x00cc) luma_curve[%d]:%d \n", 
			i, pMfnrCfg->luma_curve[i]);
	}

	//0x00d0
	printf("(0x00d0) txt_th0_y:%d txt_th1_y:%d \n", 
		pMfnrCfg->txt_th0_y,
		pMfnrCfg->txt_th1_y);
	
	//0x00d4
	printf("(0x00d0) txt_th0_c:%d txt_th1_c:%d \n", 
		pMfnrCfg->txt_th0_c,
		pMfnrCfg->txt_th1_c);

	//0x00d8
	printf("(0x00d8) txt_thy_dlt:%d txt_thc_dlt:%d \n", 
		pMfnrCfg->txt_thy_dlt,
		pMfnrCfg->txt_thc_dlt);

	//0x00dc - 0x00ec
	for(i=0; i<6; i++){
		printf("(0x00dc - 0x00ec) gfcoef_y0[%d]:%d \n", 
			i, pMfnrCfg->gfcoef_y0[i]);
	}
	for(i=0; i<3; i++){
		printf("(0x00dc - 0x00ec) gfcoef_y1[%d]:%d \n", 
			i, pMfnrCfg->gfcoef_y1[i]);
	}
	for(i=0; i<3; i++){
		printf("(0x00dc - 0x00ec) gfcoef_y2[%d]:%d \n", 
			i, pMfnrCfg->gfcoef_y2[i]);
	}
	for(i=0; i<3; i++){
		printf("(0x00dc - 0x00ec) gfcoef_y3[%d]:%d \n", 
			i, pMfnrCfg->gfcoef_y3[i]);
	}

	//0x00f0 - 0x0100
	for(i=0; i<6; i++){
		printf("(0x00f0 - 0x0100) gfcoef_yg0[%d]:%d \n", 
			i, pMfnrCfg->gfcoef_yg0[i]);
	}
	for(i=0; i<3; i++){
		printf("(0x00f0 - 0x0100) gfcoef_yg1[%d]:%d \n", 
			i, pMfnrCfg->gfcoef_yg1[i]);
	}
	for(i=0; i<3; i++){
		printf("(0x00f0 - 0x0100) gfcoef_yg2[%d]:%d \n", 
			i, pMfnrCfg->gfcoef_yg2[i]);
	}
	for(i=0; i<3; i++){
		printf("(0x00f0 - 0x0100) gfcoef_yg3[%d]:%d \n", 
			i, pMfnrCfg->gfcoef_yg3[i]);
	}


	//0x0104 - 0x0110
	for(i=0; i<6; i++){
		printf("(0x0104 - 0x0110) gfcoef_yl0[%d]:%d \n", 
			i, pMfnrCfg->gfcoef_yl0[i]);
	}
	for(i=0; i<3; i++){
		printf("(0x0104 - 0x0110) gfcoef_yl1[%d]:%d \n", 
			i, pMfnrCfg->gfcoef_yl1[i]);
	}
	for(i=0; i<3; i++){
		printf("(0x0104 - 0x0110) gfcoef_yl2[%d]:%d \n", 
			i, pMfnrCfg->gfcoef_yl2[i]);
	}

	//0x0114 - 0x0120
	for(i=0; i<6; i++){
		printf("(0x0114 - 0x0120) gfcoef_cg0[%d]:%d \n", 
			i, pMfnrCfg->gfcoef_cg0[i]);
	}
	for(i=0; i<3; i++){
		printf("(0x0114 - 0x0120) gfcoef_cg1[%d]:%d \n", 
			i, pMfnrCfg->gfcoef_cg1[i]);
	}
	for(i=0; i<3; i++){
		printf("(0x0114 - 0x0120) gfcoef_cg2[%d]:%d \n", 
			i, pMfnrCfg->gfcoef_cg2[i]);
	}


	//0x0124 - 0x012c
	for(i=0; i<6; i++){
		printf("(0x0124 - 0x012c) gfcoef_cl0[%d]:%d \n", 
			i, pMfnrCfg->gfcoef_cl0[i]);
	}
	for(i=0; i<3; i++){
		printf("(0x0124 - 0x012c) gfcoef_cl1[%d]:%d \n", 
			i, pMfnrCfg->gfcoef_cl1[i]);
	}


	//0x0130 - 0x0134
	//dir_idx = 0;  i = lvl;
	for(i=0; i<4; i++){
		printf("(0x0130 - 0x0134) scale_yg[%d]:%d \n", 
			i, pMfnrCfg->scale_yg[i]);
	}

	//0x0138 - 0x013c
	//dir_idx = 1;  i = lvl;
	for(i=0; i<3; i++){
		printf("(0x0138 - 0x013c) scale_yl[%d]:%d \n", 
			i, pMfnrCfg->scale_yl[i]);
	}

	//0x0140 - 0x0148
	//dir_idx = 0;  i = lvl;
	for(i=0; i<3; i++){
		printf("(0x0140 - 0x0148) scale_cg[%d]:%d \n", 
			i, pMfnrCfg->scale_cg[i]);
		printf("(0x0140 - 0x0148) scale_y2cg[%d]:%d \n", 
			i, pMfnrCfg->scale_y2cg[i]);	
	}

	//0x014c - 0x0154
	//dir_idx = 1;  i = lvl;
	for(i=0; i<2; i++){
		printf("(0x014c - 0x0154) scale_cl[%d]:%d \n", 
			i, pMfnrCfg->scale_cl[i]);
	}
	for(i=0; i<3; i++){
		printf("(0x014c - 0x0154) scale_y2cl[%d]:%d \n", 
			i, pMfnrCfg->scale_y2cl[i]);
	}

	//0x0158 
	for(i=0; i<3; i++){
		printf("(0x0158) weight_y[%d]:%d \n", 
			i, pMfnrCfg->weight_y[i]);
	}

	printf("%s:(%d) exit \n", __FUNCTION__, __LINE__);
}


