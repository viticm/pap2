
//////////////////////////////////////////////////////////////////////////
//
// Trimmer : ZhaoXinyu (Chiyer)
// Time    : 2009-02-18
// Desc    :
//
//////////////////////////////////////////////////////////////////////////

#ifndef _KG3DWaterEffect_H_
#define _KG3DWaterEffect_H_

#include "KG3DInterface.h"
#include "KG3DTemplates.h"
#include "IEEditor.h"

#define WATER_SHADER_FILE TEXT("Data\\public\\shader\\water.fx")

class  KG3DTexture;
class  KG3DSFX;
class  KG3DSceneSceneEditor;
struct KG3DTerrainConverMap;
class  KG3DScene;

enum /* WaterSfxType */
{
    WST_BIG = 0,
    WST_SMALL,

    WST_COUNT
};

class KG3DWaterEffect : public IEKG3DWaterEffect
{
public :
	struct SFX_Container
	{
		list<KG3DSFX*>m_listSFX;
		SFX_Container();
		~SFX_Container();

		HRESULT AddSFX(DWORD dwType,D3DXVECTOR3& vPos);
		HRESULT UnInit();
		HRESULT FrameMove(DWORD& dwCount);
		HRESULT Render();
	};

	struct  SFXControler
	{
		KG3DSFX* m_lpSFX;
		PVOID    m_lpUser;
		DWORD    m_dwLastInWaterTime;
		DWORD    m_dwLastUpdateTime;
		DWORD    m_dwLastAddSfxTime;
		SFX_Container m_SFXOnce;

		D3DXVECTOR3 m_vUserLastPosition;
		float       m_fUserSpeedY;
		float       m_fUserSpeedXZ;

		SFXControler()
		{
			m_lpSFX = NULL;
			m_lpUser = NULL;
			m_dwLastInWaterTime = 0;
			m_SFXOnce.UnInit();
			m_fUserSpeedY = 0;
			m_fUserSpeedXZ = 0;
			m_vUserLastPosition = D3DXVECTOR3(0,0,0);
			m_dwLastUpdateTime = 0;
			m_dwLastAddSfxTime = 0;
		}

		HRESULT UnInit()
		{
			m_lpSFX = NULL;
			m_lpUser = NULL;
			m_dwLastInWaterTime = NULL;
			m_SFXOnce.UnInit();
			m_fUserSpeedY = 0;
			m_fUserSpeedXZ = 0;
			m_vUserLastPosition = D3DXVECTOR3(0,0,0);
			m_dwLastUpdateTime = 0;
			m_dwLastAddSfxTime = 0;

			return S_OK;
		}
	};

	TKG3DResourcePool<SFXControler>m_SFXConPool;
	map<PVOID, SFXControler*> m_mapWaveSFX;
	DWORD m_dwLastActiveTime;

    //LPD3DXEFFECT        m_lpEffectWater;
	KG3DTexture*        m_lpWaterBumpTexture1;
	KG3DTexture*        m_lpWaterBumpTexture2;
	KG3DTexture*        m_lpWaterSurfaceTexture;
	KG3DTexture*        m_lpWaterSprayTexure;
	LPDIRECT3DTEXTURE9 m_lpReflectTexture;
	LPDIRECT3DTEXTURE9 m_lpRefractTexture;
	LPDIRECT3DTEXTURE9 m_lpWaveTexture;

    HRESULT PrepareTexture(int nDetail);

	HRESULT Init();
	HRESULT UnInit();

	HRESULT OnLostDevice();
	HRESULT OnResetDevice();

	HRESULT FrameMove();

	HRESULT BeginEffectForRiver(KG3DSceneSceneEditor* pScene, BOOL bUnderWater);
    HRESULT BeginEffect(KG3DSceneSceneEditor* pScene, BOOL bUnderWater = FALSE);
    HRESULT EndEffect();

	HRESULT SaveToFile(IIniFile* pIniFile);
	HRESULT LoadFromFile(IIniFile* pIniFile);

    HRESULT DeleteAllSFX();
    HRESULT DeleteSFX(PVOID pUser);

protected :
    bool       m_bEffectBegin;
    bool       m_bPassBegin;

public :
    /*
        m_vWaveParam
        x : the twisting coefficient of refract
        y : the fresnel coefficient
        z : the water velocity of U
        w : the water velocity of V
     */
    D3DXVECTOR4 m_vWaveParam;
    /*
        m_vTextParam
        x : the UV scaling of water texture
        y : the UV scaling of bump texture
        z : the twisting coefficient of reflect
        w : the decay of refract
    */
    D3DXVECTOR4 m_vTextParam;
    D3DXVECTOR4 m_vWaveParam2;
    D3DXCOLOR   m_WaterColor[WD_COUNT];
    D3DXCOLOR   m_WaterSpecrColor;

	KG3DFogParamShader	m_waterFogParam;
	
    //D3DXCOLOR   m_WaterFogColor;
    //float       m_fWaterFogStart;
    //float       m_fWaterFogEnd;

    D3DXVECTOR4 m_vWaveParamUse;
    int         m_nDetail;
    TCHAR       m_szSurfaceTextPath[MAX_PATH];
    TCHAR       m_szBump1TextPath[MAX_PATH];

public:

    virtual LPCTSTR GetWaterSurfaceTextureName();
    virtual LPCTSTR GetWaterBumpTextureName();

    virtual HRESULT SetWaterSurfaceTexture(LPCTSTR szFileName);
    virtual HRESULT SetWaterBumpTexture(LPCTSTR szFileName);

    virtual HRESULT ReloadEffect();

    virtual D3DXVECTOR4* GetTextParamPtr()  { return &m_vTextParam;  }
    virtual D3DXVECTOR4* GetWaveParamPtr()  { return &m_vWaveParam;  }
    virtual D3DXVECTOR4* GetWaveParam2Ptr() { return &m_vWaveParam2; }

    virtual D3DXCOLOR* GetWaterColorPtr(int nDetail)  { return &m_WaterColor[nDetail]; }
    virtual D3DXCOLOR* GetSpecColorPtr()   { return &m_WaterSpecrColor; }
    virtual D3DXCOLOR* GetFogColorPtr()    { return &/*m_WaterFogColor*/m_waterFogParam.vColor; }

    virtual HRESULT SetFogInfo(float fStart, float fEnd);
    virtual HRESULT GetFogInfo(float& fStart, float& fEnd);

	KG3DWaterEffect();
	~KG3DWaterEffect();

	HRESULT CheckEntityAndWater(PVOID pUser,D3DXVECTOR3& vPosition,float fHeight,KG3DSceneSceneEditor* pEditor);
	HRESULT RenderWaveSFX(BOOL bWave,BOOL bOnce);

};

#endif
