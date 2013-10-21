// BaseLib.h : Declaration of the CBaseLib

#pragma once
#include "resource.h"       // main symbols

#include "AtlKG3DEngineProxy.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CBaseLib

class ATL_NO_VTABLE CBaseLib :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CBaseLib, &CLSID_BaseLib>,
	public IBaseLib
{
public:
	CBaseLib()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_BASELIB)


BEGIN_COM_MAP(CBaseLib)
	COM_INTERFACE_ENTRY(IBaseLib)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
	STDMETHOD(InitLog)(BSTR szIdentity, LONG nMaxlineEachFile, LONG Options, BSTR szPath);
	STDMETHOD(InitMemory)(BSTR memoryName);
	STDMETHOD(UnInitMemory)();
};

OBJECT_ENTRY_AUTO(__uuidof(BaseLib), CBaseLib)
