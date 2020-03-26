#include "rk_aiq_anr_algo_uvnr.h"

RKAIQ_BEGIN_DECLARE



ANRresult_t init_uvnr_params(RKAnr_Uvnr_Params_t *pParams, CalibDb_UVNR_t *pCalibdb)
{
	ANRresult_t res = ANR_RET_SUCCESS;
	int i = 0;
	int j = 0;

	if(pParams == NULL){
		LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ANR_RET_NULL_POINTER;
	}

	if(pCalibdb == NULL){
		LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ANR_RET_NULL_POINTER;
	}

	for(i=0; i<MAX_ISO_STEP; i++){
		pParams->ratio[i] = pCalibdb->step0_uvgrad_ratio[i];
		pParams->offset[i] = pCalibdb->step0_uvgrad_offset[i];
		
		pParams->wStep1[i] = pCalibdb->step1_downSample_w[i];
		pParams->hStep1[i] = pCalibdb->step1_downSample_h[i];
		pParams->meanSize1[i] = pCalibdb->step1_downSample_meansize[i];
		
		pParams->medSize1[i] = pCalibdb->step1_median_size[i];
		pParams->medRatio1[i] = pCalibdb->step1_median_ratio[i];
		pParams->isMedIIR1[i] = pCalibdb->step1_median_IIR[i];

		pParams->bfSize1[i] = pCalibdb->step1_bf_size[i];
		pParams->sigmaR1[i] = pCalibdb->step1_bf_sigmaR[i];
		pParams->sigmaD1[i] = pCalibdb->step1_bf_sigmaD[i];
		pParams->uvgain1[i] = pCalibdb->step1_bf_uvgain[i];
		pParams->bfRatio1[i] = pCalibdb->step1_bf_ratio[i];
		pParams->isRowIIR1[i] = pCalibdb->step1_bf_isRowIIR[i];
		pParams->isYcopy1[i] = pCalibdb->step1_bf_isYcopy[i];		
		
		pParams->wStep2[i] = pCalibdb->step2_downSample_w[i];
		pParams->hStep2[i] = pCalibdb->step2_downSample_h[i];
		pParams->meanSize2[i] = pCalibdb->step2_downSample_meansize[i];	
		
		pParams->medSize2[i] = pCalibdb->step2_median_size[i];
		pParams->medRatio2[i] = pCalibdb->step2_median_ratio[i];
		pParams->isMedIIR2[i] = pCalibdb->step2_median_IIR[i];
		
		pParams->bfSize3[i] = pCalibdb->step2_bf_size[i];
		pParams->sigmaR2[i] = pCalibdb->step2_bf_sigmaR[i];
		pParams->sigmaD2[i] = pCalibdb->step2_bf_sigmaD[i];
		pParams->uvgain2[i] = pCalibdb->step2_bf_uvgain[i];
		pParams->bfRatio2[i] = pCalibdb->step2_bf_ratio[i];
		pParams->isRowIIR2[i] = pCalibdb->step2_bf_isRowIIR[i];
		pParams->isYcopy2[i] = pCalibdb->step2_bf_isYcopy[i];
		
		pParams->bfSize3[i] = pCalibdb->step3_bf_size[i];
		pParams->sigmaR3[i] = pCalibdb->step3_bf_sigmaR[i];
		pParams->sigmaD3[i] = pCalibdb->step3_bf_sigmaD[i];
		pParams->uvgain3[i] = pCalibdb->step3_bf_uvgain[i];
		pParams->bfRatio3[i] = pCalibdb->step3_bf_ratio[i];
		pParams->isRowIIR3[i] = pCalibdb->step3_bf_isRowIIR[i];
		pParams->isYcopy3[i] = pCalibdb->step3_bf_isYcopy[i];

		pParams->sigmaAdj_x[i] = pCalibdb->sigma_adj_luma[i];
		pParams->sigamAdj_y[i] = pCalibdb->sigma_adj_ratio[i];

		pParams->threAdj_x[i] = pCalibdb->threshold_adj_luma[i];
		pParams->threAjd_y[i] = pCalibdb->threshold_adj_thre[i];
		
		
	}

	for(i=0; i<4; i++){
		pParams->nonMed1[i] = pCalibdb->step1_nonMed1[i];
		pParams->nonBf1[i] = pCalibdb->step1_nonBf1[i];
		pParams->block2_ext[i] = pCalibdb->step2_nonExt_block[i];
		pParams->nonMed2[i] = pCalibdb->step2_nonMed[i];
		pParams->nonBf2[i] = pCalibdb->step2_nonBf[i];
		pParams->nonBf3[i] = pCalibdb->step3_nonBf3[i];
	}

	for(i=0; i<3; i++){
		pParams->kernel_3x3_table[i] = pCalibdb->kernel_3x3[i];
	}

	for(i=0; i<5; i++){
		pParams->kernel_5x5_talbe[i] = pCalibdb->kernel_5x5[i];
	}

	for(i=0; i<8; i++){
		pParams->kernel_9x9_table[i] = pCalibdb->kernel_9x9[i];
	}

	pParams->kernel_9x9_num = pCalibdb->kernel_9x9_num;

	return ANR_RET_SUCCESS;
	
}


float interpISO(int ISO_low, int ISO_high, float value_low, float value_high, int ISO, float value)
{
	if (ISO <= ISO_low)
	{
		value = value_low;
	}
	else if (ISO >= ISO_high)
	{
		value = value_high;
	}
	else
	{
		value = float(ISO - ISO_low) / float(ISO_high - ISO_low)*(value_high - value_low) + value_low;
	}
	return value;
}


ANRresult_t select_uvnr_params_by_ISO(RKAnr_Uvnr_Params_t *stRKUVNrParams, RKAnr_Uvnr_Params_Select_t *stRKUVNrParamsSelected, int iso)
{
	ANRresult_t res = ANR_RET_SUCCESS;

	if(stRKUVNrParams == NULL){
		LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ANR_RET_NULL_POINTER;
	}

	if(stRKUVNrParamsSelected == NULL){
		LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ANR_RET_NULL_POINTER;
	}
	
	//确定iso等级
	//rkuvnriso@50 100 200 400 800 1600 3200  6400 12800
	//		isogain: 1  2   4   8   16  32   64    128  256
	//	   isoindex: 0  1   2   3   4   5    6     7    8
	int isoGainStd[9] = { 1,2,4,8,16,32,64,128,256 };
	int ISO = iso / 50;
	if (ISO<1)
	{
		ISO = 1;
	}
	int isoIndex = int(log(float(ISO)) / log(2.0f));
	int isoGainLow = 0;
	int isoGainHigh = 0;
	int isoIndexLow = 0;
	int isoIndexHigh = 0;
	if (ISO <= isoGainStd[0])
	{
		isoGainLow = isoGainStd[0];
		isoGainHigh = isoGainStd[0];
		isoIndexLow = 0;
		isoIndexHigh = 0;
	}
	else if (ISO >= isoGainStd[8])
	{
		isoGainLow = isoGainStd[8];
		isoGainHigh = isoGainStd[8];
		isoIndexLow = 8;
		isoIndexHigh = 8;
	}
	else
	{
		isoGainLow = isoGainStd[isoIndex];
		isoGainHigh = isoGainStd[isoIndex + 1];
		isoIndexLow = isoIndex;
		isoIndexHigh = isoIndex + 1;
	}

	//取数
	memcpy(stRKUVNrParamsSelected->select_iso, stRKUVNrParams->rkuvnrISO, sizeof(char) * 256);
	//step0:uvgain预处理	
	stRKUVNrParamsSelected->ratio = interpISO(isoGainLow, isoGainHigh, stRKUVNrParams->ratio[isoIndexLow],
		stRKUVNrParams->ratio[isoIndexHigh], ISO, stRKUVNrParamsSelected->ratio);
	stRKUVNrParamsSelected->offset = interpISO(isoGainLow, isoGainHigh, stRKUVNrParams->offset[isoIndexLow],
		stRKUVNrParams->offset[isoIndexHigh], ISO, stRKUVNrParamsSelected->offset);
	//step1-下采样1
	//均值1
	stRKUVNrParamsSelected->wStep1 = stRKUVNrParams->wStep1[isoIndex];
	stRKUVNrParamsSelected->hStep1 = stRKUVNrParams->hStep1[isoIndex];
	stRKUVNrParamsSelected->meanSize1 = stRKUVNrParams->meanSize1[isoIndex];
	//中值1
	memcpy(stRKUVNrParamsSelected->nonMed1, stRKUVNrParams->nonMed1, sizeof(int) * 4);
	stRKUVNrParamsSelected->medSize1 = stRKUVNrParams->medSize1[isoIndex];
	stRKUVNrParamsSelected->medRatio1 = interpISO(isoGainLow, isoGainHigh, stRKUVNrParams->medRatio1[isoIndexLow],
		stRKUVNrParams->medRatio1[isoIndexHigh], ISO, stRKUVNrParamsSelected->medRatio1);
	stRKUVNrParamsSelected->isMedIIR1 = stRKUVNrParams->isMedIIR1[isoIndex];
	//双边1
	memcpy(stRKUVNrParamsSelected->nonBf1, stRKUVNrParams->nonBf1, sizeof(int) * 4);
	stRKUVNrParamsSelected->bfSize1 = stRKUVNrParams->bfSize1[isoIndex];
	stRKUVNrParamsSelected->sigmaR1 = interpISO(isoGainLow, isoGainHigh, stRKUVNrParams->sigmaR1[isoIndexLow],
		stRKUVNrParams->sigmaR1[isoIndexHigh], ISO, stRKUVNrParamsSelected->sigmaR1);
	stRKUVNrParamsSelected->sigmaD1 = interpISO(isoGainLow, isoGainHigh, stRKUVNrParams->sigmaD1[isoIndexLow],
		stRKUVNrParams->sigmaD1[isoIndexHigh], ISO, stRKUVNrParamsSelected->sigmaD1);
	stRKUVNrParamsSelected->uvgain1 = interpISO(isoGainLow, isoGainHigh, stRKUVNrParams->uvgain1[isoIndexLow],
		stRKUVNrParams->uvgain1[isoIndexHigh], ISO, stRKUVNrParamsSelected->uvgain1);
	stRKUVNrParamsSelected->bfRatio1 = interpISO(isoGainLow, isoGainHigh, stRKUVNrParams->bfRatio1[isoIndexLow],
		stRKUVNrParams->bfRatio1[isoIndexHigh], ISO, stRKUVNrParamsSelected->bfRatio1);
	stRKUVNrParamsSelected->isRowIIR1 = stRKUVNrParams->isRowIIR1[isoIndex];
	stRKUVNrParamsSelected->isYcopy1 = stRKUVNrParams->isYcopy1[isoIndex];

	//step2-下采样2
	memcpy(stRKUVNrParamsSelected->block2_ext, stRKUVNrParams->block2_ext, sizeof(int) * 4);
	//均值2
	stRKUVNrParamsSelected->wStep2 = stRKUVNrParams->wStep2[isoIndex];
	stRKUVNrParamsSelected->hStep2 = stRKUVNrParams->hStep2[isoIndex];
	stRKUVNrParamsSelected->meanSize2 = stRKUVNrParams->meanSize2[isoIndex];
	//中值2
	memcpy(stRKUVNrParamsSelected->nonMed2, stRKUVNrParams->nonMed2, sizeof(int) * 4);
	stRKUVNrParamsSelected->medSize2 = stRKUVNrParams->medSize2[isoIndex];
	stRKUVNrParamsSelected->medRatio2 = interpISO(isoGainLow, isoGainHigh, stRKUVNrParams->medRatio2[isoIndexLow],
		stRKUVNrParams->medRatio2[isoIndexHigh], ISO, stRKUVNrParamsSelected->medRatio2);
	stRKUVNrParamsSelected->isMedIIR2 = stRKUVNrParams->isMedIIR2[isoIndex];
	//双边2
	memcpy(stRKUVNrParamsSelected->nonBf2, stRKUVNrParams->nonBf2, sizeof(int) * 4);
	stRKUVNrParamsSelected->bfSize2 = stRKUVNrParams->bfSize2[isoIndex];
	stRKUVNrParamsSelected->sigmaR2 = interpISO(isoGainLow, isoGainHigh, stRKUVNrParams->sigmaR2[isoIndexLow],
		stRKUVNrParams->sigmaR2[isoIndexHigh], ISO, stRKUVNrParamsSelected->sigmaR2);
	stRKUVNrParamsSelected->sigmaD2 = interpISO(isoGainLow, isoGainHigh, stRKUVNrParams->sigmaD2[isoIndexLow],
		stRKUVNrParams->sigmaD2[isoIndexHigh], ISO, stRKUVNrParamsSelected->sigmaD2);
	stRKUVNrParamsSelected->uvgain2 = interpISO(isoGainLow, isoGainHigh, stRKUVNrParams->uvgain2[isoIndexLow],
		stRKUVNrParams->uvgain2[isoIndexHigh], ISO, stRKUVNrParamsSelected->uvgain2);
	stRKUVNrParamsSelected->bfRatio2 = interpISO(isoGainLow, isoGainHigh, stRKUVNrParams->bfRatio2[isoIndexLow],
		stRKUVNrParams->bfRatio2[isoIndexHigh], ISO, stRKUVNrParamsSelected->bfRatio2);
	stRKUVNrParamsSelected->isRowIIR2 = stRKUVNrParams->isRowIIR2[isoIndex];
	stRKUVNrParamsSelected->isYcopy2 = stRKUVNrParams->isYcopy2[isoIndex];

	//step3
	memcpy(stRKUVNrParamsSelected->nonBf3, stRKUVNrParams->nonBf3, sizeof(int) * 4);
	//双边3
	stRKUVNrParamsSelected->bfSize3 = stRKUVNrParams->bfSize3[isoIndex];
	stRKUVNrParamsSelected->sigmaR3 = interpISO(isoGainLow, isoGainHigh, stRKUVNrParams->sigmaR3[isoIndexLow],
		stRKUVNrParams->sigmaR3[isoIndexHigh], ISO, stRKUVNrParamsSelected->sigmaR3);
	stRKUVNrParamsSelected->sigmaD3 = interpISO(isoGainLow, isoGainHigh, stRKUVNrParams->sigmaD3[isoIndexLow],
		stRKUVNrParams->sigmaD3[isoIndexHigh], ISO, stRKUVNrParamsSelected->sigmaD3);
	stRKUVNrParamsSelected->uvgain3 = interpISO(isoGainLow, isoGainHigh, stRKUVNrParams->uvgain3[isoIndexLow],
		stRKUVNrParams->uvgain3[isoIndexHigh], ISO, stRKUVNrParamsSelected->uvgain3);
	stRKUVNrParamsSelected->bfRatio3 = interpISO(isoGainLow, isoGainHigh, stRKUVNrParams->bfRatio3[isoIndexLow],
		stRKUVNrParams->bfRatio3[isoIndexHigh], ISO, stRKUVNrParamsSelected->bfRatio3);
	stRKUVNrParamsSelected->isRowIIR3 = stRKUVNrParams->isRowIIR3[isoIndex];
	stRKUVNrParamsSelected->isYcopy3 = stRKUVNrParams->isYcopy3[isoIndex];

	//kernels
	memcpy(stRKUVNrParamsSelected->kernel_3x3_table, stRKUVNrParams->kernel_3x3_table, sizeof(float) * 3);
	memcpy(stRKUVNrParamsSelected->kernel_5x5_table, stRKUVNrParams->kernel_5x5_talbe, sizeof(float) * 5);
	memcpy(stRKUVNrParamsSelected->kernel_9x9_table, stRKUVNrParams->kernel_9x9_table, sizeof(float) * 8);
	stRKUVNrParamsSelected->kernel_9x9_num = stRKUVNrParams->kernel_9x9_num;

	//curves
	memcpy(stRKUVNrParamsSelected->sigmaAdj_x, stRKUVNrParams->sigmaAdj_x, sizeof(int) * 9);
	memcpy(stRKUVNrParamsSelected->sigmaAdj_y, stRKUVNrParams->sigamAdj_y, sizeof(float) * 9);
	memcpy(stRKUVNrParamsSelected->threAdj_x, stRKUVNrParams->threAdj_x, sizeof(int) * 9);
	memcpy(stRKUVNrParamsSelected->threAdj_y, stRKUVNrParams->threAjd_y, sizeof(int) * 9);

	return ANR_RET_SUCCESS;

}


ANRresult_t uvnr_fix_transfer(RKAnr_Uvnr_Params_Select_t *uvnr, RKAnr_Uvnr_Fix_t *pNrCfg)
{
	LOGI_ANR("%s:(%d) enter \n", __FUNCTION__, __LINE__);

	int i=0;
	ANRresult_t res = ANR_RET_SUCCESS;

	if(uvnr == NULL){
		LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ANR_RET_NULL_POINTER;
	}

	if(pNrCfg == NULL){
		LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ANR_RET_NULL_POINTER;
	}

	//0x0080
	pNrCfg->uvnr_step1_en = 0;
	pNrCfg->uvnr_step2_en = 0;
	pNrCfg->nr_gain_en = 0;
	pNrCfg->uvnr_nobig_en = 0;
	pNrCfg->uvnr_big_en = 0;
	

	//0x0084
	pNrCfg->uvnr_gain_1sigma = (unsigned char)(uvnr->ratio * (1 << RKUVNR_ratio));
	
	//0x0088
	pNrCfg->uvnr_gain_offset = (unsigned char)(uvnr->offset * (1 << RKUVNR_offset));
	
	//0x008c
	pNrCfg->uvnr_gain_uvgain[0] = (unsigned char)(uvnr->uvgain1 * (1 << RKUVNR_uvgain));
	pNrCfg->uvnr_gain_uvgain[1] = (unsigned char)(uvnr->uvgain3 * (1 << RKUVNR_uvgain));
	pNrCfg->uvnr_gain_t2gen = (unsigned char)(uvnr->uvgain2 * (1 << RKUVNR_uvgain));
	// no need set
	//pNrCfg->uvnr_gain_iso = uvnr->bfRatio1;

	//0x0090
	pNrCfg->uvnr_t1gen_m3alpha = (uvnr->medRatio1 * (1 << RKUVNR_medRatio));

	//0x0094
	pNrCfg->uvnr_t1flt_mode = uvnr->kernel_9x9_num;

	//0x0098
	pNrCfg->uvnr_t1flt_msigma = (unsigned short)(uvnr->sigmaR1 * (1 << RKUVNR_sigmaR));

	//0x009c
	pNrCfg->uvnr_t1flt_wtp = (unsigned char)(uvnr->bfRatio1 * (1 << RKUVNR_bfRatio));

	//0x00a0-0x00a4
	for(i=0; i<8; i++){
		pNrCfg->uvnr_t1flt_wtq[i] = (unsigned char)(uvnr->kernel_9x9_table[i] * (1 << RKUVNR_kernels));
	}

	//0x00a8
	pNrCfg->uvnr_t2gen_m3alpha = (unsigned char)(uvnr->medRatio2 * (1 << RKUVNR_medRatio));

	//0x00ac
	pNrCfg->uvnr_t2gen_msigma = (unsigned short)(uvnr->sigmaR2 * (1 << RKUVNR_sigmaR));

	//0x00b0
	pNrCfg->uvnr_t2gen_wtp = (unsigned char)(uvnr->kernel_5x5_table[0] * (1 << RKUVNR_kernels));

	//0x00b4
	for(i=0; i<4; i++){
		pNrCfg->uvnr_t2gen_wtq[i] = (unsigned char)(uvnr->kernel_5x5_table[i+1] * (1 << RKUVNR_kernels));
	}

	//0x00b8
	pNrCfg->uvnr_t2flt_msigma = (unsigned short)(uvnr->sigmaR3 * (1 << RKUVNR_sigmaR));

	//0x00bc
	pNrCfg->uvnr_t2flt_wtp = (unsigned char)(uvnr->bfRatio3 * (1 << RKUVNR_bfRatio));
	for(i=0; i<3; i++){
		pNrCfg->uvnr_t2flt_wt[i] = (unsigned char)(uvnr->kernel_3x3_table[i] * (1 << RKUVNR_kernels));
	}

	#if UVNR_FIX_VALUE_PRINTF
	uvnr_fix_Printf(pNrCfg);
	#endif

	LOGI_ANR("%s:(%d) exit \n", __FUNCTION__, __LINE__);

	return ANR_RET_SUCCESS;
}


ANRresult_t uvnr_fix_Printf(RKAnr_Uvnr_Fix_t  * pNrCfg)
{
	int i = 0;
	printf("%s:(%d) enter \n", __FUNCTION__, __LINE__);

	ANRresult_t res = ANR_RET_SUCCESS;

	if(pNrCfg == NULL){
		LOGE_ANR("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ANR_RET_NULL_POINTER;
	}
	
	//0x0080
	printf("(0x0080) uvnr_step1_en:%d uvnr_step2_en:%d nr_gain_en:%d uvnr_nobig_en:%d uvnr_big_en:%d\n", 
		pNrCfg->uvnr_step1_en,
		pNrCfg->uvnr_step2_en,
		pNrCfg->nr_gain_en ,
		pNrCfg->uvnr_nobig_en,
		pNrCfg->uvnr_big_en);	

	//0x0084
	printf("(0x0084) uvnr_gain_1sigma:%d \n", 
		pNrCfg->uvnr_gain_1sigma);
		
	//0x0088
	printf("(0x0088) uvnr_gain_offset:%d \n", 
		pNrCfg->uvnr_gain_offset);
	
	//0x008c
	printf("(0x008c) uvnr_gain_uvgain:%d uvnr_step2_en:%d uvnr_gain_t2gen:%d uvnr_gain_iso:%d\n", 
		pNrCfg->uvnr_gain_uvgain[0],
		pNrCfg->uvnr_gain_uvgain[1],
		pNrCfg->uvnr_gain_t2gen,
		pNrCfg->uvnr_gain_iso);
		

	//0x0090
	printf("(0x0090) uvnr_t1gen_m3alpha:%d \n", 
		pNrCfg->uvnr_t1gen_m3alpha);

	//0x0094
	printf("(0x0094) uvnr_t1flt_mode:%d \n", 
		pNrCfg->uvnr_t1flt_mode);

	//0x0098
	printf("(0x0098) uvnr_t1flt_msigma:%d \n", 
		pNrCfg->uvnr_t1flt_msigma);

	//0x009c
	printf("(0x009c) uvnr_t1flt_wtp:%d \n", 
		pNrCfg->uvnr_t1flt_wtp);

	//0x00a0-0x00a4
	for(i=0; i<8; i++){
		printf("(0x00a0-0x00a4) uvnr_t1flt_wtq[%d]:%d \n", 
			i, pNrCfg->uvnr_t1flt_wtq[i]);
	}

	//0x00a8
	printf("(0x00a8) uvnr_t2gen_m3alpha:%d \n", 
		pNrCfg->uvnr_t2gen_m3alpha);

	//0x00ac
	printf("(0x00ac) uvnr_t2gen_msigma:%d \n", 
		pNrCfg->uvnr_t2gen_msigma);

	//0x00b0
	printf("(0x00b0) uvnr_t2gen_wtp:%d \n", 
		pNrCfg->uvnr_t2gen_wtp);

	//0x00b4
	for(i=0; i<4; i++){
		printf("(0x00b4) uvnr_t2gen_wtq[%d]:%d \n", 
			i, pNrCfg->uvnr_t2gen_wtq[i]);
	}

	//0x00b8
	printf("(0x00b8) uvnr_t2flt_msigma:%d \n", 
		pNrCfg->uvnr_t2flt_msigma);

	//0x00bc
	printf("(0x00bc) uvnr_t2flt_wtp:%d \n", 
		pNrCfg->uvnr_t2flt_wtp);
	for(i=0; i<3; i++){
		printf("(0x00bc) uvnr_t2flt_wt[%d]:%d \n", 
			i, pNrCfg->uvnr_t2flt_wt[i]);
	}

		
	printf("%s:(%d) exit \n", __FUNCTION__, __LINE__);

	return ANR_RET_SUCCESS;
}

RKAIQ_END_DECLARE




