// KSceneSFXEditorExpPage.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSFXEditorExpPage.h"
#include "KSceneSFXEditorFrame.h"
#include "IEKG3DSFX.h"


// KSceneSFXEditorExpPage dialog

IMPLEMENT_DYNAMIC(KSceneSFXEditorExpPage, KGLilisSlipPanel<CPropertyPage>)

KSceneSFXEditorExpPage::KSceneSFXEditorExpPage(CWnd* pParent /*=NULL*/)
	: KGLilisSlipPanel<CPropertyPage>(KSceneSFXEditorExpPage::IDD)
{

}

KSceneSFXEditorExpPage::~KSceneSFXEditorExpPage()
{
}

void KSceneSFXEditorExpPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_TIME, m_EditTimeScal);
    DDX_Control(pDX, IDC_EDIT_X, m_EditSwingX);
    DDX_Control(pDX, IDC_EDIT_X2, m_EditFrequX);
    DDX_Control(pDX, IDC_EDIT_Y, m_EditSwingY);
    DDX_Control(pDX, IDC_EDIT_Y2, m_EditFrequY);
    DDX_Control(pDX, IDC_EDIT_Z, m_EditSwingZ);
    DDX_Control(pDX, IDC_EDIT_Z2, m_EditFrequZ);
    DDX_Control(pDX, IDC_EDIT_SCANL_X, m_EditScanX);
    DDX_Control(pDX, IDC_EDIT_SCANL_Y, m_EditScanY);
    DDX_Control(pDX, IDC_EDIT_SCANL_Z, m_EditScanZ);
    DDX_Control(pDX, IDC_BUTTON_ADD_VP, m_Button0);
    DDX_Control(pDX, IDC_BUTTON_DEL_VP, m_Button1);
    DDX_Control(pDX, IDC_BUTTON_BROWSE_TEXTURE, m_Button2);
    DDX_Control(pDX, IDC_COLOR_BUTTON_VP, m_btColor);
    DDX_Control(pDX, IDC_EDIT_UV_X, m_EditUV_X);
    DDX_Control(pDX, IDC_EDIT_UV_Y, m_EditUV_Y);
    DDX_Control(pDX, IDC_EDIT_UV_Z, m_EditUV_Z);
    DDX_Control(pDX, IDC_EDIT_UV_W, m_EditUV_W);
    DDX_Control(pDX, IDC_EDIT_U, m_EditClipU);
    DDX_Control(pDX, IDC_EDIT_V, m_EditClipV);
    DDX_Control(pDX, IDC_EDIT_SPEED, m_EditFrameSpeed);
}


BEGIN_MESSAGE_MAP(KSceneSFXEditorExpPage, KGLilisSlipPanel<CPropertyPage>)
    ON_EN_SETFOCUS(IDC_EDIT_TIME, &KSceneSFXEditorExpPage::OnEnSetfocusEditTime)
    ON_EN_SETFOCUS(IDC_EDIT_X, &KSceneSFXEditorExpPage::OnEnSetfocusEditSwing)
    ON_EN_SETFOCUS(IDC_EDIT_X2, &KSceneSFXEditorExpPage::OnEnSetfocusEditFrequ)
    ON_EN_SETFOCUS(IDC_EDIT_Y, &KSceneSFXEditorExpPage::OnEnSetfocusEditSwing)
    ON_EN_SETFOCUS(IDC_EDIT_Y2, &KSceneSFXEditorExpPage::OnEnSetfocusEditFrequ)
    ON_EN_SETFOCUS(IDC_EDIT_Z, &KSceneSFXEditorExpPage::OnEnSetfocusEditSwing)
    ON_EN_SETFOCUS(IDC_EDIT_Z2, &KSceneSFXEditorExpPage::OnEnSetfocusEditFrequ)
    ON_EN_SETFOCUS(IDC_EDIT_SCANL_X, &KSceneSFXEditorExpPage::OnEnSetfocusModelScanl)
    ON_EN_SETFOCUS(IDC_EDIT_SCANL_Y, &KSceneSFXEditorExpPage::OnEnSetfocusModelScanl)
    ON_EN_SETFOCUS(IDC_EDIT_SCANL_Z, &KSceneSFXEditorExpPage::OnEnSetfocusModelScanl)
    ON_BN_CLICKED(IDC_CHECK_SHOW_SCANL_MDL, &KSceneSFXEditorExpPage::OnBnClickedCheckShowScanlMdl)
    ON_BN_CLICKED(IDC_BUTTON_ADD_VP, &KSceneSFXEditorExpPage::OnBnClickedButtonAddVp)
    ON_CBN_SELCHANGE(IDC_COMBO_VP, &KSceneSFXEditorExpPage::OnCbnSelchangeComboVp)
    ON_CBN_SETFOCUS(IDC_COMBO_VP, &KSceneSFXEditorExpPage::OnCbnSetfocusComboVp)
    ON_BN_CLICKED(IDC_BUTTON_DEL_VP, &KSceneSFXEditorExpPage::OnBnClickedButtonDelVp)
    ON_CBN_SELCHANGE(IDC_COMBO_VP_TYPE, &KSceneSFXEditorExpPage::OnCbnSelchangeComboVpType)
    ON_STN_CLICKED(IDC_COLOR_BUTTON_VP, &KSceneSFXEditorExpPage::OnBnClickedColorButtonVp)
    ON_BN_DOUBLECLICKED(IDC_COLOR_BUTTON_VP, &KSceneSFXEditorExpPage::OnBnClickedColorButtonVp)
    ON_EN_SETFOCUS(IDC_EDIT_UV_X, &KSceneSFXEditorExpPage::OnEnSetfocusUVTime)
    ON_EN_SETFOCUS(IDC_EDIT_UV_Y, &KSceneSFXEditorExpPage::OnEnSetfocusUVTime)
    ON_EN_SETFOCUS(IDC_EDIT_UV_Z, &KSceneSFXEditorExpPage::OnEnSetfocusUVTime)
    ON_EN_SETFOCUS(IDC_EDIT_UV_W, &KSceneSFXEditorExpPage::OnEnSetfocusUVTime)
    ON_CBN_SELCHANGE(IDC_COMBO_BLEND_TYPE, &KSceneSFXEditorExpPage::OnCbnSelchangeComboBlendType)
    ON_BN_CLICKED(IDC_BUTTON_BROWSE_TEXTURE, &KSceneSFXEditorExpPage::OnBnClickedButtonBrowseTexture)
END_MESSAGE_MAP()


namespace {
    
    int GetInt(LPARAM lParam) {
        CString* str = (CString*)(DWORD_PTR)lParam;
        int n = atoi(str->GetBuffer());
        str->ReleaseBuffer();
        return n;
    }

    float GetFloat(LPARAM lParam) {
        CString* str = (CString*)(DWORD_PTR)lParam;
        float f = (float)atof(str->GetBuffer());
        str->ReleaseBuffer();
        return f;
    }

}

BOOL KSceneSFXEditorExpPage::OnInitDialog()
{
    BOOL bRes = KGLilisSlipPanel<CPropertyPage>::OnInitDialog();

    CComboBox* pCombox = (CComboBox*)GetDlgItem(IDC_COMBO_VP_TYPE);

    if (!pCombox)
        return FALSE;

    pCombox->ResetContent();
    pCombox->AddString(TEXT("纵向投影"));
    pCombox->AddString(TEXT("横向投影"));
    pCombox->AddString(TEXT("环绕投影"));

    pCombox = (CComboBox*)GetDlgItem(IDC_COMBO_BLEND_TYPE);

    if (!pCombox)
        return FALSE;

    pCombox->AddString(TEXT("混合"));
    pCombox->AddString(TEXT("叠加"));
    pCombox->AddString(TEXT("屏幕"));

    UpdateUI();

    return bRes;
}

BOOL KSceneSFXEditorExpPage::OnSetActive()
{
    UpdateUI();
    return TRUE;
}

IKG3DSfxBoxProjection* KSceneSFXEditorExpPage::GetSelBoxProjection()
{
    CComboBox* pCombox = (CComboBox*)GetDlgItem(IDC_COMBO_VP);

    if (!pCombox)
        return NULL;

    int nSel = pCombox->GetCurSel();

    if (nSel == CB_ERR)
        return NULL;

    IEKG3DSFX* sfx = GetSfx();

    if (!sfx)
        return NULL;

    return sfx->GetBoxProjection(nSel);
}

LRESULT KSceneSFXEditorExpPage::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    IEKG3DSfxExp* pSfxExp = NULL;

    GET_EDIT_INFO();

    KG_PROCESS_ERROR(pFrame);
    KG_PROCESS_ERROR(pScene);
    KG_PROCESS_ERROR(pSfx);

    pSfxExp = pSfx->GetExpEffect();

    KG_PROCESS_ERROR(pSfxExp);

    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();

    switch (message)
    {
    case WM_EDIT_RECEIVE_ENTER:
        {
            switch (wParam)
            {
            case IDC_EDIT_TIME :
                {
                    float fScal = GetFloat(lParam);
                    pSfxExp->GetTimeControlTimeLine()->Insert(nFrame, &fScal);
                    UpdateKeyFrameUI();
                }
                break;
            case IDC_EDIT_X :
            case IDC_EDIT_Y :
            case IDC_EDIT_Z :
                {
                    D3DXVECTOR3 v;
                    float f = GetFloat(lParam);
                    pSfxExp->GetCameraSwingTimeLine()->GetData(static_cast<float>(nFrame), &v);
                    
                    if (wParam == IDC_EDIT_X)
                        v.x = f;
                    else if (wParam == IDC_EDIT_Y)
                        v.y = f;
                    else
                        v.z = f;
        
                    pSfxExp->GetCameraSwingTimeLine()->Insert(nFrame, &v);
                    UpdateKeyFrameUI();
            
                }
                break;
            case IDC_EDIT_X2 :
            case IDC_EDIT_Y2 :
            case IDC_EDIT_Z2 :
                {
                    D3DXVECTOR3 v;
                    float f = GetFloat(lParam);
                    pSfxExp->GetCameraFrequencyTimeLine()->GetData(static_cast<float>(nFrame), &v);

                    if (wParam == IDC_EDIT_X2)
                        v.x = f;
                    else if (wParam == IDC_EDIT_Y2)
                        v.y = f;
                    else
                        v.z = f;

                    pSfxExp->GetCameraFrequencyTimeLine()->Insert(nFrame, &v);
                    UpdateKeyFrameUI();

                }
                break;
            case IDC_EDIT_SCANL_X :
            case IDC_EDIT_SCANL_Y :
            case IDC_EDIT_SCANL_Z :
                {
                    D3DXVECTOR3 v;
                    float f = GetFloat(lParam);
                    pSfxExp->GetModelScanlTimeLine()->GetData(static_cast<float>(nFrame), &v);

                    if (wParam == IDC_EDIT_SCANL_X)
                        v.x = f;
                    else if (wParam == IDC_EDIT_SCANL_Y)
                        v.y = f;
                    else
                        v.z = f;

                    pSfxExp->GetModelScanlTimeLine()->Insert(nFrame, &v);
                    UpdateKeyFrameUI();
                   
                    pScene->UpdateSfxExp();

                }
                break;
            case IDC_EDIT_UV_X :
            case IDC_EDIT_UV_Y :
            case IDC_EDIT_UV_Z :
            case IDC_EDIT_UV_W :
                {
                    IKG3DSfxBoxProjection* pBoxProjection = GetSelBoxProjection();

                    if (!pBoxProjection)
                        break;

                    IKG3DTimeLine* ptlUV = pBoxProjection->GetUVTimeLine();

                    if (!ptlUV)
                        break;

                    float f = GetFloat(lParam);
                    
                    D3DXVECTOR4 v;
                    ptlUV->GetData((float)nFrame, &v);

                    switch (wParam)
                    {
                    case IDC_EDIT_UV_X :
                        v.x = f;
                        break;
                    case IDC_EDIT_UV_Y :
                        v.y = f;
                        break;
                    case IDC_EDIT_UV_Z :
                        v.z = f;
                        break;
                    case IDC_EDIT_UV_W :
                        v.w = f;
                        break;
                    default :
                        break;
                    }
                    
                    ptlUV->Insert(nFrame, &v);
                    UpdateKeyFrameUI();

                }
                break;
            case IDC_EDIT_SPEED :
                {
                    IKG3DSfxBoxProjection* pBoxProjection = GetSelBoxProjection();

                    if (!pBoxProjection)
                        break;

                    float f = GetFloat(lParam);
                    pBoxProjection->SetUVFrameSpeed(f);
                }
                break;
            case IDC_EDIT_U :
                {
                    IKG3DSfxBoxProjection* pBoxProjection = GetSelBoxProjection();

                    if (!pBoxProjection)
                        break;

                    int n = GetInt(lParam);

                    int u = 0;
                    int v = 0;
                    pBoxProjection->GetTexClip(u, v);
                    pBoxProjection->SetTexClip(n, v);
                }
                break;
            case IDC_EDIT_V :
                {
                    IKG3DSfxBoxProjection* pBoxProjection = GetSelBoxProjection();

                    if (!pBoxProjection)
                        break;

                    int n = GetInt(lParam);

                    int u = 0;
                    int v = 0;
                    pBoxProjection->GetTexClip(u, v);
                    pBoxProjection->SetTexClip(u, n);
                }
                break;
            default :
                break;
            }
        }
        break;
    case WM_COLORPOINTER_CHANGE:
        {
            IKG3DSfxBoxProjection* pBoxProjection = GetSelBoxProjection();

            if (!pBoxProjection)
                return FALSE;

            if (pFrame->GetKeysFrame()->GetTimeLine() == pBoxProjection->GetColorTimeLine())
            {
                m_Color = m_btColor.GetBkColor();
                pBoxProjection->GetColorTimeLine()->Insert(nFrame, &m_Color);
            }

        }
        break;
    default:
        break;
    }

Exit0 :
    return KGLilisSlipPanel<CPropertyPage>::WindowProc(message, wParam, lParam);
}

void KSceneSFXEditorExpPage::OnEnSetfocusEditTime()
{
    IEKG3DSFX* sfx = GetSfx();

    if (!sfx)
        return;

    IEKG3DSfxExp* exp = sfx->GetExpEffect();

    if (!exp)
        return;

    int nFrame = GetCurrKeyFrame();
    float fScal = 0.f;
    char text[12];
    exp->GetTimeControlTimeLine()->GetData((float)nFrame, &fScal);
    sprintf(text, "%.2f", fScal);
    m_EditTimeScal.SetWindowText(text);

    SetCurrTimeLine(exp->GetTimeControlTimeLine(), "时间控制", SFX_TLDT_FLOAT);
}

void KSceneSFXEditorExpPage::OnEnSetfocusEditSwing()
{
    IEKG3DSFX* sfx = GetSfx();

    if (!sfx)
        return;

    IEKG3DSfxExp* exp = sfx->GetExpEffect();

    if (!exp)
        return;

    int nFrame = GetCurrKeyFrame();
    D3DXVECTOR3 v;
    char text[32];
    exp->GetCameraSwingTimeLine()->GetData((float)nFrame, &v);
    sprintf(text, "%.2f", v.x);
    SetDlgItemText(IDC_EDIT_X, text);
    sprintf(text, "%.2f", v.y);
    SetDlgItemText(IDC_EDIT_Y, text);
    sprintf(text, "%.2f", v.z);
    SetDlgItemText(IDC_EDIT_Z, text);

    SetCurrTimeLine(exp->GetCameraSwingTimeLine(), "镜头振动振幅", SFX_TLDT_FLOAT);
}

void KSceneSFXEditorExpPage::OnEnSetfocusModelScanl()
{
    IEKG3DSFX* sfx = GetSfx();

    if (!sfx)
        return;

    IEKG3DSfxExp* exp = sfx->GetExpEffect();

    if (!exp)
        return;

    int nFrame = GetCurrKeyFrame();
    D3DXVECTOR3 v;
    char text[32];
    exp->GetModelScanlTimeLine()->GetData((float)nFrame, &v);
    sprintf(text, "%.2f", v.x);
    SetDlgItemText(IDC_EDIT_SCANL_X, text);
    sprintf(text, "%.2f", v.y);
    SetDlgItemText(IDC_EDIT_SCANL_Y, text);
    sprintf(text, "%.2f", v.z);
    SetDlgItemText(IDC_EDIT_SCANL_Z, text);

    SetCurrTimeLine(exp->GetModelScanlTimeLine(), "模型控制(缩放)", SFX_TLDT_VECTOR3);
}

void KSceneSFXEditorExpPage::OnEnSetfocusEditFrequ()
{
    IEKG3DSFX* sfx = GetSfx();

    if (!sfx)
        return;

    IEKG3DSfxExp* exp = sfx->GetExpEffect();

    if (!exp)
        return;

    int nFrame = GetCurrKeyFrame();
    D3DXVECTOR3 v;
    char text[12];
    exp->GetCameraFrequencyTimeLine()->GetData((float)nFrame, &v);
    sprintf(text, "%.2f", v.x);
    SetDlgItemText(IDC_EDIT_X2, text);
    sprintf(text, "%.2f", v.y);
    SetDlgItemText(IDC_EDIT_Y2, text);
    sprintf(text, "%.2f", v.z);
    SetDlgItemText(IDC_EDIT_Z2, text);

    SetCurrTimeLine(exp->GetCameraFrequencyTimeLine(), "镜头振动频率", SFX_TLDT_FLOAT);
}



void KSceneSFXEditorExpPage::OnBnClickedCheckShowScanlMdl()
{
    BOOL bShow = IsDlgButtonChecked(IDC_CHECK_SHOW_SCANL_MDL);

    IEKG3DSceneSFXEditor* pEditor = GetSfxEditor();
    pEditor->ShowExpScanlModel(bShow);

    IEKG3DSFX* sfx = GetSfx();

    if (!sfx)
        return;

    IEKG3DSfxExp* exp = sfx->GetExpEffect();

    if (!exp)
        return;

    if (bShow)
    {
        SetCurrTimeLine(exp->GetModelScanlTimeLine(), "模型控制(缩放)", SFX_TLDT_VECTOR3);
    }
}

void KSceneSFXEditorExpPage::OnBnClickedButtonAddVp()
{
    IEKG3DSceneSFXEditor* pEditor = GetSfxEditor();

    if (!pEditor)
        return;

    pEditor->AddBoxProjection();

    CComboBox* pCombox = (CComboBox*)GetDlgItem(IDC_COMBO_VP);

    if (!pCombox)
        return;

    IEKG3DSFX* sfx = GetSfx();

    if (!sfx)
        return;

    IKG3DSfxBoxProjection* bp = sfx->GetBoxProjection((int)sfx->GetBoxProjectionNum() - 1);

    if (!bp)
        return;

    char szName[32];
    sprintf(szName, "%02d", sfx->GetBoxProjectionNum() - 1);
    pCombox->AddString(szName);

    pCombox->SetCurSel((int)sfx->GetBoxProjectionNum() - 1);
    OnCbnSelchangeComboVp();

}

void KSceneSFXEditorExpPage::UpdateUI()
{
    IEKG3DSFX* sfx = GetSfx();

    if (!sfx)
        return;

    CComboBox* pCombox = (CComboBox*)GetDlgItem(IDC_COMBO_VP);

    if (!pCombox)
        return;

    pCombox->ResetContent();

    for (int i = 0; i < (int)sfx->GetBoxProjectionNum(); ++i)
    {
        char szName[32];
        sprintf(szName, "%02d", i);
        pCombox->AddString(szName);
    }

    if (sfx->GetBoxProjectionNum() > 0)
    {
        pCombox->SetCurSel(0);
        OnCbnSelchangeComboVp();
    }
}

void KSceneSFXEditorExpPage::OnCbnSelchangeComboVp()
{
    IEKG3DSceneSFXEditor* pEditor = GetSfxEditor();

    if (!pEditor)
        return;

    IEKG3DSFX* sfx = GetSfx();

    if (!sfx)
        return;

    CComboBox* pCombox = (CComboBox*)GetDlgItem(IDC_COMBO_VP);

    if (!pCombox)
        return;

    int nSel = pCombox->GetCurSel();

    if (nSel == CB_ERR)
        return;

    IKG3DSfxBoxProjection* bp = sfx->GetBoxProjection(nSel);

    if (!bp)
        return;

    IEKG3DSceneSelectionTool* pSelTool = NULL;
    HRESULT hr = pEditor->GetSelectionToolInterface(&pSelTool);
    _ASSERTE(SUCCEEDED(hr));

    pSelTool->ClearSelection();

    IEKG3DSceneEditorBase *piSceneEditorBase = NULL;
    pEditor->GetSceneEditBase(&piSceneEditorBase);

    GETSELECT(piSceneEditorBase);

    pSelector->CopySelectable2SelectedSet(bp);

    pCombox = (CComboBox*)GetDlgItem(IDC_COMBO_VP_TYPE);

    if (!pCombox)
        return;

    pCombox->SetCurSel(bp->GetProjType());

    pCombox = (CComboBox*)GetDlgItem(IDC_COMBO_BLEND_TYPE);

    if (!pCombox)
        return;

    pCombox->SetCurSel(bp->GetBlendMode());

    char szTexName[MAX_PATH];
    *szTexName = 0;
    bp->GetTexture(szTexName);
    SetDlgItemText(IDC_EDIT_TEXTURE_NAME, szTexName);

    int u = 0;
    int v = 0;
    float fFrameSpeed = 0.f;

    bp->GetTexClip(u, v);
    fFrameSpeed = bp->GetUVFrameSpeed();

    SetDlgItemInt(IDC_EDIT_U, u);
    SetDlgItemInt(IDC_EDIT_V, v);
    
    char szText[32];
    sprintf(szText, "%f", fFrameSpeed);
    SetDlgItemText(IDC_EDIT_SPEED, szText);
}

void KSceneSFXEditorExpPage::OnCbnSetfocusComboVp()
{
   // UpdateUI();
}

void KSceneSFXEditorExpPage::OnBnClickedButtonDelVp()
{
    IEKG3DSceneSFXEditor* pEditor = GetSfxEditor();

    if (!pEditor)
        return;

    CComboBox* pCombox = (CComboBox*)GetDlgItem(IDC_COMBO_VP);

    if (!pCombox)
        return;

    int nSel = pCombox->GetCurSel();

    if (nSel == CB_ERR)
        return;

    pEditor->RemoveBoxProjection(nSel);

    UpdateUI();
}

void KSceneSFXEditorExpPage::OnCbnSelchangeComboVpType()
{
    CComboBox* pCombox = (CComboBox*)GetDlgItem(IDC_COMBO_VP_TYPE);

    if (!pCombox)
        return;

    int nSel = pCombox->GetCurSel();

    if (nSel == CB_ERR)
        return;

    IKG3DSfxBoxProjection* pBoxProject = GetSelBoxProjection();

    if (!pBoxProject)
        return;

    pBoxProject->SetProjType(nSel);

}

void KSceneSFXEditorExpPage::OnBnClickedColorButtonVp()
{
    IKG3DSfxBoxProjection* pBoxProjection = GetSelBoxProjection();

    if (!pBoxProjection)
        return;

    IKG3DTimeLine* ptlColor = pBoxProjection->GetColorTimeLine();

    if (!ptlColor)
        return;

    int nFrame = GetCurrKeyFrame();

    ptlColor->GetData((float)nFrame, &m_Color);
    m_btColor.SetBKColor(m_Color);
    SetCurrTimeLine(ptlColor, "颜色", SFX_TLDT_D3DXCOLOR);
    UpdateKeyFrameUI();
}

void KSceneSFXEditorExpPage::OnEnSetfocusUVTime()
{
    IKG3DSfxBoxProjection* pBoxProjection = GetSelBoxProjection();

    if (!pBoxProjection)
        return;

    D3DXVECTOR4 v;
    pBoxProjection->GetUVTimeLine()->GetData((float)GetCurrKeyFrame(), &v);
    
    char szText[32];
    sprintf(szText, "%f", v.x);
    m_EditUV_X.SetWindowText(szText);
    sprintf(szText, "%f", v.y);
    m_EditUV_Y.SetWindowText(szText);
    sprintf(szText, "%f", v.z);
    m_EditUV_Z.SetWindowText(szText);
    sprintf(szText, "%f", v.w);
    m_EditUV_W.SetWindowText(szText);

    SetCurrTimeLine(pBoxProjection->GetUVTimeLine(), "UV", SFX_TLDT_VECTOR4);
    UpdateKeyFrameUI();
}
void KSceneSFXEditorExpPage::OnCbnSelchangeComboBlendType()
{
    CComboBox* pCombox = (CComboBox*)GetDlgItem(IDC_COMBO_BLEND_TYPE);

    if (!pCombox)
        return;

    int nSel = pCombox->GetCurSel();

    if (nSel == CB_ERR)
        return;

    IKG3DSfxBoxProjection* pBoxProject = GetSelBoxProjection();

    if (!pBoxProject)
        return;

    switch (nSel)
    {
    case 0 :
        pBoxProject->SetBlendMode(SFX_BM_SRCALPHA_INVSRCALPHA);
        break;
    case 1 :
        pBoxProject->SetBlendMode(SFX_BM_SRCALPHA_ONE);
        break;
    case 2 :
        pBoxProject->SetBlendMode(SFX_BM_SRCALPHA_SCREEN);
        break;
    default :
        break;
    }



}

void KSceneSFXEditorExpPage::OnBnClickedButtonBrowseTexture()
{
    GET_SFX_EDITOR();

    IKG3DSfxBoxProjection* pBoxProject = GetSelBoxProjection();

    if (!pBoxProject)
        return;

    char szFilters[] = "Texture Files|*.TGA;*.BMP;*.JPG;*.DDS;*.PNG|\
                       (*.TGA)|*.TGA|\
                       (*.BMP)|*.BMP|\
                       (*.JPG)|*.JPG|\
                       (*.DDS)|*.DDS|\
                       (*.PNG)|*.PNG|\
                       All Files (*.*)|*.*||";

    CFileDialog fileDlg(
        TRUE,
        NULL,
        NULL,
        OFN_EXPLORER|OFN_FILEMUSTEXIST| OFN_HIDEREADONLY,
        szFilters,
        this
        );

    if (fileDlg.DoModal() == IDOK)
    {   
        char szRetPath[MAX_PATH];
        g_GetFilePathFromFullPath(szRetPath, fileDlg.GetPathName().GetBuffer());
        fileDlg.GetPathName().ReleaseBuffer();
        pBoxProject->SetTexture(szRetPath);
        SetDlgItemText(IDC_EDIT_TEXTURE_NAME, szRetPath);

        return;
    }
}
