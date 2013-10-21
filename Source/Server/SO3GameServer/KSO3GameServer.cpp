#include "stdafx.h"
#include "KSO3GameServer.h"
#include "KSO3World.h"
#include "KPlayerServer.h"
#include "KRelayClient.h"
#include "KLogClient.h"
#include "KRecorderFactory.h"

KSO3GameServer::KSO3GameServer()
{
    m_piRecorderTools = NULL;
    m_nRunMode        = 0;
    m_szLocale[0]     = '\0';
}

BOOL KSO3GameServer::Init()
{
    BOOL                    bResult                 = false;
	BOOL                    bRetCode                = false;
    const char*             pszLocale               = NULL;
    BOOL                    bWorldOpen              = false;
    BOOL                    bPlayerServerInit       = false;
    BOOL                    bGoddessClientInit      = false;
    BOOL                    bRelayClientInit        = false;
    BOOL                    bEyesInitFlag           = false;
    IRecorderFactory*       piRecorderFactory       = NULL;

    g_SetRootPath(NULL);

    bRetCode = LoadConfig();
    KGLOG_PROCESS_ERROR(bRetCode);

    pszLocale = setlocale(LC_ALL, m_szLocale);
    KGLOG_PROCESS_ERROR(pszLocale);

    KGLogPrintf(KGLOG_INFO, "Set locale: %s\n", pszLocale);

    piRecorderFactory = CreateRecorderFactoryInterface((KRecoderMode)m_nRunMode);
    KGLOG_PROCESS_ERROR(piRecorderFactory);

    m_piRecorderTools = piRecorderFactory->CreateToolsInterface();
    KGLOG_PROCESS_ERROR(m_piRecorderTools);

    g_pSO3World = new KSO3World;
    KGLOG_PROCESS_ERROR(g_pSO3World);
    
    bRetCode = g_pSO3World->Init(piRecorderFactory);    
	KGLOG_PROCESS_ERROR(bRetCode);
    bWorldOpen = true;

	bRetCode = g_RelayClient.Init(piRecorderFactory);
	KGLOG_PROCESS_ERROR(bRetCode);
    bRelayClientInit = true;

	bRetCode = g_PlayerServer.Init(piRecorderFactory);
	KGLOG_PROCESS_ERROR(bRetCode);
    bPlayerServerInit = true;
	
    bRetCode = m_Eyes.Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bEyesInitFlag = true;

    g_LogClient.Init();

    m_fIdleFrame    = 0.0f;
    m_fLastShowIdle = 0.0f;

    g_pSO3World->m_bRunFlag = true;

	bResult = true;
Exit0:
    if (!bResult)
    {
        if (bEyesInitFlag)
        {
            m_Eyes.UnInit();
            bEyesInitFlag = false;
        }

        if (bPlayerServerInit)
        {
            g_PlayerServer.UnInit();
            bPlayerServerInit = false;
        }

        if (bRelayClientInit)
        {
            g_RelayClient.UnInit();
            bRelayClientInit = false;
        }

        if (bWorldOpen)
        {
            g_pSO3World->UnInit();
            bWorldOpen = false;
        }

        KG_DELETE(g_pSO3World);

        KG_COM_RELEASE(m_piRecorderTools);
    }
    KG_COM_RELEASE(piRecorderFactory);
	return bResult;
}

void KSO3GameServer::UnInit(void)
{
    g_LogClient.UnInit();
    m_Eyes.UnInit();

    g_pSO3World->UnInit();

    KG_DELETE(g_pSO3World);

    g_PlayerServer.UnInit();

    KG_COM_RELEASE(m_piRecorderTools);
    
    g_RelayClient.UnInit();
    
	return;
}

void KSO3GameServer::Run()
{
    int64_t nInterval         = 0;
    int64_t nGameTime         = 0;
    DWORD   dwCurrentTime     = 0;
    int     nIdleFrame        = 0;
    int     nNextFlushFrame   = 0;
    DWORD   dwIdleSleepTime   = 10;
    
    if (m_nRunMode == ermReplay)
    {
        dwIdleSleepTime = 0;
    }
    
    while (g_pSO3World->m_bRunFlag)
    {        
        dwCurrentTime = m_piRecorderTools->GetTickCount();

        g_RelayClient.ProcessPackage();
        g_PlayerServer.Breathe();

        nInterval = dwCurrentTime - g_pSO3World->m_dwStartTime;
        if (nInterval * GAME_FPS < nGameTime)
        {
            nIdleFrame++;
            KGThread_Sleep(dwIdleSleepTime);
            continue;
        }

        g_pSO3World->m_nIdleFrame = nIdleFrame;
        ShowPerformance(nIdleFrame);
        nIdleFrame = 0;

        m_Eyes.Activate();
   
        g_RelayClient.Activate();
        g_LogClient.Activate();
	    g_pSO3World->Activate();

        nGameTime = (int64_t)(g_pSO3World->m_nGameLoop - g_pSO3World->m_nStartLoop) * 1000;

        if (g_pSO3World->m_nGameLoop > nNextFlushFrame)
        {
	        g_PlayerServer.FlushSend();
            nNextFlushFrame = g_pSO3World->m_nGameLoop + GAME_FPS / 4;
        }
    }

    g_pSO3World->m_StatDataServer.SendAllStatData();
}

#ifdef WIN32
BOOL KSO3GameServer::ShowPerformance(int nIdleFrame)
{
    BOOL    bResult    = false;
    int     nRetCode   = 0;
    char    szTitle[64];

    m_fIdleFrame = (m_fIdleFrame * 96.0f + nIdleFrame * 4.0f) / 100.0f;
    KG_PROCESS_ERROR(abs(m_fIdleFrame - m_fLastShowIdle) >= 0.1f);
    
    m_fLastShowIdle = m_fIdleFrame;

    nRetCode = (int)snprintf(szTitle, sizeof(szTitle), "Game server for JXOnline III --- %.1f", m_fLastShowIdle);
    KG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(szTitle));

    SetConsoleTitle(szTitle);

    bResult = true;
Exit0:
    return bResult;
}
#else
BOOL KSO3GameServer::ShowPerformance(int nIdleFrame)
{
    BOOL    bResult    = false;

    m_fIdleFrame = (m_fIdleFrame * 96.0f + nIdleFrame * 4.0f) / 100.0f;
    KG_PROCESS_ERROR(fabs(m_fIdleFrame - m_fLastShowIdle) > 0.5f);
    
    m_fLastShowIdle = m_fIdleFrame;

    bResult = true;
Exit0:
    return bResult;
}
#endif

BOOL KSO3GameServer::LoadConfig()
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    IIniFile*   piIniFile   = NULL;

    piIniFile = g_OpenIniFile(GS_SETTINGS_FILENAME);
	KGLOG_PROCESS_ERROR(piIniFile);
    
    bRetCode = piIniFile->GetInteger("Global", "RunMode", 0, &m_nRunMode);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piIniFile->GetString("Global", "Locale", "", m_szLocale, sizeof(m_szLocale));
    KGLOG_PROCESS_ERROR(bRetCode);
    
    bResult = true;
Exit0:
    KG_COM_RELEASE(piIniFile);
    return bResult;
}
