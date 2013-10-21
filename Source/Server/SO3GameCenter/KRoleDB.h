//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KRoleDB.h
//  Version     :   1.0
//  Creater     :   Zhao Chunfeng
//  Date        :   2008-06-02 09:14:39
//  Comment     :   角色数据库相关操作
//////////////////////////////////////////////////////////////////////////////////////
#ifndef _KROLE_DB_H_
#define _KROLE_DB_H_

#include <map>
#include <list>
#include "KDBBase.h"
#include "Engine/KThread.h"
#include "Engine/KMutex.h"
#include "Engine/KTextFilter.h"
#include "Common/KG_Memory.h"
#include "ProtocolBasic.h"
#include "KRoleDBDataDef.h"
#include "Protocol/BishopRelayProtocol.h"

#define KG_ID_FIELD_NAME            "ID"
#define KG_NAME_FIELD_NAME          "RoleName"
#define KG_ACCOUNT_FIELD_NAME       "Account"
#define KG_BASE_INFO_FIELD_NAME     "BaseInfo"
#define KG_EXT_INFO_FIELD_NAME      "ExtInfo"
#define KG_LAST_MODIFY_FIELD_NAME   "LastModify"
#define KG_DELETE_TIME_FIELD_NAME   "DeleteTime"

struct KACCOUNT_LOGIN_INFO 
{
    int             nChargeFlag;                // 付费状态
    tagExtPointInfo	ExtPointInfo;			    // 可用的附送点
    DWORD           dwEndTimeOfFee;             // 收费总截止时间
    DWORD           dwCoin;                     // 金币数
    DWORD           dwLimitPlayTimeFlag;        // 防沉迷标志位
    DWORD           dwLimitOnlineSecond;        // 防沉迷累计在线时间
    DWORD           dwLimitOfflineSecond;       // 防沉迷累计离线时间
    int             nIdentity;                  // 请求包的id
};

#define MAX_ROLE_DB_SQL_SIZE    (MAX_ROLE_DATA_SIZE * 2)

class KRoleManager;

class KRoleDB
{
public:
    KRoleDB();
    ~KRoleDB();

    BOOL                Init();
    void                UnInit();
    
    void                Activate();

    float               GetRoleSaveTimeCost() { return m_fRoleSaveTimeCost; } 
    float               GetRoleLoadTimeCost() { return m_fRoleLoadTimeCost; }
    
public:
    void                DoCreateRole(KDB_ROLE_GENERATOR_PARAM* pRoleParam, unsigned long ulIdentity);
    void                DoSaveRoleData(int nGSIndex, DWORD dwPlayerID, const KRoleBaseInfo& crBaseInfo, IKG_Buffer* piRoleData, int nUserValue);
    void                DoLoadRoleData(int nGSIndex, DWORD dwPlayerID);
    void                DoDeleteRole(DWORD dwRoleID);
    void                DoQueryRoleBaseData(DWORD dwRoleID, int nPlayerIndex, int nRoleIndex, int nRoleCount);
    void                DoRename(DWORD dwRoleID, int nPlayerIndex, char szNewName[]);

private:
    static void         WorkThreadFunction(void* pvParam);
    int                 ThreadFunction();
    void                DBThreadActivate();
    
    void                ProcessCreateQueue();
    void                ProcessSaveQueue(int nSaveCount);
    void                ProcessLoadQueue();
    void                ProcessDeleteQueue();
    void                ProcessQueryRoleBaseInfoQueue();
    void                ProcessRenameQueue();

private:
    BOOL                Connect();
    void                Disconnect();

private:    
    void                DealWithLoadRoleDataRespond();
    void                DealWithCreateRoleRespond();
    void                DealWithQueryRoleBaseDataRespond();
    void                DealWithRenameRespond();

private:
    MYSQL*              m_pDBHandle;
    time_t              m_nNextPingTime;
    KThread             m_WorkThread;
    BOOL                m_bRunFlag;
    char*               m_pszSQL;

public:
    int                 m_nDBThreadWorkLoop;
    int                 m_nDBThreadIdleLoop;

private:
    // ------------- 创建角色 -------------------
    struct KCREATE_REQUEST
    {
        KDB_ROLE_GENERATOR_PARAM    Param;
        unsigned long               ulIdentity;
    };
    typedef std::list<KCREATE_REQUEST> KCREATE_QUEUE;
    KCREATE_QUEUE       m_CreateQueue;
    KMutex              m_CreateQueueLock;

    struct KCREATE_RESULT 
    {
        unsigned long   ulIdentity;
        BYTE            byCode;
        KROLE_LIST_INFO RoleListInfo;
    };
    typedef std::list<KCREATE_RESULT> KCREATE_RESULT_QUEUE;
    KCREATE_RESULT_QUEUE    m_CreateResult;
    KMutex                  m_CreateResultLock;
    
    DWORD   GetCurrentRoleID();
    BOOL    GetRoleBaseInfo(KDB_ROLE_GENERATOR_PARAM* pParam, DWORD dwRoleID, KRoleBaseInfo* pRetBaseInfo);
    BOOL    GetCreateTask(KCREATE_REQUEST* pTask);
    BOOL    CheckRoleCountInAccount(KDB_ROLE_GENERATOR_PARAM* pCreateRoleParam);
    BYTE    CreateRole(KDB_ROLE_GENERATOR_PARAM* pCreateRoleParam, KROLE_LIST_INFO* pRetRoleListInfo);

private:
    // -------- 角色保存相关数据结构 ----------------------
    struct KSAVE_REQUEST
    {
        int                     nGSIndex;
        int                     nUserValue;
        KRoleBaseInfo           BaseInfo;
        IKG_Buffer*             piExtInfo;
    };
    
    typedef std::map<DWORD, KSAVE_REQUEST>  KSAVE_DATA_TABLE;
    typedef std::list<DWORD>                KSAVE_QUEUE;
    
    KSAVE_DATA_TABLE    m_SaveDataTable;
    KSAVE_QUEUE         m_SaveQueue;
    KMutex              m_SaveQueueLock;

    float               m_fRoleSaveTimeCost;

    BOOL        GetSaveTask(DWORD* pdwRetRoleID, KSAVE_REQUEST* pTask);
    BOOL        SaveRoleData(DWORD dwRetRoleID, const KRoleBaseInfo& crBaseInfo, IKG_Buffer* piExtInfo);

private:
    // --------- 角色加载相关数据结构 --------------------
    struct KLOAD_REQUEST
    {
        DWORD       dwPlayerID;
        int         nGSIndex;
    };
    typedef std::list<KLOAD_REQUEST>        KLOAD_QUEUE;
    
    KLOAD_QUEUE m_LoadQueue;
    KMutex      m_LoadQueueLock;

    struct KLOAD_RESULT
    {
        BOOL        bSucceed;
        DWORD       dwPlayerID;
        int         nGSIndex;
        IKG_Buffer* piRoleData;
    };
    typedef std::list<KLOAD_RESULT>        KLOAD_RESULT_QUEUE;
    
    KLOAD_RESULT_QUEUE  m_LoadRetQueue;
    KMutex              m_LoadRetLock;
    float               m_fRoleLoadTimeCost;

    BOOL        GetLoadTask(KLOAD_REQUEST* pTask);
    BOOL        LoadRoleData(DWORD dwRoleID, IKG_Buffer** ppiRoleExtInfo);
    BOOL        LoadRoleDataFromSaveQueue(DWORD dwRoleID, IKG_Buffer** ppiRoleExtInfo);
    BOOL        LoadRoleDataFromDB(DWORD dwRoleID, IKG_Buffer** ppiRoleExtInfo);

private:
    // -------- 删除角色 -----------------------------
    struct KDELETE_REQUEST 
    {
        DWORD           dwRoleID;
    };
    typedef std::list<KDELETE_REQUEST>      KDELETE_REQUEST_QUEUE;
    KDELETE_REQUEST_QUEUE   m_DeleteQueue;
    KMutex                  m_DeleteQueueLock;
    
    BOOL        GetDeleteTask(KDELETE_REQUEST* pTask);
    BOOL        DeleteRole(DWORD dwRoleID);

private:
    // ---------- 查询角色基本信息 -----------------------------
    struct KQUERY_ROLE_BASE_INFO_REQUEST
    {
        DWORD               dwRoleID;
        int                 nPlayerIndex;
        int                 nRoleIndex;
        int                 nRoleCount;
    };
    typedef std::list<KQUERY_ROLE_BASE_INFO_REQUEST> KQUERY_ROLE_BASE_INFO_REQUEST_QUEUE;
    KQUERY_ROLE_BASE_INFO_REQUEST_QUEUE m_QueryRoleBaseInfoQueue;
    KMutex                              m_QueryRoleBaseInfoQueueLock;

    struct KQUERY_ROLE_BASE_INFO_RESULT
    {
        DWORD               dwRoleID;
        int                 nPlayerIndex;
        int                 nRoleIndex;
        int                 nRoleCount;
        BOOL                bSucceed;
        KRoleBaseInfo       BaseInfo;
    };

    typedef std::list<KQUERY_ROLE_BASE_INFO_RESULT> KQUERY_ROLE_BASE_INFO_RESULT_QUEUE;
    KQUERY_ROLE_BASE_INFO_RESULT_QUEUE   m_QueryRoleBaseResultQueue;
    KMutex                               m_QueryRoleBaseResultQueueLock;

    BOOL        GetQueryRoleBaseInfoTask(KQUERY_ROLE_BASE_INFO_REQUEST* pTask);
    BOOL        QueryRoleBaseInfo(DWORD dwRoleID, KRoleBaseInfo* pBaseInfo);
    BOOL        QueryRoleBaseInfoFromSaveQueue(DWORD dwRoleID, KRoleBaseInfo* pBaseInfo);
    BOOL        QueryRoleBaseInfoFromDB(DWORD dwRoleID, KRoleBaseInfo* pBaseInfo);

private:
    // ---------- 更改角色姓名信息 -----------------------------
    struct KRENAME_REQUEST
    {
        DWORD dwRoleID;
        int   nPlayerIndex;
        char  szNewName[_NAME_LEN];
    };
    typedef std::list<KRENAME_REQUEST> KRENAME_REQUEST_QUEUE;
    KRENAME_REQUEST_QUEUE   m_RenameQueue;
    KMutex                  m_RenameQueueLock;

    struct KRENAME_RESULT
    {
        BYTE            byCode;
        DWORD           dwRoleID;
        int             nPlayerIndex;
        KROLE_LIST_INFO RoleListInfo;
    };
    typedef std::list<KRENAME_RESULT> KRENAME_RESULT_QUEUE;
    KRENAME_RESULT_QUEUE    m_RenameResultQueue;
    KMutex                  m_RenameResultQueueLock;

    BOOL        GetRenameTask(KRENAME_REQUEST* pTask);
    BYTE        Rename(DWORD dwRoleID, char szNewName[]);

    BOOL        DoQuery(const char cszSQL[]);
};
#endif
