#include "stdafx.h"
#include "KSceneSceneEditorDialogPaintTexture.h"

#include "IEEditor.h"
#include "IEKG3DTerrain.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(KSceneSceneEditorDialogPaintTexture, CDialog)

BEGIN_MESSAGE_MAP(KSceneSceneEditorDialogPaintTexture, CDialog)
	ON_BN_CLICKED(IDC_RADIO4, OnSetOperatorLayter1)
	ON_BN_CLICKED(IDC_RADIO3, OnSetOperatorLayter2)
	ON_BN_CLICKED(IDC_RADIO2, OnSetOperatorLayter3)
	ON_BN_CLICKED(IDC_RADIO1, OnSetOperatorLayter4)

	ON_BN_CLICKED(IDC_CHECK6, OnSetVisibleLayter1)
	ON_BN_CLICKED(IDC_CHECK5, OnSetVisibleLayter2)
	ON_BN_CLICKED(IDC_CHECK4, OnSetVisibleLayter3)
	ON_BN_CLICKED(IDC_CHECK3, OnSetVisibleLayter4)

	ON_BN_CLICKED(IDC_CHECK2, OnSetIsDrawRegionEdge)
	ON_BN_CLICKED(IDC_CHECK1,OnSetIsSelectAllRegion)

	ON_BN_CLICKED(IDC_BUTTON1,OnSetSelectBrushMap)
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(NM_THEMECHANGED, IDC_SLIDER2, &KSceneSceneEditorDialogPaintTexture::OnNMThemeChangedSlider2)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

BOOL KSceneSceneEditorDialogPaintTexture::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bnVLayter1.SetCheck(m_bLayerIsVisible[0]);
	m_bnVLayter2.SetCheck(m_bLayerIsVisible[1]);
	m_bnVLayter3.SetCheck(m_bLayerIsVisible[2]);
	m_bnVLayter4.SetCheck(m_bLayerIsVisible[3]);

	m_IsSelectAllRegion.SetCheck(m_bIsSelectAllRegion);
	m_IsDrawRegionEdge.SetCheck(m_bIsDrawRegionEdge);

	/*RECT rect;
	m_Slider.Create(TBS_HORZ | TBS_AUTOTICKS | TBS_BOTTOM,)*/
	m_Slider.SetRange(-180,180);
	m_Slider.SetTicFreq(1);
	m_Slider.SetPos(0);

	// TODO:  Add extra initialization here

	//////////////////////////////////////////////////////////////////////////

	m_szStatic.Format("Brush Map旋转角度 : %d", 0);
//	m_szStatic += "chi sui";

	UpdateData(FALSE);



	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

KSceneSceneEditorDialogPaintTexture::KSceneSceneEditorDialogPaintTexture(CWnd* pParent) : CDialog(KSceneSceneEditorDialogPaintTexture::IDD, pParent)
, m_nOLayter1(0)
, m_szStatic(_T(""))
{
	m_lpCurrentTerrainEx = NULL;
	m_lpSceneEditor = NULL;
	m_nOperatorLayer = 1;
	m_bLayerIsVisible[0] = true;
	m_bLayerIsVisible[1] = m_bLayerIsVisible[2] = m_bLayerIsVisible[3] = true;
	m_bIsSelectAllRegion = true;
	m_bIsDrawRegionEdge = false;
	m_nRotationAngle = 0;
}
KSceneSceneEditorDialogPaintTexture::~KSceneSceneEditorDialogPaintTexture()
{

}

void KSceneSceneEditorDialogPaintTexture::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_CHECK6, m_bnVLayter1);
	DDX_Control(pDX, IDC_CHECK5, m_bnVLayter2);
	DDX_Control(pDX, IDC_CHECK4, m_bnVLayter3);
	DDX_Control(pDX, IDC_CHECK3, m_bnVLayter4);

	DDX_Control(pDX, IDC_SLIDER2, m_Slider);
//	DDX_Slider(pDX, IDC_SLIDERSZIE, m_nRotationAngle);
	/*m_nRotationAngle = m_Slider.GetPos();
	m_szStatic.Format("Brush Map旋转角度 : %d", m_nRotationAngle);*/

	DDX_Text(pDX, IDC_RADIO4, m_nOperatorLayer);
	DDX_Control(pDX, IDC_CHECK1, m_IsSelectAllRegion);
	DDX_Control(pDX, IDC_CHECK2, m_IsDrawRegionEdge);
	DDX_Text(pDX, IDC_STATIC123, m_szStatic);
}

inline void KSceneSceneEditorDialogPaintTexture::OnSetOperatorLayter1()
{
	m_nOperatorLayer = 0;
	m_lpCurrentTerrainEx->SetTextureOpertaorLayter(m_nOperatorLayer);
}

inline void KSceneSceneEditorDialogPaintTexture::OnSetOperatorLayter2()
{
	m_nOperatorLayer = 1;
	m_lpCurrentTerrainEx->SetTextureOpertaorLayter(m_nOperatorLayer);
}


inline void KSceneSceneEditorDialogPaintTexture::OnSetOperatorLayter3()
{
	m_nOperatorLayer = 2;
	m_lpCurrentTerrainEx->SetTextureOpertaorLayter(m_nOperatorLayer);
}

inline void KSceneSceneEditorDialogPaintTexture::OnSetOperatorLayter4()
{
	m_nOperatorLayer = 3;
	m_lpCurrentTerrainEx->SetTextureOpertaorLayter(m_nOperatorLayer);
}

inline void KSceneSceneEditorDialogPaintTexture::OnSetVisibleLayter1()
{
	if(m_bLayerIsVisible[0])
		m_bLayerIsVisible[0] = false;
	else
		m_bLayerIsVisible[0] = true;

	m_lpCurrentTerrainEx->SetVisibleLayter(m_bLayerIsVisible);
}

inline void KSceneSceneEditorDialogPaintTexture::OnSetVisibleLayter2()
{
	if(m_bLayerIsVisible[1])
		m_bLayerIsVisible[1] = false;
	else
		m_bLayerIsVisible[1] = true;

	m_lpCurrentTerrainEx->SetVisibleLayter(m_bLayerIsVisible);
}

inline void KSceneSceneEditorDialogPaintTexture::OnSetVisibleLayter3()
{
	if(m_bLayerIsVisible[2])
		m_bLayerIsVisible[2] = false;
	else
		m_bLayerIsVisible[2] = true;

	m_lpCurrentTerrainEx->SetVisibleLayter(m_bLayerIsVisible);
}
inline void KSceneSceneEditorDialogPaintTexture::OnSetVisibleLayter4()
{
	if(m_bLayerIsVisible[3])
		m_bLayerIsVisible[3] = false;
	else
		m_bLayerIsVisible[3] = true;

	m_lpCurrentTerrainEx->SetVisibleLayter(m_bLayerIsVisible);
}

void KSceneSceneEditorDialogPaintTexture::SetDataToTerrainEx()
{
	m_lpCurrentTerrainEx->SetVisibleLayter(m_bLayerIsVisible);
	m_lpCurrentTerrainEx->SetTextureOpertaorLayter(m_nOperatorLayer);
	m_lpCurrentTerrainEx->SetIsSelectAllRegion(m_bIsSelectAllRegion);
	m_lpCurrentTerrainEx->SetIsDrawRegionEdge(m_bIsDrawRegionEdge);
}

void KSceneSceneEditorDialogPaintTexture::OnSetIsSelectAllRegion()
{
	if(m_bIsSelectAllRegion)
		m_bIsSelectAllRegion = false;
	else
		m_bIsSelectAllRegion = true;

	m_lpCurrentTerrainEx->SetIsSelectAllRegion(m_bIsSelectAllRegion);
}

void KSceneSceneEditorDialogPaintTexture::OnSetIsDrawRegionEdge()
{
	if(m_bIsDrawRegionEdge)
		m_bIsDrawRegionEdge = false;
	else
		m_bIsDrawRegionEdge = true;

	m_lpCurrentTerrainEx->SetIsDrawRegionEdge(m_bIsDrawRegionEdge);
}

void KSceneSceneEditorDialogPaintTexture::OnSetSelectBrushMap()
{
	char bigBuff[800] = "";
	char szFilter[] ="Brush Map (*.tga)|*.tga|";
	TCHAR FileDir[256];

	CFileDialog dlg(false, "tga", NULL,
		OFN_HIDEREADONLY , szFilter);

	dlg.m_ofn.lpstrFile = bigBuff;
	dlg.m_ofn.nMaxFile = sizeof(bigBuff);

	if (	dlg.DoModal() == IDOK)
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

		//m_lpCurrentTerrainEx->m_Brush.ChangeBrushMap(FileDir,*m_lpCurrentTerrainEx);
		m_lpCurrentTerrainEx->SetCurrentBrushMap(FileDir);

		UpdateData(false);
	}
}
void KSceneSceneEditorDialogPaintTexture::OnNMThemeChangedSlider2(NMHDR *pNMHDR, LRESULT *pResult)
{
	// This feature requires Windows XP or greater.
	// The symbol _WIN32_WINNT must be >= 0x0501.
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void KSceneSceneEditorDialogPaintTexture::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

	if (pScrollBar != (CWnd*)&m_Slider)
		return;

	if(nSBCode == SB_ENDSCROLL)
	{
		m_szStatic.Format("Brush Map旋转角度 : %d", m_nRotationAngle);
		m_lpCurrentTerrainEx->RotationBrushMap(m_nRotationAngle);
		return;
	}

//	m_nRotationAngle = m_Slider.GetPos();
	m_nRotationAngle = nPos;
	m_szStatic.Format("Brush Map旋转角度 : %d", nPos);
	m_nRotationAngle = nPos;
	UpdateData(false);

}
