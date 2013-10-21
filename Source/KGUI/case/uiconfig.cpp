#include "stdafx.h"
#include "./uiconfig.h"
#include "../KGUI.h"
#include "../driver/kfilepathmgr.h"

#define F_UI_SCREEN_SHOT_FOLDER "ScreenShot"

namespace UI
{

	KUiConfig* KUiConfig::ms_pSelf = NULL;


	KUiConfig::KUiConfig()
	{
        ResetUserdata();
	}


	KUiConfig::~KUiConfig()
	{
	}

	void KUiConfig::ResetUserdata()
	{
		m_szUserAccount[0] = '\0';
		m_szUserRegion[0] = '\0';
		m_szUserServer[0] = '\0';
		m_szUserRoleName[0] = '\0';
    }

	int KUiConfig::Init()
	{
        int nRetCode = false;
        int nResult = false;
		char szBuffer[MAX_PATH];
		
        ASSERT(ms_pSelf == NULL);

		ms_pSelf = new(std::nothrow) KUiConfig;
		KGLOG_PROCESS_ERROR(ms_pSelf);
		
		_snprintf(szBuffer, _countof(szBuffer), "\\%s\\", F_UI_USER_DATA_FOLDER);

		if (!IsFilePathExist(szBuffer))
        {
			nRetCode = CreatePath(szBuffer);
            KGLOG_PROCESS_ERROR(nRetCode);
        }

		_snprintf(szBuffer, _countof(szBuffer), "\\%s\\", F_UI_SCREEN_SHOT_FOLDER);

		if (!IsFilePathExist(szBuffer))
        {
			nRetCode = CreatePath(szBuffer);
            KGLOG_PROCESS_ERROR(nRetCode);
        }
		
		nResult = true;
Exit0:
        if (!nResult)
        {
		    Exit();
        }
		return nResult;
	}

	void KUiConfig::Exit()
	{
		if (ms_pSelf == NULL)
			return;

		delete ms_pSelf;
	}

	KUiConfig& KUiConfig::GetSelf()
	{
		ASSERT(ms_pSelf);
		return *ms_pSelf;
	}

	bool KUiConfig::ReInit()
	{
		return true;
	}

    static void DeleteDir(char const* szDir)
    {
        WIN32_FIND_DATAA wfd;
        HANDLE hFile = INVALID_HANDLE_VALUE;
        char szPath[MAX_PATH];
        char szFile[MAX_PATH];

        _snprintf(szPath, _countof(szPath), "%s\\*.*", szDir);
		szPath[_countof(szPath) - 1] = '\0';

        hFile = FindFirstFileA(szPath, &wfd);
        do
        {
            if (strcmp(wfd.cFileName, ".") != 0 && strcmp(wfd.cFileName, "..") != 0)
            {
                _snprintf(szFile, _countof(szFile), "%s\\%s", szDir, wfd.cFileName);
				szFile[_countof(szFile) - 1] = '\0';

                if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
                {
                    DeleteDir(szFile);
                    RemoveDirectoryA(szFile);
                }
                else
                {
                    SetFileAttributesA(szFile, FILE_ATTRIBUTE_NORMAL);
                    DeleteFileA(szFile);
                }
            }

        }
        while (FindNextFileA(hFile, &wfd)); 
        FindClose(hFile);
    }

    int KUiConfig::RemoveUserdata()
    {
        char szPath[MAX_PATH];
		int nLen = 0;
		int nUserdataFolder = 0;

        g_GetPriorRootPath(szPath);
        if (szPath[0] == '\0')
		{
            g_GetRootPath(szPath);
		}
		szPath[_countof(szPath) - 1] = '\0';

		nLen = (int)strlen(szPath);
		nUserdataFolder = (int)strlen(F_UI_USER_DATA_FOLDER);
		KGLOG_PROCESS_ERROR(nLen > 0 && nUserdataFolder > 0 && nLen + nUserdataFolder < MAX_PATH - 2);

		if (szPath[nLen - 1] != '\\' && szPath[nLen - 1] != '/')
		{
			szPath[nLen] = '\\';
			szPath[nLen + 1] = '\0';
		}
		strcat(szPath, F_UI_USER_DATA_FOLDER);

        DeleteDir(szPath);

		return true;
Exit0:
        return false;
    }

	int KUiConfig::SetUserAccount(LPCSTR pcszAccountName)
	{
        int nRetCode = false;
        int nResult = false;
		char szBuffer[MAX_PATH];

		KGLOG_PROCESS_ERROR(pcszAccountName);

		strncpy(m_szUserAccount, pcszAccountName, _countof(m_szUserAccount) - 1);
		m_szUserAccount[_countof(m_szUserAccount) - 1] = '\0';

		_snprintf(szBuffer, _countof(szBuffer), "\\%s\\%s\\", F_UI_USER_DATA_FOLDER, m_szUserAccount);
		FormatFilePath(szBuffer);

		if (!IsFilePathExist(szBuffer))
        {
			nRetCode = CreatePath(szBuffer);
            KGLOG_PROCESS_ERROR(nRetCode);
        }

		nResult = true;
Exit0:
        return nResult;
	}


	int KUiConfig::SetUserServer(LPCSTR pcszRegionName, LPCSTR pcszServerName)
	{
        int nRetCode = false;
        int nResult = false;
		char szBuffer[MAX_PATH];

		KGLOG_PROCESS_ERROR(pcszServerName && pcszServerName[0] != '\0');
		KGLOG_PROCESS_ERROR(pcszRegionName && pcszRegionName[0] != '\0');

		strncpy(m_szUserServer, pcszServerName, _countof(m_szUserServer) - 1);
		m_szUserServer[_countof(m_szUserServer) - 1] = '\0';

		strncpy(m_szUserRegion, pcszRegionName, _countof(m_szUserRegion) - 1);
		m_szUserRegion[_countof(m_szUserRegion) - 1] = '\0';

		_snprintf(szBuffer, _countof(szBuffer), "\\%s\\%s\\%s\\%s\\", F_UI_USER_DATA_FOLDER, m_szUserAccount, m_szUserRegion, m_szUserServer);
		FormatFilePath(szBuffer);

		if (!IsFilePathExist(szBuffer))
        {
			nRetCode = CreatePath(szBuffer);
            KGLOG_PROCESS_ERROR(nRetCode);
        }

		nResult = true;
Exit0:
		return nResult;
	}

	int KUiConfig::SetUserRoleName(LPCSTR pcszRoleName)
	{
		int nRetCode = false;
        int nResult = false;
		char szBuffer[MAX_PATH];

		KGLOG_PROCESS_ERROR(pcszRoleName);
        KGLOG_PROCESS_ERROR(pcszRoleName[0] != '\0');

		strncpy(m_szUserRoleName, pcszRoleName, _countof(m_szUserRoleName) - 1);
		m_szUserRoleName[_countof(m_szUserRoleName) - 1] = '\0';

		_snprintf(szBuffer, _countof(szBuffer), "\\%s\\%s\\%s\\%s\\%s", F_UI_USER_DATA_FOLDER, m_szUserAccount, m_szUserRegion, m_szUserServer, m_szUserRoleName);
		FormatFilePath(szBuffer);

		if (!IsFilePathExist(szBuffer))
        {
			nRetCode = CreatePath(szBuffer);
            KGLOG_PROCESS_ERROR(nRetCode);
        }

		nResult = true;
Exit0:
		return nResult;
	}

	LPCSTR KUiConfig::GetUserAccount() const
	{
		return m_szUserAccount;
	}

	LPCSTR KUiConfig::GetUserRegion() const
	{
		return m_szUserRegion;
	}

	LPCSTR KUiConfig::GetUserServer() const
	{
		return m_szUserServer;
	}

	LPCSTR KUiConfig::GetUserRoleName() const
	{
		return m_szUserRoleName;
	}

	LPCSTR KUiConfig::GetUserDataFloder() const
	{
		return F_UI_USER_DATA_FOLDER;
	}

	int KUiConfig::GetUserDataPath(char szFilePath[MAX_PATH], LPCSTR pcszRoleName) const
	{
		int nRetCode = false;
        int nResult = false;

		KG_ASSERT_EXIT(szFilePath);

		KGLOG_PROCESS_ERROR(m_szUserAccount[0] != '\0');
		KGLOG_PROCESS_ERROR(m_szUserRegion[0] != '\0');
		KGLOG_PROCESS_ERROR(m_szUserServer[0] != '\0');

		if (!pcszRoleName)
		{
			nRetCode = m_szUserRoleName[0] != '\0';
			KGLOG_PROCESS_ERROR(nRetCode);
		}

		_snprintf(szFilePath, MAX_PATH, "\\%s\\%s\\%s\\%s\\%s", F_UI_USER_DATA_FOLDER, m_szUserAccount, m_szUserRegion, m_szUserServer, pcszRoleName ? pcszRoleName: m_szUserRoleName);
		FormatFilePath(szFilePath);

		nResult = true;
Exit0:
		return nResult;
	}

	int KUiConfig::IsFilePathExist(LPCSTR szFilePath)
	{
        char szPriorRootPath[MAX_PATH];
        char szFullPath[MAX_PATH];

		if (szFilePath == NULL || szFilePath[0] == '\0')
			return false;

        g_GetPriorRootPath(szPriorRootPath);

        if (szPriorRootPath[0] != '\0')
            g_GetPriorFullPath(szFullPath, szFilePath);
        else
            g_GetFullPath(szFullPath, szFilePath);

		return ::GetFileAttributesA(szFullPath) != 0xFFFFFFFF;
	}

    int KUiConfig::CreatePath(LPCSTR szFilePath)
    {
        int nRetCode = false;
        int nResult = false;;
        char szPriorRootPath[MAX_PATH];
        char szFullPath[MAX_PATH];

        KGLOG_PROCESS_ERROR(szFilePath);
        KGLOG_PROCESS_ERROR(szFilePath[0] != '\0');

        g_GetPriorRootPath(szPriorRootPath);

        if (szPriorRootPath[0] != '\0')
            g_GetPriorFullPath(szFullPath, szFilePath);
        else
            g_GetFullPath(szFullPath, szFilePath);

        nRetCode = g_CreatePath(szFullPath);
        KGLOG_PROCESS_ERROR(nRetCode);

        nResult = true;
Exit0:
        return nResult;
    }

	int KUiConfig::Load(LPCSTR szAccountName)
	{
		int nRetCode = false;
        int nResult = false;
		char szFilePath[MAX_PATH];
		IIniFile* pIni = NULL;

		ResetUserdata();

		KGLOG_PROCESS_ERROR(szAccountName);
        KGLOG_PROCESS_ERROR(szAccountName[0] != '\0');

		_snprintf(szFilePath, _countof(szFilePath), "\\%s\\%s\\%s", F_UI_USER_DATA_FOLDER, szAccountName, "login.ini");
		FormatFilePath(szFilePath);

		nRetCode = !g_IsFileExist(szFilePath);
		KG_PROCESS_SUCCESS(nRetCode);

		pIni = g_OpenIniFile(szFilePath);
		KGLOG_PROCESS_ERROR(pIni);

		pIni->GetString("LastLogin", "RoleName", "", m_szUserRoleName, _countof(m_szUserRoleName));
		pIni->GetString("LastLogin", "RegionName", "", m_szUserRegion, _countof(m_szUserRegion));
		pIni->GetString("LastLogin", "ServerName", "", m_szUserServer, _countof(m_szUserServer));

		strncpy(m_szUserAccount, szAccountName, _countof(m_szUserAccount) - 1);
		m_szUserAccount[_countof(m_szUserAccount) - 1] = '\0';

Exit1:
		nResult = true;
Exit0:
        KG_COM_RELEASE(pIni);
		return nResult;
	}


	int KUiConfig::Save(LPCSTR szAccountName)
	{
		int nRetCode = false;
        int nResult = false;
		char szFilePath[MAX_PATH];
		IIniFile* pIni = NULL;

		nRetCode = szAccountName && szAccountName[0] != '\0';
		KGLOG_PROCESS_ERROR(nRetCode);

		_snprintf(szFilePath, _countof(szFilePath), "\\%s\\%s\\%s", F_UI_USER_DATA_FOLDER, szAccountName, "login.ini");
		FormatFilePath(szFilePath);

		if (g_IsFileExist(szFilePath))
		{
			pIni = g_OpenIniFile(szFilePath);
			KGLOG_PROCESS_ERROR(pIni);
		}
		else
		{
			pIni = g_CreateIniFile();
			KGLOG_PROCESS_ERROR(pIni);
		}

		pIni->WriteString("LastLogin", "RoleName", m_szUserRoleName);
		pIni->WriteString("LastLogin", "RegionName", m_szUserRegion);
		pIni->WriteString("LastLogin", "ServerName", m_szUserServer);

		pIni->Save(szFilePath);

#if 0
        KGLogPrintf(KGLOG_INFO, "KGUI Save(%s, %s, %s, %s, %s, %s, %s\n",
		    m_szUserAccount, m_szUserRegion, m_szUserServer, m_szUserRoleName, m_szVersionLineFullName, m_szVersion, m_szVersionLineName);
#endif

		nResult = true;
Exit0:
        KG_COM_RELEASE(pIni);
		return nResult;
	}

    int KUiConfig::IsUserdataExist(LPCSTR pcszAccountName) const
    {
        int nRetCode = false;
        int nResult = false;
        char szPath[MAX_PATH];
		int nLen = 0;
		int nUserdataFolder = 0;

        g_GetPriorRootPath(szPath);
        if (szPath[0] == '\0')
		{
            g_GetRootPath(szPath);
		}
		szPath[_countof(szPath) - 1] = '\0';

		nLen = (int)strlen(szPath);
		nUserdataFolder = (int)strlen(F_UI_USER_DATA_FOLDER);
		KGLOG_PROCESS_ERROR(nLen > 0 && nUserdataFolder > 0 && nLen + nUserdataFolder < MAX_PATH - 4);

		if (szPath[nLen - 1] != '\\' && szPath[nLen - 1] != '/')
		{
			szPath[nLen] = '\\';
			szPath[nLen + 1] = '\0';
		}
		
        strcat_s(szPath, _countof(szPath), F_UI_USER_DATA_FOLDER);
        strcat_s(szPath, _countof(szPath), "\\");
		strcat_s(szPath, _countof(szPath), pcszAccountName);

		nRetCode = IsFilePathExist(szPath);
        KG_PROCESS_ERROR(nRetCode);

        nResult = true;
Exit0:
        return nResult;
    }

}
