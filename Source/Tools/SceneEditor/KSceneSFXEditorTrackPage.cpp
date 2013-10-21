////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSFXEditorTrackPage.cpp
//  Version     : 1.0
//  Creator     : Zeb
//  Create Date : 2005-12-1 16:08:41
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KSceneSFXEditorTrackPage.h"


////////////////////////////////////////////////////////////////////////////////

// KSceneSFXEditorTrackPage.cpp : implementation file
//

#include "SceneEditor.h"
//#include "MainFrm.h" //(by dengzhihui 2006年7月6日 14:50:12
#include "KSceneSFXEditorFrame.h"
#include "IEKG3DSceneSFXEditor.h"
#include "IEKG3DSFX.h"
#include ".\kscenesfxeditortrackpage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


#define SFX_BIND_TO_TRACK_LAUNCHER_INDEX 0
#define SFX_BIND_TO_TRACK_PARTICLE_INDEX 1

// KSceneSFXEditorTrackPage dialog

IMPLEMENT_DYNAMIC(KSceneSFXEditorTrackPage, CDialog)
KSceneSFXEditorTrackPage::KSceneSFXEditorTrackPage(CWnd* pWnd /* = NULL */)
	: CDialog(KSceneSFXEditorTrackPage::IDD, pWnd)
{
}

KSceneSFXEditorTrackPage::~KSceneSFXEditorTrackPage()
{
}

void KSceneSFXEditorTrackPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_EDIT_TRACK_X, m_editTrackX);
    DDX_Control(pDX, IDC_EDIT_TRACK_Y, m_editTrackY);
    DDX_Control(pDX, IDC_EDIT_TRACK_Z, m_editTrackZ);

    DDX_Control(pDX, IDC_EDIT_INTERVAL, m_editTrackInterval);
    DDX_Control(pDX, IDC_EDIT_FRAME_NUM, m_editTrackFrameNum);

    DDX_Control(pDX, IDC_LIST_TRACK, m_listTrack);
    DDX_Control(pDX, IDC_LIST_LAUNCHER, m_listLauncher);
    DDX_Control(pDX, IDC_LIST_BILLBOARD, m_listBillboard);
    DDX_Control(pDX, IDC_LIST_MODEL, m_listModel);
    DDX_Control(pDX, IDC_LIST_METEOR, m_listMeteor);

    DDX_Control(pDX, IDC_COMBO_BIND_MODE, m_comboBindMode);

    DDX_Control(pDX, IDC_BUTTON_METEOR_OK, m_button10);
    DDX_Control(pDX, IDC_BUTTON_METEOR_CN, m_button11);

    DDX_Control(pDX, IDC_BUTTON_MODEL_OK, m_button8);
    DDX_Control(pDX, IDC_BUTTON_MODEL_CN, m_button9);
    DDX_Control(pDX, IDC_BUTTON_ADD_TRACK, m_button1);
    DDX_Control(pDX, IDC_BUTTON_DEL_TRACK, m_button2);
    DDX_Control(pDX, IDC_BUTTON_EDIT_TRACK, m_button3);
    DDX_Control(pDX, IDC_BUTTON_LAUNCHER_BIND_TO_TRACK, m_button4);
    DDX_Control(pDX, IDC_BUTTON_LAUNCHER_BIND_CANCEL, m_button5);
    DDX_Control(pDX, IDC_BUTTON_BILLBOARD_BIND_TO_TRACK, m_button6);
    DDX_Control(pDX, IDC_BUTTON_BILLBOARD_BIND_CANCEL, m_button7);
}


BEGIN_MESSAGE_MAP(KSceneSFXEditorTrackPage, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_ADD_TRACK, OnBnClickedButtonAddTrack)
    ON_BN_CLICKED(IDC_BUTTON_DEL_TRACK, OnBnClickedButtonDelTrack)
    ON_BN_CLICKED(IDC_BUTTON_LAUNCHER_BIND_TO_TRACK, OnBnClickedButtonLauncherBindToTrack)
    ON_NOTIFY(NM_CLICK, IDC_LIST_TRACK, OnNMClickListTrack)
    ON_BN_CLICKED(IDC_BUTTON_EDIT_TRACK, OnBnClickedButtonEditTrack)
    ON_BN_CLICKED(IDC_BUTTON_LAUNCHER_BIND_CANCEL, OnBnClickedButtonLauncherBindCancel)
    ON_CBN_SELCHANGE(IDC_COMBO_BIND_MODE, OnCbnSelchangeComboBindMode)
    ON_NOTIFY(NM_CLICK, IDC_LIST_LAUNCHER, OnNMClickListLauncher)
    ON_BN_CLICKED(IDC_BUTTON_BILLBOARD_BIND_TO_TRACK, OnBnClickedButtonBillboardBindToTrack)
    ON_BN_CLICKED(IDC_BUTTON_BILLBOARD_BIND_CANCEL, OnBnClickedButtonBillboardBindCancel)
    ON_BN_CLICKED(IDC_BUTTON_MODEL_OK, OnBnClickedButtonModelBindToTrack)
    ON_BN_CLICKED(IDC_BUTTON_MODEL_CN, OnBnClickedButtonModelBindCancel)
    ON_BN_CLICKED(IDC_BUTTON_METEOR_OK, OnBnClickedButtonMeteorBindToTrack)
    ON_BN_CLICKED(IDC_BUTTON_METEOR_CN, OnBnClickedButtonMeteorBindCancel)
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

HBRUSH KSceneSFXEditorTrackPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

// KSceneSFXEditorTrackPage message handlers

void KSceneSFXEditorTrackPage::UpdateUI()
{
    UpdateBindInfo();
    UpdateTrackUI();
    UpdateLauncherUI();
    UpdateBillboardUI();
    UpdateModelUI();
    UpdateMeteorUI();
}

void KSceneSFXEditorTrackPage::UpdateTrackUI()
{
    LPCTSTR pszName = NULL;
    if (!m_pDoc)
        return;
    IEKG3DSceneSFXEditor *pEditor = NULL;
    m_pDoc->GetSceneSFXEditor(&pEditor);
    if (!pEditor)
        return;

    IEKG3DSFX* pSFX = NULL;
    pEditor->GetSFX(&pSFX);

    DWORD dwTrackNum = pSFX->GetTrackNum();

    m_listTrack.DeleteAllItems();

    DWORD i = 0;
    DWORD dwTrackLauncherCount = 0;
    DWORD dwTrackBillboardCount = 0;
    DWORD dwModelCount = 0;
    DWORD dwMeteorCount = 0;
    for (i = 0; i < dwTrackNum; ++i)
    {
        pEditor->GetTrackBindInfoLauncherCount(i, &dwTrackLauncherCount);
        pEditor->GetTrackBindInfoBillboardCount(i, &dwTrackBillboardCount);
        pEditor->GetTrackBindInfoModelCount(i, &dwModelCount);
        pEditor->GetTrackBindInfoMeteorCount(i, &dwMeteorCount);

        CString str;
        str.Format(
            "轨道%d(%d, %d, %d, %d)",
            i + 1,
            dwTrackLauncherCount,
            dwTrackBillboardCount,
            dwModelCount,
            dwMeteorCount
        );

        m_listTrack.InsertItem(i, str);

        IEKG3DSFXTrack *pTrack = NULL;
        pSFX->GetTrack(i, &pTrack);
        ASSERT(pTrack);

        pszName = pTrack->GetName();

        m_listTrack.SetItemText(i, 1, pszName);
    }
}

void KSceneSFXEditorTrackPage::UpdateLauncherUI()
{
    if (!m_pDoc)
        return;
    IEKG3DSceneSFXEditor *pEditor = NULL;
    m_pDoc->GetSceneSFXEditor(&pEditor);
    if (!pEditor)
        return;

    IEKG3DSFX* pSFX = NULL;
    pEditor->GetSFX(&pSFX);
    ASSERT(pSFX);

    DWORD dwLauncherNum = pSFX->GetParticleLauncherNum();
    m_listLauncher.DeleteAllItems();
    while (dwLauncherNum > 0)
    {
        dwLauncherNum--;

        CString str;
        str.Format("发射器%d", dwLauncherNum + 1);
        m_listLauncher.InsertItem(0, str);

        IEKG3DSFXParticleLauncher *pLauncher = NULL;
        pSFX->GetParticleLauncher(dwLauncherNum, &pLauncher);
        ASSERT(pLauncher);

        if (pLauncher->IsBindToTrack())
        {
            CString str;
            str.Format("轨道%d", pLauncher->GetBindTrackIndex() + 1);
            m_listLauncher.SetItemText(0, 1, str);
        }
    }
}

void KSceneSFXEditorTrackPage::UpdateBillboardUI()
{
    if (!m_pDoc)
        return;
    IEKG3DSceneSFXEditor *pEditor = NULL;
    m_pDoc->GetSceneSFXEditor(&pEditor);
    if (!pEditor)
        return;

    IEKG3DSFX* pSFX = NULL;
    pEditor->GetSFX(&pSFX);
    ASSERT(pSFX);

    DWORD dwBillboardNum = pSFX->GetBillboardNum();
    m_listBillboard.DeleteAllItems();
    while (dwBillboardNum > 0)
    {
        dwBillboardNum--;

        CString str;
        str.Format("公告板%d", dwBillboardNum + 1);
        m_listBillboard.InsertItem(0, str);

        IEKG3DSFXBillboard *pBillboard = NULL;
        pSFX->GetBillboard(dwBillboardNum, &pBillboard);
        ASSERT(pBillboard);

        if (pBillboard->IsBindToTrack())
        {
            CString str;
            str.Format("轨道%d", pBillboard->GetBindTrackIndex() + 1);
            m_listBillboard.SetItemText(0, 1, str);
        }
    }
}

void KSceneSFXEditorTrackPage::UpdateMeteorUI()
{
    if (!m_pDoc)
        return;
    IEKG3DSceneSFXEditor *pEditor = NULL;
    m_pDoc->GetSceneSFXEditor(&pEditor);
    if (!pEditor)
        return;

    IEKG3DSFX* pSFX = NULL;
    pEditor->GetSFX(&pSFX);
    ASSERT(pSFX);

    DWORD Nums = pSFX->GetMeteorNum();
    m_listMeteor.DeleteAllItems();
    while (Nums > 0)
    {
        Nums--;
        CString str;
        str.Format("刀光%d", Nums + 1);
        m_listMeteor.InsertItem(0, str);

        int nTrackIndex = pSFX->GetMeteorBindTrackIndex(Nums);
        if (nTrackIndex != -1)
        {
            str.Format("轨道%d", nTrackIndex + 1);
            m_listMeteor.SetItemText(0, 1, str);
        }
    }
}

void KSceneSFXEditorTrackPage::UpdateModelUI()
{
    if (!m_pDoc)
        return;
    IEKG3DSceneSFXEditor *pEditor = NULL;
    m_pDoc->GetSceneSFXEditor(&pEditor);
    if (!pEditor)
        return;

    IEKG3DSFX* pSFX = NULL;
    pEditor->GetSFX(&pSFX);
    ASSERT(pSFX);

    DWORD dwModelNums = pSFX->GetNumModels();
    m_listModel.DeleteAllItems();
    while (dwModelNums > 0)
    {
        dwModelNums--;
        CString str;
        str.Format("模型%d", dwModelNums + 1);
        m_listModel.InsertItem(0, str);

        IEKG3DModel *pModel = NULL;
        pSFX->GetModel(dwModelNums, &pModel);
        ASSERT(pModel);



            int nTrackIndex = 0;
            pSFX->GetModelBindTrackIndex(dwModelNums, &nTrackIndex);
            if (nTrackIndex != -1)
            {
                str.Format("轨道%d", nTrackIndex + 1);
                m_listModel.SetItemText(0, 1, str);
            }
    }
}

void KSceneSFXEditorTrackPage::UpdateBindInfo()
{
    if (!m_pDoc)
        return;
    IEKG3DSceneSFXEditor *pEditor = NULL;
    m_pDoc->GetSceneSFXEditor(&pEditor);
    if (!pEditor)
        return;
    pEditor->UpdateTrackBindInfo();
}

BOOL KSceneSFXEditorTrackPage::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  Add extra initialization here
    m_listTrack.InsertColumn(0, _T("轨道名"),	LVCFMT_CENTER, 100);
    m_listTrack.InsertColumn(1, _T("轨道文件名"), LVCFMT_CENTER, 500);

    m_listLauncher.InsertColumn(0, _T("发射器"),	LVCFMT_CENTER, 70);
    m_listLauncher.InsertColumn(1, _T("轨道名"), LVCFMT_CENTER, 100);

    m_listBillboard.InsertColumn(0, _T("公告板"),	LVCFMT_CENTER, 70);
    m_listBillboard.InsertColumn(1, _T("轨道名"), LVCFMT_CENTER, 100);

    m_listModel.InsertColumn(0, _T("模  型"),	LVCFMT_CENTER, 70);
    m_listModel.InsertColumn(1, _T("轨道名"), LVCFMT_CENTER, 100);

    m_listMeteor.InsertColumn(0, _T("刀  光"),	LVCFMT_CENTER, 70);
    m_listMeteor.InsertColumn(1, _T("轨道名"), LVCFMT_CENTER, 100);


    m_listMeteor.SetExtendedStyle(
        m_listMeteor.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES
        );

    m_listModel.SetExtendedStyle(
        m_listModel.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES
        );

    m_listTrack.SetExtendedStyle(
        m_listTrack.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES
        );

    m_listLauncher.SetExtendedStyle(
        m_listLauncher.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES
        );

    m_listBillboard.SetExtendedStyle(
        m_listBillboard.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES
        );

    m_comboBindMode.InsertString(SFX_BIND_TO_TRACK_LAUNCHER_INDEX, "发射器绑定");
    m_comboBindMode.InsertString(SFX_BIND_TO_TRACK_PARTICLE_INDEX, "粒子绑定");

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT KSceneSFXEditorTrackPage::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    // TODO: Add your specialized code here and/or call the base class
    switch (message)
    {
    case WM_EDIT_RECEIVE_ENTER:
        {
            switch (wParam)
            {
            case IDC_EDIT_TRACK_X:
                OnSetTrackX();
                break;
            case IDC_EDIT_TRACK_Y:
                OnSetTrackY();
                break;
            case IDC_EDIT_TRACK_Z:
                OnSetTrackZ();
                break;
            case IDC_EDIT_INTERVAL:
                OnSetInterval();
                break;
            }
        }
    }

    return CDialog::WindowProc(message, wParam, lParam);
}

void KSceneSFXEditorTrackPage::OnSetTrackX()
{
    if (!m_pDoc)
        return;
    IEKG3DSceneSFXEditor *pEditor = NULL;
    m_pDoc->GetSceneSFXEditor(&pEditor);
    if (!pEditor)
        return;

    IEKG3DSFX* pSFX = NULL;
    pEditor->GetSFX(&pSFX);
    ASSERT(pSFX);

    int nSelectTrack = m_listTrack.GetSelectionMark();
    if (nSelectTrack == -1)
        return;
    else
    {
        MessageBox(TEXT("请选中一个轨道"));
    }

    IEKG3DSFXTrack *pTrack = NULL;
    pSFX->GetTrack(nSelectTrack, &pTrack);
    ASSERT(pTrack);

    D3DXVECTOR3 vPos;
    pTrack->GetTranslation(&vPos);
    vPos.x = (float)GetDlgItemInt(IDC_EDIT_TRACK_X);
    pTrack->SetTranslation(&vPos);
}

void KSceneSFXEditorTrackPage::OnSetTrackY()
{
    if (!m_pDoc)
        return;
    IEKG3DSceneSFXEditor *pEditor = NULL;
    m_pDoc->GetSceneSFXEditor(&pEditor);
    if (!pEditor)
        return;

    IEKG3DSFX* pSFX = NULL;
    pEditor->GetSFX(&pSFX);
    ASSERT(pSFX);

    int nSelectTrack = m_listTrack.GetSelectionMark();
    if (nSelectTrack == -1)
        return;
    else
    {
        MessageBox(TEXT("请选中一个轨道"));
    }

    IEKG3DSFXTrack *pTrack = NULL;
    pSFX->GetTrack(nSelectTrack, &pTrack);
    ASSERT(pTrack);

    D3DXVECTOR3 vPos;
    pTrack->GetTranslation(&vPos);
    vPos.y = (float)GetDlgItemInt(IDC_EDIT_TRACK_Y);
    pTrack->SetTranslation(&vPos);

}

void KSceneSFXEditorTrackPage::OnSetTrackZ()
{
    if (!m_pDoc)
        return;
    IEKG3DSceneSFXEditor *pEditor = NULL;
    m_pDoc->GetSceneSFXEditor(&pEditor);
    if (!pEditor)
        return;

    IEKG3DSFX* pSFX = NULL;
    pEditor->GetSFX(&pSFX);
    ASSERT(pSFX);

    int nSelectTrack = m_listTrack.GetSelectionMark();
    if (nSelectTrack == -1)
        return;
    else
    {
        MessageBox(TEXT("请选中一个轨道"));
    }

    IEKG3DSFXTrack *pTrack = NULL;
    pSFX->GetTrack(nSelectTrack, &pTrack);
    ASSERT(pTrack);

    D3DXVECTOR3 vPos;
    pTrack->GetTranslation(&vPos);
    vPos.z = (float)GetDlgItemInt(IDC_EDIT_TRACK_Z);
    pTrack->SetTranslation(&vPos);

}

void KSceneSFXEditorTrackPage::OnSetInterval()
{
    if (!m_pDoc)
        return;
    IEKG3DSceneSFXEditor *pEditor = NULL;
    m_pDoc->GetSceneSFXEditor(&pEditor);
    if (!pEditor)
        return;

    IEKG3DSFX* pSFX = NULL;
    pEditor->GetSFX(&pSFX);
    ASSERT(pSFX);

    UINT uInterval = GetDlgItemInt(IDC_EDIT_INTERVAL);
    int nSelect = m_listTrack.GetSelectionMark();
    if (nSelect != -1)
    {
        IEKG3DSFXTrack *pTrack = NULL;
        pSFX->GetTrack(nSelect, &pTrack);
        pTrack->SetInterval(uInterval);
    }
    else
    {
        MessageBox(TEXT("请选中一个轨道"));
    }
}

void KSceneSFXEditorTrackPage::OnBnClickedButtonAddTrack()
{
    if (!m_pDoc)
        return;
    IEKG3DSceneSFXEditor *pEditor = NULL;
    m_pDoc->GetSceneSFXEditor(&pEditor);
    if (!pEditor)
        return;

    CString str;
    if (BrowseTrackFileName(&str))
    {
        if (FAILED(pEditor->AddTrack(str, 0, 0, 0)))
            MessageBox("添加轨道失败！");

        SFXUINeedUpdate();
    }

}

void KSceneSFXEditorTrackPage::OnBnClickedButtonDelTrack()
{
    if (!m_pDoc)
        return;
    IEKG3DSceneSFXEditor *pEditor = NULL;
    m_pDoc->GetSceneSFXEditor(&pEditor);
    if (!pEditor)
        return;

    // TODO: Add your control notification handler code here
    int nSelectTrack = m_listTrack.GetSelectionMark();
    if (nSelectTrack == -1)
        return;

    if (FAILED(pEditor->RemoveTrack(nSelectTrack)))
        MessageBox("删除轨道失败！");

    UpdateUI();
}

BOOL KSceneSFXEditorTrackPage::BrowseTrackFileName(CString *pStrName)
{
    char szFilters[] = "Track Files (*.Track)|*.Track|All Files (*.*)|*.*||";

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
        *pStrName = fileDlg.GetPathName();
        return TRUE;
    }

    return FALSE;
}

BOOL KSceneSFXEditorTrackPage::OnSetActive()
{
    // TODO: Add your specialized code here and/or call the base class
    //SFXUINeedUpdate();
    UpdateUI();

    return TRUE;//CPropertyPage::OnSetActive();
}

void KSceneSFXEditorTrackPage::OnBnClickedButtonLauncherBindToTrack()
{
    if (!m_pDoc)
        return;
    IEKG3DSceneSFXEditor *pEditor = NULL;
    m_pDoc->GetSceneSFXEditor(&pEditor);
    if (!pEditor)
        return;

    int nSelectLauncher = m_listLauncher.GetSelectionMark();
    if (nSelectLauncher == -1)
        return;

    int nSelectTrack = m_listTrack.GetSelectionMark();
    if (nSelectTrack == -1)
        return;

    pEditor->LauncherBindToTrack(nSelectLauncher, nSelectTrack);

    SFXUINeedUpdate();
}

void KSceneSFXEditorTrackPage::OnNMClickListTrack(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: Add your control notification handler code here
    *pResult = 0;

    if (!m_pDoc)
        return;
    IEKG3DSceneSFXEditor *pEditor = NULL;
    m_pDoc->GetSceneSFXEditor(&pEditor);
    if (!pEditor)
        return;

    IEKG3DSFX* pSFX = NULL;
    pEditor->GetSFX(&pSFX);
    ASSERT(pSFX);

    int nSelectTrack = m_listTrack.GetSelectionMark();
    if (nSelectTrack == -1)
    {
        m_editTrackFrameNum.SetWindowText("");
        m_editTrackInterval.SetWindowText("");
        m_editTrackX.SetWindowText("");
        m_editTrackY.SetWindowText("");
        m_editTrackZ.SetWindowText("");
        return;
    }

    IEKG3DSFXTrack *pTrack = NULL;
    pSFX->GetTrack(nSelectTrack, &pTrack);
    ASSERT(pTrack);

    CString str;
    D3DXVECTOR3 vPos;
    pTrack->GetTranslation(&vPos);
    str.Format("%f", vPos.x);
    m_editTrackX.SetWindowText(str);
    str.Format("%f", vPos.y);
    m_editTrackY.SetWindowText(str);
    str.Format("%f", vPos.z);
    m_editTrackZ.SetWindowText(str);

    str.Format("%d", pTrack->GetInterval());
    m_editTrackInterval.SetWindowText(str);

    str.Format("%d", pTrack->GetFrameNum());
    m_editTrackFrameNum.SetWindowText(str);
}

void KSceneSFXEditorTrackPage::OnBnClickedButtonEditTrack()
{
    // TODO: Add your control notification handler code here
    int nSelect = m_listTrack.GetSelectionMark();
    if (nSelect == -1)
    {
        MessageBox(TEXT("请选中一个轨道"));
        return;
    }

    if (!m_pDoc)
        return;

    IEKG3DSceneSFXEditor *pEditor = NULL;
    m_pDoc->GetSceneSFXEditor(&pEditor);
    if (!pEditor)
        return;

    IEKG3DSFX* pSFX = NULL;
    pEditor->GetSFX(&pSFX);
    ASSERT(pSFX);

    IEKG3DSFXTrack *pTrack = NULL;
    pSFX->GetTrack(nSelect, &pTrack);

    CString str;
    if (BrowseTrackFileName(&str))
    {
        pEditor->ChangeTrack(nSelect, str);
    }
}

void KSceneSFXEditorTrackPage::OnBnClickedButtonLauncherBindCancel()
{
    // TODO: Add your control notification handler code here
    if (!m_pDoc)
        return;
    IEKG3DSceneSFXEditor *pEditor = NULL;
    m_pDoc->GetSceneSFXEditor(&pEditor);
    if (!pEditor)
        return;

    int nSelectLauncher = m_listLauncher.GetSelectionMark();
    if (nSelectLauncher == -1)
        return;

    pEditor->LauncherUnbindFromTrack(nSelectLauncher);
    SFXUINeedUpdate();
}

void KSceneSFXEditorTrackPage::OnCbnSelchangeComboBindMode()
{
    // TODO: Add your control notification handler code here
    if (!m_pDoc)
        return;
    IEKG3DSceneSFXEditor *pEditor = NULL;
    m_pDoc->GetSceneSFXEditor(&pEditor);
    if (!pEditor)
        return;

    IEKG3DSFX* pSFX = NULL;
    pEditor->GetSFX(&pSFX);
    ASSERT(pSFX);

    int nSelectLauncher = m_listLauncher.GetSelectionMark();
    if (nSelectLauncher == -1)
    {
        m_comboBindMode.SetCurSel(-1);
        return;
    }

    IEKG3DSFXParticleLauncher *pLauncher = NULL;
    pSFX->GetParticleLauncher(nSelectLauncher, &pLauncher);
    ASSERT(pLauncher);

    if (pLauncher->IsBindToTrack())
    {
        int nMode = m_comboBindMode.GetCurSel();
        if (nMode != -1)
        {
            pLauncher->SetBindToTrackMode((SFX_LAUNCHER_BIND_TO_TRACK_MODE)nMode);
        }
    }
}

void KSceneSFXEditorTrackPage::OnNMClickListLauncher(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: Add your control notification handler code here
    *pResult = 0;

    if (!m_pDoc)
        return;
    IEKG3DSceneSFXEditor *pEditor = NULL;
    m_pDoc->GetSceneSFXEditor(&pEditor);
    if (!pEditor)
        return;

    IEKG3DSFX* pSFX = NULL;
    pEditor->GetSFX(&pSFX);
    ASSERT(pSFX);

    int nSelectLauncher = m_listLauncher.GetSelectionMark();
    if (nSelectLauncher == -1)
    {
        m_comboBindMode.SetCurSel(-1);
        return;
    }

    IEKG3DSFXParticleLauncher *pLauncher = NULL;
    pSFX->GetParticleLauncher(nSelectLauncher, &pLauncher);
    ASSERT(pLauncher);

    if (pLauncher->IsBindToTrack())
    {
        SFX_LAUNCHER_BIND_TO_TRACK_MODE eType = pLauncher->GetBindToTrackMode();
        if (eType == SFX_LBTTM_NORMAL)
            m_comboBindMode.SetCurSel(SFX_BIND_TO_TRACK_LAUNCHER_INDEX);
        else if (eType == SFX_LBTTM_PARTICLE_TO_TRACK)
            m_comboBindMode.SetCurSel(SFX_BIND_TO_TRACK_PARTICLE_INDEX);
    }
    else
        m_comboBindMode.SetCurSel(-1);
}

void KSceneSFXEditorTrackPage::OnBnClickedButtonBillboardBindToTrack()
{
    // TODO: Add your control notification handler code here
    if (!m_pDoc)
        return;
    IEKG3DSceneSFXEditor *pEditor = NULL;
    m_pDoc->GetSceneSFXEditor(&pEditor);
    if (!pEditor)
        return;

    int nSelectTrack = m_listTrack.GetSelectionMark();
    if (nSelectTrack == -1)
        return;

    POSITION pos = m_listBillboard.GetFirstSelectedItemPosition();
    if (pos == NULL)
    {
        MessageBox("还没有选择公告板", "大牌提示");
    }
    else
    {
        while (pos)
        {
            int nSelectBillboard = m_listBillboard.GetNextSelectedItem(pos);
            pEditor->BillboardBindToTrack(nSelectBillboard, nSelectTrack);
        }
    }

    SFXUINeedUpdate();
}

void KSceneSFXEditorTrackPage::OnBnClickedButtonBillboardBindCancel()
{
    // TODO: Add your control notification handler code here
    if (!m_pDoc)
        return;
    IEKG3DSceneSFXEditor *pEditor = NULL;
    m_pDoc->GetSceneSFXEditor(&pEditor);
    if (!pEditor)
        return;

    POSITION pos = m_listBillboard.GetFirstSelectedItemPosition();
    if (pos == NULL)
    {
        MessageBox("还没有选择公告板", "大牌提示");
    }
    else
    {
        while (pos)
        {
            int nSelectBillboard = m_listBillboard.GetNextSelectedItem(pos);
            pEditor->BillboardUnbindFromTrack(nSelectBillboard);
        }
    }

    SFXUINeedUpdate();
}

void KSceneSFXEditorTrackPage::OnBnClickedButtonModelBindToTrack()
{
    if (!m_pDoc)
        return;

    IEKG3DSceneSFXEditor *pEditor = NULL;
    m_pDoc->GetSceneSFXEditor(&pEditor);
    if (!pEditor)
        return;

    int nSelectTrack = m_listTrack.GetSelectionMark();
    if (nSelectTrack == -1)
        return;

    POSITION pos = m_listModel.GetFirstSelectedItemPosition();
    if (pos == NULL)
    {
    }
    else
    {
        while (pos)
        {
            int nSelectModel = m_listModel.GetNextSelectedItem(pos);
            pEditor->ModelBindToTrack(nSelectModel, nSelectTrack);
        }
    }

    SFXUINeedUpdate();
}

void KSceneSFXEditorTrackPage::OnBnClickedButtonModelBindCancel()
{
    if (!m_pDoc)
        return;

    IEKG3DSceneSFXEditor *pEditor = NULL;
    m_pDoc->GetSceneSFXEditor(&pEditor);
    if (!pEditor)
        return;

    POSITION pos = m_listModel.GetFirstSelectedItemPosition();
    if (pos == NULL)
    {
    }
    else
    {
        while (pos)
        {
            int nSelectModel = m_listModel.GetNextSelectedItem(pos);
            pEditor->ModeldUnbindFromTrack(nSelectModel);
        }
    }

    SFXUINeedUpdate();
}

void KSceneSFXEditorTrackPage::OnBnClickedButtonMeteorBindToTrack()
{
    if (!m_pDoc)
        return;

    IEKG3DSceneSFXEditor *pEditor = NULL;
    m_pDoc->GetSceneSFXEditor(&pEditor);
    if (!pEditor)
        return;

    int nSelectTrack = m_listTrack.GetSelectionMark();
    if (nSelectTrack == -1)
        return;

    POSITION pos = m_listMeteor.GetFirstSelectedItemPosition();
    if (pos == NULL)
    {
    }
    else
    {
        while (pos)
        {
            int nSelectModel = m_listMeteor.GetNextSelectedItem(pos);
            pEditor->MeteorBindToTrack(nSelectModel, nSelectTrack);
        }
    }

    SFXUINeedUpdate();
}

void KSceneSFXEditorTrackPage::OnBnClickedButtonMeteorBindCancel()
{
    if (!m_pDoc)
        return;

    IEKG3DSceneSFXEditor *pEditor = NULL;
    m_pDoc->GetSceneSFXEditor(&pEditor);
    if (!pEditor)
        return;

    POSITION pos = m_listMeteor.GetFirstSelectedItemPosition();
    if (pos == NULL)
    {
    }
    else
    {
        while (pos)
        {
            int nSelectModel = m_listMeteor.GetNextSelectedItem(pos);
            pEditor->MeteorUnbindFromTrack(nSelectModel);
        }
    }

    SFXUINeedUpdate();
}