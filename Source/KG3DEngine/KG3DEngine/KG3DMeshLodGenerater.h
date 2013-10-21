#pragma once

class KG3DMesh;

class KG3DMeshLodGenerater
{
public:
	KG3DMeshLodGenerater(int NumBoneSrc,KG3DMesh* pMesh);
	~KG3DMeshLodGenerater(void);

	int m_nNumBoneSrc;
	KG3DMesh* m_lpMeshSrc;

	HRESULT AddBone(DWORD dwBoneIndex,DWORD dwVertexIndex,float fWeight);

	HRESULT FillVertexSys(DWORD dwVertex,DWORD dwFace,BYTE* pData,int nDataSize,int nPos,int nNormal,int nUV,DWORD* pSubset);

	HRESULT SortBonesByVertexData(DWORD dwVertex,DWORD dwFace,BYTE* pData,int nDataSize,int nBoneIndexOffset,int nBoneWeightOffset,
		int nPos,int nPos1,int nNormal,int nNormal1,int nUV,WORD* pIndex,DWORD* pSubset);

	struct VertexBoneWeight
	{
		DWORD dwNumBone;

		D3DXVECTOR3 vPosition;
		D3DXVECTOR3 vPosition1;
		D3DXVECTOR3 vNormal;
		D3DXVECTOR3 vNormal1;
		D3DXVECTOR2 vUV;

		D3DXVECTOR3 vPositionSrc;
		D3DXVECTOR3 vNormalSrc;

		DWORD dwBoneIndex[4];
		float fWeight[4];

		DWORD dwBoneIndexReal[4];

		VertexBoneWeight()
		{
			ZeroMemory(this,sizeof(VertexBoneWeight));
		}

		HRESULT AddBone(DWORD dwBoneIndex,float fWeight);
	};

	struct VertexBoneInfo
	{

		DWORD dwVertexIndex;
		float fWeight;
	};
	vector<VertexBoneWeight>m_vecVertexBoneLink;

	map< DWORD,vector<VertexBoneInfo> >m_mapVertexIndex;

	map< DWORD,vector<DWORD> >m_mapAttribute;

	struct BoneReplace
	{
		string szNameSrc;
		string szNameDest;
	};
	vector<BoneReplace>m_vecBoneReplace;
	map<DWORD,int>m_mapBoneRemap;

	LPD3DXMESH m_lpMeshSys;
	//////////////////////////////////////////////////////////////////////////
	HRESULT ReduceBones();
	HRESULT TryBoneReplace(DWORD dwBoneIndex,DWORD& dwDestBoneIndex);
	HRESULT SortBonesFromVertex(WORD* pIndex);

	HRESULT AddBoneReplaceRule(LPSTR pNameSrc,LPSTR pNameDest);

	HRESULT VertexProcess(VertexBoneWeight* pVertex,const D3DXMATRIX* matTemp[],DWORD* BoneMap);

	HRESULT SaveToMesh(LPSTR pName);

	HRESULT GenerateBoneRemap();

	int FindBoneNewIndex(DWORD dwOldIndex);

	HRESULT SavePrograssMeshAsLowMesh(LPD3DXPMESH pMeshProgress,LPSTR pName);
};
