// KSceneEditorDialogAutoGrass.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneEditorDialogAutoGrass.h"


// KSceneEditorDialogAutoGrass dialog

IMPLEMENT_DYNAMIC(KSceneEditorDialogAutoGrass, CDialog)

KSceneEditorDialogAutoGrass::KSceneEditorDialogAutoGrass(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneEditorDialogAutoGrass::IDD, pParent)
{
	m_nWidhtCount = 1;
	m_nHeightCount= 1;
	m_nGrassDensity = 1;
	m_lpSceneSceneEditor = NULL;
	m_lpOutDoorSpaceMgr = NULL;
	m_byDetialTexIndex = 0;
	m_fWidthMin = 100;
	m_fWidthMax = 200;
	m_fHeightMin = 100;
	m_fHeightMax = 200;
	sprintf(m_GrassTexturePath, "%sData\\source\\maps_source\\Texture\\GrassTexture\\",g_szDefWorkDirectory);
}

KSceneEditorDialogAutoGrass::~KSceneEditorDialogAutoGrass()
{
}

void KSceneEditorDialogAutoGrass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PATTERN, m_ListBoxPattern);
	DDX_Control(pDX, IDC_COMBO_TEXTURE, m_ComboxBoxTexture);
	DDX_Control(pDX, IDC_PREVIEW, m_StaticPreview);
	DDX_Text(pDX,IDC_EDIT_DENSITY,m_nGrassDensity);
	DDX_Text(pDX, IDC_EDIT_WIDTHMIN, m_fWidthMin);
	DDX_Text(pDX, IDC_EDIT_WIDTHMAX, m_fWidthMax);
	DDX_Text(pDX, IDC_EDIT_HEIGHTMIN, m_fHeightMin);
	DDX_Text(pDX, IDC_EDIT_HEIGHTMAX, m_fHeightMax);
	DDX_Text(pDX, IDC_EDIT_SPLITH, m_nWidhtCount);
	DDV_MinMaxInt(pDX, m_nWidhtCount, 1, 100);
	DDX_Text(pDX, IDC_EDIT_SPLITV, m_nHeightCount);
	DDV_MinMaxInt(pDX, m_nHeightCount, 1, 100);
}


BEGIN_MESSAGE_MAP(KSceneEditorDialogAutoGrass, CDialog)
	ON_BN_CLICKED(IDC_BUN_REFRESHTEXTURE, &KSceneEditorDialogAutoGrass::OnBnClickedBunRefreshtexture)
	ON_BN_CLICKED(IDC_BUN_REFRESH, &KSceneEditorDialogAutoGrass::OnBnClickedBunRefresh)
	ON_CBN_SELCHANGE(IDC_COMBO_TEXTURE, &KSceneEditorDialogAutoGrass::OnCbnSelchangeComboTexture)
	ON_STN_CLICKED(IDC_PREVIEW, &KSceneEditorDialogAutoGrass::OnStnClickedPreview)
	ON_LBN_SELCHANGE(IDC_LIST_PATTERN, &KSceneEditorDialogAutoGrass::OnLbnSelchangeListPattern)
	ON_EN_CHANGE(IDC_EDIT_DENSITY, &KSceneEditorDialogAutoGrass::OnEnChangeEditDensity)
	ON_BN_CLICKED(IDC_DEL_GRASS, &KSceneEditorDialogAutoGrass::OnBnClickedDelGrass)
	ON_BN_CLICKED(IDC_BULID_GRASS, &KSceneEditorDialogAutoGrass::OnBnClickedBulidGrass)
	ON_BN_CLICKED(IDC_BUTTON_DELGRASS, &KSceneEditorDialogAutoGrass::OnBnClickedButtonDelgrass)
	ON_EN_KILLFOCUS(IDC_EDIT_WIDTHMIN, &KSceneEditorDialogAutoGrass::OnEnKillfocusEditWidthmin)
	ON_EN_KILLFOCUS(IDC_EDIT_WIDTHMAX, &KSceneEditorDialogAutoGrass::OnEnKillfocusEditWidthmax)
	ON_EN_KILLFOCUS(IDC_EDIT_HEIGHTMIN, &KSceneEditorDialogAutoGrass::OnEnKillfocusEditHeightmin)
	ON_EN_KILLFOCUS(IDC_EDIT_HEIGHTMAX, &KSceneEditorDialogAutoGrass::OnEnKillfocusEditHeightmax)
	ON_BN_CLICKED(IDC_BUTTON_ADDGRASS, &KSceneEditorDialogAutoGrass::OnBnClickedButtonAddgrass)
	ON_EN_CHANGE(IDC_EDIT_SPLITH, &KSceneEditorDialogAutoGrass::OnEnChangeEditSplith)
	ON_EN_CHANGE(IDC_EDIT_SPLITV, &KSceneEditorDialogAutoGrass::OnEnChangeEditSplitv)
END_MESSAGE_MAP()


// KSceneEditorDialogAutoGrass message handlers
BOOL KSceneEditorDialogAutoGrass::OnInitDialog()
{
	CDialog::OnInitDialog();


	m_Selector.SetStaticControl(&m_StaticPreview);
	if(m_lpOutDoorSpaceMgr)
		m_lpOutDoorSpaceMgr->SetCurGrassSet(MAP_GRASS);
	OnBnClickedBunRefreshtexture();
	OnBnClickedBunRefresh();
	SetComboBoxTexture();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
void KSceneEditorDialogAutoGrass::SetComboBoxTexture()
{
	if (m_lpOutDoorSpaceMgr)
	{
		TCHAR strTextureName[MAX_PATH];
		std::string strName = m_lpOutDoorSpaceMgr->GetGrassSetTextureName(MAP_GRASS);
		_splitpath(strName.c_str(), NULL, NULL, strTextureName, NULL);
		int nSel = m_ComboxBoxTexture.FindStringExact(0,strTextureName);
		if(nSel >=0)
		{
			m_ComboxBoxTexture.SetCurSel(nSel);
			OnCbnSelchangeComboTexture();
		}
	}
}
void KSceneEditorDialogAutoGrass::OnBnClickedBunRefreshtexture()
{
//	TCHAR strTexturePath[MAX_PATH];
	TCHAR strFileFilter[MAX_PATH];
	TCHAR strFileName[MAX_PATH];
	//sprintf(strTexturePath, "%sData\\source\\maps_source\\Texture\\GrassTexture\\", g_szDefWorkDirectory);
	sprintf(strFileFilter, "%s*.tga", m_GrassTexturePath/*strTexturePath*/);
	CFileFind FileFind;

	m_ComboxBoxTexture.ResetContent();
	BOOL bReturnCode = FileFind.FindFile(strFileFilter);
	while (bReturnCode)
	{
		bReturnCode = FileFind.FindNextFile();
		CString strFilePath = FileFind.GetFilePath();
		_splitpath(strFilePath, NULL, NULL, strFileName, NULL);
		m_ComboxBoxTexture.AddString(strFileName);
	}
	m_nLastSel = -1;
}

void KSceneEditorDialogAutoGrass::OnBnClickedBunRefresh()
{
	CFileFind Files;
	TCHAR strGrassPatternPath[MAX_PATH];
	BOOL bReturnCode = FALSE;
	HRESULT hr = E_FAIL;
	int nIndex = 0;
	TCHAR strFileName[MAX_PATH];

	HRESULT hRetCode = E_FAIL;

	m_ListBoxPattern.ResetContent();

	KG_PROCESS_ERROR(m_lpSceneSceneEditor);
	KG_PROCESS_ERROR(m_lpOutDoorSpaceMgr);

	sprintf(strGrassPatternPath, "%sData\\public\\GrassPattern\\*.mesh", g_szDefWorkDirectory);
	
	bReturnCode = Files.FindFile(strGrassPatternPath);

	while(bReturnCode)
	{
		int nDummy = 0;
		bReturnCode = Files.FindNextFile();
		CString strFilePath = Files.GetFilePath();

		hr = m_lpOutDoorSpaceMgr->FindPattern(strFilePath,&nDummy,MAP_GRASS);
		
		nIndex++;
		if (FAILED(hr))
		{
			KGLogPrintf(KGLOG_ERR, "Load grass pattern %s failed.", strFilePath);
			continue;
		}
		_splitpath(strFilePath, NULL, NULL, strFileName, NULL);
		m_ListBoxPattern.AddString(strFileName);
	}

Exit0:
	return;
}

void KSceneEditorDialogAutoGrass::OnCbnSelchangeComboTexture()
{
	UpdateTextureInfo(FALSE);//FALSE
}

void KSceneEditorDialogAutoGrass::UpdateTextureInfo(BOOL bFrameToData /* = TRUE */)
{
	INT* pTextureIndex = NULL;
	TCHAR strTexturePathName[MAX_PATH];
	TCHAR strTextureFileName[MAX_PATH];

	BYTE* pBitmap = NULL;
	DWORD dwSize = 0;
	int nCurSel = -1;

	KG_PROCESS_ERROR(m_lpSceneSceneEditor);
	KG_PROCESS_ERROR(m_lpOutDoorSpaceMgr);
	UpdateData();
	m_lpSceneSceneEditor->SetTBModifierWidthMax(m_fWidthMax);
	m_lpSceneSceneEditor->SetTBModifierWidthMin(m_fWidthMin);
	m_lpSceneSceneEditor->SetTBModifierHeightMax(m_fHeightMax);
	m_lpSceneSceneEditor->SetTBModifierHeightMin(m_fHeightMin);

	if (!bFrameToData)
		LoadTextureSettings();

	m_lpSceneSceneEditor->SetTBModifierTextureStride(m_nWidhtCount,m_nHeightCount,MAP_GRASS);
	

	nCurSel = m_ComboxBoxTexture.GetCurSel();
	KG_PROCESS_ERROR(nCurSel != CB_ERR);

	if (bFrameToData)
		SaveTextureSettings();

	m_ComboxBoxTexture.GetLBText(nCurSel, strTextureFileName);
	sprintf(strTexturePathName, "%s%s.tga", m_GrassTexturePath, strTextureFileName);
	m_Selector.LoadTga(strTexturePathName);
	//m_StaticPreview.SetBitmap(m_BitmapPreview);

	m_Selector.SetSection(m_nWidhtCount, m_nHeightCount);
	m_Selector.OnSelect(NULL, 0, FALSE);


	INT nNumTextureIndex = 0;
	m_Selector.GetSelectSection(&pTextureIndex,nNumTextureIndex);
	m_lpSceneSceneEditor->SetTBModifierTextureIndexInfo(pTextureIndex,&nNumTextureIndex);
	m_lpOutDoorSpaceMgr->SetGrassSetTexture(strTexturePathName,MAP_GRASS);
	

	m_nLastSel = nCurSel;
Exit0:
	SAFE_DELETE_ARRAY(pBitmap);
	SAFE_DELETE_ARRAY(pTextureIndex);
	return;
}

void KSceneEditorDialogAutoGrass::LoadTextureSettings()
{
	CString strTextureName;
	TCHAR strIniFileName[MAX_PATH];
	int nCurSel = m_ComboxBoxTexture.GetCurSel();
	IIniFile* pInIFile = NULL;
	int nReturnCode = 0;

	KG_PROCESS_ERROR(nCurSel != CB_ERR);

	m_ComboxBoxTexture.GetLBText(nCurSel, strTextureName);
	sprintf(strIniFileName, "%s%s.ini", m_GrassTexturePath, strTextureName);
	
	////把文件的只读属性去掉////////////////////////////////////by huangjinshou
	DWORD dwFileAttribute = ::GetFileAttributes(strIniFileName);
	dwFileAttribute &=~FILE_ATTRIBUTE_READONLY; 
	::SetFileAttributes(strIniFileName,dwFileAttribute);
	///////////////////////////////////////////////////////////
	pInIFile = g_OpenIniFile(strIniFileName, false, true);
	KG_PROCESS_ERROR(pInIFile);

	pInIFile->GetInteger("Texture", "StrideX", 1, &m_nWidhtCount);
	pInIFile->GetInteger("Texture", "StrideY", 1, &m_nHeightCount);

	UpdateData(FALSE);
Exit0:
	SAFE_RELEASE(pInIFile);
	return;
}
void KSceneEditorDialogAutoGrass::OnStnClickedPreview()
{
	POINT point;
	IEKG3DTerrain* pTerrainEx = NULL;
	INT* pTextureIndex = NULL;

	KG_PROCESS_ERROR(m_lpSceneSceneEditor);

	GetCursorPos(&point);
	m_StaticPreview.ScreenToClient(&point);
	SHORT s = GetAsyncKeyState(VK_CONTROL);
	m_Selector.OnSelect(&point, 1, !(0xf000&s));

	INT nNumTextureIndex = 0;
	m_Selector.GetSelectSection(&pTextureIndex,nNumTextureIndex);
	
	m_lpSceneSceneEditor->SetTBModifierTextureIndexInfo(pTextureIndex,&nNumTextureIndex);
	for(int i = 0;i< 8;i++ )
	{
		if(i < nNumTextureIndex)
			m_pbyGrassTexIndex[i] = (BYTE)(pTextureIndex[i]);
		else
			m_pbyGrassTexIndex[i] = 255;
	}
	UpdateTextureInfo();
Exit0:
	SAFE_DELETE_ARRAY(pTextureIndex);
	return;
}

void KSceneEditorDialogAutoGrass::OnLbnSelchangeListPattern()
{
	HRESULT hRetCode = E_FAIL;
	int nCount = 0;
	int nSelCount = 0;
	int* pSelPatterns = NULL;
	TCHAR strPatternPathName[MAX_PATH];
	KG_PROCESS_ERROR(m_lpSceneSceneEditor);
	KG_PROCESS_ERROR(m_lpOutDoorSpaceMgr);
	

	nSelCount = m_ListBoxPattern.GetSelCount();
	KG_PROCESS_ERROR(nSelCount > 0);

	pSelPatterns = new int[nSelCount];
	KG_PROCESS_ERROR(pSelPatterns);
	for(int n = 0;n< 8;n++ )
		m_pbyPatternIndex[n] = 255;

	nCount = 0;
	for (int i = 0; i < m_ListBoxPattern.GetCount(); i++)
	{
		int nSel = m_ListBoxPattern.GetSel(i);
		if ( nSel > 0)
		{
			CString strGrassPatternName;
			m_ListBoxPattern.GetText(i, strGrassPatternName);
			
			sprintf(strPatternPathName, "Data\\public\\GrassPattern\\%s.mesh",strGrassPatternName);
			
			_strlwr_s(strPatternPathName, sizeof(TCHAR) * MAX_PATH);
			int nPatternIndex = -1;
			
			hRetCode = m_lpOutDoorSpaceMgr->FindPattern(strPatternPathName,&nPatternIndex,MAP_GRASS);

			KGLOG_COM_PROCESS_ERROR(hRetCode);
			ASSERT(nPatternIndex != -1);
			pSelPatterns[nCount] = nPatternIndex;
			if(nCount < 8)
				m_pbyPatternIndex[nCount] = (BYTE)i;
			nCount++;
		}
	}
	m_lpSceneSceneEditor->UpdatePatternList(pSelPatterns, nSelCount);

Exit0:
	
	SAFE_DELETE_ARRAY(pSelPatterns);
	//OnBnClickedButtonAddgrass();
	return;
}

void KSceneEditorDialogAutoGrass::OnEnChangeEditDensity()
{
	UpdateData();
	if(m_nGrassDensity <= 0)
	{
		m_nGrassDensity = 1;
		UpdateData(FALSE);
	}
	if (m_lpSceneSceneEditor)
	{
		m_lpSceneSceneEditor->SetTBModifierGrassDensity(m_nGrassDensity);
	}
}

void KSceneEditorDialogAutoGrass::OnBnClickedDelGrass()
{
	// TODO: Add your control notification handler code here
	if(MessageBox("确定要删除当前纹理上所有的草吗？","提示",MB_OKCANCEL) != IDOK)
		return ;
	if (m_lpSceneSceneEditor)
		m_lpSceneSceneEditor->DeleteAutoGrass(m_byDetialTexIndex);
}

void KSceneEditorDialogAutoGrass::OnBnClickedBulidGrass()
{
	// TODO: Add your control notification handler code here
	if( MessageBox("该功能会改变所有当前纹理上的草，确定要执行吗？","提示",MB_OKCANCEL) != IDOK)
		return ;
	UpdateTextureInfo(FALSE);
	if (m_lpSceneSceneEditor)
		m_lpSceneSceneEditor->BuildAutoGrass(m_byDetialTexIndex);
	if(m_lpOutDoorSpaceMgr)
		m_lpOutDoorSpaceMgr->SetDetailTextureGrassData(m_byDetialTexIndex,NULL,m_pbyGrassTexIndex,m_pbyPatternIndex,TRUE);
}

void KSceneEditorDialogAutoGrass::OnBnClickedButtonDelgrass()
{
	if (m_lpSceneSceneEditor)
	{
		m_lpSceneSceneEditor->SetEditState(SCENESTATE_TERRAIN);
		m_lpSceneSceneEditor->SetTBModifierState(MAP_AUTOGRASS_DELETE);
	}
}
void KSceneEditorDialogAutoGrass::SetDetialTexData(BYTE byIndex,BYTE* byGrassTexIndex,BYTE* byPatternIndex)
{
	ClearSelected();
	m_byDetialTexIndex = byIndex;
	m_pbyGrassTexIndex = byGrassTexIndex;
	m_pbyPatternIndex = byPatternIndex;
	
	for (int i = 0; i < 6; i++)
	{
		if(m_pbyPatternIndex[i] != 255)
		{
			m_ListBoxPattern.SetSel(m_pbyPatternIndex[i],TRUE);
		}
	}

	m_Selector.SetSelected(m_pbyGrassTexIndex,8);
	INT* pTextureIndex = NULL;
	INT nNumTextureIndex = 0;
	m_Selector.GetSelectSection(&pTextureIndex,nNumTextureIndex);
	m_lpSceneSceneEditor->SetTBModifierTextureIndexInfo(pTextureIndex,&nNumTextureIndex);
	OnLbnSelchangeListPattern();
	SAFE_DELETE_ARRAY(pTextureIndex);

}

void KSceneEditorDialogAutoGrass::ClearSelected()
{
	int nCount = m_ListBoxPattern.GetCount();
	for (int i=0; i<nCount; i++)
	{
		int nSel = m_ListBoxPattern.GetSel(i);
		if ( nSel > 0)
			m_ListBoxPattern.SetSel(i,FALSE);
	}
	m_Selector.OnSelect(NULL, 0, FALSE);
}
void KSceneEditorDialogAutoGrass::OnEnKillfocusEditWidthmin()
{
	UpdateData(TRUE);
	if(!m_lpSceneSceneEditor)
		return ;
	m_lpSceneSceneEditor->SetTBModifierWidthMin(m_fWidthMin);
}

void KSceneEditorDialogAutoGrass::OnEnKillfocusEditWidthmax()
{
	UpdateData(TRUE);
	if(!m_lpSceneSceneEditor)
		return ;
	m_lpSceneSceneEditor->SetTBModifierWidthMax(m_fWidthMax);
}

void KSceneEditorDialogAutoGrass::OnEnKillfocusEditHeightmin()
{
	UpdateData(TRUE);
	if(!m_lpSceneSceneEditor)
		return ;
	m_lpSceneSceneEditor->SetTBModifierHeightMin(m_fHeightMin);
}

void KSceneEditorDialogAutoGrass::OnEnKillfocusEditHeightmax()
{
	UpdateData(TRUE);
	if(!m_lpSceneSceneEditor)
		return ;

	m_lpSceneSceneEditor->SetTBModifierHeightMax(m_fHeightMax);
}

void KSceneEditorDialogAutoGrass::OnOK()
{

}
void KSceneEditorDialogAutoGrass::OnBnClickedButtonAddgrass()
{
	// TODO: Add your control notification handler code here
	UpdateTextureInfo(FALSE);
	if (m_lpSceneSceneEditor)
	{
		m_lpSceneSceneEditor->SetEditState(SCENESTATE_TERRAIN);
		m_lpSceneSceneEditor->SetTBModifierState(MAP_AUTOGRASS_MANUALADD);

		if(m_lpOutDoorSpaceMgr)
			m_lpOutDoorSpaceMgr->SetDetailTextureGrassData(m_byDetialTexIndex,NULL,m_pbyGrassTexIndex,m_pbyPatternIndex,TRUE);
	}
}

void KSceneEditorDialogAutoGrass::OnEnChangeEditSplith()
{
	UpdateData(TRUE);

	if(!m_lpSceneSceneEditor)
		return;
	if(m_lpOutDoorSpaceMgr)
		m_lpOutDoorSpaceMgr->SetCurGrassSet(MAP_GRASS);

	m_lpSceneSceneEditor->SetTBModifierTextureStride(m_nWidhtCount,m_nHeightCount,MAP_GRASS);
}

void KSceneEditorDialogAutoGrass::OnEnChangeEditSplitv()
{
	UpdateData(TRUE);

	if(!m_lpSceneSceneEditor)
		return;
	if(m_lpOutDoorSpaceMgr)
		m_lpOutDoorSpaceMgr->SetCurGrassSet(MAP_GRASS);

	m_lpSceneSceneEditor->SetTBModifierTextureStride(m_nWidhtCount,m_nHeightCount,MAP_GRASS);
}

void KSceneEditorDialogAutoGrass::SaveTextureSettings()
{
	CString strTextureName;
	TCHAR strIniFileName[MAX_PATH];
	IIniFile* pInIFile = NULL;
	int nReturnCode = 0;

	KG_PROCESS_ERROR(m_nLastSel != -1);

	m_ComboxBoxTexture.GetLBText(m_nLastSel, strTextureName);
	sprintf(strIniFileName, "%s%s.ini", m_GrassTexturePath, strTextureName);

	pInIFile = g_OpenIniFile(strIniFileName, false, true);
	KG_PROCESS_ERROR(pInIFile);

	pInIFile->WriteInteger("Texture", "StrideX", m_nWidhtCount);
	pInIFile->WriteInteger("Texture", "StrideY", m_nHeightCount);

	pInIFile->Save(strIniFileName);
Exit0:
	SAFE_RELEASE(pInIFile);
	return;
}
