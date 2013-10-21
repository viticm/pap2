#include "stdafx.h"
#include "sceneeditor.h"
#include "iekg3danimationtag.h"
#include "KSceneModelEditorAnimationPanel.h"
#include "MainFrm.h"
#include "KSceneModelEditorFrameWnd.h"
#include "KG3DAnimationContainer.h"
#include "iekg3dclip.h"
#include "KDlgMotionTag.h"
#include "IKG3DSoundShell.h"
#include "KModelEditorTagExport.h"
#include "KDlgSoundTag.h"
#include "KDlgSfxTag.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorAnimationPanel
static TCHAR s_strOutputInfo[MAX_PATH];
IMPLEMENT_DYNCREATE(KSceneModelEditorAnimationPanel, CFormView)

static TCHAR *s_strStartPath = "data\\source\\player\\";

KSceneModelEditorAnimationPanel::KSceneModelEditorAnimationPanel()
	: CFormView(KSceneModelEditorAnimationPanel::IDD)
{
	//{{AFX_DATA_INIT(KSceneModelEditorAnimationPanel)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
    m_pContainer = NULL;
    m_nLastSFXTagDialog = -1;
    m_strSFXStartPath = "data\\source\\other\\特效";
	m_PatternID = KDlgPlayerCheckTool::INVALIDATEPATTERN;
	m_strCurTaniName = "";
}

KSceneModelEditorAnimationPanel::~KSceneModelEditorAnimationPanel()
{
}

void KSceneModelEditorAnimationPanel::DoDataExchange(CDataExchange* pDX)
{
    CFormView::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_ANIMATIONSEEKBAR1, m_AnimationSeekBar);
    //{{AFX_DATA_MAP(KSceneModelEditorAnimationPanel)
    // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
    DDX_Control(pDX, IDC_BUTTON_RIGHT, m_ctlCursorRight);
    DDX_Control(pDX, IDC_BUTTON_VERNIER, m_ctlCursorPosition);
    DDX_Control(pDX, IDC_BUTTON_LEFT, m_ctlCursorLeft);
}


BEGIN_MESSAGE_MAP(KSceneModelEditorAnimationPanel, CFormView)
	//{{AFX_MSG_MAP(KSceneModelEditorAnimationPanel)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_WM_PAINT()
    ON_MESSAGE(WM_ANIMATION_SEEK, &KSceneModelEditorAnimationPanel::OnAnimationSeek)
    ON_BN_CLICKED(IDC_BUTTON_PLAY, &KSceneModelEditorAnimationPanel::OnBnClickedButtonPlay)
    ON_BN_CLICKED(IDC_BUTTON_SAVE, &KSceneModelEditorAnimationPanel::OnBnClickedButtonSave)
    ON_BN_CLICKED(IDC_BUTTON_LOAD, &KSceneModelEditorAnimationPanel::OnBnClickedButtonLoad)
    ON_BN_CLICKED(IDC_BUTTON_LEFT, &KSceneModelEditorAnimationPanel::OnBnClickedButtonLeft)
    ON_BN_CLICKED(IDC_BUTTON_RIGHT, &KSceneModelEditorAnimationPanel::OnBnClickedButtonRight)
	ON_BN_CLICKED(IDC_TAG_EXPORT, &KSceneModelEditorAnimationPanel::OnBnClickedTagExport)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorAnimationPanel diagnostics

#ifdef _DEBUG
void KSceneModelEditorAnimationPanel::AssertValid() const
{
	CFormView::AssertValid();
}

void KSceneModelEditorAnimationPanel::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorAnimationPanel message handlers

void KSceneModelEditorAnimationPanel::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CFormView::OnPaint() for painting messages
	//this->m_DialogTimeLine.OnDraw(0,&dc);
}
BEGIN_EVENTSINK_MAP(KSceneModelEditorAnimationPanel, CFormView)
    ON_EVENT(KSceneModelEditorAnimationPanel, IDC_ANIMATIONSEEKBAR1, 227, KSceneModelEditorAnimationPanel::OnMotionTagCopyAnimationseekbar1, VTS_I4 VTS_I4)
    ON_EVENT(KSceneModelEditorAnimationPanel, IDC_ANIMATIONSEEKBAR1, 224, KSceneModelEditorAnimationPanel::OnMotionTagDeleteAnimationseekbar1, VTS_I4)
    ON_EVENT(KSceneModelEditorAnimationPanel, IDC_ANIMATIONSEEKBAR1, 223, KSceneModelEditorAnimationPanel::OnMotionTagLoadAnimationseekbar1, VTS_I4 VTS_PI4)
    ON_EVENT(KSceneModelEditorAnimationPanel, IDC_ANIMATIONSEEKBAR1, 222, KSceneModelEditorAnimationPanel::OnMotionTagSaveAnimationseekbar1, VTS_I4 VTS_I4)
    ON_EVENT(KSceneModelEditorAnimationPanel, IDC_ANIMATIONSEEKBAR1, 218, KSceneModelEditorAnimationPanel::OnNewMotionTagAnimationseekbar1, VTS_I4)
    ON_EVENT(KSceneModelEditorAnimationPanel, IDC_ANIMATIONSEEKBAR1, 216, KSceneModelEditorAnimationPanel::OnNewSFXTagAnimationseekbar1, VTS_I4)
    ON_EVENT(KSceneModelEditorAnimationPanel, IDC_ANIMATIONSEEKBAR1, 217, KSceneModelEditorAnimationPanel::OnNewSoundTagAnimationseekbar1, VTS_I4)
    ON_EVENT(KSceneModelEditorAnimationPanel, IDC_ANIMATIONSEEKBAR1, 215, KSceneModelEditorAnimationPanel::OnQueryExistTagTypeAnimationseekbar1, VTS_I4 VTS_PI4 VTS_PI4 VTS_PI4)
    ON_EVENT(KSceneModelEditorAnimationPanel, IDC_ANIMATIONSEEKBAR1, 209, KSceneModelEditorAnimationPanel::OnSFXSelectAnimationseekbar1, VTS_I4)
    ON_EVENT(KSceneModelEditorAnimationPanel, IDC_ANIMATIONSEEKBAR1, 225, KSceneModelEditorAnimationPanel::OnSFXTagCopyAnimationseekbar1, VTS_I4 VTS_I4)
    ON_EVENT(KSceneModelEditorAnimationPanel, IDC_ANIMATIONSEEKBAR1, 213, KSceneModelEditorAnimationPanel::OnSFXTagDeleteAnimationseekbar1, VTS_I4)
    ON_EVENT(KSceneModelEditorAnimationPanel, IDC_ANIMATIONSEEKBAR1, 214, KSceneModelEditorAnimationPanel::OnSFXTagFillTagListAnimationseekbar1, VTS_I4 VTS_I4 VTS_PBSTR VTS_PI4 VTS_PI4)
    //ON_EVENT(KSceneModelEditorAnimationPanel, IDC_ANIMATIONSEEKBAR1, 211, KSceneModelEditorAnimationPanel::OnSFXTagLoadAnimationseekbar1, VTS_I4 VTS_PI4 VTS_PR4 VTS_PI4 VTS_PI4 VTS_PI4 VTS_PI4 VTS_PI4 VTS_PBSTR VTS_PBSTR)
    ON_EVENT(KSceneModelEditorAnimationPanel, IDC_ANIMATIONSEEKBAR1, 212, KSceneModelEditorAnimationPanel::OnSFXTagNewAnimationseekbar1, VTS_PI4 VTS_I4)
    //ON_EVENT(KSceneModelEditorAnimationPanel, IDC_ANIMATIONSEEKBAR1, 210, KSceneModelEditorAnimationPanel::OnSFXTagSaveAnimationseekbar1, VTS_I4 VTS_I4 VTS_R4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_BSTR VTS_I4)
    ON_EVENT(KSceneModelEditorAnimationPanel, IDC_ANIMATIONSEEKBAR1, 228, KSceneModelEditorAnimationPanel::OnSoundFileSelectionAnimationseekbar1, VTS_BSTR)
    ON_EVENT(KSceneModelEditorAnimationPanel, IDC_ANIMATIONSEEKBAR1, 226, KSceneModelEditorAnimationPanel::OnSoundTagCopyAnimationseekbar1, VTS_I4 VTS_I4)
    ON_EVENT(KSceneModelEditorAnimationPanel, IDC_ANIMATIONSEEKBAR1, 221, KSceneModelEditorAnimationPanel::OnSoundTagDeleteAnimationseekbar1, VTS_I4)
    //ON_EVENT(KSceneModelEditorAnimationPanel, IDC_ANIMATIONSEEKBAR1, 220, KSceneModelEditorAnimationPanel::OnSoundTagLoadAnimationseekbar1, VTS_I4 VTS_PBSTR VTS_PI4 VTS_PI4 VTS_PI4)
    //ON_EVENT(KSceneModelEditorAnimationPanel, IDC_ANIMATIONSEEKBAR1, 219, KSceneModelEditorAnimationPanel::OnSoundTagSaveAnimationseekbar1, VTS_I4 VTS_BSTR VTS_I4 VTS_I4 VTS_I4)
    ON_EVENT(KSceneModelEditorAnimationPanel, IDC_ANIMATIONSEEKBAR1, 230, KSceneModelEditorAnimationPanel::OnMotionDialogDoModalAnimationseekbar1, VTS_I4)
    ON_EVENT(KSceneModelEditorAnimationPanel, IDC_ANIMATIONSEEKBAR1, 232, KSceneModelEditorAnimationPanel::OnKeyFrameCopyAnimationseekbar1, VTS_I4 VTS_I4 VTS_I4)
    ON_EVENT(KSceneModelEditorAnimationPanel, IDC_ANIMATIONSEEKBAR1, 231, KSceneModelEditorAnimationPanel::OnKeyFrameMoveAnimationseekbar1, VTS_I4 VTS_I4 VTS_I4)
	ON_EVENT(KSceneModelEditorAnimationPanel, IDC_ANIMATIONSEEKBAR1, 220, KSceneModelEditorAnimationPanel::OnSoundTagLoadAnimationseekbar1, VTS_I4 VTS_PI4 VTS_PI4 VTS_PI4 VTS_PI4)
	ON_EVENT(KSceneModelEditorAnimationPanel, IDC_ANIMATIONSEEKBAR1, 219, KSceneModelEditorAnimationPanel::OnSoundTagSaveAnimationseekbar1, VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4)
ON_EVENT(KSceneModelEditorAnimationPanel, IDC_ANIMATIONSEEKBAR1, 229, KSceneModelEditorAnimationPanel::OnPlaySoundFileAnimationseekbar1, VTS_BSTR VTS_R4)
ON_EVENT(KSceneModelEditorAnimationPanel, IDC_ANIMATIONSEEKBAR1, 234, KSceneModelEditorAnimationPanel::OnLoadSoundTagCandidateAnimationseekbar1, VTS_I4 VTS_I4 VTS_PBSTR VTS_PR4 VTS_PR4 VTS_PR4 VTS_PR4)
ON_EVENT(KSceneModelEditorAnimationPanel, IDC_ANIMATIONSEEKBAR1, 233, KSceneModelEditorAnimationPanel::OnSaveSoundTagCandidateAnimationseekbar1, VTS_I4 VTS_I4 VTS_BSTR VTS_R4 VTS_R4 VTS_R4 VTS_R4)
ON_EVENT(KSceneModelEditorAnimationPanel, IDC_ANIMATIONSEEKBAR1, 235, KSceneModelEditorAnimationPanel::OnDeleteSoundTagCandidateAnimationseekbar1, VTS_I4 VTS_I4)
ON_EVENT(KSceneModelEditorAnimationPanel, IDC_ANIMATIONSEEKBAR1, 211, KSceneModelEditorAnimationPanel::OnSFXTagLoadAnimationseekbar1, VTS_I4 VTS_PI4 VTS_PR4 VTS_PI4 VTS_PI4 VTS_PI4 VTS_PI4 VTS_PI4 VTS_PBSTR VTS_PBSTR VTS_PI4)
ON_EVENT(KSceneModelEditorAnimationPanel, IDC_ANIMATIONSEEKBAR1, 210, KSceneModelEditorAnimationPanel::OnSFXTagSaveAnimationseekbar1, VTS_I4 VTS_I4 VTS_R4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_BSTR VTS_I4 VTS_I4)
	ON_EVENT(KSceneModelEditorAnimationPanel, IDC_ANIMATIONSEEKBAR1, 236, KSceneModelEditorAnimationPanel::OnSoundDialogDoModalAnimationseekbar1, VTS_I4)
	ON_EVENT(KSceneModelEditorAnimationPanel, IDC_ANIMATIONSEEKBAR1, 237, KSceneModelEditorAnimationPanel::OnSfxDialogDoModalAnimationseekbar1, VTS_I4)
END_EVENTSINK_MAP()

void KSceneModelEditorAnimationPanel::OnMotionTagCopyAnimationseekbar1(long lSrc, long lDest)
{
	UINT uRetCode = IEIR_FAILED;
	IEKG3DAnimationTag* pTag = GetTag(TAGTYPE_MOTION);
	KG_PROCESS_ERROR(pTag);
	
	uRetCode = pTag->CopyTag(lSrc, lDest);
	if (uRetCode == IEIR_FRAMEOVERWRITEN)
	{
		sprintf_s(s_strOutputInfo,
			MAX_PATH,
			"Frame %d exists already, overwrite?", 
            lDest);
		if (MessageBox(s_strOutputInfo, "Prompt", MB_YESNO) == IDYES)
		{
			pTag->RemoveTag(lDest);
			uRetCode = pTag->CopyTag(lSrc, lDest);
		}
	}
Exit0:
	if (uRetCode == IEIR_FAILED)
	{
		sprintf_s(s_strOutputInfo, 
			MAX_PATH,
			"Copy frame %d to frame %d failed.", 
			lSrc, 
			lDest);
		MessageBox(s_strOutputInfo, "Error", MB_OK);
	}
    else
    {
        UpdateUI();
        m_pContainer->SetModifyState();
    }
}

LRESULT KSceneModelEditorAnimationPanel::OnAnimationSeek(WPARAM wParam, LPARAM lParam)
{
    LRESULT lReturn = 0;
    IKG3DAnimationController* pController = NULL;
    IKG3DModel* pModel = m_pContainer->GetAttachedModel();
    UINT uCurFrame;
    long lWidth;
    CPoistionButton* pCallBackReceiver;
    KG_PROCESS_ERROR(pModel);

    pController = pModel->GetAnimationController(ANICTL_PRIMARY);
    KG_PROCESS_ERROR(pController);
    
    m_AnimationSeekBar.GetGraduationWidth(&lWidth);
    UINT uCurPos = static_cast<UINT>(wParam);
    uCurFrame = uCurPos / lWidth;
    //需要修改dwCurTime = pController->Frame2Time(uCurFrame);
	
	float fRation = uCurFrame / static_cast<float>(m_ctlCursorPosition.m_nMaxFrame);
	
	pController->SeekAnimation(ANIMATION_SEEK_SET, static_cast<int>(pController->GetCycleTime() * fRation));
    pController->PauseAnimation(TRUE);

    pCallBackReceiver = reinterpret_cast<CPoistionButton*>(lParam);
    KG_PROCESS_ERROR(pCallBackReceiver);
    pCallBackReceiver->SendMessage(WM_ANIMATION_SEEK_CALLBACK, uCurFrame);
Exit0:
    return lReturn;
}

void KSceneModelEditorAnimationPanel::OnMotionTagDeleteAnimationseekbar1(long nID)
{
	UINT uRetCode = IEIR_FAILED;
	IEKG3DAnimationTag* pTag = GetTag(TAGTYPE_MOTION);
	KG_PROCESS_ERROR(pTag);

	sprintf_s(s_strOutputInfo, 
		MAX_PATH,
		"Are you sure to delete frame %d ?",
		nID);
	
	if (MessageBox(s_strOutputInfo, "Caution", MB_OKCANCEL) == IDOK)
	{
		uRetCode = pTag->RemoveTag(nID);
	}
Exit0:
	if (uRetCode == IEIR_FAILED)
	{
		sprintf_s(s_strOutputInfo, 
			MAX_PATH,
			"Delete frame %d failed.",
			nID);
		MessageBox(s_strOutputInfo, "Error", MB_OK);
	}
    else
    {
        UpdateUI();
        m_pContainer->SetModifyState();
    }
}

void KSceneModelEditorAnimationPanel::OnMotionTagLoadAnimationseekbar1(long nID, long* lType)
{
	IEKG3DAnimationMotionTag* pTag = dynamic_cast<IEKG3DAnimationMotionTag*>(GetTag(TAGTYPE_MOTION));
	KG_PROCESS_ERROR(pTag);
	*lType = static_cast<long>(pTag->GetMotionTagType(nID));
Exit0:
	;
}

void KSceneModelEditorAnimationPanel::OnMotionTagSaveAnimationseekbar1(long nID, long lType)
{
	UINT uRetCode = IEIR_FAILED;
	MotionTagDataInfo Data;
	IEKG3DAnimationTag* pTag = GetTag(TAGTYPE_MOTION);
	KG_PROCESS_ERROR(pTag);
	Data.Type = static_cast<enuAnimationMotionTagType>(lType);
	uRetCode = pTag->ModifyTag(nID, static_cast<void*>(&Data));
Exit0:
	if (uRetCode == IEIR_FAILED)
	{
		sprintf_s(s_strOutputInfo, 
			MAX_PATH,
			"Save frame %d failed.",
			nID);
		MessageBox(s_strOutputInfo, 
			"Error",
			MB_OK);
	}
    else
    {
        m_pContainer->SetModifyState();
    }
}

void KSceneModelEditorAnimationPanel::OnMotionTagLoadAnimationseekbar(UINT uFrame, MotionTagDataRuntime* &pData)//MotionTagDataInfo& Data)
{
    UINT uRetCode = IEIR_FAILED;
    IEKG3DAnimationMotionTag* pTag = dynamic_cast<IEKG3DAnimationMotionTag*>(GetTag(TAGTYPE_MOTION));
    KG_PROCESS_ERROR(pTag);
	uRetCode = pTag->GetKeyFrame(uFrame, pData);
Exit0:
    ;
}

static HRESULT RegisterOCX(LPCSTR strFileName)
{
    HRESULT hr = E_FAIL;
    BOOL bNeedRelease = FALSE;
    TCHAR strOcxFilePath[MAX_PATH];
    FARPROC lpDllEntryPoint;
    sprintf_s(strOcxFilePath, 
        MAX_PATH,
        "%s%s",
        g_szDefWorkDirectory,
        strFileName);
    //OCX必须和Exe放在一起
    HINSTANCE hLib = LoadLibrary(strOcxFilePath);
    KG_PROCESS_ERROR(hLib != NULL && hLib != (HINSTANCE)HINSTANCE_ERROR);

    bNeedRelease = TRUE;
    lpDllEntryPoint = GetProcAddress(hLib, _T("DllRegisterServer"));
    KG_PROCESS_ERROR(lpDllEntryPoint);

    hr = (*lpDllEntryPoint)();
    KG_COM_PROCESS_ERROR(hr);

    bNeedRelease = FALSE;
    hr = S_OK;
Exit0:
    if (bNeedRelease)
    {
        FreeLibrary(hLib);
    }
    return hr;
}

void KSceneModelEditorAnimationPanel::OnInitialUpdate()
{
    TCHAR strSoundFileStartPath[MAX_PATH];
	CFormView::OnInitialUpdate();
    m_ctlCursorPosition.SetLockState(CPoistionButton::LOCK_HORIZONTAL);
    m_ctlCursorPosition.SetLink(&m_ctlCursorRight);
    m_ctlCursorPosition.SetLink(&m_ctlCursorLeft);
    m_ctlCursorPosition.m_nMax = 500;
    m_ctlCursorPosition.m_nMin = 0;
	RegisterOCX("AnimationSeekBarProj1.ocx");
    sprintf_s(strSoundFileStartPath, 
		MAX_PATH,
		"%sdata\\sound\\",
		g_szDefWorkDirectory);
	m_AnimationSeekBar.SetStartPath(TAGTYPE_SOUND, strSoundFileStartPath);
	InitSound();
}

void KSceneModelEditorAnimationPanel::InitSound()
{
	if (g_pSoundShell)
	{
		g_pSoundShell->Unlock();
	}
}

void KSceneModelEditorAnimationPanel::OnMotionTagSaveAnimationseekbar(UINT uFrame, const MotionTagDataRuntime* &Data)
{
    UINT uRetCode = IEIR_FAILED;
    IEKG3DAnimationMotionTag* pTag = dynamic_cast<IEKG3DAnimationMotionTag*>(GetTag(TAGTYPE_MOTION));
    KG_PROCESS_ERROR(pTag);
    uRetCode = pTag->ModifyTag(uFrame, (void*)(&Data));
Exit0:
    if (uRetCode == IEIR_FAILED)
    {
        sprintf_s(s_strOutputInfo, 
            MAX_PATH,
            "Save frame @ %d failed.",
            uFrame);
        MessageBox(s_strOutputInfo, 
            "Error",
            MB_OK);
    }
    else
    {
        m_pContainer->SetModifyState();
    }
	pTag->ClearUnused();
}

void KSceneModelEditorAnimationPanel::OnNewMotionTagAnimationseekbar1(long nFrame)
{
	UINT uRetCode = IEIR_FAILED;
	UINT uID = ((nFrame << 16) | FRAMEINDEX_NEW);
	MotionTagDataInfo Data;
	strcpy_s(Data.strTagName, TAG_NAME_MAX_LENGTH, "New Motion Tag");
    IEKG3DAnimationTag* pTag = GetTag(TAGTYPE_MOTION);
	KG_PROCESS_ERROR(pTag);
	Data.Type = MTT_USERDEFINE;

	
	sprintf_s(Data.strTagName, 256, "User Define Tag @ %d", nFrame);
	uRetCode = pTag->ModifyTag(uID, static_cast<void*>(&Data));
Exit0:
	if (uRetCode == IEIR_FAILED)
	{
		sprintf_s(s_strOutputInfo, 
			MAX_PATH,
			"New frame @ %d failed.",
			nFrame);
		MessageBox(s_strOutputInfo, 
			"Error",
			MB_OK);
	}
    else
    {
        m_pContainer->SetModifyState();
    }
}

void KSceneModelEditorAnimationPanel::OnNewSFXTagAnimationseekbar1(long nFrame)
{
	UINT uRetCode = IEIR_FAILED;
	SFXKeyFrameInfoVersion1 NewFrame;
    strcpy_s(NewFrame.strTagName, TAG_NAME_MAX_LENGTH, "New SFX Tag");
	IEKG3DAnimationTag* pTag = GetTag(TAGTYPE_SFX);
	KG_PROCESS_ERROR(pTag);
	uRetCode = pTag->ModifyTag(nFrame, static_cast<void*>(&NewFrame));
Exit0:
	if (uRetCode != NewFrame.uID)
	{
		sprintf_s(s_strOutputInfo,
			MAX_PATH,
			"New frame @ %d failed.",
			nFrame);
		MessageBox(s_strOutputInfo,
			"Error",
			MB_OK);
	}
    else
    {
        m_pContainer->SetModifyState();
    }
}


void KSceneModelEditorAnimationPanel::OnNewSoundTagAnimationseekbar1(long nFrame)
{
	UINT uRetCode = IEIR_FAILED;
	SoundDataInfoVersion2 NewFrame;
    strcpy_s(NewFrame.strTagName, TAG_NAME_MAX_LENGTH, "New Sound Tag");
	NewFrame.dwValidateCandidate = 0;
	UINT uId = 0;
	SoundDataInfoVersion3 NewFrameVersion3;
	strcpy_s(NewFrameVersion3.strTagName, TAG_NAME_MAX_LENGTH, "New Sound Tag");
	IEKG3DAnimationTag* pTag = GetTag(TAGTYPE_SOUND);
	KG_PROCESS_ERROR(pTag);
	uId = GETID(nFrame,2);
	uRetCode = pTag->ModifyTag(uId, static_cast<void*>(&NewFrame));
	KG_PROCESS_ERROR(uRetCode);
	uId = GETID(nFrame,3);
	uRetCode = pTag->ModifyTag(uId, static_cast<void*>(&NewFrameVersion3));
	KG_PROCESS_ERROR(uRetCode);
Exit0:
	if (uRetCode == IEIR_FAILED)
	{
		sprintf_s(s_strOutputInfo,
			MAX_PATH,
			"New sound frame %d failed.",
			nFrame);
		MessageBox(s_strOutputInfo,
			"Error",
			MB_OK);
	}
    else
    {
        m_pContainer->SetModifyState();
    }
}

void KSceneModelEditorAnimationPanel::OnQueryExistTagTypeAnimationseekbar1(long lFrame, long* bSFXTag, long* bSoundTag, long* bMotionTag)
{
	*bSFXTag = 0;
	*bSoundTag = 0;
	*bMotionTag = 0;
	IEKG3DAnimationTag* pTag = GetTag(TAGTYPE_MOTION);
	if (pTag)
	{
		*bMotionTag = pTag->GetNumKeyFrame(lFrame);
	}
	
	pTag = GetTag(TAGTYPE_SOUND);
	if (pTag)
	{
		*bSoundTag = pTag->GetNumKeyFrame(lFrame);
	}

	pTag = GetTag(TAGTYPE_SFX);
	if (pTag)
	{
		*bSFXTag = pTag->GetNumKeyFrame(lFrame);
	}
}

void KSceneModelEditorAnimationPanel::OnSFXSelectAnimationseekbar1(long uID)
{
    HRESULT hr = E_FAIL;
    UINT uFrame = 0;
    UINT uIndex = 0;
    IKG3DModel *pModel = NULL;
    IKG3DModel *pBindInfo = NULL;
    IEKG3DAnimationSFXTag* pTag = NULL;
    MFCFramework::KSceneEditorApp* pApp = NULL;
    MFCFramework::CMainFrame* pMainWnd = NULL;
    KSceneModelEditorFrameWnd* pFrameWnd = NULL;
    const SFXKeyFrameInfoVersion1* pKeyFrameData = NULL;
    CWnd* pInvoker = NULL;
    
    KG_PROCESS_ERROR(m_pContainer);
    
    pModel = m_pContainer->GetAttachedModel();
    KG_PROCESS_ERROR(pModel);
    
    pTag = dynamic_cast<IEKG3DAnimationSFXTag*>(GetTag(TAGTYPE_SFX));
	KG_PROCESS_ERROR(pTag);
    
    hr = pTag->GetKeyFrameData(uID, &pKeyFrameData);
    KG_COM_PROCESS_ERROR(hr);

    pApp = static_cast<MFCFramework::KSceneEditorApp*>(AfxGetApp());
    KG_PROCESS_ERROR(pApp);

    hr = pTag->GetBindInfoInstance(&pBindInfo);
    KG_COM_PROCESS_ERROR(hr);
        
    hr = pTag->GetTagFrameIndexByID(uID, &uFrame, &uIndex);
    KG_COM_PROCESS_ERROR(hr);

    m_nLastSFXTagDialog = static_cast<int>(uFrame);

    pMainWnd = static_cast<MFCFramework::CMainFrame*>(AfxGetMainWnd());
    KG_PROCESS_ERROR(pMainWnd);

    pFrameWnd = static_cast<KSceneModelEditorFrameWnd*>(pMainWnd->GetActiveFrame());
    KG_PROCESS_ERROR(pFrameWnd);

    pInvoker = (CWnd*)(pFrameWnd->GetModelEditorView());
    KG_PROCESS_ERROR(pInvoker);

    pApp->NewSFXEditorToModifyBindInfo(pModel, 
        pBindInfo, 
        pKeyFrameData->uBindFileIndex, 
        pInvoker, 
        m_strSFXStartPath,
        g_pGraphicsTool->GetNowTime());
Exit0:
	;
}

void KSceneModelEditorAnimationPanel::OnSFXTagCopyAnimationseekbar1(long lSrc, long lDest)
{
	HRESULT hr = E_FAIL;
	UINT uRetCode = IEIR_FAILED;
	IEKG3DAnimationSFXTag* pTag = dynamic_cast<IEKG3DAnimationSFXTag*>(GetTag(TAGTYPE_SFX));
	KG_PROCESS_ERROR(pTag);
	
	uRetCode = pTag->CopyTag(lSrc, lDest);
Exit0:
	if (uRetCode == IEIR_FAILED)
	{
		sprintf_s(s_strOutputInfo,
			MAX_PATH,
			"Copy from frame %d to %d failed.",
			lSrc,
			lDest);
		MessageBox(s_strOutputInfo, "Error", MB_OK);
	}
    else
    {
        m_pContainer->SetModifyState();
    }
}

void KSceneModelEditorAnimationPanel::OnSFXTagDeleteAnimationseekbar1(long nID)
{
	UINT uRetCode = IEIR_FAILED;
	UINT uFrame = 0;
    UINT uIndex = 0;
	HRESULT hr = E_FAIL;
	IEKG3DAnimationSFXTag* pTag = dynamic_cast<IEKG3DAnimationSFXTag*>(GetTag(TAGTYPE_SFX));
	KG_PROCESS_ERROR(pTag);

	hr = pTag->GetTagFrameIndexByID(nID, &uFrame, &uIndex);
	KG_COM_PROCESS_ERROR(hr);

	uRetCode = pTag->RemoveTag(nID);
Exit0:
	if (uRetCode == IEIR_FAILED)
	{
		sprintf_s(s_strOutputInfo,
			MAX_PATH,
			"Delete frame %d failed.",
			uFrame);
		MessageBox(s_strOutputInfo, "Error", MB_OK);
	}
    else
    {
        m_pContainer->SetModifyState();
    }
}

void KSceneModelEditorAnimationPanel::OnSFXTagFillTagListAnimationseekbar1(long nFrame, 
																		   long nCount, 
																		   BSTR* strTagName, 
																		   long* nID, 
																		   long* nReturn)
{
	UINT uNumKeyFrames = 0;
    LPCSTR TagName;
	SFXKeyFrameInfoVersion1 KeyFrame;
	*nReturn = 0;

    IEKG3DAnimationSFXTag* pTag = dynamic_cast<IEKG3DAnimationSFXTag*>(GetTag(TAGTYPE_SFX));
	KG_PROCESS_ERROR(pTag);
	
	uNumKeyFrames = pTag->GetNumKeyFrame(nFrame);
	if (nCount < static_cast<int>(uNumKeyFrames))
	{
		UINT uID;
        pTag->GetTagIDByFrameIndex(nFrame, nCount, &uID);
        pTag->GetKeyFrameName(uID, &TagName);
        *nID = static_cast<long>(uID);
        *strTagName = T2BSTR(TagName);
        *nReturn = 1;
	}
Exit0:
	;
}

//void KSceneModelEditorAnimationPanel::OnSFXTagLoadAnimationseekbar1(long nID, 
//                                                                    long* nPlayType, 
//                                                                    float* fPlaySpeed, 
//                                                                    long* nAutoAdaptSpeed, 
//                                                                    long* nInterruptable, 
//                                                                    long* nKeepMotionFinish, 
//                                                                    long* nNotify, 
//                                                                    long* nBindIndex, 
//                                                                    BSTR* strSFXFileName, 
//                                                                    BSTR* strTagName)
//{
//	HRESULT hr = E_FAIL;
//    const SFXKeyFrameInfoVersion1* pKeyFrame = NULL;
//    IEKG3DAnimationSFXTag* pTag = dynamic_cast<IEKG3DAnimationSFXTag*>(GetTag(TAGTYPE_SFX));
//	LPCSTR cstrSFXFileName;
//    LPCSTR cstrTagName;
//    KG_PROCESS_ERROR(pTag);
//
//    hr = pTag->GetKeyFrameName(nID, &cstrTagName);
//    KG_COM_PROCESS_ERROR(hr);
//    *strTagName = T2BSTR(cstrTagName);
//
//    hr = pTag->GetKeyFrameData(nID, &pKeyFrame);
//    KG_COM_PROCESS_ERROR(hr);
//
//    
//    *nPlayType = static_cast<long>(pKeyFrame->FinishState);
//    *fPlaySpeed = static_cast<float>(pKeyFrame->fSpeed);
//    *nAutoAdaptSpeed = static_cast<long>(pKeyFrame->bAdaptSpeedToMotion);
//    *nInterruptable = static_cast<long>(pKeyFrame->bInterruptable);
//    *nKeepMotionFinish = static_cast<long>(pKeyFrame->MotionFinishEvent);
//    *nNotify = static_cast<long>(pKeyFrame->bNotify);
//    *nBindIndex = static_cast<long>(pKeyFrame->uBindFileIndex);
//    
//    hr = pTag->GetSFXFileName(pKeyFrame->uBindFileIndex, &cstrSFXFileName);
//    KG_COM_PROCESS_ERROR(hr);
//
//    *strSFXFileName = T2BSTR(cstrSFXFileName);
//Exit0:
//	;
//}

void KSceneModelEditorAnimationPanel::OnSFXTagNewAnimationseekbar1(long* nID, long nFrame)
{
	UINT uRetCode = IEIR_FAILED;
    SFXKeyFrameInfoVersion1 NewFrame;
    strcpy_s(NewFrame.strTagName, TAG_NAME_MAX_LENGTH, "New SFX Tag");
    UINT uID;
    *nID = 0xfffffff;
    IEKG3DAnimationSFXTag* pTag = dynamic_cast<IEKG3DAnimationSFXTag*>(GetTag(TAGTYPE_SFX));
	KG_PROCESS_ERROR(pTag);
    
    uRetCode = pTag->ModifyTag(nFrame, static_cast<void*>(&NewFrame));
    KG_PROCESS_ERROR(uRetCode != IEIR_FAILED);

    uRetCode = pTag->GetNumKeyFrame(nFrame);
    ASSERT(uRetCode != 0);

    if (SUCCEEDED(pTag->GetTagIDByFrameIndex(nFrame, uRetCode - 1, &uID)))
    {
        uRetCode = static_cast<UINT>(IEIR_SUCCESS);
        *nID = static_cast<long>(uID);
    }
    else
    {
        uRetCode = IEIR_FAILED;
    }
Exit0:
	if (uRetCode == IEIR_FAILED)
    {
        sprintf_s(s_strOutputInfo, MAX_PATH, "Create new sfx tag @ frame %d failed.", nFrame);
        MessageBox(s_strOutputInfo, "Error", MB_OK);
    }
    else
    {
        m_pContainer->SetModifyState();
    }
}

//void KSceneModelEditorAnimationPanel::OnSFXTagSaveAnimationseekbar1(long nTagID, 
//                                                                    long nPlayType, 
//                                                                    float fPlaySpeed,
//                                                                    long nAutoAdaptSpeed,
//                                                                    long nInterruptable,
//                                                                    long nKeepMotionFinish,
//                                                                    long nNotify, 
//                                                                    LPCTSTR strTagName,
//                                                                    long nBindID)
//{
//	HRESULT hr = E_FAIL;
//    UINT uFrame = 0;
//    UINT uIndex = 0;
//    SFXKeyFrameInfo ModifyFrame;
//    IEKG3DAnimationSFXTag* pTag = dynamic_cast<IEKG3DAnimationSFXTag*>(GetTag(TAGTYPE_SFX));
//	KG_PROCESS_ERROR(pTag);
//
//    ModifyFrame.bAdaptSpeedToMotion = static_cast<BOOL>(nAutoAdaptSpeed);
//    ModifyFrame.bInterruptable = static_cast<BOOL>(nInterruptable);
//    ModifyFrame.bNotify = static_cast<BOOL>(nNotify);
//    ModifyFrame.FinishState = static_cast<SFXFinishState>(nPlayType);
//    ModifyFrame.fSpeed = fPlaySpeed;
//    ModifyFrame.MotionFinishEvent = static_cast<SFXMotionFinshState>(nKeepMotionFinish);
//    strcpy_s(ModifyFrame.strTagName, 256, strTagName);
//
//    hr = pTag->GetTagFrameIndexByID(nTagID, &uFrame, &uIndex);
//    KG_COM_PROCESS_ERROR(hr);
//
//    ModifyFrame.uID = nTagID;
//    ModifyFrame.uBindFileIndex = nBindID;
//    
//    UINT uRetCode= pTag->ModifyTag(uFrame, static_cast<void*>(&ModifyFrame));
//    uRetCode == ModifyFrame.uID ? hr = S_OK : hr = E_FAIL;
// Exit0:
//	if (FAILED(hr))
//    {
//        sprintf_s(s_strOutputInfo, 
//            MAX_PATH,
//            "Save frame @ %d , Index %d failed.",
//            uFrame,
//            uIndex);
//        MessageBox(s_strOutputInfo, "Error", MB_OK);
//    }
//    else
//    {
//        m_pContainer->SetModifyState();
//    }
//}

void KSceneModelEditorAnimationPanel::OnSoundFileSelectionAnimationseekbar1(LPCTSTR strFilePath)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(g_pSoundShell);
	
	g_pSoundShell->StopAllSound();
	hr = g_pSoundShell->PlaySound(1, strFilePath, 0, 1.0f, FALSE, NULL);

Exit0:
	if (FAILED(hr))
    {
        sprintf_s(s_strOutputInfo, 
            MAX_PATH,
            "Play sound file %s failed.",
            strFilePath);
        MessageBox(strFilePath, "Error");
    }
    ;
}

void KSceneModelEditorAnimationPanel::OnSoundTagCopyAnimationseekbar1(long lSrc, long lDest)
{
	UINT uRetCode = IEIR_FAILED;
    IEKG3DAnimationTag* pTag = GetTag(TAGTYPE_SOUND);
	KG_PROCESS_ERROR(pTag);
    uRetCode = pTag->CopyTag(lSrc, lDest);
    if (uRetCode == IEIR_FRAMEOVERWRITEN)
    {
        sprintf_s(s_strOutputInfo, 
            MAX_PATH,
            "Frame @ %d exist, overwrite it?",
            lDest);
        if (MessageBox(s_strOutputInfo, "Caution", MB_OKCANCEL) == IDOK)
        {
            uRetCode = pTag->RemoveTag(lDest);
            uRetCode = pTag->CopyTag(lSrc, lDest);
        }
    }
Exit0:
	if (uRetCode == IEIR_FAILED)
    {
        sprintf_s(s_strOutputInfo, 
            MAX_PATH,
            "Copy from frame %d to %d failed.",
            lSrc, 
            lDest);
        MessageBox(s_strOutputInfo, "Error");
    }
	else
	{
		UpdateUI();
        m_pContainer->SetModifyState();
	}
}

void KSceneModelEditorAnimationPanel::OnSoundTagDeleteAnimationseekbar1(long nID)
{
	UINT uRetCode = IEIR_FAILED;
    IEKG3DAnimationTag* pTag = GetTag(TAGTYPE_SOUND);
	KG_PROCESS_ERROR(pTag);
    uRetCode = pTag->RemoveTag(nID);
Exit0:
	if (uRetCode == IEIR_FAILED)
    {
        sprintf_s(s_strOutputInfo, 
            MAX_PATH,
            "Sound Tag @ %d delete failed.",
            nID);
        MessageBox(s_strOutputInfo, "Error", MB_OK);
    }
	else
	{
		UpdateUI();
        m_pContainer->SetModifyState();
	}
}


IEKG3DAnimationTag* KSceneModelEditorAnimationPanel::GetTag(enuTagType Type)
{
	IEKG3DAnimationTag* pReturn = NULL;
	KG_PROCESS_ERROR(m_pContainer);
	pReturn = m_pContainer->GetTag(Type);
Exit0:
	return pReturn;
}

void KSceneModelEditorAnimationPanel::OnSFXEditorClose()
{
    if (m_nLastSFXTagDialog != -1)

    {
        m_AnimationSeekBar.OpenSFXTagEditorDlg(m_nLastSFXTagDialog);
    }
}

void KSceneModelEditorAnimationPanel::OnBnClickedButtonPlay()
{
    static int i = 0;
	KG_PROCESS_ERROR(m_pContainer);
    m_pContainer->Start(1.0f, 0, ENU_ANIMATIONPLAY_ONCE, ANICTL_PRIMARY);
Exit0:
    ;
}

void KSceneModelEditorAnimationPanel::OnBnClickedButtonSave()
{
	HRESULT hr = E_FAIL;
    LPCSTR strFilter = "Animation Tag Files (*.tani)|*.tani||";
    CFileDialog dlgSave(FALSE, 
        ".tani", "Noname", 4 | 2, strFilter, this);
    KG_PROCESS_ERROR(m_pContainer);
    KG_PROCESS_SUCCESS(dlgSave.DoModal() == IDCANCEL);
    hr = m_pContainer->Save(dlgSave.GetPathName());
    KG_COM_PROCESS_ERROR(hr);

	UpdateUI();
Exit1:
    hr = S_OK;
Exit0:
    if (FAILED(hr))
    {
        MessageBox("Save Tag file failed.");
    }
}

void KSceneModelEditorAnimationPanel::GetAnimationDefaultPath(LPSTR strAnimationPath)
{
	const TCHAR *strModelName = NULL;
	TCHAR strModelPath[MAX_PATH];
	
	KG_PROCESS_ERROR(m_pContainer);
	if (m_PatternID != KDlgPlayerCheckTool::INVALIDATEPATTERN)
	{
		//如果有设置PatternID, 说明是从MDL检查工具打开的, 动作的路径使用Pattern的
		sprintf_s(strAnimationPath, 
			MAX_PATH,
			"%s%s%s\\动作\\",
			g_szDefExeDirectory,
			s_strStartPath,
			KDlgPlayerCheckTool::m_PatternPathName[m_PatternID].c_str());
	}
	else
	{
		IKG3DModel *pModel = NULL;
		pModel = m_pContainer->GetAttachedModel();
		KG_PROCESS_ERROR(pModel);

		pModel->GetName(&strModelName);
		strcpy_s(strModelPath,
			MAX_PATH,
			strModelName);
		
		_strlwr_s(strModelPath, MAX_PATH);
		for (size_t i = 0; i < sizeof(g_ModelAnimationKeywords) / sizeof(const TCHAR*); i++)
		{
			TCHAR *strModify = strstr(strModelPath, g_ModelAnimationKeywords[i]);
			if (strModify)
			{
				strModify[0] = '\0';
				strcat(strModify, "动作\\");
				break;
			}
		}
		strcpy_s(strAnimationPath, 
			MAX_PATH,
			strModelPath);
	}
Exit0:
	;
}

void KSceneModelEditorAnimationPanel::OnLoadTagAnimation(LPCSTR strFileName)
{
	HRESULT hResult = E_FAIL;
	IKG3DModel *pModel = NULL;
	CString strPathName;
	IEKG3DAnimationMotionTag* AniMotionTag = NULL;
	vector<string> vecSocketName;
	IEKG3DModel* piModel = NULL;
	KG_PROCESS_ERROR(m_pContainer);

	pModel = m_pContainer->GetAttachedModel();
	KG_PROCESS_ERROR(pModel);
	
	m_AnimationSeekBar.Clear();
	hResult = m_pContainer->Load(strFileName);
	KG_COM_PROCESS_ERROR(hResult);
	m_strCurTaniName = strFileName;
	m_pContainer->AttachToModel(pModel);
	GET_MODEL_EDITOR();
	AniMotionTag = static_cast<IEKG3DAnimationMotionTag*>(m_pContainer->GetTag(TAGTYPE_MOTION));
	int nSocket = AniMotionTag->GetBindedSocketName(vecSocketName);	
	pEditor->ReleaseSFXMeteor();
	pEditor->GetCurModel(&piModel);
	strPathName.Format("%sdata\\public\\Meteor\\Meteor.Sfx",g_szDefWorkDirectory);
	for (int i=0; i< nSocket;i++ )
	{
		hResult = pEditor->LoadSFXMeteor(strPathName,vecSocketName[i].c_str(),piModel);
		if (FAILED(hResult))
		{
			MessageBox(strPathName, "Error");
		}
	}
	piModel->GetAnimationTagContainer(&m_pContainer);
	UpdateUI();

	hResult = S_OK;
Exit0:
	if (FAILED(hResult))
	{
		MessageBox("Load Tag file failed.");
	}
}

void KSceneModelEditorAnimationPanel::OnBnClickedButtonLoad()
{
    TCHAR strDefaultPath[MAX_PATH];
	LPCSTR strFilter = "Animation Tag Files (*.tani)|*.tani||";
    CFileDialog dlgLoad(TRUE, 
        ".tani", 
		"Noname", 
		4 | 2, 
		strFilter, 
		this);
	KG_PROCESS_ERROR(m_pContainer);
	GetAnimationDefaultPath(strDefaultPath);
	dlgLoad.m_ofn.lpstrInitialDir = strDefaultPath;
	KG_PROCESS_SUCCESS(dlgLoad.DoModal() == IDCANCEL);
	OnLoadTagAnimation(dlgLoad.GetPathName());
Exit1:
Exit0:
	;
}

void KSceneModelEditorAnimationPanel::SetAnimation(LPCSTR strFileName)
{
	HRESULT hr = E_FAIL;
    IEKG3DClip* pClip = NULL;
    long lWidth;
    IEKG3DEngineManager* pEngingManager = NULL;
	IKG3DAnimationController* pController = NULL;
	IKG3DModel* pModel = NULL;

	KG_PROCESS_ERROR(m_pContainer);

	if (m_pContainer->GetModifyState())
    {
        if (MessageBox("动作标签已经修改, 是否要保存?", "提示", MB_YESNO) == IDYES)
        {
            OnBnClickedButtonSave();
        }
    }
    m_pContainer->Clear();
    m_pContainer->SetAnimation(strFileName);
    
    pEngingManager = dynamic_cast<IEKG3DEngineManager*>(g_GetEngineManager());
    KG_PROCESS_ERROR(pEngingManager);

	pModel = m_pContainer->GetAttachedModel();
	KG_PROCESS_ERROR(pModel);

	pController = pModel->GetAnimationController(ANICTL_PRIMARY);
	KG_PROCESS_ERROR(pController);

	DWORD dwPlayType = pController->GetPlayType();
	m_pContainer->Start(1.0f, 0, dwPlayType, ANICTL_PRIMARY);
	
	UpdateUI();
	m_AnimationSeekBar.GetGraduationWidth(&lWidth);
    m_ctlCursorPosition.OnMouseMove(0, CPoint(lWidth / 2, 0));
   
Exit0:
    ;
}

void KSceneModelEditorAnimationPanel::UpdateUI()
{
    HRESULT hr = E_FAIL;
    KG3DAnimationContainer Container;
    IEKG3DClip* pClip = NULL;
    LPCSTR strAnimationFileName = NULL;
    DWORD dwNumFrames = 0;
	long lWidth;

    KG_PROCESS_ERROR(m_pContainer);
    
    strAnimationFileName = m_pContainer->GetAnimationFilePath();
    hr = Container.LoadAnimationFromFile(strAnimationFileName, 0);
    KG_COM_PROCESS_ERROR(hr);

    hr = Container.GetAnimation(0, &pClip);
    KG_COM_PROCESS_ERROR(hr);

    hr = pClip->GetNumFrames(&dwNumFrames);
    KG_COM_PROCESS_ERROR(hr);

    m_AnimationSeekBar.SetRange(max(0, dwNumFrames - 1));
    m_AnimationSeekBar.Refresh();

	m_ctlCursorPosition.m_nMaxFrame = static_cast<int>(dwNumFrames - 1);
	m_AnimationSeekBar.GetGraduationWidth(&lWidth);
	m_ctlCursorPosition.m_nMin = 0 + lWidth / 2;
	m_ctlCursorPosition.m_nMax = (dwNumFrames - 1) * lWidth + lWidth / 2;
Exit0:
    ;
}

void KSceneModelEditorAnimationPanel::OnMotionDialogDoModalAnimationseekbar1(long nFrame)
{
    KDlgMotionTag dlgMotionTag(dynamic_cast<IEKG3DAnimationMotionTag*>(m_pContainer->GetTag(TAGTYPE_MOTION)), this);
    dlgMotionTag.m_uFrame = static_cast<int>(nFrame);
    dlgMotionTag.m_pAnimationPanel = this;
	IKG3DModel *pModel = m_pContainer->GetAttachedModel();
	if (pModel)
	{
		std::vector<IKG3DModel*> vecFaces;
		pModel->GetBindToSocketModel("s_face", vecFaces);
		for (size_t i = 0; i < vecFaces.size(); i++)
		{
			if (vecFaces[i])
			{
				TCHAR strLower[MAX_PATH];
				LPCSTR strFaceName = NULL;
				vecFaces[i]->GetName(&strFaceName);
				strcpy_s(strLower, MAX_PATH, strFaceName);
				_strlwr_s(strLower, MAX_PATH);
				if (strstr(strLower, "_face"))
				{
					dlgMotionTag.m_strCurrentFacePath = strFaceName;
					break;
				}
			}
		}
	}
	dlgMotionTag.DoModal();
}

void KSceneModelEditorAnimationPanel::OnBnClickedButtonLeft()
{
	long lStep = 0;
	RECT rectCursor;
	m_AnimationSeekBar.GetGraduationWidth(&lStep);
	m_ctlCursorPosition.GetWindowRect(&rectCursor);
	m_ctlCursorPosition.OnLButtonDown(0, CPoint(rectCursor.left, rectCursor.top));
	m_ctlCursorPosition.OnMouseMove(0, CPoint(rectCursor.left - lStep, rectCursor.top));
	m_ctlCursorPosition.OnLButtonUp(0, CPoint(0, 0));
}

void KSceneModelEditorAnimationPanel::OnBnClickedButtonRight()
{
    long lStep = 0;
	RECT rectCursor;
	m_AnimationSeekBar.GetGraduationWidth(&lStep);
	m_ctlCursorPosition.GetWindowRect(&rectCursor);
	m_ctlCursorPosition.OnLButtonDown(0, CPoint(rectCursor.left, rectCursor.top));
	m_ctlCursorPosition.OnMouseMove(0, CPoint(rectCursor.left + lStep, rectCursor.top));
	m_ctlCursorPosition.OnLButtonUp(0, CPoint(0, 0));
}

BEGIN_MESSAGE_MAP(CPoistionButton, CButton)
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_MESSAGE(WM_ANIMATION_SEEK_CALLBACK, &CPoistionButton::OnAnimationSeekCallBack)
END_MESSAGE_MAP()

LRESULT CPoistionButton::OnAnimationSeekCallBack(WPARAM wParam, LPARAM lParam)
{
    TCHAR strFrameInfo[MAX_PATH];
    m_uCurFrame = static_cast<UINT>(wParam);
    sprintf_s(strFrameInfo, 
        MAX_PATH,
        "%d/%d",
        m_uCurFrame, 
        m_nMaxFrame);
    SetWindowText(strFrameInfo);
    return 0;
}

void CPoistionButton::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    m_bSetCapture = TRUE;
    m_ptCaptureStartPoint = point;
    SetCapture();
    CButton::OnLButtonDown(nFlags, point);
}

void CPoistionButton::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (!m_bMoved)
	{
		CPoint ptScreen = point;
		ClientToScreen(&ptScreen);
		static_cast<KSceneModelEditorAnimationPanel*>(GetParent())->m_AnimationSeekBar.ShowMenu(ptScreen.x, ptScreen.y, m_uCurFrame);
	}
	m_bMoved = FALSE;
	m_bSetCapture = FALSE; 
    ReleaseCapture();
    CButton::OnLButtonUp(nFlags, point);
}

static void MoveWindowByOffset(CWnd* pWnd, CPoint& ptOffset)
{
    CRect Rect;
    pWnd->GetWindowRect(&Rect);
    int nWidth = Rect.right - Rect.left;
    int nHeight = Rect.bottom - Rect.top;
    Rect.left += ptOffset.x;
    Rect.top += ptOffset.y;
    Rect.bottom = Rect.top + nHeight;
    Rect.right = Rect.left + nWidth;
    CWnd* pParent = pWnd->GetParent();
    if (pParent)
    {
        pParent->ScreenToClient(&Rect);
    }
    pWnd->MoveWindow(&Rect, FALSE);
}

void CPoistionButton::OnMouseMove(UINT nFlags, CPoint point)
{
    CButton::OnMouseMove(nFlags, point);
    if (m_bSetCapture)
    {
        m_bMoved = TRUE;
		CRect RectOrg;
        GetWindowRect(&RectOrg);
        CPoint ptOffset = CPoint(point.x - m_ptCaptureStartPoint.x, point.y - m_ptCaptureStartPoint.y);
        switch (m_LockState)
        {
        case LOCK_NONE:
            break;
        case LOCK_VERTICAL:
            ptOffset.x = 0;
            break;
        case LOCK_HORIZONTAL:
            ptOffset.y = 0;
            break;
        }
        int nCurPos = GetCurrentPosition();
        if (nCurPos + ptOffset.x <= m_nMin)
           ptOffset.x = m_nMin - nCurPos;
        if (nCurPos + ptOffset.x >= m_nMax)
           ptOffset.x = m_nMax - nCurPos;
        MoveWindowByOffset(this, ptOffset);
        ChildWndTable::iterator i = m_Children.begin();
        while (i != m_Children.end())
        {
            MoveWindowByOffset(*i, ptOffset);
            i++;
        }
        CWnd* pParent = GetParent();
        if (pParent)
        {
            CRect RectCur;
            GetWindowRect(&RectCur);
            CRect RectInvalidate(CPoint(0, min(RectOrg.top, RectCur.top)),
                CPoint(1280, max(RectOrg.bottom, RectCur.bottom)));
            pParent->ScreenToClient(&RectInvalidate);
            pParent->InvalidateRect(&RectInvalidate);
            pParent->SendMessage(WM_ANIMATION_SEEK,WPARAM(nCurPos + ptOffset.x), LPARAM(this));
        }
    }
}

CPoistionButton::CPoistionButton()
{
    m_bSetCapture = FALSE;
	m_bMoved = FALSE;
	m_uCurFrame = 0;
}

CPoistionButton::~CPoistionButton()
{

}

void CPoistionButton::SetLink(CWnd* pChild)
{
    m_Children.insert(pChild);
}

int CPoistionButton::GetCurrentPosition()
{
    int nReturn = 0;
    CRect Rect;
    GetWindowRect(&Rect);
    nReturn = Rect.left;
    ChildWndTable::iterator i = m_Children.begin();
    while (i != m_Children.end())
    {
        (*i)->GetWindowRect(&Rect);
        if (nReturn > Rect.left)
            nReturn = Rect.left;
        i++;
    }
    CWnd* pParent = GetParent();
    if (pParent)
    {
        CPoint ptTranslated = CPoint(nReturn, 0);
        pParent->ScreenToClient(&ptTranslated);
        nReturn = ptTranslated.x;
    }
    return nReturn;
}

void KSceneModelEditorAnimationPanel::OnKeyFrameCopyAnimationseekbar1(long lType, long lID, long lDest)
{
    IEKG3DAnimationTag* pTag = m_pContainer->GetTag(static_cast<enuTagType>(lType));
    KG_PROCESS_ERROR(pTag);
    pTag->CopyTag(lID, lDest);
    UpdateUI();
    m_pContainer->SetModifyState();
Exit0:
    ;
}

void KSceneModelEditorAnimationPanel::OnKeyFrameMoveAnimationseekbar1(long lType, long lID, long FrameDest)
{
    UINT uRetCode = FrameDest;
    IEKG3DAnimationTag* pTag = m_pContainer->GetTag(static_cast<enuTagType>(lType));
    KG_PROCESS_ERROR(pTag);
    uRetCode = pTag->MoveTag(lID, FrameDest);
    if (uRetCode == (UINT)FrameDest)
    {
        pTag->RemoveTag(FrameDest);
        pTag->MoveTag(lID, FrameDest);
    }
    UpdateUI();
    m_pContainer->SetModifyState();
Exit0:
    ;
}

void KSceneModelEditorAnimationPanel::OnSoundTagLoadAnimationseekbar1(long lID, 
																	  long* bLoop, 
																	  long* bKeep, 
																	  long* bInterruptable, 
																	  long* lNumCanidate)
{
	HRESULT hr = E_FAIL;
	const SoundDataInfoVersion2* pKeyFrameData = NULL;
	IEKG3DAnimationSoundTag* pTag = dynamic_cast<IEKG3DAnimationSoundTag*>(GetTag(TAGTYPE_SOUND));
	KG_PROCESS_ERROR(pTag);

	hr = pTag->GetKeyFrameData(lID, &pKeyFrameData);
	KG_COM_PROCESS_ERROR(hr);

	*bLoop = pKeyFrameData->bLoop;
	*bKeep = (pKeyFrameData->MotionChangeState == SMCS_KEEP);
	*bInterruptable = (pKeyFrameData->InterruptMode == SPI_ENABLE);

	*lNumCanidate = static_cast<long>(pKeyFrameData->dwValidateCandidate);

	for (DWORD i = 0; i < pKeyFrameData->dwValidateCandidate; i++)
	{
		const SoundTagSoundFileCandidate1& Candidate = pKeyFrameData->SoundCandidates[i];
		m_AnimationSeekBar.LoadSoundCandidate(i, 
			Candidate.strSoundFileName.strFileName, 
			Candidate.fHitRate, 
			Candidate.fVolume);
	}
	hr = S_OK;
Exit0:
	if (FAILED(hr))
	{
		sprintf_s(s_strOutputInfo, MAX_PATH, "Load Sound Tag @ Frame %d failed", lID);
		MessageBox(s_strOutputInfo, "Error");
	}	
}

void KSceneModelEditorAnimationPanel::OnSoundTagSaveAnimationseekbar1(long lID, 
																	  long lLoop, 
																	  long lKeep, 
																	  long lInterruptable, 
																	  long lNumCandidate)
{
	UINT uRetCode = IEIR_FAILED;
	SoundDataInfoVersion2 ModifyFrame;
	const SoundDataInfoVersion2 *pOrgData = NULL;
	IEKG3DAnimationSoundTag* pTag = dynamic_cast<IEKG3DAnimationSoundTag*>(GetTag(TAGTYPE_SOUND));
	KG_PROCESS_ERROR(pTag);

	pTag->GetKeyFrameData(lID, &pOrgData);

	lInterruptable ? ModifyFrame.InterruptMode = SPI_ENABLE : ModifyFrame.InterruptMode = SPI_DISABLE;
	lKeep ? ModifyFrame.MotionChangeState = SMCS_KEEP : ModifyFrame.MotionChangeState = SMCS_DELETE;
	ModifyFrame.bLoop = lLoop;
	ModifyFrame.dwValidateCandidate = static_cast<DWORD>(lNumCandidate);
	memcpy_s(&ModifyFrame.SoundCandidates, 
		sizeof(SoundTagSoundFileCandidate1) * ModifyFrame.dwValidateCandidate,
		&pOrgData->SoundCandidates,
		sizeof(SoundTagSoundFileCandidate1) * ModifyFrame.dwValidateCandidate);
	uRetCode = pTag->ModifyTag(lID, static_cast<SoundDataInfoVersion2*>(&ModifyFrame));
Exit0:
	if (uRetCode == IEIR_FAILED)
	{
		sprintf_s(s_strOutputInfo, 
			MAX_PATH,
			"Save sound tag @ frame %d failed.", 
			lID);
		MessageBox(s_strOutputInfo, "Error");
	};
}

void KSceneModelEditorAnimationPanel::OnPlaySoundFileAnimationseekbar1(LPCTSTR strSoundFilePath, float fVolume)
{
	HRESULT hr = E_FAIL;

	KG_PROCESS_ERROR(g_pSoundShell);

	g_pSoundShell->StopAllSound();
	hr = g_pSoundShell->PlaySound(1, strSoundFilePath, 0, fVolume / 100.0f, FALSE, NULL);

Exit0:
	if (FAILED(hr))
	{
		sprintf_s(s_strOutputInfo, 
			MAX_PATH,
			"Play sound file %s failed.",
			strSoundFilePath);
		MessageBox(s_strOutputInfo, "Error");
	}
	;	
}

void KSceneModelEditorAnimationPanel::OnLoadSoundTagCandidateAnimationseekbar1(long lFrame, 
																			   long lIndex, 
																			   BSTR* strSoundFileName, 
																			   float* fHitRate, 
																			   float* fVolume, 
																			   float* fMinDis, 
																			   float* fMaxDis)
{
	HRESULT hr = E_FAIL;
	const SoundDataInfoVersion2* pKeyFrameData = NULL;
	IEKG3DAnimationSoundTag* pTag = dynamic_cast<IEKG3DAnimationSoundTag*>(GetTag(TAGTYPE_SOUND));
	KG_PROCESS_ERROR(pTag);

	hr = pTag->GetKeyFrameData(lFrame, &pKeyFrameData);
	KG_COM_PROCESS_ERROR(hr);

	KG_PROCESS_ERROR(pKeyFrameData->dwValidateCandidate > static_cast<DWORD>(lIndex));

	const SoundTagSoundFileCandidate1& Candidate = pKeyFrameData->SoundCandidates[lIndex];
	*fHitRate = Candidate.fHitRate;
	*fVolume = Candidate.fVolume;
	*fMinDis = Candidate.fMinDistance;
	*fMaxDis = Candidate.fMaxDistance;
	if (Candidate.strSoundFileName.strFileName[0] == '\0')
	{
		*strSoundFileName = A2BSTR("New Sound Candidate");
	}
	else
	{
		*strSoundFileName = A2BSTR(Candidate.strSoundFileName.strFileName);
	}
	hr = S_OK;
Exit0:
	if (FAILED(hr))
	{
		sprintf_s(s_strOutputInfo, MAX_PATH, "Load Sound Tag @ Frame %d Candidate %d failed", lFrame, lIndex);
		MessageBox(s_strOutputInfo, "Error");
	}
}

void KSceneModelEditorAnimationPanel::OnSaveSoundTagCandidateAnimationseekbar1(long lFrame, 
																			   long lIndex, 
																			   LPCTSTR strSoundFileName, 
																			   float fHitRate, 
																			   float fVolume, 
																			   float fMinDis, 
																			   float fMaxDis)
{
	HRESULT hr = E_FAIL;
	UINT uRetCode = IEIR_FAILED;
	const SoundDataInfoVersion2* pKeyFrameData = NULL;
	SoundDataInfoVersion2 KeyFrameDataNew;
	IEKG3DAnimationSoundTag* pTag = dynamic_cast<IEKG3DAnimationSoundTag*>(GetTag(TAGTYPE_SOUND));
	KG_PROCESS_ERROR(pTag);

	hr = pTag->GetKeyFrameData(lFrame, &pKeyFrameData);
	KG_COM_PROCESS_ERROR(hr);

	KG_PROCESS_ERROR(pKeyFrameData->dwValidateCandidate > static_cast<DWORD>(lIndex));

	memcpy_s(&KeyFrameDataNew, sizeof(SoundDataInfoVersion2), pKeyFrameData, sizeof(SoundDataInfoVersion2));

	KeyFrameDataNew.SoundCandidates[lIndex].fHitRate = fHitRate;
	KeyFrameDataNew.SoundCandidates[lIndex].fVolume = fVolume;
	KeyFrameDataNew.SoundCandidates[lIndex].fMinDistance = fMinDis;
	KeyFrameDataNew.SoundCandidates[lIndex].fMaxDistance = fMaxDis;
	if (!strcmp(strSoundFileName, "New Sound Candidate"))
		KeyFrameDataNew.SoundCandidates[lIndex].strSoundFileName.strFileName[0] = '\0';
	else
	{
		strcpy_s(KeyFrameDataNew.SoundCandidates[lIndex].strSoundFileName.strFileName, 
			MAX_PATH,
			strSoundFileName);
	}

	//KeyFrameDataNew.dwValidateCandidate = static_cast<DWORD>(lNumCandidate);
	uRetCode = pTag->ModifyTag(lFrame, &KeyFrameDataNew);
Exit0:
	;	
}

void KSceneModelEditorAnimationPanel::OnDeleteSoundTagCandidateAnimationseekbar1(long lFrame, long lCandidate)
{
	HRESULT hr = E_FAIL;
	IEKG3DAnimationSoundTag* pTag = dynamic_cast<IEKG3DAnimationSoundTag*>(GetTag(TAGTYPE_SOUND));
	KG_PROCESS_ERROR(pTag);

	pTag->DeleteSoundCandidate(lFrame, lCandidate);
Exit0:
	;
}
void KSceneModelEditorAnimationPanel::OnBnClickedTagExport()
{
	// TODO: Add your control notification handler code here

	KModelEditorTagExport dlgTagExport;
	dlgTagExport.SetTaniName(m_strCurTaniName);
	dlgTagExport.SetAnimationTagContainer(m_pContainer);
	dlgTagExport.DoModal();
	
}

void KSceneModelEditorAnimationPanel::OnSFXTagLoadAnimationseekbar1(long nID, 
																	long* nPlayType, 
																	float* fPlaySpeed, 
																	long* nAutoAdaptSpeed, 
																	long* nInterruptable, 
																	long* nKeepMotionFinish, 
																	long* nNotify, 
																	long* nBindIndex, 
																	BSTR* strSFXFileName, 
																	BSTR* strTagName, 
																	long* nWorldPos)
{
	HRESULT hr = E_FAIL;
	const SFXKeyFrameInfoVersion1* pKeyFrame = NULL;
	IEKG3DAnimationSFXTag* pTag = dynamic_cast<IEKG3DAnimationSFXTag*>(GetTag(TAGTYPE_SFX));
	LPCSTR cstrSFXFileName;
	LPCSTR cstrTagName;
	KG_PROCESS_ERROR(pTag);

	hr = pTag->GetKeyFrameName(nID, &cstrTagName);
	KG_COM_PROCESS_ERROR(hr);
	*strTagName = T2BSTR(cstrTagName);

	hr = pTag->GetKeyFrameData(nID, &pKeyFrame);
	KG_COM_PROCESS_ERROR(hr);


	*nPlayType = static_cast<long>(pKeyFrame->FinishState);
	*fPlaySpeed = static_cast<float>(pKeyFrame->fSpeed);
	*nAutoAdaptSpeed = static_cast<long>(pKeyFrame->bAdaptSpeedToMotion);
	*nInterruptable = static_cast<long>(pKeyFrame->bInterruptable);
	*nKeepMotionFinish = static_cast<long>(pKeyFrame->MotionFinishEvent);
	*nNotify = static_cast<long>(pKeyFrame->bNotify);
	*nBindIndex = static_cast<long>(pKeyFrame->uBindFileIndex);
	*nWorldPos = static_cast<long>(pKeyFrame->bSetWorldPosition);

	hr = pTag->GetSFXFileName(pKeyFrame->uBindFileIndex, &cstrSFXFileName);
	KG_COM_PROCESS_ERROR(hr);

	*strSFXFileName = T2BSTR(cstrSFXFileName);
Exit0:
	;
}

void KSceneModelEditorAnimationPanel::OnSFXTagSaveAnimationseekbar1(long nTagID, 
																	long nPlayType, 
																	float fPlaySpeed, 
																	long nAutoAdaptSpeed, 
																	long nInterruptable, 
																	long nKeepMotionFinish, 
																	long nNotify, 
																	LPCTSTR strTagName, 
																	long nBindID, 
																	long nWorldPos)
{
	HRESULT hr = E_FAIL;
	UINT uFrame = 0;
	UINT uIndex = 0;
	SFXKeyFrameInfoVersion1 ModifyFrame;
	IEKG3DAnimationSFXTag* pTag = dynamic_cast<IEKG3DAnimationSFXTag*>(GetTag(TAGTYPE_SFX));
	KG_PROCESS_ERROR(pTag);

	ModifyFrame.bAdaptSpeedToMotion = static_cast<BOOL>(nAutoAdaptSpeed);
	ModifyFrame.bInterruptable = static_cast<BOOL>(nInterruptable);
	ModifyFrame.bNotify = static_cast<BOOL>(nNotify);
	ModifyFrame.FinishState = static_cast<SFXFinishState>(nPlayType);
	ModifyFrame.fSpeed = fPlaySpeed;
	ModifyFrame.MotionFinishEvent = static_cast<SFXMotionFinshState>(nKeepMotionFinish);
	ModifyFrame.bSetWorldPosition = static_cast<BOOL>(nWorldPos);
	strcpy_s(ModifyFrame.strTagName, 256, strTagName);

	hr = pTag->GetTagFrameIndexByID(nTagID, &uFrame, &uIndex);
	KG_COM_PROCESS_ERROR(hr);

	ModifyFrame.uID = nTagID;
	ModifyFrame.uBindFileIndex = nBindID;

	UINT uRetCode= pTag->ModifyTag(uFrame, static_cast<void*>(&ModifyFrame));
	uRetCode == ModifyFrame.uID ? hr = S_OK : hr = E_FAIL;
Exit0:
	if (FAILED(hr))
	{
		sprintf_s(s_strOutputInfo, 
			MAX_PATH,
			"Save frame @ %d , Index %d failed.",
			uFrame,
			uIndex);
		MessageBox(s_strOutputInfo, "Error", MB_OK);
	}
	else
	{
		m_pContainer->SetModifyState();
	}
}
void KSceneModelEditorAnimationPanel::OnSoundDialogDoModalAnimationseekbar1(long nFrame)
{
	// TODO: Add your message handler code here
	KdlgSoundTag DlgSound(dynamic_cast<IEKG3DAnimationSoundTag*>(m_pContainer->GetTag(TAGTYPE_SOUND)), this);
	//DlgSound.m_pAnimationPanel = this;
	DlgSound.SetAnimationPanel(this);
	DlgSound.SetFrame(UINT(nFrame));
	DlgSound.DoModal();
}

void KSceneModelEditorAnimationPanel::OnSfxDialogDoModalAnimationseekbar1(long nFrame)
{
	// TODO: Add your message handler code here
	KDlgSfxTag DlgSfx(dynamic_cast<IEKG3DAnimationSFXTag*>(m_pContainer->GetTag(TAGTYPE_SFX)), this);
	DlgSfx.SetFrame(nFrame);
	DlgSfx.SetAnimationPanel(this);
	DlgSfx.DoModal();
}
