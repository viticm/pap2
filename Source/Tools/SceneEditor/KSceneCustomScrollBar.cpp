// KSceneCustomScrollBar.cpp : implementation file
//

#include "stdafx.h"
#include "KSceneCustomScrollBar.h"
//#include "MathTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// KSceneCustomScrollBar

IMPLEMENT_DYNAMIC(KSceneCustomScrollBar, CScrollBar)

KSceneCustomScrollBar::KSceneCustomScrollBar()
:m_nBarType(SBS_VERT)
,m_bLockMessagePocess(false)
{
	
}

KSceneCustomScrollBar::~KSceneCustomScrollBar()
{
}


BEGIN_MESSAGE_MAP(KSceneCustomScrollBar, CScrollBar)
//	ON_WM_VSCROLL()
	ON_WM_VSCROLL_REFLECT()
	ON_WM_HSCROLL_REFLECT()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_DESTROY()
//	ON_WM_SHOWWINDOW()
//	ON_WM_ACTIVATE()
//	ON_WM_CHILDACTIVATE()
END_MESSAGE_MAP()

void KSceneCustomScrollBar::VScroll( UINT nSBCode, UINT nPos )
{
	if (nSBCode == SB_THUMBPOSITION)
	{
		m_bLockMessagePocess = true;
		this->SetScrollPos(nPos);
		m_bLockMessagePocess = false;
	}
	///消息反射说起来NiuX，但其实就是CWnd的各个On什么的向子窗口发消息而已。所以一定要调用CWnd的On才能反射
	OnScroll32(WM_VSCROLL, nSBCode, nPos);
}
void KSceneCustomScrollBar::HScroll( UINT nSBCode, UINT nPos )
{
	if (nSBCode == SB_THUMBPOSITION)
	{
		m_bLockMessagePocess = true;
		this->SetScrollPos(nPos);
		m_bLockMessagePocess = false;
	}
	OnScroll32(WM_HSCROLL, nSBCode, nPos);
}
void KSceneCustomScrollBar::ReAlignBindedWindows()
{
	ReCalculateScrollScales();
	SCROLLINFO SI;
	SI.cbSize = sizeof(SI);
	SI.fMask = SIF_ALL;
	this->GetScrollInfo(&SI);
	m_bLockMessagePocess = true;
	OnScroll32(m_nBarType == SB_VERT ? WM_VSCROLL : WM_HSCROLL , SB_THUMBPOSITION, SI.nPos);
	m_bLockMessagePocess = false;
}
void KSceneCustomScrollBar::OnScroll32( UINT message, UINT nSBCode, UINT nPos )
{	
	size_t uVecSize = m_vecWnd.size();
	if (uVecSize > 0)
	{
		for (size_t i = 0; i < uVecSize; i++)
		{
			CWnd* pWnd = m_vecWnd[i];
			if (pWnd)
			{				
				float fScale = m_vecScrollScale[i];
				INT nNewPos = MT_Round(((float)nPos)*fScale);
				pWnd->SendMessage(message, MAKELONG(nSBCode, nNewPos), (LPARAM)(this->GetSafeHwnd()));				
			}
		}
	}
}

LRESULT KSceneCustomScrollBar::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	LRESULT lpRet =  CScrollBar::WindowProc(message, wParam, lParam);
	
	if (message == SBM_SETSCROLLINFO && !m_bLockMessagePocess)
	{
		ReAlignBindedWindows();
	}

	return lpRet;
}



// KSceneCustomScrollBar message handlers


BOOL KSceneCustomScrollBar::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	///禁止相应这种消息
	//return CScrollBar::OnMouseWheel(nFlags, zDelta, pt);
	return FALSE;
}
void KSceneCustomScrollBar::ReCalculateScrollScales()
{
	size_t uVecSize = m_vecWnd.size();
	if (uVecSize <= 0)
		return;
	DWORD dwStype = this->GetStyle();
	if (dwStype & SB_VERT/*WS_VSCROLL*/)
	{
		m_nBarType = SB_VERT;
	}
	else
	{
		m_nBarType = SB_HORZ;
	}
	_ASSERTE(uVecSize == m_vecScrollScale.size());
	SCROLLINFO SI;
	SI.cbSize = sizeof(SCROLLINFO);
	SI.fMask = SIF_ALL;
	this->GetScrollInfo(&SI,SIF_ALL);
	INT nThisRange = SI.nMax - SI.nMin - SI.nPage + 1;
	enum
	{
		MAX_SCROLL_UNIT = 10000,
	};	
	if (abs(SI.nMax) > MAX_SCROLL_UNIT || abs(SI.nMin) > MAX_SCROLL_UNIT || nThisRange < 0)
	{
		for (size_t i = 0; i < uVecSize; i++)
		{
			m_vecScrollScale[i] = 1;
		}
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	
	_ASSERTE(nThisRange >= 0);
	for (size_t i = 0; i < uVecSize; i++)
	{
		float fScale = 0;
		CWnd* pTargetWnd = m_vecWnd[i];
		if (pTargetWnd)
		{
			pTargetWnd->GetScrollInfo(m_nBarType, &SI);
			INT nThatRange = (SI.nMax - SI.nMin - SI.nPage + 1);
			_ASSERTE(nThatRange >= 0);
			if (nThisRange == nThatRange)
			{
				fScale = 1;
			}
			else
			{
				fScale = nThisRange != 0 ? ((float)nThatRange/(float)nThisRange) : (float)nThatRange;
			}				
		}
		m_vecScrollScale[i] = fScale;			
	}
	
}
void KSceneCustomScrollBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	
	ReCalculateScrollScales();
	CScrollBar::OnLButtonDown(nFlags, point);
}

void KSceneCustomScrollBar::OnDestroy()
{
	CScrollBar::OnDestroy();

	// TODO: Add your message handler code here
	m_vecWnd.clear();
	m_vecScrollScale.clear();
}




