/*****************************************************************************************
//	界面窗口体系结构--滚动条窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-7-23
*****************************************************************************************/
#include "../../Public/KWin32.h"
#include "../../Public/KIniFile.h"
#include "../Elem/WndMessage.h"
#include "WndScrollBar.h"
#include "Wnds.h"
#include "../../Public/iRepresentShell.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern iRepresentShell*	g_pRepresentShell;

//--------------------------------------------------------------------------
//	功能：构造函数
//--------------------------------------------------------------------------
KWndScrollBar::KWndScrollBar()
{
	m_Flag = 0;
	m_nMinValue = 0;
	m_nMaxValue = 0;
	m_nCurValue = 0;
//	m_nLineSize = 0;
	m_nPageSize = 0;
	m_nMinPosition  = 0;
	m_nMaxPosition  = 0;
	m_nImgRange = 0;
	AddChild(&m_SlideBtn);
}

void KWndScrollBar::Clone(KWndScrollBar* pCopy)
{
	if (pCopy)
	{
		KWndImage::Clone(pCopy);
		pCopy->m_nMinValue = m_nMinValue;
		pCopy->m_nMaxValue = m_nMaxValue;
		pCopy->m_nCurValue = m_nMinValue - 1;
		pCopy->m_nPageSize = m_nPageSize;
		pCopy->m_nMinPosition = m_nMinPosition;
		pCopy->m_nMaxPosition = m_nMaxPosition;
		pCopy->m_Flag = (m_Flag & (~WNDSCROLL_F_DRAGGING_SLIDE));
		pCopy->m_nImgRange = m_nImgRange;
		m_SlideBtn.Clone(&pCopy->m_SlideBtn);
		pCopy->SetScrollPos(m_nMinValue);
	}
}

//--------------------------------------------------------------------------
//	功能：初始化窗口
//--------------------------------------------------------------------------
int KWndScrollBar::Init(KIniFile* pIniFile, const char* pSection)
{
	if (KWndImage::Init(pIniFile, pSection))
	{
		int		nValue;
		pIniFile->GetInteger(pSection, "Type", 0, &nValue);
		if (nValue)
			m_Flag |= WNDSCROLL_ES_VERTICAL;
		else
			m_Flag &= ~WNDSCROLL_ES_VERTICAL;
		pIniFile->GetInteger(pSection, "RepeatImg", 0, &nValue);
		m_nImgRange = 0;
		if (nValue && g_pRepresentShell)
		{
			KImageParam	Param;
			Param.nWidth = 0;
			Param.nHeight = 0;
			g_pRepresentShell->GetImageParam(m_Image.szImage, &Param, m_Image.nType);
			if (m_Flag & WNDSCROLL_ES_VERTICAL)
				m_nImgRange = Param.nHeight;
			else
				m_nImgRange = Param.nWidth;
		}

		pIniFile->GetInteger(pSection, "Min", 0, &m_nMinValue);
		pIniFile->GetInteger(pSection, "Max", 0, &m_nMaxValue);
//		pIniFile->GetInteger(pSection, "Position", 0, &m_nCurValue);
//		pIniFile->GetInteger(pSection, "LineSize", 1, &m_nLineSize);
		pIniFile->GetInteger(pSection, "PageSize", 1, &m_nPageSize);
		pIniFile->GetInteger(pSection, "SlideBegin",0, &m_nMinPosition);
		pIniFile->GetInteger(pSection, "SlideEnd",  0, &m_nMaxPosition);
		char Buffer[128];
		sprintf(Buffer, "%s_Btn", pSection);
		m_SlideBtn.Init(pIniFile, Buffer);

		SetSlideBtnPos();

		return true;
	}
	return false;
}

//设置窗口大小
void KWndScrollBar::SetSize(int nWidth, int nHeight)
{
	if (m_Flag & WNDSCROLL_ES_VERTICAL)
		m_nMaxPosition += nHeight - m_Height;
	else
		m_nMaxPosition += nWidth - m_Height;

	if (m_nMaxPosition < m_nMinPosition)
		m_nMaxPosition = m_nMinPosition;

	KWndImage::SetSize(nWidth, nHeight);
	SetSlideBtnPos();
}

//--------------------------------------------------------------------------
//	功能：窗口函数
//--------------------------------------------------------------------------
int KWndScrollBar::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	if (IsDisable())
		return KWndImage::WndProc(uMsg, uParam, nParam);
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
		OnLButtonDown(GET_X_LPARAM(nParam), GET_Y_LPARAM(nParam));
		break;
	case WM_MOUSEMOVE:
		if ((uParam & MK_LBUTTON) &&
			(m_Flag & WNDSCROLL_F_DRAGGING_SLIDE))
			OnDraggingSlide(GET_X_LPARAM(nParam), GET_Y_LPARAM(nParam));
		break;
	case WM_LBUTTONUP:
		if (m_Flag & WNDSCROLL_F_DRAGGING_SLIDE)
		{
			m_Flag &= ~WNDSCROLL_F_DRAGGING_SLIDE;
			Wnd_ReleaseCapture();
		}
		break;
	case WND_N_BUTTON_CLICK:
		if (m_Flag & WNDSCROLL_F_DRAGGING_SLIDE &&
			uParam == (unsigned int)(KWndWindow*)&m_SlideBtn)
		{
			m_Flag &= ~WNDSCROLL_F_DRAGGING_SLIDE;
			Wnd_ReleaseCapture();
		}
		break;
	case WND_N_BUTTON_DOWN:
		if (uParam == (unsigned int)(KWndWindow*)&m_SlideBtn)
			OnSlideBtnPressed();
		break;
	default:
		return KWndImage::WndProc(uMsg, uParam, nParam);
	}
	return 0;
}

//窗体绘制
void KWndScrollBar::PaintWindow()
{
	KWndWindow::PaintWindow();
	if (g_pRepresentShell)
	{
		m_Image.oPosition.nX = m_nAbsoluteLeft;
		m_Image.oPosition.nY = m_nAbsoluteTop;
		if (m_nImgRange == 0)
		{
			g_pRepresentShell->DrawPrimitives(1, &m_Image, RU_T_IMAGE, true);
		}
		else
		{
			int*	pValue;
			int		nEnd;
			if (m_Flag & WNDSCROLL_ES_VERTICAL)
			{
				pValue = &m_Image.oPosition.nY;
				nEnd = m_nAbsoluteTop + m_Height;
			}
			else
			{
				pValue = &m_Image.oPosition.nX;
				nEnd = m_nAbsoluteLeft + m_Width;
			}

			do
			{
				g_pRepresentShell->DrawPrimitives(1, &m_Image, RU_T_IMAGE, true);
				(*pValue) =  (*pValue) + m_nImgRange;
			}while((*pValue) < nEnd);
		}
	}
}

//--------------------------------------------------------------------------
//	功能：设置取值范围
//--------------------------------------------------------------------------
void KWndScrollBar::SetValueRange(int nMinValue,int nMaxValue)
{
	if (nMinValue <= nMaxValue)
	{
		m_nMinValue = nMinValue;
		m_nMaxValue = nMaxValue;
		SetScrollPos(m_nCurValue);
	}
}

//--------------------------------------------------------------------------
//	功能：设置位置
//--------------------------------------------------------------------------
void KWndScrollBar::SetScrollPos(int nPosition)
{
	if (nPosition != m_nCurValue)
	{
		if (nPosition < m_nMinValue)
			nPosition = m_nMinValue;
		if (nPosition > m_nMaxValue)
			nPosition = m_nMaxValue;
		if (nPosition != m_nCurValue)
		{
			m_nCurValue = nPosition;
			SetSlideBtnPos();
			if (m_pParentWnd)
			{
				m_pParentWnd->WndProc(WND_N_SCORLLBAR_POS_CHANGED, (unsigned int)(KWndWindow*)this, m_nCurValue);
			}
		}
	}
}

//--------------------------------------------------------------------------
//	功能：设置滑动块窗口位置
//--------------------------------------------------------------------------
void KWndScrollBar::SetSlideBtnPos()
{
	int nValueRange = m_nMaxValue - m_nMinValue;
	int nSpaceRange = m_nMaxPosition - m_nMinPosition;

	if (nValueRange <= 0)
		nValueRange = 1;

	int	Left, Top;
	m_SlideBtn.GetPosition(&Left, &Top);
	int	pos = m_nMinPosition + m_nCurValue * nSpaceRange / nValueRange;
	if (pos < m_nMinPosition)
		pos  = m_nMinPosition;
	if (pos > m_nMaxPosition)
		pos = m_nMaxPosition;

	int	nWidth, nHeight;
	m_SlideBtn.GetSize(&nWidth, &nHeight);
	if ((m_Flag & WNDSCROLL_ES_VERTICAL) == 0)
		Left = pos - nWidth / 2;
	else
		Top = pos - nHeight / 2;
	m_SlideBtn.SetPosition(Left, Top);
}

//--------------------------------------------------------------------------
//	功能：响应鼠标左键按下
//--------------------------------------------------------------------------
void KWndScrollBar::OnLButtonDown(int x,int y)
{
	int	Left, Top, Width, Height;
	m_SlideBtn.GetAbsolutePos(&Left, &Top);
	m_SlideBtn.GetSize(&Width, &Height);
	if ((m_Flag & WNDSCROLL_ES_VERTICAL) == 0) // Horizontal
	{
		if (x < Left + Width / 2)
			SetScrollPos(m_nCurValue - m_nPageSize);
		else
			SetScrollPos(m_nCurValue + m_nPageSize);
	}
	else
	{
		if (y < Top + Height / 2)
			SetScrollPos(m_nCurValue - m_nPageSize);
		else
			SetScrollPos(m_nCurValue + m_nPageSize);
	}
}

int KWndScrollBar::ScrollLine(bool bPre)
{
	int nPos = m_nCurValue + (bPre ? (-1) : 1);
	SetScrollPos(nPos);
	return m_nCurValue;
}

int	KWndScrollBar::ScrollPage(bool bPre)
{
	int nPos = m_nCurValue + (bPre ? (-m_nPageSize) : m_nPageSize);
	SetScrollPos(nPos);
	return m_nCurValue;
}

//--------------------------------------------------------------------------
//	功能：响应滑动按钮被按下
//--------------------------------------------------------------------------
void KWndScrollBar::OnSlideBtnPressed()
{
	Wnd_SetCapture(this);
	m_Flag |= WNDSCROLL_F_DRAGGING_SLIDE;
}

//--------------------------------------------------------------------------
//	功能：正在拖动滑动按钮
//--------------------------------------------------------------------------
void KWndScrollBar::OnDraggingSlide(int x, int y)
{
	int nValueRange = m_nMaxValue - m_nMinValue;
	int nSpaceRange = m_nMaxPosition - m_nMinPosition;
	if (nSpaceRange <= 0)
		nSpaceRange = 1;
	int	nValue = m_nMinValue;
	if (nValueRange != 0)
	{
		if ((m_Flag & WNDSCROLL_ES_VERTICAL) == 0)
			nValue += (x - m_nAbsoluteLeft - m_nMinPosition + nSpaceRange / nValueRange / 2) * nValueRange / nSpaceRange;
		else
			nValue += (y - m_nAbsoluteTop - m_nMinPosition + nSpaceRange / nValueRange / 2) * nValueRange / nSpaceRange;
	}
	SetScrollPos(nValue);
}

int	KWndScrollBar::GetMinHeight()
{
	int nHeight = 0;
	m_SlideBtn.GetSize(NULL, &nHeight);
	return max(nHeight, 3 + m_nMinPosition + m_Height - m_nMaxPosition);
}