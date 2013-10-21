#include "stdafx.h"

#include <time.h>
#include <string>
#include "mysql.h"
#include "errmsg.h"
#include "KGPublic.h"
#include "KTestDBManager.h"
#include "KRoleDB.h"
#include "kgamecenterwrapper.h"
#include "KDBBase.h"

KTestDBManager g_DBManager;

#define KG_ROLE_TABLE_NAME "role"

KTestDBManager::KTestDBManager()
{
    m_pDBHandle         = NULL;
}

KTestDBManager::~KTestDBManager()
{
	if (m_pDBHandle)
	{
		DeleteDB(GCSETTING->m_szDBName);
		mysql_close(m_pDBHandle);
		m_pDBHandle = NULL;
	}
}

BOOL KTestDBManager::Init()
{
    BOOL          bResult       = false;
    int           nRetCode      = false;
    MYSQL        *pTmpHandle    = NULL; 

    ASSERT(!m_pDBHandle);


    m_pDBHandle = mysql_init(NULL);

    pTmpHandle = mysql_real_connect(
		m_pDBHandle, GCSETTING->m_szDBIP, GCSETTING->m_szDBAcc, 
		GCSETTING->m_szDBPsw, NULL, 3306, NULL, 0);
    if (!pTmpHandle)
    {

        KGLogPrintf(
            KGLOG_ERR, 
            "[KPlayerDataLoader] Can't open database \"%s\" at \"%s\", db error : \"%s\"\n", 
            GCSETTING->m_szDBIP, "", mysql_error(m_pDBHandle)
        );
        goto Exit0;
    }

	_CheckCharacterSet();

    nRetCode = DeleteDB(GCSETTING->m_szDBName);
   KG_PROCESS_ERROR(nRetCode);

	sprintf(m_szSQL, "create database %s", GCSETTING->m_szDBName);
	nRetCode = _DoQuey(m_szSQL, strlen(m_szSQL));
	KG_PROCESS_ERROR(nRetCode);
	
	mysql_select_db(m_pDBHandle, GCSETTING->m_szDBName);
    bResult = TRUE;

	
Exit0:
    return bResult;
}


BOOL KTestDBManager::_CheckCaseInsensitive()
{
    BOOL        bResult         = false;
    int         nRetCode        = false;
    char        szSQL[]         = "show variables like \'lower_case_table_names\'";
    MYSQL_RES*  pResultSet      = NULL;
    MYSQL_ROW   Row             = NULL;
    unsigned    uFieldCount     = 0;

    KG_ASSERT_EXIT(m_pDBHandle);

    nRetCode = _DoQuey(szSQL, sizeof(szSQL) - 1);
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
        KGLogPrintf(KGLOG_ERR, "[KASynchronousDB] MySQL config error, Please set MySQL variables: \"lower_case_table_names\" = 1\n");
        goto Exit0;
    }

    bResult = TRUE;
Exit0:
    if (pResultSet)
    {
        mysql_free_result(pResultSet);
        pResultSet = NULL;
    }
    return bResult;
}

BOOL KTestDBManager::_CheckCharacterSet()
{
    BOOL        bResult         = false;
    int         nRetCode        = false;
    char        szSQL[]         = "show variables like \'character_set_database\'";
    MYSQL_RES*  pResultSet      = NULL;
    MYSQL_ROW   Row             = NULL;
    unsigned    uFieldCount     = 0;

    // get database character set
    KG_ASSERT_EXIT(m_pDBHandle);

    nRetCode = _DoQuey(szSQL, sizeof(szSQL) - 1);
    KGLOG_PROCESS_ERROR(nRetCode);

    pResultSet = mysql_store_result(m_pDBHandle);
    KGLOG_PROCESS_ERROR(pResultSet);

    uFieldCount = mysql_num_fields(pResultSet);    
    KGLOG_PROCESS_ERROR(uFieldCount == 2);

    Row = mysql_fetch_row(pResultSet);
    KGLOG_PROCESS_ERROR(Row);

    KG_ASSERT_EXIT(Row[1]);
    nRetCode = mysql_set_character_set(m_pDBHandle, Row[1]);    
    KGLOG_PROCESS_ERROR(nRetCode == 0);

    bResult = TRUE;
Exit0:
    if (pResultSet)
    {
        mysql_free_result(pResultSet);
        pResultSet = NULL;
    }
    return bResult;
}

BOOL KTestDBManager::_CheckTables()
{
    BOOL        bResult     = false;
    int         nRetCode    = 0;
    MYSQL_RES*  pTableSet   = NULL;
    MYSQL_ROW   Row         = NULL;
    typedef struct
    {
	    BOOL operator()(const string& X, const string& Y) const
	    {
            return STR_CASE_CMP((const unsigned char *)X.c_str(), (const unsigned char *)Y.c_str()) < 0;
	    }
    } TABLE_NAME_STRICMP_LESS;
    typedef set<string, TABLE_NAME_STRICMP_LESS> TABLE_SET;
    TABLE_SET           TableSet;
    TABLE_SET::iterator it;

    KG_ASSERT_EXIT(m_pDBHandle);

    pTableSet = mysql_list_tables(m_pDBHandle, NULL);
    KGLOG_PROCESS_ERROR(pTableSet);
    
    // load information for each table
    while (TRUE)
    {
        Row = mysql_fetch_row(pTableSet);
        if (!Row)
            break;
        TableSet.insert(Row[0]);
    }

    bResult = TRUE;
Exit0:
    if (pTableSet)
    {
        mysql_free_result(pTableSet);
        pTableSet = NULL;
    }
    return bResult;
}


BOOL KTestDBManager::_DoQuey(const char cszSQL[], unsigned long ulLength)
{
    BOOL        bResult             = false;
    int         nRetCode            = false;
    int         nReconnectTimes     = 0;
    unsigned    uMySQLErrorCode     = 0;

    KG_ASSERT_EXIT(cszSQL);

    while (TRUE) 
    {
        nRetCode = mysql_real_query(m_pDBHandle, cszSQL, ulLength);
        if (nRetCode == 0)
            break;

        uMySQLErrorCode = mysql_errno(m_pDBHandle);
        if (
            (uMySQLErrorCode != CR_SERVER_LOST) &&
            (uMySQLErrorCode != CR_CONN_HOST_ERROR) &&
            (uMySQLErrorCode != CR_CONN_HOST_ERROR) &&
            (uMySQLErrorCode != CR_SERVER_GONE_ERROR)
        )
        {
            KGLogPrintf(                                                        
                KGLOG_ERR,                                                    
                "[KASynchronousDB] Call \"mysql_real_query\" failed : %s\n",
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
                "[KASynchronousDB] Call \"mysql_ping\" failed : %s\n",
                mysql_error(m_pDBHandle)
            );  
            goto Exit0;
        }
    }

//Exit1:
    bResult = TRUE;
Exit0:
    return bResult; 
}

BOOL KTestDBManager::_CheckDB()
{
	BOOL        bResult     = false;
	int         nRetCode    = 0;
	MYSQL_RES*  pDBSet   = NULL;
	MYSQL_ROW   Row         = NULL;
	typedef struct
	{
		BOOL operator()(const string& X, const string& Y) const
		{
			return STR_CASE_CMP((const unsigned char *)X.c_str(), (const unsigned char *)Y.c_str()) < 0;
		}
	} TABLE_NAME_STRICMP_LESS;
	typedef set<string, TABLE_NAME_STRICMP_LESS> TABLE_SET;
	TABLE_SET           TableSet;
	TABLE_SET::iterator it;

	KG_ASSERT_EXIT(m_pDBHandle);

	pDBSet = mysql_list_dbs(m_pDBHandle, NULL);
	KGLOG_PROCESS_ERROR(pDBSet);

	// load information for each table
	while (TRUE)
	{
		Row = mysql_fetch_row(pDBSet);
		if (!Row)
			break;
		TableSet.insert(Row[0]);
	}

	it = TableSet.find((string)KGameCenterWrapper::GCSettings()->m_szDBName);
	if (it == TableSet.end())
	{
		// 创建数据库
		char szSQL[1024];
		int  nSQLLen = 0;
		nSQLLen = snprintf(szSQL, sizeof(szSQL), 
			"CREATE DATABASE %s",
			KGameCenterWrapper::GCSettings()->m_szDBName
			);

		nRetCode = mysql_real_query(m_pDBHandle, szSQL, nSQLLen);
		if (nRetCode)
		{
			KGLogPrintf(
				KGLOG_ERR, 
				"DB ERROR when try to create database \'%s\': %s", 
				KGameCenterWrapper::GCSettings()->m_szDBName, mysql_error(m_pDBHandle)
				);
			goto Exit0;
		}

	}

	nRetCode = mysql_select_db(m_pDBHandle, KGameCenterWrapper::GCSettings()->m_szDBName);
	if (nRetCode)
	{
		KGLogPrintf(
			KGLOG_ERR, 
			"DB ERROR when try to select database \'%s\': %s", 
			KGameCenterWrapper::GCSettings()->m_szDBName, mysql_error(m_pDBHandle)
			);
		goto Exit0;
	}

	bResult = TRUE;
Exit0:
	return bResult; 
}

BOOL KTestDBManager::ClearTable(const char* szTableName)
{
	BOOL        bResult     = false;
	int         nRetCode    = 0;

	KG_ASSERT_EXIT(m_pDBHandle);

	char szSQL[1024];
	int  nSQLLen = 0;
	nSQLLen = snprintf(szSQL, sizeof(szSQL), 
		"truncate table %s",
		szTableName
		);

	nRetCode = mysql_real_query(m_pDBHandle, szSQL, nSQLLen);
	if (nRetCode)
	{
		KGLogPrintf(
			KGLOG_ERR, 
			"DB ERROR when try to truncate table \'%s\': %s", 
			szTableName, mysql_error(m_pDBHandle)
			);
		goto Exit0;
	}

	bResult = TRUE;
Exit0:
	return bResult; 
}

BOOL KTestDBManager::DeleteDB(const char* szDBName)
{
	BOOL        bResult     = false;
	int         nRetCode    = 0;

	KG_ASSERT_EXIT(m_pDBHandle);

	char szSQL[1024];
	int  nSQLLen = 0;
	nSQLLen = snprintf(szSQL, sizeof(szSQL), 
		"DROP DATABASE if exists %s",
		szDBName
		);

	nRetCode = mysql_real_query(m_pDBHandle, szSQL, nSQLLen);
	if (nRetCode)
	{
		KGLogPrintf(
			KGLOG_ERR, 
			"DB ERROR when try to truncate table \'%s\': %s", 
			szDBName, mysql_error(m_pDBHandle)
			);
		goto Exit0;
	}

	bResult = TRUE;
Exit0:
	return bResult; 
}

BOOL KTestDBManager::InsertRoleInDB(const char* pszRoleGroup, const char* pszRoleName, void* pRoleInfoBuffer, int nBufferLength)
{
	BOOL                bResult         = false;
	int                 nRetCode        = false;
	DWORD               dwRetMailID     = 0;
	int                 nParamCount     = 0;
	unsigned long       ulNameSrcLen    = 0;
	unsigned long       ulGroupSrcLen      = 0;
	unsigned long       ulSrcLen   = 0;
	MYSQL_STMT*         pStmt           = NULL;  
 	MYSQL_BIND          Bind[3];
	my_ulonglong        lAffectedRow    = 0;
   
	char  m_szInsertSQL[] = (
		"INSERT INTO `ROLE` (ROLENAME , LISTITEM , GroupName)"
		"VALUES ( ?, ? , ?) "
		);

	pStmt = mysql_stmt_init(m_pDBHandle);
	if (!pStmt)
	{
		KGLogPrintf(KGLOG_ERR, " mysql_stmt_init(), out of memory\n");
		goto Exit0;
	}

	ulSrcLen =  (unsigned long)strlen(m_szInsertSQL);
	if (mysql_stmt_prepare(pStmt, m_szInsertSQL, ulSrcLen))
	{
		KGLogPrintf(KGLOG_ERR, " %s\n", mysql_stmt_error(pStmt));
		goto Exit0;
	}

	nParamCount= mysql_stmt_param_count(pStmt);
	if (nParamCount != 3) /* validate parameter count */
	{
		KGLogPrintf(KGLOG_ERR, " invalid parameter count returned by MySQL\n");
		goto Exit0;
	}

	memset(Bind, 0, sizeof(Bind));

    ulNameSrcLen = (unsigned long)strlen(pszRoleName);
	Bind[0].buffer_type	  = MYSQL_TYPE_STRING;
	Bind[0].buffer		  = (void*)pszRoleName;
	Bind[0].buffer_length = _NAME_LEN;
	Bind[0].length		  = &ulNameSrcLen;

	Bind[1].buffer_type	  = MYSQL_TYPE_BLOB;
	Bind[1].buffer		  = pRoleInfoBuffer;
	Bind[1].buffer_length = 1024;
	Bind[1].length		  = (unsigned long*)&nBufferLength;

	ulGroupSrcLen = (unsigned long)strlen(pszRoleGroup);
	Bind[2].buffer_type	  = MYSQL_TYPE_STRING;
	Bind[2].buffer		  = (void*)pszRoleGroup;
	Bind[2].buffer_length = _NAME_LEN;
	Bind[2].length		  = &ulGroupSrcLen;

	if (mysql_stmt_bind_param(pStmt, Bind))
	{
		KGLogPrintf(KGLOG_ERR, " %s\n", mysql_stmt_error(pStmt));
		goto Exit0;
	}

	if (mysql_stmt_execute(pStmt))
	{
		KGLogPrintf(KGLOG_ERR, " %s\n", mysql_stmt_error(pStmt));
		goto Exit0;
	}

	lAffectedRow = mysql_stmt_affected_rows(pStmt);
	KGLOG_PROCESS_ERROR(lAffectedRow <= 1);

	bResult = true;
Exit0:
	if (pStmt)
	{
		mysql_stmt_close(pStmt);
	}
	return bResult;
}

BOOL KTestDBManager::UpDateFellowShipData(DWORD dwPlayerID, BYTE byData[], size_t uDatalength)
{
	BOOL                            bResult             = false;
	int                             nRetCode            = false;
	MYSQL_RES*                      pQueryRes           = NULL;
	MYSQL_STMT*                     pMySQLSTMT          = NULL;
	MYSQL_STMT*                     pInsertFellowship   = NULL;
	MYSQL_STMT*                     pUpdateFellowship   = NULL;
	int                             nParamCount         = 0;
	int                             nRowNum             = 0;
	unsigned long                   ulStrLen            = 0;
	unsigned int                    uDataLen            = 0;
	unsigned int                    uGroupNameLen       = 0;
	my_ulonglong                    lAffectedRow        = 0;
	char                            szSQL[1024];
	MYSQL_BIND                      Bind[3];


	pInsertFellowship = mysql_stmt_init(m_pDBHandle);
	KGLOG_PROCESS_ERROR(pInsertFellowship);

	ulStrLen = snprintf(szSQL, sizeof(szSQL), 
		"INSERT INTO "
		"    %s "
		"    (FellowshipData, PlayerID) "
		"VALUES "
		"    (?, ?) "
		";",
		FELLOWSHIP_TABLE_NAME
		);

	KGLOG_PROCESS_ERROR(ulStrLen < sizeof(szSQL));

	nRetCode = mysql_stmt_prepare(pInsertFellowship, szSQL, ulStrLen);
	if (nRetCode)
	{
		KGLogPrintf(KGLOG_ERR, "DB ERROR: %s\n", mysql_stmt_error(pInsertFellowship));
		goto Exit0;
	}

	pUpdateFellowship = mysql_stmt_init(m_pDBHandle);
	KGLOG_PROCESS_ERROR(pUpdateFellowship);

	ulStrLen = snprintf(szSQL, sizeof(szSQL), 
		"UPDATE "
		"    %s "
		"SET "
		"    FellowshipData = ? "
		"WHERE "
		"    PlayerID = ?  "
		";",
		FELLOWSHIP_TABLE_NAME
		);

	KGLOG_PROCESS_ERROR(ulStrLen < sizeof(szSQL));

	nRetCode = mysql_stmt_prepare(pUpdateFellowship, szSQL, ulStrLen);
	if (nRetCode)
	{
		KGLogPrintf(KGLOG_ERR, "DB ERROR: %s\n", mysql_stmt_error(pUpdateFellowship));
		goto Exit0;
	}

	memset(Bind, 0, sizeof(Bind));

	nRetCode = snprintf(szSQL, sizeof(szSQL), 
		"SELECT "
		"    FellowshipData "
		"FROM "
		"    %s "
		"WHERE "
		"    PlayerID = %u  "
		";", 
		FELLOWSHIP_TABLE_NAME, 
		dwPlayerID
		);
	KGLOG_PROCESS_ERROR(nRetCode < sizeof(szSQL));

	nRetCode = _DoQuey(szSQL, nRetCode);
	KGLOG_PROCESS_ERROR(nRetCode);

	pQueryRes = mysql_store_result(m_pDBHandle);
	KGLOG_PROCESS_ERROR(pQueryRes);

	nRowNum = (int)mysql_num_rows(pQueryRes);
	KGLOG_PROCESS_ERROR(nRetCode >= 0 && nRowNum <= 1);

	if (nRowNum == 0)
	{
		pMySQLSTMT = pInsertFellowship;
	}
	else 
	{
		pMySQLSTMT = pUpdateFellowship;
	}

	// FellowshipData
	uDataLen = (unsigned)uDatalength;
	Bind[0].buffer_type     = MYSQL_TYPE_BLOB;
	Bind[0].buffer          = byData;
	Bind[0].buffer_length   = 2048;
	Bind[0].length          = (unsigned long*)&uDataLen;

	// PlayerID
	Bind[1].buffer_type     = MYSQL_TYPE_LONG;
	Bind[1].buffer          = &dwPlayerID;
	Bind[1].buffer_length   = sizeof(DWORD);
	Bind[1].is_unsigned     = TRUE;

	
	nParamCount = mysql_stmt_param_count(pMySQLSTMT);
	KGLOG_PROCESS_ERROR(nParamCount == 2);

	nRetCode = mysql_stmt_bind_param(pMySQLSTMT, Bind);
	if (nRetCode)
	{
		KGLogPrintf(KGLOG_ERR, "DB ERROR: %s\n", mysql_stmt_error(pMySQLSTMT));
		goto Exit0;
	}

	nRetCode = mysql_stmt_execute(pMySQLSTMT);
	if (nRetCode)
	{
		KGLogPrintf(KGLOG_ERR, "DB ERROR: %s\n", mysql_stmt_error(pMySQLSTMT));
		goto Exit0;
	}

	lAffectedRow = mysql_stmt_affected_rows(pMySQLSTMT);
	KGLOG_PROCESS_ERROR(lAffectedRow <= 1);

	bResult = true;
Exit0:
	if (pInsertFellowship)
	{
		mysql_stmt_close(pInsertFellowship);
		pInsertFellowship = NULL;
	}

	if (pUpdateFellowship)
	{
		mysql_stmt_close(pUpdateFellowship);
		pUpdateFellowship = NULL;
	}

	if (pQueryRes)
	{
		mysql_free_result(pQueryRes);
		pQueryRes = NULL;
	}
	return bResult;
}

BOOL KTestDBManager::LoadMapCopyInfo(DWORD dwMapID, __int64 nMapCopyIndex, BYTE** ppbyData, size_t& uDataLen)
{
    BOOL                bResult         = false;
    BOOL                bRetCode        = false;
    int                 nRetCode        = false;
    MYSQL_RES*          pQueryRes       = NULL;
    unsigned long*      pulFieldLen     = 0;
    DWORD               dwOwnerID       = 0;
    BYTE*               pbyMapData      = NULL;
    char                szSQL[256];
    MYSQL_ROW           QueryRow;

    ASSERT(m_pDBHandle);

    nRetCode = snprintf(
        szSQL, sizeof(szSQL),
        "select " 
        "   MapID, MapCopyIndex, OwnerID, MapData"
        "  from " 
        "   %s "
        " where " 
        "    MapID  = %d AND MapCopyIndex = %d "
        " ;",
        MAIL_TABLE_NAME,
        dwMapID,
        nMapCopyIndex
        );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(szSQL));

    nRetCode = _DoQuey(szSQL, (unsigned long)strlen(szSQL));
    KGLOG_PROCESS_ERROR(nRetCode);

    pQueryRes = mysql_use_result(m_pDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    while (true)
    {
        QueryRow = mysql_fetch_row(pQueryRes);
        if (!QueryRow)
            break;

        pulFieldLen = mysql_fetch_lengths(pQueryRes);
        KGLOG_PROCESS_ERROR(pulFieldLen);

        pbyMapData  = (BYTE*)QueryRow[3];        

        if (pulFieldLen[3])
        {
            *ppbyData = new BYTE(pulFieldLen[3]);
            KGLOG_PROCESS_ERROR(ppbyData);

            memcpy(*ppbyData, pbyMapData, pulFieldLen[3]);

            uDataLen = pulFieldLen[3];
        }
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

IKG_Buffer* KTestDBManager::GetRoleData(const char szTablename[], const char szIndex[], const char szCondition[], DWORD dwRoleID)
{
    int nRetCode = false;

    nRetCode =  snprintf(
        m_szSQL, sizeof(m_szSQL), "select %s from %s where %s = %u",
        szIndex, szTablename, szCondition, dwRoleID
        );
    KGLOG_PROCESS_ERROR(nRetCode < (int)sizeof(m_szSQL));

    return QueyRueslt(m_szSQL);
Exit0:
    return NULL;
}

char *strmov(register char *dst, register const char *src)
{
	while ((*dst++ = *src++)) ;
	return dst-1;
}

IKG_Buffer* KTestDBManager::GetRoleData(const char szTablename[], const char szIndex[], const char szCondition[], const char szRoleName[])
{
    int nRetCode = false;
	memset(m_szSQL, 0, sizeof(m_szSQL));
	char* pEnd = NULL;
    nRetCode =  snprintf(
        m_szSQL, sizeof(m_szSQL), "select %s from %s where %s = '",
        szIndex, szTablename, szCondition
        );
    KGLOG_PROCESS_ERROR(nRetCode < (int)sizeof(m_szSQL));
	pEnd = m_szSQL + nRetCode;
	if (strlen(szRoleName) != 0)
	{
		pEnd += mysql_real_escape_string(m_pDBHandle, m_szSQL + nRetCode, szRoleName, strlen(szRoleName));
	}
	
	*pEnd++ = '\'';
	*pEnd == '\0';

    return QueyRueslt(m_szSQL);
Exit0:
    return NULL;
}


BOOL KTestDBManager::DoQuery(MYSQL_STMT* pStmt)
{
    BOOL        bResult     = false;
    int         nRetCode    = 0;
    unsigned    uErrNo      = 0;

    ASSERT(pStmt);

    nRetCode = mysql_stmt_execute(pStmt);
    if (nRetCode != 0)
    {
        uErrNo = mysql_stmt_errno(pStmt);
        if (uErrNo == CR_SERVER_LOST || uErrNo == CR_CONN_HOST_ERROR || uErrNo == CR_SERVER_GONE_ERROR)
        {
            KGLogPrintf(KGLOG_INFO, "Lost connection with database, need reconnect!\n");
        }
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;
}

int KTestDBManager::AccountRoleCount(const char szAccount[])
{
    IKG_Buffer*                 piResult        = NULL;
    IKG_Buffer*                 piRoleData      = NULL;
    int                         nRetCode        = false;
    HRESULT                     hRetCode        = E_FAIL;
    int                         nParamCount     = 0;
    IKG_Buffer_ReSize*          piReSize        = NULL;
    int                         nCount          = 0;
    unsigned long               ulRoleDataSize  = 0;
    unsigned long               ulRoleNameSize  = 0;
    MYSQL_BIND                  Bind;
    MYSQL_STMT*                 pStmtLoadRole   = NULL;
    int                         nStrLen    = 0;
    char                        szSQL[1024];

    pStmtLoadRole = mysql_stmt_init(m_pDBHandle);
    KGLOG_PROCESS_ERROR(pStmtLoadRole);

    nStrLen = snprintf(
        szSQL, sizeof(szSQL), "select count(*)  from %s where %s = ?",
         KG_ROLE_TABLE_NAME, KG_ACCOUNT_FIELD_NAME
        );
    KGLOG_PROCESS_ERROR(nStrLen > 0 && nStrLen < sizeof(szSQL));
    szSQL[sizeof(szSQL) - 1] = '\0';

    nRetCode = mysql_stmt_prepare(pStmtLoadRole, szSQL, (unsigned long)nStrLen);
    KGLOG_PROCESS_ERROR(nRetCode == 0);

    // 操作数据库
    ulRoleNameSize = (unsigned long)strlen(szAccount);
    ZeroMemory(&Bind, sizeof(MYSQL_BIND));

    Bind.buffer_type     = MYSQL_TYPE_VAR_STRING;
    Bind.buffer          = (void*)szAccount;
    Bind.buffer_length   = ulRoleNameSize;
    Bind.length          = &ulRoleNameSize;

    nParamCount = mysql_stmt_param_count(pStmtLoadRole);
    KGLOG_PROCESS_ERROR(nParamCount == 1);

    nRetCode = mysql_stmt_bind_param(pStmtLoadRole, &Bind);
    KGLOG_PROCESS_ERROR(nRetCode == 0);

    nRetCode = DoQuery(pStmtLoadRole);
    KGLOG_PROCESS_ERROR(nRetCode);

    ZeroMemory(&Bind, sizeof(MYSQL_BIND));
    Bind.buffer_type     = MYSQL_TYPE_LONG;
    Bind.buffer          = (void*)&nCount;
    Bind.buffer_length   = sizeof(nCount);
    Bind.length          = &ulRoleDataSize;

    nRetCode = mysql_stmt_bind_result(pStmtLoadRole, &Bind);
    KGLOG_PROCESS_ERROR(nRetCode == 0);

    nRetCode = mysql_stmt_fetch(pStmtLoadRole);
    KGLOG_PROCESS_ERROR(nRetCode == 0);

    nRetCode = mysql_stmt_free_result(pStmtLoadRole);
    KGLOG_PROCESS_ERROR(nRetCode == 0);

Exit0:
    if (pStmtLoadRole)
    {
        mysql_stmt_close(pStmtLoadRole);
        pStmtLoadRole = NULL;
    }
    return nCount;
}


IKG_Buffer* KTestDBManager::QueyRueslt(const char szSQL[])
{
    int                         nRetCode        = false;
    unsigned long long          ullRoleCount    = 0;
    IKG_Buffer*                 pBuffer   = NULL;
    void*                       pResult  = NULL;
    unsigned long*              pulDataSize = NULL;
    MYSQL_RES*                  pQueryRes       = NULL;
    MYSQL_ROW                   QueryRow;

    assert(szSQL);

    nRetCode = _DoQuey(szSQL, strlen(szSQL));
    KGLOG_PROCESS_ERROR(nRetCode);

    pQueryRes = mysql_store_result(m_pDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    ullRoleCount = mysql_num_rows(pQueryRes);
    KG_PROCESS_ERROR(ullRoleCount == 1);

    QueryRow = mysql_fetch_row(pQueryRes);
    KGLOG_PROCESS_ERROR(QueryRow);

    KGLOG_PROCESS_ERROR(QueryRow[0] != NULL);

    pulDataSize = mysql_fetch_lengths(pQueryRes);
    KGLOG_PROCESS_ERROR(pulDataSize);

    pBuffer = KG_MemoryCreateBuffer(*pulDataSize);
    KGLOG_PROCESS_ERROR(pBuffer);

    pResult = pBuffer->GetData();
    KGLOG_PROCESS_ERROR(pResult);

    memcpy(pResult, QueryRow[0], *pulDataSize);

Exit0:
    if(pQueryRes)
    {
        mysql_free_result(pQueryRes);
        pQueryRes = NULL;
    }
    return pBuffer;
}