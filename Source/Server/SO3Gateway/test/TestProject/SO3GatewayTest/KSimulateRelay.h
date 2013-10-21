///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName :   KSimulateRelay.h
// Creator  :   liuzhibiao
// Date     :   2008-10-31
// Comment  :   v1.0
//              v1.0    2008-10-31 加入存储用户登陆数据的数据结构和相关方法
///////////////////////////////////////////////////////////////

#ifndef _KG_SIMULATERELAY_H_
#define _KG_SIMULATERELAY_H_

#include "Protocol/AccountLoginDef.h"
#include "KG_SO3GateWayTestDef.h"
#include "GatewayRelayProtocol.h"
#include "KRoleDBDataDef.h"

#define MAX_PLAYER_IN_GAMEWORLD 50

struct KInGameWorldPlayer
{
    char  szAccount[LOGIN_USER_ACCOUNT_MAX_LEN];
    int   nGameWorldMapID;
    int   nNumInGameWorld;
};

class KSimulateRelay
{
public:
    int  Init(const KG_SIMULATE_GAMECENTER_CONFIG& crSimulateRelayConfig);
	void UnInit();
	void Activate();
    int  GetUserEnterGameRequestCount(){ return m_nUserEnterGameRequestCount;}
    int  SetWhenRevDoNotRespond(int nG2RProtocol);
    int  HadRevKickInfo(){ return m_nHadRevKickInfo;}
    int  SetGameLogResult(int nEnterGameResult);
    int  SetAllowLoginInGameWorldCount(int nAllowCount);    // 当GC返回登陆成功的时候，这参数才生效
    int  GetPlayer(KInGameWorldPlayer* pInGameWorldPlayer, char szAccountName[]);
    int  ChangeTheLoginMap(int nMapID);                     // 更改登陆地图
    int  HadConnections(int* pnHadConnected);

    int DoChangeExtPointRequest();
    int DoSetChargeFlagRequest();
    int DoActivePresentCoodeRequest();

    int DoFreezeCoinRequest();
    int DoAccountExchangeRequest();

    //方便测试用:
    int m_nRevChangeExtPointRespond;
    int m_nRevActivePresentCodeRespond;
    int m_nRevPlayerSetChargeFlagRespond;
    int m_nRevGetZoneChargeFlagNotify;

    int m_nRevFreezeCoinRespond;
    int m_nRevAccountExchangeRespond;

private:
    int DoSetProtocolVersion(int nLowerVersion, int nUpperVersion);
    int DoSyncRoleList(int nPlayerIndex, int nRoleIndex, int nRoleCount, KROLE_LIST_INFO* pBaseData);
    int DoSyncNewbieMaps(int nPlayerIndex);
    int DoCreateRoleRespond(int nSucceed, int nPlayerIndex);
    int DoDeleteRoleRespond(DWORD dwPacketIdentity, int nRespondCode);
    int DoReplyLoginData(DWORD dwPacketIdentity, DWORD dwRoleID, int nRespondCode);
    int DoPingRespond();

private:
    static void WorkThreadFunction(void *pvParam);
    int ThreadFunction();

private:
    int ProcessAcceptor();
    int ProcessPackage();

private:
	typedef int (KSimulateRelay::*PROTOCOL_FUNCTION)(BYTE* pbyData, size_t uDataLen);

	PROTOCOL_FUNCTION m_ProtocolFunctions[g2r_protocol_total];
	size_t            m_uPakSize[g2r_protocol_total];

    int OnPingRequest( BYTE* pbyData, size_t uDataLen );
	int OnQueryAccountState(BYTE* pbyData, size_t uDataLen);
	int OnQueryRoleList(BYTE* pbyData, size_t uDataLen);
	int OnQueryNewbieMaps(BYTE* pbyData, size_t uDataLen);
	int OnCreateRoleRequest(BYTE* pbyData, size_t uDataLen);
	int OnDeleteRoleRequest(BYTE* pbyData, size_t uDataLen);
	int OnGameLoginRequest(BYTE* pbyData, size_t uDataLen);
    int OnKickAccountRequest(BYTE* pbyData, size_t uDataLen);

	int OnChangeExtPointRespond(BYTE* pbyData, size_t uDataLen);//new
    int OnSetChargeFlagRespond(BYTE* pbyData, size_t uDataLen);//new
    int OnGetZoneChargeFlagNotify(BYTE* pbyData, size_t uDataLen);//new
    int OnActivePresentCodeRespond(BYTE* pbyData, size_t uDataLen);//new

    int OnFreezeCoinResond(BYTE* pbyData, size_t uDataLen);
    int OnAccountExchangeResond(BYTE* pbyData, size_t uDataLen);

private:
	KG_SocketAcceptor               m_Acceptor;
	IKG_SocketStream*               m_piSocket;
    KG_SIMULATE_GAMECENTER_CONFIG   m_RelayConfig;
    KThread                         m_WorkThread;
    int                             m_nExitFlag;
    int                             m_nUserEnterGameRequestCount;
    int                             m_nDoNotRespondProtocol;
    int                             m_nHadRevKickInfo;
    int                             m_nQueueID;

    list<KInGameWorldPlayer*>       m_InGameWorldPlayerList;
    int                             m_nLoginInGameWorldNum; //记录当前有几个玩家进入了游戏世界
    int                             m_nHadConnections;
};

#endif//_KG_SIMULATERELAY_H_

