#include "stdafx.h"
#include "KAchievement.h"
#include "KAchievementInfoList.h"
#include "KSO3World.h"
#include "KPlayerServer.h"
#include "KRelayClient.h"
#include "KPlayer.h"

KAchievement::KAchievement() : m_BitSet()
{
    m_pPlayer   = NULL;
    m_nPoint    = 0;
    m_nRecord   = 0;
}

KAchievement::~KAchievement()
{

}

BOOL KAchievement::Init(KPlayer* pPlayer)
{
    m_pPlayer = pPlayer;
    return true;
}

BOOL KAchievement::Save(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize)
{
    BOOL                bResult                 = false;
    BOOL                bRetCode                = false;
    size_t              uUsedSize               = 0;
    size_t              uLeftSize               = uBufferSize;
    BYTE*               pbyOffset               = pbyBuffer;

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(int));
    *(int*)pbyOffset = m_nPoint;
    uLeftSize -= sizeof(int);
    pbyOffset += sizeof(int);

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(int));
    *(int*)pbyOffset = m_nRecord;
    uLeftSize -= sizeof(int);
    pbyOffset += sizeof(int);

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(size_t));
    *(size_t*)pbyOffset = m_CounterTable.size();
    uLeftSize -= sizeof(size_t);
    pbyOffset += sizeof(size_t);

    for (KCOUNTER_TABLE::iterator it = m_CounterTable.begin(); it != m_CounterTable.end(); ++it)
    {
        KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(int) * 2);

        *(int*)pbyOffset = it->first;
        uLeftSize -= sizeof(int);
        pbyOffset += sizeof(int);

        *(int*)pbyOffset = it->second;
        uLeftSize -= sizeof(int);
        pbyOffset += sizeof(int);
    }

    bRetCode = m_BitSet.Save(&uUsedSize, pbyOffset, uLeftSize);
    KGLOG_PROCESS_ERROR(bRetCode);
    uLeftSize -= uUsedSize;
    pbyOffset += uUsedSize;

    *puUsedSize = uBufferSize - uLeftSize;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KAchievement::Load(BYTE* pbyData, size_t uDataLen)
{
    BOOL                bResult                 = false;
    BOOL                bRetCode                = false;
    size_t              uLeftSize               = uDataLen;
    BYTE*               pbyOffset               = pbyData;
    size_t              uCountSize              = 0;

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(int));
    m_nPoint = *(int*)pbyOffset;
    uLeftSize -= sizeof(int);
    pbyOffset += sizeof(int);

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(int));
    m_nRecord = *(int*)pbyOffset;
    uLeftSize -= sizeof(int);
    pbyOffset += sizeof(int);

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(size_t));
    uCountSize = *(size_t*)pbyOffset;
    uLeftSize -= sizeof(size_t);
    pbyOffset += sizeof(size_t);

    for (size_t i = 0; i < uCountSize; ++i)
    {
        int nID     = 0;
        int nCount  = 0;

        KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(int) * 2);

        nID = *(int*)pbyOffset;
        uLeftSize -= sizeof(int);
        pbyOffset += sizeof(int);

        nCount = *(int*)pbyOffset;
        uLeftSize -= sizeof(int);
        pbyOffset += sizeof(int);

        m_CounterTable[nID] = nCount;
    }

    bRetCode = m_BitSet.Load(pbyOffset, uLeftSize);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KAchievement::AddCount(int nID, int nDeltaCount)
{
    BOOL                        bResult     = false;
    BOOL                        bRetCode    = false;
    BOOL                        bFinish     = false;
    KACHIEVEMENT_INFO*          pInfo       = NULL;
    int                         nCount      = 0;
    KCOUNTER_TABLE::iterator    it;

    assert(m_pPlayer);

#ifdef _SERVER
    assert(m_pPlayer->m_pScene);

    g_PlayerServer.DoSyncAchievementCount(m_pPlayer, nID, nDeltaCount);
#endif // _SERVER

    while (true)
    {
        pInfo = g_pSO3World->m_Settings.m_AchievementInfoList.GetInfo(nID);
        KGLOG_PROCESS_ERROR(pInfo);
        KG_PROCESS_SUCCESS(pInfo->nTriggerVal < 0);// 如果小于0 表示这个成就暂时不开放

        bRetCode = m_BitSet.GetBit(nID, &bFinish);
        KGLOG_PROCESS_ERROR(bRetCode);

        if (bFinish)
        {
            nCount = pInfo->nTriggerVal;
        }
        else
        {
            it = m_CounterTable.find(nID);

            nCount = 0;
            if (it != m_CounterTable.end())
            {
                nCount = it->second;
            }
        }

        nCount += nDeltaCount;

        if (!bFinish)
        {
            if (nCount >= pInfo->nTriggerVal)
            {
                // 触发达成该成就
                OnAchieve(nID, pInfo);

                if (it != m_CounterTable.end())
                {
                    m_CounterTable.erase(it);
                }
            }
            else
            {
                m_CounterTable[nID] = nCount;
                break;
            }
        }

        switch (pInfo->byShiftType)
        {
        case astNeverShift:
            KG_PROCESS_SUCCESS(true);
            break;

        case astComplete:
            nDeltaCount = nCount;
            if (bFinish)
            {
                nDeltaCount -= pInfo->nTriggerVal;
            }
            break;

        case astShiftTriggerValOnce:
            nDeltaCount = pInfo->nTriggerVal;
            if (bFinish)
            {
                nDeltaCount = 0;
            }
            break;

        default:
            assert(false);
        }
        nID = pInfo->nShiftID;
    }

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

int KAchievement::GetCount(int nID)
{
    BOOL                        bRetCode    = false;
    BOOL                        bFinish     = false;
    KACHIEVEMENT_INFO*          pInfo       = NULL;
    int                         nCount      = -1;
    KCOUNTER_TABLE::iterator    it;

    bRetCode = m_BitSet.GetBit(nID, &bFinish);
    KGLOG_PROCESS_ERROR(bRetCode);

    if (bFinish)
    {
        pInfo = g_pSO3World->m_Settings.m_AchievementInfoList.GetInfo(nID);
        KGLOG_PROCESS_ERROR(pInfo);

        nCount = pInfo->nTriggerVal;
    }
    else
    {
        it = m_CounterTable.find(nID);

        nCount = 0;
        if (it != m_CounterTable.end())
        {
            nCount = it->second;
        }
    }

Exit0:
    return nCount;
}

BOOL KAchievement::IsAchieve(int nID)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    KGLOG_PROCESS_ERROR(nID >= 0 && nID < MAX_ACHIEVEMENT_ID);

    bRetCode = m_BitSet.GetBit(nID, &bResult);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return bResult;
}

BOOL KAchievement::AddPoint(int nDeltaPoint)
{
    BOOL bResult = false;

    KGLOG_PROCESS_ERROR(nDeltaPoint >= 0 || m_nPoint >= -nDeltaPoint);
    assert(m_pPlayer);

    m_nPoint += nDeltaPoint;

#ifdef _SERVER
    g_PlayerServer.DoSyncAchievementPoint(m_pPlayer, nDeltaPoint);

    g_pSO3World->m_StatDataServer.UpdateAchievementPointStat(m_pPlayer, nDeltaPoint);
#endif // _SERVER

    bResult = true;
Exit0:
    return bResult;
}

BOOL KAchievement::Acquire(int nID)
{
    BOOL                        bResult     = false;
    BOOL                        bRetCode    = false;
    BOOL                        bFinish     = false;
    KACHIEVEMENT_INFO*          pInfo       = NULL;
    int                         nCount      = 0;
    KCOUNTER_TABLE::iterator    it;

    bRetCode = m_BitSet.GetBit(nID, &bFinish);
    KGLOG_PROCESS_ERROR(bRetCode);
    KG_PROCESS_ERROR(!bFinish);

    pInfo = g_pSO3World->m_Settings.m_AchievementInfoList.GetInfo(nID);
    KGLOG_PROCESS_ERROR(pInfo);
    KG_PROCESS_SUCCESS(pInfo->nTriggerVal < 0);// 如果小于0 表示这个成就暂时不开放

    it = m_CounterTable.find(nID);
    if (it != m_CounterTable.end())
    {
        nCount = it->second;
    }
    KGLOG_PROCESS_ERROR(nCount <= pInfo->nTriggerVal);

    bRetCode = AddCount(nID, pInfo->nTriggerVal - nCount);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KAchievement::OnAchieve(int nID, KACHIEVEMENT_INFO* pInfo)
{
    BOOL                bResult         = false;
    BOOL                bRetCode        = false;
    BYTE                byType          = aatInvalid;
    const int           nNearbyPoint    = 25;
    const int           nScenePoint     = 39;

    KGLOG_PROCESS_ERROR(nID >= 0 && nID < MAX_ACHIEVEMENT_ID);
    assert(m_pPlayer);
    assert(pInfo);

    bRetCode = m_BitSet.SetBit(nID, true);
    KGLOG_PROCESS_ERROR(bRetCode);

    m_nPoint  += pInfo->nPoint;
    m_nRecord += pInfo->nPoint;

#ifdef _SERVER
    if (pInfo->byPrefix != 0)
    {
        m_pPlayer->m_Designation.AcquirePrefix(pInfo->byPrefix);
    }

    if (pInfo->byPostfix != 0)
    {
        m_pPlayer->m_Designation.AcquirePostfix(pInfo->byPostfix);
    }

    if (
        m_pPlayer->m_nLevel < g_pSO3World->m_Settings.m_ConstList.nFreeMaxLevel ||
        !m_pPlayer->m_bFreeLimitFlag
    )
    {
        m_pPlayer->AddExp(pInfo->nExp);

        g_pSO3World->m_StatDataServer.UpdateExpStat(m_pPlayer, pInfo->nExp, "OTHER");
    }

    g_PlayerServer.DoNewAchievementNotify(m_pPlayer, nID);

    if (pInfo->nPoint <= nNearbyPoint)
    {
        byType = aatNearby;
    }
    else if (pInfo->nPoint <= nScenePoint)
    {
        byType = aatScene;
    }
    else
    {
        byType = aatGlobal;
    }

    if (byType == aatGlobal)
    {
        g_RelayClient.DoAchievementGlobalAnnounceRequest(m_pPlayer->m_dwID, nID);
    }
    else
    {
        g_PlayerServer.BroadcastAchievementAnnounce(m_pPlayer->m_dwID, byType, m_pPlayer->m_szName, nID);
    }

    CallFinishScript(nID);

    g_pSO3World->m_StatDataServer.UpdateAchievementStat(nID);

    g_pSO3World->m_StatDataServer.UpdateAchievementPointStat(m_pPlayer, pInfo->nPoint);

#endif // _SERVER

    bResult = true;
Exit0:
    return bResult;
}

#ifdef _SERVER
BOOL KAchievement::CallFinishScript(int nID)
{
    BOOL    bRetCode    = false;
    int     nTopIndex   = 0;

    assert(m_pPlayer);

    bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(ACHIEVEMENT_SCRIPT);
    KG_PROCESS_ERROR(bRetCode);

    bRetCode = g_pSO3World->m_ScriptCenter.IsFuncExist(ACHIEVEMENT_SCRIPT, SCRIPT_ON_FINISH_ACHIEVEMENT);
    KG_PROCESS_ERROR(bRetCode);

    g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);

    g_pSO3World->m_ScriptCenter.PushValueToStack(m_pPlayer);
    g_pSO3World->m_ScriptCenter.PushValueToStack(nID);

    g_pSO3World->m_ScriptCenter.CallFunction(ACHIEVEMENT_SCRIPT, SCRIPT_ON_FINISH_ACHIEVEMENT, 0);
    g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);

Exit0:
    return true;
}
#endif // _SERVER
