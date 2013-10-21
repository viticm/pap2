#include "KGTestSaveAndLoad.h"

const char TEST_CASE_SAVE_LOAD[] = "ValidateSaveMtlByNewLoadingFunc";

KGTestSaveAndLoad::KGTestSaveAndLoad()
{
	strncpy_s(m_szCaseName, sizeof(m_szCaseName), TEST_CASE_SAVE_LOAD, strlen(TEST_CASE_SAVE_LOAD));
}

KGTestSaveAndLoad::~KGTestSaveAndLoad()
{
}


int KGTestSaveAndLoad::Init(IKG3DEngineManager*		pEngineMgr,
							IKG3DAutoTestInterface* pTestInterface,
							const char				cszClientPath[],
							const char				cszResultPath[])
{
	int nResult	 = false;
	int nRetCode = false;

	nRetCode = KGMaterialTestCase::Init(pEngineMgr, pTestInterface, cszClientPath, cszResultPath);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = InitResultLog(m_szCaseName);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

void KGTestSaveAndLoad::UnInit()
{
	KGMaterialTestCase::UnInit();
}

int KGTestSaveAndLoad::CompareNewMtlData(IEKG3DMaterial* pSavedMaterial, IEKG3DMaterial* pSourceMaterial)
{
	int nResult	 = false;
	int nRetCode = false;
	HRESULT hrRetCode = E_FAIL;
	DWORD dwSavedSubsetCount  = 0;
	DWORD dwSourceSubsetCount = 0;
	KG3DMaterial::KG3DMaterialSubset* pSavedSubset  = NULL;
	KG3DMaterial::KG3DMaterialSubset* pSourceSubset = NULL;

	KG_ASSERT_EXIT(pSavedMaterial);
	KG_ASSERT_EXIT(pSourceMaterial);

	pSavedMaterial->GetMaterialCount(&dwSavedSubsetCount);
	pSourceMaterial->GetMaterialCount(&dwSourceSubsetCount);
	if (dwSavedSubsetCount != dwSourceSubsetCount)
	{
		m_vecResult.push_back("Different Data: m_dwNumMaterials");
		KG_PROCESS_ERROR(false);
	}
	KGLOG_PROCESS_ERROR(dwSourceSubsetCount > 0);
	for (DWORD dwMtlIndex = 0; dwMtlIndex < dwSavedSubsetCount; dwMtlIndex++)
	{
		string strMtlsub = string("Testing MaterialSubset Index: ") +
								  KGTestCommon::IntToString(dwMtlIndex) + "\t";
		hrRetCode = pSavedMaterial->GetSubMaterial(dwMtlIndex, (PVOID*)&pSavedSubset);
		KGLOG_COM_PROCESS_ERROR(hrRetCode);
		hrRetCode = pSourceMaterial->GetSubMaterial(dwMtlIndex, (PVOID*)&pSourceSubset);
		KGLOG_COM_PROCESS_ERROR(hrRetCode);

		if (pSavedSubset->m_dwPassID != pSourceSubset->m_dwPassID)
		{
			m_vecResult.push_back(strMtlsub + "Different Data: m_dwPassID");
		}
		if (pSavedSubset->m_dwMaterialID != pSourceSubset->m_dwMaterialID)
		{
			m_vecResult.push_back(strMtlsub + "Different Data: m_dwMaterialID");
		}

		nRetCode = KGTestCommon::D3DMATERIAL9Equal(pSavedSubset->m_sMaterial9, pSourceSubset->m_sMaterial9);
		if (!nRetCode)
		{
			m_vecResult.push_back(strMtlsub + "Different Data: m_Material9");
		}
		if (pSavedSubset->m_dwOption != pSourceSubset->m_dwOption)
		{
			m_vecResult.push_back(strMtlsub + "Different Data: m_dwOption");
		}
		//材质选项数据比对
		if (pSavedSubset->m_dwNumOptions != pSourceSubset->m_dwNumOptions)
		{
			m_vecResult.push_back(strMtlsub + "Different Data: m_dwNumOptions");
		}
		else
		{
			for (DWORD dwMtlOption = 0; dwMtlOption < pSavedSubset->m_dwNumOptions; dwMtlOption++)
			{
				nRetCode = CompareOption(pSavedSubset->m_pMatOptions[dwMtlOption],
					pSourceSubset->m_pMatOptions[dwMtlOption]);
				if (!nRetCode)
				{
					m_vecResult.push_back(strMtlsub + "Different Data: m_pMatOptions");
				}
			}
		}
		//贴图数据比对
		if (pSavedSubset->m_dwNumUsedTexture != pSourceSubset->m_dwNumUsedTexture)
		{
			m_vecResult.push_back(strMtlsub + "Different Data: m_dwNumUsedTexture");
		}
		else
		{
			for (DWORD dwTexIndex = 0; dwTexIndex < pSavedSubset->m_dwNumUsedTexture; dwTexIndex++)
			{
				//基本数据比对
				nRetCode = strncmp(pSavedSubset->m_scTextureType[dwTexIndex].c_str(),
								   pSourceSubset->m_scTextureType[dwTexIndex].c_str(),
								   MAT_TEXTURETYPE_LENGTH);
				if (nRetCode != 0)
				{
					m_vecResult.push_back(strMtlsub + 
										  string("Different Data: m_scTextureType Index:") +
										  KGTestCommon::IntToString(dwTexIndex));
				}
				nRetCode = strncmp(pSavedSubset->m_scTextureName[dwTexIndex].c_str(),
								   pSourceSubset->m_scTextureName[dwTexIndex].c_str(),
								   MAT_TEXTUREFILENAME_LENGTH);
				if (nRetCode != 0)
				{
					m_vecResult.push_back(strMtlsub + 
										  string("Different Data: m_scTextureName Index:") +
										  KGTestCommon::IntToString(dwTexIndex));
				}
				//贴图实例数据比对
				if (pSavedSubset->m_lpTextures[dwTexIndex] && pSourceSubset->m_lpTextures[dwTexIndex])
				{
					nRetCode = strncmp(pSavedSubset->m_lpTextures[dwTexIndex]->m_scNameReal.c_str(),
									   pSourceSubset->m_lpTextures[dwTexIndex]->m_scNameReal.c_str(),
									   MAT_TEXTURETYPE_LENGTH) ;
					if (nRetCode != 0)
					{
						m_vecResult.push_back(strMtlsub + 
											  string("Different Data: m_strNameReal Index:") +
											  KGTestCommon::IntToString(dwTexIndex));
					}
				}
				if (pSavedSubset->m_lpNormalMap && pSourceSubset->m_lpNormalMap)
				{
					nRetCode = strncmp(pSavedSubset->m_lpNormalMap->m_scNameReal.c_str(),
									   pSourceSubset->m_lpNormalMap->m_scNameReal.c_str(),
									   MAT_TEXTURETYPE_LENGTH);
					if (nRetCode != 0)
					{
						m_vecResult.push_back(strMtlsub + 
											  string("Different Data: m_strNameReal Index:") +
											  KGTestCommon::IntToString(dwTexIndex));
					}
				}
				//贴图选项数据比对
				if (pSavedSubset->m_pTextureNumOption[dwTexIndex] !=
					pSourceSubset->m_pTextureNumOption[dwTexIndex])
				{
					m_vecResult.push_back(strMtlsub +
										  string("Different Data: m_pTextureNumOption Index:") +
										  KGTestCommon::IntToString(dwTexIndex));
				}
				else
				{
					for (DWORD dwTexOption = 0; dwTexOption < pSavedSubset->m_pTextureNumOption[dwTexIndex]; dwTexOption++)
					{
						nRetCode = CompareOption(pSavedSubset->m_pTextureOptions[dwTexIndex][dwTexOption],
							pSourceSubset->m_pTextureOptions[dwTexIndex][dwTexOption]);
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
		for (int nIndex = 0; nIndex < KG3DMaterial::KG3DMaterialSubset::cNumColorCast; nIndex++)
		{
			nRetCode = KGTestCommon::D3DCOLORVALUEqual(pSavedSubset->m_ColorCast[nIndex],
				pSourceSubset->m_ColorCast[nIndex]);
			if (!nRetCode)
			{
				m_vecResult.push_back(string("Different Data: m_ColorCast Index:") +
					KGTestCommon::IntToString(nIndex));
				break;
			}
		}
		if (!KGTestCommon::FloatEqual(pSavedSubset->m_fSpecPower, pSourceSubset->m_fSpecPower))
		{
			m_vecResult.push_back("Different Data: m_fSpecPower");
		}
		if (!KGTestCommon::FloatEqual(pSavedSubset->m_fEmssPower, pSourceSubset->m_fEmssPower))
		{
			m_vecResult.push_back("Different Data: m_fEmssPower");
		}
	}
	KG_PROCESS_ERROR(m_vecResult.size() == 1);

	nResult = true;
Exit0:
	return nResult;
}

void KGTestSaveAndLoad::OneSaveMtlByNewLoadingFunc(const char cszFileName[])
{
	int nRetCode = false;
	int nCompareResult = false;
	int nPrepareData   = false;
	HRESULT hrRetCode = E_FAIL;
	char szNewFileName[MAX_PATH] = {0};
	IEKG3DMaterial* pSourceMaterial = NULL;
	IEKG3DMaterial* pSavedMaterial  = NULL;

	_ASSERTE(cszFileName);

	m_vecResult.push_back(string("MaterialFileName: ") + cszFileName);

	nRetCode = LoadMaterialByNewFunc(cszFileName, &pSourceMaterial);
	if (!nRetCode)
	{
		m_vecResult.push_back(string("LoadMaterialByNewFunc Failed(Source): ") + cszFileName);
		KGLOG_PROCESS_ERROR(false);
	}

	nRetCode = _snprintf_s(szNewFileName,
						   sizeof(szNewFileName),
						   sizeof(szNewFileName) - 1,
						   "%s.mtl",
						   cszFileName
				);
	KGLOG_PROCESS_ERROR(nRetCode != -1);

	nRetCode = _access(szNewFileName, 0);
	_ASSERTE(nRetCode == -1);
	KGLOG_PROCESS_ERROR(nRetCode == -1);

	hrRetCode = pSourceMaterial->SaveToFile(szNewFileName, 0);
	if (!nRetCode)
	{
		m_vecResult.push_back(string("SaveToFile Failed: ") + szNewFileName);
		KGLOG_PROCESS_ERROR(false);
	}

	nRetCode = LoadMaterialByNewFunc(szNewFileName, &pSavedMaterial);
	if (!nRetCode)
	{
		m_vecResult.push_back(string("LoadMaterialByNewFunc Failed(Saved): ") + szNewFileName);
		KGLOG_PROCESS_ERROR(false);
	}
	nPrepareData = true;

	nCompareResult = CompareNewMtlData(pSourceMaterial, pSavedMaterial);
	KGLOG_PROCESS_ERROR(nCompareResult);

Exit0:
	if (!(nPrepareData && nCompareResult))
	{
		m_vecResult.push_back("Failed\n");
	}
	else
	{
		m_vecResult.clear();
	}
	nRetCode = remove(szNewFileName);
	if (nRetCode == -1 && nPrepareData)
	{
		KGLogPrintf(KGLOG_ERR, "remove file faild: %s", szNewFileName);
	}
	ReleaseMtl(pSourceMaterial);
	pSourceMaterial = NULL;
	ReleaseMtl(pSavedMaterial);
	pSavedMaterial = NULL;
}

void KGTestSaveAndLoad::RunTest(const char cszFileName[])
{
	_ASSERTE(cszFileName);
	if (!m_MtlCanBeLoaded)
		return;
	OneSaveMtlByNewLoadingFunc(cszFileName);
	OuputResult();
}

int KGTestSaveAndLoad::SetCaseName(const char szCaseName[])
{
	int nResult	 = false;
	int nRetCode = false;

	nRetCode = strncpy_s(m_szCaseName, sizeof(m_szCaseName), szCaseName, strlen(szCaseName));
	KGLOG_PROCESS_ERROR(nRetCode == 0);

	nResult = true;
Exit0:
	return nResult;
}