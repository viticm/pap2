//*********************************************************************
// file		: kclient.h
// author	: wangbin
// datetime	: 2007-12-13 16:43
// comment	: IClient接口实现
//*********************************************************************
#ifndef __KCLIENT_H__
#define __KCLIENT_H__

#include "Common/KG_Socket.h"
#include "Common/KG_Memory.h"
#include "networkinterface.h"
#include <list>

class KClient : public IClient
{
public:
	KClient(int nMaxConnectionCount);
	virtual ~KClient();
	static BOOL CreateClient(int nMaxConnectionCount, IClient**	ppClient);
public:
	virtual int Startup();
	virtual int Cleanup();
	virtual int Shutdown();
	virtual int Release();
	virtual int ConnectTo(char *pszIp, unsigned short uPort);
	virtual int Disconnect(unsigned int uLinkId);
	virtual void RegisterMsgFilter(void *pParam, CALLBACK_CLIENT_EVENT pfnEventNotify);
	virtual int SendPackToServer(unsigned int uLinkId, const void *pData, unsigned int uLen);
	virtual const void *GetPackFromServer(unsigned int uLinkId, unsigned int &uLen);
	virtual int FlushData();
	virtual int FlushData(unsigned int uLinkId);
	virtual bool IsAllRecvKey();
	virtual bool IsRecvKey(unsigned int uLinkId);
	virtual void Bind(const char* pszBindIp);
private:
	void OnConnectionClose(unsigned int uLinkId);
private:
	KG_SocketConnector		m_cConnector;			// 客户端管理器
	std::list<unsigned int>	m_listFreeLink;			// 空闲连接号
	IKG_SocketStream**		m_ppConnections;		// 连接对象数组
	INT						m_nMaxConnCount;		// 最大连接数
	CALLBACK_CLIENT_EVENT	m_pfnCallBack;			// 网络事件回调函数
	void*					m_pCallParam;			// 回调参数
	char					m_szBuffer[64 * 1024];
};

#endif //__KCLIENT_H__
