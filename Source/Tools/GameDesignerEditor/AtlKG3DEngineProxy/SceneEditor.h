// SceneEditor.h : Declaration of the CSceneEditor

#pragma once
#include "resource.h"       // main symbols
#include "AtlKG3DEngineProxy.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CSceneEditor

class ATL_NO_VTABLE CSceneEditor :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSceneEditor, &CLSID_SceneEditor>,
	public IDispatchImpl<ISceneEditor, &IID_ISceneEditor, &LIBID_AtlKG3DEngineProxyLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
private:
	INT m_nRegionWinID;
	DWORD* m_pdwRegionDiffValue;
	INT m_nRegionDiffValueCount;
public:
	CSceneEditor()
	{
		m_nRegionWinID = -1;
		m_pdwRegionDiffValue = NULL;
		m_nRegionDiffValueCount = INT_MAX;
	}

DECLARE_REGISTRY_RESOURCEID(IDR_SCENEEDITOR)


BEGIN_COM_MAP(CSceneEditor)
	COM_INTERFACE_ENTRY(ISceneEditor)
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
	STDMETHOD(AddWindowing)(BSTR szName, LONG ihWnd, LONG iWindowType, LONG* windowid, ULONG editState);
	STDMETHOD(UpdateTopPosAsMainPos)();
	STDMETHOD(RemoveWindowing)(LONG iWindowID);
	STDMETHOD(GetCameraTopScale)(FLOAT* scale);
	STDMETHOD(SetCameraTopScale)(FLOAT scale);
	STDMETHOD(SetCameraTopPos)(FLOAT x, FLOAT z);
	STDMETHOD(GetCameraTopPos)(AtlVector3* pos);
	STDMETHOD(SetNPCPosition)();
	STDMETHOD(GetCameraPosLookat)(AtlVector3* pos, AtlVector3* lookat);
	STDMETHOD(SetCameraPosLookat)(AtlVector3* pos, AtlVector3* lookat);
	STDMETHOD(SetCameraLocation)(FLOAT camerax, FLOAT cameray, FLOAT cameraz, FLOAT lookatx, FLOAT lookaty, FLOAT lookatz);
	STDMETHOD(GetSelectedEntityCount)(LONG* count);
	STDMETHOD(SetFullScreen)(BOOL bFullScreen);
	STDMETHOD(TogglePMode)();
	STDMETHOD(IsPMode)(BOOL* isPMode);
	STDMETHOD(OnResizeWindow)();
	STDMETHOD(RotateSelectedObject)(BSTR* newRotation);
	STDMETHOD(SetCameraSpeed)(FLOAT fPercent);
	STDMETHOD(NpcPlayNextAction)();
	STDMETHOD(NpcPlayPreviousAction)();
	STDMETHOD(UpdateSelectableEntityList)(LONG doCount);
	STDMETHOD(ClearSelectedEntityList)();
	STDMETHOD(AddSelectedRepresentObject)(LONG representObjPtr, LONG bAdd);
	STDMETHOD(BeginAddPolyVertex)();
	STDMETHOD(EndAddPolyVertex)(ULONG dwType);
	STDMETHOD(SetPolyHeight)(FLOAT fHeight);
	STDMETHOD(EnableRenderPoly)(LONG bEnable);
	STDMETHOD(SetFocus)();
	STDMETHOD(SetRenderBlockEdge)(BOOL bRenderEdge);
	STDMETHOD(SetCameraDis)(FLOAT fDis);
	STDMETHOD(SetFirstSelectedKing)();
	STDMETHOD(DropSelectedEntity)();
	STDMETHOD(ZoomToObj)();
	STDMETHOD(GetEditorSceneSize)(LONG* width, LONG* height);
	STDMETHOD(SetObjectSelectMask)(DWORD nMask);
	STDMETHOD(SetCurrentOutputWindow)(LONG nWindowID);
	STDMETHOD(GetEditState)(LONG* nState);
	STDMETHOD(SetEditState)(LONG nState);
	STDMETHOD(EnableShowAppointLogicalRegion)(LONG bEnable);
	STDMETHOD(SetAppointLogicalRegion)(LONG xStart, LONG zStart, LONG xCount, LONG zCount);
	STDMETHOD(GetAppointLogicalRegion)(LONG* xStart, LONG* zStart, LONG* xCount, LONG* zCount);
	STDMETHOD(GetSetLogicDisplayParams)(LONG bGet, LONG* bShowCell, LONG* bShowTerrainCell, LONG* bShowItemCell, LONG* bShowSelectedCell);
	STDMETHOD(ClearLogicModifyState)();
	STDMETHOD(SetLogicModifyState)(LONG lstate, BSTR scriptName);
	STDMETHOD(SetLogicCurrentColor)(LONG color);
	STDMETHOD(SetLogicBrushParams)(LONG brushwidth, LONG brushtype, LONG displaywidth, LONG brushheight);
	STDMETHOD(SetCurrentEditBrushIndex)(LONG nIndex);
	STDMETHOD(DeleteCurrentCellInfo)();
	STDMETHOD(DeleteSelectedEntity)();
	STDMETHOD(ModifyCurrentCellInfo)(LONG oldInfo, LONG newInfo, BSTR oldScript, BSTR newScript);
	STDMETHOD(TransToEngineFormat)(LONG inMask, LONG* outMask);
	STDMETHOD(BuildPolyAndSetInHand)(LONG VergeNum, FLOAT Radius, FLOAT Height, FLOAT Degree);

	STDMETHOD(SetSnapToolScale)(FLOAT fScale);
	STDMETHOD(SetSpeedTreeLeafScale)(FLOAT fScale);
	STDMETHOD(RenderSceneSnap)(BSTR fileName);
	STDMETHOD(SetRenderTreeFlag)(LONG bFlag);
	STDMETHOD(AddRegionInfoManager)(BSTR fileName, LONG nPixelTolerance, LONG nSampleTimes, LONG nEliminateIsoDataCount, LONG nTotalValue, ULONG uHowManyPixelsInOneRegion, ULONG* dwHandle);
	STDMETHOD(ShowRegionInfoMap)(LONG ihWnd, ULONG dwHandle, LONG bShow);
	STDMETHOD(DelRegionInfoManager)(ULONG dwHandle);
	STDMETHOD(EnableRegionMask)(ULONG dwHandle, LONG nEnable);
	STDMETHOD(GetRegionMapInfo)(ULONG dwHandle, ULONG* puHeight, ULONG* puWidth, ULONG* puPixelPerRegion, ULONG* puRegionCount, ULONG* puMemorySize);
	STDMETHOD(GetRegionDiffValue)(ULONG dwHandle, ULONG uIndex, LONG* pnValue);
	STDMETHOD(SaveRegionMap)(BSTR filePath, ULONG dwHandle);
	};

OBJECT_ENTRY_AUTO(__uuidof(SceneEditor), CSceneEditor)
