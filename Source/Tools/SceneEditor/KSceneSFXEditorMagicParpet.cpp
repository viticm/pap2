

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSFXEditorMagicParpet.h"
#include "KSceneSFXEditorFrame.h"
#include "IEKG3DSFX.h"
#include "IEKG3DTexture.h"
#include "KColorDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNAMIC(KSceneSFXEditorMagicParpet, CDialog)

KSceneSFXEditorMagicParpet::KSceneSFXEditorMagicParpet(CWnd* pWnd /* = NULL */)
	: CDialog(KSceneSFXEditorMagicParpet::IDD, pWnd),
      m_bInit(false)
{

}

KSceneSFXEditorMagicParpet::~KSceneSFXEditorMagicParpet()
{
}

void KSceneSFXEditorMagicParpet::DoDataExchange(CDataExchange* pDX)
{

    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BUTTON_ADD_PARPET, m_buttonAddParpet);
    DDX_Control(pDX, IDC_BUTTON_DEL_PARPET, m_buttonDelParpet);

    DDX_Control(pDX, IDC_EDIT_GRID_NUM, m_editGridNum);
    DDX_Control(pDX, IDC_EDIT_CLIP_HEIGHT, m_editClipHeight);
    DDX_Control(pDX, IDC_EDIT_HEIGHT_OFFSET, m_editHeightOffset);

    DDX_Control(pDX, IDC_EDIT_TEXTURE_NAME, m_editTextureName);
    DDX_Control(pDX, IDC_BUTTON_BROWSE_TEXTURE, m_buttonBrowseTexture);


    //   DDX_Control(pDX, IDC_TEXTURE_NAME, m_editFPTextureName);
    //   DDX_Control(pDX, IDC_EDIT_FOOTWIDTH, m_EditFootWidth);
    //    DDX_Control(pDX, IDC_ADD_FOOTPRINT, m_ButtonAddFootprint);
    // DDX_Control(pDX, IDC_DEL_FOOTPRINT, m_Button_DelFootprint);
    //  DDX_Control(pDX, IDC_BROWSE_TEXTURE, m_Button_Browse);
    //DDX_Control(pDX, IDC_EDIT_OFFSETX, m_EditOffsetX);
    //DDX_Control(pDX, IDC_EDIT_OFFSETY, m_EditOffsetY);
    //   DDX_Control(pDX, IDC_COMBO_FOOTPRINTTYPE, m_ComboFootprintType);
    DDX_Control(pDX, IDC_EDIT_LIFE,m_editLife);
    DDX_Control(pDX, IDC_COMBO5, m_comboType);
    DDX_Control(pDX, IDC_COMBO6, m_comboFace);
    DDX_Control(pDX, IDC_COMBO1, m_comboAoes);
    DDX_Control(pDX, IDC_TL_COLOR, m_stColor);
    DDX_Control(pDX, IDC_EDIT_U, m_EditU);
    DDX_Control(pDX, IDC_EDIT_V, m_EditV);
    DDX_Control(pDX, IDC_EDIT_SPEED, m_EditSpeed);
}


BEGIN_MESSAGE_MAP(KSceneSFXEditorMagicParpet, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_ADD_PARPET, &KSceneSFXEditorMagicParpet::OnBnClickedButtonAddParpet)
    ON_BN_CLICKED(IDC_BUTTON_DEL_PARPET, &KSceneSFXEditorMagicParpet::OnBnClickedButtonDelParpet)
    ON_BN_CLICKED(IDC_BUTTON_BROWSE_TEXTURE, &KSceneSFXEditorMagicParpet::OnBnClickedButtonBrowseTexture)
	ON_BN_CLICKED(IDC_ADD_FOOTPRINT, &KSceneSFXEditorMagicParpet::OnBnClickedAddFootprint)
	ON_BN_CLICKED(IDC_DEL_FOOTPRINT, &KSceneSFXEditorMagicParpet::OnBnClickedDelFootprint)
	ON_BN_CLICKED(IDC_BROWSE_TEXTURE, &KSceneSFXEditorMagicParpet::OnBnClickedBrowseTexture)
    ON_CBN_SELCHANGE(IDC_COMBO5, &KSceneSFXEditorMagicParpet::OnCbnSelchangeCombo5)
    ON_CBN_SELCHANGE(IDC_COMBO1, &KSceneSFXEditorMagicParpet::OnCbnSelchangeCombo1)
    ON_CBN_SELCHANGE(IDC_COMBO6, &KSceneSFXEditorMagicParpet::OnCbnSelchangeCombo6)
    ON_STN_CLICKED(IDC_TL_COLOR, &KSceneSFXEditorMagicParpet::OnStnClickedTlColor)
    ON_BN_DOUBLECLICKED(IDC_TL_COLOR, &KSceneSFXEditorMagicParpet::OnBnClickedTlColor)
    ON_CBN_SELCHANGE(IDC_COMBO_FOOTPRINTTYPE, &KSceneSFXEditorMagicParpet::OnCbnSelchangeComboFootprinttype)
    ON_EN_SETFOCUS(IDC_EDIT_GRID_NUM, &KSceneSFXEditorMagicParpet::OnEnSetfocusEditGridNum)
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

HBRUSH KSceneSFXEditorMagicParpet::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hBursh = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

    if (nCtlColor == CTLCOLOR_EDIT || nCtlColor == CTLCOLOR_LISTBOX)
    {
        static HBRUSH hFocus = ::CreateSolidBrush(RGB(240, 200, 85));
        static HBRUSH hNorml = ::CreateSolidBrush(RGB(210, 210, 233));
        COLORREF cFocus = RGB(240, 200, 85);
        COLORREF cNorml = RGB(210, 210, 233);

        if (::GetFocus() == pWnd->m_hWnd)
        {
            hBursh = hFocus;
            pDC->SetBkColor(cFocus);
        }
        else
        {
            hBursh = hNorml;
            pDC->SetBkColor(cNorml);
        }
    }

    return hBursh;
}


int  KSceneSFXEditorMagicParpet::UpdateUI()
{
    if (!IsWindow(m_hWnd))
        return FALSE;

    m_comboAoes.EnableWindow(FALSE);
    m_comboFace.EnableWindow(FALSE);
    m_comboType.EnableWindow(FALSE);
    m_editGridNum.EnableWindow(FALSE);
    m_editClipHeight.EnableWindow(FALSE);
    m_editHeightOffset.EnableWindow(FALSE);
    m_editLife.EnableWindow(FALSE);
    m_editTextureName.EnableWindow(FALSE);
    m_buttonBrowseTexture.EnableWindow(FALSE);
    m_EditU.EnableWindow(FALSE);
    m_EditV.EnableWindow(FALSE);
    m_EditSpeed.EnableWindow(FALSE);
    m_stColor.EnableWindow(FALSE);
    m_stColor.SetBKColor(0xAAAAAAAA);

    IEKG3DSFX* pSfx = GetSfx();
    
    if (!pSfx)
        return FALSE;

    if (pSfx->GetMagicParpetNum() > 0)
        m_comboAoes.EnableWindow(TRUE);

    IEKG3DSFXMagicParpet* aoe = GetSelAoe();

    if (!aoe)
        return FALSE;

    m_editTextureName.EnableWindow(TRUE);
    m_buttonBrowseTexture.EnableWindow(TRUE);
    m_comboType.EnableWindow(TRUE);
    m_editGridNum.EnableWindow(TRUE);
    m_editClipHeight.EnableWindow(TRUE);
    m_editHeightOffset.EnableWindow(aoe->GetType() != AOE_PROJECT);
    m_EditU.EnableWindow(aoe->GetType() == AOE_PROJECT);
    m_EditV.EnableWindow(aoe->GetType() == AOE_PROJECT);
    m_EditSpeed.EnableWindow(aoe->GetType() == AOE_PROJECT);
    m_comboType.SetCurSel(aoe->GetType());
    if (aoe->GetType() != AOE_PROJECT)
        m_comboFace.EnableWindow(TRUE);
    else
    {
        m_stColor.EnableWindow(TRUE);
        D3DXCOLOR color;
        aoe->GetColorsTimeLine()->GetData((float)GetCurrKeyFrame(), &color);
        m_stColor.SetBKColor(color);
    }
    if (aoe->GetType() == AOE_FOOTPRINT)
        m_editLife.EnableWindow(TRUE);
    m_comboFace.SetCurSel(aoe->GetFaceType());

    CString str;
    str.Format("%d", aoe->GetRadius());
    m_editGridNum.SetWindowText(str);
    str.Format("%f", aoe->GetHeightLimit());
    m_editClipHeight.SetWindowText(str);
    str.Format("%f", aoe->GetOffset());
    m_editHeightOffset.SetWindowText(str);
    str.Format("%f", aoe->GetLife());
    m_editLife.SetWindowText(str);
    int x;
    int y;
    aoe->GetTexCull(&x, &y);
    str.Format("%d", x);
    m_EditU.SetWindowText(str);
    str.Format("%d", y);
    m_EditV.SetWindowText(str);
    str.Format("%.2f", (float)pSfx->GetTotalFrameNum() / (float)(x * y) / aoe->GetTexSpeed());
    m_EditSpeed.SetWindowText(str);

    IEKG3DTexture* pTexture = aoe->GetTexture();
    LPCTSTR pszName = NULL;
    
    if (pTexture)
    {
        pTexture->GetName(&pszName);
        str.Format("%s", pszName);
    }
    else
        str.Format("%s", TEXT(""));
    m_editTextureName.SetWindowText(str);


    /*
    UpdateMagicParpetProperty();
	UpdateFootprintProperty();
    */


    return TRUE;
}

BOOL KSceneSFXEditorMagicParpet::OnSetActive()
{
    if (!m_bInit)
    {
        IEKG3DSFX* sfx = GetSfx();

        if (!sfx)
            return FALSE;

        m_comboAoes.ResetContent();

        for (size_t i = 0; i < sfx->GetMagicParpetNum(); ++i)           
        {
            char szName[128];
            sprintf(szName, "aoe-%02d", i);
            m_comboAoes.AddString(szName);
            m_comboAoes.SetItemData((int)i, (DWORD_PTR)sfx->GetMagicParpet((int)i));
        }

        if (sfx->GetMagicParpetNum() > 0)
            m_comboAoes.SetCurSel(0);

        m_bInit = false;
    }

    return UpdateUI();

}

void KSceneSFXEditorMagicParpet::OnBnClickedButtonAddParpet()
{
    GET_SFX_EDITOR();

    HRESULT hr = E_FAIL;
    IEKG3DSFXMagicParpet *pMagicParpet = NULL;
    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);

    char szName[128];
    int  item =  (int)pSFX->GetMagicParpetNum();
    sprintf(szName, "aoe-%02d", item);

    pSFX->AddMagicParpet(&pMagicParpet, AOE_PROJECT);
    pMagicParpet->SetRadius(50);

    m_comboAoes.AddString(szName);
    m_comboAoes.SetItemData(item, (DWORD_PTR)pMagicParpet);
    m_comboAoes.SetCurSel(item);

    UpdateUI();

}

void KSceneSFXEditorMagicParpet::UpdateMagicParpetProperty()
{
//    GET_SFX_EDITOR();
//    IEKG3DTexture *pTexture = NULL;
//    LPCTSTR pszName = NULL;
//    IEKG3DSFX *pSFX = NULL;
//    pScene->GetSFX(&pSFX);
//    IEKG3DSFXMagicParpet *pMagicParpet = pSFX->GetMagicParpet(0);
//    if (!pMagicParpet)
//    {
//        m_buttonAddParpet.EnableWindow();
//        m_buttonDelParpet.EnableWindow(FALSE);
//
//        m_editGridNum.EnableWindow(FALSE);
//        //m_editGridSize.EnableWindow(FALSE);
//        m_editHeightOffset.EnableWindow(FALSE);
//        m_editClipHeight.EnableWindow(FALSE);
//		m_editLife.EnableWindow(FALSE);
//        m_editTextureName.EnableWindow(FALSE);
//        m_buttonBrowseTexture.EnableWindow(FALSE);
//
//		m_BLeft.EnableWindow(FALSE);
//		m_BDefault.EnableWindow(FALSE);
//		m_BRight.EnableWindow(FALSE);
//
//		CButton* pCheck = (CButton*)(GetDlgItem(IDC_BILLBORD));
//		pCheck->EnableWindow(FALSE);
//        return;
//    }
//
//    m_buttonAddParpet.EnableWindow(FALSE);
//    m_buttonDelParpet.EnableWindow();
//
//    m_editGridNum.EnableWindow();
//    //m_editGridSize.EnableWindow();
//    m_editHeightOffset.EnableWindow();
//    m_editClipHeight.EnableWindow();
//	m_editLife.EnableWindow();
//    m_editTextureName.EnableWindow();
//    m_buttonBrowseTexture.EnableWindow();
//	
//	m_BLeft.EnableWindow(TRUE);
//	m_BDefault.EnableWindow(TRUE);
//	m_BRight.EnableWindow(TRUE);
//
//	CButton* pCheck = (CButton*)(GetDlgItem(IDC_BILLBORD));
//	pCheck->EnableWindow(TRUE);
//
//	pCheck->SetCheck(pMagicParpet->IsFootprint());
//    CString str;
//    //str.Format("%f", pMagicParpet->GetGridSize());
//    //m_editGridSize.SetWindowText(str);
//    str.Format("%d", pMagicParpet->GetRadius());
//    m_editGridNum.SetWindowText(str);
//    str.Format("%f", pMagicParpet->GetHeightLimit());
//    m_editClipHeight.SetWindowText(str);
//    str.Format("%f", pMagicParpet->GetOffset());
//    m_editHeightOffset.SetWindowText(str);
//
//	str.Format("%d", (int)pMagicParpet->GetLife());
//	m_editLife.SetWindowText(str);
//
//	int nOption = pMagicParpet->GetOptionFaceTo();
//	switch(nOption)
//	{
//	case 1://FaceToCamera
//		m_BLeft.SetCheck(TRUE);
//		break;
//	case 2://FaceToSowrdsMan
//		m_BRight.SetCheck(TRUE);
//		break;
//	case 0://DEFAULT
//		m_BDefault.SetCheck(TRUE);
//		break;
//	default:
//		
//		break;
//	}
//	  pTexture = pMagicParpet->GetTexture();
//    if (pTexture)
//    {
//        pTexture->GetName(&pszName);
//        str.Format("%s", pszName);
//    }
//    else
//        str.Format("%s", TEXT(""));
//    m_editTextureName.SetWindowText(str);
}

BOOL KSceneSFXEditorMagicParpet::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_comboType.AddString(TEXT("普通"));
    m_comboType.AddString(TEXT("投影"));
    m_comboType.AddString(TEXT("脚印"));
    m_comboType.AddString(TEXT("选择"));

    m_comboFace.AddString(TEXT("无"));
    m_comboFace.AddString(TEXT("眼睛"));
    m_comboFace.AddString(TEXT("主角"));

    m_stColor.SetBKColor(0xffffffff);
    m_stColor.SetAlphaEnable(TRUE);

    return TRUE;
}

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

LRESULT KSceneSFXEditorMagicParpet::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    // TODO: Add your specialized code here and/or call the base class

    switch (message)
    {
    case WM_EDIT_RECEIVE_ENTER:
        {
            switch (wParam)
            {
            case IDC_EDIT_GRID_NUM:
                OnSetGridNum();
                break;
            case IDC_EDIT_GRID_SIZE:
                OnSetGridSize();
                break;
            case IDC_EDIT_HEIGHT_OFFSET:
                OnSetHeightOffset();
                break;
            case IDC_EDIT_CLIP_HEIGHT:
                OnSetClipHeight();
                break;
			case IDC_EDIT_FOOTWIDTH:
				OnSetFootWidth();
				break;
			case IDC_COMBO_FOOTPRINTTYPE:
				{
					int aj = 0;
					break;
				}
			case IDC_EDIT_LIFE:
					OnSetLife();
					break;

            case IDC_EDIT_U :
                {
                    IEKG3DSFXMagicParpet* aoe = GetSelAoe();

                    if (!aoe)
                        break;

                    int x;
                    int y;
                    aoe->GetTexCull(&x, &y);
                    aoe->SetTexCull(GetInt(lParam), y);
                }
                break;

            case IDC_EDIT_V :
                {
                    IEKG3DSFXMagicParpet* aoe = GetSelAoe();

                    if (!aoe)
                        break;

                    int x;
                    int y;
                    aoe->GetTexCull(&x, &y);
                    aoe->SetTexCull(x, GetInt(lParam));
                }
                break;

            case IDC_EDIT_SPEED :
                {
                    IEKG3DSFXMagicParpet* aoe = GetSelAoe();
                    IEKG3DSFX* sfx = GetSfx();

                    if (!aoe || !sfx)
                        break;

                    int frmNum = (int)sfx->GetTotalFrameNum();
                    int x;
                    int y;
                    aoe->GetTexCull(&x, &y);
                    int texNum = x * y;
                    
                    float fullSpeed = (float)frmNum / (float)texNum;
                    aoe->SetTexSpeed(fullSpeed / GetFloat(lParam));

                }
                break;

            default:
                break;
            }
            break;
        }
    case WM_COLORPOINTER_CHANGE:
        {
            IEKG3DSFXMagicParpet* aoe = GetSelAoe();

            if (!aoe)
                break;

            if (GetCurrTimeLine() == aoe->GetColorsTimeLine())
            {
                int nFrame = GetCurrKeyFrame();
                D3DXCOLOR color = m_stColor.GetBkColor();
                aoe->GetColorsTimeLine()->Insert(nFrame, &color);
                UpdateKeyFrameUI();

            }

            /*
            if (pFrame->GetKeysFrame()->GetTimeLine() == pLifeTime->GetColorTimeLineL())
            {
                EnableWindow();
                GET_SFX_EDITOR_RET(-1);
                GET_LIFE_TIME_LINE_RET(-1);
                int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
                static_cast<TKG3DTimeLine<D3DXCOLOR>*>(pLifeTime->GetColorTimeLineL())->InsertKeyFrame( nFrame, m_ColorL );
                pFrame->GetKeysFrame()->UpdateSliderCtrl();
                UpdateColor(pLifeTime, nFrame);
            }

            if (pFrame->GetKeysFrame()->GetTimeLine() == pLifeTime->GetColorTimeLineR())
            {
                EnableWindow();
                int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
                static_cast<TKG3DTimeLine<D3DXCOLOR>*>(pLifeTime->GetColorTimeLineR())->InsertKeyFrame( nFrame, m_ColorR );
                pFrame->GetKeysFrame()->UpdateSliderCtrl();
                UpdateColor(pLifeTime, nFrame);
            }*/
        }
        break;
    default:
        break;
    }

    return CDialog::WindowProc(message, wParam, lParam);
}

void KSceneSFXEditorMagicParpet::OnSetGridNum()
{
    IEKG3DSFXMagicParpet* aoe = GetSelAoe();

    if (!aoe)
        return;

    CString str;
    m_editGridNum.GetWindowText(str);
    int nVal = ::atoi(str);

    if (aoe->GetType() == AOE_PROJECT)
    {
        aoe->GetRadiusTimeLine()->Insert(GetCurrKeyFrame(), &nVal);
        UpdateKeyFrameUI();
    }
    else
        aoe->SetRadius(nVal, true);


    /*
    GET_SFX_EDITOR();
    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    IEKG3DSFXMagicParpet *pMagicParpet = pSFX->GetMagicParpet(0);
    if (!pMagicParpet)
        return;

    CString str;
    m_editGridNum.GetWindowText(str);
    int nVal = ::atoi(str);
    pMagicParpet->SetRadius(nVal, true);
    */
}

void KSceneSFXEditorMagicParpet::OnSetGridSize()
{
   /* GET_SFX_EDITOR();
    KG3DSFXMagicParpet *pMagicParpet = pScene->m_lpSFX->GetMagicParpet();
    if (!pMagicParpet)
        return;

    CString str;
    m_editGridSize.GetWindowText(str);
    float fVal = (float)::atof(str);
    pMagicParpet->SetGridSize(fVal);*/
}

void KSceneSFXEditorMagicParpet::OnSetClipHeight()
{
    GET_SFX_EDITOR();
    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    IEKG3DSFXMagicParpet *pMagicParpet = pSFX->GetMagicParpet(0);
    if (!pMagicParpet)
        return;

    CString str;
    m_editClipHeight.GetWindowText(str);
    float fVal = (float)::atof(str);
    pMagicParpet->SetHeightLimit(fVal);
}

void KSceneSFXEditorMagicParpet::OnSetHeightOffset()
{
    GET_SFX_EDITOR();
    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    IEKG3DSFXMagicParpet *pMagicParpet = pSFX->GetMagicParpet(0);
    if (!pMagicParpet)
        return;

    CString str;
    m_editHeightOffset.GetWindowText(str);
    float fVal = (float)::atof(str);
    pMagicParpet->SetOffset(fVal);
}

void KSceneSFXEditorMagicParpet::OnBnClickedButtonDelParpet()
{
    IEKG3DSFX* sfx = GetSfx();

    if (!sfx)
        return;

    sfx->RemoveMagicParpet(GetSelAoe());
    m_comboAoes.DeleteString(m_comboAoes.GetCurSel());
    
    if (sfx->GetMagicParpetNum() > 0)
        m_comboAoes.SetCurSel(0);

    UpdateUI();

}

void KSceneSFXEditorMagicParpet::OnBnClickedButtonBrowseTexture()
{
    GET_SFX_EDITOR();

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
        SetDlgItemText(IDC_EDIT_TEXTURE_NAME, fileDlg.GetPathName());
        
        IEKG3DSFXMagicParpet* aoe = GetSelAoe();

        if (!aoe)
            return;

        aoe->SetTexture(fileDlg.GetPathName().GetBuffer());
        fileDlg.GetPathName().ReleaseBuffer();

        return;
    }
}

void KSceneSFXEditorMagicParpet::OnBnClickedAddFootprint()
{
	// TODO: Add your control notification handler code here
	GET_SFX_EDITOR();

	HRESULT hr = E_FAIL;
	IEKG3DSFX *pSFX = NULL;
	 pScene->GetSFX(&pSFX);
	 if (!pSFX)
		return ; 
	IEKG3DSFXSelectMark *pSelectMark = NULL;
	pSFX->AddSelectMark(&pSelectMark);
	UpdateFootprintProperty();
}

void KSceneSFXEditorMagicParpet::OnBnClickedDelFootprint()
{
	// TODO: Add your control notification handler code here
	GET_SFX_EDITOR();
	IEKG3DSFX *pSFX = NULL;
	pScene->GetSFX(&pSFX);
	if (!pSFX)
		return ; 
	pSFX->RemoveSelectMark();
	UpdateFootprintProperty();
}

void KSceneSFXEditorMagicParpet::OnBnClickedBrowseTexture()
{
	// TODO: Add your control notification handler code here
	GET_SFX_EDITOR();

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
		SetDlgItemText(IDC_TEXTURE_NAME, fileDlg.GetPathName());
		IEKG3DSFX *pSFX = NULL;
		pScene->GetSFX(&pSFX);
		if (!pSFX)
			return ; 
		IEKG3DSFXSelectMark* pFootPrint = pSFX->GetSelectMark();
		if (!pFootPrint)
			return;

		TCHAR szRetPath[MAX_PATH];
		g_GetFilePathFromFullPath(szRetPath, fileDlg.GetPathName().GetBuffer());

		pFootPrint->LoadTexture(szRetPath);
		UpdateFootprintProperty();
		return;
	}
}

void KSceneSFXEditorMagicParpet::UpdateFootprintProperty()
{
	GET_SFX_EDITOR();
	IEKG3DTexture *pTexture = NULL;
	LPCTSTR pszName = NULL;
	IEKG3DSFX *pSFX = NULL;
	pScene->GetSFX(&pSFX);
	IEKG3DSFXSelectMark *pSelectMark = pSFX->GetSelectMark();
	m_ComboFootprintType.EnableWindow(FALSE);
	if (!pSelectMark)
	{
		m_ButtonAddFootprint.EnableWindow();
		
		m_Button_DelFootprint.EnableWindow(FALSE);
		m_editFPTextureName.EnableWindow(FALSE);
		m_EditFootWidth.EnableWindow(FALSE);
		m_Button_Browse.EnableWindow(FALSE);
	//	m_ComboFootprintType.EnableWindow(FALSE);
		/*m_EditOffsetY.EnableWindow(FALSE);*/
		return;
	}

	m_ButtonAddFootprint.EnableWindow(FALSE);

	m_Button_DelFootprint.EnableWindow();
	m_editFPTextureName.EnableWindow();
	m_EditFootWidth.EnableWindow();

	m_Button_Browse.EnableWindow();
//	m_ComboFootprintType.EnableWindow();
	/*m_EditOffsetY.EnableWindow();*/

	CString str;
	//str.Format("%f", pMagicParpet->GetGridSize());
	//m_editGridSize.SetWindowText(str);
	str.Format("%d", pSelectMark->GetTextureWidth());
	m_EditFootWidth.SetWindowText(str);


	pTexture = pSelectMark->GetTexture();
	if (pTexture)
	{
		pTexture->GetName(&pszName);
		str.Format("%s", pszName);
	}
	else
		str.Format("%s", TEXT(""));
	m_editFPTextureName.SetWindowText(str);
}

void KSceneSFXEditorMagicParpet::OnSetFootWidth()
{
	// TODO: Add your control notification handler code here
	GET_SFX_EDITOR();
	IEKG3DSFX *pSFX = NULL;
	pScene->GetSFX(&pSFX);
	IEKG3DSFXSelectMark *pSelectMark = pSFX->GetSelectMark();
	if (!pSelectMark)
		return;

	CString str;
	m_EditFootWidth.GetWindowText(str);
	DWORD fVal = (DWORD)::atoi(str);
	pSelectMark->SetTextureWidth(fVal);
	pSelectMark->SetTextureHeight(fVal);
}

void KSceneSFXEditorMagicParpet::OnSetLife()
{
	GET_SFX_EDITOR();
	IEKG3DSFX *pSFX = NULL;
	pScene->GetSFX(&pSFX);
	IEKG3DSFXMagicParpet *pMagicParpet = pSFX->GetMagicParpet(0);
	if (!pMagicParpet)
		return;

	CString str;
	m_editLife.GetWindowText(str);
	float fVal = (float)::atof(str);
	pMagicParpet->SetLife(fVal);
}

IEKG3DSFXMagicParpet* KSceneSFXEditorMagicParpet::GetSelAoe()
{
    int nSel = m_comboAoes.GetCurSel();

    if (nSel == CB_ERR)
        return NULL;

    return (IEKG3DSFXMagicParpet*)m_comboAoes.GetItemData(nSel);
}

void KSceneSFXEditorMagicParpet::OnCbnSelchangeCombo5()
{
    IEKG3DSFXMagicParpet* aoe = GetSelAoe();
    
    if (!aoe)
        return;

    int nSel = m_comboType.GetCurSel();

    switch (nSel)
    {
    case 0 :
        aoe->SetType(AOE_NORMAL, false);
        break;
    case 1 :
        aoe->SetType(AOE_PROJECT, false);
        break;
    case 2 :
        aoe->SetType(AOE_FOOTPRINT, false);
        break;
    case 3 :
        aoe->SetType(AOE_SELECTOR, false);
        break;
    default :
        return;
        break;
    }

    UpdateUI();
}

void KSceneSFXEditorMagicParpet::OnCbnSelchangeCombo1()
{
      int nSel = m_comboAoes.GetCurSel();

      if (nSel == CB_ERR)
          return;

      SetCurrTimeLine(NULL, "", SFX_TLDT_NONE);

      UpdateUI();
}

void KSceneSFXEditorMagicParpet::OnCbnSelchangeCombo6()
{
    IEKG3DSFXMagicParpet* aoe = GetSelAoe();

    if (!aoe)
        return;

    int nSel = m_comboFace.GetCurSel();

    switch (nSel)
    {
    case 0 :
        aoe->SetFaceType(AOE_FACE_NONE);
        break;
    case 1 :
        aoe->SetFaceType(AOE_FACE_EYES);
        break;
    case 2 :
        aoe->SetFaceType(AOE_FACE_PLAYER);
        break;
    default :
        return;
        break;
    }

    UpdateUI();
}


void KSceneSFXEditorMagicParpet::OnStnClickedTlColor()
{

    
}

void KSceneSFXEditorMagicParpet::OnBnClickedTlColor()
{
    IEKG3DSFXMagicParpet* aoe = GetSelAoe();

    if (!aoe)
        return;

    int nFrame = GetCurrKeyFrame();
    D3DXCOLOR color;
    aoe->GetColorsTimeLine()->GetData((float)nFrame, &color);
    SetCurrTimeLine(aoe->GetColorsTimeLine(), "aoe颜色", SFX_TLDT_D3DXCOLOR);
    m_stColor.SetBKColor(color);
}

void KSceneSFXEditorMagicParpet::OnEnSetfocusEditGridNum()
{
    IEKG3DSFXMagicParpet* aoe = GetSelAoe();

    if (!aoe)
        return;

    int nFrame = GetCurrKeyFrame();
    int radius;
    char text[12];
    aoe->GetRadiusTimeLine()->GetData((float)nFrame, &radius);
    sprintf(text, "%d", radius);
    m_editGridNum.SetWindowText(text);
    SetCurrTimeLine(aoe->GetRadiusTimeLine(), "aoe半径", SFX_TLDT_INT);
}

void KSceneSFXEditorMagicParpet::OnCbnSelchangeComboFootprinttype()
{
    // TODO: Add your control notification handler code here
}
