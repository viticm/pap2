#pragma once

#include <list>
#include <map>
#include <map>

enum
{
    VERTEX_FVF = D3DFVF_XYZ | D3DFVF_NORMAL
};

typedef std::map<std::pair<KVertexEx*, KVertexEx*>, int>    KEdgeMap;
typedef std::multimap<float, KVertexEx*>                    KCostMap;

struct KEntity 
{
    KVertexEx*      pVertex;
    int             nVertexCount;
    KTriangleEx*    pTriangle;
    int             nTriangleCount;
    BOOL            bDraw;
    D3DXVECTOR3     vMin;
    D3DXVECTOR3     vMax;
    KCostMap        CostMap;
};

typedef std::list<KEntity*>                                 KEntityList;

struct KTerrainRegion
{
    KVertexEx*      pVertex;
    int             nVertexCount;
    KTriangleEx*    pTriangle;
    int             nTriangleCount;
    KEntityList     EntityList; // 与本Region有交集的Entity列表
    KCostMap        CostMap;
};

class KScene
{
public:
    KScene(void);
    ~KScene(void);

    BOOL Setup();
    void Clear();

    BOOL Load(const char cszFileName[]);

    void UpdateViewArea(float fX, float fZ);

    void Draw(IDirect3DDevice9* piDevice);

    void GetMaxCoordinate(D3DXVECTOR3* pMax);
    BOOL GetHeight(D3DXVECTOR3& vPos, float* pfHeight);

    BOOL ComputeEdgeCollapseCostsAtVertex(KVertexEx* pVertex, int nVertexCount, KCostMap* pCostMap);
    KVertexEx* GetMinCostEdge(KCostMap* pCostMap);

    BOOL CheckCanCollapse(KVertexEx* pVertex);
    BOOL Collapse(KVertexEx* pVertex, KCostMap* pCostMap);

    void PR();

public:
    BOOL m_bDrawUpFaceOnly;
    BOOL m_bPR;
    BOOL m_bDrawSolid;
    BOOL m_bDrawWireframe;

private:
    BOOL LoadTerrain(int nVertexCountX, int nVertexCountZ, float fHeight[]);
    BOOL LoadEntity(int nVertexCount, int nTriangleCount, BYTE* pbyData, size_t uDataLen);

    BYTE Draw(
        IDirect3DDevice9* piDevice,
        KVertexEx* pVertex, int nVertexCount,
        KTriangleEx* pTriangle, int nTriangleCount,
        BOOL bSolid, 
        BOOL bWireframe
    );

    KTerrainRegion* GetTerrainRegion(float fX, float fZ);
    KTerrainRegion* GetTerrainRegion(int nX, int nZ);

    BOOL ComputeEdgeCollapseCosts(KVertexEx* pVertexU, KVertexEx* pVertexV);

public:
    KEntityList         m_EntityList;

private:
    int                 m_nRegionCountX;
    int                 m_nRegionCountZ;
    KTerrainRegion*     m_pRegionTable;
    KTerrainRegion*     m_pRegionAll;

    int                 m_nViewRegionX;
    int                 m_nViewRegionZ;
    KEdgeMap            m_EdgeMap;
};

extern KScene g_Scene;

