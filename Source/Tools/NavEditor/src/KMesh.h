#ifndef _KMESH_H_
#define _KMESH_H_

#include <vector>
#include "KApplication.h"

struct KEdge
{
    int m_nIndexA;
    int m_nIndexB;
};

typedef std::vector<KVertex>    KVertexVec;
typedef std::vector<KTriangle>  KTriangleVec;
typedef std::vector<KEdge>      KEdgeVec;


class KMesh
{
public:
    int             AddVertex(const D3DXVECTOR3* cpVertex);
    int             AddTriangle(const KTriangle* cpTriangle);

    KVertex*        GetVertex(int nIndex);
    KTriangle*      GetTriangle(int nIndex);
    KEdge*          GetEdge(int nIndex);

    int             CountVertex();
    int             CountTriangle();
    int             CountEdge();

private:
    KVertexVec      m_Vertexs;
    KTriangleVec    m_Faces;
    KEdgeVec        m_Edges;
};

#endif  // _KMESH_H_
