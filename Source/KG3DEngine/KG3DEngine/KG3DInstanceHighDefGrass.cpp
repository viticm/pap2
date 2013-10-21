#include "StdAfx.h"
#include "KG3DInstanceHighDefGrass.h"
#include "KG3DRenderState.h"
#include "KG3DTextureTable.h"
#include "kg3dmodeltable.h"
#include "kg3dgrasspatterncontainer.h"
#include "KG3DGraphicsTool.h"
#include "KG3DRenderGlobalState.h"
#include "KG3DEngineManager.h"
#include "KG3DSubSetRenderer.h"
extern KG3DEngineOption g_cEngineOption;

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif
////////////////////////////////////////////////////////////////////////////////

extern TCHAR g_Def_WorkDirectory[];


float g_fGrassLODLevel01 = 1000.0f;
float g_fGrassLODLevel12 = 2000.0f;
float g_fGrassLODLevel23 = 8000.0f;
float g_fGrassDownScale = 300.0f;


LPDIRECT3DVERTEXDECLARATION9 KG3DInstanceHighDefGrass::m_lpVertexDeclaration = NULL;


KG3DInstanceHighDefGrass::KG3DInstanceHighDefGrass(void)
{
	m_pIB = NULL;
	m_pVB = NULL;
	m_bDirtyData = FALSE;
	m_dwBufferSize = 0;

	m_dwNumGrassToRender = 0;

	m_bAutoRotated = TRUE;

	m_dwLastTime = 0;
	m_pUser = NULL;

}

KG3DInstanceHighDefGrass::~KG3DInstanceHighDefGrass(void)
{
	SAFE_RELEASE(m_pIB);
	SAFE_RELEASE(m_pVB);
}

HRESULT KG3DInstanceHighDefGrass::Init()
{
	HRESULT hr = E_FAIL;
	//m_SixStar[0]
	hr = S_OK;
//Exit0:
	return hr;
}


HRESULT KG3DInstanceHighDefGrass::RenderBegin(KG3DTexture* pTexture,KG3DTexture* pTextureCloud)
{
	HRESULT hr = E_FAIL;
	
	D3DLIGHT9 light;
	D3DXMATRIX matWorld, matView, matProj, matViewProj;

	DWORD dwNowTime = g_cGraphicsTool.GetNowTime();
	
	float fNowTimeInSecond;

	LPD3DXEFFECT lpEffect = g_GetDefaultShaderGroup().GetDefaultEffect(DEFST_HIGH_GRASS);
	KG_PROCESS_ERROR(NULL != lpEffect);

	fNowTimeInSecond = dwNowTime * 0.001F;

	g_pd3dDevice->GetLight(0, &light);
	g_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
	g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	matViewProj = matView * matProj;


	lpEffect->SetFloat("Time",fNowTimeInSecond);
	

	{
		const KG3DLightParam& lightParam = g_GetRenderGlobalState().GetSunLightParam();
		lpEffect->SetFloatArray("LightDirection", (float*)&lightParam.dir, 3);
		lpEffect->SetVector("LightDiffuse", (D3DXVECTOR4*)&lightParam.diffuse);
		lpEffect->SetVector("LightAmbient", (D3DXVECTOR4*)&lightParam.sceneAmbient);
	}

	{
		D3DXVECTOR4 vCloudSpeed(dwNowTime*0.00001F,dwNowTime*0.000037F,0,0);
		lpEffect->SetVector("vCloudSpeed",&vCloudSpeed);
		if(pTextureCloud)
			lpEffect->SetTexture("cloudtex",pTextureCloud->GetTexture());

	}

    hr = lpEffect->SetTechnique("Tech");
    KGLOG_COM_CHECK_ERROR(hr);

    lpEffect->SetMatrix("mViewProj", &matViewProj);
	if(pTexture)
		g_pd3dDevice->SetTexture(0, pTexture->GetTexture());
	
    if (pTextureCloud)
    {
        g_pd3dDevice->SetTexture(1, pTextureCloud->GetTexture());
    }

	UINT uiNumPasses = 0;
	lpEffect->Begin(&uiNumPasses, 0);
	lpEffect->BeginPass(0);

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DInstanceHighDefGrass::RenderEnd()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

	LPD3DXEFFECT lpEffect = g_GetDefaultShaderGroup().GetDefaultEffect(DEFST_HIGH_GRASS);
	if (lpEffect)
    {
        lpEffect->EndPass();
        lpEffect->End();

        lpEffect->SetTexture("cloudtex", NULL);
    }

    hr = g_pd3dDevice->SetTexture(0, NULL);
    KGLOG_COM_PROCESS_ERROR(hr);

	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KG3DInstanceHighDefGrass::Render(unsigned uOption, void* pExtInfo,KG3DTexture* pTexture)
{	
	HRESULT hr = E_FAIL;
	KG3DRenderState& RenderState = g_GetRenderStateMgr();

	KG_PROCESS_ERROR(m_RenderInfo.pIB);
	KG_PROCESS_ERROR(m_RenderInfo.pVB);
    KG_PROCESS_SUCCESS(!m_RenderInfo.dwUsedIBSize);

	D3DMATERIAL9 material;
	material.Diffuse.a = 1.0f;
	material.Diffuse.r = 1.0f;
	material.Diffuse.b = 1.0f;
	material.Diffuse.g = 1.0f;

	material.Ambient = material.Diffuse;
	
	material.Specular.a = 0.0f;
	material.Specular.r = 0.0f;
	material.Specular.g = 0.0f;
	material.Specular.b = 0.0f;

	material.Emissive = material.Specular;

	D3DLIGHT9 light;

	g_pd3dDevice->GetLight(0, &light);
	g_pd3dDevice->SetMaterial(&material);
	RenderState.SetRenderState(D3DRS_LIGHTING, TRUE);
	RenderState.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	RenderState.SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	//Note: AlphaRef set in KG3DGrassRender
	RenderState.SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	RenderState.SetRenderState(D3DRS_SPECULARENABLE, FALSE);
	RenderState.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
	RenderState.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_PASSTHRU);
	RenderState.SetRenderState(D3DRS_COLORVERTEX, TRUE);
	RenderState.SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);

	hr = g_pd3dDevice->SetTexture(0, pTexture->GetTexture());
	KG_COM_PROCESS_ERROR(hr);

	hr = g_pd3dDevice->SetTexture(1, NULL);
	KG_COM_PROCESS_ERROR(hr);

	if(!g_cEngineOption.bGrassAnimation)
	{

		hr = g_pd3dDevice->SetIndices(m_RenderInfo.pIB);
		KG_COM_PROCESS_ERROR(hr);

		hr = g_pd3dDevice->SetStreamSource(0, m_RenderInfo.pVB, 0, sizeof(HighDefGrassVertex));
		KG_COM_PROCESS_ERROR(hr);

		hr = g_pd3dDevice->SetFVF(HighDefGrassVertex::dwFVF);
		KG_COM_PROCESS_ERROR(hr);

		hr = g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 
			0, 
			0, 
			m_RenderInfo.dwUsedVBSize, 
			0, 
			m_RenderInfo.dwUsedIBSize / 3);
		KG_COM_PROCESS_ERROR(hr);
	}
	else
	{
		RenderBegin(pTexture,NULL);

		RenderWithoutStateChange();

		RenderEnd();
	}

	hr = S_OK;
Exit0:
	RenderState.Restore();

Exit1:
    hr = g_pd3dDevice->SetTexture(0, NULL);
    KGLOG_COM_CHECK_ERROR(hr);

    return hr;
}

void KG3DInstanceHighDefGrass::GetUVFromIndex(DWORD dwTexIndex,
											  D3DXVECTOR2& vec2UV,DWORD m_dwStrideX,DWORD m_dwStrideY)
{
	float fUSpan = 1.0f / m_dwStrideX;
	float fVSpan = 1.0f / m_dwStrideY;
	
	int nIndexU = dwTexIndex % m_dwStrideX;
	int nIndexV = dwTexIndex / m_dwStrideX;

	vec2UV.x = vec2UV.x * fUSpan + nIndexU * fUSpan;
	vec2UV.y = vec2UV.y * fVSpan + nIndexV * fVSpan;
}

//void KG3DInstanceHighDefGrass::GetUVFromIndex(DWORD dwTexIndex,
//											  DWORD dwVertexIndex,
//											  D3DXVECTOR2& vec2UV,DWORD m_dwStrideX,DWORD m_dwStrideY)
//{
//	float fUSpan = 1.0f / m_dwStrideX;
//	float fVSpan = 1.0f / m_dwStrideY;
//
//	int nIndexU = dwTexIndex % m_dwStrideX;
//	int nIndexV = dwTexIndex / m_dwStrideX;
//
//	switch(dwVertexIndex)
//	{
//	case 0:
//		{
//			vec2UV.x = fUSpan * nIndexU;
//			vec2UV.y = fVSpan * nIndexV;
//		}
//		break;
//	case 1:
//		{
//			vec2UV.x = fUSpan * (nIndexU + 1);
//			vec2UV.y = fVSpan * nIndexV;
//		}
//		break;
//	case 2:
//		{
//			vec2UV.x = fUSpan * (nIndexU + 1);
//			vec2UV.y = fVSpan * (nIndexV + 1);
//		}
//		break;
//	case 3:
//		{
//			vec2UV.x = fUSpan * nIndexU;
//			vec2UV.y = fVSpan * (nIndexV + 1);
//		}	
//		break;
//	}
//}

HRESULT KG3DInstanceHighDefGrass::SetTexture(const TCHAR* strFileName, DWORD dwStrideX, DWORD dwStrideY)
{
	HRESULT hr = E_FAIL;
	hr = S_OK;
//Exit0:
	return hr;
}


void KG3DInstanceHighDefGrass::SetPatchRenderID(DWORD dwID)
{
	m_dwPathchRenderID = dwID;
}

DWORD KG3DInstanceHighDefGrass::GetPatchRenderID()
{
	return m_dwPathchRenderID;
}

HRESULT KG3DInstanceHighDefGrass::RenderWithoutStateChange()
{
	HRESULT hr = E_FAIL;

	hr = g_pd3dDevice->SetIndices(m_RenderInfo.pIB);

	hr = g_pd3dDevice->SetStreamSource(0, m_RenderInfo.pVB, 0, sizeof(HighDefGrassVertex));

	hr = g_pd3dDevice->SetFVF(HighDefGrassVertex::dwFVF);

	LPD3DXEFFECT lpEffect = g_GetDefaultShaderGroup().GetDefaultEffect(DEFST_HIGH_GRASS);
	KG_PROCESS_ERROR(NULL != lpEffect);
	lpEffect->CommitChanges();

	hr = g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 
		0, 
		0, 
		m_RenderInfo.dwUsedVBSize, 
		0, 
		m_RenderInfo.dwUsedIBSize / 3);

	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DInstanceHighDefGrass::RenderStone(KG3DTexture* pTexture)
{
	HRESULT hr = E_FAIL;
	KG3DRenderState& RenderState = g_GetRenderStateMgr();

	KG_PROCESS_ERROR(m_RenderInfo.pIB);
	KG_PROCESS_ERROR(m_RenderInfo.pVB);
	KG_PROCESS_SUCCESS(!m_RenderInfo.dwUsedIBSize);

	D3DMATERIAL9 material;
	material.Diffuse.a = 1.0f;
	material.Diffuse.r = 1.0f;
	material.Diffuse.b = 1.0f;
	material.Diffuse.g = 1.0f;

	material.Ambient = material.Diffuse;

	material.Specular.a = 0.0f;
	material.Specular.r = 0.0f;
	material.Specular.g = 0.0f;
	material.Specular.b = 0.0f;

	material.Emissive = material.Specular;

	D3DLIGHT9 light;
	g_pd3dDevice->GetLight(0, &light);
	g_pd3dDevice->SetMaterial(&material);
	RenderState.SetRenderState(D3DRS_LIGHTING, TRUE);
	RenderState.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	RenderState.SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	//Note: AlphaRef set in KG3DGrassRender
	RenderState.SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	RenderState.SetRenderState(D3DRS_SPECULARENABLE, FALSE);
	RenderState.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
	RenderState.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	RenderState.SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_PASSTHRU);
	RenderState.SetRenderState(D3DRS_COLORVERTEX, TRUE);
	RenderState.SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);

	hr = g_pd3dDevice->SetTexture(1, NULL);
	KG_COM_PROCESS_ERROR(hr);

	hr = g_pd3dDevice->SetTexture(0, pTexture->GetTexture());
	KG_COM_PROCESS_ERROR(hr);


		hr = g_pd3dDevice->SetIndices(m_RenderInfo.pIB);
		KG_COM_PROCESS_ERROR(hr);

		hr = g_pd3dDevice->SetStreamSource(0, m_RenderInfo.pVB, 0, sizeof(HighDefGrassVertex));
		KG_COM_PROCESS_ERROR(hr);

		hr = g_pd3dDevice->SetFVF(HighDefGrassVertex::dwFVF);
		KG_COM_PROCESS_ERROR(hr);

		hr = g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 
			0, 
			0, 
			m_RenderInfo.dwUsedVBSize, 
			0, 
			m_RenderInfo.dwUsedIBSize / 3);
		KG_COM_PROCESS_ERROR(hr);


	hr = S_OK;
Exit0:
	RenderState.Restore();
Exit1:
	return hr;
}

KG3DInstanceHighDefGrass::GrassCell::GrassCell()
{
	bDirtyFlag = FALSE;
	pIndices = NULL;
}

KG3DInstanceHighDefGrass::GrassCell::~GrassCell()
{
	SAFE_DELETE_ARRAY(pIndices);
}


//HRESULT KG3DInstanceHighDefGrass::LoadPattern(const TCHAR* strModelName, 
//											  size_t uIndex)
//{
//	HRESULT hr = E_FAIL;
//	size_t uNewSize = max(uIndex + 1, m_Patterns.size());
//
//	m_Patterns.resize(uNewSize);
//
//	hr = g_GrassPatternContainer.LoadPattern(strModelName, m_Patterns[uIndex]);
//	KG_COM_PROCESS_ERROR(hr);
//
//	hr = S_OK;
//Exit0:
//	return hr;
//}



KG3DInstanceHighDefGrass::RenderInfo::RenderInfo()
{
	pIB = NULL;
	pVB = NULL;
	dwVBSize = 0;
	dwIBSize = 0;
	dwUsedIBSize = 0;
	dwUsedVBSize = 0;
}

KG3DInstanceHighDefGrass::RenderInfo::~RenderInfo()
{
	SAFE_RELEASE(pIB);
	SAFE_RELEASE(pVB);
	dwVBSize = 0;
	dwIBSize = 0;
	dwUsedIBSize = 0;
	dwUsedVBSize = 0;
}

HRESULT KG3DInstanceHighDefGrass::ClearGrass()
{
	HRESULT hr = E_FAIL;

	m_RenderInfo.dwUsedIBSize = 0;
	m_RenderInfo.dwUsedVBSize = 0;

	ReleaseVideoBuffer();
	//ClearPattern();

	m_bDirtyData = TRUE;
	hr = S_OK;
//Exit0:
	return hr;
}


//DWORD KG3DInstanceHighDefGrass::GetPatternIndexSize(DWORD dwIndex)
//{
//	DWORD dwReturn = 0;
//	PatternModel* pPattern = NULL;
//
//	KG_PROCESS_ERROR(dwIndex < m_Patterns.size());
//	pPattern = g_GrassPatternContainer.GetPattern(m_Patterns[dwIndex]);
//	KG_PROCESS_ERROR(pPattern);
//
//	dwReturn = pPattern->dwIndicesCnt;
//
//Exit0:
//	return dwReturn;
//}

BOOL KG3DInstanceHighDefGrass::IsAutoRandomRotated()
{
	return m_bAutoRotated;
}

void KG3DInstanceHighDefGrass::SetAutoRandomRotated(BOOL bRotated)
{
	m_bAutoRotated = bRotated;
}


//void KG3DInstanceHighDefGrass::ClearPattern()
//{
//	m_Patterns.clear();
//}


HRESULT KG3DInstanceHighDefGrass::SetTexture(KG3DTexture* pTexture,
											 DWORD dwStrideX,
											 DWORD dwStrideY)
{
	HRESULT hr = E_FAIL;
	hr = S_OK;
//Exit0:
	return hr;
}

HRESULT KG3DInstanceHighDefGrass::GetPatternBBox(BBox& Box, unsigned int uIndex,vector<int>&m_Patterns)
{
	HRESULT hr = E_FAIL;
	PatternModel* pPattern = NULL;

	KG_PROCESS_ERROR(uIndex < m_Patterns.size());
	pPattern = g_GrassPatternContainer.GetPattern(m_Patterns[uIndex]);
	KG_PROCESS_ERROR(pPattern);

	Box = pPattern->PatternBBox;
	hr = S_OK;
Exit0:
	return hr;
}

int KG3DInstanceHighDefGrass::GetPatternIndex(int nPatternIndexInContainer,vector<int>&m_Patterns)
{
	int nReturn = -1;
	for (size_t i = 0; i < m_Patterns.size(); i++)
	{
		if (m_Patterns[i] == nPatternIndexInContainer)
		{
			nReturn = static_cast<int>(i);
			break;
		}
	}
	return nReturn;
}

void KG3DInstanceHighDefGrass::ReleaseVideoBuffer()
{
    KG_PROCESS_SUCCESS(!m_RenderInfo.pIB);
    SAFE_RELEASE(m_RenderInfo.pIB);
    SAFE_RELEASE(m_RenderInfo.pVB);
    m_RenderInfo.dwVBSize = 0;
    m_RenderInfo.dwIBSize = 0;
    m_RenderInfo.dwUsedIBSize = 0;
    m_RenderInfo.dwUsedVBSize = 0;
    m_bDirtyData = TRUE;
Exit1:
    ;
}


HRESULT KG3DInstanceHighDefGrass::InitStaticResource()
{
	HRESULT hr = E_FAIL;

	SAFE_RELEASE(m_lpVertexDeclaration);

	/*hr = KG3DD3DXCreateEffectFromFile(g_pd3dDevice, _T("Data\\public\\shader\\Grass.fx"), NULL, NULL, 0, NULL, &m_lpEffect, NULL);
	KGLOG_COM_PROCESS_ERROR(hr);*/	//改成在DefaultShaderGroup中创建

	D3DVERTEXELEMENT9 VertexElement[] = {
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION,  0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_NORMAL,    0},
		{0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_COLOR,    0},
		{0, 28, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD,  0},
		
		//{0, 32, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD,  1},
		D3DDECL_END()
	};

	hr = g_pd3dDevice->CreateVertexDeclaration(VertexElement, &m_lpVertexDeclaration);
	KG_COM_PROCESS_ERROR(hr);

	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DInstanceHighDefGrass::UnInitStaticResource()
{
	SAFE_RELEASE(m_lpVertexDeclaration);

	return S_OK;
}
