////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : itemtreeleaf.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2005-11-29 13:41:48
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "./itemtreeleaf.h"
#include "./itemeventmgr.h"
#include "./decoder.h"
#include "../../common/ksmallobjectpool.h"
#include "../../script/kscriptmgr.h"
#include "../../KGUI.h"

namespace
{
	KSmallObjectPool	g_Allocator("KItemTreeLeaf", sizeof(UI::KItemTreeLeaf));
}

namespace UI
{

DWORD KItemTreeLeaf::ms_dwItemType = ITEM_TREE_LEAFE;

KItemTreeLeaf::KItemTreeLeaf()
{
	m_nImageInfoIndex = -1;
	m_nExpandIconFrame = -1;;
	m_nCollapseIconFrame = -1;
    m_nShowIndex = -1;
    m_dwLineColor = 0;
    m_fIconWidth = 0;
    m_fIconHeight = 0;
    m_fEachIndentWidth = 0;
    m_nIndent = -1;
    m_dwStyle &= ~ITEM_TREE_LEAFE_EXPAND;
    m_dwStyle &= ~ITEM_TREE_LEAFE_NODE;
    m_dwStyle |= ITEM_TREE_LEAFE_SHOW_LINE;
}

KItemTreeLeaf::~KItemTreeLeaf()
{

}

void* KItemTreeLeaf::operator new(size_t /* nSize */, std::nothrow_t const&) throw()
{ 
	return g_Allocator.Alloc(); 
}

void KItemTreeLeaf::operator delete(void* pData, std::nothrow_t const&) throw()
{ 
	g_Allocator.Free(pData); 
}

void* KItemTreeLeaf::operator new(size_t /* nSize */)
{ 
	return g_Allocator.Alloc(); 
}

void KItemTreeLeaf::operator delete(void* pData) throw()
{ 
	g_Allocator.Free(pData); 
}

#ifdef KGUI_LUA_LOADER
int KItemTreeLeaf::Create(IKUIObject **ppObj, Lua_State *pLua, int nTableIndex)
{
    int nResult  = false;
    int nRetCode = false;

    KItemNull *pItem = NULL;

    KGLOG_PROCESS_ERROR(ppObj);

    pItem = new(std::nothrow) KItemTreeLeaf;
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

KItemTreeLeaf *KItemTreeLeaf::Create(KItemTreeLeafData &ItemTreeLeafData)
{
    KItemTreeLeaf *pItemTreeLeaf = NULL;
    int nRetCode = false;

    pItemTreeLeaf = new(std::nothrow) KItemTreeLeaf;
    KG_PROCESS_ERROR(pItemTreeLeaf);

    nRetCode = pItemTreeLeaf->Init(ItemTreeLeafData);
	KG_PROCESS_ERROR(nRetCode);
	pItemTreeLeaf->OnInitEnd(ItemTreeLeafData);

Exit0:
    if (!nRetCode)
        SAFE_RELEASE(pItemTreeLeaf);
    return pItemTreeLeaf;
}

void KItemTreeLeaf::Release()
{
	if (m_nImageInfoIndex != -1)
		g_pUI->m_ImageInfoMgr.UnloadImageInfo(m_nImageInfoIndex);
	KItemHandle::Release();
}

#ifdef KGUI_LUA_LOADER
int KItemTreeLeaf::Init(Lua_State *pLua, int nTableIndex)
{
	int nResult  = false;
	int nRetCode = false;

    char szIconImage[MAX_PATH];
    char szLineColor[KD_SMALL_BUFFER_SIZE];
    int nExpandIconFrame = 0;
    int nCollapseIconFrame = 0;
    int nShowLine = false;
    int nAlwaysNode = false;

    KLuaTableField aParamTable[] = 
    {
        KLUA_TABLE_FIELD("image",             emVAR_TYPE_STRING, "",    szIconImage,         _countof(szIconImage)),
        KLUA_TABLE_FIELD("ExpandIconFrame",   emVAR_TYPE_INT,    -1,    &nExpandIconFrame,   0),
        KLUA_TABLE_FIELD("CollapseIconFrame", emVAR_TYPE_INT,    -1,    &nCollapseIconFrame, 0),
        KLUA_TABLE_FIELD("ShowLine",          emVAR_TYPE_INT,    true,  &nShowLine,          0),
        KLUA_TABLE_FIELD("Indent",            emVAR_TYPE_INT,    -1,    &m_nIndent,          0),
        KLUA_TABLE_FIELD("IndentWidth",       emVAR_TYPE_FLOAT,  0,     &m_fEachIndentWidth, 0),
        KLUA_TABLE_FIELD("IconWidth",         emVAR_TYPE_FLOAT,  0,     &m_fIconWidth,       0),
        KLUA_TABLE_FIELD("IconHeight",        emVAR_TYPE_FLOAT,  0,     &m_fIconHeight,      0),
        KLUA_TABLE_FIELD("AlwaysNode",        emVAR_TYPE_INT,    false, &nAlwaysNode,        0),
        KLUA_TABLE_FIELD("LineColor",         emVAR_TYPE_STRING, "",    &szLineColor,        _countof(szLineColor)),
    };

    KGLOG_PROCESS_ERROR(pLua);

    nRetCode = KItemNull::Init(pLua, nTableIndex);
    KGLOG_PROCESS_ERROR(nRetCode);

    for (int i = 0; i < _countof(aParamTable); i++)
    {
        nRetCode = g_LuaGetTableField(pLua, nTableIndex, &aParamTable[i]);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    if (szIconImage[0])
    {
        FormatFilePath(szIconImage);
        SetIconImage(szIconImage, nExpandIconFrame, nCollapseIconFrame);
    }
    
    if (szLineColor[0])
    {
        KColorSchemeMgr& ColorSchemeMgr = g_pUI->m_ColorSchemeMgr;
        int nColorScheme = ColorSchemeMgr.FindScheme(ATL::CA2W(szLineColor, GetCodePage()));
        if (ColorSchemeMgr.IsExistColor(nColorScheme))
            m_dwLineColor= ColorSchemeMgr.GetColor(nColorScheme);
    }

    g_SetBitFlag(nShowLine, ITEM_TREE_LEAFE_SHOW_LINE, m_dwStyle);

    if (m_fIconWidth == 0 && m_fIconHeight == 0)
        AutoSetIconSize();

    if (nAlwaysNode)
    {		
        m_dwStyle |= ITEM_TREE_LEAFE_ALWAYS_NODE;
        SetNode();
    }

// Exit1:
	nResult = true;
Exit0:
	return nResult;
}
#endif

int KItemTreeLeaf::Init(KItemTreeLeafData &ItemTreeLeafData)
{
    int nResult = false;
    int nRetCode = false;

    nRetCode = KItemHandle::Init(ItemTreeLeafData);
    KG_PROCESS_ERROR(nRetCode);

    SetIconImage(
        ItemTreeLeafData.szImageName, 
        ItemTreeLeafData.nExpandIconFrame, 
        ItemTreeLeafData.nCollapseIconFrame
    );
    m_nIndent = ItemTreeLeafData.nIndent;
    m_fEachIndentWidth = ItemTreeLeafData.fEachIndentWidth;
	if (!ItemTreeLeafData.nDisableScale)
		m_fEachIndentWidth *= ItemTreeLeafData.fScale;
    m_dwLineColor = ItemTreeLeafData.dwLineColor;
    if (ItemTreeLeafData.nShowLine)
        m_dwStyle |= ITEM_TREE_LEAFE_SHOW_LINE;
    else
        m_dwStyle &= ~ITEM_TREE_LEAFE_SHOW_LINE;

    if (ItemTreeLeafData.nAutoIconSize)
	{
        AutoSetIconSize();
	}
    else
    {
        m_fIconWidth = ItemTreeLeafData.fIconWidth * ItemTreeLeafData.fScale;
        m_fIconHeight = ItemTreeLeafData.fIconHeight * ItemTreeLeafData.fScale;
    }

	if (ItemTreeLeafData.nAlwaysNode)
	{
		m_dwStyle |= ITEM_TREE_LEAFE_ALWAYS_NODE;
		SetNode();
	}

    nResult = true;
Exit0:
    return nResult;
}

#ifndef DISABLE_UI_EDIT
LRESULT KItemTreeLeaf::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return KItemHandle::WndProc(uMsg, wParam, lParam);
}
#endif

int KItemTreeLeaf::SetIconImage(LPCSTR szImageName, int nExpandFrame, int nCollapseFrame)
{
    int nResult = false;
    int nRetCode = false;

    KG_PROCESS_ERROR(szImageName);
    KG_PROCESS_ERROR(szImageName[0] != '\0');
	KG_PROCESS_ERROR(nExpandFrame >= -1);
	KG_PROCESS_ERROR(nCollapseFrame >= -1);

	if (m_nImageInfoIndex != -1)
	{
		g_pUI->m_ImageInfoMgr.UnloadImageInfo(m_nImageInfoIndex);
		m_nImageInfoIndex = -1;
	}

    m_nImageInfoIndex = g_pUI->m_ImageInfoMgr.LoadImageInfo(szImageName);
	KG_PROCESS_ERROR(m_nImageInfoIndex >= 0);

    m_nCollapseIconFrame = nCollapseFrame;
    m_nExpandIconFrame = nExpandFrame;

	m_dwStyle &= ~ITEM_TREE_LEAFE_NODE_FORMATED;
    nResult = true;
Exit0:
    return nResult;
}

void KItemTreeLeaf::Draw()
{
    if(!(m_dwStyle & ITEM_SHOW))
		return;

#ifndef DISABLE_UI_EDIT
	KItemNull::Draw();
#endif

    if (m_dwStyle & ITEM_TREE_LEAFE_SHOW_LINE && m_aLine.size())
        g_pUI->m_p3DUI->DrawMultiLine(&m_aLine[0], m_aLine.size());

    if (m_dwStyle & ITEM_TREE_LEAFE_NODE)
	{
		if (!(m_dwStyle & ITEM_TREE_LEAFE_NODE_FORMATED))
			FormateIconImage();
		g_pUI->m_p3DUI->DrawMultiImage(&m_Image, 1);
	}
}

DWORD KItemTreeLeaf::GetItemType()
{
    return ms_dwItemType;
}

int KItemTreeLeaf::IsItemCanChangeTo(DWORD dwType)
{
    if (
        dwType == ITEM_TREE_LEAFE ||
        dwType == ITEM_HANDLE ||
        dwType == ITEM_NULL
    )
        return true;
    return false;
}

int KItemTreeLeaf::SetAbsPos(float fAbsX, float fAbsY)
{
    AbsMove(fAbsX - m_fAbsX, fAbsY - m_fAbsY);
    return true;
}

int KItemTreeLeaf::AbsMove(float fDeltaX, float fDeltaY)
{
    KItemHandle::AbsMove(fDeltaX, fDeltaY);

	KTreeLeafLinkLineArray::iterator it = m_aLine.begin();
	KTreeLeafLinkLineArray::iterator ie = m_aLine.end();
    for (; it != ie; ++it) 
    {
        (*it).v2Min.x += fDeltaX;
        (*it).v2Min.y += fDeltaY;
        (*it).v2Max.x += fDeltaX;
        (*it).v2Max.y += fDeltaY;
    }

    m_Image.v2Min.x += fDeltaX;
    m_Image.v2Min.y += fDeltaY;
    m_Image.v2Max.x += fDeltaX;
    m_Image.v2Max.y += fDeltaY;

    return true;
}

int KItemTreeLeaf::Scale(float fScaleX, float fScaleY)
{
	if (m_dwStyle & ITEM_NEVER_SCALED)
	{
		KItemNull::Scale(fScaleX, fScaleY);
		return true;
	}

	m_fEachIndentWidth *= fScaleX;
    m_fIconWidth *= fScaleX;
    m_fIconHeight *= fScaleY;
    AdjustNodeIconPos();

    KItemHandle::Scale(fScaleX, fScaleY);
    FormatAllItemPos();

    return true;
}

int KItemTreeLeaf::FormatAllItemPos()
{
    float fX = 0.0f;
    float fTotalIndentWidth = m_fEachIndentWidth * (m_nIndent + 1);
    fX = m_fAbsX;
    
    m_fWidth -= fTotalIndentWidth;
    KItemHandle::FormatAllItemPos();
    KItemHandle::AbsMove(m_fEachIndentWidth * (m_nIndent + 1), 0);
    m_fWidth += fTotalIndentWidth;
    m_fAbsX = fX; 
    return true;
}


int KItemTreeLeaf::AppendLineLinkToPrev()
{
    KG3DUILine2 LineToPrev;
    LineToPrev.crMin = m_dwLineColor;
    LineToPrev.crMax = m_dwLineColor;
    LineToPrev.v2Min.x = m_fAbsX + m_nIndent * m_fEachIndentWidth + m_fEachIndentWidth / 2.0f;
    LineToPrev.v2Min.y = m_fAbsY;
    LineToPrev.v2Max.x = LineToPrev.v2Min.x;
    if (m_dwStyle & ITEM_TREE_LEAFE_NODE)
        LineToPrev.v2Max.y = m_fAbsY + (m_fHeight - m_fIconHeight) / 2.0f;
    else
        LineToPrev.v2Max.y = m_fAbsY + m_fHeight / 2.0f;

    m_aLine.push_back(LineToPrev);

    return true;
}

int KItemTreeLeaf::AppendLineLinkToNext()
{
    KG3DUILine2 LineToNext;
    LineToNext.crMin = m_dwLineColor;
    LineToNext.crMax = m_dwLineColor;
    LineToNext.v2Min.x = m_fAbsX + m_nIndent * m_fEachIndentWidth + m_fEachIndentWidth / 2.0f;
    if (m_dwStyle & ITEM_TREE_LEAFE_NODE)
        LineToNext.v2Min.y = m_fAbsY + (m_fHeight + m_fIconHeight) / 2.0f;
    else
        LineToNext.v2Min.y = m_fAbsY + m_fHeight / 2.0f;

    LineToNext.v2Max.x = LineToNext.v2Min.x;
    LineToNext.v2Max.y = m_fAbsY + m_fHeight;

    m_aLine.push_back(LineToNext);

    return true;
}

int KItemTreeLeaf::AppendLineLinkToItem()
{
    KG3DUILine2 LineToItem;
    LineToItem.crMin = m_dwLineColor;
    LineToItem.crMax = m_dwLineColor;

    if (m_dwStyle & ITEM_TREE_LEAFE_NODE)
        LineToItem.v2Min.x = m_fAbsX + m_nIndent * m_fEachIndentWidth + m_fEachIndentWidth / 2.0f + m_fIconWidth / 2.0f;
    else
        LineToItem.v2Min.x = m_fAbsX + m_nIndent * m_fEachIndentWidth + m_fEachIndentWidth / 2.0f;

    LineToItem.v2Min.y = m_fAbsY + m_fHeight / 2.0f;

    LineToItem.v2Max.x = m_fAbsX + (m_nIndent + 1) * m_fEachIndentWidth;
    LineToItem.v2Max.y = LineToItem.v2Min.y;

    m_aLine.push_back(LineToItem);

    return true;
}

int KItemTreeLeaf::AppendLineLinkPrevAndNext(int nIndent)
{
    ASSERT(nIndent < m_nIndent);
    KG3DUILine2 LineLinkPrevAndNext;
    LineLinkPrevAndNext.crMin = m_dwLineColor;
    LineLinkPrevAndNext.crMax = m_dwLineColor;
    LineLinkPrevAndNext.v2Min.x = m_fAbsX + nIndent * m_fEachIndentWidth + m_fEachIndentWidth / 2.0f;
    LineLinkPrevAndNext.v2Min.y = m_fAbsY;
    LineLinkPrevAndNext.v2Max.x = LineLinkPrevAndNext.v2Min.x;
    LineLinkPrevAndNext.v2Max.y = m_fAbsY + m_fHeight;

    m_aLine.push_back(LineLinkPrevAndNext);

    return true;
}

int KItemTreeLeaf::ClearAllLine()
{
    m_aLine.clear();
    return true;
}

int KItemTreeLeaf::GetIndent()
{
    return m_nIndent;
}

int KItemTreeLeaf::SetIndent(int nIndent)
{
    m_nIndent = nIndent;

    return true;
}

int KItemTreeLeaf::SetEachIndentWidth(float fWidth)
{
	if (fWidth < 0.0f)
		fWidth = 0.0f;
    m_fEachIndentWidth = fWidth;

	return true;
}

float KItemTreeLeaf::GetEachIndentWidth()
{
    return m_fEachIndentWidth;
}

int KItemTreeLeaf::IsExpand()
{
    return (m_dwStyle & ITEM_TREE_LEAFE_EXPAND) != 0;
}

int KItemTreeLeaf::Expand()
{
    m_dwStyle |= ITEM_TREE_LEAFE_EXPAND;
	m_dwStyle &= ~ITEM_TREE_LEAFE_NODE_FORMATED;

    return true;
}

int KItemTreeLeaf::Collapse()
{
    m_dwStyle &= ~ITEM_TREE_LEAFE_EXPAND;
	m_dwStyle &= ~ITEM_TREE_LEAFE_NODE_FORMATED;

    return true;
}

int KItemTreeLeaf::IsNode()
{
    return (m_dwStyle & ITEM_TREE_LEAFE_NODE) != 0;
}

int KItemTreeLeaf::SetNode()
{
    m_dwStyle |= ITEM_TREE_LEAFE_NODE;

	return true;
}

int KItemTreeLeaf::ClearNode()
{
	if (m_dwStyle & ITEM_TREE_LEAFE_ALWAYS_NODE)
		return true;

	m_dwStyle &= ~ITEM_TREE_LEAFE_NODE;

    return true;
}

int KItemTreeLeaf::SetNodeIconSize(float fWidth, float fHeight)
{
	KG_PROCESS_ERROR(fWidth > 0.0f);
	KG_PROCESS_ERROR(fHeight > 0.0f);
    m_fIconWidth = fWidth;
    m_fIconHeight = fHeight;

Exit0:
	return true;
}

int KItemTreeLeaf::AdjustNodeIconPos()
{
    m_Image.v2Min.x = m_fAbsX + m_nIndent * m_fEachIndentWidth + (m_fEachIndentWidth - m_fIconWidth) / 2.0f;
    m_Image.v2Min.y = m_fAbsY + (m_fHeight - m_fIconHeight) / 2.0f;
    m_Image.v2Max.x = m_Image.v2Min.x + m_fIconWidth;
    m_Image.v2Max.y = m_Image.v2Min.y + m_fIconHeight;

	return true;
}

int KItemTreeLeaf::AutoSetIconSize()
{
    int nResult = false;
    SIZE size = { 0, 0 };
    KImageInfo *pImageInfo = NULL;

    pImageInfo = g_pUI->m_ImageInfoMgr.GetImageInfo(m_nImageInfoIndex);
    KG_PROCESS_ERROR(pImageInfo);

    size = pImageInfo->GetFrameSize(m_nImageInfoIndex);
    m_fIconWidth = static_cast<float>(size.cx);
    m_fIconHeight = static_cast<float>(size.cy);

	if (!(m_dwStyle & ITEM_NEVER_SCALED))
	{
		float fScale = g_pUI->m_WndStation.GetUIScale();
		m_fIconWidth *= fScale;
		m_fIconHeight *= fScale;
	}

	AdjustNodeIconPos();

    nResult = true;
Exit0:
    return nResult;
}

int KItemTreeLeaf::PtInIcon(float fAbsX, float fAbsY)
{
    if(!(m_dwStyle & ITEM_TREE_LEAFE_NODE))
        return false;

    return fAbsX >= m_Image.v2Min.x && 
        fAbsX <= m_Image.v2Max.x &&
        fAbsY >= m_Image.v2Min.y &&
        fAbsY <= m_Image.v2Max.y;
}

int KItemTreeLeaf::SetShowIndex(int nIndex)
{
    m_nShowIndex = nIndex;
    return true;
}

int KItemTreeLeaf::GetShowIndex()
{
    return m_nShowIndex;
}

int KItemTreeLeaf::FormateIconImage()
{
	int nResult = false;
	SIZE sizeImage;
	int nShowFrame = m_nCollapseIconFrame;
	KG_PROCESS_ERROR(m_nImageInfoIndex >= 0);

	if (m_dwStyle & ITEM_TREE_LEAFE_EXPAND)
		nShowFrame = m_nExpandIconFrame;

	KImageInfo* pImageInfo = g_pUI->m_ImageInfoMgr.GetImageInfo(m_nImageInfoIndex);
	KG_PROCESS_ERROR(pImageInfo);

	KG_PROCESS_ERROR(nShowFrame >= 0 && nShowFrame < pImageInfo->GetFrameCount());

	RECT const& rcSrc = pImageInfo->GetImageRect(nShowFrame);
	sizeImage = pImageInfo->GetImageSize();

	m_Image.dwImageID = pImageInfo->GetImageID(nShowFrame);
	m_Image.crDiffuse = m_dwAlpha << 24;
	m_Image.v2UVMin = D3DXVECTOR2((float)(rcSrc.left) / (float)sizeImage.cx, (float)(rcSrc.top) / (float)sizeImage.cy);
	m_Image.v2UVMax = D3DXVECTOR2((float)(rcSrc.right) / (float)sizeImage.cx, (float)(rcSrc.bottom) / (float)sizeImage.cy);

	AdjustNodeIconPos();

	nResult = true;
Exit0:
	if (!nResult)
	{
		ZeroMemory(&m_Image, sizeof(m_Image));
	}
	m_dwStyle |= ITEM_TREE_LEAFE_NODE_FORMATED;
	return nResult;
}

}   //namespace UI

