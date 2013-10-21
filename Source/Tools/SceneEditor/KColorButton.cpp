// KColorButton.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KColorButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KColorButton

KColorButton::KColorButton()
{
	m_dwColor = 0;
}

KColorButton::~KColorButton()
{
}


BEGIN_MESSAGE_MAP(KColorButton, CButton)
	//{{AFX_MSG_MAP(KColorButton)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
//ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)

/////////////////////////////////////////////////////////////////////////////
// KColorButton message handlers

void KColorButton::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CRect Rect;
	GetWindowRect(&Rect);
	CRect CC;
	CC.left = 3;
	CC.top  = 3;
	CC.right  = Rect.Width()-3;
	CC.bottom = Rect.Height()-3;
	dc.FillSolidRect(&CC,m_dwColor);
	// Do not call CButton::OnPaint() for painting messages
}

/*void KColorButton::OnClicked() 
{
	// TODO: Add your control notification handler code here
	CPaintDC dc(this); // device context for painting
	CRect Rect;
	GetWindowRect(&Rect);
	CRect CC;
	CC.left = 3;
	CC.top  = 3;
	CC.right  = Rect.Width()-6;
	CC.bottom = Rect.Height()-6;
	dc.FillSolidRect(&CC,0x000000FF);
}*/
