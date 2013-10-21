/* -------------------------------------------------------------------------
//	文件名		：	kdatabaseplugin.cpp
//	创建者		：	wangbin
//	创建时间	：	2007-12-19 10:44:00
//	功能描述	：	数据库插件实现：执行指定的SQL语句然后将结果返回
// -----------------------------------------------------------------------*/
#pragma warning(disable:4005)
#include "def.h"
#include "kdatabaseplugin.h"
#include "Engine/KGLog.h"
//#include "Engine/kinifile.h"
#include "fsencrypt.h"
#include "kringbuffer.h"
#ifdef __linux
#include <unistd.h>
#endif

#define DB_CFG					"guard.cfg"
#define SECTION_DATABASE		"DBSecure"
#define SQL_SETUTF8				"SET names utf8"
#define SQL_SETGBK				"SET character_set_client=gbk"
#define DEFAULT_CHARACTERSET	"GB2312"

KDatabasePlugin::KDatabasePlugin() :
	m_pSender(NULL),
	m_pDatabase(NULL),
	m_pGuard(NULL),
	m_tLastPing(0),
	m_bStop(FALSE)
{
#ifdef WIN32
	m_hThread = NULL;
#else
	m_hThread = 0;
#endif
	CONST INT nReqBufLen = 5 * 1024 * 1024;
	CONST INT nResBufLen = 10 * 1024 * 1024;
	m_pszReqBuff = new CHAR[nReqBufLen];
	m_pszResBuff = new CHAR[nResBufLen];
	m_pReqBuffer = new KRingBuffer(m_pszReqBuff, nReqBufLen);	// 请求缓冲区
	m_pResBuffer = new KRingBuffer(m_pszResBuff, nResBufLen);	// 结果缓冲区
}

KDatabasePlugin::~KDatabasePlugin()
{
	delete m_pReqBuffer;
	m_pReqBuffer = NULL;
	delete m_pResBuffer;
	m_pResBuffer = NULL;
	delete[] m_pszReqBuff;
	m_pszReqBuff = NULL;
	delete[] m_pszResBuff;
	m_pszResBuff = NULL;
}

int KDatabasePlugin::Disconnect()
{
	return fseye_success;
}

int KDatabasePlugin::GetDescribe(char* pszDesc, int nLen)
{
	ASSERT(pszDesc && nLen > 0);
	strncpy(pszDesc, "Execute Sql Script etc..", nLen - 1);
	pszDesc[nLen - 1] = '\0';
	return fseye_success;
}

int KDatabasePlugin::GetGUID(char* pszGuid, int nLen)
{
	ASSERT(pszGuid && nLen - 1);
	strncpy(pszGuid, "{244627F1-5FD0-43eb-8B2D-12E90B853130}", nLen - 1);
	pszGuid[nLen - 1] = '\0';
	return fseye_success;
}

int KDatabasePlugin::GetVersion(int& nMaxVersion, int& nMinVersion)
{
	nMaxVersion = EXESQLMAXVER;
	nMinVersion = EXESQLMINVER;
	return fseye_success;
}

int KDatabasePlugin::GetAuthor(char* pszAuthor, int nLen)
{
	ASSERT(pszAuthor && nLen > 0);
	strncpy(pszAuthor, "wangbin", nLen - 1);
	pszAuthor[nLen - 1] = '\0';
	return fseye_success;
}

int KDatabasePlugin::Breathe()
{
	time_t tNow = time(NULL);
	if (m_tLastPing == 0)
	{
		m_tLastPing = tNow;
	}
	else if (tNow - m_tLastPing >= emPING_INTERVAL)
	{
		mysql_ping(m_pDatabase);
		m_tLastPing = tNow;
	}
	while (HandleResult());
	return fseye_success;
}

int KDatabasePlugin::LoadPlug(ISender* pSender, IGuard* pGuard)
{
	ASSERT(pSender && pGuard);
	int nRet = mydb_err_opendb;
	do 
	{
		CHAR szPath[MAX_PATH];
		pGuard->GetRootPath(szPath);
		g_SetRootPath(szPath);
		IIniFile* ini = g_OpenIniFile(DB_CFG);
		if (!ini)
		{
			pGuard->Log("Failed to load %s!", DB_CFG);
			break;
		}
		CHAR szHost[MAX_PATH];
		CHAR szDatabase[MAX_PATH];
		CHAR szUser[MAX_PATH];
		CHAR szPasswd[MAX_PATH];
		CHAR szCharacterSet[MAX_PATH];
		if (!ini->GetString(SECTION_DATABASE, "HostName", "", szHost, MAX_PATH - 1) ||
			!ini->GetString(SECTION_DATABASE, "DBName", "", szDatabase, MAX_PATH - 1) ||
			!ini->GetString(SECTION_DATABASE, "LoginName", "", szUser, MAX_PATH - 1) ||
			szHost[0] == '\0' ||
			szDatabase[0] == '\0' ||
			szUser[0] == '\0')
		{
			pGuard->Log("Warning: Invalid database config!");
			break;
		}
		if (!ini->GetString(SECTION_DATABASE, "LoginPwd", "", szPasswd, MAX_PATH - 1) || szPasswd[0] == '\0')
		{
			pGuard->Log("Failed to read database password!");
			break;
		}
		ini->GetString(SECTION_DATABASE, "CharacterSet", DEFAULT_CHARACTERSET, szCharacterSet, MAX_PATH - 1);
		szCharacterSet[MAX_PATH - 1] = '\0';

		CHAR szPlainPasswd[MAX_PATH];
		szPlainPasswd[0] = '\0';
		if (TRUE)//!DBDecrypt(szPasswd, szPlainPasswd))
		{
			strcpy(szPlainPasswd, szPasswd);
		}
		if (!InitializeDatabase(
				szHost,
				3306,
				szUser,
				szPlainPasswd,
				szDatabase,
				szCharacterSet))
		{
			pGuard->Log("Failed to initialize database!");
			break;
		}

		m_pSender = pSender;
		m_pGuard = pGuard;

		StartThread();
		nRet = fseye_success;
	}
	while(0);
	return nRet;
}

int KDatabasePlugin::Release()
{
	StopThread();

	UnInitializeDatabase();
	KGLogUnInit(NULL);
	return fseye_success;
}

BOOL KDatabasePlugin::ExecuteSql(LPCSTR pszSql, INT nLen, MYSQL_RES** ppRes, BOOL bLog)
{
	ASSERT(pszSql && nLen > 0);
	MYSQL_RES* pRes = NULL;
	BOOL bRet = FALSE;
	do 
	{
		/*
		mysql_real_query(m_pDatabase, SQL_SETUTF8, sizeof(SQL_SETUTF8) - 1);
		mysql_real_query(m_pDatabase, SQL_SETGBK, sizeof(SQL_SETGBK) - 1);
		char szNames[100];
		strcpy(szNames, " SET names gb2312");
		int nResult = 0;
		nResult = mysql_real_query(m_pDatabase, szNames, strlen(szNames));

		strcpy(szNames, " SET character_set_client=gb2312");
		nResult = mysql_real_query(m_pDatabase, szNames, strlen(szNames));
		*/

		if (mysql_real_query(m_pDatabase, pszSql, nLen) != 0)
		{
			// 日志
			LPCSTR pszError = mysql_error(m_pDatabase);
			CHAR szBuf[18192];
			CHAR szSql[8192];
			memcpy(szSql, pszSql, nLen);
			szSql[nLen] = 0;
			snprintf(szBuf, sizeof(szBuf) - 1, "Failed to execute sql %s:\"%s\"", pszError, szSql);
			szBuf[sizeof(szBuf) - 1] = '\0';
			m_pGuard->Log(szBuf);
			break;
		}
		pRes = mysql_use_result(m_pDatabase);
		if (!pRes)
		{
			bRet = mysql_affected_rows(m_pDatabase) > 0;
		}
		else
		{
			bRet = TRUE;		
		}
	}
	while(0);
	if (ppRes)
		*ppRes = pRes;
	return bRet;
}

bool KDatabasePlugin::CheckProtocol(int nProtocol)
{
	return nProtocol == e2g_exesql_def || nProtocol == e2g_exesql2_def;
}

int KDatabasePlugin::ProcessNetMessage(unsigned char* pData, unsigned int nLen)
{
	ASSERT(pData && nLen > 0);
	int nRet = fseye_success;
	BYTE byProtocol = *(BYTE*)pData;
	if (byProtocol != e2g_exesql_def &&
		byProtocol != e2g_exesql2_def)
	{
		ASSERT(FALSE);
	}
	else
	{
		size_t nSize = m_pReqBuffer->GetFreeSize();
		if (nSize < nLen + sizeof(unsigned int))
		{
			ASSERT(FALSE);
			m_pGuard->Log("Request sql buffer is full!!!");
		}
		else
		{
			m_pReqBuffer->PutData(&nLen, sizeof(unsigned int));
			m_pReqBuffer->PutData(pData, nLen);
		}
	}
	/*
	switch (byProtocol)
	{
	case e2g_exesql_def:
		ExecuteSql(pData, nLen);
		break;
	case e2g_exesql2_def:
		ExecuteSql2(pData, nLen);
		break;
	default:
		ASSERT(FALSE);
		nRet = fseye_success;
		break;
	}
	*/
	return fseye_success;
}

VOID KDatabasePlugin::HandleQueryResult(MYSQL_RES* pRes, g2e_exesql* pMsg, int nMaxLen)
{
	ASSERT(pRes && pMsg && nMaxLen > 0);
	INT nFieldCount = mysql_num_fields(pRes);
	CHAR* pRet = (CHAR*)(pMsg + 1);
	MYSQL_ROW ppFields = mysql_fetch_row(pRes);
	pMsg->nLen = 0;
	while (ppFields)
	{
		ULONG *pnLen = mysql_fetch_lengths(pRes);
		for (int i = 0; i < nFieldCount; ++i)
		{
			ULONG nFieldLen = pnLen[i];
			UINT nDataLen = pMsg->nLen + nFieldLen + sizeof(WORD);
			if ((int)nDataLen > nMaxLen)
			{
				// 数据太多，分包发送
				ASSERT(pMsg->nLen > 0);
				pMsg->nRetCode = mydb_more_result;
				//m_pSender->SendPackToServer((UCHAR*)pMsg, sizeof(g2e_exesql) + pMsg->nLen);
				PostResult((UCHAR*)pMsg, sizeof(g2e_exesql) + pMsg->nLen);
				pRet = (CHAR*)(pMsg + 1);
				pMsg->nLen = 0;
				nDataLen = nFieldLen + sizeof(WORD);
			}
			*(WORD*)pRet = (WORD)nFieldLen;
			pRet += sizeof(WORD);
			memcpy(pRet, ppFields[i], nFieldLen);
			pRet += nFieldLen;
			pMsg->nLen = nDataLen;
		}
		ppFields = mysql_fetch_row(pRes);
	}
	pMsg->nRetCode = fseye_success;
	//m_pSender->SendPackToServer((UCHAR*)pMsg, sizeof(g2e_exesql) + pMsg->nLen);
	PostResult((UCHAR*)pMsg, sizeof(g2e_exesql) + pMsg->nLen);
}

BOOL KDatabasePlugin::InitializeDatabase(LPCSTR		pszHost,
										 UINT		nPort,
										 LPCSTR		pszUser,
										 LPCSTR		pszPasswd,
										 LPCSTR		pszDatabase,
										 LPCSTR		pszCharacterSet)
{
	ASSERT(pszHost && pszUser && pszPasswd && pszDatabase && pszCharacterSet);
	BOOL bRet = FALSE;
	do 
	{
		if (m_pDatabase)
		{
			ASSERT(FALSE);
			break;
		}
		MYSQL* pDatabase = mysql_init(NULL);
		if (!pDatabase)
			break;
		if (!mysql_real_connect(pDatabase, pszHost, pszUser, pszPasswd, pszDatabase, nPort, NULL, 0))
		{
			LPCSTR pszError = mysql_error(pDatabase);
			printf("Cannot connect MySql DB :%s\n",  pszError);
			mysql_close(pDatabase);
			break;
		}
		// 设置字符集
		CHAR szSQL[4096];
		INT nLen = sprintf(szSQL, "set @@CHARACTER_SET_CLIENT=%s", pszCharacterSet);
		if (mysql_real_query(pDatabase, szSQL, nLen) != 0)
		{
			LPCSTR pszError = mysql_error(pDatabase);
			printf("Failed to execute SQL: %s\n",  pszError);
			mysql_close(pDatabase);
			break;
		}
		nLen = sprintf(szSQL, "set @@CHARACTER_SET_CONNECTION=%s", pszCharacterSet);
		if (mysql_real_query(pDatabase, szSQL, nLen) != 0)
		{
			LPCSTR pszError = mysql_error(pDatabase);
			printf("Failed to execute SQL: %s\n",  pszError);
			mysql_close(pDatabase);
			break;
		}
		nLen = sprintf(szSQL, "set @@CHARACTER_SET_RESULTS=%s", pszCharacterSet);
		if (mysql_real_query(pDatabase, szSQL, nLen) != 0)
		{
			LPCSTR pszError = mysql_error(pDatabase);
			printf("Failed to execute SQL: %s\n",  pszError);
			mysql_close(pDatabase);
			break;
		}
		m_pDatabase = pDatabase;
		bRet = TRUE;
	}
	while(0);
	return bRet;
}

VOID KDatabasePlugin::UnInitializeDatabase()
{
	ASSERT(m_pDatabase);
	mysql_close(m_pDatabase);
	m_pDatabase = NULL;
}

VOID KDatabasePlugin::ExecuteSql(LPVOID pData, unsigned int nLen)
{
	ASSERT(pData && nLen > 0);
	do 
	{
		e2g_exesql* pMsg = (e2g_exesql*)pData;
		if (pMsg->nSqlLen == 0 ||
			pMsg->nSqlLen + sizeof(e2g_exesql) != nLen)
		{
			ASSERT(FALSE);
			break;
		}

		LPCSTR pszSQL = (LPCSTR)(pMsg + 1);
		MYSQL_RES* pRes = NULL;
		BOOL bRet = ExecuteSql(pszSQL, pMsg->nSqlLen, &pRes);
		// 服务器ID为-1表示消息是由其他插件转发而来，不需要将执行结果通知Web
		if (pMsg->wServer != (WORD)(-1))
		{
			CHAR szBuff[16 * 1024];
			g2e_exesql* pResult = (g2e_exesql*)szBuff;
			memset(pResult, 0, sizeof(g2e_exesql));
			pResult->nRetCode	= mydb_err_query;
			pResult->wProtocol	= g2e_exesql_def;
			pResult->wServer	= pMsg->wServer;
			pResult->nSessionID	= pMsg->nSessionID;
			pResult->nRetCode	= fseye_success;
			pResult->nLen		= 0;
			if (!bRet)
			{
				pResult->nRetCode = mydb_err_query;
				//m_pSender->SendPackToServer((unsigned char*)pResult, sizeof(g2e_exesql));
				PostResult((unsigned char*)pResult, sizeof(g2e_exesql));
				break;
			}
			if (pRes)
				HandleQueryResult(pRes, pResult, sizeof(szBuff) - sizeof(g2e_exesql));
		}
		if (pRes)
			mysql_free_result(pRes);
	}
	while(0);
}

// 执行SQL语句：如果第一个SQL语句执行不成功则执行第二个SQL语句
VOID KDatabasePlugin::ExecuteSql2(LPVOID pData, unsigned int nLen)
{
	ASSERT(pData && nLen > 0);
	do 
	{
		e2g_exesql2* pMsg = (e2g_exesql2*)pData;
		if (pMsg->nSql1Len == 0 ||
			pMsg->nSql2Len == 0 ||
			pMsg->nSql1Len + pMsg->nSql2Len + sizeof(e2g_exesql2) != nLen)
		{
			ASSERT(FALSE);
			break;
		}

		// 执行SQL语句
		LPCSTR pszSql1 = (LPCSTR)(pMsg + 1);
		LPCSTR pszSql2 = &pszSql1[pMsg->nSql1Len];
		MYSQL_RES* pRes = NULL;
		BOOL bRet = TRUE;
		if (!ExecuteSql(pszSql1, pMsg->nSql1Len, &pRes, FALSE) &&
			!ExecuteSql(pszSql2, pMsg->nSql2Len, &pRes, TRUE))
		{
			bRet = FALSE;
		}

		// 服务器ID为-1表示消息是由其他插件转发而来，不需要将执行结果通知Web
		if (pMsg->wServer != (WORD)(-1))
		{
			CHAR szBuff[8192];
			g2e_exesql* pResult = (g2e_exesql*)szBuff;
			memset(pResult, 0, sizeof(g2e_exesql));
			pResult->nRetCode	= mydb_err_query;
			pResult->wProtocol	= g2e_exesql_def;
			pResult->wServer	= pMsg->wServer;
			pResult->nSessionID	= pMsg->nSessionID;
			pResult->nRetCode	= fseye_success;
			pResult->nLen		= 0;
			if (bRet)
			{
				pResult->nRetCode = mydb_err_query;
				//m_pSender->SendPackToServer((unsigned char*)pResult, sizeof(g2e_exesql));
				PostResult((unsigned char*)pResult, sizeof(g2e_exesql));
				break;
			}
			if (pRes)
				HandleQueryResult(pRes, pResult, sizeof(szBuff) - sizeof(g2e_exesql));
		}
		if (pRes)
			mysql_free_result(pRes);
	}
	while(0);
}

VOID KDatabasePlugin::StartThread()
{
	m_bStop = FALSE;
#ifdef WIN32
	m_hThread = ::CreateThread(NULL,
		0,
		&KDatabasePlugin::ThreadWork,
		this,
		0,
		NULL);
#else
	pthread_create(&m_hThread,
		NULL,
		&KDatabasePlugin::ThreadWork,
		this);
#endif
	ASSERT(m_hThread);
}

VOID KDatabasePlugin::StopThread()
{
	m_bStop = TRUE;
#ifdef WIN32
	::WaitForSingleObject(m_hThread, 5000);
	::CloseHandle(m_hThread);
	m_hThread = NULL;
#else
	VOID* pRet = NULL;
	pthread_join(m_hThread, &pRet);
	m_hThread = 0;
#endif
}

#ifdef WIN32
DWORD WINAPI KDatabasePlugin::ThreadWork(VOID* pParam)
{
	ASSERT(pParam);
	KDatabasePlugin* pPlug = (KDatabasePlugin*)pParam;
	pPlug->Work();
	return 0;
}
#else
VOID* KDatabasePlugin::ThreadWork(VOID* pParam)
{
	ASSERT(pParam);
	KDatabasePlugin* pPlug = (KDatabasePlugin*)pParam;
	pPlug->Work();
	return NULL;
}
#endif

VOID KDatabasePlugin::Work()
{
	while (!m_bStop)
	{
		if (!HandleRequest())
		{
#ifdef WIN32
			Sleep(5);
#else
			usleep(5000);
#endif
		}
	}
}

BOOL KDatabasePlugin::HandleRequest()
{
	BOOL bResult = FALSE;
	do 
	{
		unsigned int nLen = 0;
		if (!m_pReqBuffer->GetData(&nLen, sizeof(unsigned int)))
			break;
		ASSERT(nLen > 0);
		VOID* pData = m_pReqBuffer->GetData(nLen, sizeof(unsigned int));
		if (!pData)
			break;
		BYTE byProtocol = *(BYTE*)pData;
		switch (byProtocol)
		{
		case e2g_exesql_def:
			ExecuteSql(pData, nLen);
			break;
		case e2g_exesql2_def:
			ExecuteSql2(pData, nLen);
			break;
		default:
			ASSERT(FALSE);
			break;
		}
		m_pReqBuffer->PopData(sizeof(nLen) + nLen);
		bResult = TRUE;
	}
	while(0);
	return bResult;
}

VOID KDatabasePlugin::PostResult(VOID* pData, unsigned int nLen)
{
	size_t nSize = m_pResBuffer->GetFreeSize();
	if (nSize >= nLen + sizeof(unsigned int))
	{
		m_pResBuffer->PutData(&nLen, sizeof(unsigned int));
		m_pResBuffer->PutData(pData, nLen);
	}
	else
	{
		ASSERT(FALSE);
		printf("Result sql buffer is full!\n");
	}
}

BOOL KDatabasePlugin::HandleResult()
{
	unsigned int nLen = 0;
	BOOL bResult = FALSE;
	if (m_pResBuffer->GetData(&nLen, sizeof(unsigned int)))
	{
		VOID* pData = m_pResBuffer->GetData(nLen, sizeof(unsigned int));
		if (pData)
		{
			m_pSender->SendPackToServer((UCHAR*)pData, nLen);
			m_pResBuffer->PopData(nLen + sizeof(unsigned int));
			bResult = TRUE;
		}
	}
	return bResult;
}
