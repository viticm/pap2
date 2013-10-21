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
#include  "stdafx.h"
#include  "KG3DUITest.h"

#ifndef SAFE_DELETE
	#define SAFE_DELETE( POINTER )	if ( NULL != POINTER )	{	delete POINTER; POINTER = NULL;	}
#endif



//////////////////////////////////////////////////////////////////////////
//KG3DEngine Interface;
#include "KG3DInterface.h"
#include "KG3DUITexture.h"
#include "KG3DMemManager.h"




//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
extern LPDIRECT3D9             g_pD3D; // Used to create the D3DDevice
extern LPDIRECT3DDEVICE9		g_pd3dDevice; // Our rendering device

IKG3DEngineManager*		g_pEngineManager = NULL;
KG3DUITexture*			g_pd3dUITexture = NULL;
extern KG3DMemManager	g_cEngineMemStack;


#define D3DFVF_CUSTOMVERTEX ( D3DFVF_XYZRHW | D3DFVF_TEX1 )
struct QuadVertex
{
	float x, y, z,rhw;
	float tu, tv;
};

QuadVertex g_quadVertices[144*6];


HWND g_hWnd = NULL;


//-----------------------------------------------------------------------------
// Name: InitD3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D( HWND hWnd )
{
	int nOutputWindowID;

	g_pD3D = Direct3DCreate9( D3D_SDK_VERSION );

	D3DDISPLAYMODE d3ddm;

	g_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm );

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof(d3dpp) );

	d3dpp.Windowed               = TRUE;
	d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat       = d3ddm.Format;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;

	g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice );


	g_cEngineMemStack.Initialize( KG3DMEM_LEQUAL_32M );
	g_hWnd = hWnd;


	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitGeometry()
// Desc: Creates the scene geometry
//-----------------------------------------------------------------------------
HRESULT InitGeometry()
{
	g_pd3dUITexture = new KG3DUITexture( );
	g_pd3dUITexture->Initialize( g_pd3dDevice );
	
	FILE* fp = fopen( "dump.txt", "wb" );

	for ( int i = 0; i < 144; ++ i )
	{
		char szTemp[MAX_PATH] = {0};
		char szFolder[MAX_PATH] = {0};
		DWORD ImageId = 0;

		::GetCurrentDirectory( MAX_PATH, szFolder );
		snprintf( szTemp, MAX_PATH, "%s/Image/skills_%d.tga", szFolder, i );
		g_pd3dUITexture->UploadSliceTexture( szTemp, &ImageId );
		fprintf( fp, "%s\t%d\r\n", szTemp, ImageId );
	}


	fprintf( fp, "\r\nMemory Usage:\t%d\r\n", g_pd3dUITexture->GetMemoryUsage() );
	fprintf( fp, "\r\nTexture Usage:\t%d\r\n", g_pd3dUITexture->GetTextureUsage() );
	fprintf( fp, "\r\nSlice Usage:\t%d\r\n", g_pd3dUITexture->GetSliceUsage() );


	fclose( fp );


	// load vertex buffer
#define  KG3D_TLVERTEX( vertex, nx, ny, nu, nv )	\
	vertex->x = nx;			\
	vertex->y = ny;			\
	vertex->z = 0.0f;		\
	vertex->rhw = 1.0f;		\
	vertex->tu = nu;			\
	vertex->tv = nv;

	QuadVertex* pQuadVertices = g_quadVertices;
	DWORD ImageId = 0;
	INT ImageNums = 0;

	char szTemp[MAX_PATH] = {0};
	char szFolder[MAX_PATH] = {0};

	::GetCurrentDirectory( MAX_PATH, szFolder );
	for ( int i = 0; i < 12; ++ i )
	{
		for ( int j = 0; j < 12; ++ j )
		{
			KG3D_TLVERTEX( ( pQuadVertices + 0 ), j * 40, i * 40, 0.0f, 0.0f );
			KG3D_TLVERTEX( ( pQuadVertices + 1 ), j * 40 + 40, i * 40, 1.0f,0.0f );
			KG3D_TLVERTEX( ( pQuadVertices + 2 ), j * 40, i * 40 + 40, 0.0f,1.0f );
			KG3D_TLVERTEX( ( pQuadVertices + 3 ), j * 40 + 40, i * 40 + 40, 1.0f,1.0f );


			snprintf( szTemp, MAX_PATH, "%s/Image/skills_%d.tga", szFolder, i * 12 + j );
			if ( g_pd3dUITexture->UploadSliceTexture( szTemp, &ImageId ) )
			{
			}

			if ( timeGetTime() % 2 == 0 )
			{
				g_pd3dUITexture->EvictTexture( ImageId );
			}

			pQuadVertices += 4;
		}
	}


	// restore all
	for ( int i = 0; i < 144; ++ i )
	{
		snprintf( szTemp, MAX_PATH, "%s/Image/skills_%d.tga", szFolder, i );
		if ( g_pd3dUITexture->UploadSliceTexture( szTemp, &ImageId ) )
		{

		}

	}


	// load texture sampler state
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE );
	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID );

	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE );
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------
VOID Cleanup()
{
	if( g_pd3dUITexture != NULL )
	{
		g_pd3dUITexture->CleanUp();
		delete g_pd3dUITexture;
	}

	if( g_pd3dDevice != NULL )
		g_pd3dDevice->Release();

	if( g_pD3D != NULL )
		g_pD3D->Release();


	g_pd3dDevice = NULL;
	g_pD3D = NULL;
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
	  g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,255,0), 1.0f, 0 );


	  // Begin the scene
	  if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	  {
	      // setup the world, view, and projection matrices
	      SetupMatrices();


		  
		  // set texture
		  char szTemp[MAX_PATH] = {0};
		  char szFolder[MAX_PATH] = {0};
		  DWORD ImageId = 0;
		  DWORD ImageFirst = 0;
		  QuadVertex* pQuadVertices = g_quadVertices;
		  LPDIRECT3DTEXTURE9 pTexture = NULL;


		  ::GetCurrentDirectory( MAX_PATH, szFolder );
		  for ( int i = 1; i < 144; ++ i )
		  {
			  snprintf( szTemp, MAX_PATH, "%s/Image/skills_%d.tga", szFolder, i );
			  if ( g_pd3dUITexture->UploadSliceTexture( szTemp, &ImageId ) )
			  {
				  LPDIRECT3DTEXTURE9 pTexture = g_pd3dUITexture->GetTextureInterface( ImageId );
				  SliceUpload_t  Info;

				  g_pd3dUITexture->GetTextureUpload( ImageId, Info );
				  pQuadVertices[0].tu = Info.left;	pQuadVertices[0].tv = Info.top;
				  pQuadVertices[1].tu = Info.left + Info.width;	pQuadVertices[1].tv = Info.top;
				  pQuadVertices[2].tu = Info.left;	pQuadVertices[2].tv = Info.top + Info.height;
				  pQuadVertices[3].tu = Info.left + Info.width;	pQuadVertices[3].tv = Info.top + Info.height;

				  pTexture = g_pd3dUITexture->GetTextureInterface( ImageId );
				  g_pd3dDevice->SetTexture( 0, pTexture );

			  }
			

			  g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
			  g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, pQuadVertices, sizeof( QuadVertex ) );
			  pQuadVertices += 4;
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

	if(g_pEngineManager)
		g_pEngineManager->ProcessMessage(msg,wParam,lParam);


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
		WS_OVERLAPPEDWINDOW, 10, 10, 800, 700,
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
				{
					Render();
				}

			}
		}
	}

	UnregisterClass( "D3D Tutorial", wc.hInstance );
	return 0;
}



