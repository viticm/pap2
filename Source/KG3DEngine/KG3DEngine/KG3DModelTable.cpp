#include "StdAfx.h"
#include "KG3DModelST.h"
#include "kg3dmodeltable.h"
#include "KG3DScene.h"
#include "KG3DSFX.h"
#include "KG3DModelSpeedTree.h"
#include "kg3dmodelpointlight.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KG3DModelTable g_cModelTable;
KG3DModelSTCache g_cModelSTCache;

static TypeInfo s_ModelTableTypes[] = {
	{ ".mesh",   MESHTYPE_DEFAULT,   TRUE }, 
	{ ".stree",  MESHTYPE_SPEEDTREE, TRUE },
	{ ".mdl",    MESHTYPE_MODELST,   TRUE },
	{ ".modelst",MESHTYPE_MODELST,   TRUE },
	{ ".sfx",    MESHTYPE_SFX,       TRUE },
	{ ".bind",   MESHTYPE_SUEDE,     TRUE },
	{ ".plight", MESHTYPE_POINTLIGHT,TRUE }
};

IKG3DResourceManager* g_GetModelManager()
{
	return &g_cModelTable;
}

KG3DModelTable::KG3DModelTable(void)
{
	m_dwReleaseQueueSize = 80;
}

KG3DModelTable::~KG3DModelTable(void)
{
	;
}

HRESULT KG3DModelTable::Init()
{
	HRESULT hRetCode = TKG3DResourceManagerBase<KG3DModel>::Init();
	m_RefTable = s_ModelTableTypes;
	m_RefTableSize = sizeof(s_ModelTableTypes) / sizeof(TypeInfo);
	m_pDefaultType = NULL;
	return hRetCode;
}

HRESULT KG3DModelTable::UnInit()
{	
    TypeExclusiveList temp = m_listExclusiveRes;
    m_listExclusiveRes.clear();

    for (TypeExclusiveList::iterator i = temp.begin(); 
         i != temp.end(); ++i)
    {
        KG3DModel* pModel = (KG3DModel*)*i;
        if (pModel->GetType() == MESHTYPE_SFX || pModel->GetType() == MESHTYPE_SUEDE)
            m_listExclusiveRes.push_front(pModel);
        else
            m_listExclusiveRes.push_back(pModel);
    }

    temp.clear();

	return TKG3DResourceManagerBase<KG3DModel>::UnInit();
}

HRESULT KG3DModelTable::Get1NewResourcePointer(KG3DModel** ppT, DWORD dwType, DWORD_PTR Option)
{
    HRESULT hRetCode = E_FAIL;
	BOOL bNeedInit = TRUE;
	KG_PROCESS_ERROR(ppT);

	*ppT = NULL;
	switch(dwType)
    {
    case MESHTYPE_SPEEDTREE :
        (*ppT) = new KG3DModelSpeedTree;
        break;
	case MESHTYPE_MODELST:
		(*ppT) = new KG3DModelST;
		break;
    case MESHTYPE_SFX:
        (*ppT) = new KG3DSFX;
        break;
    case MESHTYPE_SUEDE :
        (*ppT) = new KG3DSFXSuede;
        break;
	case MESHTYPE_POINTLIGHT:
		(*ppT) = new KG3DModelPointLight;
		break;
	default:
        TKG3DResourceManagerBase<KG3DModel>::Get1NewResourcePointer(
            ppT, dwType, Option
        );
		bNeedInit = FALSE;
    }
    KG_PROCESS_ERROR(*ppT);

	if (bNeedInit)
	{
		(*ppT)->Init();
		(*ppT)->SetType(dwType);
	}

    hRetCode = S_OK;
Exit0:
    return hRetCode;
}

HRESULT KG3DModelTable::Get1NewResourse(IKG3DResourceBase** ppResource,DWORD dwType)
{
    HRESULT hResult = E_FAIL;
    KG3DModel *pModel = NULL;
	hResult = TKG3DResourceManagerBase<KG3DModel>::Get1NewResource(&pModel,dwType,0);
    *ppResource = pModel;
    return hResult;
}

HRESULT KG3DModelTable::IELoadModelFromFile(
    const TCHAR cszFileName[],
    UINT uFileNameHash,
    UINT uOption,
    IEKG3DModel **ppRetResource
)
{
    HRESULT hResult = E_FAIL;
    KG3DModel *pModel = NULL; 

    KGLOG_PROCESS_ERROR(cszFileName);
    KGLOG_PROCESS_ERROR(ppRetResource);

    hResult = LoadResourceFromFile(cszFileName, uFileNameHash, uOption, (IKG3DResourceBase **)&pModel);
    KGLOG_COM_PROCESS_ERROR(hResult);

    *ppRetResource = pModel;

    hResult = S_OK;
Exit0:
    return hResult;

}
HRESULT KG3DModelTable::IELoadResourceFromFile(
    const TCHAR cszFileName[], 
    unsigned uFileNameHash, 
    unsigned uOption, 
    IKG3DResourceBase **ppRetResource
)
{
    HRESULT hResult = E_FAIL;

    KGLOG_PROCESS_ERROR(cszFileName);
    KGLOG_PROCESS_ERROR(ppRetResource);

    hResult = LoadResourceFromFile(cszFileName, uFileNameHash, uOption, ppRetResource);
    KGLOG_COM_PROCESS_ERROR(hResult);

    hResult = S_OK;
Exit0:
    return hResult;

}
HRESULT KG3DModelTable::IEGet1NewResourse(IEKG3DModel **ppModel, DWORD dwType)
{
    HRESULT hResult = E_FAIL;
    KG3DModel *pModel = NULL;
  
    hResult = TKG3DResourceManagerBase<KG3DModel>::Get1NewResource(&pModel,dwType,0);
    *ppModel = pModel;

    return hResult;
}

HRESULT KG3DModelTable::GetResourse(IKG3DResourceBase** ppResource,DWORD dwID)
{
	return TKG3DResourceManagerBase<KG3DModel>::GetResource((KG3DModel**)(ppResource),dwID);
}

HRESULT KG3DModelTable::SetBalance(DWORD nSizeMemery,DWORD nSzieVideo)
{
	return S_OK;
}

HRESULT KG3DModelTable::LoadResourceFromFile(const char cszFileName[],
									 unsigned uFileNameHash, 
									 unsigned uOption, 
									 IKG3DResourceBase **ppRetResource)
{
    HRESULT hrResult = E_FAIL;
	HRESULT hrRetCode = E_FAIL;
	
	TypeInfo* pTypeInfo = NULL;
	hrRetCode = GetTypeInfoByFileName(&pTypeInfo, cszFileName);
	KG_COM_PROCESS_ERROR(hrRetCode);

	/*if (g_eEngineOption.bUseMultiThreadLoad)
	{
		uOption |= MLO_TEXTURE_MULTI;
	}*/
	
	hrRetCode = LoadResourceFromFileEx(cszFileName,
		uFileNameHash,
		uOption,
		NULL,
		NULL,
		ppRetResource);
	KG_COM_PROCESS_ERROR(hrRetCode);


    g_LoadPostProcessor.InputModel((KG3DModel*)*ppRetResource);

	hrResult = S_OK;
Exit0:
	return hrResult;
}


const MDLFileContent* KG3DModelSTCache::GetMDLContent(LPCSTR strFileName)
{
	m_lock.Lock();

	const MDLFileContent *pContent = NULL;
	DWORD dwHash = g_FileNameHash(strFileName);
	std::map<DWORD, MDLFileContent>::iterator it = m_Cache.find(dwHash);
	if (it != m_Cache.end())
	{
		pContent = &it->second;
		std::deque<DWORD>::iterator itFind = find(m_LastUsed.begin(), m_LastUsed.end(), dwHash);
		if (itFind != m_LastUsed.end())
		{
			m_LastUsed.erase(itFind);
		}
	}
	else
	{
		MDLFileContent Content;
		HRESULT hResult = KG3DModelST::LoadMDLContent(strFileName, Content);
		KG_COM_PROCESS_ERROR(hResult);

		m_Cache[dwHash] = Content;
		pContent = &m_Cache[dwHash];
	}
	m_LastUsed.push_back(dwHash);
	OnCompact();
Exit0:
	m_lock.Unlock();

	return pContent;
}

BOOL KG3DModelSTCache::CheckMDL(LPCSTR strFileName)
{
	m_lock.Lock();
	BOOL bReturn = FALSE;
	DWORD dwHash = g_FileNameHash(strFileName);
	std::map<DWORD, MDLFileContent>::iterator it = m_Cache.find(dwHash);
	if (it != m_Cache.end())
	{
		bReturn = TRUE;
	}
	m_lock.Unlock();
	return bReturn;
}

void KG3DModelSTCache::OnCompact()
{
	m_lock.Lock();
	int nSize = static_cast<int>(m_Cache.size()) - s_nMaxCacheSize;
	while (nSize > 0)
	{
		DWORD dwToErase = *m_LastUsed.begin();
		m_Cache.erase(m_Cache.find(dwToErase));
		m_LastUsed.pop_front();
		nSize--;
	}
	m_lock.Unlock();
}