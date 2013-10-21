// KDlgMotionTag.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KDlgMotionTag.h"
#include "KSceneModelEditorAnimationPanel.h"
#include "IEKG3DMesh.h"
#include "IEKG3DAnimation.h"
// KDlgMotionTag dialog
#define CREATE_ID(a, b) ((a << 16) | b)
static int s_nGroupDamamgePercentage[] = {
	IDC_EDIT_DP,
	IDC_STATIC_DP
};

const TCHAR *s_strConfigFile = "data\\public\\TagEditor\\TagEditor.ini";
const TCHAR *s_strPlayerEquipRepresent = "PlayerEquipRepresent";

static int s_nGroupMeteor[] = { 
	IDC_METEOR_BEGIN,
	IDC_METEOR_END,
	IDC_SOCKETNAME,
	IDC_COMBO_SOCKET,
	IDC_LIST_SOCKET,
	IDC_BUTTON_LOAD,
	IDC_LIST_BINDED,
	IDC_BINDED_SOCKET
};

static int s_nGroupFace[] = {
	IDC_STATIC_FACEMOTIONTYPE,
	IDC_COMBO_FACEMOTIONTYPE,
	IDC_BUTTON_SELANI,
	IDC_STATIC_MOTIONDIR,
	IDC_EDIT_FACEMOTIONDIR,
	IDC_STATIC_PLAYRATE,
	IDC_EDIT_PLAYRATE,
	IDC_STATIC_PLAYSPEED,
	IDC_EDIT_PLAYSPEED,
	IDC_STATIC_PLAYTYPE,
	IDC_RADIO_LOOP,
	IDC_RADIO_ONCE,
	IDC_CHECK_KEEP,
};

static int s_nGroupMissile[] = 
{
	IDC_STATIC_MISSILEID,
	IDC_STATIC_MISSILEDAMAGE,
	IDC_EDIT_MISSILE_ID,
	IDC_EDIT_MISSILEDAMAGE,
};

static int s_nGroupEquipSfx[] = 
{
	IDC_STATIC_EQUIPSFX,
	IDC_COMBO_EQUIPTYPE,
	IDC_STATIC_EQUIPSFXID,
	IDC_EDIT_EQUIPSFXID,
};

static int s_nGroundEffect[] = // add by chenganlai
{
	IDC_STATIC_GROUNDEFFECT,
	IDC_STATIC_FOOTTYPE,
	IDC_COMBO_FOOTTYPE,
};
static int* s_pControlGroup[] = 
{
	s_nGroupDamamgePercentage, 
	s_nGroupMeteor, 
	s_nGroupFace,
	s_nGroupMissile,
	s_nGroupEquipSfx,
	s_nGroundEffect,// add by chenganlai
};

static int s_GroupFirstItem[] = 
{
	IDC_STATIC_DP,
	IDC_SOCKETNAME,
	IDC_STATIC_FACEMOTIONTYPE,
	IDC_STATIC_MISSILEID,
	IDC_STATIC_EQUIPSFX,
	IDC_STATIC_GROUNDEFFECT,// add by chenganlai
};

static DWORD  s_GroupControlCount[] = { 
	sizeof(s_nGroupDamamgePercentage) / sizeof(int) ,
	sizeof(s_nGroupMeteor) / sizeof(int),
	sizeof(s_nGroupFace) / sizeof(int),
	sizeof(s_nGroupMissile) / sizeof(int),
	sizeof(s_nGroupEquipSfx) / sizeof(int),
	sizeof(s_nGroundEffect) / sizeof(int), // add by chenganlai
};

static TCHAR* s_strTypes[] = {
	"伤害百分比",//MTBT_DAMAGEPERCENTAGE
	"刀光",//MTBT_METEOR
	"面部表情",//MTBT_FACEMOTION
	"子弹",//MTBT_MISSILE
	"装备特效", //MTBT_EQUIPSFX
	"地面效果",// MTBT_GROUNDEFFECT    // add by chenganlai
};
UINT KDlgMotionTag::m_uFrameCopy = 0xfffffff;// add 
KDlgMotionTag::pfFillUserInterface KDlgMotionTag::s_FillUserInterface[] =
{
	&KDlgMotionTag::FillDamage,
	&KDlgMotionTag::FillMetero,
	&KDlgMotionTag::FillFaceMotion,
	&KDlgMotionTag::FillMissile,
	&KDlgMotionTag::FillEquipSfx,
	&KDlgMotionTag::FillGroundEffect, // add by chenganlai
};

KDlgMotionTag::pfUpdateTagData KDlgMotionTag::s_UpdateTagData[] = 
{
	&KDlgMotionTag::UpdateDamage,
	&KDlgMotionTag::UpdateMetero,
	&KDlgMotionTag::UpdateFaceMotion,
	&KDlgMotionTag::UpdateMissile,
	&KDlgMotionTag::UpdateEquipSfx,
	&KDlgMotionTag::UpdateGroundEffect,// add by chenganlai
};

IMPLEMENT_DYNAMIC(KDlgMotionTag, CDialog)

KDlgMotionTag::KDlgMotionTag(IEKG3DAnimationMotionTag* pTag,
							 CWnd* pParent /*=NULL*/)
	: CDialog(KDlgMotionTag::IDD, pParent)
    , m_strTagName(_T(""))
    , m_uFrame(0)
    , m_pAnimationPanel(NULL)
	, m_pMotionTag(pTag)
	, m_fDamage(0)
	, m_fPlayRate(0)
	, m_fPlaySpeed(0)
	, m_nLastSel(-1)
	, m_dwEquipSfxID(0)
{
}

KDlgMotionTag::~KDlgMotionTag()
{
}

void KDlgMotionTag::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TAGNAME, m_strTagName);
	DDX_Text(pDX, IDC_EDIT_PLAYRATE, m_fPlayRate);
	DDX_Text(pDX, IDC_EDIT_PLAYSPEED, m_fPlaySpeed);
	DDX_Control(pDX, IDC_COMBO_SOCKET, m_Combo_Socket);
	DDX_Control(pDX, IDC_LIST_SOCKET, m_ListSocket);
	DDX_Control(pDX, IDC_LIST_BINDED, m_listBinded);
	DDX_Control(pDX, IDC_COMBO_SUBTAG, m_comboSubTag);
	DDX_Control(pDX, IDC_COMBO_ADDTYPE, m_comboAddTag);
	DDX_Control(pDX, IDC_COMBO_FACEMOTIONTYPE, m_comboMotionType);
	DDX_Control(pDX, IDC_RADIO_LOOP, m_buttonLoop);
	DDX_Control(pDX, IDC_RADIO_ONCE, m_buttonOnce);
	DDX_Text(pDX, IDC_EDIT_DP, m_fDamage);
	DDX_Text(pDX, IDC_EDIT_FACEMOTIONDIR, m_strFaceMotionDir);
	DDX_Control(pDX, IDC_RADIO_START, m_buttonStart);
	DDX_Control(pDX, IDC_RADIO_END, m_buttonEnd);
	DDX_Control(pDX, IDC_RADIO_NONE, m_buttonNone);
	DDX_Text(pDX, IDC_EDIT_MISSILE_ID, m_dwMissileID);
	DDX_Text(pDX, IDC_EDIT_MISSILEDAMAGE, m_fMissileDamage);
	DDX_Control(pDX, IDC_CHECK_KEEP, m_ctlKeepMotion);
	DDX_Text(pDX, IDC_EDIT_EQUIPSFXID, m_dwEquipSfxID);
	DDX_Control(pDX, IDC_COMBO_EQUIPTYPE, m_comboEquipSfxType);
    DDX_Control(pDX, IDC_COMBO_FOOTTYPE, m_ComSFXType);
    
}


BEGIN_MESSAGE_MAP(KDlgMotionTag, CDialog)
    ON_BN_CLICKED(IDOK, &KDlgMotionTag::OnBnClickedOk)
    ON_WM_CLOSE()
    ON_BN_CLICKED(IDC_BUTTON_DELETE, &KDlgMotionTag::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_METEOR_BEGIN, &KDlgMotionTag::OnBnClickedMeteorBegin)
	ON_BN_CLICKED(IDC_METEOR_END, &KDlgMotionTag::OnBnClickedMeteorEnd)
	ON_CBN_SELCHANGE(IDC_COMBO_SOCKET, &KDlgMotionTag::OnCbnSelchangeComboSocket)
	ON_LBN_SELCHANGE(IDC_LIST_SOCKET, &KDlgMotionTag::OnLbnSelchangeListSocket)
	ON_BN_CLICKED(IDC_BUTTON_LOAD, &KDlgMotionTag::OnBnClickedButtonLoad)
	ON_LBN_SELCHANGE(IDC_LIST_BINDED, &KDlgMotionTag::OnLbnSelchangeListBinded)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &KDlgMotionTag::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, &KDlgMotionTag::OnBnClickedButtonDel)
	ON_CBN_SELCHANGE(IDC_COMBO_SUBTAG, &KDlgMotionTag::OnCbnSelchangeComboSubtag)
	ON_CBN_SELCHANGE(IDC_COMBO_FACEMOTIONTYPE, &KDlgMotionTag::OnCbnSelchangeComboFacemotiontype)
	ON_BN_CLICKED(IDC_BUTTON_SELANI, &KDlgMotionTag::OnBnClickedButtonSelani)
	ON_BN_CLICKED(IDC_RADIO_START, &KDlgMotionTag::OnBnClickedRadioStart)
	ON_BN_CLICKED(IDC_RADIO_END, &KDlgMotionTag::OnBnClickedRadioEnd)
	ON_BN_CLICKED(IDC_RADIO_NONE, &KDlgMotionTag::OnBnClickedRadioNone)
	ON_BN_CLICKED(IDC_RADIO_ONCE, &KDlgMotionTag::OnBnClickedRadioOnce)
	ON_BN_CLICKED(IDC_RADIO_LOOP, &KDlgMotionTag::OnBnClickedRadioLoop)
	ON_BN_CLICKED(IDC_BUTTON_COPY, &KDlgMotionTag::OnBnClickedButtonCopy)
	ON_BN_CLICKED(IDC_BUTTON_PASTE, &KDlgMotionTag::OnBnClickedButtonPaste)
	ON_EN_CHANGE(IDC_EDIT_TAGNAME, &KDlgMotionTag::OnEnChangeEditTagname)
END_MESSAGE_MAP()


// KDlgMotionTag message handlers
void KDlgMotionTag::OnBnClickedOk()
{
    UpdateData();
    strcpy_s(m_Data.strTagName,
		TAG_NAME_MAX_LENGTH, 
		m_strTagName);
}
void KDlgMotionTag::SetcomboSubTagCursel(int nCursel)
{
	m_comboSubTag.SetCurSel(nCursel);
	m_nLastSel = nCursel;
}
BOOL KDlgMotionTag::OnInitDialog()
{
    CDialog::OnInitDialog();

	for (size_t i = 0; i < sizeof(s_pControlGroup) / sizeof(int*); i++)
	{
		//把控件都移动到正确的位置

		//计算控件需要移动的距离
		RECT rectWindow;
		RECT rectWindowCtrl;
		GetDlgItem(s_GroupFirstItem[i])->GetWindowRect(&rectWindowCtrl);
		GetDlgItem(IDC_STATIC_CONTROLPOS)->GetWindowRect(&rectWindow);

		int nMoveX = rectWindowCtrl.left - rectWindow.left;
		int nMoveY = rectWindowCtrl.top - rectWindow.top;

		for (DWORD j = 0; j < s_GroupControlCount[i]; j++)
		{
			CWnd *pCtrl = GetDlgItem(s_pControlGroup[i][j]);
			pCtrl->GetWindowRect(&rectWindowCtrl);
			rectWindowCtrl.left -= nMoveX;
			rectWindowCtrl.right -= nMoveX;
			rectWindowCtrl.top -= nMoveY;
			rectWindowCtrl.bottom -= nMoveY;
			ScreenToClient(&rectWindowCtrl);
			pCtrl->MoveWindow(&rectWindowCtrl);
			pCtrl->ShowWindow(SW_HIDE);
		}
	}

	RECT rectFrame;
	GetDlgItem(IDC_STATIC_FRAME)->GetWindowRect(&rectFrame);
	int nWidth = abs(rectFrame.right - rectFrame.left);
	int nBottom = rectFrame.bottom + 20;
	RECT rectWindow;
	GetWindowRect(&rectWindow);
	rectWindow.right = rectWindow.left + nWidth + 20;
	rectWindow.bottom = nBottom;
	MoveWindow(&rectWindow);

	LoadPlayerEquipRepresentConfig();

	long ldummy = 0;
	m_pAnimationPanel->OnMotionTagLoadAnimationseekbar1(0, &ldummy);
	
	KG_PROCESS_ERROR(m_pAnimationPanel);
    for (size_t i = 0; i < sizeof(s_strTypes) / sizeof(TCHAR*); i++)
	{
		m_comboAddTag.AddString(s_strTypes[i]);
	}
	m_comboAddTag.SetCurSel(0);

	FillSubList();
	if (m_comboSubTag.GetCount())
	{
		//m_comboSubTag.SetCurSel(0);
		SetcomboSubTagCursel(0);
		Update();
		//OnCbnSelchangeComboSubtag();
	}
	
	if (m_uFrame == m_pMotionTag->GetMotionStart())
	{
		m_buttonStart.SetCheck(BST_CHECKED);
	}
	else if (m_uFrame == m_pMotionTag->GetMotionEnd())
	{
		m_buttonEnd.SetCheck(BST_CHECKED);
	}
	else
	{
		m_buttonNone.SetCheck(BST_CHECKED);
	}

	

Exit0:
    return TRUE;
}

HRESULT KDlgMotionTag::LoadPlayerEquipRepresentConfig()
{
	int nNumReprsentType = 0;
	HRESULT hResult = E_FAIL;
	m_comboEquipSfxType.ResetContent();

	IIniFile *pConfig = g_OpenIniFile(s_strConfigFile);
	KG_PROCESS_ERROR(pConfig);
	
	
	pConfig->GetInteger(s_strPlayerEquipRepresent, "NumType", 0, &nNumReprsentType);
	for (int i = 0; i < nNumReprsentType; i++)
	{
		TCHAR strTypeKey[MAX_PATH];
		TCHAR strTypeName[MAX_PATH];
		sprintf_s(strTypeKey,
			MAX_PATH,
			"Type%d",
			i);
		pConfig->GetString(s_strPlayerEquipRepresent, 
			strTypeKey, 
			"", 
			strTypeName, 
			MAX_PATH);
		m_comboEquipSfxType.AddString(strTypeName);
	}

	if (nNumReprsentType)
	{
		m_comboEquipSfxType.SetCurSel(0);
	}
	
	hResult = S_OK;
Exit0:
	SAFE_RELEASE(pConfig);
	return hResult;
}

void KDlgMotionTag::FillFaceMotionType()
{
	HRESULT hResult = E_FAIL;
	IKG3DFaceMotionSettings *pSettings = g_GetEngineManager()->GetFaceMotionSettings();
	m_comboMotionType.ResetContent();

	KG_PROCESS_ERROR(pSettings);

	DWORD dwNumMotion = pSettings->GetNumFaceMotion();
	for (DWORD i = 0; i < dwNumMotion; i++)
	{
		m_comboMotionType.AddString(pSettings->GetFaceMotionTypeName(i));
	}
Exit0:
	;
}


void KDlgMotionTag::OnClose()
{
	DWORD dwType = 0;
	int nCurSel = m_comboSubTag.GetCurSel();
	KG_PROCESS_ERROR(nCurSel != CB_ERR);
	m_nLastSel  = nCurSel;
	if (m_nLastSel != -1)
	{
		dwType = m_pMotionTag->GetBlockType(m_uFrame, m_nLastSel);
		KG_PROCESS_ERROR(dwType < MTBT_COUNT);

		(this->*s_UpdateTagData[dwType])();
	}
Exit0:
	if (m_pMotionTag)
	{
		m_pMotionTag->ClearUnused();
		UpdateDetail();
	}
    CDialog::OnClose();
}
void KDlgMotionTag::UpdateDetail()
{
	_ASSERTE(m_pMotionTag);
	MotionTagDataRuntime *pMotionTagDataRunTime = NULL;
	UpdateData();
	m_pMotionTag->GetKeyFrame(m_uFrame,pMotionTagDataRunTime);
	KG_PROCESS_ERROR(pMotionTagDataRunTime);
	strcpy_s(pMotionTagDataRunTime->strTagName,m_strTagName);
Exit0:
	;
	//m_buttonNone.GetCheck()
}
void KDlgMotionTag::OnBnClickedButtonDelete()
{
	MotionTagDataRuntime *pMotionTagDataRunTime = NULL;
    KG_PROCESS_ERROR(m_pAnimationPanel);
	for (int i = m_comboSubTag.GetCount() - 1; i >= 0; i--)
	{
		m_pAnimationPanel->OnMotionTagDeleteAnimationseekbar1((m_uFrame << 16) | i);
	}
	OnOK();

Exit0:
	if (m_pMotionTag)
	{
		m_pMotionTag->ClearUnused();
	}
	;
}


void KDlgMotionTag::SetMeteorSwith(BOOL bStart)
{
	int nSel = m_ListSocket.GetCurSel();
	KG_PROCESS_ERROR(nSel != LB_ERR);

	MotionTagBlockMeteor *pMeteor = static_cast<MotionTagBlockMeteor*>(GetCurrentSubBlock());
	KG_PROCESS_ERROR(pMeteor);
	
	pMeteor->dwFlags[nSel] = bStart;
	return ;
Exit0:
	MessageBox("没有选中任何Socket！");

}

void KDlgMotionTag::OnBnClickedMeteorBegin()
{
	SetMeteorSwith(TRUE);
}

void KDlgMotionTag::OnBnClickedMeteorEnd()
{
	SetMeteorSwith(FALSE);
}

void KDlgMotionTag::FillComboSocket()
{
	m_Combo_Socket.ResetContent();
	
	HRESULT hr = E_FAIL;
	HRESULT hRetCode = NULL;
	IEKG3DModel* pCurModel;
	int dwNumSockets = 0;
	LPCTSTR pszSocketName = NULL;
	GET_MODEL_EDITOR();
	hRetCode = pEditor->GetCurModel(&pCurModel);

	KGLOG_COM_PROCESS_ERROR(hRetCode);
	
	if(SUCCEEDED(pCurModel->GetNumSocket(&dwNumSockets)))
	{
		for(int i = 0; i < dwNumSockets; i++)
		{
			hr = pCurModel->GetSocketName(i, &pszSocketName);
			KGLOG_COM_PROCESS_ERROR(hr);
			int nIndex = m_Combo_Socket.AddString(pszSocketName);
		}
	}
Exit0:
	return ;
}

void* KDlgMotionTag::GetCurrentSubBlock()
{
	void *pReturn = NULL;
	MotionTagDataRuntime *pTag = NULL;
	m_pMotionTag->GetKeyFrame(m_uFrame, pTag);

	int nCurSel = m_comboSubTag.GetCurSel();
	KG_PROCESS_ERROR(nCurSel != CB_ERR);
	KG_PROCESS_ERROR(nCurSel < static_cast<int>(pTag->Blocks.size()));

	pReturn = pTag->Blocks[nCurSel];
Exit0:
	return pReturn;
}

void KDlgMotionTag::OnCbnSelchangeComboSocket()
{
	int nCount = m_ListSocket.GetCount();
	KG_PROCESS_ERROR(nCount < MOTIONTAG_MAX_FLAG - 1);

	int nSel = m_Combo_Socket.GetCurSel();
	if(nSel >= 0)
	{	
		MotionTagBlockMeteor *pMeteor = NULL;
		CString ComboStr,ListStr;
		m_Combo_Socket.GetLBText(nSel, ComboStr);
		int nFind = m_ListSocket.FindString(0, ComboStr);
		KG_PROCESS_ERROR(nFind < 0);

		m_ListSocket.AddString(ComboStr);
		pMeteor = static_cast<MotionTagBlockMeteor*>(GetCurrentSubBlock());
		KG_PROCESS_ERROR(pMeteor);

		strcpy_s(pMeteor->strIndicator[nCount], 
			MAX_BONE_NAME_LENGTH,
			ComboStr);
		pMeteor->dwNumFlag = static_cast<DWORD>(nCount + 1);
	}
	return;
Exit0:
	MessageBox("ListSocket已满或所选的Socket已存在！");
}



void KDlgMotionTag::OnLbnSelchangeListSocket()
{
	MotionTagBlockMeteor *pMeteor = static_cast<MotionTagBlockMeteor*>(GetCurrentSubBlock());
	KG_PROCESS_ERROR(pMeteor);

	int nSel = m_ListSocket.GetCurSel();
	KG_PROCESS_ERROR(nSel != LB_ERR);
	KG_PROCESS_ERROR(nSel < MOTIONTAG_MAX_FLAG);

	BOOL b = pMeteor->dwFlags[nSel];
	((CButton*)GetDlgItem(IDC_METEOR_BEGIN))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_METEOR_END))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_METEOR_BEGIN))->SetCheck(b);
	((CButton*)GetDlgItem(IDC_METEOR_END))->SetCheck(!b);
	return ;
Exit0:
	((CButton*)GetDlgItem(IDC_METEOR_BEGIN))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_METEOR_END))->EnableWindow(FALSE);
}

void KDlgMotionTag::FillListSocket(MotionTagBlockMeteor *pData)
{
	m_ListSocket.ResetContent();
	KG_PROCESS_ERROR(pData);
	
	for (DWORD i = 0 ; i < pData->dwNumFlag; i++)
	{
		m_ListSocket.AddString(pData->strIndicator[i]);
	}
Exit0:
	;
}

void KDlgMotionTag::FillListBinded()
{
	m_listBinded.ResetContent();
	DWORD dwType;
	IEKG3DModel* pCurModel = NULL;
	vector<IEKG3DModel*> vecChilds;
	vector<IEKG3DModel*>::iterator i;
	GET_MODEL_EDITOR();
	pEditor->GetCurModel(&pCurModel);
	KG_PROCESS_ERROR(pCurModel);
	pCurModel->GetAllChilds(vecChilds);
	i= vecChilds.begin();
	while (i != vecChilds.end())
	{
		
		(*i)->GetType(&dwType) ;
		if(dwType == MESHTYPE_SFX)
		{
			LPCTSTR strName;
			(*i)->IEGetBindBoneName(&strName);
			int nFind = m_listBinded.FindString(0,strName);
			if(nFind < 0)
			{
				m_listBinded.AddString(strName);
			}
			
		}
		i++;
	}
Exit0:
	;
}
void KDlgMotionTag::OnBnClickedButtonLoad()
{
	HRESULT hr = E_FAIL;
	IEKG3DModel* pCurModel;
	TCHAR strFileNameBuffer[MAX_PATH * 10] = "";
	TCHAR strFilter[] = "SFX Files (*.Sfx)|*.Sfx|";
	TCHAR strDefaultPath[MAX_PATH];
	LPCTSTR pszFullName = NULL;
	strDefaultPath[0] = '\0';

	CFileDialog DlgLoad(TRUE, ".Sfx", "test.Sfx", OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT, strFilter);
	
	int nSel = m_ListSocket.GetCurSel();
	if(nSel<0)
	{
		MessageBox("没有选中任何Socket！","Error");
		return;
	}

	GET_MODEL_EDITOR();
	pEditor->GetCurModel(&pCurModel);
	KG_PROCESS_ERROR(pCurModel);
	sprintf(strDefaultPath, "%sdata\\public\\Meteor", g_szDefWorkDirectory);

	DlgLoad.m_ofn.lpstrFile = strFileNameBuffer;
	DlgLoad.m_ofn.nMaxFile = sizeof(strFileNameBuffer);
	DlgLoad.m_ofn.lpstrInitialDir = strDefaultPath;

	KG_PROCESS_ERROR(DlgLoad.DoModal() == IDOK);

	POSITION FilePos = DlgLoad.GetStartPosition();

	while(FilePos)
	{
		CString strPathName = DlgLoad.GetNextPathName(FilePos);
		CString SocketName="";
		IEKG3DSFX* pSFXMeteor = NULL;
		m_ListSocket.GetText(nSel,SocketName);
		hr = pEditor->LoadSFXMeteor(strPathName,SocketName,pCurModel);
		if (FAILED(hr))
		{
			MessageBox(strPathName, "Error");
		}
		
	}
	FillListBinded();
	hr = S_OK;

Exit0:
	return;
}

void KDlgMotionTag::OnLbnSelchangeListBinded()
{
	int nCount = m_ListSocket.GetCount();
	KG_PROCESS_ERROR(nCount < MOTIONTAG_MAX_FLAG - 1);
	
	int nSel = m_listBinded.GetCurSel();
	if(nSel >= 0)
	{	
		MotionTagBlockMeteor *pMeteor = NULL;
		CString BindedStr,ListStr;
		m_listBinded.GetText(nSel,BindedStr);
		int nFind = m_ListSocket.FindString(0,BindedStr);
		KG_PROCESS_ERROR(nFind <0);
		m_ListSocket.AddString(BindedStr);

		pMeteor = static_cast<MotionTagBlockMeteor*>(GetCurrentSubBlock());
		KG_PROCESS_ERROR(pMeteor);

		strcpy_s(pMeteor->strIndicator[pMeteor->dwNumFlag],
			MAX_BONE_NAME_LENGTH,
			BindedStr);
		pMeteor->dwNumFlag = static_cast<DWORD>(nCount + 1);
	}
	return;
Exit0:
	MessageBox("ListSocket已满或所选的Socket已存在！");
}

void KDlgMotionTag::FillSubList()
{
	MotionTagDataRuntime *pMotionTagData = NULL;
	m_comboSubTag.ResetContent();
	m_pMotionTag->GetKeyFrame(m_uFrame, pMotionTagData);
	KG_PROCESS_ERROR(pMotionTagData);

	for (size_t i = 0; i < pMotionTagData->Blocks.size(); i++)
	{
		DWORD dwType = *(static_cast<DWORD*>(pMotionTagData->Blocks[i]));
		if (dwType > MTBT_COUNT)
			continue;
		m_comboSubTag.AddString(s_strTypes[dwType]);
	}

	m_strTagName = pMotionTagData->strTagName;
	UpdateData(FALSE);
Exit0:
	;
}
void KDlgMotionTag::SaveState()
{
	DWORD dwType = 0;
	if (m_nLastSel != -1)
	{
		dwType = m_pMotionTag->GetBlockType(m_uFrame, m_nLastSel);
		KG_PROCESS_ERROR(dwType < MTBT_COUNT);

		(this->*s_UpdateTagData[dwType])();
	}
Exit0:
	;
}
void KDlgMotionTag::Update()
{
	DWORD dwType = 0;
	MotionTagDataRuntime *pTagData = NULL;
	int nCurSel = m_comboSubTag.GetCurSel();
	KG_PROCESS_ERROR(nCurSel != CB_ERR);

	dwType = m_pMotionTag->GetBlockType(m_uFrame, nCurSel);
	KG_PROCESS_ERROR(dwType < MTBT_COUNT);

	DWORD dwNumGroup = sizeof(s_pControlGroup) / sizeof(DWORD*);
	for (DWORD i = 0; i < dwNumGroup; i++)
	{
		int nShowWindow = SW_HIDE;
		if (dwType == i)
		{
			nShowWindow = SW_SHOW;
		}
		for (DWORD j = 0; j < s_GroupControlCount[i]; j++)
		{
			GetDlgItem(s_pControlGroup[i][j])->ShowWindow(nShowWindow);
		}
	}

	m_pMotionTag->GetKeyFrame(m_uFrame, pTagData);
	KG_PROCESS_ERROR(pTagData);

	m_nLastSel = nCurSel;
	(this->*s_FillUserInterface[dwType])(pTagData->Blocks[nCurSel]);
Exit0:
	;
}
void KDlgMotionTag::OnBnClickedButtonAdd()
{
	int nCount = m_comboSubTag.GetCount();
	DWORD dwType = 0;
	UINT uID = GETID(m_uFrame, nCount);
	DWORD dwCurSel = static_cast<DWORD>(m_comboAddTag.GetCurSel());
	MotionTagDataRuntime *pTagData = NULL;
	MotionTagDataRuntime *pTagData1 = NULL;
	KG_PROCESS_ERROR(dwCurSel != CB_ERR);
	m_pMotionTag->GetKeyFrame(m_uFrame, pTagData);
	if(!pTagData)
	{
		m_pAnimationPanel->OnNewMotionTagAnimationseekbar1(long(m_uFrame));
		m_pMotionTag->GetKeyFrame(m_uFrame,pTagData1);
		strcpy_s(pTagData1->strTagName,TAG_NAME_MAX_LENGTH,m_strTagName);
	}
	void *pNewTag = m_pMotionTag->GetNewBlock(dwCurSel);
	m_pMotionTag->ModifyTag(uID, pNewTag);
	SAFE_DELETE_ARRAY(pNewTag);
	FillSubList();

	nCount = m_comboSubTag.GetCount();
	//KG_PROCESS_ERROR(nCount);

	SetcomboSubTagCursel(nCount - 1);
	Update();

Exit0:
	;
}

void KDlgMotionTag::OnBnClickedButtonDel()
{
	int nCurSel = m_comboSubTag.GetCurSel();
	KG_PROCESS_ERROR(nCurSel != CB_ERR);
	KG_PROCESS_ERROR(m_pAnimationPanel);
	UINT uID = GETID(m_uFrame, nCurSel);
	m_pAnimationPanel->OnMotionTagDeleteAnimationseekbar1(uID);
	//RemoveTag(uID);
	FillSubList();

	int nCount = m_comboSubTag.GetCount();
	//KG_PROCESS_ERROR(nCount);
	//if(nCount == 0)
	SetcomboSubTagCursel(min(nCurSel + 1, nCount - 1));
	Update();
Exit0:
	;
}

void KDlgMotionTag::FillMetero(void *pData)
{
	MotionTagBlockMeteor *pMeteor = static_cast<MotionTagBlockMeteor*>(pData);
	KG_PROCESS_ERROR(pMeteor);
	
	FillComboSocket();
	FillListBinded();
	FillListSocket(pMeteor);
Exit0:
	;
}

void KDlgMotionTag::FillFaceMotion(void *pData)
{
	MotionTagBlockFaceMotion *pFace = static_cast<MotionTagBlockFaceMotion*>(pData);
	IKG3DFaceMotionSettings *pFaceMotionSettings = g_GetEngineManager()->GetFaceMotionSettings();
	KG_PROCESS_ERROR(pFace);

	FillFaceMotionType();
	//m_buttonSwitch.SetCheck(pFace->bKeepOnMotionFinish ? BST_UNCHECKED : BST_CHECKED);
	//m_buttonKeep.SetCheck(pFace->bKeepOnMotionFinish ? BST_CHECKED : BST_UNCHECKED);
	m_buttonLoop.SetCheck(pFace->dwPlayType == ENU_ANIMATIONPLAY_CIRCLE ? BST_CHECKED : BST_UNCHECKED);
	m_buttonOnce.SetCheck(pFace->dwPlayType == ENU_ANIMATIONPLAY_CIRCLE ? BST_UNCHECKED : BST_CHECKED);
	BOOL bEnable = pFace->dwPlayType != ENU_ANIMATIONPLAY_CIRCLE;
	m_ctlKeepMotion.EnableWindow(bEnable);
	if (bEnable)
	{
		m_ctlKeepMotion.SetCheck(pFace->bKeepOnMotionFinish ? BST_CHECKED : BST_UNCHECKED);
	}
	m_fPlayRate = pFace->fPlayRation;
	m_fPlaySpeed = pFace->fPlaySpeed;
	if (pFace->dwMotionType != FACEMOTION_SPECIFIED)
	{
		m_comboMotionType.SetCurSel(static_cast<int>(pFace->dwMotionType));
		OnCbnSelchangeComboFacemotiontype();
	}
	else
	{
		TCHAR strDriver[MAX_PATH];
		TCHAR strPath[MAX_PATH];
		TCHAR strFile[MAX_PATH];
		LPCSTR strSuffix = NULL;
		LPCSTR strModify = NULL;

		KG_PROCESS_ERROR(pFaceMotionSettings);
		
		_splitpath_s(m_strCurrentFacePath,
			strDriver,
			MAX_PATH,
			strPath,
			MAX_PATH,
			strFile,
			MAX_PATH,
			NULL,
			0);
		pFaceMotionSettings->GetFaceMotionPathFromFacePath(strPath);
		strModify = pFaceMotionSettings->GetFaceMotionModifyPath();
		m_strFaceMotionDir.Format("%s%s%s%s%s.ani", strDriver, strPath, strModify, strFile, pFace->strPath);
	}
	UpdateData(FALSE);
Exit0:
	;
}

void KDlgMotionTag::FillDamage(void *pData)
{
	MotionTagBlockDamagePercentage *pDamage = static_cast<MotionTagBlockDamagePercentage*>(pData);
	KG_PROCESS_ERROR(pDamage);

	m_fDamage = pDamage->fDamagePercentage;
	UpdateData(FALSE);
Exit0:
	;
}

void KDlgMotionTag::FillMissile(void *pData)
{
	MotionTagBlockMissile *pMissile = static_cast<MotionTagBlockMissile*>(pData);
	KG_PROCESS_ERROR(pMissile);

	m_fMissileDamage = pMissile->fDamagePercentage;
	m_dwMissileID    = pMissile->dwMissileID;
	UpdateData(FALSE);
Exit0:
	;
}

void KDlgMotionTag::FillEquipSfx(void *pData)
{
	MotionTagBlockEquipSFX *pEquipSfx = static_cast<MotionTagBlockEquipSFX*>(pData);
	KG_PROCESS_ERROR(pEquipSfx);

	m_comboEquipSfxType.SetCurSel(static_cast<int>(pEquipSfx->dwEquipType));
	m_dwEquipSfxID = pEquipSfx->dwSfxID;
	UpdateData(FALSE);
Exit0:
	;
}
void KDlgMotionTag::FillGroundEffect(void *pData)
{
	MotionTagBlockGroundEffect *pGroundEffect = static_cast<MotionTagBlockGroundEffect*>(pData);
	DWORD dwSFXType = 0;
	KG_PROCESS_ERROR(pGroundEffect);
	dwSFXType = pGroundEffect->dwSFXIndex;
	static TCHAR* s_strSFXTypes[4] = {
		"小体型",
		"中体型",
		"大体型",
		"马",
	};
	m_ComSFXType.ResetContent();
	for (DWORD i = 0 ; i < sizeof(s_strSFXTypes) / sizeof(TCHAR*) ; i ++)
	{
		m_ComSFXType.AddString(s_strSFXTypes[i]);
	}
	m_ComSFXType.SetCurSel(dwSFXType);
	UpdateData(FALSE);
Exit0:
	;
}
void KDlgMotionTag::UpdateGroundEffect()
{
	MotionTagDataRuntime *pData = NULL;
	MotionTagBlockGroundEffect *pGoundEffect = NULL;
	UpdateData();
	m_pMotionTag->GetKeyFrame(m_uFrame,pData);
	KG_PROCESS_ERROR(pData);
	KG_PROCESS_ERROR(m_nLastSel < static_cast<int>(pData->Blocks.size()));

	pGoundEffect = static_cast<MotionTagBlockGroundEffect*>(pData->Blocks[m_nLastSel]);
	KG_PROCESS_ERROR(pGoundEffect);
	pGoundEffect->dwSFXIndex = static_cast<DWORD>(m_ComSFXType.GetCurSel());
Exit0:
	;
}
void KDlgMotionTag::UpdateEquipSfx()
{
	MotionTagDataRuntime *pData = NULL;
	MotionTagBlockEquipSFX *pEquipSfx = NULL;

	UpdateData();
	m_pMotionTag->GetKeyFrame(m_uFrame, pData);
	KG_PROCESS_ERROR(pData);
	KG_PROCESS_ERROR(m_nLastSel < static_cast<int>(pData->Blocks.size()));

	pEquipSfx = static_cast<MotionTagBlockEquipSFX*>(pData->Blocks[m_nLastSel]);
	KG_PROCESS_ERROR(pEquipSfx);

	pEquipSfx->dwEquipType = static_cast<DWORD>(m_comboEquipSfxType.GetCurSel());
	pEquipSfx->dwSfxID = m_dwEquipSfxID;
Exit0:
	;
}


void KDlgMotionTag::OnCbnSelchangeComboSubtag()
{
	DWORD dwType = 0;
	MotionTagDataRuntime *pTagData = NULL;
	int nCurSel = m_comboSubTag.GetCurSel();
	KG_PROCESS_ERROR(nCurSel != CB_ERR);
	
	if (m_nLastSel != -1)
	{
		dwType = m_pMotionTag->GetBlockType(m_uFrame, m_nLastSel);
		KG_PROCESS_ERROR(dwType < MTBT_COUNT);

		(this->*s_UpdateTagData[dwType])();
	}

	dwType = m_pMotionTag->GetBlockType(m_uFrame, nCurSel);
	KG_PROCESS_ERROR(dwType < MTBT_COUNT);

	DWORD dwNumGroup = sizeof(s_pControlGroup) / sizeof(DWORD*);
	for (DWORD i = 0; i < dwNumGroup; i++)
	{
		int nShowWindow = SW_HIDE;
		if (dwType == i)
		{
			nShowWindow = SW_SHOW;
		}
		for (DWORD j = 0; j < s_GroupControlCount[i]; j++)
		{
			GetDlgItem(s_pControlGroup[i][j])->ShowWindow(nShowWindow);
		}
	}
	
	m_pMotionTag->GetKeyFrame(m_uFrame, pTagData);
	KG_PROCESS_ERROR(pTagData);

	m_nLastSel = nCurSel;
	(this->*s_FillUserInterface[dwType])(pTagData->Blocks[nCurSel]);
Exit0:
	;
}

void KDlgMotionTag::SaveLastSel()
{
	MotionTagDataRuntime *pTagData = NULL;
	KG_PROCESS_ERROR(m_nLastSel != CB_ERR);

	DWORD dwType = m_pMotionTag->GetBlockType(m_uFrame, m_nLastSel);
	KG_PROCESS_ERROR(dwType < MTBT_COUNT);
	
	(this->*s_UpdateTagData[dwType])();
	
Exit0:
	;
}

void KDlgMotionTag::UpdateDamage()
{
	MotionTagDataRuntime *pData = NULL;
	MotionTagBlockDamagePercentage *pDamage = NULL;
	
	UpdateData();
	m_pMotionTag->GetKeyFrame(m_uFrame, pData);
	KG_PROCESS_ERROR(pData);
	KG_PROCESS_ERROR(m_nLastSel < static_cast<int>(pData->Blocks.size()));

	pDamage = static_cast<MotionTagBlockDamagePercentage*>(pData->Blocks[m_nLastSel]);
	KG_PROCESS_ERROR(pDamage);

	pDamage->fDamagePercentage = m_fDamage;
Exit0:
	;
}

void KDlgMotionTag::UpdateMissile()
{
	MotionTagDataRuntime *pData = NULL;
	MotionTagBlockMissile *pMissile = NULL;

	UpdateData();
	m_pMotionTag->GetKeyFrame(m_uFrame, pData);
	KG_PROCESS_ERROR(pData);
	KG_PROCESS_ERROR(m_nLastSel < static_cast<int>(pData->Blocks.size()));

	pMissile = static_cast<MotionTagBlockMissile*>(pData->Blocks[m_nLastSel]);
	KG_PROCESS_ERROR(pMissile);

	pMissile->fDamagePercentage = m_fMissileDamage;
	pMissile->dwMissileID = m_dwMissileID;
Exit0:
	;
}


void KDlgMotionTag::UpdateMetero()
{
	MotionTagDataRuntime *pData = NULL;
	MotionTagBlockMeteor *pMeteor = NULL;
	
	UpdateData();
	m_pMotionTag->GetKeyFrame(m_uFrame, pData);
	KG_PROCESS_ERROR(pData);
	KG_PROCESS_ERROR(m_nLastSel < static_cast<int>(pData->Blocks.size()));

	pMeteor = static_cast<MotionTagBlockMeteor*>(pData->Blocks[m_nLastSel]);
	KG_PROCESS_ERROR(pMeteor);

Exit0:
	;
}

void KDlgMotionTag::UpdateFaceMotion()
{
	MotionTagDataRuntime *pData = NULL;
	MotionTagBlockFaceMotion *pFaceMotion = NULL;

	UpdateData();
	m_pMotionTag->GetKeyFrame(m_uFrame, pData);
	KG_PROCESS_ERROR(pData);
	KG_PROCESS_ERROR(m_nLastSel < static_cast<int>(pData->Blocks.size()));

	pFaceMotion = static_cast<MotionTagBlockFaceMotion*>(pData->Blocks[m_nLastSel]);
	KG_PROCESS_ERROR(pFaceMotion);

	pFaceMotion->fPlayRation = m_fPlayRate;
	pFaceMotion->fPlaySpeed = m_fPlaySpeed;
	pFaceMotion->dwPlayType = (m_buttonLoop.GetCheck() == BST_CHECKED) ? ENU_ANIMATIONPLAY_CIRCLE : ENU_ANIMATIONPLAY_ONCE;
	pFaceMotion->bKeepOnMotionFinish = (m_ctlKeepMotion.GetCheck() == BST_CHECKED);
Exit0:
	;
}

void KDlgMotionTag::OnCbnSelchangeComboFacemotiontype()
{
	MotionTagDataRuntime *pData = NULL;
	MotionTagBlockFaceMotion *pFaceMotion = NULL;
	LPCSTR strSuffix = NULL;
	TCHAR strDriver[MAX_PATH];
	TCHAR strPath[MAX_PATH];
	TCHAR strFile[MAX_PATH];

	IKG3DFaceMotionSettings *pFaceMotionSettings = g_GetEngineManager()->GetFaceMotionSettings();
	m_pMotionTag->GetKeyFrame(m_uFrame, pData);
	KG_PROCESS_ERROR(pData);
	KG_PROCESS_ERROR(m_nLastSel < static_cast<int>(pData->Blocks.size()));

	pFaceMotion = static_cast<MotionTagBlockFaceMotion*>(pData->Blocks[m_nLastSel]);
	KG_PROCESS_ERROR(pFaceMotion);

	pFaceMotion->dwMotionType = static_cast<DWORD>(m_comboMotionType.GetCurSel());
	
	KG_PROCESS_ERROR(pFaceMotionSettings);
	strSuffix = pFaceMotionSettings->GetFaceMotionSuffixName(pFaceMotion->dwMotionType);

	_splitpath_s(m_strCurrentFacePath,
		strDriver,
		MAX_PATH,
		strPath,
		MAX_PATH,
		strFile,
		MAX_PATH,
		NULL,
		0);
	pFaceMotionSettings->GetFaceMotionPathFromFacePath(strPath);
	LPCSTR strModifyPath = pFaceMotionSettings->GetFaceMotionModifyPath();
	m_strFaceMotionDir.Format("%s%s%s%s%s.ani", strDriver, strPath, strModifyPath, strFile, strSuffix);
	UpdateData(FALSE);
Exit0:
	;
}

static const TCHAR * s_strFaceMotionAniTypeKeyworld = "_face";

void KDlgMotionTag::OnBnClickedButtonSelani()
{
	MotionTagDataRuntime *pData = NULL;
	MotionTagBlockFaceMotion *pFaceMotion = NULL;
	TCHAR strLowerFaceMotionPath[MAX_PATH];
	TCHAR strFaceMotionSuffix[MAX_PATH];
	TCHAR strFilter[] = "Animation Files (*.ani)|*.ani|";
	TCHAR strDrive[MAX_PATH];
	TCHAR strPath[MAX_PATH];
	TCHAR strInitPath[MAX_PATH];
	IKG3DFaceMotionSettings *pSettings = g_GetEngineManager()->GetFaceMotionSettings();
	CFileDialog dlgFile(TRUE, ".ani", "test.ani", OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT, strFilter);
	
	KG_PROCESS_ERROR(pSettings);
	_splitpath_s(m_strCurrentFacePath,
		strDrive,
		MAX_PATH,
		strPath,
		MAX_PATH,
		NULL,
		0,
		NULL,
		0);
	pSettings->GetFaceMotionPathFromFacePath(strPath);

	if (strDrive[0] != '\0')
	{
		sprintf_s(strInitPath,
			"%s%s%s",
			strDrive,
			strPath,
			pSettings->GetFaceMotionModifyPath());
	}
	else
	{
		sprintf_s(strInitPath,
			"%s%s%s",
			g_szDefExeDirectory,
			strPath,
			pSettings->GetFaceMotionModifyPath());
	}
	
	dlgFile.m_ofn.lpstrInitialDir = strInitPath;
	
	KG_PROCESS_ERROR(dlgFile.DoModal() == IDOK);

	m_pMotionTag->GetKeyFrame(m_uFrame, pData);
	KG_PROCESS_ERROR(pData);
	
	int nCurSel = m_comboSubTag.GetCurSel();
	KG_PROCESS_ERROR(nCurSel != CB_ERR);
	KG_PROCESS_ERROR(nCurSel < static_cast<int>(pData->Blocks.size()));

	pFaceMotion = static_cast<MotionTagBlockFaceMotion*>(pData->Blocks[nCurSel]);
	KG_PROCESS_ERROR(pFaceMotion);
	
	strcpy_s(strLowerFaceMotionPath, MAX_PATH, dlgFile.GetPathName());
	_strlwr_s(strLowerFaceMotionPath, MAX_PATH);
	const TCHAR *pFind = strstr(strLowerFaceMotionPath, s_strFaceMotionAniTypeKeyworld);
	if (!pFind)
	{
		MessageBox("不可通用的动画", "警告", MB_OK);
	}
	else
	{
		strcpy_s(strFaceMotionSuffix, MAX_PATH, pFind + strlen(s_strFaceMotionAniTypeKeyworld));
		TCHAR *pFind1 = strstr(strFaceMotionSuffix, ".ani");
		if (pFind1)
		{
			*pFind1 = '\0';
		}
		strcpy_s(pFaceMotion->strPath, MAX_PATH, strFaceMotionSuffix);
		pFaceMotion->dwMotionType = static_cast<DWORD>(FACEMOTION_SPECIFIED);
		m_strFaceMotionDir = strLowerFaceMotionPath;
		m_comboMotionType.SetCurSel(-1);//设置为空
	}
	
	UpdateData(FALSE);
Exit0:
	;
}
void KDlgMotionTag::OnBnClickedRadioStart()
{
	KG_PROCESS_ERROR(m_pMotionTag);

	m_pMotionTag->SetMotionStart(m_uFrame);
Exit0:
	;
}

void KDlgMotionTag::OnBnClickedRadioEnd()
{
	KG_PROCESS_ERROR(m_pMotionTag);

	m_pMotionTag->SetMotionEnd(m_uFrame);
Exit0:
	;
}

void KDlgMotionTag::OnBnClickedRadioNone()
{
	KG_PROCESS_ERROR(m_pMotionTag);

	if (m_pMotionTag->GetMotionEnd() == m_uFrame)
	{
		m_pMotionTag->SetMotionEnd(IEKG3DAnimationMotionTag::INVALIDATE_FRAME);
	}
	else if (m_pMotionTag->GetMotionStart() == m_uFrame)
	{
		m_pMotionTag->SetMotionStart(IEKG3DAnimationMotionTag::INVALIDATE_FRAME);
	}
Exit0:
	;
}

void KDlgMotionTag::OnBnClickedRadioOnce()
{
	if (m_buttonOnce.GetCheck() == BST_CHECKED)
	{
		m_ctlKeepMotion.EnableWindow(TRUE);
	}
}

void KDlgMotionTag::OnBnClickedRadioLoop()
{
	if (m_buttonLoop.GetCheck() == BST_CHECKED)
	{
		m_ctlKeepMotion.EnableWindow(FALSE);
	}
}

void KDlgMotionTag::OnBnClickedButtonCopy()
{
	// TODO: Add your control notification handler code here
	m_uFrameCopy = m_uFrame;
}

void KDlgMotionTag::OnBnClickedButtonPaste()
{
	// TODO: Add your control notification handler code here
	_ASSERTE(m_pMotionTag);
	MotionTagDataRuntime *pData = NULL;
	MotionTagDataRuntime *pDataSource = NULL;
	UINT uBlockCount = 0; 
	UINT uBlockCountSource = 0;
	KG_PROCESS_ERROR(m_uFrameCopy != m_uFrame);
	KG_PROCESS_ERROR(m_uFrameCopy != 0xfffffff);
	m_pMotionTag->GetKeyFrame(m_uFrameCopy,pDataSource);
	if(!pDataSource)
	{
		MessageBox("不能念帖空文件","ERROR");
		KG_PROCESS_ERROR(pDataSource);
	}
	m_pMotionTag->GetKeyFrame(m_uFrame,pData);
	uBlockCount = pData->Blocks.size();
	while (uBlockCount --)
	{
		UINT uID = GETID(m_uFrame, 0);
		m_pMotionTag->RemoveTag(uID);
	}
	strcpy_s(pData->strTagName,pDataSource->strTagName);
	//_sprintf_s_l(pData->strTagName,TAG_NAME_MAX_LENGTH,"%s",pDataSource->strTagName);
	uBlockCountSource = (pDataSource->Blocks).size();
	for (UINT i = 0 ; i < uBlockCountSource ; i ++)
	{
		UINT uID = GETID(m_uFrameCopy,i);
		m_pMotionTag->CopyTag(uID,m_uFrame);
	}
	FillSubList();
	if (m_comboSubTag.GetCount())
	{
		//m_comboSubTag.SetCurSel(0);
		SetcomboSubTagCursel(0);
		Update();
	}
Exit0:

	;
}
extern BYTE* g_SceneEditorKeyState;
BOOL KDlgMotionTag::PreTranslateMessage(MSG* pMsg)
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
			OnBnClickedButtonDel();
			g_SceneEditorKeyState[VK_DELETE] = 0;
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void KDlgMotionTag::OnEnChangeEditTagname()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData();
	MotionTagDataRuntime *pData = NULL;
	m_pMotionTag->GetKeyFrame(m_uFrame,pData);
	KG_PROCESS_ERROR(pData);
	strcpy_s(pData->strTagName,TAG_NAME_MAX_LENGTH,m_strTagName);
Exit0:
	;
}
