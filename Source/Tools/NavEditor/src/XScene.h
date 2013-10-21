#pragma once

#include <vector>

typedef struct 
{
    std::vector<int> FaceTable;
} KRegion, *PRegion;

#define DEFAULT_REGION_SIZE 2.0f
#define MAX_REGION_COUNT_X  800
#define MAX_REGION_COUNT_Y  400
#define MAX_REGION_COUNT_Z  800
#define VIEW_REGION_COUNT   5

struct XScene
{
public:
    XScene();

    void     SetRegionSize(float fSize);

    BOOL     LoadMeshData(
        const KVertex* pVertex, int nVertexCount,
        const KTriangle* pTriangle, int nTriangleCount
    );

    void     BuildRegionTriangle();
    void     GetMaxCoordinate(D3DXVECTOR3* pOut) { *pOut = m_vMaxCoordinate; }

    KRegion* GetRegion(int nX, int nY, int nZ, BOOL bAutoCreate = false);
    KRegion* GetRegion(float fX, float fY, float fZ, BOOL bAutoCreate = false);

    // 标记可视点,返回可视三角形数
    int      UpdateViewRegions(float fX, float fY, float fZ);
    BOOL     GetViewVertexData(void* pvBuffer, size_t uBufferSize);

    BOOL     Draw(IDirect3DDevice9* piDevice);

    BOOL     GetHeight(D3DXVECTOR3& vPos, float* pfOut);

    BOOL     GetMaxHeight(float fX, float fZ, float* pfHeight);

private:
    void     SetTriangleToRegions(int nTriangle);
    void     SetTriangleTo27Regions(int nTriangle, int nCenterX, int nCenterY, int nCenterZ);

    typedef std::vector<KVertex> KVertexTable;
    typedef std::vector<KTriangle> KTriangleTable;
    typedef std::vector<KRegion*> KRegionTable;

    KVertexTable    m_VertexTable;
    KTriangleTable  m_TriangleTable;

    float           m_fRegionSize;
    int             m_nRegionCountX;
    int             m_nRegionCountY;
    int             m_nRegionCountZ;
    KRegionTable    m_RegionTable;
    D3DXVECTOR3     m_vMaxCoordinate;

    KRegion*        m_pViewRegion;
    KVertexTable    m_ViewVertexTable;
};

extern XScene g_XS;