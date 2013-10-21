
////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSFXPointLightParticle.cpp
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2009-06-20 16:11:41
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KG3DSFXPointLightParticle.h"
#include "KG3DScene.h"
#include "KG3DScenePointLightRender.h"

KG3DSFXPointLightParticle::KG3DSFXPointLightParticle(UINT uIndex)
    : KG3DSFXParticle(uIndex)
{
}

KG3DSFXPointLightParticle::~KG3DSFXPointLightParticle()
{
}


HRESULT KG3DSFXPointLightParticle::Update(float fCurrentFrame)
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    KG3DScenePointLightRender* pPointLightRender = NULL;

    KG3DScene* pScene = g_cGraphicsTool.GetCurScene();
    KG_PROCESS_ERROR(pScene);

    pPointLightRender = pScene->GetPointLightRender();
    KG_PROCESS_ERROR(pPointLightRender);

    hRetCode = __super::Update(fCurrentFrame);
    KG_COM_PROCESS_ERROR(hRetCode);

    pPointLightRender->AddPointLight(this);

    hResult = S_OK;
Exit0 :
    return hResult;
}