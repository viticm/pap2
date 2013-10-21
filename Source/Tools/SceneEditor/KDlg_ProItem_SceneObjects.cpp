// KDlg_ProItem_SceneObjects.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KDlg_ProItem_SceneObjects.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_SceneObjects dialog

namespace PropertyBase
{
KDlg_ProItem_SceneObjects::KDlg_ProItem_SceneObjects(CWnd* pParent /*=NULL*/)
	: CDialog(KDlg_ProItem_SceneObjects::IDD, pParent)
{
	//{{AFX_DATA_INIT(KDlg_ProItem_SceneObjects)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void KDlg_ProItem_SceneObjects::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KDlg_ProItem_SceneObjects)
	DDX_Control(pDX, IDC_TREE1, m_Tree);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KDlg_ProItem_SceneObjects, CDialog)
	//{{AFX_MSG_MAP(KDlg_ProItem_SceneObjects)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE1, OnDblclkTree1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_SceneObjects message handlers

void KDlg_ProItem_SceneObjects::OnButton() 
{
	// TODO: Add your control notification handler code here
	FillTree();
}

void KDlg_ProItem_SceneObjects::OnDblclkTree1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	//HTREEITEM Item = m_Tree.GetSelectedItem();
	//if(Item)
	//{
	//	int Image;
	//	m_Tree.GetItemImage(Item,Image,Image);
	//	
	//	if(Image==0)
	//	{
	//		TCHAR str[256];
	//		TCHAR FileName[256];
	//		wsprintf(str,"%s.Mesh",m_Tree.GetItemText(Item));

	//		HTREEITEM pParentItem = m_Tree.GetParentItem(Item);
	//		while((pParentItem)&&(pParentItem!=TVI_ROOT))
	//		{
	//			TCHAR DirName[256];
	//			wsprintf(DirName,"%s",m_Tree.GetItemText(pParentItem));
	//			wsprintf(FileName,"%s\\%s",DirName,str);
	//			wsprintf(str,"%s",FileName);
	//			pParentItem = m_Tree.GetParentItem(pParentItem);;
	//		}
	//		wsprintf(FileName,"%s%s",g_Def_ModelDirectory,str);

	//		if(m_lpProperty)
	//		{
	//			KSceneSceneEdit* pEdit = (KSceneSceneEdit*) m_lpProperty->Address;
	//			
	//			DWORD MeshID = 0;
	//			if (SUCCEEDED(g_cMeshTable.LoadFromFile(&MeshID,FileName)))
	//			{
	//				LPMODEL pMesh = NULL;
	//				g_cMeshTable.GetXMesh(&pMesh,MeshID);

	//				TCHAR path_buffer[256];
	//				TCHAR drive[256];
	//				TCHAR dir[256];
	//				TCHAR fname[256];
	//				TCHAR ext[256];

	//				wsprintf(path_buffer,"%s",FileName);
	//				_splitpath( path_buffer, drive, dir, fname, ext );
	//				wsprintf(str,"%s%s%s.Mtl",drive,dir,fname);
	//				pMesh->LoadMaterial(str);

	//				pMesh->m_dwShadow = 0;
	//				pEdit->m_dwState = EDITSTATE_PLACEOBJECT;

	//				g_cObjectTable.Get1NewObject(&pEdit->m_lpInHandObject,0);
	//				pEdit->m_lpInHandObject->AddMesh(MeshID);

	//				::SetFocus(hBaseWnd);
	//			}

	//			{
	//				TCHAR path_buffer[256];
	//				TCHAR drive[256];
	//				TCHAR dir[256];
	//				TCHAR fname[256];
	//				TCHAR ext[256];

	//				TCHAR DirName[256];
	//				TCHAR Name[256];

	//				wsprintf(path_buffer,"%s",FileName);
	//				_splitpath( path_buffer, drive, dir, fname, ext );
	//				wsprintf(DirName,"%s%s",drive,dir);
	//				wsprintf(Name,"%s%s",fname,ext);

	//				pEdit->SetInHandObjectFileDirectory(DirName,Name);
	//			}
	//		}
	//	}
	//	
	//}
	*pResult = 0;
}



BOOL KDlg_ProItem_SceneObjects::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	FillTree();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

HRESULT KDlg_ProItem_SceneObjects::FillTree()
{
	m_Tree.DeleteAllItems();

	//FillTreeWithDir(g_Def_ModelDirectory,TVI_ROOT);
	return S_OK;
}

HRESULT KDlg_ProItem_SceneObjects::FillTreeWithDir(LPSTR Dir,HTREEITEM hStart)
{
	TCHAR path_buffer[256];
	TCHAR drive[256];
	TCHAR dir[256];
	TCHAR fname[256];
	TCHAR ext[256];

	WIN32_FIND_DATA Find;
	TCHAR str[256];
	wsprintf(str,"%s\\*.*",Dir);

	HANDLE hFind = FindFirstFile(str,&Find);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		wsprintf(path_buffer,"%s",Find.cFileName);
		_splitpath( path_buffer, drive, dir, fname, ext );
		wsprintf(str,"%s%s",fname,ext);

		if((_strcmpi(path_buffer,".")!=0)&&(_strcmpi(path_buffer,"..")!=0))
		{
			wsprintf(str,"%s%s",Dir,path_buffer);
			DWORD Attrib = GetFileAttributes(str);

			if((Attrib!=0xFFFFFFFF)&&(Attrib&FILE_ATTRIBUTE_DIRECTORY))
			{
				HTREEITEM hItem = m_Tree.InsertItem(fname,1,1,hStart);
				wsprintf(path_buffer,"%s\\",str);
				FillTreeWithDir(path_buffer,hItem);
			}
			else if(_strcmpi(ext,".Mesh")==0)
			{
				HTREEITEM hItem = m_Tree.InsertItem(fname,0,0,hStart);
			}
		}

		while(FindNextFile(hFind,&Find))
		{
			wsprintf(path_buffer,"%s",Find.cFileName);
			_splitpath( path_buffer, drive, dir, fname, ext );
			wsprintf(str,"%s%s",fname,ext);
			if((_strcmpi(path_buffer,".")!=0)&&(_strcmpi(path_buffer,"..")!=0))
			{
				wsprintf(str,"%s%s",Dir,path_buffer);
				DWORD Attrib = GetFileAttributes(str);

				if((Attrib!=0xFFFFFFFF)&&(Attrib&FILE_ATTRIBUTE_DIRECTORY))
				{
					HTREEITEM hItem = m_Tree.InsertItem(fname,1,1,hStart);
					wsprintf(path_buffer,"%s\\",str);
					FillTreeWithDir(path_buffer,hItem);
				}
				else if(_strcmpi(ext,".Mesh")==0)
				{
					HTREEITEM hItem = m_Tree.InsertItem(fname,0,0,hStart);
				}
			}
		}
		FindClose(hFind);
	}
	return S_OK;
}
}
