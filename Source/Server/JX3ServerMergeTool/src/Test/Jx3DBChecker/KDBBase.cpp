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
#include "mysql/errmsg.h"
#include "mysql/mysqld_error.h"

static const unsigned long MYSQL5_VERSION_ID = 50000; // MySQL 5 Version ID

KDBBase::KDBBase()
{
    m_pDBHandle             = NULL;
    m_bConnected            = false;
    m_nNextPingTime         = 0;
    m_nDBState              = dbs_not_connect;
}

KDBBase::~KDBBase()
{
    ASSERT(m_pDBHandle == NULL);
    ASSERT(!m_bConnected);
}

int KDBBase::Connect(
    const char cszDBAddr[], const char cszDBName[], 
    const char cszUserName[], const char cszPassword[]
)
{
    int          bResult                   = false;
    int           nRetCode                  = false;
    MYSQL*        pTmpHandle                = NULL;
    my_bool       bReconnectFlag            = 0;
    unsigned long ulMySQLClientVersionID    = 0;
//    int           nStrLen                   = 0;
//    char          szSQL[1024];

    ASSERT(m_pDBHandle == NULL);

    m_pDBHandle = mysql_init(NULL);
    KGLOG_PROCESS_ERROR(m_pDBHandle);
    
    // enable reconnect if MySQL client version > 5
    ulMySQLClientVersionID = mysql_get_client_version();
    if (ulMySQLClientVersionID >= MYSQL5_VERSION_ID)
    {
        bReconnectFlag = 1;
        nRetCode = mysql_options(m_pDBHandle, MYSQL_OPT_RECONNECT, &bReconnectFlag);
        KGLOG_PROCESS_ERROR(nRetCode == 0);
    }

    pTmpHandle = mysql_real_connect(
        m_pDBHandle, cszDBAddr, cszUserName, cszPassword, "", 3306, NULL, 0
    );
    if (pTmpHandle == NULL)
    {
        KGLogPrintf(
            KGLOG_ERR, 
            "[DB] Can't open database \"%s\" at \"%s\", db error : \"%s\"\n", 
            cszDBName, cszDBAddr, mysql_error(m_pDBHandle)
        );
        m_nDBState = dbs_not_connect;
        goto Exit0;
    }
    
    m_bConnected = true;
    
    nRetCode = mysql_select_db(m_pDBHandle, cszDBName);
    if (!nRetCode)
    {
        m_nDBState = dbs_Exist;
    }
    else
    {
        KGLogPrintf(
            KGLOG_ERR, 
            "[DB] Can't use database \"%s\" at \"%s\", db error : \"%s\"\n", 
            cszDBName, cszDBAddr, mysql_error(m_pDBHandle)
        );
        m_nDBState = dbs_new_Create;
    }

    //nStrLen = snprintf(
    //    szSQL, sizeof(szSQL), "create database if not exists %s /*!40100 DEFAULT CHARACTER SET gbk */", cszDBName
    //);
    //KGLOG_PROCESS_ERROR(nStrLen > 0 && nStrLen < (int)sizeof(szSQL));

    //nRetCode = DoQuery(szSQL);
    //KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = mysql_select_db(m_pDBHandle, cszDBName);
    KGLOG_PROCESS_ERROR(nRetCode == 0);

    nRetCode = CheckCaseInsensitive();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = CheckCharacterSet();
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    if (!bResult)
    {
        if (m_pDBHandle)
        {
            mysql_close(m_pDBHandle);
            m_pDBHandle = NULL;
        }

        m_bConnected = false;
    }
    return bResult;
}

void KDBBase::Disconnect()
{
    m_bConnected = false;

    if (m_pDBHandle)
    {
        mysql_close(m_pDBHandle);
        m_pDBHandle     = NULL;
    }
}

int KDBBase::CheckCaseInsensitive()
{
    int        bResult         = false;
    int         nRetCode        = 0;
    char        szSQL[]         = "show variables like \'lower_case_table_names\'";
    MYSQL_RES*  pResultSet      = NULL;
    MYSQL_ROW   Row             = NULL;
    unsigned    uFieldCount     = 0;

    nRetCode = DoQuery(szSQL);
    KGLOG_PROCESS_ERROR(nRetCode);

    pResultSet = mysql_store_result(m_pDBHandle);
    KGLOG_PROCESS_ERROR(pResultSet);

    uFieldCount = mysql_num_fields(pResultSet);    
    KGLOG_PROCESS_ERROR(uFieldCount == 2);

    Row = mysql_fetch_row(pResultSet);
    KGLOG_PROCESS_ERROR(Row);

    nRetCode = strcmp(Row[1], "1");
    if (nRetCode)
    {
        //KGLogPrintf(KGLOG_ERR, "[DB] DB error, lower_case_table_names --> 1\n");
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

int KDBBase::CheckCharacterSet()
{
    int        bResult         = false;
    int         nRetCode        = 0;
    char        szSQL[]         = "show variables like \'character_set_database\'";
    MYSQL_RES*  pResultSet      = NULL;
    MYSQL_ROW   Row             = NULL;
    unsigned    uFieldCount     = 0;

    // get database character set

    nRetCode = DoQuery(szSQL);
    KGLOG_PROCESS_ERROR(nRetCode);

    pResultSet = mysql_store_result(m_pDBHandle);
    KGLOG_PROCESS_ERROR(pResultSet);

    uFieldCount = mysql_num_fields(pResultSet);
    KGLOG_PROCESS_ERROR(uFieldCount == 2);

    Row = mysql_fetch_row(pResultSet);
    KGLOG_PROCESS_ERROR(Row);

    KGLOG_PROCESS_ERROR(Row[1]);
    
    nRetCode = mysql_set_character_set(m_pDBHandle, Row[1]); 
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

int KDBBase::DoQuery(const char cszSQL[])
{
    int        bResult             = false;
    int         nRetCode            = false;
    int         nReconnectTimes     = 0;
    unsigned    uMySQLErrorCode     = 0;
    size_t      uStrLen             = strlen(cszSQL);
    
    KGLOG_PROCESS_ERROR(m_bConnected);

    while (true) 
    {
        nRetCode = mysql_real_query(m_pDBHandle, cszSQL, (unsigned long)uStrLen);
        if (nRetCode == 0)
            break;

        uMySQLErrorCode = mysql_errno(m_pDBHandle);
        KGLOG_PROCESS_ERROR(uMySQLErrorCode != ER_DUP_ENTRY);

        if (
            (uMySQLErrorCode != CR_SERVER_LOST) &&
            (uMySQLErrorCode != CR_CONN_HOST_ERROR) &&
            (uMySQLErrorCode != CR_SERVER_GONE_ERROR)
        )
        {
            KGLogPrintf(                                                        
                KGLOG_ERR,                                                    
                "[DB] Call \"mysql_real_query\" failed : %s\n",
                mysql_error(m_pDBHandle)
            );  
            goto Exit0;
        }
        
        while (nReconnectTimes < 5)
        { 
            nRetCode = mysql_ping(m_pDBHandle);
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
                mysql_error(m_pDBHandle)
            );

            Disconnect(); // need reconnect in work thread
            goto Exit0;
        }
    }

//Exit1:
    bResult = true;
Exit0:
    return bResult; 
}
