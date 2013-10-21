// KSceneSceneEditorDialogNewMapWizard.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KSceneSceneEditorDialogNewMapWizard.h"
//#include "SceneSkinEdit.h"
//#include "MeshTerran.h"
#include ".\kscenesceneeditordialognewmapwizard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KSceneSceneEditorDialogNewMapWizard dialog

KSceneSceneEditorDialogNewMapWizard::KSceneSceneEditorDialogNewMapWizard(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneSceneEditorDialogNewMapWizard::IDD, pParent)
{
	//{{AFX_DATA_INIT(KSceneSceneEditorDialogNewMapWizard)
	m_CStringMapSize = _T("");
	m_CStringMapDescribe = _T("");
	m_CString_DefaultTexture = _T("");
	m_fGroundHeight = 0.0f;
	m_CString_IniFile = _T("");
	//}}AFX_DATA_INIT
	m_dwWidth  = 128;
	m_dwHeight = 128;

	//m_lpSceneSceneEditor = NULL;
}


void KSceneSceneEditorDialogNewMapWizard::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KSceneSceneEditorDialogNewMapWizard)
	DDX_Control(pDX, IDC_LIST_TEXTURE, m_ListCtrlTexture);
	DDX_Control(pDX, IDC_COMBO_MAP_WIDTH, m_ComboWidth);
	DDX_Control(pDX, IDC_COMBO_MAP_HEIGHT, m_ComboHeight);
	DDX_Text(pDX, IDC_STATIC_MAP_SIZE, m_CStringMapSize);
	DDX_Text(pDX, IDC_STATIC_MAP_DESCRIBE, m_CStringMapDescribe);
	DDX_Text(pDX, IDC_STATIC_DEFAULT_TEXTURE, m_CString_DefaultTexture);
	DDX_Text(pDX, IDC_EDIT_GROUND_HEIGHT, m_fGroundHeight);
	DDX_Text(pDX, IDC_EDIT_INIFILE, m_CString_IniFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KSceneSceneEditorDialogNewMapWizard, CDialog)
	//{{AFX_MSG_MAP(KSceneSceneEditorDialogNewMapWizard)
	ON_CBN_SELCHANGE(IDC_COMBO_MAP_HEIGHT, OnSelchangeComboMapHeight)
	ON_CBN_SELCHANGE(IDC_COMBO_MAP_WIDTH, OnSelchangeComboMapWidth)
	ON_NOTIFY(NM_CLICK, IDC_LIST_TEXTURE, OnClickListTexture)
	ON_EN_CHANGE(IDC_EDIT_GROUND_HEIGHT, OnChangeEditGroundHeight)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_BROWNINI, OnBnClickedButtonBrownini)
	ON_BN_CLICKED(IDC_BUTTON_NEWINI, OnBnClickedButtonNewini)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_TEXTURE, OnLvnItemchangedListTexture)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KSceneSceneEditorDialogNewMapWizard message handlers

void KSceneSceneEditorDialogNewMapWizard::FillGroundSetCombo()
{
	//if(!m_lpSceneSceneEditor)
	//	return;	
	//m_CString_IniFile.Format("%s",m_lpSceneSceneEditor->m_lpMapMesh->m_scMaterialIniFile.c_str());
	/*TCHAR path_buffer[256];
	TCHAR drive[256];
	TCHAR dir[256];
	TCHAR fname[256];
	TCHAR ext[256];

	WIN32_FIND_DATA Find;
	TCHAR str[256];
	wsprintf(str,"%s地表贴图\\*.ini",g_Def_WorkDirectory);
	HANDLE hFind = FindFirstFile(str,&Find);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		wsprintf(path_buffer,"%s",Find.cFileName);
		_splitpath( path_buffer, drive, dir, fname, ext );
		wsprintf(str,"%s%s",fname,ext);

		while(FindNextFile(hFind,&Find))
		{
			wsprintf(path_buffer,"%s",Find.cFileName);
			_splitpath( path_buffer, drive, dir, fname, ext );
			wsprintf(str,"%s%s",fname,ext);
		}
		FindClose(hFind);
	}*/

}

BOOL KSceneSceneEditorDialogNewMapWizard::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	//for(int i=4;i<129;i+=4)
	//{
	//	TCHAR Str[256];
	//	sprintf(Str,"%d",i*32);

	//	m_ComboHeight.AddString(Str);
	//	m_ComboWidth .AddString(Str);
	//}
	//// TODO: Add extra initialization here
	//m_ComboHeight.SetCurSel(0);
	//m_ComboWidth .SetCurSel(0);

	//m_dwWidth  = (m_ComboWidth .GetCurSel() + 1) * 128;
	//m_dwHeight = (m_ComboHeight.GetCurSel() + 1) * 128;

	//m_CStringMapSize.Format("地图尺寸:%d米 X %d米",m_dwWidth/2,m_dwHeight/2);
	//
	//m_CStringMapDescribe.Format("地图描述:微型地图");

	//FillGroundSetCombo();


	//FillGroundTextureList();

	//if(!m_lpSceneSceneEditor)
	//	return TRUE;

	//m_fGroundHeight = m_lpSceneSceneEditor->m_lpMapMesh->m_fHeightValue;

	//UpdateData(FALSE);
	//
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneSceneEditorDialogNewMapWizard::OnSelchangeComboMapHeight() 
{
	// TODO: Add your control notification handler code here
	m_dwWidth  = (m_ComboWidth .GetCurSel() + 1) * 128;
	m_dwHeight = (m_ComboHeight.GetCurSel() + 1) * 128;

	m_CStringMapSize.Format("地图尺寸:%d米 X %d米",m_dwWidth/2,m_dwHeight/2);

	int K = m_dwWidth*m_dwHeight;
	if(K<=256*256)
		m_CStringMapDescribe.Format("地图描述:　微型地图");
	else if(K<=800*800)
		m_CStringMapDescribe.Format("地图描述:　小型地图");
	else if(K<=1500*1500)
		m_CStringMapDescribe.Format("地图描述:　中型地图");
	else if(K<=2500*2500)
		m_CStringMapDescribe.Format("地图描述:　大型地图");
	else if(K<=3400*3400)
		m_CStringMapDescribe.Format("地图描述:　巨型地图");
	else
		m_CStringMapDescribe.Format("地图描述:　超巨型地图");

	UpdateData(FALSE);
}

void KSceneSceneEditorDialogNewMapWizard::OnSelchangeComboMapWidth() 
{
	// TODO: Add your control notification handler code here
	m_dwWidth  = (m_ComboWidth .GetCurSel() + 1) * 128;
	m_dwHeight = (m_ComboHeight.GetCurSel() + 1) * 128;

	m_CStringMapSize.Format("地图尺寸:%d米 X %d米",m_dwWidth/2,m_dwHeight/2);

	int K = m_dwWidth*m_dwHeight;
	if(K<=256*256)
		m_CStringMapDescribe.Format("地图描述:　微型地图");
	else if(K<=800*800)
		m_CStringMapDescribe.Format("地图描述:　小型地图");
	else if(K<=1500*1500)
		m_CStringMapDescribe.Format("地图描述:　中型地图");
	else if(K<=2500*2500)
		m_CStringMapDescribe.Format("地图描述:　大型地图");
	else if(K<=3400*3400)
		m_CStringMapDescribe.Format("地图描述:　巨型地图");
	else
		m_CStringMapDescribe.Format("地图描述:　超巨型地图");

	UpdateData(FALSE);
}

//void KSceneSceneEditorDialogNewMapWizard::SetSceneEditor(KSceneSceneEdit* pSceneEdit)
//{
//	m_lpSceneSceneEditor = pSceneEdit;
//}

void KSceneSceneEditorDialogNewMapWizard::FillGroundTextureList()
{
	m_ListCtrlTexture.DeleteAllItems();

	/*if(!m_lpSceneSceneEditor)
		return;	*/

	//for(DWORD i=0;i<m_lpSceneSceneEditor->m_lpMapMesh->m_vecGroundTextGroups.size();i++)
	//{
	//	KModelTerrain::_GroundTextureGroup* pGroup =
	//		m_lpSceneSceneEditor->m_lpMapMesh->m_vecGroundTextGroups[i];

	//	TCHAR str[256];
	//	wsprintf(str,"%s",pGroup->SetName);
	//	int index = m_ListCtrlTexture.InsertItem(i,str,0);	
	//	m_ListCtrlTexture.SetItemData(index,pGroup->vecTextureIndex[0]);
	//}
}

void KSceneSceneEditorDialogNewMapWizard::OnClickListTexture(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	//if(!m_lpSceneSceneEditor)
	//	return;	

	//POSITION pos = m_ListCtrlTexture.GetFirstSelectedItemPosition();
	//while (pos)
	//{
	//	int index = m_ListCtrlTexture.GetNextSelectedItem(pos);
	//	int TextureIndex = m_ListCtrlTexture.GetItemData(index);
	//	
	//	KModelTerrain::_GroundTextureGroup* pGroup =
	//		m_lpSceneSceneEditor->m_lpMapMesh->m_vecGroundTextGroups[index];

	//	m_CString_DefaultTexture.Format("初始地表：　%s",pGroup->SetName);

	//	m_lpSceneSceneEditor->m_lpMapMesh->m_nDefaultGroundTextureIndex =
	//		pGroup->vecTextureIndex[0];

	//	UpdateData(FALSE);
	//}
	*pResult = 0;
}

void KSceneSceneEditorDialogNewMapWizard::OnChangeEditGroundHeight() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	//if(!m_lpSceneSceneEditor)
	//	return;	
	// m_lpSceneSceneEditor->m_lpMapMesh->m_fHeightValue = m_fGroundHeight;

}

void KSceneSceneEditorDialogNewMapWizard::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void KSceneSceneEditorDialogNewMapWizard::OnBnClickedButtonBrownini()
{
	// TODO: Add your control notification handler code here
}

void KSceneSceneEditorDialogNewMapWizard::OnBnClickedButtonNewini()
{
	// TODO: Add your control notification handler code here
	//TCHAR str[256];
	//wsprintf(str,"%s地表贴图\\*.ini",g_szDefWorkDirectory);
}

void KSceneSceneEditorDialogNewMapWizard::OnLvnItemchangedListTexture(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}
