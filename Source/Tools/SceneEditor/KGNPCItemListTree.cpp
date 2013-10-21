#include "stdafx.h"
#include "kglog.h"
#include "KGNPCItemListTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


void KGNPCItemListTree::Helper_UpdateUI(CWnd* pWnd, const Directory& Child, HTREEITEM hParent)
{
	CTreeCtrl* pTree = static_cast<CTreeCtrl*>(pWnd);
	HTREEITEM hThis = TVI_ROOT;
	int nIndex = 0;

	hThis = pTree->InsertItem(Child.DirName, hParent);
	pTree->SetItemData(hThis, NULL);

	for (std::list<CString>::const_iterator i = Child.FileName.begin(),  j = Child.FilePath.begin();
		i != Child.FileName.end();
		i++, j++)
	{
		HTREEITEM hChild = pTree->InsertItem((*i), hThis);
		pTree->SetItemData(hChild, (DWORD_PTR)(LPCTSTR(*j)));
	}

	for (std::list<Directory*>::const_iterator i = Child.Children.begin();
		i != Child.Children.end();
		i++)
	{
		Helper_UpdateUI(pWnd, **i, hThis);
	}
}


void KGNPCItemListTree::UpdateUI()
{
	for (std::list<CWnd*>::iterator i = m_Wnds.begin();
		i != m_Wnds.end();
		i++)
	{
		CTreeCtrl* pWnd = static_cast<CTreeCtrl*>(*i);
		pWnd->DeleteAllItems();

		Helper_UpdateUI(pWnd, m_Root, TVI_ROOT);

	}
}


void KGNPCItemListTree::FindFiles(CFileFind& FindFile,
								  Directory& Dir,
								  BOOL bIncludeSubDir)
{
	BOOL bReturn = TRUE;

	while (bReturn)
	{
		bReturn = FindFile.FindNextFile();
		if (FindFile.IsDirectory() && bIncludeSubDir)
		{
			CString strDirName = FindFile.GetFileName();
			TCHAR strFixDirName[MAX_PATH];
			strcpy_s(strFixDirName, MAX_PATH, strDirName);
			_strlwr_s(strFixDirName, MAX_PATH);

			if (m_DirFilter.find(strFixDirName) != m_DirFilter.end())
			{
				continue;
			}

			Directory* pNewEntry = new Directory;
			KGLOG_PROCESS_ERROR(pNewEntry);

			pNewEntry->DirName = strDirName;
			pNewEntry->pParent = &Dir;
			if (&Dir == &m_Root)
			{
				Dir.Children.push_back(pNewEntry);
			}
			CFileFind FindChildFile;
			CString strPathName = FindFile.GetFilePath();
			TCHAR strFilter[MAX_PATH];
			sprintf_s(strFilter, MAX_PATH, "%s\\*.*", strPathName);
			BOOL bReturn1 = FindChildFile.FindFile(strFilter);
			if (!bReturn1)
				continue;

			FindFiles(FindChildFile, *pNewEntry, bIncludeSubDir);
			pNewEntry->UpdateFileName();
		}
		else
		{
			CString strDirName = FindFile.GetFileName();
			TCHAR strFileExt[MAX_PATH];
			_splitpath_s(strDirName,
				NULL,
				0,
				NULL,
				0,
				NULL,
				0,
				strFileExt,
				MAX_PATH);
			_strlwr_s(strFileExt, MAX_PATH);

			std::set<CString>::iterator i = m_Suffix.begin();
			while (i != m_Suffix.end())
			{
				if (!strcmp((*i), strFileExt))
				{
					CString strPathName = FindFile.GetFilePath();
					//Dir.FilePath.push_back(strPathName);
					Dir.pParent->FilePath.push_back(strPathName);
				}
				i++;
			}
		}
	}
Exit0:
	return;
}