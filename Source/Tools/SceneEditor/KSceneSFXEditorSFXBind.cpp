//////////////////////////////////////////////////////////////////////////////////
////
////  FileName    : KSceneSFXEditorSFXBind.cpp
////  Version     : 1.0
////  Creator     : Zeb
////  Create Date : 2005-10-18 17:17:00
////  Comment     :
////
//////////////////////////////////////////////////////////////////////////////////
//
#include "StdAfx.h"
//#include "KSceneSFXEditorSFXBind.h"
//
//

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//
//// KSceneSFXEditorSFX_BIND.cpp : implementation file
////
//#include "SceneEditor.h"
//#include "IEKG3DSceneSFXEditor.h"
//#include "KSceneSFXEditorFrame.h"
//#include "KG3DMeshEx.h"
//#include "IEKG3DSFX.h"
//#include "KSceneSFXEditorSFXBind.h"
//
//// KSceneSFXEditorSFX_BIND dialog
//
//IMPLEMENT_DYNAMIC(KSceneSFXEditorSFXBind, CPropertyPage)
//KSceneSFXEditorSFXBind::KSceneSFXEditorSFXBind()
//	: CPropertyPage(KSceneSFXEditorSFXBind::IDD)
//{
//}
//
//KSceneSFXEditorSFXBind::~KSceneSFXEditorSFXBind()
//{
//}
//
//void KSceneSFXEditorSFXBind::DoDataExchange(CDataExchange* pDX)
//{
//	CPropertyPage::DoDataExchange(pDX);
//
//    DDX_Control(pDX, IDC_EDIT_MESH_NAME, m_editMeshName);
//    DDX_Control(pDX, IDC_EDIT_MTL_NAME, m_editMtlName);
//    DDX_Control(pDX, IDC_EDIT_ANI_NAME, m_editAniName);
//
//    DDX_Control(pDX, IDC_LIST_BONE, m_listBone);
//    DDX_Control(pDX, IDC_LIST_LAUNCHER, m_listLauncher);
//
//    DDX_Control(pDX, IDC_LIST_BILLBOARD, m_listBillboard);
//}
//
//
//BEGIN_MESSAGE_MAP(KSceneSFXEditorSFXBind, CPropertyPage)
//    ON_BN_CLICKED(IDC_BUTTON_BROWSE_MESH, OnBnClickedButtonBrowseMesh)
//    ON_BN_CLICKED(IDC_BUTTON_BROWSE_MTL, OnBnClickedButtonBrowseMtl)
//    ON_BN_CLICKED(IDC_BUTTON_BROWSE_ANI, OnBnClickedButtonBrowseAni)
//    ON_BN_CLICKED(IDC_BUTTON_BIND_TO_BONE, OnBnClickedButtonBindToBone)
//    ON_BN_CLICKED(IDC_RADIO_PLAY, OnBnClickedRadioPlay)
//    ON_BN_CLICKED(IDC_RADIO_STOP, OnBnClickedRadioStop)
//    ON_BN_CLICKED(IDC_BUTTON_BIND_CANCEL, OnBnClickedButtonBindCancel)
//    ON_BN_CLICKED(IDC_BUTTON_BILLBOARD_BIND_TO_BONE, OnBnClickedButtonBillboardBindToBone)
//    ON_BN_CLICKED(IDC_BUTTON_BILLBOARD_BIND_CANCEL, OnBnClickedButtonBillboardBindCancel)
//END_MESSAGE_MAP()
//
//
//// KSceneSFXEditorSFX_BIND message handlers
//
//void KSceneSFXEditorSFXBind::OnBnClickedButtonBrowseMesh()
//{
//    if (m_pDoc && m_pDoc->GetScene())
//    {
//        // TODO: Add your control notification handler code here
//        char szFilters[] = "Mesh Files|*.Mesh|All Files (*.*)|*.*||";
//
//        CFileDialog fileDlg(
//            TRUE,
//            NULL,
//            NULL,
//            OFN_EXPLORER|OFN_FILEMUSTEXIST| OFN_HIDEREADONLY,
//            szFilters,
//            this
//        );
//
//        if (fileDlg.DoModal() == IDOK)
//        {
//            IEKG3DSceneSFXEditor *pEditor =
//                (IEKG3DSceneSFXEditor*)m_pDoc->GetScene();
//            if (FAILED(pEditor->LoadMesh(fileDlg.GetPathName().GetBuffer(0))))
//                MessageBox("载入Mesh失败！");
//            else
//            {
//                m_editMeshName.SetWindowText(fileDlg.GetPathName());
//                BindSfx();
//                UpdateBoneListUI();
//                UpdateUI();
//            }
//        }
//    }
//}
//
//void KSceneSFXEditorSFXBind::OnBnClickedButtonBrowseMtl()
//{
//    // TODO: Add your control notification handler code here
//    if (m_pDoc && m_pDoc->GetScene())
//    {
//        // TODO: Add your control notification handler code here
//        char szFilters[] = "Material Files|*.Mtl|All Files (*.*)|*.*||";
//
//        CFileDialog fileDlg(
//            TRUE,
//            NULL,
//            NULL,
//            OFN_EXPLORER|OFN_FILEMUSTEXIST| OFN_HIDEREADONLY,
//            szFilters,
//            this
//        );
//
//        if (fileDlg.DoModal() == IDOK)
//        {
//            IEKG3DSceneSFXEditor *pEditor =
//                (IEKG3DSceneSFXEditor*)m_pDoc->GetScene();
//            if (FAILED(pEditor->LoadMaterial(fileDlg.GetPathName().GetBuffer(0))))
//                MessageBox("载入Material失败！");
//            else
//                m_editMtlName.SetWindowText(fileDlg.GetPathName());
//        }
//    }
//}
//
//void KSceneSFXEditorSFXBind::OnBnClickedButtonBrowseAni()
//{
//    // TODO: Add your control notification handler code here
//    if (m_pDoc && m_pDoc->GetScene())
//    {
//        // TODO: Add your control notification handler code here
//        char szFilters[] = "Animation Files|*.Ani|All Files (*.*)|*.*||";
//
//        CFileDialog fileDlg(
//            TRUE,
//            NULL,
//            NULL,
//            OFN_EXPLORER|OFN_FILEMUSTEXIST| OFN_HIDEREADONLY,
//            szFilters,
//            this
//        );
//
//        if (fileDlg.DoModal() == IDOK)
//        {
//            IEKG3DSceneSFXEditor *pEditor =
//                (IEKG3DSceneSFXEditor*)m_pDoc->GetScene();
//            if (FAILED(pEditor->LoadAnimation(fileDlg.GetPathName().GetBuffer(0))))
//                MessageBox("载入Animation失败！");
//            else
//            {
//                m_editAniName.SetWindowText(fileDlg.GetPathName());
//            }
//        }
//    }
//}
//
//void KSceneSFXEditorSFXBind::UpdateUI()
//{
//    UpdateBindInfo();
//    UpdateBoneListBindInfoUI();
//    UpdateLauncherListUI();
//    UpdateBillboardListUI();
//}
//
//
//void KSceneSFXEditorSFXBind::UpdateBoneListBindInfoUI()
//{
//    if (m_pDoc == NULL || m_pDoc->GetScene() == NULL)
//        return;
//
//    IEKG3DSceneSFXEditor *pEditor = (IEKG3DSceneSFXEditor*)m_pDoc->GetScene();
//    if (pEditor->m_pModel == NULL)
//        return;
//
//    KG3DMeshEx *pMesh = (KG3DMeshEx*)pEditor->m_pModel->GetMesh();
//
//    DWORD dwNumBone;
//    pMesh->GetNumBones(dwNumBone);
//
//    int i;
//    for (i = 0; i < (int)dwNumBone; ++i)
//    {
//        CString str;
//        KG3DMesh::_BoneInfo *pBoneInfo = NULL;
//        pMesh->GetBoneInfo(i, &pBoneInfo);
//        str.Format("(%d)", pEditor->m_vecBindInformation[i].vecBillboard.size());
//        str = pBoneInfo->scName.c_str() + str;
//        m_listBone.SetItemText(i, 0, str);
//
//        if (pEditor->m_vecBindInformation[i].nLauncher1 != -1)
//        {
//            str.Format("发射器%d", pEditor->m_vecBindInformation[i].nLauncher1 + 1);
//            m_listBone.SetItemText(i, 1, str);
//        }
//        else
//            m_listBone.SetItemText(i, 1, "");
//
//
//        if (pEditor->m_vecBindInformation[i].nLauncher2 != -1)
//        {
//            str.Format("发射器%d", pEditor->m_vecBindInformation[i].nLauncher2 + 1);
//            m_listBone.SetItemText(i, 2, str);
//        }
//        else
//            m_listBone.SetItemText(i, 2, "");
//   }
//
//    pMesh->Release();
//}
//
//void KSceneSFXEditorSFXBind::UpdateBoneListUI()
//{
//    if (m_pDoc == NULL || m_pDoc->GetScene() == NULL)
//        return;
//
//    IEKG3DSceneSFXEditor *pEditor = (IEKG3DSceneSFXEditor*)m_pDoc->GetScene();
//    KG3DMeshEx *pMesh = (KG3DMeshEx*)pEditor->m_pModel->GetMesh();
//
//    DWORD dwNumBone;
//    pMesh->GetNumBones(dwNumBone);
//
//    int i;
//    KG3DMesh::_BoneInfo *pBoneInfo = NULL;
//    for (i = dwNumBone - 1; i >= 0; --i)
//    {
//        pMesh->GetBoneInfo(i, &pBoneInfo);
//        CString strBoneName = pBoneInfo->scName.c_str();
//        m_listBone.InsertItem(0, strBoneName);
//    }
//
//    pMesh->Release();
//}
//
//void KSceneSFXEditorSFXBind::UpdateLauncherListUI()
//{
//    if (m_pDoc == NULL || m_pDoc->GetScene() == NULL)
//        return;
//
//    IEKG3DSceneSFXEditor *pEditor = (IEKG3DSceneSFXEditor*)m_pDoc->GetScene();
//    KG3DSFX *pSFX = pEditor->m_lpSFX;
//    ASSERT(pSFX);
//
//    DWORD dwLauncherNum = pSFX->GetParticleLauncherNum();
//    m_listLauncher.DeleteAllItems();
//    while (dwLauncherNum > 0)
//    {
//        dwLauncherNum--;
//
//        CString str;
//        str.Format("发射器%d", dwLauncherNum + 1);
//        m_listLauncher.InsertItem(0, str);
//
//        KG3DSFXParticleLauncher *pLauncher = NULL;
//        pSFX->GetParticleLauncher(dwLauncherNum, &pLauncher);
//        ASSERT(pLauncher);
//
//        if (pLauncher->IsBindToBone())
//        {
//            CString str = pLauncher->GetBindName().c_str();
//            m_listLauncher.SetItemText(0, 1, str);
//        }
//    }
//}
//
//void KSceneSFXEditorSFXBind::UpdateBillboardListUI()
//{
//    if (m_pDoc == NULL || m_pDoc->GetScene() == NULL)
//        return;
//
//    IEKG3DSceneSFXEditor *pEditor = (IEKG3DSceneSFXEditor*)m_pDoc->GetScene();
//    KG3DSFX *pSFX = pEditor->m_lpSFX;
//    ASSERT(pSFX);
//
//    DWORD dwBillboard = pSFX->GetBillboardNum();
//    m_listBillboard.DeleteAllItems();
//    while (dwBillboard > 0)
//    {
//        dwBillboard--;
//
//        CString str;
//        str.Format("公告板%d", dwBillboard + 1);
//        m_listBillboard.InsertItem(0, str);
//
//        KG3DSFXBillboard *pBillboard = NULL;
//        pSFX->GetBillboard(dwBillboard, &pBillboard);
//        ASSERT(pBillboard);
//
//        if (pBillboard->IsBindToBone())
//        {
//            CString str = pBillboard->GetBindName().c_str();
//            m_listBillboard.SetItemText(0, 1, str);
//        }
//    }
//}
//
//BOOL KSceneSFXEditorSFXBind::OnInitDialog()
//{
//    CPropertyPage::OnInitDialog();
//
//    // TODO:  Add extra initialization here
//    m_listBone.InsertColumn(0, _T("BoneName"),	LVCFMT_CENTER, 100);
//    m_listBone.InsertColumn(1, _T("Launcher1"), LVCFMT_CENTER, 50);
//    m_listBone.InsertColumn(2, _T("Launcher2"), LVCFMT_CENTER, 50);
//
//    m_listLauncher.InsertColumn(0, _T("Launcher"),	LVCFMT_CENTER, 70);
//    m_listLauncher.InsertColumn(1, _T("BoneName"), LVCFMT_CENTER, 100);
//
//    m_listBillboard.InsertColumn(0, _T("Billboard"),	LVCFMT_CENTER, 70);
//    m_listBillboard.InsertColumn(1, _T("BoneName"), LVCFMT_CENTER, 100);
//
//    m_listBone.SetExtendedStyle(
//        m_listBone.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES
//    );
//
//    m_listLauncher.SetExtendedStyle(
//        m_listLauncher.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES
//        );
//
//    m_listBillboard.SetExtendedStyle(
//        m_listBillboard.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES
//        );
//
//    return TRUE;  // return TRUE unless you set the focus to a control
//    // EXCEPTION: OCX Property Pages should return FALSE
//}
//
//BOOL KSceneSFXEditorSFXBind::OnSetActive()
//{
//    // TODO: Add your specialized code here and/or call the base class
//    UpdateUI();
//
//    return CPropertyPage::OnSetActive();
//}
//
//void KSceneSFXEditorSFXBind::BindSfx()
//{
//    // TODO: Add your control notification handler code here
//    if (m_pDoc == NULL || m_pDoc->GetScene() == NULL)
//        return;
//
//    IEKG3DSceneSFXEditor *pEditor = (IEKG3DSceneSFXEditor*)m_pDoc->GetScene();
//    KG3DSFX *pSFX = pEditor->m_lpSFX;
//    ASSERT(pSFX);
//
//    ASSERT(pEditor->m_pModel);
//    pSFX->BindToModel(pEditor->m_pModel);
//}
//
//void KSceneSFXEditorSFXBind::UpdateBindInfo()
//{
//    if (m_pDoc == NULL || m_pDoc->GetScene() == NULL)
//        return;
//
//    IEKG3DSceneSFXEditor *pEditor = (IEKG3DSceneSFXEditor*)m_pDoc->GetScene();
//    KG3DSFX *pSFX = pEditor->m_lpSFX;
//    ASSERT(pSFX);
//
//    if (pEditor->m_pModel == NULL)
//        return;
//
//    KG3DMesh *pMesh = (KG3DMesh*)pEditor->m_pModel->GetMesh();
//    if (pMesh == NULL)
//        return;
//
//    pMesh->Release();
//
//    int i;
//    for (i = 0; i < (int)pEditor->m_vecBindInformation.size(); ++i)
//    {
//        pEditor->m_vecBindInformation[i].nLauncher1 = -1;
//        pEditor->m_vecBindInformation[i].nLauncher2 = -1;
//        pEditor->m_vecBindInformation[i].vecBillboard.clear();
//    }
//
//    DWORD dwLaucherNum = pEditor->m_lpSFX->GetParticleLauncherNum();
//    for (i = 0; i < (int)dwLaucherNum; ++i)
//    {
//        KG3DSFXParticleLauncher *pLauncher = NULL;
//        pEditor->m_lpSFX->GetParticleLauncher(i, &pLauncher);
//        ASSERT(pLauncher);
//
//        if (pLauncher->IsBindToBone())
//        {
//            int nBoneIndex = pMesh->FindBone(pLauncher->GetBindName().c_str());
//            if (nBoneIndex >= 0)
//            {
//                ASSERT(nBoneIndex < (int)pEditor->m_vecBindInformation.size());
//                int a = pEditor->m_vecBindInformation[nBoneIndex].nLauncher1;
//                int b = pEditor->m_vecBindInformation[nBoneIndex].nLauncher2;
//
//                ASSERT(nBoneIndex < (int)pEditor->m_vecBindInformation.size());
//                if (pEditor->m_vecBindInformation[nBoneIndex].nLauncher1 == -1)
//                    pEditor->m_vecBindInformation[nBoneIndex].nLauncher1 = i;
//                else if (pEditor->m_vecBindInformation[nBoneIndex].nLauncher2 == -1)
//                    pEditor->m_vecBindInformation[nBoneIndex].nLauncher2 = i;
//                else
//                    ASSERT(0);
//            }
//        }
//    }
//
//    DWORD dwBillboardNum = pEditor->m_lpSFX->GetBillboardNum();
//    for (i = 0; i < (int)dwBillboardNum; ++i)
//    {
//        KG3DSFXBillboard *pBillboard = NULL;
//        pEditor->m_lpSFX->GetBillboard(i, &pBillboard);
//        ASSERT(pBillboard);
//
//        if (pBillboard->IsBindToBone())
//        {
//            int nBoneIndex = pMesh->FindBone(pBillboard->GetBindName().c_str());
//            if (nBoneIndex >= 0)
//            {
//                ASSERT(nBoneIndex < (int)pEditor->m_vecBindInformation.size());
//
//                pEditor->m_vecBindInformation[nBoneIndex].vecBillboard.push_back(i);
//            }
//        }
//    }
//}
//
//void KSceneSFXEditorSFXBind::OnBnClickedButtonBindToBone()
//{
//    // TODO: Add your control notification handler code here
//    if (m_listLauncher.GetSelectedCount() > 1)
//    {
//        MessageBox("绑定的时候一次只能选中一个发射器！", "大牌提示：");
//        return;
//    }
//
//    if (m_pDoc == NULL || m_pDoc->GetScene() == NULL)
//        return;
//
//    IEKG3DSceneSFXEditor *pEditor = (IEKG3DSceneSFXEditor*)m_pDoc->GetScene();
//    KG3DSFX *pSFX = pEditor->m_lpSFX;
//    ASSERT(pSFX);
//
//    int nSelectLauncher = m_listLauncher.GetSelectionMark();
//    if (nSelectLauncher == -1)
//        return;
//
//    KG3DSFXParticleLauncher *pLauncher = NULL;
//    pSFX->GetParticleLauncher(nSelectLauncher, &pLauncher);
//    ASSERT(pLauncher);
//
//    POSITION pos = m_listBone.GetFirstSelectedItemPosition();
//    if (pos == NULL)
//    {
//        MessageBox("请先选中需要绑定的骨头", "大牌提示：");
//        return;
//    }
//    else
//    {
//        while (pos)
//        {
//            int nItem = m_listBone.GetNextSelectedItem(pos);
//
//            ASSERT(nItem >= 0);
//            ASSERT(nItem < (int)pEditor->m_vecBindInformation.size());
//
//            CString strBoneName;
//            KG3DMeshEx *pMesh = (KG3DMeshEx*)pEditor->m_pModel->GetMesh();
//            pMesh->Release();
//
//            KG3DMesh::_BoneInfo *pBoneInfo = NULL;
//            pMesh->GetBoneInfo(nItem, &pBoneInfo);
//            strBoneName = pBoneInfo->scName.c_str();
//
//            if (
//                pEditor->m_vecBindInformation[nItem].nLauncher1 != -1 &&
//                pEditor->m_vecBindInformation[nItem].nLauncher2 != -1
//            )
//            {
//                MessageBox("每个骨头限制只能绑定2个发射器！", "大牌提示：");
//            }
//            else
//            {
//                if (pLauncher->IsBindToBone())
//                {
//                    CString str = pLauncher->GetBindName().c_str();
//                    if (str != strBoneName)
//                    {
//                        KG3DSFXParticleLauncher *pCloneLauncher = NULL;
//                        pLauncher->Clone(&pCloneLauncher);
//
//                        pLauncher = pCloneLauncher;
//                        nSelectLauncher = pSFX->GetParticleLauncherNum() - 1;
//                    }
//                    else
//                    {
//                        MessageBox("这个发射器已经绑定到这个骨头上了！", "大牌提示：");
//                        continue;
//                    }
//                }
//
//                if (SUCCEEDED(pLauncher->BindToBone(strBoneName)))
//                {
//                    if (pEditor->m_vecBindInformation[nItem].nLauncher1 == -1)
//                        pEditor->m_vecBindInformation[nItem].nLauncher1 = nSelectLauncher;
//                    else if (pEditor->m_vecBindInformation[nItem].nLauncher2 == -1)
//                        pEditor->m_vecBindInformation[nItem].nLauncher2 = nSelectLauncher;
//                    else
//                        ASSERT(0);
//                }
//            }
//        }
//    }
//
//    UpdateUI();
//}
//
//void KSceneSFXEditorSFXBind::OnBnClickedRadioPlay()
//{
//    // TODO: Add your control notification handler code here
//    if (m_pDoc == NULL || m_pDoc->GetScene() == NULL)
//        return;
//
//    IEKG3DSceneSFXEditor *pEditor = (IEKG3DSceneSFXEditor*)m_pDoc->GetScene();
//
//    if (pEditor->m_pModel == NULL)
//        return;
//
//    pEditor->m_pModel->GetAnimationController(ANICTL_PRIMARY)()->SetSpeed(1.0f);
//    pEditor->m_pModel->PlayAnimation(1, "", 1.0F,0, NULL);
//}
//
//void KSceneSFXEditorSFXBind::OnBnClickedRadioStop()
//{
//    // TODO: Add your control notification handler code here
//    if (m_pDoc == NULL || m_pDoc->GetScene() == NULL)
//        return;
//
//    IEKG3DSceneSFXEditor *pEditor = (IEKG3DSceneSFXEditor*)m_pDoc->GetScene();
//
//    pEditor->m_pModel->GetAnimationController(ANICTL_PRIMARY)()->PauseAnimation(TRUE);
//}
//
//void KSceneSFXEditorSFXBind::OnBnClickedButtonBindCancel()
//{
//    // TODO: Add your control notification handler code here
//    if (m_pDoc == NULL || m_pDoc->GetScene() == NULL)
//        return;
//
//    IEKG3DSceneSFXEditor *pEditor = (IEKG3DSceneSFXEditor*)m_pDoc->GetScene();
//    KG3DSFX *pSFX = pEditor->m_lpSFX;
//    ASSERT(pSFX);
//
//    POSITION pos = m_listLauncher.GetFirstSelectedItemPosition();
//    while (pos)
//    {
//        int nSelectLauncher = m_listLauncher.GetNextSelectedItem(pos);
//        KG3DSFXParticleLauncher *pLauncher = NULL;
//        pSFX->GetParticleLauncher(nSelectLauncher, &pLauncher);
//        ASSERT(pLauncher);
//
//        KG3DMeshEx *pMesh = NULL;
//        if (pEditor->m_pModel)
//        {
//            pMesh = (KG3DMeshEx*)pEditor->m_pModel->GetMesh();
//            pMesh->Release();
//        }
//
//        if (pLauncher->IsBindToBone())
//        {
//            if (pMesh == NULL)
//            {
//                pLauncher->UnBind();
//            }
//            else
//            {
//                CString strBoneName = pLauncher->GetBindName().c_str();
//                int nBoneIndex = pMesh->FindBone(strBoneName);
//                if (nBoneIndex >= 0)
//                {
//                    if (pEditor->m_vecBindInformation[nBoneIndex].nLauncher1 == nSelectLauncher)
//                        pEditor->m_vecBindInformation[nBoneIndex].nLauncher1 = -1;
//                    else if (pEditor->m_vecBindInformation[nBoneIndex].nLauncher2 == nSelectLauncher)
//                        pEditor->m_vecBindInformation[nBoneIndex].nLauncher2 = -1;
//                    else
//                        ASSERT(0);
//
//                    pLauncher->UnBind();
//                }
//                else
//                    ASSERT(0);
//            }
//        }
//    }
//
//    UpdateUI();
//}
//
//void KSceneSFXEditorSFXBind::OnBnClickedButtonBillboardBindToBone()
//{
//    // TODO: Add your control notification handler code here
//    if (m_listBillboard.GetSelectedCount() > 1)
//    {
//        MessageBox("绑定的时候一次只能选中一个公告板！", "大牌提示：");
//        return;
//    }
//
//    if (m_pDoc == NULL || m_pDoc->GetScene() == NULL)
//        return;
//
//    IEKG3DSceneSFXEditor *pEditor = (IEKG3DSceneSFXEditor*)m_pDoc->GetScene();
//    KG3DSFX *pSFX = pEditor->m_lpSFX;
//    ASSERT(pSFX);
//
//    int nSelectBillboard = m_listBillboard.GetSelectionMark();
//    if (nSelectBillboard == -1)
//        return;
//
//    KG3DSFXBillboard *pBillboard = NULL;
//    pSFX->GetBillboard(nSelectBillboard, &pBillboard);
//    ASSERT(pBillboard);
//
//    POSITION pos = m_listBone.GetFirstSelectedItemPosition();
//    if (pos == NULL)
//    {
//        MessageBox("请先选中需要绑定的骨头", "大牌提示：");
//        return;
//    }
//    else
//    {
//        while (pos)
//        {
//            int nItem = m_listBone.GetNextSelectedItem(pos);
//
//            ASSERT(nItem >= 0);
//            ASSERT(nItem < (int)pEditor->m_vecBindInformation.size());
//
//            CString strBoneName;
//            KG3DMeshEx *pMesh = (KG3DMeshEx*)pEditor->m_pModel->GetMesh();
//            pMesh->Release();
//
//            KG3DMesh::_BoneInfo *pBoneInfo = NULL;
//            pMesh->GetBoneInfo(nItem, &pBoneInfo);
//            strBoneName = pBoneInfo->scName.c_str();
//
//            if (pBillboard->IsBindToBone())
//            {
//                CString str = pBillboard->GetBindName().c_str();
//                if (str != strBoneName)
//                {
//                    KG3DSFXBillboard *pCloneBillboard = NULL;
//                    pBillboard->Clone(&pCloneBillboard);
//
//                    pBillboard = pCloneBillboard;
//                    nSelectBillboard = pSFX->GetBillboardNum() - 1;
//                }
//                else
//                {
//                    MessageBox("这个公告板已经绑定到这个骨头上了！", "大牌提示：");
//                    continue;
//                }
//            }
//
//            if (SUCCEEDED(pBillboard->BindToBone(strBoneName)))
//            {
//                for (int i = 0; i < (int)pEditor->m_vecBindInformation[nItem].vecBillboard.size(); ++i)
//                {
//                    if (pEditor->m_vecBindInformation[nItem].vecBillboard[i] == nSelectBillboard)
//                        ASSERT(0);
//                }
//
//                pEditor->m_vecBindInformation[nItem].vecBillboard.push_back(nSelectBillboard);
//            }
//        }
//    }
//
//    UpdateUI();
//}
//
//void KSceneSFXEditorSFXBind::OnBnClickedButtonBillboardBindCancel()
//{
//    // TODO: Add your control notification handler code here
//    if (m_pDoc == NULL || m_pDoc->GetScene() == NULL)
//        return;
//
//    IEKG3DSceneSFXEditor *pEditor = (IEKG3DSceneSFXEditor*)m_pDoc->GetScene();
//    KG3DSFX *pSFX = pEditor->m_lpSFX;
//    ASSERT(pSFX);
//
//    POSITION pos = m_listBillboard.GetFirstSelectedItemPosition();
//    while (pos)
//    {
//        int nSelectBillboard = m_listBillboard.GetNextSelectedItem(pos);
//        KG3DSFXBillboard *pBillboard = NULL;
//        pSFX->GetBillboard(nSelectBillboard, &pBillboard);
//        ASSERT(pBillboard);
//
//        KG3DMeshEx *pMesh = NULL;
//        if (pEditor->m_pModel)
//        {
//            pMesh = (KG3DMeshEx*)pEditor->m_pModel->GetMesh();
//            pMesh->Release();
//        }
//
//        if (pBillboard->IsBindToBone())
//        {
//            if (pMesh == NULL)
//            {
//                pBillboard->UnBind();
//            }
//            else
//            {
//                CString strBoneName = pBillboard->GetBindName().c_str();
//                int nBoneIndex = pMesh->FindBone(strBoneName);
//                if (nBoneIndex >= 0)
//                {
//                    int i = 0;
//                    for (; i < (int)pEditor->m_vecBindInformation[nBoneIndex].vecBillboard.size(); ++i)
//                    {
//                        if (pEditor->m_vecBindInformation[nBoneIndex].vecBillboard[i] == nSelectBillboard)
//                        {
//                            pEditor->m_vecBindInformation[nBoneIndex].vecBillboard.erase(
//                                pEditor->m_vecBindInformation[nBoneIndex].vecBillboard.begin() + i
//                            );
//                            pBillboard->UnBind();
//                            break;
//                        }
//                    }
//
//                    if (i > (int)pEditor->m_vecBindInformation[nBoneIndex].vecBillboard.size())
//                        ASSERT(0);
//                }
//                else
//                    ASSERT(0);
//            }
//        }
//    }
//
//    UpdateUI();
//}
