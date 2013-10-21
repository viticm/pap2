#pragma once

enum KG3DDataFlowProcessorType
{
	DATA_PROCESSOR_NONE = 0,

	DATA_PROCESSOR_DATACREATER,
	DATA_PROCESSOR_DATADESTROYER,
	DATA_PROCESSOR_DATAMODIFY,

	DATA_PROCESSOR_ALGORITHM,


	DATA_PROCESSOR_SYSTEMINPUT,
	DATA_PROCESSOR_SYSTEMOUTPUT,

	DATA_PROCESSOR_END,//放在末尾
	DATA_PROCESSOR_FORCEDWORD = 0xFFFFFFFF,
};

#define CONTAINER_NONE   0
#define CONTAINER_SINGLE 1
#define CONTAINER_LIST   2
#define CONTAINER_ARRAY  3

class KG3DDataFlowArc;
class KG3DSceneDataFlowEditor;
class KG3DDataFlowProcessorDataModifier;
class KG3DRepresentObjectDataFlowArc;
class KG3DRepresentObjectDataFlowProcessor;

class KG3DDataFlowProcessor
{
	friend class KG3DDataFlowProcessorTable;

private:

	KG3DDataFlowProcessorTable* m_lpTable;
	HRESULT SetTable(KG3DDataFlowProcessorTable* pTable)
	{
		m_lpTable = pTable;
		return S_OK;
	}
	HRESULT SetHandle(UINT handle)
	{
		m_uHandle = handle;
		return S_OK;
	}

	UINT m_uHandle;
	ULONG m_ulRefCount;
	KG3DDataFlowProcessorType m_eProcessorType;
protected:
	KG3DSceneDataFlowEditor* m_lpEditor;

	struct ConnectionNode//弧线连接点
	{
		UINT uDataDefine;
		DWORD dwContainerType;
		UINT  uArcHandle;
		KG3DDataFlowArc* lpArc;
		DWORD dwArcConnectionIndex;

		BOOL bDefineLock;//固定某种格式
		BOOL bContainerLock;//固定某种容器
		BOOL bHasInnerNode;
		UINT uInnerNodeHandle;
		ConnectionNode()
		{
			uDataDefine = 0;
			dwContainerType = 0;
			uArcHandle = 0;
			lpArc = NULL;
			dwArcConnectionIndex = 0;

			bDefineLock = FALSE;
			bContainerLock = FALSE;
			bHasInnerNode = FALSE;
			uInnerNodeHandle = 0;
		}
	};

	vector<ConnectionNode>m_vecInputNode;//输入连接点
	vector<ConnectionNode>m_vecOutputNode;//输出连接点

	//////////////////////////////////////////////////////////////////////////
public:
	map<UINT,KG3DDataFlowProcessor*>m_mapInnerProcessor;
	map<UINT,KG3DDataFlowArc*>m_mapInnerArc;
	map<UINT,KG3DRepresentObjectDataFlowProcessor*>m_mapInnerRepProcessor;
	map<UINT,KG3DRepresentObjectDataFlowArc*>m_mapInnerRepArc;


	HRESULT SetProcessType(KG3DDataFlowProcessorType eType);


public:
	HRESULT NewOneInnerProcessor(UINT* pHandle,DWORD dwType);
	HRESULT DeleteInnerProcessor(UINT uHandle);
	HRESULT NewOneInnerArc(UINT* pHandle);
	HRESULT DeleteInnerArc(UINT uHandle);

	virtual HRESULT CreateInnerDataModifer();
	HRESULT ClearInnerObject();

	UINT GetHandle()
	{
		return m_uHandle;
	}

	DWORD GetNumInputNode();
	DWORD GetNumOutPutNode();

	KG3DDataFlowProcessorType GetProcessType();

	virtual HRESULT DoProcess();//处理数据

	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release();

	virtual HRESULT Init(KG3DSceneDataFlowEditor* pEditor);

	HRESULT SetInputNodeType(DWORD dwIndex,UINT uDataDefine,DWORD dwContainerType);
	HRESULT GetInputNodeType(DWORD dwIndex,UINT* puDataDefine,DWORD* pdwContainerType,BOOL& bDefineLock,BOOL& bContainerLock);
	HRESULT SetOutputNodeType(DWORD dwIndex,UINT uDataDefine,DWORD dwContainerType);
	HRESULT GetOutputNodeType(DWORD dwIndex,UINT* puDataDefine,DWORD* pdwContainerType,BOOL& bDefineLock,BOOL& bContainerLock);

	KG3DDataFlowProcessorDataModifier* GetInputDataModifier(DWORD dwIndex);
	KG3DDataFlowProcessorDataModifier* GetOutputDataModifier(DWORD dwIndex);

	HRESULT ConnectArcInputNode(DWORD dwIndex,UINT uArcHandle,KG3DDataFlowArc* pArc,DWORD dwArcConnectionIndex);
	HRESULT ConnectArcOutputNode(DWORD dwIndex,UINT uArcHandle,KG3DDataFlowArc* pArc,DWORD dwArcConnectionIndex);

	HRESULT Refresh(KG3DSceneDataFlowEditor* pEditor);

	KG3DDataFlowProcessor(void);
	virtual ~KG3DDataFlowProcessor(void);
};

class KG3DDataFlowProcessorDataCreater://数据对象创建
	public KG3DDataFlowProcessor
{

};

class KG3DDataFlowProcessorDataDestroyer://数据对象销毁
	public KG3DDataFlowProcessor
{

};

class KG3DDataFlowProcessorDataModifier :
	public KG3DDataFlowProcessor
{
private:
	UINT m_uDataDefine;
public:
	UINT GetDataDefine()
	{
		return m_uDataDefine;
	}
	HRESULT SetDataDefine(UINT uDataDefine)
	{
		m_uDataDefine = uDataDefine;
		return S_OK;
	}
	//////////////////////////////////////////////////////////////////////////

	virtual HRESULT Init(KG3DSceneDataFlowEditor* pEditor);
	HRESULT CreateConnectionNodes();

};

class KG3DDataFlowProcessorAlgorithm://运算器
	public KG3DDataFlowProcessor
{
public:
	virtual HRESULT Init(KG3DSceneDataFlowEditor* pEditor);
};
 
class KG3DDataFlowProcessorSystemInput://系统数据输入
	public KG3DDataFlowProcessor
{
public:
	virtual HRESULT Init(KG3DSceneDataFlowEditor* pEditor);

	virtual HRESULT ProcessRunMessage(UINT message, WPARAM wParam, LPARAM lParam);

};

class KG3DDataFlowProcessorSystemOutPut://系统数据输出
	public KG3DDataFlowProcessor
{

};

