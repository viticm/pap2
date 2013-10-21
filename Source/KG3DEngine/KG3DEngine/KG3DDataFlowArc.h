#pragma once

class KG3DDataFlowProcessor;
class KG3DDataFlowDataObject;

class KG3DDataFlowArc
{
private:
	BOOL m_bDataReady;
public:
	UINT m_uDataType;
	DWORD m_dwContainerType;

	struct ConnectionInfo
	{
		UINT uHandleProcessor;
		KG3DDataFlowProcessor* lpProcessor;
		DWORD dwIndex;

		ConnectionInfo()
		{
			uHandleProcessor = 0;
			lpProcessor = NULL;
			dwIndex = 0;
		}
	};

	//////////////////////////////////////////////////////////////////////////
	list<KG3DDataFlowDataObject*>m_listDataObject;

	vector<ConnectionInfo>m_vecInputConnection;//输入的节点
	vector<ConnectionInfo>m_vecOutputConnection;//输出的节点

	string m_szName;

	HRESULT SetName(LPCTSTR pName);
	HRESULT GetName(LPCTSTR pName);

	HRESULT SetType(UINT uDataType,DWORD dwContainerType);
	HRESULT GetType(UINT* puDataType,DWORD* pdwContainerType);

	//////////////////////////////////////////////////////////////////////////
	HRESULT SetNumInputConnection(DWORD dwNum);
	HRESULT GetNumInputConnection(DWORD* pdwNum);
	HRESULT SetNumOutputConnection(DWORD dwNum);
	HRESULT GetNumOutputConnection(DWORD* pdwNum);

	HRESULT SetInputConnection(DWORD dwIndex,UINT uProcessorHandle,KG3DDataFlowProcessor* pProcessor,DWORD dwProcessorConnectionIndex);
	HRESULT GetInputConnection(DWORD dwIndex,UINT* puProcessorHandle,KG3DDataFlowProcessor** ppProcessor,DWORD* pdwProcessorConnectionIndex);
	HRESULT SetOutputConnection(DWORD dwIndex,UINT uProcessorHandle,KG3DDataFlowProcessor* pProcessor,DWORD dwProcessorConnectionIndex);
	HRESULT GetOutputConnection(DWORD dwIndex,UINT* puProcessorHandle,KG3DDataFlowProcessor** ppProcessor,DWORD* pdwProcessorConnectionIndex);

	//////////////////////////////////////////////////////////////////////////
	//计算相关
	HRESULT StartCompute();
	HRESULT EndCompute();
	
	BOOL    IsDataReady();

	HRESULT InputDataList(KG3DDataFlowDataObject* pObject);
	HRESULT OutputData();

public:
	KG3DDataFlowArc(void);
	virtual ~KG3DDataFlowArc(void);
};
