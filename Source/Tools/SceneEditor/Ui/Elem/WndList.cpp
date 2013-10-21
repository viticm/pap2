/*****************************************************************************************
//	界面窗口体系结构--列表窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-8-8
*****************************************************************************************/
#include "../../Public/KWin32.h"
#include "../../Public/KIniFile.h"
#include "../Elem/WndMessage.h"
#include "../Elem/WndScrollBar.h"
#include "WndList.h"
#include "UiImage.h"

#include "../../Public/iRepresentShell.h"
#include "../../Public/Text.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern iRepresentShell*	g_pRepresentShell;
#define	NO_BG_COLOR		0xffffffff

#define IS_MULTI_SEL	(m_Style & WNDLIST_ES_MULTI_SEL)

//--------------------------------------------------------------------------
//	功能：构造函数
//--------------------------------------------------------------------------
KWndList::KWndList()
{
	m_pContent		= NULL;
	m_nNumItem		= 0;
	m_nItemSize		= 0;
	m_nStringOffset = 0;
	m_nColorOffset  = NO_CUSTOM_COLOR;
	m_nTopItemIndex = 0;
	m_nFontSize		= 12;
	m_nSelItemIndex		  = -1;
	m_nHighLightItemIndex = -1;
	m_nNumColumn = 1;
	m_uIsItemSelStatus = 0;
	m_puItemSelStatusList = NULL;
	m_uNumSelStatusList = 0;
	m_pScrollbar = NULL;
	m_ItemBorderColor = m_SelItemBorderColor = m_HighLightBorderColor = 0;
}

void KWndList::Clone(KWndList* pCopy)
{
	if (pCopy)
	{
		KWndWindow::Clone(pCopy);
		pCopy->m_nFontSize = m_nFontSize;
		pCopy->m_nNumColumn = m_nNumColumn;
		pCopy->m_ItemColor	= m_ItemColor;
		pCopy->m_SelItemColor = m_SelItemColor;
		pCopy->m_uSelItemBgColor = m_uSelItemBgColor;
		pCopy->m_HighLightColor	 = m_HighLightColor;
		pCopy->m_ItemBorderColor = m_ItemBorderColor;
		pCopy->m_SelItemBorderColor = m_SelItemBorderColor;
		pCopy->m_HighLightBorderColor = m_HighLightBorderColor;
		pCopy->EnableMultiSel(IS_MULTI_SEL);
		pCopy->UpdateData();
	}
}

KWndList::~KWndList()
{
	EnableMultiSel(0);
}

void KWndList::SetScrollbar(KWndScrollBar* pScroll)
{
	if (m_pScrollbar = pScroll)
	{
		m_pScrollbar->SetStyle(m_pScrollbar->GetStyle() | WND_S_MOVEALBE);
		if (m_nNumItem > GetVisibleItemCount())
		{
			m_pScrollbar->Enable(true);
			m_pScrollbar->SetValueRange(0, m_nNumItem - GetVisibleItemCount());
		}
		else
			m_pScrollbar->Enable(false);
	}
}

//--------------------------------------------------------------------------
//	功能：初始化
//--------------------------------------------------------------------------
int KWndList::Init(KIniFile* pIniFile, const char* pSection)
{
	if (KWndWindow::Init(pIniFile, pSection))
	{
		pIniFile->GetInteger(pSection, "NumColumn", 1, &m_nNumColumn);
		if (m_nNumColumn < 1)
			m_nNumColumn = 1;
		pIniFile->GetInteger(pSection, "Font", 16, &m_nFontSize);
		if (m_nFontSize < 8)
			m_nFontSize = 8;
		//====读取文字对齐方式====
		m_Style &= ~WNDLIST_ES_HALIGN_FILTER;
		int		nValue;
		pIniFile->GetInteger(pSection, "HAlign", 0, &nValue);
		if (nValue == 1)
			m_Style |= WNDLIST_ES_HALIGN_CENTRE;
		else if (nValue == 2)
			m_Style |= WNDLIST_ES_HALIGN_RIGHT;

		char	Buff[16];
		pIniFile->GetString(pSection, "Color", "", Buff, 16);
		m_ItemColor = GetColor(Buff);
		pIniFile->GetString(pSection, "BorderColor", "", Buff, 16);
		m_ItemBorderColor = GetColor(Buff);
		pIniFile->GetString(pSection, "SelColor", "", Buff, 16);
		m_SelItemColor = GetColor(Buff);
		pIniFile->GetString(pSection, "SelBorderColor", "", Buff, 16);
		m_SelItemBorderColor = GetColor(Buff);
		if (pIniFile->GetString(pSection, "SelItemBgColor", "", Buff, 16) && Buff[0])
			m_uSelItemBgColor = (GetColor(Buff) & 0xffffff);
		else
			m_uSelItemBgColor = NO_BG_COLOR;
		pIniFile->GetInteger(pSection, "HighLight", 0, &nValue);
		if (nValue)
		{
			m_Style |= WNDLIST_ES_HIGHLIGHT_ENABLE;
			pIniFile->GetString(pSection, "HighLightColor", "", Buff, 16);
			m_HighLightColor = GetColor(Buff);
			pIniFile->GetString(pSection, "HighLightBorderColor", "", Buff, 16);
			m_HighLightBorderColor = GetColor(Buff);
		}
		else
		{
			m_nHighLightItemIndex = -1;
			m_Style &= ~WNDLIST_ES_HIGHLIGHT_ENABLE;
		}
		pIniFile->GetInteger(pSection, "MultiSel", 0, &nValue);
		EnableMultiSel(nValue);

		UpdateData();
		return true;
	}
	return false;
}

//启用/禁用多选
void KWndList::EnableMultiSel(int bEnable)
{
	if (bEnable && IS_MULTI_SEL == 0)
	{
		m_Style |= WNDLIST_ES_MULTI_SEL;
		m_uIsItemSelStatus = 0;
		m_nSelItemIndex = -1;
		if (m_nNumItem > 32)
		{
			m_uNumSelStatusList = (m_nNumItem - 1) / 32;
			m_puItemSelStatusList = (unsigned int*)malloc(sizeof(unsigned int) * m_uNumSelStatusList);
			if (m_puItemSelStatusList)
				memset(m_puItemSelStatusList, 0, sizeof(unsigned int) * m_uNumSelStatusList);
			else
			{
				m_uNumSelStatusList = 0;
				m_Style &= ~WNDLIST_ES_MULTI_SEL;
			}
		}
	}
	else if (bEnable == false && IS_MULTI_SEL)
	{
		m_Style &= ~WNDLIST_ES_MULTI_SEL;
		m_uIsItemSelStatus = 0;
		if (m_puItemSelStatusList)
		{
			free(m_puItemSelStatusList);
			m_puItemSelStatusList = NULL;
		}
		m_uNumSelStatusList = 0;
	}
}

void KWndList::SetCustomColorOffset(int nOffset)
{
	if (nOffset >= 0)
		m_nColorOffset = nOffset;
	else
		m_nColorOffset = NO_CUSTOM_COLOR;
}

//--------------------------------------------------------------------------
//	功能：窗口函数
//--------------------------------------------------------------------------
int KWndList::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch(uMsg)
	{
	case WM_MOUSEMOVE://to be check
		if (m_Style & WNDLIST_ES_HIGHLIGHT_ENABLE)
			OnMouseMove(GET_X_LPARAM(nParam), GET_Y_LPARAM(nParam));
		break;
	case WM_LBUTTONDOWN:
		OnLButtonDown(GET_X_LPARAM(nParam), GET_Y_LPARAM(nParam));
		break;
	case WM_MOUSEWHEEL:
		{
			if (m_pScrollbar && !m_pScrollbar->IsDisable())
			{
				int zDelta = GET_Y_LPARAM(uParam);
				int nPos = m_pScrollbar->GetScrollPos();
				nPos += (-zDelta / WHEEL_DELTA);
				m_pScrollbar->SetScrollPos(nPos);
			}
		}
		break;
	case WM_RBUTTONDOWN:
		OnRButtonDown(GET_X_LPARAM(nParam), GET_Y_LPARAM(nParam));
		break;
	case WM_LBUTTONDBLCLK:
		OnLButtonDClick(GET_X_LPARAM(nParam), GET_Y_LPARAM(nParam));
		break;
	default:
		return KWndWindow::WndProc(uMsg, uParam, nParam);
	}
	return 0;
}

//获得的指定位置的选项的索引
int	KWndList::GetIemIndexAtPoint(int x, int y)
{
	int nSel = m_nTopItemIndex + (y - m_nAbsoluteTop) / (m_nFontSize + 1) * m_nNumColumn;
	if (m_nNumColumn > 1)
	{
		int nColumnWidth = m_Width / m_nNumColumn;
		if (nColumnWidth > 0)
			nSel += (x - m_nAbsoluteLeft) / nColumnWidth;
	}
	if (nSel >= m_nNumItem)
		nSel =  -1;
	return nSel;
}

//--------------------------------------------------------------------------
//	功能：响应鼠标左键在此按下
//--------------------------------------------------------------------------
void KWndList::OnLButtonDown(int x, int y)
{
	int nSel = GetIemIndexAtPoint(x, y);
	if (nSel != m_nSelItemIndex)
	{
		if (IS_MULTI_SEL == 0)
		{
			m_nSelItemIndex = nSel;
			if (m_pParentWnd)
			{
				m_pParentWnd->WndProc(WND_N_LIST_ITEM_SEL,
					(unsigned int)(KWndWindow*)this, m_nSelItemIndex);
			}
		}
		else if (nSel >= 0)
		{
			unsigned int uFlag = (1 << (nSel % 32));
			if (nSel < 32)
				m_uIsItemSelStatus ^= uFlag;
			else
				m_puItemSelStatusList[nSel / 32] ^= uFlag;
			if (m_pParentWnd)
			{
				m_pParentWnd->WndProc(WND_N_LIST_ITEM_SEL,
					(unsigned int)(KWndWindow*)this, nSel);
			}
		}
	}
}

//判断某项是否被选中
int	KWndList::IsItemSel(int nIndex)
{
	if (IS_MULTI_SEL == 0)
		return (m_nSelItemIndex == nIndex);
	unsigned int uFlag = (1 << (nIndex % 32));
	if (nIndex < 32)
		return (m_uIsItemSelStatus & uFlag);
	return (m_puItemSelStatusList[nIndex / 32] & uFlag);
}

//--------------------------------------------------------------------------
//	功能：响应鼠标右键在此按下
//--------------------------------------------------------------------------
void KWndList::OnRButtonDown(int x, int y)
{
	int nSel = GetIemIndexAtPoint(x, y);
	if (nSel >= 0 && m_pParentWnd)
	{
		m_pParentWnd->WndProc(WND_N_LIST_ITEM_R_CLICK,
			(unsigned int)(KWndWindow*)this, nSel);
	}
}

//响应鼠标左键double click
void KWndList::OnLButtonDClick(int x, int y)
{
	int nSel = GetIemIndexAtPoint(x, y);
	if (nSel >= 0 && m_pParentWnd)
	{
		m_pParentWnd->WndProc(WND_N_LIST_ITEM_D_CLICK,
			(unsigned int)(KWndWindow*)this, nSel);
	}
}

int KWndList::SetCurSel(int nSel)
{
	if (nSel < 0 || nSel >= m_nNumItem)
	{
		nSel = -1;
		if (IS_MULTI_SEL)
			return nSel;
	}

	if (IS_MULTI_SEL)
	{
		unsigned int uFlag = (1 << (nSel % 32));
		if (nSel < 32)
			m_uIsItemSelStatus |= uFlag;
		else
			m_puItemSelStatusList[nSel / 32] |= uFlag;
	}
	else if (nSel != m_nSelItemIndex)
	{
		m_nSelItemIndex = nSel;
		UpdateData();
		if (m_pParentWnd)
			m_pParentWnd->WndProc(WND_N_LIST_ITEM_SEL, (unsigned int)(KWndWindow*)this, m_nSelItemIndex);
	}
	return nSel;
}

void KWndList::UpdateData()
{
	int nVisibleItem = GetVisibleItemCount();
	if (m_nTopItemIndex > m_nSelItemIndex)
		m_nTopItemIndex = m_nSelItemIndex / m_nNumColumn * m_nNumColumn;
	else if(m_nTopItemIndex + nVisibleItem <= m_nSelItemIndex)
		m_nTopItemIndex = m_nNumColumn * ( m_nSelItemIndex / m_nNumColumn  + 1) - nVisibleItem;
	if (m_nTopItemIndex < 0)
		m_nTopItemIndex = 0;
}

//--------------------------------------------------------------------------
//	功能：响应鼠标移动
//--------------------------------------------------------------------------
void KWndList::OnMouseMove(int x, int y)
{
	int nIndex = GetIemIndexAtPoint(x, y);
	if (nIndex != m_nHighLightItemIndex)
	{
		m_nHighLightItemIndex = nIndex;
		if (m_pParentWnd)
		{
			m_pParentWnd->WndProc(WND_N_LIST_ITEM_HIGHLIGHT, (unsigned int)(KWndWindow*)this, m_nHighLightItemIndex);
		}
	}
}

//--------------------------------------------------------------------------
//	功能：绘制窗口
//--------------------------------------------------------------------------
void KWndList::PaintWindow()
{
	KWndWindow::PaintWindow();
	if (g_pRepresentShell)
	{
		int nVisibleItem = GetVisibleItemCount();
		int EndIndex = m_nTopItemIndex + nVisibleItem;
		if (EndIndex > m_nNumItem)
			EndIndex = m_nNumItem;
		char	szBuffer[128];

		unsigned int uColor;
		unsigned int uBorderColor;
		BYTE*	pItem = m_pContent + m_nTopItemIndex * m_nItemSize;
		int		i, nColumn, x, y, nColumnWidth;
		x = m_nAbsoluteLeft;
		y = m_nAbsoluteTop;
		nColumnWidth = m_Width / m_nNumColumn;
		int	nMaxLen = (nColumnWidth * 2) / m_nFontSize + 1;
		if (nMaxLen > sizeof(szBuffer))
			nMaxLen = sizeof(szBuffer);
		for (i = m_nTopItemIndex, nColumn = 0; i < EndIndex; i++, nColumn++)
		{
			if (nColumn == m_nNumColumn)
			{
				nColumn = 0;
				x = m_nAbsoluteLeft;
				y += m_nFontSize + 1;
			}
			if (IsItemSel(i))
			{
				uColor = m_SelItemColor;
				uBorderColor = m_SelItemBorderColor;
				if (m_uSelItemBgColor != NO_BG_COLOR)
				{
					KRUShadow	Shadow;
					Shadow.Color.Color_dw = m_uSelItemBgColor;
					Shadow.oPosition.nX = x;
					Shadow.oPosition.nY = y;
					Shadow.oPosition.nZ = 0;
					Shadow.oEndPos.nX = x + nColumnWidth;
					Shadow.oEndPos.nY = y + m_nFontSize + 1;
					g_pRepresentShell->DrawPrimitives(1, &Shadow, RU_T_SHADOW, true);
				}
			}
			else if ((m_nColorOffset != NO_CUSTOM_COLOR) &&
					(*(int*)(&pItem[m_nColorOffset])))
			{
				uColor = *(int*)(&pItem[m_nColorOffset]);
				uBorderColor = m_ItemBorderColor;
			}
			else if (i == m_nHighLightItemIndex)
			{
				uColor = m_HighLightColor;
				uBorderColor = m_HighLightBorderColor;
			}
			else
			{
				uColor = m_ItemColor;
				uBorderColor = m_ItemBorderColor;
			}

			const char* pShowString = TGetLimitLenString((const char*)(&pItem[m_nStringOffset]), -1, szBuffer, nMaxLen);

			int nLen = strlen(pShowString);
			int nPaintX;
			if (m_Style & WNDLIST_ES_HALIGN_CENTRE)
				nPaintX = x + (nColumnWidth - nLen * m_nFontSize / 2) / 2;
			else if (m_Style & WNDLIST_ES_HALIGN_RIGHT)
				nPaintX = x + nColumnWidth - nLen * m_nFontSize / 2;
			else
				nPaintX = x;

			g_pRepresentShell->OutputText(m_nFontSize, pShowString, nLen, nPaintX, y,
				uColor, 0, TEXT_IN_SINGLE_PLANE_COORD, uBorderColor);

			pItem += m_nItemSize;
			x += nColumnWidth;
		}
	}
}

//--------------------------------------------------------------------------
//	功能：设置列表内容
//--------------------------------------------------------------------------
void KWndList::SetContent(BYTE* pContent, int nNumItem, int nItemSize, int nStringOffset)
{
	bool bRestoreMultiSel = false;
	if (IS_MULTI_SEL)
	{
		bRestoreMultiSel = true;
		EnableMultiSel(false);
	}
	m_pContent = pContent;
	m_nSelItemIndex       = -1;
	m_nHighLightItemIndex = -1;
	m_nTopItemIndex = 0;
	if (m_pContent)
	{
		m_nNumItem = nNumItem;
		m_nItemSize = nItemSize;
		m_nStringOffset = nStringOffset;
	}
	else
	{
		m_nNumItem		= 0;
		m_nItemSize		= 0;
		m_nStringOffset = 0;
	}
	if (m_pParentWnd)
	{
		if (m_Style & WNDLIST_ES_HIGHLIGHT_ENABLE)
			m_pParentWnd->WndProc(WND_N_LIST_ITEM_HIGHLIGHT, (unsigned int)(KWndWindow*)this, m_nHighLightItemIndex);
		m_pParentWnd->WndProc(WND_N_LIST_ITEM_SEL, (unsigned int)(KWndWindow*)this, m_nSelItemIndex);
	}
	if (bRestoreMultiSel)
		EnableMultiSel(true);
	if (m_pScrollbar)
	{
		if (m_nNumItem > GetVisibleItemCount())
		{
			m_pScrollbar->Enable(true);
			m_pScrollbar->SetValueRange(0, m_nNumItem - GetVisibleItemCount());
		}
		else
			m_pScrollbar->Enable(false);
	}
}

//--------------------------------------------------------------------------
//	功能：查找列表（显示）字串
//--------------------------------------------------------------------------
int	KWndList::FindString(int nPrecedingStart, const char* pString)
{
	if (pString)
	{
		BYTE*	pItem = m_pContent;
		for (int i = nPrecedingStart + 1; i < m_nNumItem; i++, pItem += m_nItemSize)
		{
			if (strcmp((const char*)&pItem[m_nStringOffset], pString) == 0)
				return i;
		}
	}
	return -1;
}

//--------------------------------------------------------------------------
//	功能：获得列表项数据中的一个整数
//--------------------------------------------------------------------------
int KWndList::GetItemDataInt(int nItemIndex, int nDataOffset)
{
	if (nItemIndex >= 0 && nItemIndex < m_nNumItem &&
		nDataOffset >= 0 && ((int)(nDataOffset + sizeof(int)) <= m_nItemSize))
	{
		return (*(int*)&m_pContent[m_nItemSize * nItemIndex + nDataOffset]);
	}
	return 0;
}

//--------------------------------------------------------------------------
//	功能：获得列表项的(显示)字串
//--------------------------------------------------------------------------
void KWndList::GetString(int nItemIndex, char* pBuffer)
{
	if (pBuffer)
	{
		if (nItemIndex >= 0 && nItemIndex < m_nNumItem)
		{
			strcpy(pBuffer, (char*)&m_pContent[m_nItemSize * nItemIndex + m_nStringOffset]);
		}
		else
			pBuffer[0] = 0;
	}
}

//--------------------------------------------------------------------------
//	功能：获得列表项的(显示)字串的长度
//--------------------------------------------------------------------------
int KWndList::GetStrignLen(int nItemIndex)
{
	if (nItemIndex >= 0 && nItemIndex < m_nNumItem)
	{
		return strlen((char*)&m_pContent[m_nItemSize * nItemIndex + m_nStringOffset]);
	}
	return 0;
}

//--------------------------------------------------------------------------
//	功能：设置被显示的最顶列表项的索引值
//--------------------------------------------------------------------------
void KWndList::SetTopItemIndex(int nTopItemIndex)
{
	if (nTopItemIndex >= 0 && nTopItemIndex < m_nNumItem)
		m_nTopItemIndex = nTopItemIndex;
}

//--------------------------------------------------------------------------
//	功能：获得列表框可以同时显示的项的数目
//--------------------------------------------------------------------------
int	KWndList::GetVisibleItemCount() const
{
	return (m_Height / (m_nFontSize + 1)) * m_nNumColumn;
}