/*****************************************************************************************
//	界面窗口体系结构--列表窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-9-11
*****************************************************************************************/
#include "../../Public/KWin32.h"
#include "../../Public/KIniFile.h"
#include "../Elem/WndMessage.h"
#include "WndList2.h"
#include "UiImage.h"
#include "../../Public//Text.h"
#include "../../Public/iRepresentShell.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern iRepresentShell*	g_pRepresentShell;

struct KWndList2Item
{
	int		Data;
	DWORD	StringSize;
	char	String[1];
};

//--------------------------------------------------------------------------
//	功能：构造函数
//--------------------------------------------------------------------------
KWndList2::KWndList2()
{
	m_pContent		= NULL;
	m_nItemSpace	= 0;
	m_pScrollbar    = NULL;
	m_nNumItem		= 0;
	m_nFontSize		= 12;
	m_nTopItemIndex = 0;
	m_nSelItemIndex		  = -1;
	m_nHighLightItemIndex = -1;
	m_ItemBorderColor = m_SelItemBorderColor = m_HighLightBorderColor = 0;
}

//--------------------------------------------------------------------------
//	功能：析构函数
//--------------------------------------------------------------------------
KWndList2::~KWndList2()
{
	ResetContent();
}

void KWndList2::Clone(KWndList2* pCopy)
{
	if (pCopy)
	{
		pCopy->m_nFontSize	= m_nFontSize;
		pCopy->m_ItemColor	= m_ItemColor;
		pCopy->m_SelItemColor	= m_SelItemColor;
		pCopy->m_HighLightColor	= m_HighLightColor;
		pCopy->m_ItemBorderColor = m_ItemBorderColor;
		pCopy->m_SelItemBorderColor = m_SelItemBorderColor;
		pCopy->m_HighLightBorderColor = m_HighLightBorderColor;
	}
}

//--------------------------------------------------------------------------
//	功能：初始化
//--------------------------------------------------------------------------
int KWndList2::Init(KIniFile* pIniFile, const char* pSection)
{
	if (KWndWindow::Init(pIniFile, pSection))
	{
		pIniFile->GetInteger(pSection, "Font", 16, &m_nFontSize);
		if (m_nFontSize < 8)
			m_nFontSize = 8;
		char	Buff[16];
		pIniFile->GetString(pSection, "Color", "", Buff, 16);
		m_ItemColor = GetColor(Buff);
		pIniFile->GetString(pSection, "BorderColor", "", Buff, 16);
		m_ItemBorderColor = GetColor(Buff);
		pIniFile->GetString(pSection, "SelColor", "", Buff, 16);
		m_SelItemColor = GetColor(Buff);
		pIniFile->GetString(pSection, "SelBorderColor", "", Buff, 16);
		m_SelItemBorderColor = GetColor(Buff);

		int		nValue;
		pIniFile->GetInteger(pSection, "HighLight", 0, &nValue);
		if (nValue)
		{
			m_Style |= WNDLIST2_ES_HIGHLIGHT_ENABLE;
			pIniFile->GetString(pSection, "HighLightColor", "", Buff, 16);
			m_HighLightColor = GetColor(Buff);
			pIniFile->GetString(pSection, "HighLightBorderColor", "", Buff, 16);
			m_HighLightBorderColor = GetColor(Buff);
		}
		else
			m_Style &= ~WNDLIST2_ES_HIGHLIGHT_ENABLE;

		return true;
	}
	return false;
}


void KWndList2::SetScrollbar(KWndScrollBar* pScroll)
{
	if (m_pScrollbar = pScroll)
	{
		m_pScrollbar->SetStyle(m_pScrollbar->GetStyle() | WND_S_MOVEALBE);
		if (m_nNumItem > GetVisibleLineCount())
		{
			m_pScrollbar->Enable(true);
			m_pScrollbar->SetValueRange(0, m_nNumItem - GetCount());
		}
		else
			m_pScrollbar->Enable(false);
	}
}


//--------------------------------------------------------------------------
//	功能：窗口函数
//--------------------------------------------------------------------------
int KWndList2::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch(uMsg)
	{
	case WM_MOUSEMOVE://to be check
		OnMouseMove(GET_X_LPARAM(nParam), GET_Y_LPARAM(nParam));
		break;
	case WM_LBUTTONDOWN:
		OnLButtonDown(GET_X_LPARAM(nParam), GET_Y_LPARAM(nParam));
		break;
	case WM_MOUSEWHEEL:
		if (m_pScrollbar && !m_pScrollbar->IsDisable())
		{
			int zDelta = GET_Y_LPARAM(uParam);
			int nPos = m_pScrollbar->GetScrollPos();
			nPos += (-zDelta / WHEEL_DELTA);
			m_pScrollbar->SetScrollPos(nPos);
		}
		break;
	default:
		return KWndWindow::WndProc(uMsg, nParam, nParam);
	}
	return 0;
}


int KWndList2::SetCurSel(int nSel)
{
	if (nSel >= 0 && nSel < m_nNumItem && nSel != m_nSelItemIndex)
	{
		m_nSelItemIndex = nSel;
		if (m_nSelItemIndex < m_nTopItemIndex)
			m_nTopItemIndex = m_nSelItemIndex;
		else if(m_nSelItemIndex >= m_nTopItemIndex + GetVisibleLineCount())
		{
			m_nTopItemIndex = m_nSelItemIndex - GetVisibleLineCount() + 1;
			if (m_nTopItemIndex < 0)
				m_nTopItemIndex = 0;
		}

		if (m_pParentWnd)
			m_pParentWnd->WndProc(WND_N_LIST_ITEM_SEL, (unsigned int)(KWndWindow*)this, m_nSelItemIndex);
	}
	else if(nSel == -1)
	{
		m_nSelItemIndex = -1;
	}
	return m_nSelItemIndex;
}

//--------------------------------------------------------------------------
//	功能：响应鼠标左键在此按下
//--------------------------------------------------------------------------
void KWndList2::OnLButtonDown(int x, int y)
{
	int nSel = m_nTopItemIndex + (y - m_nAbsoluteTop) / (m_nFontSize + 1);
	if (nSel >= m_nNumItem)
		nSel = -1;
	if (nSel != m_nSelItemIndex)
	{
		m_nSelItemIndex = nSel;
		if (m_pParentWnd)
			m_pParentWnd->WndProc(WND_N_LIST_ITEM_SEL, (unsigned int)(KWndWindow*)this, m_nSelItemIndex);
	}
}

//--------------------------------------------------------------------------
//	功能：响应鼠标移动
//--------------------------------------------------------------------------
void KWndList2::OnMouseMove(int x, int y)
{
	if (m_Style & WNDLIST2_ES_HIGHLIGHT_ENABLE)
	{
		int nIndex = m_nTopItemIndex + (y - m_nAbsoluteTop) / (m_nFontSize + 1);
		if (nIndex >= m_nNumItem)
			nIndex = -1;
		if (nIndex != m_nHighLightItemIndex)
		{
			m_nHighLightItemIndex = nIndex;
			if (m_pParentWnd)
				m_pParentWnd->WndProc(WND_N_LIST_ITEM_HIGHLIGHT, (unsigned int)(KWndWindow*)this, m_nHighLightItemIndex);
		}
	}
}

//--------------------------------------------------------------------------
//	功能：添加项
//	参数：int nIndex --> 新项添加的位置(基于0的索引数)
//		　const char* pString --> 新项的字符串
//	返回：成功添加的话，返回新项在列表中的索引未知；否则返回WNDLIST_ERROR，
//			失败的原因可能是pString指针为NULL，或者无法为新项分配存储空间。
//--------------------------------------------------------------------------
int KWndList2::AddString(int nIndex, const char* pString)
{
	if (pString == NULL)
		return WNDLIST_ERROR;
	if (m_nItemSpace >= m_nNumItem)
	{
		void** pNew = (void**)realloc(m_pContent, sizeof(void*) * (m_nItemSpace + 10));
		if (pNew)
		{
			m_pContent = pNew;
			m_nItemSpace += 10;
		}
		else
			return WNDLIST_ERROR;
	}

	char szString[256];
	strcpy(szString, pString);
	int nStrLen = strlen(pString);
	nStrLen = TEncodeText(szString, nStrLen);
	KWndList2Item* pItem = (KWndList2Item*)malloc(sizeof(KWndList2Item) + nStrLen);
	if (pItem)
	{
		pItem->Data = 0;
		pItem->StringSize = nStrLen;
		memcpy(pItem->String, szString, nStrLen);
		pItem->String[nStrLen] = 0;

		if (nIndex < 0)
			nIndex = 0;
		else if (nIndex > m_nNumItem)
			nIndex = m_nNumItem;
		for (int i = m_nNumItem; i > nIndex; i--)
			m_pContent[i] = m_pContent[i - 1];
		m_pContent[nIndex] = pItem;
		m_nNumItem ++;

		if (m_Style & WNDLIST2_ES_HIGHLIGHT_ENABLE)
		{
			m_nHighLightItemIndex = -1;
			if (m_pParentWnd)
				m_pParentWnd->WndProc(WND_N_LIST_ITEM_HIGHLIGHT, (unsigned int)(KWndWindow*)this, m_nHighLightItemIndex);
		}
		if (m_nSelItemIndex >= nIndex)
		{
			m_nSelItemIndex ++;
			if (m_pParentWnd)
				m_pParentWnd->WndProc(WND_N_LIST_ITEM_SEL, (unsigned int)(KWndWindow*)this, m_nSelItemIndex);
		}
		if (m_pScrollbar)
	    {
    		if(m_nNumItem > GetVisibleLineCount())
		    {
    			m_pScrollbar->Enable(TRUE);
		    }
		    else
		    {
    			m_pScrollbar->Enable(FALSE);
		    }
			m_pScrollbar->SetValueRange(0, m_nNumItem);
	    }
	}
	else
		nIndex = WNDLIST_ERROR;
	return nIndex;
}

//--------------------------------------------------------------------------
//	功能：删除项
//	参数：int nIndex --> 要删除项的索引
//	返回：如果成功删除指定的项，则返回列表里剩余项的数目，否则返回WNDLIST_ERROR
//--------------------------------------------------------------------------
int	KWndList2::DeleteString(int nIndex)
{
	if (nIndex >= 0 && nIndex < m_nNumItem)
	{
		free(m_pContent[nIndex]);
		m_nNumItem --;
		for (int i = nIndex; i < m_nNumItem; i++)
			m_pContent[i] = m_pContent[i + 1];
		if (m_nTopItemIndex >= m_nNumItem && m_nTopItemIndex)
			m_nTopItemIndex --;
		if (m_nSelItemIndex > nIndex)
			m_nSelItemIndex --;
		else if (m_nSelItemIndex == nIndex)
			m_nSelItemIndex = -1;
		if (m_Style & WNDLIST2_ES_HIGHLIGHT_ENABLE)
		{
			m_nHighLightItemIndex = -1;
			if (m_pParentWnd)
				m_pParentWnd->WndProc(WND_N_LIST_ITEM_HIGHLIGHT, (unsigned int)(KWndWindow*)this, m_nHighLightItemIndex);
		}
		if (m_pParentWnd)
				m_pParentWnd->WndProc(WND_N_LIST_ITEM_SEL, (unsigned int)(KWndWindow*)this, m_nSelItemIndex);
		if (m_pScrollbar)
	    {
    		if(m_nNumItem > GetVisibleLineCount())
		    {
    			m_pScrollbar->Enable(TRUE);
		    }
		    else
		    {
    			m_pScrollbar->Enable(FALSE);
		    }
			m_pScrollbar->SetValueRange(0, m_nNumItem);
	    }
		return m_nNumItem;
	}
	else
		return WNDLIST_ERROR;
}

//--------------------------------------------------------------------------
//	功能：删除列表中全部的内容
//--------------------------------------------------------------------------
void KWndList2::ResetContent()
{
	if (m_pContent)
	{
		for (int i = 0; i < m_nNumItem; i++)
			free(m_pContent[i]);
		m_nNumItem = 0;
		m_nItemSpace = 0;
		free(m_pContent);
		m_pContent = NULL;
	}
	m_nTopItemIndex = 0;
	m_nSelItemIndex = -1;
	m_nHighLightItemIndex = -1;
	if (m_pParentWnd)
	{
		if (m_Style & WNDLIST2_ES_HIGHLIGHT_ENABLE)
			m_pParentWnd->WndProc(WND_N_LIST_ITEM_HIGHLIGHT, (unsigned int)(KWndWindow*)this, m_nHighLightItemIndex);
		m_pParentWnd->WndProc(WND_N_LIST_ITEM_SEL, (unsigned int)(KWndWindow*)this, m_nSelItemIndex);
	}
	if(m_pScrollbar)
	{
		m_pScrollbar->Enable(FALSE);
		m_pScrollbar->SetScrollPos(0);
		m_pScrollbar->SetValueRange(0, 0);
	}
}

//--------------------------------------------------------------------------
//	功能：绘制窗口
//--------------------------------------------------------------------------
void KWndList2::PaintWindow()
{
	KWndWindow::PaintWindow();
	if (g_pRepresentShell)
	{
		int y = m_nAbsoluteTop;
		int EndIndex = m_nTopItemIndex + GetVisibleLineCount();
		if (EndIndex > m_nNumItem)
			EndIndex = m_nNumItem;

		unsigned int Color;
		unsigned int BorderColor;
		KOutputTextParam Param;

		Param.nNumLine = 1;
		Param.nSkipLine = 0;
		Param.nX = m_nAbsoluteLeft;
		Param.nVertAlign = 0;
		Param.nZ = TEXT_IN_SINGLE_PLANE_COORD;
		for (int i = m_nTopItemIndex; i < EndIndex; i++)
		{
			Param.nY = y;
			if (i == m_nSelItemIndex)
			{
				Color = m_SelItemColor;
				BorderColor = m_SelItemBorderColor;
			}
			else if (i == m_nHighLightItemIndex)
			{
				Color = m_HighLightColor;
				BorderColor = m_HighLightBorderColor;
			}
			else
			{
				Color = m_ItemColor;
				BorderColor = m_ItemBorderColor;
			}
			Param.BorderColor = BorderColor;
			Param.Color = Color;
			KWndList2Item* pItem = (KWndList2Item*)m_pContent[i];
			g_pRepresentShell->OutputRichText(m_nFontSize, &Param, pItem->String, pItem->StringSize);
			y += m_nFontSize + 1;
		}
	}
}

//--------------------------------------------------------------------------
//	功能：查找列表（显示）字串
//--------------------------------------------------------------------------
int	KWndList2::FindString(int nPrecedingStart, const char* pString)
{
	if (pString)
	{
		unsigned int nLen = strlen(pString);
		for (int i = nPrecedingStart + 1; i < m_nNumItem; i++)
		{
			KWndList2Item*	pItem = (KWndList2Item*)m_pContent[i];
			if (nLen == pItem->StringSize &&
				memcmp(pItem->String, pString, nLen) == 0)
				return i;
		}
	}
	return WNDLIST_ERROR;
}

//--------------------------------------------------------------------------
//	功能：设置列表项关联数据的值
//--------------------------------------------------------------------------
int	KWndList2::SetItemData(int nIndex, int nData)
{
	if (nIndex >= 0 && nIndex < m_nNumItem)
	{
		((KWndList2Item*)m_pContent[nIndex])->Data = nData;
		return 0;
	}
	return WNDLIST_ERROR;
}

//--------------------------------------------------------------------------
//	功能：获得列表项数据中的一个整数
//--------------------------------------------------------------------------
int KWndList2::GetItemData(int nIndex)
{
	if (nIndex >= 0 && nIndex < m_nNumItem)
	{
		return (((KWndList2Item*)m_pContent[nIndex])->Data);
	}
	return WNDLIST_ERROR;
}

//--------------------------------------------------------------------------
//	功能：获得列表项的(显示)字串
//--------------------------------------------------------------------------
int KWndList2::GetString(int nIndex, char* pBuffer, unsigned int nSize)
{
	if (nIndex >= 0 && nIndex < m_nNumItem && pBuffer)
	{
		KWndList2Item* pItem = (KWndList2Item*)m_pContent[nIndex];
		if (pItem->StringSize < nSize)
		{
			memcpy(pBuffer, pItem->String, pItem->StringSize);
			pBuffer[pItem->StringSize] = 0;
			return pItem->StringSize;
		}
	}
	return WNDLIST_ERROR;
}

//--------------------------------------------------------------------------
//	功能：设置被显示的最顶列表项的索引值
//--------------------------------------------------------------------------
void KWndList2::SetTopItemIndex(int nTopItemIndex)
{
	if (nTopItemIndex >= 0 && nTopItemIndex < m_nNumItem)
		m_nTopItemIndex = nTopItemIndex;
}


//--------------------------------------------------------------------------
//	功能：获得列表框可以同时显示的项的数目
//--------------------------------------------------------------------------
int	KWndList2::GetVisibleLineCount() const
{
	return m_Height / (m_nFontSize + 1);
}
