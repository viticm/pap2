////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_GameDatabase.cpp
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-12-11  15:07:01
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "KG_GameDatabase.h"


KG_GameDatabase::KG_GameDatabase()
{
    m_pMySQL = NULL;
}

KG_GameDatabase::~KG_GameDatabase()
{
}

int KG_GameDatabase::Init(
    const KG_MYSQL_PARAM &crMySQLParam, const KG_EQUIPMENT_TYPE_MAP &crEquipmentTypeMap,
    void *pvContext
)
{
	int nResult  = false;
	int nRetCode = false;

    KGLOG_PROCESS_ERROR(crMySQLParam.szServer);
    KGLOG_PROCESS_ERROR(crMySQLParam.szDatabase);
    KGLOG_PROCESS_ERROR(crMySQLParam.szUserName);
    KGLOG_PROCESS_ERROR(crMySQLParam.szPassword);

    KG_USE_ARGUMENT(pvContext);

    ASSERT(m_pMySQL == NULL);
    m_pMySQL = KG_ConnectMySQLDatabase(crMySQLParam);
    KGLOG_PROCESS_ERROR(m_pMySQL && "KG_ConnectMySQLDatabase()");

    m_EquipmentTypeMap = crEquipmentTypeMap;

	nResult = true;
Exit0:
    if (!nResult)
    {
        if (m_pMySQL)
        {
            mysql_close(m_pMySQL);
            m_pMySQL = NULL;
        }
    }
	return nResult;
}

int KG_GameDatabase::UnInit(void *pvContext)
{
    KG_USE_ARGUMENT(pvContext);

    if (m_pMySQL)
    {
        mysql_close(m_pMySQL);
        m_pMySQL = NULL;
    }

    return true;
}

int KG_GameDatabase::GetTongAndRoleDataList(
    KG_TONG_DATA_LIST *pRetTongDataList, KG_ROLE_DATA_LIST *pRetRoleDataList
)
{
	int nResult  = false;
	int nRetCode = false;
    KG_TONG_DATA *pTongData = NULL;
    KG_ROLE_DATA_LIST::iterator itRoleData;
    _ROLE_TONG_MAP::iterator itRoleTong;
    const char SQL_SET_REPEATABLE_READ[] = "set session transaction isolation level repeatable read";
    const char SQL_START_TRANSACTION[]   = "start transaction with consistent snapshot";

    KGLOG_PROCESS_ERROR(pRetTongDataList);
    KGLOG_PROCESS_ERROR(pRetRoleDataList);

    // Start the transaction with repeatable read and make sure all data read is the snapshot of database.
    ASSERT(m_pMySQL);
    nRetCode = mysql_query(m_pMySQL, SQL_SET_REPEATABLE_READ);
    KGD_MYSQL_PROCESS_ERROR(nRetCode == 0, m_pMySQL, SQL_SET_REPEATABLE_READ);

    nRetCode = mysql_query(m_pMySQL, SQL_START_TRANSACTION);
    KGD_MYSQL_PROCESS_ERROR(nRetCode == 0, m_pMySQL, SQL_START_TRANSACTION); 

    nRetCode = _GetTongInformation(pRetTongDataList);
    KGLOG_PROCESS_ERROR(nRetCode && "_GetTongInformation()");

    nRetCode = _GetRoleInformation(pRetRoleDataList);
    KGLOG_PROCESS_ERROR(nRetCode && "_GetRoleInformation()");

    // Find the tong which role belong to and add role equipment point to the tong.
    for (itRoleData = pRetRoleDataList->begin(); itRoleData != pRetRoleDataList->end(); ++itRoleData)
    {
        itRoleTong = m_RoleTongMap.find(itRoleData->nRoleID);
        if (itRoleTong != m_RoleTongMap.end())
        {
            pTongData = itRoleTong->second;
            ASSERT(pTongData);

            itRoleData->nTongID       = pTongData->nTongID;
            itRoleData->nTongMasterID = pTongData->nMasterID;

            if (itRoleData->nRoleID == pTongData->nMasterID)         // the master of tong
            {
                strncpy(pTongData->szMasterName, itRoleData->szRoleName, sizeof(pTongData->szMasterName));
                KGLOG_PROCESS_ERROR(pTongData->szMasterName[sizeof(pTongData->szMasterName) - 1] == '\0');
            }
            pTongData->fEquipmentPoint += itRoleData->fEquipmentPoint;            
        }
        else    // The role not belong to any tong.
        {
            itRoleData->nTongID       = -1;
            itRoleData->nTongMasterID = -1;
        }
    }

	nResult = true;
Exit0:
    if (nResult)
    {
        mysql_commit(m_pMySQL);
    }
    else
    {
        mysql_rollback(m_pMySQL);       
    }
	return nResult;
}

int KG_GameDatabase::InsertTongStatData(
    const char cszKeyName[], const KG_GAME_STAT_TONG_DATA_LIST &crTongDataList
)
{
    int nResult  = false;
    int nRetCode = false;
    int nLuaArrayIndex = 0;
    unsigned char byLuaData[4096];
    unsigned char *pbyWritePos = NULL;
    unsigned char *pbyTailPos  = NULL;
    unsigned char *pbyTable    = NULL;
    unsigned char *pbySubTable = NULL;
    KG_GAME_STAT_TONG_DATA_LIST::const_iterator cit;

    ASSERT(cszKeyName);
    ASSERT(cszKeyName[0]);

    // ---------------------------------- Get the lua data -----------------------------------------
    pbyWritePos = byLuaData;
    pbyTailPos  = byLuaData + sizeof(byLuaData);

    pbyTable = LuaPackage_NewTable(pbyWritePos, pbyTailPos - pbyWritePos);
    KGLOG_PROCESS_ERROR(pbyTable);
    pbyWritePos = pbyTable;

    nLuaArrayIndex = 1; // start with 1.
    for (cit = crTongDataList.begin(); cit != crTongDataList.end(); ++cit)
    {
        // Push key
        pbyWritePos = LuaPackupNumber(pbyWritePos, pbyTailPos - pbyWritePos, nLuaArrayIndex);
        KGLOG_PROCESS_ERROR(pbyWritePos);

        // ----------------------------------- Push value ------------------------------------------
        pbySubTable = LuaPackage_NewTable(pbyWritePos, pbyTailPos - pbyWritePos);
        KGLOG_PROCESS_ERROR(pbySubTable);
        pbyWritePos = pbySubTable;

        pbyWritePos = LuaPackupString(pbyWritePos, pbyTailPos - pbyWritePos, "TongID");
        KGLOG_PROCESS_ERROR(pbyWritePos);
        pbyWritePos = LuaPackupNumber(pbyWritePos, pbyTailPos - pbyWritePos, cit->nTongID);
        KGLOG_PROCESS_ERROR(pbyWritePos);
        pbyWritePos = LuaPackage_SetTable(pbySubTable, pbyWritePos);
        KGLOG_PROCESS_ERROR(pbyWritePos);

        pbyWritePos = LuaPackupString(pbyWritePos, pbyTailPos - pbyWritePos, "MasterID");
        KGLOG_PROCESS_ERROR(pbyWritePos);
        pbyWritePos = LuaPackupNumber(pbyWritePos, pbyTailPos - pbyWritePos, cit->nMasterID);
        KGLOG_PROCESS_ERROR(pbyWritePos);
        pbyWritePos = LuaPackage_SetTable(pbySubTable, pbyWritePos);
        KGLOG_PROCESS_ERROR(pbyWritePos);     

        pbyWritePos = LuaPackupString(pbyWritePos, pbyTailPos - pbyWritePos, "TongCampID");
        KGLOG_PROCESS_ERROR(pbyWritePos);
        pbyWritePos = LuaPackupNumber(pbyWritePos, pbyTailPos - pbyWritePos, cit->nTongCampID);
        KGLOG_PROCESS_ERROR(pbyWritePos);
        pbyWritePos = LuaPackage_SetTable(pbySubTable, pbyWritePos);
        KGLOG_PROCESS_ERROR(pbyWritePos);

        pbyWritePos = LuaPackupString(pbyWritePos, pbyTailPos - pbyWritePos, "MaxMemberCount");
        KGLOG_PROCESS_ERROR(pbyWritePos);
        pbyWritePos = LuaPackupNumber(pbyWritePos, pbyTailPos - pbyWritePos, cit->nMaxMemberCount);
        KGLOG_PROCESS_ERROR(pbyWritePos);
        pbyWritePos = LuaPackage_SetTable(pbySubTable, pbyWritePos);
        KGLOG_PROCESS_ERROR(pbyWritePos);

        pbyWritePos = LuaPackupString(pbyWritePos, pbyTailPos - pbyWritePos, "RankValue");
        KGLOG_PROCESS_ERROR(pbyWritePos);
        pbyWritePos = LuaPackupNumber(pbyWritePos, pbyTailPos - pbyWritePos, cit->nValue);
        KGLOG_PROCESS_ERROR(pbyWritePos);
        pbyWritePos = LuaPackage_SetTable(pbySubTable, pbyWritePos);
        KGLOG_PROCESS_ERROR(pbyWritePos);

        pbyWritePos = LuaPackupString(pbyWritePos, pbyTailPos - pbyWritePos, "Rank");
        KGLOG_PROCESS_ERROR(pbyWritePos);
        pbyWritePos = LuaPackupNumber(pbyWritePos, pbyTailPos - pbyWritePos, cit->nRank);
        KGLOG_PROCESS_ERROR(pbyWritePos);
        pbyWritePos = LuaPackage_SetTable(pbySubTable, pbyWritePos);
        KGLOG_PROCESS_ERROR(pbyWritePos);
        // -----------------------------------------------------------------------------------------

        pbyWritePos = LuaPackage_SetTable(pbyTable, pbyWritePos);
        KGLOG_PROCESS_ERROR(pbyWritePos);

        nLuaArrayIndex++;
    }
    // ---------------------------------------------------------------------------------------------

    nRetCode = _InsertRankResult(cszKeyName, byLuaData, (unsigned)(pbyWritePos - byLuaData));
    KGLOG_PROCESS_ERROR(nRetCode && "_InsertRankResult()");

    nResult = true;
Exit0:
    return nResult;
}

int KG_GameDatabase::InsertRoleStatData(
    const char cszKeyName[], const KG_GAME_STAT_ROLE_DATA_LIST &crRoleDataList
)
{
    int nResult  = false;
    int nRetCode = false;
    int nLuaArrayIndex = 0;
    unsigned char byLuaData[4096];
    unsigned char *pbyWritePos = NULL;
    unsigned char *pbyTailPos  = NULL;
    unsigned char *pbyTable    = NULL;
    unsigned char *pbySubTable = NULL;
    KG_GAME_STAT_ROLE_DATA_LIST::const_iterator cit;

    ASSERT(cszKeyName);
    ASSERT(cszKeyName[0]);

    // ---------------------------------- Get the lua data -----------------------------------------
    pbyWritePos = byLuaData;
    pbyTailPos  = byLuaData + sizeof(byLuaData);

    pbyTable = LuaPackage_NewTable(pbyWritePos, pbyTailPos - pbyWritePos);
    KGLOG_PROCESS_ERROR(pbyTable);
    pbyWritePos = pbyTable;

    nLuaArrayIndex = 1; // start with 1.
    for (cit = crRoleDataList.begin(); cit != crRoleDataList.end(); ++cit)
    {
        // Push key
        pbyWritePos = LuaPackupNumber(pbyWritePos, pbyTailPos - pbyWritePos, nLuaArrayIndex);
        KGLOG_PROCESS_ERROR(pbyWritePos);

        // ----------------------------------- Push value ------------------------------------------
        pbySubTable = LuaPackage_NewTable(pbyWritePos, pbyTailPos - pbyWritePos);
        KGLOG_PROCESS_ERROR(pbySubTable);
        pbyWritePos = pbySubTable;

        pbyWritePos = LuaPackupString(pbyWritePos, pbyTailPos - pbyWritePos, "RoleID");
        KGLOG_PROCESS_ERROR(pbyWritePos);
        pbyWritePos = LuaPackupNumber(pbyWritePos, pbyTailPos - pbyWritePos, cit->nRoleID);
        KGLOG_PROCESS_ERROR(pbyWritePos);
        pbyWritePos = LuaPackage_SetTable(pbySubTable, pbyWritePos);
        KGLOG_PROCESS_ERROR(pbyWritePos);

        pbyWritePos = LuaPackupString(pbyWritePos, pbyTailPos - pbyWritePos, "TongID");
        KGLOG_PROCESS_ERROR(pbyWritePos);
        pbyWritePos = LuaPackupNumber(pbyWritePos, pbyTailPos - pbyWritePos, cit->nTongID);
        KGLOG_PROCESS_ERROR(pbyWritePos);
        pbyWritePos = LuaPackage_SetTable(pbySubTable, pbyWritePos);
        KGLOG_PROCESS_ERROR(pbyWritePos);     

        pbyWritePos = LuaPackupString(pbyWritePos, pbyTailPos - pbyWritePos, "TongMasterID");
        KGLOG_PROCESS_ERROR(pbyWritePos);
        pbyWritePos = LuaPackupNumber(pbyWritePos, pbyTailPos - pbyWritePos, cit->nTongMasterID);
        KGLOG_PROCESS_ERROR(pbyWritePos);
        pbyWritePos = LuaPackage_SetTable(pbySubTable, pbyWritePos);
        KGLOG_PROCESS_ERROR(pbyWritePos);

        pbyWritePos = LuaPackupString(pbyWritePos, pbyTailPos - pbyWritePos, "RoleLevel");
        KGLOG_PROCESS_ERROR(pbyWritePos);
        pbyWritePos = LuaPackupNumber(pbyWritePos, pbyTailPos - pbyWritePos, cit->nRoleLevel);
        KGLOG_PROCESS_ERROR(pbyWritePos);
        pbyWritePos = LuaPackage_SetTable(pbySubTable, pbyWritePos);
        KGLOG_PROCESS_ERROR(pbyWritePos);

        pbyWritePos = LuaPackupString(pbyWritePos, pbyTailPos - pbyWritePos, "RoleForceID");
        KGLOG_PROCESS_ERROR(pbyWritePos);
        pbyWritePos = LuaPackupNumber(pbyWritePos, pbyTailPos - pbyWritePos, cit->nRoleForceID);
        KGLOG_PROCESS_ERROR(pbyWritePos);
        pbyWritePos = LuaPackage_SetTable(pbySubTable, pbyWritePos);
        KGLOG_PROCESS_ERROR(pbyWritePos);

        pbyWritePos = LuaPackupString(pbyWritePos, pbyTailPos - pbyWritePos, "RoleCampID");
        KGLOG_PROCESS_ERROR(pbyWritePos);
        pbyWritePos = LuaPackupNumber(pbyWritePos, pbyTailPos - pbyWritePos, cit->nRoleCampID);
        KGLOG_PROCESS_ERROR(pbyWritePos);
        pbyWritePos = LuaPackage_SetTable(pbySubTable, pbyWritePos);
        KGLOG_PROCESS_ERROR(pbyWritePos);

        pbyWritePos = LuaPackupString(pbyWritePos, pbyTailPos - pbyWritePos, "RankValue");
        KGLOG_PROCESS_ERROR(pbyWritePos);
        pbyWritePos = LuaPackupNumber(pbyWritePos, pbyTailPos - pbyWritePos, cit->nValue);
        KGLOG_PROCESS_ERROR(pbyWritePos);
        pbyWritePos = LuaPackage_SetTable(pbySubTable, pbyWritePos);
        KGLOG_PROCESS_ERROR(pbyWritePos);

        pbyWritePos = LuaPackupString(pbyWritePos, pbyTailPos - pbyWritePos, "Rank");
        KGLOG_PROCESS_ERROR(pbyWritePos);
        pbyWritePos = LuaPackupNumber(pbyWritePos, pbyTailPos - pbyWritePos, cit->nRank);
        KGLOG_PROCESS_ERROR(pbyWritePos);
        pbyWritePos = LuaPackage_SetTable(pbySubTable, pbyWritePos);
        KGLOG_PROCESS_ERROR(pbyWritePos);
        // -----------------------------------------------------------------------------------------

        pbyWritePos = LuaPackage_SetTable(pbyTable, pbyWritePos);
        KGLOG_PROCESS_ERROR(pbyWritePos);

        nLuaArrayIndex++;
    }
    // ---------------------------------------------------------------------------------------------

    nRetCode = _InsertRankResult(cszKeyName, byLuaData, (unsigned)(pbyWritePos - byLuaData));
    KGLOG_PROCESS_ERROR(nRetCode && "_InsertRankResult()");

    nResult = true;
Exit0:
    return nResult;
}

int KG_GameDatabase::_GetTongInformation(KG_TONG_DATA_LIST *pRetTongDataList)
{
	int nResult  = false;
	int nRetCode = false;
    MYSQL_RES *pTongSet = NULL;
    MYSQL_ROW Row = NULL;
    unsigned long *pulLengths = NULL;
    KG_TONG_DATA *pTongData = NULL;
    int nParsedTongCount = 0;
    const char QUERY_TONG_LIST[] = "select `ID`, `Data` from `Tong`";

    puts("Start parse tong information.");

    ASSERT(pRetTongDataList);
    pRetTongDataList->clear();
    
    ASSERT(m_pMySQL);
    nRetCode = mysql_query(m_pMySQL, QUERY_TONG_LIST);
    KGD_MYSQL_PROCESS_ERROR(nRetCode == 0, m_pMySQL, QUERY_TONG_LIST);

    pTongSet = mysql_use_result(m_pMySQL);
    KGD_MYSQL_PROCESS_ERROR(pTongSet, m_pMySQL, "mysql_use_result()");

    m_RoleTongMap.clear();
    while (true)
    {
        Row = mysql_fetch_row(pTongSet);
        if (Row == NULL)
        {
            nRetCode = (int)mysql_errno(m_pMySQL);
            KGD_MYSQL_PROCESS_ERROR(nRetCode == 0, m_pMySQL, "mysql_fetch_row()");
            break;  // No more row
        }

        pulLengths = mysql_fetch_lengths(pTongSet);
        KGD_MYSQL_PROCESS_ERROR(pulLengths, m_pMySQL, "mysql_fetch_lengths()");

        pRetTongDataList->push_back(KG_TONG_DATA());
        pTongData = &(*(pRetTongDataList->rbegin()));

        nRetCode = _ParseTongData(
            atoi(Row[0]), (const unsigned char *)Row[1], pulLengths[1], pTongData
        );
        KGLOG_PROCESS_ERROR(nRetCode && "_ParseTongData()");

        nParsedTongCount++;
        if ((nParsedTongCount % 1000) == 0)
            printf("%d tongs had been parsed.\n", nParsedTongCount);
    }
    printf("Total %d tongs had been parsed.\n", nParsedTongCount);

	nResult = true;
Exit0:
    if (pTongSet)
    {
        mysql_free_result(pTongSet);
        pTongSet = NULL;
    }
	return nResult;
}

int KG_GameDatabase::_GetRoleInformation(KG_ROLE_DATA_LIST *pRetRoleDataList)
{
	int nResult  = false;
	int nRetCode = false;
    MYSQL_RES *pRoleSet = NULL;
    MYSQL_ROW Row = NULL;
    unsigned long *pulLengths = NULL;
    KG_ROLE_DATA *pRoleData = NULL;
    int nParsedRoleCount = 0;
    const char QUERY_ROLE_LIST[] = "select `ID`, `RoleName`, `BaseInfo`, `ExtInfo` from `Role`";

    puts("Start parse role information.");

    ASSERT(pRetRoleDataList);
    pRetRoleDataList->clear();

    ASSERT(m_pMySQL);
    nRetCode = mysql_query(m_pMySQL, QUERY_ROLE_LIST);
    KGD_MYSQL_PROCESS_ERROR(nRetCode == 0, m_pMySQL, QUERY_ROLE_LIST);

    pRoleSet = mysql_use_result(m_pMySQL);
    KGD_MYSQL_PROCESS_ERROR(pRoleSet, m_pMySQL, "mysql_use_result()");

    while (true)
    {
        Row = mysql_fetch_row(pRoleSet);
        if (Row == NULL)
        {
            nRetCode = (int)mysql_errno(m_pMySQL);
            KGD_MYSQL_PROCESS_ERROR(nRetCode == 0, m_pMySQL, "mysql_fetch_row()");

            break;  // No more row
        }

        pulLengths = mysql_fetch_lengths(pRoleSet);
        KGD_MYSQL_PROCESS_ERROR(pulLengths, m_pMySQL, "mysql_fetch_lengths()");

        pRetRoleDataList->push_back(KG_ROLE_DATA());
        pRoleData = &(*(pRetRoleDataList->rbegin()));

        nRetCode = _ParseRoleData(
            atoi(Row[0]), Row[1],
            (const unsigned char *)Row[2], pulLengths[2], 
            (const unsigned char *)Row[3], pulLengths[3],
            pRoleData
        );
        KGLOG_PROCESS_ERROR(nRetCode && "_ParseRoleData()");

        nParsedRoleCount++;
        if ((nParsedRoleCount % 1000) == 0)
            printf("%d roles had been parsed.\n", nParsedRoleCount);
    }
    printf("Total %d roles had been parsed.\n", nParsedRoleCount);

	nResult = true;
Exit0:
    if (pRoleSet)
    {
        mysql_free_result(pRoleSet);
        pRoleSet = NULL;
    }
	return nResult;
}

int KG_GameDatabase::_ParseTongData(
    int nTongID, const unsigned char cbyTongData[], unsigned uTongDataSize,
    KG_TONG_DATA *pRetTongData
)
{
	int nResult  = false;
	int nRetCode = false;
    int nTongInfoInitFlag = false;
    KG_TongInfo TongInfo;
    int nTotalMemberCount = 0;
    int nMemberID         = 0;
    pair<_ROLE_TONG_MAP::iterator, bool>  InsertResult;

    ASSERT(cbyTongData);
    ASSERT(uTongDataSize);
    ASSERT(pRetTongData);

    pRetTongData->nTongID = nTongID;

    nRetCode = TongInfo.Init(cbyTongData, uTongDataSize, NULL);
    KGLOG_PROCESS_ERROR(nRetCode && "TongInfo.Init()");
    nTongInfoInitFlag = false;

    nRetCode = TongInfo.GetTongName(sizeof(pRetTongData->szTongName), pRetTongData->szTongName);
    KGLOG_PROCESS_ERROR(nRetCode && "TongInfo.GetTongName()");

    nRetCode = TongInfo.GetMasterID(&(pRetTongData->nMasterID));
    KGLOG_PROCESS_ERROR(nRetCode && "TongInfo.GetMasterID()");

    nRetCode = TongInfo.GetCampID(&(pRetTongData->nTongCampID));
    KGLOG_PROCESS_ERROR(nRetCode && "TongInfo.GetCampID()");

    nRetCode = TongInfo.GetMaxMemberCount(&(pRetTongData->nMaxMemberCount));
    KGLOG_PROCESS_ERROR(nRetCode && "TongInfo.GetMaxMemberCount()");

    // Set the initial value as 0, and add members equipment point later.
    pRetTongData->fEquipmentPoint = 0.0f;

    nRetCode = TongInfo.GetTotalMemberCount(&nTotalMemberCount);
    KGLOG_PROCESS_ERROR(nRetCode && "TongInfo.GetTotalMemberCount()");

    // The relationship between tong and role is only in tong information, save the relationship 
    // into the map and useful for getting the TongID the role belongs to.
    for (int i = 0; i < nTotalMemberCount; i++)
    {
        nRetCode = TongInfo.GetMemberID(i, &nMemberID);
        KGLOG_PROCESS_ERROR(nRetCode && "TongInfo.GetMemberID()");

        InsertResult = m_RoleTongMap.insert(make_pair(nMemberID, pRetTongData));
        KGLOG_PROCESS_ERROR(InsertResult.second);
    }

	nResult = true;
Exit0:
    if (!nResult)
    {
        KGLogPrintf(KGLOG_DEBUG, "Failed to parse the information of tong: %d", nTongID);
    }
    if (nTongInfoInitFlag)
    {
        TongInfo.UnInit(NULL);
        nTongInfoInitFlag = false;
    }    
	return nResult;
}

int KG_GameDatabase::_ParseRoleData(
    int nRoleID, const char cszRoleName[],
    const unsigned char cbyBaseInfo[], unsigned uBaseInfoSize,
    const unsigned char cbyExtInfo[], unsigned uExtInfoSize,
    KG_ROLE_DATA *pRetRoleData
)
{
	int nResult  = false;
	int nRetCode = false;
    int nRoleInfoInitFlag = false;
    KG_RoleInfo RoleInfo;

    ASSERT(cszRoleName);
    ASSERT(cszRoleName[0]);
    ASSERT(cbyBaseInfo);
    ASSERT(uBaseInfoSize);
    // ASSERT(cbyExtInfo);   // ExtInfo could be NULL.
    // ASSERT(uExtInfoSize);
    ASSERT(pRetRoleData);

    pRetRoleData->nRoleID = nRoleID;

    strncpy(pRetRoleData->szRoleName, cszRoleName, sizeof(pRetRoleData->szRoleName));
    KGLOG_PROCESS_ERROR(pRetRoleData->szRoleName[sizeof(pRetRoleData->szRoleName) - 1] == '\0');

    // Set the initial value as -1, which indicate the role not belong to any tong, and find the tong
    // later.
    pRetRoleData->nTongID       = -1;
    pRetRoleData->nTongMasterID = -1;

    nRetCode = RoleInfo.Init(cbyBaseInfo, uBaseInfoSize, cbyExtInfo, uExtInfoSize, NULL);
    KGLOG_PROCESS_ERROR(nRetCode && "RoleInfo.Init()");
    nRoleInfoInitFlag = true;

    nRetCode = RoleInfo.GetLevel(&(pRetRoleData->nRoleLevel));
    KGLOG_PROCESS_ERROR(nRetCode && "RoleInfo.GetLevel()");

    nRetCode = RoleInfo.GetForceID(&(pRetRoleData->nRoleForceID));
    KGLOG_PROCESS_ERROR(nRetCode && "RoleInfo.GetForceID()");

    nRetCode = RoleInfo.GetCampID(&(pRetRoleData->nRoleCampID));
    KGLOG_PROCESS_ERROR(nRetCode && "RoleInfo.GetCampID()");

    nRetCode = RoleInfo.GetTotalMoney(&(pRetRoleData->nTotalMoney));
    KGLOG_PROCESS_ERROR(nRetCode && "RoleInfo.GetTotalMoney()");

    nRetCode = RoleInfo.GetAchievementRecord(&(pRetRoleData->nAchievementRecord));
    KGLOG_PROCESS_ERROR(nRetCode && "RoleInfo.GetAchievementRecord()");

    nRetCode = _GetRoleTotalTrain(RoleInfo, &(pRetRoleData->nTotalTrain));
    KGLOG_PROCESS_ERROR(nRetCode && "_GetRoleTotalTrain()");

    nRetCode = _GetRoleTotalRepute(RoleInfo, &(pRetRoleData->nTotalRepute));
    KGLOG_PROCESS_ERROR(nRetCode && "_GetRoleTotalRepute()");

    nRetCode = _GetRoleEquipmentPoint(RoleInfo, &(pRetRoleData->fEquipmentPoint));
    KGLOG_PROCESS_ERROR(nRetCode && "_GetRoleEquipmentPoint()");

	nResult = true;
Exit0:
    if (!nResult)
    {
        KGLogPrintf(KGLOG_DEBUG, "Failed to parse the data of role: %d", nRoleID);
    }
    if (nRoleInfoInitFlag)
    {
        RoleInfo.UnInit(NULL);
        nRoleInfoInitFlag = false;
    }
	return nResult;
}

int KG_GameDatabase::_GetRoleTotalTrain(const KG_RoleInfo &crRoleInfo, int *pnRetTrain)
{
	int nResult  = false;
	int nRetCode = false;
    int nUsedTrain = 0;
    int nSkillLevel = 0;
    const int SPECIAL_VENATION_ID_ARRAY[] = {9, 40, 41, 42, 43, 44, 45, 46, 170, 1206};
    const int SKILL_TRAIN_ARRAY[] = {0, 2222, 2353, 2500, 3333, 3636, 4000, 6667, 8000, 10000, 40000};
    
    // 玩家修为等于 丹田修为值加上所有奇穴对应的修为，丹田修为值即为玩家已用修为

    // 统计奇穴名称：奇穴·任脉·下极俞 奇穴·任脉·气端 奇穴·督脉·腰眼 奇穴·督脉·抬肩 奇穴·督脉·睛中 奇穴·带脉·曲泉 奇穴·带脉·龙玄 奇穴·冲脉·极泉 奇穴·冲脉·合阳 奇穴·任脉·崇骨
    // 对应技能ID：  9                40             41             42             43             44             45             46             170            1206

    // 奇穴技能等级转换为修为
    // 奇穴技能等级：1重  2重  3重  4重  5重  6重  7重  8重  9重   10重
    // 对应修为：    2222 2353 2500 3333 3636 4000 6667 8000 10000 40000

    ASSERT(pnRetTrain);
    *pnRetTrain = 0;

    nRetCode = crRoleInfo.GetUsedTrain(&nUsedTrain);
    KGLOG_PROCESS_ERROR(nRetCode && "crRoleInfo.GetUsedTrain()");
    *pnRetTrain += nUsedTrain;

    for (int i = 0; i < sizeof(SPECIAL_VENATION_ID_ARRAY) / sizeof(SPECIAL_VENATION_ID_ARRAY[0]); i++)
    {
        nRetCode = crRoleInfo.IsHaveSkill(SPECIAL_VENATION_ID_ARRAY[i]);
        if (nRetCode == false)
            continue;

        nRetCode = crRoleInfo.GetSkillLevel(SPECIAL_VENATION_ID_ARRAY[i], &nSkillLevel);
        KGLOG_PROCESS_ERROR(nRetCode && "crRoleInfo.GetSkillLevel()");

        KGLOG_PROCESS_ERROR(nSkillLevel >= 1);
        KGLOG_PROCESS_ERROR(nSkillLevel < sizeof(SKILL_TRAIN_ARRAY) / sizeof(SKILL_TRAIN_ARRAY[0]));
        *pnRetTrain += SKILL_TRAIN_ARRAY[nSkillLevel];
    }

	nResult = true;
Exit0:
	return nResult;
}

int KG_GameDatabase::_GetRoleTotalRepute(const KG_RoleInfo &crRoleInfo, int *pnRetRepute)
{
	int nResult  = false;
	int nRetCode = false;
    int nReputeLevel = 0;
    int nExtRepute   = 0;
    const int STAT_FORCE_ID_ARRAY[] = {
        11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 34, 35,36, 42, 43, 44, 45, 46, 47, 48, 49, 50, 54, 55, 56
    };
    const int REPUTE_VALUE_ARRAY[] = {
        0, 12000, 20400, 24000, 27600, 36000, 48000, 66000, 86000, 108000, 132000, 158000, 186000, 216000
    };

    // 统计势力名称：少林 万花 天策 纯阳 七秀 五毒 唐门 藏剑 丐帮 明教 城市·扬州 城市·洛阳 城市·长安 镖局联盟 南洋商会 东漓寨 长歌门 昆仑 刀宗 隐元会 恶人谷 浩气盟 关中商会 西域商会 播州商会
    // 对应ID：      11   12   13   14   15   16   17   18   19   20   34        35        36        42       43       44     45     46   47   48     49     50     54       55       56

    // 势力声望等级：1     2    3    4    5    6     7     8     9     10    11    12    13
    // 对应声望值：  12000 8400 3600 3600 8400 12000 18000 20000 22000 24000 26000 28000 30000
    // 比如读取到玩家35号势力的当前声望为6级300，则总值为 12000+8400+3600+3600+8400+300

    ASSERT(pnRetRepute);
    *pnRetRepute = 0;

    for (int i = 0; i < sizeof(STAT_FORCE_ID_ARRAY) / sizeof(STAT_FORCE_ID_ARRAY[0]); i++)
    {
        nRetCode = crRoleInfo.GetForceReputeLevel(STAT_FORCE_ID_ARRAY[i], &nReputeLevel);
        KGLOG_PROCESS_ERROR(nRetCode && "crRoleInfo.GetForceReputeLevel()");

        KGLOG_PROCESS_ERROR(nReputeLevel >= 0);
        KGLOG_PROCESS_ERROR(nReputeLevel < sizeof(REPUTE_VALUE_ARRAY) / sizeof(REPUTE_VALUE_ARRAY[0]));
        
        *pnRetRepute += REPUTE_VALUE_ARRAY[nReputeLevel];

        nRetCode = crRoleInfo.GetForceExtRepute(STAT_FORCE_ID_ARRAY[i], &nExtRepute);
        KGLOG_PROCESS_ERROR(nRetCode && "crRoleInfo.GetForceExtRepute()");

        *pnRetRepute += nExtRepute;
    }

	nResult = true;
Exit0:
	return nResult;
}

int KG_GameDatabase::_GetRoleEquipmentPoint(const KG_RoleInfo &crRoleInfo, float *pfRetLevel)
{
	int nResult  = false;
	int nRetCode = false;
    int nEmptyFlag = false;
    int nTypeID = 0;
    int nItemID = 0;
    KG_EQUIPMENT_TYPE_MAP::iterator itType;
    KG_EQUIPMENT_POINT_MAP *pEquipmentPointMap = NULL;
    KG_EQUIPMENT_POINT_MAP::iterator itPoint;
    const int ARMED_BOX_ID = 0;
    const int EQUIPMENT_ID_ARRAY[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 22};

    // 装备身上所对应的BoxID为0
    // 统计身上的装备：   近战武器 远程武器 上装 帽子 项链 左手戒指 右手戒指 腰带 腰坠 下装 鞋子 护手 暗器
    // 对应的Position ID：0        1        2    3    4    5        6        7    8     9   10   11   22

    ASSERT(pfRetLevel);
    *pfRetLevel = 0;

    for (int i = 0; i < sizeof(EQUIPMENT_ID_ARRAY) / sizeof(EQUIPMENT_ID_ARRAY[0]); i++)
    {
        nRetCode = crRoleInfo.GetItemInfo(
            ARMED_BOX_ID, EQUIPMENT_ID_ARRAY[i], &nEmptyFlag, &nTypeID, &nItemID
        );
        KGLOG_PROCESS_ERROR(nRetCode && "crRoleInfo.GetItemInfo()");

        if (nEmptyFlag)
            continue;

        itType = m_EquipmentTypeMap.find(nTypeID);
        if (itType == m_EquipmentTypeMap.end())
        {
            printf("Failed to find the item, Type ID: %d, Item ID: %d\n", nTypeID, nItemID);
            KGLogPrintf(
                KGLOG_DEBUG, "Failed to find the item, Type ID: %d, Item ID: %d", nTypeID, nItemID
            );
            continue;
        }

        pEquipmentPointMap = &(itType->second);

        itPoint = pEquipmentPointMap->find(nItemID);
        if (itPoint == pEquipmentPointMap->end())
        {
            printf("Failed to find the item, Type ID: %d, Item ID: %d\n", nTypeID, nItemID);
            KGLogPrintf(
                KGLOG_DEBUG, "Failed to find the item, Type ID: %d, Item ID: %d", nTypeID, nItemID
            );
            continue;
        }

        *pfRetLevel += itPoint->second;        
    }   

	nResult = true;
Exit0:
	return nResult;
}

int KG_GameDatabase::_InsertRankResult(
    const char cszName[], const unsigned char cbyValue[], unsigned uValueSize
)
{
    int nResult  = false;
    int nRetCode = false;
    unsigned uBufferSize = 0;
    IKG_Buffer *piBuffer = NULL;
    char *pchBuffer   = NULL;
    char *pchWritePos = NULL;
    unsigned uLeftSize   = 0;
    unsigned uDataLength = 0;

    ASSERT(cszName);
    ASSERT(cszName[0]);
    ASSERT(cbyValue);
    ASSERT(uValueSize);

    // replace `GlobalCustomData` set `Name` = '%s', `Value` = '%s'

    uBufferSize = (unsigned)strlen(cszName) * 2 + uValueSize * 2 + 128;
    piBuffer = KG_MemoryCreateBuffer(uBufferSize);
    KGLOG_PROCESS_ERROR(piBuffer);

    pchBuffer = (char *)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pchBuffer);

    pchWritePos = pchBuffer;
    uLeftSize = uBufferSize;

    uDataLength = sizeof("replace `GlobalCustomData` set `Name` = '") - 1;
    KGLOG_PROCESS_ERROR(uDataLength <= uLeftSize);
    memcpy(pchWritePos, "replace `GlobalCustomData` set `Name` = '", uDataLength);
    pchWritePos += uDataLength;
    uLeftSize   -= uDataLength;

    uDataLength = (unsigned)strlen(cszName);
    KGLOG_PROCESS_ERROR(uDataLength * 2 + 1 <= uLeftSize);
    nRetCode = (int)mysql_real_escape_string(m_pMySQL, pchWritePos, cszName, uDataLength);
    pchWritePos += nRetCode;
    uLeftSize   -= nRetCode;

    uDataLength = sizeof("', `Value` = '") - 1;
    KGLOG_PROCESS_ERROR(uDataLength <= uLeftSize);
    memcpy(pchWritePos, "', `Value` = '", uDataLength);
    pchWritePos += uDataLength;
    uLeftSize   -= uDataLength;

    KGLOG_PROCESS_ERROR(uValueSize * 2 + 1 <= uLeftSize);
    nRetCode = (int)mysql_real_escape_string(m_pMySQL, pchWritePos, (char *)cbyValue, uValueSize);
    pchWritePos += nRetCode;
    uLeftSize   -= nRetCode;

    KGLOG_PROCESS_ERROR(uLeftSize >= 1);
    *pchWritePos = '\'';
    pchWritePos++;

    nRetCode = mysql_real_query(m_pMySQL, pchBuffer, (unsigned)(pchWritePos - pchBuffer));
    KGD_MYSQL_PROCESS_ERROR(nRetCode == 0, m_pMySQL, pchBuffer);

    nResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return nResult;
}
