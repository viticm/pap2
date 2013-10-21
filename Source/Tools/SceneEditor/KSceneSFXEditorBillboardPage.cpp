////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSFXEditorBillboardPage.cpp
//  Version     : 1.0
//  Creator     : Zeb
//  Create Date : 2005-12-1 16:07:57
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KSceneSFXEditorBillboardPage.h"
#include "IEKG3DSceneSFXEditor.h"
//#include "KG3DSceneEditorBase.h"
#include "KG3DInterface.h"
#include "IEKG3DTexture.h"

////////////////////////////////////////////////////////////////////////////////

// KSceneSFXEditorBillboardPage.cpp : implementation file
//

#include "SceneEditor.h"
//(by dengzhihui 2006年7月5日 15:52:26
//#include "MainFrm.h"
#include "KColorDialog.h"
//)
#include "KSceneSFXEditorFrame.h"
#include "IEKG3DSceneSFXEditor.h"
#include "IEKG3DSFX.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


#define BILLBOARD_BLEND_MODE_SRCALPHA_INVSRCALPHA 0
#define BILLBOARD_BLEND_MODE_SRCALPHA_ONE         1
#define BILLBOARD_BLEND_MODE_SRCALPHA_INV         2

// KSceneSFXEditorBillboardPage dialog

IMPLEMENT_DYNAMIC(KSceneSFXEditorBillboardPage, CDialog)
KSceneSFXEditorBillboardPage::KSceneSFXEditorBillboardPage(CWnd* pWnd)
	: CDialog(KSceneSFXEditorBillboardPage::IDD, pWnd)
{
}

KSceneSFXEditorBillboardPage::~KSceneSFXEditorBillboardPage()
{
}

void KSceneSFXEditorBillboardPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_COMBO_BILLBOARD_LIST, m_comboBillboard);
	DDX_Control(pDX, IDC_EDIT_BILLBOARD_NAME, m_editBillboardName);
    DDX_Control(pDX, IDC_BUTTON_ADD_BILLBOARD, m_buttonAddBillboard);
    DDX_Control(pDX, IDC_BUTTON_DEL_BILLBOARD, m_buttonDelBillboard);

    DDX_Control(pDX, IDC_COMBO_BLEND_MODE, m_comboBlendMode);
    DDX_Control(pDX, IDC_COMBO_TYPE, m_comboType);

    DDX_Control(pDX, IDC_RADIO_BILLBOARD_WIDTH, m_radioWidth);
    DDX_Control(pDX, IDC_EDIT_BILLBOARD_WIDTH, m_editWidth);

    DDX_Control(pDX, IDC_RADIO_BILLBOARD_HEIGHT, m_radioHeight);
    DDX_Control(pDX, IDC_EDIT_BILLBOARD_HEIGHT, m_editHeight);

    DDX_Control(pDX, IDC_RADIO_BILLBOARD_COLOR, m_radioColor);
    DDX_Control(pDX, IDC_BILLBOARD_COLOR_PIC, m_picColor);

    DDX_Control(pDX, IDC_RADIO_BILLBOARD_TEXTURE_INDEX, m_radioTextureFrameIndex);
    DDX_Control(pDX, IDC_EDIT_BILLBOARD_TEXTURE_INDEX, m_editTextureFrameIndex);

    DDX_Control(pDX, IDC_RADIO_BILLBOARD_TEXTURE2_INDEX, m_radioTexture2FrameIndex);
    DDX_Control(pDX, IDC_EDIT_BILLBOARD_TEXTURE2_INDEX, m_editTexture2FrameIndex);

    DDX_Control(pDX, IDC_CHECK_ROTATION_ANIMATION, m_checkRotationAnimation);
    DDX_Control(pDX, IDC_RADIO_ROTATION_ANIMATION, m_radioRotationAnimation);
    DDX_Control(pDX, IDC_EDIT_ROTATION_ANIMATION, m_editRotationAnimation);

    DDX_Control(pDX, IDC_CHECK_ADJUST_AXIS, m_checkSpecialAdjustAxis);

    DDX_Control(pDX, IDC_RADIO_UV_OFFSET, m_radioUVOffset);
    DDX_Control(pDX, IDC_EDIT_U_OFFSET, m_editOffsetU);
    DDX_Control(pDX, IDC_EDIT_V_OFFSET, m_editOffsetV);

    DDX_Control(pDX, IDC_EDIT_BILLBOARD_TEXTURE_NAME, m_editTextureName);
    DDX_Control(pDX, IDC_BUTTON_BILLBOARD_BROWSE_TEXTURE_NAME, m_buttonBrowseTextureName);

    DDX_Control(pDX, IDC_EDIT_BILLBOARD_TEXTURE2_NAME, m_editTexture2Name);
    DDX_Control(pDX, IDC_BUTTON_BILLBOARD_BROWSE_TEXTURE2_NAME, m_buttonBrowseTexture2Name);

    DDX_Control(pDX, IDC_EDIT_BILLBOARD_TEXTURE_CUT, m_editTextureCut);
    DDX_Control(pDX, IDC_EDIT_BILLBOARD_TEXTURE2_CUT, m_editTexture2Cut);
}


BEGIN_MESSAGE_MAP(KSceneSFXEditorBillboardPage, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_ADD_BILLBOARD, OnBnClickedButtonAddBillboard)
    ON_BN_CLICKED(IDC_BUTTON_DEL_BILLBOARD, OnBnClickedButtonDelBillboard)
    ON_CBN_SELCHANGE(IDC_COMBO_BILLBOARD_LIST, OnCbnSelchangeComboBillboardList)
    ON_CBN_DROPDOWN(IDC_COMBO_BILLBOARD_LIST, OnCbnDropdownComboBillboardList)
    ON_BN_CLICKED(IDC_RT, OnCheckRenderTarget)

    ON_BN_CLICKED(IDC_RADIO_BILLBOARD_WIDTH, OnBnClickedRadioBillboardWidth)
    ON_BN_CLICKED(IDC_RADIO_BILLBOARD_HEIGHT, OnBnClickedRadioBillboardHeight)
    ON_BN_CLICKED(IDC_RADIO_BILLBOARD_COLOR, OnBnClickedRadioBillboardColor)
    ON_BN_CLICKED(IDC_RADIO_BILLBOARD_TEXTURE_INDEX, OnBnClickedRadioBillboardTextureIndex)
    ON_STN_CLICKED(IDC_BILLBOARD_COLOR_PIC, OnStnClickedBillboardColorPic)
    ON_BN_CLICKED(IDC_CHECK_ROTATION_ANIMATION, OnBnClickedCheckRotationAnimation)
    ON_BN_CLICKED(IDC_CHECK_ADJUST_AXIS, OnBnClickedCheckAdjustAxis)
    ON_BN_CLICKED(IDC_RADIO_UV_OFFSET, OnBnClickedRadioUvOffset)
    ON_BN_CLICKED(IDC_RADIO_ROTATION_ANIMATION, OnBnClickedRadioRotationAnimation)
    ON_BN_CLICKED(IDC_RADIO_BILLBOARD_TEXTURE2_INDEX, OnBnClickedRadioBillboardTexture2Index)
    ON_BN_CLICKED(IDC_BUTTON_BILLBOARD_BROWSE_TEXTURE2_NAME, OnBnClickedButtonBillboardBrowseTexture2Name)
    ON_BN_CLICKED(IDC_BUTTON_BILLBOARD_BROWSE_TEXTURE_NAME, OnBnClickedButtonBillboardBrowseTextureName)
    ON_EN_SETFOCUS(IDC_EDIT_BILLBOARD_WIDTH, &KSceneSFXEditorBillboardPage::OnEnSetfocusEditBillboardWidth)
    ON_EN_SETFOCUS(IDC_EDIT_BILLBOARD_HEIGHT, &KSceneSFXEditorBillboardPage::OnEnSetfocusEditBillboardHeight)
    ON_EN_SETFOCUS(IDC_EDIT_BILLBOARD_TEXTURE_INDEX, &KSceneSFXEditorBillboardPage::OnEnSetfocusEditBillboardTextureIndex)
    ON_EN_SETFOCUS(IDC_EDIT_BILLBOARD_TEXTURE2_INDEX, &KSceneSFXEditorBillboardPage::OnEnSetfocusEditBillboardTexture2Index)
    ON_EN_SETFOCUS(IDC_EDIT_ROTATION_ANIMATION, &KSceneSFXEditorBillboardPage::OnEnSetfocusEditRotationAnimation)
    ON_EN_SETFOCUS(IDC_EDIT_U_OFFSET, &KSceneSFXEditorBillboardPage::OnEnSetfocusEditUOffset)
    ON_EN_SETFOCUS(IDC_EDIT_V_OFFSET, &KSceneSFXEditorBillboardPage::OnEnSetfocusEditVOffset)
    ON_CBN_SELCHANGE(IDC_COMBO_BLEND_MODE, &KSceneSFXEditorBillboardPage::OnCbnSelchangeComboBlendMode)
    ON_CBN_SELCHANGE(IDC_COMBO_TYPE, &KSceneSFXEditorBillboardPage::OnCbnSelchangeComboType)
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


HBRUSH KSceneSFXEditorBillboardPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

    if (pWnd == &m_picColor)
    {
        GET_SFX_EDITOR_RET(hBursh);
        GET_BILLBOARD_RET(hBursh);
        int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
        UpdateBillboardColor(pBillboard, nFrame);
    }

    return hBursh;
}

// KSceneSFXEditorBillboardPage message handlers

void KSceneSFXEditorBillboardPage::UpdateBillboardList()
{
    GET_SFX_EDITOR();

	int nSelect = m_comboBillboard.GetCurSel();
	m_comboBillboard.ResetContent();

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
	DWORD dwCount = pSFX->GetBillboardNum();
    SFX_ENTITY_DATA *pEntityData = NULL;
#ifdef _DEBUG_
    {
        DWORD dwSceneBillboardCount = 0;
        pScene->GetSceneBillboardEntityDataCount(&dwSceneBillboardCount);
        ASSERT(dwCount = dwSceneBillboardCount);
    }
#endif
	for (DWORD i = 0; i < dwCount; ++i)
	{
        pScene->GetBillboardEntityData(i, &pEntityData);
        if (pEntityData->szEntityName[0] == '\0')
        {
            pScene->SetBillboardEntityDataNameToDefault(pEntityData);
        }

		m_comboBillboard.InsertString(-1, pEntityData->szEntityName);
	}

	m_comboBillboard.SetCurSel(nSelect);
}

void KSceneSFXEditorBillboardPage::OnCheckRenderTarget()
{
    GET_SFX_EDITOR();
    GET_BILLBOARD();
    if (((CButton*)GetDlgItem(IDC_RT))->GetCheck())
    {
        pBillboard->SetRenderTargetTexFlag(TRUE);
    }
    else
    {
        pBillboard->SetRenderTargetTexFlag(TRUE);
    }
}

void KSceneSFXEditorBillboardPage::UpdateBillboardSelected()
{
    HRESULT hRetCode = E_FAIL;
    SCENEENTITYTYPE EntityType = SCENEENTITY_NONE;
    VOID *pvEntityPointer = NULL;
    IEKG3DSFX *pSFX = NULL;
    IEKG3DSceneEditorBase *piSceneEditorBase = NULL;

    GET_SFX_EDITOR();

    hRetCode = pScene->GetSFX(&pSFX);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = pScene->GetSceneEditBase(&piSceneEditorBase);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

	IEKG3DSceneSelectBase* pSelector = NULL;
	piSceneEditorBase->GetSceneSelector(&pSelector);
	_ASSERTE(pSelector);

	IEKG3DSceneSelectionTool* pSelTool = NULL;
	piSceneEditorBase->GetSelectionToolInterface(&pSelTool);
	_ASSERTE(NULL != pSelTool);
	if(NULL == pSelTool)
		return;

	//if (pSelector->GetSelectedEntitySize() == 1)
	if(1 == pSelTool->GetSelectionCount())
    {
		IEKG3DSceneSelectionToolAccessProxy* pProxy = NULL;
		pSelTool->GetAccessProxy(&pProxy);
		_ASSERTE(NULL != pProxy);
		if(NULL == pProxy)
			return;

		IEKG3DSceneEntity* pEntity = pProxy->Reset();
		for (ULONG i = 0; NULL != pEntity && i < pSelTool->GetSelectionCount(); ++i, pEntity = pProxy->Next())
		//for(INT i = 0; i < pSelector->GetSelectedEntitySize(); ++i)
        {
			/*IEKG3DSceneEntity* pEntity = NULL;
			pSelector->GetSelectedEntity(i, &pEntity);
			_ASSERTE(pEntity);*/

            IEKG3DSFXBillboard *pBillboard = NULL;
            DWORD dwIndex = pSFX->GetBillboardNum();
            for (DWORD i = 0; i < dwIndex; ++i)
            {
                pSFX->GetBillboard(i, &pBillboard);
                ASSERT(pBillboard);

                hRetCode = pEntity->GetPointer(&pvEntityPointer);
                KGLOG_COM_PROCESS_ERROR(hRetCode);

                if ((VOID *)pBillboard == pvEntityPointer)
                {
                    hRetCode = pEntity->GetType(&EntityType);
                    KGLOG_COM_PROCESS_ERROR(hRetCode);

                    KG_ASSERT_EXIT(EntityType == SCENEENTITY_SFX_BILLBOARD);
                    m_comboBillboard.SetCurSel(i);
                    UpdateBillboardList();
                    return;
                }
            }

            //++it;
        }
    }
    else
    {
        m_comboBillboard.SetCurSel(-1);
        UpdateBillboardUI();
        return;
    }
Exit0:
    return;
}

void KSceneSFXEditorBillboardPage::UpdateBillboardName(IEKG3DSceneSFXEditor *pScene, int nSelect)
{
    HRESULT hRetCode = E_FAIL;
    SFX_ENTITY_DATA *pEntityData = NULL;
    hRetCode = pScene->GetBillboardEntityData((DWORD)nSelect, &pEntityData);
    KGLOG_COM_PROCESS_ERROR(hRetCode);
    m_editBillboardName.SetWindowText(pEntityData->szEntityName);

Exit0:
    return;
}

void KSceneSFXEditorBillboardPage::UpdateBillboardColor(IEKG3DSFXBillboard *pBillboard, int nFrame)
{
    D3DXCOLOR color;
    pBillboard->DiffuseTimeLineGetData(&color,(float) nFrame);
    DWORD dwColor = (DWORD)color;

    CRect rect;
    m_picColor.GetClientRect (rect);
    CClientDC dc(&m_picColor);
    CBrush Brush(RGB(D3DCOLOR_GETRED(dwColor), D3DCOLOR_GETGREEN(dwColor), D3DCOLOR_GETBLUE(dwColor)));
    CBrush* pOldBrush = dc.SelectObject(&Brush);
    dc.Rectangle(&rect);
    dc.SelectObject(pOldBrush);
}

void KSceneSFXEditorBillboardPage::UpdateBillboardWidth(IEKG3DSFXBillboard *pBillboard, int nFrame)
{
    float fWidth;
    pBillboard->WidthTimeLineGetData(&fWidth, (float)nFrame);
    CString str;
    str.Format("%.3f", fWidth);
    m_editWidth.SetWindowText(str);
}

void KSceneSFXEditorBillboardPage::UpdateBillboardHeight(IEKG3DSFXBillboard *pBillboard, int nFrame)
{
    float fHeight;
    pBillboard->HeightTimeLineGetData(&fHeight, (float)nFrame);
    CString str;
    str.Format("%.3f", fHeight);
    m_editHeight.SetWindowText(str);
}

void KSceneSFXEditorBillboardPage::UpdateTextureFrameIndex1(IEKG3DSFXBillboard *pBillboard, int nFrame)
{
    if (!pBillboard || !pBillboard->GetTexture())
    {
        m_radioTextureFrameIndex.EnableWindow(FALSE);
        m_editTextureFrameIndex.EnableWindow(FALSE);
        return;
    }

    m_editTextureFrameIndex.EnableWindow();

    int nIndex;
    pBillboard->TextureFrameIndexGetData(&nIndex, (float)nFrame);
    CString str;
    str.Format("%d", nIndex);
    m_editTextureFrameIndex.SetWindowText(str);
}

void KSceneSFXEditorBillboardPage::UpdateTextureFrameIndex2(IEKG3DSFXBillboard *pBillboard, int nFrame)
{
    if (!pBillboard || !pBillboard->GetTexture2())
    {
        m_radioTexture2FrameIndex.EnableWindow(FALSE);
        m_editTexture2FrameIndex.EnableWindow(FALSE);
        return;
    }

    m_editTexture2FrameIndex.EnableWindow();
    int nIndex;
    pBillboard->Texture2FrameIndexGetData(&nIndex, (float)nFrame);
    CString str;
    str.Format("%d", nIndex);
    m_editTexture2FrameIndex.SetWindowText(str);
}

void KSceneSFXEditorBillboardPage::UpdateRotation(IEKG3DSFXBillboard *pBillboard, int nFrame)
{
    //ASSERT(false);
    //if (!pBillboard->IsRotation())
    //{
    //    m_checkRotationAnimation.SetCheck(FALSE);
    //    m_radioRotationAnimation.EnableWindow(FALSE);
    //    m_editRotationAnimation.EnableWindow(FALSE);
    //    return;
    //}

    //m_checkRotationAnimation.SetCheck(TRUE);

    //float fRotation;
    //pBillboard->RotationTimeLineGetData(&fRotation, (float)nFrame);
    //CString str;
    //str.Format("%d", (DWORD)(fRotation / D3DX_PI * 180));
    //m_editRotationAnimation.SetWindowText(str);
}

void KSceneSFXEditorBillboardPage::UpdateUV(IEKG3DSFXBillboard *pBillboard, int nFrame)
{
    D3DXVECTOR2 vUV;
    pBillboard->UVTimeLineGetData(&vUV, (float)nFrame);
    CString str;
    str.Format("%.3f", vUV.x);
    m_editOffsetU.SetWindowText(str);
    str.Format("%.3f", vUV.y);
    m_editOffsetV.SetWindowText(str);
}

void KSceneSFXEditorBillboardPage::UpdateTextureName1(IEKG3DSFXBillboard *pBillboard)
{
    IEKG3DTexture *pTexture = NULL;
    LPCTSTR pszName = NULL;
    if (!pBillboard || !pBillboard->GetTexture())
    {
        m_editTextureName.EnableWindow(FALSE);
        return;
    }

    m_editTextureName.EnableWindow();
    pTexture = pBillboard->GetTexture();
    pTexture->GetName(&pszName);
    m_editTextureName.SetWindowText(pszName);
}

void KSceneSFXEditorBillboardPage::UpdateTextureName2(IEKG3DSFXBillboard *pBillboard)
{
    IEKG3DTexture *pTexture = NULL;
    LPCTSTR pszName = NULL;
    if (!pBillboard || !pBillboard->GetTexture2())
    {
        m_editTexture2Name.EnableWindow(FALSE);
        return;
    }

    m_editTexture2Name.EnableWindow();
    pTexture = pBillboard->GetTexture2();
    pTexture->GetName(&pszName);
    m_editTexture2Name.SetWindowText(pszName);
}

void KSceneSFXEditorBillboardPage::UpdateTextureCut1(IEKG3DSFXBillboard *pBillboard)
{
    if (!pBillboard || !pBillboard->GetTexture())
    {
        m_editTextureCut.EnableWindow(FALSE);
        return;
    }

    m_editTextureCut.EnableWindow();
    CString str;
    str.Format("%d", pBillboard->GetTextureCutNum());
    m_editTextureCut.SetWindowText(str);
}

void KSceneSFXEditorBillboardPage::UpdateTextureCut2(IEKG3DSFXBillboard *pBillboard)
{
    if (!pBillboard || !pBillboard->GetTexture2())
    {
        m_editTexture2Cut.EnableWindow(FALSE);
        return;
    }

    m_editTexture2Cut.EnableWindow();
    CString str;
    str.Format("%d", pBillboard->GetTexture2CutNum());
    m_editTexture2Cut.SetWindowText(str);
}

void KSceneSFXEditorBillboardPage::UpdateBillboardAjustAxis(IEKG3DSFXBillboard *pBillboard)
{
    //ASSERT(false);
    //if (pBillboard->IsLockAxis())
    //    m_checkSpecialAdjustAxis.SetCheck(TRUE);
    //else
    //    m_checkSpecialAdjustAxis.SetCheck(FALSE);
}

void KSceneSFXEditorBillboardPage::UpdateBillboardUI()
{
    GET_SFX_EDITOR();

    int nSelect = m_comboBillboard.GetCurSel();
    if (nSelect == CB_ERR)
    {
        m_editBillboardName.EnableWindow(FALSE);
        m_checkSpecialAdjustAxis.EnableWindow(FALSE);
        m_comboBlendMode.EnableWindow(FALSE);
        m_comboType.EnableWindow(FALSE);
        m_radioWidth.EnableWindow(FALSE);
        m_editWidth.EnableWindow(FALSE);
        m_radioHeight.EnableWindow(FALSE);
        m_editHeight.EnableWindow(FALSE);
        m_radioColor.EnableWindow(FALSE);
        m_picColor.EnableWindow(FALSE);
        m_radioTextureFrameIndex.EnableWindow(FALSE);
        m_editTextureFrameIndex.EnableWindow(FALSE);
        m_radioTexture2FrameIndex.EnableWindow(FALSE);
        m_editTexture2FrameIndex.EnableWindow(FALSE);
        m_checkRotationAnimation.EnableWindow(FALSE);
        m_radioRotationAnimation.EnableWindow(FALSE);
        m_editRotationAnimation.EnableWindow(FALSE);
        m_radioUVOffset.EnableWindow(FALSE);
        m_editOffsetU.EnableWindow(FALSE);
        m_editOffsetV.EnableWindow(FALSE);
        m_editTextureName.EnableWindow(FALSE);
        m_editTexture2Name.EnableWindow(FALSE);
        m_buttonBrowseTextureName.EnableWindow(FALSE);
        m_buttonBrowseTexture2Name.EnableWindow(FALSE);
        m_editTextureCut.EnableWindow(FALSE);
        m_editTexture2Cut.EnableWindow(FALSE);
        return;
    }
    else
    {
        m_editBillboardName.EnableWindow();
        m_checkSpecialAdjustAxis.EnableWindow();
        m_comboBlendMode.EnableWindow();
        m_comboType.EnableWindow();
        m_radioWidth.EnableWindow();
        m_editWidth.EnableWindow();
        m_radioHeight.EnableWindow();
        m_editHeight.EnableWindow();
        m_radioColor.EnableWindow();
        m_picColor.EnableWindow();
        m_radioTextureFrameIndex.EnableWindow();
        m_editTextureFrameIndex.EnableWindow();
        m_radioTexture2FrameIndex.EnableWindow();
        m_editTexture2FrameIndex.EnableWindow();
        m_checkRotationAnimation.EnableWindow();
        m_radioRotationAnimation.EnableWindow();
        m_editRotationAnimation.EnableWindow();
        m_radioUVOffset.EnableWindow();
        m_editOffsetU.EnableWindow();
        m_editOffsetV.EnableWindow();
        m_editTextureName.EnableWindow();
        m_editTexture2Name.EnableWindow();
        m_buttonBrowseTextureName.EnableWindow();
        m_buttonBrowseTexture2Name.EnableWindow();
        m_editTextureCut.EnableWindow();
        m_editTexture2Cut.EnableWindow();
    }

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    IEKG3DSFXBillboard *pBillboard = NULL;
    pSFX->GetBillboard(nSelect, &pBillboard);
    ASSERT(pBillboard);
    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();

    UpdateBlendMode(pBillboard);
    UpdateBillboardName(pScene, nSelect);
    UpdateBillboardColor(pBillboard, nFrame);
    UpdateBillboardWidth(pBillboard, nFrame);
    UpdateBillboardHeight(pBillboard, nFrame);
    UpdateTextureFrameIndex1(pBillboard, nFrame);
    UpdateTextureFrameIndex2(pBillboard, nFrame);
    UpdateRotation(pBillboard, nFrame);
    UpdateUV(pBillboard, nFrame);
    UpdateTextureName1(pBillboard);
    UpdateTextureName2(pBillboard);
    UpdateTextureCut1(pBillboard);
    UpdateTextureCut2(pBillboard);
    UpdateType(pBillboard);
}

void KSceneSFXEditorBillboardPage::UpdateSpecifiedUI()
{
    GET_SFX_EDITOR();
    int nSelect = m_comboBillboard.GetCurSel();
    KG_PROCESS_ERROR(nSelect != -1);

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    IEKG3DSFXBillboard *pBillboard = NULL;
    pSFX->GetBillboard(nSelect, &pBillboard);
    ASSERT(pBillboard);
    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();

    if (m_radioWidth.GetCheck())
        UpdateBillboardWidth(pBillboard, nFrame);
    else if (m_radioHeight.GetCheck())
        UpdateBillboardHeight(pBillboard, nFrame);
    else if (m_radioColor.GetCheck())
        UpdateBillboardColor(pBillboard, nFrame);
    else if (m_radioTextureFrameIndex.GetCheck())
        UpdateTextureFrameIndex1(pBillboard, nFrame);
    else if (m_radioTexture2FrameIndex.GetCheck())
        UpdateTextureFrameIndex2(pBillboard, nFrame);
    else if (m_radioRotationAnimation.GetCheck())
        UpdateRotation(pBillboard, nFrame);
    else if (m_radioUVOffset.GetCheck())
        UpdateUV(pBillboard, nFrame);
Exit0:
    return;
}

void KSceneSFXEditorBillboardPage::UpdateUI()
{
	UpdateBillboardList();
    UpdateBillboardSelected();
    UpdateBillboardUI();
}

void KSceneSFXEditorBillboardPage::OnBnClickedButtonAddBillboard()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();

    pScene->AddBillboard(0, 0, 0);
    UpdateBillboardList();
    UpdateBillboardUI();
}

void KSceneSFXEditorBillboardPage::OnBnClickedButtonDelBillboard()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();

    int nSelect = m_comboBillboard.GetCurSel();
    if (nSelect == CB_ERR)
        return;

	if (SUCCEEDED(pScene->RemoveBillboard(nSelect)))
    {
        UpdateBillboardList();
        m_comboBillboard.SetCurSel(-1);
        UpdateBillboardUI();
    }
}

void KSceneSFXEditorBillboardPage::OnCbnSelchangeComboBillboardList()
{
    HRESULT hRetCode = E_FAIL;
    VOID *pvEntityPointer = NULL;


    // TODO: Add your control notification handler code here
    UpdateBillboardUI();

    GET_SFX_EDITOR();
    GET_BILLBOARD();

	IEKG3DSceneSelectionTool* pSelTool = NULL;
	HRESULT hr = pScene->GetSelectionToolInterface(&pSelTool);
	_ASSERTE(SUCCEEDED(hr));

    IEKG3DSceneEditorBase *piSceneEditorBase = NULL;
    pScene->GetSceneEditBase(&piSceneEditorBase);
	IEKG3DSceneSelectBase* pSelector = NULL;
	piSceneEditorBase->GetSceneSelector(&pSelector);
	_ASSERTE(pSelector);

	//pSelector->SelectNone(0);
	pSelTool->ClearSelection();

	pSelector->CopySelectable2SelectedSet(pBillboard);
    pFrame->GetKeysFrame()->UpdateTransformUI();

    int id = GetCheckedRadioButton(IDC_RADIO_BILLBOARD_COLOR, IDC_RADIO_UV_OFFSET);
    CheckDlgButton(id, BST_UNCHECKED);
    //pFrame->m_dlgKeyFrame.SetTimeLine("", NULL, SFX_TLDT_NONE);
    pFrame->GetKeysFrame()->SetTimeLine("", NULL, SFX_TLDT_NONE);
//Exit0:
    return;
}

void KSceneSFXEditorBillboardPage::OnCbnDropdownComboBillboardList()
{
    // TODO: Add your control notification handler code here
    UpdateBillboardList();
}

void KSceneSFXEditorBillboardPage::OnBnClickedRadioBillboardWidth()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_BILLBOARD();

    pFrame->GetKeysFrame()->SetTimeLine("公告板宽度", pBillboard->GetWidthTimeLine(), SFX_TLDT_FLOAT);
}

void KSceneSFXEditorBillboardPage::OnBnClickedRadioBillboardHeight()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_BILLBOARD();

    pFrame->GetKeysFrame()->SetTimeLine("公告板高度", pBillboard->GetHeightTimeLine(), SFX_TLDT_FLOAT);
}

void KSceneSFXEditorBillboardPage::OnBnClickedRadioBillboardColor()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_BILLBOARD();

    m_radioColor.SetFocus();
    pFrame->GetKeysFrame()->SetTimeLine("公告板颜色", pBillboard->GetDiffuseTimeLine(), SFX_TLDT_D3DXCOLOR);
}

void KSceneSFXEditorBillboardPage::OnBnClickedRadioBillboardTextureIndex()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_BILLBOARD();

    pFrame->GetKeysFrame()->SetTimeLine("公告板纹理1索引", pBillboard->GetTextureFrameIndex(), SFX_TLDT_INT);
}

void KSceneSFXEditorBillboardPage::OnStnClickedBillboardColorPic()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_BILLBOARD();

    CheckRadioButton(IDC_RADIO_BILLBOARD_COLOR, IDC_RADIO_UV_OFFSET, IDC_RADIO_BILLBOARD_COLOR);
    OnBnClickedRadioBillboardColor();

    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    pBillboard->DiffuseTimeLineGetData(&m_Color, (float)nFrame);

    //EnableWindow(FALSE);
    GetColorPickerDialog()->Init((D3DCOLORVALUE*)&m_Color, "粒子颜色", this, TRUE);
}

LRESULT KSceneSFXEditorBillboardPage::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    // TODO: Add your specialized code here and/or call the base class
    switch (message)
    {
    case WM_COLORPOINTER_CHANGE:
        {
            EnableWindow();
            GET_SFX_EDITOR_RET(-1);
            GET_BILLBOARD_RET(-1);
            int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
            pBillboard->DiffuseTimeLineInsertKeyFrame(nFrame, m_Color);
            //pFrame->m_dlgKeyFrame.UpdateSliderCtrl();
            pFrame->GetKeysFrame()->UpdateSliderCtrl();
            UpdateBillboardColor(pBillboard, nFrame);
        }
        break;

  /*  case WM_COLORPOINTER_CHANGE :
        {
            DWORD dwColor = (DWORD)wParam;
            CRect rect;
            m_picColor.GetClientRect (rect);
            CClientDC dc(&m_picColor);
            CBrush Brush(RGB(D3DCOLOR_GETRED(dwColor), D3DCOLOR_GETGREEN(dwColor), D3DCOLOR_GETBLUE(dwColor)));
            CBrush* pOldBrush = dc.SelectObject(&Brush);
            dc.Rectangle(&rect);
            dc.SelectObject(pOldBrush);
            
        }
        break;*/


    case WM_EDIT_RECEIVE_ENTER:
        {
            switch (wParam)
            {
            case IDC_EDIT_BILLBOARD_WIDTH:
                OnSetBillboardWidth();
                break;
            case IDC_EDIT_BILLBOARD_HEIGHT:
                OnSetBillboardHeight();
                break;
            case IDC_EDIT_BILLBOARD_TEXTURE_INDEX:
                OnSetBillboardTextureFrameIndex();
                break;
            case IDC_EDIT_BILLBOARD_TEXTURE2_INDEX:
                OnSetBillboardTexture2FrameIndex();
                break;
            case IDC_EDIT_V_OFFSET:
                OnSetVOffset();
                break;
            case IDC_EDIT_U_OFFSET:
                OnSetUOffset();
                break;
            case IDC_EDIT_ROTATION_ANIMATION:
                OnSetRotation();
                break;
            case IDC_EDIT_BILLBOARD_TEXTURE_CUT:
                OnSetTextureCutNum();
                break;
            case IDC_EDIT_BILLBOARD_TEXTURE2_CUT:
                OnSetTexture2CutNum();
                break;
            case IDC_EDIT_BILLBOARD_TEXTURE_NAME:
                OnSetTexture();
                break;
            case IDC_EDIT_BILLBOARD_TEXTURE2_NAME:
                OnSetTexture2();
                break;
			case IDC_EDIT_BILLBOARD_NAME:
				OnSetBillboardName();
				break;
            }
        }
    }

    return CDialog::WindowProc(message, wParam, lParam);
}

void KSceneSFXEditorBillboardPage::OnSetBillboardName()
{
    HRESULT hRetCode = E_FAIL;
    SFX_ENTITY_DATA *pEntityData = NULL;

    GET_SFX_EDITOR();

	int nSelect = m_comboBillboard.GetCurSel();
	if (nSelect != CB_ERR)
	{
        hRetCode = pScene->GetBillboardEntityData((DWORD)nSelect, &pEntityData);

        CString str;
		m_editBillboardName.GetWindowText(
            pEntityData->szEntityName,
            sizeof(pEntityData->szEntityName) / sizeof(pEntityData->szEntityName[0])
        );
		KGLOG_PROCESS_ERROR(pEntityData->szEntityName[0] != '\0');

		m_comboBillboard.DeleteString(nSelect);
		m_comboBillboard.InsertString(nSelect, pEntityData->szEntityName);
		m_comboBillboard.SetCurSel(nSelect);
	}
Exit0:
    return;
}

void KSceneSFXEditorBillboardPage::OnSetRotation()
{
    GET_SFX_EDITOR();
    GET_BILLBOARD();

    CString str;
    GetDlgItemText(IDC_EDIT_ROTATION_ANIMATION, str);
    int nValue = ::atoi(str);
    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    float fValue = (float)nValue / 180.0f * D3DX_PI;
    pBillboard->RotationTimeLineInsertKeyFrame(nFrame, fValue);

    //pFrame->m_dlgKeyFrame.UpdateSliderCtrl();
    pFrame->GetKeysFrame()->UpdateSliderCtrl();
}

void KSceneSFXEditorBillboardPage::OnSetTextureCutNum()
{
    GET_SFX_EDITOR();
    GET_BILLBOARD();

    UINT uCut = GetDlgItemInt(IDC_EDIT_BILLBOARD_TEXTURE_CUT);
    if (uCut < 1)
    {
        uCut = pBillboard->GetTextureCutNum();
        SetDlgItemInt(IDC_EDIT_BILLBOARD_TEXTURE_CUT, uCut);
        ::MessageBox(
            m_hWnd, TEXT("请输入一个大于等于1的整数!!"),
            TEXT("SFX ERROR"), MB_ICONWARNING | MB_OK
        );
    }
    else
        pBillboard->SetTextureCutNum(uCut);
}

void KSceneSFXEditorBillboardPage::OnSetTexture2CutNum()
{
    GET_SFX_EDITOR();
    GET_BILLBOARD();

    UINT uCut = GetDlgItemInt(IDC_EDIT_BILLBOARD_TEXTURE2_CUT);
    if (uCut < 1)
    {
        uCut = pBillboard->GetTexture2CutNum();
        SetDlgItemInt(IDC_EDIT_BILLBOARD_TEXTURE_CUT, uCut);
        ::MessageBox(
            m_hWnd, TEXT("请输入一个大于等于1的整数!!"),
            TEXT("SFX ERROR"), MB_ICONWARNING | MB_OK
        );
    }
    else
        pBillboard->SetTexture2CutNum(uCut);
}

void KSceneSFXEditorBillboardPage::OnSetUOffset()
{
    GET_SFX_EDITOR();
    GET_BILLBOARD();

    D3DXVECTOR2 vUV;
    CString str;
    GetDlgItemText(IDC_EDIT_U_OFFSET, str);
    float fValue = (float)::atof(str);
    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    pBillboard->UVTimeLineGetData(&vUV, (float)nFrame);
    vUV.x = fValue;
    pBillboard->UVTimeLineInsertKeyFrame(nFrame, vUV);

    //pFrame->m_dlgKeyFrame.UpdateSliderCtrl();
    pFrame->GetKeysFrame()->UpdateSliderCtrl();
}

void KSceneSFXEditorBillboardPage::OnSetVOffset()
{
    GET_SFX_EDITOR();
    GET_BILLBOARD();

    D3DXVECTOR2 vUV;
    CString str;
    GetDlgItemText(IDC_EDIT_V_OFFSET, str);
    float fValue = (float)::atof(str);
    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    pBillboard->UVTimeLineGetData(&vUV, (float)nFrame);
    vUV.y = fValue;
    pBillboard->UVTimeLineInsertKeyFrame(nFrame, vUV);

    //pFrame->m_dlgKeyFrame.UpdateSliderCtrl();
    pFrame->GetKeysFrame()->UpdateSliderCtrl();
}

void KSceneSFXEditorBillboardPage::OnSetBillboardWidth()
{
    GET_SFX_EDITOR();
    GET_BILLBOARD();

    CString str;
    GetDlgItemText(IDC_EDIT_BILLBOARD_WIDTH, str);
    float fValue = (float)::atof(str);
    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    pBillboard->WidthTimeLineInsertKeyFrame(nFrame, fValue);
    pBillboard->SetWidht(fValue);

    //pFrame->m_dlgKeyFrame.UpdateSliderCtrl();
    pFrame->GetKeysFrame()->UpdateSliderCtrl();
}

void KSceneSFXEditorBillboardPage::OnSetBillboardHeight()
{
    GET_SFX_EDITOR();
    GET_BILLBOARD();

    CString str;
    GetDlgItemText(IDC_EDIT_BILLBOARD_HEIGHT, str);
    float fValue =(float) ::atof(str);
    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    pBillboard->HeightTimeLineInsertKeyFrame(nFrame, fValue);
    pBillboard->SetHeight(fValue);

    //pFrame->m_dlgKeyFrame.UpdateSliderCtrl();
    pFrame->GetKeysFrame()->UpdateSliderCtrl();
}

void KSceneSFXEditorBillboardPage::OnSetBillboardTextureFrameIndex()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_BILLBOARD();

    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    UINT uValue = GetDlgItemInt(IDC_EDIT_BILLBOARD_TEXTURE_INDEX);
    DWORD dwCutNum = pBillboard->GetTextureCutNum();
    if (uValue < dwCutNum * dwCutNum)
        pBillboard->TextureFrameIndexInsertKeyFrame(nFrame, uValue);
    else
        MessageBox("错误的索引值", "ERROR");
    pFrame->GetKeysFrame()->UpdateSliderCtrl();
}

void KSceneSFXEditorBillboardPage::OnSetBillboardTexture2FrameIndex()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_BILLBOARD();

    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    UINT uValue = GetDlgItemInt(IDC_EDIT_BILLBOARD_TEXTURE2_INDEX);
    DWORD dwCutNum = pBillboard->GetTexture2CutNum();
    if (uValue < dwCutNum * dwCutNum)
        pBillboard->Texture2FrameIndexInsertKeyFrame(nFrame, uValue);
    else
        MessageBox("错误的索引值", "ERROR");
    pFrame->GetKeysFrame()->UpdateSliderCtrl();
}

bool KSceneSFXEditorBillboardPage::BrowseTexture(CString& strTextureName)
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR_RET(false);
    GET_BILLBOARD_RET(false);

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
        strTextureName = fileDlg.GetPathName();
        return true;
    }

    return false;
}

void KSceneSFXEditorBillboardPage::OnBnClickedButtonBillboardBrowseTextureName()
{
    // TODO: Add your control notification handler code here
    CString strTextureName;
    if(BrowseTexture(strTextureName))
    {
        GET_SFX_EDITOR();
        GET_BILLBOARD();

        if (SUCCEEDED(pBillboard->LoadTexture(strTextureName)))
        {
            m_editTextureName.SetWindowText(strTextureName);
            m_radioTextureFrameIndex.EnableWindow(TRUE);
            UpdateTextureFrameIndex1(pBillboard, pFrame->GetKeysFrame()->m_Slider.GetCurPos());

            m_editTextureCut.EnableWindow(TRUE);
        }
    }
}

void KSceneSFXEditorBillboardPage::OnBnClickedCheckRotationAnimation()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_BILLBOARD();

    if (m_checkRotationAnimation.GetCheck())
    {
       // pBillboard->EnableRotation(TRUE);
        //pBillboard->EnableLockAxis(FALSE);

        m_checkSpecialAdjustAxis.SetCheck(FALSE);
        m_radioRotationAnimation.EnableWindow();
        m_editRotationAnimation.EnableWindow();
    }
    else
    {
        //pBillboard->EnableRotation(FALSE);
        m_radioRotationAnimation.EnableWindow();
        m_editRotationAnimation.EnableWindow();
    }
}

void KSceneSFXEditorBillboardPage::OnBnClickedCheckAdjustAxis()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_BILLBOARD();

    if (m_checkSpecialAdjustAxis.GetCheck())
    {
        //pBillboard->EnableLockAxis(TRUE);
        //pBillboard->EnableRotation(FALSE);

        m_checkRotationAnimation.SetCheck(FALSE);
        m_radioRotationAnimation.SetCheck(FALSE);
        m_editRotationAnimation.EnableWindow(FALSE);
    }
    else
    {
       // pBillboard->EnableLockAxis(FALSE);
    }
}

void KSceneSFXEditorBillboardPage::OnBnClickedRadioUvOffset()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_BILLBOARD();

    pFrame->GetKeysFrame()->SetTimeLine("公告板纹UV偏移", pBillboard->GetUVTimeLine(), SFX_TLDT_VECTOR2);
}

void KSceneSFXEditorBillboardPage::OnBnClickedRadioRotationAnimation()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_BILLBOARD();

    pFrame->GetKeysFrame()->SetTimeLine("公告旋转", pBillboard->GetRotationTimeLine(), SFX_TLDT_FLOAT);
}

void KSceneSFXEditorBillboardPage::OnBnClickedRadioBillboardTexture2Index()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_BILLBOARD();

    pFrame->GetKeysFrame()->SetTimeLine("公告板纹理2帧索引", pBillboard->GetTexture2FrameIndex(), SFX_TLDT_INT);
}

void KSceneSFXEditorBillboardPage::OnBnClickedButtonBillboardBrowseTexture2Name()
{
    // TODO: Add your control notification handler code here
    CString strTextureName;
    if(BrowseTexture(strTextureName))
    {
        GET_SFX_EDITOR();
        GET_BILLBOARD();

        if (SUCCEEDED(pBillboard->LoadTexture2(strTextureName)))
        {
            m_editTexture2Name.SetWindowText(strTextureName);
            m_radioTexture2FrameIndex.EnableWindow(TRUE);
            UpdateTextureFrameIndex2(pBillboard, pFrame->GetKeysFrame()->m_Slider.GetCurPos());
            m_editTexture2Cut.EnableWindow(TRUE);
        }
    }
}

void KSceneSFXEditorBillboardPage::OnSetTexture()
{
    GET_SFX_EDITOR();
    GET_BILLBOARD();

    CString strTextureName;
    GetDlgItemText(IDC_EDIT_BILLBOARD_TEXTURE_NAME, strTextureName);
    if (SUCCEEDED(pBillboard->LoadTexture(strTextureName)))
    {
        m_radioTextureFrameIndex.EnableWindow(TRUE);
        UpdateTextureFrameIndex1(pBillboard, pFrame->GetKeysFrame()->m_Slider.GetCurPos());
        m_editTextureCut.EnableWindow(TRUE);
    }
    else
    {
        MessageBox("加载纹理失败", "ERROR");
    }

    UpdateTextureName1(pBillboard);
}

void KSceneSFXEditorBillboardPage::OnSetTexture2()
{
    GET_SFX_EDITOR();
    GET_BILLBOARD();

    CString strTextureName;
    GetDlgItemText(IDC_EDIT_BILLBOARD_TEXTURE2_NAME, strTextureName);
    if (SUCCEEDED(pBillboard->LoadTexture2(strTextureName)))
    {
        m_radioTexture2FrameIndex.EnableWindow(TRUE);
        UpdateTextureFrameIndex2(pBillboard, pFrame->GetKeysFrame()->m_Slider.GetCurPos());
        m_editTexture2Cut.EnableWindow(TRUE);

    }
    else
    {
        MessageBox("加载纹理失败", "ERROR");
    }

    UpdateTextureName2(pBillboard);
}
BOOL KSceneSFXEditorBillboardPage::OnSetActive()
{
    // TODO: Add your specialized code here and/or call the base class
    UpdateUI();

    return TRUE;
}


void KSceneSFXEditorBillboardPage::OnEnSetfocusEditBillboardWidth()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_BILLBOARD();

    CheckRadioButton(IDC_RADIO_BILLBOARD_COLOR, IDC_RADIO_UV_OFFSET, IDC_RADIO_BILLBOARD_WIDTH);
    OnBnClickedRadioBillboardWidth();

    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    UpdateBillboardWidth(pBillboard, nFrame);
}

void KSceneSFXEditorBillboardPage::OnEnSetfocusEditBillboardHeight()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_BILLBOARD();

    CheckRadioButton(IDC_RADIO_BILLBOARD_COLOR, IDC_RADIO_UV_OFFSET, IDC_RADIO_BILLBOARD_HEIGHT);
    OnBnClickedRadioBillboardHeight();

    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    UpdateBillboardHeight(pBillboard, nFrame);
}

void KSceneSFXEditorBillboardPage::OnEnSetfocusEditBillboardTextureIndex()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_BILLBOARD();

    CheckRadioButton(IDC_RADIO_BILLBOARD_COLOR, IDC_RADIO_UV_OFFSET, IDC_RADIO_BILLBOARD_TEXTURE_INDEX);
    OnBnClickedRadioBillboardTextureIndex();

    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    UpdateTextureFrameIndex1(pBillboard, nFrame);
}

void KSceneSFXEditorBillboardPage::OnEnSetfocusEditBillboardTexture2Index()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_BILLBOARD();

    CheckRadioButton(IDC_RADIO_BILLBOARD_COLOR, IDC_RADIO_UV_OFFSET, IDC_RADIO_BILLBOARD_TEXTURE2_INDEX);
    OnBnClickedRadioBillboardTexture2Index();

    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    UpdateTextureFrameIndex2(pBillboard, nFrame);
}

void KSceneSFXEditorBillboardPage::OnEnSetfocusEditRotationAnimation()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_BILLBOARD();

    CheckRadioButton(IDC_RADIO_BILLBOARD_COLOR, IDC_RADIO_UV_OFFSET, IDC_RADIO_ROTATION_ANIMATION);
    OnBnClickedRadioRotationAnimation();

    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    UpdateRotation(pBillboard, nFrame);
}

void KSceneSFXEditorBillboardPage::OnEnSetfocusEditUOffset()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_BILLBOARD();

    CheckRadioButton(IDC_RADIO_BILLBOARD_COLOR, IDC_RADIO_UV_OFFSET, IDC_RADIO_UV_OFFSET);
    OnBnClickedRadioUvOffset();

    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    UpdateUV(pBillboard, nFrame);
}

void KSceneSFXEditorBillboardPage::OnEnSetfocusEditVOffset()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_BILLBOARD();

    CheckRadioButton(IDC_RADIO_BILLBOARD_COLOR, IDC_RADIO_UV_OFFSET, IDC_RADIO_UV_OFFSET);
    OnBnClickedRadioUvOffset();
    //OnBnClickedRadioRotationAnimation();

    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    UpdateUV(pBillboard, nFrame);
}

BOOL KSceneSFXEditorBillboardPage::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  Add extra initialization here
    m_comboBlendMode.InsertString(BILLBOARD_BLEND_MODE_SRCALPHA_INVSRCALPHA, "Alpha");
    m_comboBlendMode.InsertString(BILLBOARD_BLEND_MODE_SRCALPHA_ONE, "Addition");
    m_comboBlendMode.InsertString(BILLBOARD_BLEND_MODE_SRCALPHA_INV, "Subtriction");

    m_comboType.AddString("面向眼睛");
    m_comboType.AddString("锁 Y 轴");
    m_comboType.AddString("锁 X 轴");

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneSFXEditorBillboardPage::UpdateBlendMode(IEKG3DSFXBillboard *pBillboard)
{
    if (pBillboard->GetBlendMode() == SFX_BM_SRCALPHA_INVSRCALPHA)
        m_comboBlendMode.SetCurSel(BILLBOARD_BLEND_MODE_SRCALPHA_INVSRCALPHA);
    else if (pBillboard->GetBlendMode() == SFX_BM_SRCALPHA_ONE)
        m_comboBlendMode.SetCurSel(BILLBOARD_BLEND_MODE_SRCALPHA_ONE);
    else if (pBillboard->GetBlendMode() == SFX_BM_SRCALPHA_INVSRCCOLOR)
        m_comboBlendMode.SetCurSel(BILLBOARD_BLEND_MODE_SRCALPHA_INV);

}

void KSceneSFXEditorBillboardPage::UpdateType(IEKG3DSFXBillboard *pBillboard)
{
    m_comboType.SetCurSel((int)pBillboard->GetBillboardType());

}


void KSceneSFXEditorBillboardPage::OnCbnSelchangeComboType()
{
    GET_SFX_EDITOR();
    GET_BILLBOARD();
    pBillboard->SetBillboardType((DWORD)m_comboType.GetCurSel());
}



void KSceneSFXEditorBillboardPage::OnCbnSelchangeComboBlendMode()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_BILLBOARD();

    int nSelectBM = m_comboBlendMode.GetCurSel();
    SFX_BLEND_MODE bm = SFX_BM_SRCALPHA_INVSRCALPHA;
    if (nSelectBM == BILLBOARD_BLEND_MODE_SRCALPHA_INVSRCALPHA)
        bm = SFX_BM_SRCALPHA_INVSRCALPHA;
    else if (nSelectBM == BILLBOARD_BLEND_MODE_SRCALPHA_ONE)
        bm = SFX_BM_SRCALPHA_ONE;
    else if (nSelectBM == BILLBOARD_BLEND_MODE_SRCALPHA_INV)
        bm = SFX_BM_SRCALPHA_INVSRCCOLOR;
    else
        ASSERT(0);

    pBillboard->SetBlendMode(bm);
}
