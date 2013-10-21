#ifndef _KBSP_H_
#define _KBSP_H_

#include "KApplication.h"
#include "MathTools.h"
#include <vector>

#define ON_TAG           0x00  // On the plane
#define OUT_TAG          0x01  // Above the plane  
#define IN_TAG           0x02  // Below the plane
#define INOUT_TAG        0x0E  // Above and below the plane  
#define OUTON_TAG        0x11  // Above and on the plane  
#define INON_TAG         0x12  // Below and on the plane
#define UNCLASSIFIED_TAG 0x0F  // Expecting to be classified

#define BROKEN_TAG       0x0B  // Splitted and unused ... 
#define PHANTOM_TAG      0x0C  // Phantom face: verts form collinear triangle
#define OVERLAPPED_TAG   0x0D  // Overlapped face

#define ON_ON_IN_TAG     IN_TAG
#define ON_IN_ON_TAG     IN_TAG << 2
#define IN_ON_ON_TAG     IN_TAG << 4

#define ON_ON_OUT_TAG    OUT_TAG
#define ON_OUT_ON_TAG    OUT_TAG << 2
#define OUT_ON_ON_TAG    OUT_TAG << 4

#define ON_ON_ON_TAG     ON_TAG
#define IN_IN_IN_TAG     IN_ON_ON_TAG | ON_IN_ON_TAG | ON_ON_IN_TAG
#define OUT_OUT_OUT_TAG  OUT_ON_ON_TAG | ON_OUT_ON_TAG | ON_ON_OUT_TAG

#define IN_IN_ON_TAG     IN_ON_ON_TAG | ON_IN_ON_TAG
#define IN_ON_IN_TAG     IN_ON_ON_TAG | ON_ON_IN_TAG
#define ON_IN_IN_TAG     ON_IN_ON_TAG | ON_ON_IN_TAG

#define OUT_OUT_ON_TAG   OUT_ON_ON_TAG | ON_OUT_ON_TAG
#define OUT_ON_OUT_TAG   OUT_ON_ON_TAG | ON_ON_OUT_TAG
#define ON_OUT_OUT_TAG   ON_OUT_ON_TAG | ON_ON_OUT_TAG

#define IN_OUT_OUT_TAG   IN_ON_ON_TAG | ON_OUT_OUT_TAG
#define OUT_IN_OUT_TAG   ON_IN_ON_TAG | OUT_ON_OUT_TAG
#define OUT_OUT_IN_TAG   ON_ON_IN_TAG | OUT_OUT_ON_TAG

#define OUT_IN_IN_TAG    ON_IN_IN_TAG | OUT_ON_ON_TAG
#define IN_OUT_IN_TAG    IN_ON_IN_TAG | ON_OUT_ON_TAG
#define IN_IN_OUT_TAG    IN_IN_ON_TAG | ON_ON_OUT_TAG

#define IN_ON_OUT_TAG    IN_ON_ON_TAG | ON_ON_OUT_TAG
#define IN_OUT_ON_TAG    IN_ON_ON_TAG | ON_OUT_ON_TAG
#define ON_IN_OUT_TAG    ON_IN_ON_TAG | ON_ON_OUT_TAG
#define ON_OUT_IN_TAG    ON_ON_IN_TAG | ON_OUT_ON_TAG
#define OUT_IN_ON_TAG    ON_IN_ON_TAG | OUT_ON_ON_TAG
#define OUT_ON_IN_TAG    ON_ON_IN_TAG | OUT_ON_ON_TAG

typedef enum BOP_TAGEnum
{
    BOP_IN           = IN_TAG,
    BOP_ON           = ON_TAG,
    BOP_OUT          = OUT_TAG,
    BOP_INOUT        = INOUT_TAG,
    BOP_INON         = INON_TAG,
    BOP_OUTON        = OUTON_TAG,
    BOP_UNCLASSIFIED = UNCLASSIFIED_TAG,
    BOP_PHANTOM      = PHANTOM_TAG,
    BOP_OVERLAPPED   = OVERLAPPED_TAG,
    BOP_BROKEN       = BROKEN_TAG,
    BOP_IN_ON_ON     = IN_ON_ON_TAG,
    BOP_ON_IN_ON     = ON_IN_ON_TAG,
    BOP_ON_ON_IN     = ON_ON_IN_TAG,
    BOP_OUT_ON_ON    = OUT_ON_ON_TAG,
    BOP_ON_OUT_ON    = ON_OUT_ON_TAG,
    BOP_ON_ON_OUT    = ON_ON_OUT_TAG,
    BOP_ON_ON_ON     = ON_ON_ON_TAG,
    BOP_IN_IN_IN     = IN_IN_IN_TAG,
    BOP_OUT_OUT_OUT  = OUT_OUT_OUT_TAG,
    BOP_IN_IN_ON     = IN_IN_ON_TAG,
    BOP_IN_ON_IN     = IN_ON_IN_TAG,
    BOP_ON_IN_IN     = ON_IN_IN_TAG,
    BOP_OUT_OUT_ON   = OUT_OUT_ON_TAG,
    BOP_OUT_ON_OUT   = OUT_ON_OUT_TAG,
    BOP_ON_OUT_OUT   = ON_OUT_OUT_TAG,
    BOP_IN_OUT_OUT   = IN_OUT_OUT_TAG,
    BOP_OUT_IN_OUT   = OUT_IN_OUT_TAG,
    BOP_OUT_OUT_IN   = OUT_OUT_IN_TAG,
    BOP_OUT_IN_IN    = OUT_IN_IN_TAG,
    BOP_IN_OUT_IN    = IN_OUT_IN_TAG,
    BOP_IN_IN_OUT    = IN_IN_OUT_TAG,
    BOP_IN_ON_OUT    = IN_ON_OUT_TAG,
    BOP_IN_OUT_ON    = IN_OUT_ON_TAG,
    BOP_ON_IN_OUT    = ON_IN_OUT_TAG,
    BOP_ON_OUT_IN    = ON_OUT_IN_TAG,
    BOP_OUT_IN_ON    = OUT_IN_ON_TAG,
    BOP_OUT_ON_IN    = OUT_ON_IN_TAG
} BOP_TAG;

inline BOP_TAG BOP_MakeTag(BOP_TAG tag1, BOP_TAG tag2, BOP_TAG tag3)
{
    return (BOP_TAG) (tag1 << 4 | tag2 << 2 | tag3); 
}

inline BOP_TAG BOP_AddON(BOP_TAG tag)
{
    return (tag == BOP_IN ? BOP_INON : (tag == BOP_OUT ? BOP_OUTON : tag));
}

inline bool BOP_CompTag(BOP_TAG tag1, BOP_TAG tag2)
{
    return (tag1 == tag2) || (BOP_AddON(tag1) == BOP_AddON(tag2));
}

bool BOP_Intersect(const D3DXVECTOR3& vL1, const D3DXVECTOR3& pL1, 
                   const D3DXVECTOR3& vL2, const D3DXVECTOR3& pL2, 
                   D3DXVECTOR3 &intersection)
{
    float t     = -1.0f;
    float den   = (vL1.y * vL2.x - vL1.x * vL2.y);

    if (!IsFloatZero(den))
    {
        t = (pL2.y * vL1.x - vL1.y * pL2.x + pL1.x * vL1.y - pL1.y * vL1.x) / den ;
    }
    else
    {
        den = (vL1.y * vL2.z - vL1.z * vL2.y);
        if (!IsFloatZero(den))
        {
            t = (pL2.y * vL1.z - vL1.y * pL2.z + pL1.z * vL1.y - pL1.y * vL1.z) / den ;
        }
        else
        {
            den = (vL1.x * vL2.z - vL1.z * vL2.x);
            if (!IsFloatZero(den))
            {
                t = (pL2.x * vL1.z - vL1.x * pL2.z + pL1.z * vL1.x - pL1.x * vL1.z) / den ;
            }
            else
            {
                return false;
            }
        }
    }

    intersection.x = vL2.x * t + pL2.x;
    intersection.y = vL2.y * t + pL2.y;
    intersection.z = vL2.z * t + pL2.z;

    return true;
}

BOOL BOP_GetCircleCenter(const D3DXVECTOR3& p1, const D3DXVECTOR3& p2, const D3DXVECTOR3& p3, D3DXVECTOR3& center)
{
    // Compute quad plane
    D3DXVECTOR3 p1p2    = p2 - p1;
    D3DXVECTOR3 p1p3    = p3 - p1;
    D3DXPLANE   plane;
    D3DXVECTOR3 plane_normal;
    D3DXVECTOR3 vL1;
    D3DXVECTOR3 vL2;
    D3DXVECTOR3 pL1;
    D3DXVECTOR3 pL2;
    float       fLen    = 0.0f;

    D3DXPlaneFromPoints(&plane, &p1, &p2, &p3);
    plane_normal = D3DXVECTOR3(plane.a, plane.b, plane.c);

    // Compute first line vector, perpendicular to plane vector and edge (p1,p2)
    D3DXVec3Cross(&vL1, &p1p2, &plane_normal);

    fLen = D3DXVec3Length(&vL1);
    if (IsFloatZero(fLen))
        return false;

    D3DXVec3Normalize(&vL1, &vL1);
    
    // Compute first line point, middle point of edge (p1,p2)
    D3DXVec3Lerp(&pL1, &p1, &p2, 0.5f);

    // Compute second line vector, perpendicular to plane vector and edge (p1,p3)
    D3DXVec3Cross(&vL2, &p1p3, &plane_normal);
    
    fLen = D3DXVec3Length(&vL2);
    if (IsFloatZero(fLen))
        return false;

    D3DXVec3Normalize(&vL2, &vL2);
    
    // Compute first line point, middle point of edge (p1,p3)
    D3DXVec3Lerp(&pL2, &p1, &p3, 0.5f);

    // Compute intersection (the lines lay on the same plane, so the intersection exists
    // only if they are not parallel!!)
    
    return BOP_Intersect(vL1, pL1, vL2, pL2, center);
}

bool BOP_IsInsideCircle(const D3DXVECTOR3& p1, const D3DXVECTOR3& p2, const D3DXVECTOR3& p3, const D3DXVECTOR3& q)
{
    BOOL        bRetCode    = false;
    float       fRadius     = 0.0f;
    float       fDistance   = 0.0f;
    D3DXVECTOR3 center;
    D3DXVECTOR3 p1pc;
    D3DXVECTOR3 pqpc;

    // Compute circle center
    bRetCode = BOP_GetCircleCenter(p1,p2,p3, center);

    if (!bRetCode)
        return true; // p1,p2 and p3 are collinears

    // Check if q is inside the circle
    p1pc = p1 - center;
    pqpc = q - center;
    
    fRadius = D3DXVec3Length(&p1pc);
    fDistance = D3DXVec3Length(&pqpc);
    
    return (fDistance <= fRadius + EPSILON);
}

typedef std::vector<D3DXVECTOR3>    BSPPoints;
typedef BSPPoints::iterator         BSPPointsIt;

class KBSPNode
{
public:
    KBSPNode(const D3DXPLANE& crPlane);
    ~KBSPNode();

    int AddFace(BSPPoints Points, const D3DXPLANE& crPlane);

    BOP_TAG ClassifyFace(const D3DXVECTOR3& p1, const D3DXVECTOR3& p2, const D3DXVECTOR3& p3, const D3DXPLANE& plane);

private:
    BOP_TAG TestPoint(const D3DXVECTOR3* cpPoint);

    BOP_TAG SubClassifyFaceIn(const D3DXVECTOR3& p1, const D3DXVECTOR3& p2, const D3DXVECTOR3& p3, const D3DXPLANE& plane);
    BOP_TAG SubClassifyFaceOut(const D3DXVECTOR3& p1, const D3DXVECTOR3& p2, const D3DXVECTOR3& p3, const D3DXPLANE& plane);

private:
    KBSPNode*   m_pInChild;
    KBSPNode*   m_pOutChild;
    D3DXPLANE   m_Plane;
    int         m_nDeepth;
};

class KBSPTree
{
public:
    KBSPTree();
    ~KBSPTree();

    void AddFace(const D3DXVECTOR3& p1, const D3DXVECTOR3& p2, const D3DXVECTOR3& p3, const D3DXPLANE& plane);

    BOP_TAG ClassifyFace(const D3DXVECTOR3& p1, const D3DXVECTOR3& p2, const D3DXVECTOR3& p3, const D3DXPLANE& plane);


private:
    KBSPNode* m_pRootNode;
    //KBSPNode* m_pBspBB;
    //BOP_BBox     m_bbox;
};

#endif  // _KBSP_H_
