////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : itembox.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-2-24 10:51:18
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "./itembox.h"
#include "./itemtext.h"
#include "./itemeventmgr.h"
#include "./pickdropboxmgr.h"
#include "./decoder.h"
#include "../../common/ksmallobjectpool.h"
#include "../../script/kscriptmgr.h"
#include "../../KGUI.h"

////////////////////////////////////////////////////////////////////////////////
namespace
{
	KSmallObjectPool	g_Allocator("KItemBox", sizeof(UI::KItemBox));
}

namespace UI
{

DWORD KItemBox::ms_dwItemType = ITEM_BOX;

KItemBox::KItemBox()
{
	m_dwStatus = 0;
	m_nIndex = -1;
	m_nObjectType = -1;
	m_nSparkingAnimateIndex = -1;
	m_nInUseAnimateIndex = -1;
	m_nStaringAnimateIndex = -1;
	m_nIconID = -1;
	m_fCoolingPercentage = 0.0f;
	for (int i = 0; i < ITEM_BOX_OVER_OBJECT_TEXT_COUNT; ++i)
	{
		m_aOverText[i].OverText.wszText[0] = L'\0';
	}
}

KItemBox::~KItemBox()
{

}

void* KItemBox::operator new(size_t /* nSize */, std::nothrow_t const&) throw()
{ 
	return g_Allocator.Alloc(); 
}

void KItemBox::operator delete(void* pData, std::nothrow_t const&) throw()
{ 
	g_Allocator.Free(pData); 
}

void* KItemBox::operator new(size_t /* nSize */)
{ 
	return g_Allocator.Alloc(); 
}

void KItemBox::operator delete(void* pData) throw()
{ 
	g_Allocator.Free(pData); 
}

#ifdef KGUI_LUA_LOADER
int KItemBox::Create(IKUIObject **ppObj, Lua_State *pLua, int nTableIndex)
{
    int nResult  = false;
    int nRetCode = false;

    KItemNull *pItem = NULL;

    KGLOG_PROCESS_ERROR(ppObj);

    pItem = new(std::nothrow) KItemBox;
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

KItemBox *KItemBox::Create(KItemBoxData &ItemBoxData)
{
	KItemBox *pItemBox = NULL;
	int nRetCode = false;

	pItemBox = new(std::nothrow) KItemBox;
	KG_PROCESS_ERROR(pItemBox);

	nRetCode = pItemBox->Init(ItemBoxData);
	KG_PROCESS_ERROR(nRetCode);
	pItemBox->OnInitEnd(ItemBoxData);

Exit0:
	if (!nRetCode)
		SAFE_RELEASE(pItemBox);
	return pItemBox;
}

void KItemBox::Release()
{
	ClearObject();
	KItemNull::Release();
}

#ifdef KGUI_LUA_LOADER
int KItemBox::Init(Lua_State *pLua, int nTableIndex)
{
	int nResult  = false;
	int nRetCode = false;

    KLuaTableField aParamTable[] = 
    { 
        KLUA_TABLE_FIELD("index", emVAR_TYPE_INT, -1, &m_nIndex, 0),
    };

    KGLOG_PROCESS_ERROR(pLua);

    nRetCode = KItemNull::Init(pLua, nTableIndex);
    KGLOG_PROCESS_ERROR(nRetCode);

    for (int i = 0; i < _countof(aParamTable); i++)
    {
        nRetCode = g_LuaGetTableField(pLua, nTableIndex, &aParamTable[i]);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

// Exit1:
	nResult = true;
Exit0:
	return nResult;
}
#endif

int KItemBox::Init(KItemBoxData &ItemBoxData)
{
	int nResult = false;
	int nRetCode = false;

	nRetCode = KItemNull::Init(ItemBoxData);
	KG_PROCESS_ERROR(nRetCode);

	m_nIndex = ItemBoxData.nIndex;

	nResult = true;
Exit0:
	return nResult;
}

void KItemBox::Draw()
{
	if (!(m_dwStyle & ITEM_SHOW))
		return;

#ifndef DISABLE_UI_EDIT
	KItemNull::Draw();
#endif
	if (m_dwStatus & ITEM_BOX_HAVE_OBJECT)
	{
		if (!(m_dwStyle & ITEM_DATA_FORMATED))
			FormatIconDrawData();

		g_pUI->m_p3DUI->DrawMultiImage(&m_Image, 1);
		KPickDropBoxMgr::GetSelf().DrawBoxAddOnLayer(m_fAbsX, m_fAbsY, m_fWidth, m_fHeight, m_dwStatus, m_fCoolingPercentage);

		if (m_dwStatus & ITEM_BOX_OBJECT_SPARKING)
		{
			g_pUI->m_AnimateImageMgr.Draw(m_nSparkingAnimateIndex, m_fAbsX, m_fAbsY, m_fWidth, m_fHeight, m_dwAlpha);
			if (g_pUI->m_AnimateImageMgr.IsFinished(m_nSparkingAnimateIndex))
				SetObjectSparking(false);
		}
		if (m_dwStatus & ITEM_BOX_OBJECT_INUSE)
			g_pUI->m_AnimateImageMgr.Draw(m_nInUseAnimateIndex, m_fAbsX, m_fAbsY, m_fWidth, m_fHeight, m_dwAlpha);
		if (m_dwStatus & ITEM_BOX_OBJECT_STARING)
			g_pUI->m_AnimateImageMgr.Draw(m_nStaringAnimateIndex, m_fAbsX, m_fAbsY, m_fWidth, m_fHeight, m_dwAlpha);
	}
	else
	{
		if (m_dwStatus & ITEM_BOX_OBJECT_MOUSEOVER)
			KPickDropBoxMgr::GetSelf().DrawBoxMouseOverLayer(m_fAbsX, m_fAbsY, m_fWidth, m_fHeight);
	}

	for (int i = 0; i < ITEM_BOX_OVER_OBJECT_TEXT_COUNT; ++i)
	{
		if (m_aOverText[i].OverText.wszText[0] != L'\0')
			g_pUI->m_p3DUI->DrawMultiText(&m_aOverText[i].OverText, 1);
	}
}

DWORD KItemBox::GetItemType()
{
	return ms_dwItemType;
}

int KItemBox::IsItemCanChangeTo(DWORD dwType)
{
	if (dwType == ITEM_NULL || dwType == ITEM_BOX)
		return true;
	return false;
}

int KItemBox::SetRelPos(float fRelX, float fRelY)
{
	return KItemNull::SetRelPos(fRelX, fRelY);
}

int KItemBox::SetAbsPos(float fAbsX, float fAbsY)
{
	AbsMove(fAbsX - m_fAbsX, fAbsY - m_fAbsY);
	return true;
}

int KItemBox::AbsMove(float fDeltaX, float fDeltaY)
{
	KItemNull::AbsMove(fDeltaX, fDeltaY);

	if (m_dwStatus & ITEM_BOX_HAVE_OBJECT)
	{
		m_Image.v2Min = D3DXVECTOR2(m_fAbsX, m_fAbsY);
		m_Image.v2Max = D3DXVECTOR2(m_fAbsX + m_fWidth, m_fAbsY + m_fHeight);
	}
	for (int i = 0; i < ITEM_BOX_OVER_OBJECT_TEXT_COUNT; ++i)
	{
		if (m_aOverText[i].OverText.wszText[0] != L'\0')
		{
			m_aOverText[i].OverText.v2Min.x += fDeltaX;
			m_aOverText[i].OverText.v2Min.y += fDeltaY;
			m_aOverText[i].OverText.v2Max.x += fDeltaX;
			m_aOverText[i].OverText.v2Max.y += fDeltaY;
		}
	}
	
	return true;
}

int KItemBox::Scale(float fScaleX, float fScaleY)
{
	KItemNull::Scale(fScaleX, fScaleY);

	for (int i = 0; i < ITEM_BOX_OVER_OBJECT_TEXT_COUNT; ++i)
	{
		FormatOverText(i);
	}

	return true;
}

int KItemBox::SetIndex(int nIndex)
{
	m_nIndex = nIndex;
	return true;
}

int KItemBox::GetIndex()
{
	return m_nIndex;
}

int KItemBox::SetObjectIcon(int nIconID)
{
	KIconModel const* pIconModel = NULL;

	KG_PROCESS_SUCCESS(m_nIconID == nIconID);

	ClearObjectIcon();

	m_nIconID = nIconID;

	pIconModel = g_pUI->GetIconModel((DWORD)m_nIconID);
	if (pIconModel)
		g_pUI->m_ImageInfoMgr.LoadImageInfo(GetIconFullPath(pIconModel->szFileName));

	m_dwStyle &= ~ITEM_DATA_FORMATED;

Exit1:
	return true;
}

int KItemBox::ClearObjectIcon()
{
	if (m_nIconID != -1)
	{
		KIconModel const* pIconModel = g_pUI->GetIconModel((DWORD)m_nIconID);
		if (pIconModel)
			g_pUI->m_ImageInfoMgr.UnloadImageInfo(GetIconFullPath(pIconModel->szFileName));

		m_nIconID = -1;
		m_dwStyle &= ~ITEM_DATA_FORMATED;	
	}
	return true;
}

int KItemBox::GetObjectIcon()
{
	return m_nIconID;
}

int KItemBox::SetObject(int nObjectType, const ITEM_BOX_OBJECT_DATA &cObjectData)
{
	ClearObject();
	m_nObjectType = nObjectType;
	m_ObjectData = cObjectData;
	m_dwStatus |= ITEM_BOX_HAVE_OBJECT;
	return true;
}

int KItemBox::GetObject(int &nObjectType, ITEM_BOX_OBJECT_DATA &ObjectData)
{
	nObjectType = m_nObjectType;
	ObjectData = m_ObjectData;
	return true;
}

int KItemBox::GetObjectType()
{
	return m_nObjectType;
}

int KItemBox::GetObjectData(ITEM_BOX_OBJECT_DATA &ObjectData)
{
	ObjectData = m_ObjectData;
	return true;
}

int KItemBox::ClearObject()
{
	ClearObjectIcon();
	SetObjectSparking(false);
	SetObjectInUse(false);
	SetObjectStaring(false);

	if (m_dwStatus & ITEM_BOX_OBJECT_MOUSEOVER)
		m_dwStatus = ITEM_BOX_OBJECT_MOUSEOVER;
	else
		m_dwStatus = 0;

	m_nObjectType = INVALID_OBJECT_TYPE;
	m_ObjectData.Reset();

	return true;
}

int KItemBox::IsEmpty()
{
	if (m_dwStatus & ITEM_BOX_HAVE_OBJECT)
		return false;
	return true;
}

int KItemBox::EnableObject(int nEnable)
{
	m_dwStatus |= ITEM_BOX_OBJECT_DISABLE;
	if (nEnable)
		m_dwStatus &= ~ITEM_BOX_OBJECT_DISABLE;
	return true;
}

int KItemBox::IsObjectEnable()
{
	if (m_dwStatus & ITEM_BOX_OBJECT_DISABLE)
		return false;
	return true;
}

int KItemBox::EnableObjectEquip(int nEnable)
{
	m_dwStatus |= ITEM_BOX_OBJECT_UNEQUIPABLE;
	if (nEnable)
		m_dwStatus &= ~ITEM_BOX_OBJECT_UNEQUIPABLE;
	return true;
}

int KItemBox::IsObjectEquipable()
{
	if (m_dwStatus & ITEM_BOX_OBJECT_UNEQUIPABLE)
		return false;
	return true;
}


int KItemBox::SetObjectCoolDown(int nCoolDown)
{
	m_dwStatus &= ~ITEM_BOX_OBJECT_COOLINGDOWN;
	if (nCoolDown)
		m_dwStatus |= ITEM_BOX_OBJECT_COOLINGDOWN;
	return true;
}

int KItemBox::IsObjectCoolDown()
{
	if (m_dwStatus & ITEM_BOX_OBJECT_COOLINGDOWN)
		return true;
	return false;
}

int KItemBox::SetObjectSparking(int nSparking)
{
	if (nSparking)
	{
		if (m_nSparkingAnimateIndex != -1)
			KPickDropBoxMgr::GetSelf().RemoveSparkingAnimate(m_nSparkingAnimateIndex);
		m_nSparkingAnimateIndex = KPickDropBoxMgr::GetSelf().AppendSparkingAnimate();
		m_dwStatus |= ITEM_BOX_OBJECT_SPARKING;
	}
	else
	{
		if (m_nSparkingAnimateIndex != -1)
			KPickDropBoxMgr::GetSelf().RemoveSparkingAnimate(m_nSparkingAnimateIndex);
		m_nSparkingAnimateIndex = -1;
		m_dwStatus &= ~ITEM_BOX_OBJECT_SPARKING;
	}

	return true;
}

int KItemBox::SetObjectInUse(int nInUse)
{
	if (nInUse)
	{
		if (m_dwStatus & ITEM_BOX_OBJECT_INUSE)
			return true;
		if (m_nInUseAnimateIndex != -1)
			KPickDropBoxMgr::GetSelf().RemoveInUseAnimate(m_nInUseAnimateIndex);
		m_nInUseAnimateIndex = KPickDropBoxMgr::GetSelf().AppendInUseAnimate();
		m_dwStatus |= ITEM_BOX_OBJECT_INUSE;
	}
	else
	{
		if (!(m_dwStatus & ITEM_BOX_OBJECT_INUSE))
			return true;
		if (m_nInUseAnimateIndex != -1)
			KPickDropBoxMgr::GetSelf().RemoveInUseAnimate(m_nInUseAnimateIndex);
		m_nInUseAnimateIndex = -1;
		m_dwStatus &= ~ITEM_BOX_OBJECT_INUSE;
	}

	return true;
}

int KItemBox::SetObjectStaring(int nStaring)
{
	if (nStaring)
	{
		if (m_dwStatus & ITEM_BOX_OBJECT_STARING)
			return true;
		if (m_nStaringAnimateIndex != -1)
			KPickDropBoxMgr::GetSelf().RemoveStaringAnimate(m_nStaringAnimateIndex);
		m_nStaringAnimateIndex = KPickDropBoxMgr::GetSelf().AppendStaringAnimate();
		m_dwStatus |= ITEM_BOX_OBJECT_STARING;
	}
	else
	{
		if (!(m_dwStatus & ITEM_BOX_OBJECT_STARING))
			return true;
		if (m_nStaringAnimateIndex != -1) 
			KPickDropBoxMgr::GetSelf().RemoveStaringAnimate(m_nStaringAnimateIndex);
		m_nStaringAnimateIndex = -1;
		m_dwStatus &= ~ITEM_BOX_OBJECT_STARING;
	}

	return true;
}


int KItemBox::SetObjectSelected(int nSelect)
{
	m_dwStatus &= ~ITEM_BOX_OBJECT_SELECTED;
	if (nSelect)
		m_dwStatus |= ITEM_BOX_OBJECT_SELECTED;
	return true;
}

int KItemBox::IsObjectSelected()
{
	if (m_dwStatus & ITEM_BOX_OBJECT_SELECTED)
		return true;
	return false;
}

int KItemBox::SetObjectMouseOver(int nMouseOver)
{
	m_dwStatus &= ~ITEM_BOX_OBJECT_MOUSEOVER;
	if (nMouseOver)
		m_dwStatus |= ITEM_BOX_OBJECT_MOUSEOVER;
	return true;
}

int KItemBox::IsObjectMouseOver()
{
	if (m_dwStatus & ITEM_BOX_OBJECT_MOUSEOVER)
		return true;
	return false;
}

int KItemBox::SetObjectPressed(int nPressed)
{
	m_dwStatus &= ~ITEM_BOX_OBJECT_PRESSED;
	if (nPressed)
		m_dwStatus |= ITEM_BOX_OBJECT_PRESSED;
	return true;
}

int KItemBox::IsObjectPressed()
{
	if (m_dwStatus & ITEM_BOX_OBJECT_PRESSED)
		return true;
	return false;
}

int KItemBox::SetCoolDownPercentage(float fPercentage)
{
	if (fPercentage < 0.0f)
		fPercentage = 0.0f;
	if (fPercentage > 1.0f)
		fPercentage = 1.0f;

	m_fCoolingPercentage = fPercentage;
	return true;
}

float KItemBox::GetCoolDownPercentage()
{
	return m_fCoolingPercentage;
}

int KItemBox::SetOverText(int nOverTextIndex, LPCWSTR wszText)
{
	int nResult = false;

	KG_PROCESS_ERROR(nOverTextIndex >= 0);
	KG_PROCESS_ERROR(nOverTextIndex < ITEM_BOX_OVER_OBJECT_TEXT_COUNT);
	KG_PROCESS_ERROR(wszText);

	KITEMBOXOVERTEXT& OverText = m_aOverText[nOverTextIndex];

	wcsncpy(OverText.OverText.wszText, wszText, _countof(OverText.OverText.wszText));
	OverText.OverText.wszText[_countof(OverText.OverText.wszText) - 1] = L'\0';

	OverText.OverText.nTextLength = -1;
	FormatOverText(nOverTextIndex);

	nResult = true;
Exit0:
	return nResult; 
}

LPCWSTR KItemBox::GetOverText(int nOverTextIndex) const
{
	if (nOverTextIndex < 0 || nOverTextIndex >= ITEM_BOX_OVER_OBJECT_TEXT_COUNT)
		return L"";

	return m_aOverText[nOverTextIndex].OverText.wszText;
}

int KItemBox::SetOverTextFontScheme(int nOverTextIndex, int nFontScheme)
{
	int nResult = false;

	KG_PROCESS_ERROR(nOverTextIndex >= 0);
	KG_PROCESS_ERROR(nOverTextIndex < ITEM_BOX_OVER_OBJECT_TEXT_COUNT);

	m_aOverText[nOverTextIndex].nFontScheme = nFontScheme;

	FormatOverText(nOverTextIndex);

	nResult = true;
Exit0:
	return nResult;
}

int KItemBox::GetOverTextFontScheme(int nOverTextIndex)
{
	if (nOverTextIndex < 0 || nOverTextIndex >= ITEM_BOX_OVER_OBJECT_TEXT_COUNT)
		return -1;
	return m_aOverText[nOverTextIndex].nFontScheme;
}

int KItemBox::SetOverTextPosition(int nOverTextIndex, int nPosType)
{
	int nResult = false;

	KG_PROCESS_ERROR(nOverTextIndex >= 0);
	KG_PROCESS_ERROR(nOverTextIndex < ITEM_BOX_OVER_OBJECT_TEXT_COUNT);

	m_aOverText[nOverTextIndex].nPosition = nPosType;

	FormatOverText(nOverTextIndex);

	nResult = true;
Exit0:
	return nResult;
}

int KItemBox::GetOverTextPosition(int nOverTextIndex)
{
	if (nOverTextIndex < 0 || nOverTextIndex >= ITEM_BOX_OVER_OBJECT_TEXT_COUNT)
		return -1;
	return m_aOverText[nOverTextIndex].nPosition;	
}

int KItemBox::FormatOverText(int nOverTextIndex)
{
	int nResult = false;
	float fTextWidth = 0.0f;
	float fTextHeight = 0.0f;
	DWORD dwFontID = 0;
	KFontScheme const* pFontScheme = NULL;

	ASSERT(nOverTextIndex >= 0);
	ASSERT(nOverTextIndex < ITEM_BOX_OVER_OBJECT_TEXT_COUNT);

	KG3DUIText2& Text = m_aOverText[nOverTextIndex].OverText;

	KG_PROCESS_ERROR(Text.wszText[0] != L'\0');

	pFontScheme = g_pUI->m_FontSchemeMgr.GetFontScheme(m_aOverText[nOverTextIndex].nFontScheme);
	KGLOG_PROCESS_ERROR(pFontScheme);

	Text.crFont = g_pUI->m_ColorSchemeMgr.GetColor(pFontScheme->nFontColorSchemeIndex);
	Text.crBorder = g_pUI->m_ColorSchemeMgr.GetColor(pFontScheme->nBorderColorSchemeIndex);
	Text.crShadow = g_pUI->m_ColorSchemeMgr.GetColor(pFontScheme->nShadowColorSchemeIndex);
	Text.byBorder = pFontScheme->nBorderSize;
	Text.byShadow = pFontScheme->nShadowOffset;
	Text.fScale = 1.0f;
	Text.dwFontID = pFontScheme->nFontID;
	Text.byAlpha = 0xFF;
	Text.fSpacing = 0.0f;

	g_pUI->m_p3DUI->GetTextExtent(Text.dwFontID, Text.wszText, -1, fTextWidth, fTextHeight, 0.0f, Text.fScale, Text.byBorder);

	switch (m_aOverText[nOverTextIndex].nPosition)
	{
	case POSITION_LEFT_TOP:  
	case POSITION_TOP_LEFT:
		Text.v2Min = D3DXVECTOR2(m_fAbsX, m_fAbsY);
		Text.v2Max = D3DXVECTOR2(m_fAbsX + fTextWidth, m_fAbsY + fTextHeight);
		break;
	case POSITION_LEFT_BOTTOM:     
	case POSITION_BOTTOM_LEFT:
		Text.v2Min = D3DXVECTOR2(m_fAbsX, m_fAbsY + m_fHeight - fTextHeight);
		Text.v2Max = D3DXVECTOR2(m_fAbsX + fTextWidth, m_fAbsY + m_fHeight);
		break;
	case POSITION_TOP_RIGHT:
	case POSITION_RIGHT_TOP:
		Text.v2Min = D3DXVECTOR2(m_fAbsX + m_fWidth - fTextWidth, m_fAbsY);
		Text.v2Max = D3DXVECTOR2(m_fAbsX + m_fWidth, m_fAbsY + fTextHeight);
		break;
	case POSITION_RIGHT_BOTTOM:
	case POSITION_BOTTOM_RIGHT:
		Text.v2Min = D3DXVECTOR2(m_fAbsX + m_fWidth - fTextWidth, m_fAbsY + m_fHeight - fTextHeight);
		Text.v2Max = D3DXVECTOR2(m_fAbsX + m_fWidth, m_fAbsY + m_fHeight);
		break;
	default:
		Text.v2Min = D3DXVECTOR2(m_fAbsX, m_fAbsY);
		Text.v2Max = D3DXVECTOR2(m_fAbsX + m_fWidth, m_fAbsY + m_fHeight);
		break;
	}

	nResult = true;
Exit0:
	return nResult;
}

int KItemBox::FormatIconDrawData()
{
	int nResult = false;
	int nRetCode = false;
	int nImageInfoIndex = -1;
	int nFrame = -1;
	int nTextureFrame = 0;
	RECT rcSrc;
	SIZE sizeImage;
	LPCSTR pcszImage = NULL;
	KIconModel const* pIconModel = NULL;

	pIconModel = g_pUI->GetIconModel((DWORD)m_nIconID);
	KG_PROCESS_ERROR(pIconModel);

	KImageInfo* pImageInfo = g_pUI->m_ImageInfoMgr.GetImageInfo(GetIconFullPath(pIconModel->szFileName));
	KG_PROCESS_ERROR(pImageInfo);
	KG_PROCESS_ERROR(pIconModel->dwFrame < (DWORD)pImageInfo->GetFrameCount());

	rcSrc = pImageInfo->GetImageRect(pIconModel->dwFrame);
	sizeImage = pImageInfo->GetImageSize();

	m_Image.dwImageID = pImageInfo->GetImageID(pIconModel->dwFrame);
	m_Image.v2Min = D3DXVECTOR2(m_fAbsX, m_fAbsY);
	m_Image.v2Max = D3DXVECTOR2(m_fAbsX + m_fWidth, m_fAbsY + m_fHeight);
	m_Image.v2UVMin = D3DXVECTOR2((float)(rcSrc.left) / (float)sizeImage.cx, (float)(rcSrc.top) / (float)sizeImage.cy);
	m_Image.v2UVMax = D3DXVECTOR2((float)(rcSrc.right) / (float)sizeImage.cx, (float)(rcSrc.bottom) / (float)sizeImage.cy);

	m_Image.crDiffuse = m_dwAlpha << 24;

	nResult = true;
Exit0:
	if (!nResult)
	{
		ZeroMemory(&m_Image, sizeof(m_Image));
	}
	m_dwStyle |= ITEM_DATA_FORMATED;
	return nResult;
}

}//namespace UI