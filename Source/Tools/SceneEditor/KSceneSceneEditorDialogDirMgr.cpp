
// KSceneSceneEditorDialogDirMgr.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSceneEditorDialogDirMgr.h"
//#include ".\kg3denginemanager.h"
#include "KG3DSceneEditorDirectory.h"
//#include "KG3DRepresentObjectSet.h"
//#include ".\kg3drepresentobjecttable.h"
//#include "KG3DSceneSceneEditor.h"
#include "IEEditor.h"
//#include "KG3DObjectPropertyDefine.h"
#include "KSceneSceneEditorDialogOranment.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

#define DIR_MGR_DATA_BASE_PATH	"Data"


// KSceneSceneEditorDialogDirMgr dialog

IMPLEMENT_DYNAMIC(KSceneSceneEditorDialogDirMgr, CDialog)


KSceneSceneEditorDialogDirMgr::KSceneSceneEditorDialogDirMgr(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneSceneEditorDialogDirMgr::IDD, pParent)
{
	m_lpObjectSet = NULL;
	m_lpObject = NULL;
	m_lpSceneEditor = NULL;
	m_nOutputWindowID = 0;
}

KSceneSceneEditorDialogDirMgr::~KSceneSceneEditorDialogDirMgr()
{
}

void KSceneSceneEditorDialogDirMgr::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_Tree);
	DDX_Control(pDX, IDC_STATIC_PREVIEW, m_Static_Preview);
	DDX_Control(pDX, IDC_LIST_DIRS, m_ListBox_Dir);
	DDX_Control(pDX, IDC_LIST_MODELS, m_ListBox_Model);
	DDX_Control(pDX, IDC_STATIC_SETNAME, m_Static_SetName);
}


BEGIN_MESSAGE_MAP(KSceneSceneEditorDialogDirMgr, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &KSceneSceneEditorDialogDirMgr::OnTvnSelchangedTree1)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &KSceneSceneEditorDialogDirMgr::OnBnClickedButtonRefresh)
	ON_NOTIFY(TVN_BEGINDRAG, IDC_TREE1, &KSceneSceneEditorDialogDirMgr::OnTvnBegindragTree1)
	ON_BN_CLICKED(IDC_BUTTON_ADDDIR, &KSceneSceneEditorDialogDirMgr::OnBnClickedButtonAdddir)
	ON_BN_CLICKED(IDC_BUTTON_REMOVEDIR, &KSceneSceneEditorDialogDirMgr::OnBnClickedButtonRemovedir)
	ON_BN_CLICKED(IDC_BUTTON_NEWSE, &KSceneSceneEditorDialogDirMgr::OnBnClickedButtonNewse)
	ON_BN_CLICKED(IDC_BUTTON_ADDTOSET, &KSceneSceneEditorDialogDirMgr::OnBnClickedButtonAddtoset)
	ON_BN_CLICKED(IDC_BUTTON_SAVESET, &KSceneSceneEditorDialogDirMgr::OnBnClickedButtonSaveset)
	ON_BN_CLICKED(IDC_BUTTON_LOADSET, &KSceneSceneEditorDialogDirMgr::OnBnClickedButtonLoadset)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_REMOVEFROMSET, &KSceneSceneEditorDialogDirMgr::OnBnClickedButtonRemovefromset)
	ON_LBN_SELCHANGE(IDC_LIST_MODELS, &KSceneSceneEditorDialogDirMgr::OnLbnSelchangeListModels)
	ON_BN_CLICKED(IDC_BUTTON_REMOVEFORMTREE, &KSceneSceneEditorDialogDirMgr::OnBnClickedButtonRemoveformtree)
    ON_NOTIFY(TVN_ITEMEXPANDING, IDC_TREE1, &KSceneSceneEditorDialogDirMgr::OnTvnItemExpanding)
END_MESSAGE_MAP()


// KSceneSceneEditorDialogDirMgr message handlers

void KSceneSceneEditorDialogDirMgr::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	TCHAR Name[256];
	TCHAR FileName[256];

	if(!m_lpSceneEditor)
		return;

	int nImage = IMAGE_ERROR;
	HTREEITEM hItem = m_Tree.GetSelectedItem();

	nImage = GetItemFullText(hItem,Name);

	switch(nImage)
	{
	case IMAGE_MESH:
	case IMAGE_MODEL:
	case IMAGE_SPEEDTREE:
	case IMAGE_SFX:
		{
			wsprintf(FileName,"%s%s",g_szDefWorkDirectory,Name);

			IEKG3DRepresentObject* pObj = NULL;
			HRESULT hr = g_pEngineManager->BuildKG3DRepresentObject(FileName, REPRESENTOBJECT_DEFAULT, 0, 0, &pObj);
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
	}
}


void KSceneSceneEditorDialogDirMgr::FillTree()
{
	m_Tree.DeleteAllItems();

	TCHAR Name[256];
	wsprintf(Name,"%s", DIR_MGR_DATA_BASE_PATH);
	HTREEITEM hItem = m_Tree.InsertItem(Name,IMAGE_DIR,IMAGE_DIR,TVI_ROOT);

	FillTreeWithDir(Name, hItem);
}

int KSceneSceneEditorDialogDirMgr::GetItemImage(LPSTR extName)
{
	if(_strcmpi(extName,".Mesh")==0)
		return IMAGE_MESH;
	else if (_strcmpi(extName,".STREE")==0)
		return IMAGE_SPEEDTREE;
	else if(_strcmpi(extName,".Sfx")==0)
		return IMAGE_SFX;
	else if(_strcmpi(extName, ".mdl") == 0)
		return IMAGE_MODEL;
	else
		return IMAGE_ERROR;
}

HRESULT KSceneSceneEditorDialogDirMgr::FillTreeWithDir(LPSTR Dir,HTREEITEM hStart)
{
	//TCHAR path_buffer[256];
	TCHAR drive[256];
	TCHAR dir[256];
	TCHAR fname[256];
	TCHAR ext[256];

	WIN32_FIND_DATA Find;

	TCHAR szFullPath[MAX_PATH];
	TCHAR szSingleFile[MAX_PATH];

	wsprintf(szFullPath,"%s%s\\*.*", g_szDefWorkDirectory, Dir);

	HANDLE hFind = FindFirstFile(szFullPath, &Find);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		strcpy(szSingleFile, Find.cFileName);

		_splitpath(szSingleFile, drive, dir, fname, ext );

		if ((_strcmpi(szSingleFile, ".") != 0) 
			&& (_strcmpi(szSingleFile, "..") != 0)
			&& (_strcmpi(szSingleFile, ".svn")))
		{
			if ((Find.dwFileAttributes != 0xFFFFFFFF)
				&&(Find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				HTREEITEM hItem = m_Tree.InsertItem(szSingleFile, IMAGE_DIR,IMAGE_DIR, hStart);
                //插入一错误项，说明hItem这项对应的目录还未被搜索过
                m_Tree.InsertItem(szSingleFile, IMAGE_ERROR, IMAGE_DIR, hItem);
			}
			else
			{
				int n = GetItemImage(ext);
				if (n != IMAGE_ERROR)
				{
					HTREEITEM hItem = m_Tree.InsertItem(szSingleFile, n, n, hStart);
				}
			}
		}

		while(FindNextFile(hFind, &Find))
		{
			strcpy(szSingleFile, Find.cFileName);
			_splitpath(szSingleFile, drive, dir, fname, ext );

			int nDebug = _strcmpi(szSingleFile, ".svn");
			if((_strcmpi(szSingleFile, ".") != 0) && 
				(_strcmpi(szSingleFile, "..") != 0) &&
				(_strcmpi(szSingleFile, ".svn")))
			{

				if((Find.dwFileAttributes != 0xFFFFFFFF)
					&&(Find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					HTREEITEM hItem = m_Tree.InsertItem(szSingleFile, IMAGE_DIR,IMAGE_DIR, hStart);
                    //插入一错误项，说明hItem这项对应的目录还未被搜索过
                    m_Tree.InsertItem(szSingleFile, IMAGE_ERROR, IMAGE_DIR, hItem);
				}
				else
				{
					int n = GetItemImage(ext);
					if (n != IMAGE_ERROR)
					{
						HTREEITEM hItem = m_Tree.InsertItem(szSingleFile, n, n, hStart);
					}
				}
			}
		}
		FindClose(hFind);
	}
	return S_OK;
}

BOOL KSceneSceneEditorDialogDirMgr::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_imgList.Create( IDB_BITMAP_ORAN, 24, 1, RGB( 128, 128, 128 ) );
	m_Tree.SetImageList(&m_imgList,TVSIL_NORMAL);

	FillDirListBox();

	m_ListBox_Model.EnableWindow(FALSE);
	m_Static_SetName.EnableWindow(FALSE);
	// TODO:  Add extra initialization here
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneSceneEditorDialogDirMgr::OnBnClickedButtonRefresh()
{
	FillTree();
	// TODO: Add your control notification handler code here
}

void KSceneSceneEditorDialogDirMgr::OnTvnBegindragTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void KSceneSceneEditorDialogDirMgr::OnBnClickedButtonAdddir()
{
	// TODO: Add your control notification handler code here
	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem)
		return;

	int nImage = IMAGE_ERROR;
	m_Tree.GetItemImage(hItem,nImage,nImage);
	if(nImage==IMAGE_DIR)
	{
		CString ItemName = m_Tree.GetItemText(hItem);
		TCHAR Name[256];
		wsprintf(Name,"");
		TCHAR Title[256];
		wsprintf(Title,"%s",ItemName);

		while(hItem)
		{
			CString ItemName = m_Tree.GetItemText(hItem);
			TCHAR TempName[256];
			wsprintf(TempName,"%s\\%s", ItemName, Name);
			wsprintf(Name,"%s",TempName);

			hItem = m_Tree.GetParentItem(hItem);
		}
		g_cEditorDirectory.AddDirectory(Title, Name);
		FillDirListBox();
	}
}

void KSceneSceneEditorDialogDirMgr::FillDirListBox()
{
	{
		int n = m_ListBox_Dir.GetCount();
		for(int i=0;i<n;i++)
		{
			m_ListBox_Dir.DeleteString(0);
		}
	}

	int nCount = g_cEditorDirectory.GetNumofDirectory();
	for(int i=0;i<nCount;i++)
	{
		TCHAR Title[256];
		TCHAR Name[256];
		g_cEditorDirectory.GetDirectory(i,Title,Name);
		TCHAR str[256];
		wsprintf(str,"%s : %s",Title,Name);
		int nIndex = m_ListBox_Dir.AddString(str);
		m_ListBox_Dir.SetItemData(nIndex,i);
	}
}

void KSceneSceneEditorDialogDirMgr::FillSetModelListBox()
{
	{
		int n = m_ListBox_Model.GetCount();
		for(int i=0;i<n;i++)
		{
			m_ListBox_Model.DeleteString(0);
		}
	}

	if(m_lpObjectSet)
	{
		//IEKG3DModel* pModel = (IEKG3DModel*)m_lpObjectSet;
		IEKG3DRepresentObjectSet* pSet = (IEKG3DRepresentObjectSet*)(m_lpObjectSet);
		//pModel->GetIEKG3DModelSet(&pSet);

		int nCount = pSet->GetNumModelofSet();
		for(int i=0;i<nCount;i++)
		{
			TCHAR MeshName[256];
			TCHAR MtlName[256];
			TCHAR AniName[256];
			ZeroMemory(MeshName,sizeof(TCHAR)*256);
			ZeroMemory(MtlName,sizeof(TCHAR)*256);
			ZeroMemory(AniName,sizeof(TCHAR)*256);

			pSet->GetModelInfo(i,MeshName,MtlName,AniName);

			TCHAR str[256];
			wsprintf(str,"%s : %s : %s",MeshName,MtlName,AniName);
			int nIndex = m_ListBox_Model.AddString(str);
			m_ListBox_Model.SetItemData(nIndex,i);
		}
	}
}

void KSceneSceneEditorDialogDirMgr::OnBnClickedButtonRemovedir()
{
	// TODO: Add your control notification handler code here
	int n = m_ListBox_Dir.GetCount();
	for(int i=0;i<n;i++)
	{
		int sel = m_ListBox_Dir.GetSel(i);
		if(sel)
		{
			g_cEditorDirectory.DeleteDirectory(i);
		}
	}
	FillDirListBox();
}

void KSceneSceneEditorDialogDirMgr::OnBnClickedButtonNewse()
{
	HRESULT hr = S_OK;
	// TODO: Add your control notification handler code here
	SAFE_RELEASE(m_lpObject);
	IKG3DResourceManager* pObjectTable = NULL;
	g_pEngineManager->GetRepresentObjectTable(&pObjectTable);

	//指针转换错误，使用Manger进行一下过渡
	//hr = pObjectTable->NewOneResourse(REPRESENTOBJECT_SET,(IKG3DResourceBase**)&m_lpObjectSet);
	//hr = g_pEngineManager->NewKG3DRepresentObjectSetInTable(&m_lpObjectSet);
	hr = g_pEngineManager->NewKG3DRepresentObject(REPRESENTOBJECT_SET, 0, 0, &m_lpObject);
	KGLOG_COM_PROCESS_ERROR(hr);
	//hr = m_lpObjectSet->GetIERepresentObject(&m_lpObject);
	hr = m_lpObject->GetAnotherIEInterface(REPRESENTOBJECT_SET, (VOID**)&m_lpObjectSet);
	_ASSERTE(NULL != m_lpObjectSet);
	KGLOG_COM_PROCESS_ERROR(hr);

	m_ListBox_Model.EnableWindow(TRUE);
	m_Static_SetName.EnableWindow(TRUE);
	m_Static_SetName.SetWindowText("未命名的模型集合");


	FillSetModelListBox();
	return;
Exit0:
	return;
}

int KSceneSceneEditorDialogDirMgr::GetItemFullText(HTREEITEM hItem,LPSTR pFullName)
{
	int nImage = IMAGE_ERROR;
	m_Tree.GetItemImage(hItem,nImage,nImage);

	CString ItemName = m_Tree.GetItemText(hItem);
	TCHAR Name[256];
	wsprintf(Name,"%s",ItemName);
	HTREEITEM hItemParent = m_Tree.GetParentItem(hItem);
	while(hItemParent)
	{
		CString ItemName = m_Tree.GetItemText(hItemParent);
		TCHAR TempName[256];
		wsprintf(TempName,"%s\\%s",ItemName,Name);
		wsprintf(Name,"%s",TempName);

		hItemParent = m_Tree.GetParentItem(hItemParent);
	}

	wsprintf(pFullName,"%s",Name);
	return nImage;
}

void KSceneSceneEditorDialogDirMgr::OnBnClickedButtonAddtoset()
{
	// TODO: Add your control notification handler code here
	if(!m_lpObjectSet)
		return;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(!hItem)
		return;

	int nImage = IMAGE_ERROR;
	m_Tree.GetItemImage(hItem,nImage,nImage);
	switch(nImage)
	{
	case IMAGE_MESH:
	case IMAGE_MODEL:
	case IMAGE_SPEEDTREE:
	case IMAGE_SFX:
		{
			CString ItemName = m_Tree.GetItemText(hItem);
			TCHAR Name[256];
			wsprintf(Name,"%s",ItemName);
			HTREEITEM hItemParent = m_Tree.GetParentItem(hItem);
			while(hItemParent)
			{
				CString ItemName = m_Tree.GetItemText(hItemParent);
				TCHAR TempName[256];
				wsprintf(TempName,"%s\\%s",ItemName,Name);
				wsprintf(Name,"%s",TempName);

				hItemParent = m_Tree.GetParentItem(hItemParent);
			}

			TCHAR strDrive[MAX_PATH];
			TCHAR strPath[MAX_PATH];
			TCHAR strFile[MAX_PATH];
			TCHAR strExt[MAX_PATH];
//			TCHAR strFileName[MAX_PATH];

			_splitpath(Name, strDrive, strPath, strFile, strExt);

			TCHAR MtlName[MAX_PATH];
			wsprintf(MtlName,"%s%s%s.Mtl",strDrive,strPath,strFile);
			
			TCHAR AniName[MAX_PATH];
			wsprintf(AniName,"%s%s%s.Ani",strDrive,strPath,strFile);
			if(!g_IsFileExist(AniName))
				wsprintf(AniName,"");
			int nIndex  = 0;

			IEKG3DRepresentObjectSet* pSet = (m_lpObjectSet);

			pSet->AddModelInfo(nIndex,Name,MtlName,AniName);
			FillSetModelListBox();

			break;
		}
	case IMAGE_DIR:
		{
			HTREEITEM ItemNext = m_Tree.GetNextItem(hItem,TVGN_CHILD);
			while(ItemNext)
			{
				{
					CString ItemName = m_Tree.GetItemText(ItemNext);
					TCHAR Name[256];
					wsprintf(Name,"%s",ItemName);
					HTREEITEM hItemParent = m_Tree.GetParentItem(ItemNext);
					while(hItemParent)
					{
						CString ItemName = m_Tree.GetItemText(hItemParent);
						TCHAR TempName[256];
						wsprintf(TempName,"%s\\%s",ItemName,Name);
						wsprintf(Name,"%s",TempName);

						hItemParent = m_Tree.GetParentItem(hItemParent);
					}

					TCHAR strDrive[MAX_PATH];
					TCHAR strPath[MAX_PATH];
					TCHAR strFile[MAX_PATH];
					TCHAR strExt[MAX_PATH];
//					TCHAR strFileName[MAX_PATH];

					_splitpath(Name, strDrive, strPath, strFile, strExt);

					TCHAR MtlName[MAX_PATH];
					wsprintf(MtlName,"%s%s%s.Mtl",strDrive,strPath,strFile);
					TCHAR AniName[MAX_PATH];

					wsprintf(AniName,"%s%s%s.Ani",strDrive,strPath,strFile);
					if(!g_IsFileExist(AniName))
						wsprintf(AniName,"");

					int nIndex  = 0;

					IEKG3DRepresentObjectSet* pSet = (m_lpObjectSet);

					pSet->AddModelInfo(nIndex,Name,MtlName,AniName);
					FillSetModelListBox();
				}
				ItemNext = m_Tree.GetNextItem(ItemNext,TVGN_NEXT);
			}
			break;
		}
	}
}

void KSceneSceneEditorDialogDirMgr::OnBnClickedButtonSaveset()
{
	// TODO: Add your control notification handler code here
	if(!m_lpObjectSet)
		return ;

	CFileDialog dlg(false, "ini", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "剑网3D模型集合文件(*.ModelSet)|*.ModelSet||");
	if(dlg.DoModal() != IDOK)
	{
		return;
	}

	TCHAR Name[256];
	wsprintf(Name,"%s",dlg.GetPathName());
	m_lpObject->SaveToIniFile(Name,0);
}

void KSceneSceneEditorDialogDirMgr::OnBnClickedButtonLoadset()
{
	HRESULT hr = S_OK;
	// TODO: Add your control notification handler code here
	CFileDialog dlg(true, "ModelSet", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "剑网3D模型集合文件(*.ModelSet)|*.ModelSet||");
	if(dlg.DoModal() != IDOK)
	{
		return;
	}

	TCHAR Name[256];
	wsprintf(Name,"%s",dlg.GetPathName());

	SAFE_RELEASE(m_lpObject);

	IKG3DResourceManager* pObjectTable = NULL;
	g_pEngineManager->GetRepresentObjectTable(&pObjectTable);

	//hr = pObjectTable->NewOneResourse(REPRESENTOBJECT_SET,(IKG3DResourceBase**)&m_lpObjectSet);
	//hr = g_pEngineManager->NewKG3DRepresentObjectSetInTable(&m_lpObjectSet);
	hr = g_pEngineManager->BuildKG3DRepresentObject(Name, REPRESENTOBJECT_SET, 0, 0, &m_lpObject);
	KGLOG_COM_PROCESS_ERROR(hr);
	//hr = m_lpObjectSet->GetIERepresentObject(&m_lpObject);
	hr = m_lpObject->GetAnotherIEInterface(REPRESENTOBJECT_SET, (VOID**)&m_lpObjectSet);
	_ASSERTE(NULL != m_lpObjectSet);
	KGLOG_COM_PROCESS_ERROR(hr);

	KGLOG_COM_PROCESS_ERROR(hr);

	m_ListBox_Model.EnableWindow(TRUE);
	m_Static_SetName.EnableWindow(TRUE);
	m_Static_SetName.SetWindowText(Name);

	//m_lpObject->LoadFromIniFile(Name,0);
	FillSetModelListBox();
Exit0:
	return;
}


void KSceneSceneEditorDialogDirMgr::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);
	if(m_lpSceneEditor)
	{
		if(bShow)
		{
			m_nOutputWindowID = 0;
			m_lpSceneEditor->AddOutputWindow("ScenePreView",m_Static_Preview.GetSafeHwnd(),
				OUTPUTWND_SCENEEDITOR_OBJECTPREVEIW,0,&m_nOutputWindowID);
		}
		else
		{
			m_lpSceneEditor->RemoveOutputWindow(m_nOutputWindowID);
		}
	}
	// TODO: Add your message handler code here
}

void KSceneSceneEditorDialogDirMgr::OnBnClickedButtonRemovefromset()
{
	// TODO: Add your control notification handler code here
	int n = m_ListBox_Model.GetCurSel();
	IEKG3DRepresentObjectSet* pSet = (m_lpObjectSet);
	pSet->DeleteModelInfo(n);
	FillSetModelListBox();
}



void KSceneSceneEditorDialogDirMgr::OnLbnSelchangeListModels()
{
	if(!m_lpSceneEditor)
		return;
	// TODO: Add your control notification handler code here
	int n = m_ListBox_Model.GetCurSel();
	TCHAR MeshName[256];
	TCHAR MtlName[256];
	TCHAR AniName[256];
	TCHAR FileName[256];

	IEKG3DRepresentObjectSet* pSet = (m_lpObjectSet);
	HRESULT hr = pSet->GetModelInfo(n,MeshName,MtlName,AniName);
	if(SUCCEEDED(hr))
	{
		wsprintf(FileName,"%s%s",g_szDefWorkDirectory,MeshName);

		IEKG3DRepresentObject* pObj = NULL;
		HRESULT hr = g_pEngineManager->BuildKG3DRepresentObject(FileName, REPRESENTOBJECT_DEFAULT, 0, 0, &pObj);
		if (SUCCEEDED(hr))
		{
			hr = m_lpSceneEditor->SetObjectInHandle(pObj, TRUE);
			if (FAILED(hr))
			{
				SAFE_RELEASE(pObj);
			}
		}
	}
}

void KSceneSceneEditorDialogDirMgr::RemoveItemInListFromTree(HTREEITEM hItem)
{
	TCHAR Name[256];
	int nImage = GetItemFullText(hItem,Name);
	switch(nImage)
	{
	case IMAGE_MESH:
	case IMAGE_MODEL:
	case IMAGE_SPEEDTREE:
	case IMAGE_SFX:
		{
			IEKG3DRepresentObjectSet* pSet = (m_lpObjectSet);

			for(int i=0;i<pSet->GetNumModelofSet();i++)
			{
				TCHAR MeshName[256];
				TCHAR MtlName[256];
				TCHAR AniName[256];
//				TCHAR FileName[256];

				IEKG3DRepresentObjectSet* pSet = (m_lpObjectSet);
				HRESULT hr = pSet->GetModelInfo(i,MeshName,MtlName,AniName);

				if(_strcmpi(MeshName,Name)==0)
				{
					HTREEITEM hItemNext = m_Tree.GetNextItem(hItem,TVGN_NEXT);
					if(hItemNext)
						RemoveItemInListFromTree(hItemNext);
					m_Tree.DeleteItem(hItem);
					break;
				}
			}
			HTREEITEM hItemNext = m_Tree.GetNextItem(hItem,TVGN_NEXT);
			if(hItemNext)
				RemoveItemInListFromTree(hItemNext);
			break;
		}
	case IMAGE_DIR:
		{
			HTREEITEM hItemChild = m_Tree.GetChildItem(hItem);
			if(hItemChild)
				RemoveItemInListFromTree(hItemChild);
			HTREEITEM hItemNext = m_Tree.GetNextItem(hItem,TVGN_NEXT);
			if(hItemNext)
				RemoveItemInListFromTree(hItemNext);
			break;
		}
	}
}

void KSceneSceneEditorDialogDirMgr::OnBnClickedButtonRemoveformtree()
{
	// TODO: Add your control notification handler code here
	if(!m_lpObjectSet)
		return;
	HTREEITEM hitem = m_Tree.GetChildItem(TVI_ROOT);
	RemoveItemInListFromTree(hitem);
}

void KSceneSceneEditorDialogDirMgr::OnTvnItemExpanding(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

    TCHAR Name[256];
    int nImage = IMAGE_ERROR;
    HTREEITEM hItem = pNMTreeView->itemNew.hItem;

    nImage = GetItemFullText(hItem,Name);
    KG_PROCESS_SUCCESS(nImage != IMAGE_DIR);

    if (m_Tree.ItemHasChildren(hItem))
    {
        int nItemImage = IMAGE_ERROR;
        int nItemImageSelect = IMAGE_ERROR;
        HTREEITEM hChildItem = m_Tree.GetChildItem(hItem);

        m_Tree.GetItemImage(hChildItem, nItemImage, nItemImageSelect);
        //有错误项，说明hItem这项对应的目录还未被搜索过，刷新它
        if (nItemImage != nItemImageSelect)
        {
            m_Tree.DeleteItem(hChildItem);
            FillTreeWithDir(Name, hItem);
        }
    }

Exit1:
    *pResult = 0;
}
