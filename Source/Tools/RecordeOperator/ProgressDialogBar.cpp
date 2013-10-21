// ProgressDialogBar.cpp : implementation file
//

#include "stdafx.h"
#include "RecordeOperator.h"
#include "ProgressDialogBar.h"
#include "RecordeOperatorDoc.h"
#include "MainFrm.h"
#include "ChildFrm.h"


// CProgressDialogBar dialog

IMPLEMENT_DYNAMIC(CProgressDialogBar, CDialogBar)
CProgressDialogBar::CProgressDialogBar()
{
	
}

CProgressDialogBar::~CProgressDialogBar()
{
}


void CProgressDialogBar::SetPorgressRange(int nMin,int nMax,BOOL bRedraw)
{
	m_sliderProgress.SetRange(nMin,nMax,bRedraw);
}
void CProgressDialogBar::SetProgressPos(int nPos)
{
	m_sliderProgress.SetPos(nPos);
}

void CProgressDialogBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER_PROGRESS, m_sliderProgress);
	DDX_Control(pDX, IDC_BUTTON_PLAY, m_buttonPlay);
	DDX_Control(pDX, IDC_BUTTON_STOP, m_buttonStop);
	DDX_Control(pDX, IDC_BUTTON_FREE, m_buttonFreeCamera);
}


LONG CProgressDialogBar::OnInitDialog ( UINT wParam, LONG lParam)
{
	BOOL bRet = HandleInitDialog(wParam, lParam);
	if (!UpdateData(FALSE))
	{

	}
	return bRet;
}
void CProgressDialogBar::OnHScroll(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar)
{
	switch(pScrollBar->GetDlgCtrlID())   
	{   
		case IDC_SLIDER_PROGRESS:
				g_bSlider = FALSE;
				break;   	 
		default:
			break;
	}   
	CDialogBar::OnHScroll(nSBCode,nPos,pScrollBar);   
}

BEGIN_MESSAGE_MAP(CProgressDialogBar, CDialogBar)
	 ON_MESSAGE(WM_INITDIALOG, OnInitDialog )  
	 ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CProgressDialogBar message handlers

