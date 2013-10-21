#include "stdafx.h"
#include "KMailClient.h"
#include "KSO3World.h"
#include "KPlayerClient.h"

#ifdef _CLIENT

KMailInfo::KMailInfo(KMailListInfo* pMailListInfo)
{
    assert(pMailListInfo);

    dwMailID = pMailListInfo->dwMailID;
    
    strncpy(szSenderName, pMailListInfo->szSender, sizeof(szSenderName));
    szSenderName[sizeof(szSenderName) - 1] = '\0';

    strncpy(szTitle, pMailListInfo->szTitle, sizeof(szTitle));
    szTitle[sizeof(szTitle) - 1] = '\0';

    nSendTime   = pMailListInfo->nSendTime;
    nRecvTime   = pMailListInfo->nRecvTime;

    bReadFlag   = pMailListInfo->bReadFlag;
    bMoneyFlag  = pMailListInfo->bMoneyFlag;
    bItemFlag   = pMailListInfo->bItemFlag;

    byType      = pMailListInfo->byType;
    
    uTextLen        = 0;
    bGotContentFlag = false;

    nMoney       = 0;
    byText[0]    = '\0';
    for (int i = 0; i < KMAIL_MAX_ITEM_COUNT; i++)
    {
        dwItemID[i]   = ERROR_ID;
        nItemPrice[i] = 0;
    }
}

int KMailInfo::GetLeftSecond(int nMailSurvivalTime)
{
    int     nResult         = 0;
    time_t  nNowTime        = 0;

    assert(g_pSO3World);

    nNowTime = g_pSO3World->m_nBaseTime + (g_pSO3World->m_nGameLoop / GAME_FPS);
    nResult = (int)max(nRecvTime + nMailSurvivalTime - nNowTime, 0);

    return nResult;
}

KMailClient::KMailClient()
{
    m_nTotalCount       = 0;
    m_nUnreadCount      = 0;
    m_nSysCount         = 0;
    m_nSysUnreadCount   = 0;

    m_bGotMessageMail   = false;
    m_bMailListApplying = false;
}

KMailClient::~KMailClient()
{
}

void KMailClient::Clear()
{
	for (KMailTable::iterator it = m_MailTable.begin(); it != m_MailTable.end(); ++it)
	{
        KMailInfo*  pMailInfo = it->second;
        KItem*      pItem     = NULL;
        DWORD       dwItemID  = ERROR_ID;
        
        for (int i = 0; i < KMAIL_MAX_ITEM_COUNT; i++)
        {
            dwItemID = pMailInfo->dwItemID[i];

            if (dwItemID == ERROR_ID)
                continue;

            pItem = g_pSO3World->m_ItemSet.GetObj(dwItemID);
            if (pItem)
            {
                g_pSO3World->m_ItemManager.FreeItem(pItem);
                pItem = NULL;
            }

            pMailInfo->dwItemID[i] = ERROR_ID;
        }

        KG_DELETE(pMailInfo);
	}

    m_MailTable.clear();

    m_nTotalCount       = 0;
    m_nUnreadCount      = 0;
    m_nSysCount         = 0;
    m_nSysUnreadCount   = 0;

    m_bMailListApplying = false;
}

void KMailClient::Reset()
{
    Clear();
    m_bGotMessageMail = false;
}

BOOL KMailClient::AddMailList(int nCount, KMailListInfo* pMailList)
{
    BOOL    bResult = false;

    for (int i = 0; i < nCount; i++)
    {
		KMailInfo* pMailInfo = new KMailInfo(pMailList + i);
        KGLOG_PROCESS_ERROR(pMailInfo);

		m_MailTable.insert(make_pair(pMailInfo->dwMailID, pMailInfo));
    }

    bResult = true;
Exit0:
    return bResult;
}

void KMailClient::CountMail()
{
    m_nTotalCount       = 0;
    m_nUnreadCount      = 0;
    m_nSysCount		    = 0;
    m_nSysUnreadCount	= 0;

    for (KMailTable::iterator it = m_MailTable.begin(); it != m_MailTable.end(); ++it)
    {
        KMailInfo* pMailInfo = it->second;

        if (pMailInfo->byType == eMailType_GmMessage || pMailInfo->byType == eMailType_PlayerMessage)
            continue;

        m_nTotalCount++;

        if (pMailInfo->byType != eMailType_Player)
        {
            m_nSysCount++;

            if (!pMailInfo->bReadFlag)
            {
                m_nSysUnreadCount++;
            }
        }

        if (!pMailInfo->bReadFlag)
        {
            m_nUnreadCount++;
        }
    }
}

BOOL KMailClient::UpdateMailContent(DWORD dwMailID, KMailContent* pContent, int* pnMailType)
{
    BOOL                    bResult     = false;
    BOOL                    bRetCode    = false;
    KMailInfo*              pMailInfo   = NULL;
    BYTE*                   pbyOffset   = NULL;
    KMailTable::iterator    it;

    it = m_MailTable.find(dwMailID);
    KGLOG_PROCESS_ERROR(it != m_MailTable.end());

    pMailInfo = it->second;
    assert(pMailInfo);

    *pnMailType = pMailInfo->byType;

    pbyOffset = pContent->byData;

    // Text
    pMailInfo->uTextLen = 0;
    pMailInfo->byText[0] = '\0';
    if (pContent->wTextLen > 0)
    {
        KGLOG_PROCESS_ERROR(pContent->wTextLen <= MAX_MAIL_TEXT_LEN);

        pMailInfo->uTextLen = pContent->wTextLen;

        memcpy(pMailInfo->byText, pbyOffset, pContent->wTextLen);
        pMailInfo->byText[sizeof(pMailInfo->byText) - 1] = '\0';

        pbyOffset += pContent->wTextLen;
    }

    // Money
    pMailInfo->bMoneyFlag   = false;
    pMailInfo->nMoney       = pContent->nMoney;
    if (pMailInfo->nMoney > 0)
    {
        pMailInfo->bMoneyFlag = true;
    }

    // Item
    pMailInfo->bItemFlag = false;
    for (int i = 0; i < KMAIL_MAX_ITEM_COUNT; i++)
    {
        KItem* pItem        = NULL;
        BYTE*  pbyItemData  = NULL;
        size_t uItemDataLen = 0;

        uItemDataLen = pContent->ItemDesc[i].byDataLen;

        if (uItemDataLen == 0)
            continue;

        pbyItemData  = pbyOffset;
        pbyOffset   += uItemDataLen;

        if (pContent->ItemDesc[i].bAcquired)
        {
            if (pMailInfo->dwItemID[i] == ERROR_ID)
                continue;
            
            pItem = g_pSO3World->m_ItemSet.GetObj(pMailInfo->dwItemID[i]);
            if (pItem)
            {
                g_pSO3World->m_ItemManager.FreeItem(pItem);
                pItem = NULL;
            }

            pMailInfo->dwItemID[i] = ERROR_ID;
        }
        else
        {
            if(pMailInfo->dwItemID[i] == ERROR_ID)
            {
                pItem = g_pSO3World->m_ItemManager.GenerateItemFromBinaryData(ERROR_ID, pbyItemData, uItemDataLen);
                KGLOG_PROCESS_ERROR(pItem);

                pMailInfo->dwItemID[i]   = pItem->m_dwID;
                pMailInfo->nItemPrice[i] = pContent->ItemDesc[i].nPrice;                
            }

            pMailInfo->bItemFlag = true;
        }
    }

    pMailInfo->bGotContentFlag = true;

    if (pMailInfo->byType == eMailType_GmMessage || pMailInfo->byType == eMailType_PlayerMessage)
    {
        for (KMailTable::iterator it = m_MailTable.begin(); it != m_MailTable.end(); ++it)
        {
            pMailInfo = it->second;

            if (pMailInfo->byType != eMailType_GmMessage && pMailInfo->byType != eMailType_PlayerMessage)
                continue;

            if (pMailInfo->bGotContentFlag)
                continue;

            g_PlayerClient.DoQueryMailContent(ERROR_ID, pMailInfo->dwMailID, pMailInfo->byType);
            break;
        }
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KMailClient::ApplyMessageMailContent()
{
    BOOL bResult  = false;
    BOOL bRetCode = false;

    for (KMailTable::iterator it = m_MailTable.begin(); it != m_MailTable.end(); ++it)
    {
        KMailInfo* pMailInfo = it->second;

        if (pMailInfo->byType != eMailType_GmMessage && pMailInfo->byType != eMailType_PlayerMessage)
            continue;

        if (pMailInfo->bGotContentFlag)
            continue;

        bRetCode = g_PlayerClient.DoQueryMailContent(ERROR_ID, pMailInfo->dwMailID, pMailInfo->byType);
        KGLOG_PROCESS_ERROR(bRetCode);
        break;
    }

    bResult = true;
Exit0:
    return bResult;
}

#endif // _CLIENT

