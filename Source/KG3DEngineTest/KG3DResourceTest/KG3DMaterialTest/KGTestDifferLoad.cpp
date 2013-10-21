#include "KGTestDifferLoad.h"


#ifdef _DEBUG
const char OLD_LOAD_DLL_NAME[] = "OldMaterialLoadD.dll";
#else
const char OLD_LOAD_DLL_NAME[] = "OldMaterialLoad.dll";
#endif

const char TEST_CASE_DIFFER_LOAD[] = "ValidateMtlByDifferLoadingFunc";

KGTestDifferLoad::KGTestDifferLoad()
{
	m_pOldLoadCtrl = NULL;
	m_hLoadCtrlDLL = NULL;
}

KGTestDifferLoad::~KGTestDifferLoad()
{
	UnInit();
}

int KGTestDifferLoad::Init(IKG3DEngineManager*		pEngineMgr,
						   IKG3DAutoTestInterface*	pTestInterface,
						   const char				cszClientPath[],
						   const char				cszResultPath[])
{
	int nResult  = false;
	int nRetCode = false;

	nRetCode = KGMaterialTestCase::Init(pEngineMgr, pTestInterface, cszClientPath, cszResultPath);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = InitResultLog(TEST_CASE_DIFFER_LOAD);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = LoadOldEngineDLL();
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;

}

void KGTestDifferLoad::UnInit()
{
	KGMaterialTestCase::UnInit();
	if (m_pOldLoadCtrl)
	{
		m_pOldLoadCtrl->UnInit();
		m_pOldLoadCtrl = NULL;
	}
	if (m_hLoadCtrlDLL)
	{
		FreeLibrary(m_hLoadCtrlDLL);
		m_hLoadCtrlDLL = NULL;
	}
}


void KGTestDifferLoad::OneMtlByDifferLoadingFunc(const char cszFileName[])
{
	int nResult  = false;
	int nRetCode = false;
	int nMtlLoaded = false;
	int nBaseData  = false;
	int nSubData   = false;
	IEKG3DMaterial* pMaterial = NULL;

	_ASSERTE(cszFileName);

	m_vecResult.push_back(string("MaterialFileName: ") + cszFileName);

	nRetCode = LoadMaterialByNewFunc(cszFileName, &pMaterial);
	if (!nRetCode)
	{
		m_vecResult.push_back(string("LoadMaterialByNewFunc Failed: ") + cszFileName);
		KGLOG_PROCESS_ERROR(false);
	}

	nRetCode = m_pOldLoadCtrl->LoadMaterialByOldFunc(cszFileName);
	if (!nRetCode)
	{
		m_vecResult.push_back(string("LoadMaterialByOldFunc Failed: ") + cszFileName);
		KGLOG_PROCESS_ERROR(false);
	}
	nMtlLoaded = true;


	//基本数据比对
	nBaseData = CompareDifferVersionMtlBaseData(pMaterial);
	//材质子集数据比对
	nSubData = CompareDifferVersionMtlSubset(pMaterial);

	nResult = true;
Exit0:
	if (!(nMtlLoaded && nBaseData && nSubData))
	{
		m_vecResult.push_back("Case Failed\n");
	}
	else
	{
		m_vecResult.clear();
	}
	ReleaseMtl(pMaterial);
	pMaterial = NULL;
	if (nMtlLoaded)
	{
		nRetCode = m_pOldLoadCtrl->Reset();
		if (!nRetCode)
		{
			KGLogPrintf(KGLOG_ERR, "OldLoadCtrl Reset Failed!");
		}
	}
}

int KGTestDifferLoad::CompareDifferVersionMtlBaseData(IEKG3DMaterial* pSourceMaterial)
{
	int nResult	 = false;
	int nRetCode = false;
	KG3DMaterial* pMaterial = NULL;
	IKGLoadController::BaseMtlData* pBaseMtlData = NULL;

	KG_ASSERT_EXIT(pSourceMaterial);

	nRetCode = m_pOldLoadCtrl->GetBaseData(&pBaseMtlData);
	KGLOG_PROCESS_ERROR(nRetCode);
	pMaterial = dynamic_cast<KG3DMaterial*>(pSourceMaterial);
	KGLOG_PROCESS_ERROR(nRetCode);

	if (pBaseMtlData->m_bHasDetail != pMaterial->m_bHasDetail)
	{
		m_vecResult.push_back("Different Data: m_bHasDetail");
	}
	if (pBaseMtlData->m_bSortAsSFX != pMaterial->m_bSortAsSFX)
	{
		m_vecResult.push_back("Different Data: m_bSortAsSFX");
	}
	if (pBaseMtlData->m_eLoadingState != pMaterial->m_eLoadingState)
	{
		m_vecResult.push_back("Different Data: m_eLoadingState");
	}
	KG_PROCESS_ERROR(m_vecResult.size() == 1);

	nResult = true;
Exit0:
	return nResult;
}

int KGTestDifferLoad::CompareDifferVersionMtlSubset(IEKG3DMaterial* pSourceMaterial)
{
	int nResult	 = false;
	int nRetCode = false;
	HRESULT hrRetCode = E_FAIL;
	DWORD dwSubsetCount = 0;
	IKGLoadController::OldMaterialSubset* pOldSubset = NULL;
	KG3DMaterial::KG3DMaterialSubset* pNewSubset = NULL;

	KG_ASSERT_EXIT(pSourceMaterial);

	pSourceMaterial->GetMaterialCount(&dwSubsetCount);
	if (m_pOldLoadCtrl->GetNumMaterials() != dwSubsetCount)
	{
		m_vecResult.push_back("Different Data: m_dwNumMaterials");
		KG_PROCESS_ERROR(false);
	}
	KGLOG_PROCESS_ERROR(dwSubsetCount > 0);
	for (DWORD dwMtlIndex = 0; dwMtlIndex < dwSubsetCount; dwMtlIndex++)
	{
		string strMtlsub = string("Testing MaterialSubset Index: ") +
						   KGTestCommon::IntToString(dwMtlIndex) + "\t";
		nRetCode = m_pOldLoadCtrl->GetSubset(&pOldSubset, dwMtlIndex);
		KGLOG_PROCESS_ERROR(nRetCode);
		hrRetCode = pSourceMaterial->GetSubMaterial(dwMtlIndex, (PVOID*)&pNewSubset);
		KGLOG_COM_PROCESS_ERROR(hrRetCode);

		if (pOldSubset->m_dwPassID != pNewSubset->m_dwPassID)
		{
			m_vecResult.push_back(strMtlsub + "Different Data: m_dwPassID");
		}
		if (pOldSubset->m_dwMaterialID != pNewSubset->m_dwMaterialID)
		{
			m_vecResult.push_back(strMtlsub + "Different Data: m_dwMaterialID");
		}

		nRetCode = KGTestCommon::D3DMATERIAL9Equal(pOldSubset->m_Material9, pNewSubset->m_sMaterial9);
		if (!nRetCode)
		{
			m_vecResult.push_back(strMtlsub + "Different Data: m_Material9");
		}
		if (pOldSubset->m_dwOption != pNewSubset->m_dwOption)
		{
			m_vecResult.push_back(strMtlsub + "Different Data: m_dwOption");
		}
		//材质选项数据比对
		if (pOldSubset->m_dwNumOptions != pNewSubset->m_dwNumOptions)
		{
			m_vecResult.push_back(strMtlsub + "Different Data: m_dwNumOptions");
		}
		else
		{
			for (DWORD dwMtlOption = 0; dwMtlOption < pOldSubset->m_dwNumOptions; dwMtlOption++)
			{
				nRetCode = CompareOption(pOldSubset->m_pMatOptions[dwMtlOption],
					pNewSubset->m_pMatOptions[dwMtlOption]);
				if (!nRetCode)
				{
					m_vecResult.push_back(strMtlsub + "Different Data: m_pMatOptions");
				}
			}
		}
		//贴图数据比对
		if (pOldSubset->m_dwNumUsedTexture != pNewSubset->m_dwNumUsedTexture)
		{
			m_vecResult.push_back(strMtlsub + "Different Data: m_dwNumUsedTexture");
		}
		else
		{
			for (DWORD dwTexIndex = 0; dwTexIndex < pOldSubset->m_dwNumUsedTexture; dwTexIndex++)
			{
				//基本数据比对
				nRetCode = strncmp(pOldSubset->m_TextureType[dwTexIndex].c_str(),
					pNewSubset->m_scTextureType[dwTexIndex].c_str(),
					MAT_TEXTURETYPE_LENGTH);
				if (nRetCode != 0)
				{
					m_vecResult.push_back(strMtlsub + 
										  string("Different Data: m_scTextureType Index:") +
										  KGTestCommon::IntToString(dwTexIndex));
				}
				nRetCode = strncmp(pOldSubset->m_TextureName[dwTexIndex].c_str(),
					pNewSubset->m_scTextureName[dwTexIndex].c_str(),
					MAT_TEXTUREFILENAME_LENGTH);
				if (nRetCode != 0)
				{
					m_vecResult.push_back(strMtlsub + 
										  string("Different Data: m_scTextureName Index:") +
										  KGTestCommon::IntToString(dwTexIndex));
				}
				//贴图实例数据比对
				if (pOldSubset->m_Textures[dwTexIndex].m_bHasTexture && pNewSubset->m_lpTextures[dwTexIndex])
				{
					nRetCode = strncmp(pOldSubset->m_Textures[dwTexIndex].m_strNameReal.c_str(),
						pNewSubset->m_lpTextures[dwTexIndex]->m_scNameReal.c_str(),
						MAT_TEXTURETYPE_LENGTH) ;
					if (nRetCode != 0)
					{
						m_vecResult.push_back(strMtlsub + 
											  string("Different Data: m_strNameReal Index:") +
											  KGTestCommon::IntToString(dwTexIndex));
					}	
				}
				if (pOldSubset->m_NormalMap.m_bHasTexture && pNewSubset->m_lpNormalMap)
				{
					nRetCode = strncmp(pOldSubset->m_NormalMap.m_strNameReal.c_str(),
						pNewSubset->m_lpNormalMap->m_scNameReal.c_str(),
						MAT_TEXTURETYPE_LENGTH);
					if (nRetCode != 0)
					{
						m_vecResult.push_back(strMtlsub + 
											  string("Different Data: m_strNameReal Index:") +
											  KGTestCommon::IntToString(dwTexIndex));
					}
				}
				//贴图选项数据比对
				if (pOldSubset->m_TextureNumOption[dwTexIndex] !=
					pNewSubset->m_pTextureNumOption[dwTexIndex])
				{
					m_vecResult.push_back(strMtlsub + 
										  string("Different Data: m_pTextureNumOption Index:") +
										  KGTestCommon::IntToString(dwTexIndex));
				}
				else
				{
					for (DWORD dwTexOption = 0; dwTexOption < pOldSubset->m_TextureNumOption[dwTexIndex]; dwTexOption++)
					{
						nRetCode = CompareOption(pOldSubset->m_pTextureOptions[dwTexIndex][dwTexOption],
							pNewSubset->m_pTextureOptions[dwTexIndex][dwTexOption]);
						if (!nRetCode)
						{
							m_vecResult.push_back(strMtlsub + 
												  string("Different Data: m_pTextureOptions TexIndex: ") +
												  KGTestCommon::IntToString(dwTexIndex) + 
												  string(" OptionIndex: ") +
												  KGTestCommon::IntToString(dwTexOption));
						}
					}
				}
			}
		}
		//其它颜色数据比对
		for (int nIndex = 0; nIndex < IKGLoadController::OldMaterialSubset::cNumColorCast; nIndex++)
		{
			nRetCode = KGTestCommon::D3DCOLORVALUEqual(pOldSubset->m_pColorCast[nIndex],
				pNewSubset->m_ColorCast[nIndex]);
			if (!nRetCode)
			{
				m_vecResult.push_back(string("Different Data: m_ColorCast Index:") +
									  KGTestCommon::IntToString(nIndex));
				break;
			}
		}
		if (!KGTestCommon::FloatEqual(pOldSubset->m_fSpecPower, pNewSubset->m_fSpecPower))
		{
			m_vecResult.push_back("Different Data: m_fSpecPower");
		}
		if (!KGTestCommon::FloatEqual(pOldSubset->m_fEmssPower, pNewSubset->m_fEmssPower))
		{
			m_vecResult.push_back("Different Data: m_fEmssPower");
		}
	}
	KG_PROCESS_ERROR(m_vecResult.size() == 1);

	nResult = true;
Exit0:
	return nResult;
}

void KGTestDifferLoad::RunTest(const char cszFileName[])
{	
	_ASSERTE(cszFileName);
	if (!m_MtlCanBeLoaded)
		return;
	OneMtlByDifferLoadingFunc(cszFileName);
	OuputResult();
}

int KGTestDifferLoad::LoadOldEngineDLL()
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hrRetCode = E_FAIL;
	int nLoadCtrlInitFlag = false;
	int nCtrlDllLoadFlag  = false;

	m_hLoadCtrlDLL = LoadLibrary(OLD_LOAD_DLL_NAME);
	KGLOG_PROCESS_ERROR(m_hLoadCtrlDLL);
	nCtrlDllLoadFlag = true;

	typedef HRESULT (*pfnGetLoadController)(void** m_pOldLoadCtrl);
	pfnGetLoadController GetLoadController;

	GetLoadController = (pfnGetLoadController)GetProcAddress(m_hLoadCtrlDLL, "GetLoadController");
	GetLoadController((void**)&m_pOldLoadCtrl);
	nRetCode = m_pOldLoadCtrl->Init(m_szClientPath);
	KGLOG_PROCESS_ERROR(nRetCode);
	nLoadCtrlInitFlag = true;

	nResult = true;
Exit0:
	if (!nResult)
	{
		if (nLoadCtrlInitFlag && m_pOldLoadCtrl)
		{
			m_pOldLoadCtrl->UnInit();
			m_pOldLoadCtrl = NULL;
		}
		if (nCtrlDllLoadFlag && m_hLoadCtrlDLL)
		{
			FreeLibrary(m_hLoadCtrlDLL);
			m_hLoadCtrlDLL = NULL;
		}
	}
	return nResult;
}