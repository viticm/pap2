#include "stdafx.h"
#include "KPlayer.h"
#include "KPlayerServer.h"
#include "KSkillRecipeList.h"
#include "KSO3World.h"

#define MAX_SKILL_REICPE_LIST_SIZE  1024

inline bool _RecipeListNodeCompare(const KG_SKILL_RECIPE_LIST_NODE& crLeft, const KG_SKILL_RECIPE_LIST_NODE& crRight)
{
    assert(sizeof(DWORD) == 4);
    return (crLeft.dwRecipeKey & 0xFFFF0000) < (crRight.dwRecipeKey & 0xFFFF0000);
}

BOOL KSkillRecipeList::Init(KPlayer* pPlayer)
{
    BOOL bResult = false;

    KGLOG_PROCESS_ERROR(pPlayer);

    m_pPlayer = pPlayer;

    bResult = true;
Exit0:
    return bResult;
}

void KSkillRecipeList::UnInit()
{
    Clear();
}

BOOL KSkillRecipeList::CanAdd(DWORD dwRecipeKey)
{
    BOOL                            bResult = false;
    KG_SKILL_RECIPE_LIST_NODE       Node;
    KG_SKILL_RECIPE_LIST::iterator  it;

    assert(sizeof(DWORD) == 4);

    Node.dwRecipeKey    = dwRecipeKey;
    Node.bActive        = false;

    it = std::lower_bound(m_SkillRecipeVector.begin(), m_SkillRecipeVector.end(), Node, _RecipeListNodeCompare);

    if (
        it != m_SkillRecipeVector.end() &&
        (it->dwRecipeKey & 0xFFFF0000) == (dwRecipeKey & 0xFFFF0000) &&
        (it->dwRecipeKey & 0x0000FFFF) >= (dwRecipeKey & 0x0000FFFF)
    )
    {
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KSkillRecipeList::Add(DWORD dwRecipeKey)
{
    BOOL                        bResult     = false;
    BOOL                        bRetCode    = false;
    KSKILL_RECIPE_BASE_INFO*    pRecipeInfo = NULL;

    pRecipeInfo = g_pSO3World->m_SkillManager.GetSkillRecipeBaseInfo(dwRecipeKey);
    KGLOG_PROCESS_ERROR(pRecipeInfo);

    bRetCode = _Add(dwRecipeKey, false);
    KGLOG_PROCESS_ERROR(bRetCode);

#ifdef _SERVER
    g_PlayerServer.DoSyncSkillRecipe(m_pPlayer->m_nConnIndex, m_SkillRecipeVector.size(), &m_SkillRecipeVector[0]);

    if (!pRecipeInfo->bIsSystemRecipe)
    {
        KG_ADD_SKILL_RECIPE_NOTIFY Notify;
        GET_RECIPE_ID_OF_KEY(Notify.dwRecipeID, dwRecipeKey);
        GET_RECIPE_LEVEL_OF_KEY(Notify.dwRecipeLevel, dwRecipeKey);
        g_PlayerServer.DoMessageNotify(m_pPlayer->m_nConnIndex, ectAddSkillRecipe, 0, &Notify, sizeof(Notify));
    }
#endif

    bResult = true;
Exit0:
    return bResult;
}

BOOL KSkillRecipeList::Delete(DWORD dwRecipeKey)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    KG_SKILL_RECIPE_LIST_NODE       Node;
    KG_SKILL_RECIPE_LIST::iterator  it;

    Node.dwRecipeKey = dwRecipeKey;
    it = std::lower_bound(m_SkillRecipeVector.begin(), m_SkillRecipeVector.end(), Node, _RecipeListNodeCompare);

    bRetCode = (it != m_SkillRecipeVector.end());
    KG_PROCESS_ERROR(bRetCode);

    KG_PROCESS_ERROR(it->dwRecipeKey == dwRecipeKey);

    m_SkillRecipeVector.erase(it);

#ifdef _SERVER
    if (m_SkillRecipeVector.empty())
    {
        g_PlayerServer.DoSyncSkillRecipe(m_pPlayer->m_nConnIndex, 0, NULL);
    }
    else
    {
        g_PlayerServer.DoSyncSkillRecipe(m_pPlayer->m_nConnIndex, m_SkillRecipeVector.size(), &m_SkillRecipeVector[0]);
    }
#endif

    bResult = true;
Exit0:
    return bResult;
}

BOOL KSkillRecipeList::Find(DWORD dwRecipeKey)
{
    BOOL                        bResult     = false;
    KG_SKILL_RECIPE_LIST_NODE*  pNode       = NULL;

    pNode = _Find(dwRecipeKey);
    KG_PROCESS_ERROR(pNode);

    bResult = true;
Exit0:
    return bResult;
}

int KSkillRecipeList::Count()
{
    return (int)m_SkillRecipeVector.size();
}

void KSkillRecipeList::Clear()
{
    m_SkillRecipeVector.clear();
}

class KCheckSkillRecipeCount 
{
public:
    KCheckSkillRecipeCount()
    {
        m_pSkillRecipeList = NULL;
        m_nSkillRecipeType = 0;
        m_bPass = false;
    }

    BOOL Init(KSkillRecipeList* pSkillRecipeList, int nSkillRecipeType)
    {
        BOOL bResult = false;

        KG_PROCESS_ERROR(pSkillRecipeList);
        KG_PROCESS_ERROR(nSkillRecipeType);

        m_pSkillRecipeList  = pSkillRecipeList;
        m_nSkillRecipeType  = nSkillRecipeType;
        m_bPass             = true;

        bResult = true;
    Exit0:
        return bResult;
    }

    BOOL UnInit()
    {
        KCheckSkillRecipeCount();
        return true;
    }

    BOOL IsPass()
    {
        return m_bPass;
    }

    BOOL operator () (DWORD dwSkillID, DWORD dwSkillLevel)
    {
        BOOL    bResult         = false;
        BOOL    bRetCode        = false;
        size_t  uCount          = 0;
        size_t  uActiveCount    = 0;
        KSkill* pSkill          = NULL;

        KGLOG_PROCESS_ERROR(m_pSkillRecipeList);

        pSkill = g_pSO3World->m_SkillManager.GetSkill_RAW(dwSkillID, dwSkillLevel);
        KGLOG_PROCESS_ERROR(pSkill);

        KG_PROCESS_SUCCESS(pSkill->m_pBaseInfo->nRecipeType != m_nSkillRecipeType);

        bRetCode = m_pSkillRecipeList->GetRecipeList(dwSkillID, dwSkillLevel, 0, &uCount, &uActiveCount, NULL);
        KGLOG_PROCESS_ERROR(bRetCode);

        KG_PROCESS_ERROR(uActiveCount < MAX_SKILL_REICPE_COUNT);

    Exit1:
        bResult = true;
    Exit0:
        if (!bResult)
            m_bPass = false;

        return bResult;
    }

private:
    KSkillRecipeList*   m_pSkillRecipeList;
    int                 m_nSkillRecipeType;
    BOOL                m_bPass;
};

BOOL KSkillRecipeList::Active(DWORD dwRecipeKey)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    DWORD                       dwLevel         = 0;
    size_t                      uCount          = 0;
    size_t                      uActiveCount    = 0;
    KG_SKILL_RECIPE_LIST_NODE*  pNode           = NULL;
    KSKILL_RECIPE_BASE_INFO*    pRecipeInfo     = NULL;
    KSkillManager::KSKILL_RECIPE_BASE_INFO_MAP::iterator   it;

    pNode = _Find(dwRecipeKey);
    KG_PROCESS_ERROR(pNode);

    it = g_pSO3World->m_SkillManager.m_SkillRecipeBaseInfoMap.find(dwRecipeKey);
    KGLOG_PROCESS_ERROR(it != g_pSO3World->m_SkillManager.m_SkillRecipeBaseInfoMap.end());

    pRecipeInfo = &(it->second);
    assert(pRecipeInfo);

    if (pRecipeInfo->nSkillRecipeType)
    {
        KCheckSkillRecipeCount CheckSkillRecipeCount;

        bRetCode = CheckSkillRecipeCount.Init(this, pRecipeInfo->nSkillRecipeType);
        KGLOG_PROCESS_ERROR(bRetCode);

        m_pPlayer->m_SkillList.Traverse(CheckSkillRecipeCount);

        bRetCode = CheckSkillRecipeCount.IsPass();
        CheckSkillRecipeCount.UnInit();

        KG_PROCESS_ERROR(bRetCode);
    }

    if (pRecipeInfo->dwSkillID)
    {
        dwLevel = m_pPlayer->m_SkillList.GetSkillLevel(pRecipeInfo->dwSkillID);
        KGLOG_PROCESS_ERROR(dwLevel || pRecipeInfo->nSkillRecipeType);
        KGLOG_PROCESS_ERROR(!pRecipeInfo->dwSkillLevel || dwLevel == pRecipeInfo->dwSkillLevel);

        bRetCode = GetRecipeList(pRecipeInfo->dwSkillID, dwLevel, 0, &uCount, &uActiveCount, NULL);
        KGLOG_PROCESS_ERROR(bRetCode);

        KG_PROCESS_ERROR(uActiveCount < MAX_SKILL_REICPE_COUNT);
    }

    pNode->bActive = true;

#ifdef _SERVER
    g_PlayerServer.DoSyncSkillRecipe(m_pPlayer->m_nConnIndex, m_SkillRecipeVector.size(), &m_SkillRecipeVector[0]);
#endif

    bResult = true;
Exit0:
    return bResult;
}

BOOL KSkillRecipeList::Deactive(DWORD dwRecipeKey)
{
    BOOL                        bResult     = false;
    KG_SKILL_RECIPE_LIST_NODE*  pNode       = NULL;

    pNode = _Find(dwRecipeKey);
    KG_PROCESS_ERROR(pNode);

    pNode->bActive = false;

#ifdef _SERVER
    g_PlayerServer.DoSyncSkillRecipe(m_pPlayer->m_nConnIndex, m_SkillRecipeVector.size(), &m_SkillRecipeVector[0]);
#endif

    bResult = true;
Exit0:
    return bResult;
}

struct KG_GET_USER_SKILL_RECIPE 
{
    KG_GET_USER_SKILL_RECIPE()
    {
        pRecipeSkillBaseInfoMap = NULL;
        dwSkillID               = 0;
        dwSkillLevel            = 0;
        nRecipeType             = 0;
        uCount                  = 0;
        uActiveCount            = 0;
        uMaxArrayLength         = 0;
        pArray                  = NULL;
    };

    BOOL operator() (const KG_SKILL_RECIPE_LIST_NODE& crRecipeListNode);

    KSkillManager::KSKILL_RECIPE_BASE_INFO_MAP* pRecipeSkillBaseInfoMap;
    DWORD                                       dwSkillID;
    DWORD                                       dwSkillLevel;
    int                                         nRecipeType;
    size_t                                      uCount;
    size_t                                      uActiveCount;
    size_t                                      uMaxArrayLength;
    KG_SKILL_RECIPE_LIST_NODE*                  pArray;
};

BOOL KG_GET_USER_SKILL_RECIPE::operator () (const KG_SKILL_RECIPE_LIST_NODE& crRecipeListNode)
{
    BOOL                        bResult                 = false;
    KSKILL_RECIPE_BASE_INFO*    pRecipeSkillBaseInfo    = NULL;
    std::map<DWORD, KSKILL_RECIPE_BASE_INFO>::iterator it;

    assert(pRecipeSkillBaseInfoMap);

    it = pRecipeSkillBaseInfoMap->find(crRecipeListNode.dwRecipeKey);
    KGLOG_PROCESS_ERROR(it != pRecipeSkillBaseInfoMap->end());

    pRecipeSkillBaseInfo = &(it->second);
    assert(pRecipeSkillBaseInfo);

    // Avoid system recipes. 
    KG_PROCESS_SUCCESS(pRecipeSkillBaseInfo->bIsSystemRecipe);

    if (
        !(pRecipeSkillBaseInfo->dwSkillID == dwSkillID && pRecipeSkillBaseInfo->dwSkillLevel == 0) &&
        !(pRecipeSkillBaseInfo->dwSkillID == dwSkillID && pRecipeSkillBaseInfo->dwSkillLevel == dwSkillLevel) &&
        !(pRecipeSkillBaseInfo->nSkillRecipeType && !pRecipeSkillBaseInfo->bIsSystemRecipe && pRecipeSkillBaseInfo->nSkillRecipeType == nRecipeType)
    )
        goto Exit1;

    if (pArray)
    {
        KGLOG_PROCESS_ERROR(uCount < uMaxArrayLength);

        pArray[uCount].dwRecipeKey = crRecipeListNode.dwRecipeKey;
        pArray[uCount].bActive = crRecipeListNode.bActive;
    }

    if (crRecipeListNode.bActive)
    {
        uActiveCount++;
    }

    uCount++;

Exit1:
    bResult = true;
Exit0:
    if (!bResult)
    {
        uCount = 0;
    }
    return bResult;
}

BOOL KSkillRecipeList::GetRecipeList(
    DWORD dwSkillID, DWORD dwSkillLevel, 
    size_t uMaxListLength, size_t* puArrayCount, size_t* puActiveCount, KG_SKILL_RECIPE_LIST_NODE RecipeList[]
)
{
    BOOL                        bResult     = false;
    KG_GET_USER_SKILL_RECIPE    GetUserSkillRecipeAll;
    KSkill*                     pSkill      = NULL;

    pSkill = g_pSO3World->m_SkillManager.GetSkill_RAW(dwSkillID, dwSkillLevel);
    KGLOG_PROCESS_ERROR(pSkill);

    GetUserSkillRecipeAll.pRecipeSkillBaseInfoMap = &(g_pSO3World->m_SkillManager.m_SkillRecipeBaseInfoMap);

    GetUserSkillRecipeAll.dwSkillID     = dwSkillID;
    GetUserSkillRecipeAll.dwSkillLevel  = dwSkillLevel;
    GetUserSkillRecipeAll.nRecipeType   = pSkill->m_pBaseInfo->nRecipeType;

    GetUserSkillRecipeAll.uMaxArrayLength = uMaxListLength;
    GetUserSkillRecipeAll.pArray = RecipeList;

    Traverse(GetUserSkillRecipeAll);

    assert(GetUserSkillRecipeAll.uCount >= GetUserSkillRecipeAll.uActiveCount);
    assert(GetUserSkillRecipeAll.uActiveCount <= MAX_SKILL_REICPE_COUNT);

    *puArrayCount   = GetUserSkillRecipeAll.uCount;
    *puActiveCount  = GetUserSkillRecipeAll.uActiveCount;

    bResult = true;
Exit0:
    return bResult;
}

#ifdef _SERVER
BOOL KSkillRecipeList::Load(size_t* puUsedSize, BYTE* pbyData, size_t uDataLen)
{
    BOOL                                bResult             = false;
    BOOL                                bRetCode            = false;
    size_t                              uSize               = 0;
    KG_SKILL_RECIPE_LIST_DB_DATA_BASE*  pSkillRecipeData    = NULL;

    KGLOG_PROCESS_ERROR(uDataLen >= sizeof(KG_SKILL_RECIPE_LIST_DB_DATA_BASE));
    pSkillRecipeData = (KG_SKILL_RECIPE_LIST_DB_DATA_BASE*)pbyData;

    KGLOG_PROCESS_ERROR(pSkillRecipeData->wVersion >= KG_SKILL_RECIPE_DB_LOAD_LOWEST_VER);
    KGLOG_PROCESS_ERROR(pSkillRecipeData->wVersion <= KG_SKILL_RECIPE_DB_CURRENT_VER);

    switch (pSkillRecipeData->wVersion)
    {
    case 1:
        bRetCode = _LoadDataV1(puUsedSize, pbyData, uDataLen);
        KGLOG_PROCESS_ERROR(bRetCode);
        break;
    case 2:
        bRetCode = _LoadDataV2(puUsedSize, pbyData, uDataLen);
        KGLOG_PROCESS_ERROR(bRetCode);
        break;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KSkillRecipeList::_LoadDataV1(size_t* puUsedSize, BYTE* pbyData, size_t uDataLen)
{
    BOOL                                bResult             = false;
    BOOL                                bRetCode            = false;
    size_t                              uSize               = 0;
    KG_SKILL_RECIPE_LIST_DB_DATA_V1*    pSkillRecipeData    = NULL;

    KGLOG_PROCESS_ERROR(uDataLen >= sizeof(KG_SKILL_RECIPE_LIST_DB_DATA_V1));
    pSkillRecipeData = (KG_SKILL_RECIPE_LIST_DB_DATA_V1*)pbyData;

    assert(pSkillRecipeData->wVersion == 1);

    uSize = sizeof(KG_SKILL_RECIPE_LIST_DB_DATA_V1) + sizeof(DWORD) * pSkillRecipeData->wCount;
    KGLOG_PROCESS_ERROR(uDataLen >= uSize);

    KG_PROCESS_SUCCESS(pSkillRecipeData->wCount == 0);

    for (int i = 0; i < pSkillRecipeData->wCount; i++)
    {
        bRetCode = _Add(pSkillRecipeData->dwRecipeKeyArray[i], false);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    g_PlayerServer.DoSyncSkillRecipe(
        m_pPlayer->m_nConnIndex,
        pSkillRecipeData->wCount,
        &m_SkillRecipeVector[0]
    );

Exit1:
    *puUsedSize = uSize;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KSkillRecipeList::_LoadDataV2(size_t* puUsedSize, BYTE* pbyData, size_t uDataLen)
{
    BOOL                                bResult             = false;
    BOOL                                bRetCode            = false;
    size_t                              uSize               = 0;
    KG_SKILL_RECIPE_LIST_DB_DATA_V2*    pSkillRecipeData    = NULL;

    KGLOG_PROCESS_ERROR(uDataLen >= sizeof(KG_SKILL_RECIPE_LIST_DB_DATA_V2));
    pSkillRecipeData = (KG_SKILL_RECIPE_LIST_DB_DATA_V2*)pbyData;

    assert(pSkillRecipeData->wVersion == 2);

    uSize = sizeof(KG_SKILL_RECIPE_LIST_DB_DATA_V2) + sizeof(KG_SKILL_RECIPE_LIST_DB_DATA_V2::_RecipeNode) * pSkillRecipeData->wCount;
    KGLOG_PROCESS_ERROR(uDataLen >= uSize);

    KG_PROCESS_SUCCESS(pSkillRecipeData->wCount == 0);

    for (int i = 0; i < pSkillRecipeData->wCount; i++)
    {
        bRetCode = _Add(pSkillRecipeData->RecipeArray[i].dwRecipeKey, pSkillRecipeData->RecipeArray[i].byActive);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    g_PlayerServer.DoSyncSkillRecipe(
        m_pPlayer->m_nConnIndex,
        pSkillRecipeData->wCount,
        &m_SkillRecipeVector[0]
    );

Exit1:
    *puUsedSize = uSize;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KSkillRecipeList::Save(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize)
{
    BOOL                                bResult             = false;
    int                                 nCount              = 0;
    size_t                              uSize               = 0;
    KG_SKILL_RECIPE_LIST_DB_DATA_V2*    pSkillRecipeData    = NULL;

    nCount = (int)m_SkillRecipeVector.size();

    uSize = sizeof(KG_SKILL_RECIPE_LIST_DB_DATA_V2) + sizeof(KG_SKILL_RECIPE_LIST_DB_DATA_V2::_RecipeNode) * nCount;

    KGLOG_PROCESS_ERROR(uBufferSize >= uSize);
    pSkillRecipeData = (KG_SKILL_RECIPE_LIST_DB_DATA_V2*)pbyBuffer;

    pSkillRecipeData->wVersion = KG_SKILL_RECIPE_DB_CURRENT_VER;

    assert(nCount >= 0 && nCount <= USHRT_MAX);
    pSkillRecipeData->wCount = (WORD)nCount;

    for (int i = 0; i < nCount; i++)
    {
        pSkillRecipeData->RecipeArray[i].dwRecipeKey    = m_SkillRecipeVector[i].dwRecipeKey;
        pSkillRecipeData->RecipeArray[i].byActive       = (BYTE)m_SkillRecipeVector[i].bActive;
    }

    *puUsedSize = uSize;

    bResult = true;
Exit0:
    return bResult;
}
#endif

KG_SKILL_RECIPE_LIST_NODE* KSkillRecipeList::_Find(DWORD dwRecipeKey)
{
    KG_SKILL_RECIPE_LIST_NODE*      pResult     = NULL;
    BOOL                            bRetCode    = false;
    KG_SKILL_RECIPE_LIST_NODE       Node;
    KG_SKILL_RECIPE_LIST::iterator  it;

    Node.dwRecipeKey = dwRecipeKey;
    it = std::lower_bound(m_SkillRecipeVector.begin(), m_SkillRecipeVector.end(), Node, _RecipeListNodeCompare);

    bRetCode = (it != m_SkillRecipeVector.end());
    KG_PROCESS_ERROR(bRetCode);

    KG_PROCESS_ERROR(it->dwRecipeKey == dwRecipeKey);

    pResult = &(*it);
Exit0:
    return pResult;
}

BOOL KSkillRecipeList::_Add(DWORD dwRecipeKey, BOOL bActive)
{
    BOOL    bResult         = false;
    DWORD   dwRecipeID      = (dwRecipeKey & 0xFFFF0000);
    DWORD   dwRecipeLevel   = (dwRecipeKey & 0x0000FFFF);
    KG_SKILL_RECIPE_LIST_NODE       Node;
    KG_SKILL_RECIPE_LIST::iterator  it;

    assert(sizeof(DWORD) == 4);

    Node.dwRecipeKey    = dwRecipeKey;
    Node.bActive        = bActive;

    it = std::lower_bound(m_SkillRecipeVector.begin(), m_SkillRecipeVector.end(), Node, _RecipeListNodeCompare);

    if (it == m_SkillRecipeVector.end())
    {
        KGLOG_PROCESS_ERROR(m_SkillRecipeVector.size() < MAX_SKILL_REICPE_LIST_SIZE);
        m_SkillRecipeVector.insert(it, Node);
    }
    else if ((it->dwRecipeKey & 0xFFFF0000) != dwRecipeID)
    {
        KGLOG_PROCESS_ERROR(m_SkillRecipeVector.size() < MAX_SKILL_REICPE_LIST_SIZE);
        m_SkillRecipeVector.insert(it, Node);
    }
    else if ((it->dwRecipeKey & 0x0000FFFF) < dwRecipeLevel)    // Level up
    {
        it->dwRecipeKey = dwRecipeKey;
        it->bActive     = bActive;
    }

    bResult = true;
Exit0:
    if (!bResult)
        KGLogPrintf(KGLOG_ERR, "[SKILL] Add skill recipe (%lu, %lu) failed! \n", dwRecipeID, dwRecipeLevel);

    return bResult;
}
