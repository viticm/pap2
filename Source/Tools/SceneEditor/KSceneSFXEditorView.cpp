////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSFXEditorView.cpp
//  Version     : 1.0
//  Creator     : Zeb
//  Create Date : 2005-10-8 16:08:48
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KSceneSFXEditorView.h"
////////////////////////////////////////////////////////////////////////////////
#include "KSceneSFXEditorDoc.h"
//#include "KG3DEngineManager.h"//by dengzhihui Refactor 2006年11月6日
//#include "SceneEditorCommon.h"//by dengzhihui Refactor 2006年11月6日
#include "IEKG3DScene.h"
#include "KSceneSettingDialog.h"
#include "Resource.h"

#include "SceneEditor.h"
#include "KSceneSFXEditorDoc.h"
#include "IEKG3DSceneSFXEditor.h"
//#include "KG3DSceneEditorBase.h"
//#include "MainFrm.h" //(by dengzhihui 2006年7月6日 14:50:31
#include "KSceneSFXEditorTimeView.h"
#include "KSceneSFXEditorFrame.h"
#include "IEKG3DSFX.h"
#include "KSceneEditBaseSelectDialog.h"
#include "KSceneSFXEditorPanel.h"
#include "KGSFXModelPane.h"
#include "KSceneSFXEditorKeyFrameDlg.h"
#include "IEKG3DMeshBone.h"
#include "IEEditor.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------------------
// SFX Main Window
//-----------------------------------------------------------------------------------

IMPLEMENT_DYNCREATE(KGSFXMainPaneWindow, CDialog)

KGSFXMainPaneWindow::KGSFXMainPaneWindow(CWnd* pParent)
{
    m_dwWndState = SFX_WND_STATE_FREE;
    m_pDependWnd = NULL;
    m_dwPrevTime = 0;
}

KGSFXMainPaneWindow::~KGSFXMainPaneWindow()
{
}

int KGSFXMainPaneWindow::SetDependWnd(CView* pDependWnd)
{
    ASSERT(pDependWnd);
    m_pDependWnd = pDependWnd;
    return TRUE;
}

BEGIN_MESSAGE_MAP(KGSFXMainPaneWindow, CDialog)
    ON_WM_SETFOCUS()
    ON_WM_KILLFOCUS()
	ON_WM_SIZE()
    ON_WM_SETCURSOR()
	//ON_COMMAND(ID_SCENE_SETTING, OnSceneSetting)
END_MESSAGE_MAP()


BOOL KGSFXMainPaneWindow::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    GET_SFX_EDITOR_RET(CWnd::OnSetCursor(pWnd, nHitTest, message));

    IEKG3DSceneEditorBase* pEditorBase = NULL;
    pScene->GetSceneEditBase(&pEditorBase);

    if (pEditorBase && pEditorBase->GetRtsActive())
    {
        switch (pScene->GetEditState())
        {
        case SCENESTATE_MOVE :
            SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_MOVE));
            return TRUE;
        case SCENESTATE_ROTATE :
            SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_ROT));
            return TRUE;
        case SCENESTATE_SCALE :
            SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_SCL));
            return TRUE;
        default :
            break;
        }
    }
    return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

BOOL KGSFXMainPaneWindow::PreTranslateMessage(MSG* pMsg)
{
	if(m_hAccel)
	{
		if (::TranslateAccelerator(GetSafeHwnd(), m_hAccel, pMsg))
			return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}


void KGSFXMainPaneWindow::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    //
    CRect rectClient;
    GetClientRect(&rectClient);
    if (rectClient.Width() <= 10 || rectClient.Height() <= 10)
        return;
    //

    if (m_pDependWnd)
    {
        KSceneSFXEditorDoc *pDoc = (KSceneSFXEditorDoc*)m_pDependWnd->GetDocument();
        if (!pDoc)
            return;
        IEKG3DSceneSFXEditor *pEditor = NULL;
        pDoc->GetSceneSFXEditor(&pEditor);
        if (!pEditor)
            return;

        IEKG3DScene* piScene = NULL;
        pEditor->GetScene(&piScene);
        if (!piScene)
            return;
        piScene->OnResizeWindow(NULL);
    }
}

void KGSFXMainPaneWindow::OnSetFocus(CWnd* pOldWnd)
{
    CDialog::OnSetFocus(pOldWnd);
    if (m_pDependWnd)
    {
        KSceneSFXEditorDoc *pDoc = (KSceneSFXEditorDoc*)m_pDependWnd->GetDocument();
        if (!pDoc)
            return;
        IEKG3DSceneSFXEditor *pEditor = NULL;
        pDoc->GetSceneSFXEditor(&pEditor);
        if (!pEditor)
            return;

        pEditor->SetFocus(TRUE);

        IEKG3DScene* piScene = NULL;
        pEditor->GetScene(&piScene);
        if (!piScene)
            return;
        g_pEngineManager->SetFocusScene(piScene);//by dengzhihui Refactor 2006年11月6日

    }
    Init();
}

void KGSFXMainPaneWindow::OnKillFocus(CWnd* pNewWnd)
{
    CDialog::OnKillFocus(pNewWnd);

    if (m_pDependWnd)
    {
        KSceneSFXEditorDoc *pDoc = (KSceneSFXEditorDoc*)m_pDependWnd->GetDocument();
        if (!pDoc)
            return;
        IEKG3DSceneSFXEditor *pEditor = NULL;
        pDoc->GetSceneSFXEditor(&pEditor);
        if (!pEditor)
            return;

        pEditor->SetFocus(FALSE);
        IEKG3DSceneEditorBase *piSceneEditorBase = NULL;
        pEditor->GetSceneEditBase(&piSceneEditorBase);
        if (GetEditState(piSceneEditorBase) == SCENESTATE_SFX_ADD_BILLBOARD ||
            GetEditState(piSceneEditorBase) == SCENESTATE_SFX_ADD_LAUNCHER)
            piSceneEditorBase->SetEditState(SCENESTATE_SELECT);

    }
}

void KGSFXMainPaneWindow::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

int   KGSFXMainPaneWindow::Init()
{
    m_hCursor = ::LoadCursor(NULL, IDC_HAND);
    SetClassLongPtr(m_hWnd, GCL_HCURSOR, (LONG)(LONG_PTR)m_hCursor);


	m_hAccel = LoadAccelerators(
		AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_SFXEDITOR)
		);
    return TRUE;
	/*KSceneSFXEditorDoc* pDoc = (KSceneSFXEditorDoc*) GetDocument();
	if (pDoc)
	{
		KG3DScene* pScene = pDoc->GetScene();
		if (pScene)
		{
			pScene->AddOutputWindow("SFX Main", m_hWnd,
				OUTPUTWND_SFXEDITOR_MAIN,0,&m_nOutputWindowID);
		}
	}*/

	//SetTimer(0,1000,NULL);
	// TODO: Add your specialized code here and/or call the base class
}

DWORD KGSFXMainPaneWindow::UpdateWndState()
{
    KSceneSFXEditorView* pEditView = dynamic_cast<KSceneSFXEditorView*>(m_pDependWnd);
    KG_PROCESS_ERROR(pEditView);

    if (m_dwWndState == SFX_WND_STATE_FREE)
    {
        ModifyStyle(NULL, WS_CAPTION | WS_BORDER | WS_SIZEBOX, SWP_DRAWFRAME);
        RECT rectWnd;
        GetWindowRect(&rectWnd);
        SetParent(NULL);
        MoveWindow(&rectWnd);
    }
    else
    {
        ModifyStyle(WS_CAPTION | WS_BORDER | WS_SIZEBOX, NULL, SWP_DRAWFRAME);
        SetParent(m_pDependWnd);
        MoveWindow(&(pEditView->GetPaneRect(WND_MAIN_PANE)));
    }
Exit0:
    return m_dwWndState;
}

LRESULT KGSFXMainPaneWindow::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT hr = CDialog::WindowProc(message, wParam, lParam);

    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    msg.hwnd    = m_hWnd;
    msg.wParam  = wParam;
    msg.lParam  = lParam;
    msg.message = message;
    m_InputProxy.CheckInputEvent(&msg);
	//ATLTRACE("message %d, wParam %d, lParam %d\n", msg.message, msg.wParam, msg.lParam);
    switch (message)
    {
	case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MOUSEWHEEL:
    case WM_MOUSEMOVE:
    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_CHAR:
        {
            g_pEngineManager->ProcessMessage(message,wParam,lParam);//<PROCESS_MESSAGE>

            switch (message)
            {
            case WM_LBUTTONDOWN:
                OnLButtonDownMessage();
                break;
            case WM_LBUTTONUP:
                OnLButtonUpMessage();
                break;
            case WM_RBUTTONUP :
                OnRButtonUpMessage();
                break;
            case WM_KEYDOWN:
                OnKeyDownMessage();
                break;
            case WM_MOUSEMOVE:
                OnMouseMoveMessage();
                break;
            default:
                break;
            }
        }
        break;
    case WM_LBUTTONDBLCLK :
        if (m_dwWndState == SFX_WND_STATE_FREE)
            m_dwWndState =  SFX_WND_STATE_BIND;
        else
            m_dwWndState = SFX_WND_STATE_FREE;
        UpdateWndState();
        break;
    }

    if (message == WM_KEYUP && wParam == VK_F9)
    {
        GET_SFX_EDITOR_RET(-1);
        KSceneSFXEditorView::GetActiveSFXView()->m_paneSFXModelView.m_sheet.SetActivePage(3);


    }

    if (message == WM_KEYUP && wParam == VK_F8)
    {
        //ShowWindow(SW_SHOW);
        GET_SFX_EDITOR_RET(E_FAIL);
        pScene->ShowVideoInfo(!pScene->IsVideoInfoShow());
    }

    if (message == WM_KEYUP && wParam == VK_F5)
    {
        GET_SFX_EDITOR_RET(E_FAIL);
        pScene->ShowBindBone();
    }

    if (message == WM_KEYUP && wParam == 'F')
    {
        GET_SFX_EDITOR_RET(E_FAIL);
        DWORD dwD3DFillModel = 0;
        pScene->GetD3DFillModel(&dwD3DFillModel);

        if (dwD3DFillModel == D3DFILL_WIREFRAME)
            pScene->SetD3DFillModel(D3DFILL_SOLID);
        else
            pScene->SetD3DFillModel(D3DFILL_WIREFRAME);
    }

    if (message == WM_KEYUP && wParam == 'U')
    {
        KSceneSFXEditorView* pSFXView = KSceneSFXEditorView::GetActiveSFXView();
        GET_SFX_EDITOR_RET(E_FAIL);
        pScene->UnBindSelModel();
        //pScene->SetCurBindModel();
        //pSFXView->m_paneSFXModelView.m_sheet.m_pageSockBone.OnSetActive();
        ////pSFXView->m_paneSFXKeyFrame.FillComb();
    }

    if (message == WM_KEYUP && wParam == 'N')
    {
        KSceneSFXEditorView* pSFXView = KSceneSFXEditorView::GetActiveSFXView();
        GET_SFX_EDITOR_RET(E_FAIL);
        pScene->BindSelMdlToTargetMdl();
    }

    return hr;
}

void KGSFXMainPaneWindow::OnMouseMoveMessage()
{
    GET_SFX_EDITOR();
    
    if (pScene->GetEditState() == SCENESTATE_SFX_GEO)
    {
        DWORD dwType;
        DWORD dwSubType;

        pScene->GetGeoType(dwType, dwSubType);

        switch (dwType)
        {
        case SFX_GEO_TYPE_LAUNCHER :
            //switch (dwSubType)
            //{
            //case SFX_GEO__LAUNCHER_SUB_TYPE_RECT :
                switch (pScene->GetGeoStep())
                {
                case SFX_GEO_STEP_1 :
                    pScene->ModifyGeoElement(m_vPervRayPoint, pScene->GetGeoRayPointHor(), m_vPervRayPoint);
                    break;
                case SFX_GEO_STEP_2 :
                    if (dwSubType == SFX_GEO__LAUNCHER_SUB_TYPE_CYLR)
                        pScene->ModifyGeoElement(m_vPervRayPoint, m_vPervRayPointEx, pScene->GetGeoRayPointVel(m_vPervRayPointEx));
                    else
                       pScene->ModifyGeoElement(m_vPervRayPoint, m_vPervRayPointEx, pScene->GetGeoRayPointHor());
                    break;
                    
                }
               // break;
           // }
            break;
        }
    }

    DWORD dwCurrTime = timeGetTime();
    if (dwCurrTime - m_dwPrevTime < 100)
        return;
    m_dwPrevTime = dwCurrTime;

    IEKG3DSceneEditorBase *piSceneEditorBase = NULL;
    pScene->GetSceneEditBase(&piSceneEditorBase);

    if (
        ((GetEditState(piSceneEditorBase) == SCENESTATE_MOVE) ||
         (GetEditState(piSceneEditorBase) == SCENESTATE_ROTATE) ||
         (GetEditState(piSceneEditorBase) == SCENESTATE_SCALE)) &&
         pFrame
    )
        pFrame->GetKeysFrame()->UpdateTransformUI();

    if (GetEditState(piSceneEditorBase) == SCENESTATE_SCALE)
    {
        //pFrame->GetPanelView()->GetBillboardPage()->UpdateBillboardHeight()
    }

    //POINT pMouse;
    //GetCursorPos(&pMouse);
    //ScreenToClient(&pMouse);
    //RECT rectClient;
    //GetClientRect(&rectClient);
    //if (::PtInRect(&rectClient, &pMouse))
    //    SetFocus();
    //else
    //    KillFou
   
}

void KGSFXMainPaneWindow::OnKeyDownMessage()
{
    CFrameWnd *pMainFrame = (CFrameWnd*)AfxGetMainWnd();
    KSceneSFXEditorFrame* pFrame = (KSceneSFXEditorFrame*)pMainFrame->GetActiveFrame();

    //pFrame->m_dlgSelect.UpdateUI();
    pFrame->GetKeysFrame()->UpdateTransformUI();

    KSceneSFXEditorParticlePage *pParticlePage = (KSceneSFXEditorParticlePage*)pFrame->GetPanelView()->GetParticlePage();
    KSceneSFXEditorBillboardPage *pBillboardPage = pFrame->GetPanelView()->GetBillboardPage();
    KSceneSFXEditorTrackPage *pTrackPage = pFrame->GetPanelView()->GetTrackPage();
    KSceneSFXEditChannel* pChannelPage = pFrame->GetPanelView()->GetChannelPage();
    KSceneSFXEditorModel* pModelPage = pFrame->GetPanelView()->GetModelPage();
    KSceneSFXEditorMeteor* pMeteor = pFrame->GetPanelView()->GetMeteorPage();
    //SFXUINeedUpdate();
   
    if (GetAsyncKeyState(VK_DELETE) & 0x8000)
    {
        if (pParticlePage->m_hWnd)
        {
            pParticlePage->UpdateLauncherListUI();
            pParticlePage->UpdateLauncherSelected();
            pParticlePage->UpdateLauncherUI();
        }
        
        if (pBillboardPage->m_hWnd)
        {
            pBillboardPage->UpdateBillboardList();
            pBillboardPage->UpdateBillboardSelected();
            pBillboardPage->UpdateBillboardUI();
        }

        if (pTrackPage->m_hWnd)
        {
            pTrackPage->UpdateUI();
        }

        if (pChannelPage)
        {
            pChannelPage->Check();
        }
    
        if (pMeteor)
        {
            pMeteor->UpdateMeteorList();
            pMeteor->UpdateProperties();
        }

        if (KSceneSFXEditorView::GetActiveSFXView())
            KSceneSFXEditorView::GetActiveSFXView()->m_paneSFXModelView.m_sheet.m_pageGlobPage.Init();
        
        if (pModelPage)
        {
            pModelPage->Check();
            pModelPage->UpdateUI();
        }
    }
}

void KGSFXMainPaneWindow::OnLButtonDownMessage()
{
    GET_SFX_EDITOR();
    
    if (pScene->GetEditState() == SCENESTATE_SFX_GEO)
    {
        if (pScene->GetGeoStep() == SFX_GEO_STEP_P)
        {
            m_vPervRayPoint = pScene->GetGeoRayPointHor();
            pScene->CreateGeoElement();
            pScene->ModifyGeoElement(m_vPervRayPoint, pScene->GetGeoRayPointHor(), m_vPervRayPoint);
            pScene->SetGeoStep(SFX_GEO_STEP_1);
        }

        if (pScene->GetGeoStep() == SFX_GEO_STEP_2)
        {
            pScene->SetGeoStep(SFX_GEO_STEP_F);
            pScene->AppendGeoElementToSfx();
        }
    }
}

void KGSFXMainPaneWindow::OnLButtonUpMessage()
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    KSceneSFXEditorParticlePage*  pParticlesPage = NULL;
    KSceneSFXEditorBillboardPage* pBillboardPage = NULL;
    KSceneSFXEditorModel*         pModelPage     = NULL;

    KSceneSFXEditorDoc* pCurDoc = NULL;
    IEKG3DSceneSFXEditor* pEditor = NULL;

    KSceneSFXEditorView*  pActiveView = NULL;
    CFrameWnd*            pMainFrame  = NULL;
    KSceneSFXEditorFrame* pChilFrame  = NULL;
    IEKG3DSceneEditorBase *piSceneEditorBase = NULL;

    pActiveView = dynamic_cast<KSceneSFXEditorView*>(m_pDependWnd);
    KG_PROCESS_ERROR(pActiveView);

    pCurDoc = dynamic_cast<KSceneSFXEditorDoc*>(m_pDependWnd->GetDocument());
    KG_PROCESS_ERROR(pCurDoc);

    hRetCode = pCurDoc->GetSceneSFXEditor(&pEditor);
    KGLOG_COM_PROCESS_ERROR(hRetCode);
    KG_PROCESS_ERROR(pEditor);

    pParticlesPage = pActiveView->m_paneSFXEditerWnd.GetParticlePage();
    KG_PROCESS_ERROR(pParticlesPage);

    pBillboardPage = pActiveView->m_paneSFXEditerWnd.GetBillboardPage();
    KG_PROCESS_ERROR(pBillboardPage);

    pModelPage     = pActiveView->m_paneSFXEditerWnd.GetModelPage();
    KG_PROCESS_ERROR(pModelPage);

    pMainFrame = dynamic_cast<CFrameWnd*>(AfxGetMainWnd());
    KG_PROCESS_ERROR(pMainFrame);

    pChilFrame = dynamic_cast<KSceneSFXEditorFrame*>(pMainFrame->GetActiveFrame());
    KG_PROCESS_ERROR(pChilFrame);

    //pChilFrame->m_dlgSelect.UpdateList();
    hRetCode = pEditor->GetSceneEditBase(&piSceneEditorBase);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    if (GetEditState(piSceneEditorBase) == SCENESTATE_SELECT)
    {
        pActiveView->m_paneSFXEditerWnd.UpdateUI();

        if (pParticlesPage->m_hWnd)
        {
            if (pActiveView->m_paneSFXEditerWnd.m_lpPropertySheet->GetActiveIndex() == 0)
            {
                pParticlesPage->UpdateParticleKindUI();
                pParticlesPage->UpdateLauncherListUI();
                pParticlesPage->UpdateLauncherSelected();
                pParticlesPage->UpdateLauncherUI();
                pParticlesPage->UpdateForceFieldUI();
            }

            //
            if (pActiveView->m_paneSFXEditerWnd.m_lpPropertySheet->GetActiveIndex() == 2)
                pModelPage->UpdateModelSelected();
        }
    }
    else if (GetEditState(piSceneEditorBase) == SCENESTATE_SFX_ADD_LAUNCHER)
    {
        if (pParticlesPage->m_hWnd)
            pParticlesPage->UpdateLauncherListUI();
    }
    else if (GetEditState(piSceneEditorBase) == SCENESTATE_SFX_ADD_BILLBOARD)
    {
        if (pBillboardPage->m_hWnd)
            pBillboardPage->UpdateBillboardList();
    }

    pActiveView->m_paneSFXKeyFrame.OnSetTransformation(FALSE);
    pChilFrame->GetKeysFrame()->UpdateTransformUI();
    SetFocus();


    // updat scene
    pEditor->AddHandMdlToRefMdl();
    pEditor->UpdateBoneSel();
    //pActiveView->m_paneSFXKeyFrame.FillComb();
    IEKG3DMeshBone* piBoneMesh =NULL;
    IEKG3DModel *pModel = NULL;
    pEditor->GetCurBindModel(&pModel);
    if (pModel)
    {
        hRetCode = pModel->GetIEKG3DMeshBone(&piBoneMesh);
        KGLOG_COM_PROCESS_ERROR(hRetCode);

        HTREEITEM hTreeItem =
            pActiveView->m_paneSFXModelView.m_sheet.m_pageSockBone.m_bone.GetRootItem();
        while (hTreeItem && piBoneMesh)
        {
            DWORD dwType;
            TCHAR szBoneName[128];
            CString strItemText =
                pActiveView->m_paneSFXModelView.m_sheet.m_pageSockBone.m_bone.GetItemText(hTreeItem);
            piBoneMesh->GetSelBoneName(szBoneName, sizeof(szBoneName), &dwType);
            if (strItemText == szBoneName)
                pActiveView->m_paneSFXModelView.m_sheet.m_pageSockBone.m_bone.SelectItem(hTreeItem);
            hTreeItem =
                pActiveView->m_paneSFXModelView.m_sheet.m_pageSockBone.m_bone.GetNextItem(hTreeItem, TVGN_NEXT);
        }
    }
    pActiveView->m_paneSFXEditerWnd.m_lpPropertySheet->m_pageBind.UpdateSFXSuedeList();


    if (pEditor->GetEditState() == SCENESTATE_SFX_GEO)
    {
        DWORD dwType;
        DWORD dwSubType;

        pEditor->GetGeoType(dwType, dwSubType);

        switch (dwType)
        {
        case SFX_GEO_TYPE_LAUNCHER :
            switch (dwSubType)
            {
            case SFX_GEO__LAUNCHER_SUB_TYPE_RECT :
            case SFX_GEO__LAUNCHER_SUB_TYPE_SHER :
                switch (pEditor->GetGeoStep())
                {
                case SFX_GEO_STEP_1 :
                    pEditor->SetGeoStep(SFX_GEO_STEP_F);
                    pEditor->AppendGeoElementToSfx();
                    break;
                }
                break;
            case SFX_GEO__LAUNCHER_SUB_TYPE_CIRC :
            case SFX_GEO__LAUNCHER_SUB_TYPE_CYLR :
                switch (pEditor->GetGeoStep())
                {
                case SFX_GEO_STEP_1 :
                    m_vPervRayPointEx = pEditor->GetGeoRayPointHor();
                    pEditor->ModifyGeoElement(m_vPervRayPoint, pEditor->GetGeoRayPointHor(), m_vPervRayPointEx);
                    pEditor->SetGeoStep(SFX_GEO_STEP_2);
                    break;
               
                } 
                break;
            }
            break;
        

        }
    }

    hResult = true;
Exit0:
    return;
}


void KGSFXMainPaneWindow::OnRButtonUpMessage()
{
    GET_SFX_EDITOR();
    pScene->SetEditState(SCENESTATE_SELECT);

    KSceneSFXEditorView*  pActiveView = pActiveView = dynamic_cast<KSceneSFXEditorView*>(m_pDependWnd);
    ASSERT(pActiveView);
    pActiveView->m_paneSFXEditerWnd.UpdateUI();
}
//-----------------------------------------------------------------------------------
// SFX Main View
//-----------------------------------------------------------------------------------

KSceneSFXEditorView* KSceneSFXEditorView::GetActiveSFXView()
{
    KSceneSFXEditorView* pSFXView = NULL;

    CFrameWnd* pMDIMainFrame = NULL;
	CFrameWnd* pActiveFrame  = NULL;

    pMDIMainFrame = dynamic_cast<CFrameWnd*>(AfxGetMainWnd());
    KG_PROCESS_ERROR(pMDIMainFrame);

    pActiveFrame = pMDIMainFrame->GetActiveFrame();
    KG_PROCESS_ERROR(pActiveFrame);

    pSFXView = dynamic_cast<KSceneSFXEditorView*>(pActiveFrame->GetActiveView());
Exit0:
    return pSFXView;
}

IEKG3DSceneSFXEditor* KSceneSFXEditorView::GetSfxScene()
{
    if (!m_hWnd)
        return NULL;

    KSceneSFXEditorFrame* pFrame = (KSceneSFXEditorFrame*)GetParent();

    if (!pFrame)
        return NULL;

    KSceneSFXEditorDoc* pDoc = (KSceneSFXEditorDoc*)pFrame->GetActiveDocument();

    if (!pDoc)
        return NULL;

    IEKG3DSceneSFXEditor* pScene = NULL;
    pDoc->GetSceneSFXEditor(&pScene);

    return pScene;
}

IMPLEMENT_DYNCREATE(KSceneSFXEditorView, CView)

void KSceneSFXEditorView::OnDraw(CDC* pDC)
{
}

KSceneSFXEditorView::KSceneSFXEditorView(void)
{
	m_nOutputWindowID = -1;
    m_pInvoker = NULL;
}

KSceneSFXEditorView::~KSceneSFXEditorView(void)
{
}

BEGIN_MESSAGE_MAP(KSceneSFXEditorView, CView)
    ON_COMMAND(ID_SCENE_SETTING, OnSceneSetting)
    ON_WM_SIZE()
    ON_WM_DESTROY()
	ON_COMMAND(ID_METEOR, &KSceneSFXEditorView::OnMeteor)
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()

void KSceneSFXEditorView::OnSceneSetting()
{
    KSceneSFXEditorDoc* pDoc = (KSceneSFXEditorDoc*) GetDocument();
    if (!pDoc)
        return;
    IEKG3DSceneSFXEditor *pEditor = NULL;
    pDoc->GetSceneSFXEditor(&pEditor);
    if (!pEditor)
        return;

    KSceneSettingDialog dlg;
    /*
    IEKG3DScene *piScene = NULL;
        pEditor->GetScene(&piScene);
    
        dlg.SetSceneEditor(piScene);*/
    
    dlg.DoModal();
}

void KSceneSFXEditorView::UpdateUI()
{
    KSceneSFXEditorDoc *pDoc = (KSceneSFXEditorDoc*)GetDocument();
    if (pDoc)
    {
        IEKG3DSceneSFXEditor *pEditor = NULL;
        pDoc->GetSceneSFXEditor(&pEditor);
        if (pEditor)
        {
            BOOL bIsFocus = FALSE;

            pEditor->GetFocus(&bIsFocus);
            if (bIsFocus)
                SetFocus();
            else
                AfxGetMainWnd()->SetFocus();
        }
    }
    if (::IsWindow(m_paneSFXEditerWnd.m_hWnd))
        m_paneSFXEditerWnd.UpdateUI();
    if (::IsWindow(m_paneSFXKeyFrame.m_hWnd))
        m_paneSFXKeyFrame.UpdateUI();
    if (::IsWindow(m_paneSFXModelView.m_hWnd))
        m_paneSFXModelView.UpdateUI();

    /*if (pEditor)
    {
        if (pEditor->GetSFXSceneMode() == SFX_SCENE_MODE_EDIT)
        {
            m_paneSFXKeyFrame.ShowWindow(SW_SHOW);
            m_paneSFXSuedeTool.ShowWindow(SW_HIDE);
        }
        else
        {
            m_paneSFXKeyFrame.ShowWindow(SW_HIDE);
            m_paneSFXSuedeTool.ShowWindow(SW_SHOW);
        }
    }*/
    //if (::IsWindow(m_paneSFXKeyFrame))
}

RECT KSceneSFXEditorView::GetPaneRect(DWORD dwID)
{
    RECT rectClientALL;
    RECT rectModelPane;
    RECT rectMainPane;
    RECT rectEditPane;
    RECT rectPlayPane;

    GetClientRect(&rectClientALL);
    m_paneSFXEditerWnd.GetWindowRect(&rectEditPane);
    m_paneSFXModelView.GetWindowRect(&rectModelPane);
    m_paneSFXMainWnd.GetWindowRect(&rectMainPane);
    m_paneSFXKeyFrame.GetWindowRect(&rectPlayPane);

    ScreenToClient(&rectModelPane);
    ScreenToClient(&rectMainPane);
    ScreenToClient(&rectEditPane);
    ScreenToClient(&rectPlayPane);

    RECT rectPane;
    SetRect(&rectPane, 0, 0, 0, 0);
    switch (dwID)
    {
    case WND_MAIN_PANE :
        if (
            (m_paneSFXModelView.m_dwWndState == SFX_WND_STATE_BIND) //&&
            //(m_paneSFXModelView.IsWindowVisible())
        )
            rectPane.left = rectModelPane.right;
        else
            rectPane.left = rectClientALL.left;
        rectPane.top = 0;
        if (
            (m_paneSFXEditerWnd.m_dwWndState == SFX_WND_STATE_BIND)// &&
            //(m_paneSFXEditerWnd.IsWindowVisible())
        )
            rectPane.right = rectEditPane.left;
        else
            rectPane.right = rectClientALL.right;
        if (
            (m_paneSFXKeyFrame.m_dwWndState == SFX_WND_STATE_BIND)// &&
            //(m_paneSFXKeyFrame.IsWindowVisible())
        )
            rectPane.bottom = rectPlayPane.top;
        else
            rectPane.bottom = rectClientALL.bottom;
        break;
    case WND_EDIT_PANE :
        rectPane.right  = rectClientALL.right;
        rectPane.left   = rectPane.right - (rectEditPane.right - rectEditPane.left);
        rectPane.top    = rectClientALL.top;
        //rectPane.bottom = rectClientALL.bottom;
        if (
            (m_paneSFXKeyFrame.m_dwWndState == SFX_WND_STATE_BIND)// &&
            //(m_paneSFXKeyFrame.IsWindowVisible())
            )
            rectPane.bottom = rectPlayPane.top;
        else
            rectPane.bottom = rectClientALL.bottom;
        break;
    case WND_LEFT_PANE :
        rectPane.left   = rectClientALL.left;
        rectPane.top    = rectClientALL.top;
        rectPane.right  = rectPane.left + (rectModelPane.right - rectModelPane.left);
        //rectPane.bottom = rectClientALL.bottom;
        if (
            (m_paneSFXKeyFrame.m_dwWndState == SFX_WND_STATE_BIND)// &&
            //(m_paneSFXKeyFrame.IsWindowVisible())
            )
            rectPane.bottom = rectPlayPane.top;
        else
            rectPane.bottom = rectClientALL.bottom;
        break;
    case WND_PLAY_PANE :
        //if (
        //    (m_paneSFXModelView.m_dwWndState == SFX_WND_STATE_BIND)// &&
        //    //(m_paneSFXModelView.IsWindowVisible())
        // )
        //    rectPane.left = rectModelPane.right;
        //else
        //    rectPane.left = rectClientALL.left;
        //if (
        //    (m_paneSFXEditerWnd.m_dwWndState == SFX_WND_STATE_BIND) //&&
        //   // (m_paneSFXEditerWnd.IsWindowVisible())
        //)
        //    rectPane.right = rectEditPane.left;
        //else
        //    rectPane.right = rectClientALL.right;
        rectPane.left   = rectClientALL.left;
        rectPane.right  = rectClientALL.right;
        rectPane.bottom = rectClientALL.bottom;
        rectPane.top    = rectPane.bottom - 78;//(rectPlayPane.bottom - rectPlayPane.top);
        break;
    default :
        break;
    }
    return rectPane;
}

void KSceneSFXEditorView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

    m_colorProject.colorBacks = RGB(197, 197, 197);
    m_colorProject.colorFocus = RGB(255, 100, 100);

	KSceneSFXEditorDoc* pDoc = (KSceneSFXEditorDoc*) GetDocument();
	if (pDoc)
	{
        // create Main View Pane
        static PROXYKEYMAP KEYMAP[] =
        {	//           C  S  A  L  M  R  ?    Trigger     ID old
            //{VK_MENU , 0, 0, 1, 0, 1, 0, 0, WM_MOUSEMOVE,   EXEACTION_ROTATE_CAMERA,  0, 0},	//alt+MBT, 旋转摄像机镜头
			{VK_RBUTTON, 0, 0, 0, 0, 0, 1, 0, WM_MOUSEMOVE,   EXEACTION_ROTATE_CAMERA,  0, 0},	//按住右键不放，旋转摄像机镜头
            {VK_CONTROL, 1, 0, 0, 0, 1, 0, 0, WM_MOUSEMOVE,   EXEACTION_ZOOM_VIEW,  0, 0},	//ctrl+MBT, 快速缩放视角
            {VK_MBUTTON, 0, 0, 0, 0, 1, 0, 0, WM_MOUSEMOVE,   EXEACTION_PAN_VIEW,  0, 0},	//MBT, PAN视图
            {VK_LBUTTON, 1, 0, 0, 1, 0, 0, 0, WM_LBUTTONDOWN, EXEACTION_INVERT_ACTION,  0, 0},	//CTRL＋LBT，反向当前操作

            {0		   , 0, 0, 0, 0, 0, 0, 0, 0             , 0,  0, 0},	//结束
        };
        IEKG3DSceneSFXEditor *piSFXEditor = NULL;
        pDoc->GetSceneSFXEditor(&piSFXEditor);
        if (!piSFXEditor)
            return;

        IEKG3DSceneEditorBase *pSceneEditorBase = NULL;
        piSFXEditor->GetSceneEditBase(&pSceneEditorBase);
        if (!pSceneEditorBase)
            return;

        m_paneSFXMainWnd.m_InputProxy.Init(pSceneEditorBase, &m_paneSFXMainWnd, KEYMAP);

        m_paneSFXMainWnd.Create(IDD_SFX_MAIN_VIEW_WND, this);
        m_paneSFXMainWnd.SetDependWnd(this);
        SetWindowLong(m_paneSFXMainWnd.m_hWnd, GWL_EXSTYLE, 0);
        m_paneSFXMainWnd.m_dwWndState = SFX_WND_STATE_BIND;
        m_paneSFXMainWnd.Init();
        m_paneSFXMainWnd.ShowWindow(SW_SHOW);

		pSceneEditorBase->SetEditState(SCENESTATE_SELECT);

        //layour main wnd
        RECT rectMain;
        rectMain.left    = 200;
        rectMain.right   = GetSystemMetrics(SM_CXFULLSCREEN) - 200;
        rectMain.top     = 200;
        rectMain.bottom  = GetSystemMetrics(SM_CYFULLSCREEN) - 200;
        m_paneSFXMainWnd.MoveWindow(&rectMain);

		IEKG3DScene* pScene = NULL;
        piSFXEditor->GetScene(&pScene);
		if (pScene)
		{
			pScene->AddOutputWindow("SFX Main", m_paneSFXMainWnd.m_hWnd,
				OUTPUTWND_SFXEDITOR_MAIN,0,&m_nOutputWindowID);
            pScene->EnablePostRenderEffect(m_nOutputWindowID, TRUE);
            pScene->EnablePostRenderEffect(m_nOutputWindowID, POST_EFFECT_SOCKWARE, TRUE);
            pScene->EnablePostRenderEffect(m_nOutputWindowID, POST_EFFECT_AOE, TRUE);

            // load config
            TCHAR szIniPath[MAX_PATH];
            g_GetFullPath(szIniPath, TEXT("config.ini"));
            IIniFile* pIniFile = g_OpenIniFile(szIniPath);

            if (pIniFile)
            {
                DWORD dwColor = 0xFF808080;
                pIniFile->GetStruct(TEXT("SfxEditor"), TEXT("bkcolor"), &dwColor, sizeof(DWORD));
                piSFXEditor->SetBkColor(dwColor);
                pIniFile->Release();
            }



		}
        // crate Left Model View Pane
        m_paneSFXModelView.Create(IDD_PANE_SFX_MODEL_VIEW, this);
        m_paneSFXModelView.SetDependWnd(this);
        SetWindowLong(m_paneSFXModelView.m_hWnd, GWL_EXSTYLE, 0);
        m_paneSFXModelView.m_dwWndState = SFX_WND_STATE_BIND;
        m_paneSFXModelView.ShowWindow(SW_SHOW);

        // Create Right Edit Pane
        m_paneSFXEditerWnd.SetDependWnd(this);
        m_paneSFXEditerWnd.Create(IDD_SFXEDITOR_TOOLVIEW, this);
        m_paneSFXEditerWnd.ShowWindow(SW_SHOW);
        m_paneSFXEditerWnd.m_dwWndState = SFX_WND_STATE_BIND;
        m_paneSFXEditerWnd.UpdateUI();

        // create key frame
        m_paneSFXKeyFrame.Create(IDD_SFXEDITOR_KEYFRAME, this);
        m_paneSFXKeyFrame.SetDependWnd(this);
        m_paneSFXKeyFrame.ShowWindow(SW_SHOW);
        m_paneSFXKeyFrame.m_dwWndState = SFX_WND_STATE_BIND;
        m_paneSFXKeyFrame.UpdateUI();

        //
        /*m_paneSFXSuedeTool.Create(IDD_DIALOG_SUEDE_TOOL, this);
        m_paneSFXSuedeTool.ModifyStyle(
            WS_CAPTION | WS_BORDER | WS_SIZEBOX, NULL, SWP_DRAWFRAME
        );*/
        /*m_paneSFXSuedeTool.SetParent(this);
        m_popuSocketBindDlg.Create(IDD_DIALOG_SFX_SOCKET_BIND, this);*/

		pSceneEditorBase->SetEditState(SCENESTATE_SELECT);
	}
}

void KSceneSFXEditorView::OnDestroy()
{
    IEKG3DScene* pScene =NULL;;
    IEKG3DSceneSFXEditor *piSFXEditor = NULL;

    if (m_pInvoker)
    {
        m_pInvoker->SendMessage(WM_SFXEDITORCLOSE_INVOKERCALLBACK);
    }
    
	KSceneSFXEditorDoc* pDoc = (KSceneSFXEditorDoc *)GetDocument();
	KGLOG_PROCESS_ERROR(pDoc);

    pDoc->GetSceneSFXEditor(&piSFXEditor);
    KGLOG_PROCESS_ERROR(piSFXEditor);

	piSFXEditor->GetScene(&pScene);
    KGLOG_PROCESS_ERROR(pScene);

	pScene->RemoveOutputWindow(m_nOutputWindowID);
	m_nOutputWindowID = -1;
Exit0:
	CView::OnDestroy();
}

void KSceneSFXEditorView::OnSize(UINT nType, int cx, int cy)
{
    CView::OnSize(nType, cx, cy);
    if (::IsWindow(m_paneSFXKeyFrame.m_hWnd))
        m_paneSFXKeyFrame.UpdateWndState();
    if (::IsWindow(m_paneSFXModelView.m_hWnd))
       m_paneSFXModelView.UpdateWndState();
    if (::IsWindow(m_paneSFXEditerWnd.m_hWnd))
        m_paneSFXEditerWnd.UpdateWndState();
    if (::IsWindow(m_paneSFXMainWnd.m_hWnd))
        m_paneSFXMainWnd.UpdateWndState();
  /*  if (::IsWindow(m_paneSFXKeyFrame.m_hWnd) && (::IsWindow(m_paneSFXSuedeTool.m_hWnd)))
    {
        RECT rectSFXKey;
        m_paneSFXKeyFrame.GetWindowRect(&rectSFXKey);
        ScreenToClient(&rectSFXKey);
        m_paneSFXSuedeTool.MoveWindow(&rectSFXKey);
    }*/
    UpdateUI();
}



void KSceneSFXEditorView::OnMeteor()
{
	KSceneSFXEditorMeteor* pMeteorEditor = m_paneSFXEditerWnd.GetMeteorPage();
	ASSERT(pMeteorEditor);
	pMeteorEditor->OnMeteorOpenClost();

}

void KSceneSFXEditorView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	// TODO: Add your specialized code here and/or call the base class
	IKG3DScene* pScene = NULL;
	IEKG3DSceneSFXEditor *pIEScene = NULL;
	KSceneSFXEditorDoc* pDoc = (KSceneSFXEditorDoc*) GetDocument();
	KGLOG_PROCESS_ERROR(pDoc);

	pDoc->GetSceneSFXEditor((IEKG3DSceneSFXEditor**)&pIEScene);
	pScene = dynamic_cast<IKG3DScene*>(pIEScene);
	KG_PROCESS_ERROR(pScene);

	g_GetEngineManager()->SetSoundFocusScene(pScene);
Exit0:
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}
