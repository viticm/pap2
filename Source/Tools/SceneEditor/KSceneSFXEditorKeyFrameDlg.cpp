
#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSFXEditorKeyFrameDlg.h"
#include "IEKG3DSceneSFXEditor.h"
//#include "KG3DSceneEditorBase.h"
#include "KSceneSFXEditorFrame.h"
#include "IEKG3DSFX.h"
#include "KSceneSFXEditorView.h"
#include "KGSFXSocketBindDlg.h"
#include "..\KG3DEngine\KG3DSFXFileDefine.h"
#include "KSceneHelperFuncs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


#ifndef SFX_PLAY_TIMER
#define SFX_PLAY_TIMER 77987655
#endif

IMPLEMENT_DYNAMIC(KSceneSFXEditorKeyFrameDlg, CDialog)

KSceneSFXEditorKeyFrameDlg::KSceneSFXEditorKeyFrameDlg(CWnd* pParent /*=NULL*/)
{
    m_pTimeLine = NULL;
    m_eTimeLineDataType = SFX_TLDT_NONE;
    m_bAutoKey = FALSE;
    m_bSetKey  = TRUE;
    m_bLoop    = TRUE;
    m_dwWndState = SFX_WND_STATE_FREE;
    m_pDependWnd = NULL;
    m_nUpdateSliderDelay = 100;
}

KSceneSFXEditorKeyFrameDlg::~KSceneSFXEditorKeyFrameDlg()
{
}

void KSceneSFXEditorKeyFrameDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_BUTTON_M_RESET, m_buttonMoveReset);
    DDX_Control(pDX, IDC_BUTTON_R_RESET, m_buttonRotaReset);
    DDX_Control(pDX, IDC_SLIDER_KEYFRAME, m_Slider);
    DDX_Control(pDX, IDC_BUTTON_MOVE_START, m_buttonMoveStart);
    DDX_Control(pDX, IDC_BUTTON_MOVE_END, m_buttonMoveEnd);
    DDX_Control(pDX, IDC_CHECK_PLAY_SFX, m_checkPlay);
    DDX_Control(pDX, IDC_BUTTON_MOVE_LEFT, m_buttonMoveLeft);
    DDX_Control(pDX, IDC_BUTTON_MOVE_RIGHT, m_buttonMoveRight);
    DDX_Control(pDX, IDC_BUTTON_S_RESET, m_buttonScalReset);

    DDX_Control(pDX, IDC_CHECK_AUTOKEY, m_checkAutoKey);
    DDX_Control(pDX, IDC_BUTTON_SET_KEYFRAME, m_buttonSet);

    DDX_Control(pDX, IDC_STATIC_EDIT_STATE, m_staticEditState);
    DDX_Control(pDX, IDC_EDIT_TRANS_X, m_editTransX);
    DDX_Control(pDX, IDC_EDIT_TRANS_Y, m_editTransY);
    DDX_Control(pDX, IDC_EDIT_TRANS_Z, m_editTransZ);
    DDX_Control(pDX, IDC_STATIC_X, m_staticX);
    DDX_Control(pDX, IDC_STATIC_Y, m_staticY);
    DDX_Control(pDX, IDC_STATIC_Z, m_staticZ);

    DDX_Control(pDX, IDC_STATIC_CUR_FRAME, m_staticCurFrame);
    DDX_Control(pDX, IDC_EDIT_TOTAL_FRAME_NUM, m_editTotalKeyFrameNum);
    DDX_Control(pDX, IDC_EDIT_FRAME_INTERVAL, m_editKeyFrameInterval);

    //DDX_Control(pDX, IDC_STATIC_PARTICLE_NUM, m_staticParticleNum);
    DDX_Control(pDX, IDC_LOOP, m_buttonLoopOrOne);

    DDX_Check(pDX, IDC_CHECK_AUTOKEY, m_bAutoKey);
    DDX_Check(pDX, IDC_CHECK_SETKEY, m_bSetKey);

    //DDX_Control(pDX, IDC_BUTTON_BIND_ARM, m_buttonBindSocket);
    DDX_Control(pDX, IDC_ANI,  m_comb);
}


BEGIN_MESSAGE_MAP(KSceneSFXEditorKeyFrameDlg, CDialog)
    ON_WM_SIZE()
    ON_WM_VSCROLL()
    ON_WM_HSCROLL()
    ON_BN_CLICKED(IDC_CHECK_PLAY_SFX, &KSceneSFXEditorKeyFrameDlg::OnBnClickedButtonPlaySfx)
    ON_BN_CLICKED(IDC_BUTTON_MOVE_LEFT, &KSceneSFXEditorKeyFrameDlg::OnBnClickedButtonMoveLeft)
    ON_BN_CLICKED(IDC_BUTTON_MOVE_START, &KSceneSFXEditorKeyFrameDlg::OnBnClickedButtonMoveStart)
    ON_BN_CLICKED(IDC_BUTTON_MOVE_RIGHT, &KSceneSFXEditorKeyFrameDlg::OnBnClickedButtonMoveRight)
    ON_BN_CLICKED(IDC_BUTTON_MOVE_END, &KSceneSFXEditorKeyFrameDlg::OnBnClickedButtonMoveEnd)
    ON_BN_CLICKED(IDC_CHECK_AUTOKEY, &KSceneSFXEditorKeyFrameDlg::OnBnClickedCheckAutokey)
    ON_BN_CLICKED(IDC_CHECK_SETKEY, &KSceneSFXEditorKeyFrameDlg::OnBnClickedCheckSetkey)
    ON_BN_CLICKED(IDC_BUTTON_SET_KEYFRAME, &KSceneSFXEditorKeyFrameDlg::OnBnClickedButtonSetKeyframe)
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_BUTTON_BIND_ARM, &KSceneSFXEditorKeyFrameDlg::OnButtonClickSocketBind)
    ON_BN_CLICKED(IDC_LOOP, &KSceneSFXEditorKeyFrameDlg::OnBnClickedLoop)
    ON_WM_LBUTTONDBLCLK()
    ON_WM_CTLCOLOR()
    ON_CBN_SELCHANGE(IDC_ANI, &KSceneSFXEditorKeyFrameDlg::OnCbnSelchangeComboAni)
    ON_WM_SHOWWINDOW()
    ON_EN_CHANGE(IDC_EDIT_TRANS_Y, &KSceneSFXEditorKeyFrameDlg::OnEnChangeEditTransY)
    ON_EN_CHANGE(IDC_EDIT_TRANS_X, &KSceneSFXEditorKeyFrameDlg::OnEnChangeEditTransX)
    ON_CBN_SETFOCUS(IDC_ANI, &KSceneSFXEditorKeyFrameDlg::OnCbnSetfocusAni)
    ON_BN_CLICKED(IDC_BUTTON_M_RESET, &KSceneSFXEditorKeyFrameDlg::OnBnClickedMoveReset)
    ON_BN_CLICKED(IDC_BUTTON_R_RESET, &KSceneSFXEditorKeyFrameDlg::OnBnClickedRotaReset)
    ON_BN_CLICKED(IDC_BUTTON_S_RESET, &KSceneSFXEditorKeyFrameDlg::OnBnClickedScalReset)
END_MESSAGE_MAP()

void KSceneSFXEditorKeyFrameDlg::OnBnClickedMoveReset()
{
    HRESULT hRetCode = E_FAIL;
    GET_SFX_EDITOR();
    IEKG3DSceneEditorBase *pSceneEditorBase = NULL;

    hRetCode = pScene->GetSceneEditBase(&pSceneEditorBase);
    KGLOG_COM_PROCESS_ERROR(hRetCode);
    KG_PROCESS_ERROR(pSceneEditorBase);
    
    GETSELECT(pSceneEditorBase);

	{
		//if (pSelector->GetSelectedEntitySize() == 1)


		D3DXVECTOR3 vNorTrans = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		//pSelector->GetSelectedEntity(0, &pEntity);

		IEKG3DSceneEntity *pEntity = KSF::GetUniqueSelectedEntity(pScene);
		if(NULL == pEntity)
			return;

		pEntity->SetTranslation(&vNorTrans);
	}
    
Exit0:
    return;
}

void KSceneSFXEditorKeyFrameDlg::OnBnClickedRotaReset()
{
    HRESULT hRetCode = E_FAIL;
    GET_SFX_EDITOR();
    IEKG3DSceneEditorBase *pSceneEditorBase = NULL;
    
    hRetCode = pScene->GetSceneEditBase(&pSceneEditorBase);
    KGLOG_COM_PROCESS_ERROR(hRetCode);
    KG_PROCESS_ERROR(pSceneEditorBase);

    GETSELECT(pSceneEditorBase);

    /*if (pSelector->GetSelectedEntitySize() != 1)
		return;*/
    {
        
        D3DXVECTOR3 vRotation = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        D3DXQUATERNION qNorRotation;
        D3DXQuaternionRotationYawPitchRoll(
            &qNorRotation, 0.0f, 0.0f, 0.0f
        );

		/*IEKG3DSceneEntity *pEntity = NULL;
        pSelector->GetSelectedEntity(0, &pEntity);*/

		IEKG3DSceneEntity* pEntity = KSF::GetUniqueSelectedEntity(pScene);
		if(NULL == pEntity)
			return;

        pEntity->SetRotation(&qNorRotation, &vRotation);
    }
Exit0:
    return;
}

void KSceneSFXEditorKeyFrameDlg::OnBnClickedScalReset()
{
    HRESULT hRetCode = E_FAIL;
    GET_SFX_EDITOR();
    IEKG3DSceneEditorBase *pSceneEditorBase = NULL;

    hRetCode = pScene->GetSceneEditBase(&pSceneEditorBase);
    KGLOG_COM_PROCESS_ERROR(hRetCode);
    KG_PROCESS_ERROR(pSceneEditorBase);
    
    GETSELECT(pSceneEditorBase);

    //if (pSelector->GetSelectedEntitySize() == 1)
    {
        /*IEKG3DSceneEntity *pEntity = NULL;
        pSelector->GetSelectedEntity(0, &pEntity);*/

		IEKG3DSceneEntity* pEntity = KSF::GetUniqueSelectedEntity(pScene);
		if(NULL == pEntity)
			return;

        D3DXVECTOR3 vScal =  D3DXVECTOR3(1.0f, 1.0f, 1.0f);
        pEntity->SetScaling(&vScal);
        D3DXQUATERNION q;
        D3DXQuaternionIdentity(&q);
        pEntity->SetScalingRotation(&q);
    }
Exit0:
    return;
}

void KSceneSFXEditorKeyFrameDlg::OnCbnSelchangeComboAni()
{
    int nResult  = true;
    int nIndex   = 0;

    GET_SFX_EDITOR();

    CString strAinPath = TEXT("");
    TCHAR*  pExt = NULL;
    TCHAR   szAinName[128];
    TCHAR   szFullPath[MAX_PATH];

    nIndex = m_comb.GetCurSel();
    KG_PROCESS_ERROR(nIndex != CB_ERR);

    m_comb.GetLBText(nIndex, szAinName);
    g_GetFullPath(szFullPath, m_strSelPath.GetBuffer());
    pExt = strrchr(szFullPath, '\\');
    KG_PROCESS_ERROR(pExt);

    strAinPath = m_strSelPath;
    strAinPath.Replace("模型", "动作");
    strAinPath.Replace(++pExt, szAinName);
    m_strAinPath = strAinPath;

    g_GetFullPath(szFullPath, m_strAinPath.GetBuffer());
    pScene->UpdateBindOrRefrenceAni(szFullPath);

Exit0:
    return;
}

int KSceneSFXEditorKeyFrameDlg::FillComb()
{
    int nResult  = false;
    HRESULT hRetCode = E_FAIL;
    SCENEENTITYTYPE EntityType = SCENEENTITY_NONE;

    TCHAR  szFilePath[MAX_PATH];
    TCHAR  szFullPath[MAX_PATH];
    TCHAR* pExt = NULL;
    CFileFind fileFind;

    IEKG3DSceneEntity *pEntity = NULL;
    IEKG3DModel*   pSelModel = NULL;
    LPCTSTR pszName = NULL;
    IEKG3DSceneEditorBase *pSceneEditorBase = NULL;

    GET_SFX_EDITOR_RET(false);

    m_comb.ResetContent();

    hRetCode = pScene->GetSceneEditBase(&pSceneEditorBase);
    KGLOG_COM_PROCESS_ERROR(hRetCode);
    KG_PROCESS_ERROR(pSceneEditorBase);

	GETSELECT(pSceneEditorBase) false;

	IEKG3DSceneSelectionTool* pSelTool = NULL;
	pScene->GetSelectionToolInterface(&pSelTool);
	KG_PROCESS_ERROR(NULL != pSelTool);

	//if (pSelector->GetSelectedEntitySize())
	if(0 != pSelTool->GetSelectionCount())
    {
	   /* pSelector->GetSelectedEntity(0, &pEntity);
	    _ASSERTE(pSelector);*/

		IEKG3DSceneSelectionToolAccessProxy* pProxy = NULL;
		pSelTool->GetAccessProxy(&pProxy);
		KG_PROCESS_ERROR(NULL != pProxy);
		/*pProxy->Begin();
		pEntity = pProxy->GetCur();*/
		pEntity = pProxy->Reset();
		KG_PROCESS_ERROR(NULL != pEntity);
    }
    else
    {
        size_t size = pSelector->GetSelectableEntitySize();
        KG_PROCESS_ERROR(size);

        size_t i;

        for ( i = 0; i < size; i++)
        {
            pSelector->GetSelectableEntity((INT)i, &pEntity);

            SCENEENTITYTYPE Type;
            pEntity->GetType(&Type);
            if (Type == SCENEENTITY_MODEL)
                break;
        }

        KG_PROCESS_ERROR(i != size);

		//NULL;//这里的意思是找到第一个场景中的Entity
    }


    hRetCode = pEntity->GetType(&EntityType);
    KGLOG_COM_PROCESS_ERROR(hRetCode);
    KG_PROCESS_ERROR(EntityType == SCENEENTITY_MODEL);

    hRetCode = pEntity->GetPointer((VOID **)&pSelModel);
    KGLOG_COM_PROCESS_ERROR(hRetCode);
    KG_PROCESS_ERROR(pSelModel);


    //g_GetFullPath(szFullPath, m_strSelPath.GetBuffer());
    hRetCode = pSelModel->GetName(&pszName);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    m_strSelPath = pszName;
    strncpy(szFullPath, pszName, sizeof(szFilePath));
    pExt = strrchr(szFullPath, '\\');
    KG_PROCESS_ERROR(pExt);
    *pExt = '\0';
    pExt = strrchr(szFullPath, '\\');
    KG_PROCESS_ERROR(pExt);
    *pExt = '\0';

    sprintf(szFilePath, "%s\\动作\\%s", szFullPath, TEXT("*.*"));
    BOOL bWorking = fileFind.FindFile(szFilePath);
    while (bWorking)
    {
        bWorking = fileFind.FindNextFile();
        if (fileFind.IsDots())
            continue;
        CString strPath = fileFind.GetFilePath();
        CString strName = fileFind.GetFileName();
        if (fileFind.IsDirectory())
            continue;

        TCHAR  szName[MAX_PATH];
        strncpy(szName, strName.GetBuffer(), sizeof(szName));
        TCHAR* pszExt = strrchr(szName, '.');
        if (!pszExt)
            continue;
        if (!stricmp(pszExt, TEXT(".ani")))
            m_comb.AddString(szName);
        else
            continue;

    }
    m_comb.AddString(TEXT("浏览......"));
    fileFind.Close();

    nResult = true;
Exit0:
    return nResult;
}

void KSceneSFXEditorKeyFrameDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
}

void KSceneSFXEditorKeyFrameDlg::OnCancel()
{
}

void KSceneSFXEditorKeyFrameDlg::OnButtonClickSocketBind()
{
    /*GET_SFX_EDITOR();
    IEKG3DModel* pRefModel = pScene->m_pRefrenceModel;
    KSceneSFXEditorView* pView = dynamic_cast<KSceneSFXEditorView*>(pFrame->GetActiveView());
    if (pRefModel && pView)
    {
        pView->m_popuSocketBindDlg.Init();
        pView->m_popuSocketBindDlg.ShowWindow(SW_SHOW);
    }
    else
    {
        AfxMessageBox(TEXT("请先打开一个参考模型~"));
    }*/
}

HBRUSH KSceneSFXEditorKeyFrameDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    //if (nCtlColor == CTLCOLOR_BTN)
    //{
    //    if (((CButton*)pWnd)->GetCheck())
    //    {
    //        pDC->SetTextColor(RGB(255, 255, 255));
    //        pDC->SetBkColor(RGB(255, 0, 0));
    //        return CreateSolidBrush(RGB(255, 0, 0));
    //    }
    //}
    return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}

// KSceneSFXEditorKeyFrameDlg message handlers

BOOL KSceneSFXEditorKeyFrameDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    HICON hIconLoop = LoadIcon(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_ICON_LOOP));
    HICON hIconOne  = LoadIcon(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_ICON_ONE));
    if (m_bLoop)
        m_buttonLoopOrOne.SetIcon(hIconLoop);
    else
        m_buttonLoopOrOne.SetIcon(hIconOne);

    HICON hIcon = LoadIcon(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_ICON_PLAY));
    m_checkPlay.SetIcon(hIcon);

    m_buttonSet.LoadBitmaps(
        IDB_BITMAP_SET_N,
        IDB_BITMAP_SET_K,
        IDB_BITMAP_SET_N,
        IDB_BITMAP_SET_N
    );
    UpdateData(FALSE);

    // do this now!
    m_Slider.Init();


  

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneSFXEditorKeyFrameDlg::UpdateUI()
{
    GET_SFX_EDITOR();

    CString str;
    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    DWORD dwData = pSFX->GetTotalFrameNum();
    str.Format("%d", dwData);
    m_editTotalKeyFrameNum.SetWindowText(str);

    dwData = pSFX->GetFrameInterval();
    str.Format("%d", dwData);
    m_editKeyFrameInterval.SetWindowText(str);

    UpdateCurFrameUI();
    UpdateSliderCtrl();
    UpdateTransformUI();
}

void KSceneSFXEditorKeyFrameDlg::OnSetTranslation()
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;
    IEKG3DSceneEditorBase *pSceneEditorBase = NULL;

    GET_SFX_EDITOR();

    hRetCode = pScene->GetSceneEditBase(&pSceneEditorBase);
    KGLOG_COM_PROCESS_ERROR(hRetCode);
    KG_PROCESS_ERROR(pSceneEditorBase);

	GETSELECT(pSceneEditorBase);

	IEKG3DSceneSelectionToolAccessProxy* pProxy = KSF::GetSelectionProxy(pScene);
	if(NULL == pProxy)
		return;

	//for (INT i = 0;i < pSelector->GetSelectedEntitySize(); i++)
	for(IEKG3DSceneEntity *piEntity = pProxy->Reset(); NULL != piEntity; piEntity = pProxy->Next())
    {
		/*IEKG3DSceneEntity *piEntity = NULL;
		pSelector->GetSelectedEntity(i, &piEntity);*/
        D3DXVECTOR3 Value;
        hRetCode = piEntity->GetTranslation(&Value);
        KGLOG_COM_PROCESS_ERROR(hRetCode);
        CString str;
        m_editTransX.GetWindowText(str);
        Value.x = (float)atof(str);
        m_editTransY.GetWindowText(str);
        Value.y = (float)atof(str);
        m_editTransZ.GetWindowText(str);
        Value.z = (float)atof(str);
        piEntity->SetTranslation(&Value);
        piEntity->UpdateTransformation();
        //++i;
    }
//Exit1:
    hResult = S_OK;
Exit0:
    return;
}

void KSceneSFXEditorKeyFrameDlg::OnSetRotation()
{
    HRESULT hRetCode = E_FAIL;
    SCENEENTITYTYPE EntityType = SCENEENTITY_NONE;
    IEKG3DSceneEditorBase *pSceneEditorBase = NULL;

    GET_SFX_EDITOR();
    hRetCode = pScene->GetSceneEditBase(&pSceneEditorBase);
    KGLOG_COM_PROCESS_ERROR(hRetCode);
    KG_PROCESS_ERROR(pSceneEditorBase);

	GETSELECT(pSceneEditorBase);

	IEKG3DSceneSelectionToolAccessProxy* pProxy = KSF::GetSelectionProxy(pScene);
	if(NULL == pProxy)
		return;
   
	//for (INT i = 0; i < pSelector->GetSelectedEntitySize(); i ++)
	for(IEKG3DSceneEntity* piEntity = pProxy->Reset(); NULL != piEntity; piEntity = pProxy->Next())
    {
		/*IEKG3DSceneEntity *piEntity = NULL;
		pSelector->GetSelectedEntity(i, &piEntity);*/

        float x = 0, y = 0, z = 0;
        CString str;
        m_editTransX.GetWindowText(str);
        x = (float)atof(str) * D3DX_PI / 180;
        m_editTransY.GetWindowText(str);
        y = (float)atof(str) * D3DX_PI / 180;
        m_editTransZ.GetWindowText(str);
        z = (float)atof(str) * D3DX_PI / 180;

        D3DXQUATERNION q;
        D3DXQuaternionRotationYawPitchRoll(&q, y, x, z);
        piEntity->SetRotation(&q, &D3DXVECTOR3(x, y, z));

        hRetCode = piEntity->GetType(&EntityType);
        KGLOG_COM_PROCESS_ERROR(hRetCode);

        if (EntityType == SCENEENTITY_SFX_BILLBOARD)
        {
            SFX_ENTITY_DATA *pData = NULL;
            hRetCode = piEntity->GetRefData((VOID **)&pData);
            KGLOG_COM_PROCESS_ERROR(hRetCode);
            pData->fRotationX = x;
            pData->fRotationY = y;
            pData->fRotationZ = z;
        }
        piEntity->UpdateTransformation();
        //++i;
    }
Exit0:
    return;
}

void KSceneSFXEditorKeyFrameDlg::ShowModelFrameBound()
{
	int nResult  = false;
	int nRetCode = false;

	//DWORD dwNumFrame = 0;
	//FLOAT fFrameTime = 0;
	//FLOAT fTotalLeng = 0;
	//DWORD dwSFXFrameNum = 0;
	//float fSFXFrameTime = 0.0f;
	//size_t uAniNum = 0;
	//float fScale   = 0.0f;
	//float fStart   = 0.0f;

	//IEKG3DModel* pModel = NULL;
	//GET_SFX_EDITOR();
	//KG_PROCESS_ERROR(pScene);
	//pModel = pScene->m_lpSFX->GetModel(m_dwCurIndex);
	//KG_PROCESS_ERROR(pModel);

	//uAniNum = pModel->m_vecAnimationLoaded.size();
	//KG_PROCESS_ERROR(uAniNum);

	//dwSFXFrameNum = pScene->m_lpSFX->GetTotalFrameNum();
	//fSFXFrameTime = (float)pScene->m_lpSFX->GetFrameInterval();
	//pAnimation    = pModel->m_vecAnimationLoaded[uAniNum - 1];
	//fStart		  = pScene->m_lpSFX->GetModelStartFrame();

	//pAnimation->GetClipData(0, clipInfo);

	//dwNumFrame = clipInfo.pClip->GetNumFrames();
	//fFrameTime = clipInfo.pClip->GetFrameLength();
	//fTotalLeng = fFrameTime * dwNumFrame;

	//fScale = fFrameTime / fSFXFrameTime;

	nResult = true;
//Exit0:
	return;
}

void KSceneSFXEditorKeyFrameDlg::OnSetScale()
{
    HRESULT hRetCode = E_FAIL;
    IEKG3DSceneEditorBase *pSceneEditorBase = NULL;

    GET_SFX_EDITOR();

    hRetCode = pScene->GetSceneEditBase(&pSceneEditorBase);
    KGLOG_COM_PROCESS_ERROR(hRetCode);
    KG_PROCESS_ERROR(pSceneEditorBase);

	GETSELECT(pSceneEditorBase);


	//for(INT i = 0; i < pSelector->GetSelectedEntitySize(); i ++)
 //   {
 //       IEKG3DSceneEntity *piEntity = NULL;//&(*i);
	//	pSelector->GetSelectedEntity(i, &piEntity);
	IEKG3DSceneSelectionToolAccessProxy* pProxy = KSF::GetSelectionProxy(pScene);
	if(NULL == pProxy)
		return;

	//for (INT i = 0; i < pSelector->GetSelectedEntitySize(); i ++)
	for(IEKG3DSceneEntity* piEntity = pProxy->Reset(); NULL != piEntity; piEntity = pProxy->Next())
	{
        float x = 0, y = 0, z = 0;
        CString str;
        m_editTransX.GetWindowText(str);
        x = (float)atof(str);
        m_editTransY.GetWindowText(str);
        y = (float)atof(str);
        m_editTransZ.GetWindowText(str);
        z = (float)atof(str);
        piEntity->SetScaling(&D3DXVECTOR3(x, y, z));
        //++i;
    }
Exit0:
    return;
}

void KSceneSFXEditorKeyFrameDlg::OnSetTransX()
{
    GET_SFX_EDITOR();

    IEKG3DSceneEditorBase *pSceneEditorBase = NULL;
    pScene->GetSceneEditBase(&pSceneEditorBase);
    if (!pSceneEditorBase)
        return;
    GETSELECT(pSceneEditorBase);

    if (GetEditState(pSceneEditorBase) == SCENESTATE_MOVE)
        OnSetTranslation();
    else if (GetEditState(pSceneEditorBase) == SCENESTATE_ROTATE)
    {
		IEKG3DSceneEntity* pEntity = KSF::GetUniqueSelectedEntity(pScene);

        //if (pSelector->GetSelectedEntitySize() == 1)
		if(NULL != pEntity)
        {
            m_editTransY.SetWindowText(TEXT(""));
            m_editTransZ.SetWindowText(TEXT(""));
            CString strValue;
            m_editTransX.GetWindowText(strValue);      
            float fValue = static_cast<float>(atof(strValue.GetBuffer()));
            //IEKG3DSceneEntity *pEntity = NULL;
            //pSelector->GetSelectedEntity(0, &pEntity);
            pEntity->RotationX(D3DXToRadian(fValue));

        }
    }
    else if (GetEditState(pSceneEditorBase) == SCENESTATE_SCALE)
        OnSetScale();
}

void KSceneSFXEditorKeyFrameDlg::OnSetTransY()
{
    GET_SFX_EDITOR();

    IEKG3DSceneEditorBase *pSceneEditorBase = NULL;
    pScene->GetSceneEditBase(&pSceneEditorBase);
    if (!pSceneEditorBase)
        return;

    if (GetEditState(pSceneEditorBase) == SCENESTATE_MOVE)
        OnSetTranslation();
    else if (GetEditState(pSceneEditorBase) == SCENESTATE_ROTATE)
    {
        GETSELECT(pSceneEditorBase);


		IEKG3DSceneEntity* pEntity = KSF::GetUniqueSelectedEntity(pScene);

        //if (pSelector->GetSelectedEntitySize() == 1)
		if(NULL != pEntity)
        {
            PVOID pvPoint = NULL;
            m_editTransX.SetWindowText(TEXT(""));       
            m_editTransZ.SetWindowText(TEXT(""));
           /* IEKG3DSceneEntity *pEntity = NULL;
            pSelector->GetSelectedEntity(0, &pEntity);*/
            
            pEntity->GetPointer(&pvPoint);
            KG3DModel* pModel = (KG3DModel*)(pvPoint);
            CString strValue;
            m_editTransY.GetWindowText(strValue);      
            float fValue = static_cast<float>(atof(strValue.GetBuffer()));
            pEntity->RotationY(D3DXToRadian(fValue));

        }
    }
    else if (GetEditState(pSceneEditorBase) == SCENESTATE_SCALE)
        OnSetScale();

}

void KSceneSFXEditorKeyFrameDlg::OnSetTransZ()
{
    GET_SFX_EDITOR();

    IEKG3DSceneEditorBase *pSceneEditorBase = NULL;
    pScene->GetSceneEditBase(&pSceneEditorBase);
    if (!pSceneEditorBase)
        return;

    if (GetEditState(pSceneEditorBase) == SCENESTATE_MOVE)
        OnSetTranslation();
    else if (GetEditState(pSceneEditorBase) == SCENESTATE_ROTATE)
    {
        GETSELECT(pSceneEditorBase);

		IEKG3DSceneEntity* pEntity = KSF::GetUniqueSelectedEntity(pScene);
        /*if (pSelector->GetSelectedEntitySize() == 1)*/
		if(NULL != pEntity)
        {
            PVOID pvPoint = NULL;
            m_editTransX.SetWindowText(TEXT(""));       
            m_editTransY.SetWindowText(TEXT(""));
            /*IEKG3DSceneEntity *pEntity = NULL;
            pSelector->GetSelectedEntity(0, &pEntity);*/
            pEntity->GetPointer(&pvPoint);
            KG3DModel* pModel = (KG3DModel*)(pvPoint);
            CString strValue;
            m_editTransZ.GetWindowText(strValue);      
            float fValue = static_cast<float>(atof(strValue.GetBuffer()));
            pEntity->RotationZ(D3DXToRadian(fValue));
        }
    }
    else if (GetEditState(pSceneEditorBase) == SCENESTATE_SCALE)
        OnSetScale();
}

void KSceneSFXEditorKeyFrameDlg::UpdateCurFrameUI(bool bIsOnTimer /* = true */)
{
    GET_SFX_EDITOR();

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    CString str;
    float fData = pSFX->GetCurrentFrame();
    str.Format("%.1f", fData);
    m_staticCurFrame.SetWindowText(str);

    if (!bIsOnTimer)
    {
        KGSFXSoundPanel* pSoundPanel = 
            (KGSFXSoundPanel*)pFrame->GetPanelView()->GetSoundPage();
        pSoundPanel->UpdateUI();
    }

}

void KSceneSFXEditorKeyFrameDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    // TODO: Add your message handler code here
    if (m_Slider.m_hWnd == NULL)
        return;

    RECT rectSlider;
    RECT rectClient;

    // play slider
    m_Slider.GetWindowRect(&rectSlider);
    GetClientRect(&rectClient);
    rectClient.left  += 150;
    rectClient.right -= 150;
    rectClient.top    = 0;
    rectClient.bottom = rectSlider.bottom - rectSlider.top;
    m_Slider.MoveWindow(&rectClient);

    // play buttons
    rectClient.top += 2;
    RECT rectButton;
    rectButton.left   = rectClient.right + 15;
    rectButton.top    = rectClient.top;
    rectButton.right  = rectButton.left + 20;
    rectButton.bottom = rectButton.top + 20;
    if (::IsWindow(m_buttonMoveStart.m_hWnd))
        m_buttonMoveStart.MoveWindow(&rectButton);
    rectButton.left   = rectButton.right + 7;
    rectButton.right  = rectButton.left + 20;
    if (::IsWindow(m_buttonMoveLeft.m_hWnd))
        m_buttonMoveLeft.MoveWindow(&rectButton);
    rectButton.left   = rectButton.right + 2;
    rectButton.right  = rectButton.left + 20;
    if (::IsWindow(m_checkPlay.m_hWnd))
        m_checkPlay.MoveWindow(&rectButton);
    rectButton.left   = rectButton.right + 2;
    rectButton.right  = rectButton.left + 20;
    if (::IsWindow(m_buttonMoveRight.m_hWnd))
        m_buttonMoveRight.MoveWindow(&rectButton);
    rectButton.left   = rectButton.right + 7;
    rectButton.right  = rectButton.left + 20;
    if (::IsWindow(m_buttonMoveEnd.m_hWnd))
        m_buttonMoveEnd.MoveWindow(&rectButton);
    rectButton.left   = rectClient.right + 15;
    rectButton.top    = rectClient.top + 25;
    rectButton.right  = rectButton.left + 40;
    rectButton.bottom = rectButton.top + 40;
    if (::IsWindow(m_buttonSet.m_hWnd))
        m_buttonSet.MoveWindow(&rectButton);
    rectButton.left   = rectButton.right + 2;
    rectButton.right  = rectButton.left + 78;
    rectButton.bottom = rectButton.top + 19;
    if (GetDlgItem(IDC_CHECK_AUTOKEY))
        GetDlgItem(IDC_CHECK_AUTOKEY)->MoveWindow(&rectButton);
    rectButton.top    = rectButton.bottom + 2;
    rectButton.bottom = rectButton.top + 19;
    if (GetDlgItem(IDC_CHECK_SETKEY))
        GetDlgItem(IDC_CHECK_SETKEY)->MoveWindow(&rectButton);

    // static
    rectButton.left   = rectClient.left;
    rectButton.top    = rectClient.bottom + 4;
    rectButton.right  = rectButton.left + 50;
    rectButton.bottom = rectButton.top + 20;
    if (::IsWindow(m_staticEditState.m_hWnd))
        m_staticEditState.MoveWindow(&rectButton);
    rectButton.left   = rectButton.right + 5;
    rectButton.right  = rectButton.left + 10;
    if (::IsWindow(m_staticX.m_hWnd))
        m_staticX.MoveWindow(&rectButton);
    rectButton.left   = rectButton.right + 2;
    rectButton.right  = rectButton.left + 50;
    if (::IsWindow(m_editTransX.m_hWnd))
        m_editTransX.MoveWindow(&rectButton);
    rectButton.left   = rectButton.right + 2;
    rectButton.right  = rectButton.left + 10;
    if (::IsWindow(m_staticY.m_hWnd))
        m_staticY.MoveWindow(&rectButton);
    rectButton.left   = rectButton.right + 2;
    rectButton.right  = rectButton.left + 50;
    if (::IsWindow(m_editTransY.m_hWnd))
        m_editTransY.MoveWindow(&rectButton);
    rectButton.left   = rectButton.right + 2;
    rectButton.right  = rectButton.left + 10;
    if (::IsWindow(m_staticZ.m_hWnd))
        m_staticZ.MoveWindow(&rectButton);
    rectButton.left   = rectButton.right + 2;
    rectButton.right  = rectButton.left + 50;
    if (::IsWindow(m_editTransZ.m_hWnd))
        m_editTransZ.MoveWindow(&rectButton);

    rectButton.left   = rectButton.right + 2;
    rectButton.right  = rectButton.left + 70;
    if (GetDlgItem(IDC_BUTTON_M_RESET))
        GetDlgItem(IDC_BUTTON_M_RESET)->MoveWindow(&rectButton);

    rectButton.left   = rectButton.right + 2;
    rectButton.right  = rectButton.left + 70;
    if (GetDlgItem(IDC_BUTTON_R_RESET))
        GetDlgItem(IDC_BUTTON_R_RESET)->MoveWindow(&rectButton);

    rectButton.left   = rectButton.right + 2;
    rectButton.right  = rectButton.left + 70;
    if (GetDlgItem(IDC_BUTTON_S_RESET))
        GetDlgItem(IDC_BUTTON_S_RESET)->MoveWindow(&rectButton);
    
    rectButton.left   = rectButton.right + 2;
    rectButton.right  = rectButton.left + 70;
    if (::IsWindow(m_editTotalKeyFrameNum.m_hWnd))
        m_editTotalKeyFrameNum.MoveWindow(&rectButton);

    rectButton.left   = rectButton.right + 2;
    rectButton.right  = rectButton.left + 70;
    if (::IsWindow(m_editKeyFrameInterval.m_hWnd))
        m_editKeyFrameInterval.MoveWindow(&rectButton);

    rectButton.left   = rectButton.right + 2;
    rectButton.right  = rectButton.left + 70;
    if (::IsWindow(m_staticCurFrame.m_hWnd))
        m_staticCurFrame.MoveWindow(&rectButton);

    rectButton.left   = rectButton.right + 2;
    rectButton.right  = rectClient.right;
    if (::IsWindow(m_buttonLoopOrOne.m_hWnd))
        m_buttonLoopOrOne.MoveWindow(&rectButton);
    

}

void KSceneSFXEditorKeyFrameDlg::OnSetTotalFrameNum()
{
    GET_SFX_EDITOR();

    DWORD dwFrameNum = GetDlgItemInt(IDC_EDIT_TOTAL_FRAME_NUM);
    if (dwFrameNum > 0)
    {
        IEKG3DSFX *pSFX = NULL;
        pScene->GetSFX(&pSFX);
        pSFX->SetTotalFrameNum(dwFrameNum);
    }

    UpdateSliderCtrl();
	//UpdateModelAniBoundLine();
}

void KSceneSFXEditorKeyFrameDlg::OnSetFrameInterval()
{
    GET_SFX_EDITOR();

    DWORD dwFrameInterval = GetDlgItemInt(IDC_EDIT_FRAME_INTERVAL);
    if (dwFrameInterval > 0)
    {
        IEKG3DSFX *pSFX = NULL;
        pScene->GetSFX(&pSFX);
        pSFX->SetFrameInterval(dwFrameInterval);
    }

	UpdateModelAniBoundLine();
	UpdateSliderCtrl();
}

void KSceneSFXEditorKeyFrameDlg::UpdateTransformUI()
{
    HRESULT hRetCode = E_FAIL;
    SCENEENTITYTYPE EntityType = SCENEENTITY_NONE;
    VOID *pvEntityPointer = NULL;

    GET_SFX_EDITOR();

    IEKG3DSceneEditorBase *pSceneEditorBase = NULL;
    pScene->GetSceneEditBase(&pSceneEditorBase);
    if (!pSceneEditorBase)
        return;

    if (GetEditState(pSceneEditorBase) == SCENESTATE_SELECT)
        m_staticEditState.SetWindowText("选择");
    else if (GetEditState(pSceneEditorBase) == SCENESTATE_MOVE)
        m_staticEditState.SetWindowText("平移");
    else if (GetEditState(pSceneEditorBase) == SCENESTATE_COPY)
        m_staticEditState.SetWindowText("拷贝");
    else if (GetEditState(pSceneEditorBase) == SCENESTATE_ROTATE)
        m_staticEditState.SetWindowText("旋转");
    else if (GetEditState(pSceneEditorBase) == SCENESTATE_SCALE)
        m_staticEditState.SetWindowText("缩放");
    else
        m_staticEditState.SetWindowText("");

	GETSELECT(pSceneEditorBase);

	IEKG3DSceneSelectionTool* pSelTool = NULL;
	pScene->GetSelectionToolInterface(&pSelTool);
	if(NULL == pSelTool)
		return;

	//if (pSelector->GetSelectedEntitySize() != 1)
	if(1 != pSelTool->GetSelectionCount())
    {
        m_editTransX.EnableWindow(FALSE);
        m_editTransY.EnableWindow(FALSE);
        m_editTransZ.EnableWindow(FALSE);
        return;
    }

    if (m_checkPlay.GetCheck())
    {
        m_editTransX.EnableWindow(FALSE);
        m_editTransY.EnableWindow(FALSE);
        m_editTransZ.EnableWindow(FALSE);
    }
    else
    {
        m_editTransX.EnableWindow();
        m_editTransY.EnableWindow();
        m_editTransZ.EnableWindow();
    }

    IEKG3DSFXParticleLauncher *pLauncher = NULL;
    IEKG3DSFXBillboard *pBillboard = NULL;
    IEKG3DSFXMeteor* pMeteor = NULL;

    CString strX, strY, strZ;

    IEKG3DSceneEntity *piEntity = NULL;
	//pSelector->GetSelectedEntity(0, &piEntity);
 //   ASSERT(piEntity);

	piEntity = KSF::GetFirstSelectedEntity(pScene);

    hRetCode = piEntity->GetType(&EntityType);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = piEntity->GetPointer(&pvEntityPointer);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    switch (GetEditState(pSceneEditorBase))
    {
    case SCENESTATE_SELECT:
    case SCENESTATE_COPY:
        {
			// 打开这几句，会猛发WM_MOUSEMOVE - -！
            /*m_editTransX.EnableWindow(FALSE);
            m_editTransY.EnableWindow(FALSE);
            m_editTransZ.EnableWindow(FALSE);*/
        }
        break;
    case SCENESTATE_MOVE:
        {
            D3DXVECTOR3 v;
            piEntity->GetTranslation(&v);
            strX.Format("%.3f", v.x);
            strY.Format("%.3f", v.y);
            strZ.Format("%.3f", v.z);

            if (EntityType == SCENEENTITY_SFX_LAUNCHER)
            {
                pLauncher = (IEKG3DSFXParticleLauncher *)pvEntityPointer;
                if (m_pTimeLine != &pLauncher->GetRtsTimeLine()->m_TranslationTimeLine )
                    SetTimeLine("平移", &pLauncher->GetRtsTimeLine()->m_TranslationTimeLine, SFX_TLDT_VECTOR3);
            }
            else if (EntityType == SCENEENTITY_SFX_BILLBOARD)
            {
                pBillboard = (IEKG3DSFXBillboard *)pvEntityPointer;
                if (m_pTimeLine != pBillboard->GetPositionLine())
                    SetTimeLine("平移", pBillboard->GetPositionLine(), SFX_TLDT_VECTOR3);
            }
            else if (EntityType == SCENEENTITY_SFX_METEOR)
            {
                pMeteor = (IEKG3DSFXMeteor*)pvEntityPointer;
                if (m_pTimeLine != &pMeteor->GetRtsTimeLine()->m_TranslationTimeLine )
                    SetTimeLine("平移", &pMeteor->GetRtsTimeLine()->m_TranslationTimeLine, SFX_TLDT_VECTOR3);
            }
        }
        break;
    case SCENESTATE_SCALE:
        {
            D3DXVECTOR3 v(1.0f, 1.0f, 1.0f);
            piEntity->GetScaling(&v);
            strX.Format("%.3f", v.x);
            strY.Format("%.3f", v.y);
            strZ.Format("%.3f", v.z);

            if (EntityType == SCENEENTITY_SFX_LAUNCHER)
            {
                pLauncher = (IEKG3DSFXParticleLauncher*)pvEntityPointer;
                if (m_pTimeLine != &pLauncher->GetRtsTimeLine()->m_ScalingTimeLine)
                    SetTimeLine("缩放", &pLauncher->GetRtsTimeLine()->m_ScalingTimeLine, SFX_TLDT_VECTOR3);
            }
            else if (EntityType == SCENEENTITY_SFX_METEOR)
            {
                pMeteor = (IEKG3DSFXMeteor*)pvEntityPointer;
                if (m_pTimeLine != &pMeteor->GetRtsTimeLine()->m_ScalingTimeLine )
                    SetTimeLine("缩放", &pMeteor->GetRtsTimeLine()->m_ScalingTimeLine, SFX_TLDT_VECTOR3);
            }
        }
        break;
    case SCENESTATE_ROTATE:
        {
            //D3DXVECTOR3 v;
            //D3DXQUATERNION q;
            //if (e.m_Type == SCENEENTITY_SFX_LAUNCHER)
            //{
            //    e.GetRotation(&q, &v);
            //    v.x = v.x * 180 / D3DX_PI;
            //    v.y = v.y * 180 / D3DX_PI;
            //    v.z = v.z * 180 / D3DX_PI;
            //}
            //else if (e.m_Type == SCENEENTITY_SFX_BILLBOARD)
            //{
            //    SFX_ENTITY_DATA *pData = (SFX_ENTITY_DATA*)e.m_pRefData;
            //    v.x = pData->fRotationX * 180 / D3DX_PI;
            //    v.y = pData->fRotationY * 180 / D3DX_PI;
            //    v.z = pData->fRotationZ * 180 / D3DX_PI;
            //}
            //else if (e.m_Type == SCENEENTITY_MODEL)
            //{
            //    //KG3DModel* pModel = (KG3DModel*)e.m_pPointer;
            //    //D3DXVECTOR3 vX = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
            //    //D3DXVECTOR3 vY = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
            //    //D3DXVECTOR3 vZ = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
            //    //D3DXVECTOR3 vv;
            //    //D3DXQUATERNION q;
            //    //D3DXMATRIX mat;
            //    //pModel->GetRotation(&q);
            //    //D3DXMatrixRotationQuaternion(&mat, &q);

            //}
            //else
            //{
            //    v.x = 0;
            //    v.y = 0;
            //    v.z = 0;
            //}
            //strX.Format("%.3f", v.x);
            //strY.Format("%.3f", v.y);
            //strZ.Format("%.3f", v.z);
            strX.Format("%.3f", D3DXToDegree(pScene->GetRotationCoordAngelX()));
            strY.Format("%.3f", D3DXToDegree(pScene->GetRotationCoordAngelY()));
            strZ.Format("%.3f", D3DXToDegree(pScene->GetRotationCoordAngelZ()));
            if (EntityType == SCENEENTITY_SFX_LAUNCHER)
            {
                pLauncher = (IEKG3DSFXParticleLauncher*)pvEntityPointer;
                if (m_pTimeLine != &pLauncher->GetRtsTimeLine()->m_RotationTimeLine)
                    SetTimeLine("旋转", &pLauncher->GetRtsTimeLine()->m_RotationTimeLine, SFX_TLDT_VECTOR4);
            }
            else if (EntityType == SCENEENTITY_SFX_METEOR)
            {
                pMeteor = (IEKG3DSFXMeteor*)pvEntityPointer;
                if (m_pTimeLine != &pMeteor->GetRtsTimeLine()->m_RotationTimeLine )
                    SetTimeLine("旋转", &pMeteor->GetRtsTimeLine()->m_RotationTimeLine, SFX_TLDT_VECTOR4);
            }
        }
        break;
    default:
        break;
    }

    m_editTransX.SetWindowText(strX);
    m_editTransY.SetWindowText(strY);
    m_editTransZ.SetWindowText(strZ);

Exit0:
    return;
}

void KSceneSFXEditorKeyFrameDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{

}

void KSceneSFXEditorKeyFrameDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    // must do this to be sure is the scroolbar i regardful
    /*if (pScrollBar->m_hWnd != m_Slider.m_hWnd)
        return;*/

    GET_SFX_EDITOR();

    // do this to get real pos!
    static int nPosSave = 0; 
    nPos = m_Slider.GetCurPos();

    if ((UINT)nPosSave == nPos)
        return;

    nPosSave = nPos;

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    if (nPos >= (UINT)pSFX->GetTotalFrameNum())
        return;

    pSFX->SetCurrentFrame((float)nPos);
    pSFX->TransformationFrameMove();

    UpdateCurFrameUI();
    UpdateTransformUI();

    /*if (pScene->m_pRefrenceModel && pScene->m_pRefrenceModel->GetAnimationController(ANICTL_PRIMARY)())
        pScene->m_pRefrenceModel->GetAnimationController(ANICTL_PRIMARY)()->SeekAnimation(
        ANIMATION_SEEK_SET, (float)nPos * pScene->m_lpSFX->GetFrameInterval()
    );*/
    pScene->RefModelsSeekAnimation(
        ANIMATION_SEEK_SET, (float)nPos * pSFX->GetFrameInterval()
    );


    KSceneSFXEditorPanel *pPanel = (KSceneSFXEditorPanel*)pFrame->GetPanelView();
    if (pPanel->GetActivePageIndex() == 0)
    {
        KSceneSFXEditorParticlePage *pParticlePage = (KSceneSFXEditorParticlePage*)pPanel->GetParticlePage();
        pParticlePage->UpdateParticleKindUI();
        pParticlePage->UpdateLauncherUI();
        pParticlePage->UpdateForceFieldUI();
    }
    else if (pPanel->GetActivePageIndex() == 1)
    {
        KSceneSFXEditorBillboardPage *pBillboardPage = (KSceneSFXEditorBillboardPage*)pPanel->GetBillboardPage();
        pBillboardPage->UpdateSpecifiedUI();
    }

    CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

template<class T>
void KSceneSFXEditorKeyFrameDlg::UpdateSliderTics()
{
    TKG3DTimeLine<T>* pTimeLine = (TKG3DTimeLine<T>*)m_pTimeLine;
    DWORD nFrameNum = (DWORD)pTimeLine->GetSize();
    for (DWORD i = 0; i < nFrameNum; ++i)
    {
        int nFrame;
        T Data;
        pTimeLine->GetKeyFrameByIndex(&nFrame, &Data, i);
        m_Slider.SetTic(nFrame);
    }
}

void KSceneSFXEditorKeyFrameDlg::UpdateSliderCtrl()
{
    GET_SFX_EDITOR();

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);

    m_Slider.ClearTics();
    m_Slider.SetRange(0, (int)pSFX->GetTotalFrameNum() - 1);
    switch (m_eTimeLineDataType)
    {
    case SFX_TLDT_NONE:
        return;
    case SFX_TLDT_INT:
        UpdateSliderTics<int>();
        break;
    case SFX_TLDT_D3DXCOLOR:
        UpdateSliderTics<D3DXCOLOR>();
        break;
    case SFX_TLDT_FLOAT:
        UpdateSliderTics<float>();
        break;
    case SFX_TLDT_VECTOR3:
        UpdateSliderTics<D3DXVECTOR3>();
        break;
    case SFX_TLDT_VECTOR2:
        UpdateSliderTics<D3DXVECTOR2>();
        break;
    case SFX_TLDT_SOUND :
        UpdateSliderTics<_SoundInfo>();
        break;
    case SFX_TLDT_VECTOR4:
        UpdateSliderTics<D3DXQUATERNION>();
        break;
    case SFX_TLDT_VECTOR5:
        UpdateSliderTics<FLOAT5>();
        break;
    default:
        ASSERT(0);
    }
}

template<class T>
size_t KSceneSFXEditorKeyFrameDlg::GetTimeLineSize()
{
    TKG3DTimeLine<T>* pTimeLine = (TKG3DTimeLine<T>*)m_pTimeLine;
    return pTimeLine->GetSize();
}


void KSceneSFXEditorKeyFrameDlg::UpdateModelAniBoundLine()
{
	GET_SFX_EDITOR();
	
	IEKG3DSFX *pSFX = NULL;
	pScene->GetSFX(&pSFX);

	if (!pSFX)
		return;

	float   fSpeed = 0.0f;
	float	fFrameLength = 0.0f;
	int		nTatalFrame = 0;
	int		nFrame = 0;
	int		nDate  = 0;
	DWORD	dwNumFrame = 0;
	float   fFrameTime = 0.f;

	size_t uNumModel = pSFX->GetNumModels();

	for (size_t i = 0; i < uNumModel; i++)
	{
		pSFX->GetModelAnimationSpeed(&fSpeed, i);
		pSFX->GetModelAnimationFrameTime(&fFrameTime, i);
		pSFX->GetModelAnimationFrameCount(&dwNumFrame, i);

		fFrameLength = (dwNumFrame - 1) * fFrameTime / fSpeed;
		nTatalFrame  = static_cast<int>(fFrameLength / pSFX->GetFrameInterval());

		if (nTatalFrame <= 0)
			nTatalFrame  = 1;

		TKG3DTimeLine<int>* pLine = static_cast<TKG3DTimeLine<int>*>(pSFX->GetModelAniBoundTimeLine((DWORD)i));
		pLine->GetKeyFrameByIndex(&nFrame, &nDate, 1);
		pLine->RemoveKeyFrame(nFrame);
		pLine->GetKeyFrameByIndex(&nFrame, &nDate, 0);
		nTatalFrame = nFrame + nTatalFrame;
		pLine->InsertKeyFrame(nTatalFrame, nTatalFrame);
	}

}


void KSceneSFXEditorKeyFrameDlg::SetTimeLine(const char cszTimeLineName[], TKG3DTimeLinebase* pTimeLine, SFX_TIME_LINE_DATA_TYPE t)
{
    /*if (pTimeLine == m_pTimeLine)
        return;*/

    m_pTimeLine = pTimeLine;
    m_eTimeLineDataType = t;
    //if (!pTimeLine)
    //    return;
    CString str;
    switch (m_eTimeLineDataType)
    {
    case SFX_TLDT_VECTOR4 :
        str.Format("%s(%d)", cszTimeLineName, GetTimeLineSize<D3DXQUATERNION>());
        break;
    case SFX_TLDT_VECTOR5 :
        str.Format("%s(%d)", cszTimeLineName, GetTimeLineSize<FLOAT5>());
        break;
    case SFX_TLDT_NONE:
        str = "";
        break;
    case SFX_TLDT_INT:
        str.Format("%s(%d)", cszTimeLineName, GetTimeLineSize<int>());
        break;
    case SFX_TLDT_D3DXCOLOR:
        str.Format("%s(%d)", cszTimeLineName, GetTimeLineSize<D3DXCOLOR>());
        break;
    case SFX_TLDT_FLOAT:
        str.Format("%s(%d)", cszTimeLineName, GetTimeLineSize<float>());
        break;
    case SFX_TLDT_VECTOR3:
        str.Format("%s(%d)", cszTimeLineName, GetTimeLineSize<D3DXVECTOR3>());
        break;
    case SFX_TLDT_VECTOR2:
        str.Format("%s(%d)", cszTimeLineName, GetTimeLineSize<D3DXVECTOR2>());
        break;
    case SFX_TLDT_SOUND:
        str.Format("%s(%d)", cszTimeLineName, GetTimeLineSize<_SoundInfo>());
        break;
    default:
        break;
    }

    SetWindowText(str);
    GetDlgItem(IDC_INFO)->SetWindowText(str);
    UpdateSliderCtrl();
}

void KSceneSFXEditorKeyFrameDlg::OnBnClickedButtonPlaySfx()
{
    GET_SFX_EDITOR();

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);

    if (m_checkPlay.GetCheck())
    {
        HICON hIcon = LoadIcon(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_ICON_STOP));
        m_checkPlay.SetIcon(hIcon);
        m_Slider.EnableWindow(FALSE);

        SetTimer(SFX_PLAY_TIMER, m_nUpdateSliderDelay, 0);
		
        if (m_bLoop)
        {
            pSFX->PlayAnimation(SFX_AP_LOOP, 1.f, (int)pSFX->GetCurrentFrame() + 1);
            //pSFX->SetModelAnimationMode(ENU_ANIMATIONPLAY_CIRCLE);
        }
        else
        {
            int nTatalFrame = pSFX->GetTotalFrameNum();
            int nCurrtFrame = (int)pSFX->GetCurrentFrame();
            if (nTatalFrame == nCurrtFrame)
                pSFX->SetCurrentFrame(0);
            pSFX->PlayAnimation(SFX_AP_ONE_TIME, 1.f, (int)pSFX->GetCurrentFrame() + 1);
            //pSFX->SetModelAnimationMode(ENU_ANIMATIONPLAY_ONCE);
        }
       /* if (pScene->m_pRefrenceModel)
        {
            SFX_ANIMATION_PLAY dwSFXPlayMode = SFX_AP_ONE_TIME;
            if (m_bLoop)
                dwSFXPlayMode = SFX_AP_LOOP;
            if ((pScene->m_pRefrenceModel->GetType() == MESHTYPE_SFX))
                ((KG3DSFX*)pScene->m_pRefrenceModel)->PlayAnimation(dwSFXPlayMode);
            else if ((pScene->m_pRefrenceModel->GetAnimationController(ANICTL_PRIMARY)()))
                pScene->m_pRefrenceModel->GetAnimationController(ANICTL_PRIMARY)()->PauseAnimation(FALSE);
        }*/
        SFX_ANIMATION_PLAY dwSFXPlayMode = SFX_AP_ONE_TIME;
        if (m_bLoop)
            dwSFXPlayMode = SFX_AP_LOOP;
        pScene->RefModelsPlaySFX(dwSFXPlayMode, FALSE);

        m_buttonMoveEnd.EnableWindow(FALSE);
        m_buttonMoveStart.EnableWindow(FALSE);
        m_buttonMoveLeft.EnableWindow(FALSE);
        m_buttonMoveRight.EnableWindow(FALSE);

        m_checkAutoKey.EnableWindow(FALSE);
        m_buttonSet.EnableWindow(FALSE);
        m_editTotalKeyFrameNum.EnableWindow(FALSE);
        m_editKeyFrameInterval.EnableWindow(FALSE);

        m_editTransX.EnableWindow(FALSE);
        m_editTransY.EnableWindow(FALSE);
        m_editTransZ.EnableWindow(FALSE);

        KGSFXSoundPanel* pSoundPanel = 
            (KGSFXSoundPanel*)pFrame->GetPanelView()->GetSoundPage();
        pSoundPanel->UpdateUI();
    }
    else
    {
        HICON hIcon = LoadIcon(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_ICON_PLAY));
        m_checkPlay.SetIcon(hIcon);

        m_Slider.EnableWindow();

        KillTimer(SFX_PLAY_TIMER);
		
        pSFX->PlayAnimation(SFX_AP_PAUSE, 1.f, (int)pSFX->GetCurrentFrame() + 1);
		
	//	pSFX->SetMeteorSampling(FALSE);
		
        //if (pScene->m_pRefrenceModel)
        //{
        //    if ((pScene->m_pRefrenceModel->GetType() == MESHTYPE_SFX))
        //        ((KG3DSFX*)pScene->m_pRefrenceModel)->PlayAnimation(SFX_AP_PAUSE);
        //    else if ((pScene->m_pRefrenceModel->GetAnimationController(ANICTL_PRIMARY)()))
        //        pScene->m_pRefrenceModel->GetAnimationController(ANICTL_PRIMARY)()->PauseAnimation(TRUE);
        //}
        pScene->RefModelsPlaySFX(SFX_AP_PAUSE, TRUE);


        m_buttonMoveEnd.EnableWindow();
        m_buttonMoveStart.EnableWindow();
        m_buttonMoveLeft.EnableWindow();
        m_buttonMoveRight.EnableWindow();

        m_checkAutoKey.EnableWindow();
        m_buttonSet.EnableWindow();
        m_editTotalKeyFrameNum.EnableWindow();
        m_editKeyFrameInterval.EnableWindow();

        m_editTransX.EnableWindow();
        m_editTransY.EnableWindow();
        m_editTransZ.EnableWindow();

        KGSFXSoundPanel* pSoundPanel = 
            (KGSFXSoundPanel*)pFrame->GetPanelView()->GetSoundPage();
        pSoundPanel->UpdateUI();
    }



//Exit0:
    return;
}

void KSceneSFXEditorKeyFrameDlg::OnBnClickedButtonMoveLeft()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();

    int nMin, nMax;
    m_Slider.GetRange(nMin, nMax);
    int nPos = m_Slider.GetCurPos();
    --nPos;
    if (nPos < nMin)    nPos = nMin;

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    pSFX->SetCurrentFrame((float)nPos);
   /* if (pScene->m_pRefrenceModel && pScene->m_pRefrenceModel->GetAnimationController(ANICTL_PRIMARY)())
        pScene->m_pRefrenceModel->GetAnimationController(ANICTL_PRIMARY)()->SeekAnimation(
            ANIMATION_SEEK_SET, (float)nPos * pScene->m_lpSFX->GetFrameInterval()
        );*/
    pScene->RefModelsSeekAnimation(
        ANIMATION_SEEK_SET, (float)nPos * pSFX->GetFrameInterval()
    );

    m_Slider.SetPos(nPos);
    m_Slider.Invalidate();
    m_Slider.UpdateWindow();
    UpdateCurFrameUI();
    pSFX->TransformationFrameMove();
    UpdateTransformUI();

    KSceneSFXEditorParticlePage *pParticlePage = (KSceneSFXEditorParticlePage*)pFrame->GetPanelView()->GetParticlePage();
    pParticlePage->UpdateParticleKindUI();
    pParticlePage->UpdateLauncherUI();
    pParticlePage->UpdateForceFieldUI();
}

void KSceneSFXEditorKeyFrameDlg::OnBnClickedButtonMoveStart()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();

    int nMin, nMax;
    m_Slider.GetRange(nMin, nMax);

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    pSFX->SetCurrentFrame((float)nMin);
   /* if (pScene->m_pRefrenceModel && pScene->m_pRefrenceModel->GetAnimationController(ANICTL_PRIMARY)())
        pScene->m_pRefrenceModel->GetAnimationController(ANICTL_PRIMARY)()->SeekAnimation(
        ANIMATION_SEEK_SET, (float)nMin * pScene->m_lpSFX->GetFrameInterval()
    );*/
    pScene->RefModelsSeekAnimation(
        ANIMATION_SEEK_SET, (float)nMin * pSFX->GetFrameInterval()
    );

    m_Slider.SetPos(nMin);
    m_Slider.Invalidate();
    m_Slider.UpdateWindow();
    UpdateCurFrameUI();
    pSFX->TransformationFrameMove();
    UpdateTransformUI();

    KSceneSFXEditorParticlePage *pParticlePage = (KSceneSFXEditorParticlePage*)pFrame->GetPanelView()->GetParticlePage();
    pParticlePage->UpdateParticleKindUI();
    pParticlePage->UpdateLauncherUI();
    pParticlePage->UpdateForceFieldUI();
}

void KSceneSFXEditorKeyFrameDlg::OnBnClickedButtonMoveRight()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();

    int nMin, nMax;
    m_Slider.GetRange(nMin, nMax);
    int nPos = m_Slider.GetCurPos();
    ++nPos;
    if (nPos > nMax)    nPos = nMax;

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    pSFX->SetCurrentFrame((float)nPos);
    /*if (pScene->m_pRefrenceModel && pScene->m_pRefrenceModel->GetAnimationController(ANICTL_PRIMARY)())
        pScene->m_pRefrenceModel->GetAnimationController(ANICTL_PRIMARY)()->SeekAnimation(
        ANIMATION_SEEK_SET, (float)nPos * pScene->m_lpSFX->GetFrameInterval()
    );*/
    pScene->RefModelsSeekAnimation(
        ANIMATION_SEEK_SET, (float)nPos * pSFX->GetFrameInterval()
    );


    m_Slider.SetPos(nPos);
    m_Slider.Invalidate();
    m_Slider.UpdateWindow();
    UpdateCurFrameUI();
    pSFX->TransformationFrameMove();
    UpdateTransformUI();

    KSceneSFXEditorParticlePage *pParticlePage = (KSceneSFXEditorParticlePage*)pFrame->GetPanelView()->GetParticlePage();
    pParticlePage->UpdateParticleKindUI();
    pParticlePage->UpdateLauncherUI();
    pParticlePage->UpdateForceFieldUI();
}

void KSceneSFXEditorKeyFrameDlg::OnBnClickedButtonMoveEnd()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();

    int nMin, nMax;
    m_Slider.GetRange(nMin, nMax);

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    pSFX->SetCurrentFrame((float)nMax);
   /* if (pScene->m_pRefrenceModel && pScene->m_pRefrenceModel->GetAnimationController(ANICTL_PRIMARY)())
        pScene->m_pRefrenceModel->GetAnimationController(ANICTL_PRIMARY)()->SeekAnimation(
        ANIMATION_SEEK_SET, (float)nMax * pScene->m_lpSFX->GetFrameInterval()
    );*/
    pScene->RefModelsSeekAnimation(
        ANIMATION_SEEK_SET, (float)nMax * pSFX->GetFrameInterval()
    );

    m_Slider.SetPos(nMax);
    m_Slider.Invalidate();
    m_Slider.UpdateWindow();
    UpdateCurFrameUI();
    pSFX->TransformationFrameMove();
    UpdateTransformUI();

    KSceneSFXEditorParticlePage *pParticlePage = (KSceneSFXEditorParticlePage*)pFrame->GetPanelView()->GetParticlePage();
    pParticlePage->UpdateParticleKindUI();
    pParticlePage->UpdateLauncherUI();
    pParticlePage->UpdateForceFieldUI();
}

void KSceneSFXEditorKeyFrameDlg::OnBnClickedCheckAutokey()
{
    UpdateData();
    if (m_bAutoKey)
        m_bSetKey = 0;
    else
        m_bSetKey = 1;
    UpdateData(FALSE);
}

void KSceneSFXEditorKeyFrameDlg::OnBnClickedCheckSetkey()
{
    UpdateData();
    if (m_bSetKey)
        m_bAutoKey = 0;
    else
        m_bAutoKey = 1;
    UpdateData(FALSE);
}



void KSceneSFXEditorKeyFrameDlg::OnSetTransformation(BOOL bForce)
{
    HRESULT hRetCode = E_FAIL;
    SCENEENTITYTYPE EntityType = SCENEENTITY_NONE;
    VOID *pvEntityPointer = NULL;

    GET_SFX_EDITOR();

    if (!bForce && !m_bAutoKey)
        return;

    IEKG3DSceneEditorBase *pSceneEditorBase = NULL;
    pScene->GetSceneEditBase(&pSceneEditorBase);
    if (!pSceneEditorBase)
        return;

    int nPos = m_Slider.GetCurPos();
	GETSELECT(pSceneEditorBase);

    DWORD dwSetFlag = 0;

    if (bForce)
    {
        if (GetEditState(pSceneEditorBase) == SCENESTATE_MOVE)
            dwSetFlag = SET_TRANS;
        else if (GetEditState(pSceneEditorBase) == SCENESTATE_ROTATE)
            dwSetFlag = SET_ROTAN;
        else if (GetEditState(pSceneEditorBase) == SCENESTATE_SCALE)
            dwSetFlag = SET_SCALG;
        else 
            dwSetFlag = SET_TRANS | SET_ROTAN | SET_SCALG;
    }

	IEKG3DSceneSelectionToolAccessProxy* pProxy = KSF::GetSelectionProxy(pScene);
	if(NULL == pProxy)
		return;

	//for (INT i = 0; i < pSelector->GetSelectedEntitySize(); i ++)
	for(IEKG3DSceneEntity* piEntity = pProxy->Reset(); NULL != piEntity; piEntity = pProxy->Next())
    {
        /*IEKG3DSceneEntity *piEntity = NULL;
		pSelector->GetSelectedEntity(i, &piEntity);*/

        hRetCode = piEntity->GetType(&EntityType);
        KGLOG_COM_PROCESS_ERROR(hRetCode);

        hRetCode = piEntity->GetPointer(&pvEntityPointer);
        KGLOG_COM_PROCESS_ERROR(hRetCode);

        if (EntityType == SCENEENTITY_SFX_LAUNCHER)
        {
            IEKG3DSFXParticleLauncher *pLauncher = (IEKG3DSFXParticleLauncher*)pvEntityPointer;
            pLauncher->InsertRtsKeyFrame(nPos, dwSetFlag);

        }
        else if (EntityType == SCENEENTITY_SFX_BILLBOARD)
        {
            IEKG3DSFXBillboard *pBillboard = (IEKG3DSFXBillboard*)pvEntityPointer;
            pBillboard->InsertRtsKeyFrame(nPos, dwSetFlag);
        }
        else if (EntityType == SCENEENTITY_SFX_METEOR)
        {
            IEKG3DSFXMeteor* pMeteor = (IEKG3DSFXMeteor*)pvEntityPointer;
            pMeteor->InsertRtsKeyFrame(nPos, dwSetFlag);
        }
        else
        {
            pScene->OnEntityTransformation(piEntity, nPos);
        }

       // i++;
    }

    UpdateSliderCtrl();
Exit0:
    return;
}

void KSceneSFXEditorKeyFrameDlg::OnBnClickedButtonSetKeyframe()
{
    // TODO: Add your control notification handler code here
    OnSetTransformation(TRUE);
    UpdateSliderCtrl();
}

void KSceneSFXEditorKeyFrameDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: Add your message handler code here and/or call default
    GET_SFX_EDITOR();

    if (nIDEvent == SFX_PLAY_TIMER)
    {
        IEKG3DSFX *pSFX = NULL;
        pScene->GetSFX(&pSFX);

        float fCurrentFrame = pSFX->GetCurrentFrame();
        m_Slider.SetPos((int)(fCurrentFrame + 0.5f));
        //m_Slider.Invalidate();
        //m_Slider.UpdateWindow();

        UpdateTransformUI();
        UpdateCurFrameUI(true);

        //CString str;
        //str.Format("%d", pScene->m_lpSFX->m_ParticleContainer.m_vecRectangleParticle.size());
        //m_staticParticleNum.SetWindowText(str);

        if (!m_bLoop && fCurrentFrame == pSFX->GetTotalFrameNum())
        {

            pSFX->SetCurrentFrame(0);
            m_Slider.SetPos(0);
            m_checkPlay.SetCheck(FALSE);
            OnBnClickedButtonPlaySfx();
        }
    }

    CDialog::OnTimer(nIDEvent);
}

template<class T>
void KSceneSFXEditorKeyFrameDlg::OperateTics(int *pTics, int nCnt, int nOffset, int nOperate)
{
    ASSERT(pTics);
    if (!nCnt)
        return;

    TKG3DTimeLine<T>* pTimeLine = (TKG3DTimeLine<T>*)m_pTimeLine;
    vector<T>   data;
    int i;
    for (i = 0; i < nCnt; ++i)
    {
        T val;
        pTimeLine->GetData(&val, (float)pTics[i]);
        data.push_back(val);
        if (nOperate != SFX_SLIDER_TICS_COPY)
            pTimeLine->RemoveKeyFrame(pTics[i]);
    }

    if (nOperate != SFX_SLIDER_TICS_DEL)
    {
        for (i = 0; i < nCnt; ++i)
        {
            pTimeLine->InsertKeyFrame(pTics[i] + nOffset, data[i]);
        }
    }
}

LRESULT KSceneSFXEditorKeyFrameDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    // TODO: Add your specialized code here and/or call the base class
    switch (message)
    {
    case WM_SFX_SLIDER_OPERATE_TICS:
        {
            SLIDER_OPERATE_TICS* t = (SLIDER_OPERATE_TICS*)wParam;
            switch (m_eTimeLineDataType)
            {
            case SFX_TLDT_INT:
                OperateTics<int>(t->pTicsList, t->nCnt, t->nOffset, t->nOperate);
                break;
            case SFX_TLDT_FLOAT:
                OperateTics<float>(t->pTicsList, t->nCnt, t->nOffset, t->nOperate);
                break;
            case SFX_TLDT_D3DXCOLOR:
                OperateTics<D3DXCOLOR>(t->pTicsList, t->nCnt, t->nOffset, t->nOperate);
                break;
            case SFX_TLDT_VECTOR3:
                OperateTics<D3DXVECTOR3>(t->pTicsList, t->nCnt, t->nOffset, t->nOperate);
                break;
            case SFX_TLDT_VECTOR2:
                OperateTics<D3DXVECTOR2>(t->pTicsList, t->nCnt, t->nOffset, t->nOperate);
                break;
            case SFX_TLDT_VECTOR4:
                OperateTics<D3DXQUATERNION>(t->pTicsList, t->nCnt, t->nOffset, t->nOperate);
                break;
            case SFX_TLDT_VECTOR5:
                OperateTics<FLOAT5>(t->pTicsList, t->nCnt, t->nOffset, t->nOperate);
                break;
            case SFX_TLDT_SOUND :
                OperateTics<_SoundInfo>(t->pTicsList, t->nCnt, t->nOffset, t->nOperate);
                break;
            default:
                ASSERT(0);
            }
            UpdateSliderCtrl();
        }
        break;

    case WM_EDIT_RECEIVE_ENTER:
        {
            switch (wParam)
            {
            case IDC_EDIT_FRAME_INTERVAL:
                OnSetFrameInterval();
                break;
            case IDC_EDIT_TOTAL_FRAME_NUM:
                OnSetTotalFrameNum();
                break;
            case IDC_EDIT_TRANS_X:
                OnSetTransX();
                break;
            case IDC_EDIT_TRANS_Y:
                OnSetTransY();
                break;
            case IDC_EDIT_TRANS_Z:
                OnSetTransZ();
                break;
            default:
                break;
            }
        }
        break;

    default:
        break;
    }

    return CDialog::WindowProc(message, wParam, lParam);
}

int KSceneSFXEditorKeyFrameDlg::SetDependWnd(CView* pDependWnd)
{
    ASSERT(pDependWnd);
    m_pDependWnd = pDependWnd;
    return TRUE;
}

DWORD KSceneSFXEditorKeyFrameDlg::UpdateWndState()
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
        MoveWindow(&(pEditView->GetPaneRect(WND_PLAY_PANE)));
    }
    pEditView->m_paneSFXMainWnd.UpdateWndState();
Exit0:
    return m_dwWndState;
}

void KSceneSFXEditorKeyFrameDlg::OnOK()
{
    // TODO: Add your specialized code here and/or call the base class
}

void KSceneSFXEditorKeyFrameDlg::OnBnClickedLoop()
{
   m_bLoop = !m_bLoop;
   HICON hIconLoop = LoadIcon(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_ICON_LOOP));
   HICON hIconOne  = LoadIcon(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_ICON_ONE));
   if (m_bLoop)
       m_buttonLoopOrOne.SetIcon(hIconLoop);
   else
       m_buttonLoopOrOne.SetIcon(hIconOne);
   OnBnClickedButtonPlaySfx();
}

void KSceneSFXEditorKeyFrameDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
      if (m_dwWndState == SFX_WND_STATE_FREE)
        m_dwWndState =  SFX_WND_STATE_BIND;
    else
        m_dwWndState = SFX_WND_STATE_FREE;
    UpdateWndState();
    CDialog::OnLButtonDblClk(nFlags, point);
}


void KSceneSFXEditorKeyFrameDlg::OnEnChangeEditTransY()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
}

void KSceneSFXEditorKeyFrameDlg::OnEnChangeEditTransX()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
}

void KSceneSFXEditorKeyFrameDlg::OnCbnSetfocusAni()
{
    FillComb();
}
