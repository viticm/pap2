#include "stdafx.h"
#include "./kfilepathmgr.h"

namespace UI
{

	KFilePathMgr::KFilePathMgr()
	{
	}

	KFilePathMgr::~KFilePathMgr()
	{
	}

    bool KFilePathMgr::Init()
    {
        bool bResult = false;
        char szPath[MAX_PATH];
        char szKey[128];
        IIniFile *pIni = NULL;

#ifdef KG_PUBLISH
		pIni = g_OpenIniFileInPak("\\ui\\configfilepath.ini");
		if (!pIni)
			pIni = g_OpenIniFile("\\ui\\configfilepath.ini");
		KGLOG_PROCESS_ERROR(pIni && "open ini file ui\\configfilepath.ini failed!\n");
#else
		pIni = g_OpenIniFile("\\ui\\configfilepath.ini");
		KGLOG_PROCESS_ERROR(pIni && "open ini file ui\\configfilepath.ini failed!\n");
#endif

        szKey[0] = '\0';
    	while (pIni->GetNextKey("FILE_PATH", szKey, szKey))
        {
            pIni->GetString("FILE_PATH", szKey, "", szPath, _countof(szPath));
            FormatFilePath(szPath);

            m_KeyToPath[szKey] = szPath;
        }

        bResult = true;
Exit0:
        SAFE_RELEASE(pIni);
        return bResult;
    }

	void KFilePathMgr::Exit()
	{
	}

	int KFilePathMgr::AddFilePath(LPCSTR szFilePath)
	{
		return m_aFilePath.Append(szFilePath);
	}

	bool KFilePathMgr::RemoveFilePath(int nIndex)
	{
		return m_aFilePath.Remove(nIndex);
	}

	LPCSTR KFilePathMgr::GetFilePath(int nIndex) const
	{
        std::string const* pFilePath = m_aFilePath.GetAt(nIndex);
		return pFilePath ? pFilePath->c_str() : "";
	}

    LPCSTR KFilePathMgr::GetFilePath(LPCSTR pszKey) const
    {
        if (pszKey == NULL)
            return NULL;
        KKeyToFilePathMap::const_iterator it = m_KeyToPath.find(pszKey);
        if (it != m_KeyToPath.end())
            return it->second.c_str();
        return NULL;
	}

	BOOL KFilePathMgr::TabError(LPCSTR szFmt, ...) const
	{
		char szBuffer[512];

		ASSERT(szFmt);

		va_list args;
		va_start(args, szFmt);
		vsnprintf(szBuffer, _countof(szBuffer), szFmt, args);
		va_end(args);

		szBuffer[_countof(szBuffer) - 1] = '\0';

		std::string const strTabError(szBuffer);

		if (m_TabErrors.find(strTabError) != m_TabErrors.end())
		{
			++m_TabErrors[strTabError];
			return TRUE;
		}

		m_TabErrors.insert(std::make_pair(strTabError, 1));
		KGLogPrintf(KGLOG_INFO, "KGUI tab %s", szBuffer);
		return FALSE;
	}

	void FormatFilePath(LPSTR pszPath)
	{
		if (!pszPath)
			return;

		while (pszPath && *pszPath != '\0')
		{
			if (*pszPath == '/')
				*pszPath = '\\';
			if (*pszPath >= 'A' && *pszPath <= 'Z')
				*pszPath = 'a' + (*pszPath - 'A');
			pszPath = ::CharNextA(pszPath);
		}
	}

	
	LPCSTR FormatFilePath(LPCSTR pcszPath)
	{
        //将所有文件名统一换成标准的文件名
        //如：C:\\Kingsoft/MyProject.ini 更换为 c:\\kingsoft\\myproject.ini

		static CHAR szPath[MAX_PATH];

		if (!pcszPath)
			return NULL;
		strncpy(szPath, pcszPath, _countof(szPath));
		szPath[_countof(szPath) - 1] = '\0';
		FormatFilePath(szPath);
		return szPath;
	}


}
