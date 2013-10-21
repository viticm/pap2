#pragma once
#include "KG3DShaderHolder.h"
interface IEKG3DModel;
class KG3DScenePointLightRender;

class KG3DStaticModelRender
{
public:
	typedef set<KG3DModel*> ModelSet;

	struct ModelContainer
	{
		map<DWORD, ModelSet >m_mapModelSet;

		KG3DShaderHolder m_vShader;

		HRESULT Init(BOOL bPlayer);
		HRESULT UnInit();
 
		HRESULT RenderModelGroup(ModelSet& models,LPD3DXEFFECT pEffect,BOOL bUpdateMatrix,LPDIRECT3DVERTEXDECLARATION9 pDeclaration);
		HRESULT FrameMove(KG3DScenePointLightRender* pLights);
		HRESULT Render(KG3DStaticModelRender* pRender,BOOL bPlayer);
		HRESULT RenderForDepth(KG3DStaticModelRender* pRender);

		HRESULT AddModel(KG3DModel* pModel);

		DWORD GetNumModel();
		HRESULT SetShaderParam(BOOL bPlayer);
		HRESULT GetAllModels(vector<IEKG3DModel*>* pvecModels);

		BOOL m_bRenderPlayer;
	};

	ModelContainer m_vModelContainerScene;
	ModelContainer m_vModelContainerPlayer;


	LPDIRECT3DVERTEXDECLARATION9 m_lpVertexDeclaration;
	LPDIRECT3DVERTEXDECLARATION9 m_lpVertexDeclarationNormal;

	HRESULT Init();
	HRESULT UnInit();

	HRESULT FrameMove(KG3DScenePointLightRender* pLights);
	HRESULT Render();

	HRESULT AddModel(KG3DModel* pModel);



	HRESULT RenderForDepth();
	DWORD GetNumModel();
	HRESULT GetAllModels(vector<IEKG3DModel*>* pvecModels);

	KG3DStaticModelRender(void);
	~KG3DStaticModelRender(void);
};
