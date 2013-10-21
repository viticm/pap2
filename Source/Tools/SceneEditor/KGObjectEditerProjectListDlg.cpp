
#include "stdafx.h"
#include "KGObjectEditerProjectListDlg.h"
#include "KG3DObjectEditor.h"
#include "KGLog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNAMIC(KGObjectEditerProjectListDlg, CDialog)

KGObjectEditerProjectListDlg::KGObjectEditerProjectListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(KGObjectEditerProjectListDlg::IDD, pParent)
{
	m_pParentWindow = (KG3DObjectEditor*)pParent;
	m_eShowMode = SHOW_MODEL_BIND;
}

KGObjectEditerProjectListDlg::~KGObjectEditerProjectListDlg()
{
}

BOOL KGObjectEditerProjectListDlg::OnInitDialog()
{
	int nResult  = FALSE;
	int nRetCode = FALSE;

	nRetCode = CDialog::OnInitDialog();
	KGLOG_PROCESS_ERROR(nRetCode);

	m_listPropertyView_L.InsertColumn(0, _T("属性名"));
	m_listPropertyView_L.InsertColumn(1, _T("属性值"));
	m_listPropertyView_L.SetColumnWidth(0, 150);
	m_listPropertyView_L.SetColumnWidth(1, 400);
	m_listPropertyView_L.SetBkColor(RGB(92, 92, 92));
	m_listPropertyView_L.SetExtendedStyle(LVS_EX_ONECLICKACTIVATE);
	m_listPropertyView_L.SetupToolTip(200, RGB(68, 68, 68), RGB(255, 255, 255));
	m_listPropertyView_L.SetHoverTime(100);

	m_listPropertyView_R.InsertColumn(0, _T("属性名"));
	m_listPropertyView_R.InsertColumn(1, _T("属性值"));
	m_listPropertyView_R.SetColumnWidth(0, 150);
	m_listPropertyView_R.SetColumnWidth(1, 400);
	m_listPropertyView_R.SetBkColor(RGB(92, 92, 92));
	m_listPropertyView_R.SetExtendedStyle(LVS_EX_ONECLICKACTIVATE);
	m_listPropertyView_R.SetupToolTip(200, RGB(68, 68, 68), RGB(255, 255, 255));
	m_listPropertyView_R.SetHoverTime(100);

	m_staticCtrl_L.ShowWindow(SW_HIDE);
	m_staticCtrl_R.ShowWindow(SW_HIDE);
	m_comboxCtrl_L.ShowWindow(SW_HIDE);
	m_comboxCtrl_R.ShowWindow(SW_HIDE);

	m_listPropertyView_L.ShowWindow(SW_SHOW);
	m_listPropertyView_R.ShowWindow(SW_HIDE);

	m_tabCtrl.InsertItem(0, TEXT("逻辑"));
	m_tabCtrl.InsertItem(1, TEXT("表现"));
    m_tabCtrl.InsertItem(2, TEXT("动作"));

	//m_bnShowLogicList.LoadIcon(AfxGetApp()->LoadIcon(IDC_LOGIC));
	//m_bnShowRepresent.LoadIcon(AfxGetApp()->LoadIcon(IDC_REPER));
	//m_bnShowRepresent.m_nStateFlag = true;
	//m_bnShowLogicList.m_nStateFlag = true;

	/*
	m_fontStatic.CreateFont(
		14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS,
		"宋体"
	);
	*/

	/*m_staticCtrl_L.SetFont(&m_fontStatic);
	m_staticCtrl_R.SetFont(&m_fontStatic);*/

	nResult = TRUE;
Exit0:
	return nResult;
}

void KGObjectEditerProjectListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PROPERTY_L, m_listPropertyView_L);
	DDX_Control(pDX, IDC_LIST_PROPERTY_R, m_listPropertyView_R);
	DDX_Control(pDX, IDC_COMBO_L,   m_comboxCtrl_L);
	DDX_Control(pDX, IDC_COMBO_R,   m_comboxCtrl_R);
	DDX_Control(pDX, IDC_STATIC_L,  m_staticCtrl_L);
	DDX_Control(pDX, IDC_STATIC_R,  m_staticCtrl_R);
	//DDX_Control(pDX, IDC_LOGIC,	    m_bnShowLogicList);
	//DDX_Control(pDX, IDC_REPRESENT, m_bnShowRepresent);
	DDX_Control(pDX, IDC_TAB,		m_tabCtrl);
}

BEGIN_MESSAGE_MAP(KGObjectEditerProjectListDlg, CDialog)
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_NCHITTEST()
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_WM_DRAWITEM()
	ON_MESSAGE(WM_NOTIFY_ITEM_VALUE_UPDATE, OnListItemUpdate)
	ON_MESSAGE(WM_LIST_UPDATE_SCENE, OnUpdateScene)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_PROPERTY_L, &KGObjectEditerProjectListDlg::OnLvnColumnclickListProperty)
	ON_BN_CLICKED(IDC_LOGIC, &KGObjectEditerProjectListDlg::OnBnClickedLogic)
	ON_BN_CLICKED(IDC_REPRESENT, &KGObjectEditerProjectListDlg::OnBnClickedRepresent)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, &KGObjectEditerProjectListDlg::OnTcnSelchangeTab)
END_MESSAGE_MAP()

void KGObjectEditerProjectListDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
	if (::IsWindow(m_tabCtrl.m_hWnd))
	{
		CDC* pDC = m_tabCtrl.GetDC();
		CRect rectClient;
		rectClient.left -= 2;
		rectClient.right += 2;
		GetClientRect(&rectClient);
		COLORREF color;
		int nIndex = m_tabCtrl.GetCurSel();
        if (nIndex == 2)
            color = RGB(255, 0, 255);
		else if (nIndex == 1)
			color = RGB(0, 0, 255);
		else
			color = RGB(255, 0, 0);
		CPen pen(PS_SOLID, 4, color);
		pDC->SelectObject(&pen);

		pDC->MoveTo(rectClient.left,  24);
		pDC->LineTo(rectClient.right, 24);
	}
}
int KGObjectEditerProjectListDlg::SetShowMode(SHOW_MODEL eShowMode)
{
	m_eShowMode = eShowMode;
	return true;
}

SHOW_MODEL KGObjectEditerProjectListDlg::GetShowMode()
{
	return m_eShowMode;
}

int KGObjectEditerProjectListDlg::BandData(
	ITabFile** ppTabFile_L, IIniFile** ppIniFile_L,
	ITabFile** ppTabFile_R, IIniFile** ppIniFile_R
)
{
	m_listPropertyView_L.BandData(ppTabFile_L, ppIniFile_L/*, LIST_EDIT_TYPE_BAND*/);
	m_listPropertyView_R.BandData(ppTabFile_R, ppIniFile_R/*, LIST_EDIT_TYPE_BAND*/);

	return true;
}

int KGObjectEditerProjectListDlg::UpdateProperty(int nPos)
{
	int nRepresentID = 0;

	m_listPropertyView_L.UpdateProperty(nPos);
	(*(m_listPropertyView_L.m_ppTabFile))->GetInteger(nPos, _T("RepresentID"), 0, &nRepresentID);
	UpdateProperty_R(nRepresentID, true);

	return true;
}

int KGObjectEditerProjectListDlg::UpdateProperty_L(int nPos, int nIdFlag)
{
	if (nIdFlag)
		m_listPropertyView_L.UpdateProperty(_T("ID"), nPos);
	else
		m_listPropertyView_L.UpdateProperty(nPos);
	return true;
}

int KGObjectEditerProjectListDlg::UpdateProperty_R(int nPos, int nIdFlag)
{
	if (nIdFlag)
		m_listPropertyView_R.UpdateProperty(_T("RepresentID"), nPos);
	else
		m_listPropertyView_R.UpdateProperty(nPos);
	return true;
}

LRESULT KGObjectEditerProjectListDlg::OnUpdateScene(WPARAM wParam, LPARAM lParam)
{
	::SendMessage(m_pParentWindow->m_hWnd, WM_LIST_UPDATE_SCENE, wParam, lParam);
	return true;
}

LRESULT KGObjectEditerProjectListDlg::OnListItemUpdate(WPARAM wParam, LPARAM lParam)
{
	LPKGLISTITEM pListItem  = (LPKGLISTITEM)wParam;
	USER_ITEM_DATA itemData;

	KGLOG_PROCESS_ERROR(pListItem);

	pListItem->GetStructData(&itemData, sizeof(itemData));

	if (!strcmp(itemData.szPropertyName, _T("RepresentID")))
	{
		UpdateProperty_R(atoi(itemData.szPropertyValue), true);
		::SendMessage(m_pParentWindow->m_hWnd, WM_LIST_UPDATE_SCENE, wParam, lParam);
	}

Exit0:
	::SendMessage(m_pParentWindow->m_hWnd, WM_NOTIFY_ITEM_VALUE_UPDATE, wParam, lParam);
	return true;
}

void KGObjectEditerProjectListDlg::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
	if (m_eShowMode == SHOW_MODEL_BIND)
	{
		m_eShowMode = SHOW_MODEL_FLOAT;
	}
	else if (m_eShowMode == SHOW_MODEL_FLOAT)
	{
		m_eShowMode = SHOW_MODEL_BIND;
	}
	else
	{
		m_eShowMode = SHOW_MODEL_HIDE;
	}
	m_pParentWindow->UpdateInterface();
	CDialog::OnNcLButtonDblClk(nHitTest, point);
}

void KGObjectEditerProjectListDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (m_eShowMode == SHOW_MODEL_BIND)
	{
		m_eShowMode = SHOW_MODEL_FLOAT;
	}
	else if (m_eShowMode == SHOW_MODEL_FLOAT)
	{
		m_eShowMode = SHOW_MODEL_BIND;
	}
	else
	{
		m_eShowMode = SHOW_MODEL_HIDE;
	}
	m_pParentWindow->UpdateInterface();
	CDialog::OnLButtonDblClk(nFlags, point);
}

LRESULT KGObjectEditerProjectListDlg::OnNcHitTest(CPoint point)
{
	LRESULT lHitTest = CDialog::OnNcHitTest(point);

	if (lHitTest == HTCAPTION && m_eShowMode == SHOW_MODEL_BIND)
	{
		lHitTest =  HTCLIENT;
	}
	return lHitTest;
}

HBRUSH KGObjectEditerProjectListDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (pWnd == this)// || pWnd == &m_staticCtrl_R || pWnd == &m_staticCtrl_L)
	{
		//pDC->SetBkColor(RGB(68, 68, 68));
	//	pDC->SetTextColor(RGB(255, 255, 255));
		return CreateSolidBrush(RGB(68, 68, 68));
	}
	return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}

void KGObjectEditerProjectListDlg::OnSize(UINT nType, int cx, int cy)
{
	CRect rectClient;
	CRect rectTabCtl;

	GetClientRect(&rectClient);
	rectClient.bottom += 1;
	m_tabCtrl.MoveWindow(&rectClient);

	rectClient.top += 25;
	m_listPropertyView_L.MoveWindow(&rectClient);
	m_listPropertyView_R.MoveWindow(&rectClient);

	/*CRect rectList_L;
	CRect rectList_R;
	CRect rectStatic;
	CRect rectCombox;
	CRect rectButton;

	int   nWidth  = 0;
	int   nHeight = 0;

	GetClientRect(&rectList_L);
	GetClientRect(&rectList_R);*/

	//rectButton.left   = 1;
	//rectButton.top    = 1;
	//rectButton.right  = rectButton.left   + 32;
	//rectButton.bottom = rectButton.top    + 32;
	//
	////m_bnShowLogicList.MoveWindow(&rectButton);
	//rectButton.left  = rectButton.right   + 4;
	//rectButton.right = rectButton.left    + 32;
	//m_bnShowRepresent.MoveWindow(&rectButton);

	/*rectList_L.top    = rectButton.bottom + 2;
	rectList_R.top    = rectButton.bottom + 2;
	rectList_L.bottom = rectList_L.bottom - rectList_L.Height() / 2;
	rectList_R.top    = rectList_R.top    + rectList_R.Height() / 2 + 2;*/

	/*if (m_bnShowLogicList && m_bnShowRepresent)
	{
		if (m_bnShowLogicList.m_nStateFlag && m_bnShowRepresent.m_nStateFlag)
		{
		}
		else if (m_bnShowLogicList.m_nStateFlag)
		{
			rectList_L.bottom = rectList_R.bottom;
			ZeroMemory(&rectList_R, sizeof(rectList_R));
		}
		else if (m_bnShowRepresent.m_nStateFlag)
		{
			rectList_R.top    = rectList_L.top;
			ZeroMemory(&rectList_L, sizeof(rectList_L));
		}
		else
		{
			ZeroMemory(&rectList_L, sizeof(rectList_L));
			ZeroMemory(&rectList_R, sizeof(rectList_R));
		}
	}

	if (m_listPropertyView_L)
		m_listPropertyView_L.MoveWindow(&rectList_L);
	if (m_listPropertyView_R)
		m_listPropertyView_R.MoveWindow(&rectList_R);

	nWidth = m_listPropertyView_L.GetColumnWidth(0);
	m_listPropertyView_L.SetColumnWidth(1, rectList_L.Width() - (nWidth));
	m_listPropertyView_R.SetColumnWidth(0, nWidth);
	m_listPropertyView_R.SetColumnWidth(1, rectList_R.Width() - (nWidth));*/

}


void KGObjectEditerProjectListDlg::OnLvnColumnclickListProperty(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	SendMessage(WM_LBUTTONDBLCLK);
	*pResult = 0;
}

void KGObjectEditerProjectListDlg::OnCancel()
{
	m_eShowMode = SHOW_MODEL_HIDE;
	ShowWindow(SW_HIDE);
	return;
}

void KGObjectEditerProjectListDlg::OnOK()
{
	return;
}

KGClassButton::KGClassButton()
{
	m_nStateFlag  = false;
	m_nSelFlag	  = true;
	m_hButtonIcon = NULL;
}

KGClassButton::~KGClassButton()
{

}

int KGClassButton::LoadIcon(HICON hIcon)
{
	m_hButtonIcon = hIcon;
	return true;
}

void KGClassButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_nStateFlag)
	{
		m_nStateFlag = false;
	}
	else
	{
		m_nStateFlag = true;
	}
	CButton::OnLButtonUp(nFlags, point);
}

void KGClassButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CClientDC dc(this);
	COLORREF color;
	CRect rect;
	CPen  pen;

	GetClientRect(&rect);
	dc.FillRect(&rect, &CBrush(RGB(255, 255, 255)));
	if (m_nStateFlag)
	{
		color = RGB(255, 2, 2);
	}
	else
	{
		color = RGB(100, 100, 100);
	}

	pen.CreatePen(PS_SOLID, 2, color);
	dc.SelectObject(&pen);

	if (m_hButtonIcon)
	{
		dc.DrawIcon(rect.left, rect.top, m_hButtonIcon);
	}

	if (m_nSelFlag)
	{
		dc.MoveTo(rect.left, rect.top);
		dc.LineTo(rect.right, rect.top);
		dc.LineTo(rect.right, rect.bottom);
		dc.LineTo(rect.left, rect.bottom);
		dc.LineTo(rect.left, rect.top - 2);
	}
}

BEGIN_MESSAGE_MAP(KGClassButton, CButton)
	//ON_WM_PAINT()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()



void KGObjectEditerProjectListDlg::OnBnClickedLogic()
{
	SendMessage(WM_SIZE);
}

void KGObjectEditerProjectListDlg::OnBnClickedRepresent()
{
	SendMessage(WM_SIZE);
}

void KGObjectEditerProjectListDlg::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nSelIndex = m_tabCtrl.GetCurSel();
    if (nSelIndex == 2)
    {
        m_listPropertyView_L.ShowWindow(SW_HIDE);
        m_listPropertyView_R.ShowWindow(SW_HIDE);
    }
	if (nSelIndex == 1)
	{
		m_listPropertyView_L.ShowWindow(SW_HIDE);
		m_listPropertyView_R.ShowWindow(SW_SHOW);
	}
	else
	{
		m_listPropertyView_L.ShowWindow(SW_SHOW);
		m_listPropertyView_R.ShowWindow(SW_HIDE);
	}

	*pResult = 0;
}
