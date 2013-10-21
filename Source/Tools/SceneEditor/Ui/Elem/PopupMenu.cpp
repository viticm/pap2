/*****************************************************************************************
//	弹出选择菜单
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-2-21
*****************************************************************************************/
#include "../../Public/KWin32.h"

#include <Windows.h>
#include <string.h>
#include "../../Public/KEngine.h"
#include "../../Public/KiniFile.h"
#include "PopupMenu.h"
#include "AutoLocateWnd.h"
#include "WndWindow.h"
#include "WndMessage.h"
#include "Wnds.h"
#include "SpecialFuncs.h"
//#include "../../Public/iRepresentShell.h"
//#include "../../Public/KRepresentUnit.h"
//extern iRepresentShell*	g_pRepresentShell;

#include "../../Public/Text.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


KPopupMenuData* KPopupMenu::m_pMenu = NULL;
KWndWindow*		KPopupMenu::m_pCaller = NULL;
unsigned int	KPopupMenu::m_uCallerParam = 0;
short			KPopupMenu::m_nImgWidth = 0;
short			KPopupMenu::m_nImgHeight = 0;
short			KPopupMenu::m_nIndent = 0;
short			KPopupMenu::m_nFrame = 0;
int				KPopupMenu::m_nMenuHeight = 0;
char			KPopupMenu::m_szImage[128] = "";
unsigned int	KPopupMenu::m_uDefBorderLineColor = 0xff000000;	//菜单的变框颜色，为0则无
unsigned int	KPopupMenu::m_uDefTextColor		 = 0xffffffff;	//菜单项的文字色
unsigned int	KPopupMenu::m_uDefTextBorderColor= 0xff000000;	//菜单项的文字边缘色
unsigned int	KPopupMenu::m_uDefSelTextColor	 = 0xffe0e0e0;	//被选中菜单项的文字色
unsigned int	KPopupMenu::m_uDefSelTextBorderColor = 0xff000000;	//被选中菜单项的文字边缘色
unsigned int	KPopupMenu::m_uDefItemBgColor	 = 0x0a404040;	//菜单项的背景色
unsigned int	KPopupMenu::m_uDefSelItemBgColor = 0x0a202020;	//被选中菜单项的背景色


#define	SCHEME_INI				"弹出说明文字.ini"

void KPopupMenu::OnWndDelete(KWndWindow* pWnd)
{
	if (pWnd == m_pCaller)
		Cancel();
}

void KPopupMenu::Popup(KPopupMenuData* pMenu, KWndWindow* pCaller, unsigned int uParam)
{
	Cancel();

	if ((m_pMenu = pMenu) == NULL)
	{
		m_pCaller = NULL;
		return;
	}

	if (m_pMenu->nNumItem <= 0)
	{
		Cancel();
		return;
	}

	m_pCaller = pCaller;
	m_uCallerParam = uParam;

	//----处理头尾图形----
	if (m_szImage[0] == 0 || m_nImgWidth == 0)
		m_pMenu->usMenuFlag &= ~PM_F_HAVE_HEAD_TAIL_IMG;

	//----处理文字缩进----
	if (m_pMenu->nItemTitleIndent == MENU_ITEM_DEFAULT_INDENT)
		m_pMenu->nItemTitleIndent = m_nIndent;

	bool bCalcRightWidth = false;
	if (m_pMenu->nItemRightWidth < 0)
	{
		if ((m_pMenu->usMenuFlag & PM_F_TAB_SPLIT_ITEM_TEXT) == 0)
			m_pMenu->nItemRightWidth = 0;
		else
			bCalcRightWidth = true;
	}

	//----处理菜单横向宽度----
	//----处理菜单项的纵向高度----
	int nLenWid, nMaxItemWid = 0, nLenHei, nMaxItemHei = 0, nMaxRightLen = 0;
	for (int i = 0; i < m_pMenu->nNumItem; i++)
	{
		nLenHei = TGetEncodedTextLineCount(m_pMenu->Items[i].szData,
			m_pMenu->Items[i].uDataLen, -1, nLenWid, m_pMenu->byFontSize, 0, 1);
		if (nMaxItemWid < nLenWid)
			nMaxItemWid = nLenWid;
		if (nMaxItemHei < nLenHei)
			nMaxItemHei = nLenHei;

		if (bCalcRightWidth)
		{
			int	nTabPos = TFindSpecialCtrlInEncodedText(m_pMenu->Items[i].szData,
				m_pMenu->Items[i].uDataLen, 0, KTC_TAB);
			if (nTabPos >= 0)
			{
				TGetEncodedTextLineCount(m_pMenu->Items[i].szData + nTabPos + 1,
					m_pMenu->Items[i].uDataLen - nTabPos - 1, 0, nLenWid, m_pMenu->byFontSize);
				if (nMaxRightLen < nLenWid)
					nMaxRightLen = nLenWid;
			}
		}
	}

	if (bCalcRightWidth)
	{
		m_pMenu->nItemRightWidth = nMaxRightLen * m_pMenu->byFontSize / 2;
	}

	if (m_pMenu->nItemWidth == MENU_ITEM_DEFAULT_WIDTH)
	{
		if ((m_pMenu->usMenuFlag & PM_F_TAB_SPLIT_ITEM_TEXT) == 0)
		{
			m_pMenu->nItemWidth = m_pMenu->byFontSize * nMaxItemWid / 2 +
				m_pMenu->nItemTitleIndent * 2 + m_pMenu->nItemRightWidth;
		}
		else
		{
			m_pMenu->nItemWidth = m_pMenu->byFontSize * (nMaxItemWid + 3) / 2 +
				m_pMenu->nItemTitleIndent * 2;
		}
	}

	if (m_pMenu->nItemHeight == MENU_ITEM_DEFAULT_HEIGHT)
	{
		m_pMenu->nItemHeight = m_pMenu->byFontSize * nMaxItemHei + 2 * m_pMenu->byItemTitleUpSpace;
		if (m_pMenu->usMenuFlag & PM_F_HAVE_ITEM_SEPARATOR)
			m_pMenu->nItemHeight += 1;
	}

	if (m_pMenu->nItemHeight < 1)
		m_pMenu->nItemHeight = 1;

	//----调整选中菜单项----
	if (m_pMenu->nSelectedItem >= m_pMenu->nNumItem)
		m_pMenu->nSelectedItem = -1;

	//----计算整个菜单的纵向高度----
	m_nMenuHeight = m_pMenu->nItemHeight * m_pMenu->nNumItem;

	if (m_pMenu->usMenuFlag & PM_F_HAVE_ITEM_SEPARATOR)
		m_nMenuHeight--;

	if (m_pMenu->usMenuFlag & PM_F_HAVE_HEAD_TAIL_IMG)
		m_nMenuHeight += 2 * m_nImgHeight;


	//----计算菜单窗口的位置----
	int x, y;
	if (m_pMenu->nX == MENU_DEFAULT_POS)
	{
		//ALW_GetWndPosition(x, y, m_pMenu->nItemWidth, m_nMenuHeight, false, true);
	}
	else
	{
		x = m_pMenu->nX + m_pMenu->nItemWidth / 2;
		y = m_pMenu->nY;
		//ALW_GetWndPosition(x, y, m_pMenu->nItemWidth, m_nMenuHeight, true, true);
	}

	m_pMenu->nXX = m_pMenu->nX;
	m_pMenu->nYY = m_pMenu->nY;

	m_pMenu->nX = x;
	m_pMenu->nY = y;
}

void KPopupMenu::PaintMenu()
{
	/*if (m_pMenu == 0)
		return;
	KRUShadow	Shadow;
	Shadow.oPosition.nX = m_pMenu->nX;
	Shadow.oEndPos.nX = Shadow.oPosition.nX + m_pMenu->nItemWidth;
	Shadow.oPosition.nY = m_pMenu->nY;
	Shadow.oEndPos.nY = Shadow.oPosition.nY + m_nMenuHeight;
	Shadow.oPosition.nZ = 0;

	//绘制上下边的图形
	if (m_pMenu->usMenuFlag & PM_F_HAVE_HEAD_TAIL_IMG)
	{
		Shadow.oPosition.nY += m_nImgHeight;
		Shadow.oEndPos.nY -= m_nImgHeight;

		KRUImagePart	Image;
		Image.nType = ISI_T_SPR;
		Image.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
		Image.uImage = 0;
		Image.nISPosition = 0;
		strcpy(Image.szImage, m_szImage);
		Image.Color.Color_b.a = 255;
		Image.bRenderFlag = 0;
		Image.nFrame = m_nFrame;

		Image.oPosition.nX = m_pMenu->nX;
		Image.oEndPos.nX = Image.oPosition.nX + m_nImgWidth;
		Image.oPosition.nZ = Image.oEndPos.nZ = 0;
		while(Image.oEndPos.nX < Shadow.oEndPos.nX)
		{
			Image.oPosition.nY = m_pMenu->nY;
			Image.oEndPos.nY = Image.oPosition.nY + m_nImgHeight;
			g_pRepresentShell->DrawPrimitives(1, &Image, RU_T_IMAGE, true);
			Image.oEndPos.nY = m_pMenu->nY + m_nMenuHeight;
			Image.oPosition.nY = Image.oEndPos.nY - m_nImgHeight;
			g_pRepresentShell->DrawPrimitives(1, &Image, RU_T_IMAGE, true);
			Image.oPosition.nX += m_nImgWidth;
			Image.oEndPos.nX += m_nImgWidth;
		};
		if (Image.oPosition.nX < Shadow.oEndPos.nX)
		{
			Image.oEndPos.nX = Shadow.oEndPos.nX;
			Image.oImgLTPos.nX = 0;
			Image.oImgLTPos.nY = 0;
			Image.oImgRBPos.nX = Image.oEndPos.nX - Image.oPosition.nX;
			Image.oImgRBPos.nY = m_nImgHeight;
			Image.oPosition.nY = m_pMenu->nY;
			Image.oEndPos.nY = Image.oPosition.nY + m_nImgHeight;
			g_pRepresentShell->DrawPrimitives(1, &Image, RU_T_IMAGE_PART, true);
			Image.oEndPos.nY = m_pMenu->nY + m_nMenuHeight;
			Image.oPosition.nY = Image.oEndPos.nY - m_nImgHeight;
			g_pRepresentShell->DrawPrimitives(1, &Image, RU_T_IMAGE_PART, true);
		}
	}

	KRULine	Lines[2];
	if (m_pMenu->uBorderLineColor)
	{
		//绘制左右两侧的边线
		Lines[0].Color.Color_dw = m_pMenu->uBorderLineColor;
		Lines[0].oPosition.nX = Shadow.oPosition.nX - 1;
		Lines[0].oPosition.nY = m_pMenu->nY;
		Lines[0].oEndPos.nX = Lines[0].oPosition.nX;
		Lines[0].oEndPos.nY = m_pMenu->nY + m_nMenuHeight;
		Lines[0].oEndPos.nZ = Lines[0].oPosition.nZ = 0;

		Lines[1].Color.Color_dw = Lines[0].Color.Color_dw;
		Lines[1].oPosition.nX = Shadow.oEndPos.nX;
		Lines[1].oPosition.nY = Lines[0].oPosition.nY;
		Lines[1].oEndPos.nX   = Lines[1].oPosition.nX;
		Lines[1].oEndPos.nY   = Lines[0].oEndPos.nY;
		Lines[1].oEndPos.nZ   = Lines[1].oPosition.nZ = 0;

		g_pRepresentShell->DrawPrimitives(2, &Lines[0], RU_T_LINE, true);
	}

	if (m_pMenu->usMenuFlag & PM_F_HAVE_ITEM_SEPARATOR)
	{
		Lines[0].oPosition.nX = Shadow.oPosition.nX;
		Lines[0].oEndPos.nX = Shadow.oEndPos.nX;
		Lines[0].oEndPos.nZ = Lines[0].oPosition.nZ = 0;
		Lines[0].Color.Color_dw = m_pMenu->uBorderLineColor;
	}

	//绘制非选中的菜单项
	int nTextStartX = Shadow.oPosition.nX + m_pMenu->nItemTitleIndent;

	for (char i = 0; i < m_pMenu->nNumItem; i++)
	{
		Shadow.oEndPos.nY = Shadow.oPosition.nY + m_pMenu->nItemHeight;
		if (m_pMenu->usMenuFlag & PM_F_HAVE_ITEM_SEPARATOR)
		{
			Shadow.oEndPos.nY --;
			Lines[0].oEndPos.nY = Lines[0].oPosition.nY = Shadow.oPosition.nY - 1;
			g_pRepresentShell->DrawPrimitives(1, &Lines[0], RU_T_LINE, true);
		}
		if (i != m_pMenu->nSelectedItem)
		{
			Shadow.Color.Color_dw = m_pMenu->Items[i].uBgColor;
			g_pRepresentShell->DrawPrimitives(1, &Shadow, RU_T_SHADOW, true);
			if (m_pMenu->uTextColor)
			{
				KOutputTextParam param;
				param.nX = nTextStartX;
				param.nY = Shadow.oPosition.nY + m_pMenu->byItemTitleUpSpace;
				param.nZ = TEXT_IN_SINGLE_PLANE_COORD;
				param.nSkipLine = 0;
				param.nNumLine = m_pMenu->nItemHeight / m_pMenu->byFontSize + 1;
				param.Color = m_pMenu->uTextColor;
				param.BorderColor = m_pMenu->uTextBorderColor;
				param.nVertAlign = 1;
				if ((m_pMenu->usMenuFlag & PM_F_TAB_SPLIT_ITEM_TEXT) == 0)
				{
					g_pRepresentShell->OutputRichText(m_pMenu->byFontSize, &param,
						m_pMenu->Items[i].szData, m_pMenu->Items[i].uDataLen);
				}
				else
				{
					OutputTabSplitText(m_pMenu->Items[i].szData, m_pMenu->Items[i].uDataLen,
						m_pMenu->nItemWidth, m_pMenu->byFontSize, &param);
				}
			}
		}
		Shadow.oPosition.nY += m_pMenu->nItemHeight;
	}
	if (m_pMenu->usMenuFlag & PM_F_HAVE_ITEM_SEPARATOR)
	{
		Lines[0].oEndPos.nY = Lines[0].oPosition.nY = Shadow.oPosition.nY - 1;
		g_pRepresentShell->DrawPrimitives(1, &Lines[0], RU_T_LINE, true);
	}

	//绘制选中的菜单项
	if (m_pMenu->nSelectedItem >= 0)
	{
		Shadow.oPosition.nY -= m_pMenu->nItemHeight * (m_pMenu->nNumItem - m_pMenu->nSelectedItem);
		Shadow.oEndPos.nY = Shadow.oPosition.nY + m_pMenu->nItemHeight;
		if (m_pMenu->uSelItemBgColor)
			Shadow.Color.Color_dw = m_pMenu->uSelItemBgColor;
		else
			Shadow.Color.Color_dw = m_pMenu->Items[m_pMenu->nSelectedItem].uBgColor;

		if (m_pMenu->usMenuFlag & PM_F_ZOOM_SELECTED_ITEM)
		{
			Shadow.oPosition.nX -= 4;
			Shadow.oEndPos.nX += 4;
			Shadow.oPosition.nY -= 4;
			if (m_pMenu->usMenuFlag & PM_F_HAVE_ITEM_SEPARATOR)
				Shadow.oEndPos.nY += 3;
			else
				Shadow.oEndPos.nY += 4;
		}
		g_pRepresentShell->DrawPrimitives(1, &Shadow, RU_T_SHADOW, true);

		KOutputTextParam param;
		if (m_pMenu->uSelTextColor)
		{
			param.Color = m_pMenu->uSelTextColor;
			param.BorderColor = m_pMenu->uSelTextBorderColor;
		}
		else
		{
			param.Color = m_pMenu->uTextColor;
			param.BorderColor = m_pMenu->uTextBorderColor;
		}
		if (param.Color)
		{
			if (m_pMenu->usMenuFlag & PM_F_ZOOM_SELECTED_ITEM)
				Shadow.oPosition.nY += 4;
			param.nX = nTextStartX;
			param.nY = Shadow.oPosition.nY + m_pMenu->byItemTitleUpSpace;
			param.nZ = TEXT_IN_SINGLE_PLANE_COORD;
			param.nSkipLine = 0;
			param.nNumLine = m_pMenu->nItemHeight / m_pMenu->byFontSize + 1;
			param.nVertAlign = 1;

			if ((m_pMenu->usMenuFlag & PM_F_TAB_SPLIT_ITEM_TEXT) == 0)
			{
				g_pRepresentShell->OutputRichText(m_pMenu->byFontSize, &param,
					m_pMenu->Items[m_pMenu->nSelectedItem].szData,
					m_pMenu->Items[m_pMenu->nSelectedItem].uDataLen);
			}
			else
			{
				OutputTabSplitText(m_pMenu->Items[m_pMenu->nSelectedItem].szData,
					m_pMenu->Items[m_pMenu->nSelectedItem].uDataLen,
					m_pMenu->nItemWidth, m_pMenu->byFontSize, &param);
			}
		}

		if (m_pMenu->uBorderLineColor)
		{
			KRURect		Rect;
			Rect.Color.Color_dw = m_pMenu->uBorderLineColor;
			Rect.oPosition.nX = Shadow.oPosition.nX - 1;
			Rect.oEndPos.nX = Shadow.oEndPos.nX;
			Rect.oPosition.nY = Shadow.oPosition.nY - 1;
			Rect.oEndPos.nY = Shadow.oEndPos.nY;
			if (m_pMenu->usMenuFlag & PM_F_HAVE_ITEM_SEPARATOR)
				Rect.oEndPos.nY--;
			g_pRepresentShell->DrawPrimitives(1, &Rect, RU_T_RECT, true);
		}
	}*/
}

//窗口函数
int	KPopupMenu::HandleInput(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	if (m_pMenu)
	{
		switch(uMsg)
		{
		case WM_KEYDOWN:
			if (uParam == VK_ESCAPE)
			{
				Cancel();
				nRet = 1;
			}
			else if (uParam == VK_DOWN)
			{
				nRet = SelectItem(m_pMenu->nSelectedItem + 1, false);
			}
			else if (uParam == VK_UP)
			{
				nRet = SelectItem(m_pMenu->nSelectedItem - 1, false);
			}
			else if (uParam == VK_RETURN)
			{
				nRet = SelectItem(m_pMenu->nSelectedItem, true);
			}
			break;
		default:
			if ((uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST) || uMsg == WM_MOUSEHOVER)
			{
				if (uMsg == WM_MOUSEMOVE)
					SelectItem(GET_X_LPARAM(nParam), GET_Y_LPARAM(nParam), false);
				else if (uMsg == WM_LBUTTONUP)
					SelectItem(GET_X_LPARAM(nParam), GET_Y_LPARAM(nParam), true);
				nRet = true;
			}
			break;
		}
	}
	return nRet;
}

bool KPopupMenu::SelectItem(int nIndex, bool bApply)
{
	bool bRet = false;

	if (nIndex < 0 || nIndex >= m_pMenu->nNumItem)
		return bRet;

	unsigned int nMID = WND_M_MENUITEM_SELECTED;
	m_pMenu->nSelectedItem = nIndex;
	bRet = true;

	if (bApply)
	{
		KPopupMenuData* pMenu = m_pMenu;
		short nID = -1;
		if (pMenu->nSelectedItem >= 0)
			nID = pMenu->Items[pMenu->nSelectedItem].uID;

		KWndWindow* pCaller = m_pCaller;
		unsigned int uCallerParam = m_uCallerParam;
		if (!(m_pMenu && (m_pMenu->usMenuFlag & PM_F_CANCEL_BY_CALLER)))
			Cancel();

		if (pCaller)
		{
			pCaller->WndProc(nMID, (DWORD)pCaller, MAKELONG(nID, uCallerParam));
		}
	}
	return bRet;
}

bool KPopupMenu::SelectItem(int x, int y, bool bApply)
{
	bool bRet = false;
	unsigned int nMID = WND_M_MENUITEM_SELECTED;
	m_pMenu->nSelectedItem = -1;
	if (x >= m_pMenu->nX && y >= m_pMenu->nY &&
		x < m_pMenu->nX + m_pMenu->nItemWidth &&
		y < m_pMenu->nY + m_nMenuHeight)
	{
		if (m_pMenu->usMenuFlag & PM_F_HAVE_HEAD_TAIL_IMG)
			y -= m_nImgHeight;
		if (y > m_pMenu->nY)
		{
			m_pMenu->nSelectedItem = (y - m_pMenu->nY) / m_pMenu->nItemHeight;
			if (m_pMenu->nSelectedItem >= m_pMenu->nNumItem)
				m_pMenu->nSelectedItem = -1;
		}

		if (m_pMenu->nItemRightWidth > 0)
		{
			if ((x - m_pMenu->nX) >= m_pMenu->nItemWidth - m_pMenu->nItemRightWidth)
				nMID = WND_M_MENUITEMRIGHT_SELECTED;
		}

		bRet = true;
	}
	if (bApply)
	{
		KPopupMenuData* pMenu = m_pMenu;
		short nID = -1;
		if (pMenu->nSelectedItem >= 0)
			nID = pMenu->Items[pMenu->nSelectedItem].uID;

		KWndWindow* pCaller = m_pCaller;
		unsigned int uCallerParam = m_uCallerParam;
		if (!(m_pMenu && (m_pMenu->usMenuFlag & PM_F_CANCEL_BY_CALLER)))
			Cancel();

		if (pCaller)
		{
			pCaller->WndProc(nMID, (DWORD)pCaller, MAKELONG(nID, uCallerParam));
		}
	}
	return bRet;
}

void KPopupMenu::Cancel()
{
	if (m_pMenu && (m_pMenu->usMenuFlag & PM_F_AUTO_DEL_WHEN_HIDE))
	{
		free(m_pMenu);
	}
	m_pMenu = NULL;
	m_pCaller = NULL;
}

void KPopupMenu::InitMenuData(KPopupMenuData* pMenu, int nNumItem, int nFontSize)
{
	if (pMenu == NULL || nNumItem < 1)
		return;

	pMenu->usMenuFlag = 0;

	if (m_uDefBorderLineColor)
		pMenu->usMenuFlag |= PM_F_HAVE_ITEM_SEPARATOR;

	pMenu->byFontSize = nFontSize;
	pMenu->nItemHeight = MENU_ITEM_DEFAULT_HEIGHT;
	pMenu->nItemTitleIndent = MENU_ITEM_DEFAULT_INDENT;
	pMenu->byItemTitleUpSpace = MENU_ITEM_DEFAULT_TITLEUPSPACE;
	pMenu->nItemWidth = MENU_ITEM_DEFAULT_WIDTH;
	pMenu->nItemRightWidth = 0;
	pMenu->nNumItem = nNumItem;
	pMenu->nSelectedItem = -1;

	pMenu->nX = MENU_DEFAULT_POS;
	pMenu->nY = MENU_DEFAULT_POS;
	pMenu->nXX = MENU_DEFAULT_POS;
	pMenu->nYY = MENU_DEFAULT_POS;

	pMenu->uBorderLineColor = m_uDefBorderLineColor;
	pMenu->uSelItemBgColor = m_uDefSelItemBgColor;
	pMenu->uSelTextColor   = m_uDefSelTextColor;
	pMenu->uSelTextBorderColor   = m_uDefSelTextBorderColor;
	pMenu->uTextColor      = m_uDefTextColor;
	pMenu->uTextBorderColor      = m_uDefTextBorderColor;
	for (int i = 0; i < nNumItem; i++)
	{
		pMenu->Items[i].szData[0] = 0;
		pMenu->Items[i].uDataLen = 0;
		pMenu->Items[i].uBgColor = m_uDefItemBgColor;
		pMenu->Items[i].uID = i;
	}
}

//载入界面方案
void KPopupMenu::LoadTheme(const char* pScheme)
{
	if (pScheme == NULL)
		return;
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
	if (Ini.Load(Buff))
	{
		int nValue;
		Ini.GetInteger("Main", "ImgWidth",  0, &nValue);
		if(nValue >= 0)
			m_nImgWidth = nValue;
		else
			m_nImgWidth = 0;
		Ini.GetInteger("Main", "ImgHeight", 0, &nValue);
		if(nValue >= 0)
			m_nImgHeight = nValue;
		else
			m_nImgHeight = 0;
		Ini.GetInteger("Main", "Indent",    0, &nValue);
		m_nIndent = nValue;
		Ini.GetInteger("Main", "Frame", 0, &nValue);
		m_nFrame = nValue;
		Ini.GetString("Main", "Image", "" , m_szImage, sizeof(m_szImage));

		if (Ini.GetString("Menu", "BorderLineColor", "0,0,0", Buff, sizeof(Buff)))
			m_uDefBorderLineColor = GetColor(Buff);
		else
			m_uDefBorderLineColor = 0;
		if (Ini.GetString("Menu", "ItemBgColor", "0,0,0", Buff, sizeof(Buff)))
			m_uDefItemBgColor =  (GetColor(Buff) & 0xffffff) | 0x0a000000;
		else
			m_uDefItemBgColor = 0;
		if (Ini.GetString("Menu", "SelItemBgColor", "0,0,0", Buff, sizeof(Buff)))
			m_uDefSelItemBgColor =  (GetColor(Buff) & 0xffffff) | 0x0a000000;
		else
			m_uDefSelItemBgColor = 0;
		if (Ini.GetString("Menu", "TextColor", "0,0,0", Buff, sizeof(Buff)))
			m_uDefTextColor =  GetColor(Buff);
		else
			m_uDefTextColor = 0;
		if (Ini.GetString("Menu", "TextBorderColor", "0,0,0", Buff, sizeof(Buff)))
			m_uDefTextBorderColor =  GetColor(Buff);
		else
			m_uDefTextBorderColor = 0;
		if (Ini.GetString("Menu", "SelTextColor", "0,0,0", Buff, sizeof(Buff)))
			m_uDefSelTextColor =  GetColor(Buff);
		else
			m_uDefSelTextColor = 0;
		if (Ini.GetString("Menu", "SelTextBorderColor", "0,0,0", Buff, sizeof(Buff)))
			m_uDefSelTextBorderColor =  GetColor(Buff);
		else
			m_uDefSelTextBorderColor = 0;
	}
}
