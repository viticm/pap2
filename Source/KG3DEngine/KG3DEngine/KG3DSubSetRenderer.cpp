////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSubSetRenderer.cpp
//  Version     : 1.0
//  Creator     : Yang Lin
//  Create Date : 2008-3-28 17:35:31
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "KG3DSubSetRenderer.h"
#include "kg3dRenderState.h"
#include "KG3DGraphicsTool.h"
#include "KG3DModel.h"
#include "KG3DScene.h"
#include "KG3DTextureTable.h"
#include "KG3DSFXProjectionTextureMgr.h"
#include "KG3DTerrainBlock.h"
#include "KG3DTerrainBlockGrass.h"
#include "KG3DGraphiceEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
////////////////////////////////////////////////////////////////////////////////

extern int g_nGrassVertexBufferFillPerFrame;
extern int g_nLoadCount;


const static DWORD INVALIDATE_PASS = 0xffffffff;
static DWORD s_dwLastPass = INVALIDATE_PASS;
static LPD3DXEFFECT s_pCurrentEffect = NULL;
static KG3DDefaultShaderType s_CurrentShaderType = DEFST_COUNT;
extern DWORD g_dwRenderCount;
extern HRESULT g_SetMaterialDetailInfo(LPD3DXEFFECT pEffect,KG3DModel* pModel, int nSubSetID);

#if 0
void g_SetShadowInfo(LPD3DXEFFECT pShader )
{
	HRESULT  hr = E_FAIL;
	_ASSERTE(NULL != pShader);
	KG_PROCESS_SUCCESS(g_cEngineOption.ModelShadowType < EM_MODEL_SHADOW_TYPE_MIDDLE);	//开高影子才需要设里面的参数
	{
		IEKG3DScene* pCurScene = g_cGraphicsTool.GetScene();
		KG_PROCESS_ERROR(NULL != pCurScene);
		KG_PROCESS_ERROR(NULL != pShader);
#if defined(DEBUG) | defined(_DEBUG)
		_KG3D_DEBUG_SUPPERSUB_POINTER_EQUAL(IEKG3DScene, KG3DScene);
		KG3DScene* pSceneSolid = dynamic_cast<KG3DScene*>(pCurScene);
		_ASSERTE(NULL != pSceneSolid && _T("应该能够转换"));
#else
		KG3DScene* pSceneSolid = static_cast<KG3DScene*>(pCurScene);
#endif

		IEKG3DSceneOutputWnd* pWnd;
		pSceneSolid->GetCurOutputWnd(&pWnd);
		KG_PROCESS_ERROR(NULL != pWnd);

		LPDIRECT3DTEXTURE9 pTexture = NULL;
		float fBiasU = 0,fBiasV = 0;
		DWORD dwWidth = 0,dwHeight = 0;
		pWnd->GetViewPortWidthHeight(&dwWidth,&dwHeight);
		if (dwHeight != 0 && dwWidth != 0)
		{
			fBiasU = (float)(1.0f / dwWidth * 0.25);
			fBiasV = (float)(1.0f / dwHeight * 0.25);
		}

		hr = pWnd->GetShadowMask(&pTexture);
		KG_COM_PROCESS_ERROR(hr);

		hr = pShader->SetBool("bShadowMask",TRUE);
		hr = pShader->SetTexture("ShadowTexture",pTexture);
		hr = pShader->SetFloat("fBiasU",fBiasU);
		hr = pShader->SetFloat("fBiasV",fBiasV);
	}
	return;
Exit1:
Exit0:
	hr = pShader->SetBool("bShadowMask",FALSE);
	hr = pShader->SetTexture("ShadowTexture",NULL);
	return;
}
#endif

namespace
{
	void SetShaderSharedParamConver(LPD3DXEFFECT pShader, KG3DTerrainConverMap* pConverMap)
	{
		HRESULT hr = E_FAIL;
		_ASSERTE(NULL != pShader);
		if(NULL != pConverMap)
		{
			pShader->SetVector("vConverRect",&pConverMap->GetRect());
			LPDIRECT3DTEXTURE9 pTex = pConverMap->GetRenderTargetTex();
			if(NULL != pTex)
			{
				hr = pShader->SetTexture("tConver", pTex);
			}
			else
			{
				KG3DTexture* pConverTexture = g_cTextureTable.GetConverTexture();
				_ASSERTE(NULL != pConverTexture);
				hr = pShader->SetTexture("tConver", pConverTexture->GetTexture());
			}
		}
	}

	void SetShaderSharedParamEnv(LPD3DXEFFECT pShader, const D3DXVECTOR3& camPos, LPDIRECT3DCUBETEXTURE9 pEnvMap)
	{
		HRESULT hr = E_FAIL;
		_ASSERTE(NULL != pShader);
		hr = pShader->SetTexture("tEnv",pEnvMap);
		hr = pShader->SetValue("eyes", &camPos, sizeof(D3DXVECTOR3));
	}

	void SetShaderSharedParamShadow(LPD3DXEFFECT pShader, const KG3DShaderParamShadowMap& shadowMapParam, LPDIRECT3DTEXTURE9 pShadowMap)
	{
		HRESULT hr = E_FAIL;
		_ASSERTE(NULL != pShader);
		hr = pShader->SetValue("shadowMapParam", &shadowMapParam, sizeof(KG3DShaderParamShadowMap));
		hr = pShader->SetTexture("ShadowTexture", pShadowMap);
	}

	void SetShaderSharedParamLightAndFog(LPD3DXEFFECT pShader, const KG3DLightParam& sunLight, const KG3DLightParam& playerLight
								, const KG3DFogParamShader& fogParam)
	{
		HRESULT hr = E_FAIL;
		_ASSERTE(NULL != pShader);
		hr = pShader->SetValue("sunLight", &sunLight, sizeof(KG3DLightParam));
		hr = pShader->SetValue("playerLight", &playerLight, sizeof(KG3DLightParam));
		hr = pShader->SetValue("fogParam", &fogParam, sizeof(KG3DFogParamShader));
	}
};

extern VOID g_SetShadowParams(LPD3DXEFFECT pShader,KG3DSceneOutputWnd& outWnd,BOOL bForceDisable)
{
	HRESULT hr = E_FAIL;
	if (bForceDisable)
	{
		KG3DShaderParamShadowMap paramShadowMap = 
		{
			false,
			D3DXVECTOR2(0, 0),
		};
		SetShaderSharedParamShadow(pShader, paramShadowMap, NULL);
	}
	else
	{
		/*hr = pShader->SetBool("bShadowMask",TRUE);
		hr = pShader->SetFloat("fBiasU",fBiasU);
		hr = pShader->SetFloat("fBiasV",fBiasV);*/

		bool bShadowMapOK = false;

		do 
		{		
			//bool bUseShadowMap = !!(g_cEngineOption.ModelShadowType > EM_MODEL_SHADOW_TYPE_LOW);

			//if (! bUseShadowMap)
			//	break;

			float fBiasU = 0,fBiasV = 0;
			DWORD dwWidth = 0,dwHeight = 0;
			hr = outWnd.GetViewPortWidthHeight(&dwWidth,&dwHeight);
			if(FAILED(hr))
				break;

			if (dwHeight != 0 && dwWidth != 0)
			{
				fBiasU = (float)(1.0f / dwWidth * 0.25);
				fBiasV = (float)(1.0f / dwHeight * 0.25);
			}

			LPDIRECT3DTEXTURE9 pTexture = NULL;
			hr = outWnd.GetShadowMask(&pTexture);		
			if (FAILED(hr))
				break;

			KG3DShaderParamShadowMap paramShadowMap = 
			{
				true,
				D3DXVECTOR2(fBiasU, fBiasV),
			};

			//_ASSERTE(NULL != pTexture);
			SetShaderSharedParamShadow(pShader, paramShadowMap, pTexture);
			bShadowMapOK = true;

		} while(false);

		if (! bShadowMapOK)
		{
			KG3DShaderParamShadowMap paramShadowMap = 
			{
				false,
				D3DXVECTOR2(0, 0),
			};
			SetShaderSharedParamShadow(pShader, paramShadowMap, NULL);
		}	
	}
}

extern VOID g_SetShaderSharedParams(KG3DScene& sceneCur, KG3DSceneOutputWnd& outWnd, KG3DFogParamShader* pfogUnderWater, KG3DTerrainConverMap* pConverMap)//<Shared Pool> 统一设Shader的光照和Fog等。
{
	LPD3DXEFFECT pShader = g_GetDefaultShaderGroup().GetDefaultEffect(DEFST_LIBRARY);
	_ASSERTE(NULL != pShader);

	//env
	SetShaderSharedParamEnv(pShader, outWnd.GetCamera().GetPosition(), sceneCur.GetEnvMap(NULL, 0));

	//conver
	SetShaderSharedParamConver(pShader, pConverMap);

	//shadow
	g_SetShadowParams(pShader,outWnd);

	//light	//fog
	{
		//现在两个Light一起用
		const KG3DLightParam& sunLightParam = g_cGraphicsTool.GetSunLightParam(sceneCur);
		const KG3DLightParam& playerLightParam = g_cGraphicsTool.GetPlayerLightParam(sceneCur);

		//pShader->SetValue("sky_light_dir", &vDir, sizeof(D3DXVECTOR3));	//天光方向应该是0,-1,0，不要改了。像上面这样就违反天光的意义了，在别的Shader里面有的是这样用的，有的是那样用的，那样就是往死里整了。天光就是让它是天光吧。
	
		KG3DFogParam fogParam = g_cGraphicsTool.GetFogParam(sceneCur);

		KG3DFogParamShader paramShader = (NULL != pfogUnderWater) ? *pfogUnderWater : fogParam;
		if (NULL == pfogUnderWater && ! fogParam.bEnable)
		{
			paramShader.fDensityDevidedBy_endMinusStart = 0.f;	//这个值可以控制Fog的作用域，设为0就可以不作用了
		}
		//hr = pShader->SetVector("FogInfo", &fogParam.vStartEndDensityNull);
		//hr = pShader->SetValue("FogColor", &fogParam.vColor, sizeof(fogParam.vColor));
		
		SetShaderSharedParamLightAndFog(pShader, sunLightParam, playerLightParam, paramShader);

		//对于水下的雾来说，和场景雾不同，要设一下d3d
		if (NULL != pfogUnderWater)
		{
			/*
			注意下面的
			DemandFullControlOfFog
			ReleaseFullControlOfFog
			如果DemandFullControlOfFog，D3DRS_FOGENABLE系列由Fog控制，但是用完一定要
			ReleaseFullControlOfFog
			否则雾就永远是不正确的状态。
			*/


			g_cGraphicsTool.DemandFullControlOfFog(sceneCur);	
			g_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
			g_pd3dDevice->SetRenderState(D3DRS_FOGCOLOR, paramShader.vColor);
			g_pd3dDevice->SetRenderState(D3DRS_FOGSTART,FtoDW(paramShader.fStart));
			if(0 == paramShader.fDensityDevidedBy_endMinusStart)
				paramShader.fDensityDevidedBy_endMinusStart = FLT_MIN;
			FLOAT fFogEnd = (1 / paramShader.fDensityDevidedBy_endMinusStart) + paramShader.fStart;
			g_pd3dDevice->SetRenderState(D3DRS_FOGEND,  FtoDW(fFogEnd)); 
		}
		else
		{
			g_cGraphicsTool.ReleaseFullControlOfFog(sceneCur);
		}
		
	}	
}

extern VOID g_SetShaderSharedParamsDefault(const D3DXVECTOR3& camPos)
{
	//设定MiniScene的光照
	LPD3DXEFFECT pShader = g_GetDefaultShaderGroup().GetDefaultEffect(DEFST_LIBRARY);
	_ASSERTE(NULL != pShader);

	//env
	SetShaderSharedParamEnv(pShader, camPos, KG3DTextureTable::GetDefaultEnvMap());

	//Conver
	SetShaderSharedParamConver(pShader, &KG3DTerrainConverMap::GetUniqueDefault());

	//shadow
	KG3DShaderParamShadowMap paramShadowMap = 
	{
		false,
		D3DXVECTOR2(0, 0),
	};
	SetShaderSharedParamShadow(pShader, paramShadowMap, NULL);

	//light fog
	const KG3DLightParam& sunParam = KG3D_ENVIRONMENT_EFFECT::KG3DEnvEffLight::GetDefaultSunParam();
	const KG3DLightParam& playerParam = KG3D_ENVIRONMENT_EFFECT::KG3DEnvEffLight::GetDefaultPlayerParam();
	KG3DFogParamShader fogParam = g_cGraphicsTool.GetDefaultFogParam();

	SetShaderSharedParamLightAndFog(pShader, sunParam, playerParam, fogParam);
}

#if 0
/*static*/ HRESULT g_SetLightInfo(LPD3DXEFFECT pShader)
{

	


	HRESULT hResult  = E_FAIL;
	HRESULT hr = E_FAIL;
	IEKG3DScene* pCurScene = g_cGraphicsTool.GetScene();
	KG_PROCESS_ERROR(NULL != pCurScene);
	KG_PROCESS_ERROR(NULL != pShader);
	{
#if defined(DEBUG) | defined(_DEBUG)
		_KG3D_DEBUG_SUPPERSUB_POINTER_EQUAL(IEKG3DScene, KG3DScene);
		KG3DScene* pSceneSolid = dynamic_cast<KG3DScene*>(pCurScene);
		_ASSERTE(NULL != pSceneSolid && _T("应该能够转换"));
#else
		KG3DScene* pSceneSolid = static_cast<KG3DScene*>(pCurScene);
#endif
		

		hr = pShader->SetTexture("tEnv",pSceneSolid->GetEnvMap(NULL, 0));
		
		//Light
		{
			const KG3DLightParam& sunLightParam = g_cGraphicsTool.GetSunLightParam(*pSceneSolid);
			const KG3DLightParam& playerLightParam = g_cGraphicsTool.GetPlayerLightParam(*pSceneSolid);

			hr = pShader->SetValue("sunLight", &sunLightParam, sizeof(KG3DLightParam));
			hr = pShader->SetValue("playerLight", &playerLightParam, sizeof(KG3DLightParam));

			KG3DCamera* pCamera  = g_cGraphicsTool.GetCamera();
			if(pCamera)
			{
				D3DXVECTOR3 vEyes;
				pCamera->GetPosition(&vEyes);
				hr = pShader->SetValue("eyes", &vEyes, sizeof(D3DXVECTOR3));

				//D3DXVECTOR3 vFront = pCamera->GetCameraRight();
				//D3DXVECTOR3 vDir = -vFront;
			
				//pShader->SetValue("sky_light_dir", &vDir, sizeof(D3DXVECTOR3));	//天光方向应该是0,-1,0，不要改了。像上面这样就违反天光的意义了，在别的Shader里面有的是这样用的，有的是那样用的，那样就是往死里整了。天光就是让它是天光吧。
			}
		}
	}
	hResult = S_OK;

Exit0 :
	return hResult;
}

/*static*/ HRESULT g_SetFogInfo(LPD3DXEFFECT pShader)
{
	HRESULT hr = E_FAIL;
	IEKG3DScene* pCurScene = g_cGraphicsTool.GetScene();
	KG_PROCESS_ERROR(NULL != pCurScene);
	KG_PROCESS_ERROR(NULL != pShader);
	{
#if defined(DEBUG) | defined(_DEBUG)
		_KG3D_DEBUG_SUPPERSUB_POINTER_EQUAL(IEKG3DScene, KG3DScene);
		KG3DScene* pSceneSolid = dynamic_cast<KG3DScene*>(pCurScene);
		_ASSERTE(NULL != pSceneSolid && _T("应该能够转换"));
#else
		KG3DScene* pSceneSolid = static_cast<KG3DScene*>(pCurScene);
#endif
		KG3DFogParam fogParam = g_cGraphicsTool.GetFogParam(*pSceneSolid);

		if (! fogParam.bEnable)
		{
			fogParam.fDensityDevidedBy_endMinusStart = 0.f;	//这个值可以控制Fog的作用域，设为0就可以不作用了
		}
		//hr = pShader->SetVector("FogInfo", &fogParam.vStartEndDensityNull);
		//hr = pShader->SetValue("FogColor", &fogParam.vColor, sizeof(fogParam.vColor));
		KG3DFogParamShader& paramShader = fogParam;
		hr = pShader->SetValue("fogParam", &paramShader, sizeof(KG3DFogParamShader));
		
	}
	return S_OK;
Exit0:
	return E_FAIL;
}
#endif

HRESULT g_SetPointLighting(LPD3DXEFFECT pShader,KG3D_PointLight_Info* pInfo, DWORD dwMaxPointLightNum, bool bCommit)
{
	if(pInfo)
	{
        DWORD dwPointLightNum = pInfo->m_nNumPointLights;

        dwPointLightNum = min(dwPointLightNum, dwMaxPointLightNum);

		pShader->SetInt("nPointLightCount", dwPointLightNum);
		pShader->SetVectorArray("PointLightPosition",pInfo->m_vPointLightPosition, dwPointLightNum);
		pShader->SetVectorArray("PointLightColor"   ,pInfo->m_vPointLightColor   , dwPointLightNum);
	}
	else
	{
		pShader->SetInt("nPointLightCount",0);
	}

    if (bCommit)
	    pShader->CommitChanges();


	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
KG3DModelSubsetInfo::KG3DModelSubsetInfo()
{
	ZeroMemory(this,sizeof(KG3DModelSubsetInfo));
}

HRESULT KG3DModelSubsetInfo::UnInit()
{
	SAFE_RELEASE(pModel);

	SAFE_RELEASE(pXMeshToRender);
	ZeroMemory(this,sizeof(KG3DModelSubsetInfo));

	return S_OK;
}
//////////////////////////////////////////////////////////////////////////

HRESULT KG3DSubsetRender::Init()
{
	return S_OK;
}

HRESULT KG3DSubsetRender::UnInit(TKG3DResourcePool<KG3DSubMaterialGroup>*pPool,TKG3DResourcePool<KG3DModelSubsetInfo>*pInfoPool)
{
	std::map<DWORD,KG3DSubMaterialGroup*>::iterator i = m_mapMaterialGroup.begin();
	std::map<DWORD,KG3DSubMaterialGroup*>::iterator iend = m_mapMaterialGroup.end();
	while (i!=iend)
	{
		KG3DSubMaterialGroup* pgroup = i->second;
		pgroup->Clear(pInfoPool);
		pPool->FreeResource(pgroup);
		i++;
	}
	m_mapMaterialGroup.clear();

	return S_OK;
}

HRESULT KG3DSubsetRender::Clear(TKG3DResourcePool<KG3DModelSubsetInfo>*pInfoPool)
{
	std::map<DWORD,KG3DSubMaterialGroup*>::iterator i = m_mapMaterialGroup.begin();
	std::map<DWORD,KG3DSubMaterialGroup*>::iterator iend = m_mapMaterialGroup.end();
	while (i!=iend)
	{
		KG3DSubMaterialGroup* pgroup = i->second;
		pgroup->Clear(pInfoPool);
		i++;
	}
	return S_OK;
}

KG3DSubsetRender::KG3DSubsetRender()
{
	;
}

KG3DSubsetRender::~KG3DSubsetRender()
{
	m_ShaderType = DEFST_COUNT;
}

HRESULT KG3DSubsetRender::Optimize()
{
	//BOOL bUnit = (m_ShaderType == DEFST_NEWLIGHT);

	std::map<DWORD,KG3DSubMaterialGroup*>::iterator i = m_mapMaterialGroup.begin();
	std::map<DWORD,KG3DSubMaterialGroup*>::iterator iend = m_mapMaterialGroup.end();
	while (i!=iend)
	{
		KG3DSubMaterialGroup* pgroup = i->second;
		pgroup->Optimize(FALSE);
		i++;
	}
	return S_OK;
}

HRESULT KG3DSubsetRender::InputMeshSubset(DWORD dwSubsetKey,KG3DModelSubsetInfo& newsubset,TKG3DResourcePool<KG3DSubMaterialGroup>*pPool,TKG3DResourcePool<KG3DModelSubsetInfo>*pInfoPool)
{
	KG3DSubMaterialGroup* pnewSet = NULL;

	map<DWORD,KG3DSubMaterialGroup*>::iterator iFind = m_mapMaterialGroup.find(dwSubsetKey);

	if(iFind==m_mapMaterialGroup.end())
	{
		pnewSet = pPool->RequestResource();
		KGLOG_PROCESS_ERROR(pnewSet);
		pnewSet->Init();

		KG3DModelSubsetInfo* pNewInfo = pInfoPool->RequestResource();
		KGLOG_PROCESS_ERROR(pNewInfo);

		(*pNewInfo) = newsubset;

		if(pNewInfo->pModel)
		{
			pNewInfo->pModel->AddRef();
		}

        pnewSet->m_vecSubsetMaterialSet.push_back(pNewInfo);
		
        m_mapMaterialGroup[dwSubsetKey] = pnewSet;

		pnewSet->m_bNeedRefresh = TRUE;
	}
	else
	{
		KG3DSubMaterialGroup* psmset = iFind->second;
		KGLOG_PROCESS_ERROR(psmset);

		KG3DModelSubsetInfo* pNewInfo = pInfoPool->RequestResource();
		KGLOG_PROCESS_ERROR(pNewInfo);

		(*pNewInfo) = newsubset;
		
		if(pNewInfo->pModel)
		{
			pNewInfo->pModel->AddRef();
		}
        
        psmset->m_vecSubsetMaterialSet.push_back(pNewInfo);

		psmset->m_bNeedRefresh = TRUE;
	}

	return S_OK;
Exit0:
	if(pnewSet)
	{
		pPool->FreeResource(pnewSet);
		pnewSet = NULL;
	}
	return E_FAIL;
}

HRESULT KG3DSubsetRender::RemoveMeshSubset(DWORD dwSubsetKey,KG3DModelSubsetInfo* pSubset,TKG3DResourcePool<KG3DSubMaterialGroup>*pPool,TKG3DResourcePool<KG3DModelSubsetInfo>*pInfoPool)
{
	map<DWORD,KG3DSubMaterialGroup*>::iterator iFind = m_mapMaterialGroup.find(dwSubsetKey);
	KGLOG_PROCESS_ERROR(iFind != m_mapMaterialGroup.end());

	KG3DSubMaterialGroup* psmset = iFind->second;
	size_t i = 0;
    KG3DModelSubsetInfo* pInfo = NULL;

    size_t uSize = psmset->m_vecSubsetMaterialSet.size();

    for (i = 0; i < uSize; ++i)
    {
        pInfo = psmset->m_vecSubsetMaterialSet[i];
        if((pInfo->pModel == pSubset->pModel) && (pInfo->dwSubsetID == pSubset->dwSubsetID))
        {
            break;
        }
    }
	if (i < uSize)
	{
        psmset->m_vecSubsetMaterialSet[i] = psmset->m_vecSubsetMaterialSet[uSize - 1];
        psmset->m_vecSubsetMaterialSet.resize(uSize - 1);
        pInfoPool->FreeResource(pInfo);
        psmset->m_bNeedRefresh = TRUE;
        SAFE_RELEASE(psmset->m_lpKG3DTexture);
	}
    if(psmset->m_vecSubsetMaterialSet.size() == 0)
    {
        pPool->FreeResource(psmset);
        m_mapMaterialGroup.erase(iFind);
    }

	return S_OK;
Exit0:
	return E_FAIL;
}


HRESULT KG3DSubsetRender::RenderForColorOnly(DWORD dwAlphaType,BOOL bForceAlphaTest,KG3DTerrainConverMap* pConverMap)
{
	HRESULT hRetCode = E_FAIL;
	HRESULT hResult = E_FAIL;
    //KG3DRenderState RenderState;

	std::map<DWORD,KG3DSubMaterialGroup*>::iterator t = m_mapMaterialGroup.begin();
	std::map<DWORD,KG3DSubMaterialGroup*>::iterator iend = m_mapMaterialGroup.end();
	UINT uDummy = 0;
	LPD3DXEFFECT pEffect = NULL;
    KG3DSubMaterialGroup *pSubGroup = NULL;

    if (m_mapMaterialGroup.empty())
        return S_OK;

    pEffect = g_cGraphicsTool.GetDefaultShader(m_ShaderType);
	KG_PROCESS_ERROR(pEffect);

    hRetCode = pEffect->SetTechnique("ConverMapTechnique");
    KGLOG_COM_CHECK_ERROR(hRetCode);

	hRetCode = pEffect->Begin(&uDummy,0 );//D3DXFX_DONOTSAVESTATE
	KG_COM_PROCESS_ERROR(hRetCode);

	ASSERT(!s_pCurrentEffect);

	s_pCurrentEffect = pEffect;
	s_CurrentShaderType = m_ShaderType;

	//g_SetShadowInfo(pEffect);//<ShadowParamMoveToShared>

	/*if (pConverMap)//<ConverMap Shared>
	{
		pEffect->SetVector("vConverRect",&pConverMap->GetRect());
		if (pConverMap->GetRenderTargetTex())
        {
			pEffect->SetTexture("tConver",pConverMap->GetRenderTargetTex());
        }
		else
		{
			KG3DTexture* pConverTexture = g_cTextureTable.GetConverTexture();
			pEffect->SetTexture("tConver", pConverTexture->GetTexture());
		}
	}*/

    pSubGroup = t->second;
    pSubGroup->RenderColorOnlyBegin(bForceAlphaTest);
	
	while (t!=iend)
	{
		KG3DSubMaterialGroup* pGroup = t->second;

		pGroup->RenderColorOnly(pEffect,m_ShaderType==DEFST_NEWLIGHT_DETAIL);

		++t;
	}
	
    pSubGroup->RenderColorOnlyEnd();

	if (s_dwLastPass != INVALIDATE_PASS)
	{
		s_pCurrentEffect->EndPass();
		s_dwLastPass = INVALIDATE_PASS;
	}

	hRetCode = pEffect->End();
	KG_COM_PROCESS_ERROR(hRetCode);

	s_pCurrentEffect = NULL;
	s_CurrentShaderType = DEFST_COUNT;

	hResult = S_OK;

Exit0:
    //RenderState.Restore();
	g_pd3dDevice->SetVertexShader(NULL);
	g_pd3dDevice->SetPixelShader(NULL);

	/*if (pEffect)
    {
        pEffect->SetTexture("tConver", NULL);
		}*///<ConverMap Shared>
	return hResult;
}

HRESULT KG3DSubsetRender::RenderReflect(float fWaterHeight,D3DXPLANE& WaterPlane,D3DXPLANE& WaterPlaneFX)
{
	std::map<DWORD,KG3DSubMaterialGroup*>::iterator t = m_mapMaterialGroup.begin();
	std::map<DWORD,KG3DSubMaterialGroup*>::iterator iend = m_mapMaterialGroup.end();

	while (t!=iend)
	{
		KG3DSubMaterialGroup*& pGroup = t->second;
		//////////////////////////////////////////////////////////////////////////
		pGroup->RenderReflect(fWaterHeight,WaterPlane,WaterPlaneFX);
		t++;
	}
	return S_OK;
}

HRESULT KG3DSubsetRender::RenderDepthOnly(DWORD dwAlphaType)
{
	HRESULT hr = E_FAIL;
	DWORD dwNowTime = timeGetTime();

	std::map<DWORD,KG3DSubMaterialGroup*>::iterator t = m_mapMaterialGroup.begin();
	std::map<DWORD,KG3DSubMaterialGroup*>::iterator iend = m_mapMaterialGroup.end();
	UINT uDummy = 0;
	LPD3DXEFFECT pEffect = NULL;
    KG3DSubMaterialGroup *pSubGroup = NULL;

    if (m_mapMaterialGroup.empty())
        return S_OK;

    pEffect = g_cGraphicsTool.GetDefaultShader(m_ShaderType);
	KG_PROCESS_ERROR(pEffect);
	s_pCurrentEffect = pEffect;
	s_CurrentShaderType = m_ShaderType;

    hr = pEffect->SetTechnique("tecZ");
    KGLOG_COM_CHECK_ERROR(hr);

	hr = pEffect->Begin(&uDummy, 0);//D3DXFX_DONOTSAVESTATE
	hr = pEffect->BeginPass(0);

    pSubGroup = t->second;
    pSubGroup->RenderDepthOnlyBegin();
	while (t!=iend)
	{
		KG3DSubMaterialGroup*& Group = t->second;
		Group->RenderDepthOnly();
		t++;
	}
    pSubGroup->RenderDepthOnlyEnd();

	pEffect->EndPass();
	pEffect->End();
	s_pCurrentEffect = NULL;
	s_CurrentShaderType = DEFST_COUNT;
	hr = S_OK;
Exit0:
	g_pd3dDevice->SetPixelShader(NULL);
	g_pd3dDevice->SetVertexShader(NULL);

	DWORD dwCost = timeGetTime() - dwNowTime;
	if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost >= 30)
	{
		KGLogPrintf(KGLOG_ERR, "TimeOptimize %d KG3DSubsetRender::RenderDepthOnly.",dwCost);
	}
	return hr;
}

HRESULT KG3DSubsetRender::Render()
{
	HRESULT hResult = E_FAIL;
	HRESULT hRetCode = E_FAIL;
	UINT uDummy = 0;
    LPD3DXEFFECT pEffect = NULL;
	std::map<DWORD,KG3DSubMaterialGroup*>::iterator t = m_mapMaterialGroup.begin();
	std::map<DWORD,KG3DSubMaterialGroup*>::iterator iend = m_mapMaterialGroup.end();

	pEffect = g_cGraphicsTool.GetDefaultShader(m_ShaderType);
	KG_PROCESS_ERROR(pEffect);

    hRetCode = pEffect->SetTechnique("DefaultTechnique");
    KGLOG_COM_CHECK_ERROR(hRetCode);

	hRetCode = pEffect->Begin(&uDummy, 0);//D3DXFX_DONOTSAVESTATE
	KG_COM_PROCESS_ERROR(hRetCode);

	s_pCurrentEffect = pEffect;
	s_CurrentShaderType = m_ShaderType;
	//g_SetShadowInfo(pEffect);//<ShadowParamMoveToShared>

	while (t!=iend)
	{
		KG3DSubMaterialGroup*& pGroup = t->second;
		//////////////////////////////////////////////////////////////////////////
		//if(pGroup->m_dwAlphaType & dwAlphaType)
		pGroup->Render(m_ShaderType==DEFST_NEWLIGHT_DETAIL);
		t++;
	}

	if (s_dwLastPass != INVALIDATE_PASS)
	{
		s_pCurrentEffect->EndPass();
		s_dwLastPass = INVALIDATE_PASS;
	}

	hRetCode = pEffect->End();
	KG_COM_PROCESS_ERROR(hRetCode);

	s_pCurrentEffect = NULL;
	s_CurrentShaderType = DEFST_COUNT;

	hResult = S_OK;
Exit0:
	g_pd3dDevice->SetPixelShader(NULL);
	g_pd3dDevice->SetVertexShader(NULL);

	return hResult;
}


HRESULT KG3DSubMaterialGroup::Clear(TKG3DResourcePool<KG3DModelSubsetInfo>*pInfoPool)
{
    for (size_t i = 0; i < m_vecSubsetMaterialSet.size(); ++i)
    {
        if (m_vecSubsetMaterialSet[i])
        {
            pInfoPool->FreeResource(m_vecSubsetMaterialSet[i]);
        }
    }

    m_vecSubsetMaterialSet.clear();

	return S_OK;
}

HRESULT KG3DSubMaterialGroup::Init()
{
	SAFE_RELEASE(m_lpKG3DTexture);
	m_dwAlphaType = 1;
	m_bNeedRefresh = FALSE;
	m_lpShareVB = NULL;
    m_pRenderState = new KG3DRenderState;
    ASSERT(m_pRenderState);
	return S_OK;
}

HRESULT KG3DSubMaterialGroup::UnInit()
{
	SAFE_RELEASE(m_lpKG3DTexture);
	m_dwAlphaType = 1;
	m_bNeedRefresh = FALSE;
	m_lpShareVB = NULL;
    SAFE_DELETE(m_pRenderState);
	return S_OK;
}

/*static*/HRESULT KG3DSubMaterialGroup::SetNewLightShaderConst(LPD3DXEFFECT pShader,
									  KG3DModelSubsetInfo& Info, DWORD dwShaderType)
{
	

	KG3DTexture *pTexture = NULL;

	if ((dwShaderType != DEFST_NEWLIGHT) && (dwShaderType != DEFST_NEWLIGHT_DETAIL))
	{
		return S_OK;	//只有上面这2种才需要设下面的参数
	}

	{
		HRESULT hResult = E_FAIL;

		KG3DMaterial::KG3DMaterialSubset *pSubset = Info.pModel->GetMaterialSubset(Info.dwSubsetID);
		D3DXMATRIX matWorld, matView, matProj, matWorldViewProj;
		KG3DRuntimeMaterial *pRuntimeMaterial = NULL;

		D3DMATERIAL9 material;

		KG_PROCESS_ERROR(pShader);
		KG_PROCESS_ERROR(pSubset);
		KG_PROCESS_ERROR(Info.pModel);

		pRuntimeMaterial = Info.pModel->GetRuntimeMaterial();

		material = pSubset->m_sMaterial9;
		pRuntimeMaterial->ApplyMaterial(material);

		if(1)
		{
			/*hResult = pShader->SetValue("spe_exp",     &material.Power,      sizeof(float));
			hResult = pShader->SetValue("emssive_power", &pSubset->m_fEmssPower, sizeof(float));
			hResult = pShader->SetBool("bhightlight", TRUE);*/

			//上面改成一次设进去
			KG3DShaderParamMaterialEx materialEx;
			materialEx.bhightlight = TRUE;
			materialEx.color_cast = D3DXCOLOR(1,1,1,1);
			materialEx.emssive_power = pSubset->m_fEmssPower;
			materialEx.spe_exp = material.Power;
			pShader->SetValue("materialEx", &materialEx, sizeof(materialEx));


			material.Power = pSubset->m_fSpecPower;
			hResult = pShader->SetValue("materialCur", &material, sizeof(material));
		}

		matWorld = *Info.pMatWorld;
		g_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
		g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProj);

		matWorldViewProj = matWorld * matView * matProj;
		hResult = pShader->SetMatrix("WorldViewProj", &matWorldViewProj);
		hResult = pShader->SetMatrix("matWorld", &matWorld);

		if (g_ProjTextureRender.NeedProject(Info.pModel->m_BBox) && Info.pModel->GetBindType() == ENUM_BIND_NONE)
			g_ProjTextureRender.SetEffectParam(pShader);

		if (!pSubset->m_lpTextures[1])
		{
			g_pd3dDevice->SetTexture(1, NULL);
		}
		else
		{
			hResult = g_pd3dDevice->SetTexture(1, pSubset->m_lpTextures[1]->GetTexture());
			KG_COM_PROCESS_ERROR(hResult);
		}

		if (Info.pModel)
			g_SetPointLighting(pShader, Info.pModel->GetPointLightInfo(), MAX_POINTLIGHT, FALSE);
		_MtlAlphaOperation* pAlphaOp = pSubset->GetAlphaOperation();
		BOOL bEnableAlphaFactor = !(pAlphaOp->EnableAlphaTest || pAlphaOp->EnableAlphaBlend);

		_MtlRenderTwiceAlphaOption *pTwiceOption = pSubset->GetAlphaRenderTwice();
		if (pTwiceOption->bEnable)
		{
			bEnableAlphaFactor = FALSE;
		}

		pShader->SetBool("bAlphaUseFactor", bEnableAlphaFactor);

		if(Info.pModel->IsClientAdd())
		{
			g_SetMaterialDetailInfo(pShader,Info.pModel, Info.dwSubsetID);
		}

		pShader->CommitChanges();
	}

	return S_OK;
Exit0:
	SAFE_RELEASE(pTexture);
	return E_FAIL;
}


HRESULT KG3DSubMaterialGroup::BeginPass(KG3DModelSubsetInfo &Info,BOOL bForcePass0)
{
	HRESULT hRetCode = E_FAIL;
	HRESULT hResult = E_FAIL;
	DWORD dwPassID = 0;
	KG3DMaterial::KG3DMaterialSubset *pSubset = Info.pModel->GetMaterialSubset(Info.dwSubsetID);
	KG_PROCESS_ERROR(pSubset);
	dwPassID = pSubset->m_dwPassID;
	
    if (Info.pModel && g_ProjTextureRender.NeedProject(Info.pModel->m_BBox) && Info.pModel->GetBindType() == ENUM_BIND_NONE)
        dwPassID = dwPassID * 2 + 1;

	if(bForcePass0)
		dwPassID = 0;
	KG_PROCESS_SUCCESS(dwPassID == s_dwLastPass);

	ASSERT(s_pCurrentEffect);
	if (s_dwLastPass != INVALIDATE_PASS)
	{
		s_pCurrentEffect->EndPass();
		s_dwLastPass = INVALIDATE_PASS;
	}


	hResult = s_pCurrentEffect->BeginPass(dwPassID);
	KG_COM_PROCESS_ERROR(hResult);

	s_dwLastPass = dwPassID;
Exit1:
	ASSERT(s_CurrentShaderType < DEFST_COUNT);
	/*if (s_SetShaderConst[s_CurrentShaderType])
	{
		(*s_SetShaderConst[s_CurrentShaderType])(s_pCurrentEffect, Info,bSkipTexture2);
	}*/

	SetNewLightShaderConst(s_pCurrentEffect, Info, s_CurrentShaderType);


	hRetCode = S_OK;
Exit0:
	return hRetCode;
}

DWORD GetCullOptionByMaterial( KG3DMaterial::KG3DMaterialSubset& subMtl)
{
	if (subMtl.m_dwOption & MATERIAL_OPTION_CULL_NONE)
	{
		return D3DCULL_NONE;
	}
	else if (subMtl.m_dwOption & MATERIAL_OPTION_CULL_CCW)
	{
		return D3DCULL_CCW;
	}

	return D3DCULL_CW;
}



HRESULT KG3DSubMaterialGroup::Render(BOOL bForcePass0)
{
	HRESULT hResult = E_FAIL;
	HRESULT hRetCode = E_FAIL;
	D3DXMATRIX matSave;
	g_pd3dDevice->GetTransform(D3DTS_WORLD, &matSave);

	if(m_lpKG3DTexture)
		g_pd3dDevice->SetTexture(0,m_lpKG3DTexture->GetTexture());
	//////////////////////////////////////////////////////////////////////////
	/*DWORD AlphaBlend,AlphaTest;
	g_pd3dDevice->GetRenderState(D3DRS_ALPHATESTENABLE,&AlphaTest);
	g_pd3dDevice->GetRenderState(D3DRS_ALPHABLENDENABLE,&AlphaBlend);

	
	KG3DRenderState State;
	State.SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE2X);*/

	/*
	原来下面这里使用RenderState，会连续不断的保存各个状态，而其实状态是相互覆盖的
	，所以只要以开始保存所有状态，最后恢复就好了。中间的状态直接覆盖
	*/

	KG3DRenderState State;
	
	State.SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	State.SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
	State.SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE2X);
	State.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	State.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_DESTALPHA);
	State.SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	State.SetRenderState(D3DRS_BLENDFACTOR, 0);
				State.SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, FALSE);

	State.SetRenderState(D3DRS_BLENDOPALPHA, FALSE);

	State.SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_SRCALPHA);
	State.SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_DESTALPHA);

	State.SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	State.SetRenderState(D3DRS_ALPHAREF, 0);
	State.SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);

    for (size_t i = 0; i < m_vecSubsetMaterialSet.size(); ++i)
	{
		KG3DModelSubsetInfo* pInfo = m_vecSubsetMaterialSet[i];
		KG3DMaterial::KG3DMaterialSubset* pSubMtl = pInfo->pModel->GetMaterialSubset(pInfo->dwSubsetID);
		if (pSubMtl)
		{
			DWORD dwCull = GetCullOptionByMaterial(*pSubMtl);
			g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, dwCull);

			_MtlAlphaOperation* pMtlAlphaOp = pSubMtl->GetAlphaOperation();
			hResult = g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, pMtlAlphaOp->EnableAlphaBlend);
			hResult = g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, pMtlAlphaOp->SrcBlend);
			hResult = g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, pMtlAlphaOp->DestBlend);
			hResult = g_pd3dDevice->SetRenderState(D3DRS_BLENDOP, pMtlAlphaOp->BlendOperator);
			hResult = g_pd3dDevice->SetRenderState(D3DRS_BLENDFACTOR, pMtlAlphaOp->BlendFactor);
			hResult = g_pd3dDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE
				, pMtlAlphaOp->EnableSeparateAlphaBlend);
			hResult = g_pd3dDevice->SetRenderState(D3DRS_BLENDOPALPHA, pMtlAlphaOp->BlendOperatorAlpha);
			hResult = g_pd3dDevice->SetRenderState(D3DRS_SRCBLENDALPHA, pMtlAlphaOp->SrcBlendAlpha);
			hResult = g_pd3dDevice->SetRenderState(D3DRS_DESTBLENDALPHA, pMtlAlphaOp->DestBlendAlpha);
			hResult = g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, pMtlAlphaOp->EnableAlphaTest);
			hResult = g_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, pMtlAlphaOp->AlphaTestRef);
			hResult = g_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, pMtlAlphaOp->AlphaFunction);
			
			hResult = BeginPass(*pInfo, bForcePass0);
		}

		if(pInfo->pXMeshToRender)
		{
			pInfo->pXMeshToRender->DrawSubset(pInfo->dwSubsetID);
		}
	}

	State.Restore();
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matSave);
	/*g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE,AlphaTest);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,AlphaBlend);*/

	hRetCode =  S_OK;
	return hRetCode;
}

HRESULT KG3DSubMaterialGroup::RenderColorOnlyBegin(BOOL bForceAlphaTest)
{
    HRESULT hrResult = E_FAIL;

	KGLOG_PROCESS_ERROR(m_pRenderState);

    m_pRenderState->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    if(bForceAlphaTest)
    {
        m_pRenderState->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);
        m_pRenderState->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
    }
    else
    {
        m_pRenderState->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
        m_pRenderState->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
    }


    m_pRenderState->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE2X);

    if(bForceAlphaTest)
    {
        m_pRenderState->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
        m_pRenderState->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    }
    else
    {
        m_pRenderState->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_DISABLE);
    }
	
	for (int i = 0; i < 4; i++)
	{
		m_pRenderState->SetSamplerState(i, D3DSAMP_MINFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMinFilter);
		m_pRenderState->SetSamplerState(i, D3DSAMP_MAGFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMagFilter);
		m_pRenderState->SetSamplerState(i, D3DSAMP_MIPFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMipFilter);
		m_pRenderState->SetSamplerState(i, D3DSAMP_MAXANISOTROPY ,g_cEngineOption.dwMaxAnisotropy);
	}
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DSubMaterialGroup::RenderColorOnly(LPD3DXEFFECT pEffect,BOOL bDetail)
{
	HRESULT hResult = E_FAIL;
	HRESULT hRetCode = E_FAIL;
	D3DXMATRIX matSave;
	static D3DXMATRIX matId = *D3DXMatrixIdentity(&matId);


	g_pd3dDevice->GetTransform(D3DTS_WORLD, &matSave);

	if(m_lpKG3DTexture)
		g_pd3dDevice->SetTexture(0,m_lpKG3DTexture->GetTexture());

    for (size_t i = 0; i < m_vecSubsetMaterialSet.size(); ++i)
	{
		KG3DModelSubsetInfo* pInfo = m_vecSubsetMaterialSet[i];
		KG3DMaterial::KG3DMaterialSubset* pSubMtl = pInfo->pModel->GetMaterialSubset(pInfo->dwSubsetID);
		if (pSubMtl)
		{
			DWORD dwCull = GetCullOptionByMaterial(*pSubMtl);
			g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, dwCull);

			hResult = BeginPass(*pInfo, bDetail);
		}

		if(pInfo->pXMeshToRender && !pInfo->bPutInBuffer && !pInfo->pModel->m_bHideFromRenderTools)
		{
			pInfo->pXMeshToRender->DrawSubset(pInfo->dwSubsetID);
		}
	}

	//{
	//	DWORD dwFace = 0;
	//	LPDIRECT3DVERTEXBUFFER9 pVB = GetShareGrassVBuffer(dwFace);
	//	if(pVB && dwFace && pLastInfo)
	//	{
	//		D3DXMATRIX matView, matProjection, matWorldViewProj;	
	//		g_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
	//		g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProjection);
	//		matWorldViewProj = matView * matProjection;

	//		s_pCurrentEffect->SetMatrix("WorldViewProj", &matWorldViewProj);//在"default ligting with ambient map for editor new.fx"和"new light mode.fx"
	//		s_pCurrentEffect->CommitChanges();

	//		g_pd3dDevice->SetVertexDeclaration(g_cGraphicsTool.GetGrassVertexDec());
	//		g_pd3dDevice->SetStreamSource(0,pVB,0,sizeof(KG3DTerrainGrassVertex));
	//		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,dwFace);
	//	}
	//}

	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matSave);
	/*g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE,AlphaTest);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,AlphaBlend);*/

	hRetCode =  S_OK;
	return hRetCode;
}

HRESULT KG3DSubMaterialGroup::RenderColorOnlyEnd()
{
    m_pRenderState->Restore();

    return S_OK;
}

HRESULT KG3DSubMaterialGroup::RenderDepthOnlyBegin()
{
    HRESULT hrResult = E_FAIL;

	KGLOG_PROCESS_ERROR(m_pRenderState);

	DWORD dwColorWrite = 0;
	if(g_cEngineOption.bUseRenderTarget_DepthTexture)
	{
		if (g_nDepthTexturePackingType == 0)
			dwColorWrite = D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_ALPHA;
		else
			dwColorWrite = D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA;
	}

    m_pRenderState->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
    m_pRenderState->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    m_pRenderState->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_pRenderState->SetRenderState(D3DRS_CULLMODE,D3DCULL_CW);
    m_pRenderState->SetRenderState(D3DRS_LIGHTING,FALSE);
    m_pRenderState->SetRenderState(D3DRS_COLORWRITEENABLE ,dwColorWrite);
    m_pRenderState->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);
    m_pRenderState->SetRenderState(D3DRS_ALPHAREF, 0x00000008E);
    m_pRenderState->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DSubMaterialGroup::RenderDepthOnly()
{
	HRESULT hr = S_OK;
	DWORD dwNowTime = timeGetTime(); 
	D3DXMATRIX matSave;
	static D3DXMATRIX matId = *D3DXMatrixIdentity(&matId);
	g_pd3dDevice->GetTransform(D3DTS_WORLD, &matSave);

	if(m_lpKG3DTexture)
		g_pd3dDevice->SetTexture(0,m_lpKG3DTexture->GetTexture());


	//GraphicsStruct::RenderState colorWrite(D3DRS_COLORWRITEENABLE ,D3DCOLORWRITEENABLE_ALPHA);
	D3DXMATRIX matView, matProjection;	//把这两个从循环里面移出来
	g_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
	g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProjection);

    for (size_t i = 0; i < m_vecSubsetMaterialSet.size(); ++i)
	{
		KG3DModelSubsetInfo* pInfo = m_vecSubsetMaterialSet[i];
		if(pInfo->pXMeshToRender && pInfo->pMatWorld && !pInfo->bPutInBuffer && !pInfo->pModel->m_bHideFromRenderTools)
		{
			D3DXMATRIX matWorld, matWorldView, matWorldViewProj;	

			matWorld = *(pInfo->pMatWorld);
			
			matWorldViewProj = matWorld * matView * matProjection;
		
			s_pCurrentEffect->SetMatrix("WorldViewProj", &matWorldViewProj);//在"default ligting with ambient map for editor new.fx"和"new light mode.fx"
																			//都定义了float4x4 WorldViewProj

			KG3DMaterial::KG3DMaterialSubset* pSubMtl = pInfo->pModel->GetMaterialSubset(pInfo->dwSubsetID);
			if(pSubMtl)
			{
				_MtlAlphaOperation* pAlphaOp = pSubMtl->GetAlphaOperation();
				BOOL bEnableAlphaFactor = !(pAlphaOp->EnableAlphaTest || pAlphaOp->EnableAlphaBlend);

				_MtlRenderTwiceAlphaOption *pTwiceOption = pSubMtl->GetAlphaRenderTwice();
				if (pTwiceOption->bEnable)
				{
					bEnableAlphaFactor = FALSE;
				}

				s_pCurrentEffect->SetBool("bAlphaUseFactor", bEnableAlphaFactor);
				//g_pd3dDevice->SetTransform(D3DTS_WORLD,vInfo.pMatWorld);
				s_pCurrentEffect->CommitChanges();

				pInfo->pXMeshToRender->DrawSubset(pInfo->dwSubsetID);
			}
		}
	}

	//{
	//	DWORD dwFace = 0;
	//	LPDIRECT3DVERTEXBUFFER9 pVB = GetShareGrassVBuffer(dwFace);

	//	if(pVB && dwFace)
	//	{
	//		D3DXMATRIX matWorld, matView, matProjection, matWorldView, matWorldViewProj;	
	//		g_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
	//		g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProjection);
	//		matWorldViewProj = matView * matProjection;

	//		s_pCurrentEffect->SetMatrix("WorldViewProj", &matWorldViewProj);//在"default ligting with ambient map for editor new.fx"和"new light mode.fx"
	//		s_pCurrentEffect->CommitChanges();

	//		g_pd3dDevice->SetVertexDeclaration(g_cGraphicsTool.GetGrassVertexDec());
	//		g_pd3dDevice->SetStreamSource(0,pVB,0,sizeof(KG3DTerrainGrassVertex));
	//		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,dwFace);
	//	}
	//}

	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matSave);

	hr =  S_OK;

	DWORD dwCost = timeGetTime() - dwNowTime;
	if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost >= 15)
	{
		KGLogPrintf(KGLOG_ERR, "TimeOptimize %d KG3DSubMaterialGroup::RenderDepthOnly.",dwCost);
	}

	return hr;
}

HRESULT KG3DSubMaterialGroup::RenderDepthOnlyEnd()
{
    m_pRenderState->Restore();

    return S_OK;
}

HRESULT KG3DSubMaterialGroup::RenderReflect(float fWaterHeight,D3DXPLANE& WaterPlane,D3DXPLANE& WaterPlaneFX)
{
	HRESULT hr = E_FAIL;
	LPD3DXEFFECT pShader = NULL;
    KG3DModelSubsetInfo *pInfo = NULL;
    KG3DMaterial::KG3DMaterialSubset *pSubMtl = NULL;
    
    if (m_vecSubsetMaterialSet.empty())
        return S_OK;

	pShader = g_cGraphicsTool.GetDefaultShader(DEFST_NEWLIGHT);
	KG_PROCESS_ERROR(pShader);

	//////////////////////////////////////////////////////////////////////////
	g_pd3dDevice->SetRenderState(D3DRS_CLIPPLANEENABLE,D3DCLIPPLANE0);
	g_pd3dDevice->SetClipPlane(0,(float *)&WaterPlane);

    pInfo = m_vecSubsetMaterialSet[0];
    pSubMtl = pInfo->pModel->GetMaterialSubset(pInfo->dwSubsetID);
	ASSERT(pSubMtl);
	pInfo->pModel->Shader_DrawMeshSubset_Begin(pInfo->dwSubsetID,pSubMtl,pShader);

    for (size_t i = 0; i < m_vecSubsetMaterialSet.size(); ++i)
    {
        pInfo = m_vecSubsetMaterialSet[i];
        pSubMtl = pInfo->pModel->GetMaterialSubset(pInfo->dwSubsetID);
		ASSERT(pSubMtl);

        pInfo->pModel->Shader_DrawMeshSubset_NoChange(pInfo->dwSubsetID,pSubMtl,pShader);
    }
	pInfo->pModel->Shader_DrawMeshSubset_End(pInfo->dwSubsetID,pSubMtl,pShader);



	g_pd3dDevice->SetRenderState(D3DRS_CLIPPLANEENABLE,FALSE);

	hr =  S_OK;
Exit0:
	return hr;
}

HRESULT KG3DSubMaterialGroup::GetShareGrassVB(list<KG3DModelSubsetInfo*>&listSubsetToBuffer,DWORD dwNumFace)
{
	/*HRESULT hr = S_OK;
	BOOL bNeedRefresh = FALSE;
	if(!m_lpShareVB)
	{
		g_cTerrainBlockGrassShareVBHighPool.GetOneShareResource(&m_lpShareVB,this);
		bNeedRefresh = TRUE;
	}
	else if(m_lpShareVB->pUser!=this)
	{
		g_cTerrainBlockGrassShareVBHighPool.GetOneShareResource(&m_lpShareVB,this);
		bNeedRefresh = TRUE;
	}

	if(bNeedRefresh||m_bNeedRefresh)
	{
		hr = m_lpShareVB->CreateFromModels(dwNumFace,listSubsetToBuffer,this);
		if(FAILED(hr))
		{
			m_lpShareVB = NULL;
		}
		else
		{
			g_nLoadCount ++;
			g_nGrassVertexBufferFillPerFrame++;
		}
		m_bNeedRefresh = FALSE;
	}*/

	return S_OK;
//Exit0:
//	return E_FAIL;
}

LPDIRECT3DVERTEXBUFFER9 KG3DSubMaterialGroup::GetShareGrassVBuffer(DWORD& dwFaces)
{
	if(m_lpShareVB && m_lpShareVB->pUser == this)
	{
		dwFaces = m_lpShareVB->m_dwUsedFaces;
		return m_lpShareVB->m_pVB;
	}

	return NULL;
}

KG3DSubMaterialGroup::KG3DSubMaterialGroup()
{
	m_lpKG3DTexture = NULL;
	m_bNeedRefresh = FALSE;
    m_pRenderState = NULL;
}

KG3DSubMaterialGroup::~KG3DSubMaterialGroup()
{
	SAFE_RELEASE(m_lpKG3DTexture);
	m_bNeedRefresh = FALSE;
    ASSERT(!m_pRenderState);
}

HRESULT KG3DSubMaterialGroup::Optimize(BOOL bUnit)
{
	SAFE_RELEASE(m_lpKG3DTexture);
	//if(m_bNeedRefresh)
	{
		list<KG3DModelSubsetInfo*>listSubsetToBuffer;
        KG3DMaterial::KG3DMaterialSubset* pSubMtl = NULL;
		//DWORD dwFaceToBuffer = 0;

		int s = 0;
        for (size_t i = 0; i < m_vecSubsetMaterialSet.size(); ++i)
		{
			KG3DModelSubsetInfo* pInfo = m_vecSubsetMaterialSet[i];
			pInfo->bPutInBuffer = FALSE;
            pSubMtl = NULL;

			if(pInfo->pModel)
			{
				SAFE_RELEASE(pInfo->pXMeshToRender);

				pInfo->pXMeshToRender = pInfo->pModel->GetRenderXMesh();
				
				if(pInfo->pXMeshToRender)
					pInfo->pXMeshToRender->AddRef();

				pInfo->pMatWorld = &pInfo->pModel->m_matWorld;

                pSubMtl = pInfo->pModel->GetMaterialSubset(pInfo->dwSubsetID);
            }
			
			if(pSubMtl && s==0)
			{
				SAFE_RELEASE(m_lpKG3DTexture);
				m_lpKG3DTexture = pSubMtl->GetSubMeshTexture();
				if(m_lpKG3DTexture)
				{
					m_lpKG3DTexture->AddRef();
				}
				m_dwAlphaType = pSubMtl->GetAlphaType();
			}
			s++;
		}

		//////////////////////////////////////////////////////////////////////////
		//
		/*if (m_bNeedRefresh && dwFaceToBuffer && bUnit)
		{
			GetShareGrassVB(listSubsetToBuffer,dwFaceToBuffer);
		}*/
		//m_bNeedRefresh = FALSE;
	}

	return S_OK;
}


HRESULT KG3DSubMaterialGroup::PrintfProfile()
{
	DWORD count = 0;
	DWORD dwless =0;
	DWORD dwlesscount =0;

	vector<KG3DModelSubsetInfo*>::iterator i = m_vecSubsetMaterialSet.begin();
	vector<KG3DModelSubsetInfo*>::iterator iend = m_vecSubsetMaterialSet.end();

	KGLogPrintf(KGLOG_ERR," SUBSET排序组容量 %d",m_vecSubsetMaterialSet.size());
	KGLogPrintf(KGLOG_ERR," SUBSET排序组总面数 %d 小于200的有 %d ID,共 %d 面",count,dwless,dwlesscount);

	return S_OK;
}