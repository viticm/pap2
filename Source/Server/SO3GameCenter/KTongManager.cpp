/************************************************************************/
/* 帮会管理		                                                        */
/* Copyright : Kingsoft 2007										    */
/* Author	 : Zhao chunfeng										    */
/* History	 : 基于原来的TongData重构								    */
/* 2007.11.13	Create												    */
/************************************************************************/
#include "stdafx.h"
#include "StrMap.h"
#include "KRole.h"
#include "KTongManager.h"
#include "KSO3GameCenter.h"
#include "KTong.h"
#include "KDBTools.h"

using namespace std;

#pragma pack(1)
struct KTongSchemaFileHeader
{
    int nVersion;
    int nCount;
};
#pragma pack()

#define TONG_SCHEMA_FILE_VERSION    2
#define MAX_TONG_SCHEMA_TEMPLATE    64

KTongManager::KTongManager()
{
    for (int i = 0; i < (int)sizeof(m_nOperationIndex)/sizeof(int); i++)
        m_nOperationIndex[i] = INVALID_TONG_OPERATION;
}

BOOL KTongManager::Init()
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    KTongHistory::Config();

    bRetCode = LoadOperationConfig();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = LoadSchemaTemplate();
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

void KTongManager::UnInit()
{
    KTong* pTong = NULL;
    
    for (KTONG_TABLE::iterator it = m_TongTable.begin(); it != m_TongTable.end(); ++it)
    {
        pTong = &it->second;

        if (pTong->m_bUpdateFlag)
        {
            g_pSO3GameCenter->m_MiscDB.DoSaveTongData(pTong->m_dwID, pTong);
            pTong->m_bUpdateFlag = false;
        }

        it->second.UnInit();
    }

    m_TongTable.clear();
}

void KTongManager::Activate()
{   
    if (g_pSO3GameCenter->m_nWorkLoop % (GAME_FPS * 10) == 0)
    {
        RemoveTimeOutInvite();
    }
    
    if (g_pSO3GameCenter->m_nWorkLoop % (GAME_FPS * 10) == 2)
    {
        ProcessCheckTongState();
    }

    if (g_pSO3GameCenter->m_nWorkLoop % (GAME_FPS * 10) == 8)
    {
        for (KTONG_TABLE::iterator it = m_TongTable.begin(); it != m_TongTable.end(); ++it)
        {
            it->second.Activate();
        }
    }
}

void KTongManager::RemoveTimeOutInvite()
{
    time_t                                  nTimeNow = g_pSO3GameCenter->m_nTimeNow;
    KRole*                                  pInviter = NULL;
    KRole*                                  pPlayer  = NULL;
    KTONG_JOIN_APPLICATION_TABLE::iterator  it;

    for (it = m_InviteApplication.begin(); it != m_InviteApplication.end(); NULL)
    {
        if (it->second.nTimeOut < nTimeNow)
        {
            pInviter = g_pSO3GameCenter->m_RoleManager.GetRole(it->second.InviterID);
            pPlayer  = g_pSO3GameCenter->m_RoleManager.GetRole(it->first);

            m_InviteApplication.erase(it++);

            if (pInviter && pPlayer && pInviter->IsOnline())
            {
                SendTongEventNotify(
                    pInviter->m_nConnIndex, pInviter->m_dwPlayerID, tenInviteTimeOutError,
                    pPlayer->m_szName
                );
            }

            continue;
        }

        ++it;
    }
}

void KTongManager::ProcessCheckTongState()
{
    BYTE byTongState = 0;

    for (KTONG_TABLE::iterator it = m_TongTable.begin(); it != m_TongTable.end(); NULL)
    {
        KTong*  pTong               = &it->second;
        BOOL    bTongStateChange    = false;
        
        switch (pTong->m_eState)
        {
        case tsTrialTong:
            if (pTong->m_Roster.size() >= MIN_TONG_MEMBER_COUNT)
            {
                pTong->m_eState         = tsNormalTong;
                pTong->m_nStateTimer    = 0;
                pTong->m_bUpdateFlag    = true;
                pTong->m_nLastUpdateFrameTable[tdtBaseInfo] = g_pSO3GameCenter->m_nWorkLoop;
                bTongStateChange        = true;
            }
            else if (g_pSO3GameCenter->m_nTimeNow > pTong->m_nStateTimer)
            {
                byTongState = (BYTE)tsInvalid;
                g_pSO3GameCenter->m_GameServer.DoTongBroadcastMessage(pTong, tbmTongStateChange, ERROR_ID, &byTongState, sizeof(byTongState));
                pTong->Disband();
                pTong->UnInit();
                m_TongTable.erase(it++);
                continue;   // for (KTONG_TABLE::iterator it ..
            }
            break;          // switch (pTong->m_eState)

        case tsNormalTong:
            if (pTong->m_Roster.size() < MIN_TONG_MEMBER_COUNT)
            {
                pTong->m_eState         = tsDisbandTong;
                pTong->m_nStateTimer    = g_pSO3GameCenter->m_nTimeNow + TONG_DISBAND_TIME;
                pTong->m_bUpdateFlag    = true;
                pTong->m_nLastUpdateFrameTable[tdtBaseInfo] = g_pSO3GameCenter->m_nWorkLoop;
                bTongStateChange        = true;
            }
            break;

        case tsDisbandTong:
            if (pTong->m_Roster.size() >= MIN_TONG_MEMBER_COUNT)
            {
                pTong->m_eState         = tsNormalTong;
                pTong->m_nStateTimer    = 0;
                pTong->m_bUpdateFlag    = true;
                pTong->m_nLastUpdateFrameTable[tdtBaseInfo] = g_pSO3GameCenter->m_nWorkLoop;
                bTongStateChange        = true;
            }
            else if (g_pSO3GameCenter->m_nTimeNow > pTong->m_nStateTimer)
            {
                byTongState = (BYTE)tsInvalid;
                g_pSO3GameCenter->m_GameServer.DoTongBroadcastMessage(pTong, tbmTongStateChange, ERROR_ID, &byTongState, sizeof(byTongState));
                pTong->Disband();
                pTong->UnInit();
                m_TongTable.erase(it++);
                continue;
            }
            break;

       default:
            assert(false);
            break;
        }

        if (bTongStateChange)
        {
            byTongState = (BYTE)pTong->m_eState;
            g_pSO3GameCenter->m_GameServer.DoTongBroadcastMessage(pTong,  tbmTongStateChange, ERROR_ID,&byTongState, sizeof(byTongState));
        }

        ++it; 
    }
}

BOOL KTongManager::CreateTong(DWORD dwMasterID, const char cszName[], int nTemplateIndex)
{
    BOOL                                bResult     = false;
    BOOL                                bRetCode    = false;
    KRole*                              pMaster     = NULL;
    KTong*                              pTong       = NULL;
    KTong*                              pTongRet    = NULL;
    DWORD                               dwTongID    = ERROR_ID;
    unsigned long long                  ullID       = ERROR_ID;
    BOOL                                bInsertFlag = false;
    pair<KTONG_TABLE::iterator, BOOL>   TongInsRet;
    
    assert(cszName);

    KGLOG_PROCESS_ERROR(nTemplateIndex >= 0 && nTemplateIndex < (int)m_SchemaTemplate.size());

    pMaster = g_pSO3GameCenter->m_RoleManager.GetRole(dwMasterID);
    KGLOG_PROCESS_ERROR(pMaster);
    
    if (pMaster->m_dwTongID != ERROR_ID)
    {
        SendTongEventNotify(pMaster->m_nConnIndex, dwMasterID, tenCreateTongAlreadyInTongError);
        goto Exit0;
    }

    // 帮会名关键字过滤
    bRetCode = g_pSO3GameCenter->m_piTextFilter->Check(cszName);
    if (!bRetCode)
    {
        SendTongEventNotify(pMaster->m_nConnIndex, dwMasterID, tenCreateTongNameIllegalError);
        goto Exit0;
    }

    pTongRet = GetTongByName(cszName);
    
    if (pTongRet)
    {
        SendTongEventNotify(pMaster->m_nConnIndex, dwMasterID, tenCreateTongNameConflictError);
        goto Exit0;
    }

    bRetCode = g_pSO3GameCenter->m_piSequence->GenerateID("TongID", 8, &ullID);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    dwTongID = (DWORD)ullID;

    TongInsRet = m_TongTable.insert(make_pair(dwTongID, KTong()));
    KGLOG_PROCESS_ERROR(TongInsRet.second);
    bInsertFlag = true;

    pTong = &TongInsRet.first->second;
    pTong->Init();

    pTong->m_dwID               = dwTongID;
    pTong->m_dwMaster           = pMaster->m_dwPlayerID;
    pTong->m_nMaxMemberCount    = TONG_INIT_MAX_MEMBER_COUNT;
    pTong->m_nDevelopmentPoint  = 0;
    pTong->m_eState             = tsTrialTong;          // 帮会进入筹备状态
    pTong->m_eCamp              = pMaster->m_eCamp;     // 设置阵营为创立者所属的阵营
    pTong->m_nStateTimer        = g_pSO3GameCenter->m_nTimeNow + TONG_TRIAL_TIME; 

    strncpy(pTong->m_szName, cszName, sizeof(pTong->m_szName));
    pTong->m_szName[sizeof(pTong->m_szName) - 1] = '\0';

    pTong->m_Schema = m_SchemaTemplate[nTemplateIndex];
    pTong->m_Repertory.m_Page[0].bEnable = true; // 开启仓库第一页

    bRetCode = pTong->AddNewMember(pMaster, m_SchemaTemplate[nTemplateIndex].nMasterGroup);
    KGLOG_PROCESS_ERROR(bRetCode);

    KGLogPrintf(KGLOG_INFO, "Player '%s'(id:%d) create tong '%s'(id:%d) success!\n", 
        pMaster->m_szName, dwMasterID, pTong->m_szName, pTong->m_dwID
    );

    // 同步帮会ID
    g_pSO3GameCenter->m_GameServer.DoChangePlayerTongNotify(pMaster->m_nConnIndex, dwMasterID, dwTongID, ctrCreate, pTong->m_szName);

    pTong->m_bUpdateFlag = true;
    
    bRetCode = OnJoinTong(dwMasterID, dwTongID);
    KGLOG_CHECK_ERROR(bRetCode);

    bResult = true;
Exit0:
    if (!bResult)
    {
        if (bInsertFlag)
        {
            if (pTong)
                pTong->UnInit();

            m_TongTable.erase(dwTongID);
            bInsertFlag = false;
        }
    }

    return bResult;
}

KTong* KTongManager::GetTong(DWORD dwID)
{
    KTONG_TABLE::iterator it = m_TongTable.find(dwID);
    if (it != m_TongTable.end())
    {
        return &it->second;
    }
    return NULL;
}

KTong* KTongManager::GetTongByName(const char* pszTongName)
{
    KTong* pTongResult = NULL;

    for (KTONG_TABLE::iterator it = m_TongTable.begin(); it != m_TongTable.end(); ++it)
    {
        if (strncmp(pszTongName, it->second.m_szName, sizeof(it->second.m_szName)) == 0)
        {
            pTongResult = &it->second;
            break;
        }
    }

    return pTongResult;
}

int KTongManager::GetOperationIndex(KTONG_OPERATION_TYPE eOperationType)
{
    assert(eOperationType > totInvalid && eOperationType < totTotal);
    return m_nOperationIndex[eOperationType];
}

BOOL KTongManager::LoadTongData(DWORD dwTongID, BYTE* pbyTongData, size_t uDataLen)
{
    BOOL                                    bResult     = false;
    BOOL                                    bRetCode    = false;
    KTong*                                  pTong       = NULL;
    BOOL                                    bInsertFlag = false;
    std::pair<KTONG_TABLE::iterator, bool>  InsertRet;

    assert(pbyTongData);
    
    InsertRet = m_TongTable.insert(make_pair(dwTongID, KTong()));
    KGLOG_PROCESS_ERROR(InsertRet.second);
    bInsertFlag = true;

    pTong = &InsertRet.first->second;
    KGLOG_PROCESS_ERROR(pTong);
    pTong->Init();

    pTong->m_dwID = dwTongID;

    bRetCode = pTong->Load(dwTongID, pbyTongData, uDataLen);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;

Exit0:
    if (!bResult)
    {
        if (bInsertFlag)
        {
            pTong->UnInit();
            m_TongTable.erase(dwTongID);
            bInsertFlag = false;
        }
    }

    return bResult;
}

void KTongManager::ProcessApplyTongInfoRequest(DWORD dwPlayerID, int nLastUpdateFrame)
{
    BOOL                        bRetCode    = false;
    DWORD                       dwTongID    = ERROR_ID;
    KRole*                      pPlayer     = NULL;
    KTong*                      pTong       = NULL;

    pPlayer = g_pSO3GameCenter->m_RoleManager.GetRole(dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    KG_PROCESS_ERROR(pPlayer->m_dwTongID != ERROR_ID);

    pTong = GetTong(pPlayer->m_dwTongID);
    KGLOG_PROCESS_ERROR(pTong);

    if (nLastUpdateFrame < pTong->m_nLastUpdateFrameTable[tdtBaseInfo])
    {
        g_pSO3GameCenter->m_GameServer.DoUpdateTongBaseInfo(pPlayer->m_nConnIndex, dwPlayerID, pTong);
    }

    if (nLastUpdateFrame < pTong->m_nLastUpdateFrameTable[tdtDevelopmentInfo])
    {
        g_pSO3GameCenter->m_GameServer.DoUpdateTongDevelopmentInfo(pPlayer->m_nConnIndex, dwPlayerID, pTong);
    }

    if (nLastUpdateFrame < pTong->m_nLastUpdateFrameTable[tdtAnnouncement])
    {
        g_pSO3GameCenter->m_GameServer.DoUpdateTongTextInfo(
            pPlayer->m_nConnIndex, dwPlayerID, 
            tdtAnnouncement, pTong->m_szAnnouncement
        );
    }

    if (nLastUpdateFrame < pTong->m_nLastUpdateFrameTable[tdtOnlineMessage])
    {
        g_pSO3GameCenter->m_GameServer.DoUpdateTongTextInfo(
            pPlayer->m_nConnIndex, dwPlayerID, 
            tdtOnlineMessage, pTong->m_szOnlineMessage
        );
    }

    if (nLastUpdateFrame < pTong->m_nLastUpdateFrameTable[tdtIntroduction])
    {
        g_pSO3GameCenter->m_GameServer.DoUpdateTongTextInfo(
            pPlayer->m_nConnIndex, dwPlayerID, 
            tdtIntroduction, pTong->m_szIntroduction
        );
    }

    if (nLastUpdateFrame < pTong->m_nLastUpdateFrameTable[tdtRules])
    {
        g_pSO3GameCenter->m_GameServer.DoUpdateTongTextInfo(
            pPlayer->m_nConnIndex, dwPlayerID, 
            tdtRules, pTong->m_szRules
        );
    }

    if (nLastUpdateFrame < pTong->m_nLastUpdateFrameTable[tdtSchemaInfo])
    {
        g_pSO3GameCenter->m_GameServer.DoUpdateTongSchemaInfo(pPlayer->m_nConnIndex, dwPlayerID, &pTong->m_Schema);
    }

    if (nLastUpdateFrame < pTong->m_nLastUpdateFrameTable[tdtMemorabilia])
    {
        g_pSO3GameCenter->m_GameServer.DoUpdateTongMemorabilia(pPlayer->m_nConnIndex, dwPlayerID, pTong);
    }

    g_pSO3GameCenter->m_GameServer.DoUpdateTongClientDataVersion(
        pPlayer->m_nConnIndex, dwPlayerID, tufInfo, g_pSO3GameCenter->m_nWorkLoop
    );

Exit0:
    return;
}

void KTongManager::ProcessApplyRosterRequest(DWORD dwPlayerID, int nClientLastUpdateFrame, DWORD* pdwMemberList, int nMemberCount)
{
    BOOL                            bRetCode        = false;
    DWORD                           dwTongID        = ERROR_ID;
    KRole*                          pRole           = NULL;
    const KTong*                    pTong           = NULL;

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);

    KG_PROCESS_ERROR(pRole->m_dwTongID != ERROR_ID);

    pTong = GetTong(pRole->m_dwTongID);
    KGLOG_PROCESS_ERROR(pTong);

    for (KTong::KROSTER::const_iterator MemberIter = pTong->m_Roster.begin(); MemberIter != pTong->m_Roster.end(); ++MemberIter)
    {
        if (MemberIter->second.nLastUpdateFrame <= nClientLastUpdateFrame)
            continue;

        bRetCode = g_pSO3GameCenter->m_GameServer.DoSyncTongMemberInfo(
            pRole->m_nConnIndex, dwPlayerID, MemberIter->first, MemberIter->second
        );
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    for (int i = 0; i < nMemberCount; i++)
    {
        if (pTong->m_Roster.find(pdwMemberList[i]) == pTong->m_Roster.end())
        {
            g_pSO3GameCenter->m_GameServer.DoTongDeleteMemberNotify(pRole->m_nConnIndex, dwPlayerID, pdwMemberList[i]);
        }
    }

    bRetCode = g_pSO3GameCenter->m_GameServer.DoUpdateTongClientDataVersion(
        pRole->m_nConnIndex, dwPlayerID, tufRoster, g_pSO3GameCenter->m_nWorkLoop
    );
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KTongManager::ProcessInvitePlayerJoinTongRequest(DWORD dwPlayerID, char* pszNewMemberName)
{
    BOOL                    bRetCode    = false;
    KRole*                  pPlayer     = NULL;
    KRole*                  pNewMember  = NULL;
    KTong*                  pTong       = NULL;
    KTongMember*            pMember     = NULL;
    KTONG_JOIN_APPLICATION  Apply;

    assert(pszNewMemberName);

    pPlayer = g_pSO3GameCenter->m_RoleManager.GetRole(dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    KG_PROCESS_ERROR(pPlayer->m_dwTongID != ERROR_ID);
       
    pTong   = GetTong(pPlayer->m_dwTongID);
    KGLOG_PROCESS_ERROR(pTong);

    pNewMember = g_pSO3GameCenter->m_RoleManager.GetRole(pszNewMemberName);
    if (pNewMember == NULL)
    {
        // 角色名不存在
        // send Message
        SendTongEventNotify(
            pPlayer->m_nConnIndex, dwPlayerID, tenInviteNameNotFoundError, pszNewMemberName
        );
        goto Exit0;
    }
    
    if (pNewMember->m_dwTongID != ERROR_ID)
    {
        // 对方已加入别的帮会
        // Send Message
        SendTongEventNotify(
            pPlayer->m_nConnIndex, dwPlayerID, tenInviteTargetAlreadyJoinTongError, pszNewMemberName
        );
        goto Exit0; 
    }

    // 对方是否处于敌对阵营
    bRetCode = pTong->CheckCampCompatible(pNewMember->m_eCamp);
    if (!bRetCode)
    {
        SendTongEventNotify(
            pPlayer->m_nConnIndex, dwPlayerID, tenInviteTargetInRivalCampError, pszNewMemberName
        );
        goto Exit0; 
    }

    if (m_InviteApplication.find(pNewMember->m_dwPlayerID) != m_InviteApplication.end())
    {
        // 对方正在被邀请
        // Send Message
        SendTongEventNotify(
            pPlayer->m_nConnIndex, dwPlayerID, tenInviteTargetBusyError, pszNewMemberName
        );
        goto Exit0;
    }

    if (!pNewMember->IsOnline())
    {
        // 对方不在线
        // Send Message
        SendTongEventNotify(
            pPlayer->m_nConnIndex, dwPlayerID, tenInviteTargetOfflineError, pszNewMemberName
        );
        goto Exit0;
    }

    if ((int)pTong->m_Roster.size() >= pTong->m_nMaxMemberCount)
    {
        // 人满
        // Send Message to Inviter
        SendTongEventNotify(pPlayer->m_nConnIndex, dwPlayerID, tenInviteMemberNumberLimitError);
        goto Exit0;
    }

    pMember = pTong->GetMember(dwPlayerID);

    // 检查权限
    bRetCode = pTong->CanAdvanceOperate(pMember->nGroupID, pTong->m_Schema.nDefaultGroup, totAddToGroup);
    if (!bRetCode)
    {
        // 未授权
        // Send Message
        SendTongEventNotify(pPlayer->m_nConnIndex, dwPlayerID, tenInviteNoPermissionError);
        goto Exit0;
    }

    Apply.dwTongID      = pPlayer->m_dwTongID;
    Apply.InviterID     = dwPlayerID;
    Apply.nTimeOut      = g_pSO3GameCenter->m_nTimeNow + TONG_INVITE_TIME;

    m_InviteApplication[pNewMember->m_dwPlayerID] = Apply;

    //  发送确认请求
    g_pSO3GameCenter->m_GameServer.DoInvitePlayerJoinTongRequest(
        pNewMember->m_nConnIndex, pNewMember->m_dwPlayerID, dwPlayerID, pTong->m_dwID, 
        pPlayer->m_szName, pTong->m_szName
    );

Exit0:
    return;
}

void KTongManager::ProcessInvitePlayerJoinTongRespond(DWORD dwPlayerID, DWORD dwInviterID, DWORD dwTongID, INVITE_TONG_RESULT_CODE eAccept)
{
    BOOL                                    bRetCode        = false;
    KRole*                                  pRole           = NULL;
    KRole*                                  pInviterRole    = NULL;
    KTong*                                  pTong           = NULL;
    KTongMember*                            pInviterMember  = NULL;
    KTONG_JOIN_APPLICATION*                 pInviteRecord   = NULL;
    KTONG_MEMBER_JOIN_OR_QUIT_BROADCAST     ChangeInfo;
    KTONG_JOIN_APPLICATION_TABLE::iterator  it;

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);

    pInviterRole =  g_pSO3GameCenter->m_RoleManager.GetRole(dwInviterID);
    KGLOG_PROCESS_ERROR(pInviterRole);

    it = m_InviteApplication.find(pRole->m_dwPlayerID);
    if (it == m_InviteApplication.end())
    {
        // 请求不存在或已失效
        if (eAccept == itrSuccess && pRole->m_nConnIndex != 0)
        {
            SendTongEventNotify(pRole->m_nConnIndex, dwPlayerID, tenInviteInvaildError);
        }

        goto Exit0;
    }

    pInviteRecord = &it->second;
    KGLOG_PROCESS_ERROR(pInviteRecord->dwTongID == dwTongID);
    KGLOG_PROCESS_ERROR(pInviteRecord->InviterID == dwInviterID);

    m_InviteApplication.erase(it);

    if (eAccept == itrRefuse)
    {
        // 拒绝
        // Send Message to inviter
        if (pInviterRole->m_nConnIndex != 0)
        {
            SendTongEventNotify(
                pInviterRole->m_nConnIndex, dwInviterID, tenInviteTargetRefuse, pRole->m_szName
            );
        }
        goto Exit0;
    }

    if (eAccept == itrChargeLimit)
    {
        // 拒绝：因为免费阶段
        if (pInviterRole->m_nConnIndex != 0)
        {
            SendTongEventNotify(
                pInviterRole->m_nConnIndex, dwInviterID, tenInviteTargetChargeLimit, pRole->m_szName
            );
        }
        goto Exit0;
    }

    pTong = GetTong(dwTongID);
    if(!pTong)  // 帮会已解散
    {
        SendTongEventNotify(pRole->m_nConnIndex, dwPlayerID, tenInviteInvaildError);
        goto Exit0;
    }

    // 目标帮会是敌对阵营
    bRetCode = pTong->CheckCampCompatible(pRole->m_eCamp);
    if (!bRetCode)
    {
        SendTongEventNotify(
            pInviterRole->m_nConnIndex, dwInviterID, tenTargetTongIsRivalCampError, pTong->m_szName
        );

        SendTongEventNotify(
            pInviterRole->m_nConnIndex, dwInviterID, tenInviteTargetInRivalCampError, pRole->m_szName
        );
        goto Exit0; 
    }

    if ((int)pTong->m_Roster.size() >= pTong->m_nMaxMemberCount)
    {
        // 人满
        // Send Message to player and inviter
        SendTongEventNotify(pRole->m_nConnIndex, dwPlayerID, tenInviteMemberNumberLimitError);
        SendTongEventNotify(pInviterRole->m_nConnIndex, dwInviterID, tenInviteMemberNumberLimitError);
        goto Exit0;
    }

    pInviterMember = pTong->GetMember(dwInviterID);
    if(!pInviterMember) //邀请人已删号或退出了帮会
    {
        SendTongEventNotify(pRole->m_nConnIndex, dwPlayerID, tenInviteInvaildError);
        goto Exit0;
    }

    // 再次检查邀请人权限
    bRetCode = pTong->CanAdvanceOperate(pInviterMember->nGroupID, pTong->m_Schema.nDefaultGroup, totAddToGroup);
    if (!bRetCode)
    {
        SendTongEventNotify(pRole->m_nConnIndex, dwPlayerID, tenInviteInvaildError);
        goto Exit0;
    }

    bRetCode = pTong->AddNewMember(pRole, pTong->m_Schema.nDefaultGroup);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = pTong->m_History.RecordJoin(dwPlayerID);
    KGLOG_CHECK_ERROR(bRetCode);

    g_pSO3GameCenter->m_GameServer.DoChangePlayerTongNotify(pRole->m_nConnIndex, dwPlayerID, dwTongID, ctrJoin, pTong->m_szName);

    SendTongEventNotify(
        pInviterRole->m_nConnIndex, dwInviterID, tenInviteSuccess, pRole->m_szName
    );

    strncpy(ChangeInfo.szMemberName, pRole->m_szName, sizeof(ChangeInfo));
    ChangeInfo.szMemberName[sizeof(ChangeInfo.szMemberName) - 1] = '\0';

    g_pSO3GameCenter->m_GameServer.DoTongBroadcastMessage(pTong, tbmMemberJoin, dwPlayerID, (BYTE*)&ChangeInfo, sizeof(ChangeInfo));
        
    bRetCode = OnJoinTong(dwPlayerID, dwTongID);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KTongManager::ProcessKickOutTongMemberRequest(DWORD dwPlayerID, DWORD dwMemberID)
{
    BOOL                                bRetCode        = false;
    KTong*                              pTong           = NULL;
    KRole*                              pSrcRole        = NULL;
    KRole*                              pDstRole        = NULL;
    KTongMember*                        pSrcMember      = NULL;
    KTongMember*                        pDstMember      = NULL;
    KTONG_MEMBER_JOIN_OR_QUIT_BROADCAST ChangeInfo;

    pSrcRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwPlayerID);
    KGLOG_PROCESS_ERROR(pSrcRole);

    pDstRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwMemberID);
    KGLOG_PROCESS_ERROR(pDstRole);

    KG_PROCESS_ERROR(pSrcRole->m_dwTongID != ERROR_ID);
    KG_PROCESS_ERROR(pSrcRole->m_dwTongID == pDstRole->m_dwTongID);

    pTong = GetTong(pSrcRole->m_dwTongID);
    KGLOG_PROCESS_ERROR(pTong);

    pSrcMember = pTong->GetMember(dwPlayerID);
    KGLOG_PROCESS_ERROR(pSrcMember);

    pDstMember = pTong->GetMember(dwMemberID);
    if (pDstMember == NULL)
    {
        SendTongEventNotify(pSrcRole->m_nConnIndex, dwPlayerID, tenKickOutTargetNotFoundError);
        goto Exit0;
    }

    // 检查权限
    bRetCode = pTong->CanAdvanceOperate(pSrcMember->nGroupID, pDstMember->nGroupID, totMoveFromGroup);
    if (!bRetCode)
    {
        
        SendTongEventNotify(
            pSrcRole->m_nConnIndex, dwPlayerID, tenKickOutTargetNoPermissionError, pDstRole->m_szName
        );
        goto Exit0;
    }

    // 移除
    bRetCode = pTong->RemoveMember(pDstRole);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = pTong->m_History.RecordKickout(dwPlayerID, dwMemberID);
    KGLOG_CHECK_ERROR(bRetCode);

    // Send Message
    g_pSO3GameCenter->m_GameServer.DoChangePlayerTongNotify(pDstRole->m_nConnIndex, dwMemberID, pDstRole->m_dwTongID, ctrFired, pTong->m_szName);

    SendTongEventNotify(
        pSrcRole->m_nConnIndex, dwPlayerID, tenKickOutSuccess, pDstRole->m_szName
    );

    strncpy(ChangeInfo.szMemberName, pDstRole->m_szName, sizeof(ChangeInfo));
    ChangeInfo.szMemberName[sizeof(ChangeInfo.szMemberName) - 1] = '\0';

    g_pSO3GameCenter->m_GameServer.DoTongBroadcastMessage(pTong, tbmMemberFired, dwPlayerID, (BYTE*)&ChangeInfo, sizeof(ChangeInfo));
Exit0:
    return;
}

void KTongManager::ProcessModifyTongInfoRequest(DWORD dwPlayerID, int nInfoType, BYTE* pbyData, size_t uDataLen)
{
    BOOL            bRetCode    = false;
    KRole*          pRole       = NULL;
    KTong*          pTong       = NULL;
    KTongMember*    pMember     = NULL;

    assert(pbyData);
    
    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);

    KG_PROCESS_ERROR(pRole->m_dwTongID != ERROR_ID);

    pTong = GetTong(pRole->m_dwTongID);
    KGLOG_PROCESS_ERROR(pTong);

    pMember = pTong->GetMember(dwPlayerID);
    KGLOG_PROCESS_ERROR(pMember);

    switch (nInfoType)
    {
    case tmrTongName:
        KGLOG_PROCESS_ERROR(uDataLen >= 4 && uDataLen <= sizeof(pTong->m_szName));

        // 只有帮主才能修改帮会名称
        if (dwPlayerID != pTong->m_dwMaster)
        {
            SendTongEventNotify(pRole->m_nConnIndex, dwPlayerID, tenModifyTongNameNoPermissionError);
            goto Exit0;
        }

        strncpy(pTong->m_szName, (char*)pbyData, sizeof(pTong->m_szName));
        pTong->m_szName[sizeof(pTong->m_szName) - 1] = '\0';
        pTong->m_nLastUpdateFrameTable[tdtBaseInfo] = g_pSO3GameCenter->m_nWorkLoop;

        SendTongEventNotify(pRole->m_nConnIndex, dwPlayerID, tenModifyTongNameSuccess);

        break;

    case tmrAnnouncement:
        KGLOG_PROCESS_ERROR(uDataLen <= sizeof(pTong->m_szAnnouncement));

        bRetCode = pTong->CanBaseOperate(pMember->nGroupID, totModifyAnnouncement);

        if (!bRetCode)
        {
            SendTongEventNotify(pRole->m_nConnIndex, dwPlayerID, tenModifyAnnouncementNoPermissionError);
            goto Exit0;
        }

        strncpy(pTong->m_szAnnouncement, (char*)pbyData, sizeof(pTong->m_szAnnouncement));
        pTong->m_szAnnouncement[sizeof(pTong->m_szAnnouncement) - 1] = '\0';
        pTong->m_nLastUpdateFrameTable[tdtAnnouncement] = g_pSO3GameCenter->m_nWorkLoop;

        SendTongEventNotify(pRole->m_nConnIndex, dwPlayerID, tenModifyAnnouncementSuccess);

        break;

    case tmrOnlineMessage:
        KGLOG_PROCESS_ERROR(uDataLen <= sizeof(pTong->m_szOnlineMessage));

        bRetCode = pTong->CanBaseOperate(pMember->nGroupID, totModifyOnlineMessage);

        if (!bRetCode)
        {
            SendTongEventNotify(pRole->m_nConnIndex, dwPlayerID, tenModifyOnlineMessageNoPermissionError);
            goto Exit0;
        }

        strncpy(pTong->m_szOnlineMessage, (char*)pbyData, sizeof(pTong->m_szOnlineMessage));
        pTong->m_szOnlineMessage[sizeof(pTong->m_szOnlineMessage) - 1] = '\0';
        pTong->m_nLastUpdateFrameTable[tdtOnlineMessage] = g_pSO3GameCenter->m_nWorkLoop;
        SendTongEventNotify(pRole->m_nConnIndex, dwPlayerID, tenModifyOnlineMessageSuccess);

        break;

    case tmrIntroduction:
        KGLOG_PROCESS_ERROR(uDataLen <= sizeof(pTong->m_szIntroduction));

        bRetCode = pTong->CanBaseOperate(pMember->nGroupID, totModifyIntroduction);

        if (!bRetCode)
        {
            SendTongEventNotify(pRole->m_nConnIndex, dwPlayerID, tenModifyIntroductionNoPermissionError);
            goto Exit0;
        }

        strncpy(pTong->m_szIntroduction, (char*)pbyData, sizeof(pTong->m_szIntroduction));
        pTong->m_szIntroduction[sizeof(pTong->m_szIntroduction) - 1] = '\0';
        pTong->m_nLastUpdateFrameTable[tdtIntroduction] = g_pSO3GameCenter->m_nWorkLoop;
        SendTongEventNotify(pRole->m_nConnIndex, dwPlayerID, tenModifyIntroductionSuccess);

        break;

    case tmrRules:
        KGLOG_PROCESS_ERROR(uDataLen <= sizeof(pTong->m_szRules));

        bRetCode = pTong->CanBaseOperate(pMember->nGroupID, totModifyRules);
        
        if (!bRetCode)
        {
            SendTongEventNotify(pRole->m_nConnIndex, dwPlayerID, tenModifyRulesNoPermissionError);
            goto Exit0;
        }

        strncpy(pTong->m_szRules, (char*)pbyData, sizeof(pTong->m_szRules));
        pTong->m_szRules[sizeof(pTong->m_szRules) - 1] = '\0';
        pTong->m_nLastUpdateFrameTable[tdtRules] = g_pSO3GameCenter->m_nWorkLoop;
        SendTongEventNotify(pRole->m_nConnIndex, dwPlayerID, tenModifyRulesSuccess);

        break;
    case tmrMemorabilia:

        bRetCode = pTong->CanBaseOperate(pMember->nGroupID, totModifyMemorabilia);
        // Send Message to player
        KGLOG_PROCESS_ERROR(bRetCode);

        KGLOG_PROCESS_ERROR(uDataLen > sizeof(KTONG_MODIFY_MEMORABILIA_INFO));
        {
            KTONG_MODIFY_MEMORABILIA_INFO* pInfo = (KTONG_MODIFY_MEMORABILIA_INFO*)pbyData;
            KGLOG_PROCESS_ERROR(pInfo->byOperationType <= mmtDelete);

            bRetCode = ProcessModifyMemorabiliarequest((KTONG_MODIFY_MEMORABILIA_TYPE)pInfo->byOperationType, pInfo, uDataLen, pTong);
            
            if(!bRetCode)
            {
                SendTongEventNotify(pRole->m_nConnIndex, dwPlayerID, tenModifyMemorabiliaError);
                goto Exit0;
            }

            SendTongEventNotify(pRole->m_nConnIndex, dwPlayerID, tenModifyMemorabiliaSuccess);


            pTong->m_nLastUpdateFrameTable[tdtMemorabilia] = g_pSO3GameCenter->m_nWorkLoop;
        }
        break;

    default:
        KGLogPrintf(KGLOG_DEBUG, "[KTongManager] ProcessModifyTongTextInfoRequest unrecognized Text Info Type!\n");
        goto Exit0;
    }

    pTong->m_bUpdateFlag = true;

Exit0:
    return;
}

BOOL KTongManager::ProcessModifyMemorabiliarequest(
        KTONG_MODIFY_MEMORABILIA_TYPE eType, 
        KTONG_MODIFY_MEMORABILIA_INFO* pInfo, 
        size_t uDataLen, 
        KTong* pTong
)
{
    BOOL bResult = false;

    switch (eType)
    {
    case mmtAdd:
        KGLOG_PROCESS_ERROR(uDataLen == sizeof(KTONG_ADD_MEMORABILIA));
        KG_PROCESS_ERROR(pTong->m_Memorabilia.size() < MAX_TONG_MEMORABILIA_COUNT);
        {
            KTONG_ADD_MEMORABILIA*  pAdd = (KTONG_ADD_MEMORABILIA*)pInfo;
            KTongIncidentRecord     Record;
            Record.dwID = ++(pTong->m_dwMemorabiliaCount);
            Record.nTime = pAdd->nTime;
            strncpy(Record.szDescription, pAdd->szDescription, sizeof(Record.szDescription));
            Record.szDescription[sizeof(Record.szDescription) - 1] = '\0';

            pTong->m_Memorabilia.push_back(Record);
        }

        break;
    case mmtModify:
        KGLOG_PROCESS_ERROR(uDataLen == sizeof(KTONG_MODIFY_MEMORABILIA));
        {
            KTONG_MODIFY_MEMORABILIA*   pModify = (KTONG_MODIFY_MEMORABILIA*)pInfo;
            BOOL                        bFound  = false;
          
            for (KTong::KINCIDENT_TABLE::iterator it = pTong->m_Memorabilia.begin(); it != pTong->m_Memorabilia.end(); ++it)
            {
                if (it->dwID == pModify->dwID)
                {
                    it->nTime = pModify->nTime;
                    strncpy(it->szDescription, pModify->szDescription, sizeof(it->szDescription));
                    it->szDescription[sizeof(it->szDescription) - 1] = '\0';
                    bFound = true;
                    break;
                }
            }
            // 该记录已不存在的情况
            KG_PROCESS_ERROR(bFound);
        }

        break;
    case mmtDelete:
        KGLOG_PROCESS_ERROR(uDataLen == sizeof(KTONG_DELETE_MEMORABILIA));
        {
            KTONG_DELETE_MEMORABILIA*   pDelete = (KTONG_DELETE_MEMORABILIA*)pInfo;
            BOOL                        bFound  = false;
            for (KTong::KINCIDENT_TABLE::iterator it = pTong->m_Memorabilia.begin(); it != pTong->m_Memorabilia.end(); ++it)
            {
                if (it->dwID == pDelete->dwID)
                {
                    pTong->m_Memorabilia.erase(it);
                    bFound = true;
                    break;
                }
            }
            // 该记录已不存在的情况
            KG_PROCESS_ERROR(bFound);
        }

        break;
    }

    bResult = true;
Exit0:
    return bResult;
}

void KTongManager::ProcessModifyTongSchemaRequest(DWORD dwPlayerID, int nGroupIndex, int nModifyType, BYTE* pbyData, size_t uDataLen)
{
    BOOL            bRetCode    = false;
    KRole*          pRole       = NULL;
    KTong*          pTong       = NULL;
    KTongGroup*     pGroup      = NULL;
    KTongMember*    pMember     = NULL;

    assert(pbyData);
    KGLOG_PROCESS_ERROR(nGroupIndex >= 0 && nGroupIndex < MAX_TONG_GROUP);
    
    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);

    KG_PROCESS_ERROR(pRole->m_dwTongID != ERROR_ID);

    pTong = GetTong(pRole->m_dwTongID);
    KGLOG_PROCESS_ERROR(pTong);

    pMember = pTong->GetMember(dwPlayerID);
    KGLOG_PROCESS_ERROR(pMember);

    pGroup = &(pTong->m_Schema.Data[nGroupIndex]);
    assert(pGroup);

    switch (nModifyType)
    {
    case tmsGroupName:

        KGLOG_PROCESS_ERROR(uDataLen == sizeof(KTONG_MODIFY_GROUP_NAME));

        {
            KTONG_MODIFY_GROUP_NAME*            pModify    = (KTONG_MODIFY_GROUP_NAME*)pbyData;
            KTONG_CHANGE_GROUP_NAME_BROADCAST   ChangeInfo;

            bRetCode = pTong->CanAdvanceOperate(pMember->nGroupID, nGroupIndex, totModifyGroupName);
            if (!bRetCode)
            {
                SendTongEventNotify(pRole->m_nConnIndex, dwPlayerID, tenModifyGroupNameNoPermissionError);
                goto Exit0;
            }

            strncpy(ChangeInfo.szOldGroupName, pGroup->szName, sizeof(ChangeInfo.szOldGroupName));
            ChangeInfo.szOldGroupName[sizeof(ChangeInfo.szOldGroupName) - 1] = '\0';
            strncpy(ChangeInfo.szNewGroupName, pModify->szGroupName, sizeof(ChangeInfo.szNewGroupName));
            ChangeInfo.szNewGroupName[sizeof(ChangeInfo.szNewGroupName) - 1] = '\0';

            strncpy(pGroup->szName, pModify->szGroupName, sizeof(pGroup->szName));
            pGroup->szName[sizeof(pGroup->szName) - 1] = '\0';

            SendTongEventNotify(pRole->m_nConnIndex, dwPlayerID, tenModifyGroupNameSuccess);
            g_pSO3GameCenter->m_GameServer.DoTongBroadcastMessage(pTong, tbmGroupNameChange, ERROR_ID, (BYTE*)&ChangeInfo, sizeof(ChangeInfo));
        }
        break;

    case tmsBaseOperation:

        KGLOG_PROCESS_ERROR(uDataLen == sizeof(KTONG_MODIFY_BASE_OPERATION_MASK));

        {
            KTONG_MODIFY_BASE_OPERATION_MASK*   pModify = (KTONG_MODIFY_BASE_OPERATION_MASK*)pbyData;
            KTONG_CHANGE_GROUP_RIGHT_BROADCAST  ChangeInfo;
            
            KGLOG_PROCESS_ERROR(pModify->byOffset < TONG_OPERATION_NUM);

            bRetCode = pTong->CanBaseGrant(pMember->nGroupID, nGroupIndex, (int)pModify->byOffset);
            if (!bRetCode)
            {
                SendTongEventNotify(pRole->m_nConnIndex, dwPlayerID, tenModifyBaseOperationMaskNoPermissionError);
                goto Exit0;
            }

            if (pModify->bValue)
            {
                pGroup->byBaseOperationMask |= (1 << (pModify->byOffset));
            }
            else
            {
                pGroup->byBaseOperationMask &= ~(1 << (pModify->byOffset));
            }

            bRetCode = pTong->m_History.RecordChangeGroupBaseRight(
                dwPlayerID, nGroupIndex, pModify->byOffset, pModify->bValue
            );
            KGLOG_CHECK_ERROR(bRetCode);

            SendTongEventNotify(pRole->m_nConnIndex, dwPlayerID, tenModifyBaseOperationMaskSuccess);

            strncpy(ChangeInfo.szGroupName, pGroup->szName, sizeof(ChangeInfo.szGroupName));
            ChangeInfo.szGroupName[sizeof(ChangeInfo.szGroupName) - 1] = '\0';
            g_pSO3GameCenter->m_GameServer.DoTongBroadcastMessage(pTong, tbmGroupRightChange, ERROR_ID, (BYTE*)&ChangeInfo, sizeof(ChangeInfo));
        }

        break;

    case tmsAdvanceOperation:

        KGLOG_PROCESS_ERROR(uDataLen == sizeof(KTONG_MODIFY_ADVANCE_OPERATION_MASK));

        {
            KTONG_MODIFY_ADVANCE_OPERATION_MASK*    pModify = (KTONG_MODIFY_ADVANCE_OPERATION_MASK*)pbyData;
            KTONG_CHANGE_GROUP_RIGHT_BROADCAST      ChangeInfo;
            
            KGLOG_PROCESS_ERROR(pModify->byIndex < MAX_TONG_GROUP);
            KGLOG_PROCESS_ERROR(pModify->byOffset < TONG_OPERATION_NUM);

            bRetCode = pTong->CanAdvanceGrant(pMember->nGroupID, nGroupIndex, (int)pModify->byIndex, (int)pModify->byOffset);
            if (!bRetCode)
            {
                SendTongEventNotify(pRole->m_nConnIndex, dwPlayerID, tenModifyAdvanceOperationMaskNoPermissionError);
                goto Exit0;
            }

            if (pModify->bValue)
            {
                pGroup->byAdvanceOperationMask[pModify->byIndex] |= (1 << (pModify->byOffset));
            }
            else
            {
                pGroup->byAdvanceOperationMask[pModify->byIndex] &= ~(1 << (pModify->byOffset));
            }

            bRetCode = pTong->m_History.RecordChangeGroupAdvanceRight(
                dwPlayerID, nGroupIndex, pModify->byIndex, pModify->byOffset, pModify->bValue
            );
            KGLOG_CHECK_ERROR(bRetCode);

            SendTongEventNotify(pRole->m_nConnIndex, dwPlayerID, tenModifyAdvanceOperationMaskSuccess);
            strncpy(ChangeInfo.szGroupName, pGroup->szName, sizeof(ChangeInfo.szGroupName));
            ChangeInfo.szGroupName[sizeof(ChangeInfo.szGroupName) - 1] = '\0';
            g_pSO3GameCenter->m_GameServer.DoTongBroadcastMessage(pTong, tbmGroupRightChange, ERROR_ID, (BYTE*)&ChangeInfo, sizeof(ChangeInfo));
        }
        break;

    case tmsGroupWage:

        KG_PROCESS_ERROR(uDataLen == sizeof(int));

        {
            int                                 nOldWage    = 0;
            int                                 nWage       = 0;
            KTONG_CHANGE_GROUP_WAGE_BROADCAST   ChangeInfo;

            bRetCode = pTong->CanAdvanceOperate(pMember->nGroupID, nGroupIndex, totModifyWage);
            if (!bRetCode)
            {
                SendTongEventNotify(pRole->m_nConnIndex, dwPlayerID, tenModifyGroupWageNoPermissionError);
                goto Exit0;
            }

            memcpy(&nWage, pbyData, sizeof(nWage));
            KGLOG_PROCESS_ERROR(nWage >= 0);

            nOldWage        = pGroup->nWage;
            pGroup->nWage   = nWage;
            
            bRetCode = pTong->m_History.RecordChangeGroupWage(dwPlayerID, nGroupIndex, nOldWage, nWage);
            KGLOG_CHECK_ERROR(bRetCode);

            SendTongEventNotify(pRole->m_nConnIndex, dwPlayerID, tenModifyGroupWageSuccess);

            ChangeInfo.nWage = nWage;
            strncpy(ChangeInfo.szGroupName, pGroup->szName, sizeof(ChangeInfo.szGroupName));
            ChangeInfo.szGroupName[sizeof(ChangeInfo.szGroupName) - 1] = '\0';
            g_pSO3GameCenter->m_GameServer.DoTongBroadcastMessage(pTong, tbmGroupWageChange, ERROR_ID, (BYTE*)&ChangeInfo, sizeof(ChangeInfo));
        }

        break;

    default:
        KGLogPrintf(KGLOG_DEBUG, "[KTongManager] Process unrecognized Modify Tong Schema Request!\n");
        goto Exit0;
    }

    pTong->m_nLastUpdateFrameTable[tdtSchemaInfo] = g_pSO3GameCenter->m_nWorkLoop;
    pTong->m_bUpdateFlag = true;

Exit0:
    return;
}

void KTongManager::ProcessQuitRequest(DWORD dwPlayerID)
{
    BOOL                                bRetCode    = false;
    KRole*                              pRole       = NULL;
    KTong*                              pTong       = NULL;
    KTONG_MEMBER_JOIN_OR_QUIT_BROADCAST ChangeInfo;

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);

    if (pRole->m_dwTongID == ERROR_ID)
    {
        SendTongEventNotify(pRole->m_nConnIndex, dwPlayerID, tenQuitNotInTongError);
        goto Exit0;
    }

    pTong =  GetTong(pRole->m_dwTongID);
    KGLOG_PROCESS_ERROR(pTong);

    if (dwPlayerID == pTong->m_dwMaster)
    {
        SendTongEventNotify(pRole->m_nConnIndex, dwPlayerID, tenQuitIsMasterError);
        goto Exit0;
    }

    bRetCode = pTong->RemoveMember(pRole);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = pTong->m_History.RecordQuit(dwPlayerID);
    KGLOG_CHECK_ERROR(bRetCode);

    assert(pRole->m_dwTongID == ERROR_ID);
    g_pSO3GameCenter->m_GameServer.DoChangePlayerTongNotify(pRole->m_nConnIndex, pRole->m_dwPlayerID, pRole->m_dwTongID, ctrQuit, pTong->m_szName);

    strncpy(ChangeInfo.szMemberName, pRole->m_szName, sizeof(ChangeInfo.szMemberName));
    ChangeInfo.szMemberName[sizeof(ChangeInfo.szMemberName) - 1] = '\0';
    g_pSO3GameCenter->m_GameServer.DoTongBroadcastMessage(pTong, tbmMemberQuit, ERROR_ID, (BYTE*)&ChangeInfo, sizeof(ChangeInfo));
Exit0:
    return;
}

void KTongManager::ProcessChangeMemberGroupRequest(DWORD dwPlayerID, DWORD dwTargetID, int nTargetGroup)
{
    BOOL                                bRetCode            = false;
    BOOL                                bHavePermissoion    = false;
    KRole*                              pRole               = NULL;
    KRole*                              pTargetRole         = NULL;
    KTong*                              pTong               = NULL;
    KTongMember*                        pMember             = NULL;
    KTongMember*                        pTargetMember       = NULL;
    int                                 nCurrentGroup       = 0;
    KTONG_MEMBER_CHANGE_GROUP_BROADCAST ChangeInfo;

    KGLOG_PROCESS_ERROR(nTargetGroup >= 0 && nTargetGroup < MAX_TONG_GROUP);

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);

    pTargetRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwTargetID);
    KGLOG_PROCESS_ERROR(pTargetRole);

    KG_PROCESS_ERROR(pRole->m_dwTongID != ERROR_ID);
    
    pTong = GetTong(pRole->m_dwTongID);
    KGLOG_PROCESS_ERROR(pTong);

    KG_PROCESS_ERROR(pTong->m_dwMaster != dwTargetID);
    KGLOG_PROCESS_ERROR(pTong->m_Schema.Data[nTargetGroup].bEnable);

    pMember = pTong->GetMember(dwPlayerID);
    KGLOG_PROCESS_ERROR(pMember);

    pTargetMember = pTong->GetMember(dwTargetID);
    KGLOG_PROCESS_ERROR(pTargetMember);

    nCurrentGroup = pTargetMember->nGroupID;
    KGLOG_PROCESS_ERROR(nCurrentGroup >= 0 && nCurrentGroup < MAX_TONG_GROUP);
    KG_PROCESS_ERROR(nCurrentGroup != nTargetGroup);

    bRetCode = pTong->CanAdvanceOperate(pMember->nGroupID, nCurrentGroup, totMoveFromGroup);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = pTong->CanAdvanceOperate(pMember->nGroupID, nTargetGroup, totAddToGroup);
    KGLOG_PROCESS_ERROR(bRetCode);

    bHavePermissoion = true;

    pTargetMember->nGroupID = nTargetGroup;
    pTargetMember->nLastUpdateFrame = g_pSO3GameCenter->m_nWorkLoop;

    bRetCode = pTong->m_History.RecordChangeMemberGroup(dwPlayerID, dwTargetID, nCurrentGroup, nTargetGroup);
    KGLOG_CHECK_ERROR(bRetCode);

    strncpy(ChangeInfo.szMemberName, pTargetRole->m_szName, sizeof(pTargetRole->m_szName));
    ChangeInfo.szMemberName[sizeof(ChangeInfo.szMemberName) - 1] = '\0';
    strncpy(ChangeInfo.szOldGroupName, pTong->m_Schema.Data[nCurrentGroup].szName, sizeof(ChangeInfo.szOldGroupName));
    ChangeInfo.szOldGroupName[sizeof(ChangeInfo.szOldGroupName) - 1] = '\0';
    strncpy(ChangeInfo.szNewGroupName, pTong->m_Schema.Data[nTargetGroup].szName, sizeof(ChangeInfo.szNewGroupName));
    ChangeInfo.szNewGroupName[sizeof(ChangeInfo.szNewGroupName) - 1] = '\0';

    SendTongEventNotify(pRole->m_nConnIndex, dwPlayerID, tenChangeMemberGroupSuccess);
    g_pSO3GameCenter->m_GameServer.DoTongBroadcastMessage(pTong, tbmMemberChangeGroup, ERROR_ID, (BYTE*)&ChangeInfo, sizeof(ChangeInfo));

    pTong->m_bUpdateFlag = true;

Exit0:
    if (!bHavePermissoion && pRole)
    {
        SendTongEventNotify(pRole->m_nConnIndex, dwPlayerID, tenChangeMemberGroupError);
    }

    return;
}

void KTongManager::ProcessChangeMasterRequest(DWORD dwPlayerID, DWORD dwNewMasterID)
{
    KTong*                          pTong       = NULL;
    KRole*                          pRole       = NULL;
    KRole*                          pTargetRole = NULL;
    KTongMember*                    pMember     = NULL;
    KTongMember*                    pNewMaster  = NULL;
    KTONG_CHANGE_MASTER_BROADCAST   ChangeInfo;
    
    KGLOG_PROCESS_ERROR(dwPlayerID != dwNewMasterID);

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);
    KGLOG_PROCESS_ERROR(pRole->m_dwTongID != ERROR_ID);

    pTargetRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwNewMasterID);
    KGLOG_PROCESS_ERROR(pTargetRole);
    KGLOG_PROCESS_ERROR(pTargetRole->m_dwTongID == pRole->m_dwTongID);
    
    pTong = GetTong(pRole->m_dwTongID);
    if (dwPlayerID != pTong->m_dwMaster)
    {
        SendTongEventNotify(pRole->m_nConnIndex, dwPlayerID, tenChangeMasterError);
        goto Exit0;
    }

    pMember = pTong->GetMember(dwPlayerID);
    KGLOG_PROCESS_ERROR(pMember);

    pNewMaster = pTong->GetMember(dwNewMasterID);
    KGLOG_PROCESS_ERROR(pNewMaster);

    strncpy(ChangeInfo.szOldMasterName, pRole->m_szName, sizeof(ChangeInfo.szOldMasterName));
    ChangeInfo.szOldMasterName[sizeof(ChangeInfo.szOldMasterName) - 1] = '\0';
    strncpy(ChangeInfo.szNewMasterName, pTargetRole->m_szName,sizeof(ChangeInfo.szNewMasterName));
    ChangeInfo.szNewMasterName[sizeof(ChangeInfo.szNewMasterName) - 1] = '\0';

    pNewMaster->nGroupID    = pTong->m_Schema.nMasterGroup;
    pMember->nGroupID       = pTong->m_Schema.nDefaultGroup;
    pTong->m_dwMaster       = dwNewMasterID;

    pMember->nLastUpdateFrame       = g_pSO3GameCenter->m_nWorkLoop;
    pNewMaster->nLastUpdateFrame    = g_pSO3GameCenter->m_nWorkLoop;

    pTong->m_nLastUpdateFrameTable[tdtBaseInfo] = g_pSO3GameCenter->m_nWorkLoop;

    SendTongEventNotify(pRole->m_nConnIndex, dwPlayerID, tenChangeMasterSuccess);
    g_pSO3GameCenter->m_GameServer.DoTongBroadcastMessage(pTong, tbmMasterChange, ERROR_ID, (BYTE*)&ChangeInfo, sizeof(ChangeInfo));

    pTong->m_bUpdateFlag = true;

Exit0:
    return;
}

void KTongManager::ProcessChangeTongMemberRemarkRequest(DWORD dwPlayerID, DWORD dwTargetID, char szRemark[])
{
    BOOL            bRetCode        = false;
    KTong*          pTong           = NULL;
    KRole*          pRole           = NULL;
    KTongMember*    pCurMember      = NULL;
    KTongMember*    pTargetMember   = NULL;

    assert(szRemark);

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);
    KGLOG_PROCESS_ERROR(pRole->m_dwTongID != ERROR_ID);

    pTong = GetTong(pRole->m_dwTongID);
    KGLOG_PROCESS_ERROR(pTong);

    pCurMember = pTong->GetMember(dwPlayerID);
    KGLOG_PROCESS_ERROR(pCurMember);

    pTargetMember = pTong->GetMember(dwTargetID);
    KGLOG_PROCESS_ERROR(pTargetMember);

    bRetCode = pTong->CanAdvanceOperate(pCurMember->nGroupID, pTargetMember->nGroupID, totModifyMemberRemark);
    if (!bRetCode)
    {
        SendTongEventNotify(pRole->m_nConnIndex, dwPlayerID, tenChangeMemberRemarkError);
        goto Exit0;
    }

    strncpy(pTargetMember->szRemark, szRemark, sizeof(pTargetMember->szRemark));
    pTargetMember->nLastUpdateFrame = g_pSO3GameCenter->m_nWorkLoop;

    SendTongEventNotify(pRole->m_nConnIndex, dwPlayerID, tenChangeMemberRemarkSuccess);

    pTong->m_bUpdateFlag = true;

Exit0:
    return;
}

void KTongManager::ProcessSaveMoneyInTong(KRole* pRole, int nMoney)
{
    BOOL    bResult     = false;
    BOOL    bRetCode    = false;
    int     nSilver     = 0;
    KTong*  pTong       = NULL;
    BYTE    byBuffer[32];

    assert(pRole);
    assert(nMoney >= MONEY_CONVERSION_SILVER_RATE);
    assert(nMoney % MONEY_CONVERSION_SILVER_RATE == 0);

    KG_PROCESS_ERROR(pRole->m_dwTongID != ERROR_ID);

    pTong = GetTong(pRole->m_dwTongID);
    KGLOG_PROCESS_ERROR(pTong);

    nSilver = nMoney / MONEY_CONVERSION_SILVER_RATE;

    if (nSilver > MAX_TONG_FUND_BY_SILVER - pTong->m_nFund)
    {
        SendTongEventNotify(pRole->m_nConnIndex, pRole->m_dwPlayerID, tenSaveMoneyTooMushError, pTong->m_nFund);
        goto Exit0;
    }

    bRetCode = pTong->AddFund(nSilver);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = pTong->m_History.RecordSaveFund(pRole->m_dwPlayerID, nSilver);
    KGLOG_CHECK_ERROR(bRetCode);

    SendTongEventNotify(pRole->m_nConnIndex, pRole->m_dwPlayerID, tenSaveMoneySuccess, pTong->m_nFund);
    
    bResult = true;
Exit0:
    if (!bResult)
    {
        byBuffer[0] = '\0'; // 信件正文部分至少以1个'\0'结尾
        g_pSO3GameCenter->m_MailManager.PostRequestSendMail(
            0, pRole->m_dwPlayerID, 0, eMailType_System, 
            g_pSO3GameCenter->m_Settings.m_szSystemMailSender, 
            g_pSO3GameCenter->m_Settings.m_szTongReturnMoneyMailTitle,
            nMoney,
            byBuffer, 1,
            NULL, 0, 0
        );
    }
    return;
}

void KTongManager::ProcessRenameConflictTong(
    int nConnIndex, DWORD dwPlayerID, DWORD dwTongID, const char cszNewName[]
)
{
    BOOL        bRetCode        = false;
    KTong*      pTong           = NULL;
    KTong*      pConflictTong   = NULL;
    size_t      uNewNameLen     = 0;

    KGLOG_PROCESS_ERROR(dwPlayerID != ERROR_ID);
    KGLOG_PROCESS_ERROR(dwTongID != ERROR_ID);
    KGLOG_PROCESS_ERROR(cszNewName);

    uNewNameLen = strlen(cszNewName);
    KGLOG_PROCESS_ERROR(uNewNameLen < _NAME_LEN);

    pTong = g_pSO3GameCenter->m_TongManager.GetTong(dwTongID);
    KGLOG_PROCESS_ERROR(pTong); 

    if (pTong->m_dwMaster != dwPlayerID)
    {
        SendTongEventNotify(nConnIndex, dwPlayerID, tenRenameNoRightError);
        goto Exit0;
    }

    bRetCode = g_pSO3GameCenter->m_piTextFilter->Check(pTong->m_szName);
    if (bRetCode)
    {
        SendTongEventNotify(nConnIndex, dwPlayerID, tenRenameUnnecessaryError);
        goto Exit0;
    }

    pConflictTong = g_pSO3GameCenter->m_TongManager.GetTongByName(cszNewName);
    if (pConflictTong != NULL)
    {
        SendTongEventNotify(nConnIndex, dwPlayerID, tenRenameConflictError);
        goto Exit0;
    }

    bRetCode = g_pSO3GameCenter->m_piTextFilter->Check(cszNewName);
    if (!bRetCode)
    {
        SendTongEventNotify(nConnIndex, dwPlayerID, tenRenameIllegalError);
        goto Exit0;
    }

    strncpy(pTong->m_szName, cszNewName, sizeof(pTong->m_szName));
    pTong->m_szName[sizeof(pTong->m_szName) - 1] = '\0';
    pTong->m_nLastUpdateFrameTable[tdtBaseInfo] = g_pSO3GameCenter->m_nWorkLoop;

    pTong->m_bUpdateFlag = true;

    SendTongEventNotify(nConnIndex, dwPlayerID, tenRenameSuccess);

Exit0:
    return;
}

void KTongManager::OnPlayerEnterGame(KRole* pRole)
{
    KTong*                                  pTong       = NULL;
    KTongMember*                            pMember     = NULL;

    assert(pRole);
    
    if (pRole->m_dwTongID != ERROR_ID)
    {
        KTONG_MEMBER_ONLINE_STATE_CHANGE_BROADCAST Broadcast;
        pTong = GetTong(pRole->m_dwTongID);
        KGLOG_PROCESS_ERROR(pTong);

        pMember = pTong->GetMember(pRole->m_dwPlayerID);
        KGLOG_PROCESS_ERROR(pMember);

        pMember->bIsOnline          = true;
        pMember->nLastUpdateFrame   = g_pSO3GameCenter->m_nWorkLoop;
        // 无需存盘

        strncpy(Broadcast.szMemberName, pRole->m_szName, sizeof(Broadcast.szMemberName));
        Broadcast.szMemberName[sizeof(Broadcast.szMemberName) - 1] = '\0';
        g_pSO3GameCenter->m_GameServer.DoTongBroadcastMessage(pTong, tbmMemberOnline, pRole->m_dwPlayerID, 
            (BYTE*)&Broadcast, sizeof(Broadcast)
        );
    }
Exit0:
    return;
}

void KTongManager::OnPlayerLeaveGame(KRole* pRole)
{
    KTong*                                  pTong       = NULL;
    KTongMember*                            pMember     = NULL;
    DWORD                                   dwInviterID = 0;
    KRole*                                  pInviter    = NULL;
    KTONG_JOIN_APPLICATION_TABLE::iterator  it;

    assert(pRole);
    
    if (pRole->m_dwTongID != ERROR_ID)
    {
        KTONG_MEMBER_ONLINE_STATE_CHANGE_BROADCAST Broadcast;

        pTong = GetTong(pRole->m_dwTongID);
        KGLOG_PROCESS_ERROR(pTong);

        pMember = pTong->GetMember(pRole->m_dwPlayerID);
        KGLOG_PROCESS_ERROR(pMember);

        pMember->nLastOfflineTime   = g_pSO3GameCenter->m_nTimeNow;
        pMember->nLastUpdateFrame   = g_pSO3GameCenter->m_nWorkLoop;

        pTong->m_bUpdateFlag        = true;

        strncpy(Broadcast.szMemberName, pRole->m_szName, sizeof(Broadcast.szMemberName));
        Broadcast.szMemberName[sizeof(Broadcast.szMemberName) - 1] = '\0';
        g_pSO3GameCenter->m_GameServer.DoTongBroadcastMessage(pTong, tbmMemberOffline, pRole->m_dwPlayerID, 
            (BYTE*)&Broadcast, sizeof(Broadcast)
        );
    }
    else
    {
        it = m_InviteApplication.find(pRole->m_dwPlayerID);
        KG_PROCESS_ERROR(it != m_InviteApplication.end());

        dwInviterID = it->second.InviterID;
        m_InviteApplication.erase(it++);

        pInviter = g_pSO3GameCenter->m_RoleManager.GetRole(dwInviterID);
        KG_PROCESS_ERROR(pInviter);

        if (pInviter->IsOnline())
        {
            SendTongEventNotify(
                pInviter->m_nConnIndex, pInviter->m_dwPlayerID, tenInviteTargetOfflineError, pRole->m_szName
            );
        }        
    }

Exit0:
    return;
}

void KTongManager::SaveAll()
{
    for (KTONG_TABLE::iterator it = m_TongTable.begin(); it != m_TongTable.end(); ++it)
    {
        KTong* pTong = &it->second;       
        
        if (!pTong->m_bUpdateFlag)
           continue;

        g_pSO3GameCenter->m_MiscDB.DoSaveTongData(it->first, pTong);
        pTong->m_bUpdateFlag = false;
    }
}

void KTongManager::SendTongEventNotify(int nConnIndex, DWORD dwPlayerID, TONG_EVENT_NOTIFY_CODE eCode)
{
    g_pSO3GameCenter->m_RemoteScript.Reset();
    g_pSO3GameCenter->m_RemoteScript.PushNumber(dwPlayerID);
    g_pSO3GameCenter->m_RemoteScript.PushNumber(eCode);
    g_pSO3GameCenter->m_RemoteScript.Call(nConnIndex, TONG_EVENT_NOTIFY_FUNCTION);
}

void KTongManager::SendTongEventNotify(int nConnIndex, DWORD dwPlayerID, TONG_EVENT_NOTIFY_CODE eCode, const char cszMsg[])
{
    g_pSO3GameCenter->m_RemoteScript.Reset();
    g_pSO3GameCenter->m_RemoteScript.PushNumber(dwPlayerID);
    g_pSO3GameCenter->m_RemoteScript.PushNumber(eCode);
    g_pSO3GameCenter->m_RemoteScript.PushString(cszMsg);
    g_pSO3GameCenter->m_RemoteScript.Call(nConnIndex, TONG_EVENT_NOTIFY_FUNCTION);
}

void KTongManager::SendTongEventNotify(int nConnIndex, DWORD dwPlayerID, TONG_EVENT_NOTIFY_CODE eCode, int nParam)
{
    g_pSO3GameCenter->m_RemoteScript.Reset();
    g_pSO3GameCenter->m_RemoteScript.PushNumber(dwPlayerID);
    g_pSO3GameCenter->m_RemoteScript.PushNumber(eCode);
    g_pSO3GameCenter->m_RemoteScript.PushNumber(nParam);
    g_pSO3GameCenter->m_RemoteScript.Call(nConnIndex, TONG_EVENT_NOTIFY_FUNCTION);
}

BOOL KTongManager::LoadOperationConfig()
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

BOOL KTongManager::LoadSchemaTemplate()
{
    BOOL                    bResult     = false;
    IFile*                  piFile      = NULL;
    size_t                  uFileSize   = 0;
    size_t                  uReadBytes  = 0;
    char                    szFileName[PATH_MAX];
    KTongSchemaFileHeader   FileHeader;

    snprintf(szFileName, sizeof(szFileName), "%s/TongTemplate.dat", SETTING_DIR);
    szFileName[sizeof(szFileName) - 1] = '\0';

    piFile = g_OpenFile(szFileName);
    KGLOG_PROCESS_ERROR(piFile);

    uFileSize = piFile->Size();
    KGLOG_PROCESS_ERROR(uFileSize >= sizeof(KTongSchemaFileHeader));

    uReadBytes = piFile->Read(&FileHeader, sizeof(FileHeader));
    KGLOG_PROCESS_ERROR(uReadBytes == sizeof(FileHeader));

    KGLOG_PROCESS_ERROR(FileHeader.nVersion == TONG_SCHEMA_FILE_VERSION);
    KGLOG_PROCESS_ERROR(FileHeader.nCount >= 0 && FileHeader.nCount <= MAX_TONG_SCHEMA_TEMPLATE);
    KGLOG_PROCESS_ERROR(uFileSize == sizeof(KTongSchemaFileHeader) + sizeof(KTongSchemaData) * FileHeader.nCount);

    m_SchemaTemplate.resize(FileHeader.nCount);

    uReadBytes = piFile->Read(&m_SchemaTemplate[0], sizeof(KTongSchemaData) * FileHeader.nCount);
    KGLOG_PROCESS_ERROR(uReadBytes == sizeof(KTongSchemaData) * FileHeader.nCount);

    for (int i = 0; i < FileHeader.nCount; i++)
    {
        KTongSchemaData* pTemplate = &m_SchemaTemplate[i];
        
        KGLOG_PROCESS_ERROR(pTemplate->nMasterGroup >= 0);
        KGLOG_PROCESS_ERROR(pTemplate->nMasterGroup < MAX_TONG_GROUP);
        KGLOG_PROCESS_ERROR(pTemplate->Data[pTemplate->nMasterGroup].bEnable);

        KGLOG_PROCESS_ERROR(pTemplate->nDefaultGroup >= 0);
        KGLOG_PROCESS_ERROR(pTemplate->nDefaultGroup < MAX_TONG_GROUP);
        KGLOG_PROCESS_ERROR(pTemplate->Data[pTemplate->nDefaultGroup].bEnable);

        // 预处理,防止模板数据错误,字符串溢出
        for (int j = 0; j < MAX_TONG_GROUP; j++)
        {
            KTongGroup* pGroup = &pTemplate->Data[j];

            pGroup->szName[sizeof(pGroup->szName) - 1] = '\0';
        }
    }

    bResult = true;
Exit0:
    KG_COM_RELEASE(piFile);
    return bResult;
}

BOOL KTongManager::OnJoinTong(DWORD dwPlayerID, DWORD dwTongID)
{
    BOOL            bResult         = false;
    BOOL            bRetCode        = false;
    int             nLuaTopIndex    = 0;
    KScriptManager* pScriptManager  = NULL;

    assert(g_pSO3GameCenter);
    pScriptManager = &g_pSO3GameCenter->m_ScriptManager;

    bRetCode = pScriptManager->IsFuncExist(TONG_SCRIPTS, JOIN_TONG_FUNCTION);
    KGLOG_PROCESS_ERROR(bRetCode);

    pScriptManager->SafeCallBegin(&nLuaTopIndex);

    pScriptManager->Push(dwPlayerID);
    pScriptManager->Push(dwTongID);

    pScriptManager->Call(TONG_SCRIPTS, JOIN_TONG_FUNCTION, 0);

    pScriptManager->SafeCallEnd(nLuaTopIndex);
    bResult = true;
Exit0:
    if (!bResult)
    {
        KGLogPrintf(
            KGLOG_INFO, "Failed to Call Scripts:%s function:%s RoleID:%lu TongID:%lu\n", 
            TONG_SCRIPTS, JOIN_TONG_FUNCTION, dwPlayerID, dwTongID
        );
    }

    return bResult;
}
