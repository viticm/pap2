#ifndef _MATHTOOLS_H_
#define _MATHTOOLS_H_

#include <d3d9.h>
#include <d3dx9.h>

#define EPSILON (1.0e-5)

int triBoxOverlap(float boxcenter[3],float boxhalfsize[3],float triverts[3][3]);

BOOL IntersectSectionLine2Triangle(
    D3DXVECTOR3& vA, D3DXVECTOR3& vB, D3DXVECTOR3& vC,
    D3DXVECTOR3& vM, D3DXVECTOR3& vN,
    D3DXVECTOR3* pOut
);

BOOL IntersectSectionBox2Box(
    D3DXVECTOR3& vBoxAPoint1, D3DXVECTOR3& vBoxAPoint2,
    D3DXVECTOR3& vBoxBPoint1, D3DXVECTOR3& vBoxBPoint2
);

void UpdateNormal(
    KVertexEx* pVertex, int nVertexCount, 
    KTriangleEx* pTriangle, int nTriangleCount
);

BOOL IsFloatZero(float fValue);

int FloatComp(float fA, float fB);

#endif  // _MATHTOOLS_H_
