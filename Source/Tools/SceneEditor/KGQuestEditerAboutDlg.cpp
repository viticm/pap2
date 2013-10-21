// KGQuestEditerAboutDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGQuestEditerAboutDlg.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

// KGQuestEditerAboutDlg dialog

IMPLEMENT_DYNAMIC(KGQuestEditerAboutDlg, CDialog)

KGQuestEditerAboutDlg::KGQuestEditerAboutDlg(CWnd* pParent /*=NULL*/)
	: CDialog(KGQuestEditerAboutDlg::IDD, pParent)
{

}

KGQuestEditerAboutDlg::~KGQuestEditerAboutDlg()
{
}

BOOL KGQuestEditerAboutDlg::OnInitDialog()
{
	char szFullPath[MAX_PATH];
	g_GetFullPath(szFullPath, QUEST_HELP_FILE);

	CStdioFile fpFile(szFullPath, CFile::modeRead | CFile::typeText);
	CString szText = _T("");
	CDialog::OnInitDialog();

	//KG_PROCESS_ERROR(fpFile);
	while (fpFile.ReadString(szText))
	{
		m_szText = m_szText + szText + _T("\r\n");
	}

	fpFile.Close();
	UpdateData(false);

	return TRUE;
}

void KGQuestEditerAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT, m_szText);
}


BEGIN_MESSAGE_MAP(KGQuestEditerAboutDlg, CDialog)
END_MESSAGE_MAP()


// KGQuestEditerAboutDlg message handlers
