#include "StdAfx.h"
#include "KGameCenterWrapper.h"
#include "TestTools.h"

#include "KSO3GameCenter.h"
#include "KGatewaySimulator.h"
#include "KGameServerSimulator.h"
#include "NameGenerator.h"
#include "TestDef.h"


BOOL TestTools::PlayerEnterGame(KGameServerSimulator* pGS, KRoleInfo* pRoleInfo, DWORD dwMapID, __int64 nMapCopyID, DWORD dwQueue)
{
    BOOL	  bResult	 = false;
    int		  nRetCode	 = false;
    int       nPlayerIndex = 0;

    ASSERT(pGS);
    ASSERT(pRoleInfo);
    KROLE_LIST_INFO BaseData;

    strncpy(BaseData.szAccount, pRoleInfo->szAccount, sizeof(BaseData.szAccount));
    strncpy(BaseData.szRoleName, pRoleInfo->szRoleName, sizeof(BaseData.szRoleName));
    BaseData.dwPlayerID = pRoleInfo->dwPlayerID;
    BaseData.BaseInfo.CurrentPos.dwMapID = dwMapID;
    BaseData.BaseInfo.CurrentPos.nMapCopyIndex = nMapCopyID;

	KGLogPrintf(KGLOG_INFO, "%d %s %s", BaseData.dwPlayerID, BaseData.szAccount, BaseData.szRoleName);
    nRetCode = GATEWAY->DoGameLoginRequest(pRoleInfo->szAccount, pRoleInfo->dwPlayerID, BaseData, dwQueue);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = pGS->CheckPlayerLoginRequset(pRoleInfo->dwPlayerID, dwMapID, nMapCopyID);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = pGS->DoPlayerLoginRespond(pRoleInfo->dwPlayerID, TRUE);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = GATEWAY->CheckReplyLoginData(pRoleInfo->dwPlayerID, LOGIN_GAME_SUCCESS);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = pGS->DoConfirmPlayerLoginRequest(pRoleInfo->dwPlayerID);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = pGS->CheckConfirmPlayerLoginRespond(pRoleInfo->dwPlayerID, true);
    KGLOG_PROCESS_ERROR(nRetCode);

    pRoleInfo->pLoginOnGS = pGS;

    bResult = true;
Exit0:
    return bResult;
}
//
BOOL TestTools::PlayerLeaveGame(KRoleInfo* pRoleInfo)
{
    BOOL	  bResult		       = false;                        
    int		  nRetCode			   = false;

    ASSERT(pRoleInfo && pRoleInfo->pLoginOnGS);

    nRetCode = pRoleInfo->pLoginOnGS->DoPlayerLeaveGS(pRoleInfo->dwPlayerID);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = GATEWAY->CheckPlayerLeaveGame(pRoleInfo->szAccount);
    KGLOG_PROCESS_ERROR(nRetCode);

    pRoleInfo->pLoginOnGS = NULL;

    bResult = true;
Exit0:
    return bResult;
}

BOOL TestTools::CreateRole(KRoleInfo* pRoleInfo)
{
    int nResult = false;
    int nRetCode = false;
    KUSER_ROLE_GENERATOR_PARAM UserRoleParam;
    unsigned long ulIdentity = 100;
    IKG_Buffer* pRoleData = NULL;
    KROLE_LIST_INFO  BaseData;

    memset(&UserRoleParam, 0, sizeof(UserRoleParam));
    UserRoleParam.cRoleType = rtStandardMale;
	UserRoleParam.dwMapID = GC->GetHometownMapTable()[0];
    UserRoleParam.nMapCopyIndex = 0;

    strncpy(UserRoleParam.szRoleName, pRoleInfo->szRoleName, sizeof(UserRoleParam.szRoleName));
    nRetCode = GATEWAY->DoCreateRoleRequest(pRoleInfo->szAccount, UserRoleParam);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = GATEWAY->CheckCreateRoleRespond(eCreateRoleSucceed, &BaseData);
    KGLOG_PROCESS_ERROR(nRetCode);
    
    pRoleInfo->dwPlayerID = BaseData.dwPlayerID;

    nResult = true;
Exit0:
    KG_COM_RELEASE(pRoleData);
    return nResult;
}
//
//BOOL TestTools::SetRoleDBData(KRoleInfo* pRoleInfo, KGameServerSimulator* pGS, DWORD dwMapID, __int64 nMapCopyID)
//{
//    int nRetCode = false;
//    int nResult = false;
//    //KRoleBaseInfo BaseInfo;
//
//    //ASSERT(pRoleInfo);
//    //ASSERT(pGS);
//
//    //KROLE_LIST_INFO BaseData;
//    //strncpy(BaseData.szAccount, pRoleInfo->szAccount, sizeof(BaseData.szAccount));
//    //strncpy(BaseData.szRoleName, pRoleInfo->szRoleName, sizeof(BaseData.szRoleName));
//    //BaseData.dwPlayerID = pRoleInfo->dwPlayerID;
//    //BaseData.BaseInfo.CurrentPos.dwMapID = dwMapID;
//    //BaseData.BaseInfo.CurrentPos.nMapCopyIndex = nMapCopyID;
//
//    //unsigned int uSize = /*sizeof(KRoleDBData) + sizeof(KRoleDataBlock)*/0;
//    //BYTE* pByte = new BYTE[uSize];
//
//    //KRoleDBData* pRoleDBData = (KRoleDBData*)pByte;
//    //pRoleDBData->wVersion = rdvCurrentRoleDataVersion;
//    //pRoleDBData->wBlockCount = 1;
//    //pRoleDBData->dwDataLen = uSize;
//    //pRoleDBData->dwCRC = 0;
//    //pRoleDBData->dwCRC = CRC32(0, pRoleDBData, pRoleDBData->dwDataLen);
//
//    //nRetCode = pGS->DoSaveRoleData(BaseData.dwPlayerID, 5, BaseInfo, uSize, pByte);
//    //KG_PROCESS_ERROR(nRetCode);
//
//    //nRetCode = pGS->CheckSaveRoleDataRespond(BaseData.dwPlayerID, true, 5);
//    //KG_PROCESS_ERROR(nRetCode);
//
//    nResult = true;
//Exit0:
//    //SAFE_DELETE_ARRAY(pByte);
//    return nResult;
//}
//
//void TestTools::SetHighPerforGS(KGSList& GSList, unsigned int uIndex)
//{
//    for (int i = 0 ; i < (int)GSList.size(); i++)
//    {
//        if (GSList[i] == NULL)
//        {
//            continue;
//        }
//        if (i == (int)uIndex)
//        {
//            GSList[i]->DoUpdatePerformance(2000, 1, 1);
//        }
//        else
//        {
//            GSList[i]->DoUpdatePerformance(1009, 10000, 100);
//        }
//    }
//}
//


bool TestTools::CreateParty(KRoleInfo* pLeader, KRoleInfo* pMember)
{
	bool  bResult   = false;
	int   nRetCode  = false;
	DWORD dwPartyID = 0;
	DWORD dwCreatedPartyID = 0;
	GUID  Guid      = GUID();

	KG_PROCESS_ERROR(pLeader);
	KG_PROCESS_ERROR(pMember);
	KG_PROCESS_ERROR(pLeader->dwPartyID == ERROR_ID);
	KG_PROCESS_ERROR(pMember->dwPartyID == ERROR_ID);

	nRetCode = pLeader->pLoginOnGS->DoInvitePlayerJoinPartyRequest(
		pLeader->dwPlayerID, pMember->szRoleName);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = pMember->pLoginOnGS->CheckInvitePlayerJoinPartyRequest(
		pLeader->szRoleName, pMember->dwPlayerID, Guid);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = pMember->pLoginOnGS->DoInvitePlayerJoinPartyRespond(
		pLeader->szRoleName, pMember->dwPlayerID, TRUE, Guid);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = pLeader->pLoginOnGS->CheckPartyCreateNotify(&dwCreatedPartyID, pLeader->dwPlayerID, pLeader->szRoleName);
	KGLOG_PROCESS_ERROR(nRetCode);

	KGLOG_PROCESS_ERROR(GC->GetRolePartyID(pLeader->dwPlayerID) == dwCreatedPartyID);
	KGLOG_PROCESS_ERROR(GC->GetRolePartyID(pMember->dwPlayerID) == dwCreatedPartyID);
	
	pLeader->dwPartyID = dwCreatedPartyID;
	pMember->dwPartyID = dwCreatedPartyID;

	bResult = true;
Exit0:
	return bResult;
}
//
bool TestTools::DisbandParty(KRoleInfo* pLeader)
{
	bool  bResult		  = false;
	int   nRetCode		  = false;

	KG_PROCESS_ERROR(pLeader->dwPartyID != ERROR_ID);

	nRetCode = pLeader->pLoginOnGS->DoPartyDisbandRequest(pLeader->dwPlayerID, pLeader->dwPartyID);
	KG_PROCESS_ERROR(nRetCode);
	
	nRetCode = pLeader->pLoginOnGS->CheckPartyDisbandNotify(pLeader->dwPartyID);
	KG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	return bResult;
}
//
//
bool TestTools::JoinParty(KRoleInfo* pLeader, KRoleInfo* pJoiner)
{
	bool  bResult  = false;
	int   nRetCode = false;
	DWORD dwPartyID = 0;
	GUID  Guid     = GUID();

	KGLOG_PROCESS_ERROR(pJoiner->dwPartyID == ERROR_ID);

	nRetCode = pJoiner->pLoginOnGS->DoApplyJoinPartyRequest(pJoiner->dwPlayerID, pLeader->szRoleName);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = pLeader->pLoginOnGS->CheckApplyJoinPartyRequest(
		pLeader->dwPlayerID, pJoiner->szRoleName, Guid);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = pLeader->pLoginOnGS->DoApplyJoinPartyRespond(
		pJoiner->szRoleName, pLeader->dwPlayerID, TRUE, Guid);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = pJoiner->pLoginOnGS->CheckPartyAddMemberNotify(pLeader->dwPartyID, pJoiner->dwPlayerID);
	KGLOG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	return bResult;
}
//
//bool TestTools::CreateMap(KSO3GameCenter* pGC, DWORD dwMapID)
//{
//	BOOL                        bResult         = false;
//	BOOL                        bRetCode        = false;
//	KMapInfo*                   pMapInfo        = NULL;
//	KMapCopy*                   pMapCopy        = NULL;
//	int                         nCopySN         = 0;
//	int                         nDestConnIndex  = 0;
//	time_t                      nTimeNow        = 0;
//	int64_t                     nMapCopyIndex   = 0;
//
//	MutextHelper mh(&g_Mutex);
//	pMapInfo = pGC->m_MapManager.GetMapInfo(dwMapID);
//	KGLOG_PROCESS_ERROR(pMapInfo);
//
//	//nTimeNow        = pMapInfo ? g_pSO3GameCenter->m_nTimeNow : 0;
//	nCopySN         = pMapInfo->m_nSNGenerator++;
//	nMapCopyIndex   = MAKE_INT64(nTimeNow, nCopySN);
//
//	nDestConnIndex = pGC->m_GameServer.GetBestGSConnectionForCreateMap(pMapInfo);
//	KG_PROCESS_ERROR(nDestConnIndex);
//
//	pMapCopy = pMapInfo->CreateMapCopy(
//		nMapCopyIndex, g_pSO3GameCenter->m_nTimeNow, 0, 
//		nDestConnIndex, true
//		);
//	KGLOG_PROCESS_ERROR(pMapCopy);
//
//	g_pSO3GameCenter->m_GameServer.DoCreateMapNotify(nDestConnIndex, dwMapID, nMapCopyIndex);
//
//	pMapCopy->m_eState = eMapStateCreating;
//
//	bResult = true;
//Exit0:
//	return bResult;
//}
//
bool TestTools::DeleteRole(KRoleInfo* pDeleteRole)
{
	bool nResult = false;
	int nRetCode = false;

	nRetCode = GATEWAY->DoDeleteRoleRequest(pDeleteRole->dwPlayerID);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = GATEWAY->CheckDeleteRoleRespond(pDeleteRole->dwPlayerID, true);
	KG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

BOOL TestTools::PlayerApplyJoinParty(KRoleInfo* pApplySrc, KRoleInfo* pApplyDesLeader)
{
	BOOL  bResult  = false;
	int   nRetCode = false;
	GUID  Guid     = GUID();

	nRetCode = pApplySrc->pLoginOnGS->DoApplyJoinPartyRequest(pApplySrc->dwPlayerID, pApplyDesLeader->szRoleName);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = pApplyDesLeader->pLoginOnGS->CheckApplyJoinPartyRequest(pApplyDesLeader->dwPlayerID, pApplySrc->szRoleName, Guid);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = pApplyDesLeader->pLoginOnGS->DoApplyJoinPartyRespond(pApplySrc->szRoleName, pApplyDesLeader->dwPlayerID,
		TRUE, Guid);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = pApplySrc->pLoginOnGS->CheckPartyAddMemberNotify(pApplyDesLeader->dwPartyID, pApplySrc->dwPlayerID);
	KG_PROCESS_ERROR(nRetCode);

	pApplySrc->dwPartyID = pApplyDesLeader->dwPartyID;

	bResult = true;
Exit0:
	return bResult;
}


TestTools::KRoleInfo TestTools::CreateRole()
{
	BOOL          bRetCode            = false;
	KRoleInfo  RoleInfo;
	SocpeLock(GC->Mutex());
	unsigned long long  ullCurrentRoleID    = 0;
	bRetCode = GC->Center()->m_piSequence->GenerateID("RoleID", 1, &ullCurrentRoleID);
	if (!bRetCode)
	{
		return RoleInfo;
	}

	std::string RoleName  = NameGenerator::Instance()->LegalName(3);
	std::string AcountName = RoleName + "test";
	KRole* pRole = GC->Center()->m_RoleManager.AddRole(AcountName.c_str(), RoleName.c_str(), ullCurrentRoleID, 1, 1,cNeutral, 0, 
		GC->Center()->m_nTimeNow, GC->Center()->m_nTimeNow);
	if (!pRole)
	{
		return RoleInfo;
	}
	RoleInfo.dwPlayerID = ullCurrentRoleID;
	strncpy(RoleInfo.szAccount, AcountName.c_str(), sizeof(RoleInfo.szAccount));
	strncpy(RoleInfo.szRoleName, RoleName.c_str(), sizeof(RoleInfo.szRoleName));

	return RoleInfo;
}