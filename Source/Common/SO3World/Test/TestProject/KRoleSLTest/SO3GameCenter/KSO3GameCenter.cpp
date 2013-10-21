#include "stdafx.h"
#include "KSO3GameCenter.h"
#include "KDBTools.h"

#ifdef WIN32
    #include <conio.h>
	#include <windows.h>
	#include <time.h>
#else
	#include <sys/time.h>
#endif	//WIN32

extern int g_nTestTime;
extern int g_nTestConnection;

KSO3GameCenter::KSO3GameCenter(void)
{
    m_nWorkLoop     = 0;
    m_nIdleLoop     = 0;
    m_piSequence 	= NULL;
    m_piTextFilter  = NULL;
}

BOOL KSO3GameCenter::Init(void)
{
    BOOL                bResult                 = false;
	BOOL                bRetCode                = false;
    const char*         pszLocale               = NULL;
    BOOL                bDBToolsInitFlag        = false;
    BOOL                bGatewayInitFlag        = false;
    BOOL                bRoleManagerInit        = false;
	BOOL				bFellowshipInitFlag     = false;
    BOOL                bMiscDBInitFlag         = false;
    BOOL                bGameServerInitFlag     = false;
    // BOOL                bPartysInitFlag         = false;
    BOOL                bTeamCenterInitFlag     = false;
    BOOL                bMapManagerInitFlag     = false;
    BOOL                bMailManagerInitFlag    = false;
    BOOL                bAuctionManagerInitFlag = false;
    BOOL                bRoleDBInitFlag         = false;
    BOOL                bEyesInitFlag           = false;
    BOOL                bScriptInitFlag         = false;
    BOOL                bFilterInitFlag         = false;
    unsigned long       ulMySQLClientVersionID  = 0;
    BOOL                bBattleFieldInitFlag    = false;
    BOOL                bCampInitFlag           = false;
    BOOL                bPQCenterInitFlag       = false;
    BOOL                bTongManagerInitFlag    = false;
    KDBTools            DBTools;

	g_SetRootPath();

    m_nBaseTime = time(NULL);
    m_nTimeNow  = m_nBaseTime;

    bRetCode = m_Settings.Load();
    KGLOG_PROCESS_ERROR(bRetCode);

    pszLocale = setlocale(LC_ALL, m_Settings.m_szLocale);
    KGLOG_PROCESS_ERROR(pszLocale);

    KGLogPrintf(KGLOG_INFO, "Set locale: %s\n", pszLocale);

    bRetCode = DBTools.Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bDBToolsInitFlag = true;

    bRetCode = DBTools.CheckDatabase();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = m_Gateway.Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bGatewayInitFlag = true;

    bRetCode = m_RoleManager.Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bRoleManagerInit = true;

    bRetCode = m_RoleDB.Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bRoleDBInitFlag = true;

	bRetCode = m_TongManager.Init();
	KGLOG_PROCESS_ERROR(bRetCode);
    bTongManagerInitFlag = true;

    bRetCode = m_MiscDB.Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bMiscDBInitFlag = true;

	bRetCode = m_FellowshipCenter.Init();
	KGLOG_PROCESS_ERROR(bRetCode);
	bFellowshipInitFlag = true;

    bRetCode = m_MapManager.Init();
	KGLOG_PROCESS_ERROR(bRetCode);
    bMapManagerInitFlag = true;

    bRetCode = m_MailManager.Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bMailManagerInitFlag = true;

    bRetCode = m_AuctionManager.Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bAuctionManagerInitFlag = true;

    // bRetCode = m_PartyCenter.Init();    
    // KGLOG_PROCESS_ERROR(bRetCode);
    // bPartysInitFlag = true;

    bRetCode = m_TeamCenter.Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bTeamCenterInitFlag = true;

    bRetCode = m_HometownList.Load();
    KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = m_GameServer.Init();
	KGLOG_PROCESS_ERROR(bRetCode);
    bGameServerInitFlag = true;

    bRetCode = m_Eyes.Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bEyesInitFlag = true;

    bRetCode = m_ScriptManager.Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bScriptInitFlag = true;
    
    bRetCode = m_CampManager.Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bCampInitFlag = true;

    bRetCode = m_GlobalSystemValueManager.Load();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = m_BattleFieldManager.Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bBattleFieldInitFlag = true;

    bRetCode = m_PQCenter.Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bPQCenterInitFlag = true;

    m_piSequence = CreateIDSequence(
        m_Settings.m_szDBIP, 
        m_Settings.m_szDBName, 
        m_Settings.m_szDBAcc, 
        m_Settings.m_szDBPsw
    );
    KGLOG_PROCESS_ERROR(m_piSequence);

    m_piTextFilter = CreateTextFilterInterface();
    KGLOG_PROCESS_ERROR(m_piTextFilter);

    bRetCode = m_piTextFilter->LoadFilterFile("NameFilter.txt");
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = m_piTextFilter->LoadWhiteListFile("WhiteList.txt");
    KGLOG_PROCESS_ERROR(bRetCode);

    // 注意,其他模块的Init请放到LoadGameData之前
    bRetCode = DBTools.LoadGameData();
    KGLOG_PROCESS_ERROR(bRetCode);

    m_RemoteScript.Setup();

    CallMainScript("Setup");

    m_bRunFlag = true;

    bResult = true;
Exit0:
    if (!bResult)
    {
        KG_COM_RELEASE(m_piTextFilter);
        KG_COM_RELEASE(m_piSequence);

        if (bPQCenterInitFlag)
        {
            m_PQCenter.UnInit();
            bPQCenterInitFlag = false;
        }

        if (bBattleFieldInitFlag)
        {
            m_BattleFieldManager.UnInit();
            bBattleFieldInitFlag = false;
        }

        if (bCampInitFlag)
        {
            m_CampManager.UnInit();
            bCampInitFlag = false;
        }

        if (bScriptInitFlag)
        {
            m_ScriptManager.UnInit();
            bScriptInitFlag = false;
        }

        if (bEyesInitFlag)
        {
            m_Eyes.UnInit();
            bEyesInitFlag = false;
        }

        if (bGameServerInitFlag)
        {
            m_GameServer.UnInit();
            bGameServerInitFlag = false;
        }

        if(bMiscDBInitFlag)
        {
            m_MiscDB.UnInit();
            bMiscDBInitFlag = false;
        }
        
        if (bTeamCenterInitFlag)
        {
            m_TeamCenter.UnInit();
            bTeamCenterInitFlag = false;
        }

        // if (bPartysInitFlag) 
        // {
        //     m_PartyCenter.UnInit();
        //     bPartysInitFlag = false;
        // }

        if (bAuctionManagerInitFlag)
        {
            m_AuctionManager.UnInit();
            bAuctionManagerInitFlag = false;
        }

        if (bMailManagerInitFlag)
        {
            m_MailManager.UnInit();
            bMailManagerInitFlag = false;
        }
        
        if (bMapManagerInitFlag)
        {
            m_MapManager.UnInit();
            bMapManagerInitFlag = false;
        }

		if (bFellowshipInitFlag)
		{
			m_FellowshipCenter.UnInit();
			bFellowshipInitFlag = false;
		}

        if (bTongManagerInitFlag)
        {
            m_TongManager.UnInit();
            bTongManagerInitFlag = false;
        }

        if (bRoleDBInitFlag)
        {
            m_RoleDB.UnInit();
            bRoleDBInitFlag = false;
        }

        if (bRoleManagerInit)
        {
            m_RoleManager.UnInit();
            bRoleManagerInit = true;
        }

        if (bGatewayInitFlag)
        {
            m_Gateway.UnInit();
            bGatewayInitFlag = false;
        }
    }

    if (bDBToolsInitFlag)
    {
        DBTools.UnInit();
        bDBToolsInitFlag = false;
    }

	return bResult;
}

void KSO3GameCenter::UnInit()
{
    KG_COM_RELEASE(m_piTextFilter);
    KG_COM_RELEASE(m_piSequence);
    m_GlobalCustomDataManager.UnInit();
    m_PQCenter.UnInit();
    m_BattleFieldManager.UnInit();
    m_CampManager.UnInit();
    m_ScriptManager.UnInit();
    m_Eyes.UnInit();
    m_GameServer.UnInit();
    m_TeamCenter.UnInit();
    // m_PartyCenter.UnInit();
    m_FellowshipCenter.UnInit();
    m_AuctionManager.UnInit();
    m_MailManager.UnInit();
    m_MapManager.UnInit();
    m_MiscDB.UnInit();
    m_TongManager.UnInit();
    m_RoleManager.UnInit();
    m_RoleDB.UnInit();
    m_Gateway.UnInit();
}

void KSO3GameCenter::Run()
{
    uint64_t    uBeginTime = KG_GetTickCount();
    uint64_t    uTimeNow   = 0;
    int         nIdleCount = 0;
    int         szTest[] = {16384, 32767, 32768, 32769, 49152, 65536, 262143, 262144, 262145};

    while (m_bRunFlag)
    {
        uTimeNow = KG_GetTickCount();

        if ((uTimeNow - uBeginTime) * GAME_FPS < (uint64_t)m_nWorkLoop * 1000)
        {
            nIdleCount++;
            KGThread_Sleep(10);
            continue;
        }
        //Test Code
        if (g_nTestTime < 9 && g_nTestConnection != -1)
        {
            BYTE pbyBuffer[262155];
            size_t nRoleDataLen = szTest[g_nTestTime];
            for (size_t nAddCount = 0; nAddCount < nRoleDataLen; nAddCount++)
            {
                pbyBuffer[nAddCount] = char(nAddCount % 10);
            }

            FILE *fp = NULL;
            char szFileName[100];
            snprintf(szFileName, sizeof(szFileName), "savelog/Load_Center%d", szTest[g_nTestTime]);
            szFileName[sizeof(szFileName) - 1] = '\0';

            fp = fopen(szFileName, "a");
            if (!fp)
            {
                printf("Can't Save Load Center!");
                break;
            }

            for (size_t nAddCount2 = 0; nAddCount2 < nRoleDataLen; nAddCount2++)
            {
                fprintf(fp, "%d", pbyBuffer[nAddCount2]);
            }

            fclose(fp);
            fp = NULL;

            g_pSO3GameCenter->m_GameServer.LoadRoleData(g_nTestConnection, szTest[g_nTestTime], TRUE, pbyBuffer, nRoleDataLen);

            g_nTestTime++;
        }
        else
        {
            if (g_nTestTime == 9 && g_nTestConnection != -1)
            {
                printf("Test Load Over");
                g_nTestTime++;
            }
        }

        m_nIdleLoop = nIdleCount;
        nIdleCount  = 0;

        Activate();
    }

    m_GlobalCustomDataManager.Save();
    m_MailManager.SaveAll();
    m_TongManager.SaveAll();
}

void KSO3GameCenter::Activate()
{
    m_nTimeNow = time(NULL);
    m_nWorkLoop++;

    m_Gateway.Activate();

	m_GameServer.Activate();

    m_MapManager.Activate();

    m_FellowshipCenter.Active();

    m_MailManager.Activate();

    m_AuctionManager.Activate();

    m_RoleDB.Activate();

    m_RoleManager.Activate();

    m_MiscDB.Activate();

    m_Eyes.Activate();

    m_ScriptManager.Activate();

    m_BattleFieldManager.Activate();

    // m_PartyCenter.Activate();

    m_TeamCenter.Activate();

    m_TongManager.Activate();

    m_PQCenter.Activate();

    m_CampManager.Activate();

    m_GlobalCustomDataManager.Activate();

    m_GlobalSystemValueManager.Activate();

    if (m_nWorkLoop % GAME_FPS == 0)
    {
        CallMainScript("Activate");
    }
}

BOOL KSO3GameCenter::CallMainScript(const char cszFunction[])
{
    BOOL    bResult             = false;
    BOOL    bRetCode            = false;
    int     nIndex              = 0;
    DWORD   dwScriptID          = 0;

    dwScriptID = g_FileNameHash(MAIN_SCRIPT_FILE);

    m_ScriptManager.SafeCallBegin(&nIndex);

    bRetCode = m_ScriptManager.IsScriptExist(dwScriptID);
    KG_PROCESS_ERROR(bRetCode);

    bRetCode = m_ScriptManager.IsFuncExist(dwScriptID, cszFunction);
    KG_PROCESS_ERROR(bRetCode);

    m_ScriptManager.Call(dwScriptID, cszFunction, 0);

    bResult = true;
Exit0:
    m_ScriptManager.SafeCallEnd(nIndex);
    return bResult;
}
