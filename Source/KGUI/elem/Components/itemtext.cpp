////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : itemtext.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2005-11-9 16:58:18
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "engine/text.h"
#include "./itemtext.h"
#include "./itemeventmgr.h"
#include "./decoder.h"
#include "../../config/kdebugconfig.h"
#include "../../common/ksmallobjectpool.h"
#include "../../script/kscriptmgr.h"
#include "../../KGUI.h"

////////////////////////////////////////////////////////////////////////////////
namespace
{
	KSmallObjectPool	g_Allocator("KItemText", sizeof(UI::KItemText));
}

namespace UI
{

DWORD KItemText::ms_dwItemType = ITEM_TEXT;

KItemText::KItemText()
{
    m_fRowSpacing = 0.0f;
	m_fFontSpacing = 0.0f;
    m_nFontScheme = 0;
	m_dwAlpha = 255;

	m_nFirstShowIndex = 0;
	m_nShowCount = 0;

	m_dwSelectBgColor = 0;

	m_fFontScale = 1.0f;

	m_dwFontID = 0;
	m_dwFontColor = 0;
	m_dwBorderColor = 0;
	m_dwShadowColor = 0;
	m_byBorder = 0;
	m_byShadow = 0;
}

KItemText::~KItemText()
{

}

void* KItemText::operator new(size_t /* nSize */, std::nothrow_t const&) throw()
{ 
	return g_Allocator.Alloc(); 
}

void KItemText::operator delete(void* pData, std::nothrow_t const&) throw()
{ 
	g_Allocator.Free(pData); 
}

void* KItemText::operator new(size_t /* nSize */)
{ 
	return g_Allocator.Alloc(); 
}

void KItemText::operator delete(void* pData) throw()
{ 
	g_Allocator.Free(pData); 
}

#ifdef KGUI_LUA_LOADER
int KItemText::Create(IKUIObject **ppObj, Lua_State *pLua, int nTableIndex)
{
    int nResult  = false;
    int nRetCode = false;

    KItemNull *pItem = NULL;

    KGLOG_PROCESS_ERROR(ppObj);

    pItem = new(std::nothrow) KItemText;
    KGLOG_PROCESS_ERROR(pItem);

    if (pLua)
    {
        nRetCode = pItem->Init(pLua, nTableIndex);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    *ppObj = pItem;
    
// Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        SAFE_RELEASE(pItem);
    }
    return nResult;
}
#endif

KItemText *KItemText::Create(KItemTextData &ItemTextData)
{
    KItemText *pItemText = NULL;
    int nRetCode = false;

    pItemText = new(std::nothrow) KItemText;
    KG_PROCESS_ERROR(pItemText);

    nRetCode = pItemText->Init(ItemTextData);
	KG_PROCESS_ERROR(nRetCode);
	pItemText->OnInitEnd(ItemTextData);

Exit0:
    if (!nRetCode)
        SAFE_RELEASE(pItemText);
    return pItemText;
}

void KItemText::Release()
{
	KItemNull::Release();
}

#ifdef KGUI_LUA_LOADER
int KItemText::Init(Lua_State *pLua, int nTableIndex)
{
	int nResult  = false;
	int nRetCode = false;

    int    nMultiFlag      = false;
    int    nHAlign         = 0;
    int    nVAlign         = 0;
    int    nCenterEachLine = false;
    int    nRichText       = false;
    int    nAutoEtc        = false;
    int    nFontScheme     = 0;
    int    nShowAll        = false;
    int    nAutoAdj        = false;
    LPCSTR pcszText        = NULL;
    char   szTextName[KD_SMALL_BUFFER_SIZE];

    KColorSchemeMgr& ColorSchemeMgr = g_pUI->m_ColorSchemeMgr;

    KLuaTableField aParamTable[] = 
    {
        KLUA_TABLE_FIELD("MultiLine",     emVAR_TYPE_INT,    false, &nMultiFlag,     0),
        KLUA_TABLE_FIELD("RowSpacing",    emVAR_TYPE_FLOAT,  0,     &m_fRowSpacing,  0),
        KLUA_TABLE_FIELD("FontSpacing",   emVAR_TYPE_FLOAT,  0,     &m_fFontSpacing, 0),
        KLUA_TABLE_FIELD("HAlign",        emVAR_TYPE_INT,    0,     &nHAlign,        0),
        KLUA_TABLE_FIELD("VAlign",        emVAR_TYPE_INT,    0,     &nVAlign,        0),
        KLUA_TABLE_FIELD("CenterEachRow", emVAR_TYPE_INT,    0,     &nCenterEachLine,0),
        KLUA_TABLE_FIELD("RichText",      emVAR_TYPE_INT,    0,     &nRichText,      0),
        KLUA_TABLE_FIELD("AutoEtc",       emVAR_TYPE_INT,    0,     &nAutoEtc,       0),
        KLUA_TABLE_FIELD("font",          emVAR_TYPE_INT,    0,     &nFontScheme,    0),
        KLUA_TABLE_FIELD("text",          emVAR_TYPE_STRING, "",    szTextName,      _countof(szTextName)),
        KLUA_TABLE_FIELD("ShowAll",       emVAR_TYPE_INT,    true,  &nShowAll,       0),
        KLUA_TABLE_FIELD("MlAutoAdj",     emVAR_TYPE_INT,    0,     &nAutoAdj,       0),
    };

    KGLOG_PROCESS_ERROR(pLua);

    nRetCode = KItemNull::Init(pLua, nTableIndex);
    KGLOG_PROCESS_ERROR(nRetCode);

    for (int i = 0; i < _countof(aParamTable); i++)
    {
        nRetCode = g_LuaGetTableField(pLua, nTableIndex, &aParamTable[i]);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    pcszText = g_pUI->m_GlobalStrValuableMgr.GetAt(szTextName);
    if (pcszText && pcszText[0])
    {
        SetText(ATL::CA2W(pcszText, GetCodePage()));
    }

    SetFontScheme(nFontScheme); 
    SetHAlign(nHAlign);
    SetVAlign(nVAlign);
    SetMultiLine(nMultiFlag);
    SetRichText(nRichText);
    SetCenterEachLine(nCenterEachLine);
    SetAutoEtc(nAutoEtc);

//     if (ItemTextData.dwColor & 0xFF000000)
//         SetFontColor(ItemTextData.dwColor);
// 
    if (m_fWidth == 0 && m_fHeight == 0)
    {
        if (nMultiFlag)
            m_dwStyle |= ITEM_TEXT_AS_ML_NO_LIMIT;
        m_dwStyle |= ITEM_TEXT_AUTO_SIZE;
    }
    else if (
        nMultiFlag &&
        (m_fWidth <= 0) ||
        (m_fHeight <= 0)  
    )
    {
        m_dwStyle |= ITEM_TEXT_AUTO_SIZE;	//未实现的AUTO_SIZE在之后实现
    }

    if (m_dwStyle & ITEM_TEXT_AUTO_SIZE)
        AutoSize();

    g_SetBitFlag(nShowAll, ITEM_TEXT_AS_ML_LESS_ADJ, m_dwStyle);
    g_SetBitFlag(nAutoAdj, ITEM_TEXT_SHOW_ALL, m_dwStyle);

// Exit1:
	nResult = true;
Exit0:
	return nResult;
}
#endif

int KItemText::Init(KItemTextData &ItemTextData)
{
    int nResult = false;
    int nRetCode = false;

    nRetCode = KItemNull::Init(ItemTextData);
    KG_PROCESS_ERROR(nRetCode);

	if (ItemTextData.nText)
		SetText(KUiComponentsDecoder::GetSelf().GetDecodedText());
    
	SetRowSpacing(ItemTextData.fRowSpacing * ItemTextData.fScale);
	SetFontSpacing(ItemTextData.fFontSpacing * ItemTextData.fScale);
	SetFontScheme(ItemTextData.nFontScheme); 
    SetHAlign(ItemTextData.nHAlign);
    SetVAlign(ItemTextData.nVAlign);
    SetMultiLine(ItemTextData.nMultiLine);
	SetRichText(ItemTextData.nRichText);
	SetCenterEachLine(ItemTextData.nCenterEachLine);
	SetAutoEtc(ItemTextData.nAutoEtc);
	if (ItemTextData.dwColor & 0xFF000000)
		SetFontColor(ItemTextData.dwColor);
	if (ItemTextData.nAutoAdj)
		m_dwStyle |= ITEM_TEXT_AS_ML_LESS_ADJ;
    if (ItemTextData.nAutoSize)
	{
		if (ItemTextData.nMultiLine)
			m_dwStyle |= ITEM_TEXT_AS_ML_NO_LIMIT;
		m_dwStyle |= ITEM_TEXT_AUTO_SIZE;
	}
	else if (
		ItemTextData.nMultiLine &&
		(int)ItemTextData.fWidth <= 0 ||
		(int)ItemTextData.fHeight <= 0)
	{
		m_dwStyle |= ITEM_TEXT_AUTO_SIZE;	//未实现的AUTO_SIZE在之后实现
	}

	if (m_dwStyle & ITEM_TEXT_AUTO_SIZE)
		 AutoSize();

	if (ItemTextData.nShowAll)
		m_dwStyle |= ITEM_TEXT_SHOW_ALL;

    nResult = true;
Exit0:
    return nResult;
}

void KItemText::Draw()
{
	if (!(m_dwStyle & ITEM_SHOW))
		return;

#ifndef DISABLE_UI_EDIT
	KItemNull::Draw();
#endif

    if (!(m_dwStyle & ITEM_DATA_FORMATED))
        FormatTextForDraw();

	if (m_nShowCount > 0)
	{
		ASSERT(m_nFirstShowIndex >= 0 && m_nFirstShowIndex + m_nShowCount <= (int)m_aDrawText.size());

		if (m_dwStyle & ITEM_TEXT_SELECTED)
		{
			for (int i = m_nFirstShowIndex; i < m_nFirstShowIndex + m_nShowCount; ++i)
			{
				KG3DUIRect2 rc;
				rc.crDiffuse = m_dwSelectBgColor;
				rc.v2Min = m_aDrawText[i].v2Min;
				rc.v2Max = m_aDrawText[i].v2Max;
				g_pUI->m_p3DUI->FillMultiRect(&rc, 1);
			}
		}

		g_pUI->m_p3DUI->DrawMultiText(&m_aDrawText[m_nFirstShowIndex], m_nShowCount);
	}
   
}

DWORD KItemText::GetItemType()
{
    return ms_dwItemType;
}

int KItemText::IsItemCanChangeTo(DWORD dwType)
{
    if (
        dwType == ITEM_TEXT ||
        dwType == ITEM_NULL
    )
        return true;
    return false;
}

int KItemText::PtInItem(float fAbsX, float fAbsY)
{

#ifndef DISABLE_UI_EDIT
	if (!KDebugConfig::ms_aBoolFlag[KDebugConfig::BOOL_EDIT_MODE])
		if (!IsVisibleInWnd())
			return false;
#else
	if (!IsVisibleInWnd())
		return false;
#endif

    if (m_dwStyle & ITEM_TEXT_FORMATE_BY_OTHER)
    {
		KUITextArray::iterator it = m_aDrawText.begin() + m_nFirstShowIndex;
		KUITextArray::iterator itEnd = it + m_nShowCount;
		while (it != itEnd)
		{
			if (
				fAbsX >= (*it).v2Min.x &&
				fAbsX <= (*it).v2Max.x &&
				fAbsY >= (*it).v2Min.y &&
				fAbsY <= (*it).v2Max.y
				)
				return true;
			++it;
		}
		return false;
	}
	return KItemNull::PtInItem(fAbsX, fAbsY);

}

int KItemText::SetMultiLine(int nMultiLine)
{
	m_dwStyle &= (~ITEM_TEXT_MULTILINE);
    if (nMultiLine)
        m_dwStyle |= ITEM_TEXT_MULTILINE;

    return true;
}

int KItemText::IsMultiLine()
{
    if (m_dwStyle & ITEM_TEXT_MULTILINE)
        return true;
    return false;
}

int KItemText::SetAutoEtc(int nFlag)
{
	m_dwStyle &= (~ITEM_TEXT_AUTO_ETC);
	if (nFlag)
		m_dwStyle |= ITEM_TEXT_AUTO_ETC;

	return true;
}

int KItemText::IsAutoEtc()
{
	if (m_dwStyle & ITEM_TEXT_AUTO_ETC)
		return true;
	return false;
}


int KItemText::SetRelPos(float fRelX, float fRelY)
{
    return KItemNull::SetRelPos(fRelX, fRelY);
}

int KItemText::SetAbsPos(float fAbsX, float fAbsY)
{
    AbsMove(fAbsX - m_fAbsX, fAbsY - m_fAbsY);
    return true;
}

int KItemText::AbsMove(float fDeltaX, float fDeltaY)
{
	if (IsIntPos())
	{
		fDeltaX = floor(fDeltaX);
		fDeltaY = floor(fDeltaY);
	}

	m_fAbsX += fDeltaX;
	m_fAbsY += fDeltaY;

	KUITextArray::iterator it = m_aDrawText.begin();
	KUITextArray::iterator ie = m_aDrawText.end();
	for (; it != ie; ++it)
	{	
		(*it).v2Min.x += fDeltaX;
		(*it).v2Max.x += fDeltaX;
		(*it).v2Min.y += fDeltaY;
		(*it).v2Max.y += fDeltaY;
	}
    return true;
}

int KItemText::Scale(float fScaleX, float fScaleY)
{
	KItemNull::Scale(fScaleX, fScaleY);
	if (m_dwStyle & ITEM_NEVER_SCALED)
		return true;

	if (g_pUI->m_FontSchemeMgr.IsFontVerticalByID(m_dwFontID))
	{
		m_fRowSpacing *= fScaleX;
		m_fFontSpacing *= fScaleY;
	}
	else
	{
		m_fRowSpacing *= fScaleY;
		m_fFontSpacing *= fScaleX;
	}

	if (IsIntPos())
	{
		m_fRowSpacing = floor(m_fRowSpacing);
		m_fFontSpacing = floor(m_fFontSpacing);
	}

	if (IsAutoSize())
		AutoSize();
	
	return true;
}

int KItemText::SetFontScheme(int nFontScheme)
{
	int nResult = false;
	KFontScheme const* pFontScheme = NULL;

	pFontScheme = g_pUI->m_FontSchemeMgr.GetFontScheme(nFontScheme);
	KGLOG_PROCESS_ERROR(pFontScheme);

	m_nFontScheme = nFontScheme;

	SetFontID(pFontScheme->nFontID);
	SetFontColor(g_pUI->m_ColorSchemeMgr.GetColor(pFontScheme->nFontColorSchemeIndex));
	SetFontBorder(pFontScheme->nBorderSize, g_pUI->m_ColorSchemeMgr.GetColor(pFontScheme->nBorderColorSchemeIndex));
	SetFontShadow(pFontScheme->nShadowOffset, g_pUI->m_ColorSchemeMgr.GetColor(pFontScheme->nShadowColorSchemeIndex));
    
    m_dwStyle &= ~ITEM_DATA_FORMATED;
	nResult = true;
Exit0:
	return nResult;
}

int KItemText::GetFontScheme()
{
    return m_nFontScheme;
}

int KItemText::SetText(LPCWSTR wszText)
{
    int nResult = false;

    KG_PROCESS_ERROR(wszText);

	if (wcscmp(m_strText.c_str(), wszText) != 0)
	{
		m_strText = wszText;
		m_dwStyle &= ~ITEM_DATA_FORMATED;
	}

    nResult = true;
Exit0:
    return nResult;
}

int KItemText::AutoSize()
{
    LPCWSTR wszText = m_strText.c_str();
	float fWidth = 0.0f;
	float fHeight = 0.0f;

	int nHAlign = GetHAlign();
	int nVAlign = GetVAlign();

	SetHAlign(0);
	SetVAlign(0);

    if (wszText)
    {
		if (IsMultiLine())
		{
			if (m_dwStyle & ITEM_TEXT_AS_ML_NO_LIMIT)
			{
				SetSize(10000000.0f, 10000000.0f);
				FormatTextForDraw();
				if (!m_aDrawText.empty())
				{
					fWidth = m_aDrawText[0].v2Max.x - m_aDrawText[m_aDrawText.size() - 1].v2Min.x;
					fHeight = m_aDrawText[m_aDrawText.size() - 1].v2Max.y - m_aDrawText[0].v2Min.y;
				}
			}
			else
			{
				if (g_pUI->m_FontSchemeMgr.IsFontVerticalByID(m_dwFontID))
				{
					fHeight = m_fHeight;
					SetSize(10000000.0f, m_fHeight);
					FormatTextForDraw();
					if (!m_aDrawText.empty())
					{
						fWidth = m_aDrawText[0].v2Max.x - m_aDrawText[m_aDrawText.size() - 1].v2Min.x;
						if (m_dwStyle & ITEM_TEXT_AS_ML_LESS_ADJ)
							fHeight = m_aDrawText[m_aDrawText.size() - 1].v2Max.y - m_aDrawText[0].v2Min.y;
					}
				}
				else
				{
					fWidth = m_fWidth;
					SetSize(m_fWidth, 10000000.0f);
					FormatTextForDraw();
					if (!m_aDrawText.empty())
					{
						if (m_dwStyle & ITEM_TEXT_AS_ML_LESS_ADJ)
							fWidth = m_aDrawText[0].v2Max.x - m_aDrawText[m_aDrawText.size() - 1].v2Min.x;
						fHeight = m_aDrawText[m_aDrawText.size() - 1].v2Max.y - m_aDrawText[0].v2Min.y;
					}
				}
			}
		}
		else
		{
			g_pUI->m_p3DUI->GetTextExtent(m_dwFontID, wszText, -1, fWidth, fHeight, m_fFontSpacing, m_fFontScale, m_byBorder);
		}
    }

    SetSize(fWidth, fHeight);

	SetHAlign(nHAlign);
	SetVAlign(nVAlign);
	m_dwStyle |= ITEM_TEXT_AUTO_SIZE;
    return true;
}

int KItemText::FormatTextForDraw()
{
    m_aDrawText.clear();
	int nVertical = g_pUI->m_FontSchemeMgr.IsFontVerticalByID(m_dwFontID);
    if (m_dwStyle & ITEM_TEXT_MULTILINE)
	{
		if (nVertical)
			FormatMultiLineVertical();
		else
			FormatMultiLineHorizontal();
	}
    else
	{
		if (nVertical)
			FormatSingleLineVertical();
		else
			FormatSingleLineHorizontal();
	}

	m_nFirstShowIndex = 0;
	m_nShowCount = (int)m_aDrawText.size();

	m_dwStyle &= ~ITEM_TEXT_FORMATE_BY_OTHER;
	m_dwStyle |= ITEM_DATA_FORMATED;
	return true;
}

int KItemText::FormatSingleLineHorizontal()
{
    int nResult = false;
    int nShowCharacterCount = 0;
    float fShowTextWidth = 0.0f;  
	float fShowTextHeight = 0.0f;
    float fX = m_fAbsX; 
    float fY = m_fAbsY;
    LPCWSTR wszSrc = m_strText.c_str();
	KG3DUIText2 UIText;

    KG_PROCESS_ERROR(wszSrc && wszSrc[0] != L'\0');

    if (m_dwStyle & ITEM_TEXT_SHOW_ALL)
    {
		nShowCharacterCount = (int)m_strText.size();
        KG_PROCESS_ERROR(nShowCharacterCount > 0);
    }
	else
    {
        g_pUI->m_p3DUI->GetTextPosExtent(m_dwFontID, wszSrc, -1, m_fWidth, &nShowCharacterCount, m_fFontSpacing, m_fFontScale, m_byBorder);
        KG_PROCESS_ERROR(nShowCharacterCount > 0);
    }

	g_pUI->m_p3DUI->GetTextExtent(m_dwFontID, wszSrc, nShowCharacterCount, fShowTextWidth, fShowTextHeight, m_fFontSpacing, m_fFontScale, m_byBorder);

    switch (m_dwStyle & ITEM_TEXT_HALIGN_FILTER) 
    {
    case ITEM_TEXT_HALIGN_CENTER:
        fX += ((m_fWidth - fShowTextWidth) / 2.0f);
        break;
    case ITEM_TEXT_HALIGN_RIGHT:
        fX += (m_fWidth - fShowTextWidth);
        break;
    }
    
    switch(m_dwStyle & ITEM_TEXT_VALIGN_FILTER) 
    {
    case ITEM_TEXT_VALIGN_CENTER:
        fY += ((m_fHeight - fShowTextHeight) / 2.0f);
        break;
    case ITEM_TEXT_VALIGN_BOTTOM:
        fY += (m_fHeight - fShowTextHeight);
        break;
    }

	UIText.dwFontID = m_dwFontID;
	UIText.crFont = m_dwFontColor;
	UIText.crBorder = m_dwBorderColor;
	UIText.crShadow = m_dwShadowColor;
	UIText.byBorder = m_byBorder;
	UIText.byShadow = m_byShadow;
	UIText.byAlpha = static_cast<BYTE>(m_dwAlpha);
	UIText.fSpacing = m_fFontSpacing;
	UIText.fScale = m_fFontScale;
	UIText.v2Min.y = fY;
	UIText.v2Max.y = fY + GetSingleTextHeight();

	while (true)
	{
		int nLen = KG3DUI_TEXT_LEN - 2;
		if (nShowCharacterCount < nLen)
			nLen = nShowCharacterCount;

        if (nLen == 0)
            break;

		wcsncpy(UIText.wszText, wszSrc, nLen);
		UIText.wszText[nLen] = L'\0';
		UIText.nTextLength = nLen;
		UIText.v2Min.x = fX;
		
		if (nShowCharacterCount <= nLen)
		{
			if (m_dwStyle & ITEM_TEXT_AUTO_ETC && wszSrc[nLen] != L'\0') //显示不完，补省略号
				*(UIText.wszText + wcslen(UIText.wszText) - 1) = L'…';
			g_pUI->m_p3DUI->GetTextExtent(m_dwFontID, UIText.wszText, UIText.nTextLength, fShowTextWidth, fShowTextHeight, m_fFontSpacing, m_fFontScale, m_byBorder);
			UIText.v2Max.x = fX + fShowTextWidth;
			m_aDrawText.push_back(UIText);
			break;
		}

		g_pUI->m_p3DUI->GetTextExtent(m_dwFontID, UIText.wszText, UIText.nTextLength, fShowTextWidth, fShowTextHeight, m_fFontSpacing, m_fFontScale, m_byBorder);
		UIText.v2Max.x = fX + fShowTextWidth;
		m_aDrawText.push_back(UIText);

		fX = UIText.v2Max.x + m_fFontSpacing;
		nShowCharacterCount -= nLen;

		wszSrc += nLen;
	}

    nResult = true;
Exit0:
    return nResult;
}

int KItemText::FormatSingleLineVertical()
{
	int nResult = false;
	int nShowCharacterCount = 0;
	float fShowTextWidth = 0.0f;  
	float fShowTextHeight = 0.0f;
	float fX = m_fAbsX; 
	float fY = m_fAbsY;
	LPCWSTR wszSrc = m_strText.c_str();
    LPCWSTR wszIter = NULL;
	KG3DUIText2 UIText;

	KG_PROCESS_ERROR(wszSrc && wszSrc[0] != L'\0');

	if (m_dwStyle & ITEM_TEXT_SHOW_ALL)
    {
        nShowCharacterCount = (int)m_strText.size();
        KG_PROCESS_ERROR(nShowCharacterCount > 0);
    }
	else
    {
        g_pUI->m_p3DUI->GetTextPosExtent(m_dwFontID, wszSrc, -1, m_fHeight, &nShowCharacterCount, m_fFontSpacing, m_fFontScale, m_byBorder);
        KG_PROCESS_ERROR(nShowCharacterCount > 0);
    }

	g_pUI->m_p3DUI->GetTextExtent(m_dwFontID, wszSrc, nShowCharacterCount, fShowTextWidth, fShowTextHeight, m_fFontSpacing, m_fFontScale, m_byBorder);

	switch (m_dwStyle & ITEM_TEXT_HALIGN_FILTER) 
	{
	case ITEM_TEXT_HALIGN_CENTER:
		fX +=((m_fWidth - fShowTextWidth) / 2.0f);
		break;
	case ITEM_TEXT_HALIGN_RIGHT:
		fX += (m_fWidth - fShowTextWidth);
		break;
	}

	switch (m_dwStyle & ITEM_TEXT_VALIGN_FILTER) 
	{
	case ITEM_TEXT_VALIGN_CENTER:
		fY += ((m_fHeight - fShowTextHeight) / 2.0f);
		break;
	case ITEM_TEXT_VALIGN_BOTTOM:
		fY += (m_fHeight - fShowTextHeight);
		break;
	}

	UIText.dwFontID = m_dwFontID;
	UIText.crFont = m_dwFontColor;
	UIText.crBorder = m_dwBorderColor;
	UIText.crShadow = m_dwShadowColor;
	UIText.byBorder = m_byBorder;
	UIText.byShadow = m_byShadow;
	UIText.byAlpha = (BYTE)m_dwAlpha;
	UIText.fSpacing = m_fFontSpacing;
	UIText.fScale = m_fFontScale;
	UIText.nTextLength = -1;
	UIText.v2Min.x = fX;
	UIText.v2Max.x = fX + GetSingleTextHeight();

    wszIter = wszSrc;

	while(true)
	{
		int nLen = KG3DUI_TEXT_LEN - 2;
		if (nShowCharacterCount < nLen)
			nLen = nShowCharacterCount;

        if (nLen == 0)
            break;

		wcsncpy(UIText. wszText, wszIter, nLen);
		UIText.wszText[nLen] = L'\0';
		UIText.nTextLength = nLen;
		UIText.v2Min.y = fY;

		if (nShowCharacterCount <= nLen)
		{
			if (m_dwStyle & ITEM_TEXT_AUTO_ETC && wszSrc[nLen] != L'\0') //显示不完，补省略号
				*(UIText.wszText + wcslen(UIText.wszText) - 1) = L'…';
			g_pUI->m_p3DUI->GetTextExtent(m_dwFontID, UIText.wszText, UIText.nTextLength, fShowTextWidth, fShowTextHeight, m_fFontSpacing, m_fFontScale, m_byBorder);
			UIText.v2Max.y = fY + fShowTextHeight;
			m_aDrawText.push_back(UIText);
			break;
		}

		g_pUI->m_p3DUI->GetTextExtent(m_dwFontID, UIText.wszText, UIText.nTextLength, fShowTextWidth, fShowTextHeight, m_fFontSpacing, m_fFontScale, m_byBorder);
		UIText.v2Max.y = fY + fShowTextHeight;
		m_aDrawText.push_back(UIText);

		fY = UIText.v2Max.y + m_fFontSpacing;
		nShowCharacterCount -= nLen;

		wszIter += nLen;
	}

	nResult = true;
Exit0:
	return nResult;
}

int KItemText::FormatMultiLineHorizontal()
{
	int nResult = false;
	int nShowCharacterCount = 0;
	int nShowRowCount = 0;
	float fShowTextWidth = 0.0f;  
	float fShowTextHeight = 0.0f;
	float fRowHeight = 0.0f;
	float fX = m_fAbsX; 
	float fY = 0.0f;
	LPCWSTR wszSrc = m_strText.c_str();
    LPCWSTR wszIter = NULL;
	int nEnterKeyPos = 0;
	KG3DUIText2 UIText;

	KG_PROCESS_ERROR(wszSrc && wszSrc[0] != L'\0');

	fRowHeight = GetSingleTextHeight();

    wszIter = wszSrc;

	fY = fRowHeight;
	while (1) 
	{
		g_pUI->m_p3DUI->GetTextPosExtent(m_dwFontID, wszIter, -1, m_fWidth, &nShowCharacterCount, m_fFontSpacing, m_fFontScale, m_byBorder);
		if (g_IsCharShouldPosedInTheEnd(*(wszIter + nShowCharacterCount)))
			nShowCharacterCount++;

		if (nShowCharacterCount <= 0)
			break;
		if (!(m_dwStyle & ITEM_TEXT_SHOW_ALL) && UI_FLOAT_LARGE(fY, m_fHeight))
			break;

		if (g_GetNextNewLineKeyW(wszIter, nEnterKeyPos) == ITEM_TEXT_CONTROL_KEY_NEW_LINE)
		{
			if (nEnterKeyPos < nShowCharacterCount)
				nShowCharacterCount = nEnterKeyPos + 1;		
		}

		++nShowRowCount;
		wszIter += nShowCharacterCount;
		fY += (fRowHeight + m_fRowSpacing);
	}
	fY = m_fAbsY;

    wszIter = wszSrc;

	switch (m_dwStyle & ITEM_TEXT_VALIGN_FILTER) 
	{
	case ITEM_TEXT_VALIGN_CENTER:
		fY += ((m_fHeight - (fRowHeight + m_fRowSpacing) * nShowRowCount + m_fRowSpacing) / 2.0f);
		break;
	case ITEM_TEXT_VALIGN_BOTTOM:
		fY += (m_fHeight - (fRowHeight + m_fRowSpacing) * nShowRowCount + m_fRowSpacing);
		break;
	}

	UIText.dwFontID = m_dwFontID;
	UIText.crFont = m_dwFontColor;
	UIText.crBorder = m_dwBorderColor;
	UIText.crShadow = m_dwShadowColor;
	UIText.byBorder = m_byBorder;
	UIText.byShadow = m_byShadow;
	UIText.byAlpha = static_cast<BYTE>(m_dwAlpha);
	UIText.fSpacing = m_fFontSpacing;
	UIText.fScale = m_fFontScale;
	UIText.nTextLength = -1;

	for (int i = 0; i < nShowRowCount; ++i) 
	{
		g_pUI->m_p3DUI->GetTextPosExtent(m_dwFontID, wszIter, -1, m_fWidth, &nShowCharacterCount, m_fFontSpacing, m_fFontScale, m_byBorder);
		if (g_IsCharShouldPosedInTheEnd(*(wszIter + nShowCharacterCount)))
			nShowCharacterCount++;

		if (g_GetNextNewLineKeyW(wszIter, nEnterKeyPos) == ITEM_TEXT_CONTROL_KEY_NEW_LINE)
		{
			if (nEnterKeyPos < nShowCharacterCount)
			{
				nShowCharacterCount = nEnterKeyPos + 1;
			}
		}

		g_pUI->m_p3DUI->GetTextExtent(m_dwFontID, wszIter, nShowCharacterCount, fShowTextWidth, fShowTextHeight, m_fFontSpacing, m_fFontScale, m_byBorder);

		switch (m_dwStyle & ITEM_TEXT_HALIGN_FILTER) 
		{
		case ITEM_TEXT_HALIGN_CENTER:
			if ((m_dwStyle & ITEM_TEXT_CENTER_EACH_ROW) || (i == nShowRowCount - 1))
				fX +=((m_fWidth - fShowTextWidth) / 2.0f);
			break;
		case ITEM_TEXT_HALIGN_RIGHT:
			fX += (m_fWidth - fShowTextWidth);
			break;
		}

		while (nShowCharacterCount > 0)
		{
			int nLen = KG3DUI_TEXT_LEN - 2;
			if (nShowCharacterCount < nLen)
				nLen = nShowCharacterCount;

            if (nLen == 0)
                break;

			wcsncpy(UIText.wszText, wszIter, nLen);

			UIText.wszText[nLen] = L'\0';
			UIText.nTextLength = nLen;

			UIText.v2Min = D3DXVECTOR2(fX, fY);

			g_pUI->m_p3DUI->GetTextExtent(m_dwFontID, wszIter, nLen, fShowTextWidth, fShowTextHeight, m_fFontSpacing, m_fFontScale, m_byBorder);

			UIText.v2Max = D3DXVECTOR2(fX + fShowTextWidth, fY + fRowHeight);
			m_aDrawText.push_back(UIText);

			fX = UIText.v2Max.x + m_fFontSpacing;
			nShowCharacterCount -= nLen;
			wszIter += nLen;
		}
		fX = m_fAbsX;
		fY += (m_fRowSpacing + fRowHeight);
	}

	if (m_dwStyle & ITEM_TEXT_AUTO_ETC && m_aDrawText.size() && *(wszIter) != '\0') //显示不完，补省略号
	{
		KG3DUIText2& UITextLast = m_aDrawText[m_aDrawText.size() - 1];
		*(UITextLast.wszText + wcslen(UITextLast.wszText) - 1) = L'…';
	}

	nResult = true;
Exit0:
	return nResult;
}

int KItemText::FormatMultiLineVertical()
{
	int nResult = false;
	int nShowCharacterCount = 0;
	int nShowRowCount = 0;
	float fShowTextWidth = 0.0f;  
	float fShowTextHeight = 0.0f;
	float fRowHeight = 0.0f;
	float fX = m_fWidth; 
	float fY = m_fAbsY;
	LPCWSTR wszSrc = m_strText.c_str();
    LPCWSTR wszIter = NULL;
	int nEnterKeyPos = 0;
	KG3DUIText2 UIText;

	KG_PROCESS_ERROR(wszSrc && wszSrc[0] != L'\0');

    wszIter = wszSrc;

	fRowHeight = GetSingleTextHeight();

	fX -= fRowHeight;
	while (1) 
	{
		g_pUI->m_p3DUI->GetTextPosExtent(m_dwFontID, wszIter, -1, m_fHeight, &nShowCharacterCount, m_fFontSpacing, m_fFontScale, m_byBorder);
		if (g_IsCharShouldPosedInTheEnd(*(wszIter + nShowCharacterCount)))
			nShowCharacterCount++;

		if (nShowCharacterCount <= 0)
			break;
		if (!(m_dwStyle & ITEM_TEXT_SHOW_ALL) && UI_FLOAT_SMALL(fX, 0.0f))
			break;

		if (g_GetNextNewLineKeyW(wszIter, nEnterKeyPos) == ITEM_TEXT_CONTROL_KEY_NEW_LINE)
		{
			if (nEnterKeyPos < nShowCharacterCount)
				nShowCharacterCount = nEnterKeyPos + 1;		
		}
		++nShowRowCount;
		wszIter += nShowCharacterCount;
		fX -= (fRowHeight + m_fRowSpacing);
	}
	fX = m_fAbsX;

    wszIter = wszSrc;

	switch (m_dwStyle & ITEM_TEXT_HALIGN_FILTER) 
	{
	case ITEM_TEXT_HALIGN_CENTER:
		fX += (m_fWidth - (m_fWidth - (fRowHeight + m_fRowSpacing) * nShowRowCount + m_fRowSpacing) / 2.0f);
		break;
	case ITEM_TEXT_HALIGN_RIGHT:
		fX += m_fWidth;
		break;
	default:
		fX += ((fRowHeight + m_fRowSpacing) * nShowRowCount - m_fRowSpacing);
		break;
	}

	UIText.dwFontID = m_dwFontID;
	UIText.crFont = m_dwFontColor;
	UIText.crBorder = m_dwBorderColor;
	UIText.crShadow = m_dwShadowColor;
	UIText.byBorder = m_byBorder;
	UIText.byShadow = m_byShadow;
	UIText.byAlpha = static_cast<BYTE>(m_dwAlpha);
	UIText.fSpacing = m_fFontSpacing;
	UIText.fScale = m_fFontScale;
	UIText.nTextLength = -1;

	for (int i = 0; i < nShowRowCount; ++i) 
	{
		g_pUI->m_p3DUI->GetTextPosExtent(m_dwFontID, wszIter, -1, m_fHeight, &nShowCharacterCount, m_fFontSpacing, m_fFontScale, m_byBorder);
		if (g_IsCharShouldPosedInTheEnd(*(wszIter + nShowCharacterCount)))
			nShowCharacterCount++;

		if (g_GetNextNewLineKeyW(wszIter, nEnterKeyPos) == ITEM_TEXT_CONTROL_KEY_NEW_LINE)
		{
			if (nEnterKeyPos < nShowCharacterCount)
				nShowCharacterCount = nEnterKeyPos + 1;
		}
		g_pUI->m_p3DUI->GetTextExtent(m_dwFontID, wszIter, nShowCharacterCount, fShowTextWidth, fShowTextHeight, m_fFontSpacing, m_fFontScale, m_byBorder);

		switch (m_dwStyle & ITEM_TEXT_VALIGN_FILTER) 
		{
		case ITEM_TEXT_VALIGN_CENTER:
			if ((m_dwStyle & ITEM_TEXT_CENTER_EACH_ROW) || (i == nShowRowCount - 1))
				fY +=((m_fHeight - fShowTextHeight) / 2.0f);
			break;
		case ITEM_TEXT_VALIGN_BOTTOM:
			fY += (m_fHeight - fShowTextHeight);
			break;
		}

		while (nShowCharacterCount > 0)
		{
			int nLen = KG3DUI_TEXT_LEN - 2;
			if (nShowCharacterCount < nLen)
				nLen = nShowCharacterCount;

            if (nLen == 0)
                break;

			wcsncpy(UIText.wszText, wszIter, nLen);
			UIText.wszText[nLen] = L'\0';
			UIText.nTextLength = nLen;

			UIText.v2Min = D3DXVECTOR2(fX - fRowHeight, fY);

			g_pUI->m_p3DUI->GetTextExtent(m_dwFontID, wszIter, nLen, fShowTextWidth, fShowTextHeight, m_fFontSpacing, m_fFontScale, m_byBorder);

			UIText.v2Max = D3DXVECTOR2(fX, fY + fShowTextHeight);
			m_aDrawText.push_back(UIText);

			fY = UIText.v2Max.y + m_fFontSpacing;
			nShowCharacterCount -= nLen;
			wszIter += nLen;
		}
		fX -= (m_fRowSpacing + fRowHeight);
		fY = m_fAbsY;
	}
	if (m_dwStyle & ITEM_TEXT_AUTO_ETC && m_aDrawText.size() && *wszIter != L'\0') //显示不完，补省略号
	{
		KG3DUIText2 &UITextLast = m_aDrawText[m_aDrawText.size() - 1];
		*(UITextLast.wszText + wcslen(UITextLast.wszText) - 1) = L'…';
	}

	nResult = true;
Exit0:
	return nResult;
}

LPCWSTR KItemText::GetText() const
{
    return m_strText.c_str();
}

size_t KItemText::GetTextLength()
{
    return m_strText.size();
}

int KItemText::SetHAlign(int nAlignType)
{
    m_dwStyle &= ~ITEM_TEXT_HALIGN_FILTER;
    switch(nAlignType) 
    {
    case 1:
        m_dwStyle |= ITEM_TEXT_HALIGN_CENTER;
    	break;
    case 2:
        m_dwStyle |= ITEM_TEXT_HALIGN_RIGHT;
        break;
    }

    return true;
}

int KItemText::GetHAlign()
{
    int nResult = 0;
    switch(m_dwStyle & ITEM_TEXT_HALIGN_FILTER) 
    {
    case ITEM_TEXT_HALIGN_CENTER:
        nResult = 1;
    	break;
    case ITEM_TEXT_HALIGN_RIGHT:
        nResult = 2;
        break;
    }
    return nResult;
}

int KItemText::SetCenterEachLine(int nCenterEachLine)
{
	m_dwStyle &= ~ITEM_TEXT_CENTER_EACH_ROW;
	if (nCenterEachLine)
		m_dwStyle |= ITEM_TEXT_CENTER_EACH_ROW;

	return true;
}

int KItemText::IsCenterEachLine()
{
	if (m_dwStyle & ITEM_TEXT_CENTER_EACH_ROW)
		return true;
	return false;
}

int KItemText::SetVAlign(int nAlignType)
{
    m_dwStyle &= ~ITEM_TEXT_VALIGN_FILTER;
    switch(nAlignType) 
    {
    case 1:
        m_dwStyle |= ITEM_TEXT_VALIGN_CENTER;
    	break;
    case 2:
        m_dwStyle |= ITEM_TEXT_VALIGN_BOTTOM;
        break;
    }

    return true;
}

int KItemText::GetVAlign()
{
    int nResult = 0;
    switch(m_dwStyle & ITEM_TEXT_VALIGN_FILTER) 
    {
    case ITEM_TEXT_VALIGN_CENTER:
        nResult = 1;
    	break;
    case ITEM_TEXT_VALIGN_BOTTOM:
        nResult = 2;
        break;
    }
    return nResult;
}

int KItemText::SetRowSpacing(float fSpacing)
{
    m_fRowSpacing = fSpacing;

	if (IsIntPos())
	{
		m_fRowSpacing = floor(m_fRowSpacing);
	}

	m_dwStyle &= ~ITEM_DATA_FORMATED;
	
	return true;
}

float KItemText::GetRowSpacing()
{
    return m_fRowSpacing;
}

int KItemText::SetFontSpacing(float fSpacing)
{
	m_fFontSpacing = fSpacing;
	if (IsIntPos())
	{
		m_fFontSpacing = floor(m_fFontSpacing);
	}

	m_dwStyle &= ~ITEM_DATA_FORMATED;

	return true;
}

float KItemText::GetFontSpacing()
{
	return m_fFontSpacing;
}

int KItemText::SetAlpha(int nAlpha)
{
	int nFotmated = m_dwStyle & ITEM_DATA_FORMATED;
	KItemNull::SetAlpha(nAlpha);

	if (nFotmated)
	{
		KUITextArray::iterator it = m_aDrawText.begin();
		KUITextArray::iterator ie = m_aDrawText.end();
		for (; it != ie; ++it)
		{
			(*it).byAlpha = static_cast<BYTE>(m_dwAlpha);
		}
		m_dwStyle |= ITEM_DATA_FORMATED;
	}

	return true;
}

int KItemText::ClearDrawTextData()
{
	m_aDrawText.clear();
	m_nFirstShowIndex = 0;
	m_nShowCount = 0;

	m_fAbsX = 0;
	m_fAbsY = 0;
	m_fWidth = 0;
	m_fHeight = 0;
	return true;
}

int KItemText::AppendDrawTextData(D3DXVECTOR2 vMin, D3DXVECTOR2 vMax, LPCWSTR wszSrc, unsigned uLen)
{
	KG3DUIText2 UIText;
	ASSERT(wszSrc);

	switch (*wszSrc)
	{
	case L'\n':
    case L'\t':
		KG_PROCESS_SUCCESS(*(wszSrc + 1) == L'\0');
		break;
	case L'\0':
		goto Exit1;
		break;
	}
	if (!uLen)
		goto Exit1;
	ASSERT(uLen < KG3DUI_TEXT_LEN - 1);

	UIText.dwFontID = m_dwFontID;
	UIText.crFont = m_dwFontColor;
	UIText.crBorder = m_dwBorderColor;
	UIText.crShadow = m_dwShadowColor;
	UIText.byBorder = m_byBorder;
	UIText.byShadow = m_byShadow;
	UIText.fScale = m_fFontScale;
	UIText.byAlpha = static_cast<BYTE>(m_dwAlpha);
	UIText.fSpacing = m_fFontSpacing;

	UIText.nTextLength = uLen;
	UIText.v2Min = vMin;
	UIText.v2Max = vMax;

	wcsncpy(UIText.wszText, wszSrc, uLen);
	UIText.wszText[uLen] = L'\0';

	m_aDrawText.push_back(UIText);
	++m_nShowCount;

	if ((int)m_aDrawText.size() == 1)
	{
		//如果是第一个数据
		m_fAbsX = vMin.x;
		m_fAbsY = vMin.y;
		m_fWidth = vMax.x - vMin.x;
		m_fHeight = vMax.y - vMin.y;
	}
	else
	{
		//if (UI_FLOAT_LARGE(vMax.x, m_fAbsX + m_fWidth))
		m_fWidth = max(vMax.x, m_fAbsX + m_fWidth) - min(m_fAbsX, vMin.x);
		if (UI_FLOAT_SMALL(vMin.x, m_fAbsX))
			m_fAbsX = vMin.x;
		//不需要的，现在的情况是条件一定不成立，可能以后有需要
		//if (UI_FLOAT_SMALL(UIText.v2Min.y, m_fAbsY))
		//	m_fAbsY = UIText.v2Min.y;
		//现在的情况是这这个条件一定满足。
		//if (UI_FLOAT_LARGE(UIText.v2Max.y, m_fAbsY + m_fHeight))
			m_fHeight = vMax.y - m_fAbsY;
	}

Exit1:
	m_dwStyle |= ITEM_DATA_FORMATED;
	m_dwStyle |= ITEM_TEXT_FORMATE_BY_OTHER;
	return true;
}

static int GetNextControlKeyW(LPCWSTR wszText, int &nControlKeyPos)
{
	LPCWSTR wszSearch = wszText;
	if (!wszSearch)
		return ITEM_TEXT_CONTROL_KEY_NONE;

	while (wszSearch && *wszSearch != L'\0') 
	{
		switch (*wszSearch) 
		{
		case L'\t':
			nControlKeyPos = (int)(wszSearch - wszText);
			return ITEM_TEXT_CONTROL_KEY_TAB;
		case L'\n':
			nControlKeyPos = (int)(wszSearch - wszText);
			return ITEM_TEXT_CONTROL_KEY_NEW_LINE;
		}
		wszSearch = ::CharNextW(wszSearch);
	}
	return ITEM_TEXT_CONTROL_KEY_NONE;
}


int KItemText::GetSplitPos(LPCWSTR wszSrc, float &fPos, int &nType) const
{
	int nPos = -1;

	if (!wszSrc || wszSrc[0] == L'\0')
	{
		fPos = 0.0f;
		nType = ITEM_TEXT_CONTROL_KEY_NONE;
		return nPos;
	}

	float fWidth = 0.0f;
	float fHeight = 0.0f;
	float fScale = m_fFontScale;

	nType = GetNextControlKeyW(wszSrc, nPos);
	switch (nType)
	{
	case ITEM_TEXT_CONTROL_KEY_NEW_LINE:
		if (nPos < KG3DUI_TEXT_LEN - 1)
		{
			//存在换行符并且换行符所在的位置在一块绘制数据的范围内。
			g_pUI->m_p3DUI->GetTextExtent(m_dwFontID, wszSrc, nPos, fWidth, fHeight, m_fFontSpacing, fScale, m_byBorder);
			if(UI_FLOAT_SMALL_OR_EQUAL(fWidth, fPos))
			{
				//换行符所在的位置小于等于这一行，所有数据都在这一行并且应该在这一行换行。	
				fPos = fWidth + m_fFontSpacing;
				return nPos;
			}
		}
		else
		{
			//换行符在一个字段之外，当作没有换行符处理。
		}
		break;
	case ITEM_TEXT_CONTROL_KEY_TAB:	//同样代码，保留，稳定后如果还是一致，可以合并。
		if (nPos < KG3DUI_TEXT_LEN - 1)
		{
			//存在制表符并且制表符所在的位置在一块绘制数据的范围内。
			g_pUI->m_p3DUI->GetTextExtent(m_dwFontID, wszSrc, nPos, fWidth, fHeight, m_fFontSpacing, fScale, m_byBorder);
			if (UI_FLOAT_SMALL_OR_EQUAL(fWidth, fPos))
			{
				//制表符所在的位置小于等于这一行，所有数据都在这一行并且应该在这一行加一个制表符。
				fPos = fWidth + m_fFontSpacing;
				return nPos;
			}
			else
			{
				//制表符所在的位置大于这一行，当作没有制表符的情况处理。
			}
		}
		else
		{
			//制表符在一个字段之外，当作没有制表符处理。
		}
		break;
	default:
		//没有换行符或者制表符。
		break;
	}

	nType = ITEM_TEXT_CONTROL_KEY_NONE;

	//取得一行的字数。
	g_pUI->m_p3DUI->GetTextPosExtent(m_dwFontID, wszSrc, -1, fPos, &nPos, m_fFontSpacing, fScale, m_byBorder);	
	if (g_IsCharShouldPosedInTheEnd(*(wszSrc + nPos)))
		nPos++;
	if (nPos > KG3DUI_TEXT_LEN - 2)
		nPos = KG3DUI_TEXT_LEN - 2;

	g_pUI->m_p3DUI->GetTextExtent(m_dwFontID, wszSrc, nPos, fWidth, fHeight, m_fFontSpacing, fScale, m_byBorder);

	fPos = fWidth;

	if (*(wszSrc + nPos) == L'\n')
		nType = ITEM_TEXT_CONTROL_KEY_NEW_LINE;

	return nPos;
}

float KItemText::GetSingleTextHeight() const
{
	float fHeight = 12.0f;
	if (g_pUI->m_FontSchemeMgr.IsFontVerticalByID(m_dwFontID))
		g_pUI->m_p3DUI->GetCharWidth(m_dwFontID, m_fFontScale, fHeight);
	else
		g_pUI->m_p3DUI->GetCharHeight(m_dwFontID, m_fFontScale, fHeight);
	return fHeight;
}

float KItemText::GetTableTextWidth() const
{
	float fWidth = 12.0f;
	if (g_pUI->m_FontSchemeMgr.IsFontVerticalByID(m_dwFontID))
		g_pUI->m_p3DUI->GetCharHeight(m_dwFontID, m_fFontScale, fWidth);
	else
		g_pUI->m_p3DUI->GetCharWidth(m_dwFontID, m_fFontScale, fWidth);

	return fWidth * 2;
}

int KItemText::SetRichText(int nRichText)
{
	m_dwStyle |= ITEM_TEXT_NOT_RICH_TEXT;
	if (nRichText)
		m_dwStyle &= ~ITEM_TEXT_NOT_RICH_TEXT;
	return true;
}

int KItemText::IsRichText()
{
	if (m_dwStyle &ITEM_TEXT_NOT_RICH_TEXT)
		return false;
	return true;
}

int KItemText::AdjustDrawDataEndAbsY(float fOldY, float fNewY)
{
	size_t uCount = m_aDrawText.size();
	if (!uCount)
	{
		if (UI_FLOAT_EQUAL(m_fAbsY, fOldY))
			m_fAbsY = fNewY;
		m_fHeight = 0.0f;
		return true;
	}

	KUITextArray::iterator it = m_aDrawText.begin();
	KUITextArray::iterator ie = m_aDrawText.end();
	for (; it != ie; ++it)
	{
		if (UI_FLOAT_EQUAL((*it).v2Max.y, fOldY))
		{
			(*it).v2Min.y += (fNewY - fOldY);
			(*it).v2Max.y = fNewY;
		}
	}

	m_fAbsY = m_aDrawText[0].v2Min.y;
	m_fHeight = m_aDrawText[uCount - 1].v2Max.y - m_fAbsY; 

	return true;
}

int KItemText::AdjustShowTextV(float fAbsTop, float fAbsBottom)
{
	int nCount = (int)m_aDrawText.size();
	m_nFirstShowIndex = -1;
	m_nShowCount = nCount;

	for (int i = 0; i < nCount; ++i)
	{
		if (UI_FLOAT_LARGE_OR_EQUAL(m_aDrawText[i].v2Min.y, fAbsTop) && 
			m_nFirstShowIndex == -1
		)
			m_nFirstShowIndex = i;

		if (UI_FLOAT_LARGE(m_aDrawText[i].v2Max.y, fAbsBottom))
		{
			m_nShowCount = i;
			break;
		}
	}

	if (m_nFirstShowIndex == -1)
	{
		m_nFirstShowIndex = 0;
		m_nShowCount = 0;
	}
	else
	{
		m_nShowCount = m_nShowCount - m_nFirstShowIndex;
		ASSERT(m_nShowCount >= 0 && m_nShowCount <= nCount);
	}

	if (m_nShowCount)
		Show();
	else
		Hide();

	return true;
}

int KItemText::AdjustShowTextH(float fAbsLeft, float fAbsRight)
{
	int nCount = (int)m_aDrawText.size();
	m_nFirstShowIndex = -1;
	m_nShowCount = nCount;

	for (int i = 0; i < nCount; ++i)
	{
		if (UI_FLOAT_LARGE_OR_EQUAL(m_aDrawText[i].v2Min.x, fAbsLeft) && 
			m_nFirstShowIndex == -1
			)
			m_nFirstShowIndex = i;

		if (UI_FLOAT_LARGE(m_aDrawText[i].v2Max.x, fAbsRight))
		{
			m_nShowCount = i;
			break;
		}
	}

	if (m_nFirstShowIndex == -1)
	{
		m_nFirstShowIndex = 0;
		m_nShowCount = 0;
	}
	else
	{
		m_nShowCount = m_nShowCount - m_nFirstShowIndex;
		ASSERT(m_nShowCount >= 0 && m_nShowCount <= nCount);
	}

	if (m_nShowCount)
		Show();
	else
		Hide();

	return true;
}

int	KItemText::AdjustShowAll()
{
	m_nFirstShowIndex = 0;
	m_nShowCount = (int)m_aDrawText.size();
	Show();
	return true;
}


int KItemText::IsAutoSize()
{
	if (m_dwStyle & ITEM_TEXT_AUTO_SIZE)
		return true;
	return false;
}

int	KItemText::GetLastDrawBlockInfo(float &fEndX, float &fEndY, LPCWSTR* pwszEndControlKey)
{
	int nResult = false;
	int nCount = (int)m_aDrawText.size();
	LPCWSTR wszText = NULL;
	LPCWSTR wszClip = NULL;

	KG_PROCESS_ERROR(nCount);

	ASSERT(pwszEndControlKey);

	fEndX = m_aDrawText[nCount - 1].v2Max.x;
	fEndY = m_aDrawText[nCount - 1].v2Max.y;

	wszText = m_strText.c_str();

	wszClip = wszText + m_strText.length();

	*pwszEndControlKey = wszClip;

	while (wszClip && wszClip != wszText)
	{
		switch (*wszClip)
		{
		case L'\n':
		case L'\t':
		case L'\0':
			*pwszEndControlKey = wszClip;
			wszClip = ::CharPrevW(wszText, wszClip);
			break;
		default:
			goto Exit1;
		}
	}
	if (wszClip)
	{
		switch (*wszClip)
		{
		case L'\n':
		case L'\t':
		case L'\0':
			*pwszEndControlKey = wszClip;
			break;
		}
	}

Exit1:
	nResult = true;
Exit0:
	return nResult;
}


int	KItemText::SetSelected(int nSelected)
{
	m_dwStyle &= ~ITEM_TEXT_SELECTED;
	if (nSelected)
		m_dwStyle |= ITEM_TEXT_SELECTED;

	return true;
}

int	KItemText::SetSelBgColor(DWORD dwColor)
{
	m_dwSelectBgColor = dwColor;

	return true;
}

int	KItemText::SetFontScale(float fScale)
{
	if (fScale < 0.0f)
		fScale = 0.0f;
	m_fFontScale = fScale;
	m_dwStyle &= ~ITEM_DATA_FORMATED;
	return true;
}

float KItemText::GetFontScale()
{
	return m_fFontScale;
}

int	KItemText::SetFontID(DWORD dwID)
{
	if (g_pUI->m_FontSchemeMgr.IsFontLoaded(dwID))
	{
		m_dwFontID = dwID;
		return true;
	}
	m_dwStyle &= ~ITEM_DATA_FORMATED;
	return false;
}

int	KItemText::SetFontColor(DWORD dwColor)
{
	m_dwFontColor = dwColor;
	if (m_dwStyle & ITEM_DATA_FORMATED)
	{
		KUITextArray::iterator it = m_aDrawText.begin();
		KUITextArray::iterator ie = m_aDrawText.end();
		for (; it != ie; ++it)
		{
			it->crFont = dwColor;
		}
	}
	return true;
}

int	KItemText::SetFontBorder(WORD dwSize, DWORD dwColor)
{
	m_dwBorderColor = dwColor;
	if (dwSize > 255)
		dwSize = 255;
	m_byBorder = (BYTE)(dwSize);

	if (m_dwStyle & ITEM_DATA_FORMATED)
	{
		KUITextArray::iterator it = m_aDrawText.begin();
		KUITextArray::iterator ie = m_aDrawText.end();
		for (; it != ie; ++it)
		{
			it->crBorder = dwColor;
			it->byBorder = m_byBorder;
		}
	}
	return true;
}

int	KItemText::SetFontShadow(WORD dwSize, DWORD dwColor)
{
	m_dwShadowColor = dwColor;

	if (dwSize > 255)
		dwSize = 255;
	m_byShadow = (BYTE)(dwSize);

	if (m_dwStyle & ITEM_DATA_FORMATED)
	{
		KUITextArray::iterator it = m_aDrawText.begin();
		KUITextArray::iterator ie = m_aDrawText.end();
		for (; it != ie; ++it)
		{
			it->crShadow = dwColor;
			it->byShadow = m_byShadow;
		}
	}
	return true;
}

DWORD KItemText::GetFontID()
{
	return m_dwFontID;
}

DWORD KItemText::GetFontColor()
{
	return m_dwFontColor;
}

int	KItemText::GetFontBoder(WORD &dwSize, DWORD &dwColor)
{
	dwSize = m_byBorder;
	dwColor = m_dwBorderColor;
	return true;
}

int	KItemText::GetFontProjection(WORD &dwSize, DWORD &dwColor)
{
	dwSize = m_byShadow;
	dwColor = m_dwShadowColor;
	return true;
}

int	KItemText::GetTextExtent(int nTextLength, float& fWidth, float& fHeight) const
{
	ASSERT(nTextLength <= (int)m_strText.length());
	g_pUI->m_p3DUI->GetTextExtent(m_dwFontID, m_strText.c_str(), nTextLength, fWidth, fHeight, m_fFontSpacing, m_fFontScale, m_byBorder);
	return true;
}

int	KItemText::GetTextPosExtent(float& fWidth, int &nTextLength) const
{
	g_pUI->m_p3DUI->GetTextPosExtent(m_dwFontID, m_strText.c_str(), -1, fWidth, &nTextLength, m_fFontSpacing, m_fFontScale, m_byBorder);
	return true;
}

} //namespace UI