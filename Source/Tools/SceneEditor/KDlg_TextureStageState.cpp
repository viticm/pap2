// KDlg_TextureStageState.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KDlg_TextureStageState.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern LPDIRECT3DDEVICE9 g_pd3dDevice;

// KDlg_TextureStageState dialog


KDlg_TextureStageState& g_GetTextureStageStateDlg()
{
	static KDlg_TextureStageState Dlg;
	return Dlg;
}

IMPLEMENT_DYNAMIC(KDlg_TextureStageState, CDialog)

KDlg_TextureStageState::KDlg_TextureStageState(CWnd* pParent /*=NULL*/)
	: CDialog(KDlg_TextureStageState::IDD, pParent)
{

}

void KDlg_TextureStageState::Init(DWORD *pdwTextureIDs, CWnd* pParent)
{
	if (pdwTextureIDs)
	{
		m_pdwTextureIDs = pdwTextureIDs;
		Create(IDD_DIALOG_TEXTURESTAGE, pParent);
		ShowWindow(SW_SHOW);
	}
	else
	{
		MessageBox("Invalid initialized data.");
		delete this;
	}
}

KDlg_TextureStageState::~KDlg_TextureStageState()
{
}

void KDlg_TextureStageState::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_COLOROP, m_ctlColorOp);
	DDX_Control(pDX, IDC_COMBO_COLORARG1, m_ctlColorArg1);
	DDX_Control(pDX, IDC_COMBO_COLORARG2, m_ctlColorArg2);
	DDX_Control(pDX, IDC_COMBO_ALPHAOP, m_ctlAlphaOp);
	DDX_Control(pDX, IDC_COMBO_ALPHAARG1, m_ctlAlphaArg1);
	DDX_Control(pDX, IDC_COMBO_ALPHAARG2, m_ctlAlphaArg2);
	DDX_Control(pDX, IDC_COMBO_STAGE, m_ctlStage);
}


BEGIN_MESSAGE_MAP(KDlg_TextureStageState, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_RESTORE, &KDlg_TextureStageState::OnBnClickedButtonRestore)
END_MESSAGE_MAP()


// KDlg_TextureStageState message handlers

void KDlg_TextureStageState::OnBnClickedButtonRestore()
{

}

BOOL KDlg_TextureStageState::OnInitDialog()
{
	CDialog::OnInitDialog();

    ASSERT(m_pdwTextureIDs);
	//prepare restore data
	for (int i = 0; i < 8; i++)
	{
		if (m_pdwTextureIDs[i] == 0xffffffff)
		{
			m_nNumTexture = i;
			break;
		}

		g_pd3dDevice->GetTextureStageState(i, D3DTSS_ALPHAOP, &(m_RestoreData[i].AlphaOp));
		g_pd3dDevice->GetTextureStageState(i, D3DTSS_ALPHAARG1, &(m_RestoreData[i].AlphaArg1));
		g_pd3dDevice->GetTextureStageState(i, D3DTSS_ALPHAARG2, &(m_RestoreData[i].AlphaArg2));

		g_pd3dDevice->GetTextureStageState(i, D3DTSS_COLOROP, &(m_RestoreData[i].ColorOp));
		g_pd3dDevice->GetTextureStageState(i, D3DTSS_COLORARG1, &(m_RestoreData[i].ColorArg1));
		g_pd3dDevice->GetTextureStageState(i, D3DTSS_COLORARG2, &(m_RestoreData[i].ColorArg2));
	}
	//prepare restore data end




	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void KDlg_TextureStageState::FillLists()
{
	TCHAR strInfo[256];

	m_ctlStage.ResetContent();
	for (int i = 0; i < m_nNumTexture; i++)
	{
		sprintf(strInfo, "Texture Stage %d", i);
		m_ctlStage.AddString(strInfo);
	}


	for (int i = 0; i < sizeof(Ops); i++)
	{
		m_ctlAlphaOp.AddString(Ops[i].strDesc);
		m_ctlColorOp.AddString(Ops[i].strDesc);
	}

	for (int i = 0; i < sizeof(Args); i++)
	{
		m_ctlAlphaArg1.AddString(Args[i].strDesc);
		m_ctlAlphaArg2.AddString(Args[i].strDesc);

		m_ctlColorArg1.AddString(Args[i].strDesc);
		m_ctlColorArg2.AddString(Args[i].strDesc);
	}
}

