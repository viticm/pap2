// KSceneEditorWallSetupDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneEditorWallSetupDialog.h"
#include "IEKG3DSceneObjectPlacementTool.h"

// KSceneEditorWallSetupDialog 对话框

IMPLEMENT_DYNAMIC(KSceneEditorWallSetupDialog, CDialog)

KSceneEditorWallSetupDialog::KSceneEditorWallSetupDialog(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneEditorWallSetupDialog::IDD, pParent)
	, m_szModelName(_T(""))
	, m_fAx(0)
	, m_fAy(0)
	, m_fAz(0)
	, m_fBx(0)
	, m_fBy(0)
	, m_fBz(0)
{
	m_pWallDefine = NULL;
	m_lpSceneEditor = NULL;
	m_lpWall = NULL;
	m_nOutputWindowID = -1;
	m_nCurrentSelectedElement = -1;
	m_nCurrentSelectedWall = -1;
}

KSceneEditorWallSetupDialog::~KSceneEditorWallSetupDialog()
{
	SAFE_RELEASE(m_pWallDefine);
	if(m_nOutputWindowID!=-1 && m_lpSceneEditor)
	{
		m_lpSceneEditor->RemoveOutputWindow(m_nOutputWindowID);
		m_nOutputWindowID = -1;
	}
}

HRESULT KSceneEditorWallSetupDialog::SetWallDefine(IEKG3DWallDefine* pDefine,IEKG3DSceneSceneEditor* pEditor,IEKG3DWall* pWall)
{
	SAFE_RELEASE(m_pWallDefine);
	m_pWallDefine = pDefine;
	m_lpSceneEditor = pEditor;
	m_lpWall = pWall;
	return S_OK;
}


void KSceneEditorWallSetupDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_ListDefine);
	DDX_Control(pDX, IDC_COMBO_ELETYPE, m_Combo_ElementType);
	DDX_Text(pDX, IDC_EDIT_MODELFILE, m_szModelName);
	DDX_Text(pDX, IDC_EDIT_A_X, m_fAx);
	DDX_Text(pDX, IDC_EDIT_A_Y, m_fAy);
	DDX_Text(pDX, IDC_EDIT_A_Z, m_fAz);
	DDX_Text(pDX, IDC_EDIT_B_X, m_fBx);
	DDX_Text(pDX, IDC_EDIT_B_Y, m_fBy);
	DDX_Text(pDX, IDC_EDIT_B_Z, m_fBz);
	DDX_Control(pDX, IDC_BUTTON_PREVIEW, m_Button_Preview);
	DDX_Control(pDX, IDC_LIST_WALL, m_listWall);
}


BEGIN_MESSAGE_MAP(KSceneEditorWallSetupDialog, CDialog)
	ON_WM_SHOWWINDOW()
	ON_EN_CHANGE(IDC_EDIT_MODELFILE, &KSceneEditorWallSetupDialog::OnEnChangeEditModelfile)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST2, &KSceneEditorWallSetupDialog::OnLvnItemchangedList2)
	ON_BN_CLICKED(IDC_BUTTON_ADD_ELE, &KSceneEditorWallSetupDialog::OnBnClickedButtonAddEle)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_ELE, &KSceneEditorWallSetupDialog::OnBnClickedButtonDeleteEle)
	ON_BN_CLICKED(IDC_BUTTON_LOADELE, &KSceneEditorWallSetupDialog::OnBnClickedButtonLoadele)
	ON_BN_CLICKED(IDC_BUTTON_SAVEELE, &KSceneEditorWallSetupDialog::OnBnClickedButtonSaveele)
	ON_BN_CLICKED(IDC_BUTTON_BROWER, &KSceneEditorWallSetupDialog::OnBnClickedButtonBrower)
	ON_BN_CLICKED(IDC_BUTTON_PREVIEW, &KSceneEditorWallSetupDialog::OnBnClickedButtonPreview)
	ON_CBN_SELCHANGE(IDC_COMBO_ELETYPE, &KSceneEditorWallSetupDialog::OnCbnSelchangeComboEletype)
	ON_EN_CHANGE(IDC_EDIT_A_X, &KSceneEditorWallSetupDialog::OnEnChangeEditAX)
	ON_EN_CHANGE(IDC_EDIT_A_Y, &KSceneEditorWallSetupDialog::OnEnChangeEditAY)
	ON_EN_CHANGE(IDC_EDIT_A_Z, &KSceneEditorWallSetupDialog::OnEnChangeEditAZ)
	ON_EN_CHANGE(IDC_EDIT_B_X, &KSceneEditorWallSetupDialog::OnEnChangeEditBX)
	ON_EN_CHANGE(IDC_EDIT_B_Y, &KSceneEditorWallSetupDialog::OnEnChangeEditBY)
	ON_EN_CHANGE(IDC_EDIT_B_Z, &KSceneEditorWallSetupDialog::OnEnChangeEditBZ)
	ON_LBN_SELCHANGE(IDC_LIST_KEYPOINT, &KSceneEditorWallSetupDialog::OnLbnSelchangeListKeypoint)
	ON_LBN_SELCHANGE(IDC_LIST_WALL, &KSceneEditorWallSetupDialog::OnLbnSelchangeListWall)
	ON_BN_CLICKED(IDC_BUTTON_ADDWALL, &KSceneEditorWallSetupDialog::OnBnClickedButtonAddwall)
	ON_BN_CLICKED(IDC_BUTTON_DELETEWALL, &KSceneEditorWallSetupDialog::OnBnClickedButtonDeletewall)
	ON_BN_CLICKED(IDC_BUTTON_ADDPOINT, &KSceneEditorWallSetupDialog::OnBnClickedButtonAddpoint)
END_MESSAGE_MAP()


// KSceneEditorWallSetupDialog 消息处理程序

void KSceneEditorWallSetupDialog::OnEnChangeEditModelfile()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}

void KSceneEditorWallSetupDialog::OnLvnItemchangedList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	POSITION pos = m_ListDefine.GetFirstSelectedItemPosition();
	if(pos)
	{
		int nIndex = m_ListDefine.GetNextSelectedItem(pos);
		m_nCurrentSelectedElement = _kg_to_smaller_size_cast<int>(m_ListDefine.GetItemData(nIndex));
		UpdateElementInfo(TRUE,FALSE);
	}
	else
	{
		m_nCurrentSelectedElement = -1;
	}
}

IEKG3DSceneObjectPlacementTool* KSceneEditorWallSetupDialog::GetObjectPlacementToolInterface(IEKG3DSceneSceneEditor* pEditor)
{
	IEKG3DSceneObjectPlacementTool* pRet = NULL;
	KG_PROCESS_ERROR(NULL != pEditor);
	{
		HRESULT hr = pEditor->GetEngineIEInterface(KG3DTYPE_SCENE_OBJECT_PLACEMENT_TOOL, 0, 0, (VOID**)&pRet);
		KG_COM_PROCESS_ERROR(hr);
		return pRet;
	}

Exit0:
	//SAFE_RELEASE(pRet);
	//_ASSERTE(! m_lpObjPlacementTool);
	return NULL;
}

void KSceneEditorWallSetupDialog::UpdateElementInfo(BOOL bToUI,BOOL bComputeFromMesh)
{
	if(!m_pWallDefine)
		return;
	IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
	IEKG3DSceneObjectPlacementTool* lpObjPlacementTool = GetObjectPlacementToolInterface(m_lpSceneEditor);

	if(m_nCurrentSelectedElement!=-1)
	{
		KG3DWALLELEMENTTYPE eType;
		D3DXVECTOR3 A,B;
		TCHAR szModelName[MAX_PATH];

		if(SUCCEEDED(m_pWallDefine->GetElement(m_nCurrentSelectedElement,&eType,&A,&B,szModelName)))
		{
			if(bToUI)
			{
				map<KG3DWALLELEMENTTYPE,int>::iterator iFind = m_mapComboIndex.find(eType);
				if(iFind != m_mapComboIndex.end())
				{
					int Index = iFind->second;
					m_Combo_ElementType.SetCurSel(Index);
				}

				m_fAx = A.x;
				m_fAy = A.y;
				m_fAz = A.z;
				m_fBx = B.x;
				m_fBy = B.y;
				m_fBz = B.z;

				m_szModelName.Format("%s",szModelName);

				UpdateData(FALSE);

				if(_strcmpi(szModelName,"")!=0)
				{
					IEKG3DRepresentObject* pObj = NULL;
					HRESULT hr = pManager->BuildKG3DRepresentObject(szModelName, REPRESENTOBJECT_DEFAULT, 0, 0, &pObj);
					KG_COM_PROCESS_ERROR(hr);
					hr = lpObjPlacementTool->SetObjectInHandle(pObj, TRUE);
					KG_COM_PROCESS_ERROR(hr);
					m_lpSceneEditor->SetWallConnectionPoint(TRUE,A,B);
				}
			}
			else
			{
				UpdateData(TRUE);

				int nIndex = m_Combo_ElementType.GetCurSel();
				KG3DWALLELEMENTTYPE eTypeNew = (KG3DWALLELEMENTTYPE)m_Combo_ElementType.GetItemData(nIndex);

				A.x = m_fAx;
				A.y = m_fAy;
				A.z = m_fAz;
				B.x = m_fBx;
				B.y = m_fBy;
				B.z = m_fBz;

				wsprintf(szModelName,"%s",m_szModelName);

				m_pWallDefine->SetElement(m_nCurrentSelectedElement,eTypeNew,A,B,szModelName,bComputeFromMesh);

				m_lpSceneEditor->SetWallConnectionPoint(TRUE,A,B);
			}
		}
	}
Exit0:
	return ;

}

void KSceneEditorWallSetupDialog::OnBnClickedButtonAddEle()
{
	if(!m_pWallDefine)
		return;
	int nNewIndex = 0;
	m_pWallDefine->NewOneElement(&nNewIndex);

	FillElementList();
}

void KSceneEditorWallSetupDialog::OnBnClickedButtonDeleteEle()
{
	if(!m_pWallDefine)
		return;

	if(m_nCurrentSelectedElement!=-1)
	{
		m_pWallDefine->DeleteElement(m_nCurrentSelectedElement);
		m_nCurrentSelectedElement = -1;
	}
	FillElementList();
}

void KSceneEditorWallSetupDialog::OnBnClickedButtonLoadele()
{
	if(!m_pWallDefine)
		return;

	CFileDialog dlg(true, "WallDefine", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "剑网3D模型文件(*.WallDefine)|*.WallDefine||");
	if(dlg.DoModal() == IDOK)
	{
		TCHAR Name[256];
		wsprintf(Name,"%s",dlg.GetPathName());

		::SetCurrentDirectory(g_szDefWorkDirectory);
		TCHAR RetPath[MAX_PATH];
		g_pEngineManager->GetRelativePath(g_szDefWorkDirectory,Name,RetPath);

		m_pWallDefine->LoadFromFile(RetPath);
	}

	m_nCurrentSelectedElement = -1;
	FillElementList();

}

void KSceneEditorWallSetupDialog::OnBnClickedButtonSaveele()
{
	if(!m_pWallDefine)
		return;

	CFileDialog dlg(false, "WallDefine", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "剑网3D模型文件(*.WallDefine)|*.WallDefine||");
	if(dlg.DoModal() == IDOK)
	{
		TCHAR Name[256];
		wsprintf(Name,"%s",dlg.GetPathName());

		::SetCurrentDirectory(g_szDefWorkDirectory);
		TCHAR RetPath[MAX_PATH];
		g_pEngineManager->GetRelativePath(g_szDefWorkDirectory,Name,RetPath);

		m_pWallDefine->SaveToFile(RetPath);
	}
}

void KSceneEditorWallSetupDialog::OnBnClickedButtonBrower()
{
	CFileDialog dlg(true, "Mesh", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "剑网3D模型文件(*.mesh)|*.mesh||");
	if(dlg.DoModal() == IDOK)
	{
		TCHAR Name[256];
		wsprintf(Name,"%s",dlg.GetPathName());

		::SetCurrentDirectory(g_szDefWorkDirectory);
		TCHAR RetPath[MAX_PATH];
		g_pEngineManager->GetRelativePath(g_szDefWorkDirectory,Name,RetPath);

		m_szModelName.Format("%s",RetPath);
		UpdateData(FALSE);

		UpdateElementInfo(FALSE,TRUE);
		UpdateElementInfo(TRUE,FALSE);

		FillElementList();
	}
}

void KSceneEditorWallSetupDialog::OnBnClickedButtonPreview()
{
	// TODO: 在此添加控件通知处理程序代码
}

void KSceneEditorWallSetupDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if(m_lpSceneEditor)
	{
		if(bShow && m_nOutputWindowID==-1)
		{
			m_lpSceneEditor->AddOutputWindow("ScenePreView",m_Button_Preview.GetSafeHwnd(),
				OUTPUTWND_SCENEEDITOR_OBJECTPREVEIW,0,&m_nOutputWindowID);
			m_lpSceneEditor->IEOnResizeWindow(NULL);
		}
		else
		{
			m_lpSceneEditor->RemoveOutputWindow(m_nOutputWindowID);
			m_nOutputWindowID = -1;
		}
	}

	FillElementList();

	FillWallList();
}

HRESULT KSceneEditorWallSetupDialog::GetTypeName(LPSTR szName,KG3DWALLELEMENTTYPE eType)
{
	switch(eType)
	{
	case WALLELEMENT_WALL:
		wsprintf(szName,"墙体");
		break;
	case WALLELEMENT_DOOR:
		wsprintf(szName,"门");
		break;
	case WALLELEMENT_STARTEND:
		wsprintf(szName,"起点/终点");
	    break;
	case WALLELEMENT_CORNER_90:
		wsprintf(szName,"90°转角");
		break;
	case WALLELEMENT_CORNER_60:
		wsprintf(szName,"60°转角");
		break;
	case WALLELEMENT_CORNER_ALL:
		wsprintf(szName,"全方位转角");
		break;
	default:
		wsprintf(szName,"未定义元素");
	    break;
	}
	return S_OK;
}

HRESULT KSceneEditorWallSetupDialog::FillElementList()
{
	if (!m_pWallDefine)
	{
		return E_FAIL;
	}

	m_ListDefine.DeleteAllItems();

	int nCount = m_pWallDefine->GetNumElement();
	for (int i=0;i<nCount;i++)
	{

		KG3DWALLELEMENTTYPE eType;
		D3DXVECTOR3 A,B;
		TCHAR szModelName[MAX_PATH];


		m_pWallDefine->GetElement(i,&eType,&A,&B,szModelName);

		TCHAR szName[MAX_PATH];
		LV_ITEM lvItem;
		lvItem.mask = LVIF_TEXT | LVIF_IMAGE;
		lvItem.iItem = i;      
		lvItem.iImage   = 0;
		lvItem.pszText = szName; 

		{
			wsprintf(szName,"%d",i);
			lvItem.iSubItem = 0;
			int index = m_ListDefine.InsertItem( &lvItem );
			m_ListDefine.SetItemData(index,i); 
		}
		{
			GetTypeName(szName,eType);
			lvItem.iSubItem = 1;
			m_ListDefine.SetItem(&lvItem );
		}
		{
			wsprintf(szName,"%s",szModelName);
			lvItem.iSubItem = 2;
			m_ListDefine.SetItem(&lvItem );
		}
	}
	return S_OK;
}

BOOL KSceneEditorWallSetupDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_ListDefine.InsertColumn(0, _T("ID"  ),	    LVCFMT_CENTER, 40);
	m_ListDefine.InsertColumn(1, _T("Type"),	    LVCFMT_CENTER, 90);
	m_ListDefine.InsertColumn(2, _T("FileName" ),	LVCFMT_CENTER, 200);

	m_ListDefine.SetExtendedStyle(
		m_ListDefine.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES
		);

	{
		TCHAR szName[MAX_PATH];
		int nIndex  = 0;
		WALLELEMENT_WALL,
			WALLELEMENT_DOOR,
			WALLELEMENT_STARTEND,
			WALLELEMENT_CORNER_90,
			WALLELEMENT_CORNER_60,
			WALLELEMENT_CORNER_ALL,

		GetTypeName(szName,WALLELEMENT_WALL);
		nIndex = m_Combo_ElementType.AddString(szName);
		m_Combo_ElementType.SetItemData(nIndex,WALLELEMENT_WALL);
		m_mapComboIndex[WALLELEMENT_WALL] = nIndex;

		GetTypeName(szName,WALLELEMENT_DOOR);
		nIndex = m_Combo_ElementType.AddString(szName);
		m_Combo_ElementType.SetItemData(nIndex,WALLELEMENT_DOOR);
		m_mapComboIndex[WALLELEMENT_DOOR] = nIndex;

		GetTypeName(szName,WALLELEMENT_STARTEND);
		nIndex = m_Combo_ElementType.AddString(szName);
		m_Combo_ElementType.SetItemData(nIndex,WALLELEMENT_STARTEND);
		m_mapComboIndex[WALLELEMENT_STARTEND] = nIndex;

		GetTypeName(szName,WALLELEMENT_CORNER_90);
		nIndex = m_Combo_ElementType.AddString(szName);
		m_Combo_ElementType.SetItemData(nIndex,WALLELEMENT_CORNER_90);
		m_mapComboIndex[WALLELEMENT_CORNER_90] = nIndex;

		GetTypeName(szName,WALLELEMENT_CORNER_60);
		nIndex = m_Combo_ElementType.AddString(szName);
		m_Combo_ElementType.SetItemData(nIndex,WALLELEMENT_CORNER_60);
		m_mapComboIndex[WALLELEMENT_CORNER_60] = nIndex;

		GetTypeName(szName,WALLELEMENT_CORNER_ALL);
		nIndex = m_Combo_ElementType.AddString(szName);
		m_Combo_ElementType.SetItemData(nIndex,WALLELEMENT_CORNER_ALL);
		m_mapComboIndex[WALLELEMENT_CORNER_ALL] = nIndex;

	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneEditorWallSetupDialog::OnCbnSelchangeComboEletype()
{
	UpdateElementInfo(FALSE,FALSE);

	FillElementList();

}

void KSceneEditorWallSetupDialog::OnEnChangeEditAX()
{
	UpdateElementInfo(FALSE,FALSE);
}

void KSceneEditorWallSetupDialog::OnEnChangeEditAY()
{
	UpdateElementInfo(FALSE,FALSE);
}

void KSceneEditorWallSetupDialog::OnEnChangeEditAZ()
{
	UpdateElementInfo(FALSE,FALSE);
}

void KSceneEditorWallSetupDialog::OnEnChangeEditBX()
{
	UpdateElementInfo(FALSE,FALSE);
}

void KSceneEditorWallSetupDialog::OnEnChangeEditBY()
{
	UpdateElementInfo(FALSE,FALSE);
}

void KSceneEditorWallSetupDialog::OnEnChangeEditBZ()
{
	UpdateElementInfo(FALSE,FALSE);
}

void KSceneEditorWallSetupDialog::OnLbnSelchangeListKeypoint()
{
	// TODO: 在此添加控件通知处理程序代码
}

void KSceneEditorWallSetupDialog::OnLbnSelchangeListWall()
{
	int nIndex = m_listWall.GetCurSel();
	m_nCurrentSelectedWall =_kg_to_smaller_size_cast<int>(m_listWall.GetItemData(nIndex));
}

void KSceneEditorWallSetupDialog::OnBnClickedButtonAddwall()
{
	if(m_lpWall)
	{
		int nIndex = 0;
		m_lpWall->NewOneWall(nIndex);

		FillWallList();
	}
}

void KSceneEditorWallSetupDialog::OnBnClickedButtonDeletewall()
{
	if(m_lpWall && m_nCurrentSelectedWall!=-1)
	{
		m_lpWall->DeleteOneWall(m_nCurrentSelectedWall);
		m_nCurrentSelectedWall =-1;
	}
	FillWallList();
}

void KSceneEditorWallSetupDialog::FillWallList()
{
	if(!m_lpWall)
		return ;
	{
		int nCount = m_listWall.GetCount();
		for(int i=0;i<nCount;i++)
		{
			m_listWall.DeleteString(0);
		}
	}

	int nCount = m_lpWall->GetNumWall();
	for (int i=0;i<nCount;i++)
	{
		TCHAR szName[MAX_PATH];
		wsprintf(szName,"Wall %d",i);
		int nindex = m_listWall.AddString(szName);
		m_listWall.SetItemData(nindex,i);
	}

}
void KSceneEditorWallSetupDialog::OnBnClickedButtonAddpoint()
{
	if (m_lpWall && m_lpSceneEditor)
	{
		if(m_nCurrentSelectedWall != -1)
		{
			m_lpSceneEditor->AddWallNodeToScene(m_lpWall,m_nCurrentSelectedWall,0);
		}
	}
}
