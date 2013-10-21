// MeshMdlExt.cpp : Implementation of DLL Exports.


#include "stdafx.h"
#include "resource.h"
#include "MeshMdlExt.h"
#include "ExtractImageApp.h"


#ifndef _MESH_PREVIEW
#define _MESH_PREVIEW


class CMeshMdlExtModule : public CAtlDllModuleT< CMeshMdlExtModule >
{
public :
	DECLARE_LIBID(LIBID_MeshMdlExtLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_MESHMDLEXT, "{FFB4BCB8-2143-4553-BFDD-512389A1E55B}")
};

CMeshMdlExtModule _AtlModule;
CExtractImageApp theApp;

class CMeshMdlExtApp : public CWinApp
{
public:

// Overrides
    virtual BOOL InitInstance();
    virtual int ExitInstance();

    DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CMeshMdlExtApp, CWinApp)
END_MESSAGE_MAP()


BOOL CMeshMdlExtApp::InitInstance()
{
	return theApp.InitInstance();
}
int CMeshMdlExtApp::ExitInstance()
{
	_AtlModule.Term();
	return theApp.ExitInstance();
}

// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());  
    return (AfxDllCanUnloadNow()==S_OK && _AtlModule.GetLockCount()==0) ? S_OK : S_FALSE;

	//return S_FALSE;
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

#endif
