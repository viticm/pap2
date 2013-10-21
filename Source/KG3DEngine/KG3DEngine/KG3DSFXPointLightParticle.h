
////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSFXPointLightParticle.h
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2009-06-20 16:11:41
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KG3DSFXPARTICLE_H_
#define _KG3DSFXPARTICLE_H_

#include "KG3DSFXParticle.h"
#include "KG3DModelPointLight.h"

class KG3DSFXPointLightParticle : public KG3DSFXParticle, public KG3DModelPointLight
{
public :

    /* fill the vertex buffer of particle continer
       because the point light particle has not body, 
       so just return */
    virtual HRESULT PrepareVertexBuffer(KG3DSFXParticleContainer *pContainer) { return S_OK; }

    /* update the particle's time line data */
    virtual HRESULT Update(float fCurrentFrame);

    /* get rectangular number, because the point light
       particle has not body, so just return 0 */
    virtual size_t GetRectNum() { return 0; }

    /* get type */
    virtual WORD GetPatricleType() { return PARTICLE_TYPE_PLIGHT; }

    KG3DSFXPointLightParticle(UINT uIndex);
    virtual ~KG3DSFXPointLightParticle();
};

#endif