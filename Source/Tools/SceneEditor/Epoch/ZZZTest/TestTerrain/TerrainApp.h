// File: TerrainApp.h
// Desc:

#ifndef TERRAINAPP_H
#define TERRAINAPP_H

#ifndef EPOCH_DEBUG
#define EPOCH_DEBUG
#endif

#include"..\..\Common\Common.h"
#include"..\..\Maths\Maths.h"
#include"..\..\Render\Render.h"
#include"..\..\Terrain\Terrain.h"
#include"Font.h"

#include<windows.h>

#define TEST_VERTEX_FVF (D3DFVF_XYZ | D3DFVF_DIFFUSE)

struct TEST_VERTEX
{
	float x, y, z;
	D3DCOLOR Diffuse;
};

class TerrainApp
{
private:
	Common::String m_sConfigFilename;  // 配置文件名
	
	Common::String m_sTitle;  // 窗口标题
	unsigned int m_Width;
	unsigned int m_Height;

	HWND m_hWnd;  // 窗口句柄
	HINSTANCE m_hInst;  // 进程实例
	DWORD m_dwWindowStyle;
	bool m_bActive;

	Maths::Camera m_Camera;

	Maths::Matrix m_mtxView;
	Maths::Matrix m_mtxProj;

	IDirect3DVertexBuffer9* m_pVB;
	IDirect3DTexture9* m_pTexture;
	D3DCOLOR m_BGColor;

	Font m_Font;

	bool m_bSolid;

public:
	TerrainApp(const Common::String& sConfigFilename, HINSTANCE hInst): m_sConfigFilename(sConfigFilename), m_hInst(hInst), m_pVB(0), m_pTexture(0), m_BGColor(D3DCOLOR_XRGB(0, 0, 0)) {}
	~TerrainApp() { Destroy(); }

	bool Init();
	void Destroy();

	void Run();
	bool PumpWindowMessages();

	bool IsActive() const { return m_bActive; }
	void SetActive(bool bActive) { m_bActive = bActive; }

private:
	bool InitWindow();

	static LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

#endif