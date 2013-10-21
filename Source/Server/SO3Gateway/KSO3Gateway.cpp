#include "stdafx.h"
#include "KSO3Gateway.h"

KSO3Gateway::KSO3Gateway()
{
    m_szLocale[0] = '\0';
    m_bSndaAgent  = false;
}

BOOL KSO3Gateway::Init()
{
    BOOL            bResult             = false;
    BOOL            bRetCode            = false;
    const char*     pszLocale           = NULL;
    BOOL            bPaysysAgencyInit   = false;
    BOOL            bRelayAgencyInit    = false;
    BOOL            bPlayerManagerInit  = false;
    BOOL            bEyesInit           = false;
    BOOL            bSDOAInit           = false;

    g_SetRootPath(NULL);

    bRetCode = LoadConfig();
    KGLOG_PROCESS_ERROR(bRetCode);

    pszLocale = setlocale(LC_ALL, m_szLocale);
    KGLOG_PROCESS_ERROR(pszLocale);

    KGLogPrintf(KGLOG_INFO, "Set locale: %s\n", pszLocale);

    m_PaysysAgency.AttachModule(&m_RelayAgency);
    m_PaysysAgency.AttachModule(&m_PlayerManager);

    m_RelayAgency.AttachModule(&m_PaysysAgency);
    m_RelayAgency.AttachModule(&m_PlayerManager);
    m_RelayAgency.AttachModule(this);
    m_RelayAgency.AttachModule(&m_QueueManager);
    m_RelayAgency.AttachModule(&m_SndaAgency);

    m_PlayerManager.AttachModule(&m_PaysysAgency);
    m_PlayerManager.AttachModule(&m_RelayAgency);
    m_PlayerManager.AttachModule(&m_QueueManager);
    m_PlayerManager.AttachModule(this);

    m_QueueManager.AttachModule(&m_PlayerManager);
    m_QueueManager.AttachModule(&m_RelayAgency);

    m_Eyes.AttachModule(&m_PlayerManager);
    m_Eyes.AttachModule(&m_PaysysAgency);
    m_Eyes.AttachModule(&m_QueueManager);

    m_SndaAgency.AttachModule(&m_PlayerManager);
    m_SndaAgency.AttachModule(&m_PaysysAgency);
    m_SndaAgency.AttachModule(this);

    //bRetCode = m_PaysysAgency.Init();
    //KGLOG_PROCESS_ERROR(bRetCode);
    //bPaysysAgencyInit = true;

    bRetCode = m_RelayAgency.Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bRelayAgencyInit = true;

    bRetCode = m_PlayerManager.Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bPlayerManagerInit = true;

    bRetCode = m_Eyes.Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bEyesInit = true;

    bRetCode = m_SndaAgency.Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bSDOAInit = true;

    m_bRunFlag = true;

    bResult = true;
Exit0:
    if (!bResult)
    {
        if (bPlayerManagerInit)
        {
            m_PlayerManager.UnInit();
            bPlayerManagerInit = false;
        }

        if (bRelayAgencyInit)
        {
            m_RelayAgency.UnInit();
            bRelayAgencyInit = false;
        }

        if (bPaysysAgencyInit)
        {
            m_PaysysAgency.UnInit();
            bPaysysAgencyInit = false;
        }

        if (bEyesInit)
        {
            m_Eyes.UnInit();
            bEyesInit = false;
        }

        if (bSDOAInit)
        {
            m_SndaAgency.UnInit();
            bSDOAInit = false;
        }
    }

    return bResult;
}

void KSO3Gateway::UnInit()
{
    m_PlayerManager.UnInit();
    m_RelayAgency.UnInit();
    m_PaysysAgency.UnInit();
    m_Eyes.UnInit();
    m_SndaAgency.UnInit();
}


void KSO3Gateway::Run()
{
    BOOL bRetCode = false;

    while (m_bRunFlag)
    {
        m_nTimeNow = time(NULL);

       // m_PaysysAgency.Activate();

        m_RelayAgency.Activate();

        m_PlayerManager.Activate();

        m_QueueManager.Activate();

        m_Eyes.Activate();

        m_SndaAgency.Activate();

        bRetCode = m_Eyes.GetQuitFlag();
        if (bRetCode)
            break;

        KGThread_Sleep(10);
    }

    KGLogPrintf(KGLOG_INFO, "Gateway exit !");
}

BOOL KSO3Gateway::LoadConfig()
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    IIniFile*   piIniFile   = NULL;

    piIniFile = g_OpenIniFile(GATEWAY_CONFIG_FILE);
    KGLOG_PROCESS_ERROR(piIniFile);

    bRetCode = piIniFile->GetString("Global", "Locale", "", m_szLocale, sizeof(m_szLocale));
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piIniFile->GetBool("Global", "SD", &m_bSndaAgent);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piIniFile);
    return bResult;
}
