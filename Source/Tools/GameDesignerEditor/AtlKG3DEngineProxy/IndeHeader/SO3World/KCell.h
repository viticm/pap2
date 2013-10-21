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

#define  MAX_PARTITION_INDEX_BIT    12

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

struct KCellBaseInfo
{
	DWORD		dwCellType			:	2;		// 地表类型		
	DWORD		dwGradientDirection :   3;		// 滑动方向,8个方向,合并时取最顶的
    DWORD		dwGradientDegree    :	3;		// 坡度,单位: 90度圆弧的1/8,合并时取最顶上的,超过90度取补角
	DWORD		dwBarrierDirection  :	3;		// 障碍方向,单位: 180度圆弧的1/8,合并时取倾斜最大的
    DWORD		dwDynamic			:	1;		// 是否动态Cell
	DWORD		dwFaceUp			:	1;		// 是否面向上，用于编辑时合并CELL
    DWORD       dwNavigationIndex   :   7;      // 导航索引
    DWORD       dwPartitionIndex    :   MAX_PARTITION_INDEX_BIT;     // 所属区域索引, 4096个区域
};

struct KCell
{
    struct KBaseInfo
	{
		DWORD		dwCellType			:	2;		// 地表类型		
		DWORD		dwIndoor			:	1;		// 是否室内
		DWORD		dwPassWidth			:	3;		// 通过物限制
		DWORD		dwGradientDirection :   3;		// 滑动方向,8个方向,合并时取最顶的
        DWORD		dwGradientDegree    :	3;		// 坡度,单位: 90度圆弧的1/8,合并时取最顶上的,超过90度取补角
		DWORD		dwBarrierDirection  :	3;		// 障碍方向,单位: 180度圆弧的1/8,合并时取倾斜最大的
		DWORD		dwFaceUp			:	1;		// 是否面向上，用于编辑时合并CELL
        DWORD		dwDynamic			:	1;		// 是否动态Cell

        DWORD		dwNoObstacleRange	:	6;		// 无障碍范围
        DWORD		dwScriptIndex		:	4;		// 脚本索引
		DWORD		dwPutObj			:	1;		// 可摆放	
		DWORD		dwRest				:	1;		// 是否是休息区
		DWORD		dwStall			    :	1;		// 是否可摆摊
        DWORD		dwRideHorse		    :	1;		// 是否下马
		DWORD		dwBlockCharacter	:	1;		// 障碍信息
    } m_BaseInfo;

	WORD    m_wLowLayer;					// 下层高度(以海平面为基准)
	WORD    m_wHighLayer;					// 上层高度(以海平面为基准)
	KCell*  m_pNext;						// 面片链表

    inline void SetGradientDegree(int nDegree)
    { 
        assert(nDegree >= 0 && nDegree < DIRECTION_COUNT / 4);
        m_BaseInfo.dwGradientDegree = nDegree / (DIRECTION_COUNT / 4 / 8);
    }

    inline int GetGradientDegree()
    { 
        return m_BaseInfo.dwGradientDegree * DIRECTION_COUNT / 4 / 8; 
    }

    inline void SetGradientDirection(int nDirection)
    { 
        assert(nDirection >= 0 && nDirection < DIRECTION_COUNT);
        m_BaseInfo.dwGradientDirection = nDirection / (DIRECTION_COUNT / 8);
    }

    inline int GetGradientDirection()
    { 
        return m_BaseInfo.dwGradientDirection * DIRECTION_COUNT / 8; 
    }

    inline void SetBarrierDirection(int nDirection)
    { 
        assert(nDirection >= 0 && nDirection < DIRECTION_COUNT);
        if (nDirection >= DIRECTION_COUNT / 2)
        {
            nDirection -= DIRECTION_COUNT / 2;
        }
        m_BaseInfo.dwBarrierDirection = nDirection / (DIRECTION_COUNT / 2 / 8);
    }

    inline int GetBarrierDirection()
    { 
        return m_BaseInfo.dwBarrierDirection * DIRECTION_COUNT / 2 / 8; 
    }

    inline void SetWaterFlowSpeed(int nSpeed)
    {
    }

    inline int GetWaterFlowSpeed()
    {
        return 0;
    }

    inline void SetWaterFlowDirection(int nDirection)
    {
    }

    inline int GetWaterFlowDirection()
    {
        return 0;
    }

    inline int GetStaticFrictionCoefficient()
    {
        assert(false);
        return 0;
    }

    inline int GetDynamicFrictionCoefficient()
    {
        const int nDynamicFrictionCoefficients[ctTotal] = {768, 0, 128};
        return nDynamicFrictionCoefficients[m_BaseInfo.dwCellType];
    }

    inline int GetCriticalSlipSpeed()
    {
        const int nCriticalSlipSpeed[ctTotal] = {24, 0, 10};
        return nCriticalSlipSpeed[m_BaseInfo.dwCellType];
    }  

    inline BOOL CheckSlip()
    {
        const int nCriticalGradientDegree[ctTotal]  = {4, 8, 1};
        int       nCellType                         = m_BaseInfo.dwCellType;
        int       nGradientDegree                   = m_BaseInfo.dwGradientDegree;

        // 陆地过45度打滑,水面不能做判断打滑(要判断水底地表的打滑),冰面永远打滑
        if (nCellType == ctWater)
        {
            return false;
        }

        if (nCellType == ctIce)
        {
            return true;
        }

        return nGradientDegree >= nCriticalGradientDegree[nCellType];
    }


	DECLARE_LUA_CLASS(KCell);
	DECLARE_LUA_STRUCT_INTEGER(IsBlockCharacter, m_BaseInfo.dwBlockCharacter);
	DECLARE_LUA_STRUCT_INTEGER(CanPutObj, m_BaseInfo.dwPutObj);
	DECLARE_LUA_STRUCT_INTEGER(IsSafe, m_BaseInfo.dwRest);
	DECLARE_LUA_STRUCT_INTEGER(CanStall, m_BaseInfo.dwStall);
	DECLARE_LUA_STRUCT_INTEGER(Type, m_BaseInfo.dwCellType);
};

struct KDynamicCell : KCell 
{
    WORD m_wGroupIndex;
};

#pragma pack()


#endif	//_KCELL_H_
