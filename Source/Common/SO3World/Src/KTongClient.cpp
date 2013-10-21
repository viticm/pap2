#include "stdafx.h"

#if defined(_CLIENT)
#include "StrMap.h"
#include "KMath.h"
#include "KGPublic.h"
#include "KSO3World.h"
#include "KPlayerClient.h"
#include "SO3UI/SO3GameWorldUIHandler.h"
#include "SO3Represent/SO3GameWorldRepresentHandler.h"
#include "KTongClient.h"
#include "KPlayer.h"

#define REGISTER_SYNC_HISTORY_RECORD(RecordType, RecordDataStruct)  \
{                                                                   \
    s_uSyncRecordSize[RecordType] = sizeof(RecordDataStruct);       \
}
static size_t s_uSyncRecordSize[hrtTotal];

KTongClient::KTongClient()
{
    m_nLastRosterUpdateFrame    = -1;           // 尚未同步时的初始版本号, Center的初始版本号为0
    m_nLastInfoUpdateFrame      = -1;

    m_eState                    = tsInvalid;
    m_nStateTimer               = 0;

    m_dwMaster                  = ERROR_ID;
    m_nFund                     = 0;
    m_nLevel                    = 0;    
    m_nDevelopmentPoint         = 0;
    m_nMaxMemberCount           = 0;

    m_szTongName[0]             = '\0';
    m_szAnnouncement[0]         = '\0';
    m_szOnlineMessage[0]        = '\0';
    m_szIntroduction[0]         = '\0';
    m_szRules[0]                = '\0';

    memset(m_byTechTree, 0, sizeof(m_byTechTree));
    
    for (int i = 0; i < (int)sizeof(m_nOperationIndex)/sizeof(int); i++)
        m_nOperationIndex[i] = INVALID_TONG_OPERATION;
}

BOOL KTongClient::Init()
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    bRetCode = LoadOperationConfig();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = m_RepertoryCache.Init();
    KGLOG_PROCESS_ERROR(bRetCode);

    memset(s_uSyncRecordSize, 0, sizeof(s_uSyncRecordSize));

    REGISTER_SYNC_HISTORY_RECORD(hrtChangeGroupBaseRight, KTONG_SYNC_HISTORY_CHANGE_GROUP_BASE_RIGHT);
    REGISTER_SYNC_HISTORY_RECORD(hrtChangeGroupAdvanceRight, KTONG_SYNC_HISTORY_CHANGE_GROUP_ADVANCE_RIGHT);
    REGISTER_SYNC_HISTORY_RECORD(hrtChangeMemberGroup, KTONG_SYNC_HISTORY_CHANGE_MEMBER_GROUP);
    REGISTER_SYNC_HISTORY_RECORD(hrtJoin, KTONG_SYNC_HISTORY_JOIN_OR_QUIT);
    REGISTER_SYNC_HISTORY_RECORD(hrtQuit, KTONG_SYNC_HISTORY_JOIN_OR_QUIT);
    REGISTER_SYNC_HISTORY_RECORD(hrtKickOut, KTONG_SYNC_HISTORY_KICK_OUT_MEMBER);

    REGISTER_SYNC_HISTORY_RECORD(hrtChangeGroupWage, KTONG_SYNC_HISTORY_CHANGE_GROUP_WAGE);
    REGISTER_SYNC_HISTORY_RECORD(hrtPaySalary, KTONG_SYNC_HISTORY_PAY_SALARY);
    REGISTER_SYNC_HISTORY_RECORD(hrtSaveFund, KTONG_SYNC_HISTORY_SAVE_FUND);

    REGISTER_SYNC_HISTORY_RECORD(hrtActivateTechtree, KTONG_SYNC_HISTORY_ACTIVATE_TECHTREE);
    REGISTER_SYNC_HISTORY_RECORD(hrtProductItem, KTONG_SYNC_HISTORY_PRODUCT_ITEM);

    REGISTER_SYNC_HISTORY_RECORD(hrtMissionStart, KTONG_SYNC_HISTORY_MISSION_START);
    REGISTER_SYNC_HISTORY_RECORD(hrtMissionComplete, KTONG_SYNC_HISTORY_MISSION_END);
    REGISTER_SYNC_HISTORY_RECORD(hrtMissionFailed, KTONG_SYNC_HISTORY_MISSION_END);
    REGISTER_SYNC_HISTORY_RECORD(hrtWarStart, KTONG_SYNC_HISTORY_WAR_START);
    REGISTER_SYNC_HISTORY_RECORD(hrtWarWin, KTONG_SYNC_HISTORY_WAR_END);
    REGISTER_SYNC_HISTORY_RECORD(hrtWarLose, KTONG_SYNC_HISTORY_WAR_END);
    REGISTER_SYNC_HISTORY_RECORD(hrtAchievement, KTONG_SYNC_HISTORY_ACHIEVEMENT);

    REGISTER_SYNC_HISTORY_RECORD(hrtRepertoryTakeItem, KTONG_SYNC_HISTORY_REPERTORY_TAKE_OR_PUT_ITEM);
    REGISTER_SYNC_HISTORY_RECORD(hrtRepertoryPutItem, KTONG_SYNC_HISTORY_REPERTORY_TAKE_OR_PUT_ITEM);

    bResult = true;

Exit0:
    return bResult;
}

void KTongClient::UnInit()
{
    m_RepertoryCache.UnInit();

    for (int nType = 0 ; nType < ohtTotal; nType++)
    {
        KTONG_HISTORY_CLIENT_TABLE* pHistory = &m_History[nType];
        for(KTONG_HISTORY_CLIENT_TABLE::iterator it = pHistory->begin(); it != pHistory->end(); it++)
        {
            KMemory::Free(it->second);
            it->second = NULL;
        }
        pHistory->clear();
    }
}

void KTongClient::Reset()
{
    m_nLastRosterUpdateFrame    = -1;
    m_nLastInfoUpdateFrame      = -1;

    m_eState                    = tsInvalid;
    m_nStateTimer               = 0;

    m_dwMaster                  = ERROR_ID;
    m_nFund                     = 0;
    m_nLevel                    = 0;
    m_nDevelopmentPoint         = 0;
    m_nMaxMemberCount           = 0;

    m_szTongName[0]             = '\0';
    m_szAnnouncement[0]         = '\0';
    m_szOnlineMessage[0]        = '\0';
    m_szIntroduction[0]         = '\0';
    m_szRules[0]                = '\0';

    memset(m_byTechTree, 0, sizeof(m_byTechTree));

    m_Roster.clear();
    m_Memorabilia.clear();

    m_RepertoryCache.Reset();

    for (int nType = 0 ; nType < ohtTotal; nType++)
    {
        KTONG_HISTORY_CLIENT_TABLE* pHistory = &m_History[nType];
        for(KTONG_HISTORY_CLIENT_TABLE::iterator it = pHistory->begin(); it != pHistory->end(); it++)
        {
            KMemory::Free(it->second);
            it->second = NULL;
        }
        pHistory->clear();
    }
}

BOOL KTongClient::UpdateBaseInfo(BYTE* pbyData, size_t uDataLen)
{
    BOOL                bResult     = false;
    KTONG_BASE_INFO*    pBaseInfo   = (KTONG_BASE_INFO*)pbyData;

    assert(pBaseInfo);
    KGLOG_PROCESS_ERROR(uDataLen == sizeof(KTONG_BASE_INFO));

    m_dwMaster          = pBaseInfo->dwMasterID;
    m_nLevel            = (int)pBaseInfo->byLevel;
    m_nMaxMemberCount   = pBaseInfo->nMaxMemberCount;
    m_eCamp             = (KCAMP)pBaseInfo->byCamp;

    KGLOG_PROCESS_ERROR(pBaseInfo->byState > tsInvalid && pBaseInfo->byState < tsTotal);
    m_eState            = (KTONG_STATE)pBaseInfo->byState;
    m_nStateTimer       = g_pSO3World->m_nCurrentTime + pBaseInfo->nStateTimerOffset;

    strncpy(m_szTongName, pBaseInfo->szTongName, sizeof(m_szTongName));
    m_szTongName[sizeof(m_szTongName) - 1] = '\0';
    
    bResult = true;
Exit0:
    return bResult;
}

BOOL KTongClient::UpdateDevelopmentInfo(BYTE* pbyData, size_t uDataLen)
{
    BOOL                        bResult             = false;
    KTONG_DEVELOPMENT_INFO*     pDevelopmentInfo    = (KTONG_DEVELOPMENT_INFO*)pbyData;

    assert(pDevelopmentInfo);
    KGLOG_PROCESS_ERROR(uDataLen == sizeof(KTONG_DEVELOPMENT_INFO));

    m_nFund                 = pDevelopmentInfo->nFound;
    m_nMaxWageRate          = pDevelopmentInfo->nMaxWageRate;
    m_nDevelopmentPoint     = pDevelopmentInfo->nDevelopmentPoint;
    m_nUsedDevelopmentPoint = pDevelopmentInfo->nUsedDevelopmentPoint;
    memcpy(m_byTechTree, pDevelopmentInfo->m_byTechTree, sizeof(m_byTechTree));

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTongClient::UpdateAnnouncement(BYTE* pbyData, size_t uDataLen)
{
    BOOL                bResult     = false;
    KTONG_TEXT_INFO*    pTextInfo = (KTONG_TEXT_INFO*)pbyData;

    assert(pTextInfo);
    KGLOG_PROCESS_ERROR(uDataLen == sizeof(KTONG_TEXT_INFO) + pTextInfo->uDataLen);

    strncpy(m_szAnnouncement, (char*)pTextInfo->byContent, sizeof(m_szAnnouncement));
    m_szAnnouncement[sizeof(m_szAnnouncement) - 1] = '\0'; 

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTongClient::UpdateOnlineMessage(BYTE* pbyData, size_t uDataLen)
{
    BOOL                bResult     = false;
    KTONG_TEXT_INFO*    pTextInfo = (KTONG_TEXT_INFO*)pbyData;

    assert(pTextInfo);
    KGLOG_PROCESS_ERROR(uDataLen == sizeof(KTONG_TEXT_INFO) + pTextInfo->uDataLen);

    strncpy(m_szOnlineMessage, (char*)pTextInfo->byContent, sizeof(m_szOnlineMessage));
    m_szOnlineMessage[sizeof(m_szOnlineMessage) - 1] = '\0'; 

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTongClient::UpdateIntroduction(BYTE* pbyData, size_t uDataLen)
{
    BOOL                bResult     = false;
    KTONG_TEXT_INFO*    pTextInfo = (KTONG_TEXT_INFO*)pbyData;

    assert(pTextInfo);
    KGLOG_PROCESS_ERROR(uDataLen == sizeof(KTONG_TEXT_INFO) + pTextInfo->uDataLen);

    strncpy(m_szIntroduction, (char*)pTextInfo->byContent, sizeof(m_szIntroduction));
    m_szIntroduction[sizeof(m_szIntroduction) - 1] = '\0'; 

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTongClient::UpdateRules(BYTE* pbyData, size_t uDataLen)
{
    BOOL                bResult     = false;
    KTONG_TEXT_INFO*    pTextInfo = (KTONG_TEXT_INFO*)pbyData;

    assert(pTextInfo);
    KGLOG_PROCESS_ERROR(uDataLen == sizeof(KTONG_TEXT_INFO) + pTextInfo->uDataLen);

    strncpy(m_szRules, (char*)pTextInfo->byContent, sizeof(m_szRules));
    m_szRules[sizeof(m_szRules) - 1] = '\0'; 

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTongClient::UpdateSchemaInfo(BYTE* pbyData, size_t uDataLen)
{
    BOOL                        bResult     = false;
    int                         nGroupIndex = 0;
    KTONG_SCHEMA_DATA_INFO*     pSchemaInfo = (KTONG_SCHEMA_DATA_INFO*)pbyData;

    assert(pSchemaInfo);
    KGLOG_PROCESS_ERROR(uDataLen == sizeof(KTONG_SCHEMA_DATA_INFO));

    memcpy(&m_Schema, &pSchemaInfo->SchemaData, sizeof(KTongSchemaData));

    for (int i = 0; i < MAX_TONG_GROUP; i++)
    {
        m_Schema.Data[i].szName[sizeof(m_Schema.Data[i].szName) - 1] = '\0';
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTongClient::UpdateMemorabilia(KTongIncidentRecord* pRecord, unsigned uCount)
{
    BOOL bResult = false;

    assert(pRecord);

    if (m_Memorabilia.size() != uCount)
    {
        m_Memorabilia.resize(uCount);
    }

    if (uCount > 0)
    {    
        memcpy(&m_Memorabilia[0], pRecord, sizeof(KTongIncidentRecord) * uCount);
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTongClient::UpdateMemberInfo(const KTongMemberInfo& crTongMemberInfo)
{
    BOOL bResult = false;

    KGLOG_PROCESS_ERROR(crTongMemberInfo.dwID);

    m_Roster[crTongMemberInfo.dwID] = crTongMemberInfo;
    
    if (g_pGameWorldUIHandler)
    {
        g_pGameWorldUIHandler->OnUpdateTongRoster();
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTongClient::DeleteMember(DWORD dwMemberID)
{
    m_Roster.erase(dwMemberID);

    if (g_pGameWorldUIHandler)
    {
        g_pGameWorldUIHandler->OnUpdateTongRoster();
    }

    return true;
}

static void CheckHistoryData(KTONG_SYNC_HISTORY* pRecord)
{
    assert(pRecord);

    switch (pRecord->byRecordType)
    {
    case hrtChangeGroupBaseRight:
        {
            KTONG_SYNC_HISTORY_CHANGE_GROUP_BASE_RIGHT* pInfo = (KTONG_SYNC_HISTORY_CHANGE_GROUP_BASE_RIGHT*)pRecord;
            KGLOG_PROCESS_ERROR(pInfo->byTargetGroup < MAX_TONG_GROUP);
            KGLOG_PROCESS_ERROR(pInfo->byRightOffset < TONG_OPERATION_NUM);

            pInfo->szOperator[sizeof(pInfo->szOperator) -1 ] = '\0';
        }
        break;
    case hrtChangeGroupAdvanceRight:
        {
            KTONG_SYNC_HISTORY_CHANGE_GROUP_ADVANCE_RIGHT* pInfo = (KTONG_SYNC_HISTORY_CHANGE_GROUP_ADVANCE_RIGHT*)pRecord;
            KGLOG_PROCESS_ERROR(pInfo->bySrcGroup < MAX_TONG_GROUP);
            KGLOG_PROCESS_ERROR(pInfo->byDstGroup < MAX_TONG_GROUP);
            KGLOG_PROCESS_ERROR(pInfo->byRightOffset < TONG_OPERATION_NUM);

            pInfo->szOperator[sizeof(pInfo->szOperator) -1 ] = '\0';
        }
        break;
    case hrtChangeMemberGroup:
        {
            KTONG_SYNC_HISTORY_CHANGE_MEMBER_GROUP* pInfo = (KTONG_SYNC_HISTORY_CHANGE_MEMBER_GROUP*)pRecord;
            KGLOG_PROCESS_ERROR(pInfo->bySrcGroup < MAX_TONG_GROUP);
            KGLOG_PROCESS_ERROR(pInfo->byDstGroup < MAX_TONG_GROUP);

            pInfo->szOperator[sizeof(pInfo->szOperator) -1 ] = '\0';
            pInfo->szTargetMember[sizeof(pInfo->szTargetMember) - 1] = '\0';
        }
        break;
    case hrtJoin:
    case hrtQuit:
        {
            KTONG_SYNC_HISTORY_JOIN_OR_QUIT* pInfo = (KTONG_SYNC_HISTORY_JOIN_OR_QUIT*)pRecord;
            pInfo->szMember[sizeof(pInfo->szMember) - 1] = '\0';
        }
        break;
    case hrtKickOut:
        {
            KTONG_SYNC_HISTORY_KICK_OUT_MEMBER* pInfo = (KTONG_SYNC_HISTORY_KICK_OUT_MEMBER*)pRecord;

            pInfo->szOperator[sizeof(pInfo->szOperator) -1 ] = '\0';
            pInfo->szTargetMember[sizeof(pInfo->szTargetMember) - 1] = '\0';
        }
        break;
    case hrtChangeGroupWage:
        {
            KTONG_SYNC_HISTORY_CHANGE_GROUP_WAGE* pInfo = (KTONG_SYNC_HISTORY_CHANGE_GROUP_WAGE*)pRecord;
            KGLOG_PROCESS_ERROR(pInfo->byTargetGroup < MAX_TONG_GROUP);

            pInfo->szOperator[sizeof(pInfo->szOperator) -1 ] = '\0';
        }
        break;
    case hrtPaySalary:
        {
            KTONG_SYNC_HISTORY_PAY_SALARY* pInfo = (KTONG_SYNC_HISTORY_PAY_SALARY*)pRecord;

            pInfo->szOperator[sizeof(pInfo->szOperator) -1 ] = '\0';
        }
        break;
    case hrtSaveFund:
        {
            KTONG_SYNC_HISTORY_SAVE_FUND* pInfo = (KTONG_SYNC_HISTORY_SAVE_FUND*)pRecord;

            pInfo->szOperator[sizeof(pInfo->szOperator) -1 ] = '\0';
        }
        break;
    case hrtActivateTechtree:
        {
            KTONG_SYNC_HISTORY_ACTIVATE_TECHTREE* pInfo = (KTONG_SYNC_HISTORY_ACTIVATE_TECHTREE*)pRecord;

            pInfo->szOperator[sizeof(pInfo->szOperator) -1 ] = '\0';
        }
        break;
    case hrtProductItem:
        {
            KTONG_SYNC_HISTORY_PRODUCT_ITEM* pInfo = (KTONG_SYNC_HISTORY_PRODUCT_ITEM*)pRecord;

            pInfo->szOperator[sizeof(pInfo->szOperator) -1 ] = '\0';
        }
        break;
    case hrtMissionStart:
        {
            KTONG_SYNC_HISTORY_MISSION_START* pInfo = (KTONG_SYNC_HISTORY_MISSION_START*)pRecord;

            pInfo->szOperator[sizeof(pInfo->szOperator) -1 ] = '\0';
        }
        break;
    case hrtMissionComplete:
    case hrtMissionFailed:
        {
            KTONG_SYNC_HISTORY_MISSION_END* pInfo = (KTONG_SYNC_HISTORY_MISSION_END*)pRecord;
        }    
        break;
    case hrtWarStart:
        {
            KTONG_SYNC_HISTORY_WAR_START* pInfo = (KTONG_SYNC_HISTORY_WAR_START*)pRecord;

            pInfo->szOperator[sizeof(pInfo->szOperator) - 1 ] = '\0';
            pInfo->szEnemyTong[sizeof(pInfo->szEnemyTong) - 1] = '\0';
        }
        break;
    case hrtWarWin:
    case hrtWarLose:
        {
            KTONG_SYNC_HISTORY_WAR_END* pInfo = (KTONG_SYNC_HISTORY_WAR_END*)pRecord;

            pInfo->szEnemyTong[sizeof(pInfo->szEnemyTong) - 1] = '\0';
        }
        break;
/*
    case hrtAchievement:
        {
            KTONG_SYNC_HISTORY_ACHIEVEMENT* pInfo = (KTONG_SYNC_HISTORY_ACHIEVEMENT*)pRecord;
        }
        break;
*/
    case hrtRepertoryTakeItem:
    case hrtRepertoryPutItem:
        {
            KTONG_SYNC_HISTORY_REPERTORY_TAKE_OR_PUT_ITEM* pInfo = (KTONG_SYNC_HISTORY_REPERTORY_TAKE_OR_PUT_ITEM*)pRecord;

            pInfo->szOperator[sizeof(pInfo->szOperator) - 1 ] = '\0';
        }
        break;

    default:
        KGLOG_PROCESS_ERROR(0);
    }
Exit0:
    return;
}

BOOL KTongClient::UpdateHistory(int nType, DWORD dwStartIndex, int nCount, BYTE byData[], size_t uDataLen)
{
    BOOL                        bResult     = false;
    size_t                      uRecordSize = 0;
    size_t                      uLeftSize   = uDataLen;
    DWORD                       dwIndex     = 0;
    KTONG_SYNC_HISTORY*         pRecordHead = NULL;
    KTONG_SYNC_HISTORY*         pRecordCopy = NULL;
    KTONG_HISTORY_CLIENT_TABLE* pHistory    = NULL;;
    KTONG_HISTORY_CLIENT_TABLE::const_iterator RecordIter; 
    BYTE*                       pbyOffset   = NULL;
    
    assert(byData);
    KGLOG_PROCESS_ERROR(nCount > 0);

    pbyOffset = byData;
    dwIndex = dwStartIndex;

    assert(nType >= 0 && nType < ohtTotal);
    pHistory = &m_History[nType];

    for(int i = 0; i < nCount; i++, dwIndex++)
    {
        pRecordHead = (KTONG_SYNC_HISTORY*)pbyOffset;
        KGLOG_PROCESS_ERROR(pRecordHead->byRecordType < hrtTotal);
        uRecordSize = s_uSyncRecordSize[pRecordHead->byRecordType];
        KGLOG_PROCESS_ERROR(uRecordSize > 0);

        KGLOG_PROCESS_ERROR(uLeftSize >= uRecordSize);
        pbyOffset += uRecordSize;
        uLeftSize -= uRecordSize;
        CheckHistoryData(pRecordHead);

        RecordIter = pHistory->find(dwIndex);
        if (RecordIter != pHistory->end())
            continue;

        pRecordCopy = (KTONG_SYNC_HISTORY*)KMemory::Alloc(uRecordSize);
        memcpy(pRecordCopy, pRecordHead, uRecordSize);

        pHistory->insert(make_pair(dwIndex, pRecordCopy));
        pRecordCopy = NULL;
    }

    KGLOG_PROCESS_ERROR(uLeftSize == 0);
    bResult = true;
Exit0:
    if (pRecordCopy)
    {
        KMemory::Free(pRecordCopy);
        pRecordCopy = NULL;
    }
    return bResult;
}

BOOL KTongClient::GetMemberList(DWORD* pdwMember, int nMaxCount)
{
    BOOL bResult = false;

    assert(pdwMember);

    KGLOG_PROCESS_ERROR(nMaxCount >= (int)m_Roster.size());

    for (KCLIENT_ROSTER::iterator it = m_Roster.begin(); it != m_Roster.end(); ++it)
    {
        *pdwMember++ = it->first;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTongClient::TryGetTongName(DWORD dwTongID, char* pszRetBuffer, size_t uBufferSize)
{
    BOOL                                bResult = false;
    KTONG_DESCRIPTION_TABLE::iterator   Iter;
    
    assert(pszRetBuffer);
    assert(uBufferSize);

    if (dwTongID == ERROR_ID)
    {
        pszRetBuffer[0] = '\0';
        goto Exit1;
    }

    Iter = m_TongTable.find(dwTongID);

    if (Iter == m_TongTable.end())
    {
        pszRetBuffer[0] = '\0';

        if (m_DealingTongDescription.find(dwTongID) == m_DealingTongDescription.end())
        {
            m_DealingTongDescription.insert(dwTongID);
            g_PlayerClient.DoGetTongDescriptionRequest(dwTongID);
        }
        
        goto Exit0;
    }

    strncpy(pszRetBuffer, Iter->second.szTongName, uBufferSize);
    pszRetBuffer[uBufferSize - 1] = '\0';

Exit1:        
    bResult = true;
Exit0:
    return bResult;
}

BOOL KTongClient::FlushTongNameTraverser::operator () (DWORD dwID, KPlayer* pPlayer)
{
    if (pPlayer->m_dwTongID == dwTongID)
    {
        strncpy(pPlayer->m_szTongName, pszTongName, sizeof(pPlayer->m_szTongName));
        pPlayer->m_szTongName[sizeof(pPlayer->m_szTongName) - 1] = '\0';
        
        if (g_pGameWorldRepresentHandler)
        {
            g_pGameWorldRepresentHandler->OnCharacterUpdateRelation((KCharacter*)pPlayer);
        }
    }

    return true;
}

BOOL KTongClient::OnGetTongDescrioptionRespond(DWORD dwTongID, char szTongName[])
{
    KTONG_DESCRIPTION       TongDescription;
    FlushTongNameTraverser  Traverser;

    strncpy(TongDescription.szTongName, szTongName, sizeof(TongDescription.szTongName));
    TongDescription.szTongName[sizeof(TongDescription.szTongName) - 1] = '\0';

    m_TongTable[dwTongID] = TongDescription;
    m_DealingTongDescription.erase(dwTongID);

    Traverser.dwTongID      = dwTongID;
    Traverser.pszTongName   = TongDescription.szTongName;

    g_pSO3World->m_PlayerSet.Traverse(Traverser);

    return true;
}

KTongMemberInfo* KTongClient::GetMember(DWORD dwMemberID)
{
    KTongMemberInfo*            pResult = NULL;
    KCLIENT_ROSTER::iterator    Iter;

    Iter = m_Roster.find(dwMemberID);

    if (Iter != m_Roster.end())
        pResult = &Iter->second;

    return pResult;
}

BOOL KTongClient::LoadOperationConfig()
{
    BOOL        bResult         = false;
    BOOL        bRetCode        = false;
    int         nHeight         = 0;
    int         nOperationType  = 0;
    int         nOperationIndex = 0;
    ITabFile*   piTabFile       = NULL;
    char        szValue [32];
    char        szFileName[PATH_MAX];
    
    DECLARE_STRING_MAP_BEGIN(KTONG_OPERATION_TYPE)
        REGISTER_STR_TO_VALUE("Speak", totSpeak)
        REGISTER_STR_TO_VALUE("JoinManagerChannel", totJoinManagerChannel)
        REGISTER_STR_TO_VALUE("SpeakInManagerChannel", totSpeakInManagerChannel)
        REGISTER_STR_TO_VALUE("CheckOperationRecord", totCheckOperationRecord)
        REGISTER_STR_TO_VALUE("ModifyAnnouncement", totModifyAnnouncement)
        REGISTER_STR_TO_VALUE("ModifyOnlineMessage", totModifyOnlineMessage)
        REGISTER_STR_TO_VALUE("ModifyIntroduction", totModifyIntroduction)
        REGISTER_STR_TO_VALUE("ModifyRules", totModifyRules)
        REGISTER_STR_TO_VALUE("ModifyMemorabilia", totModifyMemorabilia)
        REGISTER_STR_TO_VALUE("DevelopTechnology", totDevelopTechnology)
        REGISTER_STR_TO_VALUE("ProductItem", totProductItem)

        REGISTER_STR_TO_VALUE("PutRepositoryPage2", totPutRepositoryPage2)
        REGISTER_STR_TO_VALUE("PutRepositoryPage3", totPutRepositoryPage3)
        REGISTER_STR_TO_VALUE("PutRepositoryPage4", totPutRepositoryPage4)
        REGISTER_STR_TO_VALUE("PutRepositoryPage5", totPutRepositoryPage5)
        REGISTER_STR_TO_VALUE("PutRepositoryPage6", totPutRepositoryPage6)
        REGISTER_STR_TO_VALUE("PutRepositoryPage7", totPutRepositoryPage7)
        REGISTER_STR_TO_VALUE("PutRepositoryPage8", totPutRepositoryPage8)
        REGISTER_STR_TO_VALUE("PutRepositoryPage9", totPutRepositoryPage9)
        REGISTER_STR_TO_VALUE("PutRepositoryPage10", totPutRepositoryPage10)

        REGISTER_STR_TO_VALUE("GetFromRepositoryPage2", totGetFromRepositoryPage2)
        REGISTER_STR_TO_VALUE("GetFromRepositoryPage3", totGetFromRepositoryPage3)
        REGISTER_STR_TO_VALUE("GetFromRepositoryPage4", totGetFromRepositoryPage4)
        REGISTER_STR_TO_VALUE("GetFromRepositoryPage5", totGetFromRepositoryPage5)
        REGISTER_STR_TO_VALUE("GetFromRepositoryPage6", totGetFromRepositoryPage6)
        REGISTER_STR_TO_VALUE("GetFromRepositoryPage7", totGetFromRepositoryPage7)
        REGISTER_STR_TO_VALUE("GetFromRepositoryPage8", totGetFromRepositoryPage8)
        REGISTER_STR_TO_VALUE("GetFromRepositoryPage9", totGetFromRepositoryPage9)
        REGISTER_STR_TO_VALUE("GetFromRepositoryPage10", totGetFromRepositoryPage10)

        REGISTER_STR_TO_VALUE("ModifyCalendar", totModifyCalendar)
        REGISTER_STR_TO_VALUE("ManageTask", totManageTask)
        REGISTER_STR_TO_VALUE("ManageVote", totManageVote)
        REGISTER_STR_TO_VALUE("AddToGroup", totAddToGroup)
        REGISTER_STR_TO_VALUE("MoveFromGroup", totMoveFromGroup)
        REGISTER_STR_TO_VALUE("ModifyWage", totModifyWage)
        REGISTER_STR_TO_VALUE("PaySalary", totPaySalary)
        REGISTER_STR_TO_VALUE("ModifyMemberRemark", totModifyMemberRemark)
        REGISTER_STR_TO_VALUE("ModifyGroupName", totModifyGroupName)
    DECLARE_STRING_MAP_END(KTONG_OPERATION_TYPE)

    snprintf(szFileName, sizeof(szFileName), "%s/TongOperation.tab", SETTING_DIR);
    szFileName[sizeof(szFileName) - 1] = '\0';

    piTabFile = g_OpenTabFile(szFileName);

    KGLOG_PROCESS_ERROR(piTabFile);

    nHeight = piTabFile->GetHeight();
    KGLOG_PROCESS_ERROR(nHeight == totTotal);

    for (int nIndex = 1; nIndex < nHeight; nIndex++)
    {
        bRetCode = piTabFile->GetString(nIndex + 1, "Operation", "", szValue, sizeof(szValue));
        KGLOG_PROCESS_ERROR(bRetCode);
        
        MAP_STRING_LOCAL(KTONG_OPERATION_TYPE, szValue, nOperationType);

        bRetCode = piTabFile->GetInteger(nIndex + 1, "Index", 0, &nOperationIndex);
        KGLOG_PROCESS_ERROR(bRetCode);

        KGLOG_PROCESS_ERROR(nOperationIndex >= 0 && nOperationIndex < TONG_OPERATION_NUM);

        KGLOG_PROCESS_ERROR(m_nOperationIndex[nOperationType] == INVALID_TONG_OPERATION);
        m_nOperationIndex[nOperationType] = nOperationIndex;
    }

    bResult = true;
Exit0:
    KG_COM_RELEASE(piTabFile);
    return bResult;
}

BOOL KTongClient::CanBaseOperate(int nGroupID, KTONG_OPERATION_TYPE eOperationType)
{
    BOOL bResult        = false;
    int  nMaskOffset    = 0;

    assert(nGroupID >= 0 && nGroupID < MAX_TONG_GROUP);
    
    nMaskOffset = m_nOperationIndex[eOperationType];
    KGLOG_PROCESS_ERROR(nMaskOffset >= 0 && nMaskOffset < TONG_OPERATION_NUM);
    
    bResult = m_Schema.Data[nGroupID].byBaseOperationMask & (1 << nMaskOffset);

Exit0:
    return bResult;
}

BOOL KTongClient::CanAdvanceOperate(int nGroup, int nTargetGroup, KTONG_OPERATION_TYPE eOperationType)
{
    BOOL bResult        = false;
    int  nMaskOffset    = 0;

    assert(nGroup >= 0 && nGroup < MAX_TONG_GROUP);
    assert(nTargetGroup >= 0 && nTargetGroup < MAX_TONG_GROUP);

    nMaskOffset = m_nOperationIndex[eOperationType];
    KGLOG_PROCESS_ERROR(nMaskOffset >= 0 && nMaskOffset < TONG_OPERATION_NUM);

    bResult = m_Schema.Data[nGroup].byAdvanceOperationMask[nTargetGroup] & (1 << nMaskOffset);

Exit0:
    return bResult;
}

#endif // _CLIENT
