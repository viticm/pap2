////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DGridHeightGetter.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-9-17 14:56:46
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG3DGridHeightGetter.h"


////////////////////////////////////////////////////////////////////////////////


FLOAT KG3DGridHeightGetterFunc( FLOAT fX, FLOAT fZ, KG3DGridVertexHeightGetter& GH )
{
	D3DXVECTOR4 vBlockRect = GH.GetBlockRect();

	INT blockCellCount = GH.GetBlockCellCount();

	_ASSERTE(0.f != vBlockRect.z && 0.f != vBlockRect.w);

	//放缩到1个Region之内
	float fScaledX = (fX - vBlockRect.x) / vBlockRect.z * blockCellCount;	
	float fScaledZ = (fZ - vBlockRect.y) / vBlockRect.w * blockCellCount;

	INT iX = (INT)floor(fScaledX);//去尾得到左下位置，1.1变1，-1.1变-2
	INT iZ = (INT)floor(fScaledZ);

	FLOAT fXTail = fScaledX - (FLOAT)iX;
	FLOAT fZTail = fScaledZ - (FLOAT)iZ;

	_ASSERTE(fXTail > -FLT_EPSILON && fZTail > -FLT_EPSILON && fXTail < 1 && fZTail < 1);

	//超过Region大小的压到Region边上
	if(iX > blockCellCount)
		iX = blockCellCount;
	if(iZ > blockCellCount)
		iZ = blockCellCount;

	INT iXNext = iX + 1 <= blockCellCount ? iX + 1 : blockCellCount;
	INT iZNext = iZ + 1 <= blockCellCount ? iZ + 1 : blockCellCount;

	/*
	地形的面是左上右下三角拼成的格子，要根据左上还是右下进行三角形插值
	从左下开始，逆时针4点定义为，1，2，3，4
	下面的公式和原来一样，只是变了形式而已，是用相似三角形一步步推导出来的，有兴趣的可以自己在纸上画出，
	左下点高度最低的时候可以推导出公式1，右上点最低的时候可以推导出公式2
	用这两种特殊情况推导出的公式形式最简单，否则会很复杂
	*/
	BOOL IsPoint_UpperLeft_Or_LowerRight = fZTail > fXTail;	//夹角在45度以上，一定是在左上三角
	if (IsPoint_UpperLeft_Or_LowerRight)
	{
		FLOAT h1 = GH.GetVertexHeight(iX, iZ);
		FLOAT h3 = GH.GetVertexHeight(iXNext, iZNext);
		FLOAT h4 = GH.GetVertexHeight(iX, iZNext);

		return h1 + (h4 - h1) * fZTail + (h3 - h4) * fXTail;
	}

	FLOAT h1 = GH.GetVertexHeight(iX, iZ);
	FLOAT h2 = GH.GetVertexHeight(iXNext, iZ);
	FLOAT h3 = GH.GetVertexHeight(iXNext, iZNext);

	return h1 + (h3 - h2) * fZTail + (h2 - h1) * fXTail;
}

#if defined(KG_ENABLE_TDD)
KG_TEST_BEGIN(KG3DTerrainBlock)
{
	static FLOAT	stTestVertices[4][16] = 
	{
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,100,100,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,100,100,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
	};
	_ASSERTE(_countof(stTestVertices[0]) == 16);

	struct GroundHeightGetter : public KG3DGridVertexHeightGetter
	{
		virtual FLOAT GetVertexHeight(int nX,int nZ)
		{
			_ASSERTE(nX < _countof(stTestVertices[0]) && nZ < _countof(stTestVertices[0]));
			_ASSERTE(nX >= 0 && nZ >= 0);
			return stTestVertices[nZ][nX];
		}
		virtual ULONG GetBlockCellCount(){return 16;}
		virtual D3DXVECTOR4 GetBlockRect()
		{
			return D3DXVECTOR4(0,0,1600,1600);
		}
	};

	GroundHeightGetter TestObjInstance;

	//每组分别是x,z和预期的结果
	FLOAT stTestCases[][3] = 
	{
		{0,		0,		0},
		{100,	100,	100},
		{150,	150,	100},
		{200,	200,	100},
		{300,	300,	0},
		{0,		100,	0},
		{100,	200,	100},
		{50,	50,		50},
		{100,	50,		50},
		{250,	250,	50},
		{50,	100,	50}
	};

	_ASSERTE(3 == _countof(stTestCases[0]));

	for (size_t i = 0; i < _countof(stTestCases); ++i)
	{
		FLOAT fRet = KG3DGridHeightGetterFunc(stTestCases[i][0], stTestCases[i][1], TestObjInstance);
		_ASSERTE(MT_FLOAT_EQUAL(fRet, stTestCases[i][2]));
	}
}
KG_TEST_END(KG3DTerrainBlock)
#endif