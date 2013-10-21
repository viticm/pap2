#include "stdafx.h"
#include "./kmodelmgr.h"

#define MODEL_TRACE 0

KModelMgr::KSceneObjectItem::KSceneObjectItem()
: nCount(0)
{
	for (int nIndex = 0; nIndex < SCENE_OBJECT_MODEL_MAX_COUNT; ++nIndex)
    {
		ap3DModel[nIndex] = NULL;
    }
}

KModelMgr::KModelMgr()
{
}

KModelMgr::~KModelMgr()
{
}

HRESULT KModelMgr::RegisterModel(IKG3DModel* p3DModel, KSceneObject* pObject, DWORD dwObjectType, DWORD dwObject)
{
#if MODEL_TRACE
    ATLTRACE("KModelMgr::RegisterModel(0x%.8x, 0x%.8x, %u, %u)\n", p3DModel, pObject, dwObjectType, dwObject);
#endif

	int nRetCode = false;
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
	KModelItem item;

	ASSERT(p3DModel);
	ASSERT(pObject);

	nRetCode = !IsModelExist(p3DModel);
	KGLOG_PROCESS_ERROR(nRetCode);

	hr = RegisterSceneObject(pObject, p3DModel);
	KGLOG_COM_PROCESS_ERROR(hr);

	item.pObject = pObject;
	item.dwObjectType = dwObjectType;
	item.dwObject = dwObject;

	m_aModelToSceneObject.insert(make_pair(p3DModel, item));
	
	hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        KGLogPrintf(KGLOG_ERR, "KModelMgr::RegisterModel(0x%.8x, 0x%.8x, %u, %u)\n", p3DModel, pObject, dwObjectType, dwObject);
    }
	return hrResult;
}

HRESULT KModelMgr::UnregisterModel(IKG3DModel* p3DModel)
{
#if MODEL_TRACE
    ATLTRACE("KModelMgr::UnregisterModel(0x%.8x)\n", p3DModel);
#endif

    HRESULT hr = E_FAIL;
	HRESULT hrResult = E_FAIL;
	KSceneObject* pObject = NULL;
	KModelToSceneObject::iterator it;

	ASSERT(p3DModel);

	it = m_aModelToSceneObject.find(p3DModel);
	KGLOG_PROCESS_ERROR(it != m_aModelToSceneObject.end());

	pObject = (*it).second.pObject;

	m_aModelToSceneObject.erase(it);
	
	hr = UnregisterSceneObject(pObject, p3DModel);
	KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KModelMgr::RegisterSceneObject(KSceneObject const* pObject, IKG3DModel* p3DModel)
{
	HRESULT hr = E_FAIL;
	HRESULT hrResult = E_FAIL;
	KSceneObjectItem Item;
	KSceneObjectToModel::iterator it;

	ASSERT(pObject);
	ASSERT(p3DModel);

	it = m_aSceneObjectToModel.find(pObject);

	if (it == m_aSceneObjectToModel.end())
	{
		KSceneObjectItem item;
		item.nCount = 1;
		item.ap3DModel[0] = p3DModel;
		m_aSceneObjectToModel.insert(make_pair(pObject, item));
	}
	else
	{
		KSceneObjectItem& item = (*it).second;
		
		KGLOG_PROCESS_ERROR(item.nCount < _countof(item.ap3DModel));

		for (int nIndex = 0; nIndex < _countof(item.ap3DModel); ++nIndex)
		{
			if (item.ap3DModel[nIndex] == NULL)
			{
				item.ap3DModel[nIndex] = p3DModel;
				++item.nCount;
				break;
			}
		}
	}

	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KModelMgr::UnregisterSceneObject(KSceneObject const* pObject, IKG3DModel* p3DModel)
{
	HRESULT hr = E_FAIL;
	HRESULT hrResult = E_FAIL;
	KSceneObjectToModel::iterator it;

	ASSERT(pObject);
	ASSERT(p3DModel);

	it = m_aSceneObjectToModel.find(pObject);
	KGLOG_PROCESS_ERROR(it != m_aSceneObjectToModel.end());

	KSceneObjectItem& item = (*it).second;
	for (int nIndex = 0; nIndex < _countof(item.ap3DModel); ++nIndex)
	{
		if (item.ap3DModel[nIndex] == p3DModel)
		{
			item.ap3DModel[nIndex] = NULL;
			if (--item.nCount == 0)
			{
				break;
			}
		}
	}

	if (item.nCount == 0)
		m_aSceneObjectToModel.erase(it);
	
	hrResult = S_OK;
Exit0:
	return hrResult;
}

BOOL KModelMgr::IsModelExist(IKG3DModel const* p3DModel) const
{
	return m_aModelToSceneObject.find(p3DModel) != m_aModelToSceneObject.end();
}

BOOL KModelMgr::IsSceneObjectExit(KSceneObject const* pSceneObject) const
{
	return m_aSceneObjectToModel.find(pSceneObject) != m_aSceneObjectToModel.end();
}

HRESULT KModelMgr::Lookup(KSceneObject** ppObject, PDWORD pdwObjectType, PDWORD pdwObject, IKG3DModel const* p3DModel) const
{
	HRESULT hrResult = E_FAIL;
	KModelToSceneObject::const_iterator it;

	ASSERT(ppObject);
	ASSERT(*ppObject == NULL);
	ASSERT(pdwObjectType);
	ASSERT(pdwObject);
	ASSERT(p3DModel);

	it = m_aModelToSceneObject.find(p3DModel);
	KG_PROCESS_ERROR(it != m_aModelToSceneObject.end());

	KModelItem const& item = (*it).second;

	*ppObject = item.pObject;
	*pdwObjectType = item.dwObjectType;
	*pdwObject = item.dwObject;

	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KModelMgr::Lookup(IKG3DModel* ap3DModel[SCENE_OBJECT_MODEL_MAX_COUNT], int& nCount, KSceneObject const* pObject) const
{
	HRESULT hrResult = E_FAIL;
	KSceneObjectToModel::const_iterator it;

	ASSERT(ap3DModel);
	ASSERT(pObject);

	memset(ap3DModel, 0, sizeof(IKG3DModel*) * SCENE_OBJECT_MODEL_MAX_COUNT);

	it = m_aSceneObjectToModel.find(pObject);
	KG_PROCESS_ERROR(it != m_aSceneObjectToModel.end());

	KSceneObjectItem const& item = (*it).second;

	nCount = 0;

	for (int nIndex = 0; nIndex < _countof(item.ap3DModel); ++nIndex)
	{
		if (item.ap3DModel[nIndex] == NULL)
			continue;

		ap3DModel[nCount++] = item.ap3DModel[nIndex];
	}

	ASSERT(nCount == item.nCount);

	hrResult = S_OK;
Exit0:
	return hrResult;
}
