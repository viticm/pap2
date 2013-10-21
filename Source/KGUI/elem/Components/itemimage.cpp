////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : itemimage.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2005-11-9 17:06:15
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "./itemimage.h"
#include "./itemeventmgr.h"
#include "./decoder.h"
#include "../../common/ksmallobjectpool.h"
#include "../../script/kscriptmgr.h"
#include "../../KGUI.h"


namespace
{
	KSmallObjectPool g_Allocator("KItemImage", sizeof(UI::KItemImage));
}

namespace UI
{

DWORD KItemImage::ms_dwItemType = ITEM_IMAGE;

KItemImage::KItemImage()
{
	m_nImageInfoIndex = -1;
	m_nFrame = -1;
	m_nImageType = ITEMIMAGE_NORMAL;
	m_fPercentage = 1.0f;
	m_dwAlpha = 255;
}

KItemImage::~KItemImage()
{

}

void* KItemImage::operator new(size_t /* nSize */, std::nothrow_t const&) throw()
{ 
	return g_Allocator.Alloc(); 
}

void KItemImage::operator delete(void* pData, std::nothrow_t const&) throw()
{ 
	g_Allocator.Free(pData); 
}

void* KItemImage::operator new(size_t /* nSize */)
{ 
	return g_Allocator.Alloc(); 
}

void KItemImage::operator delete(void* pData) throw()
{ 
	g_Allocator.Free(pData); 
}

#ifdef KGUI_LUA_LOADER
int KItemImage::Create(IKUIObject **ppObj, Lua_State *pLua, int nTableIndex)
{
    int nResult  = false;
    int nRetCode = false;

    KItemNull *pItem = NULL;

    KGLOG_PROCESS_ERROR(ppObj);

    pItem = new(std::nothrow) KItemImage;
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

KItemImage *KItemImage::Create(KItemImageData &ItemImageData)
{
    KItemImage *pItemImage = NULL;
    int nRetCode = false;

    pItemImage = new(std::nothrow) KItemImage;
    KG_PROCESS_ERROR(pItemImage);

    nRetCode = pItemImage->Init(ItemImageData);
	KG_PROCESS_ERROR(nRetCode);
	pItemImage->OnInitEnd(ItemImageData);
Exit0:
    if (!nRetCode)
        SAFE_RELEASE(pItemImage);
    return pItemImage;
}

void KItemImage::Release()
{
	UnloadSource();

	KItemNull::Release();
}

int KItemImage::UnloadSource()
{
	if (m_dwStyle & ITEM_IMAGE_RESOURCE_LOADED)
	{
		if (m_dwStyle & ITEM_IMAGE_NOT_FROM_UITEX)
		{
			g_pUI->m_p3DUI->UnloadImage(m_Image.dwImageID);
			m_Image.dwImageID = 0;
		}
		else
		{
			if (m_nImageInfoIndex != -1)
			{
				g_pUI->m_ImageInfoMgr.UnloadImageInfo(m_nImageInfoIndex);
				m_Image.dwImageID = 0;
				m_nImageInfoIndex = -1;
			}
		}
	}

	m_dwStyle &= ~ITEM_DATA_FORMATED;
	m_dwStyle &= ~ITEM_IMAGE_RESOURCE_LOADED;
	return true;
}

#ifdef KGUI_LUA_LOADER
int KItemImage::Init(Lua_State *pLua, int nTableIndex)
{
	int nResult  = false;
	int nRetCode = false;

    char szImagePath[MAX_PATH];
    int  nFrame     = 0;
    int  nImageType = 0;

    KLuaTableField aParamTable[] = 
    {
        KLUA_TABLE_FIELD("image",     emVAR_TYPE_STRING, "", szImagePath, _countof(szImagePath)),
        KLUA_TABLE_FIELD("frame",     emVAR_TYPE_INT,    0,  &nFrame,     0),
        KLUA_TABLE_FIELD("ImageType", emVAR_TYPE_INT,    0,  &nImageType, 0),
    };

    KGLOG_PROCESS_ERROR(pLua);

    nRetCode = KItemNull::Init(pLua, nTableIndex);
    KGLOG_PROCESS_ERROR(nRetCode);

    for (int i = 0; i < _countof(aParamTable); i++)
    {
        nRetCode = g_LuaGetTableField(pLua, nTableIndex, &aParamTable[i]);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    if (szImagePath[0])
    {
        FormatFilePath(szImagePath);
        if (stricmp(szImagePath, "fromiconid") == 0)
            FromIconID(nFrame);
        else
            FromUITex(szImagePath, nFrame);
    }

    if (m_fWidth == 0 && m_fHeight == 0)
        AutoSize();

    SetImageType(nImageType);

// Exit1:
	nResult = true;
Exit0:
	return nResult;
}
#endif

int KItemImage::Init(KItemImageData &ItemImageData)
{
    int nResult = false;
    int nRetCode = false;

    nRetCode = KItemNull::Init(ItemImageData);
    KG_PROCESS_ERROR(nRetCode);

	if (stricmp(ItemImageData.szImagePath, "fromiconid") == 0)
		FromIconID(ItemImageData.nFrame);
	else
		FromUITex(ItemImageData.szImagePath, ItemImageData.nFrame);

    if (ItemImageData.nAutoSize)
        AutoSize();

	SetImageType(ItemImageData.nPartImageType);

    nResult = true;
Exit0:
    return nResult;
}

int KItemImage::FromIconID(int nIconID)
{
	KIconModel const* pIconModel = g_pUI->GetIconModel((DWORD)nIconID);
	KGLOG_PROCESS_ERROR(pIconModel);

	return FromUITex(GetIconFullPath(pIconModel->szFileName), pIconModel->dwFrame);
Exit0:
	return false;
}

int KItemImage::FromUITex(LPCSTR szFile, int nImageFrame)
{
	int nResult = false;
	int nRetCode = false;

	KG_PROCESS_ERROR(szFile);
	KG_PROCESS_ERROR(szFile[0] != '\0');

	UnloadSource();

    m_nImageInfoIndex = g_pUI->m_ImageInfoMgr.LoadImageInfo(szFile);
	KG_PROCESS_ERROR(m_nImageInfoIndex >= 0);

	SetFrame(nImageFrame);

	m_dwStyle |= ITEM_IMAGE_RESOURCE_LOADED;
	m_dwStyle &= ~ITEM_IMAGE_NOT_FROM_UITEX;
	nResult = true;
Exit0:
	return nResult;
}

int KItemImage::FromTextureFile(LPCSTR pszFile, D3DXVECTOR2 &vMin, D3DXVECTOR2 &vMax)
{
	int nResult = false;
	DWORD dwID = 0;
	HRESULT hr = E_FAIL;

	UnloadSource();

	KG_PROCESS_ERROR(pszFile);
	hr = g_pUI->m_p3DUI->LoadImage(pszFile, &dwID);
	KG_COM_PROCESS_ERROR(hr);

	m_Image.dwImageID = dwID;
	m_Image.v2UVMin = vMin;
	m_Image.v2UVMax = vMax;

	m_dwStyle &= ~ITEM_DATA_FORMATED;

	m_dwStyle |= ITEM_IMAGE_RESOURCE_LOADED;
	m_dwStyle |= ITEM_IMAGE_NOT_FROM_UITEX;
	nResult = true;
Exit0:
	return nResult;

}

int KItemImage::FromScene(IKG3DScene* p3DScene)
{
    int nResult = false;
	HRESULT hr = E_FAIL;
	KG3DUIScene2 UIScene;
	DWORD dwImageID = 0;
    BOOL bCreateImage = FALSE;
    BOOL bBeginRenderToImage = FALSE;
    BOOL bBeginScene = FALSE;

	UnloadSource();

	ASSERT(p3DScene);

	hr = g_pUI->m_p3DUI->CreateImage((UINT)m_fWidth, (UINT)m_fHeight, &dwImageID, KG3DUI_IMAGE_FLAGS_POOL_DEFAULT);
	KGLOG_COM_PROCESS_ERROR(hr);

    bCreateImage = TRUE;

	UIScene.v2Min = D3DXVECTOR2(0.0f, 0.0f);
	UIScene.v2Max = D3DXVECTOR2(m_fWidth, m_fHeight);
	UIScene.p3DScene = p3DScene;
	UIScene.dwFlags = 
		KG3DUI_SCENE_FLAGS_DISABLE_TERRAIN | 
		KG3DUI_SCENE_FLAGS_DISABLE_SKYBOX | 
		KG3DUI_SCENE_FLAGS_DISABLE_WATER | 
		KG3DUI_SCENE_FLAGS_DISABLE_SUNLIGHT | 
		KG3DUI_SCENE_FLAGS_DISABLE_VIEWPORT |
		KG3DUI_SCENE_FLAGS_ENABLE_ALPHA;

    for (int n = 0; n < 4; ++n)
        p3DScene->FrameMove();

	hr = g_pUI->m_p3DUI->BeginRenderToImage(dwImageID);
    KGLOG_COM_PROCESS_ERROR(hr);

    bBeginRenderToImage = TRUE;

    hr = g_pUI->m_p3DUI->BeginScene();
    KGLOG_COM_PROCESS_ERROR(hr);

    bBeginScene = TRUE;

    hr = g_pUI->m_p3DUI->DrawMultiScene(&UIScene, 1);
    KGLOG_COM_PROCESS_ERROR(hr);

	m_Image.dwImageID = dwImageID;
	m_Image.v2UVMin = D3DXVECTOR2(0.0f, 0.0f);
	m_Image.v2UVMax = D3DXVECTOR2(1.0f, 1.0f);

	m_dwStyle |= ITEM_IMAGE_RESOURCE_LOADED;
	m_dwStyle |= ITEM_IMAGE_NOT_FROM_UITEX;

	m_dwStyle &= ~ITEM_DATA_FORMATED;

	nResult = true;
Exit0:
    if (hr == D3DERR_DRIVERINTERNALERROR || hr == D3DERR_INVALIDCALL)
    {
        KGLogPrintf(KGLOG_ERR, "KGUI KItemImage::FromScene %u.\n", hr);

        PostMessage(g_pUI->m_hWnd, WM_QUIT, 0, 0);
    }
    if (bBeginScene)
    {
        hr = g_pUI->m_p3DUI->EndScene();
        KGLOG_COM_CHECK_ERROR(hr);
    }
    if (bBeginRenderToImage)
    {
        hr = g_pUI->m_p3DUI->EndRenderToImage();
        KGLOG_COM_CHECK_ERROR(hr);
    }
    if (!nResult)
    {
        if (bCreateImage)
        {
            hr = g_pUI->m_p3DUI->UnloadImage(dwImageID);
            KGLOG_COM_CHECK_ERROR(hr);
        }
    }
	return nResult;
}

int KItemImage::ToManagedImage()
{
    int nResult = false;
    int nRetCode = false;

    if (m_Image.dwImageID != 0)
    {
        nRetCode = FromImageID(m_Image.dwImageID);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nResult = true;
Exit0:
    return nResult;
}

int KItemImage::FromImageID(DWORD dwImageID)
{
    int nResult = false;
    int nRetCode = false;
	HRESULT hr = E_FAIL;
	DWORD dwNewImageID = 0;
    BOOL bDuplicateImage = FALSE;

	// TODO: 检查m_dwStyle 能否被复制

	hr = g_pUI->m_p3DUI->DuplicateImage(dwImageID, 0, &dwNewImageID);
	KGLOG_COM_PROCESS_ERROR(hr);

    bDuplicateImage = TRUE;

	nRetCode = UnloadSource();
    KGLOG_PROCESS_ERROR(nRetCode);

	m_dwStyle |= ITEM_IMAGE_RESOURCE_LOADED;
	m_dwStyle |= ITEM_IMAGE_NOT_FROM_UITEX;

	m_Image.dwImageID = dwNewImageID;
	m_Image.v2UVMin = D3DXVECTOR2(0.0f, 0.0f);
	m_Image.v2UVMax = D3DXVECTOR2(1.0f, 1.0f);

	m_dwStyle &= ~ITEM_DATA_FORMATED;

	nResult = true;
Exit0:
    if (!nResult)
    {
        if (bDuplicateImage)
        {
            hr = g_pUI->m_p3DUI->UnloadImage(dwNewImageID);
            KGLOG_COM_CHECK_ERROR(hr);
        }
    }
	return nResult;
}


DWORD KItemImage::GetImageID() const
{
	// TODO: 不是所有图片都能取到正确的图片ID
	return m_Image.dwImageID;
}


int KItemImage::SetFrame(int nImageFrame)
{
	m_nFrame = nImageFrame;
    
	m_dwStyle &= ~ITEM_DATA_FORMATED;
	return true;
}

int KItemImage::GetFrame()
{
	return m_nFrame;
}

int KItemImage::AutoSize()
{
    int nResult = false;
    SIZE size = { 0, 0 };
    KImageInfo *pImageInfo = NULL;

	if (m_dwStyle & ITEM_IMAGE_NOT_FROM_UITEX)
	{
		UINT uW = 0;
		UINT uH = 0;
		HRESULT hr = E_FAIL;
		KG_PROCESS_ERROR(m_dwStyle & ITEM_IMAGE_RESOURCE_LOADED);
		hr = g_pUI->m_p3DUI->GetImageSize(m_Image.dwImageID, &uW, &uH);
		KG_COM_PROCESS_ERROR(hr);
		SetSize((float)uW, (float)uH);
		m_dwStyle &= ~ITEM_DATA_FORMATED;
	}
	else
	{
		pImageInfo = g_pUI->m_ImageInfoMgr.GetImageInfo(m_nImageInfoIndex);
		KG_PROCESS_ERROR(pImageInfo);
		size = pImageInfo->GetFrameSize(m_nFrame);
		SetSize((float)size.cx, (float)size.cy);
		m_dwStyle &= ~ITEM_DATA_FORMATED;
	}

	if (!(m_dwStyle & ITEM_NEVER_SCALED))
	{
		float fScale = g_pUI->m_WndStation.GetUIScale();
		m_fWidth *= fScale;
		m_fHeight *= fScale;
	}


    nResult = true;
Exit0:
    return nResult;
}

void KItemImage::Draw()
{
    if (!(m_dwStyle & ITEM_SHOW))
		return;

#ifndef DISABLE_UI_EDIT
    KItemNull::Draw();
#endif //_DEBUG
	if (!(m_dwStyle & ITEM_DATA_FORMATED))
		FormatDrawData();

	switch (m_nImageType)
	{
	case ITEMIMAGE_PART_TIMER:
		g_pUI->m_p3DUI->DrawMultiImagePart(&m_Image, 1);
		break;
	case ITEMIMAGE_ROTATE:
		g_pUI->m_p3DUI->DrawMultiImageRotate(&m_Image, 1);
		break;
	case ITEMIMAGE_NORMAL:
	case ITEMIMAGE_FLIP_HORIZONTAL:
	case ITEMIMAGE_FLIP_VERTICAL:
	case ITEMIMAGE_FLIP_CENTRAL:
		g_pUI->m_p3DUI->DrawMultiImage(&m_Image, 1);
		break;
	case ITEMIMAGE_NINE_PART:
		DrawNinePart();
		break;
	case ITEMIMAGE_LEFT_CENTER_RIGHT:
		DrawLeftCenterRight();
		break;
	case ITEMIMAGE_TOP_CENTER_BOTTOM:
		DrawTopCenterBottom();
		break;
	default:
		if (m_dwStyle & ITEM_IMAGE_NOT_FROM_UITEX)
			g_pUI->m_p3DUI->DrawMultiImagePart(&m_Image, 1);
		else
			g_pUI->m_p3DUI->DrawMultiImage(&m_Image, 1);
		break;
	}
}

DWORD KItemImage::GetItemType()
{
    return ms_dwItemType;
}

int KItemImage::IsItemCanChangeTo(DWORD dwType)
{
    if (dwType == ITEM_IMAGE || dwType == ITEM_NULL)
        return true;
    return false;
}

int KItemImage::SetImageType(int nType)
{
	if (nType < ITEMIMAGE_NORMAL || nType >= ITEMIMAGE_END)
		return false;

	if (nType == m_nImageType)
		return true;

	if (m_nImageType == ITEMIMAGE_ROTATE)
		SetPercentage(1.0f);

	m_nImageType = nType;
	if (m_nImageType == ITEMIMAGE_ROTATE)
		SetRotate(0.0f);

	m_dwStyle &= ~ITEM_DATA_FORMATED;

	return true;
}

int KItemImage::GetImageType() const
{
	return m_nImageType;
}

int KItemImage::SetPercentage(float fPercentage)
{
	if (fPercentage < 0.0f)
		fPercentage = 0.0f;
	if (fPercentage > 1.0f)
		fPercentage = 1.0f;
	m_fPercentage = fPercentage;

	m_dwStyle &= ~ITEM_DATA_FORMATED;

	return true;
}

float KItemImage::GetPercentage() const
{
	return m_fPercentage;
}

int KItemImage::SetRotate(float fRotate)
{
	if (m_nImageType != ITEMIMAGE_ROTATE)
		return SetPercentage(fRotate);

	m_fRotate = fRotate;
	m_dwStyle &= ~ITEM_DATA_FORMATED;

	return true;
}


float KItemImage::GetRotate() const
{
	return m_fRotate;
}


int KItemImage::FormatDrawData()
{
	int nResult = false;
	SIZE sizeImage;

	if (m_dwStyle & ITEM_IMAGE_NOT_FROM_UITEX)
	{
		m_Image.v2Min = D3DXVECTOR2(m_fAbsX, m_fAbsY);
		m_Image.v2Max = D3DXVECTOR2(m_fAbsX + m_fWidth, m_fAbsY + m_fHeight);
		m_Image.crDiffuse = m_dwAlpha << 24;
		m_Image.fRatio = m_fPercentage;

		switch (m_nImageType)
		{
		case ITEMIMAGE_PART_LEFT_TO_RIGHT:
			m_Image.dwType = KG3DUI_PART_TYPE_X_MIN_TO_MAX;
			break;
		case ITEMIMAGE_PART_RIGHT_TO_LEFT:
			m_Image.dwType = KG3DUI_PART_TYPE_X_MAX_TO_MIN;
			break;
		case ITEMIMAGE_PART_TOP_TO_BOTTOM:
			m_Image.dwType = KG3DUI_PART_TYPE_Y_MIN_TO_MAX;
			break;
		case ITEMIMAGE_PART_BOTTOM_TO_TOP:
			m_Image.dwType = KG3DUI_PART_TYPE_Y_MAX_TO_MIN;
			break;
		case ITEMIMAGE_PART_TIMER:
			m_Image.dwType = KG3DUI_PART_TYPE_TIMER;
			break;
		case ITEMIMAGE_ROTATE:
			m_Image.dwType = KG3DUI_PART_TYPE_TIMER;
			break;
		case ITEMIMAGE_FLIP_HORIZONTAL:
			if (m_Image.v2UVMin.x < m_Image.v2UVMax.x)
			{
				float fTemp = m_Image.v2UVMax.x;
				m_Image.v2UVMax.x = m_Image.v2UVMin.x;
				m_Image.v2UVMin.x = fTemp;
			}
			break;
		case ITEMIMAGE_FLIP_VERTICAL:
			if (m_Image.v2UVMin.y < m_Image.v2UVMax.y)
			{
				float fTemp = m_Image.v2UVMax.y;
				m_Image.v2UVMax.y = m_Image.v2UVMin.y;
				m_Image.v2UVMin.y = fTemp;
			}
			break;
		case ITEMIMAGE_FLIP_CENTRAL:
			if (m_Image.v2UVMin.y < m_Image.v2UVMax.y)
			{
				float fTemp = m_Image.v2UVMax.y;
				m_Image.v2UVMax.y = m_Image.v2UVMin.y;
				m_Image.v2UVMin.y = fTemp;
			}
			if (m_Image.v2UVMin.x < m_Image.v2UVMax.x)
			{
				float fTemp = m_Image.v2UVMax.x;
				m_Image.v2UVMax.x = m_Image.v2UVMin.x;
				m_Image.v2UVMin.x = fTemp;
			}
			break;
		default:
			break;
		}
		goto Exit1;
	}

	KG_PROCESS_ERROR(m_nImageInfoIndex >= 0);

	KImageInfo* pImageInfo = g_pUI->m_ImageInfoMgr.GetImageInfo(m_nImageInfoIndex);
	KG_PROCESS_ERROR(pImageInfo);

	KG_PROCESS_ERROR(m_nFrame >= 0 && m_nFrame < pImageInfo->GetFrameCount());

	RECT const& rcSrc = pImageInfo->GetImageRect(m_nFrame);
	sizeImage = pImageInfo->GetImageSize();

	m_Image.dwImageID = pImageInfo->GetImageID(m_nFrame);
	m_Image.crDiffuse = m_dwAlpha << 24;

	switch (m_nImageType)
	{
	case ITEMIMAGE_PART_LEFT_TO_RIGHT:
		m_Image.v2Min = D3DXVECTOR2(m_fAbsX, m_fAbsY);
		m_Image.v2Max = D3DXVECTOR2(m_fAbsX + m_fWidth * m_fPercentage, m_fAbsY + m_fHeight);
		m_Image.v2UVMin = D3DXVECTOR2((float)rcSrc.left / (float)sizeImage.cx, (float)rcSrc.top / (float)sizeImage.cy);
		m_Image.v2UVMax.x = ((float)(rcSrc.right - rcSrc.left) * m_fPercentage + (float)rcSrc.left) / (float)sizeImage.cx;
		m_Image.v2UVMax.y = (float)rcSrc.bottom / (float)sizeImage.cy;
		break;
	case ITEMIMAGE_PART_RIGHT_TO_LEFT:
		m_Image.v2Min = D3DXVECTOR2(m_fAbsX + m_fWidth * (1.0f - m_fPercentage), m_fAbsY);
		m_Image.v2Max = D3DXVECTOR2(m_fAbsX + m_fWidth, m_fAbsY + m_fHeight);
		m_Image.v2UVMin.x = ((float)rcSrc.left + (float)(rcSrc.right - rcSrc.left) * (1.0f - m_fPercentage)) / (float)sizeImage.cx;
		m_Image.v2UVMin.y = (float)rcSrc.top / (float)sizeImage.cy;
		m_Image.v2UVMax = D3DXVECTOR2((float)rcSrc.right / (float)sizeImage.cx, (float)rcSrc.bottom / (float)sizeImage.cy);
		break;
	case ITEMIMAGE_PART_TOP_TO_BOTTOM:
		m_Image.v2Min = D3DXVECTOR2(m_fAbsX, m_fAbsY);
		m_Image.v2Max = D3DXVECTOR2(m_fAbsX + m_fWidth, m_fAbsY + m_fHeight * m_fPercentage);
		m_Image.v2UVMin = D3DXVECTOR2((float)rcSrc.left / (float)sizeImage.cx, (float)rcSrc.top / (float)sizeImage.cy);
		m_Image.v2UVMax.x = (float)rcSrc.right / (float)sizeImage.cx;
		m_Image.v2UVMax.y = ((float)(rcSrc.bottom - rcSrc.top) * m_fPercentage + (float)rcSrc.top) / (float)sizeImage.cy;
		break;
	case ITEMIMAGE_PART_BOTTOM_TO_TOP:
		m_Image.v2Min = D3DXVECTOR2(m_fAbsX, m_fAbsY + m_fHeight * (1.0f - m_fPercentage));
		m_Image.v2Max = D3DXVECTOR2(m_fAbsX + m_fWidth, m_fAbsY + m_fHeight);
		m_Image.v2UVMin.x = (float)rcSrc.left / (float)sizeImage.cx;
		m_Image.v2UVMin.y = ((float)rcSrc.top + (float)(rcSrc.bottom - rcSrc.top) * (1.0f - m_fPercentage)) / (float)sizeImage.cy;
		m_Image.v2UVMax = D3DXVECTOR2((float)rcSrc.right / (float)sizeImage.cx, (float)rcSrc.bottom / (float)sizeImage.cy);
		break;
	case ITEMIMAGE_FLIP_HORIZONTAL:
		m_Image.v2Min = D3DXVECTOR2(m_fAbsX, m_fAbsY);
		m_Image.v2Max = D3DXVECTOR2(m_fAbsX + m_fWidth, m_fAbsY + m_fHeight);
		m_Image.v2UVMin = D3DXVECTOR2((float)(rcSrc.right) / (float)sizeImage.cx, (float)(rcSrc.top) / (float)sizeImage.cy);
		m_Image.v2UVMax = D3DXVECTOR2((float)(rcSrc.left) / (float)sizeImage.cx, (float)(rcSrc.bottom) / (float)sizeImage.cy);
		break;
	case ITEMIMAGE_FLIP_VERTICAL:
		m_Image.v2Min = D3DXVECTOR2(m_fAbsX, m_fAbsY);
		m_Image.v2Max = D3DXVECTOR2(m_fAbsX + m_fWidth, m_fAbsY + m_fHeight);
		m_Image.v2UVMin = D3DXVECTOR2((float)(rcSrc.left) / (float)sizeImage.cx, (float)(rcSrc.bottom) / (float)sizeImage.cy);
		m_Image.v2UVMax = D3DXVECTOR2((float)(rcSrc.right) / (float)sizeImage.cx, (float)(rcSrc.top) / (float)sizeImage.cy);
		break;
	case ITEMIMAGE_FLIP_CENTRAL:
		m_Image.v2Min = D3DXVECTOR2(m_fAbsX, m_fAbsY);
		m_Image.v2Max = D3DXVECTOR2(m_fAbsX + m_fWidth, m_fAbsY + m_fHeight);
		m_Image.v2UVMin = D3DXVECTOR2((float)(rcSrc.right) / (float)sizeImage.cx, (float)(rcSrc.bottom) / (float)sizeImage.cy);
		m_Image.v2UVMax = D3DXVECTOR2((float)(rcSrc.left) / (float)sizeImage.cx, (float)(rcSrc.top) / (float)sizeImage.cy);
		break;
	default:
		m_Image.v2Min = D3DXVECTOR2(m_fAbsX, m_fAbsY);
		m_Image.v2Max = D3DXVECTOR2(m_fAbsX + m_fWidth, m_fAbsY + m_fHeight);
		m_Image.v2UVMin = D3DXVECTOR2((float)(rcSrc.left) / (float)sizeImage.cx, (float)(rcSrc.top) / (float)sizeImage.cy);
		m_Image.v2UVMax = D3DXVECTOR2((float)(rcSrc.right) / (float)sizeImage.cx, (float)(rcSrc.bottom) / (float)sizeImage.cy);
		m_Image.dwType = KG3DUI_PART_TYPE_TIMER;
		m_Image.fRatio = m_fPercentage;
		break;
	}

Exit1:
	nResult = true;
Exit0:
	if (!nResult)
	{
		ZeroMemory(&m_Image, sizeof(m_Image));
	}
	m_dwStyle |= ITEM_DATA_FORMATED;
	return nResult;

}

void KItemImage::DrawNinePart()
{
	KG3DUIImage2 Image[9];
	float fW = 0.0f;
	float fH = 0.0f;
	float fUW = 0.0f;
	float fUH = 0.0f;

	KG_PROCESS_ERROR(m_nImageInfoIndex >= 0);
	KImageInfo* pImageInfo = g_pUI->m_ImageInfoMgr.GetImageInfo(m_nImageInfoIndex);
	KG_PROCESS_ERROR(pImageInfo);
	KG_PROCESS_ERROR(m_nFrame >= 0 && m_nFrame < pImageInfo->GetFrameCount());

	RECT const& rcSrc = pImageInfo->GetImageRect(m_nFrame);
	SIZE const& sizeImage = pImageInfo->GetImageSize();
	fW = (float)((int)(rcSrc.right - rcSrc.left) / 3);
	fH = (float)((int)(rcSrc.bottom - rcSrc.top) / 3);
	fUW = fW / (float)sizeImage.cx;
	fUH = fH / (float)sizeImage.cy;
	fW = min(m_fWidth / 3, fW);
	fH = min(m_fHeight / 3, fH);

	Image[0].crDiffuse = m_Image.crDiffuse;
	Image[0].dwImageID = m_Image.dwImageID;
	Image[0].v2Min = m_Image.v2Min;
	Image[0].v2Max.x = m_Image.v2Min.x + fW;
	Image[0].v2Max.y = m_Image.v2Min.y + fH;
	Image[0].v2UVMin = m_Image.v2UVMin;
	Image[0].v2UVMax.x = m_Image.v2UVMin.x + fUW;
	Image[0].v2UVMax.y = m_Image.v2UVMin.y + fUH;

	Image[1].crDiffuse = m_Image.crDiffuse;
	Image[1].dwImageID = m_Image.dwImageID;
	Image[1].v2Min.x = m_Image.v2Min.x + fW;
	Image[1].v2Min.y = m_Image.v2Min.y;
	Image[1].v2Max.x = m_Image.v2Max.x - fW;
	Image[1].v2Max.y = m_Image.v2Min.y + fH;
	Image[1].v2UVMin.x = m_Image.v2UVMin.x + fUW;
	Image[1].v2UVMin.y = m_Image.v2UVMin.y;
	Image[1].v2UVMax.x = m_Image.v2UVMax.x - fUW;
	Image[1].v2UVMax.y = m_Image.v2UVMin.y + fUH;

	Image[2].crDiffuse = m_Image.crDiffuse;
	Image[2].dwImageID = m_Image.dwImageID;
	Image[2].v2Min.x = m_Image.v2Max.x - fW;
	Image[2].v2Min.y = m_Image.v2Min.y;
	Image[2].v2Max.x = m_Image.v2Max.x;
	Image[2].v2Max.y = m_Image.v2Min.y + fH;
	Image[2].v2UVMin.x = m_Image.v2UVMax.x - fUW;
	Image[2].v2UVMin.y = m_Image.v2UVMin.y;
	Image[2].v2UVMax.x = m_Image.v2UVMax.x;
	Image[2].v2UVMax.y = m_Image.v2UVMin.y + fUH;

	Image[3].crDiffuse = m_Image.crDiffuse;
	Image[3].dwImageID = m_Image.dwImageID;
	Image[3].v2Min.x = m_Image.v2Min.x;
	Image[3].v2Min.y = m_Image.v2Min.y + fH;
	Image[3].v2Max.x = m_Image.v2Min.x + fW;
	Image[3].v2Max.y = m_Image.v2Max.y - fH;
	Image[3].v2UVMin.x = m_Image.v2UVMin.x;
	Image[3].v2UVMin.y = m_Image.v2UVMin.y + fUH;
	Image[3].v2UVMax.x = m_Image.v2UVMin.x + fUW;
	Image[3].v2UVMax.y = m_Image.v2UVMax.y - fUH;

	Image[4].crDiffuse = m_Image.crDiffuse;
	Image[4].dwImageID = m_Image.dwImageID;
	Image[4].v2Min.x = m_Image.v2Min.x + fW;
	Image[4].v2Min.y = m_Image.v2Min.y + fH;
	Image[4].v2Max.x = m_Image.v2Max.x - fW;
	Image[4].v2Max.y = m_Image.v2Max.y - fH;
	Image[4].v2UVMin.x = m_Image.v2UVMin.x + fUW;
	Image[4].v2UVMin.y = m_Image.v2UVMin.y + fUH;
	Image[4].v2UVMax.x = m_Image.v2UVMax.x - fUW;
	Image[4].v2UVMax.y = m_Image.v2UVMax.y - fUH;

	Image[5].crDiffuse = m_Image.crDiffuse;
	Image[5].dwImageID = m_Image.dwImageID;
	Image[5].v2Min.x = m_Image.v2Max.x - fW;
	Image[5].v2Min.y = m_Image.v2Min.y + fH;
	Image[5].v2Max.x = m_Image.v2Max.x;
	Image[5].v2Max.y = m_Image.v2Max.y - fH;
	Image[5].v2UVMin.x = m_Image.v2UVMax.x - fUW;
	Image[5].v2UVMin.y = m_Image.v2UVMin.y + fUH;
	Image[5].v2UVMax.x = m_Image.v2UVMax.x;
	Image[5].v2UVMax.y = m_Image.v2UVMax.y - fUH;

	Image[6].crDiffuse = m_Image.crDiffuse;
	Image[6].dwImageID = m_Image.dwImageID;
	Image[6].v2Min.x = m_Image.v2Min.x;
	Image[6].v2Min.y = m_Image.v2Max.y - fH;
	Image[6].v2Max.x = m_Image.v2Min.x + fW;
	Image[6].v2Max.y = m_Image.v2Max.y;
	Image[6].v2UVMin.x = m_Image.v2UVMin.x;
	Image[6].v2UVMin.y = m_Image.v2UVMax.y - fUH;
	Image[6].v2UVMax.x = m_Image.v2UVMin.x + fUW;
	Image[6].v2UVMax.y = m_Image.v2UVMax.y;

	Image[7].crDiffuse = m_Image.crDiffuse;
	Image[7].dwImageID = m_Image.dwImageID;
	Image[7].v2Min.x = m_Image.v2Min.x + fW;
	Image[7].v2Min.y = m_Image.v2Max.y - fH;
	Image[7].v2Max.x = m_Image.v2Max.x - fW;
	Image[7].v2Max.y = m_Image.v2Max.y;
	Image[7].v2UVMin.x = m_Image.v2UVMin.x + fUW;
	Image[7].v2UVMin.y = m_Image.v2UVMax.y - fUH;
	Image[7].v2UVMax.x = m_Image.v2UVMax.x - fUW;
	Image[7].v2UVMax.y = m_Image.v2UVMax.y;

	Image[8].crDiffuse = m_Image.crDiffuse;
	Image[8].dwImageID = m_Image.dwImageID;
	Image[8].v2Min.x = m_Image.v2Max.x - fW;
	Image[8].v2Min.y = m_Image.v2Max.y - fH;
	Image[8].v2Max.x = m_Image.v2Max.x;
	Image[8].v2Max.y = m_Image.v2Max.y;
	Image[8].v2UVMin.x = m_Image.v2UVMax.x - fUW;
	Image[8].v2UVMin.y = m_Image.v2UVMax.y - fUH;
	Image[8].v2UVMax.x = m_Image.v2UVMax.x;
	Image[8].v2UVMax.y = m_Image.v2UVMax.y;

	g_pUI->m_p3DUI->DrawMultiImage(Image, 9);

Exit0:
	return;
}

void KItemImage::DrawLeftCenterRight()
{
	KG3DUIImage2 Image[3];
	float fW = 0.0f;
	float fUW = 0.0f;

	KG_PROCESS_ERROR(m_nImageInfoIndex >= 0);
	KImageInfo* pImageInfo = g_pUI->m_ImageInfoMgr.GetImageInfo(m_nImageInfoIndex);
	KG_PROCESS_ERROR(pImageInfo);
	KG_PROCESS_ERROR(m_nFrame >= 0 && m_nFrame < pImageInfo->GetFrameCount());

	RECT const& rcSrc = pImageInfo->GetImageRect(m_nFrame);
	SIZE const& sizeImage = pImageInfo->GetImageSize();
	fW = (float)((int)(rcSrc.right - rcSrc.left) / 3);
	fUW = fW / (float)sizeImage.cx;
	fW = min(m_fWidth / 3, fW);

	Image[0].crDiffuse = m_Image.crDiffuse;
	Image[0].dwImageID = m_Image.dwImageID;
	Image[0].v2Min = m_Image.v2Min;
	Image[0].v2Max.x = m_Image.v2Min.x + fW;
	Image[0].v2Max.y = m_Image.v2Max.y;
	Image[0].v2UVMin = m_Image.v2UVMin;
	Image[0].v2UVMax.x = m_Image.v2UVMin.x + fUW;
	Image[0].v2UVMax.y = m_Image.v2UVMax.y;

	Image[1].crDiffuse = m_Image.crDiffuse;
	Image[1].dwImageID = m_Image.dwImageID;
	Image[1].v2Min.x = m_Image.v2Min.x + fW;
	Image[1].v2Min.y = m_Image.v2Min.y;
	Image[1].v2Max.x = m_Image.v2Max.x - fW;
	Image[1].v2Max.y = m_Image.v2Max.y;
	Image[1].v2UVMin.x = m_Image.v2UVMin.x + fUW;
	Image[1].v2UVMin.y = m_Image.v2UVMin.y;
	Image[1].v2UVMax.x = m_Image.v2UVMax.x - fUW;
	Image[1].v2UVMax.y = m_Image.v2UVMax.y;

	Image[2].crDiffuse = m_Image.crDiffuse;
	Image[2].dwImageID = m_Image.dwImageID;
	Image[2].v2Min.x = m_Image.v2Max.x - fW;
	Image[2].v2Min.y = m_Image.v2Min.y;
	Image[2].v2Max.x = m_Image.v2Max.x;
	Image[2].v2Max.y = m_Image.v2Max.y;
	Image[2].v2UVMin.x = m_Image.v2UVMax.x - fUW;
	Image[2].v2UVMin.y = m_Image.v2UVMin.y;
	Image[2].v2UVMax.x = m_Image.v2UVMax.x;
	Image[2].v2UVMax.y = m_Image.v2UVMax.y;

	g_pUI->m_p3DUI->DrawMultiImage(Image, 3);

Exit0:
	return;
}

void KItemImage::DrawTopCenterBottom()
{
	KG3DUIImage2 Image[3];
	float fH = 0.0f;
	float fUH = 0.0f;

	KG_PROCESS_ERROR(m_nImageInfoIndex >= 0);
	KImageInfo* pImageInfo = g_pUI->m_ImageInfoMgr.GetImageInfo(m_nImageInfoIndex);
	KG_PROCESS_ERROR(pImageInfo);
	KG_PROCESS_ERROR(m_nFrame >= 0 && m_nFrame < pImageInfo->GetFrameCount());

	RECT const& rcSrc = pImageInfo->GetImageRect(m_nFrame);
	SIZE const& sizeImage = pImageInfo->GetImageSize();
	fH = (float)((int)(rcSrc.bottom - rcSrc.top) / 3);
	fUH = fH / (float)sizeImage.cy;
	fH = min(m_fHeight / 3, fH);

	Image[0].crDiffuse = m_Image.crDiffuse;
	Image[0].dwImageID = m_Image.dwImageID;
	Image[0].v2Min = m_Image.v2Min;
	Image[0].v2Max.x = m_Image.v2Max.x;
	Image[0].v2Max.y = m_Image.v2Min.y + fH;
	Image[0].v2UVMin = m_Image.v2UVMin;
	Image[0].v2UVMax.x = m_Image.v2UVMax.x;
	Image[0].v2UVMax.y = m_Image.v2UVMin.y + fUH;

	Image[1].crDiffuse = m_Image.crDiffuse;
	Image[1].dwImageID = m_Image.dwImageID;
	Image[1].v2Min.x = m_Image.v2Min.x;
	Image[1].v2Min.y = m_Image.v2Min.y + fH;
	Image[1].v2Max.x = m_Image.v2Max.x;
	Image[1].v2Max.y = m_Image.v2Max.y - fH;
	Image[1].v2UVMin.x = m_Image.v2UVMin.x;
	Image[1].v2UVMin.y = m_Image.v2UVMin.y + fUH;
	Image[1].v2UVMax.x = m_Image.v2UVMax.x;
	Image[1].v2UVMax.y = m_Image.v2UVMax.y - fUH;

	Image[2].crDiffuse = m_Image.crDiffuse;
	Image[2].dwImageID = m_Image.dwImageID;
	Image[2].v2Min.x = m_Image.v2Min.x;
	Image[2].v2Min.y = m_Image.v2Max.y - fH;
	Image[2].v2Max.x = m_Image.v2Max.x;
	Image[2].v2Max.y = m_Image.v2Max.y;
	Image[2].v2UVMin.x = m_Image.v2UVMin.x;
	Image[2].v2UVMin.y = m_Image.v2UVMax.y - fUH;
	Image[2].v2UVMax.x = m_Image.v2UVMax.x;
	Image[2].v2UVMax.y = m_Image.v2UVMax.y;

	g_pUI->m_p3DUI->DrawMultiImage(Image, 3);

Exit0:
	return;
}

} //namespace UI