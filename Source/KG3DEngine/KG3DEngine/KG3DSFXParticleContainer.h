////////////////////////////////////////////////////////////////////////////////
//
//  Collator : Chiyer (Zhao XinYu)
//  Date     : 2007-2-12 17:14:16
//  
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DSFXPARTICLECONTAINER_H_
#define _INCLUDE_KG3DSFXPARTICLECONTAINER_H_

#include "KG3DSFXParticle.h"

class KG3DSFX;
class KG3DRenderState;

#define PREALLOCATE_PARTICLE_NUMBER 30

class KG3DSFXParticleContainer
{
    friend class KG3DSFX;

public :

    /* 
      render sfx's praticals, order by blend mode
     */
    HRESULT RenderParticle(const KG3DSFX* pSfx, float fCurrentFrame);
    

    /*
      get current active particle count
     */
    UINT GetParticleCount() const;


    /* 
      push one rectangle to vertex buffer
     */
    HRESULT PushRectangleToVB(
        D3DXVECTOR3*        pVector1,
        D3DXVECTOR3*		pVector2,
        D3DXVECTOR3*		pVector3,
        D3DXVECTOR3*		pVector4,
        D3DCOLOR			dwColor,
        FRECT*				pTexCoord
    );
   

    /* 
      modify the texture number of all particles
     */
    HRESULT SetTextureNum(UINT uTextureNum);


    /* 
     build one new particle
     */
    KG3DSFXParticle* NewParticle(UINT uTexIndex, UINT uBlendMode, WORD wType);


    /* 
      reset the container
     */
    void Reset();


    /* 
      device invalid and restore
     */
    static HRESULT OnStaticInvalidDevice();
    static HRESULT OnStaticRestoreDevice();


public :
    KG3DSFXParticleContainer();
    virtual ~KG3DSFXParticleContainer();


private :

    /*
      fill the vertex buffer by sorted particles
     */
    HRESULT PrepareVertexBuffer(const vector<KG3DSFXParticle*>& vecParticles, size_t uSize, size_t* pRectNum);


    /*
      update the active particles
     */
    HRESULT UpdateParticle(float fCurrentFrame);


    /*
      set the render state by sfx blend mode
     */
    void SetRenderState(KG3DRenderState& R, UINT eBlendMode);


public :

    /*
     the particles use the same texture, in one texture sort order by blend mode,
     there are 4 blend mode : alpha, add, inv, screen
     */
    struct SortedRectParticle {
        std::vector<KG3DSFXParticle*> m_vecRectParticle[SFX_BM_NUM];	
        UINT m_NextFreeIndex[SFX_BM_NUM];
        SortedRectParticle();
        ~SortedRectParticle();
        void Clear();
        KG3DSFXParticle* NewParticle(UINT uBlendMode, WORD wType);
        void UpdateParticle(float fCurrFrame);
    }; 

    std::vector<SortedRectParticle*> m_vecSortedRectParticle;	

    UINT m_uSfxTextNum;

    static LPDIRECT3DVERTEXBUFFER9 m_spVertexBuffer;
    static int m_sVectorNumber;

    VFormat::_Faces_Diffuse_Texture1* m_pRectangleVBData;

    DWORD m_dwActiveNumber;
    UINT  m_uPrimitiveNum;

};

#endif //_INCLUDE_KG3DSFXPARTICLECONTAINER_H_
