// AtlInputProxy.h : Declaration of the CAtlInputProxy

#pragma once
#include "resource.h"       // main symbols

#include "AtlKG3DEngineProxy.h"
#include "InputProxy_Impl.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CAtlInputProxy

class ATL_NO_VTABLE CAtlInputProxy :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAtlInputProxy, &CLSID_AtlInputProxy>,
	public IAtlInputProxy
{
public:
	CAtlInputProxy()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_ATLINPUTPROXY)


BEGIN_COM_MAP(CAtlInputProxy)
	COM_INTERFACE_ENTRY(IAtlInputProxy)
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

	STDMETHOD(Init)(LONG hWnd);
	STDMETHOD(CheckInput)(PMSG pMsg, LONG* pnRet);

public:
	CInputProxy* m_inputproxy;
};

OBJECT_ENTRY_AUTO(__uuidof(AtlInputProxy), CAtlInputProxy)
