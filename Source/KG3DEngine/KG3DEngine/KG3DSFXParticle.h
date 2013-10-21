////////////////////////////////////////////////////////////////////////////////
//
//  Collator : Chiyer (Zhao XinYu)
//  Date     : 2007-2-12 17:14:16
//
////////////////////////////////////////////////////////////////////////////////


#ifndef _INCLUDE_KG3DSFXPARTICLE_H_
#define _INCLUDE_KG3DSFXPARTICLE_H_

#include "KG3DSFXParticleLifeTimeLine.h"
#include "KG3DSFXParticleForceField.h"
#include "KG3DSFXTrack.h"

class KG3DSFXParticleContainer;
class KG3DSFXParticleLauncher;
class KG3DSFX;

class KG3DSFXParticle
{
    friend class KG3DSFXParticleContainer;

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
    virtual HRESULT PrepareVertexBuffer(KG3DSFXParticleContainer *pContainer );


    /* get the particle time line, color, size, etc */
    KG3DSFXParticleLifeTimeLine* GetLifeTimeLine() const { return m_pLifeTimeLine; };

    /* get rectangular particle info */
    HRESULT GetRectInfo(            
        D3DXVECTOR3* pv1,             /* vector 1 */
        D3DXVECTOR3* pv2,             /* vector 2 */
        D3DXVECTOR3* pv3,             /* vector 3 */
        D3DXVECTOR3* pv4,             /* vector 4 */
        FRECT*       pRectfText,      /* texture rect */
        const D3DXVECTOR3& vWorldPos  /* world position */
        ); 

    /* get the world position by local position and coordate type */
    void GetWorldPos(D3DXVECTOR3* pWorldPos, const D3DXVECTOR3& vLocalPos);

    /* get rectangular number */
    virtual size_t GetRectNum()    { return 1; }

    /* get type */
    virtual WORD GetPatricleType() { return PARTICLE_TYPE_NORMAL; }


public :
    KG3DSFXParticle(UINT uIndex);
    virtual ~KG3DSFXParticle();

public :
    SFX_PARTICLE_UPDATE_MODE    m_eParticleUpdateMode;
    SFX_PARTICLE_TYPE           m_eParticleType;
    SFX_BLEND_MODE              m_eBlendMode;
    KG3DSFXParticleLauncher*    m_pLauncher;
    
    D3DXVECTOR3     m_vPositionWorld;
    D3DXVECTOR3     m_vPosition;
    float           m_fWidth;
    float           m_fHeight;
	float			m_fRotation;
    float			m_fRotationStep;
    D3DCOLOR        m_dwColor;
    float           m_fLife;
    float           m_fPreFrame;
    float           m_fStartFrame;
    D3DXVECTOR3     m_vPrevPosition;


    /* If the particle has been bind to track, then m_vVelocity.x means
       particle's speed, and m_vVelocity.y means the distance which the particle
       has moved. In fact, the distance will be passed as current frame to track
       to get a transformation later. */
    D3DXVECTOR3     m_vPreVelocity;
    D3DXVECTOR3     m_vVelocity;
    float           m_fVelocity;

    KG3DSFXParticleLifeTimeLine* m_pLifeTimeLine;
    KG3DSFX*					 m_pParentSFX;
    UINT						 m_uPoolIndex;
    DWORD                        m_dwTrackIndex;

    float						 m_fRotationX;
    float						 m_fRotationY;
    float						 m_fRotationZ;
    D3DXVECTOR3                  m_vRotStep;

    DWORD						 m_dwRandomTextureFrame;

	/* random */ 

	DWORD	m_dwRandTexDelay;
	DWORD	m_dwLastRandTime;
    DWORD	m_dwRandColDelay;
    DWORD	m_dwLastRandColTime;
    DWORD   m_dwRandRotDelay;
    DWORD   m_dwLastRandRotTime;
    DWORD   m_dwRandAlpDelay;
    DWORD   m_dwLastRandAlpTime;
    DWORD   m_dwRandFcRotDelay;
    DWORD   m_dwLastRandFcRotTime;
    float   m_fLastRorceFactor;
};

#endif //_INCLUDE_KG3DSFXPARTICLE_H_
