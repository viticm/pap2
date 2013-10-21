// Game.h : Declaration of the CGame

#pragma once
#include "resource.h"       // main symbols

#include <atlctl.h>
#include "kegame.h"
#include "_IGameEvents_CP.h"
#include "SystemInfo.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CGame

class ATL_NO_VTABLE CGame :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CGame, &CLSID_Game>,
	public IConnectionPointContainerImpl<CGame>,
	public CProxy_IGameEvents<CGame>,
	public IDispatchImpl<IGame, &IID_IGame, &LIBID_kegameLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
    public IProvideClassInfo2Impl<&CLSID_Game, &DIID__IGameEvents, &LIBID_kegameLib>,
    public IObjectSafetyImpl<CGame, INTERFACESAFE_FOR_UNTRUSTED_CALLER>
{
public:
	CGame()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_GAME)


BEGIN_COM_MAP(CGame)
	COM_INTERFACE_ENTRY(IGame)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
    COM_INTERFACE_ENTRY(IProvideClassInfo)
    COM_INTERFACE_ENTRY(IProvideClassInfo2)
    COM_INTERFACE_ENTRY(IObjectSafety)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CGame)
	CONNECTION_POINT_ENTRY(__uuidof(_IGameEvents))
END_CONNECTION_POINT_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

private:
    CSystemInfo     m_SystemInfo;

public:

    STDMETHOD(GetSystemVersion)(BSTR* bstrVersion);
    STDMETHOD(GetSystemMemInfo)(BSTR* bstrMem);
    STDMETHOD(GetSystemCpuInfo)(BSTR* bstrCpu);
	STDMETHOD(GetDisplayInfo)(BSTR* bstrDisplay);
	STDMETHOD(GetHardDiskId)(BSTR* bstrHardId);
	STDMETHOD(EncryptString)(BSTR bstrSrc, BSTR* bstrDest);
};

OBJECT_ENTRY_AUTO(__uuidof(Game), CGame)
