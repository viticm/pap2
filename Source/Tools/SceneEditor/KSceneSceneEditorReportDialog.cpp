// KSceneSceneEditorReportDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSceneEditorReportDialog.h"
#include "KG3DReport.h"

// KSceneSceneEditorReportDialog dialog

IMPLEMENT_DYNAMIC(KSceneSceneEditorReportDialog, CDialog)

KSceneSceneEditorReportDialog::KSceneSceneEditorReportDialog(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneSceneEditorReportDialog::IDD, pParent)
	, m_Cstring(_T(""))
{

}

KSceneSceneEditorReportDialog::~KSceneSceneEditorReportDialog()
{
}

void KSceneSceneEditorReportDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT2, m_Edit);
	DDX_Text(pDX, IDC_EDIT2, m_Cstring);
}


BEGIN_MESSAGE_MAP(KSceneSceneEditorReportDialog, CDialog)
	ON_EN_CHANGE(IDC_EDIT2, &KSceneSceneEditorReportDialog::OnEnChangeEdit2)
END_MESSAGE_MAP()


// KSceneSceneEditorReportDialog message handlers


BOOL KSceneSceneEditorReportDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	TCHAR szName[MAX_PATH];
	int Num = g_cReport.GetNumReport();
	for (int n=0;n<Num;n++)
	{
		g_cReport.GetReport(n,szName);
		AddLine(0,szName);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneSceneEditorReportDialog::AddLine(int nLine,LPCTSTR pName)
{
	m_Cstring += pName;
	m_Cstring += "\r\n";
	UpdateData(FALSE);
}
void KSceneSceneEditorReportDialog::OnEnChangeEdit2()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}
