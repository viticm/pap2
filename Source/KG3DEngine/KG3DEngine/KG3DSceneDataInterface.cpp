#include "StdAfx.h"
#include "KG3DSceneDataInterface.h"
#include "KG3DSceneBlock.h"
#include "KG3DSceneEntity.h"
#include "KG3DMemoryFile.h"
#include "KG3DRepresentObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define  CONTAINER_WIDTH 6400

extern KG3DMemoryFile g_cMemoryFile;
extern bool operator< (const POINT& _Left, const POINT& _Right);
extern DWORD g_dwEntityNum;

extern int g_nPackFile;
BOOL g_bUseNewLoad = TRUE;

void SceneBlockFindLength(int nLevel,float& BlockLength,float& CellLength)
{
	switch(nLevel)
	{
	case BLOCK_TYPE_REGION:
		CellLength = 100;
		BlockLength = 1600;
		break;
	case BLOCK_TYPE_SECTION:
		CellLength = 800;
		BlockLength = 12800;
		break;	
	case BLOCK_TYPE_ZONE:
		CellLength = 6400;
		BlockLength = 102400;
		break;
	case BLOCK_TYPE_AREA:
		CellLength = 51200;
		BlockLength = 819200;
		break;
	}
}

D3DXVECTOR4 GetSceneBlockRect(int nLevel,int nX,int nZ)
{
	float BlockLength,CellLength;
	D3DXVECTOR4 vBlockRect;

	SceneBlockFindLength(nLevel,BlockLength,CellLength);

	vBlockRect.x = nX * BlockLength;
	vBlockRect.y = nZ * BlockLength;
	vBlockRect.z = BlockLength;
	vBlockRect.w = BlockLength;

	return vBlockRect;
}

void GetSceneBlockRect(int nLevel,int nX,int nZ,D3DXVECTOR2& A,D3DXVECTOR2 &C)
{
	float BlockLength,CellLength;
	D3DXVECTOR4 vBlockRect;

	SceneBlockFindLength(nLevel,BlockLength,CellLength);

	A.x = nX * BlockLength;
	A.y = nZ * BlockLength;
	C.x = A.x + BlockLength;
	C.y = A.y + BlockLength;
}

BOOL IsIntersectBlockRect(D3DXVECTOR4& blockRect,D3DXVECTOR2& A,D3DXVECTOR2& C)//如果有相交也算
{
	if ((A.x > blockRect.x + blockRect.z ) || (A.y > blockRect.y + blockRect.w ))
		return FALSE;
	if ((C.x < blockRect.x) || (C.y < blockRect.y))
		return FALSE;
	return TRUE;
}

HRESULT KG3DSceneDataInterfaceCore::Request_CreateNewEntity(UINT* pHandle,UINT uEntityInfo, int bForceLoad)
{
	KG3DEntityDataEx* pNewEntity = m_cEntityDataExPool.RequestResource();
	KGLOG_PROCESS_ERROR(pNewEntity);

	pNewEntity->m_uHandle = GetCurrentEntityID();
	(*pHandle) = pNewEntity->m_uHandle;
	m_mapEntity[pNewEntity->m_uHandle ] = pNewEntity;

	//InputEntity(pNewEntity);

	pNewEntity->m_uEntityInformationHandle = uEntityInfo;

	if(m_lpEventConnection)
	{
		m_lpEventConnection->OnEntityCreate(pNewEntity->m_uHandle, bForceLoad);
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneDataInterfaceCore::Request_DeleteEntity(UINT uHandle)
{
	map<UINT,KG3DEntityDataEx*>::iterator i = m_mapEntity.find(uHandle);
	if (i==m_mapEntity.end())
	{
		return E_FAIL;
	}
	else
	{
		KG3DEntityDataEx* pEntity = i->second;
		pEntity->RemoveFromContainers();
		m_listEntityDeleted.push_back(pEntity);

		m_mapEntity.erase(i);
		if (m_lpEventConnection)
		{
			m_lpEventConnection->OnEntityDelete(uHandle);
		}
		return S_OK;
	}
}

HRESULT KG3DSceneDataInterfaceCore::Request_ModifyEntityTransform(UINT uHandle,KG3DEntityTransformation& vTransformation )
{
	map<UINT,KG3DEntityDataEx*>::iterator i = m_mapEntity.find(uHandle);
	if (i==m_mapEntity.end())
	{
		return E_FAIL;
	}
	else
	{
		KG3DEntityDataEx* pEntity = i->second;
		//////////////////////////////////////////////////////////////////////////
		pEntity->RecordHistoryNode();

		pEntity->RemoveFromContainers();

		pEntity->m_Transformation = vTransformation;


		InputEntity(pEntity);

		if (m_lpEventConnection)
		{
			m_lpEventConnection->OnEntityTransformModified(uHandle, TRUE);
		}
		return S_OK;
	}
}

//HRESULT KG3DSceneDataInterfaceCore::Request_ModifyEntityInformation(KG3DRepresentObject* pObj)
//{
//    if (!pObj)
//        return E_FAIL;
//
//    map<UINT, KG3DEntityDataEx*>::iterator ifind = m_mapEntity.find(pObj->GetHandleHeldBySpaceMgr());
//
//    if (ifind == m_mapEntity.end())
//        return E_FAIL;
//
//    KG3DEntityDataEx*  pEntity = ifind->second;
//    KG3DSceneDataClip* pInfo = FindEntityInforamtion(pEntity->m_uEntityInformationHandle);
//
//    if (!pInfo)
//        return E_FAIL;
//
//    g_cMemoryFile.Reset();
//    pObj->SaveToInformationBuffer(&g_cMemoryFile);
//    DWORD dwInfoLen = g_cMemoryFile.GetLength();
//    pInfo->SetData(pObj->GetType(), dwInfoLen, g_cMemoryFile.GetBuffer());
//
//    if (m_lpEventConnection)
//        m_lpEventConnection->OnEntityInformationModified(pObj->GetHandleHeldBySpaceMgr());
//
//    return S_OK;
//}


HRESULT KG3DSceneDataInterfaceCore::Request_ModifyEntityInformation(UINT uHandle,DWORD dwDataType,DWORD dwLength,PVOID pData)
{
	HRESULT hr = E_FAIL;

	map<UINT,KG3DEntityDataEx*>::iterator i = m_mapEntity.find(uHandle);
	if (i==m_mapEntity.end())
	{
		return E_FAIL;
	}
	else
	{
		KG3DEntityDataEx* pEntity = i->second;

		UINT uNewHandle = 0;

		KG3DSceneDataClip* pInfo = NULL;
		hr = AddEntityInformation(&pInfo,uNewHandle,dwDataType,dwLength,pData);
		KGLOG_COM_PROCESS_ERROR(hr);

		if(uNewHandle != pEntity->m_uEntityInformationHandle)
		{
			//////////////////////////////////////////////////////////////////////////
			pEntity->RecordHistoryNode();

			KG3DSceneDataClip* pOldInfo = FindEntityInforamtion(pEntity->m_uEntityInformationHandle);
			if(pOldInfo)
			{
				;//do noting;
			}
			pEntity->m_uEntityInformationHandle = uNewHandle;

			//if(pOldInfo)
			//{
			//	//pOldInfo->RemoveRefrenceEntity(uHandle);
			//
			//	if(pOldInfo->m_setRefrenceEntity.size() == 0)//无人引用，本应该清除出链表，这里暂时不实现
			//	{

			//	}
			//}
			//if(pInfo)
			//	pInfo->AddRefrenceEntity(uHandle);


			if (m_lpEventConnection)
			{
				m_lpEventConnection->OnEntityInformationModified(uHandle);
			}
		}

		return S_OK;
	}

	hr = S_OK;
Exit0:
	return hr;
}


HRESULT KG3DSceneDataInterfaceCore::GetEntityInformation(UINT uHandle,DWORD* pdwDataType,DWORD* pdwLength,PVOID pData,DWORD dwMaxLen)
{
	HRESULT hr = E_FAIL;

	map<UINT,KG3DEntityDataEx*>::iterator i = m_mapEntity.find(uHandle);
	if (i==m_mapEntity.end())
	{
		return E_FAIL;
	}
	else
	{
		KG3DEntityDataEx* pEntity = i->second;

		KG3DSceneDataClip* pEntityInfo = FindEntityInforamtion(pEntity->m_uEntityInformationHandle);
		KGLOG_PROCESS_ERROR(pEntityInfo);

		(*pdwDataType) = pEntityInfo->m_dwType;
		(*pdwLength) = pEntityInfo->m_dwLength;
		return pEntityInfo->GetData(pData);
	}

	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DSceneDataInterfaceCore::GetTerrainInformation(UINT uHandle,DWORD* pdwDataType,DWORD* pdwLength,PVOID pData)
{
	HRESULT hr = E_FAIL;

	KG3DSceneDataClip* pTerrainInfo = FindTerrainInforamtion(uHandle);
	KGLOG_PROCESS_ERROR(pTerrainInfo);

	(*pdwDataType) = pTerrainInfo->m_dwType;
	(*pdwLength) = pTerrainInfo->m_dwLength;
	return pTerrainInfo->GetData(pData);
	
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DSceneDataInterfaceCore::UpdateObjTransToData(KG3DRepresentObject* obj)
{
    if (!obj)
        return E_FAIL;

    map<UINT,KG3DEntityDataEx*>::iterator i = m_mapEntity.find(obj->GetHandleHeldBySpaceMgr());
    if (i==m_mapEntity.end())
    {
        return E_FAIL;
    }
    else
    {
        KG3DEntityDataEx* pEntity = i->second;
        if (pEntity)
        {
            pEntity->m_Transformation.m_Rotation = obj->m_Rotation;
            pEntity->m_Transformation.m_Scaling = obj->m_Scaling;
            pEntity->m_Transformation.m_Translation = obj->m_Translation;
            pEntity->m_Transformation.m_vBox_A = obj->m_BBox.A;
            pEntity->m_Transformation.m_vBox_B = obj->m_BBox.B;
            pEntity->m_Transformation.m_vWorldPos = obj->GetWorldPos();
        }
        return S_OK;
    }
}

HRESULT KG3DSceneDataInterfaceCore::GetEntityTransformation(UINT uHandle,KG3DEntityTransformation& vTransformation)
{
	map<UINT,KG3DEntityDataEx*>::iterator i = m_mapEntity.find(uHandle);
	if (i==m_mapEntity.end())
	{
		return E_FAIL;
	}
	else
	{
		KG3DEntityDataEx* pEntity = i->second;
		vTransformation = pEntity->m_Transformation;
		return S_OK;
	}
}

HRESULT KG3DSceneDataInterfaceCore::GetEntityIntersectRect(set<UINT>*psetEntity,const D3DXVECTOR2& vA ,const D3DXVECTOR2& vC)
{
	HRESULT hr = E_FAIL;
	vector<POINT>vecContainer[4];
	hr = GetRectIntersectContainers(&vecContainer[0],vA,vC,BLOCK_TYPE_REGION);
	hr = GetRectIntersectContainers(&vecContainer[1],vA,vC,BLOCK_TYPE_SECTION);
	hr = GetRectIntersectContainers(&vecContainer[2],vA,vC,BLOCK_TYPE_ZONE);
	hr = GetRectIntersectContainers(&vecContainer[3],vA,vC,BLOCK_TYPE_AREA);

	for (int L=0;L<4;L++)
	{
		for (size_t i=0;i<vecContainer[L].size();i++)
		{
			POINT p = vecContainer[L][i];

			map<POINT,KG3DSceneBlockData*>::iterator s = m_mapSceneBlockData[L].find(p);

			if(s!=m_mapSceneBlockData[L].end())
			{
				KG3DSceneBlockData* pContainer = s->second;

				set<KG3DEntityDataEx*>::iterator n = pContainer->m_setEntity.begin();
				set<KG3DEntityDataEx*>::iterator nEnd = pContainer->m_setEntity.end();
				while (n!=nEnd)
				{
					KG3DEntityDataEx* pEntity = *n;

					psetEntity->insert(pEntity->m_uHandle);

					n++;
				}
			}
		}
	}
	

	return S_OK;
}

HRESULT KG3DSceneDataInterfaceCore::AddEventConnection(KG3DSceneDataEventConnection* pConnection)
{
	m_lpEventConnection = pConnection;
	return S_OK;
}

HRESULT KG3DSceneDataInterfaceCore::RemoveEventConnection(KG3DSceneDataEventConnection* pConnection)
{
	m_lpEventConnection = NULL;
	return S_OK;
}

KG3DSceneDataInterfaceCore::KG3DSceneDataInterfaceCore()
{
	m_uCurrentEntityID = 1;
	m_uCurrentEntityInformationID = 1;
	m_uCurrentTerrainInformationID = 1;
	m_lpEventConnection = NULL;
    m_dwVersion = MAP_FILE_VERSION;
	m_dwMemoryUse = 0;
	m_bPackData = FALSE;
}

KG3DSceneDataInterfaceCore::~KG3DSceneDataInterfaceCore()
{
	UnInit();
	m_lpEventConnection = NULL;
}

UINT KG3DSceneDataInterfaceCore::GetCurrentEntityID()
{
	return m_uCurrentEntityID++;
}

UINT KG3DSceneDataInterfaceCore::GetCurrentEntityInforamtionID()
{
	return m_uCurrentEntityInformationID++;
}

UINT KG3DSceneDataInterfaceCore::GetCurrentTerrainInforamtionID()
{
	return m_uCurrentTerrainInformationID++;
}

HRESULT KG3DSceneDataInterfaceCore::InitDirectory(LPCTSTR pFileName,BOOL bRead,BOOL bWrite)
{
	TCHAR szName[MAX_PATH];

	TCHAR path_buffer[256];
	TCHAR drive[256];
	TCHAR dir[256];
	TCHAR fname[256];
	TCHAR ext[256];

	wsprintf(path_buffer,"%s",pFileName);
	_splitpath( path_buffer, drive, dir, fname, ext );

	TCHAR szFileName[MAX_PATH];
	wsprintf(szFileName,"%s%s%s",drive,dir,fname);

	if(bRead)
	{
		wsprintf(szName,"%sFile",szFileName);
		m_szDirectoryRead = szName;

		_tmkdir(m_szDirectoryRead.c_str());
	}

	if(bWrite)
	{
		wsprintf(szName,"%sFile",szFileName);
		m_szDirectoryWrite = szName;

		_tmkdir(m_szDirectoryWrite.c_str());
	}

	return S_OK;
}

HRESULT KG3DSceneDataInterfaceCore::Init(LPCTSTR pFileName,BOOL bNewScene)
{
	InitDirectory(pFileName,TRUE,TRUE);
	if(bNewScene && g_nPackFile)
	{
		m_bPackData = TRUE;
	}
	return S_OK;
}

HRESULT KG3DSceneDataInterfaceCore::UnInit()
{
	list<KG3DEntityDataEx*>::iterator j = m_listEntityDeleted.begin();
	list<KG3DEntityDataEx*>::iterator jend = m_listEntityDeleted.end();
	while (j!=jend)
	{
		KG3DEntityDataEx* pEntity = *j;
		m_cEntityDataExPool.FreeResource(pEntity);
		++j;
	}
	m_listEntityDeleted.clear();
	//////////////////////////////////////////////////////////////////////////
	map<UINT,KG3DEntityDataEx*>::iterator i = m_mapEntity.begin();
	map<UINT,KG3DEntityDataEx*>::iterator iend = m_mapEntity.end();
	while (i!=iend)
	{
		KG3DEntityDataEx* pEntity = i->second;
		m_cEntityDataExPool.FreeResource(pEntity);
		++i;
	}
	m_mapEntity.clear();	
	//////////////////////////////////////////////////////////////////////////
	{
		map<UINT,KG3DSceneDataClip*>::iterator s = m_mapEntityInformation.begin();
		map<UINT,KG3DSceneDataClip*>::iterator send = m_mapEntityInformation.end();
		while (s!=send)
		{
			KG3DSceneDataClip* pInfo = s->second;
			m_cDataClipPool.FreeResource(pInfo);
			++s;
		}
		m_mapEntityInformation.clear();
	}
	{
		map<UINT,KG3DSceneDataClip*>::iterator s = m_mapTerrainInformation.begin();
		map<UINT,KG3DSceneDataClip*>::iterator send = m_mapTerrainInformation.end();
		while (s!=send)
		{
			KG3DSceneDataClip* pInfo = s->second;
			m_cDataClipPool.FreeResource(pInfo);
			++s;
		}
		m_mapTerrainInformation.clear();
	}	
	
	//////////////////////////////////////////////////////////////////////////
	{
		for (int i=0;i<4;i++)
		{
			map<POINT,KG3DSceneBlockData*>::iterator s = m_mapSceneBlockData[i].begin();//地形REGION数据
			map<POINT,KG3DSceneBlockData*>::iterator send = m_mapSceneBlockData[i].end();
			while (s!=send)
			{
				KG3DSceneBlockData* pBlock = s->second;
				m_cSceneBlockDataPool.FreeResource(pBlock);
				++s;
			}
			m_mapSceneBlockData[i].clear();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	{
		list<KG3DSceneBlockData*>::iterator s = m_listTerrainSectionDeleted.begin();//地形REGION数据
		list<KG3DSceneBlockData*>::iterator send = m_listTerrainSectionDeleted.end();
		while (s!=send)
		{
			KG3DSceneBlockData* pBlock = *s;
			m_cSceneBlockDataPool.FreeResource(pBlock);
			++s;
		}
		m_listTerrainSectionDeleted.clear();
	}

	m_cEntityDataExPool.UnInit();
	m_cSceneBlockDataPool.UnInit();

	m_cDataClipPool.UnInit();
	return S_OK;
}

HRESULT KG3DSceneDataInterfaceCore::GetRectIntersectContainers(vector<POINT>*pvecContainer,const D3DXVECTOR2& A,const D3DXVECTOR2& C,int nLevel)
{
	float CellLength  = 0;
	float BlockLength = 0;

	SceneBlockFindLength(nLevel,BlockLength,CellLength);

	
	long lXStart = (long)(A.x / BlockLength);
	long lXEnd   = (long)(C.x / BlockLength);
	long lZStart = (long)(A.y / BlockLength);
	long lZEnd   = (long)(C.y / BlockLength);

	for (long Z = lZStart;Z<=lZEnd;Z++)
	{
		for (long X=lXStart;X<=lXEnd;X++)
		{
			POINT p;
			p.x = X;
			p.y = Z;
			pvecContainer->push_back(p);
		}
	}
	return S_OK;
}

HRESULT KG3DSceneDataInterfaceCore::TryInputEntityToLevel(KG3DEntityDataEx* pEntity,D3DXVECTOR2 A,D3DXVECTOR2 C,int nLevel)
{
	HRESULT hr = E_FAIL;
	vector<POINT>vecContainer;

	hr = GetRectIntersectContainers(&vecContainer,A,C,nLevel);
	KGLOG_COM_PROCESS_ERROR(hr);
	for (size_t i=0;i<vecContainer.size();i++)
	{
		POINT p = vecContainer[i];

		map<POINT,KG3DSceneBlockData*>::iterator s = m_mapSceneBlockData[nLevel].find(p);
		if(s==m_mapSceneBlockData[nLevel].end())
		{
			return E_FAIL;
		}
		else
		{
			KG3DSceneBlockData* pContainer = s->second;
			return pContainer->InputEntity(pEntity);
		}
	}

Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneDataInterfaceCore::InputEntity(KG3DEntityDataEx* pEntity)
{
	HRESULT hr = E_FAIL;
	vector<POINT>vecContainer;
	D3DXVECTOR2 A,C;
	KGLOG_PROCESS_ERROR(pEntity);
	A.x = pEntity->m_Transformation.m_vBox_A.x;
	A.y = pEntity->m_Transformation.m_vBox_A.z;
	C.x = pEntity->m_Transformation.m_vBox_B.x;
	C.y = pEntity->m_Transformation.m_vBox_B.z;

	float W = abs(C.x - A.x);
	float H = abs(C.y - A.y);

	int nLevel = BLOCK_TYPE_REGION;

	if ((W > SCENEREGIONWIDTH) || (H > SCENEREGIONWIDTH))
		nLevel = BLOCK_TYPE_SECTION;
	if ((W > SCENESECTIONWIDTH ) || (H > SCENESECTIONWIDTH ))
		nLevel = BLOCK_TYPE_ZONE;

	hr = TryInputEntityToLevel(pEntity,A,C,nLevel);
	if(FAILED(hr))
	{
		while (nLevel<BLOCK_TYPE_AREA)
		{
			nLevel++;
			hr = TryInputEntityToLevel(pEntity,A,C,nLevel);
			KG_COM_PROCESS_SUCCESS(hr);
		}

		if(FAILED(hr))
		{
			hr = m_ErrorSceneBlock.InputEntity(pEntity);
		}
		KGLOG_COM_PROCESS_ERROR(hr);
	}

Exit1:
	hr = S_OK;
Exit0:
	return hr;
}

DWORD KG3DSceneBlockData::GetMemoryUse()
{
	DWORD dwUse = 0;
	map<DWORD,KG3DSceneDataClip*>::iterator i = m_mapData.begin();
	map<DWORD,KG3DSceneDataClip*>::iterator iend = m_mapData.end();
	while (i!=iend)
	{
		KG3DSceneDataClip* pData = i->second;
		
		dwUse += pData->m_dwLength;

		++i;
	}

	return dwUse;
}

HRESULT KG3DSceneBlockData::InputEntity(KG3DEntityDataEx* pEntity)
{
	m_setEntity.insert(pEntity);
	pEntity->OnInputToContainer(this);
	return S_OK;
}

HRESULT KG3DSceneBlockData::RemoveEntity(KG3DEntityDataEx* pEntity)
{
	m_setEntity.erase(pEntity);
	return S_OK;
}

HRESULT KG3DEntityDataEx::OnInputToContainer(KG3DSceneBlockData* pContainer)
{
	m_setContainer.insert(pContainer);
	return S_OK;
}

HRESULT KG3DEntityDataEx::RemoveFromContainers()
{
	set<KG3DSceneBlockData*>::iterator i = m_setContainer.begin();
	set<KG3DSceneBlockData*>::iterator iend = m_setContainer.end();
	while (i!=iend)
	{
		KG3DSceneBlockData* pContainer = *i;
		if(pContainer)
		{
			pContainer->RemoveEntity(this);
		}
		++i;
	}
	m_setContainer.clear();
	return S_OK;
}

HRESULT KG3DSceneDataInterfaceCore::FindEntity(KG3DEntityDataEx** ppEntity,UINT uHandle)
{
	map<UINT,KG3DEntityDataEx*>::iterator iFind = m_mapEntity.find(uHandle);
	if(iFind == m_mapEntity.end())
		return E_FAIL;
	(*ppEntity) = iFind->second;
	return S_OK;
}

HRESULT KG3DSceneDataInterfaceCore::SetEntityParent(UINT uHandle, UINT uParent)
{
    map<UINT ,KG3DEntityDataEx*>::iterator iFind = m_mapEntity.find(uHandle);
    if (iFind == m_mapEntity.end())
        return E_FAIL;

    m_mapEntity[uHandle]->m_uParent = uParent;

    return S_OK;

}

UINT KG3DSceneDataInterfaceCore::GetEntityParent(UINT uHandle)
{
    map<UINT ,KG3DEntityDataEx*>::iterator iFind = m_mapEntity.find(uHandle);
    if (iFind == m_mapEntity.end())
        return INVALID_OBJSHELL_HANDLE;

    return m_mapEntity[uHandle]->m_uParent;
}

KG3DSceneDataClip* KG3DSceneDataInterfaceCore::FindEntityInforamtion(UINT uHandle)
{
	map<UINT,KG3DSceneDataClip*>::iterator iFind = m_mapEntityInformation.find(uHandle);
	if(iFind == m_mapEntityInformation.end())
		return NULL;
	else
		return iFind->second;
}

KG3DSceneDataClip* KG3DSceneDataInterfaceCore::FindTerrainInforamtion(UINT uHandle)
{
	map<UINT,KG3DSceneDataClip*>::iterator iFind = m_mapTerrainInformation.find(uHandle);
	if(iFind == m_mapTerrainInformation.end())
		return NULL;
	else
		return iFind->second;
}




HRESULT KG3DSceneDataInterfaceCore::AddEntityInformation(KG3DSceneDataClip** ppInfo,UINT& uHandle,DWORD dwDataType,DWORD dwLength,PVOID pData)
{
	BOOL bFind = FALSE;
	map<UINT,KG3DSceneDataClip*>::iterator i = m_mapEntityInformation.begin();
	map<UINT,KG3DSceneDataClip*>::iterator iend = m_mapEntityInformation.end();
	while (i!=iend)
	{
		KG3DSceneDataClip* pInfo = i->second;

		if(pInfo->IsEqual(dwDataType,dwLength,pData))
		{
			uHandle = i->first;
			(*ppInfo) = pInfo;
			bFind = TRUE;
			break;
		}
		++i;
	}

	if(!bFind)
	{
		UINT uNewHandle = GetCurrentEntityInforamtionID();
		KG3DSceneDataClip* pNewInfo = m_cDataClipPool.RequestResource();
		KGLOG_PROCESS_ERROR(pNewInfo);

		pNewInfo->SetData(dwDataType,dwLength,pData);

		m_mapEntityInformation[uNewHandle] = pNewInfo;

		uHandle = uNewHandle;
		(*ppInfo) = pNewInfo;
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneDataInterfaceCore::AddTerrainInformation(KG3DSceneDataClip** ppInfo,UINT& uHandle,DWORD dwDataType,DWORD dwLength,PVOID pData)
{
	BOOL bFind = FALSE;
	map<UINT,KG3DSceneDataClip*>::iterator i = m_mapTerrainInformation.begin();
	map<UINT,KG3DSceneDataClip*>::iterator iend = m_mapTerrainInformation.end();
	while (i!=iend)
	{
		KG3DSceneDataClip* pInfo = i->second;

		if(pInfo->IsEqual(dwDataType,dwLength,pData))
		{
			uHandle = i->first;
			(*ppInfo) = pInfo;
			bFind = TRUE;
			break;
		}
		++i;
	}

	if(!bFind)
	{
		UINT uNewHandle = GetCurrentTerrainInforamtionID();
		KG3DSceneDataClip* pNewInfo = m_cDataClipPool.RequestResource();
		KGLOG_PROCESS_ERROR(pNewInfo);

		pNewInfo->SetData(dwDataType,dwLength,pData);

		m_mapTerrainInformation[uNewHandle] = pNewInfo;

		uHandle = uNewHandle;
		(*ppInfo) = pNewInfo;
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DEntityDataEx::RecordHistoryNode()
{
	if (0)
	{
		KG3DEntityDataEx::HistroyNode Node;
		Node.vTrans = m_Transformation;
		Node.uInfo = m_uEntityInformationHandle;
		m_listHistory.push_back(Node);
	}
	return S_OK;
}

HRESULT KG3DSceneDataInterfaceCore::Request_UndoModifyEntity(UINT uHandle)
{
	HRESULT hr = E_FAIL;
	KG3DEntityDataEx* pEntity = NULL;
	KG3DEntityDataEx::HistroyNode vNode;
	hr = FindEntity(&pEntity,uHandle);
	KGLOG_COM_PROCESS_ERROR(hr);
	KGLOG_PROCESS_ERROR(pEntity);

	KGLOG_PROCESS_ERROR(pEntity->m_listHistory.size());

	vNode = *pEntity->m_listHistory.rbegin();
	pEntity->m_listHistory.pop_back();
		
	//////////////////////////////////////////////////////////////////////////
	pEntity->RemoveFromContainers();

	pEntity->m_Transformation = vNode.vTrans;

	InputEntity(pEntity);

	if (m_lpEventConnection)
	{
		m_lpEventConnection->OnEntityTransformModified(uHandle, TRUE);
	}

	//////////////////////////////////////////////////////////////////////////
	if(vNode.uInfo != pEntity->m_uEntityInformationHandle)
	{
		/*KG3DSceneDataClip* pOldInfo = FindEntityInforamtion(pEntity->m_uEntityInformationHandle);
		KG3DSceneDataClip* pNewInfo = FindEntityInforamtion( vNode.uInfo );*/

		pEntity->m_uEntityInformationHandle = vNode.uInfo;

		/*if(pOldInfo)
			pOldInfo->RemoveRefrenceEntity(uHandle);
		
		if(pNewInfo)
			pNewInfo->AddRefrenceEntity(uHandle);*/

		if (m_lpEventConnection)
		{
			m_lpEventConnection->OnEntityInformationModified(uHandle);
		}
	}
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DSceneDataInterfaceCore::Request_UnDeleteEntity()
{
	HRESULT hr = E_FAIL;
	list<KG3DEntityDataEx*>::reverse_iterator j;
	KGLOG_PROCESS_ERROR(m_listEntityDeleted.size());

	j = m_listEntityDeleted.rbegin();
	KG3DEntityDataEx* pEntity = *j;
	m_listEntityDeleted.pop_back();
	//////////////////////////////////////////////////////////////////////////
	m_mapEntity[pEntity->m_uHandle ] = pEntity;
	InputEntity(pEntity);

	if(m_lpEventConnection)
	{
		m_lpEventConnection->OnEntityCreate(pEntity->m_uHandle, FALSE);
	}
	
	hr = S_OK;
Exit0:
	return hr;
}

//HRESULT KG3DSceneDataInterfaceCore::Test()
//{
//	for (int i=0;i<1;i++)
//	{
//		UINT uHandle = 0;
//		Request_CreateNewEntity(&uHandle);
//		KG3DEntityTransformation trans;
//		trans.m_Translation.x = rand()%100000 * 10.0F;
//		trans.m_Translation.y = 0;
//		trans.m_Translation.z = rand()%100000 * 10.0F;
//		trans.m_vBox_A = trans.m_Translation - D3DXVECTOR3(100,0,100);
//		trans.m_vBox_B = trans.m_Translation + D3DXVECTOR3(100,0,100);
//
//		Request_ModifyEntityTransform(uHandle,trans);
//	}
//	return S_OK;
//}

KG3DSceneBlockData* KG3DSceneDataInterfaceCore::FindSceneBlock(int nLevel,POINT Index)
{
	map<POINT,KG3DSceneBlockData*>::iterator iFind;

	KGLOG_PROCESS_ERROR( nLevel>=0 );
	KGLOG_PROCESS_ERROR( nLevel<4  );

	iFind = m_mapSceneBlockData[nLevel].find(Index);//地形REGION数据
	if(iFind == m_mapSceneBlockData[nLevel].end())
	{
		return NULL;
	}
	else
	{
		return iFind->second;
	}
Exit0:
	return NULL;
}

HRESULT KG3DSceneDataInterfaceCore::Request_CreateTerrainBlock(int nLevel,POINT Index)
{
	KGLOG_PROCESS_ERROR( nLevel>=0 );
	KGLOG_PROCESS_ERROR( nLevel<4  );

	KG3DSceneBlockData* pSection = FindSceneBlock(nLevel,Index);
	KGLOG_PROCESS_ERROR(pSection);

	pSection->m_bTerrainBlock = TRUE;

	if (m_lpEventConnection)
	{
		m_lpEventConnection->OnTerrainBlockCreate(nLevel,Index);
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneDataInterfaceCore::Request_DeleteTerrainBlock(int nLevel,POINT Index)
{
	KGLOG_PROCESS_ERROR( nLevel>=0 );
	KGLOG_PROCESS_ERROR( nLevel<4  );

	KG3DSceneBlockData* pSection = FindSceneBlock(nLevel,Index);
	KGLOG_PROCESS_ERROR(pSection);

	pSection->m_bTerrainBlock = FALSE;
	pSection->m_bModified = TRUE;
	if (m_lpEventConnection)
	{
		m_lpEventConnection->OnTerrainBlockDelete(nLevel,Index);
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneDataInterfaceCore::UnPrepareSceneBlock(KG3DSceneBlockData* pBlock,int nLevel,POINT Index)
{
	//HRESULT hr = E_FAIL;
	KGLOG_PROCESS_ERROR(pBlock);

	if(pBlock->m_bDataInMemory)
	{
		//如何在这里就把地形数据写到文件里去，关闭地图时，会导致不想保存所作的修改被保存了，而且没法恢复原来的数据
		//把数据写到文件时应该确保在，点选保存的时候进行。
		/*if(pBlock->m_bModified)  
		{                          
			TCHAR szName[MAX_PATH];
			GetSceneBlockFileName(szName,pBlock,FALSE);

			hr = pBlock->SaveDataToFile(szName);
			if(FAILED(hr))
			{
				KGLogPrintf(KGLOG_ERR,"SaveSceneBlockToFile %d %d %d 失败",nLevel,Index.x,Index.y);
			}

			//pBlock->m_bModified = FALSE;
		}*/
		if (!pBlock->m_bModified && !pBlock->m_bDisableDynamicUnload) //修改过的数据在没保存之前，不删掉。
		{
			//m_dwMemoryUse -= pBlock->GetMemoryUse();
			pBlock->FreeDataClips();
			pBlock->m_bDataInMemory = FALSE;
		}
	}

	return S_OK;
Exit0:
	return E_FAIL;
}
HRESULT KG3DSceneDataInterfaceCore::LoadRegionBlock(KG3DSceneBlockDataChildData* pChildData,LPCTSTR pFileName,DWORD& dwMemoryUse)
{
	HRESULT hr = E_FAIL;
	IFile* pFile = NULL;
	
	KG_PROCESS_ERROR(pChildData);
			
	pFile = g_OpenFile(pFileName);
	KGLOG_PROCESS_ERROR(pFile);

	DWORD version = 0;
	unsigned long readcount = 0;

	readcount = pFile->Read(&version,sizeof(DWORD));
	KGLOG_PROCESS_ERROR(readcount == sizeof(DWORD));

	_ASSERTE(version == MAP_FILE_VERSION_PACKREGION);

	
	readcount = pFile->Read(pChildData->m_zFilePosition,sizeof(DWORD)*64);
	KGLOG_PROCESS_ERROR(readcount == sizeof(DWORD)*64);

	for (int i=0;i<64;i++)
	{
		KG3DSceneBlockData* pRegion = pChildData->m_zChildBlock[i];
		if(pRegion && !pRegion->m_bDataInMemory)
		{
			pFile->Seek(pChildData->m_zFilePosition[i],SEEK_SET);
			pRegion->FreeDataClips();
			pRegion->FreeDataHistroyClips();
			pRegion->Init(&m_cDataClipPool);
			hr = pRegion->LoadDataFromFile(pFile);
			KGLOG_COM_CHECK_ERROR(hr);

			m_mapSceneBlockDataLock.Lock();
			
			pRegion->m_bDataInMemory = TRUE;
			pRegion->m_nLastUseTime = timeGetTime();
			dwMemoryUse += pRegion->GetMemoryUse();
			
			m_mapSceneBlockDataLock.Unlock();
		}
	}
	hr = S_OK;
Exit0:
	KG_COM_RELEASE(pFile);

	if (FAILED(hr) && pFileName)
	{
		KGLogPrintf(KGLOG_ERR, "Load file %s failed.", pFileName);
	}
	return hr;
}

HRESULT KG3DSceneDataInterfaceCore::PrepareSceneBlock(KG3DSceneBlockData* pBlock,int nLevel,POINT Index)
{
	//KG3PROFILE_DURATION("PrepareSceneBlock");
	HRESULT hr = E_FAIL;
	KGLOG_PROCESS_ERROR(pBlock);

	if(!pBlock->m_bDataInMemory)
	{
		TCHAR szName[MAX_PATH];
		GetSceneBlockFileName(szName,pBlock,TRUE);
		if(g_bUseNewLoad && m_bPackData && pBlock->m_nLevel == BLOCK_TYPE_REGION)
		{
			if(pBlock->m_pParentBlockData)
			{
				DWORD dwMemoryUse = 0;
				hr = LoadRegionBlock(pBlock->m_pParentBlockData->m_lpChildBlockData,szName,dwMemoryUse);
				KG_COM_PROCESS_ERROR(hr);
				//m_dwMemoryUse += dwMemoryUse;
			}
		}
		else
		{
			hr = pBlock->LoadDataFromFile(&m_cDataClipPool,szName,m_bPackData);
			KG_COM_PROCESS_ERROR(hr);

			m_mapSceneBlockDataLock.Lock();

			pBlock->m_nLastUseTime = timeGetTime();
			pBlock->m_bDataInMemory = TRUE;
			//m_dwMemoryUse += pBlock->GetMemoryUse();

			m_mapSceneBlockDataLock.Unlock();
		}
	}


	hr = S_OK;
Exit0:
	return hr;
}

DWORD KG3DSceneDataInterfaceCore::CheckMemoryUseMultiThread(DWORD dwTime,BOOL* pbStop)//检查内存占用
{

    int nNow = timeGetTime();

	for (int i=0;i<4;i++)
	{
		map<POINT,KG3DSceneBlockData*>::iterator s = m_mapSceneBlockData[i].begin();//地形REGION数据
		map<POINT,KG3DSceneBlockData*>::iterator send = m_mapSceneBlockData[i].end();
		while (s!=send)
		{
			KG3DSceneBlockData* pBlock = s->second;
			KG_PROCESS_SUCCESS(*pbStop);
			
			m_mapSceneBlockDataLock.Lock();

			if(pBlock &&  pBlock->m_bDataInMemory && (!pBlock->m_bDisableDynamicUnload) && ( nNow - pBlock->m_nLastUseTime >= (int)dwTime ))
			{
				UnPrepareSceneBlock(pBlock,i,s->first);
			}

			m_mapSceneBlockDataLock.Unlock();
			++s;
		}
	}
Exit1:
	return m_dwMemoryUse;
}

HRESULT KG3DSceneDataInterfaceCore::Request_ModifyTerrainBlock(int nLevel,POINT Index,DWORD dwDataType,DWORD dwLength,PVOID pData)//请求修改地形数据
{
	HRESULT hr = E_FAIL;

	KG3DSceneBlockData* pSection = FindSceneBlock(nLevel,Index);
	KGLOG_PROCESS_ERROR(pSection);

	hr = PrepareSceneBlock(pSection,nLevel,Index);
	KGLOG_COM_PROCESS_ERROR(hr);

	KGLOG_PROCESS_ERROR(pSection->m_bTerrainBlock);

	m_mapSceneBlockDataLock.Lock();
	hr = pSection->ModifyData(dwDataType,dwLength,pData,FALSE);
	m_mapSceneBlockDataLock.Unlock();
	KGLOG_COM_PROCESS_ERROR(hr);

	pSection->m_bModified = TRUE;

	if (m_lpEventConnection)
	{
		m_lpEventConnection->OnTerrainBlockModified(nLevel,Index,dwDataType);
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

BOOL KG3DSceneDataInterfaceCore::GetTerrainBlock(int nLevel,POINT Index)//查询地表SECTION是否存在
{
	KG3DSceneBlockData* pSection = FindSceneBlock(nLevel,Index);

	if (!pSection)
		return FALSE;

	PrepareSceneBlock(pSection,nLevel,Index);

	return pSection->m_bTerrainBlock;

}

HRESULT KG3DSceneDataInterfaceCore::GetTerrainBlockData(int nLevel,POINT Index,DWORD dwDataType,PVOID pData)//获取地形数据，调用者自己负责该数据的创建和删除，该函数背部完成数据复制
{
	HRESULT hr = E_FAIL;
	map<POINT,KG3DSceneBlockData*>::iterator iFind ;
	KG3DSceneDataClip* pSectionData = NULL;

	KG3DSceneBlockData* pSection = FindSceneBlock(nLevel,Index);
	if (!pSection)
		return E_FAIL;
	
	PrepareSceneBlock(pSection,nLevel,Index);

	m_mapSceneBlockDataLock.Lock();
	pSectionData = pSection->FindDataByType(dwDataType);

	KG_PROCESS_ERROR(pSectionData);
	hr = pSectionData->GetData(pData);
Exit0:
	m_mapSceneBlockDataLock.Unlock();
	return hr;
}

HRESULT KG3DSceneDataInterfaceCore::GetAllTerrainBlockDataType(int nLevel,POINT Index,vector<DWORD>*vecDataType)//获取地形块上的所有数据
{
	map<POINT,KG3DSceneBlockData*>::iterator iFind ;

	KG3DSceneBlockData* pSection = FindSceneBlock(nLevel,Index);
	if (!pSection)
		return E_FAIL;

	PrepareSceneBlock(pSection,nLevel,Index);
	//////////////////////////////////////////////////////////////////////////
	m_mapSceneBlockDataLock.Lock();
	pSection->m_bDisableDynamicUnload = TRUE;
	map<DWORD,KG3DSceneDataClip*>::iterator i = pSection->m_mapData.begin();
	map<DWORD,KG3DSceneDataClip*>::iterator iend = pSection->m_mapData.end();
	while (i!=iend)
	{
		vecDataType->push_back(i->first);
		++i;
	}
	pSection->m_bDisableDynamicUnload = FALSE;
	m_mapSceneBlockDataLock.Unlock();
	//////////////////////////////////////////////////////////////////////////

	return S_OK;
}
#ifdef KG3D_AUTO_TEST 

HRESULT KG3DSceneDataInterfaceCore::GetEntityInfoByHandle(UINT uEntityInfoHandle,DWORD* pdwDataType,DWORD* pdwLength,PVOID pData,DWORD dwMaxLen)
{
	HRESULT hr = E_FAIL;

	KG3DSceneDataClip* pEntityInfo = FindEntityInforamtion(uEntityInfoHandle);
	KGLOG_PROCESS_ERROR(pEntityInfo);

	(*pdwDataType) = pEntityInfo->m_dwType;
	(*pdwLength) = pEntityInfo->m_dwLength;
	return pEntityInfo->GetData(pData);
	
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DSceneDataInterfaceCore::GetAllEntityInformationHandle(vector<UINT>*vecHandle)//获取场景里所有EntityInformation
{
	map<UINT,KG3DSceneDataClip*>::iterator i = m_mapEntityInformation.begin();
	map<UINT,KG3DSceneDataClip*>::iterator iend = m_mapEntityInformation.end();

	while (i!=iend)
	{
		vecHandle->push_back(i->first);
		++i;
	}
	return S_OK;
}

HRESULT KG3DSceneDataInterfaceCore::GetAllTerrianInformationHandle(vector<UINT>*vecHandle)//获取场景里所有TerrainInformation
{
	map<UINT,KG3DSceneDataClip*>::iterator i = m_mapTerrainInformation.begin();
	map<UINT,KG3DSceneDataClip*>::iterator iend = m_mapTerrainInformation.end();

	while (i!=iend)
	{
		vecHandle->push_back(i->first);
		++i;
	}
	return S_OK;
}

HRESULT KG3DSceneDataInterfaceCore::GetEntityData(map<UINT,KGEntityData>* mapEntityData)
{
	map<UINT,KG3DEntityDataEx*>::iterator i   = m_mapEntity.begin();
	map<UINT,KG3DEntityDataEx*>::iterator iend = m_mapEntity.end();
	UINT uHandle =0;
	while (i!=iend)
	{
		uHandle = i->first;
		KG3DEntityDataEx* pData = i->second;
		KGEntityData EntityData;
		EntityData.uEntityInfoHandle = pData->m_uEntityInformationHandle;
		EntityData.m_Rotation        = pData->m_Transformation.m_Rotation;
		EntityData.m_Scaling         = pData->m_Transformation.m_Scaling;
		EntityData.m_Translation     = pData->m_Transformation.m_Translation;
		EntityData.m_vWorldPos       = pData->m_Transformation.m_vWorldPos;
		EntityData.m_vBox_A          = pData->m_Transformation.m_vBox_A;
		EntityData.m_vBox_B          = pData->m_Transformation.m_vBox_B;

		std::map<UINT,KGEntityData>::_Pairib Pib = mapEntityData->insert(std::pair<UINT,KGEntityData>(uHandle,EntityData));
		_ASSERTE(Pib.second);//已经存在了不能添加
		++i;
	}
	return S_OK;
}
HRESULT KG3DSceneDataInterfaceCore::GetAllSceneBlock(vector<SceneBlockIndex>* vecSceneBlockIndex)
{
	for (UINT s = 0 ; s< 4 ; s++)
	{
		map<POINT,KG3DSceneBlockData*>::iterator i = m_mapSceneBlockData[s].begin();
		map<POINT,KG3DSceneBlockData*>::iterator iend = m_mapSceneBlockData[s].end();
		while (i!=iend)
		{
			SceneBlockIndex sbIndex;
			sbIndex.nLevel = s;
			sbIndex.Index = i->first;
			vecSceneBlockIndex->push_back(sbIndex);
			++i;
		}
	}
	return S_OK;
}

HRESULT KG3DSceneDataInterfaceCore::GetSceneBlockData(int nLevel,POINT Index,DWORD& dwLenght,PVOID pData)//获取地形数据，调用者自己负责该数据的创建和删除，该函数背部完成数据复制
{
	HRESULT hr = E_FAIL;
	map<POINT,KG3DSceneBlockData*>::iterator iFind;

	KG3DSceneBlockData* pSection = FindSceneBlock(nLevel,Index);
	if (!pSection)
		return E_FAIL;

	PrepareSceneBlock(pSection,nLevel,Index);

	m_mapSceneBlockDataLock.Lock();
	byte* pBegin = (byte*)pData;
	dwLenght = 0;
	map<DWORD,KG3DSceneDataClip*>::iterator i = pSection->m_mapData.begin();
	map<DWORD,KG3DSceneDataClip*>::iterator iend = pSection->m_mapData.end();
	while (i!=iend)
	{
		//DWORD dwType = i->first;
		KG3DSceneDataClip* pSceneData = i->second;
		KG_PROCESS_ERROR(pSceneData);
		hr = pSceneData->GetData((PVOID)pBegin);
		dwLenght += pSceneData->m_dwLength;
		pBegin += dwLenght;
		++i;
	}
Exit0:
	m_mapSceneBlockDataLock.Unlock();
	return hr;
}

#endif
HRESULT KG3DSceneBlockData::Init(TKG3DResourcePool<KG3DSceneDataClip>* pPool)
{
	m_lpSceneDataClipPool = pPool;
	return S_OK;
}

HRESULT KG3DSceneBlockData::FreeDataClips()
{
	map<DWORD,KG3DSceneDataClip*>::iterator i = m_mapData.begin();
	map<DWORD,KG3DSceneDataClip*>::iterator iend = m_mapData.end();
	while (i!=iend)
	{
		KG3DSceneDataClip* pData = i->second;
		if (m_lpSceneDataClipPool)
		{
			m_lpSceneDataClipPool->FreeResource(pData);
		}
		++i;
	}
	m_mapData.clear();
	return S_OK;
}
HRESULT KG3DSceneBlockData::FreeDataHistroyClips()
{
	list<KG3DSceneDataClip*>::iterator i = m_listDataHistroy.begin();
	list<KG3DSceneDataClip*>::iterator iend = m_listDataHistroy.end();
	while (i!=iend)
	{
		KG3DSceneDataClip* pData = *i;
		if (m_lpSceneDataClipPool)
		{
			m_lpSceneDataClipPool->FreeResource(pData);
		}
		++i;
	}
	m_listDataHistroy.clear();
	return S_OK;
}

HRESULT KG3DSceneBlockData::UnInit()
{
	m_bTerrainBlock = FALSE;
	m_pParentBlockData = NULL;
	SAFE_DELETE(m_lpChildBlockData);

	{
		FreeDataClips();
	}
	//////////////////////////////////////////////////////////////////////////
	{
		list<KG3DSceneDataClip*>::iterator i = m_listDataHistroy.begin();
		list<KG3DSceneDataClip*>::iterator iend = m_listDataHistroy.end();
		while (i!=iend)
		{
			KG3DSceneDataClip* pData = *i;
			if (m_lpSceneDataClipPool)
			{
				m_lpSceneDataClipPool->FreeResource(pData);
			}
			++i;
		}
		m_listDataHistroy.clear();
	}

	m_lpSceneDataClipPool = NULL;

	m_nIndexX = 0;
	m_nIndexZ = 0;

	m_bDataInMemory = FALSE;

	m_nLevel = 0;
	m_nLastUseTime = 0;
	m_dwDataSize = 0;
	m_setEntity.clear();

	m_bModified = FALSE;
	m_bSaveChildData = FALSE;

	return S_OK;
}

KG3DSceneBlockData::KG3DSceneBlockData()
{
	m_nLevel  = 0;
	m_nIndexX = 0;
	m_nIndexZ = 0;

	m_bTerrainBlock = FALSE;
	m_lpSceneDataClipPool = NULL;

	m_bDataInMemory = FALSE;
	m_nLastUseTime = 0;

	m_dwDataSize = 0;
	m_bModified = FALSE;
	m_pParentBlockData = NULL;
	m_lpChildBlockData = NULL;
	m_bDisableDynamicUnload = FALSE;

	m_bSaveChildData = FALSE;
}

KG3DSceneBlockData::~KG3DSceneBlockData()
{
	UnInit();
}

KG3DSceneDataClip* KG3DSceneBlockData::FindDataByType(DWORD dwType)
{
	KG3DSceneDataClip* pClip = NULL;

	map<DWORD,KG3DSceneDataClip*>::iterator i = m_mapData.find(dwType);
	if(i!=m_mapData.end())
		pClip = i->second;

	return pClip;
}

HRESULT KG3DSceneBlockData::ModifyData(DWORD dwType,DWORD dwLength,PVOID lpBuffer,BOOL bRecord)
{
	HRESULT hr = E_FAIL;
	KG3DSceneDataClip* pNewData = NULL;
	KG3DSceneDataClip* pOldData = FindDataByType(dwType);

	if (pOldData)
	{
		m_mapData.erase(dwType);
	}

	if(pOldData && bRecord)
	{
		m_listDataHistroy.push_back(pOldData);
	}
	else if(pOldData)
	{
		m_lpSceneDataClipPool->FreeResource(pOldData);
	}
	
	pNewData = m_lpSceneDataClipPool->RequestResource();
	KGLOG_PROCESS_ERROR(pNewData);

	hr = pNewData->SetData(dwType,dwLength,lpBuffer);
	KGLOG_COM_PROCESS_ERROR(hr);

	m_mapData[dwType] = pNewData;

	return S_OK;
Exit0:

	if(pNewData)
	{
		m_lpSceneDataClipPool->FreeResource(pNewData);
		pNewData = NULL;
	}
	return E_FAIL;
}

HRESULT KG3DSceneBlockData::SaveChildDataToFile(FILE* pFile)
{
	HRESULT hr = E_FAIL;
	
	DWORD dwPosSave = 0;
	DWORD version = MAP_FILE_VERSION_PACKREGION;
	
	if(m_nLevel != BLOCK_TYPE_SECTION)
		return S_OK;

	
	KGLOG_PROCESS_ERROR(m_lpChildBlockData);
	fwrite(&version,sizeof(DWORD),1,pFile);

	dwPosSave = ftell(pFile);
	fwrite(m_lpChildBlockData->m_zFilePosition,sizeof(DWORD)*64,1,pFile);


	for (int i=0;i<64;i++)
	{
		KG3DSceneBlockData* pRegion = m_lpChildBlockData->m_zChildBlock[i];
		if(pRegion && pRegion->m_bDataInMemory && pRegion->m_bModified)
		{
			m_lpChildBlockData->m_zFilePosition[i] = ftell(pFile);
			hr = pRegion->SaveDataToFile(pFile);
			KGLOG_COM_PROCESS_ERROR(hr);
			pRegion->m_bModified = FALSE;
		}
		else
		{
			_ASSERTE(0);
		}
	}

	fseek(pFile,dwPosSave,SEEK_SET);
	fwrite(m_lpChildBlockData->m_zFilePosition,sizeof(DWORD)*64,1,pFile);
	

	return S_OK;
Exit0:
	return E_FAIL;
}
HRESULT KG3DSceneBlockData::SaveDataToFile(FILE* pFile)
{
	DWORD version = 0;
	DWORD dwDataCount = (DWORD)m_mapData.size();
	if(!m_bTerrainBlock)
		dwDataCount = 0;
	fwrite(&version,sizeof(DWORD),1,pFile);
	fwrite(&m_bTerrainBlock,sizeof(BOOL),1,pFile);
	fwrite(&dwDataCount,sizeof(DWORD),1,pFile);
	////////////////add by huangjinshou 2009-11-25////////////////////////////////////////
	/////如果没有m_bTerrainBlock==0代表没有地表。这时候mapData的数据可以不用存。 
	if(m_bTerrainBlock)
	{
		map<DWORD,KG3DSceneDataClip*>::iterator i = m_mapData.begin();
		map<DWORD,KG3DSceneDataClip*>::iterator iend = m_mapData.end();
		while (i!=iend)
		{
			//DWORD dwType = i->first;
			KG3DSceneDataClip* pData = i->second;
			pData->SaveDataToFile(pFile);
			++i;
		}
	}
	////////////////////////////////////////////////////////////////////////////////////
	return S_OK;
//Exit0:
//	return E_FAIL;
}
HRESULT KG3DSceneBlockData::SaveChildDataToFile(LPCTSTR pFileName)
{
	HRESULT hr = E_FAIL;
	FILE* pChildFile = NULL;
	KGLOG_PROCESS_ERROR(m_nLevel == BLOCK_TYPE_SECTION);
	if(m_lpChildBlockData)
	{
		pChildFile = fopen(pFileName,"wb");
		KGLOG_PROCESS_ERROR(pChildFile);
		hr = SaveChildDataToFile(pChildFile);
		KGLOG_COM_PROCESS_ERROR(hr);
	}
Exit0:
	if(pChildFile)
	{
		fclose(pChildFile);
		pChildFile = NULL;
	}
	return hr;
}

HRESULT KG3DSceneBlockData::SaveDataToFile(LPCTSTR pFileName)
{
	HRESULT hr = E_FAIL;
	FILE* pFile = NULL;
	
	KGLOG_PROCESS_ERROR(pFileName);
	pFile = fopen(pFileName,"wb");
	KGLOG_PROCESS_ERROR(pFile);
	hr = SaveDataToFile(pFile);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = S_OK;
Exit0:
	if(pFile)
	{
		fclose(pFile);
		pFile = NULL;
	}
	return hr;
}

HRESULT KG3DSceneBlockData::LoadDataFromPackFile(IFile* pFile)
{
	_ASSERTE(m_nLevel == BLOCK_TYPE_REGION);
	
	HRESULT hr = E_FAIL;
	DWORD version = 0;
	unsigned long readcount = 0;
	KG3DSceneBlockDataChildData ChildBlockData;
	KG_PROCESS_ERROR(pFile);

	readcount = pFile->Read(&version,sizeof(DWORD));
	KGLOG_PROCESS_ERROR(readcount == sizeof(DWORD));
	
	_ASSERTE(version == MAP_FILE_VERSION_PACKREGION);

	//KGLOG_PROCESS_ERROR(m_lpChildBlockData);
	readcount = pFile->Read(ChildBlockData.m_zFilePosition,sizeof(DWORD)*64);
	KGLOG_PROCESS_ERROR(readcount == sizeof(DWORD)*64);
	{
		POINT parentSectionIndex;
		parentSectionIndex.x = m_nIndexX / 8;
		parentSectionIndex.y = m_nIndexZ / 8;
		int x_Local = m_nIndexX - parentSectionIndex.x * 8;
		int y_Local = m_nIndexZ - parentSectionIndex.y * 8;
		int index_local = y_Local* 8 + x_Local; 
		_ASSERTE(index_local < 64 );
		//for (int i=0;i<64;i++)
		{
			//KG3DSceneBlockData* pRegion = m_lpChildBlockData->m_zChildBlock[index_local];
			if(!m_bDataInMemory)
			{
				pFile->Seek(ChildBlockData.m_zFilePosition[index_local],SEEK_SET);
				hr = LoadDataFromFile(pFile);
				KGLOG_COM_PROCESS_ERROR(hr);
			}
		}
	}
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DSceneBlockData::LoadDataFromFile(IFile* pFile)
{
//	HRESULT hr = E_FAIL;

	DWORD version = 0;
	DWORD dwDataCount = 0;
	unsigned long readcount = 0;

	readcount = pFile->Read(&version,sizeof(DWORD));
	KGLOG_PROCESS_ERROR(readcount == sizeof(DWORD));

	readcount = pFile->Read(&m_bTerrainBlock,sizeof(BOOL));
	KGLOG_PROCESS_ERROR(readcount == sizeof(BOOL));
	readcount = pFile->Read(&dwDataCount,sizeof(DWORD));
	KGLOG_PROCESS_ERROR(readcount == sizeof(DWORD));

	KGLOG_PROCESS_ERROR(dwDataCount<=100);
	////////////////add by huangjinshou 2009-11-25////////////////////////////////////////
	/////如果没有m_bTerrainBlock==0代表没有地表。这时候mapData的数据可以不用读。 
	if(m_bTerrainBlock)
	{
	for (DWORD i=0;i<dwDataCount;i++)
	{
		KG3DSceneDataClip* pData = m_lpSceneDataClipPool->RequestResource();
		KGLOG_PROCESS_ERROR(pData);

		pData->LoadDataFromFile(pFile);

		m_mapData[pData->m_dwType] = pData;
		}
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneBlockData::LoadDataFromFile(TKG3DResourcePool<KG3DSceneDataClip>* pPool,LPCTSTR pFileName,BOOL bPackRegion)
{
	HRESULT hr = E_FAIL;

	IFile* pFile = NULL;
	
	Init(pPool);

	{
		{
			map<DWORD,KG3DSceneDataClip*>::iterator i = m_mapData.begin();
			map<DWORD,KG3DSceneDataClip*>::iterator iend = m_mapData.end();
			while (i!=iend)
			{
				KG3DSceneDataClip* pData = i->second;
				if (m_lpSceneDataClipPool)
				{
					m_lpSceneDataClipPool->FreeResource(pData);
				}
				++i;
			}
			m_mapData.clear();
		}
		//////////////////////////////////////////////////////////////////////////
		{
			list<KG3DSceneDataClip*>::iterator i = m_listDataHistroy.begin();
			list<KG3DSceneDataClip*>::iterator iend = m_listDataHistroy.end();
			while (i!=iend)
			{
				KG3DSceneDataClip* pData = *i;
				if (m_lpSceneDataClipPool)
				{
					m_lpSceneDataClipPool->FreeResource(pData);
				}
				++i;
			}
			m_listDataHistroy.clear();
		}
	}

	pFile = g_OpenFile(pFileName);
	KGLOG_PROCESS_ERROR(pFile);
	if(bPackRegion && m_nLevel == BLOCK_TYPE_REGION)
	{
		//ASSERT(m_pParentBlockData);
		hr = LoadDataFromPackFile(pFile);
		KGLOG_COM_PROCESS_ERROR(hr);
	}
	else
	{
		hr = LoadDataFromFile(pFile);
		KGLOG_COM_PROCESS_ERROR(hr);
	}

	hr = S_OK;

Exit0:
    KG_COM_RELEASE(pFile);

	if (FAILED(hr) && pFileName)
	{
		KGLogPrintf(KGLOG_ERR, "Load file %s failed.", pFileName);
	}
	return hr;
}

HRESULT KG3DSceneDataClip::SetData(DWORD dwType,DWORD dwLength,PVOID lpBuffer)
{
	m_dwType = dwType;
	m_dwLength = dwLength;
	SAFE_DELETE_ARRAY(m_lpBuffer);
	m_lpBuffer = new BYTE[m_dwLength];
	KGLOG_PROCESS_ERROR(m_lpBuffer);
	memcpy(m_lpBuffer,lpBuffer,dwLength);
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneDataClip::GetData(PVOID lpBuffer)
{
	KGLOG_PROCESS_ERROR(lpBuffer);
	KGLOG_PROCESS_ERROR(m_lpBuffer);

	memcpy(lpBuffer,m_lpBuffer,m_dwLength);

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneDataClip::SaveDataToFile(FILE* pFile)
{
	fwrite(&m_dwType,sizeof(DWORD),1,pFile);
	fwrite(&m_dwLength,sizeof(DWORD),1,pFile);
	fwrite(m_lpBuffer,m_dwLength,1,pFile);

	return S_OK;
}
 
HRESULT KG3DSceneDataClip::SaveDataToFile(LPCTSTR pFileName)
{
	HRESULT hr = E_FAIL;
	FILE* pFile = NULL;
	KGLOG_PROCESS_ERROR(pFileName);
	pFile = fopen(pFileName,"wb");
	KGLOG_PROCESS_ERROR(pFile);

	SaveDataToFile(pFile);

	hr = S_OK;
Exit0:
	if(pFile)
	{
		fclose(pFile);
		pFile = NULL;
	}
	return hr;
}

HRESULT KG3DSceneDataClip::LoadDataFromFile(IFile* pFile)
{
	pFile->Read(&m_dwType,sizeof(DWORD));
	pFile->Read(&m_dwLength,sizeof(DWORD));
	
	SAFE_DELETE_ARRAY(m_lpBuffer);


	m_lpBuffer = new BYTE[m_dwLength];
	KGLOG_PROCESS_ERROR(m_lpBuffer);

	pFile->Read(m_lpBuffer,m_dwLength);

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneDataClip::LoadDataFromFile(LPCTSTR pFileName)
{
	HRESULT hr = E_FAIL;

	IFile* pFile = NULL;
	KGLOG_PROCESS_ERROR(pFileName);
	pFile = g_OpenFile(pFileName);
	KGLOG_PROCESS_ERROR(pFile);

	LoadDataFromFile(pFile);

	hr = S_OK;

Exit0:
    KG_COM_RELEASE(pFile);
	return hr;
}
//////////////////////////////////////////////////////////////////////////

HRESULT KG3DSceneDataInterfaceCore::Request_CreateNewEntityInfo(UINT& uHandle,DWORD dwDataType,DWORD dwLength,PVOID pData)
{
	HRESULT hr = S_OK;

	KG3DSceneDataClip* pNewDataClip = NULL;
	hr = AddEntityInformation(&pNewDataClip,uHandle,dwDataType,dwLength,pData);
	KGLOG_COM_PROCESS_ERROR(hr);

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneDataInterfaceCore::Request_CreateNewTerrainInfo(UINT& uHandle,DWORD dwDataType,DWORD dwLength,PVOID pData)
{
	HRESULT hr = S_OK;

	KG3DSceneDataClip* pNewDataClip = NULL;
	hr = AddTerrainInformation(&pNewDataClip,uHandle,dwDataType,dwLength,pData);
	KGLOG_COM_PROCESS_ERROR(hr);

	if(m_lpEventConnection)
	{
		m_lpEventConnection->OnTerrainInformationModified(uHandle,dwDataType);
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneDataInterfaceCore::Request_ReplaceEntity(vector<UINT>& uEntityHandles,DWORD dwDataType,DWORD dwLength,PVOID pData, bool all, OUT vector<UINT>& allNeedReplaceHandles)
{

	UINT entityinfoid = 0;
	KG3DSceneDataClip* entityFormationClip = NULL;
	AddEntityInformation(&entityFormationClip, entityinfoid, dwDataType, dwLength, pData);

	//批量替换，为了提高效率，把需要替换的entityinfoid 添加到allNeedReplaceEntityInfoId
	vector<UINT> allNeedReplaceEntityInfoIds;

	//m_mapEntity 表示所有已经加载的实体的信息
	//m_mapEntityInformation 表示其对应的entityinfo的信息
	for(vector<UINT>::iterator it = uEntityHandles.begin(); it!= uEntityHandles.end(); it ++)
	{
      UINT entityHandle = *it;
	  
	  if(entityHandle == -1)
		  continue; //这里传入的handle,编辑器操作是不会为-1的，但还是加上一个判断比较安全

	  //首先查找这个实体的地层数据
	  KG3DEntityDataEx *selectEntity = m_mapEntity[entityHandle];
      KGLOG_PROCESS_ERROR(selectEntity);
	  if(all)
	  {
		  //如果需要批量替换，把需要替换的id赛选出来，目的是过滤掉重复选中的，这样才能保证效率，减少无谓的全场景数据迭代次数
          if(find(allNeedReplaceEntityInfoIds.begin(), allNeedReplaceEntityInfoIds.end(), selectEntity->m_uEntityInformationHandle) == allNeedReplaceEntityInfoIds.end())
		  {
              allNeedReplaceEntityInfoIds.push_back(selectEntity->m_uEntityInformationHandle);
		  }
	  }
	  else
	  {
		  //并非需要批量替换的,直接更换这个实体的m_uEntityInformationHandle就可以了
          selectEntity->m_uEntityInformationHandle = entityinfoid;
	  }
	}


	if(all)
	{
		//执行遍历替换
		for(vector<UINT>::iterator it = allNeedReplaceEntityInfoIds.begin(); it != allNeedReplaceEntityInfoIds.end(); it ++)
		{
			UINT changid = *it;
			for(map<UINT,KG3DEntityDataEx*>::iterator it2 = m_mapEntity.begin(); it2!= m_mapEntity.end(); it2 ++)
			{
			   UINT entityid = it2->first; //实体本身的id
			   KG3DEntityDataEx * entityData = it2->second;
			   if(entityData->m_uEntityInformationHandle == changid)
			   {
				  entityData->m_uEntityInformationHandle = entityinfoid;
				  //这就是场景中所有需要替换的对象id了,全场景动态加载更换需要这个
				  allNeedReplaceHandles.push_back(entityid);
			   }
			}
		}
	}

	return S_OK;

Exit0:
	return E_FAIL;

}




HRESULT KG3DSceneDataInterfaceCore::Request_CreateSceneBlock(int nLevel,POINT Index)//创建场景区域
{
	KGLOG_PROCESS_ERROR( nLevel>=0 );
	KGLOG_PROCESS_ERROR( nLevel<4  );

	KG3DSceneBlockData* pBlock = FindSceneBlock(nLevel,Index);
	KGLOG_PROCESS_ERROR(!pBlock);

	KG3DSceneBlockData* pNewSceneBlock = m_cSceneBlockDataPool.RequestResource();
	KGLOG_PROCESS_ERROR(pNewSceneBlock);

	pNewSceneBlock->Init(&m_cDataClipPool);

	pNewSceneBlock->m_nIndexX = Index.x;
	pNewSceneBlock->m_nIndexZ = Index.y;
	pNewSceneBlock->m_nLevel  = nLevel;
	pNewSceneBlock->m_bDataInMemory = TRUE;//创建新的区块，这个时候没有对应的磁盘文件和具体数据，所以所有数据都在内存中了
	pNewSceneBlock->m_nLastUseTime = timeGetTime();
	
	//pNewSection->m_bModified = TRUE;

	if(nLevel <= BLOCK_TYPE_SECTION)
	{
		float CellLength  = 0;
		float BlockLength = 0;

		SceneBlockFindLength(nLevel,BlockLength,CellLength);

		D3DXVECTOR3 A(pNewSceneBlock->m_nIndexX * BlockLength,0,pNewSceneBlock->m_nIndexZ * BlockLength);
		D3DXVECTOR3 C = A + D3DXVECTOR3(BlockLength,0,BlockLength);

		m_vBBox.AddPosition(A);
		m_vBBox.AddPosition(C);

	}
    m_mapSceneBlockDataLock.Lock();
	m_mapSceneBlockData[nLevel][Index] = pNewSceneBlock;
    m_mapSceneBlockDataLock.Unlock();

	if(m_lpEventConnection)
	{
		m_lpEventConnection->OnSceneBlockCreate(nLevel,Index);
	}

	if(m_bPackData && nLevel==BLOCK_TYPE_REGION)
	{
		POINT parentSectionIndex;
		parentSectionIndex.x = Index.x / 8;
		parentSectionIndex.y = Index.y / 8;

		KG3DSceneBlockData* pParentSection = FindSceneBlock(BLOCK_TYPE_SECTION,parentSectionIndex);
		KGLOG_PROCESS_ERROR(pParentSection);
		pNewSceneBlock->m_pParentBlockData = pParentSection;

		
		if(g_bUseNewLoad)
		{
			if(!pParentSection->m_lpChildBlockData)
			{
				pParentSection->m_lpChildBlockData = new KG3DSceneBlockDataChildData;
				KGLOG_PROCESS_ERROR(pParentSection->m_lpChildBlockData);
			}
			int x_Local = Index.x - parentSectionIndex.x * 8;
			int y_Local = Index.y - parentSectionIndex.y * 8;
			int index_local = y_Local* 8 + x_Local; 
			pParentSection->m_lpChildBlockData->m_zChildBlock[index_local] = pNewSceneBlock;
		}
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneDataInterfaceCore::Request_DeleteSceneBlock(int nLevel,POINT Index)
{
	map<POINT,KG3DSceneBlockData*>::iterator iFind ;
	KGLOG_PROCESS_ERROR( nLevel>=0 );
	KGLOG_PROCESS_ERROR( nLevel<4  );

	KG3DSceneBlockData* pSection = FindSceneBlock(nLevel,Index);
	KGLOG_PROCESS_ERROR(pSection);

	iFind = m_mapSceneBlockData[nLevel].find(Index);//地形REGION数据
	m_mapSceneBlockData[nLevel].erase(iFind);

	m_listTerrainSectionDeleted.push_back(pSection);
	if(m_lpEventConnection)
	{
		m_lpEventConnection->OnSceneBlockDelete(nLevel,Index);
	}

	/*if(0)
	{
		TCHAR szName[MAX_PATH];
		GetSceneBlockFileName(szName,pSection,TRUE);

		DeleteFile(szName);
	}*/

	return S_OK;
Exit0:
	return E_FAIL;
}

BOOL KG3DSceneDataInterfaceCore::GetSceneBlock(int nLevel,POINT Index)//查询地表SECTION是否存在
{
	KGLOG_PROCESS_ERROR( nLevel>=0 );
	KGLOG_PROCESS_ERROR( nLevel<4  );

	KG3DSceneBlockData* pSection = FindSceneBlock(nLevel,Index);
	KG_PROCESS_ERROR(pSection);

	return TRUE;
Exit0:
	return FALSE;
}

HRESULT KG3DSceneDataInterfaceCore::GetEntityInformationFileName(LPSTR pFileName,UINT uHandle,BOOL bRead)
{
	if(bRead)
		wsprintf(pFileName,"%s\\%d.EntityInfo",m_szDirectoryRead.c_str(),uHandle);
	else
		wsprintf(pFileName,"%s\\%d.EntityInfo",m_szDirectoryWrite.c_str(),uHandle);
	return S_OK;
}

HRESULT KG3DSceneDataInterfaceCore::GetTerrainInformationFileName(LPSTR pFileName,UINT uHandle,BOOL bRead)
{
	if(bRead)
		wsprintf(pFileName,"%s\\%d.TerrainInfo",m_szDirectoryRead.c_str(),uHandle);
	else
		wsprintf(pFileName,"%s\\%d.TerrainInfo",m_szDirectoryWrite.c_str(),uHandle);
	return S_OK;
}

HRESULT KG3DSceneDataInterfaceCore::GetSceneBlockChildFileName(LPSTR pFileName,KG3DSceneBlockData* pBlock,BOOL bRead)
{
	_ASSERTE(m_bPackData);
	if(pBlock)
	{
		_ASSERTE(pBlock->m_nLevel == BLOCK_TYPE_SECTION);
		if(bRead)
			wsprintf(pFileName,"%s\\%d_%d_%d_Child.Block",m_szDirectoryRead.c_str(),pBlock->m_nLevel,pBlock->m_nIndexX,pBlock->m_nIndexZ);
		else
			wsprintf(pFileName,"%s\\%d_%d_%d_Child.Block",m_szDirectoryWrite.c_str(),pBlock->m_nLevel,pBlock->m_nIndexX,pBlock->m_nIndexZ);

	}
	return S_OK;
}

HRESULT KG3DSceneDataInterfaceCore::GetSceneBlockFileName(LPSTR pFileName,KG3DSceneBlockData* pBlock,BOOL bRead)
{
	if(m_bPackData && pBlock->m_nLevel == BLOCK_TYPE_REGION)
	{
		KG3DSceneBlockData* pParentBlock = pBlock->m_pParentBlockData;
		GetSceneBlockChildFileName(pFileName,pParentBlock,bRead);
	}
	else
	{
		if(bRead)
			wsprintf(pFileName,"%s\\%d_%d_%d.Block",m_szDirectoryRead.c_str(),pBlock->m_nLevel,pBlock->m_nIndexX,pBlock->m_nIndexZ);
		else
			wsprintf(pFileName,"%s\\%d_%d_%d.Block",m_szDirectoryWrite.c_str(),pBlock->m_nLevel,pBlock->m_nIndexX,pBlock->m_nIndexZ);
	}

	return S_OK;
}


HRESULT KG3DSceneDataInterfaceCore::LoadFromFile(const char cszFileName[], unsigned uFileNameHash, unsigned uOption)
{
	HRESULT hr = E_FAIL;

	MapFileHeader vHeader;
	IFile* pFile = NULL;
	size_t st;

	hr = InitDirectory(cszFileName,TRUE,TRUE);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = E_FAIL;
	pFile = g_OpenFile(cszFileName);
	KGLOG_PROCESS_ERROR(pFile);

	DWORD dwSize = sizeof(MapFileHeader);
	ZeroMemory(&vHeader,dwSize);

	st = pFile->Read(&vHeader,dwSize);
	KGLOG_PROCESS_ERROR(st == dwSize);

	KGLOG_PROCESS_ERROR(vHeader.dwMask == 0x10203040);//文件类型检查

    m_dwVersion = vHeader.dwVersion;
	m_uCurrentEntityID = vHeader.uCurrentEntityID;
	m_uCurrentEntityInformationID = vHeader.uCurrentEntityInformationID;
	m_uCurrentTerrainInformationID = vHeader.uCurrentTerrainInformationID;

	
	//////////////////////////////////////////////////////////////////////////
	if(m_dwVersion >= MAP_FILE_VERSION_PACKREGION)
	{
		m_bPackData = TRUE;

		TCHAR scEntityInfoName[MAX_PATH];
		wsprintf(scEntityInfoName,"%s\\map.EntityInfo",m_szDirectoryRead.c_str());
		LoadEntityInfoFromFile(scEntityInfoName);

		TCHAR scTerrainInfoName[MAX_PATH];
		wsprintf(scTerrainInfoName,"%s\\map.TerrainInfo",m_szDirectoryRead.c_str());
		LoadTerrainInfoFromFile(scTerrainInfoName);
	}
	else
	{
		LoadEntityInfomationFromFile(pFile);

		LoadTerrainInfomationFromFile(pFile);
	}
	LoadSceneBlockFromFile(pFile);

	LoadEntityFromFile(pFile);
	//////////////////////////////////////////////////////////////////////////

	hr = S_OK;

Exit0:
	if (FAILED(hr))
	{
		KGLogPrintf(KGLOG_ERR, "Load %s failed.\n", cszFileName);
	}

    KG_COM_RELEASE(pFile);
	return hr;
}


//获取文件列表
void GetDirectrotyFileList(string path, vector<string>& files)
{
	//文件句柄  
	intptr_t   hFile   =   0;  
	//文件信息  
	struct _finddata_t fileinfo;  

	string p;

	if((hFile = _findfirst(p.assign(path).append("\\*").c_str(),&fileinfo))   !=   -1)
	{  
		do{  
			//如果是目录,迭代之, 如果不是,加入列表
			if((fileinfo.attrib  & _A_SUBDIR)) 
			{  
				if   (strcmp(fileinfo.name,".")   !=   0   &&   strcmp(fileinfo.name,"..")   !=   0)  
					GetDirectrotyFileList(   p.assign(path).append("\\").append(fileinfo.name), files);  
			}  
			else 
			{  
				files.push_back(   p.assign(path).append("\\").append(fileinfo.name));
			}  
		}   

		while   (_findnext(   hFile,   &fileinfo   )   ==   0);  

		_findclose(hFile);
	}
}

bool isAllDigit(const string& str)
{
	int i ;
	for(i = 0; i != (int)str.length(); i++)
	{
		if(!isdigit(str[i]))
		{
			return false;
		}
	}
	return true;

}

HRESULT KG3DSceneDataInterfaceCore::SaveToFile(const char cszFileName[], unsigned uOption)
{
	HRESULT hrCode = E_FAIL;
	HRESULT hrResult = E_FAIL;

	MapFileHeader vHeader;
	FILE* pFile = NULL;
	size_t st;
	BOOL bConversion = FALSE;
	DWORD dwPos=0;
	hrCode = InitDirectory(cszFileName,FALSE,TRUE);
	KGLOG_COM_PROCESS_ERROR(hrCode);

	if((!m_bPackData) && (g_nPackFile > 1))
	{
		if(MessageBox(NULL,"当前地图格式是旧格式,是否要保存为新格式?","提示",MB_YESNO) == IDYES )
		{
			bConversion = TRUE;
		}
	}
	pFile = fopen(cszFileName,"wb");
	KGLOG_PROCESS_ERROR(pFile);

	hrCode = CreateMapFileHeader(&vHeader);
	KGLOG_COM_PROCESS_ERROR(hrCode);

	if(bConversion)
		vHeader.dwVersion = MAP_FILE_VERSION_PACKREGION;
	dwPos = ftell(pFile);
	st = fwrite(&vHeader,sizeof(MapFileHeader),1,pFile);
	KGLOG_PROCESS_ERROR(st == 1);
	
	//小心这些东西的更改，造成存盘丢失事情就大了
	//////////////////////////////////////////////////////////////////////////
	if(vHeader.dwVersion >= MAP_FILE_VERSION_PACKREGION)
	{
		CHAR scEntityInfoName[MAX_PATH];
		wsprintf(scEntityInfoName,"%s\\map.EntityInfo",m_szDirectoryWrite.c_str());
		SaveEntityInfoToFile(scEntityInfoName);

		TCHAR scTerrainInfoName[MAX_PATH];
		wsprintf(scTerrainInfoName,"%s\\map.TerrainInfo",m_szDirectoryWrite.c_str());
		SaveTerrainInfoToFile(scTerrainInfoName);
	}
	else
	{
		SaveEntityInfomationToFile(pFile);	//EntityInfo
		SaveTerrainInfomationToFile(pFile);	//TerrainInfo
	}

	SaveSceneBlockToFile(pFile,bConversion);		//SceneBlock
	
	SaveEntityToFile(pFile);			//map文件
	
	//////////////////////////////////////////////////////////////////////////
	////这里是为了确保MapFileHeader里的dwNumEntityInformation数目跟实际的数目一样
	vHeader.dwNumEntityInformation = (DWORD)m_mapEntityInformation.size();
	fseek(pFile,dwPos,SEEK_SET);
	st = fwrite(&vHeader,sizeof(MapFileHeader),1,pFile);
	KGLOG_PROCESS_ERROR(st == 1);
	/////////////////////////////////////////////////////////////////////////////
	hrCode = InitDirectory(cszFileName,TRUE,TRUE);
	KGLOG_COM_PROCESS_ERROR(hrCode);
	hrResult = S_OK;

	if(bConversion)
	{
		//需要删除旧的格式
        vector<string> files;
		GetDirectrotyFileList(m_szDirectoryWrite, files);
		int i = 0;
		for(vector<string>::iterator it = files.begin(); it!= files.end(); it ++)
		{
			i ++;
			string &file = *it;
			TCHAR strDrive[MAX_PATH];
			TCHAR strDir[MAX_PATH];
			TCHAR strFileName[MAX_PATH];
			TCHAR strExt[MAX_PATH];
			_splitpath(file.c_str(), strDrive, strDir, strFileName, strExt);
            string _strFileName = strFileName;
			string _strExt = strExt;
			

			int id1 = _strFileName.find("_");
		    if(id1 > 0)
			{
				string _block = _strFileName.substr(0, _strFileName.find("_"));	
				//前缀是否是"数字_"
				if(_block!="" && ! isAllDigit(_block))
					continue;

				int _b = atoi(_block.c_str());
				if(_b == 0 && _strExt == ".Block")
					_unlink(file.c_str());	
				continue;
			}
		
		
			//前缀是否"数字"
			if(!isAllDigit(strFileName))
				continue;
				
			if(_strExt == ".EntityInfo" || _strExt == ".TerrainInfo")
				_unlink(file.c_str());
		}
	}
Exit0:
	if(pFile)
	{
		fclose(pFile);
		pFile = NULL;
	}
	return hrResult;
}

HRESULT KG3DSceneDataInterfaceCore::SaveEntityToFile(FILE* pFile)
{
	DWORD dwNum = (DWORD)m_mapEntity.size();
	fwrite(&dwNum,sizeof(DWORD),1,pFile);
	//////////////////////////////////////////////////////////////////////////

	map<UINT,KG3DEntityDataEx*>::iterator i = m_mapEntity.begin();
	map<UINT,KG3DEntityDataEx*>::iterator iend = m_mapEntity.end();

	while(i!=iend)
	{
		UINT uHandle = i->first;
		KG3DEntityDataEx* pEntity = i->second;

		fwrite(&uHandle,sizeof(UINT),1,pFile);
		fwrite(&pEntity->m_uEntityInformationHandle,sizeof(UINT),1,pFile);
		fwrite(&pEntity->m_Transformation,sizeof(KG3DEntityTransformation),1,pFile);
        
        // version 01

        fwrite(&pEntity->m_uParent, sizeof(pEntity->m_uParent), 1, pFile);
        

		++i;
	}

	return S_OK;
}

HRESULT KG3DSceneDataInterfaceCore::SaveEntityInfomationToFile(FILE* pFile)
{
	HRESULT hr = E_FAIL;

	{
		map<UINT,KG3DSceneDataClip*>::iterator i = m_mapEntityInformation.begin();//场景物件信息，用于创建一个场景物件的数据
		map<UINT,KG3DSceneDataClip*>::iterator iend = m_mapEntityInformation.end();//场景物件信息，用于创建一个场景物件的数据
		while (i!=iend)
		{
			KG3DSceneDataClip* pData = i->second;
			pData->m_nRefrence = 0;
			++i;
		}
		//////////////////////////////////////////////////////////////////////////
		map<UINT,KG3DEntityDataEx*>::iterator s = m_mapEntity.begin();
		map<UINT,KG3DEntityDataEx*>::iterator send = m_mapEntity.end();

		while(s!=send)
		{
			KG3DEntityDataEx* pEntity = s->second;

			map<UINT,KG3DSceneDataClip*>::iterator k = m_mapEntityInformation.find(pEntity->m_uEntityInformationHandle);
			if(k!=m_mapEntityInformation.end())
			{
				KG3DSceneDataClip* pData = k->second;
				pData->m_nRefrence++;
			}			
			++s;
		}
		//////////////////////////////////////////////////////////////////////////
		i = m_mapEntityInformation.begin();//场景物件信息，用于创建一个场景物件的数据
		iend = m_mapEntityInformation.end();//场景物件信息，用于创建一个场景物件的数据
		while (i!=iend)
		{
			KG3DSceneDataClip* pData = i->second;
			
			if(!pData->m_nRefrence)//如果引用计数为0.就删除
			{
				m_cDataClipPool.FreeResource(pData);
				i = m_mapEntityInformation.erase(i);
			}
			else
			{
				++i;
			}
		}
	}

	DWORD dwPos = ftell(pFile);

	DWORD dwNum = (DWORD)m_mapEntityInformation.size();
	fwrite(&dwNum,sizeof(DWORD),1,pFile);
	//////////////////////////////////////////////////////////////////////////

	map<UINT,KG3DSceneDataClip*>::iterator i = m_mapEntityInformation.begin();//场景物件信息，用于创建一个场景物件的数据
	map<UINT,KG3DSceneDataClip*>::iterator iend = m_mapEntityInformation.end();//场景物件信息，用于创建一个场景物件的数据
	dwPos = ftell(pFile);

	int K = 0;
	while (i!=iend)
	{
		UINT uHandle = i->first;
		KG3DSceneDataClip* pDataClip = i->second;

		fwrite(&uHandle,sizeof(UINT),1,pFile);
		dwPos = ftell(pFile);

		if(pDataClip)
		{
			TCHAR szName[MAX_PATH];
			GetEntityInformationFileName(szName,uHandle,FALSE);

			hr = pDataClip->SaveDataToFile(szName);
			if(FAILED(hr))
			{
				KGLogPrintf(KGLOG_ERR,"SaveEntityInfomationToFile %d 失败",uHandle);
			}
			
		}

		++i;
		K++;
	}

	dwPos = ftell(pFile);

	return S_OK;
}

HRESULT KG3DSceneDataInterfaceCore::SaveTerrainInfomationToFile(FILE* pFile)
{
	HRESULT hr = E_FAIL;

	DWORD dwNum = (DWORD)m_mapTerrainInformation.size();
	fwrite(&dwNum,sizeof(DWORD),1,pFile);
	//////////////////////////////////////////////////////////////////////////
	map<UINT,KG3DSceneDataClip*>::iterator i = m_mapTerrainInformation.begin();//场景物件信息，用于创建一个场景物件的数据
	map<UINT,KG3DSceneDataClip*>::iterator iend = m_mapTerrainInformation.end();//场景物件信息，用于创建一个场景物件的数据

	while (i!=iend)
	{
		UINT uHandle = i->first;
		KG3DSceneDataClip* pDataClip = i->second;

		fwrite(&uHandle,sizeof(UINT),1,pFile);

		if(pDataClip)
		{
			TCHAR szName[MAX_PATH];
			GetTerrainInformationFileName(szName,uHandle,FALSE);

			hr = pDataClip->SaveDataToFile(szName);
			if(FAILED(hr))
			{
				KGLogPrintf(KGLOG_ERR,"SaveTerrainInfomationToFile %d 失败",uHandle);
			}
		}
		++i;
	}
	return S_OK;
}

HRESULT KG3DSceneDataInterfaceCore::SaveSceneBlockToFile(FILE* pFile,BOOL& bConversion)
{
	HRESULT hrReturn = E_FAIL;
	HRESULT hr = E_FAIL;
	BOOL bForceLoad = FALSE;
	if(bConversion)
		bForceLoad = TRUE;
	else
		bForceLoad = _strcmpi(m_szDirectoryRead.c_str(),m_szDirectoryWrite.c_str());

	if(bForceLoad)
	{
		for (int s=3;s>=0;s--)
		{
			//////////////////////////////////////////////////////////////////////////
			map<POINT,KG3DSceneBlockData*>::iterator i = m_mapSceneBlockData[s].begin();//场景数据模块
			map<POINT,KG3DSceneBlockData*>::iterator iend = m_mapSceneBlockData[s].end();//场景数据模块

			while (i!=iend)
			{
				POINT Index = i->first;
				KG3DSceneBlockData* pBlock = i->second;

				if(pBlock)
				{
					PrepareSceneBlock(pBlock,s,Index);
					pBlock->m_bModified = TRUE;

					if((m_bPackData || bConversion)&&(pBlock->m_nLevel == BLOCK_TYPE_SECTION))
						pBlock->m_bSaveChildData = TRUE;
				}
				++i;
			}
		}
	}
	if(bConversion)
		m_bPackData = TRUE;
	if(m_bPackData)
	{
		set<KG3DSceneBlockData*> setLoadBlock;
		map<POINT,KG3DSceneBlockData*>::iterator i = m_mapSceneBlockData[BLOCK_TYPE_REGION].begin();//场景数据模块
		map<POINT,KG3DSceneBlockData*>::iterator iend = m_mapSceneBlockData[BLOCK_TYPE_REGION].end();//场景数据模块
		while (i!=iend)
		{
			KG3DSceneBlockData* pRegion = i->second;

			POINT parentSectionIndex;
			parentSectionIndex.x = pRegion->m_nIndexX / 8;
			parentSectionIndex.y = pRegion->m_nIndexZ / 8;

			KG3DSceneBlockData* pParentSection = FindSceneBlock(BLOCK_TYPE_SECTION,parentSectionIndex);
			if(pParentSection)
			{
				int x_Local = pRegion->m_nIndexX - parentSectionIndex.x * 8;
				int y_Local = pRegion->m_nIndexZ - parentSectionIndex.y * 8;
				int index_local = y_Local* 8 + x_Local; 

				if(!pParentSection->m_lpChildBlockData)
				{
					pParentSection->m_lpChildBlockData = new KG3DSceneBlockDataChildData;
					KGLOG_PROCESS_ERROR(pParentSection->m_lpChildBlockData);
				}

				pRegion->m_pParentBlockData = pParentSection;
				pParentSection->m_lpChildBlockData->m_zChildBlock[index_local] = pRegion;

				if(pRegion->m_bModified)
				{
					setLoadBlock.insert(pParentSection);
				}
			}
			i++;
		}	
		if(!bForceLoad)
		{
			POINT Index;
			set<KG3DSceneBlockData*>::iterator it = setLoadBlock.begin();
			while(it != setLoadBlock.end())
			{
				KG3DSceneBlockData* pScetion = *it;
				/*Index.x = pScetion->m_nIndexX;
				Index.y = pScetion->m_nIndexZ;
				PrepareSceneBlock(pScetion,BLOCK_TYPE_SECTION,Index);
				pScetion->m_bModified = TRUE;*/
				pScetion->m_bSaveChildData = TRUE;
				if(pScetion->m_lpChildBlockData)
				{
					for (int i=0;i<64;i++)
					{
						KG3DSceneBlockData* pRegion = pScetion->m_lpChildBlockData->m_zChildBlock[i];
						if(pRegion)
						{
							Index.x = pRegion->m_nIndexX;
							Index.y = pRegion->m_nIndexZ;
							PrepareSceneBlock(pRegion,BLOCK_TYPE_REGION,Index);
							pRegion->m_bModified = TRUE;
						}
					}
				}
				++it;
			}
		}
		
	}

	for (int s=3;s>=0;s--)
	{
		DWORD dwNum = (DWORD)m_mapSceneBlockData[s].size();
		fwrite(&dwNum,sizeof(DWORD),1,pFile);
		//////////////////////////////////////////////////////////////////////////
		map<POINT,KG3DSceneBlockData*>::iterator i = m_mapSceneBlockData[s].begin();//场景数据模块
		map<POINT,KG3DSceneBlockData*>::iterator iend = m_mapSceneBlockData[s].end();//场景数据模块

		while (i!=iend)
		{
			POINT Index = i->first;
			KG3DSceneBlockData* pBlock = i->second;

			fwrite(&Index,sizeof(POINT),1,pFile);
			if(m_bPackData && s == BLOCK_TYPE_REGION)
			{	
				++i;
				continue;
			}

			if(pBlock)
			{
				if(m_bPackData && s == BLOCK_TYPE_SECTION && pBlock->m_bSaveChildData)
				{
					TCHAR szChildName[MAX_PATH];
					GetSceneBlockChildFileName(szChildName,pBlock,FALSE);
					hr = pBlock->SaveChildDataToFile(szChildName);
					if(FAILED(hr))
						KGLogPrintf(KGLOG_ERR,"SaveSceneBlockToFile %d %d %d ChildData 失败",s,Index.x,Index.y);

					pBlock->m_bSaveChildData = FALSE;
				}
				if(pBlock->m_bDataInMemory && pBlock->m_bModified)
				{
					TCHAR szName[MAX_PATH];
					GetSceneBlockFileName(szName,pBlock,FALSE);
					hr = pBlock->SaveDataToFile(szName);
					if(FAILED(hr))
						KGLogPrintf(KGLOG_ERR,"SaveSceneBlockToFile %d %d %d 失败",s,Index.x,Index.y);

					pBlock->m_bModified = FALSE;
				}
			}
			++i;
		}
	}
	hrReturn = S_OK;
Exit0:
	return hrReturn;
}

HRESULT KG3DSceneDataInterfaceCore::LoadEntityFromFile(IFile* pFile)
{
	DWORD dwNowTime = timeGetTime();

	DWORD dwNum = 0;
	pFile->Read(&dwNum,sizeof(DWORD));
	//////////////////////////////////////////////////////////////////////////
	g_dwEntityNum = dwNum;
	UINT uHandle = 0;
    UINT uParent = INVALID_OBJSHELL_HANDLE;
	UINT uEntityInformationHandle = 0;
	KG3DEntityTransformation vTrans;

	for (DWORD i=0;i<dwNum;i++)
	{
		pFile->Read(&uHandle                 ,sizeof(UINT)                    );
		pFile->Read(&uEntityInformationHandle,sizeof(UINT)                    );


        if (m_dwVersion > MAP_FILE_VERSION_00)
        {
		    pFile->Read(&vTrans ,sizeof(KG3DEntityTransformation));
        }
        else
        {
            KG3DEntityTransformation_v0 TransV0;
            pFile->Read(&TransV0 ,sizeof(KG3DEntityTransformation_v0));
            
            vTrans.m_Rotation    = TransV0.m_Rotation;
            vTrans.m_Scaling     = TransV0.m_Scaling;
            vTrans.m_Translation = TransV0.m_Translation;
            vTrans.m_vBox_A      = TransV0.m_vBox_A;
            vTrans.m_vBox_B      = TransV0.m_vBox_B;
            vTrans.m_vWorldPos  = TransV0.m_Translation;
        }


        if (m_dwVersion > MAP_FILE_VERSION_00)
            pFile->Read(&uParent, sizeof(uParent));


		{
			KG3DEntityDataEx* pNewEntity = m_cEntityDataExPool.RequestResource();
			if (pNewEntity)
			{
				pNewEntity->m_uHandle = uHandle;
				pNewEntity->m_uEntityInformationHandle = uEntityInformationHandle;
				pNewEntity->m_Transformation = vTrans;
                pNewEntity->m_uParent = uParent;

				m_mapEntity[ pNewEntity->m_uHandle ] = pNewEntity;

				InputEntity(pNewEntity);

				if(m_lpEventConnection)
				{
					m_lpEventConnection->OnEntityCreate(pNewEntity->m_uHandle, FALSE);

					m_lpEventConnection->OnEntityTransformModified(pNewEntity->m_uHandle, FALSE);
				}
			}
			else
			{
				KGLogPrintf(KGLOG_ERR,"m_cEntityDataExPool.RequestResource() 失败");
			}
		}
	}

	if(g_cEngineOption.bEnableTimeOptimizeLog)
	{
		DWORD dwCost = timeGetTime() - dwNowTime;
		KGLogPrintf(KGLOG_INFO,"TimeOptimize KG3DSceneDataInterfaceCore::LoadEntityFromFile cost %d",dwCost);
	}

	return S_OK;
}

HRESULT KG3DSceneDataInterfaceCore::LoadEntityInfomationFromFile(IFile* pFile)
{
	HRESULT hr = E_FAIL;
	DWORD dwNowTime = timeGetTime();

	DWORD dwPos = pFile->Tell();

	DWORD dwNum = 0;
	pFile->Read(&dwNum,sizeof(DWORD));
	//////////////////////////////////////////////////////////////////////////

	for (DWORD i=0;i<dwNum;i++)
	{
		UINT uHandle = 0;

		pFile->Read(&uHandle,sizeof(UINT));

		KG3DSceneDataClip* pNewInfo = m_cDataClipPool.RequestResource();
		if(pNewInfo)
		{
			m_mapEntityInformation[ uHandle] = pNewInfo;

			TCHAR szName[MAX_PATH];
			GetEntityInformationFileName(szName,uHandle,TRUE);

			hr = pNewInfo->LoadDataFromFile(szName);
			if (FAILED(hr))
			{
				KGLogPrintf(KGLOG_ERR,"加载失败 %s",szName);
			}
		}
		else
		{
			KGLogPrintf(KGLOG_ERR,"m_cDataClipPool.RequestResource() 失败");
		}
	}
	
	dwPos = pFile->Tell();

	if(g_cEngineOption.bEnableTimeOptimizeLog)
	{
		DWORD dwCost = timeGetTime() - dwNowTime;
		KGLogPrintf(KGLOG_INFO,"TimeOptimize KG3DSceneDataInterfaceCore::LoadEntityInfomationFromFile cost %d",dwCost);
	}

	return S_OK;

}

HRESULT KG3DSceneDataInterfaceCore::LoadTerrainInfomationFromFile(IFile* pFile)
{
	HRESULT hr = E_FAIL;
	DWORD dwNum = 0;
	DWORD dwNowTime = timeGetTime();

	pFile->Read(&dwNum,sizeof(DWORD));
	//////////////////////////////////////////////////////////////////////////

	for (DWORD i=0;i<dwNum;i++)
	{
		UINT uHandle = 0;

		pFile->Read(&uHandle,sizeof(UINT));

		KG3DSceneDataClip* pNewInfo = m_cDataClipPool.RequestResource();
		if(pNewInfo)
		{
			m_mapTerrainInformation[ uHandle ] = pNewInfo;

			TCHAR szName[MAX_PATH];
			GetTerrainInformationFileName(szName,uHandle,TRUE);

			hr = pNewInfo->LoadDataFromFile(szName);
			if (FAILED(hr))
			{
				KGLogPrintf(KGLOG_ERR,"加载失败 %s",szName);
			}
			else
			{
				if(m_lpEventConnection)
				{
					m_lpEventConnection->OnTerrainInformationModified(uHandle,pNewInfo->m_dwType);
				}
			}
		}
		else
		{
			KGLogPrintf(KGLOG_ERR,"m_cDataClipPool.RequestResource() 失败");
		}
	}

	if(g_cEngineOption.bEnableTimeOptimizeLog)
	{
		DWORD dwCost = timeGetTime() - dwNowTime;
		KGLogPrintf(KGLOG_INFO,"TimeOptimize KG3DSceneDataInterfaceCore::LoadTerrainInfomationFromFile cost %d",dwCost);
	}

	return S_OK;
}


HRESULT KG3DSceneDataInterfaceCore::LoadSceneBlockFromFile(IFile* pFile)
{
	DWORD dwNowTime = timeGetTime();

	m_vBBox.Clear();

	for (int s=3;s>=0;s--)
	{
		DWORD dwNum = 0;
		pFile->Read(&dwNum,sizeof(DWORD));
		//////////////////////////////////////////////////////////////////////////
		for (DWORD i=0;i<dwNum;i++)
		{
			POINT Index;
			pFile->Read(&Index,sizeof(POINT));

			Request_CreateSceneBlock(s,Index);

			KG3DSceneBlockData* pBlick = FindSceneBlock(s,Index);
			if(pBlick)
			{
				pBlick->m_bDataInMemory = FALSE;

				if(0)
				{
					PrepareSceneBlock(pBlick,s,Index);

					if(pBlick->m_bTerrainBlock)
					{
						if (m_lpEventConnection)
						{
							m_lpEventConnection->OnTerrainBlockCreate(s,Index);

							map<DWORD,KG3DSceneDataClip*>::iterator t = pBlick->m_mapData.begin();
							map<DWORD,KG3DSceneDataClip*>::iterator tend = pBlick->m_mapData.end();
							while (t!=tend)
							{
								DWORD dwType = t->first;
								//KG3DSceneDataClip* pData = t->second;

								m_lpEventConnection->OnTerrainBlockModified(s,Index,dwType);
								++t;
							}
						}
					}
				}
			}
		}
	}

	if(g_cEngineOption.bEnableTimeOptimizeLog)
	{
		DWORD dwCost = timeGetTime() - dwNowTime;
		KGLogPrintf(KGLOG_INFO,"TimeOptimize KG3DSceneDataInterfaceCore::LoadSceneBlockFromFile cost %d",dwCost);
	}

	return S_OK;
}

HRESULT KG3DSceneDataInterfaceCore::CreateMapFileHeader(MapFileHeader* pHeader)
{
	ZeroMemory(pHeader,sizeof(MapFileHeader));
	pHeader->dwMask = 0x10203040;
	
	if(m_bPackData)
		pHeader->dwVersion = MAP_FILE_VERSION_PACKREGION;
	else
		pHeader->dwVersion = MAP_FILE_VERSION;

	pHeader->uCurrentEntityID = m_uCurrentEntityID;
	pHeader->uCurrentEntityInformationID = m_uCurrentEntityInformationID;
	pHeader->uCurrentTerrainInformationID = m_uCurrentTerrainInformationID;
	pHeader->dwNumEntity = (DWORD)m_mapEntity.size();
	pHeader->dwNumEntityInformation = (DWORD)m_mapEntityInformation.size();
	pHeader->dwNumTerrainInformation = (DWORD)m_mapTerrainInformation.size();
	for (int i=0;i<4;i++)
	{
		pHeader->dwNumSceneBlock[i] = (DWORD) m_mapSceneBlockData[i].size();
	}

	return S_OK;
}

HRESULT KG3DSceneDataInterfaceCore::GetSceneRect(D3DXVECTOR2& LowerLeft,D3DXVECTOR2& UpperRight)//获取整个场景的长宽
{
	//m_vBBox.GetXZRect(A,C);
	//Box是没有排序的，不能保证AC点的正确性
	FLOAT xMin = m_vBBox.A.x;
	FLOAT xMax = m_vBBox.B.x;
	if(xMin > xMax)
		std::swap(xMin, xMax);

	FLOAT zMin = m_vBBox.A.z;
	FLOAT zMax = m_vBBox.B.z;
	if(zMin > zMax)
		std::swap(zMin, zMax);

	LowerLeft = D3DXVECTOR2(xMin, zMin);
	UpperRight = D3DXVECTOR2(xMax, zMax);
	
	return S_OK;
}

HRESULT KG3DSceneDataInterfaceCore::GetEntityOnSceneBlock(set<UINT>*psetEntity,int nLevel,POINT Index)
{
	KGLOG_PROCESS_ERROR( nLevel>=0 );
	KGLOG_PROCESS_ERROR( nLevel<4  );

	{
		KG3DSceneBlockData* pBlock = FindSceneBlock(nLevel,Index);
		KGLOG_PROCESS_ERROR(pBlock);

		psetEntity->clear();

		set<KG3DEntityDataEx*>::iterator n = pBlock->m_setEntity.begin();
		set<KG3DEntityDataEx*>::iterator nEnd = pBlock->m_setEntity.end();
		while (n!=nEnd)
		{
			KG3DEntityDataEx* pEntity = *n;

			psetEntity->insert(pEntity->m_uHandle);

			++n;
		}
	}

	return S_OK;
Exit0:
	return E_FAIL;
}



HRESULT KG3DSceneDataInterfaceCore::Request_ModifyTerrainInformation(UINT uHandle,DWORD dwDataType,DWORD dwLength,PVOID pData)
{
	HRESULT hr = E_FAIL;
	BOOL IsModify = FALSE;
	KG3DSceneDataClip* pTerrainInfo = FindTerrainInforamtion(uHandle);  // 查找是否存在uHandle对应的地表信息数据
	if (pTerrainInfo)
	{
		if(!pTerrainInfo->IsEqual(dwDataType,dwLength,pData))//比较新数据与旧数据的类型，数据块大小是否一样。相同的情况，新数据直接覆盖旧数据
		{//不相同则
			//UINT uNewHandle = 0;
			KG3DSceneDataClip* pNewInfo = m_cDataClipPool.RequestResource();//申请新的存储空间，
			KGLOG_PROCESS_ERROR(pNewInfo);

			pNewInfo->SetData(dwDataType,dwLength,pData);//往存储空间写入数据
			m_mapTerrainInformation[uHandle] = pNewInfo;//uHandle与新数据块建立连接。
			
			m_cDataClipPool.FreeResource(pTerrainInfo);//删除原来的地表信息数据
		}
		IsModify = TRUE;
	}
	

	if (IsModify && m_lpEventConnection)
	{
		m_lpEventConnection->OnTerrainInformationModified(uHandle,dwDataType);
	}
	
	hr = S_OK;
Exit0:
	return hr;
}


HRESULT KG3DSceneDataInterfaceCore::SaveEntityInfoToFile(LPCTSTR pFileName)
{
	{
		map<UINT,KG3DSceneDataClip*>::iterator i = m_mapEntityInformation.begin();//场景物件信息，用于创建一个场景物件的数据
		map<UINT,KG3DSceneDataClip*>::iterator iend = m_mapEntityInformation.end();//场景物件信息，用于创建一个场景物件的数据
		while (i!=iend)
		{
			KG3DSceneDataClip* pData = i->second;
			pData->m_nRefrence = 0;
			++i;
		}
		//////////////////////////////////////////////////////////////////////////
		map<UINT,KG3DEntityDataEx*>::iterator s = m_mapEntity.begin();
		map<UINT,KG3DEntityDataEx*>::iterator send = m_mapEntity.end();

		while(s!=send)
		{
			KG3DEntityDataEx* pEntity = s->second;

			map<UINT,KG3DSceneDataClip*>::iterator k = m_mapEntityInformation.find(pEntity->m_uEntityInformationHandle);
			if(k!=m_mapEntityInformation.end())
			{
				KG3DSceneDataClip* pData = k->second;
				pData->m_nRefrence++;
			}			
			++s;
		}
		//////////////////////////////////////////////////////////////////////////
		i = m_mapEntityInformation.begin();//场景物件信息，用于创建一个场景物件的数据
		iend = m_mapEntityInformation.end();//场景物件信息，用于创建一个场景物件的数据
		while (i!=iend)
		{
			KG3DSceneDataClip* pData = i->second;

			if(!pData->m_nRefrence)//如果引用计数为0.就删除
			{
				m_cDataClipPool.FreeResource(pData);
				i = m_mapEntityInformation.erase(i);
			}
			else
			{
				++i;
			}
		}
	}

	DWORD dwPosSave = 0;
	FILE* pFile = NULL;
	DWORD dwNum = (DWORD)m_mapEntityInformation.size();
	HRESULT hr = E_FAIL;
	DWORD version = MAP_FILE_VERSION_PACKREGION;
	//////////////////////////////////////////////////////////////////////////

	pFile = fopen(pFileName,"wb");
	KG_PROCESS_ERROR(pFile);
	
	fwrite(&version,sizeof(DWORD),1,pFile);
	fwrite(&dwNum,sizeof(DWORD),1,pFile);
	dwPosSave = ftell(pFile);
	if(dwNum > 0)
	{	
		int n = 0;

		InfoFileData* pInfoData = new InfoFileData[dwNum];

		fwrite(pInfoData,sizeof(InfoFileData),dwNum,pFile);

		map<UINT,KG3DSceneDataClip*>::iterator i = m_mapEntityInformation.begin();//场景物件信息，用于创建一个场景物件的数据
		map<UINT,KG3DSceneDataClip*>::iterator iend = m_mapEntityInformation.end();//场景物件信息，用于创建一个场景物件的数据

		while (i!=iend)
		{
			pInfoData[n].uHandle= i->first;
			KG3DSceneDataClip* pDataClip = i->second;

			pInfoData[n].dwFilePos = ftell(pFile);

			if(pDataClip)
			{
				pDataClip->SaveDataToFile(pFile);
			}
			++i;
			n++;
		}
		fseek(pFile,dwPosSave,SEEK_SET);
		fwrite(pInfoData,sizeof(InfoFileData)* dwNum,1,pFile);

		if(pInfoData)
			SAFE_DELETE_ARRAY(pInfoData);
	}
	hr = S_OK;
Exit0:
	if(pFile)
	{
		fclose(pFile);
		pFile = NULL;
	}
	
	return hr;
}

HRESULT KG3DSceneDataInterfaceCore::LoadEntityInfoFromFile(LPCTSTR pFileName)
{
	HRESULT hr = E_FAIL;
	DWORD dwNowTime = timeGetTime();
	DWORD version = 0;
	unsigned long readcount = 0;
	IFile* pFile = NULL;
	DWORD dwNum = 0;
	KGLOG_PROCESS_ERROR(pFileName);
	pFile = g_OpenFile(pFileName);
	KG_PROCESS_ERROR(pFile);

	readcount = pFile->Read(&version,sizeof(DWORD));
	KGLOG_PROCESS_ERROR(readcount == sizeof(DWORD));

	_ASSERTE(version == MAP_FILE_VERSION_PACKREGION);

	readcount = pFile->Read(&dwNum,sizeof(DWORD));
	KGLOG_PROCESS_ERROR(readcount == sizeof(DWORD));

	if(dwNum > 0)
	{	

		InfoFileData* pInfoData = new InfoFileData[dwNum];
		
		readcount = pFile->Read(pInfoData,sizeof(InfoFileData)* dwNum);
		KGLOG_PROCESS_ERROR(readcount == sizeof(InfoFileData)* dwNum);
		
		for(int i = 0; i< (int)dwNum; i++)
		{
			KG3DSceneDataClip* pNewInfo = m_cDataClipPool.RequestResource();
			if(pNewInfo)
			{	
				std::map<UINT,KG3DSceneDataClip*>::_Pairib InsertResult;
				InsertResult = m_mapEntityInformation.insert(std::pair<UINT,KG3DSceneDataClip*>(pInfoData[i].uHandle,pNewInfo));
				ASSERT(InsertResult.second);

			//	m_mapEntityInformation[pInfoData[i].uHandle ] = pNewInfo;
				{
					pFile->Seek(pInfoData[i].dwFilePos,SEEK_SET);
					hr = pNewInfo->LoadDataFromFile(pFile);
					if (FAILED(hr))
					{
						KGLogPrintf(KGLOG_ERR,"加载失败 EntityInformation Handle %d",pInfoData[i].uHandle);
					}
				}
			}
			else
			{
				KGLogPrintf(KGLOG_ERR,"m_cDataClipPool.RequestResource() 失败");
			}

		}
		if(pInfoData)
			SAFE_DELETE_ARRAY(pInfoData);
	}
	hr = S_OK;
Exit0:
	KG_COM_RELEASE(pFile);

	if(g_cEngineOption.bEnableTimeOptimizeLog)
	{
		DWORD dwCost = timeGetTime() - dwNowTime;
		KGLogPrintf(KGLOG_INFO,"TimeOptimize KG3DSceneDataInterfaceCore::LoadEntityInfomationFromFile cost %d",dwCost);
	}

	return hr;
}


HRESULT KG3DSceneDataInterfaceCore::SaveTerrainInfoToFile(LPCTSTR pFileName)
{
	DWORD dwPosSave = 0;
	FILE* pFile = NULL;
	HRESULT hr = E_FAIL;

	DWORD version = MAP_FILE_VERSION_PACKREGION;
	DWORD dwNum = (DWORD)m_mapTerrainInformation.size();
	//////////////////////////////////////////////////////////////////////////
	
	pFile = fopen(pFileName,"wb");
	KG_PROCESS_ERROR(pFile);

	fwrite(&version,sizeof(DWORD),1,pFile);
	fwrite(&dwNum,sizeof(DWORD),1,pFile);
	dwPosSave = ftell(pFile);
	if(dwNum > 0)
	{	
		int n = 0;

		InfoFileData* pInfoData = new InfoFileData[dwNum];

		fwrite(pInfoData,sizeof(InfoFileData),dwNum,pFile);

		map<UINT,KG3DSceneDataClip*>::iterator i = m_mapTerrainInformation.begin();
		map<UINT,KG3DSceneDataClip*>::iterator iend = m_mapTerrainInformation.end();

		while (i!=iend)
		{
			pInfoData[n].uHandle= i->first;
			KG3DSceneDataClip* pDataClip = i->second;

			pInfoData[n].dwFilePos = ftell(pFile);

			if(pDataClip)
			{
				pDataClip->SaveDataToFile(pFile);
			}
			++i;
			n++;
		}
		fseek(pFile,dwPosSave,SEEK_SET);
		fwrite(pInfoData,sizeof(InfoFileData)* dwNum,1,pFile);

		if(pInfoData)
			SAFE_DELETE_ARRAY(pInfoData);
	}
	hr = S_OK;
Exit0:
	if(pFile)
	{
		fclose(pFile);
		pFile = NULL;
	}		
	return hr;
}


HRESULT KG3DSceneDataInterfaceCore::LoadTerrainInfoFromFile(LPCTSTR pFileName)
{
	HRESULT hr = E_FAIL;
	DWORD version = 0;
	unsigned long readcount = 0;
	IFile* pFile = NULL;
	DWORD dwNum = 0;
	DWORD dwNowTime = timeGetTime();
	KGLOG_PROCESS_ERROR(pFileName);
	pFile = g_OpenFile(pFileName);
	KG_PROCESS_ERROR(pFile);

	readcount = pFile->Read(&version,sizeof(DWORD));
	KGLOG_PROCESS_ERROR(readcount == sizeof(DWORD));

	_ASSERTE(version == MAP_FILE_VERSION_PACKREGION);

	readcount = pFile->Read(&dwNum,sizeof(DWORD));
	KGLOG_PROCESS_ERROR(readcount == sizeof(DWORD));

	if(dwNum > 0)
	{	

		InfoFileData* pInfoData = new InfoFileData[dwNum];

		readcount = pFile->Read(pInfoData,sizeof(InfoFileData)* dwNum);
		KGLOG_PROCESS_ERROR(readcount == sizeof(InfoFileData)* dwNum);

		for(int i = 0; i< (int)dwNum; i++)
		{
			KG3DSceneDataClip* pNewInfo = m_cDataClipPool.RequestResource();
			if(pNewInfo)
			{	
				std::map<UINT,KG3DSceneDataClip*>::_Pairib InsertResult;
				InsertResult = m_mapTerrainInformation.insert(std::pair<UINT,KG3DSceneDataClip*>(pInfoData[i].uHandle,pNewInfo));
				ASSERT(InsertResult.second);

				{
					pFile->Seek(pInfoData[i].dwFilePos,SEEK_SET);
					hr = pNewInfo->LoadDataFromFile(pFile);
					if (FAILED(hr))
					{
						KGLogPrintf(KGLOG_ERR,"加载失败 TerrainInformation Handle %d",pInfoData[i].uHandle);
					}
					else
					{
						if(m_lpEventConnection)
						{
							m_lpEventConnection->OnTerrainInformationModified(pInfoData[i].uHandle,pNewInfo->m_dwType);
						}
					}
				}
			}
			else
			{
				KGLogPrintf(KGLOG_ERR,"m_cDataClipPool.RequestResource() 失败");
			}

		}
		if(pInfoData)
			SAFE_DELETE_ARRAY(pInfoData);
	}
	hr = S_OK;
Exit0:
	KG_COM_RELEASE(pFile);

	if(g_cEngineOption.bEnableTimeOptimizeLog)
	{
		DWORD dwCost = timeGetTime() - dwNowTime;
		KGLogPrintf(KGLOG_INFO,"TimeOptimize KG3DSceneDataInterfaceCore::LoadTerrainInfomationFromFile cost %d",dwCost);
	}
	return hr;
}