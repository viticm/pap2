#include "KFace2Face.h"

void BOP_Face2Face(KVertex* pVertex, int nVertexCount, KTriangle* pFacesA, int nFaceCountA, KTriangle* pFacesB, int nFaceCountB)
{
    for (int i = 0; i < nFaceCountA; i++)
    {
        D3DXVECTOR3*    pA1 = &(pVertex[pFacesA[i].nA].vPos);
        D3DXVECTOR3*    pA2 = &(pVertex[pFacesA[i].nB].vPos);
        D3DXVECTOR3*    pA3 = &(pVertex[pFacesA[i].nC].vPos);
        D3DXVECTOR3     NormalA;
        D3DXPLANE       PlaneA;

        D3DXPlaneFromPoints(&PlaneA, pA1, pA2, pA3);

        NormalA = D3DXVECTOR3(PlaneA.a, PlaneA.b, PlaneA.c);
        D3DXVec3Normalize(&NormalA, &NormalA);

        for (int j = 0; j < nFaceCountB; j++)
        {
            D3DXVECTOR3*    pB1 = &(pVertex[pFacesB[j].nA].vPos);
            D3DXVECTOR3*    pB2 = &(pVertex[pFacesB[j].nB].vPos);
            D3DXVECTOR3*    pB3 = &(pVertex[pFacesB[j].nC].vPos);
            D3DXVECTOR3     NormalB;
            D3DXPLANE       PlaneB;
            float           fDistance;
            BOOL            bPA1OnPlaneB = false;
            BOOL            bPA2OnPlaneB = false;
            BOOL            bPA3OnPlaneB = false;
            
            D3DXPlaneFromPoints(&PlaneB, pB1, pB2, pB3);

            NormalB = D3DXVECTOR3(PlaneB.a, PlaneB.b, PlaneB.c);
            D3DXVec3Normalize(&NormalB, &NormalB);

            fDistance = D3DXVec3Dot(&NormalB, pA1) + PlaneB.d;
            bPA1OnPlaneB = IsFloatZero(fDistance);

            fDistance = D3DXVec3Dot(&NormalB, pA2) + PlaneB.d;
            bPA2OnPlaneB = IsFloatZero(fDistance);

            fDistance = D3DXVec3Dot(&NormalB, pA3) + PlaneB.d;
            bPA3OnPlaneB = IsFloatZero(fDistance);

            if (bPA1OnPlaneB && bPA2OnPlaneB && bPA3OnPlaneB)
            {
                float fDotSum = D3DXVec3Dot(&NormalA, &NormalB);

//              if (fDotSum > 0)
//                  BOP_intersectCoplanarFaces(mesh,facesB,faceA,faceB,false);
            }
            else
            {
//              BOP_intersectNonCoplanarFaces(mesh,facesA,facesB,faceA,faceB);
            }
        }
    }
}
