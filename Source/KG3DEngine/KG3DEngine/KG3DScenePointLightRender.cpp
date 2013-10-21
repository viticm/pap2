#include "StdAfx.h"
#include "KG3DScenePointLightRender.h"
#include "KG3DModelPointLight.h"
#include "KG3DSceneBlock.h"
#include "KG3DTerrainBlock.h"
#include "KG3DGraphicsTool.h"
#include "KG3DSFX.h"
#include "KG3DModelTable.h"
#include "KG3DTerrainRoad.h"
extern DWORD g_dwRenderCount;
extern BOOL g_bEnablePointLight;

KG3DScenePointLightRender::KG3DScenePointLightRender(void)
{
}

KG3DScenePointLightRender::~KG3DScenePointLightRender(void)
{
}

HRESULT KG3DScenePointLightRender::Init()
{
	return S_OK;
}

HRESULT KG3DScenePointLightRender::UnInit()
{
	set<KG3DModelPointLight*>::iterator i = m_setPointLight.begin();
	set<KG3DModelPointLight*>::iterator iend = m_setPointLight.end();
	while (i!=iend)
	{
		KG3DModelPointLight* pLight = *i;
		SAFE_RELEASE(pLight);
		i++;
	}
	m_setPointLight.clear();

	return S_OK;
}

HRESULT KG3DScenePointLightRender::AddPointLight(KG3DModelPointLight* pLight)
{
	KGLOG_PROCESS_ERROR(pLight);

	if(g_bEnablePointLight)
	{
		set<KG3DModelPointLight*>::iterator i = m_setPointLight.find(pLight);
		if(i==m_setPointLight.end())
		{
			pLight->AddRef();
			m_setPointLight.insert(pLight);
		}
		pLight->SetVisibleCount(g_dwRenderCount);
	}


	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DScenePointLightRender::FrameMove()
{
	set<KG3DModelPointLight*>::iterator i = m_setPointLight.begin();
	set<KG3DModelPointLight*>::iterator iend = m_setPointLight.end();
	while (i!=iend)
	{
		KG3DModelPointLight* pLight = *i;
		
		if(g_dwRenderCount - pLight->GetVisibleCount() > 20)
		{
			SAFE_RELEASE(pLight);
			i = m_setPointLight.erase(i);
		}
		else
			i++;
	}

	return S_OK;
}

HRESULT KG3DScenePointLightRender::Render()
{
    /*
	GraphicsStruct::RenderState zwrite(D3DRS_ZWRITEENABLE,FALSE);
	GraphicsStruct::RenderStateAlpha alpha(0x0,FALSE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_ONE);
    */

/*
	set<KG3DModelPointLight*>::iterator i = m_setPointLight.begin();
	set<KG3DModelPointLight*>::iterator iend = m_setPointLight.end();
	while (i!=iend)
	{
		KG3DModelPointLight* pLight = *i;
        //D3DXVECTOR3 pos = D3DXVECTOR3(pLight->m_matWorld._41, pLight->m_matWorld._42, pLight->m_matWorld._43);

        //m_sfxPointLightFlag->SetTranslation(&pos);
        //m_sfxPointLightFlag->FrameMove();
        //m_sfxPointLightFlag->Render(0, 0);
        
		//pLight->Render(0,NULL);
		//g_cGraphicsTool.DrawAABBox(&pLight->m_BBox,0x80808080);

		i++;
	}
*/
	return S_OK;
}

HRESULT KG3DScenePointLightRender::FindRefrenceLightForTerrainBlock(KG3DTerrainBlock* pBlock,D3DXVECTOR2& A,D3DXVECTOR2& C)
{
	KG3D_PointLight_Info info;
	FindRefrenceLightInRect(info,A,C);
	pBlock->SetPointLightInfo(info);
	return S_OK;
}

HRESULT KG3DScenePointLightRender::FindRefrenceLightForModel(KG3DModel* pModel)
{
	KG3D_PointLight_Info info;

	D3DXVECTOR2 A,C;
	pModel->m_BBox.GetXZRect(A,C);

	FindRefrenceLightInRect(info,A,C);

	pModel->SetPointLightInfo(info);

	return S_OK;
}

HRESULT KG3DScenePointLightRender::FindRefrenceLightForRoad(KG3DTerrainRoad* pRoad)
{
	KG3D_PointLight_Info info;
	D3DXVECTOR2 A,C;
	pRoad->m_BBox.GetXZRect(A,C);
	FindRefrenceLightInRect(info,A,C);
	pRoad->SetPointLightInfo(info);
	return S_OK;
}

namespace {

    bool RectIntersect(const D3DXVECTOR2& A1, const D3DXVECTOR2& B1, const D3DXVECTOR2& A2, const D3DXVECTOR2& B2)
    {
        if ((B2.x < A1.x) || (A2.x > B1.x) || (B2.y < A1.y) || (A2.y > B1.y))
            return false;
        return true;
    }
}

HRESULT KG3DScenePointLightRender::FindRefrenceLightInRect(KG3D_PointLight_Info& info,D3DXVECTOR2& A,D3DXVECTOR2& C)
{
	info.m_nNumPointLights = 0;

	set<KG3DModelPointLight*>::iterator i = m_setPointLight.begin();
	set<KG3DModelPointLight*>::iterator iend = m_setPointLight.end();
	while (i!=iend)
	{
		KG3DModelPointLight* pLight = *i;

        D3DXVECTOR3 vCenter = pLight->m_BBox.GetCenter();
        float fRadius = pLight->m_BBox.GetDiameter() * 0.5f;

        D3DXVECTOR2 vMin = D3DXVECTOR2(vCenter.x - fRadius, vCenter.z - fRadius);
        D3DXVECTOR2 vMax = D3DXVECTOR2(vCenter.x + fRadius, vCenter.z + fRadius);

        if (RectIntersect(A, C, vMin, vMax))
            if (FAILED(info.InputLight(pLight)))
                break;

		/* if(pLight->m_BBox.IsIntersectRect(A,C))
		{
			if(FAILED(info.InputLight(pLight)))
			{
				break;
			}
		}
        */

		i++;
	}

	return S_OK;
}

