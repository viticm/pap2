#ifndef _KPLAYERMANAGER_H_ 
#define _KPLAYERMANAGER_H_ 

#include <map>
#include <set>
#include "GatewayClientProtocol.h"
#include "SO3GlobalDef.h"

enum KAccountState
{
    easInvalid, 
    easWaitForHandshake,                // 等待客户端传来版本号等
    easWaitForVerifyInfo,               // 等待客户端传来用户名,密码,或者盛大Token等信息
    easQueryAccountState,               // 正在向Relay查询帐号状态
    easWaitForSDOAVerify,               // 等待SDOA验证
    easWaitForPaysysVerify,             // 等待Paysys验证
    easWaitForMibaoPassword,            // 等待客户端传来密保密码
    easWaitForMibaoVerify,              // 等待Paysys密保验证
    easWaitForRoleList,                 // 等待角色列表
    easWaitForPlayerOperation,          // 等待玩家发出创建角色/删除角色/登陆角色的请求
    easWaitForNewbieMapRespond,         // 等待新手村响应
    easWaitForCreateRoleRespond,        // 等待创建角色的响应
    easWaitForDeleteRoleRespond,        // 等待删除角色的响应
    easWaitForLoginPermission,          // 等待登陆密钥
    easQueueWaiting,                    // 登陆排队状态
    easWaitForRenameRespond,            // 等待更名确认
    easWaitForDisconnect,               // 等待客户端主动断线
    easTotal
};

typedef std::map<DWORD, IKG_Buffer*> KROLE_LIST;

struct KPlayerAgency
{
    char                          szAccount[_NAME_LEN];
    char                          szPsw[LOGIN_USER_PASSWORD_MAX_LEN];
    char                          szAct[LOGIN_USER_PASSWORD_MAX_LEN];
    char                          szSndaToken[SNDA_TOKEN_LEN];
    char                          szIDCard[ID_CARD_LEN];

    BYTE                          byMibaoMode;
    char                          szMatrixPosition[9];

    KAccountState                 nState;
    int                           nIndex;
    DWORD                         dwQueueID;
    IKG_SocketStream*             piSocket;
    time_t                        nLastPingTime;
    BOOL                          bAccountInGame;
    int                           nChargeFlag;
    tagExtPointInfo               ExtPointInfo;
    DWORD                         dwEndOfDayTime;             // 月卡截止时间
    DWORD 	                      dwLeftTimeOfPoint;		  // 计点剩余点数
    DWORD                         dwLeftTimeOfDay;            // 天卡剩余秒数
    DWORD                         dwEndTimeOfFee;             // 玩家收费总截止时间
    DWORD                         dwCoin;                     // 金币数
    DWORD                         dwLimitPlayTimeFlag;        // 防沉迷标志位
    DWORD                         dwLimitOnlineSecond;        // 防沉迷累计在线时间
    DWORD                         dwLimitOfflineSecond;       // 防沉迷累计离线时间

    DWORD                         dwLoginTime;                // 本次登录时间
    DWORD                         dwLastLoginTime;
    DWORD                         dwLastLoginIP;

    KROLE_LIST                    RoleList;

    DWORD                         dwVerifyPostTime;           // 向Paysys投递验证请求的时间,用于测量验证效率
};

class KPaysysAgency;
class KRelayAgency;
class KQueueManager;
class KSO3Gateway;

class KPlayerManager
{
public:
    KPlayerManager();

    BOOL Init();
    void UnInit();

    void Activate();

    void AttachModule(KPaysysAgency* pModule) { m_pPaysysAgency = pModule; }
    void AttachModule(KRelayAgency* pModule) { m_pRelayAgency = pModule; }
    void AttachModule(KQueueManager* pModule) { m_pQueueManager = pModule; }
    void AttachModule(KSO3Gateway* pModule) { m_pGateway = pModule; }

    KPlayerAgency* GetPlayer(int nIndex)
    {
        KPlayerTable::iterator it = m_PlayerTable.find(nIndex);
        if (it != m_PlayerTable.end())
        {
            return &it->second;
        }
        return NULL;
    }

    int  GetConnectionCount() { return (int)m_PlayerTable.size(); }

    void UnlockAccount(KPlayerAgency* pPlayer);

private:
    BOOL ProcessNewConnection(IKG_SocketStream* piSocket);
    BOOL ProcessPackage(IKG_SocketStream* piSocket);

    void CheckConnectionTimeout();
    void RecycleConnections();

    void OnDisconnect(KPlayerAgency* pPlayer);

public:
    BOOL DoPingRespond(KPlayerAgency* pPlayer, DWORD dwTime);
    BOOL DoHandshakeRespond(KPlayerAgency* pPlayer, int nCode);
    BOOL DoAccountLockedNotify(KPlayerAgency* pPlayer);
    BOOL DoAccountVerifyRespond(KPlayerAgency* pPlayer, int nRespondCode);
    BOOL DoMibaoVerifyRespond(KPlayerAgency* pPlayer, int nRespondCode);
    BOOL DoKickAccountNotify(KPlayerAgency* pPlayer);
    BOOL DoSyncRoleList(KPlayerAgency* pPlayer, int nRoleIndex, int nRoleCount, BYTE* pbyRoleData, size_t uRoleDataLen);
    BOOL DoSyncNewbieMaps(KPlayerAgency* pPlayer, DWORD dwMapID, KNEWBIE_MAP_COPY_INFO CopyInfo[], int nCopyCount);
    BOOL DoCreateRoleRespond(KPlayerAgency* pPlayer, BYTE byCode, DWORD dwRoleID, BYTE* pbyData, size_t uDataLen);
    BOOL DoDeleteRoleRespond(KPlayerAgency* pPlayer, BYTE byCode, DWORD dwRoleID, time_t nDeleteTime);
    BOOL DoGameLoginRespond(KPlayerAgency* pPlayer, BYTE byCode, DWORD dwRoleID, DWORD dwIP, int nPort, const GUID& crGuid);
    BOOL DoSyncQueueState(KPlayerAgency* pPlayer, int nPosition);
    BOOL DoGiveupQueueRespond(KPlayerAgency* pPlayer);
    BOOL DoRenameRespond(KPlayerAgency* pPlayer, BYTE byCode, DWORD dwRoleID, BYTE* pbyData, size_t uDataLen);
    BOOL DoSndaVerifyRespond(KPlayerAgency* pPlayer, int nRespondCode);

private:
    void OnPingSignal(KPlayerAgency* pPlayer, BYTE* pbyData, size_t uDataLen);
    void OnHandshakeRequest(KPlayerAgency* pPlayer, BYTE* pbyData, size_t uDataLen);
    void OnAccountVerifyRequest(KPlayerAgency* pPlayer, BYTE* pbyData, size_t uDataLen);
    void OnSndaTokenVerifyRequest(KPlayerAgency* pPlayer, BYTE* pbyData, size_t uDataLen);
    void OnMibaoVerifyRequest(KPlayerAgency* pPlayer, BYTE* pbyData, size_t uDataLen);
    void OnQueryNewbieMaps(KPlayerAgency* pPlayer, BYTE* pbyData, size_t uDataLen);
    void OnCreateRoleRequest(KPlayerAgency* pPlayer, BYTE* pbyData, size_t uDataLen);
    void OnDeleteRoleRequest(KPlayerAgency* pPlayer, BYTE* pbyData, size_t uDataLen);
    void OnGameLoginRequest(KPlayerAgency* pPlayer, BYTE* pbyData, size_t uDataLen);
    void OnGiveupQueueRequest(KPlayerAgency* pPlayer, BYTE* pbyData, size_t uDataLen);
    void OnRenameRequest(KPlayerAgency* pPlayer, BYTE* pbyData, size_t uDataLen);

private:
    typedef void (KPlayerManager::*PC2G_PROTOCOL_FUNC)(KPlayerAgency* pPlayer, BYTE* pbyData, size_t uDataLen);

    PC2G_PROTOCOL_FUNC m_PakProcessor[c2g_protocol_total];
    size_t             m_uPakSize[c2g_protocol_total];
   
private:
    typedef std::map<int, KPlayerAgency> KPlayerTable;
    KPlayerTable                m_PlayerTable;

	struct KAccountCompareLess
	{
		bool operator()(const string& crX, const string& crY) const
		{
			return STR_CASE_CMP(crX.c_str(), crY.c_str()) < 0;
		}
	};

    typedef std::set<string, KAccountCompareLess> KLockedAccountTable;
    KLockedAccountTable         m_LockedAccountTable;

    int                         m_nPingCycle;

    KPaysysAgency*              m_pPaysysAgency;
    KRelayAgency*               m_pRelayAgency;
    KQueueManager*              m_pQueueManager;
    KSO3Gateway*                m_pGateway;

private:
    KG_SocketServerAcceptor*    m_pSockerServer;
    KG_SOCKET_EVENT*            m_pSocketEvents;
    int                         m_nNextPlayerIndex;
    int                         m_nMaxPlayer;
    BOOL                        m_bRecycleConnections;
    time_t                      m_nTimeNow;
    time_t                      m_nNextPingCheck;
};

#endif
