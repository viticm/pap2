// KDlg_PropertyDialog.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KDlg_PropertyDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KDlg_PropertyDialog dialog
namespace PropertyBase
{


KDlg_PropertyDialog::KDlg_PropertyDialog(CWnd* pParent /*=NULL*/)
	: CDialog(KDlg_PropertyDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(KDlg_PropertyDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_lpPropertyBase = NULL;
	m_bShow = TRUE;
}

KDlg_PropertyDialog::~KDlg_PropertyDialog()
{
	Clear();
}

BOOL KDlg_PropertyDialog::ShowWindow (int nCmdShow)
{
	if((TRUE==nCmdShow)&&(FALSE==m_bShow))
	{
		m_bShow = nCmdShow;
		CreateUI();
	}
	m_bShow = nCmdShow;


	int curIndex = m_Tab.GetCurSel();
	for(DWORD i=0;i<m_vecPage.size();i++)
	{
		m_vecPage[i]->EnableWindow(FALSE);
		m_vecPage[i]->ShowWindow(FALSE);
	}

	if((curIndex>=0)&&(curIndex<(int)m_vecPage.size()))
	{
		m_vecPage[curIndex]->EnableWindow(TRUE);
		m_vecPage[curIndex]->ShowWindow(nCmdShow);
		//m_vecPage[curIndex]->SetFocus();
	}

	return CDialog::ShowWindow(nCmdShow);
}

HRESULT KDlg_PropertyDialog::Clear()
{
	for(DWORD i=0;i<m_vecPage.size();i++)
	{
		KDlg_PropertyPage* pPage = m_vecPage[i];
		SAFE_DELETE(pPage);
	}
	m_vecPage.clear();

	return S_OK;
}

HRESULT KDlg_PropertyDialog::CreateUI()
{
	Clear();

	m_Tab.DeleteAllItems();

	if(!m_bShow)
		return S_OK;

	if(m_lpPropertyBase)
	{
		for(DWORD i=0;i<m_lpPropertyBase->m_vecPage.size();i++)
		{
			PropertyBase::_PropertyPage* pPage = &m_lpPropertyBase->m_vecPage[i];

			KDlg_PropertyPage * pDlg = new KDlg_PropertyPage();
			pDlg->SetPropertyPage(pPage);
			pDlg->Create(IDD_PROPAGE4,this);
			pDlg->SetWindowText(pPage->scName.c_str());

			AddPage(pDlg,NULL);
			pDlg->ShowWindow(m_bShow);

		}
	}

	RecalcLayout();

	//ShowWindow(m_bShow);

	return S_OK;
}

void KDlg_PropertyDialog::SetPropertyBase(PropertyBase::KPropertyBase* pProperty)
{
	if(m_lpPropertyBase != pProperty)
	{
		m_lpPropertyBase = pProperty;
		//CreateUI();
	}
}

void KDlg_PropertyDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KDlg_PropertyDialog)
	DDX_Control(pDX, IDC_TAB1, m_Tab);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KDlg_PropertyDialog, CDialog)
	//{{AFX_MSG_MAP(KDlg_PropertyDialog)
	ON_WM_CREATE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, OnSelchangeTab1)
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KDlg_PropertyDialog message handlers

int KDlg_PropertyDialog::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	return 0;
}


BOOL KDlg_PropertyDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CreateUI();
	// TODO: Add extra initialization here
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void KDlg_PropertyDialog::OnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	int curIndex = m_Tab.GetCurSel();
	for(DWORD i=0;i<m_vecPage.size();i++)
	{
		m_vecPage[i]->EnableWindow(FALSE);
		m_vecPage[i]->ShowWindow(FALSE);
	}

	if((curIndex>=0)&&(curIndex<(int)m_vecPage.size()))
	{
		m_vecPage[curIndex]->EnableWindow(TRUE);
		m_vecPage[curIndex]->ShowWindow(TRUE);
		m_vecPage[curIndex]->SetFocus();
	}

	RecalcLayout();
	*pResult = 0;
}

void KDlg_PropertyDialog::RecalcLayout()
{
	if(!::IsWindow(m_hWnd))
		return ;

	/*CWnd *pParent = GetParent();
	if(pParent == NULL) //必须有父窗口
		return ;*/

	CRect rcRect;
	m_Tab.GetWindowRect(rcRect);

	//pParent->ScreenToClient(&rcRect);

	int nRow = m_Tab.GetRowCount();
	CRect rcItem(0, 0, 0, 0);
	m_Tab.GetItemRect(0, &rcItem);

	CPoint pt = CPoint(0, 0);
	int K = nRow*(rcItem.Height() + 3);
	pt.Offset(3, K);

	for(DWORD i=0;i<m_vecPage.size();i++)
	{
		CRect rcMove;
		m_vecPage[i]->GetWindowRect(rcMove);

		CPoint Move;
		Move.x = rcRect.left - rcMove.left + pt.x; 
		Move.y = rcRect.top - rcMove.top + pt.y;

		rcMove.OffsetRect(Move);
		rcMove.right = rcMove.left + rcRect.Width() - 3;
		rcMove.bottom = rcMove.top + rcRect.Height() - K - 3;

		m_vecPage[i]->MoveWindow(rcMove);

		if(rcRect.right < rcMove.right)
			rcRect.right = rcMove.right;
		if(rcRect.bottom < rcMove.bottom)
			rcRect.bottom = rcMove.bottom;

	}

	//MoveWindow(rcRect);
}

int KDlg_PropertyDialog::AddPage(KDlg_PropertyPage *pPage, CRect *pRect)
{
	int nItem = -1;
	if(pPage && ::IsWindow(pPage->GetSafeHwnd()) &&
		::IsWindow(m_hWnd))
	{
		CString Name;
		pPage->GetWindowText(Name);
		m_Tab.InsertItem((int)m_vecPage.size(), Name);
	
		m_vecPage.push_back(pPage);

		DWORD dwStyle = WS_CHILD | WS_VISIBLE;
		//SetWindowLong(pPage->GetSafeHwnd(), GWL_STYLE, dwStyle);
		//SetWindowLong(pPage->GetSafeHwnd(), GWL_EXSTYLE, 0);
		pPage->RedrawWindow();

		RecalcLayout();

		//开始的初始选择
		LRESULT lr;
		OnSelchangeTab1(NULL, &lr);
	}
	return nItem;
}

void KDlg_PropertyDialog::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	RecalcLayout();
	// Do not call CDialog::OnPaint() for painting messages
}

void KDlg_PropertyDialog::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	for(DWORD i=0;i<m_vecPage.size();i++)
	{
		m_vecPage[i]->EnableWindow(FALSE);
		m_vecPage[i]->ShowWindow(FALSE);
	}
	m_bShow = FALSE;
	CDialog::OnClose();
}

void KDlg_PropertyDialog::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	
}
};