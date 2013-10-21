/* -------------------------------------------------------------------------
//	文件名		：	kgmsplugin.h
//	创建者		：	romandou
//	创建时间	：	2007-12-19 10:44:00
//	功能描述	：	
// -----------------------------------------------------------------------*/
#ifndef __KGMSPLUGIN_H__
#define __KGMSPLUGIN_H__

#include "Ksdef.h"
#include "guardpluginterface.h"
#include "fseye_protocol.h"
class KGMSClient  ;
class KGMSPlug : public IGuardPlugin
{
public:
	enum
	{
		emPING_INTERVAL = 30,	// ping数据库的间隔时间（秒）
	};
	KGMSPlug();
	virtual ~KGMSPlug();
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
	IGuard *GetGuard(){return m_pGuard;};
	void	SendToServer(void*, size_t);
private:
	BOOL Initialize(IGuard* pGuard);
	void UnInitialize();
private:
	ISender*	m_pSender;
	IGuard*		m_pGuard;
	KGMSClient*	m_pGMSClient;
	time_t		m_tLastPing;
};

#endif //__KGMSPLUGIN_H__
