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

    bResult = true;

Exit0:
    return bResult;
}

void KTongClient::UnInit()
{
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
    
    if (g_pGameWorldUIHandler)
    {
        g_pGameWorldUIHandler->OnUpdateTongInfo();
    }
    
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

    m_nFund             = pDevelopmentInfo->nFound;
    m_nDevelopmentPoint = pDevelopmentInfo->nDevelopmentPoint;
    memcpy(m_byTechTree, pDevelopmentInfo->m_byTechTree, sizeof(m_byTechTree));

    if (g_pGameWorldUIHandler)
    {
        g_pGameWorldUIHandler->OnUpdateTongInfo();
    }

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

    if (g_pGameWorldUIHandler)
    {
        g_pGameWorldUIHandler->OnUpdateTongInfo();
    }

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

    if (g_pGameWorldUIHandler)
    {
        g_pGameWorldUIHandler->OnUpdateTongInfo();
    }

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

    if (g_pGameWorldUIHandler)
    {
        g_pGameWorldUIHandler->OnUpdateTongInfo();
    }

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

    if (g_pGameWorldUIHandler)
    {
        g_pGameWorldUIHandler->OnUpdateTongInfo();
    }

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

    if (g_pGameWorldUIHandler)
    {
        g_pGameWorldUIHandler->OnUpdateTongInfo();
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

BOOL KTongClient::ProcessGetTongDescrioptionRespond(DWORD dwTongID, char szTongName[])
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

        //REGISTER_STR_TO_VALUE("GetFromRepositoryPage1", totGetFromRepositoryPage1)
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
