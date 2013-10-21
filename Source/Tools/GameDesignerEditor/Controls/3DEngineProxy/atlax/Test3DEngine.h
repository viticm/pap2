// Test3DEngine.h : Declaration of the CTest3DEngine

#pragma once
#include "resource.h"       // main symbols
#include "atlax.h"
#include "_ITest3DEngineEvents_CP.h"
#include "./3DEngine/InputProxy.h"
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
		//: m_pActive3DScene(NULL)
		: m_index(-1)
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
	STDMETHOD(Init)(LONG hWnd, BSTR strEnginePath, BSTR strStartUpPath);
	STDMETHOD(ActivateScene)(BSTR editor_name);
	STDMETHOD(RemoveScene)(BSTR editor_name);
	STDMETHOD(AddScene)(LONG hwndOutput, BSTR editor_name, LONG iOption);
	STDMETHOD(LoadBillboardSfx)(BSTR filename);
	STDMETHOD(SetBillboardHeight)(INT npc_height, INT head_top_adjust);
	STDMETHOD(LoadOneMap_NoOutputWindow)(BSTR filename, LONG* ptrScene);
	STDMETHOD(GetEditorSceneCount)(BSTR editor_name, LONG* count);
	STDMETHOD(Render)(void);
	STDMETHOD(FrameMove)(void);
	STDMETHOD(UnInit)(void);
	STDMETHOD(OnResizeWindow)(void);
	STDMETHOD(get_PlayMode)(VARIANT_BOOL* pVal);
	STDMETHOD(put_PlayMode)(VARIANT_BOOL newVal);
	STDMETHOD(OnWndMsg)(BSTR editor_name, PMSG WndMsg, INT* pnRet);
	STDMETHOD(LoadFile)(BSTR strFileName);
	STDMETHOD(LoadAni)(BSTR strFileName, BOOL bLoop);
	STDMETHOD(UnloadFile)(BSTR strFileName);
	STDMETHOD(LoadPlayerModel)(BSTR strFileName);
	STDMETHOD(LoadSelectorSfxFile)(BSTR sfx_name);
	STDMETHOD(BindToPlayer)(BSTR strFileName, DWORD dwType, BSTR strBindToObj);
	STDMETHOD(ZoomView)(LONG lDelta);
	STDMETHOD(ChangeModelPart)(BSTR filename, INT PartID);
	STDMETHOD(LoadFace)(BSTR strfilename);
	STDMETHOD(ItemsClear)(void);
	STDMETHOD(SaveMDLToFile)(BSTR strFileName);
	STDMETHOD(LoadMaterial)(BSTR strFileName);
	STDMETHOD(LoadAnimation)(BSTR strFileName, BOOL bLoopPlay);
	STDMETHOD(LoadPluginModel)(BSTR strFileName, BSTR strBindDest);
	STDMETHOD(ClearPluginModel)(BSTR strBindDest);
	STDMETHOD(ScaleSelectorSfx)(FLOAT x, FLOAT y, FLOAT z);
	STDMETHOD(ToggleRotateModelEditor)(void);
	STDMETHOD(SetCameraInfo)(LONG pX, LONG pY, LONG pZ, LONG lX, LONG lY, LONG lZ, LONG width, LONG height);
	STDMETHOD(GetCameraInfo)(LONG* pX, LONG* pY, LONG* pZ, LONG* lX, LONG* lY, LONG* lZ, LONG* width, LONG* height);
	STDMETHOD(AdjustCameraOrthogonal)(LONG iOffset);
	STDMETHOD(SetModelScale)(FLOAT fScale);
	STDMETHOD(SaveModelToImageFile)(BSTR fileName);
	STDMETHOD(LoadSkillSfx)(BSTR strFileName, FLOAT scale, BSTR socketName, LONG iIndex);
	STDMETHOD(ReadMapTerrainInfo)(LONG ptrScene, LONG* width, LONG* height);
	STDMETHOD(FileToImage)(BSTR src3DFileName, BSTR desImageFileName);
	STDMETHOD(GetCurSceneIndex)(LONG* index);
	STDMETHOD(ActivateSceneByIndex)(LONG index);
	STDMETHOD(SaveMdlWithAniToImages)(LONG ptrModel, BSTR filename);
	STDMETHOD(SaveLogicScene)(BSTR iniFile, LONG ptr3DScene);
	STDMETHOD(ShowProgressForm)(BOOL bShow);
	STDMETHOD(UnloadPlayerModel)(void);
	STDMETHOD(GetHeight)(LONG ptrScene, FLOAT x, FLOAT z, FLOAT* y);
	STDMETHOD(GetFileNameHash)(BSTR fileName, DWORD* hashid);
	STDMETHOD(QuaternionRotationByNDir)(FLOAT* x, FLOAT* y, FLOAT* z, FLOAT* w, LONG nDir);
	STDMETHOD(LoadPluginMaterial)(BSTR strSocketName, BSTR strMtlFile);
	STDMETHOD(GetAniFrameCount)(BSTR strAniFileName, LONG* count);
	STDMETHOD(LoadPlaneForSelector)(BSTR strFileName);
	STDMETHOD(SetCaptionHeight)(FLOAT fHeight);
	STDMETHOD(SetCaptionText)(BSTR strName, BSTR strTitle);
	STDMETHOD(GetBBoxXY)(BSTR mdl, FLOAT* x, FLOAT* y);
	STDMETHOD(GetMdlBBoxMax2DLength)(BSTR mdl, FLOAT* len);
	STDMETHOD(GetEditorSceneSize)(LONG ptrScene, LONG* width, LONG* height);
	STDMETHOD(ToggleEngine)();
	STDMETHOD(SetAllPluginModelScale)(FLOAT fScale);
	STDMETHOD(LoadNPCMarkSfxFile(BSTR sfx_name, FLOAT fHeight));
private:
	// active preview indexing
	INT m_index;
	std::map<std::string, INT> m_NameHash;

	// scenes
	//IEKG3DScene* m_pActive3DScene;
	std::vector<IEKG3DScene*> m_p3DScenesVec;

	// windows
	HWND m_hRenderWndBase;
	//INT m_nOutputWindowID;
	std::vector<INT> m_OutputWindowIDs;

	// objects
    //IEKG3DRepresentObject* m_pRepresentObject;
	std::vector<IEKG3DRepresentObject*> m_RepresentObjects;
	std::vector<IEKG3DRepresentObject*> m_RepresentObjectSelectors;
	std::vector<IKG3DModel*> m_ItemModels; // 只是装来释放的，所以无需用vector装vector
	IEKG3DModel* m_pSelectedObj; // 只是用来指示，可以共用

	// input
	std::vector<CInputProxy*> m_InputProxies;
	
	// model to destroy
	IEKG3DModel* m_pLastModel;
};

OBJECT_ENTRY_AUTO(__uuidof(Test3DEngine), CTest3DEngine)


// CTest3DEngine
#define EDITOR_CONFIG_FILE "Config.ini"

#define GETMODELEDITOR() \
	IEKG3DSceneModelEditor* pModelEditor = NULL;\
	m_p3DScenesVec[m_index]->GetSceneModelEditor(&pModelEditor);\
	_ASSERTE(pModelEditor); \
	if(!pModelEditor) return
#define GETSCENEBASE_MODEL() \
	IEKG3DSceneEditorBase* pBase = NULL;\
	m_p3DScenesVec[m_index]->GetSceneEditorBase(&pBase);\
	_ASSERTE(pBase); \
	if(!pBase) return

#define GETEDITOR() \
	IEKG3DSceneSceneEditor* pEditor = NULL;\
	m_p3DScenesVec[m_index]->GetSceneSceneEditor(&pEditor);\
	_ASSERTE(pEditor); \
	if(!pEditor) return
#define GETNPC() \
	IEKG3DRepresentNPC* pNpc = NULL;\
	pEditor->GetRepresentNPC(&pNpc);\
	_ASSERTE(pNpc);\
	if (!pNpc) return
#define GETSCENEBASE() \
	IEKG3DSceneEditorBase* pBase = NULL;\
	pEditor->GetSceneEditorBase(&pBase); \
	_ASSERTE(pBase);\
	if (!pBase) return
#define GETSCENE() \
	IEKG3DScene* pScene = m_p3DScenesVec[m_index];\
	_ASSERTE(pScene); \
	if(!pScene) return
#define GETSELECT() \
	IEKG3DSceneSelectBase* pSelector = NULL; \
	pEditor->GetSceneSelector(&pSelector); \
	_ASSERTE(pSelector); \
	if (!pSelector) \
	return
#define KG_COM_PROCESS_ERROR(Condition) \
	do  \
{   \
	if (FAILED(Condition))  \
	goto Exit0;         \
} while (false)
#define KG_PROCESS_ERROR(Condition) \
	do  \
{   \
	if (!(Condition))   \
	goto Exit0;     \
} while (false)
