#ifndef _KMONSTER_CONTROLLER_H_
#define _KMONSTER_CONTROLLER_H_

#include <vector>
#include <map>

#include "Common/KG_Socket.h"
#include "Common/KG_Memory.h"
#include "Engine/KThread.h"
#include "KGPublic.h"
#include "Engine/KMutex.h"
#include "MonsterProtocol.h"

#define MONSTERINITFILE "\\MonsterInitData.ini"
struct KCtrllerMgrParam
{
    char szListenIP[32];
    int  nListenPort;
};

struct KStatisticData
{
    int nOnlineCount;
    int nOfflineCount;
    int nLoginCount;

    int nCpuIdle;
    int nCpuUsage;
};

class KMonsterController;

class KMonsterControllerMgr
{
public:
    KMonsterControllerMgr();
    ~KMonsterControllerMgr();

    int Init();
    int UnInit();   

    int CreatMonsterSmart(int nCount,int nCpuLimit);
    int CreatMonster(int nMonsterIdx, int nCount);
    int DestroyMonster(int nMonsterIdx, int nCount);
    int DestroyAllMonster(int nMonsterIdx);

    int QueryMonsterList();
    int Statistic();
    
	KS2C_Init* GetMonsterInitData();
	int ReInitMonster();
    int OnDisconnect(KMonsterController *pCtrller);

private:
    KThread     m_WorkThread;
    int         m_nThreadFlag;
    int         m_nExitFlag;
	int         m_nTotalCounter;
	int         SetMonsterInitData();
    int         ThreadFunction();
    static void WorkThreadFunction(void *pvParam);

private:
	typedef std::vector<KMonsterController *> CONTROLLER_VECT;
    CONTROLLER_VECT  m_MonsterControllerList;
    KCtrllerMgrParam m_Param;
    KS2C_Init        m_MonsterInitData;
	
    KMutex m_Lock;
};

extern KMonsterControllerMgr g_CtrlMgr;

class KMonsterController
{
public:
    KMonsterController();
    ~KMonsterController();

    int Init(IKG_SocketStream *piSocket, int nIndex, KMonsterControllerMgr *pMgr);
    int UnInit();
    int Exit(){m_nExitFlag = TRUE;return TRUE;};

    char *GetAddress(){return m_szIP;};
    KStatisticData &GetStatisticData(){return m_StatisticData;}
    
    int CreatMonster(int nCount);
    int DestroyMonster(int nCount);
    int DestroyAllMonster();
    int MonsterInit(KS2C_Init* pMonsterInitData);

private:
    KThread     m_WorkThread;
    int         m_nExitFlag;
    int         ThreadFunction();
    static void WorkThreadFunction(void *pvParam);
private:
    int m_nIndex;
    IKG_SocketStream *m_piSocket;
    char m_szIP[32];

    KStatisticData m_StatisticData;

    KMonsterControllerMgr *m_pMgr;    
};

#endif //_KMONSTER_CONTROLLER_H_