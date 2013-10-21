#include "stdafx.h"
#include "SO3GlobalDef.h"
#include <time.h>
#include <set>
#include <list>
#include <string>
#include "mysql.h"
#include "errmsg.h"
#include "KGPublic.h"
#include "Engine/KG_Time.h"
#include "KMiscDB.h"
#include "KRole.h"
#include "KSO3GameCenter.h"

#define REGISTER_DB_RESPOND_FUNC(ProtocolID, FuncName, ProtocolStruct)	                    \
    {                                                                                       \
        assert(ProtocolID < sizeof(m_ProcessRespondFuns) / sizeof(m_ProcessRespondFuns[0]));\
        m_ProcessRespondFuns[ProtocolID]    = &KMiscDB::FuncName;                           \
        m_uRespondProtocolSize[ProtocolID]  = sizeof(ProtocolStruct);                       \
    }

KMiscDB::KMiscDB()
{
    m_pDBHandle                 = NULL;
    m_nNextPingTime             = 0;
    m_pszSQL                    = NULL;
    
    m_nDBThreadWorkLoop         = 0;
    m_nDBThreadIdleLoop         = 0;
    m_bRequestProcessing        = false;
    m_nOperationSpeed           = 0;
    m_fOperationSpeed           = 0.0f;

    memset(m_nOperationCount, 0, sizeof(m_nOperationCount));
    memset(m_ProcessRequestFuns, 0, sizeof(m_ProcessRequestFuns));
    memset(m_ProcessRespondFuns, 0, sizeof(m_ProcessRespondFuns));
    memset(m_uRequestProtocolSize, 0, sizeof(m_uRequestProtocolSize));
    memset(m_uRespondProtocolSize, 0, sizeof(m_uRespondProtocolSize));

    REGISTER_DB_RESPOND_FUNC(emrpLoadFellowshipRespond, OnLoadFellowshipData, KLOAD_FELLOWSHIP_DATA_RESPOND);
    REGISTER_DB_RESPOND_FUNC(emrpLoadMailBoxRespond, OnLoadMailBox, KLOAD_MAIL_BOX_RESPOND);
    REGISTER_DB_RESPOND_FUNC(emrpCheckWithdrawMailRespond, OnCheckWithdrawMail, KCHECK_WITHDRAW_MAIL_RESPOND);
    REGISTER_DB_RESPOND_FUNC(emrpLoadPQListRespond, OnLoadPQList, KLOAD_PQ_LIST_RESPOND);
    REGISTER_DB_RESPOND_FUNC(emrpAuctionLookupRespond, OnAuctionLookup, KAUCTION_LOOKUP_DB_RESPOND);
    REGISTER_DB_RESPOND_FUNC(emrpAuctionBidRespond, OnAuctionBid, KAUCTION_BID_DB_RESPOND);
    REGISTER_DB_RESPOND_FUNC(emrpAuctionSellRespond, OnAuctionSell, KAUCTION_SELL_DB_RESPOND);
    REGISTER_DB_RESPOND_FUNC(emrpAuctionCancelRespond, OnAuctionCancel, KAUCTION_CANCEL_DB_RESPOND);
    REGISTER_DB_RESPOND_FUNC(emrpAuctionCheckDurationRespond, OnAuctionCheckDuration, KAUCTION_CHECK_DURATION_RESPOND);
    REGISTER_DB_RESPOND_FUNC(emrpGameCardBuyRespond, OnGameCardBuyRespond, KGAME_CARD_BUY_RESPOND);
    REGISTER_DB_RESPOND_FUNC(emrpGameCardLookupRespond, OnGameCardLookupRespond, KGAME_CARD_LOOKUP_RESPOND);
    REGISTER_DB_RESPOND_FUNC(emrpGameCardCancelRespond, OnGameCardCancelRespond, KGAME_CARD_CANCEL_RESPOND);
    REGISTER_DB_RESPOND_FUNC(emrpGameCardCheckDurationRespond, OnGameCardCheckDurationRespond, KGAME_CARD_CHECK_DURATION_RESPOND);
}

BOOL KMiscDB::Init()
{
    BOOL    bResult         = false;
    int     nRetCode        = false;

    m_pszSQL = new char[MAX_MISC_DB_SQL_SIZE];
    KGLOG_PROCESS_ERROR(m_pszSQL);

    m_pDBHandle = MySQL_Connect(
        g_pSO3GameCenter->m_Settings.m_szDBIP, 
        g_pSO3GameCenter->m_Settings.m_szDBName, 
        g_pSO3GameCenter->m_Settings.m_szDBAcc, 
        g_pSO3GameCenter->m_Settings.m_szDBPsw
    );
    KGLOG_PROCESS_ERROR(m_pDBHandle);

    m_bRunFlag = true;

    nRetCode = m_WorkThread.Create(WorkThreadFunction, this);
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    if (!bResult)
    {
        if (m_pDBHandle)
        {
            MySQL_Disconnect(m_pDBHandle);
            m_pDBHandle = NULL;
        }

        KG_DELETE_ARRAY(m_pszSQL);
    }
    return bResult;
}

void KMiscDB::UnInit()
{
    IKG_Buffer* piBuffer = NULL;

    m_bRunFlag = false;
    m_WorkThread.Destroy();

    while(!m_RequestQueue.empty())
    {
        piBuffer = m_RequestQueue.front();

        KMISC_DB_REQUEST* pHeader = (KMISC_DB_REQUEST*)piBuffer->GetData();

        KGLogPrintf(KGLOG_ERR, "Unprocessed DB request: %d", pHeader->nProtocolID);

        KG_COM_RELEASE(piBuffer);
        m_RequestQueue.pop_front();
    }

    while(!m_RespondQueue.empty())
    {
        piBuffer = m_RespondQueue.front();

        KMISC_DB_RESPOND* pHeader = (KMISC_DB_RESPOND*)piBuffer->GetData();

        KGLogPrintf(KGLOG_ERR, "Unprocessed DB respond: %d", pHeader->nProtocolID);

        KG_COM_RELEASE(piBuffer);
        m_RespondQueue.pop_front();
    }

    if (m_pDBHandle)
    {
        MySQL_Disconnect(m_pDBHandle);
        m_pDBHandle = NULL;
    }

    KG_DELETE_ARRAY(m_pszSQL);
}

void KMiscDB::Activate()
{
    IKG_Buffer*             piPackage   = NULL;
    BYTE*                   pbyData     = NULL;
    size_t                  uDataLen    = 0;
    KMISC_DB_RESPOND*       pHeader     = NULL;
    PROCESS_RESPOND_FUNC    ProcessFunc = NULL;

    while (true)
    {
        KG_COM_RELEASE(piPackage);

        piPackage = PopRespond();
        KG_PROCESS_ERROR(piPackage);

        pbyData = (BYTE*)piPackage->GetData();
        KGLOG_PROCESS_ERROR(pbyData);

        pHeader = (KMISC_DB_RESPOND*)pbyData;
        KGLOG_PROCESS_ERROR(pHeader);

        KGLOG_PROCESS_ERROR(pHeader->nProtocolID > emrpRespondBegin);
        KGLOG_PROCESS_ERROR(pHeader->nProtocolID < emrpRespondEnd);

        uDataLen = piPackage->GetSize();
        KGLOG_PROCESS_ERROR(uDataLen >= m_uRespondProtocolSize[pHeader->nProtocolID]);

        ProcessFunc = m_ProcessRespondFuns[pHeader->nProtocolID];
        KGLOG_PROCESS_ERROR(ProcessFunc);

        (this->*ProcessFunc)(pbyData, uDataLen);
    }

Exit0:
    KG_COM_RELEASE(piPackage);
    return;
}

BOOL KMiscDB::IsAllPackageComplete()
{
    BOOL bResult            = false;
    BOOL bRequestQueueEmpty = false;
    BOOL bRespondQueueEmpty = false;

    m_RequestQueueLock.Lock();
    bRequestQueueEmpty = m_RequestQueue.empty();
    m_RequestQueueLock.Unlock();
    KG_PROCESS_ERROR(bRequestQueueEmpty);

    m_RespondQueueLock.Lock();
    bRespondQueueEmpty = m_RespondQueue.empty();
    m_RespondQueueLock.Unlock();
    KG_PROCESS_ERROR(bRespondQueueEmpty);

    KG_PROCESS_ERROR(!m_bRequestProcessing);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KMiscDB::PushRequest(IKG_Buffer* piRequest)
{
    assert(piRequest);

    m_RequestQueueLock.Lock();

    piRequest->AddRef();

    m_RequestQueue.push_back(piRequest);

    m_RequestQueueLock.Unlock();

    return true;
}
 
IKG_Buffer* KMiscDB::PopRespond()
{
    IKG_Buffer* piBuffer = NULL;

    m_RespondQueueLock.Lock();

    if (!m_RespondQueue.empty())
    {
        piBuffer = m_RespondQueue.front();
        m_RespondQueue.pop_front();
    }

    m_RespondQueueLock.Unlock();

    return piBuffer;
}

BOOL KMiscDB::DoSaveFellowshipData(DWORD dwPlayerID, BYTE* pbyFellowData, size_t uDataLen)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    IKG_Buffer*                     piBuffer    = NULL;
    KSAVE_FELLOWSHIP_DATA_REQUEST*  pResquest   = NULL;
    
    piBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(KSAVE_FELLOWSHIP_DATA_REQUEST) + uDataLen));
    KGLOG_PROCESS_ERROR(piBuffer);

    pResquest = (KSAVE_FELLOWSHIP_DATA_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pResquest);

    pResquest->nProtocolID  = emrpSaveFellowshipRequest;
    pResquest->dwPlayerID   = dwPlayerID;
    pResquest->uDataLen     = uDataLen;

    memcpy(pResquest->byData, pbyFellowData, uDataLen);

    bRetCode = PushRequest(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KMiscDB::DoLoadFellowshipData(DWORD dwPlayerID)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    IKG_Buffer*                     piBuffer    = NULL;
    KLOAD_FELLOWSHIP_DATA_REQUEST*  pResquest   = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(KLOAD_FELLOWSHIP_DATA_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pResquest = (KLOAD_FELLOWSHIP_DATA_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pResquest);

    pResquest->nProtocolID = emrpLoadFellowshipRequest;
    pResquest->dwPlayerID  = dwPlayerID;

    bRetCode = PushRequest(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KMiscDB::DoSaveMapCopyInfo(DWORD dwMapID, int nMapCopyIndex)
{
    BOOL                            bResult                 = false;
    BOOL                            bRetCode                = false;
    size_t                          uPakSize                = 0;
    IKG_Buffer*                     piBuffer                = NULL;
    IKG_Buffer_ReSize*              piReSize                = NULL;
    KMapInfo*                       pMapInfo                = NULL;
    KMapCopy*                       pMapCopy                = NULL;
    KSAVE_MAP_COPY_INFO_REQUEST*    pRequest                = NULL;

    pMapInfo = g_pSO3GameCenter->m_MapManager.GetMapInfo(dwMapID);
    KGLOG_PROCESS_ERROR(pMapInfo);

    pMapCopy = pMapInfo->GetMapCopy(nMapCopyIndex);
    KGLOG_PROCESS_ERROR(pMapCopy);

    piBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(KSAVE_MAP_COPY_INFO_REQUEST) + MAX_MAP_COPY_DATA_LEN));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (KSAVE_MAP_COPY_INFO_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->nProtocolID      = emrpSaveMapCopyInfoRequest;
    pRequest->dwMapID          = dwMapID;
    pRequest->nMapCopyIndex    = nMapCopyIndex;
    pRequest->dwOwnerID        = pMapCopy->m_dwOwnerID;
    pRequest->nCreateTime      = pMapCopy->m_nCreateTime;
    pRequest->nLastSaveTime    = g_pSO3GameCenter->m_nTimeNow;
    
    bRetCode = pMapCopy->Save(pRequest->byData, MAX_MAP_COPY_DATA_LEN, &pRequest->uDataLen);
    KGLOG_PROCESS_ERROR(bRetCode);

    piBuffer->QueryInterface(IID_IKG_Buffer_ReSize, (void**)&piReSize);
    KGLOG_PROCESS_ERROR(piReSize);
    
    uPakSize = sizeof(KSAVE_MAP_COPY_INFO_REQUEST) + pRequest->uDataLen;

    bRetCode = piReSize->SetSmallerSize((unsigned)uPakSize);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    bRetCode = PushRequest(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    bResult = true;
Exit0:
    KG_COM_RELEASE(piReSize);
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KMiscDB::DoDeleteMapCopyInfo(DWORD dwMapID, int nMapCopyIndex)
{
    IKG_Buffer*                     piRequest   = NULL;
    KDELETE_MAP_COPY_INFO_REQUEST*  pRequest    = NULL;
    BOOL                            bRetCode    = false;
    BOOL                            bResult     = false;

    piRequest = KG_MemoryCreateBuffer(sizeof(KDELETE_MAP_COPY_INFO_REQUEST));
    KGLOG_PROCESS_ERROR(piRequest);

    pRequest = (KDELETE_MAP_COPY_INFO_REQUEST*)piRequest->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->nProtocolID   = emrpDeleteMapCopyInfoRequest;
    pRequest->dwMapID       = dwMapID;
    pRequest->nMapCopyIndex = nMapCopyIndex;

    bRetCode = PushRequest(piRequest);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piRequest);
    return bResult;
}

BOOL KMiscDB::DoUpdateRoleBlackList(DWORD dwRoleID, time_t nFreezeTime)
{
    IKG_Buffer*                     piRequest   = NULL;
    KUPDATE_ROLE_BLACK_LIST*        pRequest    = NULL;
    BOOL                            bRetCode    = false;
    BOOL                            bResult     = false;

    piRequest = KG_MemoryCreateBuffer((unsigned)sizeof(KUPDATE_ROLE_BLACK_LIST));
    KGLOG_PROCESS_ERROR(piRequest);

    pRequest = (KUPDATE_ROLE_BLACK_LIST*)piRequest->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->nProtocolID   = emrpUpdateRoleBlackList;
    pRequest->dwRoleID      = dwRoleID;
    pRequest->nFreezeTime   = nFreezeTime;

    bRetCode = PushRequest(piRequest);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piRequest);
    return bResult;
}

BOOL KMiscDB::DoUpdateRoleDeleteList(DWORD dwID, time_t nEndTime)
{
    IKG_Buffer*                 piRequest   = NULL;
    KUPDATE_ROLE_DELETE_LIST*   pRequest    = NULL;
    BOOL                        bRetCode    = false;
    BOOL                        bResult     = false;

    piRequest = KG_MemoryCreateBuffer((unsigned)sizeof(KUPDATE_ROLE_DELETE_LIST));
    KGLOG_PROCESS_ERROR(piRequest);

    pRequest = (KUPDATE_ROLE_DELETE_LIST*)piRequest->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->nProtocolID   = emrpUpdateRoleDeleteList;
    pRequest->dwID          = dwID;
    pRequest->nEndTime      = nEndTime;

    bRetCode = PushRequest(piRequest);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piRequest);
    return bResult;
}

BOOL KMiscDB::DoLoadMailBox(DWORD dwMailBoxID)
{
    BOOL                    bResult  = false;
    BOOL                    bRetCode = false;
    IKG_Buffer*             piBuffer = NULL;
    KLOAD_MAIL_BOX_REQUEST* pRequest = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(KLOAD_MAIL_BOX_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (KLOAD_MAIL_BOX_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->nProtocolID = emrpLoadMailBoxRequest;
    pRequest->dwMailBoxID = dwMailBoxID;

    bRetCode = PushRequest(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KMiscDB::DoSaveMailBox(
    DWORD dwMailBoxID, time_t nBaseTime, int nMailCount, BYTE* pbyMailBoxInfo, size_t uMailBoxLen
)
{
    BOOL                    bResult  = false;
    BOOL                    bRetCode = false;
    IKG_Buffer*             piBuffer = NULL;
    KSAVE_MAIL_BOX_REQUEST* pRequest = NULL;

    assert(pbyMailBoxInfo);
    assert(uMailBoxLen > 0);

    piBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(KSAVE_MAIL_BOX_REQUEST) + uMailBoxLen));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (KSAVE_MAIL_BOX_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->nProtocolID   = emrpSaveMailBoxRequest;
    pRequest->dwMailBoxID   = dwMailBoxID;
    pRequest->nBaseTime     = nBaseTime;
    pRequest->nMailCount    = nMailCount;
    pRequest->uMailBoxLen   = uMailBoxLen;
    memcpy(pRequest->MailBoxInfo, pbyMailBoxInfo, uMailBoxLen);

    bRetCode = PushRequest(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KMiscDB::DoCheckWithdrawMail(time_t nTimeNow, time_t nSurvivalTime)
{
    BOOL                            bResult  = false;
    BOOL                            bRetCode = false;
    IKG_Buffer*                     piBuffer = NULL;
    KCHECK_WITHDRAW_MAIL_REQUEST*   pRequest = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(KCHECK_WITHDRAW_MAIL_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (KCHECK_WITHDRAW_MAIL_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->nProtocolID   = emrpCheckWithdrawMailRequest;
    pRequest->nDeadLineTime = nTimeNow > nSurvivalTime ? nTimeNow - nSurvivalTime : 0;

    bRetCode = PushRequest(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KMiscDB::DoSaveGlobalMail(time_t nEndTime, DWORD dwMailIndex, KMail* pMail, size_t uMailLen)
{
    BOOL                            bResult      = false;
    BOOL                            bRetCode     = false;
    IKG_Buffer*                     piBuffer     = NULL;
    KSAVE_GLOBAL_MAIL_REQUEST*      pRequest     = NULL;

    assert(pMail);

    piBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(KSAVE_GLOBAL_MAIL_REQUEST) + uMailLen));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (KSAVE_GLOBAL_MAIL_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->nProtocolID   = emrpSaveGlobalMailRequest;
    pRequest->nVersion      = MAILSYS_VERSION;
    pRequest->nEndTime      = nEndTime;
    pRequest->dwMailIndex   = dwMailIndex;

    memcpy(pRequest->byData, pMail, uMailLen);

    bRetCode = PushRequest(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KMiscDB::DoDeleteGlobalMail(DWORD dwMailID)
{
    BOOL                            bResult  = false;
    BOOL                            bRetCode = false;
    IKG_Buffer*                     piBuffer = NULL;
    KDELETE_GLOBAL_MAIL_REQUEST*    pRequest = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(KDELETE_GLOBAL_MAIL_REQUEST)));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (KDELETE_GLOBAL_MAIL_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->nProtocolID   = emrpDeleteGlobalMailRequest;
    pRequest->dwMailIndex   = dwMailID;

    bRetCode = PushRequest(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KMiscDB::DoLoadPQList()
{
    BOOL                    bResult  = false;
    BOOL                    bRetCode = false;
    IKG_Buffer*             piBuffer = NULL;
    KLOAD_PQ_LIST_REQUEST*  pRequest = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(KLOAD_PQ_LIST_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (KLOAD_PQ_LIST_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->nProtocolID  = emrpLoadPQListRequest;

    bRetCode = PushRequest(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KMiscDB::DoSavePQ(const KGPQ_DB_DATA& crPQData)
{
    BOOL                bResult  = false;
    BOOL                bRetCode = false;
    IKG_Buffer*         piBuffer = NULL;
    KSAVE_PQ_REQUEST*   pRequest = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(KSAVE_PQ_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (KSAVE_PQ_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->nProtocolID   = emrpSavePQRequest;
    pRequest->PQData        = crPQData;

    bRetCode = PushRequest(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KMiscDB::DoDeletePQ(DWORD dwPQID)
{
    BOOL                bResult  = false;
    BOOL                bRetCode = false;
    IKG_Buffer*         piBuffer = NULL;
    KDELETE_PQ_REQUEST* pRequest = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(KDELETE_PQ_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (KDELETE_PQ_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->nProtocolID   = emrpDeletePQRequest;
    pRequest->dwPQID        = dwPQID;

    bRetCode = PushRequest(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KMiscDB::DoSaveTongData(DWORD dwTongID, KTong* pTong)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    size_t                      uDataUsedSize   = 0;
    HRESULT                     hRetCode        = E_FAIL;
    IKG_Buffer*                 piBuffer        = NULL;
    IKG_Buffer_ReSize*          piResize        = NULL;
    KSAVE_TONG_DATA_REQUEST*    pRequest        = NULL;

    assert(pTong);

    piBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(KSAVE_TONG_DATA_REQUEST) + MAX_TONG_DATA_LEN));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (KSAVE_TONG_DATA_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    bRetCode = pTong->Save(&uDataUsedSize, pRequest->byData, MAX_TONG_DATA_LEN);
    KGLOG_PROCESS_ERROR(bRetCode);

    pRequest->nProtocolID   = emrSaveTongDataRequest;
    pRequest->dwTongID      = dwTongID;
    pRequest->uDataLen      = uDataUsedSize;

    hRetCode = piBuffer->QueryInterface(IID_IKG_Buffer_ReSize, (void**)&piResize);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    bRetCode = piResize->SetSmallerSize((unsigned)(sizeof(KSAVE_TONG_DATA_REQUEST) + pRequest->uDataLen));
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = PushRequest(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piResize);
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KMiscDB::DoRemoveTongData(DWORD dwTongID)
{
    BOOL                        bResult     = false;
    BOOL                        bRetCode    = false;
    IKG_Buffer*                 piBuffer    = NULL;
    KREMOVE_TONG_DATA_REQUEST*  pRequest    = NULL;

    piBuffer = KG_MemoryCreateBuffer(sizeof(KREMOVE_TONG_DATA_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (KREMOVE_TONG_DATA_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->nProtocolID   = emrRemoveTongDataRequest;
    pRequest->dwTongID      = dwTongID;

    bRetCode = PushRequest(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KMiscDB::DoAuctionLookup(BYTE byRequestID, DWORD dwPlayerID, KAUCTION_LOOKUP_PARAM* pParam)
{
    BOOL                        bResult  = false;
    BOOL                        bRetCode = false;
    IKG_Buffer*                 piBuffer = NULL;
    KAUCTION_LOOKUP_DB_REQUEST* pRequest = NULL;

    assert(pParam);

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(KAUCTION_LOOKUP_DB_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (KAUCTION_LOOKUP_DB_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->nProtocolID   = emrpAuctionLookupRequest;
    pRequest->byRequestID   = byRequestID;
    pRequest->dwPlayerID    = dwPlayerID;
    pRequest->Param         = *pParam;

    bRetCode = PushRequest(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KMiscDB::DoAuctionBid(DWORD dwBidderID, DWORD dwSaleID, DWORD dwCRC, int nPrice)
{
    BOOL                        bResult  = false;
    BOOL                        bRetCode = false;
    IKG_Buffer*                 piBuffer = NULL;
    KAUCTION_BID_DB_REQUEST*    pRequest = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(KAUCTION_BID_DB_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (KAUCTION_BID_DB_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->nProtocolID   = emrpAuctionBidRequest;
    pRequest->dwBidderID    = dwBidderID;
    pRequest->dwSaleID      = dwSaleID;
    pRequest->dwCRC         = dwCRC;
    pRequest->nPrice        = nPrice;

    bRetCode = PushRequest(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KMiscDB::DoAuctionSell(
    DWORD dwSellerID, char* pszSaleName, char* pszSellerName, int nAucGenre, int nAucSub, int nRequireLevel,
    int nQuality, int nStartPrice, int nBuyItNowPrice, int nCustodyCharges, time_t nDurationTime,
    BYTE* pbyItemData, size_t uDataLen
)
{
    BOOL                        bResult  = false;
    BOOL                        bRetCode = false;
    IKG_Buffer*                 piBuffer = NULL;
    KAUCTION_SELL_DB_REQUEST*   pRequest = NULL;

    assert(dwSellerID);
    assert(pszSaleName);
    assert(pszSellerName);

    piBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(KAUCTION_SELL_DB_REQUEST) + uDataLen));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (KAUCTION_SELL_DB_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->nProtocolID       = emrpAuctionSellRequest;
    pRequest->dwSellerID        = dwSellerID;

    strncpy(pRequest->szSaleName, pszSaleName, sizeof(pRequest->szSaleName));
    pRequest->szSaleName[sizeof(pRequest->szSaleName) - 1] = '\0';

    strncpy(pRequest->szSellerName, pszSellerName, sizeof(pRequest->szSellerName));
    pRequest->szSellerName[sizeof(pRequest->szSellerName) - 1] = '\0';

    pRequest->nAucGenre         = nAucGenre;
    pRequest->nAucSub           = nAucSub;
    pRequest->nRequireLevel     = nRequireLevel;
    pRequest->nQuality          = nQuality;
    pRequest->nStartPrice       = nStartPrice;
    pRequest->nBuyItNowPrice    = nBuyItNowPrice;
    pRequest->nCustodyCharges   = nCustodyCharges;
    pRequest->nDurationTime     = nDurationTime;
    pRequest->uItemDataLen      = uDataLen;

    if (pbyItemData)
    {
        memcpy(pRequest->byItemData, pbyItemData, uDataLen);
    }
                    
    bRetCode = PushRequest(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KMiscDB::DoAuctionCancel(DWORD dwSellerID, DWORD dwSaleID)
{
    BOOL                        bResult  = false;
    BOOL                        bRetCode = false;
    IKG_Buffer*                 piBuffer = NULL;
    KAUCTION_CANCEL_DB_REQUEST* pRequest = NULL;

    assert(dwSellerID);

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(KAUCTION_CANCEL_DB_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (KAUCTION_CANCEL_DB_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->nProtocolID   = emrpAuctionCancelRequest;
    pRequest->dwSellerID    = dwSellerID;
    pRequest->dwSaleID      = dwSaleID;

    bRetCode = PushRequest(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KMiscDB::DoAuctionCheckDuration(time_t nTimeNow)
{
    BOOL                                bResult  = false;
    BOOL                                bRetCode = false;
    IKG_Buffer*                         piBuffer = NULL;
    KAUCTION_CHECK_DURATION_REQUEST*    pRequest = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(KAUCTION_CHECK_DURATION_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (KAUCTION_CHECK_DURATION_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->nProtocolID   = emrpAuctionCheckDurationRequest;
    pRequest->nTimeNow      = nTimeNow;

    bRetCode = PushRequest(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KMiscDB::DoSaveGlobalCustomData(const char cszName[], BYTE* pbyData, size_t uDataLen)
{
    BOOL                                bResult  = false;
    BOOL                                bRetCode = false;
    IKG_Buffer*                         piBuffer = NULL;
    KSAVE_GLOBAL_CUSTOM_DATA_REQUEST*   pRequest = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(KSAVE_GLOBAL_CUSTOM_DATA_REQUEST) + uDataLen);
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (KSAVE_GLOBAL_CUSTOM_DATA_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->nProtocolID   = emrpSaveGlobalCustomData;

    strncpy(pRequest->szName, cszName, sizeof(pRequest->szName));
    pRequest->szName[sizeof(pRequest->szName) - 1] = '\0';

    memcpy(pRequest->byData, pbyData, uDataLen);

    bRetCode = PushRequest(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KMiscDB::DoDeleteGlobalCustomData(const char cszName[])
{
    BOOL                                bResult  = false;
    BOOL                                bRetCode = false;
    IKG_Buffer*                         piBuffer = NULL;
    KDELETE_GLOBAL_CUSTOM_DATA_REQUEST* pRequest = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(KDELETE_GLOBAL_CUSTOM_DATA_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (KDELETE_GLOBAL_CUSTOM_DATA_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->nProtocolID   = emrpDeleteGlobalCustomData;

    strncpy(pRequest->szName, cszName, sizeof(pRequest->szName));
    pRequest->szName[sizeof(pRequest->szName) - 1] = '\0';

    bRetCode = PushRequest(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KMiscDB::DoSaveGlobalSystemValue(const char* pszName, int nValue)
{
    BOOL                                bResult  = false;
    BOOL                                bRetCode = false;
    IKG_Buffer*                         piBuffer = NULL;
    KSAVE_GLOBAL_SYSTEM_VALUE_REQUEST*  pRequest = NULL;

    assert(pszName);

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(KSAVE_GLOBAL_SYSTEM_VALUE_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (KSAVE_GLOBAL_SYSTEM_VALUE_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->nProtocolID   = emrpSaveGlobalSystemValue;
    pRequest->nValue        = nValue;

    strncpy(pRequest->szName, pszName, sizeof(pRequest->szName));
    pRequest->szName[sizeof(pRequest->szName) - 1] = '\0';

    bRetCode = PushRequest(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KMiscDB::DoSaveStatData(time_t nStatTime, int nDataCount, int64_t* pnData)
{
    BOOL                        bResult     = false;
    BOOL                        bRetCode    = false;
    IKG_Buffer*                 piBuffer    = NULL;
    KSAVE_STAT_DATA_REQUEST*    pRequest    = NULL;
    KStatData_DB*               pStatDB     = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(KSAVE_STAT_DATA_REQUEST) + sizeof(KStatData_DB) + sizeof(int64_t) * nDataCount);
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (KSAVE_STAT_DATA_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->nProtocolID = emrpSaveStatData;
    pRequest->nStatTime   = nStatTime;

    pStatDB = (KStatData_DB*)pRequest->byDataDB;

    pStatDB->nDataCount = nDataCount;
    for (int i = 0; i < nDataCount; i++)
        pStatDB->nData[i] = pnData[i];

    bRetCode = PushRequest(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KMiscDB::DoSaveStatDataName(char* pszName, int nNameID)
{
    BOOL                            bResult  = false;
    BOOL                            bRetCode = false;
    IKG_Buffer*                     piBuffer = NULL;
    KSAVE_STAT_DATA_NAME_REQUEST*   pRequest = NULL;

    KGLOG_PROCESS_ERROR(pszName);

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(KSAVE_STAT_DATA_NAME_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (KSAVE_STAT_DATA_NAME_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->nProtocolID = emrpSaveStatDataName;
    pRequest->nNameID     = nNameID;

    strncpy(pRequest->szName, pszName, sizeof(pRequest->szName));
    pRequest->szName[sizeof(pRequest->szName) - 1] = '\0';

    bRetCode = PushRequest(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KMiscDB::DoUpdateActivityData(const KActivityRecord& rRecord)
{
    BOOL                            bResult  = false;
    BOOL                            bRetCode = false;
    IKG_Buffer*                     piBuffer = NULL;
    KUPDATE_ACTIVITY_DATA_REQUEST*  pRequest = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(KUPDATE_ACTIVITY_DATA_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (KUPDATE_ACTIVITY_DATA_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->nProtocolID   = emrpUpdateActivityData;
    pRequest->Data          = rRecord;

    bRetCode = PushRequest(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KMiscDB::DoDeleteActivityData(DWORD dwRoleID, int nType)
{
    BOOL                            bResult  = false;
    BOOL                            bRetCode = false;
    IKG_Buffer*                     piBuffer = NULL;
    KDELETE_ACTIVITY_DATE_REQUEST*  pRequest = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(KDELETE_ACTIVITY_DATE_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (KDELETE_ACTIVITY_DATE_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->nProtocolID   = emrpDeleteActivityData;
    pRequest->dwRoleID      = dwRoleID;
    pRequest->nType         = nType;

    bRetCode = PushRequest(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KMiscDB::DoGameCardSellRequest(
    DWORD dwSellerID, int nCoin, int nGameTime, BYTE byType, int nPrice, time_t nEndTime
)
{
    BOOL                        bResult  = false;
    BOOL                        bRetCode = false;
    IKG_Buffer*                 piBuffer = NULL;
    KGAME_CARD_SELL_REQUEST*    pRequest = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(KGAME_CARD_SELL_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (KGAME_CARD_SELL_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->nProtocolID   = emrpGameCardSellRequest;
    pRequest->dwSellerID    = dwSellerID;
    pRequest->nCoin         = nCoin;
    pRequest->nGameTime     = nGameTime;
    pRequest->byType        = byType;
    pRequest->nPrice        = nPrice;
    pRequest->nEndTime      = nEndTime;

    bRetCode = PushRequest(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KMiscDB::DoGameCardBuyRequest(DWORD dwID, DWORD dwBuyerID, int nPrice)
{
    BOOL                    bResult  = false;
    BOOL                    bRetCode = false;
    IKG_Buffer*             piBuffer = NULL;
    KGAME_CARD_BUY_REQUEST* pRequest = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(KGAME_CARD_BUY_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (KGAME_CARD_BUY_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->nProtocolID   = emrpGameCardBuyRequest;
    pRequest->dwID          = dwID;
    pRequest->dwBuyerID     = dwBuyerID;
    pRequest->nPrice        = nPrice;

    bRetCode = PushRequest(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KMiscDB::DoGameCardLookupRequest(DWORD dwPlayerID, BYTE byCardType, int nStartIndex, BYTE byOrderType, BOOL bDesc)
{
    BOOL                        bResult  = false;
    BOOL                        bRetCode = false;
    IKG_Buffer*                 piBuffer = NULL;
    KGAME_CARD_LOOKUP_REQUEST*  pRequest = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(KGAME_CARD_LOOKUP_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (KGAME_CARD_LOOKUP_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->nProtocolID   = emrpGameCardLookupRequest;
    pRequest->dwPlayerID    = dwPlayerID;
    pRequest->byCardType    = byCardType;
    pRequest->nStartIndex   = nStartIndex;
    pRequest->byOrderType   = byOrderType;
    pRequest->bDesc         = bDesc;

    bRetCode = PushRequest(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KMiscDB::DoGameCardCancelRequest(DWORD dwPlayerID, DWORD dwID)
{
    BOOL                        bResult  = false;
    BOOL                        bRetCode = false;
    IKG_Buffer*                 piBuffer = NULL;
    KGAME_CARD_CANCEL_REQUEST*  pRequest = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(KGAME_CARD_CANCEL_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (KGAME_CARD_CANCEL_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->nProtocolID   = emrpGameCardCancelRequest;
    pRequest->dwPlayerID    = dwPlayerID;
    pRequest->dwID          = dwID;

    bRetCode = PushRequest(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KMiscDB::DoGameCardCheckDuration(time_t nTimeNow)
{
    BOOL                                bResult  = false;
    BOOL                                bRetCode = false;
    IKG_Buffer*                         piBuffer = NULL;
    KGAME_CARD_CHECK_DURATION_REQUEST*  pRequest = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(KGAME_CARD_CHECK_DURATION_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (KGAME_CARD_CHECK_DURATION_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->nProtocolID   = emrpGameCardCheckDurationRequest;
    pRequest->nTimeNow      = nTimeNow;

    bRetCode = PushRequest(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KMiscDB::DoAddFarmerRecordRequest(const char cszAccount[], time_t nPunishEndTime)
{
    BOOL                        bResult     = false;
    BOOL                        bRetCode    = false;
    IKG_Buffer*                 piBuffer    = NULL;
    KADD_FARMER_RECORD_REQUEST* pRequest    = NULL;
    size_t                      uStrLen     = 0;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(KADD_FARMER_RECORD_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (KADD_FARMER_RECORD_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->nProtocolID = emrpAddFarmerRecordRequest;
    
    uStrLen = strlen(cszAccount);
    KGLOG_PROCESS_ERROR(uStrLen < sizeof(pRequest->szAccount));
    strcpy(pRequest->szAccount, cszAccount);

    pRequest->nPunishEndTime    = nPunishEndTime;
    
    bRetCode = PushRequest(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KMiscDB::DoDeleteFarmerRecordRequest(const char cszAccount[])
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    IKG_Buffer*                     piBuffer    = NULL;
    KDELETE_FARMER_RECORD_REQUEST*  pRequest    = NULL;
    size_t                          uStrLen     = 0;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(KDELETE_FARMER_RECORD_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (KDELETE_FARMER_RECORD_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->nProtocolID = emrpDeleteFarmerRecordRequest;

    uStrLen = strlen(cszAccount);
    KGLOG_PROCESS_ERROR(uStrLen < sizeof(pRequest->szAccount));
    strcpy(pRequest->szAccount, cszAccount);

    bRetCode = PushRequest(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KMiscDB::DoSaveMentorRecordRequest(DWORD dwMentorID, DWORD dwApprenticeID, BYTE* pbyMentorData, size_t uDataLen)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    IKG_Buffer*                     piBuffer    = NULL;
    KSAVE_MENTOR_RECORD_REQUEST*    pRequest    = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(KSAVE_MENTOR_RECORD_REQUEST) + uDataLen));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (KSAVE_MENTOR_RECORD_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->nProtocolID    = emrpSaveMentorRecordRequest;
    pRequest->dwMentorID     = dwMentorID;
    pRequest->dwApprenticeID = dwApprenticeID;
    pRequest->uDataLen       = uDataLen;

    memcpy(pRequest->byMentorData, pbyMentorData, uDataLen);

    bRetCode = PushRequest(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KMiscDB::DoDeleteMentorRecordRequest(DWORD dwMentorID, DWORD dwApprenticeID)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    IKG_Buffer*                     piBuffer    = NULL;
    KDELETE_MENTOR_RECORD_REQUEST*  pRequest    = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)sizeof(KDELETE_MENTOR_RECORD_REQUEST));
    KGLOG_PROCESS_ERROR(piBuffer);

    pRequest = (KDELETE_MENTOR_RECORD_REQUEST*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pRequest);

    pRequest->nProtocolID    = emrpDeleteMentorRecordRequest;
    pRequest->dwMentorID     = dwMentorID;
    pRequest->dwApprenticeID = dwApprenticeID;

    bRetCode = PushRequest(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

void KMiscDB::OnLoadFellowshipData(BYTE* pbyData, size_t uDataLen)
{
    KLOAD_FELLOWSHIP_DATA_RESPOND* pRespond = (KLOAD_FELLOWSHIP_DATA_RESPOND*) pbyData;

    g_pSO3GameCenter->m_FellowshipCenter.OnLoadFellowshipData(
        pRespond->dwPlayerID,
        pRespond->byData,
        uDataLen - sizeof(KLOAD_FELLOWSHIP_DATA_RESPOND)
    );
// Exit0:
    return;
}

void KMiscDB::OnLoadMailBox(BYTE* pbyData, size_t uDataLen)
{
    KLOAD_MAIL_BOX_RESPOND* pRespond = (KLOAD_MAIL_BOX_RESPOND*)pbyData;

    g_pSO3GameCenter->m_MailManager.OnLoadMailBoxRespond(
        pRespond->bSucceed, pRespond->dwMailBoxID, pRespond->nBaseTime,
        pRespond->MailBoxInfo, pRespond->uMailBoxLen
    );
}

void KMiscDB::OnCheckWithdrawMail(BYTE* pbyData, size_t uDataLen)
{
    KCHECK_WITHDRAW_MAIL_RESPOND* pRespond = (KCHECK_WITHDRAW_MAIL_RESPOND*)pbyData;

    g_pSO3GameCenter->m_MailManager.OnCheckWithdrawMailRespond(
        pRespond->nMailBoxCount, pRespond->dwMailBoxIDList
    );
}

void KMiscDB::OnLoadPQList(BYTE* pbyData, size_t uDataLen)
{
    KLOAD_PQ_LIST_RESPOND* pRespond = (KLOAD_PQ_LIST_RESPOND*)pbyData;

    for (int i = 0; i < pRespond->nCount; i++)
    {
        BOOL                bRetCode = false;
        const KGPQ_DB_DATA& crDBData = pRespond->PQData[i];
        KGPQTemplateInfo*   pPQTemplate = g_pSO3GameCenter->m_PQCenter.GetPQTemplate(crDBData.dwPQTemplateID);
        KGPQ                PQ;

        if (!pPQTemplate)
        {
            KGLogPrintf(KGLOG_ERR, 
                "[PQ] Load PQ from DB failed! PQTemplate not found, PQID: %lu, PQTemplateID: %lu.\n", 
                crDBData.dwPQID, crDBData.dwPQTemplateID
            );
            continue;
        }

        PQ.m_dwPQID         = crDBData.dwPQID;
        PQ.m_pPQTemplate    = pPQTemplate;

        PQ.m_nBeginTime     = crDBData.nBeginTime;
        PQ.m_nExpiration    = PQ.m_nBeginTime + PQ.m_pPQTemplate->nExpiration;

        if (PQ.m_pPQTemplate->nEndTime)
            PQ.m_nEndTime       = PQ.m_nBeginTime + PQ.m_pPQTemplate->nEndTime;

        memcpy(&PQ.m_nPQValue, &crDBData.nPQValue, sizeof(PQ.m_nPQValue));
        PQ.m_bFinished      = crDBData.bFinished;

        PQ.m_eOwnerType     = (KGPQ_OWNER_TYPE)crDBData.byOnwerType;
        PQ.m_dwOwnerID1     = crDBData.dwOnwerID1;
        PQ.m_dwOwnerID2     = crDBData.dwOnwerID2;
        PQ.m_dwOwnerID3     = crDBData.dwOnwerID3;

        bRetCode = g_pSO3GameCenter->m_PQCenter.LoadPQ(PQ);
        if (!bRetCode)
        {
            KGLogPrintf(KGLOG_ERR, 
                "[PQ] Load PQ from DB failed! PQID: %lu, PQTemplateID: %lu.\n", 
                crDBData.dwPQID, crDBData.dwPQTemplateID
            );
            continue;
        }
    }

    g_pSO3GameCenter->m_PQCenter.m_bLoadCompleted = true;
    g_pSO3GameCenter->CallMainScript("OnPQLoadComplete");
}

void KMiscDB::OnAuctionLookup(BYTE* pbyData, size_t uDataLen)
{
    KAUCTION_LOOKUP_DB_RESPOND* pRespond = (KAUCTION_LOOKUP_DB_RESPOND*)pbyData;

    g_pSO3GameCenter->m_AuctionManager.OnLookupRespond(pRespond->byRespondID, pRespond->dwPlayerID, &pRespond->Page);
}

void KMiscDB::OnAuctionBid(BYTE* pbyData, size_t uDataLen)
{
    KAUCTION_BID_DB_RESPOND*    pRespond = (KAUCTION_BID_DB_RESPOND*)pbyData;

    g_pSO3GameCenter->m_AuctionManager.OnBidRespond(
        pRespond->byCode, pRespond->dwSellerID, pRespond->nSellPrice, pRespond->dwBidderID,
        pRespond->nBidPrice, pRespond->dwOldBidderID, pRespond->nOldBidPrice,pRespond->szSaleName,
        pRespond->nCustodyCharges, pRespond->uItemDataLen, pRespond->byItemData
    );
}

void KMiscDB::OnAuctionSell(BYTE* pbyData, size_t uDataLen)
{
    KAUCTION_SELL_DB_RESPOND* pRespond = (KAUCTION_SELL_DB_RESPOND*)pbyData;

    g_pSO3GameCenter->m_AuctionManager.OnSellRespond(pRespond->byCode);
}

void KMiscDB::OnAuctionCancel(BYTE* pbyData, size_t uDataLen)
{
    KAUCTION_CANCEL_DB_RESPOND* pRespond = (KAUCTION_CANCEL_DB_RESPOND*)pbyData;

    g_pSO3GameCenter->m_AuctionManager.OnCancelRespond(
        pRespond->byCode, pRespond->dwSellerID, pRespond->szSaleName, pRespond->uItemDataLen, pRespond->byItemData
    );
}

void KMiscDB::OnAuctionCheckDuration(BYTE* pbyData, size_t uDataLen)
{
    KAUCTION_CHECK_DURATION_RESPOND* pRespond = (KAUCTION_CHECK_DURATION_RESPOND*)pbyData;
    
    g_pSO3GameCenter->m_AuctionManager.OnCheckDurationRespond(
        pRespond->szSaleName, pRespond->dwSellerID, pRespond->dwBidderID, pRespond->nPrice, pRespond->nBuyItNowPrice,
        pRespond->nCustodyCharges, pRespond->uItemDataLen, pRespond->byItemData
    );
}

void KMiscDB::OnGameCardBuyRespond(BYTE* pbyData, size_t uDataLen)
{
    KGAME_CARD_BUY_RESPOND* pRespond = (KGAME_CARD_BUY_RESPOND*)pbyData;

    if (!g_pSO3GameCenter->m_Gateway.IsAvailable())
    {
        g_pSO3GameCenter->m_GameServer.DoGameCardBuyRespond(pRespond->dwBuyerID, gorSystemError, pRespond->nPrice);
        goto Exit0;
    }

    g_pSO3GameCenter->m_GameCardManager.OnDBBuyRespond(
        pRespond->byCode, pRespond->dwSellerID, pRespond->nCoin, pRespond->nGameTime,
        pRespond->byType, pRespond->nPrice, pRespond->dwBuyerID
    );

Exit0:
    return;
}

void KMiscDB::OnGameCardLookupRespond(BYTE* pbyData, size_t uDataLen)
{
    KGAME_CARD_LOOKUP_RESPOND* pRespond = (KGAME_CARD_LOOKUP_RESPOND*)pbyData;

    g_pSO3GameCenter->m_GameCardManager.OnDBLookupRespond(
        pRespond->dwPlayerID, pRespond->nTotalCount, pRespond->nCount, pRespond->CardList
    );
}

void KMiscDB::OnGameCardCancelRespond(BYTE* pbyData, size_t uDataLen)
{
    KGAME_CARD_CANCEL_RESPOND*  pRespond    = (KGAME_CARD_CANCEL_RESPOND*)pbyData;
    KRole*                      pRole       = NULL;

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pRespond->dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);

    g_pSO3GameCenter->m_Gateway.DoFreezeCoinRequest(pRole->m_dwPlayerID, pRole->m_pszAccount, false, pRespond->nCoin);

Exit0:
    return;
}

void KMiscDB::OnGameCardCheckDurationRespond(BYTE* pbyData, size_t uDataLen)
{
    KGAME_CARD_CHECK_DURATION_RESPOND*  pRespond    = (KGAME_CARD_CHECK_DURATION_RESPOND*)pbyData;
    KRole*                              pRole       = NULL;

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pRespond->dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);

    g_pSO3GameCenter->m_Gateway.DoFreezeCoinRequest(pRole->m_dwPlayerID, pRole->m_pszAccount, false, pRespond->nCoin);

Exit0:
    return;
}
