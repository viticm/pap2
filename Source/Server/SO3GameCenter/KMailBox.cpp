#include "stdafx.h"
#include "KSO3GameCenter.h"
#include "KMailBox.h"
#include "KMailDef.h"

KMailBox::KMailBox()
{
    m_dwID                   = ERROR_ID;
    m_bSaved                 = true;
    m_nBaseTime              = 0;
    m_dwNextMailID           = 1;
    m_dwLastGlobalMailIndex  = 0;
}

KMailBox::~KMailBox()
{
}

void KMailBox::Clear()
{
    for (KMailTable::iterator it = m_MailTable.begin(); it != m_MailTable.end(); ++it)
    {
        KMEMORY_FREE(it->second);
    }

    m_MailTable.clear();

    m_bSaved                 = true;
    m_nBaseTime              = 0;
    m_dwNextMailID           = 1;
}

BOOL KMailBox::Load(BYTE* pbyData, size_t uDataLen)
{
    BOOL                bResult   = false;
    BYTE*               pbyOffset = pbyData;
    size_t              uLeftLen  = uDataLen;
    KMailBoxDBHead*     pHead     = NULL;
    size_t              uMailLen  = 0;
    WORD                wVersion  = 0;
    KMail*              pSrcMail  = NULL;
    KMail*              pDstMail  = NULL;
    std::pair<KMailTable::iterator, bool>  InsRet;
    
    assert(pbyData);
    assert(uDataLen > 0);

    KGLOG_PROCESS_ERROR(uLeftLen >= sizeof(KMailBoxDBHead));
    pHead = (KMailBoxDBHead*)pbyOffset;

    m_dwNextMailID          = pHead->dwNextMailID;
    m_dwLastGlobalMailIndex = pHead->dwLastGlobalMailIndex;
    wVersion                = pHead->wVersion;

    pbyOffset += sizeof(KMailBoxDBHead);
    uLeftLen  -= sizeof(KMailBoxDBHead);

    KGLOG_PROCESS_ERROR(wVersion == MAILSYS_VERSION);

    while (uLeftLen > 0)
    {
        pSrcMail = (KMail*)pbyOffset;

        KGLOG_PROCESS_ERROR(uLeftLen >= sizeof(KMail));
        uMailLen = sizeof(KMail) + pSrcMail->wTextLen;
        for (int i = 0; i < KMAIL_MAX_ITEM_COUNT; i++)
        {
            uMailLen += pSrcMail->ItemDesc[i].byDataLen;
        }
        KGLOG_PROCESS_ERROR(uLeftLen >= uMailLen);

        pDstMail = (KMail*)KMemory::Alloc(uMailLen);
        KGLOG_PROCESS_ERROR(pDstMail);

        memcpy(pDstMail, pSrcMail, uMailLen);

        InsRet = m_MailTable.insert(std::make_pair(pDstMail->dwMailID, pDstMail));
        KGLOG_PROCESS_ERROR(InsRet.second);

        pDstMail = NULL;

        pbyOffset += uMailLen;
        uLeftLen  -= uMailLen;
    }

    bResult = true;
Exit0:
    if (!bResult)
    {
        KMEMORY_FREE(pDstMail);
    }
    return bResult;
}

BOOL KMailBox::Save(BYTE* pbyBuffer, size_t uBufferSize, size_t* puUsedSize)
{
    BOOL               bResult   = false;
    BYTE*              pbyOffset = pbyBuffer;
    size_t             uLeftSize = uBufferSize;
    KMailBoxDBHead*    pHead     = NULL;
    KMail*             pMail     = NULL;
    size_t             uMailLen  = 0;

    assert(pbyBuffer);
    assert(puUsedSize);

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KMailBoxDBHead));
    pHead = (KMailBoxDBHead*)pbyOffset;

    pHead->wVersion              = MAILSYS_VERSION;
    pHead->dwNextMailID          = m_dwNextMailID;
    pHead->dwLastGlobalMailIndex = m_dwLastGlobalMailIndex;

    pbyOffset += sizeof(KMailBoxDBHead);
    uLeftSize -= sizeof(KMailBoxDBHead);

    for (KMailTable::iterator it = m_MailTable.begin(); it != m_MailTable.end(); ++it)
    {
        size_t uMailLen = 0;

        pMail = it->second;
        assert(pMail);

        KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KMail));
        uMailLen = sizeof(KMail) + pMail->wTextLen;
        for (int i = 0; i < KMAIL_MAX_ITEM_COUNT; i++)
        {
            uMailLen += pMail->ItemDesc[i].byDataLen;
        }
        
        KGLOG_PROCESS_ERROR(uLeftSize >= uMailLen);
        memcpy(pbyOffset, pMail, uMailLen);

        pbyOffset += uMailLen;
        uLeftSize -= uMailLen;
    }

    *puUsedSize = pbyOffset - pbyBuffer;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KMailBox::SendMail(KMail* pMail, size_t uMailLen)
{
    BOOL                                   bResult      = false;
    BOOL                                   bRetCode     = false;
    KMail*                                 pNewMail     = NULL;
    int                                    nMaxCount    = 0;
    KMailCount                             MailCount;
    std::pair<KMailTable::iterator, bool>  InsRet;

    assert(pMail);
    assert(pMail->byType < eMailType_Total);

    GetMailCount(&MailCount);

    // 留言邮件，数量达到上限时覆盖最早的邮件    
    if (pMail->byType == eMailType_GmMessage)
    {        
        nMaxCount = g_pSO3GameCenter->m_Settings.m_nMaxGmMsgMailCount;
        EliminateMail(pMail->byType, MailCount.nGmMsgMail + 1 - nMaxCount);
    }
    else if (pMail->byType == eMailType_PlayerMessage)
    {
        nMaxCount = g_pSO3GameCenter->m_Settings.m_nMaxPlayerMsgMailCount;
        EliminateMail(pMail->byType, MailCount.nPlayerMsgMail + 1 - nMaxCount);
    }

    if (m_MailTable.size() >= KMAILBOX_MAX_MAIL_COUNT)
    {
        KRole* pRole = g_pSO3GameCenter->m_RoleManager.GetRole(m_dwID);
        KGLOG_PROCESS_ERROR(pRole);

        KGLogPrintf(
            KGLOG_INFO, "[Mail] Failed to send mail: %s-->%s, title:%s, money = %d",
            pMail->szSenderName, pRole->m_szName, pMail->szTitle, pMail->nMoney
        );

        goto Exit0;
    }

    pNewMail = (KMail*)KMemory::Alloc(uMailLen);
    KGLOG_PROCESS_ERROR(pNewMail);

    memcpy(pNewMail, pMail, uMailLen);

    pNewMail->bRead     = false;
    pNewMail->nLockTime = 0;    
    pNewMail->dwMailID  = m_dwNextMailID++;

    if (m_nBaseTime == 0)
    {
        m_nBaseTime = (pNewMail->nRecvTime / KMAIL_WITHDRAW_CYCLE + 1) * KMAIL_WITHDRAW_CYCLE;
    }

    InsRet = m_MailTable.insert(std::make_pair(pNewMail->dwMailID, pNewMail));
    KGLOG_PROCESS_ERROR(InsRet.second);

    m_bSaved = false;

    bResult = true;
Exit0:
    if (!bResult)
    {
        KMEMORY_FREE(pNewMail);
    }
    return bResult;
}

BOOL KMailBox::SendGlobalMail(KMail* pMail)
{
    BOOL                                    bResult  = false;
    KMail*                                  pNewMail = NULL;
    size_t                                  uMailLen = 0;
    std::pair<KMailTable::iterator, bool>   InsRet;

    assert(pMail);

    KG_PROCESS_ERROR(m_MailTable.size() < KMAILBOX_MAX_MAIL_COUNT);

    uMailLen = sizeof(KMail) + pMail->wTextLen;
    for (int i = 0; i < KMAIL_MAX_ITEM_COUNT; i++)
    {
        uMailLen += pMail->ItemDesc[i].byDataLen;
    }

    pNewMail = (KMail*)KMemory::Alloc(uMailLen);
    KGLOG_PROCESS_ERROR(pNewMail);

    memcpy(pNewMail, pMail, uMailLen);

    pNewMail->bRead = false;
    pNewMail->dwMailID = m_dwNextMailID++;

    InsRet = m_MailTable.insert(std::make_pair(pNewMail->dwMailID, pNewMail));
    KGLOG_PROCESS_ERROR(InsRet.second);

    if (m_nBaseTime == 0)
    {
        m_nBaseTime = (pMail->nRecvTime / KMAIL_WITHDRAW_CYCLE + 1) * KMAIL_WITHDRAW_CYCLE;
    }

    m_bSaved = false;

    bResult = true;
Exit0:
    if (!bResult)
    {
        KMEMORY_FREE(pNewMail);
    }
    return bResult;
}

int KMailBox::GetMailList(KMailList* pMailList, time_t nTimeNow, DWORD dwStartID)
{
    BOOL                 bRetCode     = false;
    KMail*               pMail        = NULL;
    int                  nCount       = 0;
    KMailListInfo        MailListInfo;
    KMailTable::iterator BeginIter;

    assert(pMailList);
    
    BeginIter = m_MailTable.upper_bound(dwStartID);

    for (KMailTable::iterator it = BeginIter; it != m_MailTable.end(); ++it)
    {
        if (nCount >= SYNC_MAIL_LIST_MAX_COUNT)
            break;

        pMail = it->second;
        assert(pMail);

        if (nTimeNow < pMail->nRecvTime)
            continue;

        MailListInfo.byType     = pMail->byType;
        MailListInfo.dwMailID   = pMail->dwMailID;

        // 发件玩家可能改名了，干脆在这里重新赋发件人名字
        if (pMail->byType == eMailType_Player)
        {
            KRole* pSender = g_pSO3GameCenter->m_RoleManager.GetRole(pMail->dwSenderID);

            if (pSender && strncmp(pMail->szSenderName, pSender->m_szName, sizeof(pMail->szSenderName)) == 0)
            {
                strncpy(pMail->szSenderName, pSender->m_szName, sizeof(pMail->szSenderName));
                pMail->szSenderName[sizeof(pMail->szSenderName) - 1] = '\0';

                m_bSaved = false;
            }
        }

        strncpy(MailListInfo.szSender, pMail->szSenderName, sizeof(MailListInfo.szSender));
        MailListInfo.szSender[sizeof(MailListInfo.szSender) - 1] = '\0';

        strncpy(MailListInfo.szTitle, pMail->szTitle, sizeof(MailListInfo.szTitle));
        MailListInfo.szTitle[sizeof(MailListInfo.szTitle) - 1] = '\0';

        MailListInfo.nSendTime  = pMail->nSendTime;
        MailListInfo.nRecvTime  = pMail->nRecvTime;
        MailListInfo.bReadFlag  = pMail->bRead;
        MailListInfo.bMoneyFlag = (pMail->nMoney > 0);

        MailListInfo.bItemFlag  = false;
        for (int i = 0; i < KMAIL_MAX_ITEM_COUNT; i++)
        {
            KMailItemDesc* pItemDesc = &pMail->ItemDesc[i];

            if (pItemDesc->byDataLen > 0 && !pItemDesc->bAcquired)
            {
                MailListInfo.bItemFlag = true;
                break;
            }
        }

        pMailList->push_back(MailListInfo);
        nCount++;
    }

    return nCount;
}


BOOL KMailBox::DeleteMail(DWORD dwMailID)
{
    BOOL                   bResult      = false;
    KMailTable::iterator   it;

    it = m_MailTable.find(dwMailID);
    KGLOG_PROCESS_ERROR(it != m_MailTable.end());

    KMEMORY_FREE(it->second);

    m_MailTable.erase(it);

    ResetBaseTime();

    m_bSaved = false;

    bResult = true;
Exit0:
    return bResult;
}

KMail* KMailBox::GetMail(DWORD dwMailID)
{
    KMailTable::iterator it = m_MailTable.find(dwMailID);

    if (it != m_MailTable.end())
    {
        return it->second;
    }

    return NULL;
}

int KMailBox::GetWithdrawMail(
    time_t nTimeNow, time_t nSurvivalTime, 
    std::vector<KMail*, KMemory::KAllocator<KMail*> >* pMailList
 )
{
    int     nResult = 0;
    KMail*  pMail   = NULL;

    assert(pMailList);
    assert(pMailList->size() == 0);

    for (KMailTable::iterator it = m_MailTable.begin(); it != m_MailTable.end(); NULL)
    {
        BOOL bReturn = false;

        pMail = it->second;
        assert(pMail);

        if (nTimeNow < pMail->nRecvTime + nSurvivalTime)
        {
            ++it;
            continue;
        }

        if (pMail->byType == eMailType_Player)
        {
            if (!pMail->bRead)
            {
                bReturn = true; //未读邮件需要退回
            }
            else if (pMail->nMoney > 0)
            {
                bReturn = true; // 有未取金钱的邮件需要退回
            }
            else
            {
                for (int i = 0; i < KMAIL_MAX_ITEM_COUNT; i++)
                {
                    if (pMail->ItemDesc[i].bAcquired == false)
                    {
                        bReturn = true; // 有未取道具的邮件需要退回 
                        break;
                    }
                }
            }
        }

        if (bReturn)
        {
            pMailList->push_back(pMail);
            nResult++;
        }
        else
        {
            KMEMORY_FREE(pMail); // 无需return的邮件，直接删除
        }

        m_bSaved = false;

        m_MailTable.erase(it++);
    }

    ResetBaseTime();

//Exit0:
    return nResult;
}

BOOL KMailBox::DeleteAll()
{
    KG_PROCESS_SUCCESS(m_MailTable.size() == 0);

    for (KMailTable::iterator it = m_MailTable.begin(); it != m_MailTable.end(); ++it)
    {
        KMEMORY_FREE(it->second);
    }
    m_MailTable.clear();

    m_nBaseTime = 0;

    m_bSaved = false;
Exit1:
    return true;
}

void KMailBox::GetMailCount(KMailCount* pMailCount)
{
    KMail*  pMail   = NULL;

    assert(pMailCount);

    memset(pMailCount, 0, sizeof(KMailCount));

    pMailCount->nTotalCount = (int)m_MailTable.size();

    for (KMailTable::iterator it = m_MailTable.begin(); it != m_MailTable.end(); ++it)
    {
        pMail = it->second;

        switch (pMail->byType)
        {
        case eMailType_Player:
            pMailCount->nPlayerMail++;
            break;
        case eMailType_Auction:
            pMailCount->nAuctionMail++;
            break;
        case eMailType_System:
            pMailCount->nSystemMail++;
            break;
        case eMailType_GmMessage:
            pMailCount->nGmMsgMail++;
            break;
        case eMailType_PlayerMessage:
            pMailCount->nPlayerMsgMail++;
            break;
        default:
            assert(false);
            break;
        }
    }
}

void KMailBox::ResetBaseTime()
{
    KMail*   pMail       = NULL;
    time_t   nBaseTime   = 0;

    for (KMailTable::iterator it = m_MailTable.begin(); it != m_MailTable.end(); ++it)
    {
        pMail = it->second;
        assert(pMail);

        if (nBaseTime == 0 || pMail->nRecvTime < nBaseTime)
        {
            nBaseTime = pMail->nRecvTime;
        }
    }

    if (nBaseTime != 0)
    {
        m_nBaseTime = (nBaseTime / KMAIL_WITHDRAW_CYCLE + 1) * KMAIL_WITHDRAW_CYCLE;
    }
    else
    {
        m_nBaseTime = 0;
    }
}

// 按照ID从小到大，释放掉一些邮件
void KMailBox::EliminateMail(int nType, int nCount)
{
    KMail*  pMail   = NULL;

    for (KMailTable::iterator it = m_MailTable.begin(); it != m_MailTable.end(); NULL)
    {
        if (nCount <= 0)
            break;

        pMail = it->second;
        if (pMail->byType != nType)
        {
            ++it;
            continue;
        }

        KMEMORY_FREE(pMail);
        m_MailTable.erase(it++);
        nCount--;

        m_bSaved = false;
    }
}
