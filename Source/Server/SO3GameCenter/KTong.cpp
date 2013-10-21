#include "stdafx.h"
#include "StrMap.h"
#include "KTong.h"
#include "KRole.h"
#include "KSO3GameCenter.h"
#include "KItemDef.h"

KTong::KTong()
{
    m_dwID                  = ERROR_ID;
    m_dwMaster              = ERROR_ID;
    m_nFund                 = 0;
    m_nMaxWageRate          = 0;
    m_nLevel                = 0;
    m_nDevelopmentPoint     = 0;
    m_nMaxMemberCount       = 0;

    m_szName[0]             = '\0';
    m_szAnnouncement[0]     = '\0';
    m_szOnlineMessage[0]    = '\0';
    m_szIntroduction[0]     = '\0';
    m_szRules[0]            = '\0';

    m_bUpdateFlag           = false;
    m_eState                = tsInvalid;
    m_nStateTimer           = 0;

    m_eCamp                 = cNeutral;
    m_dwMemorabiliaCount    = 0;

    m_nUsedDevelopmentPoint = 0;
    memset(m_byTechTree, 0, sizeof(m_byTechTree));
    memset(m_nLastUpdateFrameTable, 0, sizeof(m_nLastUpdateFrameTable));
}

BOOL KTong::Init()
{
    BOOL bResult            = false;
    BOOL bHistoryInitFlag   = false;
    BOOL bRepertoryInitFlag = false;

    bHistoryInitFlag = m_History.Init();
    KGLOG_PROCESS_ERROR(bHistoryInitFlag);

    bRepertoryInitFlag = m_Repertory.Init();
    KGLOG_PROCESS_ERROR(bRepertoryInitFlag);

    bResult = true;
Exit0:
    if (!bResult)
    {
        if (bHistoryInitFlag)
            m_History.UnInit();
        if (bRepertoryInitFlag)
            m_Repertory.UnInit();
    }
    return bResult;
}

void KTong::UnInit()
{
    m_Repertory.UnInit();
    m_History.UnInit();
}

void KTong::Activate()
{
    if (m_bUpdateFlag)
    {
        g_pSO3GameCenter->m_MiscDB.DoSaveTongData(m_dwID, this);
        m_bUpdateFlag = false;
    }

    for (KROSTER::iterator it = m_Roster.begin(); it != m_Roster.end(); ++it)
    {
        BOOL            bUpdateFlag = false;
        KRole*          pRole       = NULL;
        KTongMember*    pMember     = &it->second;

        pRole = g_pSO3GameCenter->m_RoleManager.GetRole(it->first);
        if (!pRole)
            continue;
        
        bUpdateFlag |= (pMember->nLevel != pRole->m_byLevel);
        bUpdateFlag |= (pMember->dwForceID != pRole->m_dwForceID);
        bUpdateFlag |= (pMember->dwMapID != pRole->m_dwMapID);
        bUpdateFlag |= (pMember->bIsOnline != pRole->IsOnline());
        
        if (!bUpdateFlag)
            continue;
        
        pMember->nLevel     = pRole->m_byLevel;
        pMember->dwForceID  = pRole->m_dwForceID;
        pMember->dwMapID    = pRole->m_dwMapID;
        pMember->bIsOnline  = pRole->IsOnline();

        pMember->nLastUpdateFrame = g_pSO3GameCenter->m_nWorkLoop;
    }
}

BOOL KTong::Load(DWORD dwTongID, BYTE* pbyData, size_t uDataLen)
{
    BOOL                bResult     = false;
    BOOL                bRetCode    = false;
    KTONG_DB_HEADER*    pHeader     = (KTONG_DB_HEADER*)pbyData;
    BYTE*               pbyOffSet   = pbyData;
    size_t              uLeftSize   = uDataLen;
    
    assert(pbyData);

    KGLOG_PROCESS_ERROR(uDataLen >= sizeof(KTONG_DB_HEADER));
    KGLOG_PROCESS_ERROR(pHeader->nVersion <= TONG_DB_VERSION);

    m_dwID = dwTongID;

    pbyOffSet += sizeof(KTONG_DB_HEADER);
    uLeftSize -= sizeof(KTONG_DB_HEADER);

    while (uLeftSize)
    {
        KTONG_DB_SECTION* pSection = (KTONG_DB_SECTION*)pbyOffSet;
        pbyOffSet += sizeof(KTONG_DB_SECTION);
        uLeftSize -= sizeof(KTONG_DB_SECTION);

        KGLOG_PROCESS_ERROR(uLeftSize >= pSection->uDataLen);

        switch (pSection->nType)
        {
        case eTongBaseInfo:
            if (pHeader->nVersion == 1)
            {
                bRetCode = LoadBaseInfoVersion1(pSection->byData, pSection->uDataLen);
                KGLOG_PROCESS_ERROR(bRetCode);
            }
            else
            {
                KGLOG_PROCESS_ERROR(pHeader->nVersion == TONG_DB_VERSION);
                bRetCode = LoadBaseInfo(pSection->byData, pSection->uDataLen);
                KGLOG_PROCESS_ERROR(bRetCode);
            }
            break;

        case eTongSchema:
            bRetCode = LoadSchema(pSection->byData, pSection->uDataLen);
            KGLOG_PROCESS_ERROR(bRetCode);
            break;

        case eTongRoster:
            if (pHeader->nVersion == 1)
            {
                bRetCode = LoadRosterVersion1(pSection->byData, pSection->uDataLen);
                KGLOG_PROCESS_ERROR(bRetCode);
            }
            else
            {
                KGLOG_PROCESS_ERROR(pHeader->nVersion == TONG_DB_VERSION);
                bRetCode = LoadRoster(pSection->byData, pSection->uDataLen);
                KGLOG_PROCESS_ERROR(bRetCode);
            }
            break;

        case eTongAnnouncement:
            strncpy(m_szAnnouncement, (const char*)pSection->byData, sizeof(m_szAnnouncement));
            m_szAnnouncement[sizeof(m_szAnnouncement) - 1] = '\0';
            break;

        case eTongOnlineMessage:
            strncpy(m_szOnlineMessage, (const char*)pSection->byData, sizeof(m_szOnlineMessage));
            m_szOnlineMessage[sizeof(m_szOnlineMessage) - 1] = '\0';
            break;

        case eTongIntroduction:
            strncpy(m_szIntroduction, (const char*)pSection->byData, sizeof(m_szIntroduction));
            m_szIntroduction[sizeof(m_szIntroduction) - 1] = '\0';
            break;

        case eTongRules:
            strncpy(m_szRules, (const char*)pSection->byData, sizeof(m_szRules));
            m_szRules[sizeof(m_szRules) - 1] = '\0';
            break;

        case eTongMemorabilia:
            bRetCode = LoadMemorabilia(pSection->byData, pSection->uDataLen);
            KGLOG_PROCESS_ERROR(bRetCode);
            break;

        case eTongRepertory:
            bRetCode = m_Repertory.Load(pSection->byData, pSection->uDataLen);
            KGLOG_PROCESS_ERROR(bRetCode);
            break;

        case eTongDevelopmentInfo:
            bRetCode = LoadDevelopmentInfo(pSection->byData, pSection->uDataLen);
            KGLOG_PROCESS_ERROR(bRetCode);
            break;
        case eTongHistory:
            bRetCode = m_History.Load(pSection->byData, pSection->uDataLen);
            KGLOG_PROCESS_ERROR(bRetCode);
            break;

        default:
            KGLogPrintf(KGLOG_ERR, "[KTong] DB error, can not recognize section type\n");
            goto Exit0;
        }

        pbyOffSet += pSection->uDataLen;
        uLeftSize -= pSection->uDataLen;
    }

    if (pHeader->nVersion == 1)
    {
        // 旧版本数据开启仓库默认页
        m_Repertory.m_Page[0].bEnable = true;
    }

    if (pHeader->nVersion < TONG_DB_VERSION) // 不同版本的数据格式转换之后尽快存盘
    {
        m_bUpdateFlag = true;
    }
 
    bResult = true;
Exit0:
    return bResult;
}


BOOL KTong::Save(size_t* puUseSize, BYTE* pbyBuffer, size_t uBufferSize)
{
    BOOL                bResult     = false;
    BOOL                bRetCode    = false;
    int                 nRetCode    = 0;
    KTONG_DB_HEADER*    pHeader     = NULL;
    unsigned int        uUsedSize   = 0;
    size_t              uLeftSize   = uBufferSize;
    size_t              uStrLen     = 0;
    BYTE*               pbyOffset   = NULL;
    KTONG_DB_SECTION*   pSection    = NULL;

    assert(pbyBuffer);
    assert(puUseSize);

    assert(uBufferSize >=
        sizeof(KTONG_DB_HEADER) +
        sizeof(KTONG_DB_SECTION) + sizeof(KTONG_DB_BASEINFO) +
        sizeof(KTONG_DB_SECTION) + sizeof(KTONG_DB_SCHEMA) + 
        sizeof(KTONG_DB_SECTION) + sizeof(KTONG_DB_ROSTER) + sizeof(KTONG_DB_MEMBER_DATA) * MAX_TONG_MEMBER_COUNT +
        sizeof(KTONG_DB_SECTION) + sizeof(m_szAnnouncement) +
        sizeof(KTONG_DB_SECTION) + sizeof(m_szOnlineMessage) +
        sizeof(KTONG_DB_SECTION) + sizeof(m_szIntroduction) +
        sizeof(KTONG_DB_SECTION) + sizeof(m_szRules) +
        sizeof(KTONG_DB_SECTION) +  (sizeof(KTONG_DB_MEMORABILIA) + TONG_MEMORABILIA_RECORD_LEN) * MAX_TONG_MEMORABILIA_COUNT +
        sizeof(KTONG_DB_SECTION) + TONG_REPERTORY_PAGE_NUM *
            (sizeof(KTONG_REPERTORY_DB_PAGE) + TONG_REPERTORY_PAGE_CAPACITY * (sizeof(KTONG_REPERTORY_DB_ITEM) + MAX_ITEM_DATA_SIZE)) +
        sizeof(KTONG_DB_SECTION) + sizeof(KTONG_DB_DEVELOPMENT_DATA) +
        sizeof(KTONG_DB_SECTION) + sizeof(KTONG_DB_HISTORY_CATEGORY) * ohtTotal + sizeof(KTONG_MAX_SIZE_HISTORY_DATA) * TOTAL_RECORD_COUNT
    );

    // --------------------------------------------------------------------
    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KTONG_DB_HEADER));

    pHeader = (KTONG_DB_HEADER*)pbyBuffer;
    pbyOffset = pbyBuffer + sizeof(KTONG_DB_HEADER);
    uLeftSize -= sizeof(KTONG_DB_HEADER);

    pHeader->nVersion = TONG_DB_VERSION;

    // --------------------------------------------------------------------
    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KTONG_DB_SECTION));

    pSection = (KTONG_DB_SECTION*)pbyOffset;
    pbyOffset += sizeof(KTONG_DB_SECTION);
    uLeftSize -= sizeof(KTONG_DB_SECTION);

    pSection->nType = eTongBaseInfo;

    bRetCode = SaveBaseInfo(pSection->byData, uLeftSize, &uUsedSize);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    pSection->uDataLen = uUsedSize;
    pbyOffset += uUsedSize;
    uLeftSize -= uUsedSize;

    // --------------------------------------------------------------------
    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KTONG_DB_SECTION));

    pSection = (KTONG_DB_SECTION*)pbyOffset;
    pbyOffset += sizeof(KTONG_DB_SECTION);
    uLeftSize -= sizeof(KTONG_DB_SECTION);

    pSection->nType = eTongSchema;
    
    bRetCode = SaveSchema(pSection->byData, uLeftSize, &uUsedSize);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    pSection->uDataLen = uUsedSize;
    pbyOffset += uUsedSize;
    uLeftSize -= uUsedSize;

    // --------------------------------------------------------------------
    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KTONG_DB_SECTION));

    pSection = (KTONG_DB_SECTION*)pbyOffset;
    pbyOffset += sizeof(KTONG_DB_SECTION);
    uLeftSize -= sizeof(KTONG_DB_SECTION);

    pSection->nType = eTongRoster;
    
    bRetCode = SaveRoster(pSection->byData, uLeftSize, &uUsedSize);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    pSection->uDataLen = uUsedSize;
    pbyOffset += uUsedSize;
    uLeftSize -= uUsedSize;

    // --------------------------------------------------------------------
    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KTONG_DB_SECTION));

    pSection = (KTONG_DB_SECTION*)pbyOffset;
    pbyOffset += sizeof(KTONG_DB_SECTION);
    uLeftSize -= sizeof(KTONG_DB_SECTION);

    pSection->nType = eTongAnnouncement;

    uStrLen = strlen(m_szAnnouncement) + 1;
    KGLOG_PROCESS_ERROR(uStrLen <= uLeftSize);
    memcpy(pSection->byData, m_szAnnouncement, uStrLen);

    pSection->uDataLen = uStrLen;
    pbyOffset += uStrLen;
    uLeftSize -= uStrLen;

    // --------------------------------------------------------------------
    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KTONG_DB_SECTION));

    pSection = (KTONG_DB_SECTION*)pbyOffset;
    pbyOffset += sizeof(KTONG_DB_SECTION);
    uLeftSize -= sizeof(KTONG_DB_SECTION);

    pSection->nType = eTongOnlineMessage;

    uStrLen = strlen(m_szOnlineMessage) + 1;
    KGLOG_PROCESS_ERROR(uStrLen <= uLeftSize);
    memcpy(pSection->byData, m_szOnlineMessage, uStrLen);

    pSection->uDataLen = uStrLen;
    pbyOffset += uStrLen;
    uLeftSize -= uStrLen;

    // --------------------------------------------------------------------
    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KTONG_DB_SECTION));

    pSection = (KTONG_DB_SECTION*)pbyOffset;
    pbyOffset += sizeof(KTONG_DB_SECTION);
    uLeftSize -= sizeof(KTONG_DB_SECTION);

    pSection->nType = eTongIntroduction;

    uStrLen = strlen(m_szIntroduction) + 1;
    KGLOG_PROCESS_ERROR(uStrLen <= uLeftSize);
    memcpy(pSection->byData, m_szIntroduction, uStrLen);

    pSection->uDataLen = uStrLen;
    pbyOffset += uStrLen;
    uLeftSize -= uStrLen;

    // --------------------------------------------------------------------
    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KTONG_DB_SECTION));

    pSection = (KTONG_DB_SECTION*)pbyOffset;
    pbyOffset += sizeof(KTONG_DB_SECTION);
    uLeftSize -= sizeof(KTONG_DB_SECTION);

    pSection->nType = eTongRules;

    uStrLen = strlen(m_szRules) + 1;
    KGLOG_PROCESS_ERROR(uStrLen <= uLeftSize);
    memcpy(pSection->byData, m_szRules, uStrLen);

    pSection->uDataLen = uStrLen;
    pbyOffset += uStrLen;
    uLeftSize -= uStrLen;
    // --------------------------------------------------------------------

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KTONG_DB_SECTION));

    pSection = (KTONG_DB_SECTION*)pbyOffset;
    pbyOffset += sizeof(KTONG_DB_SECTION);
    uLeftSize -= sizeof(KTONG_DB_SECTION);

    pSection->nType = eTongMemorabilia;
    bRetCode = SaveMemorabilia(pSection->byData, uLeftSize, &uUsedSize);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    pSection->uDataLen = uUsedSize;
    pbyOffset += uUsedSize;
    uLeftSize -= uUsedSize;

    // --------------------------------------------------------------------

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KTONG_DB_SECTION));

    pSection = (KTONG_DB_SECTION*)pbyOffset;
    pbyOffset += sizeof(KTONG_DB_SECTION);
    uLeftSize -= sizeof(KTONG_DB_SECTION);

    pSection->nType = eTongRepertory;

    bRetCode = m_Repertory.Save(pSection->byData, uLeftSize, &uUsedSize);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    pSection->uDataLen = uUsedSize;
    pbyOffset += uUsedSize;
    uLeftSize -= uUsedSize;

    // --------------------------------------------------------------------
    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KTONG_DB_SECTION));

    pSection = (KTONG_DB_SECTION*)pbyOffset;
    pbyOffset += sizeof(KTONG_DB_SECTION);
    uLeftSize -= sizeof(KTONG_DB_SECTION);

    pSection->nType = eTongDevelopmentInfo;

    bRetCode = SaveDevelopmentInfo(pSection->byData, uLeftSize, &uUsedSize);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    pSection->uDataLen = uUsedSize;
    pbyOffset += uUsedSize;
    uLeftSize -= uUsedSize;

    // --------------------------------------------------------------------
    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KTONG_DB_SECTION));

    pSection = (KTONG_DB_SECTION*)pbyOffset;
    pbyOffset += sizeof(KTONG_DB_SECTION);
    uLeftSize -= sizeof(KTONG_DB_SECTION);
        
    pSection->nType = eTongHistory;
    bRetCode = m_History.Sava(pSection->byData, uLeftSize, &uUsedSize);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    pSection->uDataLen = uUsedSize;
    pbyOffset += uUsedSize;
    uLeftSize -= uUsedSize;
    // --------------------------------------------------------------------
    *puUseSize = uBufferSize - uLeftSize;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTong::AddNewMember(KRole* pRole, int nGroupID)
{
    BOOL                            bResult     = false;
    KTongMember*                    pMember     = NULL;
    pair<KROSTER::iterator, BOOL>   MemberInsRet;

    assert(pRole);

    KGLOG_PROCESS_ERROR(pRole->m_dwTongID == ERROR_ID);
    KGLOG_PROCESS_ERROR(nGroupID >= 0 && nGroupID < MAX_TONG_GROUP);
    KGLOG_PROCESS_ERROR(m_nMaxMemberCount > (int)m_Roster.size());
    KGLOG_PROCESS_ERROR(m_Schema.Data[nGroupID].bEnable);

    MemberInsRet = m_Roster.insert(make_pair(pRole->m_dwPlayerID, KTongMember()));
    KGLOG_PROCESS_ERROR(MemberInsRet.second);

    pMember                     = &MemberInsRet.first->second;

    pMember->dwID               = pRole->m_dwPlayerID;
    pMember->nGroupID           = nGroupID;
    pMember->nJoinTime          = g_pSO3GameCenter->m_nTimeNow;
    pMember->szRemark[0]        = '\0';
    pMember->nLastOfflineTime   = 0;
    pMember->nSalary            = 0;

    pMember->nLevel             = pRole->m_byLevel;
    pMember->dwForceID          = pRole->m_dwForceID;
    pMember->dwMapID            = pRole->m_dwMapID;
    pMember->bIsOnline          = pRole->IsOnline();

    pMember->nLastUpdateFrame   = g_pSO3GameCenter->m_nWorkLoop;

    pRole->m_dwTongID           = m_dwID;

    m_bUpdateFlag               = true;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTong::RemoveMember(KRole* pRole)
{
    BOOL                bResult     = false;
    BOOL                bRetCode    = false;
    KTongMember*        pMember     = NULL;
    KROSTER::iterator   it;

    assert(pRole);

    // 会长必须移交之后才可退出帮会
    KG_PROCESS_ERROR(pRole->m_dwPlayerID != m_dwMaster);

    it = m_Roster.find(pRole->m_dwPlayerID);
    KG_PROCESS_ERROR(it != m_Roster.end());

    pMember = &it->second;

    bRetCode = AddFund(pMember->nSalary);
    KGLOG_PROCESS_ERROR(bRetCode);
    pMember->nSalary = 0;

    m_Roster.erase(it);
    pRole->m_dwTongID = ERROR_ID;

    m_bUpdateFlag = true;

    bResult = true;
Exit0:
    return bResult;
}

void KTong::Disband()
{
    KRole*  pRole       = NULL;

    for (KTong::KROSTER::iterator it = m_Roster.begin(); it != m_Roster.end(); ++it)
    {
        pRole = g_pSO3GameCenter->m_RoleManager.GetRole(it->first);
        if (pRole)
        {
            pRole->m_dwTongID = ERROR_ID;
            g_pSO3GameCenter->m_GameServer.DoChangePlayerTongNotify(pRole->m_nConnIndex, pRole->m_dwPlayerID, ERROR_ID, ctrDisband, m_szName);
        }
    }

    g_pSO3GameCenter->m_MiscDB.DoRemoveTongData(m_dwID);
}

BOOL KTong::CanBaseOperate(int nGroup, int nOperation)
{
    BOOL    bResult     = false;
    int     nMaskOffset = 0;

    assert(nGroup >= 0 && nGroup < MAX_TONG_GROUP);
    assert(nOperation > totInvalid && nOperation < totTotal);

    nMaskOffset = g_pSO3GameCenter->m_TongManager.m_nOperationIndex[nOperation];
    KGLOG_PROCESS_ERROR(nMaskOffset >= 0 && nMaskOffset < TONG_OPERATION_NUM);

    bResult = m_Schema.Data[nGroup].byBaseOperationMask & (1 << nMaskOffset);
Exit0:
    return bResult;
}

BOOL KTong::CanAdvanceOperate(int nGroup, int nTargetGroup, int nOperation)
{
    BOOL    bResult     = false;
    int     nMaskOffset = 0;

    assert(nGroup >= 0 && nGroup < MAX_TONG_GROUP);
    assert(nTargetGroup >= 0 && nTargetGroup < MAX_TONG_GROUP);
    assert(nOperation > totInvalid && nOperation < totTotal);

    nMaskOffset = g_pSO3GameCenter->m_TongManager.m_nOperationIndex[nOperation];
    KGLOG_PROCESS_ERROR(nMaskOffset >= 0 && nMaskOffset < TONG_OPERATION_NUM);

    bResult = m_Schema.Data[nGroup].byAdvanceOperationMask[nTargetGroup] & (1 << nMaskOffset);
Exit0:
    return bResult;
}

BOOL KTong::CanBaseGrant(int nGroup, int nTargetGroup, int nMaskOffset)
{
    BOOL    bResult     = false;

    assert(nGroup >= 0 && nGroup < MAX_TONG_GROUP);
    assert(nTargetGroup >= 0 && nTargetGroup < MAX_TONG_GROUP);

    KGLOG_PROCESS_ERROR(nMaskOffset >= 0 && nMaskOffset < TONG_OPERATION_NUM);

    bResult = m_Schema.Data[nGroup].byBaseGrantMask[nTargetGroup] & (1 << nMaskOffset);
Exit0:
    return bResult;
}

BOOL KTong::CanAdvanceGrant(int nGroup, int nTargetGroupA, int nTargetGroupB, int nMaskOffset)
{
    BOOL    bResult     = false;

    assert(nGroup >= 0 && nGroup < MAX_TONG_GROUP);
    assert(nTargetGroupA >= 0 && nTargetGroupA < MAX_TONG_GROUP);
    assert(nTargetGroupB >= 0 && nTargetGroupB < MAX_TONG_GROUP);

    KGLOG_PROCESS_ERROR(nMaskOffset >= 0 && nMaskOffset < TONG_OPERATION_NUM);

    bResult = m_Schema.Data[nGroup].byAdvanceGrantMask[nTargetGroupA][nTargetGroupB] & (1 << nMaskOffset);
Exit0:
    return bResult;
}

BOOL KTong::CanSpeak(DWORD dwMemberID)
{
    BOOL            bResult     = false;
    BOOL            bRetCode    = false;
    int             nMaskOffset = 0;
    KTongMember*    pMember     = NULL;

    pMember = GetMember(dwMemberID);
    KGLOG_PROCESS_ERROR(pMember);

    bRetCode = CanBaseOperate(pMember->nGroupID, totSpeak);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTong::ChangeCamp(KCAMP eCamp)
{
    BOOL bResult = false;

    KGLOG_PROCESS_ERROR(eCamp == cGood || eCamp == cEvil);
    KG_PROCESS_ERROR(m_eCamp == cNeutral);

    m_eCamp = eCamp;
    m_nLastUpdateFrameTable[tdtBaseInfo] = g_pSO3GameCenter->m_nWorkLoop;
    
    m_bUpdateFlag = true;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTong::PaySalary(DWORD dwMemberID, int nTotalSalary)
{
    BOOL            bResult     = false;
    BOOL            bRetCode    = false;
    KTongMember*    pMember     = NULL;
    KTongMember*    pDstMember  = NULL;
    int             nLeftMoney  = 0;
    int             nSilver     = 0;
    
    pMember = GetMember(dwMemberID);
    KGLOG_PROCESS_ERROR(pMember);

    bRetCode = CanBaseOperate(pMember->nGroupID, totPaySalary);
    KGLOG_PROCESS_ERROR(bRetCode);

    nLeftMoney = m_nFund;
    for (KROSTER::iterator it = m_Roster.begin(); it != m_Roster.end(); ++it)
    {
        pDstMember = &it->second;
        nLeftMoney -= m_Schema.Data[pDstMember->nGroupID].nWage;
        KGLOG_PROCESS_ERROR(nLeftMoney >= 0);
    }

    KGLOG_PROCESS_ERROR(m_nFund - nTotalSalary == nLeftMoney);
    KGLOG_PROCESS_ERROR(nTotalSalary <= m_nFund * (m_nMaxWageRate / 100.0));

    for (KROSTER::iterator it = m_Roster.begin(); it != m_Roster.end(); ++it)
    {
        pDstMember = &it->second;
        nSilver = m_Schema.Data[pDstMember->nGroupID].nWage;
        KGLOG_PROCESS_ERROR(nSilver >= 0);

        if (nSilver > INT_MAX - pDstMember->nSalary)
            nSilver = INT_MAX - pDstMember->nSalary;

        assert(m_nFund >= nSilver);
        pDstMember->nSalary += nSilver;
        m_nFund -= nSilver;
    }

    bRetCode = m_History.RecordPaySalary(dwMemberID, nTotalSalary);
    KGLOG_CHECK_ERROR(bRetCode);

    m_nLastUpdateFrameTable[tdtDevelopmentInfo] = g_pSO3GameCenter->m_nWorkLoop;
    m_bUpdateFlag = true;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTong::AddFund(int nSilver)
{
    BOOL bResult = false;

    if (nSilver >= 0)
    {
        if (m_nFund > MAX_TONG_FUND_BY_SILVER -  nSilver)
        {
            m_nFund = MAX_TONG_FUND_BY_SILVER;
        }
        else
        {
            m_nFund += nSilver;
        }
    }
    else
    {
        KGLOG_PROCESS_ERROR(m_nFund >= -nSilver);
        m_nFund += nSilver;
    }

    m_nLastUpdateFrameTable[tdtDevelopmentInfo] = g_pSO3GameCenter->m_nWorkLoop;
    m_bUpdateFlag = true;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTong::LoadBaseInfoVersion1(BYTE* pbyData, size_t uDataLen)
{
    BOOL                        bResult     = false;
    KTONG_DB_BASEINFO_VERSION1* pBaseInfo   = NULL;

    assert(pbyData);
    KGLOG_PROCESS_ERROR(uDataLen == sizeof(KTONG_DB_BASEINFO_VERSION1));

    pBaseInfo           = (KTONG_DB_BASEINFO_VERSION1*)pbyData;

    m_dwMaster          = pBaseInfo->dwMaster;
    m_nFund             = pBaseInfo->nFund;
    m_nLevel            = pBaseInfo->nLevel;
    m_nDevelopmentPoint = pBaseInfo->nDevelopmentPoint;
    m_nMaxMemberCount   = (int)pBaseInfo->wMaxMemberCount;
    m_eState            = (KTONG_STATE)pBaseInfo->byTongState;
    m_nStateTimer       = pBaseInfo->nStateTimer;
    m_eCamp             = (KCAMP)pBaseInfo->byCamp;

    strncpy(m_szName, pBaseInfo->szName, sizeof(m_szName));
    m_szName[sizeof(m_szName) - 1] = '\0';

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTong::LoadBaseInfo(BYTE* pbyData, size_t uDataLen)
{
    BOOL                bResult     = false;
    KTONG_DB_BASEINFO*  pBaseInfo   = NULL;

    assert(pbyData);
    KGLOG_PROCESS_ERROR(uDataLen == sizeof(KTONG_DB_BASEINFO));

    pBaseInfo           = (KTONG_DB_BASEINFO*)pbyData;

    m_dwMaster          = pBaseInfo->dwMaster;
    m_nLevel            = pBaseInfo->nLevel;
    m_nMaxMemberCount   = (int)pBaseInfo->wMaxMemberCount;
    m_eState            = (KTONG_STATE)pBaseInfo->byTongState;
    m_nStateTimer       = pBaseInfo->nStateTimer;
    m_eCamp             = (KCAMP)pBaseInfo->byCamp;

    strncpy(m_szName, pBaseInfo->szName, sizeof(m_szName));
    m_szName[sizeof(m_szName) - 1] = '\0';

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTong::SaveBaseInfo(BYTE* pbyBuffer, size_t uBufferSize, size_t* puUsedSize)
{
    BOOL                bResult     = false;
    KTONG_DB_BASEINFO*  pBaseInfo   = NULL;

    assert(pbyBuffer);
    assert(puUsedSize);
    KGLOG_PROCESS_ERROR(uBufferSize >= sizeof(KTONG_DB_BASEINFO));

    pBaseInfo = (KTONG_DB_BASEINFO*)pbyBuffer;

    pBaseInfo->dwMaster             = m_dwMaster;
    pBaseInfo->nLevel               = m_nLevel;
    pBaseInfo->wMaxMemberCount      = (WORD)m_nMaxMemberCount;
    pBaseInfo->byTongState          = (BYTE)m_eState;
    pBaseInfo->nStateTimer          = m_nStateTimer;
    pBaseInfo->byCamp               = (BYTE)m_eCamp;

    strncpy(pBaseInfo->szName, m_szName, sizeof(pBaseInfo->szName));
    pBaseInfo->szName[sizeof(pBaseInfo->szName) - 1] = '\0';

    *puUsedSize = sizeof(KTONG_DB_BASEINFO);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTong::LoadSchema(BYTE* pbyData, size_t uDataLen)
{
    BOOL                bResult         = false;
    KTONG_DB_SCHEMA*    pSchemaSection  = NULL;
    
    assert(pbyData);
    KGLOG_PROCESS_ERROR(uDataLen == sizeof(KTONG_DB_SCHEMA));

    pSchemaSection = (KTONG_DB_SCHEMA*)pbyData;

    memcpy(&m_Schema, pSchemaSection, sizeof(KTongSchemaData));

    for (int nGroupIndex = 0; nGroupIndex < MAX_TONG_GROUP; nGroupIndex++)
    {
        m_Schema.Data[nGroupIndex].szName[sizeof(m_Schema.Data[nGroupIndex].szName) - 1] = '\0';
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTong::SaveSchema(BYTE* pbyBuffer, size_t uBufferSize, size_t* puUsedSize)
{
    BOOL                bResult         = false;
    KTONG_DB_SCHEMA*    pSchemaSection  = NULL;

    assert(pbyBuffer);
    assert(puUsedSize);
    KGLOG_PROCESS_ERROR(uBufferSize >= sizeof(KTONG_DB_SCHEMA));

    pSchemaSection = (KTONG_DB_SCHEMA*)pbyBuffer;

    memcpy(&pSchemaSection->Schema, &m_Schema, sizeof(pSchemaSection->Schema));

    *puUsedSize = sizeof(KTONG_DB_SCHEMA);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTong::LoadRosterVersion1(BYTE* pbyData, size_t uDataLen)
{
    BOOL                            bResult         = false;
    KTONG_DB_ROSTER*                pRosterSection  = NULL;
    int                             nMemberCount    = 0;
    KTONG_DB_MEMBER_DATA_VERSION1*  pMemberData     = NULL;
    KRole*                          pRole           = NULL;

    assert(pbyData);
    KGLOG_PROCESS_ERROR(uDataLen >= sizeof(KTONG_DB_ROSTER));

    pRosterSection = (KTONG_DB_ROSTER*)pbyData;
    nMemberCount = pRosterSection->nMemberCount;

    KGLOG_PROCESS_ERROR(nMemberCount >= 0);
    KGLOG_PROCESS_ERROR(uDataLen == sizeof(KTONG_DB_ROSTER) + sizeof(KTONG_DB_MEMBER_DATA_VERSION1) * nMemberCount);
    
    pMemberData = (KTONG_DB_MEMBER_DATA_VERSION1*)pRosterSection->byData;

    for (int i = 0; i < nMemberCount; i++)
    {
        KTongMember Member;

        pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pMemberData[i].dwID);
        if (pRole == NULL)
        {
            KGLogPrintf(
                KGLOG_INFO, "[Tong] Tong member %u not exist, tong: (%u, %s)\n",
                pMemberData[i].dwID, m_dwID, m_szName
            );
            continue;
        }

        pRole->m_dwTongID       = m_dwID;
        Member.dwID             = pMemberData[i].dwID;
        Member.nGroupID         = pMemberData[i].nGroupID;
        Member.nJoinTime        = pMemberData[i].nJoinTime;
        Member.nLastOfflineTime = pMemberData[i].nLastOfflineTime;
        Member.nSalary          = 0;
        Member.nLevel           = pRole->m_byLevel;
        Member.dwForceID        = pRole->m_dwForceID;
        Member.dwMapID          = pRole->m_dwMapID;
        Member.bIsOnline        = false;
        
        Member.nLastUpdateFrame = 0;

        strncpy(Member.szRemark, pMemberData[i].szRemark, sizeof(Member.szRemark));
        Member.szRemark[sizeof(Member.szRemark) - 1] = '\0';

        m_Roster[Member.dwID] = Member;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTong::LoadRoster(BYTE* pbyData, size_t uDataLen)
{
    BOOL                    bResult         = false;
    KTONG_DB_ROSTER*        pRosterSection  = NULL;
    int                     nMemberCount    = 0;
    KTONG_DB_MEMBER_DATA*   pMemberData     = NULL;
    KRole*                  pRole           = NULL;

    assert(pbyData);
    KGLOG_PROCESS_ERROR(uDataLen >= sizeof(KTONG_DB_ROSTER));

    pRosterSection = (KTONG_DB_ROSTER*)pbyData;
    nMemberCount = pRosterSection->nMemberCount;

    KGLOG_PROCESS_ERROR(nMemberCount >= 0);
    KGLOG_PROCESS_ERROR(uDataLen == sizeof(KTONG_DB_ROSTER) + sizeof(KTONG_DB_MEMBER_DATA) * nMemberCount);
    
    pMemberData = (KTONG_DB_MEMBER_DATA*)pRosterSection->byData;

    for (int i = 0; i < nMemberCount; i++)
    {
        KTongMember Member;

        pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pMemberData[i].dwID);
        if (pRole == NULL)
        {
            KGLogPrintf(
                KGLOG_INFO, "[Tong] Tong member %u not exist, tong: (%u, %s)\n",
                pMemberData[i].dwID, m_dwID, m_szName
            );
            continue;
        }

        pRole->m_dwTongID       = m_dwID;

        Member.dwID             = pMemberData[i].dwID;
        Member.nGroupID         = pMemberData[i].nGroupID;
        Member.nJoinTime        = pMemberData[i].nJoinTime;
        Member.nLastOfflineTime = pMemberData[i].nLastOfflineTime;
        Member.nSalary          = pMemberData[i].nSalary;

        Member.nLevel           = pRole->m_byLevel;
        Member.dwForceID        = pRole->m_dwForceID;
        Member.dwMapID          = pRole->m_dwMapID;
        Member.bIsOnline        = false;
        
        Member.nLastUpdateFrame = 0;

        strncpy(Member.szRemark, pMemberData[i].szRemark, sizeof(Member.szRemark));
        Member.szRemark[sizeof(Member.szRemark) - 1] = '\0';

        m_Roster[Member.dwID] = Member;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTong::SaveRoster(BYTE* pbyBuffer, size_t uBufferSize, size_t* puUsedSize)
{
    BOOL                    bResult     = false;
    KTONG_DB_ROSTER*        pRosterData = NULL;
    BYTE*                   pbyOffset   = NULL;

    assert(pbyBuffer);
    assert(puUsedSize);
    KGLOG_PROCESS_ERROR(uBufferSize >= sizeof(KTONG_DB_ROSTER) + sizeof(KTONG_DB_MEMBER_DATA) * m_Roster.size());
    
    pRosterData = (KTONG_DB_ROSTER*)pbyBuffer; 
    
    pRosterData->nMemberCount = (int)m_Roster.size();

    pbyOffset = pRosterData->byData;

    for(KROSTER::iterator Iter = m_Roster.begin(); Iter != m_Roster.end(); ++Iter)
    {
        KTONG_DB_MEMBER_DATA*   pData       = (KTONG_DB_MEMBER_DATA*)pbyOffset;
        const KTongMember&      crMember    = Iter->second;

        pData->dwID             = crMember.dwID;
        pData->nGroupID         = crMember.nGroupID;
        pData->nJoinTime        = crMember.nJoinTime;
        pData->nLastOfflineTime = crMember.nLastOfflineTime;
        pData->nSalary          = crMember.nSalary;

        strncpy(pData->szRemark, crMember.szRemark, sizeof(pData->szRemark));
        pData->szRemark[sizeof(pData->szRemark) - 1] = '\0';

        pbyOffset += sizeof(KTONG_DB_MEMBER_DATA);
    }

    *puUsedSize =  sizeof(KTONG_DB_ROSTER) + sizeof(KTONG_DB_MEMBER_DATA) * m_Roster.size();

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTong::LoadMemorabilia(BYTE* pbyData, size_t uDataLen)
{
    BOOL                    bResult         = false;
    KTONG_DB_MEMORABILIA*   pMemorabilia    = NULL;
    BYTE*                   pbyOffset       = pbyData;
    size_t                  uLeftSize       = uDataLen;
    DWORD                   dwRecordCount   = 0;
    size_t                  uRecordSize     = 0;
    KTongIncidentRecord     Record;

    assert(pbyData);
    KGLOG_PROCESS_ERROR(m_Memorabilia.size() == 0);

    while(uLeftSize > 0)
    {
        KGLOG_PROCESS_ERROR(uLeftSize > sizeof(KTONG_DB_MEMORABILIA));
        pMemorabilia = (KTONG_DB_MEMORABILIA*)pbyOffset;

        uRecordSize = sizeof(KTONG_DB_MEMORABILIA) + pMemorabilia->byDataLen;
        KGLOG_PROCESS_ERROR(uLeftSize >= uRecordSize);

        Record.dwID     = ++dwRecordCount;
        Record.nTime    = pMemorabilia->nTime;

        strncpy(Record.szDescription, (const char*)pMemorabilia->byData, sizeof(Record.szDescription));
        Record.szDescription[sizeof(Record.szDescription) -1] = '\0';

        m_Memorabilia.push_back(Record);

        uLeftSize -= uRecordSize;
        pbyOffset += uRecordSize;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTong::SaveMemorabilia(BYTE* pbyBuffer, size_t uBufferSize, size_t* puUsedSize)
{
    BOOL                    bResult         = false;
    BYTE*                   pbyOffset       = pbyBuffer;
    KTONG_DB_MEMORABILIA*   pMemorabilia    = NULL;
    size_t                  uStrLen         = 0;
    size_t                  uRecordSize     = 0;
    size_t                  uLeftsize       = uBufferSize;

    assert(pbyBuffer);
    assert(puUsedSize);

    for (int i = 0 ; i < (int)m_Memorabilia.size(); i++)
    {
        KGLOG_PROCESS_ERROR(uLeftsize >= sizeof(KTONG_DB_MEMORABILIA) + TONG_MEMORABILIA_RECORD_LEN);

        pMemorabilia = (KTONG_DB_MEMORABILIA*)pbyOffset;
        pMemorabilia->nTime = m_Memorabilia[i].nTime;

        uStrLen = strlen(m_Memorabilia[i].szDescription) + 1;
        KGLOG_PROCESS_ERROR(uStrLen <= TONG_MEMORABILIA_RECORD_LEN);

        memcpy(pMemorabilia->byData, m_Memorabilia[i].szDescription, uStrLen);

        pMemorabilia->byDataLen = (BYTE)uStrLen;
        uRecordSize = sizeof(KTONG_DB_MEMORABILIA) + uStrLen;
        
        uLeftsize -= uRecordSize;
        pbyOffset += uRecordSize;
    }

    *puUsedSize = uBufferSize - uLeftsize;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTong::LoadDevelopmentInfo(BYTE* pbyData, size_t uDataLen)
{
    BOOL                        bResult     = false;
    KTONG_DB_DEVELOPMENT_DATA*  pData       = NULL;

    assert(pbyData);
    KGLOG_PROCESS_ERROR(uDataLen == sizeof(KTONG_DB_DEVELOPMENT_DATA));
    pData = (KTONG_DB_DEVELOPMENT_DATA*)pbyData;

    m_nFund                 = pData->nFund;
    m_nMaxWageRate          = pData->nMaxWageRate;
    m_nDevelopmentPoint     = pData->nDevelopmentPoint;
    m_nUsedDevelopmentPoint = pData->nUsedDevelopmentPoint;

    KGLOG_PROCESS_ERROR(sizeof(m_byTechTree) == sizeof(pData->byTechTree));
    memcpy(m_byTechTree, pData->byTechTree, sizeof(m_byTechTree));

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTong::SaveDevelopmentInfo(BYTE* pbyBuffer, size_t uBufferSize, size_t* puUsedSize)
{
    BOOL                        bResult = false;
    KTONG_DB_DEVELOPMENT_DATA*  pData   = NULL;

    assert(pbyBuffer);
    assert(puUsedSize);
    KGLOG_PROCESS_ERROR(uBufferSize >= sizeof(KTONG_DB_DEVELOPMENT_DATA));

    pData = (KTONG_DB_DEVELOPMENT_DATA*)pbyBuffer;
    pData->nFund                = m_nFund;
    pData->nMaxWageRate         = m_nMaxWageRate;
    pData->nDevelopmentPoint    = m_nDevelopmentPoint;
    pData->nUsedDevelopmentPoint = m_nUsedDevelopmentPoint;
    
    assert(sizeof(pData->byTechTree) == sizeof(m_byTechTree));
    memcpy(pData->byTechTree, m_byTechTree, sizeof(pData->byTechTree));
    
    *puUsedSize = sizeof(KTONG_DB_DEVELOPMENT_DATA);

    bResult = true;
Exit0:
    return bResult;
}
