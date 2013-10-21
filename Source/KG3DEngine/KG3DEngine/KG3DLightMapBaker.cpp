#include "StdAfx.h"
#include "KG3DLightMapBaker.h"
#include "KG3DScenePvsEditor.h"
#include "KG3DModelPointLight.h"
#include "KG3DIntersection.h"
#include "KG3DModelLightMap.h"
#include "KG3DSceneShadowMap.h"

//////////////////////////////////////////////////////////////////////////
void D3DUtil_GetCubeMapViewMatrix( DWORD dwFace,D3DXVECTOR3 vEyePt,KG3DCamera& vCamera ) 
{ 
	D3DXVECTOR3 vLookDir; 
	D3DXVECTOR3 vUpDir; 

	switch( dwFace ) 
	{ 
	case D3DCUBEMAP_FACE_POSITIVE_X: 
		vLookDir = D3DXVECTOR3( 1.0f, 0.0f, 0.0f ); 
		vUpDir  = D3DXVECTOR3( 0.0f, 1.0f, 0.0f ); 
		break; 
	case D3DCUBEMAP_FACE_NEGATIVE_X: 
		vLookDir = D3DXVECTOR3(-1.0f, 0.0f, 0.0f ); 
		vUpDir  = D3DXVECTOR3( 0.0f, 1.0f, 0.0f ); 
		break; 
	case D3DCUBEMAP_FACE_POSITIVE_Y: 
		vLookDir = D3DXVECTOR3( 0.0f, 1.0f, 0.0f ); 
		vUpDir  = D3DXVECTOR3( 0.0f, 0.0f,-1.0f ); 
		break; 
	case D3DCUBEMAP_FACE_NEGATIVE_Y: 
		vLookDir = D3DXVECTOR3( 0.0f,-1.0f, 0.0f ); 
		vUpDir  = D3DXVECTOR3( 0.0f, 0.0f, 1.0f ); 
		break; 
	case D3DCUBEMAP_FACE_POSITIVE_Z: 
		vLookDir = D3DXVECTOR3( 0.0f, 0.0f, 1.0f ); 
		vUpDir  = D3DXVECTOR3( 0.0f, 1.0f, 0.0f ); 
		break; 
	case D3DCUBEMAP_FACE_NEGATIVE_Z: 
		vLookDir = D3DXVECTOR3( 0.0f, 0.0f,-1.0f ); 
		vUpDir  = D3DXVECTOR3( 0.0f, 1.0f, 0.0f ); 
		break; 
	} 

	vLookDir += vEyePt;

	vCamera.SetPositionLookAtUp(&vEyePt,&vLookDir,&vUpDir);
} 
//////////////////////////////////////////////////////////////////////////
KG3DLightMapBaker::KG3DLightMapBaker(void)
{
	m_nNumPhotonTotal = 0;
	m_nComputedPhoton = 0;
	m_nComputedModel  = 0;
	m_lpPhotons = NULL;
	m_bShowDebug = FALSE;
	m_lpShadowMapForDiectionLight = NULL;
	m_lpPointLightTool = NULL;
	m_lpEnvLightTool = NULL;
	m_lpJitterTexture = NULL;

	m_bNeedEnableLightmap2 = FALSE;
	m_bNeedDisableLightmap2 = FALSE;

	m_bComputeDirectLight = FALSE;
	m_bComputeIndirectLight = FALSE;

	m_vRenderTargetToolDepth.Init(512,FALSE,D3DFMT_R32F);
	m_vRenderTargetToolEnvColor.Init(128,FALSE,D3DFMT_A8R8G8B8);
	m_vRenderTargetToolEnvDepth.Init(128,FALSE,D3DFMT_R32F);
}

KG3DLightMapBaker::~KG3DLightMapBaker(void)
{
	m_nNumPhotonTotal = 0;
	m_nComputedPhoton = 0;
	m_nComputedModel  = 0;
	SAFE_DELETE_ARRAY(m_lpPhotons);
	SAFE_DELETE(m_lpShadowMapForDiectionLight);
	SAFE_DELETE(m_lpPointLightTool);
	SAFE_DELETE(m_lpEnvLightTool);
	SAFE_RELEASE(m_lpJitterTexture);

	m_vRenderTargetTool.UnInit();
	m_vRenderTargetToolDepth.UnInit();
	m_vRenderTargetToolEnvColor.UnInit();
	m_vRenderTargetToolEnvDepth.UnInit();
	UninitEvnLighting();
}

int KG3DLightMapBaker::ComputeLightMapSize(KG3DModel* pModel)
{
	float D = pModel->m_BBox.GetDiameter() * 0.01F;
	if(D<=1)
		return 32;
	else if(D<=5)
		return 64;
	else if(D<=20)
		return 128;
	else if(D<=60)
		return 256;
	else if(D<=120)
		return 512;
	else  if(D<250)
		return 1024;
	else 
		return 2048;
}

int g_nPhotonCountPerLight = 10000;
int g_nReflectPhotonCount = 10;
float g_fReflectPhontPower = 5;

HRESULT KG3DLightMapBaker::InitFromPVS(KG3DScenePvsEditor* pScene)
{
	HRESULT hr = E_FAIL;

	IEKG3DRepresentPVS* pvs = pScene->GetPvs();

	m_lpPvsScene = pScene;

	m_vecModel.clear();
	m_vecPointLight.clear();
	//////////////////////////////////////////////////////////////////////////
	{
		KG3DRepresentObjectPVS* pPVS = static_cast<KG3DRepresentObjectPVS*>(pvs);

		pPVS->GetModels(&m_vecModel,&m_vecPointLight,FALSE);
	}

	//int PhotonCount = g_nPhotonCountPerLight;
	////////////////////////////////////////////////////////////////////////////
	//if(m_vecPointLight.size())
	//{
	//	m_nNumPhotonTotal = m_vecPointLight.size() * PhotonCount * (1 + g_nReflectPhotonCount);

	//	m_lpPhotons = new Photon[m_nNumPhotonTotal];
	//	KGLOG_PROCESS_ERROR(m_lpPhotons);

	//	ZeroMemory(m_lpPhotons,sizeof(Photon)*m_nNumPhotonTotal);

	//	m_nComputedPhoton = 0;
	//	m_nComputedModel  = 0;

	//	int nCount = 0;
	//	for (size_t i=0;i<m_vecPointLight.size();i++)
	//	{
	//		KG3DModelPointLight* pLight = m_vecPointLight[i];

	//		for (int n=0;n<PhotonCount;n++)
	//		{
	//			nCount = n * (g_nReflectPhotonCount+1);

	//			m_lpPhotons[nCount].vPosition.x = pLight->m_matWorld._41;
	//			m_lpPhotons[nCount].vPosition.y = pLight->m_matWorld._42;
	//			m_lpPhotons[nCount].vPosition.z = pLight->m_matWorld._43;

	//			float a = (rand()%20000 - 10000)*0.0001F;
	//			float b = (rand()%20000 - 10000)*0.0001F;
	//			float c = (rand()%20000 - 10000)*0.0001F;
	//			D3DXVECTOR3 N(a,b,c);
	//			D3DXVec3Normalize(&m_lpPhotons[nCount].vDirection,&N);

	//			m_lpPhotons[nCount].vColor = Value2Color(pLight->m_vColor);

	//		}
	//	}
	//}
	//////////////////////////////////////////////////////////////////////////
	//启动直射光阴影图
	SAFE_DELETE(m_lpShadowMapForDiectionLight);
	m_lpShadowMapForDiectionLight = new KG3DShadowMapLevel;
	KGLOG_PROCESS_ERROR(m_lpShadowMapForDiectionLight);

	m_lpShadowMapForDiectionLight->m_bNeedColorSurface = TRUE;
	hr = m_lpShadowMapForDiectionLight->Init(2048);
	KGLOG_COM_PROCESS_ERROR(hr);

	//////////////////////////////////////////////////////////////////////////
	//启动点光源阴影
	m_lpPointLightTool = new KG3DPointLightShadow;
	KGLOG_PROCESS_ERROR(m_lpPointLightTool);

	hr = m_lpPointLightTool->Init(512);
	KGLOG_COM_PROCESS_ERROR(hr);

	if(!m_lpEnvLightTool)
	{
		m_lpEnvLightTool = new KG3DPointLightShadow;
	}
	KGLOG_PROCESS_ERROR(m_lpEnvLightTool);

	hr = m_lpEnvLightTool->Init(128);
	KGLOG_COM_PROCESS_ERROR(hr);
	//////////////////////////////////////////////////////////////////////////
	for (size_t i=0;i<m_vecModel.size();i++)
	{
		KG3DModel* pModel = m_vecModel[i];
		if(pModel->IsSecondUVExist())
		{
			int nSize = ComputeLightMapSize(pModel);
			pModel->EnableLightmap(TRUE, nSize ,TRUE,"");
			pModel->m_eRuntimeShaderType = DEFST_LIGHT_MAP;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	hr = g_cGraphicsTool.InitJitter(&m_lpJitterTexture);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = InitEvnLighting();
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DLightMapBaker::Render()
{
	/*if(m_nComputedPhoton < m_nNumPhotonTotal)
	{
		GraphicsStruct::RenderState light(D3DRS_LIGHTING,FALSE);

		g_pd3dDevice->SetTexture(0,NULL);
		g_pd3dDevice->SetTexture(1,NULL);
		g_pd3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE);
		g_pd3dDevice->DrawPrimitiveUP(D3DPT_POINTLIST,m_nComputedPhoton,m_lpPhotons,sizeof(Photon));
	}*/

	if(0)
	{
		for (size_t i=0;i<m_vecEnvLight.size();i++)
		{
			EnvLight* pLight = m_vecEnvLight[i];
			D3DXMATRIX mat;
			D3DXMatrixTranslation(&mat,pLight->vPosition.x,pLight->vPosition.y,pLight->vPosition.z);
			D3DCOLORVALUE color = Color2Value(0xFFFFFFFF);

			g_pd3dDevice->SetTexture(0,pLight->pCubeMapColor);
			GraphicsStruct::TextureStageState s(0,D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_CAMERASPACENORMAL);
			//pShader->CommitChanges();
			g_cGraphicsTool.DrawSphere(&mat,color,40);
		}
	}

	{
		TCHAR szString[MAX_PATH];
		wsprintf(szString,"DirectLight:%d EnvLight:%d InDirect:%d",
			m_listModelForBake.size(),
			m_vecEnvLightRefresh.size(), 
			m_listModelForBakeEnv.size());
		g_cGraphicsTool.DrawDebugText(szString);
	}

	return S_OK;
}

HRESULT KG3DLightMapBaker::ComputeOnePhoton()
{
	if(m_nComputedPhoton < m_nNumPhotonTotal)
	{
		float fDisTance = 0;
		D3DXVECTOR3 vFaceNormal(0,0,0);

		BOOL bInter = RayIntersect(
			&m_lpPhotons[m_nComputedPhoton],
			&fDisTance,
			&vFaceNormal);

		if(!bInter)
			fDisTance = 100000;

		m_lpPhotons[m_nComputedPhoton].vPosition += m_lpPhotons[m_nComputedPhoton].vDirection * fDisTance;
		m_lpPhotons[m_nComputedPhoton].fDistance = fDisTance;

		float K = g_fReflectPhontPower / g_nReflectPhotonCount;

		for (int i=0;i<g_nReflectPhotonCount;i++)
		{
			Photon* pPhotonSrc = &m_lpPhotons[m_nComputedPhoton];
			Photon* pPhotonReflect = &m_lpPhotons[m_nComputedPhoton + 1 + i];

			if(bInter)
			{

				pPhotonReflect->vPosition  = pPhotonSrc->vPosition - pPhotonSrc->vDirection * 10;

				D3DCOLORVALUE col = Color2Value(pPhotonSrc->vColor);
				col.a *= K;
				col.r *= K;
				col.g *= K;
				col.b *= K;
				pPhotonReflect->vColor = Value2Color(col);

				pPhotonReflect->fDistance  = pPhotonSrc->fDistance;

				float a = (rand()%20000 - 10000)*0.0001F;
				float b = (rand()%20000 - 10000)*0.0001F;
				float c = (rand()%20000 - 10000)*0.0001F;
				D3DXVECTOR3 N(a,b,c);
				D3DXVec3Normalize(&pPhotonReflect->vDirection,&(N*0.5 - pPhotonSrc->vDirection ));

				BOOL bReflectInter = RayIntersect(
					&m_lpPhotons[m_nComputedPhoton + 1 + i],
					&fDisTance,
					&vFaceNormal);

				if(!bReflectInter)
					fDisTance = 100000;

				pPhotonReflect->vPosition += pPhotonReflect->vDirection * fDisTance;
				pPhotonReflect->fDistance += fDisTance;
			}
			else
			{
				(*pPhotonReflect) = (*pPhotonSrc);
			}
		}

		m_nComputedPhoton += g_nReflectPhotonCount+1;
	}

	return S_OK;
}

HRESULT KG3DLightMapBaker::FrameMove()
{
	if(m_bComputeDirectLight)
	{
		ProcessDirectLighting();
	}

	if(m_bComputeIndirectLight)
	{
		ProcessIndirectLighting();
	}

	return S_OK;
}


HRESULT KG3DLightMapBaker::ComputeCubeMapForEnvLight(EnvLight* pEnv)
{
	HRESULT hr = E_FAIL;
	KG3DRepresentObjectPVS* pPVS = (KG3DRepresentObjectPVS*)m_lpPvsScene->GetPvs();
	KG3DCamera vCamera;

	KGLOG_PROCESS_ERROR(m_lpEnvLightTool);
	KGLOG_PROCESS_ERROR(pEnv);

	hr = m_lpEnvLightTool->ComputeCubeMapForPosition(pEnv->vPosition,
		m_lpPvsScene,
		pEnv->pCubeMapColor,
		pEnv->pCubeMapDepth,
		&m_vRenderTargetToolEnvColor,
		&m_vRenderTargetToolEnvDepth);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = S_OK;
Exit0:
	return hr;
}

BOOL KG3DLightMapBaker::RayIntersect(Photon* pPhoton,float* pfDistance,D3DXVECTOR3* pFaceNormal)
{
	BOOL bIntersected = FALSE;
	float fDistMin = 100000;

	for (size_t i=0;i<m_vecModel.size();i++)
	{
		KG3DModel* pModel = m_vecModel[i];

		float fDist = 100000;

		BOOL bInter = pModel->RayIntersection(pPhoton->vPosition,pPhoton->vDirection,&fDist,pFaceNormal,FALSE);
		if(bInter)
		{
			fDistMin = min(fDistMin,fDist);
			bIntersected = TRUE;
		}
	}
	(*pfDistance) = fDistMin;
	return bIntersected;
}


HRESULT KG3DLightMapBaker::RenderPointLightMapForModel(KG3DModel* pModel)
{
	KG3DDefaultShaderType eShaderSave = pModel->m_eRuntimeShaderType;
	pModel->m_eRuntimeShaderType = DEFST_LIGHT_MAP_BAKE_POINTLIGHT;
	//////////////////////////////////////////////////////////////////////////
	LPD3DXEFFECT pShader = g_cGraphicsTool.GetDefaultShader(DEFST_LIGHT_MAP_BAKE_POINTLIGHT);

	for (size_t i=0;i<m_vecPointLight.size();i++)
	{
		KG3DModelPointLight* pLight = m_vecPointLight[i];
	
		{
			D3DXVECTOR4 vLight;
			vLight.x = pLight->m_matWorld._41;
			vLight.y = pLight->m_matWorld._42;
			vLight.z = pLight->m_matWorld._43;
			vLight.w = pLight->m_BBox.GetDiameter();

			pShader->SetVector("vPointLightPosition",&vLight);
			pShader->SetVector("vPointLightColor",(D3DXVECTOR4*)&pLight->m_vColor);
			pShader->SetTexture("tShadowmap1",pLight->m_lpCubeTexture);
			pShader->SetTexture("Jitter", m_lpJitterTexture);

			D3DXVECTOR4 noise[64];
			for (int s=0;s<64;s++)
			{
				noise[s].x = 0.01F * (rand()%200 -100);
				noise[s].y = 0.01F * (rand()%200 -100);
				noise[s].z = 0.01F * (rand()%200 -100);
				noise[s].w = 0;
			}
			pShader->SetVectorArray("vUSNoise",noise,64);

			pShader->CommitChanges();
		}
		//////////////////////////////////////////////////////////////////////////
		{
			GraphicsStruct::RenderStateAlpha alpha(0x80,FALSE,TRUE,D3DBLEND_ONE,D3DBLEND_ONE);
			GraphicsStruct::RenderState      cull(D3DRS_CULLMODE,D3DCULL_NONE);

			pModel->Render(0,NULL);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	pModel->m_eRuntimeShaderType = eShaderSave;

	return S_OK;
}


HRESULT KG3DLightMapBaker::RenderEnvLightMapForModel(KG3DModel* pModel)
{
	KG3DDefaultShaderType eShaderSave = pModel->m_eRuntimeShaderType;
	pModel->m_eRuntimeShaderType = DEFST_LIGHT_MAP_BAKE_ENVLIGHT;
	//////////////////////////////////////////////////////////////////////////
	LPD3DXEFFECT pShader = g_cGraphicsTool.GetDefaultShader(DEFST_LIGHT_MAP_BAKE_ENVLIGHT);

	for (size_t i=0;i<m_vecEnvLight.size();i++)
	{
		EnvLight* pLight = m_vecEnvLight[i];

		if(pLight->m_BBox.IsTwoBoxIntersect(pModel->m_BBox))
		{
			D3DXVECTOR4 vLight;
			vLight.x = pLight->vPosition.x;
			vLight.y = pLight->vPosition.y;
			vLight.z = pLight->vPosition.z;
			vLight.w = pLight->m_Radius;

			pShader->SetVector("vPointLightPosition",&vLight);
			pShader->SetTexture("tColor",pLight->pCubeMapColor);
			pShader->SetTexture("tDepth",pLight->pCubeMapDepth);

			D3DXVECTOR4 noise[64];
			for (int s=0;s<64;s++)
			{
				noise[s].x = 0.01F * (rand()%200 -100);
				noise[s].y = 0.01F * (rand()%200 -100);
				noise[s].z = 0.01F * (rand()%200 -100);
				noise[s].w = 0;
			}
			pShader->SetVectorArray("vUSNoise",noise,64);

			pShader->CommitChanges();
		}
		//////////////////////////////////////////////////////////////////////////
		{
			GraphicsStruct::RenderStateAlpha alpha(0x80,FALSE,TRUE,D3DBLEND_ONE,D3DBLEND_ONE);
			GraphicsStruct::RenderState      cull(D3DRS_CULLMODE,D3DCULL_NONE);

			pModel->Render(0,NULL);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	pModel->m_eRuntimeShaderType = eShaderSave;

	return S_OK;
}

HRESULT KG3DLightMapBaker::RenderDirectionLightMapForModel(KG3DModel* pModel)
{
	KG3DDefaultShaderType eShaderSave = pModel->m_eRuntimeShaderType;
	pModel->m_eRuntimeShaderType = DEFST_LIGHT_MAP_BAKE_DIRECTIONLIGHT;
	//////////////////////////////////////////////////////////////////////////
	LPD3DXEFFECT pShader = g_cGraphicsTool.GetDefaultShader(DEFST_LIGHT_MAP_BAKE_DIRECTIONLIGHT);

	if(m_lpShadowMapForDiectionLight)
	{
		pShader->SetMatrix("matLight",&m_lpShadowMapForDiectionLight->m_matVP);
		pShader->SetTexture("tShadowmap1",m_lpShadowMapForDiectionLight->m_lpSMShadowMap);
		pShader->CommitChanges();
	}
	//////////////////////////////////////////////////////////////////////////
	{
		GraphicsStruct::RenderStateAlpha alpha(0x80,FALSE,TRUE,D3DBLEND_ONE,D3DBLEND_ONE);
		GraphicsStruct::RenderState      cull(D3DRS_CULLMODE,D3DCULL_NONE);

		pModel->Render(0,NULL);
	}
	//////////////////////////////////////////////////////////////////////////
	pModel->m_eRuntimeShaderType = eShaderSave;

	return S_OK;
}

HRESULT KG3DLightMapBaker::RenderPhotonsLightMapForModel(KG3DModel* pModel)
{

	KG3DDefaultShaderType eShaderSave = pModel->m_eRuntimeShaderType;
	pModel->m_eRuntimeShaderType = DEFST_LIGHT_MAP_BAKE_DIRECTIONLIGHT;
	//////////////////////////////////////////////////////////////////////////
	LPD3DXEFFECT pShader = g_cGraphicsTool.GetDefaultShader(DEFST_LIGHT_MAP_BAKE_DIRECTIONLIGHT);
	pShader->SetFloat("fPhotonScale",200.0F / (g_nPhotonCountPerLight + g_nPhotonCountPerLight * g_fReflectPhontPower));
	pShader->SetFloat("fUVOffset", 0.0F / pModel->m_lpLightMap->m_nLightmapWidth);

#define	nCountPerPass 55

	{
		int nRenderCount = 0;
		D3DXVECTOR4 vPhotonPosition[nCountPerPass];
		D3DXVECTOR4 vPhotonColor   [nCountPerPass];
		D3DXVECTOR4 vPhotonDirect  [nCountPerPass];

		while (nRenderCount < nCountPerPass && m_nBakedPhoton < m_nNumPhotonTotal)
		{
			float dis = max(10.0F,m_lpPhotons[m_nBakedPhoton].fDistance);

			vPhotonPosition[nRenderCount].x = m_lpPhotons[m_nBakedPhoton].vPosition.x;
			vPhotonPosition[nRenderCount].y = m_lpPhotons[m_nBakedPhoton].vPosition.y;
			vPhotonPosition[nRenderCount].z = m_lpPhotons[m_nBakedPhoton].vPosition.z;
			vPhotonPosition[nRenderCount].w = 10 / dis;

			vPhotonDirect[nRenderCount].x = - m_lpPhotons[m_nBakedPhoton].vDirection.x;
			vPhotonDirect[nRenderCount].y = - m_lpPhotons[m_nBakedPhoton].vDirection.y;
			vPhotonDirect[nRenderCount].z = - m_lpPhotons[m_nBakedPhoton].vDirection.z;
			vPhotonDirect[nRenderCount].w = 1;

			D3DCOLORVALUE color = Color2Value(m_lpPhotons[m_nBakedPhoton].vColor);
			vPhotonColor[nRenderCount].x = color.r;
			vPhotonColor[nRenderCount].y = color.g;
			vPhotonColor[nRenderCount].z = color.b;
			vPhotonColor[nRenderCount].w = 5000 * color.a / dis;// * 10000 / m_lpPhotons[nPhotonCount].fDistance;

			nRenderCount++;
			m_nBakedPhoton++;
		}

		pShader->SetVectorArray("szPhotonsPosition",vPhotonPosition,nRenderCount);
		pShader->SetVectorArray("szPhotonsColor",vPhotonColor,nRenderCount);
		pShader->SetVectorArray("szPhotonsDirect",vPhotonDirect,nRenderCount);
		pShader->SetInt("nNumPhoton",nRenderCount);

		GraphicsStruct::RenderStateAlpha alpha(0x80,FALSE,TRUE,D3DBLEND_ONE,D3DBLEND_ONE);
		GraphicsStruct::RenderState      cull(D3DRS_CULLMODE,D3DCULL_NONE);

		pModel->Render(0,NULL);
	}
	//////////////////////////////////////////////////////////////////////////
	pModel->m_eRuntimeShaderType = eShaderSave;

	return S_OK;
}

HRESULT KG3DLightMapBaker::BakeLightMapForModel(KG3DModel* pModel,BOOL bClear,BOOL bEnvLight)
{
	if(pModel->m_lpLightMap->m_nLightmapWidth != m_vRenderTargetTool.m_nWidth)
	{
		m_vRenderTargetTool.UnInit();
		m_vRenderTargetTool.Init(pModel->m_lpLightMap->m_nLightmapWidth,FALSE,D3DFMT_A8R8G8B8);
	}
	//////////////////////////////////////////////////////////////////////////
	m_vRenderTargetTool.BeginNewRenderTarget(NULL,NULL,NULL);

	g_pd3dDevice->BeginScene();
	
	if(bClear)
		g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0, 1.0f, 0);

	if(!bEnvLight)
	{
		RenderDirectionLightMapForModel(pModel);

		RenderPointLightMapForModel(pModel);
	}
	else
	{
		RenderEnvLightMapForModel(pModel);
	}


	g_pd3dDevice->EndScene();

	m_vRenderTargetTool.EndNewRenderTarget();

	if(!bEnvLight)
		m_vRenderTargetTool.ExportRenderData(pModel->m_lpLightMap->m_lpLightMapForCompute,TRUE);
	else
		m_vRenderTargetTool.ExportRenderData(pModel->m_lpLightMap->m_lpLightMapForCompute2,TRUE);

	//////////////////////////////////////////////////////////////////////////
	return S_OK;
}

HRESULT KG3DLightMapBaker::ShowDebug()
{
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0, 1.0f, 0);

	KG3DModel* pModel = m_vecModel[0];
	if(pModel->m_eRuntimeShaderType == DEFST_LIGHT_MAP)
	{
		RenderPhotonsLightMapForModel(pModel);
	}
	return S_OK;
}

HRESULT KG3DLightMapBaker::PrepareModelForBake(KG3DModel* pModel,BOOL bComputeDirectLight,BOOL bComputeIndirectLight)
{
	m_nBakedPhoton = 0;
	m_listModelForBake.clear();

	if(pModel)
	{
		if(pModel->m_eRuntimeShaderType == DEFST_LIGHT_MAP)
		{
			m_listModelForBake.push_back(pModel);
		}
	}
	else
	{
		for (size_t i=0;i<m_vecModel.size();i++)
		{
			KG3DModel* pModel = m_vecModel[i];
			if(pModel->m_eRuntimeShaderType == DEFST_LIGHT_MAP)
			{
				m_listModelForBake.push_back(pModel);
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	for (size_t i=0;i<m_vecModel.size();i++)
	{
		KG3DModel* pModel = m_vecModel[i];
		if(pModel->IsSecondUVExist())
		{
			pModel->m_eRuntimeShaderType = DEFST_COUNT;
		}
	}	

	ComputeCubeMapForPointLight();

	for (size_t i=0;i<m_vecModel.size();i++)
	{
		KG3DModel* pModel = m_vecModel[i];
		if(pModel->IsSecondUVExist())
		{
			pModel->m_eRuntimeShaderType = DEFST_LIGHT_MAP;
		}
	}

	m_listModelForBakeEnv = m_listModelForBake;

	m_bComputeDirectLight = bComputeDirectLight;
	m_bComputeIndirectLight = bComputeIndirectLight;

	if(m_bComputeIndirectLight)
	{
		m_vecEnvLightRefresh = m_vecEnvLight;
	}

	return S_OK;
}


HRESULT KG3DLightMapBaker::ComputeCubeMapForPointLight(KG3DModelPointLight* pLight)
{
	HRESULT hr = E_FAIL;
	KG3DCamera vCamera;
	KGLOG_PROCESS_ERROR(m_lpPointLightTool);

	hr = m_lpPointLightTool->ComputeCubeMapForPointLight(pLight,m_lpPvsScene,&m_vRenderTargetToolDepth);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DLightMapBaker::ComputeCubeMapForPointLight()
{
	HRESULT hr = E_FAIL;

	for (size_t i=0;i<m_vecPointLight.size();i++)
	{
		KG3DModelPointLight* pPointLight = m_vecPointLight[i];
		SAFE_RELEASE(pPointLight->m_lpCubeTexture);

		hr = D3DXCreateCubeTexture(g_pd3dDevice,512,1,0,D3DFMT_R32F,D3DPOOL_MANAGED,&pPointLight->m_lpCubeTexture);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = ComputeCubeMapForPointLight(pPointLight);
		KGLOG_COM_PROCESS_ERROR(hr);
	}

	hr = S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DLightMapBaker::EnvLight::Init(float R,D3DXVECTOR3 vPos,DWORD dwSize)
{
	m_Radius = R;
	D3DXVECTOR3 V(m_Radius,m_Radius,m_Radius);

	HRESULT hr = E_FAIL;
	vPosition = vPos;
	m_BBox.Clear();
	m_BBox.AddPosition(vPos - V);
	m_BBox.AddPosition(vPos + V);

	hr = D3DXCreateCubeTexture(g_pd3dDevice,128,1,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&pCubeMapColor);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = D3DXCreateCubeTexture(g_pd3dDevice,128,1,0,D3DFMT_R32F,D3DPOOL_MANAGED,&pCubeMapDepth);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DLightMapBaker::InitEvnLighting()
{
	UninitEvnLighting();

	int nYCount = 9;
	int nXCount = 9;
	int nZCount = 9;

	KG3DRepresentObjectPVS* pPVS = m_lpPvsScene->m_pPvs;

	float a = pPVS->m_BBox.GetLength() / (nXCount+1);
	float b = pPVS->m_BBox.GetHeight() / (nYCount+1);
	float c = pPVS->m_BBox.GetWidth()  / (nZCount+1);

	float Radius = max(max(a,b),c) * 4;

	for (int Y =0;Y<=nYCount;Y++)
	{
		for (int Z=0;Z<=nZCount;Z++)
		{
			for (int X=0;X<=nXCount;X++)
			{

				D3DXVECTOR3 vPos = pPVS->m_BBox.A;
				vPos.x += X * pPVS->m_BBox.GetLength() / nXCount;
				vPos.y += Y * pPVS->m_BBox.GetHeight() / nYCount;
				vPos.z += Z * pPVS->m_BBox.GetWidth()  / nZCount;

				if(pPVS->IsPositionInSet(vPos))
				{
					EnvLight* pEnv = new EnvLight;
					KGLOG_PROCESS_ERROR(pEnv);

					pEnv->Init(Radius,vPos,128);
					m_vecEnvLight.push_back(pEnv);
				}
			}
		}
	}

	m_bNeedEnableLightmap2 = TRUE;
	m_bNeedDisableLightmap2 = TRUE;
Exit0:
	return S_OK;
}

HRESULT KG3DLightMapBaker::UninitEvnLighting()
{
	for (size_t i=0;i<m_vecEnvLight.size();i++)
	{
		EnvLight* pEnv = m_vecEnvLight[i];
		SAFE_DELETE(pEnv);
	}
	m_vecEnvLight.clear();

	return S_OK;
}

HRESULT KG3DLightMapBaker::ProcessDirectLighting()
{
	if(m_listModelForBake.size() )
	{
		KG3DModel* pModel = *m_listModelForBake.begin();
		if(m_lpShadowMapForDiectionLight)
		{
			float fSzie = pModel->m_BBox.GetDiameter();
			float fLength = 30000;

			D3DXVECTOR3 vPosition = D3DXVECTOR3(pModel->m_matWorld._41,pModel->m_matWorld._42,pModel->m_matWorld._43);
			D3DXVECTOR3 vNormal = -g_cGraphicsTool.GetSunLightParam(*m_lpPvsScene).dir;
			D3DXVECTOR3 vC = vPosition + vNormal * fLength;
			m_lpShadowMapForDiectionLight->m_vCameraLight.SetPosition(vC);
			m_lpShadowMapForDiectionLight->m_vCameraLight.SetLookAtPosition(vPosition);
			m_lpShadowMapForDiectionLight->m_vCameraLight.SetUpDirection(D3DXVECTOR3(0,1,0));
			m_lpShadowMapForDiectionLight->m_vCameraLight.SetOrthogonal(fSzie,fSzie,100,fLength*1.5F);
			m_lpShadowMapForDiectionLight->m_vCameraLight.SetCamera();

			D3DXMATRIX matView_L,matProj_L;
			matView_L = m_lpShadowMapForDiectionLight->m_vCameraLight.GetViewMatrix();
			matProj_L = m_lpShadowMapForDiectionLight->m_vCameraLight.GetProjectionMatrix();

			m_lpShadowMapForDiectionLight->m_matVP = matView_L * matProj_L;

			m_lpShadowMapForDiectionLight->ProcessShadowMapWithPVS(m_lpPvsScene->m_pPvs);
		}
		//////////////////////////////////////////////////////////////////////////

		BakeLightMapForModel(pModel,TRUE,FALSE);

		m_listModelForBake.pop_front();
	}

	return S_OK;
}

HRESULT KG3DLightMapBaker::ProcessIndirectLighting()
{
	if(m_vecEnvLightRefresh.size())
	{
		EnvLight* pEnv = m_vecEnvLightRefresh[m_vecEnvLightRefresh.size() - 1];

		ComputeCubeMapForEnvLight(pEnv);

		m_vecEnvLightRefresh.pop_back();
	}
	else
	{
		if(m_bNeedEnableLightmap2)
		{
			for (size_t i=0;i<m_vecModel.size();i++)
			{
				KG3DModel* pModel = m_vecModel[i];
				if(pModel->IsSecondUVExist())
				{
					pModel->EnableUseLightmap2(TRUE,1,1);
				}
			}
			m_bNeedEnableLightmap2 = FALSE;
		}
		//////////////////////////////////////////////////////////////////////////
		if(m_listModelForBakeEnv.size())
		{
			KG3DModel* pModel = *m_listModelForBakeEnv.begin();

			BakeLightMapForModel(pModel,TRUE,TRUE);

			m_listModelForBakeEnv.pop_front();

			//////////////////////////////////////////////////////////////////////////	
			if(m_listModelForBakeEnv.size() == 0 && m_bNeedDisableLightmap2)
			{
				for (size_t i=0;i<m_vecModel.size();i++)
				{
					KG3DModel* pModel = m_vecModel[i];
					if(pModel->IsSecondUVExist())
					{
						pModel->EnableUseLightmap2(FALSE,0.7,0.8F);
					}
				}
				m_bNeedDisableLightmap2 = FALSE;
			}		
		}
	}

	return S_OK;
}