////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSFXParticleLifeTimeLine.cpp
//  Version     : 1.0
//  Creator     : Zeb
//  Create Date : 2005-10-8 14:39:11
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KG3DSFXParticleLifeTimeLine.h"



#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

KG3DSFXParticleLifeTimeLine::KG3DSFXParticleLifeTimeLine(void)
{
    m_ColorTimeLineL.InsertKeyFrame(0, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
    m_ColorTimeLineR.InsertKeyFrame(0, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
    m_fWidthTimeLine.InsertKeyFrame(0, 20.f);
    m_fHeightTimeLine.InsertKeyFrame(0, 20.f);
	m_fRotationTimeLineMin.InsertKeyFrame(0, 0.f);
    m_fRotationTimeLineMax.InsertKeyFrame(0, 0.f);
    // for default, we don't use random texture frame idx.
    m_nTextureFrameTimeLine.InsertKeyFrame(0, 0);

    m_uTextureIndex = 0;

    m_fRotationFactor = 0;

    m_wType = 0;
    m_wFlag = 0;

    m_dwSmooth = 100;
}

KG3DSFXParticleLifeTimeLine::~KG3DSFXParticleLifeTimeLine(void)
{
}

void KG3DSFXParticleLifeTimeLine::SetParticleType(const WORD wType)
{
    if (m_wType == wType)
        return;

    m_wType = wType;
}