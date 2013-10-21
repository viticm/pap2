#pragma once
#include "scene\kg3dsceneeditorbase.h"

class KG3DDataFlowDataDefine;
class KG3DDataFlowProcessorTable;
class KG3DDataFlowArcTable;

class KG3DRepresentObjectDataFlowProcessor;
class KG3DRepresentObjectDataFlowArc;
class KG3DDataFlowDataObjectTable;

#define EDITSTATE_MAIN       0
#define EDITSTATE_PROCESSOR  1
#define EDITSTATE_EVENT      2

class KG3DSceneDataFlowEditor :
	public IEKG3DSceneDataFlowEditor,
	public KG3DSceneEditorBase
{
	friend class KG3DRepresentObjectDataFlowArc;

public:
	UINT m_uProcessorSystemKeyInput;
	//////////////////////////////////////////////////////////////////////////
	UINT m_uDefineSystemKeyInput;
	UINT m_uDefineINT;
	UINT m_uDefineDCOLOR;
	UINT m_uDefineFLOAT;
	UINT m_uDefineFLOAT2;
	UINT m_uDefineFLOAT3;
	UINT m_uDefineFLOAT4;
	UINT m_uDefineCHAR32;
	UINT m_uDefineCHAR128;
	UINT m_uDefineCHAR256;
	UINT m_uDefineWCHAR32;
	UINT m_uDefineWCHAR128;
	UINT m_uDefineWCHAR256;

	KG3DDataFlowDataObjectTable* m_lpDataTable;
public:
	//////////////////////////////////////////////////////////////////////////
	//针对数据定义的操作
	virtual HRESULT NewOneDataDefine(UINT* pHandle);
	virtual HRESULT DeleteDataDefine(UINT uHandle);
	virtual HRESULT SetDataDefineName(LPCTSTR pName,UINT uHandle);
	virtual HRESULT GetDataDefineName(LPCTSTR pName,UINT uHandle);
	virtual HRESULT SetNumVariableofDataDefine(DWORD dwNumVariable,UINT uHandle);
	virtual HRESULT GetNumVariableofDataDefine(DWORD* pdwNumVariable,UINT uHandle);
	virtual HRESULT SetVariableofDataDefine(DWORD dwIndex,KG3DDATAFLOWVARIABLETYPE eType,LPCTSTR pName,UINT uHandle);
	virtual HRESULT GetVariableofDataDefine(DWORD dwIndex,KG3DDATAFLOWVARIABLETYPE* peType,LPCTSTR pName,UINT uHandle);
	virtual HRESULT GetAllDataDefine(vector<UINT>* pvecHandle);
	virtual HRESULT GetNumVariableType(DWORD* pdwNumType);
	virtual HRESULT GetVariableType(KG3DDATAFLOWVARIABLETYPE* peType,LPCTSTR pTypeName,DWORD dwIndex);

	map<UINT,KG3DDataFlowDataDefine*>m_mapDataDefine;//数据定义
	UINT m_uCurrentDataDefineIndex;
	UINT GetCurrentDataDefineHandle();

	HRESULT SaveDataDefineToFile(LPCTSTR pFileName);
	HRESULT LoadDataDefineFromFile(LPCTSTR pFileName);
	//////////////////////////////////////////////////////////////////////////
	//针对处理单元
	virtual HRESULT GetAllDataProcessorType(vector<DWORD>*pvecProcessorType);
	virtual HRESULT GetDataProcessorDescriptionByType(LPCTSTR pDesc,DWORD dwType);

	virtual HRESULT NewOneDataProcessor(UINT* pHandle,DWORD dwType);
	virtual HRESULT DeleteDataProcessor(UINT pHandle);
	virtual HRESULT GetAllDataProcessor(vector<UINT>* pvecHandle);

	virtual HRESULT SetProcessorInputNode(UINT uHandle,DWORD dwIndex,UINT uDataDefine,DWORD dwContainer);
	virtual HRESULT GetProcessorInputNode(UINT uHandle,DWORD dwIndex,UINT* puDataDefine,DWORD* pdwContainer,BOOL& bDefineLock,BOOL& bContainerLock);//获取连接点信息，后两个参数为确定是否数据定义固定和容器固定
	virtual HRESULT SetProcessorOutputNode(UINT uHandle,DWORD dwIndex,UINT uDataDefine,DWORD dwContainer);
	virtual HRESULT GetProcessorOutputNode(UINT uHandle,DWORD dwIndex,UINT* puDataDefine,DWORD* pdwContainer,BOOL& bDefineLock,BOOL& bContainerLock);//获取连接点信息，后两个参数为确定是否数据定义固定和容器固定

	KG3DDataFlowProcessorTable* m_lpProcessorTable;
	map<UINT,KG3DRepresentObjectDataFlowProcessor*>m_mapRepProcessor;
	HRESULT FindRepProcessor(KG3DRepresentObjectDataFlowProcessor** ppObject,UINT uHandle);
	//////////////////////////////////////////////////////////////////////////
	//针对弧线
	virtual HRESULT NewOneArc(UINT* pHandle);
	virtual HRESULT DeleteArc(UINT uHandle);
	virtual HRESULT SetArcName(UINT uHandle,LPCTSTR pName);
	virtual HRESULT GetArcName(UINT uHandle,LPCTSTR pName);
	virtual HRESULT SetArcType(UINT uHandle,UINT uDataDefine,DWORD dwContainerType);
	virtual HRESULT GetArcType(UINT uHandle,UINT* puDataDefine,DWORD* pdwContainerType);
	virtual HRESULT SetNumArcInputConnection(UINT uHandle,DWORD dwNum);
	virtual HRESULT GetNumArcInputConnection(UINT uHandle,DWORD* pdwNum);
	virtual HRESULT SetNumArcOutputConnection(UINT uHandle,DWORD dwNum);
	virtual HRESULT GetNumArcOutputConnection(UINT uHandle,DWORD* pdwNum);

	virtual HRESULT ConnectArcToProcessorOutput(UINT uArcHandle,DWORD dwArcConnectionIndex,UINT uProcessorHandle,DWORD dwProcessorConnectionIndex);
	virtual HRESULT ConnectArcToProcessorInput(UINT uArcHandle,DWORD dwArcConnectionIndex,UINT uProcessorHandle,DWORD dwProcessorConnectionIndex);

	virtual HRESULT GetAllArc(vector<UINT>* pvecHandle);

	KG3DDataFlowArcTable* m_lpArcTable;

	map<UINT,KG3DRepresentObjectDataFlowArc*>m_mapRepArc;//数据定义
	HRESULT FindRepArc(KG3DRepresentObjectDataFlowArc** ppObject,UINT uHandle);

	//////////////////////////////////////////////////////////////////////////
	HRESULT RenderRunWindow(KG3DSceneOutputWnd* pWnd);
	HRESULT RenderMainWindow(KG3DSceneOutputWnd& wndCur);
	HRESULT RenderPreViewWindow(KG3DSceneOutputWnd* pWnd);
	virtual HRESULT RenderForEachWindow(KG3DSceneOutputWnd& wndCur);

	HRESULT ClearnUp();

	HRESULT Refresh();

	//////////////////////////////////////////////////////////////////////////
	UINT m_uCurrentSelectedProcessor;//当前编辑的处理器
	HRESULT ClearnUpProcessorInnerObjects();
public:
	BOOL    m_bRunning;

	virtual HRESULT ToggleEditCurrentSelectedProcessor(BOOL bEdit);

	KG3DDataFlowDataDefine* FindDataDefine(UINT uHandle);

	virtual HRESULT FrameMove();
	virtual HRESULT RunDataFlow(BOOL bRun);
	virtual HRESULT ProcessRunMessage(UINT message, WPARAM wParam, LPARAM lParam);

	HRESULT CreateDefaultDataDefine();
	HRESULT	CreateSystemKeyInputDefine();

	virtual HRESULT Init();
	virtual HRESULT GetSceneEditorBase(IEKG3DSceneEditorBase** pBase)
	{
		(*pBase) = this;
		return S_OK;
	}
	KG3DSceneDataFlowEditor(void);
	virtual ~KG3DSceneDataFlowEditor(void);
};
