// KSceneSceneEditorDialogDoodad.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSceneEditorDialogDoodad.h"

#include "KNpcTemplateList.h"
#include "KSO3World.h"
#include "IEEditor.h"
#include "SO3Represent/SO3RepresentInterface.h"
#include "KSceneEditorReviveDialog.h"
#include "KSceneHelperFuncs.h"
#include "KSceneEditorDialogCheckRelive.h"
#include "..\..\KG3DEngine\EditInterface\IEKG3DSceneSelectionTool.h"
#include "..\..\KG3DEngine\EditInterface\IEKG3DSceneObjectPlacementTool.h"


// KSceneSceneEditorDialogDoodad dialog

extern IKSO3RepresentHandler* g_pRepresentHandler;

IMPLEMENT_DYNAMIC(KSceneSceneEditorDialogDoodad, CDialog)

KSceneSceneEditorDialogDoodad::KSceneSceneEditorDialogDoodad(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneSceneEditorDialogDoodad::IDD, pParent)
	, m_Substr(_T(""))
{
	m_lpSceneSceneEditor = NULL;
	m_nOutputWindowID = -1;
	m_FindNode = NULL;
	m_lpDocLogical = NULL;
	m_dwDoodadTempleteID = 0;
	m_dwDefaultReliveID = 0;
}
  
  
KSceneSceneEditorDialogDoodad::~KSceneSceneEditorDialogDoodad()
{
}

void KSceneSceneEditorDialogDoodad::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_NPC, m_Tree);
	DDX_Control(pDX, IDC_STATIC_PREVIEW, m_Static_PreView);
	DDX_Control(pDX, IDC_BUTTON_RELIVE, m_Button_Relive);
	DDX_Control(pDX, IDC_BUTTON_CHECKRELIVE, m_Button_CheckRelive);
}


BEGIN_MESSAGE_MAP(KSceneSceneEditorDialogDoodad, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_NPC, &KSceneSceneEditorDialogDoodad::OnTvnSelchangedTreeNpc)
	ON_BN_CLICKED(IDC_BUTTON_NEWNPC, &KSceneSceneEditorDialogDoodad::OnBnClickedButtonNewnpc)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_LOOKUP, &KSceneSceneEditorDialogDoodad::OnBnClickedButtonLookup)
	ON_EN_CHANGE(IDC_EDIT_LOOKUP, &KSceneSceneEditorDialogDoodad::OnEnChangeEditLookup)
	ON_BN_CLICKED(IDC_BUTTON_RELIVE, &KSceneSceneEditorDialogDoodad::OnBnClickedButtonRelive)
	ON_BN_CLICKED(IDC_BUTTON_CHECKRELIVE, &KSceneSceneEditorDialogDoodad::OnBnClickedButtonCheckRelive)
	ON_BN_CLICKED(IDC_DOODAD_REPLACE_ONE, &KSceneSceneEditorDialogDoodad::OnBnClickedDoodadReplaceOne)
	ON_BN_CLICKED(IDC_DOODAD_REPLACE_ALL, &KSceneSceneEditorDialogDoodad::OnBnClickedDoodadReplaceAll)
END_MESSAGE_MAP()


// KSceneSceneEditorDialogDoodad message handlers

HRESULT KSceneSceneEditorDialogDoodad::CreateDoodadInHandle()
{
	HRESULT hr = E_FAIL;

	KGLOG_PROCESS_ERROR(g_pSO3World);

	const KDoodadTemplate* pcDoodadTemplate = g_pSO3World->m_Settings.m_DoodadTemplateList.GetTemplate(m_dwDoodadTempleteID);
	if (pcDoodadTemplate && pcDoodadTemplate->m_dwTemplateID != 0)
	{
		KRepresentDoodadResource DoodadRes;

		g_pRepresentHandler->GetDoodadResource(pcDoodadTemplate->m_dwRepresentID,&DoodadRes);

		LPCTSTR pMeshName = DoodadRes.aAnimation[0].szMeshFilePath;

		LPCTSTR pMtlName = DoodadRes.aAnimation[0].szMtlFilePath;
		LPCTSTR pAniName = DoodadRes.aAnimation[0].szAniFilePath;
		if(!pMeshName)
		{
			pMeshName = "data\\public\\scenerunner\\scenerunner.Mesh";
			pAniName  = "data\\public\\scenerunner\\scenerunner.Ani";
		}
		TCHAR szMeshName[MAX_PATH];
		TCHAR szMtlName[MAX_PATH];
		TCHAR szAniName[MAX_PATH];
		wsprintf(szMeshName,"%s",pMeshName);
		wsprintf(szMtlName,"%s" ,pMtlName);
		wsprintf(szAniName,"%s" ,pAniName);

		{
			IEKG3DRepresentObject* pObj = BuildRepresentDoodad(g_GetEngineIEInterface()
				, m_dwDoodadTempleteID, m_szDoodadName, szMeshName, szMtlName, pAniName,m_dwDefaultReliveID
				, true, pcDoodadTemplate->m_bDynamicObstacle);

			if(pObj)
			{
				hr = m_lpSceneSceneEditor->SetObjectInHandle(pObj, TRUE);//生命周期管理移交了
				if (FAILED(hr))
				{
					SAFE_RELEASE(pObj);
				}
			}
		}

	}

	return S_OK;
Exit0:
	return E_FAIL;
}

void KSceneSceneEditorDialogDoodad::OnTvnSelchangedTreeNpc(NMHDR *pNMHDR, LRESULT *pResult)
{
	HRESULT hRetCode = S_OK;

	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	if(!m_lpSceneSceneEditor)
		return ;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem)
		return ;

	m_dwDoodadTempleteID = (DWORD)m_Tree.GetItemData(hItem);
	m_szDoodadName = m_Tree.GetItemText(hItem);

	CreateDoodadInHandle();

}

void KSceneSceneEditorDialogDoodad::OnBnClickedButtonNewnpc()
{
	HRESULT hRetCode = S_OK;
	if(!m_lpSceneSceneEditor)
		return ;

    if (g_pSO3World == NULL)
        return;

	m_Tree.DeleteAllItems();
	m_mapType.clear();

	TCHAR Name[MAX_PATH];

	HTREEITEM hClass = NULL;
	int nCount = g_pSO3World->m_Settings.m_DoodadTemplateList.GetDoodadTemplateCount();
	
	for (int nIndex = 0; nIndex < nCount; nIndex++)
	{
		const KDoodadTemplate* pcDoodadTemplate = g_pSO3World->m_Settings.m_DoodadTemplateList.GetTemplateByIndex(nIndex);

		if (pcDoodadTemplate  && pcDoodadTemplate->m_dwTemplateID != 0)
		{
			map<CString,HTREEITEM>::iterator s = m_mapType.find(pcDoodadTemplate->m_szMapName);
			if(s==m_mapType.end())
			{
				hClass = m_Tree.InsertItem(pcDoodadTemplate->m_szMapName,NULL,TVI_LAST);
				m_Tree.SetItemData(hClass,0);
				m_mapType[pcDoodadTemplate->m_szMapName] = hClass;
			}
			else
			{
				hClass = s->second;
			}
			HTREEITEM hItem = m_Tree.InsertItem(pcDoodadTemplate->m_szName,hClass,TVI_LAST);
			m_Tree.SetItemData(hItem, pcDoodadTemplate->m_dwTemplateID);

			//////////////////////////////////////////////////////////////////////////
			DWORD dwCount =0;
			dwCount = _kg_to_smaller_size_cast<DWORD>(m_Tree.GetItemData(hClass));
			dwCount++;
			m_Tree.SetItemData(hClass,dwCount);
			wsprintf(Name,"%s （%d）",pcDoodadTemplate->m_szMapName,dwCount);
			m_Tree.SetItemText(hClass,Name);
		}
		
		////不分类
		//if (pcDoodadTemplate  && pcDoodadTemplate->m_dwTemplateID != 0)
		//{
		//	HTREEITEM hItem = m_Tree.InsertItem(pcDoodadTemplate->m_szName);
		//	m_Tree.SetItemData(hItem, pcDoodadTemplate->m_dwTemplateID);
		//}
	}

	m_Tree.SortTree(NULL);
}

void KSceneSceneEditorDialogDoodad::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if(bShow)
	{
		m_nOutputWindowID = 0;
		m_lpSceneSceneEditor->AddOutputWindow("ScenePreView",m_Static_PreView.GetSafeHwnd(),
			OUTPUTWND_SCENEEDITOR_OBJECTPREVEIW,0,&m_nOutputWindowID);
		m_lpSceneSceneEditor->IEOnResizeWindow(NULL);
	}
	else
	{
		m_lpSceneSceneEditor->RemoveOutputWindow(m_nOutputWindowID);
	}
}

BOOL KSceneSceneEditorDialogDoodad::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	OnBnClickedButtonNewnpc();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneSceneEditorDialogDoodad::OnBnClickedButtonLookup()
{
	UpdateData(TRUE);
	if(m_Substr=="")
	{
		MessageBox("查找内容为空，请输入!");
		return ;
	}
	m_FindNode = m_Tree.FindNextItembySubString(m_Substr,m_FindNode);
	KG_PROCESS_ERROR(m_FindNode);
	

	m_Tree.SelectItem(m_FindNode);
	((CButton*)(GetDlgItem(IDC_BUTTON_LOOKUP)))->SetWindowText("查找下一个");
	return ; 
Exit0:
	m_Tree.SelectItem(NULL);
	MessageBox("查找结束，没有找到任何选合的Doodad项!");
	((CButton*)(GetDlgItem(IDC_BUTTON_LOOKUP)))->SetWindowText("开始查找");
}

void KSceneSceneEditorDialogDoodad::OnEnChangeEditLookup()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	GetDlgItemText(IDC_EDIT_LOOKUP,m_Substr);
	((CButton*)(GetDlgItem(IDC_BUTTON_LOOKUP)))->SetWindowText("开始查找");
	m_FindNode = NULL;
}

void KSceneSceneEditorDialogDoodad::OnBnClickedButtonRelive()
{
	KSceneEditorReviveDialog dlg;
	dlg.SetDocLogical(m_lpDocLogical,FALSE);

	dlg.DoModal();

	m_dwDefaultReliveID = dlg.m_dwCurrentSelectedID;

	TCHAR szName[MAX_PATH];
	wsprintf(szName,"重生组ID %d",m_dwDefaultReliveID );
	m_Button_Relive.SetWindowText(szName);

	CreateDoodadInHandle();
}

void KSceneSceneEditorDialogDoodad::OnBnClickedButtonCheckRelive()
{
	KSceneEditorDialogCheckRelive::This->ShowWindow(SW_NORMAL);
}


bool KSceneSceneEditorDialogDoodad::ReplaceSelectedDoodad(OUT vector<DWORD> &templateids)
{

	//得到鼠标拾取的doodad
	IEKG3DRepresentObject * obj = 0;
	m_lpSceneSceneEditor->GetObjectInHandle(&obj);

	if(!obj)
	{
		MessageBox("请从列表中选取Doodad物件", "警告", 0);
		return false;
	}


	IEKG3DRepresentDoodad *handledoodad = m_lpSceneSceneEditor->GetIEKG3DRepresentDoodad(obj);

	DWORD templateId = 0;
	handledoodad->GetTemplateID(&templateId);

	IEKG3DSceneObjectPlacementTool* pPlacementTool = KSH::GetEngineInterfaceEx<IEKG3DSceneObjectPlacementTool>();
	IEKG3DSceneSelectionTool* pSelTool = NULL;
	if(FAILED(m_lpSceneSceneEditor->GetSelectionToolInterface(&pSelTool)))
		return false;


	IEKG3DSceneSelectionToolAccessProxy* pProxy = NULL;
	if(FAILED(pSelTool->GetAccessProxy(&pProxy)))
		return false;

	//安全考虑先做一次检查
	int selectedNum = 0;
	for (IEKG3DSceneEntity* pEntity = pProxy->Reset(); NULL != pEntity; pEntity = pProxy->Next())
	{
		if(!pEntity)
			continue;

		void *pointer;
		pEntity->GetPointer(&pointer);

		IEKG3DRepresentObject* pObject = (IEKG3DRepresentObject*)pointer;
		if(!pObject)
			continue;

		if(pObject->GetType() != REPRESENTOBJECT_DOODAD)
		{
			char msg[256];
			pEntity->GetName(msg, 256);
			strcat(msg, "不能被替换");
			MessageBox(msg, "警告", 0);
			return false;
		}
		selectedNum ++;
	}


	if(!selectedNum)
	{
		::MessageBox(0, "并未选中需要被替换的Doodad物件", "警告", 0);
		return false;
	}

	int nOK = ::MessageBox(0, "确定要替换吗？","提示信息",MB_OKCANCEL   |   MB_ICONQUESTION);
	if(nOK != IDOK)
	{
		return false;
	}



    //////////////////////////////上面检测通过了，下面执行替换



	//将要被移除的
	vector<IEKG3DRepresentObject *> removeObjs;

	//替换掉需要添加到选择列表的
	vector<IEKG3DRepresentObject *> selObjs;

	for (IEKG3DSceneEntity* pEntity = pProxy->Reset(); NULL != pEntity; pEntity = pProxy->Next())
	{
		if(!pEntity)
			continue;

		void *pointer;
		pEntity->GetPointer(&pointer);

		IEKG3DRepresentObject* pObject = (IEKG3DRepresentObject*)pointer;
		if(!pObject)
			continue;

		IEKG3DRepresentObject * pOutObject = NULL;
		pPlacementTool->DoPlaceObjectByOther(pObject, &pOutObject);


		//添加完成了，会重新分配一个doodadinfo, doodadinfo列表中新旧并存，需要把旧的doodadinfo的有用信息拷贝过来
		KSceneEditorDocLogical::DoodadInfo *infoOld = 0;
		KSceneEditorDocLogical::DoodadInfo *infoNew = 0;

		//从Doodad列表中分别查找新旧两个Doodadinfo
		for(std::list<KSceneEditorDocLogical::DoodadInfo*>::iterator it = m_lpDocLogical->m_listDoodadInfo.begin(); it!=  m_lpDocLogical->m_listDoodadInfo.end(); it ++)
		{
			KSceneEditorDocLogical::DoodadInfo *info = *it;

			if(info->pPointer == pObject)
				infoOld = info;

			if(info->pPointer == pOutObject)
				infoNew = info;

			if(infoOld && infoNew)
				break;
		}

		string newName = infoNew->szName;
		DWORD newTemplateId = infoNew->nTempleteID;
		

		assert(infoNew && infoOld);

		//数据拷贝
		*infoNew = *infoOld;

		//恢复自己的数据
		infoNew->szName = newName;
		infoNew->nTempleteID = newTemplateId;
		infoNew->pPointer = pOutObject;

		//更新已替换的templateid列表，过滤掉重复的
		if(find(templateids.begin(), templateids.end(), infoOld->nTempleteID) == templateids.end())
		{
			templateids.push_back(infoOld->nTempleteID);
		}

		//从doodad信息列表中删除
		m_lpDocLogical->DeleteDoodad(infoOld);
		removeObjs.push_back(pObject);
		selObjs.push_back(pOutObject);
	}


	for(vector<IEKG3DRepresentObject *>::iterator it = removeObjs.begin(); it != removeObjs.end(); it ++)
	{
		IEKG3DRepresentObject * obj = *it;
		//从选择列表中删除
		pSelTool->SetSelected(*obj, false);
		//从场景中删除
		m_lpSceneSceneEditor->RemoveRepresentObject(obj);
	}

	//更换以后的添加到选择列表
	for(vector<IEKG3DRepresentObject *>::iterator it = selObjs.begin(); it != selObjs.end(); it ++)
	{
		IEKG3DRepresentObject * obj = *it;
		pSelTool->SetSelected(*obj, true);
	}



	return true;
}

void KSceneSceneEditorDialogDoodad::OnBnClickedDoodadReplaceOne()
{
	vector<DWORD> replacedTemplateids;
	ReplaceSelectedDoodad(replacedTemplateids);

	//替换完成，设置为场景选择状态，这样能取消对模型列表的选择
	m_lpSceneSceneEditor->SetEditState(SCENESTATE_SELECT);

}

void KSceneSceneEditorDialogDoodad::OnBnClickedDoodadReplaceAll()
{

	//先替换掉被选中的
	vector<DWORD> replacedTemplateids;
	if(!ReplaceSelectedDoodad(replacedTemplateids))
	{
		m_lpSceneSceneEditor->SetEditState(SCENESTATE_SELECT);
		return;
	}

	//得到鼠标拾取的
	IEKG3DRepresentObject * obj;
	m_lpSceneSceneEditor->GetObjectInHandle(&obj);
	IEKG3DRepresentDoodad *handleDoodad = m_lpSceneSceneEditor->GetIEKG3DRepresentDoodad(obj);

	DWORD templateId = 0;
	handleDoodad->GetTemplateID(&templateId);


	char doodadName[256];
	handleDoodad->GetName(doodadName, 256);

	IEKG3DSceneObjectPlacementTool* pPlacementTool = KSH::GetEngineInterfaceEx<IEKG3DSceneObjectPlacementTool>();
	IEKG3DSceneSelectionTool* pSelTool = NULL;
	if(FAILED(m_lpSceneSceneEditor->GetSelectionToolInterface(&pSelTool)))
		return;

	IEKG3DSceneSelectionToolAccessProxy* pProxy = NULL;
	if(FAILED(pSelTool->GetAccessProxy(&pProxy)))
		return;



	//在场景里面做查找，选中的已经被替换了，经过下面的迭代，场景中已经加载但未被选中，而且需要被替换的就出来了
	vector<IEKG3DRepresentObject*> UnSelectedNeedReplaceDooldads;
	for(vector<DWORD>::iterator it  = replacedTemplateids.begin(); it!= replacedTemplateids.end(); it ++)
	{
		DWORD &tid = *it;
		m_lpSceneSceneEditor->FindLoadedDoodad(UnSelectedNeedReplaceDooldads, tid);
	}

	for(vector<IEKG3DRepresentObject*>::iterator it = UnSelectedNeedReplaceDooldads.begin(); it!= UnSelectedNeedReplaceDooldads.end(); it ++)
	{
		IEKG3DRepresentObject *pObject = *it;

		IEKG3DRepresentObject * pOutObject = NULL;
		pPlacementTool->DoPlaceObjectByOther(pObject, &pOutObject);


		//添加完成了，会重新分配一个doodadinfo, doodadinfo列表中新旧并存，需要把旧的doodadinfo的有用信息拷贝过来
		KSceneEditorDocLogical::DoodadInfo *infoOld = 0;
		KSceneEditorDocLogical::DoodadInfo *infoNew = 0;

		//从Doodad列表中分别查找新旧两个doodadinfo
		for(std::list<KSceneEditorDocLogical::DoodadInfo*>::iterator it2 = m_lpDocLogical->m_listDoodadInfo.begin(); it2!=  m_lpDocLogical->m_listDoodadInfo.end(); it2 ++)
		{
			KSceneEditorDocLogical::DoodadInfo *info = *it2;

			if(info->pPointer == pObject)
				infoOld = info;

			if(info->pPointer == pOutObject)
				infoNew = info;

			if(infoOld && infoNew)
				break;
		}

		string newName = infoNew->szName;
		DWORD newTemplateId = infoNew->nTempleteID;

		assert(infoNew && infoOld);

		//数据拷贝
		*infoNew = *infoOld;

		//恢复自己的数据
		infoNew->szName = newName;
		infoNew->nTempleteID = newTemplateId;
		infoNew->pPointer = pOutObject;

		//从可选择列表中删除
		pSelTool->SetSelected(*pObject, false);

		//从场景中删除
		m_lpSceneSceneEditor->RemoveRepresentObject(pObject);

		//从Doodad信息列表中删除
		m_lpDocLogical->DeleteDoodad(infoOld);

	}


	//剩下的一定是未被加载到场景中，但也要被替换的，直接更换templateId  和doodadname这三个信息就好了
	for(vector<DWORD>::iterator it  = replacedTemplateids.begin(); it!= replacedTemplateids.end(); it ++)
	{
		DWORD &tid =  *it;
		for(std::list<KSceneEditorDocLogical::DoodadInfo*>::iterator it2 = m_lpDocLogical->m_listDoodadInfo.begin(); it2!=  m_lpDocLogical->m_listDoodadInfo.end(); it2 ++)
		{
			KSceneEditorDocLogical::DoodadInfo *info = *it2;
			if(info->nTempleteID == tid)
			{
				info->szName = doodadName;
				info->nTempleteID = templateId;
			}
		}
	}


	//替换完成，设置为场景选择状态，这样能取消对模型列表的选择
	m_lpSceneSceneEditor->SetEditState(SCENESTATE_SELECT);

}
