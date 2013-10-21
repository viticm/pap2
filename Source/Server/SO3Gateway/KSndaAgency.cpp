#include "stdafx.h"
#include "KSO3Gateway.h"
#include <queue>
#include "Engine/KMutex.h"

#ifdef WIN32
#define KG_LoadLibrary LoadLibrary
#define KG_GetProcessAddress GetProcAddress
#define KG_FreeLibrary FreeLibrary
#else
#include <dlfcn.h>
inline void* KG_LoadLibrary(const char* pszLibFileName)
{
    void* pvResult = NULL;

    KGLOG_PROCESS_ERROR(pszLibFileName);

    pvResult = dlopen(pszLibFileName, RTLD_NOW);
    KGLOG_PROCESS_ERROR(pvResult);
Exit0:
    if (!pvResult)
    {
        char* pszErr = dlerror();
        if (pszErr)
        {
            KGLogPrintf(KGLOG_ERR, "[SDOA] %s\n", pszErr);
        }
    }
    return pvResult;
}
#define KG_GetProcessAddress dlsym
#define KG_FreeLibrary dlclose
#endif

typedef ISDOAUserInfoAuthen* (*KSdoaCreateUserInfoAuthenFunc)(void);

typedef std::queue<KTOKEN_VERIFY_RESPOND> KTOKEN_VERIFY_RESPOND_QUEUE;
static KTOKEN_VERIFY_RESPOND_QUEUE g_SDOAVerifyRespondQueue;
static KMutex                      g_SDOAVerifyRespondQueueLock;

int SNDACALLBACK SdoaUserInfoCallBackFunc(int nMsgType, unsigned long ulUniqueID, ISDOAMsg* piMsg);

KSndaAgency::KSndaAgency()
{
    m_hSDOALibrary      = NULL;
    m_piUserInfoAuthen  = NULL;
    m_pPlayerManager    = NULL;
    m_pPaysysAgency     = NULL;
    m_pGateway          = NULL;
}

KSndaAgency::~KSndaAgency()
{
}

BOOL KSndaAgency::Init()
{
    BOOL                            bResult                     = false;
    int                             nRetCode                    = false;
    KSdoaCreateUserInfoAuthenFunc   SdoaCreateUserInfoAuthen    = NULL;

    KG_PROCESS_SUCCESS(!m_pGateway->m_bSndaAgent);

    m_hSDOALibrary = KG_LoadLibrary(SDOA_DLL_FILE);
    KGLOG_PROCESS_ERROR(m_hSDOALibrary);

    SdoaCreateUserInfoAuthen = (KSdoaCreateUserInfoAuthenFunc)KG_GetProcessAddress(m_hSDOALibrary, "sdoaCreateUserInfoAuthen");
    KGLOG_PROCESS_ERROR(SdoaCreateUserInfoAuthen);

    m_piUserInfoAuthen = SdoaCreateUserInfoAuthen();
    KGLOG_PROCESS_ERROR(m_piUserInfoAuthen);

    nRetCode = m_piUserInfoAuthen->Initialize(SDOA_CONFIG_FILE, &SdoaUserInfoCallBackFunc);
    KGLOG_PROCESS_ERROR(nRetCode == 0);

Exit1:
    bResult = true;
Exit0:
    if (!bResult)
    {
        KGThread_Sleep(10000);

        KG_COM_RELEASE(m_piUserInfoAuthen);

        if (m_hSDOALibrary)
        {
            KG_FreeLibrary(m_hSDOALibrary);
            m_hSDOALibrary = NULL;
        }
    }
    KGLogPrintf(KGLOG_INFO, "[SDOA] Initialize %s!", bResult ? "succeed" : "failed");
    return bResult;
}

void KSndaAgency::UnInit()
{
    KG_COM_RELEASE(m_piUserInfoAuthen);

    if (m_hSDOALibrary)
    {
        KG_FreeLibrary(m_hSDOALibrary);
        m_hSDOALibrary = NULL;
    }
}

void KSndaAgency::Activate()
{
    KG_PROCESS_ERROR(m_pGateway->m_bSndaAgent);

    g_SDOAVerifyRespondQueueLock.Lock();
    while (!g_SDOAVerifyRespondQueue.empty())
    {
        KTOKEN_VERIFY_RESPOND& rRespond = g_SDOAVerifyRespondQueue.front();
        OnTokenVerifyRespond(&rRespond);

        g_SDOAVerifyRespondQueue.pop();
    }
    g_SDOAVerifyRespondQueueLock.Unlock();

Exit0:
    return;
}

BOOL KSndaAgency::DoTokenVerifyRequest(KPlayerAgency* pPlayer)
{
    BOOL            bResult     = false;
    int             nRetCode    = 0;
    const char*     pszIP       = NULL;
    struct in_addr  RemoteIP;
    u_short         uRemotePort;

    assert(pPlayer);
    KGLOG_PROCESS_ERROR(m_piUserInfoAuthen);
    KGLOG_PROCESS_ERROR(pPlayer->piSocket);

    nRetCode = pPlayer->piSocket->GetRemoteAddress(&RemoteIP, &uRemotePort);
    KGLOG_PROCESS_ERROR(nRetCode);

    pszIP = inet_ntoa(RemoteIP);
    KGLOG_PROCESS_ERROR(pszIP);

    nRetCode = m_piUserInfoAuthen->AsyncGetUserInfo(pPlayer->szSndaToken, pszIP, (unsigned long)pPlayer->nIndex);
    KGLOG_PROCESS_ERROR(nRetCode == 0);

    bResult = true;
Exit0:
    return bResult;
}

void KSndaAgency::OnTokenVerifyRespond(const KTOKEN_VERIFY_RESPOND* pRespond)
{
    int             nRetCode    = 0;
    KPlayerAgency*  pPlayer     = NULL;

    assert(pRespond);

    if (pRespond->nCode != 0)
    {
        KGLogPrintf(KGLOG_DEBUG, "[SDOA] Token verify failed! PlayerIndex = %lu, Result = %d\n", pRespond->nPlayerIndex, pRespond->nCode);
        goto Exit0;
    }

    pPlayer = m_pPlayerManager->GetPlayer(pRespond->nPlayerIndex);
    KGLOG_PROCESS_ERROR(pPlayer);

    KGLOG_PROCESS_ERROR(pPlayer->nState == easWaitForSDOAVerify);

    nRetCode = strncmp(pPlayer->szAccount, pRespond->szSndaID, sizeof(pPlayer->szAccount));
    KGLOG_PROCESS_ERROR(nRetCode == 0);

    strncpy(pPlayer->szIDCard, pRespond->szIDCard, sizeof(pPlayer->szIDCard));
    pPlayer->szIDCard[sizeof(pPlayer->szIDCard) - 1] = '\0';

    pPlayer->dwVerifyPostTime = KG_GetTickCount();

    pPlayer->nState = easWaitForPaysysVerify;
    m_pPaysysAgency->DoAccountVerifySDRequest(pPlayer);

Exit0:
    return;
}

int SNDACALLBACK SdoaUserInfoCallBackFunc(int nMsgType, unsigned long ulUniqueID, ISDOAMsg* piMsg)
{
    KTOKEN_VERIFY_RESPOND Respond;

    Respond.nCode           = nMsgType;
    Respond.nPlayerIndex    = (int)ulUniqueID;
    Respond.szSndaID[0]     = '\0';
    Respond.szIDCard[0]     = '\0';

    if (nMsgType == 0)
    {
        const char* pszValue = NULL;

        pszValue = piMsg->GetValue("sndaID");
        if (pszValue)
        {
            strncpy(Respond.szSndaID, pszValue, sizeof(Respond.szSndaID));
            Respond.szSndaID[sizeof(Respond.szSndaID) - 1] = '\0';
        }

        pszValue = piMsg->GetValue("idCard");
        if (pszValue)
        {
            strncpy(Respond.szIDCard, pszValue, sizeof(Respond.szIDCard));
            Respond.szIDCard[sizeof(Respond.szIDCard) - 1] = '\0';
        }
    }

    g_SDOAVerifyRespondQueueLock.Lock();
    g_SDOAVerifyRespondQueue.push(Respond);
    g_SDOAVerifyRespondQueueLock.Unlock();

    return true;
}
