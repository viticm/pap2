////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : itemhandle.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2005-11-9 17:18:24
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "./itemhandle.h"
#include "./itemeventmgr.h"
#include "./itemtreeleaf.h"
#include "./decoder.h"
#include "../wndwindow.h"
#include "../wndmessage.h"
#include "../../config/kdebugconfig.h"
#include "../../common/ksmallobjectpool.h"
#include "../../script/kscriptmgr.h"
#include "../../KGUI.h"


////////////////////////////////////////////////////////////////////////////////

#define K_ITEM_SHOW_TEST(pItem, fX, fY, fWidth, fHeight)			\
	if (m_dwStyle & ITEM_HANDLEL_CONTROL_SHOW)				\
	{																\
		if (m_dwStyle & ITEM_HANDLE_USE_STENCIL)					\
		{															\
			if (UI_FLOAT_SMALL(fX + fWidth, m_fAbsX) ||				\
				UI_FLOAT_LARGE(fX, m_fAbsX + m_fWidth) ||			\
				UI_FLOAT_SMALL(fY + fHeight, m_fAbsY) ||			\
				UI_FLOAT_LARGE(fY, m_fAbsY + m_fHeight))			\
				pItem->Hide();										\
			else													\
				pItem->Show();										\
		}															\
		else														\
		{															\
			if (UI_FLOAT_SMALL(fX, m_fAbsX) ||						\
				UI_FLOAT_LARGE(fX + fWidth, m_fAbsX + m_fWidth) ||	\
				UI_FLOAT_SMALL(fY, m_fAbsY) ||						\
				UI_FLOAT_LARGE(fY + fHeight, m_fAbsY + m_fHeight))	\
				pItem->Hide();										\
			else													\
				pItem->Show();										\
		}															\
	}

#define K_RICH_ITEM_SHOW_TEST_H(pItem, fX, fY, fWidth, fHeight)										\
	if (m_dwStyle & ITEM_HANDLEL_CONTROL_SHOW)														\
	{																								\
		if (m_dwStyle & ITEM_HANDLE_USE_STENCIL)													\
		{																							\
			KItemText *pText = g_ItemCast<KItemText>(pItem);										\
			if (pText)																				\
				pText->AdjustShowAll();																\
			if (UI_FLOAT_SMALL(fX + fWidth, m_fAbsX) || UI_FLOAT_LARGE(fX, m_fAbsX + m_fWidth))		\
				pItem->Hide();																		\
			else																					\
				pItem->Show();																		\
		}																							\
		else																						\
		{																							\
			KItemText *pText = g_ItemCast<KItemText>(pItem);										\
			if (pText && pText->IsRichText())														\
				pText->AdjustShowTextH(m_fAbsX, m_fAbsX + m_fWidth);								\
			else																					\
			{																						\
				if (UI_FLOAT_SMALL(fX, m_fAbsX) || UI_FLOAT_LARGE(fX + fWidth, m_fAbsX + m_fWidth))	\
					pItem->Hide();																	\
				else																				\
					pItem->Show();																	\
			}																						\
		}																							\
	}

#define K_RICH_ITEM_SHOW_TEST_V(pItem, fX, fY, fWidth, fHeight)										\
	if (m_dwStyle & ITEM_HANDLEL_CONTROL_SHOW)														\
	{																								\
		if (m_dwStyle & ITEM_HANDLE_USE_STENCIL)													\
		{																							\
			KItemText *pText = g_ItemCast<KItemText>(pItem);										\
			if (pText)																				\
				pText->AdjustShowAll();																\
			if (UI_FLOAT_SMALL(fY + fHeight, m_fAbsY) || UI_FLOAT_LARGE(fY, m_fAbsY + m_fHeight))	\
				pItem->Hide();																		\
			else																					\
				pItem->Show();																		\
		}																							\
		else																						\
		{																							\
			KItemText *pText = g_ItemCast<KItemText>(pItem);										\
			if (pText && pText->IsRichText())														\
				pText->AdjustShowTextV(m_fAbsY, m_fAbsY + m_fHeight);								\
			else																					\
			{																						\
				if (UI_FLOAT_SMALL(fY, m_fAbsY) || UI_FLOAT_LARGE(fY + fHeight, m_fAbsY + m_fHeight))	\
					pItem->Hide();																	\
				else																				\
					pItem->Show();																	\
			}																						\
		}																							\
	}

namespace
{
	KSmallObjectPool	g_Allocator("KItemHandle", sizeof(UI::KItemHandle));
}

namespace UI
{

DWORD KItemHandle::ms_dwItemType = ITEM_HANDLE;

KItemHandle::KItemHandle()
{
    m_dwHandleType   = ITEM_HANDLE_CUSTOM;
	m_dwFirstItemPosType	= POSITION_BY_CUSTOM;
    m_fMinRowHeight         = 0.0f;
    m_fMaxRowHeight         = 0.0f;
    m_fRowSpacing           = 0.0f;
    m_fItemStartRelX        = 0.0f;
    m_fItemStartRelY        = 0.0f;
	m_fAllItemHeight		= 0.0f;
	m_fAllItemWidth			= 0.0f;
	m_pWndOwner				= NULL;
}

KItemHandle::~KItemHandle()
{
}

void* KItemHandle::operator new(size_t /* nSize */, std::nothrow_t const&) throw()
{ 
	return g_Allocator.Alloc(); 
}

void KItemHandle::operator delete(void* pData, std::nothrow_t const&) throw()
{ 
	g_Allocator.Free(pData); 
}

void* KItemHandle::operator new(size_t /* nSize */)
{ 
	return g_Allocator.Alloc(); 
}

void KItemHandle::operator delete(void* pData) throw()
{ 
	g_Allocator.Free(pData); 
}

#ifdef KGUI_LUA_LOADER
int KItemHandle::Create(IKUIObject **ppObj, Lua_State *pLua, int nTableIndex)
{
    int nResult  = false;
    int nRetCode = false;

    KItemNull *pItem = NULL;

    KGLOG_PROCESS_ERROR(ppObj);

    pItem = new(std::nothrow) KItemHandle;
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

KItemHandle *KItemHandle::Create(KItemHandleData &ItemHandleData)
{
    KItemHandle *pItemHandle = NULL;
    int nRetCode = false;

    pItemHandle = new(std::nothrow) KItemHandle;
    KG_PROCESS_ERROR(pItemHandle);

    nRetCode = pItemHandle->Init(ItemHandleData);
	KG_PROCESS_ERROR(nRetCode);
	pItemHandle->OnInitEnd(ItemHandleData);
Exit0:
    if(!nRetCode)
        SAFE_RELEASE(pItemHandle);    
    return pItemHandle;
}

#ifndef DISABLE_UI_EDIT
LRESULT KItemHandle::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
	case WM_LBUTTONDOWN:
        for (int i = (int)(m_aItem.size()) - 1; i >= 0; --i)
            if (m_aItem[i]->WndProc(uMsg, wParam, lParam))
                return TRUE;
		break;
	case WM_MOUSEMOVE:
		for (int i = (int)(m_aItem.size()) - 1; i >= 0; --i)
			if (m_aItem[i]->WndProc(uMsg, wParam, lParam))
				return TRUE;
		break;
    }
    return KItemNull::WndProc(uMsg, wParam, lParam);
}

int KItemHandle::PtInMeOrMyChild(float fAbsX, float fAbsY)
{
    if (PtInItem(fAbsX, fAbsY))
        return true;

    for (KItemArray::iterator it = m_aItem.begin(); it != m_aItem.end(); ++it)
    {
        KItemHandle *pHandle = g_ItemCast<KItemHandle>(*it);
        if (pHandle && pHandle->PtInMeOrMyChild(fAbsX, fAbsY))
            return true;
        if ((*it)->PtInItem(fAbsX, fAbsY))
            return true;
    }

    return false;
}
#endif // #ifndef DISABLE_UI_EDIT


void KItemHandle::Release()
{
    Clear();
	KItemNull::Release();
}

#ifdef KGUI_LUA_LOADER
int KItemHandle::Init(Lua_State *pLua, int nTableIndex)
{
	int nResult  = false;
	int nRetCode = false;

    int nPixelScroll = false;

    KLuaTableField aParamTable[] = 
    {
        KLUA_TABLE_FIELD("type",             emVAR_TYPE_INT,   0,     &m_dwHandleType,       0),
        KLUA_TABLE_FIELD("MinRowHeight",     emVAR_TYPE_FLOAT, 0,     &m_fMinRowHeight,      0),
        KLUA_TABLE_FIELD("MaxRowHeight",     emVAR_TYPE_FLOAT, 0,     &m_fMaxRowHeight,      0),
        KLUA_TABLE_FIELD("RowSpacing",       emVAR_TYPE_FLOAT, 0,     &m_fRowSpacing,        0),
        KLUA_TABLE_FIELD("FirstItemPosType", emVAR_TYPE_INT,   0,     &m_dwFirstItemPosType, 0),
        KLUA_TABLE_FIELD("PixelScroll",      emVAR_TYPE_INT,   false, &nPixelScroll,         0),
    };

    KGLOG_PROCESS_ERROR(pLua);

    nRetCode = KItemNull::Init(pLua, nTableIndex);
    KGLOG_PROCESS_ERROR(pLua);

    m_emObjType = em_OBJ_TYPE_HANDLE;      // 指定类型

    for (int i = 0; i < _countof(aParamTable); i++)
    {
        nRetCode = g_LuaGetTableField(pLua, nTableIndex, &aParamTable[i]);
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    g_SetBitFlag(!m_fWidth && !m_fHeight, ITEM_HANDLE_AUOT_ADJUST_SIZE_BY_CHILD, m_dwStyle); // 宽和高都为零是设置为自动
    g_SetBitFlag(nPixelScroll, ITEM_HANDLE_USE_STENCIL, m_dwStyle);

// Exit1:
	nResult = true;
Exit0:
	return nResult;
}

int KItemHandle::AddChild(IKUIObject *pChild)
{
	int nResult  = false;
	int nRetCode = false;

    KGLOG_PROCESS_ERROR(pChild);

    switch(pChild->m_emObjType)
    {
    case em_OBJ_TYPE_ITEM:
    case em_OBJ_TYPE_HANDLE:
        nRetCode = AppendItem((KItemNull *)pChild);
        KGLOG_PROCESS_ERROR(nRetCode);
        break;
    default:
        ASSERT(false && "类型错误");
        break;
    }

// Exit1:
	nResult = true;
Exit0:
	return nResult;
}
#endif

int KItemHandle::Init(KItemHandleData &ItemHandleData)
{
    int nResult = false;
    int nRetCode = false;

    nRetCode = KItemNull::Init(ItemHandleData);
    KG_PROCESS_ERROR(nRetCode);

    m_dwHandleType = static_cast<DWORD>(ItemHandleData.nHandleType);
    m_dwFirstItemPosType = static_cast<DWORD>(ItemHandleData.nFirstItemPosType);
    m_fRowSpacing = ItemHandleData.fRowSpacing * ItemHandleData.fScale;
    m_fMinRowHeight = ItemHandleData.fRowHeight * ItemHandleData.fScale;
    m_fMaxRowHeight = ItemHandleData.fMaxRowHeight * ItemHandleData.fScale;
	if (ItemHandleData.nAutoSize)
		m_dwStyle |= ITEM_HANDLE_AUOT_ADJUST_SIZE_BY_CHILD;
	if (ItemHandleData.nPixelScroll)
		m_dwStyle |= ITEM_HANDLE_USE_STENCIL;
	if (ItemHandleData.nControlShow)
		m_dwStyle |= ITEM_HANDLEL_CONTROL_SHOW;

	if (IsIntPos())
	{
		m_fRowSpacing = floor(m_fRowSpacing);
		m_fMinRowHeight = floor(m_fMinRowHeight);
		m_fMaxRowHeight = floor(m_fMaxRowHeight);
	}

    nResult = true;
Exit0:
    return nResult;
}

void KItemHandle::Draw()
{
#ifndef DISABLE_UI_EDIT
    if(!(m_dwStyle & ITEM_SHOW))
		return;

    KItemNull::Draw();
#endif //_DEBUG
	if (m_dwStyle & ITEM_HANDLE_DRAW_STENCIL_BEGIN)
	{
		g_pUI->m_p3DUI->EndStencil();
		m_dwStyle &= ~ITEM_HANDLE_DRAW_STENCIL_BEGIN;
		return;
	}

	if (m_dwStyle & ITEM_HANDLE_USE_STENCIL)
	{
		g_pUI->m_p3DUI->BeginStencil();
		
		KG3DUIRect2 rcShadow;
		rcShadow.crDiffuse = 0xFFFFFFFF;
		rcShadow.v2Min = D3DXVECTOR2(m_fAbsX, m_fAbsY);
		rcShadow.v2Max = D3DXVECTOR2(m_fAbsX + m_fWidth, m_fAbsY + m_fHeight);
		if (NeedExtent())
			rcShadow.v2Max.x += 30;
		g_pUI->m_p3DUI->FillMultiRect(&rcShadow, 1);

		g_pUI->m_p3DUI->SetStencilFunc();

		m_dwStyle |= ITEM_HANDLE_DRAW_STENCIL_BEGIN;
	}

}

int KItemHandle::AppendItem(KItemNull *pItem)
{
    int nRetCode = false;
    int nResult = false;

    KG_PROCESS_ERROR(pItem);

    m_aItem.push_back(pItem);
	pItem->SetParent(this);

	if (m_dwStyle & ITEM_HANDLE_FORMAT_POS_WHEN_APPEND)
		FormatLastItemPos();

    nRetCode = pItem->UpdateLevel(m_nLevel + 1);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KItemHandle::InsertItem(int nPos, KItemNull *pItem)
{
    int nResult = false;
    int nRetCode = false;
    LPCSTR pcszItemName = NULL;
    int nSize = 0;
    
    KG_PROCESS_ERROR(pItem);

    nSize = (int)m_aItem.size();
    if (nPos < 0)
        nPos = 0;

    if (nPos > nSize)
        nPos = nSize;

    m_aItem.insert(m_aItem.begin() + nPos, pItem);
	pItem->SetParent(this);
	if (m_dwStyle & ITEM_HANDLE_FORMAT_POS_WHEN_APPEND)
		FormatAllItemPos();

    nRetCode = pItem->UpdateLevel(m_nLevel + 1);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KItemHandle::InsertItem(KItemNull *pItem, KItemNull *pItemReference, int nBeforeOrBehind)
{
    int nResult = false;
    int nRetCode = false;
    KItemArray::iterator it;

    KG_PROCESS_ERROR(pItem);
    KG_PROCESS_ERROR(pItemReference);

	it = std::find(m_aItem.begin(), m_aItem.end(), pItemReference);
    KG_PROCESS_ERROR(it != m_aItem.end());

    if (nBeforeOrBehind != INSERT_BEFORE)
		++it;
	m_aItem.insert(it, pItem);
	pItem->SetParent(this);
	if (m_dwStyle & ITEM_HANDLE_FORMAT_POS_WHEN_APPEND)
		FormatAllItemPos();

    nRetCode = pItem->UpdateLevel(m_nLevel + 1);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

DWORD KItemHandle::GetItemType()
{
    return ms_dwItemType;
}

int KItemHandle::IsItemCanChangeTo(DWORD dwType)
{
    if (
        dwType == ITEM_HANDLE || 
        dwType == ITEM_NULL
    )
        return true;
    return false;
}

int KItemHandle::SetAbsPos(float fAbsX, float fAbsY)
{
    AbsMove(fAbsX - m_fAbsX, fAbsY - m_fAbsY);
    return true;
}

int KItemHandle::SetSizeByAllItemSize()
{
    m_fWidth = m_fAllItemWidth;
    m_fHeight = m_fAllItemHeight;

	if (IsIntPos())
	{
		m_fWidth = floor(m_fWidth);
		m_fHeight = floor(m_fHeight);
	}

	SetItemStartRelPos(0.0f, 0.0f);
//	AdjustItemShowInfo();

    return true;
}

int KItemHandle::SetItemStartRelPos(float fRelX, float fRelY)
{
	if (IsIntPos())
	{
		fRelX = floor(fRelX);
		fRelY = floor(fRelY);
	}

    switch(m_dwHandleType) 
    {
    case ITEM_HANDLE_CUSTOM:
    case ITEM_HANDLE_AUTO_NEW_LINE_ROW_HEIGHT_CHANGELESS:
    case ITEM_HANDLE_AUTO_NEW_LINE_ROW_HEIGHT_BETWEEN_MIN_MAX:
    case ITEM_HANDLE_AUTO_NEW_LINE_ROW_HEIGHT_BY_MAX_ITEM:
	case ITEM_HANDLE_LEFT_RIGHT_ALIGN_BY_TABLE:
        for (KItemArray::iterator it = m_aItem.begin(); it != m_aItem.end(); ++it) 
        {
            ASSERT(*it);
            (*it)->AbsMove(fRelX - m_fItemStartRelX, fRelY - m_fItemStartRelY);
        }    
        m_fItemStartRelX = fRelX;
        m_fItemStartRelY = fRelY;
        AdjustItemShowInfo();  
        break;
    case ITEM_HANDLE_CHAT_ITEM_HANDLE:
        m_fItemStartRelX = fRelX;
        m_fItemStartRelY = fRelY;
        FormatAllItemPosByChatItemHandle();
        break;
    case ITEM_HANDLE_TREE_ITEM_HANDLE:
        m_fItemStartRelX = fRelX;
        m_fItemStartRelY = fRelY;
        FormatAllItemPosByTreeItemHandle();
    	break;
    default:
        break;
    }
    return true;
}

int KItemHandle::GetItemStartRelPos(float &fRelX, float &fRelY)
{
	fRelX = m_fItemStartRelX;
	fRelY = m_fItemStartRelY;
	return true;
}

int KItemHandle::AbsMove(float fDeltaX, float fDeltaY)
{
    KItemNull::AbsMove(fDeltaX, fDeltaY);
    for (KItemArray::iterator it = m_aItem.begin(); it != m_aItem.end(); ++it) 
    {
        ASSERT(*it);
        (*it)->AbsMove(fDeltaX, fDeltaY);
    }
    return true;
}

int KItemHandle::Scale(float fScaleX, float fScaleY)
{

	KItemNull::Scale(fScaleX, fScaleY);

	if (m_dwStyle & ITEM_NEVER_SCALED)
		return true;
    

    for (KItemArray::iterator it = m_aItem.begin(); it != m_aItem.end(); ++it)
    {
        ASSERT(*it);
        (*it)->Scale(fScaleX, fScaleY);
    }

	m_fMinRowHeight *= fScaleY;
	m_fMaxRowHeight *= fScaleY;
	m_fRowSpacing *= fScaleY;

	m_fItemStartRelX *= fScaleX;
	m_fItemStartRelY *= fScaleY;
	m_fAllItemWidth *= fScaleX;
	m_fAllItemHeight *= fScaleY;

	if (IsIntPos())
	{
		m_fMinRowHeight = floor(m_fMinRowHeight);
		m_fMaxRowHeight = floor(m_fMaxRowHeight);
		m_fRowSpacing = floor(m_fRowSpacing);
		m_fItemStartRelX = floor(m_fItemStartRelX);
		m_fItemStartRelY = floor(m_fItemStartRelY);
	}

    FormatAllItemPos();
    return true;
}

int KItemHandle::SetAlpha(int nAlpha)
{
	for (KItemArray::iterator it = m_aItem.begin(); it != m_aItem.end(); ++it)
	{
		ASSERT(*it);
		(*it)->SetAlpha(nAlpha);
	}
	KItemNull::SetAlpha(nAlpha);
	return true;
}

int KItemHandle::SetHandleStyle(DWORD dwHandleStyle)
{
    int nResult = false;

    KG_PROCESS_ERROR(dwHandleStyle >= ITEM_HANDLE_STYLE_BEGINE);
    KG_PROCESS_ERROR(dwHandleStyle < ITEM_HANDEL_STYLE_END);

    m_dwHandleType = dwHandleStyle;

    nResult = true;
Exit0:
    return nResult;
}

int KItemHandle::SetMinRowHeight(float fHeight)
{
    m_fMinRowHeight = fHeight;
	if (m_fMinRowHeight < 0.0f)
		m_fMinRowHeight = 0.0f;
	if (IsIntPos())
	{
		m_fMinRowHeight = floor(m_fMinRowHeight);
	}
	return true;
}

int KItemHandle::SetMaxRowHeight(float fHeight)
{
    m_fMaxRowHeight = fHeight;
	if (m_fMaxRowHeight < 0.0f)
		m_fMaxRowHeight = 0.0f;
	if (IsIntPos())
	{
		m_fMaxRowHeight = floor(m_fMaxRowHeight);
	}
	return true;
}

int KItemHandle::SetRowHeight(float fHeight)
{
    return SetMinRowHeight(fHeight);
}

int KItemHandle::SetRowSpacing(float fSpacing)
{
    m_fRowSpacing = fSpacing;

	if (IsIntPos())
	{
		m_fRowSpacing = floor(m_fRowSpacing);
	}

	return true;
}

float KItemHandle::GetRowSpacing()
{
	return m_fRowSpacing;
}

int KItemHandle::GetAllItemSize(float &fWidth, float &fHeight)
{
    fWidth = m_fAllItemWidth;
    fHeight = m_fAllItemHeight;
    return true;
}

int KItemHandle::GetItemCount()
{
    return (int)m_aItem.size();
}

int KItemHandle::FormatAllItemPos()
{
    int nResult = false;

	if (m_dwStyle & ITEM_HANDLE_AUOT_ADJUST_SIZE_BY_CHILD)
	{
		SetSize(100000.0f, 100000.0f);
	}

    switch(m_dwHandleType) 
    {
    case ITEM_HANDLE_CUSTOM:
        FormatAllItemPosByCustom();
    	break;
    case ITEM_HANDLE_AUTO_NEW_LINE_ROW_HEIGHT_CHANGELESS:
        FormatAllItemPosByAutoNewLineRowHeightChangeless();
        break;
    case ITEM_HANDLE_AUTO_NEW_LINE_ROW_HEIGHT_BETWEEN_MIN_MAX:
        FormatAllItemPosByAutoNewLineRowHeightBetweenMinMax();
        break;
    case ITEM_HANDLE_AUTO_NEW_LINE_ROW_HEIGHT_BY_MAX_ITEM:
        FormatAllItemPosByAutoNewLineRowHeightByMaxItem();
        break;
    case ITEM_HANDLE_CHAT_ITEM_HANDLE:
        FormatAllItemPosByChatItemHandle();
        break;
    case ITEM_HANDLE_TREE_ITEM_HANDLE:
        FormatAllItemPosByTreeItemHandle();
        break;
	case ITEM_HANDLE_LEFT_RIGHT_ALIGN_BY_TABLE:
		FormatAllItemPosByLeftRightAlignByTable();
		break;
    default:
        KG_ASSERT_EXIT(0 && "No Style Seted!");
        break;
    }

	if (m_dwStyle & ITEM_HANDLE_AUOT_ADJUST_SIZE_BY_CHILD)
	{
		SetSizeByAllItemSize();
	}


    nResult = true;
Exit0:
    return nResult;
}

int KItemHandle::IsHandleRichTextHandle() const
{
	switch (m_dwHandleType) 
	{
	case ITEM_HANDLE_AUTO_NEW_LINE_ROW_HEIGHT_CHANGELESS:
	case ITEM_HANDLE_AUTO_NEW_LINE_ROW_HEIGHT_BETWEEN_MIN_MAX:
	case ITEM_HANDLE_AUTO_NEW_LINE_ROW_HEIGHT_BY_MAX_ITEM:
	case ITEM_HANDLE_LEFT_RIGHT_ALIGN_BY_TABLE:
		return true;
	}
	return false;
}

int KItemHandle::AdjustItemShowInfo()
{
    float fX = 0.0f; 
    float fY = 0.0f;
    float fWidth = 0.0f;
    float fHeight = 0.0f;
    float fEndX = m_fAbsX + m_fItemStartRelX;
    float fEndY = m_fAbsY + m_fItemStartRelY;

	if (IsHandleRichTextHandle())
	{
		if (m_dwStyle & ITEM_HANDLE_AJUST_RICH_TEXT_SHOW_H)
		{
			for (KItemArray::iterator it = m_aItem.begin(); it != m_aItem.end(); ++it) 
			{
				ASSERT(*it);
				(*it)->GetAbsPos(fX, fY);
				(*it)->GetSize(fWidth, fHeight);
				fEndX = max(fEndX, fX + fWidth);
				fEndY = max(fEndY, fY + fHeight);

				K_RICH_ITEM_SHOW_TEST_H((*it), fX, fY, fWidth, fHeight);
			}
		}
		else
		{
			for (KItemArray::iterator it = m_aItem.begin(); it != m_aItem.end(); ++it) 
			{
				ASSERT(*it);
				(*it)->GetAbsPos(fX, fY);
				(*it)->GetSize(fWidth, fHeight);
				fEndX = max(fEndX, fX + fWidth);
				fEndY = max(fEndY, fY + fHeight);

				K_RICH_ITEM_SHOW_TEST_V((*it), fX, fY, fWidth, fHeight);
			}
		}

		goto Exit1;
	}

    for (KItemArray::iterator it = m_aItem.begin(); it != m_aItem.end(); ++it) 
    {
        ASSERT(*it);
        (*it)->GetAbsPos(fX, fY);
        (*it)->GetSize(fWidth, fHeight);
        fEndX = max(fEndX, fX + fWidth);
        fEndY = max(fEndY, fY + fHeight);

		K_ITEM_SHOW_TEST((*it), fX, fY, fWidth, fHeight)
    }

Exit1:
    m_fAllItemWidth = fEndX - m_fAbsX - m_fItemStartRelX;
    m_fAllItemHeight = fEndY - m_fAbsY - m_fItemStartRelY;

    return true;
}

int KItemHandle::FormatAllItemPosByChatItemHandle()
{
	return FormatAllItemPosByAutoNewLineRowHeightByMaxItem();
}

int KItemHandle::FormatAllItemPosByTreeItemHandle()
{
    int nResult = false;
    float fX = m_fAbsX + m_fItemStartRelX;
    float fY = m_fAbsY + m_fItemStartRelY;
    float fWidth = 0;
    float fHeight = 0;
    float fShowStartY = 0.0f;
    float fShowEndY = 0.0f;
    KItemTreeLeaf *pTreeLeaf = NULL;
    KItemTreeLeaf *pTreeLeafNext = NULL;
    int i = 0;
    int nShowIndex = 0;
    int nItemCount = static_cast<int>(m_aItem.size());
    int nIndent = 0;
    int nSkip = false;
    int nlastCollaspeItemIndent = 0; 

    for (i = 0; i < nItemCount; ++i) 
    {
        ASSERT(m_aItem[i]);
        pTreeLeaf = g_ItemCast<KItemTreeLeaf>(m_aItem[i]);
        KG_ASSERT_EXIT(pTreeLeaf);

        if (nSkip && pTreeLeaf->GetIndent() > nlastCollaspeItemIndent)
        {
            pTreeLeaf->SetShowIndex(-1);
            pTreeLeaf->Hide();
            continue;
        }
        nSkip = false;

        pTreeLeaf->GetSize(fWidth, fHeight);
        pTreeLeaf->SetAbsPos(fX, fY);

        pTreeLeaf->ClearAllLine();
        nIndent = pTreeLeaf->GetIndent();
        if (i < nItemCount - 1)
        {
            pTreeLeafNext = g_ItemCast<KItemTreeLeaf>(m_aItem[i + 1]);
            KG_ASSERT_EXIT(pTreeLeafNext);
            if (pTreeLeafNext->GetIndent() == nIndent + 1)
                pTreeLeaf->SetNode();
            else
                pTreeLeaf->ClearNode();
        }
        else
            pTreeLeaf->ClearNode();

        if (
            pTreeLeaf->IsNode() && 
            !(pTreeLeaf->IsExpand())
        )
        {
            nlastCollaspeItemIndent = pTreeLeaf->GetIndent();
            nSkip = true;
        }

        pTreeLeaf->AdjustNodeIconPos();

        pTreeLeaf->AppendLineLinkToItem();
        pTreeLeaf->AppendLineLinkToPrev();
        for (int nIndex = i - 1; nIndex >= 0; --nIndex) 
        {
            pTreeLeafNext = g_ItemCast<KItemTreeLeaf>(m_aItem[nIndex]);
            KG_ASSERT_EXIT(pTreeLeafNext);
            if (pTreeLeafNext->GetIndent() > nIndent)
                pTreeLeafNext->AppendLineLinkPrevAndNext(nIndent);
            else if (pTreeLeafNext->GetIndent() == nIndent)
            {
                pTreeLeafNext->AppendLineLinkToNext();
                break;
            }
            else
                break;
        }

        pTreeLeaf->SetShowIndex(nShowIndex);
        ++nShowIndex;
		pTreeLeaf->Show();
		K_ITEM_SHOW_TEST(pTreeLeaf, fX, fY, fWidth, fHeight)

       fY += fHeight;        
    }

    m_fAllItemHeight = fY - m_fAbsY - m_fItemStartRelY;
    m_fAllItemWidth = m_fWidth;

    nResult = true;
Exit0:
    return nResult;
}

int KItemHandle::FormatAllItemPosByAutoNewLineRowHeightChangeless()
{
    float fWidth            = 0.0f;
    float fHeight           = 0.0f;
    float fX                = 0.0f;
    float fStartX       = m_fAbsX + m_fItemStartRelX;
    float fStartY       = m_fAbsY + m_fItemStartRelY;
    float fRowEndY      = fStartY + m_fMinRowHeight;
	KItemNull *pItemNull = NULL;
	KItemText *pItemText = NULL;

	for (KItemArray::iterator it = m_aItem.begin(); it != m_aItem.end(); ++it)
	{
		pItemNull = (*it);
		ASSERT(pItemNull);
		pItemText = g_ItemCast<KItemText>(pItemNull);
		if (!pItemText || !(pItemText->IsRichText()))
		{
			//不是文字,或者该段文字不能由handle设置绘制数据。
			pItemNull->GetSize(fWidth, fHeight);
/*
			if (UI_FLOAT_LARGE(fHeight, m_fMinRowHeight))
			{
				//需要等比缩放，还是覆盖上面的文字在这里操作。
				fHeight = m_fMinRowHeight;
				pItemNull->SetSize(fWidth, fHeight);
			}
			if (UI_FLOAT_LARGE(fWidth, m_fWidth))
			{
				//如果宽度超过了最大高度。如果等比索放在这里操作
				fWidth = m_fWidth;
				pItemNull->SetSize(fWidth, fHeight);
			}
*/
			if (UI_FLOAT_LARGE(fX + fWidth, m_fWidth) && !UI_FLOAT_EQUAL(fX, 0.0f))
			{
				//这一行已经放不下了，从下一行直接开始。
				fX = 0.0f;
				fRowEndY += (m_fMinRowHeight + m_fRowSpacing);
			}

			pItemNull->SetAbsPos(fStartX + fX, fRowEndY - fHeight);
			fX += fWidth;
			continue;
		}

		//是文字,并且该文字是handle设置绘制数据。
		LPCWSTR wszText = pItemText->GetText();
		float fPos = m_fWidth - fX;
		int nControlType = 0;
		int nContinueLoop = true;
		float fTableWidth = pItemText->GetTableTextWidth();
		pItemText->ClearDrawTextData();
		pItemText->SetAbsPos(fStartX + fX, fRowEndY);
		fHeight = pItemText->GetSingleTextHeight();
		int nPos = pItemText->GetSplitPos(wszText, fPos, nControlType);
		
		while (nPos != -1 && nContinueLoop)
		{
			switch(nControlType)
			{
			case ITEM_TEXT_CONTROL_KEY_TAB:
				pItemText->AppendDrawTextData(
					D3DXVECTOR2(fStartX + fX, fRowEndY - fHeight), 
					D3DXVECTOR2(fStartX + fX + fPos, fRowEndY),
					wszText, nPos
				);
				fX += fPos;
				++nPos;
				fX += fTableWidth;
				if (UI_FLOAT_LARGE(fX, m_fWidth))
				{
					fX = 0.0f;
					fRowEndY += (m_fMinRowHeight + m_fRowSpacing);
				}
				break;
			case ITEM_TEXT_CONTROL_KEY_NEW_LINE:
				pItemText->AppendDrawTextData(
					D3DXVECTOR2(fStartX + fX, fRowEndY - fHeight), 
					D3DXVECTOR2(fStartX + fX + fPos, fRowEndY),
					wszText, nPos
					);
				++nPos;
				fX = 0.0f;
				fRowEndY += (m_fMinRowHeight + m_fRowSpacing);
				break;
			default:
				if (!nPos)
				{
					//用于字符串出现的异常情况(里面存在非法字符的情况)。
					if (UI_FLOAT_LARGE(fX, 0.0f))
					{
						fX = 0.0f;
						fRowEndY += (m_fMinRowHeight + m_fRowSpacing);
						break;
					}
					//m_fWidth 太小以至于无法显示一个文字。所以推出循环。
					nContinueLoop = false;
					break;
				}
				pItemText->AppendDrawTextData(
					D3DXVECTOR2(fStartX + fX, fRowEndY - fHeight), 
					D3DXVECTOR2(fStartX + fX + fPos, fRowEndY),
					wszText, nPos
					);
				fX += fPos;
				if (UI_FLOAT_LARGE_OR_EQUAL(fX, m_fWidth))
				{
					fX = 0.0f;
					fRowEndY += (m_fMinRowHeight + m_fRowSpacing);
				}
				break;
			}
			wszText += nPos;
			fPos = m_fWidth - fX;
			nPos = pItemText->GetSplitPos(wszText, fPos, nControlType);
		}
	}

	AdjustItemShowInfo();

    return true;
}

int KItemHandle::AdjustRowItemEndAbsY(KItemArray::iterator itBegin, KItemArray::iterator itEnd, float fOldY,float fNewY)
{
	KItemNull *pItemNull = NULL;
	KItemText *pItemText = NULL;
	float fWidth = 0.0f;
	float fHeight = 0.0f;
	float fAbsX = 0.0f;
	float fAbsY = 0.0f;

	while(itBegin != itEnd)
	{
		pItemNull = (*itBegin);
		ASSERT(pItemNull);
		pItemText = g_ItemCast<KItemText>(pItemNull);
		if (!pItemText || !(pItemText->IsRichText()))
		{
			pItemNull->GetSize(fWidth, fHeight);
			pItemNull->GetAbsPos(fAbsX, fAbsY);
			ASSERT(UI_FLOAT_EQUAL(fAbsY + fHeight, fOldY));
			pItemNull->SetAbsPos(fAbsX, fNewY - fHeight);
		}
		else
			pItemText->AdjustDrawDataEndAbsY(fOldY, fNewY);
		++itBegin;
	}
	return true;
}

int KItemHandle::FormatAllItemPosByAutoNewLineRowHeightBetweenMinMax()
{
	float fWidth            = 0.0f;
	float fHeight           = 0.0f;
	float fX                = 0.0f;
	float fStartX       = m_fAbsX + m_fItemStartRelX;
	float fStartY       = m_fAbsY + m_fItemStartRelY;
	float fRowEndY      = fStartY + m_fMinRowHeight;
	float fRowHeight    =  m_fMinRowHeight;
	KItemNull *pItemNull = NULL;
	KItemText *pItemText = NULL;

	KItemArray::iterator it = m_aItem.begin(); 
	KItemArray::iterator itRowBegin = m_aItem.begin();
	while (it != m_aItem.end())
	{
		pItemNull = (*it);
		ASSERT(pItemNull);
		pItemText = g_ItemCast<KItemText>(pItemNull);
		if (!pItemText || !(pItemText->IsRichText()))
		{
			//不是文字,或者该段文字不能由handle设置绘制数据。
			pItemNull->GetSize(fWidth, fHeight);
/*
			if (UI_FLOAT_LARGE(fHeight, m_fMaxRowHeight))
			{
				//需要等比缩放，还是覆盖上面的文字在这里操作。
				fHeight = m_fMaxRowHeight;
				pItemNull->SetSize(fWidth, fHeight);
			}
			if (UI_FLOAT_LARGE(fWidth, m_fWidth))
			{
				//如果宽度超过了最大高度。如果等比索放在这里操作
				fWidth = m_fWidth;
				pItemNull->SetSize(fWidth, fHeight);
			}
*/
			if (UI_FLOAT_LARGE(fX + fWidth, m_fWidth) && !UI_FLOAT_EQUAL(fX, 0.0f))
			{
				//这一行已经放不下了，从下一行直接开始。

				float fNewRowEndY = fRowEndY - m_fMinRowHeight + fRowHeight;
				AdjustRowItemEndAbsY(itRowBegin, it, fRowEndY, fNewRowEndY);
				itRowBegin = it;

				fX = 0.0f;
				fRowEndY = fNewRowEndY + (m_fMinRowHeight + m_fRowSpacing);
				fRowHeight = m_fMinRowHeight;
				continue;
			}

			if (UI_FLOAT_LARGE(fHeight, fRowHeight))
				fRowHeight = fHeight;

			pItemNull->SetAbsPos(fStartX + fX, fRowEndY - fHeight);
			fX += fWidth;

			++it;
			continue;
		}

		//是文字,并且该文字是handle设置绘制数据。
		LPCWSTR wszText = pItemText->GetText();
		float fPos = m_fWidth - fX;
		int nControlType = 0;
		int nContinueLoop = true;
		pItemText->ClearDrawTextData();
		pItemText->SetAbsPos(fStartX + fX, fRowEndY);
		float fTableWidth = pItemText->GetTableTextWidth();
		fHeight = pItemText->GetSingleTextHeight();

		int nPos = pItemText->GetSplitPos(wszText, fPos, nControlType);
		while (nPos != -1 && nContinueLoop)
		{
			//文字不管最大高，不然会出错。
			if (UI_FLOAT_LARGE(fHeight, fRowHeight))
				fRowHeight = fHeight;
			if (UI_FLOAT_LARGE(fHeight, m_fMaxRowHeight))
				fRowHeight = m_fMaxRowHeight;
			switch(nControlType)
			{
			case ITEM_TEXT_CONTROL_KEY_TAB:
				pItemText->AppendDrawTextData(
					D3DXVECTOR2(fStartX + fX, fRowEndY - fHeight), 
					D3DXVECTOR2(fStartX + fX + fPos, fRowEndY),
					wszText, nPos
					);
				fX += fPos;
				++nPos;
				fX += fTableWidth;
				if (UI_FLOAT_LARGE(fX,m_fWidth))
				{
					float fNewRowEndY = fRowEndY - m_fMinRowHeight + fRowHeight;
					AdjustRowItemEndAbsY(itRowBegin, it + 1, fRowEndY, fNewRowEndY);
					itRowBegin = it;

					fX = 0.0f;
					fRowEndY = fNewRowEndY + (m_fMinRowHeight + m_fRowSpacing);
					fRowHeight = m_fMinRowHeight;
				}
				break;
			case ITEM_TEXT_CONTROL_KEY_NEW_LINE:
				pItemText->AppendDrawTextData(
					D3DXVECTOR2(fStartX + fX, fRowEndY - fHeight), 
					D3DXVECTOR2(fStartX + fX + fPos, fRowEndY),
					wszText, nPos
					);
				++nPos;
				{
					float fNewRowEndY = fRowEndY - m_fMinRowHeight + fRowHeight;
					AdjustRowItemEndAbsY(itRowBegin, it + 1, fRowEndY, fNewRowEndY);
					itRowBegin = it;

					fX = 0.0f;
					fRowEndY = fNewRowEndY + (m_fMinRowHeight + m_fRowSpacing);
					fRowHeight = m_fMinRowHeight;
				}
				break;
			default:
				if (!nPos)
				{
					//用于字符串出现的异常情况(里面存在非法字符的情况)。
					if (UI_FLOAT_LARGE(fX, 0.0f))
					{
						float fNewRowEndY = fRowEndY - m_fMinRowHeight + fRowHeight;
						AdjustRowItemEndAbsY(itRowBegin, it + 1, fRowEndY, fNewRowEndY);
						itRowBegin = it;

						fX = 0.0f;
						fRowEndY = fNewRowEndY + (m_fMinRowHeight + m_fRowSpacing);
						fRowHeight = m_fMinRowHeight;

						break;
					}
					//m_fWidth 太小以至于无法显示一个文字。所以推出循环。
					nContinueLoop = false;
					break;
				}
				pItemText->AppendDrawTextData(
					D3DXVECTOR2(fStartX + fX, fRowEndY - fHeight), 
					D3DXVECTOR2(fStartX + fX + fPos, fRowEndY),
					wszText, nPos
					);
				fX += fPos;
				if (UI_FLOAT_LARGE_OR_EQUAL(fX, m_fWidth))
				{
					float fNewRowEndY = fRowEndY - m_fMinRowHeight + fRowHeight;
					AdjustRowItemEndAbsY(itRowBegin, it + 1, fRowEndY, fNewRowEndY);
					itRowBegin = it;

					fX = 0.0f;
					fRowEndY = fNewRowEndY + (m_fMinRowHeight + m_fRowSpacing);
					fRowHeight = m_fMinRowHeight;
				}
				break;
			}
			wszText += nPos;
			fPos = m_fWidth - fX;
			nPos = pItemText->GetSplitPos(wszText, fPos, nControlType);
		}

		++it;
	}

	//最后一行
	float fNewRowEndY = fRowEndY - m_fMinRowHeight + fRowHeight;
	AdjustRowItemEndAbsY(itRowBegin, it, fRowEndY, fNewRowEndY);

	AdjustItemShowInfo();

	return true;
}

int KItemHandle::FormatAllItemPosByAutoNewLineRowHeightByMaxItem()
{
	float fWidth            = 0.0f;
	float fHeight           = 0.0f;
	float fX                = 0.0f;
	float fStartX       = m_fAbsX + m_fItemStartRelX;
	float fStartY       = m_fAbsY + m_fItemStartRelY;
	m_fMinRowHeight		= 30.0f;
	float fRowEndY      = fStartY + m_fMinRowHeight;
	float fRowHeight    =  0.0f;
	KItemNull *pItemNull = NULL;
	KItemText *pItemText = NULL;

	KItemArray::iterator it = m_aItem.begin(); 
	KItemArray::iterator itRowBegin = m_aItem.begin();
	while (it != m_aItem.end())
	{
		pItemNull = (*it);
		ASSERT(pItemNull);
		pItemText = g_ItemCast<KItemText>(pItemNull);
		if (!pItemText || !(pItemText->IsRichText()))
		{
			//不是文字,或者该段文字不能由handle设置绘制数据。
			pItemNull->GetSize(fWidth, fHeight);
/*
			if (UI_FLOAT_LARGE(fWidth, m_fWidth))
			{
				//如果宽度超过了最大高度。如果等比索放在这里操作
				fWidth = m_fWidth;
				pItemNull->SetSize(fWidth, fHeight);
			}
*/
			if (UI_FLOAT_LARGE(fX + fWidth, m_fWidth) && !UI_FLOAT_EQUAL(fX, 0.0f))
			{
				//这一行已经放不下了，从下一行直接开始。

				float fNewRowEndY = fRowEndY - m_fMinRowHeight + fRowHeight;
				AdjustRowItemEndAbsY(itRowBegin, it, fRowEndY, fNewRowEndY);
				itRowBegin = it;

				fX = 0.0f;
				fRowEndY = fNewRowEndY + (m_fMinRowHeight + m_fRowSpacing);
				fRowHeight = 0.0f;
				continue;
			}

			if (UI_FLOAT_LARGE(fHeight, fRowHeight))
				fRowHeight = fHeight;

			pItemNull->SetAbsPos(fStartX + fX, fRowEndY - fHeight);
			fX += fWidth;

			++it;
			continue;
		}

		//是文字,并且该文字是handle设置绘制数据。
		LPCWSTR wszText = pItemText->GetText();
		float fPos = m_fWidth - fX;
		int nControlType = 0;
		int nContinueLoop = true;
		float fTableWidth = pItemText->GetTableTextWidth();
		pItemText->ClearDrawTextData();
		pItemText->SetAbsPos(fStartX + fX, fRowEndY);
		fHeight = pItemText->GetSingleTextHeight();

		int nPos = pItemText->GetSplitPos(wszText, fPos, nControlType);
		while (nPos != -1 && nContinueLoop)
		{
			//文字不管最大高，不然会出错。
			if (UI_FLOAT_LARGE(fHeight, fRowHeight))
				fRowHeight = fHeight;
			switch(nControlType)
			{
			case ITEM_TEXT_CONTROL_KEY_TAB:
				pItemText->AppendDrawTextData(
					D3DXVECTOR2(fStartX + fX, fRowEndY - fHeight), 
					D3DXVECTOR2(fStartX + fX + fPos, fRowEndY),
					wszText, nPos
					);
				fX += fPos;
				++nPos;
				fX += fTableWidth;
				if (UI_FLOAT_LARGE(fX,m_fWidth))
				{
					float fNewRowEndY = fRowEndY - m_fMinRowHeight + fRowHeight;
					AdjustRowItemEndAbsY(itRowBegin, it + 1, fRowEndY, fNewRowEndY);
					itRowBegin = it;

					fX = 0.0f;
					fRowEndY = fNewRowEndY + (m_fMinRowHeight + m_fRowSpacing);
					fRowHeight = 0.0f;
				}
				break;
			case ITEM_TEXT_CONTROL_KEY_NEW_LINE:
				pItemText->AppendDrawTextData(
					D3DXVECTOR2(fStartX + fX, fRowEndY - fHeight), 
					D3DXVECTOR2(fStartX + fX + fPos, fRowEndY),
					wszText, nPos
					);
				++nPos;
				{
					float fNewRowEndY = fRowEndY - m_fMinRowHeight + fRowHeight;
					AdjustRowItemEndAbsY(itRowBegin, it + 1, fRowEndY, fNewRowEndY);
					itRowBegin = it;

					fX = 0.0f;
					fRowEndY = fNewRowEndY + (m_fMinRowHeight + m_fRowSpacing);
					fRowHeight = 0.0f;
				}
				break;
			default:
				if (!nPos)
				{
					//用于字符串出现的异常情况, 里面存在非法字符的情况。
					if (UI_FLOAT_LARGE(fX, 0.0f))
					{
						//这一行已经放不下了，换行。
						float fNewRowEndY = fRowEndY - m_fMinRowHeight + fRowHeight;
						AdjustRowItemEndAbsY(itRowBegin, it + 1, fRowEndY, fNewRowEndY);
						itRowBegin = it;

						fX = 0.0f;
						fRowEndY = fNewRowEndY + (m_fMinRowHeight + m_fRowSpacing);
						fRowHeight = 0.0f;

						break;
					}
					//m_fWidth 太小以至于无法显示一个文字。所以推出循环。
					nContinueLoop = false;
					break;
				}
				pItemText->AppendDrawTextData(
					D3DXVECTOR2(fStartX + fX, fRowEndY - fHeight), 
					D3DXVECTOR2(fStartX + fX + fPos, fRowEndY),
					wszText, nPos
					);
				fX += fPos;
				if (UI_FLOAT_LARGE_OR_EQUAL(fX, m_fWidth))
				{
					float fNewRowEndY = fRowEndY - m_fMinRowHeight + fRowHeight;
					AdjustRowItemEndAbsY(itRowBegin, it + 1, fRowEndY, fNewRowEndY);
					itRowBegin = it;

					fX = 0.0f;
					fRowEndY = fNewRowEndY + (m_fMinRowHeight + m_fRowSpacing);
					fRowHeight = 0.0f;
				}
				break;
			}
			wszText += nPos;
			fPos = m_fWidth - fX;
			nPos = pItemText->GetSplitPos(wszText, fPos, nControlType);
		}

		++it;
	}

	//最后一行
	float fNewRowEndY = fRowEndY - m_fMinRowHeight + fRowHeight;
	AdjustRowItemEndAbsY(itRowBegin, it, fRowEndY, fNewRowEndY);

	AdjustItemShowInfo();

	return true;
}

int KItemHandle::FormatAllItemPosByLeftRightAlignByTable()
{
	float fWidth            = 0.0f;
	float fHeight           = 0.0f;
	float fX                = 0.0f;
	float fStartX       = m_fAbsX + m_fItemStartRelX;
	float fStartY       = m_fAbsY + m_fItemStartRelY;
	m_fMinRowHeight		= 30.0f;
	float fRowEndY      = fStartY + m_fMinRowHeight;
	float fRowHeight    =  0.0f;
	KItemNull *pItemNull = NULL;
	KItemText *pItemText = NULL;
	int nNextItemRightAlign = false;

	KItemArray::iterator it = m_aItem.begin(); 
	KItemArray::iterator itRowBegin = m_aItem.begin();
	while (it != m_aItem.end())
	{
		pItemNull = (*it);
		ASSERT(pItemNull);
		pItemText = g_ItemCast<KItemText>(pItemNull);
		if (!pItemText || !(pItemText->IsRichText()))
		{
			//不是文字,或者该段文字不能由handle设置绘制数据。
			pItemNull->GetSize(fWidth, fHeight);
/*
			if (UI_FLOAT_LARGE(fWidth, m_fWidth))
			{
				//如果宽度超过了最大宽度。如果等比索放在这里操作
				fWidth = m_fWidth;
				pItemNull->SetSize(fWidth, fHeight);
			}
*/
			if (nNextItemRightAlign)
			{
				fX = m_fWidth - fWidth;
				nNextItemRightAlign = false;
			}

			if (UI_FLOAT_LARGE(fX + fWidth, m_fWidth) && !UI_FLOAT_EQUAL(fX, 0.0f))
			{
				//这一行已经放不下了，从下一行直接开始。

				float fNewRowEndY = fRowEndY - m_fMinRowHeight + fRowHeight;
				AdjustRowItemEndAbsY(itRowBegin, it, fRowEndY, fNewRowEndY);
				itRowBegin = it;

				fX = 0.0f;
				fRowEndY = fNewRowEndY + (m_fMinRowHeight + m_fRowSpacing);
				fRowHeight = 0.0f;
				continue;
			}

			if (UI_FLOAT_LARGE(fHeight, fRowHeight))
				fRowHeight = fHeight;

			pItemNull->SetAbsPos(fStartX + fX, fRowEndY - fHeight);
			fX += fWidth;

			++it;
			continue;
		}

		//是文字,并且该文字是handle设置绘制数据。
		LPCWSTR wszText = pItemText->GetText();
		float fPos = m_fWidth - fX;
		int nControlType = 0;
		int nContinueLoop = true;
		float fTableWidth = pItemText->GetTableTextWidth();
		pItemText->ClearDrawTextData();
		pItemText->SetAbsPos(fStartX + fX, fRowEndY);
		fHeight = pItemText->GetSingleTextHeight();

		int nPos = pItemText->GetSplitPos(wszText, fPos, nControlType);
		while (nPos != -1 && nContinueLoop)
		{
			//文字不管最大高，不然会出错。
			if (UI_FLOAT_LARGE(fHeight, fRowHeight))
				fRowHeight = fHeight;

			//下一段右对齐
			if (nNextItemRightAlign) 
			{
				fX = m_fWidth - fPos;
				nNextItemRightAlign = false;
			}
			switch(nControlType)
			{
			case ITEM_TEXT_CONTROL_KEY_TAB:
				pItemText->AppendDrawTextData(
					D3DXVECTOR2(fStartX + fX, fRowEndY - fHeight), 
					D3DXVECTOR2(fStartX + fX + fPos, fRowEndY),
					wszText, nPos
					);
				fX += fPos;
				++nPos;
				nNextItemRightAlign = true;
				break;
			case ITEM_TEXT_CONTROL_KEY_NEW_LINE:
				pItemText->AppendDrawTextData(
					D3DXVECTOR2(fStartX + fX, fRowEndY - fHeight), 
					D3DXVECTOR2(fStartX + fX + fPos, fRowEndY),
					wszText, nPos
					);
				++nPos;
				{
					float fNewRowEndY = fRowEndY - m_fMinRowHeight + fRowHeight;
					AdjustRowItemEndAbsY(itRowBegin, it + 1, fRowEndY, fNewRowEndY);
					itRowBegin = it;

					fX = 0.0f;
					fRowEndY = fNewRowEndY + (m_fMinRowHeight + m_fRowSpacing);
					fRowHeight = 0.0f;
				}
				break;
			default:
				if (!nPos)
				{
					//用于字符串出现的异常情况(里面存在非法字符的情况)。
					if (UI_FLOAT_LARGE(fX, 0.0f))
					{
						//这一行已经放不下了，换行。
						float fNewRowEndY = fRowEndY - m_fMinRowHeight + fRowHeight;
						AdjustRowItemEndAbsY(itRowBegin, it + 1, fRowEndY, fNewRowEndY);
						itRowBegin = it;

						fX = 0.0f;
						fRowEndY = fNewRowEndY + (m_fMinRowHeight + m_fRowSpacing);
						fRowHeight = 0.0f;

						break;
					}
					//m_fWidth 太小以至于无法显示一个文字。所以推出循环。
					nContinueLoop = false;
					break;
				}
				pItemText->AppendDrawTextData(
					D3DXVECTOR2(fStartX + fX, fRowEndY - fHeight), 
					D3DXVECTOR2(fStartX + fX + fPos, fRowEndY),
					wszText, nPos
					);
				fX += fPos;
				if (UI_FLOAT_LARGE_OR_EQUAL(fX, m_fWidth))
				{
					float fNewRowEndY = fRowEndY - m_fMinRowHeight + fRowHeight;
					AdjustRowItemEndAbsY(itRowBegin, it + 1, fRowEndY, fNewRowEndY);
					itRowBegin = it;

					fX = 0.0f;
					fRowEndY = fNewRowEndY + (m_fMinRowHeight + m_fRowSpacing);
					fRowHeight = 0.0f;
				}
				break;
			}
			wszText += nPos;
			fPos = m_fWidth - fX;
			nPos = pItemText->GetSplitPos(wszText, fPos, nControlType);
		}

		++it;
	}

	//最后一行
	float fNewRowEndY = fRowEndY - m_fMinRowHeight + fRowHeight;
	AdjustRowItemEndAbsY(itRowBegin, it, fRowEndY, fNewRowEndY);

	AdjustItemShowInfo();

	return true;
}

int KItemHandle::FormatAllItemPosByCustom()
{
    int nResult = false;
    float fX        = m_fAbsX + m_fItemStartRelX;
    float fY        = m_fAbsY + m_fItemStartRelY;
    float fWidth    = 0;
    float fHeight   = 0;
    KItemArray::iterator it = m_aItem.begin();
    if (it != m_aItem.end())
    {
        ASSERT(*it);
        (*it)->GetSize(fWidth, fHeight);
        switch(m_dwFirstItemPosType) 
        {
        case POSITION_BY_CUSTOM:
            {
                float fRelX = 0;
                float fRelY = 0;
                (*it)->GetRelPos(fRelX, fRelY);
                fX += fRelX;
                fY += fRelY;
            }
            break;
        case POSITION_LEFT_BOTTOM:
        case POSITION_BOTTOM_LEFT:
            fY += (m_fHeight - fHeight);
            break;
        case POSITION_LEFT_CENTER:
            fY += ((m_fHeight - fHeight) / 2);
            break;
        case POSITION_TOP_RIGHT:
        case POSITION_RIGHT_TOP:
            fX += (m_fWidth - fWidth);
            break;
        case POSITION_TOP_CENTER:
            fX += ((m_fWidth - fWidth) / 2);
            break;
        case POSITION_RIGHT_BOTTOM:
        case POSITION_BOTTOM_RIGHT:
            fX += (m_fWidth - fWidth);
            fY += (m_fHeight - fHeight);
            break;
        case POSITION_RIGHT_CENTER:
            fX += (m_fWidth - fWidth);
            fY += ((m_fHeight - fHeight) / 2);
            break;
        case POSITION_BOTTOM_CENTER:
            fX += ((m_fWidth - fWidth) / 2);
            fY += (m_fHeight - fHeight);
            break;
        case POSITION_LEFT_TOP:
        case POSITION_TOP_LEFT:
        default:
            break;
        }
        (*it)->SetAbsPos(fX, fY);
        ++it;
    }
    for (NULL; it != m_aItem.end(); ++it) 
    {
        ASSERT(*it);
        DWORD dwPosType = POSITION_BEGINE;
        (*it)->GetPosType(dwPosType);
        switch(dwPosType) 
        {
        case POSITION_BY_CUSTOM:
            {
                float fRelX = 0;
                float fRelY = 0;
                (*it)->GetRelPos(fRelX, fRelY);
                fX = m_fAbsX + m_fItemStartRelX + fRelX;
                fY = m_fAbsY + m_fItemStartRelY + fRelY;
                (*it)->GetSize(fWidth, fHeight);
            }
        	break;
        case POSITION_LEFT_BOTTOM:
            fY += fHeight;
            (*it)->GetSize(fWidth, fHeight);
            fX -= fWidth;
			fY -= fHeight;
            break;
        case POSITION_LEFT_TOP:
            (*it)->GetSize(fWidth, fHeight);
            fX -= fWidth;
            break;
        case POSITION_LEFT_CENTER:
            fY += (fHeight / 2);
            (*it)->GetSize(fWidth, fHeight);
            fY -= (fHeight / 2);
            fX -= fWidth;
            break;
        case POSITION_TOP_LEFT:
            (*it)->GetSize(fWidth, fHeight);
            fY -= fHeight;
            break;
        case POSITION_TOP_RIGHT:
            fX += fWidth;
            (*it)->GetSize(fWidth, fHeight);
            fX -= fWidth;
            fY -= fHeight;
            break;
        case POSITION_TOP_CENTER:
            fX += (fWidth / 2);
            (*it)->GetSize(fWidth, fHeight);
            fX -= (fWidth / 2);
            fY -= fHeight;
            break;
        case POSITION_RIGHT_TOP:
            fX += fWidth;
            (*it)->GetSize(fWidth, fHeight);
            break;
        case POSITION_RIGHT_BOTTOM:
            fX += fWidth;
            fY += fHeight;
            (*it)->GetSize(fWidth, fHeight);
            fY -= fHeight;
            break;
        case POSITION_RIGHT_CENTER:
            fX += fWidth;
            fY += (fHeight / 2);
            (*it)->GetSize(fWidth, fHeight);
            fY -= (fHeight / 2);
            break;
        case POSITION_BOTTOM_LEFT:
            fY += fHeight;
            (*it)->GetSize(fWidth, fHeight);
            break;
        case POSITION_BOTTOM_RIGHT:
            fX += fWidth;
            fY += fHeight;
            (*it)->GetSize(fWidth, fHeight);
            fX -= fWidth;
            break;
        case POSITION_BOTTOM_CENTER:
            fX += (fWidth / 2);
            fY += fHeight;
            (*it)->GetSize(fWidth, fHeight);
            fX -= (fWidth / 2);
            break;
        default:
            KG_ASSERT_EXIT( 0 && "No Postion Type Seted!");
            break;
        }   

        (*it)->SetAbsPos(fX, fY);
    }

    fX = m_fAbsX + m_fItemStartRelX;
    fY = m_fAbsY + m_fItemStartRelY;

	AdjustItemShowInfo();

    nResult = true;
Exit0:
    return nResult;
}

int KItemHandle::Clear()
{
	int nCount = (int)m_aItem.size();
	while(nCount--)
	{
		SAFE_RELEASE(m_aItem[nCount]);
		m_aItem.pop_back();
	}

    m_aItem.clear();

	return true;
}

int KItemHandle::Remove(KItemNull *pWndItem)
{
    int nResult = false;
	int nFormate = false;
    KItemArray::iterator it = std::find(m_aItem.begin(), m_aItem.end(), pWndItem);
    
    KG_PROCESS_ERROR(it != m_aItem.end());
    ASSERT(*it);
    SAFE_RELEASE(*it);
	if (m_dwStyle & ITEM_HANDLE_FORMAT_POS_WHEN_APPEND && it != m_aItem.end())
		nFormate = true;
    m_aItem.erase(it);
	if (nFormate)	
		FormatAllItemPos();

    nResult = true;
Exit0:
    return nResult;
}

KItemNull* KItemHandle::FindItem(LPCSTR szItemName)
{
    if (szItemName == NULL || szItemName[0] == '\0')
        return NULL;

    KItemArray::iterator it = m_aItem.begin();
    KItemArray::iterator ie = m_aItem.end();
    for (; it != ie; ++it)
    {
        KItemNull* pItemNull = *it;

        ASSERT(pItemNull);

        if (strcmp(szItemName, pItemNull->GetName()) == 0)
        {
            return pItemNull;
        }
    }

    return NULL;
}

KItemNull* KItemHandle::GetItemByTreePath(LPCSTR szTreePath, KItemHandle** ppHandle, LPCSTR szDelimit)
{
    static char szSrcToken[ITEM_TREE_PATH_LEN];

	if (szTreePath == NULL || szTreePath[0] == '\0')
		return NULL;

	ASSERT(szDelimit);
	ASSERT(szDelimit[0] != '\0');

	strncpy(szSrcToken, szTreePath, _countof(szSrcToken) - 1);
	szSrcToken[_countof(szSrcToken) - 1] = '\0';

	LPSTR szToken = strtok(szSrcToken, szDelimit);

	KItemNull* pItem = NULL;
    KItemHandle* pItemHandle = this;

	while (true)
	{
		pItem = pItemHandle->FindItem(szToken);
        if (pItem == NULL)
            break;

        szToken = strtok(NULL, szDelimit);
        if (szToken == NULL)
            break;

        pItemHandle = g_ItemCast<KItemHandle>(pItem);
        if (pItemHandle == NULL)
            break;
	}

    if (ppHandle)
        *ppHandle = pItemHandle;

	return pItem;
}

KItemNull* KItemHandle::GetItemByIndex(int nIndex)
{
    if (nIndex >= 0 && nIndex < (int)m_aItem.size())
        return m_aItem[nIndex];
    return NULL;
}

KItemHandle* KItemHandle::GetItemHandle(KItemNull* pItemNull)
{
    if (pItemNull == NULL)
        return NULL;

    KItemArray::iterator it = m_aItem.begin();
    KItemArray::iterator ie = m_aItem.end();
    for (; it != ie; ++it) 
    {
        KItemNull* pIterItemNull = *it;

        if (pIterItemNull == pItemNull)
            return this;

        KItemHandle* pItemHandle = g_ItemCast<KItemHandle>(pIterItemNull);
        if (pItemHandle)
        {
            pItemHandle = pItemHandle->GetItemHandle(pItemNull);
            if (pItemHandle)
            {
                return pItemHandle;
            }
        }
    }

    return NULL;
}

int KItemHandle::GetItemIndex(KItemNull *pItemNull)
{
    int nCount = (int)m_aItem.size();
    for (int i = 0; i < nCount; ++i)
    {
        if (m_aItem[i] == pItemNull)
            return i;
    }

    return -1;
}

int KItemHandle::GetItemTreePath(KItemNull* pItemNull, LPSTR szOutTreePath, int nOutTreePathLen)
{
	ASSERT(pItemNull);
	ASSERT(szOutTreePath);
	ASSERT(nOutTreePathLen > 0);

	if (pItemNull == this)
	{
		szOutTreePath[0] = '\0';
		return TRUE;
	}

    KItemArray::iterator it = m_aItem.begin();
    KItemArray::iterator ie = m_aItem.end();
	for (; it != ie; ++it) 
	{
        KItemNull* pIterItemNull = *it;
		if (pIterItemNull == pItemNull)
		{
			strncpy(szOutTreePath, pItemNull->GetName(), nOutTreePathLen);
			return TRUE;
		}

    	KItemHandle* pItemHandle = g_ItemCast<KItemHandle>(pIterItemNull);
		if (pItemHandle)
		{
            char szSubPath[ITEM_TREE_PATH_LEN];

			szSubPath[0] = '\0';

			if (pItemHandle->GetItemTreePath(pItemNull, szSubPath, _countof(szSubPath)))
			{
				_snprintf(szOutTreePath, nOutTreePathLen, "%s/%s", pItemHandle->GetName(), szSubPath);
				szOutTreePath[nOutTreePathLen - 1] = '\0';
				return TRUE;
			}
		}
	}

	return FALSE;
}

int KItemHandle::Remove(LPCSTR szItemTreePath)
{
    static char szSrcToken[ITEM_TREE_PATH_LEN];

    int nResult = false;
    LPCSTR szDelimit = "/\\";
    LPSTR szToken = NULL;
	KItemNull* pItemNull = NULL;
    KItemHandle* pItemHandle = this;

    KG_PROCESS_ERROR(szItemTreePath);
    KG_PROCESS_ERROR(szItemTreePath[0] != '\0');

    szSrcToken[0] = '\0';

	strncpy(szSrcToken, szItemTreePath, _countof(szSrcToken));
	szSrcToken[_countof(szSrcToken) - 1] = '\0';

	szToken = strtok(szSrcToken, szDelimit);
	while (true)
	{
		pItemNull = pItemHandle->FindItem(szToken);
        if (pItemNull == NULL)
            break;

		szToken = strtok(NULL, szDelimit);
        if (szToken == NULL)
            break;
        
        pItemHandle = g_ItemCast<KItemHandle>(pItemNull);
        if (pItemHandle == NULL)
            break;
	}

    KG_PROCESS_ERROR(pItemHandle);
    KG_PROCESS_ERROR(pItemNull);

    pItemHandle->Remove(pItemNull);

    nResult = true;
Exit0:
    return nResult;
}

int KItemHandle::SetItemNewIndex(int nOriginalIndex, int nNewIndex)
{
	int nResult = false;
	int nCount = (int)m_aItem.size();
	KItemNull *pItem = NULL;

	KGLOG_PROCESS_ERROR(nOriginalIndex >= 0 && nOriginalIndex < nCount);

	if (nNewIndex >= nCount)
		nNewIndex = nCount - 1;

	if (nNewIndex < 0)
		nNewIndex = 0;

	KG_PROCESS_SUCCESS(nNewIndex == nOriginalIndex);

	pItem = m_aItem[nOriginalIndex];
	if (nNewIndex > nOriginalIndex)
	{
		for (int i = nOriginalIndex; i < nNewIndex; ++i)
			m_aItem[i] = m_aItem[i + 1];
	}
	else
	{
		for (int i = nOriginalIndex; i > nNewIndex; --i)
			m_aItem[i] = m_aItem[i - 1];
	}
	m_aItem[nNewIndex] = pItem;

Exit1:
	nResult = true;
Exit0:
	return nResult;
}

int KItemHandle::GetVisibleItemCount() const
{
	int nCount = 0;
	for (KItemArray::const_iterator it = m_aItem.begin(); it != m_aItem.end(); ++it)
	{
		ASSERT(*it);
		if ((*it)->IsVisible())
			++nCount;
	}

	return nCount;
}

int	KItemHandle::PushAllEventItem(KItemArray &arItemArray)
{
	if (GetEvent())
		arItemArray.push_back(this);
	for (KItemArray::const_iterator it = m_aItem.begin(); it != m_aItem.end(); ++it)
	{
		KItemNull *pItem = (*it);
		KItemHandle *pHandle = g_ItemCast<KItemHandle>(pItem);
		if (pHandle)
		{
			pHandle->PushAllEventItem(arItemArray);
			continue;
		}
		if (pItem->GetEvent())
			arItemArray.push_back(pItem);
	}

	return true;
}

int	KItemHandle::PushAllDrawItem(KItemArray &arItemArray)
{
	if (IsVisible())
	{
		int nPush = false;
		if (m_dwStyle & ITEM_HANDLE_USE_STENCIL)
			nPush = true;
#ifndef DISABLE_UI_EDIT
		nPush = true;
#else
		if (g_ItemCast<KItemTreeLeaf>(this))
			nPush = true;
#endif //_DEBUG
		if (nPush)
			arItemArray.push_back(this);
		for (KItemArray::const_iterator it = m_aItem.begin(); it != m_aItem.end(); ++it)
		{
			KItemNull *pItem = (*it);
			if (pItem->IsVisible())
			{
				KItemHandle *pHandle = g_ItemCast<KItemHandle>(pItem);
				if (pHandle)
				{
					pHandle->PushAllDrawItem(arItemArray);
					continue;
				}
				arItemArray.push_back(pItem);
			}
		}
		if (m_dwStyle & ITEM_HANDLE_USE_STENCIL)
			arItemArray.push_back(this);
	}
	return true;
}

int KItemHandle::FormatLastItemPos()
{
	int nResult = false;
	KItemNull *pItemLast = NULL;
	KItemNull *pItem = NULL;

	int nCount = (int)m_aItem.size();
	if (!nCount)
		goto Exit1;
	if (nCount == 1)
	{
		FormatAllItemPos();
		goto Exit1;
	}

	switch(m_dwHandleType) 
	{
	case ITEM_HANDLE_CUSTOM:
		FormatLastItemPosByCustom(nCount);
		break;
	case ITEM_HANDLE_AUTO_NEW_LINE_ROW_HEIGHT_CHANGELESS:
		FormatLastItemPosByAutoNewLineRowHeightChangeless(nCount);
		break;
	case ITEM_HANDLE_AUTO_NEW_LINE_ROW_HEIGHT_BETWEEN_MIN_MAX:
		FormatLastItemPosByAutoNewLineRowHeightBetweenMinMax(nCount);
		break;
	case ITEM_HANDLE_AUTO_NEW_LINE_ROW_HEIGHT_BY_MAX_ITEM:
		FormatLastItemPosByAutoNewLineRowHeightByMaxItem(nCount);
		break;
	case ITEM_HANDLE_CHAT_ITEM_HANDLE:
		FormatLastItemPosByChatItemHandle(nCount);
		break;
	case ITEM_HANDLE_TREE_ITEM_HANDLE:
		FormatLastItemPosByTreeItemHandle(nCount);
		break;
	case ITEM_HANDLE_LEFT_RIGHT_ALIGN_BY_TABLE:
		FormatLastItemPosByLeftRightAlignByTable(nCount);
		break;
	default:
		KG_ASSERT_EXIT(0 && "No Style Seted!");
		break;
	}

	if (m_dwStyle & ITEM_HANDLE_AUOT_ADJUST_SIZE_BY_CHILD)
	{
		SetSizeByAllItemSize();
	}

Exit1:
	nResult = true;
Exit0:
	return nResult;
}

int KItemHandle::FormatLastItemPosByCustom(int nCount)
{
	int nResult = false;

	KItemNull *pItem = NULL;
	KItemNull *pItemLast = NULL;
	float fX = 0;
	float fY = 0;
	float fWidth = 0;
	float fHeight = 0;

	ASSERT(nCount > 1 && nCount == (int)m_aItem.size());
	pItem = m_aItem[nCount - 1];
	ASSERT(pItem);
	pItemLast = m_aItem[nCount - 2];
	ASSERT(pItemLast);

	DWORD dwPosType = POSITION_BEGINE;
	pItem->GetPosType(dwPosType);
	switch(dwPosType) 
	{
	case POSITION_BY_CUSTOM:
		{
			pItem->GetRelPos(fX, fY);
			pItem->GetSize(fWidth, fHeight);
			fX += (m_fAbsX + m_fItemStartRelX);
			fY += (m_fAbsY + m_fItemStartRelY);
		}
		break;
	case POSITION_LEFT_BOTTOM:
		pItemLast->GetAbsPos(fX, fY);
		pItemLast->GetSize(fWidth, fHeight);
		fY += fHeight;
		pItem->GetSize(fWidth, fHeight);
		fX -= fWidth;
		fY -= fHeight;
		break;
	case POSITION_LEFT_TOP:
		pItemLast->GetAbsPos(fX, fY);
		pItem->GetSize(fWidth, fHeight);
		fX -= fWidth;
		break;
	case POSITION_LEFT_CENTER:
		pItemLast->GetAbsPos(fX, fY);
		pItemLast->GetSize(fWidth, fHeight);
		fY += (fHeight / 2);
		pItem->GetSize(fWidth, fHeight);
		fX -= fWidth;
		fY -= (fHeight / 2);
		break;
	case POSITION_TOP_LEFT:
		pItemLast->GetAbsPos(fX, fY);
		pItem->GetSize(fWidth, fHeight);
		fY -= fHeight;
		break;
	case POSITION_TOP_RIGHT:
		pItemLast->GetAbsPos(fX, fY);
		pItemLast->GetSize(fWidth, fHeight);
		fX += fWidth;
		pItem->GetSize(fWidth, fHeight);
		fX -= fWidth;
		fY -= fHeight;
		break;
	case POSITION_TOP_CENTER:
		pItemLast->GetAbsPos(fX, fY);
		pItemLast->GetSize(fWidth, fHeight);
		fX += (fWidth / 2);
		pItem->GetSize(fWidth, fHeight);
		fX -= (fWidth / 2);
		fY -= fHeight;
		break;
	case POSITION_RIGHT_TOP:
		pItemLast->GetAbsPos(fX, fY);
		pItemLast->GetSize(fWidth, fHeight);
		fX += fWidth;
		pItem->GetSize(fWidth, fHeight);
		break;
	case POSITION_RIGHT_BOTTOM:
		pItemLast->GetAbsPos(fX, fY);
		pItemLast->GetSize(fWidth, fHeight);
		fX += fWidth;
		fY += fHeight;
		pItem->GetSize(fWidth, fHeight);
		fY -= fHeight;
		break;
	case POSITION_RIGHT_CENTER:
		pItemLast->GetAbsPos(fX, fY);
		pItemLast->GetSize(fWidth, fHeight);
		fX += fWidth;
		fY += (fHeight / 2);
		pItem->GetSize(fWidth, fHeight);
		fY -= (fHeight / 2);
		break;
	case POSITION_BOTTOM_LEFT:
		pItemLast->GetAbsPos(fX, fY);
		pItemLast->GetSize(fWidth, fHeight);
		fY += fHeight;
		pItem->GetSize(fWidth, fHeight);
		break;
	case POSITION_BOTTOM_RIGHT:
		pItemLast->GetAbsPos(fX, fY);
		pItemLast->GetSize(fWidth, fHeight);
		fX += fWidth;
		fY += fHeight;
		pItem->GetSize(fWidth, fHeight);
		fX -= (fWidth / 2);
		break;
	case POSITION_BOTTOM_CENTER:
		pItemLast->GetAbsPos(fX, fY);
		pItemLast->GetSize(fWidth, fHeight);
		fX += (fWidth / 2);
		fY += fHeight;
		pItem->GetSize(fWidth, fHeight);
		fX -= (fWidth / 2);
		break;
	default:
		KG_ASSERT_EXIT( 0 && "No Postion Type Seted!");
		break;
	}   

	pItem->SetAbsPos(fX, fY);					
	CorrectAllItemRect(fX - m_fAbsX, fY - m_fAbsY, fWidth, fHeight);
	K_ITEM_SHOW_TEST(pItem, fX, fY, fWidth, fHeight)

	nResult = true;
Exit0:
	return nResult;
}

int KItemHandle::FormatLastItemPosByAutoNewLineRowHeightChangeless(int nCount)
{
	KItemNull *pItem = NULL;
	KItemNull *pItemLast = NULL;
	KItemText *pItemText = NULL;

	float fWidth        = 0.0f;
	float fHeight       = 0.0f;
	float fStartX       = m_fAbsX + m_fItemStartRelX;
	float fStartY       = m_fAbsY + m_fItemStartRelY;
	float fX			= fStartX;
	float fY			= fStartY + m_fMinRowHeight;

	ASSERT(nCount > 1 && nCount == (int)m_aItem.size());
	pItem = m_aItem[nCount - 1];
	ASSERT(pItem);
	pItemLast = m_aItem[nCount - 2];
	ASSERT(pItemLast);

	for (int i = nCount - 2; i >= 0; --i)
	{
		LPCWSTR wszControlKey = NULL;
		pItemLast = m_aItem[i];
		ASSERT(pItemLast);
		pItemText = g_ItemCast<KItemText>(pItemLast);
		if (pItemText && pItemText->IsRichText())
		{
			//普通图文文本
			if (!pItemText->GetLastDrawBlockInfo(fX, fY, &wszControlKey))
				continue;
		}
		else
		{
			//前一个item不是文字,或者该段文字不能由handle设置绘制数据。
			pItemLast->GetAbsPos(fX, fY);
			pItemLast->GetSize(fWidth, fHeight);
			fX += fWidth;
			fY += fHeight;
		}

		for (int j = i; j < nCount - 1; ++j)
		{
			if (!wszControlKey)
			{
				pItemText = g_ItemCast<KItemText>(m_aItem[j]);
				if(!pItemText)
					continue;
				wszControlKey = pItemText->GetText();
				if (!wszControlKey)
					continue;
			}
			while (*wszControlKey != L'\0')
			{
				if (*wszControlKey == L'\t')
				{
					fX += pItemText->GetTableTextWidth();
					if(UI_FLOAT_LARGE(fX, fStartX + m_fWidth))
					{
						fX = fStartX;
						fY += (m_fMinRowHeight + m_fRowSpacing);
					}						
				}
				if (*wszControlKey == L'\n')
				{
					fX = fStartX;
					fY += (m_fMinRowHeight + m_fRowSpacing);
				}
				++wszControlKey;
			}
			wszControlKey = NULL;
		}
		break;
	}

	pItemText = g_ItemCast<KItemText>(pItem);
	if (pItemText && pItemText->IsRichText())
	{
		//普通图文文本
		//是文字,并且该文字是handle设置绘制数据。
		fX = fX - fStartX;
		float fRowEndY = fY;

		LPCWSTR wszText = pItemText->GetText();
		float fPos = m_fWidth - fX;
		int nControlType = 0;
		int nContinueLoop = true;
		float fTableWidth = pItemText->GetTableTextWidth();
		pItemText->ClearDrawTextData();
		pItemText->SetAbsPos(fStartX + fX, fRowEndY);
		fHeight = pItemText->GetSingleTextHeight();
		int nPos = pItemText->GetSplitPos(wszText, fPos, nControlType);

		while (nPos != -1 && nContinueLoop)
		{
			switch(nControlType)
			{
			case ITEM_TEXT_CONTROL_KEY_TAB:
				pItemText->AppendDrawTextData(
					D3DXVECTOR2(fStartX + fX, fRowEndY - fHeight), 
					D3DXVECTOR2(fStartX + fX + fPos, fRowEndY),
					wszText, nPos
					);
				fX += fPos;
				++nPos;
				fX += fTableWidth;
				if (UI_FLOAT_LARGE(fX, m_fWidth))
				{
					fX = 0.0f;
					fRowEndY += (m_fMinRowHeight + m_fRowSpacing);
				}
				break;
			case ITEM_TEXT_CONTROL_KEY_NEW_LINE:
				pItemText->AppendDrawTextData(
					D3DXVECTOR2(fStartX + fX, fRowEndY - fHeight), 
					D3DXVECTOR2(fStartX + fX + fPos, fRowEndY),
					wszText, nPos
					);
				++nPos;
				fX = 0.0f;
				fRowEndY += (m_fMinRowHeight + m_fRowSpacing);
				break;
			default:
				if (!nPos)
				{
					//用于字符串出现的异常情况(里面存在非法字符的情况)。
					if (UI_FLOAT_LARGE(fX, 0.0f))
					{
						fX = 0.0f;
						fRowEndY += (m_fMinRowHeight + m_fRowSpacing);
						break;
					}
					//m_fWidth 太小以至于无法显示一个文字。所以推出循环。
					nContinueLoop = false;
					break;
				}
				pItemText->AppendDrawTextData(
					D3DXVECTOR2(fStartX + fX, fRowEndY - fHeight), 
					D3DXVECTOR2(fStartX + fX + fPos, fRowEndY),
					wszText, nPos
					);
				fX += fPos;
				if (UI_FLOAT_LARGE_OR_EQUAL(fX, m_fWidth))
				{
					fX = 0.0f;
					fRowEndY += (m_fMinRowHeight + m_fRowSpacing);
				}
				break;
			}
			wszText += nPos;
			fPos = m_fWidth - fX;
			nPos = pItemText->GetSplitPos(wszText, fPos, nControlType);
		}

		pItemText->GetAbsPos(fX, fY);
		pItemText->GetSize(fWidth, fHeight);

		K_RICH_ITEM_SHOW_TEST_V(pItemText, fX, fY, fWidth, fHeight);
	}
	else
	{
		//不是文字,或者该段文字不能由handle设置绘制数据。
		pItem->GetSize(fWidth, fHeight);
/*
		if (UI_FLOAT_LARGE(fHeight, m_fMinRowHeight))
		{
			//需要等比缩放，还是覆盖上面的文字在这里操作。
			fHeight = m_fMinRowHeight;
			pItem->SetSize(fWidth, fHeight);
		}
		if (UI_FLOAT_LARGE(fWidth, m_fWidth))
		{
			//如果宽度超过了最大宽度。如果等比索放在这里操作
			fWidth = m_fWidth;
			pItem->SetSize(fWidth, fHeight);
		}
*/
		if (UI_FLOAT_LARGE(fX + fWidth, fStartX + m_fWidth) && !UI_FLOAT_EQUAL(fX, fStartX))
		{
			//这一行已经放不下了，从下一行直接开始。
			fX = fStartX;
			fY += (m_fMinRowHeight + m_fRowSpacing);
		}

		pItem->SetAbsPos(fX, fY - fHeight);

	}

	pItem->GetAbsPos(fX, fY);
	pItem->GetSize(fWidth, fHeight);
	CorrectAllItemRect(fX - m_fAbsX, fY - m_fAbsY, fWidth, fHeight);
	if (!pItemText)
	{
		K_ITEM_SHOW_TEST(pItem, fX, fY, fWidth, fHeight);
	}

	return true;
}

//对以前一行的item补偿的方法不正确.
int KItemHandle::FormatLastItemPosByAutoNewLineRowHeightBetweenMinMax(int nCount)
{
	KItemNull *pItem = NULL;
	KItemNull *pItemLast = NULL;
	KItemText *pItemText = NULL;

	float fWidth        = 0.0f;
	float fHeight       = 0.0f;
	float fStartX       = m_fAbsX + m_fItemStartRelX;
	float fStartY       = m_fAbsY + m_fItemStartRelY;
	float fX			= fStartX;
	float fY			= fStartY + m_fMinRowHeight;
	float fRowHeight    =  m_fMinRowHeight;

	ASSERT(nCount > 1 && nCount == (int)m_aItem.size());
	pItem = m_aItem[nCount - 1];
	ASSERT(pItem);
	pItemLast = m_aItem[nCount - 2];
	ASSERT(pItemLast);

	KItemArray::iterator itRowBegin = m_aItem.begin();
	for (int i = nCount - 2; i >= 0; --i)
	{
		LPCWSTR wszControlKey = NULL;
		pItemLast = m_aItem[i];
		ASSERT(pItemLast);
		pItemText = g_ItemCast<KItemText>(pItemLast);
		if (pItemText && pItemText->IsRichText())
		{
			//普通图文文本
			if (!pItemText->GetLastDrawBlockInfo(fX, fY, &wszControlKey))
				continue;
		}
		else
		{
			//前一个item不是文字,或者该段文字不能由handle设置绘制数据。
			pItemLast->GetAbsPos(fX, fY);
			pItemLast->GetSize(fWidth, fHeight);
			fX += fWidth;
			fY += fHeight;
		}

		int nFindBack = true;
		for (int j = i; j < nCount - 1; ++j)
		{
			if (!wszControlKey)
			{
				pItemText = g_ItemCast<KItemText>(m_aItem[j]);
				if(!pItemText)
					continue;
				wszControlKey = pItemText->GetText();
				if (!wszControlKey)
					continue;
			}

			fHeight = pItemText->GetSingleTextHeight();
			if (UI_FLOAT_LARGE(fHeight, fRowHeight))
				fRowHeight = fHeight;
			if (UI_FLOAT_LARGE(fHeight, m_fMaxRowHeight))
				fRowHeight = m_fMaxRowHeight;
			while (*wszControlKey != L'\0')
			{
				if (*wszControlKey == L'\t')
				{
					fX += pItemText->GetTableTextWidth();
					if(UI_FLOAT_LARGE(fX, fStartX + m_fWidth))
					{
						fX = fStartX;
						fY += (m_fMinRowHeight + m_fRowSpacing);
						fRowHeight    =  m_fMinRowHeight;
						nFindBack = false;
					}						
				}
				if (*wszControlKey == L'\n')
				{
					fX = fStartX;
					fY += (m_fMinRowHeight + m_fRowSpacing);
					fRowHeight    =  m_fMinRowHeight;
					nFindBack = false;
				}
				++wszControlKey;
			}
			wszControlKey = NULL;
		}
		if (!nFindBack)
		{
			itRowBegin = m_aItem.end() - 1; 
			break;
		}
		//回溯
		for (int j = i; j >= 0; --j)
		{
			float fX1 = 0.0f;
			float fY1 = 0.0f;
			LPCWSTR wszControlKey = NULL;
			pItemLast = m_aItem[j];
			ASSERT(pItemLast);
			pItemText = g_ItemCast<KItemText>(pItemLast);
			if (pItemText && pItemText->IsRichText())
			{
				if (pItemText->GetLastDrawBlockInfo(fX1, fY1, &wszControlKey))
				{
					if (!(UI_FLOAT_EQUAL(fY1, fY)))
					{
						itRowBegin = m_aItem.begin() + j + 1;
						break;
					}
					fHeight = pItemText->GetSingleTextHeight();
					if (UI_FLOAT_LARGE(fHeight, fRowHeight))
						fRowHeight = fHeight;
					if (UI_FLOAT_LARGE(fHeight, m_fMaxRowHeight))
						fRowHeight = m_fMaxRowHeight;
				}
			}
			else
			{
				pItemLast->GetAbsPos(fX1, fY1);
				pItemLast->GetSize(fWidth, fHeight);
				if (!(UI_FLOAT_EQUAL(fY1 + fHeight, fY)))
				{
					itRowBegin = m_aItem.begin() + j + 1;
					break;
				}
				if (UI_FLOAT_LARGE(fHeight, fRowHeight))
					fRowHeight = fHeight;
				if (UI_FLOAT_LARGE(fHeight, m_fMaxRowHeight))
					fRowHeight = m_fMaxRowHeight;
			}
		}
		break;
	}

	if (!(UI_FLOAT_EQUAL(fRowHeight, m_fMinRowHeight)))
	{
		float fNewRowEndY = fY - fRowHeight+ m_fMinRowHeight;
		AdjustRowItemEndAbsY(itRowBegin, m_aItem.end() - 1, fY, fNewRowEndY);	
		fY = fNewRowEndY;
	}

	pItemText = g_ItemCast<KItemText>(pItem);
	if (pItemText && pItemText->IsRichText())
	{
		//一般图文
		//是文字,并且该文字是handle设置绘制数据。
		fX = fX - fStartX;
		float fRowEndY = fY;

		LPCWSTR wszText = pItemText->GetText();
		float fPos = m_fWidth - fX;
		int nControlType = 0;
		int nContinueLoop = true;
		pItemText->ClearDrawTextData();
		pItemText->SetAbsPos(fStartX + fX, fRowEndY);
		float fTableWidth = pItemText->GetTableTextWidth();
		fHeight = pItemText->GetSingleTextHeight();

		int nPos = pItemText->GetSplitPos(wszText, fPos, nControlType);
		while (nPos != -1 && nContinueLoop)
		{
			//文字不管最大高，不然会出错。
			if (UI_FLOAT_LARGE(fHeight, fRowHeight))
				fRowHeight = fHeight;
			if (UI_FLOAT_LARGE(fHeight, m_fMaxRowHeight))
				fRowHeight = m_fMaxRowHeight;
			switch(nControlType)
			{
			case ITEM_TEXT_CONTROL_KEY_TAB:
				pItemText->AppendDrawTextData(
					D3DXVECTOR2(fStartX + fX, fRowEndY - fHeight), 
					D3DXVECTOR2(fStartX + fX + fPos, fRowEndY),
					wszText, nPos
					);
				fX += fPos;
				++nPos;
				fX += fTableWidth;
				if (UI_FLOAT_LARGE(fX,m_fWidth))
				{
					float fNewRowEndY = fRowEndY - m_fMinRowHeight + fRowHeight;
					AdjustRowItemEndAbsY(itRowBegin, m_aItem.end(), fRowEndY, fNewRowEndY);
					itRowBegin = m_aItem.end() - 1;

					fX = 0.0f;
					fRowEndY = fNewRowEndY + (m_fMinRowHeight + m_fRowSpacing);
					fRowHeight = m_fMinRowHeight;
				}
				break;
			case ITEM_TEXT_CONTROL_KEY_NEW_LINE:
				pItemText->AppendDrawTextData(
					D3DXVECTOR2(fStartX + fX, fRowEndY - fHeight), 
					D3DXVECTOR2(fStartX + fX + fPos, fRowEndY),
					wszText, nPos
					);
				++nPos;
				{
					float fNewRowEndY = fRowEndY - m_fMinRowHeight + fRowHeight;
					AdjustRowItemEndAbsY(itRowBegin, m_aItem.end(), fRowEndY, fNewRowEndY);
					itRowBegin = m_aItem.end() - 1;;

					fX = 0.0f;
					fRowEndY = fNewRowEndY + (m_fMinRowHeight + m_fRowSpacing);
					fRowHeight = m_fMinRowHeight;
				}
				break;
			default:
				if (!nPos)
				{
					//用于字符串出现的异常情况(里面存在非法字符的情况)。
					if (UI_FLOAT_LARGE(fX, 0.0f))
					{
						float fNewRowEndY = fRowEndY - m_fMinRowHeight + fRowHeight;
						AdjustRowItemEndAbsY(itRowBegin, m_aItem.end(), fRowEndY, fNewRowEndY);
						itRowBegin = m_aItem.end() - 1;

						fX = 0.0f;
						fRowEndY = fNewRowEndY + (m_fMinRowHeight + m_fRowSpacing);
						fRowHeight = m_fMinRowHeight;

						break;
					}
					//m_fWidth 太小以至于无法显示一个文字。所以退出循环。
					nContinueLoop = false;
					break;
				}
				pItemText->AppendDrawTextData(
					D3DXVECTOR2(fStartX + fX, fRowEndY - fHeight), 
					D3DXVECTOR2(fStartX + fX + fPos, fRowEndY),
					wszText, nPos
					);
				fX += fPos;
				if (UI_FLOAT_LARGE_OR_EQUAL(fX, m_fWidth))
				{
					float fNewRowEndY = fRowEndY - m_fMinRowHeight + fRowHeight;
					AdjustRowItemEndAbsY(itRowBegin, m_aItem.end(), fRowEndY, fNewRowEndY);
					itRowBegin = m_aItem.end() - 1;

					fX = 0.0f;
					fRowEndY = fNewRowEndY + (m_fMinRowHeight + m_fRowSpacing);
					fRowHeight = m_fMinRowHeight;
				}
				break;
			}
			wszText += nPos;
			fPos = m_fWidth - fX;
			nPos = pItemText->GetSplitPos(wszText, fPos, nControlType);
		}

		float fNewRowEndY = fRowEndY - m_fMinRowHeight + fRowHeight;
		AdjustRowItemEndAbsY(itRowBegin, m_aItem.end(), fRowEndY, fNewRowEndY);

		pItemText->GetAbsPos(fX, fY);
		pItemText->GetSize(fWidth, fHeight);

		K_RICH_ITEM_SHOW_TEST_V(pItemText, fX, fY, fWidth, fHeight);
	}
	else
	{
		//不是文字,或者该段文字不能由handle设置绘制数据。
		pItem->GetSize(fWidth, fHeight);
/*
		if (UI_FLOAT_LARGE(fHeight, m_fMaxRowHeight))
		{
			//需要等比缩放，还是覆盖上面的文字在这里操作。
			fHeight = m_fMaxRowHeight;
			pItem->SetSize(fWidth, fHeight);
		}
		if (UI_FLOAT_LARGE(fWidth, m_fWidth))
		{
			//如果宽度超过了最大高度。如果等比索放在这里操作
			fWidth = m_fWidth;
			pItem->SetSize(fWidth, fHeight);
		}
*/
		if (UI_FLOAT_LARGE(fX + fWidth, fStartX + m_fWidth) && !UI_FLOAT_EQUAL(fX, fStartX))
		{
			//这一行已经放不下了，从下一行直接开始。
			fX = fStartX;
			fY += (m_fMinRowHeight + m_fRowSpacing);
			fRowHeight = m_fMinRowHeight;
			itRowBegin = m_aItem.end() - 1;
		}

		if (UI_FLOAT_LARGE(fHeight, fRowHeight))
			fRowHeight = fHeight;

		pItem->SetAbsPos(fX, fY - fHeight);

		float fNewRowEndY = fY - m_fMinRowHeight + fRowHeight;
		AdjustRowItemEndAbsY(itRowBegin, m_aItem.end(), fY, fNewRowEndY);
	}

	pItem->GetAbsPos(fX, fY);
	pItem->GetSize(fWidth, fHeight);
	CorrectAllItemRect(fX - m_fAbsX, fY - m_fAbsY, fWidth, fHeight);
	if (!pItemText)
	{
		K_ITEM_SHOW_TEST(pItem, fX, fY, fWidth, fHeight);
	}
	return true;
}
int KItemHandle::FormatLastItemPosByAutoNewLineRowHeightByMaxItem(int nCount)
{
	KItemNull *pItem = NULL;
	KItemNull *pItemLast = NULL;
	KItemText *pItemText = NULL;

	m_fMinRowHeight		= 30.0f;
	float fWidth        = 0.0f;
	float fHeight       = 0.0f;
	float fStartX       = m_fAbsX + m_fItemStartRelX;
	float fStartY       = m_fAbsY + m_fItemStartRelY;
	float fX			= fStartX;
	float fY			= fStartY + m_fMinRowHeight;
	float fRowHeight    =  0.0f;

	ASSERT(nCount > 1 && nCount == (int)m_aItem.size());
	pItem = m_aItem[nCount - 1];
	ASSERT(pItem);
	pItemLast = m_aItem[nCount - 2];
	ASSERT(pItemLast);

	KItemArray::iterator itRowBegin = m_aItem.begin();
	for (int i = nCount - 2; i >= 0; --i)
	{
		LPCWSTR wszControlKey = NULL;
		pItemLast = m_aItem[i];
		ASSERT(pItemLast);
		pItemText = g_ItemCast<KItemText>(pItemLast);
		if (pItemText && pItemText->IsRichText())
		{
			//普通图文文本
			if (!pItemText->GetLastDrawBlockInfo(fX, fY, &wszControlKey))
				continue;
		}
		else
		{
			//前一个item不是文字,或者该段文字不能由handle设置绘制数据。
			pItemLast->GetAbsPos(fX, fY);
			pItemLast->GetSize(fWidth, fHeight);
			fX += fWidth;
			fY += fHeight;
			if (UI_FLOAT_LARGE(fHeight, fRowHeight))
				fRowHeight = fHeight;
		}

		int nFindBack = true;
		for (int j = i; j < nCount - 1; ++j)
		{
			if (!wszControlKey)
			{
				pItemText = g_ItemCast<KItemText>(m_aItem[j]);
				if(!pItemText)
					continue;
				wszControlKey = pItemText->GetText();
				if (!wszControlKey)
					continue;
			}

			fHeight = pItemText->GetSingleTextHeight();
			if (UI_FLOAT_LARGE(fHeight, fRowHeight))
				fRowHeight = fHeight;
			while (*wszControlKey != L'\0')
			{
				if (*wszControlKey == L'\t')
				{
					fX += pItemText->GetTableTextWidth();
					if(UI_FLOAT_LARGE(fX, fStartX + m_fWidth))
					{
						fRowHeight = fHeight;
						fX = fStartX;
						fY += (fRowHeight + m_fRowSpacing);
						//fRowHeight = 0.0f;
						nFindBack = false;
					}						
				}
				if (*wszControlKey == L'\n')
				{
					fRowHeight = fHeight;
					fX = fStartX;
					fY += (fRowHeight + m_fRowSpacing);
					//fRowHeight = 0.0f;
					nFindBack = false;
				}
				++wszControlKey;
			}
			wszControlKey = NULL;
		}
		if (!nFindBack)
		{
			itRowBegin = m_aItem.end() - 1; 
			break;
		}
		//回溯
		for (int j = i; j >= 0; --j)
		{
			float fX1 = 0.0f;
			float fY1 = 0.0f;
			LPCWSTR wszControlKey = NULL;
			pItemLast = m_aItem[j];
			ASSERT(pItemLast);
			pItemText = g_ItemCast<KItemText>(pItemLast);
			if (pItemText && pItemText->IsRichText())
			{
				if (pItemText->GetLastDrawBlockInfo(fX1, fY1, &wszControlKey))
				{
					if (!(UI_FLOAT_EQUAL(fY1, fY)))
					{
						itRowBegin = m_aItem.begin() + j + 1;
						break;
					}
					fHeight = pItemText->GetSingleTextHeight();
					if (UI_FLOAT_LARGE(fHeight, fRowHeight))
						fRowHeight = fHeight;
				}
			}
			else
			{
				pItemLast->GetAbsPos(fX1, fY1);
				pItemLast->GetSize(fWidth, fHeight);
				if (!(UI_FLOAT_EQUAL(fY1 + fHeight, fY)))
				{
					itRowBegin = m_aItem.begin() + j + 1;
					break;
				}
				if (UI_FLOAT_LARGE(fHeight, fRowHeight))
					fRowHeight = fHeight;
			}
		}
		break;
	}

	if (!(UI_FLOAT_EQUAL(fRowHeight, m_fMinRowHeight)))
	{
		float fNewRowEndY = fY - fRowHeight + m_fMinRowHeight;
		AdjustRowItemEndAbsY(itRowBegin, m_aItem.end() - 1, fY, fNewRowEndY);
		fY = fNewRowEndY;
	}

	pItemText = g_ItemCast<KItemText>(pItem);
	if (pItemText && pItemText->IsRichText())
	{
		//一般图文
		//是文字,并且该文字是handle设置绘制数据。
		fX = fX - fStartX;
		float fRowEndY = fY;

		LPCWSTR wszText = pItemText->GetText();
		float fPos = m_fWidth - fX;
		int nControlType = 0;
		int nContinueLoop = true;
		pItemText->ClearDrawTextData();
		pItemText->SetAbsPos(fStartX + fX, fRowEndY);
		float fTableWidth = pItemText->GetTableTextWidth();
		fHeight = pItemText->GetSingleTextHeight();

		int nPos = pItemText->GetSplitPos(wszText, fPos, nControlType);
		while (nPos != -1 && nContinueLoop)
		{
			//文字不管最大高，不然会出错。
			if (UI_FLOAT_LARGE(fHeight, fRowHeight))
				fRowHeight = fHeight;
			switch(nControlType)
			{
			case ITEM_TEXT_CONTROL_KEY_TAB:
				pItemText->AppendDrawTextData(
					D3DXVECTOR2(fStartX + fX, fRowEndY - fHeight), 
					D3DXVECTOR2(fStartX + fX + fPos, fRowEndY),
					wszText, nPos
					);
				fX += fPos;
				++nPos;
				fX += fTableWidth;
				if (UI_FLOAT_LARGE(fX,m_fWidth))
				{
					float fNewRowEndY = fRowEndY - m_fMinRowHeight + fRowHeight;
					AdjustRowItemEndAbsY(itRowBegin, m_aItem.end(), fRowEndY, fNewRowEndY);
					itRowBegin = m_aItem.end() - 1;

					fX = 0.0f;
					fRowEndY = fNewRowEndY + (m_fMinRowHeight + m_fRowSpacing);
					fRowHeight = 0.0f;
				}
				break;
			case ITEM_TEXT_CONTROL_KEY_NEW_LINE:
				pItemText->AppendDrawTextData(
					D3DXVECTOR2(fStartX + fX, fRowEndY - fHeight), 
					D3DXVECTOR2(fStartX + fX + fPos, fRowEndY),
					wszText, nPos
					);
				++nPos;
				{
					float fNewRowEndY = fRowEndY - m_fMinRowHeight + fRowHeight;
					AdjustRowItemEndAbsY(itRowBegin, m_aItem.end(), fRowEndY, fNewRowEndY);
					itRowBegin = m_aItem.end() - 1;;

					fX = 0.0f;
					fRowEndY = fNewRowEndY + (m_fMinRowHeight + m_fRowSpacing);
					fRowHeight = 0.0f;
				}
				break;
			default:
				if (!nPos)
				{
					//用于字符串出现的异常情况(里面存在非法字符的情况)。
					if (UI_FLOAT_LARGE(fX, 0.0f))
					{
						float fNewRowEndY = fRowEndY - m_fMinRowHeight + fRowHeight;
						AdjustRowItemEndAbsY(itRowBegin, m_aItem.end(), fRowEndY, fNewRowEndY);
						itRowBegin = m_aItem.end() - 1;

						fX = 0.0f;
						fRowEndY = fNewRowEndY + (m_fMinRowHeight + m_fRowSpacing);
						fRowHeight = 0.0f;

						break;
					}
					//m_fWidth 太小以至于无法显示一个文字。所以退出循环。
					nContinueLoop = false;
					break;
				}
				pItemText->AppendDrawTextData(
					D3DXVECTOR2(fStartX + fX, fRowEndY - fHeight), 
					D3DXVECTOR2(fStartX + fX + fPos, fRowEndY),
					wszText, nPos
					);
				fX += fPos;
				if (UI_FLOAT_LARGE_OR_EQUAL(fX, m_fWidth))
				{
					float fNewRowEndY = fRowEndY - m_fMinRowHeight + fRowHeight;
					AdjustRowItemEndAbsY(itRowBegin, m_aItem.end(), fRowEndY, fNewRowEndY);
					itRowBegin = m_aItem.end() - 1;

					fX = 0.0f;
					fRowEndY = fNewRowEndY + (m_fMinRowHeight + m_fRowSpacing);
					fRowHeight = 0.0f;
				}
				break;
			}
			wszText += nPos;
			fPos = m_fWidth - fX;
			nPos = pItemText->GetSplitPos(wszText, fPos, nControlType);
		}

		float fNewRowEndY = fRowEndY - m_fMinRowHeight + fRowHeight;
		AdjustRowItemEndAbsY(itRowBegin, m_aItem.end(), fRowEndY, fNewRowEndY);

		pItemText->GetAbsPos(fX, fY);
		pItemText->GetSize(fWidth, fHeight);
		K_RICH_ITEM_SHOW_TEST_V(pItemText, fX, fY, fWidth, fHeight);
	}
	else
	{
		//不是文字,或者该段文字不能由handle设置绘制数据。
		pItem->GetSize(fWidth, fHeight);
/*
		if (UI_FLOAT_LARGE(fWidth, m_fWidth))
		{
			//如果宽度超过了最大高度。如果等比索放在这里操作
			fWidth = m_fWidth;
			pItem->SetSize(fWidth, fHeight);
		}
*/
		if (UI_FLOAT_LARGE(fX + fWidth, fStartX + m_fWidth) && !UI_FLOAT_EQUAL(fX, fStartX))
		{
			//这一行已经放不下了，从下一行直接开始。
			fX = fStartX;
			fY += (m_fMinRowHeight + m_fRowSpacing);
			fRowHeight = m_fMinRowHeight;
			itRowBegin = m_aItem.end() - 1;
		}

		if (UI_FLOAT_LARGE(fHeight, fRowHeight))
			fRowHeight = fHeight;

		pItem->SetAbsPos(fX, fY - fHeight);

		float fNewRowEndY = fY - m_fMinRowHeight + fRowHeight;
		AdjustRowItemEndAbsY(itRowBegin, m_aItem.end(), fY, fNewRowEndY);
	}

	pItem->GetAbsPos(fX, fY);
	pItem->GetSize(fWidth, fHeight);
	CorrectAllItemRect(fX - m_fAbsX, fY - m_fAbsY, fWidth, fHeight);
	if (!pItemText)
	{
		K_ITEM_SHOW_TEST(pItem, fX, fY, fWidth, fHeight);
	}
	return true;
}

int KItemHandle::FormatLastItemPosByLeftRightAlignByTable(int nCount)
{
	KItemNull *pItem = NULL;
	KItemNull *pItemLast = NULL;
	KItemText *pItemText = NULL;

	m_fMinRowHeight		= 30.0f;
	float fWidth        = 0.0f;
	float fHeight       = 0.0f;
	float fStartX       = m_fAbsX + m_fItemStartRelX;
	float fStartY       = m_fAbsY + m_fItemStartRelY;
	float fX			= fStartX;
	float fY			= fStartY + m_fMinRowHeight;
	float fRowHeight    =  0.0f;
	int nNextItemRightAlign = false;

	ASSERT(nCount > 1 && nCount == (int)m_aItem.size());
	pItem = m_aItem[nCount - 1];
	ASSERT(pItem);
	pItemLast = m_aItem[nCount - 2];
	ASSERT(pItemLast);

	KItemArray::iterator itRowBegin = m_aItem.begin();
	for (int i = nCount - 2; i >= 0; --i)
	{
		LPCWSTR wszControlKey = NULL;
		pItemLast = m_aItem[i];
		ASSERT(pItemLast);
		pItemText = g_ItemCast<KItemText>(pItemLast);
		if (pItemText && pItemText->IsRichText())
		{
			//普通图文文本
			if (!pItemText->GetLastDrawBlockInfo(fX, fY, &wszControlKey))
				continue;
		}
		else
		{
			//前一个item不是文字,或者该段文字不能由handle设置绘制数据。
			pItemLast->GetAbsPos(fX, fY);
			pItemLast->GetSize(fWidth, fHeight);
			fX += fWidth;
			fY += fHeight;
			if (UI_FLOAT_LARGE(fHeight, fRowHeight))
				fRowHeight = fHeight;
		}

		int nFindBack = true;
		for (int j = i; j < nCount - 1; ++j)
		{
			if (!wszControlKey)
			{
				pItemText = g_ItemCast<KItemText>(m_aItem[j]);
				if(!pItemText)
					continue;
				wszControlKey = pItemText->GetText();
				if (!wszControlKey)
					continue;
			}

			fHeight = pItemText->GetSingleTextHeight();
			if (UI_FLOAT_LARGE(fHeight, fRowHeight))
				fRowHeight = fHeight;
			if (nNextItemRightAlign)
			{
				fX = fStartX + m_fWidth;
				nNextItemRightAlign = false;
			}
			while (*wszControlKey != L'\0')
			{
				if (*wszControlKey == L'\t')
				{
					fX += pItemText->GetTableTextWidth();
					if(UI_FLOAT_LARGE(fX, fStartX + m_fWidth))
					{
						fRowHeight = fHeight;
						fX = fStartX;
						fY += (fRowHeight + m_fRowSpacing);
						//fRowHeight = 0.0f;
						nFindBack = false;
					}	
					nNextItemRightAlign = true;
				}
				if (*wszControlKey == L'\n')
				{
					fRowHeight = fHeight;
					fX = fStartX;
					fY += (fRowHeight + m_fRowSpacing);
					//fRowHeight = 0.0f;
					nFindBack = false;
				}
				++wszControlKey;
			}
			wszControlKey = NULL;
		}
		if (!nFindBack)
		{
			itRowBegin = m_aItem.end() - 1; 
			break;
		}
		//回溯
		for (int j = i; j >= 0; --j)
		{
			float fX1 = 0.0f;
			float fY1 = 0.0f;
			LPCWSTR wszControlKey = NULL;
			pItemLast = m_aItem[j];
			ASSERT(pItemLast);
			pItemText = g_ItemCast<KItemText>(pItemLast);
			if (pItemText && pItemText->IsRichText())
			{
				if (pItemText->GetLastDrawBlockInfo(fX1, fY1, &wszControlKey))
				{
					if (!(UI_FLOAT_EQUAL(fY1, fY)))
					{
						itRowBegin = m_aItem.begin() + j + 1;
						break;
					}
					fHeight = pItemText->GetSingleTextHeight();
					if (UI_FLOAT_LARGE(fHeight, fRowHeight))
						fRowHeight = fHeight;
				}
			}
			else
			{
				pItemLast->GetAbsPos(fX1, fY1);
				pItemLast->GetSize(fWidth, fHeight);
				if (!(UI_FLOAT_EQUAL(fY1 + fHeight, fY)))
				{
					itRowBegin = m_aItem.begin() + j + 1;
					break;
				}
				if (UI_FLOAT_LARGE(fHeight, fRowHeight))
					fRowHeight = fHeight;
			}
		}
		break;
	}

	if (!(UI_FLOAT_EQUAL(fRowHeight, m_fMinRowHeight)))
	{
		float fNewRowEndY = fY - fRowHeight + m_fMinRowHeight;
		AdjustRowItemEndAbsY(itRowBegin, m_aItem.end() - 1, fY, fNewRowEndY);
		fY = fNewRowEndY;
	}

	pItemText = g_ItemCast<KItemText>(pItem);
	if (pItemText && pItemText->IsRichText())
	{
		//一般图文
		//是文字,并且该文字是handle设置绘制数据。
		fX = fX - fStartX;
		float fRowEndY = fY;

		LPCWSTR wszText = pItemText->GetText();
		float fPos = m_fWidth - fX;
		int nControlType = 0;
		int nContinueLoop = true;
		pItemText->ClearDrawTextData();
		pItemText->SetAbsPos(fStartX + fX, fRowEndY);
		float fTableWidth = pItemText->GetTableTextWidth();
		fHeight = pItemText->GetSingleTextHeight();

		int nPos = pItemText->GetSplitPos(wszText, fPos, nControlType);
		while (nPos != -1 && nContinueLoop)
		{
			//文字不管最大高，不然会出错。
			if (UI_FLOAT_LARGE(fHeight, fRowHeight))
				fRowHeight = fHeight;

			//下一段右对齐
			if (nNextItemRightAlign) 
			{
				fX = m_fWidth - fPos;
				nNextItemRightAlign = false;
			}

			switch(nControlType)
			{
			case ITEM_TEXT_CONTROL_KEY_TAB:
				pItemText->AppendDrawTextData(
					D3DXVECTOR2(fStartX + fX, fRowEndY - fHeight), 
					D3DXVECTOR2(fStartX + fX + fPos, fRowEndY),
					wszText, nPos
					);
				fX += fPos;
				++nPos;
				fX += fTableWidth;
				nNextItemRightAlign = true;
				if (UI_FLOAT_LARGE(fX,m_fWidth))
				{
					float fNewRowEndY = fRowEndY - m_fMinRowHeight + fRowHeight;
					AdjustRowItemEndAbsY(itRowBegin, m_aItem.end(), fRowEndY, fNewRowEndY);
					itRowBegin = m_aItem.end() - 1;

					fX = 0.0f;
					fRowEndY = fNewRowEndY + (m_fMinRowHeight + m_fRowSpacing);
					fRowHeight = 0.0f;
				}
				break;
			case ITEM_TEXT_CONTROL_KEY_NEW_LINE:
				pItemText->AppendDrawTextData(
					D3DXVECTOR2(fStartX + fX, fRowEndY - fHeight), 
					D3DXVECTOR2(fStartX + fX + fPos, fRowEndY),
					wszText, nPos
					);
				++nPos;
				{
					float fNewRowEndY = fRowEndY - m_fMinRowHeight + fRowHeight;
					AdjustRowItemEndAbsY(itRowBegin, m_aItem.end(), fRowEndY, fNewRowEndY);
					itRowBegin = m_aItem.end() - 1;;

					fX = 0.0f;
					fRowEndY = fNewRowEndY + (m_fMinRowHeight + m_fRowSpacing);
					fRowHeight = 0.0f;
				}
				break;
			default:
				if (!nPos)
				{
					//用于字符串出现的异常情况(里面存在非法字符的情况)。
					if (UI_FLOAT_LARGE(fX, 0.0f))
					{
						float fNewRowEndY = fRowEndY - m_fMinRowHeight + fRowHeight;
						AdjustRowItemEndAbsY(itRowBegin, m_aItem.end(), fRowEndY, fNewRowEndY);
						itRowBegin = m_aItem.end() - 1;

						fX = 0.0f;
						fRowEndY = fNewRowEndY + (m_fMinRowHeight + m_fRowSpacing);
						fRowHeight = 0.0f;

						break;
					}
					//m_fWidth 太小以至于无法显示一个文字。所以退出循环。
					nContinueLoop = false;
					break;
				}
				pItemText->AppendDrawTextData(
					D3DXVECTOR2(fStartX + fX, fRowEndY - fHeight), 
					D3DXVECTOR2(fStartX + fX + fPos, fRowEndY),
					wszText, nPos
					);
				fX += fPos;
				if (UI_FLOAT_LARGE_OR_EQUAL(fX, m_fWidth))
				{
					float fNewRowEndY = fRowEndY - m_fMinRowHeight + fRowHeight;
					AdjustRowItemEndAbsY(itRowBegin, m_aItem.end(), fRowEndY, fNewRowEndY);
					itRowBegin = m_aItem.end() - 1;

					fX = 0.0f;
					fRowEndY = fNewRowEndY + (m_fMinRowHeight + m_fRowSpacing);
					fRowHeight = 0.0f;
				}
				break;
			}
			wszText += nPos;
			fPos = m_fWidth - fX;
			nPos = pItemText->GetSplitPos(wszText, fPos, nControlType);
		}

		float fNewRowEndY = fRowEndY - m_fMinRowHeight + fRowHeight;
		AdjustRowItemEndAbsY(itRowBegin, m_aItem.end(), fRowEndY, fNewRowEndY);

		pItemText->GetAbsPos(fX, fY);
		pItemText->GetSize(fWidth, fHeight);

		K_RICH_ITEM_SHOW_TEST_V(pItemText, fX, fY, fWidth, fHeight);
	}
	else
	{
		//不是文字,或者该段文字不能由handle设置绘制数据。
		pItem->GetSize(fWidth, fHeight);
/*
		if (UI_FLOAT_LARGE(fWidth, m_fWidth))
		{
			//如果宽度超过了最大高度。如果等比索放在这里操作
			fWidth = m_fWidth;
			pItem->SetSize(fWidth, fHeight);
		}
*/
		if (UI_FLOAT_LARGE(fX + fWidth, fStartX + m_fWidth) && !UI_FLOAT_EQUAL(fX, fStartX))
		{
			//这一行已经放不下了，从下一行直接开始。
			fX = fStartX;
			fY += (m_fMinRowHeight + m_fRowSpacing);
			fRowHeight = m_fMinRowHeight;
			itRowBegin = m_aItem.end() - 1;
		}

		if (UI_FLOAT_LARGE(fHeight, fRowHeight))
			fRowHeight = fHeight;

		pItem->SetAbsPos(fX, fY - fHeight);

		float fNewRowEndY = fY - m_fMinRowHeight + fRowHeight;
		AdjustRowItemEndAbsY(itRowBegin, m_aItem.end(), fY, fNewRowEndY);
	}

	pItem->GetAbsPos(fX, fY);
	pItem->GetSize(fWidth, fHeight);
	CorrectAllItemRect(fX - m_fAbsX, fY - m_fAbsY, fWidth, fHeight);
	if (!pItemText)
	{
		K_ITEM_SHOW_TEST(pItem, fX, fY, fWidth, fHeight);
	}
	return true;
}

int KItemHandle::FormatLastItemPosByChatItemHandle(int nCount)
{
	return FormatLastItemPosByAutoNewLineRowHeightByMaxItem(nCount);
}

int KItemHandle::FormatLastItemPosByTreeItemHandle(int nCount)
{
	return FormatAllItemPosByTreeItemHandle();
}

inline int KItemHandle::CorrectAllItemRect(float fX, float fY, float fW, float fH)
{
	float fStartAllX = m_fItemStartRelX;
	float fStartAllY = m_fItemStartRelY;  
	float fEndAllX = m_fItemStartRelX + m_fAllItemWidth;
	float fEndAllY = m_fItemStartRelY + m_fAllItemHeight;
	if (fX + fW > fEndAllX)
		fEndAllX = fX + fW;
	if (fY + fH > fEndAllY)
		fEndAllY = fY + fH;

	m_fAllItemWidth = fEndAllX - fStartAllX;
	m_fAllItemHeight = fEndAllY - fStartAllY;

	return true;
}

int	KItemHandle::EnableFormatWhenAppend(int nFlag)
{
	m_dwStyle &= ~ITEM_HANDLE_FORMAT_POS_WHEN_APPEND;
	if (nFlag)
		m_dwStyle |= ITEM_HANDLE_FORMAT_POS_WHEN_APPEND;
	return true;
}

int	KItemHandle::RemoveItemUntilNewLine()
{
	switch(m_dwHandleType) 
	{
	case ITEM_HANDLE_AUTO_NEW_LINE_ROW_HEIGHT_CHANGELESS:
	case ITEM_HANDLE_AUTO_NEW_LINE_ROW_HEIGHT_BETWEEN_MIN_MAX:
	case ITEM_HANDLE_AUTO_NEW_LINE_ROW_HEIGHT_BY_MAX_ITEM:
	case ITEM_HANDLE_CHAT_ITEM_HANDLE:
	case ITEM_HANDLE_LEFT_RIGHT_ALIGN_BY_TABLE:
		break;
	default:
		return false;
	}

	KItemArray::iterator it = m_aItem.begin();
	if(it == m_aItem.end())
		return true;

	float fX0 = 0.0f;
    float fY0 = 0.0f;
	(*it)->GetAbsPos(fX0, fY0);

	while(it != m_aItem.end())
	{
		KItemText *pItemText = g_ItemCast<KItemText>(*it);
		if (pItemText)
		{
			int nPos = 0;
			if (g_GetNextNewLineKeyW(pItemText->GetText(), nPos) == ITEM_TEXT_CONTROL_KEY_NEW_LINE)
			{
				SAFE_RELEASE(*it);
				it = m_aItem.erase(it);
				break;
			}
		}
		SAFE_RELEASE(*it);
		it = m_aItem.erase(it);
	}

	while(it != m_aItem.end())
	{
		KItemText *pItemText = g_ItemCast<KItemText>(*it);
		if (!pItemText)
			break;
		LPCWSTR wszText = pItemText->GetText();
		if (wszText && *wszText != '\0')
			break;
		SAFE_RELEASE(*it);
		it = m_aItem.erase(it);
	}

	if (it == m_aItem.end())
	{
		m_fItemStartRelY = 0.0f;
		m_fItemStartRelX = 0.0f;
		m_fAllItemHeight = 0.0f;
		m_fAllItemWidth = 0.0f;
		return true;
	}

	float fX1, fY1;
	(*it)->GetAbsPos(fX1, fY1);
	m_fAllItemHeight -= (fY1 - fY0);
	m_fItemStartRelY = fY1 - m_fAbsY;

	return true;
}


int	KItemHandle::SetAjustShowRichTextType(int nType)
{
	m_dwStyle &= ~ITEM_HANDLE_AJUST_RICH_TEXT_SHOW_H;
	if (nType)
	{
		m_dwStyle |= ITEM_HANDLE_AJUST_RICH_TEXT_SHOW_H;
	}
	return true;
}

KWndWindow *KItemHandle::GetOwner()
{
	return m_pWndOwner;
}

int KItemHandle::SetOwner(KWndWindow *pOwner)
{
	m_pWndOwner = pOwner;
	return true;
}

int KItemHandle::ExchangeIndex(int nFirst, int nLast)
{
	int nResult = false;
	int nSize = (int)(m_aItem.size());
	KItemNull *pTemp = NULL;
	KG_PROCESS_ERROR(nFirst < nSize && nFirst >= 0);
	KG_PROCESS_ERROR(nLast < nSize && nLast >= 0);

	pTemp = m_aItem[nFirst];
	m_aItem[nFirst] = m_aItem[nLast];
	m_aItem[nLast] = pTemp;

	NotifyDraw();

	nResult = true;
Exit0:
	return nResult;
}

void KItemHandle::Sort()
{
	struct KItemUserDataLess
	{
		bool operator()(KItemNull* lhs, KItemNull* rhs)
		{
			ASSERT(lhs);
			ASSERT(rhs);

			return lhs->GetUserData() < rhs->GetUserData();
		}
	};

	std::sort(m_aItem.begin(), m_aItem.end(), KItemUserDataLess());
	NotifyDraw();
}

void KItemHandle::SetDisableExtent(int nDisable)
{
	m_dwStyle &= ~ITEM_HANDLE_STENCIL_DISABLE_EXTENT;
	if (nDisable)
		m_dwStyle |= ITEM_HANDLE_STENCIL_DISABLE_EXTENT;
}

int KItemHandle::IsDisableExtent()
{
	if (m_dwStyle & ITEM_HANDLE_STENCIL_DISABLE_EXTENT)
		return true;
	return false;
}

void KItemHandle::SetControlShow(int nControl)
{
	m_dwStyle &= ~ITEM_HANDLEL_CONTROL_SHOW;
	if (nControl)
		m_dwStyle |= ITEM_HANDLEL_CONTROL_SHOW;
}

int KItemHandle::IsControlShow()
{
	if (m_dwStyle & ITEM_HANDLEL_CONTROL_SHOW)
		return true;
	return false;
}

int KItemHandle::IsInStencialArea(float fX, float fY)
{
	if (m_dwStyle & ITEM_HANDLE_USE_STENCIL)
		return PtInItem(fX, fY);
	if (m_pParent)
		return m_pParent->IsInStencialArea(fX, fY);
	return TRUE;
}

int	KItemHandle::NeedExtent()
{
	if (!IsDisableExtent() &&
		(m_dwHandleType == ITEM_HANDLE_AUTO_NEW_LINE_ROW_HEIGHT_CHANGELESS || 
		m_dwHandleType == ITEM_HANDLE_AUTO_NEW_LINE_ROW_HEIGHT_BETWEEN_MIN_MAX || 
		m_dwHandleType == ITEM_HANDLE_AUTO_NEW_LINE_ROW_HEIGHT_BY_MAX_ITEM || 
		m_dwHandleType == ITEM_HANDLE_LEFT_RIGHT_ALIGN_BY_TABLE))
		return true;
	return false;
}

int KItemHandle::UpdateLevel(int nLevel)
{
    int nRetCode = false;
    int nResult = false;

    KG_PROCESS_SUCCESS(m_nLevel == nLevel);

    nRetCode = KItemNull::UpdateLevel(nLevel);
    KGLOG_PROCESS_ERROR(nRetCode);

    for (KItemArray::iterator it = m_aItem.begin(); it != m_aItem.end(); ++it)
    {
        KItemNull* pItem = *it; 

        nRetCode = pItem->UpdateLevel(nLevel + 1);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

}//namespace UI