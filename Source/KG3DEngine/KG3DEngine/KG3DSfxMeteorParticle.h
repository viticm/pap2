
////////////////////////////////////////////////////////////////////////////////
//
//  Collator : Chiyer (Zhao XinYu)
//  Date     : 2008-4-28 17:14:16
//
////////////////////////////////////////////////////////////////////////////////


#ifndef _KG3DSFX_METEOR_PARTICLE_H_
#define _KG3DSFX_METEOR_PARTICLE_H_

#include "KG3DSFXParticle.h"

#define PARTICLE_FLAG_KEEP  1

class KG3DSfxMeteorParticle : public KG3DSFXParticle
{
public :

    /* this function will be call when the particle be launch */
    virtual HRESULT OnLaunch(
        KG3DSFX*					 pParentSFX,			// the sfx which this particle pertain to
        KG3DSFXParticleLifeTimeLine* pLifeTimeLine,			// the particle time line
        SFX_PARTICLE_TYPE			 eParticleType,			// particle type
        SFX_PARTICLE_UPDATE_MODE	 eParticleUpdateMode,	// update mode
        SFX_BLEND_MODE				 eBlendMode,			// dx blend mode	 
        DWORD                        dwTrackIndex,          // the track index if sfx
        float						 fLife,					// life
        D3DXVECTOR3*				 pvPosition,			// position
        D3DXVECTOR3*				 pvVelocity,			// velocity
        float						 fFrame,				// launch frame
        DWORD						 dwRandTexDelay,    	// the random texture freq
        KG3DSFXParticleLauncher*     pLauncher
        );


    /* update the particle's time line data */
    virtual HRESULT Update(float fCurrentFrame);


    /* fill the vertex buffer of particle continer */
    virtual HRESULT PrepareVertexBuffer(KG3DSFXParticleContainer *pContainer);

    /* get rectangular number */
    virtual size_t GetRectNum() { return (m_listMeteor.size() <= 1) ? 0 : m_listMeteor.size() - 1; }

    /* get type */
    virtual WORD GetPatricleType() { return PARTICLE_TYPE_METEOR; }


public :

    KG3DSfxMeteorParticle(UINT uIndex);
    virtual ~KG3DSfxMeteorParticle();

public :

    /* the meteor node */
    struct _MeteorNode  
    {
        D3DXVECTOR3 vPosition;
        float       fLife;
    };

private :

    list<_MeteorNode> m_listMeteor; /* the list of meteor */

    float m_fMeteorLife;  /* the life(sfx frame) of each meteor */

};

#endif 