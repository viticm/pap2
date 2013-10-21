// KDlgAnimationComposer.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KDlgAnimationComposer.h"
#include "KG3DSkeletonTreeFiller.h"
#include "..\..\KG3DEngine\KG3DEngine\KG3DAnimationStruct.h"
#include "KG3DAnimationContainer.h"
#include "IEEditor.h"
#include "IEKG3DAnimationWarper.h"
#include "IEKG3DClip.h"

#define CheckModelState \
	{KG_PROCESS_ERROR(m_pModel);}
// KDlgAnimationComposer dialog

extern DWORD CheckModelTypeByKeyWord(LPCSTR strFileName);

IMPLEMENT_DYNAMIC(KDlgAnimationComposer, CDialog)

KDlgAnimationComposer::KDlgAnimationComposer(CWnd* pParent /*=NULL*/)
	: CDialog(KDlgAnimationComposer::IDD, pParent)
	, m_fComposerWeight(1.0f)
{
	m_pModel = NULL;
}

KDlgAnimationComposer::~KDlgAnimationComposer()
{
}

void KDlgAnimationComposer::SetModel(IEKG3DModel *pModel)
{
	m_pModel = pModel;
	g_GetSkeletonTreeFiller().FillSKTree(m_pModel, m_ctlSkeletonTree);
}

void KDlgAnimationComposer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_SKELETON, m_ctlSkeletonTree);
	DDX_Control(pDX, IDC_COMBO_COMPOSER, m_ctlComposerList);
	DDX_Control(pDX, IDC_COMBO_ANIMATIONUSE, m_ctlAnimationInUse);
	DDX_Control(pDX, IDC_COMBO_ROTATION, m_ctlTypeRot);
	DDX_Control(pDX, IDC_COMBO_SCALE, m_ctlTypeScale);
	DDX_Control(pDX, IDC_COMBO_TRANSLATE, m_ctlTypeTranslate);
	DDX_Control(pDX, IDC_COMBO_ANIMATION, m_ctlAnimation);
	DDX_Text(pDX, IDC_EDIT_COMPOSERWEIGHT, m_fComposerWeight);
}


BEGIN_MESSAGE_MAP(KDlgAnimationComposer, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ADDTOCOMPOSER, &KDlgAnimationComposer::OnBnClickedButtonAddtocomposer)
	ON_BN_CLICKED(IDC_BUTTON_WARPERSTART, &KDlgAnimationComposer::OnBnClickedButtonWarperstart)
	ON_BN_CLICKED(IDC_BUTTON_WARPERDISABLE, &KDlgAnimationComposer::OnBnClickedButtonWarperdisable)
	ON_NOTIFY(TVN_SELCHANGING, IDC_TREE_SKELETON, &KDlgAnimationComposer::OnTvnSelchangingTreeSkeleton)
	ON_BN_CLICKED(IDC_BUTTON_ADDCOMPOSER, &KDlgAnimationComposer::OnBnClickedButtonAddcomposer)
	ON_BN_CLICKED(IDC_BUTTON_DELCOMPOSER, &KDlgAnimationComposer::OnBnClickedButtonDelcomposer)
	ON_CBN_SELCHANGE(IDC_COMBO_ROTATION, &KDlgAnimationComposer::UpdateFromUI)
	ON_CBN_SELCHANGE(IDC_COMBO_SCALE, &KDlgAnimationComposer::UpdateFromUI)
	ON_CBN_SELCHANGE(IDC_COMBO_TRANSLATE, &KDlgAnimationComposer::UpdateFromUI)
	ON_CBN_SELCHANGE(IDC_COMBO_ANIMATIONUSE, &KDlgAnimationComposer::OnSelChangeAnimationInUse)
	ON_CBN_SELCHANGE(IDC_COMBO_COMPOSER, &KDlgAnimationComposer::OnSelChangeComposer)
	ON_BN_CLICKED(IDC_BUTTON_CHANGEANIMATION, &KDlgAnimationComposer::OnBnClickedButtonChangeanimation)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &KDlgAnimationComposer::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_LOAD, &KDlgAnimationComposer::OnBnClickedButtonLoad)
END_MESSAGE_MAP()


// KDlgAnimationComposer message handlers

void KDlgAnimationComposer::OnBnClickedButtonAddtocomposer()
{
    HRESULT hrRetCode = E_FAIL;
    IEKG3DAnimationWarper *piAnimationWarper = NULL;
	int nCurComposer = m_ctlComposerList.GetCurSel();
	int nCurAnimation = m_ctlAnimation.GetCurSel();

	KG_PROCESS_ERROR(nCurComposer != CB_ERR);
	KG_PROCESS_ERROR(nCurAnimation != CB_ERR);

	IEKG3DClip *pCurClip = reinterpret_cast<IEKG3DClip*>(m_ctlAnimation.GetItemData(nCurAnimation));

	CheckModelState;

    hrRetCode = m_pModel->GetIEKG3DAnimationWarper(&piAnimationWarper);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);
	hrRetCode = piAnimationWarper->AddAnimation(pCurClip, nCurComposer);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

	OnUpdateComposerUI(TRUE);
Exit0:
	;
}

void KDlgAnimationComposer::OnBnClickedButtonWarperstart()
{
    HRESULT hrRetCode = E_FAIL;
    IEKG3DAnimationWarper *piAnimationWarper = NULL;
	CheckModelState;

    hrRetCode = m_pModel->GetIEKG3DAnimationWarper(&piAnimationWarper);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);
    piAnimationWarper->Enable(TRUE);
Exit0:
	return;
}

void KDlgAnimationComposer::OnBnClickedButtonWarperdisable()
{
    HRESULT hrRetCode = E_FAIL;
    IEKG3DAnimationWarper *piAnimationWarper = NULL;
	CheckModelState;

    m_pModel->GetIEKG3DAnimationWarper(&piAnimationWarper);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);
	piAnimationWarper->Enable(FALSE);
Exit0:
	return;
}

void KDlgAnimationComposer::OnTvnSelchangingTreeSkeleton(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nIndex = 0;
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	nIndex = static_cast<int>(m_ctlSkeletonTree.GetItemData(pNMTreeView->itemNew.hItem));
	OnSelectBone(nIndex);
	*pResult = 0;
}

void KDlgAnimationComposer::OnSelectBone(int nIndex)
{
    HRESULT hrRetCode = E_FAIL;
    IEKG3DAnimationWarper *piAnimationWarper = NULL;
    //IEKG3DMeshBase *piMesh = NULL;
    IEKG3DAnimationComposer *pComposer = NULL;
    INT nBoneCount = 0;

    CheckModelState;

    hrRetCode = m_pModel->GetNumBones(&nBoneCount);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

	KG_PROCESS_ERROR((nIndex < nBoneCount) && nIndex >= 0);

	int nCurComposer = m_ctlComposerList.GetCurSel();
	KG_PROCESS_ERROR(nCurComposer != CB_ERR);
	
	int nCurAnimation = m_ctlAnimationInUse.GetCurSel();
	KG_PROCESS_ERROR(nCurAnimation != CB_ERR);
	DWORD dwType = 0;
	float fWeight = .0f;

    hrRetCode = m_pModel->GetIEKG3DAnimationWarper(&piAnimationWarper);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrRetCode = piAnimationWarper->GetComposer(nCurComposer, &pComposer);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

	pComposer->GetBoneInfo(nCurComposer, nIndex, dwType, fWeight);
	switch(dwType & Bone_Rotation_Mask)
	{
	case Bone_Rotation:
		m_ctlTypeRot.SetCurSel(0);
		break;
	case Bone_Rotation_None:
		m_ctlTypeRot.SetCurSel(1);
		break;
	}

	switch(dwType & Bone_Scale_Mask)
	{
	case Bone_Scale:
		m_ctlTypeScale.SetCurSel(0);
		break;
	case Bone_Scale_None:
		m_ctlTypeScale.SetCurSel(1);
		break;
	}

	switch(dwType & Bone_Translation_Mask)
	{
	case Bone_Translation:
		m_ctlTypeTranslate.SetCurSel(0);
		break;
	case Bone_Translation_None:
		m_ctlTypeTranslate.SetCurSel(1);
		break;
	}

	hrRetCode = piAnimationWarper->GetWeight(nCurComposer, nIndex, &m_fComposerWeight);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

	UpdateData(FALSE);

Exit0:
	return;
}

void KDlgAnimationComposer::OnBnClickedButtonAddcomposer()
{
    HRESULT hrRetCode = E_FAIL;
    IEKG3DAnimationWarper *piAnimationWarper = NULL;
	TCHAR strComposerName[MAX_PATH];
    DWORD dwComposerCount = 0;
	CheckModelState;

    hrRetCode = m_pModel->GetIEKG3DAnimationWarper(&piAnimationWarper);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);
    hrRetCode = piAnimationWarper->AddComposer();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);
    hrRetCode = piAnimationWarper->GetComposerCount(&dwComposerCount);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

	sprintf_s(strComposerName,
		MAX_PATH,
		"Composer %d",
        dwComposerCount);

	m_ctlComposerList.AddString(strComposerName);
	
Exit0:
	return;
}

void KDlgAnimationComposer::OnBnClickedButtonDelcomposer()
{
	MessageBox("Not Implement");
}



void KDlgAnimationComposer::UpdateFromUI()
{
    HRESULT hrRetCode = E_FAIL;
    IEKG3DAnimationWarper *piAnimationWarper = NULL;
	int nCurComposer = 0;
	int nCurBone = 0;
	int nCurAnimation = 0;
	int nNumAnimationInUse = 0;
    IEKG3DAnimationComposer *pComposer = NULL;
	CheckModelState;

	nCurComposer = m_ctlComposerList.GetCurSel();
	KG_PROCESS_ERROR(nCurComposer != CB_ERR);

    hrRetCode = m_pModel->GetIEKG3DAnimationWarper(&piAnimationWarper);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);
    hrRetCode = piAnimationWarper->GetComposer(nCurComposer, &pComposer);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

	KG_PROCESS_ERROR(pComposer);

	HTREEITEM hItem = m_ctlSkeletonTree.GetSelectedItem();
	KG_PROCESS_ERROR(hItem);

	nCurBone = static_cast<int>(m_ctlSkeletonTree.GetItemData(hItem));

	/*BOOL bInherit = static_cast<CButton*>(GetDlgItem(IDC_CHECK_INHERTSOMPOSERWEIGHT))->GetCheck() == BST_CHECKED ? TRUE : FALSE;

	m_pModel->GetAnimationWarper()->SetWeight(nCurComposer,
		nCurBone,
		m_fComposerWeight,
		bInherit);*/
	
	nCurAnimation = m_ctlAnimationInUse.GetCurSel();
	KG_PROCESS_ERROR(nCurAnimation != CB_ERR);

	DWORD dwType = 0;
	int nTypeCurSel = m_ctlTypeScale.GetCurSel();
	nTypeCurSel == 0 ? dwType |= Bone_Scale : dwType |= Bone_Scale_None;

	nTypeCurSel = m_ctlTypeRot.GetCurSel();
	nTypeCurSel == 0 ? dwType |= Bone_Rotation : dwType |= Bone_Rotation_None;

	nTypeCurSel = m_ctlTypeTranslate.GetCurSel();
	nTypeCurSel == 0 ? dwType |= Bone_Translation : dwType |= Bone_Translation_None;

	hrRetCode = pComposer->SetBoneInfo(nCurAnimation, nCurBone, dwType, 1.0f);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);
Exit0:
	return;
}


void KDlgAnimationComposer::SetAnimationContainer(KG3DAnimationContainer* pContainer)
{
	m_pAnimationContainer = pContainer;
	m_pAnimationContainer->FillAnimationComboBox(m_ctlAnimation);
}

void KDlgAnimationComposer::OnSelChangeComposer()
{
	OnUpdateComposerUI(TRUE);
}

void KDlgAnimationComposer::OnUpdateComposerUI(BOOL bSelChangeComposer)
{
    HRESULT hrRetCode = E_FAIL;
    IEKG3DAnimationWarper *piAnimationWarper = NULL;
    IEKG3DAnimationComposer *pComposer = NULL;
	int nCurComposer = 0;
	int nCurBone = 0;
	int nCurAnimation = 0;
	DWORD dwNumAnimationInUse = 0;
	CheckModelState;

	nCurComposer = m_ctlComposerList.GetCurSel();
	KG_PROCESS_ERROR(nCurComposer != CB_ERR);

    hrRetCode = m_pModel->GetIEKG3DAnimationWarper(&piAnimationWarper);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);
    hrRetCode = piAnimationWarper->GetComposer(nCurComposer, &pComposer);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

	KG_PROCESS_ERROR(pComposer);

	//Fill animation in use comboBox
	if (bSelChangeComposer)
	{
		hrRetCode = pComposer->GetAnimationCount(&dwNumAnimationInUse);
        KGLOG_COM_PROCESS_ERROR(hrRetCode);

		m_ctlAnimationInUse.ResetContent();
		for (DWORD i = 0; i < dwNumAnimationInUse; i++)
		{
			TCHAR strOutputInfo[MAX_PATH];
			sprintf_s(strOutputInfo, 
				MAX_PATH,
				"Animation %u",
				i);
			m_ctlAnimationInUse.AddString(strOutputInfo);
		}
		m_ctlAnimationInUse.SetCurSel(0);
	}

	HTREEITEM hItem = m_ctlSkeletonTree.GetSelectedItem();
	KG_PROCESS_ERROR(hItem);

	nCurBone = static_cast<int>(m_ctlSkeletonTree.GetItemData(hItem));

	hrRetCode = piAnimationWarper->GetWeight(nCurComposer, nCurBone, &m_fComposerWeight);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);


	nCurAnimation = m_ctlAnimationInUse.GetCurSel();
	KG_PROCESS_ERROR(nCurAnimation != CB_ERR);

	UpdateInfo(pComposer,
		nCurAnimation,
		nCurBone);

Exit0:
	;
}

void KDlgAnimationComposer::UpdateInfo(IEKG3DAnimationComposer *pComposer,
									   int nCurAnimation,
									   int nCurBone)
{
	DWORD dwType = 0;
	float fWeight = 0.0f;
	pComposer->GetBoneInfo(nCurAnimation, nCurBone, dwType, fWeight);
	UpdateData(FALSE);
	int nCurSel = 0;
	(dwType & Bone_Rotation_Mask) & Bone_Rotation_None ? \
		nCurSel = 1 : nCurSel = 0;
	m_ctlTypeRot.SetCurSel(nCurSel);

	(dwType & Bone_Scale_Mask) & Bone_Scale_None ? \
		nCurSel = 1 : nCurSel = 0;
	m_ctlTypeScale.SetCurSel(nCurSel);

	(dwType & Bone_Translation_Mask) & Bone_Translation_None ? \
		nCurSel = 1 : nCurSel = 0;
	m_ctlTypeTranslate.SetCurSel(nCurSel);
}

void KDlgAnimationComposer::OnSelChangeAnimationInUse()
{
	OnUpdateComposerUI(FALSE);
}

void KDlgAnimationComposer::OnOK()
{
    HRESULT hrRetCode = E_FAIL;
    IEKG3DAnimationWarper *piAnimationWarper = NULL;
	UpdateData();
	
	CheckModelState;
	BOOL bInherit = static_cast<CButton*>(GetDlgItem(IDC_CHECK_INHERTSOMPOSERWEIGHT))->GetCheck() == BST_CHECKED ? TRUE : FALSE;
	
	int nCurComposer = m_ctlComposerList.GetCurSel();
	KG_PROCESS_ERROR(nCurComposer != CB_ERR);

	HTREEITEM hItem = m_ctlSkeletonTree.GetSelectedItem();
	KG_PROCESS_ERROR(hItem);

	int nCurBone = static_cast<int>(m_ctlSkeletonTree.GetItemData(hItem));
	KG_PROCESS_ERROR(nCurBone != -1);

    hrRetCode = m_pModel->GetIEKG3DAnimationWarper(&piAnimationWarper);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);
    hrRetCode = piAnimationWarper->SetWeight(nCurComposer, nCurBone, m_fComposerWeight, bInherit);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

Exit0:
	;
}
void KDlgAnimationComposer::OnBnClickedButtonChangeanimation()
{
    HRESULT hrRetCode = E_FAIL;
    IEKG3DAnimationWarper *piAnimationWarper = NULL;
	CheckModelState;

	int nCurComposer = m_ctlComposerList.GetCurSel();
	KG_PROCESS_ERROR(nCurComposer != CB_ERR);

	int nCurAnimationInUse = m_ctlAnimationInUse.GetCurSel();
	KG_PROCESS_ERROR(nCurAnimationInUse != CB_ERR);

	int nCurAnimation = m_ctlAnimation.GetCurSel();
	KG_PROCESS_ERROR(nCurAnimation != CB_ERR);

	IEKG3DClip* pNewClip = reinterpret_cast<IEKG3DClip*>(m_ctlAnimation.GetItemData(nCurAnimation));
	KG_PROCESS_ERROR(pNewClip);

    hrRetCode = m_pModel->GetIEKG3DAnimationWarper(&piAnimationWarper);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);
    hrRetCode = piAnimationWarper->ChangeAnimation(nCurComposer,
		nCurAnimationInUse,
		pNewClip,
		NULL,
		0);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

Exit0:
	return;
}

void KDlgAnimationComposer::OnBnClickedButtonSave()
{
    HRESULT hrRetCode = E_FAIL;
    IEKG3DAnimationWarper *piAnimationWarper = NULL;
	CFileDialog dlgOpen(FALSE, 
		"awi", 
		"noname.awi",
		OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY,
		"Animation warper info(*.awi)|*.awi||",
		this);
	CheckModelState;
	KG_PROCESS_SUCCESS(dlgOpen.DoModal() != IDOK);

    hrRetCode = m_pModel->GetIEKG3DAnimationWarper(&piAnimationWarper);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);
    hrRetCode = piAnimationWarper->Save(dlgOpen.GetPathName());
	if (FAILED(hrRetCode))
	{
		MessageBox("Save animation warper info file failed.", "Error");
	}
Exit1:
Exit0:
	return;
}

void KDlgAnimationComposer::OnBnClickedButtonLoad()
{
    HRESULT hrRetCode = E_FAIL;
    IEKG3DAnimationWarper *piAnimationWarper = NULL;
    CFileDialog dlgSave(TRUE,
        "awi",
        "noname.awi",
        OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY,
        "Animation warper info(*.awi)|*.awi||",
        this);
    IEKG3DClip *piClip = NULL;
    LPCTSTR pszName = NULL;
    DWORD dwSplitType = 0;
    DWORD dwNumComposer = 0;

    KG_PROCESS_SUCCESS(dlgSave.DoModal() != IDOK);

    CheckModelState;
    hrRetCode = m_pModel->EnableWarper(TRUE);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrRetCode = m_pModel->GetIEKG3DAnimationWarper(&piAnimationWarper);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrRetCode = piAnimationWarper->Load(dlgSave.GetPathName());
    if (FAILED(hrRetCode))
    {
        MessageBox("Load animation warper info file failed.", "Error");
    }

    piClip = reinterpret_cast<IEKG3DClip*>(m_ctlAnimation.GetItemData(0));
    KG_PROCESS_ERROR(piClip);

    hrRetCode = piClip->GetPathName(&pszName);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    dwSplitType = CheckModelTypeByKeyWord(pszName);

    hrRetCode = m_pModel->PlaySplitAnimation(
        pszName,
        dwSplitType,
        ANI_JOINT_TOP,
        ENU_ANIMATIONPLAY_CIRCLE,
        1.0f,
        0,
        NULL,
        NULL,
        ANICTL_PRIMARY
    );
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrRetCode = m_pModel->PlaySplitAnimation(
        pszName,
        dwSplitType,
        ANI_JOINT_BOTTOM,
        ENU_ANIMATIONPLAY_CIRCLE,
        1.0f,
        0,
        NULL,
        NULL,
        ANICTL_PRIMARY
    );
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    piAnimationWarper->GetComposerCount(&dwNumComposer);
    m_ctlComposerList.ResetContent();
    for (DWORD i = 0; i < dwNumComposer; i++)
    {
        TCHAR strComposerName[MAX_PATH];
        sprintf_s(strComposerName,
            MAX_PATH,
            "Composer %d",
            i);
        m_ctlComposerList.AddString(strComposerName);
    }
    OnUpdateComposerUI(TRUE);

Exit1:
Exit0:
    return;
}
