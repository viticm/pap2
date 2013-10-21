#include"..\..\Render\Render.h"
#include <tlhelp32.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

LPDIRECT3DVERTEXBUFFER9 g_pVertexBuffer=NULL;

struct CUSTOMVERTEX
{
	FLOAT x, y, z;
	DWORD color;
};

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)
//#define SafeRelease(pObject) {if((pObject)!=NULL) {(pObject)->Release(); (pObject)=NULL;}}

HRESULT InitializeD3D(HWND hWnd)
{
	/*
	g_GetRenderCore()->m_pD3D=Direct3DCreate9(D3D_SDK_VERSION);
	if(g_GetRenderCore()->m_pD3D==NULL) return E_FAIL;

	//Get the currnet display mode
	D3DDISPLAYMODE d3ddm;
	if(FAILED(g_GetRenderCore()->GetD3D()->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm))) return E_FAIL;

	//Create a structure to hold the settings for our device
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));

	//Fill the structure
	//We want our program to be windowed and set the back buffer to a format that matches the current display mode

	d3dpp.Windowed=TRUE;
	d3dpp.SwapEffect=D3DSWAPEFFECT_COPY;
	d3dpp.BackBufferWidth = 800;
	d3dpp.BackBufferHeight = 600;
	d3dpp.BackBufferFormat=d3ddm.Format;  //D3DFMT_A8R8G8B8

	//Create a Direct3D device
	if(FAILED(g_GetRenderCore()->GetD3D()->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_GetRenderCore()->m_pD3DDevice)))
		return E_FAIL;
		*/

	//Create a structure to hold the settings for our device
	Render::RenderCoreDesc rcdesc;
	ZeroMemory(&rcdesc, sizeof(rcdesc));

	//Fill the structure
	//We want our program to be windowed and set the back buffer to a format that matches the current display mode

	rcdesc.m_hWnd = hWnd;
	rcdesc.m_PresentParameters.BackBufferWidth = 800;
	rcdesc.m_PresentParameters.BackBufferHeight = 600;
	rcdesc.m_PresentParameters.Windowed = TRUE;
	rcdesc.m_PresentParameters.BackBufferFormat = D3DFMT_A8R8G8B8;

	g_GetRenderCore()->Initiate(rcdesc);


	//Hide the back
	g_GetRenderCore()->GetD3DDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	//Turn off the light
	g_GetRenderCore()->GetD3DDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);

	return S_OK;
}

HRESULT InitializeVertexBuffer()
{
	//Store each point of the triangle together with its color
	CUSTOMVERTEX cvVertices[] =
	{
		{-10.f, 0.f, 0.f, D3DCOLOR_XRGB(255, 0, 0)},
		{0.f, 0.f, 10.f, D3DCOLOR_XRGB(0, 255, 0)},
		{10.f, 0.f, 0.f, D3DCOLOR_XRGB(0, 0, 255)}
	};

	//Create the vertex buffer
	if(FAILED(g_GetRenderCore()->GetD3DDevice()->CreateVertexBuffer(sizeof(cvVertices), 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVertexBuffer, 0)))
		return E_FAIL;

	//Get a pointer to the vertex buffer vertices and lock the vertex buffer
	void* pVertices = 0;
	if(FAILED(g_pVertexBuffer->Lock(0, sizeof(cvVertices), &pVertices, 0)))
		return E_FAIL;

	//Copy our stored vertices value into the vertex buffer
	memcpy(pVertices, cvVertices, sizeof(cvVertices));

	g_pVertexBuffer->Unlock();

	return S_OK;
}

void SetupRotation()
{
	D3DXMATRIX matWorld, matWorldX, matWorldY, matWorldZ;

	D3DXMatrixRotationX(&matWorldX, timeGetTime()/400.0f);
	D3DXMatrixRotationY(&matWorldY, timeGetTime()/400.0f);
	D3DXMatrixRotationZ(&matWorldZ, timeGetTime()/400.0f);

	D3DXMatrixMultiply(&matWorld, &matWorldX, &matWorldY);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matWorldZ);

	g_GetRenderCore()->GetD3DDevice()->SetTransform(D3DTS_WORLD, &matWorld);
}

void SetupCamera()
{
	//The camera has 3 settings: "Camera Position", "Look at Position", "Up Direction"
	D3DXMATRIX matView;
	D3DXMatrixLookAtLH(&matView,
		&D3DXVECTOR3(0.f, 10.f, -5.f),  //Camera Position
		&D3DXVECTOR3(0.f, 0.f, 5.f),  //Look At Position
		&D3DXVECTOR3(0.f, 1.f, 0.f)  //Up Direction
		);
	g_GetRenderCore()->GetD3DDevice()->SetTransform(D3DTS_VIEW, &matView);
}

void SetupPerspective()
{
	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI/4, 1.0f, 1.0f, 10000.0f);
	g_GetRenderCore()->GetD3DDevice()->SetTransform(D3DTS_PROJECTION, &matProj);
}

void DoRender()
{
	if(g_GetRenderCore()->GetD3DDevice()==NULL) return;

	//Clear the back buffer to a black color
	g_GetRenderCore()->GetD3DDevice()->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	//Begin the scene
	g_GetRenderCore()->GetD3DDevice()->BeginScene();

	SetupRotation();
	SetupCamera();
	SetupPerspective();

	//Rendering triangles
	int triCount=0;
	g_GetRenderCore()->GetD3DDevice()->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(CUSTOMVERTEX));
	g_GetRenderCore()->GetD3DDevice()->SetFVF(D3DFVF_CUSTOMVERTEX);
	triCount+=g_GetRenderCore()->GetD3DDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

	char msg[256];
	sprintf(msg, "每桢渲染的三角形数：%d", triCount);
	//g_pFont->DrawText(msg, 5, 5, D3DCOLOR_XRGB(0, 255, 0));

	//End the scene
	g_GetRenderCore()->GetD3DDevice()->EndScene();

	//Flip
	g_GetRenderCore()->GetD3DDevice()->Present(NULL, NULL, NULL, NULL);
}

void CleanUp()
{
	SafeRelease(g_pVertexBuffer);
}

void GameLoop()
{
	MSG msg;
	BOOL fMessage;

	PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);
	while(msg.message!=WM_QUIT)
	{
		fMessage=PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE);
		if(fMessage)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			DoRender();
		}
	}
}

LRESULT WINAPI WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_KEYUP:
		switch(wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			return 0;
		}
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
	WNDCLASSEX wc={sizeof(WNDCLASSEX), CS_CLASSDC, WinProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, "DX Project 1", NULL};
	RegisterClassEx(&wc);
	HWND hWnd=CreateWindow("DX Project 1", "挺进3D", WS_OVERLAPPEDWINDOW, 50, 50, 500, 500, GetDesktopWindow(), NULL, wc.hInstance, NULL);

	if(hWnd==NULL) return FALSE;

	if(SUCCEEDED(InitializeD3D(hWnd)))
	{
		ShowWindow(hWnd, SW_SHOWDEFAULT);
		UpdateWindow(hWnd);
		if(SUCCEEDED(InitializeVertexBuffer()))
		{
			//g_pFont=new CFont(g_pD3DDevice, "宋体", 12, true, false, false);

			GameLoop();

			//delete g_pFont;
		}
	}

	CleanUp();

	UnregisterClass("DX Project 1", wc.hInstance);

	return 0;
}