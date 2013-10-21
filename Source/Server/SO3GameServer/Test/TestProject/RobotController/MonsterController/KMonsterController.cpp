#include "KMonsterController.h"
#include <conio.h>
#include "engine\FileType.h"
#include <direct.h>

KMonsterControllerMgr g_CtrlMgr;
KMonsterControllerMgr::KMonsterControllerMgr()
{
    m_nExitFlag = FALSE;
    m_nThreadFlag = FALSE;
	memset(&m_MonsterInitData, 0, sizeof(KS2C_Init));
}


KMonsterControllerMgr::~KMonsterControllerMgr()
{

}

int KMonsterControllerMgr::Init()
{
    int nRetCode = FALSE;
    int nResult = FALSE;

	nRetCode = SetMonsterInitData();
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = m_WorkThread.Create(WorkThreadFunction, (void *)this);
    KG_PROCESS_ERROR(nRetCode);

    nResult = TRUE;
Exit0:
    return nResult;
}

int KMonsterControllerMgr::UnInit()
{
    m_nExitFlag = TRUE;
    m_WorkThread.Destroy();

    for (int i = 0; i < (int)m_MonsterControllerList.size(); ++i)
    {
        KMonsterController *pCtrller = NULL;
        pCtrller = m_MonsterControllerList[i];
        if (pCtrller)
        {
            pCtrller->Exit();
            pCtrller->UnInit();

            delete pCtrller;
            pCtrller = NULL;
        }
    }

    return TRUE;
}

void KMonsterControllerMgr::WorkThreadFunction(void *pvParam)
{
    KMonsterControllerMgr *pThis = (KMonsterControllerMgr *)pvParam;
    pThis->ThreadFunction();
}

int KMonsterControllerMgr::ThreadFunction()
{
    int nRetCode = FALSE;
    int nResult = FALSE;

    KG_SocketAcceptor Acceptor;
    int nAcceptorFlag = FALSE;
    timeval Timeout = {30, 0};

    static int nIndex = 0;

    nRetCode = Acceptor.Open(m_Param.szListenIP, m_Param.nListenPort);
    KG_PROCESS_ERROR(nRetCode);
    nAcceptorFlag = TRUE;

    nRetCode = Acceptor.SetTimeout(&Timeout);
    KG_PROCESS_ERROR(nRetCode);

    while (!m_nExitFlag)
    {
        IKG_SocketStream *piSocket = NULL;
        KMonsterController *pCtrller = NULL;
        piSocket = Acceptor.AcceptSecurity(KSG_ENCODE_DECODE);
        if (piSocket)
        {
            pCtrller = new KMonsterController();
            if (pCtrller)
            {
                m_MonsterControllerList.push_back(pCtrller);
                pCtrller->Init(piSocket, nIndex++, this);

                printf("Monster Connect %d, %s\n", m_MonsterControllerList.size(), pCtrller->GetAddress());
            }
        }
    }

    nResult = TRUE;
Exit0:
    if (nAcceptorFlag)
    {
        Acceptor.Close();
        nAcceptorFlag = FALSE;
    }
    return nResult;
}

int KMonsterControllerMgr::CreatMonsterSmart(int nCount,int nCpuLimit)
{
	int nRetCode = FALSE;
	int nResult  = FALSE;
	bool bAllBusy = true;

	KMonsterController *pCtrller = NULL;
    printf("Press Q to cancel\n");

	while ( m_nTotalCounter < nCount)
	{
		bAllBusy = true;
		CONTROLLER_VECT::iterator it;
		for (it = m_MonsterControllerList.begin(); it != m_MonsterControllerList.end(); ++it)
		{
			int nCurrent = m_nTotalCounter;
			pCtrller = *it;
			KG_PROCESS_ERROR(pCtrller);
			if ( pCtrller->GetStatisticData().nCpuUsage > nCpuLimit)
			{
				continue;
			}

			bAllBusy = false;
			nRetCode = pCtrller->CreatMonster(30);
			KG_PROCESS_ERROR(nRetCode);

			while (m_nTotalCounter < (nCurrent + 25))
			{
				Sleep(1000);
				Statistic();
				QueryMonsterList();
				if (kbhit() && getch() == 'q')
				{
					printf("\ncancel create monster\n");
					goto Exit0;
				}
			}
			if ( m_nTotalCounter > nCount)
			{
				break;
			}
		}
		if ( bAllBusy )
		{
			printf("\nAll client cpu is high than limit\n");
			break;
		}
	}
	nResult = TRUE;
Exit0:
	return nResult;
}

int KMonsterControllerMgr::CreatMonster(int nMonsterIdx, int nCount)
{
    int nRetCode = FALSE;
    int nResult  = FALSE;
    KMonsterController *pCtrller = NULL;

    if (nMonsterIdx >= 0 && nMonsterIdx < (int)m_MonsterControllerList.size())
    {
        pCtrller = m_MonsterControllerList[nMonsterIdx];
        KG_PROCESS_ERROR(pCtrller);
        nRetCode = pCtrller->CreatMonster(nCount);
        KG_PROCESS_ERROR(nRetCode);
    }
    else if (nMonsterIdx == -1)
    {
        CONTROLLER_VECT::iterator it;
        for (it = m_MonsterControllerList.begin(); it != m_MonsterControllerList.end(); ++it)
        {
            pCtrller = *it;
            KG_PROCESS_ERROR(pCtrller);
            nRetCode = pCtrller->CreatMonster(nCount);
            KG_PROCESS_ERROR(nRetCode);
        }
    }

    nResult = TRUE;
Exit0:
    return nResult;
}

int KMonsterControllerMgr::DestroyMonster(int nMonsterIdx, int nCount)
{
    int nRetCode = FALSE;
    int nResult  = FALSE;
    KMonsterController *pCtrller = NULL;

    if (nMonsterIdx >= 0 && nMonsterIdx < (int)m_MonsterControllerList.size())
    {
        pCtrller = m_MonsterControllerList[nMonsterIdx];
        KG_PROCESS_ERROR(pCtrller);
        nRetCode = pCtrller->DestroyMonster(nCount);
        KG_PROCESS_ERROR(nRetCode);
    }
    else if (nMonsterIdx == -1)
    {
        CONTROLLER_VECT::iterator it;
        for (it = m_MonsterControllerList.begin(); it != m_MonsterControllerList.end(); ++it)
        {
            pCtrller = *it;
            KG_PROCESS_ERROR(pCtrller);
            nRetCode = pCtrller->DestroyMonster(nCount);
            KG_PROCESS_ERROR(nRetCode);
        }
    }

    nResult = TRUE;
Exit0:
    return nResult;
}

int KMonsterControllerMgr::DestroyAllMonster(int nMonsterIdx)
{
    int nRetCode = FALSE;
    int nResult  = FALSE;
    KMonsterController *pCtrller = NULL;

    if (nMonsterIdx >= 0 && nMonsterIdx < (int)m_MonsterControllerList.size())
    {
        pCtrller = m_MonsterControllerList[nMonsterIdx];
        KG_PROCESS_ERROR(pCtrller);
        nRetCode = pCtrller->DestroyAllMonster();
        KG_PROCESS_ERROR(nRetCode);
    }
    else if (nMonsterIdx == -1)
    {
        CONTROLLER_VECT::iterator it;
        for (it = m_MonsterControllerList.begin(); it != m_MonsterControllerList.end(); ++it)
        {
            pCtrller = *it;
            KG_PROCESS_ERROR(pCtrller);
            nRetCode = pCtrller->DestroyAllMonster();
            KG_PROCESS_ERROR(nRetCode);
        }
    }

    nResult = TRUE;
Exit0:
    return nResult;
}

int KMonsterControllerMgr::QueryMonsterList()
{
    int nRetCode = FALSE;
    int nIdx = 0;
    KStatisticData StatData = {0, 0, 0};
    KMonsterController *pCtrller = NULL;
    CONTROLLER_VECT::iterator it;
    printf("\nID\tIP\tOnline\tOffline\tLogin\tCpuTime\t\n");
    for (it = m_MonsterControllerList.begin(); it != m_MonsterControllerList.end(); ++it)
    {
        pCtrller = *it;
        StatData = pCtrller->GetStatisticData();
        printf("%d\t%s\t%d\t%d\t%d\t%d%%\t\n", nIdx++, pCtrller->GetAddress(),
            StatData.nOnlineCount, StatData.nOfflineCount, StatData.nLoginCount,
            StatData.nCpuUsage);
    }

    return TRUE;
}

int KMonsterControllerMgr::Statistic()
{
    KStatisticData StaData = {0, 0, 0};
    KMonsterController *pCtrller = NULL;

    for (int i = 0; i < (int)m_MonsterControllerList.size(); ++i)
    {
        KStatisticData TempData = {0, 0, 0};
        pCtrller = m_MonsterControllerList[i];
        if (pCtrller)
        {
            TempData = pCtrller->GetStatisticData();
            StaData.nLoginCount += TempData.nLoginCount;
            StaData.nOfflineCount += TempData.nOfflineCount;
            StaData.nOnlineCount += TempData.nOnlineCount;
        }
    }
    
	m_nTotalCounter = StaData.nOnlineCount + StaData.nOfflineCount;
    printf(
        "\n\t|Online  |\t%d|\n"
        "\t|Offline |\t%d|\n"
        "\t|Login   |\t%d|\n",
        StaData.nOnlineCount, StaData.nOfflineCount, StaData.nLoginCount
        );

    return TRUE;
}

int KMonsterControllerMgr::OnDisconnect(KMonsterController *pCtrller)
{
    int nRetCode = FALSE;
    int nResult  = FALSE;

    m_Lock.Lock();

    CONTROLLER_VECT::iterator it;
    for (it = m_MonsterControllerList.begin(); it != m_MonsterControllerList.end(); ++it)
    {
        if (pCtrller == *it)
        {
            m_MonsterControllerList.erase(it);
            printf("Monster Disconnect %d, %s\n", m_MonsterControllerList.size(), pCtrller->GetAddress());         
            break;
        }
    }

    nResult = TRUE;
    m_Lock.Unlock();
    return nResult;
}

int KMonsterControllerMgr::SetMonsterInitData()
{
	int nRetCode = FALSE;
	int nResult  = FALSE;
	char szCurretDir[MAX_PATH];

	IIniFile* pfileMonsterInitData = NULL;
	_getcwd(szCurretDir, MAX_PATH);
	strcat_s(szCurretDir, MONSTERINITFILE);
	pfileMonsterInitData = g_OpenIniFile(szCurretDir);
	KG_PROCESS_ERROR(pfileMonsterInitData);
    
	nRetCode = pfileMonsterInitData->GetString("Controller", "ListenIP", "", m_Param.szListenIP, sizeof(m_Param.szListenIP));
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = pfileMonsterInitData->GetInteger("Controller", "ListenPort", 5555, &m_Param.nListenPort);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = pfileMonsterInitData->GetString("Bishop", "IP", "", m_MonsterInitData.szBishopIP, _NAME_LEN);
	KG_PROCESS_ERROR(nRetCode);
	
	nRetCode = pfileMonsterInitData->GetInteger("Bishop", "Port", 5622, &m_MonsterInitData.nBishopPort);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = pfileMonsterInitData->GetString("Account", "AccountNamePrefix", "", m_MonsterInitData.szAccountNamePrefix, _NAME_LEN);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = pfileMonsterInitData->GetString("Account", "Password", "", m_MonsterInitData.szPassword, _NAME_LEN);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = pfileMonsterInitData->GetInteger("Account", "StartIndex", 1, &m_MonsterInitData.nStartIndex);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = pfileMonsterInitData->GetInteger("Monster", "Reconnect", 1, &m_MonsterInitData.nReconnect);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = pfileMonsterInitData->GetInteger("Monster", "Arrangement", 1, &m_MonsterInitData.nArrangement);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = pfileMonsterInitData->GetInteger("Monster", "Range", 256, &m_MonsterInitData.nRange);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = pfileMonsterInitData->GetInteger("Monster", "RunRate", 8, &m_MonsterInitData.nRunRate);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = pfileMonsterInitData->GetInteger("Monster", "StartInterval", 500, &m_MonsterInitData.nStartInterval);
	KG_PROCESS_ERROR(nRetCode);
	
    m_MonsterInitData.byProtocolID = S2C_Init;

	nResult = TRUE;
Exit0:
	KG_COM_RELEASE(pfileMonsterInitData);
	if (!nResult)
	{
       printf("read setting file:%s fail\n",szCurretDir);
	}
	return nResult;
}

KS2C_Init* KMonsterControllerMgr::GetMonsterInitData()
{
	return &m_MonsterInitData;
}

int KMonsterControllerMgr::ReInitMonster()
{
	int nRetCode = FALSE;
	int nResult  = FALSE;
    KS2C_Init*  pKs2c_init = NULL;

	nRetCode = SetMonsterInitData();
	KG_PROCESS_ERROR(nRetCode);

	for (int i = 0; i < (int)m_MonsterControllerList.size(); i++)
	{
		m_MonsterControllerList[i]->MonsterInit(&m_MonsterInitData);
	}

	nResult = TRUE;
Exit0:
	return nResult;
    
}
//================================================================================================

KMonsterController::KMonsterController()
{
    m_piSocket = NULL;
    m_pMgr = NULL;
    m_nIndex = -1;
    m_nExitFlag = FALSE;
    memset(&m_StatisticData, 0, sizeof(m_StatisticData));
}

KMonsterController::~KMonsterController()
{

}

int KMonsterController::Init(IKG_SocketStream *piSocket, int nIndex, KMonsterControllerMgr *pMgr)
{
    int nRetCode = FALSE;
    int nResult  = FALSE;
    struct in_addr IpAddress;

    KG_PROCESS_ERROR(piSocket);
    m_piSocket = piSocket;
    m_nIndex = nIndex;
    m_pMgr = pMgr;

    MonsterInit(pMgr->GetMonsterInitData());

    nRetCode = m_piSocket->GetRemoteAddress(&IpAddress, NULL);
    KG_PROCESS_ERROR(nRetCode);
    strcpy_s(m_szIP, inet_ntoa(IpAddress));

    nRetCode = m_WorkThread.Create(WorkThreadFunction, (void *)this);
    KG_PROCESS_ERROR(nRetCode);

    nResult = TRUE;
Exit0:
    return nResult;
}

int KMonsterController::UnInit()
{
    m_WorkThread.Destroy();
    KG_COM_RELEASE(m_piSocket);
    return TRUE;
}

void KMonsterController::WorkThreadFunction(void *pvParam)
{
    KMonsterController *pThis = (KMonsterController *)pvParam;
    pThis->ThreadFunction();
}

int KMonsterController::ThreadFunction()
{
    int nRetCode = FALSE;
    int nResult  = FALSE;
    IKG_Buffer *piBuffer = NULL;
    unsigned char *pbyBuffer = NULL;

    while (!m_nExitFlag)
    {        
        nRetCode = m_piSocket->Recv(&piBuffer);
        KG_PROCESS_ERROR(nRetCode != -1);

        if (nRetCode == 1)
        {
            pbyBuffer = (unsigned char *)piBuffer->GetData();
            switch (*pbyBuffer)
            {
            case C2S_Statistic:
                {
                    KC2S_Statistic *pData = (KC2S_Statistic *)(pbyBuffer);
                    m_StatisticData.nLoginCount   = pData->nLoginCount;
                    m_StatisticData.nOfflineCount = pData->nOfflineCount;
                    m_StatisticData.nOnlineCount  = pData->nOnlineCount;

                    m_StatisticData.nCpuIdle = pData->nCpuIdle;
                    m_StatisticData.nCpuUsage = pData->nCpuUsage;
                }
                break;
            default:
                break;
            }
        } 
        KG_COM_RELEASE(piBuffer);
    }

    nResult = TRUE;
Exit0:
    if (!nResult)
    {
        if (m_pMgr)
            m_pMgr->OnDisconnect(this);
    }
    KG_COM_RELEASE(piBuffer);
    return nResult;
}

int KMonsterController::MonsterInit(KS2C_Init* pMonsterInitData)
{
    int nRetCode = FALSE;
    int nResult  = FALSE;
    IKG_Buffer *piBuffer = NULL;
    KS2C_Init *pInitData = NULL;    

    piBuffer = KG_MemoryCreateBuffer(sizeof(KS2C_Init));
    KG_PROCESS_ERROR(piBuffer);
    pInitData = (KS2C_Init *)piBuffer->GetData();
    
	KG_PROCESS_ERROR(pMonsterInitData);
	memcpy(pInitData, pMonsterInitData, sizeof(KS2C_Init));
	sprintf_s(pInitData->szAccountNamePrefix, "%s%d_%d_", pMonsterInitData->szAccountNamePrefix, m_nIndex, KG_Rand() % 256);

    nRetCode = m_piSocket->Send(piBuffer);
    KG_PROCESS_ERROR(nRetCode == 1);

    nResult = TRUE;
Exit0:
    if (!nResult)
    {
        if (m_pMgr)
            m_pMgr->OnDisconnect(this);
    }
    KG_COM_RELEASE(piBuffer);
    return nResult;
}

int KMonsterController::CreatMonster(int nCount)
{
    int nRetCode = FALSE;
    int nResult  = FALSE;
    IKG_Buffer *piBuffer = NULL;
    KS2C_CreateMonster *pCreateData = NULL;    

    KG_PROCESS_ERROR(nCount > 0);

    piBuffer = KG_MemoryCreateBuffer(sizeof(KS2C_CreateMonster));
    KG_PROCESS_ERROR(piBuffer);
    pCreateData = (KS2C_CreateMonster *)piBuffer->GetData();

    pCreateData->byProtocolID = S2C_CreateMonster;
    pCreateData->nCount = nCount;

    nRetCode = m_piSocket->Send(piBuffer);
    KG_PROCESS_ERROR(nRetCode == 1);

    nResult = TRUE;
Exit0:
    if (!nResult)
    {
        if (m_pMgr)
            m_pMgr->OnDisconnect(this);
    }
    KG_COM_RELEASE(piBuffer);
    return nResult;
}

int KMonsterController::DestroyMonster(int nCount)
{
    int nRetCode = FALSE;
    int nResult  = FALSE;
    IKG_Buffer *piBuffer = NULL;
    KS2C_DestroyMonster *pDestroyData = NULL;    

    KG_PROCESS_ERROR(nCount > 0);

    piBuffer = KG_MemoryCreateBuffer(sizeof(KS2C_DestroyMonster));
    KG_PROCESS_ERROR(piBuffer);
    pDestroyData = (KS2C_DestroyMonster *)piBuffer->GetData();

    pDestroyData->byProtocolID = S2C_DestroyMonster;
    pDestroyData->nCount = nCount;

    nRetCode = m_piSocket->Send(piBuffer);
    KG_PROCESS_ERROR(nRetCode == 1);

    nResult = TRUE;
Exit0:
    if (!nResult)
    {
        if (m_pMgr)
            m_pMgr->OnDisconnect(this);
    }
    KG_COM_RELEASE(piBuffer);
    return nResult;
}

int KMonsterController::DestroyAllMonster()
{
    int nRetCode = FALSE;
    int nResult  = FALSE;
    IKG_Buffer *piBuffer = NULL;
    KS2C_DestroyAllMonster *pDestroyData = NULL;    

    piBuffer = KG_MemoryCreateBuffer(sizeof(KS2C_DestroyAllMonster));
    KG_PROCESS_ERROR(piBuffer);
    pDestroyData = (KS2C_DestroyAllMonster *)piBuffer->GetData();

    pDestroyData->byProtocolID = S2C_DestroyAllMonster;

    nRetCode = m_piSocket->Send(piBuffer);
    KG_PROCESS_ERROR(nRetCode == 1);

    nResult = TRUE;
Exit0:
    if (!nResult)
    {
        if (m_pMgr)
            m_pMgr->OnDisconnect(this);
    }
    KG_COM_RELEASE(piBuffer);
    return nResult;
}
