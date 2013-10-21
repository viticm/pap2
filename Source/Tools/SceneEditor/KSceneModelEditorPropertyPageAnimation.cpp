#include "stdafx.h"
//#include "kg3danimationex.h"
#include "IEKG3DClip.h"
#include "sceneeditor.h"
#include "KSceneModelEditorPropertyPageAnimation.h"
#include "MeshEditorDoc.h"
#include "KSceneModelEditorFrameWnd.h"
//(by dengzhihui 2006年7月6日 11:34:09
//#include "mainfrm.h"
//)
#include "KDLG_SKWeight.h"
#include "KDlgAnimationComposer.h"
#include "KSceneModelEditorPanelView.h"
#include "IEKG3DAnimationController.h"
#include "IEKG3DAnimationTable.h"
#include "IEKG3DClipTable.h"
//#include "SceneEditorCommon.h"
#include "KSceneModelEditorAnimationPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(KSceneModelEditorPropertyPageAnimation, CPropertyPage)

static float s_fPlaySpeed = 1.f;
static float s_fTweenTime = 0.0f;
static TweenTimeSpan s_TweenTimeSpan = {400, 0, 1.0f};

struct Keyword2Type
{
	DWORD dwType;
	const TCHAR* strKeyword;
};

static Keyword2Type s_Keyword2AnimationSplitType[] = 
{
	0, "dummy",
	1, "m2",// 标准男
	2, "f2",// 标准女
	3, "m3",// 魁梧男
	4, "f3",// 性感女
	5, "m1",// 小男孩
	6, "f1",// 小孩女
};


KSceneModelEditorPropertyPageAnimation::KSceneModelEditorPropertyPageAnimation() : CPropertyPage(KSceneModelEditorPropertyPageAnimation::IDD)
, m_bLoop(FALSE)
, m_bPaused(FALSE)
{
	//{{AFX_DATA_INIT(KSceneModelEditorPropertyPageAnimation)
	m_fFrameTime = 0.0f;
	m_strAnimationName = _T("");
	m_dwNumFrame = 0;
	m_fSpeed = 1.0f;
	m_fTotalLength = 0.0f;
	m_lpModel = NULL;
	m_bInited = FALSE;
	m_nStartFrame = 0;
	m_nEndFrame = 0;
	m_fTweenTime = 0;
	m_fSpd = 1.0f;
	m_fTotalLength = 1.0f;
	m_bLoop = TRUE;
	m_bPaused = FALSE;
    m_fFloorSlipSpeed = 0;
	//}}AFX_DATA_INIT
}

KSceneModelEditorPropertyPageAnimation::~KSceneModelEditorPropertyPageAnimation()
{
}

void KSceneModelEditorPropertyPageAnimation::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KSceneModelEditorPropertyPageAnimation)
	DDX_Control(pDX, IDC_LIST_SEQU, m_ListBox_Sequence);
	DDX_Control(pDX, IDC_SLIDER_POS, m_SliderCtrl_Position);
	DDX_Control(pDX, IDC_LIST_ANIMATION, m_ListBox_Animation);
	DDX_Text(pDX, IDC_EDIT_FRAMETIME, m_fFrameTime);
	DDV_MinMaxFloat(pDX, m_fFrameTime, 0.f, 10000.f);
	DDX_Text(pDX, IDC_EDIT_NAME, m_strAnimationName);
	DDX_Text(pDX, IDC_EDIT_NUMFRAME, m_dwNumFrame);
	DDX_Text(pDX, IDC_EDIT_TOTALLENGTH, m_fTotalLength);

	DDX_Text(pDX, IDC_EDIT_START, m_nStartFrame);
	DDX_Text(pDX, IDC_EDIT_END,   m_nEndFrame);
	DDX_Text(pDX, IDC_EDIT_SPD,   m_fSpd);
	DDX_Text(pDX, IDC_EDIT_TWEENTIME, m_fTweenTime);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_CHECK_PLAYONCE, m_bLoop);
	DDX_Check(pDX, IDC_BUTTON_PAUSE, m_bPaused);

    DDX_Text(pDX, IDC_EDIT_SLIP_SPEED, m_fFloorSlipSpeed);
	DDX_Control(pDX, IDC_CHECK_ENABLEHALF, m_ctlEnableWarper);
}

void ClearListBox(CListBox& ListBox)
{
	ListBox.SetCurSel(-1);
	int Count = ListBox.GetCount();
	for(int i= Count + 1; i >= 0; i--)
	{
		ListBox.DeleteString(i);
	}
}

BEGIN_MESSAGE_MAP(KSceneModelEditorPropertyPageAnimation, CPropertyPage)
	//{{AFX_MSG_MAP(KSceneModelEditorPropertyPageAnimation)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_LOAD, OnBnClickedButtonLoad)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_LBN_SELCHANGE(IDC_LIST_ANIMATION, OnLbnSelchangeListAnimation)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_RENAME, OnBnClickedButtonRename)
	ON_BN_CLICKED(IDC_BUTTON_SAVESEPARATE, OnBnClickedButtonSaveseparate)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnBnClickedButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_SAVESEQU, OnBnClickedButtonSavesequ)
	ON_BN_CLICKED(IDC_BUTTON_LOADSEQU, OnBnClickedButtonLoadsequ)
	ON_BN_CLICKED(IDC_BUTTON_PLAYSEQU, OnBnClickedButtonPlaysequ)
	ON_LBN_SELCHANGE(IDC_LIST_SEQU, OnLbnSelchangeListSequ)
	ON_BN_CLICKED(IDC_BUTTON_PAUSE, OnBnClickedButtonPause)
	ON_BN_CLICKED(IDC_BUTTON_ANIWARPER, &KSceneModelEditorPropertyPageAnimation::OnBnClickedButtonAniwarper)
	ON_BN_CLICKED(IDC_BUTTON_NORMALIZE, &KSceneModelEditorPropertyPageAnimation::OnBnClickedButtonNormalize)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, &KSceneModelEditorPropertyPageAnimation::OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_CHECK_PLAYONCE, &KSceneModelEditorPropertyPageAnimation::OnBnClickedCheckPlayonce)
    ON_BN_CLICKED(IDC_BUTTON_FLOOR_TEX, &KSceneModelEditorPropertyPageAnimation::OnBnClickedButtonFloorTex)
	ON_BN_CLICKED(IDC_BUTTON_TOP, &KSceneModelEditorPropertyPageAnimation::OnBnClickedButtonTop)
	ON_BN_CLICKED(IDC_BUTTON_BOTTOM, &KSceneModelEditorPropertyPageAnimation::OnBnClickedButtonBottom)
	ON_BN_CLICKED(IDC_CHECK_ENABLEHALF, &KSceneModelEditorPropertyPageAnimation::OnBnClickedCheckEnablehalf)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorPropertyPageAnimation message handlers

BOOL KSceneModelEditorPropertyPageAnimation::OnInitDialog()
{
    bool bRetValue = false;
	CPropertyPage::OnInitDialog();

	m_bInited = TRUE;
	m_SliderCtrl_Position.SetRange(0,100,TRUE);
	m_SliderCtrl_Position.SetTicFreq(10);
	UpdateUIState(FALSE);

    bRetValue = true;
//Exit0:
	return bRetValue;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneModelEditorPropertyPageAnimation::OnBnClickedButtonLoad()
{
	HRESULT hr = E_FAIL;
	TCHAR strFileNameBuffer[MAX_PATH * 10] = "";
	TCHAR strFilter[] = "Animation Files (*.Ani)|*.Ani|";
	TCHAR strDefaultPath[MAX_PATH];
    LPCTSTR pszFullName = NULL;

	strDefaultPath[0] = '\0';

	CFileDialog DlgLoad(TRUE, ".ani", "test.ani", OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT, strFilter);

	KG_PROCESS_ERROR(m_lpModel);

    hr = m_lpModel->GetName(&pszFullName);
    KGLOG_COM_PROCESS_ERROR(hr);

	strcpy_s(strDefaultPath,
		MAX_PATH, 
		pszFullName);
	
	for (size_t i = 0; i < sizeof(g_ModelAnimationKeywords) / sizeof(const TCHAR*); i++)
	{
		TCHAR *strModify = strstr(strDefaultPath, g_ModelAnimationKeywords[i]);
		if (strModify)
		{
			strModify[0] = '\0';
			strcat(strModify, "动作\\");
		}
	}

	DlgLoad.m_ofn.lpstrFile = strFileNameBuffer;
	DlgLoad.m_ofn.nMaxFile = sizeof(strFileNameBuffer);
	DlgLoad.m_ofn.lpstrInitialDir = strDefaultPath;

	KG_PROCESS_ERROR(DlgLoad.DoModal() == IDOK);

	POSITION FilePos = DlgLoad.GetStartPosition();

	while(FilePos)
	{
		CString strPathName = DlgLoad.GetNextPathName(FilePos);
		hr = m_AnimationContainer.LoadAnimationFromFile(strPathName, 0);
		if (FAILED(hr))
		{
			MessageBox(strPathName, "Error");
		}
	}

	m_AnimationContainer.FillAnimationList(m_ListBox_Animation, KG3DAnimationContainer::ENUM_FILL_WITH_ANINAME);

	int nNumAnimation = m_ListBox_Animation.GetCount();
	UpdateUIState(nNumAnimation);

    GetDlgItem(IDC_CHECK_ENABLEHALF)->EnableWindow(TRUE);

	hr = S_OK;
Exit0:
	return;
}

void KSceneModelEditorPropertyPageAnimation::SetModel(IEKG3DModel* pModel)
{
	HRESULT hr = E_FAIL;
	if(m_lpModel != pModel)
	{
		m_lpModel = pModel;
	}
}

void KSceneModelEditorPropertyPageAnimation::SetModel2(IEKG3DModel* pModel)
{
	HRESULT hr = E_NOTIMPL;
}

void KSceneModelEditorPropertyPageAnimation::OnBnClickedButtonSave()
{
	HRESULT hr = E_FAIL;
	char szFilter[] ="Animation Files (*.Ani)|*.Ani|";
	CString strPathName = "";
	CString strAniName = "";
	IEKG3DClip* pClip = NULL;
	
	int nCurSel = -1;
	CFileDialog DlgSave(false, "ani", NULL, OFN_HIDEREADONLY , szFilter);

	KG_PROCESS_ERROR(m_lpModel);

	nCurSel = m_ListBox_Animation.GetCurSel();

	KG_PROCESS_ERROR(nCurSel != LB_ERR);
	KG_PROCESS_ERROR(DlgSave.DoModal() == IDOK);

	strPathName = DlgSave.GetPathName();
	pClip = reinterpret_cast<IEKG3DClip*>(m_ListBox_Animation.GetItemData(nCurSel));

	if(pClip)
	{
		hr = pClip->SaveToFile(strPathName, 0);
		KGLOG_COM_PROCESS_ERROR(hr);
	}
Exit0:
	if (FAILED(hr))
	{
		MessageBox("Save animation file failed.");
	}
}

void KSceneModelEditorPropertyPageAnimation::OnLbnSelchangeListAnimation()
{
	HRESULT hr = E_FAIL;
	int nIndex = -1;
	IEKG3DClip* pClip = NULL;
    LPCTSTR pszName = NULL;
    KSceneModelEditorFrameWnd *pWnd = NULL;
    KSceneModelEditorAnimationPanel *pAnimationSeekBar = NULL;
	UpdateData(TRUE);

	nIndex = m_ListBox_Animation.GetCurSel();
	KG_PROCESS_ERROR(nIndex >= 0);

	pClip = reinterpret_cast<IEKG3DClip*>(m_ListBox_Animation.GetItemData(nIndex));

    hr = pClip->GetPathName(&pszName);
    KGLOG_COM_PROCESS_ERROR(hr);

    m_ListBox_Animation.GetText(nIndex, m_strAnimationName);

	hr = pClip->GetNumFrames(&m_dwNumFrame);
	KGLOG_COM_PROCESS_ERROR(hr);
	hr = pClip->GetFrameLength(&m_fFrameTime);
	KGLOG_COM_PROCESS_ERROR(hr);
    m_fTotalLength = m_fFrameTime * (m_dwNumFrame - 1);
	UpdateData(FALSE);

	KG_PROCESS_ERROR(m_lpModel);

	if (m_ctlEnableWarper.GetCheck() == BST_UNCHECKED)
	{
		if (m_bLoop)
		{
			float fTweenTime = 1000.0f;
			hr = m_lpModel->PlayAnimation(ENU_ANIMATIONPLAY_CIRCLE, pszName, s_fPlaySpeed, 0, &fTweenTime, NULL, ANICTL_PRIMARY);
		}
		else
		{
			hr = m_lpModel->PlayAnimation(ENU_ANIMATIONPLAY_ONCE, pszName, s_fPlaySpeed, 0, NULL, NULL, ANICTL_PRIMARY);
		}
		if (FAILED(hr))
		{
			MessageBox("Animation file not match.", "Error", MB_OK);
		}
	}
    
    pWnd = static_cast<KSceneModelEditorFrameWnd*>(GetModelFrameWnd());
    KG_PROCESS_ERROR(pWnd);

    pAnimationSeekBar = pWnd->GetAnimationSeekBar();
    KG_PROCESS_ERROR(pAnimationSeekBar);

	m_lpModel->GetAnimationTagContainer(&pAnimationSeekBar->m_pContainer);

    pAnimationSeekBar->SetAnimation(pszName);
Exit0:
	return;
}

void KSceneModelEditorPropertyPageAnimation::OnBnClickedButtonDelete()
{
	HRESULT hr          = E_FAIL;
	int nCurSel         = m_ListBox_Animation.GetCurSel();
	IEKG3DAnimationController *pAniController = NULL;
	IEKG3DClip* pClip = NULL;

	int nNumAnimation = 0;

	KG_PROCESS_ERROR(nCurSel != LB_ERR);

	pClip = reinterpret_cast<IEKG3DClip*>(m_ListBox_Animation.GetItemData(nCurSel));
	KG_PROCESS_ERROR(pClip);

	m_AnimationContainer.ReleaseAnimation(pClip);

	m_AnimationContainer.FillAnimationList(m_ListBox_Animation,
		                                   KG3DAnimationContainer::ENUM_FILL_WITH_ANINAME);

	KG_PROCESS_ERROR(m_lpModel);

    hr = m_lpModel->GetIEKG3DAnimationController(&pAniController);
	KGLOG_COM_PROCESS_ERROR(hr);

    hr = pAniController->Detach();
    KGLOG_COM_PROCESS_ERROR(hr);

	nNumAnimation = m_ListBox_Animation.GetCount();
	UpdateUIState(nNumAnimation);

	m_fFrameTime = 0.0f;
	m_fTotalLength = 0.0f;
	m_dwNumFrame = 0;
	UpdateData(FALSE);
Exit0:
	return;
}

void KSceneModelEditorPropertyPageAnimation::OnBnClickedButtonRename()
{
	IEKG3DClip* pClip = NULL;
	HRESULT hr = E_FAIL;
	if(!m_lpModel)
		return;
    UpdateData(TRUE);
	int nCurSel = m_ListBox_Animation.GetCurSel();
	if(nCurSel != LB_ERR)
	{
		DWORD_PTR dwPtr = m_ListBox_Animation.GetItemData(nCurSel);
		pClip = reinterpret_cast<IEKG3DClip*>(dwPtr);
		m_ListBox_Animation.DeleteString(nCurSel);
		m_ListBox_Animation.InsertString(nCurSel, m_strAnimationName);
		m_ListBox_Animation.SetItemData(nCurSel, dwPtr);
		pClip->Rename(m_strAnimationName);
	}
}

void KSceneModelEditorPropertyPageAnimation::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	HRESULT hr = E_FAIL;
	float fPosAni = m_SliderCtrl_Position.GetPos() / 100.0f;
    IKG3DAnimationController *piAniController = NULL;

	KG_PROCESS_ERROR(m_lpModel);

    m_lpModel->IEGetAnimationController(&piAniController);
	int nCycleTime = piAniController->GetCycleTime();
	piAniController->SeekAnimation(ANIMATION_SEEK_SET, static_cast<int>(fPosAni * nCycleTime));
Exit0:
	return;

}

void KSceneModelEditorPropertyPageAnimation::OnBnClickedButtonSaveseparate()
{
	HRESULT hr = E_FAIL;
	int nCount = m_ListBox_Animation.GetCount();
    IEKG3DClip* pClip = NULL;
	LPCTSTR pszName = NULL;
    TCHAR strOutputInfo[512];
	BOOL bSucceed = TRUE;
	for(int i = 0; i < nCount; i++)
	{
		pClip = reinterpret_cast<IEKG3DClip*>(m_ListBox_Animation.GetItemData(i));
		hr = pClip->GetPathName(&pszName);
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = pClip->SaveToFile(pszName, 0);
		if(FAILED(hr))
		{
			sprintf(strOutputInfo, "Save %s failed.", pszName);
			MessageBox(strOutputInfo, "Error");
			bSucceed = FALSE;
		}
	}
	if (bSucceed)
	{
		MessageBox("All animations save succeed.");
	}
Exit0:
    return;
}

void KSceneModelEditorPropertyPageAnimation::OnBnClickedButtonAdd()
{
//	HRESULT hr = E_FAIL;
//	IEKG3DAnimation* pSelAnimation = NULL;
//	int nCurSel = m_ListBox_Animation.GetCurSel();
//	int nCount = 0;
//	int i = 0;
//	KG_PROCESS_ERROR(nCurSel != LB_ERR);
//
//	pSelAnimation = reinterpret_cast<IEKG3DAnimation*>(m_ListBox_Animation.GetItemData(nCurSel));
//
//	m_pAnimation->InsertAnimation(pSelAnimation, -1);
//	FillSequenceEditBox();
//Exit0:
	return;

}

void KSceneModelEditorPropertyPageAnimation::OnBnClickedButtonDel()
{
//	HRESULT hResult = E_FAIL;
//    IEKG3DAnimation *pAni = NULL;
//    IEKG3DAnimationController *pAniController = NULL;
//
//    int nCurSel = m_ListBox_Sequence.GetCurSel();
//	if (!m_lpModel ||
//		nCurSel == LB_ERR)
//		return;
//
//	hResult = m_pAnimation->RemoveClipFromAnimation(nCurSel);
//	KGLOG_COM_PROCESS_ERROR(hResult);
//    m_ListBox_Sequence.DeleteString(nCurSel);
//
//    hResult = m_lpModel->GetIEKG3DAnimation(&pAni);
//    KGLOG_COM_PROCESS_ERROR(hResult);
//
//	if (pAni == m_pAnimation)
//	{
//        hResult = m_lpModel->GetIEKG3DAnimationController(&pAniController);
//        KGLOG_COM_PROCESS_ERROR(hResult);
//
//		pAniController->PauseAnimation(TRUE);
//	}
//    hResult = S_OK;
//Exit0:
    return;
}

void KSceneModelEditorPropertyPageAnimation::OnBnClickedButtonSavesequ()
{
	/*CFileDialog dlgSave(FALSE, "seq", "NoName", OFN_OVERWRITEPROMPT, "Animation Sequence(*.seq) |*.seq||", AfxGetMainWnd());
	if ( dlgSave.DoModal() == IDOK )
	{
		CString strPathName = dlgSave.GetPathName();
		if( m_lpModel)
		{
			m_lpModel->SaveSequence(strPathName, TEST_SEQ_NAME);
		}
	}*/
}

void KSceneModelEditorPropertyPageAnimation::OnBnClickedButtonLoadsequ()
{
	//CFileDialog dlgOpen(TRUE, "seq", "NoName", OFN_OVERWRITEPROMPT, "Animation Sequence(*.seq) |*.seq||", AfxGetMainWnd());
	//if ( dlgOpen.DoModal() == IDOK )
	//{
	//	CString strPathName = dlgOpen.GetPathName();
	//	if( m_lpModel)
	//	{
	//		m_lpModel->LoadSequence(strPathName, TRUE, TRUE);
	//		for( int nIndex = m_ListBox_Sequence.GetCount() - 1; nIndex >= 0; nIndex-- )
	//		{
	//			m_ListBox_Sequence.DeleteString(0);
	//		}
	//
	//		KModel::AnimationSequenceHeader* pHeader = m_lpModel->GetLastSequence();
	//		/*TCHAR* strBuffer = new TCHAR[pHeader->dwNumAnimation * MAX_ANI_NAME_LENGTH];
 //           int nCount = 0;
	//		m_lpModel->GetAnimationNames( pHeader->SequenceName, strBuffer, nCount );
	//		for( int nIndex = 0; nIndex < pHeader->dwNumAnimation; nIndex++ )
	//		{
	//			m_ListBox_Sequence.AddString( strBuffer + nIndex * MAX_ANI_NAME_LENGTH);
	//		}
	//		SAFE_DELETE_ARRAY(strBuffer);*/
	//		FillListBoxSequence(pHeader->SequenceName);

	//	}
	//}
}
void KSceneModelEditorPropertyPageAnimation::OnBnClickedButtonPlaysequ()
{
//    HRESULT hRetCode = 0;
//	int nCurSel = 0;
//	IEKG3DAnimationController* pController = NULL;
//    IKG3DAnimationController * piController = NULL;
//	KG_PROCESS_ERROR(m_lpModel);
//
//	//m_lpModel->SetAnimation(m_pAnimation);
//	hRetCode = m_lpModel->GetIEKG3DAnimationController(&pController);
//    KGLOG_COM_PROCESS_ERROR(hRetCode);
//
//	KG_PROCESS_ERROR(pController);
//
//    hRetCode = m_lpModel->IEGetAnimationController(&piController);
//    KGLOG_COM_PROCESS_ERROR(hRetCode);
//
//	nCurSel = m_ListBox_Sequence.GetCurSel();
//	KG_PROCESS_ERROR(nCurSel != LB_ERR);
//
//	hRetCode = pController->SetAnimation(m_pClip);
//    KGLOG_COM_PROCESS_ERROR(hRetCode);
//
//	piController->StartAnimation(ENU_ANIMATIONPLAY_CIRCLE, 0, 1.0f);
//Exit0:
//	return;
}


BOOL KSceneModelEditorPropertyPageAnimation::PreTranslateMessage( MSG* pMsg )
{
	HRESULT hResult = E_FAIL;
    HRESULT hRetCode = E_FAIL;
	IEKG3DClip* pClip = NULL;
	HRESULT hr = E_FAIL;
	int nSlot = 0;
	DWORD dwNumFrame = 0;

	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)
		{
			if(	pMsg->hwnd == GetDlgItem(IDC_EDIT_START)->GetSafeHwnd() ||
				pMsg->hwnd == GetDlgItem(IDC_EDIT_END)->GetSafeHwnd()   ||
				pMsg->hwnd == GetDlgItem(IDC_EDIT_SPD)->GetSafeHwnd()   ||
				pMsg->hwnd == GetDlgItem(IDC_EDIT_TWEENTIME)->GetSafeHwnd()
				)
			{
	/*			UpdateData(TRUE);
				nSlot = m_ListBox_Sequence.GetCurSel();
				hRetCode = m_pAnimation->GetClipData(nSlot, ItemInfo);
                KGLOG_COM_PROCESS_ERROR(hRetCode);
				hRetCode = ItemInfo.piClip->GetNumFrames(&dwNumFrame);
				KGLOG_COM_PROCESS_ERROR(hRetCode);
                m_nStartFrame = min(max(0, m_nStartFrame), static_cast<int>(dwNumFrame - 1));
				m_nEndFrame = min(max(m_nStartFrame, m_nEndFrame), static_cast<int>(dwNumFrame - 1));
				m_fSpd = max(0.0f, m_fSpd);
				m_fTweenTime = max(0.0f, m_fTweenTime);
				UpdateData(FALSE);
				hRetCode = m_pAnimation->SetClipData(nSlot, m_nStartFrame, m_nEndFrame, m_fSpd, m_fTweenTime);
			    KGLOG_COM_PROCESS_ERROR(hRetCode);*/
            }
			else if(pMsg->hwnd == GetDlgItem(IDC_EDIT_FRAMETIME)->GetSafeHwnd())
			{
				UpdateData(TRUE);
				int nCurSel = m_ListBox_Animation.GetCurSel();
				KG_PROCESS_ERROR(nCurSel != LB_ERR);

				pClip = reinterpret_cast<IEKG3DClip*>(m_ListBox_Animation.GetItemDataPtr(nCurSel));
				if(pClip)
				{
                    pClip->SetFrameLength(m_fFrameTime);
					m_fTotalLength = m_fFrameTime * m_fFrameTime;
					UpdateData(FALSE);
				}
			}
            else if(pMsg->hwnd == GetDlgItem(IDC_EDIT_SLIP_SPEED)->GetSafeHwnd())
            {
                KSceneModelEditorPanelView* pWndPanel = static_cast<KSceneModelEditorPanelView*>(GetParent()->GetParent()->GetParent());
                KSceneModelEditorDoc* pDoc = NULL;
                UpdateData(TRUE);
                pDoc = (KSceneModelEditorDoc*)(pWndPanel->GetDocument());
                KG_PROCESS_ERROR(pDoc);
                KG_PROCESS_ERROR(pDoc->GetScene());
				IEKG3DSceneModelEditor* pEditor = NULL;
				pDoc->GetScene()->GetSceneModelEditor(&pEditor);
				KG_PROCESS_ERROR(pEditor);
                pEditor->SetFloorSlipSpeed(m_fFloorSlipSpeed);//by dengzhihui Refactor 2006年11月24日
            }
		}
	}
    hResult = S_OK;
Exit0:
	return hResult;
}

void KSceneModelEditorPropertyPageAnimation::OnLbnSelchangeListSequ()
{
//	AnimationItem Item;
//	int nCurSel = 0;
//
//	if (!m_lpModel)
//		goto Exit0;
//
//	nCurSel = m_ListBox_Sequence.GetCurSel();
//	if (nCurSel != LB_ERR)
//	{
//		m_pAnimation->GetClipData(nCurSel, Item);
//		m_nEndFrame = Item.nEndFrame;
//		m_nStartFrame = Item.nStartFrame;
//		m_fTweenTime = Item.fTweenTime;
//		m_fSpd = Item.fSpeed;
//		GetDlgItem(IDC_EDIT_START)->EnableWindow();
//		GetDlgItem(IDC_EDIT_END)->EnableWindow();
//		GetDlgItem(IDC_EDIT_SPD)->EnableWindow();
//		GetDlgItem(IDC_EDIT_TWEENTIME)->EnableWindow();
//		UpdateData(FALSE);
//	}
//	else
//	{
//		m_nEndFrame = 0;
//		m_nStartFrame = 0;
//		m_fSpd = 1.0f;
//		m_fTweenTime = 0.0f;
//		UpdateData(FALSE);
//		GetDlgItem(IDC_EDIT_START)->EnableWindow(FALSE);
//		GetDlgItem(IDC_EDIT_END)->EnableWindow(FALSE);
//		GetDlgItem(IDC_EDIT_SPD)->EnableWindow(FALSE);
//		GetDlgItem(IDC_EDIT_TWEENTIME)->EnableWindow(FALSE);
//	}
//
//Exit0:
	return;
}

void KSceneModelEditorPropertyPageAnimation::FillSequenceEditBox()
{
//	HRESULT hResult = E_FAIL;
//    AnimationItem Item;
//	int nNumClips = 0;
//    int i = 0;
//    LPCTSTR pszName = NULL;
//    hResult = m_pAnimation->GetNumClips(&nNumClips);
//    KGLOG_COM_PROCESS_ERROR(hResult);
//
//	ClearListBox(m_ListBox_Sequence);
//	KG_PROCESS_ERROR(nNumClips);
//
//	for (i = 0; i < nNumClips; i++)
//	{
//		m_pAnimation->GetClipData(i, Item);
//		hResult = Item.piClip->GetName(&pszName);
//        KGLOG_COM_PROCESS_ERROR(hResult);
//        m_ListBox_Sequence.AddString(pszName);
//	}
//
//Exit0:
	return;
}

void KSceneModelEditorPropertyPageAnimation::OnBnClickedButtonPause()
{
	if(!m_lpModel)
		return;
    IKG3DAnimationController *piAniController = NULL;
	CButton* pBtPauseAni = static_cast<CButton*>(GetDlgItem(IDC_BUTTON_PAUSE));
	int nCheckState = pBtPauseAni->GetCheck();
    m_lpModel->IEGetAnimationController(&piAniController);
	if(nCheckState == BST_CHECKED)//Pause animation
	{
        piAniController->PauseAnimation(TRUE);
	}
	else if(nCheckState == BST_UNCHECKED)//play
	{
		piAniController->PauseAnimation(FALSE);
	}
}

void KSceneModelEditorPropertyPageAnimation::OnBnClickedButtonAniwarper()
{
	KG_PROCESS_ERROR(m_lpModel);

	KDlgAnimationComposer* pComposer = new KDlgAnimationComposer(this);
	pComposer->Create(KDlgAnimationComposer::IDD, this);
    pComposer->SetAnimationContainer(&m_AnimationContainer);
	pComposer->SetModel(m_lpModel);
	pComposer->ShowWindow(SW_SHOW);

Exit0:
	return;
}

//void KSceneModelEditorPropertyPageAnimation::OnBnClickedButtonNormalize()
//{
//    HRESULT hRetCode = E_FAIL;
//	int nCount = m_ListBox_Animation.GetCount();
//	if (nCount < 2)
//		return;
//	int i = 0;
//	IEKG3DClip* pClipST = reinterpret_cast<IEKG3DClip*>(m_ListBox_Animation.GetItemData(i));
//	//int nClips = 0;
//    //hRetCode = pAniST->GetNumClips(&nClips);
//    //KGLOG_COM_PROCESS_ERROR(hRetCode);
//
//    //KG_PROCESS_ERROR(nClips);
//	//AnimationItem Item;
//	//pAniST->GetClipData(0, Item);
//
//	for (i = 1; i < nCount; i++)
//	{
//		IEKG3DClip* pClip = reinterpret_cast<IEKG3DClip*>(m_ListBox_Animation.GetItemData(i));
//		//hRetCode = pClip->GetNumClips(&nClips);
//       // KGLOG_COM_PROCESS_ERROR(hRetCode);
//
//		//if (!nClips)
//		//	continue;
//		//AnimationItem Item1;
//		//pClip->GetClipData(0, Item1);
//		pClipST->NormalizeClipData(pClip, FALSE);
//	}
////Exit0:
//    return;
//}

void KSceneModelEditorPropertyPageAnimation::OnBnClickedButtonNormalize()
{
	m_lpModel->PlayAnimation(ENU_ANIMATIONPLAY_ONCE, 
		"D:\\Sword\\Code\\sword3-products\\trunk\\client\\data\\source\\player\\F2\\动作\\F2b01aq攻击21.ani", 
		s_fPlaySpeed, 
		0, 
		NULL, 
		NULL, 
		ANICTL_SECONDARY);
}

void KSceneModelEditorPropertyPageAnimation::OnBnClickedButtonPlay()
{
	HRESULT hRetCode = E_FAIL;
    IEKG3DClip* pClip= NULL;
	LPCTSTR pszName = NULL;
    int nCurSel = m_ListBox_Animation.GetCurSel();
    KG_PROCESS_ERROR(nCurSel != LB_ERR);
	pClip = reinterpret_cast<IEKG3DClip*>(m_ListBox_Animation.GetItemDataPtr(nCurSel));
	hRetCode = pClip->GetPathName(&pszName);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    KG_PROCESS_ERROR(pClip);
	KG_PROCESS_ERROR(m_lpModel);

	UpdateData();

	if (m_bLoop)
	{
		m_lpModel->PlayAnimation(ENU_ANIMATIONPLAY_CIRCLE, pszName, s_fPlaySpeed, 0, NULL, NULL, ANICTL_PRIMARY);
	}
	else
	{
		m_lpModel->PlayAnimation(ENU_ANIMATIONPLAY_ONCE, pszName, s_fPlaySpeed, 0, NULL, NULL, ANICTL_PRIMARY);
	}

	m_bPaused = FALSE;

	UpdateData(FALSE);

Exit0:
	return;
}

void KSceneModelEditorPropertyPageAnimation::UpdateUIState(BOOL bAniLoaded)
{
	GetDlgItem(IDC_BUTTON_PAUSE)->EnableWindow(bAniLoaded);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(bAniLoaded);
	GetDlgItem(IDC_BUTTON_DELETE)->EnableWindow(bAniLoaded);
	GetDlgItem(IDC_BUTTON_SAVESEPARATE)->EnableWindow(bAniLoaded);
	GetDlgItem(IDC_BUTTON_ANIWARPER)->EnableWindow(bAniLoaded);
	GetDlgItem(IDC_BUTTON_NORMALIZE)->EnableWindow(bAniLoaded);
	GetDlgItem(IDC_EDIT_FRAMETIME)->EnableWindow(bAniLoaded);
	GetDlgItem(IDC_SLIDER_POS)->EnableWindow(bAniLoaded);
	GetDlgItem(IDC_CHECK_PLAYONCE)->EnableWindow(bAniLoaded);
	GetDlgItem(IDC_BUTTON_PLAY)->EnableWindow(bAniLoaded);
	GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(bAniLoaded);
	GetDlgItem(IDC_BUTTON_DEL)->EnableWindow(bAniLoaded);
	GetDlgItem(IDC_BUTTON_PLAYSEQU)->EnableWindow(bAniLoaded);
	GetDlgItem(IDC_BUTTON_RENAME)->EnableWindow(bAniLoaded);
}

void KSceneModelEditorPropertyPageAnimation::OnBnClickedCheckPlayonce()
{
	HRESULT hRetCode = E_FAIL;
    //IEKG3DAnimation* pAnimation = NULL;
	IEKG3DClip* pClip = NULL;
	int nCurSel = m_ListBox_Animation.GetCurSel();
    LPCTSTR pszAniFileName = NULL;
	KG_PROCESS_ERROR(nCurSel != LB_ERR);

	pClip = reinterpret_cast<IEKG3DClip*>(m_ListBox_Animation.GetItemDataPtr(nCurSel));
	KG_PROCESS_ERROR(pClip);

    hRetCode = pClip->GetPathName(&pszAniFileName);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

	UpdateData(TRUE);

	if (m_bLoop)
	{
		m_lpModel->PlayAnimation(ENU_ANIMATIONPLAY_CIRCLE, pszAniFileName, s_fPlaySpeed, 0, NULL, NULL, ANICTL_PRIMARY);
	}
	else
	{
		m_lpModel->PlayAnimation(ENU_ANIMATIONPLAY_ONCE, pszAniFileName, s_fPlaySpeed, 0, NULL, NULL, ANICTL_PRIMARY);
	}
Exit0:
	return;
}

void KSceneModelEditorPropertyPageAnimation::OnBnClickedButtonFloorTex()
{
    CFileDialog dlg(
        TRUE,
        TEXT("tga"),
        0,
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        TEXT("TGA贴图文件(*.tga)|*.tga|JPG贴图文件(*.jpg)|*.jpg|所有文件(*.*)|*.*||")
    );
    if (dlg.DoModal() == IDOK)
    {
		KSceneModelEditorPanelView* pWndPanel = static_cast<KSceneModelEditorPanelView*>(GetParent()->GetParent()->GetParent());
		KSceneModelEditorDoc* pDoc = NULL;
		IEKG3DSceneModelEditor* pEditor = NULL;
		UpdateData(TRUE);
		pDoc = (KSceneModelEditorDoc*)(pWndPanel->GetDocument());
		KG_PROCESS_ERROR(pDoc);
		pDoc->GetScene()->GetSceneModelEditor(&pEditor);
		KG_PROCESS_ERROR(pEditor);
		pEditor->SetFloorTexture(dlg.GetPathName());
	}
Exit0:
    return;
}

DWORD CheckModelTypeByKeyWord(LPCSTR strFileName)
{
	DWORD dwReturn = 0xffffff;
	TCHAR strLower[MAX_PATH];
	strcpy_s(strLower, MAX_PATH, strFileName);
	_strlwr(strLower);
	for (size_t i = 0; i < sizeof(s_Keyword2AnimationSplitType) / sizeof(Keyword2Type); i++)
	{
		if (strstr(strLower, s_Keyword2AnimationSplitType[i].strKeyword))
		{
			dwReturn = s_Keyword2AnimationSplitType[i].dwType;
			break;
		}
	}
	return dwReturn;
}

void KSceneModelEditorPropertyPageAnimation::OnBnClickedButtonTop()
{
	HRESULT hr = E_FAIL;
    int nCurSel = 0;
	//IEKG3DAnimation* pCurSel = NULL;
	IEKG3DClip* pCurSel = NULL;
    LPCTSTR pszName = NULL;
	//nCurSel = rand() % m_ListBox_Animation.GetCount();
	nCurSel = m_ListBox_Animation.GetCurSel();
	KG_PROCESS_ERROR(nCurSel != LB_ERR);
	pCurSel = reinterpret_cast<IEKG3DClip*>(m_ListBox_Animation.GetItemData(nCurSel));
	KG_PROCESS_ERROR(m_lpModel);
    pCurSel->GetPathName(&pszName);
	
	DWORD dwSplitType = CheckModelTypeByKeyWord(pszName);

    hr = m_lpModel->PlaySplitAnimation(
        pszName,
        dwSplitType,
        ANI_JOINT_TOP,
        ENU_ANIMATIONPLAY_CIRCLE,
        s_fPlaySpeed,
        0,
        &s_TweenTimeSpan,
		NULL,
        ANICTL_PRIMARY);
Exit0:
	return;
}


void KSceneModelEditorPropertyPageAnimation::OnBnClickedButtonBottom()
{
	HRESULT hr = E_FAIL;
	int nCurSel = 0;
	//IEKG3DAnimation* pCurSel = NULL;
	IEKG3DClip* pCurSel = NULL;
	nCurSel = m_ListBox_Animation.GetCurSel();
	KG_PROCESS_ERROR(nCurSel != LB_ERR);
	pCurSel = reinterpret_cast<IEKG3DClip*>(m_ListBox_Animation.GetItemData(nCurSel));
	KG_PROCESS_ERROR(m_lpModel);
	LPCTSTR pszName = NULL;
	pCurSel->GetPathName(&pszName);

	DWORD dwSplitType = CheckModelTypeByKeyWord(pszName);
	hr = m_lpModel->PlaySplitAnimation(
		pszName,
		dwSplitType,
		ANI_JOINT_BOTTOM,
		ENU_ANIMATIONPLAY_CIRCLE,
		s_fPlaySpeed,
		0,
		&s_TweenTimeSpan,
		NULL,
		ANICTL_PRIMARY);
Exit0:
	return;
}


//void KSceneModelEditorPropertyPageAnimation::OnBnClickedButtonBottom()
//{
//	HRESULT hrRetCode = E_FAIL;
//    int nCurSel = 0;
//	IEKG3DAnimation* pCurSel = NULL;
//	nCurSel = m_ListBox_Animation.GetCurSel();
//	KG_PROCESS_ERROR(nCurSel != LB_ERR);
//	pCurSel = reinterpret_cast<IEKG3DAnimation*>(m_ListBox_Animation.GetItemData(nCurSel));
//	KG_PROCESS_ERROR(m_lpModel);
//    LPCTSTR pszName = NULL;
//    pCurSel->GetName(&pszName);
//	/*hrRetCode = m_lpModel->PlaySplitAnimation(
//        pszName, 
//		ANIST_TYPE0,
//		TRUE,
//		ANI_JOINT_BOTTOM,
//		ENU_ANIMATIONPLAY_CIRCLE,
//		1.0f,
//		0,
//		NULL
//    );*/
//    hrRetCode = m_lpModel->PlaySplitAnimation(
//        pszName,
//        ANIST_TYPE0,
//        FALSE,
//        ANI_JOINT_BOTTOM,
//        ENU_ANIMATIONPLAY_CIRCLE,
//        1.0f,
//        0,
//        &s_fTweenTime,
//		NULL
//        );
//    if (FAILED(hrRetCode))
//    {
//        hrRetCode = m_lpModel->PlaySplitAnimation(
//            pszName, 
//            ANIST_TYPE1,
//            FALSE,
//            ANI_JOINT_BOTTOM,
//            ENU_ANIMATIONPLAY_CIRCLE,
//            1.0f,
//            0,
//            &s_fTweenTime,
//			NULL
//            );
//    }
//	{
//		//IKG3DAnimationController* pController = m_lpModel->GetAnimationController(ANICTL_PRIMARY)();
//		//pController->SetSpeed(2.0f);
//	}
//Exit0:
//	return;
//}

void KSceneModelEditorPropertyPageAnimation::OnBnClickedCheckEnablehalf()
{
    HRESULT hrRetCode = E_FAIL;

    int nCheckState = m_ctlEnableWarper.GetCheck();
    if (nCheckState == BST_CHECKED)
    {
        GetDlgItem(IDC_BUTTON_TOP)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_BOTTOM)->EnableWindow(TRUE);

        IEKG3DClip* piClip = NULL;
        LPCTSTR pszName = NULL;

        piClip = reinterpret_cast<IEKG3DClip*>(m_ListBox_Animation.GetItemData(0));
        KG_PROCESS_ERROR(piClip);

        hrRetCode = piClip->GetPathName(&pszName);
        KGLOG_COM_PROCESS_ERROR(hrRetCode);

        DWORD dwSplitType = CheckModelTypeByKeyWord(pszName);

        hrRetCode = m_lpModel->PlaySplitAnimation(
            pszName,
            dwSplitType,
            ANI_JOINT_TOP,
            ENU_ANIMATIONPLAY_CIRCLE,
            s_fPlaySpeed,
            0,
            &s_TweenTimeSpan,
            NULL,
            ANICTL_PRIMARY
        );
        KGLOG_COM_PROCESS_ERROR(hrRetCode);

        hrRetCode = m_lpModel->PlaySplitAnimation(
            pszName,
            dwSplitType,
            ANI_JOINT_BOTTOM,
            ENU_ANIMATIONPLAY_CIRCLE,
            s_fPlaySpeed,
            0,
            &s_TweenTimeSpan,
            NULL,
            ANICTL_PRIMARY
        );
        KGLOG_COM_PROCESS_ERROR(hrRetCode);
    }
    else
    {
        GetDlgItem(IDC_BUTTON_TOP)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_BOTTOM)->EnableWindow(FALSE);
    }

Exit0:
    return;
}
