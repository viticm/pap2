#include "stdafx.h"
#include "Rectangle.h"

CRectangle::CRectangle(IDirect3DDevice9* pDevice) : m_textani(pDevice)
{
	pDevice->AddRef();
	m_pDevice = pDevice;
	m_pVertexBuffer = NULL;
	InitVertexBuffer();
	
	pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,  TRUE );
	pDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	//取得显示模式
	//D3DDISPLAYMODE d3dDisplayMode;
	//if (SUCCEEDED(pDevice->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&d3dDisplayMode)))
	{
		
	}
}

CRectangle::~CRectangle()
{
	m_textani.UnInit();
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pDevice);
}

//初始化顶点坐标
bool CRectangle::InitVertexBuffer()
{
	IDirect3DDevice9* pDevice = m_pDevice;
	CUSTOMVERTEX customvertex[]={
		{0.0f,0.0f,0.0f,1.0f,0.0f,0.0f},
		{100,0.0f,0.0f,1.0f,1.0f,0.0f},
		{0.0f,100,0.0f,1.0f,0.0f,1.0f},
		{100,100,0.0f,1.0f,1.0f,1.0f}
	};

	BYTE* pVertexData;
	//创建顶点缓冲区的指针
	if(FAILED(pDevice->CreateVertexBuffer(4*sizeof(CUSTOMVERTEX), 0, CUSTOMVERTEX_FVF,D3DPOOL_MANAGED, &m_pVertexBuffer,NULL)))
	{
		return false;
	}	
	//取得顶点缓冲区的指针
	if (FAILED(m_pVertexBuffer->Lock(0,0,(void**)&pVertexData,0)))
	{
		return false;
	}
	//写入顶点数据到顶点缓冲区
	memcpy(pVertexData,customvertex,sizeof(customvertex));
	//解锁
	m_pVertexBuffer->Unlock();

	return true;
}

bool CRectangle::ResetPos(LPRECT pRect)
{	
	if (!m_pVertexBuffer) return false;
	
	float Width = (float)(pRect->right - pRect->left);
	float Height = (float)(pRect->bottom - pRect->top);

	BYTE* pVertexData;
	
	CUSTOMVERTEX customvertex[]={
		{0,0,0.0f,1.0f,0.0f,0.0f},
		{Width,0,0.0f,1.0f,1.0f,0.0f},
		{0,Height,0.0f,1.0f,0.0f,1.0f},
		{Width,Height,0.0f,1.0f,1.0f,1.0f}
	};
	//取得顶点缓冲区的指针
	if (FAILED(m_pVertexBuffer->Lock(0,0,(void**)&pVertexData,0)))
	{
		return false;
	}
	memcpy(pVertexData,customvertex,sizeof(customvertex));

	//解锁
	m_pVertexBuffer->Unlock();
	return true;
}

void CRectangle::FrameMove(LPRECT pRect)
{
	ResetPos(pRect);
	m_textani.Update(1);
}

void CRectangle::Render()
{
	IDirect3DDevice9* pDevice = m_pDevice;
	if (pDevice)
	{
		pDevice->SetStreamSource(0,m_pVertexBuffer,0,sizeof(CUSTOMVERTEX));
		pDevice->SetFVF(CUSTOMVERTEX_FVF);
		pDevice->SetTexture(0,m_textani.GetCurrentTexture());
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2);	
	}
}