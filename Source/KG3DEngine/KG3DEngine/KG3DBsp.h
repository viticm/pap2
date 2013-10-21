#pragma once

struct KG3DIntersectionFilter;
class KG3DModel;

typedef bool (*find_face_callback)(const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2, const D3DXVECTOR3& boxHalfSize, void* param);
typedef D3DXVECTOR3 TriData[3];

class KG3DBsp
{
public:
	struct Triangle
	{
		DWORD A,B,C;
		D3DXVECTOR3 Normal;
		DWORD dwUserData;

	};

	struct BspNodeSimple
	{
		int       nNumTriangles;
		int       nTriangleStart;
		D3DXPLANE plane;

		int  nSubNodeID[2];

        void Render(KG3DBsp* pBsp, int nLevel);
	};

	struct BspNodeEx 
	{
		vector<DWORD>vecTriangles;
		D3DXPLANE    plane;
		BspNodeEx*   pSubNode[2];
	};

	struct PlaneInfo
	{
		int       nRef;
		DWORD     dwTriangleIndex;
		D3DXPLANE MinPlane;
		int       nUp;
		int       nDown;
		int       Key;
	};

	
    void Render(KG3DModel* pModel);

    HRESULT GetFacesIntersectBox(const D3DXMATRIX& matBoxSpace, const D3DXVECTOR3& hs, find_face_callback ffc, void* param);
    DWORD GetFacesIntersectBox(BspNodeSimple* pNode, TriData* tris, const D3DXVECTOR3& boxHalfSize, const D3DXMATRIX& m);

    HRESULT GetFacesIntersectSphere(const D3DXVECTOR3& vCenter, float fRadius);
    DWORD GetFacesIntersectSphere(BspNodeSimple* pNode, DWORD* pTris, const D3DXVECTOR3& vCenter, float fRadius);
    bool IsTriangeSphereIntersect(const KG3DBsp::Triangle& tri, const D3DXVECTOR3& vCenter, float fRadius);




	HRESULT UnInit();

	HRESULT CreateFromMesh(DWORD dwNumVertex,DWORD dwNumFace,D3DXVECTOR3* pVertex,DWORD* pFaceIndex);
	
	BOOL GetRayIntersect(DWORD& dwCount,D3DXVECTOR3* pInterPoint,DWORD dwMaxNum,D3DXVECTOR3 Origin,D3DXVECTOR3 Direction);
	BOOL RayIntersection(D3DXVECTOR3& vSrc, D3DXVECTOR3& vNormalizedDir, FLOAT* pfRet,D3DXVECTOR3* pFaceNormal
		, KG3DIntersectionFilter* pFilter /*= NULL*/);

    HRESULT LoadFromMemory(IKG_Buffer *piFileData);
    HRESULT GetMeshInfo(DWORD *pdwRetVertexCount, DWORD *pdwRetFaceCount);
    HRESULT SaveToFile(LPSTR pFileName);

	KG3DBsp(void);
	~KG3DBsp(void);
private:
	BOOL RayIntersectionNode(BspNodeSimple* pNode,D3DXVECTOR3& vSrc, D3DXVECTOR3& vNormalizedDir,D3DXVECTOR3& vEnd,
		KG3DIntersectionFilter* pFilter,float* pDistance,D3DXVECTOR3* pFaceNormal,DWORD& dwHitCount,DWORD dwMaxHit,DWORD& dwTestCount);

	BOOL GetRayIntersectNode(BspNodeSimple* pNode,
		DWORD& dwCount,D3DXVECTOR3* pInterPoint,DWORD dwMaxNum,
		D3DXVECTOR3& vOrigin,D3DXVECTOR3& vDirection,D3DXVECTOR3& vEnd);

	HRESULT ConvertToSimpleFormat();
	HRESULT ConvertToSimpleFormatNode(BspNodeSimple* pDest,BspNodeEx* pSrc,DWORD& dwCurrentNode,DWORD& dwCurrentTriangle,Triangle* pNewTriangle);

	BspNodeEx* RequesetNodeEx();
	BspNodeSimple*   RequesetNode(DWORD& dwCurrentNode);
	
	HRESULT InputTranglesToNode(BspNodeEx* pNode,vector<DWORD>vecTriangles);

	BOOL PlaneRelateVertex(D3DXPLANE* pPlane,D3DXVECTOR3* pVertex);
	int PlaneRelateTriangle(D3DXPLANE* pPlane,Triangle* pTriangle);

	HRESULT CreateBspTree(vector<DWORD>vecTriangles,BspNodeEx** ppNode);
	int IsPlaneFit(D3DXPLANE* pPlane,vector<DWORD>*pvecTriangles,PlaneInfo* pInfo);

private:
    struct _FILE_HEAD
    {
        DWORD dwFileMask;
        DWORD dwVersion;

        DWORD dwNumVertex;
        DWORD dwTriangles;
        DWORD dwNumNodes;
        float fRadius;
    } *m_pHead;

    D3DXVECTOR3   *m_lpVertexs;
    Triangle      *m_lpTriangles;
    BspNodeSimple *m_lpNodes;
    BspNodeSimple *m_lpRootNode;
    BspNodeEx     *m_lpRootNodeEx;


    vector<BspNodeEx*>m_vecRefBspNode;

private:
    IKG_Buffer *m_piFileData;
};


/*
  for debug 
 */

class KG3DBspRender
{
public :

    void Push(KG3DModel* pModel)
    {
        if (pModel && find(m_listModel.begin(), m_listModel.end(), pModel) == m_listModel.end())
            m_listModel.push_back(pModel);
    }

    void Remove(KG3DModel* pModel)
    {
        if (pModel)
            m_listModel.remove(pModel);
    }

    void Clear()
    {
        m_listModel.clear();
    }

    void Render();

    list<KG3DModel*> m_listModel;
};