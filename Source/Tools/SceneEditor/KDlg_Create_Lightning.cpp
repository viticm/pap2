// KDlg_Create_Lightning.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
//#include "KLightning.h"
#include "KDlg_Create_Lightning.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////



// KDlg_Create_Lightning dialog

IMPLEMENT_DYNAMIC(KDlg_Create_Lightning, CDialog)
KDlg_Create_Lightning::KDlg_Create_Lightning(CWnd* pParent /*=NULL*/)
	: CDialog(KDlg_Create_Lightning::IDD, pParent)
{
}

KDlg_Create_Lightning::~KDlg_Create_Lightning()
{
}

//KLightning* KDlg_Create_Lightning::GetLightning(KModelTable* pKMT)
//{
//	KLightning* pRet = 0;
//	if(pKMT)
//	{
//		pKMT->Get1NewXMesh((LPMODEL*)(&pRet),MODELTYPE_DYNAMICLIGHTNING);
//		if(pRet)
//		{
//			pRet->Init(m_MaxNumOfNodes, 0, 100, m_Path, m_Name);
//		}
//	}
//	return pRet;
//}

void KDlg_Create_Lightning::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(KDlg_Create_Lightning, CDialog)
	ON_BN_CLICKED(IDD_CREATE_LIGHTNING_BROWSE, OnBrowseClicked)
	ON_BN_CLICKED(IDOK, OnOkClicked)
	ON_BN_CLICKED(IDCANCEL, OnCancelClicked)
END_MESSAGE_MAP()


// KDlg_Create_Lightning message handlers

void KDlg_Create_Lightning::OnBrowseClicked()
{
	TCHAR strDrive[MAX_PATH];
	TCHAR strPath[MAX_PATH];
	TCHAR strFile[MAX_PATH];
	TCHAR strExt[MAX_PATH];
	TCHAR strFileName[MAX_PATH];

	CFileDialog dlg(false, "dlf", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "闪电文件 (*.dlf)|*.dlf||");
	if(dlg.DoModal() != IDOK)
	{
		return;
	}


	CString sz = dlg.GetPathName();
	_splitpath(sz, strDrive, strPath, strFile, strExt);
	strcpy(strFileName, strFileName);
	strcat(strFileName, strExt);

	SetDlgItemText(IDD_CREATE_LIGHTNING_PATH, strFileName);

	SetDlgItemText(IDD_CREATE_LIGHTNING_NAME, dlg.GetFileName());
}

void KDlg_Create_Lightning::OnOkClicked()
{
	char sz[256];
	GetDlgItemText(IDD_CREATE_LIGHTNING_NAME, sz, 256);
	if(sz[0] == '\0')
	{
		MessageBox("没有指定文件！", "错误", MB_OK | MB_ICONSTOP);
		return;
	}
	m_Name = sz;

	GetDlgItemText(IDD_CREATE_LIGHTNING_PATH, sz, 256);
	if(sz[0] == '\0')
	{
		MessageBox("没有指定文件！", "错误", MB_OK | MB_ICONSTOP);
		return;
	}
	m_Path = sz;

	GetDlgItemText(IDD_CREATE_LIGHTNING_MAXNODENUM, sz, 256);
	if(sz[0] == '\0')
	{
		MessageBox("没有指定闪电结点最大数目\n将采用默认值：10", "警告", MB_OK | MB_ICONINFORMATION);
		m_MaxNumOfNodes = 10;
	}
	else
	{
		TCHAR* pEnd = NULL;
		m_MaxNumOfNodes = static_cast<int>(strtod(sz, &pEnd));
	}

	EndDialog(IDOK);
}

void KDlg_Create_Lightning::OnCancelClicked()
{
	OnClose();
}

void KDlg_Create_Lightning::OnClose()
{
	CDialog::OnClose();
	EndDialog(0);
}