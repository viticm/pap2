// KDlgSoundSource.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KDlgSoundSource.h"
#include "IKG3DSoundShell.h"

// KDlgSoundSource dialog

IMPLEMENT_DYNAMIC(KDlgSoundSource, CDialog)

KDlgSoundSource::KDlgSoundSource(CWnd* pParent /*=NULL*/)
: CDialog(KDlgSoundSource::IDD, pParent)
{
	m_pszTreeTouch = NULL;
}

KDlgSoundSource::~KDlgSoundSource()
{
}

void KDlgSoundSource::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_SOUNDSOURCE, m_Tree);
}


BEGIN_MESSAGE_MAP(KDlgSoundSource, CDialog)
	ON_NOTIFY(NM_CLICK, IDC_TREE_SOUNDSOURCE, &KDlgSoundSource::OnNMClickTreeSoundsource)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &KDlgSoundSource::OnBnClickedButtonRefresh)
	ON_BN_CLICKED(IDC_BUTTON_OK, &KDlgSoundSource::OnBnClickedButtonOk)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// KDlgSoundSource message handlers


HRESULT KDlgSoundSource::FillTree()
{// 填充树型控件
	HRESULT hResult = E_FAIL;
	HRESULT hRetCode = E_FAIL;
	TCHAR szDirectory[MAX_PATH];
	sprintf_s(szDirectory,"data\\sound");
	m_Tree.DeleteAllItems();
	HTREEITEM hitem = m_Tree.InsertItem(szDirectory,IMAGE_DIR,IMAGE_DIR,TVI_ROOT);
	hRetCode = FillTreeWithDir(szDirectory,".wav",hitem);
	KG_COM_PROCESS_ERROR(hRetCode);
	UpdateData(FALSE);
	hResult = S_OK;
Exit0:
	return hResult;
}
int KDlgSoundSource::GetItemImage(LPSTR extName)
{
	if(_strcmpi(extName,".wav")==0)
		return IMAGE_WAV;
	return IMAGE_ERROR;
}
HRESULT KDlgSoundSource::FillTreeWithDir(LPSTR Dir,LPSTR pFileType,HTREEITEM hStart)
{// 递归填充子结构
	TCHAR drive[MAX_PATH];
	TCHAR dir[MAX_PATH];
	TCHAR fname[MAX_PATH];
	TCHAR ext[MAX_PATH];
	WIN32_FIND_DATA Find;
	TCHAR szFullPath[MAX_PATH];
	TCHAR szSingleFile[MAX_PATH];
	TCHAR szHalfPath[MAX_PATH];
	wsprintf(szFullPath,"%s%s\\*.*",g_szDefWorkDirectory,Dir);
	HANDLE hFind = FindFirstFile(szFullPath,&Find);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		strcpy_s(szSingleFile,Find.cFileName);
		_splitpath_s(szSingleFile,drive,dir,fname,ext);
		if(_strcmpi(szSingleFile,".") != 0 && _strcmpi(szSingleFile,"..") != 0 
			&& _strcmpi(szSingleFile,".svn") != 0)
		{
			if(Find.dwFileAttributes != 0xFFFFFFFF 
				&& (Find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				HTREEITEM hItem = m_Tree.InsertItem(szSingleFile,IMAGE_DIR,IMAGE_DIR,hStart);
				wsprintf(szHalfPath,"%s\\%s",Dir,szSingleFile);
				FillTreeWithDir(szHalfPath,pFileType,hItem);
			}
			else
			{
				int n = GetItemImage(ext);
				if(n != IMAGE_ERROR)
				{
					HTREEITEM hItem = m_Tree.InsertItem(szSingleFile,n,n,hStart);
				}
			}
		} 
		while (FindNextFile(hFind,&Find))
		{
			strcpy_s(szSingleFile,Find.cFileName);
			_splitpath_s(szSingleFile,drive,dir,fname,ext);
			if(_strcmpi(szSingleFile,".") != 0 && _strcmpi(szSingleFile,"..") != 0 
				&& _strcmpi(szSingleFile,".svn") != 0)
			{
				if(Find.dwFileAttributes != 0xFFFFFFFF 
					&& (Find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					HTREEITEM hItem = m_Tree.InsertItem(szSingleFile,IMAGE_DIR,IMAGE_DIR,hStart);
					wsprintf(szHalfPath,"%s\\%s",Dir,szSingleFile);
					FillTreeWithDir(szHalfPath,pFileType,hItem);
				}
				else
				{
					int n = GetItemImage(ext);
					if(n != IMAGE_ERROR)
					{
						HTREEITEM hItem = m_Tree.InsertItem(szSingleFile,n,n,hStart);
					}
				}
			} 
		}
		FindClose(hFind);
	}
	return S_OK;
}
BOOL KDlgSoundSource::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_imgList.Create(18, 18, ILC_COLOR24, 8, 8);
	m_imgList.Add(AfxGetApp()->LoadIcon(IDC_DIR));
	m_imgList.Add(AfxGetApp()->LoadIcon(IDC_SOUND));
	m_Tree.SetImageList(&m_imgList,TVSIL_NORMAL);
	FillTree();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void KDlgSoundSource::OnNMClickTreeSoundsource(NMHDR *pNMHDR, LRESULT *pResult)
{// 点击m_tree
	// TODO: Add your control notification handler code here
	*pResult = 0;

	/*
	HTREEITEM hItem = NULL;
	int nImage = 0;
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	hItem = m_Tree.GetSelectedItem();
	KG_PROCESS_SUCCESS(hItem == NULL);
	m_Tree.GetItemImage(hItem,nImage,nImage);
	*/

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	CPoint ptMouse;
	GetCursorPos(&ptMouse);
	m_Tree.ScreenToClient(&ptMouse);
	HTREEITEM Item = NULL;
	INT nImage = 0;
	Item = m_Tree.HitTest(ptMouse);
	KG_PROCESS_SUCCESS(Item == NULL);

	m_Tree.GetItemImage(Item,nImage,nImage);

	GetItemFilePath(Item);
	if(nImage == IMAGE_WAV)
	{

		g_pSoundShell->StopAllSound();
		g_pSoundShell->PlaySound(1, m_szTreeTouch, 0, 1.0f, FALSE, NULL);

	}
	// m_Tree.GetItemImage(hItem,nImage,nImage);

	if (nImage == IMAGE_WAV)
	{
		m_bIsWav = TRUE;
	}
	else
	{
		m_bIsWav = FALSE;
	}
Exit0:
	;
Exit1:
	;
}
void KDlgSoundSource::GetItemFilePath(HTREEITEM hItem)
{// 获取完整的文件名
	TCHAR szFileName[MAX_PATH];
	int nImage;
	KG_PROCESS_ERROR(hItem);

	strcpy(szFileName, m_Tree.GetItemText(hItem));
	HTREEITEM hParentItem = m_Tree.GetParentItem(hItem);
	while (hParentItem)
	{
		sprintf_s(m_szTreeTouch ,"%s\\%s",m_Tree.GetItemText(hParentItem) ,szFileName );
		//_tcscpy_s(szFileName, m_szTreeTouch);
		strcpy_s(szFileName,m_szTreeTouch);
		hParentItem = m_Tree.GetParentItem(hParentItem);
	}
	m_Tree.GetItemImage(hItem,nImage,nImage);
	//if(nImage == 1)
	//MessageBox(szFileName);
Exit0:
	return;
}
void KDlgSoundSource::OnBnClickedButtonRefresh()
{
	// TODO: Add your control notification handler code here
	FillTree();
}

void KDlgSoundSource::OnBnClickedButtonOk()
{
	// TODO: Add your control notification handler code here
	KG_PROCESS_ERROR(m_pszTreeTouch);
	if(m_bIsWav)
	{
		strcpy(m_pszTreeTouch,m_szTreeTouch);
	}
	else
	{
		m_szTreeTouch[0] = '\0';
	}
	OnOK();
Exit0:
	;
}
HRESULT KDlgSoundSource::SetTreeTouch(LPSTR pszTreeTouch)
{
	m_pszTreeTouch = pszTreeTouch;
	m_pszTreeTouch[0] = '\0';
	return S_OK;
}
void KDlgSoundSource::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnClose();
}
