// BrowseForFolder.h: interface for the CBrowseForFolder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BROWSEFORFOLDER_H__E0429AE7_C577_4B2A_9569_50A443BD81A0__INCLUDED_)
#define AFX_BROWSEFORFOLDER_H__E0429AE7_C577_4B2A_9569_50A443BD81A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBrowseForFolder : public CObject  
{
public:
	CString BrowseForFolder(HWND hWnd, LPCTSTR lpszTitle, UINT nFlags);
	CBrowseForFolder();
	virtual ~CBrowseForFolder();

};

#endif // !defined(AFX_BROWSEFORFOLDER_H__E0429AE7_C577_4B2A_9569_50A443BD81A0__INCLUDED_)
