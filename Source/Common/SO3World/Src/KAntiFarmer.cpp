#include "stdafx.h"
#include "KAntiFarmer.h"
#include "KSO3World.h"

KAntiFarmer::KAntiFarmer()
{
    m_nNextKillNpcRecordPos = 0;
    m_KilledNpcRecords.resize(g_pSO3World->m_Settings.m_ConstList.nAntiFarmerRecordKillNpcNum, 0);
}

BOOL KAntiFarmer::Save(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize)
{
    BOOL    bResult                 = false;
    BYTE*   pbyOffset               = pbyBuffer;
    size_t  uLeftSize               = uBufferSize;
    int     nKillNpcCount           = 0;
    int     nIndexOffset            = 0;
    int     nNpcRecordNum           = (int)m_KilledNpcRecords.size();

    assert(puUsedSize);
    assert(pbyBuffer);

    // 保存在线时间纪录
    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(int));
    *(int*)pbyOffset = (int)m_OnlineRecordList.size();
    pbyOffset += sizeof(int);
    uLeftSize -= sizeof(int);

    for (KONLINE_RECORD_LIST::iterator it = m_OnlineRecordList.begin(); it != m_OnlineRecordList.end(); ++it)
    {
        KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KOnlineRecord));
        *(KOnlineRecord*)pbyOffset = *it;

        pbyOffset += sizeof(KOnlineRecord);
        uLeftSize -= sizeof(KOnlineRecord);
    }

    // 保存打怪纪录
    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(int));
    // 循环数组,下一个位置有记录,说明已经满了
    if (m_KilledNpcRecords[m_nNextKillNpcRecordPos % nNpcRecordNum] != ERROR_ID)
    {
        nKillNpcCount = nNpcRecordNum;
    }
    else
    {
        assert(m_nNextKillNpcRecordPos < nNpcRecordNum);
        nKillNpcCount = m_nNextKillNpcRecordPos;
    }

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(int));
    *(int*)pbyOffset = nKillNpcCount;
    pbyOffset += sizeof(int);
    uLeftSize -= sizeof(int);

    assert(m_nNextKillNpcRecordPos >= nKillNpcCount);
    nIndexOffset = m_nNextKillNpcRecordPos - nKillNpcCount;

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(DWORD) * nKillNpcCount);

    for (int i = 0; i < nKillNpcCount; i++)
    {
        int nIndex = (nIndexOffset + i) % nNpcRecordNum;

        *(DWORD*)pbyOffset = m_KilledNpcRecords[nIndex];
        pbyOffset += sizeof(DWORD);
        uLeftSize -= sizeof(DWORD);
    }

    *puUsedSize = (size_t)(pbyOffset - pbyBuffer);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KAntiFarmer::Load(BYTE* pbyData, size_t uDataLen)
{
    BOOL                            bResult                 = false;
    BYTE*                           pbyOffset               = pbyData;
    size_t                          uLeftSize               = uDataLen;
    int                             nOnlineRecordCount      = 0;
    KOnlineRecord*                  pOnlineRecord           = NULL;
    int                             nKillNpcRecordCount     = 0;
    int                             nLoadNpcRecordCount     = 0;

    assert(pbyOffset);

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(int));
    nOnlineRecordCount = *(int*)pbyOffset;
    pbyOffset += sizeof(int);
    uLeftSize -= sizeof(int);
        
    while (nOnlineRecordCount-- > 0)
    {
        KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KOnlineRecord));        
        pOnlineRecord = (KOnlineRecord*)pbyOffset;
        pbyOffset += sizeof(KOnlineRecord);
        uLeftSize -= sizeof(KOnlineRecord);

        m_OnlineRecordList.push_back(*pOnlineRecord);
    }

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(int));
    nKillNpcRecordCount = *(int*)pbyOffset;
    pbyOffset += sizeof(int);
    uLeftSize -= sizeof(int);

    KGLOG_PROCESS_ERROR(nKillNpcRecordCount >= 0);

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(DWORD) * nKillNpcRecordCount);
    nLoadNpcRecordCount = min(nKillNpcRecordCount, (int)m_KilledNpcRecords.size());
    memcpy(&m_KilledNpcRecords[0], pbyOffset, sizeof(DWORD) * nLoadNpcRecordCount);
    m_nNextKillNpcRecordPos = nKillNpcRecordCount;
    pbyOffset += sizeof(DWORD) * nKillNpcRecordCount;
    uLeftSize -= sizeof(DWORD) * nKillNpcRecordCount;

    KGLOG_PROCESS_ERROR(uLeftSize == 0);
        
    bResult = true;
Exit0:
    return bResult;
}

void KAntiFarmer::RecordOnlineTime()
{
    time_t          nTimeNow = g_pSO3World->m_nCurrentTime;
    KOnlineRecord   Record;

    // 删除过期节点
    while (!m_OnlineRecordList.empty())
    {
        KOnlineRecord& rNode = m_OnlineRecordList.front();
     
        if (rNode.nLogoutTime > nTimeNow - g_pSO3World->m_Settings.m_ConstList.nAntiFarmerCheckOnlineTimeRange)
            break;

        m_OnlineRecordList.pop_front();
    }

    // 看是不是直接加长最后一个在线时间区段就行了
    if (!m_OnlineRecordList.empty())
    {
        KOnlineRecord& rNode = m_OnlineRecordList.back();
                
        if (nTimeNow - rNode.nLogoutTime < ANTI_FARMER_CHECK_ONLINE_TIME_INTERVAL * 2)
        {
            KG_PROCESS_ERROR(rNode.nLogoutTime <= nTimeNow);
            rNode.nLogoutTime = nTimeNow;
            goto Exit0;
        }
    }

    Record.nLoginTime  = nTimeNow;
    Record.nLogoutTime = nTimeNow;

    m_OnlineRecordList.push_back(Record);

Exit0:
    return;
}

void KAntiFarmer::RecordKillNpc(DWORD dwNpcTemplateID)
{
    int nMaxRecordCount = (int)m_KilledNpcRecords.size();
    int nIndex          = 0;

    assert(nMaxRecordCount > 0);

    KGLOG_PROCESS_ERROR(dwNpcTemplateID != 0);

    nIndex = m_nNextKillNpcRecordPos++ % nMaxRecordCount;
    m_KilledNpcRecords[nIndex] = (dwNpcTemplateID);

Exit0:
    return;
}

typedef std::map<DWORD, int, std::less<DWORD>, KMemory::KAllocator<std::pair<DWORD, int> > > KSTAT_TABLE;
typedef std::vector<int, KMemory::KAllocator<int> > KKILL_NPC_RATE_STAT;

BOOL KAntiFarmer::Judge()
{
    BOOL                bResult             = false;
    DWORD               dwNpcTemplateID     = 0;
    int                 nMaxRecordNpcCount  = g_pSO3World->m_Settings.m_ConstList.nAntiFarmerRecordKillNpcNum;
    int                 nStatKillNpcTypeNum = g_pSO3World->m_Settings.m_ConstList.nAntiFarmerKillNpcTypeNum;
    int                 nKillNpcRecordNum   = (int)m_KilledNpcRecords.size();
    int                 nMaxCount           = 0;
    int                 nKilledNpcCount     = 0;
    DWORD               dwTemplateID        = 0;
    float               fKillNpcRate        = 0.0f;
    float               fOnlineRate         = 0.0f;
    time_t              nTimeSectionBegin   = 0;
    time_t              nOnlineTotal        = 0;
    KSTAT_TABLE         StatTable;
    KKILL_NPC_RATE_STAT NpcRate;

    KG_PROCESS_ERROR(nMaxRecordNpcCount > 0);

    for (int i = 0; i < nKillNpcRecordNum; i++)
    {
        dwNpcTemplateID = m_KilledNpcRecords[i];
        if (dwNpcTemplateID == 0)
            continue;

        KSTAT_TABLE::iterator it = StatTable.find(dwNpcTemplateID);
        if (it == StatTable.end())
        {
            StatTable[dwNpcTemplateID] = 1;
            continue;
        }

        StatTable[dwNpcTemplateID]++;
    }

    if (StatTable.empty())
        goto Exit0;

    NpcRate.reserve(nStatKillNpcTypeNum);

    // 找到排名前N种NPC所占的总数(N为配置项)
    for (int i = 0; i < nStatKillNpcTypeNum; i++)
    {
        nMaxCount       = 0;
        dwTemplateID    = ERROR_ID;

        for (KSTAT_TABLE::iterator it = StatTable.begin(); it != StatTable.end(); ++it)
        {
            if (it->second > nMaxCount)
            {
                nMaxCount       = it->second;
                dwTemplateID    = it->first;
            }
        }

        if (nMaxCount == 0)
            break;
        
        NpcRate.push_back(nMaxCount);

        StatTable.erase(dwTemplateID);
    }

    assert(!NpcRate.empty());

    if (nStatKillNpcTypeNum > (int)NpcRate.size())
    {
        nStatKillNpcTypeNum = (int)NpcRate.size();
    }
   
    for (int i = 0; i < nStatKillNpcTypeNum; i++)
    {
        nKilledNpcCount += NpcRate[i];
    }

    fKillNpcRate = (float)nKilledNpcCount / (float)nMaxRecordNpcCount;
    KG_PROCESS_ERROR(fKillNpcRate >= g_pSO3World->m_Settings.m_ConstList.fAntiFarmerKillNpcRate);

    nTimeSectionBegin = g_pSO3World->m_nCurrentTime - g_pSO3World->m_Settings.m_ConstList.nAntiFarmerCheckOnlineTimeRange;

    for (KONLINE_RECORD_LIST::iterator it = m_OnlineRecordList.begin(); it != m_OnlineRecordList.end(); ++it)
    {
        if (it->nLogoutTime < nTimeSectionBegin)
            continue;

        time_t nLoginTime = max(it->nLoginTime, nTimeSectionBegin);

        nOnlineTotal += (int)(it->nLogoutTime - nLoginTime);
    }

    fOnlineRate = (float)nOnlineTotal / (float)g_pSO3World->m_Settings.m_ConstList.nAntiFarmerCheckOnlineTimeRange;
    KG_PROCESS_ERROR(fOnlineRate >= g_pSO3World->m_Settings.m_ConstList.fAntiFarmerOnlineRate);
    
Exit1:
    bResult = true;
Exit0:
    return bResult;
}
