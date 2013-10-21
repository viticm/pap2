#pragma once

class KG3DSceneOutputWnd;

class KG3DRenderTargetTool
{
protected:
	HRESULT EndNewRenderTarget(LPDIRECT3DSURFACE9 SaveRenderTarget,
		LPDIRECT3DSURFACE9 SaveDepthBuffer);
public:

	D3DXMATRIX				m_SaveWorldMatrix,m_SaveViewMatrix,m_SaveProjectMatrix;
	D3DVIEWPORT9				m_SaveViewPort;
	LPDIRECT3DSURFACE9			m_lpSurfaceDep,m_lpSurfaceColor,m_lpOldTarget,m_lpOldDepth;
	LPDIRECT3DSURFACE9          m_lpSurfaceSystem;
	LPDIRECT3DTEXTURE9          m_lpTargetTexture;
	int m_nWidth;

	KG3DRenderTargetTool();
	~KG3DRenderTargetTool();

	HRESULT Init(int nSize,BOOL bNeedTargetTexture,D3DFORMAT colorfmt);
	HRESULT UnInit();

	HRESULT BeginNewRenderTarget(D3DXMATRIXA16 *NewWorldMatrix,
							 	 D3DXMATRIXA16 *NewViewMatrix,
								 D3DXMATRIXA16 *NewProjectMatrix);

	HRESULT EndNewRenderTarget();

	HRESULT ExportRenderData(LPDIRECT3DTEXTURE9 pTexture,BOOL bExtend);
	HRESULT ExportRenderData(LPDIRECT3DSURFACE9 pDest,BOOL bExtend);

	HRESULT ScaleOutput(KG3DSceneOutputWnd* pWnd);
};