/************************************************************************/
/* 地图上的单元格								                        */
/* Copyright : Kingsoft 2004										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2005.01.12	Create												*/
/* Comment	 :															*/
/*		KCell表示空间中一个底面为0.5M*0.5M,高度可设定的长方体			*/
/*		其拥有上下两个表面,上表面可站立,因此包括了上表面的障碍相关信息	*/
/************************************************************************/
#ifndef _KCELL_H_
#define _KCELL_H_
#include "Luna.h"
#include "Global.h"

#define MAX_CHARACTER_COUNT_ON_CELL	255
#define MAX_DOODAD_COUNT_ON_CELL	3

class KPlayer;
class KNpc;
class KDoodad;
class KMissile;

enum CELL_TYPE
{
	ctPlain = 0,
	ctWater,
    ctIce,
	ctTotal
};

#pragma pack(1)
struct KCell
{
	struct KBaseInfo
	{
		DWORD		dwScriptIndex		:	4;		// 脚本索引
		DWORD		dwBlockCharacter	:	1;		// 障碍信息
		DWORD		dwPutObj			:	1;		// 可摆放		
		DWORD		dwRest				:	1;		// 是否是休息区
		DWORD		dwStall			    :	1;		// 是否可摆摊
		DWORD		dwRideHorse		    :	1;		// 是否下马
		DWORD		dwIndoor			:	1;		// 是否室内
		DWORD		dwCellType			:	2;		// 地表类型		
		DWORD		dwPassWidth			:	3;		// 通过物限制
		DWORD		dwNoObstacleRange	:	6;		// 无障碍范围
		DWORD		dwGradientDirection :   3;		// 滑动方向,8个方向,合并时取最顶的
		DWORD		dwGradientDegree    :	3;		// 坡度,单位: 90度圆弧的1/8,合并时取最顶上的,超过90度取补角
		DWORD		dwBarrierDirection  :	3;		// 障碍方向,单位: 180度圆弧的1/8,合并时取倾斜最大的
		DWORD		dwFaceUp			:	1;		// 是否面向上，用于编辑时合并CELL
		DWORD		dwReserve			:	1;		// 保留
	} m_BaseInfo;
	WORD				m_wLowLayer;					// 下层高度(以海平面为基准)
	WORD				m_wHighLayer;					// 上层高度(以海平面为基准)
	KCell*				m_pNext;						// 面片链表

	BOOL AddNpc(KNpc* pNpc);
	BOOL AddDoodad(KDoodad* pDoodad);
	BOOL AddPlayer(KPlayer* pPlayer);

	BOOL RemoveNpc(KNpc* pNpc);
	BOOL RemoveDoodad(KDoodad* pDoodad);
	BOOL RemovePlayer(KPlayer* pPlayer);

    void SetGradientDegree(int nDegree)
    { 
        ASSERT(nDegree >= 0 && nDegree < DIRECTION_COUNT / 4);
        m_BaseInfo.dwGradientDegree = nDegree / (DIRECTION_COUNT / 4 / 8);
    }

    int GetGradientDegree()
    { 
        return m_BaseInfo.dwGradientDegree * DIRECTION_COUNT / 4 / 8; 
    }

    void SetGradientDirection(int nDirection)
    { 
        ASSERT(nDirection >= 0 && nDirection < DIRECTION_COUNT);
        m_BaseInfo.dwGradientDirection = nDirection / (DIRECTION_COUNT / 8);
    }

    int GetGradientDirection()
    { 
        return m_BaseInfo.dwGradientDirection * DIRECTION_COUNT / 8; 
    }

    void SetBarrierDirection(int nDirection)
    { 
        ASSERT(nDirection >= 0 && nDirection < DIRECTION_COUNT);
        if (nDirection >= DIRECTION_COUNT / 2)
        {
            nDirection -= DIRECTION_COUNT / 2;
        }
        m_BaseInfo.dwBarrierDirection = nDirection / (DIRECTION_COUNT / 2 / 8);
    }

    int GetBarrierDirection()
    { 
        return m_BaseInfo.dwBarrierDirection * DIRECTION_COUNT / 2 / 8; 
    }

public:
	//Lua脚本接口
	DECLARE_LUA_CLASS(KCell);

	DECLARE_LUA_STRUCT_INTEGER(IsBlockCharacter, m_BaseInfo.dwBlockCharacter);
	DECLARE_LUA_STRUCT_INTEGER(CanPutObj, m_BaseInfo.dwPutObj);
	DECLARE_LUA_STRUCT_INTEGER(IsSafe, m_BaseInfo.dwRest);
	DECLARE_LUA_STRUCT_INTEGER(CanStall, m_BaseInfo.dwStall);
	DECLARE_LUA_STRUCT_INTEGER(Type, m_BaseInfo.dwCellType);
};
#pragma pack()


#endif	//_KCELL_H_