// KDlgSfxTag.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KDlgSfxTag.h"
#include "KSceneModelEditorAnimationPanel.h"
#include "IEEditor.h"
// KDlgSfxTag dialog


static TCHAR* s_szSFXFinishState[] =
{   
	"循环",// SFX_FS_LOOP
	"单次保持循环状态" , // 保持最后状态
	"单次"//SFX_FS_DELETE
};
static UINT SFXFinishStateCount = 3;
IMPLEMENT_DYNAMIC(KDlgSfxTag, CDialog)
UINT KDlgSfxTag::m_uFrameCopy = 0xfffffff;
extern BYTE* g_SceneEditorKeyState;
KDlgSfxTag::KDlgSfxTag(IEKG3DAnimationSFXTag* pSfxTag,CWnd* pParent /*=NULL*/)
: CDialog(KDlgSfxTag::IDD, pParent)
, m_bAdaptSpeedToMotion(FALSE)
, m_bInterruptable(FALSE)
, m_bNotify(FALSE)
, m_bMotionFinishEvent(FALSE)
, m_fSpeed(0.f)
, m_bSetWorldPosition(FALSE)
, m_csTagName(_T(""))
, m_csSfxName(_T(""))
{
	m_pSfxTag = pSfxTag;
	m_uFrame = 0xfffffff;
	m_LastCursel = -1;
}

KDlgSfxTag::~KDlgSfxTag()
{
}

void KDlgSfxTag::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK1, m_bAdaptSpeedToMotion);
	DDX_Check(pDX, IDC_CHECK3, m_bInterruptable);
	DDX_Check(pDX, IDC_CHECK5, m_bNotify);
	DDX_Check(pDX, IDC_CHECK4, m_bMotionFinishEvent);
	DDX_Text(pDX, IDC_EDIT_SPEED, m_fSpeed);
	DDX_Check(pDX, IDC_CHECK2, m_bSetWorldPosition);
	DDX_Control(pDX, IDC_COMBO_TAGNAME, m_ComTagName);
	DDX_Text(pDX, IDC_EDIT_TAGNAME, m_csTagName);
	DDX_Text(pDX, IDC_EDIT4, m_csSfxName);
	DDX_Control(pDX, IDC_COMBO_FINISHSTATE, m_ComFinishState);
}


BEGIN_MESSAGE_MAP(KDlgSfxTag, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_NEW, &KDlgSfxTag::OnBnClickedButtonNew)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &KDlgSfxTag::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_SFXSELECT, &KDlgSfxTag::OnBnClickedButtonSfxselect)
	ON_CBN_DROPDOWN(IDC_COMBO_TAGNAME, &KDlgSfxTag::OnCbnDropdownComboTagname)
	ON_CBN_SELCHANGE(IDC_COMBO_TAGNAME, &KDlgSfxTag::OnCbnSelchangeComboTagname)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_COPY, &KDlgSfxTag::OnBnClickedButtonCopy)
	ON_BN_CLICKED(IDC_BUTTON_PASTE, &KDlgSfxTag::OnBnClickedButtonPaste)
	ON_EN_CHANGE(IDC_EDIT_TAGNAME, &KDlgSfxTag::OnEnChangeEditTagname)
END_MESSAGE_MAP()


// KDlgSfxTag message handlers
void KDlgSfxTag::FillComFinishState()
{
	m_ComFinishState.ResetContent();
	for (int i = 0;i < SFXFinishStateCount ; i ++)
	{
		m_ComFinishState.AddString(s_szSFXFinishState[i]);
	}
	m_ComFinishState.SetCurSel(0);
	UpdateData(FALSE);
}
void KDlgSfxTag::SetFrame(UINT uFrame)
{
	m_uFrame = uFrame;
}
void KDlgSfxTag::SetAnimationPanel(KSceneModelEditorAnimationPanel *pAnimationPanel)
{
	_ASSERTE(pAnimationPanel);
	m_pAnimationPanel = pAnimationPanel;
}
void KDlgSfxTag::SetComTagNameCurSel(int nCurSel)
{
	m_ComTagName.SetCurSel(nCurSel);
	m_LastCursel = nCurSel;
}
void KDlgSfxTag::FillComTagName()
{
	HRESULT hLoop = S_OK;
	const SFXKeyFrameInfoVersion1* pKeyFrameData = NULL;
	UINT uID = 0;
	DWORD dwCount = 0;
	DWORD nIndex = 0;
	m_ComTagName.ResetContent();
	while (SUCCEEDED(hLoop))
	{    
		hLoop = m_pSfxTag->GetTagIDByFrameIndex(m_uFrame,nIndex,&uID);
		KG_COM_PROCESS_ERROR(hLoop);
		m_pSfxTag->GetKeyFrameData(uID ,&pKeyFrameData);
		if (_strcmpi(pKeyFrameData->strTagName,"") == 0)
		{
			m_ComTagName.AddString("New SFX Tag");
		}
		else
		{
			m_ComTagName.AddString(pKeyFrameData->strTagName);
		}
		nIndex ++;
	}

	//KG_PROCESS_ERROR(dwCount);

Exit0:
	/* dwCount = m_ComTagName.GetCount();
	if(dwCount > 0)
	{
	SetComTagNameCurSel(dwCount - 1);
	}*/
	//m_ComTagName.SetCurSel(0);
	UpdateData(FALSE);
	;
}
void KDlgSfxTag::FillControlData()
{
	_ASSERTE(m_pSfxTag);
	HRESULT hResult = E_FAIL;
	const SFXKeyFrameInfoVersion1* pKeyFrameData = NULL;
	DWORD dwIndex = 0 ; 
	UINT uID = 0;
	DWORD dwCount = 0;
	DWORD dwModelIndex = 0;
	LPCSTR szSFXFileName;
	TCHAR szBindFileIndex[MAX_PATH];
	//FillComTagName();//填充标签列表
	dwCount = m_ComTagName.GetCount();
	KG_PROCESS_ERROR(dwCount);
	dwIndex = m_ComTagName.GetCurSel();
	KG_PROCESS_ERROR(dwIndex != CB_ERR);
	m_pSfxTag->GetTagIDByFrameIndex(m_uFrame,dwIndex,&uID);
	m_pSfxTag->GetKeyFrameData(uID,&pKeyFrameData);
	m_csTagName.Format(_T("%s"),pKeyFrameData->strTagName);// 赋值给tagname
	FillComFinishState();
	m_fSpeed = pKeyFrameData->fSpeed;
	m_bAdaptSpeedToMotion = pKeyFrameData->bAdaptSpeedToMotion;
	m_bInterruptable = pKeyFrameData->bInterruptable;
	m_bMotionFinishEvent = (pKeyFrameData->MotionFinishEvent == SFX_MFS_KEEP);
	m_bNotify = pKeyFrameData->bNotify;
	m_bSetWorldPosition = pKeyFrameData->bSetWorldPosition;

	m_ComFinishState.SetCurSel(pKeyFrameData->FinishState);
	// bind文件 不知道是什么 
	//

	//设置bind文件索引 
	dwModelIndex = pKeyFrameData->uBindFileIndex;
	sprintf_s(szBindFileIndex,"Bind文件索引  %d",dwModelIndex);
	SetDlgItemText(IDC_STATIC_BIND_INDEX,szBindFileIndex);
	//设置特效文件名字
	m_pSfxTag->GetSFXFileName(dwModelIndex,&szSFXFileName);
	m_csSfxName.Format(_T("%s"),szSFXFileName);


	hResult = S_OK;
Exit0:
	if(!SUCCEEDED(hResult))
	{
		m_csTagName.Format(_T(""));
		m_ComFinishState.SetCurSel(-1);
		m_fSpeed = 0.f;
		sprintf_s(szBindFileIndex,"Bind文件索引  ");
		SetDlgItemText(IDC_STATIC_BIND_INDEX,szBindFileIndex);
		m_csSfxName.Format(_T(""));
	}
	UpdateData(FALSE);
	;
}
BOOL KDlgSfxTag::OnInitDialog()
{
	CDialog::OnInitDialog();
	DWORD dwCount = 0;
	FillComTagName();
	dwCount = m_ComTagName.GetCount();
	if(dwCount > 0)
	{
		SetComTagNameCurSel(0);
	}
	else
	{
		SetComTagNameCurSel(-1);
	}
	FillControlData();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void KDlgSfxTag::OnBnClickedButtonNew()
{
	// TODO: Add your control notification handler code here
	_ASSERTE(m_pSfxTag);
	_ASSERTE(m_pAnimationPanel);
	DWORD dwCount = 0;
	m_pAnimationPanel->OnNewSFXTagAnimationseekbar1(m_uFrame);

	FillComTagName();
	dwCount = m_ComTagName.GetCount();
	if(dwCount > 0)
	{
		SetComTagNameCurSel(dwCount - 1);
	}
	else
	{
		SetComTagNameCurSel(-1);
	}
	FillControlData();
}

void KDlgSfxTag::OnBnClickedButtonDelete()
{
	// TODO: Add your control notification handler code here
	_ASSERTE(m_pAnimationPanel);
	_ASSERTE(m_pSfxTag);
	UINT nId = 0;
	HRESULT hRetCode = E_FAIL;
	DWORD dwIndex = m_ComTagName.GetCurSel();
	DWORD dwCount = 0;
	KG_PROCESS_ERROR(dwIndex != CB_ERR);
	m_pSfxTag->GetTagIDByFrameIndex(m_uFrame,dwIndex,&nId);

	m_pAnimationPanel->OnSFXTagDeleteAnimationseekbar1( long(nId));

	FillComTagName();
	dwCount = m_ComTagName.GetCount();
	if(dwCount > 0)
	{
		SetComTagNameCurSel(0);
	}
	else
	{
		SetComTagNameCurSel(-1);
	}
	FillControlData();
Exit0:
	;
}

void KDlgSfxTag::OnBnClickedButtonSfxselect()
{
	// TODO: Add your control notification handler code here
	_ASSERTE(m_pAnimationPanel);
	_ASSERTE(m_pSfxTag);
	HRESULT hResult = E_FAIL;
	UINT nId = 0;
	DWORD dwIndex = 0 ;
	dwIndex = m_ComTagName.GetCurSel();
	KG_PROCESS_ERROR(dwIndex != CB_ERR);
	m_pSfxTag->GetTagIDByFrameIndex(m_uFrame,dwIndex,&nId);
	OnOK();

	m_pAnimationPanel->OnSFXSelectAnimationseekbar1(long(nId));

	hResult = S_OK;
	//m_pSfxTag->GetSFXFileName(dwIndex,&psz);
	//FillControlData();
Exit0:
	if(hResult == E_FAIL)
	{
		MessageBox("当前没有选中任何标签","ERROR");
	}
	;
}

void KDlgSfxTag::OnCbnDropdownComboTagname()
{

}


void KDlgSfxTag::SaveState()
{
	_ASSERTE(m_pSfxTag);
	DWORD dwIndex = 0;
	DWORD dwComFinishState = 0;
	HRESULT hRetCode = E_FAIL;
	UINT uID = 0;
	const SFXKeyFrameInfoVersion1* pKeyFrameData = NULL;
	SFXKeyFrameInfoVersion1 KeyFrameData;
	UpdateData();
	KG_PROCESS_ERROR(m_LastCursel != -1);
	dwIndex = m_LastCursel;
	hRetCode = m_pSfxTag->GetTagIDByFrameIndex(m_uFrame,dwIndex ,&uID);
	KG_COM_PROCESS_ERROR(hRetCode);


	m_pSfxTag->GetKeyFrameData(uID,&pKeyFrameData);
	memcpy_s(&KeyFrameData,sizeof(SFXKeyFrameInfoVersion1),pKeyFrameData,sizeof(SFXKeyFrameInfoVersion1));
	sprintf_s(KeyFrameData.strTagName,m_csTagName.GetBuffer());
	KeyFrameData.bAdaptSpeedToMotion = m_bAdaptSpeedToMotion;
	KeyFrameData.fSpeed = m_fSpeed;
	KeyFrameData.bInterruptable = m_bInterruptable;
	KeyFrameData.MotionFinishEvent = (m_bInterruptable ? SFX_MFS_KEEP : SFX_MFS_DELETE);
	KeyFrameData.bSetWorldPosition = m_bSetWorldPosition;
	KeyFrameData.bNotify = m_bNotify;

	dwComFinishState = m_ComFinishState.GetCurSel();
	KG_PROCESS_ERROR(dwComFinishState != CB_ERR && dwComFinishState < SFXFinishStateCount);
	KeyFrameData.FinishState = (SFXFinishState)dwComFinishState;
	KeyFrameData.uID = uID; // 设置ID 
	m_pSfxTag->ModifyTag(m_uFrame,static_cast<void*>(&KeyFrameData));


Exit0:
	;
}
void KDlgSfxTag::OnCbnSelchangeComboTagname()
{
	// TODO: Add your control notification handler code here
	DWORD dwIndex = m_ComTagName.GetCurSel();
	if(m_LastCursel != -1)
	{
		SaveState();
	}
	FillComTagName();
	SetComTagNameCurSel(dwIndex);
	FillControlData();
Exit0:
	;
}

void KDlgSfxTag::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	SaveState();
	CDialog::OnClose();
}

void KDlgSfxTag::OnBnClickedButtonCopy()
{
	// TODO: Add your control notification handler code here
	m_uFrameCopy = m_uFrame;
	//SaveState();
}

void KDlgSfxTag::OnBnClickedButtonPaste()
{
	// TODO: Add your control notification handler code here
	_ASSERTE(m_pSfxTag);
	KG_PROCESS_ERROR(m_uFrameCopy != 0xfffffff);
	KG_PROCESS_ERROR(m_uFrameCopy != m_uFrame);
	DWORD dwIndex = 0;
	DWORD dwCount = 0;
	HRESULT hLoop = S_OK;
	UINT uID = 0;
	while (SUCCEEDED(hLoop))
	{
		hLoop = m_pSfxTag->GetTagIDByFrameIndex(m_uFrame,dwIndex,&uID);
		if(SUCCEEDED(hLoop))
			m_pSfxTag->RemoveTag(uID);
	}
	m_pSfxTag->CopyTag(m_uFrameCopy,m_uFrame);
Exit0:
	FillComTagName();
	dwCount = m_ComTagName.GetCount();
	if(dwCount > 0)
	{
		SetComTagNameCurSel(0);
	}
	FillControlData();
	;
}

BOOL KDlgSfxTag::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
#define DELAY 300
	static DWORD dwLastTimeCopy = timeGetTime();
	static DWORD dwLastTimePaste = timeGetTime();
	static DWORD dwLastDelete = timeGetTime();
	DWORD dwCurrentTime = timeGetTime();
	// if(pMsg->message == WM_CHAR)
	{
		if(g_SceneEditorKeyState[VK_CONTROL])
		{
			if(g_SceneEditorKeyState['C'])
			{ 
				if(dwCurrentTime - dwLastTimeCopy >= DELAY)
				{
					OnBnClickedButtonCopy();
					dwLastTimeCopy = dwCurrentTime;
					g_SceneEditorKeyState['C'] = 0;

					return TRUE;
				}
			}
			if(g_SceneEditorKeyState['V'])
			{
				if(dwCurrentTime - dwLastTimePaste >= DELAY)
				{

					dwLastTimePaste = dwCurrentTime;
					OnBnClickedButtonPaste();

					g_SceneEditorKeyState['V'] = 0;

					return TRUE;
				}
			}
		}

	}
	// if(pMsg->message ==WM_KEYDOWN)
	if(g_SceneEditorKeyState[VK_DELETE])
	{
		if(dwCurrentTime - dwLastDelete >= DELAY)
		{
			dwLastDelete = dwCurrentTime;
			OnBnClickedButtonDelete();
			g_SceneEditorKeyState[VK_DELETE] = 0;
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void KDlgSfxTag::OnEnChangeEditTagname()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	DWORD dwIndex = m_ComTagName.GetCurSel();
	KG_PROCESS_ERROR(dwIndex != CB_ERR);
	SaveState();
	FillComTagName();
	SetComTagNameCurSel(dwIndex);
	// UpdateData(FALSE);
Exit0:
	;
}
