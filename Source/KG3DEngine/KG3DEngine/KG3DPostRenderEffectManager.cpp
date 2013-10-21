
#include "StdAfx.h"
#include "KG3DPostRenderEffectManager.h"
#include "KG3DPostRenderEffectBase.h"
#include "KG3DPostRenderEffectHDR.h"
#include "KG3DPostRenderEffectBloom.h"
#include "KG3DPostRenderEffectShockWave.h"
#include "KG3DRenderState.h"
#include "KG3DCamera.h"
#include "KG3DGraphicsTool.h"
#include "KG3DGraphiceEngine.h"
#include "KG3DEngineManager.h"
#include "KG3DSceneSceneEditor.h"
#include "KG3DEnvironment.h"
#include "KG3DSFXProjectionTextureMgr.h"
#include "KG3DWaterEffect.h"
#include "KG3DDefaultShaderGroup.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern KG3DGraphicsEngine      g_cGraphicsEngine;
extern BOOL g_bEnableScaleOutput;//是否使用缩放输出，即渲染使用1024然后拉伸输出到屏幕
extern int  g_nScaleOutputSize ;

namespace PostRender
{
    void DrawFullScreenQuad(
        float U1, float V1, float U2, float V2, float fX, float fY, 
        float fWidth, float fHeight, DWORD dwEnableAlpha, float z
        )
    {
        KG3DRenderState RenderState;

        ScreenVertexTex svQuad[4];

        svQuad[0].p = D3DXVECTOR4(fX - 0.5f, fY - 0.5f, z, 1.0f);
        svQuad[0].t = D3DXVECTOR2(U1, V1);

        svQuad[1].p = D3DXVECTOR4(fX + fWidth - 0.5f, fY - 0.5f, z, 1.0f);
        svQuad[1].t = D3DXVECTOR2(U2, V1);

        svQuad[2].p = D3DXVECTOR4(fX - 0.5f, fY + fHeight - 0.5f, z, 1.0f);
        svQuad[2].t = D3DXVECTOR2(U1, V2);

        svQuad[3].p = D3DXVECTOR4(fX + fWidth - 0.5f, fY + fHeight - 0.5f, z, 1.0f);
        svQuad[3].t = D3DXVECTOR2(U2, V2);

        RenderState.SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
        RenderState.SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
        RenderState.SetRenderState(D3DRS_LIGHTING, FALSE);

        if (dwEnableAlpha)
        {
            RenderState.SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
			RenderState.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
            RenderState.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
            RenderState.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
            RenderState.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
            RenderState.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
            g_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
        }
        else
        {
            RenderState.SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
			RenderState.SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
        }
        RenderState.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
        RenderState.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

        g_pd3dDevice->SetFVF(ScreenVertexTex::FVF);
        g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, svQuad, sizeof(ScreenVertexTex));

    }

    void GetSampleOffsets_DownScale2x2_Lum(DWORD dwSrcWidth, DWORD dwSrcHeight, DWORD dwDestWidth, DWORD dwDestHeight, D3DXVECTOR2 avSampleOffsets[])
    {
        ASSERT(avSampleOffsets);

        float tU = 1.0f / dwSrcWidth;
        float tV = 1.0f / dwSrcHeight;

        float deltaU = (float) dwSrcWidth / dwDestWidth / 2.0f;
        float deltaV = (float) dwSrcHeight / dwDestHeight / 2.0f;

        int index = 0;
        for (int y = 0; y < 2; y++)
        {
            for (int x = 0; x < 2; x++)
            {
                avSampleOffsets[index].x = (x - 0.5f) * deltaU * tU;
                avSampleOffsets[index].y = (y - 0.5f) * deltaV * tV;

                index++;
            }
        }
    }

    void GetSampleOffsets_DownScale3x3(DWORD dwWidth, DWORD dwHeight, D3DXVECTOR2 avSampleOffsets[])
    {
        ASSERT(avSampleOffsets);

        float tU = 1.0f / dwWidth;
        float tV = 1.0f / dwHeight;

        int index = 0;
        for (int y = -1; y <= 1; y++)
        {
            for (int x = -1; x <= 1; x++)
            {
                avSampleOffsets[index].x = x * tU;
                avSampleOffsets[index].y = y * tV;

                index++;
            }
        }
    }

    inline float GaussianDistribution(float x, float y, float rho)
    {
        float g = 1.0f / sqrtf(2.0f * D3DX_PI * rho * rho);
        g *= expf(-(x * x + y * y) / (2 * rho * rho));

        return g;
    }

    void GetSampleOffsets_Bloom(
        DWORD dwD3DTexSize,
        float afTexCoordOffset[16],
        D3DXVECTOR4* avColorWeight,
        float fDeviation,
        float fMultiplier,
        int num
        )
    {
        ASSERT(afTexCoordOffset);
        ASSERT(avColorWeight);
        ASSERT(num > 0);

        int i = 0;
        float tu = 1.0f / (float)dwD3DTexSize;
        int first = 0;
        int last = num - 1;
        int mid = num / 2;

        float weight = fMultiplier * GaussianDistribution(0, 0, fDeviation);
        avColorWeight[first] = D3DXVECTOR4(weight, weight, weight, 1.0f);
        afTexCoordOffset[first] = 0.0f;
        avColorWeight[last] = D3DXVECTOR4(weight, weight, weight, 1.0f);
        afTexCoordOffset[last] = 0.0f;

        for (i = 1; i < mid; i++)
        {
            weight = fMultiplier * GaussianDistribution((float)i, 0, fDeviation);
            avColorWeight[i] = D3DXVECTOR4(weight, weight, weight, 1.0f);
            afTexCoordOffset[i] = i * tu;
        }

        for (i = mid; i < last; i++)
        {
            avColorWeight[i] = avColorWeight[i - mid + 1];
            afTexCoordOffset[i] = -afTexCoordOffset[i - mid + 1];
        }
    }

    float random(float s, float d)
    {
        float k = (float)rand() / (float)RAND_MAX;
        return s + k * (d - s);
    }

    int randsgn() 
    {
        return (rand() > (RAND_MAX / 2)) ? 1 : -1;
    }

    vector<KG3DPostRenderEffectManager::_ColorCurve> KG3DPostRenderEffectManager::m_svCurveStype;
    PostRenderParams KG3DPostRenderEffectManager::m_sEffectParams;
    
    KG3DPostRenderEffectManager::KG3DPostRenderEffectManager()
        : m_lpRenderTargetTexture(NULL),
          m_lpRenderTargetSave(NULL),
          m_lpDepthTexture(NULL),
          m_lpDepthTextureSave(NULL),
          m_lpShareRenderTarger(NULL),
		  m_lpMultiSampleSurface(NULL),
		  m_lpMultiSampleDepth(NULL),
		  m_lpNoMultiSampleDepth(NULL),
          m_lpCurveTexture(NULL),
          m_lpSwapTexture(NULL),
          m_dwPostRenderWidth(0),
          m_dwPostRenderHeight(0),
          m_BloomEffect(this),
          m_DetailEffect(this),
          m_ShadowEffect(this),
          m_GodRayEffect(this),
          m_ShockWareEffect(this),
          m_SSAOEffect(this),
          m_AoeEffect(this),
		  m_DOFEffect(this),
		  m_HDREffect(this),
//          m_pEffect(NULL),//<PostRenderShadowMove>
          m_bSaveToFile(FALSE),
          m_bEnable(FALSE),
          m_fTranDir(0.f),
          m_fRate(0.f),
          m_fExposure(0.5f),
          m_fExposureSave(0.5f),
          m_fAngel(0.f),
          m_dwAlphaEnable(FALSE),
          m_dwCurrType(0),          
          m_dwPrevTime(g_cGraphicsTool.GetNowTime()),
          m_pFinalPassTarget(NULL),
          m_pFinalPassTexture(NULL),
          m_fScalw(1.f),
          m_fScalh(1.f),
          m_fShockPower(0.f),
          m_nShockPowerSgn(1),
          m_bTargetRestore(TRUE),
          m_nActiveStype(-1)
    {
        memset(&m_ViewportSave, 0, sizeof(m_ViewportSave));
		m_lpParentScene = NULL;
    }

    KG3DPostRenderEffectManager::~KG3DPostRenderEffectManager()
    {
	    SAFE_RELEASE(m_lpMultiSampleDepth);
		SAFE_RELEASE(m_lpMultiSampleSurface);
		SAFE_RELEASE(m_lpNoMultiSampleDepth);
		SAFE_RELEASE(m_lpRenderTargetTexture);
        //SAFE_RELEASE(m_pEffect);//<PostRenderShadowMove>
        SAFE_RELEASE(m_lpShareRenderTarger);
        SAFE_RELEASE(m_lpDepthTexture);
        SAFE_RELEASE(m_lpSwapTexture);
        SAFE_RELEASE(m_lpCurveTexture);
		
	    m_lpRenderTargetSave = NULL;
        m_lpDepthTextureSave = NULL;

    }

    HRESULT KG3DPostRenderEffectManager::StaticConfigLoad()
    {
        memset(&m_sEffectParams, 0, sizeof(m_sEffectParams));

        m_sEffectParams.fBloomExposure1          = 1.f;   
        m_sEffectParams.fBloomExposure2          = 0.3F;      
        m_sEffectParams.fBloomDelay              = 1000;      
        m_sEffectParams.fBloomGate               = 0.03F;
        m_sEffectParams.fBloomGaussCoefficient   = 10;
        m_sEffectParams.nBloomSampleNum          = 8;

        m_sEffectParams.fDetailPower             = 1.f;
        m_sEffectParams.fBlurPower               = 0.f;
        m_sEffectParams.fBlurDistance            = 20000.f;

        m_sEffectParams.fHueScale                = 0.68F;   
        m_sEffectParams.fHueBias                 = -0.04F;
        m_sEffectParams.fSaturationScale         = 1.f;           
        m_sEffectParams.fSaturationBias          = 0.09f;      
        m_sEffectParams.fIntensityScale          = 0.67f;   
        m_sEffectParams.fIntensityBias           = 0.16f;
        m_sEffectParams.fShockWavePower          = 0.11f;

        m_sEffectParams.nGodRaySampleNum         = 3;
        m_sEffectParams.fGodRayDensity           = 0.8f;
        m_sEffectParams.fGodRayWeight            = 0.58f;
        m_sEffectParams.fGodRayDecay             = 0.8f;
        m_sEffectParams.fGodRayExposure          = 0.29f;
        m_sEffectParams.fLimitAngle              = D3DX_PI * 2;

        m_sEffectParams.nSSAOLevel               = SSAO_LEVEL_NORMAL;
        m_sEffectParams.fSSAOSampleRadius        = 20.f;
        m_sEffectParams.fSSAODarkFact            = 0.1f;
        m_sEffectParams.fBlurWeight              = 0.5f;

        m_sEffectParams.fFogDensityV             = 0.001f;
        m_sEffectParams.fSlopeThreshold          = 0.1f;
        m_sEffectParams.fHeightFalloff           = 0.004f;
        m_sEffectParams.vecFogcolor              = D3DXVECTOR4(0.7f,0.7f,0.7f,1.0f);

        m_sEffectParams.nCurveStype              = -1;

        m_sEffectParams.fSoftness                = 2.0f;
        m_sEffectParams.fJitter                  = 1.0f;

		m_sEffectParams.fHDRBrightGate           = 0.1f;
		m_sEffectParams.fHDRRectify              = 0.8f;
		m_sEffectParams.fHDRGrayLevel            = 0.15f;
		m_sEffectParams.nHDRStarType             = 4;
		m_sEffectParams.bHDREnableBlueShift      = FALSE;
		m_sEffectParams.fHDRBloomScale           = 1.0f;
		m_sEffectParams.fHDRStarScale            = 0.5f;


        for (int i = 0; i <= 255; ++i)
        {
            m_sEffectParams.CurveRed[i] = (BYTE)i;
            m_sEffectParams.CurveGreen[i] = (BYTE)i;
            m_sEffectParams.CurveBlue[i] = (BYTE)i;
        }

        LoadParams("data\\public\\post_render_params.ini");
        LoadCurveStype();

        return S_OK;
    }

    HRESULT KG3DPostRenderEffectManager::StaticConfigSave()
    {
        SaveParams("data\\public\\post_render_params.ini");
        SaveCurveStype();

        return S_OK;
    }



    HRESULT KG3DPostRenderEffectManager::LoadParams(LPCSTR pszFileName)
    {
        HRESULT hResult  = E_FAIL;

        IIniFile* pIniFile = g_OpenIniFile(pszFileName);
        KG_PROCESS_ERROR(pIniFile);

        pIniFile->GetFloat("Bloom", "fBloomExposure1", 1.0f, &m_sEffectParams.fBloomExposure1);
        pIniFile->GetFloat("Bloom", "fBloomExposure2", 0.3f, &m_sEffectParams.fBloomExposure2);
        pIniFile->GetFloat("Bloom", "BloomDelay", 300, &m_sEffectParams.fBloomDelay);
        pIniFile->GetFloat("Bloom", "BloomGate", 0.2F, &m_sEffectParams.fBloomGate);
        pIniFile->GetFloat("Bloom", "fBloomGaussCoefficient", 3.0f, &m_sEffectParams.fBloomGaussCoefficient);
        pIniFile->GetInteger("Bloom", "nBloomSampleNum", 8, &m_sEffectParams.nBloomSampleNum);

        pIniFile->GetFloat("Detail", "fDetailPower", 1.f, &m_sEffectParams.fDetailPower);
        pIniFile->GetFloat("Detail", "fBlurPower", 0.f, &m_sEffectParams.fBlurPower);
        pIniFile->GetFloat("Detail", "fBlurDistance", 20000.f, &m_sEffectParams.fBlurDistance);

        pIniFile->GetFloat("HSI", "HueScale", 1.f, &m_sEffectParams.fHueScale);
        pIniFile->GetFloat("HSI", "HueBias", 0.f, &m_sEffectParams.fHueBias);
        pIniFile->GetFloat("HSI", "SaturationScale", 1.f, &m_sEffectParams.fSaturationScale);
        pIniFile->GetFloat("HSI", "SaturationBias", 0.f, &m_sEffectParams.fSaturationBias);
        pIniFile->GetFloat("HSI", "fIntensityScale", 1.f, &m_sEffectParams.fIntensityScale);
        pIniFile->GetFloat("HSI", "IntensityBias", 0.f, &m_sEffectParams.fIntensityBias);

        pIniFile->GetFloat("ShockWave", "ShockWavePower", 0.01f, &m_sEffectParams.fShockWavePower);

        pIniFile->GetInteger("GodRay", "nGodRaySampleNum", 3, &m_sEffectParams.nGodRaySampleNum);
        pIniFile->GetFloat("GodRay", "fGodRayDensity", 0.38f, &m_sEffectParams.fGodRayDensity);
        pIniFile->GetFloat("GodRay", "fGodRayWeight", 0.1f, &m_sEffectParams.fGodRayWeight);
        pIniFile->GetFloat("GodRay", "fGodRayDecay", 1.f, &m_sEffectParams.fGodRayDecay);
        pIniFile->GetFloat("GodRay", "fGodRayExposure", 0.27f, &m_sEffectParams.fGodRayExposure);
        pIniFile->GetFloat("GodRay", "fLimitAngle", D3DX_PI * 2, &m_sEffectParams.fLimitAngle);

        pIniFile->GetInteger("SSAO", "nSSAOLevel", 1, &m_sEffectParams.nSSAOLevel);
        pIniFile->GetFloat("SSAO", "fSSAOSampleRadius", 20.f, &m_sEffectParams.fSSAOSampleRadius);
        pIniFile->GetFloat("SSAO", "fSSAODarkFact", 0.5f, &m_sEffectParams.fSSAODarkFact);
        pIniFile->GetFloat("SSAO", "fBlurWeight", 0.5f, &m_sEffectParams.fBlurWeight);

        pIniFile->GetFloat("Fog", "fFogDensityV", 0.001f, &m_sEffectParams.fFogDensityV);
        pIniFile->GetFloat("Fog", "fSlopeThreshold", 0.1f, &m_sEffectParams.fSlopeThreshold);
        pIniFile->GetFloat("Fog", "fHeightFalloff", 0.004f, &m_sEffectParams.fHeightFalloff);
        pIniFile->GetMultiFloat("Fog", "vecFogcolor",(float*)(&m_sEffectParams.vecFogcolor),4);

        pIniFile->GetFloat("shadow", "Softness", 2.0f, &m_sEffectParams.fSoftness);
        pIniFile->GetFloat("shadow", "Jitter", 1.0f, &m_sEffectParams.fJitter);
        pIniFile->GetInteger("shadow", "bEnabledAreaShadow", FALSE, &m_sEffectParams.bEnableAreaShadow);

		pIniFile->GetFloat("HDR", "BrightGate", 0.1f, &m_sEffectParams.fHDRBrightGate);
		pIniFile->GetFloat("HDR", "Rectify", 0.8f, &m_sEffectParams.fHDRRectify);
		pIniFile->GetFloat("HDR", "GrayLevel", 0.15f, &m_sEffectParams.fHDRGrayLevel);
		pIniFile->GetInteger("HDR", "StarType", 4, &m_sEffectParams.nHDRStarType);
		pIniFile->GetInteger("HDR", "EnableBlueShift", FALSE, &m_sEffectParams.bHDREnableBlueShift);
		pIniFile->GetFloat("HDR", "BloomScale", 1.0f, &m_sEffectParams.fHDRBloomScale);
		pIniFile->GetFloat("HDR", "StarScale", 0.5f, &m_sEffectParams.fHDRStarScale);

        hResult = S_OK;
Exit0 :
        SAFE_RELEASE(pIniFile);
        return hResult;
    }

    HRESULT KG3DPostRenderEffectManager::SaveParams(LPCSTR pszFileName)
    {
        HRESULT hResult  = E_FAIL;

        IIniFile* pIniFile = g_OpenIniFile(pszFileName, false, true);
        KG_PROCESS_ERROR(pIniFile);

        pIniFile->WriteFloat("Bloom", "fBloomExposure1", m_sEffectParams.fBloomExposure1);
        pIniFile->WriteFloat("Bloom", "fBloomExposure2", m_sEffectParams.fBloomExposure2);
        pIniFile->WriteFloat("Bloom", "BloomDelay", m_sEffectParams.fBloomDelay);
        pIniFile->WriteFloat("Bloom", "BloomGate", m_sEffectParams.fBloomGate);
        pIniFile->WriteFloat("Bloom", "fBloomGaussCoefficient", m_sEffectParams.fBloomGaussCoefficient);
        pIniFile->WriteInteger("Bloom", "nBloomSampleNum", m_sEffectParams.nBloomSampleNum);

        pIniFile->WriteFloat("Detail", "fDetailPower", m_sEffectParams.fDetailPower);
        pIniFile->WriteFloat("Detail", "fBlurPower", m_sEffectParams.fBlurPower);
        pIniFile->WriteFloat("Detail", "fBlurDistance", m_sEffectParams.fBlurDistance);

        pIniFile->WriteFloat("HSI","HueScale", m_sEffectParams.fHueScale);
        pIniFile->WriteFloat("HSI","HueBias", m_sEffectParams.fHueBias);
        pIniFile->WriteFloat("HSI","SaturationScale", m_sEffectParams.fSaturationScale);
        pIniFile->WriteFloat("HSI","SaturationBias", m_sEffectParams.fSaturationBias);
        pIniFile->WriteFloat("HSI","fIntensityScale", m_sEffectParams.fIntensityScale);
        pIniFile->WriteFloat("HSI","IntensityBias", m_sEffectParams.fIntensityBias);

        pIniFile->WriteFloat("ShockWave", "ShockWavePower", m_sEffectParams.fShockWavePower);

        pIniFile->WriteInteger("GodRay", "nGodRaySampleNum", m_sEffectParams.nGodRaySampleNum);
        pIniFile->WriteFloat("GodRay", "fGodRayDensity", m_sEffectParams.fGodRayDensity);
        pIniFile->WriteFloat("GodRay", "fGodRayWeight", m_sEffectParams.fGodRayWeight);
        pIniFile->WriteFloat("GodRay", "fGodRayDecay", m_sEffectParams.fGodRayDecay);
        pIniFile->WriteFloat("GodRay", "fGodRayExposure", m_sEffectParams.fGodRayExposure);
        pIniFile->WriteFloat("GodRay", "fLimitAngle", m_sEffectParams.fLimitAngle);

        pIniFile->WriteInteger("SSAO", "nSSAOLevel", m_sEffectParams.nSSAOLevel);
        pIniFile->WriteFloat("SSAO", "fSSAOSampleRadius", m_sEffectParams.fSSAOSampleRadius);
        pIniFile->WriteFloat("SSAO", "fSSAODarkFact", m_sEffectParams.fSSAODarkFact);
        pIniFile->WriteFloat("SSAO", "fBlurWeight", m_sEffectParams.fBlurWeight);

        pIniFile->WriteFloat("Fog", "fFogDensityV",m_sEffectParams.fFogDensityV);
        pIniFile->WriteFloat("Fog", "fSlopeThreshold",m_sEffectParams.fSlopeThreshold);
        pIniFile->WriteFloat("Fog", "fHeightFalloff", m_sEffectParams.fHeightFalloff);
        pIniFile->WriteMultiFloat("Fog", "vecFogcolor",(float*)(&m_sEffectParams.vecFogcolor),4);

        pIniFile->WriteInteger("shadow", "bEnabledAreaShadow", m_sEffectParams.bEnableAreaShadow);
		pIniFile->WriteFloat("shadow", "Jitter",m_sEffectParams.fJitter);
        pIniFile->WriteFloat("shadow", "Softness", m_sEffectParams.fSoftness);

		pIniFile->WriteFloat("HDR", "BrightGate", m_sEffectParams.fHDRBrightGate);
		pIniFile->WriteFloat("HDR", "Rectify", m_sEffectParams.fHDRRectify);
		pIniFile->WriteFloat("HDR", "GrayLevel",m_sEffectParams.fHDRGrayLevel);
		pIniFile->WriteInteger("HDR", "StarType", m_sEffectParams.nHDRStarType);
		pIniFile->WriteInteger("HDR", "EnableBlueShift", m_sEffectParams.bHDREnableBlueShift);
		pIniFile->WriteFloat("HDR", "BloomScale", m_sEffectParams.fHDRBloomScale);
		pIniFile->WriteFloat("HDR", "StarScale", m_sEffectParams.fHDRStarScale);


        pIniFile->Save(pszFileName);

        hResult = S_OK;
Exit0 :
        SAFE_RELEASE(pIniFile);
        return hResult;
    }

    HRESULT KG3DPostRenderEffectManager::LoadCurveStype()
    {
        m_svCurveStype.clear();

        IFile* pFile = g_OpenFile("data\\public\\color_curve.ccs");

        if (!pFile)
        {
            _ColorCurve curve;

            strcpy(curve.szName, "Default Stype");
            curve.vRed.push_back(D3DXVECTOR2(0.f, 0.f));
            curve.vRed.push_back(D3DXVECTOR2(1.f, 1.f));
            curve.vGreen.push_back(D3DXVECTOR2(0.f, 0.f));
            curve.vGreen.push_back(D3DXVECTOR2(1.f, 1.f));
            curve.vBlue.push_back(D3DXVECTOR2(0.f, 0.f));
            curve.vBlue.push_back(D3DXVECTOR2(1.f, 1.f));

            curve.fKC = 0.f;
            curve.fKM = 0.f;
            curve.fKY = 0.f;

            m_svCurveStype.push_back(curve);

            return S_OK;
        }

        int cnt = 0;
        
        pFile->Read(&cnt, sizeof(cnt));
        m_svCurveStype.resize(cnt);

        for (int i = 0; i < cnt; ++i)
        {
            _ColorCurve& curve = m_svCurveStype[i];
            int ptNum = 0;

            pFile->Read(curve.szName, sizeof(curve.szName));
            pFile->Read(&ptNum, sizeof(ptNum));

            for (int v = 0; v < ptNum; ++v)
            {
                D3DXVECTOR2 pt;
                pFile->Read(&pt, sizeof(pt));
                curve.vRed.push_back(pt);
            }

            pFile->Read(&ptNum, sizeof(ptNum));

            for (int v = 0; v < ptNum; ++v)
            {
                D3DXVECTOR2 pt;
                pFile->Read(&pt, sizeof(pt));
                curve.vGreen.push_back(pt);
            }

            pFile->Read(&ptNum, sizeof(ptNum));

            for (int v = 0; v < ptNum; ++v)
            {
                D3DXVECTOR2 pt;
                pFile->Read(&pt, sizeof(pt));
                curve.vBlue.push_back(pt);
            }

            pFile->Read(&curve.fKC, sizeof(curve.fKC));
            pFile->Read(&curve.fKM, sizeof(curve.fKM));
            pFile->Read(&curve.fKY, sizeof(curve.fKY));

        }

        pFile->Close();
        SAFE_RELEASE(pFile);
        
        return S_OK;
    }

    HRESULT KG3DPostRenderEffectManager::SaveCurveStype()
    {
        IFile* pFile = g_OpenFile("data\\public\\color_curve.ccs", false, true);

        if (!pFile)
            return E_FAIL;

        int cnt = static_cast<int>(m_svCurveStype.size());
        pFile->Write(&cnt, sizeof(cnt));

        for (size_t i = 0; i < m_svCurveStype.size(); ++i)
        {
            const _ColorCurve& curve = m_svCurveStype[i];
            int ptNum = 0;

            pFile->Write(curve.szName, sizeof(curve.szName));
            
            ptNum = static_cast<int>(curve.vRed.size());
            pFile->Write(&ptNum, sizeof(ptNum));
        
            for (int v = 0; v < ptNum; ++v)
                pFile->Write(&curve.vRed[v], sizeof(D3DXVECTOR2));

            ptNum = static_cast<int>(curve.vGreen.size());
            pFile->Write(&ptNum, sizeof(ptNum));

            for (int v = 0; v < ptNum; ++v)
                pFile->Write(&curve.vGreen[v], sizeof(D3DXVECTOR2));

            ptNum = static_cast<int>(curve.vBlue.size());
            pFile->Write(&ptNum, sizeof(ptNum));

            for (int v = 0; v < ptNum; ++v)
                pFile->Write(&curve.vBlue[v], sizeof(D3DXVECTOR2));

            pFile->Write(&curve.fKC, sizeof(curve.fKC));
            pFile->Write(&curve.fKM, sizeof(curve.fKM));
            pFile->Write(&curve.fKY, sizeof(curve.fKY));
        }


        pFile->Close();
        SAFE_RELEASE(pFile);

        return S_OK;
    }

    HRESULT KG3DPostRenderEffectManager::SetActiveStype(int index, int bFlush)
    {
        if (index < 0 || index >= (int)m_svCurveStype.size())
            return E_FAIL;

        if (!bFlush && index == m_nActiveStype)
            return S_OK;

        m_nActiveStype = index;
        
        const int n = 256;
        double pt[n];
        
        const vector<D3DXVECTOR2>* vpPts[] = {
            &m_svCurveStype[m_nActiveStype].vRed,
            &m_svCurveStype[m_nActiveStype].vGreen,
            &m_svCurveStype[m_nActiveStype].vBlue
        };
        
        for (int i = 0; i < 3; ++i)
        {
            double step = 1.0 / (double)n;
            int size = (int)vpPts[i]->size();

            double* x = new double[size];
            double* y = new double[size];
            int j = 0;
            double s[5];

            for (int s = 0; s < size; ++s)
            {
                x[s] = (*vpPts[i])[s].x;
                y[s] = (*vpPts[i])[s].y;
            }

            for (double curr = 0.0; curr <= 1.0 && j < n - 1; curr += step)
                pt[j++] = max(0.0, min(1.0, g_cGraphicsTool.CubicSplineIntl(x, y, size, -1, curr, s)));
     

            pt[j++] = g_cGraphicsTool.CubicSplineIntl(x, y, size, -1, 1.0, s);

            delete[] x;
            delete[] y;

            switch (i)
            {
            case 0 :
                _SetCurveRed(pt);
                break;
            case 1 :
                _SetCurveGreed(pt);
                break;
            case 2 :
                _SetCurveBlue(pt);
                break;
            default :
                ASSERT(false);
                break;
            }
        
        }

        g_cEngineOption.nActiveCurveStype = m_nActiveStype;

        m_sEffectParams.fKC = m_svCurveStype[m_nActiveStype].fKC;
        m_sEffectParams.fKM = m_svCurveStype[m_nActiveStype].fKM;
        m_sEffectParams.fKY = m_svCurveStype[m_nActiveStype].fKY;

        return S_OK;
    }

    int KG3DPostRenderEffectManager::GetStypeName(int index, char szName[])
    {
        if (index < 0 || index >= (int)m_svCurveStype.size())
            return false;

        strcpy(szName, m_svCurveStype[index].szName);

        return true;
    }

    int KG3DPostRenderEffectManager::SetStypeName(int index, char szName[])
    {
        if (index < 0 || index >= (int)m_svCurveStype.size())
            return false;

        strncpy(m_svCurveStype[index].szName, szName, sizeof(m_svCurveStype[index].szName));

        return true;
    }

    int KG3DPostRenderEffectManager::CloneActiveStype()
    {
        if (m_nActiveStype == -1)
            return -1;
        
        m_svCurveStype.push_back(m_svCurveStype[m_nActiveStype]);

        return static_cast<int>(m_svCurveStype.size() - 1);

    }

    int KG3DPostRenderEffectManager::DelActiveStype()
    {
        if (m_nActiveStype == -1 || m_svCurveStype.size() <= 1)
            return -1;

        vector<_ColorCurve>::iterator idel = m_svCurveStype.begin();
        advance(idel, m_nActiveStype);
        m_svCurveStype.erase(idel);

        if (m_nActiveStype == (int)m_svCurveStype.size())
            --m_nActiveStype;

        return m_nActiveStype;
    }

    int KG3DPostRenderEffectManager::ResetActiveStype(DWORD channel)
    {
        if (m_nActiveStype == -1)
            return -1;

        _ColorCurve& curve = m_svCurveStype[m_nActiveStype];

        if (channel & CHANNEL_RED)
        {
            curve.vRed.clear();
            curve.vRed.push_back(D3DXVECTOR2(0.f, 0.f));
            curve.vRed.push_back(D3DXVECTOR2(1.f, 1.f));
        }
       
        if (channel & CHANNEL_GREEN)
        {
            curve.vGreen.clear();
            curve.vGreen.push_back(D3DXVECTOR2(0.f, 0.f));
            curve.vGreen.push_back(D3DXVECTOR2(1.f, 1.f));
        }

        if (channel & CHANNEL_BLUE)
        {
            curve.vBlue.clear();
            curve.vBlue.push_back(D3DXVECTOR2(0.f, 0.f));
            curve.vBlue.push_back(D3DXVECTOR2(1.f, 1.f));
        }

        return m_nActiveStype;
    }

    D3DXVECTOR2 KG3DPostRenderEffectManager::GetSplinePoint(int stype, int channel, int index)
    {
        D3DXVECTOR2 pt = D3DXVECTOR2(0.f, 0.f);
        
        if (stype < 0 || stype >= (int)m_svCurveStype.size() || channel < 0 || channel >= 3)
            return pt;

        _ColorCurve& curve = m_svCurveStype[stype];
        const vector<D3DXVECTOR2>* pPts = NULL;

        switch (channel)
        {
        case 0 :
            pPts = &curve.vRed;
            break;
        case 1 :
            pPts = &curve.vGreen;
            break;
        case 2 :
            pPts = &curve.vBlue;
            break;
        default :
            ASSERT(false);
            break;
        }

        if (index < 0 || index >= (int)pPts->size())
            return pt;

        return (*pPts)[index];
    }

    int KG3DPostRenderEffectManager::GetSplinePointCount(int stype, int channel)
    {    

        if (stype < 0 || stype >= (int)m_svCurveStype.size() || channel < 0 || channel >= 3)
            return 0;

        const _ColorCurve& curve = m_svCurveStype[stype];
        const vector<D3DXVECTOR2>* pPts = NULL;

        switch (channel)
        {
        case 0 :
            pPts = &curve.vRed;
            break;
        case 1 :
            pPts = &curve.vGreen;
            break;
        case 2 :
            pPts = &curve.vBlue;
            break;
        default :
            ASSERT(false);
            break;
        }

        return (int)pPts->size();
    }

    int KG3DPostRenderEffectManager::GetEffectNum()
    {
        int num = 0;

        for (int i = 0; i < 32; ++i)
        {
            if ((m_dwCurrType >> i) & 0x1)
                ++num;
        }

        return num;
    }

	HRESULT KG3DPostRenderEffectManager::RenderTargetSet(DWORD dwAlphaEnable /* = FALSE */)
    {
        HRESULT hResult  = E_FAIL;
        HRESULT hRetCode = E_FAIL;

        LPDIRECT3DSURFACE9 piTarget = NULL;
        LPDIRECT3DSURFACE9 piDepth  = NULL;
		D3DVIEWPORT9       Viewport;

        g_pd3dDevice->GetViewport(&m_ViewportSave);

        KG_PROCESS_ERROR(m_lpRenderTargetTexture);

        hRetCode = g_pd3dDevice->GetRenderTarget(0, &m_lpRenderTargetSave);
        KG_COM_PROCESS_ERROR(hRetCode);

		if (g_cEngineOption.bUseRenderTarget_DepthTexture)
		{
			//KGLOG_PROCESS_ERROR(m_lpMultiSampleSurface);
			//hRetCode = g_pd3dDevice->SetRenderTarget(0,m_lpMultiSampleSurface);
			//KGLOG_COM_PROCESS_ERROR(hRetCode);

			m_bTargetRestore = FALSE;

			g_cGraphicsTool.SetRenderTarget(m_lpMultiSampleDepth,m_lpMultiSampleSurface);
			SAFE_RELEASE(m_lpDepthTextureSave);
			hRetCode = g_pd3dDevice->GetDepthStencilSurface(&m_lpDepthTextureSave);
			KG_COM_PROCESS_ERROR(hRetCode);
		}
		else
		{
			hRetCode = m_lpRenderTargetTexture->GetSurfaceLevel(0, &piTarget);
			KG_COM_PROCESS_ERROR(hRetCode);

			hRetCode = g_pd3dDevice->SetRenderTarget(0, piTarget);

			KG_COM_PROCESS_ERROR(hRetCode);

			m_bTargetRestore = FALSE;

			if (NeedDepthTexture() && m_lpDepthTexture)
			{
				hRetCode = m_lpDepthTexture->GetSurfaceLevel(0, &piDepth);
				KG_COM_PROCESS_ERROR(hRetCode);

				SAFE_RELEASE(m_lpDepthTextureSave);
				hRetCode = g_pd3dDevice->GetDepthStencilSurface(&m_lpDepthTextureSave);
				KG_COM_PROCESS_ERROR(hRetCode);

				hRetCode = g_pd3dDevice->SetDepthStencilSurface(piDepth);
				KG_COM_PROCESS_ERROR(hRetCode);
			}
		}

		Viewport.X      = 0;
		Viewport.Y      = 0;
		Viewport.Width  = m_ViewportSave.Width;
		Viewport.Height = m_ViewportSave.Height;
		Viewport.MinZ   = m_ViewportSave.MinZ;
		Viewport.MaxZ   = m_ViewportSave.MaxZ;

		hRetCode = g_pd3dDevice->SetViewport(&Viewport);
        KG_COM_PROCESS_ERROR(hRetCode);

        m_dwAlphaEnable = dwAlphaEnable;

        m_fScalw = (float)m_ViewportSave.Width / (float)m_dwPostRenderWidth;
        m_fScalh = (float)m_ViewportSave.Height / (float)m_dwPostRenderHeight;

        hResult = S_OK;
Exit0:
        KG_COM_RELEASE(piTarget);
        KG_COM_RELEASE(piDepth);
        return hResult;
    }

    HRESULT KG3DPostRenderEffectManager::RenderTargetRestore()
    {
        HRESULT hResult  = E_FAIL;
        HRESULT hRetCode = E_FAIL;

		if(g_cEngineOption.bUseRenderTarget_DepthTexture)
		{
			if (m_lpRenderTargetSave)
			{
				hRetCode = g_pd3dDevice->SetRenderTarget(0, m_lpRenderTargetSave);
				KGLOG_COM_PROCESS_ERROR(hRetCode);

				m_bTargetRestore = TRUE;
			}

			if(m_lpDepthTextureSave)
			{
				hRetCode = g_pd3dDevice->SetDepthStencilSurface(m_lpDepthTextureSave);
				KG_COM_PROCESS_ERROR(hRetCode);
			}

			g_cGraphicsTool.SetRenderTarget(NULL,NULL);

		}
		else
		{
			if (m_lpRenderTargetSave)
			{
				hRetCode = g_pd3dDevice->SetRenderTarget(0, m_lpRenderTargetSave);
				KGLOG_COM_PROCESS_ERROR(hRetCode);

				m_bTargetRestore = TRUE;
			}

			if (NeedDepthTexture() && m_lpDepthTextureSave)
			{
				hRetCode = g_pd3dDevice->SetDepthStencilSurface(m_lpDepthTextureSave);
				KG_COM_PROCESS_ERROR(hRetCode);
			}

		}

		hRetCode = g_pd3dDevice->SetViewport(&m_ViewportSave);
		KG_COM_PROCESS_ERROR(hRetCode);
		
		hRetCode = g_pd3dDevice->SetTexture(0, m_lpRenderTargetTexture);
		KG_COM_PROCESS_ERROR(hRetCode);

		g_pd3dDevice->SetTexture(1, NULL);
        g_pd3dDevice->SetTexture(2, NULL);
        g_pd3dDevice->SetTexture(3, NULL);

		if(!g_cEngineOption.bUseRenderTarget_DepthTexture)
		{
			ASSERT(m_ViewportSave.Width == m_dwPostRenderWidth);
			ASSERT(m_ViewportSave.Height == m_dwPostRenderHeight);
			KG_PROCESS_ERROR(m_ViewportSave.Width == m_dwPostRenderWidth);
			KG_PROCESS_ERROR(m_ViewportSave.Height == m_dwPostRenderHeight);
		}


		DrawFullScreenQuad(
			0.0f, 0.0f, 
			m_fScalw, m_fScalh, 
			(float)m_ViewportSave.X, (float)m_ViewportSave.Y,
			(float)m_ViewportSave.Width, (float)m_ViewportSave.Height,
            m_dwAlphaEnable
        );

		if (false)
		{
			D3DXSaveTextureToFile("d:\\test2.bmp",D3DXIFF_BMP,m_lpRenderTargetTexture,NULL);
			D3DXSaveSurfaceToFile("d:\\test3.bmp",D3DXIFF_BMP,m_lpRenderTargetSave,NULL,NULL);
		}
		g_pd3dDevice->SetTexture(0, NULL);

        hResult = S_OK;
Exit0:
        KG_COM_RELEASE(m_lpDepthTextureSave);
        KG_COM_RELEASE(m_lpRenderTargetSave);
        return hResult;
    }

    void KG3DPostRenderEffectManager::UpdateEngineOption()
    {
        g_cEngineOption.bHSIEnable       = FALSE;
        g_cEngineOption.bBloomEnable     = FALSE;
		g_cEngineOption.bHDR             = FALSE;
        g_cEngineOption.bDetail          = FALSE;
        g_cEngineOption.bShockWaveEnable = FALSE;   
        g_cEngineOption.bMotionBlur      = FALSE;   
        g_cEngineOption.ModelShadowType = EM_MODEL_SHADOW_TYPE_NONE;
        g_cEngineOption.bGodRay          = FALSE;
        g_cEngineOption.bSSAO            = FALSE;
		g_cEngineOption.bGlobalFog       = FALSE;
		g_cEngineOption.bDOF             = FALSE;
        g_cEngineOption.bCurveCMYK       = FALSE;

        if (HasEffect(POST_EFFECT_SHADOW))
			 g_cEngineOption.ModelShadowType = EM_MODEL_SHADOW_TYPE_HIGH;
            //g_cEngineOption.EnableShadowMap(TRUE);*///<Shadow Sign>

        if (HasEffect(POST_EFFECT_HSI))
            g_cEngineOption.bHSIEnable = TRUE;

        if (HasEffect(POST_EFFECT_CURVE_CMYK))
            g_cEngineOption.bCurveCMYK = TRUE;

        if (HasEffect(POST_EFFECT_BLOOM))
            g_cEngineOption.bBloomEnable = TRUE;

		if (HasEffect(POST_EFFECT_HDR))
			g_cEngineOption.bHDR = TRUE;

        if (HasEffect(POST_EFFECT_DETAIL))
            g_cEngineOption.bDetail = TRUE;

        if (HasEffect(POST_EFFECT_SOCKWARE))
            g_cEngineOption.bShockWaveEnable = TRUE;   

        if (HasEffect(POST_EFFECT_MOTIONBLUE))
             g_cEngineOption.bMotionBlur = TRUE;   

        if (HasEffect(POST_EFFECT_GODRAY))
            g_cEngineOption.bGodRay = TRUE;   

        if (HasEffect(POST_EFFECT_SSAO))
            g_cEngineOption.bSSAO = TRUE;  

		if (HasEffect(POST_EFFECT_FOG))
            g_cEngineOption.bGlobalFog = TRUE;  

		if (HasEffect(POST_EFFECT_DOF))
			g_cEngineOption.bDOF = TRUE;  

    }

    HRESULT KG3DPostRenderEffectManager::EnablePostEffect(PostRenderEffectType Type, BOOL bEnable,BOOL bScaleOutput)
    {
        HRESULT hResult  = E_FAIL;

        if (bEnable)
        {
            Enable(Type);
            ResetDevice(m_bEnable, m_dwPostRenderWidth, m_dwPostRenderHeight, TRUE,bScaleOutput);
            KG_PROCESS_ERROR(HasEffect(Type));
        }
        else
        {
            Disable(Type);
            ResetDevice(m_bEnable, m_dwPostRenderWidth, m_dwPostRenderHeight, TRUE,bScaleOutput);
            KG_PROCESS_ERROR(NotHasEffect(Type));
        }

        //UpdateEngineOption();

	

        
        hResult = S_OK;
Exit0:
		switch (Type)
		{
		case POST_EFFECT_SHADOW :
			if (HasEffect(POST_EFFECT_SHADOW))
				//g_cEngineOption.EnableShadowMap(TRUE);
			{
				if (g_cEngineOption.ModelShadowType <= EM_MODEL_SHADOW_TYPE_LOW)
					g_cEngineOption.ModelShadowType = EM_MODEL_SHADOW_TYPE_HIGH;
			}
			else
				//g_cEngineOption.EnableShadowMap(FALSE);
			{
				if (g_cEngineOption.ModelShadowType > EM_MODEL_SHADOW_TYPE_LOW)
					g_cEngineOption.ModelShadowType = EM_MODEL_SHADOW_TYPE_NONE;
			}

			break;//*///<Shadow Sign>
		case POST_EFFECT_HSI :
			if (HasEffect(POST_EFFECT_HSI))
				g_cEngineOption.bHSIEnable = TRUE;
			else
				g_cEngineOption.bHSIEnable = FALSE;
			break;
        case POST_EFFECT_CURVE_CMYK :
            if (HasEffect(POST_EFFECT_CURVE_CMYK))
                g_cEngineOption.bCurveCMYK = TRUE;
            else
                g_cEngineOption.bCurveCMYK = FALSE;
            break;
		case POST_EFFECT_BLOOM :
			if (HasEffect(POST_EFFECT_BLOOM))
				g_cEngineOption.bBloomEnable = TRUE;
			else
				g_cEngineOption.bBloomEnable = FALSE;
			break;
		case POST_EFFECT_HDR :
			if (HasEffect(POST_EFFECT_HDR))
				g_cEngineOption.bHDR = TRUE;
			else
				g_cEngineOption.bHDR = FALSE;
			break;

		case POST_EFFECT_DETAIL :
			if (HasEffect(POST_EFFECT_DETAIL))
				g_cEngineOption.bDetail = TRUE;
			else
				g_cEngineOption.bDetail = FALSE;
			break;
		case POST_EFFECT_SOCKWARE :
			if (HasEffect(POST_EFFECT_SOCKWARE))
				g_cEngineOption.bShockWaveEnable = TRUE;
			else
				g_cEngineOption.bShockWaveEnable = FALSE;
			break;
		case POST_EFFECT_MOTIONBLUE :
			if (HasEffect(POST_EFFECT_MOTIONBLUE))
				g_cEngineOption.bMotionBlur = TRUE;
			else
				g_cEngineOption.bMotionBlur = FALSE;
			break;
		case POST_EFFECT_GODRAY :
			if (HasEffect(POST_EFFECT_GODRAY))
				g_cEngineOption.bGodRay = TRUE;
			else
				g_cEngineOption.bGodRay = FALSE;
			break;
		case POST_EFFECT_SSAO :
			if (HasEffect(POST_EFFECT_SSAO))
				g_cEngineOption.bSSAO = TRUE;
			else
				g_cEngineOption.bSSAO = FALSE;
			break;

		case POST_EFFECT_FOG :
			if (HasEffect(POST_EFFECT_FOG))
				g_cEngineOption.bGlobalFog = TRUE;
			else
				g_cEngineOption.bGlobalFog = FALSE;
			break;

		case POST_EFFECT_DOF :
			if (HasEffect(POST_EFFECT_DOF))
				g_cEngineOption.bDOF = TRUE;
			else
				g_cEngineOption.bDOF = FALSE;
			break;

		default :
			break;
		}  
        return hResult;
    }

    HRESULT KG3DPostRenderEffectManager::ApplyEffect(PostRenderEffectType eType, KG3DScene* pScene)
    {
        if (!m_bEnable || NotHasEffect(eType))
            return E_FAIL;

        switch (eType)
        {
        case POST_EFFECT_SSAO :
            return m_SSAOEffect.ApplyEffect(pScene);
			break;
        case POST_EFFECT_SHADOW :
            return m_ShadowEffect.ApplyEffect(pScene);
            break;
        case POST_EFFECT_DETAIL :
            return m_DetailEffect.ApplyEffect(pScene);
            break;
        case POST_EFFECT_SOCKWARE :
            return m_ShockWareEffect.ApplyEffect(pScene);
            break;
        case POST_EFFECT_GODRAY :
            return m_GodRayEffect.ApplyEffect(pScene);
            break;
        case POST_EFFECT_BLOOM :
            return m_BloomEffect.ApplyEffect(pScene);
            break;
        case POST_EFFECT_AOE :
            return m_AoeEffect.ApplyEffect(pScene);
            break;
		case POST_EFFECT_DOF :
			return m_DOFEffect.ApplyEffect(pScene);
			break;
		case POST_EFFECT_HDR :
			return m_HDREffect.ApplyEffect(pScene);
        default :
            break;
        }
        return S_OK;
    }

    HRESULT KG3DPostRenderEffectManager::RenderAoe()
    {
        if (!m_bEnable || NotHasEffect(POST_EFFECT_AOE))
            return E_FAIL;
        return m_AoeEffect.ApplyEffectNoChangeTarget();
    }

    HRESULT KG3DPostRenderEffectManager::ReloadFX()
    {
        HRESULT hResult = E_FAIL;

        KG_PROCESS_ERROR(m_bEnable);

        if (HasEffect(POST_EFFECT_SSAO))
            KGLOG_COM_PROCESS_ERROR(m_SSAOEffect.ReloadFX());

        if (HasEffect(POST_EFFECT_DETAIL))
            KGLOG_COM_PROCESS_ERROR(m_DetailEffect.ReloadFX());

        if (HasEffect(POST_EFFECT_BLOOM))
            KGLOG_COM_PROCESS_ERROR(m_BloomEffect.ReloadFX());

        if (HasEffect(POST_EFFECT_SHADOW)/*g_cEngineOption.ModelShadowType > EM_MODEL_SHADOW_TYPE_LOW*/)//<Shadow Sign>
            KGLOG_COM_PROCESS_ERROR(m_ShadowEffect.ReloadFX());

        if (HasEffect(POST_EFFECT_GODRAY))
            KGLOG_COM_PROCESS_ERROR(m_GodRayEffect.ReloadFX());

        if (HasEffect(POST_EFFECT_SOCKWARE))
            KGLOG_COM_PROCESS_ERROR(m_ShockWareEffect.ReloadFX());

        if (HasEffect(POST_EFFECT_AOE))
            KGLOG_COM_PROCESS_ERROR(m_AoeEffect.ReloadFX());

		if (HasEffect(POST_EFFECT_DOF))
			KGLOG_COM_PROCESS_ERROR(m_DOFEffect.ReloadFX());

		if (HasEffect(POST_EFFECT_HDR))
            KGLOG_COM_PROCESS_ERROR(m_HDREffect.ReloadFX());

        KGLOG_COM_PROCESS_ERROR(LoadFxFile());

        hResult = S_OK;
Exit0:
        return hResult;
    }

    HRESULT KG3DPostRenderEffectManager::LoadFxFile()
    {
        HRESULT hResult  = E_FAIL;
       // HRESULT hRetCode = E_FAIL;
		//<PostRenderShadowMove>
        /*SAFE_RELEASE(m_pEffect);
        hRetCode = KG3DD3DXCreateEffectFromFile(
            g_pd3dDevice, 
            SCREEN_EFFECT_FX_PATH, 
            NULL, 
            NULL, 
            0, 
            NULL, 
            &m_pEffect, 
            NULL);
        KG_COM_PROCESS_ERROR(hRetCode);*/

        hResult = S_OK;
//Exit0:
       /* if (FAILED(hResult))
            SAFE_RELEASE(m_pEffect);*/
        return hResult;
    }

    D3DXVECTOR4 KG3DPostRenderEffectManager::GetSunScreenPos(KG3DScene* pScene)
    {
        ASSERT(pScene);
        D3DXVECTOR4 v = *pScene->GetEnvironment().GetSunPosition();
        v.x = (v.x * 0.5f + 0.5f) * m_fScalw;
        v.y = (0.5f - v.y * 0.5f) * m_fScalh;
        return v;
    }

    HRESULT KG3DPostRenderEffectManager::OnLostDevice()
    {
        HRESULT hr = S_OK;

        ASSERT(m_lpRenderTargetTexture == NULL || (QueryRef(m_lpRenderTargetTexture) == 1));
        ASSERT(m_lpShareRenderTarger == NULL || (QueryRef(m_lpShareRenderTarger) == 1));
        ASSERT(m_lpDepthTexture == NULL || (QueryRef(m_lpDepthTexture) == 1));
        ASSERT(m_lpDepthTextureSave == NULL || (QueryRef(m_lpDepthTextureSave) == 1));
        ASSERT(m_lpRenderTargetSave == NULL || (QueryRef(m_lpRenderTargetSave) == 1));

       /* if (m_pEffect)
        {
            hr = m_pEffect->OnLostDevice();
            KGLOG_COM_PROCESS_ERROR(hr);
        }*///<PostRenderShadowMove>

        hr = m_SSAOEffect.OnLostDevice();
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = m_GodRayEffect.OnLostDevice();
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = m_ShockWareEffect.OnLostDevice();
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = m_ShadowEffect.OnLostDevice();
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = m_DetailEffect.OnLostDevice();
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = m_BloomEffect.OnLostDevice();
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = m_AoeEffect.OnLostDevice();
        KGLOG_COM_PROCESS_ERROR(hr);

		hr = m_DOFEffect.OnLostDevice();
		KGLOG_COM_PROCESS_ERROR(hr);
		
        hr = m_HDREffect.OnLostDevice();
        KGLOG_COM_PROCESS_ERROR(hr);
        
        SAFE_RELEASE(m_lpMultiSampleSurface);
		SAFE_RELEASE(m_lpMultiSampleDepth);
		SAFE_RELEASE(m_lpNoMultiSampleDepth);
		VERIFY_RELEASE(m_lpRenderTargetTexture);
        SAFE_RELEASE(m_lpShareRenderTarger);
        SAFE_RELEASE(m_lpDepthTexture);
        SAFE_RELEASE(m_lpDepthTextureSave);
        SAFE_RELEASE(m_lpRenderTargetSave);
        SAFE_RELEASE(m_lpSwapTexture);
		SAFE_RELEASE(m_lpCurveTexture);

Exit0:
        return hr;
    }

    HRESULT KG3DPostRenderEffectManager::OnResetDevice(BOOL bScaleOutput)
    {
        HRESULT hr = S_OK;

        hr = ResetDevice(m_bEnable, m_dwPostRenderWidth, m_dwPostRenderHeight, TRUE,bScaleOutput);
        KGLOG_COM_PROCESS_ERROR(hr);
        
       /* if (m_pEffect)
        {
            hr = m_pEffect->OnResetDevice();
            KGLOG_COM_PROCESS_ERROR(hr);
        }*///<PostRenderShadowMove>

        hr = m_BloomEffect.OnResetDevice();
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = m_DetailEffect.OnResetDevice();
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = m_ShadowEffect.OnResetDevice();
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = m_ShockWareEffect.OnResetDevice();
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = m_GodRayEffect.OnResetDevice();
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = m_SSAOEffect.OnResetDevice();
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = m_AoeEffect.OnResetDevice();
        KGLOG_COM_PROCESS_ERROR(hr);

		hr = m_DOFEffect.OnResetDevice();
		KGLOG_COM_PROCESS_ERROR(hr);
		
        hr = m_HDREffect.OnResetDevice();
        KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
        return hr;
    }

	LPDIRECT3DTEXTURE9 KG3DPostRenderEffectManager::CreateHALDepth(int nWidth,int nHeight)
	{ 
		HRESULT hResult  = E_FAIL;
		HRESULT hRetCode = E_FAIL;

		LPDIRECT3DTEXTURE9 pDepthTexture = NULL;
		D3DDISPLAYMODE     displayMode;
		D3DCAPS9           devCaps;
		D3DFORMAT          DepthFmt;

		KG_PROCESS_ERROR(g_pd3dDevice);
		KG_PROCESS_ERROR(g_cGraphicsEngine.m_pD3D);

		g_pd3dDevice->GetDeviceCaps(&devCaps);
		g_cGraphicsEngine.m_pD3D->GetAdapterDisplayMode(devCaps.AdapterOrdinal, &displayMode);

		// ATI depth texture format check
		DepthFmt = FOURCC_DF24;
		hRetCode = g_cGraphicsEngine.m_pD3D->CheckDeviceFormat(
			devCaps.AdapterOrdinal, 
			devCaps.DeviceType, 
			displayMode.Format, 
			D3DUSAGE_DEPTHSTENCIL, 
			D3DRTYPE_TEXTURE, 
			DepthFmt
			);
		// Nvida depth texture format check

		if (FAILED(hRetCode))
		{
			// 8x00 
			DepthFmt = (D3DFORMAT)MAKEFOURCC('I','N','T','Z');
			hRetCode = g_cGraphicsEngine.m_pD3D->CheckDeviceFormat(
				devCaps.AdapterOrdinal, 
				devCaps.DeviceType, 
				displayMode.Format, 
				D3DUSAGE_DEPTHSTENCIL, 
				D3DRTYPE_TEXTURE, 
				DepthFmt
				);

			// other (etc.6x00)

			if (FAILED(hRetCode))
			{
				DepthFmt = D3DFMT_D24S8;
				hRetCode = g_cGraphicsEngine.m_pD3D->CheckDeviceFormat(
					devCaps.AdapterOrdinal, 
					devCaps.DeviceType, 
					displayMode.Format, 
					D3DUSAGE_DEPTHSTENCIL, 
					D3DRTYPE_TEXTURE, 
					DepthFmt
					);
			}
		}

		if (FAILED(hRetCode))
		{
			DepthFmt = D3DFMT_D16;
			hRetCode = g_cGraphicsEngine.m_pD3D->CheckDeviceFormat(
				devCaps.AdapterOrdinal, 
				devCaps.DeviceType, 
				displayMode.Format, 
				D3DUSAGE_DEPTHSTENCIL, 
				D3DRTYPE_TEXTURE, 
				DepthFmt
				);
		}

		if (FAILED(hRetCode))
		{
			DepthFmt = (D3DFORMAT)MAKEFOURCC('D','F','1','6');
			hRetCode = g_cGraphicsEngine.m_pD3D->CheckDeviceFormat(
				devCaps.AdapterOrdinal, 
				devCaps.DeviceType, 
				displayMode.Format, 
				D3DUSAGE_DEPTHSTENCIL, 
				D3DRTYPE_TEXTURE, 
				DepthFmt
				);
		}
		KG_COM_PROCESS_ERROR(hRetCode);

		/*
		// Last modify by Chen TianHong:2008-10-15 17:36:53
		Depth不能比RenderTarget小，否则D3D会出错（d3d在Release情况下，貌似会强制让更大的Depth单元工作，所以
		看上去即使Depth比RT小也是能绘制正常的）

		所以无论怎样都创建和BackBuffer大小相同大小的DepthStencil。

		理论上是不会耗的，因为Depth在硬件中是特殊
		单元，在显卡中默认已经有一个整个屏幕大小的Depth单元分配了，不同的程序只是使用同一个Depth单元而已
		，而只要不超过屏幕大小，分配不同的Depth只是硬件切出那个默认的Depth单元的一部分来使用而已。因为
		只有一个，只要不Lock，耗不了多少的。
		*/


		//		UINT uWidthUsed = g_cGraphicsEngine.GetPresentParameters().BackBufferWidth;
		//		UINT uHeightUsed = g_cGraphicsEngine.GetPresentParameters().BackBufferHeight;

		// try to create now :)
		hRetCode = g_pd3dDevice->CreateTexture(
			//uWidthUsed, 
			//uHeightUsed, 
			nWidth,
			nHeight,
			1,
			D3DUSAGE_DEPTHSTENCIL,
			DepthFmt, 
			D3DPOOL_DEFAULT, 
			&pDepthTexture, 
			NULL
			);
		KG_COM_PROCESS_ERROR(hRetCode);

		hResult = S_OK;
Exit0 :
		return pDepthTexture;
	}

    LPDIRECT3DTEXTURE9 KG3DPostRenderEffectManager::CreateDepth(int nWidth, int nHeight)
    {
        HRESULT hResult  = E_FAIL;
        HRESULT hRetCode = E_FAIL;

        LPDIRECT3DTEXTURE9 pDepthTexture = NULL;
        KG_PROCESS_ERROR(g_pd3dDevice);

		if(g_cEngineOption.bUseRenderTarget_DepthTexture)
		{
			UINT nWidth = g_cGraphicsEngine.GetPresentParameters().BackBufferWidth;
			UINT nHeight = g_cGraphicsEngine.GetPresentParameters().BackBufferHeight;
			D3DSURFACE_DESC surfaceDesc;
			m_lpMultiSampleDepth->GetDesc(&surfaceDesc);
			hRetCode = g_pd3dDevice->CreateTexture(
				nWidth, 
				nHeight, 
				1,
				D3DUSAGE_RENDERTARGET,
				surfaceDesc.Format,//D3DFMT_A16B16G16R16F,//D3DFMT_R32F, 
				D3DPOOL_DEFAULT, 
				&pDepthTexture, 
				NULL
				);
			KG_COM_PROCESS_ERROR(hRetCode);
			
		}
		else
		{
			pDepthTexture = CreateHALDepth(nWidth,nHeight);
		} 
    
        hResult = S_OK;
Exit0 :
        return pDepthTexture;
    }

    HRESULT KG3DPostRenderEffectManager::CreateDepthTexture(int nWidth, int nHeight)
    {
        SAFE_RELEASE(m_lpDepthTexture);
        m_lpDepthTexture = CreateDepth(nWidth, nHeight);

        if (!m_lpDepthTexture)
            return E_FAIL;
        return S_OK;
    }

    bool KG3DPostRenderEffectManager::NeedDepthTexture()
    {
        if (HasEffect(POST_EFFECT_MOTIONBLUE) ||
			HasEffect(POST_EFFECT_DETAIL)     ||
			HasEffect(POST_EFFECT_SHADOW)/*g_cEngineOption.ModelShadowType > EM_MODEL_SHADOW_TYPE_LOW*/     ||//<Shadow Sign>
            HasEffect(POST_EFFECT_GODRAY)     ||
            HasEffect(POST_EFFECT_SSAO)       ||
			HasEffect(POST_EFFECT_FOG)        ||
            HasEffect(POST_EFFECT_AOE)        ||
			HasEffect(POST_EFFECT_DOF)        ||
			g_cEngineOption.bUseRenderTarget_DepthTexture)
            return true;
        return false;
    }

	BOOL KG3DPostRenderEffectManager::NeedPreZRender()
	{
		if (m_bEnable && HasEffect(POST_EFFECT_SHADOW)/*g_cEngineOption.ModelShadowType > EM_MODEL_SHADOW_TYPE_LOW*/)//<Shadow Sign>
			return TRUE;
		return FALSE;
	}
	BOOL KG3DPostRenderEffectManager::NeedPreColorRender()
	{
		if (m_bEnable && HasEffect(POST_EFFECT_SHADOW)/*g_cEngineOption.ModelShadowType > EM_MODEL_SHADOW_TYPE_LOW*/)//<Shadow Sign>
			return TRUE;
		return FALSE;
	}

    bool KG3DPostRenderEffectManager::NeedSwapTexture()
    {
        return GetEffectNum() > 1;
    }

    HRESULT KG3DPostRenderEffectManager::ResetDevice(BOOL bEnable, DWORD dwWidth, DWORD dwHeight, BOOL bFocus,BOOL bScaleOutput)
    {
        HRESULT hResult  = E_FAIL;
        HRESULT hRetCode = E_FAIL;
		D3DFORMAT fmtFomat = D3DFMT_R32F;
		D3DCAPS9 D3DCaps = g_cGraphicsEngine.GetD3DCaps();

		if(bScaleOutput)
		{
			dwWidth = dwHeight = g_nScaleOutputSize;
		}

        //KG_PROCESS_SUCCESS(!bFocus && bEnable == m_bEnable && m_dwPostRenderWidth == dwWidth && m_dwPostRenderHeight == dwHeight);

        if (bEnable)
        {
            KG_PROCESS_ERROR(dwWidth > 0 && dwHeight > 0);

            if (bFocus || m_dwPostRenderWidth  != dwWidth || m_dwPostRenderHeight != dwHeight)
            {
                SAFE_RELEASE(m_lpMultiSampleSurface);
				SAFE_RELEASE(m_lpMultiSampleDepth);
				SAFE_RELEASE(m_lpNoMultiSampleDepth);
				SAFE_RELEASE(m_lpRenderTargetTexture);
                SAFE_RELEASE(m_lpShareRenderTarger);
                SAFE_RELEASE(m_lpSwapTexture);
				
				if (HasEffect(POST_EFFECT_HDR))
				{
					if (g_cEngineOption.bUseRenderTarget_DepthTexture)
					{
						dwWidth  = g_cGraphicsEngine.GetPresentParameters().BackBufferWidth;
						dwHeight = g_cGraphicsEngine.GetPresentParameters().BackBufferHeight;

						//如果有打开AA需要创建对应的Surface
						hRetCode = g_pd3dDevice->CreateRenderTarget(g_cGraphicsEngine.GetPresentParameters().BackBufferWidth, 
							g_cGraphicsEngine.GetPresentParameters().BackBufferHeight,
							D3DFMT_A16B16G16R16F,
							g_cGraphicsEngine.GetPresentParameters().MultiSampleType,
							g_cGraphicsEngine.GetPresentParameters().MultiSampleQuality,
							FALSE,
							&m_lpMultiSampleSurface,
							NULL);
						KGLOG_COM_PROCESS_ERROR(hRetCode);
						
						//ATI shit cards don't support D3DFMT_R32F when use MSAA.
						fmtFomat = D3DFMT_R32F;
						hRetCode = g_cGraphicsEngine.m_pD3D->CheckDeviceMultiSampleType(D3DCaps.AdapterOrdinal, 
							D3DCaps.DeviceType,fmtFomat, g_cGraphicsEngine.m_PresentParam.Windowed, 
							g_cGraphicsEngine.m_PresentParam.MultiSampleType, NULL);
						if (FAILED(hRetCode))
						{
							fmtFomat = D3DFMT_A8R8G8B8;//D3DFMT_A16B16G16R16F;
							hRetCode = g_cGraphicsEngine.m_pD3D->CheckDeviceMultiSampleType(D3DCaps.AdapterOrdinal, 
								D3DCaps.DeviceType,fmtFomat, g_cGraphicsEngine.m_PresentParam.Windowed, 
								g_cGraphicsEngine.m_PresentParam.MultiSampleType, NULL);
							//KGLOG_COM_PROCESS_ERROR(hRetCode);
							if (FAILED(hRetCode))
							{
								fmtFomat = D3DFMT_A16B16G16R16F;
								hRetCode = g_cGraphicsEngine.m_pD3D->CheckDeviceMultiSampleType(D3DCaps.AdapterOrdinal, 
									D3DCaps.DeviceType,fmtFomat, g_cGraphicsEngine.m_PresentParam.Windowed, 
									g_cGraphicsEngine.m_PresentParam.MultiSampleType, NULL);
								KGLOG_COM_PROCESS_ERROR(hRetCode);
							}
						}

						hRetCode = g_pd3dDevice->CreateRenderTarget(g_cGraphicsEngine.GetPresentParameters().BackBufferWidth,  
							g_cGraphicsEngine.GetPresentParameters().BackBufferHeight,
							fmtFomat,
							//D3DFMT_A16B16G16R16F,
							g_cGraphicsEngine.GetPresentParameters().MultiSampleType,
							g_cGraphicsEngine.GetPresentParameters().MultiSampleQuality,
							FALSE,
							&m_lpMultiSampleDepth,
							NULL);
	
						KGLOG_COM_PROCESS_ERROR(hRetCode);

						//hRetCode = g_pd3dDevice->CreateTexture(
						//	g_cGraphicsEngine.GetPresentParameters().BackBufferWidth,
						//	g_cGraphicsEngine.GetPresentParameters().BackBufferHeight, 
						//	1,
						//	D3DUSAGE_DEPTHSTENCIL,
						//	g_cGraphicsEngine.GetPresentParameters().AutoDepthStencilFormat, 
						//	D3DPOOL_DEFAULT, 
						//	&m_lpNoMultiSampleDepth, 
						//	NULL
						//	);
						//KG_COM_PROCESS_ERROR(hRetCode);
						m_lpNoMultiSampleDepth = CreateHALDepth(g_cGraphicsEngine.GetPresentParameters().BackBufferWidth,g_cGraphicsEngine.GetPresentParameters().BackBufferHeight);
						KGLOG_PROCESS_ERROR(m_lpNoMultiSampleDepth);
					}
					hRetCode = D3DXCreateTexture(
						g_pd3dDevice, 
						dwWidth, 
						dwHeight, 
						1, 
						D3DUSAGE_RENDERTARGET, 
						D3DFMT_A16B16G16R16F,//D3DFMT_A8R8G8B8, 
						D3DPOOL_DEFAULT, 
						&m_lpRenderTargetTexture
						);
					KGLOG_COM_PROCESS_ERROR(hRetCode);

					hRetCode = D3DXCreateTexture(
						g_pd3dDevice,
						dwWidth, 
						dwHeight, 
						1, 
						D3DUSAGE_RENDERTARGET, 
						D3DFMT_A16B16G16R16F,//D3DFMT_A8R8G8B8, 
						D3DPOOL_DEFAULT, 
						&m_lpShareRenderTarger
						);
					KGLOG_COM_PROCESS_ERROR(hRetCode);
				}
				else
				{
					if (g_cEngineOption.bUseRenderTarget_DepthTexture)
					{
						dwWidth  = g_cGraphicsEngine.GetPresentParameters().BackBufferWidth;
						dwHeight = g_cGraphicsEngine.GetPresentParameters().BackBufferHeight;

						//如果有打开AA需要创建对应的Surface
						hRetCode = g_pd3dDevice->CreateRenderTarget(g_cGraphicsEngine.GetPresentParameters().BackBufferWidth, 
							g_cGraphicsEngine.GetPresentParameters().BackBufferHeight,
							D3DFMT_A8R8G8B8,
							g_cGraphicsEngine.GetPresentParameters().MultiSampleType,
							g_cGraphicsEngine.GetPresentParameters().MultiSampleQuality,
							FALSE,
							&m_lpMultiSampleSurface,
							NULL);
						KGLOG_COM_PROCESS_ERROR(hRetCode);

						//ATI shit cards don't support D3DFMT_R32F when use MSAA.
						fmtFomat = D3DFMT_R32F;
						hRetCode = g_cGraphicsEngine.m_pD3D->CheckDeviceMultiSampleType(D3DCaps.AdapterOrdinal, 
							D3DCaps.DeviceType,fmtFomat, g_cGraphicsEngine.m_PresentParam.Windowed, 
							g_cGraphicsEngine.m_PresentParam.MultiSampleType, NULL);
						if (FAILED(hRetCode))
						{
							fmtFomat = D3DFMT_A8R8G8B8;//D3DFMT_A16B16G16R16F;
							hRetCode = g_cGraphicsEngine.m_pD3D->CheckDeviceMultiSampleType(D3DCaps.AdapterOrdinal, 
								D3DCaps.DeviceType,fmtFomat, g_cGraphicsEngine.m_PresentParam.Windowed, 
								g_cGraphicsEngine.m_PresentParam.MultiSampleType, NULL);
							//KGLOG_COM_PROCESS_ERROR(hRetCode);
							if (FAILED(hRetCode))
							{
								fmtFomat = D3DFMT_A16B16G16R16F;
								hRetCode = g_cGraphicsEngine.m_pD3D->CheckDeviceMultiSampleType(D3DCaps.AdapterOrdinal, 
									D3DCaps.DeviceType,fmtFomat, g_cGraphicsEngine.m_PresentParam.Windowed, 
									g_cGraphicsEngine.m_PresentParam.MultiSampleType, NULL);
								KGLOG_COM_PROCESS_ERROR(hRetCode);
							}
						}
						hRetCode = g_pd3dDevice->CreateRenderTarget(g_cGraphicsEngine.GetPresentParameters().BackBufferWidth,  
							g_cGraphicsEngine.GetPresentParameters().BackBufferHeight,
							fmtFomat,
							//D3DFMT_A16B16G16R16F,
							g_cGraphicsEngine.GetPresentParameters().MultiSampleType,
							g_cGraphicsEngine.GetPresentParameters().MultiSampleQuality,
							FALSE,
							&m_lpMultiSampleDepth,
							NULL);

						KGLOG_COM_PROCESS_ERROR(hRetCode);

						//hRetCode = g_pd3dDevice->CreateTexture(
						//	g_cGraphicsEngine.GetPresentParameters().BackBufferWidth,
						//	g_cGraphicsEngine.GetPresentParameters().BackBufferHeight, 
						//	1,
						//	D3DUSAGE_DEPTHSTENCIL,
						//	g_cGraphicsEngine.GetPresentParameters().AutoDepthStencilFormat, 
						//	D3DPOOL_DEFAULT, 
						//	&m_lpNoMultiSampleDepth, 
						//	NULL
						//	);
						//KG_COM_PROCESS_ERROR(hRetCode);
						m_lpNoMultiSampleDepth = CreateHALDepth(g_cGraphicsEngine.GetPresentParameters().BackBufferWidth,g_cGraphicsEngine.GetPresentParameters().BackBufferHeight);
						KGLOG_PROCESS_ERROR(m_lpNoMultiSampleDepth);
					}

					{
							hRetCode = D3DXCreateTexture(
							g_pd3dDevice, 
							dwWidth, 
							dwHeight, 
							1, 
							D3DUSAGE_RENDERTARGET, 
							D3DFMT_A8R8G8B8, 
							D3DPOOL_DEFAULT, 
							&m_lpRenderTargetTexture
							);
						KGLOG_COM_PROCESS_ERROR(hRetCode);

						hRetCode = D3DXCreateTexture(
							g_pd3dDevice,

							dwWidth, 
							dwHeight, 
							1, 
							D3DUSAGE_RENDERTARGET, 
							D3DFMT_A8R8G8B8, 
							D3DPOOL_DEFAULT, 
							&m_lpShareRenderTarger
							);
						KGLOG_COM_PROCESS_ERROR(hRetCode);
					}

				}
                //if (NeedDepthTexture())
                {
                    SAFE_RELEASE(m_lpDepthTexture);
                }

                m_dwPostRenderWidth  = dwWidth;
                m_dwPostRenderHeight = dwHeight;

                if (HasEffect(POST_EFFECT_AOE))
                    m_AoeEffect.ReCreateTexture();

            } 
          
            if (NeedSwapTexture())
            {
                if (!m_lpSwapTexture)
                {
                    hRetCode = D3DXCreateTexture(
                        g_pd3dDevice,
                        dwWidth, 
                        dwHeight, 
                        1, 
                        D3DUSAGE_RENDERTARGET, 
                        //D3DFMT_A16B16G16R16F,
						D3DFMT_A8R8G8B8, 
                        D3DPOOL_DEFAULT, 
                        &m_lpSwapTexture
                        );
                    KG_COM_PROCESS_ERROR(hRetCode);
                }
            }
            else
            {
                SAFE_RELEASE(m_lpSwapTexture);
            }
      
            if (NeedDepthTexture())
            {
                if (!m_lpDepthTexture) 
                {
                    hRetCode = CreateDepthTexture(dwWidth, dwHeight);

                    if (FAILED(hRetCode))
                    {
						Disable(POST_EFFECT_MOTIONBLUE);
						Disable(POST_EFFECT_DETAIL);
                        Disable(POST_EFFECT_SHADOW);
						//g_cEngineOption.ModelShadowType = EM_MODEL_SHADOW_TYPE_NONE;//<Shadow Sign>
                        Disable(POST_EFFECT_GODRAY);
                        Disable(POST_EFFECT_SSAO);
                        Disable(POST_EFFECT_FOG);
                        Disable(POST_EFFECT_AOE);
                    }
                }
            }
            else
            {
                SAFE_RELEASE(m_lpDepthTexture);
            }

          /*  if (m_pEffect == NULL)
            {
                hRetCode = LoadFxFile();
                KGLOG_COM_PROCESS_ERROR(hRetCode);
            }*///<PostRenderShadowMove>

            if (HasEffect(POST_EFFECT_SSAO))
                m_SSAOEffect.Enable(TRUE);
            else
                m_SSAOEffect.Enable(FALSE);

			if (HasEffect(POST_EFFECT_SHADOW))//<Shadow Sign>
                m_ShadowEffect.Enable(TRUE);
            else
                m_ShadowEffect.Enable(FALSE);

            if (HasEffect(POST_EFFECT_DETAIL))
                m_DetailEffect.Enable(TRUE);
            else
                m_DetailEffect.Enable(FALSE);

            if (HasEffect(POST_EFFECT_BLOOM))
                m_BloomEffect.Enable(TRUE);
            else
                m_BloomEffect.Enable(FALSE);

            if (HasEffect(POST_EFFECT_SOCKWARE))
                m_ShockWareEffect.Enable(TRUE);
            else
                m_ShockWareEffect.Enable(FALSE);

            if (HasEffect(POST_EFFECT_GODRAY))
                m_GodRayEffect.Enable(TRUE);
            else
                m_GodRayEffect.Enable(FALSE);

            if (HasEffect(POST_EFFECT_AOE))
                m_AoeEffect.Enable(TRUE);
            else
                m_AoeEffect.Enable(FALSE);

			if (HasEffect(POST_EFFECT_DOF))
				m_DOFEffect.Enable(TRUE);
			else
				m_DOFEffect.Enable(FALSE);

			if (HasEffect(POST_EFFECT_HDR))
                m_HDREffect.Enable(TRUE);
            else
                m_HDREffect.Enable(FALSE);

            if (HasEffect(POST_EFFECT_CURVE_CMYK))
            {
                int bFlush = false;
				if (!m_lpCurveTexture)
                {
                    if (SUCCEEDED(D3DXCreateTexture(g_pd3dDevice, 256, 1, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED  , &m_lpCurveTexture)))
                    {
                        /*
                        double pts[256];
                        for (int i = 0; i < 256; ++i)
                        pts[i] = (float)i / 255.f;

                        _SetCurveRed(pts);
                        _SetCurveGreed(pts);
                        _SetCurveBlue(pts);
                        */
						bFlush = true;
                    }
                }

                if (FAILED(SetActiveStype(g_cEngineOption.nActiveCurveStype, bFlush)))
                {
                    if (FAILED(SetActiveStype(0, bFlush)))
                        EnablePostEffect(POST_EFFECT_CURVE_CMYK, FALSE,bScaleOutput);
                }
            }

            m_bEnable = TRUE;
        }
        else
        {
			SAFE_RELEASE(m_lpMultiSampleDepth);
			SAFE_RELEASE(m_lpMultiSampleSurface);
			SAFE_RELEASE(m_lpNoMultiSampleDepth);
			SAFE_RELEASE(m_lpShareRenderTarger);
            SAFE_RELEASE(m_lpRenderTargetTexture);
            SAFE_RELEASE(m_lpDepthTexture);
            SAFE_RELEASE(m_lpSwapTexture);

            m_dwPostRenderWidth  = 0;
            m_dwPostRenderHeight = 0;

            m_bEnable = FALSE;
        }


        hResult = S_OK;
Exit0:
        if (FAILED(hResult))
        {
            SAFE_RELEASE(m_lpShareRenderTarger);
            SAFE_RELEASE(m_lpRenderTargetTexture);
            SAFE_RELEASE(m_lpDepthTexture);
            SAFE_RELEASE(m_lpSwapTexture);

            m_dwPostRenderWidth  = 0;
            m_dwPostRenderHeight = 0;

            m_dwCurrType = 0;

            m_bEnable = FALSE;

        }

        //g_cEngineOption.bPostEffectEnable = m_bEnable;

        return hResult;
    }

    HRESULT KG3DPostRenderEffectManager::ComputeExposure()
    {/*
        KG3DCamera* pCamera = g_cGraphicsTool.GetCamera();
        DWORD dwNowTime = g_cGraphicsTool.GetNowTime();

        if (pCamera)
        {
            D3DXVECTOR3 vCameraUP = pCamera->GetCameraUp();
            D3DXVECTOR3 vCameraFn = pCamera->GetCameraFront();
            D3DXVECTOR3 vWorldUP  = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

            D3DXVec3Normalize(&vCameraUP, &vCameraUP);
            
            float fDot = D3DXVec3Dot(&vWorldUP, &vCameraUP);
            float fAng = D3DXToDegree(acosf(fDot));
            float fExposure = 0;
            
            if (vCameraFn.y < 0)
                fAng = -fAng;
            m_fAngel = fAng;
            
            if (m_fAngel < g_eEngineOption.fBloomCriticalAngel_min)
                fExposure = g_eEngineOption.fBloomExposure1;
            else if (m_fAngel > g_eEngineOption.fBloomGaussCoefficient)
                fExposure = g_eEngineOption.fBloomExposure2;
            else
            {
                fExposure = 
                    g_eEngineOption.fBloomExposure1 - (m_fAngel - g_eEngineOption.fBloomCriticalAngel_min) * 
                    (g_eEngineOption.fBloomExposure1 - g_eEngineOption.fBloomExposure2) / 
                    (g_eEngineOption.fBloomGaussCoefficient - g_eEngineOption.fBloomCriticalAngel_min);
            }

            if (fabs(fExposure - m_fExposureSave) > 0.000005f)
            {
                m_fTranDir = fExposure - m_fExposure;
                m_fRate    = m_fTranDir / g_eEngineOption.fBloomDelay;
            }
            
            m_fExposureSave = fExposure;
            m_fExposure += m_fRate * (dwNowTime - m_dwPrevTime);
            
            if (m_fTranDir > 0)
            {
                if (fExposure - m_fExposure < 0)
                    m_fExposure = fExposure;
            }
            else if (m_fTranDir < 0)
            {
                if (fExposure - m_fExposure > 0)
                    m_fExposure = fExposure;
            }
            else
            {
            }
        }

        m_dwPrevTime = dwNowTime;
*/
        return S_OK;
    }

    HRESULT KG3DPostRenderEffectManager::SetEffectParameter(DWORD dwType, KG3DScene* pScene)
    {
        HRESULT hResult  = E_FAIL;
        HRESULT hRetCode = E_FAIL;
        LPDIRECT3DSURFACE9 piSurface = NULL;

		//LPD3DXEFFECT pEffect = m_pEffect;//<PostRenderShadowMove>
		LPD3DXEFFECT pEffect = g_GetDefaultShaderGroup().GetDefaultEffect(DEFST_POST_RENDER);//<PostRenderShadowMove>
		_ASSERTE(NULL != pEffect);

        for (int i = 0; i < 8; ++i)
            g_pd3dDevice->SetTexture(i, NULL);

        if (!HasNextEffect(dwType))
        {
            KG_PROCESS_ERROR(m_lpRenderTargetSave);

            hRetCode = g_pd3dDevice->SetRenderTarget(0, m_lpRenderTargetSave);
            KG_COM_PROCESS_ERROR(hRetCode);

            m_bTargetRestore = TRUE;
			if (NeedDepthTexture() && m_lpDepthTextureSave)
			{
				hRetCode = g_pd3dDevice->SetDepthStencilSurface(m_lpDepthTextureSave);
				KG_COM_PROCESS_ERROR(hRetCode);
			}

            KG_COM_RELEASE(m_lpDepthTextureSave);
            KG_COM_RELEASE(m_lpRenderTargetSave);
        }
        else
        {
            KGLOG_PROCESS_ERROR(m_pFinalPassTarget);

            hRetCode = m_pFinalPassTarget->GetSurfaceLevel(0, &piSurface);
            KG_COM_PROCESS_ERROR(hRetCode);

            hRetCode = g_pd3dDevice->SetRenderTarget(0, piSurface);
            KG_COM_PROCESS_ERROR(hRetCode);
        }

        hRetCode = g_pd3dDevice->SetViewport(&m_ViewportSave);
        KG_COM_PROCESS_ERROR(hRetCode);

        hRetCode = g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.f, 0);
        KG_COM_PROCESS_ERROR(hRetCode);

        hRetCode = g_pd3dDevice->SetTexture(0, m_pFinalPassTexture);
        KG_COM_PROCESS_ERROR(hRetCode);
		
		float fMaxU = 0.f;
		float fMaxV = 0.f;
		GetDepthTexCoordScale(&fMaxU, &fMaxV);
		hRetCode = pEffect->SetFloat(TEXT("MaxU"), fMaxU);
		hRetCode = pEffect->SetFloat(TEXT("MaxV"), fMaxV);

        switch (dwType)
        {
        case POST_EFFECT_AOE :
            {
                hRetCode = g_pd3dDevice->SetTexture(1, m_AoeEffect.GetResultTexture());
                KG_COM_PROCESS_ERROR(hRetCode);

                hRetCode = pEffect->SetTechnique(TEXT("FinalAOE"));
                KG_COM_PROCESS_ERROR(hRetCode);

            }
            break;
        case POST_EFFECT_SSAO :
            {
                hRetCode = g_pd3dDevice->SetTexture(1, m_SSAOEffect.GetSSAOTexture());
                KG_COM_PROCESS_ERROR(hRetCode);

                hRetCode = pEffect->SetTechnique(TEXT("FinalSSAO"));
                KG_COM_PROCESS_ERROR(hRetCode);

                //D3DXMATRIX matProj;
                //D3DXMATRIX matInv;

                //KG3DCamera* pCamera = g_cGraphicsTool.GetCamera();
                //matProj = pCamera->GetProjectionMatrix();

                //hRetCode = g_pd3dDevice->SetTexture(1, m_lpDepthTexture);
                //KG_COM_PROCESS_ERROR(hRetCode);

                //hRetCode = pEffect->SetTechnique(TEXT("FinalSSAO"));
                //KG_COM_PROCESS_ERROR(hRetCode);

                //hRetCode = pEffect->SetMatrix(TEXT("g_Proj"), &matProj);
                //D3DXMatrixInverse(&matInv, NULL, &matProj);
                //hRetCode = pEffect->SetMatrix(TEXT("g_ProjInv"), &matInv);

                //float MaxU = m_fScalw;
                //float MaxV = m_fScalh;

                //hRetCode = pEffect->SetFloat(TEXT("MaxU"), MaxU);
                //hRetCode = pEffect->SetFloat(TEXT("MaxV"), MaxV);

                //hRetCode = pEffect->SetFloat(TEXT("radius"), g_eEngineOption.fSSAOSampleRadius);
                //hRetCode = pEffect->SetFloat(TEXT("dark"),   g_eEngineOption.fSSAODarkFact);

                //static D3DXVECTOR3 vRandOffset[32];
                //static bool bInit = false;
                //if (!bInit)
                //{
                //    int i = 0;

                //    /*
                //    vRandOffset[i++] = D3DXVECTOR3( 1.f,  0.f, 0.f);
                //    vRandOffset[i++] = D3DXVECTOR3(-1.f,  0.f, 0.f);
                //    vRandOffset[i++] = D3DXVECTOR3( 0.f,  1.f, 0.f);
                //    vRandOffset[i++] = D3DXVECTOR3( 0.f, -1.f, 0.f);

                //    vRandOffset[i++] = D3DXVECTOR3( 0.5f,  0.5f, -0.5f);
                //    vRandOffset[i++] = D3DXVECTOR3(-0.5f,  0.5f, -0.5f);
                //    vRandOffset[i++] = D3DXVECTOR3(-0.5f, -0.5f, -0.5f);
                //    vRandOffset[i++] = D3DXVECTOR3( 0.5f, -0.5f, -0.5f);
                //    */

                //    while (i < 32)
                //    {
                //        vRandOffset[i] = D3DXVECTOR3(random(-1.f, 1.f), random(-1.f, 1.f), random(-1.f, 1.f));
                //        D3DXVec3Normalize(&vRandOffset[i], &vRandOffset[i]);
                //        vRandOffset[i] *= random(0.1f, 1.f);
                //        ++i;
                //    }
         
                //    bInit = true;
                //}

                //hRetCode = pEffect->SetValue(TEXT("random"), vRandOffset, (UINT)sizeof(vRandOffset));
                //KG_COM_PROCESS_ERROR(hRetCode);

                //switch (g_eEngineOption.nSSAOLevel)
                //{
                //case SSAO_LEVEL_LOWER :
                //    *pPass = 0;
                //    break;
                //case SSAO_LEVEL_NORMAL :
                //    *pPass = 1;
                //    break;
                //case SSAO_LEVEL_HIGH :
                //    *pPass = 2;
                //    break;
                //default :
                //    *pPass = 0;
                //    break;
                //}
            }
            break;
        case POST_EFFECT_SHADOW :
            {
				hRetCode = g_pd3dDevice->SetTexture(1, m_lpDepthTexture);
                KG_COM_PROCESS_ERROR(hRetCode);

                hRetCode = g_pd3dDevice->SetTexture(2, m_ShadowEffect.GetDepthTexture());
                KG_COM_PROCESS_ERROR(hRetCode);
				
				hRetCode = pEffect->SetTechnique("FinalShadow");
                KGLOG_COM_PROCESS_ERROR(hRetCode);
            }
            break;
        case POST_EFFECT_DETAIL :
            {
                D3DXMATRIX matProjInv;

                float fMaxU = 0.f;
                float fMaxV = 0.f;

                g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProjInv);
                D3DXMatrixInverse(&matProjInv, NULL, &matProjInv);

                g_pd3dDevice->SetTexture(1, m_DetailEffect.m_lpTextureLocalAverage2);
                g_pd3dDevice->SetTexture(2, m_DetailEffect.m_lpTextureness2);
                g_pd3dDevice->SetTexture(3, m_DetailEffect.m_lpTextureness1);
                g_pd3dDevice->SetTexture(4, m_DetailEffect.m_lpTextureness0);
				g_pd3dDevice->SetTexture(5, m_lpDepthTexture);

                hRetCode = pEffect->SetTechnique("FinalPassDetail");
                KGLOG_COM_PROCESS_ERROR(hRetCode);

                GetDepthTexCoordScale(&fMaxU, &fMaxV);

                hRetCode = pEffect->SetMatrix(TEXT("g_ProjInv"), &matProjInv);
                hRetCode = pEffect->SetFloat(TEXT("MaxU"), fMaxU);
                hRetCode = pEffect->SetFloat(TEXT("MaxV"), fMaxV);
                hRetCode = pEffect->SetFloat(TEXT("detail_power"), KG3DPostRenderEffectManager::m_sEffectParams.fDetailPower);
                hRetCode = pEffect->SetFloat(TEXT("blur_power"), KG3DPostRenderEffectManager::m_sEffectParams.fBlurPower);
                hRetCode = pEffect->SetFloat(TEXT("blur_dis"), KG3DPostRenderEffectManager::m_sEffectParams.fBlurDistance);

            }
            break;
        case POST_EFFECT_SOCKWARE :
            {
                D3DXVECTOR4 vWaveParam = D3DXVECTOR4(0.f, 0.f, 0.f, 0.f);
                
                if (pScene && pScene->InUnderWater())
                {
                    hRetCode = g_pd3dDevice->SetTexture(1, m_ShockWareEffect.GetWaterShaftTexture());
                    KG_COM_PROCESS_ERROR(hRetCode);

                    hRetCode = g_pd3dDevice->SetTexture(2, m_ShockWareEffect.GetWaveBumpTexture());
                    KG_COM_PROCESS_ERROR(hRetCode);

                    hRetCode = pEffect->SetTechnique("FinalPassWaveUnderWater");
                    KGLOG_COM_PROCESS_ERROR(hRetCode);
                }
                else
                {
                    hRetCode = g_pd3dDevice->SetTexture(1, m_ShockWareEffect.GetShockWaveTexture());
                    KG_COM_PROCESS_ERROR(hRetCode);

                    hRetCode = g_pd3dDevice->SetTexture(2, m_ShockWareEffect.GetWaveBumpTexture());
                    KG_COM_PROCESS_ERROR(hRetCode);

                    hRetCode = pEffect->SetTechnique("FinalPassWave");
                    KGLOG_COM_PROCESS_ERROR(hRetCode);

                }
                
                vWaveParam.x = static_cast<float>(g_cGraphicsTool.GetNowTime()) * 0.00001f;
                vWaveParam.y = vWaveParam.x;

                pEffect->SetFloat("WavePower", KG3DPostRenderEffectManager::m_sEffectParams.fShockWavePower);
                pEffect->SetVector("vWaveParam", &vWaveParam);
            }
            break;
        case POST_EFFECT_GODRAY :
            {
                KG_PROCESS_SUCCESS(!m_GodRayEffect.BeActive());

                hRetCode = g_pd3dDevice->SetTexture(1, m_GodRayEffect.GetRayTexture());
                KG_COM_PROCESS_ERROR(hRetCode);

                hRetCode = pEffect->SetTechnique("FinalGodRay");
                KGLOG_COM_PROCESS_ERROR(hRetCode);

                hRetCode = pEffect->SetFloat("Density", KG3DPostRenderEffectManager::m_sEffectParams.fGodRayDensity);
                KG_COM_PROCESS_ERROR(hRetCode);

                hRetCode = pEffect->SetFloat("Weight", KG3DPostRenderEffectManager::m_sEffectParams.fGodRayWeight);
                KG_COM_PROCESS_ERROR(hRetCode);

                hRetCode = pEffect->SetFloat("Decay", KG3DPostRenderEffectManager::m_sEffectParams.fGodRayDecay);
                KG_COM_PROCESS_ERROR(hRetCode);

                hRetCode = pEffect->SetFloat("GodRayExposure", KG3DPostRenderEffectManager::m_sEffectParams.fGodRayExposure);
                KG_COM_PROCESS_ERROR(hRetCode);

            }
            break;
        case POST_EFFECT_BLOOM :
            {
                hRetCode = g_pd3dDevice->SetTexture(1, m_BloomEffect.GetBloomTexture());
                KG_COM_PROCESS_ERROR(hRetCode);

                /*
                hRetCode = ComputeExposure();
                KG_COM_PROCESS_ERROR(hRetCode);
                */

                hRetCode = pEffect->SetTechnique("FinalPassBloom");
                KGLOG_COM_PROCESS_ERROR(hRetCode);

                hRetCode = pEffect->SetValue("Exposure1", &KG3DPostRenderEffectManager::m_sEffectParams.fBloomExposure1, sizeof(float));
                hRetCode = pEffect->SetValue("Exposure2", &KG3DPostRenderEffectManager::m_sEffectParams.fBloomExposure2, sizeof(float));



                /*
                hRetCode = pEffect->SetValue("Exposure", &m_fExposure, sizeof(float));
                KG_COM_PROCESS_ERROR(hRetCode);
                */
            }
            break;
        case POST_EFFECT_HSI :
            {
                hRetCode = pEffect->SetTechnique("FinalPassHSI");
                KGLOG_COM_PROCESS_ERROR(hRetCode);

                pEffect->SetFloat("HueScale",        KG3DPostRenderEffectManager::m_sEffectParams.fHueScale);
                pEffect->SetFloat("HueBias",         KG3DPostRenderEffectManager::m_sEffectParams.fHueBias);
                pEffect->SetFloat("SaturationScale", KG3DPostRenderEffectManager::m_sEffectParams.fSaturationScale);
                pEffect->SetFloat("SaturationBias",  KG3DPostRenderEffectManager::m_sEffectParams.fSaturationBias);
                pEffect->SetFloat("IntensityScale",  KG3DPostRenderEffectManager::m_sEffectParams.fIntensityScale);
                pEffect->SetFloat("IntensityBias",   KG3DPostRenderEffectManager::m_sEffectParams.fIntensityBias);
            }
            break;
        case POST_EFFECT_CURVE_CMYK :
            {
                hRetCode = pEffect->SetTechnique("FinalPassCurveCMYK");
                KGLOG_COM_PROCESS_ERROR(hRetCode);

                hRetCode = g_pd3dDevice->SetTexture(1, m_lpCurveTexture);
                KG_COM_PROCESS_ERROR(hRetCode);

                pEffect->SetFloat("cmyk_kc",  m_sEffectParams.fKC);
                pEffect->SetFloat("cmyk_km",  m_sEffectParams.fKM);
                pEffect->SetFloat("cmyk_ky",  m_sEffectParams.fKY);
            }
            break;
        case POST_EFFECT_MOTIONBLUE :
            {
                D3DXMATRIX matViewProj;
                D3DXMATRIX matView;
                D3DXMATRIX matProj;
                D3DXMATRIX matInv;

                g_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
                g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProj);

                matViewProj = matView * matProj;
                D3DXMatrixInverse(&matInv, NULL, &matViewProj);

                hRetCode = pEffect->SetMatrix("g_MatViewPorjInv", &matInv);
                hRetCode = pEffect->SetMatrix("g_MatViewPorj",    &matViewProj);
                hRetCode = pEffect->SetMatrix("g_PrvMatViewPorj", &m_matPrevViewProj);

                //float MaxU = m_fScalw;
                //float MaxV = m_fScalh;

                //hRetCode = pEffect->SetFloat("MaxU", MaxU);
                //hRetCode = pEffect->SetFloat("MaxV", MaxV);

                m_matPrevViewProj = matViewProj;

                hRetCode = g_pd3dDevice->SetTexture(1, m_lpDepthTexture);
                KG_COM_PROCESS_ERROR(hRetCode);

                hRetCode = pEffect->SetTechnique("MotionBlur");
                KGLOG_COM_PROCESS_ERROR(hRetCode);
            }
            break;

		case POST_EFFECT_FOG :
			{
				D3DXMATRIX matViewProj;
				D3DXMATRIX matViewProjInv;
                D3DXMATRIX matView;
                D3DXMATRIX matProj;
                D3DXMATRIX matViewInv;
				D3DXMATRIX matProjInv;
				D3DXVECTOR3 vecCameraPos;

				KG3DCamera* pCamera = g_cGraphicsTool.GetCamera();
				matProj = pCamera->GetProjectionMatrix();
				matView = pCamera->GetViewMatrix();
				pCamera->GetPosition(&vecCameraPos);

                matViewProj = matView * matProj;
                D3DXMatrixInverse(&matViewProjInv, NULL, &matViewProj);
                hRetCode = pEffect->SetMatrix("g_MatViewPorjInv", &matViewProjInv);
				D3DXVECTOR4 vecCameraPos4(vecCameraPos.x,vecCameraPos.y,vecCameraPos.z,1);
				hRetCode = pEffect->SetVector(TEXT("cameraWorldPos"),&vecCameraPos4);
				hRetCode = pEffect->SetVector(TEXT("fogcolors"),&(KG3DPostRenderEffectManager::m_sEffectParams.vecFogcolor));
				pEffect->SetFloat("fogDensityV",  KG3DPostRenderEffectManager::m_sEffectParams.fFogDensityV);
                pEffect->SetFloat("SlopeThreshold",  KG3DPostRenderEffectManager::m_sEffectParams.fSlopeThreshold);
                pEffect->SetFloat("heightFalloff",   KG3DPostRenderEffectManager::m_sEffectParams.fHeightFalloff);
				
                hRetCode = g_pd3dDevice->SetTexture(1, m_lpDepthTexture);
                KG_COM_PROCESS_ERROR(hRetCode);

                hRetCode = pEffect->SetTechnique("FinalFog");
                KGLOG_COM_PROCESS_ERROR(hRetCode);
			}
			break;
		case POST_EFFECT_DOF :
			{
				pEffect->SetTechnique("FinalSceneDOF");
				DWORD dwHeight = 0,dwWidth = 0;
				IEKG3DScene* pScene = g_cGraphicsTool.GetScene();
				if (pScene)
				{
					IEKG3DSceneOutputWnd* pOutputWindow = NULL;
					pScene->GetCurOutputWnd(&pOutputWindow);
					if (pOutputWindow)
					{
						pOutputWindow->GetViewPortWidthHeight(&dwWidth,&dwHeight);
					}
				}
				D3DXVECTOR4 vPixSize;
				vPixSize.x = (1.0f) / dwWidth;
				vPixSize.y = (1.0f) / dwHeight;
				vPixSize.z = (1.0f) / ((dwWidth - dwWidth % 8) / 4);
				vPixSize.w = (1.0f) / ((dwHeight - dwHeight % 8) / 4);
				pEffect->SetVector("pixelSizes",&vPixSize);
				HRESULT hr = g_pd3dDevice->SetTexture(1,m_DOFEffect.m_pTexDOFDepth);
				hr = g_pd3dDevice->SetTexture(2,m_DOFEffect.m_pTexSceneScaled);

				if (false)
				{
					hr = D3DXSaveTextureToFile("e:\\test.bmp",D3DXIFF_BMP ,m_DOFEffect.m_pTexDOFDepth,NULL);
				}
				g_pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
				g_pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_POINT );
			}
			break;
		case POST_EFFECT_HDR :
			{
				pEffect->SetTechnique("FinalSceneHDR");
				pEffect->SetFloat("g_fMiddleGray", 0.5f);
				pEffect->SetFloat( "g_fBloomScale", PostRender::KG3DPostRenderEffectManager::m_sEffectParams.fHDRBloomScale );
				pEffect->SetFloat( "g_fStarScale", PostRender::KG3DPostRenderEffectManager::m_sEffectParams.fHDRStarScale );
				pEffect->SetBool("g_bEnableBlueShift",PostRender::KG3DPostRenderEffectManager::m_sEffectParams.bHDREnableBlueShift);

				g_pd3dDevice->SetTexture( 1,m_HDREffect.m_apTexBloom[0]);
				g_pd3dDevice->SetTexture( 2, m_HDREffect.m_apTexStar[0]);
				g_pd3dDevice->SetTexture( 3, m_HDREffect.m_pTexAdaptedLuminanceCur);

				if (false)
				{
					HRESULT hr = D3DXSaveTextureToFile("e:\\test.bmp",D3DXIFF_BMP ,m_HDREffect.m_apTexBloom[0],NULL);
					hr = D3DXSaveTextureToFile("e:\\test2.bmp",D3DXIFF_BMP,m_HDREffect.m_apTexStar[0],NULL);
					hr = D3DXSaveTextureToFile("e:\\test3.bmp",D3DXIFF_BMP,m_HDREffect.m_pTexAdaptedLuminanceCur,NULL);
				}
				g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
				g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
				g_pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
				g_pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
				g_pd3dDevice->SetSamplerState( 2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
				g_pd3dDevice->SetSamplerState( 2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
				g_pd3dDevice->SetSamplerState( 3, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
				g_pd3dDevice->SetSamplerState( 3, D3DSAMP_MINFILTER, D3DTEXF_POINT );
				/*
				pEffect->Begin(&uiPassCount, 0);
				for (uiPass = 0; uiPass < uiPassCount; uiPass++)
				{
					pEffect->BeginPass(uiPass);
					DrawFullScreenQuad( 0.0f, 0.0f, 1.0f, 1.0f );
					pEffect->EndPass();
				}
				pEffect->End();
				*/
			}
			break;
        default :
            break;
        }

Exit1:
        swap(m_pFinalPassTexture, m_pFinalPassTarget);
        hResult = S_OK;
Exit0:
        KG_COM_RELEASE(piSurface);
        return hResult;
    }

    HRESULT KG3DPostRenderEffectManager::Draw(UINT pass, float z)
    {
		//LPD3DXEFFECT pEffect = m_pEffect;//<PostRenderShadowMove>
		LPD3DXEFFECT pEffect = g_GetDefaultShaderGroup().GetDefaultEffect(DEFST_POST_RENDER);//<PostRenderShadowMove>
        ASSERT(pEffect);

        UINT uPass;

        if (pass != 0xffffffff)
        {
            pEffect->Begin(&uPass,0);
            pEffect->BeginPass(pass);

            DrawFullScreenQuad(
                0.0f, 0.0f, 
                m_fScalw, 
                m_fScalh, 
                static_cast<float>(m_ViewportSave.X), 
                static_cast<float>(m_ViewportSave.Y),
                static_cast<float>(m_ViewportSave.Width), 
                static_cast<float>(m_ViewportSave.Height),
                m_dwAlphaEnable,
                z
                );

            pEffect->EndPass();
            pEffect->End();
        }
        else
        {
            DrawFullScreenQuad(
                0.0f, 0.0f, 
                m_fScalw, 
                m_fScalh, 
                static_cast<float>(m_ViewportSave.X), 
                static_cast<float>(m_ViewportSave.Y),
                static_cast<float>(m_ViewportSave.Width), 
                static_cast<float>(m_ViewportSave.Height),
                TRUE,
                z
                );
        }

        return S_OK;
    }

    HRESULT KG3DPostRenderEffectManager::Draw(UINT pass, float z, D3DVIEWPORT9& vp)
    {
		//LPD3DXEFFECT pEffect = m_pEffect;//<PostRenderShadowMove>
		LPD3DXEFFECT pEffect = g_GetDefaultShaderGroup().GetDefaultEffect(DEFST_POST_RENDER);//<PostRenderShadowMove>
        ASSERT(pEffect);

		D3DVIEWPORT9 vportSave;
		g_pd3dDevice->GetViewport(&vportSave);
		g_pd3dDevice->SetViewport(&vp);

        UINT uPass;

        pEffect->Begin(&uPass,0);
        pEffect->BeginPass(pass);

        DrawFullScreenQuad(
            0.0f, 0.0f, 
            m_fScalw, 
            m_fScalh, 
            static_cast<float>(vp.X), 
            static_cast<float>(vp.Y),
            static_cast<float>(vp.Width), 
            static_cast<float>(vp.Height),
            FALSE,
            z
            );

        pEffect->EndPass();
        pEffect->End();

		g_pd3dDevice->SetViewport(&vportSave);

        return S_OK;
    }


    HRESULT KG3DPostRenderEffectManager::DrawFullRect(UINT pass, const D3DVIEWPORT9& vp, float z)
    {
		//LPD3DXEFFECT pEffect = m_pEffect;//<PostRenderShadowMove>
		LPD3DXEFFECT pEffect = g_GetDefaultShaderGroup().GetDefaultEffect(DEFST_POST_RENDER);//<PostRenderShadowMove>
        ASSERT(pEffect);

        UINT uPass;

        pEffect->Begin(&uPass,0);
        pEffect->BeginPass(pass);

        DrawFullScreenQuad(
            0.f,
            0.f,
            1.f,
            1.f,
            static_cast<float>(vp.X), 
            static_cast<float>(vp.Y),
            static_cast<float>(vp.Width), 
            static_cast<float>(vp.Height),
            m_dwAlphaEnable,
            z
            );

        pEffect->EndPass();
        pEffect->End();

        return S_OK;
    }

    LPDIRECT3DTEXTURE9 KG3DPostRenderEffectManager::GetShareTexture()
    {
        return m_lpShareRenderTarger;
    }

    LPDIRECT3DSURFACE9 KG3DPostRenderEffectManager::GetShareSurface()
    {
        HRESULT hr = E_FAIL;
        LPDIRECT3DSURFACE9 pSurface = NULL;

        KG_PROCESS_ERROR(m_lpShareRenderTarger);

        hr = m_lpShareRenderTarger->GetSurfaceLevel(0, &pSurface);
        KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
        return pSurface;
    }

    void KG3DPostRenderEffectManager::GetDepthTexCoordScale(float* pU, float* pV)
    {
        if (!pU || !pV)
            return;

        if (g_cEngineOption.bUseRenderTarget_DepthTexture)
		{
			(*pU) = static_cast<float>(m_ViewportSave.Width) / static_cast<float>(g_cGraphicsEngine.GetPresentParameters().BackBufferWidth);
			(*pV) = static_cast<float>(m_ViewportSave.Height) / static_cast<float>(g_cGraphicsEngine.GetPresentParameters().BackBufferHeight);
		}
		else
		{
			(*pU) = 1.0;
			(*pV) = 1.0;
		}

    }

    int KG3DPostRenderEffectManager::SetActiveCurveRedPoints(D3DXVECTOR2* pts, int num)
    {
        if (m_nActiveStype < 0 || m_nActiveStype >= (int)m_svCurveStype.size())
            return false;

        m_svCurveStype[m_nActiveStype].vRed.clear();
        copy(pts, pts + num, back_inserter(m_svCurveStype[m_nActiveStype].vRed));

        SetActiveStype(m_nActiveStype, true);

        return true;
    }

    int KG3DPostRenderEffectManager::SetActiveCurveGreedPoints(D3DXVECTOR2* pts, int num)
    {
        if (m_nActiveStype < 0 || m_nActiveStype >= (int)m_svCurveStype.size())
            return false;

        m_svCurveStype[m_nActiveStype].vGreen.clear();
        copy(pts, pts + num, back_inserter(m_svCurveStype[m_nActiveStype].vGreen));

        SetActiveStype(m_nActiveStype, true);

        return true;
    }

    int KG3DPostRenderEffectManager::SetActiveCurveBluePoints(D3DXVECTOR2* pts, int num)
    {
        if (m_nActiveStype < 0 || m_nActiveStype >= (int)m_svCurveStype.size())
            return false;

        m_svCurveStype[m_nActiveStype].vBlue.clear();
        copy(pts, pts + num, back_inserter(m_svCurveStype[m_nActiveStype].vBlue));

        SetActiveStype(m_nActiveStype, true);

        return true;
    }

    int KG3DPostRenderEffectManager::SetCMYK_KC(float k)
    {
        if (m_nActiveStype < 0 || m_nActiveStype >= (int)m_svCurveStype.size())
            return false;

        m_svCurveStype[m_nActiveStype].fKC = k;
        m_sEffectParams.fKC = k; 

        return true;
    }

    int KG3DPostRenderEffectManager::SetCMYK_KM(float k)
    {
        if (m_nActiveStype < 0 || m_nActiveStype >= (int)m_svCurveStype.size())
            return false;

        m_svCurveStype[m_nActiveStype].fKM = k;
        m_sEffectParams.fKM = k; 

        return true;
    }

    int KG3DPostRenderEffectManager::SetCMYK_KY(float k)
    {
        if (m_nActiveStype < 0 || m_nActiveStype >= (int)m_svCurveStype.size())
            return false;

        m_svCurveStype[m_nActiveStype].fKY = k;
        m_sEffectParams.fKY = k; 

        return true;
    }

    int KG3DPostRenderEffectManager::_SetCurveRed(double pts[])
    {
        if (!m_lpCurveTexture)
            return false;

        D3DLOCKED_RECT lockRect;

        if (FAILED(m_lpCurveTexture->LockRect(0, &lockRect, NULL, 0 )))
            return false;

        struct _Color { BYTE rgba[4]; };

        _Color* pColor = (_Color*)lockRect.pBits;

        for (int i = 0; i < 256; ++i)
            pColor[i].rgba[2] = (BYTE)(pts[i] * 255 + 0.5);
            
        m_lpCurveTexture->UnlockRect(0);

        return true;
    }

    int KG3DPostRenderEffectManager::_SetCurveGreed(double pts[])
    {
        if (!m_lpCurveTexture)
            return false;

        D3DLOCKED_RECT lockRect;

        if (FAILED(m_lpCurveTexture->LockRect(0, &lockRect, NULL, 0 )))
            return false;

        struct _Color { BYTE rgba[4]; };

        _Color* pColor = (_Color*)lockRect.pBits;

        for (int i = 0; i < 256; ++i)
            pColor[i].rgba[1] = (BYTE)(pts[i] * 255 + 0.5);

        m_lpCurveTexture->UnlockRect(0);

        return true;
    }


    int KG3DPostRenderEffectManager::_SetCurveBlue(double pts[])
    {
        if (!m_lpCurveTexture)
            return false;

        D3DLOCKED_RECT lockRect;

        if (FAILED(m_lpCurveTexture->LockRect(0, &lockRect, NULL, 0 )))
            return false;

        struct _Color { BYTE rgba[4]; };

        _Color* pColor = (_Color*)lockRect.pBits;

        for (int i = 0; i < 256; ++i)
            pColor[i].rgba[0] = (BYTE)(pts[i] * 255 + 0.5);

        m_lpCurveTexture->UnlockRect(0);

        return true;
    }
	HRESULT KG3DPostRenderEffectManager::PreZRender(KG3DScene* pScene)
	{
		HRESULT hResult  = E_FAIL;
		LPDIRECT3DSURFACE9 piDepthSave = NULL;
		LPDIRECT3DSURFACE9 piColorSave = NULL;
		KG3DCamera* pCamera = NULL;
		D3DXVECTOR3 vecLookat,vecPos,vecUp;
		KG3DRenderState RenderState;
		D3DVIEWPORT9 viewportSave;
		BOOL viewPortSaved = FALSE;

		//LPD3DXEFFECT pEffect = m_pEffect;//<PostRenderShadowMove>
		LPD3DXEFFECT pEffect = g_GetDefaultShaderGroup().GetDefaultEffect(DEFST_POST_RENDER);//<PostRenderShadowMove>
		KG_PROCESS_ERROR(m_dwCurrType);
		KG_PROCESS_ERROR(pEffect);
		
		hResult = g_pd3dDevice->GetDepthStencilSurface(&piDepthSave);
		KGLOG_COM_PROCESS_ERROR(hResult);
		hResult = g_pd3dDevice->GetRenderTarget(0,&piColorSave);
		KGLOG_COM_PROCESS_ERROR(hResult);
		pCamera = g_cGraphicsTool.GetCamera();
		pCamera->GetPosition(&vecPos);
		pCamera->GetUpDirection(&vecUp);
		pCamera->GetLookAtPosition(&vecLookat);
		pCamera->Capture();
		hResult = g_pd3dDevice->GetViewport(&viewportSave);
		KGLOG_COM_PROCESS_ERROR(hResult);
		viewPortSaved = TRUE;
		RenderState.SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		RenderState.SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);        
		RenderState.SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		RenderState.SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);  
		if (HasEffect(POST_EFFECT_SHADOW)/*g_cEngineOption.ModelShadowType > EM_MODEL_SHADOW_TYPE_LOW*/)//<Shadow Sign>
			m_ShadowEffect.ProcessShadowMap();
		pCamera->SetPosition(vecPos);
		pCamera->SetUpDirection(vecUp);
		pCamera->SetLookAtPosition(vecLookat);
		pCamera->RestoreCapture();
		hResult = S_OK;
Exit0:
		if (piColorSave)
		{
			hResult = g_pd3dDevice->SetRenderTarget(0,piColorSave);
			KG_COM_RELEASE(piColorSave);
			KGLOG_COM_CHECK_ERROR(hResult);
		}
		if (piDepthSave)
		{
			hResult = g_pd3dDevice->SetDepthStencilSurface(piDepthSave);
			KG_COM_RELEASE(piDepthSave);
			KGLOG_COM_CHECK_ERROR(hResult);
		}
		if (viewPortSaved)
		{
			hResult = g_pd3dDevice->SetViewport(&viewportSave);
			KGLOG_COM_CHECK_ERROR(hResult);
		}
		return hResult;
	}

	HRESULT KG3DPostRenderEffectManager::PreColorRender(KG3DScene* pScene)
	{
		HRESULT hResult  = E_FAIL;
		LPDIRECT3DSURFACE9 piDepthSave = NULL;
		LPDIRECT3DSURFACE9 piColorSave = NULL;
		KG3DCamera* pCamera = NULL;
		D3DXVECTOR3 vecLookat,vecPos,vecUp;
		KG3DRenderState RenderState;
		D3DVIEWPORT9 viewportSave;
		BOOL viewPortSaved = FALSE;
		KG_PROCESS_ERROR(m_dwCurrType);
		//LPD3DXEFFECT pEffect = m_pEffect;//<PostRenderShadowMove>
		LPD3DXEFFECT pEffect = g_GetDefaultShaderGroup().GetDefaultEffect(DEFST_POST_RENDER);//<PostRenderShadowMove>
		KG_PROCESS_ERROR(pEffect);

		hResult = g_pd3dDevice->GetDepthStencilSurface(&piDepthSave);
		KGLOG_COM_PROCESS_ERROR(hResult);
		hResult = g_pd3dDevice->GetRenderTarget(0,&piColorSave);
		KGLOG_COM_PROCESS_ERROR(hResult);
		hResult = g_pd3dDevice->GetViewport(&viewportSave);
		KGLOG_COM_PROCESS_ERROR(hResult);
		viewPortSaved = TRUE;
		if (g_cEngineOption.bUseRenderTarget_DepthTexture)
		{
			LPDIRECT3DSURFACE9 pDestSurface = NULL;
			LPDIRECT3DSURFACE9 pDepthSurface = NULL;

			KGLOG_PROCESS_ERROR(m_lpDepthTexture);
			KGLOG_PROCESS_ERROR(m_lpMultiSampleDepth);

			hResult = m_lpDepthTexture->GetSurfaceLevel(0, &pDestSurface);
			KGLOG_COM_PROCESS_ERROR(hResult);

			hResult = g_pd3dDevice->StretchRect(m_lpMultiSampleDepth, NULL, pDestSurface, NULL, D3DTEXF_POINT);            
			SAFE_RELEASE(pDestSurface);
			KGLOG_COM_PROCESS_ERROR(hResult);

			KGLOG_PROCESS_ERROR(m_lpNoMultiSampleDepth);
			hResult = m_lpNoMultiSampleDepth->GetSurfaceLevel(0,&pDepthSurface);
			KGLOG_COM_PROCESS_ERROR(hResult);

			hResult = g_pd3dDevice->SetDepthStencilSurface(pDepthSurface);
			SAFE_RELEASE(pDepthSurface);
			KGLOG_COM_PROCESS_ERROR(hResult);
		}
		else
		{
			if (NeedDepthTexture() && m_lpDepthTextureSave)
			{
				hResult = g_pd3dDevice->SetDepthStencilSurface(m_lpDepthTextureSave);
				KG_COM_PROCESS_ERROR(hResult);
			}
		}
		pCamera = g_cGraphicsTool.GetCamera();

		//pCamera->GetPosition(&vecPos);
		//pCamera->GetUpDirection(&vecUp);
		//pCamera->GetLookAtPosition(&vecLookat);
		//pCamera->Capture();
		RenderState.SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		RenderState.SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);        
		RenderState.SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		RenderState.SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);  
		if (HasEffect(POST_EFFECT_SHADOW)/*g_cEngineOption.ModelShadowType > EM_MODEL_SHADOW_TYPE_LOW*/)//<Shadow Sign>
			m_ShadowEffect.ProcessPostRenderShadow();
		//pCamera->SetPosition(vecPos);
		//pCamera->SetUpDirection(vecUp);
		//pCamera->SetLookAtPosition(vecLookat);
		//pCamera->RestoreCapture();
		hResult = S_OK;
Exit0:
		if (piColorSave)
		{
			hResult = g_pd3dDevice->SetRenderTarget(0,piColorSave);
			KG_COM_RELEASE(piColorSave);
			KGLOG_COM_CHECK_ERROR(hResult);
		}
		if (piDepthSave)
		{
			hResult = g_pd3dDevice->SetDepthStencilSurface(piDepthSave);
			KG_COM_RELEASE(piDepthSave);
			KGLOG_COM_CHECK_ERROR(hResult);
		}
		if (viewPortSaved)
		{
			hResult = g_pd3dDevice->SetViewport(&viewportSave);
			KGLOG_COM_CHECK_ERROR(hResult);
		}
		return hResult;
	}

	HRESULT KG3DPostRenderEffectManager::Render(KG3DScene* pScene)
    {
        HRESULT hResult  = E_FAIL;

        KG3DRenderState RenderState;

		//LPD3DXEFFECT pEffect = m_pEffect;//<PostRenderShadowMove>
		LPD3DXEFFECT pEffect = g_GetDefaultShaderGroup().GetDefaultEffect(DEFST_POST_RENDER);//<PostRenderShadowMove>
        KG_PROCESS_ERROR(m_dwCurrType);

        KG_PROCESS_ERROR(pEffect);

        // must do this first

        //if (HasEffect(POST_EFFECT_SOCKWARE))
        //    ApplyEffect(POST_EFFECT_SOCKWARE, pScene);

        m_pFinalPassTexture = m_lpRenderTargetTexture;
        m_pFinalPassTarget  = m_lpSwapTexture;

		if (g_cEngineOption.bUseRenderTarget_DepthTexture)
		{
			LPDIRECT3DSURFACE9 pDestSurface = NULL;
			LPDIRECT3DSURFACE9 pDepthSurface = NULL;

            g_cGraphicsTool.SetRenderTarget(NULL,NULL);
            //g_cGraphicsTool.EnableRenderTarget(-1);//不再允许-1值，原来EnableRenderTarget(-1)变成DisableRenderTarget
			g_cGraphicsTool.DisableRenderTarget();

            KGLOG_PROCESS_ERROR(m_lpRenderTargetTexture);
			KGLOG_PROCESS_ERROR(m_lpMultiSampleSurface);

			hResult = m_lpRenderTargetTexture->GetSurfaceLevel(0, &pDestSurface);
			KGLOG_COM_PROCESS_ERROR(hResult);

			hResult = g_pd3dDevice->StretchRect(m_lpMultiSampleSurface, NULL, pDestSurface, NULL, D3DTEXF_POINT);
			SAFE_RELEASE(pDestSurface);
			KGLOG_COM_PROCESS_ERROR(hResult);
			if (!NeedPreColorRender())
			{
				KGLOG_PROCESS_ERROR(m_lpDepthTexture);
				KGLOG_PROCESS_ERROR(m_lpMultiSampleDepth);
	            
				hResult = m_lpDepthTexture->GetSurfaceLevel(0, &pDestSurface);
				KGLOG_COM_PROCESS_ERROR(hResult);
				
				hResult = g_pd3dDevice->StretchRect(m_lpMultiSampleDepth, NULL, pDestSurface, NULL, D3DTEXF_POINT);            
				SAFE_RELEASE(pDestSurface);
				KGLOG_COM_PROCESS_ERROR(hResult);
			}

			KGLOG_PROCESS_ERROR(m_lpNoMultiSampleDepth);
			hResult = m_lpNoMultiSampleDepth->GetSurfaceLevel(0,&pDepthSurface);
			KGLOG_COM_PROCESS_ERROR(hResult);

			hResult = g_pd3dDevice->SetDepthStencilSurface(pDepthSurface);
			SAFE_RELEASE(pDepthSurface);
			KGLOG_COM_PROCESS_ERROR(hResult);

			if (HasEffect(POST_EFFECT_SOCKWARE))
				ApplyEffect(POST_EFFECT_SOCKWARE, pScene);

            if (false)
            {
                D3DXSaveTextureToFile("d:\\test2.bmp",D3DXIFF_BMP,m_lpRenderTargetTexture,NULL);
                D3DXSaveSurfaceToFile("d:\\test3.bmp",D3DXIFF_BMP,m_lpMultiSampleSurface,NULL,NULL);
                D3DXSaveTextureToFile("d:\\test4.bmp",D3DXIFF_BMP,m_lpDepthTexture,NULL);
                D3DXSaveSurfaceToFile("d:\\test5.bmp",D3DXIFF_BMP,m_lpMultiSampleDepth,NULL,NULL);
            }

		}
		else
		{
			if (HasEffect(POST_EFFECT_SOCKWARE))
				ApplyEffect(POST_EFFECT_SOCKWARE, pScene);

			if (NeedDepthTexture() && m_lpDepthTextureSave)
			{
				hResult = g_pd3dDevice->SetDepthStencilSurface(m_lpDepthTextureSave);
				KG_COM_PROCESS_ERROR(hResult);
			}
		}

		RenderState.SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
        RenderState.SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);        
        RenderState.SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
        RenderState.SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);        
		size_t validEffectCount = 0;
        for (int i = 0; i < POST_EFFECT_MAX_COUNT; ++i)
        {
            PostRenderEffectType effect = PostRenderEffectType(1 << i);
            if (HasEffect(effect))
            {
                if (effect == POST_EFFECT_AOE) 
                    continue;
	
                if (effect != POST_EFFECT_SOCKWARE && effect != POST_EFFECT_SHADOW)
                {
                    if (FAILED(ApplyEffect(effect, pScene)))
                    {
                        if (!HasNextEffect(effect))
                            RenderTargetRestore();
                        continue;
                    }
                }

				++validEffectCount;

                if (FAILED(SetEffectParameter(effect, pScene)))
                    continue;
				
                Draw(0, 0.f);
				for (int i = 0; i < 8; ++i)
					g_pd3dDevice->SetTexture(i, NULL);
            }

			if(! HasNextEffect(effect))
				break;
        }

		if (validEffectCount == 0)	//SetEffectParameter
		{
			RenderTargetRestore();
		}

        for (int i = 0; i < 8; ++i)
            g_pd3dDevice->SetTexture(i, NULL);


        hResult = S_OK;


Exit0 :
        if (FAILED(hResult))
            RenderTargetRestore();
        return hResult;
    }

	HRESULT KG3DPostRenderEffectManager::SetParentScene(KG3DScene* pScene)
	{
		m_lpParentScene = pScene;
		m_ShadowEffect.SetParentScene( pScene );

		return S_OK;
	}
	HRESULT KG3DPostRenderEffectManager::GetShadowMask(LPDIRECT3DTEXTURE9* ppTexture)
	{
		HRESULT hRetCode = E_FAIL;
		KG_PROCESS_ERROR(ppTexture);
		KG_PROCESS_ERROR(m_bEnable);
		//KG_PROCESS_ERROR(/*HasEffect(POST_EFFECT_SHADOW)*/g_cEngineOption.ModelShadowType > EM_MODEL_SHADOW_TYPE_LOW);//<Shadow Sign>
		KG_PROCESS_ERROR(HasEffect(POST_EFFECT_SHADOW));
		*ppTexture = m_ShadowEffect.GetDepthTexture();
		hRetCode = S_OK;
Exit0:
		return hRetCode;
	}
};

