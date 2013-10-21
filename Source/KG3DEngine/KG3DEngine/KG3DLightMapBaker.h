#pragma once

#include "KG3DRenderTargetTool.h"
class KG3DScenePvsEditor;
class KG3DModelPointLight;
class KG3DModel;
class KG3DScene;
struct KG3DShadowMapLevel;
class KG3DPointLightShadow;

class KG3DLightMapBaker
{
public:
	struct EnvLight
	{
		D3DXVECTOR3 vPosition;
		LPDIRECT3DCUBETEXTURE9 pCubeMapColor;
		LPDIRECT3DCUBETEXTURE9 pCubeMapDepth;
		AABBOX m_BBox;
		float  m_Radius;

		HRESULT Init(float R,D3DXVECTOR3 vPos,DWORD dwSize);

		EnvLight()
		{
			pCubeMapColor = NULL;
			pCubeMapDepth = NULL;
			m_Radius = 100;
		}
		
		~EnvLight()
		{
			SAFE_RELEASE(pCubeMapColor);
			SAFE_RELEASE(pCubeMapDepth);
		}
	};//º‰Ω”π‚’’

	vector<EnvLight*>m_vecEnvLight;

	struct Photon
	{
		D3DXVECTOR3   vPosition;
		D3DXVECTOR3   vDirection;
		D3DCOLOR      vColor;
		float         fDistance;
	};

	struct PhotonRender
	{
		D3DXVECTOR3 vPosition;
		D3DXVECTOR3 vDirection;
		D3DCOLORVALUE vColor;
	};

	int     m_nComputedModel;
	int     m_nNumPhotonTotal;
	int     m_nComputedPhoton;
	Photon* m_lpPhotons;

	BOOL    m_bShowDebug;

	vector<KG3DModelPointLight*>m_vecPointLight;
	vector<KG3DModel*>          m_vecModel;
	list<KG3DModel*>m_listModelForBake;
	list<KG3DModel*>m_listModelForBakeEnv;

	KG3DScenePvsEditor*  m_lpPvsScene;
	KG3DRenderTargetTool m_vRenderTargetTool;
	KG3DRenderTargetTool m_vRenderTargetToolDepth;
	KG3DRenderTargetTool m_vRenderTargetToolEnvColor;
	KG3DRenderTargetTool m_vRenderTargetToolEnvDepth;
	KG3DPointLightShadow* m_lpPointLightTool;
	KG3DPointLightShadow* m_lpEnvLightTool;

	LPDIRECT3DVOLUMETEXTURE9 m_lpJitterTexture;

	int m_nBakedPhoton;
	
	KG3DShadowMapLevel* m_lpShadowMapForDiectionLight;

	vector<EnvLight*>m_vecEnvLightRefresh;

	BOOL m_bNeedEnableLightmap2;
	BOOL m_bNeedDisableLightmap2;

	BOOL m_bComputeDirectLight;
	BOOL m_bComputeIndirectLight;

public:
	HRESULT ProcessDirectLighting();
	HRESULT ProcessIndirectLighting();


	HRESULT ComputeCubeMapForEnvLight(EnvLight* pEnv);

	HRESULT InitEvnLighting();
	HRESULT UninitEvnLighting();

	HRESULT PrepareModelForBake(KG3DModel* pModel,BOOL bComputeDirectLight,BOOL bComputeIndirectLight);

	HRESULT ShowDebug();

	KG3DLightMapBaker(void);
	~KG3DLightMapBaker(void);

	BOOL RayIntersect(Photon* pPhoton,float* pfDistance,D3DXVECTOR3* pFaceNormal);

	HRESULT InitFromPVS(KG3DScenePvsEditor* pScene);

	HRESULT Render();
	HRESULT FrameMove();
	int ComputeLightMapSize(KG3DModel* pModel);

	HRESULT BakeLightMapForModel(KG3DModel* pModel,BOOL bClear,BOOL bEnvLight);
	HRESULT RenderPhotonsLightMapForModel(KG3DModel* pModel);
	HRESULT RenderDirectionLightMapForModel(KG3DModel* pModel);
	HRESULT ComputeCubeMapForPointLight();
	HRESULT ComputeCubeMapForPointLight(KG3DModelPointLight* pLight);

	HRESULT RenderPointLightMapForModel(KG3DModel* pModel);
	HRESULT RenderEnvLightMapForModel(KG3DModel* pModel);

	HRESULT ComputeOnePhoton();

};
