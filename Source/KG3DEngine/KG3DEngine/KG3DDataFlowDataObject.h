#pragma once

class KG3DDataFlowDataDefine;

class KG3DDataFlowDataObject
{
public:

	UINT m_uHandle;
	BYTE* m_lpData;
	DWORD m_dwSize;

	HRESULT Init(UINT uHandle,DWORD dwSize);
	HRESULT UnInit();

	KG3DDataFlowDataObject();
	virtual ~KG3DDataFlowDataObject();

	HRESULT SetInt(int Value,DWORD dwIndex,KG3DDataFlowDataDefine* pDefine);
	HRESULT GetInt(int* pValue,DWORD dwIndex,KG3DDataFlowDataDefine* pDefine);
protected:

	BYTE* pGetVariablePointer(DWORD dwIndex,KG3DDataFlowDataDefine* pDefine);
};

//typedef BYTE KG3DDataFlowDataObject ;


class KG3DDataFlowDataObjectContainerBase//数据容器基类
{

};

class KG3DDataFlowDataObjectContainerSingle :
	public KG3DDataFlowDataObjectContainerBase//单个数据对象的容器，用作开关和统计数据计算.该数据容器中的数据无需添加，自动管理创建和删除
{

};

class KG3DDataFlowDataObjectContainerList :
	public KG3DDataFlowDataObjectContainerBase//队列型容器，适合可变个数的数据
{

};

class KG3DDataFlowDataObjectContainerArray :
	public KG3DDataFlowDataObjectContainerBase//数组型容器，适合固定个数的数据
{

};