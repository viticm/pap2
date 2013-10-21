////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DOutDoorAssistantFuncs.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-4-7 10:56:35
/*  Comment     : 地形的辅助函数,这个类是描述算法的，和具体的类无关，简单的替换掉
DataInterface就可以为不同的类服务

*/
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DOUTDOORASSISTANTFUNCS_H_
#define _INCLUDE_KG3DOUTDOORASSISTANTFUNCS_H_

////////////////////////////////////////////////////////////////////////////////
class KG3DSceneOutDoorSpaceMgr;


struct RayIntersectionTerrainAccuracyStruct 
{
	FLOAT m_fMaxRange;			//只能最多检查N厘米内的碰撞，把碰撞线变成碰撞线段,避免无限远的碰撞检查
	FLOAT m_fTestRangePerLineSection;			//把碰撞线段分成几个子线段

	FLOAT m_fMinTestRange;		//如果不能碰撞线段的两个端点都在地形上，碰撞也可能在中间发生，也要检查。这个用于定义究竟要检查这种中间的突起到多精细的程度
	FLOAT m_fMaxTangent;		//最多可以在5/1的坡面查到点。定义地形可能有多陡峭，如果地形太陡，有太多尖凸起，那么算法要考虑很密的取样，否则，可以忽略那些碰撞线前后两端点都离地形很高的部分
	FLOAT m_fMaxAccuracy;		//当碰撞线段细分到N厘米之内，插值的结果就可以接受了

	FLOAT m_fMinGap;			//如果得到的点和测试点的高度差在这个范围内，那么可以接受
	INT	  m_nMaxGetHeightCount;	//每次做RayIntersection最多查地形30次

	RayIntersectionTerrainAccuracyStruct();

	VOID SetMaxRange(FLOAT fRange);//当碰撞检测距离加大的时候，精度也相应放大，不然对于远距离函数来说，很费;
};

struct KG3DOutDoorIntersectionGroundHeightGetter 
{
	virtual FLOAT GetGroundHeight(FLOAT fX, FLOAT fZ) = 0;
	virtual ~KG3DOutDoorIntersectionGroundHeightGetter() = 0{}		 
};

class KG3DOutDoorIntersection
{
	//<碰撞检查>
public:
	BOOL	RayIntersectionTerrain(D3DXVECTOR3 vSrc, D3DXVECTOR3 vNormalizedDir, FLOAT* pfDist
		, RayIntersectionTerrainAccuracyStruct* pAccuracy = NULL);//射线查交点。这里使用直线的参数式 P(t) = vSrc + vDir * t; 不限制结果范围最大值，

private:
	struct IntersectionRay	//碰撞线的结构体
	{
		D3DXVECTOR3 m_vSrc;
		D3DXVECTOR3 m_vNormalizedDir;
		FLOAT		m_fDirCosine;	//Dir在y=0平面上投影的夹角的余弦，有了这个和参数式里面的T，能够很容易的计算两点在y=0平面上的距离
		IntersectionRay(){}
		IntersectionRay(const IntersectionRay& Other)
		{
			memcpy_s(this, sizeof(IntersectionRay), &Other, sizeof(IntersectionRay));
		}
		IntersectionRay& operator=(const IntersectionRay& Other)
		{
			memcpy_s(this, sizeof(IntersectionRay), &Other, sizeof(IntersectionRay));
			return *this;
		}
	};
	struct IntersectionTestPoint	//碰撞点的结构体
	{
		D3DXVECTOR3 m_Pos;
		FLOAT	m_fT;		//用参数式时候的T
		FLOAT	m_fDeltaToTerrain;//这一点离地形的距离，正表示线在地形上面
		IntersectionTestPoint(){}
		IntersectionTestPoint(const IntersectionTestPoint& Other)
		{
			memcpy_s(this, sizeof(IntersectionTestPoint), &Other, sizeof(IntersectionTestPoint));
		}
		IntersectionTestPoint& operator=(const IntersectionTestPoint& Other)
		{
			memcpy_s(this, sizeof(IntersectionTestPoint), &Other, sizeof(IntersectionTestPoint));
			return *this;
		}
	};
	FLOAT	GetIntersectionPoint(const IntersectionRay& Ray
									, const IntersectionTestPoint& StartP
									, const IntersectionTestPoint& EndP
									, const RayIntersectionTerrainAccuracyStruct& Accuracy);
	BOOL TestIntersecionInRange(const IntersectionRay& Ray
								, const IntersectionTestPoint& StartP
								, const IntersectionTestPoint& EndP
								, IntersectionTestPoint& RetStart
								, IntersectionTestPoint& RetEnd
								, const RayIntersectionTerrainAccuracyStruct& Accuracy);	
public:
	virtual FLOAT	GetGroundHeight(FLOAT fX, FLOAT fZ);//转调
private:
	INT	m_nGetGoundHeightCount;
	//</碰撞检查>
public:
	KG3DOutDoorIntersection(KG3DOutDoorIntersectionGroundHeightGetter& DataInterface);
private:
	KG3DOutDoorIntersectionGroundHeightGetter& m_Data;
};

namespace KG3D_OUTDOOR
{
	ULONG	GetObjectCount(KG3DSceneOutDoorSpaceMgr& outDoorMgr, DWORD dwTypeFilter);//dwType=REPRESENTOBJECT_NONE的时候直接统计，效率较高，否则就过滤对应类型
};

#endif //_INCLUDE_KG3DOUTDOORASSISTANTFUNCS_H_
