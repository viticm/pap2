#pragma once

#include "KG3DTypes.h"
#include "KG3DShaderHolder.h"
#include "KG3DMaterial.h"

class KG3DModel;
class KG3DSkinModelRender;
interface IEKG3DModel;

class KG3DScenePointLightRender;

class KG3DSkinCPURender
{
private:
	ULONG m_ulRefCount;
	
	struct VertexFormat 
	{ 
		D3DXVECTOR3 p;
		D3DXVECTOR3 Normal;
		float tu1, tv1,tIndex;
	};

public:
	enum MODELTYPE
	{
		MODELTYPE_NONE,
		MODELTYPE_SKIN,
		MODELTYPE_STATIC,
		MODELTYPE_MODELST,
	}m_eModelType;
	
	BOOL   m_bSkinUpdate;//最后一次SKIN计算完成时间
	DWORD  m_dwLastFillTime;//最后一次填充的完成时间

	LPDIRECT3DVERTEXBUFFER9 m_lpVB1;
	LPDIRECT3DINDEXBUFFER9  m_lpIB;

	VertexFormat* m_lpVertexSrc;//顶点的数据源
	VertexFormat* m_lpBuffer1;//
	D3DXVECTOR3*  m_lpPositionLast;

	DWORD m_dwNumFace;
	DWORD m_dwNumVertex;

	KG3DModel* m_lpModel;

	KG3DSkinCPURender();
	~KG3DSkinCPURender();

	HRESULT Init();
	HRESULT UnInit();

	HRESULT FillVertexBuffer(VertexFormat* pSrc);
	HRESULT CreateFromModel(KG3DModel* pModel,BOOL bSKin);
	HRESULT ComputeSkin(VertexFormat* pDest);
	HRESULT ComputeSkin();

	HRESULT Render(LPD3DXEFFECT pEffect,KG3DSkinModelRender* pModelRender,BOOL bUpdate);

	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();

	vector<KG3DSkinCPURender*>m_vecSubRender;

};



class KG3DSkinModelRender
{
private:

    #define MAXSKINMODEL  1024

	ULONG m_ulRefCount;

	struct ModelContaner
	{
		KG3DShaderHolder m_vShader;
		map<KG3DModel*,KG3DSkinCPURender*>m_mapCPURender;
	};

	ModelContaner m_vStand;
	ModelContaner m_vDetail;

	HRESULT PutRenderToArray(KG3DSkinCPURender* pRender);
	HRESULT RemoveRenderFromArray(KG3DSkinCPURender* pRender);


public:

	HRESULT OnLostDevice();
	HRESULT OnResetDevice();

	LPDIRECT3DVERTEXDECLARATION9    m_lpVertexDeclaration;
	BOOL m_bDataChange;
	BOOL m_bExitMultiThread;//退出多线程

	KG3DSkinCPURender*     m_azSkinModelRender[MAXSKINMODEL];
	DWORD                    m_dwArrayUse;

	HRESULT Init();
	HRESULT UnInit();

	HRESULT AddModel(KG3DModel* pModel);

	KG3DSkinModelRender(void);
	~KG3DSkinModelRender(void);

	HRESULT FrameMove(KG3DScenePointLightRender* pLights);
	HRESULT Render(BOOL bUpdate);
	HRESULT RenderForDepth();

	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();

	HRESULT GetAllModels(vector<IEKG3DModel*>* pvecModels);
	HRESULT AutoReleadModifiedResource();
};

struct ModelVBFillList
{
	KG3DSkinModelRender* m_lpSkinModelRender;
	int  m_nMultiThreadComputeCount;

	DWORD m_dwState;

	ModelVBFillList()
	{
		m_lpSkinModelRender = NULL;
		m_dwState = BLOCK_LIST_STATE_NODATA;
		m_nMultiThreadComputeCount = 0;
	}
	~ModelVBFillList()
	{
		SAFE_RELEASE(m_lpSkinModelRender);
		m_lpSkinModelRender = NULL;
		m_dwState = BLOCK_LIST_STATE_NODATA;
		m_nMultiThreadComputeCount = 0;
	}
};

class KG3DRenderState;
class KG3DSkinShaderRender
{
public:
    KG3DSkinShaderRender();
    ~KG3DSkinShaderRender();

    HRESULT Init();
    HRESULT UnInit();

    HRESULT Clear();

    HRESULT InputModel(KG3DModel *pModel);
    HRESULT Sort();

    HRESULT RenderForDepth();
    HRESULT Render(BOOL bJustNear);

    unsigned GetModelCount();

    struct SKIN_MODEL_INFO
    {
        DWORD dwMaterialID;
        DWORD dwSubMaterialIndex;
        KG3DModel *pModel;
    };
private:
    HRESULT _SetMaterial(KG3DMaterial::KG3DMaterialSubset *pSubMaterial);
    std::vector<SKIN_MODEL_INFO> m_SingleSubMeshSkinModels;
    std::vector<SKIN_MODEL_INFO> m_MultiSubMeshSkinModels;

};