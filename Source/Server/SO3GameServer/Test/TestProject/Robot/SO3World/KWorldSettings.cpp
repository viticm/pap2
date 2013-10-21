#include "stdafx.h"
#include "KWorldSettings.h"

#include "KSO3World.h"

KWorldSettings	g_WorldSettings;

#if defined(_SERVER)
	#define SO3WORLD_FILENAME	"ServerWorld.ini"
#endif	//_SERVER

#if defined(_CLIENT)
	#define SO3WORLD_FILENAME	"ClientWorld.ini"
#endif	//_CLIENT

#if defined(_EDITOR)
	#define SO3WORLD_FILENAME	"EditorWorld.ini"
#endif	//_EDITOR

#define ROBOT_FILENAME "MonsterConfig.ini"

#ifdef _PERFORMANCE_OPTIMIZATION
    int g_nGameFps = 16;
#endif


KWorldSettings::KWorldSettings(void)
{
}

KWorldSettings::~KWorldSettings(void)
{
}

BOOL KWorldSettings::Init(void)
{
	BOOL bRetCode = FALSE;

	//SO3World.ini
	bRetCode = LoadWorldParam();
	KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = LoadRobotParam();
    KGLOG_PROCESS_ERROR(bRetCode);
	
	//bRetCode = InitRelationTable();
	//KGLOG_PROCESS_ERROR(bRetCode);

	//bRetCode = m_NpcTemplateList.Init();
	//KGLOG_PROCESS_ERROR(bRetCode);

//	bRetCode = m_DoodadTemplateList.Init();
//	KGLOG_CHECK_ERROR(bRetCode);

	bRetCode = m_MapListFile.Init();
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = m_ReturnPosList.Init();
	KGLOG_CHECK_ERROR(bRetCode);


	//bRetCode = m_NpcClassList.Init();
	//KGLOG_CHECK_ERROR(bRetCode);

//	bRetCode = m_DoodadClassList.Init();
//	KGLOG_CHECK_ERROR(bRetCode);

	//bRetCode = m_PatrolPathList.Init();
	//KGLOG_CHECK_ERROR(bRetCode);

	bRetCode = m_LevelUpList.Init();
	KGLOG_CHECK_ERROR(bRetCode);

    bRetCode = m_CoolDownList.Init();
    KGLOG_CHECK_ERROR(bRetCode);

//    bRetCode = m_WeaponTypeList.Init();
 //   KGLOG_CHECK_ERROR(bRetCode);

	bRetCode = m_RelationCampList.Init(RELATION_CAMP_FILE_NAME);
	KGLOG_CHECK_ERROR(bRetCode);

	bRetCode = m_RelationForceList.Init(RELATION_FORCE_FILE_NAME);
	KGLOG_CHECK_ERROR(bRetCode);

	bRetCode = m_ForceDefaultList.Init();
	KGLOG_CHECK_ERROR(bRetCode);

	//bRetCode = m_AIParamTemplateList.Init();
	//KGLOG_CHECK_ERROR(bRetCode);
//	bRetCode = m_CharacterActionList.Init();
//	KGLOG_CHECK_ERROR(bRetCode);

	return TRUE;

Exit0:

	return FALSE;
}

BOOL KWorldSettings::UnInit(void)
{
//	m_CharacterActionList.UnInit();
//	m_AIParamTemplateList.UnInit();
//    m_WeaponTypeList.UnInit();
    m_CoolDownList.UnInit();
    m_LevelUpList.UnInit();
//	m_DoodadTemplateList.UnInit();
//	m_NpcTemplateList.UnInit();
//	m_DoodadClassList.UnInit();
//	m_NpcClassList.UnInit();
//	m_PatrolPathList.UnInit();
	
	return TRUE;
}

BOOL KWorldSettings::LoadWorldParam()
{
	BOOL		bResult					= false;
	BOOL		bRetCode				= false;
	IIniFile*	piIniFile				= NULL;
	int			nValue					= 0;
	const char  cszObjAllocSection[]	= "ObjAlloc";

	piIniFile = g_OpenIniFile(SO3WORLD_FILENAME);
	KGLOG_PROCESS_ERROR(piIniFile);

	bRetCode = piIniFile->GetInteger(cszObjAllocSection, "SceneCount", 0, &nValue);
	KGLOG_PROCESS_ERROR(bRetCode);
	m_WorldParam.nSceneCount = nValue;

	bRetCode = piIniFile->GetInteger(cszObjAllocSection, "RegionCount",	0, &nValue);
	KGLOG_PROCESS_ERROR(bRetCode);
	m_WorldParam.nRegionCount = nValue;

	bRetCode = piIniFile->GetInteger(cszObjAllocSection, "PlayerCount",	0, &nValue);
	KGLOG_PROCESS_ERROR(bRetCode);
	m_WorldParam.nPlayerCount = nValue;

	bRetCode = piIniFile->GetInteger(cszObjAllocSection, "NpcCount", 0, &nValue);
	KGLOG_PROCESS_ERROR(bRetCode);
	m_WorldParam.nNpcCount = nValue;

	bRetCode = piIniFile->GetInteger(cszObjAllocSection, "DoodadCount",	0, &nValue);
	KGLOG_PROCESS_ERROR(bRetCode);
	m_WorldParam.nDoodadCount = nValue;

	bRetCode = piIniFile->GetInteger(cszObjAllocSection, "CellCount", 0, &nValue);
	KGLOG_PROCESS_ERROR(bRetCode);
	m_WorldParam.nCellCount = nValue;

	bRetCode = piIniFile->GetInteger(cszObjAllocSection, "ItemCount", 0, &nValue);
	KGLOG_PROCESS_ERROR(bRetCode);
	m_WorldParam.nItemCount = nValue;

	bRetCode = piIniFile->GetInteger(cszObjAllocSection, "LootListCount", 0, &nValue);
	KGLOG_PROCESS_ERROR(bRetCode);
	m_WorldParam.nLootListCount = nValue;

    bRetCode = piIniFile->GetInteger(cszObjAllocSection, "AttributeCount", 0, &nValue);
    KGLOG_PROCESS_ERROR(bRetCode);
    m_WorldParam.nAttributeCount = nValue;

	bRetCode = piIniFile->GetInteger(cszObjAllocSection, "ThreatNodeCount",	0, &nValue);
	KGLOG_PROCESS_ERROR(bRetCode);
	m_WorldParam.nThreatNodeCount = nValue;

    bRetCode = piIniFile->GetInteger(cszObjAllocSection, "BulletCount", 0, &nValue);
    KGLOG_PROCESS_ERROR(bRetCode);
    m_WorldParam.nBulletCount = nValue;

    bRetCode = piIniFile->GetInteger(cszObjAllocSection, "TradingBoxCount", 0, &nValue);
    KGLOG_PROCESS_ERROR(bRetCode);
    m_WorldParam.nTradingBoxCount = nValue;

	bRetCode = piIniFile->GetInteger(cszObjAllocSection, "NpcTeamCount", 0, &nValue);
	KGLOG_PROCESS_ERROR(bRetCode);
	m_WorldParam.nNpcTeamCount = nValue;

#ifdef _PERFORMANCE_OPTIMIZATION
    bRetCode = piIniFile->GetInteger(cszObjAllocSection, "GameFps", 16, (int*)&g_nGameFps);
    KGLOG_PROCESS_ERROR(bRetCode && "Not Find the Item /GameFps=XXX/ in ServerWorld.ini or ClientWorld.ini");
    KGLogPrintf(KGLOG_ERR, "GameFps = %d", g_nGameFps);
    KGLOG_PROCESS_ERROR(g_nGameFps > 0);
#endif

	bResult = true;
Exit0:
	if (!bResult)
	{
		KGLogPrintf(KGLOG_ERR, "Load world setting file \"%s\" failed !", SO3WORLD_FILENAME);
	}

	KG_COM_RELEASE(piIniFile);
	return bResult;
}

BOOL KWorldSettings::LoadRobotParam()
{
    int nResult  = false;
    int nRetCode = false;

    IIniFile       *pIniFile = NULL;

    pIniFile = g_OpenIniFile(ROBOT_FILENAME);
    KGLOG_PROCESS_ERROR(pIniFile);

    nRetCode = pIniFile->GetString("Bishop", "IP", "", m_RobotParam.szBishopIP, sizeof(m_RobotParam.szBishopIP));
    KGLOG_PROCESS_ERROR(nRetCode);
    m_RobotParam.szBishopIP[sizeof(m_RobotParam.szBishopIP) -1] = '\0';

    nRetCode = pIniFile->GetInteger("Bishop", "Port", 0, &m_RobotParam.nBishopPort);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = pIniFile->GetString("Account", "AccountNamePrefix", "", 
        m_RobotParam.szAccountNamePrefix, 
        sizeof(m_RobotParam.szAccountNamePrefix));
    KGLOG_PROCESS_ERROR(nRetCode);
    m_RobotParam.szAccountNamePrefix[sizeof(m_RobotParam.szAccountNamePrefix) - 1] = '\0';

    nRetCode = pIniFile->GetString("Account", "Password", "", m_RobotParam.szPassword, sizeof(m_RobotParam.szPassword));
    KGLOG_PROCESS_ERROR(nRetCode);
    m_RobotParam.szPassword[sizeof(m_RobotParam.szPassword) - 1] = '\0';

    nRetCode = pIniFile->GetInteger("Account", "StartIndex", 0, &m_RobotParam.nStartIndex);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = pIniFile->GetInteger("Monster", "Reconnect", 0, &m_RobotParam.nReconnect);
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(m_RobotParam.nReconnect == 0 || m_RobotParam.nReconnect == 1);

    nRetCode = pIniFile->GetInteger("Monster", "PreLoadMap", 0, &m_RobotParam.nPreLoadMap);
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(m_RobotParam.nPreLoadMap == 0 || m_RobotParam.nPreLoadMap == 1);

    nRetCode = pIniFile->GetInteger("Monster", "Arrangement", 0, (int *)&m_RobotParam.nArrangement);
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(m_RobotParam.nArrangement >= hpRandom && m_RobotParam.nArrangement <= hpUnchange);

    nRetCode = pIniFile->GetInteger("Monster", "Range", 0, (int *)&m_RobotParam.nRange);
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(m_RobotParam.nRange >= 0);

    nRetCode = pIniFile->GetInteger("Monster", "RunRate", 0, (int *)&m_RobotParam.nRunRate);
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(m_RobotParam.nRunRate > 0);
    m_RobotParam.nRunRate = (int)((double)(GAME_FPS * m_RobotParam.nRunRate) / (double)16);
    m_RobotParam.nRunRate = max(m_RobotParam.nRunRate, 0);

    nRetCode = pIniFile->GetInteger("Monster", "StartInterval", 0, &m_RobotParam.nStartInterval);
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(m_RobotParam.nStartInterval > 0);

    nResult = TRUE;
Exit0:
    if (pIniFile)
    {
        pIniFile->Release();
        pIniFile = NULL;
    }
    return nResult;
}
