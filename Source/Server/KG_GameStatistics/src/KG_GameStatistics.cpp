////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_GameStatistics.cpp
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-9-26  13:01:16
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "KG_GameStatistics.h"
#include "PassGen.h"
#include <algorithm>

///////////////////////////////////////////////////////////////////////////////
enum EQUIPMENT_TYPE
{
    EQUIPMENT_TYPE_WEAPON  = 6,
    EQUIPMENT_TYPE_ARMOR   = 7,
    EQUIPMENT_TYPE_TRINKET = 8
};

///////////////////////////////////////////////////////////////////////////////
KG_GameStatistics::KG_GameStatistics()
{
}

KG_GameStatistics::~KG_GameStatistics()
{
}

int KG_GameStatistics::Init(void *pvContext)
{
	int nResult  = false;
	int nRetCode = false;
    int nGameDatabaseInitFlag = false;
    int nPostDatabaseInitFlag = false;
    KG_SERVER_INFO ServerInfo;
    KG_MYSQL_PARAM GameDatabaseParam;
    KG_MYSQL_PARAM PostDatabaseParam;
    KG_EQUIPMENT_TYPE_MAP EquipmentTypeMap;
    const char INI_FILE_NAME[] = "KG_GameStatistics.ini";

    KG_USE_ARGUMENT(pvContext);

    nRetCode = _LoadServerInfo(INI_FILE_NAME, &ServerInfo);
    KGLOG_PROCESS_ERROR(nRetCode && "_LoadServerInfo()");

    nRetCode = _LoadMySQLDatabaseParameter(INI_FILE_NAME, "GameDatabase", &GameDatabaseParam);
    KGLOG_PROCESS_ERROR(nRetCode && "_LoadMySQLDatabaseParameter()");

    nRetCode = _LoadMySQLDatabaseParameter(INI_FILE_NAME, "PostDatabase", &PostDatabaseParam);
    KGLOG_PROCESS_ERROR(nRetCode && "_LoadMySQLDatabaseParameter()");

    nRetCode = _LoadEquipmentInformation(&EquipmentTypeMap);
    KGLOG_PROCESS_ERROR(nRetCode && "_LoadEquipmentInformation()");

    nRetCode = m_GameDatabase.Init(GameDatabaseParam, EquipmentTypeMap, NULL);
    KGLOG_PROCESS_ERROR(nRetCode && "m_GameDatabase.Init()");
    nGameDatabaseInitFlag = true;

    nRetCode = m_PostDatabase.Init(PostDatabaseParam, ServerInfo, NULL);
    KGLOG_PROCESS_ERROR(nRetCode && "m_PostDatabase.Init()");
    nPostDatabaseInitFlag = true;

	nResult = true;
Exit0:
    if (!nResult)
    {
        if (nPostDatabaseInitFlag)
        {
            m_PostDatabase.UnInit(NULL);
            nPostDatabaseInitFlag = false;
        }
        if (nGameDatabaseInitFlag)
        {
            m_GameDatabase.UnInit(NULL);
            nGameDatabaseInitFlag = false;
        }
    }
	return nResult;
}

int KG_GameStatistics::UnInit(void *pvContext)
{
    KG_USE_ARGUMENT(pvContext);

    m_PostDatabase.UnInit(NULL);
    m_GameDatabase.UnInit(NULL);

    return true;
}

int KG_GameStatistics::Run()
{
	int nResult  = false;
	int nRetCode = false;
    KG_TONG_DATA_LIST  TongDataList;
    _PTONG_DATA_VECTOR PTongDataVector;
    KG_ROLE_DATA_LIST  RoleDataList;
    _PROLE_DATA_VECTOR PRoleDataVector;

    nRetCode = m_GameDatabase.GetTongAndRoleDataList(&TongDataList, &RoleDataList);
    KGLOG_PROCESS_ERROR(nRetCode && "m_GameDatabase.GetTongAndRoleDataList()");

    // Get the vector of the points to tong data, and prepare for stat.
    PTongDataVector.clear();
    PTongDataVector.reserve(TongDataList.size());
    for (KG_TONG_DATA_LIST::iterator it = TongDataList.begin(); it != TongDataList.end(); ++it)
        PTongDataVector.push_back(&(*it));

    nRetCode = _StatTongEquipmentPoint(&PTongDataVector);
    KGLOG_PROCESS_ERROR(nRetCode && "_StatTongEquipmentPoint()");

    // Get the vector of the points to role data, and prepare for stat.
    PRoleDataVector.clear();
    PRoleDataVector.reserve(RoleDataList.size());
    for (KG_ROLE_DATA_LIST::iterator it = RoleDataList.begin(); it != RoleDataList.end(); ++it)
        PRoleDataVector.push_back(&(*it));

    nRetCode = _StatRoleMoney(&PRoleDataVector);
    KGLOG_PROCESS_ERROR(nRetCode && "_StatRoleMoney()");

    nRetCode = _StatRoleTrain(&PRoleDataVector);
    KGLOG_PROCESS_ERROR(nRetCode && "_StatRoleTrain()");

    nRetCode = _StatRoleRepute(&PRoleDataVector);
    KGLOG_PROCESS_ERROR(nRetCode && "_StatRoleRepute()");

    nRetCode = _StatRoleAchievement(&PRoleDataVector);
    KGLOG_PROCESS_ERROR(nRetCode && "_StatRoleAchievement()");

    nRetCode = _StatRoleEquipmentPoint(&PRoleDataVector);
    KGLOG_PROCESS_ERROR(nRetCode && "_StatRoleEquipmentPoint()");    

	nResult = true;
Exit0:
	return nResult;
}

int KG_GameStatistics::_LoadServerInfo(const char cszIniFileName[], KG_SERVER_INFO *pRetServerInfo)
{
	int nResult  = false;
	int nRetCode = false;
    IIniFile *piIniFile = NULL;

    ASSERT(cszIniFileName);
    ASSERT(cszIniFileName[0]);
    ASSERT(pRetServerInfo);

    piIniFile = g_OpenIniFile(cszIniFileName);
    KGLOG_PROCESS_ERROR(piIniFile && ".ini file not found.");

    nRetCode = piIniFile->GetInteger("ServerInfo", "ServerType", 0, &(pRetServerInfo->nServerType));
    KGLOG_PROCESS_ERROR(nRetCode && "Failed to load [ServerInfo]ServerType");

    nRetCode = piIniFile->GetString(
        "ServerInfo", "ServerName", NULL,
        pRetServerInfo->szServerName, sizeof(pRetServerInfo->szServerName)
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Failed to load [ServerInfo]ServerName");

	nResult = true;
Exit0:
    if (!nResult)
    {
        KGLogPrintf(KGLOG_DEBUG, "Failed to load server information from `%s`", cszIniFileName);
    }
	return nResult;
}

int KG_GameStatistics::_LoadMySQLDatabaseParameter(
    const char cszIniFileName[], const char cszSectionName[], KG_MYSQL_PARAM *pRetParam
)
{
	int nResult  = false;
	int nRetCode = false;
    IIniFile *piIniFile = NULL;
    char szPassword[64];

    ASSERT(cszIniFileName);
    ASSERT(cszIniFileName[0]);
    ASSERT(cszSectionName);
    ASSERT(cszSectionName[0]);
    ASSERT(pRetParam);

    piIniFile = g_OpenIniFile(cszIniFileName);
    KGLOG_PROCESS_ERROR(piIniFile && ".ini file not found.");

    nRetCode = piIniFile->GetString(
        cszSectionName, "Server", NULL, pRetParam->szServer, sizeof(pRetParam->szServer)
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Failed to load value of key `Server`");

    nRetCode = piIniFile->GetString(
        cszSectionName, "CharacterSet", NULL, pRetParam->szCharacterSet, sizeof(pRetParam->szCharacterSet)
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Failed to load value of key `CharacterSet`");

    nRetCode = piIniFile->GetInteger(cszSectionName, "Port", 0, &(pRetParam->nPort));
    KGLOG_PROCESS_ERROR(nRetCode && "Failed to load value of key `Port`");

    nRetCode = piIniFile->GetString(
        cszSectionName, "Database", NULL, pRetParam->szDatabase, sizeof(pRetParam->szDatabase)
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Failed to load value of key `Database`");

    nRetCode = piIniFile->GetString(
        cszSectionName, "UserName", NULL, pRetParam->szUserName, sizeof(pRetParam->szUserName)
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Failed to load value of key `UserName`");

    nRetCode = piIniFile->GetString(
        cszSectionName, "Password", NULL, szPassword, sizeof(szPassword)
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Failed to load value of key `Password`");

#ifdef _DEBUG
    strncpy(pRetParam->szPassword, szPassword, sizeof(pRetParam->szPassword));
    KGLOG_PROCESS_ERROR(pRetParam->szPassword[sizeof(pRetParam->szPassword) -1] == '\0');
#else
    nRetCode = (int)strlen(szPassword);
    KGLOG_PROCESS_ERROR((nRetCode == PG_RESULTLENSTD) && "Password error");
    nRetCode = SimplyDecryptPassword(pRetParam->szPassword, szPassword);
    KGLOG_PROCESS_ERROR(nRetCode && "Password error");
#endif

	nResult = true;
Exit0:
    if (!nResult)
    {
        KGLogPrintf(
            KGLOG_DEBUG, "Failed to load MySQL parameters of section `%s` from `%s`",
            cszSectionName, cszIniFileName
        );
    }
    KG_COM_RELEASE(piIniFile);
	return nResult;
}

int KG_GameStatistics::_LoadEquipmentInformation(KG_EQUIPMENT_TYPE_MAP *pRetEquipmentTypeMap)
{
    int nResult  = false;
    int nRetCode = false;
    KG_EQUIPMENT_POINT_MAP EquipmentPointMap;
    pair<KG_EQUIPMENT_TYPE_MAP::iterator, bool> InsertResult;

    ASSERT(pRetEquipmentTypeMap);
    pRetEquipmentTypeMap->clear();

    nRetCode = _LoadItemTabFile("settings/item/Custom_Weapon.tab", &EquipmentPointMap);
    KGLOG_PROCESS_ERROR(nRetCode && "_LoadItemTabFile()");

    InsertResult = pRetEquipmentTypeMap->insert(make_pair(EQUIPMENT_TYPE_WEAPON, EquipmentPointMap));
    ASSERT(InsertResult.second);

    nRetCode = _LoadItemTabFile("settings/item/Custom_Armor.tab", &EquipmentPointMap);
    KGLOG_PROCESS_ERROR(nRetCode && "_LoadItemTabFile()");

    InsertResult = pRetEquipmentTypeMap->insert(make_pair(EQUIPMENT_TYPE_ARMOR, EquipmentPointMap));
    ASSERT(InsertResult.second);

    nRetCode = _LoadItemTabFile("settings/item/Custom_Trinket.tab", &EquipmentPointMap);
    KGLOG_PROCESS_ERROR(nRetCode && "_LoadItemTabFile()");

    InsertResult = pRetEquipmentTypeMap->insert(make_pair(EQUIPMENT_TYPE_TRINKET, EquipmentPointMap));
    ASSERT(InsertResult.second);

    nResult = true;
Exit0:
    return nResult;
}

int KG_GameStatistics::_LoadItemTabFile(
    const char cszTabFileName[], KG_EQUIPMENT_POINT_MAP *pRetEquipmentPointMap
)
{
	int nResult  = false;
	int nRetCode = false;
    ITabFile *piTabFile = NULL;
    int nTotalRowCount = 0;
    int nDefaultID = 0;
    int nDefaultLevel =0;
    int nDefaultQuality = 0;
    int nItemID = 0;
    int nLevel = 0;
    int nQuality = 0;
    float fItemPoint = 0.0;
    pair<KG_EQUIPMENT_POINT_MAP::iterator, bool> InsertResult;
    static const float QUALITY_FACTOR_ARRAY[] = {0.0f, 0.8f, 1.0f, 1.4f, 1.8f};

    // 装备等级换算办法：装备级别*相应的品质系数（白色品质0.8倍，绿色1倍，蓝色品质1.4倍，紫色品质1.8倍）

    ASSERT(cszTabFileName);
    ASSERT(cszTabFileName[0]);
    ASSERT(pRetEquipmentPointMap);

    pRetEquipmentPointMap->clear();

    piTabFile = g_OpenTabFile(cszTabFileName);
    KGLOG_PROCESS_ERROR(piTabFile);

    nTotalRowCount = piTabFile->GetHeight();
    KGLOG_PROCESS_ERROR(nTotalRowCount > 1);

    // The second row is the default value.
    nRetCode = piTabFile->GetInteger(2, "ID", 0, &nDefaultID);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTabFile->GetInteger(2, "Level", 0, &nDefaultLevel);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTabFile->GetInteger(2, "Quality", 0, &nDefaultQuality);
    KGLOG_PROCESS_ERROR(nRetCode);

    for (int i = 3; i <= nTotalRowCount; i++)
    {
        nRetCode = piTabFile->GetInteger(i, "ID", nDefaultID, &nItemID);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->GetInteger(i, "Level", nDefaultLevel, &nLevel);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->GetInteger(i, "Quality", nDefaultQuality, &nQuality);
        KGLOG_PROCESS_ERROR(nRetCode);

        KGLOG_PROCESS_ERROR(nQuality >= 1);    // The quality could not be 0.
        KGLOG_PROCESS_ERROR(nQuality < sizeof(QUALITY_FACTOR_ARRAY) / sizeof(QUALITY_FACTOR_ARRAY[0]));
        fItemPoint = nLevel * QUALITY_FACTOR_ARRAY[nQuality];

        InsertResult = pRetEquipmentPointMap->insert(make_pair(nItemID, fItemPoint));
        KGLOG_PROCESS_ERROR(InsertResult.second);
    }

	nResult = true;
Exit0:
    KG_COM_RELEASE(piTabFile);
    if (!nResult)
    {
        KGLogPrintf(KGLOG_DEBUG, "Failed to load tab file : %s", cszTabFileName);
    }
	return nResult;
}

int KG_GameStatistics::_StatTongEquipmentPoint(_PTONG_DATA_VECTOR *pPTongDataVector)
{
    int nResult  = false;
    int nRetCode = false;
    int nTongIndex             = 0;
    int nPreEquipmentPoint     = 0;
    int nCurrentEquipmentPoint = 0;
    int nPreTongRank           = 0;
    int nCurrentTongRank       = 0;
    _PTONG_DATA_VECTOR::iterator itEnd;
    _PTONG_DATA_VECTOR::iterator it;
    _PTONG_DATA pTongData = NULL;
    KG_GAME_STAT_TONG_DATA      GameStatData;
    KG_GAME_STAT_TONG_DATA_LIST GameStatDataList;
    KG_POST_STAT_TONG_DATA      PostStatData;
    KG_POST_STAT_TONG_DATA_LIST PostStatDataList;
    const int GAME_STAT_TONG_COUNT = 20;
    const int POST_STAT_TONG_COUNT = 100;
    const int SORT_TONG_COUNT = max(GAME_STAT_TONG_COUNT, POST_STAT_TONG_COUNT);

    puts("Start to stat tong members equipment point.");

    ASSERT(pPTongDataVector);

    // Sort the top tong 
    nRetCode = (int)pPTongDataVector->size();
    if (nRetCode >= SORT_TONG_COUNT)
        itEnd = pPTongDataVector->begin() + SORT_TONG_COUNT;
    else
        itEnd = pPTongDataVector->end();

    partial_sort(pPTongDataVector->begin(), itEnd, pPTongDataVector->end(), _CompareTongEquipmentPoint);

    nTongIndex         = 1;    // Start with 1.
    nPreEquipmentPoint = -1;
    nPreTongRank       = 1;
    for (it = pPTongDataVector->begin(); it != itEnd; ++it)
    {
        pTongData = *it;
        ASSERT(pTongData);

        nCurrentEquipmentPoint = (int)pTongData->fEquipmentPoint;
        if (nCurrentEquipmentPoint == nPreEquipmentPoint)
            nCurrentTongRank = nPreTongRank;
        else
            nCurrentTongRank = nTongIndex;
        nPreEquipmentPoint = nCurrentEquipmentPoint;
        nPreTongRank       = nCurrentTongRank;

        if (nTongIndex <= GAME_STAT_TONG_COUNT)
        {
            GameStatData.nTongID         = pTongData->nTongID;
            GameStatData.nMasterID       = pTongData->nMasterID;
            GameStatData.nTongCampID     = pTongData->nTongCampID;
            GameStatData.nMaxMemberCount = pTongData->nMaxMemberCount;
            GameStatData.nValue          = nCurrentEquipmentPoint;
            GameStatData.nRank           = nCurrentTongRank;

            GameStatDataList.push_back(GameStatData);
        }

        if (nTongIndex <= POST_STAT_TONG_COUNT)
        {
            strncpy(PostStatData.szTongName, pTongData->szTongName, sizeof(PostStatData.szTongName));
            KGLOG_PROCESS_ERROR(PostStatData.szTongName[sizeof(PostStatData.szTongName) - 1] == '\0');
            strncpy(PostStatData.szMasterName, pTongData->szMasterName, sizeof(PostStatData.szMasterName));
            KGLOG_PROCESS_ERROR(PostStatData.szMasterName[sizeof(PostStatData.szMasterName) - 1] == '\0');
            PostStatData.nTongCampID     = pTongData->nTongCampID;
            PostStatData.nMaxMemberCount = pTongData->nMaxMemberCount;
            PostStatData.nValue          = nCurrentEquipmentPoint;
            PostStatData.nRank           = nCurrentTongRank;

            PostStatDataList.push_back(PostStatData);
        }

        nTongIndex++;
    }

    nRetCode = m_GameDatabase.InsertTongStatData("Rank_Tong_EquipmentPoint", GameStatDataList);
    KGLOG_PROCESS_ERROR(nRetCode && "m_GameDatabase.InsertTongStatData()");

    nRetCode = m_PostDatabase.InsertTongStatData("xsj_rank_organization", PostStatDataList);
    KGLOG_PROCESS_ERROR(nRetCode && "m_PostDatabase.InsertTongStatData()");

    nResult = true;
Exit0:
    if (nResult)
    {
        puts("Stat tong members equipment level successfully.");
    }
    else
    {
        puts("Failed to stat tong members equipment level.");
    }
    return nResult;
}

int KG_GameStatistics::_StatRoleMoney(_PROLE_DATA_VECTOR *pPRoleDataVector)
{
    int nResult  = false;
    int nRetCode = false;
    int nRoleIndex          = 0;
    int nPreRoleMoney       = 0;
    int nPreRoleRank        = 0;
    int nCurrentRoleRank    = 0;
    _PROLE_DATA_VECTOR::iterator itEnd;
    _PROLE_DATA_VECTOR::iterator it;
    _PROLE_DATA pRoleData = NULL;
    KG_GAME_STAT_ROLE_DATA      GameStatData;
    KG_GAME_STAT_ROLE_DATA_LIST GameStatDataList;
    KG_POST_STAT_ROLE_DATA      PostStatData;
    KG_POST_STAT_ROLE_DATA_LIST PostStatDataList;
    const int GMAE_STAT_ROLE_COUNT = 20;
    const int POST_STAT_ROLE_COUNT = 100;
    const int SORT_ROLE_COUNT = max(GMAE_STAT_ROLE_COUNT, POST_STAT_ROLE_COUNT);

    puts("Start to stat role money.");

    ASSERT(pPRoleDataVector);

    // Sort the top role
    nRetCode = (int)pPRoleDataVector->size();
    if (nRetCode >= SORT_ROLE_COUNT)
        itEnd = pPRoleDataVector->begin() + SORT_ROLE_COUNT;
    else
        itEnd = pPRoleDataVector->end();

    partial_sort(pPRoleDataVector->begin(), itEnd, pPRoleDataVector->end(), _CompareRoleMoney);

    nRoleIndex    = 1;  // Start with 1.
    nPreRoleMoney = -1;
    nPreRoleRank  = 1;
    for (it = pPRoleDataVector->begin(); it != itEnd; ++it)
    {
        pRoleData = *it;
        ASSERT(pRoleData);

        if (pRoleData->nTotalMoney == nPreRoleMoney)
            nCurrentRoleRank = nPreRoleRank;
        else
            nCurrentRoleRank = nRoleIndex;
        nPreRoleMoney = pRoleData->nTotalMoney;
        nPreRoleRank  = nCurrentRoleRank;

        if (nRoleIndex <= GMAE_STAT_ROLE_COUNT)
        {
            GameStatData.nRoleID        = pRoleData->nRoleID;
            GameStatData.nTongID        = pRoleData->nTongID;
            GameStatData.nTongMasterID  = pRoleData->nTongMasterID;
            GameStatData.nRoleLevel     = pRoleData->nRoleLevel;
            GameStatData.nRoleForceID   = pRoleData->nRoleForceID;
            GameStatData.nRoleCampID    = pRoleData->nRoleCampID;
            GameStatData.nValue         = pRoleData->nTotalMoney;
            GameStatData.nRank          = nCurrentRoleRank;

            GameStatDataList.push_back(GameStatData);
        }

        if (nRoleIndex <= POST_STAT_ROLE_COUNT)
        {
            strncpy(PostStatData.szRoleName, pRoleData->szRoleName, sizeof(PostStatData.szRoleName));
            KGLOG_PROCESS_ERROR(PostStatData.szRoleName[sizeof(PostStatData.szRoleName) - 1] == '\0');
            PostStatData.nRoleLevel     = pRoleData->nRoleLevel;
            PostStatData.nRoleForceID   = pRoleData->nRoleForceID;
            PostStatData.nRoleCampID    = pRoleData->nRoleCampID;
            PostStatData.nValue         = pRoleData->nTotalMoney;
            PostStatData.nRank          = nCurrentRoleRank;

            PostStatDataList.push_back(PostStatData);
        }

        nRoleIndex++;
    }

    nRetCode = m_GameDatabase.InsertRoleStatData("Rank_Role_Money", GameStatDataList);
    KGLOG_PROCESS_ERROR(nRetCode && "m_GameDatabase.InsertRoleStatData()");

    nRetCode = m_PostDatabase.InsertRoleStatData("xsj_rank_money", PostStatDataList);
    KGLOG_PROCESS_ERROR(nRetCode && "m_PostDatabase.InsertRoleStatData()");

	nResult = true;
Exit0:
    if (nResult)
    {
        puts("Stat role money successfully.");
    }
    else
    {
        puts("Failed to stat role money.");
    }
	return nResult;
}

int KG_GameStatistics::_StatRoleTrain(_PROLE_DATA_VECTOR *pPRoleDataVector)
{
    int nResult  = false;
    int nRetCode = false;
    int nRoleIndex          = 0;
    int nPreRoleTrain       = 0;
    int nPreRoleRank        = 0;
    int nCurrentRoleRank    = 0;
    _PROLE_DATA_VECTOR::iterator itEnd;
    _PROLE_DATA_VECTOR::iterator it;
    _PROLE_DATA pRoleData = NULL;
    KG_GAME_STAT_ROLE_DATA      GameStatData;
    KG_GAME_STAT_ROLE_DATA_LIST GameStatDataList;
    KG_POST_STAT_ROLE_DATA      PostStatData;
    KG_POST_STAT_ROLE_DATA_LIST PostStatDataList;
    const int GMAE_STAT_ROLE_COUNT = 20;
    const int POST_STAT_ROLE_COUNT = 100;
    const int SORT_ROLE_COUNT = max(GMAE_STAT_ROLE_COUNT, POST_STAT_ROLE_COUNT);

    puts("Start to stat role repute.");

    ASSERT(pPRoleDataVector);

    // Sort the top role
    nRetCode = (int)pPRoleDataVector->size();
    if (nRetCode >= SORT_ROLE_COUNT)
        itEnd = pPRoleDataVector->begin() + SORT_ROLE_COUNT;
    else
        itEnd = pPRoleDataVector->end();

    partial_sort(pPRoleDataVector->begin(), itEnd, pPRoleDataVector->end(), _CompareRoleTrain);

    nRoleIndex    = 1;  // Start with 1.
    nPreRoleTrain = -1;
    nPreRoleRank  = 1;
    for (it = pPRoleDataVector->begin(); it != itEnd; ++it)
    {
        pRoleData = *it;
        ASSERT(pRoleData);

        if (pRoleData->nTotalTrain == nPreRoleTrain)
            nCurrentRoleRank = nPreRoleRank;
        else
            nCurrentRoleRank = nRoleIndex;
        nPreRoleTrain = pRoleData->nTotalTrain;
        nPreRoleRank  = nCurrentRoleRank;

        if (nRoleIndex <= GMAE_STAT_ROLE_COUNT)
        {
            GameStatData.nRoleID        = pRoleData->nRoleID;
            GameStatData.nTongID        = pRoleData->nTongID;
            GameStatData.nTongMasterID  = pRoleData->nTongMasterID;
            GameStatData.nRoleLevel     = pRoleData->nRoleLevel;
            GameStatData.nRoleForceID   = pRoleData->nRoleForceID;
            GameStatData.nRoleCampID    = pRoleData->nRoleCampID;
            GameStatData.nValue         = pRoleData->nTotalTrain;
            GameStatData.nRank          = nCurrentRoleRank;

            GameStatDataList.push_back(GameStatData);
        }

        if (nRoleIndex <= POST_STAT_ROLE_COUNT)
        {
            strncpy(PostStatData.szRoleName, pRoleData->szRoleName, sizeof(PostStatData.szRoleName));
            KGLOG_PROCESS_ERROR(PostStatData.szRoleName[sizeof(PostStatData.szRoleName) - 1] == '\0');
            PostStatData.nRoleLevel     = pRoleData->nRoleLevel;
            PostStatData.nRoleForceID   = pRoleData->nRoleForceID;
            PostStatData.nRoleCampID    = pRoleData->nRoleCampID;
            PostStatData.nValue         = pRoleData->nTotalTrain;
            PostStatData.nRank          = nCurrentRoleRank;

            PostStatDataList.push_back(PostStatData);
        }

        nRoleIndex++;
    }

    nRetCode = m_GameDatabase.InsertRoleStatData("Rank_Role_Train", GameStatDataList);
    KGLOG_PROCESS_ERROR(nRetCode && "m_GameDatabase.InsertRoleStatData()");

    nRetCode = m_PostDatabase.InsertRoleStatData("xsj_rank_exercise", PostStatDataList);
    KGLOG_PROCESS_ERROR(nRetCode && "m_PostDatabase.InsertRoleStatData()");

	nResult = true;
Exit0:
    if (nResult)
    {
        puts("Stat role train successfully.");
    }
    else
    {
        puts("Failed to stat role train.");
    }
	return nResult;
}

int KG_GameStatistics::_StatRoleRepute(_PROLE_DATA_VECTOR *pPRoleDataVector)
{
    int nResult  = false;
    int nRetCode = false;
    int nRoleIndex          = 0;
    int nPreRoleRepute      = 0;
    int nPreRoleRank        = 0;
    int nCurrentRoleRank    = 0;
    _PROLE_DATA_VECTOR::iterator itEnd;
    _PROLE_DATA_VECTOR::iterator it;
    _PROLE_DATA pRoleData = NULL;
    KG_GAME_STAT_ROLE_DATA      GameStatData;
    KG_GAME_STAT_ROLE_DATA_LIST GameStatDataList;
    KG_POST_STAT_ROLE_DATA      PostStatData;
    KG_POST_STAT_ROLE_DATA_LIST PostStatDataList;
    const int GMAE_STAT_ROLE_COUNT = 20;
    const int POST_STAT_ROLE_COUNT = 100;
    const int SORT_ROLE_COUNT = max(GMAE_STAT_ROLE_COUNT, POST_STAT_ROLE_COUNT);

    puts("Start to stat role repute.");

    ASSERT(pPRoleDataVector);

    // Sort the top role
    nRetCode = (int)pPRoleDataVector->size();
    if (nRetCode >= SORT_ROLE_COUNT)
        itEnd = pPRoleDataVector->begin() + SORT_ROLE_COUNT;
    else
        itEnd = pPRoleDataVector->end();

    partial_sort(pPRoleDataVector->begin(), itEnd, pPRoleDataVector->end(), _CompareRoleRepute);

    nRoleIndex     = 1;     // Start with 1.
    nPreRoleRepute = -1;
    nPreRoleRank   = 1;
    for (it = pPRoleDataVector->begin(); it != itEnd; ++it)
    {
        pRoleData = *it;
        ASSERT(pRoleData);

        if (pRoleData->nTotalRepute == nPreRoleRepute)
            nCurrentRoleRank = nPreRoleRank;
        else
            nCurrentRoleRank = nRoleIndex;
        nPreRoleRepute = pRoleData->nTotalRepute;
        nPreRoleRank   = nCurrentRoleRank;

        if (nRoleIndex <= GMAE_STAT_ROLE_COUNT)
        {
            GameStatData.nRoleID        = pRoleData->nRoleID;
            GameStatData.nTongID        = pRoleData->nTongID;
            GameStatData.nTongMasterID  = pRoleData->nTongMasterID;
            GameStatData.nRoleLevel     = pRoleData->nRoleLevel;
            GameStatData.nRoleForceID   = pRoleData->nRoleForceID;
            GameStatData.nRoleCampID    = pRoleData->nRoleCampID;
            GameStatData.nValue         = pRoleData->nTotalRepute;
            GameStatData.nRank          = nCurrentRoleRank;

            GameStatDataList.push_back(GameStatData);
        }

        if (nRoleIndex <= POST_STAT_ROLE_COUNT)
        {
            strncpy(PostStatData.szRoleName, pRoleData->szRoleName, sizeof(PostStatData.szRoleName));
            KGLOG_PROCESS_ERROR(PostStatData.szRoleName[sizeof(PostStatData.szRoleName) - 1] == '\0');
            PostStatData.nRoleLevel     = pRoleData->nRoleLevel;
            PostStatData.nRoleForceID   = pRoleData->nRoleForceID;
            PostStatData.nRoleCampID    = pRoleData->nRoleCampID;
            PostStatData.nValue         = pRoleData->nTotalRepute;
            PostStatData.nRank          = nCurrentRoleRank;

            PostStatDataList.push_back(PostStatData);
        }

        nRoleIndex++;
    }

    nRetCode = m_GameDatabase.InsertRoleStatData("Rank_Role_Repute", GameStatDataList);
    KGLOG_PROCESS_ERROR(nRetCode && "m_GameDatabase.InsertRoleStatData()");

    nRetCode = m_PostDatabase.InsertRoleStatData("xsj_rank_prestige", PostStatDataList);
    KGLOG_PROCESS_ERROR(nRetCode && "m_PostDatabase.InsertRoleStatData()");

    nResult = true;
Exit0:
    if (nResult)
    {
        puts("Stat role repute successfully.");
    }
    else
    {
        puts("Failed to stat role repute.");
    }
    return nResult;
}

int KG_GameStatistics::_StatRoleAchievement(_PROLE_DATA_VECTOR *pPRoleDataVector)
{
    int nResult  = false;
    int nRetCode = false;
    int nRoleIndex          = 0;
    int nPreRoleAchievement = 0;
    int nPreRoleRank        = 0;
    int nCurrentRoleRank    = 0;
    _PROLE_DATA_VECTOR::iterator itEnd;
    _PROLE_DATA_VECTOR::iterator it;
    _PROLE_DATA pRoleData = NULL;
    KG_GAME_STAT_ROLE_DATA      GameStatData;
    KG_GAME_STAT_ROLE_DATA_LIST GameStatDataList;
    KG_POST_STAT_ROLE_DATA      PostStatData;
    KG_POST_STAT_ROLE_DATA_LIST PostStatDataList;
    const int GMAE_STAT_ROLE_COUNT = 20;
    const int POST_STAT_ROLE_COUNT = 100;
    const int SORT_ROLE_COUNT = max(GMAE_STAT_ROLE_COUNT, POST_STAT_ROLE_COUNT);

    puts("Start to stat role achievement.");

    ASSERT(pPRoleDataVector);

    // Sort the top role
    nRetCode = (int)pPRoleDataVector->size();
    if (nRetCode >= SORT_ROLE_COUNT)
        itEnd = pPRoleDataVector->begin() + SORT_ROLE_COUNT;
    else
        itEnd = pPRoleDataVector->end();

    partial_sort(pPRoleDataVector->begin(), itEnd, pPRoleDataVector->end(), _CompareRoleAchievement);

    nRoleIndex          = 1; // Start with 1.
    nPreRoleAchievement = -1;
    nPreRoleRank        = 1;
    for (it = pPRoleDataVector->begin(); it != itEnd; ++it)
    {
        pRoleData = *it;
        ASSERT(pRoleData);

        if (pRoleData->nAchievementRecord == nPreRoleAchievement)
            nCurrentRoleRank = nPreRoleRank;
        else
            nCurrentRoleRank = nRoleIndex;
        nPreRoleAchievement = pRoleData->nAchievementRecord;
        nPreRoleRank        = nCurrentRoleRank;

        if (nRoleIndex <= GMAE_STAT_ROLE_COUNT)
        {
            GameStatData.nRoleID        = pRoleData->nRoleID;
            GameStatData.nTongID        = pRoleData->nTongID;
            GameStatData.nTongMasterID  = pRoleData->nTongMasterID;
            GameStatData.nRoleLevel     = pRoleData->nRoleLevel;
            GameStatData.nRoleForceID   = pRoleData->nRoleForceID;
            GameStatData.nRoleCampID    = pRoleData->nRoleCampID;
            GameStatData.nValue         = pRoleData->nAchievementRecord;
            GameStatData.nRank          = nCurrentRoleRank;

            GameStatDataList.push_back(GameStatData);
        }

        if (nRoleIndex <= POST_STAT_ROLE_COUNT)
        {
            strncpy(PostStatData.szRoleName, pRoleData->szRoleName, sizeof(PostStatData.szRoleName));
            KGLOG_PROCESS_ERROR(PostStatData.szRoleName[sizeof(PostStatData.szRoleName) - 1] == '\0');
            PostStatData.nRoleLevel     = pRoleData->nRoleLevel;
            PostStatData.nRoleForceID   = pRoleData->nRoleForceID;
            PostStatData.nRoleCampID    = pRoleData->nRoleCampID;
            PostStatData.nValue         = pRoleData->nAchievementRecord;
            PostStatData.nRank          = nCurrentRoleRank;

            PostStatDataList.push_back(PostStatData);
        }

        nRoleIndex++;
    }

    nRetCode = m_GameDatabase.InsertRoleStatData("Rank_Role_Achievement", GameStatDataList);
    KGLOG_PROCESS_ERROR(nRetCode && "m_GameDatabase.InsertRoleStatData()");

    nRetCode = m_PostDatabase.InsertRoleStatData("xsj_rank_achievement", PostStatDataList);
    KGLOG_PROCESS_ERROR(nRetCode && "m_PostDatabase.InsertRoleStatData()");

    nResult = true;
Exit0:
    if (nResult)
    {
        puts("Stat role achievement successfully.");
    }
    else
    {
        puts("Failed to stat role achievement.");
    }
    return nResult;
}

int KG_GameStatistics::_StatRoleEquipmentPoint(_PROLE_DATA_VECTOR *pPRoleDataVector)
{
    int nResult  = false;
    int nRetCode = false;
    int nRoleIndex = 0;
    int nPreEquipmentPoint = 0;
    int nCurrentEquipmentPoint = 0;
    int nPreRoleRank     = 0;
    int nCurrentRoleRank = 0;
    _PROLE_DATA_VECTOR::iterator itEnd;
    _PROLE_DATA_VECTOR::iterator it;
    _PROLE_DATA pRoleData = NULL;
    KG_GAME_STAT_ROLE_DATA      GameStatData;
    KG_GAME_STAT_ROLE_DATA_LIST GameStatDataList;
    KG_POST_STAT_ROLE_DATA      PostStatData;
    KG_POST_STAT_ROLE_DATA_LIST PostStatDataList;
    const int GMAE_STAT_ROLE_COUNT = 20;
    const int POST_STAT_ROLE_COUNT = 100;
    const int SORT_ROLE_COUNT = max(GMAE_STAT_ROLE_COUNT, POST_STAT_ROLE_COUNT);

    puts("Start to stat role equipment point.");

    ASSERT(pPRoleDataVector);

    // Sort the top role
    nRetCode = (int)pPRoleDataVector->size();
    if (nRetCode >= SORT_ROLE_COUNT)
        itEnd = pPRoleDataVector->begin() + SORT_ROLE_COUNT;
    else
        itEnd = pPRoleDataVector->end();

    partial_sort(pPRoleDataVector->begin(), itEnd, pPRoleDataVector->end(), _CompareRoleEquipmentPoint);

    nRoleIndex         = 1; // Start with 1.
    nPreEquipmentPoint = -1;
    nPreRoleRank       = 1;
    for (it = pPRoleDataVector->begin(); it != itEnd; ++it)
    {
        pRoleData = *it;
        ASSERT(pRoleData);

        nCurrentEquipmentPoint = (int)pRoleData->fEquipmentPoint;
        if (nCurrentEquipmentPoint == nPreEquipmentPoint)
            nCurrentRoleRank = nPreRoleRank;
        else
            nCurrentRoleRank = nRoleIndex;
        nPreEquipmentPoint = nCurrentEquipmentPoint;
        nPreRoleRank       = nCurrentRoleRank;

        if (nRoleIndex <= GMAE_STAT_ROLE_COUNT)
        {
            GameStatData.nRoleID        = pRoleData->nRoleID;
            GameStatData.nTongID        = pRoleData->nTongID;
            GameStatData.nTongMasterID  = pRoleData->nTongMasterID;
            GameStatData.nRoleLevel     = pRoleData->nRoleLevel;
            GameStatData.nRoleForceID   = pRoleData->nRoleForceID;
            GameStatData.nRoleCampID    = pRoleData->nRoleCampID;
            GameStatData.nValue         = nCurrentEquipmentPoint;
            GameStatData.nRank          = nCurrentRoleRank;

            GameStatDataList.push_back(GameStatData);
        }

        if (nRoleIndex <= POST_STAT_ROLE_COUNT)
        {
            strncpy(PostStatData.szRoleName, pRoleData->szRoleName, sizeof(PostStatData.szRoleName));
            KGLOG_PROCESS_ERROR(PostStatData.szRoleName[sizeof(PostStatData.szRoleName) - 1] == '\0');
            PostStatData.nRoleLevel     = pRoleData->nRoleLevel;
            PostStatData.nRoleForceID   = pRoleData->nRoleForceID;
            PostStatData.nRoleCampID    = pRoleData->nRoleCampID;
            PostStatData.nValue         = nCurrentEquipmentPoint;
            PostStatData.nRank          = nCurrentRoleRank;

            PostStatDataList.push_back(PostStatData);
        }

        nRoleIndex++;
    }

    nRetCode = m_GameDatabase.InsertRoleStatData("Rank_Role_EquipmentPoint", GameStatDataList);
    KGLOG_PROCESS_ERROR(nRetCode && "m_GameDatabase.InsertRoleStatData()");

    nRetCode = m_PostDatabase.InsertRoleStatData("xsj_rank_armor", PostStatDataList);
    KGLOG_PROCESS_ERROR(nRetCode && "m_PostDatabase.InsertRoleStatData()");

    nResult = true;
Exit0:
    if (nResult)
    {
        puts("Stat top role by equipment point successfully.");
    }
    else
    {
        puts("Failed to stat top role by equipment point.");
    }
    return nResult;
}


bool KG_GameStatistics::_CompareTongEquipmentPoint(const _PTONG_DATA cLeft, const _PTONG_DATA cRight)
{
    return cLeft->fEquipmentPoint > cRight->fEquipmentPoint;
}

bool KG_GameStatistics::_CompareRoleMoney(const _PROLE_DATA cLeft, const _PROLE_DATA cRight)
{
    return cLeft->nTotalMoney > cRight->nTotalMoney;
}

bool KG_GameStatistics::_CompareRoleTrain(const _PROLE_DATA cLeft, const _PROLE_DATA cRight)
{
    return cLeft->nTotalTrain > cRight->nTotalTrain;
}

bool KG_GameStatistics::_CompareRoleRepute(const _PROLE_DATA cLeft, const _PROLE_DATA cRight)
{
    return cLeft->nTotalRepute > cRight->nTotalRepute;
}

bool KG_GameStatistics::_CompareRoleAchievement(const _PROLE_DATA cLeft, const _PROLE_DATA cRight)
{
    return cLeft->nAchievementRecord > cRight->nAchievementRecord;
}

bool KG_GameStatistics::_CompareRoleEquipmentPoint(const _PROLE_DATA cLeft, const _PROLE_DATA cRight)
{
    return cLeft->fEquipmentPoint > cRight->fEquipmentPoint;
}
