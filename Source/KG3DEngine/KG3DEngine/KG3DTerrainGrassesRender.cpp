#include "StdAfx.h"
#include "KG3DTerrainGrassesRender.h"
#include "KG3DTerrainBlockGrass.h"
#include "KG3DRenderState.h"
#include "KG3DEnvEffLight.h"
#include "KG3DTerrainBlock.h"
#include "KG3DTexture.h"
#include "KG3DTextureTable.h"
#include "KG3DSubSetRenderer.h"
#include "KG3DDefaultShaderGroup.h"
#include "../EditInterface/IEKG3DEnvEffWind.h"
#include "Scene/KG3DScene.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern DWORD              g_dwRenderCount;
extern int g_nDepthTexturePackingType;

KG3DTerrainGrassesRender::KG3DTerrainGrassesRender(void)
{
	m_lpDeclaration = NULL;
	m_dwLastAddRenderCount = 0;
}

KG3DTerrainGrassesRender::~KG3DTerrainGrassesRender(void)
{
	UnInit();
}

HRESULT KG3DTerrainGrassesRender::Init()
{
	HRESULT hr = E_FAIL;

	D3DVERTEXELEMENT9 Declaration2[] = 
	{
		{0, 0 , D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
        {0, 16, D3DDECLTYPE_D3DCOLOR , D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};
	hr = g_pd3dDevice->CreateVertexDeclaration(Declaration2, &m_lpDeclaration);
	KGLOG_COM_PROCESS_ERROR(hr);

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DTerrainGrassesRender::UnInit()
{
	SAFE_RELEASE(m_lpDeclaration);
	EmptyRenderList();
	return S_OK;
}

HRESULT KG3DTerrainGrassesRender::OnLostDevice()
{
    EmptyRenderList();

    return S_OK;
}

HRESULT KG3DTerrainGrassesRender::OnResetDevice()
{
    return S_OK;
}

HRESULT KG3DTerrainGrassesRender::EmptyGrassBlockSet(set<KG3DTerrainBlockGrass *>* pSet, vector<KG3DTerrainBlockGrass *> *pRendBlocks)
{
	set<KG3DTerrainBlockGrass *>::iterator i = pSet->begin();
	set<KG3DTerrainBlockGrass *>::iterator iend = pSet->end();
	while (i!=iend)
	{
		KG3DTerrainBlockGrass* pGrass = *i;
		SAFE_RELEASE(pGrass);
		i++;
	}
	pSet->clear();


    for (size_t i = 0; i < pRendBlocks->size(); ++i)
    {
        SAFE_RELEASE((*pRendBlocks)[i]);
    }
    pRendBlocks->clear();

	return S_OK;
}

HRESULT KG3DTerrainGrassesRender::InputGrassBlock(KG3DTerrainBlockGrass* pGrass, set<KG3DTerrainBlockGrass *>* pSet)
{
    pair<set<KG3DTerrainBlockGrass *>::iterator, BOOL> InsertResult;

	KGLOG_PROCESS_ERROR(pGrass);

    InsertResult = pSet->insert(pGrass);
    if (InsertResult.second) // true: insert success; flase: already in set
    {
        pGrass->AddRef();
        pGrass->PrepareShareGrassVB();    
    }

	pGrass->m_dwLastVisibleCount = g_dwRenderCount;

Exit0:
	return S_OK;
}

HRESULT KG3DTerrainGrassesRender::RemoveGrassBlock(KG3DTerrainBlockGrass* pGrass, set<KG3DTerrainBlockGrass *>* pSet)
{
    set<KG3DTerrainBlockGrass *>::iterator itFinder;
	KGLOG_PROCESS_ERROR(pGrass);

    itFinder = pSet->find(pGrass);

	if(itFinder != pSet->end())
	{
		pSet->erase(itFinder);
		pGrass->UnPrepareShareGrassVB();
		SAFE_RELEASE(pGrass);
	}

Exit0:
	return S_OK;
}

HRESULT KG3DTerrainGrassesRender::InputGrassBlock(KG3DTerrainBlockGrass* pGrass)
{
	return InputGrassBlock(pGrass,&m_setTerrainGrassCache);
}
HRESULT KG3DTerrainGrassesRender::InputStoneBlock(KG3DTerrainBlockGrass* pStone)
{
	return InputGrassBlock(pStone,&m_setTerrainStoneCache);
}
HRESULT KG3DTerrainGrassesRender::InputFrutexBlock(KG3DTerrainBlockGrass* pFrutex)
{
	return InputGrassBlock(pFrutex,&m_setTerrainFrutexCache);
}

HRESULT KG3DTerrainGrassesRender::RemoveGrassBlock(KG3DTerrainBlockGrass* pGrass)
{
	return RemoveGrassBlock(pGrass,&m_setTerrainGrassCache);
}
HRESULT KG3DTerrainGrassesRender::RemoveStoneBlock(KG3DTerrainBlockGrass* pStone)
{
	return RemoveGrassBlock(pStone,&m_setTerrainStoneCache);
}
HRESULT KG3DTerrainGrassesRender::RemoveFrutexBlock(KG3DTerrainBlockGrass* pFrutex)
{
	return RemoveGrassBlock(pFrutex,&m_setTerrainFrutexCache);
}


HRESULT KG3DTerrainGrassesRender::EmptyRenderList()
{
	EmptyGrassBlockSet(&m_setTerrainGrassCache, &m_vecGrassRenderBlocks);
	EmptyGrassBlockSet(&m_setTerrainStoneCache, &m_vecStoneRenderBlocks);
	EmptyGrassBlockSet(&m_setTerrainFrutexCache, &m_vecFrutexRenderBlocks);

	return S_OK;
}


void SetWindFx(LPD3DXEFFECT fx, KG3DWIND_DESCSTRUCT &windconfig)
{
	float amplitude[2] = {windconfig.m_WindAmplitude1, windconfig.m_WindAmplitude2};
	float wavelen[2] = {windconfig.m_WindWaveLen1, windconfig.m_WindWaveLen1};
	float windspeed[2] = {windconfig.m_WindSpeed1, windconfig.m_WindSpeed2};
	float waveup[2] = {windconfig.m_WindWaveScaleUp1, windconfig.m_WindWaveScaleUp2};
	float wavedown[2] = {windconfig.m_WindWaveScaleDown1, windconfig.m_WindWaveScaleDown2};
	float offset[2] = {windconfig.m_WindWaveOffset1, windconfig.m_WindWaveOffset2};
	D3DXVECTOR3 dir(windconfig.m_WindDirX, windconfig.m_WindDirY, windconfig.m_WindDirZ);
	fx->SetBool("bEnableWind", windconfig.m_bEnableDirWind);
	fx->SetBool("bEnableWave1", windconfig.m_bEnableWave1);
	fx->SetBool("bEnableWave2", windconfig.m_bEnableWave2);
	fx->SetValue("directDelta", &dir, sizeof(D3DXVECTOR3));
	fx->SetInt("sway", (int)windconfig.m_Sway);
	fx->SetFloatArray("amplitude", amplitude, 2);
	fx->SetFloatArray("wavelen", wavelen, 2);
	fx->SetFloatArray("windspeed", windspeed, 2);
	fx->SetFloatArray("waveup", waveup, 2);
	fx->SetFloatArray("wavedown", wavedown, 2);
	fx->SetFloatArray("offset", offset, 2);	
}


static DWORD s_dwAlphaRef = 128;
HRESULT KG3DTerrainGrassesRender::RenderGrassDepthOnly(BOOL bShader,BOOL bGrass,BOOL bStone,BOOL bFrutex)
{
	HRESULT hr = E_FAIL;
    BOOL bSupportTechnique = FALSE;
	KG3DWIND_DESCSTRUCT windconfig;

	KG3DRenderState State;
	State.SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
	State.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	State.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	State.SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	State.SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
	State.SetRenderState(D3DRS_ALPHAREF, s_dwAlphaRef);
	State.SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	//State.SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALPHA);
	DWORD dwColorWrite = 0;
	if(g_cEngineOption.bUseRenderTarget_DepthTexture)
	{
		if (g_nDepthTexturePackingType == 0)
			dwColorWrite = D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_ALPHA;
		else
			dwColorWrite = D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA;
	}

	State.SetRenderState(D3DRS_COLORWRITEENABLE, dwColorWrite);
	State.SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	State.SetRenderState(D3DRS_LIGHTING,FALSE);
	State.SetRenderState(D3DRS_ZWRITEENABLE,TRUE);	
	LPD3DXEFFECT pGrassEffect = g_GetDefaultShaderGroup().GetDefaultEffect(DEFST_GRASS);;
	KG_PROCESS_ERROR(NULL != pGrassEffect);
	if(bShader)
	{
		UINT uPass;
		D3DXMATRIX matView,matProj,m;
		g_pd3dDevice->GetTransform(D3DTS_VIEW,&matView);
		g_pd3dDevice->GetTransform(D3DTS_PROJECTION,&matProj);
		m = matView * matProj;

		pGrassEffect->SetMatrix("matWVP",&m);
		pGrassEffect->SetMatrix("matWV",&matView);

        hr = pGrassEffect->SetTechnique("tecZ");

		pGrassEffect->SetFloat("Time",g_cGraphicsTool.GetNowTime()*0.001F);
		
        bSupportTechnique = SUCCEEDED(hr);

		pGrassEffect->Begin(&uPass,0);
		pGrassEffect->BeginPass(0);
	}

	//读取风的配置信息
	KG3DScene* pScene = g_cGraphicsTool.GetCurScene();
	windconfig.SetDefaultValue();
	windconfig.m_id = 0; //如果风向配置可选择，这个是需要修改的，这里暂时设定成第一个风向有效 
	if (NULL != pScene)
	{
		IEKG3DEnvEffWind* pWind = NULL;
		if(SUCCEEDED(pScene->GetEngineIEInterface(KG3DTYPE_ENVEFF_WIND, 0, 0, (void **)&pWind)))
		{
			_ASSERTE(NULL != pWind);
			pWind->GetStruct(&windconfig);
		}
	}

	///////绘制草////////////////////////////////////////////
	int K = 0;
	if(bGrass)
	{
        vector<KG3DTerrainBlockGrass *>::iterator itIndex = m_vecGrassRenderBlocks.begin();
        vector<KG3DTerrainBlockGrass *>::iterator itEnd = m_vecGrassRenderBlocks.end();
        while (itIndex != itEnd)
		{
            KG3DTerrainBlockGrass* pGrassBlock = *itIndex;
			if(!K)
			{
				pGrassEffect->SetBool("bAni",g_cEngineOption.bGrassAnimation);
				g_pd3dDevice->SetTexture(0,pGrassBlock->GetGrassTexture());
				SetWindFx(pGrassEffect, windconfig);
				pGrassEffect->CommitChanges();
				K++;
			}

			DWORD dwFace = 0;
			LPDIRECT3DVERTEXBUFFER9 pVB = pGrassBlock->GetShareGrassVBuffer(dwFace);

			if(pVB && dwFace)
			{
				g_pd3dDevice->SetVertexDeclaration(m_lpDeclaration);
				g_pd3dDevice->SetStreamSource(0,pVB,0,sizeof(KG3DTerrainGrassVertex));
				g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,dwFace);
			}
            ++itIndex;
		}
	}
	/////绘制碎石///////////////////////////////////////
	K = 0;
    if (bStone)
    {
        vector<KG3DTerrainBlockGrass *>::iterator itIndex = m_vecStoneRenderBlocks.begin();
        vector<KG3DTerrainBlockGrass *>::iterator itEnd = m_vecStoneRenderBlocks.end();
        while (itIndex != itEnd)
        {
            KG3DTerrainBlockGrass* pStoneBlock = *itIndex;
            if(!K)
            {
				pGrassEffect->SetBool("bAni",FALSE);
                g_pd3dDevice->SetTexture(0,pStoneBlock->GetGrassTexture());
				pGrassEffect->CommitChanges();
                K++;
            }

            DWORD dwFace = 0;
            LPDIRECT3DVERTEXBUFFER9 pVB = pStoneBlock->GetShareGrassVBuffer(dwFace);

            if(pVB && dwFace)
            {
                g_pd3dDevice->SetVertexDeclaration(m_lpDeclaration);
                g_pd3dDevice->SetStreamSource(0,pVB,0,sizeof(KG3DTerrainGrassVertex));
                g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,dwFace);
            }

            ++itIndex;
        }
    }

	//////绘制灌木///////////////////////////////////////
	K = 0;
    if (bFrutex)
    {
        vector<KG3DTerrainBlockGrass *>::iterator itIndex = m_vecFrutexRenderBlocks.begin();
        vector<KG3DTerrainBlockGrass *>::iterator itEnd = m_vecFrutexRenderBlocks.end();
        while (itIndex != itEnd)
		{
            KG3DTerrainBlockGrass* pFrutexBlock = *itIndex;

            if(!K)
            {
				pGrassEffect->SetBool("bAni",g_cEngineOption.bGrassAnimation);
                g_pd3dDevice->SetTexture(0,pFrutexBlock->GetGrassTexture());
				SetWindFx(pGrassEffect, windconfig);
				pGrassEffect->CommitChanges();
                K++;
            }

            DWORD dwFace = 0;
            LPDIRECT3DVERTEXBUFFER9 pVB = pFrutexBlock->GetShareGrassVBuffer(dwFace);

            if(pVB && dwFace)
            {
                g_pd3dDevice->SetVertexDeclaration(m_lpDeclaration);
                g_pd3dDevice->SetStreamSource(0,pVB,0,sizeof(KG3DTerrainGrassVertex));
                g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,dwFace);
            }
            ++itIndex;
        }
    }


	if (bSupportTechnique)
	{
		pGrassEffect->EndPass();
		pGrassEffect->End();
	}

    g_pd3dDevice->SetTexture(0, NULL);

	return S_OK;
Exit0:
	return E_FAIL;
}


HRESULT KG3DTerrainGrassesRender::RenderGrassColorOnly(KG3DTerrainConverMap* pConverMap,BOOL bGrass,BOOL bStone,BOOL bFrutex)
{
    HRESULT hr = E_FAIL;
	BOOL bUseShader = TRUE;
    BOOL bSupportTechnique = FALSE;
	KG3DWIND_DESCSTRUCT windconfig;

	GraphicsStruct::RenderState cull(D3DRS_CULLMODE,D3DCULL_NONE);
	GraphicsStruct::RenderState light(D3DRS_LIGHTING,TRUE);
	GraphicsStruct::RenderState ZWrite(D3DRS_ZWRITEENABLE,FALSE);
	GraphicsStruct::RenderState ZCmp(D3DRS_ZFUNC,D3DCMP_EQUAL);
	GraphicsStruct::RenderStateAlpha Alpha(0xF0,FALSE,FALSE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);
	GraphicsStruct::RenderState colorWrite(D3DRS_COLORWRITEENABLE ,D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN);

	KG3DRenderState State;
	State.SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
	State.SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TEXTURE);
	State.SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE2X);
	State.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	State.SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
	State.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	State.SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
	State.SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
	
	g_pd3dDevice->SetTexture(0,NULL);
	g_pd3dDevice->SetTexture(1,NULL);
	//////////////////////////////////////////////////////////////////////////

	LPD3DXEFFECT pGrassEffect = g_GetDefaultShaderGroup().GetDefaultEffect(DEFST_GRASS);;
	KG_PROCESS_ERROR(NULL != pGrassEffect);
	if( bUseShader )
	{
		UINT uPass;
		D3DXMATRIX matView,matProj,m;
		g_pd3dDevice->GetTransform(D3DTS_VIEW,&matView);
		g_pd3dDevice->GetTransform(D3DTS_PROJECTION,&matProj);
		m = matView * matProj;

		pGrassEffect->SetMatrix("matWVP",&m);


        hr = pGrassEffect->SetTechnique("Tech"); // TODO: check result
        if (SUCCEEDED(hr))
        {
            pGrassEffect->Begin(&uPass,0);
            pGrassEffect->BeginPass(0);

            bSupportTechnique = TRUE;
        }
	}
	//读取风的配置信息
	KG3DScene* pScene = g_cGraphicsTool.GetCurScene();
	windconfig.SetDefaultValue();
	windconfig.m_id = 0; //如果风向配置可选择，这个是需要修改的，这里暂时设定成第一个风向有效 
	if (NULL != pScene)
	{
		IEKG3DEnvEffWind* pWind = NULL;
		if(SUCCEEDED(pScene->GetEngineIEInterface(KG3DTYPE_ENVEFF_WIND, 0, 0, (void **)&pWind)))
		{
			_ASSERTE(NULL != pWind);
			pWind->GetStruct(&windconfig);
		}
	}

	//////绘制草///////////////////////////////////////
	if(bGrass)
	{
		int K = 0;
        vector<KG3DTerrainBlockGrass *>::iterator itIndex = m_vecGrassRenderBlocks.begin();
        vector<KG3DTerrainBlockGrass *>::iterator itEnd = m_vecGrassRenderBlocks.end();
        while (itIndex != itEnd)
		{
            KG3DTerrainBlockGrass* pGrass = *itIndex;

			if(!K)
			{
				g_pd3dDevice->SetTexture(0,pGrass->GetGrassTexture());
				pGrassEffect->SetBool("bAni",g_cEngineOption.bGrassAnimation);
				SetWindFx(pGrassEffect, windconfig);
				pGrassEffect->CommitChanges();
				K++;
			}

			DWORD dwFace = 0;
			LPDIRECT3DVERTEXBUFFER9 pVB = pGrass->GetShareGrassVBuffer(dwFace);

			if(pVB && dwFace)
			{
				if (pGrass->m_lpParentTerrainBlock && pGrass->m_lpParentTerrainBlock->m_lpPointLightInfo)
				{
					g_SetPointLighting(pGrassEffect,pGrass->m_lpParentTerrainBlock->m_lpPointLightInfo);
				}
				g_pd3dDevice->SetVertexDeclaration(m_lpDeclaration);
				g_pd3dDevice->SetStreamSource(0,pVB,0,sizeof(KG3DTerrainGrassVertex));
				g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,dwFace);
			}
            ++itIndex;
		}
	}

	//////绘制碎石///////////////////////////////////////
	if(bStone)
	{
		int K = 0;
        vector<KG3DTerrainBlockGrass *>::iterator itIndex = m_vecStoneRenderBlocks.begin();
        vector<KG3DTerrainBlockGrass *>::iterator itEnd = m_vecStoneRenderBlocks.end();
        while (itIndex != itEnd)
		{
            KG3DTerrainBlockGrass* pStoneBlock = *itIndex;

			if(!K)
			{
				g_pd3dDevice->SetTexture(0,pStoneBlock->GetGrassTexture());
				pGrassEffect->SetBool("bAni",FALSE);
				pGrassEffect->CommitChanges();
				K++;
			}

			DWORD dwFace = 0;
			LPDIRECT3DVERTEXBUFFER9 pVB = pStoneBlock->GetShareGrassVBuffer(dwFace);

			if(pVB && dwFace)
			{
				if (pStoneBlock->m_lpParentTerrainBlock && pStoneBlock->m_lpParentTerrainBlock->m_lpPointLightInfo)
				{
					g_SetPointLighting(pGrassEffect,pStoneBlock->m_lpParentTerrainBlock->m_lpPointLightInfo);
				}
				g_pd3dDevice->SetVertexDeclaration(m_lpDeclaration);
				g_pd3dDevice->SetStreamSource(0,pVB,0,sizeof(KG3DTerrainGrassVertex));
				g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,dwFace);
			}
            ++itIndex;
		}
	}
	//////绘制灌木///////////////////////////////////////
	if(bFrutex)
	{
		int K = 0;
        vector<KG3DTerrainBlockGrass *>::iterator itIndex = m_vecFrutexRenderBlocks.begin();
        vector<KG3DTerrainBlockGrass *>::iterator itEnd = m_vecFrutexRenderBlocks.end();
        while (itIndex != itEnd)
		{
            KG3DTerrainBlockGrass* pFrutexBlock = *itIndex;

			if(!K)
			{
				g_pd3dDevice->SetTexture(0,pFrutexBlock->GetGrassTexture());
				pGrassEffect->SetBool("bAni",g_cEngineOption.bGrassAnimation);
				SetWindFx(pGrassEffect, windconfig);
				pGrassEffect->CommitChanges();
				K++;
			}

			DWORD dwFace = 0;
			LPDIRECT3DVERTEXBUFFER9 pVB = pFrutexBlock->GetShareGrassVBuffer(dwFace);

			if(pVB && dwFace)
			{
				if (pFrutexBlock->m_lpParentTerrainBlock && pFrutexBlock->m_lpParentTerrainBlock->m_lpPointLightInfo)
				{
					g_SetPointLighting(pGrassEffect,pFrutexBlock->m_lpParentTerrainBlock->m_lpPointLightInfo);
				}
				g_pd3dDevice->SetVertexDeclaration(m_lpDeclaration);
				g_pd3dDevice->SetStreamSource(0,pVB,0,sizeof(KG3DTerrainGrassVertex));
				g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,dwFace);
			}
            ++itIndex;
		}
	}


	if (bUseShader && bSupportTechnique)
	{
		pGrassEffect->EndPass();
		pGrassEffect->End();
    }

    //pGrassEffect->SetTexture("tConver", NULL);//<ConverMap Shared>

    g_pd3dDevice->SetTexture(0, NULL);
	hr = S_OK;
Exit0:
    return hr;
}

HRESULT KG3DTerrainGrassesRender::RenderGrassColoraAlphaBlendOnly(KG3DTerrainConverMap* pConverMap,BOOL bGrass,BOOL bStone,BOOL bFrutex)
{
    HRESULT hr = E_FAIL;
	BOOL bUseShader = TRUE;
    BOOL bSupportTechnique = FALSE;
	KG3DWIND_DESCSTRUCT windconfig;

	GraphicsStruct::RenderState cull(D3DRS_CULLMODE,D3DCULL_NONE);
	GraphicsStruct::RenderState light(D3DRS_LIGHTING,TRUE);
	GraphicsStruct::RenderState ZWrite(D3DRS_ZWRITEENABLE,FALSE);
	GraphicsStruct::RenderState ZCmp(D3DRS_ZFUNC,D3DCMP_LESS);
	GraphicsStruct::RenderStateAlpha Alpha(0x20,FALSE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);
	GraphicsStruct::RenderState colorWrite(D3DRS_COLORWRITEENABLE ,D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN);

	KG3DRenderState State;
	State.SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
	State.SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TEXTURE);
	State.SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE2X);
	State.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	State.SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
	State.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	State.SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
	State.SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);

	g_pd3dDevice->SetTexture(0,NULL);
	g_pd3dDevice->SetTexture(1,NULL);
	LPD3DXEFFECT pGrassEffect = g_GetDefaultShaderGroup().GetDefaultEffect(DEFST_GRASS);
	KG_PROCESS_ERROR(NULL != pGrassEffect);

	if( bUseShader )
	{
		UINT uPass;
		D3DXMATRIX matView,matProj,m;
		g_pd3dDevice->GetTransform(D3DTS_VIEW,&matView);
		g_pd3dDevice->GetTransform(D3DTS_PROJECTION,&matProj);
		m = matView * matProj;

		pGrassEffect->SetMatrix("matWVP",&m);
		pGrassEffect->SetMatrix("matWV",&matView);

        hr = pGrassEffect->SetTechnique("Tech");
        if (SUCCEEDED(hr))
        {
            pGrassEffect->Begin(&uPass,0);
            pGrassEffect->BeginPass(0);

            bSupportTechnique = TRUE;
        }
	}

	//读取风的配置信息
	KG3DScene* pScene = g_cGraphicsTool.GetCurScene();
	windconfig.SetDefaultValue();
	windconfig.m_id = 0; //如果风向配置可选择，这个是需要修改的，这里暂时设定成第一个风向有效 
	if (NULL != pScene)
	{
		IEKG3DEnvEffWind* pWind = NULL;
		if(SUCCEEDED(pScene->GetEngineIEInterface(KG3DTYPE_ENVEFF_WIND, 0, 0, (void **)&pWind)))
		{
			_ASSERTE(NULL != pWind);
			pWind->GetStruct(&windconfig);
		}
	}

	//////绘制草///////////////////////////////////////
	if(bGrass)
	{
		int K = 0;
        vector<KG3DTerrainBlockGrass *>::iterator itIndex = m_vecGrassRenderBlocks.begin();
        vector<KG3DTerrainBlockGrass *>::iterator itEnd = m_vecGrassRenderBlocks.end();
        while (itIndex != itEnd)                
		{
            KG3DTerrainBlockGrass* pGrass = *itIndex;

			if(!K)
			{
				g_pd3dDevice->SetVertexDeclaration(m_lpDeclaration);
				pGrassEffect->SetBool("bAni",g_cEngineOption.bGrassAnimation);
				g_pd3dDevice->SetTexture(0,pGrass->GetGrassTexture());
				SetWindFx(pGrassEffect, windconfig);
				pGrassEffect->CommitChanges();
				K++;
			}

			DWORD dwFace = 0;
			LPDIRECT3DVERTEXBUFFER9 pVB = pGrass->GetShareGrassVBuffer(dwFace);

			if(pVB && dwFace)
			{
				g_pd3dDevice->SetStreamSource(0,pVB,0,sizeof(KG3DTerrainGrassVertex));
				g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,dwFace);
			}
            ++itIndex;
		}
	}
	//////绘制碎石///////////////////////////////////////
	if(bStone)
	{
		int K = 0;
        vector<KG3DTerrainBlockGrass *>::iterator itIndex = m_vecStoneRenderBlocks.begin();
        vector<KG3DTerrainBlockGrass *>::iterator itEnd = m_vecStoneRenderBlocks.end();
        while (itIndex != itEnd)
		{
            KG3DTerrainBlockGrass* pStoneBlock = *itIndex;

			if(!K)
			{
				g_pd3dDevice->SetVertexDeclaration(m_lpDeclaration);
				pGrassEffect->SetBool("bAni",FALSE);
				g_pd3dDevice->SetTexture(0,pStoneBlock->GetGrassTexture());
				pGrassEffect->CommitChanges();
				K++;
			}

			DWORD dwFace = 0;
			LPDIRECT3DVERTEXBUFFER9 pVB = pStoneBlock->GetShareGrassVBuffer(dwFace);

			if(pVB && dwFace)
			{
				g_pd3dDevice->SetStreamSource(0,pVB,0,sizeof(KG3DTerrainGrassVertex));
				g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,dwFace);
			}
            ++itIndex;
		}
	}
	//////绘制灌木///////////////////////////////////////
	if(bFrutex)
	{
		int K = 0;
        vector<KG3DTerrainBlockGrass *>::iterator itIndex = m_vecFrutexRenderBlocks.begin();
        vector<KG3DTerrainBlockGrass *>::iterator itEnd = m_vecFrutexRenderBlocks.end();
        while (itIndex != itEnd)
		{
            KG3DTerrainBlockGrass* pFrutexBlock = *itIndex;

			if(!K)
			{
				g_pd3dDevice->SetVertexDeclaration(m_lpDeclaration);
				pGrassEffect->SetBool("bAni",g_cEngineOption.bGrassAnimation);
				g_pd3dDevice->SetTexture(0,pFrutexBlock->GetGrassTexture());
				SetWindFx(pGrassEffect, windconfig);
				pGrassEffect->CommitChanges();
				K++;
			}

			DWORD dwFace = 0;
			LPDIRECT3DVERTEXBUFFER9 pVB = pFrutexBlock->GetShareGrassVBuffer(dwFace);

			if(pVB && dwFace)
			{
				g_pd3dDevice->SetStreamSource(0,pVB,0,sizeof(KG3DTerrainGrassVertex));
				g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,dwFace);
			}
            ++itIndex;
		}
	}

	if (bUseShader && bSupportTechnique)
	{
		pGrassEffect->EndPass();
		pGrassEffect->End();
	}

    g_pd3dDevice->SetTexture(0, NULL);

	hr =S_OK;
Exit0:
	return hr;
}

HRESULT KG3DTerrainGrassesRender::FrameMove()
{   
    FrameMoveGrassBlockSet(&m_setTerrainGrassCache, &m_vecGrassRenderBlocks);
    FrameMoveGrassBlockSet(&m_setTerrainStoneCache, &m_vecStoneRenderBlocks);
    FrameMoveGrassBlockSet(&m_setTerrainFrutexCache, &m_vecFrutexRenderBlocks);

	return S_OK;
}

HRESULT KG3DTerrainGrassesRender::FrameMoveGrassBlockSet(set<KG3DTerrainBlockGrass *> *pSet, vector<KG3DTerrainBlockGrass *> *pRendBlocks)
{
	DWORD dwOut = 30;
	set<KG3DTerrainBlockGrass *>::iterator i = pSet->begin();
	set<KG3DTerrainBlockGrass *>::iterator iend = pSet->end();

    for (size_t i = 0; i < pRendBlocks->size(); ++i)
    {
        SAFE_RELEASE((*pRendBlocks)[i]);
    }
    pRendBlocks->clear();

	while (i!=iend)
	{
		KG3DTerrainBlockGrass* pGrass = *i;
        ASSERT(pGrass);
		if (g_dwRenderCount - pGrass->m_dwLastVisibleCount > dwOut)
		{   
		    pGrass->UnPrepareShareGrassVB();
		    SAFE_RELEASE(pGrass);
		    i = pSet->erase(i);
		}
		else
		{
            if (pGrass->m_dwLastVisibleCount == g_dwRenderCount)
            {
                pGrass->AddRef();
                pGrass->PrepareShareGrassVB();
                pRendBlocks->push_back(pGrass);
            }

			i++;
		}
	}
	return S_OK;
}

DWORD KG3DTerrainGrassesRender::GetVisibleGrassBlockCount()
{
	return (DWORD)m_vecGrassRenderBlocks.size();
}