#include <math.h>
#define _WIN32_WINNT 0x0500 
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <Windows.h>
#include <Windowsx.h>
#include <zmouse.h>
#include <WinUser.h>
#include "KApplication.h"
#include "WinProc.h"
#include "KCamera.h"
#include "KMainPlayer.h"
#include "KScene.h"
#include "XScene.h"
#include "MathTools.h"

#define X 0
#define Y 1
#define Z 2

#define CROSS(dest,v1,v2) \
    dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
    dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
    dest[2]=v1[0]*v2[1]-v1[1]*v2[0]; 

#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])

#define SUB(dest,v1,v2) \
    dest[0]=v1[0]-v2[0]; \
    dest[1]=v1[1]-v2[1]; \
    dest[2]=v1[2]-v2[2]; 

#define FINDMINMAX(x0,x1,x2,min,max) \
    min = max = x0;   \
    if(x1<min) min=x1;\
    if(x1>max) max=x1;\
    if(x2<min) min=x2;\
    if(x2>max) max=x2;

int planeBoxOverlap(float normal[3],float d, float maxbox[3])
{
    int q;
    float vmin[3],vmax[3];
    for(q=X;q<=Z;q++)
    {
        if(normal[q]>0.0f)
        {
            vmin[q]=-maxbox[q];
            vmax[q]=maxbox[q];
        }
        else
        {
            vmin[q]=maxbox[q];
            vmax[q]=-maxbox[q];
        }
    }
    if(DOT(normal,vmin)+d>0.0f) return 0;
    if(DOT(normal,vmax)+d>=0.0f) return 1;

    return 0;
}


/*======================== X-tests ========================*/
#define AXISTEST_X01(a, b, fa, fb)			   \
    p0 = a*v0[Y] - b*v0[Z];			       	   \
    p2 = a*v2[Y] - b*v2[Z];			       	   \
    if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;} \
    rad = fa * boxhalfsize[Y] + fb * boxhalfsize[Z];   \
    if(min>rad || max<-rad) return 0;

#define AXISTEST_X2(a, b, fa, fb)			   \
    p0 = a*v0[Y] - b*v0[Z];			           \
    p1 = a*v1[Y] - b*v1[Z];			       	   \
    if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
    rad = fa * boxhalfsize[Y] + fb * boxhalfsize[Z];   \
    if(min>rad || max<-rad) return 0;

/*======================== Y-tests ========================*/
#define AXISTEST_Y02(a, b, fa, fb)			   \
    p0 = -a*v0[X] + b*v0[Z];		      	   \
    p2 = -a*v2[X] + b*v2[Z];	       	       	   \
    if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;} \
    rad = fa * boxhalfsize[X] + fb * boxhalfsize[Z];   \
    if(min>rad || max<-rad) return 0;

#define AXISTEST_Y1(a, b, fa, fb)			   \
    p0 = -a*v0[X] + b*v0[Z];		      	   \
    p1 = -a*v1[X] + b*v1[Z];	     	       	   \
    if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
    rad = fa * boxhalfsize[X] + fb * boxhalfsize[Z];   \
    if(min>rad || max<-rad) return 0;

/*======================== Z-tests ========================*/

#define AXISTEST_Z12(a, b, fa, fb)			   \
    p1 = a*v1[X] - b*v1[Y];			           \
    p2 = a*v2[X] - b*v2[Y];			       	   \
    if(p2<p1) {min=p2; max=p1;} else {min=p1; max=p2;} \
    rad = fa * boxhalfsize[X] + fb * boxhalfsize[Y];   \
    if(min>rad || max<-rad) return 0;

#define AXISTEST_Z0(a, b, fa, fb)			   \
    p0 = a*v0[X] - b*v0[Y];				   \
    p1 = a*v1[X] - b*v1[Y];			           \
    if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
    rad = fa * boxhalfsize[X] + fb * boxhalfsize[Y];   \
    if(min>rad || max<-rad) return 0;

int triBoxOverlap(float boxcenter[3],float boxhalfsize[3],float triverts[3][3])
{

    /*    use separating axis theorem to test overlap between triangle and box */
    /*    need to test for overlap in these directions: */
    /*    1) the {x,y,z}-directions (actually, since we use the AABB of the triangle */
    /*       we do not even need to test these) */
    /*    2) normal of the triangle */
    /*    3) crossproduct(edge from tri, {x,y,z}-directin) */
    /*       this gives 3x3=9 more tests */
    float v0[3],v1[3],v2[3];
    //float axis[3];
    float min,max,d,p0,p1,p2,rad,fex,fey,fez;  
    float normal[3],e0[3],e1[3],e2[3];

    /* This is the fastest branch on Sun */
    /* move everything so that the boxcenter is in (0,0,0) */
    SUB(v0,triverts[0],boxcenter);
    SUB(v1,triverts[1],boxcenter);
    SUB(v2,triverts[2],boxcenter);

    /* compute triangle edges */
    SUB(e0,v1,v0);      /* tri edge 0 */
    SUB(e1,v2,v1);      /* tri edge 1 */
    SUB(e2,v0,v2);      /* tri edge 2 */

    /* Bullet 3:  */
    /*  test the 9 tests first (this was faster) */
    fex = fabs(e0[X]);
    fey = fabs(e0[Y]);
    fez = fabs(e0[Z]);
    AXISTEST_X01(e0[Z], e0[Y], fez, fey);
    AXISTEST_Y02(e0[Z], e0[X], fez, fex);
    AXISTEST_Z12(e0[Y], e0[X], fey, fex);

    fex = fabs(e1[X]);
    fey = fabs(e1[Y]);
    fez = fabs(e1[Z]);
    AXISTEST_X01(e1[Z], e1[Y], fez, fey);
    AXISTEST_Y02(e1[Z], e1[X], fez, fex);
    AXISTEST_Z0(e1[Y], e1[X], fey, fex);

    fex = fabs(e2[X]);
    fey = fabs(e2[Y]);
    fez = fabs(e2[Z]);
    AXISTEST_X2(e2[Z], e2[Y], fez, fey);
    AXISTEST_Y1(e2[Z], e2[X], fez, fex);
    AXISTEST_Z12(e2[Y], e2[X], fey, fex);

    /* Bullet 1: */
    /*  first test overlap in the {x,y,z}-directions */
    /*  find min, max of the triangle each direction, and test for overlap in */
    /*  that direction -- this is equivalent to testing a minimal AABB around */
    /*  the triangle against the AABB */

    /* test in X-direction */
    FINDMINMAX(v0[X],v1[X],v2[X],min,max);
    if(min>boxhalfsize[X] || max<-boxhalfsize[X]) return 0;

    /* test in Y-direction */
    FINDMINMAX(v0[Y],v1[Y],v2[Y],min,max);
    if(min>boxhalfsize[Y] || max<-boxhalfsize[Y]) return 0;

    /* test in Z-direction */
    FINDMINMAX(v0[Z],v1[Z],v2[Z],min,max);
    if(min>boxhalfsize[Z] || max<-boxhalfsize[Z]) return 0;

    /* Bullet 2: */
    /*  test if the box intersects the plane of the triangle */
    /*  compute plane equation of triangle: normal*x+d=0 */
    CROSS(normal,e0,e1);
    d=-DOT(normal,v0);  /* plane eq: normal.x+d=0 */
    if(!planeBoxOverlap(normal,d,boxhalfsize)) return 0;

    return 1;   /* box and triangle overlaps */
}

BOOL IntersectSectionLine2Triangle(
    D3DXVECTOR3& vA, D3DXVECTOR3& vB, D3DXVECTOR3& vC,
    D3DXVECTOR3& vM, D3DXVECTOR3& vN,
    D3DXVECTOR3* pOut
)
{
    BOOL            bResult  = false;
    D3DXPLANE*      pPlane   = NULL;
    D3DXVECTOR3*    pPoint   = NULL;
    float           fSideM   = 0.0f;
    float           fSideN   = 0.0f;
    D3DXVECTOR3     A2B      = vB - vA;
    D3DXVECTOR3     B2C      = vC - vB;
    D3DXVECTOR3     C2A      = vA - vC;
    D3DXPLANE       Plane;
    D3DXVECTOR3     Point;
    D3DXVECTOR3     A2P;
    D3DXVECTOR3     B2P;
    D3DXVECTOR3     C2P;
    D3DXVECTOR3     NA;
    D3DXVECTOR3     NB;
    D3DXVECTOR3     NC;

    pPlane =  D3DXPlaneFromPoints(&Plane, &vA, &vB, &vC);
    KG_PROCESS_ERROR(pPlane);

    fSideM = D3DXPlaneDotCoord(pPlane, &vM);
    fSideN = D3DXPlaneDotCoord(pPlane, &vN);
    KG_PROCESS_ERROR(fSideM * fSideN <= 0.0f);

    pPoint = D3DXPlaneIntersectLine(&Point, pPlane, &vM, &vN);
    KG_PROCESS_ERROR(pPoint);

    A2P = Point - vA;
    B2P = Point - vB;
    C2P = Point - vC;

    D3DXVec3Cross(&NA, &A2P, &A2B);
    D3DXVec3Cross(&NB, &B2P, &B2C);
    D3DXVec3Cross(&NC, &C2P, &C2A);

    fSideM = D3DXVec3Dot(&NA, &NB);
    fSideN = D3DXVec3Dot(&NA, &NC);

    KG_PROCESS_ERROR(fSideM >= 0.0f);
    KG_PROCESS_ERROR(fSideN >= 0.0f);

    if (pOut)
    {
        *pOut = Point;
    }

    bResult = true;
Exit0:
   return bResult;
}

inline void SortNumber(float& fNumber1, float& fNumber2)
{
    float fTemp   = 0.0;

    if (fNumber1 > fNumber2)
    {
        fTemp    = fNumber1;
        fNumber1 = fNumber2;
        fNumber2 = fTemp;
    }
}

BOOL IntersectSectionBox2Box(
    D3DXVECTOR3& vBoxAPoint1, D3DXVECTOR3& vBoxAPoint2,
    D3DXVECTOR3& vBoxBPoint1, D3DXVECTOR3& vBoxBPoint2
)
{
    SortNumber(vBoxAPoint1.x, vBoxAPoint2.x);
    SortNumber(vBoxAPoint1.y, vBoxAPoint2.y);
    SortNumber(vBoxAPoint1.z, vBoxAPoint2.z);

    SortNumber(vBoxBPoint1.x, vBoxBPoint2.x);
    SortNumber(vBoxBPoint1.y, vBoxBPoint2.y);
    SortNumber(vBoxBPoint1.z, vBoxBPoint2.z);

    BOOL bX = (vBoxAPoint1.x > vBoxBPoint2.x) || (vBoxAPoint2.x < vBoxBPoint1.x);
    BOOL bY = (vBoxAPoint1.y > vBoxBPoint2.y) || (vBoxAPoint2.y < vBoxBPoint1.y);
    BOOL bZ = (vBoxAPoint1.z > vBoxBPoint2.z) || (vBoxAPoint2.z < vBoxBPoint1.z);

    return !(bX || bY || bZ);
}


void UpdateNormal(
    KVertexEx* pVertex, int nVertexCount, 
    KTriangleEx* pTriangle, int nTriangleCount
)
{
    KVertexEx*      pVer    = NULL;
    KTriangleEx*    pTri    = NULL;

    pVer = pVertex;

    for (int i = 0; i < nVertexCount; i++)
    {
        pVer->vNormal = D3DXVECTOR3(0, 0, 0);
        pVer++;
    }
    
    // 遍历每个三角形, 计算法向量,累加到相应的顶点上去
    pTri = pTriangle;
    for (int i = 0; i < nTriangleCount; i++)
    {
        if (!pTri->pVertexA || !pTri->pVertexB || !pTri->pVertexC)
            continue;

        pTri->UpdateNormal();

        pTri->pVertexA->vNormal += pTri->vNormal;
        pTri->pVertexB->vNormal += pTri->vNormal;
        pTri->pVertexC->vNormal += pTri->vNormal;

        pTri++;
    }

    pVer = pVertex;
    for (int i = 0; i < nVertexCount; i++)
    {
        D3DXVec3Normalize(&pVer->vNormal, &pVer->vNormal);

        pVer++;
    }
}

BOOL IsFloatZero(float fValue)
{
    return (fValue > -EPSILON && fValue < EPSILON);
}

int FloatComp(float fA, float fB)
{
    if (fA >= fB + EPSILON)
        return 1;

    if (fB >= fA + EPSILON)
        return -1;

    return 0;
}
