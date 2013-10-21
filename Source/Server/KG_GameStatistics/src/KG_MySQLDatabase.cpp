////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_MySQLDatabase.cpp
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-12-11  14:07:38
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "KG_MySQLDatabase.h"

MYSQL *KG_ConnectMySQLDatabase(const KG_MYSQL_PARAM &crParam)
{
    int nResult  = false;
    int nRetCode = false;
    MYSQL *pResultConnection = NULL;
    MYSQL *pRetConnect       = NULL;
    unsigned long ulVersion = 0;
    my_bool bReconnectFlag = true;

    KGLOG_PROCESS_ERROR(crParam.szServer[0]);
    KGLOG_PROCESS_ERROR(crParam.szCharacterSet[0]);
    KGLOG_PROCESS_ERROR(crParam.szDatabase[0]);
    KGLOG_PROCESS_ERROR(crParam.szUserName[0]);
    KGLOG_PROCESS_ERROR(crParam.szPassword[0]);

    ulVersion = mysql_get_client_version();
    KGLOG_PROCESS_ERROR(ulVersion == MYSQL_VERSION_ID &&"MySQL library is not the appropriate version.");

    pResultConnection = mysql_init(NULL);
    KGLOG_PROCESS_ERROR(pResultConnection);

    nRetCode = mysql_options(pResultConnection, MYSQL_OPT_RECONNECT, &bReconnectFlag);
    KGLOG_PROCESS_ERROR(nRetCode == 0);

    pRetConnect = mysql_real_connect(
        pResultConnection, crParam.szServer, crParam.szUserName, crParam.szPassword,
        NULL, crParam.nPort, NULL, CLIENT_FOUND_ROWS // Return the number of found (matched) rows, not affected.
    );
    KGD_MYSQL_PROCESS_ERROR(pRetConnect, pResultConnection, "mysql_real_connect()");
    KGLOG_PROCESS_ERROR(pRetConnect == pResultConnection);
    pRetConnect = NULL; // Used for judge success or failed, now is useless.

    // Set current database.
    nRetCode = mysql_select_db(pResultConnection, crParam.szDatabase);
    KGD_MYSQL_PROCESS_ERROR(nRetCode == 0, pResultConnection, "mysql_select_db()");

    // Set connection character.
    nRetCode = mysql_set_character_set(pResultConnection, crParam.szCharacterSet);
    KGD_MYSQL_PROCESS_ERROR(nRetCode == 0, pResultConnection, "mysql_set_character_set()");

    nResult = true;
Exit0:
    if (!nResult) 
    {
        if (pResultConnection)
        {
            mysql_close(pResultConnection);
            pResultConnection = NULL;    
        }
    }
    return pResultConnection;
}

