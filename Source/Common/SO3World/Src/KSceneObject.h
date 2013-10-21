/************************************************************************/
/* 场景物体			                                                    */
/* Copyright : Kingsoft 2005										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2004.12.30	Create												*/
/* Comment	 :															*/
/*		所有在地图上显示的物体都是场景物体								*/
/************************************************************************/
#ifndef _KSCENEOBJECT_H_
#define	_KSCENEOBJECT_H_

#include "Global.h"
#include "KBaseObject.h"
#include "KSceneObjNode.h"
#include "KCell.h"
#include "Luna.h"
#include "KMath.h"

class KRegion;
class KScene;

class KSceneObject : public KBaseObject
{
public:
	KSceneObject();
	virtual ~KSceneObject();
	
	BOOL	Init();
	void	UnInit();

	BOOL	CheckGameLoop();

	BOOL    GetAbsoluteCoordinate(int* pnX, int* pnY, int* pnZ);

    virtual void SetCell(KCell* pDstCell);

public:
	int			m_nX;
	int			m_nY;
	int			m_nZ;

    int         m_nTouchRange;      // 受击范围，触及范围，物体半径

	// 当前Region中的格子坐标，Region左下角为原点
	int			m_nXCell;			// Region中的格子X坐标		单位：格子
	int			m_nYCell;			// Region中的格子Y坐标		单位：格子

	int			m_nMass;			// 质量						单位：g
	
	int			m_nWidth;			// 宽度，X轴上的				单位：点
	int			m_nLength;			// 长度，Y轴上的				单位：点
	int			m_nHeight;			// 厚度，Z轴上的				单位：点
	
	int			m_nFaceDirection;	// 物体正面的朝向,和速度无关	单位: (360/256)度
    int         m_nPitchDirection;  // 俯仰角,单位(360/256)度

public:
	int						    m_nGameLoop;		// 游戏循环数，主要用来判断重复计算

#ifdef _CLIENT
	void*                       m_pRepresentObject;	// 表现逻辑对象
#endif

	KCell*						m_pCell;			// 所属的Cell
	KRegion*					m_pRegion;			// 所属的Region
	KScene*						m_pScene;			// 所属的地图
	KSceneObjNode				m_RegionObjNode;	// 用于维护KRegion中的场景对象链表

public:
	DECLARE_LUA_INTEGER(X);
	DECLARE_LUA_INTEGER(Y);
	DECLARE_LUA_INTEGER(Z);
    DECLARE_LUA_INTEGER(TouchRange);
	DECLARE_LUA_INTEGER(XCell);
	DECLARE_LUA_INTEGER(YCell);
	DECLARE_LUA_INTEGER(Mass);
	DECLARE_LUA_INTEGER(Width);
	DECLARE_LUA_INTEGER(Length);
	DECLARE_LUA_INTEGER(Height);
	DECLARE_LUA_INTEGER(FaceDirection);
    DECLARE_LUA_INTEGER(PitchDirection);

	int LuaGetCell(Lua_State* L);
	int LuaGetScene(Lua_State* L);
    int LuaGetAbsoluteCoordinate(Lua_State* L);
};

inline BOOL g_InRange(KSceneObject* pObjA, KSceneObject* pObjB, int nRange)
{
    BOOL    bResult     = false;
    BOOL    bRetCode    = false;
    int     nXa         = 0;
    int     nYa         = 0;
    int     nZa         = 0;
    int     nXb         = 0;
    int     nYb         = 0;
    int     nZb         = 0;

    assert(pObjA);
    assert(pObjB);

    KG_PROCESS_ERROR(pObjA->m_pScene);
    KG_PROCESS_ERROR(pObjB->m_pScene);

    KG_PROCESS_ERROR(pObjA->m_pScene == pObjB->m_pScene);

    bRetCode = pObjA->GetAbsoluteCoordinate(&nXa, &nYa, &nZa);
    KG_PROCESS_ERROR(bRetCode);

    bRetCode = pObjB->GetAbsoluteCoordinate(&nXb, &nYb, &nZb);
    KG_PROCESS_ERROR(bRetCode);

    bRetCode = g_InRange(nXa, nYa, ZPOINT_TO_XYPOINT(nZa), nXb, nYb, ZPOINT_TO_XYPOINT(nZb), nRange);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

inline IN_RANGE_RESULT g_InRange(KSceneObject* pObjA, KSceneObject* pObjB, int nMinRange, int nMaxRange)
{
    IN_RANGE_RESULT nResult     = irrInvalid;
    BOOL            bRetCode    = false;
    int             nXa         = 0;
    int             nYa         = 0;
    int             nZa         = 0;
    int             nXb         = 0;
    int             nYb         = 0;
    int             nZb         = 0;

    assert(pObjA);
    assert(pObjB);

    KG_PROCESS_ERROR(pObjA->m_pScene);
    KG_PROCESS_ERROR(pObjB->m_pScene);

    KG_PROCESS_ERROR(pObjA->m_pScene == pObjB->m_pScene);

    bRetCode = pObjA->GetAbsoluteCoordinate(&nXa, &nYa, &nZa);
    KG_PROCESS_ERROR(bRetCode);

    bRetCode = pObjB->GetAbsoluteCoordinate(&nXb, &nYb, &nZb);
    KG_PROCESS_ERROR(bRetCode);

    nResult = g_InRange(nXa, nYa, ZPOINT_TO_XYPOINT(nZa), nXb, nYb, ZPOINT_TO_XYPOINT(nZb), nMinRange, nMaxRange);

Exit0:
    return nResult;
}

#endif	// _KSCENEOBJECT_H_
