#pragma once
#include "KG3DMaterial.h"

class KG3DModel;
class KG3DModelST;

class KG3DMeshSkin
{
	struct VBFormat
	{
		D3DXVECTOR3 vPosition;
		D3DXVECTOR4 vBlendWeight;
		D3DCOLOR    vBlendIndex;
		D3DXVECTOR3 vNormal;
		D3DXVECTOR2 vUV;
	};
	struct ModelInfo
	{
		int nNumVertex;
		int nVertexStart;
		int nNumFace;
		int nFaceStart;
		int nNumSubset;
		int nSubsetStart;
		int nNumBone;
		int nBoneStart;
		int nNumBoneMeshOnly;
		int nNumBoneBip;
		vector<int>vecBoneIndexMap;

		KG3DModel* pModel;
	};
	vector<ModelInfo>m_vecModelInfo;

	D3DXMATRIX** m_lppCurrentBoneMatrix;
	D3DXMATRIX** m_lppBoneMatrixBaseInverse;
	D3DXMATRIX*  m_lpBoneMatrixFinal;

	VBFormat*    m_lpVertexBufferSrc;
	VBFormat*    m_lpVertexBufferDest;

	vector<KG3DMaterial::KG3DMaterialSubset*>m_vecMaterialSubset;

public:
	LPD3DXMESH m_lpMesh;
	
	int m_nNumBone;
	int m_nNumSubset;
	int m_nNumVertex;

	int m_nUpdateTime;
	int m_nLastUpdateTime;
	BOOL m_bNeedUpdateVertex;
	BOOL m_bCreateCompleted;

	KG3DMeshSkin(void);
	~KG3DMeshSkin(void);

	HRESULT ComputeSkining();
	HRESULT MultiThreadProcess();

	HRESULT CreateSkinMesh(int nNumVertex,int nNumFace);
	HRESULT FillData();
	HRESULT FillDataWithModel(ModelInfo& vInfo,VBFormat* pVB,WORD* pIndex,DWORD* pAttrib);


	HRESULT CreateFromModelST(KG3DModelST* pModelST);
	HRESULT UnInit();
	HRESULT CreateBoneMatrix(KG3DModelST* pModelST);

	HRESULT RenderNoChange(KG3DModelST* pModelST,LPD3DXEFFECT pShader);

	HRESULT ComputeMatrix();

	HRESULT UpdateVertex();

	HRESULT FrameMove();

};
