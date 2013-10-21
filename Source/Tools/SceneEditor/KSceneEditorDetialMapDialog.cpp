// KSceneEditorDetialMapDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneEditorDetialMapDialog.h"
#include "IEKG3DTerrain.h"
#include "IEEditor.h"
// KSceneEditorDetialMapDialog dialog
#include "KSceneEditorDialogAutoGrass.h"
IMPLEMENT_DYNAMIC(KSceneEditorDetialMapDialog, CDialog)

KSceneEditorDetialMapDialog::KSceneEditorDetialMapDialog(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneEditorDetialMapDialog::IDD, pParent)
	, m_fScaleU(0)
	, m_fScaleV(0)
	, m_byCurTexIndex(0)
	, m_fEnv(0)
	, m_fSpecular(0)
	, m_fEmissive(0)
{
	m_lpSceneEditor = NULL;
	m_lpSceneOutDoorSpaceMgr = NULL;
	m_byTexCoordIndex = 0;
	memset(m_byGrassTexIndex,255,sizeof(m_byGrassTexIndex));
	memset(m_byPatternIndex,255,sizeof(m_byPatternIndex));
}

KSceneEditorDetialMapDialog::~KSceneEditorDetialMapDialog()
{
}

void KSceneEditorDetialMapDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TEXT, m_ListBox_Text);
	DDX_Control(pDX, IDC_STATIC_PREVIEW, m_StaticPreview);
	DDX_Control(pDX, IDC_BUTTON1, m_BrushMap[0]);
	DDX_Control(pDX, IDC_BUTTON2, m_BrushMap[1]);
	DDX_Control(pDX, IDC_BUTTON3, m_BrushMap[2]);
	DDX_Control(pDX, IDC_BUTTON4, m_BrushMap[3]);
	DDX_Control(pDX, IDC_BUTTON5, m_BrushMap[4]);
	DDX_Control(pDX, IDC_BUTTON6, m_BrushMap[5]);
	DDX_Control(pDX, IDC_BUTTON7, m_BrushMap[6]);
	DDX_Control(pDX, IDC_BUTTON8, m_BrushMap[7]);
	DDX_Control(pDX, IDC_BUTTON9, m_BrushMap[8]);
	DDX_Control(pDX, IDC_BUTTON10, m_BrushMap[9]);
	DDX_Control(pDX, IDC_BUTTON_LAYER0, m_firstLayer);
	DDX_Text(pDX, IDC_EDIT_SCALEU, m_fScaleU);
	DDX_Text(pDX, IDC_EDIT_SCALEV, m_fScaleV);
	DDX_Check(pDX,IDC_CHECK_AUTOGRASS,m_bAssociateGrass);
	DDX_Text(pDX, IDC_EDIT_ENV, m_fEnv);
	DDX_Text(pDX, IDC_EDIT_SPECULAR, m_fSpecular);
	DDX_Text(pDX, IDC_EDIT_EMISSIVE, m_fEmissive);
}


BEGIN_MESSAGE_MAP(KSceneEditorDetialMapDialog, CDialog)
	ON_LBN_SELCHANGE(IDC_LIST_TEXT, &KSceneEditorDetialMapDialog::OnLbnSelchangeListText)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_ADDTEXTURE, &KSceneEditorDetialMapDialog::OnBnClickedButtonAddtexture)
	ON_BN_CLICKED(IDC_BUTTON_DELTEXTURE, &KSceneEditorDetialMapDialog::OnBnClickedButtonDeltexture)
	ON_BN_CLICKED(IDC_RADIO_XZ, &KSceneEditorDetialMapDialog::OnBnClickedRadioXz)
	ON_BN_CLICKED(IDC_RADIO_XY, &KSceneEditorDetialMapDialog::OnBnClickedRadioXy)
	ON_BN_CLICKED(IDC_RADIO_YZ, &KSceneEditorDetialMapDialog::OnBnClickedRadioYz)
	ON_BN_CLICKED(IDC_BUTTON_LAYER0, &KSceneEditorDetialMapDialog::OnBnClickedButtonLayer0)
	ON_EN_CHANGE(IDC_EDIT_SCALEU, &KSceneEditorDetialMapDialog::OnEnChangeEditScaleu)
	ON_EN_CHANGE(IDC_EDIT_SCALEV, &KSceneEditorDetialMapDialog::OnEnChangeEditScalev)
	ON_BN_CLICKED(IDC_CHANGETEX, &KSceneEditorDetialMapDialog::OnBnClickedChangetex)
	ON_BN_CLICKED(IDC_CHECK_AUTOGRASS, &KSceneEditorDetialMapDialog::OnBnClickedCheckAutograss)
	ON_BN_CLICKED(IDC_BUTTON_FILTER, &KSceneEditorDetialMapDialog::OnBnClickedButtonFilter)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT, &KSceneEditorDetialMapDialog::OnBnClickedButtonExport)
	ON_EN_CHANGE(IDC_EDIT_ENV, &KSceneEditorDetialMapDialog::OnEnChangeEditEnv)
	ON_EN_CHANGE(IDC_EDIT_SPECULAR, &KSceneEditorDetialMapDialog::OnEnChangeEditSpecular)
	ON_EN_CHANGE(IDC_EDIT_EMISSIVE, &KSceneEditorDetialMapDialog::OnEnChangeEditEmissive)
END_MESSAGE_MAP()

BOOL KSceneEditorDetialMapDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_Selector.SetStaticControl(&m_StaticPreview);
	return TRUE;
}
// KSceneEditorDetialMapDialog message handlers
void KSceneEditorDetialMapDialog::SetPotinerToTgaButton()
{
	size_t i;
	int type = BRUSHMAP;
	for(i = 0; i < 10; ++i)
	{
		m_BrushMap[i].SetOutDoorSpaceMgr(m_lpSceneOutDoorSpaceMgr);
		m_BrushMap[i].SetSceneEditor(m_lpSceneEditor);
		m_BrushMap[i].InitButton(-1,type);
	}
	type = GROUNDTEXTURE;
	m_firstLayer.SetOutDoorSpaceMgr(m_lpSceneOutDoorSpaceMgr);
	m_firstLayer.SetSceneEditor(m_lpSceneEditor);
	m_firstLayer.InitButton(0,type);
	m_firstLayer.m_lpStaticSection = &m_Selector;
	LoadInfo();
}
void KSceneEditorDetialMapDialog::LoadInfo()
{
	map<BYTE,std::string> mapDetailTex;
	TCHAR path_buffer[256];
	TCHAR drive[256];
	TCHAR dir[256];
	TCHAR fname[256];
	TCHAR ext[256];
	while(m_ListBox_Text.GetCount())
	{
		m_ListBox_Text.DeleteString(0);
	}
	m_vecTexIndex.clear();
	if(SUCCEEDED(m_lpSceneOutDoorSpaceMgr->GetAllDetailTextureFromMaterialMgr(&mapDetailTex)))
	{
		map<BYTE,std::string>::iterator it = mapDetailTex.begin();
		if (mapDetailTex.size()>0)
		{
			std::string TextureName = it->second;
			TCHAR Name[MAX_PATH];
			wsprintf(Name,"%s",TextureName.c_str());
			m_firstLayer.LoadTgaFile(Name,false);
			it++;
		}
		while (it != mapDetailTex.end())
		{
			std::string TextureName = it->second;
			wsprintf(path_buffer,"%s",TextureName.c_str());
			_splitpath( path_buffer, drive, dir, fname, ext);

			m_ListBox_Text.AddString(fname);
			m_vecTexIndex.push_back(it->first);
			it++;
		}

	}

}
HRESULT KSceneEditorDetialMapDialog::FillMtl()
{	
	return S_OK;
}

void KSceneEditorDetialMapDialog::OnLbnSelchangeListText()
{
	// TODO: Add your control notification handler code here
	INT* pTextureIndex = NULL;
	TCHAR strTexturePathName[MAX_PATH];
	TCHAR strTextureFileName[MAX_PATH];
	int nCurSel = -1;
	KG_PROCESS_ERROR(m_lpSceneEditor);
	KG_PROCESS_ERROR(m_lpSceneOutDoorSpaceMgr);
	nCurSel = m_ListBox_Text.GetCurSel();
	KG_PROCESS_ERROR(nCurSel != CB_ERR);
	m_byCurTexIndex = m_vecTexIndex[nCurSel];
	m_lpSceneEditor->SetPaintTexIndex(m_byCurTexIndex);
	m_ListBox_Text.GetText(nCurSel,strTextureFileName);	
	m_strCurTexName = strTextureFileName;
	sprintf(strTexturePathName, "%s\\Data\\source\\maps_source\\Texture\\地表贴图\\%s.tga", g_szDefWorkDirectory, strTextureFileName);
	m_Selector.LoadTga(strTexturePathName,FALSE);
	m_Selector.UpdateControl(); 
	m_lpSceneEditor->SetEditState(SCENESTATE_TERRAIN);
	m_lpSceneEditor->SetTBModifierState(MAP_PAINT_GROUND);
	m_firstLayer.SetIsSelected(false);
	UpdateTexCoord();
Exit0:
	return ;
}

void KSceneEditorDetialMapDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	FillMtl();
}

void KSceneEditorDetialMapDialog::OnBnClickedButtonAddtexture()
{
	// TODO: Add your control notification handler code here
	if (!m_lpSceneOutDoorSpaceMgr)
		return ;
	char bigBuff[800] = "";
	char szFilter[] ="Texture (*.tga)|*.tga|";
	TCHAR FileDir[256];
	std::string DiablogPath;
	TCHAR szDir[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH,szDir);

	CFileDialog dlg(TRUE, NULL, NULL,
		OFN_HIDEREADONLY , szFilter);

	dlg.m_ofn.lpstrFile = bigBuff;
	dlg.m_ofn.nMaxFile = sizeof(bigBuff);


	DiablogPath = g_szDefWorkDirectory;
	DiablogPath += "Data\\source\\maps_source\\Texture\\地表贴图";
	memcpy(FileDir,DiablogPath.c_str(),DiablogPath.length());
	FileDir[DiablogPath.size()] = '\0';
	dlg.m_ofn.lpstrInitialDir = FileDir;
	
	if (dlg.DoModal() == IDOK)
	{
		TCHAR path_buffer[256];
		TCHAR drive[256];
		TCHAR dir[256];
		TCHAR fname[256];
		TCHAR ext[256];

		CString Vs = dlg.GetPathName();

		wsprintf(path_buffer,"%s",Vs);
		_splitpath( path_buffer, drive, dir, fname, ext );

		wsprintf(FileDir,"%s%s%s.tga",drive,dir,fname);

		BYTE byIndex = 0;
		int nFind = m_ListBox_Text.FindStringExact(0,fname);
		if(nFind != -1)
		{
			::SetCurrentDirectory(szDir);
			return ;
		}
		if(SUCCEEDED(m_lpSceneOutDoorSpaceMgr->AddDetailTextureToMaterialMgr(byIndex,FileDir)))
		{
			m_ListBox_Text.AddString(fname);
			m_vecTexIndex.push_back(byIndex);
		}
	}
	::SetCurrentDirectory(szDir);
	//LoadTgaFile(FileDir,TRUE);

}

void KSceneEditorDetialMapDialog::OnBnClickedButtonDeltexture()
{
	if (!m_lpSceneOutDoorSpaceMgr)
		return ;
	int nCurSel;
	BYTE byTexIndex = 0 ;
	nCurSel = m_ListBox_Text.GetCurSel();
	
	KG_PROCESS_ERROR(nCurSel != CB_ERR);
	byTexIndex =  m_vecTexIndex[nCurSel];

	if (SUCCEEDED(m_lpSceneOutDoorSpaceMgr->DelDetailTextureFromMaterialMgr(byTexIndex)))
	{
		if(m_lpSceneEditor)
			m_lpSceneEditor->ClearTextureFromTerrain(byTexIndex);
		vector<BYTE>::iterator i = m_vecTexIndex.begin();
		for (int j=0 ;j< nCurSel;j++)
			i++;
		m_vecTexIndex.erase(i);
		m_ListBox_Text.DeleteString(nCurSel);
	}
	if(m_lpSceneEditor)
	{
		m_byCurTexIndex = 0;
		m_lpSceneEditor->SetPaintTexIndex(m_byCurTexIndex);
		m_lpSceneEditor->SetEditState(SCENESTATE_NONE);
	}
Exit0:
	return ;
}


void KSceneEditorDetialMapDialog::OnBnClickedRadioXz()
{
	//int nCurSel = m_ListBox_Text.GetCurSel();
	//KG_PROCESS_ERROR(nCurSel != CB_ERR);
	m_byTexCoordIndex = 0;
	m_lpSceneOutDoorSpaceMgr->SetDetailTextureCoordIndex(m_byCurTexIndex,m_byTexCoordIndex);

}

void KSceneEditorDetialMapDialog::OnBnClickedRadioXy()
{
	// TODO: Add your control notification handler code here
	//int nCurSel = m_ListBox_Text.GetCurSel();
	//KG_PROCESS_ERROR(nCurSel != CB_ERR);
	m_byTexCoordIndex = 1;
	m_lpSceneOutDoorSpaceMgr->SetDetailTextureCoordIndex(m_byCurTexIndex,m_byTexCoordIndex);

}

void KSceneEditorDetialMapDialog::OnBnClickedRadioYz()
{
	//int nCurSel = m_ListBox_Text.GetCurSel();
	//KG_PROCESS_ERROR(nCurSel != CB_ERR);
	m_byTexCoordIndex = 2;
	m_lpSceneOutDoorSpaceMgr->SetDetailTextureCoordIndex(m_byCurTexIndex,m_byTexCoordIndex);

}

void KSceneEditorDetialMapDialog::OnBnClickedButtonLayer0()
{
	// TODO: Add your control notification handler code here
	m_strCurTexName = m_firstLayer.GetTextureName();
	//TCHAR path_buffer[256];
	TCHAR drive[256];
	TCHAR dir[256];
	TCHAR fname[256];
	TCHAR ext[256];
	_splitpath( m_strCurTexName.c_str(), drive, dir, fname, ext);
	m_strCurTexName = fname;
	m_byCurTexIndex = 0;
	UpdateTexCoord();
	m_ListBox_Text.SetCurSel(-1);
}

void KSceneEditorDetialMapDialog::OnEnChangeEditScaleu()
{
	if(!m_lpSceneOutDoorSpaceMgr)
		return;
	UpdateData(TRUE);
	m_lpSceneOutDoorSpaceMgr->SetDetailTextureUV(m_byCurTexIndex,m_fScaleU,m_fScaleV,m_fEnv,m_fSpecular,m_fEmissive);
}

void KSceneEditorDetialMapDialog::OnEnChangeEditScalev()
{
	if(!m_lpSceneOutDoorSpaceMgr)
		return;
	UpdateData(TRUE);
	m_lpSceneOutDoorSpaceMgr->SetDetailTextureUV(m_byCurTexIndex,m_fScaleU,m_fScaleV,m_fEnv,m_fSpecular,m_fEmissive);
}

void KSceneEditorDetialMapDialog::UpdateTexCoord()
{
	if(!m_lpSceneOutDoorSpaceMgr)
		return ;
	m_lpSceneOutDoorSpaceMgr->GetDetailTextureGrassData(m_byCurTexIndex,&m_bAssociateGrass,m_byGrassTexIndex,m_byPatternIndex);
	m_lpSceneOutDoorSpaceMgr->GetDetailTextureCoordIndex(m_byCurTexIndex,m_byTexCoordIndex);
	switch(m_byTexCoordIndex)
	{
	case 0:
		((CButton*)(GetDlgItem(IDC_RADIO_XZ)))->SetCheck(TRUE);
		((CButton*)(GetDlgItem(IDC_RADIO_XY)))->SetCheck(FALSE);
		((CButton*)(GetDlgItem(IDC_RADIO_YZ)))->SetCheck(FALSE);
		break;
	case 1:
		((CButton*)(GetDlgItem(IDC_RADIO_XZ)))->SetCheck(FALSE);
		((CButton*)(GetDlgItem(IDC_RADIO_XY)))->SetCheck(TRUE);
		((CButton*)(GetDlgItem(IDC_RADIO_YZ)))->SetCheck(FALSE);
		break;
	case 2:
		((CButton*)(GetDlgItem(IDC_RADIO_XZ)))->SetCheck(FALSE);
		((CButton*)(GetDlgItem(IDC_RADIO_XY)))->SetCheck(FALSE);
		((CButton*)(GetDlgItem(IDC_RADIO_YZ)))->SetCheck(TRUE);
		break;
	}
	m_lpSceneOutDoorSpaceMgr->GetDetailTextureUV(m_byCurTexIndex,&m_fScaleU,&m_fScaleV,&m_fEnv,&m_fSpecular,&m_fEmissive);
	UpdateData(FALSE);
	OnBnClickedCheckAutograss();
}
void KSceneEditorDetialMapDialog::OnBnClickedChangetex()
{
	if (!m_lpSceneOutDoorSpaceMgr)
		return ;
	char bigBuff[800] = "";
	char szFilter[] ="Texture (*.tga)|*.tga|";
	TCHAR FileDir[256];
	std::string DiablogPath;
	TCHAR szDir[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH,szDir);

	CFileDialog dlg(TRUE, NULL, NULL,
		OFN_HIDEREADONLY , szFilter);

	dlg.m_ofn.lpstrFile = bigBuff;
	dlg.m_ofn.nMaxFile = sizeof(bigBuff);


	DiablogPath = g_szDefWorkDirectory;
	DiablogPath += "Data\\source\\maps_source\\Texture\\地表贴图";
	memcpy(FileDir,DiablogPath.c_str(),DiablogPath.length());
	FileDir[DiablogPath.size()] = '\0';
	dlg.m_ofn.lpstrInitialDir = FileDir;

	if (dlg.DoModal() == IDOK)
	{
		TCHAR path_buffer[256];
		TCHAR drive[256];
		TCHAR dir[256];
		TCHAR fname[256];
		TCHAR ext[256];

		CString Vs = dlg.GetPathName();

		wsprintf(path_buffer,"%s",Vs);
		_splitpath( path_buffer, drive, dir, fname, ext );


		wsprintf(FileDir,"%s%s%s.tga",drive,dir,fname);

		BYTE byIndex = 0;
		int nFind = m_ListBox_Text.FindStringExact(0,fname);
		if(nFind != -1)
		{
			MessageBox("列表中已有该贴图！！","提示",MB_OK);
			::SetCurrentDirectory(szDir);
			return ;
		}
		m_lpSceneOutDoorSpaceMgr->ChangeDetailTexture(m_byCurTexIndex,FileDir);

		{			
			if(m_byCurTexIndex != 0)
			{
				int index = m_ListBox_Text.GetCurSel();
				m_ListBox_Text.DeleteString(index);
				m_ListBox_Text.InsertString(index,fname);
				m_ListBox_Text.SetCurSel(index);
			}
			else
			{
				m_firstLayer.ChangeTagFile(FileDir);
			}		
		}

	}
	::SetCurrentDirectory(szDir);
	
}

void KSceneEditorDetialMapDialog::OnBnClickedCheckAutograss()
{
	m_bAssociateGrass = ((CButton*)(GetDlgItem(IDC_CHECK_AUTOGRASS)))->GetCheck();
	if(m_lpSceneEditor)
		m_lpSceneEditor->SetAddAutoGrass(m_bAssociateGrass);
	static KSceneEditorDialogAutoGrass DlgAutoGrass;
	//DlgAssGrass.AttachScene(m_lpSceneEditor, NULL, 0, 0);
	if(m_lpSceneOutDoorSpaceMgr)
		m_lpSceneOutDoorSpaceMgr->SetDetailTextureGrassData(m_byCurTexIndex,&m_bAssociateGrass,NULL,NULL,TRUE);
	if (!IsWindow(DlgAutoGrass.m_hWnd))
	{
		DlgAutoGrass.SetSceneEditor(m_lpSceneEditor,m_lpSceneOutDoorSpaceMgr);
		DlgAutoGrass.Create(IDD_AUTO_GRASS, this);
	}
	if(m_bAssociateGrass)
	{
		
		CString strText;
		strText.Format(TEXT("%s  TexIndex : %d "), m_strCurTexName.c_str(), (int)m_byCurTexIndex);
		DlgAutoGrass.SetWindowText(strText);
		DlgAutoGrass.SetDetialTexData(m_byCurTexIndex,m_byGrassTexIndex,m_byPatternIndex);

		DlgAutoGrass.ShowWindow(SW_SHOW);		
	}
	else
	{
		DlgAutoGrass.ShowWindow(SW_HIDE);
	}
}

void KSceneEditorDetialMapDialog::OnBnClickedButtonFilter()
{
	// TODO: Add your control notification handler code here
	if(!m_lpSceneEditor)
		return ;
	
	TCHAR szDir[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH,szDir);

	IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
	TCHAR szAppDirectory[MAX_PATH] = { 0 };

	CFileDialog dlg(true, "tga", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "过滤贴图(*.tga)|*.tga||");
	if(dlg.DoModal() == IDOK)
	{
		TCHAR FileName[MAX_PATH];
	//	TCHAR RetFileName[MAX_PATH];
		wsprintf(FileName,"%s",dlg.GetPathName());
		//pManager->GetDefWorkDirectory(szAppDirectory, MAX_PATH);
		//pManager->GetRelativePath(szAppDirectory,FileName,RetFileName);

		m_lpSceneEditor->PaintTerrainTextureFromFilterMap(FileName);//不要传相对路径进去，里面是直接用D3DXCreateTextureFromFileEx来创建贴图
	}

	::SetCurrentDirectory(szDir);
}

void KSceneEditorDetialMapDialog::OnBnClickedButtonExport()
{
	// TODO: Add your control notification handler code here
	if(!m_lpSceneEditor)
		return ;
	TCHAR szDir[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH,szDir);

	TCHAR strFilePath[MAX_PATH];
	CFileDialog dlgSave(FALSE, 
		"tga", 
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"(*.tga)|*.tga||",
		this);

	if( dlgSave.DoModal() == IDOK)
	{
		sprintf_s(strFilePath,
			MAX_PATH,
			"%s",
			dlgSave.GetPathName());

		m_lpSceneEditor->ExprotFilterMap(strFilePath);
	}

	 ::SetCurrentDirectory(szDir);
}

void KSceneEditorDetialMapDialog::OnEnChangeEditEnv()
{
	if(!m_lpSceneOutDoorSpaceMgr)
		return;
	UpdateData(TRUE);
	m_lpSceneOutDoorSpaceMgr->SetDetailTextureUV(m_byCurTexIndex,m_fScaleU,m_fScaleV,m_fEnv,m_fSpecular,m_fEmissive);
}

void KSceneEditorDetialMapDialog::OnEnChangeEditSpecular()
{
	if(!m_lpSceneOutDoorSpaceMgr)
		return;
	UpdateData(TRUE);
	m_lpSceneOutDoorSpaceMgr->SetDetailTextureUV(m_byCurTexIndex,m_fScaleU,m_fScaleV,m_fEnv,m_fSpecular,m_fEmissive);
}

void KSceneEditorDetialMapDialog::OnEnChangeEditEmissive()
{
	if(!m_lpSceneOutDoorSpaceMgr)
		return;
	UpdateData(TRUE);
	m_lpSceneOutDoorSpaceMgr->SetDetailTextureUV(m_byCurTexIndex,m_fScaleU,m_fScaleV,m_fEnv,m_fSpecular,m_fEmissive);
}
