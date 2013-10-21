////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DOutDoorAssistantFuncs.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-4-7 10:56:40
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "KG3DOutDoorAssistantFuncs.h"
#include "KG3DSceneOutDoorSpaceMgr.h"
#include "KG3DMathFunctions.h"
#include "KG3DRepresentObject.h"	//GetObjectCount

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


KG3DOutDoorIntersection::KG3DOutDoorIntersection(KG3DOutDoorIntersectionGroundHeightGetter& DataInterface)
:m_Data(DataInterface)
{

}

/*
算法的详细解释：
1.把Ray变成参数式 P(t) = vSrc + vDir * t;
2.分段，由Src那端开始逐段检查
3.如果一段碰撞线中，Start点的地形比线低，End点的地形比线高，形成一个X型
那么必然两者中间必然会有一个交点。开始用线性插值迅速迭代到真正的交点
4.如果不能形成X型，那么中间可能有突起，还是可能有交点，进行1/2细分，直到满足某种条件退出，或者
得到一个X型，然后用上面的步骤3迭代。
5.如果没有交点，检查下一段直到检查完成。

算法在比较向下的Ray中工作较好，8次GetHeight就能得到结果。
比较坏的情况是向很远的地形中找交点，可能会达到21次查找，一般不超过15次
平均8次左右得到结果。

在陡峭的地形上，交点会弹跳到地形的顶点，但这个不严重，如果只是放物件是可以接受的，1/5坡度的地形放东西没什么用
用上面那堆枚举可以调整精度。

如果需要近距离的精确的结果，可以减短碰撞线，加大精度，这样不会造成很大负担

*/
static RayIntersectionTerrainAccuracyStruct stDefAccuracyStruct;
BOOL KG3DOutDoorIntersection::RayIntersectionTerrain( D3DXVECTOR3 vSrc, D3DXVECTOR3 vNormalizedDir
											  , FLOAT* pfDist
											  , RayIntersectionTerrainAccuracyStruct* pAccuracy/*NULL*/)
{
	FLOAT fRet = 0;

	RayIntersectionTerrainAccuracyStruct CurAccuracy = stDefAccuracyStruct;
	if (NULL != pAccuracy && pAccuracy != &stDefAccuracyStruct)
	{
		CurAccuracy = *pAccuracy;
	}

	m_nGetGoundHeightCount = 0;//用于监控迭代过程


	//创建Ray
	IntersectionRay Ray;
	Ray.m_vSrc = vSrc;
	Ray.m_vNormalizedDir = vNormalizedDir;	
	{
		D3DXVECTOR3 vProjDir = D3DXVECTOR3(vNormalizedDir.x, 0, vNormalizedDir.z);
		D3DXVec3Normalize(&vProjDir, &vProjDir);
		Ray.m_fDirCosine = D3DXVec3Dot(&vNormalizedDir, &vProjDir);//和y=0平面的夹角//这样子只要把参数式中的t乘以这个就可以知道在y=0平面上两点的距离了
	}

	//如果是垂直向下的，那么直接得到GroundHeight就好了
	if (MT_FLOAT_EQUAL(vNormalizedDir.x, 0.0f) && MT_FLOAT_EQUAL(vNormalizedDir.z, 0.0f))
	{
		FLOAT fHeight = GetGroundHeight(vSrc.x, vSrc.z);
		fRet = vSrc.y - fHeight;
		KG_PROCESS_ERROR(fRet > 0);
		goto Exit1;
	}


	{	
		/*
		按步长分解直线成线段，从近到远检查相交性
		这样不需要一次多个采样，一旦有1个范围中，形成采样点分别在线上和线下，形成了X型，那么在这个范围中
		必然有交点
		采用交点渐进逼近就能很快得到准确的交点
		*/
		
		INT	iStepCount = (INT)ceil(CurAccuracy.m_fMaxRange / CurAccuracy.m_fTestRangePerLineSection);
		if(iStepCount < 1)
		{
			iStepCount = 1;
		}
		INT iStep = 0;

		IntersectionTestPoint PStart, PEnd;

		PStart.m_fT = (FLOAT)(iStep) * CurAccuracy.m_fTestRangePerLineSection;
		PStart.m_Pos = vSrc + vNormalizedDir * PStart.m_fT;
		PStart.m_fDeltaToTerrain = PStart.m_Pos.y - GetGroundHeight(PStart.m_Pos.x, PStart.m_Pos.z);

		PEnd.m_fT = (FLOAT)(iStep + 1) * CurAccuracy.m_fTestRangePerLineSection;
		PEnd.m_Pos = vSrc + vNormalizedDir * PEnd.m_fT;
		PEnd.m_fDeltaToTerrain = PEnd.m_Pos.y - GetGroundHeight(PEnd.m_Pos.x, PEnd.m_Pos.z);

		while(iStep < iStepCount)
		{
			if (PStart.m_fDeltaToTerrain < 0 
				&& PEnd.m_fDeltaToTerrain < 0)
			{
				//查到的两点地形都在线的上面，形成一个凹地，不予处理
				goto Exit0;				
			}

			//检查是否存在交叉
			BOOL bIntersectInThisRange = PStart.m_fDeltaToTerrain * PEnd.m_fDeltaToTerrain < 0;//异号

			if (bIntersectInThisRange)
			{
				//如果Start，End得到的高分别在线的上面和下面，形成一个X，那么中间肯定是有交点的，用交点渐进逼近
				fRet = GetIntersectionPoint(Ray, PStart, PEnd, CurAccuracy);
				goto Exit1;
			}
			else
			{
				//如果光从Start，End判断出中间可能没有交点，但还是要把范围切开，仔细检查
				IntersectionTestPoint RetStart, RetEnd;
				BOOL bRet = TestIntersecionInRange(Ray, PStart, PEnd, RetStart, RetEnd, CurAccuracy);
				if (bRet)
				{
					fRet = GetIntersectionPoint(Ray, RetStart, RetEnd, CurAccuracy);
					goto Exit1;
				}
				else
				{
					if (m_nGetGoundHeightCount > CurAccuracy.m_nMaxGetHeightCount)
					{
						goto Exit0;
					}
				}
			}

			//检查下一段
			++iStep;
			
			PStart = PEnd;

			PEnd.m_fT = (FLOAT)(iStep + 1) * CurAccuracy.m_fTestRangePerLineSection;
			PEnd.m_Pos = vSrc + vNormalizedDir * PEnd.m_fT;
			PEnd.m_fDeltaToTerrain = PEnd.m_Pos.y - GetGroundHeight(PEnd.m_Pos.x, PEnd.m_Pos.z);

		}//End of While
	
		if (iStep >= iStepCount)
		{
			goto Exit0;
		}
	}
Exit1:
	//_KG3D_DEBUG_OUTPUT1("GetHeight%d", m_nGetGoundHeightCount);
	if(NULL != pfDist)
		*pfDist = fRet;
	return TRUE;
Exit0:
	return FALSE;
	
}

FLOAT KG3DOutDoorIntersection::GetGroundHeight( FLOAT fX, FLOAT fZ )
{
	++m_nGetGoundHeightCount;
#if defined(DEBUG) | defined(_DEBUG)
	FLOAT fHeight = m_Data.GetGroundHeight(fX, fZ);
	return fHeight;
#else
	return m_Data.GetGroundHeight(fX, fZ);
#endif
	
}


FLOAT KG3DOutDoorIntersection::GetIntersectionPoint( const IntersectionRay& Ray 
															, const IntersectionTestPoint& StartP
															, const IntersectionTestPoint& EndP
															, const RayIntersectionTerrainAccuracyStruct& CurAccuracy)
{
	FLOAT fRet = CurAccuracy.m_fMaxRange;

	IntersectionTestPoint PA = StartP;//PA总是T比较小的那个
	IntersectionTestPoint PB = EndP;
#if defined(DEBUG) | defined(_DEBUG)	
	INT nLoopCount = 0;
#endif

	do 
	{
#if defined(DEBUG) | defined(_DEBUG)
		++nLoopCount;
#endif
		

		IntersectionTestPoint PInter;
		FLOAT fABPointDist = 0;
		{
			FLOAT fDeltaStart = abs(PA.m_fDeltaToTerrain);
			FLOAT fDeltaEnd = abs(PB.m_fDeltaToTerrain);
			_ASSERTE(PA.m_fDeltaToTerrain * PB.m_fDeltaToTerrain < 0);

			fABPointDist = (PB.m_fT - PA.m_fT);
			_ASSERTE(fABPointDist > -FLT_EPSILON);

			//用碰撞线做一个切面，切到地形上，在这个切面上，利用碰撞线前后两个端点，和得到的地形高
			//如果认为地形是平直的话，可以得到在这个切面上，地形和碰撞线的交点PInter。
			//通常这个点会向真正的交点迅速收敛。

			PInter.m_fT =  fABPointDist* fDeltaStart / (fDeltaStart + fDeltaEnd);//用平行线定理得到的
			PInter.m_fT += PA.m_fT;

			PInter.m_Pos = Ray.m_vSrc + Ray.m_vNormalizedDir * PInter.m_fT;
		}
		FLOAT CurCheckPointHeight = GetGroundHeight(PInter.m_Pos.x, PInter.m_Pos.z);

		PInter.m_fDeltaToTerrain = PInter.m_Pos.y - CurCheckPointHeight;

		enum{em_gap_per_point = 100,	//每100米一个点
			em_max_get_height_count = 60
		};
		

		if (abs(PInter.m_fDeltaToTerrain) < CurAccuracy.m_fMinGap //如果得到的地形高度和检查点已经很接近
			|| fABPointDist * Ray.m_fDirCosine< CurAccuracy.m_fMaxAccuracy	//如果两个检查点很接近，用他们的插值做结果就行了
			|| m_nGetGoundHeightCount >= CurAccuracy.m_nMaxGetHeightCount)	
		{

#if 1
			const FLOAT fGapPerPoint = (FLOAT)em_gap_per_point;
			//用三角型和线碰撞得到交点,因为和哪个三角碰转很难得知，所以两个一起查
			FLOAT lowerLeftX = floor(PInter.m_Pos.x/fGapPerPoint) * fGapPerPoint;
			FLOAT lowerLeftZ = floor(PInter.m_Pos.z/fGapPerPoint) * fGapPerPoint;

			D3DXVECTOR3 vlowerLeft(lowerLeftX, GetGroundHeight(lowerLeftX, lowerLeftZ), lowerLeftZ);
			D3DXVECTOR3 vlowerRight(lowerLeftX + fGapPerPoint
						, GetGroundHeight(lowerLeftX + fGapPerPoint, lowerLeftZ), lowerLeftZ);
			D3DXVECTOR3 vUpperRight(lowerLeftX + fGapPerPoint
				, GetGroundHeight(lowerLeftX + fGapPerPoint, lowerLeftZ + fGapPerPoint), lowerLeftZ + fGapPerPoint);
			D3DXVECTOR3 vUpperLeft(lowerLeftX
				, GetGroundHeight(lowerLeftX, lowerLeftZ + fGapPerPoint), lowerLeftZ + fGapPerPoint);

			FLOAT fDist1 = FLT_MAX, fDist2 = FLT_MAX;
			BOOL bInter1 = KG3D_CUSTOM_HELPERS::IntersectRayTri(Ray.m_vSrc, Ray.m_vNormalizedDir
				, vlowerLeft, vUpperRight, vlowerRight, NULL, NULL, &fDist1);
			BOOL bInter2 = KG3D_CUSTOM_HELPERS::IntersectRayTri(Ray.m_vSrc, Ray.m_vNormalizedDir
				, vlowerLeft, vUpperLeft, vUpperRight, NULL, NULL, &fDist2);

			if (bInter1 || bInter2)
			{
				fRet = min(fDist1, fDist2);
				break;
			}
#endif
			
			D3DXVECTOR3 vFakeInterPoint = PInter.m_Pos;
			vFakeInterPoint.y = CurCheckPointHeight;

			fRet = D3DXVec3Length(&(vFakeInterPoint - Ray.m_vSrc));

			break;
		}
		
			

		_ASSERTE(nLoopCount < em_max_get_height_count);

		if (PInter.m_fDeltaToTerrain * PA.m_fDeltaToTerrain < 0)
		{
			PB = PInter;
		}
		else
		{
			PA = PInter;
		}

	} while(1);
	return fRet;
}
BOOL KG3DOutDoorIntersection::TestIntersecionInRange( const IntersectionRay& Ray
													   , const IntersectionTestPoint& StartP
													   , const IntersectionTestPoint& EndP
													   , IntersectionTestPoint& RetStart
													   , IntersectionTestPoint& RetEnd
													   , const RayIntersectionTerrainAccuracyStruct& CurAccuracy)
{

	FLOAT fTDelta = EndP.m_fT - StartP.m_fT;
	_ASSERTE(fTDelta > 0);

	FLOAT fDistance = fTDelta * Ray.m_fDirCosine;
	if (fDistance < CurAccuracy.m_fMinTestRange)
	{
		return FALSE;		
	}
	
	{
		//这个时候，应该线的两点都在地形上面。因为一开始就不允许地形在上面
		//，一旦出现一个TestPoint的结果在上面的话，
		//那样就有一个X型了，可以开始渐进逼近。
		_ASSERTE(StartP.m_fDeltaToTerrain > 0 - FLT_EPSILON && EndP.m_fDeltaToTerrain > 0 - FLT_EPSILON);

		FLOAT fMaxYDelta = min(abs(StartP.m_fDeltaToTerrain), abs(EndP.m_fDeltaToTerrain));
		FLOAT fMaxTangent = fMaxYDelta / fDistance;
		if (fMaxTangent > CurAccuracy.m_fMaxTangent)
		{
			//就是说现在线离地形很高，假设地形没有一种很尖的陡坡，那么认为这整段地形都不可能中间有交点
			return FALSE;
		}
	}

	if (m_nGetGoundHeightCount > CurAccuracy.m_nMaxGetHeightCount)
	{
		return FALSE;
	}

	IntersectionTestPoint MidPoint;
	MidPoint.m_fT = fTDelta/2 + StartP.m_fT;
	MidPoint.m_Pos = Ray.m_vSrc + Ray.m_vNormalizedDir * MidPoint.m_fT;
	MidPoint.m_fDeltaToTerrain = MidPoint.m_Pos.y - GetGroundHeight(MidPoint.m_Pos.x, MidPoint.m_Pos.z);

	if (MidPoint.m_fDeltaToTerrain < FLT_EPSILON)
	{
		RetStart = StartP;
		RetEnd = MidPoint;
		return TRUE;
	}
	
	//查近的那一边，再查远的那一边
	BOOL bRet = TestIntersecionInRange(Ray, StartP, MidPoint, RetStart, RetEnd, CurAccuracy);
	if (! bRet)
	{
		bRet = TestIntersecionInRange(Ray, MidPoint, EndP, RetStart, RetEnd, CurAccuracy);
	}
	return bRet;
}
#if defined(KG_ENABLE_TDD)
#include "Terrain/KG3DGridHeightGetter.h"
KG_TEST_BEGIN(KG3DOutDoorIntersection)
{	
	struct TestVertexHeightGetter: public KG3DGridVertexHeightGetter
	{
		virtual D3DXVECTOR4 GetBlockRect()
		{
			return D3DXVECTOR4(0,0,1600,1600);
		}
		virtual ULONG		GetBlockCellCount(){return BLOCK_CELL_COUNT;}
		virtual FLOAT		GetVertexHeight(int nX,int nZ)
		{
			if(nX >= 0 && nX < _countof(stTestVertices[0]) && nZ >= 0 && nZ < _countof(stTestVertices))
				return stTestVertices[nZ][nX];
			return 0;
		}
	};

	struct TestGroundHeightStruct : public KG3DOutDoorIntersectionGroundHeightGetter 
	{
		virtual FLOAT GetGroundHeight(FLOAT fX, FLOAT fZ)
		{
			TestVertexHeightGetter Getter;
			return KG3DGridHeightGetterFunc(fX, fZ, Getter);
		}
	};

	TestGroundHeightStruct HeightGetter;
	KG3DOutDoorIntersection intersectTemp(HeightGetter);
	
	struct TestCase 
	{
		D3DXVECTOR3 vSrc;
		D3DXVECTOR3 vDir;
		FLOAT fRet;
	};

	static FLOAT	stTestVertices[6][16] = //模拟出一个方形突起的地形
	{
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,100,100,100,100,0,0,0,0,0,0,0,0,0,0,0},
		{0,100,100,100,100,0,0,0,0,0,0,0,0,0,0,0},
		{0,100,100,100,100,0,0,0,0,0,0,0,0,0,0,0},
		{0,100,100,100,100,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
	};
	_ASSERTE(_countof(stTestVertices[0]) == 16 && _countof(stTestVertices) == 6);

	const FLOAT squareTwo = 1.4142f;
	TestCase Cases[] = 
	{
		{D3DXVECTOR3(0,100,300),	D3DXVECTOR3(1,-1,0), 50 * squareTwo},
		{D3DXVECTOR3(100,1000,100),	D3DXVECTOR3(0,-1,0), 900},
		{D3DXVECTOR3(500, 100, 100), D3DXVECTOR3(-1, -1, 0), 50 * squareTwo},
		{D3DXVECTOR3(300, 100, 0), D3DXVECTOR3(0, -1, 1), 50 * squareTwo},
		{D3DXVECTOR3(300, 100, 500), D3DXVECTOR3(0, -1, -1), 50 * squareTwo},
		{D3DXVECTOR3(300, 120, 200), D3DXVECTOR3(1, -1, 0), 20 * squareTwo}
	};

	for(size_t i = 0; i < _countof(Cases); ++i)
	{
		D3DXVec3Normalize(&Cases[i].vDir, &Cases[i].vDir);
		FLOAT fRet = 0;
		BOOL bRet = intersectTemp.RayIntersectionTerrain(Cases[i].vSrc, Cases[i].vDir, &fRet, NULL);
		_ASSERTE(bRet);

		_ASSERTE(MT_FLOAT_EQUAL_BIAS(fRet, Cases[i].fRet, 1));
	}	
}
KG_TEST_END(KG3DOutDoorIntersection)
#endif



RayIntersectionTerrainAccuracyStruct::RayIntersectionTerrainAccuracyStruct() 
	:m_fMaxRange(EM_RAY_INTERSECTION_MAX_RANGE)
	,m_fTestRangePerLineSection(800)
	,m_fMinTestRange(100)
	,m_fMaxTangent(5)
	,m_fMaxAccuracy(/*100*/50)
	,m_fMinGap(2)
	,m_nMaxGetHeightCount(30)
{

}

VOID RayIntersectionTerrainAccuracyStruct::SetMaxRange( FLOAT fRange ) /*当碰撞检测距离加大的时候，精度也相应放大，不然对于远距离函数来说，很费 */
{
	_ASSERTE(fRange > 0);
	FLOAT fDefaultMaxRange = (FLOAT)EM_RAY_INTERSECTION_MAX_RANGE;
	if(fRange - fDefaultMaxRange < FLT_EPSILON)//即小于等于
	{
		m_fMaxRange = fRange;
		return;
	}

	FLOAT fScalePercent = fRange / fDefaultMaxRange;

	RayIntersectionTerrainAccuracyStruct DefaultStruct;

	m_fTestRangePerLineSection = fScalePercent * DefaultStruct.m_fTestRangePerLineSection;
	m_fMinTestRange = fScalePercent * DefaultStruct.m_fMinTestRange;
	m_nMaxGetHeightCount = (INT)(max(fScalePercent, 2.f) * (FLOAT)DefaultStruct.m_nMaxGetHeightCount);
}

ULONG KG3D_OUTDOOR::GetObjectCount( KG3DSceneOutDoorSpaceMgr& outDoorMgr, DWORD dwTypeFilter )
{
	D3DXVECTOR2 vLowerLeft, vUpperRight;
	outDoorMgr.GetSceneRect(&vLowerLeft, &vUpperRight);

	if (dwTypeFilter == REPRESENTOBJECT_NONE)
	{
		struct ObjectCounter : public KG3DEntityCtVisitorForObj
		{
			size_t m_uCount;			
			virtual HRESULT Accept(KG3DRepresentObject& obj)
			{
				++m_uCount;
				return S_OK;
			}
		};

		ObjectCounter counterTemp;
		counterTemp.m_uCount = 0;

		outDoorMgr.Traverse(counterTemp, vLowerLeft.x, vLowerLeft.y, vUpperRight.x, vUpperRight.y);

		return static_cast<ULONG>(counterTemp.m_uCount);
	}
	else
	{
		struct ObjectCounter : public KG3DEntityCtVisitorForObj
		{
			size_t m_uCount;
			DWORD m_dwFilter;
			virtual HRESULT Accept(KG3DRepresentObject& obj)
			{
				if(obj.GetType() == m_dwFilter)
					++m_uCount;
				return S_OK;
			}
		};

		ObjectCounter counterTemp;
		counterTemp.m_uCount = 0;
		counterTemp.m_dwFilter = dwTypeFilter;

		outDoorMgr.Traverse(counterTemp, vLowerLeft.x, vLowerLeft.y, vUpperRight.x, vUpperRight.y);

		return static_cast<ULONG>(counterTemp.m_uCount);
	}
	return 0;	
}