// KGMemDateEditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGMemDateEditDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


// KGMemDateEditDlg dialog

IMPLEMENT_DYNAMIC(KGMemDateEditDlg, CDialog)

KGMemDateEditDlg::KGMemDateEditDlg(
	BYTE* pDate, KGMEMDATEELEMENT element[],
	INT nWitdhLeft, INT nWitdhRigh,
	CWnd* pParent
) : CDialog(KGMemDateEditDlg::IDD, pParent)
{
    m_pDateBuffer = pDate;
    m_pElement    = element;

	m_nWitdhLeft = nWitdhLeft;
	m_nWitdhRigh = nWitdhRigh;
	m_cTitleName = "";
}

KGMemDateEditDlg::~KGMemDateEditDlg()
{
    //SAFE_DELETE(m_pDateBuffer);
    //SAFE_DELETE(m_pElement);
}

void KGMemDateEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST, m_list);
}


BEGIN_MESSAGE_MAP(KGMemDateEditDlg, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL KGMemDateEditDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_list.InsertColumn(0, "属性名");
    m_list.InsertColumn(1, "属性值");
    m_list.SetColumnWidth(0, m_nWitdhLeft);
    m_list.SetColumnWidth(1, m_nWitdhRigh);
    m_list.SetBkColor(RGB(92, 92, 92));

    m_list.SetupToolTip(200, RGB(68, 68, 68), RGB(255, 255, 255));
    m_list.SetHoverTime(100);

    if (m_pElement && m_pDateBuffer)
        m_list.BandDate(m_pDateBuffer, m_pElement);
	CString Title = "物件属性 " + m_cTitleName;
	this->SetWindowText(Title);
    return TRUE;
}

void KGMemDateEditDlg::OnSize(UINT nType, int cx, int cy)
{
	if (::IsWindow(m_list.m_hWnd))
	{
		CRect rectClient;
		GetClientRect(&rectClient);

		rectClient.left += 2;
		rectClient.right -= 2;
		rectClient.top += 2;
		rectClient.bottom -= 30;

		m_list.MoveWindow(&rectClient);

		rectClient.top = rectClient.bottom + 4;
		rectClient.bottom = rectClient.top + 25;
		rectClient.left = rectClient.right - 170;
		rectClient.right = rectClient.left + 80;

		GetDlgItem(IDOK)->MoveWindow(&rectClient);

		rectClient.left = rectClient.right + 2;
		rectClient.right = rectClient.left + 80;

		GetDlgItem(IDCANCEL)->MoveWindow(&rectClient);
	}
	CDialog::OnSize(nType, cx, cy);
}

void KGMemDateEditDlg::OnOK()
{
	if (::IsWindow(m_list.m_hWnd))
		m_list.UpdateToBandDate();
	CDialog::OnOK();
}

void KGMemDateEditDlg::SetTitleName(LPCTSTR title)
{
	m_cTitleName = title;
}