#ifndef _KMISCDB_H_ 
#define _KMISCDB_H_ 

#include <deque>
#include "KDBBase.h"
#include "KLRUCacheMap.h"
#include "KAuctionManager.h"
#include "KStatDataDef.h"
#include "KActivityManager.h"
#include "KGameCardDef.h"
#include "KMailDef.h"

#define PQ_PARAM_DB_COUNT           8

struct KTong;

// ========================== Protocol define ====================================>

enum  MISCDB_REQUEST_PROTOCOL
{
    emrpRequestBegin = 0,

    emrpSaveFellowshipRequest,
    emrpLoadFellowshipRequest,
    emrpSaveMapCopyInfoRequest, 
    emrpDeleteMapCopyInfoRequest,
    emrpUpdateRoleBlackList,
    emrpUpdateRoleDeleteList,

    emrpLoadMailBoxRequest,
    emrpSaveMailBoxRequest,
    emrpSaveGlobalMailRequest,
    emrpDeleteGlobalMailRequest,
    emrpCheckWithdrawMailRequest,
    emrpLoadPQListRequest,
    emrpSavePQRequest,
    emrpDeletePQRequest,

    emrSaveTongDataRequest,
    emrRemoveTongDataRequest,

    emrpAuctionLookupRequest,
    emrpAuctionBidRequest,
    emrpAuctionSellRequest,
    emrpAuctionCancelRequest,
    emrpAuctionCheckDurationRequest,
    
    emrpSaveGlobalCustomData,
    emrpDeleteGlobalCustomData,

    emrpSaveGlobalSystemValue,

    emrpSaveStatData,
    emrpSaveStatDataName,

    emrpUpdateActivityData,
    emrpDeleteActivityData,

    emrpGameCardSellRequest,
    emrpGameCardBuyRequest,
    emrpGameCardLookupRequest,
    emrpGameCardCancelRequest,
    emrpGameCardCheckDurationRequest,

    emrpAddFarmerRecordRequest,
    emrpDeleteFarmerRecordRequest,

    emrpSaveMentorRecordRequest,
    emrpDeleteMentorRecordRequest,

    emrpRequestEnd
};

enum MISCDB_RESPOND_PROTOCOL
{
    emrpRespondBegin = 0,

    emrpLoadFellowshipRespond,
    emrpLoadMailBoxRespond,
    emrpCheckWithdrawMailRespond,
    emrpLoadPQListRespond,

    emrpAuctionLookupRespond,
    emrpAuctionBidRespond,
    emrpAuctionSellRespond,
    emrpAuctionCancelRespond,
    emrpAuctionCheckDurationRespond,

    emrpGameCardBuyRespond,
    emrpGameCardLookupRespond,
    emrpGameCardCancelRespond,
    emrpGameCardCheckDurationRespond,

    emrpRespondEnd
};

typedef struct  
{
    int     nProtocolID;
} KMISC_DB_REQUEST;

typedef struct  
{
    int     nProtocolID;
} KMISC_DB_RESPOND;


#pragma pack(1)
struct KGPQ_DB_DATA 
{
    BYTE    byVersion;
    DWORD   dwPQID;
    DWORD   dwPQTemplateID;
    time_t  nBeginTime;
    int     nPQValue[PQ_PARAM_DB_COUNT];
    BYTE    bFinished;
    BYTE    byOnwerType;
    DWORD   dwOnwerID1;
    DWORD   dwOnwerID2;
    DWORD   dwOnwerID3;
};
#pragma pack()

// REQUEST

//--------------Fellowship-------------------------

struct KSAVE_FELLOWSHIP_DATA_REQUEST : KMISC_DB_REQUEST
{
    DWORD       dwPlayerID;
    size_t      uDataLen;
    BYTE        byData[0];
};

struct KLOAD_FELLOWSHIP_DATA_REQUEST : KMISC_DB_REQUEST
{
    DWORD       dwPlayerID;
};

// -------------------MapCopyInfo----------------------

struct KSAVE_MAP_COPY_INFO_REQUEST : KMISC_DB_REQUEST
{
    DWORD       dwMapID;
    int         nMapCopyIndex;
    DWORD       dwOwnerID;
    time_t      nCreateTime;
    time_t      nLastSaveTime;
    size_t      uDataLen;
    BYTE        byData[0];
};

struct KDELETE_MAP_COPY_INFO_REQUEST : KMISC_DB_REQUEST
{
    DWORD       dwMapID;
    int         nMapCopyIndex;
};

// ----------------------Role--------------------------
struct KUPDATE_ROLE_BLACK_LIST : KMISC_DB_REQUEST
{
    DWORD   dwRoleID;
    time_t  nFreezeTime;
};

struct KUPDATE_ROLE_DELETE_LIST : KMISC_DB_REQUEST 
{
    DWORD   dwID;
    time_t  nEndTime;
};

// ----------------------Mail--------------------------
struct KLOAD_MAIL_BOX_REQUEST : KMISC_DB_REQUEST
{
    DWORD   dwMailBoxID;
};

struct KSAVE_MAIL_BOX_REQUEST : KMISC_DB_REQUEST 
{
    DWORD   dwMailBoxID;
    time_t  nBaseTime;
    int     nMailCount;
    size_t  uMailBoxLen;
    BYTE    MailBoxInfo[0];
};

struct KSAVE_GLOBAL_MAIL_REQUEST : KMISC_DB_REQUEST
{
    int     nVersion;
    time_t  nEndTime;
    DWORD   dwMailIndex;
    BYTE    byData[0]; // KMail
};

struct KDELETE_GLOBAL_MAIL_REQUEST : KMISC_DB_REQUEST
{
    DWORD   dwMailIndex;
};

struct KCHECK_WITHDRAW_MAIL_REQUEST : KMISC_DB_REQUEST
{
    time_t  nDeadLineTime;
};

struct KLOAD_PQ_LIST_REQUEST : KMISC_DB_REQUEST
{
};

struct KSAVE_PQ_REQUEST : KMISC_DB_REQUEST 
{
    KGPQ_DB_DATA    PQData;
};

struct KDELETE_PQ_REQUEST : KMISC_DB_REQUEST 
{
    DWORD           dwPQID;
};

struct KAUCTION_LOOKUP_DB_REQUEST : KMISC_DB_REQUEST 
{
    BYTE                    byRequestID;
    DWORD                   dwPlayerID;
    KAUCTION_LOOKUP_PARAM   Param;
};

struct KAUCTION_BID_DB_REQUEST : KMISC_DB_REQUEST 
{
    DWORD   dwBidderID;
    DWORD   dwSaleID;
    DWORD   dwCRC;
    int     nPrice;
};

struct KAUCTION_SELL_DB_REQUEST : KMISC_DB_REQUEST
{
    DWORD   dwSellerID;
    char    szSaleName[SALE_NAME_LEN];
    char    szSellerName[ROLE_NAME_LEN];
    int     nAucGenre;
    int     nAucSub;
    int     nRequireLevel;
    int     nQuality;
    int     nStartPrice;
    int     nBuyItNowPrice;
    int     nCustodyCharges;
    time_t  nDurationTime;
    size_t  uItemDataLen;
    BYTE    byItemData[0];
};

struct KAUCTION_CANCEL_DB_REQUEST : KMISC_DB_REQUEST 
{
    DWORD   dwSellerID;
    DWORD   dwSaleID;
};

struct KAUCTION_CHECK_DURATION_REQUEST : KMISC_DB_REQUEST 
{
    time_t  nTimeNow;
};

// 全局自定义数据
struct KSAVE_GLOBAL_CUSTOM_DATA_REQUEST : KMISC_DB_REQUEST
{
    char    szName[32];
    BYTE    byData[0];
};

struct KDELETE_GLOBAL_CUSTOM_DATA_REQUEST : KMISC_DB_REQUEST
{
    char    szName[32];
};

struct KSAVE_GLOBAL_SYSTEM_VALUE_REQUEST : KMISC_DB_REQUEST 
{
    char    szName[32];
    int     nValue;
};

// 统计数据
struct KSAVE_STAT_DATA_REQUEST : KMISC_DB_REQUEST 
{
    time_t  nStatTime;
    BYTE    byDataDB[0];
};

struct KSAVE_STAT_DATA_NAME_REQUEST : KMISC_DB_REQUEST
{
    char    szName[STAT_DATA_NAME_LEN];
    int     nNameID;
};

// 活动名单
struct KUPDATE_ACTIVITY_DATA_REQUEST : KMISC_DB_REQUEST
{
    KActivityRecord Data;
};

struct KDELETE_ACTIVITY_DATE_REQUEST : KMISC_DB_REQUEST 
{
    DWORD   dwRoleID;
    int     nType;
};

struct KGAME_CARD_SELL_REQUEST : KMISC_DB_REQUEST 
{
    DWORD   dwSellerID;
    int     nCoin;
    int     nGameTime;
    BYTE    byType;
    int     nPrice;
    time_t  nEndTime;
};

struct KGAME_CARD_BUY_REQUEST : KMISC_DB_REQUEST
{
    DWORD   dwID;
    DWORD   dwBuyerID;
    int     nPrice;
};

struct KGAME_CARD_LOOKUP_REQUEST : KMISC_DB_REQUEST 
{
    DWORD   dwPlayerID;
    BYTE    byCardType;
    int     nStartIndex;
    BYTE    byOrderType;
    BOOL    bDesc;
};

struct KGAME_CARD_CANCEL_REQUEST : KMISC_DB_REQUEST 
{
    DWORD   dwPlayerID;
    DWORD   dwID;
};

struct KGAME_CARD_CHECK_DURATION_REQUEST : KMISC_DB_REQUEST 
{
    time_t  nTimeNow;
};

struct KADD_FARMER_RECORD_REQUEST: KMISC_DB_REQUEST
{
    char    szAccount[_NAME_LEN];
    time_t  nPunishEndTime;
};

struct KDELETE_FARMER_RECORD_REQUEST : KMISC_DB_REQUEST
{
    char szAccount[_NAME_LEN];
};

//师徒
struct KSAVE_MENTOR_RECORD_REQUEST : KMISC_DB_REQUEST
{
    DWORD   dwMentorID;
    DWORD   dwApprenticeID;
    size_t  uDataLen;
    BYTE    byMentorData[];
};

struct KDELETE_MENTOR_RECORD_REQUEST : KMISC_DB_REQUEST
{
    DWORD   dwMentorID;
    DWORD   dwApprenticeID;
};

// RESPOND 

//--------------Fellowship-------------------------
struct KLOAD_FELLOWSHIP_DATA_RESPOND : KMISC_DB_RESPOND
{
    DWORD       dwPlayerID;
    BYTE        byData[0];
};

struct KLOAD_MAIL_BOX_RESPOND : KMISC_DB_RESPOND
{
    DWORD   dwMailBoxID;
    BOOL    bSucceed;
    time_t  nBaseTime;
    size_t  uMailBoxLen;
    BYTE    MailBoxInfo[0];
};

struct KCHECK_WITHDRAW_MAIL_RESPOND : KMISC_DB_RESPOND 
{
    int     nMailBoxCount;
    DWORD   dwMailBoxIDList[0];
};

struct KLOAD_PQ_LIST_RESPOND : KMISC_DB_RESPOND 
{
    int             nCount;
    KGPQ_DB_DATA    PQData[0];
};


// -------------------   Tong  ---------------------
struct KSAVE_TONG_DATA_REQUEST : KMISC_DB_REQUEST
{
    DWORD   dwTongID;
    size_t  uDataLen;
    BYTE    byData[0];
};

struct KREMOVE_TONG_DATA_REQUEST : KMISC_DB_REQUEST
{
    DWORD   dwTongID;
};

struct KAUCTION_LOOKUP_DB_RESPOND : KMISC_DB_RESPOND 
{
    BYTE                    byRespondID;
    DWORD                   dwPlayerID;
    KAUCTION_PAGE_HEAD      Page;
};

struct KAUCTION_BID_DB_RESPOND : KMISC_DB_RESPOND 
{
    BYTE    byCode;
    DWORD   dwSellerID;
    int     nSellPrice;
    DWORD   dwBidderID;
    int     nBidPrice;
    DWORD   dwOldBidderID;
    int     nOldBidPrice;
    char    szSaleName[SALE_NAME_LEN];
    int     nCustodyCharges;
    size_t  uItemDataLen;
    BYTE    byItemData[0];
};

struct KAUCTION_SELL_DB_RESPOND : KMISC_DB_RESPOND 
{
    BYTE    byCode;
};

struct KAUCTION_CANCEL_DB_RESPOND : KMISC_DB_RESPOND 
{
    BYTE    byCode;
    DWORD   dwSellerID;
    char    szSaleName[SALE_NAME_LEN];
    size_t  uItemDataLen;
    BYTE    byItemData[0];
};

struct KAUCTION_CHECK_DURATION_RESPOND : KMISC_DB_RESPOND 
{
    char    szSaleName[SALE_NAME_LEN];
    DWORD   dwSellerID;
    int     nPrice;
    int     nBuyItNowPrice;
    DWORD   dwBidderID;
    int     nCustodyCharges;
    size_t  uItemDataLen;
    BYTE    byItemData[0];
};

struct KGAME_CARD_BUY_RESPOND : KMISC_DB_RESPOND 
{
    BYTE    byCode;
    DWORD   dwSellerID;
    int     nCoin;
    int     nGameTime;
    BYTE    byType;
    int     nPrice;
    DWORD   dwBuyerID;
};

struct KGAME_CARD_LOOKUP_RESPOND : KMISC_DB_RESPOND 
{
    DWORD       dwPlayerID;
    int         nTotalCount;
    int         nCount;
    KGAME_CARD  CardList[0];
};

struct KGAME_CARD_CANCEL_RESPOND : KMISC_DB_RESPOND 
{
    BYTE    byCode;
    DWORD   dwPlayerID;
    int     nCoin;
};

struct KGAME_CARD_CHECK_DURATION_RESPOND : KMISC_DB_RESPOND 
{
    DWORD   dwPlayerID;
    int     nCoin;
};
// <========================== Protocol define ====================================

#define MAX_MISC_DB_SQL_SIZE    (1024 * 1024 * 8)

class KMiscDB
{
public:
    KMiscDB();

    BOOL Init();
    void UnInit();

    void  Activate();

    BOOL  IsAllPackageComplete();

    float GetOperationSpeed() { return m_fOperationSpeed; }

private:
    BOOL        PushRequest(IKG_Buffer* piRequest);
    IKG_Buffer* PopRespond();

public:
    BOOL DoSaveFellowshipData(DWORD dwPlayerID, BYTE* pbyFellowData, size_t uDataLen);
    BOOL DoLoadFellowshipData(DWORD dwPlayerID);
    BOOL DoSaveMapCopyInfo(DWORD dwMapID, int nMapCopyIndex);
    BOOL DoDeleteMapCopyInfo(DWORD dwMapID, int nMapCopyIndex);
    BOOL DoUpdateRoleBlackList(DWORD dwRoleID, time_t FreezeTime);
    BOOL DoUpdateRoleDeleteList(DWORD dwID, time_t nEndTime);

    // 邮件
    BOOL DoLoadMailBox(DWORD dwMailBoxID);
    BOOL DoSaveMailBox(
        DWORD dwMailBoxID, time_t nBaseTime, int nMailCount, BYTE* pbyMailBoxInfo, size_t uMailBoxLen
    );
    BOOL DoCheckWithdrawMail(time_t nTimeNow, time_t nSurvivalTime);
    BOOL DoSaveGlobalMail(time_t nEndTime, DWORD dwMailIndex, KMail* pMail, size_t uMailLen);
    BOOL DoDeleteGlobalMail(DWORD dwMailID);

    BOOL DoLoadPQList();
    BOOL DoSavePQ(const KGPQ_DB_DATA& crPQData);
    BOOL DoDeletePQ(DWORD dwPQID);

    BOOL DoSaveTongData(DWORD dwTongID, KTong* pTong);
    BOOL DoRemoveTongData(DWORD dwTongID);

    BOOL DoAuctionLookup(BYTE byRequestID, DWORD dwPlayerID, KAUCTION_LOOKUP_PARAM* pParam);
    BOOL DoAuctionBid(DWORD dwBidderID, DWORD dwSaleID, DWORD dwCRC, int nPrice);
    BOOL DoAuctionSell(
        DWORD dwSellerID, char* pszSaleName, char* pszSellerName, int nAucGenre, int nAucSub, int nRequireLevel,
        int nQuality, int nStartPrice, int nBuyItNowPrice, int nCustodyCharges, time_t nDurationTime,
        BYTE* pbyItemData, size_t uDataLen
    );
    BOOL DoAuctionCancel(DWORD dwSellerID, DWORD dwSaleID);
    BOOL DoAuctionCheckDuration(time_t nTimeNow);

    // 全局自定义数据
    BOOL DoSaveGlobalCustomData(const char cszName[], BYTE* pbyData, size_t uDataLen);
    BOOL DoDeleteGlobalCustomData(const char cszName[]);

    BOOL DoSaveGlobalSystemValue(const char* pszName, int nValue);
    
    // 统计数据
    BOOL DoSaveStatData(time_t nStatTime, int nDataCount, int64_t* pnData);
    BOOL DoSaveStatDataName(char* pszName, int nNameID);

    // 活动名单
    BOOL DoUpdateActivityData(const KActivityRecord& rRecord);
    BOOL DoDeleteActivityData(DWORD dwRoleID, int nType);

    // 月卡寄售
    BOOL DoGameCardSellRequest(DWORD dwSellerID, int nCoin, int nGameTime, BYTE byType, int nPrice, time_t nEndTime);
    BOOL DoGameCardBuyRequest(DWORD dwID, DWORD dwBuyerID, int nPrice);
    BOOL DoGameCardLookupRequest(DWORD dwPlayerID, BYTE byCardType, int nStartIndex, BYTE byOrderType, BOOL bDesc);
    BOOL DoGameCardCancelRequest(DWORD dwPlayerID, DWORD dwID);
    BOOL DoGameCardCheckDuration(time_t nTimeNow);
    
    // 反打钱
    BOOL DoAddFarmerRecordRequest(const char cszAccount[], time_t nPunishEndTime);
    BOOL DoDeleteFarmerRecordRequest(const char cszAccount[]);

    BOOL DoSaveMentorRecordRequest(DWORD dwMentorID, DWORD dwApprenticeID, BYTE* pbyMentorData, size_t uDataLen);
    BOOL DoDeleteMentorRecordRequest(DWORD dwMentorID, DWORD dwApprenticeID);

private:
    void OnLoadFellowshipData(BYTE* pbyData, size_t uDataLen);

    void OnLoadMailBox(BYTE* pbyData, size_t uDataLen);
    void OnCheckWithdrawMail(BYTE* pbyData, size_t uDataLen);
    void OnSendGlobalMail(BYTE* pbyData, size_t uDataLen);

    void OnLoadPQList(BYTE* pbyData, size_t uDataLen);

    void OnAuctionLookup(BYTE* pbyData, size_t uDataLen);
    void OnAuctionBid(BYTE* pbyData, size_t uDataLen);
    void OnAuctionSell(BYTE* pbyData, size_t uDataLen);
    void OnAuctionCancel(BYTE* pbyData, size_t uDataLen);
    void OnAuctionCheckDuration(BYTE* pbyData, size_t uDataLen);

    void OnGameCardBuyRespond(BYTE* pbyData, size_t uDataLen);
    void OnGameCardLookupRespond(BYTE* pbyData, size_t uDataLen);
    void OnGameCardCancelRespond(BYTE* pbyData, size_t uDataLen);
    void OnGameCardCheckDurationRespond(BYTE* pbyData, size_t uDataLen);

private:
    // 工作线程:
    static void     WorkThreadFunction(void* pvParam);
    void            RegisterRequestProcess();
    int             ThreadFunction();
    void            DBThreadActivate();
    BOOL            PushRespond(IKG_Buffer* piRespond);
    IKG_Buffer*     PopRequest();
    BOOL            DumpOperationCount();

    typedef BOOL (KMiscDB::*PROCESS_REQUEST_FUNC)(BYTE* pbyData, size_t uDataLen);
    PROCESS_REQUEST_FUNC    m_ProcessRequestFuns[emrpRequestEnd];
    size_t                  m_uRequestProtocolSize[emrpRequestEnd];

    int                     m_nOperationCount[emrpRequestEnd];
    int                     m_nOperationSpeed;
    float                   m_fOperationSpeed;

    typedef void (KMiscDB::*PROCESS_RESPOND_FUNC)(BYTE* pbyData, size_t uDataLen);
    PROCESS_RESPOND_FUNC    m_ProcessRespondFuns[emrpRespondEnd];
    size_t                  m_uRespondProtocolSize[emrpRespondEnd];

private:
    BOOL ProcessSaveFellowshipData(BYTE* pbyData, size_t uDataLen);
    BOOL ProcessLoadFellowshipData(BYTE* pbyData, size_t uDataLen);
    BOOL ProcessSaveMapCopyInfo(BYTE* pbyData, size_t uDataLen);
    BOOL ProcessDeleteMapCopyInfo(BYTE* pbyData, size_t uDataLen);
    BOOL ProcessUpdateRoleBlackList(BYTE* pbyData, size_t uDataLen);
    BOOL ProcessUpdateRoleDeleteList(BYTE* pbyData, size_t uDataLen);

    // 邮件
    BOOL ProcessLoadMailBox(BYTE* pbyData, size_t uDataLen);
    BOOL ProcessSaveMailBox(BYTE* pbyData, size_t uDataLen);
    BOOL ProcessCheckWithdrawMail(BYTE* pbyData, size_t uDataLen);
    BOOL ProcessSaveGlobalMail(BYTE* pbyData, size_t uDataLen);
    BOOL ProcessDeleteGlobalMail(BYTE* pbyData, size_t uDataLen);

    BOOL ProcessSaveTongData(BYTE* pbyData, size_t uDataLen);
    BOOL ProcessRemoveTongData(BYTE* pbyData, size_t uDataLen);

    BOOL ProcessLoadPQList(BYTE* pbyData, size_t uDataLen);
    BOOL ProcessSavePQ(BYTE* pbyData, size_t uDataLen);
    BOOL ProcessDeletePQ(BYTE* pbyData, size_t uDataLen);

    BOOL ProcessAuctionLookup(BYTE* pbyData, size_t uDataLen);
    BOOL ProcessAuctionBid(BYTE* pbyData, size_t uDataLen);
    BOOL ProcessAuctionSell(BYTE* pbyData, size_t uDataLen);
    BOOL ProcessAuctionCancel(BYTE* pbyData, size_t uDataLen);
    BOOL ProcessAuctionCheckDuration(BYTE* pbyData, size_t uDataLen);

    BOOL ProcessSaveGlobalCustomData(BYTE* pbyData, size_t uDataLen);
    BOOL ProcessDeleteGlobalCustomData(BYTE* pbyData, size_t uDataLen);

    BOOL ProcessSaveGlobalSystemValue(BYTE* pbyData, size_t uDataLen);

    // 统计数据
    BOOL ProcessSaveStatData(BYTE* pbyData, size_t uDataLen);
    BOOL ProcessSaveStatDataName(BYTE* pbyData, size_t uDataLen);

    // 活动名单
    BOOL ProcessUpdateActivityData(BYTE* pbyData, size_t uDataLen);
    BOOL ProcessDeleteActivityData(BYTE* pbyData, size_t uDataLen);

    // 月卡寄售
    BOOL ProcessGameCardSell(BYTE* pbyData, size_t uDataLen);
    BOOL ProcessGameCardBuy(BYTE* pbyData, size_t uDataLen);
    BOOL ProcessGameCardLookup(BYTE* pbyData, size_t uDataLen);
    BOOL ProcessGameCardCancel(BYTE* pbyData, size_t uDataLen);
    BOOL ProcessGameCardCheckDuration(BYTE* pbyData, size_t uDataLen);

    // 反打钱
    BOOL ProcessAddFarmerRecord(BYTE* pbyData, size_t uDataLen);
    BOOL ProcessDeleteFarmerRecord(BYTE* pbyData, size_t uDataLen);

    BOOL ProcessSaveMentorRecord(BYTE* pbyData, size_t uDataLen);
    BOOL ProcessDeleteMentorRecord(BYTE* pbyData, size_t uDataLen);

    // help tools
    int SelectFoundRows();

private:
    BOOL DoQuery(const char cszSQL[]);

private:
    MYSQL*                  m_pDBHandle;
    time_t                  m_nNextPingTime;
    KThread                 m_WorkThread;
    BOOL                    m_bRunFlag;
    char*                   m_pszSQL;

    typedef std::list<IKG_Buffer*, KMemory::KAllocator<IKG_Buffer*> > KPACKAGE_QUEUE;

    KPACKAGE_QUEUE          m_RequestQueue;
    BOOL                    m_bRequestProcessing;
    KMutex                  m_RequestQueueLock;

    KPACKAGE_QUEUE          m_RespondQueue;
    KMutex                  m_RespondQueueLock;

public:
    int                     m_nDBThreadWorkLoop;
    int                     m_nDBThreadIdleLoop;
};

#endif
