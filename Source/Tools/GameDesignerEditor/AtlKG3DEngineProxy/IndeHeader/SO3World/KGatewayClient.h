/************************************************************************/
/* Gateway client   					                                */
/* Copyright : Kingsoft 2008										    */
/* Author	 : Zhao chunfeng, Yan rui						            */
/* History	 :															*/
/*		2008.07.22	Create												*/
/************************************************************************/
#ifndef _KGATEWAYCLIENT_H_
#define _KGATEWAYCLIENT_H_

#ifdef _CLIENT

// 该系列枚举用于BishopClient将登陆流程相关事件通知UI模块
typedef enum
{
    LPUM_UNABLE_TO_CONNECT_SERVER,              // "无法连接服务器"
    LPUM_MISS_CONNECTION,                       // "服务器连接丢失"
    LPUM_SYSTEM_MAINTENANCE,                    // "系统维护"
    LPUM_UNMATCHED_PROTOCOL_VERSION,            // "协议版本不匹配，请更新"

    LPUM_HANDSHAKE_ACCOUNT_SYSTEM_LOST,         // "帐号系统在维护"

    LPUM_LOGIN_VERIFY_ALREADY_IN_GATEWAY,       // "该账号正在使用: 正在本组服务器的Bishop上验证呢!"
    LPUM_LOGIN_VERIFY_IN_GATEWAY_BLACK_LIST,    // "账号因为使用非法客户端而被锁入黑名单了"
    LPUM_LOGIN_VERIFY_SUCCESS,                  // "验证通过,一切顺利"
    LPUM_LOGIN_VERIFY_IN_GAME,                  // "账号正在游戏中，稍后重试(自动踢号)"
    LPUM_LOGIN_VERIFY_ACC_PSW_ERROR,            // "账号或者密码错误"
    LPUM_LOGIN_VERIFY_NO_MONEY,                 // "没钱了"
    LPUM_LOGIN_VERIFY_NOT_ACTIVE,               // "账号没有激活"
    LPUM_LOGIN_VERIFY_ACTIVATE_CODE_ERR,        // "激活码错误，不存在或已经被使用过了"
    LPUM_LOGIN_VERIFY_IN_OTHER_GROUP,           // "该账号已经在其他区服登录"
    LPUM_LOGIN_VERIFY_ACC_FREEZED,              // "账号被冻结了"
    LPUM_LOGIN_VERIFY_PAYSYS_BLACK_LIST,        // "多次密码错误,账号被Paysys锁进黑名单了"
    LPUM_LOGIN_VERIFY_LIMIT_ACCOUNT,            // "访沉迷用户，不能登入"
    LPUM_LOGIN_VERIFY_UNKNOWN_ERROR,            // "未知错误"

    LPUM_GET_ROLE_LIST_SUCCESS,                 // 获得角色列表OK
    LPUM_GET_ALL_ROLE_LIST_SUCCESS,             // 获得全部角色列表完成
    LPUM_UPDATE_HOMETOWN_LIST,                  // 更新新手村列表数据

    LPUM_CREATE_ROLE_SUCCESS,                   // "创建角色OK"
    LPUM_CREATE_ROLE_NAME_EXIST,                // "创建失败,角色名已存在"
    LPUM_CREATE_ROLE_INVALID_NAME,              // "创建失败,角色名非法"
    LPUM_CREATE_ROLE_UNABLE_TO_CREATE,          // "创建失败,无法创建角色"

    LPUM_REQUEST_LOGIN_GAME_SUCCESS,            // 已经取得游戏世界登陆信息，正在连接服务器
    LPUM_REQUEST_LOGIN_GAME_OVERLOAD,           // "游戏世界人数已满,稍后再来"
    LPUM_REQUEST_LOGIN_GAME_MAINTENANCE,        // "服务器正在维护"
    LPUM_REQUEST_LOGIN_GAME_ROLEFREEZE,         // "该角色已冻结"
    LPUM_REQUEST_LOGIN_GAME_UNKNOWN_ERROR,      //  未知错误

    LPUM_DELETE_ROLE_SUCCESS,                   // 删除OK，请更新角色列表显示
    LPUM_DELETE_ROLE_BAD_PASSWORD,              // "二级密码不对"
    LPUM_DELETE_ROLE_NOT_ALLOWED,               // "该角色不允许删除(帮主)"
    LPUM_DELETE_ROLE_UNKNOWN_ERROR,             // 不晓得什么原因，反正失败了:)

    LPUM_GIVEUP_QUEUE_SUCCESS,                  // 成功放弃排队
    LPUM_GIVEUP_QUEUE_ERROR,                    // 放弃排队出错

} LOGIN_PROCESS_UI_MESSAGE;

struct UI_ROLE_LIST_ITEM
{
    char    szAccountName[_NAME_LEN];
    char    szRoleName[_NAME_LEN];
    DWORD   dwRoleID;
    int     nRoleType;
    int     nLevel;
    DWORD   dwMapID;
    int	    nMapCopyIndex;
	time_t	nLastSaveTime;
    time_t  nTotalGameTime;
    time_t  nFreezeTime;
    WORD    wRepresentID[perRepresentCount];
};

struct UI_ROLE_CREATE_PARAM
{
    char    szRoleName[_NAME_LEN];
    int     nRoleType; 
    DWORD   dwMapID;
    int     nMapCopyIndex;
    WORD	wRepresentId[perRepresentCount];
};

struct KUI_HOMETOWN_COPY_INFO 
{
    DWORD dwCopyIndex;
    int   nLoadFactor;
};

typedef std::vector<KUI_HOMETOWN_COPY_INFO> KUI_HOMETOWN_LOAD_REFACT_TABLE;

struct KUI_HOMETOWN_MAP_INFO
{
    DWORD dwMapID;
    KUI_HOMETOWN_LOAD_REFACT_TABLE LoadFactorTable;
};

typedef std::vector<KUI_HOMETOWN_MAP_INFO> KUI_HOMETOWN_MAP_TABLE;

struct KROLE_LIST_INFO;

class KGatewayClient
{
public:
	KGatewayClient(void);
	~KGatewayClient(void);

    void    Activate();
    void    ProcessConnectGateway();
    void    ProcessPackage();
    
    int     SetGatewayAddress(const char cszLoginIP[], unsigned short usLoginPort);
    int     BeginLogin(const char cszAccountName[], const char cszPasswordMD5[], const char cszActiveCode[]);
    int     GetRoleListItem(UI_ROLE_LIST_ITEM *pRoleListItem, int nRoleIndex); // nRoleIndex 从0开始
    int     QueryHometownList();
    int     RoleLogin(const char cszRoleName[]);
    int     DeleteRole(const char cszRoleName[], const char cszSecPasswordMD5[]);    
    int     CreateRole(const UI_ROLE_CREATE_PARAM &crRoleCreateParam);
    int     GetRoleCount();
    DWORD   GetLeftTime();
    time_t  GetLastLoginTime();
    DWORD   GetLastLoginIP();
    int     ReLogin();
    void    Reset();

    const KUI_HOMETOWN_MAP_TABLE& GetHometownList() { return m_HometownList; };
    char* GetAccountName() { return m_szAccountName; }

private:
    char                    m_szLoginIP[32];
    unsigned short          m_usLoginPort;
    IKG_SocketStream*       m_piSocketStream;

    char                    m_szAccountName[_NAME_LEN];
    KSG_PASSWORD            m_Password;    
    char                    m_szActivateCode[_NAME_LEN];

    time_t                  m_nLastPingTime;

    DWORD                   m_dwLeftTime;
    time_t                  m_nLastLoginTime;
    DWORD                   m_dwLastLoginIP;

	typedef std::vector<UI_ROLE_LIST_ITEM>  ROLE_LIST;
    ROLE_LIST               m_RoleList;

    KUI_HOMETOWN_MAP_TABLE  m_HometownList;
    
    BOOL    AddRoleToRoleList(KROLE_LIST_INFO* pRoleListInfo);
    DWORD   GetRoleID(const char* cszRoleName);
    BOOL    ConnectGateway();

    BOOL    Send(IKG_Buffer* piSendBuffer);

public:
    BOOL DoPingSignal();
    BOOL DoHandshakeRequest();
    BOOL DoAccountVerifyRequest();
    BOOL DoCreateRoleRequest(const UI_ROLE_CREATE_PARAM &crRoleCreateParam);
    BOOL DoDeleteRoleRequest(DWORD dwRoleID);
    BOOL DoLoginGameRequest(DWORD dwRoleID);
    BOOL DoQueryHometownListRequest();
    BOOL DoGiveupQueueRequest();

private:
    typedef void (KGatewayClient::*PROTOCOL_FUNCTION)(BYTE* pbyData, size_t uDataLen);

    PROTOCOL_FUNCTION m_ProtocolFunctions[UCHAR_MAX + 1];
    size_t            m_uPakSize[UCHAR_MAX + 1];
    
    enum CONNECT_STATE
    {
        csInvalid,
        csWaitingConnectOK
    };
    CONNECT_STATE m_ConnectState;

    void OnPingRespond(BYTE* pbyData, size_t uDataLen);
    void OnHandshakeRespond(BYTE* pbyData, size_t uDataLen);
    void OnAccountLockedNotify(BYTE* pbyData, size_t uDataLen);
    void OnAccountVerifyRespond(BYTE* pbyData, size_t uDataLen);
    void OnKickAccountNotify(BYTE* pbyData, size_t uDataLen);
    void OnSyncRoleList(BYTE* pbyData, size_t uDataLen);
    void OnSyncNewbieMaps(BYTE* pbyData, size_t uDataLen);
    void OnCreateRoleRespond(BYTE* pbyData, size_t uDataLen);
    void OnDeleteRoleRespond(BYTE* pbyData, size_t uDataLen);
    void OnGameLoginPrepare(BYTE* pbyData, size_t uDataLen);
    void OnSyncLoginKey(BYTE* pbyData, size_t uDataLen);
    void OnSyncQueueState(BYTE* pbyData, size_t uDataLen);
    void OnGiveupQueueRespond(BYTE* pbyData, size_t uDataLen);
};


extern KGatewayClient g_GatewayClient;

#endif  // _CLIENT

#endif  // _KGATEWAYCLIENT_H_
