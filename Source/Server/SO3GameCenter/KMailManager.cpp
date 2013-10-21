#include "stdafx.h"
#include "KSO3GameCenter.h"
#include "KMailManager.h"
#include "KMailDef.h"
#include "KRole.h"
#include "KItemDef.h"
#include <vector>
//////////////////////////////////////////////////////////////////////////

#define REGISTER_OPERATION_FUNC(Type, Func) \
    m_MailOperationFunc[Type] = &KMailManager::Func

KMailManager::KMailManager()
{
    m_bCacheReady           		= true;
    m_nNextSaveTime         		= 0;
    m_nNextWithdrawTime     		= 0;
	m_nNextCheckGlobalMailLifeTime	= 0;
    m_pbyBuffer    		            = NULL;
    m_nCacheHitRatio        		= 0;
    m_nCacheMissRatio       		= 0;
    m_nCacheHitCount        		= 0;
    m_nCacheMissCount       		= 0;
    m_nMaxCache                     = MAX_CACHE_MAIL_COUNT;

    memset(m_MailOperationFunc, 0, sizeof(m_MailOperationFunc));
    REGISTER_OPERATION_FUNC(emoSendMail, OnSendMailOperation);
    REGISTER_OPERATION_FUNC(emoGetMailList, OnGetMailListOperation);
    REGISTER_OPERATION_FUNC(emoDeleteMail, OnDeleteMailOperation);
    REGISTER_OPERATION_FUNC(emoQueryMailContent, OnQueryMailContentOperation);
    REGISTER_OPERATION_FUNC(emoAcquireMailMoney, OnAcquireMailMoneyOperation);
    REGISTER_OPERATION_FUNC(emoAcquireMailItem, OnAcquireMailItemOperation);
    REGISTER_OPERATION_FUNC(emoReleaseMailMoney, OnReleaseMailMoneyOperation);
    REGISTER_OPERATION_FUNC(emoReleaseMailItem, OnReleaseMailItemOperation);
    REGISTER_OPERATION_FUNC(emoUnLockMail, OnUnLockMailOperation);
    REGISTER_OPERATION_FUNC(emoSetMailRead, OnSetMailReadOperation);
    REGISTER_OPERATION_FUNC(emoUserReturnMail, OnUserReturnMailOperation);
    REGISTER_OPERATION_FUNC(emoWithdrawMail, OnWithdrawMailOperation);
    REGISTER_OPERATION_FUNC(emoDeleteAll, OnDeleteAllOperation);
    REGISTER_OPERATION_FUNC(emoLoadMailCount, OnLoadMailCountOperation);
}

KMailManager::~KMailManager()
{
}

BOOL KMailManager::Init()
{
    BOOL                bResult  = false;
    BOOL                bRetCode = false;

    m_pbyBuffer = new BYTE[MAX_MAILBOX_DB_DATA_SIZE];
    KGLOG_PROCESS_ERROR(m_pbyBuffer);

    bResult = true;
Exit0:
    if (!bResult)
    {
        KG_DELETE_ARRAY(m_pbyBuffer);
    }
    return bResult;
}

void KMailManager::UnInit()
{
    BOOL        bRetCode    = false;
    KNode*      pNode       = NULL;

    while (!m_OperationQueue.empty())
    {
        void* pvOperation = m_OperationQueue.front();

        KMEMORY_FREE(pvOperation);

        m_OperationQueue.pop_front();
    }
    
    for (KGLOBAL_TABLE::iterator it = m_GlobalMailTable.begin(); it != m_GlobalMailTable.end(); ++it)
    {
        KMEMORY_FREE((it->second).pMail);
    }

    m_GlobalMailTable.clear();

    m_MailCountTable.clear();

    pNode = m_MailQueue.GetHead();
    while (pNode)
    {
        KNode* pNext = pNode->GetNext();

        pNode->Remove();

        KCacheNode* pCacheNode = (KCacheNode*)pNode;
        KMailBox*   pMailBox   = &pCacheNode->MailBox;

        assert(pMailBox->m_bSaved);

        pMailBox->Clear();

        m_CacheTable.erase(pCacheNode->CacheIter);
        KMemory::Delete(pCacheNode);

        pNode = pNext;
    }

    m_WithdrawMailQueue.clear();
    m_CacheTable.clear();

    KG_DELETE_ARRAY(m_pbyBuffer);

    return;
}

void KMailManager::Activate()
{
    BOOL                    bRetCode = false;
    IKG_Buffer*             piBuffer = NULL;
    BYTE*                   pbyData  = NULL;
    size_t                  uDataLen = 0;
    KMAIL_OPERATION_HEAD*   pHead    = NULL;
    KMAIL_OPERATION_FUNC    pFunc    = NULL;
    KMailBox*               pMailBox = NULL;

    while (m_bCacheReady && !m_OperationQueue.empty())
    {
        pHead = (KMAIL_OPERATION_HEAD*)m_OperationQueue.front();

        assert(pHead);

        pbyData  = (BYTE*)pHead;
        uDataLen = pHead->uSize;

        pMailBox = GetMailBox(pHead->dwMailBoxID);
        if (pMailBox == NULL)
        {
            m_nCacheMissCount++;

            bRetCode = g_pSO3GameCenter->m_MiscDB.DoLoadMailBox(pHead->dwMailBoxID);
            KGLOG_PROCESS_ERROR(bRetCode);

            m_bCacheReady = false;
            break;
        }

        m_nCacheHitCount++;

        pFunc = m_MailOperationFunc[pHead->byType];
        KGLOG_PROCESS_ERROR(pFunc);

        m_OperationQueue.pop_front();

        (this->*pFunc)(pMailBox, pbyData, uDataLen);

        KMEMORY_FREE(pHead);
    }

    if (g_pSO3GameCenter->m_nTimeNow >= m_nNextSaveTime)
    {
        SaveAll();
        m_nNextSaveTime = g_pSO3GameCenter->m_nTimeNow + 60;
    }

    if (g_pSO3GameCenter->m_nTimeNow >= m_nNextWithdrawTime && m_WithdrawMailQueue.empty())
    {
        bRetCode = g_pSO3GameCenter->m_MiscDB.DoCheckWithdrawMail(
            g_pSO3GameCenter->m_nTimeNow, g_pSO3GameCenter->m_Settings.m_nMailSurvivalTime
        );
        KGLOG_PROCESS_ERROR(bRetCode);

        m_nNextWithdrawTime = g_pSO3GameCenter->m_nTimeNow + 60 * 20;
    }

    if (g_pSO3GameCenter->m_nWorkLoop % GAME_FPS == 0 && !m_WithdrawMailQueue.empty())
    {
        DWORD dwID = m_WithdrawMailQueue.front();
        PostRequestWithdrawMail(dwID);
        m_WithdrawMailQueue.pop_front();
    }

    if (g_pSO3GameCenter->m_nWorkLoop % (GAME_FPS * 60) == 0)
    {
        m_nCacheHitRatio    = (m_nCacheHitCount + m_nCacheHitRatio) / 2;
        m_nCacheHitCount    = 0;
        m_nCacheMissRatio   = (m_nCacheMissCount + m_nCacheMissRatio) / 2;
        m_nCacheMissCount   = 0;

        CheckCache();
    }

    // 每60秒处理一次GlobalMail的生存
    if (g_pSO3GameCenter->m_nTimeNow >= m_nNextCheckGlobalMailLifeTime)
    {
        time_t nTimeNow = g_pSO3GameCenter->m_nTimeNow;

        m_nNextCheckGlobalMailLifeTime = nTimeNow + 60;

        for (KGLOBAL_TABLE::iterator it = m_GlobalMailTable.begin(); it != m_GlobalMailTable.end(); NULL)
        {
            if ((it->second).nEndTime <= nTimeNow)
            {
                g_pSO3GameCenter->m_MiscDB.DoDeleteGlobalMail(it->first);
                KMEMORY_FREE((it->second).pMail);
                m_GlobalMailTable.erase(it++);
                continue;
            }
            ++it;
        }
    }

Exit0:
    return;
}

void KMailManager::SaveAll()
{
    BOOL bRetCode = false;

    for (KCacheTable::iterator it = m_CacheTable.begin(); it != m_CacheTable.end(); ++it)
    {
        KMailBox*   pMailBox    = NULL;
        size_t      uUsedLen    = 0;
        KMailCount  MailCount;

        assert(it->second);

        pMailBox = &it->second->MailBox;
        assert(pMailBox);

        if (pMailBox->m_bSaved)
            continue;

        bRetCode = pMailBox->Save(m_pbyBuffer, MAX_MAILBOX_DB_DATA_SIZE, &uUsedLen);
        KGLOG_PROCESS_ERROR(bRetCode);

        pMailBox->GetMailCount(&MailCount);

        UpdateMailCount(it->first, MailCount);

        bRetCode = g_pSO3GameCenter->m_MiscDB.DoSaveMailBox(
            pMailBox->m_dwID, pMailBox->m_nBaseTime, MailCount.nTotalCount,
            m_pbyBuffer, uUsedLen
        );
        KGLOG_PROCESS_ERROR(bRetCode);

        pMailBox->m_bSaved = true;
    }

Exit0:
    return;
}

BOOL KMailManager::LoadGlobalMail(int nVersion, DWORD dwMailID, time_t nEndTime, BYTE* pbyData, size_t uDataLen)
{
    BOOL                                        bResult     = false;
    KMail*                                      pMail       = NULL;
    size_t                                      uMailLen    = 0;
    KMail*                                      pMailData   = (KMail*)pbyData;
    KGlobalMail                                 GlobalMail;
    std::pair<KGLOBAL_TABLE::iterator, bool>    InsRet;

    assert(pbyData);

    KGLOG_PROCESS_ERROR(nVersion == MAILSYS_VERSION);

    KGLOG_PROCESS_ERROR(uDataLen >= sizeof(KMail));
    uMailLen = sizeof(KMail) + pMailData->wTextLen;
    for (int i = 0; i < KMAIL_MAX_ITEM_COUNT; i++)
    {
        uMailLen += pMailData->ItemDesc[i].byDataLen;
    }
    KGLOG_PROCESS_ERROR(uDataLen == uMailLen);

    pMail = (KMail*)KMemory::Alloc(uMailLen);
    KGLOG_PROCESS_ERROR(pMail);

    memcpy(pMail, pMailData, uMailLen);
    
    GlobalMail.pMail    = pMail;
    GlobalMail.nEndTime = nEndTime;

    InsRet = m_GlobalMailTable.insert(std::make_pair(dwMailID, GlobalMail));
    KGLOG_PROCESS_ERROR(InsRet.second);

    bResult = true;
Exit0:
    if (!bResult)
    {
        KMEMORY_FREE(pMail);
    }
    return bResult;
}

BOOL KMailManager::PostRequestSendMail(int nRequestID, DWORD dwMailBoxID, KMail* pMail, size_t uMailLen)
{
    BOOL                        bResult      = false;
    KMAIL_SEND_MAIL_OPERATION*  pRequest     = NULL;
    size_t                      uRequestLen  = 0;
    KRole*                      pSenderRole  = NULL;

    assert(pMail);

    pMail->dwSenderID = ERROR_ID;
    if (pMail->byType == eMailType_Player)
    {
        pSenderRole = g_pSO3GameCenter->m_RoleManager.GetRole(pMail->szSenderName);
        KGLOG_PROCESS_ERROR(pSenderRole);

        pMail->dwSenderID = pSenderRole->m_dwPlayerID;
    }

    uRequestLen = sizeof(KMAIL_SEND_MAIL_OPERATION) + uMailLen;
    pRequest = (KMAIL_SEND_MAIL_OPERATION*)KMemory::Alloc(uRequestLen);
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byType       = emoSendMail;
    pRequest->dwMailBoxID  = dwMailBoxID;
    pRequest->uSize        = uRequestLen;
    pRequest->byRequestID  = (BYTE)nRequestID;

    memcpy(pRequest->byData, pMail, uMailLen);

    m_OperationQueue.push_back(pRequest);

    bResult = true;
Exit0:
    if (!bResult)
    {
        KMEMORY_FREE(pRequest);
    }
    return bResult;
}

BOOL KMailManager::PostRequestSendMail(
    int nRequestID, DWORD dwMailBoxID, int nDelayTime, BYTE byType,
    const char* pcszSenderName,
    const char* pcszTitle, 
    int nMoney,
    BYTE* pbyTextData, size_t uTextLen,
    BYTE* pbyItemData, size_t uItemDataLen, int nItemPrice
)
{
    BOOL                        bResult      = false;
    KMAIL_SEND_MAIL_OPERATION*  pRequest     = NULL;
    size_t                      uRequestLen  = 0;
    KMail*                      pMail        = NULL;
    size_t                      uMailLen     = 0;
    KRole*                      pSenderRole  = NULL;

    assert(pcszTitle);
    KGLOG_PROCESS_ERROR(uTextLen <= MAX_MAIL_TEXT_LEN);

    uMailLen    = sizeof(KMail) + uTextLen + uItemDataLen;

    uRequestLen = sizeof(KMAIL_SEND_MAIL_OPERATION) + uMailLen;
    pRequest    = (KMAIL_SEND_MAIL_OPERATION*)KMemory::Alloc(uRequestLen);
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byType       = emoSendMail;
    pRequest->dwMailBoxID  = dwMailBoxID;
    pRequest->uSize        = uRequestLen;
    pRequest->byRequestID  = (BYTE)nRequestID;

    pMail = (KMail*)pRequest->byData;

    memset(pMail, 0, uMailLen);

    pMail->bRead     = false;
    pMail->byType    = byType;

    pMail->dwSenderID = ERROR_ID;
    if (pMail->byType == eMailType_Player)
    {
        pSenderRole = g_pSO3GameCenter->m_RoleManager.GetRole(pMail->szSenderName);
        KGLOG_PROCESS_ERROR(pSenderRole);

        pMail->dwSenderID = pSenderRole->m_dwPlayerID;
    }

    strncpy(pMail->szTitle, pcszTitle, sizeof(pMail->szTitle));
    pMail->szTitle[sizeof(pMail->szTitle) - 1] = '\0';

    strncpy(pMail->szSenderName, pcszSenderName, sizeof(pMail->szSenderName));
    pMail->szSenderName[sizeof(pMail->szSenderName) - 1] = '\0';

    pMail->nSendTime = g_pSO3GameCenter->m_nTimeNow;
    pMail->nRecvTime = pMail->nSendTime + nDelayTime;
    pMail->nLockTime = 0;

    pMail->nMoney    = nMoney;

    if (uTextLen != 0)
    {
        assert(pbyTextData);
        pMail->wTextLen  = (WORD)uTextLen;
        memcpy(pMail->byData, pbyTextData, uTextLen);
    }

    if (uItemDataLen != 0)
    {
        assert(pbyItemData);
        KGLOG_PROCESS_ERROR(nItemPrice >= 0);

        pMail->ItemDesc[0].bAcquired = false;
        pMail->ItemDesc[0].byDataLen = (BYTE)uItemDataLen;
        pMail->ItemDesc[0].nPrice    = 0; // nItemPrice; // 货到付款功能暂时关闭

        memcpy(pMail->byData + uTextLen, pbyItemData, uItemDataLen);
    }

    m_OperationQueue.push_back(pRequest);

    bResult = true;
Exit0:
    if (!bResult)
    {
        KMEMORY_FREE(pRequest);
    }
    return bResult;
}

BOOL KMailManager::PostRequestGetMailList(DWORD dwMailBoxID, DWORD dwStartID)
{
    BOOL                            bResult      = false;
    KMAIL_GET_MAIL_LIST_OPERATION*  pRequest     = NULL;

    pRequest = (KMAIL_GET_MAIL_LIST_OPERATION*)KMemory::Alloc(
        sizeof(KMAIL_GET_MAIL_LIST_OPERATION)
    );
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byType       = emoGetMailList;
    pRequest->dwMailBoxID  = dwMailBoxID;
    pRequest->dwStartID    = dwStartID;
    pRequest->uSize        = sizeof(KMAIL_GET_MAIL_LIST_OPERATION);
    
    m_OperationQueue.push_back(pRequest);

    bResult = true;
Exit0:
    if (!bResult)
    {
        KMEMORY_FREE(pRequest);
    }
    return bResult;
}

BOOL KMailManager::PostRequestDeleteMail(DWORD dwMailBoxID, DWORD dwMailID)
{
    BOOL                          bResult      = false;
    KMAIL_DELETE_MAIL_OPERATION*  pRequest     = NULL;

    pRequest = (KMAIL_DELETE_MAIL_OPERATION*)KMemory::Alloc(
        sizeof(KMAIL_DELETE_MAIL_OPERATION)
    );
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byType       = emoDeleteMail;
    pRequest->dwMailBoxID  = dwMailBoxID;
    pRequest->uSize        = sizeof(KMAIL_DELETE_MAIL_OPERATION);
    pRequest->dwMailID     = dwMailID;

    m_OperationQueue.push_back(pRequest);

    bResult = true;
Exit0:
    if (!bResult)
    {
        KMEMORY_FREE(pRequest);
    }
    return bResult;
}

BOOL KMailManager::PostRequestQueryMailContent(DWORD dwMailBoxID, DWORD dwMailID, KMAIL_TYPE eMailType)
{
    BOOL                                 bResult      = false;
    KMAIL_QUERY_MAIL_CONTENT_OPERATION*  pRequest     = NULL;

    pRequest = (KMAIL_QUERY_MAIL_CONTENT_OPERATION*)KMemory::Alloc(
        sizeof(KMAIL_QUERY_MAIL_CONTENT_OPERATION)
    );
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byType       = emoQueryMailContent;
    pRequest->dwMailBoxID  = dwMailBoxID;
    pRequest->uSize        = sizeof(KMAIL_QUERY_MAIL_CONTENT_OPERATION);
    pRequest->dwMailID     = dwMailID;
    pRequest->eMailType    = eMailType;

    m_OperationQueue.push_back(pRequest);

    bResult = true;
Exit0:
    if (!bResult)
    {
        KMEMORY_FREE(pRequest);
    }
    return bResult;
}

BOOL KMailManager::PostRequestAcquireMailMoney(DWORD dwMailBoxID, DWORD dwMailID)
{
    BOOL                                 bResult     = false;
    KMAIL_ACQUIRE_MAIL_MONEY_OPERATION*  pRequest    = NULL;

    pRequest = (KMAIL_ACQUIRE_MAIL_MONEY_OPERATION*)KMemory::Alloc(
        sizeof(KMAIL_ACQUIRE_MAIL_MONEY_OPERATION)
    );
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byType       = emoAcquireMailMoney;
    pRequest->dwMailBoxID  = dwMailBoxID;
    pRequest->uSize        = sizeof(KMAIL_ACQUIRE_MAIL_MONEY_OPERATION);
    pRequest->dwMailID     = dwMailID;

    m_OperationQueue.push_back(pRequest);

    bResult = true;
Exit0:
    if (!bResult)
    {
        KMEMORY_FREE(pRequest);
    }
    return bResult;
}

BOOL KMailManager::PostRequestAcquireMailItem(DWORD dwMailBoxID, DWORD dwMailID, int nIndex)
{
    BOOL                                bResult     = false;
    KMAIL_ACQUIRE_MAIL_ITEM_OPERATION*  pRequest    = NULL;

    pRequest = (KMAIL_ACQUIRE_MAIL_ITEM_OPERATION*)KMemory::Alloc(
        sizeof(KMAIL_ACQUIRE_MAIL_ITEM_OPERATION)
    );
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byType       = emoAcquireMailItem;
    pRequest->dwMailBoxID  = dwMailBoxID;
    pRequest->uSize        = sizeof(KMAIL_ACQUIRE_MAIL_ITEM_OPERATION);
    pRequest->dwMailID     = dwMailID;
    pRequest->nIndex       = (BYTE)nIndex;

    m_OperationQueue.push_back(pRequest);

    bResult = true;
Exit0:
    if (!bResult)
    {
        KMEMORY_FREE(pRequest);
    }
    return bResult;
}

BOOL KMailManager::PostRequestReleaseMailMoney(DWORD dwMailBoxID, DWORD dwMailID)
{
    BOOL                                   bResult     = false;
    KMAIL_RELEASE_MAIL_MONEY_OPERATION*    pRequest    = NULL;

    pRequest = (KMAIL_RELEASE_MAIL_MONEY_OPERATION*)KMemory::Alloc(
        sizeof(KMAIL_RELEASE_MAIL_MONEY_OPERATION)
    );
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byType       = emoReleaseMailMoney;
    pRequest->dwMailBoxID  = dwMailBoxID;
    pRequest->uSize        = sizeof(KMAIL_RELEASE_MAIL_MONEY_OPERATION);
    pRequest->dwMailID     = dwMailID;

    m_OperationQueue.push_back(pRequest);

    bResult = true;
Exit0:
    if (!bResult)
    {
        KMEMORY_FREE(pRequest);
    }
    return bResult;
}

BOOL KMailManager::PostRequestReleaseMailItem(DWORD dwMailBoxID, DWORD dwMailID,int nIndex)
{
    BOOL                                  bResult     = false;
    KMAIL_RELEASE_MAIL_ITEM_OPERATION*    pRequest    = NULL;

    pRequest = (KMAIL_RELEASE_MAIL_ITEM_OPERATION*)KMemory::Alloc(
        sizeof(KMAIL_RELEASE_MAIL_ITEM_OPERATION)
    );
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byType       = emoReleaseMailItem;
    pRequest->dwMailBoxID  = dwMailBoxID;
    pRequest->uSize        = sizeof(KMAIL_RELEASE_MAIL_ITEM_OPERATION);
    pRequest->dwMailID     = dwMailID;
    pRequest->nIndex       = nIndex;

    m_OperationQueue.push_back(pRequest);

    bResult = true;
Exit0:
    if (!bResult)
    {
        KMEMORY_FREE(pRequest);
    }
    return bResult;
}

BOOL KMailManager::PostRequestUnLockMail(DWORD dwMailBoxID, DWORD dwMailID)
{
    BOOL                            bResult     = false;
    KMAIL_UNLOCK_MAIL_OPERATION*    pRequest    = NULL;

    pRequest = (KMAIL_UNLOCK_MAIL_OPERATION*)KMemory::Alloc(
        sizeof(KMAIL_UNLOCK_MAIL_OPERATION)
    );
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byType       = emoUnLockMail;
    pRequest->dwMailBoxID  = dwMailBoxID;
    pRequest->uSize        = sizeof(KMAIL_UNLOCK_MAIL_OPERATION);
    pRequest->dwMailID     = dwMailID;

    m_OperationQueue.push_back(pRequest);

    bResult = true;
Exit0:
    if (!bResult)
    {
        KMEMORY_FREE(pRequest);
    }
    return bResult;
}

BOOL KMailManager::PostRequestSetMailRead(DWORD dwMailBoxID, DWORD dwMailID)
{
    BOOL                              bResult     = false;
    KMAIL_SET_MAIL_READ_OPERATION*    pRequest    = NULL;

    pRequest = (KMAIL_SET_MAIL_READ_OPERATION*)KMemory::Alloc(
        sizeof(KMAIL_SET_MAIL_READ_OPERATION)
    );
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byType       = emoSetMailRead;
    pRequest->dwMailBoxID  = dwMailBoxID;
    pRequest->uSize        = sizeof(KMAIL_SET_MAIL_READ_OPERATION);
    pRequest->dwMailID     = dwMailID;

    m_OperationQueue.push_back(pRequest);

    bResult = true;
Exit0:
    if (!bResult)
    {
        KMEMORY_FREE(pRequest);
    }
    return bResult;
}

BOOL KMailManager::PostRequestUserReturnMail(DWORD dwMailBoxID, DWORD dwMailID)
{
    BOOL                                bResult     = false;
    KMAIL_USER_RETURN_MAIL_OPERATION*   pRequest    = NULL;

    pRequest = (KMAIL_USER_RETURN_MAIL_OPERATION*)KMemory::Alloc(
        sizeof(KMAIL_USER_RETURN_MAIL_OPERATION)
    );
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byType       = emoUserReturnMail;
    pRequest->dwMailBoxID  = dwMailBoxID;
    pRequest->uSize        = sizeof(KMAIL_USER_RETURN_MAIL_OPERATION);
    pRequest->dwMailID     = dwMailID;

    m_OperationQueue.push_back(pRequest);

    bResult = true;
Exit0:
    if (!bResult)
    {
        KMEMORY_FREE(pRequest);
    }
    return bResult;
}

BOOL KMailManager::PostRequestWithdrawMail(DWORD dwMailBoxID)
{
    BOOL                                bResult     = false;
    KMAIL_WITHDRAW_MAIL_OPERATION*      pRequest    = NULL;

    pRequest = (KMAIL_WITHDRAW_MAIL_OPERATION*)KMemory::Alloc(
        sizeof(KMAIL_WITHDRAW_MAIL_OPERATION)
    );
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byType       = emoWithdrawMail;
    pRequest->dwMailBoxID  = dwMailBoxID;
    pRequest->uSize        = sizeof(KMAIL_WITHDRAW_MAIL_OPERATION);

    m_OperationQueue.push_back(pRequest);

    bResult = true;
Exit0:
    if (!bResult)
    {
        KMEMORY_FREE(pRequest);
    }
    return bResult;
}

BOOL KMailManager::PostRequestDeleteAll(DWORD dwMailBoxID)
{
    BOOL                                bResult     = false;
    KMAIL_DELETE_ALL_OPERATION*         pRequest    = NULL;

    pRequest = (KMAIL_DELETE_ALL_OPERATION*)KMemory::Alloc(
        sizeof(KMAIL_DELETE_ALL_OPERATION)
    );
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byType       = emoDeleteAll;
    pRequest->dwMailBoxID  = dwMailBoxID;
    pRequest->uSize        = sizeof(KMAIL_DELETE_ALL_OPERATION);

    m_OperationQueue.push_back(pRequest);

    bResult = true;
Exit0:
    if (!bResult)
    {
        KMEMORY_FREE(pRequest);
    }
    return bResult;
}

BOOL KMailManager::PostRequestLoadMailCount(DWORD dwMailBoxID)
{
    BOOL bResult = false;
    KMAIL_LOAD_COUNT_OPERATION* pRequest = NULL;

    pRequest = (KMAIL_LOAD_COUNT_OPERATION*)KMemory::Alloc(sizeof(KMAIL_LOAD_COUNT_OPERATION));
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->byType      = emoLoadMailCount;
    pRequest->dwMailBoxID = dwMailBoxID;
    pRequest->uSize       = sizeof(KMAIL_LOAD_COUNT_OPERATION);

    m_OperationQueue.push_back(pRequest);

    bResult = true;
Exit0:
    if (!bResult)
    {
        KMEMORY_FREE(pRequest);
    }
    return bResult;
}

//////////////////////////////////////////////////////////////////////////

BOOL KMailManager::OnLoadMailBoxRespond(
    BOOL bSucceed, DWORD dwMailBoxID, time_t nBaseTime,
    BYTE* pbyMailBoxInfo, size_t uMailBoxLen
)
{
    BOOL        bResult         = false;
    BOOL        bRetCode        = false;
    KMailBox*   pMailBox        = NULL;
    DWORD       dwMaxMailIndex  = 0;

    KG_PROCESS_ERROR(bSucceed);

    pMailBox = NewMailBox(dwMailBoxID);
    KGLOG_PROCESS_ERROR(pMailBox);

    pMailBox->m_bSaved = true;

    if (uMailBoxLen > 0)
    {
        bRetCode = pMailBox->Load(pbyMailBoxInfo, uMailBoxLen);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    pMailBox->m_nBaseTime   = nBaseTime;
    pMailBox->m_dwID        = dwMailBoxID;

    dwMaxMailIndex = pMailBox->m_dwLastGlobalMailIndex;
    for (KGLOBAL_TABLE::iterator it = m_GlobalMailTable.begin(); it != m_GlobalMailTable.end(); ++it)
    {
        if (it->first <= pMailBox->m_dwLastGlobalMailIndex)
            continue;

        bRetCode = pMailBox->SendGlobalMail((it->second).pMail);

        if (!bRetCode)
            break;

        dwMaxMailIndex = it->first;
    }

    pMailBox->m_dwLastGlobalMailIndex = dwMaxMailIndex;

    bResult = true;
Exit0:
    m_bCacheReady = true;
    return bResult;
}

void KMailManager::OnCheckWithdrawMailRespond(int nMailBoxCount, DWORD* pdwMailBoxIDList)
{
    assert(pdwMailBoxIDList);

    for (int i = 0; i < nMailBoxCount; i++)
    {
        m_WithdrawMailQueue.push_back(pdwMailBoxIDList[i]);
    }
}

BOOL KMailManager::GetCachedMailCount(DWORD dwMailBoxID, KMailCount* pMailCount)
{
    BOOL        bResult = false;
    KMailCountTable::iterator it = m_MailCountTable.find(dwMailBoxID);

    KG_PROCESS_ERROR(it != m_MailCountTable.end());

    *pMailCount = it->second;

    bResult = true;
Exit0:
    return bResult;
}

void KMailManager::UnLoadMailCount(DWORD dwMailBoxID)
{
    m_MailCountTable.erase(dwMailBoxID);

    return;
}

//////////////////////////////////////////////////////////////////////////

BOOL KMailManager::SendGlobalMail(time_t nLifeTime, KMail* pMail, size_t uMailLen)
{
    BOOL                                        bResult         = false;
    BOOL                                        bRetCode        = false;
    KMail*                                      pNewMail        = false;
    unsigned long long                          ullMailIndex    = 0;
    DWORD                                       dwMailIndex     = 0;
    KGlobalMail                                 GlobalMail;
    std::pair<KGLOBAL_TABLE::iterator, bool>    InsRet;

    assert(pMail);

    pNewMail = (KMail*)KMemory::Alloc(uMailLen);
    KGLOG_PROCESS_ERROR(pNewMail);

    memcpy(pNewMail, pMail, uMailLen);

    bRetCode = g_pSO3GameCenter->m_piSequence->GenerateID("GlobalMailIndex", 1, &ullMailIndex);
    KGLOG_PROCESS_ERROR(bRetCode);

    KGLOG_PROCESS_ERROR((ullMailIndex > 0) && "GlobalMailIndex Error!\n");
    KGLOG_PROCESS_ERROR(ullMailIndex < UINT_MAX);

    dwMailIndex = (DWORD)ullMailIndex;

    GlobalMail.nEndTime = g_pSO3GameCenter->m_nTimeNow + nLifeTime;
    GlobalMail.pMail    = pNewMail;

    InsRet = m_GlobalMailTable.insert(std::make_pair(dwMailIndex, GlobalMail));
    KGLOG_PROCESS_ERROR(InsRet.second);

    for (KCacheTable::iterator it = m_CacheTable.begin(); it != m_CacheTable.end(); ++it)
    {
        BOOL      bRetCode = false;
        KMailBox* pMailBox = &((it->second)->MailBox);

        bRetCode = pMailBox->SendGlobalMail(pNewMail);
        if (bRetCode)
        {
            pMailBox->m_dwLastGlobalMailIndex = dwMailIndex;
        }
    }

    g_pSO3GameCenter->m_MiscDB.DoSaveGlobalMail(GlobalMail.nEndTime, dwMailIndex, pNewMail, uMailLen);
    
    bResult = true;
Exit0:
    if (!bResult)
    {
        KMEMORY_FREE(pNewMail);
    }
    return bResult;
}

BOOL KMailManager::SendGlobalMail(
    int nLifeTime, BYTE byType,
    const char* pcszSenderName,
    const char* pcszTitle, 
    int nMoney,
    BYTE* pbyTextData, size_t uTextLen,
    BYTE* pbyItemData, size_t uItemDataLen, int nItemPrice
)
{
    BOOL                bResult      = false;
    BOOL                bRetCode     = false;
    KMail*              pMail        = NULL;
    size_t              uMailLen     = sizeof(KMail) + uTextLen + uItemDataLen;
    unsigned long long  ullMailIndex = 0;
    DWORD               dwMailIndex  = 0;
    KGlobalMail                                 GlobalMail;
    std::pair<KGLOBAL_TABLE::iterator, bool>    InsRet;

    assert(pcszTitle);

    pMail = (KMail*)KMemory::Alloc(uMailLen);
    KGLOG_PROCESS_ERROR(pMail);

    memset(pMail, 0, uMailLen);

    pMail->bRead     = false;
    pMail->byType    = byType;

    strncpy(pMail->szTitle, pcszTitle, sizeof(pMail->szTitle));
    pMail->szTitle[sizeof(pMail->szTitle) - 1] = '\0';

    strncpy(pMail->szSenderName, pcszSenderName, sizeof(pMail->szSenderName));
    pMail->szSenderName[sizeof(pMail->szSenderName) - 1] = '\0';

    pMail->nSendTime = g_pSO3GameCenter->m_nTimeNow;
    pMail->nRecvTime = pMail->nSendTime;
    pMail->nLockTime = 0;

    pMail->nMoney    = nMoney;

    if (uTextLen != 0)
    {
        assert(pbyTextData);
        pMail->wTextLen  = (WORD)uTextLen;
        memcpy(pMail->byData, pbyTextData, uTextLen);
    }

    if (uItemDataLen != 0)
    {
        assert(pbyItemData);

        pMail->ItemDesc[0].bAcquired = false;
        pMail->ItemDesc[0].byDataLen = (BYTE)uItemDataLen;
        // pMail->ItemDesc[0].nPrice    = nItemPrice;
        // 货到付款暂不开放
        pMail->ItemDesc[0].nPrice    = 0;

        memcpy(pMail->byData + uTextLen, pbyItemData, uItemDataLen);
    }

    bRetCode = g_pSO3GameCenter->m_piSequence->GenerateID("GlobalMailIndex", 1, &ullMailIndex);
    KGLOG_PROCESS_ERROR(bRetCode);

    KGLOG_PROCESS_ERROR((ullMailIndex > 0) && "GlobalMailIndex Error!\n");
    KGLOG_PROCESS_ERROR(ullMailIndex < UINT_MAX);

    dwMailIndex = (DWORD)ullMailIndex;

    GlobalMail.nEndTime = g_pSO3GameCenter->m_nTimeNow + nLifeTime;
    GlobalMail.pMail    = pMail;

    InsRet = m_GlobalMailTable.insert(std::make_pair(dwMailIndex, GlobalMail));
    KGLOG_PROCESS_ERROR(InsRet.second);

    for (KCacheTable::iterator it = m_CacheTable.begin(); it != m_CacheTable.end(); ++it)
    {
        BOOL      bRetCode = false;
        KMailBox* pMailBox = &((it->second)->MailBox);

        bRetCode = pMailBox->SendGlobalMail(pMail);
        if (bRetCode)
        {
            pMailBox->m_dwLastGlobalMailIndex = dwMailIndex;
        }
    }

    g_pSO3GameCenter->m_MiscDB.DoSaveGlobalMail(GlobalMail.nEndTime, dwMailIndex, pMail, uMailLen);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KMailManager::DeleteGlobalMail(DWORD dwMailID)
{
    BOOL bResult = false;

    KGLOBAL_TABLE::iterator it = m_GlobalMailTable.find(dwMailID);
    KGLOG_PROCESS_ERROR(it != m_GlobalMailTable.end());

    g_pSO3GameCenter->m_MiscDB.DoDeleteGlobalMail(it->first);
    KMEMORY_FREE((it->second).pMail);
    m_GlobalMailTable.erase(it);

    bResult = true;
Exit0:
    return bResult;
}

void KMailManager::DeleteAllGlobalMail()
{
    for(KGLOBAL_TABLE::iterator it = m_GlobalMailTable.begin(); it != m_GlobalMailTable.end(); ++it)
    {
        g_pSO3GameCenter->m_MiscDB.DoDeleteGlobalMail(it->first);
        KMEMORY_FREE((it->second).pMail);
    }
    m_GlobalMailTable.clear();

    return;
}

//////////////////////////////////////////////////////////////////////////

BOOL KMailManager::OnSendMailOperation(KMailBox* pMailBox, BYTE* pbyData, size_t uDataLen)
{
    BOOL                        bResult      = false;
    BOOL                        bRetCode     = false;
    BOOL                        bSucceed     = false;
    KMAIL_SEND_MAIL_OPERATION*  pOperation   = (KMAIL_SEND_MAIL_OPERATION*)pbyData;
    KMail*                      pMail        = (KMail*)pOperation->byData;
    int                         nRequestID   = pOperation->byRequestID;
    KRole*                      pSender      = NULL;
    KRole*                      pReceiver    = NULL;
    size_t                      uMailLen     = 0;
    BOOL                        bOnLine      = false;

    pReceiver = g_pSO3GameCenter->m_RoleManager.GetRole(pMailBox->m_dwID);
    KGLOG_PROCESS_ERROR(pReceiver);

    assert(uDataLen >= sizeof(KMAIL_SEND_MAIL_OPERATION) + sizeof(KMail));

    uMailLen = sizeof(KMail) + pMail->wTextLen;
    for (int i = 0; i < KMAIL_MAX_ITEM_COUNT; i++)
    {
        uMailLen += pMail->ItemDesc[i].byDataLen;
    }
    KGLOG_PROCESS_ERROR(uDataLen == sizeof(KMAIL_SEND_MAIL_OPERATION) + uMailLen);

    if (pMail->byType == eMailType_Player)
    {
        KMailCount Count;
        
        pMailBox->GetMailCount(&Count);

        pSender = g_pSO3GameCenter->m_RoleManager.GetRole(pMail->szSenderName);
        KGLOG_PROCESS_ERROR(pSender);

        bOnLine = pSender->IsOnline();

        if (Count.nPlayerMail >= g_pSO3GameCenter->m_Settings.m_nMaxPlayerMailCount)
        {
            if (bOnLine)
            {
                bRetCode = g_pSO3GameCenter->m_GameServer.DoSendMailRespond(
                    nRequestID, pSender, mrcMailBoxFull, pMail, uMailLen, pReceiver->m_szName
                );
                KGLOG_PROCESS_ERROR(bRetCode);
            }
            else
            {
                char szTitle[MAIL_TITLE_LEN];

                snprintf(
                    szTitle, sizeof(szTitle),
                    "[%s]%s", g_pSO3GameCenter->m_Settings.m_szMailSendFailedName, pMail->szTitle
                );
                szTitle[sizeof(szTitle) - 1] = '\0';

                strncpy(pMail->szTitle, szTitle, sizeof(pMail->szTitle));
                pMail->szTitle[sizeof(pMail->szTitle) - 1] = '\0';

                snprintf(pMail->szSenderName, sizeof(pMail->szSenderName), "%s", g_pSO3GameCenter->m_Settings.m_szSystemMailSender);
                pMail->szSenderName[sizeof(pMail->szSenderName) - 1] = '\0';

                pMail->byType = eMailType_System;

                uMailLen = sizeof(KMail) + pMail->wTextLen;
                for (int i = 0; i < KMAIL_MAX_ITEM_COUNT; i++)
                {
                    uMailLen += pMail->ItemDesc[i].byDataLen;
                }

                PostRequestSendMail(0, pSender->m_dwPlayerID, pMail, uMailLen);
            }
            goto Exit1;
        }
    }

    bSucceed = pMailBox->SendMail(pMail, uMailLen);

    if (pMail->byType == eMailType_Player && bOnLine)
    {
        bRetCode = g_pSO3GameCenter->m_GameServer.DoSendMailRespond(
            nRequestID, pSender, 
            bSucceed ? mrcSucceed : mrcFailed, 
            pMail, uMailLen, pReceiver->m_szName
        );
        KGLOG_PROCESS_ERROR(bRetCode);
    }

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KMailManager::OnGetMailListOperation(KMailBox* pMailBox, BYTE* pbyData, size_t uDataLen)
{
    BOOL                            bResult         = false;
    BOOL                            bRetCode        = false;
    KMAIL_GET_MAIL_LIST_OPERATION*  pOperation      = (KMAIL_GET_MAIL_LIST_OPERATION*)pbyData;
    int                             nCount          = 0;
    KMailListInfo*                  pMailListInfo   = NULL;
    KMailList                       MailList;

    assert(uDataLen == sizeof(KMAIL_GET_MAIL_LIST_OPERATION));

    MailList.reserve(SYNC_MAIL_LIST_MAX_COUNT);

    nCount = pMailBox->GetMailList(&MailList, g_pSO3GameCenter->m_nTimeNow, pOperation->dwStartID);

    pMailListInfo = nCount > 0 ? &MailList[0] : NULL;

    bRetCode = g_pSO3GameCenter->m_GameServer.DoGetMailListRespond(
        pOperation->dwMailBoxID, pMailListInfo, nCount
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KMailManager::OnDeleteMailOperation(KMailBox* pMailBox, BYTE* pbyData, size_t uDataLen)
{
    BOOL                          bResult     = false;
    BOOL                          bRetCode    = false;
    int                           nResult     = mrcFailed;
    KMAIL_DELETE_MAIL_OPERATION*  pOperation  = (KMAIL_DELETE_MAIL_OPERATION*)pbyData;
    KRole*                        pRole       = NULL;
    KMail*                        pMail       = NULL;
    KMailItemDesc*                pItemDesc   = NULL;

    assert(uDataLen == sizeof(KMAIL_DELETE_MAIL_OPERATION));

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pOperation->dwMailBoxID);
    KGLOG_PROCESS_ERROR(pRole);

    pMail = pMailBox->GetMail(pOperation->dwMailID);
    if (!pMail)
    {
        nResult = mrcMailNotFound;
        goto Exit0;
    }

    for (int i = 0; i < KMAIL_MAX_ITEM_COUNT; i++)
    {
        pItemDesc = &pMail->ItemDesc[i];

        if (pItemDesc->byDataLen == 0)
            continue;

        if (pItemDesc->bAcquired)
            continue;

        nResult = mrcDeleteRefused;
        goto Exit0;
    }

    if (pMail->nLockTime > 0)
    {
        nResult = mrcDeleteRefused;
        goto Exit0;
    }

    bRetCode = pMailBox->DeleteMail(pOperation->dwMailID);
    KGLOG_PROCESS_ERROR(bRetCode);

    nResult = mrcSucceed;

    bResult = true;
Exit0:
    if (pRole)
    {
        g_pSO3GameCenter->m_GameServer.DoMailGeneralRespond(
            pRole->m_nConnIndex, pOperation->dwMailBoxID, pOperation->dwMailID, (BYTE)nResult
        );
    }
    return bResult;
}

BOOL KMailManager::OnQueryMailContentOperation(KMailBox* pMailBox, BYTE* pbyData, size_t uDataLen)
{
    BOOL                                bResult           = false;
    BOOL                                bRetCode          = false;
    KMAIL_QUERY_MAIL_CONTENT_OPERATION* pOperation        = (KMAIL_QUERY_MAIL_CONTENT_OPERATION*)pbyData;
    KRole*                              pRole             = NULL;
    KMail*                              pMail             = NULL;
    KMailContent*                       pContent          = NULL;
    size_t                              uContentLen       = 0;

    assert(uDataLen == sizeof(KMAIL_QUERY_MAIL_CONTENT_OPERATION));

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pOperation->dwMailBoxID);
    KGLOG_PROCESS_ERROR(pRole);

    pMail = pMailBox->GetMail(pOperation->dwMailID);
    KGLOG_PROCESS_ERROR(pMail);
    KGLOG_PROCESS_ERROR(pMail->byType == (BYTE)pOperation->eMailType);

    KGLOG_PROCESS_ERROR(g_pSO3GameCenter->m_nTimeNow >= pMail->nRecvTime);

    bRetCode = ExtractContentFromMail(pMail, &pContent, &uContentLen);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = g_pSO3GameCenter->m_GameServer.DoSyncMailContent(
        pRole->m_nConnIndex, pOperation->dwMailBoxID, pOperation->dwMailID,
        mrcSucceed, pContent, uContentLen
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    if (pContent)
    {
        KMEMORY_FREE(pContent);
    }
    return bResult;
}

BOOL KMailManager::OnAcquireMailMoneyOperation(KMailBox* pMailBox, BYTE* pbyData, size_t uDataLen)
{
    BOOL                                bResult     = false;
    BOOL                                bRetCode    = false;
    KMAIL_ACQUIRE_MAIL_MONEY_OPERATION* pOperation  = (KMAIL_ACQUIRE_MAIL_MONEY_OPERATION*)pbyData;
    KRole*                              pRole       = NULL;
    KMail*                              pMail       = NULL;
    int                                 nMoney      = 0;
    size_t                              uTextLen    = 0;
    size_t                              uItemLen    = 0;
    char*                               pszText     = NULL;
    BYTE*                               pbyItem     = NULL;
    int                                 nItemPrice  = 0;

    assert(uDataLen == sizeof(KMAIL_ACQUIRE_MAIL_MONEY_OPERATION));

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pOperation->dwMailBoxID);
    KGLOG_PROCESS_ERROR(pRole);

    pMail = pMailBox->GetMail(pOperation->dwMailID);
    if (!pMail)
    {
        g_pSO3GameCenter->m_GameServer.DoMailGeneralRespond(
            pRole->m_nConnIndex, pOperation->dwMailBoxID, pOperation->dwMailID, mrcMailNotFound
        );
        goto Exit0;
    }
    KGLOG_PROCESS_ERROR(g_pSO3GameCenter->m_nTimeNow >= pMail->nRecvTime);

    KG_PROCESS_ERROR(g_pSO3GameCenter->m_nTimeNow >= pMail->nLockTime);

    nMoney = pMail->nMoney;
    KG_PROCESS_ERROR(nMoney > 0);

    pMail->nLockTime = g_pSO3GameCenter->m_nTimeNow + 60 * 3;

    g_pSO3GameCenter->m_GameServer.DoGiveMailMoneyToPlayer(
        pRole->m_nConnIndex, pOperation->dwMailBoxID, pOperation->dwMailID, pMail->byType, nMoney, pMail->szSenderName
    );

    bResult = true;
Exit0:
    return bResult;
}

BOOL KMailManager::OnAcquireMailItemOperation(KMailBox* pMailBox, BYTE* pbyData, size_t uDataLen)
{
    BOOL                                bResult         = false;
    BOOL                                bRetCode        = false;
    KMAIL_ACQUIRE_MAIL_ITEM_OPERATION*  pOperation      = (KMAIL_ACQUIRE_MAIL_ITEM_OPERATION*)pbyData;
    KRole*                              pRole           = NULL;
    KMail*                              pMail           = NULL;
    int                                 nItemIndex      = pOperation->nIndex;
    int                                 nItemPrice      = 0;
    BYTE*                               pbyItemData     = NULL;
    size_t                              uItemDataLen    = 0;

    assert(uDataLen == sizeof(KMAIL_ACQUIRE_MAIL_ITEM_OPERATION));

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pOperation->dwMailBoxID);
    KGLOG_PROCESS_ERROR(pRole);

    pMail = pMailBox->GetMail(pOperation->dwMailID);
    if (!pMail)
    {
        g_pSO3GameCenter->m_GameServer.DoMailGeneralRespond(
            pRole->m_nConnIndex, pOperation->dwMailBoxID, pOperation->dwMailID, mrcMailNotFound
        );
        goto Exit0;
    }

    KG_PROCESS_ERROR(g_pSO3GameCenter->m_nTimeNow > pMail->nLockTime);

    KGLOG_PROCESS_ERROR(g_pSO3GameCenter->m_nTimeNow >= pMail->nRecvTime);
    KGLOG_PROCESS_ERROR(nItemIndex >= 0 && nItemIndex < KMAIL_MAX_ITEM_COUNT);
    KGLOG_PROCESS_ERROR(pMail->ItemDesc[nItemIndex].byDataLen > 0);
    KG_PROCESS_ERROR(!(pMail->ItemDesc[nItemIndex].bAcquired));

    uItemDataLen    = pMail->ItemDesc[nItemIndex].byDataLen;
    nItemPrice      = pMail->ItemDesc[nItemIndex].nPrice; 
    pbyItemData     = pMail->byData + pMail->wTextLen;

    for (int i = 0; i < nItemIndex; i++)
    {
        pbyItemData += pMail->ItemDesc[i].byDataLen;
    }

    pMail->nLockTime = g_pSO3GameCenter->m_nTimeNow + 60 * 3;

    g_pSO3GameCenter->m_GameServer.DoGiveMailItemToPlayer(
        pRole->m_nConnIndex, pOperation->dwMailBoxID, pOperation->dwMailID, pMail->byType,
        mrcSucceed, nItemIndex, nItemPrice, pbyItemData, uItemDataLen, pMail->szSenderName
    );

    bResult = true;
Exit0:
    return bResult;
}

BOOL KMailManager::OnReleaseMailMoneyOperation(KMailBox* pMailBox, BYTE* pbyData, size_t uDataLen)
{
    BOOL                                 bResult     = false;
    BOOL                                 bRetCode    = false;
    KMAIL_RELEASE_MAIL_MONEY_OPERATION*  pOperation  = (KMAIL_RELEASE_MAIL_MONEY_OPERATION*)pbyData;
    KMail*                               pMail       = NULL;
    KRole*                               pRole       = NULL;
    KMailContent*                        pContent    = NULL;
    size_t                               uContentLen = 0;

    assert(uDataLen == sizeof(KMAIL_RELEASE_MAIL_MONEY_OPERATION));

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pOperation->dwMailBoxID);
    KGLOG_PROCESS_ERROR(pRole);

    pMail = pMailBox->GetMail(pOperation->dwMailID);
    KGLOG_PROCESS_ERROR(pMail);

    KGLOG_PROCESS_ERROR(g_pSO3GameCenter->m_nTimeNow >= pMail->nRecvTime);

    pMail->nLockTime   = 0;
    pMail->nMoney      = 0;
    pMailBox->m_bSaved = false;

    bRetCode = ExtractContentFromMail(pMail, &pContent, &uContentLen);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = g_pSO3GameCenter->m_GameServer.DoSyncMailContent(
        pRole->m_nConnIndex, pOperation->dwMailBoxID, pOperation->dwMailID,
        mrcSucceed, pContent, uContentLen
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KMailManager::OnReleaseMailItemOperation(KMailBox* pMailBox, BYTE* pbyData, size_t uDataLen)
{
    BOOL                                 bResult     = false;
    BOOL                                 bRetCode    = false;
    KMAIL_RELEASE_MAIL_ITEM_OPERATION*   pOperation  = (KMAIL_RELEASE_MAIL_ITEM_OPERATION*)pbyData;
    KMail*                               pMail       = NULL;
    KRole*                               pRole       = NULL;
    KMailContent*                        pContent    = NULL;
    size_t                               uContentLen = 0;

    assert(uDataLen == sizeof(KMAIL_RELEASE_MAIL_ITEM_OPERATION));

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pOperation->dwMailBoxID);
    KGLOG_PROCESS_ERROR(pRole);

    pMail = pMailBox->GetMail(pOperation->dwMailID);
    KGLOG_PROCESS_ERROR(pMail);

    KGLOG_PROCESS_ERROR(g_pSO3GameCenter->m_nTimeNow >= pMail->nRecvTime);

    pMail->nLockTime = 0;

    KGLOG_PROCESS_ERROR(pOperation->nIndex >= 0 && pOperation->nIndex < KMAIL_MAX_ITEM_COUNT);

    // MailMoneyToSender(...);  // 货到付款给发件人付钱

    pMail->ItemDesc[pOperation->nIndex].bAcquired = true;
    pMailBox->m_bSaved = false;

    bRetCode = ExtractContentFromMail(pMail, &pContent, &uContentLen);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = g_pSO3GameCenter->m_GameServer.DoSyncMailContent(
        pRole->m_nConnIndex, pOperation->dwMailBoxID, pOperation->dwMailID,
        mrcSucceed, pContent, uContentLen
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KMailManager::OnUnLockMailOperation(KMailBox* pMailBox, BYTE* pbyData, size_t uDataLen)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    KMAIL_UNLOCK_MAIL_OPERATION*    pOperation  = (KMAIL_UNLOCK_MAIL_OPERATION*)pbyData;
    KMail*                          pMail       = NULL;

    assert(uDataLen == sizeof(KMAIL_UNLOCK_MAIL_OPERATION));

    pMail = pMailBox->GetMail(pOperation->dwMailID);
    KGLOG_PROCESS_ERROR(pMail);

    KGLOG_PROCESS_ERROR(g_pSO3GameCenter->m_nTimeNow >= pMail->nRecvTime);

    pMail->nLockTime = 0;

    pMailBox->m_bSaved = false;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KMailManager::OnSetMailReadOperation(KMailBox* pMailBox, BYTE* pbyData, size_t uDataLen)
{
    BOOL                            bResult     = false;
    KMAIL_SET_MAIL_READ_OPERATION*  pOperation  = (KMAIL_SET_MAIL_READ_OPERATION*)pbyData;
    KMail*                          pMail       = NULL;

    assert(uDataLen == sizeof(KMAIL_SET_MAIL_READ_OPERATION));

    pMail = pMailBox->GetMail(pOperation->dwMailID);
    KGLOG_PROCESS_ERROR(pMail);

    KGLOG_PROCESS_ERROR(g_pSO3GameCenter->m_nTimeNow >= pMail->nRecvTime);

    if (!pMail->bRead)
    {
        pMail->bRead = true;

        pMailBox->m_bSaved = false;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KMailManager::OnUserReturnMailOperation(KMailBox* pMailBox, BYTE* pbyData, size_t uDataLen)
{
    BOOL                                bResult     = false;
    BOOL                                bRetCode    = false;
    KMAIL_USER_RETURN_MAIL_OPERATION*   pOperation  = (KMAIL_USER_RETURN_MAIL_OPERATION*)pbyData;
    KRole*                              pRole       = NULL;
    KRole*                              pSenderRole = NULL;
    KMail*                              pMail       = NULL;
    size_t                              uMailLen    = 0;
    BOOL                                bItem       = false;
    BOOL                                bPlayerMail = false;
    BOOL                                bAttachment = false;
    char                                szTitle[MAIL_TITLE_LEN];

    assert(uDataLen == sizeof(KMAIL_USER_RETURN_MAIL_OPERATION));

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pOperation->dwMailBoxID);
    KGLOG_PROCESS_ERROR(pRole);

    pMail = pMailBox->GetMail(pOperation->dwMailID);
    KGLOG_PROCESS_ERROR(pMail);

    KGLOG_PROCESS_ERROR(g_pSO3GameCenter->m_nTimeNow >= pMail->nRecvTime);

    uMailLen = sizeof(KMail) + pMail->wTextLen;
    for (int i = 0; i < KMAIL_MAX_ITEM_COUNT; i++)
    {
        if (pMail->ItemDesc[i].byDataLen == 0)
            continue;

        uMailLen += pMail->ItemDesc[i].byDataLen;
        
        pMail->ItemDesc[i].nPrice = 0;
        
        if (!pMail->ItemDesc[i].bAcquired)   // 只要还有道具，就可退信
            bItem = true;
    }

    if (pMail->byType == eMailType_Player)
    {
        bPlayerMail = true;
    }

    if ((pMail->nMoney > 0) || bItem)
    {
        bAttachment = true;
    }

    if (!bPlayerMail || !bAttachment) // 非玩家发的邮件或是邮件即没金钱也没道具，就不退回了
    {
        g_pSO3GameCenter->m_GameServer.DoMailGeneralRespond(
            pRole->m_nConnIndex, pOperation->dwMailBoxID, pOperation->dwMailID, mrcFailed
        );
        goto Exit0;
    }

    pSenderRole = g_pSO3GameCenter->m_RoleManager.GetRole(pMail->dwSenderID);
    KGLOG_PROCESS_ERROR(pSenderRole);

    snprintf(
        szTitle, sizeof(szTitle),
        "[%s]%s", g_pSO3GameCenter->m_Settings.m_szMailUserReturnTitle, pMail->szTitle
    );
    szTitle[sizeof(szTitle) - 1] = '\0';

    strncpy(pMail->szTitle, szTitle, sizeof(pMail->szTitle));
    pMail->szTitle[sizeof(pMail->szTitle) - 1] = '\0';

    strncpy(pMail->szSenderName, pRole->m_szName, sizeof(pMail->szSenderName));
    pMail->szSenderName[sizeof(pMail->szSenderName) - 1] = '\0';
    pMail->dwSenderID = pRole->m_dwPlayerID;

    pMail->nSendTime = g_pSO3GameCenter->m_nTimeNow;
    pMail->nRecvTime = g_pSO3GameCenter->m_nTimeNow;
    pMail->nLockTime = 0;

    pMail->byType = eMailType_System;

    PostRequestSendMail(0, pSenderRole->m_dwPlayerID, pMail, uMailLen);

    pMailBox->DeleteMail(pOperation->dwMailID);

    g_pSO3GameCenter->m_GameServer.DoMailGeneralRespond(
        pRole->m_nConnIndex, pOperation->dwMailBoxID, pOperation->dwMailID, mrcSucceed
    );

    bResult = true;
Exit0:
    return bResult;
}

BOOL KMailManager::OnWithdrawMailOperation(KMailBox* pMailBox, BYTE* pbyData, size_t uDataLen)
{
    BOOL                                bResult     = false;
    BOOL                                bRetCode    = false;
    KMAIL_WITHDRAW_MAIL_OPERATION*      pOperation  = (KMAIL_WITHDRAW_MAIL_OPERATION*)pbyData;
    int                                 nMailCount  = 0;
    char                                szTitle[MAIL_TITLE_LEN];
    std::vector<KMail*, KMemory::KAllocator<KMail*> > MailList;

    assert(uDataLen == sizeof(KMAIL_WITHDRAW_MAIL_OPERATION));

    MailList.reserve(g_pSO3GameCenter->m_Settings.m_nMaxPlayerMailCount);

    nMailCount = pMailBox->GetWithdrawMail(
        g_pSO3GameCenter->m_nTimeNow, g_pSO3GameCenter->m_Settings.m_nMailSurvivalTime, 
        &MailList
    );

    for (int i = 0; i < nMailCount; i++)
    {
        KMail*    pMail         = MailList[i];
        KRole*    pSenderRole   = NULL;
        size_t    uMailLen      = 0;

        assert(pMail);

        pSenderRole = g_pSO3GameCenter->m_RoleManager.GetRole(pMail->dwSenderID);

        if (pSenderRole)
        {
            snprintf(
                szTitle, sizeof(szTitle),
                "[%s]%s", g_pSO3GameCenter->m_Settings.m_szMailWithdrawName, pMail->szTitle
            );
            szTitle[sizeof(szTitle) - 1] = '\0';

            strncpy(pMail->szTitle, szTitle, sizeof(pMail->szTitle));
            pMail->szTitle[sizeof(pMail->szTitle) - 1] = '\0';

            snprintf(pMail->szSenderName, sizeof(pMail->szSenderName), "%s", g_pSO3GameCenter->m_Settings.m_szSystemMailSender);
            pMail->szSenderName[sizeof(pMail->szSenderName) - 1] = '\0';

            pMail->byType    = eMailType_System;
            pMail->nSendTime = g_pSO3GameCenter->m_nTimeNow;
            pMail->nRecvTime = g_pSO3GameCenter->m_nTimeNow;

            uMailLen = sizeof(KMail) + pMail->wTextLen;
            for (int i = 0; i < KMAIL_MAX_ITEM_COUNT; i++)
            {
                uMailLen += pMail->ItemDesc[i].byDataLen;
            }

            PostRequestSendMail(0, pSenderRole->m_dwPlayerID, pMail, uMailLen);
        }
        else
        {
            char   szLog[1024];
            char   szTemp[1024];
            BYTE*  pbyItemData = pMail->byData + pMail->wTextLen;
            KRole* pRole       = NULL;

            pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pMailBox->m_dwID);
            if (pRole == NULL)
            {
                KMEMORY_FREE(pMail);
                continue;
            }

            snprintf(szLog, sizeof(szLog), 
                "[Mail] Role %s not exist, cannot withdraw mail:" 
                "%s-->%s, title:%s, money = %d, ",
                pMail->szSenderName,
                pMail->szSenderName, pRole->m_szName, pMail->szTitle, pMail->nMoney
            );
            szLog[sizeof(szLog) - 1] = '\0';

            for (int i = 0; i < KMAIL_MAX_ITEM_COUNT; i++)
            {
                KCOMMON_ITEM_DATA* pCommonItemData = NULL;

                if (pMail->ItemDesc[i].byDataLen == 0)
                    continue;
                
                pCommonItemData = (KCOMMON_ITEM_DATA*)pbyItemData;
                pbyItemData += pMail->ItemDesc[i].byDataLen;

                if (pMail->ItemDesc[i].bAcquired)
                    continue;

                snprintf(szTemp, sizeof(szTemp), "item(TabType = %u, TabIndex = %u), ", (unsigned)pCommonItemData->byTabType, (unsigned)pCommonItemData->wTabIndex);
                szTemp[sizeof(szTemp) - 1] = '\0';

                strncat(szLog, szTemp, max(sizeof(szLog) - strlen(szLog), strlen(szTemp)));
            }

            KGLogPrintf(KGLOG_ERR, "%s", szLog);
        }

        KMEMORY_FREE(pMail);
    }

    bResult = true;
//Exit0:
    return bResult;
}

BOOL KMailManager::OnDeleteAllOperation(KMailBox* pMailBox, BYTE* pbyData, size_t uDataLen)
{
    assert(uDataLen == sizeof(KMAIL_DELETE_ALL_OPERATION));

    pMailBox->DeleteAll();

    return true;
}

BOOL KMailManager::OnLoadMailCountOperation(KMailBox* pMailBox, BYTE* pbyData, size_t uDataLen)
{ 
    BOOL        bResult     = false;
    KRole*      pRole       = NULL;
    KROLE_STATE eRoleState  = rsInvalid;
    KMailCount  MailCount;

    assert(uDataLen == sizeof(KMAIL_LOAD_COUNT_OPERATION));

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pMailBox->m_dwID);
    KGLOG_PROCESS_ERROR(pRole);

    eRoleState = pRole->GetRoleState();
    KG_PROCESS_ERROR(eRoleState != rsOffline);

    pMailBox->GetMailCount(&MailCount);

    m_MailCountTable[pMailBox->m_dwID] = MailCount;

    bResult = true;
Exit0:
    return bResult;
}

void KMailManager::UpdateMailCount(DWORD dwID, const KMailCount& crCount)
{
    KMailCountTable::iterator it = m_MailCountTable.find(dwID);

    if (it != m_MailCountTable.end())
    {
        it->second = crCount;
    }
}

BOOL KMailManager::ExtractContentFromMail(KMail* pMail, KMailContent** ppContent, size_t* puContentLen)
{
    BOOL            bResult          = false;
    KMailContent*   pContent         = NULL;
    size_t          uContentDataLen  = 0;
    size_t          uContentLen      = 0;

    assert(pMail);

    uContentDataLen = pMail->wTextLen;
    for (int i = 0; i < KMAIL_MAX_ITEM_COUNT; i++)
    {
        uContentDataLen += pMail->ItemDesc[i].byDataLen;
    }
    uContentLen = sizeof(KMailContent) + uContentDataLen;

    pContent = (KMailContent*)KMemory::Alloc(uContentLen);
    KGLOG_PROCESS_ERROR(pContent);

    pContent->nMoney    = pMail->nMoney;
    pContent->wTextLen  = pMail->wTextLen;
    for (int i = 0; i < KMAIL_MAX_ITEM_COUNT; i++)
    {
        pContent->ItemDesc[i] = pMail->ItemDesc[i];
    }

    memcpy(pContent->byData, pMail->byData, uContentDataLen);
    
    *ppContent    = pContent;
    *puContentLen = uContentLen;

    bResult = true;
Exit0:
    return bResult;
}

void KMailManager::CheckCache()
{
    int         nMailCount      = 0;
    BOOL        bRetCode        = false;
    int         nCacheMailCount = 0;
    KMailCount  Count;

    KNode* pNode = m_MailQueue.GetHead();
    while (pNode)
    {
        KCacheNode* pCacheNode = (KCacheNode*)pNode;
        KMailBox*   pMailBox   = &pCacheNode->MailBox;

        pMailBox->GetMailCount(&Count);

        nCacheMailCount += Count.nTotalCount;

        if (nCacheMailCount > m_nMaxCache)
            break;

        pNode = pNode->GetNext();
    }

    while (pNode)
    {
        KNode* pNext = pNode->GetNext();

        pNode->Remove();

        KCacheNode* pCacheNode = (KCacheNode*)pNode;
        KMailBox*   pMailBox   = &pCacheNode->MailBox;

        if (!pMailBox->m_bSaved)
        {
            size_t  uUsedLen    = 0;
            int     nMailCount  = 0;

            bRetCode = pMailBox->Save(m_pbyBuffer, MAX_MAILBOX_DB_DATA_SIZE, &uUsedLen);
            KGLOG_PROCESS_ERROR(bRetCode);

            pMailBox->GetMailCount(&Count);

            bRetCode = g_pSO3GameCenter->m_MiscDB.DoSaveMailBox(
                pMailBox->m_dwID, pMailBox->m_nBaseTime, Count.nTotalCount,
                m_pbyBuffer, uUsedLen
            );
            KGLOG_PROCESS_ERROR(bRetCode);

            pMailBox->m_bSaved = true;
        }

        pMailBox->Clear();

        m_CacheTable.erase(pCacheNode->CacheIter);
        KMemory::Delete(pCacheNode);

        pNode = pNext;
    }

Exit0:
    return;
}

KMailBox* KMailManager::GetMailBox(DWORD dwID)
{
    KCacheTable::iterator it = m_CacheTable.find(dwID);
    if (it != m_CacheTable.end())
    {
        KCacheNode* pNode = it->second;

        pNode->Remove();

        m_MailQueue.AddHead(pNode);

        return &pNode->MailBox;
    }
    return NULL;
}

KMailBox* KMailManager::NewMailBox(DWORD dwID)
{
    KMailBox*   pResult    = NULL;
    KCacheNode* pCacheNode = NULL;
    std::pair<KCacheTable::iterator, bool> InsRet;

    pCacheNode = KMemory::New<KCacheNode>();
    KGLOG_PROCESS_ERROR(pCacheNode);

    InsRet = m_CacheTable.insert(std::make_pair(dwID, pCacheNode));
    KGLOG_PROCESS_ERROR(InsRet.second);

    m_MailQueue.AddHead(pCacheNode);
    pCacheNode->CacheIter = InsRet.first;

    pResult = &pCacheNode->MailBox;
Exit0:
    if (!pResult)
    {
        KMemory::Delete(pCacheNode);
        pCacheNode = NULL;
    }
    return pResult;
}

