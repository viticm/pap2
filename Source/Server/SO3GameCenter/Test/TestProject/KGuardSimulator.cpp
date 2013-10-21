#include "StdAfx.h"
#include "KGuardSimulator.h"
#include "KSO3GameCenter.h"
#include "FSEyes/fseye_protocol.h"
#include <time.h>

#define  TIMES 15

KGuardSimulator::KGuardSimulator()
{
}

KGuardSimulator::~KGuardSimulator()
{
}

BOOL KGuardSimulator::Init()
{
    BOOL bResult            = false;
    BOOL bRetCode           = false;
    BOOL bStreamInitFlag    = false;


    bRetCode = m_Stream.Initialize(emKGUARDSERVER_FIRSTID + emKGUARDSERVER_GAMECENTER, 200 * 1024, true);
    KGLOG_PROCESS_ERROR(bRetCode);
    bStreamInitFlag = true;

    m_RunFlag = true;

    bRetCode = m_Thread.Create(WorkFucntion, this);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    if (!bResult && bStreamInitFlag)
    {
        m_Stream.UnInitialize();
        bStreamInitFlag = false;
    }
    return bResult;
}

void KGuardSimulator::UnInit()
{
    m_RunFlag = false;

    m_Thread.Destroy();

    m_Stream.UnInitialize();

    for (int  i = 0; i < UCHAR_MAX + 1; i++)
    {
        for (int j = 0; j < UCHAR_MAX + 1; j++)
        {
            if (m_ReceiveDataCach[i][j].pData)
            {
                delete[] m_ReceiveDataCach[i][j].pData;
                m_ReceiveDataCach[i][j].pData =  NULL;
            }
        }
    }
}

void KGuardSimulator::DoE2LStop()
{
    KProtocolHead kProtocolHead;
    kProtocolHead.wProtocol = e2l_exit_def;
    kProtocolHead.wServer = 0;
    m_Stream.PutPack(&kProtocolHead, sizeof(kProtocolHead));
}

BOOL KGuardSimulator::CheckE2LStop(HANDLE hProcess, DWORD dwWaitTime)
{
    //if (!hProcess)
    //    return false;

    //DWORD dwReuslt = WaitForSingleObject(hProcess, dwWaitTime);
    //if (dwReuslt == WAIT_OBJECT_0)
    //    return true;
    return true;
}

void KGuardSimulator::DoE2LGetPid()
{
    KProtocolHead kProtocolHead;
    kProtocolHead.wProtocol = e2l_getpid_def;
    kProtocolHead.wServer = 0;
    m_Stream.PutPack(&kProtocolHead, sizeof(kProtocolHead));
}

BOOL KGuardSimulator::CheckE2LGetPid(int nProcessID, time_t* pTime)
{
    int nResult = false;
    int nRetCode = false;
    l2e_getpid* pMsg = NULL;
    BYTE* pData = NULL;

    pData = GetData(l2e_getpid_def, 0, pTime);
    KG_PROCESS_ERROR(pData);

    pMsg = (l2e_getpid*)pData;

    KG_PROCESS_ERROR(pMsg->nPid == nProcessID);

    nResult = true;
Exit0:
    return nResult;
}

void KGuardSimulator::DoG2LProtocol()
{
    KSubProtocolHead ProtocolHead;
    ProtocolHead.wProtocol = g2l_header_def;
    ProtocolHead.wSubProtocol = emKPROTOCOL_G2L_PIDREQ;
    ProtocolHead.wServer = 0;
    m_Stream.PutPack(&ProtocolHead, sizeof(ProtocolHead));
}

BOOL KGuardSimulator::CheckG2LProtocol(int nProcessID, time_t* pTime)
{
    int nRetCode = false;
    int nResult = false;
    KPROTOCOL_L2G_PIDRET* pPidRet = NULL;
    BYTE* pData = NULL;

    pData = GetData(l2g_header_def, emKPROTOCOL_L2G_PIDRET, pTime);
    KG_PROCESS_ERROR(pData);

    pPidRet = (KPROTOCOL_L2G_PIDRET*)pData;
    KG_PROCESS_ERROR(pPidRet);
    KG_PROCESS_ERROR(pPidRet->nPid == nProcessID);

    nResult = true;
Exit0:
    return nResult;
}

BOOL KGuardSimulator::CheckPlayerCount(int nPlayerCount, time_t* pTime)
{
    int nResult = false;
    int nRetCode = false;
    l2e_PlayerCount*    pProtocol = NULL;
    BYTE* pData = NULL;

    pData = GetData(l2e_header_def, l2e_PlayerCount_def, pTime);
    KG_PROCESS_ERROR(pData);

    pProtocol = (l2e_PlayerCount*)pData;

    int* pnCount = (int*)(pProtocol + 1);

    KG_PROCESS_ERROR(pnCount);
    KG_PROCESS_ERROR(pnCount[emKGUARD_PLAYERCOUNT_TOTOAL] == nPlayerCount);

    nResult = true;
Exit0:
    return nResult;
}

BYTE* KGuardSimulator::GetData(BYTE byProtocolID, BYTE bySubProtocolID /* = 0 */, time_t* pTime /* = NULL */)
{
    int nResult = false;
    int nRetCode = false;

    time_t ReceiveTime = pTime ? *pTime : time(NULL);

    for (int i = 0 ; i < TIMES; i++)
    {
        if (m_ReceiveDataCach[byProtocolID][bySubProtocolID].tReceiveTime >= ReceiveTime)
        {
            return m_ReceiveDataCach[byProtocolID][bySubProtocolID].pData;
        }
        KGThread_Sleep(100);
    }
    return NULL;
}

void KGuardSimulator::WorkFucntion(void *pvParam)
{
    KGuardSimulator* pGuardSimulator = (KGuardSimulator*)pvParam;
    pGuardSimulator->ThreadFunction();
}

void KGuardSimulator::ThreadFunction()
{
    size_t              nDataLen            = 0;
    char*               pData               = NULL;
    BYTE                byProtocol          = 0;

    while (m_RunFlag)
    {
        KGThread_Sleep(10);

        pData = m_Stream.GetPack(nDataLen);

        if (pData == NULL)
        {
            continue;
        }

        ASSERT(nDataLen > 0);

        byProtocol = *(BYTE*)pData;

        BYTE* pReceiveData = new BYTE[nDataLen];
        memcpy(pReceiveData, pData, nDataLen);

        if (byProtocol == l2e_header_def || byProtocol == l2g_header_def)
        {
            KSubProtocolHead* pSPH = (KSubProtocolHead*)pData;
            if (m_ReceiveDataCach[pSPH->wProtocol][pSPH->wSubProtocol].pData)
            {
                delete[] m_ReceiveDataCach[pSPH->wProtocol][pSPH->wSubProtocol].pData;
                m_ReceiveDataCach[pSPH->wProtocol][pSPH->wSubProtocol].pData = NULL;
            }
            m_ReceiveDataCach[pSPH->wProtocol][pSPH->wSubProtocol].pData = pReceiveData;
            m_ReceiveDataCach[pSPH->wProtocol][pSPH->wSubProtocol].tReceiveTime = time(NULL);
        }
        else
        {
            if (m_ReceiveDataCach[byProtocol][0].pData)
            {
                delete[] m_ReceiveDataCach[byProtocol][0].pData;
                m_ReceiveDataCach[byProtocol][0].pData = NULL;
            }
            m_ReceiveDataCach[byProtocol][0].pData = pReceiveData;
            m_ReceiveDataCach[byProtocol][0].tReceiveTime = time(NULL);
        }

        m_Stream.PopPack();
    }
}

BOOL KGuardSimulator::DoE2LGmCommandGS(const char szGmName[], const char szRoleName[], const char szCommand[])
{
    int nReCode = false;
    int nResult = false;
    IKG_Buffer*             piBuffer            = NULL;
    e2l_gmcmd_gs* pProtocolHead = NULL;

    KG_PROCESS_ERROR(szGmName);
    KG_PROCESS_ERROR(szRoleName);
    KG_PROCESS_ERROR(szCommand);

    piBuffer = KG_MemoryCreateBuffer(sizeof(e2l_gmcmd_gs) + strlen(szCommand) + 1);
    KGLOG_PROCESS_ERROR(piBuffer);

    pProtocolHead = (e2l_gmcmd_gs*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pProtocolHead);

    pProtocolHead->wProtocol = e2l_header_def;
    pProtocolHead->wServer = 0;
    pProtocolHead->wSubProtocol = e2l_gmcmd_gs_def;
    strncpy(pProtocolHead->szGmName, szGmName, sizeof(pProtocolHead->szGmName));
    strncpy(pProtocolHead->szRoleName, szRoleName, sizeof(pProtocolHead->szRoleName));
    strcpy(pProtocolHead->szCommand, szCommand);

    nReCode = m_Stream.PutPack((void*)pProtocolHead, sizeof(e2l_gmcmd_gs) + strlen(szCommand) + 1);
    KG_PROCESS_ERROR(nReCode);

    nResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return nResult;
}

BOOL KGuardSimulator::DoE2LGmCommandGC(const char szGmName[], const char szCommand[])
{
    int nReCode = false;
    int nResult = false;
    IKG_Buffer*             piBuffer            = NULL;
    e2l_gmcmd_gc* pProtocolHead = NULL;

    KG_PROCESS_ERROR(szGmName);
    KG_PROCESS_ERROR(szCommand);

    piBuffer = KG_MemoryCreateBuffer(sizeof(e2l_gmcmd_gc) + strlen(szCommand) + 1);
    KGLOG_PROCESS_ERROR(piBuffer);

    pProtocolHead = (e2l_gmcmd_gc*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pProtocolHead);

    pProtocolHead->wProtocol = e2l_header_def;
    pProtocolHead->wServer = 0;
    pProtocolHead->wSubProtocol = e2l_gmcmd_gc_def;
    strcpy(pProtocolHead->szGmName, szGmName);
    strcpy(pProtocolHead->szCommand, szCommand);

    nReCode = m_Stream.PutPack((void*)pProtocolHead, sizeof(e2l_gmcmd_gc) + strlen(szCommand) + 1);
    KG_PROCESS_ERROR(nReCode);

    nResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return nResult;
}

BOOL KGuardSimulator::CheckMessage(const char  szGmName[], const char szMessage[], time_t* pTime)
{
    int nResult = false;
    int nRetCode = false;
    l2e_send_gm_message*    pProtocol = NULL;
    BYTE* pData = NULL;

    pData = GetData(l2e_send_gm_message_def, 0, pTime);
    KG_PROCESS_ERROR(pData);

    pProtocol = (l2e_send_gm_message*)pData;

    nRetCode = strcmp(pProtocol->szGmName, szGmName);
    KG_PROCESS_ERROR(nRetCode == 0);

    nRetCode = strcmp(pProtocol->szMessage, szMessage);
    KG_PROCESS_ERROR(nRetCode == 0);

    nResult = true;
Exit0:
    return nResult;
}

bool KGuardSimulator::DoE2LRemoteLuaCall(char   szFunction[], size_t uParamLen, BYTE* pbyParam)
{
	int nReCode = false;
	int nResult = false;
	IKG_Buffer*             piBuffer            = NULL;
	e2l_remote_lua_call* pProtocolHead = NULL;

	piBuffer = KG_MemoryCreateBuffer(sizeof(e2l_remote_lua_call) + uParamLen);
	KGLOG_PROCESS_ERROR(piBuffer);

	pProtocolHead = (e2l_remote_lua_call*)piBuffer->GetData();
	KGLOG_PROCESS_ERROR(pProtocolHead);

	pProtocolHead->wProtocol = e2l_header_def;
	pProtocolHead->wServer = 0;
	pProtocolHead->wSubProtocol = e2l_remote_lua_call_def;
	strcpy(pProtocolHead->szFunction, szFunction);
	pProtocolHead->uParamLen = uParamLen;
	memcpy(pProtocolHead->byParam, pbyParam, uParamLen);

	nReCode = m_Stream.PutPack((void*)pProtocolHead, sizeof(e2l_remote_lua_call) + uParamLen);
	KG_PROCESS_ERROR(nReCode);

	nResult = true;
Exit0:
	KG_COM_RELEASE(piBuffer);
	return nResult;
}