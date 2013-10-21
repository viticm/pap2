// AtlKG3DEngineProxy.cpp : Implementation of DLL Exports.


#include "stdafx.h"
#include "resource.h"
#include "AtlKG3DEngineProxy.h"


class CAtlKG3DEngineProxyModule : public CAtlDllModuleT< CAtlKG3DEngineProxyModule >
{
public :
	DECLARE_LIBID(LIBID_AtlKG3DEngineProxyLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_ATLKG3DENGINEPROXY, "{61195834-D75E-4586-BFC4-ED04641CAE47}")
};

CAtlKG3DEngineProxyModule _AtlModule;


#ifdef _MANAGED
#pragma managed(push, off)
#endif

// DLL Entry Point
extern "C" BOOL WINAPI DllMain2(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	hInstance;
    return _AtlModule.DllMain(dwReason, lpReserved); 
}

#ifdef _MANAGED
#pragma managed(pop)
#endif




// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void)
{
    return _AtlModule.DllCanUnloadNow();
}


// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}


// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
    HRESULT hr = _AtlModule.DllRegisterServer();
	return hr;
}


// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
	HRESULT hr = _AtlModule.DllUnregisterServer();
	return hr;
}

