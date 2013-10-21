#include "stdafx.h"
#include "./kconfig.h"
#include "../KGUI.h"

namespace UI
{

	KConfig::KConfig()
		: m_pIniFile (NULL)
	{
		m_szLastIniFileName[0] = '\0';
	}

	KConfig::~KConfig()
	{
		Close();
	}

	bool KConfig::Init()
	{
		return true;
	}

	void KConfig::Exit()
	{
	}

	void KConfig::Close()
	{
		if (m_pIniFile)
		{
			m_pIniFile->Release();
			m_pIniFile = NULL;
			m_szLastIniFileName[0] = '\0';
		}
	}

    IIniFile* KConfig::OpenConfigFileMaxMatching(LPCSTR szFileName)
    {
#if 0
        KGLogPrintf(KGLOG_INFO, "KGUI KConfig::OpenConfigFileMaxMatching(%s)\n", szFileName);
#endif

        char szIniPath[MAX_PATH];
        char szFilePath[MAX_PATH];

		if (szFileName == NULL)
            return NULL;

        if (szFileName[0] == '\0')
            return NULL;

        if (stricmp(szFileName, m_szLastIniFileName) == 0 && m_pIniFile)
            return m_pIniFile;

        Close();

        strncpy(m_szLastIniFileName, szFileName, _countof(m_szLastIniFileName) - 1);
        m_szLastIniFileName[_countof(m_szLastIniFileName) - 1] = '\0';

        if (szFileName[0] != '\\')
        {
            szIniPath[0] = '\\';
            szIniPath[1] = '\0';
            strcat(szIniPath, szFileName);

            if (g_IsFileExist(szIniPath))
            {
                m_pIniFile = g_OpenIniFile(szIniPath);
                return m_pIniFile;
            }
        }

        if (g_IsFileExist(szFileName))
        {
            m_pIniFile = g_OpenIniFile(szFileName);
            return m_pIniFile;
        }

		LPCSTR szFilePathConfigDefault = g_pUI->m_FilePathMgr.GetFilePath("ConfigDefault");
		if (szFilePathConfigDefault == NULL)
			return NULL;

        szFilePath[0] = '\0';
		_snprintf(szFilePath, _countof(szFilePath), "%s\\%s", szFilePathConfigDefault, szFileName);
		szFilePath[_countof(szFilePath) - 1] = '\0';
        if (g_IsFileExist(szFilePath))
        {
            m_pIniFile = g_OpenIniFile(szFilePath);
            return m_pIniFile;
        }

		// -10·ÀÖ¹Òç³ö
        strncpy(szIniPath, szFileName, _countof(szIniPath) - 10);
		szIniPath[_countof(szIniPath) - 10] = '\0';
        strcat(szIniPath, ".ini");

        if (g_IsFileExist(szIniPath))
        {
            m_pIniFile = g_OpenIniFile(szIniPath);
            return m_pIniFile;
        }

        szFilePath[0] = '\0';
		_snprintf(szFilePath, _countof(szFilePath), "%s\\%s", szFilePathConfigDefault, szIniPath);
		szFilePath[_countof(szFilePath) - 1] = '\0';
        if (g_IsFileExist(szFilePath))
        {
            m_pIniFile = g_OpenIniFile(szFilePath);
            return m_pIniFile;
        }

        return NULL;
    }

}


