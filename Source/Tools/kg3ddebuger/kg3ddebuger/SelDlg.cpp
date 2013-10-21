// SelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "kg3ddebuger.h"
#include "SelDlg.h"


// CSelDlg dialog

IMPLEMENT_DYNAMIC(CSelDlg, CDialog)

CSelDlg::CSelDlg(vector<tstring> procName, CWnd* pParent /* = NULL */)
	: CDialog(CSelDlg::IDD, pParent), 
      m_SelProc(-1)
{
    for (size_t i = 0; i < procName.size(); ++i)
    {
        LPCTSTR t = _tcsrchr(procName[i].c_str(), TEXT('\\'));
        if (t)
            m_ProcName.push_back(++t);
        else
            m_ProcName.push_back(procName[i]);
    }
}

CSelDlg::~CSelDlg()
{
}

void CSelDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST, m_listProcess);
}


BEGIN_MESSAGE_MAP(CSelDlg, CDialog)
    ON_BN_CLICKED(IDOK, &CSelDlg::OnBnClickedOk)
    ON_LBN_SELCHANGE(IDC_LIST, &CSelDlg::OnLbnSelchangeList)
    ON_LBN_DBLCLK(IDC_LIST, &CSelDlg::OnLbnDblclkList)
END_MESSAGE_MAP()

BOOL CSelDlg::OnInitDialog()
{
    BOOL res = CDialog::OnInitDialog();

    for (size_t i = 0; i < m_ProcName.size(); ++i)
        m_listProcess.AddString(m_ProcName[i].c_str());

    if (m_ProcName.size() > 0)
    {
        m_listProcess.SetCurSel(0);
        m_SelProc = 0;
    }

    return res;
}
// CSelDlg message handlers

void CSelDlg::OnOK()
{
    m_SelProc = m_listProcess.GetCurSel();

    return CDialog::OnOK();
}

void CSelDlg::OnCancel()
{
    m_SelProc = -1;

    return CDialog::OnCancel();
}

void CSelDlg::OnBnClickedOk()
{
    
    OnOK();
}

void CSelDlg::OnLbnSelchangeList()
{
    // TODO: Add your control notification handler code here
}

void CSelDlg::OnLbnDblclkList()
{
    m_SelProc = m_listProcess.GetCurSel();

    if (m_SelProc >= 0)
        return CDialog::OnOK();
}
