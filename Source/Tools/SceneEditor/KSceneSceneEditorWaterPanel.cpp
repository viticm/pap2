// KSceneSceneEditorWaterPanel.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSceneEditorWaterPanel.h"
#include "KColorDialog.h"
#include "windowsx.h"
#include "IEKG3DTerrain.h"

IMPLEMENT_DYNAMIC(CColorWnd, CWnd)

CColorWnd::CColorWnd()
    : m_dwColor(0)
{

}

BEGIN_MESSAGE_MAP(CColorWnd, CWnd)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_MESSAGE(WM_COLORPOINTER_CHANGE, OnColorChange)
END_MESSAGE_MAP()

void CColorWnd::SetColor(DWORD dwColor)
{
    m_dwColor = RGB((dwColor >> 16) & 0xff, (dwColor >> 8) & 0xff, dwColor & 0xff);
    InvalidateRect(NULL, TRUE);
}

void CColorWnd::OnPaint()
{
    CPaintDC dc(this);
    CBrush bkBrush = CBrush(m_dwColor);
    RECT rectClient;

    GetClientRect(&rectClient);
    dc.FillRect(&rectClient, CBrush::FromHandle(GetStockBrush(GRAY_BRUSH)));
    rectClient.left += 2;
    rectClient.top += 2;
    rectClient.right -= 2;
    rectClient.bottom -= 2;
    dc.FillRect(&rectClient, &bkBrush);

}

BOOL CColorWnd::OnEraseBkgnd(CDC* pDC)
{
    return TRUE;
}


LRESULT CColorWnd::OnColorChange(WPARAM wParam, LPARAM lParam)
{
    SetColor(static_cast<DWORD>(wParam));
    return 1;
}

IMPLEMENT_DYNAMIC(KSceneSceneEditorWaterPanel, KGLilisSlipPanel<CDialog>)

KSceneSceneEditorWaterPanel::KSceneSceneEditorWaterPanel(CWnd* pParent)
	: KGLilisSlipPanel<CDialog>(KSceneSceneEditorWaterPanel::IDD, pParent),
      m_nHeight(0),
      m_fFogStart(0.f),
      m_fFogEnd(0.f)
{

}

KSceneSceneEditorWaterPanel::~KSceneSceneEditorWaterPanel()
{
}

void KSceneSceneEditorWaterPanel::DoDataExchange(CDataExchange* pDX)
{
	KGLilisSlipPanel<CDialog>::DoDataExchange(pDX);

    DDX_Text(pDX, IDC_EDIT_SURFACE, m_strSurfaceTextureName);
    DDX_Text(pDX, IDC_EDIT_BUMP, m_strBumpTextureName);
    DDX_Text(pDX, IDC_EDIT_FOG_START, m_fFogStart);
    DDX_Text(pDX, IDC_EDIT_FOG_END, m_fFogEnd);
    DDX_Control(pDX, IDC_WATER_DETAIL, m_ComboDetail);
    DDX_Control(pDX, IDC_COLOR_DIF, m_ColorWnd1);
    DDX_Control(pDX, IDC_COLOR_SPE, m_ColorWnd2);
    DDX_Control(pDX, IDC_COLOR_FOG, m_ColorWnd3);
    DDX_Text(pDX, IDC_EDIT_HEIGHT, m_nHeight);
}


BEGIN_MESSAGE_MAP(KSceneSceneEditorWaterPanel, KGLilisSlipPanel<CDialog>)
    ON_WM_SIZE()
    ON_BN_CLICKED(IDC_BUTTON_SURFACE, &KSceneSceneEditorWaterPanel::OnBnClickedButtonSurface)
    ON_BN_CLICKED(IDC_BUTTON_BUMP, &KSceneSceneEditorWaterPanel::OnBnClickedButtonBump)
    ON_STN_CLICKED(IDC_COLOR_DIF, &KSceneSceneEditorWaterPanel::OnStnClickedColorDif)
    ON_STN_CLICKED(IDC_COLOR_SPE, &KSceneSceneEditorWaterPanel::OnStnClickedColorSpe)
    ON_STN_CLICKED(IDC_COLOR_FOG, &KSceneSceneEditorWaterPanel::OnStnClickedColorFog)
    ON_CBN_SELCHANGE(IDC_WATER_DETAIL, &KSceneSceneEditorWaterPanel::OnCbnSelchangeWaterDetail)
    ON_WM_CTLCOLOR()
    ON_WM_HSCROLL()
    ON_BN_CLICKED(IDC_BUTTON_RELOAD_SHADER, &KSceneSceneEditorWaterPanel::OnBnClickedButtonReloadShader)
    ON_BN_CLICKED(IDC_CHECK_HIDE, &KSceneSceneEditorWaterPanel::OnBnClickedCheckHide)
    ON_BN_CLICKED(IDC_CHECK_EDGE, &KSceneSceneEditorWaterPanel::OnBnClickedCheckEdge)
    ON_BN_CLICKED(IDC_CHECK_SET, &KSceneSceneEditorWaterPanel::OnBnClickedCheckSet)
    ON_BN_CLICKED(IDC_CHECK_GET, &KSceneSceneEditorWaterPanel::OnBnClickedCheckGet)
    ON_EN_CHANGE(IDC_EDIT_HEIGHT, &KSceneSceneEditorWaterPanel::OnEnChangeEditHeight)
    ON_WM_TIMER()
    ON_EN_CHANGE(IDC_EDIT_FOT_START, &KSceneSceneEditorWaterPanel::OnEnChangeEditFotStart)
    ON_EN_CHANGE(IDC_EDIT_FOG_END, &KSceneSceneEditorWaterPanel::OnEnChangeEditFogEnd)
    ON_BN_CLICKED(IDC_BUTTON_DEF_HEIGHT, &KSceneSceneEditorWaterPanel::OnBnClickedButtonDefHeight)
END_MESSAGE_MAP()

void KSceneSceneEditorWaterPanel::OnSize(UINT nType, int cx, int cy)
{
    SetOffset(10, max(0, cx - 185));
    UpdateUI();

    return KGLilisSlipPanel<CDialog>::OnSize(nType, cx, cy);
}

HBRUSH KSceneSceneEditorWaterPanel::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    return KGLilisSlipPanel<CDialog>::OnCtlColor(pDC, pWnd, nCtlColor);
}

void KSceneSceneEditorWaterPanel::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    if (m_pWaterEffect && (nSBCode == SB_PAGELEFT || nSBCode == SB_PAGERIGHT || nSBCode == SB_THUMBTRACK || nSBCode == SB_THUMBPOSITION))
    {
        if (pScrollBar == GetDlgItem(IDC_SLIDER_UVS))
        {
            m_pWaterEffect->GetTextParamPtr()->x = static_cast<float>(nPos + 1) / 100000.f;
        }
        else if (pScrollBar == GetDlgItem(IDC_SLIDER_UVB))
        {
            m_pWaterEffect->GetTextParamPtr()->y = static_cast<float>(nPos + 1) / 100000.f;
        } 
        else if (pScrollBar == GetDlgItem(IDC_SLIDER_SPEED_U))
        {
            m_pWaterEffect->GetWaveParamPtr()->z = static_cast<float>(nPos) / 100.f;
        }
        else if (pScrollBar == GetDlgItem(IDC_SLIDER_SPEED_V))
        {
            m_pWaterEffect->GetWaveParamPtr()->w = static_cast<float>(nPos) / 100.f;
        }
        else if (pScrollBar == GetDlgItem(IDC_SLIDER_REFR_K))
        {
            m_pWaterEffect->GetTextParamPtr()->w = static_cast<float>(nPos) * 100.f;
        }
        else if (pScrollBar == GetDlgItem(IDC_SLIDER_FRE))
        {
            m_pWaterEffect->GetWaveParamPtr()->y = static_cast<float>(nPos) / 10.f;
        }
        else if (pScrollBar == GetDlgItem(IDC_SLIDER_WAVE))
        {
            m_pWaterEffect->GetWaveParamPtr()->x = static_cast<float>(nPos) / 1000.f;
        }
        else if (pScrollBar == GetDlgItem(IDC_SLIDER_WAVE2))
        {
            m_pWaterEffect->GetTextParamPtr()->z = static_cast<float>(nPos) / 1000.f;
        }
    }

    return KGLilisSlipPanel<CDialog>::OnVScroll(nSBCode, nPos, pScrollBar);
}

void KSceneSceneEditorWaterPanel::OnTimer(UINT_PTR nIDEvent)
{
    switch (nIDEvent)
    {
    case GET_HEIGHT_EVENT :
        {
            if (m_lpSceneSceneEditor)
            {
                float fHeight = 0;
				int nHeight = 0;
                m_lpSceneSceneEditor->GetTerrainModifyHeight(fHeight);
				nHeight =(int) (fHeight / 12.5f);
                if (nHeight != m_nHeight)
                {
                    m_nHeight = nHeight;
                    UpdateData(FALSE);
                }
            }
        }
        break;
    default :
        return KGLilisSlipPanel<CDialog>::OnTimer(nIDEvent);
        break;
    }
}

BOOL KSceneSceneEditorWaterPanel::OnInitDialog()
{
    BOOL res = KGLilisSlipPanel<CDialog>::OnInitDialog();

    if (!res)
        return FALSE;

    if (m_pWaterEffect)
    {
        KG3DEngineOption optEngine;

        m_strSurfaceTextureName = m_pWaterEffect->GetWaterSurfaceTextureName();
        m_strBumpTextureName = m_pWaterEffect->GetWaterBumpTextureName();
    
        m_ComboDetail.AddString(TEXT("Lower"));
        m_ComboDetail.AddString(TEXT("Medium"));
        m_ComboDetail.AddString(TEXT("High"));
        m_ComboDetail.AddString(TEXT("Very High"));
        m_ComboDetail.AddString(TEXT("Full Effective"));

        g_pEngineManager->GetEngineOption(&optEngine);
        m_ComboDetail.SetCurSel(optEngine.nWaterDetail);

        static_cast<CSliderCtrl*>(GetDlgItem(IDC_SLIDER_UVS))->SetPos(static_cast<int>(m_pWaterEffect->GetTextParamPtr()->x * 100000.f));
        static_cast<CSliderCtrl*>(GetDlgItem(IDC_SLIDER_UVB))->SetPos(static_cast<int>(m_pWaterEffect->GetTextParamPtr()->y * 100000.f));
        static_cast<CSliderCtrl*>(GetDlgItem(IDC_SLIDER_REFR_K))->SetPos(static_cast<int>(m_pWaterEffect->GetTextParamPtr()->w * 0.01f));
        static_cast<CSliderCtrl*>(GetDlgItem(IDC_SLIDER_SPEED_U))->SetPos(static_cast<int>(m_pWaterEffect->GetWaveParamPtr()->w * 100));
        static_cast<CSliderCtrl*>(GetDlgItem(IDC_SLIDER_SPEED_V))->SetPos(static_cast<int>(m_pWaterEffect->GetWaveParamPtr()->z * 100));
        static_cast<CSliderCtrl*>(GetDlgItem(IDC_SLIDER_FRE))->SetPos(static_cast<int>(m_pWaterEffect->GetWaveParamPtr()->y * 10.f));
        static_cast<CSliderCtrl*>(GetDlgItem(IDC_SLIDER_WAVE))->SetPos(static_cast<int>(m_pWaterEffect->GetWaveParamPtr()->x * 1000.f));
        static_cast<CSliderCtrl*>(GetDlgItem(IDC_SLIDER_WAVE2))->SetPos(static_cast<int>(m_pWaterEffect->GetTextParamPtr()->z * 1000.f));

        m_pWaterEffect->GetFogInfo(m_fFogStart, m_fFogEnd);

        m_ColorWnd1.SetColor(*m_pWaterEffect->GetWaterColorPtr(optEngine.nWaterDetail));
        m_ColorWnd2.SetColor(*m_pWaterEffect->GetSpecColorPtr());
        m_ColorWnd3.SetColor(*m_pWaterEffect->GetFogColorPtr());

        UpdateData(FALSE);
    }

    if (g_pEngineManager)
    {
        CheckDlgButton(IDC_CHECK_HIDE, !g_pEngineManager->GetRenderOption(RENDER_WATER));
    }

    if (m_lpSceneSceneEditor)
    {       
		float fHeight = 0;
		m_lpSceneSceneEditor->GetTerrainModifyHeight(fHeight);
		m_nHeight =(int) (fHeight / 12.5f);  
        UpdateData(FALSE);
    }

    return res;
}

void KSceneSceneEditorWaterPanel::UpdateCtrl(int nDetail)
{
    if (!m_pWaterEffect)
        return;

    m_ColorWnd1.SetColor(*m_pWaterEffect->GetWaterColorPtr(nDetail));
}

void KSceneSceneEditorWaterPanel::OnBnClickedButtonSurface()
{
    if (!m_pWaterEffect)
        return;

    TCHAR szFilters[] = "AniTexture File|*.AniTexture|Tga Files|*.tga|Bmp Files|*.bmp|Jpg Files|*.jpg|DDS Files|*.dds|All Files|*.*||";

    CFileDialog fileDlg(
        TRUE,
        NULL,
        NULL,
        OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
        szFilters,
        this
        );

    if (fileDlg.DoModal() == IDOK)
    {
        TCHAR szName[MAX_PATH];

        _tcscpy(szName, fileDlg.GetPathName().GetBuffer());
        fileDlg.GetPathName().ReleaseBuffer();
        g_GetFilePathFromFullPath(szName, szName); 

        m_pWaterEffect->SetWaterSurfaceTexture(szName);
    }
}

void KSceneSceneEditorWaterPanel::OnBnClickedButtonBump()
{
    if (!m_pWaterEffect)
        return;

    TCHAR szFilters[] = "Tga Files|*.tga|Bmp Files|*.bmp|Jpg Files|*.jpg|DDS Files|*.dds|All Files|*.*||";

    CFileDialog fileDlg(
        TRUE,
        NULL,
        NULL,
        OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
        szFilters,
        this
        );

    if (fileDlg.DoModal() == IDOK)
    {
        TCHAR szName[MAX_PATH];

        _tcscpy(szName, fileDlg.GetPathName().GetBuffer());
        fileDlg.GetPathName().ReleaseBuffer();
        g_GetFilePathFromFullPath(szName, szName); 

        m_pWaterEffect->SetWaterBumpTexture(szName);
    }
}

void KSceneSceneEditorWaterPanel::OnStnClickedColorDif()
{
    if (!m_pWaterEffect)
        return;

    GetColorPickerDialog()->Init((D3DCOLORVALUE*)m_pWaterEffect->GetWaterColorPtr(m_ComboDetail.GetCurSel()), TEXT("水面颜色"), &m_ColorWnd1, TRUE);
   // GetColorPickerDialog()->CenterWindow(this);
}

void KSceneSceneEditorWaterPanel::OnStnClickedColorSpe()
{
    if (!m_pWaterEffect)
        return;

    GetColorPickerDialog()->Init((D3DCOLORVALUE*)m_pWaterEffect->GetSpecColorPtr(), TEXT("水面高光"), &m_ColorWnd2, TRUE);
}

void KSceneSceneEditorWaterPanel::OnStnClickedColorFog()
{
    if (!m_pWaterEffect)
        return;

    GetColorPickerDialog()->Init((D3DCOLORVALUE*)m_pWaterEffect->GetFogColorPtr(), TEXT("水中雾色"), &m_ColorWnd3, TRUE);
}

void KSceneSceneEditorWaterPanel::OnCbnSelchangeWaterDetail()
{
    int nSel = m_ComboDetail.GetCurSel();

    if (nSel == CB_ERR)
        return;

    KG3DEngineOption optEngine;
    g_pEngineManager->GetEngineOption(&optEngine);
    optEngine.nWaterDetail = nSel;
    g_pEngineManager->SetEngineOption(&optEngine);

    UpdateCtrl(nSel);
}

void KSceneSceneEditorWaterPanel::OnBnClickedButtonReloadShader()
{
    if (m_pWaterEffect)
        m_pWaterEffect->ReloadEffect();
}

void KSceneSceneEditorWaterPanel::OnBnClickedCheckHide()
{
    if(g_pEngineManager)
        g_pEngineManager->SetRenderOption(RENDER_WATER, !IsDlgButtonChecked(IDC_CHECK_HIDE));
}

void KSceneSceneEditorWaterPanel::OnBnClickedCheckEdge()
{
    if (m_lpSceneSceneEditor)
        m_lpSceneSceneEditor->SetRenderWaterEdge(IsDlgButtonChecked(IDC_CHECK_EDGE));
}

void KSceneSceneEditorWaterPanel::OnBnClickedCheckSet()
{
    if (m_lpSceneSceneEditor)
    {
        if (IsDlgButtonChecked(IDC_CHECK_SET))
        {
            m_lpSceneSceneEditor->SetEditState(SCENESTATE_TERRAIN);
            m_lpSceneSceneEditor->SetTBModifierState(WATER_SET);
        }
        else
        {
            m_lpSceneSceneEditor->SetEditState(SCENESTATE_SELECT);
        }
    }
}

void KSceneSceneEditorWaterPanel::OnBnClickedCheckGet()
{
    if  (m_lpSceneSceneEditor)
    {
        m_lpSceneSceneEditor->SetEditState(SCENESTATE_TERRAIN);
        m_lpSceneSceneEditor->SetTBModifierState(WATER_GET);
        SetTimer(GET_HEIGHT_EVENT, 100, NULL);
    }
    else
    {
        KillTimer(GET_HEIGHT_EVENT);
    }
}

void KSceneSceneEditorWaterPanel::OnEnChangeEditHeight()
{
    if (m_lpSceneSceneEditor)
    {
        UpdateData();
		float fHeight = m_nHeight * 12.5f;
		TCHAR szText[MAX_PATH];
		sprintf(szText,"高度值： %.1f 厘米",fHeight);
		((CButton*)GetDlgItem(IDC_STATIC_HEIGHT))->SetWindowText(szText);
        m_lpSceneSceneEditor->SetTerrainModifyHeight(fHeight);
    }
}

void KSceneSceneEditorWaterPanel::OnEnChangeEditFotStart()
{
    UpdateData();

    if (m_pWaterEffect)
        m_pWaterEffect->SetFogInfo(m_fFogStart, m_fFogEnd);
}

void KSceneSceneEditorWaterPanel::OnEnChangeEditFogEnd()
{
    UpdateData();

    if (m_pWaterEffect)
        m_pWaterEffect->SetFogInfo(m_fFogStart, m_fFogEnd);
}

void KSceneSceneEditorWaterPanel::OnBnClickedButtonDefHeight()
{
    if (m_lpSceneSceneEditor)
    {
        if(MessageBox("你确定恢复水面默认高度？所有操作将丢失", "危险操作",
            MB_YESNO | MB_ICONQUESTION) == IDNO)
            return;
		float fHeight = m_nHeight * 12.5f;
        m_lpSceneSceneEditor->SetTerrainWaterDefaultHeight(fHeight);
    }
}
