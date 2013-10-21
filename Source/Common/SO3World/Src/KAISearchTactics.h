#ifndef _KAISEARCH_TACTICS_H_
#define _KAISEARCH_TACTICS_H_

#include <list>
#include <vector>

#include "KSceneObject.h"
#include "KScene.h"
#include "KCharacter.h"
#include "KRegion.h"

struct KVirtualMove 
{
    int m_nSrcX;
    int m_nSrcY;
    int m_nSrcZ;

    int m_nDstX;
    int m_nDstY;
    int m_nDstZ;

    int m_nDistance;

    int m_State;
    // ... ...


}; 

struct KSearchNode
{
	KCharacter* m_pCharacter;
	int m_nDistance2;
};

bool    g_CompByDistance(const KSearchNode& left, const KSearchNode& right);
double  g_GetRangeByLevelDiff(int nLevelDiff);

template <typename T>
void AISearchPlayer(T& rTactic)
{
    BOOL          bRetCode   = false;
    KSceneObject* pCenterObj = (KSceneObject*)rTactic.m_pSelf;
    KScene*       pScene     = NULL;

    assert(pCenterObj);

    pScene = pCenterObj->m_pScene;
    KGLOG_PROCESS_ERROR(pScene);

    pScene->TraverseRangePlayer(pCenterObj->m_pRegion, rTactic, pScene->m_nBroadcastRegion);

Exit0:
    return;
}

template <typename T>
void AISearchNpc(T& rTactic)
{
    BOOL          bRetCode   = false;
    KSceneObject* pCenterObj = (KSceneObject*)rTactic.m_pSelf;
    KScene*       pScene     = NULL;

    assert(pCenterObj);

    pScene = pCenterObj->m_pScene;
    KGLOG_PROCESS_ERROR(pScene);

    pScene->TraverseRangeNpc(pCenterObj->m_pRegion, rTactic, pScene->m_nBroadcastRegion);

Exit0:
    return;
}

template <typename T>
void AISearchCharacter(T& rTactic)
{
	BOOL          bRetCode   = false;
	KSceneObject* pCenterObj = (KSceneObject*)rTactic.m_pSelf;
    KScene*       pScene     = NULL;

    assert(pCenterObj);

    pScene = pCenterObj->m_pScene;
    KGLOG_PROCESS_ERROR(pScene);

    bRetCode = pScene->TraverseRangePlayer(pCenterObj->m_pRegion, rTactic, pScene->m_nBroadcastRegion);
    KG_PROCESS_ERROR(bRetCode);

    bRetCode = pScene->TraverseRangeNpc(pCenterObj->m_pRegion, rTactic, pScene->m_nBroadcastRegion);
    KG_PROCESS_ERROR(bRetCode);

Exit0:
	return;
}

template <typename T>
void AISearchDoodad(T& rTactic)
{
    BOOL          bRetCode   = false;
    KSceneObject* pCenterObj = (KSceneObject*)rTactic.m_pSelf;
    KScene*       pScene     = NULL;

    assert(pCenterObj);

    pScene = pCenterObj->m_pScene;
    KGLOG_PROCESS_ERROR(pScene);

    pScene->TraverseRangeDoodad(pCenterObj->m_pRegion, rTactic, pScene->m_nBroadcastRegion);

Exit0:
    return;
}

template<typename TSelfType>
struct KSearchTactic
{
    KSearchTactic() : m_pSelf(NULL){}
    TSelfType* m_pSelf;
};

struct KSearchForAnyDoodad : KSearchTactic<KPlayer>
{
	KSearchForAnyDoodad() : m_pResult(NULL), m_nDistance(INT_MAX){}
	int m_nDistance;
	DWORD m_dwDoodadTemplateID;
	KDoodad* m_pResult;
	BOOL operator()(KDoodad* pDoodad);
};

struct KSearchForTeamMember : KSearchTactic<KPlayer>
{
    KSearchForTeamMember() : m_nDistance(INT_MAX){ m_Result.reserve(16); }
    int m_nDistance;
    typedef std::vector<KPlayer*, KMemory::KAllocator<KPlayer*> > KResultVec;
    KResultVec m_Result;
    BOOL operator()(KPlayer* pPlayer);
};

struct KSearchForAnyVisiblePlayer : KSearchTactic<KCharacter>
{
    KSearchForAnyVisiblePlayer() : m_pResult(NULL){}
    KPlayer* m_pResult;
    BOOL operator()(KPlayer* pPlayer);
};


struct KSearchVisiblePlayerClosest : KSearchTactic<KCharacter>
{
    KSearchVisiblePlayerClosest() : m_pResult(NULL), m_nDistance2(INT_MAX){}
    KPlayer* m_pResult;
    int      m_nDistance2;
    BOOL operator()(KPlayer* pPlayer);
};

struct KSearchForAnyVisibleNpc : KSearchTactic<KCharacter>
{
    KSearchForAnyVisibleNpc() : m_pResult(NULL){}
    KNpc* m_pResult;
    BOOL operator()(KNpc* pPlayer);
};

//临时代码，TAB用，加入角度限制，返回距离信息
//TODO:增加是否判断障碍的选项
struct KSearchForCharacter : KSearchTactic<KCharacter>
{
	KSearchForCharacter() : m_nDistance2(INT_MAX)
        , m_nAngle(DIRECTION_COUNT)
        , m_nRelation(0)
        , m_nLifeConversionLevel(0)
        , m_bAdjustByVisible(true)
        , m_bAdjustByAttribute(false)
        , m_bAdjustRangeByLevelDiff(false)
        , m_bAdjustRangeByTargetLife(false)
        { m_Result.reserve(16);}
	int m_nDistance2;
	int m_nAngle;
	int m_nRelation;
    int m_nLifeConversionLevel;
    BOOL m_bAdjustByVisible;            //3D障碍
	BOOL m_bAdjustByAttribute;		    //根据属性调整掉等级
	BOOL m_bAdjustRangeByLevelDiff;     //根据等级差修正查找范围
    BOOL m_bAdjustRangeByTargetLife;    //根据目标血量修正查找范围
    typedef std::vector<KSearchNode, KMemory::KAllocator<KSearchNode> > KResultVec;
    KResultVec m_Result;
	BOOL operator()(KCharacter* pCharacter);
};

//这个是只找一个就返回的版本
struct KSearchForAnyCharacter : KSearchTactic<KCharacter>
{
	KSearchForAnyCharacter() : m_nDistance2(INT_MAX)
        , m_nAngle(DIRECTION_COUNT)
        , m_nRelation(0)
        , m_nLifeConversionLevel(0)
        , m_pResult(NULL)
        , m_bAdjustByVisible(true)
        , m_bAdjustByAttribute(false)
        , m_bAdjustRangeByLevelDiff(false)
        , m_bAdjustRangeByTargetLife(false){}
	int m_nDistance2;
	int m_nAngle;
	int m_nRelation;	
    int m_nLifeConversionLevel;
    BOOL m_bAdjustByVisible;            //3D障碍
    BOOL m_bAdjustRangeByTargetLife;    //根据目标血量修正查找范围
    BOOL m_bAdjustRangeByLevelDiff;     //根据等级差修正查找范围
    BOOL m_bAdjustByAttribute;		    //根据属性调整掉等级
	KCharacter* m_pResult;
	BOOL operator()(KCharacter* pCharacter);
};

struct KSearchForMultiCharacterAddThreatList : KSearchTactic<KCharacter>
{
    KSearchForMultiCharacterAddThreatList() : m_nDistance2(INT_MAX)
        , m_nAngle(DIRECTION_COUNT)
        , m_nRelation(0)
        , m_pResult(NULL)
        , m_nLifeConversionLevel(0)
        , m_bAdjustByVisible(true)
        , m_bAdjustByAttribute(false)
        , m_bAdjustRangeByLevelDiff(false)
        , m_bAdjustRangeByTargetLife(false)
        , m_nCharacterCounter(1){}
    int m_nDistance2;
    int m_nAngle;
    int m_nRelation;
    int m_nCharacterCounter;
    int m_nLifeConversionLevel;
    BOOL m_bAdjustByVisible;            //3D障碍
    BOOL m_bAdjustRangeByLevelDiff;     //根据等级差修正查找范围
    BOOL m_bAdjustRangeByTargetLife;    //根据目标血量修正查找范围
    BOOL m_bAdjustByAttribute;		    //根据属性调整掉等级
    KCharacter* m_pResult;
    BOOL operator()(KCharacter* pCharacter);
};

#ifdef _SERVER
// 搜索所有非法斗殴的Player,返回一个组对象
struct KSearchBustupCharacterList : KSearchTactic<KCharacter>
{
    KSearchBustupCharacterList() : m_nDistance2(INT_MAX)
        , m_nAngle(DIRECTION_COUNT)
        , m_nRelation(0)
        , m_nLifeConversionLevel(0)
        , m_bAdjustByVisible(true)
        , m_bAdjustByAttribute(false)
        , m_bAdjustRangeByLevelDiff(false)
        , m_bAdjustRangeByTargetLife(false)
        , m_nCharacterCounter(1){}
    int m_nDistance2;
    int m_nAngle;
    int m_nRelation;
    int m_nCharacterCounter;
    int m_nLifeConversionLevel;
    BOOL m_bAdjustByVisible;                //3D障碍
    BOOL m_bAdjustRangeByLevelDiff;         //根据等级差修正查找范围
    BOOL m_bAdjustRangeByTargetLife;        //根据目标血量修正查找范围
    BOOL m_bAdjustByAttribute;              //根据属性调整掉等级
    typedef std::set<KCharacter*, std::less<KCharacter*>, KMemory::KAllocator<KCharacter*> > KResultSet;
    KResultSet m_Result;  //返回的数据
    BOOL operator()(KCharacter* pCharacter);
};
#endif

#endif //_KAISEARCH_TACTICS_H_
