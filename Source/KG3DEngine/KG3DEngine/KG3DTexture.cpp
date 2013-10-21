#include "StdAfx.h"
#include "kg3dtexture.h"
#include "kg3dtexturetable.h"
#include "KG3DEngineManager.h"
#include "KG3DGraphiceEngine.h"
#include "KG3DGraphicsTool.h"
#include "Jpegfile.h"
#include "KG3DMesh.h"
#include "tgalib.h"
#include "KG3DClip.h"
#include "KG3DMultiThreadLoader.h"
#include "KG3DFileLoader.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

int g_nNumTextureDynamicLoaded = 0;
int g_nNumTextureDynamicLoadedFrame = 0;
int g_nNumAllowTextureDynamicLoadFrame = 5;//每桢允许操作(加载，更新)的贴图上限

extern DWORD g_dwMainThreadID;//渲染线程ID


#ifndef KG3DENGINE_DISABLE_MULTITHREAD
KThread g_MultiThreadLoaderThread;                            //加载线程
KThread g_TerrainLoadThread;
KThread g_ModelProcessThread[8];
#endif


int     g_ModelProcessThreadIndex[8];
BOOL    g_ModelProcessThreadLock[8];

BOOL volatile g_bThreadExit = FALSE;                             //加载完成退出的标志

extern int g_nLoadCount;
extern int g_nLoadTime ;



extern HRESULT g_MultiThreadSceneBlockLoadUnLoad();

void g_MultiThreadLoadProgress(void * pInput)        //多线程加载主函数
{
#ifndef KG3DENGINE_DISABLE_MULTITHREAD
	while (!g_bThreadExit)
	{
#endif
		if(!g_cMultiThreadLoader.m_bPauseMultiThreadLoad)
		{
			g_cMultiThreadLoader.ProcessMultiThreadLoad();
		}
		else
		{
			Sleep(100);
		}

#ifndef KG3DENGINE_DISABLE_MULTITHREAD
		Sleep(10);
	}
#endif
};


void g_MultiThreadLoadProgressTerrain(void * pInput)        //地形多线程加载主函数
{
#ifndef KG3DENGINE_DISABLE_MULTITHREAD
	while (!g_bThreadExit)
	{
#endif
		g_MultiThreadSceneBlockLoadUnLoad();
#ifndef KG3DENGINE_DISABLE_MULTITHREAD
		Sleep(1);
	}
#endif
};

extern HRESULT g_ModelVertexBufferFillProcess(int nIndex);

extern BOOL g_bModelProcessExit;

void g_MultiThreadModelProgress(void * pInput)        //模型填充BUFFER线程
{
#ifndef KG3DENGINE_DISABLE_MULTITHREAD
	while (!(g_bThreadExit || g_bModelProcessExit))
	{
#endif
		int nIndex = (*(int*)pInput);
		g_ModelVertexBufferFillProcess(nIndex);

#ifndef KG3DENGINE_DISABLE_MULTITHREAD
        Sleep(0);
	}
#endif
};

#ifdef KG3DENGINE_DISABLE_MULTITHREAD
void UpdateMultiThreadModelProgress(void* pInput)
{
    for (int i = 0; i< g_cEngineOption.nNumCpuThread; ++i)
    {
        g_MultiThreadModelProgress((void*)(g_ModelProcessThreadIndex + i));
    }
}
#endif


HRESULT InitModelProcessTread()
{
    g_bModelProcessExit = FALSE;
    for (int i=0;i<g_cEngineOption.nNumCpuThread;i++)
    {
        g_ModelProcessThreadIndex[i] = i;
#ifndef KG3DENGINE_DISABLE_MULTITHREAD
        g_ModelProcessThread[i].Create(g_MultiThreadModelProgress,(void*)&g_ModelProcessThreadIndex[i]);
#endif
    }
    return S_OK;
}

HRESULT UnInitModelProcessTread()
{
    g_bModelProcessExit = TRUE;
    for (int i=0;i<g_cEngineOption.nNumCpuThread;i++)
    {
#ifndef KG3DENGINE_DISABLE_MULTITHREAD
        g_ModelProcessThread[i].Destroy();
#endif
    }
    return S_OK;
}

HRESULT InitMultiThreadTextureRead()
{
#ifndef KG3DENGINE_DISABLE_MULTITHREAD
	/*int nRet =*/ g_MultiThreadLoaderThread.Create(g_MultiThreadLoadProgress,NULL);
	//SetThreadPriority(g_TextureReadThread.m_ThreadHandle,3);

	g_TerrainLoadThread.Create(g_MultiThreadLoadProgressTerrain,NULL);
#endif

	if(g_cEngineOption.bCpuSkin)
	{
		for (int i=0;i<g_cEngineOption.nNumCpuThread;i++)
		{
			g_ModelProcessThreadIndex[i] = i;
#ifndef KG3DENGINE_DISABLE_MULTITHREAD
			g_ModelProcessThread[i].Create(g_MultiThreadModelProgress,(void*)&g_ModelProcessThreadIndex[i]);
#endif
		}
	}

	return S_OK;
}

HRESULT UnInitMultiThreadTextureRead()
{
	g_bThreadExit = TRUE;

#ifndef KG3DENGINE_DISABLE_MULTITHREAD
	g_MultiThreadLoaderThread.Destroy();
	g_TerrainLoadThread.Destroy();
	
	if(g_cEngineOption.bCpuSkin)
	{
		for (int i=0;i<g_cEngineOption.nNumCpuThread;i++)
		{
			g_ModelProcessThread[i].Destroy();
		}
	}
#endif

	return S_OK;
}

KG3DTexture::KG3DTexture(void)
:m_pDepthStencilSurface(NULL)
{
	m_dwID = 0;
	m_dwType = 0;
	m_bCanReload = FALSE;
    m_dwTimes = 0;
	m_ulRefCount = 0;
	m_FileTime.dwHighDateTime = 0;
	m_FileTime.dwLowDateTime = 0;

	m_eDynamicLoadState = STATE_DELAYLOAD;
	m_uOption = 0;
	m_pTexture = NULL;
	m_pTextureSmall = NULL;
	m_pTextureSys = NULL;
	m_pTextureSysSmall = NULL;

	m_dwRenderTarger = 0;
	m_nDim = 0;

	m_bAutoGenMipmap = FALSE;
	m_eImageType = D3DXIFF_FORCE_DWORD;

    m_hFileLoader = NULL;

	m_pVideoMemShare = NULL;
}

KG3DTexture::~KG3DTexture(void)
{
	UnInit();
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DTexture::GetName(LPCTSTR *ppszName)
{
    HRESULT hResult = E_FAIL;
    KGLOG_PROCESS_ERROR(ppszName);
    *ppszName = m_scName.c_str();
    
    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DTexture::GetID(DWORD *pdwID)
{
    HRESULT hResult = E_FAIL;
    KGLOG_PROCESS_ERROR(pdwID);
    *pdwID = m_dwID;

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DTexture::CheckLoadState()
{
    HRESULT hr = S_OK;

	DWORD dwThreadID = ::GetCurrentThreadId();
	KG_PROCESS_ERROR(dwThreadID == g_dwMainThreadID);
	ASSERT(dwThreadID == g_dwMainThreadID);

	switch(m_eDynamicLoadState)
	{
	case STATE_NONE:
		{
			if(g_nNumTextureDynamicLoadedFrame<=g_nNumAllowTextureDynamicLoadFrame)
			{
    			ForceReload();
				g_nNumTextureDynamicLoaded++;
			}
			break;
		}
	case STATE_SYSTEMMEM:
		{
			if(g_nNumTextureDynamicLoadedFrame<=g_nNumAllowTextureDynamicLoadFrame)
			{
				UpdateTexture();
				g_nNumTextureDynamicLoadedFrame++;
			}
			break;
		}
    case STATE_DELAYLOAD:
        if(g_nNumTextureDynamicLoadedFrame<=g_nNumAllowTextureDynamicLoadFrame)
        {
            _ProcessDelayLoad();
            g_nNumTextureDynamicLoadedFrame++;
        }
        break;
	}
	m_dwTimes = g_dwRenderCount;

	return hr;
Exit0:
	return E_FAIL;
}

HRESULT KG3DTexture::ForceReload()///强制重载，目的可能是为了取得Alpha通道，可能是为了重新取得信息
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

	SAFE_RELEASE(m_pTexture);
	SAFE_RELEASE(m_pTextureSmall);
   
    if (m_hFileLoader)
    {
        g_FileLoader.DeleteTask(m_hFileLoader);
        m_hFileLoader = NULL;
    }
	hr = LoadFromFile(m_scName.c_str(), 0, m_uOption);
	KGLOG_COM_PROCESS_ERROR(hr);

	KG_PROCESS_ERROR(m_pTexture);

	///通知注册的Receiver贴图重绘，因为Sender如果没有被注册Receiver的话，是不做任何东西也几乎没有成员，所以是不会影响性能的，请不要删除，所有需要知道贴图重绘事件的类都需要这句///<CTH>
	m_KG3DSRMessageSender.PostMessage( KG3DMessage( KM_TEXTURE_RELOADED, reinterpret_cast<DWORD_PTR>(this),  reinterpret_cast<DWORD_PTR>(&m_scName) ) ); 
	
	hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        KGLogPrintf(KGLOG_INFO, "KG3DTexture::ForceReload(%s)\n", m_scName.c_str());
    }
	return hrResult;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DTexture::GetIEKG3DTexture(IEKG3DTexture **ppIEKG3DTexture)
{
    HRESULT hResult = E_FAIL;
    KGLOG_PROCESS_ERROR(ppIEKG3DTexture);

    *ppIEKG3DTexture = this;
    hResult = S_OK;
Exit0:
    return hResult;
}
//////////////////////////////////////////////////////////////////////////
//引用计数
ULONG STDMETHODCALLTYPE KG3DTexture::AddRef()
{
    return (ULONG)KG_InterlockedIncrement((long *)&m_ulRefCount);
}

ULONG STDMETHODCALLTYPE KG3DTexture::Release()
{
    ULONG uNewRefCount = (ULONG)KG_InterlockedDecrement((long *)&m_ulRefCount);
    if (uNewRefCount == 0)
        _OnReferenceZero(); 
    return uNewRefCount;
}

ULONG STDMETHODCALLTYPE KG3DTexture::ReleaseWithoutDelay()
{
    ULONG uNewRefCount = (ULONG)KG_InterlockedDecrement((long *)&m_ulRefCount);
    if (uNewRefCount == 0)
        _OnReferenceZero(RELEASE_ATONCE); 
    return uNewRefCount;
}

HRESULT STDMETHODCALLTYPE KG3DTexture::QueryInterface( 
	/* [in] */ REFIID riid,
	/* [iid_is][out] */ void **ppvObject)
{
	return E_FAIL;
}

HRESULT KG3DTexture::_OnReferenceZero( DWORD dwReleaseOpion /*= RELEASE_INQUEUE*/ )
{
	ULONG uRef = m_ulRefCount;
	g_cTextureTable.ReleaseResource(this, dwReleaseOpion);
	return uRef;
}
//////////////////////////////////////////////////////////////////////////
//初始化,清除数据
HRESULT KG3DTexture::Init()
{
	m_dwID = 0;
	m_ulRefCount = 0;
	TCHAR Name[256];
    wsprintf(Name,"!@#$Texture_%d",m_dwID);
	m_scName = Name;
	m_pTexture = NULL;
	m_pTextureSmall = NULL;
	m_pTextureSys = NULL;
	m_pTextureSysSmall = NULL;
	m_pDiscard = NULL;
	m_bCanReload = FALSE;
	m_bAutoGenMipmap = FALSE;
	m_eDynamicLoadState = STATE_DELAYLOAD;
	m_eImageType = D3DXIFF_FORCE_DWORD;
	return S_OK;
}

HRESULT KG3DTexture::UnInit()
{
    if (m_hFileLoader)
    {
        g_FileLoader.DeleteTask(m_hFileLoader);
        m_hFileLoader = NULL;
    }

    SAFE_RELEASE(m_pTexture);	//m_pTexture作为是否创建的标志
    SAFE_RELEASE(m_pTextureSmall);	//m_pTexture作为是否创建的标志
	SAFE_RELEASE(m_pTextureSys);
	SAFE_RELEASE(m_pTextureSysSmall);
	SAFE_RELEASE(m_pDiscard);
	SAFE_RELEASE(m_pDepthStencilSurface);

	if(m_pVideoMemShare)
	{
		g_cTextureTable.m_VideoMemTextureMgr.FreeResource(this,m_pVideoMemShare);
		m_pVideoMemShare = NULL;
	}
	return S_OK;
}

HRESULT KG3DTexture::_CheckOption(unsigned uOption,BOOL bInMultiThread)
{
	m_dwRenderTarger = 0;
	m_Desc.Pool = D3DPOOL_DEFAULT;
	m_bCanReload = FALSE;
	if (uOption & TEXOP_DISABLEDTX)
	{
		if(bInMultiThread)
		{
			m_Desc.Format = D3DFMT_A8R8G8B8;
			m_Desc.Pool = D3DPOOL_MANAGED; ///指定了不允许压缩，意味着可能要Lock贴图，所以要用managed，不然Pool是Default的话，取Surface太麻烦了，CTH
		}
		else
		{
			m_bCanReload = TRUE;
			m_Desc.Pool = D3DPOOL_SYSTEMMEM;
			m_Desc.Format = D3DFMT_FROM_FILE;
		}
	}
	else if (uOption & TEXOP_RENDERTARGET)
	{
		m_dwRenderTarger = D3DUSAGE_RENDERTARGET;
		m_Desc.Format = D3DFMT_A8R8G8B8;
	}
	else if ((uOption & TEXOP_RELOAD) &&
		m_eImageType != D3DXIFF_FORCE_DWORD)//是支持动态加载的类型
	{
		m_bCanReload = TRUE;
		m_Desc.Pool = D3DPOOL_SYSTEMMEM;
		m_Desc.Format = D3DFMT_FROM_FILE;
	}
	else 
	{		
		m_bCanReload = TRUE;
		m_Desc.Pool = D3DPOOL_SYSTEMMEM;
		m_Desc.Format = D3DFMT_FROM_FILE;
	}
	m_nDim = (uOption & TEXOP_DISABLEPOW2 ) ? D3DX_DEFAULT_NONPOW2 : D3DX_DEFAULT;
	m_Desc.Usage = ( uOption & TEXOP_DYNAMIC ) ? D3DUSAGE_DYNAMIC : 0;

	//检测mipmap的选项
	m_bAutoGenMipmap = FALSE;
	m_Desc.MipLevels = 0;
	
	if (uOption & TEXOP_DISABLEMIPMAP)
	{
		m_Desc.MipLevels = 1;
	}
	else if ((m_eImageType != D3DXIFF_DDS) && //dds 格式无法自动生成mipmap
		(g_cGraphicsEngine.GetD3DCaps().Caps2 & D3DCAPS2_CANAUTOGENMIPMAP))
	{
		HRESULT hResult = g_cGraphicsEngine.m_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			g_cGraphicsEngine.m_DisplayMode.Format,
			D3DUSAGE_AUTOGENMIPMAP,
			D3DRTYPE_TEXTURE,
			m_Desc.Format);
		if (hResult == S_OK)
		{
			m_bAutoGenMipmap = TRUE;
			m_Desc.Usage |= D3DUSAGE_AUTOGENMIPMAP;
			m_Desc.MipLevels = 1;
		}
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
//加载,保存
HRESULT KG3DTexture::_LoadFromMemory( LPCVOID pSrcData, UINT SrcSize, unsigned uOption ,BOOL bRetryIfFailed)
{
	HRESULT hr = E_FAIL;
	LPDIRECT3DTEXTURE9 pTexture = NULL;
	LPDIRECT3DTEXTURE9 pTextureSmall = NULL;
	BOOL bSmallTextureInit = FALSE;
	D3DXIMAGE_INFO FileInfo;
    D3DSURFACE_DESC desc;

    m_Desc.Width = 0;
	m_Desc.Height = 0;

	DWORD dwTime = timeGetTime();

	ASSERT( NULL != pSrcData );
	ASSERT( 0 != SrcSize );
	hr = D3DXGetImageInfoFromFileInMemory( pSrcData, SrcSize, &FileInfo );
	KGLOG_COM_PROCESS_ERROR(hr);

	m_dwTextureType = FileInfo.ResourceType;
	if (FileInfo.Height == 1 && FileInfo.Width == 1)
	{
		m_Desc.MipLevels = 0;
		m_Desc.Usage &= ~D3DUSAGE_AUTOGENMIPMAP;
	}
	switch( FileInfo.ResourceType )
	{
	case D3DRTYPE_TEXTURE:
		{
			if (m_dwRenderTarger & D3DUSAGE_RENDERTARGET)
			{
				hr = D3DXCreateTextureFromFileInMemoryEx(g_pd3dDevice, pSrcData, SrcSize,
					m_nDim,m_nDim,m_Desc.MipLevels,
					D3DUSAGE_RENDERTARGET,m_Desc.Format,m_Desc.Pool,D3DX_DEFAULT,D3DX_DEFAULT,
					0,NULL,NULL,
					&pTexture);
                KGLOG_COM_PROCESS_ERROR(hr);
			}
			else
			{
				D3DXIMAGE_INFO imageinfo;
				hr = D3DXGetImageInfoFromFileInMemory(pSrcData,SrcSize,&imageinfo);
				KGLOG_COM_PROCESS_ERROR(hr);

				m_Desc.Width = imageinfo.Width;
				m_Desc.Height = imageinfo.Height;

				if(uOption & TEXOP_DISABLESCALE)
				{
					hr = D3DXCreateTextureFromFileInMemoryEx(g_pd3dDevice, pSrcData, SrcSize,
						m_nDim,m_nDim,m_Desc.MipLevels,
						m_Desc.Usage,m_Desc.Format,m_Desc.Pool,D3DX_DEFAULT,D3DX_DEFAULT,
						0,NULL,NULL,
						&pTexture);
				}
				else
				{
					hr = D3DXCreateTextureFromFileInMemoryEx(g_pd3dDevice, pSrcData, SrcSize,
						imageinfo.Width / g_cEngineOption.nTextureScale,
						imageinfo.Height / g_cEngineOption.nTextureScale ,m_Desc.MipLevels,
						m_Desc.Usage,m_Desc.Format,m_Desc.Pool,D3DX_DEFAULT,D3DX_DEFAULT,
						0,NULL,NULL,
						&pTexture);
					
					HRESULT hrSmall = D3DXCreateTextureFromFileInMemoryEx(g_pd3dDevice, pSrcData, SrcSize,
						imageinfo.Width  / g_cEngineOption.nTextureScale / 8,
						imageinfo.Height / g_cEngineOption.nTextureScale / 8,
						m_Desc.MipLevels,
						m_Desc.Usage,m_Desc.Format,m_Desc.Pool,D3DX_DEFAULT,D3DX_DEFAULT,
						0,NULL,NULL,
						&pTextureSmall);
					if(SUCCEEDED(hrSmall))
					{
						bSmallTextureInit = TRUE;
					}
				}
				if(FAILED(hr))
				{
					KGLogPrintf(KGLOG_ERR,"D3DXCreateTextureFromFileInMemory Failed %s",m_scName.c_str());
					if(bRetryIfFailed)
						m_eDynamicLoadState = STATE_NONE;
					else
						m_eDynamicLoadState = STATE_LOADFAILED;
				}
				KGLOG_COM_PROCESS_ERROR(hr);
			}
			break;
		}
	case D3DRTYPE_CUBETEXTURE:
		{
			hr = D3DXCreateCubeTextureFromFileInMemoryEx(g_pd3dDevice, pSrcData, SrcSize,
				D3DX_DEFAULT,D3DX_DEFAULT,0,D3DFMT_DXT2,
				D3DPOOL_MANAGED,D3DX_DEFAULT,D3DX_DEFAULT,
				0,NULL,NULL,
				(LPDIRECT3DCUBETEXTURE9*)&pTexture);
			KGLOG_COM_PROCESS_ERROR(hr);
			break;
		}
	case D3DRTYPE_VOLUMETEXTURE:
		{
			hr = D3DXCreateVolumeTextureFromFileInMemory(g_pd3dDevice, pSrcData, SrcSize, (LPDIRECT3DVOLUMETEXTURE9*)&pTexture);
			KGLOG_COM_PROCESS_ERROR(hr);
			break;
		}
	default:
		assert(0);
	}

	if (m_Desc.Usage & D3DUSAGE_AUTOGENMIPMAP)
	{
		hr = pTexture->SetAutoGenFilterType(D3DTEXF_LINEAR);
        KGLOG_COM_PROCESS_ERROR(hr);
		pTexture->GenerateMipSubLevels(); // return void
	}

    hr = pTexture->GetLevelDesc(0,&desc);
    KGLOG_COM_PROCESS_ERROR(hr);
    m_Desc.Width = desc.Width;
    m_Desc.Height = desc.Height;
    m_Desc.Format = desc.Format; // maybe old m_Desc.Format = D3DFMT_FROM_FILE, set to real format

	hr = S_OK;

Exit0:
	if (SUCCEEDED(hr))
	{
		if(m_bCanReload)
		{
			SAFE_RELEASE(m_pTextureSys);
			SAFE_RELEASE(m_pTextureSysSmall);

			m_pTextureSys      = pTexture;
			m_pTextureSysSmall = pTextureSmall;
			if(m_pTextureSysSmall)
				m_pTextureSysSmall->AddRef();

			m_eDynamicLoadState = STATE_SYSTEMMEM;
		}
		else
		{
			SAFE_RELEASE(m_pTexture);
			SAFE_RELEASE(m_pTextureSmall);
			m_pTexture = pTexture;
		
			m_eDynamicLoadState = STATE_VIDEOMEM;
		}
	}
	else
	{
		SAFE_RELEASE(pTexture)
	}

	SAFE_RELEASE(pTextureSmall);

	int loadcost = (int)(timeGetTime() - dwTime);
	g_nLoadTime += loadcost;

    g_nLoadCount++;
	return hr;
}
////////////////////////////////////////////////////////////////////////////////
int KG3DTexture::_ProcessDelayLoad()
{
    int nResult  = false;
    int nRetCode = false;
    HRESULT hrRetCode = E_FAIL;
    FILE_LOAD_STATE eFileLoadState = FILE_LOAD_STATE_FAILED;
    IKG_Buffer *piFileBuf = NULL;
    void *pvData = NULL;
    unsigned uSize = 0;

    ASSERT(m_hFileLoader);
    ASSERT(m_eDynamicLoadState == STATE_DELAYLOAD);

    nRetCode = g_FileLoader.FetchResultNormal(m_hFileLoader, &eFileLoadState, &piFileBuf);
    KGLOG_PROCESS_ERROR(nRetCode);
    KG_PROCESS_SUCCESS(eFileLoadState == FILE_LOAD_STATE_WAIT);
    KGLOG_PROCESS_ERROR(eFileLoadState != FILE_LOAD_STATE_FAILED);

    pvData = piFileBuf->GetData();
    uSize  = piFileBuf->GetSize();
    hrRetCode = _LoadFromMemory(pvData, uSize, m_uOption, FALSE);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

Exit1:
    nResult = true;
Exit0:
    KG_COM_RELEASE(piFileBuf);
    if (!nResult)
    {
        m_eDynamicLoadState = STATE_LOADFAILED;
    }
    if (m_eDynamicLoadState != STATE_DELAYLOAD)
    {
        // state changed during process
        if (m_hFileLoader)
        {
            g_FileLoader.DeleteTask(m_hFileLoader);
            m_hFileLoader = NULL;
        }  
    }
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
bool bCheck = false;
HRESULT KG3DTexture::UpdateTexture()
{
	HRESULT hr = S_OK;
	DWORD dwTime = timeGetTime();

	if(!m_pTextureSys)
	{
		KGLogPrintf(KGLOG_ERR, "KG3DTexture::UpdateTexture %s failed.", m_scName.c_str());
		int k = 0;
	}
	KGLOG_PROCESS_ERROR(m_pTextureSys);


	float nGate1 = 20;
	float nGate0 = 0.5F * nGate1;

	if(m_bCanReload && m_pTextureSysSmall)
	{

		if(!m_pTextureSmall)
		{
			switch (m_dwTextureType)
			{
			case D3DRTYPE_TEXTURE:
				{
					hr = D3DXCreateTexture(g_pd3dDevice,
						m_Desc.Width / 8,
						m_Desc.Height/ 8, 
						m_Desc.MipLevels, 
						m_Desc.Usage,
						m_Desc.Format,
						D3DPOOL_DEFAULT,
						&m_pTextureSmall);
					KG_COM_PROCESS_ERROR(hr);
				}
				break;
			case D3DRTYPE_CUBETEXTURE:
			case D3DRTYPE_VOLUMETEXTURE:
				ASSERT(0);
				break;
			}

			KGLOG_PROCESS_ERROR(m_pTextureSmall);

			m_pTextureSysSmall->AddDirtyRect(NULL);

			hr = g_pd3dDevice->UpdateTexture(m_pTextureSysSmall,m_pTextureSmall);
			KGLOG_COM_PROCESS_ERROR(hr);
		}
	}
		
	//////////////////////////////////////////////////////////////////////////
	if(m_bCanReload)
	{
		if((m_pVideoMemShare && m_pVideoMemShare->pUser!= this) ||
			!m_pVideoMemShare)
		{
			m_pVideoMemShare = g_cTextureTable.m_VideoMemTextureMgr.RequestResource(this);
			KG_PROCESS_ERROR(m_pVideoMemShare);

			if(!m_pVideoMemShare->pTexture)
			{
				switch (m_dwTextureType)
				{
				case D3DRTYPE_TEXTURE:
					{
						hr = D3DXCreateTexture(g_pd3dDevice,
							m_Desc.Width,
							m_Desc.Height, 
							m_Desc.MipLevels, 
							m_Desc.Usage,
							m_Desc.Format,
							D3DPOOL_DEFAULT,
							&m_pVideoMemShare->pTexture);
						KG_COM_PROCESS_ERROR(hr);
					}
					break;
				case D3DRTYPE_CUBETEXTURE:
				case D3DRTYPE_VOLUMETEXTURE:
					ASSERT(0);
					break;
				}
			}
		}

		KGLOG_PROCESS_ERROR(m_pVideoMemShare->pTexture);

		KGLOG_PROCESS_ERROR(m_pTextureSys);
		m_pTextureSys->AddDirtyRect(NULL);

		hr = g_pd3dDevice->UpdateTexture(m_pTextureSys,m_pVideoMemShare->pTexture);
		KGLOG_COM_PROCESS_ERROR(hr);
	}
	/*else
	{
		if(!m_pTexture)
		{
			switch (m_dwTextureType)
			{
			case D3DRTYPE_TEXTURE:
				{
					hr = D3DXCreateTexture(g_pd3dDevice,
						m_Desc.Width,
						m_Desc.Height, 
						m_Desc.MipLevels, 
						m_Desc.Usage,
						m_Desc.Format,
						D3DPOOL_DEFAULT,
						&m_pTexture);
					KG_COM_PROCESS_ERROR(hr);
				}
				break;
			case D3DRTYPE_CUBETEXTURE:
			case D3DRTYPE_VOLUMETEXTURE:
				ASSERT(0);
				break;
			}

			KGLOG_PROCESS_ERROR(m_pTexture);

			KGLOG_PROCESS_ERROR(m_pTextureSys);
			m_pTextureSys->AddDirtyRect(NULL);

			hr = g_pd3dDevice->UpdateTexture(m_pTextureSys,m_pTexture);
			KGLOG_COM_PROCESS_ERROR(hr);
		}
	}*/

	m_eDynamicLoadState = STATE_VIDEOMEM;

	int loadcost = (int)(timeGetTime() - dwTime);
	g_nLoadTime += loadcost;

	g_nLoadCount++;

	if(g_cEngineOption.bEnableTimeOptimizeLog && loadcost > 5)
	{
		KGLogPrintf(KGLOG_INFO,"TimeOptimize %d KG3DTexture::UpdateTexture ",loadcost);
	}
	return S_OK;
Exit0:
	//KGLogPrintf(KGLOG_ERR, "KG3DTexture::UpdateTexture %s failed.", m_scName.c_str());
	SAFE_RELEASE(m_pTextureSmall);
	return E_FAIL;
}


struct TextureSuffixToType
{
	TCHAR *strSuffix;
	D3DXIMAGE_FILEFORMAT Type;
};

static TextureSuffixToType s_strSuffixToType[] = {
	{".tga", D3DXIFF_TGA},
	{".jpg", D3DXIFF_JPG},
	{".dds", D3DXIFF_DDS},
};

HRESULT KG3DTexture::LoadFromFile(const char cszFileNameSrc[], 
								  unsigned uFileNameHash, 
								  unsigned uOption)
{
	HRESULT hrResult = E_FAIL;
	HRESULT hrRetCode = E_FAIL;
    int     nRetCode = false;
    DWORD dwSize = 0;
    IFile *pFile = NULL;
    BYTE *pBuffer = NULL;
    const char *pcszExtName = NULL;
    char szDDSFileName[MAX_PATH];
    int nReplaceFileExistFlag = false;

    pcszExtName = strrchr(cszFileNameSrc, '.');
    KGLOG_PROCESS_ERROR(pcszExtName);

	DWORD dwThreadID = ::GetCurrentThreadId();
	if(dwThreadID != g_dwMainThreadID)
	{
		uOption |= TEXOP_RELOAD;
	}
    m_uOption = uOption;
    m_scName = cszFileNameSrc;

    if (stricmp(pcszExtName, ".dds") != 0)
    {
        unsigned uFileNameLen = (unsigned)(pcszExtName - cszFileNameSrc);

        ASSERT((uFileNameLen + sizeof(".dds") < MAX_PATH));
        memcpy(szDDSFileName, cszFileNameSrc, uFileNameLen);
        memcpy(szDDSFileName + uFileNameLen, ".dds", sizeof(".dds"));

        if (g_IsFileExist(szDDSFileName))
        {
            nReplaceFileExistFlag = true;
            pcszExtName  = szDDSFileName + uFileNameLen;
        }
    }
    if (nReplaceFileExistFlag)
    {
        m_scNameReal = szDDSFileName;
    }
    else
    {
        nRetCode = g_IsFileExist(cszFileNameSrc);	
        KG_PROCESS_ERROR(nRetCode && "File not exist");
        m_scNameReal = cszFileNameSrc;
    }

    //////////////////////////////////////////////////////////////////////////
    for (size_t i = 0; i < _countof(s_strSuffixToType); ++i)
    {
        if (stricmp(pcszExtName, s_strSuffixToType[i].strSuffix) == 0)
        {
            m_eImageType = s_strSuffixToType[i].Type;
            break;
        }
    }
	//////////////////////////////////////////////////////////////////////////

	_CheckOption(uOption,dwThreadID == g_dwMainThreadID); //todo: move to _LoadFromMemory();

	if(g_eEngineOption.bUseMultiThreadLoad && (m_uOption & TEXOP_RELOAD) &&
		m_bCanReload &&
		(m_eDynamicLoadState == STATE_DELAYLOAD))
	{
        m_hFileLoader = g_FileLoader.CreateTask(m_scNameReal.c_str(), FILE_TYPE_NORMAL);
        KGLOG_PROCESS_ERROR(m_hFileLoader);

        g_nLoadCount++;
		KG_PROCESS_SUCCESS(TRUE);
	}
	else
	{
		pFile = g_OpenFile(m_scNameReal.c_str());
		KG_PROCESS_ERROR(pFile);

		DWORD  m_dwFileLen= pFile->Size();
		ASSERT(m_dwFileLen);
		KG_PROCESS_ERROR(m_dwFileLen);

		pBuffer = (PBYTE)malloc(m_dwFileLen);
		KG_PROCESS_ERROR(pBuffer);

		dwSize = pFile->Read(pBuffer, m_dwFileLen);
		KG_PROCESS_ERROR(m_dwFileLen == dwSize);

		hrRetCode = _LoadFromMemory( pBuffer, m_dwFileLen, uOption, FALSE);
		KG_COM_PROCESS_ERROR(hrRetCode);
		if(m_bCanReload && m_eDynamicLoadState == STATE_SYSTEMMEM)
			UpdateTexture();

		g_nNumTextureDynamicLoadedFrame++;

	}

    if(!g_bClient && g_eEngineOption.bEnableAutoReLoadTexture)
    {
        _GetFileTime(m_scNameReal.c_str(), m_FileTime);
    }

Exit1:
	hrResult = S_OK;
Exit0:

#ifdef _DEBUG
	if (FAILED(hrResult) && cszFileNameSrc)
	{
		KGLogPrintf(KGLOG_ERR, "Load File %s failed.\n", cszFileNameSrc);
	}	
#endif
	KG_COM_RELEASE(pFile);
    free(pBuffer);
	return hrResult;
}

BOOL KG3DTexture::IsFileChanged()
{
	//CTH 2008-01-22 基于和LoadFromFile相同的原因，限制在非客户端比较安全
	if (g_bClient)
	{
		return FALSE;		
	}

	if(!IsResourceReady())
	{
		return FALSE;
	}

	if(!g_eEngineOption.bEnableAutoReLoadTexture)
		return FALSE;

	FILETIME filetime;
	ZeroMemory(&filetime,sizeof(FILETIME));

	if(! _GetFileTime(m_scNameReal.c_str(), filetime))
		return FALSE;

	if((m_FileTime.dwHighDateTime!=filetime.dwHighDateTime)||
		(m_FileTime.dwLowDateTime!=filetime.dwLowDateTime))
	{
		SAFE_RELEASE(m_pTextureSys);
		SAFE_RELEASE(m_pTextureSysSmall);
		return TRUE;
	}
	else
		return FALSE;
}

HRESULT KG3DTexture::SaveToFile(const char cszFileName[],   unsigned uOption)
{
	HRESULT hr = E_FAIL;
	D3DXIMAGE_FILEFORMAT iff;
	KG_PROCESS_ERROR(cszFileName);
	KG_PROCESS_ERROR(cszFileName[0] != '\0');

	iff = _GetFileTypeFromName( cszFileName );
	hr = D3DXSaveTextureToFile(cszFileName, iff, m_pTexture, NULL);
Exit0:
	return hr;
}

HRESULT KG3DTexture::SaveSysToFile(const char cszFileName[],   unsigned uOption)
{
    HRESULT hr = E_FAIL;
    D3DXIMAGE_FILEFORMAT iff;
    KG_PROCESS_ERROR(cszFileName);
    KG_PROCESS_ERROR(cszFileName[0] != '\0');

    iff = _GetFileTypeFromName(cszFileName);
    hr = D3DXSaveTextureToFile(cszFileName, iff, m_pTextureSys, NULL);
Exit0:
    return hr;
}


D3DXIMAGE_FILEFORMAT KG3DTexture::_GetFileTypeFromName( const char* strFileName )
{
	D3DXIMAGE_FILEFORMAT iff = D3DXIFF_TGA;
	TCHAR strFileName2[MAX_PATH];
	size_t nDotPos = strlen( strFileName ) - 1;
	while ( nDotPos >= 0 && strFileName[nDotPos] != '.' )
	{
		nDotPos--;
	}
	strcpy( strFileName2, strFileName + nDotPos + 1 );
	_strlwr( strFileName2 );

	if ( !strcmp( strFileName2, "jpg" ) )
		iff = D3DXIFF_JPG;
	else if ( !strcmp( strFileName2, "tga" ) )
		iff = D3DXIFF_TGA;
	else if ( !strcmp( strFileName2, "dds" ) )
		iff = D3DXIFF_DDS;
	else if ( !strcmp( strFileName2, "bmp" ) )
		iff = D3DXIFF_BMP;
	return iff;
}
//////////////////////////////////////////////////////////////////////////
//type
DWORD KG3DTexture::GetID()
{
    return m_dwID;
}
unsigned KG3DTexture::GetType()
{
	return m_dwType;
}

//////////////////////////////////////////////////////////////////////////
//内存管理
HRESULT KG3DTexture::PrepareRender(unsigned uOption)//压缩数据
{
	return E_NOTIMPL;
}

HRESULT KG3DTexture::PostRender()//恢复数据，对象可以立即被使用
{
	return E_NOTIMPL;
}
HRESULT KG3DTexture::GetDateInformation(int* pnMem,int* pnVedioMem,int* pnConMem,int* pnConVMem)//得到数据信息，参数依次为占用内存大小，现存大小，压缩后内存大小，压缩后内存大小
{
	return E_NOTIMPL;
}
unsigned   KG3DTexture::GetLastUseTime()//查询上次使用的时间
{
	return m_dwTimes;
}
unsigned   KG3DTexture::GetUseFrequency()//查询使用频率，一段时间里的使用次数
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////
HRESULT KG3DTexture::Update(unsigned uMethod, DWORD dwTime)//更新贴图,对动画贴图起作用
{
	return S_OK;

}
HRESULT KG3DTexture::Update(int nFrame)//更新贴图
{
	return S_OK;

}

HRESULT KG3DTexture::GetSize(unsigned *puWidth, unsigned *puHeight)
{
    std::pair<UINT, UINT> vWidthHeight;

	KGLOG_PROCESS_ERROR (NULL != puHeight && NULL != puWidth );
	
	vWidthHeight = GetSize();
	*puWidth = vWidthHeight.first;
	*puHeight = vWidthHeight.second;
	
	return S_OK;
Exit0:
	return E_FAIL;
}

std::pair<UINT, UINT> KG3DTexture::GetSize()
{
	if(NULL == m_pTexture)
		return std::pair<UINT, UINT>(0,0);

	D3DSURFACE_DESC	d3d_desc;
	HRESULT hr = m_pTexture->GetLevelDesc( 0, &d3d_desc );//因为Tex在D3D中会自动放缩成Power2，所以不能用Create的长宽作为贴图真正的长宽。
	if ( SUCCEEDED(hr))
	{
		return std::pair<UINT, UINT>(d3d_desc.Width, d3d_desc.Height);
	}
	
	return std::pair<UINT, UINT>(0,0);
}

LPDIRECT3DTEXTURE9 KG3DTexture::GetTexture()
{
	HRESULT hr = E_FAIL;
	m_dwTimes = g_dwRenderCount;

	if(m_bCanReload&&(m_eDynamicLoadState!=STATE_VIDEOMEM))
	{
		CheckLoadState();
	}

	if(m_bCanReload && m_eDynamicLoadState == STATE_VIDEOMEM)
	{
		if(m_pVideoMemShare && m_pVideoMemShare->pUser == this)
		{
			m_pVideoMemShare->OnResourceUse();
			return m_pVideoMemShare->pTexture;
		}
		else if(g_nNumTextureDynamicLoadedFrame <= g_nNumAllowTextureDynamicLoadFrame)
		{
			g_nNumTextureDynamicLoadedFrame++;

			hr = UpdateTexture();
			if(SUCCEEDED(hr) && m_pVideoMemShare)
				return m_pVideoMemShare->pTexture;
			else
				return m_pTextureSmall;
		}
		else
		{
			return m_pTextureSmall;
		}
	}
	else
	{
		PreLoad();

		return m_pTexture;
	}
}

HRESULT KG3DTexture::SetTexture(LPDIRECT3DDEVICE9 pDevice, int i) //Device必须显式传进来，不然在绘制的函数中容易会忽略这句对Device的影响
{
	LPDIRECT3DTEXTURE9 pTexture = GetTexture();
	pDevice->SetTexture(i,pTexture);
	return S_OK;
}

HRESULT KG3DTexture::_CreateTexturePrivate( BOOL bUseDevice, UINT Width,UINT Height,UINT MipLevels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool, UINT uBackBufferSizeDividing )
{
	HRESULT hr = E_FAIL;
	SAFE_RELEASE(m_pTexture);
	SAFE_RELEASE(m_pDepthStencilSurface);

	UINT uWidthUse = Width;
	UINT uHeightUse = Height;
	UINT MipLevelsUse = MipLevels;
	D3DSURFACE_DESC	Desc;

	if (uBackBufferSizeDividing > 0 && uBackBufferSizeDividing <= MAX_DIVISOR_OF_BACKBUFFER)
	{
		UINT uBackbufferWidth = g_cGraphicsEngine.GetPresentParameters().BackBufferWidth;
		UINT uBackbufferHeight = g_cGraphicsEngine.GetPresentParameters().BackBufferHeight;
		uWidthUse = uBackbufferWidth/uBackBufferSizeDividing;
		uHeightUse = uBackbufferHeight /uBackBufferSizeDividing;
	}

	KG_PROCESS_ERROR(uWidthUse <= 4096 && uHeightUse <= 4096);

	{
		if (Usage == D3DUSAGE_RENDERTARGET)
		{
			MipLevelsUse = 1;
			hr = g_pd3dDevice->CreateTexture(uWidthUse, uHeightUse, MipLevelsUse, Usage
				, Format, D3DPOOL_DEFAULT, &m_pTexture, NULL);
			KGLOG_COM_PROCESS_ERROR(hr);
			
			hr = m_pTexture->GetLevelDesc(0, &Desc);
			_ASSERTE(SUCCEEDED(hr));
		}
		else if (Usage == D3DUSAGE_DEPTHSTENCIL)
		{
			MipLevelsUse = 1;
			hr = g_pd3dDevice->CreateDepthStencilSurface(uWidthUse, uHeightUse, Format, D3DMULTISAMPLE_NONE
				, 0, TRUE, &m_pDepthStencilSurface, NULL);
			KGLOG_COM_PROCESS_ERROR(hr);

			hr = m_pDepthStencilSurface->GetDesc(&Desc);
			_ASSERTE(SUCCEEDED(hr));
		}
		else
		{
			hr = bUseDevice 
				? g_pd3dDevice->CreateTexture(uWidthUse, uHeightUse, MipLevelsUse, Usage, Format, Pool, &m_pTexture, NULL) 
				: D3DXCreateTexture(g_pd3dDevice,uWidthUse,uHeightUse,MipLevelsUse,Usage,Format,Pool,&m_pTexture);
			KGLOG_COM_PROCESS_ERROR(hr);

			hr = m_pTexture->GetLevelDesc(0, &Desc);
			_ASSERTE(SUCCEEDED(hr));
		}
	}//</>

	KG_COM_PROCESS_ERROR(hr);
	{
		m_Desc.Format = Desc.Format;
		m_Desc.Pool = Desc.Pool;
		m_Desc.Width = Desc.Width;
		m_Desc.Height = Desc.Height;
		m_Desc.Usage = Desc.Usage;
		m_Desc.MipLevels = MipLevelsUse;

		m_Desc.SizeIsFractionOfBackBufferSize = uBackBufferSizeDividing;
	}

	m_bCanReload = FALSE;
	return hr;
Exit0:
	SAFE_RELEASE(m_pTexture);
	SAFE_RELEASE(m_pDepthStencilSurface);
	return hr;
}

HRESULT KG3DTexture::CreateTexture(unsigned int uWidth,
								   unsigned int uHeight,
								   D3DFORMAT Format,
								   TEXTYPE	texType /*= TEXTYPE_COMMON*/,	//0是默认，其它参考枚举
								   UINT uBackBufferSizeDividing /*= 0*/)
{
	DWORD Usage = 0;
	if(texType == TEXTYPE_RENDER_TARGET)
		Usage = D3DUSAGE_RENDERTARGET;
	if(texType == TEXTYPE_DEPTH)
		Usage = D3DUSAGE_DEPTHSTENCIL;

	D3DPOOL Pool = D3DPOOL_MANAGED;
	return _CreateTexturePrivate(FALSE, uWidth, uHeight, 0, Usage, Format, Pool, uBackBufferSizeDividing);
}

D3DFORMAT KG3DTexture::GetFormat()
{
	D3DSURFACE_DESC	d3d_desc;

	KGLOG_PROCESS_ERROR( m_pTexture );
	
	if ( FAILED( m_pTexture->GetLevelDesc( 0, &d3d_desc ) ) )
	{
		goto Exit0;
	}
	return d3d_desc.Format;

Exit0:
	return D3DFMT_UNKNOWN;
}

bool KG3DTexture::EvictResources( )
{
	if (m_bCanReload && m_eDynamicLoadState == STATE_VIDEOMEM)
	{
		SAFE_RELEASE(m_pTexture);
		SAFE_RELEASE(m_pTextureSmall);

		m_eDynamicLoadState = STATE_SYSTEMMEM;

		return true;
	}

	m_dwTimes = g_dwRenderCount;

	return false;
}


BOOL KG3DTexture::IsResourceReady()
{
	if(m_bCanReload)
	{
		CheckLoadState();

		if(m_eDynamicLoadState==STATE_VIDEOMEM || m_eDynamicLoadState==STATE_LOADFAILED )
			return TRUE;
		else
		{
			return FALSE;
		}
	}

	return TRUE;
}

HRESULT KG3DTexture::OnLostDevice()
{
	if (m_bCanReload)
    {
		if(m_pVideoMemShare)
		{
			g_cTextureTable.m_VideoMemTextureMgr.FreeResource(this,m_pVideoMemShare);
		}
		m_pVideoMemShare = NULL;
		EvictResources();
    }
    else
    {
        if (m_Desc.Pool == D3DPOOL_DEFAULT)
        {
            VERIFY_RELEASE(m_pTexture);
        }
    }

	SAFE_RELEASE(m_pDepthStencilSurface);

	return S_OK;
}

HRESULT KG3DTexture::OnResetDevice()
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

	if (m_Desc.Usage == D3DUSAGE_RENDERTARGET || m_Desc.Usage == D3DUSAGE_DEPTHSTENCIL)
	{
		hr = _CreateTexturePrivate(FALSE, m_Desc.Width, m_Desc.Height, m_Desc.MipLevels, m_Desc.Usage, m_Desc.Format, m_Desc.Pool, m_Desc.SizeIsFractionOfBackBufferSize);
        KGLOG_COM_PROCESS_ERROR(hr);
	}
    else if (!m_bCanReload && m_Desc.Pool == D3DPOOL_DEFAULT)
    {
        hr = ForceReload();
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    hrResult = S_OK;
Exit0:
	return hrResult;
}
HRESULT KG3DTexture::GetSurfaceDesc( UINT dwLevel, D3DSURFACE_DESC* pDesc )
{
	HRESULT hr = E_FAIL;
	LPDIRECT3DSURFACE9 lpSurface = NULL;
	KG_PROCESS_ERROR(m_pTexture && pDesc);
	hr = m_pTexture->GetSurfaceLevel(dwLevel, &lpSurface);
	KG_COM_PROCESS_ERROR(hr);
	hr = lpSurface->GetDesc(pDesc);
Exit0:
	SAFE_RELEASE(lpSurface);
	return hr;
}

VOID KG3DTexture::PreLoad()
{
	if (m_pTexture && m_Desc.Pool == D3DPOOL_MANAGED)
	{
		m_pTexture->PreLoad();
	}
}

bool KG3DTexture::_GetFileTime(LPCSTR cszFileName, FILETIME& FileTimeRet )
{
	//CTH //2008-01-22 编辑器使用的自动贴图重新加载需要检查文件时间，而打包的函数中没有检查文件时间的函数
	//所以这里原来用的CreateFile没有变。但这样用相对路径的时候，CreateFile会失败，造成某些贴图加载失败
	//这里把贴图自动重载限制在编辑器中，手工拼一个全路径。因为只有编辑器用的比较多，而且这个函数很凶险
	//很容易造成多次重新加载，还是限制在非客户端比较好。
	if(! g_bClient)
	{
		TCHAR tcsFullFilePath[MAX_PATH];
		g_GetFullPath(tcsFullFilePath, cszFileName);
		HANDLE hFile = NULL;
		hFile = CreateFile(tcsFullFilePath,    // file to open
			GENERIC_READ,          // open for reading
			FILE_SHARE_READ,       // share for reading
			NULL,                  // default security
			OPEN_EXISTING,         // existing file only
			FILE_ATTRIBUTE_NORMAL, // normal file
			NULL);                 // no attr. template

		if (hFile != INVALID_HANDLE_VALUE) 
		{
			FILETIME FileTimeTemp;
			if(::GetFileTime(hFile,NULL,NULL,&FileTimeTemp))
			{
				FileTimeRet = FileTimeTemp;
				CloseHandle(hFile);
				return true;
			}
			CloseHandle(hFile);
		}
	}
	return false;
} 

HRESULT KG3DTexture::RefreshOption(unsigned uOption)
{
	m_uOption = uOption;
	return S_OK;
}

HRESULT KG3DTexture::ColorFill( CONST RECT* pRect, D3DCOLOR color )
{
	return KG3D_CUSTOM_HELPERS::FillTexture(m_pTexture, pRect, color);
}

HRESULT KG3DTexture::SetAsRenderTarget(LPDIRECT3DDEVICE9 pDevice, DWORD RTIndex)	//不要直接在里面就用g_pd3dDevice，要显式的传，这样子对device的使用才能函数中清晰的表达。
{
	_ASSERTE(NULL != pDevice);
	KG_PROCESS_ERROR(NULL != m_pTexture);
	{
		LPDIRECT3DSURFACE9 lpSurface = NULL;
		HRESULT hr = m_pTexture->GetSurfaceLevel(0, &lpSurface);
		KG_COM_PROCESS_ERROR(hr);

		hr = pDevice->SetRenderTarget(RTIndex, lpSurface);

		SAFE_RELEASE(lpSurface);

		return hr;
	}
Exit0:
	return E_FAIL;
}

HRESULT KG3DTexture::SetAsDelphStencil(LPDIRECT3DDEVICE9 pDevice)
{
	_ASSERTE(NULL != pDevice);
	KG_PROCESS_ERROR(NULL != m_pDepthStencilSurface);
	{
		HRESULT hr = pDevice->SetDepthStencilSurface(m_pDepthStencilSurface);
		return hr;
	}
Exit0:
	return E_FAIL;
}

HRESULT KG3DTexture::StretchRect( KG3DTexture& DestTex, CONST RECT* pSrcRect, CONST RECT* pDestRect, D3DTEXTUREFILTERTYPE FilterType )
{
	HRESULT hr = E_FAIL;
    LPDIRECT3DSURFACE9 pSrcSurface = NULL;
    LPDIRECT3DSURFACE9 pDestSurface = NULL;
    
    KG_PROCESS_ERROR(m_pTexture);
    m_pTexture->GetSurfaceLevel(0, &pSrcSurface);
    KG_PROCESS_ERROR(pSrcSurface);

    KG_PROCESS_ERROR(DestTex.m_pTexture);
    DestTex.m_pTexture->GetSurfaceLevel(0, &pDestSurface);
	KG_PROCESS_ERROR(pDestSurface);

	_ASSERTE(NULL != g_pd3dDevice);

	hr = g_pd3dDevice->StretchRect(pSrcSurface, pSrcRect, pDestSurface, pDestRect, FilterType);;

Exit0:
	SAFE_RELEASE(pSrcSurface);
	SAFE_RELEASE(pDestSurface);
	return hr;
}

HRESULT KG3DVideoMemTexture::OnResourceUse()
{
	TKG3DResourceLimitSharePool<KG3DVideoMemTexture>* pPool = 
		static_cast< TKG3DResourceLimitSharePool<KG3DVideoMemTexture>* >(pMgr);

	return pPool->OnResourceUse(this);
}

KG3DVideoMemTexture::~KG3DVideoMemTexture()
{
	OnFreeResource();
}

HRESULT KG3DVideoMemTexture::OnFreeResource()
{
	SAFE_RELEASE(pTexture);
	return S_OK;
}
