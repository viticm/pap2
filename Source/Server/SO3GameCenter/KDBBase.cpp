//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KDBBase.cpp
//  Version     :   1.0
//  Creater     :   Xia Yong
//  Date        :   2008-01-11 09:34:39
//  Comment     :   ·ÃÎÊÊý¾Ý¿â
//////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KDBBase.h"
#include "errmsg.h"

static const unsigned long MYSQL5_VERSION_ID = 50000; // MySQL 5 Version ID

BOOL MySQL_CheckCaseInsensitive(MYSQL* pHandle)
{
    BOOL        bResult         = false;
    int         nRetCode        = 0;
    char        szSQL[]         = "show variables like \'lower_case_table_names\'";
    MYSQL_RES*  pResultSet      = NULL;
    MYSQL_ROW   Row             = NULL;
    unsigned    uFieldCount     = 0;
    
    assert(pHandle);

    nRetCode = MySQL_Query(pHandle, szSQL);
    KGLOG_PROCESS_ERROR(nRetCode);

    pResultSet = mysql_store_result(pHandle);
    KGLOG_PROCESS_ERROR(pResultSet);

    uFieldCount = mysql_num_fields(pResultSet);    
    KGLOG_PROCESS_ERROR(uFieldCount == 2);

    Row = mysql_fetch_row(pResultSet);
    KGLOG_PROCESS_ERROR(Row);

    nRetCode = strcmp(Row[1], "1");
    if (nRetCode)
    {
        KGLogPrintf(KGLOG_ERR, "[DB] DB error, lower_case_table_names --> 1\n");
        goto Exit0;
    }

    bResult = true;
Exit0:
    if (pResultSet)
    {
        mysql_free_result(pResultSet);
        pResultSet = NULL;
    }
    return bResult;
}

BOOL MySQL_CheckCharacterSet(MYSQL* pHandle)
{
    BOOL        bResult         = false;
    int         nRetCode        = 0;
    char        szSQL[]         = "show variables like \'character_set_database\'";
    MYSQL_RES*  pResultSet      = NULL;
    MYSQL_ROW   Row             = NULL;
    unsigned    uFieldCount     = 0;

    assert(pHandle);
    
    // get database character set
    nRetCode = MySQL_Query(pHandle, szSQL);
    KGLOG_PROCESS_ERROR(nRetCode);

    pResultSet = mysql_store_result(pHandle);
    KGLOG_PROCESS_ERROR(pResultSet);

    uFieldCount = mysql_num_fields(pResultSet);    
    KGLOG_PROCESS_ERROR(uFieldCount == 2);

    Row = mysql_fetch_row(pResultSet);
    KGLOG_PROCESS_ERROR(Row);

    KGLOG_PROCESS_ERROR(Row[1]);

    nRetCode = mysql_set_character_set(pHandle, Row[1]);    
    KGLOG_PROCESS_ERROR(nRetCode == 0);

    bResult = true;
Exit0:
    if (pResultSet)
    {
        mysql_free_result(pResultSet);
        pResultSet = NULL;
    }
    return bResult;
}

MYSQL* MySQL_Connect(
    const char cszDBAddr[], const char cszDBName[], const char cszUserName[], const char cszPassword[]
)
{
    MYSQL*        pResult                   = NULL;
    int           nRetCode                  = false;
    MYSQL*        pDBHandle                 = NULL;
    MYSQL*        pTmpHandle                = NULL;
    my_bool       bReconnectFlag            = 0;
    unsigned long ulMySQLClientVersionID    = 0;
    int           nStrLen                   = 0;
    char          szSQL[1024];

    pDBHandle = mysql_init(NULL);
    KGLOG_PROCESS_ERROR(pDBHandle);
    
    // enable reconnect if MySQL client version > 5
    ulMySQLClientVersionID = mysql_get_client_version();
    if (ulMySQLClientVersionID >= MYSQL5_VERSION_ID)
    {
        bReconnectFlag = 1;
        nRetCode = mysql_options(pDBHandle, MYSQL_OPT_RECONNECT, &bReconnectFlag);
        KGLOG_PROCESS_ERROR(nRetCode == 0);
    }

    pTmpHandle = mysql_real_connect(
        pDBHandle, cszDBAddr, cszUserName, cszPassword, "", 3306, NULL, 0
    );
    if (pTmpHandle == NULL)
    {
        KGLogPrintf(
            KGLOG_ERR, 
            "[DB] Can't open database \"%s\" at \"%s\", db error : \"%s\"\n", 
            cszDBName, cszDBAddr, mysql_error(pDBHandle)
        );
        goto Exit0;
    }
    
    nStrLen = snprintf(
        szSQL, sizeof(szSQL), "create database if not exists %s", cszDBName
    );
    KGLOG_PROCESS_ERROR(nStrLen > 0 && nStrLen < (int)sizeof(szSQL));

    nRetCode = MySQL_Query(pDBHandle, szSQL);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = mysql_select_db(pDBHandle, cszDBName);
    KGLOG_PROCESS_ERROR(nRetCode == 0);

    nRetCode = MySQL_CheckCaseInsensitive(pDBHandle);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = MySQL_CheckCharacterSet(pDBHandle);
    KGLOG_PROCESS_ERROR(nRetCode);

    pResult = pDBHandle;
Exit0:
    if (!pResult)
    {
        if (pDBHandle)
        {
            KGLogPrintf(KGLOG_ERR, "%d:%s\n", mysql_errno(pDBHandle), mysql_error(pDBHandle));

            mysql_close(pDBHandle);
            pDBHandle = NULL;
        }
    }
    return pResult;
}

void MySQL_Disconnect(MYSQL* pHandle)
{
    if (pHandle)
    {
        mysql_close(pHandle);
        pHandle = NULL;
    }
}

BOOL MySQL_Query(MYSQL* pHandle, const char cszSQL[], unsigned* puRetMySQLErrorCode /*= NULL*/)
{
    BOOL        bResult             = false;
    int         nRetCode            = false;
    int         nReconnectTimes     = 0;
    unsigned    uMySQLErrorCode     = 0;
    size_t      uStrLen             = strlen(cszSQL);

    assert(pHandle);
    assert(cszSQL);
    
    while (true) 
    {
        nRetCode = mysql_real_query(pHandle, cszSQL, (unsigned long)uStrLen);
        if (nRetCode == 0)
            break;

        uMySQLErrorCode = mysql_errno(pHandle);
        KG_PROCESS_ERROR(uMySQLErrorCode != ER_DUP_ENTRY);

        if (
            (uMySQLErrorCode != CR_SERVER_LOST) &&
            (uMySQLErrorCode != CR_CONN_HOST_ERROR) &&
            (uMySQLErrorCode != CR_SERVER_GONE_ERROR)
        )
        {
            KGLogPrintf(                                                        
                KGLOG_ERR,                                                    
                "[DB] Call \"mysql_real_query\" failed : %s\n",
                mysql_error(pHandle)
            );  
            goto Exit0;
        }
        
        while (nReconnectTimes < 5)
        { 
            nRetCode = mysql_ping(pHandle);
            if (nRetCode == 0)
                break;

            KGThread_Sleep(300);
            ++nReconnectTimes;
        }
        
        if (nReconnectTimes >= 5)
        {
            KGLogPrintf(                                                        
                KGLOG_ERR,                                                    
                "[DB] Call \"mysql_ping\" failed : %s\n",
                mysql_error(pHandle)
            );
            
            goto Exit0;
        }
    }

    bResult = true;
Exit0:
    if (puRetMySQLErrorCode)
    {
        *puRetMySQLErrorCode = uMySQLErrorCode;
    }
    return bResult; 
}
