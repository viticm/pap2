// KSceneEditorAboutBox.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneEditorAboutBox.h"
#include<fstream>

// KSceneEditorAboutBox dialog

IMPLEMENT_DYNAMIC(KSceneEditorAboutBox, CDialog)

KSceneEditorAboutBox::KSceneEditorAboutBox(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneEditorAboutBox::IDD, pParent)
	, m_strVersion(_T(""))
{

}

KSceneEditorAboutBox::~KSceneEditorAboutBox()
{
}
BOOL KSceneEditorAboutBox::OnInitDialog() 
{
	CDialog::OnInitDialog();
	// TODO: Add extra initialization here
	TCHAR FileName[MAX_PATH];
	TCHAR Text[MAX_PATH];
	CString strText;
	CString strVerName = "Sword3.version=";
	sprintf_s(FileName,"%s\\version.cfg",g_szDefWorkDirectory);
	ifstream in(FileName);
	while(!in.eof())
	{
		in.getline(Text,MAX_PATH);
		strText =Text;
		int nIndex = strText.Find(strVerName);
		if(nIndex != 0)
			continue;
		
		m_strVersion.Format("version : %s", strText.Right(strText.GetLength() - strVerName.GetLength()));
		UpdateData(FALSE); 
		break;
	} 
	in.close();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
void KSceneEditorAboutBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_VERSION, m_strVersion);
}


BEGIN_MESSAGE_MAP(KSceneEditorAboutBox, CDialog)
END_MESSAGE_MAP()


// KSceneEditorAboutBox message handlers
