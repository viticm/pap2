#include "StdAfx.h"
#include "D3DManager.h"
#include <string.h>

CD3DManager::CD3DManager()
{
	m_prect = NULL;
	m_hRenderWnd = NULL;
	m_pIDirect3DDevice = NULL;
	m_pIDirect3D = NULL;
	m_pFileTexture = NULL;
	m_pDSFileTexture = NULL;
}

CD3DManager::~CD3DManager()
{
	UnInit();
}

bool CD3DManager::CreateD3Device(HWND hWnd, bool bFullScreen)
{
	if (m_pIDirect3D != NULL)
		return true;

	//创建IDirect3D9对象
	m_pIDirect3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (m_pIDirect3D == NULL)
	{
		return false;
	}
	D3DCAPS9 d3dcaps;
	m_pIDirect3D->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&d3dcaps);
	bool bHarwareProcess;
	if (d3dcaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		bHarwareProcess = true;
	}
	else
	{
		bHarwareProcess = false;
	}
	//取得显示模式
	D3DDISPLAYMODE d3dDisplayMode;
	if (FAILED(m_pIDirect3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&d3dDisplayMode)))
	{
		return false;
	}
	//设置D3D设备的渲染参数
	D3DPRESENT_PARAMETERS d3dPresent_Param; //渲染参数结构体
	ZeroMemory(&d3dPresent_Param,sizeof(d3dPresent_Param));  //结构体清零
	d3dPresent_Param.BackBufferWidth = d3dDisplayMode.Width;
	d3dPresent_Param.BackBufferHeight = d3dDisplayMode.Height;
	d3dPresent_Param.BackBufferFormat = d3dDisplayMode.Format;
	d3dPresent_Param.BackBufferCount = 1; //一个后绘图表面
	d3dPresent_Param.hDeviceWindow = hWnd;
	d3dPresent_Param.Windowed = !bFullScreen; //false为全屏幕显示方式
	d3dPresent_Param.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dPresent_Param.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	//创建D3D设备
	if (bHarwareProcess)
	{
		if (FAILED(m_pIDirect3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hWnd,D3DCREATE_HARDWARE_VERTEXPROCESSING,&d3dPresent_Param,&m_pIDirect3DDevice)))
		{
			return false;
		}
	}
	else
	{
		if (FAILED(m_pIDirect3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hWnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING,&d3dPresent_Param,&m_pIDirect3DDevice)))
		{
			return false;
		}
	}
	return true;
}

BOOL CD3DManager::Init(HWND hRenderWnd)
{
	if (CreateD3Device(hRenderWnd, false))
	{
		m_hRenderWnd = hRenderWnd;
		m_prect = new CRectangle(m_pIDirect3DDevice);
		return TRUE;
	}
	return FALSE;
}

void CD3DManager::UnInit()
{
	SAFE_RELEASE(m_pIDirect3DDevice);
	SAFE_RELEASE(m_pIDirect3D);

	SAFE_RELEASE( m_pFileTexture );
	SAFE_RELEASE( m_pDSFileTexture );

	delete m_prect;
	m_prect = NULL;
	m_hRenderWnd = NULL;
}

#define MAX_LEN 102400
char szBuf[MAX_LEN]; //10K

BOOL CD3DManager::SetTextures(LPCTSTR lpstrFileNames)
{
	if (m_pIDirect3DDevice == NULL)
		return FALSE;

	//m_files.clear();
	if (lstrlen(lpstrFileNames) >= MAX_LEN) 
		return FALSE;

	char* seps   = "\r\n";
	char *token;

	memcpy(szBuf, lpstrFileNames, strlen(lpstrFileNames) + 1);
	token = strtok(szBuf, seps); // C4996
	KG3DTextureAnimation* pTexture = m_prect->GetTextureManager();
	pTexture->Clean();
	while( token != NULL )
	{
		string s = string(token);
		//m_files.push_back(token);
		HRESULT hr = pTexture->LoadFromFile((LPCTSTR)token);
		token = strtok( NULL, seps ); // C4996
	}
	return TRUE;
}

#define EXPORTSIZE 512

BOOL CD3DManager::ExportToFile(INT nWidth, INT nHeight, LPCTSTR lpstrFileNames)
{
	if (m_pIDirect3DDevice == NULL)
		return FALSE;

	if (lstrlen(lpstrFileNames) >= MAX_LEN) 
		return FALSE;

	SAFE_RELEASE( m_pFileTexture );
	SAFE_RELEASE( m_pDSFileTexture );

	INT nExpWidth, nExpHeight;
	nExpWidth = nWidth == -1 ? EXPORTSIZE : nWidth;
	nExpHeight = nHeight == -1 ? EXPORTSIZE : nHeight;

	if (FAILED(m_pIDirect3DDevice->CreateTexture( nExpWidth, nExpHeight,
		1, D3DUSAGE_RENDERTARGET,
		D3DFMT_A8R8G8B8,
		D3DPOOL_DEFAULT,
		&m_pFileTexture,
		NULL ))) 
		return FALSE;

	/*if (FAILED(m_pIDirect3DDevice->CreateDepthStencilSurface( EXPORTSIZE,
		EXPORTSIZE,
		D3DFMT_D24X8,
		D3DMULTISAMPLE_NONE,
		0,
		TRUE,
		&m_pDSFileTexture,
		NULL )))
		return FALSE;*/


	LPDIRECT3DSURFACE9 pOldRT = NULL;
	LPDIRECT3DSURFACE9 pOldDS = NULL;
	{
		//切换渲染目标
		m_pIDirect3DDevice->GetRenderTarget( 0, &pOldRT );
		LPDIRECT3DSURFACE9 pTextureSurf;
		if( SUCCEEDED( m_pFileTexture->GetSurfaceLevel( 0, &pTextureSurf ) ) )
		{
			m_pIDirect3DDevice->SetRenderTarget( 0, pTextureSurf );
			SAFE_RELEASE( pTextureSurf );
		}
		//if( SUCCEEDED( m_pIDirect3DDevice->GetDepthStencilSurface( &pOldDS ) ) )
		//	m_pIDirect3DDevice->SetDepthStencilSurface( m_pDSFileTexture );
	}	

	char* seps   = "\r\n";
	char *token;

	memcpy(szBuf, lpstrFileNames, strlen(lpstrFileNames) + 1);
	token = strtok(szBuf, seps); // C4996
	RECT rc;
	rc.left = 0;
	rc.top = 0;
	rc.right = nExpWidth;
	rc.bottom = nExpHeight;
	while( token != NULL )
	{
		string s = string(token);
		doFrameMove(&rc);
		doRender(&rc);
		
		HRESULT hr = D3DXSaveTextureToFile((LPCTSTR)token, D3DXIFF_JPG, m_pFileTexture, NULL);

		token = strtok( NULL, seps ); // C4996
	}

	//还原渲染目标
	{
		/*
		if( pOldDS )
		{
			m_pIDirect3DDevice->SetDepthStencilSurface( pOldDS );
			pOldDS->Release();
		}
		*/
		m_pIDirect3DDevice->SetRenderTarget( 0, pOldRT );
		SAFE_RELEASE( pOldRT );
	}


	SAFE_RELEASE( m_pFileTexture );
	SAFE_RELEASE( m_pDSFileTexture );

	return TRUE;
}


void CD3DManager::FrameMove()
{
	if (m_prect)
	{
		RECT WndRect;
		::GetClientRect(m_hRenderWnd, &WndRect);
		doFrameMove(&WndRect);
	}
}

void CD3DManager::doFrameMove(LPRECT lpWndRect)
{
	if (lpWndRect)
	{
		m_prect->FrameMove(lpWndRect);
	}
}

void CD3DManager::Render()
{
	RECT WndRect;
	::GetClientRect(m_hRenderWnd, &WndRect);
	doRender(&WndRect);
}

void CD3DManager::doRender(LPRECT lpWndRect)
{
	if (m_pIDirect3DDevice && m_hRenderWnd && lpWndRect)
	{
		RECT WndRect = *lpWndRect;
		float Width = (float)(WndRect.right - WndRect.left);
		float Height = (float)(WndRect.bottom - WndRect.top);

		m_pIDirect3DDevice->Clear(0,NULL,D3DCLEAR_TARGET,D3DCOLOR_XRGB(192,192,192),1.0f,0);
		m_pIDirect3DDevice->BeginScene();

		if (m_prect)
			m_prect->Render();
		m_pIDirect3DDevice->EndScene();

		m_pIDirect3DDevice->Present(&WndRect, NULL, m_hRenderWnd, NULL);
	}
}

void CD3DManager::OnResize()
{
		
}
