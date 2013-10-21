#include "StdAfx.h"
#include "MailTools.h"
#include "KGameServerSimulator.h"
#include "Utility.h"

KMAIL_RESULT_CODE MailTools::SendMail(int nRequestID, const char* pReceiver, KMail* pMail)
{	
	size_t uDataLen = 0;
	for (int i = 0; i < KMAIL_MAX_ITEM_COUNT; i++)
	{
		uDataLen += pMail->ItemDesc[i].byDataLen;
	}
	uDataLen += pMail->wTextLen;

	scoped_array<byte> Request(new byte[sizeof(S2R_SEND_MAIL_REQUEST) + sizeof(KMail) + uDataLen]);
	S2R_SEND_MAIL_REQUEST* pSendRequest = (S2R_SEND_MAIL_REQUEST*)Request.get();
	pSendRequest->wProtocolID = s2r_send_mail_request;
	pSendRequest->byRequestID = (byte)nRequestID;
	strcpy(pSendRequest->szReceiverName, pReceiver);
	memcpy(pSendRequest->byData, pMail,  sizeof(KMail) + uDataLen);

	m_pGS->Send(pSendRequest, sizeof(S2R_SEND_MAIL_REQUEST) + sizeof(KMail) + uDataLen);
	BufferScopedPrt Buffer(m_pGS->ReceiveDataOnceTime(r2s_send_mail_respond, 2));
	if (!Buffer.get())
	{
		return mrcFailed;
	}
	R2S_SEND_MAIL_RESPOND* pRespond = (R2S_SEND_MAIL_RESPOND*)Buffer->GetData();
	if (pRespond->byRespondID != nRequestID)
	{
		return mrcFailed;
	}
	return (KMAIL_RESULT_CODE)pRespond->byResult;
}

std::vector<KMailListInfo>  MailTools::GetMailList(DWORD dwRoleID)
{
	std::vector<KMailListInfo> MailList;

	S2R_GET_MAILLIST_REQUEST GetMailListRequest;
	GetMailListRequest.wProtocolID = s2r_get_maillist_request;
	GetMailListRequest.dwPlayerID = dwRoleID;
	GetMailListRequest.dwStartID = 0;

	while (true)
	{
		m_pGS->Send(GetMailListRequest);

		BufferScopedPrt Buffer(m_pGS->ReceiveDataOnceTime(r2s_get_maillist_respond, 3));
		if (!Buffer.get())
		{
			return MailList;
		}
		R2S_GET_MAILLIST_RESPOND*  pRespond = (R2S_GET_MAILLIST_RESPOND*)Buffer->GetData();
		if (pRespond->nMailCount == 0)
		{
			return MailList;
		}
		for (int i = 0; i < pRespond->nMailCount; i++)
		{
			MailList.push_back(pRespond->MailList[i]);
		}
		GetMailListRequest.dwStartID += pRespond->nMailCount;
	}

	return MailList;
}


KMAIL_RESULT_CODE MailTools::DeleteMail(DWORD dwPlayerID, DWORD dwMailID)
{
	S2R_DELETE_MAIL DeleteMailRequest;
	DeleteMailRequest.wProtocolID = s2r_delete_mail;
	DeleteMailRequest.dwPlayerID = dwPlayerID;
	DeleteMailRequest.dwMailID = dwMailID;

	m_pGS->Send(DeleteMailRequest);

	BufferScopedPrt Buffer(m_pGS->ReceiveDataOnceTime(r2s_mail_general_respond, 3));
	if (!Buffer.get())
	{
		return mrcFailed;
	}
	R2S_MAIL_GENERAL_RESPOND* pRespond = (R2S_MAIL_GENERAL_RESPOND*)Buffer->GetData();
	if (pRespond->dwPlayerID != dwPlayerID ||
		pRespond->dwMailID != dwMailID)
	{
		return mrcFailed;
	}
	return (KMAIL_RESULT_CODE)pRespond->byRespond;
}

KMAIL_RESULT_CODE MailTools::QueryMailContent(DWORD dwPlayerID, DWORD dwMailID, BYTE byMailType, KMailContent* pMailContent)
{
	S2R_QUERY_MAIL_CONTENT QueryContent;
	QueryContent.wProtocolID = s2r_query_mail_content;
	QueryContent.dwPlayerID = dwPlayerID;
	QueryContent.dwMailID = dwMailID;
	QueryContent.byMailType = byMailType;

	m_pGS->Send(QueryContent);

	BufferScopedPrt Buffer(m_pGS->ReceiveDataOnceTime(r2s_sync_mail_content, 3));
	if (!Buffer.get())
	{
		return mrcFailed;
	}

	R2S_SYNC_MAIL_CONTENT* pContent  = (R2S_SYNC_MAIL_CONTENT*)Buffer->GetData();
	if (pContent->dwMailID != dwMailID ||
		pContent->dwPlayerID != dwPlayerID)
	{
		return mrcFailed;
	}

	if (pMailContent)
	{
		memcpy(pMailContent, pContent->byData, Buffer->GetSize() - sizeof(R2S_SYNC_MAIL_CONTENT));
	}

	return (KMAIL_RESULT_CODE)pContent->byResult;
}

KMAIL_RESULT_CODE MailTools::AcquiteMailMoney(DWORD dwPlayerID, DWORD dwMailID, int& nMoney)
{
	S2R_ACQUIRE_MAIL_MONEY_REQUEST Request;
	Request.wProtocolID = s2r_acquire_mail_money_request;
	Request.dwPlayerID = dwPlayerID;
	Request.dwMailID = dwMailID;

	m_pGS->Send(Request);

	BufferScopedPrt Buffer(m_pGS->ReceiveDataOnceTime(r2s_sync_mail_content, 3));
	if (!Buffer.get())
	{
		return mrcFailed;
	}

	R2S_GIVE_MAIL_MONEY_TO_PLAYER* pContent  = (R2S_GIVE_MAIL_MONEY_TO_PLAYER*)Buffer->GetData();
	if (pContent->dwMailID != dwMailID ||
		pContent->dwPlayerID != dwPlayerID)
	{
		return mrcFailed;
	}

	nMoney = pContent->nMoney;
	return mrcSucceed;
}

KMAIL_RESULT_CODE MailTools::AcquiteMailItem(DWORD dwPlayerID, DWORD dwMailID, int nIndex, R2S_GIVE_MAIL_ITEM_TO_PLAYER* pItemData)
{
	S2R_ACQUIRE_MAIL_ITEM_REQUEST Request;
	Request.wProtocolID = s2r_acquire_mail_item_request;
	Request.dwPlayerID = dwPlayerID;
	Request.byIndex = nIndex;
	Request.dwMailID = dwMailID;

	m_pGS->Send(Request);

	BufferScopedPrt Buffer(m_pGS->ReceiveDataOnceTime(r2s_give_mail_item_to_player, 3));
	if (!Buffer.get())
	{
		return mrcFailed;
	}

	R2S_GIVE_MAIL_ITEM_TO_PLAYER* pRespond  = (R2S_GIVE_MAIL_ITEM_TO_PLAYER*)Buffer->GetData();
	if (pRespond->dwMailID != dwMailID ||
		pRespond->dwPlayerID != dwPlayerID ||
		pRespond->byItemIndex != nIndex)
	{
		return mrcFailed;
	}

	if(pItemData)
	{
		memcpy(pItemData, pRespond, sizeof(R2S_GIVE_MAIL_ITEM_TO_PLAYER) + pRespond->byItemLen);
	}
	return (KMAIL_RESULT_CODE)pRespond->byResult;
}


KMAIL_RESULT_CODE MailTools::ReleaseMailMoney(DWORD dwPlayerID, DWORD dwMailID, KMailContent* pMailContent)
{
	S2R_GIVE_MAIL_MONEY_TO_PLAYER_RESPOND Request;
	Request.wProtocolID = s2r_give_mail_money_to_player;
	Request.dwPlayerID = dwPlayerID;
	Request.dwMailID = dwMailID;
	Request.byResult  = mrcSucceed;

	m_pGS->Send(Request);

	BufferScopedPrt Buffer(m_pGS->ReceiveDataOnceTime(r2s_sync_mail_content, 3));
	if (!Buffer.get())
	{
		return mrcFailed;
	}

	R2S_SYNC_MAIL_CONTENT* pContent  = (R2S_SYNC_MAIL_CONTENT*)Buffer->GetData();
	if (pContent->dwMailID != dwMailID ||
		pContent->dwPlayerID != dwPlayerID)
	{
		return mrcFailed;
	}

	if (pMailContent)
	{
		memcpy(pMailContent, pContent->byData, Buffer->GetSize() - sizeof(R2S_SYNC_MAIL_CONTENT));
	}
	return (KMAIL_RESULT_CODE)pContent->byResult;
}


KMAIL_RESULT_CODE MailTools::ReleaseMailMoneyFail(DWORD dwPlayerID, DWORD dwMailID)
{
	S2R_GIVE_MAIL_MONEY_TO_PLAYER_RESPOND Request;
	Request.wProtocolID = s2r_give_mail_money_to_player;
	Request.dwPlayerID = dwPlayerID;
	Request.dwMailID = dwMailID;
	Request.byResult  = mrcFailed;

	m_pGS->Send(Request);

	BufferScopedPrt Buffer(m_pGS->ReceiveDataOnceTime(r2s_mail_general_respond, 3));
	if (!Buffer.get())
	{
		return mrcFailed;
	}

	R2S_MAIL_GENERAL_RESPOND* pContent  = (R2S_MAIL_GENERAL_RESPOND*)Buffer->GetData();
	if (pContent->dwMailID != dwMailID ||
		pContent->dwPlayerID != dwPlayerID)
	{
		return mrcFailed;
	}

	return (KMAIL_RESULT_CODE)pContent->byRespond;
}


KMAIL_RESULT_CODE MailTools::ReleaseMailItem(DWORD dwPlayerID, DWORD dwMailID, DWORD dwIndex, KMailContent* pMailContent)
{
	S2R_GIVE_MAIL_ITEM_TO_PLAYER_RESPOND Request;
	Request.wProtocolID = s2r_give_mail_item_to_player;
	Request.dwPlayerID = dwPlayerID;
	Request.dwMailID = dwMailID;
	Request.byIndex = dwIndex;
	Request.byResult  = mrcSucceed;

	m_pGS->Send(Request);

	BufferScopedPrt Buffer(m_pGS->ReceiveDataOnceTime(r2s_sync_mail_content, 3));
	if (!Buffer.get())
	{
		return mrcFailed;
	}

	R2S_SYNC_MAIL_CONTENT* pContent  = (R2S_SYNC_MAIL_CONTENT*)Buffer->GetData();
	if (pContent->dwMailID != dwMailID ||
		pContent->dwPlayerID != dwPlayerID)
	{
		return mrcFailed;
	}

	if (pMailContent)
	{
		memcpy(pMailContent, pContent->byData, Buffer->GetSize() - sizeof(R2S_SYNC_MAIL_CONTENT));
	}
	return (KMAIL_RESULT_CODE)pContent->byResult;
}


KMAIL_RESULT_CODE MailTools::ReleaseMailItemFail(DWORD dwPlayerID, DWORD dwMailID, byte byIndex)
{
	S2R_GIVE_MAIL_ITEM_TO_PLAYER_RESPOND Request;
	Request.wProtocolID = s2r_give_mail_money_to_player;
	Request.dwPlayerID = dwPlayerID;
	Request.dwMailID = dwMailID;
	Request.byIndex = byIndex;
	Request.byResult  = mrcFailed;

	m_pGS->Send(Request);

	BufferScopedPrt Buffer(m_pGS->ReceiveDataOnceTime(r2s_mail_general_respond, 3));
	if (!Buffer.get())
	{
		return mrcFailed;
	}

	R2S_MAIL_GENERAL_RESPOND* pContent  = (R2S_MAIL_GENERAL_RESPOND*)Buffer->GetData();
	if (pContent->dwMailID != dwMailID ||
		pContent->dwPlayerID != dwPlayerID)
	{
		return mrcFailed;
	}

	return (KMAIL_RESULT_CODE)pContent->byRespond;
}

KMAIL_RESULT_CODE MailTools::UserReturnMail(DWORD dwPlayerID, DWORD dwMailID)
{
	S2R_RETURN_MAIL Request;
	Request.wProtocolID = s2r_return_mail;
	Request.dwPlayerID = dwPlayerID;
	Request.dwMailID = dwMailID;

	m_pGS->Send(Request);

	BufferScopedPrt Buffer(m_pGS->ReceiveDataOnceTime(r2s_mail_general_respond, 3));
	if (!Buffer.get())
	{
		return mrcFailed;
	}

	R2S_MAIL_GENERAL_RESPOND* pContent  = (R2S_MAIL_GENERAL_RESPOND*)Buffer->GetData();
	if (pContent->dwMailID != dwMailID ||
		pContent->dwPlayerID != dwPlayerID)
	{
		return mrcFailed;
	}

	return (KMAIL_RESULT_CODE)pContent->byRespond;
}

void MailTools::SetMailRead(DWORD dwPlayerID, DWORD dwMailID)
{
	S2R_SET_MAIL_READ Request;
	Request.wProtocolID = s2r_set_mail_read;
	Request.dwPlayerID = dwPlayerID;
	Request.dwMailID = dwMailID;

	m_pGS->Send(Request);
}


void MailTools::DoSendGloalMail(int nLifeTime, KMail* pMail)
{
	byte byBuffer[1024];
	S2R_SEND_GLOBAL_MAIL_REQUEST* pRequest = (S2R_SEND_GLOBAL_MAIL_REQUEST*)byBuffer;
	pRequest->wProtocolID = s2r_send_global_mail_request;
	pRequest->nLifeTime = nLifeTime;
	int nLength = 0;
	for (int i = 0; i < KMAIL_MAX_ITEM_COUNT; i++)
	{
		nLength += pMail->ItemDesc[i].byDataLen;
	}
	nLength += pMail->wTextLen;
	nLength += sizeof(KMail);

	memcpy(pRequest->byData, pMail, nLength);
	m_pGS->Send(pRequest, sizeof(S2R_SEND_GLOBAL_MAIL_REQUEST) + nLength);
}