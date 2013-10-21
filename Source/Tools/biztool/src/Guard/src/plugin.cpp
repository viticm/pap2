//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-08-03 11:00
//      File_base        : plugin
//      File_ext         : .cpp
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "plugin.h"
#include "fseye_protocol.c"
#include "client.h"

/////////////////////////////////////////////////////////////////////////////
//
//              Global variables and Macro and Structure Definitions
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//              Function Definitions
//
/////////////////////////////////////////////////////////////////////////////

PlugInMan::PlugInMan() : m_cBuffer(m_szBuffer, sizeof(m_szBuffer)), m_pGuardClient(NULL)
{
	for (int i = 0; i < fseye_protocol_maxcount; ++i)
	{
		m_arPlug[i] = NULL;
	}
}

PlugInMan::~PlugInMan( )
{

}

/////////////////////////////////////////////////////////////////////////////

int PlugInMan::Init(KGuardClient* pGuardClient, LPCSTR pszPath)
{ 
	ASSERT(pszPath && pGuardClient);

#ifdef WIN32
	mkdir( PLUGPATH );
#else
	mkdir( PLUGPATH, 0755 );
	chmod( PLUGPATH, 0755 );
#endif

	m_pGuardClient = pGuardClient;
	return fseye_success; 
}

void PlugInMan::UnInit()
{
	m_pGuardClient = NULL;
}

int PlugInMan::LoadPlug()
{
	LaunchAllPlug();
	return fseye_success;
}

int PlugInMan::UnloadPlug()
{
	UnlaunchAllPlug();
	return fseye_success;
}

// 重新加载插件
void PlugInMan::ReloadPlug()
{
	UnlaunchAllPlug();
	LaunchAllPlug();
}

int PlugInMan::LaunchAllPlug( )
{
	for (int i = 0; i < fseye_protocol_maxcount; ++i)
	{
		m_arPlug[i] = NULL;
	}
	int nPlugCount = 0;
#ifdef WIN32
	char szFile[MAX_NAME] = {PLUGPATH""PLUGEXT};
	WIN32_FIND_DATA sFindData;
	HANDLE hFind = FindFirstFile(szFile, &sFindData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			INT nNameLen = strlen(sFindData.cFileName);
			if ((sFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 &&
				nNameLen > 4 &&
				stricmp(&sFindData.cFileName[nNameLen - 4], ".dll") == 0)
			{
				sprintf(szFile, PLUGPATH"%s", sFindData.cFileName);
				if (LaunchPlug(szFile) != fseye_success)
				{
					m_pGuardClient->Log("Failed to load %s!", szFile);
				}
				else
				{
					m_pGuardClient->Log("Load %s succcessfully!", szFile);
					nPlugCount++;
				}
			}
		}
		while (FindNextFile(hFind, &sFindData));
		FindClose(hFind);
	}
#else
	dirent*	ent;
	struct stat	filestat;
	char szFile[MAX_NAME] = {PLUGPATH};	
	DIR* dir = opendir(szFile);
	while (ent = readdir(dir))
	{
		sprintf(szFile, PLUGPATH"%s", ent->d_name);
		lstat(szFile, &filestat);
		if (S_ISREG(filestat.st_mode))
		{
			if (LaunchPlug(szFile) != fseye_success)
			{
				m_pGuardClient->Log("Failed to load %s!", szFile);
			}
			else
			{
				m_pGuardClient->Log("Load %s succcessfully!", szFile);
				nPlugCount++;
			}
		}
	}
	closedir(dir);
#endif
	m_pGuardClient->Log("%d plugs is loaded!", nPlugCount);
	return fseye_success;
}

int PlugInMan::UnlaunchAllPlug( )
{
	std::list<PLUGIN>::iterator it = m_listPlug.begin();
	for (; it != m_listPlug.end(); ++it)
	{
		PLUGIN& sPlug = *it;
		UnlaunchPlug(&sPlug);
	}
	m_listPlug.clear();
	for (int i = 0; i < fseye_protocol_maxcount; ++i)
	{
		m_arPlug[i] = NULL;
	}
	return fseye_success;
}

void PlugInMan::RegisterProtocol(IGuardPlugin* pPlug)
{
	ASSERT(pPlug);
	for (int i = 0; i < fseye_protocol_maxcount; i++)
	{
		if (!pPlug->CheckProtocol(i))
			continue;
		if (m_arPlug[i] != NULL)
		{
			CHAR szMsg[1024];
			szMsg[0] = '\0';
			m_arPlug[i]->GetDescribe(szMsg, sizeof(szMsg));
			m_pGuardClient->Log("Warning: Protocol[%d] is already registered by %s!", i, szMsg);
		}
		else
		{
			m_arPlug[i] = pPlug;
		}
	}
}

int PlugInMan::LaunchPlug(LPCSTR pszFile)
{
	ASSERT(pszFile);
	int nRet = guard_err;
#ifdef WIN32
	// DEBUG
	char szPath[MAX_PATH];
	getcwd(szPath, MAX_PATH);

	HMODULE PlugHandle = ::LoadLibrary(pszFile);
	if (PlugHandle)
	{
		PLUGIN sPlug;
		sPlug.Handle = PlugHandle;
		PCREATEPLUGINTERFACE pCreate = (PCREATEPLUGINTERFACE)GetProcAddress(PlugHandle, CREATEFUNCTIONNAME);
		if (pCreate)
		{
			pCreate(&sPlug.pPlugInterface);

			if (sPlug.pPlugInterface)
			{
				nRet = sPlug.pPlugInterface->LoadPlug(m_pGuardClient, this);
				if (nRet == fseye_success)
				{
					strcpy(sPlug.szPath, pszFile);
					sPlug.pPlugInterface->GetDescribe(sPlug.szDesc, MAX_DESC);
					sPlug.pPlugInterface->GetGUID(sPlug.szGUID, MAX_NAME);
					sPlug.pPlugInterface->GetAuthor(sPlug.szAuthor, MAX_NAME);
					sPlug.pPlugInterface->GetVersion(sPlug.nMaxVersion, sPlug.nMinVersion);
					sPlug.nErrCode = nRet;
					m_listPlug.push_back(sPlug);
					RegisterProtocol(sPlug.pPlugInterface);
					m_pGuardClient->Log("Launch Plug Success ![%s]", sPlug.szDesc);
				}
			}
			else
			{
				nRet = plug_opendll_err;
			}
			if (nRet != fseye_success)
			{
				FreeLibrary(PlugHandle);
			}
		}
		else
		{
			nRet = plug_getproc_err;
		}
	}
	else
	{
#ifdef WIN32
		LPSTR pszMsg= NULL;
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			0, // Default language
			(LPTSTR)&pszMsg,
			0,
			NULL);
		m_pGuardClient->Log(pszMsg);
		LocalFree(pszMsg);
#endif
		nRet = plug_creat_err;
	}

#else

	void* PlugHandle = dlopen(pszFile, RTLD_NOW);
	if( PlugHandle )
	{
		PLUGIN sPlug;
		sPlug.Handle = PlugHandle;
		PCREATEPLUGINTERFACE pCreate = (PCREATEPLUGINTERFACE)dlsym( PlugHandle, CREATEFUNCTIONNAME );
		if (pCreate)
		{
			pCreate(&sPlug.pPlugInterface);
			if (sPlug.pPlugInterface)
			{
				nRet = sPlug.pPlugInterface->LoadPlug(m_pGuardClient, this);				
				if (nRet == fseye_success)
				{
					strcpy(sPlug.szPath, pszFile);
					sPlug.pPlugInterface->GetDescribe(sPlug.szDesc, MAX_DESC);
					sPlug.pPlugInterface->GetGUID(sPlug.szGUID, MAX_NAME);
					sPlug.pPlugInterface->GetAuthor(sPlug.szAuthor, MAX_NAME);
					sPlug.pPlugInterface->GetVersion(sPlug.nMaxVersion, sPlug.nMinVersion);
					sPlug.nErrCode = nRet;
					m_listPlug.push_back(sPlug);
					RegisterProtocol(sPlug.pPlugInterface);
					m_pGuardClient->Log("Launch Plug Success ![%s]", sPlug.szDesc);
				}
			}
			else
			{
				m_pGuardClient->Log("plug error1 %s", pszFile);
				nRet = plug_opendll_err;
			}
			if (nRet != fseye_success)
			{
				m_pGuardClient->Log("plug error2 %s", pszFile);
				dlclose(PlugHandle);
			}
		}
		else
		{
			m_pGuardClient->Log("plug error3 %s", pszFile);
			nRet = plug_getproc_err;
		}
	}
	else
	{
		nRet = plug_creat_err;
		m_pGuardClient->Log("plug error4 %s", pszFile);
	}
#endif

	return nRet;
}

int PlugInMan::UnlaunchPlug(PLUGIN* pPlug)
{
	ASSERT(pPlug);	
	int nRet = guard_err;
	if (pPlug->pPlugInterface)
	{
		pPlug->pPlugInterface->Release( );
		pPlug->pPlugInterface = NULL;
	}
	if (pPlug->Handle)
	{
#ifdef WIN32
		FreeLibrary((HMODULE)pPlug->Handle);
#else
		dlclose(pPlug->Handle);
#endif
	}
	return guard_err;
}

int PlugInMan::FilterProtocol(unsigned char* pData, unsigned int datasize)
{
	switch( PH( pData ) )
	{
	case e2g_getpluginfo_def:
		{
			g2e_getpluginfo sMsg;
			memset(&sMsg, 0, sizeof(sMsg));

			sMsg.wProtocol  = g2e_getpluginfo_def;
			sMsg.wServer = 0;
			std::list<PLUGIN>::iterator it = m_listPlug.begin();
			int nMaxCount = sizeof(sMsg.PlugInfo) / sizeof(sMsg.PlugInfo[0]);
			for (int nCount = 0; nCount < nMaxCount && it != m_listPlug.end(); ++it, ++nCount)
			{
				PLUGIN* pPlug = &(*it);
				pluginfo* pInfo = &sMsg.PlugInfo[nCount];
				strncpy(pInfo->szPath, pPlug->szPath, sizeof(pInfo->szPath) - 1);
				pInfo->szPath[sizeof(pInfo->szPath) - 1] = '\0';
				strncpy(pInfo->szGUID, pPlug->szGUID, sizeof(pInfo->szGUID) - 1);
				pInfo->szGUID[sizeof(pInfo->szGUID) - 1] = '\0';
				strncpy(pInfo->szAuthor, pPlug->szAuthor, sizeof(pInfo->szAuthor) - 1);
				pInfo->szAuthor[sizeof(pInfo->szAuthor) - 1] = '\0';
				strncpy(pInfo->szDesc, pPlug->szDesc, sizeof(pInfo->szDesc) - 1);
				pInfo->szDesc[sizeof(pInfo->szDesc) - 1] = '\0';
				sprintf(
					pInfo->szVersion,
					"%d.%d\n",
					pPlug->nMaxVersion,
					pPlug->nMinVersion);
				sprintf( 
					pInfo->szModHAndModE,
					"Handle : %p ErrCode : %p\n",
					pPlug->Handle,
					pPlug->nErrCode );
				sMsg.PlugCount++;
			}

			m_pGuardClient->SendPackToServer( 
				(unsigned char*)&sMsg, 
				sizeof(g2e_getpluginfo) );
		}
		break;
	default:
		return guard_err;
	}

	return fseye_success;
}

int PlugInMan::ProcessNetMessage(unsigned char* pData, unsigned int nLen)
{
	int nRet = FilterProtocol(pData, nLen);
	if (nRet == fseye_success)
		return nRet;

	nRet = guard_err;
	do 
	{
		BYTE byProtocol = *(BYTE*)pData;
		if (byProtocol >= fseye_protocol_maxcount)
		{
			ASSERT(FALSE);
			break;
		}
		IGuardPlugin* pPlug = m_arPlug[byProtocol];
		if (!pPlug)
		{
			m_pGuardClient->Log("Protocol[%u:%u] is not registered!", byProtocol, nLen);
			break;
		}
		nRet = pPlug->ProcessNetMessage(pData, nLen);
	}
	while (0);
	return nRet;
}

void PlugInMan::Breathe()
{
	std::list<PLUGIN>::iterator it = m_listPlug.begin();
	for (; it != m_listPlug.end(); ++it)
	{
		PLUGIN* pPlug = &(*it);
		try
		{
			pPlug->pPlugInterface->Breathe( );
		}
		catch (...)
		{
			char szDesc[500];
			pPlug->pPlugInterface->GetDescribe(szDesc, sizeof(szDesc));
			m_pGuardClient->Log("plug %s is trycatch!", szDesc);
		}
	}
	ProcessMessages();
}

void PlugInMan::Disconnect( )
{
	std::list<PLUGIN>::iterator it = m_listPlug.begin();
	for (; it != m_listPlug.end(); ++it)
	{
		PLUGIN* pPlug = &(*it);
		ASSERT(pPlug->pPlugInterface);
		pPlug->pPlugInterface->Disconnect();
	}
}

void PlugInMan::GetRootPath(LPSTR pszPath)
{
	ASSERT(pszPath);
	g_GetRootPath(pszPath);
}

void PlugInMan::PutMessage(VOID* pData, int nLen)
{
	ASSERT(pData && nLen > 0);
	m_cBuffer.PutPack(pData, nLen);
}

void PlugInMan::Log(LPCSTR pszFormat, ...)
{
	if (m_pGuardClient)
	{
		va_list vl;
		va_start(vl, pszFormat);
		m_pGuardClient->LogV(pszFormat, vl);
		va_end(vl);
	}
}

void PlugInMan::ProcessMessages()
{
	size_t nLen = 0;
	CHAR* pData = m_cBuffer.GetPack(nLen);
	while (pData)
	{
		//ASSERT(nLen > 0);
		if (nLen <= 0)
			m_pGuardClient->Log("ASSERT!!!! ProcessMesge Len == %u", nLen);
		if (nLen > 0)

		ProcessNetMessage((unsigned char *)pData, nLen);
		m_cBuffer.PopPack();
		pData = m_cBuffer.GetPack(nLen);
	}
}
