// AtlBase.h : Declaration of the CAtlBase

#pragma once
#include "resource.h"       // main symbols

#include "AtlKG3DEngineProxy.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CAtlBase

class ATL_NO_VTABLE CAtlBase :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAtlBase, &CLSID_AtlBase>,
	public IAtlBase
{
public:
	CAtlBase()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_ATLBASE)


BEGIN_COM_MAP(CAtlBase)
	COM_INTERFACE_ENTRY(IAtlBase)
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

	STDMETHOD(Init3DEngineManager)(BSTR strStartUpPath, LONG ihWnd);
	STDMETHOD(FrameMove)(void);
	STDMETHOD(Render)(void);
	STDMETHOD(ProcessMessage)(ULONG message, LONG wParam, LONG lParam);
	STDMETHOD(GetSO3World)(ISO3World** ptr);
	STDMETHOD(InitGlobalVars)(void);
	STDMETHOD(ShowProgressForm)(LONG bShow);
	STDMETHOD(ShowTopViewForm)(LONG bShow);
	STDMETHOD(UnInitEngine)(void);
	STDMETHOD(InitLogicalEditorBase)(void);
	STDMETHOD(InitKeyStateBuf)(void);
	STDMETHOD(UpdateKeyState)(void);
	STDMETHOD(SetFocusScene)(LONG pScene);
	STDMETHOD(MsgOnMouseWheel)(SHORT nDelta);
	STDMETHOD(SetDefWorkingDir)(BSTR strDir);	
	STDMETHOD(GetSceneEditorCameraTopScale)(FLOAT* fScale);
	STDMETHOD(SetCurDir)(void);
	STDMETHOD(QuaternionRotationByNDir)(FLOAT* x, FLOAT* y, FLOAT* z, FLOAT* w, LONG nDir);
	STDMETHOD(GetRepresentObjTranslation)(LONG representObjPtr, AtlVector3* vec);
	// ahpho suntao
	STDMETHOD (LoadNpcScene)(LONG hwndOutput);
	STDMETHOD (LoadDoodadScene)(LONG hwndOutput);
	STDMETHOD (FocusNpcScene)();
	STDMETHOD (FocusDoodadScene)();
	STDMETHOD (LoadNpcModel)(BSTR fileName);
	STDMETHOD (LoadFace)(BSTR meshFileName);
	STDMETHOD (LoadPluginModel)(BSTR strFileName, BSTR strBindDest);
	STDMETHOD (LoadPluginMaterial)(BSTR strSocketName, BSTR strMtlFile);
	STDMETHOD (LoadDoodadModel)(BSTR fileName);
	STDMETHOD (OnWndMsg)(PMSG WndMsg, INT* pnRet, BSTR preViewType);
	STDMETHOD (OnResizeNpcWindow)(void);
	STDMETHOD (OnResizeDoodadWindow)(void);
	STDMETHOD (ModelPlayAnimation)(BSTR fileName);
	STDMETHOD (SwitchDoodadBrush)(LONG nTemplateID, BSTR szDoodadName, LONG dwDefaultReliveID);
	STDMETHOD (SwitchNpcBrush)(LONG nTemplateID, BSTR szNpcName, LONG dwDefaultReliveID, LONG dwDefaultRandomID, LONG dwDefaultAISet);
	STDMETHOD (QuaternionNDirByRotation)(FLOAT x, FLOAT y, FLOAT z, FLOAT w, LONG* nDir);
	STDMETHOD(SetRootPath)(BSTR str3DEngineDir);
	STDMETHOD(SetShowProfile)(BOOL bShow);
};

OBJECT_ENTRY_AUTO(__uuidof(AtlBase), CAtlBase)

