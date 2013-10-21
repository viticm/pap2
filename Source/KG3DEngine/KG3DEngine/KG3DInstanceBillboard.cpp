#include "StdAfx.h"
#include "MathTool.h"
#include "KG3DInstanceBillboard.h"
#include "KG3DEngineManager.h"
#include "KG3DGraphicsTool.h"
#include "KG3DGraphiceEngine.h"
#include "KG3DRenderState.h"
#include "KG3DTextureTable.h"
#include "kg3dgrasspatterncontainer.h"
#include "KG3DRenderGlobalState.h"
#include "KG3DSubSetRenderer.h"
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

static D3DVERTEXELEMENT9 g_BillBoardVD[] = 
{
	{ 0, 0,      D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0, 3 * 4,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
	{ 0, 6 * 4,  D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	{ 0, 8 * 4,  D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
	{ 0, 10 * 4, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },
	{ 0, 11 * 4, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3 },
	{ 0, 12 * 4, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,  0 },
	D3DDECL_END()
};

KG3DInstanceBillboard::KG3DInstanceBillboard(void)
{
	m_dwMaxBatch = 220;

	if (g_cGraphicsEngine.GetD3DCaps().VertexShaderVersion == D3DVS_VERSION(1, 1))
	{
		//96 at least
	}
	else
	{
		//256 at least
	}

	m_pInstanceInfo = NULL;
	m_pIndexBuffer = NULL;
	m_pVec4BillBoardPos = NULL;

	m_dwInstanceBufferSize = 0;

	m_bDirtyFlag = FALSE;

	m_dwLastTime = 0;
	m_pUser = NULL;
}

KG3DInstanceBillboard::~KG3DInstanceBillboard(void)
{
	UnInit();
}

HRESULT KG3DInstanceBillboard::UnInit()
{
	SAFE_RELEASE(m_pInstanceInfo);
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_DELETE_ARRAY(m_pVec4BillBoardPos);
	return S_OK;
}

HRESULT KG3DInstanceBillboard::RenderBegin(KG3DTexture* pTexture,KG3DTexture* pTextureCloud)
{
    HRESULT hr = E_FAIL;
	UINT uNumPasses = 0;
	
    //LPD3DXEFFECT pEffect = g_BillBoardEffect.m_pBillBoardShader;
	LPD3DXEFFECT pEffect = g_GetDefaultShaderGroup().GetDefaultEffect(DEFST_BILLBOARD); 
    DWORD dwNowTime = g_cGraphicsTool.GetNowTime();
	float fNowTimeInSecond = dwNowTime * 0.001F;
	D3DXMATRIX matView;
	D3DXMATRIX matWorld;
	D3DXMATRIX matProj;
	D3DXMATRIX matViewInv;
	D3DXMATRIX matWorldViewProj;
	D3DXVECTOR3 vec3Up(0, 1.0f, 0);
	D3DXVECTOR3 vec3Left(-1.0f, 0, 0);
    
    _ASSERTE(NULL != pEffect);
	
	g_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
	g_pd3dDevice->GetTransform(D3DTS_WORLD, &matWorld);
	g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	matWorldViewProj = matWorld * matView * matProj;

	{
		const KG3DLightParam& lightParam = g_GetRenderGlobalState().GetSunLightParam();

		pEffect->SetValue("vec3LightDir", &lightParam.dir, sizeof(D3DVECTOR));
		pEffect->SetValue("LightDiffuse", &lightParam.diffuse, sizeof(D3DCOLORVALUE));
		pEffect->SetValue("Ambient", &lightParam.sceneAmbient, sizeof(D3DCOLORVALUE));
	}

	


	pEffect->SetFloat("Time",fNowTimeInSecond);

	{
		D3DXVECTOR4  vCloudSpeed(dwNowTime*0.00001F,dwNowTime*0.000037F,0,0);
		pEffect->SetVector("vCloudSpeed",&vCloudSpeed);
		if(pTextureCloud)
		{
			pEffect->SetTexture("cloudtex",pTextureCloud->GetTexture());
			g_pd3dDevice->SetTexture(1, pTextureCloud->GetTexture());
		}

	}

	if (pTexture)
	{
		g_pd3dDevice->SetTexture(0, pTexture->GetTexture());
		pEffect->SetTexture("g_texture", pTexture->GetTexture());
	}

	D3DXMatrixInverse(&matViewInv, NULL, &matView);

	D3DXVec3TransformNormal(&vec3Up, &vec3Up, &matViewInv);
	D3DXVec3TransformNormal(&vec3Left, &vec3Left, &matViewInv);

	pEffect->SetMatrix("WorldViewProj", &matWorldViewProj);
	pEffect->SetValue("vec3Up", &vec3Up, sizeof(D3DXVECTOR3));
	pEffect->SetValue("vec3Left", &vec3Left, sizeof(D3DXVECTOR3));

	hr = pEffect->Begin(&uNumPasses, 0);
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = pEffect->BeginPass(0);
	if (FAILED(hr))
	{
		pEffect->End();
	}
    KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
	return hr;
}


HRESULT KG3DInstanceBillboard::RenderEnd()         
{
    HRESULT hr = E_FAIL;
    
    //pEffect = g_BillBoardEffect.m_pBillBoardShader;
	LPD3DXEFFECT pEffect = g_GetDefaultShaderGroup().GetDefaultEffect(DEFST_BILLBOARD); 
    _ASSERTE(NULL != pEffect);

	hr = pEffect->EndPass();
    KGLOG_COM_CHECK_ERROR(hr);

	hr = pEffect->End();
    KGLOG_COM_CHECK_ERROR(hr);

    hr = pEffect->SetTexture("cloudtex", NULL);
    KGLOG_COM_CHECK_ERROR(hr);

    hr = pEffect->SetTexture("g_texture", NULL);
    KGLOG_COM_CHECK_ERROR(hr);

    hr = g_pd3dDevice->SetTexture(0, NULL);
    KGLOG_COM_CHECK_ERROR(hr);

    hr = g_pd3dDevice->SetTexture(1, NULL);
    KGLOG_COM_CHECK_ERROR(hr);

	return hr;
}

HRESULT KG3DInstanceBillboard::RenderWithoutEffectChange()
{
	int nBillBoardRendered = 0;
	int nTotal = static_cast<int>(m_BillBoardData.size());
	LPD3DXEFFECT pEffect = g_GetDefaultShaderGroup().GetDefaultEffect(DEFST_BILLBOARD); 
	_ASSERTE(NULL != pEffect);
	KG_PROCESS_ERROR(m_pInstanceInfo);
	KG_PROCESS_ERROR(m_pIndexBuffer);
	KG_PROCESS_ERROR(m_pVec4BillBoardPos);
	KGLOG_PROCESS_ERROR(g_BillBoardEffect.m_pBillBoardInstance);

	g_pd3dDevice->SetStreamSource(0, m_pInstanceInfo, 0, sizeof(InstanceData));
	g_pd3dDevice->SetIndices(m_pIndexBuffer);
	g_pd3dDevice->SetVertexDeclaration(g_BillBoardEffect.m_pBillBoardInstance);

	while (nBillBoardRendered < nTotal)
	{
		int nNumToRender = min(nTotal - nBillBoardRendered, static_cast<int>(m_dwMaxBatch));

		pEffect->SetVectorArray("vec4BillboardPos", (m_pVec4BillBoardPos + nBillBoardRendered), nNumToRender);

		pEffect->CommitChanges();

		g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 
			0,
			0,
			nNumToRender * 4, 
			nBillBoardRendered * 6,
			nNumToRender * 2);

		nBillBoardRendered += m_dwMaxBatch;
	}
Exit0:
	return S_OK;
}

HRESULT KG3DInstanceBillboard::Render(unsigned uOption, void* pExtInfo,KG3DTexture* pTexture)
{
	HRESULT hr = E_FAIL;
    BOOL bRenderBeging = FALSE;

	LPD3DXEFFECT pEffect = g_GetDefaultShaderGroup().GetDefaultEffect(DEFST_BILLBOARD); 
	_ASSERTE(NULL != pEffect);

	KG3DRenderState& RenderState = g_GetRenderStateMgr();

	KG_PROCESS_ERROR(m_pVec4BillBoardPos);
	KG_PROCESS_ERROR(m_pInstanceInfo);
    KG_PROCESS_ERROR(m_BillBoardData.size());
    KG_PROCESS_ERROR(m_pIndexBuffer);
    KGLOG_PROCESS_ERROR(g_BillBoardEffect.m_pBillBoardInstance);

	RenderState.SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	RenderState.SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	RenderState.SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);

	RenderState.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	RenderState.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	RenderState.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE2X);

	hr = RenderState.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	KG_COM_PROCESS_ERROR(hr);

	hr = RenderBegin(pTexture,NULL);
    KG_COM_PROCESS_ERROR(hr);

    bRenderBeging = TRUE;

	hr = RenderWithoutEffectChange();
    KG_COM_PROCESS_ERROR(hr);

    hr = S_OK;
Exit0:

    if (bRenderBeging)
    {
	    hr = RenderEnd();
        KGLOG_COM_CHECK_ERROR(hr);
    }
	
	RenderState.Restore();
	if (pEffect)
	{
		pEffect->EndPass();
		pEffect->End();
	}

	hr = g_pd3dDevice->SetTexture(0, NULL);
    KGLOG_COM_CHECK_ERROR(hr);

    return hr;
}

HRESULT KG3DInstanceBillboard::FrameMove(DWORD m_dwStrideX,DWORD m_dwStrideY)
{
	HRESULT hr = E_FAIL;
	if (m_bDirtyFlag)
	{
		hr = UpdateVertexData(m_dwStrideX,m_dwStrideY);
		m_bDirtyFlag = FALSE;
	}
	hr = S_OK;
	return hr;
}

void KG3DInstanceBillboard::AddBillBoard(const BillboardInfo* pInfo, unsigned int uNum)
{
	KG_PROCESS_ERROR(pInfo);
	
	KG_PROCESS_ERROR(uNum != 0);
	
	m_bDirtyFlag = TRUE;
	for (unsigned int i = 0; i < uNum; i++)
	{
		m_BillBoardData.push_back(pInfo[i]);
	}
Exit0:
	return;
}

HRESULT KG3DInstanceBillboard::UpdateVertexData(DWORD m_dwStrideX,DWORD m_dwStrideY)
{
	HRESULT hr = E_FAIL;
	InstanceData* pData = NULL;
	short* pIndices = NULL;

	if (m_dwInstanceBufferSize < m_BillBoardData.size() * 4)
	{
		SAFE_RELEASE(m_pInstanceInfo);
		SAFE_DELETE_ARRAY(m_pVec4BillBoardPos);
		SAFE_RELEASE(m_pIndexBuffer);
	}

	while (m_dwInstanceBufferSize < m_BillBoardData.size() * 4)
	{
		m_dwInstanceBufferSize = max(1, m_dwInstanceBufferSize);
		m_dwInstanceBufferSize <<= 1;
	}

	if (!m_pInstanceInfo)
	{
		hr = g_pd3dDevice->CreateVertexBuffer(sizeof(InstanceData) * m_dwInstanceBufferSize, 
			0, 
			0, 
			D3DPOOL_MANAGED, 
			&m_pInstanceInfo, 
			NULL);
		KG_COM_PROCESS_ERROR(hr);
	}

	if (!m_pVec4BillBoardPos)
	{
		m_pVec4BillBoardPos = new D3DXVECTOR4[m_dwInstanceBufferSize / 4];
		KGLOG_PROCESS_ERROR(m_pVec4BillBoardPos);
	}

	if (!m_pIndexBuffer)
	{
		hr = g_pd3dDevice->CreateIndexBuffer(sizeof(short) * m_dwInstanceBufferSize /4 * 6, 
			0,
			D3DFMT_INDEX16, 
			D3DPOOL_MANAGED,
			&m_pIndexBuffer,
			NULL);
		KG_COM_PROCESS_ERROR(hr);
	}


	hr = m_pInstanceInfo->Lock(0, 0, (void**)&pData, 0);
	KG_COM_PROCESS_ERROR(hr);
	
	for (size_t i = 0; i < m_BillBoardData.size(); i++)
	{
		for (int j = 0; j < 4; j++)
		{
			pData[i * 4 + j].vec3Pos = m_BillBoardData[i].m_vec3Pos;
			pData[i * 4 + j].fVertexIndex = static_cast<float>(j);
			pData[i * 4 + j].fIntanceIndex = static_cast<float>(i % m_dwMaxBatch);
			pData[i * 4 + j].vec2Size = m_BillBoardData[i].m_vec2Size;
			pData[i * 4 + j].dwColor = m_BillBoardData[i].m_dwColor;
			pData[i * 4 + j].vec3Normal = m_BillBoardData[i].m_vec3Normal;
			GetUVFromIndex(m_BillBoardData[i].m_TextureIndex, j, pData[i * 4 + j].vec2UV,m_dwStrideX,m_dwStrideY);
		}
		m_pVec4BillBoardPos[i].x = m_BillBoardData[i].m_vec3Pos.x;
		m_pVec4BillBoardPos[i].y = m_BillBoardData[i].m_vec3Pos.y;
		m_pVec4BillBoardPos[i].z = m_BillBoardData[i].m_vec3Pos.z;
	}

	hr = m_pIndexBuffer->Lock(0, 0, (void**)&pIndices, 0);
	KG_COM_PROCESS_ERROR(hr);
	
	for (short i = 0; i < static_cast<short>(m_BillBoardData.size()); i++)
	{
		pIndices[i * 6] = i * 4 + 2;
		pIndices[i * 6 + 1] = i * 4 + 3;
		pIndices[i * 6 + 2] = i * 4;

		pIndices[i * 6 + 3] = i * 4;
		pIndices[i * 6 + 4] = i * 4 + 1;
		pIndices[i * 6 + 5] = i * 4 + 2;
	}
	hr = S_OK;
Exit0:
	if (m_pInstanceInfo)
	{
		m_pInstanceInfo->Unlock();
	}

	if (m_pIndexBuffer)
	{
		m_pIndexBuffer->Unlock();
	}
	return hr;
}

HRESULT KG3DInstanceBillboard::Init()
{
	return S_OK;
}

void KG3DInstanceBillboard::GetUVFromIndex(DWORD dwTexIndex,
										   DWORD dwVertexIndex,
										   D3DXVECTOR2& vec2UV,DWORD m_dwStrideX,DWORD m_dwStrideY)
{
	float fUSpan = 1.0f / m_dwStrideX;
	float fVSpan = 1.0f / m_dwStrideY;

	int nIndexU = dwTexIndex % m_dwStrideX;
	int nIndexV = dwTexIndex / m_dwStrideX;

	switch(dwVertexIndex)
	{
	case 0:
		{
			vec2UV.x = fUSpan * nIndexU;
			vec2UV.y = fVSpan * nIndexV;
		}
		break;
	case 1:
		{
			vec2UV.x = fUSpan * (nIndexU + 1);
			vec2UV.y = fVSpan * nIndexV;
		}
		break;
	case 2:
		{
			vec2UV.x = fUSpan * (nIndexU + 1);
			vec2UV.y = fVSpan * (nIndexV + 1);
		}
		break;
	case 3:
		{
			vec2UV.x = fUSpan * nIndexU;
			vec2UV.y = fVSpan * (nIndexV + 1);
		}	
		break;
	}
}

HRESULT KG3DInstanceBillboard::SetTexture(LPCSTR strTexture, 
										  DWORD dwStrideX, 
										  DWORD dwStrideY)
{
	HRESULT hr = E_FAIL;
	hr = S_OK;
//Exit0:
	return hr;
}

void KG3DInstanceBillboard::ClearBillboard()
{
	m_BillBoardData.clear();
	m_bDirtyFlag = TRUE;
}

HRESULT KG3DInstanceBillboard::SetTexture(KG3DTexture* pTexture,
										  DWORD dwStrideX,
										  DWORD dwStrideY)
{
	HRESULT hr = E_FAIL;
	hr = S_OK;
//Exit0:
	return hr;
}
void KG3DInstanceBillboard::ReleaseVideoBuffer()
{
    SAFE_RELEASE(m_pIndexBuffer);
    SAFE_RELEASE(m_pInstanceInfo);
}


KG3DBillboardEffect g_BillBoardEffect;

KG3DBillboardEffect::KG3DBillboardEffect()
: m_pBillBoardInstance(NULL)
//, m_pBillBoardShader(NULL)
{
}

KG3DBillboardEffect::~KG3DBillboardEffect()
{
    ASSERT(m_pBillBoardInstance == NULL);
    //ASSERT(m_pBillBoardShader == NULL);
}

HRESULT KG3DBillboardEffect::Init()
{
	HRESULT hr = E_FAIL;

	//ASSERT(m_pBillBoardShader == NULL);

    /*hr = KG3DD3DXCreateEffectFromFile(g_pd3dDevice, _T("Data\\public\\shader\\shaderbillboard.fx"),
        NULL, NULL, 0, NULL, &m_pBillBoardShader, NULL);
    KGLOG_COM_PROCESS_ERROR(hr);*/
	
    ASSERT(m_pBillBoardInstance == NULL);

    hr = g_pd3dDevice->CreateVertexDeclaration(g_BillBoardVD, &m_pBillBoardInstance);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
	return hr;
}

HRESULT KG3DBillboardEffect::UnInit()
{
	VERIFY_RELEASE(m_pBillBoardInstance);
	//VERIFY_RELEASE(m_pBillBoardShader);

    return S_OK;
}

HRESULT KG3DBillboardEffect::OnLostDevice()
{
    //HRESULT hr = E_FAIL;
	HRESULT hResult = E_FAIL;

	//KG_PROCESS_SUCCESS(!m_pBillBoardShader);

    //hr = m_pBillBoardShader->OnLostDevice();
    //KGLOG_COM_CHECK_ERROR(hr);

	hResult = S_OK;
//Exit0:
    return hResult;
}

HRESULT KG3DBillboardEffect::OnResetDevice()
{
    //HRESULT hr = E_FAIL;
	HRESULT hResult = E_FAIL;
	//KG_PROCESS_SUCCESS(!m_pBillBoardShader);

    //hr = m_pBillBoardShader->OnResetDevice();
    //KGLOG_COM_CHECK_ERROR(hr);
	hResult = S_OK;
//Exit0:
    return hResult;
}
