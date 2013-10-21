#include "stdafx.h"
#include "sceneeditor.h"
//#include "mainfrm.h" //by dengzhihui 2006年7月6日 14:41:07
#include "KSceneModelEditorPropertyPageMesh.h"
#include "KSceneModelEditorMeshBindDialog.h"
#include "MeshEditorDoc.h"
#include "KSceneModelEditorPanelView.h"
#include "KG3DSkeletonTreeFiller.h"
#include "IEKG3DBip.h"
#include "IEKG3DModelTable.h"
#include "IEKG3DMeshBone.h"

//#include "SceneEditorCommon.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

const static DWORD s_dwMaxScale = 2;
IMPLEMENT_DYNCREATE(KSceneModelEditorPropertyPageMesh, CPropertyPage)

KSceneModelEditorPropertyPageMesh::KSceneModelEditorPropertyPageMesh() : CPropertyPage(KSceneModelEditorPropertyPageMesh::IDD)
, m_fWeight(0)
, m_fLod(0)
{
	//{{AFX_DATA_INIT(KSceneModelEditorPropertyPageMesh)
	m_scSocketName = _T("");
	m_nLOD = 0;
	//}}AFX_DATA_INIT
	m_lpCurModel = NULL;
	//m_lpModelEditor = NULL;
	m_lpDialogSocketSet = NULL;
	m_bExpandBones = FALSE;
	m_fWeight = 0.0f ;
	m_dwBoneIndex = 0;
	m_oldMesh = NULL;
	m_fScaleX = 1.0f;
	m_fScaleY = 1.0f;
	m_fScaleZ = 1.0f;
	m_lpCurModelParent = NULL;
}

KSceneModelEditorPropertyPageMesh::~KSceneModelEditorPropertyPageMesh()
{
	//SAFE_RELEASE(m_lpCurModel);
	SAFE_DELETE(m_lpDialogSocketSet);
}

void KSceneModelEditorPropertyPageMesh::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KSceneModelEditorPropertyPageMesh)
	DDX_Control(pDX, IDC_STATIC_LOD, m_Static_LOD);
	DDX_Control(pDX, IDC_STATIC_BONE, m_ButtonBone);
	DDX_Control(pDX, IDC_STATIC_SOCKET, m_ButtonSocket);
	DDX_Control(pDX, IDC_LIST_SOCKET, m_ListBoxSocket);
	DDX_Control(pDX, IDC_TREE_BONE, m_TreeBone);
	DDX_Text(pDX, IDC_EDIT1, m_scSocketName);
	DDX_Slider(pDX, IDC_SLIDER_LOD, m_nLOD);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_CHECK_PROGRESS, m_ctlCheckProgress);
	DDX_Text(pDX, IDC_BBOX_WEIGHT, m_fWeight);
	DDX_Control(pDX, IDC_SLIDER_X, m_SliderX);
	DDX_Control(pDX, IDC_SLIDER_Y, m_SliderY);
	DDX_Control(pDX, IDC_SLIDER_Z, m_SliderZ);
	DDX_Text(pDX, IDC_EDIT_BBOXX, m_fScaleX);
	DDX_Text(pDX, IDC_EDIT_BBOXY, m_fScaleY);
	DDX_Text(pDX, IDC_EDIT_BBOXZ, m_fScaleZ);
	DDX_Text(pDX, IDC_EDIT_LOD, m_fLod);
}

IEKG3DMesh* KSceneModelEditorPropertyPageMesh::GetReleasedMesh()
{
    HRESULT hRetCode = NULL;
	KGLOG_PROCESS_ERROR(m_lpCurModel);
	IEKG3DMesh* pMesh = NULL;
	//IEKG3DMeshBase* pMeshBase = NULL;

    hRetCode = m_lpCurModel->GetIEKG3DMesh(&pMesh);
    KG_COM_PROCESS_ERROR(hRetCode);
	return pMesh;
Exit0:
	return NULL;
}

void KSceneModelEditorPropertyPageMesh::SetCurModel(IEKG3DModel* pCurModel,IEKG3DModel* pParent)
{
    HRESULT hRetCode = E_FAIL;
	int nNumBones = 0;
	TCHAR strNumBones[MAX_PATH];
	IEKG3DMesh* pMesh = NULL;
	INT nMaxFace = 0;
	INT nMinFace = 0;
	m_dwBoneIndex = (DWORD)-1;
	m_lpCurModelParent = pParent;
	if(m_lpCurModel != pCurModel)
	{
		//SAFE_RELEASE(m_lpCurModel);
		m_lpCurModel = pCurModel;
		KG_PROCESS_ERROR(::IsWindow(GetSafeHwnd()));
		g_GetSkeletonTreeFiller().FillSKTree(m_lpCurModel, m_TreeBone);
		FillListBoxSocket();

		KG_PROCESS_ERROR(m_lpCurModel);
		//m_lpCurModel->AddRef();

		hRetCode = m_lpCurModel->GetIEKG3DMesh(&pMesh);
		KG_COM_PROCESS_ERROR(hRetCode);

		hRetCode = pMesh->GetNumBones(&nNumBones);
		KG_COM_PROCESS_ERROR(hRetCode);

		if (nNumBones)
		{
			sprintf(strNumBones, "Bone: %d", nNumBones);
		}
		else
		{
			sprintf(strNumBones, "No Bone exist.");
		}
		GetDlgItem(IDC_STATIC_NUMBONES)->SetWindowText(strNumBones);	
		static_cast<CButton*>(GetDlgItem(IDC_CHECK_PROGRESS))->SetCheck(BST_UNCHECKED);
		GetDlgItem(IDC_SLIDER_LOD)->EnableWindow(FALSE);
		OnBnClickedCheckPartbbox();
	}
Exit0:
	return;
}

void  KSceneModelEditorPropertyPageMesh::FillListBoxSocket()
{

	HRESULT hr = E_FAIL;
    IEKG3DMesh* pMesh = NULL;
	DWORD dwNumSockets = 0;
	TCHAR strOutputInfo[256];
    LPCTSTR pszSocketName = NULL;
	//Clear List Box first
	m_ListBoxSocket.SetCurSel(-1);
	int Count = m_ListBoxSocket.GetCount();
	for(int i=Count+1;i>=0;i--)
	{
		m_ListBoxSocket.DeleteString(i);
	}

	KG_PROCESS_ERROR(m_lpCurModel);

	pMesh = GetReleasedMesh();
	KG_PROCESS_ERROR(pMesh);

	hr = pMesh->GetSocketCount(&dwNumSockets);
	if(SUCCEEDED(hr))
	{
		for(DWORD i = 0; i < dwNumSockets; i++)
		{
			hr = pMesh->GetSocketName(i, &pszSocketName);
			KGLOG_COM_PROCESS_ERROR(hr);
            int nIndex = m_ListBoxSocket.AddString(pszSocketName);
			m_ListBoxSocket.SetItemData(nIndex, i);
		}
		wsprintf(strOutputInfo, "插槽: %d 个", dwNumSockets);
		m_ButtonSocket.SetWindowText(strOutputInfo);
	}
	else
	{
		m_ButtonSocket.SetWindowText("没有插槽");
	}

Exit0:
	return;
}


BEGIN_MESSAGE_MAP(KSceneModelEditorPropertyPageMesh, CPropertyPage)
	//{{AFX_MSG_MAP(KSceneModelEditorPropertyPageMesh)
	ON_LBN_SELCHANGE(IDC_LIST_SOCKET, OnSelchangeListSocket)
	ON_BN_CLICKED(IDC_BUTTON_SRENAME, OnButtonSrename)
	ON_NOTIFY(NM_CLICK, IDC_TREE_BONE, OnClickTreeBone)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_BONE, OnSelchangedTreeBone)
	ON_BN_CLICKED(IDC_BUTTON_SADD, OnButtonSadd)
	ON_BN_CLICKED(IDC_BUTTON_SDELETE, OnButtonSdelete)
	ON_BN_CLICKED(IDC_BUTTON_BIND, OnButtonBind)
	ON_BN_CLICKED(IDC_BUTTON_SUNBIND, OnButtonSunbind)
	ON_BN_CLICKED(IDC_CHECK_PROGRESS, OnCheckProgress)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON_SSET, OnBnClickedButtonSset)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
    ON_BN_CLICKED(IDC_BILLBOARD, OnBnClickBillBoard)
	ON_BN_CLICKED(IDC_BUTTON_BSSAVE, OnBnClickedButtonBssave)
	ON_BN_CLICKED(IDC_BUTTON_BSLOAD, OnBnClickedButtonBsload)
	ON_BN_CLICKED(IDC_BUTTON_NORMALSPE, OnBnClickedButtonNormalSpe)
	ON_BN_CLICKED(IDC_BUTTON_EXPAND, &KSceneModelEditorPropertyPageMesh::OnBnClickedButtonExpand)
	ON_BN_CLICKED(IDC_BUTTON_COLLAPSE, &KSceneModelEditorPropertyPageMesh::OnBnClickedButtonCollapse)
	ON_BN_CLICKED(IDC_CHECK_SHOWBONES, &KSceneModelEditorPropertyPageMesh::OnBnClickedCheckShowbones)
	ON_BN_CLICKED(IDC_BUTTON_CHANGEMODEL, &KSceneModelEditorPropertyPageMesh::OnBnClickedButtonChangemodel)
	ON_BN_CLICKED(IDC_BUTTON_HALFSPHERENORMAL, &KSceneModelEditorPropertyPageMesh::OnBnClickedButtonNomralHalfSphere)
	ON_BN_CLICKED(IDC_CHECK_SHOWBBOX, &KSceneModelEditorPropertyPageMesh::OnBnClickedCheckShowbbox)
	ON_BN_CLICKED(IDC_CHECK_SHOWPICTHON, &KSceneModelEditorPropertyPageMesh::OnBnClickedCheckShowpicthon)
	ON_BN_CLICKED(IDC_CHECK_PARTBBOX, &KSceneModelEditorPropertyPageMesh::OnBnClickedCheckPartbbox)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_X, &KSceneModelEditorPropertyPageMesh::OnNMCustomdrawSliderX)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_Y, &KSceneModelEditorPropertyPageMesh::OnNMCustomdrawSliderY)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_Z, &KSceneModelEditorPropertyPageMesh::OnNMCustomdrawSliderZ)
	ON_BN_CLICKED(IDC_BUN_CREATBONEBOX, &KSceneModelEditorPropertyPageMesh::OnBnClickedBunCreatbonebox)
	ON_BN_CLICKED(IDC_BUTTON_SAVETOLOW, &KSceneModelEditorPropertyPageMesh::OnBnClickedButtonSavetolow)
	ON_EN_CHANGE(IDC_EDIT_LOD, &KSceneModelEditorPropertyPageMesh::OnEnChangeEditLod)
	ON_BN_CLICKED(IDC_BUTTON_SAVETOMEDIUM, &KSceneModelEditorPropertyPageMesh::OnBnClickedButtonSavetomedium)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_BONESCALE, &KSceneModelEditorPropertyPageMesh::OnNMCustomdrawSliderBonescale)
	ON_EN_CHANGE(IDC_EDIT_BONESCALE, &KSceneModelEditorPropertyPageMesh::OnEnChangeEditBonescale)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorPropertyPageMesh message handlers

void KSceneModelEditorPropertyPageMesh::OnSelchangeListSocket()
{
	// TODO: Add your control notification handler code here
	HRESULT hr = E_FAIL;
	TCHAR strOutputInfo[256];
	IEKG3DMesh* pMesh = NULL;
    LPCTSTR pszSocketName = NULL;
    LPCTSTR pszSocketParentBoneName = NULL;
	int Index = m_ListBoxSocket.GetCurSel();
	DWORD dwSocketIndex = (DWORD)m_ListBoxSocket.GetItemData(Index);

	KGLOG_PROCESS_ERROR(m_lpCurModel);

	pMesh = GetReleasedMesh();
	KG_PROCESS_ERROR(pMesh);

    hr = pMesh->GetSocketName(dwSocketIndex, &pszSocketName);
    if(SUCCEEDED(hr))
	{
        pMesh->GetSocketParentBoneName(dwSocketIndex, &pszSocketParentBoneName);
        wsprintf(strOutputInfo, "插槽: %s 连接于 %s", pszSocketName, pszSocketParentBoneName);
		m_ButtonSocket.SetWindowText(strOutputInfo);
		m_scSocketName = pszSocketName;
	}
	else
	{
		m_ButtonSocket.SetWindowText("非法的插槽");
		m_scSocketName = "";
	}
	UpdateData(FALSE);

Exit0:
	return;
}

void KSceneModelEditorPropertyPageMesh::OnButtonSrename()
{
	// TODO: Add your control notification handler code here
	HRESULT hr = E_FAIL;
	TCHAR strOutputInfo[256];

	int Index = m_ListBoxSocket.GetCurSel();
	DWORD dwSocketIndex = (DWORD)m_ListBoxSocket.GetItemData(Index);
	IEKG3DMesh* pMesh = NULL;

	KGLOG_PROCESS_ERROR(m_lpCurModel);

	pMesh = GetReleasedMesh();
	KG_PROCESS_ERROR(pMesh);

    LPCTSTR pszSocketName = NULL;
    hr = pMesh->GetSocketName(dwSocketIndex, &pszSocketName);
	if(SUCCEEDED(hr))
	{
		UpdateData(TRUE);
		wsprintf(strOutputInfo, "将插槽 %s 重命名为 %s", pszSocketName, m_scSocketName);
		if(MessageBox(strOutputInfo, "确认重命名插槽", MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			wsprintf(strOutputInfo, "%s", m_scSocketName);
			pMesh->SetSocketName(dwSocketIndex, strOutputInfo);
			FillListBoxSocket();
		}
	}
Exit0:
	return;
}

void KSceneModelEditorPropertyPageMesh::OnClickTreeBone(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: Add your control notification handler code here

	*pResult = 0;
}

void KSceneModelEditorPropertyPageMesh::OnSelchangedTreeBone(NMHDR* pNMHDR, LRESULT* pResult)
{
	HRESULT hrRetCode = E_FAIL;
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	int nMax = 0;
	//DWORD dwIndex = 0;
	float fScale = 0.0f;
	int nSliderPos = 0;
	IEKG3DMesh* pMesh = NULL;
    IEKG3DMeshBone* piMeshBone =NULL;
    LPCTSTR pszName = NULL;
	TCHAR strOutputInfo[256];
	CSliderCtrl* pBoneScaleSlider = static_cast<CSliderCtrl*>(GetDlgItem(IDC_SLIDER_BONESCALE));

	KGLOG_PROCESS_ERROR(m_lpCurModel);
	if (m_oldMesh)
	{
		m_oldMesh->SetCurSelected((DWORD)-1);
	}
	
	pMesh = GetReleasedMesh();
	KG_PROCESS_ERROR(pMesh);
	m_oldMesh = pMesh;
	HTREEITEM hItem = m_TreeBone.GetSelectedItem();
	KG_PROCESS_ERROR(hItem);
	m_dwBoneIndex = (DWORD)m_TreeBone.GetItemData(hItem);
	pMesh->SetCurSelected(m_dwBoneIndex);
    hrRetCode = pMesh->GetBoneInfoName(m_dwBoneIndex, &pszName);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);
	wsprintf(strOutputInfo, "选定骨骼: %s", pszName);
	m_ButtonBone.SetWindowText(strOutputInfo);

    hrRetCode = m_lpCurModel->GetIEKG3DMeshBone(&piMeshBone);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrRetCode = piMeshBone->SetSelectedBone(pszName);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);


	
	if(m_dwBoneIndex >= 0)
	{
		float x,y,z;
		pMesh->GetBoneBBoxScale(m_dwBoneIndex,x,y,z);
		m_SliderX.SetPos(int(x*10));
		m_SliderY.SetPos(int(y*10));
		m_SliderZ.SetPos(int(z*10));
		SetBBoxScaleGroundEnable(TRUE);

		CSliderCtrl* pBoneScaleSlider = static_cast<CSliderCtrl*>(GetDlgItem(IDC_SLIDER_BONESCALE));


		D3DXVECTOR3 vScale(1,1,1);
		m_lpCurModel->GetBoneScale(m_dwBoneIndex,&vScale);

		int nMax = pBoneScaleSlider->GetRangeMax();
		int nMin = pBoneScaleSlider->GetRangeMin();
		int nSliderPos = (nMin + vScale.x * (nMax - nMin)) / s_dwMaxScale;

		pBoneScaleSlider->SetPos(nSliderPos);
	}
	else
	{
		SetBBoxScaleGroundEnable(FALSE);
	}
	*pResult = 0;
Exit0:
	return;
}

void KSceneModelEditorPropertyPageMesh::OnButtonSadd()
{
	// TODO: Add your control notification handler code here
	HRESULT hr = E_FAIL;
	IEKG3DMesh* pMesh = NULL;
    LPCTSTR pszName = NULL;
	HTREEITEM hItem = 0;
	int nSocketIndex = -1;
	DWORD dwBoneIndex = 0;
	TCHAR strOutputInfo[MAX_PATH];
	KG_PROCESS_ERROR(m_lpCurModel);

	pMesh = GetReleasedMesh();
	KG_PROCESS_ERROR(pMesh);

	hItem = m_TreeBone.GetSelectedItem();
	if(!hItem)
	{
		MessageBox("没有选定附着的骨骼！", "发生错误", MB_OK);
		goto Exit0;
	}

	UpdateData(TRUE);
	if(_strcmpi(m_scSocketName, "")==0)
	{
		MessageBox("请给插槽取个名字", "发生错误", MB_OK);
		goto Exit0;
	}

	pMesh->FindSocket(m_scSocketName, &nSocketIndex);
	//如果这个名字的Socket已经有了，那么就不添加了
	if (nSocketIndex != -1)
	{
		sprintf(strOutputInfo, "名字为%s的插槽已经存在.", m_scSocketName);
		MessageBox(strOutputInfo, "错误");
		goto Exit0;
	}


	dwBoneIndex = (DWORD)m_TreeBone.GetItemData(hItem);

    pMesh->GetBoneInfoName(dwBoneIndex, &pszName);

	wsprintf(strOutputInfo, "%s", m_scSocketName);

	pMesh->AddSocket(pszName, strOutputInfo);

    // 重新调整model的m_pSocketMatrix
    m_lpCurModel->ResizeSocketMatrix();





	FillListBoxSocket();

Exit0:
	return;
}

void KSceneModelEditorPropertyPageMesh::OnButtonSdelete()
{
	HRESULT hr = E_FAIL;
	int nIndex = 0;
	DWORD dwSocketIndex = 0;
	IEKG3DMesh* pMesh = NULL;
    LPCTSTR pszSocketName = NULL;

	KGLOG_PROCESS_ERROR(m_lpCurModel);

	pMesh = GetReleasedMesh();
	KG_PROCESS_ERROR(pMesh);

	nIndex = m_ListBoxSocket.GetCurSel();
	dwSocketIndex = (DWORD)m_ListBoxSocket.GetItemData(nIndex);
    hr = pMesh->GetSocketName(dwSocketIndex, &pszSocketName);
    if(SUCCEEDED(hr))
	{
		pMesh->RemoveSocket(pszSocketName);
	}
	FillListBoxSocket();

Exit0:

	return;
}

void KSceneModelEditorPropertyPageMesh::OnButtonBind()
{
	// TODO: Add your control notification handler code here
	HRESULT hr = E_FAIL;
	IEKG3DMesh* pMesh = NULL;
	DWORD dwNumSockets = 0;
    LPCTSTR pszSocketName = NULL;
	KSceneModelEditorMeshBindDialog Dlg;
	//MFCFramework::CMainFrame* pMainFrame = NULL;
	CFrameWnd* pFrame = NULL;
	KSceneModelEditorDoc* pDoc = NULL;
	IEKG3DSceneModelEditor* pEditor = NULL;

	KGLOG_PROCESS_ERROR(m_lpCurModel);

	pMesh = GetReleasedMesh();
	KG_PROCESS_ERROR(pMesh);

	hr = pMesh->GetSocketCount(&dwNumSockets);
	int nIndex = m_ListBoxSocket.GetCurSel();
	if(nIndex == LB_ERR)
		goto Exit0;

	DWORD dwSocketIndex = (DWORD)m_ListBoxSocket.GetItemData(nIndex);
	if((dwSocketIndex < 0) || (dwSocketIndex >= dwNumSockets))
		goto Exit0;


    hr = pMesh->GetSocketName(dwSocketIndex, &pszSocketName);
    KGLOG_COM_PROCESS_ERROR(hr);

	//(by dengzhihui 2006年7月6日 14:40:18
	CFrameWnd* pMainFrame = (CFrameWnd*)AfxGetMainWnd();
	ASSERT(pMainFrame);
	//)

	pFrame = pMainFrame->GetActiveFrame();
	ASSERT(pFrame);

	pDoc = (KSceneModelEditorDoc*)pFrame->GetActiveDocument();
	ASSERT(pDoc);

	IEKG3DScene* pScene = pDoc->GetScene();
	pScene->GetSceneModelEditor(&pEditor);//by dengzhihui Refactor 2006年11月24日
	ASSERT(pEditor);

	Dlg.SetSocket(pEditor, m_lpCurModel, pszSocketName);
	Dlg.DoModal();

Exit0:
	return;
}

void KSceneModelEditorPropertyPageMesh::OnButtonSunbind()
{
    HRESULT hRetCode = E_FAIL;
	int nCurSel = 0;
	TCHAR strSocketName[MAX_BONE_NAME_LENGTH];
	//MFCFramework::CMainFrame* pMainFrame = NULL;
	KSceneModelEditorDoc* pDoc = NULL;
	CFrameWnd* pFrame = NULL;
    IEKG3DModel *pModel = NULL;

	KSceneModelEditorPanelView* pWndPanel = static_cast<KSceneModelEditorPanelView*>(GetParent()->GetParent()->GetParent());
	KG_PROCESS_ERROR(pWndPanel);

	pDoc = (KSceneModelEditorDoc*) (pWndPanel->GetDocument());
	KG_PROCESS_ERROR(pDoc);

	KG_PROCESS_ERROR(m_lpCurModel);

	nCurSel = m_ListBoxSocket.GetCurSel();
	KG_PROCESS_ERROR(nCurSel != LB_ERR);

	m_ListBoxSocket.GetText(nCurSel, strSocketName);
    
	hRetCode = m_lpCurModel->UnbindChild(strSocketName);
    KGLOG_COM_PROCESS_ERROR(hRetCode);
Exit0:
	return;
}

void KSceneModelEditorPropertyPageMesh::OnCheckProgress()
{
	HRESULT hr = E_FAIL;
    IEKG3DMesh* pMesh = NULL;
    DWORD dwOption = 0;
    DWORD dwType = 0;
	KGLOG_PROCESS_ERROR(m_lpCurModel);

    m_lpCurModel->GetIEKG3DMesh(&pMesh);
	KG_PROCESS_ERROR(pMesh);
    pMesh->GetType(&dwType);
	if (dwType == MESHTYPE_DEFAULT)
	{
		if (m_ctlCheckProgress.GetCheck() == BST_CHECKED)
		{
			hr = pMesh->CreateProgressMesh();
			KG_COM_PROCESS_ERROR(hr);

			GetDlgItem(IDC_SLIDER_LOD)->EnableWindow(TRUE);
		}
		else
		{
			pMesh->GetOption(&dwOption);
            dwOption &= ~MESH_PROGRESS;
            pMesh->SetOption(dwOption);
			pMesh->SetNumFaceLOD(m_nLOD);
            GetDlgItem(IDC_SLIDER_LOD)->EnableWindow(FALSE);
		}
	}


Exit0:
	return ;
}

void KSceneModelEditorPropertyPageMesh::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	HRESULT hr = E_FAIL;
	TCHAR strName[256];
	TCHAR strScale[20];
	HTREEITEM hItem = NULL;
	DWORD dwIndex = 0;
	IEKG3DMesh* pMesh = NULL;
	CScrollBar* pBoneScaleBar = static_cast<CScrollBar*>(GetDlgItem(IDC_SLIDER_BONESCALE));
	CSliderCtrl* pBoneScaleSlider = reinterpret_cast<CSliderCtrl*>(pBoneScaleBar);
	LPD3DXPMESH pProgressMesh = NULL;

	KGLOG_PROCESS_ERROR(m_lpCurModel);

	pMesh = GetReleasedMesh();
	KG_PROCESS_ERROR(pMesh);

	if(pScrollBar == pBoneScaleBar)
	{
		//Min scale is 0.2 and max scale is 5
		hItem = m_TreeBone.GetSelectedItem();
		if(hItem)
		{
			dwIndex = (DWORD)m_TreeBone.GetItemData(hItem);

			int nSliderPos = pBoneScaleSlider->GetPos();
			int nMax = pBoneScaleSlider->GetRangeMax();
			int nMin = pBoneScaleSlider->GetRangeMin();
			float fScale = static_cast<float>(nSliderPos - nMin) / (nMax-nMin) * 2;
			//pMesh->SetBoneInfoScale(dwIndex, fScale, fScale, fScale);
            sprintf(strScale, "%1.4f", fScale);
			GetDlgItem(IDC_EDIT_BONESCALE)->SetWindowText(strScale);
		}
	}
	else if (pScrollBar == GetDlgItem(IDC_SLIDER_LOD))
	{
		UpdateData(TRUE);

		int nMin = 0;
		int nMax = 0;
		hr = pMesh->GetProgressMeshMinFaces(&nMin);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = pMesh->GetProgressMeshMaxFaces(&nMax);
		KGLOG_COM_PROCESS_ERROR(hr);

		int nCur = nMin + (int)((nMax-nMin) * m_nLOD * 0.01f);
		pMesh->SetNumFaceLOD(nCur);
		wsprintf(strName, "自动减面: %d", nCur);
		m_Static_LOD.SetWindowText(strName);
	}

Exit0:
	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

void KSceneModelEditorPropertyPageMesh::OnBnClickedButton4()
{


}

void KSceneModelEditorPropertyPageMesh::OnBnClickedButtonSset()
{

}

void KSceneModelEditorPropertyPageMesh::OnBnClickBillBoard()
{
    KSceneModelEditorDoc* pDoc = NULL;
    IEKG3DScene* pScene = NULL;
    IEKG3DSceneModelEditor* pSceneModel = NULL;

    KSceneModelEditorPanelView* pWndPanel = static_cast<KSceneModelEditorPanelView*>(GetParent()->GetParent()->GetParent());
    pDoc = (KSceneModelEditorDoc*) (pWndPanel->GetDocument());


    pScene = pDoc->GetScene();
    pScene->GetSceneModelEditor(&pSceneModel);

    IDirect3DSurface9* pSurface = pSceneModel->GetBillBoardTexture(1024, 800);
    if (!pSurface)
        return;

    /*
    LPCTSTR pName = NULL;
        TCHAR szImageName[MAX_PATH];
        m_lpCurModel->GetFullName(&pName);
        strncpy(szImageName, pName, sizeof(szImageName));
        TCHAR* pDot = _tcsrchr(szImageName, '.');
        if (!pDot)
            return;
    
        pDot[1] = 0;
    
        _tcscat(szImageName, "tga");*/
	TCHAR szImageName[MAX_PATH];
	{
		szImageName[0] = '0';
		LPCTSTR pName = NULL;
		m_lpCurModel->GetName(&pName);
		KG_CUSTOM_HELPERS::StrReplaceExtension(pName, _T("tga"), szImageName);
	}
    

    if (SUCCEEDED(D3DXSaveSurfaceToFile(szImageName,
        D3DXIFF_TGA, 
        pSurface,
        NULL,
        NULL
        )))
        AfxMessageBox("Success!!");
    else
        AfxMessageBox("Error!!");
    
}

void KSceneModelEditorPropertyPageMesh::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	HRESULT hr = E_FAIL;
	TCHAR strOutputInfo[256];
	int nMax = 0;
	DWORD dwIndex = 0;
	int nSliderPos = 0;
	float fScale = 0.0f;
	IEKG3DMesh* pMesh = NULL;
	HTREEITEM hItem = m_TreeBone.GetSelectedItem();
	CSliderCtrl* pBoneScaleSlider = static_cast<CSliderCtrl*>(GetDlgItem(IDC_SLIDER_BONESCALE));

	KGLOG_PROCESS_ERROR(m_lpCurModel);

	pMesh = GetReleasedMesh();
	KG_PROCESS_ERROR(pMesh);

	if (!hItem)
		goto Exit0;

	dwIndex = (DWORD)m_TreeBone.GetItemData(hItem);


	nMax = pBoneScaleSlider->GetRangeMax();
	fScale = 1.0f;

	{
		D3DXVECTOR3 vScale(1,1,1);
		hr = m_lpCurModel->SetBoneScale(dwIndex,&vScale);
		//hr = pMesh->SetBoneInfoScale(dwIndex, 1.0f, 1.0f, 1.0f);
		KGLOG_COM_PROCESS_ERROR(hr);
	}

    sprintf(strOutputInfo, "%1.4f", fScale);
	nSliderPos = static_cast<int>((fScale - 0.5f) / 1.5f * nMax);
	pBoneScaleSlider->SetPos(nSliderPos);
	GetDlgItem(IDC_EDIT_BONESCALE)->SetWindowText(strOutputInfo);

Exit0:
	return;
}



void KSceneModelEditorPropertyPageMesh::OnBnClickedButtonBssave()
{
	// TODO: Add your control notification handler code here
	/*if (m_lpCurModel)
	{
		CFileDialog DlgSaveBS(FALSE, "BS");
		if (DlgSaveBS.DoModal() == IDOK)
		{
			if (m_lpCurModel->SaveBoneScale(DlgSaveBS.GetPathName()) != S_OK)
			{
				MessageBox("保存骨骼缩放信息出错", "错误");
			}
		}
	}*/
}

void KSceneModelEditorPropertyPageMesh::OnBnClickedButtonBsload()
{
	// TODO: Add your control notification handler code here
	/*if (m_lpCurModel)
	{
		CFileDialog DlgLoadBoneScale(TRUE, "BS");
		if (DlgLoadBoneScale.DoModal() == IDOK)
		{
			if (m_lpCurModel->LoadBoneScale(DlgLoadBoneScale.GetPathName()) != S_OK)
			{
				MessageBox("读取骨骼缩放信息出错", "错误");
			}
		}
	}*/
}

void KSceneModelEditorPropertyPageMesh::OnBnClickedButtonNormalSpe()
{
	HRESULT hr = E_FAIL;
	IEKG3DMesh* pMesh = NULL;

	KGLOG_PROCESS_ERROR(m_lpCurModel);

	pMesh = GetReleasedMesh();
	KG_PROCESS_ERROR(pMesh);

	if(pMesh)
		pMesh->SphereNormal();
Exit0:
	return;
}

void KSceneModelEditorPropertyPageMesh::OnBnClickedButtonNomralHalfSphere()
{
    HRESULT hRetCode = E_FAIL;

	IEKG3DMesh* pMesh = NULL;
    KG_PROCESS_ERROR(m_lpCurModel);

    hRetCode = m_lpCurModel->GetIEKG3DMesh(&pMesh);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

	pMesh->HalfSphereNormal(0);

Exit0:
	return;
}

void KSceneModelEditorPropertyPageMesh::OnBnClickedButtonExpand()
{
	HTREEITEM hItem = m_TreeBone.GetSelectedItem();
	BOOL bExpandNext = FALSE;
	if (!hItem)
	{
		hItem = m_TreeBone.GetRootItem();
		bExpandNext = TRUE;
	}
	if (!hItem)
		return;

	m_bExpandBones = !m_bExpandBones;
	ExpandChildNode(hItem, bExpandNext, TRUE);
}

void KSceneModelEditorPropertyPageMesh::ExpandChildNode(HTREEITEM hItem, BOOL bExpandNext, BOOL bExpand)
{
	UINT uExpand = bExpand ? TVE_EXPAND : TVE_COLLAPSE;
	HTREEITEM hChildItem = NULL;
	do
	{

		hChildItem = m_TreeBone.GetChildItem(hItem);
		if (hChildItem)
		{
			ExpandChildNode(hChildItem, TRUE, bExpand);
		}
		m_TreeBone.Expand(hItem, uExpand);
		hItem = m_TreeBone.GetNextItem(hItem, TVGN_NEXT);
	}while (hItem && bExpandNext);
}


void KSceneModelEditorPropertyPageMesh::OnBnClickedButtonCollapse()
{
	HTREEITEM hItem = m_TreeBone.GetSelectedItem();
	BOOL bExpandNext = FALSE;
	if (!hItem)
	{
		hItem = m_TreeBone.GetRootItem();
		bExpandNext = TRUE;
	}
	if (!hItem)
		return;

	m_bExpandBones = !m_bExpandBones;
	ExpandChildNode(hItem, bExpandNext, FALSE);
}



void KSceneModelEditorPropertyPageMesh::OnBnClickedCheckShowbones()
{
	CButton* pButton = static_cast<CButton*>(GetDlgItem(IDC_CHECK_SHOWBONES));
	int nCheckState = pButton->GetCheck();
	KSceneModelEditorPanelView* pWndPanel = static_cast<KSceneModelEditorPanelView*>(GetParent()->GetParent()->GetParent());
	IEKG3DModel* pModel = NULL;
	KSceneModelEditorDoc* pDoc = NULL;

	KG_PROCESS_ERROR(pWndPanel);
	pDoc = (KSceneModelEditorDoc*) (pWndPanel->GetDocument());
	KG_PROCESS_ERROR(pDoc);

	pDoc->GetCurrentModel(&pModel);

	KG_PROCESS_ERROR(pModel);

	if (nCheckState == BST_CHECKED)
		pModel->ShowBones(TRUE);
	else
		pModel->ShowBones(FALSE);
Exit0:
	return;
}

void KSceneModelEditorPropertyPageMesh::OnBnClickedButtonChangemodel()
{
	HRESULT hr = E_FAIL;
	KSceneModelEditorDoc* pDoc = NULL;

	KSceneModelEditorPanelView* pWndPanel = static_cast<KSceneModelEditorPanelView*>(GetParent()->GetParent()->GetParent());
	IEKG3DModelST* pModelST = NULL;
	IEKG3DModel *pModel = NULL;
    IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
    CString strFileName;
	IEKG3DModel* pModelLoaded = NULL;
    IEKG3DModelTable *pModelTable = NULL;
	CFileDialog dlgLoad(TRUE, ".mesh");
	int nCurSelModelIndex = 0;
    DWORD dwType = 0;

	KG_PROCESS_ERROR(pWndPanel);

	pDoc = (KSceneModelEditorDoc*) (pWndPanel->GetDocument());
	KG_PROCESS_ERROR(pDoc);
    
    pDoc->GetCurrentModel(&pModel);

    hr = pModel->GetType(&dwType);
    KGLOG_COM_PROCESS_ERROR(hr);

	KG_PROCESS_ERROR(dwType == MESHTYPE_MODELST);
    pModel->GetIEKG3DModelST(&pModelST);

	KG_PROCESS_ERROR(dlgLoad.DoModal() == IDOK);

	strFileName = dlgLoad.GetPathName();

    pManager->GetIEKG3DModelTable(&pModelTable);

	hr = pModelTable->IELoadResourceFromFile(strFileName, 0, 0, (IKG3DResourceBase **)&pModelLoaded);
	KG_COM_PROCESS_ERROR(hr);

	nCurSelModelIndex = pWndPanel->GetCurrentPartIndex();
	KG_PROCESS_ERROR(nCurSelModelIndex != CB_ERR);

	pModel->ChangeModel(nCurSelModelIndex, pModelLoaded);
    
	m_lpCurModel = (IEKG3DModel*)pModelLoaded;
	pModelLoaded->Release();

Exit0:
	return;
}


BOOL KSceneModelEditorPropertyPageMesh::OnInitDialog()
{
	int nNumBones = 0;
	TCHAR strNumBones[MAX_PATH];
	HRESULT hRetCode = E_FAIL;
	IEKG3DMesh* pMesh = NULL;
	CPropertyPage::OnInitDialog();

	m_SliderX.SetRange(0,50,TRUE);
	m_SliderX.SetPos(10);
	m_SliderY.SetRange(0,50,TRUE);
	m_SliderY.SetPos(10);
	m_SliderZ.SetRange(0,50,TRUE);
	m_SliderZ.SetPos(10);

	if (m_lpCurModel)
	{
		g_GetSkeletonTreeFiller().FillSKTree(m_lpCurModel, m_TreeBone);
	
		FillListBoxSocket();
	
		KG_PROCESS_ERROR(m_lpCurModel);

		hRetCode = m_lpCurModel->GetIEKG3DMesh(&pMesh);
		KGLOG_COM_PROCESS_ERROR(hRetCode);

		hRetCode = pMesh->GetNumBones(&nNumBones);
		KGLOG_COM_PROCESS_ERROR(hRetCode);

		if (nNumBones)
		{
			sprintf(strNumBones, "Bone: %d", nNumBones);
		}
		else
		{
			sprintf(strNumBones, "No Bone exist.");
		}

		GetDlgItem(IDC_STATIC_NUMBONES)->SetWindowText(strNumBones);	
	}

Exit0:
	return TRUE;
}

void KSceneModelEditorPropertyPageMesh::OnBnClickedCheckShowbbox()
{
	CButton* pButton = static_cast<CButton*>(GetDlgItem(IDC_CHECK_SHOWBBOX));
	int nCheckState = pButton->GetCheck();
	KSceneModelEditorPanelView* pWndPanel = static_cast<KSceneModelEditorPanelView*>(GetParent()->GetParent()->GetParent());
	IEKG3DModel* pModel = NULL;
	KSceneModelEditorDoc* pDoc = NULL;

	KG_PROCESS_ERROR(pWndPanel);
	pDoc = (KSceneModelEditorDoc*) (pWndPanel->GetDocument());
	KG_PROCESS_ERROR(pDoc);

	pDoc->GetCurrentModel(&pModel);

	KG_PROCESS_ERROR(pModel);

	if (nCheckState == BST_CHECKED)
		pModel->SetbDrawBigBox(TRUE);
	else
		pModel->SetbDrawBigBox(FALSE);
	OnBnClickedCheckPartbbox();
Exit0:
	return;
}

BOOL KSceneModelEditorPropertyPageMesh::PreTranslateMessage( MSG* pMsg )
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)
		{
			if(pMsg->hwnd == GetDlgItem(IDC_BBOX_WEIGHT)->GetSafeHwnd())
			{
				CFrameWnd* pFrame = NULL;
				KSceneModelEditorDoc* pDoc = NULL;
				IEKG3DSceneModelEditor* pEditor = NULL;
				IEKG3DModel* pModel = NULL;
				UpdateData(TRUE);
				CFrameWnd* pMainFrame = (CFrameWnd*)AfxGetMainWnd();
				ASSERT(pMainFrame);
				pFrame = pMainFrame->GetActiveFrame();
				ASSERT(pFrame);
				pDoc = (KSceneModelEditorDoc*)pFrame->GetActiveDocument();
				ASSERT(pDoc);
				IEKG3DScene* pScene = pDoc->GetScene();
				ASSERT(pScene);
				pScene->GetSceneModelEditor(&pEditor);
				ASSERT(pEditor);
				pEditor->SetfBoneWeight(m_fWeight);
				
				/*pDoc->GetCurrentModel(&pModel);
				pModel->ComputeMeshBigBox();*/
				IEKG3DMesh* pMesh =NULL;
				pMesh = GetReleasedMesh();
				KGLOG_PROCESS_ERROR(pMesh);
				pMesh->ComputeBoneBBoxByIndex(m_dwBoneIndex);
				KG_PROCESS_ERROR(m_lpCurModel);
				m_lpCurModel->UpdateBoneBBoxInfo();

			}	
		}
	}
Exit0:
	return FALSE;
}

void KSceneModelEditorPropertyPageMesh::OnBnClickedCheckShowpicthon()
{
	CButton* pButton = static_cast<CButton*>(GetDlgItem(IDC_CHECK_SHOWPICTHON));
	int nCheckState = pButton->GetCheck();

	CFrameWnd* pFrame = NULL;
	KSceneModelEditorDoc* pDoc = NULL;
	IEKG3DSceneModelEditor* pEditor = NULL;
	UpdateData(TRUE);
	CFrameWnd* pMainFrame = (CFrameWnd*)AfxGetMainWnd();
	ASSERT(pMainFrame);
	pFrame = pMainFrame->GetActiveFrame();
	ASSERT(pFrame);
	pDoc = (KSceneModelEditorDoc*)pFrame->GetActiveDocument();
	ASSERT(pDoc);
	IEKG3DScene* pScene = pDoc->GetScene();
	ASSERT(pScene);
	pScene->GetSceneModelEditor(&pEditor);
	ASSERT(pEditor);

	if (nCheckState == BST_CHECKED)
		pEditor->SetbOnlyShowPitchOn(TRUE);
	else
		pEditor->SetbOnlyShowPitchOn(FALSE);
	
}

void KSceneModelEditorPropertyPageMesh::OnBnClickedCheckPartbbox()
{
	KG_PROCESS_ERROR(m_lpCurModel);
	CButton* pButton = static_cast<CButton*>(GetDlgItem(IDC_CHECK_PARTBBOX));
	int nCheckState = pButton->GetCheck();
	
	KSceneModelEditorPanelView* pWndPanel = static_cast<KSceneModelEditorPanelView*>(GetParent()->GetParent()->GetParent());
	IEKG3DModel* pModel = NULL;
	KSceneModelEditorDoc* pDoc = NULL;

	KG_PROCESS_ERROR(pWndPanel);
	pDoc = (KSceneModelEditorDoc*) (pWndPanel->GetDocument());
	KG_PROCESS_ERROR(pDoc);

	pDoc->GetCurrentModel(&pModel);

	KG_PROCESS_ERROR(pModel);

	pButton = static_cast<CButton*>(GetDlgItem(IDC_CHECK_SHOWBBOX));
	int nState = pButton->GetCheck();

	if (nCheckState == BST_CHECKED && nState == BST_CHECKED)
	{
		pModel->SetbDrawBigBox(FALSE);
		//IEKG3DMesh* pMesh = GetReleasedMesh();
		m_lpCurModel->SetbDrawBigBox(TRUE);
	}
	else
	{
		if (nState == BST_CHECKED)
			pModel->SetbDrawBigBox(TRUE);
		else
			m_lpCurModel->SetbDrawBigBox(FALSE);
	}
Exit0:
	return;
}

void KSceneModelEditorPropertyPageMesh::OnNMCustomdrawSliderX(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	KG_PROCESS_ERROR(m_lpCurModel);
	IEKG3DMesh* pMesh = NULL;

	int nScaleX = m_SliderX.GetPos();
	m_fScaleX = (float)nScaleX * 0.1f;
	UpdateData(FALSE);
	pMesh = GetReleasedMesh();
	KG_PROCESS_ERROR(pMesh);
	pMesh->SetBoneBBoxScale(m_dwBoneIndex,m_fScaleX,m_fScaleY,m_fScaleZ);
	m_lpCurModel->UpdateBoneBBoxInfo();
Exit0:
	return ;
}

void KSceneModelEditorPropertyPageMesh::OnNMCustomdrawSliderY(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	KG_PROCESS_ERROR(m_lpCurModel);
	IEKG3DMesh* pMesh = NULL;

	int nScaleY = m_SliderY.GetPos();
	m_fScaleY = (float)nScaleY * 0.1f;
	UpdateData(FALSE);
	pMesh = GetReleasedMesh();
	KG_PROCESS_ERROR(pMesh);
	pMesh->SetBoneBBoxScale(m_dwBoneIndex,m_fScaleX,m_fScaleY,m_fScaleZ);
	m_lpCurModel->UpdateBoneBBoxInfo();
Exit0:
	return ;
}

void KSceneModelEditorPropertyPageMesh::OnNMCustomdrawSliderZ(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	KG_PROCESS_ERROR(m_lpCurModel);
	IEKG3DMesh* pMesh = NULL;

	int nScaleZ = m_SliderZ.GetPos();
	m_fScaleZ = (float)nScaleZ * 0.1f;
	UpdateData(FALSE);
	pMesh = GetReleasedMesh();
	KG_PROCESS_ERROR(pMesh);
	pMesh->SetBoneBBoxScale(m_dwBoneIndex,m_fScaleX,m_fScaleY,m_fScaleZ);
	m_lpCurModel->UpdateBoneBBoxInfo();
Exit0:
	return ;
}

void KSceneModelEditorPropertyPageMesh::SetBBoxScaleGroundEnable(BOOL bVal)
{
	GetDlgItem(IDC_EDIT_BBOXX)->EnableWindow(bVal);
	GetDlgItem(IDC_EDIT_BBOXY)->EnableWindow(bVal);
	GetDlgItem(IDC_EDIT_BBOXZ)->EnableWindow(bVal);
	m_SliderX.EnableWindow(bVal);
	m_SliderY.EnableWindow(bVal);
	m_SliderZ.EnableWindow(bVal);
}

void KSceneModelEditorPropertyPageMesh::OnBnClickedBunCreatbonebox()
{
	KG_PROCESS_ERROR(m_lpCurModel);
	IEKG3DMesh* pMesh = GetReleasedMesh();
	pMesh->CreateBBoxOfBone();
	m_lpCurModel->UpdateBoneBBoxInfo();
Exit0:
	;

}

void KSceneModelEditorPropertyPageMesh::OnBnClickedButtonSavetolow()
{
	KG_PROCESS_ERROR(m_lpCurModel);
	IEKG3DMesh* pMesh = GetReleasedMesh();

	if(pMesh)
	{
		LPCSTR pName;
		pMesh->GetName(&pName);
		TCHAR szName[MAX_PATH];

		TCHAR path_buffer[256];
		TCHAR drive[256];
		TCHAR dir[256];
		TCHAR fname[256];
		TCHAR ext[256];

		TCHAR szWorkingDir[MAX_PATH];
		::g_pEngineManager->GetDefWorkDirectory(szWorkingDir,MAX_PATH);

		wsprintf(path_buffer,"%s",pName);
		_splitpath( path_buffer, drive, dir, fname, ext );

		wsprintf(szName,"%s%s%s%s_l%s",szWorkingDir,drive,dir,fname,ext);

		HRESULT hr = pMesh->SavePrograssMeshAsLowMesh(szName);
		if(SUCCEEDED(hr))
		{
			MessageBox(szName,"地面低模保存成功");
		}
		else
		{
			MessageBox(szName,"地面低模保存失败");
		}
	}

Exit0:
	;
}

void KSceneModelEditorPropertyPageMesh::OnEnChangeEditLod()
{
	
	UpdateData(TRUE);
	KG_PROCESS_ERROR(m_lpCurModelParent);

	m_lpCurModelParent->IESetLod(m_fLod);
	
Exit0:
	;
}

void KSceneModelEditorPropertyPageMesh::OnBnClickedButtonSavetomedium()
{
	KG_PROCESS_ERROR(m_lpCurModel);
	IEKG3DMesh* pMesh = GetReleasedMesh();

	if(pMesh)
	{
		LPCSTR pName;
		pMesh->GetName(&pName);
		TCHAR szName[MAX_PATH];

		TCHAR path_buffer[256];
		TCHAR drive[256];
		TCHAR dir[256];
		TCHAR fname[256];
		TCHAR ext[256];

		TCHAR szWorkingDir[MAX_PATH];
		::g_pEngineManager->GetDefWorkDirectory(szWorkingDir,MAX_PATH);

		wsprintf(path_buffer,"%s",pName);
		_splitpath( path_buffer, drive, dir, fname, ext );

		wsprintf(szName,"%s%s%s%s_m%s",szWorkingDir,drive,dir,fname,ext);

		HRESULT hr = pMesh->SavePrograssMeshAsLowMesh(szName);
		if(SUCCEEDED(hr))
		{
			MessageBox(szName,"地面低模保存成功");
		}
		else
		{
			MessageBox(szName,"地面低模保存失败");
		}
	}

Exit0:
	;
}

void KSceneModelEditorPropertyPageMesh::OnNMCustomdrawSliderBonescale(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	// TODO: Add your control notification handler code here
	HRESULT hr = E_FAIL;
	TCHAR strOutputInfo[256];
	int nMax = 0;
	int nMin = 0;
	DWORD dwIndex = 0;
	int nSliderPos = 0;
	D3DXVECTOR3 vScale(1.0f ,1.0f ,1.0f);
	float fScale = 0.0f;
	IEKG3DMesh* pMesh = NULL;
	HTREEITEM hItem = m_TreeBone.GetSelectedItem();
	CSliderCtrl* pBoneScaleSlider = static_cast<CSliderCtrl*>(GetDlgItem(IDC_SLIDER_BONESCALE));

	KGLOG_PROCESS_ERROR(m_lpCurModel);
	KG_PROCESS_ERROR(hItem);

	dwIndex = (DWORD)m_TreeBone.GetItemData(hItem);
	nMax = pBoneScaleSlider->GetRangeMax();
	nMin = pBoneScaleSlider->GetRangeMin();
	nSliderPos = pBoneScaleSlider->GetPos();

	fScale = (nSliderPos-nMin) * 1.0f / (nMax - nMin)  * 2;
	hr = m_lpCurModel->SetBoneScale(dwIndex,&D3DXVECTOR3(fScale, fScale, fScale));
	KGLOG_COM_PROCESS_ERROR(hr);
	
	sprintf(strOutputInfo, "%1.4f", fScale);
	GetDlgItem(IDC_EDIT_BONESCALE)->SetWindowText(strOutputInfo);
Exit0:
	return;
}

void KSceneModelEditorPropertyPageMesh::OnEnChangeEditBonescale()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}
