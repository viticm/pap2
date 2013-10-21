///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName : KG_SO3GateWayTestDef
// Creator  : liuzhibiao
// Date     : 2008-12-16
// Comment  : v1.1
//			  V0.6  增加测试点数据结构
//            V1.1  改变数据结构
///////////////////////////////////////////////////////////////

#ifndef _KG_SO3GATEWAYTESTDEF_H_
#define _KG_SO3GATEWAYTESTDEF_H_

#pragma pack(1)

#define IP_LENGTH                       16
#define REV_TIME_OUT                    -1
#define CLIENT_HAD_KICK                 100
#define PAYSYS_HAD_REV_USER_LOGINOUT    200
#define USER_ACCOUNT_LOCKED             300
#define CLIENT_REV_TIMEOUT              400
#define CLIENT_REV_LOGINGAME_STATE_INFO 500
#define CLIENT_REV_GIVEUP_QUEUE_RESPOND 600
#define TEXT_DESCRIPTION_LENGTH         128
#define MAX_RECV_LOOP_TIMES             100
#define GATEWAY_CONNECT_CONFIG_FILE     "gatewaytestconfig.ini"
#define CLIENT_GATEWAY_VERSION          15

enum KG_GATEWAY_TEST_POINT
{
    KG_CLIENT_HANDSHAKE,    // 客户端握手
    KG_USER_LOGIN,          // 用户登陆
    KG_USER_MIBAO,          // 用户密保
    KG_USER_RETRY,          // 用户重试
    KG_ROLE_LIST,           // 用户获取角色列表
    KG_ROLE_ENTER_GAME,     // 用户选择角色登陆游戏世界
    KG_CREATE_ROLE,         // 用户创建角色
    KG_DELETE_ROLE,         // 用户删除角色
    KG_HOMETOWN_LIST,       // 用户获取新手村
    KG_GATEWAY_RECONNECT    // Gateway断线重连
};

struct KG_SIMULATE_PAYSYS_CONFIG
{
    char    szPaysysIPAddr[IP_LENGTH];      // KG_SocketAcceptor::Init参数
    int     nPaysysPort;                    // 虚拟paysys的监听端口     

    int     nPaysysLostconnect;             // paysys连接丢失
    int     nGatewayLoginResult;            // 网关登录返回码
    //int     nGatewayLoginReconnectResult;   // 网关重联返回码

    int     nNotRespondProtocol;            // 不做响应的协议（只接收，不回复）
    int     nNotHandleProtocol;             // 不做处理的协议（接收和回复都跳过）
    int     nNotRespondSubProtocol;         // 不做响应的子协议（只接收，不回复）
    int     nUserLoginResult;               // 帐户登陆结果
    WORD    wType;                          // 登陆形式AccountUserLoginInfo/AccountUserPasspodVerifyRet
    BYTE    byMibaoMode;                    // 模式
    int     nMibaoResult;                   // 密保返回结果
};

struct KG_SIMULATE_GAMECENTER_CONFIG
{
    char  szRelayIPAddr[IP_LENGTH];       // 虚拟GameCenter的ip
    int   nRelayPort;                     // 虚拟GameCenter的监听端口 

    int   nGameWorldProtocolLowerVersion;   // 游戏世界版本号Lower
    int   nGameWorldProtocolUpperVersion;   // 游戏世界版本号Upper
    int   nMapCount;                        // 新手村数目   
    int   nGetRoleListCount;                // 角色个数
    int   nCreatRoleResult;                 // 创建角色成功？
    int   nDeleteRoleResult;                // 删除角色成功？
    int   nUserOnlineResult;                // 玩家是否在线？
    int   nEnterGameResult;                 // 进入游戏是否成功？
    int   nNotRespondProtocol;              // 不做响应的协议
    int   nAllowPlayerInGameWorld;          // 允许进入游戏世界的人数
};

struct KG_SIMULATE_CLIENT_CONFIG
{
    char            szConnectIPAddr[IP_LENGTH];         // Client连接Gateway的IP
    int             nConnectPort;                       // Client连接Gateway的Port

    int             nClientGatewayVersion;              // 客户端保存的GateWay版本
    int             nClientGameWorldVersion;            // 客户端游戏世界版本
    char            szClientAccountName[_NAME_LEN];     // 登陆的用户名
    KSG_PASSWORD    szClientAccountPassword;            // 密码
    char            szClientActivateCode[_NAME_LEN];    // 激活码
    int             nDelRoleID;                         // 需要删除的角色id号
    int             nHadRevKick;                        // 需要收到踢号信息
    int             nUserGiveUpQueue;                   // 当登陆游戏世界需要排队时,用户选择放弃排队
    int             nNeedTestTimeOut;                   // 是否需要检测接收超时标志位
    int             nNeedStopWhenRevAppointInfo;        // 是否需要收到指定消息的时候终止运行
    int             nNeedSendPing;
};

typedef struct KG_RESULT            //预计结果
{
    int nTestPoint;                 //检查点
    int nResult;                    //回复包类型 
    int nTypeResult;                //子码 

}KG_FORECAST_RESULT, KG_RETURN_RESULT;

struct KG_GATEWAY_TEST_MANAGER
{
    KG_SIMULATE_PAYSYS_CONFIG       PaysysManagerConfig;
    KG_SIMULATE_GAMECENTER_CONFIG   GameCenterConfig;
    KG_SIMULATE_CLIENT_CONFIG       ClientConfig;

    KG_FORECAST_RESULT              ForecastConfig;
    char                            m_szTestGWUserName[_NAME_LEN];
    char                            m_szGatewayPath[MAX_PATH];
    int                             nNeedSimulatePaysys;
    int                             nNeedSimulateRelay;
};

#pragma pack()

#endif//_KG_SO3GATEWAYTESTDEF_H_
