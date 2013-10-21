// ***************************************************************
//
//  Description: 
//  -------------------------------------------------------------
//	Author: Zeb
//  -------------------------------------------------------------
//	Create Date: 2006-04-30
//	-------------------------------------------------------------
//	History: 
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
//
// ***************************************************************
#include "StdAfx.h"
#include "KG3DPostRenderEffectBase.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace PostRender
{
    KG3DPostRenderEffectBase::KG3DPostRenderEffectBase(KG3DPostRenderEffectManager* pMgr)
    {
        ASSERT(pMgr);
        m_pPostRenderEffectMgr = pMgr;
        m_bEnable = FALSE;
    }

    KG3DPostRenderEffectBase::~KG3DPostRenderEffectBase()
    {
    }

    HRESULT KG3DPostRenderEffectBase::Enable(BOOL bEnable)
    {
	    m_bEnable = bEnable;
        return S_OK;
    }

    BOOL KG3DPostRenderEffectBase::IsEnable()
    {
        return m_bEnable;
    }
};