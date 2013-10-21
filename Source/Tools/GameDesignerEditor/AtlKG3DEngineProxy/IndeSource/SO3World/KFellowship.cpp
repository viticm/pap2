#include "stdafx.h"
#include "KGPublic.h"
#include "KSO3World.h"
#include "KFellowship.h"
#include "KRelayClient.h"
#include "KPlayerServer.h"
#include "KPlayer.h"

// CLASS: KGFellowshipMgr //////////////////////////////////////////////////
BOOL KGFellowshipMgr::Init()
{
    assert(m_FellowshipMap.empty());
    assert(m_ReverseFellowshipSet.empty());

    m_GroupNamesMap.clear();

#ifdef _CLIENT
    m_bWaitInitSync = true;
#endif // _CLIENT

    return true;
}

void KGFellowshipMgr::UnInit()
{
    m_FellowshipMap.clear();
    m_ReverseFellowshipSet.clear();

    m_GroupNamesMap.clear();
}

void KGFellowshipMgr::Reset()
{
    m_FellowshipMap.clear();
    m_ReverseFellowshipSet.clear();

    m_GroupNamesMap.clear();
}

struct _GetIDArrayFunc 
{
    _GetIDArrayFunc() : m_uArraySize(0) {};

    BOOL operator ()(DWORD dwPlayerID, DWORD dwAlliedPlayerID)
    {
        KGLOG_PROCESS_ERROR(m_uArraySize < KG_FELLOWSHIP_MAX_RECORD);

        m_dwAlliedPlayerIDArray[m_uArraySize++] = dwAlliedPlayerID;

        return true;
Exit0:
        return false;
    };

    DWORD   m_dwAlliedPlayerIDArray[KG_FELLOWSHIP_MAX_RECORD];
    size_t  m_uArraySize;
};

#ifdef _SERVER
BOOL KGFellowshipMgr::LoadFellowshipData(DWORD dwPlayerID)
{
    g_RelayClient.DoApplyFellowshipDataRequest(dwPlayerID);
    return true;
}

BOOL KGFellowshipMgr::OnLoadFellowshipData(DWORD dwPlayerID, size_t uDataSize, BYTE byData[])
{
    BOOL                bResult         = false;
    BOOL                bRetCode        = false;
    KG_FELLOWSHIP_DB*   pFellowshipDB   = NULL;

    UnloadPlayerFellowship(dwPlayerID);
    m_GroupNamesMap[dwPlayerID].nGroupCount = 0;

    KG_PROCESS_SUCCESS(!uDataSize); // This player do not have any fellowship data saved before. 

    KGLOG_PROCESS_ERROR(uDataSize > sizeof(KG_FELLOWSHIP_DB));
    pFellowshipDB = (KG_FELLOWSHIP_DB*)byData;

    switch(pFellowshipDB->nVersion)
    {
    case 1:
        bRetCode = OnLoadFellowshipDataV1(dwPlayerID, uDataSize, byData);
        KGLOG_PROCESS_ERROR(bRetCode);
        break;

    default:
        KGLogPrintf(KGLOG_ERR, "Unexcepted fellowship data type: %d.", pFellowshipDB->nVersion);
    }

Exit1:
    m_OnlineIDSet.insert(dwPlayerID);
    bResult = true;
Exit0:
    if (!bResult)
    {
        UnloadPlayerFellowship(dwPlayerID);
    }
    return bResult;
}

BOOL KGFellowshipMgr::OnLoadFellowshipDataV1(DWORD dwPlayerID, size_t uDataSize, BYTE byData[])
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    KG_FELLOWSHIP_DB_V1*    pFellowshipDBv1 = NULL;
    size_t                  uExceptedSize   = 0;
    int                     nArrayLength    = 0;
    DWORD                   dwAlliedPlayerIDArray[KG_FELLOWSHIP_MAX_RECORD + KG_FELLOWSHIP_MAX_FOE_RECORD + KG_FELLOWSHIP_MAX_BLACKLIST_RECORD];
    KG_FELLOWSHIP_GROUPNAMES_MAP::iterator it;
    BYTE*                   pbyData         = NULL;

    KGLOG_PROCESS_ERROR(uDataSize >= sizeof(KG_FELLOWSHIP_DB_V1));

    pFellowshipDBv1 = (KG_FELLOWSHIP_DB_V1*)byData;

    uExceptedSize  = sizeof(KG_FELLOWSHIP_DB_V1);
    uExceptedSize += pFellowshipDBv1->nFriendCount * sizeof(KG_FELLOWSHIP_ENTRY_V1);
    uExceptedSize += pFellowshipDBv1->nFoeCount * sizeof(KG_FELLOWSHIP_FOE_ENTRY_V1);
    uExceptedSize += pFellowshipDBv1->nBlackListCount * sizeof(KG_FELLOWSHIP_BLACKLIST_ENTRY_V1);

    KGLOG_PROCESS_ERROR(uDataSize == uExceptedSize);
    KGLOG_PROCESS_ERROR(pFellowshipDBv1->nFriendCount <= KG_FELLOWSHIP_MAX_RECORD);
    KGLOG_PROCESS_ERROR(pFellowshipDBv1->nFoeCount <= KG_FELLOWSHIP_MAX_FOE_RECORD);
    KGLOG_PROCESS_ERROR(pFellowshipDBv1->nBlackListCount <= KG_FELLOWSHIP_MAX_BLACKLIST_RECORD);

    it = m_GroupNamesMap.find(dwPlayerID);
    assert(it != m_GroupNamesMap.end());
    for (int i = 0; i < KG_FELLOWSHIP_MAX_CUSTEM_GROUP; i++)
    {
        if (pFellowshipDBv1->szGroupName[i][0] == '\0')
            break;

        strncpy(it->second.szGroupName[i], pFellowshipDBv1->szGroupName[i], sizeof(it->second.szGroupName[i]));
        it->second.szGroupName[i][sizeof(it->second.szGroupName[i]) - sizeof('\0')] = '\0';

        it->second.nGroupCount++;
    }

    for (int i = it->second.nGroupCount; i < KG_FELLOWSHIP_MAX_CUSTEM_GROUP; i++)
        it->second.szGroupName[i][0] = '\0';

    pbyData = pFellowshipDBv1->byData;

    // Friend
    for (int i = 0; i < pFellowshipDBv1->nFriendCount; i++)
    {
        KGFellowship*           pFellowship = NULL;
        KPlayer*                pPlayer     = NULL;
        KG_FELLOWSHIP_ENTRY_V1* pFellowshipEntry = (KG_FELLOWSHIP_ENTRY_V1*)pbyData;

        pFellowship = AddFellowship(dwPlayerID, pFellowshipEntry->dwAlliedPlayerID, "", false);
        KGLOG_PROCESS_ERROR(pFellowship);

        pFellowship->m_nAttraction  = pFellowshipEntry->nAttraction;
        pFellowship->m_BitFlag      = pFellowshipEntry->Flags;

        // Level
        pPlayer = g_pSO3World->m_PlayerSet.GetObj(pFellowshipEntry->dwAlliedPlayerID);
        if (pPlayer)
            pFellowship->m_nPlayerLevel = pPlayer->m_nLevel;
        else
            g_RelayClient.DoApplyFellowshipPlayerLevelAndForceID(pFellowshipEntry->dwAlliedPlayerID);

        strncpy(pFellowship->m_szName, "----", sizeof(pFellowship->m_szName));
        pFellowship->m_szName[sizeof(pFellowship->m_szName) - sizeof('\0')] = '\0';

        strncpy(pFellowship->m_szRemark, pFellowshipEntry->szRemark, sizeof(pFellowship->m_szRemark));
        pFellowship->m_szRemark[sizeof(pFellowship->m_szRemark) - sizeof('\0')] = '\0';

        dwAlliedPlayerIDArray[nArrayLength++] = pFellowshipEntry->dwAlliedPlayerID;

        pbyData += sizeof(KG_FELLOWSHIP_ENTRY_V1);
    }

    // Foe
    for (int i = 0; i < pFellowshipDBv1->nFoeCount; i++)
    {
        KGFoe*                      pFoe = NULL;
        KPlayer*                    pPlayer = NULL;
        KG_FELLOWSHIP_FOE_ENTRY_V1* pFoeEntry = (KG_FELLOWSHIP_FOE_ENTRY_V1*)pbyData;

        pFoe = AddFoe(dwPlayerID, pFoeEntry->dwAlliedPlayerID, "", false);
        KGLOG_PROCESS_ERROR(pFoe);

        // Level
        pPlayer = g_pSO3World->m_PlayerSet.GetObj(pFoeEntry->dwAlliedPlayerID);
        if (pPlayer)
            pFoe->m_nPlayerLevel = pPlayer->m_nLevel;
        else
            g_RelayClient.DoApplyFellowshipPlayerLevelAndForceID(pFoeEntry->dwAlliedPlayerID);

        strncpy(pFoe->m_szName, "----", sizeof(pFoe->m_szName));
        pFoe->m_szName[sizeof(pFoe->m_szName) - sizeof('\0')] = '\0';

        dwAlliedPlayerIDArray[nArrayLength++] = pFoeEntry->dwAlliedPlayerID;

        pbyData += sizeof(KG_FELLOWSHIP_FOE_ENTRY_V1);
    }

    // BlackList
    for (int i = 0; i < pFellowshipDBv1->nBlackListCount; i++)
    {
        KGBlackNode*                        pBlackNode      = NULL;
        KG_FELLOWSHIP_BLACKLIST_ENTRY_V1*   pBlackListEntry = (KG_FELLOWSHIP_BLACKLIST_ENTRY_V1*)pbyData;

        pBlackNode = AddBlackList(dwPlayerID, pBlackListEntry->dwAlliedPlayerID, "", false);
        KGLOG_PROCESS_ERROR(pBlackNode);

        strncpy(pBlackNode->m_szName, "----", sizeof(pBlackNode->m_szName));
        pBlackNode->m_szName[sizeof(pBlackNode->m_szName) - sizeof('\0')] = '\0';

        dwAlliedPlayerIDArray[nArrayLength++] = pBlackListEntry->dwAlliedPlayerID;

        pbyData += sizeof(KG_FELLOWSHIP_BLACKLIST_ENTRY_V1);
    }

    if (nArrayLength)
        g_RelayClient.DoGetFellowshipName(dwPlayerID, nArrayLength, dwAlliedPlayerIDArray);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGFellowshipMgr::SaveFellowshipData(DWORD dwPlayerID)
{
    BOOL                                    bResult         = false;
    BOOL                                    bRetCode        = false;
    KG_FELLOWSHIP_DB_V1*                    pFellowshipDBv1 = NULL;
    IKG_Buffer*                             piBuffer        = NULL;
    int                                     nFriendCount    = CountFellowship(dwPlayerID);
    int                                     nFoeCount       = CountFoe(dwPlayerID);
    int                                     nBlackListCount = CountBlackList(dwPlayerID);
    size_t                                  uSize           = 0;
    int                                     nGroupCount     = 0;
    BYTE*                                   pbyData         = NULL;
    _GetIDArrayFunc                         GetIDArrayFunc;
    KG_FELLOWSHIP_ONLINE_ID_SET::iterator   it;
    KG_FELLOWSHIP_GROUPNAMES_MAP::iterator  itGroup;

    it = m_OnlineIDSet.find(dwPlayerID);
    KG_PROCESS_SUCCESS(it == m_OnlineIDSet.end());

    uSize       = sizeof(KG_FELLOWSHIP_DB_V1);
    uSize      += nFriendCount * sizeof(KG_FELLOWSHIP_ENTRY_V1);
    uSize      += nFoeCount * sizeof(KG_FELLOWSHIP_FOE_ENTRY_V1);
    uSize      += nBlackListCount * sizeof(KG_FELLOWSHIP_BLACKLIST_ENTRY_V1);
    piBuffer    = KG_MemoryCreateBuffer((unsigned)uSize);
    KGLOG_PROCESS_ERROR(piBuffer);

    pFellowshipDBv1 = (KG_FELLOWSHIP_DB_V1*)piBuffer->GetData();
    assert(pFellowshipDBv1);

    pFellowshipDBv1->nVersion = KG_FELLOWSHIP_DB_CURRENT_DATA_VERSION;

    // Group name
    itGroup = m_GroupNamesMap.find(dwPlayerID);
    if (itGroup != m_GroupNamesMap.end())
        nGroupCount = itGroup->second.nGroupCount;

    for (int i = 0; i < nGroupCount; i++)
    {
        strncpy(pFellowshipDBv1->szGroupName[i], itGroup->second.szGroupName[i], sizeof(pFellowshipDBv1->szGroupName[i]));
        pFellowshipDBv1->szGroupName[i][sizeof(pFellowshipDBv1->szGroupName[i]) - sizeof('\0')] = '\0';
    }

    for (int i = nGroupCount; i < KG_FELLOWSHIP_MAX_CUSTEM_GROUP; i++)
        pFellowshipDBv1->szGroupName[i][0] = '\0';

    pbyData = pFellowshipDBv1->byData;

    // Fellowship
    GetIDArrayFunc.m_uArraySize = 0;
    TraverseFellowshipID(dwPlayerID, GetIDArrayFunc);
    KGLOG_PROCESS_ERROR((int)GetIDArrayFunc.m_uArraySize == nFriendCount);
    pFellowshipDBv1->nFriendCount = nFriendCount;

    for (int i = 0; i < (int)GetIDArrayFunc.m_uArraySize; i++)
    {
        DWORD                       dwAlliedPlayerID    = GetIDArrayFunc.m_dwAlliedPlayerIDArray[i];
        KGFellowship*               pFellowship         = NULL;
        KG_FELLOWSHIP_ENTRY_V1*     pFellowshipEntry    = (KG_FELLOWSHIP_ENTRY_V1*)pbyData;

        pFellowship = GetFellowship(dwPlayerID, dwAlliedPlayerID);
        KGLOG_PROCESS_ERROR(pFellowship);

        pFellowshipEntry->dwAlliedPlayerID  = dwAlliedPlayerID;
        pFellowshipEntry->nAttraction       = pFellowship->m_nAttraction;

        memcpy(&pFellowshipEntry->Flags, &pFellowship->m_BitFlag, sizeof(pFellowshipEntry->Flags));
        strncpy(pFellowshipEntry->szRemark, pFellowship->m_szRemark, sizeof(pFellowshipEntry->szRemark));
        pFellowshipEntry->szRemark[sizeof(pFellowshipEntry->szRemark) - sizeof('\0')] = '\0';

        pbyData += sizeof(KG_FELLOWSHIP_ENTRY_V1);
    }

    // Foe
    GetIDArrayFunc.m_uArraySize = 0;
    TraverseFoeID(dwPlayerID, GetIDArrayFunc);
    KGLOG_PROCESS_ERROR((int)GetIDArrayFunc.m_uArraySize == nFoeCount);
    pFellowshipDBv1->nFoeCount = nFoeCount;

    for (int i = 0; i < (int)GetIDArrayFunc.m_uArraySize; i++)
    {
        DWORD                       dwAlliedPlayerID    = GetIDArrayFunc.m_dwAlliedPlayerIDArray[i];
        KGFoe*                      pFoe                = NULL;
        KG_FELLOWSHIP_FOE_ENTRY_V1* pFoeEntry           = (KG_FELLOWSHIP_FOE_ENTRY_V1*)pbyData;

        pFoe = GetFoe(dwPlayerID, dwAlliedPlayerID);
        KGLOG_PROCESS_ERROR(pFoe);

        pFoeEntry->dwAlliedPlayerID  = dwAlliedPlayerID;

        pbyData += sizeof(KG_FELLOWSHIP_FOE_ENTRY_V1);
    }

    // BlackList
    GetIDArrayFunc.m_uArraySize = 0;
    TraverseBlackListID(dwPlayerID, GetIDArrayFunc);
    KGLOG_PROCESS_ERROR((int)GetIDArrayFunc.m_uArraySize == nBlackListCount);
    pFellowshipDBv1->nBlackListCount = nBlackListCount;

    for (int i = 0; i < (int)GetIDArrayFunc.m_uArraySize; i++)
    {
        DWORD                               dwAlliedPlayerID    = GetIDArrayFunc.m_dwAlliedPlayerIDArray[i];
        KGBlackNode*                        pBlackNode          = NULL;
        KG_FELLOWSHIP_BLACKLIST_ENTRY_V1*   pBlackListEntry     = (KG_FELLOWSHIP_BLACKLIST_ENTRY_V1*)pbyData;

        pBlackNode = GetBlackListNode(dwPlayerID, dwAlliedPlayerID);
        KGLOG_PROCESS_ERROR(pBlackNode);

        pBlackListEntry->dwAlliedPlayerID  = dwAlliedPlayerID;

        pbyData += sizeof(KG_FELLOWSHIP_BLACKLIST_ENTRY_V1);
    }

    g_RelayClient.DoUpdateFellowshipData(dwPlayerID, uSize, (BYTE*)pFellowshipDBv1);

Exit1:
    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

struct _FallOffFunc 
{
    _FallOffFunc() : m_nDays(0), m_pFellowshipMgr(NULL), m_bFallOffFlag(false) {};

    BOOL operator ()(DWORD dwPlayerID, DWORD dwAlliedPlayerID)
    {
        int                 nAttractionFallOff  = 0;
        int                 nAttractionLevel    = 0;
        KGFellowship*       pFellowship         = m_pFellowshipMgr->GetFellowship(dwPlayerID, dwAlliedPlayerID);

        assert(m_nDays >= 0);
        assert(pFellowship);

        for (int i = 5; i < m_nDays; i++)   // 大于5天连续不上线才开始衰减
        {
            nAttractionLevel = KGFellowshipMgr::AttractionValue2Level(pFellowship->m_nAttraction);
            if (nAttractionLevel <= 3)
                break;

            nAttractionFallOff = max((7 - nAttractionLevel), 0) * (6 * (nAttractionLevel - 4) * (nAttractionLevel - 4) + 2) + 5;

            pFellowship->m_nAttraction -= nAttractionFallOff;
            m_bFallOffFlag              = true;
        }

        return true;
    };

    KGFellowshipMgr*    m_pFellowshipMgr;
    int                 m_nDays;
    BOOL                m_bFallOffFlag;
};

BOOL KGFellowshipMgr::AttractionFalloff(DWORD dwPlayerID, int nDays)
{
    BOOL            bResult     = false;
    BOOL            bRetCode    = false;
    _FallOffFunc    FallOffFunc;

    KGLOG_PROCESS_ERROR(dwPlayerID != ERROR_ID);
    KG_PROCESS_ERROR(nDays > 5);

    FallOffFunc.m_nDays = nDays;
    FallOffFunc.m_pFellowshipMgr = this;

    bRetCode = TraverseFellowshipID(dwPlayerID, FallOffFunc);
    KG_PROCESS_ERROR(bRetCode);

    if (FallOffFunc.m_bFallOffFlag)
    {
        KPlayer* pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
        if (pPlayer)
            g_PlayerServer.DoMessageNotify(pPlayer->m_nConnIndex, ectFellowshipAttractionFallOff, 0);
    }

    bResult = true;
Exit0:
    return bResult;
}

struct _RefreshDailyCountFunc 
{
    _RefreshDailyCountFunc() : m_pConstList(NULL), m_pFellowshipMgr(NULL) {};

    BOOL operator ()(DWORD dwPlayerID, DWORD dwAlliedPlayerID)
    {
        int             nAttractionFallOff  = 0;
        int             nAttractionLevel    = 0;
        KGFellowship*   pFellowship         = m_pFellowshipMgr->GetFellowship(dwPlayerID, dwAlliedPlayerID);
        assert(pFellowship);

        assert(m_pConstList->nWhisperAttractiveDailyCount < (1 << 4));
        assert(m_pConstList->nDuelAttractiveDailyCount < (1 << 4));

        pFellowship->m_BitFlag.nWhisperDailyCount   = m_pConstList->nWhisperAttractiveDailyCount;
        pFellowship->m_BitFlag.nDuelDailyCount      = m_pConstList->nDuelAttractiveDailyCount;

        return true;
    };

    KGWConstList *      m_pConstList;
    KGFellowshipMgr*    m_pFellowshipMgr;
};

BOOL KGFellowshipMgr::RefreshDailyCount(DWORD dwPlayerID)
{
    BOOL                    bResult     = false;
    BOOL                    bRetCode    = false;
    _RefreshDailyCountFunc  RefreshDailyCountFunc;

    KGLOG_PROCESS_ERROR(dwPlayerID != ERROR_ID);

    RefreshDailyCountFunc.m_pConstList = &(g_pSO3World->m_Settings.m_ConstList);
    RefreshDailyCountFunc.m_pFellowshipMgr = this;

    bRetCode = TraverseFellowshipID(dwPlayerID, RefreshDailyCountFunc);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

#endif // _SERVER

BOOL KGFellowshipMgr::UnloadPlayerFellowship(DWORD dwPlayerID)
{
    BOOL bResult = false;

    assert(dwPlayerID);

    _GetIDArrayFunc UnLoadFunc;

    // 好友
    UnLoadFunc.m_uArraySize = 0;
    TraverseFellowshipID(dwPlayerID, UnLoadFunc);

    for (int i = 0; i < (int)UnLoadFunc.m_uArraySize; i++)
        DelFellowship(dwPlayerID, UnLoadFunc.m_dwAlliedPlayerIDArray[i]);

    // 仇人
    UnLoadFunc.m_uArraySize = 0;
    TraverseFoeID(dwPlayerID, UnLoadFunc);

    for (int i = 0; i < (int)UnLoadFunc.m_uArraySize; i++)
        DelFoe(dwPlayerID, UnLoadFunc.m_dwAlliedPlayerIDArray[i]);

    // 黑名单
    UnLoadFunc.m_uArraySize = 0;
    TraverseBlackListID(dwPlayerID, UnLoadFunc);

    for (int i = 0; i < (int)UnLoadFunc.m_uArraySize; i++)
        DelBlackList(dwPlayerID, UnLoadFunc.m_dwAlliedPlayerIDArray[i]);

    m_GroupNamesMap.erase(dwPlayerID);

    m_OnlineIDSet.erase(dwPlayerID);

    bResult = true;
Exit0:
    return bResult;
}

KGFellowship* KGFellowshipMgr::GetFellowship(DWORD dwPlayerID, DWORD dwAlliedPlayerID)
{
    KGFellowship* pResult = NULL;
    KG_FELLOWSHIP_MAP::iterator it;

    it = m_FellowshipMap.find(KG_ID_PAIR(dwPlayerID, dwAlliedPlayerID));
    KG_PROCESS_ERROR(it != m_FellowshipMap.end());

    pResult = &(it->second);
Exit0:
    return pResult;
}

KGFellowship* KGFellowshipMgr::AddFellowship(DWORD dwPlayerID, DWORD dwAlliedPlayerID, const char cszAlliedPlayerName[], BOOL bNotifyClient)
{
    int                                     nResult     = rrcFailed;
    int                                     nRetCode    = false;
    KGFellowship*                           pResult     = NULL;
    KGFellowship*                           pFellowship = NULL;
    KPlayer*                                pPlayer     = NULL;
    std::pair<KG_FELLOWSHIP_MAP_IT, bool>   RetPairRM;
    std::pair<KG_ID_PAIR_SET_IT,  bool>     RetPairRRS;

    assert(dwPlayerID);
    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
    KG_PROCESS_ERROR(pPlayer);

    KG_PROCESS_ERROR_RET_CODE(dwAlliedPlayerID != ERROR_ID, rrcInvalidName);
    KG_PROCESS_ERROR_RET_CODE(dwAlliedPlayerID != dwPlayerID, rrcAddSelf);

    nRetCode = CountFellowship(dwPlayerID);
    KG_PROCESS_ERROR_RET_CODE(nRetCode < KG_FELLOWSHIP_MAX_RECORD, rrcFellowshipListFull);

    // Add relation link, dwPlayerID ---> dwAlliedPlayerID
    RetPairRM = m_FellowshipMap.insert(std::pair<const KG_ID_PAIR, KGFellowship>(KG_ID_PAIR(dwPlayerID, dwAlliedPlayerID), KGFellowship()));
    KG_PROCESS_ERROR_RET_CODE(RetPairRM.second, rrcFellowshipExists);

    // Add reverse relation link, dwPlayerID <--- dwAlliedPlayerID
    RetPairRRS = m_ReverseFellowshipSet.insert(KG_ID_PAIR(dwAlliedPlayerID, dwPlayerID));
    KGLOG_CHECK_ERROR(RetPairRRS.second);

    pFellowship = &((RetPairRM.first)->second);

#ifdef _SERVER
    if (cszAlliedPlayerName)
    {
        strncpy(pFellowship->m_szName, cszAlliedPlayerName, sizeof(pFellowship->m_szName));
        pFellowship->m_szName[sizeof(pFellowship->m_szName) - 1] = '\0';
    }

    g_RelayClient.DoGetFellowshipName(dwPlayerID, 1, &dwAlliedPlayerID);
    g_RelayClient.DoApplyFellowshipPlayerLevelAndForceID(dwAlliedPlayerID);

    ((RetPairRM.first)->second).m_BitFlag.nWhisperDailyCount = g_pSO3World->m_Settings.m_ConstList.nWhisperAttractiveDailyCount;
    ((RetPairRM.first)->second).m_BitFlag.nDuelDailyCount = g_pSO3World->m_Settings.m_ConstList.nDuelAttractiveDailyCount;
#endif

    nResult = rrcSuccessAdd;
    pResult = pFellowship;
Exit0:
#ifdef _SERVER
    if (pPlayer && bNotifyClient)
    {
        if (nResult == rrcSuccessAdd && cszAlliedPlayerName)
        {
            g_PlayerServer.DoMessageNotify(
                pPlayer->m_nConnIndex, ectFellowshipErrorCode, nResult,
                cszAlliedPlayerName, (strlen(cszAlliedPlayerName) + 1) * sizeof(cszAlliedPlayerName[0])
            );

            g_RelayClient.DoAddFellowshipNotify(dwAlliedPlayerID, pPlayer->m_dwID, pPlayer->m_szName, 1);
        }
        else
        {
            g_PlayerServer.DoMessageNotify(pPlayer->m_nConnIndex, ectFellowshipErrorCode, nResult);
        }
    }
#endif
    return pResult;
}

BOOL KGFellowshipMgr::DelFellowship(DWORD dwPlayerID, DWORD dwAlliedPlayerID)
{
    BOOL bResult    = false;
    int  nRetCode   = 0;

    // Remove relation link, dwPlayerID -X-> dwAlliedPlayerID
    nRetCode = (int)m_FellowshipMap.erase(KG_ID_PAIR(dwPlayerID, dwAlliedPlayerID));
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    // Remove reverse relation link, dwPlayerID <-X- dwAlliedPlayerID
    nRetCode = (int)m_ReverseFellowshipSet.erase(KG_ID_PAIR(dwAlliedPlayerID, dwPlayerID));
    KGLOG_CHECK_ERROR(nRetCode == 1);

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

struct _FellowshipCount 
{
    _FellowshipCount()
    {
        nCount          = 0;
    }

    BOOL operator () (DWORD dwPlayerID, DWORD dwAlliedPlayerID)
    {
        nCount++; 
        return true; 
    }

    int                 nCount;
};

BOOL KGFellowshipMgr::CountFellowship(DWORD dwPlayerID)
{
    _FellowshipCount Count;

    assert(dwPlayerID);

    TraverseFellowshipID(dwPlayerID, Count);

    return Count.nCount;
}

BOOL KGFellowshipMgr::ClearFellowship(DWORD dwPlayerID)
{
    BOOL            bResult = false;
    _GetIDArrayFunc ClearFunc;

    assert(dwPlayerID);

    ClearFunc.m_uArraySize = 0;
    TraverseFellowshipID(dwPlayerID, ClearFunc);

    for (int i = 0; i < (int)ClearFunc.m_uArraySize; i++)
        DelFellowship(dwPlayerID, ClearFunc.m_dwAlliedPlayerIDArray[i]);

    m_GroupNamesMap.erase(dwPlayerID);

    bResult = true;
//Exit0:
    return bResult;
}

KGFoe* KGFellowshipMgr::GetFoe(DWORD dwPlayerID, DWORD dwAlliedPlayerID)
{
    KGFoe*          pResult = NULL;
    KG_FOE_MAP_IT   it;

    it = m_FoeMap.find(KG_ID_PAIR(dwPlayerID, dwAlliedPlayerID));
    KG_PROCESS_ERROR(it != m_FoeMap.end());

    pResult = &(it->second);
Exit0:
    return pResult;
}

KGFoe* KGFellowshipMgr::AddFoe(DWORD dwPlayerID, DWORD dwAlliedPlayerID, const char cszAlliedPlayerName[], BOOL bNotifyClient)
{
    int                                 nResult     = rrcFailed;
    int                                 nRetCode    = false;
    KGFoe*                              pResult     = NULL;
    KGFoe*                              pFoe        = NULL;
    KPlayer*                            pPlayer     = NULL;
    std::pair<KG_FOE_MAP_IT, bool>      RetPairRM;
    std::pair<KG_ID_PAIR_SET_IT,  bool> RetPairRRS;

    assert(dwPlayerID);
    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
    KG_PROCESS_ERROR(pPlayer);

    KG_PROCESS_ERROR_RET_CODE(dwAlliedPlayerID != ERROR_ID, rrcInvalidName);
    KG_PROCESS_ERROR_RET_CODE(dwAlliedPlayerID != dwPlayerID, rrcAddSelf);

    nRetCode = CountFoe(dwPlayerID);
    KG_PROCESS_ERROR_RET_CODE(nRetCode < KG_FELLOWSHIP_MAX_FOE_RECORD, rrcFoeListFull);

    // Add relation link, dwPlayerID ---> dwAlliedPlayerID
    RetPairRM = m_FoeMap.insert(std::pair<const KG_ID_PAIR, KGFoe>(KG_ID_PAIR(dwPlayerID, dwAlliedPlayerID), KGFoe()));
    KG_PROCESS_ERROR_RET_CODE(RetPairRM.second, rrcFoeExists);

    // Add reverse relation link, dwPlayerID <--- dwAlliedPlayerID
    RetPairRRS = m_ReverseFoeSet.insert(KG_ID_PAIR(dwAlliedPlayerID, dwPlayerID));
    KGLOG_CHECK_ERROR(RetPairRRS.second);
    
    pFoe = &((RetPairRM.first)->second);

#ifdef _SERVER
    if (cszAlliedPlayerName)
    {
        strncpy(pFoe->m_szName, cszAlliedPlayerName, sizeof(pFoe->m_szName));
        pFoe->m_szName[sizeof(pFoe->m_szName) - 1] = '\0';
    }

    g_RelayClient.DoGetFellowshipName(dwPlayerID, 1, &dwAlliedPlayerID);
    g_RelayClient.DoApplyFellowshipPlayerLevelAndForceID(dwAlliedPlayerID);
#endif

    nResult = rrcSuccessAddFoe;
    pResult = pFoe;
Exit0:
#ifdef _SERVER
    if (pPlayer && bNotifyClient)
    {
        if (nResult == rrcSuccessAddFoe && cszAlliedPlayerName)
        {
            g_PlayerServer.DoMessageNotify(
                pPlayer->m_nConnIndex, ectFellowshipErrorCode, nResult,
                cszAlliedPlayerName, (strlen(cszAlliedPlayerName) + 1) * sizeof(cszAlliedPlayerName[0])
            );

            g_RelayClient.DoAddFellowshipNotify(dwAlliedPlayerID, pPlayer->m_dwID, pPlayer->m_szName, 2);
        }
        else
        {
            g_PlayerServer.DoMessageNotify(pPlayer->m_nConnIndex, ectFellowshipErrorCode, nResult);
        }
    }
#endif
    return pResult;
}

BOOL KGFellowshipMgr::DelFoe(DWORD dwPlayerID, DWORD dwAlliedPlayerID)
{
    BOOL bResult    = false;
    int  nRetCode   = 0;

    // Remove relation link, dwPlayerID -X-> dwAlliedPlayerID
    nRetCode = (int)m_FoeMap.erase(KG_ID_PAIR(dwPlayerID, dwAlliedPlayerID));
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    // Remove reverse relation link, dwPlayerID <-X- dwAlliedPlayerID
    nRetCode = (int)m_ReverseFoeSet.erase(KG_ID_PAIR(dwAlliedPlayerID, dwPlayerID));
    KGLOG_CHECK_ERROR(nRetCode == 1);

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KGFellowshipMgr::CountFoe(DWORD dwPlayerID)
{
    _FellowshipCount Count;

    assert(dwPlayerID);

    TraverseFoeID(dwPlayerID, Count);

    return Count.nCount;
}

BOOL KGFellowshipMgr::ClearFoe(DWORD dwPlayerID)
{
    BOOL            bResult = false;
    _GetIDArrayFunc ClearFunc;

    assert(dwPlayerID);

    ClearFunc.m_uArraySize = 0;
    TraverseFoeID(dwPlayerID, ClearFunc);

    for (int i = 0; i < (int)ClearFunc.m_uArraySize; i++)
        DelFoe(dwPlayerID, ClearFunc.m_dwAlliedPlayerIDArray[i]);

    bResult = true;
//Exit0:
    return bResult;
}

KGBlackNode* KGFellowshipMgr::GetBlackListNode(DWORD dwPlayerID, DWORD dwAlliedPlayerID)
{
    KGBlackNode*            pResult = NULL;
    KG_BLACK_LIST_MAP_IT    it;

    it = m_BlackListMap.find(KG_ID_PAIR(dwPlayerID, dwAlliedPlayerID));
    KG_PROCESS_ERROR(it != m_BlackListMap.end());

    pResult = &(it->second);
Exit0:
    return pResult;
}

KGBlackNode* KGFellowshipMgr::AddBlackList(DWORD dwPlayerID, DWORD dwAlliedPlayerID, const char cszAlliedPlayerName[], BOOL bNotifyClient)
{
    int                                     nResult     = rrcFailed;
    int                                     nRetCode    = false;
    KGBlackNode*                            pResult     = NULL;
    KGBlackNode*                            pBlackNode  = NULL;
    KPlayer*                                pPlayer     = NULL;
    std::pair<KG_BLACK_LIST_MAP_IT, bool>   RetPairRM;
    std::pair<KG_ID_PAIR_SET_IT,  bool>     RetPairRRS;

    assert(dwPlayerID);
    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
    KG_PROCESS_ERROR(pPlayer);

    KG_PROCESS_ERROR_RET_CODE(dwAlliedPlayerID != ERROR_ID, rrcInvalidName);
    KG_PROCESS_ERROR_RET_CODE(dwAlliedPlayerID != dwPlayerID, rrcAddSelf);

    nRetCode = CountBlackList(dwPlayerID);
    KG_PROCESS_ERROR_RET_CODE(nRetCode < KG_FELLOWSHIP_MAX_BLACKLIST_RECORD, rrcBlackListFull);

    // Add relation link, dwPlayerID ---> dwAlliedPlayerID
    RetPairRM = m_BlackListMap.insert(std::pair<const KG_ID_PAIR, KGBlackNode>(KG_ID_PAIR(dwPlayerID, dwAlliedPlayerID), KGBlackNode()));
    KG_PROCESS_ERROR_RET_CODE(RetPairRM.second, rrcBlackListExists);

    // Add reverse relation link, dwPlayerID <--- dwAlliedPlayerID
    RetPairRRS = m_ReverseBlackListSet.insert(KG_ID_PAIR(dwAlliedPlayerID, dwPlayerID));
    KGLOG_CHECK_ERROR(RetPairRRS.second);

    pBlackNode = &((RetPairRM.first)->second);

#ifdef _SERVER
    if (cszAlliedPlayerName)
    {
        strncpy(pBlackNode->m_szName, cszAlliedPlayerName, sizeof(pBlackNode->m_szName));
        pBlackNode->m_szName[sizeof(pBlackNode->m_szName) - 1] = '\0';
    }

    g_RelayClient.DoGetFellowshipName(dwPlayerID, 1, &dwAlliedPlayerID);
#endif

    nResult = rrcSuccessAddBlackList;
    pResult = pBlackNode;
Exit0:
#ifdef _SERVER
    if (pPlayer && bNotifyClient)
    {
        if (nResult == rrcSuccessAddBlackList && cszAlliedPlayerName)
        {
            g_PlayerServer.DoMessageNotify(
                pPlayer->m_nConnIndex, ectFellowshipErrorCode, nResult,
                cszAlliedPlayerName, (strlen(cszAlliedPlayerName) + 1) * sizeof(cszAlliedPlayerName[0])
            );

            g_RelayClient.DoAddFellowshipNotify(dwAlliedPlayerID, pPlayer->m_dwID, pPlayer->m_szName, 3);
        }
        else
        {
            g_PlayerServer.DoMessageNotify(pPlayer->m_nConnIndex, ectFellowshipErrorCode, nResult);
        }
    }
#endif
    return pResult;
}

BOOL KGFellowshipMgr::DelBlackList(DWORD dwPlayerID, DWORD dwAlliedPlayerID)
{
    BOOL bResult    = false;
    int  nRetCode   = 0;

    // Remove relation link, dwPlayerID -X-> dwAlliedPlayerID
    nRetCode = (int)m_BlackListMap.erase(KG_ID_PAIR(dwPlayerID, dwAlliedPlayerID));
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    // Remove reverse relation link, dwPlayerID <-X- dwAlliedPlayerID
    nRetCode = (int)m_ReverseBlackListSet.erase(KG_ID_PAIR(dwAlliedPlayerID, dwPlayerID));
    KGLOG_CHECK_ERROR(nRetCode == 1);

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KGFellowshipMgr::CountBlackList(DWORD dwPlayerID)
{
    _FellowshipCount Count;

    assert(dwPlayerID);

    TraverseBlackListID(dwPlayerID, Count);

    return Count.nCount;
}

BOOL KGFellowshipMgr::ClearBlackList(DWORD dwPlayerID)
{
    BOOL            bResult = false;
    _GetIDArrayFunc ClearFunc;

    assert(dwPlayerID);

    ClearFunc.m_uArraySize = 0;
    TraverseBlackListID(dwPlayerID, ClearFunc);

    for (int i = 0; i < (int)ClearFunc.m_uArraySize; i++)
        DelBlackList(dwPlayerID, ClearFunc.m_dwAlliedPlayerIDArray[i]);

    bResult = true;
//Exit0:
    return bResult;
}

BOOL KGFellowshipMgr::AddFellowshipGroup(DWORD dwPlayerID, const char cszGroupName[])
{
    int         nResult     = rrcFailed;
    BOOL        bRetCode    = false;
    KPlayer*    pPlayer     = NULL;
    KG_FELLOWSHIP_GROUPNAMES_MAP::iterator  it;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
    KG_PROCESS_ERROR(pPlayer);

    it = m_GroupNamesMap.find(dwPlayerID);
    KG_PROCESS_ERROR(it != m_GroupNamesMap.end());

    KGLOG_PROCESS_ERROR_RET_CODE(it->second.nGroupCount < KG_FELLOWSHIP_MAX_CUSTEM_GROUP, rrcFellowshipGroupFull);

    strncpy(
        it->second.szGroupName[it->second.nGroupCount], 
        cszGroupName, 
        sizeof(it->second.szGroupName[it->second.nGroupCount])
    );
    it->second.szGroupName[it->second.nGroupCount][sizeof(it->second.szGroupName[it->second.nGroupCount]) - sizeof('\0')] = '\0';

    it->second.nGroupCount += 1;

    nResult = rrcSuccess;
Exit0:
#ifdef _SERVER
    if (nResult != rrcSuccess && pPlayer)
    {
        g_PlayerServer.DoMessageNotify(pPlayer->m_nConnIndex, ectFellowshipErrorCode, nResult);
    }
#endif
    return (nResult == rrcSuccess);
}

BOOL KGFellowshipMgr::DelFellowshipGroup(DWORD dwPlayerID, DWORD dwGroupID)
{
    int                                     nResult     = rrcFailed;
    int                                     nRetCode    = 0;
    KPlayer*                                pPlayer     = NULL;
    KGFellowship*                           pFellowship = NULL;
    GetFellowshipIDListFunc                 TraverseIDFunc;
    KG_FELLOWSHIP_GROUPNAMES_MAP::iterator  it;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
    KG_PROCESS_ERROR(pPlayer);

    it = m_GroupNamesMap.find(dwPlayerID);
    KG_PROCESS_ERROR(it != m_GroupNamesMap.end());

    KG_PROCESS_ERROR_RET_CODE(dwGroupID < (DWORD)it->second.nGroupCount, rrcFellowshipGroupNotFound);
    for (int i = (int)dwGroupID; i < it->second.nGroupCount - 1; i++)
    {
        strncpy(
            it->second.szGroupName[i], 
            it->second.szGroupName[i + 1], 
            sizeof(it->second.szGroupName[i])
        );
        it->second.szGroupName[i][sizeof(it->second.szGroupName[i]) - sizeof('\0')] = '\0';

        it->second.szGroupName[i + 1][0] = '\0';
    }

    TraverseFellowshipID(dwPlayerID, TraverseIDFunc);
    for (int nIndex = 0; nIndex < TraverseIDFunc.m_nIDListCount; nIndex++)
    {
        pFellowship = GetFellowship(dwPlayerID, TraverseIDFunc.m_dwIDList[nIndex]);
        assert(pFellowship);

        if (pFellowship->m_BitFlag.dwGroupID == dwGroupID + 1)
        {
            pFellowship->m_BitFlag.dwGroupID = 0;
        }

        if (pFellowship->m_BitFlag.dwGroupID > dwGroupID + 1)
        {
            assert(pFellowship->m_BitFlag.dwGroupID != 0);
            pFellowship->m_BitFlag.dwGroupID--;
        }
    }

    it->second.nGroupCount--;

    nResult = rrcSuccess;
Exit0:
#ifdef _SERVER
    if (nResult != rrcSuccess && pPlayer)
    {
        g_PlayerServer.DoMessageNotify(pPlayer->m_nConnIndex, ectFellowshipErrorCode, nResult);
    }
#endif
    return (nResult == rrcSuccess);
}

BOOL KGFellowshipMgr::RenameFellowshipGroup(DWORD dwPlayerID, DWORD dwGroupID, const char cszGroupName[])
{
    int         nResult     = rrcFailed;
    BOOL        bRetCode    = false;
    KPlayer*    pPlayer     = NULL;
    KG_FELLOWSHIP_GROUPNAMES_MAP::iterator  it;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
    KG_PROCESS_ERROR(pPlayer);

    it = m_GroupNamesMap.find(dwPlayerID);
    KG_PROCESS_ERROR(it != m_GroupNamesMap.end());

    KGLOG_PROCESS_ERROR_RET_CODE(dwGroupID < (DWORD)it->second.nGroupCount, rrcFellowshipGroupNotFound);

    strncpy(it->second.szGroupName[dwGroupID], cszGroupName, sizeof(it->second.szGroupName[dwGroupID]));
    it->second.szGroupName[dwGroupID][sizeof(it->second.szGroupName[dwGroupID]) - sizeof('\0')] = '\0';

    nResult = rrcSuccess;
Exit0:
#ifdef _SERVER
    if (nResult != rrcSuccess && pPlayer)
    {
        g_PlayerServer.DoMessageNotify(pPlayer->m_nConnIndex, ectFellowshipErrorCode, nResult);
    }
#endif
    return (nResult == rrcSuccess);
}

#ifdef _SERVER
int KGFellowshipMgr::AddFellowshipAttraction(DWORD dwPlayerID, DWORD dwAlliedPlayerID, int nAttraction, BOOL bBidirectional /* = true */)
{
    BOOL                bResult             = false;
    KGFellowship*       pFellowshipA        = NULL;
    KGFellowship*       pFellowshipB        = NULL;
    KPlayer*            pPlayerA            = NULL;
    KPlayer*            pPlayerB            = NULL;
    KMESSAGE_ATTRACTION MessageAttraction;

    pFellowshipA = GetFellowship(dwPlayerID, dwAlliedPlayerID);
    KG_PROCESS_ERROR(pFellowshipA);

    pPlayerA = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayerA);

    if (bBidirectional)
    {
        pFellowshipB = GetFellowship(dwAlliedPlayerID, dwPlayerID);
        KG_PROCESS_ERROR(pFellowshipB);

        pPlayerB = g_pSO3World->m_PlayerSet.GetObj(dwAlliedPlayerID);
        KGLOG_PROCESS_ERROR(pPlayerB);
    }

    KG_PROCESS_SUCCESS(nAttraction == 0);

    pFellowshipA->m_nAttraction += nAttraction;

    if (bBidirectional)
        pFellowshipB->m_nAttraction += nAttraction;

    // Notify client
    MessageAttraction.nAttraction = nAttraction;

    strncpy(MessageAttraction.szAlliedPlayerName, pFellowshipA->m_szName, sizeof(MessageAttraction.szAlliedPlayerName));
    MessageAttraction.szAlliedPlayerName[sizeof(MessageAttraction.szAlliedPlayerName) - sizeof('\0')] = '\0';

    g_PlayerServer.DoMessageNotify(pPlayerA->m_nConnIndex, ectFellowshipAttractionErrorCode, 0, &MessageAttraction, sizeof(KMESSAGE_ATTRACTION));

    if (bBidirectional)
    {
        strncpy(MessageAttraction.szAlliedPlayerName, pFellowshipB->m_szName, sizeof(MessageAttraction.szAlliedPlayerName));
        MessageAttraction.szAlliedPlayerName[sizeof(MessageAttraction.szAlliedPlayerName) - sizeof('\0')] = '\0';

        g_PlayerServer.DoMessageNotify(pPlayerB->m_nConnIndex, ectFellowshipAttractionErrorCode, 0, &MessageAttraction, sizeof(KMESSAGE_ATTRACTION));
    }

Exit1:
    bResult = true;
Exit0:
    return bResult;
}
#endif

int KGFellowshipMgr::AttractionValue2Level(int nAttraction, int nCoefficient /* = 1 */)
{
    int         nLevel                  = 0;
    int         nAttractionLevelTalbe[] = {0, 100, 200, 300, 500, 800, 1000};
    const int   cnTalbeSize             = sizeof(nAttractionLevelTalbe) / sizeof(nAttractionLevelTalbe[0]);

    if (nAttraction < 0)
    {
        return -1;
    }

    for (nLevel = 0; nLevel < cnTalbeSize; nLevel++)
    {
        if (nAttractionLevelTalbe[nLevel] * nCoefficient >= nAttraction)
            break;
    }

    return nLevel;
}

BOOL GetFellowshipIDListFunc::operator()(DWORD dwPlayerID, DWORD dwAlliedPlayerID)
{
    BOOL bResult = false;

    KGLOG_PROCESS_ERROR(m_nIDListCount < KG_FELLOWSHIP_MAX_RECORD);

    m_dwIDList[m_nIDListCount++] = dwAlliedPlayerID;

    return true;
Exit0:
    return false;
}
