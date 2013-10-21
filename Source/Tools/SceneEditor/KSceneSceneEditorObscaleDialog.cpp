// KSceneSceneEditorObscaleDialog.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KSceneSceneEditorObscaleDialog.h"
#include ".\kscenesceneeditorobscaledialog.h"
//#include "SceneSkinEdit.h"
//#include "MeshTerran.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KSceneSceneEditorObscaleDialog dialog


KSceneSceneEditorObscaleDialog::KSceneSceneEditorObscaleDialog(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneSceneEditorObscaleDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(KSceneSceneEditorObscaleDialog)
	//}}AFX_DATA_INIT
	m_lpSceneSceneEditor = NULL;
}


void KSceneSceneEditorObscaleDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KSceneSceneEditorObscaleDialog)
	DDX_Control(pDX, IDC_LIST2, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KSceneSceneEditorObscaleDialog, CDialog)
	//{{AFX_MSG_MAP(KSceneSceneEditorObscaleDialog)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CHECK3, OnBnClickedCheck3)
	ON_BN_CLICKED(IDC_CHECK11, OnBnClickedCheck11)
	ON_BN_CLICKED(IDC_CHECK12, OnBnClickedCheck12)
	ON_LBN_SELCHANGE(IDC_LIST2, OnLbnSelchangeList2)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_CHECK1, OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_CHECK2, OnBnClickedCheck2)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KSceneSceneEditorObscaleDialog message handlers

void KSceneSceneEditorObscaleDialog::OnBnClickedCheck3()
{
	//// TODO: Add your control notification handler code here
	//if(!m_lpSceneSceneEditor)
	//	return;

	//if(((CButton*)GetDlgItem(IDC_CHECK3))->GetCheck())
	//{
	//	m_lpSceneSceneEditor->m_lpMapMesh->m_Cell4Brush.m_Obstacle.m_PersonObstacle = cotAll;
	//}
	//else
	//{
	//	m_lpSceneSceneEditor->m_lpMapMesh->m_Cell4Brush.m_Obstacle.m_PersonObstacle = cotNone;
	//}

	//((CButton*)GetDlgItem(IDC_CHECK2))->SetCheck(0);
	//m_lpSceneSceneEditor->m_lpMapMesh->m_CellOperator = 0;
}

void KSceneSceneEditorObscaleDialog::OnBnClickedCheck11()
{
	// TODO: Add your control notification handler code here

}

void KSceneSceneEditorObscaleDialog::OnBnClickedCheck12()
{
	// TODO: Add your control notification handler code here
	/*if(!m_lpSceneSceneEditor)
		return;

	if(((CButton*)GetDlgItem(IDC_CHECK12))->GetCheck())
	{
		m_lpSceneSceneEditor->m_lpMapMesh->m_Cell4Brush.m_Obstacle.m_bBlockMissile = true;
	}
	else
	{
		m_lpSceneSceneEditor->m_lpMapMesh->m_Cell4Brush.m_Obstacle.m_bBlockMissile = false;
	}
	((CButton*)GetDlgItem(IDC_CHECK2))->SetCheck(0);
	m_lpSceneSceneEditor->m_lpMapMesh->m_CellOperator = 0;*/
}

void KSceneSceneEditorObscaleDialog::OnLbnSelchangeList2()
{
	//// TODO: Add your control notification handler code here
	//if(!m_lpSceneSceneEditor)
	//	return;
	//int Index = m_List.GetCurSel();
	//if(Index>=0)
	//{
	//	DWORD K = m_List.GetItemData(Index);
	//	((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck(0);
	//	m_lpSceneSceneEditor->m_lpMapMesh->m_CellOperator = 1;
	//	m_lpSceneSceneEditor->m_lpMapMesh->m_Cell4Brush.m_dwScriptID = K + 1;
	//}
}

void KSceneSceneEditorObscaleDialog::FillList()
{
	/*{
		m_List.SetCurSel(-1);
		int Count = m_List.GetCount();
		for(int i=Count+1;i>=0;i--)
		{
			m_List.DeleteString(i);
		}
	}

	if(!m_lpSceneSceneEditor)
		return;

	DWORD Size = m_lpSceneSceneEditor->m_lpMapMesh->m_vecScriptFile.size();
	for(DWORD i=0;i<Size;i++)
	{
		TCHAR Name[256];
		wsprintf(Name,"%d %s",i+1,m_lpSceneSceneEditor->m_lpMapMesh->m_vecScriptFile[i].c_str());
		int Index = m_List.AddString(Name);
		m_List.SetItemData(Index,i);
	}*/
}

void KSceneSceneEditorObscaleDialog::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	//if(!m_lpSceneSceneEditor)
	//	return;

	//char bigBuff[800] = "";  // maximum common dialog buffer size
	//char szFilter[] ="脚本文件 (*.*)|*.*||";

	//CFileDialog dlg(true, NULL, NULL,
	//	OFN_HIDEREADONLY , szFilter);
	//// Modify OPENFILENAME members directly to point to bigBuff
	//dlg.m_ofn.lpstrFile = bigBuff;
	//dlg.m_ofn.nMaxFile = sizeof(bigBuff);
	//if ( dlg.DoModal() == IDOK)
	//{
	//	TCHAR Name[256];
	//	wsprintf(Name,"%s",dlg.GetPathName());
	//	m_lpSceneSceneEditor->m_lpMapMesh->m_vecScriptFile.push_back(Name);
	//}

	//FillList();
}

void KSceneSceneEditorObscaleDialog::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	//if(!m_lpSceneSceneEditor)
	//	return;
	//int Index = m_List.GetCurSel();
	//if(Index)
	//{
	//	DWORD K = m_List.GetItemData(Index);

	//	char bigBuff[800] = "";  // maximum common dialog buffer size
	//	char szFilter[] ="脚本文件 (*.*)|*.*||";

	//	CFileDialog dlg(true, NULL, NULL,
	//		OFN_HIDEREADONLY , szFilter);
	//	// Modify OPENFILENAME members directly to point to bigBuff
	//	dlg.m_ofn.lpstrFile = bigBuff;
	//	dlg.m_ofn.nMaxFile = sizeof(bigBuff);
	//	if ( dlg.DoModal() == IDOK)
	//	{
	//		TCHAR Name[256];
	//		wsprintf(Name,"%s",dlg.GetPathName());
	//		m_lpSceneSceneEditor->m_lpMapMesh->m_vecScriptFile[K] = Name;
	//	}
	//}

	//FillList();

}

void KSceneSceneEditorObscaleDialog::OnBnClickedCheck1()
{
	// TODO: Add your control notification handler code here
	/*((CButton*)GetDlgItem(IDC_CHECK2))->SetCheck(0);
	if(!m_lpSceneSceneEditor)
		return;
	m_lpSceneSceneEditor->m_lpMapMesh->m_CellOperator = 0;*/
}

void KSceneSceneEditorObscaleDialog::OnBnClickedCheck2()
{
	// TODO: Add your control notification handler code here
	/*((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck(0);
	if(!m_lpSceneSceneEditor)
		return;
	m_lpSceneSceneEditor->m_lpMapMesh->m_CellOperator = 1;*/

}

BOOL KSceneSceneEditorObscaleDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	//FillList();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
