#include "StdAfx.h"
#include "KGameServerSimulator.h"
#include "SO3Result.h"
#include "KGameCenterWrapper.h"

KGameServerSimulator::KGameServerSimulator(void)
{
}

KGameServerSimulator::~KGameServerSimulator(void)
{
	UnInit();
}

bool KGameServerSimulator::DoPingSignal()
{
	bool				    bResult					= false;
	int					    nRetCode				= false;
	IKG_Buffer*			    piSendBuffer			= NULL;
	S2R_PING_SIGNAL* pCreateMapResond        = NULL;

	piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_PING_SIGNAL));
	KGLOG_PROCESS_ERROR(piSendBuffer);

	pCreateMapResond = (S2R_PING_SIGNAL*)piSendBuffer->GetData();
	KGLOG_PROCESS_ERROR(pCreateMapResond);

	pCreateMapResond->wProtocolID		= s2r_ping_signal;

	nRetCode = Send(piSendBuffer);
	KG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

int KGameServerSimulator::GetProtocolIndex(IKG_Buffer* pBuffer)
{
	if (pBuffer->GetSize() > sizeof(INTERNAL_PROTOCOL_HEADER))
	{
		INTERNAL_PROTOCOL_HEADER* p = (INTERNAL_PROTOCOL_HEADER*)pBuffer->GetData();
		return p->wProtocolID;
	}
	return -1;
}
bool KGameServerSimulator::Init()
{
	SCOPEPROTOCOL(this, r2s_handshake_respond);
	SCOPEPROTOCOL(this, r2s_create_map_notify);

	if (!Connect("127.0.0.1", GCSETTING->m_nGameServerPort, GCSETTING->m_nGatewayPingCycle))
	{
		return false;
	}
	DoHandshakeRequest();

	KGThread_Sleep(1000);

	IKG_Buffer* p = GetProtocolDataOnce(r2s_handshake_respond);
	if (!p)
	{
		return false;
	}

	std::list<IKG_Buffer*>& List = GetProtocolData(r2s_create_map_notify);
	std::list<IKG_Buffer*>::iterator Iter = List.begin();
	for (Iter; Iter != List.end(); ++Iter)
	{
		R2S_CREATE_MAP_NOTIFY* p = (R2S_CREATE_MAP_NOTIFY*)((*Iter)->GetData());
		DoCreateMapRespond(p->dwMapID, p->nMapCopyIndex, true);
		MapData md;
		md.dwMapID = p->dwMapID;
		md.nMapCopy = p->nMapCopyIndex;
		m_CreatedMapCopy.push_back(md);
	}
	DoUpdatePerformance(2000, 0, 1);

	DoSyncMentorData(ULONG_MAX, ULONG_MAX);

	KGThread_Sleep(100);

	return true;
}


bool KGameServerSimulator::DoCreateMapRespond(DWORD dwMapID, __int64 nMapCopyIndex, bool nResult)
{
	bool				    bResult					= false;
	int					    nRetCode				= false;
	IKG_Buffer*			    piSendBuffer			= NULL;
	S2R_CREATE_MAP_RESPOND* pCreateMapResond        = NULL;

	piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_CREATE_MAP_RESPOND));
	KGLOG_PROCESS_ERROR(piSendBuffer);

	pCreateMapResond = (S2R_CREATE_MAP_RESPOND*)piSendBuffer->GetData();
	KGLOG_PROCESS_ERROR(pCreateMapResond);

	pCreateMapResond->wProtocolID		= s2r_create_map_respond;
	pCreateMapResond->dwMapID			= dwMapID;
	pCreateMapResond->nMapCopyIndex		= nMapCopyIndex;
	pCreateMapResond->bResult			= nResult;

	nRetCode = Send(piSendBuffer);
	KG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

bool KGameServerSimulator::DoPlayerLoginRespond(DWORD dwPlayerID, bool bPermit)
{
	bool                 bResult				  = false;
	int                  nRetCode				  = false;
	IKG_Buffer*          piSendBuffer			  = NULL;
	S2R_PLAYER_LOGIN_RESPOND* pPlayerLoginRespond = NULL;

	piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_PLAYER_LOGIN_RESPOND));
	KGLOG_PROCESS_ERROR(piSendBuffer);

	pPlayerLoginRespond = (S2R_PLAYER_LOGIN_RESPOND*)piSendBuffer->GetData();
	KGLOG_PROCESS_ERROR(pPlayerLoginRespond);

	pPlayerLoginRespond->wProtocolID	  = s2r_player_login_respond;
	pPlayerLoginRespond->dwPlayerID		  = dwPlayerID;
	pPlayerLoginRespond->bPermit		  = bPermit;
	pPlayerLoginRespond->Guid             = GUID();

	nRetCode = Send(piSendBuffer);
	KG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

bool KGameServerSimulator::DoSearchMapRequset(DWORD dwPlayerID, DWORD dwMapID, __int64 nMapCopyIndex, int nPosX, int nPosY, int nPosZ)
{
	bool                 bResult				= false;
	int                  nRetCode			    = false;
	IKG_Buffer*          piSendBuffer			= NULL;
	S2R_SEARCH_MAP_REQUEST* pSearchMapRequset   = NULL;

	piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_SEARCH_MAP_REQUEST));
	KGLOG_PROCESS_ERROR(piSendBuffer);

	pSearchMapRequset = (S2R_SEARCH_MAP_REQUEST*)piSendBuffer->GetData();
	KGLOG_PROCESS_ERROR(pSearchMapRequset);

	pSearchMapRequset->wProtocolID		= s2r_search_map_request;
	pSearchMapRequset->dwPlayerID	    = dwPlayerID;
	pSearchMapRequset->dwMapID			= dwMapID;
	pSearchMapRequset->nMapCopyIndex	= nMapCopyIndex;
	pSearchMapRequset->nPosX			= nPosX;
	pSearchMapRequset->nPosY			= nPosY;
	pSearchMapRequset->nPosZ			= nPosZ;

	nRetCode = Send(piSendBuffer);
	KG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

bool KGameServerSimulator::DoTranferPlayerRequest(KROLE_LIST_INFO& RoleBaseInfo)
{
	bool                 bResult        = false;
	int                  nRetCode       = false;
	IKG_Buffer*          piSendBuffer   = NULL;
	S2R_TRANSFER_PLAYER_REQUEST* pTransferPlayerRequest        = NULL;

	piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_TRANSFER_PLAYER_REQUEST));
	KGLOG_PROCESS_ERROR(piSendBuffer);

	pTransferPlayerRequest = (S2R_TRANSFER_PLAYER_REQUEST*)piSendBuffer->GetData();
	KGLOG_PROCESS_ERROR(pTransferPlayerRequest);

	pTransferPlayerRequest->wProtocolID		 = s2r_transfer_player_request;
	//pTransferPlayerRequest->ChangePointInfo  = tagExtPointInfo();
	pTransferPlayerRequest->ExtPointInfo	 = tagExtPointInfo();
	pTransferPlayerRequest->nEndTimeOfFee		 = 500;
	pTransferPlayerRequest->dwRoleID         = RoleBaseInfo.dwPlayerID;
	pTransferPlayerRequest->RoleBaseInfo	 = RoleBaseInfo.BaseInfo;

	nRetCode = Send(piSendBuffer);
	KG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

bool KGameServerSimulator::DoTranferPlayerRequest(DWORD dwPlayerID, const char szAccount[], const char szRoleName[], DWORD dwMapID, __int64 nMapCopyID)
{
	bool                 bResult        = false;
	int                  nRetCode       = false;
	KROLE_LIST_INFO           BaseData;

	BaseData.dwPlayerID = dwPlayerID;
	strncpy(BaseData.szAccount, szAccount, _NAME_LEN);
	strncpy(BaseData.szRoleName, szRoleName, _NAME_LEN);
	BaseData.BaseInfo.CurrentPos.dwMapID = dwMapID;
	BaseData.BaseInfo.CurrentPos.nMapCopyIndex = nMapCopyID;

	nRetCode = DoTranferPlayerRequest(BaseData);
	KG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	return bResult;
}

bool KGameServerSimulator::DoTranferPlayerRespond(DWORD dwPlayerID, bool bSucceed)
{
	bool                 bResult        = false;
	int                  nRetCode       = false;
	IKG_Buffer*          piSendBuffer   = NULL;
	S2R_TRANSFER_PLAYER_RESPOND* pTransferPlayerRespond        = NULL;

	piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_TRANSFER_PLAYER_RESPOND));
	KGLOG_PROCESS_ERROR(piSendBuffer);

	pTransferPlayerRespond = (S2R_TRANSFER_PLAYER_RESPOND*)piSendBuffer->GetData();
	KGLOG_PROCESS_ERROR(pTransferPlayerRespond);

	pTransferPlayerRespond->wProtocolID		 = s2r_transfer_player_respond;
	pTransferPlayerRespond->dwPlayerID		 = dwPlayerID;
	pTransferPlayerRespond->bSucceed		 = bSucceed;

	nRetCode = Send(piSendBuffer);
	KG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

bool KGameServerSimulator::DoConfirmPlayerLoginRequest(DWORD dwPlayerID)
{
	bool                 bResult        = false;
	int                  nRetCode       = false;
	IKG_Buffer*          piSendBuffer   = NULL;
	S2R_CONFIRM_PLAYER_LOGIN_REQUEST* pConfirmPlayerLoginRequest        = NULL;

	piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_CONFIRM_PLAYER_LOGIN_REQUEST));
	KGLOG_PROCESS_ERROR(piSendBuffer);

	pConfirmPlayerLoginRequest = (S2R_CONFIRM_PLAYER_LOGIN_REQUEST*)piSendBuffer->GetData();
	KGLOG_PROCESS_ERROR(pConfirmPlayerLoginRequest);

	pConfirmPlayerLoginRequest->wProtocolID		 = s2r_confirm_player_login_request;
	pConfirmPlayerLoginRequest->dwPlayerID		 = dwPlayerID;

	nRetCode = Send(piSendBuffer);
	KG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

bool KGameServerSimulator::DoPlayerLeaveGS(DWORD dwPlayerID)
{
	bool                 bResult        = false;
	int                  nRetCode       = false;
	IKG_Buffer*          piSendBuffer   = NULL;
	S2R_PLAYER_LEAVE_GS* pPlayerLeaveGS = NULL;

	piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_PLAYER_LEAVE_GS));
	KGLOG_PROCESS_ERROR(piSendBuffer);

	pPlayerLeaveGS = (S2R_PLAYER_LEAVE_GS*)piSendBuffer->GetData();
	KGLOG_PROCESS_ERROR(pPlayerLeaveGS);

	pPlayerLeaveGS->wProtocolID		 = s2r_player_leave_gs;
	pPlayerLeaveGS->dwPlayerID		 = dwPlayerID;

	nRetCode = Send(piSendBuffer);
	KG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

void KGameServerSimulator::CloseConnect()
{
	UnInit();
}

bool KGameServerSimulator::CheckPlayerLoginRequset(DWORD dwPlayerID, DWORD dwMapID, __int64  nMapCopyIndex)
{
	bool                        bResult         = false;
	bool                        bRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_PLAYER_LOGIN_REQUEST*   pLoginRequest   = NULL;

	piPackage = ReceiveDataOnce(r2s_player_login_request);
	KGLOG_PROCESS_ERROR(piPackage);

	pLoginRequest = (R2S_PLAYER_LOGIN_REQUEST*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pLoginRequest);

	KGLOG_PROCESS_ERROR(pLoginRequest->dwRoleID == dwPlayerID);
	KGLOG_PROCESS_ERROR(pLoginRequest->BaseInfo.CurrentPos.dwMapID == dwMapID);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::CheckCreateMapNotify(DWORD dwMapID, __int64  nMapCopyIndex)
{
	bool                        bResult         = false;
	bool                        bRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_CREATE_MAP_NOTIFY*      pCreateMapNotify   = NULL;

	piPackage = ReceiveDataOnce(r2s_create_map_notify);
	KGLOG_PROCESS_ERROR(piPackage);

	pCreateMapNotify = (R2S_CREATE_MAP_NOTIFY*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pCreateMapNotify);

	KGLOG_PROCESS_ERROR(pCreateMapNotify->dwMapID == dwMapID);
	KGLOG_PROCESS_ERROR(pCreateMapNotify->nMapCopyIndex == nMapCopyIndex);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::CheckCreateMapNotify(DWORD dwMapID, __int64*  nMapCopyIndex)
{
	bool                        bResult         = false;
	bool                        bRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_CREATE_MAP_NOTIFY*      pCreateMapNotify   = NULL;

	KG_PROCESS_ERROR(nMapCopyIndex);

	piPackage = ReceiveDataOnce(r2s_create_map_notify);
	KGLOG_PROCESS_ERROR(piPackage);

	pCreateMapNotify = (R2S_CREATE_MAP_NOTIFY*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pCreateMapNotify);

	KGLOG_PROCESS_ERROR(pCreateMapNotify->dwMapID == dwMapID);
	*nMapCopyIndex = pCreateMapNotify->nMapCopyIndex;

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::CheckSearchMapRespond(DWORD dwPlayerID, SWITCH_MAP_RESULT_CODE nResultCode, bool bSwitchServer, DWORD dwMapID, __int64 nMapCopyIndex)
{
	bool                        bResult         = false;
	bool                        bRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_SEARCH_MAP_RESPOND*      pSearchMapRespond   = NULL;

	piPackage = ReceiveDataOnce(r2s_search_map_respond);
	KGLOG_PROCESS_ERROR(piPackage);

	pSearchMapRespond = (R2S_SEARCH_MAP_RESPOND*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pSearchMapRespond);

	KGLOG_PROCESS_ERROR(pSearchMapRespond->dwPlayerID	 == dwPlayerID);
	KGLOG_PROCESS_ERROR(pSearchMapRespond->nResultCode		 == nResultCode);
	KGLOG_PROCESS_ERROR(pSearchMapRespond->bSwitchServer == bSwitchServer);
	if (pSearchMapRespond->nResultCode == smrcSuccess)
	{
		KGLOG_PROCESS_ERROR(pSearchMapRespond->nMapCopyIndex == nMapCopyIndex);
	}
	KGLOG_PROCESS_ERROR(pSearchMapRespond->dwMapID		 == dwMapID);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::CheckTransferPlayerRequest(DWORD dwPlayerID, DWORD dwMapID, __int64 nMapCopyIndex)
{
	bool							  bResult				   = false;
	bool						      bRetCode                 = false;
	IKG_Buffer*						  piPackage				   = NULL;
	R2S_TRANSFER_PLAYER_REQUEST*      pTransferPlayerRequest   = NULL;

	piPackage = ReceiveDataOnce(r2s_transfer_player_request);
	KGLOG_PROCESS_ERROR(piPackage);

	pTransferPlayerRequest = (R2S_TRANSFER_PLAYER_REQUEST*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pTransferPlayerRequest);

	KGLOG_PROCESS_ERROR(pTransferPlayerRequest->dwRoleID				  == dwPlayerID);
	KGLOG_PROCESS_ERROR(pTransferPlayerRequest->BaseInfo.CurrentPos.dwMapID		  == dwMapID);
	KGLOG_PROCESS_ERROR(pTransferPlayerRequest->BaseInfo.CurrentPos.nMapCopyIndex == nMapCopyIndex);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::CheckTransferPlayerRespond(DWORD dwPlayerID, bool bSucceed)
{
	bool                        bResult         = false;
	bool                        bRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_TRANSFER_PLAYER_RESPOND*      pTransferPlayerRespond   = NULL;

	piPackage = ReceiveDataOnce(r2s_transfer_player_respond);
	KGLOG_PROCESS_ERROR(piPackage);

	pTransferPlayerRespond = (R2S_TRANSFER_PLAYER_RESPOND*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pTransferPlayerRespond);

	KGLOG_PROCESS_ERROR(pTransferPlayerRespond->dwPlayerID == dwPlayerID);
	KGLOG_PROCESS_ERROR(pTransferPlayerRespond->bSucceed   == bSucceed);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::CheckConfirmPlayerLoginRespond(DWORD dwPlayerID, bool  bPermit)
{
	bool                        bResult         = false;
	bool                        bRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_CONFIRM_PLAYER_LOGIN_RESPOND*      pConfirmPlayerLoginRespond   = NULL;

	piPackage = ReceiveDataOnce(r2s_confirm_player_login_respond);
	KGLOG_PROCESS_ERROR(piPackage);

	pConfirmPlayerLoginRespond = (R2S_CONFIRM_PLAYER_LOGIN_RESPOND*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pConfirmPlayerLoginRespond);

	KGLOG_PROCESS_ERROR(pConfirmPlayerLoginRespond->dwPlayerID == dwPlayerID);
	KGLOG_PROCESS_ERROR(pConfirmPlayerLoginRespond->bPermit    == bPermit);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::DoInvitePlayerJoinPartyRequest(DWORD dwInviteSrc, const char szInviteRole[])
{
	bool                 bResult				= false;
	int                  nRetCode			    = false;
	IKG_Buffer*          piSendBuffer			= NULL;
	S2R_INVITE_PLAYER_JOIN_TEAM_REQUEST* pInvitePlayerJoinParty   = NULL;

	piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_INVITE_PLAYER_JOIN_TEAM_REQUEST));
	KGLOG_PROCESS_ERROR(piSendBuffer);

	pInvitePlayerJoinParty = (S2R_INVITE_PLAYER_JOIN_TEAM_REQUEST*)piSendBuffer->GetData();
	KGLOG_PROCESS_ERROR(pInvitePlayerJoinParty);

	pInvitePlayerJoinParty->wProtocolID		 = s2r_invite_player_join_team_request;
	pInvitePlayerJoinParty->dwInviteSrc      = dwInviteSrc;
	strncpy(pInvitePlayerJoinParty->szInviteRole, szInviteRole, _NAME_LEN);

	nRetCode = Send(piSendBuffer);
	KG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

bool KGameServerSimulator::DoInvitePlayerJoinPartyRespond(const char szInviteSrc[], DWORD dwInviteDst, bool bAgreeFlag, GUID guid)
{
	bool                 bResult				= false;
	int                  nRetCode			    = false;
	IKG_Buffer*          piSendBuffer			= NULL;
	S2R_INVITE_PLAYER_JOIN_TEAM_RESPOND* pInviteRespond   = NULL;

	piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_INVITE_PLAYER_JOIN_TEAM_RESPOND));
	KGLOG_PROCESS_ERROR(piSendBuffer);

	pInviteRespond = (S2R_INVITE_PLAYER_JOIN_TEAM_RESPOND*)piSendBuffer->GetData();
	KGLOG_PROCESS_ERROR(pInviteRespond);

	pInviteRespond->wProtocolID		 = s2r_invite_player_join_team_respond;
	strncpy(pInviteRespond->szInviteSrc, szInviteSrc, _NAME_LEN);
	pInviteRespond->dwInviteDst      = dwInviteDst;
	pInviteRespond->bAgreeFlag       = bAgreeFlag;
	//	pInviteRespond->guid             = guid;

	nRetCode = Send(piSendBuffer);
	KG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

bool KGameServerSimulator::DoApplyJoinPartyRequest(DWORD dwApplySrc, const char  szApplyDst[])
{
	bool                 bResult				= false;
	int                  nRetCode			    = false;
	IKG_Buffer*          piSendBuffer			= NULL;
	S2R_APPLY_JOIN_TEAM_REQUEST* pApplyJoinPartyRequest   = NULL;

	piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_APPLY_JOIN_TEAM_REQUEST));
	KGLOG_PROCESS_ERROR(piSendBuffer);

	pApplyJoinPartyRequest = (S2R_APPLY_JOIN_TEAM_REQUEST*)piSendBuffer->GetData();
	KGLOG_PROCESS_ERROR(pApplyJoinPartyRequest);

	pApplyJoinPartyRequest->wProtocolID		 = s2r_apply_join_team_request;
	pApplyJoinPartyRequest->dwApplySrc       = dwApplySrc;
	strncpy(pApplyJoinPartyRequest->szApplyDst, szApplyDst, _NAME_LEN);

	nRetCode = Send(piSendBuffer);
	KG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

bool KGameServerSimulator::DoApplyJoinPartyRespond(const char  szApplySrc[], DWORD dwApplyDst, bool  bAgreeFlag, GUID  guid)
{
	bool                 bResult				= false;
	int                  nRetCode			    = false;
	IKG_Buffer*          piSendBuffer			= NULL;
	S2R_APPLY_JOIN_TEAM_RESPOND* pApplyRespond   = NULL;

	piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_APPLY_JOIN_TEAM_RESPOND));
	KGLOG_PROCESS_ERROR(piSendBuffer);

	pApplyRespond = (S2R_APPLY_JOIN_TEAM_RESPOND*)piSendBuffer->GetData();
	KGLOG_PROCESS_ERROR(pApplyRespond);

	pApplyRespond->wProtocolID		 = s2r_apply_join_team_respond;
	pApplyRespond->dwApplyDst        = dwApplyDst;
	pApplyRespond->bAgreeFlag        = bAgreeFlag;
	//	pApplyRespond->guid              = guid;
	strncpy(pApplyRespond->szApplySrc, szApplySrc, _NAME_LEN);

	nRetCode = Send(piSendBuffer);
	KG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

bool KGameServerSimulator::DoPartyDelMemberRequest(DWORD dwPartyID, DWORD dwRequestSrc, DWORD dwMemberID)
{
	bool                 bResult				= false;
	int                  nRetCode			    = false;
	IKG_Buffer*          piSendBuffer			= NULL;
	S2R_TEAM_DEL_MEMBER_REQUEST*   pDelMemberRequest   = NULL;

	piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_TEAM_DEL_MEMBER_REQUEST));
	KGLOG_PROCESS_ERROR(piSendBuffer);

	pDelMemberRequest = (S2R_TEAM_DEL_MEMBER_REQUEST*)piSendBuffer->GetData();
	KGLOG_PROCESS_ERROR(pDelMemberRequest);

	pDelMemberRequest->wProtocolID		= s2r_team_del_member_request;
	pDelMemberRequest->dwTeamID        = dwPartyID;
	pDelMemberRequest->dwRequestSrc     = dwRequestSrc;
	pDelMemberRequest->dwMemberID       = dwMemberID;

	nRetCode = Send(piSendBuffer);
	KG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

bool KGameServerSimulator::DoPartyChangeAuthorityRequest(DWORD dwRequestSrc, BYTE byType, DWORD dwTargetID)
{
	bool                 bResult				= false;
	int                  nRetCode			    = false;
	IKG_Buffer*          piSendBuffer			= NULL;
	S2R_TEAM_CHANGE_AUTHORITY_REQUEST *   pChangeLeaderRequest   = NULL;

	piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_TEAM_CHANGE_AUTHORITY_REQUEST));
	KGLOG_PROCESS_ERROR(piSendBuffer);

	pChangeLeaderRequest = (S2R_TEAM_CHANGE_AUTHORITY_REQUEST*)piSendBuffer->GetData();
	KGLOG_PROCESS_ERROR(pChangeLeaderRequest);

	pChangeLeaderRequest->wProtocolID	   = s2r_team_change_authority_request;
	pChangeLeaderRequest->dwRequestSrc        = dwRequestSrc;
	pChangeLeaderRequest->byType     = byType;
	pChangeLeaderRequest->dwTargetID    = dwTargetID;

	nRetCode = Send(piSendBuffer);
	KG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

bool KGameServerSimulator::DoPartyDisbandRequest(DWORD dwLeaderID, DWORD dwPartyID)
{
	bool                 bResult				= false;
	int                  nRetCode			    = false;
	IKG_Buffer*          piSendBuffer			= NULL;
	S2R_TEAM_DISBAND_REQUEST*      pPartyDisbandRequest   = NULL;

	piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_TEAM_DISBAND_REQUEST));
	KGLOG_PROCESS_ERROR(piSendBuffer);

	pPartyDisbandRequest = (S2R_TEAM_DISBAND_REQUEST*)piSendBuffer->GetData();
	KGLOG_PROCESS_ERROR(pPartyDisbandRequest);

	pPartyDisbandRequest->wProtocolID	   = s2r_team_disband_request;
	pPartyDisbandRequest->dwTeamID        = dwPartyID;
	pPartyDisbandRequest->dwLeaderID       = dwLeaderID;

	nRetCode = Send(piSendBuffer);
	KG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

bool KGameServerSimulator::DoPartySyncMemberMaxLMR(DWORD dwPartyID, DWORD dwMemberID, int nMaxLife, int nMaxMana)
{
	bool                 bResult				= false;
	int                  nRetCode			    = false;
	IKG_Buffer*          piSendBuffer			= NULL;
	S2R_SYNC_TEAM_MEMBER_MAX_LMR*      pSyncPartyMaxLMR   = NULL;

	piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_SYNC_TEAM_MEMBER_MAX_LMR));
	KGLOG_PROCESS_ERROR(piSendBuffer);

	pSyncPartyMaxLMR = (S2R_SYNC_TEAM_MEMBER_MAX_LMR*)piSendBuffer->GetData();
	KGLOG_PROCESS_ERROR(pSyncPartyMaxLMR);

	pSyncPartyMaxLMR->wProtocolID	   = s2r_sync_team_member_max_lmr;
	pSyncPartyMaxLMR->dwTeamID        = dwPartyID;
	pSyncPartyMaxLMR->dwMemberID       = dwMemberID;
	pSyncPartyMaxLMR->nMaxLife         = nMaxLife;
	pSyncPartyMaxLMR->nMaxMana         = nMaxMana;

	nRetCode = Send(piSendBuffer);
	KG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

bool KGameServerSimulator::DoPartySyncMemberCurrentLMR(DWORD dwPartyID, DWORD dwMemberID, BYTE byLifePercent, BYTE byManaPercent, BYTE byRagePercent)
{
	bool                 bResult				= false;
	int                  nRetCode			    = false;
	IKG_Buffer*          piSendBuffer			= NULL;
	S2R_SYNC_TEAM_MEMBER_CURRENT_LMR*      pSyncPartyCurrentLMR   = NULL;

	piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_SYNC_TEAM_MEMBER_CURRENT_LMR));
	KGLOG_PROCESS_ERROR(piSendBuffer);

	pSyncPartyCurrentLMR = (S2R_SYNC_TEAM_MEMBER_CURRENT_LMR*)piSendBuffer->GetData();
	KGLOG_PROCESS_ERROR(pSyncPartyCurrentLMR);

	pSyncPartyCurrentLMR->wProtocolID	   = s2r_sync_team_member_current_lmr;
	pSyncPartyCurrentLMR->dwTeamID        = dwPartyID;
	pSyncPartyCurrentLMR->dwMemberID       = dwMemberID;
	pSyncPartyCurrentLMR->byLifePercent    = byLifePercent;
	pSyncPartyCurrentLMR->byManaPercent    = byManaPercent;
	pSyncPartyCurrentLMR->byRagePercent    = byRagePercent;

	nRetCode = Send(piSendBuffer);
	KG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

bool KGameServerSimulator::DoPartySyncMemberMisc(DWORD dwPartyID,DWORD dwMemberID, DWORD dwPortraitID, DWORD dwForceID, BYTE byLevel, BYTE byRoleType)
{
	bool                 bResult				= false;
	int                  nRetCode			    = false;
	IKG_Buffer*          piSendBuffer			= NULL;
	S2R_SYNC_TEAM_MEMBER_MISC*      pSyncPartyMisc   = NULL;

	piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_SYNC_TEAM_MEMBER_MISC));
	KGLOG_PROCESS_ERROR(piSendBuffer);

	pSyncPartyMisc = (S2R_SYNC_TEAM_MEMBER_MISC*)piSendBuffer->GetData();
	KGLOG_PROCESS_ERROR(pSyncPartyMisc);

	pSyncPartyMisc->wProtocolID	     = s2r_sync_team_member_misc;
	pSyncPartyMisc->dwTeamID        = dwPartyID;
	pSyncPartyMisc->dwMemberID       = dwMemberID;
	//pSyncPartyMisc->dwPortraitID     = dwPortraitID;
	pSyncPartyMisc->dwForceID        = dwForceID;
	pSyncPartyMisc->byLevel          = byLevel;
	pSyncPartyMisc->byRoleType       = byRoleType;

	nRetCode = Send(piSendBuffer);
	KG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

bool KGameServerSimulator::DoPartySyncMemberPosition(DWORD dwPartyID, DWORD dwMemberID, DWORD dwMapID, INT64 nMapCopyIndex, int nPosX, int nPosY)
{
	bool                 bResult				= false;
	int                  nRetCode			    = false;
	IKG_Buffer*          piSendBuffer			= NULL;
	S2R_SYNC_TEAM_MEMBER_POSITION*      pSyncPartyPosition   = NULL;

	piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_SYNC_TEAM_MEMBER_POSITION));
	KGLOG_PROCESS_ERROR(piSendBuffer);

	pSyncPartyPosition = (S2R_SYNC_TEAM_MEMBER_POSITION*)piSendBuffer->GetData();
	KGLOG_PROCESS_ERROR(pSyncPartyPosition);

	pSyncPartyPosition->wProtocolID	     = s2r_sync_team_member_position;
	pSyncPartyPosition->dwTeamID        = dwPartyID;
	pSyncPartyPosition->dwMemberID       = dwMemberID;
	pSyncPartyPosition->dwMapID          = dwMapID;
	pSyncPartyPosition->nMapCopyIndex    = nMapCopyIndex;
	pSyncPartyPosition->nPosX            = nPosX;
	pSyncPartyPosition->nPosY			 = nPosY;

	nRetCode = Send(piSendBuffer);
	KG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

bool KGameServerSimulator::CheckInvitePlayerJoinPartyRequest(const char szInviteSrc[], DWORD dwInviteDst, GUID&  guid)
{
	bool                        bResult         = false;
	int                         nRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_INVITE_PLAYER_JOIN_TEAM_REQUEST* pPartyInvitation   = NULL;

	piPackage = ReceiveDataOnce(r2s_invite_player_join_team_request);
	KGLOG_PROCESS_ERROR(piPackage);

	pPartyInvitation = (R2S_INVITE_PLAYER_JOIN_TEAM_REQUEST*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pPartyInvitation);

	KG_PROCESS_ERROR(pPartyInvitation->dwInviteDst == dwInviteDst);
	nRetCode = strcmp(pPartyInvitation->szInviteSrc, szInviteSrc);
	KG_PROCESS_ERROR(nRetCode == 0);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::CheckApplyJoinPartyRequest(DWORD dwApplyDst, const char  szApplySrc[], GUID&  guid)
{
	bool                        bResult         = false;
	int                         nRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_APPLY_JOIN_TEAM_REQUEST*   pApplyToDst    = NULL;

	piPackage = ReceiveDataOnce(r2s_apply_join_team_request);
	KGLOG_PROCESS_ERROR(piPackage);

	pApplyToDst = (R2S_APPLY_JOIN_TEAM_REQUEST*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pApplyToDst);

	KG_PROCESS_ERROR(pApplyToDst->dwApplyDst == dwApplyDst);
	nRetCode = strcmp(pApplyToDst->szApplySrc, szApplySrc);
	KG_PROCESS_ERROR(nRetCode == 0);
//	guid = pApplyToDst->guid;


	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::CheckPartyCreateNotify(DWORD* dwPartyID, DWORD dwLeaderID, char  szLeaderName[])
{
	bool                        bResult         = false;
	int                         nRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_TEAM_CREATE_NOTIFY*    pNotify    = NULL;

	piPackage = ReceiveDataOnce(r2s_team_create_notify);
	KGLOG_PROCESS_ERROR(piPackage);

	pNotify = (R2S_TEAM_CREATE_NOTIFY*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pNotify);

	*dwPartyID = pNotify->dwTeamID; 

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::CheckPartyAddMemberNotify(DWORD dwPartyID, DWORD dwMemberID)
{
	bool                        bResult         = false;
	int                         nRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_TEAM_ADD_MEMBER_NOTIFY*    pNotify    = NULL;

	piPackage = ReceiveDataOnce(r2s_team_add_member_notify);
	KGLOG_PROCESS_ERROR(piPackage);

	pNotify = (R2S_TEAM_ADD_MEMBER_NOTIFY*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pNotify);

	KG_PROCESS_ERROR(pNotify->dwTeamID == dwPartyID);
	KG_PROCESS_ERROR(pNotify->dwMemberID == dwMemberID);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::CheckPartyChangeAuthorityNotify(DWORD dwPartyID, BYTE byType, DWORD dwNewID)
{
	bool                        bResult         = false;
	int                         nRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_TEAM_CHANGE_AUTHORITY_NOTIFY*    pNotify    = NULL;

	piPackage = ReceiveDataOnce(r2s_team_change_authority_notify);
	KGLOG_PROCESS_ERROR(piPackage);

	pNotify = (R2S_TEAM_CHANGE_AUTHORITY_NOTIFY*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pNotify);

	KG_PROCESS_ERROR(pNotify->dwTeamID == dwPartyID);
	KG_PROCESS_ERROR(pNotify->byType == byType);
	KG_PROCESS_ERROR(pNotify->dwNewID == dwNewID);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::CheckPartyDelMemberNotify(DWORD dwPartyID, DWORD dwMemberID)
{
	bool                        bResult         = false;
	int                         nRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_TEAM_DEL_MEMBER_NOTIFY*    pDelMemberNotify    = NULL;

	piPackage = ReceiveDataOnce(r2s_team_del_member_notify);
	KGLOG_PROCESS_ERROR(piPackage);

	pDelMemberNotify = (R2S_TEAM_DEL_MEMBER_NOTIFY*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pDelMemberNotify);

	KG_PROCESS_ERROR(pDelMemberNotify->dwTeamID == dwPartyID);
	KG_PROCESS_ERROR(pDelMemberNotify->dwMemberID == dwMemberID);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::CheckPartyDisbandNotify(DWORD dwPartyID)
{
	bool                        bResult         = false;
	int                         nRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_TEAM_DISBAND_NOTIFY       *pPartyDisbandNotify    = NULL;

	piPackage = ReceiveDataOnce(r2s_team_disband_notify);
	KGLOG_PROCESS_ERROR(piPackage);

	pPartyDisbandNotify = (R2S_TEAM_DISBAND_NOTIFY*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pPartyDisbandNotify);

	KG_PROCESS_ERROR(pPartyDisbandNotify->dwTeamID == dwPartyID);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::CheckSyncPartyMemberOnlineFlag(DWORD dwPartyID, DWORD dwMemberID, bool  bOnlineFlag)
{
	bool                        bResult         = false;
	int                         nRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_SYNC_TEAM_MEMBER_ONLINE_FLAG  *pSyncOnlineInfo    = NULL;

	piPackage = ReceiveDataOnce(r2s_sync_team_member_online_flag);
	KGLOG_PROCESS_ERROR(piPackage);

	pSyncOnlineInfo = (R2S_SYNC_TEAM_MEMBER_ONLINE_FLAG*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pSyncOnlineInfo);

	KG_PROCESS_ERROR(pSyncOnlineInfo->dwTeamID == dwPartyID);
	KG_PROCESS_ERROR(pSyncOnlineInfo->dwMemberID == dwMemberID);
	KG_PROCESS_ERROR(pSyncOnlineInfo->bOnlineFlag == bOnlineFlag);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::CheckPartySyncMemberMaxLMR(DWORD dwPartyID, DWORD dwMemberID, int nMaxLife, int nMaxMana)
{
	bool                 bResult				= false;
	int                  nRetCode			    = false;
	IKG_Buffer*          piPackage			    = NULL;
	R2S_SYNC_TEAM_MEMBER_MAX_LMR*      pSyncPartyMaxLMR   = NULL;

	piPackage = ReceiveDataOnce(r2s_sync_team_member_max_lmr);
	KGLOG_PROCESS_ERROR(piPackage);

	pSyncPartyMaxLMR = (R2S_SYNC_TEAM_MEMBER_MAX_LMR*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pSyncPartyMaxLMR);

	KG_PROCESS_ERROR(pSyncPartyMaxLMR->dwTeamID        == dwPartyID);
	KG_PROCESS_ERROR(pSyncPartyMaxLMR->dwMemberID       == dwMemberID);
	KG_PROCESS_ERROR(pSyncPartyMaxLMR->nMaxLife         == nMaxLife);
	KG_PROCESS_ERROR(pSyncPartyMaxLMR->nMaxMana         == nMaxMana);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::CheckPartySyncMemberCurrentLMR(DWORD dwPartyID, DWORD dwMemberID, BYTE byLifePercent, BYTE byManaPercent, BYTE byRagePercent)
{
	bool                 bResult				= false;
	int                  nRetCode			    = false;
	IKG_Buffer*          piPackage			    = NULL;
	S2R_SYNC_TEAM_MEMBER_CURRENT_LMR*      pSyncPartyCurrentLMR   = NULL;

	piPackage = ReceiveDataOnce(r2s_sync_team_member_current_lmr);
	KGLOG_PROCESS_ERROR(piPackage);

	pSyncPartyCurrentLMR = (S2R_SYNC_TEAM_MEMBER_CURRENT_LMR*)piPackage->GetData();
	KG_PROCESS_ERROR(pSyncPartyCurrentLMR);

	KG_PROCESS_ERROR(pSyncPartyCurrentLMR->dwTeamID        == dwPartyID);
	KG_PROCESS_ERROR(pSyncPartyCurrentLMR->dwMemberID       == dwMemberID);
	KG_PROCESS_ERROR(pSyncPartyCurrentLMR->byLifePercent    == byLifePercent);
	KG_PROCESS_ERROR(pSyncPartyCurrentLMR->byManaPercent    == byManaPercent);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::CheckPartySyncMemberMisc(DWORD dwPartyID,DWORD dwMemberID, DWORD dwPortraitID, DWORD dwForceID, BYTE byLevel, BYTE byRoleType)
{
	bool                 bResult				= false;
	int                  nRetCode			    = false;
	IKG_Buffer*          piPackage			    = NULL;
	R2S_SYNC_TEAM_MEMBER_MISC*      pSyncPartyMisc   = NULL;

	piPackage = ReceiveDataOnce(r2s_sync_team_member_misc);
	KGLOG_PROCESS_ERROR(piPackage);

	pSyncPartyMisc = (R2S_SYNC_TEAM_MEMBER_MISC*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pSyncPartyMisc);

	KG_PROCESS_ERROR(pSyncPartyMisc->dwTeamID        == dwPartyID);
	KG_PROCESS_ERROR(pSyncPartyMisc->dwMemberID       == dwMemberID);
	KG_PROCESS_ERROR(pSyncPartyMisc->dwForceID        == dwForceID);
	KG_PROCESS_ERROR(pSyncPartyMisc->byLevel          == byLevel);
	KG_PROCESS_ERROR(pSyncPartyMisc->byRoleType       == byRoleType);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::CheckPartySyncMemberPosition(DWORD dwPartyID, DWORD dwMemberID, DWORD dwMapID, INT64 nMapCopyIndex, int nPosX, int nPosY)
{
	bool                 bResult				= false;
	int                  nRetCode			    = false;
	IKG_Buffer*          piPackage			= NULL;
	S2R_SYNC_TEAM_MEMBER_POSITION*      pSyncPartyPosition   = NULL;

	piPackage = ReceiveDataOnce(r2s_sync_team_member_position);
	KGLOG_PROCESS_ERROR(piPackage);

	pSyncPartyPosition = (S2R_SYNC_TEAM_MEMBER_POSITION*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pSyncPartyPosition);

	KG_PROCESS_ERROR(pSyncPartyPosition->dwTeamID        == dwPartyID);
	KG_PROCESS_ERROR(pSyncPartyPosition->dwMemberID       == dwMemberID);
	KG_PROCESS_ERROR(pSyncPartyPosition->dwMapID          == dwMapID);
	KG_PROCESS_ERROR(pSyncPartyPosition->nMapCopyIndex    == nMapCopyIndex);
	KG_PROCESS_ERROR(pSyncPartyPosition->nPosX            == nPosX);
	KG_PROCESS_ERROR(pSyncPartyPosition->nPosY			  == nPosY);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::CheckPartyMessageNotify(DWORD dwPlayerID, int nCode)
{
	bool                 bResult				= false;
	int                  nRetCode			    = false;
	IKG_Buffer*          piPackage			= NULL;
	R2S_PARTY_MESSAGE_NOTIFY*      pPartyMessageNotify   = NULL;

	piPackage = ReceiveDataOnce(r2s_party_message_notify);
	KGLOG_PROCESS_ERROR(piPackage);

	pPartyMessageNotify = (R2S_PARTY_MESSAGE_NOTIFY*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pPartyMessageNotify);

	KG_PROCESS_ERROR(pPartyMessageNotify->dwPlayerID  == dwPlayerID);
	KG_PROCESS_ERROR(pPartyMessageNotify->nCode       == nCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::CheckDeleteMapNotify(DWORD dwMapID, __int64 nMapCopyIndex)
{
	bool                 bResult				= false;
	int                  nRetCode			    = false;
	IKG_Buffer*          piPackage			= NULL;
	R2S_DELETE_MAP_NOTIFY*      pDeleteMapNotify   = NULL;

	piPackage = ReceiveDataOnce(r2s_delete_map_notify);
	KGLOG_PROCESS_ERROR(piPackage);

	pDeleteMapNotify = (R2S_DELETE_MAP_NOTIFY*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pDeleteMapNotify);

	KG_PROCESS_ERROR(pDeleteMapNotify->dwMapID  == dwMapID);
	KG_PROCESS_ERROR(pDeleteMapNotify->nMapCopyIndex       == nMapCopyIndex);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}


bool KGameServerSimulator::DoApplyFellowshipDataRequest(DWORD dwPlayerID)
{
	bool                 bResult				= false;
	int                  nRetCode			    = false;
	IKG_Buffer*          piSendBuffer			= NULL;
	S2R_APPLY_FELLOWSHIP_DATA_REQUEST*      pApplyFellowship   = NULL;

	piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_APPLY_FELLOWSHIP_DATA_REQUEST));
	KGLOG_PROCESS_ERROR(piSendBuffer);

	pApplyFellowship = (S2R_APPLY_FELLOWSHIP_DATA_REQUEST*)piSendBuffer->GetData();
	KGLOG_PROCESS_ERROR(pApplyFellowship);

	pApplyFellowship->wProtocolID	  = s2r_apply_fellowship_data_request;
	pApplyFellowship->dwPlayerID	  = dwPlayerID;

	nRetCode = Send(piSendBuffer);
	KG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

bool KGameServerSimulator::DoUpdateFellowshipData(DWORD dwPlayerID, BYTE byData[], size_t uDataLen)
{
	bool                 bResult				= false;
	int                  nRetCode			    = false;
	IKG_Buffer*          piSendBuffer			= NULL;
	S2R_UPDATE_FELLOWSHIP_DATA*      pPak   = NULL;

	piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_UPDATE_FELLOWSHIP_DATA) + (unsigned int)uDataLen);
	KGLOG_PROCESS_ERROR(piSendBuffer);

	pPak = (S2R_UPDATE_FELLOWSHIP_DATA*)piSendBuffer->GetData();
	KGLOG_PROCESS_ERROR(pPak);

	pPak->wProtocolID	  = s2r_update_fellowship_data;
	pPak->dwPlayerID	  = dwPlayerID;
	memcpy(pPak->byData, byData, uDataLen);

	nRetCode = Send(piSendBuffer);
	KG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

bool KGameServerSimulator::DoAddFellowshipRequest(DWORD dwPlayerID, const char szAlliedPlayerName[])
{
	bool                 bResult				= false;
	int                  nRetCode			    = false;
	IKG_Buffer*          piSendBuffer			= NULL;
	S2R_ADD_FELLOWSHIP_REQUEST*      pPak   = NULL;

	piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_ADD_FELLOWSHIP_REQUEST));
	KGLOG_PROCESS_ERROR(piSendBuffer);

	pPak = (S2R_ADD_FELLOWSHIP_REQUEST*)piSendBuffer->GetData();
	KGLOG_PROCESS_ERROR(pPak);

	pPak->wProtocolID	  = s2r_add_fellowship_request;
	pPak->dwPlayerID	  = dwPlayerID;
	strcpy(pPak->szAlliedPlayerName, szAlliedPlayerName);

	nRetCode = Send(piSendBuffer);
	KG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

bool KGameServerSimulator::DoGetFellowshipNameRequest(DWORD   dwPlayerID, BYTE byCount, DWORD   dwAlliedPlayerIDArray[])
{
	bool                 bResult				= false;
	int                  nRetCode			    = false;
	IKG_Buffer*          piSendBuffer			= NULL;
	S2R_GET_FELLOWSHIP_NAME_REQUEST*      pPak   = NULL;

	piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_GET_FELLOWSHIP_NAME_REQUEST) + (byCount * sizeof(DWORD)));
	KGLOG_PROCESS_ERROR(piSendBuffer);

	pPak = (S2R_GET_FELLOWSHIP_NAME_REQUEST*)piSendBuffer->GetData();
	KGLOG_PROCESS_ERROR(pPak);

	pPak->wProtocolID	  = s2r_get_fellowship_name_request;
	pPak->dwPlayerID	  = dwPlayerID;
	pPak->byCount		  = byCount;
	memcpy(pPak->dwAlliedPlayerIDArray, dwAlliedPlayerIDArray, (byCount * sizeof(DWORD)));

	nRetCode = Send(piSendBuffer);
	KG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

bool KGameServerSimulator::CheckApplyFellowshipDataRespond(DWORD dwPlayerID, BYTE  byData[], unsigned int uLength)
{
	bool                        bResult         = false;
	int                         nRetCode        = false;
	unsigned int                uDataLength         = 0;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_APPLY_FELLOWSHIP_DATA_RESPOND*   pRespond		= NULL;

	piPackage = ReceiveDataOnce(r2s_apply_fellowship_data_respond);
	KGLOG_PROCESS_ERROR(piPackage);

	pRespond = (R2S_APPLY_FELLOWSHIP_DATA_RESPOND*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pRespond);

	KG_PROCESS_ERROR(pRespond->dwPlayerID == dwPlayerID);
	uDataLength = piPackage->GetSize() - sizeof(R2S_APPLY_FELLOWSHIP_DATA_RESPOND);
	KG_PROCESS_ERROR(uDataLength == uLength);

	if (uDataLength)
	{
		nRetCode = memcmp(pRespond->byData, byData, uLength);
		KG_PROCESS_ERROR(nRetCode == 0);
	}

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::CheckAddFellowshipRespond(DWORD dwPlayerID, DWORD dwAlliedPlayerID)
{
	bool                        bResult         = false;
	int                         nRetCode        = false;
	unsigned int                uLength         = 0;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_ADD_FELLOWSHIP_RESPOND*   pRespond		= NULL;

	piPackage = ReceiveDataOnce(r2s_add_fellowship_respond);
	KGLOG_PROCESS_ERROR(piPackage);

	pRespond = (R2S_ADD_FELLOWSHIP_RESPOND*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pRespond);

	KG_PROCESS_ERROR(pRespond->dwPlayerID == dwPlayerID);
	KG_PROCESS_ERROR(pRespond->dwAlliedPlayerID == dwAlliedPlayerID);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::CheckGetFellowshipNameRespond(DWORD dwPlayerID, int nArrayLength, DWORD dwPalyerIDArray[])
{
	bool                        bResult         = false;
	int                         nRetCode        = false;
	unsigned int                uLength         = 0;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_GET_FELLOWSHIP_NAME_RESPOND*   pRespond		= NULL;

	piPackage = ReceiveDataOnce(r2s_get_fellowship_name_respond);
	KGLOG_PROCESS_ERROR(piPackage);

	pRespond = (R2S_GET_FELLOWSHIP_NAME_RESPOND*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pRespond);

	KG_PROCESS_ERROR(pRespond->dwPlayerID == dwPlayerID);
	//	KG_PROCESS_ERROR(pRespond->byCount == nArrayLength);
	for (int i = 0 ; i < pRespond->byCount; i++)
	{
		bool bFindSame = false;
		for (int j = 0; j < nArrayLength; j++)
		{
			if (dwPalyerIDArray[j] == pRespond->AlliedPlayerInfo[i].dwID)
			{
				bFindSame = true;
				break;
			}
		}
		if (!bFindSame)
		{
			nRetCode = strcmp(pRespond->AlliedPlayerInfo[i].szName, "");
			KG_PROCESS_ERROR(nRetCode == 0);
		}
	}

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::CheckSyncFellowshipMapID(DWORD dwPlayerID, DWORD dwMapID)
{
	bool                        bResult         = false;
	int                         nRetCode        = false;
	unsigned int                uLength         = 0;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_SYNC_FELLOWSHIP_MAPID*   pRespond		= NULL;

	piPackage = ReceiveDataOnce(r2s_sync_fellowship_mapid);
	KGLOG_PROCESS_ERROR(piPackage);

	pRespond = (R2S_SYNC_FELLOWSHIP_MAPID*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pRespond);

	KG_PROCESS_ERROR(pRespond->dwPlayerID == dwPlayerID);
	KG_PROCESS_ERROR(pRespond->dwMapID == dwMapID);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}


bool KGameServerSimulator::DoSaveSceneRequest(DWORD dwMapID, __int64 nMapCopyIndex, char* pSaveData, size_t uLength)
{
	bool				    bResult					= false;
	int					    nRetCode				= false;
	IKG_Buffer*			    piSendBuffer			= NULL;
	S2R_SAVE_SCENE_DATA* pSaveSceneData        = NULL;

	piSendBuffer = KG_MemoryCreateBuffer((unsigned int)(sizeof(S2R_SAVE_SCENE_DATA) + uLength));
	KGLOG_PROCESS_ERROR(piSendBuffer);

	pSaveSceneData = (S2R_SAVE_SCENE_DATA*)piSendBuffer->GetData();
	KGLOG_PROCESS_ERROR(pSaveSceneData);

	pSaveSceneData->wProtocolID		= s2r_save_scene_data;
	pSaveSceneData->dwMapID			= dwMapID;
	pSaveSceneData->nMapCopyIndex		= nMapCopyIndex;
	pSaveSceneData->uDataLen		= uLength;
	memcpy(pSaveSceneData->byData, pSaveData, uLength);

	nRetCode = Send(piSendBuffer);
	KG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

bool KGameServerSimulator::DoGetMapDataResquest(DWORD dwMapID, __int64 nMapCopyIndex)
{
	bool				    bResult					= false;
	//    int					    nRetCode				= false;
	//    IKG_Buffer*			    piSendBuffer			= NULL;
	//    S2R_APPLY_SCENE_DATA_REQUEST* pApplyMapData        = NULL;
	//
	//    piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_APPLY_SCENE_DATA_REQUEST));
	//    KGLOG_PROCESS_ERROR(piSendBuffer);
	//
	//    pApplyMapData = (S2R_APPLY_SCENE_DATA_REQUEST*)piSendBuffer->GetData();
	//    KGLOG_PROCESS_ERROR(pApplyMapData);
	//
	//    pApplyMapData->wProtocolID		= s2r_apply_scene_data_request;
	//    pApplyMapData->dwMapID			= dwMapID;
	//    pApplyMapData->nMapCopyIndex	= nMapCopyIndex;
	//
	//    nRetCode = Send(piSendBuffer);
	//    KG_PROCESS_ERROR(nRetCode);
	//
	//    bResult = true;
	//Exit0:
	//    KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

bool KGameServerSimulator::CheckApplyCustmonSceneDataRespond(DWORD dwMapID, __int64 nMapCopyIndex, char* pSaveData, size_t uLength)
{
	bool                        bResult         = false;
	//    int                         nRetCode        = false;
	//    IKG_Buffer*                 piPackage       = NULL;
	//    R2S_APPLY_SCENE_DATA_RESPOND*   pRespond		= NULL;
	//
	//    piPackage = ReceiveDataOnce(r2s_apply_scene_data_respond);
	//    KGLOG_PROCESS_ERROR(nRetCode);
	//
	//    pRespond = (R2S_APPLY_SCENE_DATA_RESPOND*)piPackage->GetData();
	//    KGLOG_PROCESS_ERROR(pRespond);
	//
	//    KG_PROCESS_ERROR(pRespond->dwMapID == dwMapID);
	//    KG_PROCESS_ERROR(pRespond->nMapCopyIndex == nMapCopyIndex);
	//    KG_PROCESS_ERROR(pRespond->uDataLen == uLength);
	//    nRetCode = memcmp(pRespond->byData, pSaveData, uLength);
	//    KG_PROCESS_ERROR(nRetCode == 0);
	//
	//    bResult = true;
	//Exit0:
	//    KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::DoResetMapCopyRequest(DWORD dwPlayerID, DWORD dwMapID, __int64  nMapCopyIndex)
{
	bool				    bResult					= false;
//	int					    nRetCode				= false;
//	IKG_Buffer*			    piSendBuffer			= NULL;
//	S2R_RESET_MAP_COPY_REQUEST* pResetMapCopy        = NULL;
//
//	piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_RESET_MAP_COPY_REQUEST));
//	KGLOG_PROCESS_ERROR(piSendBuffer);
//
//	pResetMapCopy = (S2R_RESET_MAP_COPY_REQUEST*)piSendBuffer->GetData();
//	KGLOG_PROCESS_ERROR(pResetMapCopy);
//
//	pResetMapCopy->wProtocolID		= s2r_reset_map_copy_request;
//	pResetMapCopy->dwMapID			= dwMapID;
//	pResetMapCopy->nMapCopyIndex	= nMapCopyIndex;
//	pResetMapCopy->dwPlayerID       = dwPlayerID;
//
//	nRetCode = Send(piSendBuffer);
//	KG_PROCESS_ERROR(nRetCode);
//
//	bResult = true;
//Exit0:
//	KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

bool KGameServerSimulator::CheckRefreshMapCopyPrepare(DWORD dwMapID, __int64 nMapCopyIndex)
{
	bool                        bResult         = false;
	int                         nRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
	//R2S_REFRESH_MAP_COPY_PREPARE*   pRespond		= NULL;

	//piPackage = ReceiveDataOnce(r2s_refresh_map_copy_prepare);
	//KGLOG_PROCESS_ERROR(nRetCode);

	//pRespond = (R2S_REFRESH_MAP_COPY_PREPARE*)piPackage->GetData();
	//KGLOG_PROCESS_ERROR(pRespond);

	//KG_PROCESS_ERROR(pRespond->dwMapID == dwMapID);
	//KG_PROCESS_ERROR(pRespond->nMapCopyIndex == nMapCopyIndex);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::CheckSetMapOwer(DWORD dwPlayerID, DWORD dwMapID, __int64 nMapCopyIndex)
{
	bool                        bResult         = false;
	int                         nRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_SET_MAP_COPY_OWNER*   pRespond		= NULL;

	piPackage = ReceiveDataOnce(r2s_set_map_copy_owner);
	KGLOG_PROCESS_ERROR(piPackage);

	pRespond = (R2S_SET_MAP_COPY_OWNER*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pRespond);

	KG_PROCESS_ERROR(pRespond->dwMapID == dwMapID);
	KG_PROCESS_ERROR(pRespond->nMapCopyIndex == nMapCopyIndex);
	KG_PROCESS_ERROR(pRespond->dwOwnerID == dwPlayerID);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::DoPlayerEnterSceneNotify(DWORD dwPlayerID, DWORD dwMapID, __int64 nMapCopyIndex)
{
	bool				    bResult					= false;
	int					    nRetCode				= false;
	IKG_Buffer*			    piSendBuffer			= NULL;
	S2R_PLAYER_ENTER_SCENE_NOTIFY* pResetMapCopy        = NULL;

	piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_PLAYER_ENTER_SCENE_NOTIFY));
	KGLOG_PROCESS_ERROR(piSendBuffer);

	pResetMapCopy = (S2R_PLAYER_ENTER_SCENE_NOTIFY*)piSendBuffer->GetData();
	KGLOG_PROCESS_ERROR(pResetMapCopy);

	pResetMapCopy->wProtocolID		= s2r_player_enter_scene_notify;
	pResetMapCopy->dwMapID			= dwMapID;
	pResetMapCopy->nMapCopyIndex	= nMapCopyIndex;
	pResetMapCopy->dwPlayerID       = dwPlayerID;

	nRetCode = Send(piSendBuffer);
	KG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

bool KGameServerSimulator::DoSaveRoleData(DWORD dwPlayerID, int nUserValue, KRoleBaseInfo& BaseInfo, size_t uRoleDataLen)
{
	bool				    bResult					= false;
	int					    nRetCode				= false;
	IKG_Buffer*			    piSendBuffer			= NULL;
	S2R_SAVE_ROLE_DATA*     pSaveRole               = NULL;

	piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_SAVE_ROLE_DATA));
	    KGLOG_PROCESS_ERROR(piSendBuffer);
	
	    pSaveRole = (S2R_SAVE_ROLE_DATA*)piSendBuffer->GetData();
	    KGLOG_PROCESS_ERROR(pSaveRole);
	
	    pSaveRole->wProtocolID		= s2r_save_role_data;
	    pSaveRole->dwRoleID		= dwPlayerID;
	    pSaveRole->nUserValue	    = nUserValue;
	    pSaveRole->uRoleDataLen      = uRoleDataLen;
	    pSaveRole->BaseInfo = BaseInfo;
	
	    nRetCode = Send(piSendBuffer);
	    KG_PROCESS_ERROR(nRetCode);
	
	    bResult = true;
	Exit0:
	    KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

bool KGameServerSimulator::DoSyncRoleData(DWORD   dwRoleID, size_t  uOffset, BYTE byData[], size_t uLength)
{
	bool				    bResult					= false;
	int					    nRetCode				= false;
	IKG_Buffer*			    piSendBuffer			= NULL;
	S2R_SYNC_ROLE_DATA*     pSaveRole               = NULL;

	piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_SYNC_ROLE_DATA) + uLength);
	KGLOG_PROCESS_ERROR(piSendBuffer);

	pSaveRole = (S2R_SYNC_ROLE_DATA*)piSendBuffer->GetData();
	KGLOG_PROCESS_ERROR(pSaveRole);

	pSaveRole->wProtocolID		= s2r_sync_role_data;
	pSaveRole->dwRoleID		= dwRoleID;
	pSaveRole->uOffset	    = uOffset;
	memcpy(pSaveRole->byData, byData, uLength);

	nRetCode = Send(piSendBuffer);
	KG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

bool KGameServerSimulator::DoLoadRoleData(DWORD dwPlayerID)
{
	bool				    bResult					= false;
	int					    nRetCode				= false;
	IKG_Buffer*			    piSendBuffer			= NULL;
	S2R_LOAD_ROLE_DATA_REQUEST*     pLoadRole               = NULL;

	piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_LOAD_ROLE_DATA_REQUEST));
	KGLOG_PROCESS_ERROR(piSendBuffer);

	pLoadRole = (S2R_LOAD_ROLE_DATA_REQUEST*)piSendBuffer->GetData();
	KGLOG_PROCESS_ERROR(pLoadRole);

	pLoadRole->wProtocolID		= s2r_load_role_data_request;
	pLoadRole->dwPlayerID		= dwPlayerID;

	nRetCode = Send(piSendBuffer);
	KG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

bool KGameServerSimulator::CheckSaveRoleDataRespond(DWORD dwPlayerID, bool bSucceed, int nUserValue)
{
	bool                        bResult         = false;
	int                         nRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_SAVE_ROLE_DATA_RESPOND*   pRespond		= NULL;

	piPackage = ReceiveDataOnce(r2s_save_role_data_respond);
	KGLOG_PROCESS_ERROR(piPackage);

	pRespond = (R2S_SAVE_ROLE_DATA_RESPOND*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pRespond);

	KG_PROCESS_ERROR(pRespond->dwPlayerID == dwPlayerID);
	KG_PROCESS_ERROR(pRespond->bSucceed == bSucceed);
	KG_PROCESS_ERROR(pRespond->nUserValue == nUserValue);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::CheckLoadRoleDataRespond(DWORD dwPlayerID, bool bSucceed, BYTE* pbyRoleData, unsigned uRoleDataLen)
{
	bool                        bResult         = false;
	int                         nRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_LOAD_ROLE_DATA*   pRespond		= NULL;

	piPackage = ReceiveDataOnce(r2s_load_role_data);
	KGLOG_PROCESS_ERROR(piPackage);


	pRespond = (R2S_LOAD_ROLE_DATA*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pRespond);

	KG_PROCESS_ERROR(pRespond->dwRoleID == dwPlayerID);
	KG_PROCESS_ERROR(pRespond->bSucceed == bSucceed);


	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::DoUpdatePerformance(int nIdleFrame, size_t uMemory, int nConnectionCount)
{
	bool				    bResult					= false;
	int					    nRetCode				= false;
	IKG_Buffer*			    piSendBuffer			= NULL;
	S2R_UPDATE_PERFORMANCE*     pSaveRole               = NULL;

	piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_UPDATE_PERFORMANCE));
	KGLOG_PROCESS_ERROR(piSendBuffer);

	pSaveRole = (S2R_UPDATE_PERFORMANCE*)piSendBuffer->GetData();
	KGLOG_PROCESS_ERROR(pSaveRole);

	pSaveRole->wProtocolID		= s2r_update_performance;
	pSaveRole->nIdleFrame		= nIdleFrame;
	pSaveRole->uMemory	    = uMemory;
	pSaveRole->nConnectionCount      = nConnectionCount;

	nRetCode = Send(piSendBuffer);
	KG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

bool KGameServerSimulator::DoPartySetLootModeRequest(DWORD dwPartyID, DWORD dwRequestSrc, int nLootMode)
{
	bool                 bResult				= false;
	int                  nRetCode			    = false;
	IKG_Buffer*          piSendBuffer			= NULL;
	S2R_TEAM_SET_LOOT_MODE_REQUEST*      pLootModeRequest   = NULL;

	piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_TEAM_SET_LOOT_MODE_REQUEST));
	KGLOG_PROCESS_ERROR(piSendBuffer);

	pLootModeRequest = (S2R_TEAM_SET_LOOT_MODE_REQUEST*)piSendBuffer->GetData();
	KGLOG_PROCESS_ERROR(pLootModeRequest);

	pLootModeRequest->wProtocolID	   = s2r_team_set_loot_mode_request;
	pLootModeRequest->dwTeamID        = dwPartyID;
	pLootModeRequest->dwRequestSrc       = dwRequestSrc;
	pLootModeRequest->nLootMode         = nLootMode;

	nRetCode = Send(piSendBuffer);
	KG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

bool KGameServerSimulator::CheckPartySetLootModeRequest(DWORD dwPartyID, int nLootMode)
{
	bool                        bResult         = false;
	int                         nRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_TEAM_SET_LOOT_MODE_NOTIFY*   pRespond		= NULL;

	piPackage = ReceiveDataOnce(r2s_team_set_loot_mode_notify);
	KGLOG_PROCESS_ERROR(piPackage);

	pRespond = (R2S_TEAM_SET_LOOT_MODE_NOTIFY*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pRespond);

	KG_PROCESS_ERROR(pRespond->dwTeamID == dwPartyID);
	KG_PROCESS_ERROR(pRespond->nLootMode == nLootMode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::DoPartySetRollQualityRequest(DWORD dwPartyID, DWORD dwRequestSrc, int nRollQuality)
{
	bool                 bResult				= false;
	int                  nRetCode			    = false;
	IKG_Buffer*          piSendBuffer			= NULL;
	S2R_TEAM_SET_ROLL_QUALITY_REQUEST*      pLootModeRequest   = NULL;

	piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_TEAM_SET_ROLL_QUALITY_REQUEST));
	KGLOG_PROCESS_ERROR(piSendBuffer);

	pLootModeRequest = (S2R_TEAM_SET_ROLL_QUALITY_REQUEST*)piSendBuffer->GetData();
	KGLOG_PROCESS_ERROR(pLootModeRequest);

	pLootModeRequest->wProtocolID	   = s2r_team_set_roll_quality_request;
	pLootModeRequest->dwTeamID        = dwPartyID;
	pLootModeRequest->dwRequestSrc       = dwRequestSrc;
	pLootModeRequest->nRollQuality         = nRollQuality;

	nRetCode = Send(piSendBuffer);
	KG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

bool KGameServerSimulator::CheckPartySetRollQualityRequest(DWORD dwPartyID, int nRollQuality)
{
	bool                        bResult         = false;
	int                         nRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_TEAM_SET_LOOT_MODE_NOTIFY*   pRespond		= NULL;

	piPackage = ReceiveDataOnce(r2s_team_set_loot_mode_notify);
	KGLOG_PROCESS_ERROR(piPackage);

	pRespond = (R2S_TEAM_SET_LOOT_MODE_NOTIFY*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pRespond);

	KG_PROCESS_ERROR(pRespond->dwTeamID == dwPartyID);
	KG_PROCESS_ERROR(pRespond->nRollQuality == nRollQuality);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::DoPartySetFormationLeader(DWORD dwPartyID, DWORD dwRequestSrc, DWORD dwNewFormationLeader, int   nGourpIndex)
{
	bool                 bResult				= false;
	int                  nRetCode			    = false;
	IKG_Buffer*          piSendBuffer			= NULL;
	S2R_TEAM_SET_FORMATION_LEADER_REQUEST*      pRequest   = NULL;

	piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_TEAM_SET_FORMATION_LEADER_REQUEST));
	KGLOG_PROCESS_ERROR(piSendBuffer);

	pRequest = (S2R_TEAM_SET_FORMATION_LEADER_REQUEST*)piSendBuffer->GetData();
	KGLOG_PROCESS_ERROR(pRequest);

	pRequest->wProtocolID	   = s2r_team_set_formation_leader_request;
	pRequest->dwTeamID        = dwPartyID;
	pRequest->dwRequestSrc       = dwRequestSrc;
	pRequest->dwNewFormationLeader         = dwNewFormationLeader;
	pRequest->nGroupIndex         = nGourpIndex;

	nRetCode = Send(piSendBuffer);
	KG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

bool KGameServerSimulator::CheckPartySetFormationLeader(DWORD dwPartyID, DWORD dwNewFormationLeader)
{
	bool                        bResult         = false;
	int                         nRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_TEAM_SET_FORMATION_LEADER_NOTIFY*   pRespond		= NULL;

	piPackage = ReceiveDataOnce(r2s_team_set_formation_leader_notify);
	KGLOG_PROCESS_ERROR(piPackage);

	pRespond = (R2S_TEAM_SET_FORMATION_LEADER_NOTIFY*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pRespond);

	KG_PROCESS_ERROR(pRespond->dwTeamID == dwPartyID);
	KG_PROCESS_ERROR(pRespond->dwNewFormationLeader == dwNewFormationLeader);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}


bool KGameServerSimulator::DoApplyFellowshipPlayerLevel(DWORD dwPlayerID)
{
	bool                 bResult				= false;
	int                  nRetCode			    = false;
	IKG_Buffer*          piSendBuffer			= NULL;
	S2R_APPLY_FELLOWSHIP_PLAYER_LEVEL_AND_FORCEID*      pRequest   = NULL;

	piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_APPLY_FELLOWSHIP_PLAYER_LEVEL_AND_FORCEID));
	KGLOG_PROCESS_ERROR(piSendBuffer);

	pRequest = (S2R_APPLY_FELLOWSHIP_PLAYER_LEVEL_AND_FORCEID*)piSendBuffer->GetData();
	KGLOG_PROCESS_ERROR(pRequest);

	pRequest->wProtocolID	   = s2r_apply_fellowship_player_level_and_forceid;
	pRequest->dwPlayerID        = dwPlayerID;

	nRetCode = Send(piSendBuffer);
	KG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

bool KGameServerSimulator::CheckSyncFellowshipPlayerLevel(DWORD dwPlayerID, int nLevel)
{
	bool                        bResult         = false;
	int                         nRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_SYNC_FELLOWSHIP_PLAYER_LEVEL*   pRespond		= NULL;

	piPackage = ReceiveDataOnce(r2s_sync_fellowship_player_level);
	KGLOG_PROCESS_ERROR(piPackage);

	pRespond = (R2S_SYNC_FELLOWSHIP_PLAYER_LEVEL*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pRespond);

	KG_PROCESS_ERROR(pRespond->dwPlayerID == dwPlayerID);
	KG_PROCESS_ERROR(pRespond->byLevel == nLevel);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}


bool KGameServerSimulator::DoRefreshMapCopyRequest(DWORD dwMapID, int64_t  nMapCopyIndex)
{
	bool                 bResult				= false;
	int                  nRetCode			    = false;
	IKG_Buffer*          piSendBuffer			= NULL;
	//S2R_REFRESH_MAP_COPY_REQUEST*      pRequest   = NULL;

	//piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_REFRESH_MAP_COPY_REQUEST));
	//KGLOG_PROCESS_ERROR(piSendBuffer);

	//pRequest = (S2R_REFRESH_MAP_COPY_REQUEST*)piSendBuffer->GetData();
	//KGLOG_PROCESS_ERROR(pRequest);

	//pRequest->wProtocolID	   = s2r_refresh_map_copy_request;
	//pRequest->dwMapID        = dwMapID;
	//pRequest->nMapCopyIndex        = nMapCopyIndex;

	//nRetCode = Send(piSendBuffer);
	//KG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piSendBuffer);
	return bResult;
}

bool KGameServerSimulator::DoApplyCustomSceneDataRequest(DWORD dwMapID, int64_t  nMapCopyIndex)
{
	bool                 bResult				= false;
	//    int                  nRetCode			    = false;
	//    IKG_Buffer*          piSendBuffer			= NULL;
	//    S2R_APPLY_SCENE_DATA_REQUEST*      pRequest   = NULL;
	//
	//    piSendBuffer = KG_MemoryCreateBuffer(sizeof(S2R_APPLY_SCENE_DATA_REQUEST));
	//    KGLOG_PROCESS_ERROR(piSendBuffer);
	//
	//    pRequest = (S2R_APPLY_SCENE_DATA_REQUEST*)piSendBuffer->GetData();
	//    KGLOG_PROCESS_ERROR(pRequest);
	//
	//    pRequest->wProtocolID	   = s2r_apply_scene_data_request;
	//    pRequest->dwMapID        = dwMapID;
	//    pRequest->nMapCopyIndex        = nMapCopyIndex;
	//
	//    nRetCode = Send(piSendBuffer);
	//    KG_PROCESS_ERROR(nRetCode);
	//
	//    bResult = true;
	//Exit0:
	//    KG_COM_RELEASE(piSendBuffer);
	return bResult;
}


bool KGameServerSimulator::CheckGmCommand(DWORD dwPlayerID, const char cszGmName[], const char cszGmCommand[])
{
	bool                        bResult         = false;
	int                         nRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_GM_COMMAND*   pRespond		= NULL;

	piPackage = ReceiveDataOnce(r2s_gm_command);
	KGLOG_PROCESS_ERROR(piPackage);

	pRespond = (R2S_GM_COMMAND*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pRespond);

	KG_PROCESS_ERROR(pRespond->dwPlayerID == dwPlayerID);
	nRetCode = strcmp(pRespond->szGmName, cszGmName);
	KG_PROCESS_ERROR(nRetCode == 0);
	nRetCode = strcmp(pRespond->szGmCommand, cszGmCommand);
	KG_PROCESS_ERROR(nRetCode == 0);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::CheckKickOut(DWORD dwPlayerID)
{
	bool                        bResult         = false;
	int                         nRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_KICK_ACCOUNT_NOTIFY*   pRespond		= NULL;

	piPackage = ReceiveDataOnce(r2s_kick_account_notify);
	KGLOG_PROCESS_ERROR(piPackage);

	pRespond = (R2S_KICK_ACCOUNT_NOTIFY*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pRespond);

	KG_PROCESS_ERROR(pRespond->dwPlayerID == dwPlayerID);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::CheckGlobalSysMsg(BYTE  byMsgType, DWORD dwTalkerID, const char  szTalker[],
											 DWORD dwReceiverID, const char  szReceiver[], BYTE  byTalkData[])
{
	bool                        bResult         = false;
	int                         nRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_TALK_MESSAGE*   pRespond		= NULL;

	piPackage = ReceiveDataOnce(r2s_talk_message);
	KGLOG_PROCESS_ERROR(piPackage);

	pRespond = (R2S_TALK_MESSAGE*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pRespond);

	KGLOG_PROCESS_ERROR(pRespond->byMsgType == byMsgType);
	KGLOG_PROCESS_ERROR(pRespond->dwTalkerID == dwTalkerID);
	KGLOG_PROCESS_ERROR(pRespond->dwReceiverID == dwReceiverID);
	nRetCode = strncmp(pRespond->szTalker, szTalker, sizeof(pRespond->szTalker));
	KG_PROCESS_ERROR(nRetCode == 0);
	nRetCode = strncmp(pRespond->szReceiver, szReceiver, sizeof(pRespond->szReceiver));
	KG_PROCESS_ERROR(nRetCode == 0);
	nRetCode = strcmp((char*)pRespond->byTalkData, (char*)byTalkData);
	KG_PROCESS_ERROR(nRetCode == 0);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::DoHandshakeRequest()
{
	bool                    bResult         = false;
	bool                    bRetCode        = false;
	IKG_Buffer*             piPackage       = NULL;
	S2R_HANDSHAKE_REQUEST*  pHandshake      = NULL;

	piPackage = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_HANDSHAKE_REQUEST));
	KGLOG_PROCESS_ERROR(piPackage);

	pHandshake = (S2R_HANDSHAKE_REQUEST*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pHandshake);

	pHandshake->wProtocolID            = s2r_handshake_request;
	pHandshake->nGameWorldLowerVersion = GAME_WORLD_LOWEST_VERSION;
	pHandshake->nGameWorldUpperVersion = GAME_WORLD_CURRENT_VERSION;

	bRetCode = Send(piPackage);
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::CheckHandshakeRespond()
{
	bool                        bResult         = false;
	int                         nRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_HANDSHAKE_RESPOND*   pRespond		= NULL;

	piPackage = ReceiveDataOnce(r2s_handshake_respond);
	KGLOG_PROCESS_ERROR(piPackage);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}


bool KGameServerSimulator::DoJoinBattleFieldQueueRequest(DWORD dwMapID, int64_t nCopyIndex, DWORD dwGroupID, bool bTeamJoin, DWORD dwRoleID)
{
	bool                    bResult         = false;
	bool                    bRetCode        = false;
	IKG_Buffer*             piPackage       = NULL;
	S2R_JOIN_BATTLE_FIELD_QUEUE_REQUEST*    pJoinRequest    = NULL;

	piPackage = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_JOIN_BATTLE_FIELD_QUEUE_REQUEST));
	KGLOG_PROCESS_ERROR(piPackage);

	pJoinRequest = (S2R_JOIN_BATTLE_FIELD_QUEUE_REQUEST*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pJoinRequest);

	pJoinRequest->wProtocolID            = s2r_join_battle_field_queue_request;
	pJoinRequest->bTeamJoin				 = bTeamJoin;
	pJoinRequest->dwGroupID				= dwGroupID;
	pJoinRequest->dwMapID				= dwMapID;
	pJoinRequest->dwRoleID				= dwRoleID;
	pJoinRequest->nCopyIndex			= nCopyIndex;

	bRetCode = Send(piPackage);
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::CheckJoinBattleFieldQueueRespond(DWORD dwPlayerID, DWORD dwMapID, int nErrorCode)
{
	bool                        bResult         = false;
	int                         nRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_JOIN_BATTLE_FIELD_QUEUE_RESPOND*   pRespond		= NULL;

	piPackage = ReceiveDataOnce(r2s_join_battle_field_queue_respond);
	KGLOG_PROCESS_ERROR(piPackage);

	pRespond = (R2S_JOIN_BATTLE_FIELD_QUEUE_RESPOND*)piPackage->GetData();
	KG_PROCESS_ERROR(pRespond);

	KG_PROCESS_ERROR(pRespond->dwMapID == dwMapID);
	KG_PROCESS_ERROR(pRespond->dwPlayerID == dwPlayerID);
	KG_PROCESS_ERROR(pRespond->nErrorCode == nErrorCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::DoChangeRoleLevelRequest(DWORD dwPlayerID, int nLevel)
{
	bool                    bResult         = false;
	bool                    bRetCode        = false;
	IKG_Buffer*             piPackage       = NULL;
	S2R_CHANGE_ROLE_LEVEL_REQUEST*    pRequest    = NULL;

	piPackage = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_CHANGE_ROLE_LEVEL_REQUEST));
	KGLOG_PROCESS_ERROR(piPackage);

	pRequest = (S2R_CHANGE_ROLE_LEVEL_REQUEST*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pRequest);

	pRequest->wProtocolID            = s2r_change_role_level_request;
	pRequest->byLevel	= nLevel;
	pRequest->dwPlayerID				= dwPlayerID;

	bRetCode = Send(piPackage);
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::DoChangeRoleForceIDRequest(DWORD dwPlayerID, BYTE  byForceID)
{
	bool                    bResult         = false;
	bool                    bRetCode        = false;
	IKG_Buffer*             piPackage       = NULL;
	S2R_CHANGE_ROLE_FORCEID_REQUEST*    pRequest    = NULL;

	piPackage = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_CHANGE_ROLE_FORCEID_REQUEST));
	KGLOG_PROCESS_ERROR(piPackage);

	pRequest = (S2R_CHANGE_ROLE_FORCEID_REQUEST*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pRequest);

	pRequest->wProtocolID            = s2r_change_role_forceid_request;
	pRequest->byForceID	= byForceID;
	pRequest->dwPlayerID				= dwPlayerID;

	bRetCode = Send(piPackage);
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}


bool KGameServerSimulator::CheckSyncFellowshipPlayerForceID(DWORD dwPlayerID, BYTE  byForceID)
{
	bool                        bResult         = false;
	int                         nRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_SYNC_FELLOWSHIP_PLAYER_FORCEID*   pRespond		= NULL;

	piPackage = ReceiveDataOnce(r2s_sync_fellowship_player_forceid);
	KGLOG_PROCESS_ERROR(piPackage);

	pRespond = (R2S_SYNC_FELLOWSHIP_PLAYER_FORCEID*)piPackage->GetData();
	KG_PROCESS_ERROR(pRespond);

	KG_PROCESS_ERROR(pRespond->dwPlayerID == dwPlayerID);
	KG_PROCESS_ERROR(pRespond->byForceID == byForceID);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::CheckBattleFieldNotify()
{
	bool                        bResult         = false;
	int                         nRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_BATTLE_FIELD_NOTIFY*   pRespond		= NULL;

	piPackage = ReceiveDataOnce(r2s_battle_field_notify);
	KGLOG_PROCESS_ERROR(piPackage);

	pRespond = (R2S_BATTLE_FIELD_NOTIFY*)piPackage->GetData();
	KG_PROCESS_ERROR(pRespond);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::CheckBattleFieldNotify(R2S_BATTLE_FIELD_NOTIFY& BattleFifldNotify)
{
	bool                        bResult         = false;
	int                         nRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_BATTLE_FIELD_NOTIFY*   pRespond		= NULL;

	piPackage = ReceiveDataOnce(r2s_battle_field_notify);
	KGLOG_PROCESS_ERROR(piPackage);

	pRespond = (R2S_BATTLE_FIELD_NOTIFY*)piPackage->GetData();
	KG_PROCESS_ERROR(pRespond);

	BattleFifldNotify = *pRespond;

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}


void KGameServerSimulator::ClearReceiveData()
{
	int                  nRetCode        = false;
	timeval              TimeoutValue    = {1, 0};
	IKG_Buffer*          piBuffer        = NULL;

	KGLOG_PROCESS_ERROR(m_piSocketStream);

	while (true)
	{
		nRetCode = m_piSocketStream->CheckCanRecv(&TimeoutValue);
		KG_PROCESS_ERROR(nRetCode == 1);

		nRetCode = m_piSocketStream->Recv(&piBuffer);
		KG_PROCESS_ERROR(nRetCode == 1);

		KG_COM_RELEASE(piBuffer);
	}

Exit0:
	KG_COM_RELEASE(piBuffer);
	return;
}

bool KGameServerSimulator::DoAcceptJoinBattleField(DWORD   dwMapID, int64_t nCopyIndex, DWORD   dwRoleID)
{
	bool                    bResult         = false;
	bool                    bRetCode        = false;
	IKG_Buffer*             piPackage       = NULL;
	S2R_ACCEPT_JOIN_BATTLE_FIELD*    pRequest    = NULL;

	piPackage = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_ACCEPT_JOIN_BATTLE_FIELD));
	KGLOG_PROCESS_ERROR(piPackage);

	pRequest = (S2R_ACCEPT_JOIN_BATTLE_FIELD*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pRequest);

	pRequest->wProtocolID            = s2r_accept_join_battle_field;
	pRequest->dwMapID	= dwMapID;
	pRequest->nCopyIndex				= nCopyIndex;
	pRequest->dwRoleID = dwRoleID;

	bRetCode = Send(piPackage);
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}


bool KGameServerSimulator::CheckAcceptJoinBattleFieldRespond(DWORD  dwMapID, int64_t nCopyIndex, DWORD   dwRoleID)
{
	bool                        bResult         = false;
	int                         nRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_ACCEPT_JOIN_BATTLE_FIELD_RESPOND*   pRespond		= NULL;

	piPackage = ReceiveDataOnce(r2s_accept_join_battle_field_respond);
	KGLOG_PROCESS_ERROR(piPackage);

	pRespond = (R2S_ACCEPT_JOIN_BATTLE_FIELD_RESPOND*)piPackage->GetData();
	KG_PROCESS_ERROR(pRespond);

	KG_PROCESS_ERROR(pRespond->dwPlayerID == dwRoleID);
	KG_PROCESS_ERROR(pRespond->dwEnterMapID == dwMapID);
	KG_PROCESS_ERROR(pRespond->nEnterCopyIndex == nCopyIndex);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::DoLeaveBattleFieldQueueRequest(DWORD dwMapID, DWORD dwRoleID)
{
	bool                    bResult         = false;
	bool                    bRetCode        = false;
	IKG_Buffer*             piPackage       = NULL;
	S2R_LEAVE_BATTLE_FIELD_QUEUE_REQUEST*    pRequest    = NULL;

	piPackage = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_LEAVE_BATTLE_FIELD_QUEUE_REQUEST));
	KGLOG_PROCESS_ERROR(piPackage);

	pRequest = (S2R_LEAVE_BATTLE_FIELD_QUEUE_REQUEST*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pRequest);

	pRequest->wProtocolID            = s2r_leave_battle_field_queue_request;
	pRequest->dwMapID	= dwMapID;
	pRequest->dwRoleID = dwRoleID;

	bRetCode = Send(piPackage);
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}


bool KGameServerSimulator::CheckLeaveBattleFieldQueueRespond(    DWORD   dwPlayerID, DWORD   dwMapID, int     nErrorCode)
{
	bool                        bResult         = false;
	int                         nRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_LEAVE_BATTLE_FIELD_QUEUE_RESPOND*   pRespond		= NULL;

	piPackage = ReceiveDataOnce(r2s_leave_battle_field_queue_respond);
	KGLOG_PROCESS_ERROR(piPackage);

	pRespond = (R2S_LEAVE_BATTLE_FIELD_QUEUE_RESPOND*)piPackage->GetData();
	KG_PROCESS_ERROR(pRespond);

	KG_PROCESS_ERROR(pRespond->dwPlayerID == dwPlayerID);
	KG_PROCESS_ERROR(pRespond->dwMapID == dwMapID);
	KG_PROCESS_ERROR(pRespond->nErrorCode == nErrorCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::DoGetBattleFieldList(DWORD dwPlayerID, DWORD dwMapID)
{
	bool                    bResult         = false;
	bool                    bRetCode        = false;
	IKG_Buffer*             piPackage       = NULL;
	S2R_GET_BATTLE_FIELD_LIST*    pRequest    = NULL;

	piPackage = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_GET_BATTLE_FIELD_LIST));
	KGLOG_PROCESS_ERROR(piPackage);

	pRequest = (S2R_GET_BATTLE_FIELD_LIST*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pRequest);

	pRequest->wProtocolID            = s2r_get_battle_field_list;
	pRequest->dwMapID	= dwMapID;
	pRequest->dwPlayerID = dwPlayerID;

	bRetCode = Send(piPackage);
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::CheckSyncBattleFieldList(DWORD dwPlayerID, DWORD   dwMapID, int64_t nCopyIndex)
{
	bool                        bResult         = false;
	int                         nRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_SYNC_BATTLE_FIELD_LIST*   pRespond		= NULL;

	piPackage = ReceiveDataOnce(r2s_sync_battle_field_list);
	KGLOG_PROCESS_ERROR(piPackage);
	pRespond = (R2S_SYNC_BATTLE_FIELD_LIST*)piPackage->GetData();
	KG_PROCESS_ERROR(pRespond);

	KG_PROCESS_ERROR(pRespond->dwPlayerID == dwPlayerID);
	KG_PROCESS_ERROR(pRespond->dwMapID == dwMapID);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::CheckSyncPQ(R2S_SYNC_PQ& PQInfo)
{
	bool                        bResult         = false;
	int                         nRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_SYNC_PQ*   pRespond		= NULL;

	piPackage = ReceiveDataOnce(r2s_sync_pq);
	KGLOG_PROCESS_ERROR(piPackage);

	pRespond = (R2S_SYNC_PQ*)piPackage->GetData();
	KG_PROCESS_ERROR(pRespond);

	PQInfo = *pRespond;

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}


bool KGameServerSimulator::CheckDeletePQ(DWORD dwPQID)
{
	bool                        bResult         = false;
	int                         nRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_DELETE_PQ*   pRespond		= NULL;

	piPackage = ReceiveDataOnce(r2s_delete_pq);
	KGLOG_PROCESS_ERROR(piPackage);

	pRespond = (R2S_DELETE_PQ*)piPackage->GetData();
	KG_PROCESS_ERROR(pRespond);

	KG_PROCESS_ERROR(pRespond->dwPQID == dwPQID);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::CheckMapCopyDoClearPlayerPrepare(DWORD dwMapID, int64_t  nMapCopyIndex, int      nType, int      nDelayTime)
{
	bool                        bResult         = false;
	int                         nRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_MAP_COPY_DO_CLEAR_PLAYER_PREPARE*   pRespond		= NULL;

	piPackage = ReceiveDataOnce(r2s_map_copy_do_clear_player_prepare);
	KGLOG_PROCESS_ERROR(piPackage);

	pRespond = (R2S_MAP_COPY_DO_CLEAR_PLAYER_PREPARE*)piPackage->GetData();
	KG_PROCESS_ERROR(pRespond);

	KG_PROCESS_ERROR(pRespond->dwMapID == dwMapID);
	KG_PROCESS_ERROR(pRespond->nMapCopyIndex == nMapCopyIndex);
	KG_PROCESS_ERROR(pRespond->nType == nType);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}


bool KGameServerSimulator::CheckMapCopyDoClearPlayer(DWORD dwMapID, int64_t  nMapCopyIndex)
{
	bool                        bResult         = false;
	int                         nRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_MAP_COPY_DO_CLEAR_PLAYER*   pRespond		= NULL;

	piPackage = ReceiveDataOnce(r2s_map_copy_do_clear_player);
	KGLOG_PROCESS_ERROR(piPackage);

	pRespond = (R2S_MAP_COPY_DO_CLEAR_PLAYER*)piPackage->GetData();
	KG_PROCESS_ERROR(pRespond);

	KG_PROCESS_ERROR(pRespond->dwMapID == dwMapID);
	KG_PROCESS_ERROR(pRespond->nMapCopyIndex == nMapCopyIndex);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::DoAuctionSellRequest(S2R_AUCTION_SELL_REQUEST& AuctionSellRequest)
{
	bool                    bResult         = false;
	bool                    bRetCode        = false;
	IKG_Buffer*             piPackage       = NULL;
	S2R_AUCTION_SELL_REQUEST*    pRequest    = NULL;

	piPackage = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_AUCTION_SELL_REQUEST) + AuctionSellRequest.uItemDataLen);
	KGLOG_PROCESS_ERROR(piPackage);

	pRequest = (S2R_AUCTION_SELL_REQUEST*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pRequest);

	memcpy(pRequest, &AuctionSellRequest, sizeof(AuctionSellRequest) + AuctionSellRequest.uItemDataLen);
	pRequest->wProtocolID            = s2r_auction_sell_request;

	bRetCode = Send(piPackage);
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::DoAuctionLookupRequest(S2R_AUCTION_LOOKUP_REQUEST& AuctionLookUpRequest)
{
	bool                    bResult         = false;
	bool                    bRetCode        = false;
	IKG_Buffer*             piPackage       = NULL;
	S2R_AUCTION_LOOKUP_REQUEST*    pRequest    = NULL;

	piPackage = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_AUCTION_LOOKUP_REQUEST));
	KGLOG_PROCESS_ERROR(piPackage);

	pRequest = (S2R_AUCTION_LOOKUP_REQUEST*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pRequest);

	memcpy(pRequest, &AuctionLookUpRequest, sizeof(AuctionLookUpRequest));
	pRequest->wProtocolID            = s2r_auction_lookup_request;

	bRetCode = Send(piPackage);
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::DoAuctioBidRequest(DWORD   dwBidderID, DWORD   dwSaleID, int     nPrice, DWORD dwCRC)
{
	bool                    bResult         = false;
	bool                    bRetCode        = false;
	IKG_Buffer*             piPackage       = NULL;
	S2R_AUCTION_BID_REQUEST*    pRequest    = NULL;

	piPackage = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_AUCTION_BID_REQUEST));
	KGLOG_PROCESS_ERROR(piPackage);

	pRequest = (S2R_AUCTION_BID_REQUEST*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pRequest);

	pRequest->wProtocolID            = s2r_auction_bid_request;
	pRequest->dwBidderID = dwBidderID;
	pRequest->dwSaleID = dwSaleID;
	pRequest->nPrice = nPrice;
	pRequest->dwCRC = dwCRC;

	bRetCode = Send(piPackage);
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::DoAuctionCancelRequest(DWORD   dwPlayerID, DWORD   dwSaleID)
{
	bool                    bResult         = false;
	bool                    bRetCode        = false;
	IKG_Buffer*             piPackage       = NULL;
	S2R_AUCTION_CANCEL_REQUEST*    pRequest    = NULL;

	piPackage = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_AUCTION_CANCEL_REQUEST));
	KGLOG_PROCESS_ERROR(piPackage);

	pRequest = (S2R_AUCTION_CANCEL_REQUEST*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pRequest);

	pRequest->wProtocolID            = s2r_auction_cancel_request;
	pRequest->dwPlayerID = dwPlayerID;
	pRequest->dwSaleID = dwSaleID;

	bRetCode = Send(piPackage);
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}


bool KGameServerSimulator::CheckAuctionLookUpRespond(R2S_AUCTION_LOOKUP_RESPOND& AuctionLookUpRespond)
{
	bool                        bResult         = false;
	int                         nRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_MAP_COPY_DO_CLEAR_PLAYER*   pRespond		= NULL;

	piPackage = ReceiveDataOnceTime(r2s_auction_lookup_respond, 5);
	KGLOG_PROCESS_ERROR(piPackage);

	pRespond = (R2S_MAP_COPY_DO_CLEAR_PLAYER*)piPackage->GetData();
	KG_PROCESS_ERROR(pRespond);

	memcpy(&AuctionLookUpRespond, pRespond, piPackage->GetSize());

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}


bool KGameServerSimulator::CheckAuctionBidRespond(DWORD   dwPlayerID, BYTE    byCode, int     nBidPrice)
{
	bool                        bResult         = false;
	int                         nRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_AUCTION_BID_RESPOND*   pRespond		= NULL;

	piPackage = ReceiveDataOnce(r2s_auction_bid_respond);
	KGLOG_PROCESS_ERROR(piPackage);

	pRespond = (R2S_AUCTION_BID_RESPOND*)piPackage->GetData();
	KG_PROCESS_ERROR(pRespond);

	KG_PROCESS_ERROR(pRespond->dwPlayerID == dwPlayerID);
	KG_PROCESS_ERROR(pRespond->byCode == byCode);
	KG_PROCESS_ERROR(pRespond->nBidPrice == nBidPrice);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::CheckAuctionCancelRespond(    DWORD   dwPlayerID, BYTE    byCode)
{
	bool                        bResult         = false;
	int                         nRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_AUCTION_CANCEL_RESPOND*   pRespond		= NULL;

	piPackage = ReceiveDataOnce(r2s_auction_cancel_respond);
	KGLOG_PROCESS_ERROR(piPackage);

	pRespond = (R2S_AUCTION_CANCEL_RESPOND*)piPackage->GetData();
	KG_PROCESS_ERROR(pRespond);

	KG_PROCESS_ERROR(pRespond->dwPlayerID == dwPlayerID);
	KG_PROCESS_ERROR(pRespond->byCode == byCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}

bool KGameServerSimulator::CheckAuctionMessageNotify(    DWORD   dwPlayerID, BYTE    byCode, char    szSaleName[], int     nPrice)
{
	bool                        bResult         = false;
	int                         nRetCode        = false;
	IKG_Buffer*                 piPackage       = NULL;
	R2S_AUCTION_MESSAGE_NOTIFY*   pRespond		= NULL;

	piPackage = ReceiveDataOnce(r2s_auction_message_notify);
	KGLOG_PROCESS_ERROR(piPackage);

	pRespond = (R2S_AUCTION_MESSAGE_NOTIFY*)piPackage->GetData();
	KG_PROCESS_ERROR(pRespond);

	KG_PROCESS_ERROR(pRespond->dwPlayerID == dwPlayerID);
	KG_PROCESS_ERROR(pRespond->byCode == byCode);
	KG_PROCESS_ERROR(pRespond->nPrice == nPrice);
	nRetCode = strcmp(pRespond->szSaleName, szSaleName);
	KG_PROCESS_ERROR(nRetCode == 0);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}


bool KGameServerSimulator::DoSyncMentorData(DWORD dwMentorID, DWORD dwApprenticeID)
{
	bool                    bResult         = false;
	bool                    bRetCode        = false;
	IKG_Buffer*             piPackage       = NULL;
	S2R_APPLY_MENTOR_DATA*    pRequest    = NULL;

	piPackage = KG_MemoryCreateBuffer((unsigned)sizeof(S2R_APPLY_MENTOR_DATA ));
	KGLOG_PROCESS_ERROR(piPackage);

	pRequest = (S2R_APPLY_MENTOR_DATA*)piPackage->GetData();
	KGLOG_PROCESS_ERROR(pRequest);

	pRequest->wProtocolID            = s2r_apply_mentor_data;
	pRequest->dwMentorID = dwMentorID;
	pRequest->dwApprenticeID = dwApprenticeID;

	bRetCode = Send(piPackage);
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	KG_COM_RELEASE(piPackage);
	return bResult;
}