// MeshMdlShlExt.cpp : Implementation of CMeshMdlShlExt

#include "stdafx.h"
#include "MeshMdlShlExt.h"
#include <shlobj.h>
#include "ExtractImageApp.h"

extern CExtractImageApp theApp;





HRESULT CMeshMdlShlExt::GetLocation(LPWSTR pszPathBuffer,
										DWORD cchMax, DWORD *pdwPriority,
										const SIZE *prgSize, DWORD dwRecClrDepth,
										DWORD *pdwFlags)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	m_bmSize = *prgSize;
	if (*pdwFlags & IEIFLAG_ASYNC)	return E_PENDING; 
	return NOERROR;
}

HRESULT CMeshMdlShlExt::Load(LPCOLESTR wszFile, DWORD dwMode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	USES_CONVERSION;
	_tcscpy(m_szFile, OLE2T((WCHAR*)wszFile)); 

	return S_OK;	
}

// IExtractImage::Extract
HRESULT CMeshMdlShlExt::Extract(HBITMAP* phBmpThumbnail)
{ 
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	theApp.CreateThumbnail(phBmpThumbnail, m_szFile);

	

	return NOERROR; 
}

HRESULT CMeshMdlShlExt::GetDateStamp(FILETIME *pDateStamp)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	FILETIME ftCreationTime,ftLastAccessTime,ftLastWriteTime;
	// open the file and get last write time
	HANDLE hFile = CreateFile(m_szFile,GENERIC_READ,FILE_SHARE_READ,NULL,
		OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(!hFile) return E_FAIL;
	GetFileTime(hFile,&ftCreationTime,&ftLastAccessTime,&ftLastWriteTime);
	CloseHandle(hFile);
	*pDateStamp = ftLastWriteTime;
	return NOERROR; 
}

