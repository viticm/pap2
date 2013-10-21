#ifndef CASE_RENDER_K_MODEL_MGR_H
#define CASE_RENDER_K_MODEL_MGR_H

#include "./krlresource.h"

class KSceneObject;
class IKG3DModel;

class KModelMgr
{
private:
	struct KModelItem
	{
		KSceneObject*	pObject;
		DWORD			dwObjectType;
		DWORD			dwObject;
	};

	struct KSceneObjectItem
	{
		KSceneObjectItem();

		IKG3DModel* ap3DModel[SCENE_OBJECT_MODEL_MAX_COUNT];
		int			nCount;
	};

	typedef stdext::hash_map<IKG3DModel const*, KModelItem>			KModelToSceneObject;
	typedef stdext::hash_map<KSceneObject const*, KSceneObjectItem>	KSceneObjectToModel;

public:
	KModelMgr();
	~KModelMgr();

	HRESULT RegisterModel(IKG3DModel* p3DModel, KSceneObject* pObject, DWORD dwObjectType, DWORD dwObject);
	HRESULT UnregisterModel(IKG3DModel* p3DModel);

	HRESULT Lookup(KSceneObject** ppObject, PDWORD pdwObjectType, PDWORD pdwObject, IKG3DModel const* p3DModel) const;
	HRESULT Lookup(IKG3DModel* ap3DModel[SCENE_OBJECT_MODEL_MAX_COUNT], int& nCount, KSceneObject const* pObject) const;
	
private:
	BOOL IsModelExist(IKG3DModel const* p3DModel) const;
	BOOL IsSceneObjectExit(KSceneObject const* pSceneObject) const;

	HRESULT RegisterSceneObject(KSceneObject const* pObject, IKG3DModel* p3DModel);
	HRESULT UnregisterSceneObject(KSceneObject const* pObject, IKG3DModel* p3DModel);

private:
	KModelToSceneObject	m_aModelToSceneObject;
	KSceneObjectToModel	m_aSceneObjectToModel;
};

#endif // CASE_RENDER_K_MODEL_MGR_H