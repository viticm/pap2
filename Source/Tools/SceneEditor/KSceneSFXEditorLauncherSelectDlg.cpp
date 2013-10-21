////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSFXEditorLauncherSelectDlg.cpp
//  Version     : 1.0
//  Creator     : Zeb
//  Create Date : 2005-11-1 14:37:04
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KSceneSFXEditorLauncherSelectDlg.h"


////////////////////////////////////////////////////////////////////////////////

// KSceneSFXEditorLauncherSelectDlg.cpp : implementation file
//

#include "SceneEditor.h"
#include ".\kscenesfxeditorlauncherselectdlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


// KSceneSFXEditorLauncherSelectDlg dialog

IMPLEMENT_DYNAMIC(KSceneSFXEditorLauncherSelectDlg, CDialog)
KSceneSFXEditorLauncherSelectDlg::KSceneSFXEditorLauncherSelectDlg(
    IEKG3DSFX *pSFX, CWnd* pParent /*=NULL*/
) : CDialog(KSceneSFXEditorLauncherSelectDlg::IDD, pParent)
{
    ASSERT(pSFX);
    m_pSFX = pSFX;
}

KSceneSFXEditorLauncherSelectDlg::~KSceneSFXEditorLauncherSelectDlg()
{
}

void KSceneSFXEditorLauncherSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_LIST_SELECT_LAUNCHER, m_listLauncher);
}


BEGIN_MESSAGE_MAP(KSceneSFXEditorLauncherSelectDlg, CDialog)
END_MESSAGE_MAP()


// KSceneSFXEditorLauncherSelectDlg message handlers

BOOL KSceneSFXEditorLauncherSelectDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  Add extra initialization here
    m_listLauncher.InsertColumn(0, _T("发射器列表："),	LVCFMT_CENTER, 150);

    m_listLauncher.SetExtendedStyle(
        m_listLauncher.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES
    );

    ASSERT(m_pSFX);

    DWORD dwLauncherNum = m_pSFX->GetParticleLauncherNum();
    m_listLauncher.DeleteAllItems();
    while (dwLauncherNum > 0)
    {
        dwLauncherNum--;

        CString str;
        str.Format("发射器%d", dwLauncherNum + 1);
        m_listLauncher.InsertItem(0, str);
    }

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneSFXEditorLauncherSelectDlg::OnOK()
{
    // TODO: Add your specialized code here and/or call the base class
    ASSERT(m_pSFX);
    m_vecSelectedLauncher.clear();
    POSITION pos = m_listLauncher.GetFirstSelectedItemPosition();
    while (pos)
    {
        int nSelect = m_listLauncher.GetNextSelectedItem(pos);
        IEKG3DSFXParticleLauncher *pLauncher = NULL;
        m_pSFX->GetParticleLauncher(nSelect, &pLauncher);
        ASSERT(pLauncher);

        m_vecSelectedLauncher.push_back(pLauncher);
    }

    CDialog::OnOK();
}

std::vector<IEKG3DSFXParticleLauncher*>* KSceneSFXEditorLauncherSelectDlg::GetSelectedLauncherList()
{
    return &m_vecSelectedLauncher;
}