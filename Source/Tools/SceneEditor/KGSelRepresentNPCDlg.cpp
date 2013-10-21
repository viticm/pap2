////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGSelRepresentNPCDlg.cpp
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2007-1-22 16:10:39
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "SceneEditorCommon.h"
#include "SceneEditor.h"
#include "KGSelRepresentNPCDlg.h"


IMPLEMENT_DYNAMIC(KGSelRepresentNPCDlg, CDialog)

KGSelRepresentNPCDlg::KGSelRepresentNPCDlg(CWnd* pParent /*=NULL*/)
	: CDialog(KGSelRepresentNPCDlg::IDD, pParent)
{
    m_nRenderWndId = -1;
}

KGSelRepresentNPCDlg::~KGSelRepresentNPCDlg()
{
}

void KGSelRepresentNPCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST, m_listNpcRepresent);
}


BEGIN_MESSAGE_MAP(KGSelRepresentNPCDlg, CDialog)
    ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL KGSelRepresentNPCDlg::OnInitDialog()
{
    int nResult  = false;
    int nRetCode = false;

    HRESULT hr = E_FAIL;

    IEKG3DScene* pScene = NULL;

    CWnd* pWnd = GetDlgItem(IDC_PREVIEW);
    KG_PROCESS_ERROR(pWnd);

    nRetCode = CDialog::OnInitDialog();
    KG_PROCESS_ERROR(nRetCode);

    pScene = g_pGraphicsTool->GetScene();
    KG_PROCESS_ERROR(pScene);

    hr = pScene->AddOutputWindow(
        TEXT("Select NPC Reprsents"),
        pWnd->m_hWnd,
        OUTPUTWND_SCENEEDITOR_PREVIEW,
        0,
        &m_nRenderWndId
        );
    KG_COM_PROCESS_ERROR(hr);

    ::SetTimer(m_hWnd, 0, 10, NULL);
    
    nResult = true;
Exit0:
    return nResult;
}

int KGSelRepresentNPCDlg::GetRenderWndID()
{
    return m_nRenderWndId;

}

void KGSelRepresentNPCDlg::OnTimer(UINT_PTR nIDEvent)
{
    g_pEngineManager->FrameMove();
    g_pEngineManager->Render();
    return CDialog::OnTimer(nIDEvent);
}

void KGSelRepresentNPCDlg::OnOK()
{
    KillTimer(0);
    CDialog::OnOK();
}

void KGSelRepresentNPCDlg::OnCancel()
{
    KillTimer(0);
    CDialog::OnOK();
}