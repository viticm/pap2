#include "StdAfx.h"
#include "kg3dmaterial.h"
#include "kg3dmodel.h"
#include "kg3dscenetable.h"
#include "kg3dtexturetable.h"
#include "kg3dmaterialtable.h"
#include "kg3denginemanager.h"
#include "KG3DRenderState.h"
#include "kg3dgraphicstool.h"
#include "KG3DGraphiceEngine.h"
#include "kg3dmaterialex.h"
#include "KG3DEffectFileTable.h"
#include "KG3DRenderGlobalState.h"
#include "MathTool.h"
#include "..\KG3DMultiThreadLoader.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern KG3DSceneTable  g_cSceneTable;
extern LPDIRECT3DDEVICE9 g_pd3dDevice;
extern KG3DMaterial::KG3DMaterialSubset* g_pSubMatLast;

float g_fHightLight = 0.25f;

static EffectParameterSettingTool* GetEffectParameterSettingTool()
{
	static EffectParameterSettingTool g_Tool;
	return &g_Tool; 
}

static TCHAR* g_strTextureType[] = {
	"*MAP_NORMAL",
	"*MAP_AMBIENT"};

	static KG3DMaterial::enuShaderType g_ShaderTypes[] = {
		KG3DMaterial::MAT_SHADER_TYPE_NORMAL_NOSKIN,
		KG3DMaterial::MAT_SHADER_TYPE_AMBIENT,
		KG3DMaterial::MAT_SHADER_TYPE_NONE
	};

	LPD3DXEFFECT EffectParameterSettingTool::m_pEffect;
	BOOL EffectParameterSettingTool::m_bMatrixSet[ENUM_MATRIX_COUNT];
	Semantic2TextureTool g_S2TTool;



	HRESULT KG3DMaterial::KG3DMaterialSubset::LoadEffectFromFile(const char* cszFilename)
	{
		HRESULT hr = E_FAIL;
		hr = g_cEffectFileTable.LoadFromFile(cszFilename, m_pEffect);
		/*if(FAILED(hr))
		{
		KGLogPrintf(KGLOG_ERR,"%s",cszFilename);
		}*/
		KG_COM_PROCESS_ERROR(hr);
		strcpy_s(strEffectFileName, MAX_PATH, cszFilename);
Exit0:
		return hr;
	}

	KG3DMaterial::KG3DMaterial(void)
	{
		m_dwID = 0;
		m_dwType = 0;
		m_ulRefCount = 0;
		m_pMaterials = NULL;
		m_dwNumMaterials = 0;
		m_pMaterialsMulti = NULL;
		m_dwNumMaterialsMulti = 0;
		m_nLastEffect = -1;
		m_ShaderType = MAT_SHADER_TYPE_NONE;
		m_bSortAsSFX = FALSE;
		m_bHasDetail = FALSE;
		m_eLoadingState = STATE_NONE;
		m_bInMultiThreadLoadList = FALSE;
		m_uOption = 0;
		bTest = FALSE;
	}

	KG3DMaterial::~KG3DMaterial(void)
	{
		ASSERT(!bTest);
		UnInit();
	}

	DWORD KG3DMaterial::KG3DMaterialSubset::ComputeHashValue()
	{
		if(m_lpTextures[0])
			m_dwMaterialID = m_lpTextures[0]->GetID();
		else
			m_dwMaterialID = 0;
		return m_dwMaterialID;
	}

	ULONG STDMETHODCALLTYPE KG3DMaterial::AddRef()
	{
		return (ULONG)KG_InterlockedIncrement((long *)&m_ulRefCount);
	}


	HRESULT KG3DMaterial::OnReferenceZero()
	{
		ASSERT(m_ulRefCount>=0);
		g_cMaterialTable.ReleaseResource(this, RELEASE_ATONCE); 
		return S_OK;
	}

	ULONG STDMETHODCALLTYPE KG3DMaterial::Release()
	{
		ULONG uNewRefCount = (ULONG)KG_InterlockedDecrement((long *)&m_ulRefCount);
		if (uNewRefCount == 0)
			OnReferenceZero(); 
		return uNewRefCount;
	}

	HRESULT STDMETHODCALLTYPE KG3DMaterial::QueryInterface(
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject)
	{
		return S_OK;
	}


	//////////////////////////////////////////////////////////////////////////
	//初始化,清除数据
	HRESULT KG3DMaterial::Init()
	{
		m_scName = "";
		m_pMaterials = NULL;

		return S_OK;
	}

	HRESULT KG3DMaterial::UnInit()
	{
		ClearEffectParaSets();
		if (m_pMaterials != NULL)
		{
			for (size_t i = 0; i < m_dwNumMaterials; i++)
			{
				if (g_pSubMatLast == (&(m_pMaterials[i])))
				{
					g_pSubMatLast = NULL;
				}
			}
		}
		SAFE_DELETE_ARRAY(m_pMaterialsMulti);
		m_dwNumMaterialsMulti = 0;
		SAFE_DELETE_ARRAY(m_pMaterials);
		m_dwNumMaterials = 0;
		m_nLastEffect = -1;
		m_ShaderType = MAT_SHADER_TYPE_NONE;
		m_eLoadingState = STATE_NONE;
		m_uOption = 0;
		return S_OK;
	}

	void KG3DMaterial::ClearEffectParaSets()
	{
		std::vector<_ShaderConstant*>* pCurParaSet = NULL;
		for(size_t i = 0; i < m_EffParaSets.size(); i++)
		{
			pCurParaSet = m_EffParaSets[i];
			for(std::vector<_ShaderConstant*>::iterator j = pCurParaSet->begin(); j != pCurParaSet->end(); j++)
			{
				delete *j;
			}
			pCurParaSet->clear();
			SAFE_DELETE(m_EffParaSets[i]);
		}

		m_EffParaSets.clear();
	}

	void KG3DMaterial::ModifyLoadOption(DWORD &dwTextureLoadOption, 
		DWORD dwSubsetID)
	{
		_MtlRenderTwiceAlphaOption *pRenderTwiceAlpha = NULL;
		KG_PROCESS_ERROR(dwSubsetID < m_dwNumMaterials);

		if (m_ShaderType == MAT_SHADER_TYPE_NORMAL_NOSKIN)
		{
			//带Normalmap的贴图， 要按照原图来加载
			dwTextureLoadOption |= (TEXOP_DISABLEDTX | TEXOP_DISABLESCALE);
		}

		pRenderTwiceAlpha = m_pMaterials[dwSubsetID].GetAlphaRenderTwice();
		KGLOG_PROCESS_ERROR(pRenderTwiceAlpha);

Exit0:
		;
	}

	HRESULT KG3DMaterial::ReloadTexture(DWORD dwOption)
	{
		HRESULT hResult  = E_FAIL;
		HRESULT hRetCode = E_FAIL;
		DWORD dwShaderType = MAT_SHADER_TYPE_NONE;
		DWORD dwTextureLoadOption = (dwOption & MLO_TEXTURE_MULTI) ? TEXOP_RELOAD : 0;
		for (DWORD i = 0; i < m_dwNumMaterials; i++)
		{
			KG3DMaterialSubset& SubMat = m_pMaterials[i];
			//GetShaderType
			for (DWORD j = 0; j < SubMat.m_dwNumUsedTexture; j++)
			{
				DWORD k = 0;
				DWORD dwSize = sizeof(g_strTextureType) / sizeof(TCHAR*);
				for (k = 0; k < dwSize; k++)
				{
					if (!strcmp(g_strTextureType[k], SubMat.m_scTextureType[j].c_str()))
						break;
				}
				if (k < dwSize && 
					dwShaderType < static_cast<DWORD>(g_ShaderTypes[k]))
					dwShaderType = g_ShaderTypes[k];
			}
		}

		SetShaderType(dwShaderType);
		for (DWORD i = 0; i < m_dwNumMaterials; i++)
		{
			KG3DMaterialSubset& SubMat = m_pMaterials[i];
			for (DWORD j = 0; j < SubMat.m_dwNumUsedTexture; j++)
			{
				KG3DTexture *pTexture = NULL;
				if (!strcmp(SubMat.m_scTextureName[j].c_str(), ""))
					continue;

				ModifyLoadOption(dwTextureLoadOption, 
					i);

				SAFE_RELEASE(SubMat.m_lpTextures[j]);

				if(j==1 && (dwShaderType == MAT_SHADER_TYPE_AMBIENT))
				{
					//跳过 
				}
				else
				{
					hResult = g_cTextureTable.LoadResourceFromFile(SubMat.m_scTextureName[j].c_str(), 
						0, 
						dwTextureLoadOption, 
						(IKG3DResourceBase**)&pTexture);
					if (SUCCEEDED(hResult))
					{
						SubMat.m_lpTextures[j] = pTexture;
					}
					else
					{
						SubMat.m_lpTextures[j] = NULL;
					}
				}


				if(j==0)
				{
					if((g_cEngineOption.bEnableSpeedTreeNormalMap && (m_uOption & MLO_MESH_SPEEDTREE)) || g_cEngineOption.bEnableNormalMap)
					{
						TCHAR strFile[MAX_PATH]; 
						TCHAR strDrive[MAX_PATH];
						TCHAR strPath[MAX_PATH];
						TCHAR strExt[MAX_PATH];
						_splitpath_s(SubMat.m_scTextureName[j].c_str(), 
							strDrive, 
							MAX_PATH,
							strPath, 
							MAX_PATH, 
							strFile, 
							MAX_PATH, 
							strExt,
							MAX_PATH);
						CHAR szNormalName[MAX_PATH];
						wsprintf(szNormalName,"%s%s%s_n%s",strDrive,strPath,strFile,strExt);
						SAFE_RELEASE(SubMat.m_lpNormalMap);
						g_cTextureTable.LoadResourceFromFile(szNormalName,0,TEXOP_RELOAD,(IKG3DResourceBase**)&SubMat.m_lpNormalMap );
					}
				}
			}


			SubMat.ComputeHashValue();
		}

		hRetCode = S_OK;
		return hRetCode;
	}

	//////////////////////////////////////////////////////////////////////////
	//加载,保存
	HRESULT KG3DMaterial::LoadFromFile(const TCHAR cszFileName[], 
		unsigned uFileNameHash, 
		unsigned uOption)
	{
		HRESULT hRetCode = E_FAIL;
		HRESULT hResult  = E_FAIL;

		m_scName = cszFileName;
		m_uOption = uOption;

		if((uOption & MLO_TEXTURE_MULTI) && (g_eEngineOption.bUseMultiThreadLoad))
		{
			g_cMultiThreadLoader.TryMultiThreadLoad_Material(this);
		}
		else
		{
			hResult = LoadFromFileDirect(cszFileName,uFileNameHash,uOption);
			KG_COM_PROCESS_ERROR(hResult);
		}
		hRetCode = S_OK;
Exit0:
		return hRetCode;
	}

	HRESULT KG3DMaterial::LoadFromFileMultiThread()
	{
		HRESULT hResult  = E_FAIL;

		hResult = LoadFromFileDirect(m_scName.c_str(),0,m_uOption);
		KG_COM_PROCESS_ERROR(hResult);

		m_eLoadingState = STATE_LOADCOMPLETE;

Exit0:
		//KGLogPrintf(KGLOG_INFO,"ReleaseDebug 0x%x KG3DModel::LoadFromFileMultiThread %s ",this, m_scName.c_str());

		return hResult;
	}

	HRESULT KG3DMaterial::LoadFromFileDirect(const TCHAR cszFileName[], 
		unsigned uFileNameHash, 
		unsigned uOption)
	{
		HRESULT hrResult  = E_FAIL;
		HRESULT hrRetCode = E_FAIL;
		DWORD dwNowTime = timeGetTime();

		DWORD dwMask = 0;//MATL对应的ASECC代码
		DWORD dwBlockEnd  = 0xFFFFFFFF;  
		DWORD dwBlockLength = 0;
		IFile *pFile = NULL;
		DWORD dwSize = 0;
		DWORD dwNumMaterialOption = 0;
		DWORD dwNumTexture = 0;

		float Amount = 0.0f;
		TCHAR strTextureType[32];
		TCHAR strTexFileName[MAX_PATH];

		m_scName = cszFileName;
		//Clear old parameters
		ClearEffectParaSets();
		if (m_pMaterials != NULL)
		{
			for (size_t i = 0; i < m_dwNumMaterials; i++)
				if (g_pSubMatLast == (&(m_pMaterials[i])))
					g_pSubMatLast = NULL;
		}

		SAFE_DELETE_ARRAY(m_pMaterials);
		m_dwNumMaterials = 0;

		pFile = g_OpenFile(cszFileName);
		KG_PROCESS_ERROR(pFile);

		dwSize = pFile->Read(&dwMask, sizeof(DWORD));
		KGLOG_PROCESS_ERROR(dwSize == sizeof(DWORD));
		KGLOG_PROCESS_ERROR(dwMask == 0x4D41544C);

		//Read block length
		dwSize = pFile->Read(&dwBlockLength, sizeof(DWORD));
		KGLOG_PROCESS_ERROR(dwSize == sizeof(DWORD));

		//read num materials
		dwSize = pFile->Read(&m_dwNumMaterials, sizeof(DWORD));
		KGLOG_PROCESS_ERROR(dwSize == sizeof(DWORD));
		KGLOG_PROCESS_ERROR(m_dwNumMaterials <= 128); // just for check file error, normally this value < 10

		m_pMaterials = new KG3DMaterialSubset[m_dwNumMaterials];
		KGLOG_PROCESS_ERROR(m_pMaterials);

		for (DWORD i = 0; i < m_dwNumMaterials; i++)
		{
			DWORD dwTextureStageIndex = 0;
			KG3DMaterialSubset* pSubMat = NULL;
			pSubMat = &m_pMaterials[i]; 
			dwSize = pFile->Read(&pSubMat->m_sMaterial9, sizeof(D3DMATERIAL9));
			KGLOG_PROCESS_ERROR(dwSize == sizeof(D3DMATERIAL9));
			Swap(pSubMat->m_sMaterial9.Ambient, pSubMat->m_sMaterial9.Diffuse); 

			NORMALIZE_MAT_POWER(pSubMat->m_sMaterial9.Power);

			dwSize = pFile->Read(&pSubMat->m_dwOption, sizeof(DWORD));
			KGLOG_PROCESS_ERROR(dwSize == sizeof(DWORD));
			pSubMat->m_dwOption |= MATERIAL_OPTION_FILL_SOLID;

			if(pSubMat->m_dwOption & MATERIAL_OPTION_SORTASSFX)
				m_bSortAsSFX = TRUE;

			if(pSubMat->m_dwOption & MATERIAL_OPTION_DETAIL)
				m_bHasDetail = TRUE;

			dwSize = pFile->Read(&dwNumMaterialOption, sizeof(DWORD));
			KGLOG_PROCESS_ERROR(dwSize == sizeof(DWORD));
			for (DWORD j = 0; j < dwNumMaterialOption; j++)
			{
				hrRetCode = pSubMat->LoadMaterialOption(pFile);
				if (hrRetCode == E_NOTIMPL)
				{
					//Effect
					LoadSavedEffect(pFile, pSubMat);
				}
			}

			_MtlRenderTwiceAlphaOption * pOption = pSubMat->GetAlphaRenderTwice();
			if (pOption->bEnable)
			{
				//特殊处理， 为了保证RenderTwiceAlpha的物体在半透明状态下的正常显示
				_MtlAlphaOperation *pAlphaOption = pSubMat->GetAlphaOperation();
				pAlphaOption->AlphaTestRef = 128;
				pAlphaOption->BackAlphaTestRef = 128;
				pAlphaOption->EnableAlphaTest = TRUE;
			}

			pSubMat->m_dwNumOptions = static_cast<DWORD>(pSubMat->m_pMatOptions.size());

			dwSize = pFile->Read(&dwNumTexture, sizeof(DWORD));
			KGLOG_PROCESS_ERROR(dwSize == sizeof(DWORD));

			pSubMat->m_dwNumUsedTexture = dwNumTexture;

			ZeroMemory(pSubMat->m_lpTextures,sizeof(KG3DTexture*)*8);

			for (DWORD dwTextIndex = 0; dwTextIndex < dwNumTexture; dwTextIndex++)
			{
				TCHAR strTest[MAX_PATH];
				TCHAR strRetName[MAX_PATH];
				TCHAR *pModify = NULL;

				dwSize = pFile->Read(&Amount, sizeof(float));
				KGLOG_PROCESS_ERROR(dwSize == sizeof(float));

				dwSize = pFile->Read(strTextureType, sizeof(TCHAR) * MAT_TEXTURETYPE_LENGTH);
				KGLOG_PROCESS_ERROR(dwSize == sizeof(TCHAR) * MAT_TEXTURETYPE_LENGTH);

				dwSize = pFile->Read(strTexFileName, sizeof(TCHAR) * MAT_TEXTUREFILENAME_LENGTH);
				KGLOG_PROCESS_ERROR(dwSize == sizeof(TCHAR) * MAT_TEXTUREFILENAME_LENGTH);

				if (Amount == 1.0f || Amount == 0.0f)
				{
					strcpy_s(strTest,
						MAX_PATH, 
						cszFileName);
					pModify = strrchr(strTest, int('\\'));
					if (pModify)
					{
						*(pModify +1) = '\0';
					}
					g_pFileFinder->GetAbsPathFromRelativePath(strTest, 
						strTexFileName, 
						strRetName);

					TCHAR strlowerDef[MAX_PATH];
					strcpy_s(strlowerDef, MAX_PATH, g_Def_WorkDirectory);
					_strlwr_s(strlowerDef, MAX_PATH);
					_strlwr_s(strRetName);
					size_t uSize = 0;
					if (strstr(strRetName, strlowerDef))
					{
						uSize = strlen(strlowerDef);
					}
					pModify = strRetName + uSize;
				}
				else
				{
					pModify = strTexFileName;
				}

				dwSize = pFile->Read(&pSubMat->m_pTextureNumOption[dwTextureStageIndex],
					sizeof(DWORD));
				KGLOG_PROCESS_ERROR(dwSize == sizeof(DWORD));

				for(DWORD j = 0; j < pSubMat->m_pTextureNumOption[dwTextureStageIndex]; j++)
				{
					hrRetCode = pSubMat->LoadTexturOption(pFile, dwTextureStageIndex);
				}

				if (!strcmp(strTextureType, "*MAP_OPACITY"))
				{
					pSubMat->ClearTextureStage(dwTextureStageIndex);
					continue;
				}
				pSubMat->m_scTextureType[dwTextIndex] = strTextureType;
				pSubMat->m_scTextureName[dwTextIndex] = pModify;

				pSubMat->m_lpTextures[dwTextIndex] = NULL;

				dwTextureStageIndex++;
			}

			if (!dwTextureStageIndex)
			{
				pSubMat->CleanMtlOptions();
			}


			pSubMat->m_dwNumUsedTexture = dwTextureStageIndex;

			if (pSubMat->m_dwOption & MATERIAL_OPTION_VERSION_2)
			{
				if (pSubMat->m_dwOption & MATERIAL_OPTION_INCLUDEALLDEFAULTCOLORCAST)
				{
					dwSize = pFile->Read(&pSubMat->m_ColorCast, sizeof(D3DCOLORVALUE)*(KG3DMaterialSubset::cNumColorCast));
					KGLOG_PROCESS_ERROR(dwSize == sizeof(D3DCOLORVALUE) * (KG3DMaterialSubset::cNumColorCast));
				}
				else
				{
					dwSize = pFile->Read(&pSubMat->m_ColorCast[0], sizeof(D3DCOLORVALUE));
					KGLOG_PROCESS_ERROR(dwSize == sizeof(D3DCOLORVALUE));
					pSubMat->m_dwOption |= MATERIAL_OPTION_INCLUDEALLDEFAULTCOLORCAST;
				}
				dwSize = pFile->Read(&pSubMat->m_fSpecPower, sizeof(float));
				KGLOG_PROCESS_ERROR(dwSize == sizeof(float));

				dwSize = pFile->Read(&pSubMat->m_fEmssPower, sizeof(float));
				KGLOG_PROCESS_ERROR(dwSize == sizeof(float));
			}

		}

		//If there is Effect Parameter Sets, load them 
		if(m_EffParaSets.size())
		{
			hrRetCode = LoadEffectParameter(pFile);
			KG_COM_PROCESS_ERROR(hrRetCode);
		}

		dwSize = pFile->Read(&dwBlockEnd, sizeof(DWORD));
		KGLOG_PROCESS_ERROR(dwSize == sizeof(DWORD));

		Helper_MoveEffectParameter2SubMaterial();

		m_scName = cszFileName;

		ReloadTexture(uOption);

		SetSubsetPassID();

		//////////////////////////////////////////////////////////////////////////
		if(m_bHasDetail)
		{
			TCHAR szName[MAX_PATH];
			wsprintf(szName,"%s.ini",cszFileName);

			LoadMaterialDetails(szName);
		}

		m_eLoadingState = STATE_LOADCOMPLETE; 
		hrResult = S_OK;
Exit0:
		KG_COM_RELEASE(pFile);

		if (FAILED(hrResult) && cszFileName)
		{
			KGLogPrintf(KGLOG_ERR, "Load Metrial Failed:%s", cszFileName);
		}

		DWORD dwCost = timeGetTime() - dwNowTime;
		if(dwCost >= 30)
		{
			//KGLogPrintf(KGLOG_ERR, "%d KG3DMaterial::LoadFromFile %s.",dwCost,cszFileName);
		}
		return hrResult;	
	}

	void KG3DMaterial::SetSubsetPassID()
	{
		int nVSVersion = 0;
		int nPSVersion = 0;
		for (DWORD i = 0; i < m_dwNumMaterials; i++)
		{
			switch (m_ShaderType)
			{
			case MAT_SHADER_TYPE_NONE:
				break;
			case MAT_SHADER_TYPE_AMBIENT:
				break;
			case MAT_SHADER_TYPE_SKIN:
				{
					KG3DMaterialSubset &SubMat = m_pMaterials[i];

					nVSVersion = D3DSHADER_VERSION_MAJOR(g_cGraphicsEngine.GetD3DCaps().VertexShaderVersion);
					nPSVersion = D3DSHADER_VERSION_MAJOR(g_cGraphicsEngine.GetD3DCaps().PixelShaderVersion);

					if (SubMat.m_lpTextures[0] == NULL || nVSVersion <= 1 || nPSVersion <= 1)
					{
						SubMat.m_dwPassID = PASS_SkinShader_old;
					}
					else if ((SubMat.m_dwOption & MATERIAL_OPTION_SPEC) && (SubMat.m_dwOption & MATERIAL_OPTION_EMAP)) 
					{
						SubMat.m_dwPassID = PASS_SkinShader_all;
					}
					else if (SubMat.m_dwOption & MATERIAL_OPTION_SPEC) 
					{
						SubMat.m_dwPassID = PASS_SkinShader_spec;
					}
					else if (SubMat.m_dwOption & MATERIAL_OPTION_EMAP) 
					{
						SubMat.m_dwPassID = PASS_SkinShader_emap;
					}
					else
					{
						SubMat.m_dwPassID = PASS_SkinShader_diff;
					}
				}
				break;
			case MAT_SHADER_TYPE_NORMAL_NOSKIN:
				break;
			}
		}
	}

	void KG3DMaterial::KG3DMaterialSubset::ClearTextureStage(DWORD dwStageIndex)
	{
		KG_PROCESS_ERROR(dwStageIndex < MAX_NUM_TEXTURESTAGE);
		m_pTextureOptions[dwStageIndex].clear();

		if (m_lpTextures[dwStageIndex])
		{
			SAFE_RELEASE(m_lpTextures[dwStageIndex]);
		}

		for(DWORD i = dwStageIndex + 1; i < MAX_NUM_TEXTURESTAGE; i++)
		{
			m_scTextureName[i - 1] = m_scTextureName[i];
			m_scTextureType[i - 1] = m_scTextureType[i];
			m_lpTextures[i - 1] = m_lpTextures[i];
			m_pTextureOptions[i - 1] = m_pTextureOptions[i];

			m_lpTextures[i] = NULL;
			m_pTextureOptions[i].clear();
			m_scTextureType[i] = "";
			m_scTextureName[i] = "";
		}
Exit0:
		return;
	}

	static const float s_CurrentTextureFileNameVersion = 2.0f;
	HRESULT KG3DMaterial::SaveToFile(const TCHAR cszFileName[],   unsigned uOption)
	{
		HRESULT hr = E_FAIL;
		DWORD dwMask = 0x4D41544C;//MATL对应的ASECC代码
		DWORD dwBlockEnd  = 0xFFFFFFFF;  
		DWORD dwBlockLength = 0;
		FILE* fpFile = NULL;
		TCHAR strPathBuffer[256];
		TCHAR strType[256];
		TCHAR strName[256];
		TCHAR strOutputInfo[MAX_PATH*2];
		KG3DMaterialSubset* pMaterial = NULL;
		_MtlOption* pOption = NULL;

		DWORD dwNumMtlOption = 0;
		_MtlOption EffectOption;
		DWORD dwNumTexture = 0;

		if(m_vecDetailDefine.size()>0)
			m_bHasDetail = TRUE;
		else
			m_bHasDetail = FALSE;

		strOutputInfo[0] = '\0';
		fpFile = fopen(cszFileName, "wb");
		KG_PROCESS_ERROR(fpFile);

		fwrite(&dwMask, sizeof(DWORD), 1, fpFile);
		//计算并写入快长度
		dwBlockLength += sizeof(DWORD) * 2;//子部分数目，结束标志

		for(DWORD dwSubIndex = 0; dwSubIndex < m_dwNumMaterials; dwSubIndex++)
		{
			dwBlockLength += sizeof(float) * 17
				+ sizeof(DWORD)//四种颜色，反光强度，帖图数目
				+ 8 * (sizeof(float) + sizeof(TCHAR) * MAX_PATH);//帖图信息 
		}

		fwrite(&dwBlockLength, sizeof(DWORD), 1, fpFile);
		fwrite(&m_dwNumMaterials, sizeof(DWORD), 1,fpFile);                  //子部分数目
		for(DWORD dwSubIndex=0; dwSubIndex < m_dwNumMaterials; dwSubIndex++)
		{
			pMaterial = &m_pMaterials[dwSubIndex]; 
			fwrite(&pMaterial->m_sMaterial9.Ambient, sizeof(D3DCOLORVALUE), 1, fpFile); 
			fwrite(&pMaterial->m_sMaterial9.Diffuse, sizeof(D3DCOLORVALUE), 1, fpFile); 
			fwrite(&pMaterial->m_sMaterial9.Specular, sizeof(D3DCOLORVALUE), 1,fpFile); 
			fwrite(&pMaterial->m_sMaterial9.Emissive, sizeof(D3DCOLORVALUE), 1, fpFile); 
			fwrite(&pMaterial->m_sMaterial9.Power, sizeof(float), 1, fpFile);
			dwNumMtlOption = pMaterial->m_dwNumOptions;
			if(pMaterial->m_pEffect)
			{
				dwNumMtlOption = pMaterial->m_dwNumOptions + 1;
			}	

			pMaterial->m_dwOption |= MATERIAL_OPTION_VERSION_2;
			pMaterial->m_dwOption |= MATERIAL_OPTION_INCLUDEALLDEFAULTCOLORCAST;
			if(m_bSortAsSFX)
				pMaterial->m_dwOption |= MATERIAL_OPTION_SORTASSFX;

			if(m_bHasDetail)
				pMaterial->m_dwOption |= MATERIAL_OPTION_DETAIL;

			fwrite(&pMaterial->m_dwOption,sizeof(DWORD),1,fpFile);//材质选项
			fwrite(&dwNumMtlOption,sizeof(DWORD),1,fpFile);//选项数目
			for(DWORD dwMOpIndex = 0; dwMOpIndex < pMaterial->m_dwNumOptions; dwMOpIndex++)
			{
				pOption = pMaterial->m_pMatOptions[dwMOpIndex];
				hr = SaveOption(pOption, fpFile);
				KGLOG_COM_PROCESS_ERROR(hr);
			}
			if(pMaterial->m_pEffect)
			{
				EffectOption.pData = pMaterial;
				EffectOption.Type = OPTION_MATERIAL_MSEFFECT;
				hr = SaveOption(&EffectOption, fpFile);
				KGLOG_COM_PROCESS_ERROR(hr);
				EffectOption.pData = NULL;
			}

			dwNumTexture = 0;
			for(DWORD dwTexIndex = 0;dwTexIndex < 8; dwTexIndex++)
			{
				if (!pMaterial->m_lpTextures[dwTexIndex])
					break;
				dwNumTexture++;
			}

			fwrite(&dwNumTexture, sizeof(DWORD), 1, fpFile);//帖图数目
			for(DWORD dwTextIndex = 0; dwTextIndex < dwNumTexture; dwTextIndex++)
			{
				fwrite(&s_CurrentTextureFileNameVersion, sizeof(float), 1, fpFile);
				wsprintf(strType, "%s", pMaterial->m_scTextureType[dwTextIndex].c_str());
				wsprintf(strName, "%s", pMaterial->m_scTextureName[dwTextIndex].c_str());
				//wsprintf(strPathBuffer, "%s", pMaterial->m_scTextureName[dwTextIndex].c_str());
				//hr = g_pFileFinder->GetRelativePath(cszFileName, (pMaterial->m_scTextureName[dwTextIndex]).c_str(), strFileName);
				g_pFileFinder->GetRelativePath(pMaterial->m_scTextureName[dwTextIndex].c_str(), strPathBuffer, 256);
				fwrite(strType, sizeof(TCHAR), 20, fpFile);
				//fwrite(strFileName, sizeof(TCHAR), 256, fpFile);
				fwrite(strPathBuffer, sizeof(TCHAR), 256, fpFile);

				pMaterial->m_pTextureNumOption[dwTextIndex] = 
					static_cast<DWORD>(pMaterial->m_pTextureOptions[dwTextIndex].size());
				fwrite(&pMaterial->m_pTextureNumOption[dwTextIndex], sizeof(DWORD), 1, fpFile);//帖图选项数目
				for(DWORD i_TOption=0;i_TOption<pMaterial->m_pTextureNumOption[dwTextIndex];i_TOption++)
				{
					pOption = pMaterial->m_pTextureOptions[dwTextIndex][i_TOption];
					hr = SaveOption(pOption,fpFile);
					KGLOG_COM_PROCESS_ERROR(hr);
				}
			}

			fwrite(&(pMaterial->m_ColorCast),  sizeof(D3DCOLORVALUE) * (KG3DMaterialSubset::cNumColorCast), 1, fpFile);
			fwrite(&(pMaterial->m_fSpecPower), sizeof(float), 1, fpFile);
			fwrite(&(pMaterial->m_fEmssPower), sizeof(float), 1, fpFile);
		}

		if(m_EffParaSets.size())
		{
			hr = SaveEffectParameter(fpFile);
			KGLOG_COM_PROCESS_ERROR(hr);
		}

		fwrite(&dwBlockEnd,sizeof(DWORD),1,fpFile);
		if(fpFile)
		{
			fclose(fpFile);
			fpFile = NULL;
		}

		//////////////////////////////////////////////////////////////////////////
		if(m_bHasDetail)
		{
			TCHAR szName[MAX_PATH];
			wsprintf(szName,"%s.ini",cszFileName);
			SaveMaterialDetails(szName);
		}
		hr = S_OK;

Exit0:
		if(strOutputInfo[0] != '\0')
		{
			KGLogPrintf(KGLOG_ERR, strOutputInfo);
		}
		if(fpFile)
		{
			fclose(fpFile);
			fpFile = NULL;
		}
		return hr;
	}

	//////////////////////////////////////////////////////////////////////////
	//ID,type
	DWORD KG3DMaterial::GetID()
	{
		return m_dwID;
	}

	unsigned KG3DMaterial::GetType()
	{
		return m_dwType;
	}

	//////////////////////////////////////////////////////////////////////////
	//内存管理
	HRESULT KG3DMaterial::PrepareRender(unsigned uOption) //压缩数据
	{
		return S_OK;
	}
	HRESULT KG3DMaterial::PostRender()//恢复数据，对象可以立即被使用
	{
		return S_OK;
	}
	HRESULT KG3DMaterial::GetDateInformation(int* pnMem,int* pnVedioMem,int* pnConMem,int* pnConVMem)//得到数据信息，参数依次为占用内存大小，现存大小，压缩后内存大小，压缩后内存大小
	{
		return S_OK;
	}
	unsigned   KG3DMaterial::GetLastUseTime()//查询上次使用的时间
	{
		return 0;
	}

	unsigned   KG3DMaterial::GetUseFrequency()//查询使用频率，一段时间里的使用次数
	{
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////
	HRESULT KG3DMaterial::RestoreMaterial(DWORD dwIndex)
	{
		HRESULT hr = E_FAIL;
		KG_PROCESS_ERROR(dwIndex < m_dwNumMaterials);
		g_GetRenderStateMgr().Restore();
		g_pd3dDevice->SetTexture(1, NULL);
		g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		//hr = m_pMaterials[dwIndex].RestoreMaterial();
		hr = S_OK;
Exit0:
		return hr;
	}

	HRESULT KG3DMaterial::KG3DMaterialSubset::RestoreMaterial()
	{
		g_GetRenderStateMgr().Restore();
		return S_OK;
	}

	HRESULT KG3DMaterial::KG3DMaterialSubset::RestoreBillboardCloudMaterial()
	{
		g_GetRenderStateMgr().Restore();
		return S_OK;
	}

	LPDIRECT3DTEXTURE9 KG3DMaterial::GetSpeedTreeLeafTexture(DWORD MaterialID)
	{
		KG3DMaterialSubset* pMaterial = NULL;
		//ASSERT(MaterialID >= m_dwNumMaterials);
		pMaterial = &m_pMaterials[MaterialID]; 

		if (pMaterial->m_lpTextures[0])
		{
			return pMaterial->m_lpTextures[0]->GetTexture();
		}

		return NULL;
	}

	DWORD KG3DMaterial::KG3DMaterialSubset::GetAlphaType()
	{
		void* pOption = NULL;

		pOption = FindOption(OPTION_MATERIAL_ALPHAOPERATION, 0);
		if(!pOption)
		{
			return 1;
		}
		else
		{
			_MtlOption* pOp = (_MtlOption*)pOption;
			_MtlAlphaOperation* pAlpha = (_MtlAlphaOperation*)pOp->pData;

			if(pAlpha->EnableAlphaBlend)
				return 4;
			else if (pAlpha->EnableAlphaTest)
			{
				return 2;
			}
		}
		return 1;
	}

	KG3DTexture* KG3DMaterial::KG3DMaterialSubset::GetSubMeshTexture()
	{
		KG3DTexture* pTexture = m_lpTextures[0];
		return pTexture;
	}

	KG3DTexture* KG3DMaterial::KG3DMaterialSubset::GetSubMeshTextureNormal()
	{
		return m_lpNormalMap;
	}


	HRESULT KG3DMaterial::KG3DMaterialSubset::SetMaterialEffect(float fAlpha,DWORD dwOption,int nShader,LPD3DXEFFECT pEffect)
	{
		return S_OK;
	}

	HRESULT KG3DMaterial::KG3DMaterialSubset::SetBillboardCloudMaterial()
	{    
		KG3DRenderState& RenderStateMgr = g_GetRenderStateMgr();
		D3DMATERIAL9 matRuntime;

		if (m_dwOption & MATERIAL_OPTION_HIDE)
			return S_FALSE;

		// cull
		RenderStateMgr.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		// alpha
		RenderStateMgr.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		RenderStateMgr.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		RenderStateMgr.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		RenderStateMgr.SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
		RenderStateMgr.SetRenderState(D3DRS_ALPHAREF, 0x80);
		RenderStateMgr.SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		RenderStateMgr.SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		RenderStateMgr.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		RenderStateMgr.SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);


		// z buffer
		if (m_dwOption & MATERIAL_OPTION_ZBUFFER_FALSE)
			RenderStateMgr.SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
		else if (m_dwOption & MATERIAL_OPTION_ZBUFFER_USEW)
			RenderStateMgr.SetRenderState(D3DRS_ZENABLE, D3DZB_USEW);
		else
			RenderStateMgr.SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);


		// light
		if (m_dwOption & MATERIAL_OPTION_LIGHTING)
			RenderStateMgr.SetRenderState(D3DRS_LIGHTING, TRUE);
		else
			RenderStateMgr.SetRenderState(D3DRS_LIGHTING, FALSE);

		// fill
		if (m_dwOption & MATERIAL_OPTION_FILL_SOLID)
			RenderStateMgr.SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		else if (m_dwOption & MATERIAL_OPTION_FILL_POINT)
			RenderStateMgr.SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);
		else if (m_dwOption & MATERIAL_OPTION_FILL_WIREFRAME)
			RenderStateMgr.SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

		// hight light
		if (m_dwOption & MATERIAL_OPTION_SPECULARENABLE)
		{
			RenderStateMgr.SetRenderState(D3DRS_SPECULARENABLE, FALSE);
			RenderStateMgr.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
			RenderStateMgr.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
		}
		else
		{
			RenderStateMgr.SetRenderState(D3DRS_SPECULARENABLE, FALSE);
			RenderStateMgr.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			RenderStateMgr.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
		}
		RenderStateMgr.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);


		// z write
		if (m_dwOption & MATERIAL_OPTION_ZWRITEDISABLE)
			RenderStateMgr.SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		else
			RenderStateMgr.SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

		memcpy(&matRuntime, &m_sMaterial9, sizeof(D3DMATERIAL9));

		matRuntime.Ambient.a = matRuntime.Ambient.r = matRuntime.Ambient.g = matRuntime.Ambient.b = 1;
		matRuntime.Diffuse = matRuntime.Ambient;
		matRuntime.Emissive =matRuntime.Ambient;
		g_pd3dDevice->SetMaterial(&matRuntime);

		return S_OK;
	}


	HRESULT KG3DMaterial::KG3DMaterialSubset::SetMaterial(float fAlpha, DWORD dwOption)
	{
		HRESULT hr = E_FAIL;
		_MtlOption* pOpt = NULL;
		DWORD dwTime = g_cGraphicsTool.GetNowTime() * 2;
		KG3DTexture* pTexture = NULL;
		KG3DRenderState& RenderStateMgr = g_GetRenderStateMgr();
		D3DMATERIAL9 matRuntime;

		if (dwOption == RENDER_SHADOW)
		{
			//颜色受环境光影响的影子
			D3DCOLORVALUE Ambient = g_GetRenderGlobalState().GetSunLightParam().sceneAmbient;

			Ambient.a = Ambient.a * m_sMaterial9.Ambient.a;
			Ambient.r = Ambient.r * m_sMaterial9.Ambient.r;
			Ambient.g = Ambient.g * m_sMaterial9.Ambient.g;
			Ambient.b = Ambient.b * m_sMaterial9.Ambient.b;

			DWORD dwAmbient = ColorValue(Ambient);

			RenderStateMgr.SetRenderState(D3DRS_TEXTUREFACTOR, dwAmbient);
			//这种方式就还好了

			if (m_lpTextures[0])
			{
				g_pd3dDevice->SetTexture(0, m_lpTextures[0]->GetTexture());
				RenderStateMgr.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
				RenderStateMgr.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			}
			hr = S_OK;
			goto Exit0;
		}

		if (m_dwOption & MATERIAL_OPTION_HIDE)
		{
			return S_FALSE;
		}



		//裁减信息
		if (m_dwOption & MATERIAL_OPTION_CULL_NONE)
		{
			hr = RenderStateMgr.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		}
		else if (m_dwOption & MATERIAL_OPTION_CULL_CCW)
		{
			hr = RenderStateMgr.SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		}
		else
		{
			hr = RenderStateMgr.SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
		}

		//Z buffer信息
		if (m_dwOption & MATERIAL_OPTION_ZBUFFER_FALSE)
		{
			hr = RenderStateMgr.SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
		}
		else if (m_dwOption & MATERIAL_OPTION_ZBUFFER_USEW)
		{
			hr = RenderStateMgr.SetRenderState(D3DRS_ZENABLE, D3DZB_USEW);
		}
		else
		{
			hr = RenderStateMgr.SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
		}

		//是否打开光照
		if (m_dwOption & MATERIAL_OPTION_LIGHTING)
		{
			hr = RenderStateMgr.SetRenderState(D3DRS_LIGHTING, TRUE);
		}
		else
		{
			hr = RenderStateMgr.SetRenderState(D3DRS_LIGHTING, FALSE);
		}

		//填充模式
		if (m_dwOption & MATERIAL_OPTION_FILL_SOLID)
		{
			hr = RenderStateMgr.SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		}	
		else if (m_dwOption & MATERIAL_OPTION_FILL_POINT)
		{
			hr = RenderStateMgr.SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);
		}
		else if (m_dwOption & MATERIAL_OPTION_FILL_WIREFRAME)
		{
			hr = RenderStateMgr.SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		}

		//高光高亮模式

		if (m_dwOption & MATERIAL_OPTION_SPECULARENABLE)
		{
			hr = RenderStateMgr.SetRenderState(D3DRS_SPECULARENABLE, FALSE);
			hr = RenderStateMgr.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
			hr = RenderStateMgr.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
		}
		else
		{
			hr = RenderStateMgr.SetRenderState(D3DRS_SPECULARENABLE, FALSE);
			hr = RenderStateMgr.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			hr = RenderStateMgr.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
		}


		//Z write

		if (m_dwOption & MATERIAL_OPTION_ZWRITEDISABLE)
		{
			hr = RenderStateMgr.SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		}
		else
		{
			hr = RenderStateMgr.SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		}

		_MtlAlphaOperation *pAlphaOp = GetAlphaOperation();
		float fTemp = static_cast<float>(pAlphaOp->BackAlphaTestRef) * fAlpha;
		pAlphaOp->AlphaTestRef = static_cast<DWORD>(fTemp);

		int nAddModeFlag = false;

		for(DWORD dwOptIndex = 0; dwOptIndex < m_dwNumOptions; dwOptIndex++)
		{
			pOpt = m_pMatOptions[dwOptIndex];
			hr = SetOption(0, pOpt);

			if (pOpt->Type == OPTION_MATERIAL_ALPHAOPERATION)
			{
				_MtlAlphaOperation* pMtlAlphaOp = reinterpret_cast<_MtlAlphaOperation*>(pOpt->pData);
				if (pMtlAlphaOp->BlendOperator == D3DBLENDOP_ADD &&
					pMtlAlphaOp->SrcBlend == D3DBLEND_SRCALPHA &&
					pMtlAlphaOp->DestBlend == D3DBLEND_ONE)
					nAddModeFlag = true;
			}
		}
		memcpy(&matRuntime, &m_sMaterial9, sizeof(D3DMATERIAL9));

		if (fAlpha < 1.0f && !nAddModeFlag)
		{
			hr = RenderStateMgr.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			hr = RenderStateMgr.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			hr = RenderStateMgr.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		}

		matRuntime.Diffuse.a = fAlpha;
		RenderStateMgr.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

		if (dwOption & RENDER_HIGHTLIGHT)
		{
			matRuntime.Diffuse.a = fAlpha;
			matRuntime.Emissive.a = 1.0f;
			matRuntime.Emissive.r = matRuntime.Emissive.g = matRuntime.Emissive.b = g_fHightLight;
		}

		hr = g_pd3dDevice->SetMaterial(&matRuntime);

		KG3DTexture *pNullTexture = g_cTextureTable.GetNullTexture();
		ASSERT(pNullTexture);

		for(int i = 0; i < 8; i++)
		{
			if (!m_lpTextures[i])
			{
				if (i == 0)
				{
					hr = g_pd3dDevice->SetTexture(i, pNullTexture->GetTexture());
				}
				else
				{
					hr = g_pd3dDevice->SetTexture(i, NULL);
				}
				hr = RenderStateMgr.SetTextureStageState(i, D3DTSS_COLOROP, D3DTOP_DISABLE);
				hr = RenderStateMgr.SetTextureStageState(i, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
				break;
			}

			pTexture = m_lpTextures[i];
			hr = pTexture->Update(1, dwTime);
			pTexture->PreLoad();

			hr = g_pd3dDevice->SetTexture(i, pTexture->GetTexture());

			hr = RenderStateMgr.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			hr = RenderStateMgr.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
			hr = RenderStateMgr.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			hr = RenderStateMgr.SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

			for(DWORD dwTOptionIndex = 0; dwTOptionIndex < m_pTextureNumOption[i]; dwTOptionIndex++)
			{
				pOpt = m_pTextureOptions[i][dwTOptionIndex];
				hr = SetOption(i, pOpt);
			}
		}
		hr = S_OK;
Exit0:
		return hr;
	}


	HRESULT KG3DMaterial::KG3DMaterialSubset::SetOption(DWORD dwTextureStage, _MtlOption* pOption)
	{
		HRESULT hr = E_FAIL;
		D3DXMATRIX Mat1;
		D3DXMATRIX Mat2;
		KG3DRenderState& RenderState = g_GetRenderStateMgr();

		KG_PROCESS_ERROR(dwTextureStage < 8 && pOption);

		switch(pOption->Type)
		{
		case OPTION_MATERIAL_ZFUN:
			{
				hr = RenderState.SetRenderState(D3DRS_ZFUNC, *(DWORD*)(pOption->pData));
				break;
			}
		case OPTION_MATERIAL_CULLING:
			{
				ASSERT(0);
				//_MatCull* pMatCull = reinterpret_cast<_MatCull*>(pOption->pData);
				//hr = RenderState.SetRenderState(D3DRS_CULLMODE, pMatCull->CullMode);
				break;
			}
		case OPTION_MATERIAL_ALPHABLEND:
			{
				ASSERT(0);
				//_MatBlend* pMatBlend = reinterpret_cast<_MatBlend*>(pOption->pData);

				//hr = hr = RenderState.SetRenderState(D3DRS_SRCBLEND, pMatBlend->SrcBlend);
				//hr = RenderState.SetRenderState(D3DRS_DESTBLEND, pMatBlend->DestBlend);
				//hr = RenderState.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				break;
			}
		case OPTION_MATERIAL_ALPHAOPERATION:
			{
				_MtlAlphaOperation* pMtlAlphaOp = reinterpret_cast<_MtlAlphaOperation*>(pOption->pData);
				hr = RenderState.SetRenderState(D3DRS_ALPHABLENDENABLE, pMtlAlphaOp->EnableAlphaBlend);
				hr = RenderState.SetRenderState(D3DRS_SRCBLEND, pMtlAlphaOp->SrcBlend);
				hr = RenderState.SetRenderState(D3DRS_DESTBLEND, pMtlAlphaOp->DestBlend);
				hr = RenderState.SetRenderState(D3DRS_BLENDOP, pMtlAlphaOp->BlendOperator);
				hr = RenderState.SetRenderState(D3DRS_BLENDFACTOR, pMtlAlphaOp->BlendFactor);
				hr = RenderState.SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, pMtlAlphaOp->EnableSeparateAlphaBlend);
				hr = RenderState.SetRenderState(D3DRS_BLENDOPALPHA, pMtlAlphaOp->BlendOperatorAlpha);
				hr = RenderState.SetRenderState(D3DRS_SRCBLENDALPHA, pMtlAlphaOp->SrcBlendAlpha);
				hr = RenderState.SetRenderState(D3DRS_DESTBLENDALPHA, pMtlAlphaOp->DestBlendAlpha);
				hr = RenderState.SetRenderState(D3DRS_ALPHATESTENABLE, pMtlAlphaOp->EnableAlphaTest);
				hr = RenderState.SetRenderState(D3DRS_ALPHAREF, pMtlAlphaOp->AlphaTestRef);
				hr = RenderState.SetRenderState(D3DRS_ALPHAFUNC, pMtlAlphaOp->AlphaFunction);
				break;
			}
		case OPTION_TEXTURE_OPERATION:
			{
				_TextureOp* pTextureOp = reinterpret_cast<_TextureOp*>(pOption->pData);
				hr = RenderState.SetTextureStageState(dwTextureStage, D3DTSS_COLOROP, pTextureOp->ColorOp);
				hr = RenderState.SetTextureStageState(dwTextureStage, D3DTSS_ALPHAOP, pTextureOp->AlphaOp);
				hr = RenderState.SetTextureStageState(dwTextureStage, D3DTSS_COLORARG1, pTextureOp->ColorArg1);
				hr = RenderState.SetTextureStageState(dwTextureStage, D3DTSS_COLORARG2, pTextureOp->ColorArg2);
				hr = RenderState.SetTextureStageState(dwTextureStage, D3DTSS_ALPHAARG1, pTextureOp->AlphaArg1);
				hr = RenderState.SetTextureStageState(dwTextureStage, D3DTSS_ALPHAARG2, pTextureOp->AlphaArg2);
				//hr = RenderState.SetRenderState(D3DRS_BLENDFACTOR,pTextureOp->TFactor);
				break;
			}
		case OPTION_TEXTURE_OPERATIONEX:
			{
				_MtlTextureOpEx* pTextureOpEx = reinterpret_cast<_MtlTextureOpEx*>(pOption->pData);
				KG_PROCESS_ERROR(pTextureOpEx);
				hr = RenderState.SetTextureStageState(dwTextureStage, D3DTSS_COLOROP, pTextureOpEx->ColorOp);
				hr = RenderState.SetTextureStageState(dwTextureStage, D3DTSS_ALPHAOP, pTextureOpEx->AlphaOp);
				hr = RenderState.SetTextureStageState(dwTextureStage, D3DTSS_COLORARG1, pTextureOpEx->ColorArg1);
				hr = RenderState.SetTextureStageState(dwTextureStage, D3DTSS_COLORARG2, pTextureOpEx->ColorArg2);
				hr = RenderState.SetTextureStageState(dwTextureStage, D3DTSS_ALPHAARG1, pTextureOpEx->AlphaArg1);
				hr = RenderState.SetTextureStageState(dwTextureStage, D3DTSS_ALPHAARG2, pTextureOpEx->AlphaArg2);
				hr = RenderState.SetTextureStageState(dwTextureStage, D3DTSS_ALPHAARG0, pTextureOpEx->AlphaArg0);
				hr = RenderState.SetTextureStageState(dwTextureStage, D3DTSS_COLORARG0, pTextureOpEx->ColorArg0);
				hr = RenderState.SetTextureStageState(dwTextureStage, D3DTSS_RESULTARG, pTextureOpEx->Result);
				hr = RenderState.SetTextureStageState(dwTextureStage, D3DTSS_CONSTANT , pTextureOpEx->Constant);
				//hr = RenderState.SetRenderState(D3DRS_BLENDFACTOR,pTextureOp->TFactor);
				break;
			}
		case OPTION_TEXTURE_MAPMETHOD:
			{
				_TextureMap* pMap = reinterpret_cast<_TextureMap*>(pOption->pData);
				hr = RenderState.SetTextureStageState(dwTextureStage,D3DTSS_TEXCOORDINDEX,pMap->TextureMap);
				break;
			}
		case OPTION_TEXTURE_TRANSFROM:
			{
				float fNowTimeInSecond = g_cGraphicsTool.GetNowTime() * 0.001f;
				_TextureTf* pTF = reinterpret_cast<_TextureTf*>(pOption->pData);

				hr = RenderState.SetTextureStageState(dwTextureStage,
					D3DTSS_TEXTURETRANSFORMFLAGS,
					D3DTTFF_COUNT3|D3DTTFF_PROJECTED
					);

				D3DXMatrixScaling(&Mat1, pTF->Scaling.x, pTF->Scaling.y, pTF->Scaling.z);
				D3DXMatrixRotationYawPitchRoll(&Mat2, 
					pTF->Rotation.x * fNowTimeInSecond, 
					pTF->Rotation.y * fNowTimeInSecond,
					pTF->Rotation.z*fNowTimeInSecond);
				D3DXMatrixMultiply(&Mat1, &Mat1, &Mat2);
				Mat1._41 = pTF->Position.x * fNowTimeInSecond;
				Mat1._42 = pTF->Position.y * fNowTimeInSecond;
				Mat1._43 = pTF->Position.z * fNowTimeInSecond;

				switch(dwTextureStage)
				{
				case 0:
					hr = g_pd3dDevice->SetTransform(D3DTS_TEXTURE0,&Mat1);
					break;
				case 1:
					hr = g_pd3dDevice->SetTransform(D3DTS_TEXTURE1,&Mat1);
					break;
				case 2:
					hr = g_pd3dDevice->SetTransform(D3DTS_TEXTURE2,&Mat1);
					break;
				case 3:
					g_pd3dDevice->SetTransform(D3DTS_TEXTURE3,&Mat1);
					break;
				}
				break;
			}
		}
		hr = S_OK;
Exit0:
		return hr;
	}

	HRESULT KG3DMaterial::SetMaterial(DWORD dwIndex, float fAlpha, DWORD dwOption, void** pExtInfo)
	{
		HRESULT hr = E_FAIL;
		HRESULT hrRetCode = E_FAIL;

		if (pExtInfo)
		{
			*pExtInfo = NULL;
		}

		KG_PROCESS_ERROR(dwIndex < m_dwNumMaterials);
		hr = m_pMaterials[dwIndex].SetMaterial(fAlpha, dwOption);
		KG_COM_PROCESS_ERROR(hr);

		KG_PROCESS_SUCCESS(!m_pMaterials[dwIndex].m_pEffect);

		if (!(dwOption & RENDER_SHADOW))
		{
			Effect_SetParameter(dwIndex);
			if (pExtInfo)
			{
				*pExtInfo = (void*)m_pMaterials[dwIndex].m_pEffect;
			}
		}
Exit1:
		hrRetCode = S_OK;
Exit0:
		return hrRetCode;
	}

	//HRESULT KG3DMaterial::SetOption(DWORD dwTextureStage, _MtlOption* pOption)
	//{
	//	HRESULT hr = S_OK;
	//	_MatCull* pMatCull = NULL;
	//	_MatBlend* pMatBlend = NULL;
	//	_MtlAlphaOperation* pMtlAlphaOp = NULL;
	//	_TextureOp* pTextureOp = NULL;
	//	_TextureOpEx* pTextureOpEx = NULL;
	//	_TextureMap* pMap = NULL;
	//	float fNowTimeInSecond = g_cGraphicsTool.GetNowTime() * 0.001f;
	//	_TextureTf* pTF = NULL;
	//	KG3DRenderState& RenderState = g_GetRenderStateMgr();
	//	D3DXMATRIX Mat1;
	//	D3DXMATRIX Mat2;
	//
	//	switch(pOption->Type)
	//	{
	//	case OPTION_MATERIAL_CULLING:
	//		{
	//			pMatCull = reinterpret_cast<_MatCull*>(pOption->pData);
	//			hr = RenderState.SetRenderState(D3DRS_CULLMODE, pMatCull->CullMode);
	//			break;
	//		}
	//	case OPTION_MATERIAL_ALPHABLEND:
	//		{
	//			pMatBlend = reinterpret_cast<_MatBlend*>(pOption->pData);
	//			hr = RenderState.SetRenderState(D3DRS_SRCBLEND, pMatBlend->SrcBlend);
	//			hr = RenderState.SetRenderState(D3DRS_DESTBLEND, pMatBlend->DestBlend);
	//			hr = RenderState.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	//			break;
	//		}
	//	case OPTION_MATERIAL_ALPHAOPERATION:
	//		{
	//			pMtlAlphaOp = reinterpret_cast<_MtlAlphaOperation*>(pOption->pData);
	//			
	//			hr = RenderState.SetRenderState(D3DRS_ALPHABLENDENABLE, pMtlAlphaOp->EnableAlphaBlend);
	//			hr = RenderState.SetRenderState(D3DRS_SRCBLEND, pMtlAlphaOp->SrcBlend);
	//			hr = RenderState.SetRenderState(D3DRS_DESTBLEND, pMtlAlphaOp->DestBlend);
	//			hr = RenderState.SetRenderState(D3DRS_BLENDOP, pMtlAlphaOp->BlendOperator);
	//			hr = RenderState.SetRenderState(D3DRS_BLENDFACTOR, pMtlAlphaOp->BlendFactor);
	//
	//			hr = RenderState.SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, pMtlAlphaOp->EnableSeparateAlphaBlend);
	//			hr = RenderState.SetRenderState(D3DRS_BLENDOPALPHA, pMtlAlphaOp->BlendOperatorAlpha);
	//			hr = RenderState.SetRenderState(D3DRS_SRCBLENDALPHA, pMtlAlphaOp->SrcBlendAlpha);
	//			hr = RenderState.SetRenderState(D3DRS_DESTBLENDALPHA, pMtlAlphaOp->DestBlendAlpha);
	//
	//			hr = RenderState.SetRenderState(D3DRS_ALPHATESTENABLE, pMtlAlphaOp->EnableAlphaTest);
	//			hr = RenderState.SetRenderState(D3DRS_ALPHAREF, pMtlAlphaOp->AlphaTestRef);
	//			hr = RenderState.SetRenderState(D3DRS_ALPHAFUNC, pMtlAlphaOp->AlphaFunction);
	//			break;
	//		}
	//	case OPTION_TEXTURE_OPERATION:
	//		{
	//			pTextureOp = reinterpret_cast<_TextureOp*>(pOption->pData);
	//			hr = RenderState.SetTextureStageState(dwTextureStage,D3DTSS_COLOROP,pTextureOp->ColorOp);
	//			hr = RenderState.SetTextureStageState(dwTextureStage,D3DTSS_ALPHAOP,pTextureOp->AlphaOp);
	//			hr = RenderState.SetTextureStageState(dwTextureStage,D3DTSS_COLORARG1,pTextureOp->ColorArg1);
	//			hr = RenderState.SetTextureStageState(dwTextureStage,D3DTSS_COLORARG2,pTextureOp->ColorArg2);
	//			hr = RenderState.SetTextureStageState(dwTextureStage,D3DTSS_ALPHAARG1,pTextureOp->AlphaArg1);
	//			hr = RenderState.SetTextureStageState(dwTextureStage,D3DTSS_ALPHAARG2,pTextureOp->AlphaArg2);
	//			//hr = RenderState.SetRenderState(D3DRS_BLENDFACTOR,pTextureOp->TFactor);
	//			break;
	//		}
	//	case OPTION_TEXTURE_OPERATIONEX:
	//		{
	//			pTextureOpEx = reinterpret_cast<_TextureOpEx*>(pOption->pData);
	//			
	//			hr = RenderState.SetTextureStageState(dwTextureStage,D3DTSS_COLOROP,pTextureOpEx->ColorOp);
	//			hr = RenderState.SetTextureStageState(dwTextureStage,D3DTSS_ALPHAOP,pTextureOpEx->AlphaOp);
	//			hr = RenderState.SetTextureStageState(dwTextureStage,D3DTSS_COLORARG1,pTextureOpEx->ColorArg1);
	//			hr = RenderState.SetTextureStageState(dwTextureStage,D3DTSS_COLORARG2,pTextureOpEx->ColorArg2);
	//			hr = RenderState.SetTextureStageState(dwTextureStage,D3DTSS_ALPHAARG1,pTextureOpEx->AlphaArg1);
	//			hr = RenderState.SetTextureStageState(dwTextureStage,D3DTSS_ALPHAARG2,pTextureOpEx->AlphaArg2);
	//			hr = RenderState.SetTextureStageState(dwTextureStage,D3DTSS_ALPHAARG0,pTextureOpEx->AlphaArg0);
	//			hr = RenderState.SetTextureStageState(dwTextureStage,D3DTSS_COLORARG0,pTextureOpEx->ColorArg0);
	//			hr = RenderState.SetTextureStageState(dwTextureStage,D3DTSS_RESULTARG,pTextureOpEx->Result);
	//			hr = RenderState.SetTextureStageState(dwTextureStage,D3DTSS_CONSTANT ,pTextureOpEx->Constant);
	//			//hr = RenderState.SetRenderState(D3DRS_BLENDFACTOR,pTextureOp->TFactor);
	//			break;
	//		}
	//	case OPTION_TEXTURE_MAPMETHOD:
	//		{
	//			pMap = reinterpret_cast<_TextureMap*>(pOption->pData);
	//			hr = RenderState.SetTextureStageState(dwTextureStage, D3DTSS_TEXCOORDINDEX, pMap->TextureMap);
	//			break;
	//		}
	//	case OPTION_TEXTURE_TRANSFROM:
	//		{
	//			pTF = reinterpret_cast<_TextureTf*>(pOption->pData);
	//			hr = RenderState.SetTextureStageState(dwTextureStage,
	//											   D3DTSS_TEXTURETRANSFORMFLAGS,
	//											   D3DTTFF_COUNT3|D3DTTFF_PROJECTED
	//											   );
	//			D3DXMatrixScaling(&Mat1, pTF->Scaling.x, pTF->Scaling.y, pTF->Scaling.z);
	//			D3DXMatrixRotationYawPitchRoll(&Mat2, 
	//				                           pTF->Rotation.x * fNowTimeInSecond, 
	//										   pTF->Rotation.y * fNowTimeInSecond,
	//										   pTF->Rotation.z*fNowTimeInSecond
	//										   );
	//			D3DXMatrixMultiply(&Mat1, &Mat1, &Mat2);
	//			Mat1._41 = pTF->Position.x * fNowTimeInSecond;
	//			Mat1._42 = pTF->Position.y * fNowTimeInSecond;
	//			Mat1._43 = pTF->Position.z * fNowTimeInSecond;
	//			switch(dwTextureStage)
	//			{
	//			case 0:
	//				hr = g_pd3dDevice->SetTransform(D3DTS_TEXTURE0,&Mat1);
	//				break;
	//			case 1:
	//				hr = g_pd3dDevice->SetTransform(D3DTS_TEXTURE1,&Mat1);
	//				break;
	//			case 2:
	//				hr = g_pd3dDevice->SetTransform(D3DTS_TEXTURE2,&Mat1);
	//				break;
	//			case 3:
	//				hr = g_pd3dDevice->SetTransform(D3DTS_TEXTURE3,&Mat1);
	//				break;
	//			}
	//			break;
	//		}
	//	}
	//	return hr;
	//}


	HRESULT KG3DMaterial::SetMaterial(DWORD AmbientColor, DWORD MaterialID, float fAlpha)
	{
		return S_OK;
	}

	void KG3DMaterial::Swap(D3DCOLORVALUE &color1, D3DCOLORVALUE &color2)
	{
		D3DCOLORVALUE temp = color1;
		color1 = color2;
		color2 = temp;
	}

	HRESULT KG3DMaterial::KG3DMaterialSubset::LoadMaterialOption(IFile* pFile)
	{
		HRESULT hr = E_FAIL;
		DWORD dwSize = 0;
		DWORD dwType = 0;
		KG_PROCESS_ERROR(pFile);

		dwSize = pFile->Read(&dwType, sizeof(DWORD));
		KGLOG_PROCESS_ERROR(dwSize == sizeof(DWORD));

		switch(dwType)
		{
		case OPTION_MATERIAL_ZFUN:
			{
				_MtlOption* pOption = static_cast<_MtlOption*>(GetMtlOption(dwType, 0xffffffff));
				KG_PROCESS_ERROR(pOption);
				dwSize = pFile->Read(pOption->pData, sizeof(DWORD));
				KG_PROCESS_ERROR(dwSize == sizeof(DWORD));
				break;
			}
		case OPTION_MATERIAL_CULLING:
			{
				_MatCull CullOption;

				dwSize = pFile->Read(&CullOption, sizeof(_MatCull));
				KG_PROCESS_ERROR(dwSize == sizeof(_MatCull));

				//pOption->pData = pMatCull;
				//move data into material option
				m_dwOption &= ~MATERIAL_OPTION_CULL_CCW;
				m_dwOption &= ~MATERIAL_OPTION_CULL_CW;
				m_dwOption &= ~MATERIAL_OPTION_CULL_NONE;
				switch(CullOption.CullMode)
				{
				case D3DCULL_CCW:
					{
						m_dwOption |= MATERIAL_OPTION_CULL_CCW;
						break;
					}
				case D3DCULL_CW:
					{
						m_dwOption |= MATERIAL_OPTION_CULL_CW;
						break;
					}
				case D3DCULL_NONE:
					{
						m_dwOption |= MATERIAL_OPTION_CULL_NONE;
						break;
					}
				}
				break;
			}
		case OPTION_MATERIAL_ALPHABLEND:
			{
				_MatBlend BlendOption;;
				_MtlAlphaOperation* pMtlAlphaOp = GetAlphaOperation();
				KGLOG_PROCESS_ERROR(pMtlAlphaOp);

				dwSize = pFile->Read(&BlendOption, sizeof(_MatBlend));
				KG_PROCESS_ERROR(dwSize == sizeof(_MatBlend));

				pMtlAlphaOp->SrcBlend = BlendOption.SrcBlend;
				pMtlAlphaOp->DestBlend = BlendOption.DestBlend;

				pMtlAlphaOp->EnableAlphaBlend = TRUE;
				break;
			}
		case OPTION_MATERIAL_ALPHAOPERATION:
			{
				_MtlAlphaOperation* pAlphaOption = GetAlphaOperation();
				KG_PROCESS_ERROR(pAlphaOption);

				dwSize = pFile->Read(pAlphaOption, sizeof(_MtlAlphaOperation));
				KG_PROCESS_ERROR(dwSize == sizeof(_MtlAlphaOperation));
				pAlphaOption->BackAlphaTestRef = pAlphaOption->AlphaTestRef;
				break;
			}
		case OPTION_MATERIAL_ALPHARENDERTWICE:
			{
				_MtlRenderTwiceAlphaOption *pAlphaOption = GetAlphaRenderTwice();
				KG_PROCESS_ERROR(pAlphaOption);

				dwSize = pFile->Read(pAlphaOption, sizeof(_MtlRenderTwiceAlphaOption));
				KG_PROCESS_ERROR(dwSize == sizeof(_MtlRenderTwiceAlphaOption));
				break;
			}
		case OPTION_MATERIAL_MSEFFECT:
			{
				//hr = LoadSavedEffect(pFile, this);
				//pFile->Seek(MAX_PATH * sizeof(TCHAR) + sizeof(int) + sizeof(int), SEEK_CUR);
				//KG_COM_PROCESS_ERROR(hr);
				hr = E_NOTIMPL;
				KG_COM_PROCESS_ERROR(hr);
				break;
			}
		}

		hr = S_OK;
Exit0:
		return hr;
	}

	HRESULT KG3DMaterial::KG3DMaterialSubset::LoadTexturOption(IFile* pFile, 
		DWORD dwTextureStageIndex)
	{
		HRESULT hr = E_FAIL;
		DWORD dwSize = 0;
		void* pDataReadIn = NULL;
		_MtlOption* pNewOption = NULL;
		KG_PROCESS_ERROR(pFile);
		KG_PROCESS_ERROR(dwTextureStageIndex < 8);

		pNewOption = new _MtlOption;
		KG_PROCESS_ERROR(pNewOption);

		dwSize = pFile->Read(&(pNewOption->Type), sizeof(DWORD));
		KG_PROCESS_ERROR(dwSize == sizeof(DWORD));

		ASSERT(pNewOption->Type != OPTION_MATERIAL_MSEFFECT);

		switch(pNewOption->Type)
		{
		case OPTION_TEXTURE_OPERATION:
			{
				pDataReadIn = new _MtlTextureOp;
				KGLOG_PROCESS_ERROR(pDataReadIn);

				dwSize = pFile->Read(pDataReadIn, sizeof(_MtlTextureOp));
				KG_PROCESS_ERROR(dwSize == sizeof(_MtlTextureOp));

				pNewOption->pData = pDataReadIn;
				break;
			}
		case OPTION_TEXTURE_OPERATIONEX:
			{
				pDataReadIn = new _TextureOpEx;
				KG_PROCESS_ERROR(pDataReadIn);

				dwSize = pFile->Read(pDataReadIn, sizeof(_TextureOpEx));
				KG_PROCESS_ERROR(dwSize == sizeof(_TextureOpEx));

				pNewOption->pData = pDataReadIn;
				break;
			}
		case OPTION_TEXTURE_MAPMETHOD:
			{
				pDataReadIn = new _TextureMap;
				KGLOG_PROCESS_ERROR(pDataReadIn);

				dwSize = pFile->Read(pDataReadIn, sizeof(_TextureMap));
				KG_PROCESS_ERROR(dwSize == sizeof(_TextureMap));

				pNewOption->pData = pDataReadIn;
				break;
			}
		case OPTION_TEXTURE_TRANSFROM:
			{
				pDataReadIn = new _TextureTf;
				KGLOG_PROCESS_ERROR(pDataReadIn);

				dwSize = pFile->Read(pDataReadIn, sizeof(_TextureTf));
				KG_PROCESS_ERROR(dwSize == sizeof(_TextureTf));

				pNewOption->pData = pDataReadIn;
				break;
			}
		}

		m_pTextureOptions[dwTextureStageIndex].push_back(pNewOption);
		hr = S_OK;
Exit0:
		if (FAILED(hr))
		{
			SAFE_DELETE(pDataReadIn);
			SAFE_DELETE(pNewOption);
		}
		return hr;
	}


	HRESULT KG3DMaterial::LoadSavedEffect(IFile* pFile, KG3DMaterialSubset* pMtl)
	{
		HRESULT hr = E_FAIL;
		DWORD i = 0;
		int nTechnique = 0;
		int nEffParaIndex = 0;
		TCHAR strPath[MAX_PATH];
		TCHAR strFileName[MAX_PATH];
		TCHAR strOutputInfo[MAX_PATH << 1];
		TCHAR strFixedFileName[MAX_PATH];
		DWORD dwSize = 0;

		int nNumEffParaSets = static_cast<int>(m_EffParaSets.size());

		strPath[0] = '\0';
		strFileName[0] = '\0';
		strFixedFileName[0] = '\0';

		dwSize = pFile->Read(strFileName, MAX_PATH * sizeof(TCHAR));
		KGLOG_PROCESS_ERROR(dwSize == MAX_PATH * sizeof(TCHAR));

		dwSize = pFile->Read(&nTechnique, sizeof(int));
		KGLOG_PROCESS_ERROR(dwSize == sizeof(int));

		dwSize = pFile->Read(&nEffParaIndex, sizeof(int));
		KGLOG_PROCESS_ERROR(dwSize == sizeof(int));

		//return S_OK;
		if (nNumEffParaSets > nEffParaIndex && 
			m_EffParaSets[nEffParaIndex]->size())
		{

			//If another subset using the same ParaSet has already initialized
			//the ParaSet table, we only need to find subset id
			for(i = 0; i < m_dwNumMaterials; i++)
			{
				if(m_pMaterials[i].m_nEffParaIndex == nEffParaIndex)
					break;
			}

			if(i < m_dwNumMaterials)
			{
				pMtl->m_nCurTechnique = nTechnique;
				pMtl->m_nEffParaIndex = nEffParaIndex;
				pMtl->m_pEffect = m_pMaterials[i].m_pEffect;
				pMtl->m_pEffect->AddRef();
			}
			else
			{
				ASSERT(0);
			}
		}
		else
		{
			return S_OK;//不知道干什么用的，先关掉这部分功能

			//The paraset needs to be initialized
			hr = g_pFileFinder->GetAbsPath(m_scName.c_str(), strPath);
			hr = g_pFileFinder->GetAbsPathFromRelativePath(strPath, strFileName, strFixedFileName);
			hr = pMtl->LoadEffectFromFile(strFixedFileName);
			if(FAILED(hr))
			{
				//Temporarily used to fix wrong relative path saved
				g_pFileFinder->strmov(strFileName, 3, 3);
				hr = g_pFileFinder->GetAbsPathFromRelativePath(strPath, strFileName, strFixedFileName);
				hr = pMtl->LoadEffectFromFile(strFixedFileName);
				if (FAILED(hr))
				{
					sprintf(strOutputInfo, "Load FX file: %s failed.", strFixedFileName);
					KGLogPrintf(KGLOG_ERR, strOutputInfo);
					g_cEffectFileTable.Release(pMtl->m_pEffect);
					goto Exit0;
				}
			}

			pMtl->m_nCurTechnique = nTechnique;
			pMtl->m_nEffParaIndex = nEffParaIndex;
			hr = Effect_OnLoad(nEffParaIndex, pMtl);
			if(FAILED(hr))
			{
				sprintf(strOutputInfo, "Initialize FX %s parameter set failed.", strFixedFileName);
				KGLogPrintf(KGLOG_ERR, strOutputInfo);
				goto Exit0;
			}
		}

		hr = S_OK;
Exit0:
		return hr;
	}

	HRESULT KG3DMaterial::Effect_OnLoad(int nIndex, KG3DMaterialSubset* pMtl)
	{
		HRESULT hr = E_FAIL;
		HRESULT hrRetCode = E_FAIL;
		D3DXEFFECT_DESC Desc;
		LPD3DXEFFECT pD3DXEffect = NULL;
		D3DXHANDLE ParHandle = 0;
		D3DXPARAMETER_DESC ParDesc;
		TCHAR str[256];
		_ShaderConstant* pNewCon = NULL;
		vector<_ShaderConstant*>* pEffParas = NULL;
		vector<_ShaderConstant*>* pNewEffParaSet = NULL;
		TCHAR* strChar = NULL;
		_TextureData* pTextData = NULL;
		DWORD dwType = TEXTYPE_COMMON;
		DWORD dwParameterType = MSEFFECT_TEXTURE;
		LPCSTR strName = NULL;
		D3DXHANDLE AnonationHandle = NULL;

#ifdef _DEBUG
		int nNumConst = 0;
#endif

		KG_PROCESS_ERROR(nIndex < static_cast<int>(m_dwNumMaterials));
		while(nIndex >= static_cast<int>(m_EffParaSets.size()))
		{
			pNewEffParaSet = new vector<_ShaderConstant*>;
			ASSERT(pNewEffParaSet);
			m_EffParaSets.push_back(pNewEffParaSet);
		}

		pEffParas = m_EffParaSets[nIndex];
		pEffParas->clear();
		pD3DXEffect = pMtl->m_pEffect;
		KG_PROCESS_ERROR(pD3DXEffect);

		hr = pD3DXEffect->GetDesc(&Desc);
		KG_COM_PROCESS_ERROR(hr);

		for(UINT K = 0; K < Desc.Parameters; K++)
		{
			ParHandle = pD3DXEffect->GetParameter(NULL, K);
			if(ParHandle)
			{
				hr = pD3DXEffect->GetParameterDesc(ParHandle, &ParDesc);
				if (FAILED(hr))
					continue;
				wsprintf(str, "%s", ParDesc.Name);
				switch(ParDesc.Class)
				{
				case D3DXPC_SCALAR:
					switch(ParDesc.Type)
					{
					case D3DXPT_VOID:
						{
							pNewCon = new _ShaderConstant(MSEFFECT_VOID, str, NULL, ParHandle);
							goto PUSH_BACK;
						}
					case D3DXPT_BOOL:
						{
							pNewCon = new _ShaderConstant(MSEFFECT_BOOL, str, NULL, ParHandle);
							goto PUSH_BACK;
						}
					case D3DXPT_INT:
						{
							int * pValue = new int;
							ASSERT(pValue);
							pD3DXEffect->GetValue(ParHandle,pValue,sizeof(int));
							pNewCon = new _ShaderConstant(MSEFFECT_INT, str, pValue, ParHandle);
							goto PUSH_BACK;
						}
					case D3DXPT_FLOAT:
						{
							if(ParDesc.Semantic)//处理高光级数
							{
								if(_stricmp(ParDesc.Semantic,"MATERIALPOWER")==0)
								{
									pNewCon = new _ShaderConstant(MSEFFECT_MATERIALPOWER, str, NULL, ParHandle);
									goto PUSH_BACK;
								}
							}
							else
							{
								float* pValue = new float;
								ASSERT(pValue);
								pD3DXEffect->GetValue(ParHandle,pValue,sizeof(float));
								pNewCon = new _ShaderConstant(MSEFFECT_FLOAT, str, pValue, ParHandle);
								goto PUSH_BACK;
							}
						}
					}
					break;
				case D3DXPC_VECTOR:
					if(ParDesc.Rows==1)
					{
						if(ParDesc.Semantic!=NULL)
						{
							if(_stricmp(ParDesc.Semantic,"MATERIALAMBIENT")==0)
							{
								pNewCon = new _ShaderConstant(MSEFFECT_MATERIALAMBIENT, str, NULL, ParHandle);
								goto PUSH_BACK;
							}

							if(_stricmp(ParDesc.Semantic,"MATERIALDIFFUSE")==0)
							{
								pNewCon = new _ShaderConstant(MSEFFECT_MATERIALDIFFUSE, str, NULL, ParHandle);
								goto PUSH_BACK;
							}

							if(_stricmp(ParDesc.Semantic,"MATERIALSPECULAR")==0)
							{
								pNewCon = new _ShaderConstant(MSEFFECT_MATERIALSPECULAR, str, NULL, ParHandle);
								goto PUSH_BACK;
							}

							if(_stricmp(ParDesc.Semantic,"MATERIALEMISSIVE")==0)
							{
								pNewCon = new _ShaderConstant(MSEFFECT_MATERIALEMISSIVE, str, NULL, ParHandle);
								goto PUSH_BACK;
							}

							if(_stricmp(ParDesc.Semantic,"COLOR")==0)
							{
								D3DXVECTOR4* pValue = new D3DXVECTOR4;
								ASSERT(pValue);
								pD3DXEffect->GetVector(ParHandle,(D3DXVECTOR4*)pValue);
								pNewCon = new _ShaderConstant(MSEFFECT_COLORRGBA, str, pValue, ParHandle);
								goto PUSH_BACK;
							}

							if(!_stricmp(ParDesc.Semantic, "POSITION"))
							{
								D3DXVECTOR4* pPos = new D3DXVECTOR4;
								pNewCon = new _ShaderConstant(MSEFFECT_POSITION, str, pPos, ParHandle);
								goto PUSH_BACK;
							}

							if(!_stricmp(ParDesc.Semantic, "LIGHTDIR0"))
							{
								pNewCon = new _ShaderConstant(MSEFFECT_LIGHTDIR0, str, NULL, ParHandle);
								goto PUSH_BACK;
							}

							if(!_stricmp(ParDesc.Semantic, "LIGHTDIFF0"))
							{
								pNewCon = new _ShaderConstant(MSEFFECT_LIGHTDIFF0, str, NULL, ParHandle);
								goto PUSH_BACK;
							}

							if(!_stricmp(ParDesc.Semantic, "ENVAMB"))
							{
								pNewCon = new _ShaderConstant(MSEFFECT_ENVAMB, str, NULL, ParHandle);
								goto PUSH_BACK;
							}

							if(!_stricmp(ParDesc.Semantic, "FOGINFO"))
							{
								pNewCon = new _ShaderConstant(ENU_MSEFFECT_FOGINFO, str, NULL, ParHandle);
								goto PUSH_BACK;
							}

							if(!_stricmp(ParDesc.Semantic, "FOGCOLOR"))
							{
								pNewCon = new _ShaderConstant(ENU_MSEFFECT_FOGCOLOR, str, NULL, ParHandle);
								goto PUSH_BACK;
							}
							//////////////////////////////////////////////////////////////////////////
							if(_stricmp(ParDesc.Semantic,"LOCALBBOXMIN")==0)
							{
								pNewCon = new _ShaderConstant(MSEFFECT_LOCALBBOXMIN, str, NULL, ParHandle);
								goto PUSH_BACK;
							}

							if(_stricmp(ParDesc.Semantic,"LOCALBBOXMAX")==0)
							{
								pNewCon = new _ShaderConstant(MSEFFECT_LOCALBBOXMAX, str, NULL, ParHandle);
								goto PUSH_BACK;
							}

							if(_stricmp(ParDesc.Semantic,"LOCALBBOXSIZE")==0)
							{
								pNewCon = new _ShaderConstant(MSEFFECT_LOCALBBOXSIZE, str, NULL, ParHandle);
								goto PUSH_BACK;
							}						
						}
						else
						{
							PVOID Pointer = NULL;
							switch(ParDesc.Columns)
							{
							case 2:
								{
									Pointer = new D3DXVECTOR2(0,0);
									pNewCon = new _ShaderConstant(MSEFFECT_FLOAT2, str, Pointer, ParHandle);
									goto PUSH_BACK;
								}
							case 3:
								{
									Pointer = new D3DXVECTOR3(0,0,0);
									pNewCon = new _ShaderConstant(MSEFFECT_FLOAT3, str, Pointer, ParHandle);
									goto PUSH_BACK;
								}
							case 4:
								{
									Pointer = new D3DXVECTOR4(0,0,0,0);
									pNewCon = new _ShaderConstant(MSEFFECT_FLOAT4, str, Pointer, ParHandle);
									goto PUSH_BACK;
								}
							}
						}
					}
					break;
				case D3DXPC_MATRIX_ROWS:
					{
						if(ParDesc.Semantic)
						{
							if(_stricmp(ParDesc.Semantic,"WORLD")==0)
							{
								pNewCon = new _ShaderConstant(MSEFFECT_WORLD, str, NULL, ParHandle);
								goto PUSH_BACK;
							}

							if(_stricmp(ParDesc.Semantic,"WORLDI")==0)
							{
								pNewCon = new _ShaderConstant(MSEFFECT_WORLDI, str, NULL, ParHandle);
								goto PUSH_BACK;
							}

							if(_stricmp(ParDesc.Semantic,"WORLDIT")==0)
							{
								pNewCon = new _ShaderConstant(MSEFFECT_WORLDIT, str, NULL, ParHandle);
								goto PUSH_BACK;
							}			

							if(_stricmp(ParDesc.Semantic,"VIEW")==0)
							{
								pNewCon = new _ShaderConstant(MSEFFECT_VIEW, str, NULL, ParHandle);
								goto PUSH_BACK;
							}	

							if(_stricmp(ParDesc.Semantic,"VIEWI")==0)
							{
								pNewCon = new _ShaderConstant(MSEFFECT_VIEWI, str, NULL, ParHandle);
								goto PUSH_BACK;
							}	

							if(_stricmp(ParDesc.Semantic,"VIEWIT")==0)
							{
								pNewCon = new _ShaderConstant(MSEFFECT_VIEWIT, str, NULL, ParHandle);
								goto PUSH_BACK;
							}		

							if(_stricmp(ParDesc.Semantic,"PROJECTION")==0)
							{
								pNewCon = new _ShaderConstant(MSEFFECT_PROJECTION, str, NULL, ParHandle);
								goto PUSH_BACK;
							}			

							if(_stricmp(ParDesc.Semantic,"PROJECTIONI")==0)
							{
								pNewCon = new _ShaderConstant(MSEFFECT_PROJECTIONI, str, NULL, ParHandle);
								goto PUSH_BACK;
							}

							if(_stricmp(ParDesc.Semantic,"PROJECTIONIT")==0)
							{
								pNewCon = new _ShaderConstant(MSEFFECT_PROJECTIONIT, str, NULL, ParHandle);
								goto PUSH_BACK;
							}

							if(_stricmp(ParDesc.Semantic,"WORLDVIEW")==0)
							{
								pNewCon = new _ShaderConstant(MSEFFECT_WORLDVIEW, str, NULL, ParHandle);
								goto PUSH_BACK;
							}

							if(_stricmp(ParDesc.Semantic,"WORLDVIEWI")==0)
							{
								pNewCon = new _ShaderConstant(MSEFFECT_WORLDVIEWI, str, NULL, ParHandle);
								goto PUSH_BACK;
							}

							if(_stricmp(ParDesc.Semantic,"WORLDVIEWIT")==0)
							{
								pNewCon = new _ShaderConstant(MSEFFECT_WORLDVIEWIT, str, NULL, ParHandle);
								goto PUSH_BACK;
							}

							if(_stricmp(ParDesc.Semantic,"WORLDVIEWPROJECTION")==0)
							{
								pNewCon = new _ShaderConstant(MSEFFECT_WORLDVIEWPROJ, str, NULL, ParHandle);
								goto PUSH_BACK;
							}

							if(_stricmp(ParDesc.Semantic,"WORLDVIEWPROJI")==0)
							{
								pNewCon = new _ShaderConstant(MSEFFECT_WORLDVIEWPROJI, str, NULL, ParHandle);
								goto PUSH_BACK;
							}

							if(_stricmp(ParDesc.Semantic,"WORLDVIEWPROJIT")==0)
							{
								pNewCon = new _ShaderConstant(MSEFFECT_WORLDVIEWPROJIT, str, NULL, ParHandle);
								goto PUSH_BACK;
							}

							if (_stricmp(ParDesc.Semantic, "VIEWPROJ") == 0)
							{
								pNewCon = new _ShaderConstant(MSEFFECT_VIEWPROJ, str, NULL, ParHandle);
								goto PUSH_BACK;
							}

							if(_stricmp(ParDesc.Semantic,"SUNTRANSFORMATION")==0)
							{
								pNewCon = new _ShaderConstant(MSEFFECT_SUNDIRMATRIX, str, NULL, ParHandle);
								goto PUSH_BACK;
							}				
						}
					}
					break;
				case D3DXPC_MATRIX_COLUMNS:
					break;
				case D3DXPC_OBJECT:
					{
						switch(ParDesc.Type)
						{
						case D3DXPT_STRING:
							{
								strChar = new TCHAR[256];
								ASSERT(strChar);
								pNewCon = new _ShaderConstant(MSEFFECT_STRING, str, strChar, ParHandle);
								goto PUSH_BACK;
								break;
							}
						case D3DXPT_TEXTURE:
							{
								pTextData = new _TextureData;
								KG_PROCESS_ERROR(pTextData);
								g_InitTextureData(pTextData);

								dwType = TEXTYPE_COMMON;
								dwParameterType = MSEFFECT_TEXTURE;
								strName = NULL;
								for(UINT S = 0;S < ParDesc.Annotations; S++)
								{
									AnonationHandle = pD3DXEffect->GetAnnotation(ParHandle,S);
									pD3DXEffect->GetParameterDesc(AnonationHandle, &ParDesc);
									if(ParDesc.Type == D3DXPT_STRING)
									{
										if(_stricmp(ParDesc.Name,"type")==0)
										{
											pD3DXEffect->GetString(AnonationHandle, &strName);
											if(_stricmp(strName,"CUBE")==0)
											{
												dwType = TEXTYPE_CUBE;
												dwParameterType = MSEFFECT_TEXTURECUBE;
											}
											else if(_stricmp(strName,"VOLUME")==0)
											{
												dwType = TEXTYPE_VOLUME;
												dwParameterType = MSEFFECT_TEXTUREVOLUME;
											}
										}
										else if(_stricmp(ParDesc.Name,"name")==0)
										{
											pD3DXEffect->GetString(AnonationHandle, &strName);
											pTextData->FileName = strName;

										}
									}
								}
								pTextData->FileName = "";
								pTextData->TextureID = 0xffffffff;
								pNewCon = new _ShaderConstant(dwParameterType,str,pTextData,ParHandle);
								goto PUSH_BACK;
							}
						case D3DXPT_TEXTURE1D:
						case D3DXPT_TEXTURE2D:
						case D3DXPT_TEXTURE3D:
							break;//do nothing
						}
						break;
					}
				case D3DXPC_STRUCT:
					break;

				}
			}
			continue;
PUSH_BACK:
			ASSERT(pNewCon);
#ifdef _DEBUG
			nNumConst++;
#endif 
			pEffParas->push_back(pNewCon);
		}
		hrRetCode = S_OK;
Exit0:
		return hrRetCode;
	}

	/*检查Shader参数的时候发现KG3DMaterial::Effect_SetParameter被注释掉了，里面
	有对Light参数的取和修改。在8月讨论的统一Light参数的会议终于已经确认Light参数要
	从Environment的Light里面取，这里是没有修改的。如果以后要恢复的话要改成从Environment
	的Light里面取Light参数。涉及的变量有：1.Shader里带ENVAMB语义的会记录到带MSEFFECT_ENVAMB
	类型的_ShaderConstant中

	// Last modify by Chen TianHong:2008-8-28 14:27:46
	*/

	//HRESULT KG3DMaterial::Effect_SetParameter(int nMaterialIndex, KG3DModel* pModel)
	//{
	//	HRESULT hr = S_OK;
	//	static D3DXMATRIX matWorld, matView, matProject, matWorldView, matWorldViewProj, matViewProj;
	//	KG3DMaterialSubset* pMaterial = &m_pMaterials[nMaterialIndex];
	//	std::vector<_ShaderConstant*>& EffParas = *m_EffParaSets[pMaterial->m_nEffParaIndex];
	//	LPD3DXEFFECT pD3DXEffect = pMaterial->m_pEffect->GetEffect();
	//	if(!pD3DXEffect)
	//		return E_FAIL;
	//
	//	if(pMaterial->m_nCurTechnique == -1)
	//	{
	//		pD3DXEffect->ValidateTechnique(NULL);
	//		//m_nCurTechnique = 0;
	//	}
	//	else
	//	{
	//		D3DXHANDLE pTec = pD3DXEffect->GetTechnique(pMaterial->m_nCurTechnique);
	//		if(pTec)
	//		{
	//			pD3DXEffect->SetTechnique(pTec);
	//		}
	//	}
	//
	//	g_pd3dDevice->GetTransform(D3DTS_WORLD, &matWorld);
	//	g_pd3dDevice->GetTransform(D3DTS_VIEW,  &matView);
	//	g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProject);
	//
	//	for(UINT K = 0; K < EffParas.size(); K++)
	//	{
	//		_ShaderConstant* pConst = EffParas[K];
	//		switch(pConst->dwType)
	//		{
	//		case MSEFFECT_WORLD:
	//			{
	//				hr = pD3DXEffect->SetValue(pConst->m_hParameter,&matWorld,sizeof(D3DXMATRIX));
	//				break;
	//			}
	//		case MSEFFECT_WORLDI:
	//			{
	//				D3DXMatrixInverse(&matWorld,NULL,&matWorld);
	//				hr = pD3DXEffect->SetValue(pConst->m_hParameter,&matWorld,sizeof(D3DXMATRIX));
	//				break;
	//			}
	//		case MSEFFECT_WORLDIT:
	//			{
	//				D3DXMatrixInverse(&matWorld,NULL,&matWorld);
	//				D3DXMatrixTranspose(&matWorld,&matWorld);
	//				hr = pD3DXEffect->SetValue(pConst->m_hParameter,&matWorld,sizeof(D3DXMATRIX));
	//				break;
	//			}
	//
	//		case MSEFFECT_VIEW:
	//			{
	//				hr = pD3DXEffect->SetValue(pConst->m_hParameter,&matView,sizeof(D3DXMATRIX));
	//				break;
	//			}
	//		case MSEFFECT_VIEWI:
	//			{
	//				D3DXMatrixInverse(&matView,NULL,&matView);
	//				hr = pD3DXEffect->SetValue(pConst->m_hParameter,&matView,sizeof(D3DXMATRIX));
	//				break;
	//			}
	//		case MSEFFECT_VIEWIT:
	//			{
	//				D3DXMatrixInverse(&matView, NULL, &matView);
	//				D3DXMatrixTranspose(&matView, &matView);
	//				hr = pD3DXEffect->SetValue(pConst->m_hParameter,&matView,sizeof(D3DXMATRIX));
	//				break;
	//			}
	//
	//		case MSEFFECT_PROJECTION:
	//			{
	//				hr = pD3DXEffect->SetValue(pConst->m_hParameter,&matProject,sizeof(D3DXMATRIX));
	//				break;
	//			}
	//		case MSEFFECT_PROJECTIONI:
	//			{
	//				D3DXMatrixInverse(&matProject, NULL, &matProject);
	//				hr = pD3DXEffect->SetValue(pConst->m_hParameter,&matProject,sizeof(D3DXMATRIX));
	//				break;
	//			}
	//		case MSEFFECT_PROJECTIONIT:
	//			{
	//				D3DXMatrixInverse(&matProject, NULL, &matProject);
	//				D3DXMatrixTranspose(&matProject, &matProject);
	//				hr = pD3DXEffect->SetValue(pConst->m_hParameter,&matProject,sizeof(D3DXMATRIX));
	//				break;
	//			}
	//		case MSEFFECT_WORLDVIEW:
	//			{
	//				matWorldView = matWorld * matView;
	//				hr = pD3DXEffect->SetValue(pConst->m_hParameter, &matWorldView, sizeof(D3DXMATRIX));
	//				break;
	//			}
	//		case MSEFFECT_WORLDVIEWI:
	//			{
	//				matWorldView = matWorld * matView;
	//				D3DXMatrixInverse(&matWorldView, NULL, &matWorldView);
	//				hr = pD3DXEffect->SetValue(pConst->m_hParameter, &matWorldView, sizeof(D3DXMATRIX));
	//				break;
	//			}
	//		case MSEFFECT_WORLDVIEWIT:
	//			{
	//				matWorldView = matWorld * matView;
	//				D3DXMatrixInverse(&matWorldView, NULL, &matWorldView);
	//				D3DXMatrixTranspose(&matWorldView, &matWorldView);
	//				hr = pD3DXEffect->SetValue(pConst->m_hParameter, &matWorldView, sizeof(D3DXMATRIX));
	//				break;
	//			}		
	//		case MSEFFECT_WORLDVIEWPROJ:
	//			{
	//				matWorldViewProj = matWorld * matView * matProject;
	//				hr = pD3DXEffect->SetValue(pConst->m_hParameter, &matWorldViewProj,sizeof(D3DXMATRIX));
	//				break;
	//			}
	//		case MSEFFECT_WORLDVIEWPROJI:
	//			{
	//				matWorldViewProj = matWorld * matView * matProject;
	//				D3DXMatrixInverse(&matWorldViewProj, NULL, &matWorldViewProj);
	//				hr = pD3DXEffect->SetValue(pConst->m_hParameter,&matWorldViewProj,sizeof(D3DXMATRIX));
	//				break;
	//			}
	//		case MSEFFECT_WORLDVIEWPROJIT:
	//			{
	//				matWorldViewProj = matWorld * matView * matProject;
	//				D3DXMatrixInverse(&matWorldViewProj, NULL, &matWorldViewProj);
	//				D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);
	//				hr = pD3DXEffect->SetValue(pConst->m_hParameter,&matWorldViewProj,sizeof(D3DXMATRIX));
	//				break;
	//			}
	//		case MSEFFECT_VIEWPROJ:
	//			{
	//				matViewProj = matView * matProject;
	//				hr = pD3DXEffect->SetValue(pConst->m_hParameter, &matViewProj, sizeof(D3DXMATRIX));
	//				break;
	//			}
	//
	//		case MSEFFECT_SUNDIRMATRIX:
	//			{
	//				//K3DScene* pScene = g_cGraphicsTool.GetCurScene();
	//				//if(pScene)
	//				//{
	//				//	D3DXMATRIX LightTrans;
	//				//	//g_pd3dDevice->GetTransform(D3DTS_VIEW , &LightTrans);
	//
	//				//	//pScene->m_SunLight.Direction;
	//				//	D3DXMatrixLookAtLH(&LightTrans,&D3DXVECTOR3(0,0,0),
	//				//		(D3DXVECTOR3*)&pScene->m_SunLight.Direction,
	//				//		&D3DXVECTOR3(0,0,0.001f));
	//				//	//hr = pD3DXEffect->SetMatrix(pConst->scName.c_str(),&LightTrans);
	//				//	hr = pD3DXEffect->SetValue(pConst->m_hParameter,&LightTrans,sizeof(D3DXMATRIX));
	//				//}
	//				break;
	//			}
	//		case MSEFFECT_POSITION:
	//			{
	//				D3DXVECTOR4* pData = static_cast<D3DXVECTOR4*>(pConst->pData);
	//				hr = pD3DXEffect->SetVector(pConst->m_hParameter, pData);
	//				break;
	//			}
	//			//////////////////////////////////////////////////////////////////////////
	//			//values 	
	//		case MSEFFECT_COLORRGBA:
	//			{
	//				D3DXVECTOR4* pData = (D3DXVECTOR4*) pConst->pData;
	//				hr = pD3DXEffect->SetVector(pConst->m_hParameter,pData);
	//				break;
	//			}
	//			//////////////////////////////////////////////////////////////////////////
	//			//values 
	//		case MSEFFECT_FLOAT:
	//			{
	//				float* pData = (float*) pConst->pData;
	//				hr = pD3DXEffect->SetValue(pConst->m_hParameter,pData,4);
	//				break;
	//			}
	//		case MSEFFECT_INT:
	//			{
	//				int* pData = (int*) pConst->pData;
	//				hr = pD3DXEffect->SetValue(pConst->m_hParameter,pData,4);
	//				break;
	//			}
	//			//////////////////////////////////////////////////////////////////////////
	//			//Materials	
	//		case MSEFFECT_TEXTURE:
	//		case MSEFFECT_TEXTURECUBE:
	//		case MSEFFECT_TEXTUREVOLUME:
	//			{
	//				KG3DTexture* pTexture = NULL;
	//				LPTEXTUREDATA pTextData = (LPTEXTUREDATA) pConst->pData;
	//
	//				if (pTextData->TextureID != 0xffffffff && SUCCEEDED(g_cTextureTable.GetResource(&pTexture, pTextData->TextureID)))
	//				{
	//					LPDIRECT3DCUBETEXTURE9 pCube = (LPDIRECT3DCUBETEXTURE9) pTexture->GetTexture();
	//					hr = pD3DXEffect->SetTexture(pConst->scName.c_str(),pCube);
	//				}
	//				else
	//				{
	//					hr = pD3DXEffect->SetTexture(pConst->scName.c_str(), NULL);
	//				}
	//				break;
	//			}
	//		case MSEFFECT_MATERIALAMBIENT:
	//			{
	//				D3DXVECTOR4 Vec;
	//				Vec.x = pMaterial->m_sMaterial9.Ambient.r;
	//				Vec.y = pMaterial->m_sMaterial9.Ambient.g;
	//				Vec.z = pMaterial->m_sMaterial9.Ambient.b;
	//				Vec.w = pMaterial->m_sMaterial9.Ambient.a;
	//
	//				//hr = pD3DXEffect->SetVector(pConst->scName.c_str(),&Vec);
	//				hr = pD3DXEffect->SetValue(pConst->m_hParameter,&Vec,sizeof(D3DXVECTOR4));
	//				break;
	//			}
	//		case MSEFFECT_LIGHTDIFF0:
	//			{
	//				D3DLIGHT9 Light;
	//				g_pd3dDevice->GetLight(0, &Light);
	//				hr = pD3DXEffect->SetValue(pConst->m_hParameter, &Light.Diffuse, sizeof(D3DXVECTOR4));
	//				break;
	//			}
	//		case MSEFFECT_ENVAMB:
	//			{
	//				DWORD dwAmbient;
	//				D3DXVECTOR4 vecAmb = *(D3DXVECTOR4*)& g_GetRenderGlobalState().GetSunLightParam().sceneAmbient;
	//				hr = pD3DXEffect->SetValue(pConst->m_hParameter, &vecAmb, sizeof(D3DXVECTOR4));
	//				break;
	//			}
	//		case MSEFFECT_MATERIALDIFFUSE:
	//			{
	//				D3DXVECTOR4 Vec;
	//				Vec.x = pMaterial->m_sMaterial9.Diffuse.r;
	//				Vec.y = pMaterial->m_sMaterial9.Diffuse.g;
	//				Vec.z = pMaterial->m_sMaterial9.Diffuse.b;
	//				Vec.w = pMaterial->m_sMaterial9.Diffuse.a;
	//
	//				//hr = pD3DXEffect->SetVector(pConst->scName.c_str(),&Vec);
	//				hr = pD3DXEffect->SetValue(pConst->m_hParameter,&Vec,sizeof(D3DXVECTOR4));
	//				break;
	//			}
	//		case MSEFFECT_MATERIALSPECULAR:
	//			{
	//				D3DXVECTOR4 Vec;
	//				Vec.x = pMaterial->m_sMaterial9.Specular.r;
	//				Vec.y = pMaterial->m_sMaterial9.Specular.g;
	//				Vec.z = pMaterial->m_sMaterial9.Specular.b;
	//				Vec.w = pMaterial->m_sMaterial9.Specular.a;
	//
	//				//hr = pD3DXEffect->SetVector(pConst->scName.c_str(),&Vec);
	//				hr = pD3DXEffect->SetValue(pConst->m_hParameter,&Vec,sizeof(D3DXVECTOR4));
	//				break;
	//			}
	//		case MSEFFECT_MATERIALEMISSIVE:
	//			{
	//				D3DXVECTOR4 Vec;
	//				Vec.x = pMaterial->m_sMaterial9.Emissive.r;
	//				Vec.y = pMaterial->m_sMaterial9.Emissive.g;
	//				Vec.z = pMaterial->m_sMaterial9.Emissive.b;
	//				Vec.w = pMaterial->m_sMaterial9.Emissive.a;
	//
	//				hr = pD3DXEffect->SetValue(pConst->m_hParameter,&Vec,sizeof(D3DXVECTOR4));
	//				break;
	//			}
	//
	//		case MSEFFECT_MATERIALPOWER:
	//			{
	//				hr = pD3DXEffect->SetValue(pConst->m_hParameter,&pMaterial->m_sMaterial9.Power,sizeof(float));
	//				break;
	//			}
	//
	//		case MSEFFECT_LOCALBBOXMIN:
	//			{
	//				D3DXVECTOR4 Vec;
	//				//Vec.x = pModel->m_BBox_A.x;
	//				//Vec.y = pModel->m_BBox_A.y;
	//				//Vec.z = pModel->m_BBox_A.z;
	//				//Vec.w = 0;
	//				//hr = pD3DXEffect->SetVector(pConst->scName.c_str(),&Vec);
	//				hr = pD3DXEffect->SetValue(pConst->m_hParameter,&Vec,sizeof(D3DXVECTOR4));
	//				break;
	//			}
	//		case MSEFFECT_LOCALBBOXMAX:
	//			{
	//				D3DXVECTOR4 Vec;
	//				/*Vec.x = pModel->m_BBox_B.x;
	//				Vec.y = pModel->m_BBox_B.y;
	//				Vec.z = pModel->m_BBox_B.z;
	//				Vec.w = 0;*/
	//				//hr = pD3DXEffect->SetVector(pConst->scName.c_str(),&Vec);
	//				hr = pD3DXEffect->SetValue(pConst->m_hParameter,&Vec,sizeof(D3DXVECTOR4));
	//				break;
	//			}
	//		case MSEFFECT_LOCALBBOXSIZE:
	//			{
	//				D3DXVECTOR4 Vec;
	//				/*Vec.x = pModel->m_BBox_B.x - pModel->m_BBox_A.x;
	//				Vec.y = pModel->m_BBox_B.y - pModel->m_BBox_A.y;
	//				Vec.z = pModel->m_BBox_B.z - pModel->m_BBox_A.z;
	//				Vec.w = 0;*/
	//				//hr = pD3DXEffect->SetVector(pConst->scName.c_str(),&Vec);
	//				hr = pD3DXEffect->SetValue(pConst->m_hParameter,&Vec,sizeof(D3DXVECTOR4));
	//				break;
	//			}
	//		case MSEFFECT_LIGHTPOS0:
	//		case MSEFFECT_LIGHTPOS1:
	//		case MSEFFECT_LIGHTPOS2:
	//		case MSEFFECT_LIGHTPOS3:
	//		case MSEFFECT_LIGHTPOS4:
	//		case MSEFFECT_LIGHTPOS5:
	//		case MSEFFECT_LIGHTPOS6:
	//		case MSEFFECT_LIGHTPOS7:
	//			{
	//				DWORD dwLightIndex = pConst->dwType - MSEFFECT_LIGHTPOS0;
	//				BOOL bLightEnable = FALSE;
	//				D3DLIGHT9 light;
	//				g_pd3dDevice->GetLightEnable(dwLightIndex, &bLightEnable);
	//				if(bLightEnable)
	//				{
	//					g_pd3dDevice->GetLight(dwLightIndex, &light);
	//					D3DXVECTOR4 vec4LightPos = D3DXVECTOR4(light.Position.x, light.Position.y, light.Position.z, 1.0f);
	//					hr = pD3DXEffect->SetVector(pConst->m_hParameter, &vec4LightPos);
	//				}
	//			}
	//		}
	//	}
	//	for(int K = 0; K < 8; K++)
	//	{
	//		if(pMaterial->m_dwTextureIDs[K] != 0xffffffff)
	//		{
	//			KG3DTexture* pTexture = NULL;
	//
	//			TCHAR TextName[256];
	//			wsprintf(TextName,"Tex%d",K);
	//
	//			if (SUCCEEDED(g_cTextureTable.GetResource(&pTexture,pMaterial->m_dwTextureIDs[K])))
	//			{
	//				hr = pD3DXEffect->SetTexture(TextName, pTexture->GetTexture());
	//			}
	//		}
	//		else
	//		{
	//			break;
	//		}
	//	}
	//	return S_OK;
	//}
	//
	//

	HRESULT KG3DMaterial::Effect_SetParameter(int nMaterialIndex)
	{
		HRESULT hr = E_FAIL;
		static D3DXMATRIX matMats[ENUM_MATRIX_COUNT];
		KG3DMaterialSubset* pMaterial = &m_pMaterials[nMaterialIndex];
		std::vector<_ShaderConstant*>& EffParas = *m_EffParaSets[pMaterial->m_nEffParaIndex];
		D3DXHANDLE pTec = NULL;
		LPD3DXEFFECT pD3DXEffect = NULL;
		EffectParameterSettingTool* pTool = NULL;
		_ShaderConstant* pConst = NULL;

		pD3DXEffect = pMaterial->m_pEffect;
		KG_PROCESS_ERROR(pD3DXEffect);

		if(pMaterial->m_nCurTechnique == -1)
		{
			pD3DXEffect->ValidateTechnique(NULL);
		}
		else
		{
			pTec = pD3DXEffect->GetTechnique(pMaterial->m_nCurTechnique);
			if(pTec)
			{
				hr = pD3DXEffect->SetTechnique(pTec);
				KGLOG_COM_CHECK_ERROR(hr);
			}
		}

		g_pd3dDevice->GetTransform(D3DTS_WORLD, &matMats[ENUM_MATRIX_WORLD]);
		g_pd3dDevice->GetTransform(D3DTS_VIEW, &matMats[ENUM_MATRIX_VIEW]);
		g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matMats[ENUM_MATRIX_PROJ]);


		pTool = GetEffectParameterSettingTool();
		pTool->SetEffect(pD3DXEffect);
		for(UINT K = 0; K < EffParas.size(); K++)
		{
			pConst = EffParas[K];
			pTool->m_pFunctions[pConst->dwType](matMats, (void*)&pMaterial->m_sMaterial9, pConst);
		}

		hr = S_OK;

Exit0:
		return hr;
	}

	KG3DMaterial::_TextureOpEx::_TextureOpEx()
	{
		ColorOp = D3DTOP_SELECTARG1;
		AlphaOp = D3DTOP_DISABLE;	
		AlphaArg1 = D3DTA_TEXTURE;
		AlphaArg2 = D3DTA_CURRENT;
		ColorArg1 = D3DTA_TEXTURE;
		ColorArg2 = D3DTA_CURRENT;	

		ColorArg0 = D3DTA_CURRENT;
		AlphaArg0 = D3DTA_CURRENT;
		Result = D3DTA_CURRENT;
		Constant = D3DTA_CONSTANT;
	}

	KG3DMaterial::_TextureOp::_TextureOp()
	{
		ColorOp = D3DTOP_SELECTARG1;
		AlphaOp = D3DTOP_DISABLE;	
		AlphaArg1 = D3DTA_TEXTURE;
		AlphaArg2 = D3DTA_CURRENT;
		ColorArg1 = D3DTA_TEXTURE;
		ColorArg2 = D3DTA_CURRENT;	
		TFactor = 0;
	}

	KG3DMaterial::_TextureMap::_TextureMap()
	{
		TextureMap = D3DTSS_TCI_PASSTHRU;
	}

	HRESULT KG3DMaterial::_ShaderConstant::Clear()
	{
		HRESULT hr = S_OK;
		switch(dwType)
		{
		case MSEFFECT_STRING:
			{
				SAFE_DELETE_ARRAY(pData);
				break;
			}
		case MSEFFECT_INT:
			{
				int* pInt = static_cast<int*>(pData);
				SAFE_DELETE(pInt);
				break;
			}
		case MSEFFECT_FLOAT:
			{
				float* pFloat = static_cast<float*>(pData);
				SAFE_DELETE(pFloat);
				break;
			}
		case MSEFFECT_FLOAT2:
			{
				D3DXVECTOR2* pVec2 = static_cast<D3DXVECTOR2*>(pData);
				SAFE_DELETE(pVec2);
				break;
			}
		case MSEFFECT_FLOAT3:
			{
				D3DXVECTOR3* pVec3 = static_cast<D3DXVECTOR3*>(pData);
				SAFE_DELETE(pVec3);
				break;
			}
		case MSEFFECT_FLOAT4:
			{
				D3DXVECTOR4* pVec4 = static_cast<D3DXVECTOR4*>(pData);
				SAFE_DELETE(pVec4);
				break;
			}
			break;

		case MSEFFECT_TEXTURE:
		case MSEFFECT_TEXTURE3D:
		case MSEFFECT_TEXTURECUBE:
			{
				LPTEXTUREDATA pText = (LPTEXTUREDATA) pData;
				KG3DTexture* pTexture = NULL;
				hr = g_cTextureTable.GetResource(&pTexture, pText->TextureID);
				if(SUCCEEDED(hr))
				{
					pTexture->Release();
					SAFE_RELEASE(pTexture);
				}
				SAFE_DELETE(pText);
				break;
			}
		default:
			break;
		}
		scName.clear();
		return S_OK;
	}

	KG3DMaterial::KG3DMaterialSubset::KG3DMaterialSubset()
	{

		memset(m_pTextureNumOption, 0, sizeof(DWORD) * cNumID);
		memset(m_lpTextures, 0, sizeof(KG3DTexture*) * cNumID);

		m_dwNumOptions = 0;
		m_dwOption = 0;
		m_pEffect = NULL;
		m_dwNumFaces = 0;
		m_nCurTechnique = 0;
		m_nEffParaIndex = -1;
		strEffectFileName[0] = '\0';

		//m_ColorCast.a = 0.f;
		//m_ColorCast.r = 0.f;
		//m_ColorCast.g = 0.f;
		//m_ColorCast.b = 1.f;
		for (int i = 0;i < cNumColorCast;i++)
			m_ColorCast[i] = g_cEngineOption.ColorCast[i];

		m_fSpecPower  = 1.f;
		m_fEmssPower  = 1.f;

		m_dwNumUsedTexture = 0;
		m_dwMaterialID = 0;
		m_dwPassID = 0;
		m_lpNormalMap = NULL;
	}

	KG3DMaterial::KG3DMaterialSubset::~KG3DMaterialSubset()
	{
		//MessageBox(NULL,"Material disconsturct","Note",0);
		if (m_pEffect)
		{
			g_cEffectFileTable.Release(m_pEffect);
			m_pEffect = NULL;
		}
		for(int i = 0; i < 8; i++)
		{
			SAFE_RELEASE(m_lpTextures[i]);
		}

		SAFE_RELEASE(m_lpNormalMap);

		CleanTextureStageOptions();

		CleanMtlOptions();
	};

	void KG3DMaterial::KG3DMaterialSubset::CleanTextureStageOptions(DWORD dwStage)
	{
		_MtlOption* pOption = NULL;

		if(dwStage>=8)
			return;

		for(int j = 0; j < static_cast<int>(m_pTextureOptions[dwStage].size()); j++)
		{
			pOption = m_pTextureOptions[dwStage][j];
			SAFE_DELETE(pOption);
		}

		SAFE_RELEASE(m_lpTextures[dwStage]);

		m_pTextureNumOption[dwStage] = 0;
		m_pTextureOptions[dwStage].clear();
	}

	void KG3DMaterial::KG3DMaterialSubset::CleanTextureStageOptions()
	{
		_MtlOption* pOption = NULL;

		for(int i = 0; i < 8; i++)
		{
			for(int j = 0; j < static_cast<int>(m_pTextureOptions[i].size()); j++)
			{
				pOption = m_pTextureOptions[i][j];
				SAFE_DELETE(pOption);
			}
			m_pTextureNumOption[i] = 0;
			m_pTextureOptions[i].clear();
		}
	}
	void KG3DMaterial::KG3DMaterialSubset::CleanMtlOptions()
	{
		_MtlOption* pOption = NULL;

		for(int i = 0; i < static_cast<int>(m_pMatOptions.size()); i++)
		{
			pOption = m_pMatOptions[i];
			SAFE_DELETE(pOption);
		}
		m_dwNumOptions = 0;
		m_pMatOptions.clear();
	}


	_MtlAlphaOperation* KG3DMaterial::KG3DMaterialSubset::GetAlphaOperation()
	{
		_MtlAlphaOperation* pAlpha = NULL;
		_MtlOption* pOp = NULL;
		for(DWORD i = 0; i < m_dwNumOptions; i++)
		{
			pOp = m_pMatOptions[i];
			if (pOp->Type == OPTION_MATERIAL_ALPHAOPERATION)
			{
				pAlpha = (_MtlAlphaOperation*) pOp->pData;
				goto Exit0;
			}
		}

		pOp = new _MtlOption;
		ASSERT(pOp);
		pAlpha = new _MtlAlphaOperation;
		ASSERT(pAlpha);

		pAlpha->EnableAlphaBlend = FALSE;
		pAlpha->SrcBlend = D3DBLEND_SRCALPHA;
		pAlpha->DestBlend = D3DBLEND_INVSRCALPHA;
		pAlpha->BlendFactor = 0xffffffff;
		pAlpha->BlendOperator = D3DBLENDOP_ADD;

		pAlpha->EnableSeparateAlphaBlend = FALSE;
		pAlpha->BlendOperatorAlpha = D3DBLENDOP_ADD;
		pAlpha->SrcBlendAlpha = D3DBLEND_ONE;
		pAlpha->DestBlendAlpha = D3DBLEND_ZERO;

		pAlpha->EnableAlphaTest = TRUE;
		pAlpha->AlphaTestRef = 0x80;
		pAlpha->BackAlphaTestRef = pAlpha->AlphaTestRef;
		pAlpha->AlphaFunction = D3DCMP_GREATEREQUAL;

		pOp->pData = pAlpha;
		pOp->Type = OPTION_MATERIAL_ALPHAOPERATION;
		m_pMatOptions.push_back(pOp);
		m_dwNumOptions++;
Exit0:
		return pAlpha;
	}

	_MtlRenderTwiceAlphaOption* KG3DMaterial::KG3DMaterialSubset::GetAlphaRenderTwice()
	{
		_MtlRenderTwiceAlphaOption *pAlpha = NULL;
		_MtlOption* pOption = NULL;
		for (DWORD i = 0; i < m_dwNumOptions; i++)
		{
			pOption = m_pMatOptions[i];
			if (pOption->Type == OPTION_MATERIAL_ALPHARENDERTWICE)
			{
				pAlpha = (_MtlRenderTwiceAlphaOption*) pOption->pData;
				goto Exit0;
			}
		}
		pAlpha = new _MtlRenderTwiceAlphaOption;
		KG_PROCESS_ERROR(pAlpha);

		pOption = new _MtlOption;
		KG_PROCESS_ERROR(pOption);

		pAlpha->dwAlphaRefPass0 = _MtlRenderTwiceAlphaOption::s_dwRef0;
		pAlpha->dwAlphaRefPass1 = _MtlRenderTwiceAlphaOption::s_dwRef1;
		pAlpha->bEnable = FALSE;

		pOption->Type = OPTION_MATERIAL_ALPHARENDERTWICE;
		pOption->pData = pAlpha;

		m_pMatOptions.push_back(pOption);
		m_dwNumOptions++;
Exit0:
		if (!pOption)
		{
			SAFE_DELETE(pAlpha);
		}
		return pAlpha;
	}

	KG3DMaterial::_MtlTextureOpEx* KG3DMaterial::KG3DMaterialSubset::GetTextureStageOperation(DWORD dwTextIndex)
	{
		ASSERT(dwTextIndex < 8);
		DWORD NumOption = static_cast<DWORD>(m_pTextureOptions[dwTextIndex].size());
		_MtlOption* pOp = NULL;
		_MtlTextureOpEx* pTextureOp = NULL;
		for(DWORD i = 0; i < NumOption; i++)
		{
			pOp = m_pTextureOptions[dwTextIndex][i];
			if (pOp->Type == OPTION_TEXTURE_OPERATIONEX)
			{
				pTextureOp = reinterpret_cast<_MtlTextureOpEx*>(pOp->pData);
				goto Exit0;
			}
		}

		pOp = new _MtlOption;
		ASSERT(pOp);

		pTextureOp = new _MtlTextureOpEx;
		ASSERT(pTextureOp);

		pOp->pData = pTextureOp;
		pOp->Type = OPTION_TEXTURE_OPERATIONEX;
		m_pTextureOptions[dwTextIndex].push_back(pOp);
		m_pTextureNumOption[dwTextIndex]++;
Exit0:
		return pTextureOp;
	}


	KG3DMaterial::_MatBlend::_MatBlend()
	{
		SrcBlend  = D3DBLEND_SRCALPHA;
		DestBlend = D3DBLEND_INVSRCALPHA;
	}

	KG3DMaterial::_MtlOption::_MtlOption()
	{
		Type  =  0;
		pData = NULL;
	}

	KG3DMaterial::_MtlOption::~_MtlOption()
	{
		Type  =  0;
		SAFE_DELETE(pData);
	}

	bool KG3DMaterial::_ShaderConstant::operator == (_ShaderConstant& Const)
	{
		if ((dwType == Const.dwType)&&(scName==Const.scName))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	KG3DMaterial::_MtlTextureData::_MtlTextureData()
	{
		TextureID = 0;
	}

	KG3DMaterial::_MtlTextureData::~_MtlTextureData()
	{
		FileName.clear();
		TextureID = 0;
	}

	KG3DMaterial::_MtlTextureOpEx::_MtlTextureOpEx()
	{
		ColorOp = D3DTOP_SELECTARG1;
		AlphaOp = D3DTOP_DISABLE;	
		AlphaArg1 = D3DTA_TEXTURE;
		AlphaArg2 = D3DTA_CURRENT;
		ColorArg1 = D3DTA_TEXTURE;
		ColorArg2 = D3DTA_CURRENT;	

		ColorArg0 = D3DTA_CURRENT;
		AlphaArg0 = D3DTA_CURRENT;
		Result = D3DTA_CURRENT;
		Constant = D3DTA_CONSTANT;
	}

	KG3DMaterial::_MtlTextureOp::_MtlTextureOp()
	{
		ColorOp = D3DTOP_SELECTARG1;
		AlphaOp = D3DTOP_DISABLE;	
		AlphaArg1 = D3DTA_TEXTURE;
		AlphaArg2 = D3DTA_CURRENT;
		ColorArg1 = D3DTA_TEXTURE;
		ColorArg2 = D3DTA_CURRENT;	
		TFactor = 0;
	}

	KG3DMaterial::_MtlTextureMap::_MtlTextureMap()
	{
		TextureMap = D3DTSS_TCI_PASSTHRU;
	}

	void g_InitTextureData(_TextureData *pData)
	{
		pData->TextureID = 0xffffffff;
	}

	void g_UnInitTextureData(_TextureData *pData)
	{
		pData->FileName.clear();
		pData->TextureID = 0xffffffff;
	}

	KG3DMaterial::_MtlBlend::_MtlBlend()
	{
		SrcBlend  = D3DBLEND_SRCALPHA;
		DestBlend = D3DBLEND_INVSRCALPHA;
	}

	HRESULT KG3DMaterial::SaveOption(_MtlOption* pOption, FILE *fpFile)
	{
		HRESULT hr = E_FAIL;
		TCHAR strFileName[MAX_PATH];
		KG3DMaterialSubset* pMtl = NULL;

		fwrite(&pOption->Type, sizeof(DWORD), 1, fpFile);
		switch(pOption->Type)
		{
		case OPTION_MATERIAL_CULLING:
			{
				_MatCull *pMatCull = reinterpret_cast<_MatCull*>(pOption->pData);
				fwrite(pMatCull, sizeof(_MatCull), 1, fpFile);
				break;
			}
		case OPTION_MATERIAL_ALPHABLEND:
			{
				_MatBlend *pMatBlend = reinterpret_cast<_MatBlend*>(pOption->pData);
				fwrite(pMatBlend, sizeof(_MatBlend), 1, fpFile);
				break;
			}
		case OPTION_MATERIAL_ALPHAOPERATION:
			{
				_MtlAlphaOperation *pMtlAlphaOp = reinterpret_cast<_MtlAlphaOperation*>(pOption->pData);
				fwrite(pMtlAlphaOp, sizeof(_MtlAlphaOperation), 1, fpFile);
				break;
			}
		case OPTION_MATERIAL_ALPHARENDERTWICE:
			{
				_MtlRenderTwiceAlphaOption *pMtlRenderTwiceOp = reinterpret_cast<_MtlRenderTwiceAlphaOption*>(pOption->pData);
				fwrite(pMtlRenderTwiceOp, sizeof(_MtlRenderTwiceAlphaOption), 1, fpFile);
				break;
			}
		case OPTION_MATERIAL_MSEFFECT:
			{
				pMtl = reinterpret_cast<KG3DMaterialSubset*>(pOption->pData);
				hr = g_pFileFinder->GetRelativePath(m_scName.c_str(), pMtl->strEffectFileName, strFileName);
				fwrite(strFileName,sizeof(TCHAR),MAX_PATH,fpFile);
				fwrite(&pMtl->m_nCurTechnique,sizeof(int),1,fpFile);
				fwrite(&pMtl->m_nEffParaIndex, sizeof(int), 1, fpFile);
				break;
			}

		case OPTION_TEXTURE_OPERATION:
			{
				_TextureOp *pTextureOp = (_TextureOp*) pOption->pData;
				fwrite(pTextureOp,sizeof(_TextureOp),1,fpFile);
				break;
			}
		case OPTION_TEXTURE_OPERATIONEX:
			{
				_TextureOpEx *pTextureOpEx = (_TextureOpEx*) pOption->pData;
				fwrite(pTextureOpEx,sizeof(_TextureOpEx),1,fpFile);
				break;
			}
		case OPTION_TEXTURE_MAPMETHOD:
			{
				_TextureMap *pMap = (_TextureMap*) pOption->pData;
				fwrite(pMap,sizeof(_TextureMap),1,fpFile);
				break;
			}
		case OPTION_TEXTURE_TRANSFROM:
			{
				_TextureTf *pTf = (_TextureTf*) pOption->pData;
				fwrite(pTf,sizeof(_TextureTf),1,fpFile);
				break;
			}
		case OPTION_MATERIAL_ZFUN:
			{
				fwrite(pOption->pData, sizeof(DWORD), 1, fpFile);
				break;
			}
		}
		hr = S_OK;
		return hr;
	}

	KG3DMaterial::KG3DMaterialSubset* KG3DMaterial::GetSubMaterial(DWORD dwIndex)
	{
		KG3DMaterialSubset* pSubMtl = NULL;;

		if(dwIndex < m_dwNumMaterials)
		{
			pSubMtl = &m_pMaterials[dwIndex];
		}

		return pSubMtl;
	}


	HRESULT KG3DMaterial::_ShaderConstant::GetDesc(LPSTR strDesc)
	{
		HRESULT hr = S_OK;
		switch(dwType)
		{
		case MSEFFECT_VOID:
			wsprintf(strDesc,"任意类型");
			break;
		case MSEFFECT_BOOL:
			wsprintf(strDesc,"BOOL");
			break;
		case MSEFFECT_INT:
			{
				//			int* pValue = (int*) this->pData;
				//wsprintf(strDesc,"INT %d",*pValue);
				wsprintf(strDesc, "INT");
				break;
			}
		case MSEFFECT_FLOAT:
			{
				//			float * pValue = (float*) this->pData;
				//sprintf(strDesc,"FLOAT %.2f",*pValue);
				wsprintf(strDesc, "FLOAT");
				break;
			}
		case MSEFFECT_FLOAT2:
			{
				wsprintf(strDesc,"FLOAT2");
				break;
			}
		case MSEFFECT_FLOAT3:
			{
				wsprintf(strDesc,"FLOAT3");
				break;
			}
		case MSEFFECT_FLOAT4:
			{
				wsprintf(strDesc,"FLOAT4");
				break;
			}
		case MSEFFECT_TEXTURE:
			{
				LPTEXTUREDATA  pValue = (LPTEXTUREDATA) pData;		
				wsprintf(strDesc,"TEXTURE %s",pValue->FileName.c_str());
				break;
			}
		case MSEFFECT_TEXTURE1D:
			{
				LPTEXTUREDATA  pValue = (LPTEXTUREDATA) pData;		
				wsprintf(strDesc,"TEXTURE1D %s",pValue->FileName.c_str());
				break;
			}
		case MSEFFECT_TEXTURE2D:
			{
				LPTEXTUREDATA  pValue = (LPTEXTUREDATA) pData;		
				wsprintf(strDesc,"TEXTURE2D %s",pValue->FileName.c_str());
				break;
			}
		case MSEFFECT_TEXTURE3D:
			{
				LPTEXTUREDATA  pValue = (LPTEXTUREDATA) pData;		
				wsprintf(strDesc,"TEXTURE3D %s",pValue->FileName.c_str());
				break;
			}
		case MSEFFECT_TEXTURECUBE:
			{
				LPTEXTUREDATA  pValue = (LPTEXTUREDATA) pData;		
				wsprintf(strDesc,"TEXTURECUBE %s",pValue->FileName.c_str());
				break;
			}

		case MSEFFECT_MATERIALAMBIENT:
			{
				wsprintf(strDesc,"MATERIALAMBIENT 自动");
				//hr = E_FAIL;
				break;
			}
		case MSEFFECT_MATERIALDIFFUSE:
			{
				wsprintf(strDesc,"MATERIALDIFFUSE自动");
				//hr = E_FAIL;
				break;
			}
		case MSEFFECT_MATERIALSPECULAR:
			{
				wsprintf(strDesc,"MATERIALSPECULAR自动");
				//hr = E_FAIL;
				break;
			}
		case MSEFFECT_MATERIALEMISSIVE:
			{
				sprintf(strDesc,"MATERIALEMISSIVE自动");
				hr = E_FAIL;
				break;
			}
		case MSEFFECT_MATERIALPOWER:
			{
				wsprintf(strDesc,"MATERIALPOWER自动");
				hr = E_FAIL;
				break;
			}
		case  MSEFFECT_LOCALBBOXMIN :
			{
				wsprintf(strDesc,"LOCALBBOXMIN 自动");
				hr = E_FAIL;
				break;
			}
		case MSEFFECT_LOCALBBOXMAX:
			{
				wsprintf(strDesc,"LOCALBBOXMAX自动");
				hr = E_FAIL;
				break;
			}
		case MSEFFECT_WORLD :
			{
				wsprintf(strDesc,"WORLD 自动");
				hr = E_FAIL;
				break;
			}
		case MSEFFECT_WORLDI :
			{
				wsprintf(strDesc,"WORLDI自动");
				hr = E_FAIL;
				break;
			}
		case MSEFFECT_WORLDIT  :
			{
				wsprintf(strDesc,"WORLDIT 自动");
				hr = E_FAIL;
				break;
			}
		case MSEFFECT_VIEW :
			{
				wsprintf(strDesc,"VIEW自动");
				hr =  E_FAIL;
				break;
			}
		case MSEFFECT_VIEWI :
			{
				wsprintf(strDesc,"VIEWI自动");
				hr = E_FAIL;
				break;	
			}
		case MSEFFECT_VIEWIT :
			{
				wsprintf(strDesc,"VIEWIT自动");
				hr = E_FAIL;
			}
			break;
		case MSEFFECT_PROJECTION  :
			{
				wsprintf(strDesc,"PROJECTION自动");
				hr = E_FAIL;
				break;
			}
		case MSEFFECT_PROJECTIONI  :
			{
				wsprintf(strDesc,"PROJECTIONI 自动");
				hr = E_FAIL;
				break;
			}
		case MSEFFECT_PROJECTIONIT  :
			{
				wsprintf(strDesc,"PROJECTIONIT 自动");
				hr = E_FAIL;
				break;
			}
		case MSEFFECT_WORLDVIEW :
			{
				wsprintf(strDesc,"WORLDVIEW自动");
				hr = E_FAIL;
				break;
			}
		case MSEFFECT_WORLDVIEWI :
			{
				wsprintf(strDesc,"WORLDVIEWI自动");
				hr = E_FAIL;
				break;
			}
		case MSEFFECT_WORLDVIEWIT :
			{
				wsprintf(strDesc,"WORLDVIEWIT自动");
				hr = E_FAIL;
				break;
			}
		case MSEFFECT_WORLDVIEWPROJ :
			{
				wsprintf(strDesc,"WORLDVIEWPROJ自动");
				hr = E_FAIL;
				break;
			}
		case MSEFFECT_WORLDVIEWPROJI :
			{
				wsprintf(strDesc,"WORLDVIEWPROJI自动");
				hr = E_FAIL;
				break;
			}
		case MSEFFECT_WORLDVIEWPROJIT :
			{
				wsprintf(strDesc,"WORLDVIEWPROJIT自动");
				hr = E_FAIL;
				break;
			}
		case MSEFFECT_SUNDIRMATRIX :
			{
				wsprintf(strDesc,"SUNDIRMATRIX自动");
				hr = E_FAIL;
				break;
			}
		case MSEFFECT_POSITION :
			{
				wsprintf(strDesc,"POSITION");
				break;
			}
		case MSEFFECT_DIRECTION :
			{
				wsprintf(strDesc,"DIRECTION");
				break;
			}
		case MSEFFECT_COLORRGBA :
			{
				wsprintf(strDesc,"COLORRGBA");
				break;
			}
		case MSEFFECT_STRING :
			{
				wsprintf(strDesc,"STRING");
				break;
			}
		default:
			{
				wsprintf(strDesc,"未定义");
				break;
			}
		}
		return hr;
	}


	void NormalizeDir(TCHAR* strDir)
	{
		KG_PROCESS_ERROR(strDir);

		TCHAR* strTemp = strDir;

		strTemp = strstr(strDir, "\\\\");
		if (strTemp == strDir)
		{
			strTemp = strstr(strDir + 2, "\\\\");
		}

		while(strTemp)
		{
			g_pFileFinder->strShiftLeft(strTemp, 0, 1);
			strTemp = strstr(strDir, "\\\\");
		}

		strTemp = strstr(strDir, "\\..");
		while(strTemp)
		{
			TCHAR* strTemp1 = max(strTemp - 1, strDir);
			while(strTemp1 > strDir && *strTemp1 != '\\')
			{
				strTemp1--;
			}

			g_pFileFinder->strShiftLeft(strTemp1, 0, strTemp - strTemp1 + 3);
			strTemp = strstr(strDir, "\\..");
		}
Exit0:
		return;
	}


	size_t GetSeekSize(DWORD dwType)
	{
		size_t uReturn = 0;
		static std::map<DWORD, size_t> s_MapSeekTable;
		if (!s_MapSeekTable.size())
		{
			s_MapSeekTable[MSEFFECT_COLORRGBA] = sizeof(D3DXVECTOR4);
			s_MapSeekTable[MSEFFECT_POSITION] = sizeof(D3DXVECTOR4);
			s_MapSeekTable[MSEFFECT_BOOL] = sizeof(BOOL);
			s_MapSeekTable[MSEFFECT_INT] = sizeof(INT);
			s_MapSeekTable[MSEFFECT_FLOAT] = sizeof(float);
			s_MapSeekTable[MSEFFECT_FLOAT2] = sizeof(D3DXVECTOR2);
			s_MapSeekTable[MSEFFECT_FLOAT3] = sizeof(D3DXVECTOR3);
			s_MapSeekTable[MSEFFECT_FLOAT4] = sizeof(D3DXVECTOR4);

			s_MapSeekTable[MSEFFECT_TEXTURE] = sizeof(TCHAR) * MAX_PATH;
			s_MapSeekTable[MSEFFECT_TEXTUREVOLUME] = sizeof(TCHAR) * MAX_PATH;
			s_MapSeekTable[MSEFFECT_TEXTURECUBE] = sizeof(TCHAR) * MAX_PATH;
		}
		if (s_MapSeekTable.find(dwType) != s_MapSeekTable.end())
		{
			uReturn = s_MapSeekTable[dwType];
		}
		return uReturn;
	}



	HRESULT KG3DMaterial::LoadEffectParameter(IFile* pFile)
	{
		int nRetCode = false;
		HRESULT hr = E_FAIL;
		DWORD dwSize = 0;
		TCHAR strParameterName[MAX_PATH];
		DWORD dwNumConsts = 0;
		DWORD dwType = 0;
		TCHAR strTextureFileName[MAX_PATH];
		TCHAR strOutputInfo[MAX_PATH*3];
		_TextureData* pTextureData = NULL;
		_EffectParameterHeader Header;

		dwSize = pFile->Read(&Header, sizeof(_EffectParameterHeader));

		KGLOG_PROCESS_ERROR(dwSize == sizeof(_EffectParameterHeader));
		KG_PROCESS_ERROR(Header.dwMask == _EffectParameterHeader::m_dwEffHeaderMask);

		strOutputInfo[0] = '\0';

		for (unsigned int i = 0; i < Header.dwNumParaSets; i++)
		{
			map<string, int> ParametersMap;
			std::vector<_ShaderConstant*>& vecCurParSet = *m_EffParaSets[i];
			for(int j = 0; j < static_cast<int>(vecCurParSet.size()); j++)
			{
				_ShaderConstant& ShaderConst = *vecCurParSet[j];
				ParametersMap[ShaderConst.scName] = j;
			}

			dwSize = pFile->Read(&dwNumConsts, sizeof(DWORD));
			KGLOG_PROCESS_ERROR(dwSize == sizeof(DWORD));

			for (DWORD k = 0; k < dwNumConsts; k++)
			{
				dwSize = pFile->Read(&dwType, sizeof(DWORD));
				KGLOG_PROCESS_ERROR(dwSize == sizeof(DWORD));

				dwSize = pFile->Read(strParameterName, sizeof(TCHAR) * MAX_PATH);
				KGLOG_PROCESS_ERROR(dwSize == sizeof(TCHAR) * MAX_PATH);

				if (ParametersMap.find(strParameterName) == ParametersMap.end())
				{
					size_t uSizeToSeek = GetSeekSize(dwType);
					nRetCode = pFile->Seek((long)uSizeToSeek, SEEK_CUR);
					KGLOG_PROCESS_ERROR(nRetCode != -1);
					continue;
				}
				switch(dwType)
				{
				case MSEFFECT_POSITION:
				case MSEFFECT_COLORRGBA:
					{
						D3DXVECTOR4* pData = static_cast<D3DXVECTOR4*>(vecCurParSet[ParametersMap[strParameterName]]->pData);
						if(pData)
						{
							dwSize = pFile->Read(pData, sizeof(D3DXVECTOR4));
							KGLOG_PROCESS_ERROR(dwSize == sizeof(D3DXVECTOR4));
						}
						_ASSERT(_CrtCheckMemory());
						break;
					}
				case MSEFFECT_BOOL:
					{
						BOOL* pData = static_cast<BOOL*>(vecCurParSet[ParametersMap[strParameterName]]->pData);
						dwSize = pFile->Read(pData, sizeof(BOOL));
						KGLOG_PROCESS_ERROR(dwSize == sizeof(BOOL));
						_ASSERT(_CrtCheckMemory());
						break;
					}
				case MSEFFECT_INT:
					{
						INT* pData = static_cast<INT*>(vecCurParSet[ParametersMap[strParameterName]]->pData);
						dwSize = pFile->Read(pData, sizeof(INT));
						KGLOG_PROCESS_ERROR(dwSize == sizeof(INT));
						_ASSERT(_CrtCheckMemory());
						break;
					}
				case MSEFFECT_FLOAT:
					{
						float* pData = static_cast<float*>(vecCurParSet[ParametersMap[strParameterName]]->pData);
						if(pData)
						{
							dwSize = pFile->Read(pData, sizeof(float));
							KGLOG_PROCESS_ERROR(dwSize == sizeof(float));
						}
						else
						{
							float fData = 0.0f;
							dwSize = pFile->Read(&fData, sizeof(float));
							KGLOG_PROCESS_ERROR(dwSize == sizeof(float));
						}
						_ASSERT(_CrtCheckMemory());
						break;
					}
				case MSEFFECT_FLOAT2:
					{
						float* pData = static_cast<float*>(vecCurParSet[ParametersMap[strParameterName]]->pData);
						dwSize = pFile->Read(pData, sizeof(float) * 2);
						KGLOG_PROCESS_ERROR(dwSize == sizeof(float) * 2);
						//_ASSERT(_CrtCheckMemory());
						break;
					}
				case MSEFFECT_FLOAT3:
					{
						D3DXVECTOR3* pData = static_cast<D3DXVECTOR3*>(vecCurParSet[ParametersMap[strParameterName]]->pData);
						dwSize = pFile->Read(pData, sizeof(float) * 3);
						KGLOG_PROCESS_ERROR(dwSize == sizeof(float) * 3);
						//_ASSERT(_CrtCheckMemory());
						break;
					}
				case MSEFFECT_FLOAT4:
					{
						float* pData = static_cast<float*>(vecCurParSet[ParametersMap[strParameterName]]->pData);
						dwSize = pFile->Read(pData, sizeof(float) * 4);
						KGLOG_PROCESS_ERROR(dwSize == sizeof(float) * 4);
						//_ASSERT(_CrtCheckMemory());
						break;
					}
				case MSEFFECT_TEXTURE:
				case MSEFFECT_TEXTUREVOLUME:		
				case MSEFFECT_TEXTURECUBE:
					{
						pTextureData = static_cast<_TextureData*>(vecCurParSet[ParametersMap[strParameterName]]->pData);
						if(!pTextureData)
						{
							pTextureData = new _TextureData;
							KG_PROCESS_ERROR(pTextureData);
							g_InitTextureData(pTextureData);

							vecCurParSet[ParametersMap[strParameterName]]->pData = static_cast<void*>(pTextureData);
						}


						dwSize = pFile->Read(strTextureFileName, sizeof(TCHAR) * MAX_PATH);
						KGLOG_PROCESS_ERROR(dwSize == sizeof(TCHAR) * MAX_PATH);

						pTextureData->FileName = strTextureFileName; 
						_ASSERT(_CrtCheckMemory());
						break;
					}
				}
				_ASSERT(_CrtCheckMemory());
			}
			ParametersMap.clear();
		}
		hr = S_OK;
Exit0:

		return hr;
	}

	HRESULT KG3DMaterial::SaveEffectParameter(FILE* fpFile)
	{
		HRESULT hr = E_FAIL;
		TCHAR strParameterName[MAX_PATH];
		DWORD dwNumConsts = 0;
		DWORD dwNumActualConsts = 0;
		TCHAR strRelativePath[MAX_PATH];
		TCHAR strAbsPath[MAX_PATH];

		_EffectParameterHeader Header;
		KG_PROCESS_ERROR(m_EffParaSets.size());
		ASSERT(fpFile);
		_TextureData* pTextureData = NULL;
		long fCurPos = 0;
		Header.dwMask = _EffectParameterHeader::m_dwEffHeaderMask;
		Header.dwNumParaSets = static_cast<DWORD>(m_EffParaSets.size());
		Header.dwVersion  = _EffectParameterHeader::m_dwCurrentVersion;

		fwrite(&Header, sizeof(Header), 1, fpFile);
		for(DWORD j = 0; j < Header.dwNumParaSets; j++)
		{
			long lSavePos = ftell(fpFile);
			vector<_ShaderConstant*>& vecCurParSet = *m_EffParaSets[j];
			dwNumConsts = static_cast<DWORD>(vecCurParSet.size());
			fwrite(&dwNumConsts, sizeof(DWORD), 1, fpFile);
			for(DWORD i = 0; i < dwNumConsts; i++)
			{
				_ShaderConstant& ShaderConst = *(vecCurParSet[i]);
				switch(ShaderConst.dwType)
				{
				case MSEFFECT_WORLD:
				case MSEFFECT_WORLDI:
				case MSEFFECT_WORLDIT:
				case MSEFFECT_VIEW:
				case MSEFFECT_VIEWI:
				case MSEFFECT_VIEWIT:
				case MSEFFECT_PROJECTION:
				case MSEFFECT_PROJECTIONI:
				case MSEFFECT_PROJECTIONIT:
				case MSEFFECT_WORLDVIEW:
				case MSEFFECT_WORLDVIEWI:
				case MSEFFECT_WORLDVIEWIT:
				case MSEFFECT_WORLDVIEWPROJ:
				case MSEFFECT_WORLDVIEWPROJI:
				case MSEFFECT_WORLDVIEWPROJIT:
				case MSEFFECT_SUNDIRMATRIX:
					{
						//Saving these parameters to file is not necessary
						break;
					}
				case MSEFFECT_POSITION:
				case MSEFFECT_COLORRGBA:
					{
						fwrite(&ShaderConst.dwType, sizeof(DWORD), 1, fpFile);
						strcpy(strParameterName, ShaderConst.scName.c_str());
						fwrite(strParameterName, sizeof(TCHAR), MAX_PATH, fpFile);
						fwrite(ShaderConst.pData, sizeof(D3DXVECTOR4), 1, fpFile);
						dwNumActualConsts++;
						break;
					}
				case MSEFFECT_DIRECTION:
					{
						break;
					}
				case MSEFFECT_STRING:
					{
						break;
					}
				case MSEFFECT_VOID:
					{
						break;
					}
				case MSEFFECT_BOOL:
					{
						fwrite(&ShaderConst.dwType, sizeof(DWORD), 1, fpFile);
						strcpy(strParameterName, ShaderConst.scName.c_str());
						fwrite(strParameterName, sizeof(TCHAR), MAX_PATH, fpFile);
						fwrite(ShaderConst.pData, sizeof(BOOL), 1, fpFile);
						dwNumActualConsts++;
						break;
					}
				case MSEFFECT_INT:
					{
						fwrite(&ShaderConst.dwType, sizeof(DWORD), 1, fpFile);
						strcpy(strParameterName, ShaderConst.scName.c_str());
						fwrite(strParameterName, sizeof(TCHAR), MAX_PATH, fpFile);
						fwrite(ShaderConst.pData, sizeof(INT), 1, fpFile);
						dwNumActualConsts++;
						break;
					}
				case MSEFFECT_FLOAT:
					{
						fwrite(&ShaderConst.dwType, sizeof(DWORD), 1, fpFile);
						strcpy(strParameterName, ShaderConst.scName.c_str());
						fwrite(strParameterName, sizeof(TCHAR), MAX_PATH, fpFile);
						fwrite(ShaderConst.pData, sizeof(float), 1, fpFile);
						dwNumActualConsts++;
						break;
					}
				case MSEFFECT_FLOAT2:
					{
						fwrite(&ShaderConst.dwType, sizeof(DWORD), 1, fpFile);
						strcpy(strParameterName, ShaderConst.scName.c_str());
						fwrite(strParameterName, sizeof(TCHAR), MAX_PATH, fpFile);
						fwrite(ShaderConst.pData, sizeof(float), 2, fpFile);
						dwNumActualConsts++;
						break;
					}
				case MSEFFECT_FLOAT3:
					{
						fwrite(&ShaderConst.dwType, sizeof(DWORD), 1, fpFile);
						strcpy(strParameterName, ShaderConst.scName.c_str());
						fwrite(strParameterName, sizeof(TCHAR), MAX_PATH, fpFile);
						fwrite(ShaderConst.pData, sizeof(float), 3, fpFile);
						dwNumActualConsts++;
						break;
					}
				case MSEFFECT_FLOAT4:
					{
						fwrite(&ShaderConst.dwType, sizeof(DWORD), 1, fpFile);
						strcpy(strParameterName, ShaderConst.scName.c_str());
						fwrite(strParameterName, sizeof(TCHAR), MAX_PATH, fpFile);
						fwrite(ShaderConst.pData, sizeof(float), 4, fpFile);
						dwNumActualConsts++;
						break;
					}
				case MSEFFECT_TEXTURE:
				case MSEFFECT_TEXTUREVOLUME:		
				case MSEFFECT_TEXTURECUBE:
					{
						fwrite(&ShaderConst.dwType, sizeof(DWORD), 1, fpFile);
						strcpy(strParameterName, ShaderConst.scName.c_str());
						fwrite(strParameterName, sizeof(TCHAR), MAX_PATH, fpFile);
						pTextureData = static_cast<_TextureData*>(ShaderConst.pData);
						g_pFileFinder->GetAbsPath(m_scName.c_str(), strAbsPath);
						if(!strcmp(pTextureData->FileName.c_str(), ""))
						{
							//如果没有贴图， 那么就写空的
							ZeroMemory(strRelativePath, sizeof(TCHAR) * MAX_PATH);
						}
						else
						{
							hr = g_pFileFinder->GetRelativePath(strAbsPath, pTextureData->FileName.c_str(), strRelativePath);
						}
						fwrite(strRelativePath, sizeof(TCHAR), MAX_PATH, fpFile);
						dwNumActualConsts++;
						break;
					}
				case MSEFFECT_TEXTURE1D:
				case MSEFFECT_TEXTURE2D:
				case MSEFFECT_TEXTURE3D:
					{
						//We are not going to use these parameter type.
						break;
					}
				case MSEFFECT_MATERIALAMBIENT:
				case MSEFFECT_MATERIALDIFFUSE:
				case MSEFFECT_MATERIALSPECULAR:
				case MSEFFECT_MATERIALEMISSIVE:
				case MSEFFECT_MATERIALPOWER:
					{
						//These parameter is saved in the material, not effect parameter
						break;
					}
				}
			}
			fCurPos = ftell(fpFile);
			fseek(fpFile, lSavePos, SEEK_SET);
			fwrite(&dwNumActualConsts, sizeof(DWORD), 1, fpFile);
			dwNumActualConsts = 0;
			fseek(fpFile, fCurPos, SEEK_SET);
		}

		hr = S_OK;
Exit0:
		return hr;
	}

	//ENU_MSEFFECT_WORLD
	HRESULT EffectParameterSettingTool::SetWorldMatrix(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		hr = m_pEffect->SetValue(pConst->m_hParameter, &pMats[ENUM_MATRIX_WORLD], sizeof(D3DXMATRIX));
		return hr;
	}

	//ENU_MSEFFECT_WORLDI
	HRESULT EffectParameterSettingTool::SetWorldIMatrix(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		D3DXMATRIX matWorldI;
		D3DXMatrixInverse(&matWorldI, 0, &pMats[ENUM_MATRIX_WORLD]);
		hr = m_pEffect->SetValue(pConst->m_hParameter, &matWorldI, sizeof(D3DXMATRIX));
		return hr;
	}

	//ENU_MSEFFECT_VIEW
	HRESULT EffectParameterSettingTool::SetViewMatrix(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		hr = m_pEffect->SetValue(pConst->m_hParameter, &pMats[ENUM_MATRIX_VIEW], sizeof(D3DXMATRIX));
		return hr;
	}

	//ENU_MSEFFECT_PROJECTION
	HRESULT EffectParameterSettingTool::SetProjMatrix(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		hr = m_pEffect->SetValue(pConst->m_hParameter, &pMats[ENUM_MATRIX_PROJ], sizeof(D3DXMATRIX));
		return hr;
	}

	//ENU_MSEFFECT_WORLDVIEW
	HRESULT EffectParameterSettingTool::SetWorldViewMatrix(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		if(!m_bMatrixSet[ENUM_MATRIX_WORLDVIEW])
		{
			m_bMatrixSet[ENUM_MATRIX_WORLDVIEW] = TRUE;
			pMats[ENUM_MATRIX_WORLDVIEW] = pMats[ENUM_MATRIX_WORLD] * pMats[ENUM_MATRIX_VIEW];
		}
		hr = m_pEffect->SetValue(pConst->m_hParameter, &pMats[ENUM_MATRIX_WORLDVIEW], sizeof(D3DXMATRIX));
		return hr;
	}

	//ENU_MSEFFECT_WORLDVIEWPROJ
	HRESULT EffectParameterSettingTool::SetWorldViewProjMatrix(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		if(!m_bMatrixSet[ENUM_MATRIX_WORLDVIEWPROJ])
		{
			m_bMatrixSet[ENUM_MATRIX_WORLDVIEWPROJ] = TRUE;
			pMats[ENUM_MATRIX_WORLDVIEWPROJ] = pMats[ENUM_MATRIX_WORLD] * 
				pMats[ENUM_MATRIX_VIEW] * 
				pMats[ENUM_MATRIX_PROJ];
		}
		hr = m_pEffect->SetValue(pConst->m_hParameter, &pMats[ENUM_MATRIX_WORLDVIEWPROJ], sizeof(D3DXMATRIX));
		return hr;
	}

	//ENU_MSEFFECT_VIEWI
	HRESULT EffectParameterSettingTool::SetViewIMatrix(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		D3DXMATRIX matViewI;
		D3DXMatrixInverse(&matViewI, 0, &pMats[ENUM_MATRIX_VIEW]);
		hr = m_pEffect->SetValue(pConst->m_hParameter, &matViewI, sizeof(D3DXMATRIX));
		return hr;
	}

	//ENU_MSEFFECT_WORLDIT
	HRESULT EffectParameterSettingTool::SetWorldITMatrix(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		D3DXMATRIX matWorldIT;
		D3DXMatrixInverse(&matWorldIT, 0, &pMats[ENUM_MATRIX_WORLD]);
		D3DXMatrixTranspose(&matWorldIT, &matWorldIT);
		hr = m_pEffect->SetValue(pConst->m_hParameter, &matWorldIT, sizeof(D3DXMATRIX));
		return hr;
	}

	//ENU_MSEFFECT_VIEWIT
	HRESULT EffectParameterSettingTool::SetViewITMatrix(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		D3DXMATRIX matViewIT;
		D3DXMatrixInverse(&matViewIT, 0, &pMats[ENUM_MATRIX_VIEW]);
		D3DXMatrixTranspose(&matViewIT, &matViewIT);
		hr = m_pEffect->SetValue(pConst->m_hParameter, &matViewIT, sizeof(D3DXMATRIX));
		return hr;
	}

	//ENU_MSEFFECT_PROJECTIONI
	HRESULT EffectParameterSettingTool::SetProjIMatrix(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		D3DXMATRIX matProjI;
		D3DXMatrixInverse(&matProjI, 0, &pMats[ENUM_MATRIX_PROJ]);
		hr = m_pEffect->SetValue(pConst->m_hParameter, &matProjI, sizeof(D3DXMATRIX));
		return hr;
	}

	//ENU_MSEFFECT_PROJECTIONIT
	HRESULT EffectParameterSettingTool::SetProjITMatrix(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		D3DXMATRIX matProjIT;
		D3DXMatrixInverse(&matProjIT, 0, &pMats[ENUM_MATRIX_PROJ]);
		D3DXMatrixTranspose(&matProjIT, &matProjIT);
		hr = m_pEffect->SetValue(pConst->m_hParameter, &matProjIT, sizeof(D3DXMATRIX));
		return hr;
	}

	//ENU_MSEFFECT_WORLDVIEWI
	HRESULT EffectParameterSettingTool::SetWorldViewIMatrix(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		D3DXMATRIX matWorldViewI;
		if(!m_bMatrixSet[ENUM_MATRIX_WORLDVIEW])
		{
			m_bMatrixSet[ENUM_MATRIX_WORLDVIEW] = TRUE;
			pMats[ENUM_MATRIX_WORLDVIEW] = pMats[ENUM_MATRIX_WORLD] * pMats[ENUM_MATRIX_VIEW];
		}
		D3DXMatrixInverse(&matWorldViewI, 0, &pMats[ENUM_MATRIX_WORLDVIEW]);
		hr = m_pEffect->SetValue(pConst->m_hParameter, &matWorldViewI, sizeof(D3DXMATRIX));
		return hr;
	}

	//ENU_MSEFFECT_WORLDVIEWIT
	HRESULT EffectParameterSettingTool::SetWorldViewITMatrix(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		D3DXMATRIX matWorldViewIT;
		if(!m_bMatrixSet[ENUM_MATRIX_WORLDVIEW])
		{
			m_bMatrixSet[ENUM_MATRIX_WORLDVIEW] = TRUE;
			pMats[ENUM_MATRIX_WORLDVIEW] = pMats[ENUM_MATRIX_WORLD] * pMats[ENUM_MATRIX_VIEW];
		}
		D3DXMatrixInverse(&matWorldViewIT, 0, &pMats[ENUM_MATRIX_WORLDVIEW]);
		D3DXMatrixTranspose(&matWorldViewIT, &matWorldViewIT);
		hr = m_pEffect->SetValue(pConst->m_hParameter, &matWorldViewIT, sizeof(D3DXMATRIX));
		return hr;
	}

	//ENU_MSEFFECT_WORLDVIEWPROJI
	HRESULT EffectParameterSettingTool::SetWorldViewProjIMatrix(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		D3DXMATRIX matWorldViewProjI;
		if(!m_bMatrixSet[ENUM_MATRIX_WORLDVIEWPROJ])
		{
			m_bMatrixSet[ENUM_MATRIX_WORLDVIEWPROJ] = TRUE;
			pMats[ENUM_MATRIX_WORLDVIEWPROJ] = pMats[ENUM_MATRIX_WORLD] * 
				pMats[ENUM_MATRIX_VIEW] * 
				pMats[ENUM_MATRIX_PROJ];
		}
		D3DXMatrixInverse(&matWorldViewProjI, 0, &pMats[ENUM_MATRIX_WORLDVIEWPROJ]);
		hr = m_pEffect->SetValue(pConst->m_hParameter, &matWorldViewProjI, sizeof(D3DXMATRIX));
		return hr;
	}

	//ENU_MSEFFECT_WORLDVIEWPROJIT
	HRESULT EffectParameterSettingTool::SetWorldViewProjITMatrix(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		D3DXMATRIX matWorldViewProjIT;
		if(!m_bMatrixSet[ENUM_MATRIX_WORLDVIEWPROJ])
		{
			m_bMatrixSet[ENUM_MATRIX_WORLDVIEWPROJ] = TRUE;
			pMats[ENUM_MATRIX_WORLDVIEWPROJ] = pMats[ENUM_MATRIX_WORLD] * 
				pMats[ENUM_MATRIX_VIEW] * 
				pMats[ENUM_MATRIX_PROJ];
		}
		D3DXMatrixInverse(&matWorldViewProjIT, 0, &pMats[ENUM_MATRIX_WORLDVIEWPROJ]);
		D3DXMatrixTranspose(&matWorldViewProjIT, &matWorldViewProjIT);
		hr = m_pEffect->SetValue(pConst->m_hParameter, &matWorldViewProjIT, sizeof(D3DXMATRIX));
		return hr;
	}

	//ENU_MSEFFECT_VIEWPROJ
	HRESULT EffectParameterSettingTool::SetViewProjMatrix(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		if(!m_bMatrixSet[ENUM_MATRIX_VIEWPROJ])
		{
			pMats[ENUM_MATRIX_VIEWPROJ] = pMats[ENUM_MATRIX_VIEW] * pMats[ENUM_MATRIX_PROJ];
			m_bMatrixSet[ENUM_MATRIX_VIEWPROJ] = TRUE;
		}
		hr = m_pEffect->SetValue(pConst->m_hParameter, &pMats[ENUM_MATRIX_VIEWPROJ], sizeof(D3DXMATRIX));
		return hr;
	}

	//ENU_MSEFFECT_POSITION
	HRESULT EffectParameterSettingTool::SetPos(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		hr = m_pEffect->SetVector(pConst->m_hParameter, (D3DXVECTOR4*)pConst->pData);
		return hr;
	}

	//ENU_MSEFFECT_DIRECTION
	HRESULT EffectParameterSettingTool::SetDir(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		return hr;
	}

	//ENU_MSEFFECT_COLORRGBA
	HRESULT EffectParameterSettingTool::SetColor(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		hr = m_pEffect->SetVector(pConst->m_hParameter, (D3DXVECTOR4*)pConst->pData);
		return hr;
	}

	//ENU_MSEFFECT_VOID
	HRESULT EffectParameterSettingTool::SetVoid(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		return hr;
	}

	//ENU_MSEFFECT_BOOL
	HRESULT EffectParameterSettingTool::SetBOOL(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		return hr;
	}

	//ENU_MSEFFECT_INT
	HRESULT EffectParameterSettingTool::SetInt(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		hr = m_pEffect->SetValue(pConst->m_hParameter, pConst->pData, sizeof(int));
		return hr;
	}

	//ENU_MSEFFECT_FLOAT
	HRESULT EffectParameterSettingTool::SetFloat(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		hr = m_pEffect->SetValue(pConst->m_hParameter, pConst->pData, sizeof(float));
		return hr;
	}

	//ENU_MSEFFECT_FLOAT2
	HRESULT EffectParameterSettingTool::SetFloat2(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		return hr;
	}

	//ENU_MSEFFECT_FLOAT3
	HRESULT EffectParameterSettingTool::SetFloat3(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		return hr;
	}

	//ENU_MSEFFECT_FLOAT4
	HRESULT EffectParameterSettingTool::SetFloat4(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		return hr;
	}

	//ENU_MSEFFECT_TEXTURE
	HRESULT EffectParameterSettingTool::SetTexture(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		KG3DTexture* pTexture = NULL;
		LPDIRECT3DTEXTURE9 pd3dTexture = NULL;
		_TextureData* pTextData = reinterpret_cast<_TextureData*>(pConst->pData);

		if (pTextData->TextureID != 0xffffffff) 
		{
			hr = g_cTextureTable.GetResource(&pTexture, pTextData->TextureID); 
			KGLOG_COM_PROCESS_ERROR(hr);
			pTexture->Release();

			pd3dTexture =  pTexture->GetTexture();
			hr = m_pEffect->SetTexture(pConst->m_hParameter, (LPDIRECT3DBASETEXTURE9)pd3dTexture);
			KGLOG_COM_PROCESS_ERROR(hr);
		}
		else
		{
			hr = m_pEffect->SetTexture(pConst->m_hParameter, NULL);
			KGLOG_COM_PROCESS_ERROR(hr);
		}

Exit0:
		return hr;
	}

	//ENU_MSEFFECT_TEXTURE1D
	HRESULT EffectParameterSettingTool::SetTexture1D(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		return hr;
	}

	//ENU_MSEFFECT_TEXTURE2D
	HRESULT EffectParameterSettingTool::SetTexture2D(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		return hr;
	}

	//ENU_MSEFFECT_TEXTURE3D
	HRESULT EffectParameterSettingTool::SetTexture3D(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		return hr;
	}

	//ENU_MSEFFECT_TEXTURECUBE
	HRESULT EffectParameterSettingTool::SetTextureCube(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		return hr;
	}

	//ENU_MSEFFECT_TEXTUREVOLUME
	HRESULT EffectParameterSettingTool::SetTextureVolume(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		return hr;
	}

	//ENU_MSEFFECT_MATERIALAMBIENT
	HRESULT EffectParameterSettingTool::SetAmbient(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		D3DMATERIAL9* pMaterial = reinterpret_cast<D3DMATERIAL9*>(pData);
		hr = m_pEffect->SetValue(pConst->m_hParameter, &pMaterial->Ambient, sizeof(D3DXVECTOR4));
		return hr;
	}

	//ENU_MSEFFECT_MATERIALDIFFUSE
	HRESULT EffectParameterSettingTool::SetDiffuse(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		D3DMATERIAL9* pMaterial = reinterpret_cast<D3DMATERIAL9*>(pData);
		hr = m_pEffect->SetValue(pConst->m_hParameter, &pMaterial->Diffuse, sizeof(D3DXVECTOR4));
		return hr;
	}

	//ENU_MSEFFECT_MATERIALSPECULAR
	HRESULT EffectParameterSettingTool::SetSpecular(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		D3DMATERIAL9* pMaterial = reinterpret_cast<D3DMATERIAL9*>(pData);
		hr = m_pEffect->SetValue(pConst->m_hParameter, &pMaterial->Specular, sizeof(D3DXVECTOR4));
		return hr;
	}

	//ENU_MSEFFECT_MATERIALEMISSIVE
	HRESULT EffectParameterSettingTool::SetEmissive(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		D3DMATERIAL9* pMaterial = reinterpret_cast<D3DMATERIAL9*>(pData);
		hr = m_pEffect->SetValue(pConst->m_hParameter, &pMaterial->Emissive,sizeof(D3DXVECTOR4));
		return hr;
	}

	//ENU_MSEFFECT_MATERIALPOWER
	HRESULT EffectParameterSettingTool::SetPower(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		D3DMATERIAL9* pMaterial = reinterpret_cast<D3DMATERIAL9*>(pData);
		hr = m_pEffect->SetValue(pConst->m_hParameter, &pMaterial->Power, sizeof(float));
		return hr;
	}

	//ENU_MSEFFECT_LOCALBBOXMIN
	HRESULT EffectParameterSettingTool::SetBBoxMin(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		return hr;
	}

	//ENU_MSEFFECT_LOCALBBOXMAX
	HRESULT EffectParameterSettingTool::SetBBoxMax(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		return hr;
	}

	//ENU_MSEFFECT_LOCALBBOXSIZE
	HRESULT EffectParameterSettingTool::SetBoxSize(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		return hr;
	}

	//ENU_MSEFFECT_STRING
	HRESULT EffectParameterSettingTool::SetString(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		return hr;
	}

	//ENU_MSEFFECT_LIGHTPOS0-7
	HRESULT EffectParameterSettingTool::SetLightDir(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		_ASSERTE(NULL && _T("光参数应该从KG3DScene的KG3DEnvironment的KG3DEnvEffLightSet的KG3DEnvEffLight中取，参考\"剑3统一光照模型.txt\""));
		HRESULT hr = E_FAIL;
		DWORD dwLightIndex = pConst->dwType - MSEFFECT_LIGHTDIR0;
		BOOL bLightEnable = FALSE;
		D3DLIGHT9 light;
		D3DXVECTOR4 vec4LightPos;
		g_pd3dDevice->GetLightEnable(dwLightIndex, &bLightEnable);
		if(bLightEnable)
		{
			g_pd3dDevice->GetLight(dwLightIndex, &light);
			hr = m_pEffect->SetValue(pConst->m_hParameter, &light.Direction, sizeof(D3DVECTOR));
		}
		return hr;
	}

	//ENU_MSEFFECT_LIGHTDIFF0-7
	HRESULT EffectParameterSettingTool::SetLightDiff(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		_ASSERTE(NULL && _T("光参数应该从KG3DScene的KG3DEnvironment的KG3DEnvEffLightSet的KG3DEnvEffLight中取，参考\"剑3统一光照模型.txt\""));
		HRESULT hr = E_FAIL;
		BOOL bLightEnable = FALSE;
		D3DLIGHT9 light;
		DWORD dwLightIndex = pConst->dwType - MSEFFECT_LIGHTDIFF0;
		g_pd3dDevice->GetLightEnable(dwLightIndex, &bLightEnable);
		if(bLightEnable)
		{
			g_pd3dDevice->GetLight(dwLightIndex, &light);
			hr = m_pEffect->SetValue(pConst->m_hParameter, &light.Diffuse, sizeof(D3DCOLORVALUE));
		}
		return hr;
	}

	//ENU_MSEFFECT_ENVAMB		
	HRESULT EffectParameterSettingTool::SetEnvAmbient(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		D3DXVECTOR4 vecAmb = *(D3DXVECTOR4*)&g_GetRenderGlobalState().GetSunLightParam().sceneAmbient;
		hr = m_pEffect->SetValue(pConst->m_hParameter, &vecAmb, sizeof(D3DXVECTOR4));
		return hr;
	}

	//ENU_MSEFFECT_SUNDIRMATRIX
	HRESULT EffectParameterSettingTool::SetSunDirMatrix(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		return hr;
	}


	EffectParameterSettingTool::EffectParameterSettingTool()
	{
		m_pFunctions[ENU_MSEFFECT_WORLD] = SetWorldMatrix;
		m_pFunctions[ENU_MSEFFECT_WORLDI] = SetWorldIMatrix;
		m_pFunctions[ENU_MSEFFECT_VIEW] = SetViewMatrix;
		m_pFunctions[ENU_MSEFFECT_PROJECTION] = SetProjMatrix;
		m_pFunctions[ENU_MSEFFECT_WORLDVIEW] = SetWorldViewMatrix;
		m_pFunctions[ENU_MSEFFECT_WORLDVIEWPROJ] = SetWorldViewProjMatrix;
		m_pFunctions[ENU_MSEFFECT_VIEWI] = SetViewIMatrix;
		m_pFunctions[ENU_MSEFFECT_WORLDIT] = SetWorldITMatrix;
		m_pFunctions[ENU_MSEFFECT_VIEWIT] = SetViewITMatrix;
		m_pFunctions[ENU_MSEFFECT_PROJECTIONI] = SetProjIMatrix;
		m_pFunctions[ENU_MSEFFECT_PROJECTIONIT] = SetProjITMatrix;
		m_pFunctions[ENU_MSEFFECT_WORLDVIEWI] = SetWorldViewIMatrix;
		m_pFunctions[ENU_MSEFFECT_WORLDVIEWIT] = SetWorldViewITMatrix;
		m_pFunctions[ENU_MSEFFECT_WORLDVIEWPROJI] = SetWorldViewProjIMatrix;
		m_pFunctions[ENU_MSEFFECT_WORLDVIEWPROJIT] = SetWorldViewProjITMatrix;
		m_pFunctions[ENU_MSEFFECT_VIEWPROJ] = SetViewProjMatrix;
		m_pFunctions[ENU_MSEFFECT_POSITION] = SetPos;
		m_pFunctions[ENU_MSEFFECT_DIRECTION] = SetDir;
		m_pFunctions[ENU_MSEFFECT_COLORRGBA] = SetColor;
		m_pFunctions[ENU_MSEFFECT_VOID] = SetVoid;
		m_pFunctions[ENU_MSEFFECT_BOOL] = SetBOOL;
		m_pFunctions[ENU_MSEFFECT_INT] = SetInt;
		m_pFunctions[ENU_MSEFFECT_FLOAT] = SetFloat;
		m_pFunctions[ENU_MSEFFECT_FLOAT2] = SetFloat2;
		m_pFunctions[ENU_MSEFFECT_FLOAT3] = SetFloat3;
		m_pFunctions[ENU_MSEFFECT_FLOAT4] = SetFloat4;
		m_pFunctions[ENU_MSEFFECT_TEXTURE] = SetTexture;
		m_pFunctions[ENU_MSEFFECT_TEXTURE1D] = SetTexture1D;
		m_pFunctions[ENU_MSEFFECT_TEXTURE2D] = SetTexture2D;
		m_pFunctions[ENU_MSEFFECT_TEXTURE3D] = SetTexture3D;
		m_pFunctions[ENU_MSEFFECT_TEXTURECUBE] = SetTextureCube;
		m_pFunctions[ENU_MSEFFECT_TEXTUREVOLUME] = SetTextureVolume;
		m_pFunctions[ENU_MSEFFECT_MATERIALAMBIENT] = SetAmbient;
		m_pFunctions[ENU_MSEFFECT_MATERIALDIFFUSE] = SetDiffuse;
		m_pFunctions[ENU_MSEFFECT_MATERIALSPECULAR] = SetSpecular;
		m_pFunctions[ENU_MSEFFECT_MATERIALEMISSIVE] = SetEmissive;
		m_pFunctions[ENU_MSEFFECT_MATERIALPOWER] = SetPower;
		m_pFunctions[ENU_MSEFFECT_LOCALBBOXMIN] = SetBBoxMin;
		m_pFunctions[ENU_MSEFFECT_LOCALBBOXMAX] = SetBBoxMax;
		m_pFunctions[ENU_MSEFFECT_LOCALBBOXSIZE] = SetBoxSize;
		m_pFunctions[ENU_MSEFFECT_STRING] = SetString;
		m_pFunctions[ENU_MSEFFECT_LIGHTDIR0] = SetLightDir;
		m_pFunctions[ENU_MSEFFECT_LIGHTDIR1] = SetLightDir;
		m_pFunctions[ENU_MSEFFECT_LIGHTDIR2] = SetLightDir;
		m_pFunctions[ENU_MSEFFECT_LIGHTDIR3] = SetLightDir;
		m_pFunctions[ENU_MSEFFECT_LIGHTDIR4] = SetLightDir;
		m_pFunctions[ENU_MSEFFECT_LIGHTDIR5] = SetLightDir;
		m_pFunctions[ENU_MSEFFECT_LIGHTDIR6] = SetLightDir;
		m_pFunctions[ENU_MSEFFECT_LIGHTDIR7] = SetLightDir;

		m_pFunctions[ENU_MSEFFECT_LIGHTDIFF0] = SetLightDiff;
		m_pFunctions[ENU_MSEFFECT_LIGHTDIFF1] = SetLightDiff;
		m_pFunctions[ENU_MSEFFECT_LIGHTDIFF2] = SetLightDiff;
		m_pFunctions[ENU_MSEFFECT_LIGHTDIFF3] = SetLightDiff;
		m_pFunctions[ENU_MSEFFECT_LIGHTDIFF4] = SetLightDiff;
		m_pFunctions[ENU_MSEFFECT_LIGHTDIFF5] = SetLightDiff;
		m_pFunctions[ENU_MSEFFECT_LIGHTDIFF6] = SetLightDiff;
		m_pFunctions[ENU_MSEFFECT_LIGHTDIFF7] = SetLightDiff;

		m_pFunctions[ENU_MSEFFECT_ENVAMB] = SetEnvAmbient;
		m_pFunctions[ENU_MSEFFECT_SUNDIRMATRIX] = SetSunDirMatrix;

		m_pFunctions[ENU_MSEFFECT_FOGINFO] = SetFogInfo;
		m_pFunctions[ENU_MSEFFECT_FOGCOLOR] = SetFogColor;
	}

	void EffectParameterSettingTool::SetEffect(LPD3DXEFFECT pEffect)
	{
		ASSERT(pEffect);
		m_pEffect = pEffect; 
		memset(m_bMatrixSet, 0, sizeof(BOOL) * ENUM_MATRIX_COUNT);
	}

	//ENU_MSEFFECT_FOGINFO
	HRESULT EffectParameterSettingTool::SetFogInfo(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		static D3DXVECTOR4 FogInfo(0,0,0,0);
		hr = g_pd3dDevice->GetRenderState(D3DRS_FOGSTART,(DWORD*)&FogInfo.x);
		hr = g_pd3dDevice->GetRenderState(D3DRS_FOGEND,(DWORD*)&FogInfo.y);
		hr = g_pd3dDevice->GetRenderState(D3DRS_FOGDENSITY,(DWORD*)&FogInfo.z);
		hr = m_pEffect->SetVector(pConst->m_hParameter, &FogInfo);
		return hr;
	}

	//ENU_MSEFFECT_FOGCOLOR
	HRESULT EffectParameterSettingTool::SetFogColor(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst)
	{
		HRESULT hr = E_FAIL;
		DWORD dwFogColor;
		D3DCOLORVALUE Value;
		hr = g_pd3dDevice->GetRenderState(D3DRS_FOGCOLOR, &dwFogColor);
		Value = Color2Value(dwFogColor);
		hr = m_pEffect->SetVector(pConst->m_hParameter, (D3DXVECTOR4*)&Value);
		return hr;
	}


	void* KG3DMaterial::KG3DMaterialSubset::GetMtlOption(DWORD dwType, DWORD dwExtInfo)
	{
		void* pOption = NULL;
		_MtlOption* pNewOption = NULL;
		HRESULT hr = E_FAIL;
		if (dwExtInfo != 0xffffffff)
		{
			KG_PROCESS_ERROR(dwExtInfo < 8);
		}

		pOption = FindOption(dwType, dwExtInfo);
		if (!pOption)
		{
			pNewOption = new _MtlOption;
			KGLOG_PROCESS_ERROR(pNewOption);

			pNewOption->Type = dwType;
			pOption = pNewOption;

			switch(dwType)
			{
			case OPTION_MATERIAL_ZFUN:
				{
					pNewOption->pData = new DWORD;
					KG_PROCESS_ERROR(pNewOption->pData);
					*(DWORD*)(pNewOption->pData) = D3DCMP_LESSEQUAL;
					break;
				}
			}

			if (dwExtInfo == 0xffffffff)
			{
				m_pMatOptions.push_back(pNewOption);
				m_dwNumOptions++;
			}
			else
			{
				KG_PROCESS_ERROR(m_lpTextures[dwExtInfo]);
				m_pTextureOptions[dwExtInfo].push_back(pNewOption);
				m_pTextureNumOption[dwExtInfo]++;
			}

		}

		hr = S_OK;

Exit0:
		if (FAILED(hr))
		{
			SAFE_DELETE(pNewOption);
		}
		return pOption;
	}

	void* KG3DMaterial::KG3DMaterialSubset::FindOption(DWORD dwType, DWORD dwExtInfo)
	{
		void* pReturn = NULL;
		if (dwExtInfo == 0xffffffff)
		{
			//material info
			for(size_t i = 0; i < m_pMatOptions.size(); i++)
			{
				if (m_pMatOptions[i]->Type == dwType)
				{
					pReturn = m_pMatOptions[i];
					break;
				}
			}
		}
		else
		{
			KG_PROCESS_ERROR(dwExtInfo < 8);
			KG_PROCESS_ERROR(m_lpTextures[dwExtInfo]);

			for (size_t i  = 0; i < m_pTextureOptions[dwExtInfo].size(); i++)
			{
				if (m_pTextureOptions[dwExtInfo][i]->Type == dwType)
				{
					pReturn = m_pTextureOptions[dwExtInfo][i];
					break;
				}
			}

		}

Exit0:
		return pReturn;
	}

	void KG3DMaterial::CopyOption(DWORD dwType, DWORD dwExtInfo, unsigned int nSrc, unsigned int nDest)
	{
		_MtlOption* pSrc = NULL;
		_MtlOption* pDest = NULL;

		KG_PROCESS_ERROR(nSrc < m_dwNumMaterials);
		KG_PROCESS_ERROR(nDest < m_dwNumMaterials);
		KG_PROCESS_ERROR(nSrc != nDest);

		pSrc = static_cast<_MtlOption*>(m_pMaterials[nSrc].GetMtlOption(dwType, dwExtInfo));
		KG_PROCESS_ERROR(pSrc);

		pDest = static_cast<_MtlOption*>(m_pMaterials[nDest].GetMtlOption(dwType, dwExtInfo));
		KG_PROCESS_ERROR(pDest);

		pDest->Type = pSrc->Type;

		switch(pDest->Type)
		{
		case OPTION_MATERIAL_ZFUN:
			{
				memcpy(pDest->pData, pSrc->pData, sizeof(DWORD));
				break;
			}
		}
Exit0:
		return;
	}


	void KG3DMaterial::Helper_MoveEffectParameter2SubMaterial()
	{
		for(DWORD i = 0; i < m_dwNumMaterials; i++)
		{
			int nIndex = m_pMaterials[i].m_nEffParaIndex;
			if (nIndex == -1)
				continue;

			std::vector<_ShaderConstant*>* pConsts = m_EffParaSets[nIndex];
			for (size_t j = 0; j < pConsts->size(); j++)
			{
				//KG3DTexture* pTexture = NULL;
				if (!strcmp((*pConsts)[j]->scName.c_str(), "ambientMap"))
				{
					_TextureData* pData = static_cast<_TextureData*>((*pConsts)[j]->pData);
					m_pMaterials[i].m_scTextureName[1] = pData->FileName;
					m_pMaterials[i].m_scTextureType[1] = "*MAP_AMBIENT";
					if (m_pMaterials[i].m_dwNumUsedTexture < 2)
					{
						m_pMaterials[i].m_dwNumUsedTexture = 2;
					}
					continue;
				}
				if (!strcmp((*pConsts)[j]->scName.c_str(), "specMap"))
				{
					_TextureData* pData = static_cast<_TextureData*>((*pConsts)[j]->pData);
					m_pMaterials[i].m_scTextureName[2] = pData->FileName;
					m_pMaterials[i].m_scTextureType[2] = "*MAP_SPECULAR";
					if (m_pMaterials[i].m_dwNumUsedTexture < 3)
					{
						m_pMaterials[i].m_dwNumUsedTexture = 3;
					}
					continue;
				}
			}
		}
	}


	void KG3DMaterial::SetShaderType(DWORD dwType)
	{
		m_ShaderType = dwType;
		SetSubsetPassID();
	}

	DWORD KG3DMaterial::GetShaderType()
	{
		return m_ShaderType;
	}

	HRESULT KG3DMaterial::LoadShaderForSubset(unsigned int uIndex, 
		LPCSTR strFileName)
	{
		HRESULT hr = E_FAIL;
		TCHAR strFileNamelwr[MAX_PATH];
		BOOL bLoaded = FALSE;
		int nNumSetCleared = 0;
		DWORD dwIndex = uIndex;

		strFileNamelwr[0] = '\0';
		KG_PROCESS_ERROR(strFileName);
		KG_PROCESS_ERROR(uIndex < m_dwNumMaterials);

		strcpy_s(strFileNamelwr, MAX_PATH, strFileName);
		_strlwr_s(strFileNamelwr, MAX_PATH);

		for (DWORD i = 0; i < m_dwNumMaterials; i++)
		{
			if (i == uIndex)
				continue;

			if (!strcmp(m_pMaterials[i].strEffectFileName, strFileNamelwr))
			{
				SAFE_RELEASE(m_pMaterials[uIndex].m_pEffect);
				m_pMaterials[uIndex].m_pEffect = m_pMaterials[i].m_pEffect;
				m_pMaterials[uIndex].m_pEffect->AddRef();
				strcpy(m_pMaterials[uIndex].strEffectFileName, strFileNamelwr);
				bLoaded = TRUE;
				break;
			}
		}

		if (!bLoaded)
		{
			hr = m_pMaterials[uIndex].LoadEffectFromFile(strFileNamelwr);
			if (FAILED(hr))
			{
				KGLogPrintf(KGLOG_ERR, "KG3DMaterial::LoadShaderForSubset(%s) failed.", strFileName);
				KG_COM_PROCESS_ERROR(hr);
			}
		}

		//这里是故意的, 之后马上调用ClearUnusedEffectParameterSet()把它恢复成正确值
		m_pMaterials[uIndex].m_nEffParaIndex = static_cast<int>(GetNumEffectParameterSets());
		ClearUnusedEffectParameterSet(nNumSetCleared);

		Effect_OnLoad(m_pMaterials[uIndex].m_nEffParaIndex, &m_pMaterials[uIndex]);

		g_S2TTool.Apply(this, 1, &dwIndex);

		hr = S_OK;
Exit0:
		if (FAILED(hr))
		{
			KGLogPrintf(KGLOG_ERR, 
				"KG3DMaterial::LoadShaderForSubset(%d, %s)", 
				uIndex, 
				strFileNamelwr);
		}
		return hr;
	}

	Semantic2TextureTool::Semantic2TextureTool()
	{
		//这里的semantic都要是小写
		Semantic2TextureFinder diffuse = {"diffusemap", KG3DMaterial_Helper_AutoMapFinder::FindDiffuseMapForEffect};
		Semantic2TextureFinder ambient = {"ambientmap", KG3DMaterial_Helper_AutoMapFinder::FindAmbientMapForEffect};
		Semantic2TextureFinder specular = {"specularmap", KG3DMaterial_Helper_AutoMapFinder::FindSpcularMapForEffect};
		Semantic2TextureFinder normal = {"spectexture", KG3DMaterial_Helper_AutoMapFinder::FindNormalMapForEffect};
		m_vecSemantic2MapFinder.push_back(diffuse);
		m_vecSemantic2MapFinder.push_back(ambient);
		m_vecSemantic2MapFinder.push_back(specular);
		m_vecSemantic2MapFinder.push_back(normal);
	}



	HRESULT Semantic2TextureTool::AutoFindMapsBySemantics(KG3DMaterial* pMaterial, 
		int nIndex, 
		const TCHAR* strSemantic, 
		TCHAR* strRetFileName)
	{
		HRESULT hr = E_FAIL;
		TCHAR strLowcaseSeman[256];
		size_t uNumFinder = m_vecSemantic2MapFinder.size();
		KGLOG_PROCESS_ERROR(pMaterial);
		KGLOG_PROCESS_ERROR(strSemantic);

		strcpy(strLowcaseSeman, strSemantic);
		_strlwr(strLowcaseSeman);

		for(unsigned int i = 0; i < uNumFinder; i++)
		{
			if(!strcmp(m_vecSemantic2MapFinder[i].strSemantic, strLowcaseSeman))		
			{
				m_vecSemantic2MapFinder[i].pFunc(pMaterial, nIndex, strRetFileName);
				hr = S_OK;
				break;
			}	
		}
Exit0:
		return hr;
	}

	HRESULT Semantic2TextureTool::Apply(KG3DMaterial *pMaterial, 
		int nNumToSet, 
		DWORD* pIndices)
	{
		HRESULT hr = E_FAIL;
		D3DXPARAMETER_DESC desc;
		TCHAR strAutoTextureFileName[MAX_PATH];
		strAutoTextureFileName[0] = '\0';
		KG3DTexture* pAutoFindTexture = NULL;

		KGLOG_PROCESS_ERROR(pMaterial);

		if (nNumToSet)
		{
			KGLOG_PROCESS_ERROR(pIndices);

			for (int i = 0; i < nNumToSet; i++)
			{
				if (pIndices[i] >= pMaterial->m_dwNumMaterials)
				{
					KGLogPrintf(KGLOG_ERR, "Semantic2TextureTool::Apply() Invalidate subset index.");
					continue;
				}

				KG3DMaterial::KG3DMaterialSubset& SubMat = pMaterial->m_pMaterials[pIndices[i]];
				LPD3DXEFFECT pd3dEffect = SubMat.m_pEffect;

				std::vector<KG3DMaterial::_ShaderConstant*>& vecConsts = *(pMaterial->m_EffParaSets[SubMat.m_nEffParaIndex]);
				for(size_t j = 0; j < vecConsts.size(); j++)
				{
					pd3dEffect->GetParameterDesc(vecConsts[j]->scName.c_str(), &desc);
					switch(vecConsts[j]->dwType)
					{
					case ENU_MSEFFECT_TEXTURE:
					case ENU_MSEFFECT_TEXTURE1D:
					case ENU_MSEFFECT_TEXTURE2D:
					case ENU_MSEFFECT_TEXTURE3D:
					case ENU_MSEFFECT_TEXTUREVOLUME:
					case ENU_MSEFFECT_TEXTURECUBE:
						{
							_TextureData* pTextureData = reinterpret_cast<_TextureData*>(vecConsts[j]->pData);
							hr = AutoFindMapsBySemantics(pMaterial, pIndices[i], desc.Semantic, strAutoTextureFileName);
							if(FAILED(hr))
								continue;

							hr = g_cTextureTable.LoadResourceFromFile(strAutoTextureFileName, 0, TEXOP_RELOAD,
								(IKG3DResourceBase **)&pAutoFindTexture);
							if(SUCCEEDED(hr))
							{
								KG3DTexture* pTextureToRelease = NULL;
								hr = g_cTextureTable.GetResource(&pTextureToRelease, pTextureData->TextureID);
								if (SUCCEEDED(hr))
								{
									pTextureToRelease->Release();
									SAFE_RELEASE(pTextureToRelease);
								}

								pTextureData->TextureID = pAutoFindTexture->GetID();
								pTextureData->FileName = strAutoTextureFileName;
							}
							break;
						}
					}
				}
			}
		}
		else
		{
			//如果没有指定个数，为所有的Subset搜索对应贴图
			for (DWORD i = 0; i < pMaterial->m_dwNumMaterials; i++)
			{
				KG3DMaterial::KG3DMaterialSubset& SubMat = pMaterial->m_pMaterials[i];
				LPD3DXEFFECT pd3dEffect = SubMat.m_pEffect;
				std::vector<KG3DMaterial::_ShaderConstant*>& vecConsts = *(pMaterial->m_EffParaSets[SubMat.m_nEffParaIndex]);
				for(size_t j = 0; j < vecConsts.size(); j++)
				{
					pd3dEffect->GetParameterDesc(vecConsts[j]->scName.c_str(), &desc);
					switch(vecConsts[j]->dwType)
					{
					case ENU_MSEFFECT_TEXTURE:
					case ENU_MSEFFECT_TEXTURE1D:
					case ENU_MSEFFECT_TEXTURE2D:
					case ENU_MSEFFECT_TEXTURE3D:
					case ENU_MSEFFECT_TEXTUREVOLUME:
					case ENU_MSEFFECT_TEXTURECUBE:
						{
							_TextureData* pTextureData = reinterpret_cast<_TextureData*>(vecConsts[j]->pData);
							hr = AutoFindMapsBySemantics(pMaterial, i, desc.Semantic, strAutoTextureFileName);
							if(FAILED(hr))
								continue;

							hr = g_cTextureTable.LoadResourceFromFile(strAutoTextureFileName, 0, TEXOP_RELOAD,
								(IKG3DResourceBase **)&pAutoFindTexture);
							if(SUCCEEDED(hr))
							{
								KG3DTexture* pTextureToRelease = NULL;
								hr = g_cTextureTable.GetResource(&pTextureToRelease, pTextureData->TextureID);
								if (SUCCEEDED(hr))
								{
									pTextureToRelease->Release();
									SAFE_RELEASE(pTextureToRelease);
								}

								pTextureData->TextureID = pAutoFindTexture->GetID();
								pTextureData->FileName = strAutoTextureFileName;
							}
							break;
						}
					}
				}
			}
		}

		hr = S_OK;
Exit0:
		return hr;
	}


	HRESULT KG3DMaterial::ClearUnusedEffectParameterSet(int& nNumSetCleared)
	{
		HRESULT hr = S_OK;

		std::map<int, int> usedEffParaSet;
		size_t size = m_EffParaSets.size();
		nNumSetCleared = 0;
		for(unsigned int k = 0; k < size; k++)
		{
			usedEffParaSet[k] = 0;
		}
		for(DWORD i = 0; i < m_dwNumMaterials; i++)
		{
			//set the used effect parameter set index flag
			if(m_pMaterials[i].m_nEffParaIndex != -1)
				usedEffParaSet[m_pMaterials[i].m_nEffParaIndex]++;

		}
		int l = static_cast<int>(usedEffParaSet.size() - 1);
		while(l >= 0)
		{
			if(usedEffParaSet[l] == 0)
			{
				nNumSetCleared++;
				for(DWORD j = 0; j < m_dwNumMaterials; j++)
				{
					//这里修正在使用将被删掉的ParaSet之后的ParaSet的Subset的Index值
					if(m_pMaterials[j].m_nEffParaIndex > l) 
					{
						ASSERT(m_pMaterials[j].m_nEffParaIndex != -1);
						m_pMaterials[j].m_nEffParaIndex--;
					}
				}
				for(size_t k = 0; k < m_EffParaSets[l]->size(); k++)
				{
					delete (*m_EffParaSets[l])[k];
				}
				m_EffParaSets[l]->clear();
				SAFE_DELETE(m_EffParaSets[l]);
			}
			l--;
		};

		for (int i = static_cast<int>(m_EffParaSets.size()) - 1; i >= 0; i--)
		{
			if (usedEffParaSet[i] == 0)
			{
				std::vector<std::vector<_ShaderConstant*>*>::iterator it = m_EffParaSets.begin();
				advance(it, i);
				m_EffParaSets.erase(it);
			}
		}
		return hr;
	}

	HRESULT KG3DMaterial::ClearShaderForSubset(unsigned int uIndex)
	{
		HRESULT hr = E_FAIL;
		int nNumSetCleared = 0;
		KG_PROCESS_ERROR(uIndex < m_dwNumMaterials);

		KG3DMaterialSubset& SubMat = m_pMaterials[uIndex];
		SubMat.strEffectFileName[0] = '\0';
		KG_PROCESS_SUCCESS(!SubMat.m_pEffect);


		g_cEffectFileTable.Release(SubMat.m_pEffect);
		m_ShaderType = MAT_SHADER_TYPE_NONE;

		ClearUnusedEffectParameterSet(nNumSetCleared);

Exit1:	
		hr = S_OK;
Exit0:
		return hr;
	}

	HRESULT KG3DMaterial::SetSubsetTexture(unsigned int uSubsetID, 
		unsigned int uStageID,
		KG3DTexture* pTexture)
	{
		HRESULT hr = E_FAIL;
		KG_PROCESS_ERROR(pTexture);
		KG_PROCESS_ERROR(m_pMaterials);
		KG_PROCESS_ERROR(uSubsetID < m_dwNumMaterials);
		KG_PROCESS_ERROR(uStageID < KG3DMaterialSubset::cNumID);

		if (m_pMaterials[uSubsetID].m_lpTextures[uStageID])
		{
			SAFE_RELEASE(m_pMaterials[uSubsetID].m_lpTextures[uStageID]);
		}

		m_pMaterials[uSubsetID].m_lpTextures[uStageID] = pTexture;
		m_pMaterials[uSubsetID].m_scTextureName[uStageID] = pTexture->m_scName;

		hr = S_OK;
Exit0:
		return hr;
	}

	HRESULT KG3DMaterial::CreateSubset(DWORD dwNumSubset)
	{
		HRESULT hr = E_FAIL;
		if (m_pMaterials != NULL)
		{
			for (size_t i = 0; i < m_dwNumMaterials; i++)
				if (g_pSubMatLast == (&(m_pMaterials[i])))
					g_pSubMatLast = NULL;
		}
		SAFE_DELETE_ARRAY(m_pMaterials);
		m_dwNumMaterials = 0;

		KG_PROCESS_ERROR(dwNumSubset > 0);

		m_pMaterials = new KG3DMaterialSubset[dwNumSubset];
		KG_PROCESS_ERROR(m_pMaterials);

		m_dwNumMaterials = dwNumSubset;

		hr = S_OK;
Exit0:
		return hr;
	}

	////////////////////////////////////////////////////////////////////////////////
	HRESULT KG3DMaterial::GetEffParaSetsVectorItemVectorSize(DWORD dwPosition, DWORD *pdwSize)
	{
		HRESULT hResult = E_FAIL;
		DWORD dwEffParaSetsVectorSize = 0;
		KGLOG_PROCESS_ERROR(pdwSize);

		dwEffParaSetsVectorSize = (DWORD)m_EffParaSets.size();
		KGLOG_PROCESS_ERROR(dwPosition < dwEffParaSetsVectorSize);

		std::vector<KG3DMaterial::_ShaderConstant*> *pvecConsts = m_EffParaSets[dwPosition];
		*pdwSize = (DWORD)pvecConsts->size();
		hResult = S_OK;
Exit0:
		return hResult;
	}

	HRESULT KG3DMaterial::GetEffParaSetsVectorItemShadeItemName(
		DWORD dwEffParaSetsIndex,
		DWORD dwShaderIndex, 
		LPCTSTR *ppszName
		)
	{
		HRESULT hResult = E_FAIL;
		DWORD dwEffParaSetsVectorSize = 0;
		DWORD dwShaderSize = 0;
		KGLOG_PROCESS_ERROR(ppszName);

		dwEffParaSetsVectorSize = (DWORD)m_EffParaSets.size();
		KGLOG_PROCESS_ERROR(dwEffParaSetsIndex < dwEffParaSetsVectorSize);

		std::vector<KG3DMaterial::_ShaderConstant*> &vecConsts = *(m_EffParaSets[dwEffParaSetsIndex]);
		dwShaderSize = (DWORD)vecConsts.size();
		KGLOG_PROCESS_ERROR(dwShaderIndex < dwShaderSize);

		*ppszName = (vecConsts[dwShaderIndex])->scName.c_str();

		hResult = S_OK;
Exit0:
		return hResult;
	}
	HRESULT KG3DMaterial::GetEffParaSetsVectorItemShadeItemType(
		DWORD dwEffParaSetsIndex,
		DWORD dwShaderIndex, 
		DWORD *pdwType
		)
	{
		HRESULT hResult = E_FAIL;
		DWORD dwEffParaSetsVectorSize = 0;
		DWORD dwShaderSize = 0;
		KGLOG_PROCESS_ERROR(pdwType);

		dwEffParaSetsVectorSize = (DWORD)m_EffParaSets.size();
		KGLOG_PROCESS_ERROR(dwEffParaSetsIndex < dwEffParaSetsVectorSize);

		std::vector<KG3DMaterial::_ShaderConstant*> &vecConsts = *(m_EffParaSets[dwEffParaSetsIndex]);
		dwShaderSize = (DWORD)vecConsts.size();
		KGLOG_PROCESS_ERROR(dwShaderIndex < dwShaderSize);

		*pdwType = (vecConsts[dwShaderIndex])->dwType;

		hResult = S_OK;
Exit0:
		return hResult;

	}
	HRESULT KG3DMaterial::GetEffParaSetsVectorItemShadeItemData(
		DWORD dwEffParaSetsIndex,
		DWORD dwShaderIndex, 
		PVOID *ppvData
		)
	{
		HRESULT hResult = E_FAIL;
		DWORD dwEffParaSetsVectorSize = 0;
		DWORD dwShaderSize = 0;
		KGLOG_PROCESS_ERROR(ppvData);

		dwEffParaSetsVectorSize = (DWORD)m_EffParaSets.size();
		KGLOG_PROCESS_ERROR(dwEffParaSetsIndex < dwEffParaSetsVectorSize);

		std::vector<KG3DMaterial::_ShaderConstant*> &vecConsts = *(m_EffParaSets[dwEffParaSetsIndex]);
		dwShaderSize = (DWORD)vecConsts.size();
		KGLOG_PROCESS_ERROR(dwShaderIndex < dwShaderSize);

		*ppvData = (vecConsts[dwShaderIndex])->pData;

		hResult = S_OK;
Exit0:
		return hResult;
	}
	HRESULT KG3DMaterial::GetSubMaterialItemEffectString(DWORD dwIndex, LPCTSTR pszName, LPCTSTR *ppszString)
	{
		HRESULT hResult = E_FAIL;
		LPD3DXEFFECT pd3dEffect = NULL;
		D3DXHANDLE handle = 0;
		LPCTSTR strFxDesc = NULL;
		KGLOG_PROCESS_ERROR(ppszString);
		KGLOG_PROCESS_ERROR(dwIndex < m_dwNumMaterials);

		pd3dEffect = (m_pMaterials[dwIndex]).m_pEffect;

		handle = pd3dEffect->GetAnnotationByName(pszName, "UIName");
		KGLOG_PROCESS_ERROR(handle);
		pd3dEffect->GetString(handle, &strFxDesc);
		*ppszString = strFxDesc;

		hResult = S_OK;
Exit0:
		return hResult;
	}
	HRESULT KG3DMaterial::GetName(LPCTSTR *ppszName)
	{
		HRESULT hResult = E_FAIL;

		KGLOG_PROCESS_ERROR(ppszName);

		*ppszName = m_scName.c_str();

		hResult = S_OK;
Exit0:
		return hResult;
	}



	HRESULT KG3DMaterial::GetMaterialCount(DWORD *pdwCount)
	{
		HRESULT hResult = E_FAIL;

		KGLOG_PROCESS_ERROR(pdwCount);

		*pdwCount = m_dwNumMaterials;

		hResult = S_OK;
Exit0:
		return hResult;
	}
	HRESULT KG3DMaterial::GetSubMaterialItemAlphaOp(DWORD dwIndex, _MtlAlphaOperation **ppAlphaOp)
	{
		HRESULT hResult = E_FAIL;

		KGLOG_PROCESS_ERROR(ppAlphaOp);
		KGLOG_PROCESS_ERROR(dwIndex < m_dwNumMaterials);

		*ppAlphaOp = m_pMaterials[dwIndex].GetAlphaOperation();

		hResult = S_OK;
Exit0:
		return hResult;
	}

	static void ColorRef2Value(COLORREF rgb, D3DCOLORVALUE& Color)
	{
		Color.a = ((rgb & 0xff000000)>>24) / 255.0f;
		Color.r = (rgb & 0x000000ff) / 255.0f;
		Color.g = ((rgb & 0x0000ff00)>>8) / 255.0f;
		Color.b = ((rgb & 0x00ff0000)>>16) / 255.0f;
	}

	HRESULT KG3DMaterial::SetSubMaterialColorRef2Value(
		DWORD dwIndex, 
		DWORD dwAmbientColor,
		DWORD dwDiffuseColor,
		DWORD dwEmissive,
		DWORD dwSpecular
		)
	{
		HRESULT hResult = E_FAIL;
		KGLOG_PROCESS_ERROR(dwIndex < m_dwNumMaterials);

		KG3DMaterial::KG3DMaterialSubset& SubMat = m_pMaterials[dwIndex];

		ColorRef2Value(dwAmbientColor, SubMat.m_sMaterial9.Ambient);
		ColorRef2Value(dwDiffuseColor, SubMat.m_sMaterial9.Diffuse);
		ColorRef2Value(dwEmissive, SubMat.m_sMaterial9.Emissive);
		ColorRef2Value(dwSpecular, SubMat.m_sMaterial9.Specular);

		hResult = S_OK;
Exit0:
		return hResult;
	}

	HRESULT KG3DMaterial::GetSubMaterialOption(DWORD dwIndex, DWORD *pdwOption)
	{
		HRESULT hResult = E_FAIL;
		KG_PROCESS_ERROR(pdwOption);
		KGLOG_PROCESS_ERROR(dwIndex < m_dwNumMaterials);

		*pdwOption = m_pMaterials[dwIndex].m_dwOption;

		hResult = S_OK;
Exit0:
		return hResult;
	}

	HRESULT KG3DMaterial::SetSubMaterialOption(DWORD dwIndex, DWORD dwOption)
	{
		HRESULT hResult = E_FAIL;
		KGLOG_PROCESS_ERROR(dwIndex < m_dwNumMaterials);

		m_pMaterials[dwIndex].m_dwOption = dwOption;

		hResult = S_OK;
Exit0:
		return hResult;
	}

	HRESULT KG3DMaterial::GetSubMaterial(DWORD dwIndex, PVOID *ppSubMat)
	{
		HRESULT hResult = E_FAIL;
		KGLOG_PROCESS_ERROR(dwIndex < m_dwNumMaterials);
		KGLOG_PROCESS_ERROR(ppSubMat);

		*ppSubMat = static_cast<PVOID>(&(m_pMaterials[dwIndex]));

		hResult = S_OK;
Exit0:
		return hResult;
	}
	HRESULT KG3DMaterial::GetSubMaterialTextureID(DWORD dwIndex, DWORD *pdwTextureIDs)
	{
		HRESULT hResult = E_FAIL;
		KGLOG_PROCESS_ERROR(dwIndex < m_dwNumMaterials);
		KGLOG_PROCESS_ERROR(pdwTextureIDs);

		for (int i=0;i<8;i++)
		{
			if(m_pMaterials[dwIndex].m_lpTextures[i])
			{
				pdwTextureIDs[i] = m_pMaterials[dwIndex].m_lpTextures[i]->GetID();
			}
			else
			{
				pdwTextureIDs[i] = 0xFFFFFFFF;
			}
		}

		hResult = S_OK;
Exit0:
		return hResult;
	}

	BOOL KG3DMaterial::IsResourceReady()
	{
		BOOL bReady = IsLoaded();
		KG3DTexture* pTexture = NULL;

		if(!bReady)
			return bReady;

		for (DWORD i = 0; i < m_dwNumMaterials; i++)
		{
			KG3DMaterial::KG3DMaterialSubset& SubMat = m_pMaterials[i];
			for(int i = 0; i < 1; i++)
			{
				pTexture = SubMat.m_lpTextures[i];
				if (pTexture)
				{
					if(!pTexture->IsResourceReady())
					{
						bReady = FALSE;
						break;
					}
				}
				else
				{
					break;
				}
			}

		}

		return bReady;
	}

	HRESULT KG3DMaterial::RefreshOption(unsigned uOption)
	{
		HRESULT hResult  = E_FAIL;

		//KG_PROCESS_SUCCESS(m_uOption == uOption);

		//if(m_bInMultiThreadLoadList)
		//{
		//	/*int k =0*/;
		//}
		//else
		//{
		//	if (!IsResourceReady() && !(uOption & MLO_TEXTURE_MULTI))
		//	{
		//		hResult = ReloadTexture(uOption);
		//		KG_COM_PROCESS_ERROR(hResult);
		//	}
		//}

		m_uOption = uOption;

		//Exit1:
		hResult = S_OK;
		//Exit0:
		return hResult;
	}

	void KG3DMaterial::GetUseResource(vector<string>& vNames)
	{
		TCHAR strDriver[MAX_PATH];
		TCHAR strPath[MAX_PATH];
		TCHAR strTexturePath[MAX_PATH];

		if(!IsLoaded())
			return;

		_splitpath_s(m_scName.c_str(),
			strDriver,
			MAX_PATH,
			strPath,
			MAX_PATH,
			NULL,
			0,
			NULL,
			0);
		for (DWORD i = 0; i < m_dwNumMaterials; i++)
		{
			for (DWORD j = 0; j < m_pMaterials[i].m_dwNumUsedTexture; j++)
			{
				sprintf_s(strTexturePath, 
					MAX_PATH,
					"%s%s%s",
					strDriver,
					strPath,
					m_pMaterials[i].m_scTextureName[j].c_str());
				vNames.push_back(strTexturePath);
			}
		}
	}

	HRESULT KG3DMaterial::RemoveTexture(DWORD dwSubsetIndex, 
		DWORD dwIndex)
	{
		HRESULT hRetCode = E_FAIL;

		KG_PROCESS_ERROR(m_pMaterials);
		KG_PROCESS_ERROR(dwSubsetIndex < m_dwNumMaterials);
		KG3DMaterialSubset &Subset = m_pMaterials[dwSubsetIndex];

		KG_PROCESS_ERROR(dwIndex < KG3DMaterialSubset::cNumID);

		if (Subset.m_lpTextures[dwIndex])
		{
			SAFE_RELEASE(Subset.m_lpTextures[dwIndex]);
			Subset.m_scTextureName[dwIndex] = "";
		}
		hRetCode = S_OK;
Exit0:
		return hRetCode;
	}

	HRESULT KG3DMaterial::ChangeTexture(DWORD dwSubsetIndex, 
		DWORD dwIndex, 
		LPCSTR strNewTextureFileName)
	{
		HRESULT hRetCode = E_FAIL;
		HRESULT hResult  = E_FAIL;
		KG3DTexture *pNewTexture = NULL;
		TCHAR strRelativePath[MAX_PATH];
		KG_PROCESS_ERROR(m_pMaterials);
		KG_PROCESS_ERROR(dwSubsetIndex < m_dwNumMaterials);
		KG3DMaterialSubset &Subset = m_pMaterials[dwSubsetIndex];

		KG_PROCESS_ERROR(dwIndex < KG3DMaterialSubset::cNumID);

		if (Subset.m_lpTextures[dwIndex])
		{
			//如果有旧的贴图， 把旧的清掉
			SAFE_RELEASE(Subset.m_lpTextures[dwIndex]);
			Subset.m_scTextureName[dwIndex] = "";
			Subset.m_scTextureType[dwIndex] = "";
		}

		//加载新的贴图
		g_pFileFinder->GetRelativePath(strNewTextureFileName, 
			strRelativePath,
			MAX_PATH);

		hResult = g_cTextureTable.LoadResourceFromFile(strRelativePath, 
			0, 
			0,
			(IKG3DResourceBase**)&pNewTexture);
		KG_COM_PROCESS_ERROR(hResult);

		Subset.m_lpTextures[dwIndex] = pNewTexture;
		Subset.m_scTextureName[dwIndex] = strRelativePath;
		hRetCode = S_OK;
Exit0:
		return hRetCode;
	}

	HRESULT KG3DMaterial::GetTexturePathName(DWORD dwSubsetIndex,
		DWORD dwIndex,
		LPSTR strBuffer, 
		DWORD dwSize)
	{
		HRESULT hRetCode = E_FAIL;
		KG_PROCESS_ERROR(strBuffer);
		KG_PROCESS_ERROR(dwSize);
		KG_PROCESS_ERROR(m_pMaterials);
		strBuffer[0] = '\0';

		KG_PROCESS_ERROR(dwSubsetIndex < m_dwNumMaterials);

		KG3DMaterialSubset &Subset = m_pMaterials[dwSubsetIndex];
		KG_PROCESS_ERROR(dwIndex < KG3DMaterialSubset::cNumID);
		KG_PROCESS_ERROR(Subset.m_lpTextures[dwIndex]);

		strcpy_s(strBuffer, dwSize, Subset.m_scTextureName[dwIndex].c_str());
		hRetCode = S_OK;
Exit0:
		return hRetCode;
	}


	KG3DMaterialDetail::KG3DMaterialDetail()
	{
		m_cMainColor.r = m_cMainColor.g = m_cMainColor.b = m_cMainColor.a = 1;

		m_fMainColorMin = 0;
		m_fMainColorMax = 1;
		m_fMainColorScale = 1;
		m_fEnv = 1;
		m_fSpecular = 0.2F;

		m_szDetail0FileName = "";
		m_szDetail1FileName = "";

		m_lpDetailTexture0 = NULL;
		m_lpDetailTexture1 = NULL;
		m_lpChannelTexture = NULL;

		m_fDetail0UVScale = 1;
		m_fDetail1UVScale = 1;
		m_fDetail0ColorScale = 1;
		m_fDetail1ColorScale = 1;

		m_fDetail0Env      = 0;
		m_fDetail0Specular = 0;
		m_fDetail1Env      = 0;
		m_fDetail1Specular = 0;

		m_cDetial0Color = m_cMainColor;
		m_cDetial1Color = m_cMainColor;

	}

	KG3DMaterialDetail::~KG3DMaterialDetail()
	{
		SAFE_RELEASE(m_lpDetailTexture0);
		SAFE_RELEASE(m_lpDetailTexture1);
		SAFE_RELEASE(m_lpChannelTexture);

	}

	HRESULT KG3DMaterial::GetMaterilDetail(int nIndex,KG3DMaterialDetail* pDest)
	{
		KGLOG_PROCESS_ERROR(pDest);
		if(pDest && nIndex>=0 && nIndex< (int)m_vecDetailDefine.size())
		{
			KG3DMaterialDetail& vDetail = m_vecDetailDefine[nIndex];

			pDest->m_fMainColorMax   = vDetail.m_fMainColorMax;
			pDest->m_fMainColorMin   = vDetail.m_fMainColorMin;
			pDest->m_fMainColorScale = vDetail.m_fMainColorScale;
			pDest->m_cMainColor      = vDetail.m_cMainColor;
			pDest->m_fEnv            = vDetail.m_fEnv;
			pDest->m_fSpecular       = vDetail.m_fSpecular;

			pDest->m_fDetail0ColorScale = vDetail.m_fDetail0ColorScale;
			pDest->m_fDetail0UVScale    = vDetail.m_fDetail0UVScale;
			pDest->m_cDetial0Color      = vDetail.m_cDetial0Color;
			pDest->m_szDetail0FileName  = vDetail.m_szDetail0FileName;
			pDest->m_fDetail0Env        = vDetail.m_fDetail0Env;
			pDest->m_fDetail0Specular   = vDetail.m_fDetail0Specular;

			pDest->m_fDetail1ColorScale = vDetail.m_fDetail1ColorScale;
			pDest->m_fDetail1UVScale    = vDetail.m_fDetail1UVScale;
			pDest->m_cDetial1Color      = vDetail.m_cDetial1Color;
			pDest->m_szDetail1FileName  = vDetail.m_szDetail1FileName;
			pDest->m_fDetail1Env        = vDetail.m_fDetail1Env;
			pDest->m_fDetail1Specular   = vDetail.m_fDetail1Specular;

			pDest->m_szName = vDetail.m_szName;

			return S_OK;
		}
Exit0:
		return E_FAIL;
	}


	HRESULT KG3DMaterial::GetMaterilDetail(int nDetailType,LPSTR pszName,float* pfEnv,float* pfSpecular,
		float* pBodyMax,float* pBodyMin,float* pBodyScale,D3DCOLORVALUE* pBodyColor,LPSTR pChannelFile,
		float* pDetail0_ColorScale,float* pDetail0_UVScale,float* pfDetail0_Env,float* pfDetail0_Specular,D3DCOLORVALUE* pDetail0_Color,LPSTR pDetail0_File,
		float* pDetail1_ColorScale,float* pDetail1_UVScale,float* pfDetail1_Env,float* pfDetail1_Specular,D3DCOLORVALUE* pDetail1_Color,LPSTR pDetail1_File)
	{ 
		if(nDetailType>=0 && nDetailType< (int)m_vecDetailDefine.size())
		{
			KG3DMaterialDetail& vDetail = m_vecDetailDefine[nDetailType];

			(*pBodyMax)   = vDetail.m_fMainColorMax;
			(*pBodyMin)   = vDetail.m_fMainColorMin;
			(*pBodyScale) = vDetail.m_fMainColorScale;
			(*pBodyColor) = vDetail.m_cMainColor;
			(*pfEnv)      = vDetail.m_fEnv;
			(*pfSpecular) = vDetail.m_fSpecular;
			wsprintf(pChannelFile,"%s",vDetail.m_szChannelFileName.c_str());

			(*pDetail0_ColorScale) = vDetail.m_fDetail0ColorScale;
			(*pDetail0_UVScale)    = vDetail.m_fDetail0UVScale;
			(*pDetail0_Color)      = vDetail.m_cDetial0Color;
			(*pfDetail0_Env)       = vDetail.m_fDetail0Env;
			(*pfDetail0_Specular)  = vDetail.m_fDetail0Specular; 
			wsprintf(pDetail0_File,"%s",vDetail.m_szDetail0FileName.c_str());

			(*pDetail1_ColorScale) = vDetail.m_fDetail1ColorScale;
			(*pDetail1_UVScale)    = vDetail.m_fDetail1UVScale;
			(*pDetail1_Color)      = vDetail.m_cDetial1Color;
			(*pfDetail1_Env)       = vDetail.m_fDetail1Env;
			(*pfDetail1_Specular)  = vDetail.m_fDetail1Specular; 
			wsprintf(pDetail1_File,"%s",vDetail.m_szDetail1FileName.c_str());

			wsprintf(pszName,"%s",vDetail.m_szName.c_str());
			return S_OK;
		}

		return E_FAIL;
	}

	HRESULT KG3DMaterial::SetMaterilDetail(int nDetailType,LPSTR pszName,float fEnv,float fSpecular,
		float fBodyMax,float fBodyMin,float fBodyScale,D3DCOLORVALUE vBodyColor,LPSTR pChannelFile,
		float fDetail0_ColorScale,float fDetail0_UVScale,float fDetail0_Env,float fDetail0_Specular,D3DCOLORVALUE vDetail0_Color,LPSTR pDetail0_File,
		float fDetail1_ColorScale,float fDetail1_UVScale,float fDetail1_Env,float fDetail1_Specular,D3DCOLORVALUE vDetail1_Color,LPSTR pDetail1_File)
	{

		if(nDetailType>=0 && nDetailType< (int)m_vecDetailDefine.size())
		{
			KG3DMaterialDetail& vDetail = m_vecDetailDefine[nDetailType];

			vDetail.m_fMainColorMax   =  fBodyMax  ;
			vDetail.m_fMainColorMin   =  fBodyMin  ;
			vDetail.m_fMainColorScale =  fBodyScale;
			vDetail.m_cMainColor      =  vBodyColor;
			vDetail.m_fEnv            =  fEnv      ;
			vDetail.m_fSpecular       =  fSpecular ;
			vDetail.m_szChannelFileName  = pChannelFile;

			vDetail.m_fDetail0ColorScale = fDetail0_ColorScale;
			vDetail.m_fDetail0UVScale    = fDetail0_UVScale   ;
			vDetail.m_cDetial0Color      = vDetail0_Color     ;
			vDetail.m_fDetail0Env        = fDetail0_Env       ;
			vDetail.m_fDetail0Specular   = fDetail0_Specular  ;
			vDetail.m_szDetail0FileName  = pDetail0_File;

			vDetail.m_fDetail1ColorScale = fDetail1_ColorScale;
			vDetail.m_fDetail1UVScale    = fDetail1_UVScale   ;
			vDetail.m_cDetial1Color      = vDetail1_Color     ;
			vDetail.m_fDetail1Env        = fDetail1_Env       ;
			vDetail.m_fDetail1Specular   = fDetail1_Specular  ;
			vDetail.m_szDetail1FileName  = pDetail1_File;

			vDetail.m_szName = pszName;

			return S_OK;
		}

		return E_FAIL;
	}

	HRESULT KG3DMaterialDetail::LoadFormIni(IIniFile* pFile,LPSTR SectionName)
	{
		TCHAR szName   [MAX_PATH];
		TCHAR szChannel[MAX_PATH];
		TCHAR szDetail0[MAX_PATH];
		TCHAR szDetail1[MAX_PATH];

		pFile->GetMultiFloat(SectionName,"MainColor"        ,(float*)&m_cMainColor,4);
		pFile->GetFloat     (SectionName,"MainColorMin"     ,0                    ,&m_fMainColorMin);
		pFile->GetFloat     (SectionName,"MainColorMax"     ,1                    ,&m_fMainColorMax);
		pFile->GetFloat     (SectionName,"MainColorScale"   ,1                    ,&m_fMainColorScale);
		pFile->GetString    (SectionName,"ChannelFileName"  ,""                   ,szChannel,MAX_PATH);
		pFile->GetFloat     (SectionName,"Env"              ,1                    ,&m_fEnv           );
		pFile->GetFloat     (SectionName,"Specular"         ,1                    ,&m_fSpecular      );

		pFile->GetFloat     (SectionName,"Detail0UVScale"   ,0                    ,&m_fDetail0UVScale);
		pFile->GetFloat     (SectionName,"Detail0ColorScale",0                    ,&m_fDetail0ColorScale);
		pFile->GetFloat     (SectionName,"Detail0Env"       ,0                    ,&m_fDetail0Env);
		pFile->GetFloat     (SectionName,"Detail0Specular"  ,0                    ,&m_fDetail0Specular);
		pFile->GetMultiFloat(SectionName,"Detial0Color"     ,(float*)&m_cDetial0Color,4);
		pFile->GetString    (SectionName,"Detail0FileName"  ,""                   ,szDetail0,MAX_PATH);

		pFile->GetFloat     (SectionName,"Detail1UVScale"   ,0                    ,&m_fDetail1UVScale);
		pFile->GetFloat     (SectionName,"Detail1ColorScale",0                    ,&m_fDetail1ColorScale);
		pFile->GetFloat     (SectionName,"Detail1Env"       ,0                    ,&m_fDetail1Env);
		pFile->GetFloat     (SectionName,"Detail1Specular"  ,0                    ,&m_fDetail1Specular);
		pFile->GetMultiFloat(SectionName,"Detial1Color"     ,(float*)&m_cDetial1Color,4);
		pFile->GetString    (SectionName,"Detail1FileName"  ,""                   ,szDetail1,MAX_PATH);

		pFile->GetString    (SectionName,"Name"             ,""                   ,szName,MAX_PATH);

		m_szChannelFileName = szChannel;
		m_szDetail0FileName = szDetail0;
		m_szDetail1FileName = szDetail1;
		m_szName            = szName;

		return S_OK;
	}


	HRESULT KG3DMaterialDetail::SaveDefineToIni(IIniFile* pFile,LPSTR SectionName)
	{
		TCHAR szName   [MAX_PATH];
		TCHAR szChannel[MAX_PATH];
		TCHAR szDetail0[MAX_PATH];
		TCHAR szDetail1[MAX_PATH];

		wsprintf(szChannel,"%s",m_szChannelFileName.c_str());
		wsprintf(szDetail0,"%s",m_szDetail0FileName.c_str());
		wsprintf(szDetail1,"%s",m_szDetail1FileName.c_str());
		wsprintf(szName   ,"%s",m_szName.c_str()           );

		pFile->WriteMultiFloat(SectionName,"MainColor"        ,(float*)&m_cMainColor,4   );
		pFile->WriteFloat     (SectionName,"MainColorMin"     ,m_fMainColorMin           );
		pFile->WriteFloat     (SectionName,"MainColorMax"     ,m_fMainColorMax           );
		pFile->WriteFloat     (SectionName,"MainColorScale"   ,m_fMainColorScale         );
		pFile->WriteString    (SectionName,"ChannelFileName"  ,szChannel                 );
		pFile->WriteFloat     (SectionName,"Env"              ,m_fEnv                   );
		pFile->WriteFloat     (SectionName,"Specular"         ,m_fSpecular              );

		pFile->WriteFloat     (SectionName,"Detail0UVScale"   ,m_fDetail0UVScale         );
		pFile->WriteFloat     (SectionName,"Detail0ColorScale",m_fDetail0ColorScale      );
		pFile->WriteFloat     (SectionName,"Detail0Env"       ,m_fDetail0Env             );
		pFile->WriteFloat     (SectionName,"Detail0Specular"  ,m_fDetail0Specular        );
		pFile->WriteMultiFloat(SectionName,"Detial0Color"     ,(float*)&m_cDetial0Color,4);
		pFile->WriteString    (SectionName,"Detail0FileName"  ,szDetail0                 );

		pFile->WriteFloat     (SectionName,"Detail1UVScale"   ,m_fDetail1UVScale         );
		pFile->WriteFloat     (SectionName,"Detail1ColorScale",m_fDetail1ColorScale      );
		pFile->WriteFloat     (SectionName,"Detail1Env"       ,m_fDetail1Env             );
		pFile->WriteFloat     (SectionName,"Detail1Specular"  ,m_fDetail1Specular        );
		pFile->WriteMultiFloat(SectionName,"Detial1Color"     ,(float*)&m_cDetial1Color,4);
		pFile->WriteString    (SectionName,"Detail1FileName"  ,szDetail1                 );
		pFile->WriteString    (SectionName,"Name"             ,szName                    );

		return S_OK;
	}

	HRESULT KG3DMaterialDetail::LoadTextures()
	{
		HRESULT hr = E_FAIL;

		KG3DTexture* pDetailTexture0 = NULL;
		KG3DTexture* pDetailTexture1 = NULL;
		KG3DTexture* pChannelTexture = NULL;

		if(m_szChannelFileName.size())
		{
			hr = g_cTextureTable.LoadResourceFromFile(m_szChannelFileName.c_str(),0,TEXOP_RELOAD,(IKG3DResourceBase**)&pChannelTexture);
			if(SUCCEEDED(hr))
			{
				SAFE_RELEASE(m_lpChannelTexture);
				m_lpChannelTexture = pChannelTexture;
			}
		}

		if(m_szDetail0FileName.size())
		{
			hr = g_cTextureTable.LoadResourceFromFile(m_szDetail0FileName.c_str(),0,TEXOP_RELOAD,(IKG3DResourceBase**)&pDetailTexture0);
			if(SUCCEEDED(hr))
			{
				SAFE_RELEASE(m_lpDetailTexture0);
				m_lpDetailTexture0 = pDetailTexture0;
			}
		}

		if(m_szDetail1FileName.size())
		{
			hr = g_cTextureTable.LoadResourceFromFile(m_szDetail1FileName.c_str(),0,TEXOP_RELOAD,(IKG3DResourceBase**)&pDetailTexture1);
			if(SUCCEEDED(hr))
			{
				SAFE_RELEASE(m_lpDetailTexture1);
				m_lpDetailTexture1 = pDetailTexture1;
			}
		}

		return S_OK;
	}


	int KG3DMaterial::GetMaterialDetailDefineCount()
	{
		return (int)m_vecDetailDefine.size();
	}

	HRESULT KG3DMaterial::AddMaterialDetailDefine() 
	{
		KG3DMaterialDetail detail;
		m_vecDetailDefine.push_back(detail);
		return S_OK;
	}

	HRESULT KG3DMaterial::DeleteMaterialDetailDefine(int nIndex)
	{
		int S = 0;
		vector<KG3DMaterialDetail>::iterator i = m_vecDetailDefine.begin();
		vector<KG3DMaterialDetail>::iterator iend = m_vecDetailDefine.end();
		while (i!=iend)
		{
			if(S==nIndex)
			{
				m_vecDetailDefine.erase(i);
				return S_OK;
			}
			i++;
			S++;
		}

		return S_OK;
	}

	HRESULT KG3DMaterial::SaveMaterialDetails(LPSTR pFileName)
	{
		TCHAR szSection[MAX_PATH];

		KGLOG_PROCESS_ERROR(pFileName);
		IIniFile* pFile = g_OpenIniFile(pFileName,true,true);
		KGLOG_PROCESS_ERROR(pFile);

		int nCount = (int)m_vecDetailDefine.size();

		pFile->WriteInteger("Main","DefineCount",nCount);

		for (int i=0;i<nCount;i++)
		{
			wsprintf(szSection,"DetailDefine%d",i);
			KG3DMaterialDetail& vDetail = m_vecDetailDefine[i];
			vDetail.SaveDefineToIni(pFile,szSection);
		}

		pFile->Save(pFileName);
		SAFE_RELEASE(pFile);
		return S_OK;
Exit0:
		return E_FAIL;
	}

	HRESULT KG3DMaterial::LoadMaterialDetails(LPSTR pFileName)
	{
		TCHAR szSection[MAX_PATH];
		KGLOG_PROCESS_ERROR(pFileName);
		IIniFile* pFile = g_OpenIniFile(pFileName);
		KG_PROCESS_ERROR(pFile);

		int nCount = 0;
		pFile->GetInteger("Main","DefineCount",0,&nCount);
		for (int i=0;i<nCount;i++)
		{
			wsprintf(szSection,"DetailDefine%d",i);

			KG3DMaterialDetail vDetail;
			vDetail.LoadFormIni(pFile,szSection);
			m_vecDetailDefine.push_back(vDetail);
		}

		SAFE_RELEASE(pFile);
		return S_OK;
Exit0:
		return E_FAIL;
	}


	BOOL KG3DMaterialDetail::IsResourceReady()
	{
		BOOL bLoad = TRUE;

		if(m_lpChannelTexture)
			bLoad &= m_lpChannelTexture->IsResourceReady();

		if(m_lpDetailTexture0)
			bLoad &= m_lpDetailTexture0->IsResourceReady();

		if(m_lpDetailTexture1)
			bLoad &= m_lpDetailTexture1->IsResourceReady();

		return bLoad;
	}


