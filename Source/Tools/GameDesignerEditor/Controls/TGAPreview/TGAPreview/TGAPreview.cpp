// TGAPreview.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "TGAPreview.h"
#include "D3DManager.h"

#include <d3d9.h>
#include <d3dx9.h>

#include <map>

using namespace std;

#ifdef _MANAGED
#pragma managed(push, off)
#endif

typedef map<HWND, CD3DManager> defmap;
defmap g_d3dmgr;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

// This is an example of an exported variable
//TGAPREVIEW_API int nTGAPreview=0;

// This is an example of an exported function.
TGAPREVIEW_API int fnTGAPreview(void)
{
	return 42;
}

//CD3DManager g_d3d;

BOOL Init(HWND hRenderWnd)
{
	if (g_d3dmgr.find(hRenderWnd) == g_d3dmgr.end())
	{
		//CD3DManager d3d;
		g_d3dmgr[hRenderWnd] = CD3DManager();
		CD3DManager& d3d = g_d3dmgr[hRenderWnd];
		return d3d.Init(hRenderWnd);
	}
	return false;
}

TGAPREVIEW_API void UnInit(HWND hRenderWnd)
{
	if (g_d3dmgr.find(hRenderWnd) != g_d3dmgr.end())
	{
		CD3DManager& d3d = g_d3dmgr[hRenderWnd];
		d3d.UnInit();
		g_d3dmgr.erase(hRenderWnd);
	}
	
	if (hRenderWnd == NULL)
	{
		g_d3dmgr.clear();
		//int* a = new int[1];
	}
}

//Filenames:以\r\n分隔的文件名列表
TGAPREVIEW_API BOOL SetFileNames(HWND hRenderWnd, char* Filenames, INT nWidth, INT nHeight)
{
	if (g_d3dmgr.find(hRenderWnd) != g_d3dmgr.end())
	{
		CD3DManager& d3d = g_d3dmgr[hRenderWnd];
		return d3d.SetTextures(Filenames);
	}
	return false;
}

TGAPREVIEW_API void OnResize(HWND hRenderWnd)
{
	if (g_d3dmgr.find(hRenderWnd) != g_d3dmgr.end())
	{
		CD3DManager& d3d = g_d3dmgr[hRenderWnd];
		d3d.OnResize();
	}
}

TGAPREVIEW_API void FrameMove(HWND hRenderWnd)
{
	if (g_d3dmgr.find(hRenderWnd) != g_d3dmgr.end())
	{
		CD3DManager& d3d = g_d3dmgr[hRenderWnd];
		d3d.FrameMove();
	}
}

TGAPREVIEW_API void Render(HWND hRenderWnd)
{
	if (g_d3dmgr.find(hRenderWnd) != g_d3dmgr.end())
	{
		CD3DManager& d3d = g_d3dmgr[hRenderWnd];
		d3d.Render();
	}
}

//Filenames:以\r\n分隔的文件名列表
TGAPREVIEW_API BOOL ExportToFile(HWND hRenderWnd, INT nWidth, INT nHeight, char* Filenames)
{
	if (g_d3dmgr.find(hRenderWnd) != g_d3dmgr.end())
	{
		CD3DManager& d3d = g_d3dmgr[hRenderWnd];
		return d3d.ExportToFile(nWidth, nHeight, Filenames);
	}
	return false;
}
