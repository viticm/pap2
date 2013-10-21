#include "StdAfx.h"
#include "KG3DTerrainBlock.h"
#include "KG3DTemplates.h"
#include "KG3DSceneBlock.h"
#include "KG3DTexture.h"
#include "KG3DHeightMap.h"
#include "KG3DTerrainBlockGrass.h"
#include "KG3DMemoryFile.h"
#include "KG3DSceneDataInterface.h"
#include "KG3DTextureTable.h"
#include "KG3DTerrainBlockModifier.h"
#include "KG3DSceneOutDoorSpaceMgr.h"
#include "KG3DSFXProjectionTextureMgr.h"
#include "KG3DTerrainRoad.h"
#include "Terrain/KG3DTerrainRiver.h"
#include "Terrain/KG3DGridHeightGetter.h"
#include "KG3DEngineManager.h"
#include "KG3DResourceDelayReleaseMgr.h"
#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

TKG3DResourcePool<KG3DTerrainBlockShareVB> g_cTerrainBlockShareVB;
TKG3DResourcePool<KG3DTerrainBlockWaterShareVB> g_cTerrainBlockWaterShareVB;
TKG3DResourcePool<KG3DTerrainBlockBlendFactorShareVB>g_cTerrainBlockBlendFactorShareVB;

TKG3DResourcePool<KG3DTerrainBlock>g_cTerrainBlockPool;
TKG3DResourcePool<KG3DTerrainBlockBlendFactor> g_cTerrainBlockBlendFactorPool;
TKG3DResourcePool<KG3DTerrainBlockWaterVertexs> g_cTerrainWaterVertexPool;
extern BOOL g_bClient;
extern KG3DEngineManager           g_cEngineManager;
//////////////////////////////////////////////////////////////////////////
extern TKG3DResourcePool<KG3DSceneZone> g_cSceneZonePool;
extern TKG3DResourcePool<KG3DSceneSection> g_cSceneSectionPool;
extern TKG3DResourcePool<KG3DSceneRegion> g_cSceneRegionPool;
extern TKG3DResourcePool<KG3DEntityObjectShell>g_cObjectShellPool;

extern KG3DEngineOption g_cEngineOption;			//客户端使用的选项
extern int g_nLoadCount;
extern int g_nGrassVertexBufferFillPerFrame;

extern DWORD g_dwMainThreadID;//渲染线程ID
extern TKG3DResourcePool<KG3D_PointLight_Info>g_cPointLightInfoPool;

HRESULT InitTerrainBlockStaticResource()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    BOOL bInitTerrainBlockShareVB = FALSE;
    BOOL bInitTerrainBlockWaterShareVB = FALSE;
    BOOL bInitTerrainBlockBlendFactorShareVB = FALSE;
    BOOL bInitTerrainBlockGrassShareVBHighPool = FALSE;
    BOOL bInitTerrainBlockModifier = FALSE;

	hr = g_cTerrainBlockShareVB.Init(600,3000);
    KGLOG_COM_PROCESS_ERROR(hr);

    bInitTerrainBlockShareVB = TRUE;

	hr = g_cTerrainBlockWaterShareVB.Init(600,3000);
    KGLOG_COM_PROCESS_ERROR(hr);

    bInitTerrainBlockWaterShareVB = TRUE;

    hr = g_cTerrainBlockBlendFactorShareVB.Init(1200,3000);
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_cTerrainBlockGrassShareVBHighPool.Init(300,3000);
    KGLOG_COM_PROCESS_ERROR(hr);

    bInitTerrainBlockGrassShareVBHighPool = TRUE;

	if(!g_bClient)
	{
		hr = g_cTerrainBlockModifier.Init(); //g_cTerrainBlockModifier只在编辑器里用~`所以在客户端不需要始初化
		KGLOG_COM_PROCESS_ERROR(hr);
	}
    bInitTerrainBlockModifier = TRUE;

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (bInitTerrainBlockModifier)
        {
            hr = g_cTerrainBlockModifier.UnInit();
            KGLOG_COM_CHECK_ERROR(hr);
        }
        if (bInitTerrainBlockGrassShareVBHighPool)
        {
            hr = g_cTerrainBlockGrassShareVBHighPool.UnInit();
            KGLOG_COM_CHECK_ERROR(hr);
        }
        if (bInitTerrainBlockBlendFactorShareVB)
        {
            hr = g_cTerrainBlockBlendFactorShareVB.UnInit();
            KGLOG_COM_CHECK_ERROR(hr);
        }
        if (bInitTerrainBlockWaterShareVB)
        {
            hr = g_cTerrainBlockWaterShareVB.UnInit();
            KGLOG_COM_CHECK_ERROR(hr);
        }
        if (bInitTerrainBlockShareVB)
        {
            hr = g_cTerrainBlockShareVB.UnInit();
            KGLOG_COM_CHECK_ERROR(hr);
        }
    }
	return hrResult;
}

HRESULT UnInitTerrainBlockStaticResource()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

	hr = g_cTerrainBlockModifier.UnInit();
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_cSceneZonePool.UnInit();
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_cSceneSectionPool.UnInit();
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_cSceneRegionPool.UnInit();
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_cTerrainBlockPool.UnInit();
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_cTerrainBlockGrassPool.UnInit();
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_cTerrainBlockShareVB.UnInit();
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_cTerrainBlockWaterShareVB.UnInit();
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_cTerrainBlockBlendFactorShareVB.UnInit();
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_cTerrainBlockBlendFactorPool.UnInit(); 
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_cTerrainWaterVertexPool.UnInit();
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_cTerrainBlockGrassShareVBHighPool.UnInit();
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_cObjectShellPool.UnInit();
	KGLOG_COM_PROCESS_ERROR(hr);

	hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT OnLostDeviceTerrainBlockStaticResource()
{
    HRESULT hr = E_FAIL;

	hr = g_cTerrainBlockShareVB.OnLostDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_cTerrainBlockWaterShareVB.OnLostDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_cTerrainBlockBlendFactorShareVB.OnLostDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_cTerrainBlockGrassShareVBHighPool.OnLostDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_cTerrainBlockModifier.OnLostDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_cTerrainBlockPool.OnLostDevice();
	KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
    return hr;
}

HRESULT OnResetDeviceTerrainBlockStaticResource()
{
    HRESULT hr = E_FAIL;

    hr = g_cTerrainBlockModifier.OnResetDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_cTerrainBlockShareVB.OnResetDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_cTerrainBlockWaterShareVB.OnResetDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_cTerrainBlockBlendFactorShareVB.OnResetDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_cTerrainBlockGrassShareVBHighPool.OnResetDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
    return hr;
}

//////////////////////////////////////////////////////////////////////////
KG3DTerrainConverMap::~KG3DTerrainConverMap()
{
	UnInit();

}

HRESULT KG3DTerrainConverMap::ComputeHeightMapSize()
{
	float Width = abs(m_vRect.z);
	float Height = abs(m_vRect.w);
	float fSize = max(Width,Height);
	DWORD dwSize = 256;

	if(fSize<=25600)
		dwSize = 256;
	else if(fSize<=51200)
		dwSize = 512;
	else if(fSize<=102400)
		dwSize = 1024;
	else if(fSize<=204800)
		dwSize = 2048;
	else if(fSize<=409600)
		dwSize = 4096;

	m_dwHeightMapSize = dwSize;

	return S_OK;
}

HRESULT KG3DTerrainConverMap::LoadFromInformationBuffer(DWORD dwLength,KG3DMemoryFile* pFile)
{
	CHAR szConverName[MAX_PATH];
	CHAR szHeightName[MAX_PATH];

	KGLOG_PROCESS_ERROR(pFile);

	//pFile->Reset();
	pFile->read( szConverName  , sizeof(CHAR)*MAX_PATH );
	pFile->read( szHeightName  , sizeof(CHAR)*MAX_PATH );
	pFile->read(&m_vRect  ,  sizeof(D3DXVECTOR4) );
	pFile->read(&m_fLowest,sizeof(float));
	pFile->read(&m_fHeightTotal,sizeof(float));

	m_szTextureFileName = szConverName;
	m_szHeightColorFileName = szHeightName;

	SAFE_RELEASE(m_lpColorTex);
	HRESULT hr = g_cTextureTable.LoadResourceFromFile(szConverName,0,0/*TEXOP_RELOAD*/,(IKG3DResourceBase**)&m_lpColorTex);
	if (FAILED(hr))
	{
		m_lpColorTex = g_cTextureTable.GetConverTexture();
		if(m_lpColorTex)
			m_lpColorTex->AddRef();
	}
	
	if(g_cEngineOption.bEnableDynamicConverMap && NULL == m_lpRenderTarget)
	{
		hr = g_pd3dDevice->CreateTexture( 512, 512,1, D3DUSAGE_RENDERTARGET,
			D3DFMT_A8R8G8B8,
			D3DPOOL_DEFAULT,
			&m_lpRenderTarget,NULL);
		KGLOG_COM_PROCESS_ERROR(hr);
	}
	if(!m_lpTextureLight)
	{
		g_cTextureTable.LoadResourceFromFile("data\\source\\other\\特效\\贴图\\GX_光效\\GQ_光球.tga",0,0/*TEXOP_RELOAD*/,(IKG3DResourceBase**)&m_lpTextureLight);
		//g_cTextureTable.LoadResourceFromFile("data\\source\\other\\特效\\贴图\\GX_光效\\XX_X星星.tga",0,0,(IKG3DResourceBase**)&m_lpTextureLight);
	}
	if(!m_lpTextureCloud)
	{
		g_cTextureTable.LoadResourceFromFile("data\\public\\defaultscene\\cloud.tga",0,0/*TEXOP_RELOAD*/,(IKG3DResourceBase**)&m_lpTextureCloud);
		//g_cTextureTable.LoadResourceFromFile("data\\source\\other\\特效\\贴图\\GX_光效\\XX_X星星.tga",0,0,(IKG3DResourceBase**)&m_lpTextureLight);
	}
	

	ComputeHeightMapSize();

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DTerrainConverMap::SaveToInformationBuffer(DWORD& dwLength,KG3DMemoryFile* pFile)
{
	CHAR szConverName[MAX_PATH];
	CHAR szHeightName[MAX_PATH];

	KGLOG_PROCESS_ERROR(pFile);

	wsprintf(szConverName,"%s",m_szTextureFileName.c_str());
	wsprintf(szHeightName,"%s",m_szHeightColorFileName.c_str());

	//pFile->Reset();
	pFile->write( szConverName  , sizeof(CHAR)*MAX_PATH );
	pFile->write( szHeightName  , sizeof(CHAR)*MAX_PATH );
	pFile->write(&m_vRect  ,  sizeof(D3DXVECTOR4) );
	pFile->write(&m_fLowest,sizeof(float));
	pFile->write(&m_fHeightTotal,sizeof(float));

	dwLength = (DWORD)pFile->GetPos();

	return S_OK;
Exit0:
	return E_FAIL;
}
void KG3DTerrainConverMap::SetTerrainMapName( const char* cszFileName)
{
	m_szTerrainMap = cszFileName;
}
HRESULT KG3DTerrainConverMap::LoadTerrainMap(const char* cszFileName)
{
	KG_PROCESS_ERROR(g_IsFileExist(cszFileName));
	SAFE_RELEASE(m_lpTerrainMap);
	m_szTerrainMap = cszFileName;
	return g_cTextureTable.LoadResourceFromFile(cszFileName,0,0/*TEXOP_RELOAD*/,(IKG3DResourceBase**)&m_lpTerrainMap);
Exit0:
	return E_FAIL;
}
LPDIRECT3DTEXTURE9 KG3DTerrainConverMap::GetTerrainMap()
{
	if(m_lpTerrainMap)
		return m_lpTerrainMap->GetTexture();
	else if(g_IsFileExist(m_szTerrainMap.c_str()))
	{
		g_cTextureTable.LoadResourceFromFile(m_szTerrainMap.c_str(),0,0/*TEXOP_RELOAD*/,(IKG3DResourceBase**)&m_lpTerrainMap);
		return m_lpTerrainMap->GetTexture();
	}
	return NULL;
}
LPDIRECT3DTEXTURE9 KG3DTerrainConverMap::GetRenderTargetTex()
{
	if(g_cEngineOption.bEnableDynamicConverMap && m_bEnableDynamic)
	{

		if(g_cEngineOption.bEnableDynamicConverMap && NULL == m_lpRenderTarget)
		{
			g_pd3dDevice->CreateTexture( 512, 512,1, D3DUSAGE_RENDERTARGET,
				D3DFMT_A8R8G8B8,
				D3DPOOL_DEFAULT,
				&m_lpRenderTarget,NULL);
		}
		return m_lpRenderTarget;
	}
	else if(m_lpColorTex)
		return m_lpColorTex->GetTexture();

	return NULL;
}

KG3DTerrainConverMap::KG3DTerrainConverMap()
	:m_lpColorTex(NULL)
{
	m_lpTextureLight = NULL;
	m_lpTextureCloud = NULL;
	m_lpRenderTarget = NULL;
	m_lpTerrainMap   = NULL;

	m_vRect = D3DXVECTOR4(0,0,12800,12800);
	m_fLowest = -2000;
	m_fHeightTotal = 80000;
	m_dwHeightMapSize = 0;
	m_bEnableDynamic = FALSE;
}

LPDIRECT3DTEXTURE9 KG3DTerrainConverMap::GetColorTex()
{
	return NULL == m_lpColorTex ? NULL : m_lpColorTex->GetTexture();
}

VOID KG3DTerrainConverMap::SetRectVector( FLOAT StartX, FLOAT StartY, FLOAT Width, FLOAT Height )
{
	m_vRect = D3DXVECTOR4(StartX, StartY, Width, Height);
}

HRESULT KG3DTerrainConverMap::OnLostDevice()
{
    SAFE_RELEASE(m_lpRenderTarget);
	SAFE_RELEASE(m_lpTerrainMap);
    return S_OK;
}

HRESULT KG3DTerrainConverMap::OnResetDevice()
{
    HRESULT hr = S_OK;

	ASSERT(m_lpRenderTarget == NULL);

	if(m_bEnableDynamic)
	{
		hr = g_pd3dDevice->CreateTexture(512, 512, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_lpRenderTarget, NULL);
		KGLOG_COM_PROCESS_ERROR(hr);
	}
	if(!m_lpTerrainMap)
		g_cTextureTable.LoadResourceFromFile(m_szTerrainMap.c_str(),0,0/*TEXOP_RELOAD*/,(IKG3DResourceBase**)&m_lpTerrainMap);
Exit0:
    return hr;
}

//////////////////////////////////////////////////////////////////////////
KG3DTerrainBlockShareVB::KG3DTerrainBlockShareVB()
{
	pVB = NULL;
	dwLastTime = 0;
	pUser = NULL;
	m_lpLockVertex = NULL;
	m_eState = STATE_NODATA;
	m_bUserLock = FALSE;
	//nPosition = 0;
}

KG3DTerrainBlockShareVB::~KG3DTerrainBlockShareVB()
{
	UnInit();

	OnLostDevice();
}

HRESULT KG3DTerrainBlockShareVB::Init()
{
	HRESULT hr = S_OK;

	m_lpLockVertex = NULL;
	m_eState = STATE_NODATA;
	m_bUserLock = FALSE;

	if (!pVB)
	{
		hr = OnResetDevice();
		KGLOG_COM_PROCESS_ERROR(hr);
	}
	
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DTerrainBlockShareVB::UnInit()
{
	if(pVB && m_eState == STATE_LOCKED)
	{
		pVB->Unlock();
	}
	
	m_lpLockVertex = NULL;
	m_eState = STATE_NODATA;

	dwLastTime = 0;
	pUser = NULL;
	m_bUserLock = FALSE;

	return S_OK;
}

HRESULT KG3DTerrainBlockShareVB::OnLostDevice()
{
    //SAFE_RELEASE(pVB);
	if (pVB)
	{
	    KG3DResourceDelayReleaseMgr::Push(pVB);
    	pVB = NULL;
	}
	pUser = NULL;
    return S_OK;
}

HRESULT KG3DTerrainBlockShareVB::OnResetDevice()
{
    HRESULT hr = E_FAIL;

    ASSERT(pVB == NULL);

	DWORD dwThreadID = ::GetCurrentThreadId();
	KGLOG_PROCESS_ERROR(dwThreadID == g_dwMainThreadID);
	ASSERT(dwThreadID == g_dwMainThreadID);

    hr = g_pd3dDevice->CreateVertexBuffer(17 * 17 * sizeof(VFormat::_Faces_Normal_Texture0), 
        D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_CUSTOMVERTEXL, D3DPOOL_DEFAULT, &pVB, NULL);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
    return hr;
}

//////////////////////////////////////////////////////////////////////////
KG3DTerrainBlockWaterShareVB::KG3DTerrainBlockWaterShareVB()
{
	pVB = NULL;
	dwLastTime = 0;
	pUser = NULL;
	m_bUserLock = FALSE;
}

KG3DTerrainBlockWaterShareVB::~KG3DTerrainBlockWaterShareVB()
{
	UnInit();
	OnLostDevice();
}

HRESULT KG3DTerrainBlockWaterShareVB::Init()
{
	HRESULT hr = S_OK;

	if (!pVB)
	{
		hr = OnResetDevice();
		KGLOG_COM_PROCESS_ERROR(hr);
	}

	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DTerrainBlockWaterShareVB::UnInit()
{
	dwLastTime = 0;
	pUser = NULL;
	m_bUserLock = FALSE;

	return S_OK;
}

HRESULT KG3DTerrainBlockWaterShareVB::OnLostDevice()
{
    //VERIFY_RELEASE(pVB);
    if (pVB)
    {
        ASSERT(QueryRef(pVB) == 1);
    	KG3DResourceDelayReleaseMgr::Push(pVB);
    	pVB = NULL;
    }

	pUser = NULL;
    return S_OK;
}

HRESULT KG3DTerrainBlockWaterShareVB::OnResetDevice()
{
    HRESULT hr = E_FAIL;

	DWORD dwThreadID = ::GetCurrentThreadId();
	KGLOG_PROCESS_ERROR(dwThreadID == g_dwMainThreadID);
	ASSERT(dwThreadID == g_dwMainThreadID);

    ASSERT(pVB == NULL);

    hr = g_pd3dDevice->CreateVertexBuffer(17 * 17 * sizeof(VFormat::_Faces_Normal_Diffuse_Texture0), 
        D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &pVB, NULL);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
    return hr;
}

//////////////////////////////////////////////////////////////////////////
HRESULT KG3DTerrainBlockWaterVertexs::Init(KG3DTerrainBlock* pBlock)
{
	//DWORD dwSize = (DWORD)sizeof(VFormat::FACES_NORMAL_DIFFUSE_TEXTURE0) * 17 * 17;

	for (int i=0;i<17*17;i++)
	{
		m_WaterVertexs[i].p = pBlock->m_Vertexs[i].p;
		m_WaterVertexs[i].Normal = D3DXVECTOR3(0,1,0);
		m_WaterVertexs[i].diffuse = 0xFFFFFFFF;
	}
	return S_OK;
}

float* KG3DTerrainBlockWaterVertexs::GetVertexPointer(int nX,int nZ)
{
	/*//nX和nZ有可能为负，引用这个函数的函数没有阈值检查 Last modify by Chen TianHong:2008-5-9 22:15:21
	ASSERT(nX<=BLOCK_CELL_COUNT);
		ASSERT(nZ<=BLOCK_CELL_COUNT);
		ASSERT(nX>=0);
		ASSERT(nZ>=0);*/
	static FLOAT stFloat = 0;
	if (nX <= BLOCK_CELL_COUNT && nX >= 0 && nZ <= BLOCK_CELL_COUNT && nZ >= 0)
		return &m_WaterVertexs[nZ * (BLOCK_CELL_COUNT+1) + nX].p.y;
	return &stFloat;
}

//////////////////////////////////////////////////////////////////////////
KG3DTerrainBlockBlendFactorShareVB::KG3DTerrainBlockBlendFactorShareVB()
{
	pVB = NULL;
	dwLastTime = 0;
	pUser = NULL;
	m_bUserLock = FALSE;
}

KG3DTerrainBlockBlendFactorShareVB::~KG3DTerrainBlockBlendFactorShareVB()
{
	UnInit();
	OnLostDevice();
}

HRESULT KG3DTerrainBlockBlendFactorShareVB::Init()
{
	HRESULT hr = S_OK;

	if (!pVB)
	{
        hr = OnResetDevice();
		KGLOG_COM_PROCESS_ERROR(hr);
	}
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DTerrainBlockBlendFactorShareVB::UnInit()
{
	dwLastTime = 0;
	pUser = NULL;
	m_bUserLock = FALSE;

	return S_OK;
}

HRESULT KG3DTerrainBlockBlendFactorShareVB::OnLostDevice()
{
    //SAFE_RELEASE(pVB);
	if (pVB)
	{
	    KG3DResourceDelayReleaseMgr::Push(pVB);
    	pVB = NULL;
	}

	pUser = NULL;
    return S_OK;
}

HRESULT KG3DTerrainBlockBlendFactorShareVB::OnResetDevice()
{
    HRESULT hr = E_FAIL;

    hr = g_pd3dDevice->CreateVertexBuffer(17 * 17 * sizeof(float/*D3DXVECTOR4*/), D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &pVB, NULL);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
    return hr;
}

//////////////////////////////////////////////////////////////////////////
KG3DTerrainBlock::KG3DTerrainBlock(void)
{
    m_Container = NULL;
	m_lpShareGroundVB = NULL;
	m_nIndex_X = 0;
	m_nIndex_Z = 0;
	m_nLevel = BLOCK_TYPE_REGION;

	m_bNeedRefreshVectexVB = TRUE;
	m_bNeedRefreshBlendFacetorVB = TRUE;
	m_bNeedRefreshWaterVectexVB = TRUE;

	m_lpConverTexture = NULL;
	m_uHandleConverMap = 0;
	m_uHandleDetailMgr = 0;

	m_vConverUV = D3DXVECTOR4(0,0,1,1);
	m_vBlockRect= D3DXVECTOR4(0,0,0,0);

	m_fHeightMin = 0;
	m_fHeightMax = 0;

	m_lpBlendFactor = NULL;
	m_lpzBlendFactorShareVB = NULL;

	m_nNumDetailTexture = 0;
	m_nNumBlendFactor = 0;
	ZeroMemory(m_byteDetailTextureIndex,sizeof(BYTE)*TERRAIN_BLOCK_MAX_DETAILMAP);
	ZeroMemory(m_byteTextureIndex,sizeof(BYTE)*17*17);

	m_ullMaterialKey = 0;
	m_lpWaterVertex = NULL;
	m_lpShareWaterVB = NULL;

	m_bWaterVisible = FALSE;

	m_lpGrassData = NULL;
	m_lpStoneData = NULL;
	m_lpFrutexData = NULL;
	m_fViewDistance = 0;
	m_dwLastVisibleFrameCount = 0;

	m_ulRefCount = 0;

	m_lpPointLightInfo = NULL;

	m_nLodLevel = 0;
	m_nIndexIB = 0;

	m_bLoaded = FALSE;
	m_dwLoadedState = LOADED_NONE;
}

ULONG KG3DTerrainBlock::AddRef()
{
    return (ULONG)KG_InterlockedIncrement((long *)&m_ulRefCount);
}

ULONG KG3DTerrainBlock::Release()
{
    ULONG uNewRefCount = (ULONG)KG_InterlockedDecrement((long *)&m_ulRefCount);
    if (uNewRefCount == 0)
	{
		UnInit();
		g_cTerrainBlockPool.FreeResource(this);
	}
    return uNewRefCount;
}

HRESULT KG3DTerrainBlock::QueryInterface( 
    REFIID riid,
    void __RPC_FAR *__RPC_FAR *ppvObject)
{
    return E_FAIL;
}

KG3DTerrainBlock::~KG3DTerrainBlock(void)
{
	UnInit();
}

HRESULT KG3DTerrainBlock::UnInit()
{
	m_dwLoadedState = LOADED_NONE;
	m_bLoaded = FALSE;
	m_vecDataLoadType.clear();
	
	m_uHandleConverMap = 0;
	SAFE_RELEASE(m_lpConverTexture);

	CleanBlendFactor();

	ReleaseVertexWater();
	
	DeleteGrassData();
	DeleteStoneData();
	DeleteFrutexData();

	UnPrepareSharedGroundVB();

	m_nIndex_X = 0;
	m_nIndex_Z = 0;
	m_nLevel = BLOCK_TYPE_REGION;

	m_bNeedRefreshVectexVB = TRUE;
	m_bNeedRefreshBlendFacetorVB = TRUE;

	m_bNeedRefreshWaterVectexVB = TRUE;

	m_lpConverTexture = NULL;
	m_uHandleConverMap = 0;
	m_uHandleDetailMgr = 0;

	m_vConverUV = D3DXVECTOR4(0,0,1,1);
	m_vBlockRect= D3DXVECTOR4(0,0,0,0);

	m_fHeightMin = 0;
	m_fHeightMax = 0;

	//m_lpBlendFactor = NULL;//ZeroMemory(m_lpBlendFactor,sizeof(KG3DTerrainBlockBlendFactor*)*TERRAIN_BLOCK_MAX_BLENDFACTOR);
	UnPrepareSharedBlendFactorVB();
	
	m_nNumDetailTexture = 0;
	m_nNumBlendFactor = 0;
	ZeroMemory(m_byteDetailTextureIndex,sizeof(BYTE)*TERRAIN_BLOCK_MAX_DETAILMAP);
	ZeroMemory(m_byteTextureIndex,sizeof(BYTE)*17*17);

	m_ullMaterialKey = 0;
	m_lpWaterVertex = NULL;

	UnPrepareSharedWaterVB();

	m_bWaterVisible = FALSE;

	m_lpGrassData = NULL;
	m_fViewDistance = 0;
	
	m_mapRoad.clear();
	m_mapRiver.clear();
	m_dwLastVisibleFrameCount = 0;
    m_Container = NULL;
	

	if(m_lpPointLightInfo)
	{
		g_cPointLightInfoPool.FreeResource(m_lpPointLightInfo);
		m_lpPointLightInfo = NULL;
	}

	m_nLodLevel = 0;
	m_nIndexIB = 0;
	return S_OK;
}


HRESULT KG3DTerrainBlock::Init(int nLevel,int nIndex_X,int nIndex_Z)
{
	m_nIndex_X = nIndex_X;
	m_nIndex_Z = nIndex_Z;
	m_nLevel = nLevel;
	float CellLength = 0;
	float BlockLength = 0;

	SceneBlockFindLength(nLevel,BlockLength,CellLength);

	for (int nZ = 0;nZ <= BLOCK_CELL_COUNT;nZ++ )
	{
		for (int nX = 0;nX <= BLOCK_CELL_COUNT;nX++ )
		{
			int nIndex = nZ * (BLOCK_CELL_COUNT+1) + nX;
			m_Vertexs[nIndex].p.x = m_nIndex_X * BlockLength + nX * CellLength;
			m_Vertexs[nIndex].p.z = m_nIndex_Z * BlockLength + nZ * CellLength;
			m_Vertexs[nIndex].p.y = 0;
			m_Vertexs[nIndex].Normal = D3DXVECTOR3(0,1,0);
		}
	}
	m_fHeightMin = 0;
	m_fHeightMax = 200;
    m_fWaterHeightMin = 1.0e10;
    m_mapWaterBoundingPlanes.clear();

	m_vBlockRect = GetSceneBlockRect(nLevel,nIndex_X,nIndex_Z);

	//Test();
	ComputeMaterialKey();
	return S_OK; 
}

HRESULT KG3DTerrainBlock::PrepareSharedGroundVB()
{
	DWORD dwThreadID = GetCurrentThreadId();
	/*if(dwThreadID != g_dwMainThreadID)
	{
		int k = 0;
	}*/
	KG_PROCESS_ERROR(dwThreadID == g_dwMainThreadID);

	HRESULT hr = S_OK;
	BOOL bNeedRefresh = FALSE;
	
	if(!m_lpShareGroundVB)
	{
		m_lpShareGroundVB = g_cTerrainBlockShareVB.RequestResource();
		KGLOG_PROCESS_ERROR(m_lpShareGroundVB);
		bNeedRefresh = TRUE;
	}
	/*else if(m_lpShareGroundVB->pUser != this)
	{
		g_cTerrainBlockShareVB.GetOneShareResource(&m_lpShareGroundVB,this);
		bNeedRefresh = TRUE;
	}*/

	KGLOG_PROCESS_ERROR(m_lpShareGroundVB);

	if(bNeedRefresh||m_bNeedRefreshVectexVB)
	{
		m_lpShareGroundVB->m_eState = KG3DTerrainBlockShareVB::STATE_NODATA;

		//if(g_nGrassVertexBufferFillPerFrame <= 3 )
		{
			VFormat::_Faces_Normal_Texture0 *pVer = NULL;

			DWORD dwSize = 17 * 17 * sizeof(VFormat::_Faces_Normal_Texture0);

			hr = m_lpShareGroundVB->Init();
			KGLOG_COM_PROCESS_ERROR(hr);

			hr = m_lpShareGroundVB->pVB->Lock(0,dwSize,(void **)&pVer, D3DLOCK_DISCARD);
			KGLOG_COM_PROCESS_ERROR(hr);
	
			memcpy(pVer,m_Vertexs,dwSize);

			hr = m_lpShareGroundVB->pVB->Unlock();
			KGLOG_COM_PROCESS_ERROR(hr);
			
			m_bNeedRefreshVectexVB = FALSE;

			g_nLoadCount++;

			m_lpShareGroundVB->m_eState = KG3DTerrainBlockShareVB::STATE_DATAOK;
		}

		/*if(m_lpShareGroundVB)
		{
			g_cTerrainBlockShareVB.OnResourceUse(m_lpShareGroundVB);
		}	*/
	}



	return S_OK;
Exit0:
	if(m_lpShareGroundVB)
	{
		m_lpShareGroundVB->m_eState = KG3DTerrainBlockShareVB::STATE_NODATA;
		m_bNeedRefreshVectexVB = TRUE;
	}
	return E_FAIL;
}

HRESULT KG3DTerrainBlock::UnPrepareSharedGroundVB()
{
	if(m_lpShareGroundVB)
	{
		g_cTerrainBlockShareVB.FreeResource(m_lpShareGroundVB);
		m_lpShareGroundVB = NULL;
	}

	return S_OK;
}

HRESULT KG3DTerrainBlock::PrepareSharedWaterVB()
{
	DWORD dwThreadID = GetCurrentThreadId();
	//if(dwThreadID != g_dwMainThreadID)
	//{
	//	int k = 0;
	//}
	KG_PROCESS_ERROR(dwThreadID == g_dwMainThreadID);

	HRESULT hr = S_OK;
	BOOL bNeedRefresh = FALSE;

	KGLOG_PROCESS_ERROR(m_lpWaterVertex);

	if(!m_lpShareWaterVB)
	{
		m_lpShareWaterVB = g_cTerrainBlockWaterShareVB.RequestResource();
		KGLOG_PROCESS_ERROR(m_lpShareWaterVB);
		bNeedRefresh = TRUE;
	}
	/*else if(m_lpShareWaterVB->pUser != this)
	{
		g_cTerrainBlockWaterShareVB.GetOneShareResource(&m_lpShareWaterVB,this);
		bNeedRefresh = TRUE;
	}*/

	if(bNeedRefresh||m_bNeedRefreshWaterVectexVB)
	{
		//if(g_nGrassVertexBufferFillPerFrame<=3)
		{
			VFormat::FACES_NORMAL_DIFFUSE_TEXTURE0 *pVer = NULL;

			DWORD dwSize = 17 * 17 * sizeof(VFormat::FACES_NORMAL_DIFFUSE_TEXTURE0);

			hr = m_lpShareWaterVB->Init();
			KGLOG_COM_PROCESS_ERROR(hr);

			hr = m_lpShareWaterVB->pVB->Lock(0,dwSize,(void **)&pVer,0 );
			KGLOG_COM_PROCESS_ERROR(hr);

			memcpy(pVer,m_lpWaterVertex->m_WaterVertexs,dwSize);

			hr = m_lpShareWaterVB->pVB->Unlock();
			KGLOG_COM_PROCESS_ERROR(hr);

			m_bNeedRefreshWaterVectexVB = FALSE;
			//g_nGrassVertexBufferFillPerFrame++;

			g_nLoadCount ++;
		}

		/*if(m_lpShareWaterVB)
		{
			g_cTerrainBlockWaterShareVB.OnResourceUse(m_lpShareWaterVB);
		}	*/
	}

	return S_OK;
Exit0:
	if(m_lpShareWaterVB)
	{
		m_bNeedRefreshWaterVectexVB = TRUE;
	}
	return E_FAIL;
}

HRESULT KG3DTerrainBlock::UnPrepareSharedWaterVB()
{
	if(m_lpShareWaterVB)
	{
		g_cTerrainBlockWaterShareVB.FreeResource(m_lpShareWaterVB);
		m_lpShareWaterVB = NULL;
	}
	return S_OK;
}

HRESULT KG3DTerrainBlock::PrepareSharedBlendFactorVB()
{
	DWORD dwThreadID = GetCurrentThreadId();
	/*if(dwThreadID != g_dwMainThreadID)
	{
		int k = 0;
	}*/
	KG_PROCESS_ERROR(dwThreadID == g_dwMainThreadID);

	HRESULT hr = S_OK;
	BOOL bNeedRefresh = FALSE;

	if(!m_lpBlendFactor)
	{
		KGLogPrintf(KGLOG_ERR, "PrepareSharedBlendFactorVB %d %d Failed.\n", m_nIndex_X,m_nIndex_Z);
	}
	KG_PROCESS_ERROR(m_lpBlendFactor);

	if(!m_lpzBlendFactorShareVB)
	{
		m_lpzBlendFactorShareVB = g_cTerrainBlockBlendFactorShareVB.RequestResource();
		KGLOG_PROCESS_ERROR(m_lpzBlendFactorShareVB);
		bNeedRefresh = TRUE;
	}
	/*else if(m_lpzBlendFactorShareVB[i]->pUser != &m_lpBlendFactor[i])
	{
		g_cTerrainBlockBlendFactorShareVB.GetOneShareResource(&m_lpzBlendFactorShareVB[i],&m_lpBlendFactor[i]);
		bNeedRefresh = TRUE;
	}*/

	if(bNeedRefresh||m_bNeedRefreshBlendFacetorVB)
	{
		//D3DXVECTOR4 *pVer = NULL;
		float *pVer = NULL;
		DWORD dwSize = 17 * 17 * sizeof(float /*D3DXVECTOR4*/);

		hr = m_lpzBlendFactorShareVB->Init();
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = m_lpzBlendFactorShareVB->pVB->Lock(0,dwSize,(void **)&pVer,0 );
		KGLOG_COM_PROCESS_ERROR(hr);

		memcpy(pVer,m_lpBlendFactor->vBlend,dwSize);

		hr = m_lpzBlendFactorShareVB->pVB->Unlock();
		KGLOG_COM_PROCESS_ERROR(hr);

		m_bNeedRefreshBlendFacetorVB = FALSE;

		g_nLoadCount ++;

		/*if(m_lpzBlendFactorShareVB[i])
		{
			g_cTerrainBlockBlendFactorShareVB.OnResourceUse(m_lpzBlendFactorShareVB[i]);
		}*/	
	}

	return S_OK;
Exit0:
	if(m_lpzBlendFactorShareVB)
	{
		m_bNeedRefreshBlendFacetorVB = TRUE;
	}
	return E_FAIL;
}

HRESULT KG3DTerrainBlock::UnPrepareSharedBlendFactorVB()
{
	if(m_lpzBlendFactorShareVB)
	{
		g_cTerrainBlockBlendFactorShareVB.FreeResource(m_lpzBlendFactorShareVB);
		m_lpzBlendFactorShareVB = NULL;
	}
	return S_OK;
//Exit0:
	//return E_FAIL;
}
LPDIRECT3DVERTEXBUFFER9 KG3DTerrainBlock::GetWaterVertexBuffer()
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(m_lpWaterVertex);
	
	hr = PrepareSharedWaterVB();
	KG_COM_PROCESS_ERROR(hr);
	
    KG_PROCESS_ERROR(m_lpShareWaterVB);

	return m_lpShareWaterVB->pVB;
Exit0:
	return NULL;
}

LPDIRECT3DVERTEXBUFFER9 KG3DTerrainBlock::GetGroundVertexBuffer()
{
	//if (m_bNeedRefreshVectexVB)
	PrepareSharedGroundVB();

	if (m_lpShareGroundVB && m_lpShareGroundVB->m_eState == KG3DTerrainBlockShareVB::STATE_DATAOK)
		return m_lpShareGroundVB->pVB;

    return NULL;
}

LPDIRECT3DVERTEXBUFFER9 KG3DTerrainBlock::GetBlendFactorBuffer()
{
	HRESULT hr = E_FAIL;
	//KGLOG_PROCESS_ERROR(i>=0);
	//KGLOG_PROCESS_ERROR(i<TERRAIN_BLOCK_MAX_BLENDFACTOR );

	hr = PrepareSharedBlendFactorVB();
	KG_COM_PROCESS_ERROR(hr);
	
	if (m_lpzBlendFactorShareVB)
	{
		return m_lpzBlendFactorShareVB->pVB;
	}
Exit0:
	return NULL;
}

HRESULT KG3DTerrainBlock::OnLostDevice()
{
	if (m_lpGrassData)
	{
		m_lpGrassData->m_bNeedRefreshGrass = TRUE;
	}
	if (m_lpStoneData)
	{
		m_lpStoneData->m_bNeedRefreshGrass = TRUE;
	}
	if (m_lpFrutexData)
	{
		m_lpFrutexData->m_bNeedRefreshGrass = TRUE;
	}

	return S_OK;
}

//HRESULT KG3DTerrainBlock::SetConverTexture(UINT uHandleConverMap,D3DXVECTOR2 A,D3DXVECTOR2 C,KG3DSceneDataInterface* pDataInterface)
//{
//	m_uHandleConverMap = uHandleConverMap;
//
//	float X_Len = C.x - A.x;
//	float Z_Len = A.y - C.y;
//
//	m_vConverUV.x = (m_vBlockRect.x - A.x) / X_Len;
//	m_vConverUV.y = (m_vBlockRect.y - C.y) / Z_Len;
//	m_vConverUV.z =  abs(m_vBlockRect.z        / X_Len);
//	m_vConverUV.w =  abs(m_vBlockRect.w        / Z_Len);
//
//	//////////////////////////////////////////////////////////////////////////
//	POINT index = {m_nIndex_X,m_nIndex_Z};
//	DWORD dwSize = 0;
//	{
//		g_cMemoryFile.Reset();
//		g_cMemoryFile.write(&m_uHandleConverMap,sizeof(UINT));
//		g_cMemoryFile.write(&m_vConverUV,sizeof(D3DXVECTOR4));
//		dwSize = g_cMemoryFile.GetLength();
//	}
//
//	pDataInterface->Request_ModifyTerrainBlock(m_nLevel,index,Terrain_Block_Convermap,dwSize,g_cMemoryFile.GetBuffer());
//
//	return S_OK;
//}

//LPDIRECT3DTEXTURE9 KG3DTerrainBlock::GetConverTexture()
//{
//	if (m_lpConverTexture)
//	{
//		return m_lpConverTexture->GetTexture();
//	}
//	return NULL;
//}

//HRESULT KG3DTerrainBlock::SetHeight(KG3DHeightMap* pHeightMap,D3DXVECTOR2 A,D3DXVECTOR2 C,KG3DSceneDataInterface* pDataInterface,BOOL bToFile)
//{
//	float U_Len = C.x - A.x;
//	float V_Len = C.y - A.y;
//
//	int K = 0;
//	for (int nZ = 0;nZ <= BLOCK_CELL_COUNT;nZ++ )
//	{
//		for (int nX = 0;nX <= BLOCK_CELL_COUNT;nX++ )
//		{
//			int nIndex = nZ * (BLOCK_CELL_COUNT+1) + nX;
//
//			float U = (m_Vertexs[nIndex].p.x - A.x) / U_Len;
//			float V = (m_Vertexs[nIndex].p.z - A.y) / V_Len;
//
//			float H = m_Vertexs[nIndex].p.y = pHeightMap->GetHeight(U,V);
//			m_Vertexs[nIndex].Normal = pHeightMap->GetNormal(U,V);
//
//			if(K==0)
//			{
//				m_fHeightMin = m_fHeightMax = H;
//				K++;
//			}
//			else
//			{
//				m_fHeightMax = max(H,m_fHeightMax);
//				m_fHeightMin = min(H,m_fHeightMin);
//			}
//		}
//	}
//
//	m_bNeedRefreshVectexVB = TRUE;
//
//	CheckWaterVisible();
//
//	//////////////////////////////////////////////////////////////////////////
//	if(bToFile)
//	{
//		DWORD dwVersion = 0;
//		DWORD dwLength = 0;
//
//		KG3DMemoryFile* pFile = g_cMemoryFilePool.RequestResource();
//		if(pFile)
//		{
//			pFile->write(&dwVersion,sizeof(DWORD));
//			pFile->write(&m_fHeightMax,sizeof(float));
//			pFile->write(&m_fHeightMin,sizeof(float));
//			pFile->write(m_Vertexs,sizeof(m_Vertexs));
//
//			dwLength = pFile->GetLength();
//
//			POINT Index = {m_nIndex_X,m_nIndex_Z};
//			pDataInterface->Request_ModifyTerrainBlock(m_nLevel,Index,Terrain_Block_GroundHeight,dwLength,pFile->GetBuffer());
//			g_cMemoryFilePool.FreeResource(pFile);
//			pFile = NULL;
//		}
//	}
//
//	return S_OK;
//}

//HRESULT KG3DTerrainBlock::SetWaterHeight(KG3DHeightMap* pHeightMap,D3DXVECTOR2 A,D3DXVECTOR2 C,KG3DSceneDataInterface* pDataInterface,BOOL bToFile)
//{
//	float U_Len = C.x - A.x;
//	float V_Len = C.y - A.y;
//
//	CreateVertexWater();
//	KGLOG_PROCESS_ERROR(m_lpWaterVertex);
//
//	for (int nZ = 0;nZ <= BLOCK_CELL_COUNT;nZ++ )
//	{
//		for (int nX = 0;nX <= BLOCK_CELL_COUNT;nX++ )
//		{
//			int nIndex = nZ * (BLOCK_CELL_COUNT+1) + nX;
//
//			float U = (m_lpWaterVertex->m_WaterVertexs[nIndex].p.x - A.x) / U_Len;
//			float V = (m_lpWaterVertex->m_WaterVertexs[nIndex].p.z - A.y) / V_Len;
//
//			float H = m_lpWaterVertex->m_WaterVertexs[nIndex].p.y = pHeightMap->GetHeight(U,V);
//			m_lpWaterVertex->m_WaterVertexs[nIndex].Normal = pHeightMap->GetNormal(U,V);
//			
//			m_fHeightMax = max(H,m_fHeightMax);
//			m_fHeightMin = min(H,m_fHeightMin);
//			
//		}
//	}
//
//	m_bNeedRefreshWaterVectexVB = TRUE;
//
//	CheckWaterVisible();
//
//	if(bToFile && m_lpWaterVertex)
//	{
//		DWORD dwVersion = 0;
//		DWORD dwLength = 0;
//
//		KG3DMemoryFile* pFile = g_cMemoryFilePool.RequestResource();
//		if(pFile)
//		{
//			pFile->write(&dwVersion,sizeof(DWORD));
//			pFile->write(&m_fHeightMax,sizeof(float));
//			pFile->write(&m_fHeightMin,sizeof(float));
//			pFile->write(m_lpWaterVertex->m_WaterVertexs,sizeof(m_lpWaterVertex->m_WaterVertexs));
//
//			dwLength = pFile->GetLength();
//
//			POINT Index = {m_nIndex_X,m_nIndex_Z};
//			pDataInterface->Request_ModifyTerrainBlock(m_nLevel,Index,Terrain_Block_Water,dwLength,pFile->GetBuffer());
//			g_cMemoryFilePool.FreeResource(pFile);
//			pFile = NULL;
//		}
//	}
//	return S_OK;
//Exit0:
//	return E_FAIL;
//}

float KG3DTerrainBlock::GetGroundHeight(float fX,float fZ)
{
	if(!(m_dwLoadedState & LOADED_GROUNDVERTEX))
		return 0;
	//这里比原来变得复杂了一些，但算法也很容易更改了，这个算法变得和任何类无关了，主要是可以用TDD来检查错误了
	struct GroundHeightGetter : public KG3DGridVertexHeightGetter
	{
		virtual D3DXVECTOR4 GetBlockRect(){return pBlock->m_vBlockRect;}
		virtual ULONG		GetBlockCellCount(){return BLOCK_CELL_COUNT;}
		virtual FLOAT		GetVertexHeight(int nX,int nZ){return pBlock->GetVertexHeight(nX, nZ);}
		KG3DTerrainBlock* pBlock;
	};
	GroundHeightGetter GH;
	GH.pBlock = this;
	return ::KG3DGridHeightGetterFunc(fX, fZ, GH);
}

BYTE KG3DTerrainBlock::GetDetailTextureIndex(float fX,float fZ)
{
	float X = 0 ,Y = 0;

	float u = (fX - m_vBlockRect.x) / m_vBlockRect.z;
	float v = (fZ - m_vBlockRect.y) / m_vBlockRect.w;

	float u_T = u * BLOCK_CELL_COUNT;
	float v_T = v * BLOCK_CELL_COUNT;

	X = 0 ;Y = 0;
	modf(u_T,&X);
	modf(v_T,&Y);
	int xx = (int)X;
	int zz = (int)Y;
	if (xx >= 0 && xx <= BLOCK_CELL_COUNT && zz >=0 && zz <= BLOCK_CELL_COUNT)
		return m_byteTextureIndex[zz * (BLOCK_CELL_COUNT + 1) + xx];	
	else
		return 0;
}

HRESULT KG3DTerrainBlock::GetWaterHeight(float& fHeight,float fX,float fZ)
{
	if(!(m_dwLoadedState & LOADED_WATERVERTEX))
		return E_FAIL;
	if(!m_lpWaterVertex)
	{
		fHeight = m_fMainWaterHeight;
		return S_OK;
	}	

	float X = 0 ,Y = 0;

	float u = (fX - m_vBlockRect.x) / m_vBlockRect.z;
	float v = (fZ - m_vBlockRect.y) / m_vBlockRect.w;

	float u_T = u * BLOCK_CELL_COUNT;
	float v_T = v * BLOCK_CELL_COUNT;

	X = 0 ;Y = 0;
	float S_X = modf(u_T,&X);
	float S_Z = modf(v_T,&Y);

	float* pA, *pB, *pC, *pD;

	int xx = (int)X;
	int zz = (int)Y;

	pA = m_lpWaterVertex->GetVertexPointer(xx,zz);
	pB = m_lpWaterVertex->GetVertexPointer(xx,zz);
	pC = m_lpWaterVertex->GetVertexPointer(xx,zz);
	pD = m_lpWaterVertex->GetVertexPointer(xx,zz);

	if (xx < BLOCK_CELL_COUNT)
		pB = m_lpWaterVertex->GetVertexPointer(xx + 1, zz);

	if (zz < BLOCK_CELL_COUNT)
		pD = m_lpWaterVertex->GetVertexPointer(xx, zz + 1);

	if ((xx < BLOCK_CELL_COUNT) && (zz < BLOCK_CELL_COUNT))
		pC = m_lpWaterVertex->GetVertexPointer(xx + 1, zz + 1);

	float Height = 0;
	if (S_X + S_Z <= 1.0f)
	{
		float b = *pB - *pA;
		float d = *pD - *pA;
		Height = S_X * b + S_Z * d + *pA;
	}
	else
	{
		float c = *pC - *pB;
		float d = *pD - *pB;
		Height = (S_X - 1) * (c - d) + S_Z * c + *pB;
	}

	fHeight = Height;

	return S_OK;
}


FLOAT KG3DTerrainBlock::GetVertexHeight(int nX,int nZ)
{
	if(!(m_dwLoadedState & LOADED_GROUNDVERTEX))
		return 0;
	ASSERT(nX<=BLOCK_CELL_COUNT && nZ<=BLOCK_CELL_COUNT 
		&& nX >= 0 && nZ >= 0);

	return m_Vertexs[nZ * (BLOCK_CELL_COUNT+1) + nX].p.y;
}

D3DXVECTOR3* KG3DTerrainBlock::GetVertexNormalPointer(int nX,int nZ)
{
	ASSERT(nX<=BLOCK_CELL_COUNT);
	ASSERT(nZ<=BLOCK_CELL_COUNT);
	ASSERT(nX>=0);
	ASSERT(nZ>=0);

	return &m_Vertexs[nZ * (BLOCK_CELL_COUNT+1) + nX].Normal;
}
D3DXVECTOR3 KG3DTerrainBlock::GetVertexPos(int nX,int nZ)
{
	ASSERT(nX<=BLOCK_CELL_COUNT);
	ASSERT(nZ<=BLOCK_CELL_COUNT);
	ASSERT(nX>=0);
	ASSERT(nZ>=0);

	return m_Vertexs[nZ * (BLOCK_CELL_COUNT+1) + nX].p;
}
D3DXVECTOR3 KG3DTerrainBlock::GetVertexNormal(int nX,int nZ)
{
	ASSERT(nX<=BLOCK_CELL_COUNT);
	ASSERT(nZ<=BLOCK_CELL_COUNT);
	ASSERT(nX>=0);
	ASSERT(nZ>=0);

	return m_Vertexs[nZ * (BLOCK_CELL_COUNT+1) + nX].Normal;
}
HRESULT KG3DTerrainBlock::WedNormal(KG3DTerrainBlock* pLeft,KG3DTerrainBlock* pRight,KG3DTerrainBlock* pUp,KG3DTerrainBlock* pDown)
{
	int nWidth = BLOCK_CELL_COUNT + 1;

	if(pRight)
	{
		for (int nZ = 0;nZ <= BLOCK_CELL_COUNT;nZ++ )
		{
			int nIndex_A =  nZ * nWidth + BLOCK_CELL_COUNT;
			int nIndex_B =  nZ * nWidth + 0;

			D3DXVECTOR3  V = (m_Vertexs[nIndex_A].Normal + pRight->m_Vertexs[nIndex_B].Normal) * 0.5F;
			m_Vertexs[nIndex_A].Normal = V;
			pRight->m_Vertexs[nIndex_B].Normal = V;
		}
	}
	if(pUp)
	{
		for (int nX = 0;nX <= BLOCK_CELL_COUNT;nX++ )
		{
			int nIndex_A =  BLOCK_CELL_COUNT * nWidth + nX;
			int nIndex_B =  0 * nWidth + nX;

			D3DXVECTOR3  V = (m_Vertexs[nIndex_A].Normal + pUp->m_Vertexs[nIndex_B].Normal) * 0.5F;
			m_Vertexs[nIndex_A].Normal = V;
			pUp->m_Vertexs[nIndex_B].Normal = V;
		}
	}
	return S_OK;
}


HRESULT KG3DTerrainBlock::SetNumBlendFactor(int nNumFactor)
{
	if(nNumFactor == 0)
		CleanBlendFactor();
	else
	{
		m_lpBlendFactor = g_cTerrainBlockBlendFactorPool.RequestResource();
		KGLOG_PROCESS_ERROR(m_lpBlendFactor);
	}
	m_nNumBlendFactor = nNumFactor;
	_ASSERT(m_nNumBlendFactor < 2);
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DTerrainBlock::CleanBlendFactor()
{
	if(m_lpBlendFactor)
	{
		g_cTerrainBlockBlendFactorPool.FreeResource(m_lpBlendFactor);
		m_lpBlendFactor = NULL;
	}
	return S_OK;
}

void SetBlendFactor(int nPos,D3DXVECTOR4* pValue)
{
	float *Value = (float*)pValue;
	Value[0] = Value[1] = Value[2] = Value[3] = 0;
	Value[nPos] = 1.0F;
}

HRESULT KG3DTerrainBlock::Test()
{
	SetNumBlendFactor(1);

	int P = 0;
	if(m_lpBlendFactor)
	{
		int nWidth = BLOCK_CELL_COUNT + 1;

		for (int nZ = 0;nZ <= BLOCK_CELL_COUNT;nZ++ )
		{
			for (int nX = 0;nX <= BLOCK_CELL_COUNT;nX++ )
			{
				int nIndex =  nZ * nWidth + nX;

				D3DXVECTOR3 Normal  = m_Vertexs[nIndex].Normal;
				D3DXVECTOR4 Pos = m_Vertexs[nIndex].p;

				float K = D3DXVec3Dot(&Normal,&D3DXVECTOR3(0,1,0));
				if(K>0.8F)
					P = 3;
				else if(K>0.5F)
					P = 2;
				else if (K>0.3)
					P = 1;
				else 
					P = 0;
			//	SetBlendFactor(P,&m_lpBlendFactor[0]->vBlend[nIndex]);
			}
		}
	}

	m_bNeedRefreshVectexVB = TRUE;
	return S_OK;
}

HRESULT KG3DTerrainBlock::PocessDetailFactors(BYTE byteTextureIndex[])
{
	int nWidth = BLOCK_CELL_COUNT + 1;
	if(m_nNumDetailTexture == 0)
		SetNumBlendFactor(0);
	else 
		SetNumBlendFactor(1);
	
	//////////////////////////////////////////////////////////////////////////
	for (int nZ = 0;nZ <= BLOCK_CELL_COUNT;nZ++ )
	{
		for (int nX = 0;nX <= BLOCK_CELL_COUNT;nX++ )
		{
			int nIndex =  nZ * nWidth + nX;
		

			BYTE bText = byteTextureIndex[nIndex];
			
			int nPos = FindDetailTexture(bText);
			if(nPos!=-1)
			{
				//SetBlendFactorPos(nPos,nX,nZ);
				if(m_lpBlendFactor)
				{
					m_lpBlendFactor->vBlend[nIndex] = (float)nPos;
					m_bNeedRefreshBlendFacetorVB = TRUE;
				}
			}
			else
			{
				if(m_lpBlendFactor)
				{
					m_lpBlendFactor->vBlend[nIndex] = 0;
				}
			}
		}
	}
	return S_OK;
}

HRESULT KG3DTerrainBlock::SetDetailTexture(set<BYTE>*psetIndex,BYTE byteTextureIndex[],KG3DSceneDataInterface* lpDataInterface,UINT uHandleMtlMgr)
{
	m_uHandleDetailMgr = uHandleMtlMgr;

	map<BYTE,int>mapIndexUse;
	
	{
		int K = 0;
		set<BYTE>::iterator i = psetIndex->begin();
		set<BYTE>::iterator iend = psetIndex->end();
		while (i!=iend && K < TERRAIN_BLOCK_MAX_DETAILMAP )
		{
			BYTE bIndex = *i;

			m_byteDetailTextureIndex[K] = bIndex;

			++i;
			K++;
		}
		m_nNumDetailTexture = K;
	}

	PocessDetailFactors(byteTextureIndex);
	ComputeMaterialKey();
	//////////////////////////////////////////////////////////////////////////
	{
		DWORD dwVersion = 0;

		KG3DMemoryFile* pFile = g_cMemoryFilePool.RequestResource();
		if(pFile)
		{
			pFile->write(&dwVersion,sizeof(DWORD));
			pFile->write(&m_uHandleDetailMgr,sizeof(UINT));
			pFile->write(m_byteDetailTextureIndex,sizeof(m_byteDetailTextureIndex));
			pFile->write(&m_nNumDetailTexture,sizeof(int));
			pFile->write(byteTextureIndex,sizeof(BYTE)*17*17);
			DWORD dwLength = pFile->GetLength();

			POINT Index = {m_nIndex_X,m_nIndex_Z};
			lpDataInterface->Request_ModifyTerrainBlock(m_nLevel,Index,Terrain_Block_DetailMtl,dwLength,pFile->GetBuffer());
			g_cMemoryFilePool.FreeResource(pFile);
			pFile = NULL;
		}

	}
	//////////////////////////////////////////////////////////////////////////

	return S_OK;
}

HRESULT KG3DTerrainBlock::CheckBorderMtl(KG3DTerrainBlock* pNear[],KG3DSceneDataInterface* lpDataInterface)
{
	if(pNear[0])
	{
		for (int Z=0;Z<=BLOCK_CELL_COUNT;Z++)
		{
			int nIndexA = Z * (BLOCK_CELL_COUNT + 1) + 16;
			int nIndexB = Z * (BLOCK_CELL_COUNT + 1) + 0;
			
			pNear[0]->m_byteTextureIndex[nIndexA] = m_byteTextureIndex[nIndexB];
		}
		pNear[0]->PocessDetailFactors(pNear[0]->m_byteTextureIndex);
	}
	if(pNear[2])
	{
		for (int X=0;X<=BLOCK_CELL_COUNT;X++)
		{
			int nIndexA = 16 * (BLOCK_CELL_COUNT + 1) + X;
			int nIndexB = 0  * (BLOCK_CELL_COUNT + 1) + X;

			pNear[2]->m_byteTextureIndex[nIndexA] = m_byteTextureIndex[nIndexB];
		}
		pNear[2]->PocessDetailFactors(pNear[2]->m_byteTextureIndex);
	}

	return S_OK;
}

int KG3DTerrainBlock::FindDetailTexture(BYTE bIndex)
{
	for (int i=0;i<m_nNumDetailTexture;i++)
	{
		if(m_byteDetailTextureIndex[i]==bIndex)
			return i;
	}
	return -1;
}

/*HRESULT KG3DTerrainBlock::SetBlendFactorPos(int nPos,int nX,int nZ)
{
	ASSERT(nX<=BLOCK_CELL_COUNT);
	ASSERT(nZ<=BLOCK_CELL_COUNT);
	ASSERT(nX>=0);
	ASSERT(nZ>=0);

	int nIndex = nZ * (BLOCK_CELL_COUNT+1) + nX;

	if(m_lpBlendFactor[0])
	{
		m_lpBlendFactor[0]->vBlend[nIndex] = D3DXVECTOR4(0,0,0,0);
	}
	if(m_lpBlendFactor[1])
	{
		m_lpBlendFactor[1]->vBlend[nIndex] = D3DXVECTOR4(0,0,0,0);
	}
	if(nPos<4)
	{
		if(m_lpBlendFactor[0])
		{
			SetBlendFactor(nPos,&m_lpBlendFactor[0]->vBlend[nIndex]);
			m_bNeedRefreshBlendFacetorVB[0] = TRUE;
		}
	}
	else
	{
		if(m_lpBlendFactor[1])
		{
			SetBlendFactor(nPos-4,&m_lpBlendFactor[1]->vBlend[nIndex]);
			m_bNeedRefreshBlendFacetorVB[1] = TRUE;
		}
	}

	return S_OK;
}*/

HRESULT KG3DTerrainBlock::ComputeMaterialKey()
{
    ASSERT(sizeof(m_ullMaterialKey) == sizeof(m_byteDetailTextureIndex));
    memcpy(&m_ullMaterialKey, m_byteDetailTextureIndex, sizeof(m_ullMaterialKey));
	return S_OK;
}

HRESULT KG3DTerrainBlock::CreateVertexWater()
{
	if(!m_lpWaterVertex)
	{
		m_lpWaterVertex = g_cTerrainWaterVertexPool.RequestResource();
		KGLOG_PROCESS_ERROR(m_lpWaterVertex);
		m_lpWaterVertex->Init(this);
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DTerrainBlock::ReleaseVertexWater()
{
	if(m_lpWaterVertex)
	{
		g_cTerrainWaterVertexPool.FreeResource(m_lpWaterVertex);
		m_lpWaterVertex = NULL;
	}
	return S_OK;
}

D3DXCOLOR ComputerWaterAlpha(float fHeight,D3DXCOLOR dwColor)
{
    const float cfLerpBound = g_cEngineOption.fWaterReduHeight;
    const float alphaMin = g_cEngineOption.fWaterAlphaMin;

    if (fHeight >= 0.f)
        return dwColor & 0x00ffffff;

    if ((fHeight = -fHeight) > cfLerpBound)
        return dwColor | 0xff000000;

    return (static_cast<DWORD>(((fHeight / cfLerpBound) * (1.f - alphaMin) + alphaMin) * 255 + 0.5f) << 24) | (dwColor & 0x00ffffff);



}

HRESULT KG3DTerrainBlock::CheckWaterVisible()
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
	if(!((m_dwLoadedState & LOADED_GROUNDVERTEX) && (m_dwLoadedState & LOADED_WATERVERTEX)))
		return E_FAIL;
	if(!m_lpWaterVertex)
		return S_OK;
	
	int nWidth = BLOCK_CELL_COUNT + 1;

	BOOL bVisable = FALSE;

	for (int nZ = 0;nZ <= BLOCK_CELL_COUNT;nZ++ )
	{
		for (int nX = 0;nX <= BLOCK_CELL_COUNT;nX++ )
		{
			int nIndex =  nZ * nWidth + nX;

			float Ground = GetVertexHeight(nX,nZ);
			float Water = m_lpWaterVertex->m_WaterVertexs[nIndex].p.y;

			float K = Ground - Water;

			m_lpWaterVertex->m_WaterVertexs[nIndex].diffuse = ComputerWaterAlpha(K,m_lpWaterVertex->m_WaterVertexs[nIndex].diffuse);

			if(Water>Ground)
			{
				bVisable = TRUE;
			}
		}
	}

    if (bVisable)
    {
        hrRetCode = _PickUpWaterBoundingPlanes();
        KGLOG_COM_PROCESS_ERROR(hrRetCode);
    }

	m_bWaterVisible = bVisable;
	if(!m_bWaterVisible)
	{
		//ReleaseVertexWater();
	}

	m_bNeedRefreshWaterVectexVB = TRUE;

    hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KG3DTerrainBlock::CreateGrassData()
{
	HRESULT hr = E_FAIL;

	if(!m_lpGrassData)
	{
		m_lpGrassData = g_cTerrainBlockGrassPool.RequestResource();
		KGLOG_PROCESS_ERROR(m_lpGrassData);
		m_lpGrassData->AddRef();
		hr = m_lpGrassData->Init(MAP_GRASS);
		KGLOG_COM_PROCESS_ERROR(hr);
		m_lpGrassData->m_lpParentTerrainBlock = this;
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DTerrainBlock::DeleteGrassData()
{
	SAFE_RELEASE(m_lpGrassData);
	return S_OK;
}

HRESULT KG3DTerrainBlock::OnTerrainBlockModified(DWORD dwDataType,KG3DSceneDataInterface* pDataInterface,KG3DSceneOutDoorSpaceMgr* pMgr,BOOL bMultiThread)
{
	HRESULT hr = S_OK;
	KG3DMemoryFile* pMemoryFile = g_cMemoryFilePool.RequestResource();
	POINT Index = {m_nIndex_X,m_nIndex_Z};

	KGLOG_PROCESS_ERROR(pDataInterface);
	KGLOG_PROCESS_ERROR(pMemoryFile);

	switch(dwDataType)
	{
	case Terrain_Block_GroundHeight:
		{
			hr = pDataInterface->GetTerrainBlockData(m_nLevel,Index,dwDataType,pMemoryFile->GetBuffer());
			KGLOG_COM_PROCESS_ERROR(hr);

			DWORD dwVersion = 0;

			pMemoryFile->Reset();
			pMemoryFile->read(&dwVersion,sizeof(DWORD));
			pMemoryFile->read(&m_fHeightMax,sizeof(float));
			pMemoryFile->read(&m_fHeightMin,sizeof(float));
			pMemoryFile->read(m_Vertexs,sizeof(m_Vertexs));
			//if(dwVersion == 1)
			//	pMemoryFile->read(m_VertexsEx,sizeof(m_VertexsEx));
			//FillVertexEx();
			m_bNeedRefreshVectexVB = TRUE;
			
			m_dwLoadedState |= LOADED_GROUNDVERTEX;
			if(!g_bClient)
				CheckWaterVisible();
		}
		break;
	case Terrain_Block_Water:
		{
			hr = pDataInterface->GetTerrainBlockData(m_nLevel,Index,dwDataType,pMemoryFile->GetBuffer());
			KGLOG_COM_PROCESS_ERROR(hr);

			CreateVertexWater();
			float fMax = 0,fMin = 0;

			if(m_lpWaterVertex)
			{
				DWORD dwVersion = 0;
				pMemoryFile->Reset();
				pMemoryFile->read(&dwVersion,sizeof(DWORD));
				pMemoryFile->read(&fMax,sizeof(float));
				pMemoryFile->read(&fMin,sizeof(float));
				pMemoryFile->read(m_lpWaterVertex->m_WaterVertexs,sizeof(m_lpWaterVertex->m_WaterVertexs));
			}
			m_dwLoadedState |= LOADED_WATERVERTEX;//这个一定要放在CheckWaterVisible前面
			CheckWaterVisible();	
			if(m_bWaterVisible)
			{
				m_fHeightMax = max(fMax,m_fHeightMax);
				m_fHeightMin = min(fMin,m_fHeightMin);
			}
		}
		break;
	case Terrain_Block_DetailMtl:
		{
			hr = pDataInterface->GetTerrainBlockData(m_nLevel,Index,dwDataType,pMemoryFile->GetBuffer());
			KGLOG_COM_PROCESS_ERROR(hr);

			//////////////////////////////////////////////////////////////////////////
			{
				DWORD dwVersion = 0;

				pMemoryFile->Reset();
				pMemoryFile->read(&dwVersion,sizeof(DWORD));
				pMemoryFile->read(&m_uHandleDetailMgr,sizeof(UINT));
				pMemoryFile->read(m_byteDetailTextureIndex,sizeof(m_byteDetailTextureIndex));
				pMemoryFile->read(&m_nNumDetailTexture,sizeof(int));
				pMemoryFile->read(m_byteTextureIndex,sizeof(BYTE)*17*17);
			}

			PocessDetailFactors(m_byteTextureIndex);
			ComputeMaterialKey();
		}
		break;
	case Terrain_Block_Grass:
		{
			if(!g_cEngineOption.bRenderGrass)
				break;

			hr = pDataInterface->GetTerrainBlockData(m_nLevel,Index,dwDataType,pMemoryFile->GetBuffer());
			KGLOG_COM_PROCESS_ERROR(hr);

			//////////////////////////////////////////////////////////////////////////
			pMemoryFile->Reset();

			DWORD dwVersion = 0;
			UINT  uHandleGrassSet = 0;
			DWORD dwNumGrass = 0;
			pMemoryFile->read(&dwVersion,sizeof(DWORD));
			hr = CreateGrassData();
			KGLOG_COM_PROCESS_ERROR(hr);
			if(dwVersion == 1)
				m_lpGrassData->m_bAutoGrass = TRUE;

			pMemoryFile->read(&uHandleGrassSet,sizeof(UINT));
			pMemoryFile->read(&dwNumGrass,sizeof(DWORD));
			
			m_lpGrassData->CreateGrass(dwNumGrass);
			m_lpGrassData->SetGrassSet(pMgr->m_lpSetPointer[MAP_GRASS],uHandleGrassSet);
			if(dwVersion == 1)
			{
				pMemoryFile->read(m_lpGrassData->m_lpAutoGrassData,sizeof(DWORD)*GRASS_NUM * GRASS_NUM);
				pMemoryFile->read(m_lpGrassData->m_lpAutoGrassWidth,sizeof(float)*GRASS_NUM * GRASS_NUM);
				pMemoryFile->read(m_lpGrassData->m_lpAutoGrassHeight,sizeof(float)*GRASS_NUM * GRASS_NUM);	
			}
			else
			{
				for (DWORD i=0;i<dwNumGrass;i++)
				{
					KG3DTerrainGrass* pGrassDest = &m_lpGrassData->m_lpGrassData[i];

					pMemoryFile->read(pGrassDest,sizeof(KG3DTerrainGrass));
					if(!g_bClient)
						m_lpGrassData->m_listGrass.push_back(*pGrassDest);
				}
				
			}
			m_lpGrassData->CreateGrassVertex();
		}	
		break;
	case Terrain_Block_Stone:
		{
			if(!g_cEngineOption.bRenderGrass)
				break;

			hr = pDataInterface->GetTerrainBlockData(m_nLevel,Index,dwDataType,pMemoryFile->GetBuffer());
			KGLOG_COM_PROCESS_ERROR(hr);

			//////////////////////////////////////////////////////////////////////////
			pMemoryFile->Reset();

			DWORD dwVersion = 0;
			UINT  uHandleStoneSet = 0;
			DWORD dwNumStone = 0;
			pMemoryFile->read(&dwVersion,sizeof(DWORD));
			pMemoryFile->read(&uHandleStoneSet,sizeof(UINT));
			pMemoryFile->read(&dwNumStone,sizeof(DWORD));

			hr = CreateStoneData();
			KGLOG_COM_PROCESS_ERROR(hr);
			m_lpStoneData->CreateGrass(dwNumStone);
			m_lpStoneData->SetGrassSet(pMgr->m_lpSetPointer[MAP_STONE],uHandleStoneSet);

			for (DWORD i=0;i<dwNumStone;i++)
			{
				KG3DTerrainGrass* pStoneDest = &m_lpStoneData->m_lpGrassData[i];

				pMemoryFile->read(pStoneDest,sizeof(KG3DTerrainGrass));
				if(!g_bClient)
					m_lpStoneData->m_listGrass.push_back(*pStoneDest);
			}

			m_lpStoneData->CreateGrassVertex();
		}
		break;
	case Terrain_Block_Frutex:
		{
			if(!g_cEngineOption.bRenderGrass)
				break;

			hr = pDataInterface->GetTerrainBlockData(m_nLevel,Index,dwDataType,pMemoryFile->GetBuffer());
			KGLOG_COM_PROCESS_ERROR(hr);

			//////////////////////////////////////////////////////////////////////////
			pMemoryFile->Reset();

			DWORD dwVersion = 0;
			UINT  uHandleFrutexSet = 0;
			DWORD dwNumFrutex = 0;
			pMemoryFile->read(&dwVersion,sizeof(DWORD));
			pMemoryFile->read(&uHandleFrutexSet,sizeof(UINT));
			pMemoryFile->read(&dwNumFrutex,sizeof(DWORD));

			hr = CreateFrutexData();
			KGLOG_COM_PROCESS_ERROR(hr);
			m_lpFrutexData->CreateGrass(dwNumFrutex);
			m_lpFrutexData->SetGrassSet(pMgr->m_lpSetPointer[MAP_FRUTEX],uHandleFrutexSet);

			for (DWORD i=0;i<dwNumFrutex;i++)
			{
				KG3DTerrainGrass* pFrutexDest = &m_lpFrutexData->m_lpGrassData[i];

				pMemoryFile->read(pFrutexDest,sizeof(KG3DTerrainGrass));
				if(!g_bClient)
					m_lpFrutexData->m_listGrass.push_back(*pFrutexDest);
			}

			m_lpFrutexData->CreateGrassVertex();
		}
		break;
	case Terrain_Block_Road:
		{
			//DWORD dwLen = 0;
			hr = pDataInterface->GetTerrainBlockData(m_nLevel,Index,dwDataType,pMemoryFile->GetBuffer());
			KGLOG_COM_PROCESS_ERROR(hr);
			pMemoryFile->Reset();
			DWORD dwNum = 0;
			DWORD dwVersion = 0;
			UINT uHandle = 0;
			pMemoryFile->Reset();
			pMemoryFile->read(&dwVersion,sizeof(DWORD));
			pMemoryFile->read(&dwNum,sizeof(DWORD));
			for (DWORD i= 0;i<dwNum;i++)
			{
				KG3DTerrainRoad* pRoad =NULL; 
				pMemoryFile->read(&uHandle,sizeof(UINT));
				hr = pMgr->m_lpRoadMgr->LoadByHandle(uHandle,&pRoad);
				if(SUCCEEDED(hr))
					m_mapRoad.insert(std::pair<UINT,KG3DTerrainRoad*>(uHandle,pRoad));
				
			}
		
		}
		break;
	case Terrain_Block_River:
		{
			hr = pDataInterface->GetTerrainBlockData(m_nLevel,Index,dwDataType,pMemoryFile->GetBuffer());
			KGLOG_COM_PROCESS_ERROR(hr);
			pMemoryFile->Reset();
			DWORD dwNum = 0;
			DWORD dwVersion = 0;
			UINT uHandle = 0;
			pMemoryFile->Reset();
			pMemoryFile->read(&dwVersion,sizeof(DWORD));
			pMemoryFile->read(&dwNum,sizeof(DWORD));
			for (DWORD i= 0;i<dwNum;i++)
			{
				KG3DTerrainRiver* pRiver =NULL; 
				pMemoryFile->read(&uHandle,sizeof(UINT));
				hr = pMgr->m_lpRiverMgr->LoadByHandle(uHandle,&pRiver);
				if(SUCCEEDED(hr))
					m_mapRiver.insert(std::pair<UINT,KG3DTerrainRiver*>(uHandle,pRiver));

			}
		}
		break;
	}

	hr = S_OK;
Exit0:
	if(pMemoryFile)
	{
		g_cMemoryFilePool.FreeResource(pMemoryFile);
		pMemoryFile = NULL;
	}
	return hr;
}

KG3DTerrainBlockGrass* KG3DTerrainBlock::GetTerrainGrassBlock(BOOL bCreate /* = FALSE*/)
{
	if(bCreate)
		CreateGrassData();
	return m_lpGrassData;
}
KG3DTerrainBlockGrass* KG3DTerrainBlock::GetTerrainStoneBlock(BOOL bCreate /* = FALSE*/)
{
	if(bCreate)
		CreateStoneData();
	return m_lpStoneData;
}

KG3DTerrainBlockGrass* KG3DTerrainBlock::GetTerrainFrutexBlock(BOOL bCreate /* = FALSE*/)
{
	if(bCreate)
		CreateFrutexData();
	return m_lpFrutexData;
}

HRESULT KG3DTerrainBlock::CreateStoneData()
{
	HRESULT hr = E_FAIL;

	if(!m_lpStoneData)
	{
		m_lpStoneData = g_cTerrainBlockGrassPool.RequestResource();
		KGLOG_PROCESS_ERROR(m_lpStoneData);
		m_lpStoneData->AddRef();
		hr = m_lpStoneData->Init(MAP_STONE);
		KGLOG_COM_PROCESS_ERROR(hr);
		m_lpStoneData->m_lpParentTerrainBlock = this;
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DTerrainBlock::DeleteStoneData()
{
	SAFE_RELEASE(m_lpStoneData);
	return S_OK;
}

HRESULT KG3DTerrainBlock::CreateFrutexData()
{
	HRESULT hr = E_FAIL;

	if(!m_lpFrutexData)
	{
		m_lpFrutexData = g_cTerrainBlockGrassPool.RequestResource();
		KGLOG_PROCESS_ERROR(m_lpFrutexData);
		m_lpFrutexData->AddRef();
		hr = m_lpFrutexData->Init(MAP_FRUTEX);
		KGLOG_COM_PROCESS_ERROR(hr);
		m_lpFrutexData->m_lpParentTerrainBlock = this;
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DTerrainBlock::DeleteFrutexData()
{
	SAFE_RELEASE(m_lpFrutexData);
	return S_OK;
}

HRESULT KG3DTerrainBlock::GSFFitGround(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr)
{
	if (m_lpGrassData)
		m_lpGrassData->FitGround(pOutDoorSpaceMgr);
	if (m_lpStoneData)
		m_lpStoneData->FitGround(pOutDoorSpaceMgr);
	if (m_lpFrutexData)
		m_lpFrutexData->FitGround(pOutDoorSpaceMgr);
	return S_OK;
}

HRESULT KG3DTerrainBlock::PaintTexture(D3DXVECTOR2& vA,D3DXVECTOR2& vC,BYTE TextureIndex)
{
	int BrushX=0,BrushZ=0;
	float fBlockLength,fCellWidth;
	float fBrushSize = (vC.x - vA.x)/2.0f;
	D3DXVECTOR2 BrushPos(vA.x+fBrushSize,vA.y + fBrushSize);
	SceneBlockFindLength(m_nLevel,fBlockLength,fCellWidth);
	D3DXVECTOR2 vStartPos(m_nIndex_X * fBlockLength,m_nIndex_Z * fBlockLength);
	int nXStart = (int)((vA.x - vStartPos.x)/fCellWidth);
	int nZStart = (int)((vA.y - vStartPos.y)/fCellWidth);
	int nXEnd   = (int)((vC.x - vStartPos.x)/fCellWidth);
	int nZEnd   = (int)((vC.y - vStartPos.y)/fCellWidth);
	nXStart = max(0,nXStart);
	nZStart = max(0,nZStart);
	nXEnd   = min(16,nXEnd);
	nZEnd   = min(16,nZEnd); 

	D3DXVECTOR2 vPos;
	float fWeight = 0;
	for (int nZ = nZStart ; nZ <= nZEnd; nZ++)
	{
		vPos.y = (vStartPos.y + nZ * fCellWidth);
		BrushZ = (int)((vPos.y - vA.y)/100);
		for(int nX = nXStart; nX <= nXEnd;nX++)
		{
			vPos.x = vStartPos.x + nX * fCellWidth;	
			BOOL bPaint = FALSE;
			int Index = nZ * 17 + nX;
			if(fBrushSize <= 200.0f)
			{
				float d = (vPos.x - BrushPos.x) * (vPos.x - BrushPos.x) + (vPos.y - BrushPos.y) * (vPos.y - BrushPos.y);
				if( d < (fBrushSize * fBrushSize))
					bPaint = TRUE;
			}
			else
			{
				BrushX = (int)((vPos.x - vA.x)/100);
				fWeight = g_cTerrainBlockModifier.m_Brush.GetWeight(BrushX,BrushZ);
				if(fWeight > g_cTerrainBlockModifier.m_fAlphaTest)
					bPaint = TRUE;
			}
			if( bPaint)
			{
				m_byteTextureIndex[Index] = TextureIndex;
				if(m_nLevel == BLOCK_TYPE_REGION)
				{
					if(g_cTerrainBlockModifier.m_bAddAutoGrass)
					{
						KG3DTerrainBlockGrass* pGrassBlock = GetTerrainGrassBlock(TRUE);
						if( pGrassBlock)
						{
							pGrassBlock->SetAutoGrassData(nX,nZ,FALSE,0,0,0,0,0); //先清除原有的，
							if(rand() % g_cTerrainBlockModifier.GetGrassDensity() == 0)
							{
								BYTE byGrassTexIndex = (BYTE)g_cTerrainBlockModifier.GetRandomTextureIndex();
								BYTE byPattren = (BYTE)g_cTerrainBlockModifier.GetPatternIndexBySelection();
								float SWidth =max(5.0f, g_cTerrainBlockModifier.GetWidthMax() - g_cTerrainBlockModifier.GetWidthMin());
								float fWidth = g_cTerrainBlockModifier.GetWidthMin() + rand() % 100 / 100.0F * SWidth;
								float SHeight = max(5.0f, g_cTerrainBlockModifier.GetHeightMax() - g_cTerrainBlockModifier.GetHeightMin());
								float fHeight = g_cTerrainBlockModifier.GetHeightMin() + rand()%100/100.0F*SHeight;
								pGrassBlock->SetAutoGrassData(nX,nZ,TRUE,TextureIndex,byGrassTexIndex,byPattren,fWidth,fHeight);
							}
						}
					}
					else
					{
						KG3DTerrainBlockGrass* pGrassBlock = GetTerrainGrassBlock();
						if( pGrassBlock && pGrassBlock->m_bAutoGrass)
						{
							pGrassBlock->SetAutoGrassData(nX,nZ,FALSE,0,0,0,0,0);//刷除该标志位
						}
					}
				}	
				
			}
		}
	}

	

	return S_OK;
}

HRESULT KG3DTerrainBlock::ModifyDetailTexture(KG3DSceneDataInterface* lpDataInterface)
{
	int       nWidth                    = BLOCK_CELL_COUNT + 1;
	const int cnMaxByteValue            = 256;
	int       nTextNum[cnMaxByteValue]  = {0};
	memset(nTextNum, 0, sizeof(nTextNum));
	for (int nZ = 0;nZ <= BLOCK_CELL_COUNT;nZ++ )
	{
		for (int nX = 0;nX <= BLOCK_CELL_COUNT;nX++ )
		{
			int nIndex =  nZ * nWidth + nX;
			BYTE bText = m_byteTextureIndex[nIndex];
			nTextNum[bText]++;
		}
	}
	{
		int   K		   = 0;
		int   nMaxNum  = 0;
		int  bIndexID = -1;
		for (K = 0; K < TERRAIN_BLOCK_MAX_DETAILMAP; K++)
		{   
			nMaxNum = 0;
			bIndexID = -1;

			for (int i = 0; i < cnMaxByteValue; i++)
			{
				if (nMaxNum < nTextNum[i])
				{
					nMaxNum = nTextNum[i];
					bIndexID = i;
				}
			}
			if (bIndexID == -1)
				break;

			m_byteDetailTextureIndex[K] = (BYTE)bIndexID;
			_ASSERTE(bIndexID < _countof(nTextNum));
			nTextNum[bIndexID] = 0;
		}

		m_nNumDetailTexture = K;    
	}
 
	PocessDetailFactors(m_byteTextureIndex);
	ComputeMaterialKey();
	//////////////////////////////////////////////////////////////////////////
	{
		DWORD dwVersion = 0;
		KG3DMemoryFile* pFile = g_cMemoryFilePool.RequestResource();
		if(pFile)
		{
			//pFile->Reset();
			pFile->write(&dwVersion,sizeof(DWORD));
			pFile->write(&m_uHandleDetailMgr,sizeof(UINT));
			pFile->write(m_byteDetailTextureIndex,sizeof(m_byteDetailTextureIndex));
			pFile->write(&m_nNumDetailTexture,sizeof(int));
			pFile->write(m_byteTextureIndex,sizeof(BYTE)*17*17);
			DWORD dwLength = pFile->GetLength();

			POINT Index = {m_nIndex_X,m_nIndex_Z};
			lpDataInterface->Request_ModifyTerrainBlock(m_nLevel,Index,Terrain_Block_DetailMtl,dwLength,pFile->GetBuffer());
			g_cMemoryFilePool.FreeResource(pFile);
			pFile = NULL;
		}

	}
	//////////////////////////////////////////////////////////////////////////

	return S_OK;
}
/////道路/////////////////////////////////
HRESULT KG3DTerrainBlock::InputRoad(KG3DTerrainRoad* pRoad,KG3DSceneDataInterface* lpDataInterface)
{
	KG_PROCESS_ERROR(pRoad);
	KG_PROCESS_ERROR(lpDataInterface);
	m_mapRoad.insert(std::pair<UINT,KG3DTerrainRoad*>(pRoad->m_uHandle,pRoad));
	{
		KG3DTerrainRoad::TerrainBlockInfo TBlockInfo;

		TBlockInfo.Index.x = m_nIndex_X;
		TBlockInfo.Index.y = m_nIndex_Z;
		TBlockInfo.nLevel = m_nLevel;
		pRoad->m_vecParentTerrainBlock.push_back(TBlockInfo);
	}
	
	ModifyRoadData(lpDataInterface);
	return S_OK;
Exit0:
	return E_FAIL;
}
HRESULT KG3DTerrainBlock::EraseRoad(UINT uHandle,KG3DSceneDataInterface* lpDataInterface)
{
	KG_PROCESS_ERROR(lpDataInterface);
	m_mapRoad.erase(uHandle);
	ModifyRoadData(lpDataInterface);
	return S_OK;
Exit0:
	return E_FAIL;
}
HRESULT KG3DTerrainBlock::ModifyRoadData(KG3DSceneDataInterface* lpDataInterface)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(lpDataInterface);
	{
		map<UINT,KG3DTerrainRoad*>::iterator i = m_mapRoad.begin();
		DWORD dwNum = (DWORD)m_mapRoad.size();
		DWORD dwVersion = 0;
		DWORD dwLength=0;

		KG3DMemoryFile* pFile = g_cMemoryFilePool.RequestResource();
		if(pFile)
		{
			pFile->write(&dwVersion,sizeof(DWORD));
			pFile->write(&dwNum,sizeof(DWORD));

			while (i != m_mapRoad.end())
			{
				UINT uHandle = i->first;
				pFile->write(&uHandle,sizeof(UINT));	
				++i;
			}
			dwLength = pFile->GetLength();

			POINT Index = {m_nIndex_X,m_nIndex_Z};
			hr = lpDataInterface->Request_ModifyTerrainBlock(m_nLevel,Index,Terrain_Block_Road,dwLength,pFile->GetBuffer());

			g_cMemoryFilePool.FreeResource(pFile);
			pFile = NULL;
		}

	}
	//////////////////////////////////////////////////////////////////////////
Exit0:
	return hr;
}
HRESULT KG3DTerrainBlock::GetVisibleRoad(map<int,KG3DTerrainRoad*>* pmapRoad,const KG3DFrustum* pFrustum)
{
	map<UINT,KG3DTerrainRoad*>::iterator i = m_mapRoad.begin();
	while (i != m_mapRoad.end())
	{
		KG3DTerrainRoad* pRoad = i->second;
		if(pRoad && pFrustum->IsAABBoxVisible(pRoad->m_BBox))
		{
			bool bSecceeded = (pmapRoad->insert(std::pair<int,KG3DTerrainRoad*>(pRoad->m_nID,pRoad))).second;
			if(bSecceeded)
			{
				pRoad->AddRef();
				pRoad->CheckVisablePassage(pFrustum);
			}
		}
		++i;
	}
	return S_OK;
}

/////河流/////////////////////////////////
HRESULT KG3DTerrainBlock::InputRiver(KG3DTerrainRiver* pRiver,KG3DSceneDataInterface* lpDataInterface)
{
	KG_PROCESS_ERROR(pRiver);
	KG_PROCESS_ERROR(lpDataInterface);
	m_mapRiver.insert(std::pair<UINT,KG3DTerrainRiver*>(pRiver->m_uHandle,pRiver));
	{
		KG3DTerrainRiver::TerrainBlockInfo TBlockInfo;

		TBlockInfo.Index.x = m_nIndex_X;
		TBlockInfo.Index.y = m_nIndex_Z;
		TBlockInfo.nLevel = m_nLevel;
		pRiver->m_vecParentTerrainBlock.push_back(TBlockInfo);
	}

	ModifyRiverData(lpDataInterface);
	return S_OK;
Exit0:
	return E_FAIL;
}
HRESULT KG3DTerrainBlock::EraseRiver(UINT uHandle,KG3DSceneDataInterface* lpDataInterface)
{
	KG_PROCESS_ERROR(lpDataInterface);
	m_mapRiver.erase(uHandle);
	ModifyRiverData(lpDataInterface);
	return S_OK;
Exit0:
	return E_FAIL;
}
HRESULT KG3DTerrainBlock::ModifyRiverData(KG3DSceneDataInterface* lpDataInterface)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(lpDataInterface);
	{
		map<UINT,KG3DTerrainRiver*>::iterator i = m_mapRiver.begin();
		DWORD dwNum = (DWORD)m_mapRiver.size();
		DWORD dwVersion = 0;
		DWORD dwLength=0;

		KG3DMemoryFile* pFile = g_cMemoryFilePool.RequestResource();
		if(pFile)
		{
			pFile->write(&dwVersion,sizeof(DWORD));
			pFile->write(&dwNum,sizeof(DWORD));

			while (i != m_mapRiver.end())
			{
				UINT uHandle = i->first;
				pFile->write(&uHandle,sizeof(UINT));	
				++i;
			}
			dwLength = pFile->GetLength();

			POINT Index = {m_nIndex_X,m_nIndex_Z};
			hr = lpDataInterface->Request_ModifyTerrainBlock(m_nLevel,Index,Terrain_Block_River,dwLength,pFile->GetBuffer());
			g_cMemoryFilePool.FreeResource(pFile);
			pFile = NULL;
		}

	}
	//////////////////////////////////////////////////////////////////////////
Exit0:
	return hr;
}
HRESULT KG3DTerrainBlock::GetVisibleRiver(set<KG3DTerrainRiver*>* psetRiver,const KG3DFrustum* pFrustum)
{
	set<KG3DTerrainRiver*>::iterator iF;
	map<UINT,KG3DTerrainRiver*>::iterator i = m_mapRiver.begin();
	while (i != m_mapRiver.end())
	{
		KG3DTerrainRiver* pRiver = i->second;
		if(pRiver && pFrustum->IsAABBoxVisible(pRiver->m_BBox))
		{
			iF = psetRiver->find(pRiver);
			if(iF == psetRiver->end())
			{
				pRiver->AddRef();
				psetRiver->insert(pRiver);
				pRiver->CheckVisablePassage(pFrustum);
			}
		}
		++i;
	}
	return S_OK;
}

HRESULT KG3DTerrainBlock::PrepareRender()
{
	KG_PROCESS_ERROR(m_bLoaded);

	DWORD dwThreadID = GetCurrentThreadId();
	/*if(dwThreadID != g_dwMainThreadID)
	{
		int k = 0;
	}*/
	KG_PROCESS_ERROR(dwThreadID == g_dwMainThreadID);


	PrepareSharedGroundVB();
	if(m_bWaterVisible)
		PrepareSharedWaterVB();

    PrepareSharedBlendFactorVB();
	
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DTerrainBlock::UnPrepareRender()
{
	KG_PROCESS_ERROR(m_bLoaded);

	UnPrepareSharedGroundVB();
	UnPrepareSharedWaterVB();
	UnPrepareSharedBlendFactorVB();

	return S_OK;
Exit0:
	return E_FAIL;
}

///水面///////////////////////////////////////
HRESULT KG3DTerrainBlock::SetWaterHeight(D3DXVECTOR2& vA,D3DXVECTOR2& vC,float fHeight,KG3DSceneDataInterface* pDataInterface,BOOL bToFile)
{
	KGLOG_PROCESS_ERROR(m_lpWaterVertex);
	{
		float fBlockLength,fCellWidth;
		SceneBlockFindLength(m_nLevel,fBlockLength,fCellWidth);
		D3DXVECTOR2 vStartPos(m_nIndex_X * fBlockLength,m_nIndex_Z * fBlockLength);
		int nXStart = (int)(vA.x - vStartPos.x)/(int)fCellWidth;
		int nZStart = (int)(vA.y - vStartPos.y)/(int)fCellWidth;
		int nXEnd   = (int)(vC.x - vStartPos.x)/(int)fCellWidth;
		int nZEnd   = (int)(vC.y - vStartPos.y)/(int)fCellWidth;
		nXStart = max(0,nXStart);
		nZStart = max(0,nZStart);
		nXEnd   = min(16,nXEnd);
		nZEnd   = min(16,nZEnd); 
		for (int nZ = nZStart ; nZ <= nZEnd; nZ++)
		{
			for(int nX = nXStart; nX <= nXEnd;nX++)
			{
				float *pHeight = m_lpWaterVertex->GetVertexPointer(nX,nZ);
				(*pHeight) = fHeight;		
			}
		}
		m_fHeightMax = max(fHeight,m_fHeightMax);
		m_fHeightMin = min(fHeight,m_fHeightMin);

		m_bNeedRefreshWaterVectexVB = TRUE;

		CheckWaterVisible();

		if(bToFile && m_lpWaterVertex)
		{
			DWORD dwVersion = 0;
			DWORD dwLength = 0;

			KG3DMemoryFile* pFile = g_cMemoryFilePool.RequestResource();
			if(pFile)
			{
				pFile->write(&dwVersion,sizeof(DWORD));
				pFile->write(&m_fHeightMax,sizeof(float));
				pFile->write(&m_fHeightMin,sizeof(float));
				pFile->write(m_lpWaterVertex->m_WaterVertexs,sizeof(m_lpWaterVertex->m_WaterVertexs));

				dwLength = pFile->GetLength();

				POINT Index = {m_nIndex_X,m_nIndex_Z};
				pDataInterface->Request_ModifyTerrainBlock(m_nLevel,Index,Terrain_Block_Water,dwLength,pFile->GetBuffer());
				g_cMemoryFilePool.FreeResource(pFile);
				pFile = NULL;
			}
		}
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DTerrainBlock::PaintTextureByFilterMap(float& fWidth,float& fHeight,D3DSURFACE_DESC& desc,DWORD* pColor,BYTE byTexIndex)
{
	int   nWidth = BLOCK_CELL_COUNT + 1;
	int nU,nV; 
	for (int nZ = 0;nZ <= BLOCK_CELL_COUNT;nZ++ )
	{
		for (int nX = 0;nX <= BLOCK_CELL_COUNT;nX++ )
		{
			int nIndex =  nZ * nWidth + nX;

			float fPercentX = m_Vertexs[nIndex].p.x / fWidth;
			float fPercentZ = m_Vertexs[nIndex].p.z / fHeight;
			//_ASSERT(fPercentX <= 1);
			//_ASSERT(fPercentZ <= 1);
			if(m_byteTextureIndex[nIndex] == byTexIndex)
				m_byteTextureIndex[nIndex] = 0;
			if(fPercentX <= 1 && fPercentZ <= 1)
			{
				nU = (int)((desc.Width-1) * fPercentX);
				nV = (int)((desc.Height-1)* fPercentZ);
				int i = nV * desc.Width +  nU; 		
				BYTE byTest = (BYTE)((pColor[i] & 0xff000000) >> 24);
				if(byTest >= 128)
					m_byteTextureIndex[nIndex] = byTexIndex;
			}
		}
	}

	return S_OK;
}
HRESULT KG3DTerrainBlock::PaintToNormalMap(float& fWidth,float& fHeight,D3DSURFACE_DESC& desc,DWORD* pColor)
{
	int   nWidth = BLOCK_CELL_COUNT + 1;
	int nU,nV; 
	for (int nZ = 0;nZ <= BLOCK_CELL_COUNT;nZ++ )
	{
		for (int nX = 0;nX <= BLOCK_CELL_COUNT;nX++ )
		{
			int nIndex =  nZ * nWidth + nX;
		
			float fPercentX = m_Vertexs[nIndex].p.x / fWidth;
			float fPercentZ = m_Vertexs[nIndex].p.z / fHeight;
			if(fPercentX <= 1 && fPercentZ <= 1)
			{
				nU = (int)((desc.Width-1) * fPercentX);
				nV = (int)((desc.Height-1)* fPercentZ);
				int i = nV * desc.Width +  nU; 
				float r = (m_Vertexs[nIndex].Normal.x + 1.0f) / 2.0f * 255.f;
				float g = (m_Vertexs[nIndex].Normal.y + 1.0f) / 2.0f * 255.f;
				float b = (m_Vertexs[nIndex].Normal.z + 1.0f) / 2.0f * 255.f;
				float a = m_lpBlendFactor == NULL ? 0 : ((m_lpBlendFactor->vBlend[nIndex]/7.0f) * 255.f);
				pColor[i] = D3DCOLOR_ARGB((DWORD)a,(DWORD)r,(DWORD)g,(DWORD)b);						
			}
		}
	}

	return S_OK;
}
HRESULT KG3DTerrainBlock::PaintToFilterMap(float& fWidth,float& fHeight,D3DSURFACE_DESC& desc,DWORD* pColor,BYTE byTexIndex)
{
	int   nWidth = BLOCK_CELL_COUNT + 1;
	int nU,nV; 
	for (int nZ = 0;nZ <= BLOCK_CELL_COUNT;nZ++ )
	{
		for (int nX = 0;nX <= BLOCK_CELL_COUNT;nX++ )
		{
			int nIndex =  nZ * nWidth + nX;
			if(m_byteTextureIndex[nIndex] == byTexIndex)
			{
				float fPercentX = m_Vertexs[nIndex].p.x / fWidth;
				float fPercentZ = m_Vertexs[nIndex].p.z / fHeight;
				if(fPercentX <= 1 && fPercentZ <= 1)
				{
					nU = (int)((desc.Width-1) * fPercentX);
					nV = (int)((desc.Height-1)* fPercentZ);
					int i = nV * desc.Width +  nU; 
					pColor[i] = 0xC0C0C0C0;						
				}
			}
		}
	}

	return S_OK;
}
HRESULT KG3DTerrainBlock::AddAutoGrassByBrush(D3DXVECTOR2& vA,D3DXVECTOR2& vC)
{
	int BrushX=0,BrushZ=0;
	float fBlockLength,fCellWidth;
	float fBrushSize = (vC.x - vA.x)/2.0f;
	D3DXVECTOR2 BrushPos(vA.x+fBrushSize,vA.y + fBrushSize);
	BYTE byTextureIndex = g_cTerrainBlockModifier.m_byPaintTexIndex;
	SceneBlockFindLength(m_nLevel,fBlockLength,fCellWidth);
	D3DXVECTOR2 vStartPos(m_nIndex_X * fBlockLength,m_nIndex_Z * fBlockLength);
	int nXStart = (int)((vA.x - vStartPos.x)/fCellWidth);
	int nZStart = (int)((vA.y - vStartPos.y)/fCellWidth);
	int nXEnd   = (int)((vC.x - vStartPos.x)/fCellWidth);
	int nZEnd   = (int)((vC.y - vStartPos.y)/fCellWidth);
	nXStart = max(0,nXStart);
	nZStart = max(0,nZStart);
	nXEnd   = min(16,nXEnd);
	nZEnd   = min(16,nZEnd); 

	D3DXVECTOR2 vPos;
	float fWeight = 0;
	for (int nZ = nZStart ; nZ <= nZEnd; nZ++)
	{
		vPos.y = (vStartPos.y + nZ * fCellWidth);
		BrushZ = (int)((vPos.y - vA.y)/100);
		for(int nX = nXStart; nX <= nXEnd;nX++)
		{
			vPos.x = vStartPos.x + nX * fCellWidth;	
			BOOL bPaint = FALSE;
			int Index = nZ * 17 + nX;
			if(fBrushSize <= 200.0f)
			{
				float d = (vPos.x - BrushPos.x) * (vPos.x - BrushPos.x) + (vPos.y - BrushPos.y) * (vPos.y - BrushPos.y);
				if( d < (fBrushSize * fBrushSize))
					bPaint = TRUE;
			}
			else
			{
				BrushX = (int)((vPos.x - vA.x)/100);
				fWeight = g_cTerrainBlockModifier.m_Brush.GetWeight(BrushX,BrushZ);
				if(fWeight > g_cTerrainBlockModifier.m_fAlphaTest)
					bPaint = TRUE;
			}
			if( bPaint)
			{
				if(m_nLevel == BLOCK_TYPE_REGION)
				{
					if(g_cTerrainBlockModifier.m_bAddAutoGrass && (m_byteTextureIndex[Index] == byTextureIndex))
					{
						KG3DTerrainBlockGrass* pGrassBlock = GetTerrainGrassBlock(TRUE);
						if( pGrassBlock)
						{
							pGrassBlock->SetAutoGrassData(nX,nZ,FALSE,0,0,0,0,0); //先清除原有的，
							if(rand() % g_cTerrainBlockModifier.GetGrassDensity() == 0)
							{
								BYTE byGrassTexIndex = (BYTE)g_cTerrainBlockModifier.GetRandomTextureIndex();
								BYTE byPattren = (BYTE)g_cTerrainBlockModifier.GetPatternIndexBySelection();
								float SWidth =max(5.0f, g_cTerrainBlockModifier.GetWidthMax() - g_cTerrainBlockModifier.GetWidthMin());
								float fWidth = g_cTerrainBlockModifier.GetWidthMin() + rand() % 100 / 100.0F * SWidth;
								float SHeight = max(5.0f, g_cTerrainBlockModifier.GetHeightMax() - g_cTerrainBlockModifier.GetHeightMin());
								float fHeight = g_cTerrainBlockModifier.GetHeightMin() + rand()%100/100.0F*SHeight;
								pGrassBlock->SetAutoGrassData(nX,nZ,TRUE,byTextureIndex,byGrassTexIndex,byPattren,fWidth,fHeight);
							}	
						}
					}
				}	

			}
		}
	}



	return S_OK;
}

HRESULT KG3DTerrainConverMap::Default()
{
	m_vRect = D3DXVECTOR4(-10000,-10000,20000,20000);
	SAFE_RELEASE(m_lpColorTex);

	m_lpColorTex = g_cTextureTable.GetConverTexture();
	if(NULL == m_lpColorTex)
		return E_FAIL;
	
	m_lpColorTex->AddRef();

	return S_OK;
}

HRESULT KG3DTerrainConverMap::UnInit()
{
	SAFE_RELEASE(m_lpColorTex);
	SAFE_RELEASE(m_lpTextureLight);
	SAFE_RELEASE(m_lpTextureCloud);
	SAFE_RELEASE(m_lpRenderTarget);
	return S_OK;
}
static KG3DTerrainConverMap s_ConverMap;
KG3DTerrainConverMap& KG3DTerrainConverMap::GetUniqueDefault()
{
	return s_ConverMap;
}

HRESULT KG3DTerrainConverMap::InitUniqueDefault()
{
	return s_ConverMap.Default();
}	

HRESULT KG3DTerrainConverMap::UnInitUniqueDefault()
{
	return s_ConverMap.UnInit();
}

HRESULT KG3DTerrainBlock::SetPointLightInfo(KG3D_PointLight_Info& vInfo)
{
	if (g_bEnablePointLight)
	{
		if(vInfo.m_nNumPointLights)
		{
			if(!m_lpPointLightInfo)
			{
				m_lpPointLightInfo = g_cPointLightInfoPool.RequestResource();
			}
			KGLOG_PROCESS_ERROR(m_lpPointLightInfo);

			(*m_lpPointLightInfo) = vInfo;
		}
		else
		{
			if(m_lpPointLightInfo)
			{
				g_cPointLightInfoPool.FreeResource(m_lpPointLightInfo);
				m_lpPointLightInfo = NULL;
			}
		}
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DTerrainBlock::_PickUpWaterBoundingPlanes()
{
    HRESULT hrResult = E_FAIL;
    map<int, _RECTANGLE>::iterator itFinder, itIndex;
    
    m_mapWaterBoundingPlanes.clear();

    for (int nZ = 0; nZ <= BLOCK_CELL_COUNT; ++nZ)
    {
        for (int nX = 0; nX <= BLOCK_CELL_COUNT; ++nX)
        {
            int nIndex = nZ * (BLOCK_CELL_COUNT + 1) + nX;
            float fWaterHeight = m_lpWaterVertex->m_WaterVertexs[nIndex].p.y;
            float fGroundHeight = GetVertexHeight(nX,nZ);

            if (fGroundHeight >= fWaterHeight)
                continue;
            
            int nWaterHeight = (int)(fWaterHeight + 0.5); // 四舍五入

            itFinder = m_mapWaterBoundingPlanes.find(nWaterHeight);
            if (itFinder == m_mapWaterBoundingPlanes.end())
            {
                _RECTANGLE WaterBoundingPlane;
                BOOL bInHorizontalPlane = FALSE;

                if (!bInHorizontalPlane && (nX + 1 <= BLOCK_CELL_COUNT))
                {
                    bInHorizontalPlane = (nWaterHeight == (int)(m_lpWaterVertex->m_WaterVertexs[nIndex + 1].p.y + 0.5));
                }

                if (!bInHorizontalPlane && (nZ + 1 <= BLOCK_CELL_COUNT))
                {
                    bInHorizontalPlane = (nWaterHeight == (int)(m_lpWaterVertex->m_WaterVertexs[nIndex + BLOCK_CELL_COUNT].p.y + 0.5));
                }

                if (!bInHorizontalPlane && (nX + 1 <= BLOCK_CELL_COUNT) && (nZ + 1 <= BLOCK_CELL_COUNT))
                {
                    bInHorizontalPlane = (nWaterHeight == (int)(m_lpWaterVertex->m_WaterVertexs[nIndex + BLOCK_CELL_COUNT + 1].p.y + 0.5));
                }

                if (bInHorizontalPlane)
                {
                    WaterBoundingPlane.vLeftBottomPos = m_lpWaterVertex->m_WaterVertexs[nIndex].p;
                    WaterBoundingPlane.vRightUpPos = m_lpWaterVertex->m_WaterVertexs[nIndex].p;

                    m_mapWaterBoundingPlanes.insert(make_pair(nWaterHeight, WaterBoundingPlane));
                }
            }
            else
            {
                itFinder->second.vLeftBottomPos.x = min(itFinder->second.vLeftBottomPos.x, m_lpWaterVertex->m_WaterVertexs[nIndex].p.x);
                itFinder->second.vLeftBottomPos.z = min(itFinder->second.vLeftBottomPos.z, m_lpWaterVertex->m_WaterVertexs[nIndex].p.z);
                itFinder->second.vRightUpPos.x = max(itFinder->second.vRightUpPos.x, m_lpWaterVertex->m_WaterVertexs[nIndex].p.x);
                itFinder->second.vRightUpPos.z = max(itFinder->second.vRightUpPos.z, m_lpWaterVertex->m_WaterVertexs[nIndex].p.z);
            }
        }
    }

    m_fWaterHeightMin = 1e10;

    itIndex = m_mapWaterBoundingPlanes.begin();
    while (itIndex != m_mapWaterBoundingPlanes.end())
    {
        // if area not large enough, then erase it
        D3DXVECTOR3 vDiff = itIndex->second.vLeftBottomPos - itIndex->second.vRightUpPos;
        if (vDiff.x * vDiff.z < 1.0f)
        {
            itIndex = m_mapWaterBoundingPlanes.erase(itIndex);
            continue;
        }

        m_fWaterHeightMin = min(m_fWaterHeightMin, itIndex->second.vLeftBottomPos.y);

        ++itIndex;
    }

    hrResult = S_OK;
//Exit0:
    return hrResult;
}

HRESULT KG3DTerrainBlock::ClearTexture(BYTE byTexIndex)
{
	int   nWidth = BLOCK_CELL_COUNT + 1;
	for (int nZ = 0;nZ <= BLOCK_CELL_COUNT;nZ++ )
	{
		for (int nX = 0;nX <= BLOCK_CELL_COUNT;nX++ )
		{
			int nIndex =  nZ * nWidth + nX;
			if(m_byteTextureIndex[nIndex] == byTexIndex)
			{
				m_byteTextureIndex[nIndex] = 0;
				if(m_nLevel == BLOCK_TYPE_REGION)
				{
					KG3DTerrainBlockGrass* pGrassBlock = GetTerrainGrassBlock();
					if( pGrassBlock)
						pGrassBlock->SetAutoGrassData(nX,nZ,FALSE,0,0,0,0,0); //先清除原有的
				}
			}
		}
	}
	return S_OK;
}