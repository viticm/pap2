#include "StdAfx.h"
#include "KG3DRuntimeMaterial.h"
#include "kg3dmaterial.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL KG3DRuntimeMaterial::IsgUseGadualChange(D3DCOLORVALUE& A,D3DCOLORVALUE& B,float maxSpace)
{
	float r,g,b;
	r = A.r - B.r;
	if (r<0)
		r = -r;
	g = A.g - B.g;
	if (g<0)
		g = -g;
	b = A.b - B.b;
	if (b<0)
		b = -b;
	if ( r>maxSpace && g >maxSpace && b > maxSpace)
	{
		return TRUE;
	}
	return FALSE;
}

KG3DRuntimeMaterial::KG3DRuntimeMaterial(void)
{
	m_bEnable = FALSE;
	m_dwMaterialFlag = 0;
	m_bEnableLightAmbient = FALSE;
	m_LastDiffuse.r = m_LastDiffuse.g = m_LastDiffuse.b = 0.0f;
	m_CurDiffuse.r = m_CurDiffuse.g = m_CurDiffuse.b = 0.0f;
	m_Diffuse.r = m_Diffuse.g = m_Diffuse.b = 1.0f;
	m_dwLastTime = timeGetTime(); 
	m_fAlpha = 1.0f;
	m_fIlluminationLevel = 0.0f;

	m_State.reserve(32);
}

KG3DRuntimeMaterial::~KG3DRuntimeMaterial(void)
{
	m_bEnable = FALSE;
}

HRESULT KG3DRuntimeMaterial::BeginRender()
{
	HRESULT hr = E_FAIL;
    HRESULT hRetCode = E_FAIL;
	D3DMATERIAL9 matSave;
	KG_PROCESS_ERROR(m_bEnable);

    if (! m_State.empty())
    {
		StateBlock *pEnd = (&m_State.back() + 1);
		for (StateBlock *pCurrent = &m_State.front();
			pCurrent != pEnd;
			pCurrent++)
		{
			switch (pCurrent->m_dwType)
			{
			case STATE_BLOCK_RENDERSTATE:
				hr = m_RenderState.SetRenderState(pCurrent->m_dwRenderState, pCurrent->m_dwValue);
				break;
			case STATE_BLOCK_TEXTURESTATE:
				hr = m_RenderState.SetTextureStageState(pCurrent->m_dwTextureStageIndex, pCurrent->m_dwTextureState, pCurrent->m_dwValue);
				break;
			case STATE_BLOCK_SAMPLERSTATE:
				hr = m_RenderState.SetSamplerState(pCurrent->dwSampler, pCurrent->dwType, pCurrent->m_dwValue);
				break;
			}
		}
    }
	g_pd3dDevice->GetMaterial(&matSave);
	if (m_dwMaterialFlag & MT_FLAG_AMBIENT)
	{
		matSave.Ambient = m_Material.Ambient;
	}
	if (m_dwMaterialFlag & MT_FLAG_DIFFUSE)
	{
		float fAlpha = matSave.Diffuse.a;
		matSave.Diffuse = m_Material.Diffuse;
		
		if(IsgUseGadualChange(m_Material.Diffuse,m_Diffuse,0.0001f))
		{
			DWORD dwCurTime = timeGetTime();
			DWORD T	=dwCurTime - m_dwLastTime;
			float H = 0.0025f;
 			if (T>400)
			{
				m_dwLastTime = dwCurTime;
				T = 0; 
				m_LastDiffuse = m_CurDiffuse;
				m_CurDiffuse = m_Material.Diffuse;
			}

			matSave.Diffuse.r = m_LastDiffuse.r + ((m_CurDiffuse.r - m_LastDiffuse.r)* ((float)T*H));
			matSave.Diffuse.g = m_LastDiffuse.g + ((m_CurDiffuse.g - m_LastDiffuse.g)* ((float)T*H));
			matSave.Diffuse.b = m_LastDiffuse.b + ((m_CurDiffuse.b - m_LastDiffuse.b)* ((float)T*H));
			
			m_Diffuse = matSave.Diffuse;
		}
		matSave.Diffuse.a = fAlpha;
	}
	if (m_dwMaterialFlag & MT_FLAG_SPECULAR)
	{
		matSave.Specular = m_Material.Specular;
	}

	m_MaterialSave = matSave;
	g_pd3dDevice->SetMaterial(&matSave);
	{
		std::map<int, LightPara>::iterator i = m_LightDiffuse.begin();
		while (i != m_LightDiffuse.end())
		{
			D3DLIGHT9 light;
			hr = g_pd3dDevice->GetLight(i->first, &light);
			if (SUCCEEDED(hr))
			{
				i->second.SaveValue = light.Diffuse;
				light.Diffuse = i->second.Value;
				g_pd3dDevice->SetLight(i->first, &light);
			}
			i++;
		}
	}

	if (m_bEnableLightAmbient)
	{
		m_RenderState.SetRenderState(D3DRS_AMBIENT, m_dwAmbient);//这个事实上是SunLight的
	}
    hRetCode = S_OK;
Exit0:
	return hRetCode;
}

void KG3DRuntimeMaterial::Clear()
{
	m_State.clear();	//注意，这里其实是没有释放内存的，
	m_State.reserve(32);	//这里保证一下内存
}

void KG3DRuntimeMaterial::EndRender()
{
	m_RenderState.Restore();
	std::map<int, LightPara>::iterator i = m_LightDiffuse.begin();
	while (i != m_LightDiffuse.end())
	{
		D3DLIGHT9 light;
		HRESULT hr = g_pd3dDevice->GetLight(i->first, &light);
		if (SUCCEEDED(hr))
		{
			light.Diffuse = i->second.SaveValue;
			g_pd3dDevice->SetLight(i->first, &light);
		}
		i++;
	}
	g_pd3dDevice->SetMaterial(&m_MaterialSave);
	/*m_State.clear();*/

	this->Clear();
}

HRESULT KG3DRuntimeMaterial::SetTexture(DWORD dwTextureStage, KG3DTexture* pTexture)
{
	return E_FAIL;
}

void KG3DRuntimeMaterial::SetRenderState(
					   D3DRENDERSTATETYPE State,
					   DWORD Value)
{
	{
		//避免重复的
		if (! m_State.empty())
		{
			StateBlock *pEnd = (&m_State.back() + 1);
			for (StateBlock *pCurrent = &m_State.front();
				pCurrent != pEnd;
				++pCurrent)
			{
				if (pCurrent->m_dwType != STATE_BLOCK_RENDERSTATE)
					continue;
				if (pCurrent->m_dwRenderState == State)
				{
					pCurrent->m_dwValue = Value;
					KG_PROCESS_SUCCESS(1);
				}
			}
		}
        /*StateBlock *pStateBlock = m_State.push_back();
        ASSERT(pStateBlock);
        pStateBlock->m_dwType = STATE_BLOCK_RENDERSTATE; 
        pStateBlock->m_dwRenderState = State;
        pStateBlock->m_dwValue = Value;*/

		StateBlock tempBlock;

		tempBlock.m_dwType = STATE_BLOCK_RENDERSTATE; 
		tempBlock.m_dwRenderState = State;
		tempBlock.m_dwValue = Value;

		m_State.push_back(tempBlock);
	}
Exit1:
	;
}

void KG3DRuntimeMaterial::SetTextureStageState(
							 DWORD Stage,
							 D3DTEXTURESTAGESTATETYPE Type,
							 DWORD Value
							 )
{
	{
		//避免重复的
		if (! m_State.empty())
		{
			StateBlock *pEnd = (&m_State.back() + 1);
			for (StateBlock *pCurrent = &m_State.front();
				pCurrent != pEnd;
				++pCurrent)
			{
				if (pCurrent->m_dwType != STATE_BLOCK_TEXTURESTATE)
					continue;
				if (pCurrent->m_dwTextureStageIndex == Stage && pCurrent->m_dwTextureState == Type)
				{
					pCurrent->m_dwValue = Value;
					KG_PROCESS_SUCCESS(1);
				}
			}
		}

        /*StateBlock *pStateBlock = m_State.push_back();
        ASSERT(pStateBlock);
        pStateBlock->m_dwType = STATE_BLOCK_TEXTURESTATE; 
        pStateBlock->m_dwTextureStageIndex = Stage;
        pStateBlock->m_dwTextureState = Type;
        pStateBlock->m_dwValue = Value;*/

		StateBlock tempBlock;
		tempBlock.m_dwType = STATE_BLOCK_TEXTURESTATE; 
		tempBlock.m_dwTextureStageIndex = Stage;
		tempBlock.m_dwTextureState = Type;
		tempBlock.m_dwValue = Value;

		m_State.push_back(tempBlock);
	}
Exit1:
	;
}

void KG3DRuntimeMaterial::SetSamplerState(
						DWORD Sampler,
						D3DSAMPLERSTATETYPE Type,
						DWORD Value
						)
{
	{
		//避免重复的
		if (! m_State.empty())
		{
			StateBlock *pEnd = (&m_State.back() + 1);
			for (StateBlock *pCurrent = &m_State.front();
				pCurrent != pEnd;
				++pCurrent)
			{
				if (pCurrent->m_dwType != STATE_BLOCK_SAMPLERSTATE)
					continue;
				if (pCurrent->dwSampler == Sampler && pCurrent->dwType == Type)
				{
					pCurrent->m_dwValue = Value;
					KG_PROCESS_SUCCESS(1);
				}
			}
		}

       /* StateBlock *pStateBlock = m_State.push_back();
        ASSERT(pStateBlock);
        pStateBlock->m_dwType = STATE_BLOCK_SAMPLERSTATE; 
        pStateBlock->dwSampler = Sampler;
        pStateBlock->dwType = Type;
        pStateBlock->m_dwValue = Value;*/

		StateBlock tempBlock;
		tempBlock.m_dwType = STATE_BLOCK_SAMPLERSTATE; 
		tempBlock.dwSampler = Sampler;
		tempBlock.dwType = Type;
		tempBlock.m_dwValue = Value;

		m_State.push_back(tempBlock);
	}
Exit1:
	;
}
void KG3DRuntimeMaterial::SetLightDiffuse(int nIndex, const D3DCOLORVALUE& Diffuse)
{
	KG_PROCESS_ERROR(nIndex < 8);
	m_LightDiffuse[nIndex].Value = Diffuse;
Exit0:
	;
}

void KG3DRuntimeMaterial::ApplyMaterial(D3DMATERIAL9 &material)
{
	if (m_dwMaterialFlag & MT_FLAG_AMBIENT)
	{
		material.Ambient = m_Material.Ambient;
	}

	material.Diffuse.a = m_fAlpha;
	if (m_fAlpha < 1.0f)
	{
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		DWORD dwAlphaRef = 0;
		g_pd3dDevice->GetRenderState(D3DRS_ALPHAREF, &dwAlphaRef);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, static_cast<DWORD>(dwAlphaRef * m_fAlpha));
		g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	}
	else
	{
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	}

	material.Emissive.a = 1.0f;
	material.Emissive.r = m_fIlluminationLevel;
	material.Emissive.g = m_fIlluminationLevel;
	material.Emissive.b = m_fIlluminationLevel;
	
	if (m_dwMaterialFlag & MT_FLAG_DIFFUSE)
	{
		float fAlpha = material.Diffuse.a;
		material.Diffuse = m_Material.Diffuse;

		if(IsgUseGadualChange(m_Material.Diffuse,m_Diffuse,0.0001f))
		{
			DWORD dwCurTime = timeGetTime();
			DWORD T	=dwCurTime - m_dwLastTime;
			float H = 0.0025f;
			if (T>400)
			{
				m_dwLastTime = dwCurTime;
				T = 0; 
				m_LastDiffuse = m_CurDiffuse;
				m_CurDiffuse = m_Material.Diffuse;
			}

			material.Diffuse.r = m_LastDiffuse.r + ((m_CurDiffuse.r - m_LastDiffuse.r)* ((float)T*H));
			material.Diffuse.g = m_LastDiffuse.g + ((m_CurDiffuse.g - m_LastDiffuse.g)* ((float)T*H));
			material.Diffuse.b = m_LastDiffuse.b + ((m_CurDiffuse.b - m_LastDiffuse.b)* ((float)T*H));

			m_Diffuse = material.Diffuse;
		}
		material.Diffuse.a = fAlpha;
	}
	if (m_dwMaterialFlag & MT_FLAG_SPECULAR)
	{
		material.Specular = m_Material.Specular;
	}
}

void KG3DRuntimeMaterial::SetAlpha(float fAlpha)
{
	m_fAlpha = fAlpha;
}

void KG3DRuntimeMaterial::SetHighLight(float fIlluminationLevel)
{
	if (fIlluminationLevel > 1.0f)
	{
		m_fIlluminationLevel = g_fHightLight;
	}
	else
	{
		m_fIlluminationLevel = 0.0f;
	}
}