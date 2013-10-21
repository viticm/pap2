#include "stdafx.h"
#include "directorylist.h"
#include "kglog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


void CDirectoryList::Directory::UpdateFileName()
{
	TCHAR strFileName[MAX_PATH];
	TCHAR strFileExt[MAX_PATH];
	TCHAR strFile[MAX_PATH];

	FileName.clear();
	std::list<CString>::iterator i = FilePath.begin();
	while(i != FilePath.end())
	{
		_splitpath_s((*i),
			NULL,
			0,
			NULL,
			0,
			strFileName,
			MAX_PATH,
			strFileExt,
			MAX_PATH);

		sprintf_s(strFile, MAX_PATH, "%s%s", strFileName, strFileExt);
		FileName.push_back(strFile);
		i++;
	}
}

CDirectoryList::CDirectoryList()
{
	AddDirFilter(".");
	AddDirFilter("..");
}

CDirectoryList::~CDirectoryList()
{
	ClearDirectory();
}

void CDirectoryList::SetDirectory(LPCSTR strDirectory)
{
	strcpy_s(m_strDirectory, MAX_PATH * sizeof(TCHAR), strDirectory);
	if (m_strDirectory[strlen(m_strDirectory) - 1] != '\\')
	{
		strcat_s(m_strDirectory, MAX_PATH, "\\");
	}

	m_Root.DirName = m_strDirectory;
	UpdateContent();
}

LPCSTR CDirectoryList::GetDirectory()
{
	return m_strDirectory;
}

void CDirectoryList::SetIncludeSubDir(BOOL bInclude)
{
	m_bIncludeSubDir = bInclude;
}

BOOL CDirectoryList::GetIncludeSubDir()
{
	return m_bIncludeSubDir;
}

void CDirectoryList::FindFiles(CFileFind& FindFile,
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
			Dir.Children.push_back(pNewEntry);
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
					Dir.FilePath.push_back(strPathName);
				}
				i++;
			}
		}
	}
Exit0:
	return;
}


void CDirectoryList::UpdateContent()
{
	BOOL bReturn = FALSE;
	CFileFind FindFile;
	TCHAR strFilter[MAX_PATH];

	std::set<CString>::iterator i = m_Suffix.begin();
	ClearDirectory();

	sprintf_s(strFilter, MAX_PATH, "%s*.*", m_strDirectory);

	bReturn = FindFile.FindFile(strFilter);
	KG_PROCESS_ERROR(bReturn);

	FindFiles(FindFile, m_Root, m_bIncludeSubDir);
	m_Root.UpdateFileName();
Exit0:
	return;
}

void CDirectoryList::AttachWindow(CWnd* pWnd)
{
	std::list<CWnd*>::iterator i;
	KG_PROCESS_ERROR(pWnd);

	i = find(m_Wnds.begin(), m_Wnds.end(), pWnd);

	if (i == m_Wnds.end())
	{
		m_Wnds.push_back(pWnd);
	}
Exit0:
	return;
}

void CDirectoryList::DetachWindow(CWnd* pWnd)
{
	std::list<CWnd*>::iterator i;
	KG_PROCESS_ERROR(pWnd);

	i = find(m_Wnds.begin(), m_Wnds.end(), pWnd);

	if (i != m_Wnds.end())
	{
		m_Wnds.erase(i);
	}
Exit0:
	return;
}

void CDirectoryList::AddSuffix(LPCSTR strSuffix)
{
	TCHAR strFixSuffix[MAX_PATH];
	strcpy_s(strFixSuffix, MAX_PATH, strSuffix);
	_strlwr_s(strFixSuffix, MAX_PATH);

	m_Suffix.insert(strFixSuffix);
}

void CDirectoryList::AddDirFilter(LPCSTR strDirFilter)
{
	TCHAR strFixDirFilter[MAX_PATH];
	strcpy_s(strFixDirFilter, MAX_PATH, strDirFilter);
	_strlwr_s(strFixDirFilter, MAX_PATH);

	m_DirFilter.insert(strFixDirFilter);
}

static void ClearDirectory(CDirectoryList::Directory& Dir)
{
	std::list<CDirectoryList::Directory*>::iterator i = Dir.Children.begin();
	while (i != Dir.Children.end())
	{
		ClearDirectory(*(*i));
		delete (*i);
		i++;
	}
}

void CDirectoryList::ClearDirectory()
{
	::ClearDirectory(m_Root);
}

void CDirectoryList::UpdateUI()
{
	return;
}

void CDirectoryList::Helper_UpdateUI(CWnd* pWnd, const Directory& Dir)
{
	return;
}