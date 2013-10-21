// KDlg_Create_LVF.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
//#include "KVolumeFog.h"
#include "KDlg_Create_LVF.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////



// KDlg_Create_LVF dialog

IMPLEMENT_DYNAMIC(KDlg_Create_LVF, CDialog)
KDlg_Create_LVF::KDlg_Create_LVF(CWnd* pParent /*=NULL*/)
	: CDialog(KDlg_Create_LVF::IDD, pParent)
{
}

KDlg_Create_LVF::~KDlg_Create_LVF()
{
}

//KVolumeFog* KDlg_Create_LVF::GetVolumeFog(KModelTable* pKMT)
//{
//	if(!pKMT)
//	{
//		return 0;
//	}
//
//	KVolumeFog* pVF = 0;
//
//	pKMT->Get1NewXMesh((LPMODEL*)(&pVF),MODELTYPE_LAYEREDVOLUMETRICFOG);
//
//	pVF->Create(D3DXVECTOR3(0, 0, 0), 500, 500, -10, m_NumOfFPs, D3DCOLOR_ARGB(40, 255, 255, 255), 10, 10, m_Path, m_Filename);
//
//	return pVF;
//}

void KDlg_Create_LVF::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(KDlg_Create_LVF, CDialog)
	ON_BN_CLICKED(IDD_Create_LVF_Create, OnCreateClicked)
	ON_BN_CLICKED(IDD_Create_LVF_Cancel, OnCancelClicked)
	ON_BN_CLICKED(IDD_Create_LVF_Browse, OnBrowseClicked)
	ON_EN_CHANGE(IDD_Create_LVF_FPNUM, OnFPNumChanged)
END_MESSAGE_MAP()


// KDlg_Create_LVF message handlers

void KDlg_Create_LVF::OnCreateClicked()
{
	char sz[256];
	memset(sz, 0, 256);

	GetDlgItemText(IDD_Create_LVF_Name, sz, 256);
	if(sz[0] == '\0')
	{
		MessageBox("名字不能为空！", "错误", MB_OK);
		return;
	}

	m_Filename = sz;
	if(!m_Filename.find('.'))
	{
		m_Filename += string(".lvf");
	}

	memset(sz, 0, 256);
	GetDlgItemText(IDD_Create_LVF_Path, sz, 256);
	if(sz[0] == '\0')
	{
		MessageBox("位置不能为空！", "错误", MB_OK);
		return;
	}
	m_Path = sz;

	if(m_Path.at(m_Path.size() - 1) != '\\')
	{
		m_Path += "\\";
	}

	GetDlgItemText(IDD_Create_LVF_FPNUM, sz, 256);
	if(sz[0] == '\0')
	{
		MessageBox("雾平面个数不能为空!", "错误", MB_OK);
		return;
	}
	//Common::String sFPNum = sz;
	m_NumOfFPs = static_cast<int>(strtod(sz, 0));//sFPNum.IntValue();

	//if(Common::File::IsFileExist(m_Path + m_Filename))
	//{
	//	if(MessageBox("指定的文件已存在，确定要覆盖吗？", "文件存在", MB_OKCANCEL) == IDCANCEL)
	//	{
	//		return;
	//	}
	//}

	EndDialog(IDD_Create_LVF_Create);
}

void KDlg_Create_LVF::OnCancelClicked()
{
	OnClose();
}

void KDlg_Create_LVF::OnClose()
{
	CDialog::OnClose();
	EndDialog(0);
}


void KDlg_Create_LVF::OnPathChanged()
{
}

void KDlg_Create_LVF::OnBrowseClicked()
{
	TCHAR strDriver[MAX_PATH];
	TCHAR strPath[MAX_PATH];
	TCHAR strName[MAX_PATH];
	TCHAR strExt[MAX_PATH];
	TCHAR strFileName[MAX_PATH];

	CFileDialog dlg(false, "lvf", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "体积雾文件 (*.lvf)|*.lvf||");
	if(dlg.DoModal() != IDOK)
	{
		return;
	}

	string sz = dlg.GetPathName();
	_splitpath(sz.c_str(), strDriver, strPath, strName, strExt);
	//sz = sz.substr(0, sz.find_last_of('\\') + 1);
	strcpy(strFileName, strName);
	strcat(strFileName, strExt);
	SetDlgItemText(IDD_Create_LVF_Path, strFileName);

	SetDlgItemText(IDD_Create_LVF_Name, dlg.GetFileName());
}
void KDlg_Create_LVF::OnFPNumChanged()
{
	char szText[256];
	GetDlgItemText(IDD_Create_LVF_FPNUM, szText, 256);

	if(szText[0] == '\0')
	{
		return;
	}

	//Common::String sText = szText;

	m_NumOfFPs = static_cast<unsigned int>(strtod(szText, 0));//sText.IntValue();
}
