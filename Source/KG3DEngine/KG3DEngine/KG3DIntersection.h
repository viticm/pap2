////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DIntersection.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-4-8 9:57:54
/*  Comment     : 用C方式写碰撞引擎，省的和各个类形成强耦合。这个头文件包括了很多类的声明
不要在Scene有关的类以外的地方包含这个头文件，这是个高层算法，低层实现不应该和它耦合。
如果有东西需要低层声明的，放KG3DIntersectionBase.h
*/
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DINTERSECTION_H_
#define _INCLUDE_KG3DINTERSECTION_H_

#include "KG3DIntersectionBase.h"

class KG3DScene;
class KG3DSceneEntityContainer;
class KG3DSceneOutDoorSpaceMgr;

enum EM_RAY_INTERSECTION_MASK
{
	EM_RAY_INTERSECTION_MASK_TERRAIN = 0x01,
	EM_RAY_INTERSECTION_MASK_WATER = 0x02,
	EM_RAY_INTERSECTION_MASK_TERRAIN_PART = 0xff,	//即包括地形，水或者以后其它大地的部分
	EM_RAY_INTERSECTION_MASK_OBJECT = 0x100,	
};


////////////////////////////////////////////////////////////////////////////////
/*总入口，检查地形和物件。所有的TestRange参数都表示测试线长度，关于结果的过滤，看KG3DIntersectionFilter。
返回的碰撞物可能是ppObjectIntersected也可能是ppEntityIntersected，视乎新旧地形。返回值要立刻使用。
结果是从vSrc开始在fTestRange之内的碰撞，把测试范围放到无限并不是个好主意，如果为了速度应该尽量压缩测试范围
*/
BOOL RayIntersection(KG3DScene& Scene
					 , const D3DXVECTOR3& vSrc
					 , const D3DXVECTOR3& vNormalizedDir
					 , FLOAT* pfRet	//从vSrc到交点的距离，用vSrc+ vNormalizedDir * T可以得到交点坐标
					 , KG3DRepresentObject** ppObjectIntersected
					 , KG3DSceneEntity** ppEntityIntersected
					 , FLOAT fTestRange = EM_RAY_INTERSECTION_MAX_RANGE
					 , KG3DIntersectionFilter* pFilter = NULL
					 );//直线交点，直线为隐式P(t) = vSrc + vDir * t，最后两个返回碰到的对象，如果撞到地形则不返回对象

//只检查地形,结果可以是交点，也可以是到射线的原点的距离, dwMask是EM_RAY_INTERSECTION_MASK的组合
BOOL RayIntersectionTerrain(KG3DScene& Scene, const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vNormalizedDir
							, FLOAT* pfDist
							, FLOAT fTestRange = EM_RAY_INTERSECTION_MAX_RANGE
							, DWORD dwMask = EM_RAY_INTERSECTION_MASK_TERRAIN);

//只检查物件的话，用下面第一个
BOOL RayIntersectionEntity(KG3DScene& Scene, const D3DXVECTOR3& vSrc
						   , const D3DXVECTOR3& vNormalizedDir, FLOAT* pDistance
						   , KG3DRepresentObject** ppObj, KG3DSceneEntity** pEntityIntersected
						   ,  FLOAT fTestRange = EM_RAY_INTERSECTION_MAX_RANGE
						   , KG3DIntersectionFilter* pFilter = NULL);	//和物件碰撞的总入口

//特殊版本，垂直的情况
BOOL RayIntersectionVertical(KG3DScene& sceneIn, const D3DXVECTOR3& vSrc
									, FLOAT* pfIntersectionHeight, KG3DRepresentObject** ppObject, DWORD dwMask
									, KG3DIntersectionFilter* pFiler = NULL);//dwMask是EM_RAY_INTERSECTION_MASK的组合，如果传入了ppObject且Mask指定与Object碰撞，会返回碰撞的Object指针，不加引用计数。pfIntersectionHeight可以为NULL。返回的是碰撞点的高度，和其它函数有点区别。

//不需要显式使用下面这些函数版本，用上面的RayIntersectionEntity，Scene的版本就行了，会根据是否新格式来调用
namespace Private
{
	BOOL RayIntersectionEntity(KG3DSceneOutDoorSpaceMgr& OutDoor
		, const D3DXVECTOR3& vSrc
		, const D3DXVECTOR3& vNormalizedDir
		, FLOAT* pDistance
		, KG3DRepresentObject** ppObj
		, FLOAT fTestRange = EM_RAY_INTERSECTION_MAX_RANGE
		, KG3DIntersectionFilter* pFilter = NULL);//参数意义同上

	BOOL RayIntersectionEntity(KG3DSceneEntityContainer& Container
		, const D3DXVECTOR3& vSrc
		, const D3DXVECTOR3& vNormalizedDir
		, FLOAT* pDistance
		, KG3DRepresentObject** ppObj
		, FLOAT fTestRange
		, KG3DIntersectionFilter* pFilter
		, BOOL bNewFormatOrOldFormat);//用两点式和Container碰撞，得到最近的交点和最近的Obj，结果为隐式P(t) = vSrc + vDir * t中的t。最后两个参数选。光检查静态物体。测试范围是全Container

	BOOL RayIntersectionEntity(KG3DSceneEntity& Entity
		, const D3DXVECTOR3& vSrc
		, const D3DXVECTOR3& vNormalizedDir
		, FLOAT* pRet
		, KG3DIntersectionFilter* pFilter = NULL);

/*
	//下面是多线的版本，用于Intersector。上面的版本可以用下面的版本实现
	BOOL RayIntersectionEntityMulti(KG3DSceneEntity& Entity
		, const D3DXVECTOR3* pvSrcArray
		, const D3DXVECTOR3* pvNormalizedDirArray
		, FLOAT* pRetArray
		, size_t Count
		, KG3DIntersectionFilter* pFilter = NULL);//pRetArray可选

	BOOL RayIntersectionEntityNoOutDoorMulti(KG3DScene& Scene
		, const D3DXVECTOR3* pvSrcArray
		, const D3DXVECTOR3* pvNormalizedDirArray
		, FLOAT* pRetArray
		, KG3DSceneEntity** ppEntityIntersectedArray	//每条线返回最近的，可能重复
		, FLOAT* fTestRangeArray
		, size_t Count;
		, KG3DIntersectionFilter* pFilter = NULL);//ppEntityIntersected和pRetArray可选

	BOOL RayIntersectionEntityMulti(KG3DSceneEntityContainer& Container
		, const D3DXVECTOR3* pvSrcArray
		, const D3DXVECTOR3* pvNormalizedDirArray
		, FLOAT* pDistanceArray
		, KG3DRepresentObject** ppObjArray
		, FLOAT* fTestRangeArray
		, size_t Count;
		, KG3DIntersectionFilter* pFilter
		, BOOL bNewFormatOrOldFormat);
*/
};

#endif //_INCLUDE_KG3DINTERSECTION_H_
