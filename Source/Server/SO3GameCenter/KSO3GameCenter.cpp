#include "stdafx.h"
#include "KSO3GameCenter.h"
#include "KDBTools.h"
#include "lzo/lzoconf.h"

#ifdef WIN32
    #include <conio.h>
	#include <windows.h>
	#include <time.h>
#else
	#include <sys/time.h>
#endif	//WIN32


KSO3GameCenter::KSO3GameCenter(void)
{
    m_nWorkLoop         = 0;
    m_nIdleLoop         = 0;
    m_piSequence 	    = NULL;
    m_piTextFilter      = NULL;
    m_nZoneChargeFlag   = 0;
}

BOOL KSO3GameCenter::Init(void)
{
    BOOL                bResult                             = false;
	BOOL                bRetCode                            = false;
    int                 nRetCode                            = 0;
    const char*         pszLocale                           = NULL;
    BOOL                bDBToolsInitFlag                    = false;
    BOOL                bGatewayInitFlag                    = false;
    BOOL                bRoleManagerInit                    = false;
	BOOL				bFellowshipInitFlag                 = false;
    BOOL                bMiscDBInitFlag                     = false;
    BOOL                bGameServerInitFlag                 = false;
    // BOOL                bTeamsInitFlag                     = false;
    BOOL                bTeamCenterInitFlag                 = false;
    BOOL                bMapManagerInitFlag                 = false;
    BOOL                bMailManagerInitFlag                = false;
    BOOL                bAuctionManagerInitFlag             = false;
    BOOL                bRoleDBInitFlag                     = false;
    BOOL                bEyesInitFlag                       = false;
    BOOL                bScriptInitFlag                     = false;
    BOOL                bFilterInitFlag                     = false;
    unsigned long       ulMySQLClientVersionID              = 0;
    BOOL                bBattleFieldInitFlag                = false;
    BOOL                bCampInitFlag                       = false;
    BOOL                bPQCenterInitFlag                   = false;
    BOOL                bTongManagerInitFlag                = false;
    BOOL                bGlobalCustomDataManagerInitFlag    = false;
    BOOL                bStatDataManagerInitFlag            = false;
    BOOL                bRoadCenterInitFlag                 = false;
    BOOL                bApexProxyInitFlag                  = false;
    BOOL                bGameCardManagerInitFlag            = false;

    KDBTools            DBTools;

	g_SetRootPath();

    m_nBaseTime = time(NULL);
    m_nTimeNow  = m_nBaseTime;

    bRetCode = m_Settings.Load();
    KGLOG_PROCESS_ERROR(bRetCode);

    pszLocale = setlocale(LC_ALL, m_Settings.m_szLocale);
    KGLOG_PROCESS_ERROR(pszLocale);

    KGLogPrintf(KGLOG_INFO, "Set locale: %s\n", pszLocale);

    nRetCode = lzo_init();
    KGLOG_PROCESS_ERROR(nRetCode == LZO_E_OK);

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

    bRetCode = m_GlobalCustomDataManager.Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bGlobalCustomDataManagerInitFlag = true;

    bRetCode = m_StatDataManager.Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bStatDataManagerInitFlag = true;

    bRetCode = m_RoadCenter.Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bRoadCenterInitFlag = true;

    bRetCode = m_ApexProxy.Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bApexProxyInitFlag = true;

    bRetCode = m_GameCardManager.Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bGameCardManagerInitFlag = true;

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

    m_LogServerAgency.Init();

    m_RemoteScript.Setup();

    CallMainScript("Setup");

    m_RoleManager.CalculateCampActivePlayerCount();

    m_bQuitFlag = false;

    bResult = true;
Exit0:
    if (!bResult)
    {
        KG_COM_RELEASE(m_piTextFilter);
        KG_COM_RELEASE(m_piSequence);

        m_LogServerAgency.UnInit();

        if (bGameCardManagerInitFlag)
        {
            m_GameCardManager.UnInit();
            bGameCardManagerInitFlag = false;
        }

        if (bApexProxyInitFlag)
        {
            m_ApexProxy.UnInit();
            bApexProxyInitFlag = false;
        }

        if (bRoadCenterInitFlag)
        {
            m_RoadCenter.UnInit();
            bRoadCenterInitFlag = false;
        }

        if (bStatDataManagerInitFlag)
        {
            m_StatDataManager.UnInit();
            bStatDataManagerInitFlag = false;
        }

        if (bGlobalCustomDataManagerInitFlag)
        {
            m_GlobalCustomDataManager.UnInit();
            bGlobalCustomDataManagerInitFlag = false;
        }

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
    m_LogServerAgency.UnInit();
    m_GameCardManager.UnInit();
    m_ApexProxy.UnInit();
    m_RoadCenter.UnInit();
    m_StatDataManager.UnInit();
    m_GlobalCustomDataManager.UnInit();
    m_PQCenter.UnInit();
    m_BattleFieldManager.UnInit();
    m_CampManager.UnInit();
    m_ScriptManager.UnInit();
    m_Eyes.UnInit();
    m_GameServer.UnInit();
    m_TeamCenter.UnInit();
    // m_TeamCenter.UnInit();
    m_FellowshipCenter.UnInit();
    m_AuctionManager.UnInit();
    m_MailManager.UnInit();
    m_MapManager.UnInit();
    m_TongManager.UnInit();
    m_MiscDB.UnInit();
    m_RoleManager.UnInit();
    m_RoleDB.UnInit();
    m_Gateway.UnInit();
    m_AntiFarmerManager.Clear();
    m_MentorCenter.Clear();
}

void KSO3GameCenter::Run()
{
    BOOL        bRetCode   = false;
    uint64_t    uBeginTime = KG_GetTickCount();
    uint64_t    uTimeNow   = 0;
    int         nIdleCount = 0;

    while (true)
    {
        uTimeNow = KG_GetTickCount();

        m_Gateway.Activate();
        m_GameServer.Activate();

        if ((uTimeNow - uBeginTime) * GAME_FPS < (uint64_t)m_nWorkLoop * 1000)
        {
            nIdleCount++;
            KGThread_Sleep(10);
            continue;
        }

        bRetCode = CheckQuitComplete();
        if (bRetCode)
            break;

        m_nIdleLoop = nIdleCount;
        nIdleCount  = 0;

        Activate();
    }
}

void KSO3GameCenter::Activate()
{
    m_nTimeNow = time(NULL);
    m_nWorkLoop++;

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

    // m_TeamCenter.Activate();

    m_TeamCenter.Activate();

    m_TongManager.Activate();

    m_PQCenter.Activate();

    m_CampManager.Activate();

    m_GlobalCustomDataManager.Activate();

    m_GlobalSystemValueManager.Activate();

    m_StatDataManager.Activate();

    m_ActivityManager.Activate();

    m_ApexProxy.Breathe();

    m_GameCardManager.Activate();

    m_LogServerAgency.Activate();

    m_AntiFarmerManager.Activate();

    m_MentorCenter.Activate();

    if (m_nWorkLoop % GAME_FPS == 0)
    {
        CallMainScript("Activate");
    }
}

void KSO3GameCenter::Quit()
{
    m_GameServer.PrepareToQuit();
    m_Gateway.PrepareToQuit();
    m_bQuitFlag = true;
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

BOOL KSO3GameCenter::CheckQuitComplete()
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    // 只有哪些“变化才会存盘”的数据，才能在这里保存
    // 比如统计数据，就不能在这里保存，因为只要调用了save
    // 它就回生成一个数据库操作请求，导致数据库线程无法停止工作

    KG_PROCESS_ERROR(m_bQuitFlag);

    // 确保所有GS都已经关闭
    bRetCode = m_GameServer.IsAnyGSConnected();
    KG_PROCESS_ERROR(!bRetCode);

    m_MailManager.SaveAll();

    // 确保所有自定义数据存盘完毕
    m_GlobalCustomDataManager.SaveAll();
    m_GlobalSystemValueManager.SaveAll();

    // 确保所有帮会数据存盘完毕
    m_TongManager.SaveAll();

    // 确保所有好友数据存盘完毕
    m_FellowshipCenter.SaveAllFellowshipData();

    // 确保所有公共任务数据存盘完毕
    m_PQCenter.SaveAll();

    m_MentorCenter.SaveAll();

    // 角色数据库不用检查,因为其工作线程会保证
    //m_RoleDB.CheckSaveQueueComplete();

    // 确保MiscDB的请求与返回队列都全部被处理了
    // 这里必须保证请求队列和应答队列都被完全处理
    // 因为返回的应答可能带来新的数据变化
    bRetCode = m_MiscDB.IsAllPackageComplete();
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}
