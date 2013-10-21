//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-08-03 11:00
//      File_base        : plugin
//      File_ext         : .h
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////
#ifndef _GUARD_PLUGIN_H_
#define _GUARD_PLUGIN_H_

#include "guardpluginterface.h"
#include "kpackbuffer.h"
#include "def.h"
#include <list>
#include <string>

class KGuardClient;

class PlugInMan : public IGuard
{
	typedef struct _Plugin
	{
		void* Handle;
		int nErrCode;
		int nMaxVersion;
		int nMinVersion;
		char szPath[MAX_NAME];
		char szDesc[MAX_DESC];
		char szGUID[MAX_NAME];
		char szAuthor[MAX_NAME];
		IGuardPlugin*	pPlugInterface;

		_Plugin( ):
		Handle( NULL ),
		nErrCode( fseye_success ),
		pPlugInterface( NULL )
		{
			szPath[0] = 0;
			szDesc[0] = 0;
			szGUID[0] = 0;
		}

	}PLUGIN,*PPLUGIN;
public:
	PlugInMan();
	~PlugInMan();
	int LoadPlug();
	int UnloadPlug();
	void ReloadPlug();	// 重新加载插件
	void Breathe();
	int ProcessNetMessage(unsigned char* pData, unsigned int datasize);
	void Disconnect( );	
	int Init(KGuardClient* pGuardClient, LPCSTR pszPath);
	void UnInit();
private:
	virtual void PutMessage(void* pData, int nLen);
	virtual void GetRootPath(LPSTR pszPath);
	virtual void Log(LPCSTR pszFormat, ...);
	void ProcessMessages();
	int FilterProtocol(unsigned char* pData, unsigned int datasize);
	int LaunchAllPlug();
	int UnlaunchAllPlug();
	int LaunchPlug(LPCSTR pszFile);
	int UnlaunchPlug(PLUGIN* pPlug);
	void RegisterProtocol(IGuardPlugin* pPlug);
private:
	std::list<PLUGIN>	m_listPlug;
	char				m_szBuffer[64 * 1024];
	KPackBuffer			m_cBuffer;
	IGuardPlugin*		m_arPlug[fseye_protocol_maxcount];
	KGuardClient*		m_pGuardClient;
};

#endif
