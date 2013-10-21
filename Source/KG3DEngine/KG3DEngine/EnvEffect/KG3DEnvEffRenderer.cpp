////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DEnvEffRenderer.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-3-31 15:07:06
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "KG3DEnvEffRenderer.h"

#include "KG3DSkySystem.h"
#include "KG3DEnvEffDynamicWeather.h"
#include "KG3DEnvEffFog.h"
#include "KG3DMeshLensFlare.h"
#include "KG3DEnvEffLightSet.h"
#include "KG3DEnvEffLight.h"
#include "KG3DGraphicsTool.h"
#include "KG3DEnvironment.h"
#include "KG3DEngineManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

namespace KG3D_ENVIRONMENT_EFFECT
{
	HRESULT KG3DEnvEffRenderer::Render(KG3DEnvironment& Env, DWORD dwFilter)
	{
		_KG3D_DEBUG_SUPPERSUB_POINTER_EQUAL(KG3DEnvEffect, KG3DSkySystem);
		_KG3D_DEBUG_SUPPERSUB_POINTER_EQUAL(KG3DEnvEffect, KG3DEnvEffLensFlareSun);
		_KG3D_DEBUG_SUPPERSUB_POINTER_EQUAL(KG3DEnvEffect, KG3DEnvEffLensFlareMoon);
#if 1
		KG3DSkySystem* pSkySystem = static_cast<KG3DSkySystem*>(Env.GetEnvEffect(KG3DTYPE_ENVEFF_SKYSYSTEM_SYSTEM, 0));//Env.GetTopEnvEffect<KG3DTYPE_ENVEFF_SKYSYSTEM_SYSTEM>();
		KG3DEnvEffLensFlareSun* pSunFlare = static_cast<KG3DEnvEffLensFlareSun*>(Env.GetEnvEffect(KG3DTYPE_ENVEFF_LENSFLARE_SUN, 0));//Env.GetTopEnvEffect<KG3DTYPE_ENVEFF_LENSFLARE_SUN>();
		KG3DEnvEffLensFlareMoon* pMoonFlare = static_cast<KG3DEnvEffLensFlareMoon*>(Env.GetEnvEffect(KG3DTYPE_ENVEFF_LENSFLARE_MOON, 0));//Env.GetTopEnvEffect<KG3DTYPE_ENVEFF_LENSFLARE_MOON>();
		//1.渲染天气系统

		KG3DEnvEffRenderContext renderContext;
		{
			KG3DCamera* pCamera = g_cGraphicsTool.GetCamera();
			_ASSERTE(NULL != pCamera);
			KG_PROCESS_ERROR(NULL != pCamera);

			D3DXMATRIX matProj;
			const KG3DCameraPersepectiveData& perpData = pCamera->GetPerspectiveData();
			D3DXMatrixPerspectiveFovLH(&matProj, perpData.fFovy, perpData.fAspect, perpData.fzNear, FLT_MAX/2);
			renderContext.matViewProj = pCamera->GetViewMatrix() * matProj;

			renderContext.vCamPos = pCamera->GetPosition();

			KG3DFogParam fogParam = Env.GetFog().GetFogParam();
			renderContext.vFogColor = fogParam.vColor;
		}

		if (pSkySystem && pSkySystem->IsEnable())
		{
			//pSkySystem->RenderSpecial(pSunFlare, pMoonFlare);

			//1.渲染天空盒
			KG3DMeshSkyBox* pSkyBox = pSkySystem->GetSkyBox();

			///天空系统全体成员的设定，注意RenderState是会在析构的时候自动还原RS的，所以要小心生命周期
			GraphicsStruct::RenderState Rs_ZEnable(D3DRS_ZENABLE, D3DZB_TRUE);
			GraphicsStruct::RenderState Rs_ZWriteEnable(D3DRS_ZWRITEENABLE, FALSE);
			GraphicsStruct::RenderState Rs_CullMode(D3DRS_CULLMODE, D3DCULL_CW);
			GraphicsStruct::RenderState Rs_Light(D3DRS_LIGHTING, FALSE);
			GraphicsStruct::RenderState Rs_Fog(D3DRS_FOGENABLE, FALSE);

			///设定Camera的ZFar和World
			GraphicsStruct::RenderStateGlobleCameraZFar Rs_GloableCameraZFar;
			GraphicsStruct::RenderStateWorldMatrixMoveWithCamera Rs_WorldMatrixMoveWithCamera;//让Camera跟着走

			D3DXMATRIX matViewProj = D3DXMatrixGetIdentity();
			{
				D3DXMATRIX matView, matProj;
				g_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
				g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProj);
				matViewProj = matView * matProj;
			}


			if(1)///构造生命域
			{
				KG3DMeshSkyBox::RenderState	SkyBoxRenderState;	///利用构造和析构的关系
				if (pSkyBox && pSkyBox->IsEnable())
				{
					pSkyBox->Render(renderContext);
				}
			}

			//2.在这个地方，渲染LensFlare，
			{
				GraphicsStruct::RenderState Rs_ZEnable2(D3DRS_ZENABLE, D3DZB_FALSE);

				if (pSunFlare && pSunFlare->IsEnable())
				{
					FLOAT fSunDecay = g_cEngineOption.bGodRay && g_cEngineOption.bPostEffectEnable ? 0.5f : 1.0f;
					pSunFlare->RenderFirstFlare(fSunDecay);
				}
				if (pMoonFlare && pMoonFlare->IsEnable())
				{
					pMoonFlare->RenderFirstFlare();
				}
			}

			//打开ZWrite，云和山能阻挡阳光
			

			g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);//这里不能用GraphicStruct
			//GraphicsStruct::RenderState rs_Always(D3DRS_ZFUNC, D3DCMP_ALWAYS);
			g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

			//用于控制Alpha写入
			GraphicsStruct::RenderState RsAlphaTest(D3DRS_ALPHATESTENABLE, TRUE);
			GraphicsStruct::RenderState AlphaRef(D3DRS_ALPHAREF, 0x0000000);
			GraphicsStruct::RenderState RS_AlphaFunc(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

			//3.渲染远山
            if (!(dwFilter & KG3DTYPE_ENVEFF_SKYSYSTEM_FARMOUNTAIN))
			{
				int nFarmountSize = pSkySystem->GetFarMountVecSize();
				//KG3DMeshFarMountain::RenderState	FarMountRenderState;
				for (int i = 0; i < nFarmountSize; ++i)
				{
					KG3DMeshFarMountain* pFarMount = pSkySystem->GetFarMount(i);
					if (pFarMount->IsEnable())
					{
						pFarMount->Render(renderContext);
					}
				}
			}	

			///<DW_SUB_Frame>

			//渲染云
			if(pSkySystem->GetCloudVecSize() > 0)
			{
				KG3DCloud* pFirstCloud = pSkySystem->GetCloud(0);
				_ASSERTE(NULL != pFirstCloud);

				HRESULT hr = pFirstCloud->BeginRender(matViewProj);
				if (SUCCEEDED(hr))
				{
					int nCloudCount = pSkySystem->GetCloudVecSize();

					for(int i = 0; i < nCloudCount; ++i)
					{
						KG3DCloud* p = pSkySystem->GetCloud(i);
						_ASSERTE(NULL != p);
						if (p->IsEnable())
							p->Render(renderContext);
					}

					pFirstCloud->EndRender();
				}
			}		
		}
		else
		{
			GraphicsStruct::RenderState Rs_ZEnable2(D3DRS_ZENABLE, D3DZB_FALSE);
			if (pSunFlare && pSunFlare->IsEnable())
			{
				FLOAT fSunDecay = g_cEngineOption.bGodRay && g_cEngineOption.bPostEffectEnable ? 0.5f : 1.0f;
				pSunFlare->RenderFirstFlare(fSunDecay);
			}
			if (pMoonFlare && pMoonFlare->IsEnable())
			{
				pMoonFlare->RenderFirstFlare();
			}
		}

		/*KG3DEnvEffect* pEnvEffect = Env.m_EnvEffList.ResetIterator();
		while (NULL != pEnvEffect)*/
		KGCH::IteratorHolder itHolder;
		for(KG3DEnvEffect* pEnvEffect = Env.m_EnvEffList.GetFirstTopEffect(itHolder)
			; NULL != pEnvEffect
			; pEnvEffect = Env.m_EnvEffList.GetNextTopEffect(itHolder))
		{
			if (pEnvEffect->IsEnable())
			{
				pEnvEffect->Render(renderContext);
			}
			//pEnvEffect = Env.m_EnvEffList.NextIterator();
		}
#endif
Exit0:
		return S_OK;
	}

	HRESULT KG3DEnvEffRenderer::PostRender( KG3DEnvironment& Env )
	{
#if 1
		/*KG3DEnvEffect* pEnvEffect = Env.m_EnvEffList.ResetIterator();
		while (pEnvEffect)*/
		KGCH::IteratorHolder itHolder;
		for(KG3DEnvEffect* pEnvEffect = Env.m_EnvEffList.GetFirstTopEffect(itHolder)
			; NULL != pEnvEffect
			; pEnvEffect = Env.m_EnvEffList.GetNextTopEffect(itHolder))
		{
			if (pEnvEffect->IsEnable())
			{
				pEnvEffect->PostRender();
			}
			//pEnvEffect = Env.m_EnvEffList.NextIterator();
		}
#endif
		return S_OK;
	}

    const D3DXVECTOR4* KG3DEnvEffRenderer::GetSunPosition(KG3DEnvironment& Env)
    {
        static D3DXVECTOR4 v = D3DXVECTOR4(0.5f, 0.2f, 0.f, 0.f);
		_KG3D_DEBUG_SUPPERSUB_POINTER_EQUAL(KG3DEnvEffect, KG3DEnvEffLensFlareSun);
        KG3DEnvEffLensFlareSun* pSunFlare = static_cast<KG3DEnvEffLensFlareSun*>(Env.GetEnvEffect(KG3DTYPE_ENVEFF_LENSFLARE_SUN, 0));//Env.GetTopEnvEffect<KG3DTYPE_ENVEFF_LENSFLARE_SUN>();
        if (pSunFlare)
            return pSunFlare->GetSunPosition();
        return &v;
    }

    HRESULT KG3DEnvEffRenderer::RenderSun(KG3DEnvironment& Env)
    {
		_KG3D_DEBUG_SUPPERSUB_POINTER_EQUAL(KG3DEnvEffect, KG3DEnvEffLensFlareSun);
        KG3DEnvEffLensFlareSun* pSunFlare = static_cast<KG3DEnvEffLensFlareSun*>(Env.GetEnvEffect(KG3DTYPE_ENVEFF_LENSFLARE_SUN, 0));//Env.GetTopEnvEffect<KG3DTYPE_ENVEFF_LENSFLARE_SUN>();
        if (!pSunFlare)
            return E_FAIL;

		//RenderFirstFlare里面有设这些状态了
       /* GraphicsStruct::RenderState Rs_ZWriteEnable(D3DRS_ZWRITEENABLE, FALSE);
        GraphicsStruct::RenderState Rs_CullMode(D3DRS_CULLMODE, D3DCULL_CW);
        GraphicsStruct::RenderState Rs_Light(D3DRS_LIGHTING, FALSE);
        GraphicsStruct::RenderState Rs_Fog(D3DRS_FOGENABLE, FALSE);
		 GraphicsStruct::RenderState Rs_ZEnable2(D3DRS_ZENABLE, D3DZB_FALSE);
		*/

        ///设定Camera的ZFar和World
        GraphicsStruct::RenderStateGlobleCameraZFar Rs_GloableCameraZFar;
        GraphicsStruct::RenderStateWorldMatrixMoveWithCamera Rs_WorldMatrixMoveWithCamera;//让Camera跟着走

       

        if (pSunFlare && pSunFlare->IsEnable())
        {
            pSunFlare->RenderFirstFlare();
        }

        return S_OK;

    }
};