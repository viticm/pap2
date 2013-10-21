//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KDBTools.cpp
//  Version     :   1.0
//  Creater     :   Liuzhibiao
//  Date        :   2009-7-13
//  Comment     :   访问数据库扩展
//////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "mysql/mysql.h"
#include "KDBBase.h"
#include "KDBTools.h"

KDBTools::KDBTools(void)
{
}

KDBTools::~KDBTools(void)
{
}

int KDBTools::Init(char szConnectIP[], char DBAcc[], char szDBPsw[], char szDBName[])
{
    int  nResult    = false;
    BOOL bRetCode   = false;

    bRetCode = Connect(
        szConnectIP,
        szDBName,
        DBAcc, 
        szDBPsw
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    memset(m_szSQL, '\0', sizeof(m_szSQL));

    nResult = true;
Exit0:
    return nResult;
}

void KDBTools::UnInit()
{
    Disconnect();
}

int KDBTools::GetInfo(char szTableName[], char szGetFieldName[],
                      char szSearchFieldName[], char szSearchName[],
                      BYTE* pbyData, size_t uDataSize, size_t& uUseSize, int& nHadFind
)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    int                         nRetCode        = false;
    MYSQL_RES*                  pQueryRes       = NULL;
    MYSQL_ROW                   QueryRow;
    unsigned long*              pulLengths      = NULL;

    ASSERT(m_pDBHandle);
    ASSERT(szTableName);
    ASSERT(szGetFieldName);
    ASSERT(szSearchFieldName);
    ASSERT(szSearchName);
    ASSERT(pbyData);

    nRetCode = snprintf(
        m_szSQL, sizeof(m_szSQL), "select %s from %s where %s = \'%s\'", 
        szGetFieldName, szTableName, szSearchFieldName, szSearchName
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    KGLOG_PROCESS_ERROR(bRetCode);

    pQueryRes = mysql_use_result(m_pDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    QueryRow = mysql_fetch_row(pQueryRes);
    if (QueryRow == NULL)
    {
        nHadFind = false;
        goto Exit1;
    }
    else
    {
        nHadFind = true;
    }

    pulLengths = mysql_fetch_lengths(pQueryRes);
    KGLOG_PROCESS_ERROR(pulLengths);

    uUseSize = pulLengths[0];
    KGLOG_PROCESS_ERROR(uDataSize >= uUseSize);

    memcpy(pbyData, QueryRow[0], uUseSize);

Exit1:
    bResult = true;
Exit0:

    if (pQueryRes)
    {
        mysql_free_result(pQueryRes);
        pQueryRes = NULL;
    }
    return bResult;
}

int KDBTools::DelInfoIfFind(char szTableName[], char szGetFieldName[],
                            char szSearchFieldName[], char szSearchName[],
                            int& nHadFind
                            )
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    int                         nRetCode        = false;
    MYSQL_RES*                  pQueryRes       = NULL;
    MYSQL_ROW                   QueryRow;

    ASSERT(m_pDBHandle);
    ASSERT(szTableName);
    ASSERT(szGetFieldName);
    ASSERT(szSearchFieldName);
    ASSERT(szSearchName);

    nRetCode = snprintf(
        m_szSQL, sizeof(m_szSQL), "select %s from %s where %s = \'%s\'", 
        szGetFieldName, szTableName, szSearchFieldName, szSearchName
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    KGLOG_PROCESS_ERROR(bRetCode);

    pQueryRes = mysql_use_result(m_pDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    QueryRow = mysql_fetch_row(pQueryRes);
    if (QueryRow == NULL)
    {
        nHadFind = false;
        goto Exit1;
    }
    else
    {
        nHadFind = true;
    }

    if (nHadFind)
    {
        nRetCode = snprintf(
            m_szSQL, sizeof(m_szSQL), "DELETE * from %s where %s = \'%s\'", 
            szTableName, szSearchFieldName, szSearchName
            );
        KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

        bRetCode = DoQuery(m_szSQL);
        KGLOG_PROCESS_ERROR(bRetCode);

        pQueryRes = mysql_use_result(m_pDBHandle);
        KGLOG_PROCESS_ERROR(pQueryRes);
    }

Exit1:
    bResult = true;
Exit0:

    if (pQueryRes)
    {
        mysql_free_result(pQueryRes);
        pQueryRes = NULL;
    }
    return bResult;
}

int KDBTools::LockTable(const char cszTableName[])
{
    int nResult  = false;
    int nRetCode = false;
    string strQuery;

    KGLOG_PROCESS_ERROR(cszTableName);
    KGLOG_PROCESS_ERROR(cszTableName[0]);

    strQuery  = "lock tables `";
    strQuery += cszTableName;
    strQuery += "` write";

    ASSERT(m_pDBHandle);
    nRetCode = mysql_real_query(
        m_pDBHandle, strQuery.c_str(), (unsigned long)strQuery.length()
    );
    KGD_MYSQL_PROCESS_ERROR(nRetCode == 0, m_pDBHandle, strQuery.c_str());
    nResult = true;
Exit0:
    return nResult;
}

int KDBTools::UnLockTable()
{
    int nResult  = false;
    int nRetCode = false;

    ASSERT(m_pDBHandle);
    nRetCode = mysql_real_query(
        m_pDBHandle, "unlock tables", sizeof("unlock tables") - 1
    );
    KGD_MYSQL_PROCESS_ERROR(nRetCode == 0, m_pDBHandle, "unlock tables");

    nResult = true;
Exit0:
    return nResult;
}

int KDBTools::HadRecordInTable(char szTableName[], char szGetFieldName[],
                               char szSearchFieldName[], int nID,
                               int& nHadFind
)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    int                         nRetCode        = false;
    MYSQL_RES*                  pQueryRes       = NULL;
    MYSQL_ROW                   QueryRow;

    ASSERT(m_pDBHandle);
    ASSERT(szTableName);
    ASSERT(szGetFieldName);
    ASSERT(szSearchFieldName);

    nRetCode = snprintf(
        m_szSQL, sizeof(m_szSQL), "select %s from %s where %s = %u", 
        szGetFieldName, szTableName, szSearchFieldName, nID
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    KGLOG_PROCESS_ERROR(bRetCode);

    pQueryRes = mysql_use_result(m_pDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);


    QueryRow = mysql_fetch_row(pQueryRes);
    if (QueryRow == NULL)
    {
        nHadFind = false;
        goto Exit1;
    }
    else
    {
        nHadFind = true;
    }

Exit1:
    bResult = true;
Exit0:

    if (pQueryRes)
    {
        mysql_free_result(pQueryRes);
        pQueryRes = NULL;
    }
    return bResult;
}

int KDBTools::GetFieldsName(const char cszTableName[], FIELD_NAME_VECTOR& refRetFieldNameVector)
{
    int nResult  = false;
    MYSQL_RES *pFieldSet = NULL;
    MYSQL_FIELD *pFields = NULL;
    unsigned uFieldCount = 0;

    KGLOG_PROCESS_ERROR(cszTableName);
    KGLOG_PROCESS_ERROR(cszTableName[0]);

    refRetFieldNameVector.clear();

    ASSERT(m_pDBHandle);
    pFieldSet = mysql_list_fields(m_pDBHandle, cszTableName, NULL);
    KGD_MYSQL_PROCESS_ERROR(pFieldSet, m_pDBHandle, "mysql_list_fields()");

    uFieldCount = mysql_num_fields(pFieldSet);
    KGLOG_PROCESS_ERROR(uFieldCount);

    pFields = mysql_fetch_fields(pFieldSet);
    KGLOG_PROCESS_ERROR(pFields);

    for (unsigned i = 0; i < uFieldCount; ++i)
    {
        refRetFieldNameVector.push_back(pFields[i].name);
    }

    nResult = true;
Exit0:
    if (pFieldSet)
    {
        mysql_free_result(pFieldSet);
        pFieldSet = NULL;
    }
    if (!nResult)
    {
        refRetFieldNameVector.clear();
    }
    return nResult;
}

MYSQL_RES *KDBTools::CreateRecordSet(
                                     const char cszTableName[], 
                                     const FIELD_NAME_VECTOR &crFieldNameVector,
                                     const FIELD_ID_VECTOR &crOrderByFields,
                                     const bool bHasCondition,
                                     const char cszSearchFieldName[],
                                     const char cszSearchThing[]
)
{
    int nResult  = false;
    int nRetCode = false;
    MYSQL_RES *pRetRecordSet = NULL;
    string strQuery;
    unsigned uTableFieldCount   = 0;
    //unsigned uOrderByFieldCount = 0;
    //unsigned uFieldID           = 0;

    KGLOG_PROCESS_ERROR(cszTableName);
    KGLOG_PROCESS_ERROR(cszTableName[0]);

    uTableFieldCount = (unsigned)crFieldNameVector.size();
    KGLOG_PROCESS_ERROR(uTableFieldCount);

    strQuery = "select";
    for (unsigned i = 0; i < uTableFieldCount; i++)
    {
        if (i > 0)
            strQuery += ",";

        strQuery += " `";
        strQuery += crFieldNameVector[i];
        strQuery += "`";
    }
    strQuery += " from `";
    strQuery += cszTableName;
    strQuery += "`";

    if (bHasCondition)
    {
        strQuery += " where ";
        strQuery += cszSearchFieldName;
        strQuery += " = \"";
        strQuery += cszSearchThing;
        strQuery += "\"";
    }

    //// crOrderByFields is empty indicate the record set not order by any fields.
    //uOrderByFieldCount = (unsigned)crOrderByFields.size();
    //if (uOrderByFieldCount > 0) 
    //{
    //    strQuery += " order by";
    //    for (unsigned i = 0; i < uOrderByFieldCount; i++)
    //    {
    //        uFieldID = crOrderByFields[i];
    //        KGLOG_PROCESS_ERROR(uFieldID < uTableFieldCount);

    //        if (i > 0)
    //            strQuery += ",";

    //        strQuery += " `";
    //        strQuery += crFieldNameVector[uFieldID];
    //        strQuery += "`";
    //    }
    //}

    ASSERT(m_pDBHandle);
    nRetCode = mysql_real_query(
        m_pDBHandle, strQuery.c_str(), (unsigned long)strQuery.length()
    );
    KGD_MYSQL_PROCESS_ERROR(nRetCode == 0, m_pDBHandle, strQuery.c_str());

    pRetRecordSet = mysql_use_result(m_pDBHandle);
    KGD_MYSQL_PROCESS_ERROR(pRetRecordSet, m_pDBHandle, "mysql_use_result()");

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (pRetRecordSet)
        {
            mysql_free_result(pRetRecordSet);
            pRetRecordSet = NULL;
        }
    }
    return pRetRecordSet;
}

int KDBTools::FreeRecordSet(MYSQL_RES*& refpRecordSet)
{
    int nResult  = false;

    //KGLOG_PROCESS_ERROR(pRecordSet);

    if (refpRecordSet)
    {
        mysql_free_result(refpRecordSet);
        refpRecordSet = NULL;
    }

    nResult = true;
Exit0:
    return nResult;
}

int KDBTools::GetNextRecord(MYSQL_RES* pRecordSet, FIELD_INFO_VECTOR& refFieldInfoVector, bool& refbIsNoMoreRecodes)
{
    int nResult  = false;
    int nRetCode = false;
//    int nCreateBufferFlag = false;
    MYSQL_ROW Row = NULL;
    unsigned long *pulLengths = NULL;
    unsigned uFieldCount = 0;
//    unsigned uBufferSize = 0;
    FIELD_INFO fieldInfo;

    fieldInfo.uDataSize = 0;
    KGLOG_PROCESS_ERROR(pRecordSet);

    uFieldCount = mysql_num_fields(pRecordSet);
    KGLOG_PROCESS_ERROR(uFieldCount);

    //refFieldInfoVector.resize(uFieldCount);

    Row = mysql_fetch_row(pRecordSet);
    if (Row == NULL)
    {   
        nRetCode = (int)mysql_errno(m_pDBHandle);
        KGD_MYSQL_PROCESS_ERROR(nRetCode == 0, m_pDBHandle, "mysql_fetch_row()");
        refbIsNoMoreRecodes = true;
        KG_PROCESS_SUCCESS(true);   // no more records
    }
    else
    {
        refbIsNoMoreRecodes = false;
    }

    pulLengths  = mysql_fetch_lengths(pRecordSet);
    KGLOG_PROCESS_ERROR(pulLengths);

    for (unsigned i = 0; i < uFieldCount; i++)
    {
        fieldInfo.pvData = NULL;
        fieldInfo.uDataSize = 0;

        if (pulLengths[i] != 0) // NULL or empty string
        {
            fieldInfo.pvData = new unsigned char[pulLengths[i]];
            KGLOG_PROCESS_ERROR(fieldInfo.pvData);  
            fieldInfo.uDataSize = (unsigned)pulLengths[i];
            memcpy(fieldInfo.pvData, Row[i], (unsigned)pulLengths[i]);
        }

        refFieldInfoVector.push_back(fieldInfo);
    }

Exit1:    
    nResult = true;
Exit0:
    if (!nResult)
    {

        for (size_t i = 0; i < refFieldInfoVector.size(); i++)
        {
            KG_DELETE_ARRAY(refFieldInfoVector[i].pvData);
        }
        refFieldInfoVector.clear();
    }
    return nResult;
}

int KDBTools::UpDateField(char szTableName[], char szUpdateFieldName[], 
                          unsigned char* ucUpdateData, unsigned int unUndateDataSize,
                          char szSearchFieldName[], int nID,
                          int& nHadFind
)
{    
    int nResult  = false;
    int nRetCode = false;

    char*           pszPos          = NULL;
    size_t          uLeftBufferSize = 0;
    unsigned long   ulLength        = 0;
    my_ulonglong    lAffectedRow    = 0;
    uLeftBufferSize = MAX_ROLE_DB_SQL_SIZE;

    memset(m_szSQL,0, sizeof(m_szSQL));
    pszPos = m_szSQL;

    nRetCode = snprintf(
        pszPos, uLeftBufferSize, "update %s set %s = '",
        szTableName, szUpdateFieldName
    );

    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = (unsigned long)unUndateDataSize;
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);

    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)ucUpdateData, ulLength);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    nRetCode = snprintf(pszPos, uLeftBufferSize, "' ");
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    nRetCode = snprintf(pszPos, uLeftBufferSize, " where %s = %u ", szSearchFieldName, nID);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);

    nRetCode = DoQuery(m_szSQL);
    if (!nRetCode) 
    {
        if (m_bConnected) // 执行失败的情况下且数据库连接没有断开
        {
            //nRetCode = eCreateRoleNameAlreadyExist;
        }
        goto Exit0;
    }

    lAffectedRow = mysql_affected_rows(m_pDBHandle);
    KGLOG_PROCESS_ERROR(lAffectedRow <= 1);

    nResult = true;
Exit0:
    return nResult;
}

int KDBTools::UpDateField(char szTableName[], char szUpdateFieldName[], 
                          unsigned char* ucUpdateData, unsigned int unUndateDataSize,
                          char szSearchFieldName[], char szSearchName[],
                          int& nHadFind
)
{    
    int nResult  = false;
    int nRetCode = false;

    char*           pszPos          = NULL;
    size_t          uLeftBufferSize = 0;
    unsigned long   ulLength        = 0;
    my_ulonglong    lAffectedRow    = 0;

    uLeftBufferSize = MAX_ROLE_DB_SQL_SIZE;

    memset(m_szSQL,0, sizeof(m_szSQL));
    pszPos = m_szSQL;

    nRetCode = snprintf(
        pszPos, uLeftBufferSize, "update %s set %s = '",
        szTableName, szUpdateFieldName
    );

    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = (unsigned long)unUndateDataSize;
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);

    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)ucUpdateData, ulLength);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    nRetCode = snprintf(pszPos, uLeftBufferSize, "' ");
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    nRetCode = snprintf(pszPos, uLeftBufferSize, " where %s = `%s` ", szSearchFieldName, szSearchName);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);

    nRetCode = DoQuery(m_szSQL);
    if (!nRetCode) 
    {
        if (m_bConnected) // 执行失败的情况下且数据库连接没有断开
        {
            //nRetCode = eCreateRoleNameAlreadyExist;
        }
        goto Exit0;
    }

    lAffectedRow = mysql_affected_rows(m_pDBHandle);
    KGLOG_PROCESS_ERROR(lAffectedRow <= 1);

    nResult = true;
Exit0:
    return nResult;
}


int KDBTools::InsertNewRecord(const char cszTableName[], const FIELD_NAME_VECTOR& crFieldNameVector, const FIELD_INFO_VECTOR& crFieldInfoVector, int& rnSucceed)
{
    int nResult  = false;
    int nRetCode = false;

    char*           pszPos          = NULL;
    size_t          uLeftBufferSize = 0;
    unsigned long   ulLength        = 0;
    my_ulonglong    lAffectedRow    = 0;
    unsigned        uFieldCount     = 0;
    //unsigned        uDataLength     = 0;

    uLeftBufferSize = MAX_ROLE_DB_SQL_SIZE;

    KGLOG_PROCESS_ERROR(cszTableName);
    KGLOG_PROCESS_ERROR(cszTableName[0]);

    KGLOG_PROCESS_ERROR(crFieldNameVector.size() == crFieldInfoVector.size());

    memset(m_szSQL,0, sizeof(m_szSQL));
    pszPos = m_szSQL;

    nRetCode = snprintf(
        pszPos, uLeftBufferSize, "insert into `%s` (",
        cszTableName
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    uFieldCount = (unsigned)crFieldNameVector.size();
    KGLOG_PROCESS_ERROR(uFieldCount);
    for (unsigned i = 0; i < uFieldCount; i++)
    {
        if (i > 0)
        {
            nRetCode = snprintf(
                pszPos, uLeftBufferSize,
                " , "
            );
            KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
            pszPos          += nRetCode;
            uLeftBufferSize -= nRetCode;
        }

        nRetCode = snprintf(
            pszPos, uLeftBufferSize,
            "`%s`", crFieldNameVector[i].c_str()
        );
        KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
        pszPos          += nRetCode;
        uLeftBufferSize -= nRetCode;
    }

    nRetCode = snprintf(
        pszPos, uLeftBufferSize,
        ") values ('"
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    for (unsigned i = 0; i < uFieldCount; i++)
    {
        if (i > 0)
        {
            nRetCode = snprintf(
                pszPos, uLeftBufferSize,
                ", '"
            );
            KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
            pszPos          += nRetCode;
            uLeftBufferSize -= nRetCode;
        }

        ulLength = (unsigned long)crFieldInfoVector[i].uDataSize;
        KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);

        nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)crFieldInfoVector[i].pvData, ulLength);
        KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
        pszPos          += nRetCode;
        uLeftBufferSize -= nRetCode;

        nRetCode = snprintf(pszPos, uLeftBufferSize, "' ");
        KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
        pszPos          += nRetCode;
        uLeftBufferSize -= nRetCode;
    }

    nRetCode = snprintf(
        pszPos, uLeftBufferSize,
        ");"
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    rnSucceed = false;
    nRetCode = DoQuery(m_szSQL);
    if (!nRetCode) 
    {
        if (m_bConnected) // 执行失败的情况下且数据库连接没有断开
        {
            //nRetCode = eCreateRoleNameAlreadyExist;

        }
        goto Exit0;
    }

    lAffectedRow = mysql_affected_rows(m_pDBHandle);
    KGLOG_PROCESS_ERROR(lAffectedRow == 1);

    rnSucceed = true;
    nResult = true;
Exit0:
    return nResult;
}
