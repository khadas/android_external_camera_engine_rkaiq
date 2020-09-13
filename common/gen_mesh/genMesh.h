#ifndef __GENMESH_H__
#define __GENMESH_H__


/* ������� */
struct CameraCoeff
{
	double cx, cy;							// ��ͷ�Ĺ���
	double a0, a2, a3, a4;					// ��ͷ�Ļ���ϵ��
	double c, d, e;							// �ڲ�[c d;e 1]
	double sf;								// sf�����ӽǣ�sfԽ���ӽ�Խ��
	/* level = 0ʱ��rho-theta����ʽ��� */
	int invPolyNum0;						// ϵ������
	double invPolyCoeff0[21];				// ����ʽϵ������ߴ���20��
	/* level = 255ʱ��rho-theta����ʽ��� */
	int invPolyNum255;						// ϵ������
	double invPolyCoeff255[21];				// ����ʽϵ������ߴ���20��
};

/* ����FECӳ�����صĲ��� */
struct FecParams
{
	int srcW, srcH, dstW, dstH;							/* �������ͼ��ķֱ��� */
	int srcW_ex, srcH_ex, dstW_ex, dstH_ex;				/* ��չ�����������ֱ��� */
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

/* ����LDCHӳ�����صĲ��� */
struct LdchParams
{
	int srcW, srcH, dstW, dstH;
	int meshSizeW, meshSizeH;
	double meshStepW, meshStepH;
	int meshSize;

	double *mapx;
	double *mapy;
};


/* FEC: ��ʼ��������ͼ������ֱ��ʣ�����FECӳ������ز�����������Ҫ��buffer */
void genFecMeshInit(int srcW, int srcH, int dstW, int dstH, FecParams &fecParams, CameraCoeff &camCoeff);

/* FEC: ����ʼ�� */
void genFecMeshDeInit(FecParams &fecParams);

/* FEC: Ԥ�ȼ���Ĳ���: ����δУ����С���level=0,level=255�Ķ���ʽ���� */
void genFecPreCalcPart(FecParams &fecParams, CameraCoeff &camCoeff);

/* FEC: 4��mesh �ڴ����� */
void mallocFecMesh(int meshSize, unsigned short **pMeshXI, unsigned char **pMeshXF, unsigned short **pMeshYI, unsigned char **pMeshYF);

/* FEC: 4��mesh �ڴ��ͷ� */
void freeFecMesh(unsigned short *pMeshXI, unsigned char *pMeshXF, unsigned short *pMeshYI, unsigned char *pMeshYF);

/*
��������: ���ɲ�ͬУ���̶ȵ�meshӳ�������ISP��FECģ��
	����:
	1��FECӳ������ز�����������Ҫ��buffer: FecParams &fecParams
	2������궨����: CameraCoeff &camCoeff
	3����ҪУ���ĳ̶�: level(0-255: 0��ʾУ���̶�Ϊ0%, 255��ʾУ���̶�Ϊ100%)
	���:
	1��bool �Ƿ�ɹ�����
	2��pMeshXI, pMeshXF, pMeshYI, pMeshYF
*/
bool genFECMeshNLevel(FecParams &fecParams, CameraCoeff &camCoeff, int level, unsigned short *pMeshXI, unsigned char *pMeshXF, unsigned short *pMeshYI, unsigned char *pMeshYF);


/* =============================================================================================================================================================================== */

/* LDCH: ��ʼ��������ͼ������ֱ��ʣ�����LDCHӳ������ز�����������Ҫ��buffer */
void genLdchMeshInit(int srcW, int srcH, int dstW, int dstH, LdchParams &ldchParams, CameraCoeff &camCoeff);

/* LDCH: ����ʼ�� */
void genLdchMeshDeInit(LdchParams &ldchParams);

/* LDCH: Ԥ�ȼ���Ĳ���: ����δУ����С���level=0,level=255�Ķ���ʽ���� */
void genLdchPreCalcPart(LdchParams &ldchParams, CameraCoeff &camCoeff);

/*
��������: ���ɲ�ͬУ���̶ȵ�meshӳ�������ISP��LDCHģ��

	����:
	1��LDCHӳ������ز�����������Ҫ��buffer: LdchParams &ldchParams
	2������궨����: CameraCoeff &camCoeff
	3����ҪУ���ĳ̶�: level(0-255: 0��ʾУ���̶�Ϊ0%, 255��ʾУ���̶�Ϊ100%)
	���:
	1��bool �Ƿ�ɹ�����
	2��pMeshX
*/
bool genLDCMeshNLevel(LdchParams &ldchParams, CameraCoeff &camCoeff, int level, unsigned short *pMeshX);

#endif // !__GENMESH_H__
