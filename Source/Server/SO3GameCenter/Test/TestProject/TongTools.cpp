#include "StdAfx.h"
#include "TongTools.h"
#include "Relay_GS_Protocol.h"
#include "KGameServerSimulator.h"
#include "Utility.h"
#include "KGameCenterWrapper.h"
#include "testdef.h"
#include <sstream>

int TongTools::CreateTong(DWORD dwPlayerID, const char*  pszTongName, BYTE  byTemplateIndex)
{
	S2R_APPLY_CREATE_TONG_REQUEST CreateTong ;
	CreateTong.wProtocolID = s2r_apply_create_tong_request;
	CreateTong.dwPlayerID = dwPlayerID;
	strncpy(CreateTong.szTongName, pszTongName, sizeof(CreateTong.szTongName));
	CreateTong.byTemplateIndex = byTemplateIndex;
	m_pGS->Send((void*)&CreateTong, sizeof(CreateTong));
	SCOPEPROTOCOL(m_pGS, r2s_change_player_tong_notify);
	BufferScopedPrt Buffer(m_pGS->ReceiveDataOnce(r2s_change_player_tong_notify));
	if (!Buffer.get())
	{
		return ERROR_ID;
	}
	R2S_CHANGE_PLAYER_TONG_NOTIFY* pRespond = (R2S_CHANGE_PLAYER_TONG_NOTIFY*)Buffer->GetData();
	if ((pRespond->dwPlayerID == dwPlayerID) &&
		(pRespond->byReason == ctrCreate))
	{
		return pRespond->dwTongID;
	}
	return ERROR_ID;
}

int TongTools::CreateTong(TestTools::KRoleInfo& Role, const char* pszTongName, BYTE byTemplateIndex)
{
	return Role.dwTongID = CreateTong(Role.dwPlayerID, pszTongName, byTemplateIndex);
}

DWORD TongTools::TongID(DWORD dwRoleID)
{
	SocpeLock(GC->Mutex());
	KRole* p  = GC->Center()->m_RoleManager.GetRole(dwRoleID);
	if (p)
	{
		return p->m_dwTongID;
	}
	return ERROR_ID; 
}

bool TongTools::InviteJoinTong(DWORD dwRoleID, const char* pRoleName)
{
	S2R_INVITE_PLAYER_JOIN_TONG_REQUEST Request;
	Request.wProtocolID = s2r_invite_player_join_tong_request;
	Request.dwPlayerID = dwRoleID;
	strncpy(Request.szNewMemberName, pRoleName, sizeof(Request.szNewMemberName));
	m_pGS->Send((void*)&Request, sizeof(Request));
	SCOPEPROTOCOL(m_pGS, r2s_invite_player_join_tong_request);
	BufferScopedPrt Buffer(m_pGS->ReceiveDataOnce(r2s_invite_player_join_tong_request));
	if (!Buffer.get())
	{
		return false;
	}
	R2S_INVITE_PLAYER_JOIN_TONG_REQUEST* pRespond = (R2S_INVITE_PLAYER_JOIN_TONG_REQUEST*)Buffer->GetData();
	if (dwRoleID == pRespond->dwInviterID)
	{
		return true;
	}
	return false;
}

bool TongTools::ChangeTongCamp(DWORD dwPlayerID, BYTE byCamp)
{
	S2R_CHANGE_TONG_CAMP_REQUEST Request;
	Request.wProtocolID = s2r_change_tong_camp_request;
	Request.byCamp = byCamp;
	Request.dwPlayerID = dwPlayerID;
	m_pGS->Send(&Request, sizeof(Request));
	SCOPEPROTOCOL(m_pGS, r2s_tong_broadcast_message);
	BufferScopedPrt Buffer(m_pGS->ReceiveDataOnce(r2s_tong_broadcast_message));
	if (!Buffer.get())
	{
		return false;
	}
	R2S_TONG_BROADCAST_MESSAGE* pRespond = (R2S_TONG_BROADCAST_MESSAGE*)Buffer->GetData();
	if (tbmCampChange == pRespond->byCode)
	{
		return true;
	}
	return false;
}

bool TongTools::JoinTong(TestTools::KRoleInfo& TongMaster, TestTools::KRoleInfo& Joiner)
{
	if (!InviteJoinTong(TongMaster.dwPlayerID, Joiner.szRoleName))
	{
		return false;
	}
	if (!InviteRespond(true, TongMaster.dwPlayerID, Joiner.dwPlayerID, TongMaster.dwTongID))
	{
		return false;
	}

	BufferScopedPrt Buffer(m_pGS->ReceiveDataOnce(r2s_change_player_tong_notify));
	if (!Buffer.get())
	{
		return false;
	}
	R2S_CHANGE_PLAYER_TONG_NOTIFY* pRespond = (R2S_CHANGE_PLAYER_TONG_NOTIFY*)Buffer->GetData();
	if ((ctrJoin == pRespond->byReason) &&
		(pRespond->dwPlayerID == Joiner.dwPlayerID))
	{
		Joiner.dwTongID = TongMaster.dwTongID;
		return true;
	}
	return false;
}

bool TongTools::InviteRespond(bool bAccept, DWORD InviterID, DWORD dwPlayerID, DWORD dwTongID)
{
	S2R_INVITE_PLAYER_JOIN_TONG_RESPOND Respond;
	Respond.wProtocolID = s2r_invite_player_join_tong_respond;
	Respond.dwInviterID = InviterID;
	Respond.byAccept = bAccept;
	Respond.dwPlayerID = dwPlayerID;
	Respond.dwTongID = dwTongID;
	return m_pGS->Send(Respond);
}

bool TongTools::CheckInviteRespond(bool bAccept, DWORD InviterID, DWORD dwPlayerID, DWORD dwTongID)
{
	if (!InviteRespond(bAccept, InviterID, dwPlayerID, dwTongID))
	{
		return false;
	}

	BufferScopedPrt Buffer(m_pGS->ReceiveDataOnce(r2s_change_player_tong_notify));
	if (!Buffer.get())
	{
		return false;
	}
	R2S_CHANGE_PLAYER_TONG_NOTIFY* pRespond = (R2S_CHANGE_PLAYER_TONG_NOTIFY*)Buffer->GetData();
	if ((ctrJoin == pRespond->byReason) &&
		(pRespond->dwPlayerID == dwPlayerID))
	{
		return true;
	}
	return false;
}

bool TongTools::ChangeTongMaster(DWORD dwPlayerID, DWORD dwNewMasterID)
{
	S2R_CHANGE_TONG_MASTER_REQUEST Request;
	Request.wProtocolID = s2r_change_tong_master_request;
	Request.dwNewMasterID = dwNewMasterID;
	Request.dwPlayerID = dwPlayerID;
	m_pGS->Send(Request);
	BufferScopedPrt Buffer(m_pGS->ReceiveDataOnce(r2s_tong_broadcast_message));
	if (!Buffer.get())
	{
		return false;
	}
	R2S_TONG_BROADCAST_MESSAGE* pRespond = (R2S_TONG_BROADCAST_MESSAGE*)Buffer->GetData();
	if (tbmMasterChange == pRespond->byCode) 
	{
		return true;
	}
	return false;
}

bool TongTools::QuitTong(DWORD dwPlayerID)
{
	S2R_APPLY_QUITE_TONG_REQUEST Request;
	Request.wProtocolID = s2r_apply_quite_tong_request;
	Request.dwPlayerID = dwPlayerID;
	m_pGS->Send(Request);
	BufferScopedPrt Buffer(m_pGS->ReceiveDataOnce(r2s_change_player_tong_notify));
	if (!Buffer.get())
	{
		return false;
	}
	R2S_CHANGE_PLAYER_TONG_NOTIFY* pRespond = (R2S_CHANGE_PLAYER_TONG_NOTIFY*)Buffer->GetData();
	if ((ctrQuit == pRespond->byReason) &&
		(pRespond->dwPlayerID = dwPlayerID))
	{
		return true;
	}
	return false;
}

bool TongTools::ChangeGroup(DWORD dwPlayerID, DWORD dwTargetID, int nTargetGroup)
{
	S2R_CHANGE_TONG_MEMBER_GROUP_REQUEST Request;
	Request.wProtocolID = s2r_change_tong_member_group_request;
	Request.dwPlayerID = dwPlayerID;
	Request.dwTargetMemberID = dwTargetID;
	Request.byTargetGroup = nTargetGroup;
	m_pGS->Send(Request);
	BufferScopedPrt Buffer(m_pGS->ReceiveDataOnce(r2s_tong_broadcast_message));
	if (!Buffer.get())
	{
		return false;
	}
	R2S_TONG_BROADCAST_MESSAGE* pRespond = (R2S_TONG_BROADCAST_MESSAGE*)Buffer->GetData();
	if ((tbmMemberChangeGroup == pRespond->byCode) &&
		(pRespond->dwPlayerID == dwPlayerID))
	{
		return true;
	}
	return false;
}

bool TongTools::EnableGroup(DWORD dwTongID, int nGroup)
{
	std::stringstream ss;
	ss << "Result = false; \n"
		<< "ptong  = GetTong(" << dwTongID <<  ")\n"
		<< "if ptong then \n"
		<< "ptong.EnableGroup(" << nGroup << ")\n"
		<< "Result  =  ture\n"
		<< "end";
	return GC->CallScript(ss.str().c_str());
}

bool TongTools::ModifyBaseOperator(DWORD dwPlayerID, BYTE byGroupIndex, BYTE byModifyType, BYTE byOffset, bool bValue)
{
	char Request[sizeof(S2R_MODIFY_TONG_SCHEMA_REQUEST) + sizeof(KTONG_MODIFY_BASE_OPERATION_MASK)];
	S2R_MODIFY_TONG_SCHEMA_REQUEST* pRequest = (S2R_MODIFY_TONG_SCHEMA_REQUEST*)Request;
	pRequest->wProtocolID = s2r_modify_tong_schema_request;
	pRequest->dwPlayerID = dwPlayerID;
	pRequest->byGroupIndex = byGroupIndex;
	pRequest->byModifyType = byModifyType;
	KTONG_MODIFY_BASE_OPERATION_MASK* pMask = (KTONG_MODIFY_BASE_OPERATION_MASK*)pRequest->byData;
	pMask->bValue = bValue;
	pMask->byOffset = byOffset;
	m_pGS->Send(Request);
	BufferScopedPrt Buffer(m_pGS->ReceiveDataOnce(r2s_tong_broadcast_message));
	if (!Buffer.get())
	{
		return false;
	}
	R2S_TONG_BROADCAST_MESSAGE* pRespond = (R2S_TONG_BROADCAST_MESSAGE*)Buffer->GetData();
	if (tbmGroupRightChange == pRespond->byCode) 
	{
		return true;
	}
	return false;
}

bool TongTools::ModifyAdvanceOperator(DWORD dwPlayerID, BYTE byGroupIndex, BYTE byIndex, BYTE byOffset, bool bValue)
{
	char Request[sizeof(S2R_MODIFY_TONG_SCHEMA_REQUEST) + sizeof(KTONG_MODIFY_ADVANCE_OPERATION_MASK)];
	S2R_MODIFY_TONG_SCHEMA_REQUEST* pRequest = (S2R_MODIFY_TONG_SCHEMA_REQUEST*)Request;
	pRequest->wProtocolID = s2r_modify_tong_schema_request;
	pRequest->dwPlayerID = dwPlayerID;
	pRequest->byGroupIndex = byGroupIndex;
	pRequest->byModifyType = tmsAdvanceOperation;
	KTONG_MODIFY_ADVANCE_OPERATION_MASK* pMask = (KTONG_MODIFY_ADVANCE_OPERATION_MASK*)pRequest->byData;
	pMask->byIndex = byIndex;
	pMask->bValue = bValue;
	pMask->byOffset = byOffset;
	m_pGS->Send(Request);
	BufferScopedPrt Buffer(m_pGS->ReceiveDataOnce(r2s_tong_broadcast_message));
	if (!Buffer.get())
	{
		return false;
	}
	R2S_TONG_BROADCAST_MESSAGE* pRespond = (R2S_TONG_BROADCAST_MESSAGE*)Buffer->GetData();
	if (tbmGroupRightChange == pRespond->byCode) 
	{
		return true;
	}
	return false;
}


bool TongTools::CanBaseOperate(DWORD dwTongID, int nGroupIndex, int nOperationType)
{
	std::stringstream ss;
	ss << "Result = false; \n"
		<< "pTong = GetTong(" << dwTongID  << ")\n"
		<< "if pTong then \n"
		<< "Result = pTong.CanBaseOperate(" << nGroupIndex << "," << nOperationType << ")\n"
		<< "end";
	return GC->CallScript(ss.str().c_str());
}

bool TongTools::CanAdvanceOperate(DWORD dwTongID, int nGroupIndex, int nTargetGroup, int nOperationType)
{
	std::stringstream ss;
	ss << "Result = false; \n"
		<< "pTong = GetTong(" << dwTongID  << ")\n"
		<< "if pTong then \n"
		<< "Result = pTong.CanAdvanceOperate(" << nGroupIndex << "," << nTargetGroup << "," << nOperationType << ")\n"
		<< "end";
	return GC->CallScript(ss.str().c_str());
}


bool TongTools::GetScheMaData(DWORD dwTongID, KTongSchemaData* pData)
{
	SocpeLock(GC->Mutex());
	KTong* pTong = GC->Center()->m_TongManager.GetTong(dwTongID);
	if (pTong)
	{
		if (pData)
		{
			*pData = pTong->m_Schema;
		}
		return true;
	}
	return false;
}	

int TongTools::GetGroupID(DWORD dwTongID, DWORD dwPlayerID)
{
	SocpeLock(GC->Mutex());
	KTong* pTong = GC->Center()->m_TongManager.GetTong(dwTongID);
	if (pTong)
	{
		KTong::KROSTER::iterator Iter = pTong->m_Roster.find(dwPlayerID);
		if (Iter != pTong->m_Roster.end())
		{
			return Iter->second.nGroupID;
		}
	}
	return -1;
}

bool TongTools::GetTongGroup(DWORD dwTongID, DWORD dwGroupID, KTongGroup* pTongGroup)
{
	SocpeLock(GC->Mutex());
	KTong* pTong = GC->Center()->m_TongManager.GetTong(dwTongID);
	if (pTong)
	{
		*pTongGroup = pTong->m_Schema.Data[dwGroupID];
		return true;
	}
	return false;
}

bool TongTools::ModifyGroupName(DWORD dwPlayerID, BYTE byGroupIndex, const char* pNewName)
{
	char Request[sizeof(S2R_MODIFY_TONG_SCHEMA_REQUEST) + sizeof(KTONG_MODIFY_GROUP_NAME)];
	S2R_MODIFY_TONG_SCHEMA_REQUEST* pRequest = (S2R_MODIFY_TONG_SCHEMA_REQUEST*)Request;
	pRequest->wProtocolID = s2r_modify_tong_schema_request;
	pRequest->dwPlayerID = dwPlayerID;
	pRequest->byGroupIndex = byGroupIndex;
	pRequest->byModifyType = tmsGroupName;
	KTONG_MODIFY_GROUP_NAME* pMask = (KTONG_MODIFY_GROUP_NAME*)pRequest->byData;
	strncpy(pMask->szGroupName, pNewName, sizeof(pMask->szGroupName));
	m_pGS->Send(Request);
	BufferScopedPrt Buffer(m_pGS->ReceiveDataOnce(r2s_tong_broadcast_message));
	if (!Buffer.get())
	{
		return false;
	}
	R2S_TONG_BROADCAST_MESSAGE* pRespond = (R2S_TONG_BROADCAST_MESSAGE*)Buffer->GetData();
	if (tbmGroupNameChange == pRespond->byCode) 
	{
		return true;
	}
	return false;
}

bool TongTools::ModifyGroupWage(DWORD dwPlayerID, BYTE byGroupIndex, int nWage)
{
	char Request[sizeof(S2R_MODIFY_TONG_SCHEMA_REQUEST) + sizeof(nWage)];
	S2R_MODIFY_TONG_SCHEMA_REQUEST* pRequest = (S2R_MODIFY_TONG_SCHEMA_REQUEST*)Request;
	pRequest->wProtocolID = s2r_modify_tong_schema_request;
	pRequest->dwPlayerID = dwPlayerID;
	pRequest->byGroupIndex = byGroupIndex;
	pRequest->byModifyType = tmsGroupWage;
	memcpy(pRequest->byData, &nWage, sizeof(nWage));
	m_pGS->Send(Request);
	BufferScopedPrt Buffer(m_pGS->ReceiveDataOnce(r2s_tong_broadcast_message));
	if (!Buffer.get())
	{
		return false;
	}
	R2S_TONG_BROADCAST_MESSAGE* pRespond = (R2S_TONG_BROADCAST_MESSAGE*)Buffer->GetData();
	if (tbmGroupWageChange == pRespond->byCode) 
	{
		return true;
	}
	return false;
}

int TongTools::ModifyTongInfo(DWORD dwPlayerID, BYTE byInfoType, const char* pInfo)
{
	char Request[sizeof(S2R_MODIFY_TONG_INFO_REQUEST) + 100];
	S2R_MODIFY_TONG_INFO_REQUEST* pRequest = (S2R_MODIFY_TONG_INFO_REQUEST*)Request;
	pRequest->wProtocolID = s2r_modify_tong_info_request;
	pRequest->dwPlayerID = dwPlayerID;
	pRequest->byInfoType = byInfoType;
	pRequest->wDataLen = strlen(pInfo) + 1;
	strcpy((char*)pRequest->byData, pInfo);
	m_pGS->Send((void*)Request, sizeof(S2R_MODIFY_TONG_INFO_REQUEST) + pRequest->wDataLen);
	BufferScopedPrt Buffer(m_pGS->ReceiveDataOnce(r2s_remote_lua_call));
	if (!Buffer.get())
	{
		return tenInvalid;
	}
	R2S_REMOTE_LUA_CALL* pRspond = (R2S_REMOTE_LUA_CALL*)Buffer->GetData();
	int nRetcode = strcmp(pRspond->szFunction, TONG_EVENT_NOTIFY_FUNCTION);
	if (nRetcode != 0)
	{
		return tenInvalid;
	}
	int nResult = *(double*)(pRspond->byParam + sizeof(BYTE) + sizeof(double) + sizeof(BYTE));
	return nResult;
}

int TongTools::ModifyMemorabilia(DWORD dwPlayerID, KTONG_MODIFY_MEMORABILIA_INFO* pInfo, size_t uSize)
{
	char Request[sizeof(S2R_MODIFY_TONG_INFO_REQUEST) + 100];
	S2R_MODIFY_TONG_INFO_REQUEST* pRequest = (S2R_MODIFY_TONG_INFO_REQUEST*)Request;
	pRequest->wProtocolID = s2r_modify_tong_info_request;
	pRequest->dwPlayerID = dwPlayerID;
	pRequest->byInfoType = tmrMemorabilia;
	pRequest->wDataLen = uSize;
	memcpy(pRequest->byData, pInfo, uSize);
	m_pGS->Send((void*)Request, sizeof(S2R_MODIFY_TONG_INFO_REQUEST) + pRequest->wDataLen);
	BufferScopedPrt Buffer(m_pGS->ReceiveDataOnce(r2s_remote_lua_call));
	if (!Buffer.get())
	{
		return tenInvalid;
	}
	R2S_REMOTE_LUA_CALL* pRspond = (R2S_REMOTE_LUA_CALL*)Buffer->GetData();
	int nRetcode = strcmp(pRspond->szFunction, TONG_EVENT_NOTIFY_FUNCTION);
	if (nRetcode != 0)
	{
		return tenInvalid;
	}
	int nResult = *(double*)(pRspond->byParam + sizeof(BYTE) + sizeof(double) + sizeof(BYTE));
	return nResult;
}

bool TongTools::IsEnableGroup(DWORD dwTongID, int nGroup)
{
	std::stringstream ss;
	ss << "Result = false; \n"
		<< "ptong  = GetTong(" << dwTongID <<  ")\n"
		<< "if ptong then \n"
		<< "Result = ptong.IsGroupEnabled(" << nGroup << ")\n"
		<< "end";
	return GC->CallScript(ss.str().c_str());
}

int TongTools::SaveMoney(DWORD dwPlayerID, int nMoney)
{
	S2R_SAVE_MONEY_IN_TONG_REQUEST Request;
	Request.wProtocolID = s2r_save_money_in_tong_request;
	Request.dwPlayerID = dwPlayerID;
	Request.nMoney =  nMoney;
	m_pGS->Send(Request);
	BufferScopedPrt Buffer(m_pGS->ReceiveDataOnce(r2s_remote_lua_call));
	if (!Buffer.get())
	{
		return tenInvalid;
	}
	R2S_REMOTE_LUA_CALL* pRspond = (R2S_REMOTE_LUA_CALL*)Buffer->GetData();
	int nRetcode = strcmp(pRspond->szFunction, TONG_EVENT_NOTIFY_FUNCTION);
	if (nRetcode != 0)
	{
		return tenInvalid;
	}
	int nResult = *(double*)(pRspond->byParam + sizeof(BYTE) + sizeof(double) + sizeof(BYTE));
	return nResult;
}

int TongTools::ChangeTongMemberRemark(DWORD dwPlayerID, DWORD dwTargetMemberID, const char* pMark)
{
	S2R_CHANGE_TONG_MEMBER_REMARK_REQUEST Request ;
	Request.wProtocolID = s2r_change_tong_member_remark_request;
	Request.dwPlayerID = dwPlayerID;
	Request.dwTargetMemberID = dwTargetMemberID;
	strcpy(Request.szRemark, pMark);
	m_pGS->Send(Request);
	BufferScopedPrt Buffer(m_pGS->ReceiveDataOnce(r2s_remote_lua_call));
	if (!Buffer.get())
	{
		return tenInvalid;
	}
	R2S_REMOTE_LUA_CALL* pRspond = (R2S_REMOTE_LUA_CALL*)Buffer->GetData();
	int nRetcode = strcmp(pRspond->szFunction, TONG_EVENT_NOTIFY_FUNCTION);
	if (nRetcode != 0)
	{
		return tenInvalid;
	}
	int nResult = *(double*)(pRspond->byParam + sizeof(BYTE) + sizeof(double) + sizeof(BYTE));
	return nResult;
}


KTONG_STATE TongTools::TongState(DWORD dwID)
{
	SocpeLock(GC->Mutex());
	KTong* p  = GC->Center()->m_TongManager.GetTong(dwID);
	if (p)
	{
		return p->m_eState;
	}
	return tsInvalid; 
}
