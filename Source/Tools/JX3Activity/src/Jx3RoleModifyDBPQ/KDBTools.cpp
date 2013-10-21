//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KDBTools.cpp
//  Version     :   1.0
//  Creater     :   Liuzhibiao
//  Date        :   2009-7-13
//  Comment     :   访问数据库扩展
//////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mysql/mysql.h"
#include "KDBBase.h"
#include "KDBTools.h"
#include "KCommon.h"

KDBTools::KDBTools(void)
{
}

KDBTools::~KDBTools(void)
{
}

int KDBTools::Init(char szConnectIP[], char DBAcc[], char szDBPsw[], char szDBName[])
{
    int  nResult    = false;
    int bRetCode   = false;

    bRetCode = Connect(
        szConnectIP,
        szDBName,
        DBAcc, 
        szDBPsw
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    nResult = true;
Exit0:
    return nResult;
}

void KDBTools::UnInit()
{
    Disconnect();
}

int KDBTools::UpdatePQTableStartTime(int nID, unsigned char* ucData, unsigned long ulDataSize)
{
    int             nResult         = false;
    int             nRetCode        = false;
    char*           pszPos          = NULL;
    size_t          uLeftBufferSize = MAX_ROLE_DB_SQL_SIZE;
    my_ulonglong    lAffectedRow    = 0;

    ASSERT(m_pDBHandle);

    pszPos = m_szSQL;

    nRetCode = snprintf(
        m_szSQL, sizeof(m_szSQL),
        "update pq set %s = '",
        "Data"
    );
    
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)ucData, ulDataSize);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    nRetCode = snprintf(pszPos, uLeftBufferSize, "' ");
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    nRetCode = snprintf(pszPos, uLeftBufferSize, " where %s = %u ", "ID", nID);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);

    nRetCode = DoQuery(m_szSQL);
    KGLOG_PROCESS_ERROR(nRetCode);

    lAffectedRow = mysql_affected_rows(m_pDBHandle);
    KGLOG_PROCESS_ERROR(lAffectedRow <= 1);

    nResult = true;
Exit0:
    return nResult;
}

int KDBTools::ModifyPQTableStartTime(int nYear, int nMonth, int nDay, KDBTools* pdbOutTool)
{
    int                        bResult         = false;
    int                        bRetCode        = false;
    int                        nRetCode        = false;
    MYSQL_RES*                 pQueryRes       = NULL;
    MYSQL_ROW                  QueryRow;
    unsigned long*             pulFieldLength  = NULL;
    KG_JX3_PQ_DB_DATA*         pQPDBData       = NULL;
    char*                      pszPos          = NULL;

    ASSERT(m_pDBHandle);

    memset(m_szSQL,0, sizeof(m_szSQL));
    pszPos = m_szSQL;

    nRetCode = snprintf(
        m_szSQL, sizeof(m_szSQL),
        "select ID, Data from pq"
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    KGLOG_PROCESS_ERROR(bRetCode);

    pQueryRes = mysql_use_result(m_pDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);
    
    while (true)
    {
        unsigned long           dwID            = 0;
        tm*             ptmModify       = NULL;
        time_t          nBeginTime;
        QueryRow = mysql_fetch_row(pQueryRes);
        if (QueryRow == NULL)
        {
            break;
        }

        pulFieldLength = mysql_fetch_lengths(pQueryRes);
        KGLOG_PROCESS_ERROR(pulFieldLength);

        KGLOG_PROCESS_ERROR(pulFieldLength[1] == sizeof(KG_JX3_PQ_DB_DATA));
        
        pQPDBData = (KG_JX3_PQ_DB_DATA*)QueryRow[1];
        
        nBeginTime   = pQPDBData->nBeginTime;
        ptmModify = localtime(&nBeginTime);
        KGLOG_PROCESS_ERROR(ptmModify);

        ptmModify->tm_year  = nYear - 1900;
        ptmModify->tm_mon   = nMonth - 1;
        ptmModify->tm_mday  = nDay;
        ptmModify->tm_hour  = 12;
        ptmModify->tm_min   = 0;
        ptmModify->tm_sec   = 0;

        nBeginTime = mktime(ptmModify);
        KGLOG_PROCESS_ERROR(nBeginTime != -1);

        pQPDBData->nBeginTime = nBeginTime;

        dwID = (unsigned long)strtoul(QueryRow[0], NULL, 10);
        nRetCode = pdbOutTool->UpdatePQTableStartTime(dwID, (unsigned char*)pQPDBData, pulFieldLength[1]);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    bResult = true;
Exit0:

    if (pQueryRes)
    {
        mysql_free_result(pQueryRes);
        pQueryRes = NULL;
    }
    return bResult;
}

int KDBTools::CheckPQBeginTime(int nYear, int nMonth, int nDay, int& nIsOK)
{
    int             bResult         = false;
    int             bRetCode        = false;
    int             nRetCode        = false;
    MYSQL_RES*      pQueryRes       = NULL;
    MYSQL_ROW       QueryRow;
    unsigned long*  pulFieldLength  = NULL;
    PARSEHANDLE     pParseHandle    = NULL;
    int             nIniPQ         = false;

    ASSERT(m_pDBHandle);

    nRetCode = snprintf(
        m_szSQL, sizeof(m_szSQL),
        "select ID, Data from pq"
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    KGLOG_PROCESS_ERROR(bRetCode);

    pQueryRes = mysql_use_result(m_pDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    //pQueryRes = mysql_store_result(m_pDBHandle);
    //KGLOG_PROCESS_ERROR(pQueryRes);

    while (true)
    {
        unsigned char    byVersion;
        unsigned long   dwPQID;
        unsigned long   dwPQTemplateID;
        time_t  nBeginTime;
        int     nPQValue[8];
        int     nPQValueSize = 8;
        int     nNeedPQValueSize = 0;
        unsigned char    bFinished;
        unsigned char    byOnwerType;
        unsigned long   dwOnwerID1;
        unsigned long   dwOnwerID2;
        unsigned long   dwOnwerID3;

        QueryRow = mysql_fetch_row(pQueryRes);
        if (QueryRow == NULL)
        {
            break;
        }

        pulFieldLength = mysql_fetch_lengths(pQueryRes);
        KGLOG_PROCESS_ERROR(pulFieldLength);

        pParseHandle = PaserPQTableData((unsigned char*)QueryRow[1], pulFieldLength[1]);
        KGLOG_PROCESS_ERROR(pParseHandle);
        nIniPQ = true;

        nRetCode = GetPQTabeDataInfo(pParseHandle, byVersion, dwPQID, 
            dwPQTemplateID, nBeginTime, 
            nPQValue, nPQValueSize,  nNeedPQValueSize,
            bFinished, byOnwerType, 
            dwOnwerID1, dwOnwerID2, dwOnwerID3
        );
        KGLOG_PROCESS_ERROR(nRetCode);
        
        tm* t = localtime(&nBeginTime);
        KGLOG_PROCESS_ERROR(t);

        nIsOK = true;
        if (t->tm_year + 1900 != nYear)
        {
            nIsOK = false;
        }
        if (t->tm_mon + 1 != nMonth)
        {
            nIsOK = false;
        }
        if (t->tm_mday != nDay)
        {
            nIsOK = false;
        }
        if (t->tm_hour != 12)
        {
            nIsOK = false;
        }
        if (t->tm_min != 0)
        {
            nIsOK = false;
        }
        if (t->tm_sec != 0)
        {
            nIsOK = false;
        }

        if (!nIsOK)
        {
            KGLogPrintf(                                                        
                KGLOG_ERR,  
                "dwPQID: %d\t"
                "dwPQTemplateID: %d\t"
                "nBeginTime: %d/%d/%d,%d:%d:%d\t\n",
                dwPQID,
                dwPQTemplateID, 
                t->tm_year + 1900,t->tm_mon + 1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec
            );
        }

        EndPQDataParse(pParseHandle);
        nIniPQ = false;
    }

    bResult = true;
Exit0:

    if (pQueryRes)
    {
        if (nIniPQ)
        {
            EndPQDataParse(pParseHandle);
            nIniPQ = false;
        }
        mysql_free_result(pQueryRes);
        pQueryRes = NULL;
    }
    return bResult;
}
