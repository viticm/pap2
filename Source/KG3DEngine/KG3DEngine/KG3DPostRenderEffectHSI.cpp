////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DPostRenderEffectHSI.cpp
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-12-30 13:03:22
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KG3DPostRenderEffectManager.h"
#include "KG3DPostRenderEffectHSI.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace PostRender {

    KG3DPostRenderEffectHSI::KG3DPostRenderEffectHSI(KG3DPostRenderEffectManager* pManager)
        : KG3DPostRenderEffectBase(pManager)
    {
        m_TechniqueHandel = NULL;
    }

    KG3DPostRenderEffectHSI::~KG3DPostRenderEffectHSI()
    {
    }

    HRESULT KG3DPostRenderEffectHSI::SetTechnique(LPCSTR Name)
    {
        HRESULT hResult  = E_FAIL;
        HRESULT hRetCode = E_FAIL;
        
        KG_PROCESS_ERROR(m_pPostRenderEffectMgr->m_pEffect);
        
        m_TechniqueHandel = m_pPostRenderEffectMgr->m_pEffect->GetTechniqueByName(Name);
        KG_PROCESS_ERROR(m_TechniqueHandel);

        hResult = S_OK;
Exit0:
        return hResult;
    }

    HRESULT KG3DPostRenderEffectHSI::ApplyEffect()
    {
        return E_NOTIMPL;
    }

    HRESULT KG3DPostRenderEffectHSI::ReloadFX()
    {
        return E_NOTIMPL;
    }

}
