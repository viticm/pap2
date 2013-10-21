/*****************************************************************************************
//	浮动提示窗口
//	Copyright : Kingsoft 2002-2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-12-23
*****************************************************************************************/
#include "../../Public/KWin32.h"
#include "../../Public/KIniFile.h"
#include "MouseHover.h"
#include "AutoLocateWnd.h"
#include "WndWindow.h"
#include "../UiBase.h"
//#include "../../Public/iRepresentShell.h"
#include "../../Public/Text.h"
#include "SpecialFuncs.h"
//extern iRepresentShell*	g_pRepresentShell;

//#include "../../Public/CoreShell.h"
//extern iCoreShell*	g_pCoreShell;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


KMouseOver	g_MouseOver;

#define	SCHEME_INI				"弹出说明文字.ini"
#define	INFO_MIN_LEN			26
#define FOLLOW_CURSOR_OFFSET_X	16
#define FOLLOW_CURSOR_OFFSET_Y	8

static unsigned int	s_uHoverObjDestTextColor = 0xffffffff;	//对象提示文字的颜色

void SetHoverObjDescColor(unsigned int uColor)
{
	s_uHoverObjDestTextColor = uColor;
}

void SetMouseHoverObjectDesc(void* pWnd, int nObj, unsigned int uGenre,
			unsigned int uId, int nContainer, int x, int y)
{
	//KGameObjDesc	Desc;
	int		nLenTitle = 0, nLenProp = 0, nLenDesc = 0;

	g_MouseOver.CancelMouseHoverInfo();
	/*if (g_pCoreShell)
	{
		KUiObjAtContRegion	Obj;
		Obj.Obj.uGenre = uGenre;
		Obj.Obj.uId = uId;
		Obj.Region.h = ((KWndWindow *)pWnd)->GetWindowLong(KWndWindow::enumWLI_ID);
		Obj.Region.v = ((KWndWindow *)pWnd)->GetWindowLong(KWndWindow::enumWLI_USERDATA);
		Obj.Region.Width = Obj.Region.Height = 0;
		Obj.nContainer = nContainer;
		Desc.szDesc[0] = 0;
		Desc.szProp[0] = 0;
		Desc.szTitle[0] = 0;
		unsigned uIndex = GDI_GAME_OBJ_DESC;
		if (g_UiBase.GetStatus() & UIS_S_TRADE_REPAIR)
			uIndex = GDI_GAME_OBJ_DESC_INCLUDE_REPAIRINFO;
		else if (g_UiBase.GetStatus() & UIS_S_STALL_SETPRICE || g_pCoreShell->GetGameData(GDI_IS_STALL, -1, -1) == 1)
			uIndex = GDI_GAME_OBJ_DESC_INCLUDE_STALLINFO;
		else if (g_UiBase.GetStatus())
			uIndex = GDI_GAME_OBJ_DESC_INCLUDE_TRADEINFO;

		g_pCoreShell->GetGameData(uIndex, (unsigned int)&Obj, (int)&Desc);
		g_MouseOver.SetMouseHoverInfo(pWnd, nObj, x, y, false, false);
		if (Desc.szTitle[0])
		{
			nLenTitle = TEncodeText(Desc.szTitle, strlen(Desc.szTitle));
			g_MouseOver.SetMouseHoverTitle(Desc.szTitle, nLenTitle,s_uHoverObjDestTextColor);
		}
		if (Desc.szProp[0])
		{
			nLenProp = TEncodeText(Desc.szProp, strlen(Desc.szProp));
			g_MouseOver.SetMouseHoverProp(Desc.szProp, nLenProp, s_uHoverObjDestTextColor);
		}
		if (Desc.szDesc[0])
		{
			nLenDesc = TEncodeText(Desc.szDesc, strlen(Desc.szDesc));
			g_MouseOver.SetMouseHoverDesc(Desc.szDesc, nLenDesc, s_uHoverObjDestTextColor);
		}
	}*/
}

int DrawDraggingGameObjFunc(int x, int y, const KUiDraggedObject& Obj, int nDropQueryResult)
{
	//g_pCoreShell->DrawGameObj(Obj.uGenre, Obj.uId, x, y, 0, 0, 0);
	return false;
}

KMouseOver::KMouseOver()
{
	m_nImgWidth  = 0;
	m_nImgHeight = 0;
	m_nLeft      = 0;
	m_nTop       = 0;
	m_nWndWidth  = 0;
	m_nWndHeight = 0;
	m_nIndent    = 0;
	m_nFontSize  = 12;
	m_pMouseHoverWnd = NULL;
	m_nObj = 0;
	m_ObjTitle[0]  =0;
	m_nTitleLen    = 0;
	m_ObjProp[0]   = 0;
	m_nPropLen     = 0;
	m_ObjDesc[0]   = 0;
	m_nDescLen     = 0;
	m_uTitleColor  = 0;
	m_uPropColor   = 0;
	m_uDescColor   = 0;
	m_uTitleBgColor= 0;
	m_uPropBgColor = 0;
	m_uDescBgColor = 0;
	memset(&m_Image, 0, sizeof(KRUImage));
	m_bHeadTailImg = true;
	m_bFollowCursor = false;
	m_bShow = false;
}

int  KMouseOver::IsMoseHoverWndObj(void* pWnd, int nObj)
{
	return ((pWnd == m_pMouseHoverWnd) && (nObj == m_nObj));
}

void KMouseOver::CancelMouseHoverInfo()
{
	m_pMouseHoverWnd = 0;
	m_nObj = 0;
	m_ObjTitle[0] = 0;
	m_nTitleLen = 0;
	m_ObjProp[0] = 0;
	m_nPropLen = 0;
	m_ObjDesc[0] = 0;
	m_nDescLen = 0;
	m_bShow = false;
}

/***********************************************************************
*功能：设置鼠标提示窗口的基本信息
************************************************************************/
void KMouseOver::SetMouseHoverInfo(void* pWnd, int nObj, int x, int y,
		                        bool bHeadTailImg, bool bFollowCursor)
{
    m_pMouseHoverWnd = pWnd;
	m_nObj = nObj;
	m_bHeadTailImg = bHeadTailImg;
	m_bFollowCursor = bFollowCursor;
	m_nApplyX = x;
	m_nApplyY = y;
	m_ObjTitle[0] = 0;
	m_nTitleLen = 0;
	m_ObjProp[0] = 0;
	m_nPropLen = 0;
	m_ObjDesc[0] = 0;
	m_nDescLen = 0;
	m_bShow = false;
}


/***********************************************************************
*功能：设置鼠标提示窗口的标题(物件的名字)
************************************************************************/
void KMouseOver::SetMouseHoverTitle(const char *pTitleText, int nTitleTextLen, UINT uColor)
{
	if (nTitleTextLen > 0 && pTitleText && nTitleTextLen <= GOD_MAX_OBJ_TITLE_LEN)
	{
		memcpy(m_ObjTitle, pTitleText, nTitleTextLen);
		m_nTitleLen = nTitleTextLen;
		m_uTitleColor = uColor;
		Update(m_nApplyX, m_nApplyY);
	}
	else
	{
		m_ObjTitle[0] = 0;
		m_nTitleLen = 0;
	}
}


/***********************************************************************
*功能：设置鼠标提示窗口的物件属性
************************************************************************/
void KMouseOver::SetMouseHoverProp(const char *pPropText, int nPropTextLen, UINT uColor)
{
	if (nPropTextLen > 0 && pPropText && nPropTextLen <= GOD_MAX_OBJ_PROP_LEN)
	{
		memcpy(m_ObjProp, pPropText, nPropTextLen);
		m_nPropLen = nPropTextLen;
		m_uPropColor = uColor;
		Update(m_nApplyX, m_nApplyY);
	}
	else
	{
		m_ObjProp[0] = 0;
		m_nPropLen = 0;
	}
}


/***********************************************************************
*功能：设置鼠标提示窗口的物件说明
************************************************************************/
void KMouseOver::SetMouseHoverDesc(const char *pDescText, int nDescTextLen, UINT uColor)
{
	if (nDescTextLen > 0 && pDescText && nDescTextLen <= GOD_MAX_OBJ_DESC_LEN)
	{
		memcpy(m_ObjDesc, pDescText, nDescTextLen);
		m_nDescLen = nDescTextLen;
		m_uDescColor = uColor;
		Update(m_nApplyX, m_nApplyY);
	}
	else
	{
		m_ObjDesc[0] = 0;
		m_nDescLen = 0;
	}
}


void KMouseOver::Update(int nX, int nY)
{
	m_bShow = false;

	//if (g_pRepresentShell == NULL)
		return;

	int nMaxTitleLen, nMaxPropLen, nMaxDescLen;

	m_nMaxLineLen = 0;
	if (m_nTitleLen > 0)
	{
		m_nTitleLineNum = TGetEncodedTextLineCount(
			m_ObjTitle, m_nTitleLen, 0, nMaxTitleLen, m_nFontSize);
		m_nMaxLineLen = nMaxTitleLen;
	}
	else
	{
		m_nTitleLineNum = 0;
	}
	if (m_nPropLen > 0)
	{
		m_nPropLineNum = TGetEncodedTextLineCount(
			m_ObjProp, m_nPropLen, 0, nMaxPropLen, m_nFontSize);
		if(m_nMaxLineLen < nMaxPropLen + 3)	//此+3用于扩展tab字符占的空间
			m_nMaxLineLen = nMaxPropLen + 3;
	}
	else
	{
		m_nPropLineNum = 0;
	}
	if (m_nDescLineNum > 0)
	{
		m_nDescLineNum = TGetEncodedTextLineCount(
			m_ObjDesc, m_nDescLen, 0, nMaxDescLen, m_nFontSize);
		if(m_nMaxLineLen < nMaxDescLen)
		    m_nMaxLineLen = nMaxDescLen;
	}
	else
	{
		m_nDescLineNum = 0;
	}


	int nNumLine = m_nTitleLineNum + m_nPropLineNum + m_nDescLineNum;
	if (nNumLine == 0)
		return;

	if (m_bFollowCursor == false && m_nMaxLineLen < INFO_MIN_LEN)
		m_nMaxLineLen = INFO_MIN_LEN;
	m_nWndWidth = m_nFontSize * m_nMaxLineLen / 2 + m_nIndent * 2;
	m_nWndHeight = (m_nFontSize + 1) * nNumLine;
	if (m_bHeadTailImg)
		m_nWndHeight += m_nImgHeight * 2;
	if (m_bFollowCursor)
	{
		m_nLeft = nX + FOLLOW_CURSOR_OFFSET_X;
		m_nTop  = nY + FOLLOW_CURSOR_OFFSET_Y;
	}
	else
	{
		//ALW_GetWndPosition(m_nLeft, m_nTop, m_nWndWidth, m_nWndHeight);
	}

	//可以显示浮动窗口啦!
	m_bShow = true;
}


//鼠标的位置更新了
void KMouseOver::UpdateCursorPos(int nX, int nY)
{
	if (m_bFollowCursor && m_bShow)
	{
		m_nLeft = nX + FOLLOW_CURSOR_OFFSET_X;
		m_nTop  = nY + FOLLOW_CURSOR_OFFSET_Y;
	}
}


void KMouseOver::OnWndClosed(void* pWnd)
{
	if (pWnd && pWnd == m_pMouseHoverWnd)
		CancelMouseHoverInfo();
}

//载入界面方案
void KMouseOver::LoadScheme(const char* pScheme)
{
	if (pScheme == NULL)
		return;
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
	if (Ini.Load(Buff))
	{
		Ini.GetInteger("Main", "ImgWidth",    0, &m_nImgWidth);
		Ini.GetInteger("Main", "ImgHeight",   0, &m_nImgHeight);
		Ini.GetInteger("Main", "Indent",      0, &m_nIndent);
		Ini.GetInteger("Main", "Font",        0, &m_nFontSize);

		if(m_nImgWidth < 0)
			m_nImgWidth = 0;
		if(m_nImgHeight < 0)
			m_nImgWidth = 0;
		if(m_nFontSize < 8)
			m_nFontSize = 8;
		int nValue;
		Ini.GetInteger("Main", "ImgType", 0, &nValue);
		if (nValue == 1)
		{
			m_Image.nType = ISI_T_BITMAP16;
			m_Image.bRenderStyle = IMAGE_RENDER_STYLE_OPACITY;
		}
		else
		{
			m_Image.nType = ISI_T_SPR;
			m_Image.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
			m_Image.Color.Color_b.a = 255;
		}
		m_Image.uImage = 0;
		m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
		Ini.GetString("Main", "Image", "" , m_Image.szImage, sizeof(m_Image.szImage));
		Ini.GetInteger("Main", "Frame", 0, &nValue);
		m_Image.nFrame = nValue;

		Ini.GetString("Main", "TitleBgColor", "0, 0, 0", Buff, sizeof(Buff));
		m_uTitleBgColor = ((GetColor(Buff) & 0xffffff) | 0x0a000000);

		Ini.GetString("Main", "PropBgColor", "0, 0, 0", Buff, sizeof(Buff));
		m_uPropBgColor = ((GetColor(Buff) & 0xffffff) | 0x0a000000);

		Ini.GetString("Main", "DescBgColor", "0, 0, 0", Buff, sizeof(Buff));
		m_uDescBgColor = ((GetColor(Buff) & 0xffffff) | 0x0a000000);

		Update((m_nLeft + m_nWndWidth) / 2, m_nTop);
	}
}


void KMouseOver::PaintMouseHoverInfo()
{
	//if (m_bShow == false || g_pRepresentShell == NULL)
		return;

	//画背景阴影和边框
	KRUShadow Shadow;
	//写文字初始化
	KOutputTextParam	Param;
	Param.BorderColor = 0;
	Param.nZ = TEXT_IN_SINGLE_PLANE_COORD;

	//====标题(名字)部分====
	Shadow.oPosition.nX = m_nLeft;
	Shadow.oPosition.nY = m_nTop;
	if (m_bHeadTailImg)
		Shadow.oPosition.nY = m_nTop + m_nImgHeight;
	else
		Shadow.oPosition.nY = m_nTop;
	Shadow.oEndPos.nX = m_nLeft + m_nWndWidth;
	Shadow.oEndPos.nY = Shadow.oPosition.nY;
	if(m_nTitleLen > 0)
	{
		Shadow.Color.Color_dw = m_uTitleBgColor;
		Shadow.oEndPos.nY += (m_nFontSize + 1) * m_nTitleLineNum;
		//g_pRepresentShell->DrawPrimitives(1, &Shadow, RU_T_SHADOW, true);

		Param.nSkipLine = 0;
		Param.nNumLine = 1;
		Param.Color = m_uTitleColor;
		Param.nY = Shadow.oPosition.nY;
		int nLineLen;
		while(true)
		{
			if (TGetEncodedTextLineCount(m_ObjTitle, m_nTitleLen, 0, nLineLen, m_nFontSize, Param.nSkipLine, 1) == 0)
				break;
			Param.nX = m_nLeft + m_nWndWidth / 2 - (nLineLen * m_nFontSize) / 4;
			//g_pRepresentShell->OutputRichText(m_nFontSize, &Param, m_ObjTitle, m_nTitleLen, 0);
			Param.nSkipLine ++;
			Param.nY += m_nFontSize + 1;
		};
	}

	//====属性部分====
	if(m_nPropLen > 0)
	{
		Shadow.Color.Color_dw = m_uPropBgColor;
		Shadow.oPosition.nY = Shadow.oEndPos.nY;
		Shadow.oEndPos.nY += (m_nFontSize + 1) * m_nPropLineNum;
		//g_pRepresentShell->DrawPrimitives(1, &Shadow, RU_T_SHADOW, true);

		Param.nSkipLine = 0;
		Param.Color = m_uPropColor;
		Param.nNumLine = m_nPropLineNum;
		Param.nX = m_nLeft + m_nIndent;
		Param.nY = Shadow.oPosition.nY;
		//OutputTabSplitText(m_ObjProp, m_nPropLen, m_nWndWidth - m_nIndent * 2, m_nFontSize, &Param);
	}

	//====描述部分====
	if(m_nDescLen > 0)
	{
		Shadow.Color.Color_dw = m_uDescBgColor;
		Shadow.oPosition.nY = Shadow.oEndPos.nY;
		Shadow.oEndPos.nY += (m_nFontSize + 1) * m_nDescLineNum;
		//g_pRepresentShell->DrawPrimitives(1, &Shadow, RU_T_SHADOW, true);

		Param.nSkipLine = 0;
		Param.Color = m_uDescColor;
		Param.nNumLine = m_nDescLineNum;
		Param.nX = m_nLeft + m_nIndent;
		Param.nY = Shadow.oPosition.nY;
		//g_pRepresentShell->OutputRichText(m_nFontSize, &Param, m_ObjDesc, m_nDescLen, 0);
	}

	//画底图和边框图
	if (m_bHeadTailImg && m_nImgWidth > 0)
	{
		m_Image.oPosition.nX = m_nLeft;
		while(m_Image.oPosition.nX < m_nLeft + m_nWndWidth)
		{
			m_Image.oPosition.nY = m_nTop;
			//g_pRepresentShell->DrawPrimitives(1, &m_Image, RU_T_IMAGE, true);
			m_Image.oPosition.nY = Shadow.oEndPos.nY;
			//g_pRepresentShell->DrawPrimitives(1, &m_Image, RU_T_IMAGE, true);
			m_Image.oPosition.nX += m_nImgWidth;
		};
	}
}


/***********************************************************************
*功能：按既定的最大宽度把属性部分格式化(左右靠)
************************************************************************/
/*void KMouseOver::FormatProp()
{
    if(m_ObjProp[0])
	{
		char szBuffer[MAX_OBJ_PROP_LEN], *pPos = NULL, *pHead = NULL, *pTail = NULL;
	    int nLeftLen = 0, nRightLen = 0, i, j, k;
        //初始准备
		memset(szBuffer, 0, MAX_OBJ_PROP_LEN);
		memcpy(szBuffer, m_ObjProp, m_nPropLen);
		memset(m_ObjProp, 0, MAX_OBJ_PROP_LEN);
	    szBuffer[MAX_OBJ_PROP_LEN - 1] = 0;
		pHead = szBuffer;
		//开始循环处理
        while(pHead[0])
	    {
            pPos = strchr(pHead, 0x20);
			if(pPos == NULL) break;
			nLeftLen = pPos - pHead;

			pTail = strchr(pPos, 0x0a);
			if(pTail == NULL)
			{
				pTail = strlen(pPos) + pPos;
				if((pTail - pPos) <= 1)
				    break;
			}
			nRightLen = pTail - pPos - 1;

			*pPos = *pTail = 0;
			strcat(m_ObjProp, pHead);
			j = m_nMaxLineLen - nLeftLen - nRightLen;
			k = strlen(m_ObjProp);
			for(i=0;i < j;i++)
			{
				m_ObjProp[k] = ' ';
				k++;
			}
			m_ObjProp[k] = 0;
			strcat(m_ObjProp, pPos + 1);
			k = strlen(m_ObjProp);
			m_ObjProp[k] = 0x0a;
			m_ObjProp[k+1] = 0;

			pHead = pTail + 1;
			pPos =  NULL;
			pTail = NULL;
	    };

		j = strlen(m_ObjProp);
		m_ObjProp[j] = 0;
	    m_nPropLen = strlen(m_ObjProp);
    }
}*/
