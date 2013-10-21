
#ifndef __H_POST_RENDER_SHOCK_WAVE__
#define __H_POST_RENDER_SHOCK_WAVE__

#include "KG3DPostRenderEffectBase.h"

#define SHOCK_WAVE_BUMP_FILE TEXT("Data\\public\\defaultscene\\wavesbump.dds")
#define WATER_SHAFT_FILE TEXT("Data\\public\\defaultscene\\water_shaft.jpg")

class KG3DTexture;

namespace PostRender
{
    class KG3DPostRenderEffectManager;

    class KG3DPostRenderEffectShockWave : public KG3DPostRenderEffectBase
    {
        friend class KG3DPostRenderEffectManager;

    private :
        void CleanUp();

    public :
        KG3DPostRenderEffectShockWave(KG3DPostRenderEffectManager *pMgr);
        virtual ~KG3DPostRenderEffectShockWave(void);

    public :
        virtual HRESULT ReloadFX();
        virtual HRESULT ApplyEffect(KG3DScene* pScene);
        virtual HRESULT Enable(BOOL bEnable);

        virtual HRESULT OnLostDevice();
        virtual HRESULT OnResetDevice();

        LPDIRECT3DTEXTURE9 GetShockWaveTexture();
        LPDIRECT3DTEXTURE9 GetWaveBumpTexture();
        LPDIRECT3DTEXTURE9 GetWaterShaftTexture();

    protected :
        KG3DTexture* m_pBumpTexture;
        KG3DTexture* m_pShaftTexture;
    };
};

#endif