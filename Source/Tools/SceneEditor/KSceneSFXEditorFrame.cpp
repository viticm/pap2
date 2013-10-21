////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSFXEditorFrame.cpp
//  Version     : 1.0
//  Creator     : Zeb
//  Create Date : 2005-10-18 17:17:35
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KSceneSFXEditorFrame.h"


////////////////////////////////////////////////////////////////////////////////
//(by dengzhihui 2006年7月5日 15:57:12
//#include "MainFrm.h"
//)
#include "KSceneModelEditorPanelView.h"
#include "KSceneModelEditorAnimationPanel.h"
#include "KSceneSFXEditorView.h"
#include "KSceneSFXEditorPanel.h"
#include "KSceneSFXEditorTimeView.h"
#include ".\kscenesfxeditorframe.h"
//#include "KOutputDebug.h"
#include "IEKG3DSFX.h"
#include "SceneEditorCommon.h"
#include "..\KG3DSFXFileDefine.h"
#include "KGImportSfxDlg.h"
#include "./CommonPro.Include/XTPDepreciated.h"
#include <XTToolkitPro.h> // Xtreme Toolkit Pro component library
#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNCREATE(KSceneSFXEditorFrame, CChildFrame)

void SFXUINeedUpdate()
{
	//(by dengzhihui 2006年7月5日 15:57:24
    CMDIFrameWnd *pMainFrame = (CMDIFrameWnd*)AfxGetMainWnd();
    KSceneSFXEditorFrame *pFrame = (KSceneSFXEditorFrame*)pMainFrame->GetActiveFrame();
	//)
    pFrame->UpdateUI();
}

KSceneSFXEditorFrame::KSceneSFXEditorFrame(void)
{
    m_bCreated = FALSE;
}

KSceneSFXEditorFrame::~KSceneSFXEditorFrame(void)
{
    //g_OutputDebugInfo("Shut SFX editor...");
}

void KSceneSFXEditorFrame::ActivateFrame(int nCmdShow)
{
	// TODO: Add your specialized code here and/or call the base class
	nCmdShow = SW_SHOWMAXIMIZED;
	MFCFramework::CChildFrame::ActivateFrame(nCmdShow);
}

void KSceneSFXEditorFrame::UpdateUI()
{
    //m_dlgSelect.UpdateUI();
    //m_dlgTexture.UpdateUI();
    //m_dlgKeyFrame.UpdateUI();

    KSceneSFXEditorView* pActiveView =
        dynamic_cast<KSceneSFXEditorView*>(GetActiveView());
    if (pActiveView)
        pActiveView->UpdateUI();
    /*KSceneSFXEditorPanel *pPanel1 =
        (KSceneSFXEditorPanel*)m_wndSplitter.GetPane(0, 1);
    pPanel1->UpdateUI();

    KSceneSFXEditorView *pView =
        (KSceneSFXEditorView*)m_wndSplitter.GetPane(0, 0);
    pView->UpdateUI();*/
}

KSceneSFXEditorPanel* KSceneSFXEditorFrame::GetPanelView()
{
    KSceneSFXEditorPanel* pEditPane = NULL;
    KSceneSFXEditorView*  pCurrView = NULL;

    pCurrView = KSceneSFXEditorView::GetActiveSFXView();
    KG_PROCESS_ERROR(pCurrView);

    pEditPane = &(pCurrView->m_paneSFXEditerWnd);

Exit0:
    return pEditPane;
}

KSceneSFXEditorKeyFrameDlg* KSceneSFXEditorFrame::GetKeysFrame()
{
    KSceneSFXEditorKeyFrameDlg* pKeyFrame = NULL;
    KSceneSFXEditorView*        pCurrView = NULL;

    pCurrView = KSceneSFXEditorView::GetActiveSFXView();
    KG_PROCESS_ERROR(pCurrView);

    pKeyFrame = &(pCurrView->m_paneSFXKeyFrame);

Exit0:
    return pKeyFrame;
}

KGSFXMainPaneWindow* KSceneSFXEditorFrame::GetRenderPane()
{
    KGSFXMainPaneWindow*        pRenderPane = NULL;
    KSceneSFXEditorView*        pCurrView   = NULL;

    pCurrView = KSceneSFXEditorView::GetActiveSFXView();
    KG_PROCESS_ERROR(pCurrView);

    pRenderPane = &(pCurrView->m_paneSFXMainWnd);

Exit0:
    return pRenderPane;
}

IEKG3DSceneSFXEditor* KSceneSFXEditorFrame::GetSfxScene()
{
    KSceneSFXEditorDoc* pDoc = dynamic_cast<KSceneSFXEditorDoc*>(GetActiveDocument());

    if (!pDoc)
        return NULL;

    IEKG3DSceneSFXEditor* pScene = NULL;
    pDoc->GetSceneSFXEditor(&pScene);

    return pScene;
}

BOOL KSceneSFXEditorFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class
    //g_OutputDebugInfo("Start SFX editor...");

	/*m_wndSplitter.CreateStatic(this, 1, 2);

    m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(KSceneSFXEditorView),CSize(550, 10), pContext);
	m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(KSceneSFXEditorPanel), CSize(250, 10), pContext);

	CRect rc;
	GetClientRect(&rc);
    m_wndSplitter.SetColumnInfo(0,600,30);
	m_wndSplitter.SetColumnInfo(1,180,180);
	m_wndSplitter.RecalcLayout();*/

    MFCFramework::CChildFrame::OnCreateClient(lpcs, pContext);
    m_bCreated = TRUE;

	return TRUE;//baseclass::OnCreateClient(lpcs, pContext);
}

void KSceneSFXEditorFrame::OnSetFocus(CWnd* pOldWnd)
{
	GET_SFX_EDITOR();
    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
	pSFX->ReflushTexture();
	return MFCFramework::CChildFrame::OnSetFocus(pOldWnd);
}

BEGIN_MESSAGE_MAP(KSceneSFXEditorFrame, MFCFramework::CChildFrame)
	ON_WM_SIZE()
    ON_WM_CREATE()
	ON_WM_SETFOCUS()
    ON_UPDATE_COMMAND_UI(ID_SFX_VIEW_SELECT, &KSceneSFXEditorFrame::OnUpdateSfxViewSelect)
    //ON_COMMAND(ID_SFX_VIEW_SELECT, &KSceneSFXEditorFrame::OnSfxViewSelect)
    ON_COMMAND(ID_SFX_VIEW_TEXTURE, &KSceneSFXEditorFrame::OnSfxViewTexture)
    ON_UPDATE_COMMAND_UI(ID_SFX_VIEW_TEXTURE, &KSceneSFXEditorFrame::OnUpdateSfxViewTexture)
    ON_COMMAND(ID_SFX_VIEW_KEYFRAME, &KSceneSFXEditorFrame::OnSfxViewKeyframe)
    ON_UPDATE_COMMAND_UI(ID_SFX_VIEW_KEYFRAME, &KSceneSFXEditorFrame::OnUpdateSfxViewKeyframe)
    ON_COMMAND(ID_EXPORT_SUEDE, &OnExportSuede)
    ON_COMMAND(ID_IMPORT_SUEDE, &OnImportSuede)
    ON_COMMAND(ID_IMPORT_SFX, &OnImportSfx)
    ON_COMMAND(ID_DOPE_SHEET, &OnOpenDopeSheet)
    ON_COMMAND(ID_COORD, &KSceneSFXEditorFrame::OnSetCoord)
    ON_COMMAND(ID_COMMAND_SFX_LINK, &KSceneSFXEditorFrame::OnSetLink)
END_MESSAGE_MAP()


void KSceneSFXEditorFrame::OnSetCoord()
{
    GET_SFX_EDITOR();

    CXTPCommandBars* pCommandBars = ((MFCFramework::CMainFrame*)AfxGetMainWnd())->GetCommandBars();

    if (!pCommandBars)
        return;

    CXTPToolBar* pToolBarStd = pCommandBars->GetToolBar(IDR_MAINFRAME);

    if (!pToolBarStd)
        return;

    CXTPControlComboBox* pControlCombo = (CXTPControlComboBox*)pToolBarStd->GetControl(18);
    if (!pControlCombo)
        return;

    int nSel = pControlCombo->GetCurSel();
    switch (nSel)
    {
    case 0 :
		pScene->SetCoordMode(KG3DCOORD_WORLD);
        break;
    case 1 :
        pScene->SetCoordMode(KG3DCOORD_LOCAL);
        break;
    default :
        break;
    }
    
}



void KSceneSFXEditorFrame::OnSetLink()
{
    int a = 0;
    a++;
}



void KSceneSFXEditorFrame::OnSize(UINT nType, int cx, int cy)
{
	MFCFramework::CChildFrame::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
    if (!m_bCreated)
        return;

	/*CRect rc;
	GetClientRect(&rc);
	m_wndSplitter.SetColumnInfo(0, cx - 320, 60);
	m_wndSplitter.SetColumnInfo(1, 250, 10);

	m_wndSplitter.RecalcLayout();*/
}

int KSceneSFXEditorFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (MFCFramework::CChildFrame::OnCreate(lpCreateStruct) == -1)
        return -1;

    CXTPCommandBars* pCommandBars = ((MFCFramework::CMainFrame*)AfxGetMainWnd())->GetCommandBars();
    if (!pCommandBars)
        return 0;
    CXTPToolBar* pToolBarCombo = pCommandBars->GetToolBar(IDR_MAINFRAME + 108);
    if (!pToolBarCombo)
        return 0;
    CXTPControlComboBox* pControlCombo = (CXTPControlComboBox*)pToolBarCombo->GetControl(1);
    if (!pControlCombo)
        return 0;
    pControlCombo->SetCurSel(1);

   /* GET_SFX_EDITOR_RET(-1);
    IEKG3DSFX* pSfx = NULL;
    pScene->GetSFX(&pSfx);
 
    m_dopeSheet.Create(IDD_DOPE_SHEET, pSfx);*/

    // TODO:  Add your specialized creation code here
    //m_dlgSelect.Create(IDD_SFXEDITOR_SELECT);
    //m_dlgSelect.MoveWindow(CRect(0, 0, 120, 500), TRUE);

    //m_dlgTexture.Create(IDD_SFXEDITOR_TEXTURE_PANEL);
    //m_dlgTexture.MoveWindow(CRect(130, 100, 800, 300), TRUE);

    //m_dlgKeyFrame.Create(IDD_SFXEDITOR_KEYFRAME);
    //m_dlgKeyFrame.MoveWindow(CRect(0, 150, 800, 280), TRUE);

    CMDIFrameWnd *pMainFrame = (CMDIFrameWnd*)AfxGetMainWnd();
   // pMainFrame->MDIActivate(this);

    return 0;
}

void KSceneSFXEditorFrame::OnUpdateSfxViewSelect(CCmdUI *pCmdUI)
{
    // TODO: Add your command update UI handler code here
    //if (m_dlgSelect.IsWindowVisible())
    //    pCmdUI->SetCheck(TRUE);
    //else
    //    pCmdUI->SetCheck(FALSE);
}

void KSceneSFXEditorFrame::OnSfxViewSelect()
{
    // TODO: Add your command handler code here
    //if (m_dlgSelect.IsWindowVisible())
    //    m_dlgSelect.ShowWindow(SW_HIDE);
    //else
    //    m_dlgSelect.ShowWindow(SW_NORMAL);
}

void KSceneSFXEditorFrame::OnSfxViewTexture()
{
    // TODO: Add your command handler code here
  /*  if (m_dlgTexture.IsWindowVisible())
        m_dlgTexture.ShowWindow(SW_HIDE);
    else
        m_dlgTexture.ShowWindow(SW_NORMAL);*/
}

void KSceneSFXEditorFrame::OnUpdateSfxViewTexture(CCmdUI *pCmdUI)
{
    // TODO: Add your command update UI handler code here
    //if (m_dlgTexture.IsWindowVisible())
    //    pCmdUI->SetCheck(TRUE);
    //else
    //{
    //    //m_dlgTexture.UpdateUI();
    //    if (pActiveView)
    //        pActiveView->UpdateUI();
    //    pCmdUI->SetCheck(FALSE);
    //}
}

void KSceneSFXEditorFrame::OnSfxViewKeyframe()
{
    // TODO: Add your command handler code here
    /*if (m_dlgKeyFrame.IsWindowVisible())
        m_dlgKeyFrame.ShowWindow(SW_HIDE);
    else
    {
        m_dlgKeyFrame.UpdateUI();
        m_dlgKeyFrame.ShowWindow(SW_NORMAL);
    }*/
    GetRenderPane()->ShowWindow(SW_SHOW);
    GetKeysFrame()->UpdateUI();
}

void KSceneSFXEditorFrame::OnUpdateSfxViewKeyframe(CCmdUI *pCmdUI)
{
    // TODO: Add your command update UI handler code here
    if (GetKeysFrame()->IsWindowVisible())
        pCmdUI->SetCheck(TRUE);
    else
        pCmdUI->SetCheck(FALSE);
}


void KSceneSFXEditorFrame::OnOpenDopeSheet()
{
   // m_dopeSheet.ShowWindow(SW_SHOW);
    
}

void KSceneSFXEditorFrame::OnImportSfx()
{
    GET_SFX_EDITOR();
    CFileDialog dlg(
        TRUE,
        TEXT("sfx"),
        NULL,
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        TEXT("sfx文件|*.sfx||"),
        NULL,
        0
        );
    if (dlg.DoModal() == IDOK)
    {
        CString strName = dlg.GetPathName();
        IFile* pFile = g_OpenFile(strName.GetBuffer());
        strName.ReleaseBuffer();
        if (!pFile)
        {
            MessageBox("Open File Error", "Error", MB_OK);
            return;
        }

        DWORD fileLen = pFile->Size();
        BYTE* pBuffer = new BYTE[fileLen];
        DWORD dwSize = pFile->Read(pBuffer, fileLen);
        
        ASSERT(dwSize == fileLen);

        IEKG3DSFX* pCurSfx = NULL;
        pScene->GetSFX(&pCurSfx);
        KGImportSfxDlg dlg(this, pBuffer, pCurSfx);
        if (dlg.DoModal() == IDOK)
        {
            DWORD dwImpFrameNum;
            DWORD dwImpInterval;
            DWORD dwPrecept;
            SfxImportInfo info = dlg.GetImportInfo(dwImpFrameNum, dwImpInterval, dwPrecept);
            if (pScene->ImportSfx(info, dwImpFrameNum, dwImpInterval, dwPrecept, pBuffer))
            {
                MessageBox("Import Sfx Success :)", "Success", MB_OK);
                GetKeysFrame()->UpdateUI();
            }
        }

        pFile->Close();
        delete[] pBuffer;    
    }
}

void KSceneSFXEditorFrame::OnExportSuede()
{
    GET_SFX_EDITOR();
//    TCHAR szFileName[PATH_MAX];
    CFileDialog dlg(
        FALSE,
        TEXT("bind"),
        NULL,
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        TEXT("绑定文件|*.bind||"),
        NULL,
        0
    );
    if (dlg.DoModal() == IDOK)
    {
        CString strName = dlg.GetPathName();
        if (SUCCEEDED(pScene->ExportSelModelSuede(strName.GetBuffer())))
        {
            MSGBOXPARAMS info;
            ZeroMemory(&info, sizeof(info));
            info.cbSize      = sizeof(info);
            info.dwStyle     = MB_OK | MB_USERICON;
            info.hInstance   = AfxGetInstanceHandle();
            info.hwndOwner   = this->m_hWnd;
            info.lpszIcon    = MAKEINTRESOURCE(IDC_OFFER);
            info.lpszCaption = TEXT("SUCCESS");
            info.lpszText    = TEXT("绑定信息导出成功n_n");
            ::MessageBoxIndirect(&info);
        }
    }
}

void KSceneSFXEditorFrame::OnImportSuede()
{
    GET_SFX_EDITOR();
//    TCHAR szFileName[PATH_MAX];
    CFileDialog dlg(
        TRUE,
        TEXT("bind"),
        NULL,
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        TEXT("绑定文件|*.bind||"),
        NULL,
        0
        );
    if (dlg.DoModal() == IDOK)
    {
        CString strName = dlg.GetPathName();
        if (SUCCEEDED(pScene->ImportSuedeToSelMdl(strName.GetBuffer())))
        {
            MSGBOXPARAMS info;
            ZeroMemory(&info, sizeof(info));
            info.cbSize      = sizeof(info);
            info.dwStyle     = MB_OK | MB_USERICON;
            info.hInstance   = AfxGetInstanceHandle();
            info.hwndOwner   = this->m_hWnd;
            info.lpszIcon    = MAKEINTRESOURCE(IDC_OFFER);
            info.lpszCaption = TEXT("SUCCESS");
            info.lpszText    = TEXT("绑定信息导入成功n_n");
            ::MessageBoxIndirect(&info);
        }
    }
}