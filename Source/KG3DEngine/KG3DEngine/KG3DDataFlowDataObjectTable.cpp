#include "StdAfx.h"
#include "KG3DDataFlowDataObjectTable.h"
#include "KG3DDataFlowDataObject.h"
#include "KG3DDataFlowDataDefine.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KG3DDataFlowDataObjectTable::KG3DDataFlowDataObjectTable(void)
{
	m_uCurrentHandle = 1;
}

KG3DDataFlowDataObjectTable::~KG3DDataFlowDataObjectTable(void)
{
	Clear();
}

KG3DDataFlowDataObjectTable::DataObjectSet::DataObjectSet()
{
	m_dwMaxCountInPool = 100;
}

KG3DDataFlowDataObjectTable::DataObjectSet::~DataObjectSet()
{
	Clear();
}

HRESULT KG3DDataFlowDataObjectTable::DataObjectSet::Clear()
{
	{
		list<KG3DDataFlowDataObject*>::iterator i = m_listDataObjects.begin();//数据对象
		list<KG3DDataFlowDataObject*>::iterator iend = m_listDataObjects.end();//数据对象
		while (i!=iend)
		{
			KG3DDataFlowDataObject* pData = *i;
			SAFE_DELETE(pData);
			i++;
		}
		m_listDataObjects.clear();
	}
	{
		list<KG3DDataFlowDataObject*>::iterator i = m_listDataObjectsPool.begin();//数据对象
		list<KG3DDataFlowDataObject*>::iterator iend = m_listDataObjectsPool.end();//数据对象
		while (i!=iend)
		{
			KG3DDataFlowDataObject* pData = *i;
			SAFE_DELETE(pData);
			i++;
		}
		m_listDataObjectsPool.clear();
	}

	return S_OK;
}

HRESULT KG3DDataFlowDataObjectTable::DataObjectSet::NewOneData(KG3DDataFlowDataObject** ppData,KG3DDataFlowDataDefine* pDefine,KG3DDataFlowDataObjectTable* pTable)
{
	HRESULT hr = S_OK;
	KG3DDataFlowDataObject* pData = NULL;
	if(m_listDataObjectsPool.size())
	{
		pData = *m_listDataObjectsPool.begin();
		hr = pData->Init(pTable->GetCurrentHandle(),pDefine->GetDataLength());
		KGLOG_COM_PROCESS_ERROR(hr);

		m_listDataObjectsPool.pop_front();
	}
	else
	{
		pData = new KG3DDataFlowDataObject();
		KGLOG_PROCESS_ERROR(pData);
		hr = pData->Init(pTable->GetCurrentHandle(),pDefine->GetDataLength());
		KGLOG_COM_PROCESS_ERROR(hr);
	}

	m_listDataObjects.push_back(pData);
	(*ppData) = pData;
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DDataFlowDataObjectTable::DataObjectSet::ReleaseOneData(KG3DDataFlowDataObject* pData,KG3DDataFlowDataDefine* pDefine)
{
	m_listDataObjects.remove(pData);
	if(m_listDataObjectsPool.size()<m_dwMaxCountInPool)
	{
		m_listDataObjectsPool.push_back(pData);
	}
	else
	{
		SAFE_DELETE(pData);
	}
	return S_OK;
}

HRESULT KG3DDataFlowDataObjectTable::NewOneData(KG3DDataFlowDataObject** ppData,KG3DDataFlowDataDefine* pDefine)
{
	DataObjectSet* pSet = NULL;
	map<DWORD,DataObjectSet*>::iterator iFind = m_mapDataObjectSet.find(pDefine->m_dwType);
	if (iFind == m_mapDataObjectSet.end())
	{
		pSet = new DataObjectSet;
		KGLOG_PROCESS_ERROR(pSet);

		m_mapDataObjectSet[pDefine->m_dwType] = pSet; 
	}
	else
	{
		pSet = iFind->second;
	}

	return pSet->NewOneData(ppData,pDefine,this);
Exit0:
	return E_FAIL;
}

HRESULT KG3DDataFlowDataObjectTable::ReleaseOneData(KG3DDataFlowDataObject* pData,KG3DDataFlowDataDefine* pDefine)
{
	DataObjectSet* pSet = NULL;
	map<DWORD,DataObjectSet*>::iterator iFind = m_mapDataObjectSet.find(pDefine->m_dwType);
	if (iFind == m_mapDataObjectSet.end())
	{
		return E_FAIL;
	}
	else
	{
		pSet = iFind->second;
		return pSet->ReleaseOneData(pData,pDefine);
	}
}

HRESULT KG3DDataFlowDataObjectTable::Clear()
{
	map<DWORD,DataObjectSet*>::iterator i = m_mapDataObjectSet.begin();
	map<DWORD,DataObjectSet*>::iterator iend = m_mapDataObjectSet.end();
	while (i!=iend)
	{
		DataObjectSet* pSet = i->second;
		SAFE_DELETE(pSet);
		i++;
	}
	m_mapDataObjectSet.clear();

	return S_OK;
}
