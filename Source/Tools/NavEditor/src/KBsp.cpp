#include "KBsp.h"

KBSPNode::KBSPNode(const D3DXPLANE& crPlane)
{
    m_Plane     = crPlane;
    m_pInChild  = NULL;
    m_pOutChild = NULL;
    m_nDeepth   = 0;
}

KBSPNode::~KBSPNode()
{
    if (m_pInChild)
    {
        delete m_pInChild;
        m_pInChild = NULL;
    }

    if (m_pOutChild)
    {
        delete m_pOutChild;
        m_pOutChild = NULL;
    }

    m_nDeepth = 0;
}

int KBSPNode::AddFace(BSPPoints Points, const D3DXPLANE &crPlane)
{
    int     nResult = 0;
    int     nDeep   = 0;
    BOP_TAG nTag    = BOP_ON;

    for (BSPPointsIt it = Points.begin(), itEnd = Points.end(); it != itEnd; ++it)
    {
        nTag = (BOP_TAG) ((int)nTag | (int)TestPoint(&(*it)));
    }

    if (nTag == BOP_ON)                             // All points on the hyperplane. 
    {
        NULL;
    }
    else if ((nTag & BOP_IN) && !(nTag & BOP_OUT))  // All points inside the hyperplane. 
    {
        if (m_pInChild)
        {
            nDeep = m_pInChild->AddFace(Points, crPlane) + 1;
        }
        else
        {
            m_pInChild = new KBSPNode(crPlane);
            nDeep = 2;
        }
    }
    else if (!(nTag & BOP_IN) && (nTag & BOP_OUT))  // All points outside the hyperplane. 
    {
        if (m_pOutChild)
        {
            nDeep = m_pOutChild->AddFace(Points, crPlane) + 1;
        }
        else
        {
            m_pOutChild = new KBSPNode(crPlane);
            nDeep = 2;
        }
    }
    else                                            // Points lies in both sides. 
    {
        BSPPoints   InPoints;
        BSPPoints   OutPoints;
        D3DXVECTOR3 LastPoint   = Points.back();
        BOP_TAG     LastTag     = TestPoint(&LastPoint);
        BOP_TAG     TempState   = LastTag;

        for (BSPPointsIt it = Points.begin(), itEnd = Points.end(); it != itEnd; ++it)
        {
            D3DXVECTOR3 CurrentPoint    = *it;
            BOP_TAG     CurrentTag      = TestPoint(&CurrentPoint);

            if (LastTag != BOP_ON)
            {
                if (TempState == BOP_IN)
                {
                    if (m_pInChild) InPoints.push_back(LastPoint);
                }
                else
                {
                    if (m_pOutChild) OutPoints.push_back(LastPoint);
                }
                
                if (CurrentTag != BOP_ON && CurrentTag != TempState)
                {
                    D3DXVECTOR3 MidPoint;
                    D3DXPlaneIntersectLine(&MidPoint, &m_Plane, &LastPoint, &CurrentPoint);

                    if (m_pInChild)
                        InPoints.push_back(MidPoint);

                    if (m_pOutChild)
                        OutPoints.push_back(MidPoint);

                    TempState = CurrentTag;
                }
            }
            else
            {
                if (m_pInChild)
                    InPoints.push_back(LastPoint);

                if (m_pOutChild)
                    OutPoints.push_back(LastPoint);

                TempState = CurrentTag;
            }

            LastPoint   = CurrentPoint;
            LastTag     = CurrentTag;
        }

        if (m_pInChild)
        {
            nDeep = m_pInChild->AddFace(InPoints, crPlane) + 1;
        }
        else
        {
            m_pInChild = new KBSPNode(crPlane);
            nDeep = 2;
        }

        if (m_pOutChild)
        {
            nDeep = max(nDeep, m_pOutChild->AddFace(OutPoints, crPlane) + 1);
        }
        else
        {
            m_pOutChild = new KBSPNode(crPlane);
            nDeep = max(nDeep, 2);
        }
    }

    m_nDeepth = max(m_nDeepth, nDeep);

    return m_nDeepth;
}

BOP_TAG KBSPNode::ClassifyFace(const D3DXVECTOR3& p1, const D3DXVECTOR3& p2, const D3DXVECTOR3& p3, const D3DXPLANE& plane)
{
    BOP_TAG     nResult = BOP_UNCLASSIFIED;
    BOP_TAG     nTag    = BOP_MakeTag(TestPoint(&p1), TestPoint(&p2), TestPoint(&p3));
    float       fDotRes = 0.0f;
    D3DXVECTOR3 PN1;
    D3DXVECTOR3 PN2;
    BOP_TAG     Tag1;
    BOP_TAG     Tag2;
    BOP_TAG     Tag3;

    switch(nTag)
    {
    case BOP_IN_IN_IN:
        nResult = SubClassifyFaceIn(p1, p2, p3, plane);
    	break;

    case BOP_IN_IN_ON:
    case BOP_IN_ON_IN:
    case BOP_ON_IN_IN:
    case BOP_IN_ON_ON:
    case BOP_ON_IN_ON:
    case BOP_ON_ON_IN:
        nResult = SubClassifyFaceIn(p1, p2, p3, plane);
        nResult = BOP_AddON(nResult);
        break;

    case BOP_OUT_OUT_OUT:
        nResult = SubClassifyFaceOut(p1, p2, p3, plane);
        break;

    case BOP_OUT_OUT_ON:
    case BOP_OUT_ON_OUT:
    case BOP_ON_OUT_OUT:
    case BOP_OUT_ON_ON:
    case BOP_ON_OUT_ON:
    case BOP_ON_ON_OUT:
        nResult = SubClassifyFaceOut(p1, p2, p3, plane);
        nResult = BOP_AddON(nResult);
        break;

    case BOP_ON_ON_ON:
        PN1 = D3DXVECTOR3(m_Plane.a, m_Plane.b, m_Plane.c);
        PN2 = D3DXVECTOR3(plane.a, plane.b, plane.c);

        fDotRes = D3DXVec3Dot(&PN1, &PN2);
        if (fDotRes > 0.0f)
        {
            nResult = SubClassifyFaceIn(p1, p2, p3, plane);
            nResult = BOP_AddON(nResult);
        }
        else
        {
            nResult = SubClassifyFaceOut(p1, p2, p3, plane);
            nResult = BOP_AddON(nResult);
        }
        break;

    case BOP_IN_OUT_ON:
        D3DXPlaneIntersectLine(&PN1, &m_Plane, &p1, &p2);
        Tag1    = SubClassifyFaceIn(p1, PN1, p3, plane);
        Tag2    = SubClassifyFaceOut(PN1, p2, p3, plane);
        nResult = BOP_CompTag(Tag1, Tag2) ? BOP_AddON(Tag1) : BOP_INOUT;
        break;

    case BOP_OUT_IN_ON:
        D3DXPlaneIntersectLine(&PN1, &m_Plane, &p1, &p2);
        Tag1    = SubClassifyFaceIn(PN1, p2, p3, plane);
        Tag2    = SubClassifyFaceOut(p1, PN1, p3, plane);
        nResult = BOP_CompTag(Tag1, Tag2) ? BOP_AddON(Tag1) : BOP_INOUT;
        break;

    case BOP_IN_ON_OUT:
        D3DXPlaneIntersectLine(&PN1, &m_Plane, &p1, &p3);
        Tag1    = SubClassifyFaceIn(p1, p2, PN1, m_Plane);
        Tag2    = SubClassifyFaceOut(PN1, p2, p3, m_Plane);
        nResult = BOP_CompTag(Tag1, Tag2) ? BOP_AddON(Tag1) : BOP_INOUT;
        break;

    case BOP_OUT_ON_IN:
        D3DXPlaneIntersectLine(&PN1, &m_Plane, &p1, &p3);
        Tag1    = SubClassifyFaceIn(PN1, p2, p3, m_Plane);
        Tag2    = SubClassifyFaceOut(p1, p2, PN1, m_Plane);
        nResult = BOP_CompTag(Tag1, Tag2) ? BOP_AddON(Tag1) : BOP_INOUT;
        break;

    case BOP_ON_IN_OUT:
        D3DXPlaneIntersectLine(&PN1, &m_Plane, &p2, &p3);
        Tag1    = SubClassifyFaceIn(p1, p2, PN1, m_Plane);
        Tag2    = SubClassifyFaceOut(p1, PN1, p3, m_Plane);
        nResult = BOP_CompTag(Tag1, Tag2) ? BOP_AddON(Tag1) : BOP_INOUT;
        break;

    case BOP_ON_OUT_IN:
        D3DXPlaneIntersectLine(&PN1, &m_Plane, &p2, &p3);
        Tag1    = SubClassifyFaceIn(p1, PN1, p3, m_Plane);
        Tag2    = SubClassifyFaceOut(p1, p2, PN1, m_Plane);
        nResult = BOP_CompTag(Tag1, Tag2) ? BOP_AddON(Tag1) : BOP_INOUT;
        break;

    case BOP_IN_OUT_OUT:
        D3DXPlaneIntersectLine(&PN1, &m_Plane, &p1, &p2);
        D3DXPlaneIntersectLine(&PN2, &m_Plane, &p1, &p3);

        Tag1 = SubClassifyFaceIn(p1, PN1, PN2, plane);

        // Two ways to divide the triangle, will chose the least degenerated sub-triangles. 
        if (BOP_IsInsideCircle(p2, p3, PN1, PN2))
        {
            Tag2 = SubClassifyFaceOut(PN1, p2, PN2, plane);
            Tag3 = SubClassifyFaceOut(p2, p3, PN2, plane);
        }
        else
        {
            Tag2 = SubClassifyFaceOut(PN1, p3, PN2, plane);
            Tag3 = SubClassifyFaceOut(p2, p3, PN1, plane);
        }
        
        nResult = (BOP_CompTag(Tag1, Tag2) && BOP_CompTag(Tag2,Tag3)) ? Tag1 : BOP_INOUT;
        break;
        
    case BOP_OUT_IN_IN:
        D3DXPlaneIntersectLine(&PN1, &m_Plane, &p1, &p2);
        D3DXPlaneIntersectLine(&PN2, &m_Plane, &p1, &p3);

        Tag1 = SubClassifyFaceOut(p1, PN1, PN2, plane);

        // Two ways to divide the triangle, will chose the least degenerated sub-triangles. 
        if (BOP_IsInsideCircle(p2, p3, PN1, PN2))
        {
            Tag2 = SubClassifyFaceIn(PN1, p2, PN2, plane);
            Tag3 = SubClassifyFaceIn(p2, p3, PN2, plane);
        }
        else
        {
            Tag2 = SubClassifyFaceIn(PN1, p3, PN2, plane);
            Tag3 = SubClassifyFaceIn(p2, p3, PN1, plane);
        }

        nResult = (BOP_CompTag(Tag1, Tag2) && BOP_CompTag(Tag2, Tag3)) ? Tag1 : BOP_INOUT;
        break;

    case BOP_OUT_IN_OUT:
        D3DXPlaneIntersectLine(&PN1, &m_Plane, &p2, &p1);
        D3DXPlaneIntersectLine(&PN2, &m_Plane, &p2, &p3);

        Tag1 = SubClassifyFaceIn(p1, PN1, PN2, plane);

        // Two ways to divide the triangle, will chose the least degenerated sub-triangles. 
        if (BOP_IsInsideCircle(p1, p3, PN1, PN2))
        {
            Tag2 = SubClassifyFaceOut(p1, PN1, PN2, plane);
            Tag3 = SubClassifyFaceOut(p1, PN2, p3, plane);
        }
        else
        {
            Tag2 = SubClassifyFaceOut(p3, PN1, PN2, plane);
            Tag3 = SubClassifyFaceOut(p1, PN1, p3, plane);
        }

        nResult = (BOP_CompTag(Tag1, Tag2) && BOP_CompTag(Tag2, Tag3)) ? Tag1 : BOP_INOUT;
        break;

    case BOP_IN_OUT_IN:
        D3DXPlaneIntersectLine(&PN1, &m_Plane, &p2, &p1);
        D3DXPlaneIntersectLine(&PN2, &m_Plane, &p2, &p3);

        Tag1 = SubClassifyFaceOut(p1, PN1, PN2, plane);

        // Two ways to divide the triangle, will chose the least degenerated sub-triangles. 
        if (BOP_IsInsideCircle(p1, p3, PN1, PN2))
        {
            Tag2 = SubClassifyFaceIn(p1, PN1, PN2, plane);
            Tag3 = SubClassifyFaceIn(p1, PN2, p3, plane);
        }
        else
        {
            Tag2 = SubClassifyFaceIn(p3, PN1, PN2, plane);
            Tag3 = SubClassifyFaceIn(p1, PN1, p3, plane);
        }

        nResult = (BOP_CompTag(Tag1, Tag2) && BOP_CompTag(Tag2, Tag3)) ? Tag1 : BOP_INOUT;
        break;

    case BOP_OUT_OUT_IN:
        D3DXPlaneIntersectLine(&PN1, &m_Plane, &p3, &p1);
        D3DXPlaneIntersectLine(&PN2, &m_Plane, &p3, &p2);

        Tag1 = SubClassifyFaceIn(PN1, PN2, p3, plane);

        // Two ways to divide the triangle, will chose the least degenerated sub-triangles. 
        if (BOP_IsInsideCircle(p1, p2, PN1, PN2))
        {
            Tag2 = SubClassifyFaceOut(p1, p2, PN2, plane);
            Tag3 = SubClassifyFaceOut(p1, PN2, PN1, plane);
        }
        else
        {
            Tag2 = SubClassifyFaceOut(p1, p2, PN1, plane);
            Tag3 = SubClassifyFaceOut(p2, PN2, PN1, plane);
        }

        nResult = (BOP_CompTag(Tag1, Tag2) && BOP_CompTag(Tag2, Tag3)) ? Tag1 : BOP_INOUT;
        break;

    case BOP_IN_IN_OUT:
        D3DXPlaneIntersectLine(&PN1, &m_Plane, &p3, &p1);
        D3DXPlaneIntersectLine(&PN2, &m_Plane, &p3, &p2);

        Tag1 = SubClassifyFaceOut(PN1, PN2, p3, plane);

        // Two ways to divide the triangle, will chose the least degenerated sub-triangles. 
        if (BOP_IsInsideCircle(p1, p2, PN1, PN2))
        {
            Tag2 = SubClassifyFaceIn(p1, p2, PN2, plane);
            Tag3 = SubClassifyFaceIn(p1, PN2, PN1, plane);
        }
        else
        {
            Tag2 = SubClassifyFaceIn(p1, p2, PN1, plane);
            Tag3 = SubClassifyFaceIn(p2, PN2, PN1, plane);
        }

        nResult = (BOP_CompTag(Tag1, Tag2) && BOP_CompTag(Tag2, Tag3)) ? Tag1 : BOP_INOUT;
        break;

    default:
        nResult = BOP_UNCLASSIFIED;
        break;
    }

    return nResult;
}

BOP_TAG KBSPNode::TestPoint(const D3DXVECTOR3* cpPoint)
{
    BOP_TAG nResult     = BOP_UNCLASSIFIED;
    float   fDotCoord   = 0.0f;

    fDotCoord = D3DXPlaneDotCoord(&m_Plane, cpPoint);

    if (fDotCoord > -EPSILON && fDotCoord < EPSILON)
        nResult = BOP_ON;
    else if (fDotCoord >= EPSILON)
        nResult = BOP_OUT;
    else
        nResult = BOP_IN;

    return nResult;
}

BOP_TAG KBSPNode::SubClassifyFaceIn(const D3DXVECTOR3& p1, const D3DXVECTOR3& p2, const D3DXVECTOR3& p3, const D3DXPLANE& plane)
{
    if (m_pInChild)
        return m_pInChild->ClassifyFace(p1, p2, p3, plane);
    
    return BOP_IN;
}

BOP_TAG KBSPNode::SubClassifyFaceOut(const D3DXVECTOR3& p1, const D3DXVECTOR3& p2, const D3DXVECTOR3& p3, const D3DXPLANE& plane)
{
    if (m_pOutChild)
        return m_pOutChild->ClassifyFace(p1, p2, p3, plane);

    return BOP_OUT;
}

//////////////////////////////////////////////////////////////////////////

KBSPTree::KBSPTree()
{
    m_pRootNode = NULL;
    //m_pBspBB    = NULL;
}

KBSPTree::~KBSPTree()
{
    if (m_pRootNode)
    {
        delete m_pRootNode;
        m_pRootNode = NULL;
    }

//     if (m_pBspBB)
//     {
//         delete m_pBspBB;
//         m_pBspBB = NULL;
//     }
}

void KBSPTree::AddFace(const D3DXVECTOR3 &p1, const D3DXVECTOR3 &p2, const D3DXVECTOR3 &p3, const D3DXPLANE &plane)
{
    if (!m_pRootNode)
        m_pRootNode = new KBSPNode(plane);
    else
    {
        BSPPoints Points;
        Points.reserve(3);
        Points.push_back(p1);
        Points.push_back(p2);
        Points.push_back(p3);

        m_pRootNode->AddFace(Points, plane);
    }
}

BOP_TAG KBSPTree::ClassifyFace(const D3DXVECTOR3 &p1, const D3DXVECTOR3 &p2, const D3DXVECTOR3 &p3, const D3DXPLANE &plane)
{
    BOP_TAG nResult = BOP_OUT;

    if (m_pRootNode)
        nResult = m_pRootNode->ClassifyFace(p1, p2, p3, plane);

    return nResult;
}
