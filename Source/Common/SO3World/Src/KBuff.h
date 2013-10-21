/************************************************************************/
/* Copyright : Kingsoft 2008										    */
/* Author	 : Chen Jie 											    */
/* History	 :															*/
/*		2008.08.12	Create												*/
/************************************************************************/

#ifndef _KBUFF_H_
#define _KBUFF_H_

#include "Luna.h"

class   KCharacter;
class   KAttribute;
struct  KBUFF_LIST_NODE;

#define MAX_BUFF_REICPE_COUNT           1

class KBuff
{
public:
    DWORD           m_dwSerialNumber;

    DWORD           m_dwID;
    int             m_nLevel;

    int             m_nKindType;
    int             m_nFunctionType;    // 该类别用于Buff的作用效果和免疫
    int             m_nBuffType;        // 该类别用于清除Buff(由一些事件引起而清除某类别Buff, 比如离开打坐状态时,就会清除打坐的Buff)

    int             m_nAppendType;      // 该类别用于Buff叠加,取代,并存
    int             m_nDetachType;      // 该类别用于驱散Buff
    int             m_nDecayType;       // 该类别用于效果递减

    int             m_nRepresentPos;    // 特效表现的位置,是踩在脚底下的,顶在头上的,还是缠绕在身上的
    DWORD           m_dwRepresentID;

    int             m_nIntensity;       // 用于界定两个Buff的取代

    BOOL            m_bIsStackable;     // 是否可以叠加
    int             m_nMaxStackNum;     // 最大叠加上限

    int             m_nActiveCount;     // 跳数
    int             m_nIntervalFrame;   // 每一跳的间隔帧数
    int             m_nDamageAddPercent;  // 秘笈修改伤害(治疗)

    BOOL            m_bExclude;
    BOOL            m_bGlobalExclude;   // 全局冲突，为false则该BUFF只按施放者判断冲突
    BOOL            m_bUniqueTarget;
    BOOL            m_bCanCancel;       // 是否可由玩家使用右键主动驱散
    BOOL            m_bIsCountable;     // 是否为计数BUFF
    BOOL            m_bSave;
    BOOL            m_bIsOffLineClock;  // 是否下线记时

    char            m_szScript[MAX_SCIRPT_NAME_LEN];

    KAttribute*     m_pBeginAttribList;   // 可回滚
    KAttribute*     m_pActiveAttribList;  // 不可回滚
    KAttribute*     m_pEndTimeAttribList; // 不可回滚

#ifdef _SERVER
    int             m_nPersistMoveStateMask;
    int             m_nPersistManaShield;
    int             m_nPersistFight;
    int             m_nPersistHorse;
    int             m_nPersistTerrain;
    BOOL            m_bPersistFormationLeader;
    DWORD           m_dwMapBanMask;
#endif

    BOOL Apply(KCharacter* pCharacter, KBUFF_LIST_NODE& rBuffListNode);
    BOOL UnApply(KCharacter* pCharacter, KBUFF_LIST_NODE& rBuffListNode);
#ifdef _SERVER
    BOOL Active(KCharacter* pCharacter, KBUFF_LIST_NODE& rBuffListNode);
    BOOL EndTime(KCharacter* pCharacter, KBUFF_LIST_NODE& rBuffListNode);
#endif

    DECLARE_LUA_CLASS(KBuff);
    DECLARE_LUA_STRUCT_INTEGER(Level, m_nLevel);
    DECLARE_LUA_STRUCT_INTEGER(Count, m_nActiveCount);
    DECLARE_LUA_STRUCT_INTEGER(Interval, m_nIntervalFrame);
    int LuaGetAttribute(Lua_State* L);

private:
#ifdef _SERVER
    BOOL ApplyAttributeNotRollBack(KCharacter* pCharacter, KBUFF_LIST_NODE& rBuffListNode, KAttribute* pAttribute);
#endif
};

struct KBUFF_RECIPE_KEY
{
    DWORD   dwID;
    int     nLevel;
    DWORD   dwRecipeKey[MAX_BUFF_REICPE_COUNT];
};

bool operator <  (const KBUFF_RECIPE_KEY& lhs, const KBUFF_RECIPE_KEY& rhs);
bool operator == (const KBUFF_RECIPE_KEY& lhs, const KBUFF_RECIPE_KEY& rhs);

struct KBUFF_RECIPE_INFO
{
    DWORD       dwID;
    int         nLevel;

    int         nIntensityAdd;
    int         nCountAdd;
    int         nIntervalFrameAdd;
    int         nDamageAddPercent;

    KAttribute* pBeginAttributeListAdd;
    KAttribute* pActiveAttributeListAdd;
    KAttribute* pEndTimeAttributeListAdd;
};

class KBuffRecipePointer 
{
public:
    KBuffRecipePointer();
    KBuff*              SetRecipeKey(const KBUFF_RECIPE_KEY& crRecipeKey);
    KBUFF_RECIPE_KEY&   GetRecipeKey();
    void                ClearRecipeKey();
    KBuff*              GetPointer();

private:
    KBuff*              m_pBuffInfo;
    DWORD               m_dwSerialNumber;
    KBUFF_RECIPE_KEY    m_BuffRecipeKey;
};

#endif //_KBUFF_H_
