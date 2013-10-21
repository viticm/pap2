/* -------------------------------------------------------------------------
//	文件名		：	kmonitor.h
//	创建者		：	wangbin
//	创建时间	：	2007-12-19 15:29:00
//	功能描述	：	协议中转
// -----------------------------------------------------------------------*/
#ifndef __KMONITOR_H__
#define __KMONITOR_H__

#include "def.h"
#include "kipcstream.h"

class KMonitor : public IGuardPlugin
{
	struct KSERVER
	{
		KIpcStream		cStream;
		unsigned short	wServer;	// 服务器标识
		int				nPid;		// 进程ID，0表示未初始化，-1表示进程结束，>0正常
	};
public:
	KMonitor();
	virtual ~KMonitor();
	int LoadPlug(ISender* pSender, IGuard* pGuard);
	int Release();
	int ProcessNetMessage(unsigned char* pData, unsigned int nLen);
	int Disconnect();
	int GetDescribe(char* pszDes, int nSize);
	int GetGUID(char* pszGuid, int nSize);
	int GetVersion(int& nMaxVer, int& nMinVer);
	int GetAuthor(char* pszAuthor, int nSize);
	int Breathe();
	virtual bool CheckProtocol(int nProtocol);
private:
	void ProcessServerMessages(KSERVER* pServer, char* pData, size_t nLen);
	void Process_ServerId(char* pData, size_t nLen);
	void Process_ServerPid(char* pData, size_t nLen);
	KSERVER* FindServer(WORD wServer);
	BOOL FindProcess(int nPid);
	void QueryProcessId(KSERVER* pServer);
private:
	IGuard*		m_pGuard;
	ISender*	m_pSender;
	KSERVER*	m_pServer;
};

#endif //__KMONITOR_H__
