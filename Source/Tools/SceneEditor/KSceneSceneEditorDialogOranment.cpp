// KSceneSceneEditorDialogOranment.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KSceneSceneEditorDialogOranment.h"
//#include ".\kg3denginemanager.h"
#include ".\kscenesceneeditordialogoranment.h"
//#include "kg3dscenesceneeditor.h"
#include "KG3DSceneEditorDirectory.h"
//#include "KG3DRepresentObjectSet.h"
//#include ".\kg3drepresentobjecttable.h"
#include "shlwapi.h"
//(by dengzhihui 2006年7月5日 15:51:13
//#include "MainFrm.h"
//)
#include "ChildFrm.h"
#include "SceneEditorDoc.h"
//#include "KG3DObjectPropertyDefine.h"
#include "IEKG3DModelTable.h"
#include "iekg3dmesh.h"
#include "IEKG3DSceneObjectPlacementTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// KSceneSceneEditorDialogOranment dialog
BOOL g_bTypeEnable[12];


KSceneSceneEditorDialogOranment::KSceneSceneEditorDialogOranment(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneSceneEditorDialogOranment::IDD, pParent)
	//, m_lpObjPlacementTool(NULL)
	, m_strSub(_T(""))
{
	//{{AFX_DATA_INIT(KSceneSceneEditorDialogOranment)
	//}}AFX_DATA_INIT
	m_lpSceneSceneEditor = NULL;
	m_nOutputWindowID = 0;

	g_bTypeEnable[IMAGE_MESH]      = TRUE;
	g_bTypeEnable[IMAGE_SPEEDTREE] = TRUE;
	g_bTypeEnable[IMAGE_SFX]      = TRUE;
	g_bTypeEnable[IMAGE_OBJECT]   = TRUE;
	g_bTypeEnable[IMAGE_SET]     = TRUE;
	g_bTypeEnable[IMAGE_GROUP]   = TRUE;
	g_bTypeEnable[IMAGE_SOUNDBALL]   = TRUE;
	g_bTypeEnable[IMAGE_MODEL] = TRUE;
	g_bTypeEnable[IMAGE_WALL] = TRUE;
    g_bTypeEnable[IMAGE_PVS] = TRUE;
	g_bTypeEnable[IMAGE_POINTLIGHT] = TRUE;
}


void KSceneSceneEditorDialogOranment::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KSceneSceneEditorDialogOranment)
	DDX_Control(pDX, IDC_TREE1, m_Tree);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_STATIC_SAMLLVIEW, m_Static_PreView);
	DDX_Text(pDX, IDC_EDIT_SUBSTRING, m_strSub);
}


BEGIN_MESSAGE_MAP(KSceneSceneEditorDialogOranment, CDialog)
	//{{AFX_MSG_MAP(KSceneSceneEditorDialogOranment)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, OnSelchangedTree1)
	ON_WM_SHOWWINDOW()
    ON_BN_CLICKED(IDC_BUTTON_REFRESH, &KSceneSceneEditorDialogOranment::OnBnClickedButtonRefresh)
    ON_BN_CLICKED(IDC_BUTTON_RELOAD, &KSceneSceneEditorDialogOranment::OnBnClickedButtonReload)
	ON_NOTIFY(NM_SETFOCUS, IDC_TREE1, &KSceneSceneEditorDialogOranment::OnNMSetfocusTree1)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_CHECK_MESH, &KSceneSceneEditorDialogOranment::OnBnClickedCheckMesh)
	ON_BN_CLICKED(IDC_CHECK_SET, &KSceneSceneEditorDialogOranment::OnBnClickedCheckSet)
	ON_BN_CLICKED(IDC_CHECK_SPEEDTREE, &KSceneSceneEditorDialogOranment::OnBnClickedCheckSpeedtree)
	ON_BN_CLICKED(IDC_CHECK_OBJECT, &KSceneSceneEditorDialogOranment::OnBnClickedCheckObject)
	ON_BN_CLICKED(IDC_CHECK_SFX, &KSceneSceneEditorDialogOranment::OnBnClickedCheckSfx)
	ON_BN_CLICKED(IDC_CHECK_GROUP, &KSceneSceneEditorDialogOranment::OnBnClickedCheckGroup)
	ON_BN_CLICKED(IDC_BUTTON_CREATINI, &KSceneSceneEditorDialogOranment::OnBnClickedButtonCreatini)
	ON_BN_CLICKED(IDC_CHECK_SHOWNEW, &KSceneSceneEditorDialogOranment::OnBnClickedCheckShownew)
	ON_NOTIFY(NM_CLICK, IDC_TREE1, &KSceneSceneEditorDialogOranment::OnNMClickTree1)
    ON_BN_CLICKED(IDC_PLACE_PLIGHT, &KSceneSceneEditorDialogOranment::OnBnClickedPlacePlight)
	ON_BN_CLICKED(IDC_BUTTON_LOOKUP, &KSceneSceneEditorDialogOranment::OnBnClickedButtonLookup)
	ON_EN_CHANGE(IDC_EDIT_SUBSTRING, &KSceneSceneEditorDialogOranment::OnEnChangeEditSubstring)
	ON_BN_CLICKED(IDC_CHECK_PVS, &KSceneSceneEditorDialogOranment::OnBnClickedCheckPvs)
	ON_BN_CLICKED(IDC_CHECK_POINTLIGHT, &KSceneSceneEditorDialogOranment::OnBnClickedCheckPointlight)
	ON_BN_CLICKED(IDC_REPLACE_ONE_ENTITY, &KSceneSceneEditorDialogOranment::OnBnClickedReplaceOneEntity)
	ON_BN_CLICKED(IDC_REPLACE_ALL_ENTITY, &KSceneSceneEditorDialogOranment::OnBnClickedReplaceAllEntity)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KSceneSceneEditorDialogOranment message handlers

void KSceneSceneEditorDialogOranment::OnSize(UINT nType, int cx, int cy)
{
	CRect rectStatOld;
	CRect rectTreeOld;
	CRect rectStatNew;
	CRect rectTreeNew;	
	CRect rectClient;

	KG_PROCESS_ERROR(m_Tree);

	GetClientRect(&rectClient);
	m_Tree.GetWindowRect(&rectTreeOld);
	ScreenToClient(&rectTreeOld);
	m_Static_PreView.GetWindowRect(&rectStatOld);
	ScreenToClient(&rectStatOld);

	rectStatOld.right  = rectClient.right - 2;
	rectTreeOld.right  = rectClient.right - 2;
	rectTreeOld.bottom = rectClient.bottom - 5;

	m_Static_PreView.MoveWindow(&rectStatOld);
	m_Tree.MoveWindow(&rectTreeOld);

Exit0:
	return CDialog::OnSize(nType, cx, cy);
}

void KSceneSceneEditorDialogOranment::OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	return;//<CTH>//070316-解决C4702 unreachable code
	/*
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	    IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
	    HWND hBaseWnd;
		TCHAR strFilePath[MAX_PATH];
	    HTREEITEM Item = m_Tree.GetSelectedItem();
		if(!Item)
			return;
		int nImage = 0;
		m_Tree.GetItemImage(Item,nImage,nImage);
	    
	    //Get3DEngineInterface((void **)(&pManager));
	    pManager->GetBaseWnd(&hBaseWnd);
		GetItemFilePath(Item, strFilePath);
		switch(nImage)
		{
		case IMAGE_MESH:
		case IMAGE_MODEL:
		case IMAGE_SPEEDTREE:
		case IMAGE_SFX:
			{
				m_lpSceneSceneEditor->SetEditState(SCENESTATE_PLACEOBJ); 
				break;
			}
		case IMAGE_SET:
			{
				IEKG3DRepresentObject* pObjectSet = NULL;
				IKG3DResourceManager* pObjectTable = NULL;
				g_pEngineManager->GetRepresentObjectTable(&pObjectTable);
	
				if(SUCCEEDED(pObjectTable->NewOneResourse(REPRESENTOBJECT_SET,(IKG3DResourceBase**)&pObjectSet)))
				{
					IEKG3DRepresentObjectSet* pSet = (IEKG3DRepresentObjectSet*)(pObjectSet);
					pObjectSet->LoadFromIniFile(strFilePath,0);
					pSet->SetCurrentModel(0);
	
					m_lpSceneSceneEditor->SetObjectInHandle(pObjectSet, TRUE);
					m_lpSceneSceneEditor->SetEditState(SCENESTATE_PLACEOBJ);//m_dwEditState = SCENESTATE_PLACEOBJ; 
				}
	
				break;
			}
		case IMAGE_GROUP:
			{
				IEKG3DRepresentObject* pObject = NULL;
				IKG3DResourceManager* pObjectTable = NULL;
				g_pEngineManager->GetRepresentObjectTable(&pObjectTable);
	
				if(SUCCEEDED(pObjectTable->NewOneResourse(REPRESENTOBJECT_DEFAULT,(IKG3DResourceBase**)&pObject)))
				{
					pObject->LoadFromIniFile(strFilePath,0);
	
					m_lpSceneSceneEditor->SetObjectInHandle(pObject, TRUE);
					m_lpSceneSceneEditor->SetEditState(SCENESTATE_PLACEOBJ);//m_dwEditState = SCENESTATE_PLACEOBJ; 
				}
	
				break;
			}
		}
		::SetFocus(hBaseWnd);			
		
		*pResult = 0;*/
	
}


HRESULT KSceneSceneEditorDialogOranment::FillTree()
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

int GetItemImage(LPSTR extName)
{
	if(_strcmpi(extName,".Mesh")==0)
		return IMAGE_MESH;
	else if (_strcmpi(extName,".STREE")==0)
		return IMAGE_SPEEDTREE;
	else if(_strcmpi(extName,".Sfx")==0)
		return IMAGE_SFX;
	else if(_strcmpi(extName,".ModelSet")==0)
		return IMAGE_SET;
	else if(!_strcmpi(extName, ".mdl"))
		return IMAGE_MODEL;//改成Model，但在所有地方都保持原来的操作
	else if(!_strcmpi(extName, ".Group"))
		return IMAGE_GROUP;	
	else if(!_strcmpi(extName, ".SoundBall"))
		return IMAGE_SOUNDBALL;	
	else if(!_strcmpi(extName, ".Walldefine"))
		return IMAGE_WALL;	
	else if(!_strcmpi(extName, ".pvs"))
		return IMAGE_PVS;	
	else if(!_strcmpi(extName, ".plight"))
		return IMAGE_POINTLIGHT;	
	else
		return IMAGE_ERROR;
}

BOOL KSceneSceneEditorDialogOranment::CheckInIExist(const TCHAR *strFileName)
{
	TCHAR strIniFilePath[MAX_PATH];
	TCHAR strDriver[MAX_PATH];
	TCHAR strPath[MAX_PATH];
	TCHAR strFile[MAX_PATH];
	
	BOOL bReturn = TRUE;
	
	KG_PROCESS_SUCCESS(static_cast<CButton*>(GetDlgItem(IDC_CHECK_SHOWNEW))->GetCheck() == BST_UNCHECKED);
	_splitpath_s(strFileName,
		strDriver,
		MAX_PATH,
		strPath,
		MAX_PATH,
		strFile,
		MAX_PATH,
		NULL,
		0);

	sprintf_s(strIniFilePath, 
		MAX_PATH,
		"%s%s%s.ini",
		strDriver,
		strPath,
		strFile);
	bReturn = !::PathFileExists(strIniFilePath);
Exit1:
	return bReturn;
	
}

HRESULT KSceneSceneEditorDialogOranment::FillTreeWithDir(LPSTR Dir,LPSTR pFileType,HTREEITEM hStart)
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
					std::string strFile = fname;
					if(g_bTypeEnable[n] && CheckInIExist(strFilePath) && (!CheckIsLODModel(strFile)))
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
						std::string strFile = fname;
						if(g_bTypeEnable[n] && CheckInIExist(strFilePath)&& (!CheckIsLODModel(strFile)))
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

BOOL KSceneSceneEditorDialogOranment::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
 //   m_imgList.Create( IDB_BITMAP_ORAN, 18, 1,ILC_COLOR32);
//	m_Tree.SetImageList(&m_imgList,TVSIL_NORMAL);

    m_imgList.Create(18, 18, ILC_COLOR24, 8, 8);
    m_imgList.Add(AfxGetApp()->LoadIcon(IDC_DIR));
    m_imgList.Add(AfxGetApp()->LoadIcon(IDC_MESH));
    m_imgList.Add(AfxGetApp()->LoadIcon(IDC_SPEEDTREE));
    m_imgList.Add(AfxGetApp()->LoadIcon(IDC_SFX));
    m_imgList.Add(AfxGetApp()->LoadIcon(IDC_OBJ));
    m_imgList.Add(AfxGetApp()->LoadIcon(IDC_MESH));
    m_imgList.Add(AfxGetApp()->LoadIcon(IDC_MESH));
    m_imgList.Add(AfxGetApp()->LoadIcon(IDC_SOUND));
    m_imgList.Add(AfxGetApp()->LoadIcon(IDC_MODEL));
    m_imgList.Add(AfxGetApp()->LoadIcon(IDC_MESH));
    m_imgList.Add(AfxGetApp()->LoadIcon(IDC_PVS));
    m_imgList.Add(AfxGetApp()->LoadIcon(IDC_MESH));
    m_imgList.Add(AfxGetApp()->LoadIcon(IDC_PVS));
    m_Tree.SetImageList(&m_imgList,TVSIL_NORMAL);
	
	//FillTree(".Mesh");

	UpdateData(FALSE);

	CheckDlgButton(IDC_CHECK_MESH,TRUE);
	CheckDlgButton(IDC_CHECK_SET,TRUE);
	CheckDlgButton(IDC_CHECK_SPEEDTREE,TRUE);
	CheckDlgButton(IDC_CHECK_OBJECT,TRUE);
	CheckDlgButton(IDC_CHECK_SFX,TRUE);
	CheckDlgButton(IDC_CHECK_GROUP,TRUE);
	CheckDlgButton(IDC_CHECK_PVS,TRUE);
	CheckDlgButton(IDC_CHECK_POINTLIGHT,TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void KSceneSceneEditorDialogOranment::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);

	if(bShow)
	{
		m_nOutputWindowID = 0;
		m_lpSceneSceneEditor->AddOutputWindow("ScenePreView",m_Static_PreView.GetSafeHwnd(),
			OUTPUTWND_SCENEEDITOR_OBJECTPREVEIW,0,&m_nOutputWindowID);
	}
	else
	{
		m_lpSceneSceneEditor->RemoveOutputWindow(m_nOutputWindowID);
	}

}

void KSceneSceneEditorDialogOranment::OnOK()
{

}

void KSceneSceneEditorDialogOranment::OnBnClickedButtonRefresh()
{
	FillTree();
	// TODO: Add your control notification handler code here
}

void KSceneSceneEditorDialogOranment::OnBnClickedButtonReload()
{
    //(by dengzhihui 2006年7月5日 15:50:59
	CMDIFrameWnd *pMainFrame = (CMDIFrameWnd*)AfxGetMainWnd();
	//)
    MFCFramework::CChildFrame *pFrame = (MFCFramework::CChildFrame*)pMainFrame->GetActiveFrame();
    KSceneEditorDoc* pDoc = (KSceneEditorDoc*)pFrame->GetActiveDocument();
    if (!pDoc)
        return;

    IEKG3DScene* pScene = pDoc->GetScene();

    IEKG3DSceneEntity *piEntity = NULL;
    HRESULT hRetCode = E_FAIL;

    hRetCode = pScene->GetFirstRenderEntity(&piEntity);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    while (piEntity)
    {
        piEntity->Reload();

        hRetCode = pScene->GetNextRenderEntity(&piEntity);
        KGLOG_COM_PROCESS_ERROR(hRetCode);
    }
Exit0:
    return;
}
void KSceneSceneEditorDialogOranment::OnNMSetfocusTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	return;//<CTH>//070316-解决C4702 unreachable code
	/*
	int nResult  = false;
		int nRetCode = false;
	
		HTREEITEM hTreeItem = NULL;
		CPoint    obPoint;
		POINT     stpoint;
	
		GetCursorPos(&stpoint);
		m_Tree.ScreenToClient(&stpoint);
	
		obPoint.x = stpoint.x;
		obPoint.y = stpoint.y;
		
		hTreeItem = m_Tree.HitTest(obPoint);
		KG_PROCESS_ERROR(hTreeItem);
	
		m_Tree.SelectItem(hTreeItem);
		DWORD dwState = 0;
		m_lpSceneSceneEditor->GetEditState(&dwState);
		if (dwState == SCENESTATE_PLACEOBJ)
		{
			AfxGetMainWnd()->SetFocus();
		}
	
	Exit0:
		*pResult = 0;*/
	
}

void KSceneSceneEditorDialogOranment::OnBnClickedCheckMesh()
{
	BOOL b = IsDlgButtonChecked(IDC_CHECK_MESH);
	g_bTypeEnable[IMAGE_MESH] = b;
	g_bTypeEnable[IMAGE_MODEL] = b;
	FillTree();
}

void KSceneSceneEditorDialogOranment::OnBnClickedCheckSet()
{
	BOOL b = IsDlgButtonChecked(IDC_CHECK_SET);
	g_bTypeEnable[IMAGE_SET] = b;
	FillTree();
}

void KSceneSceneEditorDialogOranment::OnBnClickedCheckSpeedtree()
{
	BOOL b = IsDlgButtonChecked(IDC_CHECK_SPEEDTREE);
	g_bTypeEnable[IMAGE_SPEEDTREE] = b;
	FillTree();
}

void KSceneSceneEditorDialogOranment::OnBnClickedCheckObject()
{
	BOOL b = IsDlgButtonChecked(IDC_CHECK_OBJECT);
	g_bTypeEnable[IMAGE_OBJECT] = b;
	FillTree();
}

void KSceneSceneEditorDialogOranment::OnBnClickedCheckSfx()
{
	BOOL b = IsDlgButtonChecked(IDC_CHECK_SFX);
	g_bTypeEnable[IMAGE_SFX] = b;
	FillTree();
}

void KSceneSceneEditorDialogOranment::OnBnClickedCheckGroup()
{
	BOOL b = IsDlgButtonChecked(IDC_CHECK_GROUP);
	g_bTypeEnable[IMAGE_GROUP] = b;
	FillTree();
}

void KSceneSceneEditorDialogOranment::OnBnClickedCheckPvs()
{
	BOOL b = IsDlgButtonChecked(IDC_CHECK_PVS);
	g_bTypeEnable[IMAGE_PVS] = b;
	FillTree();
}
void KSceneSceneEditorDialogOranment::OnBnClickedCheckPointlight()
{
	BOOL b = IsDlgButtonChecked(IDC_CHECK_POINTLIGHT);
	g_bTypeEnable[IMAGE_POINTLIGHT] = b;
	FillTree();
}

void KSceneSceneEditorDialogOranment::OnBnClickedButtonCreatini()
{
	int nType = 0;
	TCHAR strDefaultPath[MAX_PATH];
	HRESULT hr = E_FAIL;
	IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
    TCHAR szAppDir[MAX_PATH];
	HTREEITEM hRoot = m_Tree.GetRootItem();
	KG_PROCESS_ERROR(hRoot);

	int nReturn = MessageBox("重新生成将会覆盖掉已经修改的属性, 确定.", "提示", MB_YESNO);
	KG_PROCESS_SUCCESS(nReturn == IDNO);
    
    pManager->GetDefWorkDirectory(szAppDir, MAX_PATH);
	sprintf_s(strDefaultPath, 
		MAX_PATH,
		"%sData\\public\\mesh default.ini",
		szAppDir);
	
	if (!::PathFileExists(strDefaultPath))
	{
		MessageBox("缺少文件mesh deault.ini");
		return;
	}

	HTREEITEM hNext = m_Tree.GetChildItem(hRoot);
	while (hNext)
	{
		CreateDeafultInI(hNext, strDefaultPath);		
		hNext = m_Tree.GetNextSiblingItem(hNext);
	}
	
Exit1:
Exit0:
	return;
}

void KSceneSceneEditorDialogOranment::CreateDeafultInI(HTREEITEM hItem, TCHAR* strDefaultPath)
{
	int nType = 0;
	TCHAR strFilePath[MAX_PATH];
	TCHAR strDriver[MAX_PATH];
	TCHAR strPath[MAX_PATH];
	TCHAR strFile[MAX_PATH];

	KG_PROCESS_ERROR(hItem);
	m_Tree.GetItemImage(hItem, nType, nType);
	switch (nType)
	{
	case IMAGE_MESH:
	case IMAGE_MODEL:
		{
			CString csTemp;
			GetItemFilePath(hItem, csTemp);
			_tcscpy_s(strFilePath, csTemp);
		}
		_splitpath_s(strFilePath,
			strDriver,
			MAX_PATH,
			strPath,
			MAX_PATH,
			strFile,
			MAX_PATH,
			NULL,
			0);
		sprintf_s(strFilePath, "%s%s%s.ini", strDriver, strPath, strFile);
		CopyFile(strDefaultPath, strFilePath, FALSE);
		break;
	}
	
	HTREEITEM hChild = m_Tree.GetChildItem(hItem);
	while (hChild)
	{
		CreateDeafultInI(hChild, strDefaultPath);
		hChild = m_Tree.GetNextSiblingItem(hChild);
	}
Exit0:
	return;
}

void KSceneSceneEditorDialogOranment::OnBnClickedCheckShownew()
{
	
}

void KSceneSceneEditorDialogOranment::GetItemFilePath(HTREEITEM hItem, CString& RetPath)
{
//    TCHAR szAppDir[MAX_PATH];
	TCHAR strFileName[MAX_PATH];
    IEKG3DEngineManager *pManager = g_GetEngineIEInterface();

	KG_PROCESS_ERROR(hItem);

	strcpy(strFileName, m_Tree.GetItemText(hItem));
	HTREEITEM hParentItem = m_Tree.GetParentItem(hItem);
	while (hParentItem)
	{
		RetPath.Format(_T("%s\\%s"), m_Tree.GetItemText(hParentItem), strFileName);
		_tcscpy_s(strFileName, RetPath);
		hParentItem = m_Tree.GetParentItem(hParentItem);
	}
    //pManager->GetDefWorkDirectory(szAppDir, MAX_PATH);
	RetPath.Format(_T("%s"),strFileName);
Exit0:
	return;
}

void KSceneSceneEditorDialogOranment::OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
    IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
	IEKG3DSceneObjectPlacementTool* lpObjPlacementTool = this->GetObjectPlacementToolInterface(m_lpSceneSceneEditor);
	KG_PROCESS_ERROR(NULL != pManager && NULL != lpObjPlacementTool /*&&NULL != m_lpObjPlacementTool*/);

	{
		//用HitTest检查鼠标碰撞到哪个Item，得到Image，从而判断类型
		HTREEITEM Item = NULL;
		INT nImage = 0;
		{
			NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

			CPoint ptMouse;
			GetCursorPos(&ptMouse);
			m_Tree.ScreenToClient(&ptMouse);

			Item = m_Tree.HitTest(ptMouse);
			KG_PROCESS_SUCCESS(Item == NULL);

			m_hLastItem = Item;
			m_Tree.GetItemImage(Item,nImage,nImage);
		}

		CString strFilePath;
		

		GetItemFilePath(Item, strFilePath);
		CEdit * pathText = (CEdit * )GetDlgItem(IDC_EDIT_SELECTED_NODE);
		pathText->SetWindowText(strFilePath);
		int   i   =   pathText->LineLength();   
		pathText->SetSel(i,i);

		

		switch(nImage)
		{
		case IMAGE_MESH:
		case IMAGE_MODEL:
			{
				_ASSERTE(m_lpSceneSceneEditor);

				BOOL bRet = IsDlgButtonChecked(IDC_CK_MODEL_ADDAS_ENTITY);
				if(bRet)//否则就按下面的Model的处理方式
				{
					//模拟Represent的处理方式
					bool bSuccessLoaded = false;
					IKG3DResourceManager* pModelManager = g_pEngineManager->Get3DModelTable();
					if (pModelManager)
					{
						IKG3DModel* p3DModel = NULL;
						HRESULT hr = pModelManager->LoadResourceFromFile(strFilePath, 0, 0, (IKG3DResourceBase**)&p3DModel);
						if (SUCCEEDED(hr))
						{
							_ASSERTE(m_lpSceneSceneEditor);
							IKG3DScene* pIScene = m_lpSceneSceneEditor->GetIInterface();
							hr = pIScene->AddRenderEntity(p3DModel, 0, TRUE,TRUE);
							if (SUCCEEDED(hr))
							{
								bSuccessLoaded = true;
							}
						}
					}
					if (!bSuccessLoaded)
					{
						MessageBox("载入模型失败");
					}
					break;
				}

			}
		case IMAGE_SPEEDTREE:
		case IMAGE_SFX:
		case IMAGE_POINTLIGHT:
			{
				IEKG3DRepresentObject* pObj = NULL;
				HRESULT hr = pManager->BuildKG3DRepresentObject(strFilePath, REPRESENTOBJECT_DEFAULT, 0, 0, &pObj);
				//HRESULT hr = pManager->BuildKG3DRepresentObject(NULL, REPRESENTOBJECT_NODE, 0,0, &pObj);
				KG_COM_PROCESS_ERROR(hr);
				hr = lpObjPlacementTool->SetObjectInHandle(pObj, TRUE);
				KG_COM_PROCESS_ERROR(hr);
				break;
			}
		case IMAGE_SET:
			{
				IEKG3DRepresentObject* pObj = NULL;
				HRESULT hr = pManager->BuildKG3DRepresentObject(strFilePath, REPRESENTOBJECT_SET, 0, 0, &pObj);
				
				KG_COM_PROCESS_ERROR(hr);
				hr = lpObjPlacementTool->SetObjectInHandle(pObj, TRUE);
				KG_COM_PROCESS_ERROR(hr);
				break;
				
			}
		case IMAGE_SOUNDBALL:
			{
				IEKG3DRepresentObject* pObj = NULL;
				HRESULT hr = pManager->BuildKG3DRepresentObject(strFilePath, REPRESENTOBJECT_SOUNDBALL, 0, 0, &pObj);
				KG_COM_PROCESS_ERROR(hr);
				hr = lpObjPlacementTool->SetObjectInHandle(pObj, TRUE);
				KG_COM_PROCESS_ERROR(hr);

				break;
			}
		case IMAGE_GROUP:
			{
				IEKG3DRepresentObject* pObj = NULL;
				HRESULT hr = pManager->BuildKG3DRepresentObject(strFilePath, REPRESENTOBJECT_DEFAULT, 0, 0, &pObj);
				KG_COM_PROCESS_ERROR(hr);
				hr = lpObjPlacementTool->SetObjectInHandle(pObj, TRUE);
				KG_COM_PROCESS_ERROR(hr);
				break;
			}
		case IMAGE_WALL:
			{
				IEKG3DRepresentObject* pObj = NULL;
				HRESULT hr = pManager->BuildKG3DRepresentObject(strFilePath, REPRESENTOBJECT_WALL, 0, 0, &pObj);
				KG_COM_PROCESS_ERROR(hr);
				hr = lpObjPlacementTool->SetObjectInHandle(pObj, TRUE);
				KG_COM_PROCESS_ERROR(hr);
				break;
			}
		case IMAGE_PVS :
			{
				IEKG3DRepresentObject* pObj = NULL;
				HRESULT hr = pManager->BuildKG3DRepresentObject(strFilePath, REPRESENTOBJECT_PVS, 0, 0, &pObj);
				//HRESULT hr = pManager->BuildKG3DRepresentObject(NULL, REPRESENTOBJECT_NODE, 0,0, &pObj);
				KG_COM_PROCESS_ERROR(hr);
				hr = lpObjPlacementTool->SetObjectInHandle(pObj, TRUE);
				KG_COM_PROCESS_ERROR(hr);
				break;
			}
		}
		HWND hBaseWnd;
		m_Tree.SelectItem(Item);
		pManager->GetBaseWnd(&hBaseWnd);
		::SetFocus(hBaseWnd);			
	}
Exit1:
Exit0:
	*pResult = 0;
}

IEKG3DSceneObjectPlacementTool* KSceneSceneEditorDialogOranment::GetObjectPlacementToolInterface(IEKG3DSceneSceneEditor* pEditor)
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
void KSceneSceneEditorDialogOranment::SetSceneEditor( IEKG3DSceneSceneEditor* pSceneEdit )
{
	if (pSceneEdit != m_lpSceneSceneEditor)
	{
		m_lpSceneSceneEditor = pSceneEdit;
	/*
		SAFE_RELEASE(m_lpObjPlacementTool);//当有新的场景的时候，旧的释放掉
			m_lpObjPlacementTool = this->GetObjectPlacementToolInterface(m_lpSceneSceneEditor);		*/
	
	}
}
KSceneSceneEditorDialogOranment::~KSceneSceneEditorDialogOranment()
{
	//SAFE_RELEASE(m_lpObjPlacementTool);
}

BOOL KSceneSceneEditorDialogOranment::CheckIsLODModel(std::string strFileName)
{
	size_t Length = strFileName.length();
	if(Length > 2)
	{
		char* pLod = &(strFileName[Length - 2]);
		if((_stricmp(pLod,"_L") == 0)||(_stricmp(pLod,"_M") == 0))
		   return TRUE;
	}
	return FALSE;
}
void KSceneSceneEditorDialogOranment::OnBnClickedPlacePlight()
{
    IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
    IEKG3DSceneObjectPlacementTool* lpObjPlacementTool = this->GetObjectPlacementToolInterface(m_lpSceneSceneEditor);
    KG_PROCESS_ERROR(NULL != pManager && NULL != lpObjPlacementTool /*&&NULL != m_lpObjPlacementTool*/);

    IEKG3DRepresentObject* pObj = NULL;
    HRESULT hr = pManager->BuildKG3DRepresentObject(PLIGHT_FILE_PATH, REPRESENTOBJECT_DEFAULT, 0, 0, &pObj);
    //HRESULT hr = pManager->BuildKG3DRepresentObject(NULL, REPRESENTOBJECT_NODE, 0,0, &pObj);
    KG_COM_PROCESS_ERROR(hr);
    hr = lpObjPlacementTool->SetObjectInHandle(pObj, TRUE);
    KG_COM_PROCESS_ERROR(hr);

Exit0 :
    return;
}

void KSceneSceneEditorDialogOranment::OnBnClickedButtonLookup()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_strSub=="")
	{
		MessageBox("查找内容为空，请输入!");
		return ;
	}
	m_FindNode = m_Tree.FindNextItembySubString(m_strSub,m_FindNode);
	KG_PROCESS_ERROR(m_FindNode);

	m_Tree.SelectItem(m_FindNode);
	((CButton*)(GetDlgItem(IDC_BUTTON_LOOKUP)))->SetWindowText("查找下一个");
	return ; 
Exit0:
	m_Tree.SelectItem(NULL);
	MessageBox("查找结束，没有找到任何合适的模型!");
	((CButton*)(GetDlgItem(IDC_BUTTON_LOOKUP)))->SetWindowText("开始查找");
}

void KSceneSceneEditorDialogOranment::OnEnChangeEditSubstring()
{
	GetDlgItemText(IDC_EDIT_SUBSTRING,m_strSub);
	((CButton*)(GetDlgItem(IDC_BUTTON_LOOKUP)))->SetWindowText("开始查找");
	m_FindNode = NULL;
}


void KSceneSceneEditorDialogOranment::OnBnClickedReplaceOneEntity()
{
	//替换单个实体
    m_lpSceneSceneEditor->ReplaceSelectedEntity(false);

}

void KSceneSceneEditorDialogOranment::OnBnClickedReplaceAllEntity()
{
	//替换全部实体
	m_lpSceneSceneEditor->ReplaceSelectedEntity(true);
}
