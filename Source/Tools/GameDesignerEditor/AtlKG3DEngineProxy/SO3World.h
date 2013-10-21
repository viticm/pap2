// SO3World.h : Declaration of the CSO3World

#pragma once
#include "resource.h"       // main symbols

#include "AtlKG3DEngineProxy.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CSO3World

class ATL_NO_VTABLE CSO3World :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSO3World, &CLSID_SO3World>,
	public ISO3World
{
public:
	CSO3World()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_SO3WORLD)


BEGIN_COM_MAP(CSO3World)
	COM_INTERFACE_ENTRY(ISO3World)
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

	STDMETHOD(Init)(LONG nWorldIndex);
	STDMETHOD(UnInit)(void);
	STDMETHOD(Activate)(void);
	STDMETHOD(NewEditorScene)(LONG nRegionWidth, LONG nRegionHeight, LONG* pKScene);
	STDMETHOD(LoadEditorScene)(BSTR cszFileName);
	STDMETHOD(NewClientScene)(ULONG dwMapID, LONGLONG nMapCopyIndex, LONG* pKScene);
	STDMETHOD(DeleteScene)(LONG* pScene);
	STDMETHOD(GetScene)(LONG dwMapID, LONGLONG nMapCopyIndex, LONG* pScene);
	STDMETHOD(NewNpc)(LONG dwNpcID, LONG* pNpc);
	STDMETHOD(DeleteNpc)(LONG* pNpc);
	STDMETHOD(AddNpc)(LONG* pNpc, LONG* pScene, LONG nX, LONG nY, LONG nZ);
	STDMETHOD(RemoveNpc)(LONG* pNpc, LONG bKilled);
	STDMETHOD(NewDoodad)(LONG dwDoodadID, LONG* pDoodad);
	STDMETHOD(DelDoodad)(BYTE* pDoodad);
	STDMETHOD(AddDoodad)(LONG* pDoodad, LONG* pScene, LONG nX, LONG nY, LONG nZ);
	STDMETHOD(RemoveDoodad)(LONG* pDoodad);
	STDMETHOD(NewPlayer)(LONG dwPlayerID, LONG* pPlayer);
	STDMETHOD(DelPlayer)(LONG* pPlayer);
	STDMETHOD(AddPlayer)(LONG* pPlayer, LONG* pScene, LONG nX, LONG nY, LONG nZ);
	STDMETHOD(RemovePlayer)(LONG* pPlayer);
	STDMETHOD(AttachLogicalSceneTo3DScene)();
	STDMETHOD(NewLogicSceneConnectionAndBindLogicalSceneAndSceneEditor)();
	STDMETHOD(NewLogicalDataAndAdviseWithSceneEditor)();
	STDMETHOD(LoadLogicalData)(BSTR fileName);
	STDMETHOD(SaveEditorIni)(BSTR relaPath);
	STDMETHOD(SaveCalcCollision)(LONG bOnlyAppointCell, LONG bClearCell);
	STDMETHOD(UpdateAllRest)(LONG bRest);
	STDMETHOD(SaveLogicalScene)(BSTR fileName);
	STDMETHOD(GetLogicalDataPtr)(IAtlIEKSceneEditorDocLogical** ppLogicalData);
};

OBJECT_ENTRY_AUTO(__uuidof(SO3World), CSO3World)
