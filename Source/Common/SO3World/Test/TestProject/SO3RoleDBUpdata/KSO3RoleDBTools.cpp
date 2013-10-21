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
    BOOL                bResult         = false;
    KRoleDataHeader*    pRoleData       = NULL;
    DWORD               dwCRC           = 0;

    assert(pbyRoleData);

    KGLOG_PROCESS_ERROR(uDataLen > sizeof(KRoleDataHeader));
    pRoleData = (KRoleDataHeader*)pbyRoleData;

    dwCRC = CRC32(0, pbyRoleData + sizeof(KRoleDataHeader), (DWORD)(uDataLen - sizeof(KRoleDataHeader)));
    KGLOG_PROCESS_ERROR(dwCRC == pRoleData->dwCRC);

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

    KGLogPrintf(KGLOG_INFO, "Current [DB]'s RoleDBDate Version Conver to NewNew Version! Please do not close the application !!!!.....");

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
            //m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_bHadUpdateBaseInfo = true;
        }


        //if (pulLengths[3] != sizeof(KBaseInfo_Old))
        //{
        //    //KGLogPrintf(KGLOG_INFO, "The RoleDBDate is not 11 Version, please Use The 11 version RoleDB!");
        //    //KGLOG_PROCESS_ERROR(false);// 屏蔽转换一半继续转换的功能,如果转换失败在这,请要重新恢复数据库.
        //    m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_bHadUpdateBaseInfo = true;
        //}

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

    ulLength = (unsigned long)sizeof(KRoleBaseInfo);
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)&(m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_RoleBaseInfo), ulLength);
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
    unsigned long               ulDataSize      = 0;
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
    if (bRetCode == 0)
    {
        KGLogPrintf(KGLOG_INFO, "The Reader DB\'s Table %s doesn\'t exist ", MAIL_TABLE_NAME);
        goto Exit1;
    }

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

int KSO3RoleDBTools::InsertRoleDataToNewDB(char szTableName[], char szTimeFileName[])
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

    //ulLength = (unsigned long)sizeof(KBaseInfo);
    ulLength = (unsigned long)sizeof(KRoleBaseInfo);
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)&m_pSO3RoleDBUpdater->m_SO3RoleAllInfo.m_RoleBaseInfo, ulLength);
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

    KGLOG_PROCESS_ERROR(pSO3RoleDBUpdater);

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
        "   %s mediumblob,"
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
        "   %s mediumblob,"
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
        "   %s mediumblob,"
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

int KSO3RoleDBTools::InsertMailBoxDataToNewDB(DWORD dwID)
{
    int             nResult             = false;
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

BOOL KSO3RoleDBTools::CheckPQTable()
{
    BOOL                bResult     = false;
    BOOL                bRetCode    = 0;
    int                 nStrLen     = 0;

    nStrLen = snprintf(m_szSQL, sizeof(m_szSQL), 
        "create table if not exists %s "
        "("
        "   ID integer unsigned not null, "
        "   Data blob not null, "
        "   primary key (ID), index (ID) "
        ");",
        PQ_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nStrLen > 0);
    KGLOG_PROCESS_ERROR(nStrLen < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (!bRetCode)
    {
        KGLogPrintf(
            KGLOG_ERR, 
            "DB ERROR when try to create table \'%s\'\n", 
            PQ_TABLE_NAME
            );
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;

}

BOOL KSO3RoleDBTools::CheckAuctionTable()
{
    BOOL                bResult     = false;
    BOOL                bRetCode    = 0;
    int                 nStrLen     = 0;

    nStrLen = snprintf(m_szSQL, sizeof(m_szSQL), 
        "create table if not exists %s( "
        "   ID              bigint      not null auto_increment, "
        "   Name            varchar(32) not null, "
        "   AucGenre        integer     not null, "
        "   AucSub          integer     not null, "
        "   RequireLevel    integer     not null, "
        "   Quality         integer     not null, "
        "   SellerName      varchar(32) not null, "
        "   Price           integer     not null, "
        "   BuyItNowPrice   integer     not null, "
        "   BidderID        bigint      not null, "
        "   CustodyCharges  integer     not null, "
        "   DurationTime    datetime    not null, "
        "   ItemData        blob        not null, "
        "   primary key (ID) "
        ");",
        AUCTION_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nStrLen > 0 && nStrLen < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (!bRetCode)
    {
        KGLogPrintf(
            KGLOG_ERR, 
            "DB ERROR when try to create table \'%s\'\n", 
            AUCTION_TABLE_NAME
        );
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KSO3RoleDBTools::CheckGlobalCustomDataTable()
{
    BOOL                bResult     = false;
    BOOL                bRetCode    = 0;
    int                 nStrLen     = 0;

    nStrLen = snprintf(m_szSQL, sizeof(m_szSQL), 
        "create table if not exists %s( "
        "   Name     varchar(32) not null, "
        "   Value    mediumblob  not null, "
        "   primary key (Name) "
        ");",
        GLOBAL_CUSTOM_DATA_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nStrLen > 0 && nStrLen < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (!bRetCode)
    {
        KGLogPrintf(
            KGLOG_ERR, 
            "DB ERROR when try to create table \'%s\'\n", 
            GLOBAL_CUSTOM_DATA_TABLE_NAME
        );
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KSO3RoleDBTools::CheckGlobalSystemValueTable()
{
    BOOL    beResult = false;
    BOOL    bRetCode = 0;
    int     nStrLen  = 0;

    nStrLen = snprintf(
        m_szSQL, sizeof(m_szSQL), 
        "create table if not exists %s( "
        "   Name      varchar(32)   not null, "
        "   Value     integer       not null, "
        "   primary key (Name) "
        ");",
        GLOBAL_SYSTEM_VALUE_TABLE_NAME
        );
    KGLOG_PROCESS_ERROR(nStrLen > 0 && nStrLen < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (!bRetCode)
    {
        KGLogPrintf(
            KGLOG_ERR, 
            "DB ERROR when try to create table \'%s\'\n", 
            GLOBAL_SYSTEM_VALUE_TABLE_NAME
            );
        goto Exit0;
    }

    beResult = true;
Exit0:
    return beResult;
}

BOOL KSO3RoleDBTools::CheckSequenceTable()
{
    BOOL                bResult     = false;
    BOOL                bRetCode    = 0;
    int                 nStrLen     = 0;

    nStrLen = snprintf(m_szSQL, sizeof(m_szSQL), 
        "create table if not exists %s( "
        "   Seqname     varchar(64) not null default '', "
        "   StartID     bigint(20) unsigned default null, "
        "   CurrentID   bigint(20) unsigned default null, "
        "   MaxID       bigint(20) unsigned default null, "
        "   GroupName   varchar(32) not null default '', "
        "   primary key (Seqname, GroupName) "
        ");",
        SEQUENCE_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nStrLen > 0 && nStrLen < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (!bRetCode)
    {
        KGLogPrintf(
            KGLOG_ERR, 
            "DB ERROR when try to create table \'%s\'\n", 
            SEQUENCE_TABLE_NAME
        );
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;
}


BOOL KSO3RoleDBTools::CheckRoleBlakListTable()
{
    BOOL                bResult     = false;
    BOOL                bRetCode    = 0;
    int                 nStrLen     = 0;

    nStrLen = snprintf(m_szSQL, sizeof(m_szSQL), 
        "create table if not exists %s "
        "("
        "   ID bigint not null,"
        "   FreezeTime datetime not null,"
        "   primary key (ID), index (ID)"
        ");",
        ROLE_BLACK_LIST_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nStrLen > 0);
    KGLOG_PROCESS_ERROR(nStrLen < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (!bRetCode)
    {
        KGLogPrintf(
            KGLOG_ERR, 
            "DB ERROR when try to create table \'%s\'\n", 
            ROLE_BLACK_LIST_TABLE_NAME
        );
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KSO3RoleDBTools::CheckMapCopyTable()
{
    BOOL                bResult     = false;
    BOOL                bRetCode    = 0;
    int                 nStrLen     = 0;

    nStrLen = snprintf(m_szSQL, sizeof(m_szSQL), 
        "create table if not exists %s( "
        "   MapID integer unsigned, "
        "   MapCopyIndex bigint, "
        "   SceneData blob, "
        "   CreateTime datetime not null, "
        "   LastModifyTime datetime not null, "
        "   primary key  (MapID, MapCopyIndex) "
        "); ",
        MAP_COPY_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nStrLen > 0 && nStrLen < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (!bRetCode)
    {
        KGLogPrintf(
            KGLOG_ERR, 
            "DB ERROR when try to create table \'%s\'\n", 
            MAP_COPY_TABLE_NAME
        );
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KSO3RoleDBTools::LoadAndInsertFellowshipTable()
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    int                         nRetCode        = false;
    MYSQL_RES*                  pQueryRes       = NULL;
    DWORD                       dwRoleCount     = 0;
    MYSQL_ROW                   QueryRow;

    ASSERT(m_pDBHandle);

    KGLogPrintf(KGLOG_INFO, "[DB] Fellowship Data loading ... ...");

    nRetCode = snprintf(
        m_szSQL, sizeof(m_szSQL), "select %s, %s from %s", 
        KG_PLAYER_ID_FIELD_NAME,    KG_FELLOWSHIP_DATA_FIELD_NAME, FELLOWSHIP_TABLE_NAME 
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (bRetCode == 0)
    {
        KGLogPrintf(KGLOG_INFO, "The Reader DB\'s Table %s doesn\'t exist ", FELLOWSHIP_TABLE_NAME);
        goto Exit1;
    }

    pQueryRes = mysql_use_result(m_pDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    KGLogPrintf(KGLOG_INFO, "Current [DB]'s Fellowship Copy! Please do not close the application !!!!.....");

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

        ASSERT(QueryRow[0]);
        ASSERT(QueryRow[1]);

        dwRoleID = (DWORD)strtoul(QueryRow[0], NULL, 10);

        nRetCode = m_pSO3RoleDBUpdater->InsertToNewDBFellowship(dwRoleID, (BYTE*)QueryRow[1], pulLengths[1]);
        KGLOG_PROCESS_ERROR(nRetCode);

        ++dwRoleCount;
    }

Exit1:
    bResult = true;
Exit0:

    KGLogPrintf(KGLOG_INFO, "[DB] %u Fellowship loaded !", dwRoleCount);

    if (pQueryRes)
    {
        mysql_free_result(pQueryRes);
        pQueryRes = NULL;
    }
    return bResult;
}

BOOL KSO3RoleDBTools::CheckFellowshipTable()
{
    BOOL                bResult     = false;
    BOOL                bRetCode    = 0;
    int                 nStrLen     = 0;

    nStrLen = snprintf(m_szSQL, sizeof(m_szSQL), 
        "create table if not exists "
        "    %s "
        "    ( "
        "        PlayerID               integer unsigned    not null, "
        "        FellowshipData         blob, "
        "        primary key (PlayerID) "
        "    ) "
        ";",
        FELLOWSHIP_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nStrLen > 0 && nStrLen < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (!bRetCode)
    {
        KGLogPrintf(
            KGLOG_ERR, 
            "DB ERROR when try to create table \'%s\'\n", 
            FELLOWSHIP_TABLE_NAME
        );
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KSO3RoleDBTools::CheckTongTable()
{
    BOOL                bResult     = false;
    BOOL                bRetCode    = 0;
    int                 nStrLen     = 0;

    nStrLen = snprintf(m_szSQL, sizeof(m_szSQL), 
        "create table if not exists %s "
        "("
        "   ID              bigint not null,"
        "   Data            blob,"
        "   primary         key (ID) "
        ");",
        TONG_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nStrLen > 0);
    KGLOG_PROCESS_ERROR(nStrLen < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (!bRetCode)
    {
        KGLogPrintf(
            KGLOG_ERR, 
            "DB ERROR when try to create table \'%s\'\n", 
            TONG_TABLE_NAME
        );
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KSO3RoleDBTools::CheckDatabase()
{
    BOOL    bResult     = false;
    int     nRetCode    = 0;

    nRetCode = CheckRoleTable();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = CheckFellowshipTable();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = CheckMapCopyTable();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = CheckRoleBlakListTable();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = CheckTongTable();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = CheckMailTable();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = CheckPQTable();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = CheckAuctionTable();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = CheckGlobalCustomDataTable();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = CheckGlobalSystemValueTable();//++
    KGLOG_PROCESS_ERROR(nRetCode);//++

    nRetCode = CheckGlobalMailTable();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = CheckSequenceTable();
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    return bResult;
}

int KSO3RoleDBTools::InsertFellowShipDataToNewDB(DWORD dwID, BYTE* pbyFellowshipData, unsigned long ulDataLen)
{
    int             nResult             = false;
    int             nRetCode            = false;

    char*           pszPos              = NULL;
    size_t          uLeftBufferSize     = 0;
    unsigned long   ulLength            = 0;
    my_ulonglong    lAffectedRow        = 0;

    uLeftBufferSize = MAX_ROLE_DB_SQL_SIZE;

    memset(m_szSQL,0, sizeof(m_szSQL));
    pszPos = m_szSQL;

    nRetCode = snprintf(
        pszPos, uLeftBufferSize, "insert into %s (%s, %s) "
        "values (%u, '",
        FELLOWSHIP_TABLE_NAME, KG_PLAYER_ID_FIELD_NAME, KG_FELLOWSHIP_DATA_FIELD_NAME,
        dwID
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = ulDataLen;
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)pbyFellowshipData, ulLength);
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

int KSO3RoleDBTools::InsertMapCopyDataToNewDB(BYTE* byMapID, unsigned long ulMapIDLen,
                                              BYTE* byMapCopyIndex, unsigned long ulMapCopyIndexLen,
                                              BYTE* bySceneData, unsigned long ulSceneDataLen,
                                              BYTE* byCreateTime, unsigned long ulCreateTimeLen,
                                              BYTE* byLastModify, unsigned long ulLastModifyLen
)
{
    int             nResult             = false;
    int             nRetCode            = false;

    char*           pszPos              = NULL;
    size_t          uLeftBufferSize     = 0;
    unsigned long   ulLength            = 0;
    my_ulonglong    lAffectedRow        = 0;

    uLeftBufferSize = MAX_ROLE_DB_SQL_SIZE;

    memset(m_szSQL,0, sizeof(m_szSQL));
    pszPos = m_szSQL;

    nRetCode = snprintf(
        pszPos, uLeftBufferSize, "insert into %s (%s, %s, %s, %s, %s) "
        "values ('",
        MAP_COPY_TABLE_NAME, KG_MAP_ID_FIELD_NAME, KG_MAP_COPY_INDEX_FIELD_NAME, KG_SCENE_DATA_FIELD_NAME,
        KG_CREATE_TIME_FIELD_NAME, KG_LAST_MODIFY_TIME_FILED_NAME
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = (unsigned long)ulMapIDLen;
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)byMapID, ulLength);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    nRetCode = snprintf(pszPos, uLeftBufferSize, "', '");
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = (unsigned long)ulMapCopyIndexLen;
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)byMapCopyIndex, ulLength);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    nRetCode = snprintf(pszPos, uLeftBufferSize, "', '");
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = (unsigned long)ulSceneDataLen;
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)bySceneData, ulLength);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    nRetCode = snprintf(pszPos, uLeftBufferSize, "', '");
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = (unsigned long)ulCreateTimeLen;
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)byCreateTime, ulLength);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    nRetCode = snprintf(pszPos, uLeftBufferSize, "', '");
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = (unsigned long)ulLastModifyLen;
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)byLastModify, ulLength);
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

BOOL KSO3RoleDBTools::LoadAndInsertMapCopyTable()
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    int                         nRetCode        = false;
    MYSQL_RES*                  pQueryRes       = NULL;
    DWORD                       dwRoleCount     = 0;
    MYSQL_ROW                   QueryRow;

    ASSERT(m_pDBHandle);

    KGLogPrintf(KGLOG_INFO, "[DB] MapCopy Data loading ... ...");

    nRetCode = snprintf(
        m_szSQL, sizeof(m_szSQL), "select %s, %s, %s, %s, %s from %s", 
        KG_MAP_ID_FIELD_NAME, KG_MAP_COPY_INDEX_FIELD_NAME, 
        KG_SCENE_DATA_FIELD_NAME, KG_CREATE_TIME_FIELD_NAME,
        KG_LAST_MODIFY_TIME_FILED_NAME,
        MAP_COPY_TABLE_NAME 
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (bRetCode == 0)
    {
        KGLogPrintf(KGLOG_INFO, "The Reader DB\'s Table %s doesn\'t exist ", MAP_COPY_TABLE_NAME);
        goto Exit1;
    }

    pQueryRes = mysql_use_result(m_pDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    KGLogPrintf(KGLOG_INFO, "Current [DB]'s MapCopy Copy! Please do not close the application !!!!.....");

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

        ASSERT(QueryRow[0]);
        ASSERT(QueryRow[1]);
        ASSERT(QueryRow[2]);
        ASSERT(QueryRow[3]);
        ASSERT(QueryRow[4]);

        dwRoleID = (DWORD)strtoul(QueryRow[0], NULL, 10);

        nRetCode = m_pSO3RoleDBUpdater->InsertToNewDBMapCopy((BYTE*)QueryRow[0], pulLengths[0],
                                                             (BYTE*)QueryRow[1], pulLengths[1],
                                                             (BYTE*)QueryRow[2], pulLengths[2],
                                                             (BYTE*)QueryRow[3], pulLengths[3],
                                                             (BYTE*)QueryRow[4], pulLengths[4]
        );
        KGLOG_PROCESS_ERROR(nRetCode);

        ++dwRoleCount;
    }

Exit1:
    bResult = true;
Exit0:

    KGLogPrintf(KGLOG_INFO, "[DB] %u MapCopy loaded !", dwRoleCount);

    if (pQueryRes)
    {
        mysql_free_result(pQueryRes);
        pQueryRes = NULL;
    }
    return bResult;
}

int KSO3RoleDBTools::InsertPQDataToNewDB(DWORD dwID, BYTE* pbyData, unsigned long ulDataLen)
{
    int             nResult             = false;
    int             nRetCode            = false;

    char*           pszPos              = NULL;
    size_t          uLeftBufferSize     = 0;
    unsigned long   ulLength            = 0;
    my_ulonglong    lAffectedRow        = 0;

    uLeftBufferSize = MAX_ROLE_DB_SQL_SIZE;

    memset(m_szSQL,0, sizeof(m_szSQL));
    pszPos = m_szSQL;

    nRetCode = snprintf(
        pszPos, uLeftBufferSize, "insert into %s (%s, %s) "
        "values (%u, '",
        PQ_TABLE_NAME, KG_ID_FIELD_NAME, KG_DATA_FILED_NAME,
        dwID
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = ulDataLen;
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)pbyData, ulLength);
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

BOOL KSO3RoleDBTools::LoadAndInsertPQTable()
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    int                         nRetCode        = false;
    MYSQL_RES*                  pQueryRes       = NULL;
    DWORD                       dwRoleCount     = 0;
    MYSQL_ROW                   QueryRow;

    ASSERT(m_pDBHandle);

    KGLogPrintf(KGLOG_INFO, "[DB] PQ Data loading ... ...");

    nRetCode = snprintf(
        m_szSQL, sizeof(m_szSQL), "select %s, %s from %s", 
        KG_ID_FIELD_NAME,    KG_DATA_FILED_NAME, PQ_TABLE_NAME 
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (bRetCode == 0)
    {
        KGLogPrintf(KGLOG_INFO, "The Reader DB\'s Table %s doesn\'t exist ", PQ_TABLE_NAME);
        goto Exit1;
    }

    pQueryRes = mysql_use_result(m_pDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    KGLogPrintf(KGLOG_INFO, "Current [DB]'s PQ Copy! Please do not close the application !!!!.....");

    while (true)
    {
        DWORD           dwRoleID            = ERROR_ID;
        unsigned long*  pulLengths          = NULL;

        QueryRow = mysql_fetch_row(pQueryRes);
        if (QueryRow == NULL)
        {
            goto Exit1;
        }

        pulLengths = mysql_fetch_lengths(pQueryRes);
        KGLOG_PROCESS_ERROR(pulLengths);

        ASSERT(QueryRow[0]);
        ASSERT(QueryRow[1]);

        dwRoleID = (DWORD)strtoul(QueryRow[0], NULL, 10);

        nRetCode = m_pSO3RoleDBUpdater->InsertToNewDBQP(dwRoleID, (BYTE*)QueryRow[1], pulLengths[1]);
        KGLOG_PROCESS_ERROR(nRetCode);

        ++dwRoleCount;
    }

Exit1:
    bResult = true;
Exit0:

    KGLogPrintf(KGLOG_INFO, "[DB] %u PQ loaded !", dwRoleCount);

    if (pQueryRes)
    {
        mysql_free_result(pQueryRes);
        pQueryRes = NULL;
    }
    return bResult;
}

BOOL KSO3RoleDBTools::LoadAndInsertRoleBlackListTable()
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    int                         nRetCode        = false;
    MYSQL_RES*                  pQueryRes       = NULL;
    DWORD                       dwRoleCount     = 0;
    MYSQL_ROW                   QueryRow;

    ASSERT(m_pDBHandle);

    KGLogPrintf(KGLOG_INFO, "[DB] RoleBlackList Data loading ... ...");

    nRetCode = snprintf(
        m_szSQL, sizeof(m_szSQL), "select %s, %s from %s", 
        KG_ID_FIELD_NAME,    KG_FREEZETIME_NAME_FILED_NAME, ROLE_BLACK_LIST_TABLE_NAME 
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (bRetCode == 0)
    {
        KGLogPrintf(KGLOG_INFO, "The Reader DB\'s Table %s doesn\'t exist ", ROLE_BLACK_LIST_TABLE_NAME);
        goto Exit1;
    }

    pQueryRes = mysql_use_result(m_pDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    KGLogPrintf(KGLOG_INFO, "Current [DB]'s RoleBlackList Copy! Please do not close the application !!!!.....");

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

        ASSERT(QueryRow[0]);
        ASSERT(QueryRow[1]);

        dwRoleID = (DWORD)strtoul(QueryRow[0], NULL, 10);

        nRetCode = m_pSO3RoleDBUpdater->InsertToNewDBRoleBlackList(dwRoleID, (BYTE*)QueryRow[1], pulLengths[1]);
        KGLOG_PROCESS_ERROR(nRetCode);

        ++dwRoleCount;
    }

Exit1:
    bResult = true;
Exit0:

    KGLogPrintf(KGLOG_INFO, "[DB] %u RoleBlackList loaded !", dwRoleCount);

    if (pQueryRes)
    {
        mysql_free_result(pQueryRes);
        pQueryRes = NULL;
    }
    return bResult;
}

int KSO3RoleDBTools::InsertRoleBlackListToNewDB(DWORD dwID, BYTE* pbyRoleBlackListData, unsigned long ulDataLen)
{
    int             nResult             = false;
    int             nRetCode            = false;

    char*           pszPos              = NULL;
    size_t          uLeftBufferSize     = 0;
    unsigned long   ulLength            = 0;
    my_ulonglong    lAffectedRow        = 0;

    uLeftBufferSize = MAX_ROLE_DB_SQL_SIZE;

    memset(m_szSQL,0, sizeof(m_szSQL));
    pszPos = m_szSQL;

    nRetCode = snprintf(
        pszPos, uLeftBufferSize, "insert into %s (%s, %s) "
        "values (%u, '",
        ROLE_BLACK_LIST_TABLE_NAME, KG_ID_FIELD_NAME, KG_FREEZETIME_NAME_FILED_NAME,
        dwID
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = ulDataLen;
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)pbyRoleBlackListData, ulLength);
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

BOOL KSO3RoleDBTools::LoadAndInsertSequenceTable()
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    int                         nRetCode        = false;
    MYSQL_RES*                  pQueryRes       = NULL;
    DWORD                       dwRoleCount     = 0;
    MYSQL_ROW                   QueryRow;

    ASSERT(m_pDBHandle);

    KGLogPrintf(KGLOG_INFO, "[DB] Sequence Data loading ... ...");

    nRetCode = snprintf(
        m_szSQL, sizeof(m_szSQL), "select %s, %s, %s, %s, %s from %s", 
        KG_SEQNAME_FILED_NAME, KG_START_ID_FILED_NAME, 
        KG_CURRENT_ID_FILED_NAME, KG_MAX_ID_FILED_NAME,
        KG_GROUP_NAME_FILED_NAME,
        SEQUENCE_TABLE_NAME 
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (bRetCode == 0)
    {
        KGLogPrintf(KGLOG_INFO, "The Reader DB\'s Table %s doesn\'t exist ", SEQUENCE_TABLE_NAME);
        goto Exit1;
    }

    pQueryRes = mysql_use_result(m_pDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    KGLogPrintf(KGLOG_INFO, "Current [DB]'s Sequence Copy! Please do not close the application !!!!.....");

    while (true)
    {
        DWORD           dwRoleID            = ERROR_ID;
        unsigned long*  pulLengths          = NULL;

        QueryRow = mysql_fetch_row(pQueryRes);
        if (QueryRow == NULL)
        {
            goto Exit1;
        }

        pulLengths = mysql_fetch_lengths(pQueryRes);
        KGLOG_PROCESS_ERROR(pulLengths);

        //这个表真的存在空....
        //ASSERT(QueryRow[0]);
        //ASSERT(QueryRow[1]);
        //ASSERT(QueryRow[2]);
        //ASSERT(QueryRow[3]);
        //ASSERT(QueryRow[4]);

        dwRoleID = (DWORD)strtoul(QueryRow[0], NULL, 10);

        nRetCode = m_pSO3RoleDBUpdater->InsertToNewDBSequence(
            (BYTE*)QueryRow[0], pulLengths[0],
            (BYTE*)QueryRow[1], pulLengths[1],
            (BYTE*)QueryRow[2], pulLengths[2],
            (BYTE*)QueryRow[3], pulLengths[3],
            (BYTE*)QueryRow[4], pulLengths[4]
        );
        KGLOG_PROCESS_ERROR(nRetCode);

        ++dwRoleCount;
    }

Exit1:
    bResult = true;
Exit0:

    KGLogPrintf(KGLOG_INFO, "[DB] %u Sequence loaded !", dwRoleCount);

    if (pQueryRes)
    {
        mysql_free_result(pQueryRes);
        pQueryRes = NULL;
    }
    return bResult;
}

int KSO3RoleDBTools::InsertSequenceDataToNewDB(BYTE* bySeqname, unsigned long ulSeqnameLen,
                                              BYTE* byStartid, unsigned long ulStaridLen,
                                              BYTE* byCurrentID, unsigned long ulCurrentIDLen,
                                              BYTE* byMaxID, unsigned long ulMaxIDLen,
                                              BYTE* byGroupName, unsigned long ulGroupName
)
{
    int             nResult             = false;
    int             nRetCode            = false;

    char*           pszPos              = NULL;
    size_t          uLeftBufferSize     = 0;
    unsigned long   ulLength            = 0;
    my_ulonglong    lAffectedRow        = 0;

    uLeftBufferSize = MAX_ROLE_DB_SQL_SIZE;

    memset(m_szSQL,0, sizeof(m_szSQL));
    pszPos = m_szSQL;

    nRetCode = snprintf(
        pszPos, uLeftBufferSize, "insert into %s (%s, %s, %s, %s, %s ) "
        "values ('",
        SEQUENCE_TABLE_NAME, 
        KG_SEQNAME_FILED_NAME,
        KG_START_ID_FILED_NAME,
        KG_CURRENT_ID_FILED_NAME,
        KG_MAX_ID_FILED_NAME, 
        KG_GROUP_NAME_FILED_NAME
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = ulSeqnameLen;
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)bySeqname, ulLength);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    nRetCode = snprintf(pszPos, uLeftBufferSize, "', ");
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    if(byStartid == NULL)
    {
        nRetCode = snprintf(pszPos, uLeftBufferSize, "NULL, '");
        KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
        pszPos          += nRetCode;
        uLeftBufferSize -= nRetCode;
    }
    else
    {
        nRetCode = snprintf(pszPos, uLeftBufferSize, "'");
        KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
        pszPos          += nRetCode;
        uLeftBufferSize -= nRetCode;

        ulLength = ulStaridLen;
        KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
        nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)byStartid, ulLength);
        KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
        pszPos          += nRetCode;
        uLeftBufferSize -= nRetCode;

        nRetCode = snprintf(pszPos, uLeftBufferSize, "', '");
        KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
        pszPos          += nRetCode;
        uLeftBufferSize -= nRetCode;
    }

    ulLength = ulCurrentIDLen;
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)byCurrentID, ulLength);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    nRetCode = snprintf(pszPos, uLeftBufferSize, "', ");
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    if (byMaxID == NULL)
    {
        nRetCode = snprintf(pszPos, uLeftBufferSize, " NULL, ");
        KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
        pszPos          += nRetCode;
        uLeftBufferSize -= nRetCode;
    }
    else
    {
        nRetCode = snprintf(pszPos, uLeftBufferSize, "'");
        KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
        pszPos          += nRetCode;
        uLeftBufferSize -= nRetCode;

        ulLength = ulMaxIDLen;
        KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
        nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)byMaxID, ulLength);
        KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
        pszPos          += nRetCode;
        uLeftBufferSize -= nRetCode;

        nRetCode = snprintf(pszPos, uLeftBufferSize, "', ");
        KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
        pszPos          += nRetCode;
        uLeftBufferSize -= nRetCode;
    }

    if (byGroupName == NULL)
    {
        nRetCode = snprintf(pszPos, uLeftBufferSize, "NULL ");
        KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
        pszPos          += nRetCode;
        uLeftBufferSize -= nRetCode;
    }
    else
    {
        nRetCode = snprintf(pszPos, uLeftBufferSize, "'");
        KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
        pszPos          += nRetCode;
        uLeftBufferSize -= nRetCode;

        ulLength = ulGroupName;
        KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
        nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)byGroupName, ulLength);
        KGLOG_PROCESS_ERROR(nRetCode >= 0 && nRetCode < (int)uLeftBufferSize);
        pszPos          += nRetCode;
        uLeftBufferSize -= nRetCode;

        nRetCode = snprintf(pszPos, uLeftBufferSize, "'");
        KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
        pszPos          += nRetCode;
        uLeftBufferSize -= nRetCode;
    }

    nRetCode = snprintf(pszPos, uLeftBufferSize, ");");
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

BOOL KSO3RoleDBTools::LoadAndInsertTongTable()
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    int                         nRetCode        = false;
    MYSQL_RES*                  pQueryRes       = NULL;
    DWORD                       dwRoleCount     = 0;
    MYSQL_ROW                   QueryRow;

    ASSERT(m_pDBHandle);

    KGLogPrintf(KGLOG_INFO, "[DB] Tong Data loading ... ...");

    nRetCode = snprintf(
        m_szSQL, sizeof(m_szSQL), "select %s, %s from %s", 
        KG_ID_FIELD_NAME,    KG_DATA_FILED_NAME, TONG_TABLE_NAME 
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (bRetCode == 0)
    {
        KGLogPrintf(KGLOG_INFO, "The Reader DB don not have %s Table ", TONG_TABLE_NAME);
        goto Exit1;
    }

    pQueryRes = mysql_use_result(m_pDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    KGLogPrintf(KGLOG_INFO, "Current [DB]'s Tong Copy! Please do not close the application !!!!.....");

    while (true)
    {
        DWORD           dwRoleID            = ERROR_ID;
        unsigned long*  pulLengths          = NULL;

        QueryRow = mysql_fetch_row(pQueryRes);
        if (QueryRow == NULL)
        {
            goto Exit1;
        }

        pulLengths = mysql_fetch_lengths(pQueryRes);
        KGLOG_PROCESS_ERROR(pulLengths);

        ASSERT(QueryRow[0]);
        ASSERT(QueryRow[1]);

        dwRoleID = (DWORD)strtoul(QueryRow[0], NULL, 10);

        nRetCode = m_pSO3RoleDBUpdater->InsertToNewDBTong(dwRoleID, (BYTE*)QueryRow[1], pulLengths[1]);
        KGLOG_PROCESS_ERROR(nRetCode);

        ++dwRoleCount;
    }

Exit1:
    bResult = true;
Exit0:

    KGLogPrintf(KGLOG_INFO, "[DB] %u Tong loaded !", dwRoleCount);

    if (pQueryRes)
    {
        mysql_free_result(pQueryRes);
        pQueryRes = NULL;
    }
    return bResult;
}

int KSO3RoleDBTools::InsertTongToNewDB(DWORD dwID, BYTE* byData, unsigned long ulDataLen)
{
    int             nResult             = false;
    int             nRetCode            = false;

    char*           pszPos              = NULL;
    size_t          uLeftBufferSize     = 0;
    unsigned long   ulLength            = 0;
    my_ulonglong    lAffectedRow        = 0;

    uLeftBufferSize = MAX_ROLE_DB_SQL_SIZE;

    memset(m_szSQL,0, sizeof(m_szSQL));
    pszPos = m_szSQL;

    nRetCode = snprintf(
        pszPos, uLeftBufferSize, "insert into %s (%s, %s) "
        "values (%u, '",
        TONG_TABLE_NAME, KG_ID_FIELD_NAME, KG_DATA_FILED_NAME,
        dwID
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = ulDataLen;
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)byData, ulLength);
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

BOOL KSO3RoleDBTools::LoadAndInsertAuctionTable()
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    int                         nRetCode        = false;
    MYSQL_RES*                  pQueryRes       = NULL;
    DWORD                       dwRoleCount     = 0;
    MYSQL_ROW                   QueryRow;

    ASSERT(m_pDBHandle);

    KGLogPrintf(KGLOG_INFO, "[DB] Auction Data loading ... ...");

    nRetCode = snprintf(
        m_szSQL, sizeof(m_szSQL), 
        "select %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s from %s", 
        KG_ID_FIELD_NAME,
        KG_NAME_FILED_NAME,
        KG_AUC_GENRE_FILED_NAME,      
        KG_AUC_SUB_FILED_NAME,        
        KG_REQUIRE_LEVEL_FILED_NAME,
        KG_QUALITY_FILED_NAME,    
        KG_SELLER_NAME_FILED_NAME,
        KG_PRICE_FILED_NAME,    
        KG_BUY_IT_NOW_PRICE_FILED_NAME,
        KG_BIDDER_ID_FILED_NAME,
        KG_CUSTODY_CHARGES_FILED_NAME,
        KG_DURATION_TIME_FILED_NAME,
        KG_ITEM_DATE_FILED_NAME,  
        AUCTION_TABLE_NAME 
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (bRetCode == 0)
    {
        KGLogPrintf(KGLOG_INFO, "The Reader DB\'s Table %s doesn\'t exist ", AUCTION_TABLE_NAME);
        goto Exit1;
    }

    pQueryRes = mysql_use_result(m_pDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    KGLogPrintf(KGLOG_INFO, "Current [DB]'s Auction Copy! Please do not close the application !!!!.....");

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

        ASSERT(QueryRow[0]);
        ASSERT(QueryRow[1]);
        ASSERT(QueryRow[2]);
        ASSERT(QueryRow[3]);
        ASSERT(QueryRow[4]);
        ASSERT(QueryRow[5]);
        ASSERT(QueryRow[6]);
        ASSERT(QueryRow[7]);
        ASSERT(QueryRow[8]);
        ASSERT(QueryRow[9]);
        ASSERT(QueryRow[10]);
        ASSERT(QueryRow[11]);
        ASSERT(QueryRow[12]);

        dwRoleID = (DWORD)strtoul(QueryRow[0], NULL, 10);

        // 处理物品
        BOOL bIsCommonItemData  = 0;
        size_t uTempSize        = 0;
        BYTE*  byTemp           = NULL;
        nRetCode = m_pSO3RoleDBUpdater->PublicItemConver((BYTE*)QueryRow[12], pulLengths[12], &uTempSize, &bIsCommonItemData);
        if (bIsCommonItemData)
        {
            byTemp = (BYTE*)(&m_pSO3RoleDBUpdater->m_commonEquipData);
        }
        else
        {
            byTemp = (BYTE*)(&m_pSO3RoleDBUpdater->m_customEquipData);
        }
        // end处理物品
        nRetCode = m_pSO3RoleDBUpdater->InsertToNewDBAuction(
            (BYTE*)QueryRow[0], pulLengths[0],
            (BYTE*)QueryRow[1], pulLengths[1],
            (BYTE*)QueryRow[2], pulLengths[2],
            (BYTE*)QueryRow[3], pulLengths[3],
            (BYTE*)QueryRow[4], pulLengths[4],
            (BYTE*)QueryRow[5], pulLengths[5],
            (BYTE*)QueryRow[6], pulLengths[6],
            (BYTE*)QueryRow[7], pulLengths[7],
            (BYTE*)QueryRow[8], pulLengths[8],
            (BYTE*)QueryRow[9], pulLengths[9],
            (BYTE*)QueryRow[10], pulLengths[10],
            (BYTE*)QueryRow[11], pulLengths[11],
            byTemp, (unsigned long)uTempSize
        );
        KGLOG_PROCESS_ERROR(nRetCode);

        ++dwRoleCount;
    }

Exit1:
    bResult = true;
Exit0:

    KGLogPrintf(KGLOG_INFO, "[DB] %u Auction loaded !", dwRoleCount);

    if (pQueryRes)
    {
        mysql_free_result(pQueryRes);
        pQueryRes = NULL;
    }
    return bResult;
}

int KSO3RoleDBTools::InsertToNewDBAuction(
    BYTE* pbyData0, unsigned long ulDataLen0,
    BYTE* pbyData1, unsigned long ulDataLen1,
    BYTE* pbyData2, unsigned long ulDataLen2,
    BYTE* pbyData3, unsigned long ulDataLen3,
    BYTE* pbyData4, unsigned long ulDataLen4,
    BYTE* pbyData5, unsigned long ulDataLen5,
    BYTE* pbyData6, unsigned long ulDataLen6,
    BYTE* pbyData7, unsigned long ulDataLen7,
    BYTE* pbyData8, unsigned long ulDataLen8,
    BYTE* pbyData9, unsigned long ulDataLen9,
    BYTE* pbyData10, unsigned long ulDataLen10,
    BYTE* pbyData11, unsigned long ulDataLen11,
    BYTE* pbyData12, unsigned long ulDataLen12
)
{
    int             nResult             = false;
    int             nRetCode            = false;

    char*           pszPos              = NULL;
    size_t          uLeftBufferSize     = 0;
    unsigned long   ulLength            = 0;
    my_ulonglong    lAffectedRow        = 0;

    uLeftBufferSize = MAX_ROLE_DB_SQL_SIZE;

    memset(m_szSQL,0, sizeof(m_szSQL));
    pszPos = m_szSQL;

    nRetCode = snprintf(
        pszPos, uLeftBufferSize, "insert into %s (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s ) "
        "values ('",
        AUCTION_TABLE_NAME,
        KG_ID_FIELD_NAME,
        KG_NAME_FILED_NAME,
        KG_AUC_GENRE_FILED_NAME,
        KG_AUC_SUB_FILED_NAME,
        KG_REQUIRE_LEVEL_FILED_NAME, 
        KG_QUALITY_FILED_NAME,
        KG_SELLER_NAME_FILED_NAME,
        KG_PRICE_FILED_NAME,
        KG_BUY_IT_NOW_PRICE_FILED_NAME,
        KG_BIDDER_ID_FILED_NAME,
        KG_CUSTODY_CHARGES_FILED_NAME,
        KG_DURATION_TIME_FILED_NAME,
        KG_ITEM_DATE_FILED_NAME
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = ulDataLen0;
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)pbyData0, ulLength);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    nRetCode = snprintf(pszPos, uLeftBufferSize, "', '");
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = ulDataLen1;
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)pbyData1, ulLength);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    nRetCode = snprintf(pszPos, uLeftBufferSize, "', '");
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = ulDataLen2;
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)pbyData2, ulLength);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    nRetCode = snprintf(pszPos, uLeftBufferSize, "', '");
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = ulDataLen3;
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)pbyData3, ulLength);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    nRetCode = snprintf(pszPos, uLeftBufferSize, "', '");
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = ulDataLen4;
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)pbyData4, ulLength);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    nRetCode = snprintf(pszPos, uLeftBufferSize, "', '");
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = ulDataLen5;
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)pbyData5, ulLength);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    nRetCode = snprintf(pszPos, uLeftBufferSize, "', '");
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = ulDataLen6;
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)pbyData6, ulLength);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    nRetCode = snprintf(pszPos, uLeftBufferSize, "', '");
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = ulDataLen7;
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)pbyData7, ulLength);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    nRetCode = snprintf(pszPos, uLeftBufferSize, "', '");
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = ulDataLen8;
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)pbyData8, ulLength);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    nRetCode = snprintf(pszPos, uLeftBufferSize, "', '");
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = ulDataLen9;
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)pbyData9, ulLength);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    nRetCode = snprintf(pszPos, uLeftBufferSize, "', '");
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = ulDataLen10;
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)pbyData10, ulLength);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    nRetCode = snprintf(pszPos, uLeftBufferSize, "', '");
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = ulDataLen11;
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)pbyData11, ulLength);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    nRetCode = snprintf(pszPos, uLeftBufferSize, "', '");
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = ulDataLen12;
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)pbyData12, ulLength);
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

BOOL KSO3RoleDBTools::LoadAndInsertGlobalCustomDataTable()
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    int                         nRetCode        = false;
    MYSQL_RES*                  pQueryRes       = NULL;
    DWORD                       dwRoleCount     = 0;
    MYSQL_ROW                   QueryRow;

    ASSERT(m_pDBHandle);

    KGLogPrintf(KGLOG_INFO, "[DB] GlobalCustomDataTable Data loading ... ...");

    nRetCode = snprintf(
        m_szSQL, sizeof(m_szSQL), "select %s, %s from %s", 
        KG_NAME_FILED_NAME,
        KG_VALEV_FILED_NAME,
        GLOBAL_CUSTOM_DATA_TABLE_NAME 
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (bRetCode == 0)
    {
        KGLogPrintf(KGLOG_INFO, "The Reader DB\'s Table GlobalCustomDataTable doesn\'t exist ");
        goto Exit1;
    }

    pQueryRes = mysql_use_result(m_pDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    KGLogPrintf(KGLOG_INFO, "Current [DB]'s GlobalCustomDataTable Copy! Please do not close the application !!!!.....");

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

        ASSERT(QueryRow[0]);
        ASSERT(QueryRow[1]);

        dwRoleID = (DWORD)strtoul(QueryRow[0], NULL, 10);

        nRetCode = m_pSO3RoleDBUpdater->InsertToNewDBGlobalCustomData(
            (BYTE*)QueryRow[0], pulLengths[0],
            (BYTE*)QueryRow[1], pulLengths[1]
        );
        KGLOG_PROCESS_ERROR(nRetCode);

        ++dwRoleCount;
    }

Exit1:
    bResult = true;
Exit0:

    KGLogPrintf(KGLOG_INFO, "[DB] %u GlobalCustomDataTable loaded !", dwRoleCount);

    if (pQueryRes)
    {
        mysql_free_result(pQueryRes);
        pQueryRes = NULL;
    }
    return bResult;
}

int KSO3RoleDBTools::InsertToNewDBGlobalCustomData(
    BYTE* byData0, unsigned long ulDataLen0,
    BYTE* byData1, unsigned long ulDataLen1
)
{
    int             nResult             = false;
    int             nRetCode            = false;

    char*           pszPos              = NULL;
    size_t          uLeftBufferSize     = 0;
    unsigned long   ulLength            = 0;
    my_ulonglong    lAffectedRow        = 0;

    uLeftBufferSize = MAX_ROLE_DB_SQL_SIZE;

    memset(m_szSQL,0, sizeof(m_szSQL));
    pszPos = m_szSQL;

    nRetCode = snprintf(
        pszPos, uLeftBufferSize, "insert into %s (%s, %s) "
        "values ('",
        GLOBAL_CUSTOM_DATA_TABLE_NAME,
        KG_NAME_FILED_NAME,
        KG_VALEV_FILED_NAME
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = ulDataLen0;
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)byData0, ulLength);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    nRetCode = snprintf(pszPos, uLeftBufferSize, "', '");
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = ulDataLen1;
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)byData1, ulLength);
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

BOOL KSO3RoleDBTools::CheckGlobalMailTable()
{
    BOOL                bResult     = false;
    BOOL                bRetCode    = 0;
    int                 nStrLen     = 0;

    nStrLen = snprintf(m_szSQL, sizeof(m_szSQL), 
        "create table if not exists %s( "
        "   MailIndex    bigint     not null, "
        "   EndTime      datetime   not null, "
        "   MailInfo     mediumblob not null, "
        "   primary key (MailIndex) "
        "); ",
        GLOBAL_MAIL_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nStrLen > 0 && nStrLen < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (!bRetCode)
    {
        KGLogPrintf(
            KGLOG_ERR, 
            "DB ERROR when try to create table \'%s\'\n", 
            GLOBAL_MAIL_TABLE_NAME
            );
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KSO3RoleDBTools::LoadAndInsertGlobalMailTable()
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    int                         nRetCode        = false;
    MYSQL_RES*                  pQueryRes       = NULL;
    DWORD                       dwRoleCount     = 0;
    MYSQL_ROW                   QueryRow;

    ASSERT(m_pDBHandle);

    KGLogPrintf(KGLOG_INFO, "[DB] GlobalMail Data loading ... ...");

    nRetCode = snprintf(
        m_szSQL, sizeof(m_szSQL), 
        "select %s, %s, %s from %s", 
        KG_MAILE_INDEX_FILED_NAME,
        KG_END_TIME_FILED_NAME,
        KG_MAIL_INFO_FILED_NAME, 
        GLOBAL_MAIL_TABLE_NAME 
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (bRetCode == 0)
    {
        KGLogPrintf(KGLOG_INFO, "The Reader DB\'s Table %s doesn\'t exist ", GLOBAL_MAIL_TABLE_NAME);
        goto Exit1;
    }

    pQueryRes = mysql_use_result(m_pDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    KGLogPrintf(KGLOG_INFO, "Current [DB]'s GlobalMail Copy! Please do not close the application !!!!.....");

    while (true)
    {
        DWORD           dwRoleID            = ERROR_ID;
        unsigned long*  pulLengths          = NULL;

        QueryRow = mysql_fetch_row(pQueryRes);
        if (QueryRow == NULL)
        {
            goto Exit1;
        }

        pulLengths = mysql_fetch_lengths(pQueryRes);
        KGLOG_PROCESS_ERROR(pulLengths);

        ASSERT(QueryRow[0]);
        ASSERT(QueryRow[1]);
        ASSERT(QueryRow[2]);

        dwRoleID = (DWORD)strtoul(QueryRow[0], NULL, 10);
        // 改:
        BYTE*           pbyData     = (BYTE*)QueryRow[1];
        unsigned long   ulDataSize  = pulLengths[1];
        unsigned        uExtDataSize = 0;
        nRetCode = m_pSO3RoleDBUpdater->ConverMailBoxInfo(dwRoleID, pbyData, ulDataSize);
        KGLOG_PROCESS_ERROR(nRetCode);
        nRetCode = m_pSO3RoleDBUpdater->GetMailBoxInfoSize(uExtDataSize);
        KGLOG_PROCESS_ERROR(nRetCode);
        nRetCode = m_pSO3RoleDBUpdater->InsertToNewDBGlobalMail(
            (BYTE*)QueryRow[0], pulLengths[0],
            (BYTE*)QueryRow[1], pulLengths[1],
            (BYTE*)m_pSO3RoleDBUpdater->m_piMailBoxInfoPackage->GetData(), uExtDataSize
        );
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = m_pSO3RoleDBUpdater->ResetMailBoxInfo();
        KGLOG_PROCESS_ERROR(nRetCode);

        // end改;

        //nRetCode = m_pSO3RoleDBUpdater->InsertToNewDBGlobalMail(
        //    (BYTE*)QueryRow[0], pulLengths[0],
        //    (BYTE*)QueryRow[1], pulLengths[1],
        //    (BYTE*)QueryRow[2], pulLengths[2]
        //);
        //KGLOG_PROCESS_ERROR(nRetCode);

        ++dwRoleCount;
    }

Exit1:
    bResult = true;
Exit0:

    KGLogPrintf(KGLOG_INFO, "[DB] %u GlobalMail loaded !", dwRoleCount);

    if (pQueryRes)
    {
        mysql_free_result(pQueryRes);
        pQueryRes = NULL;
    }
    return bResult;
}

int KSO3RoleDBTools::InsertToNewDBGlobalMail(
    BYTE* byData0, unsigned long ulDataLen0,
    BYTE* byData1, unsigned long ulDataLen1,
    BYTE* byData2, unsigned long ulDataLen2
)
{
    int             nResult             = false;
    int             nRetCode            = false;

    char*           pszPos              = NULL;
    size_t          uLeftBufferSize     = 0;
    unsigned long   ulLength            = 0;
    my_ulonglong    lAffectedRow        = 0;

    uLeftBufferSize = MAX_ROLE_DB_SQL_SIZE;

    memset(m_szSQL,0, sizeof(m_szSQL));
    pszPos = m_szSQL;

    nRetCode = snprintf(
        pszPos, uLeftBufferSize, "insert into %s (%s, %s, %s ) "
        "values ('",
        GLOBAL_MAIL_TABLE_NAME,
        KG_MAILE_INDEX_FILED_NAME,
        KG_END_TIME_FILED_NAME,
        KG_MAIL_INFO_FILED_NAME
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = ulDataLen0;
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)byData0, ulLength);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    nRetCode = snprintf(pszPos, uLeftBufferSize, "', '");
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = ulDataLen1;
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)byData1, ulLength);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    nRetCode = snprintf(pszPos, uLeftBufferSize, "', '");
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = ulDataLen2;
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)byData2, ulLength);
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
