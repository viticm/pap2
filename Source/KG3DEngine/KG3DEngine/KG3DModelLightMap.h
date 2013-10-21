#pragma once


class KG3DTexture;

class KG3DModelLightMap
{
public:
	KG3DTexture* m_lpLightMapTexture;//光照图
	BOOL         m_bUseShareTexture;//是否使用共享的光照图
	D3DXVECTOR4  m_vUVScale;//共享光照图的UV缩放系数

	LPDIRECT3DTEXTURE9 m_lpLightMapForCompute;//用于计算的光照图
	LPDIRECT3DTEXTURE9 m_lpLightMapForCompute2;//用于计算的光照图

	BOOL         m_bComputing;//是否处于计算模式
	int          m_nLightmapWidth;//光照图宽度 
	BOOL         m_bUseSecond;

	int          m_nKey;//光照图编号
public:
	HRESULT EnableUseLightmap2(BOOL bEnable,float ratioA,float ratioB);
	HRESULT Init(int nWidth,BOOL bComputing,LPSTR pFileName);
	HRESULT UnInit();

	LPDIRECT3DTEXTURE9 GetLightMap();

	KG3DModelLightMap(void);
	~KG3DModelLightMap(void);
};
