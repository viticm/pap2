#ifndef _KAPEX_PROXY_H_
#define _KAPEX_PROXY_H_

#include <map>

#include "SO3GlobalDef.h"
#include "ProtocolBasic.h"
#include "Engine/KMemory.h"

#define MAX_KICK_COUNT      3
#define KICK_REFRESH_TIME   3600

#define KICK_ACTION_ID_NO_DLL       47054
#define KICK_ACTION_ID_LOAD_ERROR   47200

#pragma pack(1)
struct APEX_PROTOCOL_HEADER
{
    char     cMsgId;
    uint32_t nSendId;
};

struct APEX_PING_SIGNAL : APEX_PROTOCOL_HEADER
{
};

struct APEX_ROLE_LOGIN : APEX_PROTOCOL_HEADER
{
    char szName[_NAME_LEN];
};

struct APEX_ROLE_LOGOUT : APEX_PROTOCOL_HEADER
{
    char szName[_NAME_LEN];
};

struct APEX_SYNC_ROLE_IPADDR : APEX_PROTOCOL_HEADER
{
    BYTE byUserIPFlag;
    int  nIPAddr;
};

struct APEX_CLIENT_RETCODE : APEX_PROTOCOL_HEADER
{
    int nRetCode;
};

struct APEX_SYNC_USER_DATA : APEX_PROTOCOL_HEADER 
{
    BYTE byData[0];
};

struct APEX_KICK_ROLE : APEX_PROTOCOL_HEADER
{
    int nActionType;
};

struct APEX_SWITCH_SERVER : APEX_PROTOCOL_HEADER
{
    int nGS_Index;
    int nGS_State;
};
#pragma pack()

class KApexProxy
{
public:
    KApexProxy();

    BOOL Init();
    void UnInit();

    BOOL Connect(const char szIP[], int nPort);
    void Disconnect();

    BOOL Breathe();

    BOOL Send(IKG_Buffer* piBuffer);

private:
    IKG_SocketStream*   m_piSocketStream;
    BOOL                m_bSendErrorFlag;
    time_t              m_nLastPingTime;

public:
    //////////////////////////////////////////////////////////////////////////
    BOOL DoPingSignal();

    BOOL DoRoleLogin(DWORD dwRoleID);
    BOOL DoRoleLogout(DWORD dwRoleID);

    BOOL DoSyncRoleIP(DWORD dwRoleID, int nIPAddr);

    BOOL DoReturnClientRet(DWORD dwRoleID, int nRetCode);

    BOOL DoSyncUserData(DWORD dwRoleID, BYTE* pbyData, size_t uDataSize);

    BOOL DoEntryServer(DWORD dwRoleID, int nGSIndex);
    BOOL DoLeaveServer(DWORD dwRoleID, int nGSIndex);

private:
    //////////////////////////////////////////////////////////////////////////
    void ProcessRecvData(BYTE* pbyData, size_t uDataSize);

    void OnRecvUserData(BYTE* pbyData, size_t uDataSize);
    void OnKickRole(BYTE* pbyData, size_t uDataSize);

private:
    struct KROLE_KICK_NODE
    {
        time_t  nLastRefreshTime;
        int     nKickCount;
    };

    typedef KMemory::KAllocator<std::pair<DWORD, KROLE_KICK_NODE> > KICK_NODE_ALLOC;
    typedef std::map<DWORD, KROLE_KICK_NODE, less<DWORD>, KICK_NODE_ALLOC> KICK_TABLE;

    KICK_TABLE m_KickTable;

    void ClearTimeoutKickNode();
};

#endif
