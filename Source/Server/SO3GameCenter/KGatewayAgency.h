/************************************************************************/
/* Gateway client   					                                */
/* Copyright : Kingsoft 2008										    */
/* Author	 : Zhao chunfeng, Yan rui						            */
/* History	 :															*/
/*		2008.07.22	Create												*/
/************************************************************************/
#ifndef _KGATEWAYAGENCY_H_
#define _KGATEWAYAGENCY_H_

#include "ProtocolBasic.h"
#include "GatewayRelayProtocol.h"
#include "Common/KG_Memory.h"
#include "Common/KG_Socket.h"
#include "KHometownList.h"

struct KSO3GameCenterSettings;
struct PLAYER_LOGIN_GAME_REQUEST;
struct KRoleBaseInfo;
struct KROLE_LIST_INFO;

class KGatewayAgency
{
public:
    KGatewayAgency();
    ~KGatewayAgency();

    BOOL Init();
    void UnInit();

    void Activate();

    BOOL IsAvailable() { return (m_piSocket != NULL); }

    void PrepareToQuit() { m_bQuiting = true; }

private:
    BOOL ProcessAcceptor();
    BOOL ProcessPackage();
    BOOL CheckTimeout();

public:
    BOOL DoSetGameWorldVersion(int nLowerVersion, int nUpperVersion);
    BOOL DoSyncRoleList(
        int     nPlayerIndex,   int nRoleIndex,     int nRoleCount, 
        DWORD   dwRoleID,       KRoleBaseInfo*  pBaseInfo
    );
    BOOL DoSyncNewbieMaps(int nPlayerIndex);
    BOOL DoCreateRoleRespond(BYTE byCode, unsigned long ulIdentity, KROLE_LIST_INFO* pRoleListInfo);
    BOOL DoDeleteRoleRespond(int nPlayerIndex, DWORD dwRoleID, int nCode, time_t nDeleteTime);
    BOOL DoGameLoginRespond(int nPlayerIndex, int nRespondCode, DWORD dwRoleID, DWORD dwQueueID, int nLoginIP, int nLoginPort, GUID Guid);
    BOOL DoPlayerLeaveGame(const char cszAccountName[]);
    BOOL DoChangeExtPointRequest(const char* pszAccountName, DWORD dwRoleID, WORD wExtPointIndex, WORD wChangeValue);
    BOOL DoSetChargeFlagRequest(const char* pszAccountName, DWORD dwRoleID, DWORD dwLastLoginIP, int nChargeFlag);
    BOOL DoActivePresentCodeRequest(const char* pszAccountName, DWORD dwRoleID, const char* pszPresentCode, DWORD dwActiveIP);
    BOOL DoFreezeCoinRequest(DWORD dwRequestID, const char* pszAccountName, BOOL bFreezeFlag, int nCoin);
    BOOL DoAccountExchangeRequest(
        DWORD dwRequestID, const char* pszSrcAccount, const char* pszDstAccount, DWORD dwExchangeType,
        DWORD dwDstAmount, DWORD dwSrcAmount
    );
    BOOL DoRenameRespond(BYTE byCode, int nPlayerIndex, DWORD dwRoleID, KROLE_LIST_INFO* pRoleListInfo);

private:
    BOOL Send(IKG_Buffer* piBuffer);

private:
    typedef BOOL (KGatewayAgency::*PROTOCOL_FUNCTION)(BYTE* pbyData, size_t uDataLen);

    PROTOCOL_FUNCTION m_ProtocolFunctions[g2r_protocol_total];
    size_t            m_uPakSize[g2r_protocol_total];

    BOOL OnPingSignal(BYTE* pbyData, size_t uDataLen);
    BOOL OnQueryAccountState(BYTE* pbyData, size_t uDataLen);
    BOOL OnQueryRoleList(BYTE* pbyData, size_t uDataLen);
    BOOL OnQueryNewbieMaps(BYTE* pbyData, size_t uDataLen);
    BOOL OnKickAccountNotify(BYTE* pbyData, size_t uDataLen);
    BOOL OnCreateRoleRequest(BYTE* pbyData, size_t uDataLen);
    BOOL OnDeleteRoleRequest(BYTE* pbyData, size_t uDataLen);
    BOOL OnGameLoginRequest(BYTE* pbyData, size_t uDataLen);
    BOOL OnChangeExtPointRespond(BYTE* pbyData, size_t uDataLen);
    BOOL OnSetChargeFlagRespond(BYTE* pbyData, size_t uDataLen);
    BOOL OnGetZoneChargeFlagNotify(BYTE* pbyData, size_t uDataLen);
    BOOL OnActivePresentCodeRespond(BYTE* pbyData, size_t uDataLen);
    BOOL OnFreezeCoinRespond(BYTE* pbyData, size_t uDataLen);
    BOOL OnAccountExchangeRespond(BYTE* pbyData, size_t uDataLen);
    BOOL OnRenameRequest(BYTE* pbyData, size_t uDataLen);

private:
    KG_SocketAcceptor       m_Acceptor;
    IKG_SocketStream*       m_piSocket;
    time_t                  m_nLastPingTime;
    time_t                  m_nPingCycle;
    BOOL                    m_bQuiting;
};

#endif
