/************************************************************************/
/* GameCenter和LogServer的连接	                                        */
/* Copyright : Kingsoft 2009										    */
/* Author	 : yanrui   												*/
/* History	 :															*/
/*		2009.10.20	Create												*/
/************************************************************************/

#ifndef _KLOG_SERVER_AGENCY_H_
#define _KLOG_SERVER_AGENCY_H_

#include "Common/KG_Socket.h"
#include "Protocol/KG_CSLogServerProtocal.h"

#define MAX_LOG_BUFFER_SIZE 1024

class KRole;

class KLogServerAgency
{
public:
    KLogServerAgency();

    BOOL Init();
    void UnInit();

    void Activate();

    void LogFreezeCoin(KRole* pRole, int nCoin, const char* pszComment);
    void LogUnfreezeCoin(KRole* pRole, int nCoin, const char* pszComment);
    void LogGameCardSell(KRole* pRole, int nCoin, const char* pszComment);
    void LogGameCardBuy(KRole* pRole, int nGameTime, BYTE byType, const char* pszComment);
    void LogRoleAntiFarmer(KRole* pRole, time_t nAntiSeconds);
    void LogBuyAuctionItem(DWORD dwBuyerID, DWORD dwSellerID, size_t uItemDataLen, BYTE* pbyItemData, int nPrice);
    void LogApexKickRole(KRole* pRole, int nActionID);
    void LogFreezeCoinTimeout(KRole* pRole, int nCoin, int nPrice);
    void LogAccountExchangeTimeout(KRole* pSellerRole, KRole* pBuyyerRole, int nCoin, int nGameTime, int nPrice);
    void LogMentorshipCreate(DWORD dwMentorID, DWORD dwApprenticeID, const char* pszComment);
    void LogMentorshipDelete(DWORD dwMentorID, DWORD dwApprenticeID, int nMentorValue, const char* pszComment);
    void LogAcquireMentorValue(DWORD dwMentorID, DWORD dwApprenticeID, int nValue, const char* pszComment);
    void LogPlayerItemChange(
        KRole* pRole, ITEM_OPERATION_TYPE eOperationType,
        DWORD dwItemTabType, DWORD dwItemIndex, int nStackNum, 
        const char* pcszComment, const char* pcszCommentExt = NULL
    );

private:
    BOOL LoginServer(const char cszIP[], int nPort, const char cszIdentity[]);

    BOOL Ping();

    BOOL LogPlayerAction(
        PLAYER_ACTION_TYPE eActionType, unsigned uActionLevel, const char* pszActionTarget,
        const char* pszAccountName, const char* pszRoleName, size_t uCommentSize,
        BYTE* pbyComment
    );

    BOOL Send(IKG_Buffer* piBuffer);

public:
    char              m_szIdentity[_NAME_LEN];

private:
    IKG_SocketStream* m_piSocket;
    int               m_nPingCycle;
    time_t            m_nNextPingTime;
    char              m_szTempBuffer[MAX_LOG_BUFFER_SIZE];
};

#endif // _KLOG_SERVER_AGENCY_H_
