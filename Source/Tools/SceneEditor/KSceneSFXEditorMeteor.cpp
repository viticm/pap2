// ***************************************************************
//
//  Description: Do you see my past ?
//  -------------------------------------------------------------
//	Author: Zeb
//  -------------------------------------------------------------
//	Create Date: 2006-05-25
//	-------------------------------------------------------------
//	History:
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
//
// ***************************************************************
// KSceneSFXEditorMeteor.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSFXEditorMeteor.h"
//(by dengzhihui 2006年7月5日 16:27:20
//#include "MainFrm.h"
//)
#include "KSceneSFXEditorFrame.h"
#include "IEKG3DSFX.h"
//#include "KG3DSceneEditorBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


// KSceneSFXEditorMeteor dialog

IMPLEMENT_DYNAMIC(KSceneSFXEditorMeteor, CDialog)

KSceneSFXEditorMeteor::KSceneSFXEditorMeteor(CWnd* pWnd)
	: CDialog(KSceneSFXEditorMeteor::IDD, pWnd)
{

}

KSceneSFXEditorMeteor::~KSceneSFXEditorMeteor()
{
}

void KSceneSFXEditorMeteor::DoDataExchange(CDataExchange* pDX)
{
    DDX_Control(pDX, IDC_EDIT_METEOR_WIDTH, m_editWidth);
    DDX_Control(pDX, IDC_EDIT_METEOR_LIFE, m_editLife);
    DDX_Control(pDX, IDC_EDIT_METEOR_BIND_TRACK, m_editTrackIdx);
    DDX_Control(pDX, IDC_EDIT_TEXTURE_INDEX, m_editTextureIdx);
    DDX_Control(pDX, IDC_EDIT_EMIT_INTERVAL, m_editEmitInterval);
    DDX_Control(pDX, IDC_EDIT_INTERPORATE, m_editInterporateLength);
    DDX_Control(pDX, IDC_EDIT_DURTIME, m_editDutTime);
    DDX_Control(pDX, IDC_CHECK_RANDOM_OFFSET, m_checkRandomOffset);
    DDX_Control(pDX, IDC_EDIT_OFFSET_X, m_editOffsetX);
    DDX_Control(pDX, IDC_EDIT_OFFSET_Y, m_editOffsetY);
    DDX_Control(pDX, IDC_EDIT_OFFSET_Z, m_editOffsetZ);
    DDX_Control(pDX, IDC_EDIT_TRALLYING, m_editRallying);
    DDX_Control(pDX, IDC_CHECK_ADD_BLEND, m_checkAddBlend);

    DDX_Control(pDX, IDC_BUTTON_ADD_METEOR, m_buttonAddMeteor);
    DDX_Control(pDX, IDC_BUTTON_DEL_METEOR, m_buttonDeleteMeteor);

    DDX_Control(pDX, IDC_LIST_METEOR, m_listMeteor);

	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(KSceneSFXEditorMeteor, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_ADD_METEOR, &KSceneSFXEditorMeteor::OnBnClickedButtonAddMeteor)
    ON_BN_CLICKED(IDC_BUTTON_DEL_METEOR, &KSceneSFXEditorMeteor::OnBnClickedButtonDelMeteor)
    ON_NOTIFY(NM_CLICK, IDC_LIST_METEOR, &KSceneSFXEditorMeteor::OnNMClickListMeteor)
    ON_BN_CLICKED(IDC_CHECK_ADD_BLEND, &KSceneSFXEditorMeteor::OnBnClickedCheckAddBlend)
    ON_BN_CLICKED(IDC_CHECK_RANDOM_OFFSET, &KSceneSFXEditorMeteor::OnBnClickedCheckRandomOffset)
    ON_BN_CLICKED(IDC_INTERPOLATE, &KSceneSFXEditorMeteor::OnBnClickedInterpolate)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_METEOR, &KSceneSFXEditorMeteor::OnLvnItemchangedListMeteor)
	ON_BN_CLICKED(IDC_RADIO_SAMPLING, &KSceneSFXEditorMeteor::OnBnClickedRadioSampling)
	ON_BN_CLICKED(IDC_RADIO_SAMPLING_STOP, &KSceneSFXEditorMeteor::OnBnClickedRadioSamplingStop)
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


HBRUSH KSceneSFXEditorMeteor::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

// KSceneSFXEditorMeteor message handlers

void KSceneSFXEditorMeteor::OnBnClickedButtonAddMeteor()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();

    IEKG3DSFXMeteor *pMeteor = NULL;
    HRESULT hr = pScene->AddMeteor();
    if (SUCCEEDED(hr))
    {
        UpdateMeteorList();
    }
}

void KSceneSFXEditorMeteor::OnBnClickedButtonDelMeteor()
{
    // TODO: Add your control notification handler code here
    int nSelect = m_listMeteor.GetSelectionMark();
    if (nSelect == -1)
        return;

    GET_SFX_EDITOR();
    pScene->RemoveMeteor(nSelect);
    UpdateMeteorList();
}

void KSceneSFXEditorMeteor::UpdateUI()
{
    if (!IsWindow(m_hWnd))
        return;
    UpdateMeteorList();
    UpdateProperties();
}

BOOL KSceneSFXEditorMeteor::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  Add extra initialization here
    m_listMeteor.SetBkColor(RGB(64, 64, 64));
    m_listMeteor.SetTextColor(RGB(255, 255, 255));
    m_listMeteor.SetTextBkColor(RGB(64, 64, 64));

    m_listMeteor.InsertColumn(0, _T(""),LVCFMT_CENTER, 150);
    m_listMeteor.SetExtendedStyle(m_listMeteor.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	((CButton*)GetDlgItem(IDC_RADIO_SAMPLING))->SetCheck(TRUE);
    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
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

LRESULT KSceneSFXEditorMeteor::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    // TODO: Add your specialized code here and/or call the base class
    switch (message)
    {
    case WM_EDIT_RECEIVE_ENTER:
        {
            switch (wParam)
            {
            case IDC_EDIT_METEOR_LIFE:
                OnSetLife();
                break;

            case IDC_EDIT_METEOR_WIDTH:
                OnSetWidth();
                break;

            case IDC_EDIT_METEOR_BIND_TRACK:
                OnSetTrackIdx();
                break;

            case IDC_EDIT_TEXTURE_INDEX:
                OnSetTextureIdx();
                break;

            case IDC_EDIT_EMIT_INTERVAL:
                OnSetEmitInterval();
                break;

            case IDC_EDIT_OFFSET_X:
                OnSetOffsetX();
                break;

            case IDC_EDIT_OFFSET_Y:
                OnSetOffsetY();
                break;

            case IDC_EDIT_OFFSET_Z:
                OnSetOffsetZ();

            case IDC_EDIT_TRALLYING :
                OnSetRallying();
                break;

            case IDC_EDIT_INTERPORATE :
                OnSetInterporateLength();
                break;
            case IDC_EDIT_DURTIME :
                {
                    IEKG3DSFX* sfx = GetSfx();
                    if (!sfx)
                        break;
                    int nSelect = m_listMeteor.GetSelectionMark();
                    if (nSelect == -1)
                        break;
                    IEKG3DSFXMeteor *pMeteor = NULL;
                    sfx->GetMeteor(nSelect, &pMeteor);
                    if (!pMeteor)
                        break;
                    float fDur = GetFloat(lParam);
                    pMeteor->SetDuration(fDur);
                }
                break;
            }
        }
    }

    return CDialog::WindowProc(message, wParam, lParam);
}

void KSceneSFXEditorMeteor::UpdateMeteorList()
{
    GET_SFX_EDITOR();
    m_listMeteor.DeleteAllItems();

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    for (DWORD i = 0; i < pSFX->GetMeteorNum(); ++i)
    {
        CString str;
        str.Format("Meteor %d", i);
        m_listMeteor.InsertItem(i, str);
    }
}

void KSceneSFXEditorMeteor::UpdateProperties()
{
    GET_SFX_EDITOR();
    int nSelect = m_listMeteor.GetSelectionMark();
    if (nSelect == -1)
    {
        m_editLife.EnableWindow(FALSE);
        m_editWidth.EnableWindow(FALSE);
        m_editTrackIdx.EnableWindow(FALSE);
        m_editTextureIdx.EnableWindow(FALSE);
        m_editEmitInterval.EnableWindow(FALSE);
        m_checkRandomOffset.EnableWindow(FALSE);
        m_editOffsetX.EnableWindow(FALSE);
        m_editOffsetY.EnableWindow(FALSE);
        m_editOffsetZ.EnableWindow(FALSE);
        m_editRallying.EnableWindow(FALSE);
        m_editInterporateLength.EnableWindow(FALSE);
        m_editDutTime.EnableWindow(FALSE);
        ((CButton*)GetDlgItem(IDC_INTERPOLATE))->SetCheck(FALSE);

        return;
    }
    else
    {
        m_editLife.EnableWindow();
        m_editWidth.EnableWindow();
        m_editTrackIdx.EnableWindow();
        m_editTextureIdx.EnableWindow();
        m_editEmitInterval.EnableWindow();
        m_checkRandomOffset.EnableWindow();
        m_editOffsetX.EnableWindow();
        m_editOffsetY.EnableWindow();
        m_editOffsetZ.EnableWindow();
        m_editRallying.EnableWindow();
        m_editDutTime.EnableWindow();
    }

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    IEKG3DSFXMeteor *pMeteor = NULL;
    HRESULT hr = pSFX->GetMeteor((DWORD)nSelect, &pMeteor);
    ASSERT(hr == S_OK);
    if (pMeteor->GetMeteorInterpolate() >= 0)
    {
        m_editInterporateLength.EnableWindow();
        ((CButton*)GetDlgItem(IDC_INTERPOLATE))->SetCheck(TRUE);
    }
    else
    {
        ((CButton*)GetDlgItem(IDC_INTERPOLATE))->SetCheck(FALSE);
    }

    CString str;
    str.Format("%f", pMeteor->GetLife());
    m_editLife.SetWindowText(str);

    str.Format("%f", pMeteor->GetWidth());
    m_editWidth.SetWindowText(str);

    str.Format("%d", pMeteor->GetBindTrackIdx());
    m_editTrackIdx.SetWindowText(str);

    str.Format("%d", pMeteor->GetTextureIdx());
    m_editTextureIdx.SetWindowText(str);

    str.Format("%f", pMeteor->GetEmitMinInterval());
    m_editEmitInterval.SetWindowText(str);

    str.Format("%f", pMeteor->GetOffset().x);
    m_editOffsetX.SetWindowText(str);

    str.Format("%f", pMeteor->GetOffset().y);
    m_editOffsetY.SetWindowText(str);

    str.Format("%f", pMeteor->GetOffset().z);
    m_editOffsetZ.SetWindowText(str);

    str.Format("%f", pMeteor->GetRallyingScale());
    m_editRallying.SetWindowText(str);

    str.Format("%f", pMeteor->GetDuration());
    m_editDutTime.SetWindowText(str);

    float fInterpolateLength = pMeteor->GetMeteorInterpolate();
    if (fInterpolateLength == -1)
        fInterpolateLength = 10;
    str.Format("%f", fInterpolateLength);
    m_editInterporateLength.SetWindowText(str);

    m_checkRandomOffset.SetCheck(pMeteor->IsEnableRandomOffset());
    if (pMeteor->IsEnableRandomOffset())
    {
        m_editOffsetY.EnableWindow(FALSE);
        m_editOffsetZ.EnableWindow(FALSE);
    }

    if (pMeteor->GetBlendMode() == SFX_BM_SRCALPHA_ONE)
        m_checkAddBlend.SetCheck(TRUE);
    else
        m_checkAddBlend.SetCheck(FALSE);
}

void KSceneSFXEditorMeteor::OnSetLife()
{
    GET_SFX_EDITOR();
    int nSelect = m_listMeteor.GetSelectionMark();
    if (nSelect == -1)
        return;

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    IEKG3DSFXMeteor *pMeteor = NULL;
    HRESULT hr = pSFX->GetMeteor(nSelect, &pMeteor);
    ASSERT(hr == S_OK);

    CString str;
    m_editLife.GetWindowText(str);
    float fValue = (float)::atof(str);
    pMeteor->SetLife(fValue);
}

void KSceneSFXEditorMeteor::OnSetWidth()
{
    GET_SFX_EDITOR();
    int nSelect = m_listMeteor.GetSelectionMark();
    if (nSelect == -1)
        return;

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    IEKG3DSFXMeteor *pMeteor = NULL;
    HRESULT hr = pSFX->GetMeteor(nSelect, &pMeteor);
    ASSERT(hr == S_OK);

    CString str;
    m_editWidth.GetWindowText(str);
    float fValue = (float)::atof(str);
    pMeteor->SetWidth(fValue);
}

void KSceneSFXEditorMeteor::OnSetEmitInterval()
{
    GET_SFX_EDITOR();
    int nSelect = m_listMeteor.GetSelectionMark();
    if (nSelect == -1)
        return;

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    IEKG3DSFXMeteor *pMeteor = NULL;
    HRESULT hr = pSFX->GetMeteor(nSelect, &pMeteor);
    ASSERT(hr == S_OK);

    CString str;
    m_editEmitInterval.GetWindowText(str);
    float fValue = (float)::atof(str);
    pMeteor->SetEmitMinInterval(fValue);
}

void KSceneSFXEditorMeteor::OnSetTrackIdx()
{
    GET_SFX_EDITOR();
    int nSelect = m_listMeteor.GetSelectionMark();
    if (nSelect == -1)
        return;

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    IEKG3DSFXMeteor *pMeteor = NULL;
    HRESULT hr = pSFX->GetMeteor(nSelect, &pMeteor);
    ASSERT(hr == S_OK);

    CString str;
    m_editTrackIdx.GetWindowText(str);
    int nValue = ::atoi(str);
    pMeteor->SetBindTrackIdx(nValue);
}

void KSceneSFXEditorMeteor::OnSetTextureIdx()
{
    GET_SFX_EDITOR();
    int nSelect = m_listMeteor.GetSelectionMark();
    if (nSelect == -1)
        return;

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    IEKG3DSFXMeteor *pMeteor = NULL;
    HRESULT hr = pSFX->GetMeteor(nSelect, &pMeteor);
    ASSERT(hr == S_OK);

    CString str;
    m_editTextureIdx.GetWindowText(str);
    int nValue = ::atoi(str);
    if (nValue >= 0 && nValue < (int)pSFX->GetTextureNum())
        pMeteor->SetTextureIdx((DWORD)nValue);
    str.Format("%d", (int)pMeteor->GetTextureIdx());
    m_editTextureIdx.SetWindowText(str);
}
void KSceneSFXEditorMeteor::OnNMClickListMeteor(NMHDR *pNMHDR, LRESULT *pResult)
{
    UpdateProperties();
    int nSelect = m_listMeteor.GetSelectionMark();
    if (nSelect == -1)
        return;

    GET_SFX_EDITOR();

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    IEKG3DSFXMeteor *pMeteor = NULL;
    HRESULT hr = pSFX->GetMeteor(nSelect, &pMeteor);
    ASSERT(hr == S_OK);
	BOOL b = pMeteor->GetSampling();
	if (b)
	{
		((CButton*)GetDlgItem(IDC_RADIO_SAMPLING))->SetCheck(TRUE);
		((CButton*)GetDlgItem(IDC_RADIO_SAMPLING_STOP))->SetCheck(FALSE);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO_SAMPLING_STOP))->SetCheck(TRUE);
		((CButton*)GetDlgItem(IDC_RADIO_SAMPLING))->SetCheck(FALSE);
	}
   /* IEKG3DSceneEditorBase *piSceneEditorBase = NULL;
    pScene->GetSceneEditBase(&piSceneEditorBase);
	GETSELECT(piSceneEditorBase);
    pSelector->SelectNone(0);*/ //改用ClearSelection();
    
	//KG3DSceneEntity Entity(SCENEENTITY_SFX_METEOR, pMeteor);
    //piSceneEditorBase->m_listSelectedEntity.push_back(Entity);
	
	//pSelector->AddSelectedEntity(SCENEENTITY_SFX_METEOR, pMeteor);
	IEKG3DSceneSelectionTool* pTool = NULL;
	hr = pScene->GetSelectionToolInterface(&pTool);
	if (SUCCEEDED(hr))
	{
		pTool->ClearSelection();
		hr = pTool->SetSelected(SCENEENTITY_SFX_METEOR, pMeteor, TRUE);
	}
}

BOOL KSceneSFXEditorMeteor::OnSetActive()
{
    // TODO: Add your specialized code here and/or call the base class
    UpdateUI();

    return TRUE;//CPropertyPage::OnSetActive();
}

void KSceneSFXEditorMeteor::OnSetOffsetX()
{
    GET_SFX_EDITOR();
    int nSelect = m_listMeteor.GetSelectionMark();
    if (nSelect == -1)
        return;

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    IEKG3DSFXMeteor *pMeteor = NULL;
    HRESULT hr = pSFX->GetMeteor(nSelect, &pMeteor);
    ASSERT(hr == S_OK);

    CString str;
    m_editOffsetX.GetWindowText(str);
    float fValue = (float)::atof(str);
    D3DXVECTOR3 v = pMeteor->GetOffset();
    v.x = fValue;
    pMeteor->SetOffset(v);
}

void KSceneSFXEditorMeteor::OnSetOffsetY()
{
    GET_SFX_EDITOR();
    int nSelect = m_listMeteor.GetSelectionMark();
    if (nSelect == -1)
        return;

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    IEKG3DSFXMeteor *pMeteor = NULL;
    HRESULT hr = pSFX->GetMeteor(nSelect, &pMeteor);
    ASSERT(hr == S_OK);

    CString str;
    m_editOffsetY.GetWindowText(str);
    float fValue = (float)::atof(str);
    D3DXVECTOR3 v = pMeteor->GetOffset();
    v.y = fValue;
    pMeteor->SetOffset(v);
}

void KSceneSFXEditorMeteor::OnSetRallying()
{
    GET_SFX_EDITOR();
    int nSelect = m_listMeteor.GetSelectionMark();
    if (nSelect == -1)
        return;
    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    IEKG3DSFXMeteor *pMeteor = NULL;
    HRESULT hr = pSFX->GetMeteor(nSelect, &pMeteor);
    ASSERT(hr == S_OK);

    CString str;
    m_editRallying.GetWindowText(str);
    float fValue = (float)::atof(str);
    pMeteor->SetRallyingScale(fValue);
    str.Format(TEXT("%f"), pMeteor->GetRallyingScale());
    m_editRallying.SetWindowText(str);
}

void KSceneSFXEditorMeteor::OnSetInterporateLength()
{
    GET_SFX_EDITOR();
    int nSelect = m_listMeteor.GetSelectionMark();
    if (nSelect == -1)
        return;
    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    IEKG3DSFXMeteor *pMeteor = NULL;
    HRESULT hr = pSFX->GetMeteor(nSelect, &pMeteor);
    ASSERT(hr == S_OK);

    CString str;
    m_editInterporateLength.GetWindowText(str);
    float fValue = (float)::atof(str);
    
    if (fValue > 0)
    {
        pMeteor->SetMeteorInterpolate(fValue);
        str.Format(TEXT("%f"), pMeteor->GetMeteorInterpolate());
        m_editInterporateLength.SetWindowText(str);
    }
}


void KSceneSFXEditorMeteor::OnSetOffsetZ()
{
    GET_SFX_EDITOR();
    int nSelect = m_listMeteor.GetSelectionMark();
    if (nSelect == -1)
        return;

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    IEKG3DSFXMeteor *pMeteor = NULL;
    HRESULT hr = pSFX->GetMeteor(nSelect, &pMeteor);
    ASSERT(hr == S_OK);

    CString str;
    m_editOffsetZ.GetWindowText(str);
    float fValue = (float)::atof(str);
    D3DXVECTOR3 v = pMeteor->GetOffset();
    v.z = fValue;
    pMeteor->SetOffset(v);
}

void KSceneSFXEditorMeteor::OnBnClickedCheckAddBlend()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    int nSelect = m_listMeteor.GetSelectionMark();
    if (nSelect == -1)
        return;

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    IEKG3DSFXMeteor *pMeteor = NULL;
    HRESULT hr = pSFX->GetMeteor(nSelect, &pMeteor);
    ASSERT(hr == S_OK);

    int nCheck = m_checkAddBlend.GetCheck();
    pMeteor->SetBlendMode(nCheck ? SFX_BM_SRCALPHA_ONE : SFX_BM_SRCALPHA_INVSRCALPHA);
}

void KSceneSFXEditorMeteor::OnBnClickedCheckRandomOffset()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    int nSelect = m_listMeteor.GetSelectionMark();
    if (nSelect == -1)
        return;

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    IEKG3DSFXMeteor *pMeteor = NULL;
    HRESULT hr = pSFX->GetMeteor(nSelect, &pMeteor);
    ASSERT(hr == S_OK);

    int nCheck = m_checkRandomOffset.GetCheck();
    pMeteor->EnableRandomOffset(nCheck);
    UpdateProperties();
}

void KSceneSFXEditorMeteor::OnBnClickedInterpolate()
{
    GET_SFX_EDITOR();
    int nSelect = m_listMeteor.GetSelectionMark();
    if (nSelect == -1)
        return;
    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    IEKG3DSFXMeteor *pMeteor = NULL;
    HRESULT hr = pSFX->GetMeteor(nSelect, &pMeteor);
    ASSERT(hr == S_OK);

    if (((CButton*)GetDlgItem(IDC_INTERPOLATE))->GetCheck())
    {
        m_editInterporateLength.EnableWindow();
        CString str;
        m_editInterporateLength.GetWindowText(str);
        float fValue = (float)::atof(str);
        pMeteor->SetMeteorInterpolate(fValue);
        str.Format(TEXT("%f"), pMeteor->GetMeteorInterpolate());
        m_editInterporateLength.SetWindowText(str);
    }
    else
    {
        m_editInterporateLength.EnableWindow(FALSE);
        pMeteor->SetMeteorInterpolate(-1);
    }
}

void KSceneSFXEditorMeteor::OnLvnItemchangedListMeteor(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void KSceneSFXEditorMeteor::OnBnClickedRadioSampling()
{
	GET_SFX_EDITOR();
	int nSelect = m_listMeteor.GetSelectionMark();
	if (nSelect == -1)
		return;
	IEKG3DSFX *pSFX = NULL;
	pScene->GetSFX(&pSFX);
	IEKG3DSFXMeteor *pMeteor = NULL;
	HRESULT hr = pSFX->GetMeteor(nSelect, &pMeteor);
	ASSERT(hr == S_OK);
	pMeteor->SetSampling(TRUE);
	pMeteor->AddListNode();

}

void KSceneSFXEditorMeteor::OnBnClickedRadioSamplingStop()
{
	GET_SFX_EDITOR();
	int nSelect = m_listMeteor.GetSelectionMark();
	if (nSelect == -1)
		return;
	IEKG3DSFX *pSFX = NULL;
	pScene->GetSFX(&pSFX);
	IEKG3DSFXMeteor *pMeteor = NULL;
	HRESULT hr = pSFX->GetMeteor(nSelect, &pMeteor);
	ASSERT(hr == S_OK);
	pMeteor->SetSampling(FALSE);
	
}



void KSceneSFXEditorMeteor::OnMeteorOpenClost()
{
	GET_SFX_EDITOR();
	int nSelect = m_listMeteor.GetSelectionMark();
	if (nSelect == -1)
		return;
	IEKG3DSFX *pSFX = NULL;
	pScene->GetSFX(&pSFX);
	IEKG3DSFXMeteor *pMeteor = NULL;
	HRESULT hr = pSFX->GetMeteor(nSelect, &pMeteor);
	ASSERT(hr == S_OK);
	BOOL b = pMeteor->GetSampling();
	if (!b)
	{
		((CButton*)GetDlgItem(IDC_RADIO_SAMPLING))->SetCheck(TRUE);
		((CButton*)GetDlgItem(IDC_RADIO_SAMPLING_STOP))->SetCheck(FALSE);
		pMeteor->SetSampling(TRUE);
		pMeteor->AddListNode();
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO_SAMPLING_STOP))->SetCheck(TRUE);
		((CButton*)GetDlgItem(IDC_RADIO_SAMPLING))->SetCheck(FALSE);
		pMeteor->SetSampling(FALSE);
	}
}
