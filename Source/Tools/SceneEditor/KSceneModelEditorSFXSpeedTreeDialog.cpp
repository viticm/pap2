// KSceneModelEditorSFXSpeedTreeDialog.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KSceneModelEditorSFXSpeedTreeDialog.h"
#include "IEKG3DModelSpeedTree.h"
#include "kcolordialog.h"
#include "IEEditor.h"
#include "IEKG3DMesh.h"
#include "IEKG3DModelTable.h"
#include "KSceneModelEditorFrameWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static LPCSTR s_strDefaultLeafTexturStartPath = "data\\source\\maps_source\\Texture\\";
static LPCSTR s_strDefaultSpdTreeFilePath = "data\\source\\maps_source\\树\\";
static LPCSTR s_strDefaultLeafTexturePrefix = "SY*";
/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorSFXSpeedTreeDialog dialog


KSceneModelEditorSFXSpeedTreeDialog::KSceneModelEditorSFXSpeedTreeDialog(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneModelEditorSFXSpeedTreeDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(KSceneModelEditorSFXSpeedTreeDialog)
	m_fSize = 0.0f;
	m_nPitch = 0;
	m_nRoll = 0;
	m_nWave = 0;
	m_fAccelerate = 0.0f;
	m_nMaxSpeed = 0;
	m_lpSpeedTree = NULL;
	m_lpModelEditor = NULL;
	m_fLeafRadius = 40;
	m_nLeafAuto = 100;
	//}}AFX_DATA_INIT
}


void KSceneModelEditorSFXSpeedTreeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KSceneModelEditorSFXSpeedTreeDialog)
	DDX_Control(pDX, IDC_BUTTON_DIFFUSE, m_ColorButton_Diffuse);
	DDX_Control(pDX, IDC_BUTTON_AMBIENT, m_ColorButton_Ambient);
	DDX_Control(pDX, IDC_TREE1, m_Tree);
	DDX_Text(pDX, IDC_EDIT_SIZE, m_fSize);
	DDV_MinMaxFloat(pDX, m_fSize, 0.f, 10000.f);
	DDX_Text(pDX, IDC_EDIT_PITCH, m_nPitch);
	DDX_Text(pDX, IDC_EDIT_ROLL, m_nRoll);
	DDX_Text(pDX, IDC_EDIT_WAVE, m_nWave);
	DDX_Text(pDX, IDC_EDIT_ACCELERATE, m_fAccelerate);
	DDX_Text(pDX, IDC_EDIT_MAXSPEED, m_nMaxSpeed);
	DDX_Text(pDX, IDC_EDIT_LEAFRADIUS, m_fLeafRadius);
	DDX_Text(pDX, IDC_EDIT_LEAFNUM, m_nLeafAuto);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KSceneModelEditorSFXSpeedTreeDialog, CDialog)
	//{{AFX_MSG_MAP(KSceneModelEditorSFXSpeedTreeDialog)
//	ON_NOTIFY(NM_CLICK, IDC_TREE1, OnClickTree1)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_ADDLEAF, OnBnClickedButtonAddleaf)
	ON_BN_CLICKED(IDC_CHECK_MOVE, OnBnClickedCheckMove)
	ON_BN_CLICKED(IDC_RADIO4, OnBnClickedRadio4)
	ON_BN_CLICKED(IDC_RADIO5, OnBnClickedRadio5)
	ON_BN_CLICKED(IDC_RADIO10, OnBnClickedRadio10)
	ON_BN_CLICKED(IDC_RADIO6, OnBnClickedRadio6)
	ON_BN_CLICKED(IDC_BUTTON_AMBIENT, OnBnClickedButtonAmbient)
	ON_BN_CLICKED(IDC_BUTTON_DIFFUSE, OnBnClickedButtonDiffuse)
	ON_BN_CLICKED(IDC_BUTTON_SAVEAS, OnBnClickedButtonSaveAs)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnBnClickedButtonDelete)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE1, OnNMDblclkTree1)
	ON_BN_CLICKED(IDC_CHECK_DRAWBONE, OnBnClickedCheckDrawbone)
	ON_BN_CLICKED(IDC_CHECK_SHOWLEAF, OnBnClickedCheckShowLeaf)
	ON_BN_CLICKED(IDC_BUTTON_AUTO, OnBnClickedButtonAuto)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, OnBnClickedButtonRefresh)
	ON_BN_CLICKED(IDC_BUTTON_LOADMTL, OnBnClickedButtonLoadmtl)
	ON_BN_CLICKED(IDC_BUTTON_CHANGELEAFTEX, OnBnClickedButtonChangeleaftex)
	ON_MESSAGE(WM_COLORPOINTER_CHANGE, OnColorChange)

	ON_NOTIFY(NM_CLICK, IDC_TREE1, &KSceneModelEditorSFXSpeedTreeDialog::OnNMClickTree1)
	ON_NOTIFY(TVN_ITEMEXPANDED, IDC_TREE1, &KSceneModelEditorSFXSpeedTreeDialog::OnTvnItemexpandedTree1)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &KSceneModelEditorSFXSpeedTreeDialog::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_LOAD, &KSceneModelEditorSFXSpeedTreeDialog::OnBnClickedButtonLoad)
	ON_BN_CLICKED(IDC_CHECK_ISOLATE, &KSceneModelEditorSFXSpeedTreeDialog::OnBnClickedCheckIsolate)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorSFXSpeedTreeDialog message handlers

LRESULT KSceneModelEditorSFXSpeedTreeDialog::OnColorChange(WPARAM wParam, LPARAM lParam)
{
	SetLeafDiffuse(m_Diffuse);
	SetLeafAmbient(m_Ambient);
	return S_OK;
}

#define GETSPEEDTREE() \
	IEKG3DSpeedTreeModifier* pModi = NULL;\
	m_lpModelEditor->GetSpeedTreeModifier(&pModi);


void KSceneModelEditorSFXSpeedTreeDialog::OnBnClickedButtonAddleaf()
{
	DWORD dwAttachedBone = 0;
	
	GETSPEEDTREE();

	GetCurSelectedBone(dwAttachedBone);
	
	KG_PROCESS_ERROR(m_lpModelEditor);
	KG_PROCESS_ERROR(dwAttachedBone != 0xfffffff);

	//m_lpModelEditor->GetSpeedTreeModifier()->AddLeaf(dwAttachedBone);	
	pModi->AddLeaf(dwAttachedBone);

	KG_PROCESS_ERROR(dwAttachedBone != 0xfffffff);

	BOOL bReturn = m_Tree.SelectItem(m_vecBoneNodes[dwAttachedBone].hItem);
Exit0:
	return;
}

HTREEITEM KSceneModelEditorSFXSpeedTreeDialog::GetCurSelectedBone(DWORD& dwIndex)
{
	int nTypeInfo = 0;
	HTREEITEM hSelected = 0;
	IEKG3DModel *pModelTree = NULL;

    dwIndex = 0xfffffff;
	KGLOG_PROCESS_ERROR(m_lpSpeedTree);
    
    m_lpSpeedTree->GetModelTree(&pModelTree);

	KGLOG_PROCESS_ERROR(pModelTree);

	hSelected = m_Tree.GetSelectedItem();
	KG_PROCESS_ERROR(hSelected);

	m_Tree.GetItemImage(hSelected, nTypeInfo, nTypeInfo);
	KG_PROCESS_SUCCESS(nTypeInfo == ENU_LEAF_NODE);

	dwIndex = static_cast<DWORD>(m_Tree.GetItemData(hSelected));
Exit1:
Exit0:
	return hSelected;
}

void KSceneModelEditorSFXSpeedTreeDialog::OnBnClickedCheckMove()
{
	UpdateData(TRUE);
	ToggleMoveLeaf();

	BOOL Enable = ! IsDlgButtonChecked(IDC_CHECK_MOVE);

	GetDlgItem(IDC_BUTTON_SELECTALL)->EnableWindow(Enable);
	GetDlgItem(IDC_BUTTON_SELECTNONE)->EnableWindow(Enable);
	GetDlgItem(IDC_BUTTON_SELECTGROUP)->EnableWindow(Enable);
	GetDlgItem(IDC_TREE1)->EnableWindow(Enable);
	GetDlgItem(IDC_BUTTON_ADDLEAF)->EnableWindow(Enable);
	GetDlgItem(IDC_BUTTON_DELETE)->EnableWindow(Enable);
	GetDlgItem(IDC_BUTTON_REFRESH)->EnableWindow(Enable);
	GetDlgItem(IDC_BUTTON_AUTO)->EnableWindow(Enable);
	GetDlgItem(IDC_BUTTON_LOAD)->EnableWindow(Enable);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(Enable);
	GetDlgItem(IDC_BUTTON_OPENTREEMESH)->EnableWindow(Enable);
	GetDlgItem(IDC_BUTTON_EDITTREEMESH)->EnableWindow(Enable);

}

void KSceneModelEditorSFXSpeedTreeDialog::ToggleMoveLeaf()
{
    IEKG3DModel *pModelTree = NULL;
    BOOL bNeedUpdatePosition = FALSE;
	KG_PROCESS_ERROR(m_lpModelEditor);
	KGLOG_PROCESS_ERROR(m_lpSpeedTree);
    
    m_lpSpeedTree->GetModelTree(&pModelTree);

	KGLOG_PROCESS_ERROR(pModelTree);
    
    m_lpSpeedTree->GetNeedUpdatePositionState(&bNeedUpdatePosition);
	if(bNeedUpdatePosition)
	{
// 		m_lpModelEditor->m_listSelectedEntityBack = m_lpModelEditor->m_listSelectedEntity;
// 		m_lpModelEditor->m_listSelectableEntityBack = m_lpModelEditor->m_listSelectableEntity;
// 		m_lpModelEditor->m_listSelectedEntity.clear();
// 		m_lpModelEditor->m_listSelectableEntity.clear();
		m_lpModelEditor->MoveSelectEntityToBack();
	
		for(size_t i = 0; i < m_vecBoneNodes.size(); i++)
		{
			HTREEITEM hBone = m_vecBoneNodes[i].hItem;


			HTREEITEM hLeaf = m_Tree.GetChildItem(hBone);
			while(hLeaf)
			{
				if(m_Tree.GetCheck(hLeaf))
				{
					IELeaf *pLeaf = (IELeaf *)m_Tree.GetItemData(hLeaf);

//					KG3DSceneEntity Entity(SCENEENTITY_SPEEDTREELEAF, pLeaf);
                    m_lpModelEditor->AddRenderEntity(SCENEENTITY_SPEEDTREELEAF, pLeaf);
// 					m_lpModelEditor->m_listSelectableEntity.push_back(Entity);
// 					m_lpModelEditor->m_listSelectedEntity.push_back(Entity);
					m_lpModelEditor->PushEntityToSelected(SCENEENTITY_SPEEDTREELEAF, pLeaf);//by dengzhihui Refactor 2006年11月24日
				}
				hLeaf = m_Tree.GetNextItem(hLeaf,TVGN_NEXT);
			}
		}
		m_lpSpeedTree->SetNeedUpdatePositionState(FALSE);
        m_lpModelEditor->SetEditLeafMode(TRUE);
	}
	else
	{
		for(size_t i = 0; i < m_vecBoneNodes.size(); i++)
		{
			HTREEITEM hBone = m_vecBoneNodes[i].hItem;

			HTREEITEM hLeaf = m_Tree.GetChildItem(hBone);
			while(hLeaf)
			{
				if(m_Tree.GetCheck(hLeaf))
				{
					IELeaf *pLeaf = (IELeaf *) m_Tree.GetItemData(hLeaf);
					
					IEKG3DSceneSelectBase* pSelector = NULL;
					m_lpModelEditor->GetSceneSelector(&pSelector);

					IEKG3DScene* pScene = NULL;
					m_lpModelEditor->GetScene(&pScene);
					pScene->RemoveRenderEntity (SCENEENTITY_SPEEDTREELEAF, pLeaf);
					//pSelector->RemoveSelectableEntity(SCENEENTITY_SPEEDTREELEAF,pLeaf, TRUE);//// Last modify by Chen TianHong:2008-4-16 11:02:11 用最后一个参数不需要下面这句
					//pSelector->RemoveSelectedEntity(SCENEENTITY_SPEEDTREELEAF,pLeaf);
					pLeaf->UpdateLeafOriginPosition();
				}

				hLeaf = m_Tree.GetNextItem(hLeaf,TVGN_NEXT);
			}
		}
		//m_lpSpeedTree->ComputeLeafOffsetPosition();
		m_lpSpeedTree->FillLeafVerticesBuffer();

		m_lpSpeedTree->SetNeedUpdatePositionState(TRUE);
		
		//by dengzhihui Refactor 2006年11月24日
		//m_lpModelEditor->m_listSelectedEntity = m_lpModelEditor->m_listSelectedEntityBack;
		//m_lpModelEditor->m_listSelectableEntity = m_lpModelEditor->m_listSelectableEntityBack;
		m_lpModelEditor->CopyBackToSelectedEntity();

		m_lpSpeedTree->SetLocalBBoxDirty();
		m_lpSpeedTree->ComputeBBox();
		m_lpModelEditor->SetEditLeafMode(FALSE);
	}
Exit0:
	return;
}

void KSceneModelEditorSFXSpeedTreeDialog::SetLeafUV(DWORD Type)
{
	KGLOG_PROCESS_ERROR(m_lpSpeedTree);

    IEKG3DModel *pModelTree = NULL;
    m_lpSpeedTree->GetModelTree(&pModelTree);

	KGLOG_PROCESS_ERROR(pModelTree);

	for(size_t i = 0; i < m_vecBoneNodes.size(); i++)
	{
		HTREEITEM hBone = m_vecBoneNodes[i].hItem;
		HTREEITEM hLeaf = m_Tree.GetChildItem(hBone);
		while(hLeaf)
		{
			IELeaf *pLeaf = (IELeaf *)(m_Tree.GetItemData(hLeaf));
			if(m_Tree.GetCheck(hLeaf))
			{
				pLeaf->SetTextureUV((INT)Type);
			}
			hLeaf = m_Tree.GetNextItem(hLeaf,TVGN_NEXT);
		}
	}
Exit0:
	return;
}

void KSceneModelEditorSFXSpeedTreeDialog::SetLeafSize(float size)
{
	KGLOG_PROCESS_ERROR(m_lpSpeedTree);

    IEKG3DModel *pModelTree = NULL;
    m_lpSpeedTree->GetModelTree(&pModelTree);

	KGLOG_PROCESS_ERROR(pModelTree);

	for(size_t i = 0; i < m_vecBoneNodes.size(); i++)
	{
		HTREEITEM hBone = m_vecBoneNodes[i].hItem;

		HTREEITEM hLeaf = m_Tree.GetChildItem(hBone);
		while(hLeaf)
		{
			IELeaf *pLeaf = (IELeaf *)m_Tree.GetItemData(hLeaf);
			if(m_Tree.GetCheck(hLeaf))
			{
				pLeaf->SetSize(size);
			}

			hLeaf = m_Tree.GetNextItem(hLeaf,TVGN_NEXT);
		}
	}
Exit0:
	return;
}

void KSceneModelEditorSFXSpeedTreeDialog::SetLeafWave(float Wave)
{
	KG_PROCESS_ERROR(m_lpSpeedTree);
    
    IEKG3DModel *pModelTree = NULL;
    m_lpSpeedTree->GetModelTree(&pModelTree);

	KG_PROCESS_ERROR(pModelTree);

	for(size_t i = 0; i < m_vecBoneNodes.size(); i++)
	{
		HTREEITEM hBone = m_vecBoneNodes[i].hItem;
		HTREEITEM hLeaf = m_Tree.GetChildItem(hBone);
		while(hLeaf)
		{
			IELeaf *pLeaf = (IELeaf *) m_Tree.GetItemData(hLeaf);
			if(m_Tree.GetCheck(hLeaf))
			{
				pLeaf->SetWave(Wave);
			}

			hLeaf = m_Tree.GetNextItem(hLeaf,TVGN_NEXT);
		}
	}
Exit0:
	return;
}

void KSceneModelEditorSFXSpeedTreeDialog::SetLeafAmbient(D3DCOLORVALUE Ambient)
{
	KG_PROCESS_ERROR(m_lpSpeedTree);

    IEKG3DModel *pModelTree = NULL;
    m_lpSpeedTree->GetModelTree(&pModelTree);

	KG_PROCESS_ERROR(pModelTree);

	for(size_t i = 0; i < m_vecBoneNodes.size(); i++)
	{
		HTREEITEM hBone = m_vecBoneNodes[i].hItem;
		HTREEITEM hLeaf = m_Tree.GetChildItem(hBone);
		while(hLeaf)
		{
			IELeaf *pLeaf = (IELeaf *)m_Tree.GetItemData(hLeaf);
			if(m_Tree.GetCheck(hLeaf))
			{
                pLeaf->SetAmbient(&Ambient);
			}

			hLeaf = m_Tree.GetNextItem(hLeaf,TVGN_NEXT);
		}
	}
Exit0:
	return;
}

void KSceneModelEditorSFXSpeedTreeDialog::SetLeafDiffuse(D3DCOLORVALUE Diffuse)
{
	KG_PROCESS_ERROR(m_lpSpeedTree);

    IEKG3DModel *pModelTree = NULL;
    m_lpSpeedTree->GetModelTree(&pModelTree);

	KG_PROCESS_ERROR(pModelTree);

	for(size_t i = 0; i < m_vecBoneNodes.size(); i++)
	{
		HTREEITEM hBone = m_vecBoneNodes[i].hItem;
		HTREEITEM hLeaf = m_Tree.GetChildItem(hBone);
		while(hLeaf)
		{
			IELeaf *pLeaf = (IELeaf *)m_Tree.GetItemData(hLeaf);
			if(m_Tree.GetCheck(hLeaf))
			{
				pLeaf->SetDiffuse(&Diffuse);
			}
			hLeaf = m_Tree.GetNextItem(hLeaf,TVGN_NEXT);
		}
	}
Exit0:
	return;
}

void KSceneModelEditorSFXSpeedTreeDialog::OnBnClickedRadio4()
{
	SetLeafUV(0);
}

void KSceneModelEditorSFXSpeedTreeDialog::OnBnClickedRadio5()
{
	SetLeafUV(1);
}

void KSceneModelEditorSFXSpeedTreeDialog::OnBnClickedRadio10()
{
	SetLeafUV(2);
}

void KSceneModelEditorSFXSpeedTreeDialog::OnBnClickedRadio6()
{
	SetLeafUV(3);
}



void KSceneModelEditorSFXSpeedTreeDialog::OnBnClickedButtonAmbient()
{
	// TODO: Add your control notification handler code here
	GetColorPickerDialog()->Init(&m_Ambient, "材质环境光", this);
	
	UpdateData(TRUE);
	SetLeafAmbient(m_Ambient);
}

void KSceneModelEditorSFXSpeedTreeDialog::OnBnClickedButtonDiffuse()
{
	// TODO: Add your control notification handler code here
	GetColorPickerDialog()->Init(&m_Diffuse, "材质漫反射", this);
	
	UpdateData(TRUE);
	SetLeafDiffuse(m_Diffuse);
}


BOOL KSceneModelEditorSFXSpeedTreeDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	FLOAT fRollAccelerate = 0.0f;
    FLOAT fMaxRollSpeed = 0.0f;
    FLOAT fRollRight = 0.0f;
    FLOAT fPitchLimit = 0.0f;

	// TODO: Add extra initialization here
	if(m_lpSpeedTree)
	{
		/*m_fAccelerate = m_lpSpeedTree->m_fRollAccelerate;
		m_fMaxSpeed = m_lpSpeedTree->m_fMaxRollSpeed;
		m_fRoll = m_lpSpeedTree->m_fRollRight;
		m_fPitch = m_lpSpeedTree->m_fPitchLimit;
		*/
        m_lpSpeedTree->GetRollAccelerate(&fRollAccelerate);
		m_fAccelerate =  fRollAccelerate * 10000000.0f;
        
        m_lpSpeedTree->GetMaxRollSpeed(&fMaxRollSpeed);
		m_nMaxSpeed = (int)(fMaxRollSpeed * 10000.0f);
    
        m_lpSpeedTree->GetRollRight(&fRollRight);
		m_nRoll = (int)(fRollRight / PI * 180.0f);

        m_lpSpeedTree->GetRollRight(&fPitchLimit);
		m_nPitch = (int)(fPitchLimit / PI * 180.0f);
		UpdateData(FALSE);
	}
	
	OnBnClickedButtonRefresh();
	static_cast<CButton*>(GetDlgItem(IDC_CHECK_SHOWLEAF))->SetCheck(BST_CHECKED);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneModelEditorSFXSpeedTreeDialog::OnBnClickedButtonSaveAs()
{
	HRESULT hr = E_FAIL;
	CString strPathName;
	TCHAR strCurrentPathSave[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, strCurrentPathSave);
	::SetCurrentDirectory(g_szDefWorkDirectory);

	TCHAR strDefaultSpdTreeFilePath[MAX_PATH];
	CFileDialog dlgSave(FALSE, 
		"STree", 
		0, 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
		"SpeedTree文件(*.STree)|*.STree||");
	sprintf_s(strDefaultSpdTreeFilePath, 
		MAX_PATH,
		"%s%s",
		g_szDefWorkDirectory,
		s_strDefaultSpdTreeFilePath);
	dlgSave.m_ofn.lpstrInitialDir = strDefaultSpdTreeFilePath;
	KG_PROCESS_ERROR(m_lpSpeedTree);
	
	KG_PROCESS_SUCCESS(dlgSave.DoModal() != IDOK);

    strPathName = dlgSave.GetPathName();
    strPathName.MakeLower();
    int nStreeIndex = strPathName.Find(".stree");
    strPathName.Insert(nStreeIndex, "_ST.mesh");
	hr = m_lpSpeedTree->SaveToFile(strPathName, ENU_SAVE_AS);
	if (FAILED(hr))
	{
		MessageBox("保存失败.");
	}
	else
	{
		MessageBox("保存成功.");
        CFrameWnd* pMainFrame = static_cast<CFrameWnd*>(AfxGetMainWnd());
        KSceneModelEditorFrameWnd* pWnd = static_cast<KSceneModelEditorFrameWnd*>(pMainFrame->GetActiveFrame());
        if (pWnd)
        {
            CDocument *pDoc = pWnd->GetActiveDocument();
            KG_PROCESS_SUCCESS(!pDoc);
            pDoc->SetPathName(strPathName, FALSE);
        }
	}
Exit1:
Exit0:
	::SetCurrentDirectory(strCurrentPathSave);
	return;

}

void KSceneModelEditorSFXSpeedTreeDialog::OnDeleteLeaf(HTREEITEM hLeaf, BOOL bUpdateUI)
{
	IELeaf* pLeaf = NULL;
	
	KG_PROCESS_ERROR(m_lpModelEditor);
	
	pLeaf = reinterpret_cast<IELeaf *>(m_Tree.GetItemData(hLeaf));
	KG_PROCESS_ERROR(pLeaf);
	
	GETSPEEDTREE();
	//m_lpModelEditor->GetSpeedTreeModifier()->DeleteLeaf(pLeaf);
	pModi->DeleteLeaf(pLeaf);

	if (bUpdateUI)
	{
		OnBnClickedButtonRefresh();
	}
Exit0:
	return;
}

void KSceneModelEditorSFXSpeedTreeDialog::OnDeleteLeafOnNode(HTREEITEM hNode)
{
	HTREEITEM hItemChild = 0;
	int nReturn = MessageBox("将会删除所有连接到本节点上的叶子，是否继续?", "提示", MB_YESNO);
	KG_PROCESS_SUCCESS(nReturn == IDNO);

	hItemChild = m_Tree.GetChildItem(hNode);
	while (hItemChild)
	{
		OnDeleteLeaf(hItemChild, FALSE);
		hItemChild = m_Tree.GetNextSiblingItem(hItemChild);
	}

	OnBnClickedButtonRefresh();
Exit1:
	return;
}

void KSceneModelEditorSFXSpeedTreeDialog::OnBnClickedButtonDelete()
{
	int nTypeInfo = 0;
	int nTemp = 0;
	HTREEITEM hSelected = 0;
	KG_PROCESS_ERROR(m_lpSpeedTree);

    IEKG3DModel *pModelTree = NULL;
    m_lpSpeedTree->GetModelTree(&pModelTree);

	KG_PROCESS_ERROR(pModelTree);

	hSelected = m_Tree.GetSelectedItem();
	if (hSelected)
	{
		m_Tree.GetItemImage(hSelected, nTypeInfo, nTemp);

		switch(nTypeInfo)
		{
		case ENU_BONE_NODE:
			OnDeleteLeafOnNode(hSelected);
			break;
		case ENU_LEAF_NODE:
			OnDeleteLeaf(hSelected, TRUE);
			break;
		}
	}
	else
	{
		BOOL bUpdateUI = FALSE;
		KG_PROCESS_ERROR(m_lpModelEditor);	
		for (size_t i = 0; i < m_vecBoneNodes.size(); i++)
		{
			if (m_vecBoneNodes[i].hItem)
			{
				HTREEITEM hChild = m_Tree.GetChildItem(m_vecBoneNodes[i].hItem);
				while (hChild)
				{
					if (m_Tree.GetCheck(hChild))
					{
						GETSPEEDTREE();
						//m_lpModelEditor->GetSpeedTreeModifier()->DeleteLeaf(reinterpret_cast<IELeaf*>(m_Tree.GetItemData(hChild)));
						pModi->DeleteLeaf(reinterpret_cast<IELeaf*>(m_Tree.GetItemData(hChild)));
						bUpdateUI = TRUE;
					}
					hChild = m_Tree.GetNextSiblingItem(hChild);
				}
			}
		}
		if (bUpdateUI)
		{
			OnBnClickedButtonRefresh();
		}
	}
Exit0:
	return;
}

void KSceneModelEditorSFXSpeedTreeDialog::OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	HTREEITEM hSelected = 0;
	int nImage = 0;
	KG_PROCESS_ERROR(m_lpSpeedTree);

    IEKG3DModel *pModelTree = NULL;
    m_lpSpeedTree->GetModelTree(&pModelTree);

	KG_PROCESS_ERROR(pModelTree);

	hSelected = m_Tree.GetSelectedItem();
	KG_PROCESS_ERROR(hSelected);
	
	m_Tree.GetItemImage(hSelected, nImage, nImage);
	if(nImage == ENU_BONE_NODE)
	{
		BOOL bCheck = m_Tree.GetCheck(hSelected);

		HTREEITEM hLeaf = m_Tree.GetChildItem(hSelected);
		while(hLeaf)
		{
			m_Tree.SetCheck(hLeaf,bCheck);
			hLeaf = m_Tree.GetNextItem(hLeaf, TVGN_NEXT);
		}
		m_Tree.SetCheck(hSelected,bCheck);
	}
Exit0:
	return;
}


void KSceneModelEditorSFXSpeedTreeDialog::OnBnClickedCheckDrawbone()
{
    BOOL bNeed = FALSE;
	if(!m_lpSpeedTree)
		return;
    
	bNeed = IsDlgButtonChecked(IDC_CHECK_DRAWBONE);
    m_lpSpeedTree->SetNeedDrawBone(bNeed);

}

void KSceneModelEditorSFXSpeedTreeDialog::OnBnClickedCheckShowLeaf()
{
	KG_PROCESS_ERROR(m_lpModelEditor);
	KG_PROCESS_ERROR(m_lpModelEditor);
	
	BOOL bShowLeaf = IsDlgButtonChecked(IDC_CHECK_SHOWLEAF);
    IEKG3DSpeedTreeModifier *piSpeedTreeModifier = NULL;

	m_lpModelEditor->GetSpeedTreeModifier(&piSpeedTreeModifier);
    piSpeedTreeModifier->SetAllLeafRenderFlag(bShowLeaf);
	piSpeedTreeModifier->SetAllLeafSelectableFlag(bShowLeaf);

	if (bShowLeaf && m_lpModelEditor->GetIsolateMode())
	{
		m_lpModelEditor->RestoreIsolated();
	}
Exit0:
	return;
}
void KSceneModelEditorSFXSpeedTreeDialog::DeleteAllLeaf()
{
	GETSPEEDTREE();
	KG_PROCESS_ERROR(m_lpModelEditor);
	//m_lpModelEditor->GetSpeedTreeModifier()->UnSelectAll();
	pModi->UnSelectAll();

	m_lpSpeedTree->SetEditMode(FALSE, m_lpModelEditor);
	m_Tree.DeleteAllItems();
Exit0:
	return;
}

void KSceneModelEditorSFXSpeedTreeDialog::OnBnClickedButtonAuto()
{
	if (MessageBox("执行本操作将会清除所有已存在的叶片, 确定要继续吗?", "提示", MB_YESNO) == IDNO)
		return;

	if(!m_lpSpeedTree)
		return;
	
	DeleteAllLeaf();
	UpdateData();
    
    D3DXVECTOR3 Scale = D3DXVECTOR3(m_fLeafRadius, 1.0f, 1.0f);
	m_lpSpeedTree->CreateAuto(m_nLeafAuto, &Scale);
	
	if (m_lpModelEditor)
	{
		m_lpSpeedTree->SetEditMode(TRUE, m_lpModelEditor);
	}
	OnBnClickedButtonRefresh();
}

void KSceneModelEditorSFXSpeedTreeDialog::OnBnClickedButtonRefresh()
{
	HRESULT hResult = E_FAIL;
    //IEKG3DMeshBase *pMeshBase = NULL;
	IEKG3DMesh* pTreeMesh = NULL;
    int nNumBone = 0;
    DWORD dwNumBone = 0;
	TCHAR strNodeName[MAX_PATH];
	HTREEITEM hItem = 0;
	int nTypeInfo = 0;
	BOOL bEnableAddLeafButton = FALSE;
    DWORD dwLeafSize = 0;
    IELeaf *pLeaf = NULL;

	m_Tree.DeleteAllItems();
	std::vector<BoneNodeInfo> OrgBoneNodeInfo;
	OrgBoneNodeInfo = m_vecBoneNodes;
	m_vecBoneNodes.clear();
	
	KG_PROCESS_ERROR(m_lpSpeedTree);

    IEKG3DModel *pModelTree = NULL;
    m_lpSpeedTree->GetModelTree(&pModelTree);
	
    KG_PROCESS_ERROR(pModelTree);

	hResult = pModelTree->GetIEKG3DMesh(&pTreeMesh);
	KGLOG_COM_PROCESS_ERROR(hResult);
    
    pTreeMesh->GetNumBones(&nNumBone);
    dwNumBone = (DWORD)nNumBone;

	if (!OrgBoneNodeInfo.size())
	{
		OrgBoneNodeInfo.resize(dwNumBone);
	}

	for(DWORD i = 0; i < dwNumBone; i++)
	{
		LPCTSTR pszName = NULL;
        pTreeMesh->GetBoneInfoName(i, &pszName);
        HTREEITEM hBone = m_Tree.InsertItem(pszName,
			ENU_BONE_NODE,
			ENU_BONE_NODE,
			TVI_ROOT,
			TVI_LAST);
		m_Tree.SetItemData(hBone, i);
		BoneNodeInfo Info(hBone, OrgBoneNodeInfo[i].bChecked, OrgBoneNodeInfo[i].bExpanded);
		m_vecBoneNodes.push_back(Info);
	}
	
    m_lpSpeedTree->GetLeafVectorSize(&dwLeafSize);

	for (DWORD i = 0; i < dwLeafSize; ++i)
    {
        int nBoneID = 0;
        BOOL bSelected = FALSE;
        
        m_lpSpeedTree->GetLeafVectorItem(i, &pLeaf);

        pLeaf->GetBoneID(&nBoneID);

		if((nBoneID >= 0) && (nBoneID < static_cast<int>(m_vecBoneNodes.size())))
		{
			HTREEITEM hBone = m_vecBoneNodes[nBoneID].hItem;
			sprintf_s(strNodeName, MAX_PATH, "Leaf%-3u", i);

			HTREEITEM hLeaf = m_Tree.InsertItem(strNodeName, 
				ENU_LEAF_NODE, 
				ENU_LEAF_NODE, 
				hBone, 
				TVI_LAST);
			m_Tree.SetItemData(hLeaf,(DWORD_PTR)pLeaf);
            pLeaf->GetSelectState(&bSelected);
			m_Tree.SetCheck(hLeaf, bSelected);
		}
	}

	for (size_t i = 0; i < m_vecBoneNodes.size(); i++)
	{
		if (m_vecBoneNodes[i].bExpanded)
		{
			m_Tree.Expand(m_vecBoneNodes[i].hItem, TVE_EXPAND);
		}
	}

	hItem = m_Tree.GetSelectedItem();
	if (hItem)
	{
		m_Tree.GetItemImage(hItem, nTypeInfo, nTypeInfo);
		if (nTypeInfo == ENU_BONE_NODE)
		{
			bEnableAddLeafButton = TRUE;
		}
	}
Exit0:
	GetDlgItem(IDC_BUTTON_ADDLEAF)->EnableWindow(bEnableAddLeafButton);
	return;
}



void KSceneModelEditorSFXSpeedTreeDialog::OnBnClickedButtonLoadmtl()
{
	TCHAR strDefaultSpdTreeFilePath[MAX_PATH];
	CFileDialog dlgOpen(TRUE, 
		"",
		0, 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
		"SpeedTree(*.STree)|*.STree||");
	sprintf_s(strDefaultSpdTreeFilePath, 
		MAX_PATH,
		"%s%s",
		g_szDefWorkDirectory,
		s_strDefaultSpdTreeFilePath);
	dlgOpen.m_ofn.lpstrInitialDir = strDefaultSpdTreeFilePath;
	int nReturn = MessageBox("读取树叶将会清除掉当前已有的所有树叶, 确定要继续吗?", "提示", MB_YESNO);

	KG_PROCESS_SUCCESS(nReturn == IDNO);

	KG_PROCESS_ERROR(m_lpSpeedTree);
	
	KG_PROCESS_ERROR(dlgOpen.DoModal() == IDOK);

	m_lpSpeedTree->SetEditMode(FALSE, m_lpModelEditor);
	m_lpSpeedTree->LoadLeafFromFile(dlgOpen.GetPathName());
	OnBnClickedButtonRefresh();
	m_lpSpeedTree->SetEditMode(TRUE, m_lpModelEditor);
	static_cast<CButton*>(GetDlgItem(IDC_CHECK_SHOWLEAF))->SetCheck(BST_CHECKED);
    static_cast<CButton*>(GetDlgItem(IDC_CHECK_ISOLATE))->SetCheck(BST_UNCHECKED);
    static_cast<CButton*>(GetDlgItem(IDC_RADIO4))->SetCheck(BST_UNCHECKED);
    static_cast<CButton*>(GetDlgItem(IDC_RADIO5))->SetCheck(BST_UNCHECKED);
    static_cast<CButton*>(GetDlgItem(IDC_RADIO6))->SetCheck(BST_UNCHECKED);
    static_cast<CButton*>(GetDlgItem(IDC_RADIO10))->SetCheck(BST_UNCHECKED);
Exit1:
Exit0:
	return;
}


void KSceneModelEditorSFXSpeedTreeDialog::OnBnClickedButtonChangeleaftex()
{
	HRESULT hr = E_FAIL;
	TCHAR strDefaultLeafTexturePath[MAX_PATH];
	TCHAR strOutputInfo[MAX_PATH * 2];
	TCHAR strFilter[] = "Texture Files|*.bmp;*.jpg;*.png;*.tga;*.Spr;*.DDS;*.avi;*.rmvb;*.wmv|";
	CFileDialog dlgOpen(TRUE, NULL, s_strDefaultLeafTexturePrefix, OFN_HIDEREADONLY , strFilter);
	sprintf_s(strDefaultLeafTexturePath,
		MAX_PATH,
		"%s%s",
		g_szDefWorkDirectory,
		s_strDefaultLeafTexturStartPath);
	dlgOpen.m_ofn.lpstrInitialDir = strDefaultLeafTexturePath;
	KG_PROCESS_SUCCESS(dlgOpen.DoModal() != IDOK);
	KG_PROCESS_ERROR(m_lpSpeedTree);
	
	hr = m_lpSpeedTree->ChangeLeafTexture(dlgOpen.GetPathName());
	
Exit1:
	return;
Exit0:
	sprintf_s(strOutputInfo, MAX_PATH * 2, "打开贴图文件%s失败.", dlgOpen.GetPathName());
	MessageBox(strOutputInfo, "错误");
	KGLogPrintf(KGLOG_ERR, strOutputInfo);
}

void KSceneModelEditorSFXSpeedTreeDialog::OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nTypeInfo = 0;
	int nTemp = 0;
	CPoint ptMouse;
	GetCursorPos(&ptMouse);
	m_Tree.ScreenToClient(&ptMouse);

	HTREEITEM hItem = m_Tree.HitTest(ptMouse);
	
	KG_PROCESS_SUCCESS(!hItem);
	
	m_Tree.GetItemImage(hItem, nTypeInfo, nTemp);
	GetDlgItem(IDC_BUTTON_ADDLEAF)->EnableWindow(nTypeInfo == ENU_BONE_NODE);
	
	BOOL bChecked = m_Tree.GetCheck(hItem);
	switch(nTypeInfo)
	{
	case ENU_LEAF_NODE:
		{
			IELeaf* pLeaf = reinterpret_cast<IELeaf *>(m_Tree.GetItemData(hItem));
			OnSelectLeaf(pLeaf, !bChecked);
		}
		break;
	case ENU_BONE_NODE:
		{
			BoneNodeInfo* pNodeInfo = FindNode(hItem);
			if (IsCheckBoxClicked(hItem, ptMouse))
			{
				bChecked = !bChecked;
				HTREEITEM hChildItem = m_Tree.GetChildItem(hItem);
				while (hChildItem)
				{
					IELeaf* pLeaf = reinterpret_cast<IELeaf*>(m_Tree.GetItemData(hChildItem));
					m_Tree.SetCheck(hChildItem, bChecked);
					OnSelectLeaf(pLeaf, bChecked);
					hChildItem = m_Tree.GetNextSiblingItem(hChildItem);
				}
			}

			//(by dengzhihui Refactor 2006年11月24日
// 			KG3DSceneEntity Entity = m_lpModelEditor->GetSpeedTreeModifier()->ConvertToEntity();
// 			if (bChecked)
// 			{
// 				m_lpModelEditor->AddSelectedEntity(Entity);
// 			}
// 			else
// 			{
// 				m_lpModelEditor->RemoveSelectedEntity(Entity);
// 			}
			m_lpModelEditor->ProcessEntity(bChecked);
			//)
		}
		break;
	}
	m_Tree.SelectItem(hItem);
	*pResult = 0;
Exit1:
	return;
}


void KSceneModelEditorSFXSpeedTreeDialog::OnTvnItemexpandedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	int nTypeInfo = 0;
	if (!(pNMTreeView->itemNew.hItem))
		return;

	m_Tree.GetItemImage(pNMTreeView->itemNew.hItem, nTypeInfo, nTypeInfo);
	
	if (nTypeInfo == ENU_BONE_NODE)
	{
		BoneNodeInfo* pBoneNode = FindNode(pNMTreeView->itemNew.hItem);
		if (pBoneNode)
		{
			pBoneNode->bExpanded = (pNMTreeView->action == 1) ? FALSE : TRUE;
		}
	}

	m_Tree.SelectItem(pNMTreeView->itemNew.hItem);
	*pResult = 0;
}

void KSceneModelEditorSFXSpeedTreeDialog::OnSelectLeaf(IELeaf* pLeaf,
													   BOOL bSelected)
{
	GETSPEEDTREE();
	KG_PROCESS_ERROR(m_lpModelEditor);
	
	if (bSelected)
	{
		//m_lpModelEditor->GetSpeedTreeModifier()->SelectLeaf(pLeaf);
		pModi->SelectLeaf(pLeaf);
	}
	else
	{
		//m_lpModelEditor->GetSpeedTreeModifier()->UnSelectLeaf(pLeaf);
		pModi->UnSelectLeaf(pLeaf);
	}
Exit0:
	return;
}


void KSceneModelEditorSFXSpeedTreeDialog::OnOK()
{
	KG_PROCESS_ERROR(m_lpSpeedTree);

	UpdateData(TRUE);
	SetLeafSize(m_fSize);
	SetLeafWave(m_nWave * 1000.0f);
	m_lpSpeedTree->SetPitchLimit(m_nPitch / 1000.0f);

	m_lpSpeedTree->SetRollLeft(-m_nRoll / 180.0f * PI);
	m_lpSpeedTree->SetRollRight(m_nRoll / 180.0f * PI);
	m_lpSpeedTree->SetMaxRollSpeed(m_nMaxSpeed / 10000.0f);
	m_lpSpeedTree->SetMaxPitchSpeed(m_nMaxSpeed / 10000.0f);
	m_lpSpeedTree->SetRollAccelerate(m_fAccelerate / 10000000.0f);
	m_lpSpeedTree->SetPitchAccelerate(m_fAccelerate / 10000000.0f);

	m_lpSpeedTree->FillLeafVerticesBuffer();
Exit0:
	return;
}



void KSceneModelEditorSFXSpeedTreeDialog::OnUpdateUI(IEKG3DModelSpeedTree* pSpdTree, void* pDialog)
{
    HRESULT hRetCode = E_FAIL;
	TCHAR strNodeName[MAX_PATH];
	//IEKG3DMeshBase *pMeshBase = NULL;
    IEKG3DMesh* pTreeMesh = NULL;
    int nBones = 0;
    DWORD dwNumBone = 0;
	std::vector<BoneNodeInfo> OrgNodeInfo;
    IELeaf *pLeaf = NULL;
    DWORD dwSize = 0;
    int nBoneID = 0;
    BOOL bSelected = FALSE;

    KSceneModelEditorSFXSpeedTreeDialog* pDlg = static_cast<KSceneModelEditorSFXSpeedTreeDialog*>(pDialog);
	
	KG_PROCESS_ERROR(pSpdTree);
	KG_PROCESS_ERROR(pDlg);

    IEKG3DModel *pModelTree = NULL;
    pSpdTree->GetModelTree(&pModelTree);

	KG_PROCESS_ERROR(pModelTree);

	pDlg->m_Tree.DeleteAllItems();
	OrgNodeInfo = pDlg->m_vecBoneNodes;
	pDlg->m_vecBoneNodes.clear();
    

    hRetCode = pModelTree->GetIEKG3DMesh(&pTreeMesh);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = pTreeMesh->GetNumBones(&nBones);
    KGLOG_COM_PROCESS_ERROR(hRetCode);
    
    dwNumBone = nBones;

	if (!OrgNodeInfo.size())
	{
		OrgNodeInfo.resize(dwNumBone);
	}

	

	for(DWORD i = 0; i < dwNumBone; i++)
	{
		LPCTSTR pszName = NULL;

        hRetCode = pTreeMesh->GetBoneInfoName(i, &pszName);
        KGLOG_COM_PROCESS_ERROR(hRetCode);
        
        HTREEITEM hBone = pDlg->m_Tree.InsertItem(pszName,
			ENU_BONE_NODE,
			ENU_BONE_NODE,
			TVI_ROOT,
			TVI_LAST);
		pDlg->m_Tree.SetItemData(hBone, i);
		BoneNodeInfo Info(hBone, OrgNodeInfo[i].bChecked, OrgNodeInfo[i].bExpanded);
		pDlg->m_vecBoneNodes.push_back(Info);
	}
	
    pSpdTree->GetLeafVectorSize(&dwSize);
	for (DWORD i = 0; i < dwSize; ++i)
	{
        pSpdTree->GetLeafVectorItem(i, &pLeaf);

        pLeaf->GetBoneID(&nBoneID);

		if((nBoneID >= 0) && (nBoneID < static_cast<int>(pDlg->m_vecBoneNodes.size())))
		{
			HTREEITEM hBone = pDlg->m_vecBoneNodes[nBoneID].hItem;
			sprintf_s(strNodeName, MAX_PATH, "Leaf%-3d", i);
			HTREEITEM hLeaf = pDlg->m_Tree.InsertItem(strNodeName, 
				ENU_LEAF_NODE, 
				ENU_LEAF_NODE, 
				hBone, 
				TVI_LAST);
			pDlg->m_Tree.SetItemData(hLeaf, (DWORD_PTR)pLeaf);
            pLeaf->GetSelectState(&bSelected);
			pDlg->m_Tree.SetCheck(hLeaf, bSelected);
		}
	}

	for (size_t i = 0; i < pDlg->m_vecBoneNodes.size(); i++)
	{
		if (pDlg->m_vecBoneNodes[i].bExpanded)
		{
			pDlg->m_Tree.Expand(pDlg->m_vecBoneNodes[i].hItem, TVE_EXPAND);
		}
	}
Exit0:
	return;
}

KSceneModelEditorSFXSpeedTreeDialog::BoneNodeInfo* KSceneModelEditorSFXSpeedTreeDialog::FindNode(HTREEITEM hItem)
{
	for (size_t i = 0; i < m_vecBoneNodes.size(); i++)
	{
		if (m_vecBoneNodes[i].hItem == hItem)
			return &m_vecBoneNodes[i];
	}
	return NULL;
}

KSceneModelEditorSFXSpeedTreeDialog::BoneNodeInfo::BoneNodeInfo()
{
	hItem = 0;
	bChecked = FALSE;
	bExpanded = FALSE;
}

KSceneModelEditorSFXSpeedTreeDialog::BoneNodeInfo::BoneNodeInfo(HTREEITEM hInitItem, 
																BOOL bInitChecked, 
																BOOL bInitExpanded): hItem(hInitItem),
																bChecked(bInitChecked),
																bExpanded(bInitExpanded)
{
}

BOOL KSceneModelEditorSFXSpeedTreeDialog::IsCheckBoxClicked(HTREEITEM hItem, 
															CPoint& ptMouse)
{
	RECT rectItem;
	RECT rectCheckBox;
	m_Tree.GetItemRect(hItem, &rectItem, TRUE);
	rectCheckBox.bottom = rectItem.bottom;
	rectCheckBox.top = rectItem.top;
	rectCheckBox.right = rectItem.left;
	rectCheckBox.left = rectCheckBox.right - CHECKBOX_WIDTH;
	return PtInRect(&rectCheckBox, ptMouse);
}

void KSceneModelEditorSFXSpeedTreeDialog::OnBnClickedButtonSave()
{
	HRESULT hr = E_FAIL;
	LPCTSTR pszFullName = NULL;
	TCHAR strCurrentPathSave[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, strCurrentPathSave);
	::SetCurrentDirectory(g_szDefWorkDirectory);
	
	KG_PROCESS_ERROR(m_lpSpeedTree);

	CFrameWnd* pMainFrame = static_cast<CFrameWnd*>(AfxGetMainWnd());
    KSceneModelEditorFrameWnd* pWnd = static_cast<KSceneModelEditorFrameWnd*>(pMainFrame->GetActiveFrame());
    if (pWnd)
    {
        CDocument *pDoc = pWnd->GetActiveDocument();
        KG_PROCESS_SUCCESS(!pDoc);
        hr = m_lpSpeedTree->SaveToFile(pDoc->GetPathName(), 0);
    }
    
	if (FAILED(hr))
	{
		MessageBox("保存失败.", "错误");
	}
	else
	{
		MessageBox("保存成功", "成功");
	}
Exit1:
Exit0:
	::SetCurrentDirectory(strCurrentPathSave);
	return;
}

void KSceneModelEditorSFXSpeedTreeDialog::OnBnClickedButtonLoad()
{
	HRESULT hr = E_FAIL;
	IEKG3DModel *pModel = NULL;
	IEKG3DModel *pTrunck = NULL;
    IEKG3DModelTable *pTable = NULL;
    IEKG3DModelSpeedTree *pModelSpeedTree = NULL;
    IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
	TCHAR strOutputInfo[MAX_PATH];
	TCHAR strDefaultSpdTreeFilePath[MAX_PATH];
	sprintf_s(strDefaultSpdTreeFilePath, 
		MAX_PATH,
		"%s%s",
		g_szDefWorkDirectory,
		s_strDefaultSpdTreeFilePath);
	 CFileDialog dlgOpen(TRUE, 
		"STree", 
		0, 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
		"SpeedTree文件(*.STree)|*.STree||");
	 dlgOpen.m_ofn.lpstrInitialDir = strDefaultSpdTreeFilePath;
	 KG_PROCESS_SUCCESS(dlgOpen.DoModal() != IDOK);
    KG_PROCESS_ERROR(m_lpModelEditor);

    if (m_lpSpeedTree)
    {
        m_lpSpeedTree->SetEditMode(FALSE, m_lpModelEditor);
    }

    pManager->GetIEKG3DModelTable(&pTable);
	hr = pTable->IELoadModelFromFile(dlgOpen.GetPathName(), 0, 0, &pModel);
	KG_COM_PROCESS_ERROR(hr);

	m_lpModelEditor->OpenModel(pModel, FALSE);
	m_lpSpeedTree = dynamic_cast<IEKG3DModelSpeedTree*>(pModel);
	KG_PROCESS_ERROR(m_lpSpeedTree);

    m_lpSpeedTree->SetEditMode(TRUE, m_lpModelEditor);
	m_lpSpeedTree->GetModelTree(&pTrunck);
	KG_PROCESS_ERROR(pTrunck);

	m_vecBoneNodes.clear();
	OnBnClickedButtonRefresh();

	CFrameWnd* pMainFrame = static_cast<CFrameWnd*>(AfxGetMainWnd());
	KSceneModelEditorFrameWnd* pWnd = static_cast<KSceneModelEditorFrameWnd*>(pMainFrame->GetActiveFrame());
	if (pWnd)
	{
		pWnd->GetMeshPanel()->SetCurModel(pTrunck, pTrunck);
		pWnd->GetMaterialPanel()->SetCurModel(pTrunck);
        CDocument *pDoc = pWnd->GetActiveDocument();
        KG_PROCESS_SUCCESS(!pDoc);
        pDoc->SetPathName(dlgOpen.GetPathName(), FALSE);
	}
Exit0:
	if (FAILED(hr))
	{
		sprintf_s(strOutputInfo, 
			MAX_PATH,
			"打开SpeedTree %s失败.",
			dlgOpen.GetPathName());
		MessageBox(strOutputInfo, "Error");
	}
	SAFE_RELEASE(pModel);
Exit1:
	return;
}

void KSceneModelEditorSFXSpeedTreeDialog::OnBnClickedCheckIsolate()
{
	KG_PROCESS_ERROR(m_lpModelEditor);
	m_lpModelEditor->IsolateCurrentSelection();
Exit0:
	return;
}
