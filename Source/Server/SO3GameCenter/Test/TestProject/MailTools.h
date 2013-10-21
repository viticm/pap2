#pragma once
#include <vector>
#include "KMailDef.h"
#include "Relay_GS_Protocol.h"

class KGameServerSimulator;
struct MailTools
{
	KMAIL_RESULT_CODE SendMail(int nRequestID, const char* pReceiver, KMail* pMail);
	std::vector<KMailListInfo> GetMailList(DWORD dwRoleID);
	KMAIL_RESULT_CODE QueryMailContent(DWORD dwPlayerID, DWORD dwMailID, BYTE byMailType, KMailContent* pMailContent = NULL);
	KMAIL_RESULT_CODE AcquiteMailMoney(DWORD dwPlayerID, DWORD dwMailID, int& nMoney);
	KMAIL_RESULT_CODE UserReturnMail(DWORD dwPlayerID, DWORD dwMailID);
	KMAIL_RESULT_CODE ReleaseMailMoneyFail(DWORD dwPlayerID, DWORD dwMailID);
	KMAIL_RESULT_CODE ReleaseMailItem(DWORD dwPlayerID, DWORD dwMailID, DWORD dwIndex, KMailContent* pMailContent = NULL);
	KMAIL_RESULT_CODE AcquiteMailItem(DWORD dwPlayerID, DWORD dwMailID, int nIndex, R2S_GIVE_MAIL_ITEM_TO_PLAYER* pItemData);
	KMAIL_RESULT_CODE DeleteMail(DWORD dwPlayerID, DWORD dwMailID);
	KMAIL_RESULT_CODE ReleaseMailMoney(DWORD dwPlayerID, DWORD dwMailID, KMailContent* pMailContent);
	KMAIL_RESULT_CODE ReleaseMailItemFail(DWORD dwPlayerID, DWORD dwMailID, byte byIndex);
	void DoSendGloalMail(int nLifeTime, KMail* pMail);
	void SetMailRead(DWORD dwPlayerID, DWORD dwMailID);
	KGameServerSimulator* m_pGS;
};
