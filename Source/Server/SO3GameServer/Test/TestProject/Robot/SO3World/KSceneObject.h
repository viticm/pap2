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

class KRegion;
class KScene;
class KMonster;

enum SCENE_OBJ_TYPE
{
	sotInvalid = 0,
	sotNpc,
	sotPlayer,
	sotDoodad,

	sotTotal
};

class KSceneObject : public KBaseObject
{
public:

	KSceneObject();
    KMonster *m_pOwner;
	virtual ~KSceneObject();
	
	BOOL	Init();
	BOOL	UnInit(void);

	BOOL	CheckGameLoop(void);

public:
	//原先为：当前Region中的点坐标，Region左下角为原点
	//2005.4.26改为：在整个地图上的全局点坐标，地图的左下角为原点
	//三套坐标的使用：
	//m_nX：用于确定场景物体在整个地图上的位置，配置文件、网络传输中使用的都是这个坐标
	//m_nXinCell：用于判断半格障碍,2005.07.26取消半格障碍
	//m_nXCell：用于快速访问Region中的格子数组
	//2005.06.14	更改每格象素点数为32*32
	//2006.03.02	更改每格象素点数为16*16
	int			m_nX;				// 地图中的X坐标				单位：点（100/64cm）
	int			m_nY;				// 地图中的Y坐标				单位：点
	int			m_nZ;				// 地图中的Z坐标，距地表的相对高度高度		单位：点

#if defined(_EDITOR)
	int			m_nEditorZ;			// 编辑器用的Z坐标，记录绝对高度
#endif	//_EDITOR

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
	__int64						m_nGameLoop;		// 游戏循环数，主要用来判断重复计算

#if defined(_SERVER) | defined(_EDITOR)
	int							m_nLastBroadcastFrame;
	int							m_nEnterRegionFrame[ALL_DIRECTION + 1];	
	int							m_nChangeDirection; //上次跨Region的方向
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

	void HostGetAbsoluteCoordinate(int& nX, int& nY, int& nZ) const;

	int LuaGetCell(Lua_State* L);
	int LuaGetScene(Lua_State* L);
    int LuaGetAbsoluteCoordinate(Lua_State* L);
};
#endif	//_SERVER