#pragma once

class KG3DModel;
class KG3DSelector
{
public:
	KG3DSelector();
	~KG3DSelector();

	HRESULT Init();
	HRESULT OnRender();
	void UnInit();
	void OnLostDevice();
	void OnResetDevice();
	void Push(KG3DModel *pModel);
	void ResetSelection();
	KG3DModel* GetSelected();
	POINT m_MousePoint;
private:
	void RenderModelList();
	LPDIRECT3DTEXTURE9 m_pStencilRT;//绘制模型用的模板
	LPDIRECT3DSURFACE9 m_pStencilDepth;//绘制模型用的模板深度
	LPDIRECT3DSURFACE9 m_pRT;//大小为1厘米的鼠标位置模板颜色
	LPDIRECT3DTEXTURE9 m_pResult;//用来取出颜色的Surface
	LPDIRECT3DSURFACE9 m_pDepthSave;//保存的Depth
	LPDIRECT3DSURFACE9 m_pRTSave;//保存的RT
	D3DVIEWPORT9       m_vpSave;//保存的VP
	int m_nWidth;//Stencil Target的宽度
	int m_nHeight;//Stencil Target的高度
	int m_nScale;//Stencil Target的缩放系数
	DWORD m_dwIndex;//渲染Stencil时使用的index
	std::vector<KG3DModel*> m_vecModels;//需要做精确判断的模型列表
	BOOL m_bSelected;
	KG3DModel *m_pSelectedModel;
};

extern KG3DSelector g_Selector;
