///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName : KSO3RoleDBTools.h
// Creator  : liuzhibiao
// Date     : 2009-5-26
// Comment  : v0.1
//            v0.1  2009-5-26   Create  	
///////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "mysql.h"
#include "KDBBase.h"
#include "KSO3RoleDBTools.h"
#include "KSO3RoleDBUpdater.h"

#ifdef WIN32
#define atoll _atoi64
#endif

static inline BOOL CheckRoleDataCRC(const BYTE* pbyRoleData, size_t uDataLen)
{
    BOOL            bResult         = false;
    KRoleDBData*    pRoleData       = NULL;
    unsigned        uRoleCRCInDB    = 0;
    unsigned        uCheckRoleCRC   = 0;

    assert(pbyRoleData);

    KGLOG_PROCESS_ERROR(uDataLen >= sizeof(KRoleDBData));
    pRoleData = (KRoleDBData*)pbyRoleData;

    uRoleCRCInDB = pRoleData->dwCRC;
    pRoleData->dwCRC = 0;

    uCheckRoleCRC = CRC32(0, pRoleData, pRoleData->dwDataLen);
    KGLOG_PROCESS_ERROR(uCheckRoleCRC == uRoleCRCInDB);

    pRoleData->dwCRC = uRoleCRCInDB;

    bResult = true;
Exit0:
    return bResult;
}

void KSO3RoleDBTools::UnInit()
{
    Disconnect();
}

int KSO3RoleDBTools::LoadAndUpdateTable(char szTableName[], char szTimeFileName[])
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    int                         nRetCode        = false;
    MYSQL_RES*                  pQueryRes       = NULL;
    DWORD                       dwRoleCount     = 0;
    KRoleBaseInfo*              pBaseInfo       = NULL;
    MYSQL_ROW                   QueryRow;
    
    ASSERT(m_pDBHandle);
    ASSERT(szTableName);

    KGLogPrintf(KGLOG_INFO, "[DB] %s Data loading ... ...", szTableName);

    nRetCode = snprintf(
        m_szSQL, sizeof(m_szSQL), "select %s, %s, %s, %s, %s, %s from %s", 
        KG_ID_FIELD_NAME,           KG_NAME_FIELD_NAME,     KG_ACCOUNT_FIELD_NAME, 
        KG_BASE_INFO_FIELD_NAME,    KG_EXT_INFO_FIELD_NAME, szTimeFileName, szTableName
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    KGLOG_PROCESS_ERROR(bRetCode);

    pQueryRes = mysql_use_result(m_pDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    KGLogPrintf(KGLOG_INFO, "Current [DB]'s RoleDBDate Version Conver to 11 Version! Please do not close the application !!!!.....");

    while (true)
    {
        DWORD           dwRoleID            = ERROR_ID;
        unsigned long*  pulLengths           = NULL;
        
        QueryRow = mysql_fetch_row(pQueryRes);
        if (QueryRow == NULL)
        {
            goto Exit1;
        }

        pulLengths = mysql_fetch_lengths(pQueryRes);
        KGLOG_PROCESS_ERROR(pulLengths);

        KGLOG_PROCESS_ERROR(pulLengths[1] < _NAME_LEN);
        KGLOG_PROCESS_ERROR(pulLengths[2] < _NAME_LEN);

        if (pulLengths[3] != sizeof(KRoleBaseInfo))
        {
            KGLogPrintf(KGLOG_INFO, "The RoleDBDate is not 14 Version, please Use The 14 version RoleDB!");
            KGLOG_PROCESS_ERROR(false);// 屏蔽转换一半继续转换的功能,如果转换失败在这,请要重新恢复数据库.
            m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_bHadUpdateBaseInfo = true;
        }

        ASSERT(QueryRow[0]);
        ASSERT(QueryRow[1]);
        ASSERT(QueryRow[2]);
        ASSERT(QueryRow[3]);
        ASSERT(QueryRow[5]);
        
        strncpy(m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_szRoleName, QueryRow[1], sizeof(m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_szRoleName));
        m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_szRoleName[sizeof(m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_szRoleName) - 1] = '\0';
        strncpy(m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_szAccountName, QueryRow[2], sizeof(m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_szAccountName));
        m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_szAccountName[sizeof(m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_szAccountName) - 1] = '\0';
        dwRoleID = (DWORD)strtoul(QueryRow[0], NULL, 10);
        
        pBaseInfo = (KRoleBaseInfo*)QueryRow[3];

        m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_dwRoleID = dwRoleID;
        strncpy(m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_szRoleTime, QueryRow[5],sizeof(m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_szRoleTime));
        m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_szRoleTime[sizeof(m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_szRoleTime) - 1] = '\0';

        nRetCode = m_pSO3RoleDBUpdater->UpdateBaseInfo(pBaseInfo);
        if (!nRetCode)
        {
            KGLogPrintf(KGLOG_ERR, "Current [DB]'s RoleDBData Conver BaseInfo ERROR %u %s!", dwRoleID, m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_szRoleName);
        }

        m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_bHadExtInfo = false;
        if (QueryRow[4] != NULL)
        {
            m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_bHadExtInfo = true;
            nRetCode = m_pSO3RoleDBUpdater->UpdateExtInfo((BYTE*)QueryRow[4], pulLengths[4]);
            if (!nRetCode)
            {
                KGLogPrintf(KGLOG_ERR, "Current [DB]'s RoleDBData Conver ExtInfo ERROR %u %s!", dwRoleID, m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_szRoleName);
            }
        }

        nRetCode = m_pSO3RoleDBUpdater->InsertToNewDBRole(szTableName, szTimeFileName);
        KGLOG_PROCESS_ERROR(nRetCode);
        
        nRetCode = m_pSO3RoleDBUpdater->ResetRoleAllInfo();
        KGLOG_PROCESS_ERROR(nRetCode);

        ++dwRoleCount;
    }

Exit1:
    bResult = true;
Exit0:

    KGLogPrintf(KGLOG_INFO, "[DB] %s %u roles loaded !", szTableName, dwRoleCount);

    if (pQueryRes)
    {
        mysql_free_result(pQueryRes);
        pQueryRes = NULL;
    }
    return bResult;

}

int KSO3RoleDBTools::UpdateRoleTable(char szTableName[])
{
    int nResult = false;
    int nRetCode = false;

    char*           pszPos          = NULL;
    size_t          uLeftBufferSize = 0;
    unsigned long   ulLength        = 0;
    my_ulonglong    lAffectedRow    = 0;
    uLeftBufferSize = MAX_ROLE_DB_SQL_SIZE;

    memset(m_szSQL,0, sizeof(m_szSQL));
    pszPos = m_szSQL;

    if (m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_bHadUpdateExtInfo && m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_bHadUpdateBaseInfo)
    {
        goto Exit1;
    }

    nRetCode = snprintf(
        pszPos, uLeftBufferSize, "update %s set %s = '",
        szTableName, KG_BASE_INFO_FIELD_NAME
    );

    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = (unsigned long)sizeof(KBaseInfo_Old);
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)&(m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_BaseInfo), ulLength);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    nRetCode = snprintf(pszPos, uLeftBufferSize, "', %s = ", KG_EXT_INFO_FIELD_NAME);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    if (m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_bHadExtInfo)
    {
        nRetCode = snprintf(pszPos, uLeftBufferSize, "'");
        KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
        pszPos          += nRetCode;
        uLeftBufferSize -= nRetCode;

        BYTE*           pbyExtInfo          = NULL;
        unsigned        uExtInfoDataLen     = 0;
        pbyExtInfo = (BYTE*)m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_piPackage->GetData();
        uExtInfoDataLen = (unsigned int)m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_uPackageUseSize;

        nRetCode = CheckRoleDataCRC(pbyExtInfo, uExtInfoDataLen);
        KGLOG_PROCESS_ERROR(nRetCode);

        ulLength = (unsigned long)uExtInfoDataLen;
        KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);

        nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)pbyExtInfo, ulLength);
        KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
        pszPos          += nRetCode;
        uLeftBufferSize -= nRetCode;

        nRetCode = snprintf(pszPos, uLeftBufferSize, "' ");
        KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
        pszPos          += nRetCode;
        uLeftBufferSize -= nRetCode;
    }
    else
    {
        nRetCode = snprintf(pszPos, uLeftBufferSize, "null ");
        KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
        pszPos          += nRetCode;
        uLeftBufferSize -= nRetCode;
    }

    nRetCode = snprintf(pszPos, uLeftBufferSize, " where %s = %u", KG_ID_FIELD_NAME, m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_dwRoleID);
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
    KGLOG_PROCESS_ERROR(lAffectedRow == 1);

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int KSO3RoleDBTools::LoadMailBoxTable()
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    int                         nRetCode        = false;
    MYSQL_RES*                  pQueryRes       = NULL;
    BYTE*                       pbyData         = NULL;
    unsigned long               ulDataSize       = 0;
    DWORD                       dwRoleCount     = 0;

    MYSQL_ROW                   QueryRow;

    ASSERT(m_pDBHandle);

    KGLogPrintf(KGLOG_INFO, "[DB] MailBox data loading ... ...");

    nRetCode = snprintf(
        m_szSQL, sizeof(m_szSQL), "select %s, %s, %s from %s", 
        KG_ID_FIELD_NAME, KG_MAIL_BOX_INFO_FIELD_NAME, MAIL_BASE_TIME_NAME, MAIL_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    KGLOG_PROCESS_ERROR(bRetCode);

    pQueryRes = mysql_use_result(m_pDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    KGLogPrintf(KGLOG_INFO, "Current [DB]'s MailBoxInfo Version Conver !Please do not close the application !!!!.....");

    while (true)
    {
        DWORD           dwRoleID    = ERROR_ID;
        unsigned long*  ulLengths   = NULL;

        QueryRow = mysql_fetch_row(pQueryRes);
        if (QueryRow == NULL)
        {
            goto Exit1;
        }

        ulLengths = mysql_fetch_lengths(pQueryRes);
        KGLOG_PROCESS_ERROR(ulLengths);

        ASSERT(QueryRow[0]);
        ASSERT(QueryRow[1]);
        ASSERT(QueryRow[2]);

        dwRoleID = (DWORD)strtoul(QueryRow[0], NULL, 10);

        pbyData = (BYTE*)QueryRow[1];
        ulDataSize = ulLengths[1];
        KGLOG_PROCESS_ERROR(ulDataSize <  MAIL_BOX_INFO_MAX_SIZE);

        strncpy(m_pSO3RoleDBUpdater->m_szMailBoxTime, QueryRow[2],sizeof(m_pSO3RoleDBUpdater->m_szMailBoxTime));
        m_pSO3RoleDBUpdater->m_szMailBoxTime[sizeof(m_pSO3RoleDBUpdater->m_szMailBoxTime) - 1] = '\0';

        nRetCode = m_pSO3RoleDBUpdater->ConverMailBoxInfo(dwRoleID, pbyData, ulDataSize);
        KGLOG_PROCESS_ERROR(nRetCode);


        nRetCode = m_pSO3RoleDBUpdater->InsertToNewDBMail(dwRoleID);
        KGLOG_PROCESS_ERROR(nRetCode);
        
        nRetCode = m_pSO3RoleDBUpdater->ResetMailBoxInfo();
        KGLOG_PROCESS_ERROR(nRetCode);

        ++dwRoleCount;
    }

Exit1:
    bResult = true;
Exit0:

    KGLogPrintf(KGLOG_INFO, "[DB] %u MailBoxInfo loaded !", dwRoleCount);

    if (pQueryRes)
    {
        mysql_free_result(pQueryRes);
        pQueryRes = NULL;
    }
    return bResult;
}

int KSO3RoleDBTools::UpdateMailBoxTable(DWORD dwID)
{
    int nResult = false;
    int nRetCode = false;

    char*           pszPos              = NULL;
    size_t          uLeftBufferSize     = 0;
    unsigned long   ulLength            = 0;
    my_ulonglong    lAffectedRow        = 0;
    BYTE*           pbyExtInfo          = NULL;
    unsigned        uExtInfoDataLen     = 0;

    uLeftBufferSize = MAX_ROLE_DB_SQL_SIZE;

    memset(m_szSQL,0, sizeof(m_szSQL));
    pszPos = m_szSQL;

    nRetCode = snprintf(
        pszPos, uLeftBufferSize, "update %s set %s = '",
        MAIL_TABLE_NAME, KG_MAIL_BOX_INFO_FIELD_NAME
    );

    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;


    pbyExtInfo = (BYTE*)m_pSO3RoleDBUpdater->m_piMailBoxInfoPackage->GetData();
    nRetCode = m_pSO3RoleDBUpdater->GetMailBoxInfoSize(uExtInfoDataLen);
    KGLOG_PROCESS_ERROR(nRetCode);

    ulLength = (unsigned long)uExtInfoDataLen;
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)pbyExtInfo, uExtInfoDataLen);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    nRetCode = snprintf(pszPos, uLeftBufferSize, "' ");
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;


    nRetCode = snprintf(pszPos, uLeftBufferSize, "where %s = %u", KG_ID_FIELD_NAME, dwID);
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
    KGLOG_PROCESS_ERROR(lAffectedRow == 1);

    nResult = true;
Exit0:
    return nResult;
}

int KSO3RoleDBTools::InsertRoleDateToNewDB(char szTableName[], char szTimeFileName[])
{
    int nResult = false;
    int nRetCode = false;

    char*           pszPos          = NULL;
    size_t          uLeftBufferSize = 0;
    unsigned long   ulLength        = 0;
    my_ulonglong    lAffectedRow    = 0;

    uLeftBufferSize = MAX_ROLE_DB_SQL_SIZE;

    memset(m_szSQL,0, sizeof(m_szSQL));
    pszPos = m_szSQL;

    nRetCode = snprintf(
        pszPos, uLeftBufferSize, "insert into %s (%s, %s, %s, %s, %s, %s) "
        "values (%u, '",
        szTableName, KG_ID_FIELD_NAME, szTimeFileName, KG_NAME_FIELD_NAME, KG_ACCOUNT_FIELD_NAME, 
        KG_BASE_INFO_FIELD_NAME, KG_EXT_INFO_FIELD_NAME,
        m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_dwRoleID
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = (unsigned long)strlen(m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_szRoleTime);
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_szRoleTime, ulLength);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    nRetCode = snprintf(pszPos, uLeftBufferSize, "', '");
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = (unsigned long)strlen(m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_szRoleName);
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_szRoleName, ulLength);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    nRetCode = snprintf(pszPos, uLeftBufferSize, "', '");
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = (unsigned long)strlen(m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_szAccountName);
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_szAccountName, ulLength);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    nRetCode = snprintf(pszPos, uLeftBufferSize, "', '");
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = (unsigned long)sizeof(KBaseInfo_Old);
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)&m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_BaseInfo, ulLength);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    if (m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_bHadExtInfo)
    {
        nRetCode = snprintf(pszPos, uLeftBufferSize, "', '");
        KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
        pszPos          += nRetCode;
        uLeftBufferSize -= nRetCode;

        BYTE*           pbyExtInfo          = NULL;
        unsigned        uExtInfoDataLen     = 0;
        pbyExtInfo = (BYTE*)m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_piPackage->GetData();
        uExtInfoDataLen = (unsigned int)m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_uPackageUseSize;

        nRetCode = CheckRoleDataCRC(pbyExtInfo, uExtInfoDataLen);
        KGLOG_PROCESS_ERROR(nRetCode);

        ulLength = (unsigned long)uExtInfoDataLen;
        KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);

        nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)pbyExtInfo, ulLength);
        KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
        pszPos          += nRetCode;
        uLeftBufferSize -= nRetCode;

        nRetCode = snprintf(pszPos, uLeftBufferSize, "');");
        KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
        pszPos          += nRetCode;
        uLeftBufferSize -= nRetCode;
    }
    else
    {
        nRetCode = snprintf(pszPos, uLeftBufferSize, "', null);");
        KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
        pszPos          += nRetCode;
        uLeftBufferSize -= nRetCode;
    }

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

    nResult = true;
Exit0:
    return nResult;
}

int KSO3RoleDBTools::Init(KSO3RoleDBUpdater* pSO3RoleDBUpdater, KG_ROLEDBCONVERSION_CONFIG& refconfig)
{
    int  nResult    = false;
    BOOL bRetCode   = false;

    KGLOG_PROCESS_ERROR(m_pSO3RoleDBUpdater);

    m_pSO3RoleDBUpdater     = pSO3RoleDBUpdater;

    bRetCode = Connect(
        refconfig.szConnectToIP,
        refconfig.szDBName,
        refconfig.szDBAcc, 
        refconfig.szDBPsw
    );
    KGLOG_PROCESS_ERROR(bRetCode);
    
    nResult = true;
Exit0:
    return nResult;
}

int KSO3RoleDBTools::CheckRoleTable()
{
    int     nResult = false;
    BOOL    bRetCode    = 0;
    int     nStrLen     = 0;

    nStrLen = snprintf(m_szSQL, sizeof(m_szSQL), 
        "create table if not exists %s "
        "("
        "   %s bigint not null,"
        "   %s varchar(32) not null,"
        "   %s varchar(32) not null,"
        "   %s blob not null,"
        "   %s blob,"
        "   %s datetime not null,"
        "   primary key (%s), index (%s), unique (%s)"
        ");",
        ROLE_TABLE_NAME,            KG_ID_FIELD_NAME,         KG_NAME_FIELD_NAME, 
        KG_ACCOUNT_FIELD_NAME,      KG_BASE_INFO_FIELD_NAME,  KG_EXT_INFO_FIELD_NAME, 
        KG_LAST_MODIFY_FIELD_NAME,  KG_ID_FIELD_NAME,         KG_ACCOUNT_FIELD_NAME,    KG_NAME_FIELD_NAME
    );
    KGLOG_PROCESS_ERROR(nStrLen > 0 && nStrLen < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (!bRetCode)
    {
        KGLogPrintf(
            KGLOG_ERR, 
            "DB ERROR when try to create table \'%s\'\n", 
            ROLE_TABLE_NAME
        );
        goto Exit0;
    }

    nStrLen = snprintf(m_szSQL, sizeof(m_szSQL), 
        "create table if not exists %s "
        "("
        "   %s bigint not null auto_increment,"
        "   %s varchar(32) not null,"
        "   %s varchar(32) not null,"
        "   %s blob not null,"
        "   %s blob,"
        "   %s datetime not null,"
        "   primary key (%s), index (%s)"
        ");",
        RESTORE_ROLE_TABLE_NAME,    KG_ID_FIELD_NAME,         KG_NAME_FIELD_NAME, 
        KG_ACCOUNT_FIELD_NAME,      KG_BASE_INFO_FIELD_NAME,  KG_EXT_INFO_FIELD_NAME, 
        KG_DELETE_TIME_FIELD_NAME,  KG_ID_FIELD_NAME,         KG_ACCOUNT_FIELD_NAME
    );
    KGLOG_PROCESS_ERROR(nStrLen > 0 && nStrLen < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (!bRetCode)
    {
        KGLogPrintf(
            KGLOG_ERR, 
            "DB ERROR when try to create table \'%s\'\n", 
            RESTORE_ROLE_TABLE_NAME
        );
        goto Exit0;
    }

    nStrLen = snprintf(m_szSQL, sizeof(m_szSQL), 
        "create table if not exists %s "
        "("
        "   %s bigint not null auto_increment,"
        "   %s varchar(32) not null,"
        "   %s varchar(32) not null,"
        "   %s blob not null,"
        "   %s blob,"
        "   %s datetime not null,"
        "   primary key (%s), index (%s)"
        ");",
        DELETE_ROLE_TABLE_NAME,    KG_ID_FIELD_NAME,         KG_NAME_FIELD_NAME, 
        KG_ACCOUNT_FIELD_NAME,      KG_BASE_INFO_FIELD_NAME,  KG_EXT_INFO_FIELD_NAME, 
        KG_DELETE_TIME_FIELD_NAME,  KG_ID_FIELD_NAME,         KG_ACCOUNT_FIELD_NAME
        );
    KGLOG_PROCESS_ERROR(nStrLen > 0 && nStrLen < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (!bRetCode)
    {
        KGLogPrintf(
            KGLOG_ERR, 
            "DB ERROR when try to create table \'%s\'\n", 
            RESTORE_ROLE_TABLE_NAME
        );
        goto Exit0;
    }

    nResult = true;
Exit0:
    return nResult;
}

BOOL KSO3RoleDBTools::CheckMailTable()
{
    BOOL                bResult     = false;
    BOOL                bRetCode    = 0;
    int                 nStrLen     = 0;

    nStrLen = snprintf(m_szSQL, sizeof(m_szSQL), 
        "create table if not exists %s( "
        "   ID           bigint     not null, "
        "   BaseTime     datetime   not null, "
        "   MailBoxInfo  mediumblob not null, "
        "   primary key (ID), index (ID, BaseTime) "
        "); ",
        MAIL_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nStrLen > 0 && nStrLen < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (!bRetCode)
    {
        KGLogPrintf(
            KGLOG_ERR, 
            "DB ERROR when try to create table \'%s\'\n", 
            MAIL_TABLE_NAME
        );
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;
}

int KSO3RoleDBTools::InsertMailBoxDateToNewDB(DWORD dwID)
{
    int             nResult            = false;
    int             nRetCode            = false;

    char*           pszPos              = NULL;
    size_t          uLeftBufferSize     = 0;
    unsigned long   ulLength            = 0;
    my_ulonglong    lAffectedRow        = 0;
    BYTE*           pbyExtInfo          = NULL;
    unsigned        uExtInfoDataLen     = 0;

    uLeftBufferSize = MAX_ROLE_DB_SQL_SIZE;

    memset(m_szSQL,0, sizeof(m_szSQL));
    pszPos = m_szSQL;

    nRetCode = snprintf(
        pszPos, uLeftBufferSize, "insert into %s (%s, %s, %s) "
        "values (%u, '",
        MAIL_TABLE_NAME, KG_ID_FIELD_NAME, MAIL_BASE_TIME_NAME, KG_MAIL_BOX_INFO_FIELD_NAME ,
        dwID
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = (unsigned long)strlen(m_pSO3RoleDBUpdater->m_szMailBoxTime);
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, m_pSO3RoleDBUpdater->m_szMailBoxTime, ulLength);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    nRetCode = snprintf(pszPos, uLeftBufferSize, "', '");
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    pbyExtInfo = (BYTE*)m_pSO3RoleDBUpdater->m_piMailBoxInfoPackage->GetData();
    nRetCode = m_pSO3RoleDBUpdater->GetMailBoxInfoSize(uExtInfoDataLen);
    KGLOG_PROCESS_ERROR(nRetCode);

    ulLength = (unsigned long)uExtInfoDataLen;
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)pbyExtInfo, uExtInfoDataLen);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    nRetCode = snprintf(pszPos, uLeftBufferSize, "');");
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

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

    nResult = true;
Exit0:
    return nResult;
}

int KSO3RoleDBTools::GetRoleAllInfoToCompare(BYTE* byBaseInfoData, unsigned long ulBaseInfoSize, BYTE* byExtInfoData, unsigned long ulExtInfoSize, char szTableName[], char szTimeFileName[], DWORD dwID)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    int                         nRetCode        = false;
    MYSQL_RES*                  pQueryRes       = NULL;
    MYSQL_ROW                   QueryRow;

    nRetCode = snprintf(
        m_szSQL, sizeof(m_szSQL), "select %s, %s, %s, %s, %s, %s from %s where %s = %u", 
        KG_ID_FIELD_NAME,           KG_NAME_FIELD_NAME,     KG_ACCOUNT_FIELD_NAME, 
        KG_BASE_INFO_FIELD_NAME,    KG_EXT_INFO_FIELD_NAME, szTimeFileName, szTableName, KG_ID_FIELD_NAME, dwID
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    KGLOG_PROCESS_ERROR(bRetCode);

    pQueryRes = mysql_use_result(m_pDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);


    unsigned long*  pulLengths           = NULL;

    QueryRow = mysql_fetch_row(pQueryRes);
    if (QueryRow == NULL)
    {
        goto Exit1;
    }

    pulLengths = mysql_fetch_lengths(pQueryRes);
    KGLOG_PROCESS_ERROR(pulLengths);

    KGLOG_PROCESS_ERROR(pulLengths[1] < _NAME_LEN);
    KGLOG_PROCESS_ERROR(pulLengths[2] < _NAME_LEN);

    if (pulLengths[3] != sizeof(KBaseInfo_Old))
    {
        KGLogPrintf(KGLOG_INFO, "The RoleDBDate is not 11 Version, please Use The 11 version RoleDB!");
        KGLOG_PROCESS_ERROR(false);// 屏蔽转换一半继续转换的功能,如果转换失败在这,请要重新恢复数据库.
        m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_bHadUpdateBaseInfo = true;
    }

    ASSERT(QueryRow[0]);
    ASSERT(QueryRow[1]);
    ASSERT(QueryRow[2]);
    ASSERT(QueryRow[3]);
    ASSERT(QueryRow[5]);

    //比较KBaseInfo_Old
    if(pulLengths[3] != ulBaseInfoSize)
    {
        KGLogPrintf(KGLOG_INFO, "BaseInfoSize Error");
    }
    nRetCode = memcmp(QueryRow[3], byBaseInfoData, pulLengths[3]);
    if (nRetCode != 0)
    {
        KGLogPrintf(KGLOG_INFO, "BaseInfo Error");
    }
    if (QueryRow[4] != NULL)
    {
        if (pulLengths[4] != ulExtInfoSize)
        {
            KGLogPrintf(KGLOG_INFO, "ExtInfo Size Error");
        }
        nRetCode = memcmp(QueryRow[4], byExtInfoData, pulLengths[4]);
        if (nRetCode != 0)
        {
            KGLogPrintf(KGLOG_INFO, "ExtInfo Error");
        }
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

int KSO3RoleDBTools::LoadAndCompareTable(char szTableName[], char szTimeFileName[])
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    int                         nRetCode        = false;
    MYSQL_RES*                  pQueryRes       = NULL;
    DWORD                       dwRoleCount     = 0;
    KBaseInfo_Old*              pBaseInfo       = NULL;
    MYSQL_ROW                   QueryRow;

    ASSERT(m_pDBHandle);
    ASSERT(szTableName);

    KGLogPrintf(KGLOG_INFO, "[DB] %s Data loading ... ...", szTableName);

    nRetCode = snprintf(
        m_szSQL, sizeof(m_szSQL), "select %s, %s, %s, %s, %s, %s from %s", 
        KG_ID_FIELD_NAME,           KG_NAME_FIELD_NAME,     KG_ACCOUNT_FIELD_NAME, 
        KG_BASE_INFO_FIELD_NAME,    KG_EXT_INFO_FIELD_NAME, szTimeFileName, szTableName
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    KGLOG_PROCESS_ERROR(bRetCode);

    pQueryRes = mysql_use_result(m_pDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    KGLogPrintf(KGLOG_INFO, "Current [DB]'s RoleDBDate Compare! Please do not close the application !!!!.....");

    while (true)
    {
        DWORD           dwRoleID            = ERROR_ID;
        unsigned long*  pulLengths           = NULL;

        QueryRow = mysql_fetch_row(pQueryRes);
        if (QueryRow == NULL)
        {
            goto Exit1;
        }

        pulLengths = mysql_fetch_lengths(pQueryRes);
        KGLOG_PROCESS_ERROR(pulLengths);

        KGLOG_PROCESS_ERROR(pulLengths[1] < _NAME_LEN);
        KGLOG_PROCESS_ERROR(pulLengths[2] < _NAME_LEN);

        if (pulLengths[3] != sizeof(KBaseInfo_Old))
        {
            KGLogPrintf(KGLOG_INFO, "The RoleDBDate is not 11 Version, please Use The 11 version RoleDB!");
            KGLOG_PROCESS_ERROR(false);// 屏蔽转换一半继续转换的功能,如果转换失败在这,请要重新恢复数据库.
            m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_bHadUpdateBaseInfo = true;
        }

        ASSERT(QueryRow[0]);
        ASSERT(QueryRow[1]);
        ASSERT(QueryRow[2]);
        ASSERT(QueryRow[3]);
        ASSERT(QueryRow[5]);

        strncpy(m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_szRoleName, QueryRow[1], sizeof(m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_szRoleName));
        m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_szRoleName[sizeof(m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_szRoleName) - 1] = '\0';
        strncpy(m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_szAccountName, QueryRow[2], sizeof(m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_szAccountName));
        m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_szAccountName[sizeof(m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_szAccountName) - 1] = '\0';
        dwRoleID = (DWORD)strtoul(QueryRow[0], NULL, 10);

        pBaseInfo = (KBaseInfo_Old*)QueryRow[3];

        nRetCode = m_pSO3RoleDBUpdater->GetRoleAllInfoToCompare(
            (BYTE*)QueryRow[3], pulLengths[3],
            (BYTE*)QueryRow[4], pulLengths[4],
            szTableName, szTimeFileName, dwRoleID
        );
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = m_pSO3RoleDBUpdater->ResetRoleAllInfo();
        KGLOG_PROCESS_ERROR(nRetCode);

        ++dwRoleCount;
    }

Exit1:
    bResult = true;
Exit0:

    KGLogPrintf(KGLOG_INFO, "[DB] %s %u roles loaded !", szTableName, dwRoleCount);

    if (pQueryRes)
    {
        mysql_free_result(pQueryRes);
        pQueryRes = NULL;
    }
    return bResult;
}
