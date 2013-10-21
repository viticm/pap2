////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSFXEditorModel.cpp
//  Version     : 1.0
//  Creator     : Zeb
//  Create Date : 2006-1-4 15:56:29
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KSceneSFXEditorModel.h"
//#include "KG3DSceneEditorBase.h"
#include "IEKG3DMaterial.h"
#include "KSceneHelperFuncs.h"

////////////////////////////////////////////////////////////////////////////////

// KSceneSFXEditorModel.cpp : implementation file
//
//(by dengzhihui 2006年7月5日 16:27:48
//#include "MainFrm.h"
//)


#include "KSceneSFXEditorFrame.h"

#include "IEKG3DSFX.h"
#include "KColorDialog.h"
#include "IEKG3DAnimationController.h"
#include "IEKG3DClip.h"
// KSceneSFXEditorModel dialog

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNAMIC(KSceneSFXEditorModel, CDialog)
KSceneSFXEditorModel::KSceneSFXEditorModel(CWnd* pWnd /* = NULL */)
	: CDialog(KSceneSFXEditorModel::IDD, pWnd)
{
    m_dwCurIndex = 0xFFFFFFFF;
    m_dwNumFrame = 0;
    m_pDoc = NULL;
}

KSceneSFXEditorModel::~KSceneSFXEditorModel()
{
}

void KSceneSFXEditorModel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_EDIT_MESH_NAME, m_editMeshName);
    DDX_Control(pDX, IDC_EDIT_MTL_NAME, m_editMtlName);
    DDX_Control(pDX, IDC_EDIT_ANI_NAME, m_editAniName);

    DDX_Control(pDX, IDC_BUTTON_BROWSE_MESH, m_buttonMdl);
    DDX_Control(pDX, IDC_BUTTON_BROWSE_MTL, m_buttonMtl);
    DDX_Control(pDX, IDC_BUTTON_BROWSE_ANI, m_buttonAin);

    DDX_Control(pDX, IDC_EDIT_ANIM_SPEED, m_editAnimationSpeed);
    DDX_Control(pDX, IDC_EDIT_ANIM_START, m_editAnimationStart);

    DDX_Control(pDX, IDC_COMBO_SUBSET, m_comboSubset);
    DDX_Control(pDX, IDC_PIC_SUBSET_COLOR, m_picSubsetColor);

	DDX_Text(pDX, IDC_FRAME_COUNT, m_dwNumFrame);
    DDX_Control(pDX, IDC_COMBO_INDEX, m_comboModels);

    DDX_Control(pDX, IDC_BUTTON_ADD, m_buttonAdd);
    DDX_Control(pDX, IDC_BUTTON_DEL, m_buttonDel);


}


BEGIN_MESSAGE_MAP(KSceneSFXEditorModel, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_BROWSE_MESH, OnBnClickedButtonBrowseMesh)
    ON_BN_CLICKED(IDC_BUTTON_BROWSE_ANI, OnBnClickedButtonBrowseAni)
    ON_CBN_DROPDOWN(IDC_COMBO_SUBSET, OnCbnDropdownComboSubset)
    ON_STN_CLICKED(IDC_PIC_SUBSET_COLOR, OnStnClickedPicSubsetColor)
    ON_CBN_SELCHANGE(IDC_COMBO_SUBSET, OnCbnSelchangeComboSubset)
    ON_BN_CLICKED(IDC_BUTTON_BROWSE_MTL, OnBnClickedButtonBrowseMtl)
	ON_BN_CLICKED(IDC_LOOP, &KSceneSFXEditorModel::OnBnClickedLoop)
    ON_CBN_SELCHANGE(IDC_COMBO_INDEX, OnCbnSelchangeComboModel)
    ON_BN_CLICKED(IDC_BUTTON_ADD, &KSceneSFXEditorModel::OnBnClickedButtonAdd)
    ON_BN_CLICKED(IDC_BUTTON_DEL, &KSceneSFXEditorModel::OnBnClickedButtonDel)
	ON_BN_CLICKED(IDC_BILLBORD, &KSceneSFXEditorModel::OnBnClickedBillbord)
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

HBRUSH KSceneSFXEditorModel::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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


void KSceneSFXEditorModel::OnCbnSelchangeComboModel()
{
    m_dwCurIndex = m_comboModels.GetCurSel();
    m_comboSubset.SetCurSel(-1);
    UpdateUI();
}

void KSceneSFXEditorModel::Check()
{
    GET_SFX_EDITOR();
    IEKG3DSFX *pSFX = NULL;

    pScene->GetSFX(&pSFX);

    int nNumModels = (int)pSFX->GetNumModels() - 1;
    if ((int)m_dwCurIndex  > nNumModels)
        m_dwCurIndex  = (DWORD)nNumModels;
}

void KSceneSFXEditorModel::UpdateUI()
{
    if (!::IsWindow(m_hWnd))
        return;

    //if (GetParent() && GetParent()->GetParent())
    //{
    //    KGPropertySheet* pSheet = (KGPropertySheet*)GetParent()->GetParent();
    //    if ((CWnd*)pSheet->GetActivePage() != GetParent())
    //        return;
    //}

    DWORD dwNumModels = 0;
    IEKG3DSFX *pSFX = NULL;
    IEKG3DModel *pModel = NULL;

    GET_SFX_EDITOR();
    pScene->GetSFX(&pSFX);
    
    if (m_dwCurIndex == -1)
    {
        dwNumModels = pSFX->GetNumModels();

        GetDlgItem(IDC_FRAME_COUNT)->SetWindowText(TEXT(""));
        m_comboModels.ResetContent();
        for (DWORD s = 0; s < dwNumModels; ++s)
        {
            TCHAR szModelIndex[128];
            sprintf(szModelIndex, "%s%u", TEXT("模型-"), s);
            m_comboModels.AddString(szModelIndex);
        }
        m_comboModels.SetCurSel(-1);
        m_comboSubset.ResetContent();
        m_editMeshName.SetWindowText(TEXT(""));
        m_editMtlName.SetWindowText(TEXT(""));
        m_editAniName.SetWindowText(TEXT(""));
        m_editAnimationSpeed.SetWindowText(TEXT(""));
        m_editAnimationSpeed.SetWindowText(TEXT(""));
        pFrame->GetKeysFrame()->SetTimeLine(TEXT(""), NULL, SFX_TLDT_NONE);
        GetDlgItem(IDC_BILLBORD)->EnableWindow(FALSE);
        return;
    }
    GetDlgItem(IDC_BILLBORD)->EnableWindow(TRUE);


    if (!pSFX->GetTimeLineModelAniBoundSize())
        return;
	pFrame->GetKeysFrame()->SetTimeLine(
		"模型帧范围", pSFX->GetModelAniBoundTimeLine(m_dwCurIndex), SFX_TLDT_FLOAT
	);


	//
	m_dwNumFrame = pSFX->GetMdlNumFrame(m_dwCurIndex);
	m_fFrameTime =pSFX->GetNdlFrameTime(m_dwCurIndex);
    CButton* pLoopButton = (CButton*)(GetDlgItem(IDC_LOOP));
    if (pLoopButton)
    {
	    if (pSFX->IsMdlAniConnect(m_dwCurIndex))
		    pLoopButton->SetCheck(TRUE);
	    else
		    pLoopButton->SetCheck(FALSE);
    }

    CButton* pIsBillBoardBotton = (CButton*)(GetDlgItem(IDC_BILLBORD));
    if (pIsBillBoardBotton)
    {
        if (pSFX->IsMdlBillBoard(m_dwCurIndex))
            pIsBillBoardBotton->SetCheck(TRUE);
        else
            pIsBillBoardBotton->SetCheck(FALSE);
    }

    if (pSFX->GetModel(m_dwCurIndex) == NULL)
    {
        m_editMeshName.SetWindowText("");
        m_editMtlName.SetWindowText("");
        m_editAniName.SetWindowText("");

        m_comboSubset.ResetContent();
        m_editAnimationStart.SetWindowText("");
        m_editAnimationSpeed.SetWindowText("");
        return;
    }

    pSFX->GetModel(m_dwCurIndex, &pModel);
    if (!pModel)
        return;

    CString str;
    float fValue;
    pSFX->GetModelAnimationSpeed(&fValue, m_dwCurIndex);
    str.Format("%f", fValue);
    m_editAnimationSpeed.SetWindowText(str);

    pSFX->GetModelAnimationStart(&fValue, m_dwCurIndex);
    str.Format("%f", fValue);
    m_editAnimationStart.SetWindowText(str);

    m_comboSubset.ResetContent();
    
    int nSize = pSFX->GetSubsetColorLineSize(m_dwCurIndex);
    for (int i = 0; i < nSize; ++i)
    {
        CString str;
        str.Format("Subset %d", i);
        m_comboSubset.InsertString(0, str);
    }

    int nSelect = m_comboSubset.GetCurSel();
    if (nSelect == CB_ERR)
        m_picSubsetColor.EnableWindow(FALSE);
    else
        m_picSubsetColor.EnableWindow();

    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();

    pSFX->GetModel(m_dwCurIndex, &pModel);
    if (!pModel)
        return;

    if (nSelect != CB_ERR)
    {
        D3DXCOLOR color;

		TKG3DTimeLine<D3DXCOLOR>* pColorLine =
			static_cast<TKG3DTimeLine<D3DXCOLOR>*>(pSFX->GetSubsetColorLine(m_dwCurIndex,nSelect));

        pColorLine->GetData(&color, (float)nFrame);

        DWORD dwColor = color;
        CRect rect;
        m_picSubsetColor.GetClientRect (rect);
        CClientDC dc(&m_picSubsetColor);
        CBrush Brush(RGB(D3DCOLOR_GETRED(dwColor), D3DCOLOR_GETGREEN(dwColor), D3DCOLOR_GETBLUE(dwColor)));
        CBrush* pOldBrush = dc.SelectObject(&Brush);
        dc.Rectangle(&rect);
        dc.SelectObject(pOldBrush);

    }

    char szBuffer[MAX_PATH];
    pModel->GetMeshFileName(szBuffer);
    m_editMeshName.SetWindowText(szBuffer);
    pModel->GetMaterialFileName(szBuffer);
    m_editMtlName.SetWindowText(szBuffer);
    pModel->GetAnimationFileName(szBuffer, MAX_PATH);
    m_editAniName.SetWindowText(szBuffer);

    //
    m_comboModels.ResetContent();
    for (size_t s = 0; s < pSFX->GetNumModels(); s++)
    {
        TCHAR szModelIndex[128];
        sprintf(szModelIndex, "%s%u", TEXT("模型-"), s);
        m_comboModels.AddString(szModelIndex);
    }
    m_comboModels.SetCurSel(m_dwCurIndex);

    IEKG3DSceneEditorBase *piSceneEditorBase = NULL;
    pScene->GetSceneEditBase(&piSceneEditorBase);

	//piSceneEditorBase->m_listSelectedEntity.clear();
    //KG3DSceneEntity entity(SCENEENTITY_MODEL, pModel);
    //piSceneEditorBase->m_listSelectedEntity.push_back(entity);
	
	/*GETSELECT(piSceneEditorBase);
	pSelector->ClearSelectedEntity();*/
	
	//pSelector->AddSelectedEntity(SCENEENTITY_MODEL, pModel);
	IEKG3DSceneSelectionTool* pTool = NULL;
	HRESULT hr = pScene->GetSelectionToolInterface(&pTool);
	if (SUCCEEDED(hr))
	{
		pTool->ClearSelection();
		hr = pTool->SetSelected(SCENEENTITY_MODEL, pModel, TRUE);
	}

	UpdateData(FALSE);
}

void KSceneSFXEditorModel::OnFillSubsetComboBox()
{
    if (!m_pDoc)
        return;

    IEKG3DSceneSFXEditor *pEditor = NULL;
    m_pDoc->GetSceneSFXEditor(&pEditor);
    if (!pEditor)
        return;

    IEKG3DSFX *pSFX = NULL;
    pEditor->GetSFX(&pSFX);
    IEKG3DModel *pModel = (IEKG3DModel*)pSFX->GetModel(m_dwCurIndex);
    IEKG3DMaterial* pMaterial = NULL;
    pModel->GetIEKG3DMaterial(&pMaterial);

    m_comboSubset.ResetContent();

	pSFX->DeleteSubsetColorDiffuseLine(m_dwCurIndex);
    /*for (int i = 0; i < (int)pEditor->m_lpSFX->m_vecSubsetColor[m_dwCurIndex]->size(); ++i)
    {
        SAFE_DELETE(*(pEditor->m_lpSFX->m_vecSubsetColor[m_dwCurIndex]->begin() + i));
    }

    pEditor->m_lpSFX->m_vecSubsetColor[m_dwCurIndex]->clear();*/
    DWORD dwNumMaterial = 0;
    pMaterial->GetMaterialCount(&dwNumMaterial);
    for (DWORD i = 0; i < dwNumMaterial; i++)
    {
        CString str;
        str.Format("Subset %d", i);
        m_comboSubset.InsertString(0, str);

        D3DCOLORVALUE color;
        pModel->GetModelColor(i, &color);
		pSFX->AddSubsetColorDiffuseLine(m_dwCurIndex, color);
       /* KG3DSFX::COLOR_LINE_T *pDiffuseLine = new KG3DSFX::COLOR_LINE_T;
        D3DCOLORVALUE diffuse;
        diffuse.a = diffuse.r = diffuse.g = diffuse.b = 1.0f;
        pDiffuseLine->InsertKeyFrame(0, diffuse);
        pEditor->m_lpSFX->m_vecSubsetColor[m_dwCurIndex]->push_back(pDiffuseLine);*/
    }

    /*m_comboModels.ResetContent();
    for (size_t s = 0; s < pEditor->m_lpSFX->GetNumModels(); s++)
    {
        TCHAR szModelIndex[128];
        sprintf(szModelIndex, "%s%u", TEXT("模型-"), s);
        m_comboModels.AddString(szModelIndex);
    }*/
}

void KSceneSFXEditorModel::UpdateModelSelected()
{
    HRESULT hRetCode = E_FAIL;
    SCENEENTITYTYPE EntityType = SCENEENTITY_NONE;
    VOID *pvEntityPointer = NULL;

    GET_SFX_EDITOR();

    IEKG3DSFX *pSFX = NULL;
    IEKG3DSceneEditorBase *piSceneEditorBase = NULL;
    pScene->GetSceneEditBase(&piSceneEditorBase);

    pScene->GetSFX(&pSFX);
	GETSELECT(piSceneEditorBase);

	IEKG3DSceneSelectionTool* pSelTool = NULL;
	pScene->GetSelectionToolInterface(&pSelTool);
	if(NULL == pSelTool)
		return;

	//if (pSelector->GetSelectedEntitySize() == 1)
	if(1 == pSelTool->GetSelectionCount())
    {
		IEKG3DSceneSelectionToolAccessProxy* pProxy = KSF::GetSelectionProxy(pScene);
		if(NULL == pProxy)
			return;

		//for (INT i = 0; i < pSelector->GetSelectedEntitySize(); i ++)
		for(IEKG3DSceneEntity* piEntity = pProxy->Reset(); NULL != piEntity; piEntity = pProxy->Next())
        {
			/*IEKG3DSceneEntity* piEntity = NULL;
			pSelector->GetSelectedEntity(i, &pEntity);*/

			IEKG3DModel *pModel= NULL;
            DWORD dwIndex = pSFX->GetNumModels();
            for (DWORD i = 0; i < dwIndex; ++i)
            {
                pModel = (IEKG3DModel*)pSFX->GetModel(i);
                ASSERT(pModel);

                hRetCode = piEntity->GetPointer(&pvEntityPointer);
                KGLOG_COM_PROCESS_ERROR(hRetCode);
                if ((VOID *)pModel == pvEntityPointer)
                {
                    hRetCode = piEntity->GetType(&EntityType);
                    KGLOG_COM_PROCESS_ERROR(hRetCode);

                    KG_ASSERT_EXIT(EntityType == SCENEENTITY_MODEL);
                    m_dwCurIndex = i;
                    UpdateUI();
                    return;
                }
            }

            //++it;
        }
    }
    else
    {
        //m_comboLauncher.SetCurSel(-1);
        UpdateUI();
        return;
    }
Exit0:
    return;
}

void KSceneSFXEditorModel::OnBnClickedButtonBrowseMesh()
{
    IEKG3DMaterial *pMat = NULL;
    // TODO: Add your control notification handler code here
    if (!m_pDoc)
        return;
    IEKG3DSceneSFXEditor *pEditor = NULL;
    m_pDoc->GetSceneSFXEditor(&pEditor);
    if (!pEditor)
        return;

    // TODO: Add your control notification handler code here
    char szFilters[] = "Mesh Files|*.Mesh|Mdl Files|*.Mdl|";

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
        int x = 0;//rand() % 600 - 300;
        int y = 0;//rand() % 600 - 300;
        int z = 0;//rand() % 600 - 300;

        if (FAILED(pEditor->ChangeModel(
            fileDlg.GetPathName().GetBuffer(0),
            m_dwCurIndex
            )))
            MessageBox("载入模型失败！");
        else
        {
         /*   IEKG3DModel *pModel = pEditor->m_lpSFX->GetModel(m_dwCurIndex);
            ASSERT(pModel);

            char szMeshName[MAX_PATH], szMtlName[MAX_PATH];
            pModel->GetMeshFileName(szMeshName);
            pModel->GetMaterialFileName(szMtlName);

            m_editMeshName.SetWindowText(szMeshName);
            m_editMtlName.SetWindowText(szMtlName);*/
            IEKG3DSFX *pSFX = NULL;
            pEditor->GetSFX(&pSFX);
            IEKG3DModel *pModel = (IEKG3DModel*)pSFX->GetModel(m_dwCurIndex);
            ASSERT(pModel);

            char szMeshName[MAX_PATH], szMtlName[MAX_PATH];
            pModel->GetMeshFileName(szMeshName);
            pModel->GetMaterialFileName(szMtlName);

            m_editMeshName.SetWindowText(szMeshName);
            m_editMtlName.SetWindowText(szMtlName);

            pModel->GetIEKG3DMaterial(&pMat);

            if (pMat)
            {
				pSFX->AddMutiSubsetColorDiffuseLine(m_dwCurIndex, pMat);

                /*KG3DSFX::MDL_COLOR_LINE_T* pMdlColorT = new KG3DSFX::MDL_COLOR_LINE_T;
                for (UINT i = 0; i < pModel->m_lpMaterial->m_dwNumMaterials; i++)
                {
                    KG3DSFX::COLOR_LINE_T* pColorLine = new KG3DSFX::COLOR_LINE_T();
                    pMdlColorT->push_back(pColorLine);
                }
                SAFE_DELETE(pEditor->m_lpSFX->m_vecSubsetColor[m_dwCurIndex]);
                pEditor->m_lpSFX->m_vecSubsetColor[m_dwCurIndex] = pMdlColorT;*/
            }
            UpdateUI();
            OnFillSubsetComboBox();
           // OnFillSubsetComboBox();
        }
    }
}

void KSceneSFXEditorModel::OnBnClickedButtonBrowseAni()
{
    // TODO: Add your control notification handler code here
    HRESULT hRetCode = E_FAIL;
    GET_SFX_EDITOR();

    if (!m_pDoc)
        return;
    IEKG3DSceneSFXEditor *pEditor = NULL;
    m_pDoc->GetSceneSFXEditor(&pEditor);
    if (!pEditor)
        return;

    // TODO: Add your control notification handler code here
    char szFilters[] = "Animation Files|*.Ani||";

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
        IEKG3DSFX *pSFX = NULL;
        pEditor->GetSFX(&pSFX);
        IEKG3DModel *pModel = NULL;
        //(IEKG3DModel*)pSFX->GetModel(m_dwCurIndex);
        hRetCode = pSFX->GetModel(m_dwCurIndex, &pModel);
        if (FAILED(hRetCode))
        {
            MessageBox(_T("载入动画失败！"));
            return;
        }
        ASSERT(pModel);
        
        //char szAnimationName[MAX_PATH];
        CString strPathName = fileDlg.GetPathName();
        HRESULT hr = pModel->LoadAnimationFromFile(strPathName.GetBuffer(), 0);
        strPathName.ReleaseBuffer();
        if (FAILED(hr))
        {
            MessageBox(_T("载入动画失败！"));
        }
        else
        {
            m_editAniName.SetWindowText(strPathName.GetBuffer());
            strPathName.ReleaseBuffer();

			if (pModel)
			{
                IKG3DAnimationController *piAniController = NULL;
              //  pModel->TryPlayAnimation(ENU_ANIMATIONPLAY_CIRCLE, strPathName.GetBuffer(), 0, ANICTL_PRIMARY);
              //  pModel->FrameMove();
				pModel->PlayAnimation(0, strPathName.GetBuffer(), 0, 0, NULL, NULL, ANICTL_PRIMARY);
                strPathName.ReleaseBuffer();
				pModel->IEGetAnimationController(&piAniController);
                piAniController->PauseAnimation(TRUE);
			}
			IEKG3DAnimationController *pMdlAniController = NULL;
			//IEKG3DAnimation *pIEMdlAnimation     = NULL;
			IEKG3DClip *pIEMdlClip = NULL;

			hRetCode = pModel->GetIEKG3DAnimationController(&pMdlAniController);
            KGLOG_COM_PROCESS_ERROR(hRetCode);
			KGLOG_PROCESS_ERROR(pMdlAniController);

			hRetCode = pMdlAniController->GetCurAnimation(&pIEMdlClip);
            KGLOG_COM_PROCESS_ERROR(hRetCode);

            if (pMdlAniController && pIEMdlClip)
            {
                DWORD dwSFXFrameNum = pSFX->GetTotalFrameNum();
                float fSFXFrameTime = (float)pSFX->GetFrameInterval();
                //AnimationItem clipInfo;
                //pIEMdlAnimation->GetClipData(0, clipInfo);
				

                hRetCode = pIEMdlClip->GetNumFrames(&m_dwNumFrame);
                KGLOG_COM_PROCESS_ERROR(hRetCode);
                hRetCode = pIEMdlClip->GetFrameLength(&m_fFrameTime);
                KGLOG_COM_PROCESS_ERROR(hRetCode);

                FLOAT fTotalLeng = m_fFrameTime * (m_dwNumFrame - 1);


				DWORD dwToSFXFrame = (DWORD)(fTotalLeng / fSFXFrameTime); 

                float fSpeed = 1.f;//fTotalLeng / (dwSFXFrameNum * fSFXFrameTime);
                pSFX->SetModelAnimationSpeed(fSpeed, m_dwCurIndex);
				pSFX->SetModelAnimationFrameTime(m_fFrameTime, m_dwCurIndex);

                CString strAniSpeed;
				CString strFrameNum;
				CString strStarPert;
                strAniSpeed.Format(TEXT("%f"), fSpeed);
                m_editAnimationSpeed.SetWindowText(strAniSpeed);
				strFrameNum.Format(TEXT("%u"), m_dwNumFrame);
				GetDlgItem(IDC_FRAME_COUNT)->SetWindowText(strFrameNum);

				pSFX->SetModelAnimationFrameCount(m_dwNumFrame, m_dwCurIndex);

				float fStart = 0.0f;
				pSFX->GetModelAnimationStart(&fStart, m_dwCurIndex);
				strStarPert.Format("%f", fStart);
				m_editAnimationStart.SetWindowText(strStarPert);

                //for (size_t s = 0; s < pEditor->m_lpSFX->m_timelineModelAniBound[m_dwCurIndex]->GetSize())

				TKG3DTimeLine<int>* pLine = static_cast<TKG3DTimeLine<int>*>(pSFX->GetModelAniBoundTimeLine(m_dwCurIndex));

			    pLine->Clear();
				pLine->InsertKeyFrame(0, 0);
				pLine->InsertKeyFrame(
					dwToSFXFrame,
					dwToSFXFrame
				);
				//pFrame->m_dlgKeyFrame.UpdateUI();
                pFrame->GetKeysFrame()->UpdateUI();
            }
        }
    }
Exit0:
    return;
}

LRESULT KSceneSFXEditorModel::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    // TODO: Add your specialized code here and/or call the base class
    switch (message)
    {
    case WM_COLORDIALOG_CLOSE :
    case WM_COLORPOINTER_CHANGE :
        {
            EnableWindow();
            GET_SFX_EDITOR_RET(-1);

            int nSelect = m_comboSubset.GetCurSel();
            if (nSelect == CB_ERR)
                return -1;

            int nPos = pFrame->GetKeysFrame()->m_Slider.GetCurPos();

            IEKG3DSFX *pSFX = NULL;
            pScene->GetSFX(&pSFX);

			TKG3DTimeLine<D3DXCOLOR>* pColorLine =
				static_cast<TKG3DTimeLine<D3DXCOLOR>*>(pSFX->GetSubsetColorLine(m_dwCurIndex,nSelect));

            pColorLine->InsertKeyFrame(nPos, (D3DCOLORVALUE)m_Color);

            //pFrame->m_dlgKeyFrame.UpdateSliderCtrl();
            pFrame->GetKeysFrame()->UpdateSliderCtrl();
        }
        break;

    case WM_EDIT_RECEIVE_ENTER:
        {
            switch (wParam)
            {
            case IDC_EDIT_ANIM_SPEED:
                OnSetAnimationSpeed();
                break;
			case IDC_EDIT_ANIM_START :
				OnSetAnimationStart();
				break;
            }
        }
        break;
    };

    return CDialog::WindowProc(message, wParam, lParam);
}

void KSceneSFXEditorModel::OnSetAnimationStart()
{
	if (!m_pDoc)
		return;

    IEKG3DSceneSFXEditor *pEditor = NULL;
    m_pDoc->GetSceneSFXEditor(&pEditor);
    if (!pEditor)
        return;

    IEKG3DSFX *pSFX = NULL;
    pEditor->GetSFX(&pSFX);

    CString str;
	m_editAnimationStart.GetWindowText(str);
	float fStart = (float)::atof(str);
	pSFX->SetModelAnimationStart(fStart, m_dwCurIndex);
}

void KSceneSFXEditorModel::OnSetAnimationSpeed()
{
	int nResult  = true;
	int nRetCode = true;

	GET_SFX_EDITOR();

    IEKG3DSceneSFXEditor *pEditor = NULL;
    CString strSpeed;
	float   fSpeed = 0.0f;
	float	fFrameLength = 0.0f;
	int		nTatalFrame = 0;
	int		nFrame = 0;
	int		nDate  = 0;

	KG_PROCESS_ERROR(m_pDoc);

    m_pDoc->GetSceneSFXEditor(&pEditor);
	KG_PROCESS_ERROR(pEditor);

    IEKG3DSFX *pSFX = NULL;
    pEditor->GetSFX(&pSFX);

    KG_PROCESS_ERROR(m_dwCurIndex != -1);

    m_editAnimationSpeed.GetWindowText(strSpeed);
    fSpeed = (float)::atof(strSpeed);
    pSFX->SetModelAnimationSpeed(fSpeed, m_dwCurIndex);

	fFrameLength = (m_dwNumFrame - 1) * m_fFrameTime / fSpeed;
	nTatalFrame  = static_cast<int>(fFrameLength / pSFX->GetFrameInterval());

	if (nTatalFrame <= 0) 
		nTatalFrame  = 1;

	TKG3DTimeLine<int>* pLine = static_cast<TKG3DTimeLine<int>*>(pSFX->GetModelAniBoundTimeLine(m_dwCurIndex));

	pLine->GetKeyFrameByIndex(&nFrame, &nDate, 1);
	pLine->RemoveKeyFrame(nFrame);
    pLine->GetKeyFrameByIndex(&nFrame, &nDate, 0);
	nTatalFrame = nFrame + nTatalFrame;
	//nTatalFrame = nTatalFrame - 1;
	pLine->InsertKeyFrame(nTatalFrame, nTatalFrame);

	//pFrame->m_dlgKeyFrame.UpdateUI();
    pFrame->GetKeysFrame()->UpdateUI();
	nResult = true;
Exit0:
	return;
}

void KSceneSFXEditorModel::OnCbnDropdownComboSubset()
{
    int nSelect = m_comboSubset.GetCurSel();
    if (nSelect == CB_ERR)
        m_picSubsetColor.EnableWindow(FALSE);
    else
        m_picSubsetColor.EnableWindow();
    // TODO: Add your control notification handler code here
    //UpdateUI();
}

void KSceneSFXEditorModel::OnStnClickedPicSubsetColor()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    int nSelect = m_comboSubset.GetCurSel();
    if (nSelect == CB_ERR)
        return;

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);

	TKG3DTimeLine<D3DXCOLOR>* pColorLine =
		static_cast<TKG3DTimeLine<D3DXCOLOR>*>(pSFX->GetSubsetColorLine(m_dwCurIndex,nSelect));

    ASSERT(pColorLine);

    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    pColorLine->GetData(&m_Color, (float)nFrame);
    GetColorPickerDialog()->Init((D3DCOLORVALUE*)&m_Color, "模型颜色", this, TRUE);
}

void KSceneSFXEditorModel::OnCbnSelchangeComboSubset()
{
    // TODO: Add your control notification handler code here
    //UpdateUI();

    int nSelect = m_comboSubset.GetCurSel();
    if (nSelect == CB_ERR)
    {
        m_picSubsetColor.EnableWindow(FALSE);
        return;
    }
    else
        m_picSubsetColor.EnableWindow();

    GET_SFX_EDITOR();

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);

	TKG3DTimeLine<D3DXCOLOR>* pColorLine =
		static_cast<TKG3DTimeLine<D3DXCOLOR>*>(pSFX->GetSubsetColorLine(m_dwCurIndex,nSelect));

    pFrame->GetKeysFrame()->SetTimeLine(
        "子模型偏色",
        (TKG3DTimeLinebase*)pColorLine, SFX_TLDT_D3DXCOLOR
    );
}

void KSceneSFXEditorModel::OnBnClickedButtonBrowseMtl()
{
    HRESULT hr = E_FAIL;
    // TODO: Add your control notification handler code here
    if (!m_pDoc)
        return;
    IEKG3DSceneSFXEditor *pEditor = NULL;
    m_pDoc->GetSceneSFXEditor(&pEditor);
    if (!pEditor)
        return;

    // TODO: Add your control notification handler code here
    char szFilters[] = "Material Files|*.Mtl|All Files (*.*)|*.*||";

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
        IEKG3DSFX *pSFX = NULL;
        pEditor->GetSFX(&pSFX);
        IEKG3DModel *pModel = NULL;
        hr = pSFX->GetModel(m_dwCurIndex, &pModel);
        if (FAILED(hr))
        {
            MessageBox("载入材质失败！");
            return;
        }
        ASSERT(pModel);

        char szMtlName[MAX_PATH];
        hr = pModel->LoadMaterialFromFile(fileDlg.GetPathName().GetBuffer(0), 0);
        if (FAILED(hr))
        {
            MessageBox("载入材质失败！");
        }
        else
        {
            pModel->GetMaterialFileName(szMtlName);
            m_editMtlName.SetWindowText(szMtlName);
        }
    }
}

BOOL KSceneSFXEditorModel::OnSetActive()
{
    // TODO: Add your specialized code here and/or call the base class
    UpdateUI();

    return TRUE;//CDialog::OnSetActive();
}

BOOL KSceneSFXEditorModel::OnInitDialog()
{
    return CDialog::OnInitDialog();

}

void KSceneSFXEditorModel::OnBnClickedLoop()
{
	GET_SFX_EDITOR();
    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
	if (((CButton*)(GetDlgItem(IDC_LOOP)))->GetCheck())
		pSFX->EnagleMdlAniConnect(TRUE, m_dwCurIndex);
	else
		pSFX->EnagleMdlAniConnect(FALSE, m_dwCurIndex);
}

void KSceneSFXEditorModel::OnBnClickedButtonAdd()
{
    IEKG3DMaterial *pMat = NULL;
    if (!m_pDoc)  
        return;
    IEKG3DSceneSFXEditor *pEditor = NULL;
    m_pDoc->GetSceneSFXEditor(&pEditor);
    if (!pEditor)
        return;

    // TODO: Add your control notification handler code here
    char szFilters[] = "Mesh Files|*.Mesh||";

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
        int x = 0;//rand() % 600 - 300;
        int y = 0;//rand() % 600 - 300;
        int z = 0;//rand() % 600 - 300;

        if (FAILED(pEditor->AddModel(
            fileDlg.GetPathName().GetBuffer(0),
            (float)x, (float)y, (float)z
            )))
            MessageBox("载入模型失败！");
        else
        {
            IEKG3DSFX *pSFX = NULL;
            pEditor->GetSFX(&pSFX);
            m_dwCurIndex = pSFX->GetNumModels() - 1;
            IEKG3DModel *pModel = (IEKG3DModel*)pSFX->GetModel(m_dwCurIndex);
            ASSERT(pModel);

            char szMeshName[MAX_PATH], szMtlName[MAX_PATH];
            pModel->GetMeshFileName(szMeshName);
            pModel->GetMaterialFileName(szMtlName);

            m_editMeshName.SetWindowText(szMeshName);
            m_editMtlName.SetWindowText(szMtlName);

            pModel->GetIEKG3DMaterial(&pMat);

            if (pMat)
            {
				pSFX->AddMutiSubsetColorDiffuseLine(m_dwCurIndex, pMat);
                /*KG3DSFX::MDL_COLOR_LINE_T* pMdlColorT = new KG3DSFX::MDL_COLOR_LINE_T;
                for (UINT i = 0; i < pModel->m_lpMaterial->m_dwNumMaterials; i++)
                {
                    KG3DSFX::COLOR_LINE_T* pColorLine = new KG3DSFX::COLOR_LINE_T();
                    pMdlColorT->push_back(pColorLine);
                }
                pSFX->m_vecSubsetColor.push_back(pMdlColorT);*/
            }
            /*KG3DSFX::MDL_ANI_BOUND_T*  pMdlAniT   = new KG3DSFX::MDL_ANI_BOUND_T;
            pSFX->m_timelineModelAniBound.push_back(pMdlAniT);*/
			pSFX->AddTimeLineModelAniBound();
            OnFillSubsetComboBox();
            UpdateUI();
        }
    }
//Exit0:
    return;
}

void KSceneSFXEditorModel::OnBnClickedButtonDel()
{
    GET_SFX_EDITOR();
    int index = m_comboModels.GetCurSel();
    if (index == CB_ERR)
        return;
    pScene->RemoveModel(index);
    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    m_dwCurIndex = pSFX->GetNumModels() - 1;
    UpdateUI();

}

void KSceneSFXEditorModel::OnBnClickedBillbord()
{
    GET_SFX_EDITOR();
    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    if (((CButton*)(GetDlgItem(IDC_BILLBORD)))->GetCheck())
        pSFX->EnagleMdlBillBoard(TRUE, m_dwCurIndex);
    else
        pSFX->EnagleMdlBillBoard(FALSE, m_dwCurIndex);
}
