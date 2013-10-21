/************************************************************************/
/* 游戏世界服务器和LogServer的连接	                                    */
/* Copyright : Kingsoft 2009										    */
/* Author	 : yanrui   												*/
/* History	 :															*/
/*		2009.02.10	Create												*/
/************************************************************************/

#ifndef _KLOG_CLIENT_H_
#define _KLOG_CLIENT_H_

#ifdef _SERVER
#include "Common/KG_Socket.h"
#include "Protocol/KG_CSLogServerProtocal.h"

class KLogClient
{
public:
    KLogClient();
    ~KLogClient();

    BOOL Init();
    void UnInit();

    void Activate();

    BOOL LogAcceptQuest(
        const char* pszQuestName, int nQuestLevel, const char* pszAccount, const char* pszRoleName
    );

    BOOL LogFinishQuest(
        const char* pszQuestName, int nQuestLevel, const char* pszAccount, const char* pszRoleName
    );

    BOOL LogCancelQuest(
        const char* pszQuestName, int nQuestLevel, const char* pszAccount, const char* pszRoleName
    );

    BOOL LogPlayerLevelup(int nLevel, const char* pszAccount, const char* pszRoleName);

    BOOL LogTrainvalueChange(int nAddValue, const char* pszAccount, const char* pszRoleName);

    BOOL LogPlayerLogin(const char* pszAccount, const char* pszRoleName);

    BOOL LogPlayerLogout(const char* pszAccount, const char* pszRoleName);

private:
    BOOL LoginServer(const char cszIP[], int nPort, const char cszGateway[]);

    BOOL Ping();

    BOOL LogPlayerAction(
        PLAYER_ACTION_TYPE eActionType, unsigned uActionLevel, const char* pszActionTarget,
        const char* pszAccountName, const char* pszRoleName, size_t uCommentSize,
        BYTE* pbyComment
    );

    BOOL Send(IKG_Buffer* piBuffer);

private:
    IKG_SocketStream* m_piSocket;
    int               m_nPingCycle;
    time_t            m_nNextPingTime;
};

extern KLogClient g_LogClient;

#endif // _SERVER

#endif // _KLOG_CLIENT_H_
