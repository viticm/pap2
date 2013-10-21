#pragma once

using namespace std;

class CDirectoryList
{
public:

	struct Directory
	{
		CString                DirName;
		list<CString> FileName;
		list<CString> FilePath;
		list<Directory*> Children;
		Directory* pParent;
		void UpdateFileName();
	};

	CDirectoryList();
	~CDirectoryList();
	
	void SetDirectory(LPCSTR strDirectory);
	LPCSTR GetDirectory();
	
	void SetIncludeSubDir(BOOL bInclude);
	BOOL GetIncludeSubDir();

	virtual void UpdateContent();

	virtual void AttachWindow(CWnd* pWnd);
	virtual void DetachWindow(CWnd* pWnd);

	void AddSuffix(LPCSTR strSuffix);

	void AddDirFilter(LPCSTR strDir);

	virtual void FindFiles(CFileFind& FindFile, 
		Directory& Dir, 
		BOOL bIncludeSubDir);

	virtual void UpdateUI();
	virtual void Helper_UpdateUI(CWnd* pWnd, const Directory& Dir);

protected:
	void ClearDirectory();
	list<CWnd*> m_Wnds;
	TCHAR m_strDirectory[MAX_PATH];
	BOOL  m_bIncludeSubDir;
	set<CString> m_Suffix;
	set<CString> m_DirFilter;
	Directory  m_Root;
};