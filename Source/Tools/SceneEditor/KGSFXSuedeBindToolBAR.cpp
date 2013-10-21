////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGSFXSuedeBindToolBAR.cpp
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-8-31 15:26:09
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGSFXSuedeBindToolBar.h"
#include "KSceneSFXEditorFrame.h"
#include "IEKG3DSFX.h"
#include "KGSFXMessageBox.h"
//#include "KG3DSceneEditorBase.h"
#include "KSceneHelperFuncs.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(KGSFXSuedeBindToolBar, CDialog)

KGSFXSuedeBindToolBar::KGSFXSuedeBindToolBar(CWnd* pParent /*=NULL*/)
	: CDialog(KGSFXSuedeBindToolBar::IDD, pParent)
{
    m_nTrans_X = 0;
    m_nTrans_Y = 0;
    m_nTrans_Z = 0;
    m_nRota_X = 0;
    m_nRota_Y = 0;
    m_nRota_Z = 0;
}

KGSFXSuedeBindToolBar::~KGSFXSuedeBindToolBar()
{
}

void KGSFXSuedeBindToolBar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
    DDX_Check(pDX, IDC_SFX_LOOP, m_checkSFXLoop);
    DDX_Check(pDX, IDC_SFX_PLAY, m_checkSFXPlay);
    DDX_Check(pDX, IDC_MDL_LOOP, m_checkMDLLoop);
    DDX_Check(pDX, IDC_MDL_PLAY, m_checkMDLPlay);
    DDX_Check(pDX, IDC_MDL_ANPLAY, m_checkPlay);

    DDX_Control(pDX, IDC_TRANS_X, m_editTran_X);
    DDX_Control(pDX, IDC_TRANS_Y, m_editTran_Y);
    DDX_Control(pDX, IDC_TRANS_Z, m_editTran_Z);
    DDX_Control(pDX, IDC_ROTA_X,  m_editRota_X);
    DDX_Control(pDX, IDC_ROTA_Y,  m_editRota_Y);
    DDX_Control(pDX, IDC_ROTA_Z,  m_editRota_Z);

    DDX_Text(pDX, IDC_TRANS_X, m_nTrans_X);
    DDX_Text(pDX, IDC_TRANS_Y, m_nTrans_Y);
    DDX_Text(pDX, IDC_TRANS_Z, m_nTrans_Z);
    DDX_Text(pDX, IDC_ROTA_X,  m_nRota_X);
    DDX_Text(pDX, IDC_ROTA_Y,  m_nRota_Y);
    DDX_Text(pDX, IDC_ROTA_Z,  m_nRota_Z);

    DDX_Control(pDX, IDC_BUTTON_LOAD, m_buttonLoad);
    DDX_Control(pDX, IDC_BUTTON_SAVE, m_buttonSave);
    DDX_Control(pDX, IDC_BUTTON_RESET_MODEL, m_bottonRSMdl);
    DDX_Control(pDX, IDC_BUTTON_RESET_SFX, m_bottonRSFX);
}

BEGIN_MESSAGE_MAP(KGSFXSuedeBindToolBar, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_SAVE, &KGSFXSuedeBindToolBar::OnBnClickedButtonSave)
    ON_BN_CLICKED(IDC_BUTTON_LOAD, &KGSFXSuedeBindToolBar::OnBnClickedButtonLoad)
    ON_BN_CLICKED(IDC_SFX_LOOP, &KGSFXSuedeBindToolBar::OnBnClickedSFXLoop)
    ON_BN_CLICKED(IDC_SFX_PLAY, &KGSFXSuedeBindToolBar::OnBnClickedSFXPlay)
    ON_BN_CLICKED(IDC_MDL_LOOP, &KGSFXSuedeBindToolBar::OnBnClickedMDLLoop)
    ON_BN_CLICKED(IDC_MDL_PLAY, &KGSFXSuedeBindToolBar::OnBnClickedMDLPlay)
    ON_BN_CLICKED(IDC_BUTTON_RESET_SFX, &KGSFXSuedeBindToolBar::OnBnClickedResetSFX)
    ON_BN_CLICKED(IDC_BUTTON_RESET_MODEL, &KGSFXSuedeBindToolBar::OnBnClickedResetMDL)
    ON_BN_CLICKED(IDC_MDL_ANPLAY, &KGSFXSuedeBindToolBar::OnBnClickedPlay)
    ON_MESSAGE(WM_EDIT_RECEIVE_ENTER, OnEditEnter)
    ON_WM_TIMER()
END_MESSAGE_MAP()

void KGSFXSuedeBindToolBar::OnTimer(UINT_PTR nIDEvent)
{
    //KG3DSFXSuede* pSFXSuede = NULL;
    //GET_SFX_EDITOR();

    //if (pScene->m_listSelectedEntity.size() == 1 && (*(pScene->m_listSelectedEntity.begin())).m_Type == SCENEENTITY_MODEL)
    //{
    //    IEKG3DModel* pMDL = (IEKG3DModel*)((*(pScene->m_listSelectedEntity.begin())).m_pPointer);
    //    if (pMDL->GetType() == MESHTYPE_SFX)
    //    {
    //        m_editTran_X.EnableWindow(TRUE);
    //        m_editTran_Y.EnableWindow(TRUE);
    //        m_editTran_Z.EnableWindow(TRUE);
    //        m_editRota_X.EnableWindow(TRUE);
    //        m_editRota_Y.EnableWindow(TRUE);
    //        m_editRota_Z.EnableWindow(TRUE);
    //    }
    //    else
    //    {
    //        m_editTran_X.EnableWindow(FALSE);
    //        m_editTran_Y.EnableWindow(FALSE);
    //        m_editTran_Z.EnableWindow(FALSE);
    //        m_editRota_X.EnableWindow(FALSE);
    //        m_editRota_Y.EnableWindow(FALSE);
    //        m_editRota_Z.EnableWindow(FALSE);
    //    }
    //}
    //else
    //{
    //    m_editTran_X.EnableWindow(FALSE);
    //    m_editTran_Y.EnableWindow(FALSE);
    //    m_editTran_Z.EnableWindow(FALSE);
    //    m_editRota_X.EnableWindow(FALSE);
    //    m_editRota_Y.EnableWindow(FALSE);
    //    m_editRota_Z.EnableWindow(FALSE);
    //}

    //KG_PROCESS_ERROR(::GetFocus() != m_editTran_X.m_hWnd);
    //KG_PROCESS_ERROR(::GetFocus() != m_editTran_Y.m_hWnd);
    //KG_PROCESS_ERROR(::GetFocus() != m_editTran_Z.m_hWnd);

    //KG_PROCESS_ERROR(::GetFocus() != m_editRota_X.m_hWnd);
    //KG_PROCESS_ERROR(::GetFocus() != m_editRota_Y.m_hWnd);
    //KG_PROCESS_ERROR(::GetFocus() != m_editRota_Z.m_hWnd);

    //if (nIDEvent)
    //{
    //    pSFXSuede = pScene->GetSFXSuede();
    //    if (pSFXSuede)
    //    {
    //        KG3DSFX* pSFX = (KG3DSFX*)pSFXSuede->GetSFX(pScene->GetCurSfxBindIndex());
    //        if (pSFX)
    //        {
    //            D3DXQUATERNION quan = pSFX->m_quateRotation;
    //            m_nTrans_X = pSFX->m_matWorld._41;//pSFX->m_vTransLocal.x;
    //            m_nTrans_Y = pSFX->m_matWorld._42;///pSFX->m_vTransLocal.y;
    //            m_nTrans_Z = pSFX->m_matWorld._43;//pSFX->m_vTransLocal.z;

    //            m_nRota_X = D3DXToDegree(quan.x);
    //            m_nRota_Y = D3DXToDegree(quan.y);
    //            m_nRota_Z = D3DXToDegree(quan.z);

    //            UpdateData(FALSE);
    //        }
    //    }
    //}
//Exit0:
    return CDialog::OnTimer(nIDEvent);
}

BOOL KGSFXSuedeBindToolBar::OnInitDialog()
{
    int nReslut = CDialog::OnInitDialog();

    m_checkMDLLoop = TRUE;
    m_checkMDLPlay = TRUE;
    m_checkSFXLoop = TRUE;
    m_checkSFXPlay = TRUE;
    m_checkPlay    = TRUE;

    UpdateData(FALSE);

    //SetTimer(1, 100, NULL);

    return nReslut;
}

LRESULT KGSFXSuedeBindToolBar::OnEditEnter(WPARAM wParam, LPARAM lParam)
{
    int nEditID = static_cast<int>(wParam);
    switch (nEditID)
    {
    case IDC_TRANS_X :
        break;
    case IDC_TRANS_Y :
        break;
    case IDC_TRANS_Z :
        break;
    case IDC_ROTA_X :
        break;
    case IDC_ROTA_Y :
        break;
    case IDC_ROTA_Z :
        break;
    default :
        break;
    }
    UpdateTransRota();
    return 1;
}

void KGSFXSuedeBindToolBar::UpdateTransRota()
{
    HRESULT hRetCode = E_FAIL;
    SCENEENTITYTYPE EntityType = SCENEENTITY_NONE;

    UpdateData();
    D3DXVECTOR3 vTrans = D3DXVECTOR3(m_nTrans_X, m_nTrans_Y, m_nTrans_Z);
    D3DXQUATERNION qRotation;
    qRotation.x = D3DXToRadian(m_nRota_X);
    qRotation.y = D3DXToRadian(m_nRota_Y);
    qRotation.z = D3DXToRadian(m_nRota_Z);
    qRotation.w = 1.0f;

    GET_SFX_EDITOR();
    IEKG3DSceneEntity *piEntity = NULL;
    IEKG3DModel* pMdl = NULL;
    IEKG3DSFX*   pSFX = NULL;
    IEKG3DSceneEditorBase *piSceneEditorBase = NULL;
	IEKG3DSceneSelectBase* pSelector = NULL;

	piSceneEditorBase->GetSceneSelector(&pSelector);
	_ASSERTE(pSelector);

    pScene->GetSceneEditBase(&piSceneEditorBase);

	/*KG_PROCESS_ERROR(pSelector->GetSelectedEntitySize() == 1);
	pSelector->GetSelectedEntity(0, &piEntity);
    ASSERT(piEntity);*/

	piEntity = KSF::GetUniqueSelectedEntity(piSceneEditorBase);
	KG_PROCESS_ERROR(NULL != piEntity);

    hRetCode = piEntity->GetType(&EntityType);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    KG_PROCESS_ERROR(EntityType == SCENEENTITY_MODEL);

    hRetCode = piEntity->GetPointer((VOID **)&pMdl);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    DWORD dwType = 0;
    hRetCode = pMdl->GetType(&dwType);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    KG_PROCESS_ERROR(dwType == MESHTYPE_SFX);

   /* vTrans.x += pMdl->m_matWorld._41;
    vTrans.y += pMdl->m_matWorld._42;
    vTrans.z += pMdl->m_matWorld._43;*/
    pMdl->SetTranslation(&vTrans);
    pMdl->SetRotation(&qRotation);

Exit0:
    return;
}

void KGSFXSuedeBindToolBar::OnBnClickedButtonSave()
{
    /*KG3DSFXSuede* pSFXSuede = NULL;
    GET_SFX_EDITOR();
    pSFXSuede = pScene->GetSFXSuede();
    CFileDialog dlg(
        FALSE,
        TEXT("bind"),
        NULL,
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        TEXT("绑定文件|*.bind||"),
        NULL,
        0
        );
    if (pSFXSuede && dlg.DoModal() == IDOK)
    {
        CString strName = dlg.GetPathName();
        if (FAILED(pSFXSuede->SaveToFile(strName.GetBuffer())))
            ::MessageBox(m_hWnd, "保存绑定文件出错", "Save Error!!", MB_OK | MB_ICONERROR);
    }*/
}

// 这段代码有点乱，有时间记得整理
void KGSFXSuedeBindToolBar::OnBnClickedButtonLoad()
{
    //KG3DSFXSuede* pSFXSuede = NULL;
    //IEKG3DModel*    pBindMdl  = NULL;
    //HRESULT  hRetCode = E_FAIL;
    //DWORD dwReturn;
    //CString strName = TEXT("");

    //GET_SFX_EDITOR();
    //pSFXSuede = pScene->GetSFXSuede();
    //pBindMdl  = pScene->m_pBindModel;
    //
    //if (pSFXSuede->GetSFXCount())
    //{
    //    TCHAR szInfo[1000];
    //    sprintf(
    //        szInfo,
    //        TEXT("你即将从绑定文件中导入绑定信息\n\n")
    //        TEXT("你可以选择以下操作: \n")
    //        TEXT("1. 按(追加)把绑定文件中的绑定信息追加到当前你编辑的绑定中\n")
    //        TEXT("2. 按(覆盖)导入的绑定信息将覆盖当前编辑的绑定\n")
    //        TEXT("3. 按(取消)取消导入操作\n")
    //    );
    //    dwReturn = SFXMessageBox(szInfo, "绑定信息导入", SFX_MESSAGE_STUEDE_LOAD);
    //
    //    switch (dwReturn)
    //    {
    //    case SFXMESSAGE_CANCEL :
    //        KG_PROCESS_SUCCESS(true);
    //        break;
    //    default :
    //        break;
    //    }
    //}
    //else
    //{
    //    dwReturn = SFXMESSAGE_COVER;
    //}

    //if (!pBindMdl)
    //{
    //    ::MessageBox(
    //        m_hWnd,
    //        TEXT("请先载入一个参考模型!!"),
    //        TEXT("Load Error!!"),
    //        MB_OK | MB_ICONERROR
    //    );
    //    KG_PROCESS_ERROR(false);
    //}
    //{
    //    CFileDialog dlg(
    //        TRUE,
    //        NULL,
    //        NULL,
    //        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
    //        TEXT("绑定文件|*.bind||"),
    //        NULL,
    //        0
    //    );
    //    if (pSFXSuede && dlg.DoModal() == IDOK)
    //        strName = dlg.GetPathName();
    //}

    //switch (dwReturn)
    //{
    //case SFXMESSAGE_COVER :
    //    CoverSFXSuedeSFX(strName.GetBuffer());
    //    break;
    //case SFXMESSAGE_ADD :
    //    AddSFXSuedeSFX(strName.GetBuffer());
    //    break;
    //default :
    //    break;
    //}

//Exit1:
//Exit0:
    return;
}

void KGSFXSuedeBindToolBar::AddSFXSuedeSFX(LPSTR pszPath)
{
    int nResult  = false;
    int nRetCode = false;

    nResult = true;
//Exit0:
    // Must Don't Release
    //SAFE_RELEASE(pSuedeLoad);
    return;
}

void KGSFXSuedeBindToolBar::CoverSFXSuedeSFX(LPSTR pszPath)
{
    int nResult  = false;
    int nRetCode = false;

    nResult = true;
//Exit0:
    return;
}

void KGSFXSuedeBindToolBar::OnBnClickedSFXLoop()
{
    /*KG3DSFXSuede* pSuede = NULL;
    UpdateData();
    GET_SFX_EDITOR();
    pSuede = pScene->GetSFXSuede();
    KG_PROCESS_ERROR(pScene);

    if (m_checkSFXLoop)
        pSuede->PlayAnimation(ENU_ANIMATIONPLAY_CIRCLE);
    else
        pSuede->PlayAnimation(ENU_ANIMATIONPLAY_ONCE);*/

//Exit0:
    return;
}

void KGSFXSuedeBindToolBar::OnBnClickedSFXPlay()
{
    /*KG3DSFXSuede* pSuede = NULL;
    UpdateData();
    GET_SFX_EDITOR();
    pSuede = pScene->GetSFXSuede();
    KG_PROCESS_ERROR(pScene);

    if (m_checkSFXPlay)
    {
        if (m_checkSFXLoop)
            pSuede->PlayAnimation(ENU_ANIMATIONPLAY_CIRCLE);
        else
            pSuede->PlayAnimation(ENU_ANIMATIONPLAY_ONCE);
    }
    else
    {
        pSuede->PauseAnimation();
    }*/

//Exit0:
    return;
}

void KGSFXSuedeBindToolBar::OnBnClickedMDLLoop()
{
    IEKG3DModel* pBindModel = NULL;
    IKG3DAnimationController *piAniController = NULL;
    UpdateData();
    GET_SFX_EDITOR();

    pScene->GetBindModel(&pBindModel);
    KG_PROCESS_ERROR(pBindModel);

    pBindModel->IEGetAnimationController(&piAniController);

    if (m_checkMDLLoop)
        pBindModel->PlayAnimation(
            ENU_ANIMATIONPLAY_CIRCLE,
            piAniController->GetAnimationFileName(),
            1.0f,
            0,
            NULL,
			NULL,
			ANICTL_PRIMARY
        );
    else
        pBindModel->PlayAnimation(
            ENU_ANIMATIONPLAY_ONCE,
            piAniController->GetAnimationFileName(),
            1.0f,
            0,
            NULL,
			NULL,
			ANICTL_PRIMARY
        );


Exit0:
    return;
}

void KGSFXSuedeBindToolBar::OnBnClickedMDLPlay()
{
    IEKG3DModel* pBindModel = NULL;
    IKG3DAnimationController *piAniController = NULL;
    UpdateData();
    GET_SFX_EDITOR();

    pScene->GetBindModel(&pBindModel);
    KG_PROCESS_ERROR(pBindModel);

    pBindModel->IEGetAnimationController(&piAniController);
    if (m_checkMDLPlay)
        piAniController->PauseAnimation(FALSE);
    else
        piAniController->PauseAnimation(TRUE);

Exit0:
    return;
}

void KGSFXSuedeBindToolBar::OnBnClickedPlay()
{
    UpdateData();
    if (m_checkPlay)
    {
        m_checkSFXPlay = TRUE;
        m_checkMDLPlay = TRUE;
    }
    else
    {
        m_checkSFXPlay = FALSE;
        m_checkMDLPlay = FALSE;
    }
    UpdateData(FALSE);
    OnBnClickedMDLPlay();
    OnBnClickedSFXPlay();
}

void KGSFXSuedeBindToolBar::OnBnClickedResetSFX()
{
    //GET_SFX_EDITOR();
    //KG3DSFXSuede* pSuede = pScene->GetSFXSuede();
    //KG3DSFX*      pSFX   = NULL;
    //IEKG3DModel*    pBindModel = NULL;
    //string        strName;

    ////qReset.x  = 0;
    ////qReset.y  = 0;
    ////qReset.z  = 0;
    ////qReset.w  = 1;

    //KG_PROCESS_ERROR(pSuede);

    //pSFX = (KG3DSFX*)pSuede->GetSFX(pScene->GetCurSfxBindIndex());
    //KG_PROCESS_ERROR(pSFX);

    //pSFX->GetBindModel((IKG3DModel**)&pBindModel);
    //KG_PROCESS_ERROR(pBindModel);

    ////pSFX->GetBindBoneName(strName);
    ////pSFX->UnBindFromOther();

    //pSFX->ReSet();


//Exit0:
    return;
}

void KGSFXSuedeBindToolBar::OnBnClickedResetMDL()
{
    GET_SFX_EDITOR();
    IEKG3DModel* pBindModel = NULL;

    pScene->GetBindModel(&pBindModel);
    KG_PROCESS_ERROR(pBindModel);

    pBindModel->IEReSet();

Exit0:
    return;
}

