// FileThumbExtract.cpp : Implementation of CFileThumbExtract
#include "stdafx.h"
//#include "IExtractImage.h"
#include "ThumbExtract.h"
#include "FileThumbExtract.h"
#include <shlobj.h>
#include <shlwapi.h>
#include <wchar.h>
#include <shlwapi.h>

#include "CxImage/ximage.h"

/////////////////////////////////////////////////////////////////////////////
// CFileThumbExtract
// ---------- START code from thumbnail.cpp from GotDotNet
PBITMAPINFO CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp,WORD nBits);
BOOL CreateBMPFile(HWND hwnd, LPWSTR pszFile, PBITMAPINFO pbi, HBITMAP hBMP, HDC hDC);

#define SHORTCUT_TEMPLATE  L"[InternetShortcut]\nURL=%s\n"
#define TEMP_URL           L"TEMP.URL"


BOOL CreateURLShortcut(LPCWSTR pszURL, LPCWSTR pszShortcutFile) {
   WCHAR pszShortcutContents[1024];

   HANDLE hf = CreateFileW(pszShortcutFile, GENERIC_READ | GENERIC_WRITE, (DWORD) 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE) NULL); 
   if(hf == INVALID_HANDLE_VALUE) return FALSE;
   int iCharCount = swprintf(pszShortcutContents, SHORTCUT_TEMPLATE, pszURL) + 1;
   DWORD dwWritten = 0;
   if(!WriteFile(hf, pszShortcutContents, sizeof(WCHAR)*iCharCount, &dwWritten, NULL)) return FALSE;
   CloseHandle(hf);
   return TRUE;
}

HRESULT CFileThumbExtract::CreateThumbnail(DWORD dwWidth, DWORD dwHeight, HBITMAP* pThumbnail)
{
   LPITEMIDLIST pidlItems = NULL, pidlURL = NULL, pidlWorkDir = NULL;
   HRESULT hr;
   WCHAR pszPath[MAX_PATH];
   DWORD dwPriority = 0, dwFlags = IEIFLAG_ASPECT;
   SIZE size = { dwWidth, dwHeight };
   IExtractImage* peiURL = NULL;
   IShellFolder* psfDesktop = NULL;
   IShellFolder* psfWorkDir = NULL;

	 basic_string<WCHAR> wsDir,wsFile,wsTempFile;

   hr = SHGetDesktopFolder(&psfDesktop);
   if(FAILED(hr)) goto OnExit;

   // create shortcut if URL
	 if(PathIsURLW(m_wsPath.c_str())) {
		wsTempFile = m_wsTempPath; wsTempFile += TEMP_URL;
		if(!CreateURLShortcut(m_wsPath.c_str(), wsTempFile.c_str())) goto OnExit;
		wsDir = m_wsTempPath;
		wsFile = TEMP_URL;
	 }
	 else {
		wsDir = m_wsDir;
		wsFile = m_wsFile;
	 }
   // get working directory
	 wcscpy(m_wsBuffer,wsDir.c_str());
   hr = psfDesktop->ParseDisplayName(NULL, NULL, m_wsBuffer, NULL, &pidlWorkDir, NULL);
   if(FAILED(hr)) goto OnExit;
   hr = psfDesktop->BindToObject(pidlWorkDir, NULL, IID_IShellFolder, (LPVOID*)&psfWorkDir);
   if(FAILED(hr)) goto OnExit;
   psfDesktop->Release();
   psfDesktop = NULL;
   m_pMalloc->Free(pidlWorkDir);
   pidlWorkDir = NULL;

   // retrieve link information
	 wcscpy(m_wsBuffer,wsFile.c_str());
   hr = psfWorkDir->ParseDisplayName(NULL, NULL, m_wsBuffer, NULL, &pidlURL, NULL);
   if(FAILED(hr)) goto OnExit;
   
   // query IExtractImage
   hr = psfWorkDir->GetUIObjectOf(NULL, 1, (LPCITEMIDLIST*)&pidlURL, IID_IExtractImage, NULL, (LPVOID*)&peiURL);
   if(FAILED(hr)) goto OnExit;
   
   // define thumbnail properties
   hr = peiURL->GetLocation(pszPath, MAX_PATH, &dwPriority, &size, 16, &dwFlags);
   if(FAILED(hr)) goto OnExit;
   
   // generate thumbnail
   hr = peiURL->Extract(pThumbnail);
   if(FAILED(hr)) goto OnExit;

   // clean-up IExtractImage
   peiURL->Release();
   peiURL = NULL;

OnExit:

   // free allocated structures
   if(peiURL != NULL) peiURL->Release();
   if(pidlURL != NULL) m_pMalloc->Free(pidlURL);
   if(pidlWorkDir != NULL) m_pMalloc->Free(pidlWorkDir);
   if(psfDesktop != NULL) psfDesktop->Release();
   if(psfWorkDir != NULL) psfWorkDir->Release();
   return hr;
}

BOOL CreateBitmapFile(HBITMAP hThumbnail, LPWSTR pszThumbnail) {

   // create bitmap
   PBITMAPINFO pBitmapInfo = CreateBitmapInfoStruct(NULL, hThumbnail,0);
   if(pBitmapInfo == NULL) return FALSE;

   HDC hDC = GetDC(NULL);
   CreateBMPFile(NULL, pszThumbnail, pBitmapInfo, hThumbnail, hDC);
   ReleaseDC(NULL, hDC);
   LocalFree(pBitmapInfo);

   return TRUE;
}

// ----------------------- my code here ----------------------------
STDMETHODIMP CFileThumbExtract::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IFileThumbExtract
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

CFileThumbExtract::CFileThumbExtract()
{
	m_Width = m_Height = 120;
	m_pMalloc = NULL;
	m_hThumbnail = NULL;
	m_pBitmapData = NULL;
}

HRESULT CFileThumbExtract::FinalConstruct()
{
  ::GetTempPathW(1000,m_wsBuffer);
  m_wsTempPath = m_wsBuffer;
  return SHGetMalloc(&m_pMalloc);
}

HRESULT CFileThumbExtract::FinalRelease()
{
	return FreeThumbnail();
} 

void CFileThumbExtract::ResolvePath()
{ 
	if(PathIsURLW(m_wsPath.c_str()))
		return; // dont resolve URL 
	// resolve full-path to dir and file
	wcscpy(m_wsBuffer,m_wsPath.c_str());
	::PathRemoveFileSpecW(m_wsBuffer); 
	m_wsDir = m_wsBuffer;
	m_wsFile  = ::PathFindFileNameW(m_wsPath.c_str());
}

STDMETHODIMP CFileThumbExtract::SetPath(BSTR Path)
{
	m_wsPath = Path;
	ResolvePath();
	return S_OK;
}

STDMETHODIMP CFileThumbExtract::SetDir(BSTR Dir)
{
	m_wsDir = Dir;
	return S_OK;
}

STDMETHODIMP CFileThumbExtract::SetFile(BSTR File)
{
	m_wsFile = File;
	if(::PathIsRelativeW(m_wsFile.c_str())) {
		::PathCombineW(m_wsBuffer,m_wsDir.c_str(),m_wsFile.c_str());
		m_wsPath = m_wsBuffer;
	}
	else { // file is absolute path
		m_wsPath = m_wsFile;
	}
	ResolvePath();
	return S_OK;
}

STDMETHODIMP CFileThumbExtract::GetPath(BSTR *Path)
{
	*Path = ::SysAllocString(m_wsPath.c_str()); 
	return S_OK;
}

STDMETHODIMP CFileThumbExtract::GetDir(BSTR *Dir)
{
	*Dir = ::SysAllocString(m_wsDir.c_str()); 
	return S_OK;
}

STDMETHODIMP CFileThumbExtract::GetFile(BSTR *File)
{
	*File = ::SysAllocString(m_wsFile.c_str()); 
	return S_OK;
}
STDMETHODIMP CFileThumbExtract::SetThumbnailSize(long Width, long Height)
{
	m_Width = Width; m_Height = Height;
	return S_OK;
}

STDMETHODIMP CFileThumbExtract::ExtractThumbnail(long *phBitmap) //SAFEARRAY **ppBits)
{
	HRESULT hr; 
  FreeThumbnail();
	hr = CreateThumbnail(m_Width, m_Height, &m_hThumbnail);
  if(FAILED(hr)) return Error("Could not create thumbnail");
	/*
	// i tried first to return the data
	// easier to return the bitmap and use the Bitmap.FromHbitmap static function
	// create info struct for the desired format (24 bpp)
	PBITMAPINFO pBitmapInfo = CreateBitmapInfoStruct(NULL, m_hThumbnail,24);
	if(pBitmapInfo == NULL) return Error("Could not create thumbnail");
  HDC hDC = GetDC(NULL);
	// allocate our buffer to take the bits
	DWORD dwSize = pBitmapInfo->bmiHeader.biSizeImage;
	m_pBitmapData = (LPBYTE)::GlobalAlloc(GMEM_FIXED,dwSize); 
	// get bits in desired format (24 bbp)
	::GetDIBits(hDC,m_hThumbnail,0,pBitmapInfo->bmiHeader.biHeight,
		 m_pBitmapData, pBitmapInfo, DIB_RGB_COLORS);
	LocalFree(pBitmapInfo);
  ReleaseDC(NULL, hDC);
	*/
	*phBitmap = (long)m_hThumbnail;
	return S_OK;
}

STDMETHODIMP CFileThumbExtract::FreeThumbnail()
{
  if(m_hThumbnail != NULL) {
		m_pMalloc->Free(m_hThumbnail);
		m_hThumbnail = NULL;
	}
	if(m_pBitmapData != NULL)	{ 
		::GlobalFree(m_pBitmapData); // delete [] m_pBitmapData; 
		m_pBitmapData = NULL;
	}
	return S_OK;
}

STDMETHODIMP CFileThumbExtract::get_Version(BSTR *pVal)
{
	*pVal = SysAllocString(L"Thumbnail Generator v.1.0 by Philip Software");
	return S_OK;
}

STDMETHODIMP CFileThumbExtract::get_Stride(long *pVal)
{
	return((m_Width * 24 +31) & ~31);
	return S_OK;
}

STDMETHODIMP CFileThumbExtract::SaveToFile(BSTR FilePath)
{
	//hr = ExtractThumbnail(pDummy);
  //if(FAILED(hr)) return Error("Could not create thumbnail");
	stdwstr wsFilePath,wsExt;
	wsFilePath = FilePath;
	int pos = wsFilePath.find_last_of(L".");
	wsExt = _wcsupr((WCHAR *)wsFilePath.substr(pos).c_str());
	if(wsExt == L".BMP")
		CreateBitmapFile(m_hThumbnail, FilePath);
	else if(wsExt == L".JPG" || wsExt == L".JPEG") {
		CxImage cxThumbJpg;
		cxThumbJpg.CreateFromHBITMAP(m_hThumbnail);
		cxThumbJpg.Save(FilePath,CXIMAGE_FORMAT_JPG);
	}

  FreeThumbnail();
	return S_OK;
}

STDMETHODIMP CFileThumbExtract::get_ThumbJpgData(long JpegQuality, VARIANT *pVal)
{
	SAFEARRAY *psaData; BYTE *pData,*pBuffer; long size;
  SAFEARRAYBOUND rgsabound[1];

	CxImage cxThumbJpg;
	cxThumbJpg.CreateFromHBITMAP(m_hThumbnail);
	cxThumbJpg.SetJpegQuality((BYTE)JpegQuality);
	CxMemFile memfile;	memfile.Open();
	bool bOk = cxThumbJpg.Encode(&memfile, CXIMAGE_FORMAT_JPG);
	if(!bOk) return S_OK; // NO-IMAGE
	pBuffer=memfile.GetBuffer();
	size=memfile.Size();

	// create safe array and copy image data
  rgsabound[0].lLbound = 0; rgsabound[0].cElements = size;
	psaData = SafeArrayCreate(VT_UI1, 1, rgsabound); 
	SafeArrayAccessData(psaData, (void **)&pData);
	memcpy(pData,pBuffer,size);
	SafeArrayUnaccessData(psaData);
	free(pBuffer);
	// put data in variant
	pVal->vt = VT_ARRAY | VT_UI1;
	pVal->parray = psaData;
	return S_OK;
}
