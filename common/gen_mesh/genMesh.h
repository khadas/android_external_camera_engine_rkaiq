#ifndef __GENMESH_H__
#define __GENMESH_H__


/* 相机参数 */
struct CameraCoeff
{
	double cx, cy;							// 镜头的光心
	double a0, a2, a3, a4;					// 镜头的畸变系数
	double c, d, e;							// 内参[c d;e 1]
	double sf;								// sf控制视角，sf越大视角越大
	/* level = 0时的rho-theta多项式拟合 */
	int invPolyNum0;						// 系数个数
	double invPolyCoeff0[21];				// 多项式系数，最高次数20次
	/* level = 255时的rho-theta多项式拟合 */
	int invPolyNum255;						// 系数个数
	double invPolyCoeff255[21];				// 多项式系数，最高次数20次
};

/* 生成FEC映射表相关的参数 */
struct FecParams
{
	int srcW, srcH, dstW, dstH;							/* 输入输出图像的分辨率 */
	int srcW_ex, srcH_ex, dstW_ex, dstH_ex;				/* 扩展后的输入输出分辨率 */
	int meshSizeW, meshSizeH, meshStepW, meshStepH;
	int meshSize1bin;
	int meshSize4bin;
	unsigned short	SpbNum;
	unsigned long	MeshPointNumW;
	unsigned short	SpbMeshPNumH;
	unsigned short	LastSpbMeshPNumH;

	double *mapx;
	double *mapy;
	unsigned short	*pMeshXY;

};

/* 生成LDCH映射表相关的参数 */
struct LdchParams
{
	int srcW, srcH, dstW, dstH;
	int meshSizeW, meshSizeH;
	double meshStepW, meshStepH;
	int meshSize;

	double *mapx;
	double *mapy;
};


/* FEC: 初始化，根据图像输出分辨率，计算FEC映射表的相关参数，申请需要的buffer */
void genFecMeshInit(int srcW, int srcH, int dstW, int dstH, FecParams &fecParams, CameraCoeff &camCoeff);

/* FEC: 反初始化 */
void genFecMeshDeInit(FecParams &fecParams);

/* FEC: 预先计算的部分: 浮点未校正的小表和level=0,level=255的多项式参数 */
void genFecPreCalcPart(FecParams &fecParams, CameraCoeff &camCoeff);

/* FEC: 4个mesh 内存申请 */
void mallocFecMesh(int meshSize, unsigned short **pMeshXI, unsigned char **pMeshXF, unsigned short **pMeshYI, unsigned char **pMeshYF);

/* FEC: 4个mesh 内存释放 */
void freeFecMesh(unsigned short *pMeshXI, unsigned char *pMeshXF, unsigned short *pMeshYI, unsigned char *pMeshYF);

/*
函数功能: 生成不同校正程度的mesh映射表，用于ISP的FEC模块
	输入:
	1）FEC映射表的相关参数，申请需要的buffer: FecParams &fecParams
	2）相机标定参数: CameraCoeff &camCoeff
	3）需要校正的程度: level(0-255: 0表示校正程度为0%, 255表示校正程度为100%)
	输出:
	1）bool 是否成功生成
	2）pMeshXI, pMeshXF, pMeshYI, pMeshYF
*/
bool genFECMeshNLevel(FecParams &fecParams, CameraCoeff &camCoeff, int level, unsigned short *pMeshXI, unsigned char *pMeshXF, unsigned short *pMeshYI, unsigned char *pMeshYF);


/* =============================================================================================================================================================================== */

/* LDCH: 初始化，根据图像输出分辨率，计算LDCH映射表的相关参数，申请需要的buffer */
void genLdchMeshInit(int srcW, int srcH, int dstW, int dstH, LdchParams &ldchParams, CameraCoeff &camCoeff);

/* LDCH: 反初始化 */
void genLdchMeshDeInit(LdchParams &ldchParams);

/* LDCH: 预先计算的部分: 浮点未校正的小表和level=0,level=255的多项式参数 */
void genLdchPreCalcPart(LdchParams &ldchParams, CameraCoeff &camCoeff);

/*
函数功能: 生成不同校正程度的mesh映射表，用于ISP的LDCH模块

	输入:
	1）LDCH映射表的相关参数，申请需要的buffer: LdchParams &ldchParams
	2）相机标定参数: CameraCoeff &camCoeff
	3）需要校正的程度: level(0-255: 0表示校正程度为0%, 255表示校正程度为100%)
	输出:
	1）bool 是否成功生成
	2）pMeshX
*/
bool genLDCMeshNLevel(LdchParams &ldchParams, CameraCoeff &camCoeff, int level, unsigned short *pMeshX);

#endif // !__GENMESH_H__
