#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include "KG3DTextureAnimation.h"

#define CUSTOMVERTEX_FVF (D3DFVF_XYZRHW|D3DFVF_TEX1)  //定义可变顶点格式的宏
struct CUSTOMVERTEX //顶点结构体
{
	float x,y,z,rhw;  //顶点坐标
	float u,v;  //顶点的纹理坐标
};

class CRectangle
{
public:
	CRectangle(IDirect3DDevice9* pDevice);
	~CRectangle();

	void FrameMove(LPRECT pRect);
	void Render();
	KG3DTextureAnimation* GetTextureManager() {return &m_textani;}
private:
	bool InitVertexBuffer();
	bool ResetPos(LPRECT pRect);
	KG3DTextureAnimation m_textani;

	IDirect3DVertexBuffer9* m_pVertexBuffer;  //顶点缓冲区
	IDirect3DDevice9* m_pDevice;
};