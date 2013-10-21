//////////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-6-7 18:27
//      File_base        : cfs_db_cnn
//      File_ext         : h
//      Author           : Cooler(liuyujun@263.net)
//      Description      : mysql database connection object wrap
//                         needed mysql 5.022 or above
//                         thread safe is not supported
//
//      <Change_list>
//
//      Example:
//      {
//      Change_datetime  : year-month-day hour:minute
//      Change_by        : changed by who
//      Change_purpose   : change reason
//      }
//////////////////////////////////////////////////////////////////////////

#ifndef _CFS__DB___CNN____H_____
#define _CFS__DB___CNN____H_____

//////////////////////////////////////////////////////////////////////////
// Include region
//#include "cfs_db_query.h"


//////////////////////////////////////////////////////////////////////////
// class declare region


//////////////////////////////////////////////////////////////////////////
// CFS_DBCNN class define region

class CFS_DBCNN
{
public:
	CFS_DBCNN(unsigned int unCnnTimeOut = DBDEFAULT_OPT_CNNTIMEOUT/* Seconds */, 
		const char *pcDefaultCharset = DBDEFAULT_OPT_CHARSET, 
		BOOL bAutoReCnn = DBDEFAULT_OPT_AUTORECNN);
	virtual ~CFS_DBCNN();

	void SetAutoReconnect(BOOL bFlag = TRUE);
	void SetConnectTimeOut(unsigned int unSeconds);
	void SetDefaultCharset(const char *pcCharsetName);

	BOOL IsConnected();
	BOOL Ping();
	BOOL Connect(const char *pcHost = DBDEFAULT_CNN_HOSTNAME, 
		const char *pcUser = DBDEFAULT_CNN_USER, 
		const char *pcPassword = DBDEFAULT_CNN_PASSWD, 
		const char *pcDatabase = DBDEFAULT_CNN_DB,
		const unsigned int uPort = 0 );
	void DisConnect();
	BOOL Refresh(unsigned int unOptions);
	BOOL Reload();
	BOOL Shutdown();

	BOOL CreateDB(const char *pcDBName);
	BOOL DestroyDB(const char *pcDBName);
	BOOL OpenDB(const char *pcDBName);
	BOOL IsExistDB(const char *pcDBName);
	BOOL Execute(const char *pcSQL, unsigned int unSQLLen, 
		BOOL bSaveToClient = FALSE, CFS_DBRESULT *pclsRes = NULL);

	const char *GetServerInfo();
	const char *GetHostInfo();
	const char *GetClientInfo();
	const char *GetLastError();
	unsigned int GetLastErrorCode();
	CFS_DBQUERY &GetRelateQuery();
	MYSQL &GetMYSQLHandle();

private:
	MYSQL			m_Sock;
	BOOL			m_bConnected;
	CFS_DBQUERY		m_clsCnnQuery;
	char			m_szTempSQLBuff[DBMAXSIZE_SQLBUFF];

	BOOL __SetOption(mysql_option enOption, const char *pParam);
};

// Public inline
inline 
void CFS_DBCNN::SetAutoReconnect(BOOL bFlag)
{
	__SetOption(MYSQL_OPT_RECONNECT, (const char *)&bFlag);
}

inline 
void CFS_DBCNN::SetConnectTimeOut(unsigned int unSeconds)
{
	__SetOption(MYSQL_OPT_CONNECT_TIMEOUT, (const char *)&unSeconds);
}

inline 
void CFS_DBCNN::SetDefaultCharset(const char *pcCharsetName)
{
	__SetOption(MYSQL_SET_CHARSET_NAME, pcCharsetName);
}

inline 
BOOL CFS_DBCNN::Ping()
{
	if(IsConnected())
	{
		return (0 == mysql_ping(&m_Sock));
	}
	else
	{
		// Not connected yet
		return TRUE;
	}
}

inline 
void CFS_DBCNN::DisConnect()
{
	if(IsConnected())
	{
		mysql_close(&m_Sock);
		m_bConnected = FALSE;
	}
}

inline 
BOOL CFS_DBCNN::Refresh(unsigned int unOptions)
{
	return (0 == mysql_refresh(&m_Sock, unOptions));
}

inline 
BOOL CFS_DBCNN::Reload()
{
	if(IsConnected())
	{
		if(0 == mysql_reload(&m_Sock))
		{
			return TRUE;
		}
	}

	return FALSE;
}

inline 
BOOL CFS_DBCNN::Shutdown()
{
	if(IsConnected())
	{
		if(0 == mysql_shutdown(&m_Sock, SHUTDOWN_DEFAULT))
		{
			return TRUE;
		}
	}

	return FALSE;
}

inline 
BOOL CFS_DBCNN::Execute(const char *pcSQL, 
						unsigned int unSQLLen, 
						BOOL bSaveToClient/* = FALSE*/, 
						CFS_DBRESULT *pclsRes/* = NULL*/)
{
	return m_clsCnnQuery.Execute(pcSQL, unSQLLen, bSaveToClient, pclsRes);
}

inline 
const char *CFS_DBCNN::GetServerInfo()
{
	return mysql_get_server_info(&m_Sock);
}

inline 
const char *CFS_DBCNN::GetHostInfo()
{
	return mysql_get_host_info(&m_Sock);
}

inline 
const char *CFS_DBCNN::GetClientInfo()
{
	return mysql_get_client_info();
}

inline 
const char *CFS_DBCNN::GetLastError()
{
	return mysql_error(&m_Sock);
}

inline 
unsigned int CFS_DBCNN::GetLastErrorCode()
{
	return mysql_errno(&m_Sock);
}

inline 
CFS_DBQUERY &CFS_DBCNN::GetRelateQuery()
{
	return m_clsCnnQuery;
}

inline 
MYSQL &CFS_DBCNN::GetMYSQLHandle()
{
	return m_Sock;
}

// Private inline
inline 
BOOL CFS_DBCNN::__SetOption(mysql_option enOption, const char *pParam)
{
	return (0 == mysql_options(&m_Sock, enOption, pParam));
}

inline 
BOOL CFS_DBCNN::IsConnected()
{
	return m_bConnected;
}

#endif // _CFS__DB___CNN____H_____
