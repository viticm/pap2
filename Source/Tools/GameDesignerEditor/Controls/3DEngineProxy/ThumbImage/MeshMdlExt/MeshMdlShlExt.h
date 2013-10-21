// MeshMdlShlExt.h : Declaration of the CMeshMdlShlExt

#pragma once
#include "resource.h"       // main symbols
#include <shlguid.h>
#include <AtlCom.h>
#include <shlobj.h>
#include "MeshMdlExt.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CMeshMdlShlExt

class ATL_NO_VTABLE CMeshMdlShlExt :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMeshMdlShlExt, &CLSID_MeshMdlShlExt>,
	public IPersistFile,
	public IExtractImage2
{
public:
	CMeshMdlShlExt()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_MESHMDLSHLEXT)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CMeshMdlShlExt)
	COM_INTERFACE_ENTRY(IPersistFile)
	COM_INTERFACE_ENTRY(IExtractImage)
	COM_INTERFACE_ENTRY(IExtractImage2)
END_COM_MAP()


// IExtractImage
public:

	STDMETHOD(GetLocation)(LPWSTR pszPathBuffer,
		DWORD cchMax,
		DWORD *pdwPriority,
		const SIZE *prgSize,
		DWORD dwRecClrDepth,
		DWORD *pdwFlags);
	STDMETHOD(Extract)(HBITMAP*);
	// IExtractImage2
	STDMETHOD(GetDateStamp)(FILETIME *pDateStamp);

	// IPersistFile
	STDMETHOD(Load)(LPCOLESTR wszFile, DWORD dwMode);

	STDMETHOD(GetClassID)(LPCLSID clsid)
	{	return E_NOTIMPL;	}

	STDMETHOD(IsDirty)(VOID)
	{return E_NOTIMPL; }

	STDMETHOD(Save)(LPCOLESTR, BOOL)
	{return E_NOTIMPL; }

	STDMETHOD(SaveCompleted)(LPCOLESTR)
	{ return E_NOTIMPL; }

	STDMETHOD(GetCurFile)(LPOLESTR FAR*)
	{return E_NOTIMPL; }

private:
	SIZE m_bmSize;
	HBITMAP m_hPreview;
	TCHAR m_szFile[500];


private:
	
};

OBJECT_ENTRY_AUTO(__uuidof(MeshMdlShlExt), CMeshMdlShlExt)
