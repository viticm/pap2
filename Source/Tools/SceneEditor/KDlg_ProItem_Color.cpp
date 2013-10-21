// KDlg_ProItem_Color.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KDlg_ProItem_Color.h"
#include "KColorDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_Color dialog
namespace PropertyBase
{

KDlg_ProItem_Color::KDlg_ProItem_Color(CWnd* pParent /*=NULL*/)
	: CDialog(KDlg_ProItem_Color::IDD, pParent)
{
	//{{AFX_DATA_INIT(KDlg_ProItem_Color)
	m_Name = _T("");
	//}}AFX_DATA_INIT
	m_dwColor = 0;
}


void KDlg_ProItem_Color::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KDlg_ProItem_Color)
	DDX_Control(pDX, IDC_BUTTON1, m_Color);
	DDX_Text(pDX, IDC_SNAME, m_Name);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KDlg_ProItem_Color, CDialog)
	//{{AFX_MSG_MAP(KDlg_ProItem_Color)
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_Color message handlers

BOOL KDlg_ProItem_Color::OnInitDialog() 
{
	CDialog::OnInitDialog();
	//if(m_lpProperty)
	//{
	//	m_Name = m_lpProperty->scName.c_str();

	//	if(m_lpProperty->Type==PropertyBase::_VarProperty::VAR_COLOR)
	//	{
	//		D3DCOLOR* pColor = (D3DCOLOR*) m_lpProperty->Address;

	//		SetColor(pColor);
	//		m_Color.m_dwColor = m_dwColor;
	//		m_Color.RedrawWindow();
	//	}
	//	else if(m_lpProperty->Type==PropertyBase::_VarProperty::VAR_COLORVALUE)
	//	{
	//		D3DCOLORVALUE* pColor = (D3DCOLORVALUE*) m_lpProperty->Address;

	//		SetColor(pColor);
	//		m_Color.m_dwColor = m_dwColor;
	//		m_Color.RedrawWindow();
	//	}
	//	UpdateData(FALSE);
	//}
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void KDlg_ProItem_Color::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	//if(m_lpProperty->Type==PropertyBase::_VarProperty::VAR_COLOR)
	//{
	//	/*D3DCOLOR* pColor = (D3DCOLOR*) m_lpProperty->Address;
	//	CDC* pDc = m_Color.GetDC();
	//	CRect Rect;
	//	m_Color.GetWindowRect(&Rect);
	//	CRect CC;
	//	CC.left = 0;
	//	CC.top  = 0;
	//	CC.right  = Rect.Width();
	//	CC.bottom = Rect.Height();

	//	pDc->FillSolidRect(&CC,0x00FF0000);
	//	//CString str = "ABCDEFG";
	//	//pDc->TextOut(0,0,str);

	//	m_Color.ReleaseDC(pDc);*/
	//}
	//else if(m_lpProperty->Type==PropertyBase::_VarProperty::VAR_COLORVALUE)
	//{
	//	//m_Value = ((string*) m_lpProperty->Address)->c_str();
	//}
	CDialog::OnMouseMove(nFlags, point);
}

void KDlg_ProItem_Color::OnButton1() 
{
	// TODO: Add your control notification handler code here
	//if(m_lpProperty->Type==PropertyBase::_VarProperty::VAR_COLOR)
	//{
	//	D3DCOLOR* pColor = (D3DCOLOR*) m_lpProperty->Address;
	//	KColorPickerDialog* pDlg = new KColorPickerDialog(pColor);
	//	pDlg->ShowWindow(TRUE);
	//}
	//else if(m_lpProperty->Type==PropertyBase::_VarProperty::VAR_COLORVALUE)
	//{
	//	D3DCOLORVALUE* pColor = (D3DCOLORVALUE*) m_lpProperty->Address;
	//	KColorPickerDialog* pDlg = new KColorPickerDialog(pColor);
	//	pDlg->ShowWindow(TRUE);

	//}	

	//if(m_lpProperty->Type==PropertyBase::_VarProperty::VAR_COLOR)
	//{
	//	D3DCOLOR* pColor = (D3DCOLOR*) m_lpProperty->Address;

	//	SetColor(pColor);
	//	m_Color.m_dwColor = m_dwColor;
	//	m_Color.RedrawWindow();
	//}
	//else if(m_lpProperty->Type==PropertyBase::_VarProperty::VAR_COLORVALUE)
	//{
	//	D3DCOLORVALUE* pColor = (D3DCOLORVALUE*) m_lpProperty->Address;

	//	SetColor(pColor);
	//	m_Color.m_dwColor = m_dwColor;
	//	m_Color.RedrawWindow();
	//}
}

}
