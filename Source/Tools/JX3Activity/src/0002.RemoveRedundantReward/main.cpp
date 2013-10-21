////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : main.cpp
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-9-29  14:52:21
//  Comment     : 删除中秋活动中玩家刷的月饼、烟花等奖励
//
////////////////////////////////////////////////////////////////////////////////

#include "KG_Public.h"
#include "Engine/FileType.h"
#include <list>
#include <string>

#ifdef WIN32
#include <string.h>
#define strcasecmp  _stricmp
#define strncasecmp _strnicmp
#else
#include <strings.h>
#endif

////////////////////////////////////////////////////////////////////////////////
const unsigned KG_ACTIVITY_ID               = 2;
const char     KG_ACTIVITY_DEAD_LINE[]      = "2009-10-30 07:00:00";

struct _KG_ROLE_DATA
{
    int     nRoleID;
    int     nYellowCakeCount;   // 此玩家应该删除的双黄月饼个数
    int     nRedCakeCount;      // 此玩家应该删除的红豆月饼个数
    int     nStoneCount;        // 此玩家应该删除的净琉璃个数
};

typedef std::list<_KG_ROLE_DATA> _KG_ROLE_DATA_LIST;

int _GetRoleList(MYSQL *pMySQL, _KG_ROLE_DATA_LIST *pRetRoleDataList);

int _GetCurrentLogServerName(string *pstrRetServerName);

int _GetRoleIDByName(MYSQL *pMySQL, const char cszRoleName[], int *pnRetRoleID);

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
	int nResult  = false;
	int nRetCode = false;
    int nKGLogInitFlag = false;
    KGLOG_PARAM LogParam  = {"Logs", "0002.RemoveRedundantReward", KGLOG_OPTION_FILE, 100000};
    KG_MYSQL_PARAM MySQLParam;
    MYSQL *pMySQL = NULL;
    _KG_ROLE_DATA_LIST RoleDataList;
    _KG_ROLE_DATA_LIST::iterator it;

    puts("Initializing runtime environment ...");

    nRetCode = KG_SetRuntimeEnvironment(argv[0]);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = KGLogInit(LogParam, NULL);
    KG_PROCESS_ERROR(nRetCode);
    nKGLogInitFlag = true;

    nRetCode = KG_LoadMySQLDatabaseParameter("0002.RemoveRedundantReward.ini", &MySQLParam);
    KGLOG_PROCESS_ERROR(nRetCode && "KG_LoadMySQLDatabaseParameter()");

    pMySQL = KG_ConnectMySQLDatabase(MySQLParam);
    KGLOG_PROCESS_ERROR(pMySQL);

    nRetCode = KG_RemoveOldRewardData(pMySQL, KG_ACTIVITY_ID);
    KGLOG_PROCESS_ERROR(nRetCode && "KG_RemoveOldRewardData()");

    puts("Start Get the roles data that should be remove reward...");

    nRetCode = _GetRoleList(pMySQL, &RoleDataList);
    KGLOG_PROCESS_ERROR(nRetCode);

    puts("Start Insert the role record.");

    for (it = RoleDataList.begin(); it != RoleDataList.end(); ++it)
    {
        nRetCode = KG_RewardRole(
            pMySQL, it->nRoleID, KG_ACTIVITY_ID, KG_ACTIVITY_DEAD_LINE,
            it->nYellowCakeCount, it->nRedCakeCount, it->nStoneCount, 0, 0, 0
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

int _GetRoleList(MYSQL *pMySQL, _KG_ROLE_DATA_LIST *pRetRoleDataList)
{
	int nResult  = false;
	int nRetCode = false;
    string strLogServerName;
    char szLogServerName[64];
    char szServerName[64];
    char szAccountName[64];
    char szRoleName[64];
    _KG_ROLE_DATA RoleData;
    FILE *fpFile = NULL;

    ASSERT(pMySQL);
    ASSERT(pRetRoleDataList);
    pRetRoleDataList->clear();

    nRetCode = _GetCurrentLogServerName(&strLogServerName);
    KGLOG_PROCESS_ERROR(nRetCode && "_GetCurrentLogServerName()");

    fpFile = fopen("刷月饼名单.txt", "r");
    KGLOG_PROCESS_ERROR(fpFile);

    while (true)
    {
        nRetCode = fscanf(
            fpFile, "%s %s %s %s %d %d %d", szLogServerName, szServerName, szAccountName, szRoleName,
            &(RoleData.nYellowCakeCount), &(RoleData.nRedCakeCount), &(RoleData.nStoneCount)
        );
        if (nRetCode <= 0)
        {
            nRetCode = feof(fpFile);
            if (nRetCode)
                break;

            KGLOG_PROCESS_ERROR(nRetCode && "fscanf()");
        }

        KGLOG_PROCESS_ERROR(szLogServerName[0]);
        nRetCode = strcasecmp(strLogServerName.c_str(), szLogServerName);
        if (nRetCode)
            continue;

        KGLOG_PROCESS_ERROR(szRoleName[0]);
        nRetCode = _GetRoleIDByName(pMySQL, szRoleName, &(RoleData.nRoleID));
        if (!nRetCode)
        {
            printf("!! Can not find the role `%s` in server `%s`\n", szRoleName, szServerName);
            KGLogPrintf(KGLOG_DEBUG, "Can not find the role `%s` in server `%s`", szRoleName, szServerName);
            continue;
        }

        pRetRoleDataList->push_back(RoleData);
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
    KGLOG_PROCESS_ERROR(nRetCode);

    *pstrRetLogServerName = szBuffer;

	nResult = true;
Exit0:
    if (!nResult)
    {
        KGLogPrintf(KGLOG_DEBUG, "Failed to get current log server name.");
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
