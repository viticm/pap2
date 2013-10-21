// Test3DEngine.h : Declaration of the CTest3DEngine

#pragma once
#include "resource.h"       // main symbols

#include "atlax.h"
#include "_ITest3DEngineEvents_CP.h"
#include "./3DEngine/InputProxy.h"
#include <vector>
#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CTest3DEngine
interface IEKG3DScene;
interface IEKG3DModel;
interface IEKG3DRepresentObject;
class IKG3DModel;
class ATL_NO_VTABLE CTest3DEngine :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTest3DEngine, &CLSID_Test3DEngine>,
	public IConnectionPointContainerImpl<CTest3DEngine>,
	public CProxy_ITest3DEngineEvents<CTest3DEngine>,
	public IDispatchImpl<ITest3DEngine, &IID_ITest3DEngine, &LIBID_atlaxLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CTest3DEngine()
		: m_p3DScene(NULL)
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_TEST3DENGINE)


BEGIN_COM_MAP(CTest3DEngine)
	COM_INTERFACE_ENTRY(ITest3DEngine)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CTest3DEngine)
	CONNECTION_POINT_ENTRY(__uuidof(_ITest3DEngineEvents))
END_CONNECTION_POINT_MAP()


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
	STDMETHOD(Render)(void);
	STDMETHOD(FrameMove)(void);
	STDMETHOD(UnInit)(void);
private:
	IEKG3DScene* m_p3DScene;
    IEKG3DRepresentObject* m_pRepresentObject;
	IEKG3DModel* m_pSelectedObj;
	std::vector<IKG3DModel*> m_ItemModels;
	INT m_nOutputWindowID;
	HWND m_hRenderWnd;
	CInputProxy m_InputProxy;

	
public:
	STDMETHOD(OnResizeWindow)(void);
	STDMETHOD(get_PlayMode)(VARIANT_BOOL* pVal);
	STDMETHOD(put_PlayMode)(VARIANT_BOOL newVal);
	STDMETHOD(OnWndMsg)(PMSG WndMsg, INT* pnRet);
	STDMETHOD(LoadFile)(BSTR strFileName);
    STDMETHOD(LoadAni)(BSTR strFileName);
	STDMETHOD(UnloadFile)(BSTR strFileName);
	STDMETHOD(LoadPlayerModel)(BSTR strFileName);
	STDMETHOD(BindToPlayer)(BSTR strFileName, DWORD dwType, BSTR strBindToObj);
	STDMETHOD(ZoomView)(LONG lDelta);
	STDMETHOD(ChangeModelPart)(BSTR filename, INT PartID);
	STDMETHOD(LoadFace)(BSTR strfilename);
	STDMETHOD(LoadSocket)(BSTR strBindToObj, BSTR strFileName);
	STDMETHOD(ItemsClear)(void);
	STDMETHOD(SaveMDLToFile)(BSTR strFileName);
};

OBJECT_ENTRY_AUTO(__uuidof(Test3DEngine), CTest3DEngine)
