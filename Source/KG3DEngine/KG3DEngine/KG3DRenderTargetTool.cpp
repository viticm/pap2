#include "stdafx.h"
#include "KG3DRenderTargetTool.h"
#include "CommonDefine.h"
#include "KG3DStructs.h"
#include "KG3DGraphicsTool.h"
#include "KG3DSceneOutputWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL g_bScaleOutputSmooth;

KG3DRenderTargetTool::KG3DRenderTargetTool()
{
//	m_lpScreenQuads = NULL;
	m_lpSurfaceDep = NULL;
	m_lpSurfaceColor = NULL;
	m_lpOldTarget = NULL;
	m_lpOldDepth = NULL;
	m_lpSurfaceSystem = NULL;
	m_nWidth = 0;
	m_lpTargetTexture = NULL;
}

KG3DRenderTargetTool::~KG3DRenderTargetTool()
{
	UnInit();
}

HRESULT KG3DRenderTargetTool::Init(int nSize,BOOL bNeedTargetTexture,D3DFORMAT colorfmt)
{
	HRESULT hr = E_FAIL;

    ASSERT(m_lpSurfaceDep == NULL);
	ASSERT(m_lpSurfaceColor == NULL);

	hr = g_pd3dDevice->CreateDepthStencilSurface(nSize,nSize,
		D3DFMT_D24S8,D3DMULTISAMPLE_NONE,0,
		TRUE,
		&m_lpSurfaceDep,NULL);
	KGLOG_COM_PROCESS_ERROR(hr);

	if(bNeedTargetTexture)
	{
		hr = g_pd3dDevice->CreateTexture(nSize,nSize,1,D3DUSAGE_RENDERTARGET,colorfmt,D3DPOOL_DEFAULT,&m_lpTargetTexture,NULL);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = m_lpTargetTexture->GetSurfaceLevel(0,&m_lpSurfaceColor);
		KGLOG_COM_PROCESS_ERROR(hr);
	}
	else
	{
		hr = g_pd3dDevice->CreateRenderTarget(nSize,nSize,
			colorfmt,D3DMULTISAMPLE_NONE,0,
			FALSE,
			&m_lpSurfaceColor,NULL);
		KGLOG_COM_PROCESS_ERROR(hr);
	}

	hr = g_pd3dDevice->CreateOffscreenPlainSurface(
		nSize,
		nSize,
		colorfmt,
		D3DPOOL_SYSTEMMEM,
		&m_lpSurfaceSystem,
		NULL
		);
	KGLOG_COM_PROCESS_ERROR(hr);

	m_nWidth = nSize;

	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DRenderTargetTool::UnInit()
{
	SAFE_RELEASE(m_lpSurfaceDep);
	SAFE_RELEASE(m_lpSurfaceColor);
	SAFE_RELEASE(m_lpSurfaceSystem);
	SAFE_RELEASE(m_lpTargetTexture);
	return S_OK;
}
//如果参数指针为NULL，则表示不改变当前相关的渲染状态
HRESULT KG3DRenderTargetTool::BeginNewRenderTarget(D3DXMATRIXA16 *NewWorldMatrix,
												D3DXMATRIXA16 *NewViewMatrix, 
												D3DXMATRIXA16 *NewProjectMatrix)									
{
	g_pd3dDevice->GetRenderTarget(0,&m_lpOldTarget);
	g_pd3dDevice->GetDepthStencilSurface(&m_lpOldDepth);

	D3DVIEWPORT9 NewViewPort;
	D3DSURFACE_DESC Desc;

	m_lpSurfaceColor->GetDesc(&Desc);
	NewViewPort.X = 0;NewViewPort.Y = 0;
	NewViewPort.Width = Desc.Width;
	NewViewPort.Height = Desc.Height;
	NewViewPort.MinZ = 0;NewViewPort.MaxZ = 1.0f;

	g_pd3dDevice->GetTransform(D3DTS_WORLD,&m_SaveWorldMatrix);
	g_pd3dDevice->GetTransform(D3DTS_VIEW,&m_SaveViewMatrix);
	g_pd3dDevice->GetTransform(D3DTS_PROJECTION,&m_SaveProjectMatrix);
	g_pd3dDevice->GetViewport(&m_SaveViewPort);

	if(NewWorldMatrix)
		g_pd3dDevice->SetTransform(D3DTS_WORLD,NewWorldMatrix);
	if(NewViewMatrix)
		g_pd3dDevice->SetTransform(D3DTS_VIEW,NewViewMatrix);
	if(NewProjectMatrix)
		g_pd3dDevice->SetTransform(D3DTS_PROJECTION,NewProjectMatrix);

	g_pd3dDevice->SetViewport(&NewViewPort);


	g_pd3dDevice->SetDepthStencilSurface(m_lpSurfaceDep);
	g_pd3dDevice->SetRenderTarget(0,m_lpSurfaceColor);


	return S_OK;
}

HRESULT KG3DRenderTargetTool::EndNewRenderTarget(LPDIRECT3DSURFACE9 SaveRenderTarget, 
											  LPDIRECT3DSURFACE9 SaveDepthBuffer)
{
	g_pd3dDevice->SetRenderTarget(0,SaveRenderTarget);
	g_pd3dDevice->SetDepthStencilSurface(SaveDepthBuffer);

	g_pd3dDevice->SetTransform(D3DTS_WORLD,&m_SaveWorldMatrix);
	g_pd3dDevice->SetTransform(D3DTS_VIEW,&m_SaveViewMatrix);
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION,&m_SaveProjectMatrix);
	g_pd3dDevice->SetViewport(&m_SaveViewPort);

	return S_OK;
}

HRESULT KG3DRenderTargetTool::EndNewRenderTarget()
{
	g_pd3dDevice->SetRenderTarget(0,m_lpOldTarget);
	g_pd3dDevice->SetDepthStencilSurface(m_lpOldDepth);

	g_pd3dDevice->SetTransform(D3DTS_WORLD,&m_SaveWorldMatrix);
	g_pd3dDevice->SetTransform(D3DTS_VIEW,&m_SaveViewMatrix);
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION,&m_SaveProjectMatrix);
	g_pd3dDevice->SetViewport(&m_SaveViewPort);

	if(m_lpOldTarget)
		m_lpOldTarget->Release();
	if(m_lpOldDepth)
		m_lpOldDepth->Release();
	return S_OK;
}

DWORD GetPointFromTexture(int nX,int nY,int nWidth,DWORD* pColors)
{
	nX = max(0,nX);
	nY = max(0,nY);
	nX = min(nWidth-1,nX);
	nY = min(nWidth-1,nY);

	return pColors[nY * nWidth + nX];
}

HRESULT ExtendBorder(int nWidth,DWORD* pColorsDest,DWORD* pColorsSrc)
{
	DWORD dwColor[8];

	for (int nY=0;nY<nWidth;nY++)
	{
		for (int nX=0;nX<nWidth;nX++)
		{
			DWORD dwColorSrc = GetPointFromTexture(nX,nY,nWidth,pColorsSrc);

			if(dwColorSrc == 0)
			{
				dwColor[0] = GetPointFromTexture(nX-1,nY  ,nWidth,pColorsSrc);
				dwColor[1] = GetPointFromTexture(nX+1,nY  ,nWidth,pColorsSrc);
				dwColor[2] = GetPointFromTexture(nX,nY-1  ,nWidth,pColorsSrc);
				dwColor[3] = GetPointFromTexture(nX,nY+1  ,nWidth,pColorsSrc);
				dwColor[4] = GetPointFromTexture(nX-1,nY-1,nWidth,pColorsSrc);
				dwColor[5] = GetPointFromTexture(nX+1,nY-1,nWidth,pColorsSrc);
				dwColor[6] = GetPointFromTexture(nX-1,nY+1,nWidth,pColorsSrc);
				dwColor[7] = GetPointFromTexture(nX-1,nY+1,nWidth,pColorsSrc);

				DWORD a = max(dwColor[0],dwColor[1]);
				DWORD b = max(dwColor[1],dwColor[2]);
				DWORD c = max(dwColor[3],dwColor[4]);
				DWORD d = max(dwColor[5],dwColor[6]);
				DWORD e = max(a,b);
				DWORD f = max(c,d);

				dwColorSrc = max(e,f);
			}

			pColorsDest[nY * nWidth + nX] = dwColorSrc;
		}
	}

	return S_OK;
}


HRESULT ExtendBorderNoAlpha(int nWidth,DWORD* pColorsDest,DWORD* pColorsSrc)
{
	DWORD dwColor[8];

	for (int nY=0;nY<nWidth;nY++)
	{
		for (int nX=0;nX<nWidth;nX++)
		{
			DWORD dwColorSrc = GetPointFromTexture(nX,nY,nWidth,pColorsSrc);

			if(dwColorSrc == 0)
			{
				dwColor[0] = GetPointFromTexture(nX-1,nY  ,nWidth,pColorsSrc);
				dwColor[1] = GetPointFromTexture(nX+1,nY  ,nWidth,pColorsSrc);
				dwColor[2] = GetPointFromTexture(nX,nY-1  ,nWidth,pColorsSrc);
				dwColor[3] = GetPointFromTexture(nX,nY+1  ,nWidth,pColorsSrc);
				dwColor[4] = GetPointFromTexture(nX-1,nY-1,nWidth,pColorsSrc);
				dwColor[5] = GetPointFromTexture(nX+1,nY-1,nWidth,pColorsSrc);
				dwColor[6] = GetPointFromTexture(nX-1,nY+1,nWidth,pColorsSrc);
				dwColor[7] = GetPointFromTexture(nX-1,nY+1,nWidth,pColorsSrc);

				DWORD a = max(dwColor[0],dwColor[1]);
				DWORD b = max(dwColor[1],dwColor[2]);
				DWORD c = max(dwColor[3],dwColor[4]);
				DWORD d = max(dwColor[5],dwColor[6]);
				DWORD e = max(a,b);
				DWORD f = max(c,d);

				dwColorSrc = max(e,f);
			}

			pColorsDest[nY * nWidth + nX] = (pColorsDest[nY * nWidth + nX] & 0xFF000000) | (0x00FFFFFF &dwColorSrc);
		}
	}

	return S_OK;
}

HRESULT KG3DRenderTargetTool::ExportRenderData(LPDIRECT3DTEXTURE9 pTexture,BOOL bExtend)
{
	HRESULT hr = E_FAIL;
	LPDIRECT3DSURFACE9 pDest = NULL;
	KGLOG_PROCESS_ERROR(pTexture);
	//////////////////////////////////////////////////////////////////////////

	hr = pTexture->GetSurfaceLevel(0,&pDest);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = ExportRenderData(pDest,bExtend);
	KGLOG_COM_PROCESS_ERROR(hr);

	//////////////////////////////////////////////////////////////////////////
	hr = S_OK;
Exit0:
	SAFE_RELEASE(pDest);
	return hr;
}


HRESULT KG3DRenderTargetTool::ExportRenderData(LPDIRECT3DSURFACE9 pDest,BOOL bExtend)
{
	HRESULT hr = E_FAIL;
	D3DLOCKED_RECT rect_src;
	D3DLOCKED_RECT rect_dest;
	D3DSURFACE_DESC desc;
	KGLOG_PROCESS_ERROR(pDest);

	hr = g_pd3dDevice->GetRenderTargetData(m_lpSurfaceColor,m_lpSurfaceSystem);
	KGLOG_COM_PROCESS_ERROR(hr);
	//////////////////////////////////////////////////////////////////////////
	pDest->GetDesc(&desc);

	hr = pDest->LockRect(&rect_dest,NULL,0);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = m_lpSurfaceSystem->LockRect(&rect_src,NULL,0);
	KGLOG_COM_PROCESS_ERROR(hr);

	if(bExtend)
	{

		DWORD* pColorsDest = (DWORD*)rect_dest.pBits;
		DWORD* pColorsSrc = (DWORD*)rect_src.pBits;

		ExtendBorder(desc.Width,pColorsDest,pColorsSrc );
		ExtendBorderNoAlpha(desc.Width,pColorsSrc ,pColorsDest);
		ExtendBorderNoAlpha(desc.Width,pColorsDest,pColorsSrc );
	}
	else
	{
		memcpy(rect_dest.pBits,rect_src.pBits,desc.Height * rect_src.Pitch);
	}	


	hr = pDest->UnlockRect();
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = m_lpSurfaceSystem->UnlockRect();
	KGLOG_COM_PROCESS_ERROR(hr);
	//////////////////////////////////////////////////////////////////////////
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DRenderTargetTool::ScaleOutput(KG3DSceneOutputWnd* pWnd)
{
	g_pd3dDevice->BeginScene();

	D3DXVECTOR2 A(0-0.5F,-0.5F);
	D3DXVECTOR2 C((float)pWnd->m_Viewport.Width,(float)pWnd->m_Viewport.Height);
	
	float u = 1.0F/m_nWidth;
	float v = 1.0F/m_nWidth;

	D3DXVECTOR4 vUVoffset1(u, 0,0,v);
	D3DXVECTOR4 vUVoffset2(u,v,u,-v);

	GraphicsStruct::SamplerState sample0(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
	GraphicsStruct::SamplerState sample1(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);

	if(g_bScaleOutputSmooth)
	{
		LPD3DXEFFECT pShader = g_cGraphicsTool.GetDefaultShader(DEFST_SCALEOUTPUT);
		UINT uPass = 0;

		pShader->SetTechnique("tec0");
		pShader->SetVector("vUVoffset1",&vUVoffset1);
		pShader->SetVector("vUVoffset2",&vUVoffset2);


		pShader->Begin(&uPass,0);
		pShader->BeginPass(0);

		g_cGraphicsTool.DrawScreenRect(&A,&C,0,0xFFFFFFFF,m_lpTargetTexture);

		pShader->EndPass();
		pShader->End();

	}
	else
	{

		g_cGraphicsTool.DrawScreenRect(&A,&C,0,0xFFFFFFFF,m_lpTargetTexture);
	}


	g_pd3dDevice->EndScene();


	return S_OK;
}
