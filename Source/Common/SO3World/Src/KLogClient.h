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

#define MAX_LOG_BUFFER_SIZE 1024

class KItem;
struct KTeam;

struct KITEM_LOG_INFO 
{
    DWORD       dwItemID;
    char        szItemName[_NAME_LEN];
    unsigned    uItemValue;
    char        szRestoreData[128];
};

class KLogClient
{
public:
    KLogClient();

    BOOL Init();
    void UnInit();

    void Activate();

    BOOL LogAcceptQuest(
        DWORD dwQuestID, const char* pszQuestName, int nQuestLevel, const char* pszAccount, const char* pszRoleName
    );
    BOOL LogFinishQuest(
        DWORD dwQuestID, const char* pszQuestName, int nQuestLevel, const char* pszAccount, const char* pszRoleName
    );
    BOOL LogCancelQuest(
        DWORD dwQuestID, const char* pszQuestName, int nQuestLevel, const char* pszAccount, const char* pszRoleName
    );
    BOOL LogPlayerLevelup(int nLevel, const char* pszAccount, const char* pszRoleName);
    BOOL LogTrainvalueChange(int nAddValue, const char* pszAccount, const char* pszRoleName);
    BOOL LogPlayerLogin(const char cszAccount[], const char cszRoleName[], const char cszIP[]);
    BOOL LogPlayerLogout(const char cszAccount[], const char cszRoleName[]);
    
    BOOL GetItemLogInfo(KItem* pItem, KITEM_LOG_INFO* pItemLogInfo);
    BOOL LogPlayerItemChange(
        KPlayer* pPlayer, ITEM_OPERATION_TYPE eOperationType, KITEM_LOG_INFO* pItemLogInfo, 
        const char* pszComment, const char* pszCommentExt = NULL
    );
    BOOL LogPlayerItemChange(
        KPlayer* pPlayer, ITEM_OPERATION_TYPE eOperationType, KItem* pItem, 
        const char* pszComment, const char* pszCommentExt = NULL
    );
    
    BOOL LogPlayerMoneyChange(
        int nChangeAmount, KPlayer* pPlayer, const char* pszComment, const char* pszCommentExt = NULL
    );

    BOOL LogPlayerJoinTeam(KPlayer* pPlayer, KTeam* pTeam);
    BOOL LogPlayerLeaveTeam(KPlayer* pPlayer);

    BOOL LogPlayerAddFriend(KPlayer* pPlayer, DWORD dwFriendID, const char cszFriendName[]);
    BOOL LogPlayerDelFriend(KPlayer* pPlayer, DWORD dwFriendID, const char cszFriendName[]);
    BOOL LogPlayerAddFoe(KPlayer* pPlayer, DWORD dwFoeID, const char cszFoeName[]);
    BOOL LogPlayerDelFoe(KPlayer* pPlayer, DWORD dwFoeID, const char cszFoeName[]);

private:
    BOOL LoginServer(const char cszIP[], int nPort, const char cszGateway[]);

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

extern KLogClient g_LogClient;

#endif // _SERVER

#endif // _KLOG_CLIENT_H_
