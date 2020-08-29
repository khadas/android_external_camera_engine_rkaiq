#include "rk_aiq_algo_afec_genMesh.h"

/* 根据图像输入输出分辨率，计算FEC校正表的相关参数 */
void calcFecMeshParams(int srcW, int srcH, int dstW, int dstH, int *meshSizeX, int *meshSizeY, int *meshStepW, int *meshStepH)
{
	/* 扩展对齐 */
	int dstW_ex = 32 * ((dstW + 31) / 32);
	int dstH_ex = 32 * ((dstH + 31) / 32);
	/* 校正表的步长 */

	if (srcW > 1920) { //32x16
		*meshStepW = 32;
		*meshStepH = 16;
	}
	else { //16x8
		*meshStepW = 16;
		*meshStepH = 8;
	}

	*meshSizeX = (dstW_ex + *meshStepW - 1) / *meshStepW + 1;//modify to mesh alligned to 32x32
	*meshSizeY = (dstH_ex + *meshStepH - 1) / *meshStepH + 1;//modify to mesh alligned to 32x32
}

/* FEC mesh 内存申请 */
unsigned long mallocFecMesh(int srcWidth, int srcHeight, int meshStepW, int meshStepH, unsigned short **pMeshXI, unsigned char **pMeshXF, unsigned short **pMeshYI, unsigned char **pMeshYF)
{
	unsigned short esw_picw = ((srcWidth + 31) / 32) * 32;
	unsigned short esw_pich = ((srcHeight + 31) / 32) * 32;

	// 128/16 * 121(1080P)
	unsigned short ColMeshPNum = (esw_picw + meshStepW - 1) / meshStepW + 1; // 121//modify to mesh alligned to 32x32
	unsigned short RowMeshPNum = (esw_pich + meshStepH - 1) / meshStepH + 1; // 136//modify to mesh alligned to 32x32

	unsigned short SpbMeshPNum = 128 / meshStepH * ColMeshPNum;
	unsigned short	SpbNum;
	unsigned long	MeshPointNumW;
	unsigned short	SpbMeshPNumH;
	unsigned short	LastSpbMeshPNumH;
	unsigned long MeshNumW;
	int LastSpbH;
	SpbNum = (srcHeight + 128 - 1) / 128;
	MeshNumW = esw_picw / meshStepW;
	MeshPointNumW = MeshNumW + 1;
	SpbMeshPNumH = 128 / meshStepH + 1;//16x8 -> 17, 32x16 -> 9
	LastSpbH = (esw_pich % 128 == 0) ? 128 : (esw_pich % 128);//modify to mesh alligned to 32x32
	LastSpbMeshPNumH = LastSpbH / meshStepH + 1;

	unsigned long meshSize = (SpbNum - 1) * MeshPointNumW * SpbMeshPNumH + MeshPointNumW * LastSpbMeshPNumH;
	*pMeshXI = new unsigned short[meshSize];
	*pMeshXF = new unsigned char[meshSize];
	*pMeshYI = new unsigned short[meshSize];
	*pMeshYF = new unsigned char[meshSize];
	return meshSize;
}

/* FEC mesh 内存释放 */
void freeFecMesh(unsigned short *pMeshXI, unsigned char *pMeshXF, unsigned short *pMeshYI, unsigned char *pMeshYF)
{
	delete[] pMeshXI;
	delete[] pMeshXF;
	delete[] pMeshYI;
	delete[] pMeshYF;
}
