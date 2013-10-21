// KSceneModelEditorPropertyPageMaterial.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KSceneModelEditorPropertyPageMaterial.h"
#include "KColorDialog.h"
#include "IEKG3DTexturetable.h"
#include "IEKG3DModelSpeedTree.h"
#include "KDlg_TextureStageState.h"
#include "IEEditor.h"
#include "IEKG3DTexture.h"
#include "KModelEditorMaterialDetailDialog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


TCHAR* strMapSemantics[3] = {
	"diffusemap",
	"ambientmap",
	"specularmap",
};

const static DWORD s_dwMaxNumTextureStage = 8;

static TCHAR s_strCurrentPathSave[MAX_PATH];
static BOOL s_bSaved = FALSE;
void SetDefaultPath()
{
	s_bSaved = TRUE;
	::GetCurrentDirectory(MAX_PATH, s_strCurrentPathSave);
	::SetCurrentDirectory(g_szDefWorkDirectory);
}

void RestorePath()
{
	if (s_bSaved)
	{
		::SetCurrentDirectory(s_strCurrentPathSave);
		s_bSaved = FALSE;
	}
}

//static KG3DMaterial_Helper_AutoMapFinder MapFinder;//没有用到，去掉了// Last modify by Chen Tianhong: 2007-9-29 15:10:46


/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorPropertyPageMaterial property page

IMPLEMENT_DYNCREATE(KSceneModelEditorPropertyPageMaterial, CPropertyPage)

static BOOL CheckFileReadOnly(LPCSTR strPathName)
{
	BOOL bReturn = FALSE;
	
	DWORD dwFileAttributes = GetFileAttributes(strPathName);
	if ((dwFileAttributes != INVALID_FILE_ATTRIBUTES) &&
		(dwFileAttributes & FILE_ATTRIBUTE_READONLY))
	{
		bReturn = TRUE;
		TCHAR strOutputInfo[MAX_PATH * 2];
		sprintf_s(strOutputInfo, 
			MAX_PATH * 2,
			"文件是%s是只读的， 去掉只读属性之后继续.",
			strPathName);
		MessageBox(NULL, strOutputInfo, "提示", MB_OK);
	}
	return bReturn;
}

KSceneModelEditorPropertyPageMaterial::KSceneModelEditorPropertyPageMaterial() : CPropertyPage(KSceneModelEditorPropertyPageMaterial::IDD)
, m_bSortAsSFX(FALSE)
{
	//{{AFX_DATA_INIT(KSceneModelEditorPropertyPageMaterial)
	m_bShowSelectOnly = FALSE;
	m_fAlpha = 0.0f;
	m_fPower = 0.0f;
	//}}AFX_DATA_INIT
	m_lpCurModel = NULL;
	m_lpMaterialAlphaDialog = NULL;
	m_bAutoModify = TRUE;
	m_nBaseMaterial = -1;
	m_dwDirtyTextureMask = 0x00000000;
	m_lpDetailDialog = NULL;
}


KSceneModelEditorPropertyPageMaterial::~KSceneModelEditorPropertyPageMaterial()
{
	//SAFE_DELETE(m_lpMaterialAlphaDialog);
	//m_vecSemantic2MapFinder.clear();
	//SAFE_RELEASE(m_lpCurModel);
	SAFE_DELETE(m_lpDetailDialog);
}

void KSceneModelEditorPropertyPageMaterial::SetCurModel(IEKG3DModel* pCurModel)
{
	if(m_lpCurModel != pCurModel)
	{
		//SAFE_RELEASE(m_lpCurModel);
		m_lpCurModel = pCurModel;
		//m_lpCurModel->AddRef();
		FillListBoxSubset();

		if(m_lpDetailDialog)
		{
			IEKG3DMaterial* pMat = NULL;
			KG_PROCESS_ERROR(m_lpCurModel);

			pMat = GetMaterialFromModel();
			KG_PROCESS_ERROR(pMat);

			m_lpDetailDialog->ShowWindow(SW_HIDE);
			m_lpDetailDialog->SetMaterial(pMat,m_lpCurModel);
            m_lpDetailDialog->SetParentHwnd(this);
			m_lpDetailDialog->ShowWindow(SW_SHOWNORMAL);
		}
	}
Exit0:
	;
}
void KSceneModelEditorPropertyPageMaterial::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KSceneModelEditorPropertyPageMaterial)
	DDX_Control(pDX, IDC_CHECK_HIDE, m_ButtonHide);
	DDX_Control(pDX, IDC_CHECK_ZWRITE, m_ButtonZWrite);
	DDX_Control(pDX, IDC_STATIC_TEXTURE, m_Static_Texture);
	DDX_Control(pDX, IDC_BUTTON_TEXTURETRANS, m_ButtonTextureTranslation);
	DDX_Control(pDX, IDC_BUTTON_TEXTUREREMOVE, m_ButtonTextureRemove);
	DDX_Control(pDX, IDC_BUTTON_TEXTUREOP, m_ButtonTextureOperation);
	DDX_Control(pDX, IDC_BUTTON_TEXTUREMAP, m_ButtonTextureMap);
	DDX_Control(pDX, IDC_BUTTON_TEXTUREAPPLY, m_ButtonTextureApplySet);
	DDX_Control(pDX, IDC_BUTTON_TEXTUREADD, m_ButtonTextureAdd);
	DDX_Control(pDX, IDC_EDIT_DIFFUSEALPHA, m_EditDiffuseAlpha);
	DDX_Control(pDX, IDC_EDIT_POWER, m_EditPower);
	DDX_Control(pDX, IDC_LIST_TEXTURES, m_ctlTextureList);
	DDX_Control(pDX, IDC_BUTTON_DIFFUSE, m_ColorButtonDiffuse);
	DDX_Control(pDX, IDC_BUTTON_SPECULAR, m_ColorButtonSpecular);
	DDX_Control(pDX, IDC_BUTTON_EMISSIVE, m_ColorButtonEmissive);
	DDX_Control(pDX, IDC_BUTTON_AMBIENT, m_ColorButtonAmbient);
	DDX_Control(pDX, IDC_CHECK_ZBUFFER, m_ButtonZbuffer);
	DDX_Control(pDX, IDC_CHECK_SPECULAR, m_ButtonSpecular);
	DDX_Control(pDX, IDC_CHECK_LIGHTING, m_ButtonLighting);
	DDX_Control(pDX, IDC_CHECK_2FACE, m_Button2Face);
	DDX_Control(pDX, IDC_STATIC_SUBSET, m_Static_Subset);
	DDX_Control(pDX, IDC_LIST_SUBSET, m_ctlSubsetList);
	DDX_Check(pDX, IDC_CHECK_SHOWSELECTONLY, m_bShowSelectOnly);
	DDX_Text(pDX, IDC_EDIT_DIFFUSEALPHA, m_fAlpha);
	DDX_Text(pDX, IDC_EDIT_POWER, m_fPower);
	DDX_Text(pDX, IDC_EDIT_SPEC_POWER, m_fSpecPower);
	DDX_Text(pDX, IDC_EDIT_EMSS_POWER, m_fEmssPower);
	DDX_Control(pDX, IDC_EDIT_SPEC_POWER, m_EditEmssPower);
	DDX_Control(pDX, IDC_EDIT_EMSS_POWER, m_EditSpecPower);
	DDX_Control(pDX, IDC_COMBO_BLENDMODE, m_ctlBlendMode);
	DDX_Control(pDX, IDC_CHECK_AUTOCHANGE, m_ctlAutoChange);
	DDX_Control(pDX, IDC_CHECK_NOCELL, m_ButtonNoCell);
	DDX_Control(pDX, IDC_CHECK_SPEC, m_CheckSpec);
	DDX_Control(pDX, IDC_CHECK_EMAP, m_CheckEMap);
	DDX_Control(pDX, IDC_BUTTON_COLOR_CAST, m_ColorCastBn);
	DDX_Control(pDX, IDC_BUTTON_COLOR_CAST1, m_ColorCastBn1);
	DDX_Control(pDX, IDC_BUTTON_COLOR_CAST2, m_ColorCastBn2);
	DDX_Control(pDX, IDC_BUTTON_COLOR_CAST3, m_ColorCastBn3);
	DDX_Control(pDX, IDC_BUTTON_COLOR_CAST4, m_ColorCastBn4);
	DDX_Control(pDX, IDC_BUTTON_COLOR_CAST5, m_ColorCastBn5);
	DDX_Control(pDX, IDC_BUTTON_COLOR_CAST6, m_ColorCastBn6);
	DDX_Control(pDX, IDC_BUTTON_COLOR_CAST7, m_ColorCastBn7);
	DDX_Text(pDX, IDC_EDIT_TWICEALPHATOP, m_dwAlphaTop);
	DDX_Text(pDX, IDC_EDIT_TWICEALPHABOTTOM, m_dwAlphaBottom);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_CHECK_SORTASSFX, m_bSortAsSFX);
}


BEGIN_MESSAGE_MAP(KSceneModelEditorPropertyPageMaterial, CPropertyPage)
	//{{AFX_MSG_MAP(KSceneModelEditorPropertyPageMaterial)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_BUTTON, OnButtonButton)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, OnButtonDown)
	ON_BN_CLICKED(IDC_BUTTON_TOP, OnButtonTop)
	ON_BN_CLICKED(IDC_BUTTON_UNIT, OnButtonUnit)
	ON_BN_CLICKED(IDC_BUTTON_UP, OnButtonUp)
	ON_BN_CLICKED(IDC_CHECK_SHOWSELECTONLY, OnCheckShowselectonly)
	ON_LBN_SELCHANGE(IDC_LIST_SUBSET, OnSelchangeListSubset)
	ON_BN_CLICKED(IDC_BUTTON_SELECTALL, OnButtonSelectall)
	ON_BN_CLICKED(IDC_BUTTON_SELECTINVERT, OnButtonSelectinvert)
	ON_BN_CLICKED(IDC_BUTTON_SELECTNONE, OnButtonSelectnone)
	ON_BN_CLICKED(IDC_CHECK_ZBUFFER, OnCheckZbuffer)
	ON_BN_CLICKED(IDC_CHECK_SPECULAR, OnCheckSpecular)
	ON_BN_CLICKED(IDC_CHECK_LIGHTING, OnCheckLighting)
	ON_BN_CLICKED(IDC_CHECK_2FACE, OnCheck2face)
	ON_BN_CLICKED(IDC_BUTTON_TEXTUREADD, OnButtonTextureadd)
	ON_BN_CLICKED(IDC_BUTTON_TEXTUREREMOVE, OnButtonTextureremove)
	ON_LBN_DBLCLK(IDC_LIST_TEXTURES, OnDblclkListTextures)
	ON_LBN_SELCHANGE(IDC_LIST_TEXTURES, OnSelchangeListTextures)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_SAVEID, OnBnClickedButtonSaveid)
	ON_BN_CLICKED(IDC_CHECK_ZWRITE, OnBnClickedCheckZwrite)
	ON_BN_CLICKED(IDC_CHECK_HIDE, OnBnClickedCheckHide)
	ON_BN_CLICKED(IDC_BUTTON_AMBIENT, OnBnClickedButtonAmbient)
	ON_BN_CLICKED(IDC_BUTTON_DIFFUSE, OnBnClickedButtonDiffuse)
	ON_BN_CLICKED(IDC_BUTTON_SPECULAR, OnBnClickedButtonSpecular)
	ON_BN_CLICKED(IDC_BUTTON_EMISSIVE, OnBnClickedButtonEmissive)
	ON_BN_CLICKED(IDC_BUTTON_TEXTUREMAP, OnBnClickedButtonTexturemap)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_TEXTUREOP, OnBnClickedButtonTextureop)
	ON_CBN_SELCHANGE(IDC_COMBO_ZFUN, &KSceneModelEditorPropertyPageMaterial::OnCbnSelchangeComboZfun)
	ON_CBN_SELCHANGE(IDC_COMBO_BLENDMODE, &KSceneModelEditorPropertyPageMaterial::OnCbnSelchangeComboBlendmode)
	ON_BN_CLICKED(IDC_BUTTON_BLENDADV, &KSceneModelEditorPropertyPageMaterial::OnBnClickedButtonBlendadv)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CHECK_NOCELL, &KSceneModelEditorPropertyPageMaterial::OnBnClickedCheckNocell)
    ON_BN_CLICKED(IDC_CHECK_SPEC, &KSceneModelEditorPropertyPageMaterial::OnBnClickedCheckSpec)
    ON_BN_CLICKED(IDC_CHECK_EMAP, &KSceneModelEditorPropertyPageMaterial::OnBnClickedCheckEmap)
    ON_BN_CLICKED(IDC_FLUSH_TEX,  &KSceneModelEditorPropertyPageMaterial::OnBnClickedFlushTexs)
	ON_EN_CHANGE(IDC_EDIT_TWICEALPHATOP, &KSceneModelEditorPropertyPageMaterial::OnEnChangeEditTwicealphatop)
	ON_EN_CHANGE(IDC_EDIT_TWICEALPHABOTTOM, &KSceneModelEditorPropertyPageMaterial::OnEnChangeEditTwicealphatop)
	ON_BN_CLICKED(IDC_BUTTON_SAVEAS, &KSceneModelEditorPropertyPageMaterial::OnBnClickedButtonSaveas)
	ON_BN_CLICKED(IDC_CHECK_SORTASSFX, &KSceneModelEditorPropertyPageMaterial::OnBnClickedCheckSortassfx)
	ON_BN_CLICKED(IDC_BUTTON_DETAIL, &KSceneModelEditorPropertyPageMaterial::OnBnClickedButtonDetail)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorPropertyPageMaterial message handlers


BOOL KSceneModelEditorPropertyPageMaterial::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	InitButtonMaps(&m_ColorButtonAmbient, ENUM_BUTTON_AMBIENT, 0);
	InitButtonMaps(&m_ColorButtonSpecular, ENUM_BUTTON_SPECULAR, 0);
	InitButtonMaps(&m_ColorButtonEmissive, ENUM_BUTTON_EMISSIVE, 0);
	InitButtonMaps(&m_ColorButtonDiffuse, ENUM_BUTTON_DIFFUSE, 0);

	CComboBox* pZFuncList = static_cast<CComboBox*>(GetDlgItem(IDC_COMBO_ZFUN));
	for (int i = 0; i < sizeof(strZFunc) / sizeof(TCHAR*); i++)
	{
		pZFuncList->AddString(strZFunc[i]);
	}
	FillListBoxSubset();
	m_ctlAutoChange.SetCheck(BST_CHECKED);

	{
		IEKG3DMaterial* pMaterial = NULL;
		KG_PROCESS_ERROR(m_lpCurModel);
		pMaterial = GetMaterialFromModel();
		KGLOG_PROCESS_ERROR(pMaterial);
		m_bSortAsSFX = pMaterial->IsSortAsSFX();

		UpdateData(FALSE);

	}

Exit0:

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void KSceneModelEditorPropertyPageMaterial::FillListBoxTexture(int nIndex)
{
	HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
	TCHAR strOutputInfo[MAX_PATH];
	TCHAR strTextureName[MAX_PATH];
	TCHAR strTextureExt[MAX_PATH];
    IEKG3DMaterial *piMaterial = NULL;
    int nCount = m_ctlSubsetList.GetCount();
	IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
    m_ctlTextureList.ResetContent();
	if (nIndex == -1)
	{
		m_ctlTextureList.ResetContent();
		m_ctlTextureList.EnableWindow(FALSE);
		goto Exit0;
	}

	KG_PROCESS_ERROR(m_lpCurModel);
	KG_PROCESS_ERROR(nCount > nIndex);
	m_ctlTextureList.EnableWindow(TRUE);

    hrRetCode = m_lpCurModel->GetIEKG3DMaterial(&piMaterial);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

	for(DWORD i = 0; i < s_dwMaxNumTextureStage; i++)
	{
        TEXTURE_LOAD_STATE eLoadState = TEXTURE_LOAD_UNKNOWN_ERROR;
        hrRetCode = piMaterial->GetTexturePathName(nIndex, i, m_strFullPath[i], MAX_PATH, &eLoadState);
        KGLOG_COM_PROCESS_ERROR(hrRetCode);
        
        if (eLoadState == TEXTURE_LOAD_NOT_EXIST)
        {
            sprintf_s(strOutputInfo, MAX_PATH, "图层%d 没有贴图", i);
        }
        else
        {
            ASSERT(eLoadState != TEXTURE_LOAD_UNKNOWN_ERROR);

            _splitpath_s(
                m_strFullPath[i], 
                NULL, 0, 
                NULL, 0, 
                strTextureName, MAX_PATH,
                strTextureExt,  MAX_PATH
            );
            if (eLoadState == TEXTURE_LOAD_SUCCESS)
                sprintf_s(strOutputInfo, "图层%d %s%s", i, strTextureName, strTextureExt);
            else
                sprintf_s(strOutputInfo, "图层%d <错误>%s%s", i, strTextureName, strTextureExt);
        }
		m_ctlTextureList.AddString(strOutputInfo);
	}
	OnSelchangeListTextures();
    
    hrResult = S_OK;
Exit0:
	return;
}

void KSceneModelEditorPropertyPageMaterial::FillListBoxSubset()
{
    HRESULT hr = E_FAIL;
	IEKG3DMaterial* pMaterial = NULL;
	TCHAR strSubsetName[256];
	KG_PROCESS_ERROR(::IsWindow(GetSafeHwnd()));
	m_ctlSubsetList.ResetContent();
    int nCount = 0;
    PVOID pSubMaterial = NULL;

	KG_PROCESS_ERROR(m_lpCurModel);

	pMaterial = GetMaterialFromModel();

	KGLOG_PROCESS_ERROR(pMaterial);

    hr = m_lpCurModel->GetNumMaterial(&nCount);
    KG_COM_PROCESS_ERROR(hr);

	KG_PROCESS_ERROR(nCount);

	GetDlgItem(IDC_LIST_SUBSET)->EnableWindow(TRUE);


	for (DWORD i = 0; i < (DWORD)nCount; ++i)
	{
        DWORD dwOption = 0;

        hr = m_lpCurModel->GetMaterialSubsetOption(i, &dwOption);
        KG_COM_PROCESS_ERROR(hr);

        if (dwOption & MATERIAL_OPTION_HIDE)
		{
			sprintf(strSubsetName, "Subset %d (隐藏)", i);
		}
		else
		{
			sprintf(strSubsetName, "Subset %d", i);
		}
		m_ctlSubsetList.InsertString(i, strSubsetName);
		pMaterial->GetSubMaterial(i, &pSubMaterial);

        m_ctlSubsetList.SetItemData(i , reinterpret_cast<DWORD_PTR>(pSubMaterial));

		if (dwOption & MATERIAL_OPTION_SELECTED)
		{
			m_ctlSubsetList.SetSel(i);
		}
	}
	sprintf(strSubsetName,"Total Subset %d", nCount);//by dengzhihui Refactor 2006年11月13日
	m_Static_Subset.SetWindowText(strSubsetName);

	OnSelchangeListSubset();

Exit0:
	return;
}


void KSceneModelEditorPropertyPageMaterial::OnButtonButton()
{
	// TODO: Add your control notification handler code here
	HRESULT hr = E_FAIL;
	DWORD * pNewSubsetID = NULL;
	IEKG3DMesh* pMesh = NULL;//by dengzhihui Refactor 2006年11月7日
	int nSelCount = 0;
	int nNumMat = 0;

	if(!m_lpCurModel)
		goto Exit0;

	KGLOG_PROCESS_ERROR(m_lpCurModel);

	pMesh = GetReleasedMesh();
	KGLOG_PROCESS_ERROR(pMesh);

	nSelCount = m_ctlSubsetList.GetSelCount();

    hr = m_lpCurModel->GetNumMaterial(&nNumMat);
    KG_COM_PROCESS_ERROR(hr);

    if((nSelCount == 0)||( nSelCount >= nNumMat))
		goto Exit0;
	pNewSubsetID = new DWORD[nNumMat];
	ZeroMemory(pNewSubsetID, sizeof(DWORD) * nNumMat);

	for(int i = 0;i< nNumMat; i++)
	{
		pNewSubsetID[i] = i;
	}

	for(int i = 0; i < nNumMat; i++)
	{
		if ((i < nNumMat - 1) && (m_ctlSubsetList.GetSel(i)))
		{
			for(int j = i; j< nNumMat; j++)
			{
				pNewSubsetID[j] = pNewSubsetID[j+1];
			}
			pNewSubsetID[nNumMat-1] = i;
		}
	}

	m_lpCurModel->ModifySubsetID(pNewSubsetID);
	FillListBoxSubset();

Exit0:
	SAFE_DELETE_ARRAY(pNewSubsetID);
	return;
}

void KSceneModelEditorPropertyPageMaterial::OnButtonDelete()
{
	// TODO: Add your control notification handler code here

}

void KSceneModelEditorPropertyPageMaterial::OnButtonDown()
{
	int nCurSel = m_ctlSubsetList.GetCurSel();

    IEKG3DEngineManager *pManager = g_GetEngineIEInterface();

	KG_PROCESS_ERROR(m_lpCurModel);
	KG_PROCESS_ERROR(nCurSel != LB_ERR);
	KG_PROCESS_ERROR(nCurSel != m_ctlSubsetList.GetCount() - 1);

    //Get3DEngineInterface((void **)(&pManager));

	pManager->ExchangeModelSubsetID(m_lpCurModel, nCurSel, nCurSel + 1);
	FillListBoxSubset();
Exit0:
	return;
}

//(by dengzhihui Refactor 2006年11月7日
IEKG3DMesh* KSceneModelEditorPropertyPageMaterial::GetReleasedMesh()
{
	HRESULT hRetCode = E_FAIL;
    KGLOG_PROCESS_ERROR(m_lpCurModel);
	IEKG3DMesh* pMesh = NULL;
	
	hRetCode = m_lpCurModel->GetIEKG3DMesh(&pMesh);
	KGLOG_COM_PROCESS_ERROR(hRetCode);

	return pMesh;
Exit0:
	return NULL;
}
void KSceneModelEditorPropertyPageMaterial::OnButtonTop()
{
	DWORD * pNewSubsetID = NULL;
	int nSelCount = 0;
	int nNumMats = 0;

	KGLOG_PROCESS_ERROR(m_lpCurModel);

    HRESULT hr = m_lpCurModel->GetNumMaterial(&nNumMats);
    KG_COM_PROCESS_ERROR(hr);

    nSelCount = m_ctlSubsetList.GetSelCount();
	if((nSelCount == 0) || (nSelCount >= nNumMats))
		goto Exit0;

	pNewSubsetID = new DWORD[nNumMats];
	ZeroMemory(pNewSubsetID, sizeof(DWORD) * nNumMats);

	for(int i = 0; i < nNumMats; i++)
	{
		pNewSubsetID[i] = i;
	}

	for(int i = 0; i < nNumMats; i++)
	{
		if ((i > 0) && (m_ctlSubsetList.GetSel(i)))
		{
			for(int j = i - 1; j >= 0; j--)
			{
				pNewSubsetID[j + 1] = pNewSubsetID[j];
			}
			pNewSubsetID[0] = i;
		}
	}
	m_lpCurModel->ModifySubsetID(pNewSubsetID);
	FillListBoxSubset();

Exit0:
	SAFE_DELETE_ARRAY(pNewSubsetID);
	return;
}
//)


void KSceneModelEditorPropertyPageMaterial::OnButtonUnit()
{
	// TODO: Add your control notification handler code here

}


void KSceneModelEditorPropertyPageMaterial::OnButtonUp()
{
	int nCurSel = m_ctlSubsetList.GetCurSel();
    IEKG3DEngineManager *pManager = g_GetEngineIEInterface();

	KG_PROCESS_ERROR(m_lpCurModel);
	KG_PROCESS_ERROR(nCurSel != LB_ERR);
	KG_PROCESS_ERROR(nCurSel != 0);

    //Get3DEngineInterface((void **)(&pManager));

	pManager->ExchangeModelSubsetID(m_lpCurModel, nCurSel, nCurSel - 1);

	FillListBoxSubset();
Exit0:
	return;
}

void KSceneModelEditorPropertyPageMaterial::OnCheckShowselectonly()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}

void KSceneModelEditorPropertyPageMaterial::OnSelchangeListSubset()
{
    HRESULT hr = E_FAIL;
	int nSelCount = m_ctlSubsetList.GetSelCount();
	int nCurSel = LB_ERR;
	_MtlAlphaOperation* pAlphaOp = NULL;
	CWnd* pControl = NULL;
	TCHAR strInfo[256];
	//COLORREF crColor;
    int nCount = 0;
    DWORD dwOption = 0;
    int nSelData = 0;
    BOOL bHas = FALSE;

	m_nBaseMaterial = -1;

	for (int i = 0; i < m_ctlSubsetList.GetCount(); i++)
	{
        hr = m_lpCurModel->GetMaterialSubsetOption(i, &dwOption);
        KG_COM_PROCESS_ERROR(hr);

        if (m_ctlSubsetList.GetSel(i))
		{
			nCurSel = i;
			//(//by dengzhihui Refactor 2006年11月9日
			dwOption |= MATERIAL_OPTION_SELECTED;
			dwOption &= ~MATERIAL_OPTION_FILL_WIREFRAME;
			dwOption |= MATERIAL_OPTION_FILL_SOLID;
			//)
		}
		else
		{
			//(by dengzhihui Refactor 2006年11月9日
			dwOption &= ~MATERIAL_OPTION_SELECTED;
			dwOption |= MATERIAL_OPTION_FILL_WIREFRAME;
			dwOption &= ~MATERIAL_OPTION_FILL_SOLID;
			//)
		}
        hr = m_lpCurModel->SetMaterialSubsetOption(i, dwOption);
        KG_COM_PROCESS_ERROR(hr);
    }

	if (nCurSel == LB_ERR)
	{
		//Disable all
		GetDlgItem(IDC_CHECK_ZBUFFER)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_LIGHTING)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_SPECULAR)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_ZWRITE)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_HIDE)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_2FACE)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_NOCELL)->EnableWindow(FALSE);

		m_ColorButtonDiffuse.EnableWindow(FALSE);
		m_ColorButtonSpecular.EnableWindow(FALSE);;
		m_ColorButtonEmissive.EnableWindow(FALSE);;
		m_ColorButtonAmbient.EnableWindow(FALSE);

		GetDlgItem(IDC_EDIT_POWER)->EnableWindow(FALSE);
		//GetDlgItem(IDC_EDIT_ALPHA)->EnableWindow(FALSE);

		FillListBoxTexture(nCurSel);

        GetDlgItem(IDC_COMBO_ZFUN)->EnableWindow(FALSE);

		m_ctlBlendMode.EnableWindow(FALSE);
	}
	else
	{
		KG_PROCESS_ERROR(m_lpCurModel);

        hr = m_lpCurModel->GetNumMaterial(&nCount);
        KG_COM_PROCESS_ERROR(hr);

		KG_PROCESS_ERROR(nCount > nCurSel);

		//Z buffer state
		pControl = GetDlgItem(IDC_CHECK_ZBUFFER);
		KG_PROCESS_ERROR(pControl);
		pControl->EnableWindow(TRUE);

        hr = m_lpCurModel->GetMaterialSubsetOption(nCurSel, &dwOption);
        KG_COM_PROCESS_ERROR(hr);

		if (dwOption & MATERIAL_OPTION_ZBUFFER_TRUE) //by dengzhihui Refactor 2006年11月9日
		{
			CComboBox* pZFunc = static_cast<CComboBox*>(GetDlgItem(IDC_COMBO_ZFUN));
			static_cast<CButton*>(pControl)->SetCheck(BST_CHECKED);

			pZFunc->EnableWindow(TRUE);
			//int nSel = m_modelex.GetMaterialSubsetMtlOptionData(nCurSel, OPTION_MATERIAL_ZFUN, 0xffffffff) - 1;

            hr = m_lpCurModel->GetMaterialSubsetMtlOptionData(nCurSel, OPTION_MATERIAL_ZFUN, 0xffffffff, &nSelData);
            KG_COM_PROCESS_ERROR(hr);

            pZFunc->SetCurSel(nSelData);

		}
        hr = m_lpCurModel->GetMaterialSubsetOption(nCurSel, &dwOption);
        KG_COM_PROCESS_ERROR(hr);

        if (dwOption & MATERIAL_OPTION_ZBUFFER_FALSE)
		{
			static_cast<CButton*>(pControl)->SetCheck(BST_UNCHECKED);
			GetDlgItem(IDC_COMBO_ZFUN)->EnableWindow(FALSE);
		}

		//lighting option
		pControl = GetDlgItem(IDC_CHECK_LIGHTING);
		KG_PROCESS_ERROR(pControl);
		pControl->EnableWindow(TRUE);

		//if (pSubMat->m_dwOption & MATERIAL_OPTION_LIGHTING)//by dengzhihui Refactor 2006年11月9日
		if (dwOption & MATERIAL_OPTION_LIGHTING)
		{
			static_cast<CButton*>(pControl)->SetCheck(BST_CHECKED);
		}
		else
		{
			static_cast<CButton*>(pControl)->SetCheck(BST_UNCHECKED);
		}

		//specular and 2x texture option
		pControl = GetDlgItem(IDC_CHECK_SPECULAR);
		KG_PROCESS_ERROR(pControl);
		pControl->EnableWindow(TRUE);
		if (dwOption & MATERIAL_OPTION_SPECULARENABLE)//by dengzhihui Refactor 2006年11月9日
		{
			static_cast<CButton*>(pControl)->SetCheck(BST_CHECKED);
		}
		else
		{
			static_cast<CButton*>(pControl)->SetCheck(BST_UNCHECKED);
		}

		//z write option
		pControl = GetDlgItem(IDC_CHECK_ZWRITE);
		pControl->EnableWindow(TRUE);
		if (dwOption & MATERIAL_OPTION_ZWRITEDISABLE)//by dengzhihui Refactor 2006年11月9日
		{
			static_cast<CButton*>(pControl)->SetCheck(BST_UNCHECKED);
		}
		else
		{
			static_cast<CButton*>(pControl)->SetCheck(BST_CHECKED);
		}


		//hide subset option
		pControl = GetDlgItem(IDC_CHECK_HIDE);
		pControl->EnableWindow(TRUE);
		if (dwOption & MATERIAL_OPTION_HIDE)//by dengzhihui Refactor 2006年11月9日
		{
			static_cast<CButton*>(pControl)->SetCheck(BST_CHECKED);
		}
		else
		{
			static_cast<CButton*>(pControl)->SetCheck(BST_UNCHECKED);
		}

		//cull mode
		pControl = GetDlgItem(IDC_CHECK_2FACE);
		pControl->EnableWindow(TRUE);
		if (dwOption & MATERIAL_OPTION_CULL_CW)//by dengzhihui Refactor 2006年11月9日
		{
			static_cast<CButton*>(pControl)->SetCheck(FALSE);
		}

		if (dwOption & MATERIAL_OPTION_CULL_NONE)//by dengzhihui Refactor 2006年11月9日
		{
			static_cast<CButton*>(pControl)->SetCheck(TRUE);
		}

		m_ButtonNoCell.EnableWindow(TRUE);
		if (dwOption & MATERIAL_OPTION_NOLOGICALCELL)
		{
			m_ButtonNoCell.SetCheck(FALSE);
		}
		else
		{
			m_ButtonNoCell.SetCheck(TRUE);
		}

        m_CheckSpec.EnableWindow(TRUE);
        if (dwOption & MATERIAL_OPTION_SPEC)
        {
            m_CheckSpec.SetCheck(TRUE);
        }
        else
        {
            m_CheckSpec.SetCheck(FALSE);
        }

        m_CheckEMap.EnableWindow(TRUE);
        if (dwOption & MATERIAL_OPTION_EMAP)
        {
            m_CheckEMap.SetCheck(TRUE);
        }
        else
        {
            m_CheckEMap.SetCheck(FALSE);
        }



		m_ColorButtonDiffuse.EnableWindow(TRUE);
		m_ColorButtonSpecular.EnableWindow(TRUE);
		m_ColorButtonEmissive.EnableWindow(TRUE);
		m_ColorButtonAmbient.EnableWindow(TRUE);

		//(by dengzhihui Refactor 2006年11月9日
		D3DMATERIAL9 *st9;

        hr = m_lpCurModel->GetMaterialSubSetMaterial9(nCurSel, &st9);
        KG_COM_PROCESS_ERROR(hr);

        UpdateButtonState(ENUM_BUTTON_AMBIENT, ValueToColor(st9->Ambient));
		UpdateButtonState(ENUM_BUTTON_SPECULAR, ValueToColor(st9->Specular));
		UpdateButtonState(ENUM_BUTTON_EMISSIVE, ValueToColor(st9->Emissive));
		UpdateButtonState(ENUM_BUTTON_DIFFUSE, ValueToColor(st9->Diffuse));
		//)

		m_ColorButtonDiffuse.Invalidate();
		m_ColorButtonSpecular.Invalidate();
		m_ColorButtonEmissive.Invalidate();
		m_ColorButtonAmbient.Invalidate();


		//specular shinness
		pControl = GetDlgItem(IDC_EDIT_POWER);
		pControl->EnableWindow(TRUE);
		m_fPower = st9->Power;//by dengzhihui Refactor 2006年11月9日

		//Alpha value
		pControl = GetDlgItem(IDC_EDIT_ALPHA);
		if(pControl)
			pControl->EnableWindow(TRUE);

		//Alpha blend
		pControl = GetDlgItem(IDC_CHECK_ALPHABLEND);

        IEKG3DMaterial* piMaterial = NULL;
        piMaterial = GetMaterialFromModel();
        KG_PROCESS_ERROR(piMaterial);
        piMaterial->GetSubMaterialItemAlphaOp(nCurSel, &pAlphaOp);

		m_ctlBlendMode.EnableWindow(TRUE);

		FillListBoxTexture(nCurSel);

		sprintf(strInfo, "当前有效材质%d", nCurSel);
		m_nBaseMaterial = nCurSel;
		GetDlgItem(IDC_STATIC_CURSEL)->SetWindowText(strInfo);
	}

	{

		if (m_lpMaterialAlphaDialog)
		{
			BOOL bShow = m_lpMaterialAlphaDialog->IsWindowVisible();
			ResetAlphaWindow(bShow);
		}
	}



    if (m_nBaseMaterial != 0xffffffff)
    {
        m_ColorCastBn.SetColorPickerDialogCaption(_T("偏色色(5云起的名字 - v-)"));
        m_ColorCastBn.BindColor(m_lpCurModel->GetMaterialSubsetColorCast(m_nBaseMaterial,0));
		m_ColorCastBn1.SetColorPickerDialogCaption(_T("偏色色1(5云起的名字 - v-)"));
		m_ColorCastBn1.BindColor(m_lpCurModel->GetMaterialSubsetColorCast(m_nBaseMaterial,1));
		m_ColorCastBn2.SetColorPickerDialogCaption(_T("偏色色2(5云起的名字 - v-)"));
		m_ColorCastBn2.BindColor(m_lpCurModel->GetMaterialSubsetColorCast(m_nBaseMaterial,2));
		m_ColorCastBn3.SetColorPickerDialogCaption(_T("偏色色3(5云起的名字 - v-)"));
		m_ColorCastBn3.BindColor(m_lpCurModel->GetMaterialSubsetColorCast(m_nBaseMaterial,3));
		m_ColorCastBn4.SetColorPickerDialogCaption(_T("偏色色4(5云起的名字 - v-)"));
		m_ColorCastBn4.BindColor(m_lpCurModel->GetMaterialSubsetColorCast(m_nBaseMaterial,4));
		m_ColorCastBn5.SetColorPickerDialogCaption(_T("偏色色5(5云起的名字 - v-)"));
		m_ColorCastBn5.BindColor(m_lpCurModel->GetMaterialSubsetColorCast(m_nBaseMaterial,5));
		m_ColorCastBn6.SetColorPickerDialogCaption(_T("偏色色6(5云起的名字 - v-)"));
		m_ColorCastBn6.BindColor(m_lpCurModel->GetMaterialSubsetColorCast(m_nBaseMaterial,6));
		m_ColorCastBn7.SetColorPickerDialogCaption(_T("偏色色7(5云起的名字 - v-)"));
		m_ColorCastBn7.BindColor(m_lpCurModel->GetMaterialSubsetColorCast(m_nBaseMaterial,7));

        m_fSpecPower = *(m_lpCurModel->GetMaterialSubsetSpecPower(m_nBaseMaterial));
        m_fEmssPower = *(m_lpCurModel->GetMaterialSubsetEmssPower(m_nBaseMaterial));
    }


	UpdateData(FALSE);
Exit0:
	return;
}

void KSceneModelEditorPropertyPageMaterial::OnButtonSelectall()
{
	HRESULT hr = E_FAIL;
    int nCount = 0;

	KG_PROCESS_ERROR(m_lpCurModel);

    hr = m_lpCurModel->GetNumMaterial(&nCount);
    KG_COM_PROCESS_ERROR(hr);

	for(int i = 0;i < nCount; ++i)
	{
		m_ctlSubsetList.SetSel(i, TRUE);
	}

	OnSelchangeListSubset();
Exit0:
	return;
}

void KSceneModelEditorPropertyPageMaterial::OnButtonSelectinvert()
{
	HRESULT hr = E_FAIL;
	int nSel = 0;
    int nCount = 0;

	KG_PROCESS_ERROR(m_lpCurModel);

    hr = m_lpCurModel->GetNumMaterial(&nCount);
    KG_COM_PROCESS_ERROR(hr);

	for(int i = 0; i < nCount; ++i)
	{
		nSel = m_ctlSubsetList.GetSel(i);
		m_ctlSubsetList.SetSel(i,!nSel);
	}

	OnSelchangeListSubset();
Exit0:
	return;
}

void KSceneModelEditorPropertyPageMaterial::OnButtonSelectnone()
{
	HRESULT hr = E_FAIL;
    int nCount = 0;

    KG_PROCESS_ERROR(m_lpCurModel);

    hr = m_lpCurModel->GetNumMaterial(&nCount);
    KG_COM_PROCESS_ERROR(hr);

	for(int i = 0; i < nCount; ++i)
		m_ctlSubsetList.SetSel(i, FALSE);

	m_nBaseMaterial = -1;
	OnSelchangeListSubset();
Exit0:
	return;
}

void KSceneModelEditorPropertyPageMaterial::OnCheckZbuffer()
{
    HRESULT hr = E_FAIL;
    int nCount = 0;
    DWORD dwOption = 0;

	if (m_ctlAutoChange.GetCheck() == BST_CHECKED)
	{
        hr = m_lpCurModel->GetNumMaterial(&nCount);
        KG_COM_PROCESS_ERROR(hr);

		for(DWORD i = 0; static_cast<int>(i) < nCount; i ++)
		{
			if(m_ctlSubsetList.GetSel(i))
			{
                hr = m_lpCurModel->GetMaterialSubsetOption(i, &dwOption);
                KG_COM_PROCESS_ERROR(hr);

                if(m_ButtonZbuffer.GetCheck() == BST_CHECKED)
				{
					dwOption |= MATERIAL_OPTION_ZBUFFER_TRUE;
					dwOption &= (~MATERIAL_OPTION_ZBUFFER_FALSE);
				}
				else
				{
					dwOption |= MATERIAL_OPTION_ZBUFFER_FALSE;
					dwOption &= (~MATERIAL_OPTION_ZBUFFER_TRUE);

					GetDlgItem(IDC_COMBO_ZFUN)->EnableWindow(FALSE);
				}
                hr = m_lpCurModel->SetMaterialSubsetOption(i, dwOption);
                KG_COM_PROCESS_ERROR(hr);
			}
		}
	}
	else
	{
        hr = m_lpCurModel->GetMaterialSubsetOption(m_nBaseMaterial, &dwOption);
        KG_COM_PROCESS_ERROR(hr);

		if(m_ButtonZbuffer.GetCheck() == BST_CHECKED)
		{
			dwOption |= MATERIAL_OPTION_ZBUFFER_TRUE;
			dwOption &= (~MATERIAL_OPTION_ZBUFFER_FALSE);
		}
		else
		{
			dwOption |= MATERIAL_OPTION_ZBUFFER_FALSE;
			dwOption &= (~MATERIAL_OPTION_ZBUFFER_TRUE);

			GetDlgItem(IDC_COMBO_ZFUN)->EnableWindow(FALSE);
		}
        hr = m_lpCurModel->SetMaterialSubsetOption(m_nBaseMaterial, dwOption);
        KG_COM_PROCESS_ERROR(hr);
	}

	if (m_ButtonZbuffer.GetCheck() == BST_CHECKED)
	{
		CComboBox* pZFunCombo = static_cast<CComboBox*>(GetDlgItem(IDC_COMBO_ZFUN));
		pZFunCombo->EnableWindow(TRUE);
		int nZfun = 0;
        hr = m_lpCurModel->GetMaterialSubsetMtlOptionData(m_nBaseMaterial, OPTION_MATERIAL_ZFUN, 0xffffffff, &nZfun);
        KG_COM_PROCESS_ERROR(hr);

		pZFunCombo->SetCurSel(nZfun);
	}
	else
	{
		GetDlgItem(IDC_COMBO_ZFUN)->EnableWindow(FALSE);
	}

Exit0:
	return;
}

void KSceneModelEditorPropertyPageMaterial::OnCheckSpecular()
{
	HRESULT hr = E_FAIL;
	int nCount = 0;
    DWORD dwOption = 0;

	KG_PROCESS_ERROR(m_lpCurModel);

	if (m_ctlAutoChange.GetCheck() == BST_CHECKED)
	{
        hr = m_lpCurModel->GetNumMaterial(&nCount);
        KG_COM_PROCESS_ERROR(hr);

		for(int i = 0; i < nCount; ++i)//i < pMat->m_dwNumMaterials; i++)
		{
			if(m_ctlSubsetList.GetSel(i))
			{
                hr = m_lpCurModel->GetMaterialSubsetOption(i, &dwOption);
                KG_COM_PROCESS_ERROR(hr);

                m_ButtonSpecular.GetCheck() ?
					dwOption |= MATERIAL_OPTION_SPECULARENABLE :
					dwOption &= (~MATERIAL_OPTION_SPECULARENABLE);
				hr = m_lpCurModel->SetMaterialSubsetOption(i, dwOption);
                KGLOG_COM_PROCESS_ERROR(hr);
			}
		}
	}
	else
	{
		KG_PROCESS_ERROR(m_nBaseMaterial != -1);
        hr = m_lpCurModel->GetMaterialSubsetOption(m_nBaseMaterial, &dwOption);
        KG_COM_PROCESS_ERROR(hr);

		m_ButtonSpecular.GetCheck() ?
			dwOption |= MATERIAL_OPTION_SPECULARENABLE :
			dwOption &= (~MATERIAL_OPTION_SPECULARENABLE);
        hr = m_lpCurModel->SetMaterialSubsetOption(m_nBaseMaterial, dwOption);
        KGLOG_COM_PROCESS_ERROR(hr);
	}
Exit0:
	return;
}

void KSceneModelEditorPropertyPageMaterial::OnCheckLighting()
{
    HRESULT hr = E_FAIL;
    int nCount = 0;
    DWORD dwOption = 0;

    KG_PROCESS_ERROR(m_lpCurModel);

	if (m_ctlAutoChange.GetCheck() == BST_CHECKED)
	{
        hr = m_lpCurModel->GetNumMaterial(&nCount);
        KGLOG_COM_PROCESS_ERROR(hr);

		for(int i = 0; i < nCount; i++)
		{
			if(m_ctlSubsetList.GetSel(i))
			{
                hr = m_lpCurModel->GetMaterialSubsetOption(i, &dwOption);
                KG_COM_PROCESS_ERROR(hr);

				if(m_ButtonLighting.GetCheck())
				{
					dwOption |= MATERIAL_OPTION_LIGHTING;
				}
				else
				{
					dwOption &= (~MATERIAL_OPTION_LIGHTING);
				}
                hr = m_lpCurModel->SetMaterialSubsetOption(i, dwOption);
                KGLOG_COM_PROCESS_ERROR(hr);
			}
		}
	}
	else
	{
        hr = m_lpCurModel->GetMaterialSubsetOption(m_nBaseMaterial, &dwOption);
        KG_COM_PROCESS_ERROR(hr);

		if(m_ButtonLighting.GetCheck())
		{
			dwOption |= MATERIAL_OPTION_LIGHTING;
		}
		else
		{
			dwOption &= (~MATERIAL_OPTION_LIGHTING);
		}
        hr = m_lpCurModel->GetMaterialSubsetOption(m_nBaseMaterial, &dwOption);
        KG_COM_PROCESS_ERROR(hr);
	}

Exit0:
	return;
}

void KSceneModelEditorPropertyPageMaterial::OnCheck2face()
{
    HRESULT hr = E_FAIL;
    int nCount = 0;
    DWORD dwOption = 0;

	KG_PROCESS_ERROR(m_lpCurModel);

	if (m_ctlAutoChange.GetCheck() == BST_CHECKED)
	{
        hr = m_lpCurModel->GetNumMaterial(&nCount);
        KGLOG_COM_PROCESS_ERROR(hr);

		for(int i = 0; i < nCount; i ++)
		{
			if(m_ctlSubsetList.GetSel(i))
			{
                hr = m_lpCurModel->GetMaterialSubsetOption(i, &dwOption);
                KG_COM_PROCESS_ERROR(hr);

                if(m_Button2Face.GetCheck())
				{
					dwOption |= MATERIAL_OPTION_CULL_NONE;
					dwOption &= (~MATERIAL_OPTION_CULL_CW);
				}
				else
				{
					dwOption |= MATERIAL_OPTION_CULL_CW;
					dwOption &= (~MATERIAL_OPTION_CULL_NONE);
				}

                hr = m_lpCurModel->SetMaterialSubsetOption(i, dwOption);
                KGLOG_COM_PROCESS_ERROR(hr);
			}
		}
	}
	else
	{
        hr = m_lpCurModel->GetMaterialSubsetOption(m_nBaseMaterial, &dwOption);
        KG_COM_PROCESS_ERROR(hr);

		if(m_Button2Face.GetCheck())
		{
			dwOption |= MATERIAL_OPTION_CULL_NONE;
			dwOption &= (~MATERIAL_OPTION_CULL_CW);
		}
		else
		{
			dwOption |= MATERIAL_OPTION_CULL_CW;
			dwOption &= (~MATERIAL_OPTION_CULL_NONE);
		}
        hr = m_lpCurModel->SetMaterialSubsetOption(m_nBaseMaterial, dwOption);
        KGLOG_COM_PROCESS_ERROR(hr);
	}
Exit0:
	return;
}

void KSceneModelEditorPropertyPageMaterial::OnButtonTextureadd()
{
	// TODO: Add your control notification handler code here
	HRESULT hr = E_FAIL;
	char bigBuff[800] = "";  // maximum common dialog buffer size
	char szFilter[] ="Texture Files|*.swf;*.bmp;*.jpg;*.png;*.tga;*.Spr;*.DDS;*.avi;*.rmvb;*.wmv|";
	int nStage = 0;
	CString strPathName;
    IEKG3DTexture* pTexture = NULL;
	IEKG3DTexture *pNewTexture = NULL;
    IEKG3DTextureTable* piTextureTable = NULL;
    IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
    LPCTSTR pszName = NULL;
	CFileDialog dlg(true, NULL, NULL, OFN_HIDEREADONLY , szFilter);
    DWORD dwID = 0;
    TCHAR szDir[MAX_PATH];
    ::GetCurrentDirectory(MAX_PATH, szDir);

	KG_PROCESS_ERROR(m_ctlSubsetList.GetSelCount());
	KG_PROCESS_ERROR(m_lpCurModel);

	nStage = m_ctlTextureList.GetCurSel();
	KG_PROCESS_ERROR(nStage != LB_ERR);
	KG_PROCESS_ERROR(nStage < s_dwMaxNumTextureStage);


	dlg.m_ofn.lpstrInitialDir = m_strFullPath[nStage];
	KG_PROCESS_ERROR(dlg.DoModal() == IDOK);

    int nCount = 0;
    hr = m_lpCurModel->GetNumMaterial(&nCount);
    KGLOG_COM_PROCESS_ERROR(hr);
	int nLastSel = -1;
	for(int i = 0; i < nCount; ++i)
	{
		if(m_ctlSubsetList.GetSel(i))
		{
			m_lpCurModel->ChangeTexture(i, nStage, dlg.GetPathName());
			nLastSel = i;
            if (i == 0 && nLastSel == 0)
            {
                IEKG3DMaterial* pMaterial = NULL;
                pMaterial = GetMaterialFromModel();
                if (pMaterial)
                    pMaterial->SetOrgTextureFileName(dlg.GetPathName());
            }
		}
	}
	if (nLastSel != -1)
	{
		FillListBoxTexture(nLastSel);
	}

    ::SetCurrentDirectory(szDir);
Exit0:
	return;
}

void KSceneModelEditorPropertyPageMaterial::OnButtonTextureremove()
{
	HRESULT hr = E_FAIL;
	IEKG3DTexture* pTexture = NULL;
    IEKG3DTextureTable* piTextureTable = NULL;
	int nStage = 0;
	int nCount = 0;
    IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
    DWORD dwID = 0;

	KG_PROCESS_ERROR(m_ctlSubsetList.GetSelCount());

	KG_PROCESS_ERROR(m_lpCurModel);

	nStage = m_ctlTextureList.GetCurSel();

	KG_PROCESS_ERROR(nStage >= 0);
	KG_PROCESS_ERROR(nStage < s_dwMaxNumTextureStage);

    hr = m_lpCurModel->GetNumMaterial(&nCount);
	KG_COM_PROCESS_ERROR(hr);

    for(DWORD i = 0; static_cast<int>(i) < nCount; ++i)
	{
		if(m_ctlSubsetList.GetSel(i))
		{
            m_lpCurModel->RemoveTexture(i, nStage);
            if (i == 0 && nStage == 0)
            {
                IEKG3DMaterial* pMaterial = NULL;
                pMaterial = GetMaterialFromModel();
                if (pMaterial)
                    pMaterial->SetOrgTextureFileName("");
            }
			FillListBoxTexture(i);
		}
	}

Exit0:
	return;
}

void KSceneModelEditorPropertyPageMaterial::OnDblclkListTextures()
{
	int nCurTexture = m_ctlTextureList.GetCurSel();
	HGLOBAL hData = NULL;
	void *pDest = NULL;
	TCHAR strFullPath[MAX_PATH];
	BOOL bClose = FALSE;
	KG_PROCESS_ERROR(nCurTexture != LB_ERR);
	KG_PROCESS_ERROR(nCurTexture < s_dwMaxNumTextureStage);

	if (m_strFullPath[nCurTexture][0] != '\0')
	{
		sprintf_s(strFullPath, 
			MAX_PATH,
			"%s%s",
			g_szDefExeDirectory,
			m_strFullPath[nCurTexture]);
		KG_PROCESS_ERROR(::OpenClipboard(this->GetSafeHwnd()));
		
		bClose = TRUE;
		::EmptyClipboard();
		size_t uSize = strlen(strFullPath) + 1; 
		hData = ::GlobalAlloc(GMEM_MOVEABLE, uSize * sizeof(TCHAR));	//应该是不用释放的!?
		KG_PROCESS_ERROR(hData);

		pDest = (LPWSTR)::GlobalLock(hData);
		KG_PROCESS_ERROR(pDest);

		memcpy(pDest, strFullPath, sizeof(TCHAR) * uSize);

		::GlobalUnlock(hData);
		::SetClipboardData(CF_TEXT, hData);
		MessageBox(strFullPath, "贴图信息");
	}
Exit0:
	if (bClose)
	{
		::CloseClipboard();
	}
	return;
}

void KSceneModelEditorPropertyPageMaterial::OnSelchangeListTextures()
{
	int nCurSel = m_ctlTextureList.GetCurSel();
	if (nCurSel == LB_ERR)
	{
		//Disable all Texture related button
		m_ButtonTextureTranslation.EnableWindow(FALSE);
		m_ButtonTextureRemove.EnableWindow(FALSE);
		m_ButtonTextureOperation.EnableWindow(FALSE);
		m_ButtonTextureMap.EnableWindow(FALSE);
		m_ButtonTextureApplySet.EnableWindow(FALSE);
		m_ButtonTextureAdd.EnableWindow(FALSE);
	}
	else
	{
		m_ButtonTextureTranslation.EnableWindow(FALSE);
		m_ButtonTextureRemove.EnableWindow(TRUE);
		m_ButtonTextureOperation.EnableWindow(TRUE);
		m_ButtonTextureMap.EnableWindow(TRUE);
		m_ButtonTextureApplySet.EnableWindow(FALSE);
		m_ButtonTextureAdd.EnableWindow(TRUE);

		if (m_strFullPath[nCurSel][0] != '\0')
		{
			m_ButtonTextureAdd.SetWindowText("替换贴图");
		}
		else
		{
			m_ButtonTextureAdd.SetWindowText("添加贴图");
		}

	}
}

void KSceneModelEditorPropertyPageMaterial::OnBnClickedButtonSave()
{
	HRESULT hr = E_FAIL;
	TCHAR bigBuff[800] = "";  // maximum common dialog buffer size
	TCHAR szFilter[] ="Material Files (*.Mtl)|*.Mtl|";
	IEKG3DMaterial* pMat = NULL;
	CString strPathName;
	CFileDialog dlg(FALSE, "Mtl", NULL,
		OFN_HIDEREADONLY , szFilter);

	KG_PROCESS_ERROR(m_lpCurModel);

	pMat = GetMaterialFromModel();
	KG_PROCESS_ERROR(pMat);

	dlg.m_ofn.lpstrFile = bigBuff;
	dlg.m_ofn.nMaxFile = sizeof(bigBuff);
	KG_PROCESS_ERROR(dlg.DoModal() == IDOK);

	strPathName = dlg.GetPathName();
	if (!CheckFileReadOnly(strPathName))
	{
		SetDefaultPath();
		hr = pMat->SaveToFile(strPathName, 0);
		RestorePath();
	}
Exit0:
	return;
}

void KSceneModelEditorPropertyPageMaterial::OnBnClickedButtonSaveid()
{
	HRESULT hResult = E_FAIL;
	TCHAR strName[MAX_PATH];
	TCHAR strFullPath[MAX_PATH];
	strName[0] = 0;

	KG_PROCESS_ERROR(m_lpCurModel);
	hResult = m_lpCurModel->GetMaterialName(strName, sizeof(strName));
	KG_COM_PROCESS_ERROR(hResult);

	if (strName[0] == '\\')
	{
		sprintf_s(strFullPath,
			MAX_PATH,
			"%s%s",
			g_szDefWorkDirectory,
			strName);
	}
	else
	{
		strcpy_s(strFullPath, MAX_PATH, strName);
	}
	
	if (!CheckFileReadOnly(strFullPath))
	{
		SetDefaultPath();
		hResult = m_lpCurModel->SaveMaterialToFile(strFullPath, 0);
		RestorePath();
		KG_COM_PROCESS_ERROR(hResult);
	}
	
Exit0:
	if (FAILED(hResult))
	{
		MessageBox("Save failed.", "Error");
	}
	return;
}

void KSceneModelEditorPropertyPageMaterial::OnBnClickedCheckZwrite()
{
    HRESULT hr = E_FAIL;
    int nCount = 0;
    DWORD dwOption = 0;

	KGLOG_PROCESS_ERROR(m_lpCurModel);

    if (m_ctlAutoChange.GetCheck() == BST_CHECKED)
	{
        hr = m_lpCurModel->GetNumMaterial(&nCount);
        KG_COM_PROCESS_ERROR(hr);

		for(int i = 0; i < nCount; ++i)
		{
			if(!m_ctlSubsetList.GetSel(i))
				continue;

			hr = m_lpCurModel->GetMaterialSubsetOption(i, &dwOption);
            KG_COM_PROCESS_ERROR(hr);

            if(m_ButtonZWrite.GetCheck() == BST_CHECKED)
			{
				dwOption &= (~MATERIAL_OPTION_ZWRITEDISABLE);
			}
			else
			{
				dwOption |= MATERIAL_OPTION_ZWRITEDISABLE;
			}

			hr = m_lpCurModel->SetMaterialSubsetOption(i, dwOption);
            KG_COM_PROCESS_ERROR(hr);
		}
	}
	else
	{
		hr = m_lpCurModel->GetMaterialSubsetOption(m_nBaseMaterial, &dwOption);
        KG_COM_PROCESS_ERROR(hr);

        if(m_ButtonZWrite.GetCheck() == BST_CHECKED)
		{
			dwOption &= (~MATERIAL_OPTION_ZWRITEDISABLE);
		}
		else
		{
			dwOption |= MATERIAL_OPTION_ZWRITEDISABLE;
		}
        hr = m_lpCurModel->SetMaterialSubsetOption(m_nBaseMaterial, dwOption);
        KG_COM_PROCESS_ERROR(hr);
	}

Exit0:
	return;
}

void KSceneModelEditorPropertyPageMaterial::OnBnClickedCheckHide()
{
    HRESULT hr = E_FAIL;
    int nCount = 0;
    DWORD dwOption = 0;

	KG_PROCESS_ERROR(m_lpCurModel);

	if (m_ctlAutoChange.GetCheck() == BST_CHECKED)
	{
        hr = m_lpCurModel->GetNumMaterial(&nCount);
        KG_COM_PROCESS_ERROR(hr);

		for(DWORD i = 0; static_cast<int>(i) < nCount; ++i)
		{
			if(!m_ctlSubsetList.GetSel(i))
				continue;

            hr = m_lpCurModel->GetMaterialSubsetOption(i, &dwOption);
            KG_COM_PROCESS_ERROR(hr);

			if(m_ButtonHide.GetCheck())
			{
				dwOption |= MATERIAL_OPTION_HIDE;
			}
			else
			{
				dwOption &= (~MATERIAL_OPTION_HIDE);
			}
            hr = m_lpCurModel->SetMaterialSubsetOption(i, dwOption);
            KG_COM_PROCESS_ERROR(hr);
		}
	}
	else
	{
        hr = m_lpCurModel->GetMaterialSubsetOption(m_nBaseMaterial, &dwOption);
        KG_COM_PROCESS_ERROR(hr);

		if(m_ButtonHide.GetCheck())
		{
			dwOption |= MATERIAL_OPTION_HIDE;
		}
		else
		{
			dwOption &= (~MATERIAL_OPTION_HIDE);
		}
        hr = m_lpCurModel->SetMaterialSubsetOption(m_nBaseMaterial, dwOption);
        KG_COM_PROCESS_ERROR(hr);
	}

Exit0:
	return;
}

void KSceneModelEditorPropertyPageMaterial::OnBnClickedButtonAmbient()
{
	HRESULT hr = E_FAIL;
	D3DMATERIAL9* st9;

    KG_PROCESS_ERROR(m_lpCurModel);

    hr = m_lpCurModel->GetMaterialSubSetMaterial9(m_nBaseMaterial, &st9);
    KG_COM_PROCESS_ERROR(hr);

	GetColorPickerDialog()->Init(&st9->Ambient, "材质环境光", this);
Exit0:
	return;
}

void KSceneModelEditorPropertyPageMaterial::OnBnClickedButtonDiffuse()
{
	// TODO: Add your control notification handler code here
	HRESULT hr = E_FAIL;
	D3DMATERIAL9* st9;

	KG_PROCESS_ERROR(m_lpCurModel);

    hr = m_lpCurModel->GetMaterialSubSetMaterial9(m_nBaseMaterial, &st9);
    KG_COM_PROCESS_ERROR(hr);

	GetColorPickerDialog()->Init(&st9->Diffuse, "材质漫射光", this);
Exit0:
	return;
}

void KSceneModelEditorPropertyPageMaterial::OnBnClickedButtonSpecular()
{
	HRESULT hr = E_FAIL;
    D3DMATERIAL9* st9;

	KG_PROCESS_ERROR(m_lpCurModel);

    hr = m_lpCurModel->GetMaterialSubSetMaterial9(m_nBaseMaterial, &st9);
    KG_COM_PROCESS_ERROR(hr);

	GetColorPickerDialog()->Init(&st9->Specular, "材质高光", this);
Exit0:
	return;
}

void KSceneModelEditorPropertyPageMaterial::OnBnClickedButtonEmissive()
{
	HRESULT hr = E_FAIL;
    D3DMATERIAL9* st9;

	KG_PROCESS_ERROR(m_lpCurModel);

    hr = m_lpCurModel->GetMaterialSubSetMaterial9(m_nBaseMaterial, &st9);
    KG_COM_PROCESS_ERROR(hr);

	GetColorPickerDialog()->Init(&st9->Emissive, "材质自发光", this);
Exit0:
	return;
}


BOOL KSceneModelEditorPropertyPageMaterial::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->hwnd == m_EditPower.GetSafeHwnd()
		|| pMsg->hwnd == m_EditDiffuseAlpha.GetSafeHwnd()
        || pMsg->hwnd == m_EditEmssPower.GetSafeHwnd()
        || pMsg->hwnd == m_EditSpecPower.GetSafeHwnd())
	{
		if(pMsg->message == WM_KEYDOWN)
		{
			if(pMsg->wParam == VK_RETURN)
			{
				OnEditKeyReturn(pMsg->hwnd);
			}
		}
	}
	return FALSE;
}

void KSceneModelEditorPropertyPageMaterial::OnEditKeyReturn(HWND hWnd)
{
    HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(m_lpCurModel);

	UpdateData();

	D3DMATERIAL9* pSt = NULL;
	if (m_ctlAutoChange.GetCheck() == BST_CHECKED)
	{
		for(int i = 0; i < m_ctlSubsetList.GetCount(); i++)
		{
			if(!m_ctlSubsetList.GetSel(i))//If subset is selected, apply the change
				continue;

            hr = m_lpCurModel->GetMaterialSubSetMaterial9(i, &pSt);
            KG_COM_PROCESS_ERROR(hr);

			pSt->Power = static_cast<float>(m_fPower);
			pSt->Ambient.a = m_fAlpha;
			pSt->Diffuse.a = m_fAlpha;
			pSt->Emissive.a = m_fAlpha;
			pSt->Specular.a = m_fAlpha;

            float* pSecePower = m_lpCurModel->GetMaterialSubsetSpecPower(i);
            if (pSecePower)
                *pSecePower = m_fSpecPower;

            float* pEmssPower = m_lpCurModel->GetMaterialSubsetEmssPower(i);
            if (pEmssPower)
                *pEmssPower = m_fEmssPower;
		}
	}
	else
	{
        hr = m_lpCurModel->GetMaterialSubSetMaterial9(m_nBaseMaterial, &pSt);
        KG_COM_PROCESS_ERROR(hr);
		pSt->Power = static_cast<float>(m_fPower);
		pSt->Ambient.a = m_fAlpha;
		pSt->Diffuse.a = m_fAlpha;
		pSt->Emissive.a = m_fAlpha;
		pSt->Specular.a = m_fAlpha;
        
        float* pSecePower = m_lpCurModel->GetMaterialSubsetSpecPower(m_nBaseMaterial);
        if (pSecePower)
            *pSecePower = m_fSpecPower;

        float* pEmssPower = m_lpCurModel->GetMaterialSubsetEmssPower(m_nBaseMaterial);
        if (pEmssPower)
            *pEmssPower = m_fEmssPower;

	}
Exit0:
	return;
}

void KSceneModelEditorPropertyPageMaterial::OnBnClickedButtonTexturemap()
{
	int nCount = m_ctlTextureList.GetCount();
	for(int i = 0; i < nCount; i++)
	{
		IEKG3DTexture* pTexture = reinterpret_cast<IEKG3DTexture*>(m_ctlTextureList.GetItemData(i));
		if(pTexture)
			pTexture->CheckLoadState();
	}
}
void KSceneModelEditorPropertyPageMaterial::OnBnClickedButtonTextureop()
{
	int nCurSel = m_ctlSubsetList.GetCurSel();
	IEKG3DMaterial* pMaterial = NULL;
	DWORD pdwTextureID[8];
    KG_PROCESS_ERROR(nCurSel != LB_ERR);
	KG_PROCESS_ERROR(m_lpCurModel);

	pMaterial = static_cast<IEKG3DMaterial *>(GetMaterialFromModel());
	KG_PROCESS_ERROR(pMaterial);

	KDlg_TextureStageState& Dlg = g_GetTextureStageStateDlg();

    pMaterial->GetSubMaterialTextureID(nCurSel, pdwTextureID);

	Dlg.Init(pdwTextureID, this);

Exit0:
	return;
}
IEKG3DMaterial* KSceneModelEditorPropertyPageMaterial::GetMaterialFromModel()
{
	IEKG3DMaterial* pReturn = NULL;
	IEKG3DModelSpeedTree* pSpdTree = NULL;
    IEKG3DModel *pModel = NULL;
    DWORD dwType = 0;

	KG_PROCESS_ERROR(m_lpCurModel);

	m_lpCurModel->GetType(&dwType);

    if (dwType== MESHTYPE_SPEEDTREE)
	{
		m_lpCurModel->GetIEKG3DModelSpeedTree(&pSpdTree);
		pSpdTree->GetModelTree(&pModel);
        KG_PROCESS_ERROR(pModel);
        pModel->GetIEKG3DMaterial(&pReturn);

    }
	else
    {
        m_lpCurModel->GetIEKG3DMaterial(&pReturn);
	}
Exit0:
	return pReturn;
}

void KSceneModelEditorPropertyPageMaterial::InitButtonMaps(CButton* pButton, DWORD dwID, COLORREF crColor)
{
	RECT rect;
	CDC* pDc =NULL;
	int nHeight = 0;
	int nWidth = 0;
	BOOL bReturn = FALSE;
	KG_PROCESS_ERROR(pButton);
	KG_PROCESS_ERROR(dwID < ENUM_BUTTON_COUNT);
	pDc = GetDC();
	pButton->GetClientRect(&rect);
	nWidth = abs(rect.right - rect.left);
	nHeight = abs(rect.bottom - rect.top);

	KG_PROCESS_ERROR(nWidth);
	KG_PROCESS_ERROR(nHeight);

	bReturn = m_ButtonMaps[dwID].Bitmap.CreateCompatibleBitmap(pDc, nWidth, nHeight);
	KG_PROCESS_ERROR(bReturn);

	SAFE_DELETE_ARRAY(m_ButtonMaps[dwID].pBuffer);
	m_ButtonMaps[dwID].pBuffer = new COLORREF[nWidth * nHeight];
	KG_PROCESS_ERROR(m_ButtonMaps[dwID].pBuffer);
	m_ButtonMaps[dwID].BufferSize = sizeof(COLORREF) * nWidth * nHeight;

	SetBuffer(m_ButtonMaps[dwID].pBuffer, crColor, m_ButtonMaps[dwID].BufferSize >> 2);
	m_ButtonMaps[dwID].Bitmap.SetBitmapBits(static_cast<DWORD>(m_ButtonMaps[dwID].BufferSize),
		m_ButtonMaps[dwID].pBuffer);
	pButton->SetBitmap(m_ButtonMaps[dwID].Bitmap);

Exit0:
	if (pDc)
	{
		ReleaseDC(pDc);
	}
}

void KSceneModelEditorPropertyPageMaterial::UpdateButtonState(DWORD dwButtonID, COLORREF crColor)
{
	KG_PROCESS_ERROR(dwButtonID < ENUM_BUTTON_COUNT);
	KG_PROCESS_ERROR(m_ButtonMaps[dwButtonID].pBuffer);
	SetBuffer(m_ButtonMaps[dwButtonID].pBuffer, crColor, m_ButtonMaps[dwButtonID].BufferSize >> 2);
	m_ButtonMaps[dwButtonID].Bitmap.SetBitmapBits(static_cast<DWORD>(m_ButtonMaps[dwButtonID].BufferSize),
		m_ButtonMaps[dwButtonID].pBuffer);
Exit0:
	return;
}

LRESULT KSceneModelEditorPropertyPageMaterial::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    HRESULT hr = E_FAIL;

	if (message == WM_COLORPOINTER_CHANGE)
	{
		KG_PROCESS_ERROR(m_lpCurModel);

		D3DMATERIAL9* pst;

        hr = m_lpCurModel->GetMaterialSubSetMaterial9(m_nBaseMaterial, &pst);
        KG_COM_PROCESS_ERROR(hr);

        UpdateButtonState(ENUM_BUTTON_AMBIENT, ValueToColor(pst->Ambient));
		UpdateButtonState(ENUM_BUTTON_SPECULAR, ValueToColor(pst->Specular));
		UpdateButtonState(ENUM_BUTTON_EMISSIVE, ValueToColor(pst->Emissive));
		UpdateButtonState(ENUM_BUTTON_DIFFUSE, ValueToColor(pst->Diffuse));

		m_ColorButtonDiffuse.Invalidate();
		m_ColorButtonSpecular.Invalidate();
		m_ColorButtonEmissive.Invalidate();
		m_ColorButtonAmbient.Invalidate();

		if (m_ctlAutoChange.GetCheck() == BST_CHECKED)
		{
			for(int i = 0; i < m_ctlSubsetList.GetCount(); i++)
			{
				if (m_ctlSubsetList.GetSel(i) && i != m_nBaseMaterial)
				{
                    D3DMATERIAL9* pstDest;
                    hr = m_lpCurModel->GetMaterialSubSetMaterial9(i, &pstDest);
                    KG_COM_PROCESS_ERROR(hr);

					memcpy(pstDest, pst, sizeof(D3DMATERIAL9));
				}
			}
		}

		return 0;
	}

Exit0:
	return CWnd::WindowProc(message, wParam, lParam);
}

void KSceneModelEditorPropertyPageMaterial::OnCbnSelchangeComboZfun()
{
	CComboBox* pZFuncList = static_cast<CComboBox*>(GetDlgItem(IDC_COMBO_ZFUN));

	int nCurSel = 0;

	KG_PROCESS_ERROR(m_nBaseMaterial != -1);
	KG_PROCESS_ERROR(pZFuncList);
	KG_PROCESS_ERROR(m_lpCurModel);

	nCurSel = pZFuncList->GetCurSel();

	KG_PROCESS_ERROR(nCurSel != CB_ERR);

	if (m_ctlAutoChange.GetCheck() == BST_CHECKED)
	{
		for(int i = 0; i < m_ctlSubsetList.GetCount(); i++)
		{
			if (m_ctlSubsetList.GetSel(i))
			{
				m_lpCurModel->CopyMaterialOption(OPTION_MATERIAL_ZFUN, 0xffffffff, m_nBaseMaterial, i);
			}
		}
	}

Exit0:
	return;
}

void KSceneModelEditorPropertyPageMaterial::OnCbnSelchangeComboBlendmode()
{
	int nCurSel = 0;
	KG_PROCESS_ERROR(m_lpCurModel);

	nCurSel = m_ctlBlendMode.GetCurSel();
	KG_PROCESS_ERROR(nCurSel != CB_ERR);

	GetDlgItem(IDC_EDIT_TWICEALPHATOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_TWICEALPHABOTTOM)->EnableWindow(FALSE);
	switch(nCurSel)
	{
	case 0://0 Alpha混合
		{

			if (m_ctlAutoChange.GetCheck() == BST_CHECKED)
			{
				for(int i = 0; i < m_ctlSubsetList.GetCount(); i++)
				{
					if(!m_ctlSubsetList.GetSel(i))
						continue;

					m_lpCurModel->SetMaterialSubSetAlphaOption(i,D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA, TRUE);
					m_lpCurModel->SetMaterialSubsetRenderTwiceAlphaOption(i, FALSE, m_dwAlphaTop, m_dwAlphaBottom);
				}
			}
			else
			{
				m_lpCurModel->SetMaterialSubSetAlphaOption(m_nBaseMaterial,D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA, TRUE);
				m_lpCurModel->SetMaterialSubsetRenderTwiceAlphaOption(m_nBaseMaterial, FALSE, m_dwAlphaTop, m_dwAlphaBottom);
			}

			GetDlgItem(IDC_BUTTON_BLENDADV)->EnableWindow(FALSE);
			break;
		}
	case 1://1 Alpha叠加
		{
			if (m_ctlAutoChange.GetCheck() == BST_CHECKED)
			{
				for(int i = 0; i < m_ctlSubsetList.GetCount(); i++)
				{
					if(!m_ctlSubsetList.GetSel(i))
						continue;

					m_lpCurModel->SetMaterialSubSetAlphaOption(i,D3DBLEND_SRCALPHA, D3DBLEND_ONE, TRUE);
					m_lpCurModel->SetMaterialSubsetRenderTwiceAlphaOption(i, FALSE, m_dwAlphaTop, m_dwAlphaBottom);
				}
			}
			else
			{
				m_lpCurModel->SetMaterialSubSetAlphaOption(m_nBaseMaterial,D3DBLEND_SRCALPHA, D3DBLEND_ONE, TRUE);
				m_lpCurModel->SetMaterialSubsetRenderTwiceAlphaOption(m_nBaseMaterial, FALSE, m_dwAlphaTop, m_dwAlphaBottom);
			}
			GetDlgItem(IDC_BUTTON_BLENDADV)->EnableWindow(FALSE);
			break;
		}
	case 2://2 颜色混合
		{
			if (m_ctlAutoChange.GetCheck() == BST_CHECKED)
			{
				for(int i = 0; i < m_ctlSubsetList.GetCount(); i++)
				{
					if(!m_ctlSubsetList.GetSel(i))
						continue;

					m_lpCurModel->SetMaterialSubSetAlphaOption(i,D3DBLEND_SRCCOLOR, D3DBLEND_INVSRCCOLOR, TRUE);
					m_lpCurModel->SetMaterialSubsetRenderTwiceAlphaOption(i, FALSE, m_dwAlphaTop, m_dwAlphaBottom);
				}
			}
			else
			{
				m_lpCurModel->SetMaterialSubSetAlphaOption(m_nBaseMaterial,D3DBLEND_SRCCOLOR, D3DBLEND_INVSRCCOLOR, TRUE);
				m_lpCurModel->SetMaterialSubsetRenderTwiceAlphaOption(m_nBaseMaterial, FALSE, m_dwAlphaTop, m_dwAlphaBottom);
			}
			GetDlgItem(IDC_BUTTON_BLENDADV)->EnableWindow(FALSE);
			break;
		}
	case 3://3 颜色叠加
		{
			if (m_ctlAutoChange.GetCheck() == BST_CHECKED)
			{
				for(int i = 0; i < m_ctlSubsetList.GetCount(); i++)
				{
					if(!m_ctlSubsetList.GetSel(i))
						continue;

					m_lpCurModel->SetMaterialSubSetAlphaOption(i,D3DBLEND_SRCCOLOR, D3DBLEND_ONE, TRUE);
					m_lpCurModel->SetMaterialSubsetRenderTwiceAlphaOption(i, FALSE, m_dwAlphaTop, m_dwAlphaBottom);
				}
			}
			else
			{
				m_lpCurModel->SetMaterialSubSetAlphaOption(m_nBaseMaterial,D3DBLEND_SRCCOLOR, D3DBLEND_ONE, TRUE);
				m_lpCurModel->SetMaterialSubsetRenderTwiceAlphaOption(m_nBaseMaterial, FALSE, m_dwAlphaTop, m_dwAlphaBottom);
			}
			GetDlgItem(IDC_BUTTON_BLENDADV)->EnableWindow(FALSE);
			break;
		}
	case 4://RenderTwice
		{
			BOOL bDummy = FALSE;
			m_lpCurModel->GetMaterialSubsetRenderTwiceAlphaOption(m_nBaseMaterial, 
				bDummy, 
				m_dwAlphaTop,
				m_dwAlphaBottom);
			UpdateData(FALSE);
			if (m_ctlAutoChange.GetCheck() == BST_CHECKED)
			{
				for(int i = 0; i < m_ctlSubsetList.GetCount(); i++)
				{
					if(!m_ctlSubsetList.GetSel(i))
						continue;

					m_lpCurModel->SetMaterialSubsetRenderTwiceAlphaOption(i, TRUE, m_dwAlphaTop, m_dwAlphaBottom);

				}
			}
			else
			{
				m_lpCurModel->SetMaterialSubsetRenderTwiceAlphaOption(m_nBaseMaterial, TRUE, m_dwAlphaTop, m_dwAlphaBottom);
			}
			GetDlgItem(IDC_EDIT_TWICEALPHATOP)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_TWICEALPHABOTTOM)->EnableWindow(TRUE);
			break;
		}
	case 5://自定义
		{
			if (m_ctlAutoChange.GetCheck() == BST_CHECKED)
			{
				for(int i = 0; i < m_ctlSubsetList.GetCount(); i++)
				{
					if(!m_ctlSubsetList.GetSel(i))
						continue;

					m_lpCurModel->SetMaterialSubsetRenderTwiceAlphaOption(i, FALSE, m_dwAlphaTop, m_dwAlphaBottom);

				}
			}
			else
			{
				m_lpCurModel->SetMaterialSubsetRenderTwiceAlphaOption(m_nBaseMaterial, FALSE, m_dwAlphaTop, m_dwAlphaBottom);
			}
			GetDlgItem(IDC_BUTTON_BLENDADV)->EnableWindow(TRUE);
			break;
		}
	}
Exit0:
	return;

}

void KSceneModelEditorPropertyPageMaterial::OnBnClickedButtonBlendadv()
{
	ResetAlphaWindow(TRUE);
}

void KSceneModelEditorPropertyPageMaterial::ResetAlphaWindow(BOOL bShow)
{
	HRESULT hr = E_FAIL;

	IEKG3DMaterial* pMaterial = NULL;
	std::vector<int> vecSelSubMaterial;
	KG_PROCESS_ERROR(m_lpCurModel);

	pMaterial = GetMaterialFromModel();
	KG_PROCESS_ERROR(pMaterial);

	if (m_ctlAutoChange.GetCheck())
	{
		for (int i = 0; i < m_ctlSubsetList.GetCount(); i++)
		{
			if (m_ctlSubsetList.GetSel(i))
			{
				vecSelSubMaterial.push_back(i);
			}
		}
	}
	else
	{
		KG_PROCESS_ERROR(m_nBaseMaterial != -1);
		vecSelSubMaterial.push_back(m_nBaseMaterial);
	}

	if(!m_lpMaterialAlphaDialog)
	{
		m_lpMaterialAlphaDialog = new KSceneModelEditorMaterialAlphaOperatorDialog;
		m_lpMaterialAlphaDialog->Create(IDD_MODELEDITOR_MATERIAL_ALPHABLEND, this);
	}
	m_lpMaterialAlphaDialog->SetMaterial(pMaterial, vecSelSubMaterial);
	if (bShow)
	{
		m_lpMaterialAlphaDialog->ShowWindow(SW_SHOW);
	}

Exit0:
	return;
}

void KSceneModelEditorPropertyPageMaterial::OnBnClickedCheckNocell()
{
	HRESULT hr = E_FAIL;
	int nCount = 0;
	DWORD dwOption = 0;

	KG_PROCESS_ERROR(m_lpCurModel);

	if (m_ctlAutoChange.GetCheck() == BST_CHECKED)
	{
		hr = m_lpCurModel->GetNumMaterial(&nCount);
		KGLOG_COM_PROCESS_ERROR(hr);

		for(int i = 0; i < nCount; i ++)
		{
			if(m_ctlSubsetList.GetSel(i))
			{
				hr = m_lpCurModel->GetMaterialSubsetOption(i, &dwOption);
				KG_COM_PROCESS_ERROR(hr);

				if(!m_ButtonNoCell.GetCheck())
				{
					dwOption |= MATERIAL_OPTION_NOLOGICALCELL;
				}
				else
				{
					
					dwOption &= (~MATERIAL_OPTION_NOLOGICALCELL);
				}

				hr = m_lpCurModel->SetMaterialSubsetOption(i, dwOption);
				KGLOG_COM_PROCESS_ERROR(hr);
			}
		}
	}
	else
	{
		hr = m_lpCurModel->GetMaterialSubsetOption(m_nBaseMaterial, &dwOption);
		KG_COM_PROCESS_ERROR(hr);

		if(!m_ButtonNoCell.GetCheck())
		{
			dwOption |= MATERIAL_OPTION_NOLOGICALCELL;
		}
		else
		{
			dwOption &= (~MATERIAL_OPTION_NOLOGICALCELL);
		}
		hr = m_lpCurModel->SetMaterialSubsetOption(m_nBaseMaterial, dwOption);
		KGLOG_COM_PROCESS_ERROR(hr);
	}
Exit0:
	return;
}

void KSceneModelEditorPropertyPageMaterial::OnBnClickedCheckSpec()
{
    HRESULT hr = E_FAIL;
    int nCount = 0;
    DWORD dwOption = 0;

    KG_PROCESS_ERROR(m_lpCurModel);

    if (m_ctlAutoChange.GetCheck() == BST_CHECKED)
    {
        hr = m_lpCurModel->GetNumMaterial(&nCount);
        KGLOG_COM_PROCESS_ERROR(hr);

        for(int i = 0; i < nCount; i ++)
        {
            if(m_ctlSubsetList.GetSel(i))
            {
                hr = m_lpCurModel->GetMaterialSubsetOption(i, &dwOption);
                KG_COM_PROCESS_ERROR(hr);

                if(m_CheckSpec.GetCheck())
                {
                    dwOption |= MATERIAL_OPTION_SPEC;
                }
                else
                {
                    dwOption &= (~MATERIAL_OPTION_SPEC);
                }

                hr = m_lpCurModel->SetMaterialSubsetOption(i, dwOption);
                KGLOG_COM_PROCESS_ERROR(hr);
            }
        }
    }
    else
    {
        hr = m_lpCurModel->GetMaterialSubsetOption(m_nBaseMaterial, &dwOption);
        KG_COM_PROCESS_ERROR(hr);

        if(m_CheckSpec.GetCheck())
        {
            dwOption |= MATERIAL_OPTION_SPEC;
        }
        else
        {
            dwOption &= (~MATERIAL_OPTION_SPEC);
        }
        hr = m_lpCurModel->SetMaterialSubsetOption(m_nBaseMaterial, dwOption);
        KGLOG_COM_PROCESS_ERROR(hr);
    }
Exit0:
    return;
}

void KSceneModelEditorPropertyPageMaterial::OnBnClickedFlushTexs()
{
    if (m_lpCurModel)
        m_lpCurModel->RefurbishTexts();   

}


void KSceneModelEditorPropertyPageMaterial::OnBnClickedCheckEmap()
{
    HRESULT hr = E_FAIL;
    int nCount = 0;
    DWORD dwOption = 0;

    KG_PROCESS_ERROR(m_lpCurModel);

    if (m_ctlAutoChange.GetCheck() == BST_CHECKED)
    {
        hr = m_lpCurModel->GetNumMaterial(&nCount);
        KGLOG_COM_PROCESS_ERROR(hr);

        for(int i = 0; i < nCount; i ++)
        {
            if(m_ctlSubsetList.GetSel(i))
            {
                hr = m_lpCurModel->GetMaterialSubsetOption(i, &dwOption);
                KG_COM_PROCESS_ERROR(hr);

                if(m_CheckEMap.GetCheck())
                {
                    dwOption |= MATERIAL_OPTION_EMAP;
                }
                else
                {
                    dwOption &= (~MATERIAL_OPTION_EMAP);
                }

                hr = m_lpCurModel->SetMaterialSubsetOption(i, dwOption);
                KGLOG_COM_PROCESS_ERROR(hr);
            }
        }
    }
    else
    {
        hr = m_lpCurModel->GetMaterialSubsetOption(m_nBaseMaterial, &dwOption);
        KG_COM_PROCESS_ERROR(hr);

        if(m_CheckEMap.GetCheck())
        {
            dwOption |= MATERIAL_OPTION_EMAP;
        }
        else
        {
            dwOption &= (~MATERIAL_OPTION_EMAP);
        }
        hr = m_lpCurModel->SetMaterialSubsetOption(m_nBaseMaterial, dwOption);
        KGLOG_COM_PROCESS_ERROR(hr);
    }
Exit0:
    return;
}

void KSceneModelEditorPropertyPageMaterial::OnEnChangeEditTwicealphatop()
{
	UpdateData(TRUE);
	if (m_ctlAutoChange.GetCheck() == BST_CHECKED)
	{
		for(int i = 0; i < m_ctlSubsetList.GetCount(); i++)
		{
			if(!m_ctlSubsetList.GetSel(i))
				continue;

			m_lpCurModel->SetMaterialSubsetRenderTwiceAlphaOption(i, TRUE, m_dwAlphaTop, m_dwAlphaBottom);

		}
	}
	else
	{
		m_lpCurModel->SetMaterialSubsetRenderTwiceAlphaOption(m_nBaseMaterial, TRUE, m_dwAlphaTop, m_dwAlphaBottom);
	}
}

void KSceneModelEditorPropertyPageMaterial::OnBnClickedButtonSaveas()
{
	HRESULT hr = E_FAIL;
	TCHAR bigBuff[800] = "";  // maximum common dialog buffer size
	TCHAR szFilter[] ="Material Files (*.Mtl)|*.Mtl|";
	IEKG3DMaterial* pMat = NULL;
	CString strPathName;
	CFileDialog dlg(FALSE, "Mtl", NULL,
		OFN_HIDEREADONLY , szFilter);

	KG_PROCESS_ERROR(m_lpCurModel);

	pMat = GetMaterialFromModel();
	KG_PROCESS_ERROR(pMat);

	dlg.m_ofn.lpstrFile = bigBuff;
	dlg.m_ofn.nMaxFile = sizeof(bigBuff);
	KG_PROCESS_ERROR(dlg.DoModal() == IDOK);

	strPathName = dlg.GetPathName();
	if (!CheckFileReadOnly(strPathName))
	{
		SetDefaultPath();
		hr = pMat->SaveToFile(strPathName, 0);
		RestorePath();
		KGLOG_COM_PROCESS_ERROR(hr);
	}
Exit0:
	return;
}

void KSceneModelEditorPropertyPageMaterial::OnBnClickedCheckSortassfx()
{
	UpdateData(TRUE);

	IEKG3DMaterial* pMat = NULL;
	KG_PROCESS_ERROR(m_lpCurModel);

	pMat = GetMaterialFromModel();
	KG_PROCESS_ERROR(pMat);

	pMat->SetSortAsSFX(m_bSortAsSFX);
Exit0:
	return;
}

void KSceneModelEditorPropertyPageMaterial::OnBnClickedButtonDetail()
{
	UpdateData(TRUE);

	IEKG3DMaterial* pMat = NULL;
	KG_PROCESS_ERROR(m_lpCurModel);

	pMat = GetMaterialFromModel();
	KG_PROCESS_ERROR(pMat);

	if(!m_lpDetailDialog)
	{
		m_lpDetailDialog = new KModelEditorMaterialDetailDialog(this);
		KG_PROCESS_ERROR(m_lpDetailDialog);
		m_lpDetailDialog->Create(KModelEditorMaterialDetailDialog::IDD,this);
	}
	m_lpDetailDialog->SetMaterial(pMat,m_lpCurModel);
    m_lpDetailDialog->SetParentHwnd(this);
	m_lpDetailDialog->ShowWindow(TRUE);
	
Exit0:
	return;
}
