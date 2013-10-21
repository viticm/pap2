// KSceneSceneEditorDialogGrass.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSceneEditorDialogGrass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


// KSceneSceneEditorDialogGrass dialog
TCHAR g_GrassTexturePath[MAX_PATH];

IMPLEMENT_DYNAMIC(KSceneSceneEditorDialogGrass, KGLilisPanelDLG)
KSceneSceneEditorDialogGrass::KSceneSceneEditorDialogGrass(CWnd* pParent /*=NULL*/)
	: KGLilisPanelDLG(KSceneSceneEditorDialogGrass::IDD, pParent)
	, m_nWidhtCount(0)
	, m_nHeightCount(0)
	, m_nIndexMin(0)
	, m_nIndexMax(0)
{
	m_lpSceneSceneEditor = NULL;
	m_fWidthMin = 100;
	m_fWidthMax = 200;
	m_fHeightMin = 100;
	m_fHeightMax = 200;
	m_fNumPerM2 = 10.0f;
	m_nWidhtCount = 1;
	m_nHeightCount = 1;
	m_nIndexMin = 0;
	m_nIndexMax = 0;
	m_nLastSel = -1;
	m_nAlpha = 128;
	m_Radio = 0;
	sprintf(g_GrassTexturePath, "Data\\source\\maps_source\\Texture\\GrassTexture\\");
    EnableSlip(false);
}

KSceneSceneEditorDialogGrass::~KSceneSceneEditorDialogGrass()
{
}

void KSceneSceneEditorDialogGrass::DoDataExchange(CDataExchange* pDX)
{
	KGLilisPanelDLG::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_WIDTHMIN, m_fWidthMin);
	DDX_Text(pDX, IDC_EDIT_WIDTHMAX, m_fWidthMax);
	DDX_Text(pDX, IDC_EDIT_HEIGHTMIN, m_fHeightMin);
	DDX_Text(pDX, IDC_EDIT_HEIGHTMAX, m_fHeightMax);
	DDX_Text(pDX, IDC_EDIT_NUMBER,m_fNumPerM2);
	DDX_Text(pDX, IDC_EDIT_HSPLIT, m_nWidhtCount);
	DDV_MinMaxInt(pDX, m_nWidhtCount, 1, 100);
	DDX_Text(pDX, IDC_EDIT_VSPLIT, m_nHeightCount);
	DDV_MinMaxInt(pDX, m_nHeightCount, 1, 100);
	DDX_Text(pDX, IDC_EDIT_TEXTUREMIN, m_nIndexMin);
	DDV_MinMaxInt(pDX, m_nIndexMin, 0, 1000);
	DDX_Text(pDX, IDC_EDIT_TEXTUREMAX, m_nIndexMax);
	DDV_MinMaxInt(pDX, m_nIndexMax, 0, 10000);
	DDX_Control(pDX, IDC_LIST_PATTERN, m_ListBoxGrassPattern);
	DDX_Control(pDX, IDC_COMBO_GRASSTEXTURE, m_ComboxBoxGrassTexture);
	DDX_Control(pDX, IDC_STATIC_PREVIEW, m_StaticPreview);
	DDX_Text(pDX, IDC_EDIT_ALPHA, m_nAlpha);
}


BEGIN_MESSAGE_MAP(KSceneSceneEditorDialogGrass, KGLilisPanelDLG)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_RANDOMMAP, OnBnClickedButtonRandommap)
	ON_EN_CHANGE(IDC_EDIT_WIDTHMAX, OnEnChangeEditWidthmax)
	ON_EN_CHANGE(IDC_EDIT_WIDTHMIN, OnEnChangeEditWidthmin)
	ON_EN_CHANGE(IDC_EDIT_HEIGHTMAX, OnEnChangeEditHeightmax)
	ON_EN_CHANGE(IDC_EDIT_HEIGHTMIN, OnEnChangeEditHeightmin)
	ON_BN_CLICKED(IDC_CHECK_GRASSADD, OnBnClickedCheckGrassadd)
	ON_BN_CLICKED(IDC_CHECK_GRASSREMOVE, OnBnClickedCheckGrassremove)
	ON_EN_CHANGE(IDC_EDIT_NUMBER, OnEnChangeEditNumber)
	ON_BN_CLICKED(IDC_BUTTON_OPENGRASSTEXTURE, OnBnClickedButtonOpengrasstexture)
	ON_BN_CLICKED(IDC_BUTTON_AUTOLIGHT, OnBnClickedButtonAutoLight)
	ON_EN_CHANGE(IDC_EDIT_HSPLIT, &KSceneSceneEditorDialogGrass::OnEnChangeEditHsplit)
	ON_EN_CHANGE(IDC_EDIT_VSPLIT, &KSceneSceneEditorDialogGrass::OnEnChangeEditVsplit)
	ON_EN_CHANGE(IDC_EDIT_TEXTUREMIN, &KSceneSceneEditorDialogGrass::OnEnChangeEditTexturemin)
	ON_EN_CHANGE(IDC_EDIT_TEXTUREMAX, &KSceneSceneEditorDialogGrass::OnEnChangeEditTexturemax)
	ON_CBN_SELCHANGE(IDC_COMBO_GRASSTEXTURE, &KSceneSceneEditorDialogGrass::OnCbnSelchangeComboGrasstexture)
	ON_BN_CLICKED(IDC_BUTTON_FITGROUND, &KSceneSceneEditorDialogGrass::OnBnClickedButtonFitground)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &KSceneSceneEditorDialogGrass::OnBnClickedButtonRefresh)
	ON_BN_CLICKED(IDC_BUTTON_REFRESHTEXTURE, &KSceneSceneEditorDialogGrass::OnBnClickedButtonRefreshTexture)
	ON_LBN_SELCHANGE(IDC_LIST_PATTERN, &KSceneSceneEditorDialogGrass::OnLbnSelChangeListPattern)
	ON_STN_CLICKED(IDC_STATIC_PREVIEW, &KSceneSceneEditorDialogGrass::OnStnClickedStaticPreview)
    ON_BN_CLICKED(IDC_STATIC4, &KSceneSceneEditorDialogGrass::OnBnClickedStatic4)
	ON_BN_CLICKED(IDC_RADIO2, &KSceneSceneEditorDialogGrass::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, &KSceneSceneEditorDialogGrass::OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_RADIO_GRASS, &KSceneSceneEditorDialogGrass::OnBnClickedRadioGrass)
	ON_BN_CLICKED(IDC_RADIO_STONE, &KSceneSceneEditorDialogGrass::OnBnClickedRadioStone)
	ON_BN_CLICKED(IDC_RADIO_FRUTEX, &KSceneSceneEditorDialogGrass::OnBnClickedRadioFrutex)
	ON_BN_CLICKED(IDC_CHECK_ALPHABLEND, &KSceneSceneEditorDialogGrass::OnBnClickedCheckAlphablend)
	ON_BN_CLICKED(IDC_BUTTON_DEL_ALL, &KSceneSceneEditorDialogGrass::OnBnClickedButtonDelAll)
END_MESSAGE_MAP()


// KSceneSceneEditorDialogGrass message handlers

void KSceneSceneEditorDialogGrass::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if(!m_lpSceneSceneEditor)
		return ;
}

void KSceneSceneEditorDialogGrass::OnBnClickedButtonRandommap()
{
}

BOOL KSceneSceneEditorDialogGrass::OnInitDialog()
{
	KGLilisPanelDLG::OnInitDialog();
	

	//OnBnClickedButtonRefresh();
	//OnBnClickedButtonRefreshTexture();
//	m_ListBoxGrassPattern.SetSel(0, TRUE);
	//OnLbnSelChangeListPattern();

	m_Selector.SetStaticControl(&m_StaticPreview);
	CheckDlgButton(IDC_RADIO_GRASS,TRUE);
	CheckDlgButton(IDC_RADIO_STONE,FALSE);
	CheckDlgButton(IDC_RADIO_FRUTEX,FALSE);
	if(m_lpSceneSceneEditor)
	{
		IEKG3DSceneOutDoorSpaceMgr* pOutDoorSpacemgr = m_lpSceneSceneEditor->GetIEKG3DSceneOutDoorSpaceMgr();
		if(pOutDoorSpacemgr)
			pOutDoorSpacemgr->SetCurGrassSet(m_Radio);

	}	
	CheckButtonEnable();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneSceneEditorDialogGrass::OnEnChangeEditWidthmax()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	if(!m_lpSceneSceneEditor)
		return ;
	m_lpSceneSceneEditor->SetTBModifierWidthMax(m_fWidthMax);
}

void KSceneSceneEditorDialogGrass::OnEnChangeEditWidthmin()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	if(!m_lpSceneSceneEditor)
		return ;
	m_lpSceneSceneEditor->SetTBModifierWidthMin(m_fWidthMin);
}

void KSceneSceneEditorDialogGrass::OnEnChangeEditHeightmax()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	if(!m_lpSceneSceneEditor)
		return ;

	m_lpSceneSceneEditor->SetTBModifierHeightMax(m_fHeightMax);
}

void KSceneSceneEditorDialogGrass::OnEnChangeEditHeightmin()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	if(!m_lpSceneSceneEditor)
		return ;
	  m_lpSceneSceneEditor->SetTBModifierHeightMin(m_fHeightMin);
}

void KSceneSceneEditorDialogGrass::OnBnClickedCheckGrassadd()
{
	int nSelCount = 0;
	int nCurSel = 0;

	KG_PROCESS_ERROR(m_lpSceneSceneEditor);

	for(int i = 0; i < m_ListBoxGrassPattern.GetCount(); i++)
	{
		if (m_ListBoxGrassPattern.GetSel(i) > 0)
		{
			nSelCount++;
		}
	}

	if (!nSelCount)
	{
		CheckDlgButton(IDC_CHECK_GRASSADD,FALSE);
		CheckDlgButton(IDC_CHECK_GRASSREMOVE,FALSE);

		MessageBox("请先选中草模型");
		return;
	}

	nCurSel = m_ComboxBoxGrassTexture.GetCurSel();
	if (nCurSel == CB_ERR)
	{
		CheckDlgButton(IDC_CHECK_GRASSADD,FALSE);
		CheckDlgButton(IDC_CHECK_GRASSREMOVE,FALSE);
		MessageBox("还没选中贴图");
		goto Exit0;

	}

	CheckDlgButton(IDC_CHECK_GRASSADD,TRUE);
	CheckDlgButton(IDC_CHECK_GRASSREMOVE,FALSE);
	{
		IEKG3DSceneOutDoorSpaceMgr* pOutDoorSpacemgr = m_lpSceneSceneEditor->GetIEKG3DSceneOutDoorSpaceMgr();
		if(pOutDoorSpacemgr)
			pOutDoorSpacemgr->SetCurGrassSet(m_Radio);
	}
	UpdateTextureInfo(FALSE);
	m_lpSceneSceneEditor->SetEditState(SCENESTATE_TERRAIN);
	m_lpSceneSceneEditor->SetTBModifierState(MAP_GRASS_SET);
Exit0:
	return;
}

void KSceneSceneEditorDialogGrass::OnBnClickedCheckGrassremove()
{
	// TODO: Add your control notification handler code here
	if(!m_lpSceneSceneEditor)
		return ;
	m_lpSceneSceneEditor->SetEditState(SCENESTATE_TERRAIN);
	m_lpSceneSceneEditor->SetTBModifierState(MAP_GRASS_DELETE);

	CheckDlgButton(IDC_CHECK_GRASSADD,FALSE);
	CheckDlgButton(IDC_CHECK_GRASSREMOVE,TRUE);

}

void KSceneSceneEditorDialogGrass::OnEnChangeEditNumber()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	if(!m_lpSceneSceneEditor)
		return ;

	m_lpSceneSceneEditor->SetTBModifierNumPerM2(m_fNumPerM2);
}

void KSceneSceneEditorDialogGrass::OnOK()
{

}

void KSceneSceneEditorDialogGrass::OnBnClickedButtonAutoLight()
{
}




void KSceneSceneEditorDialogGrass::OnEnChangeEditHsplit()
{
	UpdateData(TRUE);

	if(!m_lpSceneSceneEditor)
		return;
	{
		IEKG3DSceneOutDoorSpaceMgr* pOutDoorSpacemgr = m_lpSceneSceneEditor->GetIEKG3DSceneOutDoorSpaceMgr();
		if(pOutDoorSpacemgr)
			pOutDoorSpacemgr->SetCurGrassSet(m_Radio);
	}
	m_lpSceneSceneEditor->SetTBModifierTextureStride(m_nWidhtCount,m_nHeightCount,m_Radio);
}

void KSceneSceneEditorDialogGrass::OnEnChangeEditVsplit()
{
	UpdateData(TRUE);

	if(!m_lpSceneSceneEditor)
		return;
	{
		IEKG3DSceneOutDoorSpaceMgr* pOutDoorSpacemgr = m_lpSceneSceneEditor->GetIEKG3DSceneOutDoorSpaceMgr();
		if(pOutDoorSpacemgr)
			pOutDoorSpacemgr->SetCurGrassSet(m_Radio);
	}
	m_lpSceneSceneEditor->SetTBModifierTextureStride(m_nWidhtCount,m_nHeightCount,m_Radio);

}

void KSceneSceneEditorDialogGrass::OnEnChangeEditTexturemin()
{
	UpdateData(TRUE);
}

void KSceneSceneEditorDialogGrass::OnEnChangeEditTexturemax()
{
	UpdateData(TRUE);
}

void KSceneSceneEditorDialogGrass::OnBnClickedButtonFitground()
{
	// TODO: Add your control notification handler code here
	if(!m_lpSceneSceneEditor)
		return;

    m_lpSceneSceneEditor->GSFFitGround();

	UpdateTextureInfo();
}

void KSceneSceneEditorDialogGrass::OnBnClickedButtonRefresh()
{
	CFileFind Files;
	TCHAR strGrassPatternPath[MAX_PATH];
	BOOL bReturnCode = FALSE;
	HRESULT hr = E_FAIL;
	int nIndex = 0;
	TCHAR strFileName[MAX_PATH];
	IEKG3DSceneOutDoorSpaceMgr* pOutDoorSpacemgr = NULL;
    IEKG3DTerrain *pTerrainEx = NULL;
    HRESULT hRetCode = E_FAIL;
    IEKG3DEngineManager *piManager = g_GetEngineIEInterface();
	ASSERT(piManager);
	m_ListBoxGrassPattern.ResetContent();

	KG_PROCESS_ERROR(m_lpSceneSceneEditor);

	pOutDoorSpacemgr = m_lpSceneSceneEditor->GetIEKG3DSceneOutDoorSpaceMgr();
	KG_PROCESS_ERROR(pOutDoorSpacemgr); 

    //hRetCode = Get3DEngineInterface((void **)&piManager);
    //KGLOG_COM_PROCESS_ERROR(hRetCode);
   // ASSERT(piManager);
	switch(m_Radio)
	{
	case MAP_GRASS:
		{
			sprintf(strGrassPatternPath, "%sData\\public\\GrassPattern\\*.mesh", g_szDefWorkDirectory);
			break;
		}
	case MAP_STONE:
		{
			sprintf(strGrassPatternPath, "%sData\\public\\StonePattern\\*.mesh", g_szDefWorkDirectory);
			break;
		}
	case MAP_FRUTEX:
		{
			sprintf(strGrassPatternPath, "%sData\\public\\FrutexPattern\\*.mesh", g_szDefWorkDirectory);
			break;
		}
	}
	bReturnCode = Files.FindFile(strGrassPatternPath);

	while(bReturnCode)
	{
		int nDummy = 0;
		bReturnCode = Files.FindNextFile();
		CString strFilePath = Files.GetFilePath();
		hr = pOutDoorSpacemgr->FindPattern(strFilePath,&nDummy,m_Radio);
		nIndex++;
		if (FAILED(hr))
		{
			KGLogPrintf(KGLOG_ERR, "Load grass pattern %s failed.", strFilePath);
			continue;
		}
		_splitpath(strFilePath, NULL, NULL, strFileName, NULL);
		m_ListBoxGrassPattern.AddString(strFileName);
	}

Exit0:
	return;
}

BOOL KSceneSceneEditorDialogGrass::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)
		{
			UpdateTextureInfo();
		}

	}
	return KGLilisPanelDLG::PreTranslateMessage(pMsg);
}

void KSceneSceneEditorDialogGrass::UpdateTextureInfo(BOOL bFrameToData /* = TRUE */)
{
	IEKG3DTerrain* pTerrainEx = NULL;
	IEKG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr = NULL;
    INT* pTextureIndex = NULL;
	TCHAR strTexturePathName[MAX_PATH];
	TCHAR strTextureFileName[MAX_PATH];

	BYTE* pBitmap = NULL;
	DWORD dwSize = 0;
	int nCurSel = -1;

	//IEKG3DEngineManager *piManager = g_GetEngineIEInterface();
	//ASSERT(piManager);
	KG_PROCESS_ERROR(m_lpSceneSceneEditor);
	UpdateData();

	pOutDoorSpaceMgr = m_lpSceneSceneEditor->GetIEKG3DSceneOutDoorSpaceMgr();
	KG_PROCESS_ERROR(pOutDoorSpaceMgr);
	m_lpSceneSceneEditor->SetTBModifierWidthMax(m_fWidthMax);
	m_lpSceneSceneEditor->SetTBModifierWidthMin(m_fWidthMin);
	m_lpSceneSceneEditor->SetTBModifierHeightMax(m_fHeightMax);
	m_lpSceneSceneEditor->SetTBModifierHeightMin(m_fHeightMin);
	
    if (!bFrameToData)
        LoadTextureSettings();

	m_lpSceneSceneEditor->SetTBModifierTextureIndexMax(m_nIndexMax);
	m_lpSceneSceneEditor->SetTBModifierTextureIndexMin(m_nIndexMin);
	m_lpSceneSceneEditor->SetTBModifierNumPerM2(m_fNumPerM2);
	m_lpSceneSceneEditor->SetTBModifierTextureStride(m_nWidhtCount,m_nHeightCount,m_Radio);

	nCurSel = m_ComboxBoxGrassTexture.GetCurSel();
	KG_PROCESS_ERROR(nCurSel != CB_ERR);

    if (bFrameToData)
	    SaveTextureSettings();

	m_ComboxBoxGrassTexture.GetLBText(nCurSel, strTextureFileName);
	sprintf(strTexturePathName, "%s%s.tga", g_GrassTexturePath, strTextureFileName);
	m_Selector.LoadTga(strTexturePathName);
	//m_StaticPreview.SetBitmap(m_BitmapPreview);
    if (bFrameToData)
	    LoadTextureSettings();
	
    m_Selector.SetSection(m_nWidhtCount, m_nHeightCount);
	m_Selector.OnSelect(NULL, 0, FALSE);

	
	INT nNumTextureIndex = 0;
	m_Selector.GetSelectSection(&pTextureIndex,nNumTextureIndex);
	m_lpSceneSceneEditor->SetTBModifierTextureIndexInfo(pTextureIndex,&nNumTextureIndex);
	pOutDoorSpaceMgr->SetGrassSetTexture(strTexturePathName,m_Radio);

	m_nLastSel = nCurSel;
Exit0:
	SAFE_DELETE_ARRAY(pBitmap);
    SAFE_DELETE_ARRAY(pTextureIndex);
	return;
}

void KSceneSceneEditorDialogGrass::OnBnClickedButtonRefreshTexture()
{
	TCHAR strFileFilter[MAX_PATH];
	TCHAR strFileName[MAX_PATH];
	//sprintf(g_GrassTexturePath, "%sData\\source\\maps_source\\Texture\\GrassTexture\\", g_szDefWorkDirectory);
	sprintf(strFileFilter, "%s*.tga", g_GrassTexturePath);
	CFileFind FileFind;

	m_ComboxBoxGrassTexture.ResetContent();
	BOOL bReturnCode = FileFind.FindFile(strFileFilter);
	while (bReturnCode)
	{
		bReturnCode = FileFind.FindNextFile();
		CString strFilePath = FileFind.GetFilePath();
		_splitpath(strFilePath, NULL, NULL, strFileName, NULL);
		m_ComboxBoxGrassTexture.AddString(strFileName);
	}
	m_nLastSel = -1;
}

void KSceneSceneEditorDialogGrass::OnCbnSelchangeComboGrasstexture()
{
	UpdateTextureInfo(FALSE);
}

void KSceneSceneEditorDialogGrass::OnBnClickedButtonOpengrasstexture()
{
	BROWSEINFO BrowseInfo;
	memset(&BrowseInfo, 0, sizeof(BROWSEINFO));
	BrowseInfo.hwndOwner = GetSafeHwnd();
	LPCITEMIDLIST pItem = SHBrowseForFolder(&BrowseInfo);
	UpdateData(TRUE);
	if (pItem)
	{
		if (!SHGetPathFromIDList(pItem, g_GrassTexturePath))
		{
			sprintf(g_GrassTexturePath, "%s%sData\\source\\maps_source\\Texture\\GrassTexture\\", g_szDefWorkDirectory);
		}
		if (g_GrassTexturePath[strlen(g_GrassTexturePath) - 1] != '\\')
		{
			strcat(g_GrassTexturePath, "\\");
		}
		UpdateData(FALSE);
	}
	OnBnClickedButtonRefreshTexture();
}

void KSceneSceneEditorDialogGrass::OnLbnSelChangeListPattern()
{
	HRESULT hRetCode = E_FAIL;
    int nCount = 0;
	int nSelCount = 0;
	int* pSelPatterns = NULL;
	TCHAR strPatternPathName[MAX_PATH];
 
	IEKG3DTerrain* pTerrainEx = NULL;
	IEKG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr = NULL;
	KG_PROCESS_ERROR(m_lpSceneSceneEditor);
	
	pOutDoorSpaceMgr = m_lpSceneSceneEditor->GetIEKG3DSceneOutDoorSpaceMgr();
	KG_PROCESS_ERROR(pOutDoorSpaceMgr);
	
	nSelCount = m_ListBoxGrassPattern.GetSelCount();
	KG_PROCESS_ERROR(nSelCount > 0);

	pSelPatterns = new int[nSelCount];
	KG_PROCESS_ERROR(pSelPatterns);
	nCount = 0;
	for (int i = 0; i < m_ListBoxGrassPattern.GetCount(); i++)
	{
		int nSel = m_ListBoxGrassPattern.GetSel(i);
		if ( nSel > 0)
		{
			CString strGrassPatternName;
			m_ListBoxGrassPattern.GetText(i, strGrassPatternName);
			switch(m_Radio)
			{
			case MAP_GRASS:
				{
					sprintf(strPatternPathName, "Data\\public\\GrassPattern\\%s.mesh",strGrassPatternName);
					break;
				}
			case MAP_STONE:
				{
					sprintf(strPatternPathName, "Data\\public\\StonePattern\\%s.mesh",strGrassPatternName);
					break;
				}
			case MAP_FRUTEX:
				{
					sprintf(strPatternPathName, "Data\\public\\FrutexPattern\\%s.mesh",strGrassPatternName);
					break;
				}
			default:
				break;
			}
			_strlwr_s(strPatternPathName, sizeof(TCHAR) * MAX_PATH);
            int nPatternIndex = -1;
            hRetCode = pOutDoorSpaceMgr->FindPattern(strPatternPathName,&nPatternIndex,m_Radio);
			
			KGLOG_COM_PROCESS_ERROR(hRetCode);
			ASSERT(nPatternIndex != -1);
			pSelPatterns[nCount] = nPatternIndex;
			nCount++;		
		}
	}
	m_lpSceneSceneEditor->UpdatePatternList(pSelPatterns, nSelCount);
	{
		IEKG3DSceneOutDoorSpaceMgr* pOutDoorSpacemgr = m_lpSceneSceneEditor->GetIEKG3DSceneOutDoorSpaceMgr();
		if(pOutDoorSpacemgr)
			pOutDoorSpacemgr->SetCurGrassSet(m_Radio);
	}
	OnBnClickedCheckGrassadd();
Exit0:
	if (!nSelCount)
	{
		CheckDlgButton(IDC_CHECK_GRASSADD,FALSE);
		CheckDlgButton(IDC_CHECK_GRASSREMOVE,FALSE);
		m_lpSceneSceneEditor->SetEditState(0);//m_dwEditState = 0;
	}
	SAFE_DELETE_ARRAY(pSelPatterns);
	return;
}

void KSceneSceneEditorDialogGrass::SaveTextureSettings()
{
	CString strTextureName;
	TCHAR strIniFileName[MAX_PATH];
	IIniFile* pInIFile = NULL;
	int nReturnCode = 0;

	KG_PROCESS_ERROR(m_nLastSel != -1);

	m_ComboxBoxGrassTexture.GetLBText(m_nLastSel, strTextureName);
	sprintf(strIniFileName, "%s%s.ini", g_GrassTexturePath, strTextureName);

	pInIFile = g_OpenIniFile(strIniFileName, false, true);
	KG_PROCESS_ERROR(pInIFile);

	pInIFile->WriteInteger("Texture", "StrideX", m_nWidhtCount);
	pInIFile->WriteInteger("Texture", "StrideY", m_nHeightCount);

	pInIFile->Save(strIniFileName);
Exit0:
	SAFE_RELEASE(pInIFile);
	return;
}

void KSceneSceneEditorDialogGrass::LoadTextureSettings()
{
	CString strTextureName;
	TCHAR strIniFileName[MAX_PATH];
	int nCurSel = m_ComboxBoxGrassTexture.GetCurSel();
	IIniFile* pInIFile = NULL;
	int nReturnCode = 0;

	KG_PROCESS_ERROR(nCurSel != CB_ERR);

	m_ComboxBoxGrassTexture.GetLBText(nCurSel, strTextureName);
	sprintf(strIniFileName, "%s%s.ini", g_GrassTexturePath, strTextureName);
 
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

void KSceneSceneEditorDialogGrass::OnStnClickedStaticPreview()
{
	POINT point;
	IEKG3DTerrain* pTerrainEx = NULL;
   // INT* pTextureIndex = NULL;
	
	KG_PROCESS_ERROR(m_lpSceneSceneEditor);

	GetCursorPos(&point);
	m_StaticPreview.ScreenToClient(&point);
	SHORT s = GetAsyncKeyState(VK_CONTROL);
	m_Selector.OnSelect(&point, 1, !(0xf000&s));

	INT nNumTextureIndex = 0;
	/*m_Selector.GetSelectSection(&pTextureIndex,nNumTextureIndex);
	m_lpSceneSceneEditor->SetTBModifierTextureIndexInfo(pTextureIndex,&nNumTextureIndex);*/
    UpdateTextureInfo();
Exit0:
   // SAFE_DELETE_ARRAY(pTextureIndex);
	return;
}
void KSceneSceneEditorDialogGrass::OnBnClickedStatic4()
{
    // TODO: Add your control notification handler code here
}

void KSceneSceneEditorDialogGrass::OnBnClickedRadio2()
{

}

void KSceneSceneEditorDialogGrass::OnBnClickedRadio3()
{

}

void KSceneSceneEditorDialogGrass::OnBnClickedRadioGrass()
{
	// TODO: Add your control notification handler code here
	KG_PROCESS_ERROR(m_lpSceneSceneEditor);
	GetDlgItem(IDC_EDIT_HEIGHTMIN)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_HEIGHTMAX)->EnableWindow(TRUE);
	ChangeDefault("100","200","100","200","10");
	
	m_Radio = MAP_GRASS;
	sprintf(g_GrassTexturePath, "%sData\\source\\maps_source\\Texture\\GrassTexture\\", g_szDefWorkDirectory);
	OnBnClickedButtonRefresh();
	OnBnClickedButtonRefreshTexture();
	OnLbnSelChangeListPattern();
	
	IEKG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr = NULL;
	pOutDoorSpaceMgr = m_lpSceneSceneEditor->GetIEKG3DSceneOutDoorSpaceMgr();
	KG_PROCESS_ERROR(pOutDoorSpaceMgr);
	pOutDoorSpaceMgr->SetCurGrassSet(m_Radio);
Exit0:
	return ;
}

void KSceneSceneEditorDialogGrass::OnBnClickedRadioStone()
{
	// TODO: Add your control notification handler code here
	KG_PROCESS_ERROR(m_lpSceneSceneEditor);
	ChangeDefault("10","100","10","100","5");
	GetDlgItem(IDC_EDIT_HEIGHTMIN)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_HEIGHTMAX)->EnableWindow(FALSE);
	
	m_Radio = MAP_STONE;
	sprintf(g_GrassTexturePath, "%sData\\source\\maps_source\\Texture\\StoneTexture\\", g_szDefWorkDirectory);
	OnBnClickedButtonRefresh();
	OnBnClickedButtonRefreshTexture();
	OnLbnSelChangeListPattern();

	IEKG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr = NULL;
	pOutDoorSpaceMgr = m_lpSceneSceneEditor->GetIEKG3DSceneOutDoorSpaceMgr();
	KG_PROCESS_ERROR(pOutDoorSpaceMgr);
	pOutDoorSpaceMgr->SetCurGrassSet(m_Radio);
	CheckButtonEnable();
Exit0:
	return ;
}

void KSceneSceneEditorDialogGrass::OnBnClickedRadioFrutex()
{
	// TODO: Add your control notification handler code here
	KG_PROCESS_ERROR(m_lpSceneSceneEditor);
	GetDlgItem(IDC_EDIT_HEIGHTMIN)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_HEIGHTMAX)->EnableWindow(TRUE);
	ChangeDefault("100","200","400","600","1");
	
	m_Radio = MAP_FRUTEX;
	sprintf(g_GrassTexturePath, "%sData\\source\\maps_source\\Texture\\FrutexTexture\\", g_szDefWorkDirectory);
	OnBnClickedButtonRefresh();
	OnBnClickedButtonRefreshTexture();
	OnLbnSelChangeListPattern();

    IEKG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr = NULL;
	pOutDoorSpaceMgr = m_lpSceneSceneEditor->GetIEKG3DSceneOutDoorSpaceMgr();
	KG_PROCESS_ERROR(pOutDoorSpaceMgr);
	pOutDoorSpaceMgr->SetCurGrassSet(m_Radio);
	CheckButtonEnable();
Exit0:
	return ;
}

void KSceneSceneEditorDialogGrass::ChangeDefault(LPCTSTR WidthMin ,LPCTSTR WidthMax,LPCTSTR HeightMin,LPCTSTR HeightMax,LPCTSTR NumPerM2)
{
	GetDlgItem(IDC_EDIT_WIDTHMIN)->SetWindowText(WidthMin);
	GetDlgItem(IDC_EDIT_WIDTHMAX)->SetWindowText(WidthMax);
	GetDlgItem(IDC_EDIT_HEIGHTMIN)->SetWindowText(HeightMin);
	GetDlgItem(IDC_EDIT_HEIGHTMAX)->SetWindowText(HeightMax);
	GetDlgItem(IDC_EDIT_NUMBER)->SetWindowText(NumPerM2);
}

void KSceneSceneEditorDialogGrass::OnBnClickedCheckAlphablend()
{
	// TODO: Add your control notification handler code here
	IKG3DEngineManager* pMgr = g_GetEngineManager();
	KG3DEngineOption vOption;
	pMgr->GetEngineOption(&vOption);

	int nCheck = ((CButton*)GetDlgItem(IDC_CHECK_ALPHABLEND))->GetCheck();
	vOption.bGrassAlphaBlend = nCheck;

	pMgr->SetEngineOption(&vOption);
}

void KSceneSceneEditorDialogGrass::OnBnClickedButtonDelAll()
{
	// TODO: Add your control notification handler code here
	if (m_lpSceneSceneEditor)
	{
		TCHAR strText[MAX_PATH];
		switch(m_Radio)
		{
		case MAP_GRASS:
			sprintf(strText, "确定要删除整地图的草？");
			break;
		case MAP_STONE:
			sprintf(strText, "确定要删除整地图的碎石？");
			break;
		case MAP_FRUTEX:
			sprintf(strText, "确定要删除整地图的灌木？");
			break;
		}
		if(IDYES == MessageBox(strText,"提示！！",MB_YESNO))
			m_lpSceneSceneEditor->ClearAllTerrainObject();
	}

}

void KSceneSceneEditorDialogGrass::CheckButtonEnable()
{
	BOOL bEnable = FALSE;
	if(m_Radio == MAP_STONE || m_Radio == MAP_FRUTEX )
		bEnable = TRUE;

	((CButton*)GetDlgItem(IDC_COMBO_GRASSTEXTURE))->EnableWindow(bEnable);
	((CButton*)GetDlgItem(IDC_BUTTON_OPENGRASSTEXTURE))->EnableWindow(bEnable);
	((CButton*)GetDlgItem(IDC_BUTTON_REFRESHTEXTURE))->EnableWindow(bEnable);
	((CButton*)GetDlgItem(IDC_EDIT_HSPLIT))->EnableWindow(bEnable);
	((CButton*)GetDlgItem(IDC_EDIT_VSPLIT))->EnableWindow(bEnable);
	((CButton*)GetDlgItem(IDC_EDIT_WIDTHMIN))->EnableWindow(bEnable);
	((CButton*)GetDlgItem(IDC_EDIT_WIDTHMAX))->EnableWindow(bEnable);
	((CButton*)GetDlgItem(IDC_EDIT_HEIGHTMIN))->EnableWindow(bEnable);
	((CButton*)GetDlgItem(IDC_EDIT_HEIGHTMAX))->EnableWindow(bEnable);
	((CButton*)GetDlgItem(IDC_EDIT_TEXTUREMIN))->EnableWindow(bEnable);
	((CButton*)GetDlgItem(IDC_EDIT_TEXTUREMAX))->EnableWindow(bEnable);
	((CButton*)GetDlgItem(IDC_EDIT_ALPHA))->EnableWindow(bEnable);
	((CButton*)GetDlgItem(IDC_EDIT_NUMBER))->EnableWindow(bEnable);
	((CButton*)GetDlgItem(IDC_CHECK_ALLEY))->EnableWindow(bEnable);
	((CButton*)GetDlgItem(IDC_CHECK_GRASSADD))->EnableWindow(bEnable);
	((CButton*)GetDlgItem(IDC_CHECK_GRASSREMOVE))->EnableWindow(bEnable);
	((CButton*)GetDlgItem(IDC_BUTTON_DEL_ALL))->EnableWindow(bEnable);
	((CButton*)GetDlgItem(IDC_BUTTON_AUTOLIGHT))->EnableWindow(bEnable);
	((CButton*)GetDlgItem(IDC_BUTTON_FITGROUND))->EnableWindow(bEnable);
	((CButton*)GetDlgItem(IDC_LIST_PATTERN))->EnableWindow(bEnable);
	((CButton*)GetDlgItem(IDC_BUTTON_REFRESH))->EnableWindow(bEnable);
	((CButton*)GetDlgItem(IDC_STATIC_PREVIEW))->EnableWindow(bEnable);

}
