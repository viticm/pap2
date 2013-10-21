// KDLG_SKWeight.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KDLG_SKWeight.h"
#include "IEKG3DAnimationWarper.h"
#include "Helper_Func.h"
#include "KG3DSkeletonTreeFiller.h"
// KDLG_SKWeight dialog
#include "Engine/KGLog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNAMIC(KDLG_SKWeight, CDialog)

KDLG_SKWeight::KDLG_SKWeight(IEKG3DModel* pModel,
							 KG3DAnimationContainer* pAnimationContainer,
							 CWnd* pParent /*=NULL*/)
	: CDialog(KDLG_SKWeight::IDD, pParent)
	, m_fWeight(0)
{
	m_pModel = pModel;
	m_pAnimationContainer = pAnimationContainer;
	m_nCurBoneSet = -1;
	Create(IDD, pParent);
	ShowWindow(SW_SHOW);
}

KDLG_SKWeight::~KDLG_SKWeight()
{

}

void KDLG_SKWeight::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_ctlSkeleton);
	DDX_Control(pDX, IDC_COMBO_ANI, m_ctlAnimationList);
	DDX_Control(pDX, IDC_LIST_ANIWEIGHT, m_ctlAniWeightList);
	DDX_Control(pDX, IDC_CHECK_INHERT, m_ctlInheritWeight);
	DDX_Control(pDX, IDC_EDIT1, m_ctlEditWeight);
	DDX_Text(pDX, IDC_EDIT1, m_fWeight);
}


BEGIN_MESSAGE_MAP(KDLG_SKWeight, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &KDLG_SKWeight::OnTvnSelchangedTree1)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST2, &KDLG_SKWeight::OnLvnItemchangedList2)
	ON_BN_CLICKED(IDC_BUTTON_ADDANIMATION, &KDLG_SKWeight::OnBnClickedButtonAddanimation)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_ANIWEIGHT, &KDLG_SKWeight::OnLvnItemchangedListAniweight)
	ON_NOTIFY(LVN_ITEMACTIVATE, IDC_LIST_ANIWEIGHT, &KDLG_SKWeight::OnLvnItemActivateListAniweight)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, &KDLG_SKWeight::OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE, &KDLG_SKWeight::OnBnClickedButtonRemove)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &KDLG_SKWeight::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &KDLG_SKWeight::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_LOAD, &KDLG_SKWeight::OnBnClickedButtonLoad)
END_MESSAGE_MAP()


// KDLG_SKWeight message handlers

BOOL KDLG_SKWeight::LoadStrings()
{
	BOOL bReturn = FALSE;
	int nReturn = 0;
	HINSTANCE hInstance = ::AfxGetInstanceHandle();
	KG_PROCESS_ERROR(hInstance);

	nReturn = ::LoadString(hInstance, IDS_SKWEIGHT_LOADANI, m_strLoadCaption, 256);
	KG_PROCESS_ERROR(nReturn);

	nReturn = ::LoadString(hInstance, IDS_STRING_ANINAME, m_strAniName, 256);
	KG_PROCESS_ERROR(nReturn);

	nReturn = ::LoadString(hInstance, IDS_STRING_ANIWEIGHT, m_strWeight, 256);
	KG_PROCESS_ERROR(nReturn);

	bReturn = TRUE;
Exit0:
	return bReturn;
}

BOOL KDLG_SKWeight::OnInitDialog()
{
	DWORD dwStyle = 0;
	LoadStrings();
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	//FillBoneTree();
	g_GetSkeletonTreeFiller().FillSKTree(m_pModel, m_ctlSkeleton);
	FillAnimationList();
	m_ctlAniWeightList.InsertColumn(0, m_strAniName, LVCFMT_LEFT, 130);
	m_ctlAniWeightList.InsertColumn(1, m_strWeight, LVCFMT_RIGHT, 50);
	dwStyle = m_ctlAniWeightList.GetExtendedStyle() | LVS_EX_FULLROWSELECT;
	m_ctlAniWeightList.SetExtendedStyle(dwStyle);
	FillAnimationWeightList();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void KDLG_SKWeight::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	CString strBoneName;
	//int nBoneIndex = -1;
	HTREEITEM hItem = 0;
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	hItem = pNMTreeView->itemOld.hItem;
	KG_PROCESS_ERROR(hItem);

	//nBoneIndex = GetCurSelBoneIndex();
	FillAnimationWeightList();

Exit0:
	*pResult = 0;
}

void KDLG_SKWeight::FillBoneTree()
{
    HRESULT hRetCode = E_FAIL;
	IEKG3DMesh* pMesh = NULL;

	KG_PROCESS_ERROR(m_pModel);

	hRetCode = m_pModel->GetIEKG3DMesh(&pMesh);
	KGLOG_COM_PROCESS_ERROR(hRetCode);
    
	Helper_FillBoneTree(pMesh, &m_ctlSkeleton);

Exit0:
	return;
}


void KDLG_SKWeight::OnLvnItemchangedList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void KDLG_SKWeight::FillAnimationList()
{
	KG_PROCESS_ERROR(m_pModel);
	KG_PROCESS_ERROR(m_pAnimationContainer);


	m_pAnimationContainer->FillAnimationComboBox(m_ctlAnimationList);
	//m_ctlAnimationList.AddString(m_strLoadCaption);

Exit0:
	return;
}

void KDLG_SKWeight::ClearAnimationList()
{
	//do remember to release animations
}

void KDLG_SKWeight::FillAnimationWeightList()
{
    HRESULT hRetCode = E_FAIL;
	int i = 0;
	int nBoneIndex = -1;
	float fWeight = 0.0f;
	int nNumAnimation = 0;
	HTREEITEM hItem = NULL;
	IEKG3DAnimationWarper* pWarper = NULL;

	m_ctlAniWeightList.DeleteAllItems();

	KG_PROCESS_ERROR(m_pModel);

	hItem = m_ctlSkeleton.GetSelectedItem();
	if (hItem)
	{
		nBoneIndex = static_cast<int>(m_ctlSkeleton.GetItemData(hItem));
	}

	hRetCode = m_pModel->GetIEKG3DAnimationWarper(&pWarper);
	KGLOG_COM_PROCESS_ERROR(hRetCode);

	/*nNumAnimation = pWarper->GetNumAnimations();
	for (i = 0; i < nNumAnimation; i++)
	{
		strAniWeight[0] = '\0';
		
		m_ctlAniWeightList.InsertItem(i, NULL);
		m_ctlAniWeightList.SetItemText(i, 0 , pWarper->GetAnimationName(i));
		if (nBoneIndex != -1)
		{
			fWeight = pWarper->GetBoneWeight(i, 0, nBoneIndex);
		}
		else 
		{
			fWeight = 0.0f;
		}
		
		sprintf(strAniWeight, "%1.1f", fWeight);
		m_ctlAniWeightList.SetItemText(i, 1, strAniWeight);
	}*/

Exit0:
	return;
}


void KDLG_SKWeight::OnBnClickedButtonAddanimation()
{
    //HRESULT hResult = E_FAIL;
    //HRESULT hRetCode = E_FAIL;
	//int nCurSel = 0;
	//IEKG3DAnimation* pAnimation = NULL;
	//IEKG3DAnimationWarper* pWarper = NULL;
	//KG_PROCESS_ERROR(m_pModel);

    //hRetCode = m_pModel->GetIEKG3DAnimationWarper(&pWarper);
   // KGLOG_COM_PROCESS_ERROR(hRetCode);

	//nCurSel = m_ctlAnimationList.GetCurSel();
	//KG_PROCESS_ERROR(nCurSel != LB_ERR);

	//pAnimation = (IEKG3DAnimation*)(m_ctlAnimationList.GetItemDataPtr(nCurSel));
	//pWarper->InsertAnimation(pAnimation, NULL);

	FillAnimationWeightList();
//Exit0:
	return;
}

void KDLG_SKWeight::OnCancel()
{
	ClearAnimationList();
	DestroyWindow();
}

void KDLG_SKWeight::PostNcDestroy()
{
	delete this;
}


int KDLG_SKWeight::GetCurSelBoneIndex()
{
	int nReturn = -1;
	HTREEITEM hItem = 0;
	hItem = m_ctlSkeleton.GetSelectedItem();
	if (hItem)
	{
		nReturn = (int)m_ctlSkeleton.GetItemData(hItem);
	}
	return nReturn;
}
void KDLG_SKWeight::OnLvnItemchangedListAniweight(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}


void KDLG_SKWeight::OnLvnItemActivateListAniweight(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	KG_PROCESS_ERROR(pNMIA->iSubItem == 1);

	m_nCurBoneSet = pNMIA->iItem;
	OnEditWeight(pNMIA->iItem, pNMIA->iSubItem);

Exit0:
	*pResult = 0;
}

void KDLG_SKWeight::OnEditWeight(int nItem, int nSubItem)
{
	TCHAR strWeight[256];

	int nReturn = 0;
	TCHAR* strStop = NULL;
	CRect rectEdit;
	CRect rectBox;
	nReturn = m_ctlAniWeightList.GetItemText(nItem, nSubItem, strWeight, 256);

	m_ctlAniWeightList.GetSubItemRect(nItem, nSubItem, LVIR_BOUNDS, rectEdit);
	m_ctlAniWeightList.GetWindowRect(&rectBox);
	ScreenToClient(&rectBox);
	rectEdit.bottom += rectBox.top;
	rectEdit.top    += rectBox.top;
	rectEdit.left   += rectBox.left;
	rectEdit.right  += rectBox.left;
	m_ctlEditWeight.MoveWindow(&rectEdit);
	m_ctlEditWeight.ShowWindow(SW_SHOW);
	m_ctlEditWeight.SetFocus();
	m_ctlEditWeight.SetWindowText(strWeight);
}

BOOL KDLG_SKWeight::PreTranslateMessage(MSG* pMsg)
{
    HRESULT hRetCode = E_FAIL;
	int nCurSelBone = -1;
	int nCheckState = 0;
	BOOL bInherit = FALSE;
	IEKG3DAnimationWarper* pWarper = NULL;
	HWND hWndEdit = m_ctlEditWeight.GetSafeHwnd();

	if (pMsg->hwnd == hWndEdit)
	{
		if (pMsg->message == WM_KEYDOWN &&
			pMsg->wParam == VK_RETURN)
		{
			UpdateData(TRUE);
			KG_PROCESS_ERROR(m_pModel);

            hRetCode = m_pModel->GetIEKG3DAnimationWarper(&pWarper);
            KGLOG_COM_PROCESS_ERROR(hRetCode);

			nCurSelBone = GetCurSelBoneIndex();
			nCheckState = m_ctlInheritWeight.GetCheck();
			bInherit = (nCheckState == BST_CHECKED) ? TRUE : FALSE;
			//pWarper->SetBoneWeight(m_nCurBoneSet, 0, nCurSelBone, m_fWeight, bInherit);
			m_ctlEditWeight.ShowWindow(SW_HIDE);
			FillAnimationWeightList();
		}
	}
Exit0:
	return CDialog::PreTranslateMessage(pMsg);
}




void KDLG_SKWeight::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	UpdateData();
	//CDialog::OnOK();
}

void KDLG_SKWeight::OnBnClickedButtonPlay()
{
	KG_PROCESS_ERROR(m_pModel);
    IKG3DAnimationController *piAniController = NULL;
	m_pModel->IEGetAnimationController(&piAniController);
    piAniController->StartAnimation(ENU_ANIMATIONPLAY_CIRCLE, 0, 1.0f);
	m_pModel->EnableWarper(TRUE);
Exit0:
	return;
}

void KDLG_SKWeight::OnBnClickedButtonRemove()
{
    HRESULT hRetCode = E_FAIL;
	IEKG3DAnimationWarper* pWarper = NULL;
	POSITION pos;
	int nItem = 0;
	KG_PROCESS_ERROR(m_pModel);

    hRetCode = m_pModel->GetIEKG3DAnimationWarper(&pWarper);
    KGLOG_COM_PROCESS_ERROR(hRetCode);


	pos = m_ctlAniWeightList.GetFirstSelectedItemPosition();
	KG_PROCESS_ERROR(pos);

	nItem =m_ctlAniWeightList.GetNextSelectedItem(pos);

	//pWarper->RemoveAnimation(nItem);
	FillAnimationWeightList();

Exit0:
	return;
}

void KDLG_SKWeight::OnBnClickedButtonStop()
{
	KG_PROCESS_ERROR(m_pModel);
	m_pModel->EnableWarper(FALSE);

Exit0:
	return;
}

void KDLG_SKWeight::OnBnClickedButtonSave()
{
	
}

void KDLG_SKWeight::OnBnClickedButtonLoad()
{

}
