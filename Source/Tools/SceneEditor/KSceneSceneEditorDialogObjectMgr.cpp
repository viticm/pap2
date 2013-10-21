// KSceneSceneEditorDialogObjectMgr.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KSceneSceneEditorDialogObjectMgr.h"
#include ".\kscenesceneeditordialogobjectmgr.h"
//#include "KG3DSceneSceneEditor.h"
#include "IEEditor.h"//by dengzhihui Refactor 2006年11月29日
#include "KG3DSceneEditorDirectory.h"
//#include "SceneSkinEdit.h"
#include "KSceneSceneEditorDialogOranment.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KSceneSceneEditorDialogObjectMgr dialog
#define TYPE_ALLOBJECT  0
#define TYPE_SINGEMESH  1

#define TYPE_BUILDING   2
#define TYPE_NPC        3

#define TYPE_SINGEMESHGROUP  100

extern BOOL g_bTypeEnable[10];

KSceneSceneEditorDialogObjectMgr::KSceneSceneEditorDialogObjectMgr(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneSceneEditorDialogObjectMgr::IDD, pParent)
{
	//{{AFX_DATA_INIT(KSceneSceneEditorDialogObjectMgr)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_lpSceneEditor = NULL;
	m_nOutputWindowID = 0;
}


void KSceneSceneEditorDialogObjectMgr::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KSceneSceneEditorDialogObjectMgr)
	DDX_Control(pDX, IDC_TREE1, m_TreeGroup);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_TREE2, m_Tree);
	DDX_Control(pDX, IDC_STATIC_PAGES, m_outputWnd);
}


BEGIN_MESSAGE_MAP(KSceneSceneEditorDialogObjectMgr, CDialog)
	//{{AFX_MSG_MAP(KSceneSceneEditorDialogObjectMgr)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_AUTOCHECK, OnBnClickedButtonAutocheck)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, OnLvnItemchangedList1)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE1, OnNMDblclkTree1)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, OnTvnSelchangedTree1)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE2, &KSceneSceneEditorDialogObjectMgr::OnTvnSelchangedTree2)
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(NM_CLICK, IDC_TREE2, &KSceneSceneEditorDialogObjectMgr::OnNMClickTree2)
	ON_BN_CLICKED(IDC_BUTTON3, &KSceneSceneEditorDialogObjectMgr::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON_REPLACE, &KSceneSceneEditorDialogObjectMgr::OnBnClickedButton_ReplaceSelectedWithObjectInhandle)
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KSceneSceneEditorDialogObjectMgr message handlers

void KSceneSceneEditorDialogObjectMgr::OnBnClickedButtonAutocheck()
{
	// TODO: Add your control notification handler code here
	if(!m_lpSceneEditor)
		return;
	m_lpSceneEditor->GroupSameNameAndTypeObjects();
	FillTreeGroup();
}

BOOL KSceneSceneEditorDialogObjectMgr::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	FillTreeGroup();

	//FillTree();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneSceneEditorDialogObjectMgr::FillTreeGroup()
{
	m_TreeGroup.DeleteAllItems();
	
	if(!m_lpSceneEditor)
		return;

	m_TreeGroup.InsertItem("所有物件",TYPE_ALLOBJECT,TYPE_ALLOBJECT);
	HTREEITEM hSingleMode = m_TreeGroup.InsertItem("模型",TYPE_SINGEMESH,TYPE_SINGEMESH);
	//m_Tree.InsertItem("建筑",TYPE_BUILDING,TYPE_BUILDING);
	//m_Tree.InsertItem("NPC",TYPE_NPC,TYPE_NPC);

	int Index = 0;

	/*
	map<string,KG3DSceneSceneEditor::ObjectGroup>::iterator i =
	m_lpSceneEditor->m_mapObjectGroup.begin();

	while(i!=m_lpSceneEditor->m_mapObjectGroup.end())
	{
	string GroupName = i->first;
	KG3DSceneSceneEditor::ObjectGroup* pGroup = &i->second;

	TCHAR Name[256];
	wsprintf(Name,"%d %d %s",Index,pGroup->listObject.size(),GroupName.c_str());

	HTREEITEM hItem = m_Tree.InsertItem(Name,TYPE_SINGEMESHGROUP,TYPE_SINGEMESHGROUP,hSingleMode,TVI_LAST);
	m_Tree.SetItemData(hItem,(DWORD)Index);

	Index++;
	i++;
	}
	*/

	INT nCount = m_lpSceneEditor->GetObjectGroupSize();
	for (INT i = 0; i < nCount; i ++)
	{
		TCHAR szName[256];
		INT nGroupSize = 0;
		m_lpSceneEditor->GetGroupInfo(i, nGroupSize, szName, 256);
		
		TCHAR Name[256];
		wsprintf(Name,"%d %d %s",Index,nGroupSize,szName);
		
		HTREEITEM hItem = m_TreeGroup.InsertItem(Name,TYPE_SINGEMESHGROUP,TYPE_SINGEMESHGROUP,hSingleMode,TVI_LAST);
		m_TreeGroup.SetItemData(hItem,(DWORD)Index);

		Index++;		
	}
}

void KSceneSceneEditorDialogObjectMgr::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

HRESULT KSceneSceneEditorDialogObjectMgr::FillTree()
{
	m_Tree.DeleteAllItems();

	int nCount = g_cEditorDirectory.GetNumofDirectory();
	for(int i=0;i<nCount;i++)
	{
		TCHAR Title[256];
		TCHAR Name[256];

		g_cEditorDirectory.GetDirectory(i,Title,Name);

		size_t k = strlen(Name);
		if(Name[k-1] == '\\')
		{
			Name[k-1] = '\0';
		}
		HTREEITEM hitem = m_Tree.InsertItem(Name,IMAGE_DIR,IMAGE_DIR,TVI_ROOT);
		FillTreeWithDir(Name,".Mesh",hitem);
	}
	return S_OK;
}

HRESULT KSceneSceneEditorDialogObjectMgr::FillTreeWithDir(LPSTR Dir,LPSTR pFileType,HTREEITEM hStart)
{
	//TCHAR path_buffer[256];
	TCHAR drive[256];
	TCHAR dir[256];
	TCHAR fname[256];
	TCHAR ext[256];

	WIN32_FIND_DATA Find;
	IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
	TCHAR szAppName[MAX_PATH];
	TCHAR szHalfPath[MAX_PATH];
	TCHAR szFullPath[MAX_PATH];
	TCHAR szSingleFile[MAX_PATH];

	wsprintf(szFullPath,"%s%s\\*.*", g_szDefWorkDirectory, Dir);
	g_pEngineManager->GetDefWorkDirectory(szAppName, MAX_PATH);
	HANDLE hFind = FindFirstFile(szFullPath, &Find);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		strcpy(szSingleFile, Find.cFileName);

		_splitpath(szSingleFile, drive, dir, fname, ext );

		if ((_strcmpi(szSingleFile, ".") != 0) &&
			(_strcmpi(szSingleFile, "..") != 0) &&
			_strcmpi(szSingleFile, ".svn"))
		{
			if ((Find.dwFileAttributes != 0xFFFFFFFF)
				&&(Find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				HTREEITEM hItem = m_Tree.InsertItem(szSingleFile, IMAGE_DIR,IMAGE_DIR, hStart);
				wsprintf(szHalfPath,"%s\\%s", Dir, szSingleFile);
				FillTreeWithDir(szHalfPath, pFileType, hItem);
			}
			else
			{	

				int n = GetItemImage(ext);
				if (n != IMAGE_ERROR)
				{
					TCHAR strFilePath[MAX_PATH];
					sprintf_s(strFilePath,
						MAX_PATH,
						"%s%s\\%s",
						szAppName,
						Dir,
						szSingleFile);

					if(g_bTypeEnable[n])
					{
						HTREEITEM hItem = m_Tree.InsertItem(szSingleFile, n, n, hStart);
					}
				}
			}
		}

		while(FindNextFile(hFind, &Find))
		{
			strcpy(szSingleFile, Find.cFileName);
			_splitpath(szSingleFile, drive, dir, fname, ext );

			if((_strcmpi(szSingleFile, ".") != 0) && 
				(_strcmpi(szSingleFile, "..") != 0) &&
				_strcmpi(szSingleFile, ".svn"))
			{

				if((Find.dwFileAttributes != 0xFFFFFFFF)
					&&(Find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					HTREEITEM hItem = m_Tree.InsertItem(szSingleFile, IMAGE_DIR,IMAGE_DIR, hStart);
					wsprintf(szHalfPath,"%s\\%s", Dir, szSingleFile);
					FillTreeWithDir(szHalfPath, pFileType, hItem);
				}
				else	
				{
					int n = GetItemImage(ext);
					if (n != IMAGE_ERROR)
					{
						TCHAR strFilePath[MAX_PATH];
						sprintf_s(strFilePath,
							MAX_PATH,
							"%s%s\\%s",
							szAppName,
							Dir,
							szSingleFile);

						if(g_bTypeEnable[n])
						{
							HTREEITEM hItem = m_Tree.InsertItem(szSingleFile, n, n, hStart);
						}
					}
				}
			}
		}
		FindClose(hFind);
	}
	return S_OK;
}

void KSceneSceneEditorDialogObjectMgr::OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	if(!m_lpSceneEditor)
		return;
	// TODO: Add your control notification handler code here
	*pResult = 0;
	HTREEITEM hSelect = m_TreeGroup.GetSelectedItem();
	if (!hSelect)
		return;
	int nImage = 0;
	m_TreeGroup.GetItemImage(hSelect,nImage,nImage);
	DWORD Index = (DWORD)m_TreeGroup.GetItemData(hSelect);

	switch(nImage)
	{
	case TYPE_SINGEMESHGROUP:
		{
			/*IEKG3DSceneEditorBase* pBase = NULL;
			m_lpSceneEditor->GetSceneEditorBase(&pBase);
			_ASSERTE(pBase);
			if (!pBase) return;				
			pBase->ClearSelectedEntity();*/

			IEKG3DSceneSelectionTool* pTool = NULL;
			m_lpSceneEditor->GetSelectionToolInterface(&pTool);
			if(NULL == pTool)
				return;
			pTool->ClearSelection();

			m_lpSceneEditor->AddSelectedEntityByIndex(Index);
		}	
		break;
	}
}

void KSceneSceneEditorDialogObjectMgr::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}



void KSceneSceneEditorDialogObjectMgr::OnTvnSelchangedTree2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void KSceneSceneEditorDialogObjectMgr::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if(bShow)
	{
		m_nOutputWindowID = 0;
		m_lpSceneEditor->AddOutputWindow("ScenePreView",m_outputWnd.GetSafeHwnd(),
			OUTPUTWND_SCENEEDITOR_OBJECTPREVEIW,0,&m_nOutputWindowID);
	}
	else
	{
		m_lpSceneEditor->RemoveOutputWindow(m_nOutputWindowID);
	}
}

void KSceneSceneEditorDialogObjectMgr::GetItemFilePath(HTREEITEM hItem, TCHAR strFilePath[])
{
	TCHAR szAppDir[MAX_PATH];
	TCHAR strFileName[MAX_PATH];
	IEKG3DEngineManager *pManager = g_GetEngineIEInterface();

	KG_PROCESS_ERROR(hItem);

	strcpy(strFileName, m_Tree.GetItemText(hItem));
	HTREEITEM hParentItem = m_Tree.GetParentItem(hItem);
	while (hParentItem)
	{
		sprintf(strFilePath, "%s\\%s", m_Tree.GetItemText(hParentItem), strFileName);
		strcpy(strFileName, strFilePath);
		hParentItem = m_Tree.GetParentItem(hParentItem);
	}
	pManager->GetDefWorkDirectory(szAppDir, MAX_PATH);
	sprintf(strFilePath, "%s%s", szAppDir, strFileName);
Exit0:
	return;
}
void KSceneSceneEditorDialogObjectMgr::OnNMClickTree2(NMHDR *pNMHDR, LRESULT *pResult)
{
	HRESULT hr = E_FAIL;
	IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
	*pResult = 0;

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	TCHAR strFilePath[MAX_PATH];

	CPoint ptMouse;
	GetCursorPos(&ptMouse);
	m_Tree.ScreenToClient(&ptMouse);

	HTREEITEM Item = m_Tree.HitTest(ptMouse);

	if(!Item)
		return;
	int nImage = 0;
	m_hLastItem = Item;
	m_Tree.GetItemImage(Item,nImage,nImage);

	GetItemFilePath(Item, strFilePath);
	switch(nImage)
	{
	case IMAGE_MESH:
	case IMAGE_MODEL:
	case IMAGE_SPEEDTREE:
	case IMAGE_SFX:
		{
			IEKG3DRepresentObject* pObj = NULL;
			HRESULT hr = g_pEngineManager->BuildKG3DRepresentObject(strFilePath, REPRESENTOBJECT_DEFAULT, 0, 0, &pObj);
			if (SUCCEEDED(hr))
			{
				hr = m_lpSceneEditor->SetObjectInHandle(pObj, TRUE);
				if (FAILED(hr))
				{
					SAFE_RELEASE(pObj);
				}
			}
			break;
		}
	case IMAGE_SET:
		{
			IEKG3DRepresentObject* pObjSet = NULL;
			hr = pManager->BuildKG3DRepresentObject(strFilePath, REPRESENTOBJECT_SET, 0, 0, &pObjSet);
			if (SUCCEEDED(hr))
			{
				m_lpSceneEditor->SetObjectInHandle(pObjSet, TRUE);
			}


			break;
		}
	case IMAGE_SOUNDBALL:
		{
			/*
			IEKG3DRepresentObjectSoundBall* pSoundBall = NULL;
			
						if(SUCCEEDED(pManager->NewKG3DRepresentObjectSoundBallInTable(&pSoundBall)))
						{
							pSoundBall->LoadFromSoundBallDataFile(strFilePath);
			
							m_lpSceneEditor->SetObjectInHandle(pSoundBall, TRUE);
							m_lpSceneEditor->SetEditState(SCENESTATE_PLACEOBJ); 
						}*/
			IEKG3DRepresentObject* pObjSet = NULL;
			hr = pManager->BuildKG3DRepresentObject(strFilePath, REPRESENTOBJECT_SOUNDBALL, 0, 0, &pObjSet);
			if (SUCCEEDED(hr))
			{
				m_lpSceneEditor->SetObjectInHandle(pObjSet, TRUE);
			}

			break;
		}
	case IMAGE_GROUP:
		{
			IEKG3DRepresentObject* pObjSet = NULL;
			hr = pManager->BuildKG3DRepresentObject(strFilePath, REPRESENTOBJECT_DEFAULT, 0, 0, &pObjSet);
			if (SUCCEEDED(hr))
			{
				m_lpSceneEditor->SetObjectInHandle(pObjSet, TRUE);
			}

			break;
		}
	}
	HWND hBaseWnd;
	m_Tree.SelectItem(Item);
	pManager->GetBaseWnd(&hBaseWnd);
	::SetFocus(hBaseWnd);			

	*pResult = 0;
}

void KSceneSceneEditorDialogObjectMgr::OnBnClickedButton3()
{
	FillTree();
}

void KSceneSceneEditorDialogObjectMgr::OnBnClickedButton_ReplaceSelectedWithObjectInhandle()
{
	m_lpSceneEditor->ReplaceSelectedObjectWithInHand();
	m_lpSceneEditor->GroupSameNameAndTypeObjects();
	FillTreeGroup();
}


void KSceneSceneEditorDialogObjectMgr::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);
	if (nState == WA_ACTIVE || nState == WA_CLICKACTIVE)
	{
		OnBnClickedButtonAutocheck();
	}
	
	// TODO: Add your message handler code here
}
