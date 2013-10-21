// KModelEditorMaterialDetailDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KModelEditorMaterialDetailDialog.h"
#include "IEKG3DMaterial.h"
#include "IEKG3DTextureTable.h"

// KModelEditorMaterialDetailDialog dialog
COLORREF ValueToColor(D3DCOLORVALUE Value)
{
	return (0xff000000 | RGB(Value.r * 255, Value.g * 255, Value.b * 255));
}

IMPLEMENT_DYNAMIC(KModelEditorMaterialDetailDialog, CDialog)

KModelEditorMaterialDetailDialog::KModelEditorMaterialDetailDialog(CWnd* pParent /*=NULL*/)
	: CDialog(KModelEditorMaterialDetailDialog::IDD, pParent)
	, m_fBody_Max(0)
	, m_fBodyMin(0)
	, m_fBody_Scale(0)
	, m_fDetail0_ColorScale(0)
	, m_fDetail0_UVScale(0)
	, m_fDetail1_ColorScale(0)
	, m_fDetail1_UVScale(0)
	, m_fBody_Env(0)
	, m_fBody_Specular(0)
	, m_szTableName(_T(""))
	, m_szMainName(_T(""))
	, m_szMtlDefineName(_T(""))
	, m_fDetail0_Env(0)
	, m_fDetail0_Specular(0)
	, m_fDetail1_Env(0)
	, m_fDetail1_Specular(0)
    , m_bColorCastWhole(TRUE)
	, m_fFlow_ColorScale(0)
	, m_fFlow_UVScale(0)
	, m_fFlow_SpeedX(0)
	, m_fFlow_SpeedY(0)
{
	m_lpMaterial = NULL;
	m_nCurrentTableIndex = 0;
	m_nCurrentTableBodyIndex = 0;
	m_nCurrentMtlDefineIndex = 0;
    m_nPreMtlDefineIndex = -1;
	m_eModifyState = MODIFY_NONE;
	m_bHasCopyData = FALSE;
    m_hWnd = NULL;
    m_nCurSubSetIndex = 0;

    m_strTextureFileName = "";
    m_strCurTextureFileName = "";
}

KModelEditorMaterialDetailDialog::~KModelEditorMaterialDetailDialog()
{
}

void KModelEditorMaterialDetailDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_BODYMAX, m_fBody_Max);
	DDX_Text(pDX, IDC_EDIT_BODYMIN, m_fBodyMin);
	DDX_Text(pDX, IDC_EDIT_BODYSCALE, m_fBody_Scale);

	DDX_Text(pDX, IDC_EDIT_DETAIL0_COLORSCALE, m_fDetail0_ColorScale);
	DDX_Text(pDX, IDC_EDIT_DETAIL0_UVSCALE, m_fDetail0_UVScale);
	DDX_Control(pDX, IDC_BUTTON_DETAIL0_TEXTURE, m_Button_Detail0_Texture);
	DDX_Text(pDX, IDC_EDIT_DETAIL1_COLORSCALE, m_fDetail1_ColorScale);
	DDX_Text(pDX, IDC_EDIT_DETAIL1_UVSCALE, m_fDetail1_UVScale);
	DDX_Control(pDX, IDC_BUTTON_DETAIL1_TEXTURE, m_Button_Detail1_Texture);
	DDX_Control(pDX, IDC_COMBO_TABLEINDEX, m_Combo_Table);
	DDX_Control(pDX, IDC_LIST_BODYDEFINE, m_List_BodyDefine);
	DDX_Control(pDX, IDC_LIST_MTLDEFINE, m_List_MtlDefine);
	DDX_Text(pDX, IDC_EDIT_BODYENV, m_fBody_Env);
	DDX_Text(pDX, IDC_EDIT_BODYSPECULAR, m_fBody_Specular);
	DDX_Text(pDX, IDC_EDIT_TABLENAME, m_szTableName);
	DDX_Text(pDX, IDC_EDIT_MAINNAME, m_szMainName);
	DDX_Text(pDX, IDC_EDIT_MTLNAME, m_szMtlDefineName);
	DDX_Text(pDX, IDC_EDIT_BODYENV2, m_fDetail0_Env);
	DDX_Text(pDX, IDC_EDIT_BODYSPECULAR2, m_fDetail0_Specular);
	DDX_Text(pDX, IDC_EDIT_BODYENV3, m_fDetail1_Env);
	DDX_Text(pDX, IDC_EDIT_BODYSPECULAR3, m_fDetail1_Specular);
	DDX_Control(pDX, IDC_BUTTON_PASTE, m_ButtonPaste);

	DDX_Control(pDX, IDC_BUTTON_BODYCOLOR, m_Button_Color[ENUM_BUTTON_BODY1]);
	DDX_Control(pDX, IDC_BUTTON_BODYCOLOR2, m_Button_Color[ENUM_BUTTON_BODY2]);
	DDX_Control(pDX, IDC_BUTTON_DETAIL0_COLOR, m_Button_Color[ENUM_BUTTON_DETAIL1]);
	DDX_Control(pDX, IDC_BUTTON_DETAIL1_COLOR, m_Button_Color[ENUM_BUTTON_DETAIL2]);
	DDX_Control(pDX, IDC_BUTTON_FLOW_COLOR, m_Button_Color[ENUM_BUTTON_FLOW]);

	DDX_Control(pDX, IDC_CHECK_WHOLE, m_Button_ColorCastWhole);
	DDX_Control(pDX, IDC_COMBO_SUBSET, m_Combo_SubsetList);
	DDX_Text(pDX, IDC_EDIT_FLOW_COLORSCALE, m_fFlow_ColorScale);
	DDX_Text(pDX, IDC_EDIT_FLOW_UVSCALE, m_fFlow_UVScale);
	DDX_Text(pDX, IDC_EDIT_FLOW_SPEED, m_fFlow_SpeedX);
	DDX_Text(pDX, IDC_EDIT_FLOW_SPEEDY, m_fFlow_SpeedY);
	DDX_Control(pDX, IDC_BUTTON_FLOW_TEXTURE, m_Button_FlowTexture);
}


BEGIN_MESSAGE_MAP(KModelEditorMaterialDetailDialog, CDialog)
	ON_EN_CHANGE(IDC_EDIT_BODYMAX, &KModelEditorMaterialDetailDialog::OnEnChangeEditBodymax)
	ON_EN_CHANGE(IDC_EDIT_BODYMIN, &KModelEditorMaterialDetailDialog::OnEnChangeEditBodymin)
	ON_EN_CHANGE(IDC_EDIT_BODYSCALE, &KModelEditorMaterialDetailDialog::OnEnChangeEditBodyscale)

	ON_EN_CHANGE(IDC_EDIT_DETAIL0_COLORSCALE, &KModelEditorMaterialDetailDialog::OnEnChangeEditDetail0Colorscale)
	ON_EN_CHANGE(IDC_EDIT_DETAIL0_UVSCALE, &KModelEditorMaterialDetailDialog::OnEnChangeEditDetail0Uvscale)
	ON_BN_CLICKED(IDC_BUTTON_DETAIL0_TEXTURE, &KModelEditorMaterialDetailDialog::OnBnClickedButtonDetail0Texture)
	ON_EN_CHANGE(IDC_EDIT_DETAIL1_COLORSCALE, &KModelEditorMaterialDetailDialog::OnEnChangeEditDetail1Colorscale)
	ON_EN_CHANGE(IDC_EDIT_DETAIL1_UVSCALE, &KModelEditorMaterialDetailDialog::OnEnChangeEditDetail1Uvscale)
	ON_BN_CLICKED(IDC_BUTTON_DETAIL1_TEXTURE, &KModelEditorMaterialDetailDialog::OnBnClickedButtonDetail1Texture)
	ON_WM_SHOWWINDOW()
	ON_CBN_SELCHANGE(IDC_COMBO_TABLEINDEX, &KModelEditorMaterialDetailDialog::OnCbnSelchangeComboTableindex)
	ON_BN_CLICKED(IDC_BUTTON_ADDTABLE, &KModelEditorMaterialDetailDialog::OnBnClickedButtonAddtable)
	ON_LBN_SELCHANGE(IDC_LIST_BODYDEFINE, &KModelEditorMaterialDetailDialog::OnLbnSelchangeListBodydefine)
	ON_BN_CLICKED(IDC_BUTTON_ADDBODY, &KModelEditorMaterialDetailDialog::OnBnClickedButtonAddbody)
	ON_BN_CLICKED(IDC_BUTTON_DELETEBODY, &KModelEditorMaterialDetailDialog::OnBnClickedButtonDeletebody)
	ON_LBN_SELCHANGE(IDC_LIST_MTLDEFINE, &KModelEditorMaterialDetailDialog::OnLbnSelchangeListMtldefine)
	ON_BN_CLICKED(IDC_BUTTON_ADDMTLDEFINE, &KModelEditorMaterialDetailDialog::OnBnClickedButtonAddmtldefine)
	ON_BN_CLICKED(IDC_BUTTON_DELETEMTLDEFINE, &KModelEditorMaterialDetailDialog::OnBnClickedButtonDeletemtldefine)
	ON_BN_CLICKED(IDC_BUTTON_DELETETABLE, &KModelEditorMaterialDetailDialog::OnBnClickedButtonDeletetable)
	ON_BN_CLICKED(IDC_BUTTON_MAINRENAME, &KModelEditorMaterialDetailDialog::OnBnClickedButtonMainrename)
	ON_BN_CLICKED(IDC_BUTTON_DETAILRENAME, &KModelEditorMaterialDetailDialog::OnBnClickedButtonDetailrename)
	ON_BN_CLICKED(IDC_BUTTON_MTLRENAME, &KModelEditorMaterialDetailDialog::OnBnClickedButtonMtlrename)
	ON_BN_CLICKED(IDC_BUTTON_TABLERENAME, &KModelEditorMaterialDetailDialog::OnBnClickedButtonTablerename)
	ON_EN_CHANGE(IDC_EDIT_BODYENV, &KModelEditorMaterialDetailDialog::OnEnChangeEditBodyenv)
	ON_EN_CHANGE(IDC_EDIT_BODYSPECULAR, &KModelEditorMaterialDetailDialog::OnEnChangeEditBodyspecular)
	ON_BN_CLICKED(IDC_BUTTON_DETAIL0_CLEAR, &KModelEditorMaterialDetailDialog::OnBnClickedButtonDetail0Clear)
	ON_BN_CLICKED(IDC_BUTTON_, &KModelEditorMaterialDetailDialog::OnBnClickedButton)
	ON_EN_CHANGE(IDC_EDIT_BODYENV2, &KModelEditorMaterialDetailDialog::OnEnChangeEditBodyenv2)
	ON_EN_CHANGE(IDC_EDIT_BODYSPECULAR2, &KModelEditorMaterialDetailDialog::OnEnChangeEditBodyspecular2)
	ON_EN_CHANGE(IDC_EDIT_BODYENV3, &KModelEditorMaterialDetailDialog::OnEnChangeEditBodyenv3)
	ON_EN_CHANGE(IDC_EDIT_BODYSPECULAR3, &KModelEditorMaterialDetailDialog::OnEnChangeEditBodyspecular3)
	ON_BN_CLICKED(IDC_BUTTON_SAVETABLE2, &KModelEditorMaterialDetailDialog::OnBnClickedButtonSavetable2)
	ON_BN_CLICKED(IDC_BUTTON_COPY, &KModelEditorMaterialDetailDialog::OnBnClickedButtonCopy)
	ON_BN_CLICKED(IDC_BUTTON_PASTE, &KModelEditorMaterialDetailDialog::OnBnClickedButtonPaste)
	ON_BN_CLICKED(IDC_BUTTON_SAVEMTL, &KModelEditorMaterialDetailDialog::OnBnClickedButtonSavemtl)
    ON_BN_CLICKED(IDC_BUTTON_CHANGETEXTURE, &KModelEditorMaterialDetailDialog::OnBnClickedButtonChangetexture)
    ON_BN_CLICKED(IDC_CHECK_WHOLE, &KModelEditorMaterialDetailDialog::OnBnClickedCheckWhole)
    ON_CBN_SELCHANGE(IDC_COMBO_SUBSET, &KModelEditorMaterialDetailDialog::OnCbnSelchangeComboSubset)
	ON_EN_CHANGE(IDC_EDIT_FLOW_COLORSCALE, &KModelEditorMaterialDetailDialog::OnEnChangeEditFlowColorscale)
	ON_EN_CHANGE(IDC_EDIT_FLOW_UVSCALE, &KModelEditorMaterialDetailDialog::OnEnChangeEditFlowUvscale)
	ON_EN_CHANGE(IDC_EDIT_FLOW_SPEED, &KModelEditorMaterialDetailDialog::OnEnChangeEditFlowSpeed)
	ON_BN_CLICKED(IDC_BUTTON_FLOW_TEXTURE, &KModelEditorMaterialDetailDialog::OnBnClickedButtonFlowTexture)
	ON_BN_CLICKED(IDC_BUTTON_FLOW_TEXTURECLEAR, &KModelEditorMaterialDetailDialog::OnBnClickedButtonFlowTextureclear)
	ON_BN_CLICKED(IDC_BUTTON_DETAIL1_COLOR, &KModelEditorMaterialDetailDialog::OnBnClickedButtonDetail1Color)
	ON_BN_CLICKED(IDC_BUTTON_FLOW_COLOR, &KModelEditorMaterialDetailDialog::OnBnClickedButtonFlowColor)
	ON_EN_CHANGE(IDC_EDIT_FLOW_SPEEDY, &KModelEditorMaterialDetailDialog::OnEnChangeEditFlowSpeedy)
END_MESSAGE_MAP()


// KModelEditorMaterialDetailDialog message handlers

void KModelEditorMaterialDetailDialog::OnEnChangeEditBodymax()
{
	Update(TRUE);
}

void KModelEditorMaterialDetailDialog::OnEnChangeEditBodymin()
{
	Update(TRUE);
}

void KModelEditorMaterialDetailDialog::OnEnChangeEditBodyscale()
{
	Update(TRUE);
}

void KModelEditorMaterialDetailDialog::OnEnChangeEditDetail0Colorscale()
{
	Update(TRUE);
}

void KModelEditorMaterialDetailDialog::OnEnChangeEditDetail0Uvscale()
{
	Update(TRUE);
}

void KModelEditorMaterialDetailDialog::OnBnClickedButtonDetail0Texture()
{
	HRESULT hr = E_FAIL;
	TCHAR strFilter[] = "Texture Files |*.bmp;*.jpg;*.png;*.tga;*.Spr;*.DDS;";
	CString fileName;
	CFileDialog DlgLoad(TRUE, ".dds", "test.dds", OFN_HIDEREADONLY , strFilter);
	IEKG3DEngineManager *pManager = g_GetEngineIEInterface();

	TCHAR szAppDirectory[MAX_PATH] = { 0 };
	pManager->GetDefWorkDirectory(szAppDirectory, MAX_PATH);

	TCHAR szDir[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH,szDir);

	TCHAR RetFileName[MAX_PATH];

	if(DlgLoad.DoModal() == IDOK)
	{
		fileName = DlgLoad.GetPathName();

		HRESULT hr = pManager->GetRelativePath(szAppDirectory,fileName,RetFileName);
		if(SUCCEEDED(hr))
		{
			m_szDetailFile0 = RetFileName;
			m_Button_Detail0_Texture.SetWindowText(RetFileName);
			Update(TRUE);
		}
	}
	::SetCurrentDirectory(szDir);
}

void KModelEditorMaterialDetailDialog::OnEnChangeEditDetail1Colorscale()
{
	Update(TRUE);
}

void KModelEditorMaterialDetailDialog::OnEnChangeEditDetail1Uvscale()
{
	Update(TRUE);
}

void KModelEditorMaterialDetailDialog::OnBnClickedButtonDetail1Texture()
{
	HRESULT hr = E_FAIL;
	TCHAR strFilter[] = "Texture Files |*.bmp;*.jpg;*.png;*.tga;*.Spr;*.DDS;";
	CString fileName;
	CFileDialog DlgLoad(TRUE, ".dds", "test.dds", OFN_HIDEREADONLY , strFilter);
	IEKG3DEngineManager *pManager = g_GetEngineIEInterface();

	TCHAR szAppDirectory[MAX_PATH] = { 0 };
	pManager->GetDefWorkDirectory(szAppDirectory, MAX_PATH);

	TCHAR szDir[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH,szDir);

	TCHAR RetFileName[MAX_PATH];

	if(DlgLoad.DoModal() == IDOK)
	{
		fileName = DlgLoad.GetPathName();

		HRESULT hr = pManager->GetRelativePath(szAppDirectory,fileName,RetFileName);
		if(SUCCEEDED(hr))
		{
			m_szDetailFile1 = RetFileName;
			m_Button_Detail1_Texture.SetWindowText(RetFileName);
			Update(TRUE);
		}
	}
	::SetCurrentDirectory(szDir);
}



HRESULT KModelEditorMaterialDetailDialog::Update(BOOL bToModel)
{
	KGLOG_PROCESS_ERROR(m_lpMaterial);

    HRESULT hRetCode = E_FAIL;
    TCHAR szChannel[MAX_PATH] = {0};
    TCHAR szDetail0[MAX_PATH] = {0};
    TCHAR szDetail1[MAX_PATH] = {0};
	TCHAR szFlowTexture[MAX_PATH] = {0};
    TCHAR szName   [MAX_PATH] = {0};
    TCHAR szTexture[MAX_PATH] = {0};
	wsprintf(szName   ,"%s",m_szMtlDefineName      );
    wsprintf(szTexture, "%s", m_strTextureFileName.c_str());
	wsprintf(szFlowTexture, "%s", m_strFlowTexture.c_str());

	if(bToModel)
	{
		UpdateData(TRUE);

		wsprintf(szChannel,"%s",m_szChannelFile.c_str());
		wsprintf(szDetail0,"%s",m_szDetailFile0.c_str());
		wsprintf(szDetail1,"%s",m_szDetailFile1.c_str());
		wsprintf(szFlowTexture, "%s", m_strFlowTexture.c_str());

		D3DXVECTOR2 vSpeed(m_fFlow_SpeedX,m_fFlow_SpeedY);

		m_lpModel->SetMaterilDetail(-1, m_nCurSubSetIndex, szName, szTexture, m_fBody_Env,m_fBody_Specular,
			m_fBody_Max,m_fBodyMin,m_fBody_Scale,m_vBodyColor,m_vBodyColor1,szChannel,
			m_fDetail0_ColorScale,m_fDetail0_UVScale,m_fDetail0_Env,m_fDetail0_Specular,m_vDetail0Color,szDetail0,
			m_fDetail1_ColorScale,m_fDetail1_UVScale,m_fDetail1_Env,m_fDetail1_Specular,m_vDetail1Color,szDetail1,
			m_vFlow_Color,m_fFlow_ColorScale,m_fFlow_UVScale,vSpeed,szFlowTexture);

		IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
		IEKG3DTextureTable  *pTextureTable = NULL;
		pManager->GetIEKG3DTextureTable(&pTextureTable);

		if(m_eModifyState == Modify_Table_Body)
		{
			wsprintf(szName,"%s",m_szMainName);
			pTextureTable->Detail_SetBodyColor(m_nCurrentTableIndex,m_nCurrentTableBodyIndex,m_vBodyColor,m_vBodyColor1,m_fBody_Max,m_fBodyMin,m_fBody_Scale,m_fBody_Env,m_fBody_Specular,szName,
				m_vDetail0Color,m_fDetail0_ColorScale,m_fDetail0_UVScale,m_fDetail0_Env,m_fDetail0_Specular,szDetail0,
				m_vDetail1Color,m_fDetail1_ColorScale,m_fDetail1_UVScale,m_fDetail1_Env,m_fDetail1_Specular,szDetail1,
				m_vFlow_Color,m_fFlow_ColorScale,m_fFlow_UVScale,vSpeed,szFlowTexture);
		}
		else if(m_eModifyState == Modify_Mtl)
		{
			wsprintf(szName,"%s",m_szMtlDefineName);
			m_lpModel->SetMaterilDetail(m_nCurrentMtlDefineIndex, m_nCurSubSetIndex, szName, szTexture, m_fBody_Env,m_fBody_Specular,
				m_fBody_Max,m_fBodyMin,m_fBody_Scale,m_vBodyColor,m_vBodyColor1,szChannel,
				m_fDetail0_ColorScale,m_fDetail0_UVScale,m_fDetail0_Env,m_fDetail0_Specular,m_vDetail0Color,szDetail0,
				m_fDetail1_ColorScale,m_fDetail1_UVScale,m_fDetail1_Env,m_fDetail1_Specular,m_vDetail1Color,szDetail1,
				m_vFlow_Color,m_fFlow_ColorScale,m_fFlow_UVScale,vSpeed,szFlowTexture);
		}

		m_Button_Detail0_Texture.SetWindowText(m_szDetailFile0.c_str());
		m_Button_Detail1_Texture.SetWindowText(m_szDetailFile1.c_str());

		m_Button_FlowTexture.SetWindowText(m_strFlowTexture.c_str());

	}
	else
	{
		D3DXVECTOR2 vSpeed(m_fFlow_SpeedX,m_fFlow_SpeedY);

		hRetCode = m_lpModel->GetMaterilDetail(-1, m_nCurSubSetIndex, szName, szTexture, &m_fBody_Env,&m_fBody_Specular,
			&m_fBody_Max,&m_fBodyMin,&m_fBody_Scale,&m_vBodyColor,&m_vBodyColor1,szChannel,
			&m_fDetail0_ColorScale,&m_fDetail0_UVScale,&m_fDetail0_Env,&m_fDetail0_Specular,&m_vDetail0Color,szDetail0,
			&m_fDetail1_ColorScale,&m_fDetail1_UVScale,&m_fDetail1_Env,&m_fDetail1_Specular,&m_vDetail1Color,szDetail1,
			&m_vFlow_Color,&m_fFlow_ColorScale,&m_fFlow_UVScale,&vSpeed,szFlowTexture);

		m_szChannelFile = szChannel;
		m_szDetailFile0 = szDetail0;
		m_szDetailFile1 = szDetail1;
        m_strTextureFileName = szTexture;
		m_fFlow_SpeedX = vSpeed.x;
		m_fFlow_SpeedY = vSpeed.y;

        if (SUCCEEDED(hRetCode))
        {
            if (m_strCurTextureFileName != m_strTextureFileName)
            {
                m_lpModel->ChangeTexture(0, 0, m_strTextureFileName.c_str());           
            }
            m_strCurTextureFileName = m_strTextureFileName;
        }

		m_Button_Detail0_Texture.SetWindowText(m_szDetailFile0.c_str());
		m_Button_Detail1_Texture.SetWindowText(m_szDetailFile1.c_str());

		m_Button_FlowTexture.SetWindowText(m_strFlowTexture.c_str());

		UpdateData(FALSE);
	}

	return S_OK;
Exit0:
	return E_FAIL;
}


void KModelEditorMaterialDetailDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if(bShow)
	{
		Update(FALSE);

		m_ButtonPaste.EnableWindow(m_bHasCopyData);
        m_nPreMtlDefineIndex = -1;

		FillTableCombo();
		FillListMtlDefine();
        FillComboSubSet();
        SetButtonColor();

		if(m_lpMaterial)
		{
			LPCTSTR pmtlName = NULL;
			m_lpMaterial->GetName(&pmtlName);
			TCHAR strFile[MAX_PATH]; 
			TCHAR strDrive[MAX_PATH];
			TCHAR strPath[MAX_PATH];
			TCHAR strExt[MAX_PATH];
			_splitpath_s(pmtlName, 
				strDrive, 
				MAX_PATH,
				strPath, 
				MAX_PATH, 
				strFile, 
				MAX_PATH, 
				strExt,
				MAX_PATH);
			TCHAR  szCaption[MAX_PATH];
			wsprintf(szCaption,"%s%s",strFile,strExt);
			this->SetWindowText(szCaption);
            SetComboTableIndex(strFile);
		}
	}
	// TODO: Add your message handler code here
}

LRESULT KModelEditorMaterialDetailDialog::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
    case WM_COLORPOINTER_CHANGE:
        {
            DWORD dwColor = (DWORD)wParam;
            HWND hWndTmp = (HWND)lParam;
            ColorUpdate(dwColor, hWndTmp);
            Update(TRUE);
        }
        break;
    default:
        break;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

BOOL KModelEditorMaterialDetailDialog::OnInitDialog()
{
    for (int i = 0; i < ENUM_BUTTON_COUNT; i++)
        m_Button_Color[i].SetAlphaEnable(TRUE);
    
    return CDialog::OnInitDialog();
}

void KModelEditorMaterialDetailDialog::OnCbnSelchangeComboTableindex()
{
	int nIndex = m_Combo_Table.GetCurSel();
	if(nIndex>=0)
	{
		m_nCurrentTableIndex = (int)m_Combo_Table.GetItemData(nIndex);
	}

	FillListMainColor();

	IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
	IEKG3DTextureTable  *pTextureTable = NULL;
	pManager->GetIEKG3DTextureTable(&pTextureTable);

	TCHAR szName[MAX_PATH];
	pTextureTable->Detail_GetDefineTableName(m_nCurrentTableIndex,szName);
	m_szTableName.Format("%s",szName);

	UpdateData(FALSE);
}

void KModelEditorMaterialDetailDialog::OnBnClickedButtonAddtable()
{
	IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
	IEKG3DTextureTable  *pTextureTable = NULL;
	pManager->GetIEKG3DTextureTable(&pTextureTable);

	pTextureTable->Detail_AddDefineTable();

	FillTableCombo();
}

void KModelEditorMaterialDetailDialog::OnLbnSelchangeListBodydefine()
{
	int nIndex = m_List_BodyDefine.GetCurSel();
	if(nIndex>=0)
	{
		m_nCurrentTableBodyIndex = (int)m_List_BodyDefine.GetItemData(nIndex);
	}
	else
		return;

	m_eModifyState = Modify_Table_Body;

	IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
	IEKG3DTextureTable  *pTextureTable = NULL;
	pManager->GetIEKG3DTextureTable(&pTextureTable);

	TCHAR szName[MAX_PATH];
	TCHAR szFileName0[MAX_PATH];
	TCHAR szFileName1[MAX_PATH];
	TCHAR szFlowTexture[MAX_PATH];

	ZeroMemory(szName,sizeof(szName));
	ZeroMemory(szFileName0,sizeof(szFileName0));
	ZeroMemory(szFileName1,sizeof(szFileName1));
	ZeroMemory(szFlowTexture,sizeof(szFlowTexture));

	D3DXVECTOR2 vSpeed(m_fFlow_SpeedX,m_fFlow_SpeedY);

	pTextureTable->Detail_GetBodyColor(m_nCurrentTableIndex,m_nCurrentTableBodyIndex,m_vBodyColor,m_vBodyColor1,
		m_fBody_Max,m_fBodyMin,m_fBody_Scale,m_fBody_Env,m_fBody_Specular,szName,
		m_vDetail0Color,m_fDetail0_ColorScale,m_fDetail0_UVScale,m_fDetail0_Env,m_fDetail0_Specular,szFileName0,
		m_vDetail1Color,m_fDetail1_ColorScale,m_fDetail1_UVScale,m_fDetail1_Env,m_fDetail1_Specular,szFileName1,
		&m_vFlow_Color,&m_fFlow_ColorScale,&m_fFlow_UVScale,&vSpeed,szFlowTexture);

	m_szMainName.Format("%s",szName);
	m_szDetailFile0 = szFileName0;
	m_szDetailFile1 = szFileName1;
	m_fFlow_SpeedX = vSpeed.x;
	m_fFlow_SpeedY = vSpeed.y;
	m_strFlowTexture = szFlowTexture;

	UpdateData(FALSE);

	Update(TRUE);
    SetButtonColor();

}

void KModelEditorMaterialDetailDialog::OnBnClickedButtonAddbody()
{
	IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
	IEKG3DTextureTable  *pTextureTable = NULL;
	pManager->GetIEKG3DTextureTable(&pTextureTable);

	pTextureTable->Detail_AddBodyDefine(m_nCurrentTableIndex,m_nCurrentTableBodyIndex);

	FillListMainColor();
}

void KModelEditorMaterialDetailDialog::OnBnClickedButtonDeletebody()
{
	IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
	IEKG3DTextureTable  *pTextureTable = NULL;
	pManager->GetIEKG3DTextureTable(&pTextureTable);

	pTextureTable->Detail_DeleteBodyDefine(m_nCurrentTableIndex,m_nCurrentTableBodyIndex);

	FillListMainColor();
}

void KModelEditorMaterialDetailDialog::OnLbnSelchangeListMtldefine()
{
    HRESULT hRetCode = E_FAIL;
	int nIndex = m_List_MtlDefine.GetCurSel();
    KG_PROCESS_ERROR(nIndex >= 0);
		
    m_nCurrentMtlDefineIndex = (int)m_List_MtlDefine.GetItemData(nIndex);
	m_eModifyState = Modify_Mtl;

    TCHAR szName   [MAX_PATH] = {0};
    TCHAR szChannel[MAX_PATH] = {0};
    TCHAR szDetail0[MAX_PATH] = {0};
    TCHAR szDetail1[MAX_PATH] = {0};
    TCHAR szTexture[MAX_PATH] = {0};
	TCHAR szFlowTexture[MAX_PATH] = {0};
	ZeroMemory(szName,sizeof(szName));
	ZeroMemory(szChannel,sizeof(szChannel));
	ZeroMemory(szDetail0,sizeof(szDetail0));
	ZeroMemory(szDetail1,sizeof(szDetail1));
	ZeroMemory(szFlowTexture,sizeof(szFlowTexture));

	if(m_lpMaterial)
	{
        BOOL bChanged = m_nPreMtlDefineIndex != m_nCurrentMtlDefineIndex;
        if (bChanged)
        {
            m_nPreMtlDefineIndex = m_nCurrentMtlDefineIndex;

            BOOL bExist = m_lpMaterial->IsSubSetDetailExist(m_nCurrentMtlDefineIndex);
            m_bColorCastWhole = !bExist;
            m_Button_ColorCastWhole.SetCheck(m_bColorCastWhole);
            m_Combo_SubsetList.EnableWindow(bExist);
            m_lpMaterial->SetDetailWholeColorCast(m_bColorCastWhole);
        }

		D3DXVECTOR2 vSpeed(1,1);
		hRetCode = m_lpModel->GetMaterilDetail(m_nCurrentMtlDefineIndex, m_nCurSubSetIndex, szName, szTexture, &m_fBody_Env,&m_fBody_Specular,
			&m_fBody_Max,&m_fBodyMin,&m_fBody_Scale,&m_vBodyColor,&m_vBodyColor1,szChannel,
			&m_fDetail0_ColorScale,&m_fDetail0_UVScale,&m_fDetail0_Env,&m_fDetail0_Specular,&m_vDetail0Color,szDetail0,
			&m_fDetail1_ColorScale,&m_fDetail1_UVScale,&m_fDetail1_Env,&m_fDetail1_Specular,&m_vDetail1Color,szDetail1,
			&m_vFlow_Color,&m_fFlow_ColorScale,&m_fFlow_UVScale,&vSpeed,szFlowTexture);

		m_fFlow_SpeedX = vSpeed.x;
		m_fFlow_SpeedY = vSpeed.y;
		m_szDetailFile0 = szDetail0;
		m_szDetailFile1 = szDetail1;
        m_strTextureFileName = szTexture;
		m_strFlowTexture = szFlowTexture;

        if (SUCCEEDED(hRetCode))
        {
            if (m_strCurTextureFileName != m_strTextureFileName)
            {
                m_lpModel->ChangeTexture(0, 0, m_strTextureFileName.c_str());           
            }
            m_strCurTextureFileName = m_strTextureFileName;
        }

        if (bChanged)
        {
            if (SUCCEEDED(hRetCode) && !m_bColorCastWhole)
                m_lpModel->SetDetail(-1, m_nCurrentMtlDefineIndex);
        }
	}

	m_szMtlDefineName.Format("%s",szName);
	m_Button_FlowTexture.SetWindowText(szFlowTexture);

	UpdateData(FALSE);

	m_lpModel->SetDetail(m_nCurrentTableBodyIndex,m_nCurrentMtlDefineIndex);

	Update(TRUE);
    SetButtonColor();

Exit0:
    ;
}

void KModelEditorMaterialDetailDialog::OnBnClickedButtonAddmtldefine()
{
    HRESULT hRetCode = E_FAIL;
    int nIndex = m_List_MtlDefine.GetCurSel();
	if(m_lpMaterial)
	{
        if (m_lpMaterial->FindIndex(m_nCurrentMtlDefineIndex) != -1 || 
            nIndex < 0
        )
        {
            MessageBox("没有选中条目或者选中的条目已经存在", "Error!",0);
            return;
        }

		m_lpMaterial->AddMaterialDetailDefine(m_nCurrentMtlDefineIndex);

        m_nPreMtlDefineIndex = -1;
        FillListMtlDefine();
	}
}

void KModelEditorMaterialDetailDialog::OnBnClickedButtonDeletemtldefine()
{
	if(m_lpMaterial)
	{
		m_lpMaterial->DeleteMaterialDetailDefine(m_nCurrentMtlDefineIndex);

        m_nPreMtlDefineIndex = -1;
        FillListMtlDefine();
	}
}

void  KModelEditorMaterialDetailDialog::FillTableCombo()
{
	int nCount = m_Combo_Table.GetCount();
	for (int i=0;i<nCount;i++)
	{
		m_Combo_Table.DeleteString(0);
	}

	IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
	IEKG3DTextureTable  *pTextureTable = NULL;
	pManager->GetIEKG3DTextureTable(&pTextureTable);

	nCount = pTextureTable->Detail_GetDefineTableCount();
	for (int i=0;i<nCount;i++)
	{
		TCHAR szName[MAX_PATH];
		pTextureTable->Detail_GetDefineTableName(i,szName);
		TCHAR szFullName[MAX_PATH];
		wsprintf(szFullName,"%d 偏色预定义表 %s",i,szName);
		int nIndex = m_Combo_Table.AddString(szFullName);
		m_Combo_Table.SetItemData(nIndex,i);
	}
}

void KModelEditorMaterialDetailDialog::FillListMainColor()
{
	int nCount = m_List_BodyDefine.GetCount();
	for (int i=0;i<nCount;i++)
	{
		m_List_BodyDefine.DeleteString(0);
	}

	IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
	IEKG3DTextureTable  *pTextureTable = NULL;
	pManager->GetIEKG3DTextureTable(&pTextureTable);

	nCount = pTextureTable->Detail_GetBodyDefineCount(m_nCurrentTableIndex);
	for (int i=0;i<nCount;i++)
	{
		TCHAR szFullName[MAX_PATH];
		TCHAR szName[MAX_PATH];
		TCHAR szFileName0[MAX_PATH];
		TCHAR szFileName1[MAX_PATH];
		TCHAR szFlowTexture[MAX_PATH];

		D3DXVECTOR2 vSpeed(m_fFlow_SpeedX,m_fFlow_SpeedY);

		HRESULT hr = pTextureTable->Detail_GetBodyColor(m_nCurrentTableIndex,i,m_vBodyColor,m_vBodyColor1,m_fBody_Max,m_fBodyMin,m_fBody_Scale,m_fBody_Env,m_fBody_Specular,szName,
			m_vDetail0Color,m_fDetail0_ColorScale,m_fDetail0_UVScale,m_fDetail0_Env,m_fDetail0_Specular,szFileName0,
			m_vDetail1Color,m_fDetail1_ColorScale,m_fDetail1_UVScale,m_fDetail1_Env,m_fDetail1_Specular,szFileName1,
			&m_vFlow_Color,&m_fFlow_ColorScale,&m_fFlow_UVScale,&vSpeed,szFlowTexture);

		if(SUCCEEDED(hr))
		{
			wsprintf(szFullName,"%d %s",i,szName);
		}
		else
		{
			wsprintf(szFullName,"%d 未定义 ",i);
		}

		int nIndex = m_List_BodyDefine.AddString(szFullName);
		m_List_BodyDefine.SetItemData(nIndex,i);
	}
	m_nCurrentTableBodyIndex = 0;
}


void KModelEditorMaterialDetailDialog::OnBnClickedButtonDeletetable()
{
	IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
	IEKG3DTextureTable  *pTextureTable = NULL;
	pManager->GetIEKG3DTextureTable(&pTextureTable);

	pTextureTable->Detail_DeleteDefineTable(m_nCurrentTableIndex);
	
	FillTableCombo();
	FillListMainColor();
}

void KModelEditorMaterialDetailDialog::FillListMtlDefine()
{
	HRESULT hr = E_FAIL;

	int nCount = m_List_MtlDefine.GetCount();
	for (int i=0;i<nCount;i++)
	{
		m_List_MtlDefine.DeleteString(0);
	}

	if(m_lpMaterial)
	{
		int nMaxIndex = m_lpMaterial->GetMaxMtlIndex();
        nMaxIndex = max(nMaxIndex + 1, 128);
		for (int i = 0; i < nMaxIndex; i++)
		{
			TCHAR szFullName[MAX_PATH];
			TCHAR szName   [MAX_PATH];
			TCHAR szChannel[MAX_PATH];
			TCHAR szDetail0[MAX_PATH];
			TCHAR szDetail1[MAX_PATH];
			TCHAR szTexture[MAX_PATH];
			TCHAR szFlowTexture[MAX_PATH];

			D3DXVECTOR2 vSpeed(1,1);
			hr = m_lpModel->GetMaterilDetail(i, 0, szName, szTexture, &m_fBody_Env,&m_fBody_Specular,
				&m_fBody_Max,&m_fBodyMin,&m_fBody_Scale,&m_vBodyColor,&m_vBodyColor1,szChannel,
				&m_fDetail0_ColorScale,&m_fDetail0_UVScale,&m_fDetail0_Env,&m_fDetail0_Specular,&m_vDetail0Color,szDetail0,
				&m_fDetail1_ColorScale,&m_fDetail1_UVScale,&m_fDetail1_Env,&m_fDetail1_Specular,&m_vDetail1Color,szDetail1,
				&m_vFlow_Color,&m_fFlow_ColorScale,&m_fFlow_UVScale,&vSpeed,szFlowTexture);

			m_fFlow_SpeedX = vSpeed.x;
			m_fFlow_SpeedY = vSpeed.y;
			m_strFlowTexture = szFlowTexture;

			if(SUCCEEDED(hr))
			{
				wsprintf(szFullName,"%d 私有颜色预定义 %s",i,szName);
			}
			else
			{
				wsprintf(szFullName,"%d 未定义 ",i);
			}

			int nIndex = m_List_MtlDefine.AddString(szFullName);
			m_List_MtlDefine.SetItemData(nIndex,i);
		}
		m_nCurrentMtlDefineIndex = 0;
	}
}
void KModelEditorMaterialDetailDialog::OnBnClickedButtonMainrename()
{
	UpdateData(TRUE);
	Update(TRUE);
	FillListMainColor();
}

void KModelEditorMaterialDetailDialog::OnBnClickedButtonDetailrename()
{
	UpdateData(TRUE);
	Update(TRUE);
}

void KModelEditorMaterialDetailDialog::OnBnClickedButtonMtlrename()
{
	UpdateData(TRUE);
	Update(TRUE);

    m_nPreMtlDefineIndex = -1;
	FillListMtlDefine();
}

void KModelEditorMaterialDetailDialog::OnBnClickedButtonTablerename()
{
	UpdateData(TRUE);

	IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
	IEKG3DTextureTable  *pTextureTable = NULL;
	pManager->GetIEKG3DTextureTable(&pTextureTable);

	TCHAR szName[MAX_PATH];
	wsprintf(szName,"%s",m_szTableName);
	pTextureTable->Detail_SetDefineTableName(m_nCurrentTableIndex,szName);
	FillTableCombo();
}

void KModelEditorMaterialDetailDialog::OnEnChangeEditBodyenv()
{
	Update(TRUE);
}

void KModelEditorMaterialDetailDialog::OnEnChangeEditBodyspecular()
{
	Update(TRUE);
}

void KModelEditorMaterialDetailDialog::OnBnClickedButtonDetail0Clear()
{
	m_szDetailFile0 = "";
	m_Button_Detail0_Texture.SetWindowText(m_szDetailFile0.c_str());

	Update(TRUE);
}

void KModelEditorMaterialDetailDialog::OnBnClickedButton()
{
	m_szDetailFile1 = "";
	m_Button_Detail1_Texture.SetWindowText(m_szDetailFile1.c_str());
	Update(TRUE);
}

void KModelEditorMaterialDetailDialog::OnEnChangeEditBodyenv2()
{
	UpdateData(TRUE);
	Update(TRUE);
}

void KModelEditorMaterialDetailDialog::OnEnChangeEditBodyspecular2()
{
	UpdateData(TRUE);
	Update(TRUE);}

void KModelEditorMaterialDetailDialog::OnEnChangeEditBodyenv3()
{
	UpdateData(TRUE);
	Update(TRUE);
}

void KModelEditorMaterialDetailDialog::OnEnChangeEditBodyspecular3()
{
	UpdateData(TRUE);
	Update(TRUE);
}

void KModelEditorMaterialDetailDialog::OnBnClickedButtonSavetable2()
{
	IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
	IEKG3DTextureTable  *pTextureTable = NULL;
	pManager->GetIEKG3DTextureTable(&pTextureTable);

	pTextureTable->SaveDetailDefines();
}

void KModelEditorMaterialDetailDialog::OnBnClickedButtonCopy()
{
	Copy();
	m_ButtonPaste.EnableWindow(m_bHasCopyData);
}

void KModelEditorMaterialDetailDialog::OnBnClickedButtonPaste()
{
	Paste();
}

void KModelEditorMaterialDetailDialog::Copy()
{
	m_CopyData. m_fBody_Max           = m_fBody_Max;
	m_CopyData. m_fBodyMin            = m_fBodyMin;
	m_CopyData. m_fBody_Scale         = m_fBody_Scale;
	m_CopyData. m_fDetail0_ColorScale = m_fDetail0_ColorScale;
	m_CopyData. m_fDetail0_UVScale    = m_fDetail0_UVScale;
	m_CopyData. m_fDetail1_ColorScale = m_fDetail1_ColorScale;
	m_CopyData. m_fDetail1_UVScale    = m_fDetail1_UVScale;
	m_CopyData. m_vBodyColor          = m_vBodyColor;
	m_CopyData. m_vBodyColor1         = m_vBodyColor1;
	m_CopyData. m_vDetail0Color       = m_vDetail0Color; 
	m_CopyData. m_vDetail1Color		  = m_vDetail1Color;
	m_CopyData. m_szDetailFile0		  = m_szDetailFile0;
	m_CopyData. m_szDetailFile1		  = m_szDetailFile1;
	m_CopyData. m_fBody_Env           = m_fBody_Env;
	m_CopyData. m_fBody_Specular      = m_fBody_Specular;
	m_CopyData. m_fDetail0_Env        = m_fDetail0_Env;
	m_CopyData. m_fDetail0_Specular   = m_fDetail0_Specular;
	m_CopyData. m_fDetail1_Env        = m_fDetail1_Env;
	m_CopyData. m_fDetail1_Specular   = m_fDetail1_Specular;

	m_bHasCopyData = TRUE;
}

void KModelEditorMaterialDetailDialog::Paste()
{
	if(!m_bHasCopyData)
		return ;

	m_fBody_Max           = m_CopyData. m_fBody_Max          ;
	m_fBodyMin            = m_CopyData. m_fBodyMin           ;
	m_fBody_Scale         = m_CopyData. m_fBody_Scale        ;
	m_fDetail0_ColorScale = m_CopyData. m_fDetail0_ColorScale;
	m_fDetail0_UVScale    = m_CopyData. m_fDetail0_UVScale   ;
	m_fDetail1_ColorScale = m_CopyData. m_fDetail1_ColorScale;
	m_fDetail1_UVScale    = m_CopyData. m_fDetail1_UVScale   ;
	m_vBodyColor          = m_CopyData. m_vBodyColor         ;
	m_vBodyColor1         = m_CopyData. m_vBodyColor1        ;
	m_vDetail0Color       = m_CopyData. m_vDetail0Color      ;
	m_vDetail1Color       = m_CopyData. m_vDetail1Color		 ;
	m_szDetailFile0       = m_CopyData. m_szDetailFile0		 ;
	m_szDetailFile1       = m_CopyData. m_szDetailFile1		 ;
	m_fBody_Env           = m_CopyData. m_fBody_Env          ;
	m_fBody_Specular      = m_CopyData. m_fBody_Specular     ;
	m_fDetail0_Env        = m_CopyData. m_fDetail0_Env       ;
	m_fDetail0_Specular   = m_CopyData. m_fDetail0_Specular  ;
	m_fDetail1_Env        = m_CopyData. m_fDetail1_Env       ;
	m_fDetail1_Specular   = m_CopyData. m_fDetail1_Specular  ;

	UpdateData(FALSE);
	Update(TRUE);

}
void KModelEditorMaterialDetailDialog::OnBnClickedButtonSavemtl()
{
	if(m_lpMaterial)
	{
		TCHAR* pszName = NULL;
		m_lpMaterial->GetName((LPCTSTR*)&pszName);
        KG_PROCESS_ERROR(pszName);

        string strOrgTextureFileName = m_lpMaterial->GetOrgTextureFileName();

        int nLength = (int)strlen(pszName);
        if (nLength >= 1 && pszName[0] == '\\')
        {
            for (int i = 1; i < nLength; i++)
            {
                pszName[i - 1] = pszName[i];
            }
            pszName[nLength - 1] = '\0';
        }

        if (m_strCurTextureFileName != strOrgTextureFileName)
        {
            m_lpModel->ChangeTexture(0, 0, strOrgTextureFileName.c_str());
        }
		HRESULT hr = m_lpMaterial->SaveToFile(pszName, 0);
		TCHAR szMessage[MAX_PATH];
		if(SUCCEEDED(hr))
		{
			wsprintf(szMessage,"保存 %s 成功",pszName);
		}
		else
		{
			wsprintf(szMessage,"保存 %s 失败",pszName);
		}

		MessageBox(szMessage,"Save Material",0);
        if (m_strCurTextureFileName != strOrgTextureFileName)
        {
            m_lpModel->ChangeTexture(0, 0, m_strCurTextureFileName.c_str());
        }
	}
Exit0:
    ;
}

HRESULT KModelEditorMaterialDetailDialog::SetComboTableIndex(LPCTSTR strFile)
{
    HRESULT hResult = E_FAIL;

    int nCount = m_Combo_Table.GetCount();
    int nFindIndex = -1;
    TCHAR szLow[MAX_PATH];
    TCHAR szTemp[MAX_PATH];

    strcpy(szLow, strFile);
    strlwr(szLow);

    if (szLow[0] == 'f' || szLow[0] == 'm')
    {
        char szTempPre[MAX_PATH] = {0};
        char szTempAfter[MAX_PATH] = {0};
        sscanf(szLow, "%[^_]_%*[^_]_%s", szTempPre, szTempAfter);
        wsprintf(szTemp,"%s_%s", szTempPre, szTempAfter);
    }
    else
    {
        wsprintf(szTemp, "武器");
    }

    IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
    KG_PROCESS_ERROR(pManager);
    IEKG3DTextureTable  *pTextureTable = NULL;
    pManager->GetIEKG3DTextureTable(&pTextureTable);
    KG_PROCESS_ERROR(pTextureTable);

    for (int i = 0; i < nCount; i++)
    {
        TCHAR szName[MAX_PATH];
        pTextureTable->Detail_GetDefineTableName(i,szName);
        strcpy(szLow, szName);
        strlwr(szLow);
        if (!strcmp(szLow, szTemp))
        {
            nFindIndex = i;
            break;
        }
    }

    m_Combo_Table.SetCurSel(nFindIndex);
    OnCbnSelchangeComboTableindex();

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KModelEditorMaterialDetailDialog::ColorUpdate(DWORD dwColor, HWND hWnd)
{
    HRESULT hResult = E_FAIL;
    D3DCOLORVALUE d3dColorValue = ::Color2Value(dwColor);
    if (hWnd == m_Button_Color[0].GetSafeHwnd())
    {
        m_vBodyColor = d3dColorValue;
    }
    else if (hWnd == m_Button_Color[1].GetSafeHwnd())
    {
        m_vBodyColor1 = d3dColorValue;
    }
    else if (hWnd == m_Button_Color[2].GetSafeHwnd())
    {
        m_vDetail0Color = d3dColorValue;
    }
    else if (hWnd == m_Button_Color[3].GetSafeHwnd())
    {
        m_vDetail1Color = d3dColorValue;
    }
	else if(hWnd == m_Button_Color[4].GetSafeHwnd())
	{
		m_vFlow_Color = d3dColorValue;
	}

    hResult = S_OK;
    return hResult;
}

void KModelEditorMaterialDetailDialog::SetButtonColor()
{
    DWORD dwColor;
    dwColor = Value2Color(m_vBodyColor);
    m_Button_Color[ENUM_BUTTON_BODY1].SetBKColor(dwColor);
    dwColor = Value2Color(m_vBodyColor1);
    m_Button_Color[ENUM_BUTTON_BODY2].SetBKColor(dwColor);
    dwColor = Value2Color(m_vDetail0Color);
    m_Button_Color[ENUM_BUTTON_DETAIL1].SetBKColor(dwColor);
    dwColor = Value2Color(m_vDetail1Color);
    m_Button_Color[ENUM_BUTTON_DETAIL2].SetBKColor(dwColor);
	dwColor = Value2Color(m_vFlow_Color);
	m_Button_Color[ENUM_BUTTON_FLOW].SetBKColor(dwColor);

    HWND phWndTmp = GetColorPickerDialog()->GetHwnd();
    if (phWndTmp)
    {
        for (int i = 0; i < ENUM_BUTTON_COUNT; i++)
        {
            if (m_Button_Color[i].GetSafeHwnd() == phWndTmp)
            {
                m_Button_Color[i].OnBnClicked();
            }
        }
    }
}

void KModelEditorMaterialDetailDialog::OnBnClickedButtonChangetexture()
{
    HRESULT hr = E_FAIL;
    TCHAR strFilter[] = "Texture Files|*.swf;*.bmp;*.jpg;*.png;*.tga;*.Spr;*.DDS;*.avi;*.rmvb;*.wmv|";
    CString fileName;
    CFileDialog DlgLoad(TRUE, NULL, NULL, OFN_HIDEREADONLY , strFilter);
    IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
    CString cstrFileName;
    HRESULT hRetCode = E_FAIL;
    TCHAR szDir[MAX_PATH] = {0};

    ::GetCurrentDirectory(MAX_PATH, szDir);
    KG_PROCESS_ERROR(DlgLoad.DoModal() == IDOK);
    
    cstrFileName = DlgLoad.GetPathName();
    int nLength = cstrFileName.GetLength();
    LPCSTR pszFileName = cstrFileName.GetBuffer(nLength);

    hRetCode = m_lpModel->ChangeTexture(0, 0, pszFileName);
    KG_COM_PROCESS_ERROR(hRetCode);

    m_strTextureFileName = cstrFileName;
    m_strCurTextureFileName = cstrFileName;
    Update(TRUE);

    ::SetCurrentDirectory(szDir);
Exit0:
    return;
}

void KModelEditorMaterialDetailDialog::OnBnClickedCheckWhole()
{
    m_bColorCastWhole = m_Button_ColorCastWhole.GetCheck();
    m_lpMaterial->SetDetailWholeColorCast(m_bColorCastWhole);
    if (!m_bColorCastWhole)
    {
        m_Combo_SubsetList.EnableWindow(TRUE);
        OnCbnSelchangeComboSubset();
    }
    else
    {
        m_Combo_SubsetList.EnableWindow(FALSE);

        m_nCurSubSetIndex = 0;
        OnLbnSelchangeListMtldefine();
    }
}

void KModelEditorMaterialDetailDialog::OnCbnSelchangeComboSubset()
{
    int nIndex = m_Combo_SubsetList.GetCurSel();
    KG_PROCESS_ERROR(nIndex > -1);

    m_nCurSubSetIndex = nIndex;
    OnLbnSelchangeListMtldefine();
Exit0:
    ;
}

HRESULT KModelEditorMaterialDetailDialog::FillComboSubSet()
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;
    int nCount = 0;
    int nSize  = 0;
    TCHAR  szSubSetName[MAX_PATH];

    KG_PROCESS_ERROR(m_lpModel);

    nSize = m_Combo_SubsetList.GetCount();
    for (int i = 0; i < nSize; i++)
        m_Combo_SubsetList.DeleteString(0);

    m_Button_ColorCastWhole.SetCheck(TRUE);
    OnBnClickedCheckWhole();

    m_Button_ColorCastWhole.EnableWindow(FALSE);
    KG_PROCESS_ERROR(m_lpMaterial);

    m_lpModel->GetNumMaterial(&nCount);   
    if (nCount > 1)
        m_Button_ColorCastWhole.EnableWindow(TRUE);

    for (DWORD i = 0; i < (DWORD)nCount; i++)
    {
        DWORD dwOption = 0;
        hRetCode = m_lpModel->GetMaterialSubsetOption(i, &dwOption);
        KG_COM_PROCESS_ERROR(hRetCode);

        if (dwOption & MATERIAL_OPTION_HIDE)
            sprintf(szSubSetName, "SubSet %d (隐藏)", i);
        else
            sprintf(szSubSetName, "SubSet %d", i);

        m_Combo_SubsetList.InsertString(i, szSubSetName);
        if (i == 0)
            m_Combo_SubsetList.SetCurSel(0);
    }

    hResult = S_OK;
Exit0:
    return hResult;
}
void KModelEditorMaterialDetailDialog::OnEnChangeEditFlowColorscale()
{
	Update(TRUE);
}

void KModelEditorMaterialDetailDialog::OnEnChangeEditFlowUvscale()
{
	Update(TRUE);
}

void KModelEditorMaterialDetailDialog::OnEnChangeEditFlowSpeed()
{
	Update(TRUE);
}

void KModelEditorMaterialDetailDialog::OnBnClickedButtonFlowTexture()
{
	HRESULT hr = E_FAIL;
	TCHAR strFilter[] = "Texture Files |*.bmp;*.jpg;*.png;*.tga;*.Spr;*.DDS;";
	CString fileName;
	CFileDialog DlgLoad(TRUE, ".dds", "test.dds", OFN_HIDEREADONLY , strFilter);
	IEKG3DEngineManager *pManager = g_GetEngineIEInterface();

	TCHAR szAppDirectory[MAX_PATH] = { 0 };
	pManager->GetDefWorkDirectory(szAppDirectory, MAX_PATH);

	TCHAR szDir[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH,szDir);

	TCHAR RetFileName[MAX_PATH];

	if(DlgLoad.DoModal() == IDOK)
	{
		fileName = DlgLoad.GetPathName();

		HRESULT hr = pManager->GetRelativePath(szAppDirectory,fileName,RetFileName);
		if(SUCCEEDED(hr))
		{
			m_strFlowTexture = RetFileName;
			m_Button_FlowTexture.SetWindowText(RetFileName);
			Update(TRUE);
		}
	}
	::SetCurrentDirectory(szDir);
}

void KModelEditorMaterialDetailDialog::OnBnClickedButtonFlowTextureclear()
{
	m_strFlowTexture = "";
	m_Button_FlowTexture.SetWindowText(m_strFlowTexture.c_str());
	Update(TRUE);
}

void KModelEditorMaterialDetailDialog::OnBnClickedButtonDetail1Color()
{
	// TODO: Add your control notification handler code here
}

void KModelEditorMaterialDetailDialog::OnBnClickedButtonFlowColor()
{
	// TODO: Add your control notification handler code here
}

void KModelEditorMaterialDetailDialog::OnEnChangeEditFlowSpeedy()
{
	Update(TRUE);
}
