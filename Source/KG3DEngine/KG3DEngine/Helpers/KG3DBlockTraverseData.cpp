////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DBlockTraverseData.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-5-26 11:30:40
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG3DBlockTraverseData.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


////////////////////////////////////////////////////////////////////////////////

#define FLT_BIG_EPSILON	0.001f	//用FLT_EPSILON还不行，这些边界问题真恼人
KG3DBlockTraverseData KG3DBlockTraverseData::Build( const D3DXVECTOR2& vSrc, const D3DXVECTOR2& vDes, FLOAT fSceneBlockWidth )
{
	

	/*保证一次跨越一个Zone，计算出每部的Zone的Step
	在纸上画格子可以发现，若斜线(P(t) = vSrc + t * vDir)扫过两个对角的格子，那么和这两个格子
	相邻的那个格子也很容易被扫到。即若一条线扫过田字格子左下和右上的格子，那么左上或者右下的格子也
	很容易被扫到，视乎线的位置。假设线扫过田字左下，左上，右上的格子，田字上的点如下：

	0--1--2
	3--4--5
	6--7--8

	在点3，4，1向这条线做垂线，交点分别为A,B,C。那么可以发现，每跨越AB再跨越BC一定能跨越两个格子
	，AB和BC的长度能够作为Step，在纸上画画就看出来了。因为现在是直角坐标，所以计算出AB和BC的距离之后
	要再分解成X和Y的分量，存储在D3DXVECTOR2里面。因为AB和BC长度是不等的，所以要分成两个Step，第一次跨越
	AB的话，下一次就跨越BC

	下面的运算，有的时候用的是统一坐标，即格子宽度是1，有的时候用的是实际坐标，即格子宽度是SceneBlockWidth
	*/

	KG3DBlockTraverseData retData;

	_ASSERTE(fSceneBlockWidth > 0);
	D3DXVECTOR2 vNormalizedDir;
	D3DXVECTOR2 vCosSin;
	{
		D3DXVECTOR2 vDistance = vDes - vSrc;		
		D3DXVec2Normalize(&vNormalizedDir, &vDistance);
		vCosSin = D3DXVECTOR2(vNormalizedDir.x / 1, vNormalizedDir.y / 1);
	}

	//计算出步长、步数、开始坐标
	D3DXVECTOR2 vLowerLeftCornerIndex = D3DXVECTOR2(static_cast<FLOAT>(GetSceneBlockIndex(vSrc.x, fSceneBlockWidth))
		,	static_cast<FLOAT>(GetSceneBlockIndex(vSrc.y, fSceneBlockWidth)));
	retData.m_vStartIndex = vSrc / fSceneBlockWidth;

	_ASSERTE(vLowerLeftCornerIndex.x * fSceneBlockWidth - vSrc.x < FLT_BIG_EPSILON && 
		vLowerLeftCornerIndex.y * fSceneBlockWidth - vSrc.y < FLT_BIG_EPSILON && _T("vLowerLeftCornerIndex应该暂时是左下角"));

	//如果垂直和水平要另外处理，否则光是边界问题就很复杂
	bool bVertical = abs(vNormalizedDir.x) < FLT_BIG_EPSILON;
	bool bHorizontal = abs(vNormalizedDir.y) < FLT_BIG_EPSILON;
	if (! bVertical && ! bHorizontal)	
	{
		{
			/*
			由垂线的交点可以知道AB的长度必然是格子宽度乘cos(dir)，而BC是sin(dir)
			因为Step是二维的，所以又再次乘cos,sin分解为x,y
			*/
			FLOAT fLengthOfStep1 = 1 * abs(vCosSin.x);
			FLOAT fLengthOfStep2 = 1 * abs(vCosSin.y);
			retData.m_vIndexStep1 = fLengthOfStep1 * vCosSin;
			retData.m_vIndexStep2 = fLengthOfStep2 * vCosSin;
		}

		/*			
		如出上面所说的，计算出A，那么就可以用A+Step * Count来计算Zone的Index了
		用计算出来的A代替起始坐标，否则在连续访问Zone的时候，A+Step * Count不能保证一次跨越一个Zone;
		注意是一次跨越AB，下一次跨越BC
		*/


		/*根据线的方向有四种求开始交点的方向，而且对角给出两个交点，选其中一个交点开始
		如上图的田字格子，如果起点从左下开始，且线的方向斜向上，那么，3和7给出的交点都可以接受
		如果是3开始，那么第一次跨越的是Step1，从7开始就是Step2
		*/

		struct QuadrantStratege
		{
			D3DXVECTOR2 vFirstCrossTestPointsGapBetweenLeftBottomPoint;
			D3DXVECTOR2 vSecondCrossTestPointsGapBetweenLeftBottomPoint;
		};

		static QuadrantStratege quadrantStrategeArray[4] = 
		{
			{D3DXVECTOR2(0, 1), D3DXVECTOR2(1, 0)},
			{D3DXVECTOR2(1, 1), D3DXVECTOR2(0, 0)},
			{D3DXVECTOR2(1, 0), D3DXVECTOR2(0, 1)},
			{D3DXVECTOR2(0, 0), D3DXVECTOR2(1, 1)},
		};

		INT nQuradrant = 0;
		if(vNormalizedDir.x > 0)
			nQuradrant = vNormalizedDir.y > 0 ? 1 : 4;
		else
			nQuradrant = vNormalizedDir.y > 0 ? 2 : 3;

		_ASSERTE(nQuradrant >= 1 && nQuradrant <= 4);

		D3DXVECTOR2 vGridCorner = quadrantStrategeArray[nQuradrant - 1].vFirstCrossTestPointsGapBetweenLeftBottomPoint;
		D3DXVECTOR2 vGridSrc = vSrc / fSceneBlockWidth - vLowerLeftCornerIndex;	//转换到以1为单位的统一坐标

		D3DXVECTOR2 vCross = PointDistanceToLine(vGridCorner, vGridSrc, vNormalizedDir);
#if defined(DEBUG) | defined(_DEBUG)
		{
			//它们肯定在Block宽的根号2的距离内，最长的情况是线斜四十五度穿过对角，而起点在其中一角的情况
			D3DXVECTOR2 vGap = vCross - vGridSrc;
			_ASSERTE(abs(D3DXVec2Length(&vGap)) < 1.42);//1.42就是单位1格子对角线长度
		}
#endif
		//看看哪个交点和原来的起点还在同一个格子里面，把新起点设到那个地方
		BOOL bABFirst = TRUE;
		BOOL IsIntheSameGrid = MT_FLOAT_IS_IN_RANGE(vCross.x, 0, 1.0f) && MT_FLOAT_IS_IN_RANGE(vCross.y, 0, 1.0f);
		if(IsIntheSameGrid)
		{
			retData.m_vStartIndex = vCross + vLowerLeftCornerIndex;
		}
		else
		{
			//如果交点和Src不是同一个格子里面的（Src在右下，线斜向上）
			//即如上面的图，起点在B附近的话，那要先遍历BC，再遍历AB

			vGridCorner = quadrantStrategeArray[nQuradrant - 1].vSecondCrossTestPointsGapBetweenLeftBottomPoint;
			vCross = PointDistanceToLine(vGridCorner, vGridSrc, vNormalizedDir);

			_ASSERTE(MT_FLOAT_IS_IN_RANGE(vCross.x, 0, 1.0f) 
				&& MT_FLOAT_IS_IN_RANGE(vCross.y, 0, 1.0f) 
				&& _T("起点不在同一个格子，遍历的结果可能会有问题"));

			std::swap(retData.m_vIndexStep1, retData.m_vIndexStep2);//这种情况下，即上图的先跨越BC的情况，step是相反的

			retData.m_vStartIndex = vCross + vLowerLeftCornerIndex;

			bABFirst = FALSE;				
		}

	}
	else
	{
		retData.m_vIndexStep1 = vNormalizedDir;
		retData.m_vIndexStep2 = retData.m_vIndexStep1;
		retData.m_vStartIndex = vSrc / fSceneBlockWidth;
	}

#if 0
	//当线经过格子的角的时候，会发生其中一个Step很短的情况，应该合并掉
	if(D3DXVec2Length(&retData.m_vIndexStep1) < FLT_EPSILON)
	{
		_ASSERTE(D3DXVec2Length(&retData.m_vIndexStep2) > FLT_EPSILON);
		retData.m_vIndexStep1 = retData.m_vIndexStep2;
	}
	else if(D3DXVec2Length(&retData.m_vIndexStep2) < FLT_EPSILON)
	{
		_ASSERTE(D3DXVec2Length(&retData.m_vIndexStep1) > FLT_EPSILON);
		retData.m_vIndexStep2 = retData.m_vIndexStep1;
	}
#endif

	//跨越的方式都是先AB，再BC，AB=vCosSin.x，BC=vCosSin.y。单位是格子数
	{
		
		FLOAT fTraverseCount = 0;
		if(! bVertical && ! bHorizontal)
			fTraverseCount = D3DXVec2Length(&(vDes - retData.m_vStartIndex * fSceneBlockWidth)) * 2 /((abs(vCosSin.x) + abs(vCosSin.y)) * fSceneBlockWidth);
		else
			fTraverseCount = D3DXVec2Length(&(vDes - vSrc))/ fSceneBlockWidth;
		FLOAT fRemain = fTraverseCount - MT_FLOAT_TRIM_TAIL(fTraverseCount);
		INT nStepRemain = 0;
		if (fRemain > FLT_BIG_EPSILON)
		{
			++nStepRemain;
		}
		if (fRemain > D3DXVec2Length(&retData.m_vIndexStep1))
		{
			++nStepRemain;
		}
		retData.m_nStep = MT_FLOAT_TRIM_TAIL(fTraverseCount) + nStepRemain;
	}

	return retData;
}

bool operator==(const KG3DBlockTraverseData& dataA, const KG3DBlockTraverseData& dataB)
{
	return MT_VECTOR_EQUAL_2D(dataA.m_vStartIndex, dataB.m_vStartIndex, FLT_BIG_EPSILON)
		&& dataA.m_nStep == dataB.m_nStep
		&& MT_VECTOR_EQUAL_2D(dataA.m_vIndexStep1, dataB.m_vIndexStep1, FLT_BIG_EPSILON)
		&& MT_VECTOR_EQUAL_2D(dataA.m_vIndexStep2, dataB.m_vIndexStep2, FLT_BIG_EPSILON);
}

bool operator!=(const KG3DBlockTraverseData& dataA, const KG3DBlockTraverseData& dataB)
{
	return !(dataA == dataB);
}

#if defined(KG_ENABLE_TDD)
KG_TEST_BEGIN(KG3DBlockTraverseData)
{
	enum
	{
		em_bw = 1600,	//bw是blockwidth的意思
		em_bw_double = em_bw * 2,
		em_bw_half = em_bw /2,
	};
	struct TestCase 
	{
		D3DXVECTOR2 vSrc;
		D3DXVECTOR2 vDes;
		KG3DBlockTraverseData retData;
	};

	TestCase testCases[] = 
	{
		{
			D3DXVECTOR2(119830.77f, 35201.230f),
			D3DXVECTOR2(121026.63f, 35202.672f),
			{
				D3DXVECTOR2(75.f, 22.0009f),
				3,
				D3DXVECTOR2(0.0012f, 0), 
				D3DXVECTOR2(0.999999f, 0.001205f)
			}
			
		},
		//X轴向的
		{
			D3DXVECTOR2(em_bw_half, em_bw_half),
			D3DXVECTOR2(em_bw + em_bw_half, em_bw_half),

			{
				D3DXVECTOR2(0.5f, 0.5f),
				1,
				D3DXVECTOR2(1, 0), 
				D3DXVECTOR2(1, 0)
			},
		},
		{
			D3DXVECTOR2(em_bw_half, em_bw_half),
			D3DXVECTOR2(em_bw_double + em_bw_half, em_bw_half),
			{
				D3DXVECTOR2(0.5f, 0.5f),
				2,
				D3DXVECTOR2(1, 0), 
				D3DXVECTOR2(1, 0)
			},
		},
		//Y轴向
		{
			D3DXVECTOR2(em_bw_half, em_bw_half),
				D3DXVECTOR2(em_bw_half, em_bw_double + em_bw_half),
			{
				D3DXVECTOR2(0.5f, 0.5f),
				2,
				D3DXVECTOR2(0, 1), 
				D3DXVECTOR2(0, 1), 
			},
		},
		//下面分别测试4个象限
		{
			D3DXVECTOR2(-em_bw, -em_bw),
			D3DXVECTOR2(em_bw, em_bw),

			{
				D3DXVECTOR2(-0.5f, -0.5f),
				3,
				D3DXVECTOR2(0.5f,0.5f), 
				D3DXVECTOR2(0.5f,0.5f)
			},
		},
		{
			D3DXVECTOR2(em_bw, em_bw),
				D3DXVECTOR2(-em_bw, -em_bw),

			{
				D3DXVECTOR2(1.5f, 1.5f),
				5,
				D3DXVECTOR2(-0.5f,-0.5f), 
				D3DXVECTOR2(-0.5f,-0.5f)
			},
		},
		{
			D3DXVECTOR2(em_bw, -em_bw),
			D3DXVECTOR2(-em_bw, em_bw),

			{
				D3DXVECTOR2(1, -1),
				4,
				D3DXVECTOR2(-0.5f,0.5f), 
				D3DXVECTOR2(-0.5f,0.5f)
			},
		},
		{
			D3DXVECTOR2(-em_bw, em_bw),
			D3DXVECTOR2(em_bw, -em_bw),

			{
				D3DXVECTOR2(-1, 1),
				4,
				D3DXVECTOR2(0.5f,-0.5f), 
				D3DXVECTOR2(0.5f,-0.5f)
			},
		},
		
	};

	for (size_t i = 0; i < _countof(testCases); ++i)
	{
		TestCase& caseElem = testCases[i];
		KG3DBlockTraverseData data = KG3DBlockTraverseData::Build(caseElem.vSrc, caseElem.vDes, em_bw);
		_ASSERTE(data == caseElem.retData);
	}
	
}
KG_TEST_END(KG3DBlockTraverseData)
#endif