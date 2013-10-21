// KDlg_Lightning.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
//#include "KLightning.h"
#include "KDlg_Lightning.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

// KDlg_Lightning dialog

IMPLEMENT_DYNAMIC(KDlg_Lightning, CPropertyPage)
KDlg_Lightning::KDlg_Lightning()
	: CPropertyPage(KDlg_Lightning::IDD)
{
	m_pOldTex = 0;
	m_pNewTex = 0;
}

KDlg_Lightning::~KDlg_Lightning()
{
}

void KDlg_Lightning::Create()
{
	CPropertyPage::Create(IDD);
}

//void KDlg_Lightning::SetLightning(KLightning* pL)
//{
//	m_pL = pL;
//
//	// Backup the parameters
//	m_Length = pL->m_LengthOfNode;
//	m_Width = pL->m_WidthOfNode;
//	m_FrameTime = pL->m_FrameInterval;
//	m_TFactorAlpha = pL->m_Alpha;
//	m_TexName = pL->m_TexName;
//	m_pOldTex = pL->m_pTexture;
//
//	// Display
//	TCHAR strLen[255];
//	sprintf(strLen, "%f", m_Length);
//	SetDlgItemText(IDD_LIGHTNING_LENGTH, strLen);
//	//sTemp = m_Width;
//	sprintf(strLen, "%f", m_Width);
//	SetDlgItemText(IDD_LIGHTNING_WIDTH, strLen);
//	sprintf(strLen, "%u", m_FrameTime);
//	SetDlgItemText(IDD_LIGHTNING_FRAMETIME, strLen);
//	sprintf(strLen, "%d", (int)m_TFactorAlpha);
//	SetDlgItemText(IDD_LIGHTNING_TFACTOR, strLen);
//	SetDlgItemText(IDD_LIGHTNING_TEXTURE, m_TexName.c_str());
//}

void KDlg_Lightning::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(KDlg_Lightning, CPropertyPage)
	ON_EN_CHANGE(IDD_LIGHTNING_LENGTH, OnLengthChanged)
	ON_EN_CHANGE(IDD_LIGHTNING_WIDTH, OnWidthChanged)
	ON_EN_CHANGE(IDD_LIGHTNING_FRAMETIME, OnFrametimeChanged)
	ON_EN_CHANGE(IDD_LIGHTNING_TFACTOR, OnTfactorChanged)
	ON_EN_CHANGE(IDD_LIGHTNING_TEXTURE, OnTextureChanged)
	ON_BN_CLICKED(IDD_LIGHTNING_LOAD, OnLoadClicked)
	ON_BN_CLICKED(IDCANCEL, OnCancelClicked)
	ON_BN_CLICKED(IDD_LIGHTNING_RESET, OnResetClicked)
	ON_BN_CLICKED(IDD_LIGHTNING_SAVE, OnSaveClicked)
	ON_BN_CLICKED(IDOK, OnOkClicked)
END_MESSAGE_MAP()


// KDlg_Lightning message handlers

void KDlg_Lightning::OnLengthChanged()
{
	TCHAR sz[256];
	GetDlgItemText(IDD_LIGHTNING_LENGTH, sz, 256);
	if(sz[0] == '\0')
	{
		MessageBox("请键入一个数值", "警告");
		return;
	}

	TCHAR* pEnd = NULL;
	//m_pL->m_LengthOfNode = static_cast<float>(strtod(sz, &pEnd));
}

void KDlg_Lightning::OnWidthChanged()
{
	TCHAR sz[256];
	GetDlgItemText(IDD_LIGHTNING_WIDTH, sz, 256);
	if(sz[0] == '\0')
	{
		MessageBox("请键入一个数值", "警告");
		return;
	}

	//string s = sz;
	//m_pL->m_WidthOfNode = s.FloatValue();
	TCHAR* pEnd = NULL;
	//m_pL->m_WidthOfNode = static_cast<float>(strtod(sz, &pEnd));
}

void KDlg_Lightning::OnFrametimeChanged()
{
	TCHAR sz[256];
	GetDlgItemText(IDD_LIGHTNING_FRAMETIME, sz, 256);
	if(sz[0] == '\0')
	{
		MessageBox("请键入一个数值", "警告");
		return;
	}

	//string s = sz;
	TCHAR* pEnd = NULL;
	//m_pL->m_FrameInterval = static_cast<int>(strtod(sz, &pEnd));
}

void KDlg_Lightning::OnTfactorChanged()
{
	TCHAR sz[256];
	GetDlgItemText(IDD_LIGHTNING_TFACTOR, sz, 256);
	if(sz[0] == '\0')
	{
		MessageBox("请键入一个数值", "警告");
		return;
	}

	//string s = sz;
	//m_pL->m_Alpha = s.IntValue();
	TCHAR* pEnd = NULL;
	//m_pL->m_Alpha = static_cast<UCHAR>(strtod(sz, &pEnd));
}

void KDlg_Lightning::OnTextureChanged()
{
	TCHAR sz[256];
	GetDlgItemText(IDD_LIGHTNING_TEXTURE, sz, 256);
	if(sz[0] == '\0')
	{
		return;
	}

	string sTexName = sz;

	//if(m_TexFile.size())
	//{
	//	SAFE_RELEASE(m_pNewTex);
	//	if(FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, m_TexFile.c_str(), &m_pNewTex)))
	//	{
	//		return;
	//	}

	//	//m_pL->m_TexName = sTexName;
	//	//m_pL->m_pTexture = m_pNewTex;
	//}
}

void KDlg_Lightning::OnLoadClicked()
{
	CFileDialog dlg(true, "jpg", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "JPG贴图文件(*.jpg)|*.jpg||");
	if(dlg.DoModal() != IDOK)
	{
		return;
	}

	//string sAppDirectory = g_Def_WorkDirectory;
	//string sTexPath = dlg.GetPathName();
	//string sTexFilename = sTexPath.substr(sAppDirectory.size() + 1);
	//string sTexFile = dlg.GetFileName();
	//m_TexFile = sTexFile;

	//SetDlgItemText(IDD_LIGHTNING_TEXTURE, sTexFilename.c_str());
}

void KDlg_Lightning::ResetParameters()
{
	//m_pL->m_LengthOfNode = m_Length;
	//m_pL->m_WidthOfNode = m_Width;
	//m_pL->m_FrameInterval = m_FrameTime;
	//m_pL->m_TexName = m_TexName;
	//m_pL->m_pTexture = m_pOldTex;
	//if(m_pNewTex != m_pOldTex)
	//{
	//	SAFE_RELEASE(m_pNewTex);
	//}
}

void KDlg_Lightning::OnCancelClicked()
{
	ResetParameters();
	OnClose();
}

void KDlg_Lightning::OnResetClicked()
{
	ResetParameters();
	ShowWindow(false);
	//SetLightning(m_pL);
	ShowWindow(true);
}

void KDlg_Lightning::OnSaveClicked()
{
	//m_pL->SaveMesh(0);
}

void KDlg_Lightning::OnOkClicked()
{
	OnClose();
}

void KDlg_Lightning::OnClose()
{
	CPropertyPage::OnClose();
	ShowWindow(false);
}