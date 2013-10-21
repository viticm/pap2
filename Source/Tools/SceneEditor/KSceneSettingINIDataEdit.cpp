// KSceneSettingINIDataEdit.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSettingINIDataEdit.h"
//#include "MathTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// KSceneSettingINIDataEdit dialog

IMPLEMENT_DYNAMIC(KSceneSettingINIDataEdit, CDialog)

KSceneSettingINIDataEdit::KSceneSettingINIDataEdit(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneSettingINIDataEdit::IDD, pParent)
	, m_csTitle(_T(""))
	, m_csEditData(_T(""))
	, m_bHasMySelfChanged(FALSE)
{

}

KSceneSettingINIDataEdit::~KSceneSettingINIDataEdit()
{
}

void KSceneSettingINIDataEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SS_DATATITLE_STATIC, m_csTitle);
	DDX_Text(pDX, IDC_SS_ED_DATA, m_csEditData);
}


BEGIN_MESSAGE_MAP(KSceneSettingINIDataEdit, CDialog)
	ON_EN_CHANGE(IDC_SS_ED_DATA, &KSceneSettingINIDataEdit::OnEnChangeSsEdData)
END_MESSAGE_MAP()

VOID KSceneSettingINIDataEdit::SetTitle( LPCTSTR tczTitle )
{
	/*
	CWnd* pWndTemp = GetDlgItem(IDC_SS_DATATITLE_STATIC);
		pWndTemp->SetWindowText(tczTitle);*/
	_ASSERTE(tczTitle);
	m_csTitle = tczTitle;
	UpdateData(FALSE);
}

VOID KSceneSettingINIDataEdit::StringData( LPCTSTR tczData )
{
	/*
	CWnd* pWndTemp = GetDlgItem(IDC_SS_ED_DATA);
		pWndTemp->SetWindowText((LPCTSTR)tczData);*/
	_ASSERTE(tczData);
	m_csEditData = tczData;
	UpdateData(FALSE);
}

const CString& KSceneSettingINIDataEdit::StringData()
{
	CString csTemp = m_csEditData;
	UpdateData();
	if (csTemp != m_csEditData)
	{
		m_bHasMySelfChanged = TRUE;
	}
	else
	{
		m_bHasMySelfChanged = FALSE;
	}
	return m_csEditData;
}
// KSceneSettingINIDataEdit message handlers

VOID KSceneSettingINIDataEdit::Init( LPCSTR tczTitle, LPCSTR tczData )
{
	_ASSERTE(tczTitle&&tczData);
	m_csTitle = tczTitle;
	m_csEditData = tczData;
	UpdateData(FALSE);
}
void KSceneSettingINIDataEdit::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	///不让调用基类的OnOk
	CWnd* pPrarent = GetParent();
	_ASSERTE(pPrarent);
	::PostMessage(pPrarent->GetSafeHwnd(), MSG_CHILD_WINDOW_CLOSE, 0, 0);
	//CDialog::OnOK();
}

void KSceneSettingINIDataEdit::OnEnChangeSsEdData()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	m_bHasMySelfChanged = TRUE;
}
