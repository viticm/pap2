#ifndef _KMONSTER_H_
#define _KMONSTER_H_

#include "Engine/KThread.h"
#include "KBishopClient.h"
#include "KPlayerClient.h"
#include "KPlayerEyeshot.h"
#include "IKOperationSimulator.h"

class KPlayer;
class KNpc;
class KDoodad;

enum ONLINE_STATE
{
    osBishopOnline,
    osBishopOffline,
    osGameserverOnline,
    osGameserverOffline,
    osTotal
};

enum ARRANGEMENT
{
    hpRandom,
    hpRegular,
    hpUnchange,
    hpTotal
};

typedef struct _MONSTER_PARAM
{
    int  nMonsterTotal;
    int  nSerialNumber;
    int  nCreateRoleSN;

    char szAccountName[_NAME_LEN];
    char szPassword[_NAME_LEN];
    char szBishopIP[_NAME_LEN];
    int  nBishopPort;

    ARRANGEMENT  nArrangement;
    int nRange;
    int nRunRate;

    int nReconnect;
}MONSTER_PARAM;

class KMonster
{
public:
    KMonster(void);
    ~KMonster(void);

    int Init(MONSTER_PARAM &rParam);
    int UnInit();

    void SetExitFlag(){ m_nExitFlag = TRUE; }
    int  GetExitFlag(){ return m_nExitFlag; }
    ONLINE_STATE GetOnlineState(){ return m_nOnlineState; }
    const char *GetStateDescription();
    const char *GetFailDescription();

public:
    MONSTER_PARAM m_MonsterParam;

public:
    KBishopClient  m_BishopClient;
    KPlayerClient  m_PlayerClient;

public:
    int   m_nGameLoop;
    DWORD m_dwClientMapID;
    DWORD m_dwClientMapCopyIndex;
    KPlayer *m_pRole;

public:
    KPlayerEyeshot m_Eyeshot;

public:
    IKOperationSimulator *m_piOperator;

private:
    enum LOGIN_TYPE {ltConnectTo,ltReconnectTo};
    int Login(LOGIN_TYPE nLoginType);
    int GameLoop();
    int Activate();
    int Reset();

private:
    KThread     m_WorkThread;
    int         m_nThreadFlag;
    int         m_nExitFlag;
    int         ThreadFunction(void);
    static void WorkThreadFunction(void *pvParam);

private:    
    ONLINE_STATE m_nOnlineState;
    static char ms_szStateDescription[osTotal][256];
};


#endif //_KMONSTER_H_