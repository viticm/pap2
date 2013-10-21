// KdlgSoundTag.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KdlgSoundTag.h"
#include "KSceneModelEditorAnimationPanel.h"
#include "KDlgSoundSource.h"
// KdlgSoundTag dialog

const static UINT uIllegal = 0xfffffff;
IMPLEMENT_DYNAMIC(KdlgSoundTag, CDialog)

KdlgSoundTag::KdlgSoundTag(IEKG3DAnimationSoundTag* pTag,CWnd* pParent /*=NULL*/)
: CDialog(KdlgSoundTag::IDD, pParent)
, m_fPlayRate(0)
, m_fFar(0)
, m_fNear(0)
, m_Volume_V2(0)
, m_Volume_V3(0)
{
	m_nLastIndex = CB_ERR;
	m_pSoundTag = pTag;
}
UINT KdlgSoundTag::m_uCopyFrame = uIllegal;
KdlgSoundTag::~KdlgSoundTag()
{
}

void KdlgSoundTag::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SOUNDFILE, m_ComFileList);
	DDX_Text(pDX, IDC_EDIT_RATE2, m_fPlayRate);
	DDX_Text(pDX, IDC_EDIT_NEAR, m_fNear);
	DDX_Text(pDX, IDC_EDIT_FAR, m_fFar);
	DDX_Slider(pDX, IDC_SLIDER_VOLUME2, m_Volume_V2);
	DDV_MinMaxInt(pDX, m_Volume_V2, 0, 100);
	DDX_Slider(pDX, IDC_SLIDER_VOLUME_3, m_Volume_V3);
	DDV_MinMaxInt(pDX, m_Volume_V3, 0, 100);
	DDX_Control(pDX, IDC_COMBO_SOUNDTYPE, m_ComSoundType);
}

BEGIN_MESSAGE_MAP(KdlgSoundTag, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_VERSION2, &KdlgSoundTag::OnBnClickedButtonVersion2)
	ON_BN_CLICKED(IDC_BUTTON_TOVERSION3, &KdlgSoundTag::OnBnClickedButtonToversion3)
	ON_BN_CLICKED(IDC_BUTTON_FILELIST, &KdlgSoundTag::OnBnClickedButtonFilelist)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &KdlgSoundTag::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTONDELETE, &KdlgSoundTag::OnBnClickedButtondelete)
	ON_BN_CLICKED(IDC_BUTTON_DELETETAG, &KdlgSoundTag::OnBnClickedButtonDeletetag)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, &KdlgSoundTag::OnBnClickedButtonPlay)
	ON_CBN_SELCHANGE(IDC_COMBO_SOUNDFILE, &KdlgSoundTag::OnCbnSelchangeComboSoundfile)
	ON_CBN_SELCHANGE(IDC_COMBO_SOUNDTYPE, &KdlgSoundTag::OnCbnSelchangeComboSoundtype)
	ON_BN_CLICKED(IDC_CHECK1, &KdlgSoundTag::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_CHECK2, &KdlgSoundTag::OnBnClickedCheck2)
	ON_BN_CLICKED(IDC_CHECK3, &KdlgSoundTag::OnBnClickedCheck3)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

static int s_nVersion2[] = 
{
	IDC_STATIC_GROUP_2,
	IDC_STATIC_SOUNDFILE,
	IDC_COMBO_SOUNDFILE,
	IDC_BUTTON_FILELIST,
	IDC_STATIC_RATE_2,
	IDC_EDIT_RATE2,
	IDC_BUTTON_ADD,
	IDC_STATIC_VOLUME_V2,
	IDC_SLIDER_VOLUME2,
	IDC_BUTTONDELETE,
	IDC_STATIC_NEAR,
	IDC_EDIT_NEAR,
	IDC_BUTTON_TOVERSION3,
	IDC_STATIC_FAR,
	IDC_EDIT_FAR,
};
static int s_nVersion3[] = 
{
	IDC_STATIC_GROUP_3,
	IDC_STATIC_SOUNDTYPE,
	IDC_COMBO_SOUNDTYPE,
	IDC_BUTTON_VERSION2,
	IDC_STATIC_VOLUME_V3,
	IDC_SLIDER_VOLUME_3,
};
KdlgSoundTag::pfFillControl KdlgSoundTag::s_FillControl[] = 
{
	&KdlgSoundTag::FillVersion2,
	&KdlgSoundTag::FillVersion3,
};
KdlgSoundTag::pfPlay KdlgSoundTag::s_Play[] = 
{
	&KdlgSoundTag::PlayVersion2,
	&KdlgSoundTag::PlayVersion3,
};
KdlgSoundTag::pfDelete KdlgSoundTag::s_Delete[] = 
{
	&KdlgSoundTag::DeleteVersion2,
	&KdlgSoundTag::DeleteVersion3,
};
KdlgSoundTag::pfSave KdlgSoundTag::s_Save[] =
{
	&KdlgSoundTag::SaveStateV2,
	&KdlgSoundTag::SaveStateV3,
};
static TCHAR* s_strSoundType[] = 
{
	"smile",
};


void KdlgSoundTag::OnBnClickedButtonVersion2()
{
	// TODO: Add your control notification handler code here
	(this->*s_Save[m_uVersion-2])();
	m_uVersion = 2;
	m_nLastIndex = CB_ERR;
	ChangeVersion();
	(this->*s_FillControl[m_uVersion-2])();
}

void KdlgSoundTag::OnBnClickedButtonToversion3()
{
	// TODO: Add your control notification handler code here
	(this->*s_Save[m_uVersion-2])();
	m_uVersion = 3;
	m_nLastIndex = CB_ERR;
	ChangeVersion();
	(this->*s_FillControl[m_uVersion-2])();
}
void KdlgSoundTag::PlayVersion2()
{
	HRESULT hRetcode = E_FAIL;
	HRESULT hResult = E_FAIL;
	const SoundDataInfoVersion2 *pSoundTagData = NULL;
	DWORD dwIndex = m_ComFileList.GetCurSel();
	KG_PROCESS_ERROR(m_pSoundTag);
	KG_PROCESS_ERROR(dwIndex != CB_ERR);
	KG_PROCESS_ERROR(m_uFrame != uIllegal);
	hRetcode = m_pSoundTag->GetKeyFrameData(m_uFrame,&pSoundTagData);
	KG_COM_PROCESS_ERROR(hRetcode);
	UpdateData();
	m_pAnimationPanel->OnPlaySoundFileAnimationseekbar1(pSoundTagData->SoundCandidates[dwIndex].strSoundFileName.strFileName,(float)m_Volume_V2);
	hResult = S_OK;
Exit0:
	if(hResult == E_FAIL)
	{
		MessageBox("没有有效的声音文件","ERROR");
	}
}
void KdlgSoundTag::PlayVersion3()
{
	HRESULT hRetCode = E_FAIL;
	HRESULT hResult = E_FAIL;
	DWORD dwSel = m_ComSoundType.GetCurSel();
	SoundType soundType = static_cast<SoundType>(dwSel);
	AnimationSoundTagInfo SoundTagInfo;
	hRetCode = m_pSoundTag->GetSound(soundType,&SoundTagInfo);
	KG_COM_PROCESS_ERROR(hRetCode);
	UpdateData();
	m_pAnimationPanel->OnPlaySoundFileAnimationseekbar1(SoundTagInfo.strFileName,m_Volume_V3);
	hResult = S_OK;
Exit0:
	if(hResult == E_FAIL)
	{
		MessageBox("没有有效的声音文件","ERROR");
	}
}
void KdlgSoundTag::OnBnClickedButtonFilelist()
{
	// TODO: Add your control notification handler code here
	HRESULT hResult = E_FAIL;
	KDlgSoundSource DlgSoundSource(this);
	DWORD dwIndex = m_ComFileList.GetCurSel();
	UINT uId = 0;
	KG_PROCESS_ERROR(dwIndex != CB_ERR);
	KG_PROCESS_ERROR(m_uFrame != 0xfffffff);
	DlgSoundSource.SetTreeTouch(m_szFromTreeCtl);
	DlgSoundSource.DoModal();
	if(_strcmpi(m_szFromTreeCtl ,"") != 0)
	{
		SoundDataInfoVersion2 SoundTagData;
		GetKeyFrameData(m_uFrame,&SoundTagData); 
		strcpy_s(SoundTagData.SoundCandidates[dwIndex].strSoundFileName.strFileName,MAX_PATH,m_szFromTreeCtl);
		uId = GETID(m_uFrame,2);
		m_pSoundTag->ModifyTag(uId,static_cast<void *>(&SoundTagData));
		FillFileList();
		m_ComFileList.SetCurSel(dwIndex);
		m_nLastIndex = dwIndex;
		FillVersion2Detail();
	}
	hResult = S_OK;
Exit0:
	if (hResult == E_FAIL)
	{
		MessageBox("当前没有SoundCandidates被选中","ERROR");
	}
	;
}
void KdlgSoundTag::ChangeVersion()
{
	for (size_t i = 0 ; i < sizeof(s_nVersion2) /sizeof(int) ; i ++)
	{
		GetDlgItem(s_nVersion2[i])->ShowWindow(m_uVersion == 2);
	}
	for (size_t i = 0 ; i < sizeof(s_nVersion3) /sizeof(int) ; i ++)
	{
		GetDlgItem(s_nVersion3[i])->ShowWindow(m_uVersion == 3);
	}
}
void KdlgSoundTag::OnBnClickedButtonAdd()
{
	// TODO: Add your control notification handler code here
	HRESULT hRetCode = E_FAIL;
	SoundDataInfoVersion2 SoundTagData;
	SoundTagSoundFileCandidate1 SoundTagNew;
	UINT UId = 0; 
	KG_PROCESS_ERROR(m_pSoundTag);
	KG_PROCESS_ERROR(m_uFrame != uIllegal);
	GetKeyFrameData(m_uFrame,&SoundTagData);
	if(SoundTagData.dwValidateCandidate >= 5)
	{
		MessageBox("声音备选项已达到最大个数", "Sceneeditor");
		KG_PROCESS_ERROR(FALSE);
	}
	SoundTagData.SoundCandidates[SoundTagData.dwValidateCandidate] = SoundTagNew;
	SoundTagData.dwValidateCandidate ++;
	SoundTagData.SoundCandidates[SoundTagData.dwValidateCandidate - 1].strSoundFileName.strFileName[0] = '\0';// 刚建立的SoundCandidates文件为空
	UId = GETID(m_uFrame,2);
	m_pSoundTag->ModifyTag(UId,static_cast<void*>(&SoundTagData) );
	FillFileList();
	m_nLastIndex = m_ComFileList.GetCount() - 1;
	m_ComFileList.SetCurSel(m_nLastIndex);
	FillVersion2Detail();
Exit0:
	;
}

void KdlgSoundTag::OnBnClickedButtondelete()
{
	// TODO: Add your control notification handler code here
	HRESULT hRetCode = E_FAIL;
	HRESULT hResult = E_FAIL;
	int nCurSel = m_ComFileList.GetCurSel();
	const SoundDataInfoVersion2 *pSoundTagData = NULL;
	DWORD dwCount = 0;
	KG_PROCESS_ERROR(m_pSoundTag);
	KG_PROCESS_ERROR(m_uFrame != uIllegal);
	hRetCode = m_pSoundTag->DeleteSoundCandidate(m_uFrame,nCurSel);
	KG_COM_PROCESS_ERROR(hRetCode);
	hRetCode = m_pSoundTag->GetKeyFrameData(m_uFrame,&pSoundTagData);
	KG_COM_PROCESS_ERROR(hRetCode);
	FillFileList();
	if(m_ComFileList.GetCount() > 0)
	{
		dwCount = m_ComFileList.GetCount();
		m_nLastIndex = dwCount -1;
		m_ComFileList.SetCurSel(m_nLastIndex);
	}
	else
	{
		m_ComFileList.SetCurSel(-1);
		m_nLastIndex = CB_ERR;
	}
	FillVersion2Detail();
Exit0:
	;	
}

void KdlgSoundTag::OnBnClickedButtonDeletetag()
{
	// TODO: Add your control notification handler code here
	(this->*s_Delete[m_uVersion-2])();
}

void KdlgSoundTag::OnBnClickedButtonPlay()
{
	// TODO: Add your control notification handler code here
	(this->*s_Play[m_uVersion-2])();
}

BOOL KdlgSoundTag::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	HRESULT hResult = E_FAIL;
	HRESULT hRet = E_FAIL;
	m_uVersion = 2;
	MoveControl();
	FillSoundType();
	(this->*s_FillControl[m_uVersion-2])();
Exit0:
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
HRESULT KdlgSoundTag::GetKeyFrameData(UINT uFrame,SoundDataInfoVersion2 *pRetSoundTagData)
{
	HRESULT hResult = E_FAIL;
	HRESULT hRetCode = E_FAIL;
	const SoundDataInfoVersion2 * pSoundTagData = NULL;
	SoundDataInfoVersion2 SoundDataTemp;
	KG_PROCESS_ERROR(m_pSoundTag);
	KG_PROCESS_ERROR(m_uFrame != uIllegal);
	KG_PROCESS_ERROR(pRetSoundTagData);
	hRetCode = m_pSoundTag->GetKeyFrameData(m_uFrame, &pSoundTagData);
	KG_COM_PROCESS_ERROR(hRetCode);
	KG_PROCESS_ERROR(pSoundTagData);
	memcpy_s(&SoundDataTemp, sizeof(SoundDataInfoVersion2), pSoundTagData, sizeof(SoundDataInfoVersion2));
	*pRetSoundTagData = SoundDataTemp;
	hResult = S_OK;
Exit0:
	return hResult;
}
HRESULT KdlgSoundTag::GetKeyFrameData(UINT uFrame,SoundDataInfoVersion3 *pRetSoundTagData)
{
	HRESULT hResult = E_FAIL;
	HRESULT hRet = E_FAIL;
	const SoundDataInfoVersion3 * pSoundTagData = NULL;
	SoundDataInfoVersion3 SoundDataTemp;
	KG_PROCESS_ERROR(m_pSoundTag);
	KG_PROCESS_ERROR(m_uFrame != uIllegal);
	KG_PROCESS_ERROR(pRetSoundTagData);
	hRet = m_pSoundTag->GetKeyFrameData(uFrame,&pSoundTagData);
	KG_COM_PROCESS_ERROR(hRet);
	memcpy_s(&SoundDataTemp, sizeof(SoundDataInfoVersion3), pSoundTagData, sizeof(SoundDataInfoVersion3));
	*pRetSoundTagData = SoundDataTemp;
	hResult = S_OK;
Exit0:
	return hResult;
}
void KdlgSoundTag::FillVersion2()
{//填充 控件 版本2
	const SoundDataInfoVersion2 *pSoundData = NULL;
	HRESULT hRect = E_FAIL;
	KG_PROCESS_ERROR(m_pSoundTag);
	KG_PROCESS_ERROR(m_uFrame != uIllegal);
	hRect = m_pSoundTag->GetKeyFrameData(m_uFrame,&pSoundData);
	KG_COM_PROCESS_ERROR(hRect);
	((CButton *)GetDlgItem(IDC_CHECK1))->SetCheck(pSoundData->bLoop);
	((CButton *)GetDlgItem(IDC_CHECK2))->SetCheck(pSoundData->InterruptMode == SPI_ENABLE);
	((CButton *)GetDlgItem(IDC_CHECK3))->SetCheck(pSoundData->MotionChangeState == SMCS_KEEP);
Exit0:
	FillFileList();
	FillVersion2Detail();
}
void KdlgSoundTag::SaveStateV2()
{// 保存版本2
	UpdateData();
	HRESULT hRetCode = E_FAIL;
	DWORD dwCountCandidate = 0;
	DWORD dwIndex = 0;
	SoundDataInfoVersion2 SoundTagData;
	TCHAR szComCurString[MAX_PATH];
	UINT UId = GETID(m_uFrame,2);
	KG_PROCESS_ERROR(m_pSoundTag);
	KG_PROCESS_ERROR(m_uFrame != uIllegal);
	KG_PROCESS_ERROR(m_nLastIndex != CB_ERR);
	GetKeyFrameData(m_uFrame,&SoundTagData);
	dwCountCandidate = SoundTagData.dwValidateCandidate;
	dwIndex = m_nLastIndex;
	KG_PROCESS_ERROR(dwIndex != -1);
	m_ComFileList.GetLBText(dwIndex,szComCurString);// 获取当前的文件名
	SoundTagData.SoundCandidates[dwIndex].fHitRate = m_fPlayRate;
	SoundTagData.SoundCandidates[dwIndex].fMaxDistance = m_fFar;
	SoundTagData.SoundCandidates[dwIndex].fMinDistance = m_fNear;
	SoundTagData.SoundCandidates[dwIndex].fVolume = (float)m_Volume_V2;
	sprintf_s(SoundTagData.SoundCandidates[dwIndex].strSoundFileName.strFileName,"%s",szComCurString);
	m_pSoundTag->ModifyTag(UId,static_cast<void*>(&SoundTagData));
Exit0:
	;
}
void KdlgSoundTag::FillVersion2Detail()
{
	const SoundDataInfoVersion2 * pSoundTagData = NULL;
	HRESULT hRetCode = E_FAIL;
	HRESULT hResult = E_FAIL;
	DWORD dwSel = 0;
	SoundTagSoundFileCandidate1 SoundTagTemp;
	KG_PROCESS_ERROR(m_pSoundTag);
	KG_PROCESS_ERROR(m_uFrame != uIllegal);
	hRetCode = m_pSoundTag->GetKeyFrameData(m_uFrame,&pSoundTagData);
	KG_COM_PROCESS_ERROR(hRetCode);
	KG_PROCESS_ERROR(pSoundTagData);
	dwSel = m_ComFileList.GetCurSel();
	KG_PROCESS_ERROR(dwSel != CB_ERR);
	SoundTagTemp = (pSoundTagData->SoundCandidates[dwSel]); 
	m_fPlayRate = (float)SoundTagTemp.fHitRate;
	m_Volume_V2 = (int)SoundTagTemp.fVolume;
	m_fNear = SoundTagTemp.fMinDistance;
	m_fFar = SoundTagTemp.fMaxDistance;
	m_nLastIndex = dwSel;
	hResult = S_OK;
Exit0:
	if(hResult == E_FAIL)
	{
		m_fPlayRate = 0.0;
		m_Volume_V2 = 0.0;
		m_fNear = 0.0;
		m_fFar = 0.0;
	}
	UpdateData(FALSE);
}
void KdlgSoundTag::FillFileList()
{
	const SoundDataInfoVersion2 *pSoundData = NULL;
	HRESULT hRect = E_FAIL;
	DWORD dwCount = 0;
	KG_PROCESS_ERROR(m_pSoundTag);
	KG_PROCESS_ERROR(m_uFrame != uIllegal);
	hRect = m_pSoundTag->GetKeyFrameData(m_uFrame,&pSoundData);
	KG_COM_PROCESS_ERROR(hRect);
	dwCount = pSoundData->dwValidateCandidate;
	m_ComFileList.ResetContent();
	for (DWORD i = 0 ; i < dwCount ; i ++)
	{
		SoundTagSoundFileCandidate1 SoundTagTemp = pSoundData->SoundCandidates[i];
		if(_strcmpi(SoundTagTemp.strSoundFileName.strFileName ,"") == 0)
		{
			m_ComFileList.AddString("New Sound Candidate");
		}
		else
			m_ComFileList.AddString(SoundTagTemp.strSoundFileName.strFileName);
	}
	if(dwCount > 0)
	{
		m_ComFileList.SetCurSel(0);
		m_nLastIndex = 0;
	}
	else
	{
		m_ComFileList.SetCurSel(-1);
		m_nLastIndex = CB_ERR;
	}
	UpdateData(FALSE);
Exit0:
	;
}
void KdlgSoundTag::FillVersion3()
{
	HRESULT hRetCode = E_FAIL;
	const SoundDataInfoVersion3 *pSoundTagData = NULL;
	KG_PROCESS_ERROR(m_pSoundTag);
	KG_PROCESS_ERROR(m_uFrame != uIllegal);
	hRetCode = m_pSoundTag->GetKeyFrameData(m_uFrame,&pSoundTagData);
	KG_COM_PROCESS_ERROR(hRetCode);

	((CButton *)GetDlgItem(IDC_CHECK1))->SetCheck(pSoundTagData->bLoop);
	((CButton *)GetDlgItem(IDC_CHECK2))->SetCheck(pSoundTagData->InterruptMode == SPI_ENABLE);
	((CButton *)GetDlgItem(IDC_CHECK3))->SetCheck(pSoundTagData->MotionChangeState == SMCS_KEEP);


	FillVersion3Detail();
Exit0:
	;
}
void KdlgSoundTag::FillVersion3Detail()
{
	const SoundDataInfoVersion3 *pSoundTagData = NULL;
	HRESULT hRetCode = E_FAIL;
	KG_PROCESS_ERROR(m_pSoundTag);
	KG_PROCESS_ERROR(m_uFrame != uIllegal);
	hRetCode = m_pSoundTag->GetKeyFrameData(m_uFrame,&pSoundTagData);
	KG_COM_PROCESS_ERROR(hRetCode);
	m_Volume_V3 = pSoundTagData->fVolume;
	m_ComSoundType.SetCurSel(pSoundTagData->soundType);
Exit0:
	UpdateData(FALSE);
}
void KdlgSoundTag::FillSoundType()
{
	m_ComSoundType.ResetContent();
	for (size_t i = 0 ; i < sizeof(s_strSoundType) / sizeof(TCHAR *) ; i ++)
	{
		m_ComSoundType.AddString(s_strSoundType[i]);
	}
	UpdateData(FALSE);
}
HRESULT KdlgSoundTag::MoveControl()
{
	RECT TarRect3;
	RECT TarRectDetail;
	int nMove3X = 0;
	int nMove3Y = 0;
	int nMoveDetailX = 0;
	int nMoveDetailY = 0;
	GetDlgItem(s_nVersion2[0])->GetWindowRect(&TarRect3);
	GetDlgItem(s_nVersion3[0])->GetWindowRect(&TarRectDetail);
	nMove3X = TarRectDetail.left - TarRect3.left;
	nMove3Y = TarRectDetail.top - TarRect3.top;
	for (size_t i = 0 ; i < sizeof(s_nVersion3) /sizeof(int) ; i ++)
	{
		CWnd *pCtrl = GetDlgItem(s_nVersion3[i]);
		RECT rect;
		pCtrl->GetWindowRect(&rect);
		rect.left -= nMove3X;
		rect.right -= nMove3X;
		rect.bottom -= nMove3Y;
		rect.top -= nMove3Y;
		ScreenToClient(&rect);
		pCtrl->MoveWindow(&rect);
		pCtrl->ShowWindow(SW_HIDE);
	}

	{
		RECT rect;
		GetDlgItem(IDC_STATIC_DETAIL)->GetWindowRect(&rect);
		nMoveDetailX = rect.left - TarRectDetail.left;
		nMoveDetailY = rect.top - TarRectDetail.top;
		int nId[] = {IDC_BUTTON_DELETETAG,IDC_BUTTON_PLAY};
		for (size_t i = 0 ; i < sizeof(nId) / sizeof(int) ; i ++)
		{
			CWnd *pCtrl = GetDlgItem(nId[i]);
			pCtrl->GetWindowRect(&rect);
			rect.left -= nMoveDetailX;
			rect.right -= nMoveDetailX;
			rect.top -= nMoveDetailY;
			rect.bottom -= nMoveDetailY;
			pCtrl->MoveWindow(&rect);
		}
	}
	return S_OK;
}
HRESULT KdlgSoundTag::SetFrame(UINT uFrame)
{
	m_uFrame = uFrame;
	return S_OK;
}
void KdlgSoundTag::OnCbnSelchangeComboSoundfile()
{
	DWORD dwIndex = 0;
	KG_PROCESS_ERROR(m_pSoundTag);
	KG_PROCESS_ERROR(m_uFrame != uIllegal);
	if(m_nLastIndex != CB_ERR)
		SaveStateV2();
	m_nLastIndex = dwIndex;
	FillVersion2Detail();
Exit0:
	;
}

void KdlgSoundTag::OnCbnSelchangeComboSoundtype()
{
	// TODO: Add your control notification handler code here
	SoundDataInfoVersion3 SoundTagData;
	DWORD dwSel = 0;
	HRESULT hRetCode = E_FAIL;
	UINT uId = 0;
	dwSel = m_ComSoundType.GetCurSel();
	KG_PROCESS_ERROR(dwSel != CB_ERR);
	KG_PROCESS_ERROR(dwSel < ST_COUNT);
	KG_PROCESS_ERROR(m_pSoundTag);
	KG_PROCESS_ERROR(m_uFrame != uIllegal);
	hRetCode = GetKeyFrameData(m_uFrame,&SoundTagData);
	KG_COM_PROCESS_ERROR(hRetCode);
	SoundTagData.soundType = static_cast<SoundType>(dwSel);
	uId = GETID(m_uFrame,3);
	m_pSoundTag->ModifyTag(uId,static_cast<void*>(&SoundTagData));
Exit0:
	;
}
void KdlgSoundTag::SaveStateV3()
{
	UpdateData();
	SoundDataInfoVersion3 SoundTagData;
	HRESULT hRetCode = E_FAIL;
	UINT uId;
	KG_PROCESS_ERROR(m_pSoundTag);
	KG_PROCESS_ERROR(m_uFrame != uIllegal);
	hRetCode = GetKeyFrameData(m_uFrame,&SoundTagData);
	KG_COM_PROCESS_ERROR(hRetCode);
	SoundTagData.soundType = static_cast<SoundType>(m_ComSoundType.GetCurSel());
	SoundTagData.fVolume = m_Volume_V3;
	uId = GETID(m_uFrame,3);
	m_pSoundTag->ModifyTag(uId,static_cast<void*>(&SoundTagData));
Exit0:
	;
}
void KdlgSoundTag::OnBnClickedCheck1()
{
	// TODO: 在此添加控件通知处理程序代码
	HRESULT hRetCode = E_FAIL;
	KG_PROCESS_ERROR(m_pSoundTag);
	KG_PROCESS_ERROR(m_uFrame != uIllegal);
	{
			SoundDataInfoVersion3 SoundTagData;
			UINT uId = 0;
			uId = GETID(m_uFrame,3);
			hRetCode = GetKeyFrameData(m_uFrame,&SoundTagData);
			KG_COM_PROCESS_ERROR(hRetCode);
			SoundTagData.bLoop = ((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck();
			m_pSoundTag->ModifyTag(uId,static_cast<void*>(&SoundTagData));
	}
	{
		SoundDataInfoVersion2 SoundTagData;
		UINT uId = 0;
		uId = GETID(m_uFrame,2);
		hRetCode = GetKeyFrameData(m_uFrame,&SoundTagData);
		KG_COM_PROCESS_ERROR(hRetCode);
		SoundTagData.bLoop = ((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck();
		m_pSoundTag->ModifyTag(uId,static_cast<void*>(&SoundTagData));
	}
Exit0:
	;
}

void KdlgSoundTag::OnBnClickedCheck2()
{
	// TODO: 在此添加控件通知处理程序代码
	HRESULT hRetCode = E_FAIL;
	KG_PROCESS_ERROR(m_pSoundTag);
	KG_PROCESS_ERROR(m_uFrame != uIllegal);
	{
		SoundDataInfoVersion3 SoundTagData;
		UINT uId = 0;
		uId = GETID(m_uFrame,3);
		hRetCode = GetKeyFrameData(m_uFrame,&SoundTagData);
		KG_COM_PROCESS_ERROR(hRetCode);
		SoundTagData.InterruptMode = ((CButton*)GetDlgItem(IDC_CHECK2))->GetCheck()?SPI_ENABLE:SPI_DISABLE;
		m_pSoundTag->ModifyTag(uId,static_cast<void*>(&SoundTagData));
	}
	{
		SoundDataInfoVersion2 SoundTagData;
		UINT uId = 0;
		uId = GETID(m_uFrame,2);
		hRetCode = GetKeyFrameData(m_uFrame,&SoundTagData);
		KG_COM_PROCESS_ERROR(hRetCode);
		SoundTagData.InterruptMode = ((CButton*)GetDlgItem(IDC_CHECK2))->GetCheck()?SPI_ENABLE:SPI_DISABLE;
		m_pSoundTag->ModifyTag(uId,static_cast<void*>(&SoundTagData));
	}
Exit0:
	;
}

void KdlgSoundTag::OnBnClickedCheck3()
{
	// TODO: 在此添加控件通知处理程序代码
	HRESULT hRetCode = E_FAIL;
	KG_PROCESS_ERROR(m_pSoundTag);
	KG_PROCESS_ERROR(m_uFrame != uIllegal);
	{
		SoundDataInfoVersion3 SoundTagData;
		UINT uId = 0;
		uId = GETID(m_uFrame,3);
		hRetCode = GetKeyFrameData(m_uFrame,&SoundTagData);
		KG_COM_PROCESS_ERROR(hRetCode);
		SoundTagData.MotionChangeState = ((CButton*)GetDlgItem(IDC_CHECK3))->GetCheck()?SMCS_KEEP:SMCS_DELETE;
		m_pSoundTag->ModifyTag(uId,static_cast<void*>(&SoundTagData));
	}
	{
		SoundDataInfoVersion2 SoundTagData;
		UINT uId = 0;
		uId = GETID(m_uFrame,2);
		hRetCode = GetKeyFrameData(m_uFrame,&SoundTagData);
		KG_COM_PROCESS_ERROR(hRetCode);
		SoundTagData.MotionChangeState = ((CButton*)GetDlgItem(IDC_CHECK3))->GetCheck()?SMCS_KEEP:SMCS_DELETE;
		m_pSoundTag->ModifyTag(uId,static_cast<void*>(&SoundTagData));
	}
Exit0:
	;
}


void KdlgSoundTag::DeleteVersion2()
{
	SoundDataInfoVersion2 SoundTagData;
	HRESULT hRetCode = E_FAIL;
	UINT uId = 0;
	KG_PROCESS_ERROR(m_pSoundTag);
	KG_PROCESS_ERROR(m_uFrame != uIllegal);
	hRetCode = GetKeyFrameData(m_uFrame,&SoundTagData);
	SoundTagData.dwValidateCandidate = 0;
	uId = GETID(m_uFrame,2);
	m_pSoundTag->ModifyTag(uId,static_cast<void*>(&SoundTagData));
	(this->*s_FillControl[m_uVersion-2])();
	OnBnClickedButtonToversion3();
Exit0:
	;
}
void KdlgSoundTag::DeleteVersion3()
{
	SoundDataInfoVersion3 SoundTagData;
	HRESULT hRetCode = E_FAIL;
	UINT uId = 0;
	KG_PROCESS_ERROR(m_pSoundTag);
	KG_PROCESS_ERROR(m_uFrame != uIllegal);
	hRetCode = GetKeyFrameData(m_uFrame,&SoundTagData);
	SoundTagData.soundType  = ST_NONE;
	uId = GETID(m_uFrame,3);
	m_pSoundTag->ModifyTag(uId,static_cast<void*>(&SoundTagData));
	(this->*s_FillControl[m_uVersion-2])();
	OnBnClickedButtonVersion2();
Exit0:
	;
}
void KdlgSoundTag::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	(this->*s_Save[m_uVersion-2])();
	KG_PROCESS_ERROR(m_pSoundTag);
	KG_PROCESS_ERROR(m_uFrame != uIllegal);
	Clear();
Exit0:
	CDialog::OnClose();
}

void KdlgSoundTag::Clear()
{
	BOOL bV2 = FALSE;
	BOOL bV3 = FALSE;
	UINT uId = 0;
	HRESULT hRetCode = E_FAIL; 
	const SoundDataInfoVersion2 * pSoundData = NULL;
	const SoundDataInfoVersion3 * pSoundDataVersion3 = NULL;
	KG_PROCESS_ERROR(m_pSoundTag);
	KG_PROCESS_ERROR(m_uFrame != uIllegal);
	hRetCode = m_pSoundTag->GetKeyFrameData(m_uFrame,&pSoundData);
	KG_COM_PROCESS_ERROR(hRetCode);
	hRetCode = m_pSoundTag->GetKeyFrameData(m_uFrame,&pSoundDataVersion3);
	KG_COM_PROCESS_ERROR(hRetCode);
	if(pSoundData->dwValidateCandidate == 0)
		bV2 = TRUE;
	if(pSoundDataVersion3->soundType == ST_NONE)
		bV3 = TRUE;
	if(bV2 && bV3)
	{
		m_pSoundTag->RemoveTag(m_uFrame);
	}
Exit0:
	;
}

void KdlgSoundTag::Copy()
{
	m_uCopyFrame = m_uFrame;
}
void KdlgSoundTag::Paste()
{
	_ASSERTE(m_pSoundTag);  
	HRESULT hResult = E_FAIL;
	HRESULT hRetcode = E_FAIL; 
	UINT uIdSrc = 0;
	UINT uIdDest = 0;
	const SoundDataInfoVersion2 *pSoundTagData = NULL;
	KG_PROCESS_ERROR(m_uCopyFrame != uIllegal);
	KG_PROCESS_ERROR(m_uCopyFrame != m_uFrame);
	uIdDest = GETID(m_uFrame,2);
	uIdSrc = GETID(m_uCopyFrame,2);
	m_pSoundTag->ClearTag(uIdDest);
	
	uIdDest = GETID(m_uFrame,3);
	uIdSrc = GETID(m_uCopyFrame,3);
	m_pSoundTag->ClearTag(uIdDest);
	
	m_pSoundTag->CopyTag(m_uCopyFrame,m_uFrame);

	(this->*s_FillControl[m_uVersion-2])();
Exit0:
	;
}
extern BYTE* g_SceneEditorKeyState;
BOOL KdlgSoundTag::PreTranslateMessage(MSG* pMsg)
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
					Copy();
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
					Paste();
					dwLastTimeCopy = dwCurrentTime;
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
			if(m_uVersion == 2)
				OnBnClickedButtondelete();
			else
				DeleteVersion3();
			g_SceneEditorKeyState[VK_DELETE] = 0;
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}