// BaseLib.h : Declaration of the CBaseLib

#pragma once
#include "AtlKGRecordeOperator.h"
#include "resource.h"       // main symbols
#include <comsvcs.h>



// CBaseLib

class ATL_NO_VTABLE CBaseLib :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CBaseLib, &CLSID_BaseLib>,
	public IBaseLib
{
public:
	CBaseLib()
	{
        m_hWnd = NULL;
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_BASELIB)

DECLARE_NOT_AGGREGATABLE(CBaseLib)

BEGIN_COM_MAP(CBaseLib)
	COM_INTERFACE_ENTRY(IBaseLib)
END_COM_MAP()

private:
    HWND m_hWnd;


// IBaseLib
public:
    STDMETHOD(Init)(int nhWnd);

    void MyMethod( int nhWnd );
    STDMETHOD(UnInit)(void);
};

OBJECT_ENTRY_AUTO(__uuidof(BaseLib), CBaseLib)
