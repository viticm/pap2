#pragma once

class KG3DModelPointLight;
class KG3DTerrainBlock;
class KG3DTerrainRoad;
struct KG3D_PointLight_Info;

class KG3DScenePointLightRender
{
protected:
	set<KG3DModelPointLight*>m_setPointLight;
	HRESULT FindRefrenceLightInRect(KG3D_PointLight_Info& info,D3DXVECTOR2& A,D3DXVECTOR2& C);
public:
	HRESULT FindRefrenceLightForTerrainBlock(KG3DTerrainBlock* pBlock,D3DXVECTOR2& A,D3DXVECTOR2& C);
	HRESULT FindRefrenceLightForModel(KG3DModel* pModel);
	HRESULT FindRefrenceLightForRoad(KG3DTerrainRoad* pRoad);

	HRESULT AddPointLight(KG3DModelPointLight* pLight);
	HRESULT FrameMove();
	HRESULT Render();

	HRESULT Init();
	HRESULT UnInit();

	KG3DScenePointLightRender(void);
	~KG3DScenePointLightRender(void);
};
