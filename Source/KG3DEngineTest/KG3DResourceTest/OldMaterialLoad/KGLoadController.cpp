#include "KGLoadController.h"
#include "KG3DTexture.h"
#include "KG3DMaterial.h"
#include "KG3DEngineManager.h"

KGLoadController g_cKGLoadController;
#ifdef _DEBUG
const char ENGINE_DLL_NAME[] = "KG3DEngineD.dll";
#else
const char ENGINE_DLL_NAME[] = "KG3DEngine.dll";
#endif

KGLoadController::KGLoadController()
{
	m_pEngineMgr = NULL;
	m_pMaterial  = NULL;
	m_hEngineDLL = NULL;
	m_dwNumMaterials = 0;
	CleanVecSubset();
}

KGLoadController::~KGLoadController()
{
	UnInit();
}

int KGLoadController::Init(const char cszClientPath[])
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hrRetCode = E_FAIL;
	HWND hWindow = NULL;
	char szClientPath[MAX_PATH]    = {0};
	char szEngineDLLName[MAX_PATH] = {0};

	_ASSERTE(cszClientPath);

	nRetCode = strncpy_s(szClientPath, sizeof(szClientPath), cszClientPath, strlen(cszClientPath));
	KG_PROCESS_ERROR(nRetCode == 0);
	nRetCode = FormatPath(szClientPath, sizeof(szClientPath));
	KG_PROCESS_ERROR(nRetCode);

 	g_SetRootPath(szClientPath);
 
	nRetCode = _snprintf_s(szEngineDLLName,
						   sizeof(szEngineDLLName),
						   sizeof(szEngineDLLName) - 1,
						   "%s%s",
						   szClientPath,
						   ENGINE_DLL_NAME
				);
	KG_PROCESS_ERROR(nRetCode != -1);
 	m_hEngineDLL = LoadLibrary(szEngineDLLName);
	KG_PROCESS_ERROR(m_hEngineDLL);

	hWindow = ::GetDesktopWindow();
	KG_PROCESS_ERROR(hWindow);

	typedef HRESULT (*pfnGet3DEngineInterface)(void** ppvEngineManager);
	pfnGet3DEngineInterface Get3DEngineInterface;
 
 	Get3DEngineInterface = (pfnGet3DEngineInterface)GetProcAddress(m_hEngineDLL, "Get3DEngineInterface");
	Get3DEngineInterface((void**)&m_pEngineMgr);
	KG_PROCESS_ERROR(m_pEngineMgr);
	hrRetCode = m_pEngineMgr->Init(0, KG3DENG_CLIENT, hWindow, hWindow);
	KG_COM_PROCESS_ERROR(hrRetCode);

	nResult = true;
Exit0:
	if (!nResult)
	{
		UnInit();
	}
	return nResult;
}

void KGLoadController::UnInit()
{
	if (m_pEngineMgr)
	{
		m_pEngineMgr->UnInit();
		m_pEngineMgr = NULL;
	}
	CleanVecSubset();
	if (m_hEngineDLL)
	{
		FreeLibrary(m_hEngineDLL);
		m_hEngineDLL = NULL;
	}

}

int KGLoadController::LoadMaterialByOldFunc(const char cszFileName[])
{
	int nResult		= false;
	int nRetCode	= false;
	HRESULT hrRetCode = E_FAIL;
	IEKG3DMaterial* pMaterial = NULL;

	_ASSERTE(cszFileName);

	hrRetCode = m_pEngineMgr->GetNewIEKG3DMaterial(&pMaterial);
	KG_COM_PROCESS_ERROR(hrRetCode);
	KG_PROCESS_ERROR(pMaterial);
	hrRetCode = pMaterial->Init();
	KG_COM_PROCESS_ERROR(hrRetCode);
	hrRetCode = pMaterial->LoadFromFile(cszFileName, 0, 0);
	KG_COM_PROCESS_ERROR(hrRetCode);
	m_pMaterial = pMaterial;

	nRetCode = SetMaterialData();
	KG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	if (!nResult && m_pMaterial)
	{
		m_pMaterial->UnInit();
		m_pEngineMgr->DestroyIEKG3DMaterial(m_pMaterial);
	}
	return nResult;
}

int KGLoadController::SetMaterialData()
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hrRetCode = E_FAIL;
	OldMaterialSubset OldSubset;
	KG3DMaterial* pMaterial = NULL;
	KG3DMaterial::_MtlOption* pOption = NULL;

	CleanVecSubset();

	pMaterial = dynamic_cast<KG3DMaterial*>(m_pMaterial);	
	m_BaseMtlData.m_bHasDetail = pMaterial->m_bHasDetail;
	m_BaseMtlData.m_bSortAsSFX = pMaterial->m_bSortAsSFX;
	m_BaseMtlData.m_eLoadingState = pMaterial->m_eLoadingState;

	hrRetCode = m_pMaterial->GetMaterialCount(&m_dwNumMaterials);
	KG_COM_PROCESS_ERROR(hrRetCode);
	for (DWORD dwMtlIndex = 0; dwMtlIndex < m_dwNumMaterials; dwMtlIndex++)
	{
		ZeroMemory((void*)&OldSubset, sizeof(OldMaterialSubset));
		KG3DMaterial::KG3DMaterialSubset* pSubset = NULL;
		hrRetCode = m_pMaterial->GetSubMaterial(dwMtlIndex, (PVOID*)&pSubset);
		KG_COM_PROCESS_ERROR(hrRetCode);

		OldSubset.m_dwPassID	 = pSubset->m_dwPassID;
		OldSubset.m_dwMaterialID = pSubset->m_dwMaterialID;

		OldSubset.m_Material9    = pSubset->m_sMaterial9;
		OldSubset.m_dwOption     = pSubset->m_dwOption;
		OldSubset.m_dwNumOptions = pSubset->m_dwNumOptions;
		//材质选项数据
		for (DWORD dwMtlOptionIndex = 0; dwMtlOptionIndex < OldSubset.m_dwNumOptions; dwMtlOptionIndex++)
		{
			pOption = pSubset->m_pMatOptions[dwMtlOptionIndex];
			OldSubset.m_pMatOptions.push_back(pOption);
		}
		//贴图数据
		OldSubset.m_dwNumUsedTexture = pSubset->m_dwNumUsedTexture;
		for(DWORD dwTexIndex = 0; dwTexIndex < OldSubset.m_dwNumUsedTexture; dwTexIndex++)
		{
			OldSubset.m_TextureName[dwTexIndex] = pSubset->m_scTextureName[dwTexIndex];
			OldSubset.m_TextureType[dwTexIndex] = pSubset->m_scTextureType[dwTexIndex];
			OldSubset.m_TextureNumOption[dwTexIndex] = pSubset->m_pTextureNumOption[dwTexIndex];
			if (pSubset->m_lpTextures[dwTexIndex] != NULL)
			{
				OldSubset.m_Textures[dwTexIndex].m_bHasTexture = true;
				OldSubset.m_Textures[dwTexIndex].m_strNameReal = pSubset->m_lpTextures[dwTexIndex]->m_scNameReal;
			}
			else
			{
				OldSubset.m_Textures[dwTexIndex].m_bHasTexture = false;
			}
			if (pSubset->m_lpNormalMap != NULL)
			{
				OldSubset.m_NormalMap.m_bHasTexture = true;
				OldSubset.m_NormalMap.m_strNameReal = pSubset->m_lpNormalMap->m_scNameReal;
			}
			else
			{
				OldSubset.m_NormalMap.m_bHasTexture = false;
			}
			for (DWORD dwTexOptionIndex = 0; dwTexOptionIndex < OldSubset.m_TextureNumOption[dwTexIndex]; dwTexOptionIndex++)
			{
				pOption = pSubset->m_pTextureOptions[dwTexIndex][dwTexOptionIndex];
				OldSubset.m_pTextureOptions[dwTexIndex].push_back(pOption);
			}
		}
		OldSubset.m_pColorCast = pSubset->m_ColorCast;
		OldSubset.m_fSpecPower = pSubset->m_fSpecPower;
		OldSubset.m_fEmssPower = pSubset->m_fEmssPower;

		m_vecMTLSubset.push_back(OldSubset);
	}

	nResult = true;
Exit0:
	if (!nResult)
	{
		CleanVecSubset();
	}
	return nResult;
}

DWORD KGLoadController::GetNumMaterials()
{
	return m_dwNumMaterials;
}

int KGLoadController::GetSubset(OldMaterialSubset** ppOldMaterialSubset, const UINT cuIndex)
{
	int nResult = false;
	
	KG_ASSERT_EXIT(ppOldMaterialSubset);
	KG_ASSERT_EXIT(cuIndex < m_dwNumMaterials);

	*ppOldMaterialSubset = &m_vecMTLSubset[cuIndex];

	nResult = true;
Exit0:
	return nResult;
}

void KGLoadController::CleanVecSubset()
{
	OldMaterialSubset* pOldSubset = NULL;
	
	for (UINT uSubset = 0; uSubset < m_vecMTLSubset.size(); uSubset++)
	{
		pOldSubset = &m_vecMTLSubset[uSubset];
		for (UINT uTex = 0; uTex < OldMaterialSubset::cNumID; uTex++)
		{
			pOldSubset->m_pTextureOptions[uTex].clear();
		}
		pOldSubset->m_pMatOptions.clear();
	}
	m_vecMTLSubset.clear();	
}

int KGLoadController::FormatPath(char* pszPath, const size_t cuSize)
{
	int nResult  = false;
	int nRetCode = false;
	char* pRetCode = NULL;
	size_t uPathLength = 0;

	KG_ASSERT_EXIT(pszPath);
	pszPath[cuSize - 1] = '\0';

	pRetCode = strchr(pszPath, '/');
	KGLOG_PROCESS_ERROR(pRetCode == NULL);

	uPathLength = strnlen(pszPath, cuSize);
	KGLOG_PROCESS_ERROR(uPathLength > 0);
	KGLOG_PROCESS_ERROR(uPathLength <= cuSize - 1);
	KGLOG_PROCESS_ERROR(uPathLength <= _MAX_DIR - 1);

	if (pszPath[uPathLength - 1] != '\\')
	{
		pszPath[uPathLength] = '\\';
		pszPath[uPathLength + 1] = '\0';
	}

	nResult = true;
Exit0:
	return nResult;
}

int KGLoadController::GetBaseData( BaseMtlData** ppBaseMtlData )
{
	int nResult = false;

	KG_ASSERT_EXIT(ppBaseMtlData);
	
	*ppBaseMtlData = &m_BaseMtlData;

	nResult = true;
Exit0:
	return nResult;
}

int KGLoadController::Reset()
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hrRetCode = E_FAIL;

	hrRetCode = m_pMaterial->UnInit();
	KG_COM_PROCESS_ERROR(hrRetCode);
	hrRetCode = m_pEngineMgr->DestroyIEKG3DMaterial(m_pMaterial);
	KG_COM_PROCESS_ERROR(hrRetCode);

	CleanVecSubset();

	m_dwNumMaterials = 0;

	nResult = true;
Exit0:
	return nResult;

}