#pragma once

class KG3DModelST;
class KG3DModel;
interface IEKG3DModel;
class KG3DSceneModelEditorPlayerTools
{
private:
	struct Sphere
	{
		D3DXVECTOR3 vPos;
		D3DXVECTOR3 vNormal;
		float      fRadius;
		BOOL IsPositionIn(D3DXVECTOR3& vP);
		
	};

	vector<string>m_vecHead;
	vector<string>m_vecFace;
	vector<string>m_vecBody;
	vector<string>m_vecHand;
	vector<string>m_vecLeg;
	vector<string>m_vecBelt;
	vector<string>m_vecMDL;

	KG3DModelST* m_lpModelST;

	KG3DModel* m_lpModelHead;
	KG3DModel* m_lpModelFace;
	KG3DModel* m_lpModelBody;
	KG3DModel* m_lpModelHand;
	KG3DModel* m_lpModelLeg;
	KG3DModel* m_lpModelBelt;
	KG3DModel* m_lpModelStand;//标准顶点位置和法线的模型

	//VFormat::FACES_NORMAL_TEXTURE1* m_lpVertexStand;
	//DWORD m_dwNumVertexStand;

	string m_szModelBaseDir;

	//vector<VFormat::FACES_NORMAL_TEXTURE1>m_vecStandVertex;
	vector<Sphere>m_vecSplitSphere;

private:
	int FindNearestVertex(D3DXVECTOR3& vPos);

	//HRESULT LockStandModel();
	//HRESULT UnLockStandModel();

	HRESULT GetModelPartFileName(LPSTR pFileName,int nPart,int nIndex);
	HRESULT CreateSpheresFromModel(KG3DModel* pModel);

public:
	HRESULT WedVertexForModel(KG3DModel* pModel,
		                      VFormat::FACES_NORMAL_TEXTURE1* pVerStand,D3DXMATRIX* pmat);

	HRESULT SetCurrentModelPart(int nPart,int nIndex);

	HRESULT GetModelPartVector(int nIndex,vector<string>* pvec);

	HRESULT LoadPartFromIni(LPSTR pFileName);
	HRESULT LoadModifyModelST(LPSTR pFileName);


	vector<Sphere>m_vecFoundPos;//匹配的顶点

	HRESULT Render();

	HRESULT WedVertexForModel(KG3DModel* pModel,D3DXMATRIX* pmat,LPSTR szMeshName);
	HRESULT LoadStandModel(LPCSTR pFileName);

	KG3DSceneModelEditorPlayerTools(void);
	~KG3DSceneModelEditorPlayerTools(void);

public:
	HRESULT SetStandModel(KG3DModel* pModel);
	KG3DModel* GetStandModel();
	HRESULT WedModelVertex(KG3DModel* pModel,D3DXMATRIX* pmat);
};
