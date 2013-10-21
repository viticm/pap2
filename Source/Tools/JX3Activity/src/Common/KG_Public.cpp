////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_Public.cpp
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-9-9  17:57:31
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "KG_Public.h"
#include <locale.h>
#include "PassGen.h"
#include "Engine/File.h"
#include "Engine/FileType.h"

#ifndef WIN32
#include <unistd.h>
#endif

int KG_SetRuntimeEnvironment(const char cszAppFullPathName[MAX_PATH])
{
    int nResult  = false;
    int nRetCode = false;
    char szProgramPath[MAX_PATH];

    KGLOG_PROCESS_ERROR(cszAppFullPathName);

    // Set the locale to the default, which is the user-default ANSI code page obtained from 
    // the operating system.
    setlocale(LC_ALL, "");

    // Set working path to application path.
    g_ExtractFilePath(szProgramPath, const_cast<char *>(cszAppFullPathName));
    szProgramPath[sizeof(szProgramPath) - 1] = '\0';

    if (szProgramPath[0])
    {
        nRetCode = chdir(szProgramPath);
        KG_PROCESS_ERROR(nRetCode == 0);

        // Set root path for config file.
        g_SetRootPath(szProgramPath); 
    }
    else
    {
        g_SetRootPath(NULL);
    }

    nResult = true;
Exit0:
    return nResult;
}

int KG_LoadMySQLDatabaseParameter(const char cszConfigFileName[], KG_MYSQL_PARAM *pRetParam)
{
	int nResult  = false;
	int nRetCode = false;
    IIniFile *piIniFile = NULL;
    char szPassword[64];

    KGLOG_PROCESS_ERROR(cszConfigFileName);
    KGLOG_PROCESS_ERROR(cszConfigFileName[0]);
    KGLOG_PROCESS_ERROR(pRetParam);

    piIniFile = g_OpenIniFile(cszConfigFileName);
    KGLOG_PROCESS_ERROR(piIniFile && "g_OpenIniFile()");

    nRetCode = piIniFile->GetString(
        "DatabaseServer", "Server", NULL, pRetParam->szServer, sizeof(pRetParam->szServer)
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Unable load \"[DatabaseServer]:Server\"");

    nRetCode = piIniFile->GetString(
        "DatabaseServer", "Database", NULL, pRetParam->szDatabase, sizeof(pRetParam->szDatabase)
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Unable load \"[DatabaseServer]:Database\"");

    nRetCode = piIniFile->GetString(
        "DatabaseServer", "UserName", NULL, pRetParam->szUserName, sizeof(pRetParam->szUserName)
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Unable load \"[DatabaseServer]:UserName\"");

    nRetCode = piIniFile->GetString("DatabaseServer", "Password", NULL, szPassword, sizeof(szPassword));
    KGLOG_PROCESS_ERROR(nRetCode && "Unable load \"[DatabaseServer]:Password\"");

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
    KG_COM_RELEASE(piIniFile);
    if (!nResult && cszConfigFileName)
        KGLogPrintf(KGLOG_DEBUG, "Failed to load parameter from the file : %s", cszConfigFileName);
	return nResult;
}

MYSQL *KG_ConnectMySQLDatabase(const KG_MYSQL_PARAM &crParam)
{
    int nResult  = false;
    int nRetCode = false;
    MYSQL *pMySQL   = NULL;
    MYSQL *pRetConnect = NULL;
    unsigned long ulVersion = 0;
    MYSQL_RES* pMySQLResult = NULL;
    MYSQL_ROW Row = NULL;
    const char *pcszHost = "localhost";
    const char SQL_QUERY_CHARACTER[] = "show variables like 'character_set_database'";

    ASSERT(crParam.szServer[0]);
    ASSERT(crParam.szDatabase[0]);
    ASSERT(crParam.szUserName[0]);
    ASSERT(crParam.szPassword[0]);

    ulVersion = mysql_get_client_version();
    KGLOG_PROCESS_ERROR(ulVersion == MYSQL_VERSION_ID &&"MySQL library is not the appropriate version.");

    pMySQL = mysql_init(NULL);
    KGLOG_PROCESS_ERROR(pMySQL);

    if (strcmp(crParam.szServer, "127.0.0.1") != 0)
        pcszHost = crParam.szServer;

    pRetConnect = mysql_real_connect(
        pMySQL, pcszHost, crParam.szUserName, crParam.szPassword, NULL, 3306, NULL, 
        CLIENT_FOUND_ROWS // Return the number of found (matched) rows, not affected.
    );
    KGD_MYSQL_PROCESS_ERROR(pRetConnect, pMySQL, "mysql_real_connect()");
    KGLOG_PROCESS_ERROR(pRetConnect == pMySQL);
    pRetConnect = NULL; // Used for judge success or failed, now is useless.

    // Set current database.
    nRetCode = mysql_select_db(pMySQL, crParam.szDatabase);
    KGD_MYSQL_PROCESS_ERROR(nRetCode == 0, pMySQL, "mysql_select_db()");

    // Set connection character.
    nRetCode = mysql_real_query(pMySQL, SQL_QUERY_CHARACTER, (unsigned long)strlen(SQL_QUERY_CHARACTER));
    KGD_MYSQL_PROCESS_ERROR(nRetCode == 0, pMySQL, SQL_QUERY_CHARACTER);

    pMySQLResult = mysql_store_result(pMySQL);
    KGD_MYSQL_PROCESS_ERROR(pMySQLResult, pMySQL, "mysql_store_result()");

    Row = mysql_fetch_row(pMySQLResult);
    KGD_MYSQL_PROCESS_ERROR(Row != NULL, pMySQL, "mysql_fetch_row()");

    nRetCode = mysql_set_character_set(pMySQL, Row[1]);
    KGD_MYSQL_PROCESS_ERROR(nRetCode == 0, pMySQL, "mysql_set_character_set()");

    nResult = true;
Exit0:
    if (!nResult) 
    {
        if (pMySQL)
        {
            mysql_close(pMySQL);
            pMySQL = NULL;    
        }
    }
    return pMySQL;
}

int KG_CreateRewardTable(MYSQL *pMySQL)
{
	int nResult  = false;
	int nRetCode = false;
    const char SQL_CREATE_ACTIVITY_TABLE [] = 
        "create table if not exists Activity( "
        "   ID      integer     not null, "
        "   Type    integer     not null, "
        "   Time    datetime    not null, "
        "   Value0  integer     not null, "
        "   Value1  integer     not null, "
        "   Value2  integer     not null, "
        "   Value3  integer     not null, "
        "   Value4  integer     not null, "
        "   Value5  integer     not null, "
        "   primary key (ID, Type) "
        ")";

    KGLOG_PROCESS_ERROR(pMySQL);

    nRetCode = mysql_real_query(
        pMySQL, SQL_CREATE_ACTIVITY_TABLE, (unsigned long)strlen(SQL_CREATE_ACTIVITY_TABLE)
    );
    KGD_MYSQL_PROCESS_ERROR(nRetCode == 0, pMySQL, SQL_CREATE_ACTIVITY_TABLE);

	nResult = true;
Exit0:
	return nResult;
}

int KG_RemoveOldRewardData(MYSQL *pMySQL, int nActivityID)
{
	int nResult  = false;
	int nRetCode = false;
    char szQuery[256];

    KGLOG_PROCESS_ERROR(pMySQL);

    nRetCode = snprintf(szQuery, sizeof(szQuery), "delete from `Activity` where Type = %d", nActivityID);
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    KGLOG_PROCESS_ERROR(nRetCode < sizeof(szQuery));

    nRetCode = mysql_real_query(pMySQL, szQuery, (unsigned long)strlen(szQuery));
    KGD_MYSQL_PROCESS_ERROR(nRetCode == 0, pMySQL, szQuery);

    nRetCode = (int)mysql_affected_rows(pMySQL);
    if (nRetCode > 0)
    {
        printf("! There are %d rows of activity %d been removed\n", nRetCode, nActivityID);
        KGLogPrintf(KGLOG_INFO, "There are %d rows of activity %d been removed\n", nRetCode, nActivityID);
    }

	nResult = true;
Exit0:
	return nResult;
}

int KG_RewardRole(
    MYSQL *pMySQL, int nRoleID, int nActivityID, const char szDeadline[],
    int nValue0, int nValue1, int nValue2, int nValue3, int nValue4, int nValue5
)
{
	int nResult  = false;
	int nRetCode = false;
    char szQuery[256];

    KGLOG_PROCESS_ERROR(pMySQL);
    KGLOG_PROCESS_ERROR(szDeadline);
    KGLOG_PROCESS_ERROR(szDeadline[0]);

    nRetCode = snprintf(
        szQuery, sizeof(szQuery),
        "insert into `Activity` (`ID`, `Type`, `Time`, `Value0`, `Value1`, `Value2`, `Value3`, `Value4`, `Value5`)"
        " values (%d, %d, '%s', %d, %d, %d, %d, %d, %d)",
        nRoleID, nActivityID, szDeadline, nValue0, nValue1, nValue2, nValue3, nValue4, nValue5
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    KGLOG_PROCESS_ERROR(nRetCode < sizeof(szQuery));

    nRetCode = mysql_real_query(pMySQL, szQuery, nRetCode);
    KGD_MYSQL_PROCESS_ERROR(nRetCode == 0, pMySQL, szQuery);

	nResult = true;
Exit0:
	return nResult;
}

int KG_GetTongHandleList(MYSQL *pMySQL, KG_TONG_HANDLE_LIST *pRetTongHandleList)
{
	int nResult  = false;
	int nRetCode = false;
    MYSQL_RES *pTongSet = NULL;
    MYSQL_ROW Row = NULL;
    unsigned long *pulLengths = NULL;
    PARSEHANDLE hTongHandle = NULL;
    const char SQL_QUERY_TONG[] = "select `Data` from `Tong`";

    KGLOG_PROCESS_ERROR(pMySQL);
    KGLOG_PROCESS_ERROR(pRetTongHandleList);

    pRetTongHandleList->clear();

    nRetCode = mysql_real_query(pMySQL, SQL_QUERY_TONG, (unsigned long)strlen(SQL_QUERY_TONG));
    KGD_MYSQL_PROCESS_ERROR(nRetCode == 0, pMySQL, SQL_QUERY_TONG);

    pTongSet = mysql_use_result(pMySQL);
    KGD_MYSQL_PROCESS_ERROR(pTongSet, pMySQL, "mysql_use_result()");

    while (true)
    {
        Row = mysql_fetch_row(pTongSet);
        if (Row == NULL)
        {
            nRetCode = (int)mysql_errno(pMySQL);
            KGD_MYSQL_PROCESS_ERROR(nRetCode == 0, pMySQL, "mysql_fetch_row()");

            break;  // No more row
        }

        pulLengths = mysql_fetch_lengths(pTongSet);
        KGD_MYSQL_PROCESS_ERROR(pulLengths, pMySQL, "mysql_fetch_lengths()");

        hTongHandle = PaserTongData((unsigned char *)Row[0], pulLengths[0]);
        KGLOG_PROCESS_ERROR(hTongHandle);

        pRetTongHandleList->push_back(hTongHandle);
    }

	nResult = true;
Exit0:
    if (!nResult && pRetTongHandleList)
    {
        KG_TONG_HANDLE_LIST::iterator it;
        for (it = pRetTongHandleList->begin(); it != pRetTongHandleList->end(); ++it)
        {
            PARSEHANDLE hTongHandle = *it;
            if (hTongHandle)
            {
                EndTongParse(hTongHandle);
                hTongHandle = NULL;
            }
        }
        pRetTongHandleList->clear();
    }

    if (pTongSet)
    {
        mysql_free_result(pTongSet);
        pTongSet = NULL;
    }
	return nResult;
}
