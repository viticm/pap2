// ***************************************************************
//
//  Description: 
//  -------------------------------------------------------------
//	Author: Zeb
//  -------------------------------------------------------------
//	Create Date: 2006-05-30
//	-------------------------------------------------------------
//	History: 
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
//
// ***************************************************************
#ifndef __H_SFX_PARTICLE_COMMANDER__
#define __H_SFX_PARTICLE_COMMANDER__

class KG3DSFXParticleCommander
{
public:
    KG3DSFXParticleCommander(void);
    ~KG3DSFXParticleCommander(void);

//    HRESULT PushParticle(KG3DSFXParticle *pParticle);

private:
    LPDIRECT3DVERTEXBUFFER9     m_lpVB;
};


#endif