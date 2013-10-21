#include "stdafx.h"
#include "./kfontschememgr.h"
#include "../KGUI.h"


namespace
{
	static float const MIN_FONT_SCALE = 0.25f;
	static float const MAX_FONT_SCALE = 4.0f;
}


namespace UI
{

	//-------------------------------------------------------------------------------------------------------
	KFontSchemeMgr::KFontSchemeMgr()
		: m_fFontScale(1.0f)
        , m_nFontsizeOffset(0)
	{
		for(int nIndex = 0; nIndex < RS2_MAX_FONT_ITEM_NUM; ++nIndex)
		{
			m_aFontList[nIndex].dwID = nIndex;
		}
	}

	KFontSchemeMgr::~KFontSchemeMgr()
	{
	}

	bool KFontSchemeMgr::Init()
	{
		BOOL bRetCode = FALSE;

		bRetCode = LoadDefaultFontList();
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = LoadScheme();
		KGLOG_PROCESS_ERROR(bRetCode);

		return true;
Exit0:
		Exit();
		return false;
	}


	void KFontSchemeMgr::Exit()
	{
		UnloadFont();
	}

	
	void KFontSchemeMgr::UnloadFont()
	{
		HRESULT hr = E_FAIL;

		hr = g_pUI->m_p3DUI->UnloadAllFont();
		KGLOG_COM_CHECK_ERROR(hr);

		for(int nIndex = 0; nIndex < RS2_MAX_FONT_ITEM_NUM; ++nIndex)
		{
			m_aFontList[nIndex].bLoaded = false;
		}

		m_ChatFont.bLoaded = false;
	}

	void KFontSchemeMgr::Reset()
	{
		HRESULT hr = E_FAIL;

		hr = g_pUI->m_p3DUI->UnloadAllFont();
		KGLOG_COM_CHECK_ERROR(hr);

		for(int nIndex = 0; nIndex < RS2_MAX_FONT_ITEM_NUM; ++nIndex)
		{
			KFontInfo & rFontInfo = m_aFontList[nIndex];
			rFontInfo.bLoaded = false;
			hr = g_pUI->m_p3DUI->LoadFont(
                rFontInfo.dwID, 
                rFontInfo.szFile, 
                m_fFontScale * (rFontInfo.nSize + m_nFontsizeOffset), 
                rFontInfo.dwStyle
            );
			if (SUCCEEDED(hr))
				rFontInfo.bLoaded = true;
		}
	}

	int KFontSchemeMgr::LoadDefaultFontList()
	{
		BOOL bResult = FALSE;
		BOOL bRetCode = FALSE;

		LPCSTR pcszFontList = g_pUI->m_FilePathMgr.GetFilePath("FontList"); 
		KGLOG_PROCESS_ERROR(pcszFontList);

		bRetCode = LoadFontList(pcszFontList);
		KGLOG_PROCESS_ERROR(bRetCode);

		bResult = true;
Exit0:
		return bResult;
	}


	int KFontSchemeMgr::LoadFontList(LPCSTR pcszFontList)
	{
		BOOL bResult = FALSE;
		BOOL bRetCode = FALSE;
		HRESULT hr = E_FAIL;
		KSmartIniFile Ini;
		int dwFontID = 0;
		int nValue = 0;
		char szFileName[MAX_PATH];
		char szSectionName[32];
		char szNextSectionName[32];

		szFileName[0] = '\0';
		szSectionName[0] = '\0';
		szNextSectionName[0] = '\0';

		KGLOG_PROCESS_ERROR(pcszFontList);

		Ini = g_OpenIniFile(pcszFontList);
		KGLOG_PROCESS_ERROR(Ini);

		UnloadFont();

		while (Ini->GetNextSection(szSectionName, szNextSectionName))
		{
			if (szNextSectionName[0] == '[')
				dwFontID = (DWORD)(atoi(szNextSectionName + 1));
			else
				dwFontID = (DWORD)(atoi(szNextSectionName));

			KGLOG_PROCESS_ERROR(dwFontID < RS2_MAX_FONT_ITEM_NUM);
			
			KFontInfo & rFontInfo = m_aFontList[dwFontID];

			Ini->GetInteger(szNextSectionName, "Size", 20, &rFontInfo.nSize);

			rFontInfo.dwStyle = KG3DUI_TEXT_STYLE_NONE;
			Ini->GetInteger(szNextSectionName, "Border", 0, &nValue);
			if (nValue)
				rFontInfo.dwStyle |= KG3DUI_TEXT_STYLE_BORDER;
			Ini->GetInteger(szNextSectionName, "Vertical", 0, &nValue);
			if (nValue)
				rFontInfo.dwStyle |= KG3DUI_TEXT_STYLE_VERTICAL;
			Ini->GetInteger(szNextSectionName, "Projection", 0, &nValue);
			if (nValue)
				rFontInfo.dwStyle |= KG3DUI_TEXT_STYLE_SHADOW;
            Ini->GetInteger(szNextSectionName, "IsMipmap", 0, &nValue);
            if (nValue)
                rFontInfo.dwStyle |= KG3DUI_TEXT_STYLE_MIPMAP;
			Ini->GetInteger(szNextSectionName, "IsAntiAlias", 1, &nValue);
			if (!nValue)
				rFontInfo.dwStyle |= KG3DUI_TEXT_STYLE_SHADOW;

			Ini->GetString(szNextSectionName, "File", "", rFontInfo.szFile, _countof(rFontInfo.szFile));
			FormatFilePath(rFontInfo.szFile);
			Ini->GetString(szNextSectionName, "Name", "", rFontInfo.szName, _countof(rFontInfo.szName));
			Ini->GetInteger(szNextSectionName, "Chat", 0, &nValue);
			if (nValue)
			{
				rFontInfo.dwStyle &= ~KG3DUI_TEXT_STYLE_VERTICAL;
				rFontInfo.bChat = true;
			}

			strncpy(szSectionName, szNextSectionName, _countof(szSectionName));
			szSectionName[_countof(szSectionName) - 1] = '\0';
		}

		Reset();

		return TRUE;
Exit0:
		KGLogPrintf(KGLOG_ERR, "KGUI KFontSchemeMgr::LoadFont(%s)\n", szFileName);
		return bResult;
	}

	KFontScheme const* KFontSchemeMgr::GetFontScheme(int nIndex) const
	{
		if (nIndex >= 0 && nIndex < GetCount())
			return &m_aScheme[nIndex];
		return NULL;
	}


	BOOL KFontSchemeMgr::SetFontScale(float fScale)
	{
		BOOL bRetCode = FALSE;

		m_fFontScale = max(min(fScale, MAX_FONT_SCALE), MIN_FONT_SCALE);

		Reset();

		return true;
	}

	float KFontSchemeMgr::GetFontScale() const
	{
		return m_fFontScale;
	}

	BOOL KFontSchemeMgr::IsFontVertical(int nIndex) const
	{
		KFontScheme const* pFontScheme = NULL;

		pFontScheme = GetFontScheme(nIndex);
		KGLOG_PROCESS_ERROR(pFontScheme);

		ASSERT(pFontScheme->nFontID < RS2_MAX_FONT_ITEM_NUM);
		ASSERT(m_aFontList[pFontScheme->nFontID].bLoaded);
		return (m_aFontList[pFontScheme->nFontID].dwStyle & KG3DUI_TEXT_STYLE_VERTICAL) != 0;
Exit0:
		return FALSE;
	}

	int KFontSchemeMgr::IsFontVerticalByID(DWORD dwFontID) const
	{
		ASSERT(dwFontID < RS2_MAX_FONT_ITEM_NUM);
		ASSERT(m_aFontList[dwFontID].bLoaded);
		return (m_aFontList[dwFontID].dwStyle & KG3DUI_TEXT_STYLE_VERTICAL) != 0;
	}

	int KFontSchemeMgr::LoadScheme()
	{
		LPCSTR szFilePath = NULL;
        KSmartIniFile Ini;
		char szSection[32] = { 0 };
		char szColorSchemeName[32] = { 0 };

		szFilePath = g_pUI->m_FilePathMgr.GetFilePath("SchemeElemFont");
		KGLOG_PROCESS_ERROR(szFilePath);

		Ini = g_OpenIniFile(szFilePath);
		KGLOG_PROCESS_ERROR(Ini);

        //清空表里面的内容,重新载入的时候需要，Added by hu changyin
        m_aScheme.clear();

		for (int nIndex = 0; ; ++nIndex)
		{
			itoa(nIndex, szSection, 10);

			if (!Ini->IsSectionExist(szSection))
				break;

			KFontScheme FontScheme;

			Ini->GetString(szSection, "Name", "", FontScheme.szName, _countof(FontScheme.szName));
			FontScheme.szName[_countof(FontScheme.szName) - 1] = '\0';
			
			Ini->GetString(szSection, "Color", "", szColorSchemeName, _countof(szColorSchemeName));
            FontScheme.nFontColorSchemeIndex = g_pUI->m_ColorSchemeMgr.FindScheme(ATL::CA2W(szColorSchemeName, GetCodePage()));

			Ini->GetString(szSection, "BorderColor", "", szColorSchemeName, _countof(szColorSchemeName));
            FontScheme.nBorderColorSchemeIndex = g_pUI->m_ColorSchemeMgr.FindScheme(ATL::CA2W(szColorSchemeName, GetCodePage()));

			Ini->GetString(szSection, "ProjectionColor", "", szColorSchemeName, _countof(szColorSchemeName));
            FontScheme.nShadowColorSchemeIndex = g_pUI->m_ColorSchemeMgr.FindScheme(ATL::CA2W(szColorSchemeName, GetCodePage()));

			Ini->GetInteger(szSection, "FontID", 0, &FontScheme.nFontID);
			Ini->GetInteger(szSection, "Size", 12, &FontScheme.nSize);
			Ini->GetInteger(szSection, "BorderSize", 0, &FontScheme.nBorderSize);
			Ini->GetInteger(szSection, "ProjectionSize", 0, &FontScheme.nShadowOffset);

			ASSERT(IsFontLoaded(FontScheme.nFontID));
			ASSERT(FontScheme.nBorderSize >= 0 && FontScheme.nBorderSize <= 2);
			ASSERT(FontScheme.nShadowOffset >= 0 && FontScheme.nShadowOffset < 256);

			m_aScheme.push_back(FontScheme);
		}

		return true;
Exit0:
		return false;
	}

	int KFontSchemeMgr::GetCount() const
	{
		return static_cast<int>(m_aScheme.size());
	}

	int KFontSchemeMgr::IsFontLoaded(DWORD dwFontID) const
	{
		if (dwFontID < RS2_MAX_FONT_ITEM_NUM && m_aFontList[dwFontID].bLoaded)
			return true;
		return false;
	}

	int KFontSchemeMgr::SetOffset(int nOffSet)
	{
		m_nFontsizeOffset = nOffSet;
		return true;
	}

	int KFontSchemeMgr::GetOffset()
	{
		return m_nFontsizeOffset;
	}


	int KFontSchemeMgr::SetFont(DWORD dwID, LPCSTR pcszName, LPCSTR pcszFile, int nSize, DWORD dwStyle)
	{
		int nResult = false;
		HRESULT hr = E_FAIL;

		KGLOG_PROCESS_ERROR(dwID < RS2_MAX_FONT_ITEM_NUM);
		KGLOG_PROCESS_ERROR(pcszName);
		KGLOG_PROCESS_ERROR(pcszFile);

		KFontInfo & rFontInfo = m_aFontList[dwID];

		if (rFontInfo.bLoaded)
		{
			hr = g_pUI->m_p3DUI->UnloadFont(dwID);
			KGLOG_COM_PROCESS_ERROR(hr);
			rFontInfo.bLoaded = false;
		}

		strncpy(rFontInfo.szName, pcszName, _countof(rFontInfo.szName));
		rFontInfo.szName[_countof(rFontInfo.szName) - 1] = '\0';

		strncpy(rFontInfo.szFile, pcszFile, _countof(rFontInfo.szFile));
		rFontInfo.szFile[_countof(rFontInfo.szFile) - 1] = '\0';

		rFontInfo.nSize = nSize;
		rFontInfo.dwStyle = dwStyle;

		hr = g_pUI->m_p3DUI->LoadFont(
            rFontInfo.dwID, 
            rFontInfo.szFile, 
            m_fFontScale * (rFontInfo.nSize + m_nFontsizeOffset), 
            rFontInfo.dwStyle
        );
		KGLOG_COM_PROCESS_ERROR(hr);

		rFontInfo.bLoaded = true;

		nResult = true;
Exit0:
		return nResult;
	}

	const KFontSchemeMgr::KFontInfo * KFontSchemeMgr::GetFont(DWORD dwID)
	{
		if (dwID < RS2_MAX_FONT_ITEM_NUM)
			return &(m_aFontList[dwID]);
		return NULL;
	}

	DWORD KFontSchemeMgr::GetChatFontID()
	{
		for(int nIndex = 0; nIndex < RS2_MAX_FONT_ITEM_NUM; ++nIndex)
		{
			if (m_aFontList[nIndex].bChat)
				return nIndex;
		}
		return RS2_MAX_FONT_ITEM_NUM;
	}

}
