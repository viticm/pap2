////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DPostRenderEffectMisc.cpp
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-12-4 15:11:50
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KG3DPostRenderEffectManager.h"
#include "KG3DPostRenderEffectMisc.h"
#include "KG3DDefaultShaderGroup.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace PostRender {

    KG3DPostRenderEffectMisc::KG3DPostRenderEffectMisc(KG3DPostRenderEffectManager* pManager)
        : KG3DPostRenderEffectBase(pManager)
    {
        m_TechniqueHandel = NULL;
    }

    KG3DPostRenderEffectMisc::~KG3DPostRenderEffectMisc()
    {
    }

    HRESULT KG3DPostRenderEffectMisc::SetTechnique(LPCSTR Name)
    {
        HRESULT hResult  = E_FAIL;
        
		//KG_PROCESS_ERROR(m_pPostRenderEffectMgr->m_pEffect);//<PostRenderShadowMove>
		LPD3DXEFFECT pEffect = g_GetDefaultShaderGroup().GetDefaultEffect(DEFST_POST_RENDER);//<PostRenderShadowMove>
        
        m_TechniqueHandel = pEffect->GetTechniqueByName(Name);
        KG_PROCESS_ERROR(m_TechniqueHandel);

        hResult = S_OK;
Exit0:
        return hResult;
    }

    HRESULT KG3DPostRenderEffectMisc::ApplyEffect(KG3DScene* pScene)
    {
        return E_NOTIMPL;
    }

    HRESULT KG3DPostRenderEffectMisc::ReloadFX()
    {
        return E_NOTIMPL;
    }

}
