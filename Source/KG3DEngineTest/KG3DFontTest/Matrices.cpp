//-----------------------------------------------------------------------------
// File: Matrices.cpp
//
// Desc: Now that we know how to create a device and render some 2D vertices,
//       this tutorial goes the next step and renders 3D geometry. To deal with
//       3D geometry we need to introduce the use of 4x4 matrices to transform
//       the geometry with translations, rotations, scaling, and setting up our
//       camera.
//
//       Geometry is defined in model space. We can move it (translation),
//       rotate it (rotation), or stretch it (scaling) using a world transform.
//       The geometry is then said to be in world space. Next, we need to
//       position the camera, or eye point, somewhere to look at the geometry.
//       Another transform, via the view matrix, is used, to position and
//       rotate our view. With the geometry then in view space, our last
//       transform is the projection transform, which "projects" the 3D scene
//       into our 2D viewport.
//
//       Note that in this tutorial, we are introducing the use of D3DX, which
//       is a set of helper utilities for D3D. In this case, we are using some
//       of D3DX's useful matrix initialization functions. To use D3DX, simply
//       include <d3dx9.h> and link with d3dx9.lib.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <Windows.h>
#include <mmsystem.h>
#include <d3dx9.h>

#include "KG3DFontBase.h"
#include "KG3DFontTexture.h"
#include "KG3DFontRenderer.h"


int  const  FONT_NUMS	 = 9;
char const* FONT_FILES[] = { 
	"Font\\经典楷体简.ttf", "Font\\simsun.ttc",	"Font\\simhei.ttf",	"Font\\simkai.ttf",	"Font\\SimSun18030.ttc",
	"Font\\经典隶书简.ttf",	"Font\\经典圆体简.ttf",	
	"Font\\汉仪方隶简.ttf",	"Font\\经典粗黑简.ttf"	 };

char const* FONT_STR     = { "应用程序发生异常 192.128.0.5 unknown 0xc0000094" };


//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
LPDIRECT3D9             g_pD3D       = NULL; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; // Our rendering device

KG3DFontRenderer*		g_pFontRenderer = NULL;
KG3DFontBase*			g_pFontPtr[ FONT_NUMS ] = { NULL };


//-----------------------------------------------------------------------------
// Name: InitD3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D( HWND hWnd )
{
    // Create the D3D object.
    if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return E_FAIL;

    // Set up the structure used to create the D3DDevice
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

    // Create the D3DDevice
    if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pd3dDevice ) ) )
    {
        return E_FAIL;
    }

    // Turn off culling, so we see the front and back of the triangle
    g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

    // Turn off D3D lighting, since we are providing our own vertex colors
    g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitGeometry()
// Desc: Creates the scene geometry
//-----------------------------------------------------------------------------
HRESULT InitGeometry()
{
	g_pFontRenderer = new KG3DFontRenderer();
	g_pFontRenderer->Initialize( g_pd3dDevice );

	for ( int i = 0; i < FONT_NUMS; ++ i )
	{
		g_pFontPtr[i] = new KG3DFontTexture( g_pFontRenderer );
		g_pFontPtr[i]->Initialize( FONT_FILES[i], 20, 0, 96 );
	}


	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------
VOID Cleanup()
{
	for ( int i = 0; i < FONT_NUMS; ++ i )
	{
		g_pFontPtr[i]->CleanUp();
		delete g_pFontPtr[i];
	}

	g_pFontRenderer->CleanUp();
	delete g_pFontRenderer;


    if( g_pd3dDevice != NULL )
        g_pd3dDevice->Release();

    if( g_pD3D != NULL )
        g_pD3D->Release();
}



//-----------------------------------------------------------------------------
// Name: SetupMatrices()
// Desc: Sets up the world, view, and projection transform matrices.
//-----------------------------------------------------------------------------
VOID SetupMatrices()
{
    // For our world matrix, we will just rotate the object about the y-axis.
    D3DXMATRIXA16 matWorld;

    // Set up the rotation matrix to generate 1 full rotation (2*PI radians) 
    // every 1000 ms. To avoid the loss of precision inherent in very high 
    // floating point numbers, the system time is modulated by the rotation 
    // period before conversion to a radian angle.
    UINT  iTime  = timeGetTime() % 1000;
    FLOAT fAngle = iTime * (2.0f * D3DX_PI) / 1000.0f;
    D3DXMatrixRotationY( &matWorld, fAngle );
    g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    // Set up our view matrix. A view matrix can be defined given an eye point,
    // a point to lookat, and a direction for which way is up. Here, we set the
    // eye five units back along the z-axis and up three units, look at the
    // origin, and define "up" to be in the y-direction.
    D3DXVECTOR3 vEyePt( 0.0f, 3.0f,-5.0f );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMATRIXA16 matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    // For the projection matrix, we set up a perspective transform (which
    // transforms geometry from 3D view space to 2D viewport space, with
    // a perspective divide making objects smaller in the distance). To build
    // a perpsective transform, we need the field of view (1/4 pi is common),
    // the aspect ratio, and the near and far clipping planes (which define at
    // what distances geometry should be no longer be rendered).
    D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 100.0f );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
}



//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID Render()
{
    // Clear the backbuffer to a black color
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );

    // Begin the scene
    if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
    {
        // Setup the world, view, and projection matrices
        SetupMatrices();


        // Render the vertex buffer contents
		g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

	
		if ( g_pFontRenderer->Prepare() )	// 准备字体渲染
		{
			for ( int i = 0; i < FONT_NUMS; ++ i )
			{
				g_pFontPtr[i]->SetTextAlpha( 255 );
				g_pFontPtr[i]->SetTextColour( D3DCOLOR_RGBA( 255, 255, 0, 255 ) );
				g_pFontPtr[i]->SetTextScaling( 1.0f, 1.0f );

				g_pFontPtr[i]->DrawText( 0.0f, i * 60, FONT_FILES[i], strlen( FONT_FILES[i] ) );
				g_pFontPtr[i]->DrawText( 10.0f, i * 60 + 20, FONT_STR, strlen( FONT_STR ) );
				//g_pFontPtr[i]->DrawText( i * 60, 0.0f, FONT_FILES[i], strlen( FONT_FILES[i] ) );
				//g_pFontPtr[i]->DrawText( i * 60 + 20, 10.0f, FONT_STR, strlen( FONT_STR ) );
			}


			g_pFontRenderer->Flush( );	// 渲染字体到屏幕或者渲染目标
		}



        // End the scene
        g_pd3dDevice->EndScene();
    }

    // Present the backbuffer contents to the display
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}




//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_DESTROY:
            Cleanup();
            PostQuitMessage( 0 );
            return 0;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}




//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    // Register the window class
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
                      hInst, NULL, NULL, NULL, NULL,
                      "D3D Tutorial", NULL };
    RegisterClassEx( &wc );

    // Create the application's window
    HWND hWnd = CreateWindow( "D3D Tutorial", "D3D Tutorial 03: Matrices",
                              WS_OVERLAPPEDWINDOW, 10, 10, 900, 610,
                              NULL, NULL, wc.hInstance, NULL );

    // Initialize Direct3D
    if( SUCCEEDED( InitD3D( hWnd ) ) )
    {
        // Create the scene geometry
        if( SUCCEEDED( InitGeometry() ) )
        {
            // Show the window
            ShowWindow( hWnd, SW_SHOWDEFAULT );
            UpdateWindow( hWnd );

            // Enter the message loop
            MSG msg;
            ZeroMemory( &msg, sizeof(msg) );
            while( msg.message!=WM_QUIT )
            {
                if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
                {
                    TranslateMessage( &msg );
                    DispatchMessage( &msg );
                }
                else
                    Render();
            }
        }
    }

    UnregisterClass( "D3D Tutorial", wc.hInstance );
    return 0;
}



