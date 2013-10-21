#include "StdAfx.h"
//#include "KGTester.h"
#include "GraphicsStruct.h"
#include "Mathtool.h"
//不要使用任何涉及非引擎接口的h，这种类移到KG3DGraphicsStructEx.h(Helpers下面)。

extern LPDIRECT3DDEVICE9  g_pd3dDevice;


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace GraphicsStruct
{
	RenderStateAlpha::RenderStateAlpha(DWORD AlphaRef,DWORD AlphaTestEnable,
		DWORD AlphaBlendEnable,DWORD SrcBlend,DWORD DestBlend,DWORD AlphaFuc)
	{
		g_pd3dDevice->GetRenderState(D3DRS_ALPHAREF,&dwBackAlphaRef);
		g_pd3dDevice->GetRenderState(D3DRS_ALPHATESTENABLE ,&dwBackAlphaTestEnable);
		g_pd3dDevice->GetRenderState(D3DRS_ALPHABLENDENABLE,&dwBackAlphaBlendEnable);
		g_pd3dDevice->GetRenderState(D3DRS_SRCBLEND,&dwBackSrcBlend);
		g_pd3dDevice->GetRenderState(D3DRS_DESTBLEND,&dwBackDestlend);
		g_pd3dDevice->GetRenderState(D3DRS_ALPHAFUNC,&dwBackAlphaFuc);

		g_pd3dDevice->SetRenderState(D3DRS_ALPHAREF,AlphaRef);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE ,AlphaTestEnable);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,AlphaBlendEnable);
		g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, SrcBlend);
		g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,DestBlend);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC,AlphaFuc);
	}

	RenderStateAlpha::~RenderStateAlpha()
	{
		g_pd3dDevice->SetRenderState(D3DRS_ALPHAREF,dwBackAlphaRef);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE ,dwBackAlphaTestEnable);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,dwBackAlphaBlendEnable);
		g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, dwBackSrcBlend);
		g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,dwBackDestlend);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC,dwBackAlphaFuc);
	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	RenderStateCullMode::RenderStateCullMode(DWORD dwCullMode)
	{
		g_pd3dDevice->GetRenderState(D3DRS_CULLMODE,&dwBackCullModel);
		g_pd3dDevice->SetRenderState(D3DRS_CULLMODE,dwCullMode);

	}
	RenderStateCullMode::~RenderStateCullMode()
	{
		g_pd3dDevice->SetRenderState(D3DRS_CULLMODE,dwBackCullModel);
	}
	RenderState::RenderState(D3DRENDERSTATETYPE eType,DWORD dwState)
	{
		eStateType = eType;
		g_pd3dDevice->GetRenderState(eType,&dwBackRenderState);
		g_pd3dDevice->SetRenderState(eType,dwState);
	}
	RenderState::~RenderState()
	{
		g_pd3dDevice->SetRenderState(eStateType,dwBackRenderState);
	}

	TextureStageState::TextureStageState( DWORD dwStage, D3DTEXTURESTAGESTATETYPE eType, DWORD dwState )
		:m_dwStage(dwStage)
		,m_eStateType(eType)
	{
		g_pd3dDevice->GetTextureStageState(dwStage, eType, &m_dwBackTexStageState);
		g_pd3dDevice->SetTextureStageState(dwStage, eType, dwState);		
	}

	TextureStageState::~TextureStageState()
	{
		g_pd3dDevice->SetTextureStageState(m_dwStage, m_eStateType, m_dwBackTexStageState);
	}

	

	

	SamplerState::SamplerState(DWORD dwSampler, D3DSAMPLERSTATETYPE eType, DWORD dwValue)
		:m_bHasModify(FALSE)
	{
		m_dwSampler = dwSampler;
		m_eStateType = eType;
		g_pd3dDevice->GetSamplerState(dwSampler, eType, &m_dwBackSamplerState);
		if (m_dwBackSamplerState != dwValue)
		{
			g_pd3dDevice->SetSamplerState(dwSampler, eType, dwValue);
			m_bHasModify = TRUE;
		}	
	}

	SamplerState::~SamplerState()
	{
		if (m_bHasModify)
		{
			g_pd3dDevice->SetSamplerState(m_dwSampler, m_eStateType, m_dwBackSamplerState);
			m_bHasModify = FALSE;
		}
	}
};

VOID BoxToRect( const BBox& box, FRECT& rect )
{
	rect.Left = box.A.x;
	rect.Width = fabs(box.B.x - box.A.x);
	rect.Bottom = box.A.z;
	rect.Height = fabs(box.B.z - box.A.z);
}
