#pragma once
#include "Rectangle.h"
#include <vector>
#include <string>
using namespace std;
class CD3DManager
{
public:
	CD3DManager();
	~CD3DManager();

	BOOL Init(HWND hRenderWnd);
	void UnInit();

	BOOL SetTextures(LPCTSTR lpstrFileNames);
	BOOL ExportToFile(INT nWidth, INT nHeight, LPCTSTR lpstrFileNames);
	void OnResize();
	void FrameMove();
	void Render();

private:
	IDirect3D9* m_pIDirect3D;
	IDirect3DDevice9* m_pIDirect3DDevice;

	LPDIRECT3DTEXTURE9      m_pFileTexture;
	LPDIRECT3DSURFACE9      m_pDSFileTexture;

	HWND m_hRenderWnd;
	CRectangle* m_prect;
	vector<string> m_files;
	bool CreateD3Device(HWND hWnd, bool bFullScreen);

	void doFrameMove(LPRECT lpWndRect);
	void doRender(LPRECT lpWndRect);
};