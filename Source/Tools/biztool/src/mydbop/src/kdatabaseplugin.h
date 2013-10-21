/* -------------------------------------------------------------------------
//	文件名		：	kdatabaseplugin.h
//	创建者		：	wangbin
//	创建时间	：	2007-12-19 10:44:00
//	功能描述	：	数据库插件实现：执行指定的SQL语句然后将结果返回
// -----------------------------------------------------------------------*/
#ifndef __KDATABASEPLUGIN_H__
#define __KDATABASEPLUGIN_H__

#include "Ksdef.h"
#include "mysql/mysql.h"
#include "guardpluginterface.h"
#include "fseye_protocol.h"
#ifdef __linux
#include <pthread.h>
#endif

class KRingBuffer;

class KDatabasePlugin : public IGuardPlugin
{
public:
	enum
	{
		emPING_INTERVAL = 30,	// ping数据库的间隔时间（秒）
	};
	KDatabasePlugin();
	virtual ~KDatabasePlugin();
public:
	virtual int LoadPlug(ISender* pSender, IGuard* pGuard);
	virtual int Release();
	virtual int ProcessNetMessage(unsigned char* pData, unsigned int nLen);
	virtual int Disconnect();
	virtual int GetDescribe(char* pszDesc, int nLen);
	virtual int GetGUID(char* pszGuid, int nLen);
	virtual int GetVersion(int& nMaxVersion, int& nMinVersion);
	virtual int GetAuthor(char* pszAuthor, int nLen);
	virtual int Breathe();
	virtual bool CheckProtocol(int nProtocol);
private:
	VOID PostResult(VOID* pData, unsigned int nLen);
	BOOL HandleResult();
	BOOL HandleRequest();
	VOID StartThread();
	VOID StopThread();
#ifdef WIN32
	static DWORD WINAPI ThreadWork(VOID* pParam);
#else
	static VOID* ThreadWork(VOID* pParam);
#endif
	VOID Work();
	BOOL InitializeDatabase(
		LPCSTR pszHost,
		UINT   nPort,
		LPCSTR pszUser,
		LPCSTR pszPasswd,
		LPCSTR pszDatabase,
		LPCSTR pszCharacterSet);
	VOID UnInitializeDatabase();
	VOID HandleQueryResult(MYSQL_RES* pRes, g2e_exesql* pMsg, int nMaxLen);
	BOOL ExecuteSql(LPCSTR pszSql, INT nLen, MYSQL_RES** ppRes = NULL, BOOL bLog = TRUE);
	VOID ExecuteSql(LPVOID pData, unsigned int nLen);
	// 执行SQL语句：如果第一个SQL语句执行不成功则执行第二个SQL语句
	VOID ExecuteSql2(LPVOID pData, unsigned int nLen);
private:
	ISender*		m_pSender;
	IGuard*			m_pGuard;
	MYSQL*			m_pDatabase;
	time_t			m_tLastPing;
	BOOL			m_bStop;
#ifdef WIN32
	HANDLE			m_hThread;
#else
	pthread_t		m_hThread;
#endif
	KRingBuffer*	m_pReqBuffer;	// 请求缓冲区
	KRingBuffer*	m_pResBuffer;	// 结果缓冲区
	CHAR*			m_pszReqBuff;
	CHAR*			m_pszResBuff;
};

#endif //__KDATABASEPLUGIN_H__
