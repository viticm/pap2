#pragma once
#include "testtools.h"
#include "KTongDef.h"

class KTongSchemaData;
class KGameServerSimulator;

struct TongTools
{
	int CreateTong(TestTools::KRoleInfo& Role, const char*  pszTongName, BYTE  byTemplateIndex);
	int CreateTong(DWORD dwPlayerID, const char*  pszTongName, BYTE  byTemplateIndex);
	bool InviteJoinTong(DWORD dwRoleID, const char* pRoleName);
	bool ChangeTongCamp(DWORD dwPlayerID, BYTE  byCamp);
	bool JoinTong(TestTools::KRoleInfo& TongMaster, TestTools::KRoleInfo& Joiner);
	DWORD TongID(DWORD dwRoleID);
	bool InviteRespond(bool bAccept, DWORD InviterID, DWORD dwPlayerID, DWORD dwTongID);
	bool CheckInviteRespond(bool bAccept, DWORD InviterID, DWORD dwPlayerID, DWORD dwTongID);
	bool ChangeTongMaster(DWORD dwPlayerID, DWORD dwNewMasterID);
	bool ChangeGroup(DWORD dwPlayerID, DWORD dwTargetID, int nTargetGroup);
	bool ModifyBaseOperator(DWORD dwPlayerID, BYTE byGroupIndex, BYTE byModifyType, BYTE byOffset, bool bValue);
	bool QuitTong(DWORD dwPlayerID);
	bool EnableGroup(DWORD dwTongID, int nGroup);
	bool CanBaseOperate(DWORD dwTongID, int nGroupIndex, int nOperationType);
	bool GetScheMaData(DWORD dwTongID, KTongSchemaData* pData);
	int GetGroupID(DWORD dwTongID, DWORD dwPlayerID);
	bool GetTongGroup(DWORD dwTongID, DWORD dwGroupID, KTongGroup* pTongGroup);
	bool ModifyAdvanceOperator(DWORD dwPlayerID, BYTE byGroupIndex, BYTE byIndex, BYTE byOffset, bool bValue);
	bool ModifyGroupName(DWORD dwPlayerID, BYTE byGroupIndex, const char* pNewName);
	bool CanAdvanceOperate(DWORD dwTongID, int nGroupIndex, int nTargetGroup, int nOperationType);
	bool ModifyGroupWage(DWORD dwPlayerID, BYTE byGroupIndex, int nWage);
	int ModifyTongInfo(DWORD  dwPlayerID, BYTE  byInfoType, const char* pInfo);
	int ModifyMemorabilia(DWORD dwPlayerID, KTONG_MODIFY_MEMORABILIA_INFO* pInfo, size_t uSize);
	bool IsEnableGroup(DWORD dwTongID, int nGroup);
	int SaveMoney(DWORD dwPlayerID, int nMoney);
	int ChangeTongMemberRemark(DWORD dwPlayerID, DWORD dwTargetMemberID, const char* pMark);
	KTONG_STATE TongState(DWORD dwID);
	KGameServerSimulator* m_pGS;
};
