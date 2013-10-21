
#ifndef __H_POST_RENDER_EFFECT_BASE__
#define __H_POST_RENDER_EFFECT_BASE__

#define MAX_SAMPLES           16     
#define NUM_LIGHTS            2      
#define EMISSIVE_COEFFICIENT  39.78f                           
#define NUM_TONEMAP_TEXTURES  4       
#define NUM_STAR_TEXTURES     12      
#define NUM_BLOOM_TEXTURES    3    


//  Texture coordinate rectangle 
struct CoordRect
{
    float fLeftU;
    float fTopV;
    float fRightU;
    float fBottomV;
};

//  Screen quad vertex format texture
struct ScreenVertexTex
{
    D3DXVECTOR4 p; 
    D3DXVECTOR2 t; 

    enum {FVF = D3DFVF_XYZRHW | D3DFVF_TEX1};
};

// Screen quad vertex format 2 texture coord
struct ScreenVertex2Tex
{
	D3DXVECTOR4 p; 
	D3DXVECTOR2 t; 
	D3DXVECTOR2 s; 

	enum {FVF = D3DFVF_XYZRHW | D3DFVF_TEX2};
};
//  Screen quad vertex format color
struct ScreenVertexCol
{
    D3DXVECTOR4 p; 
    DWORD       c;

    enum {FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE};
};

class KG3DScene;

namespace PostRender
{
    class KG3DPostRenderEffectManager;

    class KG3DPostRenderEffectBase
    {
    protected:
        BOOL                            m_bEnable;
        KG3DPostRenderEffectManager*    m_pPostRenderEffectMgr;

    public:
        BOOL IsEnable();
	    virtual HRESULT Enable(BOOL bEnable);
	    virtual HRESULT ApplyEffect(KG3DScene* pScene) = 0;
        virtual HRESULT ReloadFX() = 0;

        virtual HRESULT OnLostDevice() = 0;
        virtual HRESULT OnResetDevice() = 0;

	    KG3DPostRenderEffectBase(KG3DPostRenderEffectManager* pMgr);
	    virtual ~KG3DPostRenderEffectBase();
    };
};

#endif