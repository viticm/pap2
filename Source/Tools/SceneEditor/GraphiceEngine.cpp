// GraphiceEngine.cpp: implementation of the GraphiceEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "GraphiceEngine.h"
#include ".\graphiceengine.h"
#include "MovieTexture.h"

//#define SHIPPING_VERSION 0 //性能测试

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; // Our rendering devic
LPDIRECT3D9             g_pD3D       = NULL; // Used to create the D3DDevice
D3DCAPS9                g_D3DCaps    ; // Caps for the device
DWORD                   g_dwRenderCount;
KGraphicsEngine         g_GraphiceEngine;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KGraphicsEngine::KGraphicsEngine()
{
	g_dwRenderCount = 0;
	m_bUseMotionBlur = FALSE;
	m_BufferTextureID = 0;
	m_SmallerTextureID = 0;
	m_BlurTextureID = 0;
	m_lpCurScene = NULL;
}

KGraphicsEngine::~KGraphicsEngine()
{
	if (g_pd3dDevice!=NULL) g_pd3dDevice->Release();
	if (g_pD3D!=NULL) g_pD3D->Release();
}

HRESULT KGraphicsEngine::InitializeWindowed(HWND hBaseWindow, HWND hRenderWindow)
{
	// Create the D3D object.
    if ( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
	{
		MessageBox(hBaseWindow,"Failed Create the D3D object!.","Failed Create the D3D object!",0);
        return E_FAIL;
	}
    // Get the current desktop display mode, so we can set up a back
    // buffer of the same format
	g_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&g_D3DCaps);

    if ( FAILED( g_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &m_DisplayMode ) ) )
	{
		MessageBox(hBaseWindow,"Failed GetAdapterDisplayMode !.","Failed Create the D3D object!",0);
		return E_FAIL;
	}
     
    // Set up the structure used to create the D3DDevice
	D3DFORMAT  DepFormat;
	if (SUCCEEDED(g_pD3D->CheckDepthStencilMatch(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		m_DisplayMode.Format,
		m_DisplayMode.Format,
		D3DFMT_D24S8)))
	{
		DepFormat = D3DFMT_D24S8;
	}
	else
	{
		DepFormat = D3DFMT_D16;
	}

    memset( &m_PresentParam, 0, sizeof(D3DPRESENT_PARAMETERS) );

    memset(&m_RenderWindowRect, 0, sizeof(m_RenderWindowRect));	

	if(hRenderWindow)
		::GetWindowRect(hRenderWindow,&m_RenderWindowRect);
	else
	{
		::GetWindowRect(hBaseWindow,&m_RenderWindowRect);
		hRenderWindow = hBaseWindow;
	}
		
	//m_PresentParam.BackBufferWidth = m_RenderWindowRect.right-m_RenderWindowRect.left ;
    //m_PresentParam.BackBufferHeight= m_RenderWindowRect.bottom - m_RenderWindowRect.top;
	m_PresentParam.BackBufferWidth = m_DisplayMode.Width ;
	m_PresentParam.BackBufferHeight= m_DisplayMode.Height;

    m_PresentParam.Windowed = TRUE ;
    m_PresentParam.SwapEffect = D3DSWAPEFFECT_DISCARD;
    m_PresentParam.BackBufferFormat = m_DisplayMode.Format;
	m_PresentParam.EnableAutoDepthStencil = TRUE;
    m_PresentParam.AutoDepthStencilFormat = DepFormat;
    m_PresentParam.MultiSampleType = D3DMULTISAMPLE_NONE;
    m_PresentParam.hDeviceWindow = hBaseWindow;
    m_PresentParam.Flags = 0;
    m_PresentParam.FullScreen_RefreshRateInHz = 0;
    m_PresentParam.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	//just set camera's aspect
	m_cCamera.Aspect = m_PresentParam.BackBufferWidth*1.0f/m_PresentParam.BackBufferHeight; 

	HRESULT hr;

	// Set default settings
	UINT AdapterToUse=D3DADAPTER_DEFAULT;
	D3DDEVTYPE DeviceType=D3DDEVTYPE_HAL;
//#if SHIPPING_VERSION
	// When building a shipping version, disable NVPerfHUD (opt-out)
//#else
	// Look for 'NVIDIA NVPerfHUD' adapter
	// If it is present, override default settings
	for (UINT Adapter=0;Adapter<g_pD3D->GetAdapterCount();Adapter++)
	{
		D3DADAPTER_IDENTIFIER9 Identifier;
		HRESULT Res=g_pD3D->GetAdapterIdentifier(Adapter,0,&Identifier);
		if (strcmp(Identifier.Description,"NVIDIA NVPerfHUD")==0)
		{
			AdapterToUse=Adapter;
			DeviceType=D3DDEVTYPE_REF;
			break;
		}
	}
//#endif

    // Create the D3DDevice
    /*if ( FAILED(hr = g_pD3D->CreateDevice( AdapterToUse, DeviceType, hBaseWindow,
                                      D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                      &m_PresentParam, &g_pd3dDevice ) ) )
    {*/
		if ( FAILED(hr = g_pD3D->CreateDevice( AdapterToUse, DeviceType, hBaseWindow,
                                      D3DCREATE_MIXED_VERTEXPROCESSING,
                                      &m_PresentParam, &g_pd3dDevice ) ) )
		{
			if ( FAILED(hr = g_pD3D->CreateDevice( AdapterToUse, DeviceType, hBaseWindow,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &m_PresentParam, &g_pd3dDevice ) ) )
			{
				
				if (hr == D3DERR_INVALIDCALL)
					MessageBox(hBaseWindow,"The method call is invalid. For example, a method's parameter may have an invalid value.","Failed Create Device!",0);
				else if (hr == E_OUTOFMEMORY )
					MessageBox(hBaseWindow,"Direct3D could not allocate sufficient memory to complete the call.","Failed Create Device!",0);
				else if (hr == D3DERR_OUTOFVIDEOMEMORY)
					MessageBox(hBaseWindow,"Direct3D does not have enough display memory to perform the operation. ","Failed Create Device!",0);
				return E_FAIL;
			}
		}
	
  /*  }*/
    
//	D3DMULTISAMPLE_TYPE Mul = D3DMULTISAMPLE_4_SAMPLES;
//
//	if( FAILED(g_pD3D->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT, 
//			D3DDEVTYPE_HAL, m_PresentParam.BackBufferFormat, 
//			FALSE, Mul, NULL ) ) &&
//			SUCCEEDED(g_pD3D->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT, 
//			D3DDEVTYPE_HAL, m_PresentParam.AutoDepthStencilFormat, 
//			FALSE, Mul, NULL ) ) )
//	{
//		Mul = D3DMULTISAMPLE_3_SAMPLES; 
//
//		if( FAILED(g_pD3D->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT, 
//			D3DDEVTYPE_HAL, m_PresentParam.BackBufferFormat, 
//			FALSE, Mul, NULL ) ) &&
//			SUCCEEDED(g_pD3D->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT, 
//			D3DDEVTYPE_HAL, m_PresentParam.AutoDepthStencilFormat, 
//			FALSE, Mul, NULL ) ) )
//		{
//			Mul = D3DMULTISAMPLE_2_SAMPLES; 
//
//			if( FAILED(g_pD3D->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT, 
//				D3DDEVTYPE_HAL, m_PresentParam.BackBufferFormat, 
//				FALSE, Mul, NULL ) ) &&
//				SUCCEEDED(g_pD3D->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT, 
//				D3DDEVTYPE_HAL, m_PresentParam.AutoDepthStencilFormat, 
//				FALSE, Mul, NULL ) ) )
//			{
//				Mul = D3DMULTISAMPLE_NONE; 
//			}
//		}
//	}
//	if (Mul != D3DMULTISAMPLE_NONE)
//	{
//		m_PresentParam.MultiSampleType = Mul;
//		g_pd3dDevice->Reset( &m_PresentParam );
//	}
    
    // Device state would normally be set here
	// Turn off culling, so we see the front and back of the triangle
     g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW  );

    // Turn off D3D lighting, since we are providing our own vertex colors

    // Turn on the zbuffer
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
    //g_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE , TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL  );

	g_cGraphicsTool.SetCurCamera(&m_cCamera);
	
	PreRender(); 
	m_bWindowed = TRUE;
    
	if (m_bUseMotionBlur)
	{
		BackBufferCreate();
		
		g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
			0xff666666, 1.0f, 0 );
	}

	return S_OK;
}

HRESULT KGraphicsEngine::PreRender()
{
	m_cCamera.SetCamera();
	RECT r;
	GetWindowRect(g_hBaseWnd,&r);
	float Weigth = (r.right - r.left)*1.0f;
	float Height = (r.bottom - r.top)*1.0f ;
	m_cCamera.Aspect = Weigth/Height;

	D3DLIGHT9 light;
    ZeroMemory( &light, sizeof(D3DLIGHT9) );
    light.Type       = D3DLIGHT_DIRECTIONAL;
    light.Diffuse.r  = 0.80f;
    light.Diffuse.g  = 0.80f;
    light.Diffuse.b  = 0.80f;
    light.Ambient.r  = 0.80f;
    light.Ambient.g  = 0.80f;
    light.Ambient.b  = 0.80f;
    light.Specular.r  = 1.00f;
    light.Specular.g  = 1.00f;
    light.Specular.b  = 1.00f;
 
	D3DXVECTOR3 vecDir = D3DXVECTOR3(10*sinf(timeGetTime()/1750.0f),
		                 100,
		                 10*cosf(timeGetTime()/1750.0f) );

	D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecDir );
    light.Range       = 1000.0f;
    
	g_pd3dDevice->SetLight( 0, &light );
	g_pd3dDevice->LightEnable( 0, TRUE );

	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE , D3DCULL_NONE );

    g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x0 );
    g_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE , TRUE );
	return S_OK;
}

HRESULT KGraphicsEngine::Render()
{
	HRESULT hr = S_OK;
	
	if(!g_hRenderWnd)
		return FALSE;

	if(TRUE!=::GetWindowRect(g_hRenderWnd,&m_RenderWindowRect))
	{
		//MessageBox(g_hBaseWnd,"不知道该渲染到什么地方","Error!",0);

		return FALSE;
	}

	if (m_lpCurScene!=NULL) 
		m_lpCurScene->FrameMove();

	g_cGraphicsTool.SetCurScene(m_lpCurScene);
	
	RECT ScrRect;
	ScrRect.left = 0;
	ScrRect.right = m_RenderWindowRect.right - m_RenderWindowRect.left;
	ScrRect.top = 0;
	ScrRect.bottom = m_RenderWindowRect.bottom - m_RenderWindowRect.top;

	D3DVIEWPORT9 ViewPort;
	ViewPort.X = m_RenderWindowRect.left;
	ViewPort.Y = m_RenderWindowRect.top;
	ViewPort.Width  = ScrRect.right;
	ViewPort.Height = ScrRect.bottom;
	ViewPort.MinZ = 0;
	ViewPort.MaxZ = 1.0f;
	g_pd3dDevice->SetViewport(&ViewPort);
	
	LPDIRECT3DSURFACE9 lpSurface = NULL;
	LPDIRECT3DSURFACE9 lpSurfaceSave = NULL;
	
	if (m_bUseMotionBlur)
	{	
		if (m_BufferTextureID)
		{
			g_pd3dDevice->GetRenderTarget(0,&lpSurfaceSave);
			lpSurfaceSave->Release();
			
			LPTEXTURE pTexture = NULL;
			if (FAILED(g_cTextureTable.GetTexture(&pTexture,m_BufferTextureID)))
			{
				return E_FAIL;
			}
			
			if (FAILED(hr = pTexture->m_lpTexture->GetSurfaceLevel(0,&lpSurface)))
				return hr;
			
			lpSurface->Release();
			g_pd3dDevice->SetRenderTarget(0,lpSurface);
		}
	}


	m_cCamera.zFar = 1000000;
	if (!m_bWindowed)
	{
		m_cCamera.Aspect = m_DisplayMode.Width*1.0f/m_DisplayMode.Height;
	}
	m_cCamera.SetCamera();

	if (m_lpCurScene!=NULL) 
		m_lpCurScene->SetLighting();

	g_pd3dDevice->BeginScene();
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
		0xFF606060, 1.0f, 0 );

	if (m_lpCurScene!=NULL) 
		m_lpCurScene->Render();
	
	
	//////////////////////////////////////////////////////////////////////////
	/*D3DXVECTOR3 V = D3DXVECTOR3(400*sinf(timeGetTime()*0.001f) ,
		                 100,
		                 400*cosf(timeGetTime()*0.001f) -2000 );
	g_cGraphicsTool.DrawLine(&(V+D3DXVECTOR3(0,35,0)),&(V+D3DXVECTOR3(0,-35,0)),0xffffffff,0xffff0000); 
	g_cGraphicsTool.DrawLine(&(V+D3DXVECTOR3(35,0,0)),&(V+D3DXVECTOR3(-35,0,0)),0xffffffff,0xffff0000); 
	g_cGraphicsTool.DrawLine(&(V+D3DXVECTOR3(0,0,35)),&(V+D3DXVECTOR3(0,0,-35)),0xffffffff,0xffff0000); */
	g_pd3dDevice->EndScene();

	if (m_bUseMotionBlur)
	{
		if (m_BufferTextureID)
		{
			g_pd3dDevice->SetRenderState(D3DRS_FOGENABLE,FALSE);

			g_pd3dDevice->SetRenderTarget(0,lpSurfaceSave);

			Render2SmallerTexture();
			
			g_pd3dDevice->BeginScene();
			g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
				0x00000000, 1.0f, 0 );

			D3DXVECTOR2 A(0,0),C((float)m_DisplayMode.Width,(float)m_DisplayMode.Height);
			float K = 3;

			D3DXVECTOR2 T1(5,5);
			D3DXVECTOR2 T2(-3,3);

			
			//g_cTextureTable.SetTexture(0,m_BufferTextureID);
			g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE ,FALSE);
			g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
			g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
			
			
			//g_cGraphicsTool.DrawScreenRectNormal(&A,&C,0.0f,0x90FFFFFF,0x90FFFFFF,0x90FFFFFF,0x90FFFFFF);
			
			g_cGraphicsTool.DrawScreenRect(&A,&C,0.0f,0xFFFFFFFF,m_BufferTextureID);

			g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
			g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);

			if (m_lpCurScene!=NULL) 
			{
				g_cGraphicsTool.DrawScreenRect(&(A-T1),&(C),0.0f,m_lpCurScene->m_Glow.m_AddColor,m_SmallerTextureID);
			}
			/*g_cGraphicsTool.DrawScreenRect(&(A-T1),&(C-T1),0.0f,0xFF808080,m_BufferTextureID);
			g_cGraphicsTool.DrawScreenRect(&(A+T1),&(C+T1),0.0f,0xFF808080,m_BufferTextureID);
			g_cGraphicsTool.DrawScreenRect(&(A-T2),&(C-T2),0.0f,0xFF808080,m_BufferTextureID);
			g_cGraphicsTool.DrawScreenRect(&(A+T2),&(C+T2),0.0f,0xFF808080,m_BufferTextureID);*/
			//g_cGraphicsTool.DrawScreenRectDp3(&A,&C,0.0f,0xFF8F8F8F,m_BufferTextureID);
			g_pd3dDevice->EndScene();
			
		}
	}

	if (m_bWindowed)
	{
		//只将backbuffer中的一个区域提交到窗口上
		if(g_hRenderWnd)
		{
			g_pd3dDevice->Present( &m_RenderWindowRect,NULL, g_hRenderWnd, NULL );
		}
		else
		{
			MessageBox(g_hBaseWnd,"不知道该渲染到什么地方","Error!",0);
		}
	}
	else
	{
		g_pd3dDevice->Present( NULL, NULL, g_hRenderWnd, NULL );
	}

	g_dwRenderCount++;
	return TRUE;
}

HRESULT KGraphicsEngine::SetCurScene(LPSCENE pScene)
{
	m_lpCurScene = pScene;
	return S_OK;
}

HRESULT KGraphicsEngine::UpdateLight()
{
	D3DLIGHT9 light;
    ZeroMemory( &light, sizeof(D3DLIGHT9) );
    light.Type       = D3DLIGHT_DIRECTIONAL;
    
	light.Diffuse.r  = 1.0f;
    light.Diffuse.g  = 1.0f;
    light.Diffuse.b  = 1.0f;
    
	light.Ambient.r  = 0.40f;
    light.Ambient.g  = 0.40f;
    light.Ambient.b  = 0.40f;
	
	if(g_cGraphicsTool.m_bSunSpecular)
	{
		light.Specular.r  = 1.00f;
		light.Specular.g  = 1.00f;
		light.Specular.b  = 1.00f;
	}
	else
	{
		light.Specular.r  = 0.00f;
		light.Specular.g  = 0.00f;
		light.Specular.b  = 0.00f;
	}
 
	D3DXVECTOR3 vecDir;
	g_cGraphicsTool.GetSunDirection(vecDir);
	/*D3DXVECTOR3 vecDir = D3DXVECTOR3(0, -1, 0 );*/
	//D3DXVECTOR3 vecDir(0,0.3f,1);

	D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecDir );
    light.Range       = 50000.0f;
    
	g_pd3dDevice->SetLight( 0, &light );
	g_pd3dDevice->LightEnable( 0, TRUE );

	
    light.Type       = D3DLIGHT_POINT;
    light.Diffuse.r  = 0.90f;
    light.Diffuse.g  = 0.70f;
    light.Diffuse.b  = 0.60f;
    light.Ambient.r  = 0.80f;
    light.Ambient.g  = 0.80f;
    light.Ambient.b  = 0.80f;
    light.Specular.r  = 1.00f;
    light.Specular.g  = 1.00f;
    light.Specular.b  = 1.00f;
	light.Attenuation0 = 0.01f;
	light.Attenuation1 = 0.02f;
	light.Attenuation2 = 0;
     light.Range       = 500.0f;

	vecDir = D3DXVECTOR3(400*sinf(timeGetTime()*0.001f) ,
		                 100,
		                 400*cosf(timeGetTime()*0.001f) -2000 );

	light.Position = vecDir ;
    
	g_pd3dDevice->SetLight( 1, &light );
	g_pd3dDevice->LightEnable( 1, TRUE );
	
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE , D3DCULL_CW );

    g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, g_cGraphicsTool.GetAmbient() );
    g_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE , TRUE );

	return S_OK;
}

HRESULT KGraphicsEngine::GetPickRay(D3DXVECTOR3 * RayOrig,D3DXVECTOR3* RayDir )
{
	D3DXVECTOR3 vPickRayDir;
    D3DXVECTOR3 vPickRayOrig;
    
	D3DXMATRIX matProj;
    g_pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );

    POINT ptCursor;
    GetCursorPos( &ptCursor );
    //ScreenToClient( g_hRenderWnd, &ptCursor );

	//ptCursor.x -= m_RenderWindowRect.left;
	//ptCursor.y += m_RenderWindowRect.top;

    D3DXVECTOR3 v;
    // Compute the vector of the pick ray in screen space
    ::GetWindowRect(g_hRenderWnd,&m_RenderWindowRect);

	float Width  = (float)( m_RenderWindowRect.right - m_RenderWindowRect.left);
	float Height = (float)( m_RenderWindowRect.bottom - m_RenderWindowRect.top);

	float X = (float)(ptCursor.x - m_RenderWindowRect.left);
	float Y = (float)(ptCursor.y - m_RenderWindowRect.top );

	v.x =(float)  ( ( ( 2.0f * X ) / Width  ) - 1.0f) / matProj._11;
    v.y =(float) -( ( ( 2.0f * Y ) / Height ) - 1.0f) / matProj._22;
    v.z =  1.0f;

    // Get the inverse view matrix
    
	D3DXMATRIX matView, m;
    g_pd3dDevice->GetTransform( D3DTS_VIEW, &matView );
    D3DXMatrixInverse( &m, NULL, &matView );

    // Transform the screen space pick ray into 3D space
    vPickRayDir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
    vPickRayDir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
    vPickRayDir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
    vPickRayOrig.x = m._41 ;
    vPickRayOrig.y = m._42 ; 
    vPickRayOrig.z = m._43 ;

	*RayOrig = vPickRayOrig;
	*RayDir  = vPickRayDir ;
	
	return S_OK;
}

HRESULT KGraphicsEngine::GetPickRay(D3DXVECTOR3 * RayOrig,D3DXVECTOR3* RayDir,POINT Cursor)
{
	D3DXVECTOR3 vPickRayDir;
    D3DXVECTOR3 vPickRayOrig;
    
	D3DXMATRIX matProj;
    g_pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );

    POINT ptCursor = Cursor;
	ScreenToClient( g_hRenderWnd, &ptCursor );

	//ptCursor.x -= m_RenderWindowRect.left;
	//ptCursor.y += m_RenderWindowRect.top;

    D3DXVECTOR3 v;
    // Compute the vector of the pick ray in screen space
         

	float Width  = (float)( m_RenderWindowRect.right - m_RenderWindowRect.left);
	float Height = (float)( m_RenderWindowRect.bottom - m_RenderWindowRect.top);

	v.x =(float)  ( ( ( 2.0f * ptCursor.x ) / Width  ) - 1.0 ) / matProj._11;
    v.y =(float) -( ( ( 2.0f * ptCursor.y ) / Height ) - 1.0) / matProj._22;
    v.z =  1.0f;

    // Get the inverse view matrix
    
	D3DXMATRIX matView, m;
    g_pd3dDevice->GetTransform( D3DTS_VIEW, &matView );
    D3DXMatrixInverse( &m, NULL, &matView );

    // Transform the screen space pick ray into 3D space
    vPickRayDir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
    vPickRayDir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
    vPickRayDir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
    vPickRayOrig.x = m._41 ;
    vPickRayOrig.y = m._42 ; 
    vPickRayOrig.z = m._43 ;

	*RayOrig = vPickRayOrig;
	*RayDir  = vPickRayDir ;
	
	return S_OK;
}
HRESULT KGraphicsEngine::SetRenderWindow(HWND hRenderWindow)
{
	if (!m_PresentParam.Windowed)
		return S_OK;
	
	HRESULT hr = S_OK;

	g_hRenderWnd = hRenderWindow;
	if(hRenderWindow)
	{
		GetWindowRect(hRenderWindow,&m_RenderWindowRect);
	}
		
	float Width  = (m_RenderWindowRect.right-m_RenderWindowRect.left  )*1.0f;
	float Height = (m_RenderWindowRect.bottom - m_RenderWindowRect.top)*1.0f;

	m_cCamera.Aspect = Width/Height;


	D3DVIEWPORT9 ViewPort;
	ViewPort.X = m_RenderWindowRect.left;
	ViewPort.Y = m_RenderWindowRect.top;
	ViewPort.Width  = m_RenderWindowRect.right - m_RenderWindowRect.left;
	ViewPort.Height = m_RenderWindowRect.bottom - m_RenderWindowRect.top;
	ViewPort.MinZ = 0;
	ViewPort.MaxZ = 1.0f;
	g_pd3dDevice->SetViewport(&ViewPort);

	/*m_PresentParam.BackBufferWidth = m_RenderWindowRect.right-m_RenderWindowRect.left ;
    m_PresentParam.BackBufferHeight= m_RenderWindowRect.bottom - m_RenderWindowRect.top;
	m_PresentParam.Windowed = TRUE;
    if ( FAILED(hr = g_pd3dDevice->Reset(&m_PresentParam)))
    {
        //fprintf(Note,"Create D3D Device Failed.\n");
		if (hr == D3DERR_INVALIDCALL)
			MessageBox(g_hBaseWnd,"The method call is invalid. For example, a method's parameter may have an invalid value.","Failed Create Device!",0);
		else if (hr == E_OUTOFMEMORY )
			MessageBox(g_hBaseWnd,"Direct3D could not allocate sufficient memory to complete the call.","Failed Create Device!",0);
		else if (hr == D3DERR_OUTOFVIDEOMEMORY)
			MessageBox(g_hBaseWnd,"Direct3D does not have enough display memory to perform the operation. ","Failed Create Device!",0);

        return E_FAIL;
    }*/
	return S_OK;
}


HRESULT KGraphicsEngine::Toggle2FullScreen()
{
	if (m_lpCurScene)
	{
		g_cMeshTable.InvalidateDeviceObjects();
		g_SceneTable.InvalidateDeviceObjects();
	}

	g_EffectMgr.InvalidateDeviceObjects();

    // Create the application's window
	m_BaseWinStyle = GetWindowLong(g_hBaseWnd,GWL_STYLE);
	SetWindowLong( g_hBaseWnd , GWL_STYLE, WS_POPUP|WS_SYSMENU|WS_VISIBLE );
	//m_LastRenderWindow = g_hRenderWnd;
	g_hRenderWnd = g_hBaseWnd;
	//GetWindowRect(g_hRenderWnd,&m_RenderWindowRect);
	//m_RenderWindowRect.left = 0;
	//m_RenderWindowRect.top = 0;
	//m_RenderWindowRect.right = m_DisplayMode.Width;
	//m_RenderWindowRect.bottom = m_DisplayMode.Height;
	

	//float Width  = (m_RenderWindowRect.right-m_RenderWindowRect.left  )*1.0f;
	//float Height = (m_RenderWindowRect.bottom - m_RenderWindowRect.top)*1.0f;


	m_PresentParam.BackBufferWidth = m_DisplayMode.Width;
    m_PresentParam.BackBufferHeight= m_DisplayMode.Height;
	
	m_cCamera.Aspect = m_DisplayMode.Width*1.0f/m_DisplayMode.Height;
	m_PresentParam.hDeviceWindow = g_hRenderWnd;
    m_PresentParam.Windowed = FALSE ;
	m_PresentParam.BackBufferCount = 2;
    m_PresentParam.SwapEffect = D3DSWAPEFFECT_DISCARD;
    m_PresentParam.BackBufferFormat = m_DisplayMode.Format;
	m_PresentParam.EnableAutoDepthStencil = TRUE;
    m_PresentParam.MultiSampleType = D3DMULTISAMPLE_NONE;
    m_PresentParam.Flags = 0;
    m_PresentParam.FullScreen_RefreshRateInHz = 0;
    m_PresentParam.PresentationInterval = D3DPRESENT_INTERVAL_ONE;//D3DPRESENT_INTERVAL_IMMEDIATE;

	if (m_bUseMotionBlur)
	{
		BackBufferRelease();
	}
	/*D3DMULTISAMPLE_TYPE Mul = D3DMULTISAMPLE_4_SAMPLES;

	if( FAILED(g_pD3D->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT, 
		D3DDEVTYPE_HAL, m_PresentParam.BackBufferFormat, 
		FALSE, Mul, NULL ) ) &&
		SUCCEEDED(g_pD3D->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT, 
		D3DDEVTYPE_HAL, m_PresentParam.AutoDepthStencilFormat, 
		FALSE, Mul, NULL ) ) )
	{
		Mul = D3DMULTISAMPLE_3_SAMPLES; 

		if( FAILED(g_pD3D->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT, 
			D3DDEVTYPE_HAL, m_PresentParam.BackBufferFormat, 
			FALSE, Mul, NULL ) ) &&
			SUCCEEDED(g_pD3D->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT, 
			D3DDEVTYPE_HAL, m_PresentParam.AutoDepthStencilFormat, 
			FALSE, Mul, NULL ) ) )
		{
			Mul = D3DMULTISAMPLE_2_SAMPLES; 

			if( FAILED(g_pD3D->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT, 
				D3DDEVTYPE_HAL, m_PresentParam.BackBufferFormat, 
				FALSE, Mul, NULL ) ) &&
				SUCCEEDED(g_pD3D->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT, 
				D3DDEVTYPE_HAL, m_PresentParam.AutoDepthStencilFormat, 
				FALSE, Mul, NULL ) ) )
			{
				Mul = D3DMULTISAMPLE_NONE; 
			}
		}
	}
	if (Mul != D3DMULTISAMPLE_NONE)
	{
		m_PresentParam.MultiSampleType = Mul;
	}*/

	HRESULT hr;
    // Create the D3DDevice
    if ( FAILED(hr = g_pd3dDevice->Reset( &m_PresentParam )))
    {
        //fprintf(Note,"Create D3D Device Failed.\n");
		if (hr == D3DERR_INVALIDCALL)
			MessageBox(g_hBaseWnd,"The method call is invalid. For example, a method's parameter may have an invalid value.","Failed Create Device!",0);
		else if (hr == E_OUTOFMEMORY )
			MessageBox(g_hBaseWnd,"Direct3D could not allocate sufficient memory to complete the call.","Failed Create Device!",0);
		else if (hr == D3DERR_OUTOFVIDEOMEMORY)
			MessageBox(g_hBaseWnd,"Direct3D does not have enough display memory to perform the operation. ","Failed Create Device!",0);

        return E_FAIL;
    }

	//g_pd3dDevice->SetDialogBoxMode( TRUE );
	g_hRenderWnd = g_hBaseWnd;
	m_bWindowed = FALSE;

	if (m_lpCurScene)
	{
		g_cMeshTable.RestoreDeviceObjects();
		g_SceneTable.RestoreDeviceObjects();
	}

	g_EffectMgr.RestoreDeviceObjects();

	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING,TRUE);

	if (m_bUseMotionBlur)
	{
		BackBufferCreate();
	}
	return E_FAIL;
}

HRESULT KGraphicsEngine::Toggle2Windowed()
{
	if (m_lpCurScene)
	{
		g_cMeshTable.InvalidateDeviceObjects();
		g_SceneTable.InvalidateDeviceObjects();
	}
	g_EffectMgr.InvalidateDeviceObjects();

	if (m_bUseMotionBlur)
	{
		BackBufferRelease();
	}

	HRESULT hr = S_OK;
	
	GetWindowRect(m_LastRenderWindow,&m_RenderWindowRect);
		
	//m_PresentParam.BackBufferWidth = m_RenderWindowRect.right-m_RenderWindowRect.left ;
    //m_PresentParam.BackBufferHeight= m_RenderWindowRect.bottom - m_RenderWindowRect.top;
	m_PresentParam.BackBufferWidth = m_DisplayMode.Width;
    m_PresentParam.BackBufferHeight= m_DisplayMode.Height;

  	D3DFORMAT  DepFormat;
	if (SUCCEEDED(g_pD3D->CheckDepthStencilMatch(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		m_DisplayMode.Format,
		m_DisplayMode.Format,
		D3DFMT_D24S8)))
	{
		DepFormat = D3DFMT_D24S8;
	}
	else
	{
		DepFormat = D3DFMT_D16;
	}
	m_PresentParam.Windowed = TRUE ;
    m_PresentParam.SwapEffect = D3DSWAPEFFECT_DISCARD;
    m_PresentParam.BackBufferFormat = m_DisplayMode.Format;
	m_PresentParam.EnableAutoDepthStencil = TRUE;
    m_PresentParam.AutoDepthStencilFormat = DepFormat;
    m_PresentParam.MultiSampleType = D3DMULTISAMPLE_NONE;
    m_PresentParam.hDeviceWindow = g_hBaseWnd;
    m_PresentParam.Flags = 0;
    m_PresentParam.FullScreen_RefreshRateInHz = 0;
	m_PresentParam.BackBufferCount = 0;
    m_PresentParam.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

	//just set camera's aspect
	m_cCamera.Aspect = m_PresentParam.BackBufferWidth*1.0f/m_PresentParam.BackBufferHeight; 

	g_pd3dDevice->SetDialogBoxMode( FALSE );
   
	if ( FAILED(hr = g_pd3dDevice->Reset( &m_PresentParam )))
    {
        //fprintf(Note,"Create D3D Device Failed.\n");
		if (hr == D3DERR_INVALIDCALL)
			MessageBox(g_hBaseWnd,"Toggle to Windowed Failed!","Failed in Toggling",0);
		else if (hr == E_OUTOFMEMORY )
			MessageBox(g_hBaseWnd,"Direct3D could not allocate sufficient memory to complete the call.","Failed Create Device!",0);
		else if (hr == D3DERR_OUTOFVIDEOMEMORY)
			MessageBox(g_hBaseWnd,"Direct3D does not have enough display memory to perform the operation. ","Failed Create Device!",0);

        return E_FAIL;
    }
	SetWindowLong( g_hBaseWnd , GWL_STYLE, m_BaseWinStyle );

	SetRenderWindow(m_LastRenderWindow);

	m_bWindowed = TRUE;

	if (m_lpCurScene)
	{
		g_cMeshTable.RestoreDeviceObjects();
		g_SceneTable.RestoreDeviceObjects();
	}
	g_EffectMgr.RestoreDeviceObjects();

	if (m_bUseMotionBlur)
	{
		BackBufferCreate();
	}
	return S_OK;
}

void KGraphicsEngine::SetCurCamera(GraphicsStruct::KCamera* pCamera)
{
	m_cCamera = *pCamera;
}

GraphicsStruct::KCamera* KGraphicsEngine::GetCurCamera(void)
{
	return &m_cCamera;
}

HRESULT KGraphicsEngine::CleanUp(void)
{
	m_lpCurScene = NULL;	
	SAFE_RELEASE(g_pd3dDevice);
	return E_NOTIMPL;
}

HRESULT KGraphicsEngine::Render2SmallerTexture()
{
	LPTEXTURE pTexBuffer  = NULL;
	LPTEXTURE pTexSmaller = NULL;
	LPTEXTURE pTexBlur    = NULL;

	if (FAILED(g_cTextureTable.GetTexture(&pTexBuffer,m_BufferTextureID)))
		return E_FAIL;
	if (FAILED(g_cTextureTable.GetTexture(&pTexSmaller,m_SmallerTextureID)))
		return E_FAIL;
	if (FAILED(g_cTextureTable.GetTexture(&pTexBlur,m_BlurTextureID)))
		return E_FAIL;
	if(!pTexBuffer->m_lpTexture)
		return E_FAIL;
	if(!pTexSmaller->m_lpTexture)
		return E_FAIL;
	if(!pTexBlur->m_lpTexture)
		return E_FAIL;
	D3DSURFACE_DESC Desc;
	if(FAILED(pTexSmaller->m_lpTexture->GetLevelDesc(0,&Desc)))
		return E_FAIL;
	D3DSURFACE_DESC DescBlur;
	if(FAILED(pTexBlur->m_lpTexture->GetLevelDesc(0,&DescBlur)))
		return E_FAIL;

	LPDIRECT3DSURFACE9 lpSurfaceSmaller = NULL;
	if(FAILED(pTexSmaller->m_lpTexture->GetSurfaceLevel(0,&lpSurfaceSmaller)))
		return E_FAIL;
	lpSurfaceSmaller->Release();
	LPDIRECT3DSURFACE9 lpSurfaceBlur = NULL;
	if(FAILED(pTexBlur->m_lpTexture->GetSurfaceLevel(0,&lpSurfaceBlur)))
		return E_FAIL;
	lpSurfaceBlur->Release();

	D3DXMATRIX Mat_View_Save;
	D3DXMATRIX Mat_Proj_Save;
	LPDIRECT3DSURFACE9 lpSurfaceSave = NULL;
	D3DVIEWPORT9 ViewPort,ViewPortBlur,ViewPortSave;
	//////////////////////////////////////////////////////////////////////////
	//Save back buffer & transform & viewport
	g_pd3dDevice->GetRenderTarget(0,&lpSurfaceSave);
	lpSurfaceSave->Release();
	g_pd3dDevice->GetTransform( D3DTS_VIEW, &Mat_View_Save );
	g_pd3dDevice->GetTransform( D3DTS_PROJECTION, &Mat_Proj_Save );
	g_pd3dDevice->GetViewport(&ViewPortSave);


	//////////////////////////////////////////////////////////////////////////

	ViewPort.X = 0;ViewPort.Y = 0;
	ViewPort.Width = Desc.Width;
	ViewPort.Height = Desc.Height;
	ViewPort.MinZ = 0;ViewPort.MaxZ = 1.0f;

	ViewPortBlur.X = 0;ViewPortBlur.Y = 0;
	ViewPortBlur.Width  = DescBlur.Width;
	ViewPortBlur.Height = DescBlur.Height;
	ViewPortBlur.MinZ = 0;ViewPortBlur.MaxZ = 1.0f;

	D3DXVECTOR2 A(0,0);
	D3DXVECTOR2 C((float)Desc.Width,(float)Desc.Height);

	D3DXVECTOR2 A1(0,0);
	D3DXVECTOR2 C1((float)DescBlur.Width,(float)DescBlur.Height);

	//////////////////////////////////////////////////////////////////////////
	g_pd3dDevice->SetViewport(&ViewPort);
	g_pd3dDevice->SetRenderTarget(0,lpSurfaceSmaller);
	//////////////////////////////////////////////////////////////////////////
	//reade began to render
	//g_pd3dDevice->BeginScene();

	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
		0x00000000, 1.0f, 0 );

	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_NONE );
	float K = 2;
	D3DXVECTOR2 T1( K,K);
	D3DXVECTOR2 T2(-K,K);

	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

	DWORD Color = m_lpCurScene->m_Glow.m_GateColor;
	g_cTextureTable.SetTexture(0,m_BufferTextureID);
	g_pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SUBTRACT);
	g_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);

	/*g_cTextureTable.SetTexture(1,m_BufferTextureID);
	g_pd3dDevice->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_MODULATE2X);
	g_pd3dDevice->SetTextureStageState(1,D3DTSS_COLORARG1,D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState(1,D3DTSS_COLORARG2,D3DTA_CURRENT);*/

	g_cGraphicsTool.DrawScreenRectNormal(&A,&C,0.0f,Color,Color,Color,Color);

	//g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	//g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
	//g_cGraphicsTool.DrawScreenRect(&A,&C,0.0f,Color,m_SmallerTextureID);
	//g_cGraphicsTool.DrawScreenRect(&(A-T1),&(C+T1),0.0f,Color,m_SmallerTextureID);

	//g_cGraphicsTool.DrawScreenRect(&A,&C,0.0f,Color,m_BufferTextureID);
	/*g_cGraphicsTool.DrawScreenRect(&(A-T1),&(C-T1),0.0f,Color,m_BufferTextureID);
	g_cGraphicsTool.DrawScreenRect(&(A+T1),&(C+T1),0.0f,Color,m_BufferTextureID);
	g_cGraphicsTool.DrawScreenRect(&(A-T2),&(C-T2),0.0f,Color,m_BufferTextureID);
	g_cGraphicsTool.DrawScreenRect(&(A+T2),&(C+T2),0.0f,Color,m_BufferTextureID);*/

	if(1)
	{
		//////////////////////////////////////////////////////////////////////////
		Color = 0x30FFFFFF;
		for(int i=0;i<8;i++)
		{
			g_pd3dDevice->SetViewport(&ViewPortBlur);
			g_pd3dDevice->SetRenderTarget(0,lpSurfaceBlur);

			//if(i==0)
			{
				g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
					0x00000000, 1.0f, 0 );
			}

			g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
			g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

			g_cGraphicsTool.DrawScreenRect(&A1,&C1,0.0f,0xFFFFFFFF,m_SmallerTextureID);

			g_pd3dDevice->SetViewport(&ViewPort);
			g_pd3dDevice->SetRenderTarget(0,lpSurfaceSmaller);

			g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
			g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_BLENDFACTOR);
			g_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR,0x60606060);

			g_cGraphicsTool.DrawScreenRect(&(A-T1),&(C+T1),0.0f,Color,m_BlurTextureID);
		}
	}
	else
	{


		//g_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR,0x60606060);

		for(int i=0;i<10;i++)
		{
			g_pd3dDevice->SetViewport(&ViewPortBlur);
			g_pd3dDevice->SetRenderTarget(0,lpSurfaceBlur);

			g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
				0x00000000, 1.0f, 0 );

			g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
			g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

			g_cGraphicsTool.DrawScreenRect(&A1,&C1,0.0f,0xFFFFFFFF,m_SmallerTextureID);

			g_pd3dDevice->SetViewport(&ViewPort);
			g_pd3dDevice->SetRenderTarget(0,lpSurfaceSmaller);

			g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
			g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);

			Color = 0x00FFFFFF|(255-i*25)<<24;
			float K = 5;
			T1 = D3DXVECTOR2(K*i,K*i);
			T2 = D3DXVECTOR2(-K*i,K*i);

			g_cGraphicsTool.DrawScreenRect(&(A-T2),&(C-T2),0.0f,Color,m_BlurTextureID);
			/*g_cGraphicsTool.DrawScreenRect(&(A+T1),&(C+T1),0.0f,Color,m_BlurTextureID);
			g_cGraphicsTool.DrawScreenRect(&(A-T2),&(C-T2),0.0f,Color,m_BlurTextureID);
			g_cGraphicsTool.DrawScreenRect(&(A+T2),&(C+T2),0.0f,Color,m_BlurTextureID);*/
		}

		/*g_pd3dDevice->SetViewport(&ViewPortBlur);
		g_pd3dDevice->SetRenderTarget(0,lpSurfaceBlur);
		g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
			0x00000000, 1.0f, 0 );
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
		g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

		g_cGraphicsTool.DrawScreenRect(&A1,&C1,0.0f,0xFFFFFFFF,m_SmallerTextureID);
		g_pd3dDevice->SetViewport(&ViewPort);
		g_pd3dDevice->SetRenderTarget(0,lpSurfaceSmaller);

		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
		g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_BLENDFACTOR);
		g_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR,0x60606060);*/
		
		Color = 0x30FFFFFF;
		g_cGraphicsTool.DrawScreenRect(&(A),&(C),0.0f,Color,m_BlurTextureID);

	}
	


	//g_pd3dDevice->EndScene();
	//////////////////////////////////////////////////////////////////////////
	//Restore
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &Mat_View_Save );
	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &Mat_Proj_Save );
	g_pd3dDevice->SetRenderTarget(0,lpSurfaceSave);
	g_pd3dDevice->SetViewport(&ViewPortSave);

	g_cTextureTable.SetTexture(1,NULL);
	g_pd3dDevice->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
	g_pd3dDevice->SetTextureStageState(1,D3DTSS_COLORARG1,D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState(1,D3DTSS_COLORARG2,D3DTA_CURRENT);

	return S_OK;
}

HRESULT KGraphicsEngine::BackBufferCreate()
{
	HRESULT hr = S_OK;
	{
		LPTEXTURE pNewTexture = NULL;
		if (m_BufferTextureID==0)
		{
			if (FAILED(g_cTextureTable.Get1NewTexture(&pNewTexture,0)))
				return hr;
		}
		else
		{
			if (FAILED(g_cTextureTable.GetTexture(&pNewTexture,m_BufferTextureID)))
				return hr;
		}

		SAFE_RELEASE(pNewTexture->m_lpTexture);

		LPDIRECT3DTEXTURE9 pTexture = NULL;
		HRESULT hr = D3DXCreateTexture(g_pd3dDevice,m_DisplayMode.Width,
			m_DisplayMode.Height,
			1,
			D3DUSAGE_RENDERTARGET,
			D3DFMT_A8R8G8B8,
			D3DPOOL_DEFAULT,
			&pTexture);

		pNewTexture->m_lpTexture = pTexture;
		m_BufferTextureID = pNewTexture->ID;
		g_cGraphicsTool.m_BufferTextureID = m_BufferTextureID;
	}
	{
		LPTEXTURE pNewTexture = NULL;
		if (m_SmallerTextureID==0)
		{
			if (FAILED(g_cTextureTable.Get1NewTexture(&pNewTexture,0)))
				return hr;
		}
		else
		{
			if (FAILED(g_cTextureTable.GetTexture(&pNewTexture,m_SmallerTextureID)))
				return hr;
		}

		SAFE_RELEASE(pNewTexture->m_lpTexture);

		LPDIRECT3DTEXTURE9 pTexture = NULL;
		HRESULT hr = D3DXCreateTexture(g_pd3dDevice,512,
			512,
			1,
			D3DUSAGE_RENDERTARGET,
			D3DFMT_A8R8G8B8,
			D3DPOOL_DEFAULT,
			&pTexture);

		pNewTexture->m_lpTexture = pTexture;
		m_SmallerTextureID = pNewTexture->ID;
	}
	{
		LPTEXTURE pNewTexture = NULL;
		if (m_BlurTextureID==0)
		{
			if (FAILED(g_cTextureTable.Get1NewTexture(&pNewTexture,0)))
				return hr;
		}
		else
		{
			if (FAILED(g_cTextureTable.GetTexture(&pNewTexture,m_BlurTextureID)))
				return hr;
		}

		SAFE_RELEASE(pNewTexture->m_lpTexture);

		LPDIRECT3DTEXTURE9 pTexture = NULL;
		HRESULT hr = D3DXCreateTexture(g_pd3dDevice,256,
			256,
			1,
			D3DUSAGE_RENDERTARGET,
			D3DFMT_A8R8G8B8,
			D3DPOOL_DEFAULT,
			&pTexture);

		pNewTexture->m_lpTexture = pTexture;
		m_BlurTextureID = pNewTexture->ID;
	}
	return S_OK;
}

HRESULT KGraphicsEngine::BackBufferRelease()
{
	HRESULT hr = S_OK;

	if (m_BufferTextureID!=0)
	{
		LPTEXTURE pNewTexture = NULL;
		if (SUCCEEDED(g_cTextureTable.GetTexture(&pNewTexture,m_BufferTextureID)))
		{
			SAFE_RELEASE(pNewTexture->m_lpTexture);
		}
	}
	if (m_SmallerTextureID!=0)
	{
		LPTEXTURE pNewTexture = NULL;
		if (SUCCEEDED(g_cTextureTable.GetTexture(&pNewTexture,m_SmallerTextureID)))
		{
			SAFE_RELEASE(pNewTexture->m_lpTexture);
		}
	}
	if (m_BlurTextureID!=0)
	{
		LPTEXTURE pNewTexture = NULL;
		if (SUCCEEDED(g_cTextureTable.GetTexture(&pNewTexture,m_BlurTextureID)))
		{
			SAFE_RELEASE(pNewTexture->m_lpTexture);
		}
	}
	return S_OK;
}
HRESULT KGraphicsEngine::EnableMotionBlur(BOOL Value)
{
	if(Value)
	{
		if(!m_bUseMotionBlur)
		{
			BackBufferCreate();
			m_bUseMotionBlur = TRUE;
		}
	}
	else
	{
		if(m_bUseMotionBlur)
		{
			BackBufferRelease();
		}
		m_bUseMotionBlur = FALSE;
	}
	return S_OK;
}

HRESULT KGraphicsEngine::Reset()
{
	m_lpCurScene = NULL;
	g_hRenderWnd = NULL;

	return S_OK;
}
