// BrowseForFolder.cpp: implementation of the CBrowseForFolder class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BrowseForFolder.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBrowseForFolder::CBrowseForFolder()
{

}

CBrowseForFolder::~CBrowseForFolder()
{

}

CString CBrowseForFolder::BrowseForFolder(HWND hWnd, LPCTSTR lpszTitle, UINT nFlags)
{
	CString strResult;
	char szDisplayName[255];
	LPMALLOC lpMalloc;  // pointer to IMalloc
	LPITEMIDLIST lpItemIDList;

	if (::SHGetMalloc(&lpMalloc) != NOERROR)
		return strResult;
	char szBuffer[255];

	LPITEMIDLIST pidl;
	SHGetSpecialFolderLocation(hWnd,CSIDL_DRIVES,&pidl);

	BROWSEINFO browseInfo;
	browseInfo.hwndOwner = hWnd;
	// set root at Desktop
	browseInfo.pidlRoot = pidl; 
	browseInfo.pszDisplayName = szDisplayName;
	browseInfo.lpszTitle = lpszTitle;   // passed in
	browseInfo.ulFlags = nFlags;   // also passed in
	browseInfo.lpfn = NULL;      // not used
	browseInfo.lParam = 0;      // not used     LPITEMIDLIST lpItemIDList;

	if ((lpItemIDList = ::SHBrowseForFolder(&browseInfo))!= NULL)
	{
		// Get the path of the selected folder from the
		// item ID list.
		if (::SHGetPathFromIDList(lpItemIDList, szBuffer))
		{
			// At this point, szBuffer contains the path 
			// the user chose.
			if (szBuffer[0] == '\0')
			{
			   // SHGetPathFromIDList failed, or
			   // SHBrowseForFolder failed.
			   AfxMessageBox("Failded to get directory",MB_ICONSTOP|MB_OK);
			   return strResult;
			}

			// We have a path in szBuffer!
			// Return it.
			strResult = szBuffer;
			return strResult;
		}
		else
		{
			// The thing referred to by lpItemIDList 
			// might not have been a file system object.
			// For whatever reason, SHGetPathFromIDList
			// didn't work!
			AfxMessageBox("Failded to get directory",MB_ICONSTOP|MB_OK);
			return strResult; // strResult is empty 
		}
		lpMalloc->Free(lpItemIDList);
		lpMalloc->Release();      
	}// If we made it this far, SHBrowseForFolder failed.
	return strResult;
}
