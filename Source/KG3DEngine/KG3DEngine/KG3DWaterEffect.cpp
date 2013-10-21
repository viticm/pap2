#include "StdAfx.h"
#include "KG3DWaterEffect.h"
#include "KG3DEngineManager.h"
#include "KG3DTextureTable.h"
#include "KG3DGraphicsTool.h"
#include "KG3DSFX.h"
#include "KG3DSceneSceneEditor.h"
#include "KG3DModelTable.h"
#include "KG3DTerrainBlock.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace {

    inline LPDIRECT3DTEXTURE9 SafeGetTexture(KG3DTexture* pTexture)
    {
        if (!pTexture)
            return NULL;
        return pTexture->GetTexture();
    }

    inline void mul(D3DXCOLOR& src, const D3DCOLORVALUE& oth, bool bAlpha = false)
    {
        if (bAlpha)
            src.a *= oth.a;

        src.r *= oth.r;
        src.g *= oth.g;
        src.b *= oth.b;
    }

    inline D3DCOLORVALUE add(const D3DCOLORVALUE& c1, const D3DCOLORVALUE& c2)
    {
        D3DCOLORVALUE v = {c1.r + c2.r, c1.g + c2.g, c1.b + c2.b, c1.a + c2.a};
        return v;
    }

    inline float frac(float fValue)
    {
        return fValue - floor(fValue);
    }

    inline const char* SafeGetRetPath(const char filePath[]) 
    {
        char rootPath[MAX_PATH];
        g_GetRootPath(rootPath);

        char* pStr = strstr(const_cast<char*>(filePath), rootPath);
        size_t offset = 0;

        if (pStr == filePath) 
        {
            offset = strlen(rootPath);            
            if (filePath[offset] == '\\')
                ++offset;
        }

        return filePath + offset;
    }

    inline const TCHAR* ColorKeyName(int nDetail)
    {
        static TCHAR szName[128];
        _stprintf(szName, TEXT("WaterColor_%02d"), nDetail);
        return szName;
    }
}

KG3DWaterEffect::KG3DWaterEffect()
    : m_bEffectBegin(false),
      m_bPassBegin(false),
      m_lpWaterSurfaceTexture(NULL),
      m_lpRefractTexture(NULL),
      m_lpReflectTexture(NULL),
      m_lpWaveTexture(NULL),
      //m_lpEffectWater(NULL),
      m_lpWaterBumpTexture1(NULL),
      m_lpWaterBumpTexture2(NULL),
      m_lpWaterSprayTexure(NULL),
      m_vWaveParam(D3DXVECTOR4(0.02f, 5.f, 0.1f, 0.f)),
      m_vWaveParam2(D3DXVECTOR4(0.08f, 100.f, 0.f, 0.f)),
      m_vWaveParamUse(D3DXVECTOR4(0.02f, 5.f, 0.1f, 0.f)),
      m_vTextParam(D3DXVECTOR4(0.00101f, 0.0001f, 0.022f, 5000.f)),
      //m_fWaterFogStart(-5000.f),
      //m_fWaterFogEnd(5000.f),
      m_WaterSpecrColor(D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.f)),
      //m_WaterFogColor(D3DXCOLOR(0.3f, 0.5f, 0.61f, 1.f)),
      m_nDetail(WD_COUNT),
      m_dwLastActiveTime(0)
{
    for (int i = WD_LOWER; i < WD_COUNT; ++i)
        m_WaterColor[i] = D3DXCOLOR(0.121569f, 0.196078f, 0.266667f, 0.8f);

    _tcsncpy(m_szSurfaceTextPath, TEXT("data\\source\\maps_source\\texture\\水波纹\\001_001.anitexture"), sizeof(m_szSurfaceTextPath));
    _tcsncpy(m_szBump1TextPath, TEXT("data\\public\\defaultscene\\wavesbump.dds"), sizeof(m_szBump1TextPath));

	m_waterFogParam.vColor = D3DXCOLOR(0.3f, 0.5f, 0.61f, 1.f);
	m_waterFogParam.fStart = -5000.F;
	m_waterFogParam.fDensityDevidedBy_endMinusStart = 1.F / (5000.f - (-5000.f));
}

KG3DWaterEffect::~KG3DWaterEffect()
{
    UnInit();
}

HRESULT KG3DWaterEffect::SaveToFile(IIniFile* pIniFile)
{
    HRESULT hResult  = E_FAIL;

    KGLOG_PROCESS_ERROR(pIniFile);

    pIniFile->WriteString(TEXT("Water"), TEXT("SurfaceTexter"), SafeGetRetPath(m_szSurfaceTextPath));
    pIniFile->WriteString(TEXT("Water"), TEXT("BumpTexture"), SafeGetRetPath(m_szBump1TextPath));

    pIniFile->WriteMultiFloat(TEXT("Water"), TEXT("vTextParam"), m_vTextParam, 4);  
    pIniFile->WriteMultiFloat(TEXT("Water"), TEXT("vWaveParam"), m_vWaveParam, 4);
    pIniFile->WriteMultiFloat(TEXT("Water"), TEXT("vWaveParam2"), m_vWaveParam2, 4);

    for (int i = WD_LOWER; i < WD_COUNT; ++i)
        pIniFile->WriteMultiFloat(TEXT("Water"), ColorKeyName(i), m_WaterColor[i], 4);  

    pIniFile->WriteMultiFloat(TEXT("Water"), TEXT("WaterSpecrColor"), m_WaterSpecrColor, 4);
    pIniFile->WriteMultiFloat(TEXT("Water"), TEXT("WaterFogColor"), /*m_WaterFogColor*/m_waterFogParam.vColor, 4);

    pIniFile->WriteFloat(TEXT("Water"), TEXT("fFogStart"), m_waterFogParam.fStart);
	_ASSERTE(0 != m_waterFogParam.fDensityDevidedBy_endMinusStart);
	if(0 == m_waterFogParam.fDensityDevidedBy_endMinusStart)
		m_waterFogParam.fDensityDevidedBy_endMinusStart = FLT_MIN;
	FLOAT fFogEnd = 1.f / m_waterFogParam.fDensityDevidedBy_endMinusStart + m_waterFogParam.fStart;
    pIniFile->WriteFloat(TEXT("Water"), TEXT("fFogEnd"), fFogEnd);

    hResult = S_OK;
Exit0 :
    return hResult;
}

HRESULT KG3DWaterEffect::LoadFromFile(IIniFile* pIniFile)
{
    HRESULT hResult  = E_FAIL;

    KGLOG_PROCESS_ERROR(pIniFile);

    pIniFile->GetString(TEXT("Water"), TEXT("SurfaceTexter"), TEXT("data\\source\\maps_source\\texture\\水波纹\\001_001.anitexture"), m_szSurfaceTextPath, sizeof(m_szSurfaceTextPath));
    pIniFile->GetString(TEXT("Water"), TEXT("BumpTexture"), TEXT("data\\public\\defaultscene\\wavesbump.dds"), m_szBump1TextPath, sizeof(m_szBump1TextPath));

    pIniFile->GetMultiFloat(TEXT("Water"), TEXT("vTextParam"), m_vTextParam, 4);  
    pIniFile->GetMultiFloat(TEXT("Water"), TEXT("vWaveParam"), m_vWaveParam, 4);
    pIniFile->GetMultiFloat(TEXT("Water"), TEXT("vWaveParam2"), m_vWaveParam2, 4);
    m_vWaveParamUse = m_vWaveParam;

    for (int i = WD_LOWER; i < WD_COUNT; ++i)
        pIniFile->GetMultiFloat(TEXT("Water"), ColorKeyName(i), m_WaterColor[i], 4);  

    pIniFile->GetMultiFloat(TEXT("Water"), TEXT("WaterSpecrColor"), m_WaterSpecrColor, 4);
    pIniFile->GetMultiFloat(TEXT("Water"), TEXT("WaterFogColor"), /*m_WaterFogColor*/m_waterFogParam.vColor, 4);

	FLOAT fFogStart = -5000.0f;
    pIniFile->GetFloat(TEXT("Water"), TEXT("fFogStart"), -5000.f, &fFogStart);
	FLOAT fFogEnd = 5000.f;
    pIniFile->GetFloat(TEXT("Water"), TEXT("fFogEnd"), 5000.f, &fFogEnd);

	if (fFogEnd > fFogStart)	//否则，fStart和fDensityDevidedBy_endMinusStart维持默认
	{
		m_waterFogParam.fStart = fFogStart;
		m_waterFogParam.fDensityDevidedBy_endMinusStart = 1 / (fFogEnd - fFogStart);
	}
	

    hResult = S_OK;
Exit0 :
    return hResult;

}

LPCTSTR KG3DWaterEffect::GetWaterSurfaceTextureName()
{
    if (m_lpWaterSurfaceTexture)
        return m_lpWaterSurfaceTexture->m_scName.c_str();
    return m_szSurfaceTextPath;
}

LPCTSTR KG3DWaterEffect::GetWaterBumpTextureName()
{
    if (m_lpWaterBumpTexture1)
        return m_lpWaterBumpTexture1->m_scName.c_str();
    return m_szBump1TextPath;
}

HRESULT KG3DWaterEffect::SetWaterSurfaceTexture(LPCTSTR szFileName)
{
    _tcsncpy(m_szSurfaceTextPath, szFileName, sizeof(m_szSurfaceTextPath));
    SAFE_RELEASE(m_lpWaterSurfaceTexture);
    return g_cTextureTable.LoadResourceFromFile(szFileName, 0, TEXOP_RELOAD,  (IKG3DResourceBase **)&m_lpWaterSurfaceTexture);
}

HRESULT KG3DWaterEffect::SetWaterBumpTexture(LPCTSTR szFileName)
{
    _tcsncpy(m_szBump1TextPath, szFileName, sizeof(m_szBump1TextPath));
    SAFE_RELEASE(m_lpWaterBumpTexture1);
    return g_cTextureTable.LoadResourceFromFile(szFileName, 0, TEXOP_DISABLEDTX,  (IKG3DResourceBase **)&m_lpWaterBumpTexture1);
}

HRESULT KG3DWaterEffect::OnLostDevice()
{
    /*if (m_lpEffectWater)
        m_lpEffectWater->OnLostDevice();*/

    SAFE_RELEASE(m_lpWaveTexture);
    SAFE_RELEASE(m_lpReflectTexture);
    SAFE_RELEASE(m_lpRefractTexture);

    m_nDetail = WD_COUNT;

    return S_OK;
}

HRESULT KG3DWaterEffect::OnResetDevice()
{
   /* if (m_lpEffectWater)
        m_lpEffectWater->OnResetDevice();*/

    return S_OK;
}

HRESULT KG3DWaterEffect::Init()
{
    /*SAFE_RELEASE(m_lpEffectWater);
	LPD3DXBUFFER pBuffer = NULL;
	HRESULT hr = KG3DD3DXCreateEffectFromFile(g_pd3dDevice, WATER_SHADER_FILE, NULL, NULL, 0, NULL, &m_lpEffectWater, &pBuffer);
	if (FAILED(hr) && NULL != pBuffer)
	{
		KGLogPrintf(KGLOG_ERR, _T("water.fx编译错误: %s"), pBuffer->GetBufferPointer());
		SAFE_RELEASE(pBuffer);
	}
	return hr;*/
	return S_OK;
}

HRESULT KG3DWaterEffect::UnInit()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    m_nDetail = WD_COUNT;

	//SAFE_RELEASE(m_lpEffectWater);
	SAFE_RELEASE(m_lpWaterBumpTexture1);
	SAFE_RELEASE(m_lpWaterBumpTexture2);
	SAFE_RELEASE(m_lpWaterSurfaceTexture);
	SAFE_RELEASE(m_lpWaterSprayTexure);
	SAFE_RELEASE(m_lpWaveTexture);
	SAFE_RELEASE(m_lpReflectTexture);
    SAFE_RELEASE(m_lpRefractTexture);

	hr = DeleteAllSFX();
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KG3DWaterEffect::PrepareTexture(int nDetail)
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    KG_PROCESS_SUCCESS(m_nDetail == nDetail);

    switch (nDetail)
    {
    case WD_LOWER :
        {
            /* because in the editor mode, one of the scenes have multiple output window 
               different output window may be have different vater detail, 
               so each render frame are required to switch water detail, so do not release
               the texture and surface which is not need */
            if (g_bClient)
            {
                SAFE_RELEASE(m_lpWaterBumpTexture1);
                SAFE_RELEASE(m_lpWaterBumpTexture2);
                SAFE_RELEASE(m_lpReflectTexture);
                SAFE_RELEASE(m_lpRefractTexture);
                SAFE_RELEASE(m_lpWaveTexture);
                SAFE_RELEASE(m_lpWaterSprayTexure);
            }

            if (!m_lpWaterSurfaceTexture)   
            {
                hRetCode = g_cTextureTable.LoadResourceFromFile(m_szSurfaceTextPath, 0, TEXOP_RELOAD, (IKG3DResourceBase **)&m_lpWaterSurfaceTexture);
                KGLOG_COM_PROCESS_ERROR(hRetCode);
            }
        }
        break;
    case WD_MEDIUM :
        {
            if (g_bClient)
            {
                SAFE_RELEASE(m_lpWaterBumpTexture2);
                SAFE_RELEASE(m_lpReflectTexture);
                SAFE_RELEASE(m_lpWaveTexture);
                SAFE_RELEASE(m_lpWaterSprayTexure);
            }

            if (!m_lpWaterSurfaceTexture)   
            {
                hRetCode = g_cTextureTable.LoadResourceFromFile(m_szSurfaceTextPath, 0, TEXOP_RELOAD, (IKG3DResourceBase **)&m_lpWaterSurfaceTexture);
                KGLOG_COM_PROCESS_ERROR(hRetCode);
            }

            if (!m_lpRefractTexture)
            {
                hRetCode = g_pd3dDevice->CreateTexture(g_cEngineOption.nWaterRefrTextSize, g_cEngineOption.nWaterRefrTextSize, 1, D3DUSAGE_RENDERTARGET,
                    D3DFMT_A8R8G8B8,
                    D3DPOOL_DEFAULT,
                    &m_lpRefractTexture, NULL);
                KGLOG_COM_PROCESS_ERROR(hRetCode);
            }

            if (!m_lpWaterBumpTexture1)
            {
                hRetCode = g_cTextureTable.LoadResourceFromFile(m_szBump1TextPath, 0, TEXOP_DISABLEDTX, (IKG3DResourceBase **)&m_lpWaterBumpTexture1);
                KGLOG_COM_PROCESS_ERROR(hRetCode);
            }
        }
        break;
    case WD_HIGH :
    case WD_VERY_HIGH :
        {
            if (g_bClient)
            {
                SAFE_RELEASE(m_lpWaterBumpTexture2);
                SAFE_RELEASE(m_lpWaterSurfaceTexture);
                SAFE_RELEASE(m_lpWaterSprayTexure);
            }

            if (!m_lpReflectTexture)
            {
                hRetCode = g_pd3dDevice->CreateTexture( g_cEngineOption.nWaterReflTextSize, g_cEngineOption.nWaterReflTextSize,1, D3DUSAGE_RENDERTARGET,
                    D3DFMT_A8R8G8B8,
                    D3DPOOL_DEFAULT,
                    &m_lpReflectTexture, NULL);
                KGLOG_COM_PROCESS_ERROR(hRetCode);
            }

            if (!m_lpRefractTexture)
            {
                hRetCode = g_pd3dDevice->CreateTexture( g_cEngineOption.nWaterRefrTextSize, g_cEngineOption.nWaterRefrTextSize, 1, D3DUSAGE_RENDERTARGET,
                    D3DFMT_A8R8G8B8,
                    D3DPOOL_DEFAULT,
                    &m_lpRefractTexture, NULL);
                KGLOG_COM_PROCESS_ERROR(hRetCode);
            }

            if (!m_lpWaveTexture)
            {
                hRetCode = g_pd3dDevice->CreateTexture( 512, 512,1, D3DUSAGE_RENDERTARGET,
                    D3DFMT_A8R8G8B8,
                    D3DPOOL_DEFAULT,
                    &m_lpWaveTexture,NULL);
                KGLOG_COM_PROCESS_ERROR(hRetCode);
            }

            if (!m_lpWaterBumpTexture1)
            {
                hRetCode = g_cTextureTable.LoadResourceFromFile(m_szBump1TextPath, 0, TEXOP_DISABLEDTX, (IKG3DResourceBase **)&m_lpWaterBumpTexture1);
                KGLOG_COM_PROCESS_ERROR(hRetCode);
            }
        }
        break;
    case WD_FULL_EFFECTIVE :
        {
            if (g_bClient)
            {
                SAFE_RELEASE(m_lpWaterBumpTexture2);
                SAFE_RELEASE(m_lpWaterSurfaceTexture);
                SAFE_RELEASE(m_lpWaterSprayTexure);
            }

            if (!m_lpReflectTexture)
            {
                hRetCode = g_pd3dDevice->CreateTexture( g_cEngineOption.nWaterReflTextSize, g_cEngineOption.nWaterReflTextSize,1, D3DUSAGE_RENDERTARGET,
                    D3DFMT_A8R8G8B8,
                    D3DPOOL_DEFAULT,
                    &m_lpReflectTexture, NULL);
                KGLOG_COM_PROCESS_ERROR(hRetCode);
            }

            if (!m_lpRefractTexture)
            {
                hRetCode = g_pd3dDevice->CreateTexture( g_cEngineOption.nWaterRefrTextSize, g_cEngineOption.nWaterRefrTextSize, 1, D3DUSAGE_RENDERTARGET,
                    D3DFMT_A8R8G8B8,
                    D3DPOOL_DEFAULT,
                    &m_lpRefractTexture, NULL);
                KGLOG_COM_PROCESS_ERROR(hRetCode);
            }

            if (!m_lpWaveTexture)
            {
                hRetCode = g_pd3dDevice->CreateTexture( 512, 512,1, D3DUSAGE_RENDERTARGET,
                    D3DFMT_A8R8G8B8,
                    D3DPOOL_DEFAULT,
                    &m_lpWaveTexture,NULL);
                KGLOG_COM_PROCESS_ERROR(hRetCode);
            }

            if (!m_lpWaterBumpTexture1)
            {
                hRetCode = g_cTextureTable.LoadResourceFromFile(m_szBump1TextPath, 0, TEXOP_DISABLEDTX, (IKG3DResourceBase **)&m_lpWaterBumpTexture1);
                KGLOG_COM_PROCESS_ERROR(hRetCode);
            }
        }
        break;
    default :
        ASSERT(false);
        KG_PROCESS_ERROR(false);
        break;
    }

    m_nDetail = nDetail;

Exit1 :
    hResult = S_OK;
Exit0 :
    return hResult;
}


HRESULT KG3DWaterEffect::BeginEffect(KG3DSceneSceneEditor* pScene, BOOL bUnderWater)
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    D3DXVECTOR4 vCameraPos = D3DXVECTOR4(0.f, 0.f, 0.f, 0.f);
    D3DXVECTOR4 vTextParam;
    D3DXVECTOR4 vLightDir;
    D3DXCOLOR   vWaterColor;
    D3DXCOLOR   vSpecColor = m_WaterSpecrColor;
    DWORD       AmbientColor;

    D3DXMATRIX matWorld;
    D3DXMATRIX matView;
    D3DXMATRIX matProj;
    D3DXMATRIX matWorldView;
    D3DXMATRIX matWorldViewProj;

    UINT uPass = 0;
    int nWaterDetail = g_cEngineOption.nWaterDetail;

    KG3DCamera* pCamera = g_cGraphicsTool.GetCamera();
    KG3DLightParam lightParam;

	LPD3DXEFFECT pWaterEffect = g_GetDefaultShaderGroup().GetDefaultEffect(DEFST_WATER);
    KG_PROCESS_ERROR(pWaterEffect);
    KG_PROCESS_ERROR(pScene);

    lightParam = g_cGraphicsTool.GetSunLightParam(*pScene);

    g_pd3dDevice->GetRenderState(D3DRS_AMBIENT, &AmbientColor);
    g_pd3dDevice->GetTransform(D3DTS_WORLD, &matWorld);
    g_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
    g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProj);

    clip(nWaterDetail, static_cast<int>(WD_LOWER), static_cast<int>(WD_FULL_EFFECTIVE));

    hRetCode = PrepareTexture(nWaterDetail);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    if (pCamera)
        pCamera->GetPosition(reinterpret_cast<D3DXVECTOR3*>(&vCameraPos));
    vLightDir = D3DXVECTOR4(lightParam.dir.x, lightParam.dir.z, lightParam.dir.y, 1.f);

    vWaterColor = m_WaterColor[nWaterDetail];

    m_vWaveParamUse.x = m_vWaveParam.x;
    m_vWaveParamUse.y = m_vWaveParam.y;
    m_vWaveParamUse.z = frac(static_cast<float>(static_cast<double>(g_cGraphicsTool.GetNowTime()) * 0.0001f * m_vWaveParam.z));
    m_vWaveParamUse.w = frac(static_cast<float>(static_cast<double>(g_cGraphicsTool.GetNowTime()) * 0.0001f * m_vWaveParam.w));

    mul(vWaterColor, add(lightParam.diffuse, lightParam.sceneAmbient));
    mul(vSpecColor, lightParam.specular);

    matWorldView = matWorld * matView;
    matWorldViewProj = matWorldView * matProj;

	hRetCode = pWaterEffect->SetTechnique(TEXT("water"));
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = pWaterEffect->SetMatrix("matWorldViewProj", &matWorldViewProj);
    hRetCode = pWaterEffect->SetMatrix("matWorldView", &matWorldView);
    hRetCode = pWaterEffect->SetVector("vCameraPos", &vCameraPos);
    hRetCode = pWaterEffect->SetVector("vWaveParam", &m_vWaveParamUse);
    hRetCode = pWaterEffect->SetVector("vWaveParam2", &m_vWaveParam2);
    hRetCode = pWaterEffect->SetVector("vTextParam", &m_vTextParam);
    hRetCode = pWaterEffect->SetVector("vLightDir",  &vLightDir);

    hRetCode = pWaterEffect->SetVector("WaterColor", reinterpret_cast<D3DXVECTOR4*>(&vWaterColor));
    hRetCode = pWaterEffect->SetVector("SpecrColor", reinterpret_cast<D3DXVECTOR4*>(&vSpecColor));

    switch (nWaterDetail)
    {
    case WD_LOWER :
        if (m_lpWaterSurfaceTexture)
        {
            hRetCode = m_lpWaterSurfaceTexture->Update(0, g_cGraphicsTool.GetNowTime());
            hRetCode = g_pd3dDevice->SetTexture(0, m_lpWaterSurfaceTexture->GetTexture());
        }
        break;
    case WD_MEDIUM :
        hRetCode = g_pd3dDevice->SetTexture(0, SafeGetTexture(m_lpWaterBumpTexture1));
        if (m_lpWaterSurfaceTexture)
        {
            hRetCode = m_lpWaterSurfaceTexture->Update(0, g_cGraphicsTool.GetNowTime());
            hRetCode = g_pd3dDevice->SetTexture(1, m_lpWaterSurfaceTexture->GetTexture());
        }
        hRetCode = g_pd3dDevice->SetTexture(2, m_lpRefractTexture);
        break;
    case WD_HIGH :
    case WD_VERY_HIGH :
        hRetCode = g_pd3dDevice->SetTexture(0, SafeGetTexture(m_lpWaterBumpTexture1));
        hRetCode = g_pd3dDevice->SetTexture(1, m_lpReflectTexture);
        hRetCode = g_pd3dDevice->SetTexture(2, m_lpRefractTexture);
        hRetCode = g_pd3dDevice->SetTexture(3, m_lpWaveTexture);
        break;
    case WD_FULL_EFFECTIVE :
        hRetCode = g_pd3dDevice->SetTexture(0, SafeGetTexture(m_lpWaterBumpTexture1));
        hRetCode = g_pd3dDevice->SetTexture(1, m_lpReflectTexture);
        hRetCode = g_pd3dDevice->SetTexture(2, m_lpRefractTexture);
        hRetCode = g_pd3dDevice->SetTexture(3, m_lpWaveTexture);
        //hRetCode = g_pd3dDevice->SetTexture(4, SafeGetTexture(m_lpWaterSprayTexure));
        break;
    default :
        KGLOG_PROCESS_ERROR(false);
        break;
    }

    hRetCode = pWaterEffect->Begin(&uPass, 0);
    KG_COM_PROCESS_ERROR(hRetCode);
    m_bEffectBegin = true;

    hRetCode = pWaterEffect->BeginPass(bUnderWater ? nWaterDetail + WD_COUNT : nWaterDetail);
    KG_COM_PROCESS_ERROR(hRetCode);
    m_bPassBegin = true;

    if (false)
    {
        D3DXSaveTextureToFile(TEXT("D:\\refl.bmp"), D3DXIFF_BMP, m_lpReflectTexture, NULL);
        D3DXSaveTextureToFile(TEXT("D:\\refr.bmp"), D3DXIFF_BMP, m_lpRefractTexture, NULL);
    }
    

    hResult = S_OK;
Exit0 :
    if (FAILED(hResult))
    {
        if (m_bPassBegin)
            pWaterEffect->EndPass();
        m_bPassBegin = false;

        if (m_bEffectBegin)
            pWaterEffect->End();
        m_bEffectBegin = false;
    }

    return hResult;
}

HRESULT KG3DWaterEffect::EndEffect()
{
    HRESULT hResult  = E_FAIL;
    
	LPD3DXEFFECT pWaterEffect = g_GetDefaultShaderGroup().GetDefaultEffect(DEFST_WATER);
    KG_PROCESS_ERROR(pWaterEffect);

    if (m_bPassBegin)
        pWaterEffect->EndPass();
    m_bPassBegin = false;

    if (m_bEffectBegin)
        pWaterEffect->End();
    m_bEffectBegin = false;
   
    hResult = S_OK;
Exit0 :
    return hResult;
}

HRESULT KG3DWaterEffect::ReloadEffect()
{
	/*SAFE_RELEASE(m_lpEffectWater);
	LPD3DXBUFFER pBuffer = NULL;
	HRESULT hr = KG3DD3DXCreateEffectFromFile(g_pd3dDevice, WATER_SHADER_FILE, NULL, NULL, 0, NULL, &m_lpEffectWater, &pBuffer);
	if (FAILED(hr) && NULL != pBuffer)
	{
		KGLogPrintf(KGLOG_ERR, _T("water.fx编译错误: %s"), pBuffer->GetBufferPointer());
		SAFE_RELEASE(pBuffer);
	}

	return hr;*/

	return S_OK;
}

HRESULT KG3DWaterEffect::DeleteAllSFX()
{
	map<PVOID,SFXControler*>::iterator i = m_mapWaveSFX.begin();
	map<PVOID,SFXControler*>::iterator iend = m_mapWaveSFX.end();
	while (i!=iend)
	{
		SFXControler* pCon = i->second;

		KG3DSFX* pSFX = pCon->m_lpSFX;

		SAFE_RELEASE(pSFX);
		pCon->m_SFXOnce.UnInit();

		m_SFXConPool.FreeResource(pCon);
		
		i++;
	}
	m_mapWaveSFX.clear();

	return S_OK;
} 

HRESULT KG3DWaterEffect::DeleteSFX(PVOID pUser)
{
	map<PVOID,SFXControler*>::iterator i = m_mapWaveSFX.find(pUser);
	if(i!=m_mapWaveSFX.end())
	{
		SFXControler* pCon = i->second;

		KG3DSFX* pSFX = pCon->m_lpSFX;

		SAFE_RELEASE(pSFX);
		pCon->m_SFXOnce.UnInit();

		m_SFXConPool.FreeResource(pCon);

		m_mapWaveSFX.erase(i);

		return S_OK;
	}

	return E_FAIL;
}


HRESULT KG3DWaterEffect::FrameMove()
{
	DWORD dwNowTime = g_cGraphicsTool.GetNowTime();

    map<PVOID,SFXControler*>::iterator i = m_mapWaveSFX.begin();
    map<PVOID,SFXControler*>::iterator iend = m_mapWaveSFX.end();

    while (i!=iend)
    {
        SFXControler* pCon = i->second;

        if(dwNowTime - pCon->m_dwLastUpdateTime > 2000)
        {
            KG3DSFX* pSFX = pCon->m_lpSFX;

            SAFE_RELEASE(pSFX);
            pCon->m_SFXOnce.UnInit();

            m_SFXConPool.FreeResource(pCon);

            i = m_mapWaveSFX.erase(i);
        }
        else
            i++;
    }

	return S_OK;
}

HRESULT KG3DWaterEffect::CheckEntityAndWater(PVOID pUser,D3DXVECTOR3& vPosition,float fHeight,KG3DSceneSceneEditor* pEditor)
{
	HRESULT hr = E_FAIL;
	DWORD dwNowTime = g_cGraphicsTool.GetNowTime();
	if(g_dwRenderCount%10!=8)
	{
		return S_OK;
	}

	D3DXVECTOR3 vWater;
	hr = pEditor->GetWaterHeight(&vWater,&vPosition);
    KG_COM_PROCESS_ERROR(hr);


	float K = vPosition.y - vWater.y;
	{
		SFXControler* pControl = NULL;
		map<PVOID,SFXControler*>::iterator i = m_mapWaveSFX.find(pUser);
		if(i==m_mapWaveSFX.end())
		{
			pControl = m_SFXConPool.RequestResource();
			if(pControl)
			{
				m_mapWaveSFX[pUser] = pControl;
			}
		}
		else
		{
			pControl = i->second;
		}

		pControl->m_lpUser = pUser;
		//////////////////////////////////////////////////////////////////////////
		D3DXVECTOR3 V = vPosition - pControl->m_vUserLastPosition;
		DWORD dwTime = dwNowTime - pControl->m_dwLastUpdateTime;
		float L_Y = min(100.0F,abs(V.y) * 10 / dwTime);
		D3DXVECTOR2 XZ(V.x,V.z);

		float L = min(100.0F,D3DXVec2Length(&XZ) * 10 / dwTime);//换算到米每秒

		pControl->m_fUserSpeedY  = (L_Y - pControl->m_fUserSpeedY)*0.5F + pControl->m_fUserSpeedY;
		pControl->m_fUserSpeedXZ = (L - pControl->m_fUserSpeedXZ)*0.5F + pControl->m_fUserSpeedXZ;

		pControl->m_vUserLastPosition = vPosition;
		pControl->m_dwLastUpdateTime = dwNowTime;

		if( (K >= -fHeight) && ( K<=0 ))
		{
			pControl->m_dwLastInWaterTime = dwNowTime;

			if(!pControl->m_lpSFX)
			{
				KG3DSFX* pNewSFX = NULL;
				HRESULT hr = g_cModelTable.LoadResourceFromFile("data\\source\\other\\特效\\系统\\SFX\\水晕.Sfx",0,MLO_MULTI,(IKG3DResourceBase**)&pNewSFX);
				if(FAILED(hr))
				{
					SAFE_RELEASE(pNewSFX);
					KGLOG_COM_PROCESS_ERROR(hr);
				}
				if(pNewSFX)
				{
					pNewSFX->PlayAnimation(SFX_AP_LOOP);
					pControl->m_lpSFX = pNewSFX;
					pNewSFX->SetUserData(dwNowTime);
				}
			}

            if (pControl->m_lpSFX)
			    pControl->m_lpSFX->SetTranslation(&vWater);

			//////////////////////////////////////////////////////////////////////////
			if(dwNowTime - pControl->m_dwLastAddSfxTime > 300)
			{
				if(pControl->m_fUserSpeedY >= 10)
				{
					pControl->m_SFXOnce.AddSFX(WST_BIG,vWater);
					pControl->m_dwLastAddSfxTime = dwNowTime;
				}
				else if(pControl->m_fUserSpeedY >= 3 || pControl->m_fUserSpeedXZ >= 3.5)
				{
					pControl->m_SFXOnce.AddSFX(WST_SMALL,vWater);
					pControl->m_dwLastAddSfxTime = dwNowTime;
				}
			}
		}
		else
		{
			if(dwNowTime - pControl->m_dwLastInWaterTime > 3000)
			{
				SAFE_RELEASE(pControl->m_lpSFX);
			}
		}
	}

	hr = S_OK;
Exit0:
	return hr;
}


HRESULT KG3DWaterEffect::RenderWaveSFX(BOOL bWave,BOOL bOnce)
{
	DWORD dwStartTime = timeGetTime();
	DWORD dwCount = 0;
	{
		map<PVOID,SFXControler*>::iterator i = m_mapWaveSFX.begin();
		map<PVOID,SFXControler*>::iterator iend = m_mapWaveSFX.end();
		while (i!=iend)
		{
			SFXControler* pCon = i->second;

			if(bWave)
			{
				KG3DSFX* pSFX = pCon->m_lpSFX;

				if(pSFX)
				{
					pSFX->FrameMove();
					pSFX->Render(0,NULL);
					dwCount++;
				}

			}

			if(bOnce)
			{
				pCon->m_SFXOnce.FrameMove(dwCount);

				pCon->m_SFXOnce.Render();
			}


			i++;
		}
	}

	DWORD dwCost = timeGetTime() - dwStartTime;
	if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost > 5)
	{
		KGLogPrintf(KGLOG_INFO,"TimeOptimize %d KG3DWaterEffect::RenderWaveSFX %d",dwCost,dwCount);
	}
	return S_OK;
}

HRESULT KG3DWaterEffect::SetFogInfo( float fStart, float fEnd )
{
	//m_fWaterFogStart = fStart; 
	//m_fWaterFogEnd = fEnd; 

	if (fEnd <= fStart)
	{
		return E_FAIL;
	}

	m_waterFogParam.fStart = fStart;
	m_waterFogParam.fDensityDevidedBy_endMinusStart  = 1 / (fEnd - fStart);

	return S_OK;
}

HRESULT KG3DWaterEffect::GetFogInfo( float& fStart, float& fEnd )
{
	//fStart = m_fWaterFogStart; fEnd = m_fWaterFogEnd; return S_OK;
	fStart = m_waterFogParam.fStart;
	_ASSERTE(0 != m_waterFogParam.fDensityDevidedBy_endMinusStart);
	if(0 == m_waterFogParam.fDensityDevidedBy_endMinusStart)
		m_waterFogParam.fDensityDevidedBy_endMinusStart = FLT_MIN;
	fEnd = 1 / (m_waterFogParam.fDensityDevidedBy_endMinusStart) + m_waterFogParam.fStart;

	return S_OK;
}

HRESULT KG3DWaterEffect::BeginEffectForRiver(KG3DSceneSceneEditor* pScene, BOOL bUnderWater)
{
	HRESULT hResult  = E_FAIL;
	HRESULT hRetCode = E_FAIL;

	//D3DXVECTOR4 vCameraPos = D3DXVECTOR4(0.f, 0.f, 0.f, 0.f);
	//D3DXVECTOR4 vTextParam;
	//D3DXVECTOR4 vLightDir;
	D3DXCOLOR   vWaterColor;
	//D3DXCOLOR   vSpecColor = m_WaterSpecrColor;
	DWORD       AmbientColor;

	D3DXMATRIX matWorld;
	D3DXMATRIX matView;
	D3DXMATRIX matProj;
	D3DXMATRIX matWorldView;
	D3DXMATRIX matWorldViewProj;

	UINT uPass = 0;
	//int nWaterDetail = g_cEngineOption.nWaterDetail;

	//KG3DCamera* pCamera = g_cGraphicsTool.GetCamera();
	KG3DLightParam lightParam;

	LPD3DXEFFECT pWaterEffect = g_GetDefaultShaderGroup().GetDefaultEffect(DEFST_WATER);
	KG_PROCESS_ERROR(pWaterEffect);
	KG_PROCESS_ERROR(pScene);

	lightParam = g_cGraphicsTool.GetSunLightParam(*pScene);

	g_pd3dDevice->GetRenderState(D3DRS_AMBIENT, &AmbientColor);
	g_pd3dDevice->GetTransform(D3DTS_WORLD, &matWorld);
	g_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
	g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProj);

//	clip(nWaterDetail, static_cast<int>(WD_LOWER), static_cast<int>(WD_FULL_EFFECTIVE));

	hRetCode = PrepareTexture(WD_LOWER);
	KGLOG_COM_PROCESS_ERROR(hRetCode);

	//if (pCamera)
		//pCamera->GetPosition(reinterpret_cast<D3DXVECTOR3*>(&vCameraPos));
	//vLightDir = D3DXVECTOR4(lightParam.dir.x, lightParam.dir.z, lightParam.dir.y, 1.f);

	vWaterColor = m_WaterColor[WD_LOWER];

	m_vWaveParamUse.x = m_vWaveParam.x;
	m_vWaveParamUse.y = m_vWaveParam.y;
	m_vWaveParamUse.z = frac(static_cast<float>(static_cast<double>(g_cGraphicsTool.GetNowTime()) * 0.0001f * m_vWaveParam.z));
	m_vWaveParamUse.w = frac(static_cast<float>(static_cast<double>(g_cGraphicsTool.GetNowTime()) * 0.0001f * m_vWaveParam.w));

	mul(vWaterColor, add(lightParam.diffuse, lightParam.sceneAmbient));
	//mul(vSpecColor, lightParam.specular);

	matWorldView = matWorld * matView;
	matWorldViewProj = matWorldView * matProj;

	hRetCode = pWaterEffect->SetTechnique(TEXT("water"));
	KGLOG_COM_PROCESS_ERROR(hRetCode);

	hRetCode = pWaterEffect->SetMatrix("matWorldViewProj", &matWorldViewProj);
	hRetCode = pWaterEffect->SetMatrix("matWorldView", &matWorldView);
	//hRetCode = pWaterEffect->SetVector("vCameraPos", &vCameraPos);
	hRetCode = pWaterEffect->SetVector("vWaveParam", &m_vWaveParamUse);
	//hRetCode = pWaterEffect->SetVector("vWaveParam2", &m_vWaveParam2);
	//hRetCode = pWaterEffect->SetVector("vTextParam", &m_vTextParam);
	//hRetCode = pWaterEffect->SetVector("vLightDir",  &vLightDir);

	hRetCode = pWaterEffect->SetVector("WaterColor", reinterpret_cast<D3DXVECTOR4*>(&vWaterColor));
	//hRetCode = pWaterEffect->SetVector("SpecrColor", reinterpret_cast<D3DXVECTOR4*>(&vSpecColor));

	if (m_lpWaterSurfaceTexture)
	{
		hRetCode = m_lpWaterSurfaceTexture->Update(0, g_cGraphicsTool.GetNowTime());
		hRetCode = g_pd3dDevice->SetTexture(0, m_lpWaterSurfaceTexture->GetTexture());
	}


	hRetCode = pWaterEffect->Begin(&uPass, 0);
	KG_COM_PROCESS_ERROR(hRetCode);
	m_bEffectBegin = true;

	hRetCode = pWaterEffect->BeginPass(10);//bUnderWater ? nWaterDetail + WD_COUNT : nWaterDetail
	KG_COM_PROCESS_ERROR(hRetCode);
	m_bPassBegin = true;


	hResult = S_OK;
Exit0 :
	if (FAILED(hResult))
	{
		if (m_bPassBegin)
			pWaterEffect->EndPass();
		m_bPassBegin = false;

		if (m_bEffectBegin)
			pWaterEffect->End();
		m_bEffectBegin = false;
	}

	return hResult;
}
KG3DWaterEffect::SFX_Container::SFX_Container()
{

}

KG3DWaterEffect::SFX_Container::~SFX_Container()
{
	UnInit();
}

HRESULT KG3DWaterEffect::SFX_Container::FrameMove(DWORD& dwCount)
{
	DWORD dwNowTime = g_cGraphicsTool.GetNowTime();

	list<KG3DSFX*>::iterator i = m_listSFX.begin();
	list<KG3DSFX*>::iterator iend = m_listSFX.end();
	while (i!=iend)
	{
		KG3DSFX* pSFX = *i;

		if (pSFX->m_nPlayFinishFlag || (dwNowTime - pSFX->GetUserData() > 4000))
		{
			SAFE_RELEASE(pSFX);
			i = m_listSFX.erase(i);
		}
		else
		{
			dwCount++;
			pSFX->FrameMove();
			i++;
		}
	}

	return S_OK;
}

HRESULT KG3DWaterEffect::SFX_Container::Render()
{
	list<KG3DSFX*>::iterator i = m_listSFX.begin();
	list<KG3DSFX*>::iterator iend = m_listSFX.end();
	while (i!=iend)
	{
		KG3DSFX* pSFX = *i;

		if(pSFX)
		{
			pSFX->Render(0,NULL);
		}

		i++;
	}

	return S_OK;
}
HRESULT KG3DWaterEffect::SFX_Container::UnInit()
{
	list<KG3DSFX*>::iterator i = m_listSFX.begin();
	list<KG3DSFX*>::iterator iend = m_listSFX.end();
	while (i!=iend)
	{
		KG3DSFX* pSFX = *i;
		SAFE_RELEASE(pSFX);
		i++;
	}
	m_listSFX.clear();
	return S_OK;
}

namespace {

    LPCSTR WaterSfxFileName[WST_COUNT] = {
        "data\\source\\other\\特效\\系统\\SFX\\其他\\S_水花01.Sfx",
        "data\\source\\other\\特效\\系统\\SFX\\其他\\S_水花03.Sfx"
    };   
}

HRESULT KG3DWaterEffect::SFX_Container::AddSFX(DWORD dwType, D3DXVECTOR3& vPos)
{
    HRESULT hReslult = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    KG3DSFX* pSfx = NULL;

    KG_PROCESS_ERROR(dwType >= WST_BIG && dwType < WST_COUNT);

    hRetCode = g_cModelTable.LoadResourceFromFile(WaterSfxFileName[dwType], 0, MLO_MULTI,(IKG3DResourceBase**)&pSfx);
    KG_COM_PROCESS_ERROR(hRetCode);

    pSfx->SetTranslation(&vPos);
    pSfx->PlayAnimation(SFX_AP_ONE_TIME);
	pSfx->SetUserData(g_cGraphicsTool.GetNowTime());
    m_listSFX.push_back(pSfx);

    hReslult = S_OK;
Exit0 :
	return hReslult;
}
