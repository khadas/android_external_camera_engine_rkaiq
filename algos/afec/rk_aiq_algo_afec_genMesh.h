/*
 * rk_aiq_algo_afec_genmesh.h
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

#ifndef _RK_AIQ_ALGO_AFEC_GENMESH_H_
#define _RK_AIQ_ALGO_AFEC_GENMESH_H_


/* 鱼眼参数 */
struct CameraCoeff
{
	double cx, cy;					// 相机镜头的光心
	double a0, a2, a3, a4;			// 鱼眼镜头的畸变系数
	double c, d, e;					// 内参[c d;e 1]
	double sf;						// sf控制视角，sf越大视角越大
	int invpolNum;					// rho-theta多项式拟合的系数个数
	double invpol[21];				// rho-theta多项式拟合的系数，最高次数20次
	double big_rho[2002];			// 预先生成的tan(theta)与rho的对应表
	double small_rho[2001];			// 预先生成的cot(theta)与rho的对应表
	double Z[5000];
};

/* 根据图像输入输出分辨率，计算FEC校正表的相关参数 */
void calcFecMeshParams(int srcW, int srcH, int dstW, int dstH, int *meshSizeX, int *meshSizeY, int *meshStepW, int *meshStepH);

/* FEC mesh 内存申请 */
unsigned long mallocFecMesh(int srcWidth, int srcHeight, int meshStepW, int meshStepH, unsigned short **pMeshXI, unsigned char **pMeshXF, unsigned short **pMeshYI, unsigned char **pMeshYF);

/* FEC mesh 内存释放 */
void freeFecMesh(unsigned short *pMeshXI, unsigned char *pMeshXF, unsigned short *pMeshYI, unsigned char *pMeshYF);

/*************************************************************
函数功能：生成不同校正程度的meshX表，用于ISP的FEC模块

	输入：
	1）int srcWidth, int srcHeight
	2）相机标定参数:
			camCoeff.cx;
			camCoeff.cy;
			camCoeff.a0;
			camCoeff.a2;
			camCoeff.a3;
			camCoeff.a4;
	3）需要校正的程度
			level(0-255: 0表示校正程度为0%, 255表示校正程度为100%)

	输出：pMeshXI, pMeshXF, pMeshYI, pMeshYF
**************************************************************/
bool genFECMeshNLevel(int srcWidth, int srcHeight, CameraCoeff &camCoeff, int level, unsigned short *pMeshXI, unsigned char *pMeshXF, unsigned short *pMeshYI, unsigned char *pMeshYF);
#endif // !_RK_AIQ_ALGO_AFEC_GENMESH_H_
