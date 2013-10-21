////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : main.cpp
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-10-12  16:21:32
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "KG_Public.h"
#include "Engine/FileType.h"
#include <string>
#include <list>

#ifdef WIN32
#include <string.h>
#define strcasecmp  _stricmp
#define strncasecmp _strnicmp
#else
#include <strings.h>
#endif

////////////////////////////////////////////////////////////////////////////////
const unsigned _KG_REWARD_VALUE_COUNT = 6;

struct _KG_ROLE_INFO
{
    int     nRoleID;
    int     nValueArray[_KG_REWARD_VALUE_COUNT];
};

typedef std::list<_KG_ROLE_INFO> _KG_ROLE_INFO_LIST;

struct _KG_REWARD_INFO
{
    int                 nActivityID;
    string              strDeadline;
    _KG_ROLE_INFO_LIST  RoleInfoList;
};

////////////////////////////////////////////////////////////////////////////////
int _GetRewardInfo(MYSQL *pMySQL, const char cszRewardFileName[], _KG_REWARD_INFO *pRetRewardInfo);

int _GetCurrentLogServerName(string *pstrRetServerName);

int _GetRoleIDByName(MYSQL *pMySQL, const char cszRoleName[], int *pnRetRoleID);

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
	int nResult  = false;
	int nRetCode = false;
    int nKGLogInitFlag = false;
    KGLOG_PARAM LogParam  = {"Logs", "OfflineReward", KGLOG_OPTION_FILE, 100000};
    KG_MYSQL_PARAM MySQLParam;
    string strRewardFileName;
    MYSQL *pMySQL = NULL;
    _KG_REWARD_INFO RewardInfo;
    _KG_ROLE_INFO_LIST::iterator it;

    puts("Initializing runtime environment ...");

    nRetCode = KG_SetRuntimeEnvironment(argv[0]);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = KGLogInit(LogParam, NULL);
    KG_PROCESS_ERROR(nRetCode);
    nKGLogInitFlag = true;

    nRetCode = KG_LoadMySQLDatabaseParameter("OfflineReward.ini", &MySQLParam);
    KGLOG_PROCESS_ERROR(nRetCode && "KG_LoadMySQLDatabaseParameter()");

    pMySQL = KG_ConnectMySQLDatabase(MySQLParam);
    KGLOG_PROCESS_ERROR(pMySQL && "KG_ConnectMySQLDatabase()");

    if (argc != 2)
    {
        puts("Please input the reward file name!");
        KGLOG_PROCESS_ERROR(false && "Please input the reward file name!");
    }
    strRewardFileName = argv[1];

    puts("Start Get the reward information ...");

    nRetCode = _GetRewardInfo(pMySQL, strRewardFileName.c_str(), &RewardInfo);
    KGLOG_PROCESS_ERROR(nRetCode && "_GetRewardInfo()");

    nRetCode = KG_RemoveOldRewardData(pMySQL, RewardInfo.nActivityID);
    KGLOG_PROCESS_ERROR(nRetCode && "KG_RemoveOldRewardData()");
    
    puts("Start Insert the reward record ...");

    for (it = RewardInfo.RoleInfoList.begin(); it != RewardInfo.RoleInfoList.end(); ++it)
    {
        nRetCode = KG_RewardRole(
            pMySQL, it->nRoleID, RewardInfo.nActivityID, RewardInfo.strDeadline.c_str(),
            it->nValueArray[0], it->nValueArray[1], it->nValueArray[2],
            it->nValueArray[3], it->nValueArray[4], it->nValueArray[5]
        );
        KGLOG_PROCESS_ERROR(nRetCode && "KG_RewardRole()");
    }

	nResult = true;
Exit0:
    if (nResult)
    {
        puts("Operation had been done successfully.");
        KGLogPrintf(KGLOG_INFO, "Operation had been done successfully.");
    }
    else
    {
        puts("Operation failed!!");
        KGLogPrintf(KGLOG_INFO, "Operation failed!!");
    }
    if (nKGLogInitFlag)
    {
        KGLogUnInit(NULL);
        nKGLogInitFlag = false;
    }
	return nResult;
}

int _GetRewardInfo(MYSQL *pMySQL, const char cszRewardFileName[], _KG_REWARD_INFO *pRetRewardInfo)
{
    int nResult  = false;
    int nRetCode = false;
    string strLogServerName;
    FILE *fpFile = NULL;
    int nActivityID = 0;
    char szDate[64];
    char szTime[64];
    char szLogServerName[64];
    char szServerName[64];
    char szRoleName[64];
    _KG_ROLE_INFO RoleData;

    ASSERT(pMySQL);
    ASSERT(cszRewardFileName);
    ASSERT(cszRewardFileName[0]);
    ASSERT(pRetRewardInfo);

    nRetCode = _GetCurrentLogServerName(&strLogServerName);
    KGLOG_PROCESS_ERROR(nRetCode && "_GetCurrentLogServerName()");

    fpFile = fopen(cszRewardFileName, "r");
    if (fpFile == NULL)
    {
        printf("! Can not open file : %s\n", cszRewardFileName);
        KGLogPrintf(KGLOG_DEBUG, "! Can not open file : %s\n", cszRewardFileName);
        KGLOG_PROCESS_ERROR(false);
    }

    // The first line is activity ID and deadline.
    nRetCode = fscanf(fpFile, "%d %s %s", &nActivityID, szDate, szTime);
    KGLOG_PROCESS_ERROR(nRetCode > 0);

    pRetRewardInfo->nActivityID = nActivityID;
    pRetRewardInfo->strDeadline = string(szDate) + " " + szTime;

    // From the second line, there are the role reward information.
    while (true)
    {
        nRetCode = fscanf(
            fpFile, "%s %s %s %d %d %d %d %d %d",
            szLogServerName, szServerName, szRoleName,
            &RoleData.nValueArray[0], &RoleData.nValueArray[1], &RoleData.nValueArray[2],
            &RoleData.nValueArray[3], &RoleData.nValueArray[4], &RoleData.nValueArray[5]
        );
        if (nRetCode <= 0)
        {
            nRetCode = feof(fpFile);
            if (nRetCode)
                break;

            KGLOG_PROCESS_ERROR(nRetCode && "fscanf()");
        }

        nRetCode = strcasecmp(strLogServerName.c_str(), szLogServerName);
        if (nRetCode)   // Not belong to this server.
            continue;

        nRetCode = _GetRoleIDByName(pMySQL, szRoleName, &(RoleData.nRoleID));
        if (!nRetCode)
        {
            printf("! Can not find the role `%s` in server `%s`\n", szRoleName, szServerName);
            KGLogPrintf(KGLOG_DEBUG, "Can not find the role `%s` in server `%s`", szRoleName, szServerName);
            continue;
        }

        pRetRewardInfo->RoleInfoList.push_back(RoleData);
    }

    nResult = true;
Exit0:
    if (fpFile)
    {
        fclose(fpFile);
        fpFile = NULL;
    }
    return nResult;
}

int _GetCurrentLogServerName(string *pstrRetLogServerName)
{
	int nResult  = false;
	int nRetCode = false;
    IIniFile *piIniFile = NULL;
    char szBuffer[128];

    ASSERT(pstrRetLogServerName);

    piIniFile = g_OpenIniFile("../gameserver01/gs_settings.ini");
    if (piIniFile == NULL)
    {
        KGLogPrintf(KGLOG_DEBUG, "Can not find ../gameserver01/gs_settings.ini.");
        KGLOG_PROCESS_ERROR(false);
    }

    nRetCode = piIniFile->GetString("LogServer", "Identity", NULL, szBuffer, sizeof(szBuffer));
    KGLOG_PROCESS_ERROR(nRetCode && "Failed to get [LogServer]Identity");

    *pstrRetLogServerName = szBuffer;

	nResult = true;
Exit0:
    if (!nResult)
    {
        KGLogPrintf(KGLOG_DEBUG, "Failed to get current LogServer name.");
    }
    KG_COM_RELEASE(piIniFile);
	return nResult;
}

int _GetRoleIDByName(MYSQL *pMySQL, const char cszRoleName[], int *pnRetRoleID)
{
	int nResult  = false;
	int nRetCode = false;
    char szQuery[128];
    MYSQL_RES *pMySQLResult = NULL;
    MYSQL_ROW Row = NULL;

    ASSERT(pMySQL);
    ASSERT(cszRoleName);
    ASSERT(cszRoleName[0]);
    ASSERT(pnRetRoleID);

    nRetCode = snprintf(
        szQuery, sizeof(szQuery), "select `ID` from `role` where `RoleName`='%s'", cszRoleName
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    KGLOG_PROCESS_ERROR(nRetCode < sizeof(szQuery));

    nRetCode = mysql_query(pMySQL, szQuery);
    KGD_MYSQL_PROCESS_ERROR(nRetCode == 0, pMySQL, szQuery);

    pMySQLResult = mysql_store_result(pMySQL);
    KGD_MYSQL_PROCESS_ERROR(pMySQLResult, pMySQL, "mysql_store_result()");

    nRetCode = (int)mysql_num_rows(pMySQLResult);
    KGLOG_PROCESS_ERROR(nRetCode == 1 && "the role not found.");

    Row = mysql_fetch_row(pMySQLResult);
    KGD_MYSQL_PROCESS_ERROR(Row, pMySQL, "mysql_fetch_row()");

    *pnRetRoleID = atoi(Row[0]);

	nResult = true;
Exit0:
    if (pMySQLResult)
    {
        mysql_free_result(pMySQLResult);
        pMySQLResult = NULL;
    }
	return nResult;
}
