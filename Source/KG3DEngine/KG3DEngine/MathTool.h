#pragma once
#ifndef _INCLUDE_MATHTOOL_H_
#define _INCLUDE_MATHTOOL_H_
#include <math.h>
#include "kg3dstructs.h"
#include "KGMathFunctions.h"

extern "C" {
    #include "Decompose.h"
};

inline void FillHMatrix(HMatrix& hMat, const D3DXMATRIX& d3dMax) {

    hMat[0][0] = d3dMax._11;
    hMat[1][0] = d3dMax._12;
    hMat[2][0] = d3dMax._13;
    hMat[3][0] = d3dMax._14;

    hMat[0][1] = d3dMax._21;
    hMat[1][1] = d3dMax._22;
    hMat[2][1] = d3dMax._23;
    hMat[3][1] = d3dMax._24;

    hMat[0][2] = d3dMax._31;
    hMat[1][2] = d3dMax._32;
    hMat[2][2] = d3dMax._33;
    hMat[3][2] = d3dMax._34;

    hMat[0][3] = d3dMax._41;
    hMat[1][3] = d3dMax._42;
    hMat[2][3] = d3dMax._43;
    hMat[3][3] = d3dMax._44;
}

inline void MatrixDecomposeAffine(const D3DXMATRIX& mat, AffineParts& affineParts) {

    HMatrix hMat;
    FillHMatrix(hMat, mat);
    decomp_affine(hMat, &affineParts);

}

inline void AffineToMartix(const AffineParts& affineParts, D3DXMATRIX& mat) {

    D3DXQUATERNION qScalingRotation;
    qScalingRotation.x = affineParts.u.x;
    qScalingRotation.y = affineParts.u.y;
    qScalingRotation.z = affineParts.u.z;
    qScalingRotation.w = affineParts.u.w;

    D3DXVECTOR3 Scaling;
    Scaling.x = affineParts.k.x;
    Scaling.y = affineParts.k.y;
    Scaling.z = affineParts.k.z;

    D3DXQUATERNION qRotation;
    qRotation.x = affineParts.q.x;
    qRotation.y = affineParts.q.y;
    qRotation.z = affineParts.q.z;
    qRotation.w = affineParts.q.w;

    D3DXVECTOR3 Translation;
    Translation.x = affineParts.t.x;
    Translation.y = affineParts.t.y;
    Translation.z = affineParts.t.z;

    D3DXMatrixTransformation(
        &mat,
        &D3DXVECTOR3(0.f, 0.f, 0.f),
        &qScalingRotation,
        &Scaling,
        &D3DXVECTOR3(0.f, 0.f, 0.f),
        &qRotation,
        &Translation
        );
}

extern int g_holdrand;

template<typename T>
T Random(const T& left, const T& right)
{
    float fRandNum = (float)(((g_holdrand = g_holdrand * 214013L + 2531011L) >> 16) & 0x7fff) / RAND_MAX;
    return (left + (right - left) * fRandNum);
}

//#include "GraphicsStruct.h"///不要在头文件上加这个，MathTool.h的引用很广，这样加.h会引发很大范围的编译耦合
class ObstructdInfo;
namespace GraphicsStruct
{
	class _BoundingBox;
	class CULLINFO;
	class _BoundingRect;
	class _Force;
};

float GetCircleAngle(D3DXVECTOR2 A,D3DXVECTOR2 vCenter);
void LockAxisY(D3DXMATRIX& Mat);

int TrimString(char *pszString);

BOOL FloatEqual(float A,float B);
BOOL PlaneEqual(D3DXPLANE* Plane1,D3DXPLANE* Plane2);

BOOL D3DMATERIAL9Equal(D3DMATERIAL9* pMat1,D3DMATERIAL9* pMat2);
BOOL IsPositionInBox(D3DXVECTOR3* pPos,GraphicsStruct::_BoundingBox* pBox);

BOOL IsLineIntersectParallelogram(D3DXVECTOR3* pInter,D3DXVECTOR3* pLineA,D3DXVECTOR3* pLineB,
					 D3DXVECTOR3* pA,D3DXVECTOR3* pB,D3DXVECTOR3* pD);
BOOL IsParallelogramIntersectParallelogram(D3DXVECTOR3* pInter,
										   D3DXVECTOR3* pA1,D3DXVECTOR3* pB1,D3DXVECTOR3* pD1,
										   D3DXVECTOR3* pA2,D3DXVECTOR3* pB2,D3DXVECTOR3* pD2);
BOOL IsPointInLine(D3DXVECTOR3* pV,D3DXVECTOR3* pLineA,D3DXVECTOR3* pLineB);
BOOL IsTriangleIntersectParallelogram(D3DXVECTOR3* pInter,
										   D3DXVECTOR3* pTA,D3DXVECTOR3* pTB,D3DXVECTOR3* pTC,
										   D3DXVECTOR3* pPA,D3DXVECTOR3* pPB,D3DXVECTOR3* pPD);
BOOL IsParallelogramIntersectBox(D3DXVECTOR3* pPA,D3DXVECTOR3* pPB,D3DXVECTOR3* pPD,
								 D3DXVECTOR3* pBA,D3DXVECTOR3* pBB,D3DXVECTOR3* pBC,D3DXVECTOR3* pBD);
BOOL IsPointInParallelogram(D3DXVECTOR3* pV,D3DXVECTOR3* pPA,D3DXVECTOR3* pPB,D3DXVECTOR3* pPD);
/*BOOL Collision_Sphere_Box(D3DXVECTOR3* pForceA,D3DXVECTOR3* pFPA,
						  D3DXVECTOR3* pForceB,D3DXVECTOR3* pFPB,
						  PH_BoneTable::_BonePoint * pBonePointA,
						  PH_BoneTable::_BonePoint * pBonePointB,
						  PH_BoneTable::_Bound * pBoundA,
						  PH_BoneTable::_Bound * pBoundB );*/
BOOL IsParallelogramIntersectSphere(D3DXVECTOR3* pIntersection,
								 D3DXVECTOR3* pPA,D3DXVECTOR3* pPB,D3DXVECTOR3* pPD,
								 D3DXVECTOR3* pS_Center,float S_Radius);
BOOL IsLineIntersectSphere(D3DXVECTOR3* pIntersection,
						   D3DXVECTOR3* pS_Center,float S_Radius,
						   D3DXVECTOR3* pLineA,D3DXVECTOR3* pLineB);

BOOL IsLineIntersectTriangle(D3DXVECTOR3& LineA,D3DXVECTOR3& LineDirection,
							 D3DXVECTOR3& a, D3DXVECTOR3& b, D3DXVECTOR3& c, D3DXVECTOR3& TriangleNormal, 
							 BOOL posSide, BOOL negSide, float& dist);



/*BOOL Collision_Sphere_Point(D3DXVECTOR3* pForceA,D3DXVECTOR3* pFPA,
							D3DXVECTOR3* pForceB,D3DXVECTOR3* pFPB,
							PH_BoneTable::_BonePoint * pBonePointA,
							PH_BoneTable::_BonePoint * pBonePointB,
							PH_BoneTable::_Bound * pBoundA,
							PH_BoneTable::_Bound * pBoundB );
BOOL Collision_Sphere_Sphere(D3DXVECTOR3* pForceA,D3DXVECTOR3* pFPA,
							 D3DXVECTOR3* pForceB,D3DXVECTOR3* pFPB,
							 PH_BoneTable::_BonePoint * pBonePointA,
							 PH_BoneTable::_BonePoint * pBonePointB,
							 PH_BoneTable::_Bound * pBoundA,
							 PH_BoneTable::_Bound * pBoundB);
BOOL IsSphereIntersectSphere(D3DXVECTOR3* pIntersection,
							 D3DXVECTOR3* pCenterA,float rA,
							 D3DXVECTOR3* pCenterB,float rB);

BOOL Collision_Sphere_Line(D3DXVECTOR3* pForceA,D3DXVECTOR3* pFPA,
							D3DXVECTOR3* pForceB,D3DXVECTOR3* pFPB,
							PH_BoneTable::_BonePoint * pBonePointA,
							PH_BoneTable::_BonePoint * pBonePointB,
							PH_BoneTable::_Bound * pBoundA,
							PH_BoneTable::_Bound * pBoundB );*/
BOOL IsSphereIntersectTriangle(D3DXVECTOR3* pIntersection,
							   D3DXVECTOR3* pCenter,float Radius,
							   D3DXVECTOR3* pA,D3DXVECTOR3* pB,D3DXVECTOR3* pC);
/*BOOL Collision_Sphere_Triangle(D3DXVECTOR3* pForceA,D3DXVECTOR3* pFPA,
							D3DXVECTOR3* pForceB,D3DXVECTOR3* pFPB,
							PH_BoneTable::_BonePoint * pBonePointA,
							PH_BoneTable::_BonePoint * pBonePointB,
							PH_BoneTable::_Bound * pBoundA,
							PH_BoneTable::_Bound * pBoundB );*/
/*BOOL Collision_Sphere_Parallelogram(D3DXVECTOR3* pForceA,D3DXVECTOR3* pFPA,
							D3DXVECTOR3* pForceB,D3DXVECTOR3* pFPB,
							PH_BoneTable::_BonePoint * pBonePointA,
							PH_BoneTable::_BonePoint * pBonePointB,
							PH_BoneTable::_Bound * pBoundA,
							PH_BoneTable::_Bound * pBoundB );
BOOL Collision_Sphere_Column(D3DXVECTOR3* pForceA,D3DXVECTOR3* pFPA,
									D3DXVECTOR3* pForceB,D3DXVECTOR3* pFPB,
									PH_BoneTable::_BonePoint * pBonePointA,
									PH_BoneTable::_BonePoint * pBonePointB,
									PH_BoneTable::_Bound * pBoundA,
									PH_BoneTable::_Bound * pBoundB );*/

D3DXVECTOR3* GetViewPosition(D3DXVECTOR3* pPos,D3DXMATRIX MatView);

void  BoundingBox_Extract(D3DXVECTOR3* pPositions,GraphicsStruct::_BoundingBox* pBox);
void  BoundingBox_TransformCoord(D3DXVECTOR3* pPositions,GraphicsStruct::_BoundingBox* pBox,
								 D3DXMATRIX * pMatrix);
void BoundingBox_Project(D3DXVECTOR3* pPositions,GraphicsStruct::_BoundingBox* pBox,
						 D3DVIEWPORT9 *pViewport,D3DXMATRIX *pProjection,
						 D3DXMATRIX *pView,D3DXMATRIX *pWorld);
BOOL  IsBoundingBoxInView(GraphicsStruct::_BoundingBox* pBox,
						  D3DVIEWPORT9 *pViewport,D3DXMATRIX *pProjection,
						  D3DXMATRIX *pView,D3DXMATRIX *pWorld);

HRESULT  GetViewVector(D3DXVECTOR3* pPosition,D3DXVECTOR3* pDirection,D3DXMATRIX *pView);
BOOL     IsPointInBoundingRect(D3DXVECTOR2* pPoint,GraphicsStruct::_BoundingRect* pRect);
BOOL     IsBoundingBoxIntersected(GraphicsStruct::_BoundingBox* pBox1,GraphicsStruct::_BoundingBox* pBox2);
BOOL     IsBoxIntersected(GraphicsStruct::_BoundingBox* pBox1,GraphicsStruct::_BoundingBox* pBox2,
					      D3DXMATRIX *pMat1  ,   D3DXMATRIX *pMat2);
BOOL     IsParallelogramIntersectBox(D3DXVECTOR3* pPA,D3DXVECTOR3* pPB,D3DXVECTOR3* pPD,
									 D3DXVECTOR3* pBoxPos);
BOOL     IsPointInBox(D3DXVECTOR3* Pos,D3DXVECTOR3* BoxPos);
BOOL     IsLineIntersectBox(D3DXVECTOR3* Inter,D3DXVECTOR3* LineA,D3DXVECTOR3* LineB,
							D3DXVECTOR3* BoxPos);

//BOOL     IsShooted(D3DXVECTOR3* Inter,DO_DataObjectTable::_DataObject* pObject,DO_DataObjectTable::_DataObject * pBullet);
HRESULT  GetViewDirection(D3DXVECTOR3* pDirection,D3DXMATRIX *pView);
int __cdecl ObjectSortCB(const VOID *arg1, const VOID *arg2);
D3DXVECTOR3   AimPosition(D3DXVECTOR3* Pos,D3DXVECTOR3* Aim);

BOOL  Collision_Sphere_Box(GraphicsStruct::_Force* pForceA,GraphicsStruct::_Force* pForceB,
						   D3DXVECTOR3* S_Center,float S_Radiu,
						   D3DXVECTOR3* BoxPositions);
/*void  Box_Extract(D3DXVECTOR3* pPositions,
				  PH_BoneTable::_Bone * pBone,
				  PH_BoneTable::_Bound * pBox);*/
BOOL  Collision_Box_Box(GraphicsStruct::_Force* pForceA,GraphicsStruct::_Force* pForceB,D3DXVECTOR3* pIntersect,
						   D3DXVECTOR3* SpeedA,
						   D3DXVECTOR3* SpeedB,
						   D3DXVECTOR3* BoxPositionsA,
						   D3DXVECTOR3* BoxPositionsB);


TCHAR* GetFileName(TCHAR* PathName);
void RemoveSpace(TCHAR* Str);



// Prototypes for the culling functions
VOID UpdateCullInfo( GraphicsStruct::CULLINFO* pCullInfo, D3DXMATRIXA16* pMatView, D3DXMATRIXA16* pMatProj );
DWORD/*GraphicsStruct::CULLSTATE*/ CullObject( GraphicsStruct::CULLINFO* pCullInfo, D3DXVECTOR3* pVecBounds, D3DXPLANE* pPlaneBounds,
									 bool IsWaterReflect);
BOOL EdgeIntersectsFace( D3DXVECTOR3* pEdges, D3DXVECTOR3* pFaces, D3DXPLANE* pPlane );

void MatrixExract(D3DXVECTOR3& Scaling,D3DXVECTOR3& Position,
				  D3DXMATRIX& Rotation,D3DXMATRIX* pInMatrix);





DWORD/*GraphicsStruct::CULLSTATE*/ CullObject_SSE2( GraphicsStruct::CULLINFO* pCullInfo, D3DXVECTOR3* pVecBounds, 
										  D3DXPLANE* pPlaneBounds );///?没有定义
BOOL EdgesIntersectsFacesSSE2(D3DXVECTOR3 *pEdges, D3DXVECTOR3 *pFacePoints, D3DXPLANE* pPlanes);


float PointDistanceLine(CONST D3DXVECTOR3 *pPoint,
						CONST D3DXVECTOR3 *pV1,
						CONST D3DXVECTOR3 *pV2);//点到线的距离
FLOAT PointDistanceLine(const D3DXVECTOR3& vP, const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vNormalizedDir);//用直线隐式的版本

WORD A8R8G8B8ToA4R4G4B4(DWORD dwIn);
DWORD A4R4G4B4ToA8R8G8B8(WORD wIn);
//void GetLine2D(float* pfA,float* pfB,float* pfC,D3DXVECTOR2& A,D3DXVECTOR2& B);



//////////////////////////////////////////////////////////////////////////
//
 
inline void  GetLine2D(float* pA, float* pB, float* pC, D3DXVECTOR2& v1, D3DXVECTOR2& v2)//A，B，D是直线的隐式定义ax+by+c = 0.参考“3D数学基础”[Fletcher][12.2.4]
{
    *pA = v2.y - v1.y;
    *pB = v1.x - v2.x;
    *pC = (v2.x - v1.x) * v2.y - (v2.y - v1.y) * v2.x;
}

inline float PointDistanceLineSq(CONST D3DXVECTOR3 *pPoint,
								 CONST D3DXVECTOR3	*pV1,
								 CONST D3DXVECTOR3	*pV2)
{
	D3DXVECTOR3 vLineDir;
	D3DXVECTOR3 vCross;

	D3DXVec3Normalize(&vLineDir, &(*pV2 - *pV1));
	D3DXVec3Cross(&vCross, &vLineDir, &(*pPoint - *pV1));
	
	return D3DXVec3LengthSq(&vCross);
	
}

inline const D3DXMATRIX* GetLHRHInterchangeMatrix()
{
    static D3DXMATRIX Mat = D3DXMATRIX(
        1.f, 0.f,  0.f, 0.f,
        0.f, 1.f,  0.f, 0.f,
        0.f, 0.f, -1.f, 0.f,
        0.f, 0.f,  0.f, 1.f        
        );
    return &Mat;
}




bool IsSphereCrossTriangle(
    const D3DXVECTOR3&    vCenter,
    float                 fRadius,
    const D3DXVECTOR3&    vTriA,
    const D3DXVECTOR3&    vTriB,
    const D3DXVECTOR3&    vTriC,
    bool                  bCW,
    ObstructdInfo*        pInfo
);

bool IsSphereCrossTriangle(
    const D3DXVECTOR3&    vCenter,
    float                 fRadius,
    const D3DXVECTOR3&    vTriA,
    const D3DXVECTOR3&    vTriB,
    const D3DXVECTOR3&    vTriC
    );

/* 
   fast triangle box overlap check
   collator : Zhao Xinyu
 */

bool IsTriangleBoxOverlap(const D3DXVECTOR3& boxCenter, const D3DXVECTOR3& boxHalfSize, D3DXVECTOR3 tri[3]);

template<typename T>
inline void clip(T& v, const T& vMin, const T& vMax)
{
    if (v < vMin)
        v = vMin;

    if (v > vMax)
        v = vMax;
}


//////////////////////////////////////////////////////////////////////////
///KG3DPoly已经被移动到MathToolEx.h里面了

///请尽量不要在这个文件里面加一些和数学函数没有关系的东西，因为编辑器里面也用这个文件
///而且这个文件经常加东西，一改就会引发连锁编译。所以最好减少头文件耦合，以减少
///连锁编译



#endif //_INCLUDE_MATHTOOL_H_

