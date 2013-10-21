// KSceneSceneEditorDialogCamera.cpp : implementation file
//
#include "stdafx.h"
#include "sceneeditor.h"
#include "KSceneSceneEditorDialogCamera.h"
#include ".\kscenesceneeditordialogcamera.h"
//#include "KG3DSceneSceneEditor.h"
#include "IEEditor.h" //by dengzhihui Refactor 2006年11月28日
#include "IEKG3DSceneCamera.h"//by dengzhihui Refactor 2006年11月28日
#include "KSceneSceneHelpers.h"
//#include "KSceneInterfaceGetters.h"
#include "KSceneCommonParamDialog.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KSceneSceneEditorDialogCamera dialog

KSceneSceneEditorDialogCamera* KSceneSceneEditorDialogCamera::m_stpSSEDialogCamera = NULL;

KSceneSceneEditorDialogCamera::KSceneSceneEditorDialogCamera(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneSceneEditorDialogCamera::IDD, pParent)
	, m_bCheckEditKeyFrame(false)
	, m_fPlaySpeed(0)
	, m_fAngle(0)
{
	//{{AFX_DATA_INIT(KSceneSceneEditorDialogCamera)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_lpSceneSceneEditor = NULL;
	m_fZFar = 4200;
	m_fPlaySpeed = 1;
}


void KSceneSceneEditorDialogCamera::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KSceneSceneEditorDialogCamera)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_EDIT_ZFAR, m_fZFar);
	DDX_Control(pDX, IDC_COMBO_CAMANI_LIST, m_ctlComboCamAniList);
	DDX_Control(pDX, IDC_BUTTON_NEW_ANI, m_BnNewAni);
	DDX_Control(pDX, IDC_CHECK_EDITKEYFRAME, m_ckEnableEditKeyFrame);
	DDX_Text(pDX, IDC_ED_PLAYSPEED, m_fPlaySpeed);
	DDX_Text(pDX, IDC_ED_PLAYANGLE, m_fAngle);
	DDV_MinMaxFloat(pDX, m_fPlaySpeed, 0.0001F, 1000);
	//DDV_MinMaxFloat(pDX, m_fAngle, 0.0001F, 360);
}


BEGIN_MESSAGE_MAP(KSceneSceneEditorDialogCamera, CDialog)
	//{{AFX_MSG_MAP(KSceneSceneEditorDialogCamera)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_EDIT_ZFAR, OnEnChangeEditZfar)
	ON_BN_CLICKED(IDC_CHECK_EDITKEYFRAME, OnBnClickedCheckEditkeyframe)
//	ON_BN_CLICKED(IDC_BUTTON_AUTOTANGET, OnBnClickedButtonAutotanget)
	ON_BN_CLICKED(IDC_BUTTON_CAMANI_CLR_ALL_KEY, OnBnClickedButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_PLAY_SEL_ANI, &KSceneSceneEditorDialogCamera::OnBnClickedButtonPlaySelAni)
	ON_BN_CLICKED(IDC_BUTTON_STOP_SEL_ANI, &KSceneSceneEditorDialogCamera::OnBnClickedButtonStopSelAni)
//	ON_BN_CLICKED(IDC_BUTTON_ADD_KEY, &KSceneSceneEditorDialogCamera::OnBnClickedButtonAddKey)
	ON_BN_CLICKED(IDC_BUTTON_NEW_ANI, &KSceneSceneEditorDialogCamera::OnBnClickedButtonNewAni)
	ON_BN_CLICKED(IDC_BUTTON_RENAME_SEL_ANI, &KSceneSceneEditorDialogCamera::OnBnClickedButtonRenameSelAni)
	ON_CBN_SELCHANGE(IDC_COMBO_CAMANI_LIST, &KSceneSceneEditorDialogCamera::OnCbnSelchangeComboCamaniList)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_SEL_ANI, &KSceneSceneEditorDialogCamera::OnBnClickedButtonClearSelAni)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_ALL_ANI, &KSceneSceneEditorDialogCamera::OnBnClickedButtonClearAllAni)
	ON_BN_CLICKED(IDC_BUTTON_ANI_UP, &KSceneSceneEditorDialogCamera::OnBnClickedButtonAniUp)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_ANI_DOWN, &KSceneSceneEditorDialogCamera::OnBnClickedButtonClearAniDown)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_RECENT_KEY, &KSceneSceneEditorDialogCamera::OnBnClickedButtonClearRecentKey)
	ON_BN_CLICKED(IDC_BUTTON_ADD_KEY, &KSceneSceneEditorDialogCamera::OnBnClickedButtonAddKey)
	ON_BN_CLICKED(IDC_BUTTON_CAMANI_AUTOTANGET, &KSceneSceneEditorDialogCamera::OnBnClickedButtonAutotanget)
	ON_BN_CLICKED(IDC_BUTTON_CAMANI_ADD_KEY_TO_HEAD, &KSceneSceneEditorDialogCamera::OnBnClickedButtonAddKeyToHead)
	ON_BN_CLICKED(IDC_BUTTON_CAMANI_PAUSE, &KSceneSceneEditorDialogCamera::OnBnClickedButtonCamaniPause)
	ON_BN_CLICKED(IDC_BUTTON1, &KSceneSceneEditorDialogCamera::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BN_CIRCLE, &KSceneSceneEditorDialogCamera::OnBnClickedButtonCircle)
	ON_WM_SHOWWINDOW()
	ON_EN_CHANGE(IDC_ED_PLAYSPEED, &KSceneSceneEditorDialogCamera::OnEnChangeEdPlayspeed)
	ON_EN_CHANGE(IDC_ED_PLAYANGLE, &KSceneSceneEditorDialogCamera::OnEnChangeEdPlayAngle)	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KSceneSceneEditorDialogCamera message handlers

void KSceneSceneEditorDialogCamera::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	//if(!m_lpSceneSceneEditor)
	//	return;
	//m_lpSceneSceneEditor->SetZFar(1000+nPos*100.0F);//m_fZFar = 1000+nPos*100.0F;
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void KSceneSceneEditorDialogCamera::OnEnChangeEditZfar()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	if(!m_lpSceneSceneEditor)
		return;
	if(m_fZFar > 400000)
	{
		m_fZFar = 400000;
		UpdateData(FALSE);
	}
	m_lpSceneSceneEditor->SetZFar(m_fZFar);//m_fZFar = m_fZFar;
}

void KSceneSceneEditorDialogCamera::OnBnClickedCheckEditkeyframe()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	BOOL bEdit = IsDlgButtonChecked(IDC_CHECK_EDITKEYFRAME);
	
	IEKG3DSceneCameraMovement* pCamMov = KSH::GetEngineInterfaceEx<IEKG3DSceneCameraMovement>();
	if(NULL == pCamMov)
		return;

	IEKG3DSceneCameraAnimation* pCamAni = NULL;
	HRESULT hr = pCamMov->GetCurCamAnimation(&pCamAni);
	if(FAILED(hr))
		return;

	hr = pCamAni->EnableEditCamAniKeyFrame(bEdit);

	if (FAILED(hr))
		CheckDlgButton(IDC_CHECK_EDITKEYFRAME, bEdit ? BST_UNCHECKED : BST_UNCHECKED);
}

void KSceneSceneEditorDialogCamera::OnOK()
{

}

#define GETCAMMOV() \
	IEKG3DScene* pBase1 = NULL;\
	m_lpSceneSceneEditor->GetScene(&pBase1); \
	_ASSERTE(pBase1);\
	IEKG3DSceneCameraMovement* pCamMov = NULL; \
	if (pBase1) \
		pBase1->GetCameraMovement(&pCamMov); \
	_ASSERTE(pCamMov); \
	if (!pBase1 || !pCamMov) \
		return 
void KSceneSceneEditorDialogCamera::OnBnClickedButtonClear()
{
	///判断代码
	if(!m_lpSceneSceneEditor)
	{
		return;
	}
	
	//KG3DSceneCameraMovement& pCamMov = &m_lpSceneSceneEditor->m_CameraMovement;
	GETCAMMOV();
    IEKG3DSceneCameraAnimation *piCameraAnimation = NULL;
    pCamMov->GetCurCamAnimation(&piCameraAnimation);
	
    BOOL bIsPlaying = FALSE;
    piCameraAnimation->GetPlayState(&bIsPlaying);
    if (bIsPlaying)
	{
		MessageBox(_T("正在播放摄像机动画，请先停止动画再进行这个操作"), _T("提示"), MB_OK|MB_ICONINFORMATION);
		return;
	}
    BOOL bIsKeyFrameEditable = FALSE;
    piCameraAnimation->IsKeyFrameEditable(&bIsKeyFrameEditable);
	if (bIsKeyFrameEditable)
	{
		MessageBox(_T("正在允许编辑摄像机动画关键帧的状态，请先禁止关键帧编辑再进行这个操作"), _T("提示"), MB_OK|MB_ICONINFORMATION);
		return;
	}

	///有用的代码
	piCameraAnimation->Clear();
}

void KSceneSceneEditorDialogCamera::OnBnClickedButtonPlaySelAni()
{
	// TODO: Add your control notification handler code here
	///防错代码
	if(!m_lpSceneSceneEditor)
	{
		return;
	}
	//KG3DSceneCameraMovement& CamMov = m_lpSceneSceneEditor->m_CameraMovement;
	GETCAMMOV();
    IEKG3DSceneCameraAnimation *piCameraAnimation = NULL;
    pCamMov->GetCurCamAnimation(&piCameraAnimation);
    DWORD dwKeyFrameCount = 0;
    piCameraAnimation->GetKeyFrameCount(&dwKeyFrameCount); 
	if (dwKeyFrameCount < 2 )
	{
		MessageBox(_T("关键帧太少了，起码两个才能形成正常的播放"), _T("提示"), MB_OK|MB_ICONINFORMATION);
		return;
	}

	///注意下面代码的顺序
    //m_lpSceneSceneEditor->m_CameraMovement.GetCurCamAnimation(&piCameraAnimation);
	pCamMov->GetCurCamAnimation(&piCameraAnimation);
	piCameraAnimation->StartPlay(TRUE);
	LockControlObjectsOnPlay( TRUE );	///锁定会引起误操作的按钮
	m_lpSceneSceneEditor->SetEditState(SCENESTATE_CAMERAANIMATION);//m_dwEditState = SCENESTATE_CAMERAANIMATION;
}

void KSceneSceneEditorDialogCamera::OnBnClickedButtonStopSelAni()
{
	// TODO: Add your control notification handler code here
	if(!m_lpSceneSceneEditor)
	{
		return;
	}
	
	///注意下面代码的顺序，正好是反的
	m_lpSceneSceneEditor->SetEditState(SCENESTATE_SELECT);//m_dwEditState = SCENESTATE_SELECT;
    IEKG3DSceneCameraAnimation *piCameraAnimation = NULL;
    //m_lpSceneSceneEditor->m_CameraMovement.GetCurCamAnimation(&piCameraAnimation);
	GETCAMMOV();
	pCamMov->GetCurCamAnimation(&piCameraAnimation);

	piCameraAnimation->StopPlay();
	LockControlObjectsOnPlay( FALSE );
}


/************************************************************************/
/*自定义函数
/************************************************************************/
HRESULT KSceneSceneEditorDialogCamera::SynWithCamAniList()
{
	if ( !m_lpSceneSceneEditor )
	{
		return E_FAIL;
	}
	
	//KG3DSceneCameraMovement& CamMov = m_lpSceneSceneEditor->m_CameraMovement;
	GETCAMMOV() E_FAIL;
    IEKG3DSceneCameraAnimation *piCameraAnimation = NULL;
    pCamMov->GetCurCamAnimation(&piCameraAnimation);
	
    BOOL bIsPlaying = FALSE;
    piCameraAnimation->GetPlayState(&bIsPlaying);
	ASSERT (false == bIsPlaying);

	///记录了当前CamAni选择项之后，清除Combo
	UpdateData( TRUE );
    DWORD dwCurCamAniIndex = 0;
    HRESULT hRetCode = E_FAIL;
    hRetCode = pCamMov->GetCurCamAniIndex(&dwCurCamAniIndex);
    if (FAILED(hRetCode))
    {
        ///重新对准选择项
        dwCurCamAniIndex = 0;
    }

	int nComboCount = m_ctlComboCamAniList.GetCount();
	for ( int i = nComboCount-1; i > -1 ; i-- )
	{
		m_ctlComboCamAniList.DeleteString( i );
	}
	ASSERT( 0 == m_ctlComboCamAniList.GetCount() );
	
	///重新加入CamAni
	
	TCHAR tczName[128];
	ZeroMemory(tczName, sizeof(tczName));
	HRESULT hr;

	///重新刷新Combo，注意Size应该动态得到
    DWORD dwAnimationCount = 0;
    pCamMov->GetAnimationCount(&dwAnimationCount);
	for (DWORD i = 0; i <dwAnimationCount; i++ )
	{
		hr = pCamMov->SetCurCamAnimation( i );
		if ( FAILED( hr ) )
		{
			return E_FAIL;
		}

        pCamMov->GetCurCamAnimation(&piCameraAnimation);
		piCameraAnimation->GetName(tczName, _countof(tczName));
		
		m_ctlComboCamAniList.AddString(tczName); 
	}

	hr = pCamMov->SetCurCamAnimation( dwCurCamAniIndex );
	if ( FAILED( hr ) )
	{
		return E_FAIL;		
	}
	m_ctlComboCamAniList.SetCurSel( dwCurCamAniIndex );	
	m_nSelIndex = m_ctlComboCamAniList.GetCurSel();	

	return S_OK;
}
HRESULT KSceneSceneEditorDialogCamera::SynWithCheckEditKeyFrame()
{
	if ( !m_lpSceneSceneEditor )
	{
		return E_FAIL;
	}
	
    IEKG3DSceneCameraAnimation *piCameraAnimation = NULL;
    //m_lpSceneSceneEditor->m_CameraMovement.GetCurCamAnimation(&piCameraAnimation);
	GETCAMMOV() E_FAIL;
	pCamMov->GetCurCamAnimation(&piCameraAnimation);

    BOOL bIsKeyFrameEditable = FALSE;
    piCameraAnimation->IsKeyFrameEditable(&bIsKeyFrameEditable);
    if (bIsKeyFrameEditable)
	{
		m_ckEnableEditKeyFrame.SetCheck( BST_CHECKED );		
	}
	else
	{
		m_ckEnableEditKeyFrame.SetCheck( BST_UNCHECKED );
	}
	return S_OK;	
}
BOOL KSceneSceneEditorDialogCamera::LockControlObjectsOnPlay( BOOL bDoLock )
{
	EnumChildWindows( this->GetSafeHwnd(), MyEnableWindow, (LPARAM)(!bDoLock) );
	return bDoLock;	
}
BOOL CALLBACK MyEnableWindow( HWND hWnd, LPARAM BOOL_LPARAM )	///自定义回调函数，用于控件遍历，禁止控件
{
	int ID = GetDlgCtrlID( hWnd );
	BOOL BoolOption = (BOOL)BOOL_LPARAM;
	if ( IDC_BUTTON_PLAY_SEL_ANI != ID && IDC_BUTTON_STOP_SEL_ANI != ID && IDC_BUTTON_CAMANI_PAUSE != ID)
	{
		::EnableWindow( hWnd, BoolOption );
	}
	return TRUE;
}
BOOL KSceneSceneEditorDialogCamera::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	
	SynWithCamAniList();
	if(m_lpSceneSceneEditor)
	{
		m_fZFar = m_lpSceneSceneEditor->GetZFar();
		UpdateData(FALSE);
	}
	OnCbnSelchangeComboCamaniList();	//第一次要更新数据
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneSceneEditorDialogCamera::OnBnClickedButtonNewAni()
{
	// TODO: Add your control notification handler code here
	///防错代码
	if ( !m_lpSceneSceneEditor )
	{
		return;
	}

	//KG3DSceneCameraMovement& CamMov = m_lpSceneSceneEditor->m_CameraMovement; 
	GETCAMMOV();
    IEKG3DSceneCameraAnimation *piCameraAnimation = NULL;
    pCamMov->GetCurCamAnimation(&piCameraAnimation);
	
    BOOL bIsPlaying = FALSE;
    piCameraAnimation->GetPlayState(&bIsPlaying);
    if (bIsPlaying)
	{
		MessageBox(_T("正在播放摄像机动画，请先停止动画再进行这个操作"), _T("提示"), MB_OK|MB_ICONINFORMATION);
		return;
	}

	
	HRESULT hr = pCamMov->NewOneCamAnimation();
	ASSERT( SUCCEEDED( hr ) );
	hr = pCamMov->GetCurCamAnimation(&piCameraAnimation);

	if (SUCCEEDED(hr))
	{
		TCHAR	tczName[128];
		piCameraAnimation->GetName(tczName, _countof(tczName));
		m_ctlComboCamAniList.AddString( tczName );
		m_ctlComboCamAniList.SetCurSel( m_ctlComboCamAniList.GetCount()-1 );
		m_nSelIndex = m_ctlComboCamAniList.GetCurSel();

		ASSERT( m_nSelIndex == m_ctlComboCamAniList.GetCount()-1 );

		SynWithCheckEditKeyFrame();
	}
}

void KSceneSceneEditorDialogCamera::OnBnClickedButtonRenameSelAni()
{
	// TODO: Add your control notification handler code here
	///防错代码
	if ( !m_lpSceneSceneEditor )
	{
		return;
	}
	
	//KG3DSceneCameraMovement& CamMov = m_lpSceneSceneEditor->m_CameraMovement;
	GETCAMMOV();
    IEKG3DSceneCameraAnimation *piCameraAnimation = NULL;
    pCamMov->GetCurCamAnimation(&piCameraAnimation);
	
    BOOL bIsPlaying = FALSE;
    piCameraAnimation->GetPlayState(&bIsPlaying);
    if (bIsPlaying)
	{
		MessageBox(_T("正在播放摄像机动画，请先停止动画再进行这个操作"), _T("提示"), MB_OK|MB_ICONINFORMATION);
		m_ctlComboCamAniList.SetCurSel( m_nSelIndex );
		return;
	}

	///有用的代码
	CString csComboText;
	HRESULT hr;
	UpdateData( TRUE );
	m_ctlComboCamAniList.GetWindowText( csComboText );
	if ( strlen( (LPCTSTR)csComboText ) <= 2 )
	{
		MessageBox(_T("命名长度不应该小于2，加长点吧"), _T("提示"), MB_OK|MB_ICONWARNING );
		goto EXIT_CODE;
	}
	
    DWORD dwAnimationCount = 0;
    pCamMov->GetAnimationCount(&dwAnimationCount);

    ASSERT( m_nSelIndex >-1 && m_nSelIndex < m_ctlComboCamAniList.GetCount());
	ASSERT( (DWORD)m_nSelIndex < dwAnimationCount );
	
	hr = pCamMov->SetCurCamAniName( (LPCTSTR)csComboText );
	if ( SUCCEEDED( hr ) )
	{
		m_ctlComboCamAniList.DeleteString( m_nSelIndex );
		m_ctlComboCamAniList.InsertString( m_nSelIndex, (LPCTSTR)csComboText );		
	}
	else
	{
		///如果重名的话，SetCurCamAniName方法会捕捉到，并返回E_FAIL
		MessageBox( _T("重命名不成功，可能因为有重名"), _T("提示"), MB_OK | MB_ICONINFORMATION );
	}
EXIT_CODE:
	m_ctlComboCamAniList.SetCurSel( m_nSelIndex );
	m_ctlComboCamAniList.SetFocus();
	
	ASSERT( m_nSelIndex == m_ctlComboCamAniList.GetCurSel() );
}

void KSceneSceneEditorDialogCamera::OnCbnSelchangeComboCamaniList()
{
	// TODO: Add your control notification handler code here
	///防错代码
	if ( !m_lpSceneSceneEditor )
	{
		return;
	}
	//KG3DSceneCameraMovement& CamMov = m_lpSceneSceneEditor->m_CameraMovement;
	GETCAMMOV();
    IEKG3DSceneCameraAnimation *piCameraAnimation = NULL;
    pCamMov->GetCurCamAnimation(&piCameraAnimation);
	
    BOOL bIsPlaying = FALSE;
    piCameraAnimation->GetPlayState(&bIsPlaying);
    if (bIsPlaying)
	{
		MessageBox(_T("正在播放摄像机动画，请先停止动画再进行这个操作"), _T("提示"), MB_OK|MB_ICONINFORMATION);
		m_ctlComboCamAniList.SetCurSel( m_nSelIndex );
		return;
	}

	///有用代码
	m_nSelIndex = m_ctlComboCamAniList.GetCurSel();
	
    DWORD dwAnimationCount = 0;
    pCamMov->GetAnimationCount(&dwAnimationCount);
	ASSERT( m_nSelIndex > -1 && (DWORD)m_nSelIndex < dwAnimationCount );
	
	RegisterCallbacks(FALSE);

	pCamMov->SetCurCamAnimation( m_nSelIndex );

	RegisterCallbacks(TRUE);
	
	SynWithCheckEditKeyFrame();
	SynFromInnerToInterface();

	if(piCameraAnimation)
	{
		m_fPlaySpeed = piCameraAnimation->GetPlaySpeed();	//单位是线速度每毫秒
		//m_fPlaySpeed *= (float)(180.0F / D3DX_PI);	//线速度到角速度，单位是每毫秒角速度
		m_fAngle = piCameraAnimation->GetPlayAngle();
		UpdateData(FALSE);
	}
#ifdef _DEBUG	///保证选择正确
	CString csNameInCombo;
    pCamMov->GetCurCamAnimation(&piCameraAnimation);
	TCHAR tczName[128];
	piCameraAnimation->GetName(tczName, _countof(tczName));
	m_ctlComboCamAniList.GetLBText( m_nSelIndex, csNameInCombo );
	ASSERT( 0 == _tcscmp(tczName, csNameInCombo ) );
#endif
	
}

void KSceneSceneEditorDialogCamera::OnBnClickedButtonClearSelAni()
{
    BOOL bIsPlaying = FALSE;
	// TODO: Add your control notification handler code here
	///防错代码
	if ( !m_lpSceneSceneEditor )
	{
		return;
	}
	
	//KG3DSceneCameraMovement& CamMov = m_lpSceneSceneEditor->m_CameraMovement; 
	GETCAMMOV();
    IEKG3DSceneCameraAnimation *piCameraAnimation = NULL;
    pCamMov->GetCurCamAnimation(&piCameraAnimation);
	
    piCameraAnimation->GetPlayState(&bIsPlaying);
	if (bIsPlaying)
	{
		MessageBox(_T("正在播放摄像机动画，请先停止动画再进行这个操作"), _T("提示"), MB_OK|MB_ICONINFORMATION);
		return;
	}
#ifdef _DEBUG
	int nCurSelIndex = m_ctlComboCamAniList.GetCurSel();
    DWORD dwCurCamAniIndex = 0;
    pCamMov->GetCurCamAniIndex(&dwCurCamAniIndex);

	ASSERT( (DWORD)nCurSelIndex == dwCurCamAniIndex );
#endif

	///有用的代码
	HRESULT hr = pCamMov->RemoveCurCamAnimation();
	
	if ( FAILED( hr ) )
	{
		return;
	}

	SynWithCamAniList();
	
	///重新设定选择焦点。
	m_nSelIndex = m_ctlComboCamAniList.GetCurSel();
	m_ctlComboCamAniList.SetFocus();
	
	SynWithCheckEditKeyFrame();
}

void KSceneSceneEditorDialogCamera::OnBnClickedButtonClearAllAni()
{
	// TODO: Add your control notification handler code here
	///防错代码
	if ( !m_lpSceneSceneEditor )
	{
		return;
	}
	
	//KG3DSceneCameraMovement& CamMov = m_lpSceneSceneEditor->m_CameraMovement;
	GETCAMMOV();
    IEKG3DSceneCameraAnimation *piCameraAnimation = NULL;
    pCamMov->GetCurCamAnimation(&piCameraAnimation);
	
    BOOL bIsPlaying = FALSE;
    piCameraAnimation->GetPlayState(&bIsPlaying);
    if (bIsPlaying)
	{
		MessageBox(_T("正在播放摄像机动画，请先停止动画再进行这个操作"), _T("提示"), MB_OK|MB_ICONINFORMATION);
		return;
	}

	///有用的代码
	int nUserChoice;
	
	nUserChoice = MessageBox( _T("确定要删除所有的动画吗？因为场景编辑器需要至少一个动画，所以即使已删除所有已存在的动画，也会创建一个临时的空动画"), _T(""), MB_OKCANCEL|MB_ICONWARNING );
	if ( nUserChoice != IDOK)
	{
		return;	///用户取消删除
	}
	
	HRESULT hr = pCamMov->Clear();
	///因为SceneSceneEditor是一直会取其中的CamAni的，所以不能为空，要主动New一个。
    DWORD dwAnimationCount = 0;
    pCamMov->GetAnimationCount(&dwAnimationCount);
    ASSERT( 0 == dwAnimationCount);
	ASSERT( SUCCEEDED(hr) );
	
	hr = pCamMov->NewOneCamAnimation();
	
	SynWithCamAniList();
	SynWithCheckEditKeyFrame();
	
	ASSERT( SUCCEEDED(hr) );
}

void KSceneSceneEditorDialogCamera::OnBnClickedButtonAniUp()
{
	// TODO: Add your control notification handler code here
	///防错代码
	if ( !m_lpSceneSceneEditor )
	{
		return;
	}
	
	//KG3DSceneCameraMovement& CamMov = m_lpSceneSceneEditor->m_CameraMovement;
	GETCAMMOV();
    IEKG3DSceneCameraAnimation *piCameraAnimation = NULL;
    pCamMov->GetCurCamAnimation(&piCameraAnimation);
	
    BOOL bIsPlaying = FALSE;
    piCameraAnimation->GetPlayState(&bIsPlaying);
    if (bIsPlaying)
	{
		MessageBox(_T("正在播放摄像机动画，请先停止动画再进行这个操作"), _T("提示"), MB_OK|MB_ICONINFORMATION);
		return;
	}
	
	///有用的代码
	HRESULT hr = pCamMov->LetCurCamAniPosMoveUp();
	
	if ( SUCCEEDED( hr ) )
	{
		SynWithCamAniList();
	}
	
	m_nSelIndex = m_ctlComboCamAniList.GetCurSel();
	m_ctlComboCamAniList.SetFocus();
}

void KSceneSceneEditorDialogCamera::OnBnClickedButtonClearAniDown()
{
	// TODO: Add your control notification handler code here
	///防错代码
	if ( !m_lpSceneSceneEditor )
	{
		return;
	}

	//KG3DSceneCameraMovement& CamMov = m_lpSceneSceneEditor->m_CameraMovement;
	GETCAMMOV();
    IEKG3DSceneCameraAnimation *piCameraAnimation = NULL;
    pCamMov->GetCurCamAnimation(&piCameraAnimation);

    BOOL bIsPlaying = FALSE;
    piCameraAnimation->GetPlayState(&bIsPlaying);
    if (bIsPlaying)
	{
		MessageBox(_T("正在播放摄像机动画，请先停止动画再进行这个操作"), _T("提示"), MB_OK|MB_ICONINFORMATION);
		return;
	}
	
	///有用的代码
	HRESULT hr = pCamMov->LetCurCamAniPosMoveDown();
	if ( SUCCEEDED( hr ) )
	{
		SynWithCamAniList();
	}

	m_nSelIndex = m_ctlComboCamAniList.GetCurSel();
	m_ctlComboCamAniList.SetFocus();
}

void KSceneSceneEditorDialogCamera::OnBnClickedButtonClearRecentKey()
{
	// TODO: Add your control notification handler code here
	///防错的代码
	if ( !m_lpSceneSceneEditor )
	{
		return;
	}
	
	//KG3DSceneCameraMovement& CamMov = m_lpSceneSceneEditor->m_CameraMovement;
	GETCAMMOV();
    IEKG3DSceneCameraAnimation *piCameraAnimation = NULL;
    pCamMov->GetCurCamAnimation(&piCameraAnimation);

    BOOL bIsPlaying = FALSE;
    piCameraAnimation->GetPlayState(&bIsPlaying);
    if (bIsPlaying)
	{
		MessageBox(_T("正在播放摄像机动画，请先停止动画再进行这个操作"), _T("提示"), MB_OK|MB_ICONINFORMATION);
		return;
	}
    BOOL bIsKeyFrameEditable = FALSE;
    piCameraAnimation->IsKeyFrameEditable(&bIsKeyFrameEditable);
    if (bIsKeyFrameEditable)
	{
		MessageBox(_T("正在允许编辑摄像机动画关键帧的状态，请先禁止关键帧编辑再进行这个操作"), _T("提示"), MB_OK|MB_ICONINFORMATION);
		return;
	}
	
	///有用的代码
	ASSERT ( false == bIsPlaying);
	piCameraAnimation->RemoveKeyFrame(0);
}



void KSceneSceneEditorDialogCamera::OnBnClickedButtonAddKey()
{
	// TODO: Add your control notification handler code here
	///防错的代码
	if ( !m_lpSceneSceneEditor )
	{
		return;
	}
	
	//KG3DSceneCameraMovement& CamMov = m_lpSceneSceneEditor->m_CameraMovement;
	GETCAMMOV();
    IEKG3DSceneCameraAnimation *piCameraAnimation = NULL;
    pCamMov->GetCurCamAnimation(&piCameraAnimation);
	
    BOOL bIsPlaying = FALSE;
    piCameraAnimation->GetPlayState(&bIsPlaying);
	if (bIsPlaying)
	{
		MessageBox(_T("正在播放摄像机动画，请先停止动画再进行这个操作"), _T("提示"), MB_OK|MB_ICONINFORMATION);
		return;
	}
    BOOL bIsKeyFrameEditable = FALSE;
    piCameraAnimation->IsKeyFrameEditable(&bIsKeyFrameEditable);
    if (bIsKeyFrameEditable)
	{
		MessageBox(_T("正在允许编辑摄像机动画关键帧的状态，请先禁止关键帧编辑再进行这个操作"), _T("提示"), MB_OK|MB_ICONINFORMATION);
		return;
	}

	///有用的代码
	ASSERT ( false == bIsPlaying);
    HRESULT hr = piCameraAnimation->AddKeyFrame();
	ASSERT( SUCCEEDED( hr ) );
}

void KSceneSceneEditorDialogCamera::OnBnClickedButtonAutotanget()
{
	// TODO: Add your control notification handler code here
	///防错代码
	if ( !m_lpSceneSceneEditor )
	{
		return;
	}
	
	//KG3DSceneCameraMovement& CamMov = m_lpSceneSceneEditor->m_CameraMovement;
	GETCAMMOV();
    IEKG3DSceneCameraAnimation *piCameraAnimation = NULL;
    pCamMov->GetCurCamAnimation(&piCameraAnimation);

    BOOL bIsPlaying = FALSE;
    piCameraAnimation->GetPlayState(&bIsPlaying);
	if (bIsPlaying)
	{
		MessageBox(_T("正在播放摄像机动画，请先停止动画再进行这个操作"), _T("提示"), MB_OK|MB_ICONINFORMATION);
		return;
	}
	
	///有用的代码
	ASSERT ( false == bIsPlaying );
	piCameraAnimation->ComputeTangent();
}
void KSceneSceneEditorDialogCamera::OnBnClickedButtonAddKeyToHead()
{
	///防错的代码
	if ( !m_lpSceneSceneEditor )
	{
		return;
	}

	//KG3DSceneCameraMovement& CamMov = m_lpSceneSceneEditor->m_CameraMovement;
	GETCAMMOV();
	IEKG3DSceneCameraAnimation *piCameraAnimation = NULL;
	pCamMov->GetCurCamAnimation(&piCameraAnimation);

	BOOL bIsPlaying = FALSE;
	piCameraAnimation->GetPlayState(&bIsPlaying);
	if (bIsPlaying)
	{
		MessageBox(_T("正在播放摄像机动画，请先停止动画再进行这个操作"), _T("提示"), MB_OK|MB_ICONINFORMATION);
		return;
	}
	BOOL bIsKeyFrameEditable = FALSE;
	piCameraAnimation->IsKeyFrameEditable(&bIsKeyFrameEditable);
	if (bIsKeyFrameEditable)
	{
		MessageBox(_T("正在允许编辑摄像机动画关键帧的状态，请先禁止关键帧编辑再进行这个操作"), _T("提示"), MB_OK|MB_ICONINFORMATION);
		return;
	}
	///干活的代码
	piCameraAnimation->AddKeyFrameToHeadFramePosition();
}
void KSceneSceneEditorDialogCamera::OnBnClickedButtonCamaniPause()
{
	// TODO: Add your control notification handler code here
	///防错代码
	if(!m_lpSceneSceneEditor)
	{
		return;
	}
	//KG3DSceneCameraMovement& CamMov = m_lpSceneSceneEditor->m_CameraMovement;
	GETCAMMOV();
	IEKG3DSceneCameraAnimation *piCameraAnimation = NULL;
	pCamMov->GetCurCamAnimation(&piCameraAnimation);
	
	BOOL bIsPlaying = FALSE;
	piCameraAnimation->GetPlayState(&bIsPlaying);
	if (!bIsPlaying)
	{
		MessageBox(_T("还没有开始播放摄像机动画啦"), _T("提示"), MB_OK|MB_ICONINFORMATION);
		return;
	}
	piCameraAnimation->PausePlay(NULL, NULL);
}

HRESULT KSceneSceneEditorDialogCamera::SynFromInnerToInterface( DWORD_PTR dwOption /*= 0*/ )
{
	IEKG3DSceneCameraAnimation* pCamAni = KSH::GetEngineInterface<KG3DTYPE_CAM_ANIMATION>();
	KG_PROCESS_ERROR(pCamAni);
	{
		

		IKG3DSceneCameraKeyFrameEx CamKey;
        DWORD dwPrevTime = 0, dwNextTime = 0;
		DWORD dwCurTime = 0;
		HRESULT hr = pCamAni->GetCurSelectedKey(&CamKey);
		KG_COM_PROCESS_ERROR(hr);
		hr = pCamAni->GetCurSelectedKeyTime(&dwCurTime, &dwPrevTime, &dwNextTime);
		KG_COM_PROCESS_ERROR(hr);
		KSH::ValueToWnd<UINT>(dwPrevTime, this->GetDlgItem(IDC_ED_PREV));
		KSH::ValueToWnd<UINT>(dwNextTime, this->GetDlgItem(IDC_ED_NEXT));
		KSH::ValueToWnd<UINT>(CamKey.dwTime, this->GetDlgItem(IDC_ED_TIME));
		KSH::ValueToWnd(CamKey.vPosition, this->GetDlgItem(IDC_ED_POS));
		KSH::ValueToWnd(CamKey.vLookAt, this->GetDlgItem(IDC_ED_LOOKATPOS));
		KSH::ValueToWnd(CamKey.vPositionTangent, this->GetDlgItem(IDC_ED_POSTAN));
		KSH::ValueToWnd(CamKey.vLookAtTangent, this->GetDlgItem(IDC_ED_LOOKATTAN));
	}
	return S_OK;
Exit0:
	return E_FAIL;
}
HRESULT KSceneSceneEditorDialogCamera::SynFromInterfaceToInner( DWORD_PTR dwOption /*= 0*/ )
{
	IEKG3DSceneCameraAnimation* pCamAni = KSH::GetEngineInterface<KG3DTYPE_CAM_ANIMATION>();
	KG_PROCESS_ERROR(pCamAni);
	{	
		DWORD dwPrevTime = 0, dwNextTime = 0;
		DWORD dwCurKeyTime = 0;
		HRESULT hr = pCamAni->GetCurSelectedKeyTime(&dwCurKeyTime, &dwPrevTime, &dwNextTime);
		KG_COM_PROCESS_ERROR(hr);
		IKG3DSceneCameraKeyFrameEx CamKey;
		hr = pCamAni->GetCurSelectedKey(&CamKey);
		KG_COM_PROCESS_ERROR(hr);
		
		
		KG_PROCESS_ERROR(KSH::WndToValue(this->GetDlgItem(IDC_ED_TIME), CamKey.dwTime));

		if (! ((dwPrevTime < 0 || CamKey.dwTime > dwPrevTime) && (dwNextTime < 0 || CamKey.dwTime < dwNextTime)))
		{
			MessageBox(_T("设定时间错误，只能在前一帧和后一帧之间设置时间"));
			goto Exit0;
		}

		KG_PROCESS_ERROR(KSH::WndToValue(this->GetDlgItem(IDC_ED_POS), CamKey.vPosition));
		KG_PROCESS_ERROR(KSH::WndToValue(this->GetDlgItem(IDC_ED_LOOKATPOS), CamKey.vLookAt));
		KG_PROCESS_ERROR(KSH::WndToValue(this->GetDlgItem(IDC_ED_POSTAN), CamKey.vPositionTangent));
		KG_PROCESS_ERROR(KSH::WndToValue(this->GetDlgItem(IDC_ED_LOOKATTAN), CamKey.vLookAtTangent));

		pCamAni->SetCurSelectedKey(&CamKey);
	}
	//this->SynFromInnerToInterface();
	return S_OK;
Exit0:
	//this->SynFromInnerToInterface();
	return E_FAIL;
}
void KSceneSceneEditorDialogCamera::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	SynFromInterfaceToInner();
	SynFromInterfaceToInner();
}

void KSceneSceneEditorDialogCamera::OnBnClickedButtonCircle()
{
	// TODO: Add your control notification handler code here
	///防错的代码
	if ( !m_lpSceneSceneEditor )
	{
		return;
	}

	//KG3DSceneCameraMovement& CamMov = m_lpSceneSceneEditor->m_CameraMovement;
	GETCAMMOV();
	IEKG3DSceneCameraAnimation *piCameraAnimation = NULL;
	pCamMov->GetCurCamAnimation(&piCameraAnimation);

	BOOL bIsPlaying = FALSE;
	piCameraAnimation->GetPlayState(&bIsPlaying);
	if (bIsPlaying)
	{
		MessageBox(_T("正在播放摄像机动画，请先停止动画再进行这个操作"), _T("提示"), MB_OK|MB_ICONINFORMATION);
		return;
	}
	BOOL bIsKeyFrameEditable = FALSE;
	piCameraAnimation->IsKeyFrameEditable(&bIsKeyFrameEditable);
	if (bIsKeyFrameEditable)
	{
		MessageBox(_T("正在允许编辑摄像机动画关键帧的状态，请先禁止关键帧编辑再进行这个操作"), _T("提示"), MB_OK|MB_ICONINFORMATION);
		return;
	}
	{
		KSceneCommonParamDialog Dlg;
		KG3DSceneCameraAnimationSPTrack_Struct Struct;
		INT nIndex1 = Dlg.AddAnParam(_T("半径"), Struct.m_fRadius,TRUE);
		KG_PROCESS_ERROR(nIndex1 >= 0);
		INT nIndex2 = Dlg.AddAnParam(_T("位置"), Struct.m_vPos,TRUE);
		KG_PROCESS_ERROR(nIndex2 >= 0);
		INT nIndex3 = Dlg.AddAnParam(_T("顺时针=1"), Struct.m_bClockWiseOrCounter,TRUE);
		KG_PROCESS_ERROR(nIndex3 >= 0);

		if (IDOK == Dlg.DoModal())
		{
			KG_PROCESS_ERROR(Dlg.GetData(nIndex1, Struct.m_fRadius));
			KG_PROCESS_ERROR(Dlg.GetData(nIndex2, Struct.m_vPos));
			KG_PROCESS_ERROR(Dlg.GetData(nIndex3, Struct.m_bClockWiseOrCounter));
			if (FAILED(piCameraAnimation->CreateSpecialTrack(emKG3DSceneCameraAnimationSpecialKeyTrack_Circle, reinterpret_cast<DWORD_PTR>(&Struct))))
			{
				MessageBox(_T("创建轨迹失败"));
			}
		}
	}
Exit0:
	return;
}
void KSceneSceneEditorDialogCamera::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: Add your message handler code here
	if (bShow)
	{
		RegisterCallbacks(TRUE);
	}
	else
	{
		RegisterCallbacks(FALSE);
	}

	if(m_lpSceneSceneEditor)
	{
		m_fZFar = m_lpSceneSceneEditor->GetZFar();
		UpdateData(FALSE);
	}

}
VOID CALLBACK KSceneSceneEditorDialogCamera::OnSelKeyChange(DWORD dwKeyFrameTime)
{
	if (m_stpSSEDialogCamera)
	{
		static DWORD dwOldNowTime = timeGetTime();
		DWORD dwNowTime = timeGetTime();
		if (dwNowTime - dwOldNowTime > 100)
		{
			dwOldNowTime = dwNowTime;
			m_stpSSEDialogCamera->SynFromInnerToInterface();
		}
	}
}

HRESULT KSceneSceneEditorDialogCamera::RegisterCallbacks( BOOL bRegisterOrUnRegister )
{
	IEKG3DSceneCameraAnimation* pCamAni = KSH::GetEngineInterface<KG3DTYPE_CAM_ANIMATION>();
	KG_PROCESS_ERROR (pCamAni);
	if(bRegisterOrUnRegister)
	{
		
		KG_COM_PROCESS_ERROR(pCamAni->SetKeySelChangeCallback(&OnSelKeyChange, NULL));
		m_stpSSEDialogCamera = this;
	}
	else
	{
		m_stpSSEDialogCamera = NULL;//注意顺序
		KG_COM_PROCESS_ERROR(pCamAni->SetKeySelChangeCallback(NULL, NULL));
		
	}
	return S_OK;
Exit0:
	return E_FAIL;
}
BOOL KSceneSceneEditorDialogCamera::DestroyWindow()
{
	// TODO: Add your specialized code here and/or call the base class
	RegisterCallbacks(FALSE);
	return CDialog::DestroyWindow();
}

void KSceneSceneEditorDialogCamera::OnEnChangeEdPlayspeed()
{
	UpdateData(TRUE);

	if ( !m_lpSceneSceneEditor )
	{
		return;
	}
	//KG3DSceneCameraMovement& CamMov = m_lpSceneSceneEditor->m_CameraMovement;
	GETCAMMOV();
	IEKG3DSceneCameraAnimation *piCameraAnimation = NULL;
	pCamMov->GetCurCamAnimation(&piCameraAnimation);

	if(piCameraAnimation)
	{
		piCameraAnimation->SetPlaySpeed(m_fPlaySpeed);
	}
}

void KSceneSceneEditorDialogCamera::OnEnChangeEdPlayAngle()
{
	UpdateData(TRUE);

	if ( !m_lpSceneSceneEditor )
	{
		return;
	}
	//KG3DSceneCameraMovement& CamMov = m_lpSceneSceneEditor->m_CameraMovement;
	GETCAMMOV();
	IEKG3DSceneCameraAnimation *piCameraAnimation = NULL;
	pCamMov->GetCurCamAnimation(&piCameraAnimation);

	if(piCameraAnimation)
	{
		FLOAT fAngHu = (float)(m_fAngle * D3DX_PI / 180.0F);
		piCameraAnimation->SetPlayAngle(fAngHu);
	}
}
