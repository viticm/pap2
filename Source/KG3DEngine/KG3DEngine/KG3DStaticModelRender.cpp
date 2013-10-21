#include "StdAfx.h"
#include "KG3DStaticModelRender.h"
#include "KG3DShaderManager.h"
#include "KG3DModel.h"
#include "KG3DGraphicsTool.h"
#include "KG3DTerrainBlock.h"
#include "KG3DSubSetRenderer.h"
#include "KG3DSFXProjectionTextureMgr.h"
#include "KG3DScenePointLightRender.h"
#include "kg3dtexture.h"

extern DWORD g_dwRenderCount;

extern HRESULT g_SetMaterialDetailInfo(LPD3DXEFFECT pEffect,KG3DModel* pModel, int nSubSetID);
extern int g_nGrassVertexBufferFillPerFrame;

KG3DStaticModelRender::KG3DStaticModelRender(void)
{
	m_lpVertexDeclarationNormal = NULL;
	m_lpVertexDeclaration = NULL;
}

KG3DStaticModelRender::~KG3DStaticModelRender(void)
{
	UnInit();
}
 

HRESULT KG3DStaticModelRender::ModelContainer::Init(BOOL bPlayer)
{
	m_bRenderPlayer = bPlayer;

	if(bPlayer)
	{
		TCHAR ShaderPath[] = _T("data\\public\\shader\\MultiTextureSkin.fx");
		m_vShader = g_GetShaderManager().Create(g_pd3dDevice, ShaderPath);	
	}
	else
	{
		if(g_cEngineOption.bEnableNormalMap)
		{
			TCHAR ShaderPath[] = _T("data\\public\\shader\\MultiTexture_normalmap.fx");
			m_vShader = g_GetShaderManager().Create(g_pd3dDevice, ShaderPath);	
		}
		else
		{
			TCHAR ShaderPath[] = _T("data\\public\\shader\\MultiTexture.fx");
			m_vShader = g_GetShaderManager().Create(g_pd3dDevice, ShaderPath);	
		}
	}

	return S_OK;
}

HRESULT KG3DStaticModelRender::ModelContainer::UnInit()
{
	map<DWORD, ModelSet >::iterator i = m_mapModelSet.begin();
	map<DWORD, ModelSet >::iterator iend = m_mapModelSet.end();

	while (i!=iend)
	{
		ModelSet& models = i->second;

		set<KG3DModel*>::iterator k = models.begin();
		set<KG3DModel*>::iterator kend = models.end();
		while (k!=kend)
		{
			KG3DModel* pModel = *k;
			pModel->Release();
			k++;
		}
		models.clear();
		i++;
	}
	m_mapModelSet.clear();

	return S_OK;
}

HRESULT KG3DStaticModelRender::Init()
{
	HRESULT hr = S_OK;
	D3DVERTEXELEMENT9 VertexElement[] = {
		{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION,  0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_NORMAL,    0 },
		{ 0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD,  0 },
		D3DDECL_END()
	};

	hr = g_pd3dDevice->CreateVertexDeclaration(VertexElement, &m_lpVertexDeclaration);
	KGLOG_COM_PROCESS_ERROR(hr);

	D3DVERTEXELEMENT9 VertexElementNormal[] = {
		{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION,  0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_NORMAL,    0 },
		{ 0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD,  0 },
		{ 1, 0 , D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TANGENT ,  0 },
		D3DDECL_END()
	};

	hr = g_pd3dDevice->CreateVertexDeclaration(VertexElementNormal, &m_lpVertexDeclarationNormal);
	KGLOG_COM_PROCESS_ERROR(hr);	
	m_vModelContainerScene.Init(FALSE);
	m_vModelContainerPlayer.Init(TRUE);

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DStaticModelRender::UnInit()
{
	SAFE_RELEASE(m_lpVertexDeclaration);
	SAFE_RELEASE(m_lpVertexDeclarationNormal);

	m_vModelContainerScene.UnInit();
	m_vModelContainerPlayer.UnInit();
	return S_OK;
}

HRESULT KG3DStaticModelRender::ModelContainer::FrameMove(KG3DScenePointLightRender* pLights)
{
	DWORD dwRemoveCount = 20;
	map<DWORD, ModelSet >::iterator i = m_mapModelSet.begin();
	map<DWORD, ModelSet >::iterator iend = m_mapModelSet.end();

	while (i!=iend)
	{
		ModelSet& models = i->second;

		set<KG3DModel*>::iterator k = models.begin();
		set<KG3DModel*>::iterator kend = models.end();
		while (k!=kend)
		{
			KG3DModel* pModel = *k;
			if(g_dwRenderCount - pModel->GetVisibleCount() > dwRemoveCount)
			{
				k = models.erase(k);
				pModel->Release();
			}
			else
			{
				pLights->FindRefrenceLightForModel(pModel);
				k++;
			}
		}

		i++;
	}

	return S_OK;
}

HRESULT KG3DStaticModelRender::FrameMove(KG3DScenePointLightRender* pLights)
{
	if(g_dwRenderCount % 10 != 0)
		return S_OK;

	m_vModelContainerScene.FrameMove(pLights);
	m_vModelContainerPlayer.FrameMove(pLights);

	return S_OK;
}

HRESULT KG3DStaticModelRender::ModelContainer::RenderModelGroup(ModelSet& models,LPD3DXEFFECT pEffect,BOOL bUpdateMatrix,LPDIRECT3DVERTEXDECLARATION9 pDeclaration)
{
	HRESULT hr = E_FAIL;
	DWORD dwNowTime = timeGetTime();
	GraphicsStruct::RenderState cull (D3DRS_CULLMODE,D3DCULL_CW);
	GraphicsStruct::RenderState alphafun(D3DRS_ALPHAFUNC,D3DCMP_GREATEREQUAL);
	DWORD dwAlphaTest = 0;
	g_pd3dDevice->GetRenderState(D3DRS_ALPHATESTENABLE,&dwAlphaTest);

	int nTextureSart = 0;
	BOOL bRenderNoFinish = TRUE;

	while (bRenderNoFinish)
	{
		bRenderNoFinish = FALSE;

		set<KG3DModel*>::iterator k = models.begin();
		set<KG3DModel*>::iterator kend = models.end();
		int N = 0;
		int nLast = (int)models.size() -1;
		while (k!=kend)
		{
			KG3DModel* pModel = *k;
			KGLOG_PROCESS_ERROR(pModel);

			KG3DMesh* pMesh = pModel->GetMesh();
			KGLOG_PROCESS_ERROR(pMesh);

			KGLOG_PROCESS_ERROR(pMesh->m_lpCpuProcessData);

			if(!pMesh->m_lpCpuProcessData->m_lpVB)
			{
				if(g_nGrassVertexBufferFillPerFrame < 1 )
				{
					pMesh->m_lpCpuProcessData->PrepareRender();

					g_nGrassVertexBufferFillPerFrame++;
				}
			}

			KG_PROCESS_ERROR(pMesh->m_lpCpuProcessData->m_lpVB);

			if(N == 0)
			{
				g_pd3dDevice->SetVertexDeclaration(pDeclaration);

				g_pd3dDevice->SetIndices(pMesh->m_lpCpuProcessData->m_lpIB);
				g_pd3dDevice->SetStreamSource(0,pMesh->m_lpCpuProcessData->m_lpVB,0,sizeof(KG3DCpuProcessVertexFormat));
				g_pd3dDevice->SetStreamSource(1,pMesh->m_lpCpuProcessData->m_lpVBTangent,0,sizeof(D3DXVECTOR3));

				KG3DMaterial* pMtl = pModel->GetMaterial();
				KG3DMaterial::KG3DMaterialSubset* pSubMtl = NULL;
				int nTexture  = pMtl->GetNumMaterial() - nTextureSart;

				if(pMtl->GetNumMaterial() >12)
				{
					g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);
				}

				int nPass = 0;

				bRenderNoFinish = FALSE;
				if(nTexture<=4)
					nPass = 0;
				else if (nTexture<=8)
					nPass = 1;
				else if(nTexture<=12)
					nPass = 2;
				else
				{
					nTexture = 12;
					nPass = 2;
					bRenderNoFinish = TRUE;
				}

				for (int i=0;i<nTexture;i++)
				{
					pSubMtl = pMtl->GetSubMaterial(i + nTextureSart);
					if(pSubMtl)
					{
                        if(m_bRenderPlayer)
                        {
                            g_SetMaterialDetailInfo(pEffect,pModel, i + nTextureSart);
                        }
						KG3DTexture* pTexture = pSubMtl->GetSubMeshTexture();
						if(pTexture)
							pTexture->SetTexture(g_pd3dDevice,i);

						g_pd3dDevice->SetSamplerState(i,D3DSAMP_MAGFILTER,(D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMagFilter);
						g_pd3dDevice->SetSamplerState(i,D3DSAMP_MINFILTER,(D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMinFilter);
						g_pd3dDevice->SetSamplerState(i,D3DSAMP_MIPFILTER,(D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMipFilter);
						g_pd3dDevice->SetSamplerState(i, D3DSAMP_MAXANISOTROPY ,g_cEngineOption.dwMaxAnisotropy);

						if(g_cEngineOption.bEnableNormalMap && nTexture<=4)
						{
							KG3DTexture* pNormal = pSubMtl->GetSubMeshTextureNormal();
							if(pNormal)
								pNormal->SetTexture(g_pd3dDevice,i+4);
							g_pd3dDevice->SetSamplerState(i+4,D3DSAMP_MAGFILTER,(D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMagFilter);
							g_pd3dDevice->SetSamplerState(i+4,D3DSAMP_MINFILTER,(D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMinFilter);
							g_pd3dDevice->SetSamplerState(i+4,D3DSAMP_MIPFILTER,(D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMipFilter);
							g_pd3dDevice->SetSamplerState(i+4, D3DSAMP_MAXANISOTROPY ,g_cEngineOption.dwMaxAnisotropy);
						}
					}
				}
				/*for(int i=nTexture;i<12;i++)
				{
					g_pd3dDevice->SetTexture(i,NULL);
				}*/

				pEffect->SetInt("nTextureStart",nTextureSart);
				pEffect->BeginPass(nPass);

				nTextureSart += nTexture;
			}


			//////////////////////////////////////////////////////////////////////////
			if(g_cEngineOption.bCpuSkin && bUpdateMatrix)
			{
				if(pModel->GetBindType() == ENUM_BIND_SOCKET)
				{
					pModel->Helper_UpdateTransformation(TRUE);
				}
			}

			//////////////////////////////////////////////////////////////////////////
			pEffect->SetMatrix("mWorld", &pModel->m_matWorld);

			g_SetPointLighting(pEffect,pModel->GetPointLightInfo());

			//pEffect->CommitChanges();

			if(!pModel->m_bHideFromRenderTools && pModel->m_bVisible/* && pModel->GetAlpha()>0.1F*/)
			{
				if(pModel->IsAnimationed())
				{
					LPD3DXMESH pMesh = pModel->GetRenderXMesh();
					if(pMesh)
					{
						KG3DMaterial* pMtl = pModel->GetMaterial();
						int nSubset = pMtl->GetNumMaterial();
						for (int s=0;s<nSubset;s++)
						{
							pMesh->DrawSubset(s);
						}
					}
				}
				else
				{
					g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,pMesh->m_lpCpuProcessData->m_dwNumVertex,0,pMesh->m_lpCpuProcessData->m_dwNumFace);
				}
			}
			//////////////////////////////////////////////////////////////////////////
			if(N == nLast)
			{
				pEffect->EndPass();
			}

			k++;
			N++;
		}
	}

	g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE,dwAlphaTest);

	hr = S_OK;
Exit0:
	DWORD dwCost = timeGetTime() - dwNowTime;
	if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost >= 15)
	{
		KGLogPrintf(KGLOG_ERR, "TimeOptimize %d KG3DStaticModelRender::ModelContainer::RenderModelGroup.",dwCost);
	}
	return hr;
}

HRESULT KG3DStaticModelRender::Render()
{
	m_vModelContainerScene.Render(this,FALSE);

	m_vModelContainerPlayer.Render(this,TRUE);

	return S_OK;
}

HRESULT KG3DStaticModelRender::ModelContainer::SetShaderParam(BOOL bPlayer)
{
	LPD3DXEFFECT pEffect = m_vShader.GetEffect();
	D3DXMATRIX matView, matProj, matViewProj;
	g_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
	g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	matViewProj = matView * matProj;
	pEffect->SetMatrix("mViewProj", &matViewProj);

	/*KG3DTerrainConverMap* pConverMap = g_cGraphicsTool.GetCurrentConverMap();
	if (pConverMap)
	{
		pEffect->SetVector("vConverRect",&pConverMap->GetRect());
		pEffect->SetTexture("tConver",pConverMap->GetRenderTargetTex());
	}*///<ConverMap Shared>
	if(!bPlayer)
		g_ProjTextureRender.SetEffectParam(pEffect);
	//g_SetShadowInfo(pEffect);//<ShadowParamMoveToShared>
	
	return S_OK;
}

HRESULT KG3DStaticModelRender::ModelContainer::Render(KG3DStaticModelRender* pRender,BOOL bPlayer)
{
	//GraphicsStruct::RenderState state(D3DRS_FILLMODE,D3DFILL_WIREFRAME);

	LPD3DXEFFECT pEffect = m_vShader.GetEffect();
	SetShaderParam(bPlayer);
	UINT uPass = 0;

	if(!bPlayer)
		pEffect->SetTechnique("Tech");
	else
		pEffect->SetTechnique("Tech_Channel");

	pEffect->Begin(&uPass,0);
	//////////////////////////////////////////////////////////////////////////

	map<DWORD, ModelSet >::iterator i = m_mapModelSet.begin();
	map<DWORD, ModelSet >::iterator iend = m_mapModelSet.end();

	while (i!=iend)
	{
		ModelSet& models = i->second;

		if(g_cEngineOption.bEnableNormalMap)
			RenderModelGroup(models,pEffect,FALSE,pRender->m_lpVertexDeclarationNormal);
		else
			RenderModelGroup(models,pEffect,FALSE,pRender->m_lpVertexDeclaration);

		i++;
	}

	pEffect->End();
	return S_OK;
}

HRESULT KG3DStaticModelRender::AddModel(KG3DModel* pModel)
{
	KGLOG_PROCESS_ERROR(pModel);
	
	if(pModel->IsClientAdd())
	{
		m_vModelContainerPlayer.AddModel(pModel);
	}
	else
	{
		m_vModelContainerScene.AddModel(pModel);
	}

Exit0:
	return S_OK;
}

HRESULT KG3DStaticModelRender::ModelContainer::AddModel(KG3DModel* pModel)
{

	KG3DMesh* pMesh = pModel->GetMesh();
	KGLOG_PROCESS_ERROR(pMesh);

	DWORD dwID = pMesh->GetID();

	{
		map<DWORD, ModelSet >::iterator i = m_mapModelSet.find(dwID);
		if(i == m_mapModelSet.end())
		{
			ModelSet newset;
			newset.insert(pModel);
			pModel->AddRef();
			pModel->SetVisibleCount(g_dwRenderCount);
			m_mapModelSet[dwID] = newset;
		}
		else
		{
			ModelSet& models = i->second;
			set<KG3DModel*>::iterator s = models.find(pModel);
			if(s == models.end())
			{
				models.insert(pModel);
				pModel->SetVisibleCount(g_dwRenderCount);
				pModel->AddRef();
			}
		}
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DStaticModelRender::RenderForDepth()
{
	m_vModelContainerScene.RenderForDepth(this);

	m_vModelContainerPlayer.RenderForDepth(this);

	return S_OK;
}

HRESULT KG3DStaticModelRender::ModelContainer::RenderForDepth(KG3DStaticModelRender* pRender)
{
	DWORD dwNowTime = timeGetTime();
	LPD3DXEFFECT pEffect = m_vShader.GetEffect();
	D3DXMATRIX matView, matProj, matViewProj;
	g_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
	g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	matViewProj = matView * matProj;
	pEffect->SetMatrix("mViewProj", &matViewProj);

	UINT uPass = 0;

	pEffect->SetTechnique("TechZ");
	pEffect->Begin(&uPass,0);
	//////////////////////////////////////////////////////////////////////////

	map<DWORD, ModelSet >::iterator i = m_mapModelSet.begin();
	map<DWORD, ModelSet >::iterator iend = m_mapModelSet.end();

	while (i!=iend)
	{
		ModelSet& models = i->second;

		RenderModelGroup(models,pEffect,TRUE,pRender->m_lpVertexDeclaration);

		i++;
	}
	pEffect->End();

	DWORD dwCost = timeGetTime() - dwNowTime;
	if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost >= 15)
	{
		KGLogPrintf(KGLOG_ERR, "TimeOptimize %d KG3DStaticModelRender::ModelContainer::RenderForDepth.",dwCost);
	}
	return S_OK;

}
DWORD KG3DStaticModelRender::GetNumModel()
{
	return m_vModelContainerPlayer.GetNumModel() + m_vModelContainerScene.GetNumModel();
}

HRESULT KG3DStaticModelRender::GetAllModels(vector<IEKG3DModel*>* pvecModels)
{
	m_vModelContainerPlayer.GetAllModels(pvecModels);
	m_vModelContainerScene.GetAllModels(pvecModels);
	return S_OK;
}

DWORD KG3DStaticModelRender::ModelContainer::GetNumModel()
{
	DWORD dwCount = 0;
	map<DWORD, ModelSet >::iterator i = m_mapModelSet.begin();
	map<DWORD, ModelSet >::iterator iend = m_mapModelSet.end();

	while (i!=iend)
	{
		ModelSet& models = i->second;

		dwCount += (DWORD)models.size();

		i++;
	}

	return dwCount;
}

HRESULT KG3DStaticModelRender::ModelContainer::GetAllModels(vector<IEKG3DModel*>* pvecModels)
{
	map<DWORD, ModelSet >::iterator i = m_mapModelSet.begin();
	map<DWORD, ModelSet >::iterator iend = m_mapModelSet.end();

	while (i!=iend)
	{
		ModelSet& models = i->second;

		set<KG3DModel*>::iterator s = models.begin();
		set<KG3DModel*>::iterator send = models.end();
		while (s!=send)
		{
			KG3DModel* pModel = *s;
			pvecModels->push_back(pModel);
			s++;
		}
		i++;
	}

	return S_OK;
}