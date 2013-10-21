#include "stdafx.h"
#include "../KGUI.h"

namespace UI
{

	KColorSchemeMgr::KColorSchemeMgr()
		: m_pScheme(NULL)
		, m_nSchemeCount(0)
	{
	}

	KColorSchemeMgr::~KColorSchemeMgr()
	{
	}

	BOOL KColorSchemeMgr::Init()
	{
		KSmartTabFile TabFile;
		LPCSTR szFilePath = NULL;
		int nRowCount = 0;
		KColorScheme* pScheme = NULL;
		
		szFilePath = g_pUI->m_FilePathMgr.GetFilePath("SchemeElemColor");
        KGLOG_PROCESS_ERROR(szFilePath);

		TabFile = g_OpenTabFile(szFilePath);
		KGLOG_PROCESS_ERROR(TabFile);

		m_nSchemeCount = TabFile->GetHeight() - 1;
		KGLOG_PROCESS_ERROR(m_nSchemeCount > 0);

		pScheme = (KColorScheme*)realloc(m_pScheme, sizeof(KColorScheme) * m_nSchemeCount);
		KGLOG_PROCESS_ERROR(pScheme);

		m_pScheme = pScheme;

		for (int nIndex = 0; nIndex < (int)m_nSchemeCount; ++nIndex)
		{
			char szName[COLOR_NAME_MAX_LEN] = { 0 };
			int	nRed = 0;
			int nGreen = 0;
			int nBlue = 0;

			TabFile->GetString(nIndex + 2, "name", "", szName, _countof(szName));
			TabFile->GetInteger(nIndex + 2, "r", 0, &nRed);
			TabFile->GetInteger(nIndex + 2, "g", 0, &nGreen);
			TabFile->GetInteger(nIndex + 2, "b", 0, &nBlue);

			wcsncpy(m_pScheme[nIndex].wszSchemeName, ATL::CA2W(szName, GetCodePage()), COLOR_NAME_MAX_LEN - 1);
			m_pScheme[nIndex].wszSchemeName[COLOR_NAME_MAX_LEN - 1] = L'\0';

			m_pScheme[nIndex].dwColor = D3DCOLOR_XRGB((BYTE)nRed, (BYTE)nGreen, (BYTE)nBlue);
		}

		return TRUE;
Exit0:
		return FALSE;
	}

	void KColorSchemeMgr::Exit()
	{
		SAFE_FREE(m_pScheme);
	}

	int KColorSchemeMgr::FindScheme(LPCWSTR wszSchemeName) const
	{
		if (wszSchemeName == NULL)
			return -1;

		for (int nIndex = 0; nIndex < (int)m_nSchemeCount; ++nIndex)
		{
			if (wcsicmp(m_pScheme[nIndex].wszSchemeName, wszSchemeName) == 0)
			{
				return nIndex;
			}
		}

		return -1;
	}

	BOOL KColorSchemeMgr::IsExistColor(int nIndex) const
	{
		return nIndex >= 0 && nIndex < (int)m_nSchemeCount;
	}

	DWORD KColorSchemeMgr::GetColor(int nIndex) const
	{
		if (IsExistColor(nIndex))
			return m_pScheme[nIndex].dwColor;
		return 0xFFFFFFFF;
	}

}
