#include "stdafx.h"
#include "mathtool.h"
#include "KG3DStructs.h"
#include "KGMathFunctions.h"
//#include "GraphicsStruct.h"
//不是必要的话，不要这里加上别的头文件的引用，让MathTools干净点，特别是GraphicStruct.h，
//会连锁的引用别的头文件，结果整个引擎的头文件都跑到编辑器去了，这样辛苦的做DLL就没有意义了
//光在引擎中用的东西可以考虑用MathToolEx.h和cpp。如果非要两边用可以考虑做一个接口类。

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern D3DXVECTOR3 g_ViewDirection;
//extern KGraphicsTool g_cGraphicsTool;

/*
 * Lock The Martix's Y Axis
 */

void LockAxisY(D3DXMATRIX& Mat)
{
    D3DXVECTOR3 vScanl;
    D3DXVECTOR3 vTrans;
    D3DXMATRIX  matRot;

    MatrixExract(vScanl, vTrans, matRot, &Mat);

    D3DXMATRIX matScanl;
    D3DXMATRIX matTrans;
    D3DXMATRIX matRotat;

    D3DXMatrixTranslation(&matTrans, vTrans.x, vTrans.y, vTrans.z);
    D3DXMatrixScaling(&matScanl, vScanl.x,  vScanl.y, vScanl.z);

    D3DXVECTOR3 vAlexY = D3DXVECTOR3(matRot._21, matRot._22, matRot._23);

    matRotat = matRot;

    if (vAlexY != D3DXVECTOR3(0.f, 1.f, 0.f))
    {
        D3DXVec3Normalize(&vAlexY, &vAlexY);

        D3DXVECTOR3 vWordY = D3DXVECTOR3(0.f, 1.f, 0.f);
        D3DXVECTOR3 vCross;
        D3DXVec3Cross(&vCross, &vAlexY, &vWordY);
        D3DXVec3Normalize(&vCross, &vCross);

        FLOAT       fDot   = D3DXVec3Dot(&vAlexY, &vWordY);
        FLOAT       fAngel = acosf(fDot);
        D3DXMATRIX  Rot;

        D3DXMatrixRotationAxis(&Rot, &vCross, fAngel);

        matRotat = matRot * Rot;

    }

    Mat = matScanl * matRotat * matTrans;

}


BOOL FloatEqual(float A,float B)
{

	if ((A-B)<0.00001f)
		return TRUE;
	else 
		return FALSE;
}

BOOL FloatEqual(float A,float B,float C)
{

	if ((A-B)<C)
		return TRUE;
	else 
		return FALSE;

}


BOOL PlaneEqual(D3DXPLANE* Plane1,D3DXPLANE* Plane2)
{
	if (FloatEqual(Plane1->a,Plane2->a))
		if (FloatEqual(Plane1->b,Plane2->b))
			if (FloatEqual(Plane1->c,Plane2->c))
				if (FloatEqual(Plane1->d,Plane2->d))
					return TRUE;
				else return FALSE;
			else return FALSE;
		else return FALSE;
	else return FALSE;
}
BOOL PlaneEqual(D3DXPLANE* Plane1,D3DXPLANE* Plane2,float C)
{
	if (FloatEqual(Plane1->a,Plane2->a,C))
		if (FloatEqual(Plane1->b,Plane2->b,C))
			if (FloatEqual(Plane1->c,Plane2->c,C))
				if (FloatEqual(Plane1->d,Plane2->d,C))
					return TRUE;
				else return FALSE;
			else return FALSE;
		else return FALSE;
	else return FALSE;
}
BOOL D3DCOLORVALUEqual(D3DCOLORVALUE* pCo1,D3DCOLORVALUE* pCo2)
{
	if (FloatEqual(pCo1->r,pCo2->r))
		if (FloatEqual(pCo1->g,pCo2->g))
			if (FloatEqual(pCo1->b,pCo2->b))
				if (FloatEqual(pCo1->a,pCo2->a))
					return TRUE;
				else return FALSE;
			else return FALSE;
		else return FALSE;
	else return FALSE;
}
BOOL D3DMATERIAL9Equal(D3DMATERIAL9* pMat1,D3DMATERIAL9* pMat2)
{
	if (D3DCOLORVALUEqual(&pMat1->Diffuse,&pMat2->Diffuse))
		if (D3DCOLORVALUEqual(&pMat1->Ambient,&pMat2->Ambient))
			if (D3DCOLORVALUEqual(&pMat1->Specular,&pMat2->Specular))
				if (D3DCOLORVALUEqual(&pMat1->Emissive ,&pMat1->Emissive ))
					if (FloatEqual(pMat1->Power,pMat2->Power))
						return TRUE;
					else return FALSE;
				else return FALSE;
			else return FALSE;
		else return FALSE;
	else return FALSE;
}
BOOL IsPositionInBox(D3DXVECTOR3* pPos,GraphicsStruct::_BoundingBox* pBox)
{
	if ((pBox->PositionA.x<=pPos->x)&&(pBox->PositionA.y<=pPos->y)&&(pBox->PositionA.z<=pPos->z)&&
		(pBox->PositionB.x>=pPos->x)&&(pBox->PositionB.y>=pPos->y)&&(pBox->PositionB.z>=pPos->z))
		return TRUE;
	else
		return FALSE;
}

BOOL IsLineIntersectParallelogram(D3DXVECTOR3* pInter,D3DXVECTOR3* pLineA,D3DXVECTOR3* pLineB,
					 D3DXVECTOR3* pA,D3DXVECTOR3* pB,D3DXVECTOR3* pD)
{
	D3DXVECTOR3 Intersect;
	D3DXPLANE plane;
	D3DXPlaneFromPoints(&plane,pA,pB,pD);
	if (D3DXPlaneIntersectLine(&Intersect,&plane,pLineA,pLineB)==NULL)
		return FALSE;//line Paralle with plane
	D3DXVECTOR3 nor = *pLineA - *pLineB;
	D3DXVECTOR3 IB = Intersect - *pLineB;

	float sa = D3DXVec3Dot(&nor,&nor);
	float ss = D3DXVec3Dot(&IB,&nor);
	if ((ss<0.0f)||(ss>sa))
		return FALSE;
	D3DXVECTOR3 PIA = Intersect - *pA;
	D3DXVECTOR3 PBA = *pB - *pA;
	D3DXVECTOR3 PDA = *pD - *pA;

	float LenIBA = D3DXVec3Dot(&PIA,&PBA);
	float LenIDA = D3DXVec3Dot(&PIA,&PDA);
	float LenBA  = D3DXVec3Dot(&PBA,&PBA);
	float LenDA  = D3DXVec3Dot(&PDA,&PDA);

	if ((LenIBA<0.0f)||(LenIBA>LenBA)||(LenIDA<0.0f)||(LenIDA>LenDA))
		return FALSE;
	else 
	{
		*pInter = Intersect;
		return TRUE;
	}
}
BOOL IsParallelogramIntersectParallelogram(D3DXVECTOR3* pInter,
										   D3DXVECTOR3* pA1,D3DXVECTOR3* pB1,D3DXVECTOR3* pD1,
										   D3DXVECTOR3* pA2,D3DXVECTOR3* pB2,D3DXVECTOR3* pD2)
{
	D3DXVECTOR3 Intersect1;
	D3DXPLANE plane;
	D3DXPlaneFromPoints(&plane,pA1,pB1,pD1);
	D3DXVECTOR3 A,B,C,D;
	A = *pA2;
	B = *pB2;
	D = *pD2;
	C = B + D - A;
	if (D3DXPlaneIntersectLine(&Intersect1,&plane,&A,&B)!=NULL)
		if (IsPointInLine(&Intersect1,&A,&B)&&IsPointInParallelogram(&Intersect1,pA1,pB1,pD1))
			return TRUE;
	if (D3DXPlaneIntersectLine(&Intersect1,&plane,&B,&C)!=NULL)
		if (IsPointInLine(&Intersect1,&B,&C)&&IsPointInParallelogram(&Intersect1,pA1,pB1,pD1))
			return TRUE;
	if (D3DXPlaneIntersectLine(&Intersect1,&plane,&C,&D)!=NULL)
		if (IsPointInLine(&Intersect1,&C,&D)&&IsPointInParallelogram(&Intersect1,pA1,pB1,pD1))
			return TRUE;
	if (D3DXPlaneIntersectLine(&Intersect1,&plane,&D,&A)!=NULL)
		if (IsPointInLine(&Intersect1,&D,&A)&&IsPointInParallelogram(&Intersect1,pA1,pB1,pD1))
			return TRUE;
	D3DXPlaneFromPoints(&plane,pA2,pB2,pD2);
	return FALSE;
}
BOOL IsPointInLine(D3DXVECTOR3* pV,D3DXVECTOR3* pLineA,D3DXVECTOR3* pLineB)
{
	D3DXVECTOR3 nor = *pLineA - *pLineB;
	D3DXVECTOR3 VB  = *pV - *pLineB;
	float sa = D3DXVec3Dot(&nor,&nor);
	float ss = D3DXVec3Dot(&VB,&nor);
	if ((ss<0.0f)||(ss>sa))
		return FALSE;
	else
		return TRUE;
}
BOOL IsPointInParallelogram(D3DXVECTOR3* pV,D3DXVECTOR3* pPA,D3DXVECTOR3* pPB,D3DXVECTOR3* pPD)
{
	D3DXVECTOR3 PIA = *pV - *pPA;
	D3DXVECTOR3 PBA = *pPB - *pPA;
	D3DXVECTOR3 PDA = *pPD - *pPA;

	float LenIBA = D3DXVec3Dot(&PIA,&PBA);
	float LenIDA = D3DXVec3Dot(&PIA,&PDA);
	float LenBA  = D3DXVec3Dot(&PBA,&PBA);
	float LenDA  = D3DXVec3Dot(&PDA,&PDA);

	if ((LenIBA<0.0f)||(LenIBA>LenBA)||(LenIDA<0.0f)||(LenIDA>LenDA))
		return FALSE;
	else 
		return TRUE;
}
BOOL IsTriangleIntersectParallelogram(D3DXVECTOR3* pInter,
										   D3DXVECTOR3* pTA,D3DXVECTOR3* pTB,D3DXVECTOR3* pTC,
										   D3DXVECTOR3* pPA,D3DXVECTOR3* pPB,D3DXVECTOR3* pPD)
{
	D3DXVECTOR3 Intersect1;
	D3DXPLANE plane;
	D3DXVECTOR3 A,B,C,D;
	D3DXPlaneFromPoints(&plane,pPA,pPB,pPD);
	A = *pTA;
	B = *pTB;
	C = *pTC;
	if (D3DXPlaneIntersectLine(&Intersect1,&plane,&A,&B)!=NULL)
		if (IsPointInLine(&Intersect1,&A,&B)&&IsPointInParallelogram(&Intersect1,pPA,pPB,pPD))
			return TRUE;
	if (D3DXPlaneIntersectLine(&Intersect1,&plane,&B,&C)!=NULL)
		if (IsPointInLine(&Intersect1,&B,&C)&&IsPointInParallelogram(&Intersect1,pPA,pPB,pPD))
			return TRUE;
	if (D3DXPlaneIntersectLine(&Intersect1,&plane,&C,&A)!=NULL)
		if (IsPointInLine(&Intersect1,&C,&A)&&IsPointInParallelogram(&Intersect1,pPA,pPB,pPD))
			return TRUE;
	return FALSE;
}

BOOL IsParallelogramIntersectBox(D3DXVECTOR3* pPA,D3DXVECTOR3* pPB,D3DXVECTOR3* pPD,
								 D3DXVECTOR3* pBA,D3DXVECTOR3* pBB,D3DXVECTOR3* pBC,D3DXVECTOR3* pBD)
{
	D3DXVECTOR3 A1,B1,C1,D1;
	D3DXVECTOR3 A2,B2,C2,D2;
	A1 = *pBB;
	A2 = *pBA;
	D2 = *pBC;
	D1 = D2 + (A1 - A2);
	B2 = *pBD;
	B1 = B2 + (A1 - A2);
	C2 = B2 + (D2 - A2);
	C1 = C2 + (A1 - A2);
	D3DXVECTOR3 Inter;
	if (IsParallelogramIntersectParallelogram(&Inter,pPA,pPB,pPD,&A1,&B1,&C1))
		return TRUE;
	if (IsParallelogramIntersectParallelogram(&Inter,pPA,pPB,pPD,&A2,&B2,&C2))
		return TRUE;
	if (IsParallelogramIntersectParallelogram(&Inter,pPA,pPB,pPD,&A1,&A2,&B1))
		return TRUE;
	if (IsParallelogramIntersectParallelogram(&Inter,pPA,pPB,pPD,&D1,&D2,&C1))
		return TRUE;
	if (IsParallelogramIntersectParallelogram(&Inter,pPA,pPB,pPD,&A1,&A2,&D1))
		return TRUE;
	if (IsParallelogramIntersectParallelogram(&Inter,pPA,pPB,pPD,&B1,&B2,&C1))
		return TRUE;
	return FALSE;
}
/*BOOL Collision_Sphere_Box(D3DXVECTOR3* pForceA,D3DXVECTOR3* pFPA,
						  D3DXVECTOR3* pForceB,D3DXVECTOR3* pFPB,
						  PH_BoneTable::_BonePoint * pBonePointA,
						  PH_BoneTable::_BonePoint * pBonePointB,
						  PH_BoneTable::_Bound * pBoundA,
						  PH_BoneTable::_Bound * pBoundB )
{
	D3DXVECTOR3 S_Center;
	pBonePointA->GetBoundSphereCenter(pBoundA,&S_Center);
	float       S_Radius;
	pBonePointA->GetBoundSphereRadius(pBoundA,&S_Radius);

	D3DXVECTOR3 A1,B1,C1,D1;
	D3DXVECTOR3 A2,B2,C2,D2;
	
	pBonePointB->GetBoundBoxVertex(pBoundB,&A1,&B1,&C1,&D1,&A2,&B2,&C2,&D2);
	
	D3DXVECTOR3 Box_Center = (A1 + C2) * 0.5f;
	D3DXVECTOR3 C_C = S_Center - Box_Center;

	/*float Boxlen = D3DXVec3LengthSq(&(Box_Center-A1));
	float Len = D3DXVec3LengthSq(&(C_C));
	if (Len>S_Radius*S_Radius+2*Boxlen)
		return FALSE;

	D3DXVECTOR3 Axis1 = (A2-A1) * 0.5f;
	D3DXVECTOR3 Axis2 = (B1-A1) * 0.5f;
	D3DXVECTOR3 Axis3 = (D1-A1) * 0.5f;

	D3DXVECTOR3 NAxis1,NAxis2,NAxis3;
	D3DXVECTOR3 LAxis1,LAxis2,LAxis3;

	D3DXVec3Normalize(&NAxis1,&Axis1);
	D3DXVec3Normalize(&NAxis2,&Axis2);
	D3DXVec3Normalize(&NAxis3,&Axis3);

	LAxis1 = Axis1 + NAxis1*S_Radius;
	LAxis2 = Axis2 + NAxis2*S_Radius;
	LAxis3 = Axis3 + NAxis3*S_Radius;

	float L1 = D3DXVec3Dot(&LAxis1,&C_C);
	float L2 = D3DXVec3Dot(&LAxis2,&C_C);
	float L3 = D3DXVec3Dot(&LAxis3,&C_C);

	float LL1 = D3DXVec3Dot(&LAxis1,&LAxis1);
	float LL2 = D3DXVec3Dot(&LAxis2,&LAxis2);
	float LL3 = D3DXVec3Dot(&LAxis3,&LAxis3);

	if ((L1<=LL1)&&(L1>=-LL1)&&(L2>=-LL2)&&(L2<=LL2)&&(L3>=-LL3)&&(L3<=LL3))
	{
		float SL1 = D3DXVec3Dot(&Axis1,&C_C);
		float SL2 = D3DXVec3Dot(&Axis2,&C_C);
		float SL3 = D3DXVec3Dot(&Axis3,&C_C);
		float SLL1 = D3DXVec3Dot(&Axis1,&Axis1);
		float SLL2 = D3DXVec3Dot(&Axis2,&Axis2);
		float SLL3 = D3DXVec3Dot(&Axis3,&Axis3);

		if ((SL1>=-1.0f*SLL1)&&(SL1<=SLL1))
		{
			if ((SL2>=-1.0f*SLL2)&&(SL2<=SLL2))
			{
				D3DXVECTOR3 Inter;
				D3DXPLANE   plane;
				if (SL3>=0)
				{
					D3DXPlaneFromPoints(&plane,&D1,&C1,&C2);
					if (D3DXPlaneIntersectLine(&Inter,&plane,&S_Center,&(S_Center+NAxis3))!=NULL)
					{
						*pForceA = S_Center - Inter;
						*pFPA = Inter;
						*pForceB = - *pForceA;
						*pFPB = *pFPA;
						return TRUE;
					}
				}
				else
				{
					D3DXPlaneFromPoints(&plane,&A1,&B1,&B2);
					if (D3DXPlaneIntersectLine(&Inter,&plane,&S_Center,&(S_Center+NAxis3))!=NULL)
					{
						*pForceA = S_Center - Inter;
						*pFPA = Inter;
						*pForceB = - *pForceA;
						*pFPB = *pFPA;
						return TRUE;
					}
				}
			}
			else if ((SL3>=-SLL3)&&(SL3<=SLL3))
			{
				D3DXVECTOR3 Inter;
				D3DXPLANE   plane;
				if (SL2>=0)
				{
					D3DXPlaneFromPoints(&plane,&B1,&C1,&B2);
					if (D3DXPlaneIntersectLine(&Inter,&plane,&S_Center,&(S_Center+NAxis2))!=NULL)
					{
						*pForceA = S_Center - Inter;
						*pFPA = Inter;
						*pForceB = - *pForceA;
						*pFPB = *pFPA;
						return TRUE;
					}
				}
				else
				{
					D3DXPlaneFromPoints(&plane,&A1,&D1,&A2);
					if (D3DXPlaneIntersectLine(&Inter,&plane,&S_Center,&(S_Center+NAxis2))!=NULL)
					{
						*pForceA = S_Center - Inter;
						*pFPA = Inter;
						*pForceB = - *pForceA;
						*pFPB = *pFPA;
						return TRUE;
					}
				}
			}
		}
		else
		{
			D3DXVECTOR3 Inter;
			D3DXPLANE   plane;
			if (SL1<=0)
			{
				D3DXPlaneFromPoints(&plane,&A1,&B1,&C1);
				if (D3DXPlaneIntersectLine(&Inter,&plane,&S_Center,&(S_Center+NAxis1))!=NULL)
				{
					*pForceA = S_Center - Inter;
					*pFPA = Inter;
					*pForceB = - *pForceA;
					*pFPB = *pFPA;
					return TRUE;
				}
			}
			else
			{
				D3DXPlaneFromPoints(&plane,&A2,&B2,&C2);
				if (D3DXPlaneIntersectLine(&Inter,&plane,&S_Center,&(S_Center+NAxis1))!=NULL)
				{
					*pForceA = S_Center - Inter;
					*pFPA = Inter;
					*pForceB = - *pForceA;
					*pFPB = *pFPA;
					return TRUE;
				}
			}
		}
	}
	else 
		return FALSE;
	/*
	D3DXVECTOR3 Inter;
	///////////////////////////////////////////////////////////////////////////////////////
	{
		D3DXPLANE plane;
		D3DXPlaneFromPoints(&plane,&A2,&A1,&D1);
		D3DXVECTOR3 Normal;
		Normal.x = plane.a;
		Normal.y = plane.b;
		Normal.z = plane.c;
		D3DXPlaneIntersectLine(&Inter,&plane,&S_Center,&(S_Center+Normal));
		if (IsPointInParallelogram(&Inter,&A2,&A1,&D1))
		{
			float leninter = D3DXVec3LengthSq(&(S_Center-Inter));
			if (leninter<=S_Radius*S_Radius)
			{
				*pForceA = S_Center - Inter;
				*pFPA = Inter;
				*pForceB = - *pForceA;
				*pFPB = Inter;
				return TRUE;
			}
		}
	}
	{
		D3DXPLANE plane;
		D3DXPlaneFromPoints(&plane,&B2,&B1,&C1);
		D3DXVECTOR3 Normal;
		Normal.x = plane.a;
		Normal.y = plane.b;
		Normal.z = plane.c;
		D3DXPlaneIntersectLine(&Inter,&plane,&S_Center,&(S_Center+Normal));
		if (IsPointInParallelogram(&Inter,&B2,&B1,&C1))
		{
			float leninter = D3DXVec3LengthSq(&(S_Center-Inter));
			if (leninter<=S_Radius*S_Radius)
			{
				*pForceA = S_Center - Inter;
				*pFPA = Inter;
				*pForceB = - *pForceA;
				*pFPB = Inter;
				return TRUE;
			}
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////
	{
		D3DXPLANE plane;
		D3DXPlaneFromPoints(&plane,&A1,&B1,&D1);
		D3DXVECTOR3 Normal;
		Normal.x = plane.a;
		Normal.y = plane.b;
		Normal.z = plane.c;
		D3DXPlaneIntersectLine(&Inter,&plane,&S_Center,&(S_Center+Normal));
		if (IsPointInParallelogram(&Inter,&A1,&B1,&D1))
		{
			float leninter = D3DXVec3LengthSq(&(S_Center-Inter));
			if (leninter<=S_Radius*S_Radius)
			{
				*pForceA = S_Center - Inter;
				*pFPA = Inter;
				*pForceB = - *pForceA;
				*pFPB = Inter;
				return TRUE;
			}
		}
	}
	{
		D3DXPLANE plane;
		D3DXPlaneFromPoints(&plane,&A2,&B2,&D2);
		D3DXVECTOR3 Normal;
		Normal.x = plane.a;
		Normal.y = plane.b;
		Normal.z = plane.c;
		D3DXPlaneIntersectLine(&Inter,&plane,&S_Center,&(S_Center+Normal));
		if (IsPointInParallelogram(&Inter,&A2,&B2,&D2))
		{
			float leninter = D3DXVec3LengthSq(&(S_Center-Inter));
			if (leninter<=S_Radius*S_Radius)
			{
				*pForceA = S_Center - Inter;
				*pFPA = Inter;
				*pForceB = - *pForceA;
				*pFPB = Inter;
				return TRUE;
			}
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////
	{
		D3DXPLANE plane;
		D3DXPlaneFromPoints(&plane,&A1,&B1,&A2);
		D3DXVECTOR3 Normal;
		Normal.x = plane.a;
		Normal.y = plane.b;
		Normal.z = plane.c;
		D3DXPlaneIntersectLine(&Inter,&plane,&S_Center,&(S_Center+Normal));
		if (IsPointInParallelogram(&Inter,&A1,&B1,&A2))
		{
			float leninter = D3DXVec3LengthSq(&(S_Center-Inter));
			if (leninter<=S_Radius*S_Radius)
			{
				*pForceA = S_Center - Inter;
				*pFPA = Inter;
				*pForceB = - *pForceA;
				*pFPB = Inter;
				return TRUE;
			}
		}
	}
	{
		D3DXPLANE plane;
		D3DXPlaneFromPoints(&plane,&D1,&C1,&D2);
		D3DXVECTOR3 Normal;
		Normal.x = plane.a;
		Normal.y = plane.b;
		Normal.z = plane.c;
		D3DXPlaneIntersectLine(&Inter,&plane,&S_Center,&(S_Center+Normal));
		if (IsPointInParallelogram(&Inter,&D1,&C1,&D2))
		{
			float leninter = D3DXVec3LengthSq(&(S_Center-Inter));
			if (leninter<=S_Radius*S_Radius)
			{
				*pForceA = S_Center - Inter;
				*pFPA = Inter;
				*pForceB = - *pForceA;
				*pFPB = Inter;
				return TRUE;
			}
		}
	}
	return FALSE;
}*/

BOOL IsParallelogramIntersectSphere(D3DXVECTOR3* pIntersection,
								    D3DXVECTOR3* pPA,D3DXVECTOR3* pPB,D3DXVECTOR3* pPD,
								    D3DXVECTOR3* pS_Center,float S_Radius)
{
	D3DXPLANE plane;
	D3DXPlaneFromPoints(&plane,pPA,pPB,pPD);
	D3DXVECTOR3 Normal;
	Normal.x = plane.a;
	Normal.y = plane.b;
	Normal.z = plane.c;
	D3DXVECTOR3 Intersection;
	D3DXPlaneIntersectLine(&Intersection,&plane,pS_Center,&(*pS_Center+Normal));
	float dis2 = D3DXVec3LengthSq(&(*pS_Center - Intersection));
	if (dis2<=S_Radius*S_Radius)
	{
		if (IsPointInParallelogram(&Intersection,pPA,pPB,pPD))
		{
			*pIntersection = Intersection;
			return TRUE;
		}
		else
			return FALSE;
	}
	else
		return FALSE;
}

/*BOOL CollisionPrecise_Sphere_Box(D3DXVECTOR3* pForceA,D3DXVECTOR3* pFPA,
						         D3DXVECTOR3* pForceB,D3DXVECTOR3* pFPB,
								 PH_BoneTable::_BonePoint * pBonePointA,
								 PH_BoneTable::_BonePoint * pBonePointB,
								 PH_BoneTable::_Bound * pBoundA,
								 PH_BoneTable::_Bound * pBoundB )
{
	D3DXVECTOR3 S_Center = pBonePointA->Position + pBoundA->A;
	float       S_Radius = pBoundA->B.x;

	D3DXVECTOR3 A1,B1,C1,D1;
	D3DXVECTOR3 A2,B2,C2,D2;
	A1 = pBonePointB->Position + pBoundB->B;
	A2 = pBonePointB->Position + pBoundB->A;
	D2 = pBonePointB->Position + pBoundB->C;
	D1 = D2 + (A1 - A2);
	B2 = pBonePointB->Position + pBoundB->D;
	B1 = B2 + (A1 - A2);
	C2 = B2 + (D2 - A2);
	C1 = C2 + (A1 - A2);
	
	D3DXVECTOR3 Inter;
	////////////////////////////////////////////////////////////////////////////////////
	//test  collision by position
	if (IsParallelogramIntersectSphere(&Inter,&A2,&A1,&D1,&S_Center,S_Radius))
	{
		D3DXPLANE plane;
		D3DXPlaneFromPoints(&plane,&A2,&A1,&D1);
		D3DXVECTOR3 Normal;

		*pForceA = S_Center - Inter;
		*pFPA = Inter;
		*pForceB = - *pForceA;
		Normal.x = plane.a;
		Normal.y = plane.b;
		Normal.z = plane.c;
		float len = D3DXVec3Dot(&Normal,pForceB);
		if (len>=0.0f)
			*pForceB = Normal;
		else
			*pForceB = -Normal;
		*pFPB = Inter;
		return TRUE;
	}
	if (IsParallelogramIntersectSphere(&Inter,&B2,&B1,&C1,&S_Center,S_Radius))
	{
		D3DXPLANE plane;
		D3DXPlaneFromPoints(&plane,&B2,&B1,&C1);
		D3DXVECTOR3 Normal;

		*pForceA = S_Center - Inter;
		*pFPA = Inter;
		*pForceB = - *pForceA;
		Normal.x = plane.a;
		Normal.y = plane.b;
		Normal.z = plane.c;
		float len = D3DXVec3Dot(&Normal,pForceB);
		if (len>=0.0f)
			*pForceB = Normal;
		else
			*pForceB = -Normal;
		*pFPB = Inter;
 		return TRUE;
	}
	if (IsParallelogramIntersectSphere(&Inter,&A1,&B1,&D1,&S_Center,S_Radius))
	{
		D3DXPLANE plane;
		D3DXPlaneFromPoints(&plane,&A1,&B1,&D1);
		D3DXVECTOR3 Normal;

		*pForceA = S_Center - Inter;
		*pFPA = Inter;
		*pForceB = - *pForceA;
		Normal.x = plane.a;
		Normal.y = plane.b;
		Normal.z = plane.c;
		float len = D3DXVec3Dot(&Normal,pForceB);
		if (len>=0.0f)
			*pForceB = Normal;
		else
			*pForceB = -Normal;
		*pFPB = Inter;
		return TRUE;
	}
	if (IsParallelogramIntersectSphere(&Inter,&A2,&B2,&D2,&S_Center,S_Radius))
	{
		D3DXPLANE plane;
		D3DXPlaneFromPoints(&plane,&A2,&B2,&D2);
		D3DXVECTOR3 Normal;

		*pForceA = S_Center - Inter;
		*pFPA = Inter;
		*pForceB = - *pForceA;
		Normal.x = plane.a;
		Normal.y = plane.b;
		Normal.z = plane.c;
		float len = D3DXVec3Dot(&Normal,pForceB);
		if (len>=0.0f)
			*pForceB = Normal;
		else
			*pForceB = -Normal;
		*pFPB = Inter;
 		return TRUE;
	}
	if (IsParallelogramIntersectSphere(&Inter,&A2,&A1,&B1,&S_Center,S_Radius))
	{
		D3DXPLANE plane;
		D3DXPlaneFromPoints(&plane,&A2,&A1,&B1);
		D3DXVECTOR3 Normal;

		*pForceA = S_Center - Inter;
		*pFPA = Inter;
		*pForceB = - *pForceA;
		Normal.x = plane.a;
		Normal.y = plane.b;
		Normal.z = plane.c;
		float len = D3DXVec3Dot(&Normal,pForceB);
		if (len>=0.0f)
			*pForceB = Normal;
		else
			*pForceB = -Normal;
		*pFPB = Inter;
		return TRUE;
	}
	if (IsParallelogramIntersectSphere(&Inter,&D2,&D1,&C1,&S_Center,S_Radius))
	{
		D3DXPLANE plane;
		D3DXPlaneFromPoints(&plane,&D2,&D1,&C1);
		D3DXVECTOR3 Normal;

		*pForceA = S_Center - Inter;
		*pFPA = Inter;
		*pForceB = - *pForceA;
		Normal.x = plane.a;
		Normal.y = plane.b;
		Normal.z = plane.c;
		float len = D3DXVec3Dot(&Normal,pForceB);
		if (len>=0.0f)
			*pForceB = Normal;
		else
			*pForceB = -Normal;
		*pFPB = Inter;
 		return TRUE;
	}
	////////////////////////////////////////////////////////////////////////////////////
	return FALSE;
}
BOOL Collision_Sphere_Point(D3DXVECTOR3* pForceA,D3DXVECTOR3* pFPA,
							D3DXVECTOR3* pForceB,D3DXVECTOR3* pFPB,
							PH_BoneTable::_BonePoint * pBonePointA,
							PH_BoneTable::_BonePoint * pBonePointB,
							PH_BoneTable::_Bound * pBoundA,
							PH_BoneTable::_Bound * pBoundB )
{
	D3DXVECTOR3 S_Center = pBonePointA->Position + pBoundA->A;
	float       S_Radius = pBoundA->B.x;
	D3DXVECTOR3 A = pBonePointB->Position + pBoundB->A;
	D3DXVECTOR3 B = pBonePointB->Last_Position + pBoundB->B;
	D3DXVECTOR3 Inter;
	D3DXVECTOR3 X = A - S_Center;
	if (D3DXVec3LengthSq(&X)<=S_Radius*S_Radius)
	{
		*pForceA = S_Center - A;
		*pFPA = A;
		*pForceB = - *pForceA;
		*pFPB = A;
 		return TRUE;
	}
	return FALSE;
}*/
BOOL IsLineIntersectSphere(D3DXVECTOR3* pIntersection,
						   D3DXVECTOR3* pS_Center,float S_Radius,
						   D3DXVECTOR3* pLineA,D3DXVECTOR3* pLineB)
{
	D3DXVECTOR3 Nor = *pLineA - *pLineB;
	D3DXPLANE plane;
	D3DXPlaneFromPointNormal(&plane,pS_Center,&Nor);
	D3DXVECTOR3 Inter;
	if (D3DXPlaneIntersectLine(&Inter,&plane,pLineA,pLineB)==NULL)
		return FALSE;
	if (IsPointInLine(&Inter,pLineA,pLineB))  
	{
		D3DXVECTOR3 IB = Inter - *pLineB;
		float sa = D3DXVec3Dot(&Nor,&Nor);
		float ss = D3DXVec3Dot(&IB,&Nor);
		if ((ss<0.0f)||(ss>sa))
			return FALSE;
		float rr = D3DXVec3LengthSq(&(Inter - *pS_Center));
		if (rr<=(S_Radius*S_Radius))
		{
			*pIntersection = Inter;
			return TRUE;
		}
		else
			return FALSE;
	}
	return FALSE;
}
/*BOOL Collision_Sphere_Sphere(D3DXVECTOR3* pForceA,D3DXVECTOR3* pFPA,
							 D3DXVECTOR3* pForceB,D3DXVECTOR3* pFPB,
							 PH_BoneTable::_BonePoint * pBonePointA,
							 PH_BoneTable::_BonePoint * pBonePointB,
							 PH_BoneTable::_Bound * pBoundA,
							 PH_BoneTable::_Bound * pBoundB)
{
	D3DXVECTOR3 S_CenterA = pBonePointA->Position + pBoundA->A;
	float       S_RadiusA = pBoundA->B.x;
	D3DXVECTOR3 S_CenterB = pBonePointB->Position + pBoundB->A;
	float       S_RadiusB = pBoundB->B.x;
	D3DXVECTOR3 Inter;
	if (IsSphereIntersectSphere(&Inter,&S_CenterA,S_RadiusA,&S_CenterB,S_RadiusB))
	{
		*pForceA = S_CenterA - Inter;
		*pFPA = Inter;
		*pForceB = - *pForceA;
		*pFPB = Inter;
 		return TRUE;
	}
	return FALSE;
}*/
BOOL IsSphereIntersectSphere(D3DXVECTOR3* pIntersection,D3DXVECTOR3* pCenterA,float rA,D3DXVECTOR3* pCenterB,float rB)
{
	D3DXVECTOR3 OO = *pCenterA - *pCenterB;
	float rr = rA + rB;
	float dis = D3DXVec3Length(&OO);
	if (dis<=rr)
	{
		D3DXVECTOR3 Nor = OO;
		D3DXVec3Normalize(&Nor,&Nor);
		(*pIntersection) = (*pCenterB + rB* Nor + *pCenterA - rA*Nor) * 0.5f;
		return TRUE;
	}
	else
		return FALSE;
}
/*BOOL Collision_Sphere_Line(D3DXVECTOR3* pForceA,D3DXVECTOR3* pFPA,
							D3DXVECTOR3* pForceB,D3DXVECTOR3* pFPB,
							PH_BoneTable::_BonePoint * pBonePointA,
							PH_BoneTable::_BonePoint * pBonePointB,
							PH_BoneTable::_Bound * pBoundA,
							PH_BoneTable::_Bound * pBoundB )
{
	D3DXVECTOR3 S_Center = pBonePointA->Position + pBoundA->A;
	float       S_Radius = pBoundA->B.x;
	D3DXVECTOR3 A = pBonePointB->Position + pBoundB->A;
	D3DXVECTOR3 B = pBonePointB->Position + pBoundB->B;
	
	D3DXVECTOR3 Inter;
	if (IsLineIntersectSphere(&Inter,&S_Center,S_Radius,&A,&B))
	{
		*pForceA = S_Center - Inter;
		*pFPA = Inter;
		*pForceB = - *pForceA;
		*pFPB = Inter;
 		return TRUE;
	}
	return FALSE;
}

BOOL Collision_Sphere_Triangle(D3DXVECTOR3* pForceA,D3DXVECTOR3* pFPA,
							D3DXVECTOR3* pForceB,D3DXVECTOR3* pFPB,
							PH_BoneTable::_BonePoint * pBonePointA,
							PH_BoneTable::_BonePoint * pBonePointB,
							PH_BoneTable::_Bound * pBoundA,
							PH_BoneTable::_Bound * pBoundB )
{
	D3DXVECTOR3 S_Center = pBonePointA->Position + pBoundA->A;
	float       S_Radius = pBoundA->B.x;
	D3DXVECTOR3 A = pBonePointB->Position + pBoundB->A;
	D3DXVECTOR3 B = pBonePointB->Position + pBoundB->B;
	D3DXVECTOR3 C = pBonePointB->Position + pBoundB->C;
	
	D3DXVECTOR3 Inter;
	if (IsSphereIntersectTriangle(&Inter,&S_Center,S_Radius,&A,&B,&C))
	{
		*pForceA = S_Center - Inter;
		*pFPA = Inter;
		*pForceB = - *pForceA;
		*pFPB = Inter;
 		return TRUE;
	}
	return FALSE;
}*/
BOOL IsSphereIntersectTriangle(D3DXVECTOR3* pIntersection,
							   D3DXVECTOR3* pCenter,float Radius,
							   D3DXVECTOR3* pA,D3DXVECTOR3* pB,D3DXVECTOR3* pC)
{
	D3DXPLANE plane;
	D3DXPlaneFromPoints(&plane,pA,pB,pC);
	D3DXVECTOR3 nor;
	nor.x = plane.a;
	nor.y = plane.b;
	nor.z = plane.c;
	D3DXVECTOR3 Inter;
	if (D3DXPlaneIntersectLine(&Inter,&plane,pCenter,&(*pCenter+nor))==NULL)
		return FALSE;
	else 
	{
		D3DXVECTOR3 IB = Inter - *pB;
		D3DXVECTOR3 AB = *pA-*pB;
		D3DXVECTOR3 CB = *pC-*pB;
		float sab = D3DXVec3Dot(&AB,&AB);
		float scb = D3DXVec3Dot(&CB,&CB);
		float s1  = D3DXVec3Dot(&AB,&IB);
		float s2  = D3DXVec3Dot(&CB,&IB);
		if ((s1<0.0f)||(s1>sab)||(s2<0.0f)||(s2>scb))
			return FALSE;
		float rr = D3DXVec3LengthSq(&(Inter-*pCenter));
		(*pIntersection) = Inter;
		if (rr<=(Radius*Radius))
			return TRUE;
		else
			return FALSE;
	}
}

/*BOOL Collision_Sphere_Parallelogram(D3DXVECTOR3* pForceA,D3DXVECTOR3* pFPA,
									D3DXVECTOR3* pForceB,D3DXVECTOR3* pFPB,
									PH_BoneTable::_BonePoint * pBonePointA,
									PH_BoneTable::_BonePoint * pBonePointB,
									PH_BoneTable::_Bound * pBoundA,
									PH_BoneTable::_Bound * pBoundB )
{
	D3DXVECTOR3 S_Center = pBonePointA->Position + pBoundA->A;
	float       S_Radius = pBoundA->B.x;
	D3DXVECTOR3 A = pBonePointB->Position + pBoundB->A;
	D3DXVECTOR3 B = pBonePointB->Position + pBoundB->B;
	D3DXVECTOR3 C = pBonePointB->Position + pBoundB->C;
	D3DXVECTOR3 D = A + C - B;

	D3DXVECTOR3 Inter;
	if (IsParallelogramIntersectSphere(&Inter,&A,&B,&D,&S_Center,S_Radius))
	{
		*pForceA = S_Center - Inter;
		*pFPA = Inter;
		*pForceB = - *pForceA;
		*pFPB = Inter;
 		return TRUE;
	}
	return FALSE;
}

BOOL Collision_Sphere_Column(D3DXVECTOR3* pForceA,D3DXVECTOR3* pFPA,
									D3DXVECTOR3* pForceB,D3DXVECTOR3* pFPB,
									PH_BoneTable::_BonePoint * pBonePointA,
									PH_BoneTable::_BonePoint * pBonePointB,
									PH_BoneTable::_Bound * pBoundA,
									PH_BoneTable::_Bound * pBoundB )
{
	D3DXVECTOR3 S_Center;
	pBonePointA->GetBoundSphereCenter(pBoundA,&S_Center);
	float       S_Radius;
	pBonePointA->GetBoundSphereRadius(pBoundA,&S_Radius);

	D3DXVECTOR3 A,B;
	pBonePointB->GetBoundColumnPointA(pBoundB,&A);
	pBonePointB->GetBoundColumnPointB(pBoundB,&B);
	float       rA,rB;
	pBonePointB->GetBoundColumnRadiusA(pBoundB,&rA);
	pBonePointB->GetBoundColumnRadiusB(pBoundB,&rB);
	D3DXVECTOR3 Normal = B - A; 
	D3DXVec3Normalize(&Normal,&Normal);
	D3DXVECTOR3 Inter;
	D3DXPLANE Plane;

	D3DXVECTOR3 AA = A - S_Radius*Normal;
	D3DXVECTOR3 BB = B + S_Radius*Normal;
	D3DXVECTOR3 C_Center = (AA + BB)*0.5f;

	D3DXPlaneFromPointNormal(&Plane,&C_Center,&Normal);
	D3DXPlaneIntersectLine(&Inter,&Plane,&S_Center,&(S_Center+Normal));

	D3DXVECTOR3 k1 = S_Center-Inter;
	D3DXVECTOR3 k2 = BB - C_Center;
	//D3DXVECTOR3 k3 = B - C_Center;

	float f1 = D3DXVec3Dot(&k1,&k2);
	float f2 = D3DXVec3Dot(&k2,&k2);
	//float f3 = D3DXVec3Dot(&k3,&k3);

	if ((f1<=f2)&&(f1>=-1.0f*f2))
	{
		if ((f1<=0.95f*f2)&&(f1>=-0.95f*f2))
		{
			float lens1 = D3DXVec3Length(&(Inter-C_Center));
			if (lens1<=rA+S_Radius)
			{
				*pForceA = Inter - C_Center;
				D3DXVec3Normalize(pForceA,pForceA);
				*pFPA = S_Center - (lens1-rA)*(*pForceA);
				*pForceB = - *pForceA;
				*pFPB = *pFPA;
				return TRUE;
			}
		}
		else
		{
			float lens1 = D3DXVec3LengthSq(&(Inter-C_Center));
			if (lens1<=rA*rA)
			{
				*pForceA = - f1*k2;
				if (f1>=0)
					*pFPA = Inter + A - C_Center;
				else
					*pFPA = Inter + B - C_Center;
				*pForceB = - *pForceA;
				*pFPB = *pFPA;
				return TRUE;
			}
		}
	}

	return FALSE;
}*/

D3DXVECTOR3* GetViewPosition(D3DXVECTOR3* pPos,D3DXMATRIX MatView)
{
	D3DXVECTOR3 AxisX,AxisY,AxisZ;

	AxisX.x = MatView._11;
	AxisX.y = MatView._21;
	AxisX.z = MatView._31;
	AxisY.x = MatView._12;
	AxisY.y = MatView._22;
	AxisY.z = MatView._32;
	AxisZ.x = MatView._13;
	AxisZ.y = MatView._23;
	AxisZ.z = MatView._33;

	float lenX,lenY,lenZ;
	lenX    = - MatView._41;
	lenY    = - MatView._42;
	lenZ    = - MatView._43;
	
	D3DXVECTOR3 Eye = AxisX*lenX + AxisY*lenY + AxisZ*lenZ;
	(*pPos) = Eye;
	return pPos;
}

WORD A8R8G8B8ToA4R4G4B4(DWORD dwIn)
{
	DWORD A = (dwIn & 0xFF000000) >> 24;
	DWORD R = (dwIn & 0x00FF0000) >> 16;
	DWORD G = (dwIn & 0x0000FF00) >> 8;
	DWORD B = (dwIn & 0x000000FF);

	static float f = 15/255.0F;
	A = (DWORD)(A*f);
	R = (DWORD)(R*f);
	G = (DWORD)(G*f);
	B = (DWORD)(B*f);

	WORD wOut = (WORD)(A<<12 | R<<8 | G<<4 | B);
	return wOut;
}

DWORD A4R4G4B4ToA8R8G8B8(WORD wIn)
{
	DWORD A = (wIn & 0xF000) >> 12;
	DWORD R = (wIn & 0x0F00) >> 8;
	DWORD G = (wIn & 0x00F0) >> 4;
	DWORD B = (wIn & 0x000F);

	static float f = 255 / 15.0F;
	A = (DWORD)(A*f);
	R = (DWORD)(R*f);
	G = (DWORD)(G*f);
	B = (DWORD)(B*f);

	DWORD dwOut = (DWORD)(A<<24 | R<<16 | G<<8 | B);
	return dwOut;
}






void  BoundingBox_Extract(D3DXVECTOR3* pPositions,GraphicsStruct::_BoundingBox* pBox)
{
	
	float X_D = pBox->PositionA.x;
	float Y_D = pBox->PositionA.y;
	float Z_D = pBox->PositionA.z;
	float X_U = pBox->PositionB.x;
	float Y_U = pBox->PositionB.y;
	float Z_U = pBox->PositionB.z;
	
	pPositions[0] = D3DXVECTOR3(X_D,Y_U,Z_D);//A1
	pPositions[1] = D3DXVECTOR3(X_U,Y_U,Z_D);//B1
	pPositions[2] = D3DXVECTOR3(X_U,Y_U,Z_U);//C1
	pPositions[3] = D3DXVECTOR3(X_D,Y_U,Z_U);//D1
	pPositions[4] = D3DXVECTOR3(X_D,Y_D,Z_D);//A2
	pPositions[5] = D3DXVECTOR3(X_U,Y_D,Z_D);//B2
	pPositions[6] = D3DXVECTOR3(X_U,Y_D,Z_U);//C2
	pPositions[7] = D3DXVECTOR3(X_D,Y_D,Z_U);//D2
	return;
}

/*void  Box_Extract(D3DXVECTOR3* pPositions,
				  PH_BoneTable::_Bone * pBone,
				  PH_BoneTable::_Bound * pBox)
{
	D3DXVECTOR3 A1,B1,D1,A2,A2A1;
	pBone->GetBoundBoxA1(pBox,&A1);
	pBone->GetBoundBoxB1(pBox,&B1);
	pBone->GetBoundBoxD1(pBox,&D1);
	pBone->GetBoundBoxA2(pBox,&A2);
	A2A1 = A2-A1;

	pPositions[0] = A1;      //A1
	pPositions[1] = B1;      //B1
	pPositions[2] = B1+D1-A1;//C1
	pPositions[3] = D1;      //D1
	pPositions[4] = A2;      //A2
	pPositions[5] = B1+A2A1; //B2
	pPositions[6] = pPositions[2]+A2A1; //C2
	pPositions[7] = D1+A2A1; //D2
	
	return;
}*/

void  BoundingBox_TransformCoord(D3DXVECTOR3* pPositions,GraphicsStruct::_BoundingBox* pBox,D3DXMATRIX * pMatrix)
{
	BoundingBox_Extract(pPositions,pBox);
	for(DWORD i=0;i<8;i++)
	{
		D3DXVec3TransformCoord(&pPositions[i],&pPositions[i],pMatrix);
	}
	return;
}
void BoundingBox_Project(D3DXVECTOR3* pPositions,GraphicsStruct::_BoundingBox* pBox,
						 D3DVIEWPORT9 *pViewport,D3DXMATRIX *pProjection,
						 D3DXMATRIX *pView,D3DXMATRIX *pWorld)
{
	BoundingBox_Extract(pPositions,pBox);
	for(DWORD i=0;i<8;i++)
	{
		D3DXVec3Project(&pPositions[i],&pPositions[i],
			pViewport,pProjection,pView,pWorld);
	}
}

BOOL  IsPositionInViewport(D3DXVECTOR3* pPosition,D3DVIEWPORT9 *pViewport)
{
	return TRUE;
}

BOOL  IsBoundingBoxIntersected(GraphicsStruct::_BoundingBox* pBox1,GraphicsStruct::_BoundingBox* pBox2)
{
	if (((pBox1->PositionA.x>=pBox2->PositionA.x)&&(pBox1->PositionA.x<=pBox2->PositionB.x))||
		((pBox2->PositionA.x>=pBox1->PositionA.x)&&(pBox2->PositionA.x<=pBox1->PositionB.x)))
	{
		if (((pBox1->PositionA.z>=pBox2->PositionA.z)&&(pBox1->PositionA.z<=pBox2->PositionB.z))||
			((pBox2->PositionA.z>=pBox1->PositionA.z)&&(pBox2->PositionA.z<=pBox1->PositionB.z)))
		{
			if (((pBox1->PositionA.y>=pBox2->PositionA.y)&&(pBox1->PositionA.y<=pBox2->PositionB.y))||
				((pBox2->PositionA.y>=pBox1->PositionA.y)&&(pBox2->PositionA.y<=pBox1->PositionB.y)))
				return TRUE;
		}
	}
	return FALSE;
}

BOOL  IsBoundingBoxInView(GraphicsStruct::_BoundingBox* pBox,
						  D3DVIEWPORT9 *pViewport,D3DXMATRIX *pProjection,
						  D3DXMATRIX *pView,D3DXMATRIX *pWorld)
{
	D3DXVECTOR3 pPositions[8];
	BoundingBox_Project(pPositions,pBox,pViewport,pProjection,pView,pWorld);
	D3DXVECTOR2 A,B;
	A = B = D3DXVECTOR2(0,0);
	int t = 0;
	for(int i=0;i<8;i++)
	{
		//if ((pPositions[i].z>=0)&&(pPositions[i].z<=1))
		if (pPositions[i].z>=0)
		{
			if (t==0)
			{
				A = B = D3DXVECTOR2(pPositions[0].x,pPositions[0].y);
				t++;
			}
		if (pPositions[i].x<A.x) A.x = pPositions[i].x;
		if (pPositions[i].y<A.y) A.y = pPositions[i].y;

		if (pPositions[i].x>B.x) B.x = pPositions[i].x;
		if (pPositions[i].y>B.y) B.y = pPositions[i].y;
		}
	}
	
	if (t==0)
	{
		return FALSE;
	}
	D3DXVECTOR2 View_A,View_B;
	View_A = D3DXVECTOR2((float)pViewport->X,(float)pViewport->Y);
	View_B = D3DXVECTOR2((float)pViewport->X+pViewport->Width,
		                 (float)pViewport->Y+pViewport->Height);

	if ((B.x>=View_A.x)&&(A.x<=View_B.x)&&(A.y<=View_B.y)&&(B.y>=View_A.y))
		return TRUE;
	else
		return FALSE;
}

HRESULT  GetViewVector(D3DXVECTOR3* pPosition,D3DXVECTOR3* pDirection,D3DXMATRIX *pView)
{
	D3DXVECTOR3 Pos = D3DXVECTOR3(0,0,0);
	
	D3DXVECTOR3 AxisX,AxisY,AxisZ;
	float lenX,lenY,lenZ;

	AxisX.x = pView->_11;
	AxisX.y = pView->_21;
	AxisX.z = pView->_31;
	lenX    = - pView->_41;
	
	AxisY.x = pView->_12;
	AxisY.y = pView->_22;
	AxisY.z = pView->_32;
	lenY    = - pView->_42;
	
	AxisZ.x = pView->_13;
	AxisZ.y = pView->_23;
	AxisZ.z = pView->_33;
	lenZ    = - pView->_43;

	(*pPosition) = AxisX*lenX + AxisY*lenY + AxisZ*lenZ;
	(*pDirection) = AxisZ;
	return S_OK;
}

BOOL IsPointInBoundingRect(D3DXVECTOR2* pPoint, GraphicsStruct::_BoundingRect* pRect)
{
	if ((pPoint->x>=pRect->P1.x)&&(pPoint->x<=pRect->P2.x)&&(pPoint->y>=pRect->P1.y)&&(pPoint->y<=pRect->P2.y))
		return TRUE;
	else
		return FALSE;
}

BOOL IsBoxIntersected(GraphicsStruct::_BoundingBox* pBox1,GraphicsStruct::_BoundingBox* pBox2,
					  D3DXMATRIX *pMat1  ,   D3DXMATRIX *pMat2)
{
	D3DXVECTOR3  Inter;
	D3DXVECTOR3  Pos1[8];
	D3DXVECTOR3  Pos2[8];
	BoundingBox_TransformCoord(Pos1,pBox1,pMat1); 
	BoundingBox_TransformCoord(Pos2,pBox2,pMat2); 
	GraphicsStruct::_BoundingBox NewBox1,NewBox2;
	D3DXComputeBoundingBox(Pos1,8,sizeof(D3DXVECTOR3),
		&NewBox1.PositionA,&NewBox1.PositionB);
	D3DXComputeBoundingBox(Pos2,8,sizeof(D3DXVECTOR3),
		&NewBox2.PositionA,&NewBox2.PositionB);
	if (IsBoundingBoxIntersected(&NewBox1,&NewBox2)) 
		return TRUE;
	return FALSE;
	/*
	for(int i=0;i<8;i++)
	{
		if (IsPointInBox(&Pos1[i],Pos2))
			return TRUE;
	}
	for(i=0;i<8;i++)
	{
		if (IsPointInBox(&Pos2[i],Pos1))
			return TRUE;
	}
	////////////////////////////////////////////////////////////////////////////////
	if (IsLineIntersectBox(&Inter,&Pos1[BOX_A1],&Pos1[BOX_B1],Pos2))
		return TRUE;
	if (IsLineIntersectBox(&Inter,&Pos1[BOX_B1],&Pos1[BOX_C1],Pos2))
		return TRUE;
	if (IsLineIntersectBox(&Inter,&Pos1[BOX_C1],&Pos1[BOX_D1],Pos2))
		return TRUE;
	if (IsLineIntersectBox(&Inter,&Pos1[BOX_D1],&Pos1[BOX_A1],Pos2))
		return TRUE;
	if (IsLineIntersectBox(&Inter,&Pos1[BOX_A2],&Pos1[BOX_B2],Pos2))
		return TRUE;
	if (IsLineIntersectBox(&Inter,&Pos1[BOX_B2],&Pos1[BOX_C2],Pos2))
		return TRUE;
	if (IsLineIntersectBox(&Inter,&Pos1[BOX_C2],&Pos1[BOX_D2],Pos2))
		return TRUE;
	if (IsLineIntersectBox(&Inter,&Pos1[BOX_D2],&Pos1[BOX_A2],Pos2))
		return TRUE;
	if (IsLineIntersectBox(&Inter,&Pos1[BOX_A1],&Pos1[BOX_A2],Pos2))
		return TRUE;
	if (IsLineIntersectBox(&Inter,&Pos1[BOX_B1],&Pos1[BOX_B2],Pos2))
		return TRUE;
	if (IsLineIntersectBox(&Inter,&Pos1[BOX_C1],&Pos1[BOX_C2],Pos2))
		return TRUE;
	if (IsLineIntersectBox(&Inter,&Pos1[BOX_D1],&Pos1[BOX_D2],Pos2))
		return TRUE;
	////////////////////////////////////////////////////////////////////////////////
	if (IsLineIntersectBox(&Inter,&Pos2[BOX_A1],&Pos2[BOX_B1],Pos1))
		return TRUE;
	if (IsLineIntersectBox(&Inter,&Pos2[BOX_B1],&Pos2[BOX_C1],Pos1))
		return TRUE;
	if (IsLineIntersectBox(&Inter,&Pos2[BOX_C1],&Pos2[BOX_D1],Pos1))
		return TRUE;
	if (IsLineIntersectBox(&Inter,&Pos2[BOX_D1],&Pos2[BOX_A1],Pos1))
		return TRUE;
	if (IsLineIntersectBox(&Inter,&Pos2[BOX_A2],&Pos2[BOX_B2],Pos1))
		return TRUE;
	if (IsLineIntersectBox(&Inter,&Pos2[BOX_B2],&Pos2[BOX_C2],Pos1))
		return TRUE;
	if (IsLineIntersectBox(&Inter,&Pos2[BOX_C2],&Pos2[BOX_D2],Pos1))
		return TRUE;
	if (IsLineIntersectBox(&Inter,&Pos2[BOX_D2],&Pos2[BOX_A2],Pos1))
		return TRUE;
	if (IsLineIntersectBox(&Inter,&Pos2[BOX_A1],&Pos2[BOX_A2],Pos1))
		return TRUE;
	if (IsLineIntersectBox(&Inter,&Pos2[BOX_B1],&Pos2[BOX_B2],Pos1))
		return TRUE;
	if (IsLineIntersectBox(&Inter,&Pos2[BOX_C1],&Pos2[BOX_C2],Pos1))
		return TRUE;
	if (IsLineIntersectBox(&Inter,&Pos2[BOX_D1],&Pos2[BOX_D2],Pos1))
		return TRUE;
	
	return FALSE;*/
}

BOOL IsParallelogramIntersectBox(D3DXVECTOR3* pPA,D3DXVECTOR3* pPB,D3DXVECTOR3* pPD,
								 D3DXVECTOR3* pBoxPos)
{
	
	D3DXVECTOR3 Inter;
	if (IsParallelogramIntersectParallelogram(&Inter,pPA,pPB,pPD,
		&pBoxPos[BOX_A1],&pBoxPos[BOX_B1],&pBoxPos[BOX_C1]))
		return TRUE;
	if (IsParallelogramIntersectParallelogram(&Inter,pPA,pPB,pPD,
		&pBoxPos[BOX_A2],&pBoxPos[BOX_B2],&pBoxPos[BOX_C2]))
		return TRUE;
	if (IsParallelogramIntersectParallelogram(&Inter,pPA,pPB,pPD,
		&pBoxPos[BOX_A1],&pBoxPos[BOX_A2],&pBoxPos[BOX_B1]))
		return TRUE;
	if (IsParallelogramIntersectParallelogram(&Inter,pPA,pPB,pPD,
		&pBoxPos[BOX_D1],&pBoxPos[BOX_D2],&pBoxPos[BOX_C1]))
		return TRUE;
	if (IsParallelogramIntersectParallelogram(&Inter,pPA,pPB,pPD,
		&pBoxPos[BOX_A1],&pBoxPos[BOX_A2],&pBoxPos[BOX_D1]))
		return TRUE;
	if (IsParallelogramIntersectParallelogram(&Inter,pPA,pPB,pPD,
		&pBoxPos[BOX_B1],&pBoxPos[BOX_B2],&pBoxPos[BOX_C1]))
		return TRUE;
	return FALSE;
}

BOOL IsPointInBox(D3DXVECTOR3* Pos,D3DXVECTOR3* BoxPos)
{
	D3DXVECTOR3 VA = *Pos - BoxPos[BOX_A1];
	D3DXVECTOR3 B1A1 = BoxPos[BOX_B1] - BoxPos[BOX_A1];
	D3DXVECTOR3 D1A1 = BoxPos[BOX_D1] - BoxPos[BOX_A1];
	D3DXVECTOR3 A2A1 = BoxPos[BOX_A2] - BoxPos[BOX_A1];
	
	float lb = D3DXVec3Dot(&VA,&B1A1);
	float lc = D3DXVec3Dot(&VA,&D1A1);
	float ld = D3DXVec3Dot(&VA,&A2A1);
	float llb = D3DXVec3Dot(&B1A1,&B1A1);
	float llc = D3DXVec3Dot(&D1A1,&D1A1);
	float lld = D3DXVec3Dot(&A2A1,&A2A1);
	
	if ((lb>=0)&&(lb<=llb)&&(lc>=0)&&(lc<=llc)&&(ld>=0)&&(ld<=lld))
		return TRUE;
	else 
		return FALSE;
}

BOOL IsPointInBox(D3DXVECTOR3* pFacesNormal,float * pDistance,D3DXVECTOR3* Pos,D3DXVECTOR3* BoxPos)
{
	D3DXVECTOR3 VA = *Pos - BoxPos[BOX_A1];
	D3DXVECTOR3 B1A1 = BoxPos[BOX_B1] - BoxPos[BOX_A1];
	D3DXVECTOR3 D1A1 = BoxPos[BOX_D1] - BoxPos[BOX_A1];
	D3DXVECTOR3 A2A1 = BoxPos[BOX_A2] - BoxPos[BOX_A1];
	
	float lb = D3DXVec3Dot(&VA,&B1A1);
	float lc = D3DXVec3Dot(&VA,&D1A1);
	float ld = D3DXVec3Dot(&VA,&A2A1);
	float llb = D3DXVec3Dot(&B1A1,&B1A1);
	float llc = D3DXVec3Dot(&D1A1,&D1A1);
	float lld = D3DXVec3Dot(&A2A1,&A2A1);
	int Min = 0;
	float Len[6];
	Len[0] = lb;
	Len[1] = lc;
	Len[2] = ld;
	Len[3] = llb - lb;
	Len[4] = llc - lc;
	Len[5] = lld - ld;

	if ((lb>=0)&&(lb<=llb)&&(lc>=0)&&(lc<=llc)&&(ld>=0)&&(ld<=lld))
	{
		float MinL = 10000;
		for(DWORD i=0;i<6;i++)
		{
			if (Len[i]<MinL)
			{
				MinL = Len[i];
				Min = i;
			}
		}
		switch(Min)
		{
		case 0:
			{
				D3DXVec3Normalize(pFacesNormal,&(B1A1));
				(*pDistance) = Len[0];
				break;
			}
		case 1:
			{
				D3DXVec3Normalize(pFacesNormal,&(D1A1));
				(*pDistance) = Len[1];
				break;
			}
		case 2:
			{
				D3DXVec3Normalize(pFacesNormal,&(A2A1));
				(*pDistance) = Len[2];
				break;
			}
		case 3:
			{
				D3DXVec3Normalize(pFacesNormal,&(-B1A1));
				(*pDistance) = Len[3];
				break;
			}
		case 4:
			{
				D3DXVec3Normalize(pFacesNormal,&(-D1A1));
				(*pDistance) = Len[4];
				break;
			}
		case 5:
			{
				D3DXVec3Normalize(pFacesNormal,&(-A2A1));
				(*pDistance) = Len[5];
				break;
			}
		}
		return TRUE;
	}
	else 
		return FALSE;
}

BOOL IsPointInBox(D3DXVECTOR3 * pInterFaceNormal,D3DXVECTOR3* pInter,
				  D3DXVECTOR3* Pos,D3DXVECTOR3* pSpeed,D3DXVECTOR3* BoxPos)
{
	D3DXVECTOR3* LineA = Pos;
	D3DXVECTOR3* LineB = &(*Pos-*pSpeed*100);
	D3DXVECTOR3 O  = (BoxPos[BOX_A1]+BoxPos[BOX_A2])*0.5f;
	D3DXVECTOR3 VO = *Pos - O;
	D3DXVECTOR3 B1O = BoxPos[BOX_B1] - O;
	D3DXVECTOR3 D1O = BoxPos[BOX_D1] - O;
	D3DXVECTOR3 A1O = BoxPos[BOX_A1] - O;



	D3DXPLANE Plane;
	
	float lb = D3DXVec3Dot(&VO,&B1O);
	float lc = D3DXVec3Dot(&VO,&D1O);
	float la = D3DXVec3Dot(&VO,&A1O);
	float llb = D3DXVec3Dot(&B1O,&B1O);
	float llc = D3DXVec3Dot(&D1O,&D1O);
	float lla = D3DXVec3Dot(&A1O,&A1O);

	if ((lb>-llb)&&(lb<llb)&&(lc>-llc)&&(lc<llc)&&(la>-lla)&&(la<lla))
	{
		if (IsLineIntersectParallelogram(pInter,LineA,LineB,
			&BoxPos[BOX_A1],&BoxPos[BOX_B1],&BoxPos[BOX_D1]))
		{
			D3DXVECTOR3 IO = *pInter - O;
			float L = D3DXVec3Dot(&IO,&A1O);
			if (L<0)
				D3DXVec3Normalize(pInterFaceNormal,&(A1O));
			else
				D3DXVec3Normalize(pInterFaceNormal,&(-A1O));
			return TRUE;
		}
		else if (IsLineIntersectParallelogram(pInter,LineA,LineB,
			&BoxPos[BOX_A2],&BoxPos[BOX_B2],&BoxPos[BOX_D2]))
		{
			D3DXVECTOR3 IO = *pInter - O;
			float L = D3DXVec3Dot(&IO,&A1O);
			if (L>0)
				D3DXVec3Normalize(pInterFaceNormal,&(A1O));
			else
				D3DXVec3Normalize(pInterFaceNormal,&(-A1O));
			return TRUE;
		}

		else if (IsLineIntersectParallelogram(pInter,LineA,LineB,
			&BoxPos[BOX_A1],&BoxPos[BOX_A2],&BoxPos[BOX_B1]))
		{
			D3DXVECTOR3 IO = *pInter - O;
			float L = D3DXVec3Dot(&IO,&D1O);
			if (L>0)
				D3DXVec3Normalize(pInterFaceNormal,&(D1O));
			else
				D3DXVec3Normalize(pInterFaceNormal,&(-D1O));
			return TRUE;
		}
		else if (IsLineIntersectParallelogram(pInter,LineA,LineB,
			&BoxPos[BOX_D1],&BoxPos[BOX_C1],&BoxPos[BOX_D2]))
		{
			D3DXVECTOR3 IO = *pInter - O;
			float L = D3DXVec3Dot(&IO,&D1O);
			if (L<0)
				D3DXVec3Normalize(pInterFaceNormal,&(D1O));
			else
				D3DXVec3Normalize(pInterFaceNormal,&(-D1O));
			return TRUE;
		}

		else if (IsLineIntersectParallelogram(pInter,LineA,LineB,
			&BoxPos[BOX_A1],&BoxPos[BOX_D1],&BoxPos[BOX_A2]))
		{
			D3DXVECTOR3 IO = *pInter - O;
			float L = D3DXVec3Dot(&IO,&B1O);
			if (L>0)
				D3DXVec3Normalize(pInterFaceNormal,&(B1O));
			else
				D3DXVec3Normalize(pInterFaceNormal,&(-B1O));
			return TRUE;
		}
		else if (IsLineIntersectParallelogram(pInter,LineA,LineB,
			&BoxPos[BOX_B1],&BoxPos[BOX_B2],&BoxPos[BOX_C1]))
		{
			D3DXVECTOR3 IO = *pInter - O;
			float L = D3DXVec3Dot(&IO,&B1O);
			if (L<0)
				D3DXVec3Normalize(pInterFaceNormal,&(B1O));
			else
				D3DXVec3Normalize(pInterFaceNormal,&(-B1O));
			return TRUE;
		}
		else
			return FALSE;
	}
	else 
		return FALSE;
}


BOOL IsLineIntersectBox(D3DXVECTOR3* Inter,D3DXVECTOR3* LineA,D3DXVECTOR3* LineB,D3DXVECTOR3* BoxPos)
{
	if (IsLineIntersectParallelogram(Inter,LineA,LineB,
		&BoxPos[BOX_A1],&BoxPos[BOX_B1],&BoxPos[BOX_D1]))
		return TRUE;
	if (IsLineIntersectParallelogram(Inter,LineA,LineB,
		&BoxPos[BOX_A2],&BoxPos[BOX_B2],&BoxPos[BOX_D2]))
		return TRUE;
	if (IsLineIntersectParallelogram(Inter,LineA,LineB,
		&BoxPos[BOX_A1],&BoxPos[BOX_A2],&BoxPos[BOX_B1]))
		return TRUE;
	if (IsLineIntersectParallelogram(Inter,LineA,LineB,
		&BoxPos[BOX_D1],&BoxPos[BOX_D2],&BoxPos[BOX_C1]))
		return TRUE;
	if (IsLineIntersectParallelogram(Inter,LineA,LineB,
		&BoxPos[BOX_A1],&BoxPos[BOX_A2],&BoxPos[BOX_D1]))
		return TRUE;
	if (IsLineIntersectParallelogram(Inter,LineA,LineB,
		&BoxPos[BOX_B1],&BoxPos[BOX_B2],&BoxPos[BOX_C1]))
		return TRUE;
	return FALSE;

}

/*BOOL  IsShooted(D3DXVECTOR3* Inter,DO_DataObjectTable::_DataObject* pObject,DO_DataObjectTable::_DataObject * pBullet)
{
	D3DXVECTOR3  Pos1[8];
	BoundingBox_TransformCoord(Pos1,&pObject->BoundingBox,&pObject->m_Transformation.Matrix);
	D3DXVECTOR3 A,B;
	A = pBullet->m_Transformation.Position;
	B = pBullet->m_Transformation.Position_Sou;
	if (IsLineIntersectBox(Inter,&A,&B,Pos1))
	{
		return TRUE;
	}
	return FALSE;
}*/

HRESULT  GetViewDirection(D3DXVECTOR3* pDirection,D3DXMATRIX *pView)
{
	D3DXVECTOR3 AxisZ;
	AxisZ.x = pView->_13;
	AxisZ.y = pView->_23;
	AxisZ.z = pView->_33;

	(*pDirection) = AxisZ;
	return S_OK;
}

/*int __cdecl ObjectSortCB(const VOID *arg1, const VOID *arg2)
{
	DWORD* ObjectA_ID = (DWORD*) arg1;
	DWORD* ObjectB_ID = (DWORD*) arg2;

	DO_DataObjectTable::_DataObject * pObjectA = NULL; 
	DO_DataObjectTable::_DataObject * pObjectB = NULL; 
    
	if (FAILED(g_pDataObjectTable->GetObject(*ObjectA_ID,&pObjectA)))
		return -1;
	if (FAILED(g_pDataObjectTable->GetObject(*ObjectB_ID,&pObjectB)))
		return -1;

    FLOAT d1 = pObjectA->m_Transformation.Position.x * g_ViewDirection.x +
		       pObjectA->m_Transformation.Position.z * g_ViewDirection.z;
 
	FLOAT d2 = pObjectB->m_Transformation.Position.x * g_ViewDirection.x +
		       pObjectB->m_Transformation.Position.z * g_ViewDirection.z;
    if (d1 < d2)
        return +1;
    return -1;
}*/

D3DXVECTOR3   AimPosition(D3DXVECTOR3* Pos,D3DXVECTOR3* Aim)
{
	D3DXVECTOR3 Out = D3DXVECTOR3(0,0,0);
	D3DXVECTOR3 Dir = *Aim - *Pos;
	D3DXVECTOR2 XZ = D3DXVECTOR2(Dir.x,Dir.z);
	D3DXVec2Normalize(&XZ,&XZ);
	D3DXVec3Normalize(&Dir,&Dir);
	float alpha = 0;
	if (XZ.y<0)
	{
		alpha = acosf(XZ.x);
	}
	else
	{
		alpha = 2*3.14159f - acosf(XZ.x);
	}
	float beta = 0;
	{
		beta = asinf(Dir.y);
	}

	D3DXVec3Normalize(&Dir,&Dir);
	Out.x = alpha - 3.14159f/2;
	Out.y = beta;
	return Out;
}

BOOL  Collision_Sphere_Box(GraphicsStruct::_Force* pForceA,GraphicsStruct::_Force* pForceB,
						   D3DXVECTOR3* S_Center,float S_Radiu,
						   D3DXVECTOR3* BoxPositions)
{

/*	D3DXVECTOR3 Box_Center = (BoxPositions[BOX_C2] + BoxPositions[BOX_A1]) * 0.5f;
	D3DXVECTOR3 C_C = *S_Center - Box_Center;

	D3DXVECTOR3 Axis1 = (BoxPositions[BOX_A2] - BoxPositions[BOX_A1]) * 0.5f;
	D3DXVECTOR3 Axis2 = (BoxPositions[BOX_B1] - BoxPositions[BOX_A1]) * 0.5f;
	D3DXVECTOR3 Axis3 = (BoxPositions[BOX_D1] - BoxPositions[BOX_A1]) * 0.5f;

	D3DXVECTOR3 NAxis1,NAxis2,NAxis3;
	D3DXVECTOR3 LAxis1,LAxis2,LAxis3;

	D3DXVec3Normalize(&NAxis1,&Axis1);
	D3DXVec3Normalize(&NAxis2,&Axis2);
	D3DXVec3Normalize(&NAxis3,&Axis3);

	LAxis1 = Axis1 + NAxis1*S_Radiu;
	LAxis2 = Axis2 + NAxis2*S_Radiu;
	LAxis3 = Axis3 + NAxis3*S_Radiu;

	float L1 = D3DXVec3Dot(&LAxis1,&C_C);
	float L2 = D3DXVec3Dot(&LAxis2,&C_C);
	float L3 = D3DXVec3Dot(&LAxis3,&C_C);

	float LL1 = D3DXVec3Dot(&LAxis1,&LAxis1);
	float LL2 = D3DXVec3Dot(&LAxis2,&LAxis2);
	float LL3 = D3DXVec3Dot(&LAxis3,&LAxis3);

	if ((L1<=LL1)&&(L1>=-LL1)&&(L2>=-LL2)&&(L2<=LL2)&&(L3>=-LL3)&&(L3<=LL3))
	{
		float SL1 = D3DXVec3Dot(&Axis1,&C_C);
		float SL2 = D3DXVec3Dot(&Axis2,&C_C);
		float SL3 = D3DXVec3Dot(&Axis3,&C_C);
		float SLL1 = D3DXVec3Dot(&Axis1,&Axis1);
		float SLL2 = D3DXVec3Dot(&Axis2,&Axis2);
		float SLL3 = D3DXVec3Dot(&Axis3,&Axis3);

		if ((SL1>=-1.0f*SLL1)&&(SL1<=SLL1))
		{
			if ((SL2>=-1.0f*SLL2)&&(SL2<=SLL2))
			{
				D3DXVECTOR3 Inter;
				D3DXPLANE   plane;
				if (SL3>=0)
				{
					D3DXPlaneFromPoints(&plane,
						&BoxPositions[BOX_D1],&BoxPositions[BOX_C1],&BoxPositions[BOX_C2]);
					if (D3DXPlaneIntersectLine(&Inter,&plane,S_Center,&(*S_Center+NAxis3))!=NULL)
					{
						pForceA->Force = *S_Center - Inter;
						pForceA->Position = Inter;
						pForceA->Time = 0;
						pForceA->Type = TYPE_FORCE_IMPULSE;
						pForceB->Force = - pForceA->Force;
						pForceB->Position = pForceA->Position;
						pForceB->Time = 0;
						pForceB->Type = TYPE_FORCE_IMPULSE;
						return TRUE;
					}
				}
				else
				{
					D3DXPlaneFromPoints(&plane,
						&BoxPositions[BOX_A1],&BoxPositions[BOX_B1],&BoxPositions[BOX_B2]);
					if (D3DXPlaneIntersectLine(&Inter,&plane,S_Center,&(*S_Center+NAxis3))!=NULL)
					{
						pForceA->Force = *S_Center - Inter;
						pForceA->Position = Inter;
						pForceA->Time = 0;
						pForceA->Type = TYPE_FORCE_IMPULSE;
						pForceB->Force = - pForceA->Force;
						pForceB->Position = pForceA->Position;
						pForceB->Time = 0;
						pForceB->Type = TYPE_FORCE_IMPULSE;
						return TRUE;
					}
				}
			}
			else if ((SL3>=-SLL3)&&(SL3<=SLL3))
			{
				D3DXVECTOR3 Inter;
				D3DXPLANE   plane;
				if (SL2>=0)
				{
					D3DXPlaneFromPoints(&plane,
						&BoxPositions[BOX_B1],&BoxPositions[BOX_B1],&BoxPositions[BOX_B2]);
					if (D3DXPlaneIntersectLine(&Inter,&plane,S_Center,&(*S_Center+NAxis2))!=NULL)
					{
						pForceA->Force = *S_Center - Inter;
						pForceA->Position = Inter;
						pForceA->Time = 0;
						pForceA->Type = TYPE_FORCE_IMPULSE;
						pForceB->Force = - pForceA->Force;
						pForceB->Position = pForceA->Position;
						pForceB->Time = 0;
						pForceB->Type = TYPE_FORCE_IMPULSE;
						return TRUE;
					}
				}
				else
				{
					D3DXPlaneFromPoints(&plane,
						&BoxPositions[BOX_A1],&BoxPositions[BOX_D1],&BoxPositions[BOX_A2]);
					if (D3DXPlaneIntersectLine(&Inter,&plane,S_Center,&(*S_Center+NAxis2))!=NULL)
					{
						pForceA->Force = *S_Center - Inter;
						pForceA->Position = Inter;
						pForceA->Time = 0;
						pForceA->Type = TYPE_FORCE_IMPULSE;
						pForceB->Force = - pForceA->Force;
						pForceB->Position = pForceA->Position;
						pForceB->Time = 0;
						pForceB->Type = TYPE_FORCE_IMPULSE;
						return TRUE;
					}
				}
			}
		}
		else
		{
			D3DXVECTOR3 Inter;
			D3DXPLANE   plane;
			if (SL1<=0)
			{
				D3DXPlaneFromPoints(&plane,
					&BoxPositions[BOX_A1],&BoxPositions[BOX_B1],&BoxPositions[BOX_C1]);
				if (D3DXPlaneIntersectLine(&Inter,&plane,S_Center,&(*S_Center+NAxis1))!=NULL)
				{
					pForceA->Force = *S_Center - Inter;
					pForceA->Position = Inter;
					pForceA->Time = 0;
					pForceA->Type = TYPE_FORCE_IMPULSE;
					pForceB->Force = - pForceA->Force;
					pForceB->Position = pForceA->Position;
					pForceB->Time = 0;
					pForceB->Type = TYPE_FORCE_IMPULSE;
					return TRUE;
				}
			}
			else
			{
				D3DXPlaneFromPoints(&plane,
					&BoxPositions[BOX_A2],&BoxPositions[BOX_B2],&BoxPositions[BOX_C2]);
				if (D3DXPlaneIntersectLine(&Inter,&plane,S_Center,&(*S_Center+NAxis1))!=NULL)
				{
					pForceA->Force = *S_Center - Inter;
					pForceA->Position = Inter;
					pForceA->Time = 0;
					pForceA->Type = TYPE_FORCE_IMPULSE;
					pForceB->Force = - pForceA->Force;
					pForceB->Position = pForceA->Position;
					pForceB->Time = 0;
					pForceB->Type = TYPE_FORCE_IMPULSE;
					return TRUE;
				}
			}
		}
	}
	else 
		return FALSE;*/
	return FALSE;
}

BOOL GetOppositePosition_Box_Position(D3DXVECTOR3* OppVec,
											  D3DXVECTOR3* BoxPos,
											  D3DXVECTOR3* Positions)
{
	D3DXVECTOR3 O = (BoxPos[BOX_A1]+BoxPos[BOX_A2])*0.5f;
	D3DXVECTOR3 PO = *Positions - O;
	D3DXVECTOR3 B1O = BoxPos[BOX_B1] - O;
	D3DXVECTOR3 D1O = BoxPos[BOX_D1] - O;
	D3DXVECTOR3 A1O = BoxPos[BOX_A1] - O;
	
	*OppVec = PO;

	float lb  = D3DXVec3Dot(&PO,&B1O);
	float ld  = D3DXVec3Dot(&PO,&D1O);
	float la  = D3DXVec3Dot(&PO,&A1O);
	float llb = D3DXVec3Dot(&B1O,&B1O);
	float lld = D3DXVec3Dot(&D1O,&D1O);
	float lla = D3DXVec3Dot(&A1O,&A1O);
	float As  = llb/lld;
	float AsO = lb / ld;

	if ((AsO<=-As)&&(AsO>=As))//
	{
		if ((la<=-lla)&&(la>=lla)&&
		   (lb>=-llb)&&(lb<=llb)&&
		   (ld>=-lld)&&(ld<=lld))
		{
			*OppVec = la*A1O;
			return TRUE;
		}
		else
		{
			*OppVec = lb*B1O;
			return TRUE;
		}
	}
	else
	{
		if ((la<=-lla)&&(la>=lla)&&
		   (lb>=-llb)&&(lb<=llb)&&
		   (ld>=-lld)&&(ld<=lld))
		{
			*OppVec = la*A1O;
			return TRUE;
		}
		else
		{
			*OppVec = ld*D1O;
			return TRUE;
		}
	}

}

BOOL  Collision_Box_Box(GraphicsStruct::_Force* pForceA,GraphicsStruct::_Force* pForceB,
						   D3DXVECTOR3* pIntersect,
						   D3DXVECTOR3* SpeedA,
						   D3DXVECTOR3* SpeedB,
						   D3DXVECTOR3* BoxPositionsA,
						   D3DXVECTOR3* BoxPositionsB)
{
	D3DXVECTOR3  Inter;
	DWORD K=0;
	D3DXVECTOR3  InterPoint[16];
	float        InPoint   [16];
	D3DXVECTOR3  InterFaceNormal;
	D3DXVECTOR3  InterNormal[16];
	D3DXVECTOR3  CenterA = (BoxPositionsA[BOX_A1]+BoxPositionsA[BOX_A2])*0.5f;
	D3DXVECTOR3  CenterB = (BoxPositionsB[BOX_A1]+BoxPositionsB[BOX_A2])*0.5f;
	D3DXVECTOR3  TestDirection;
	
	/*GetOppositePosition_Box_Position(&TestDirection,
		BoxPositionsB,
		&CenterA);*/
	for(int i=0;i<8;i++)
	{
		/*if (IsPointInBox(&InterFaceNormal,&Inter,
			&BoxPositionsA[i],
			&(-TestDirection),
			BoxPositionsB))*/
		float Dis = 0;
		if (IsPointInBox(&InterFaceNormal,&Dis,
			&BoxPositionsA[i],
			BoxPositionsB))
		{		
			InterPoint [K] = BoxPositionsA[i];
			InterNormal[K] = InterFaceNormal;
			InPoint    [K] = Dis;
 			K++;
		}
	}
	if (K>0)
	{
		D3DXVECTOR3 Position = D3DXVECTOR3(0,0,0);
		float In       = 0;
		Inter = D3DXVECTOR3(0,0,0);
		for(DWORD T=0;T<K;T++)
		{
			Position += InterPoint[T];
			Inter    += InterNormal[T];
			In       += InPoint[T];
		}
		Position /= (float)K; 
		Inter    /= (float)K; 
		In       /= (float)K; 
		pForceA->Force = Inter;
		pForceA->Position = Position;
		pForceA->Time = 1;
		/*pForceA->Type = TYPE_FORCE_IMPULSE;
		pForceB->Force = - pForceA->Force;
		pForceB->Position = pForceA->Position;
		pForceB->Time = 1;
		pForceB->Type = TYPE_FORCE_IMPULSE;*/
		(*pIntersect) = D3DXVECTOR3(In,0,0);
		return TRUE;
	}
	K=0;
	for(int i=0;i<8;i++)
	{
		/*GetOppositePosition_Box_Position(&TestDirection,
			BoxPositionsA,
			&(BoxPositionsB[i]+CenterA-CenterB));
		/*if (IsPointInBox(&InterFaceNormal,&Inter,
		      &BoxPositionsB[i],&(-TestDirection),
		      BoxPositionsA))*/
		float Dis = 0;
		if (IsPointInBox(&InterFaceNormal,&Dis,
			&BoxPositionsB[i],
			BoxPositionsA))
		{
			InterPoint [K] = BoxPositionsB[i];
			InterNormal[K] = InterFaceNormal;
			InPoint[K] = Dis;
 			K++;
		}
	}
	if (K>0)
	{
		D3DXVECTOR3 Position = D3DXVECTOR3(0,0,0);
		float In       = 0;
		Inter = D3DXVECTOR3(0,0,0);
		for(DWORD T=0;T<K;T++)
		{
			Position += InterPoint[T];
			Inter    += InterNormal[T];
			In       += InPoint[T];
		}
		Position /= (float)K; 
		Inter    /= (float)K; 
		In       /= (float)K; 
		pForceA->Force = Inter;
		pForceA->Position = Position;
		pForceA->Time = 1;
		/*pForceA->Type = TYPE_FORCE_IMPULSE;
		pForceB->Force = - pForceA->Force;
		pForceB->Position = pForceA->Position;
		pForceB->Time = 1;
		pForceB->Type = TYPE_FORCE_IMPULSE;*/
		(*pIntersect) = D3DXVECTOR3(In,0,0);
		return TRUE;
	}
	////////////////////////////////////////////////////////////////////////////////
	/*if (IsLineIntersectBox(&Inter,&BoxPositionsA[BOX_A1],&BoxPositionsA[BOX_B1],BoxPositionsB))
	{
		InterPoint[K]=Inter;
		K++;
	}
	if (IsLineIntersectBox(&Inter,&BoxPositionsA[BOX_B1],&BoxPositionsA[BOX_C1],BoxPositionsB))
	{
		InterPoint[K]=Inter;
		K++;
	}
	if (IsLineIntersectBox(&Inter,&BoxPositionsA[BOX_C1],&BoxPositionsA[BOX_D1],BoxPositionsB))
	{
		InterPoint[K]=Inter;
		K++;
	}
	if (IsLineIntersectBox(&Inter,&BoxPositionsA[BOX_D1],&BoxPositionsA[BOX_A1],BoxPositionsB))
		return TRUE;
	if (IsLineIntersectBox(&Inter,&BoxPositionsA[BOX_A2],&BoxPositionsA[BOX_B2],BoxPositionsB))
	{
		InterPoint[K]=Inter;
		K++;
	}
	if (IsLineIntersectBox(&Inter,&BoxPositionsA[BOX_B2],&BoxPositionsA[BOX_C2],BoxPositionsB))
	{
		InterPoint[K]=Inter;
		K++;
	}
	if (IsLineIntersectBox(&Inter,&BoxPositionsA[BOX_C2],&BoxPositionsA[BOX_D2],BoxPositionsB))
	{
		InterPoint[K]=Inter;
		K++;
	}
	if (IsLineIntersectBox(&Inter,&BoxPositionsA[BOX_D2],&BoxPositionsA[BOX_A2],BoxPositionsB))
	{
		InterPoint[K]=Inter;
		K++;
	}
	if (IsLineIntersectBox(&Inter,&BoxPositionsA[BOX_A1],&BoxPositionsA[BOX_A2],BoxPositionsB))
	{
		InterPoint[K]=Inter;
		K++;
	}
	if (IsLineIntersectBox(&Inter,&BoxPositionsA[BOX_B1],&BoxPositionsA[BOX_B2],BoxPositionsB))
	{
		InterPoint[K]=Inter;
		K++;
	}
	if (IsLineIntersectBox(&Inter,&BoxPositionsA[BOX_C1],&BoxPositionsA[BOX_C2],BoxPositionsB))
	{
		InterPoint[K]=Inter;
		K++;
	}
	if (IsLineIntersectBox(&Inter,&BoxPositionsA[BOX_D1],&BoxPositionsA[BOX_D2],BoxPositionsB))
	{
		InterPoint[K]=Inter;
		K++;
	}
	if (K>0)
	{
		Inter = D3DXVECTOR3(0,0,0);
		for(DWORD S=0;S<K;S++)
		{
			Inter+=InterPoint[S];
		}
		Inter /= (float)K;
		D3DXPLANE Plane;
		D3DXPlaneFromPoints(&Plane,&InterPoint[0],&InterPoint[1],&InterPoint[2]);
		D3DXVECTOR3 Normal;
		Normal.x = Plane.a;
		Normal.y = Plane.b;
		Normal.z = Plane.c;
	
		pForceA->Force = Normal;
		pForceA->Position = Inter;
		pForceA->Time = 0;
		pForceA->Type = TYPE_FORCE_IMPULSE;
		pForceB->Force = - InterFaceNormal;
		pForceB->Position = pForceA->Position;
		pForceB->Time = 0;
		pForceB->Type = TYPE_FORCE_IMPULSE;

		return TRUE;
	}
	////////////////////////////////////////////////////////////////////////////////
	/*if (IsLineIntersectBox(&Inter,&BoxPositionsB[BOX_A1],&BoxPositionsB[BOX_B1],BoxPositionsA))
		return TRUE;
	if (IsLineIntersectBox(&Inter,&BoxPositionsB[BOX_B1],&BoxPositionsB[BOX_C1],BoxPositionsA))
		return TRUE;
	if (IsLineIntersectBox(&Inter,&BoxPositionsB[BOX_C1],&BoxPositionsB[BOX_D1],BoxPositionsA))
		return TRUE;
	if (IsLineIntersectBox(&Inter,&BoxPositionsB[BOX_D1],&BoxPositionsB[BOX_A1],BoxPositionsA))
		return TRUE;
	if (IsLineIntersectBox(&Inter,&BoxPositionsB[BOX_A2],&BoxPositionsB[BOX_B2],BoxPositionsA))
		return TRUE;
	if (IsLineIntersectBox(&Inter,&BoxPositionsB[BOX_B2],&BoxPositionsB[BOX_C2],BoxPositionsA))
		return TRUE;
	if (IsLineIntersectBox(&Inter,&BoxPositionsB[BOX_C2],&BoxPositionsB[BOX_D2],BoxPositionsA))
		return TRUE;
	if (IsLineIntersectBox(&Inter,&BoxPositionsB[BOX_D2],&BoxPositionsB[BOX_A2],BoxPositionsA))
		return TRUE;
	if (IsLineIntersectBox(&Inter,&BoxPositionsB[BOX_A1],&BoxPositionsB[BOX_A2],BoxPositionsA))
		return TRUE;
	if (IsLineIntersectBox(&Inter,&BoxPositionsB[BOX_B1],&BoxPositionsB[BOX_B2],BoxPositionsA))
		return TRUE;
	if (IsLineIntersectBox(&Inter,&BoxPositionsB[BOX_C1],&BoxPositionsB[BOX_C2],BoxPositionsA))
		return TRUE;
	if (IsLineIntersectBox(&Inter,&BoxPositionsB[BOX_D1],&BoxPositionsB[BOX_D2],BoxPositionsA))
		return TRUE;*/
	
	return FALSE;	;
}

TCHAR* GetFileName(TCHAR* PathName)
{
	if (PathName==NULL) return NULL;
	DWORD LastPosition = 0;
	TCHAR* pChar = PathName;
	DWORD i=0;
	while(pChar[i]!='\0')
	{
		if (pChar[i]=='\\')
		{
			LastPosition = i;
		}
		i++;
	}
	if (LastPosition!=0)
	{
		return &pChar[LastPosition+1];
	}
	else
		return PathName;
}
void RemoveSpace(TCHAR* Str)
{
	if (Str==NULL) return ;
	TCHAR* pChar = Str;
	DWORD i=0;
	while(pChar[i]!='\0')
	{
		if (pChar[i]==' ')
		{
			pChar[i] = '_';
		}
		i++;
	}
}


//-----------------------------------------------------------------------------
// Name: UpdateCullInfo()
// Desc: Sets up the frustum planes, endpoints, and center for the frustum
//       defined by a given view matrix and projection matrix.  This info will 
//       be used when culling each object in CullObject().
//-----------------------------------------------------------------------------
VOID UpdateCullInfo( GraphicsStruct::CULLINFO* pCullInfo, D3DXMATRIXA16* pMatView, D3DXMATRIXA16* pMatProj )
{
	D3DXMATRIXA16 mat;

	D3DXMatrixMultiply( &mat, pMatView, pMatProj );
	D3DXMatrixInverse( &mat, NULL, &mat );

	pCullInfo->vecFrustum[0] = D3DXVECTOR3(-1.0f, -1.0f,  0.0f); // xyz
	pCullInfo->vecFrustum[1] = D3DXVECTOR3( 1.0f, -1.0f,  0.0f); // Xyz
	pCullInfo->vecFrustum[2] = D3DXVECTOR3(-1.0f,  1.0f,  0.0f); // xYz
	pCullInfo->vecFrustum[3] = D3DXVECTOR3( 1.0f,  1.0f,  0.0f); // XYz
	pCullInfo->vecFrustum[4] = D3DXVECTOR3(-1.0f, -1.0f,  1.0f); // xyZ
	pCullInfo->vecFrustum[5] = D3DXVECTOR3( 1.0f, -1.0f,  1.0f); // XyZ
	pCullInfo->vecFrustum[6] = D3DXVECTOR3(-1.0f,  1.0f,  1.0f); // xYZ
	pCullInfo->vecFrustum[7] = D3DXVECTOR3( 1.0f,  1.0f,  1.0f); // XYZ

	for( INT i = 0; i < 8; i++ )
		D3DXVec3TransformCoord( &pCullInfo->vecFrustum[i], &pCullInfo->vecFrustum[i], &mat );

	D3DXPlaneFromPoints( &pCullInfo->planeFrustum[0], &pCullInfo->vecFrustum[0], 
		&pCullInfo->vecFrustum[1], &pCullInfo->vecFrustum[2] ); // Near
	D3DXPlaneFromPoints( &pCullInfo->planeFrustum[1], &pCullInfo->vecFrustum[6], 
		&pCullInfo->vecFrustum[7], &pCullInfo->vecFrustum[5] ); // Far
	D3DXPlaneFromPoints( &pCullInfo->planeFrustum[2], &pCullInfo->vecFrustum[2], 
		&pCullInfo->vecFrustum[6], &pCullInfo->vecFrustum[4] ); // Left
	D3DXPlaneFromPoints( &pCullInfo->planeFrustum[3], &pCullInfo->vecFrustum[7], 
		&pCullInfo->vecFrustum[3], &pCullInfo->vecFrustum[5] ); // Right
	D3DXPlaneFromPoints( &pCullInfo->planeFrustum[4], &pCullInfo->vecFrustum[2], 
		&pCullInfo->vecFrustum[3], &pCullInfo->vecFrustum[6] ); // Top
	D3DXPlaneFromPoints( &pCullInfo->planeFrustum[5], &pCullInfo->vecFrustum[1], 
		&pCullInfo->vecFrustum[0], &pCullInfo->vecFrustum[4] ); // Bottom

	D3DXMATRIXA16 NormalMatrix,temp;
	D3DXVECTOR3 normal;
	D3DXVECTOR4 plane;
	D3DXPLANE p;
	p.a = 0.0;
	p.b = 1.0;
	p.c = 0.0;
	p.d = 0.0;

	D3DXMatrixReflect(&NormalMatrix,&p);
	D3DXMatrixInverse(&NormalMatrix,NULL,&NormalMatrix);
	D3DXMatrixTranspose(&NormalMatrix,&NormalMatrix);
	

	for(int i = 0;i < 6;++i)
	{
		normal.x = pCullInfo->planeFrustum[i].a;
		normal.y = pCullInfo->planeFrustum[i].b;
		normal.z = pCullInfo->planeFrustum[i].c;
		{
			plane.x = pCullInfo->planeFrustum[i].a;
			plane.y = pCullInfo->planeFrustum[i].b;
			plane.z = pCullInfo->planeFrustum[i].c;
			plane.w = pCullInfo->planeFrustum[i].d;
			D3DXVec4Transform(&plane, &plane, &NormalMatrix);
			D3DXVec4Normalize(&plane,&plane);
		}
		D3DXVec3TransformCoord(&normal,&normal,&NormalMatrix);
		D3DXVec3Normalize(&normal,&normal);
		pCullInfo->m_WaterReflectFrustum[i].a = plane.x;//normal.x;
		pCullInfo->m_WaterReflectFrustum[i].b = plane.y;//normal.y;
		pCullInfo->m_WaterReflectFrustum[i].c = plane.z;//normal.z;
		pCullInfo->m_WaterReflectFrustum[i].d = plane.w;//pCullInfo->planeFrustum[i].d;
	}
}




//-----------------------------------------------------------------------------
// Name: CullObject()
// Desc: Determine the cullstate for an object bounding box (OBB).  
//       The algorithm is:
//       1) If any OBB corner pt is inside the frustum, return CS_INSIDE
//       2) Else if all OBB corner pts are outside a single frustum plane, 
//          return CS_OUTSIDE
//       3) Else if any frustum edge penetrates a face of the OBB, return 
//          CS_INSIDE_SLOW
//       4) Else if any OBB edge penetrates a face of the frustum, return
//          CS_INSIDE_SLOW
//       5) Else if any point in the frustum is outside any plane of the 
//          OBB, return CS_OUTSIDE_SLOW
//       6) Else return CS_INSIDE_SLOW
//-----------------------------------------------------------------------------
GraphicsStruct::CULLSTATE CullObject_OLD( GraphicsStruct::CULLINFO* pCullInfo, D3DXVECTOR3* pVecBounds, 
					 D3DXPLANE* pPlaneBounds )
{
	BYTE bOutside[8];
	ZeroMemory( bOutside, sizeof(bOutside) );

	// Check boundary vertices against all 6 frustum planes, 
	// and store result (1 if outside) in a bitfield
	for( int iPoint = 0; iPoint < 8; iPoint++ )
	{
		for( int iPlane = 0; iPlane < 6; iPlane++ )
		{
			if ( pCullInfo->planeFrustum[iPlane].a * pVecBounds[iPoint].x +
				pCullInfo->planeFrustum[iPlane].b * pVecBounds[iPoint].y +
				pCullInfo->planeFrustum[iPlane].c * pVecBounds[iPoint].z +
				pCullInfo->planeFrustum[iPlane].d < 0)
			{
				bOutside[iPoint] |= (1 << iPlane);
			}
		}
		// If any point is inside all 6 frustum planes, it is inside
		// the frustum, so the object must be rendered.
		if ( bOutside[iPoint] == 0 )
			return GraphicsStruct::CS_INSIDE;
	}

	// If all points are outside any single frustum plane, the object is
	// outside the frustum
	if ( (bOutside[0] & bOutside[1] & bOutside[2] & bOutside[3] & 
		bOutside[4] & bOutside[5] & bOutside[6] & bOutside[7]) != 0 )
	{
		return GraphicsStruct::CS_OUTSIDE;
	}

	// Now see if any of the frustum edges penetrate any of the faces of
	// the bounding box
	D3DXVECTOR3 edge[12][2] = 
	{
		pCullInfo->vecFrustum[0], pCullInfo->vecFrustum[1], // front bottom
			pCullInfo->vecFrustum[2], pCullInfo->vecFrustum[3], // front top
			pCullInfo->vecFrustum[0], pCullInfo->vecFrustum[2], // front left
			pCullInfo->vecFrustum[1], pCullInfo->vecFrustum[3], // front right
			pCullInfo->vecFrustum[4], pCullInfo->vecFrustum[5], // back bottom
			pCullInfo->vecFrustum[6], pCullInfo->vecFrustum[7], // back top
			pCullInfo->vecFrustum[4], pCullInfo->vecFrustum[6], // back left
			pCullInfo->vecFrustum[5], pCullInfo->vecFrustum[7], // back right
			pCullInfo->vecFrustum[0], pCullInfo->vecFrustum[4], // left bottom
			pCullInfo->vecFrustum[2], pCullInfo->vecFrustum[6], // left top
			pCullInfo->vecFrustum[1], pCullInfo->vecFrustum[5], // right bottom
			pCullInfo->vecFrustum[3], pCullInfo->vecFrustum[7], // right top
	};
	D3DXVECTOR3 face[6][4] =
	{
		pVecBounds[0], pVecBounds[2], pVecBounds[3], pVecBounds[1], // front
			pVecBounds[4], pVecBounds[5], pVecBounds[7], pVecBounds[6], // back
			pVecBounds[0], pVecBounds[4], pVecBounds[6], pVecBounds[2], // left
			pVecBounds[1], pVecBounds[3], pVecBounds[7], pVecBounds[5], // right
			pVecBounds[2], pVecBounds[6], pVecBounds[7], pVecBounds[3], // top
			pVecBounds[0], pVecBounds[4], pVecBounds[5], pVecBounds[1], // bottom
	};
	D3DXVECTOR3* pEdge;
	D3DXVECTOR3* pFace;
	pEdge = &edge[0][0];
	 
	for(INT iEdge = 0; iEdge < 12; iEdge++ )
	{
		pFace = &face[0][0];
		for( INT iFace = 0; iFace < 6; iFace++ )
		{
			if ( EdgeIntersectsFace( pEdge, pFace, &pPlaneBounds[iFace] ) )
			{
				return GraphicsStruct::CS_INSIDE_SLOW;
			}
			pFace += 4;
		}
		pEdge += 2;
	}

	// Now see if any of the bounding box edges penetrate any of the faces of
	// the frustum
	D3DXVECTOR3 edge2[12][2] = 
	{
		pVecBounds[0], pVecBounds[1], // front bottom
			pVecBounds[2], pVecBounds[3], // front top
			pVecBounds[0], pVecBounds[2], // front left
			pVecBounds[1], pVecBounds[3], // front right
			pVecBounds[4], pVecBounds[5], // back bottom
			pVecBounds[6], pVecBounds[7], // back top
			pVecBounds[4], pVecBounds[6], // back left
			pVecBounds[5], pVecBounds[7], // back right
			pVecBounds[0], pVecBounds[4], // left bottom
			pVecBounds[2], pVecBounds[6], // left top
			pVecBounds[1], pVecBounds[5], // right bottom
			pVecBounds[3], pVecBounds[7], // right top
	};
	D3DXVECTOR3 face2[6][4] =
	{
		pCullInfo->vecFrustum[0], pCullInfo->vecFrustum[2], pCullInfo->vecFrustum[3], pCullInfo->vecFrustum[1], // front
			pCullInfo->vecFrustum[4], pCullInfo->vecFrustum[5], pCullInfo->vecFrustum[7], pCullInfo->vecFrustum[6], // back
			pCullInfo->vecFrustum[0], pCullInfo->vecFrustum[4], pCullInfo->vecFrustum[6], pCullInfo->vecFrustum[2], // left
			pCullInfo->vecFrustum[1], pCullInfo->vecFrustum[3], pCullInfo->vecFrustum[7], pCullInfo->vecFrustum[5], // right
			pCullInfo->vecFrustum[2], pCullInfo->vecFrustum[6], pCullInfo->vecFrustum[7], pCullInfo->vecFrustum[3], // top
			pCullInfo->vecFrustum[0], pCullInfo->vecFrustum[4], pCullInfo->vecFrustum[5], pCullInfo->vecFrustum[1], // bottom
	};
	pEdge = &edge2[0][0];
	for(INT iEdge = 0; iEdge < 12; iEdge++ )
	{
		pFace = &face2[0][0];
		for( INT iFace = 0; iFace < 6; iFace++ )
		{
			if ( EdgeIntersectsFace( pEdge, pFace, &pCullInfo->planeFrustum[iFace] ) )
			{
				return GraphicsStruct::CS_INSIDE_SLOW;
			}
			pFace += 4;
		}
		pEdge += 2;
	}

	// Now see if frustum is contained in bounding box
	// If any frustum corner point is outside any plane of the bounding box,
	// the frustum is not contained in the bounding box, so the object
	// is outside the frustum
	for( INT iPlane = 0; iPlane < 6; iPlane++ )
	{
		if ( pPlaneBounds[iPlane].a * pCullInfo->vecFrustum[0].x +
			pPlaneBounds[iPlane].b * pCullInfo->vecFrustum[0].y +
			pPlaneBounds[iPlane].c * pCullInfo->vecFrustum[0].z +
			pPlaneBounds[iPlane].d  < 0 )
		{
			return GraphicsStruct::CS_OUTSIDE_SLOW;
		}
	}

	// Bounding box must contain the frustum, so render the object
	return GraphicsStruct::CS_INSIDE_SLOW;
}




//-----------------------------------------------------------------------------
// Name: EdgeIntersectsFace()
// Desc: Determine if the edge bounded by the two vectors in pEdges intersects
//       the quadrilateral described by the four vectors in pFacePoints.  
//       Note: pPlane could be derived from pFacePoints using 
//       D3DXPlaneFromPoints, but it is precomputed in advance for greater
//       speed.
//-----------------------------------------------------------------------------
BOOL EdgeIntersectsFace( D3DXVECTOR3* pEdges, D3DXVECTOR3* pFacePoints, 
						D3DXPLANE* pPlane )
{
	// If both edge points are on the same side of the plane, the edge does
	// not intersect the face
	FLOAT fDist1;
	FLOAT fDist2;
	fDist1 = pPlane->a * pEdges[0].x + pPlane->b * pEdges[0].y +
		pPlane->c * pEdges[0].z + pPlane->d;
	fDist2 = pPlane->a * pEdges[1].x + pPlane->b * pEdges[1].y +
		pPlane->c * pEdges[1].z + pPlane->d;
	if ( fDist1 > 0 && fDist2 > 0 ||
		fDist1 < 0 && fDist2 < 0 )
	{
		return FALSE;
	}

	// Find point of intersection between edge and face plane (if they're
	// parallel, edge does not intersect face and D3DXPlaneIntersectLine 
	// returns NULL)
	D3DXVECTOR3 ptIntersection;
	if ( NULL == D3DXPlaneIntersectLine( &ptIntersection, pPlane, &pEdges[0], &pEdges[1] ) )
		return FALSE;

	// Project onto a 2D plane to make the pt-in-poly test easier
	FLOAT fAbsA = (pPlane->a > 0 ? pPlane->a : -pPlane->a);
	FLOAT fAbsB = (pPlane->b > 0 ? pPlane->b : -pPlane->b);
	FLOAT fAbsC = (pPlane->c > 0 ? pPlane->c : -pPlane->c);
	D3DXVECTOR2 facePoints[4];
	D3DXVECTOR2 point;
	if ( fAbsA > fAbsB && fAbsA > fAbsC )
	{
		// Plane is mainly pointing along X axis, so use Y and Z
		for( INT i = 0; i < 4; i++)
		{
			facePoints[i].x = pFacePoints[i].y;
			facePoints[i].y = pFacePoints[i].z;
		}
		point.x = ptIntersection.y;
		point.y = ptIntersection.z;
	}
	else if ( fAbsB > fAbsA && fAbsB > fAbsC )
	{
		// Plane is mainly pointing along Y axis, so use X and Z
		for( INT i = 0; i < 4; i++)
		{
			facePoints[i].x = pFacePoints[i].x;
			facePoints[i].y = pFacePoints[i].z;
		}
		point.x = ptIntersection.x;
		point.y = ptIntersection.z;
	}
	else
	{
		// Plane is mainly pointing along Z axis, so use X and Y
		for( INT i = 0; i < 4; i++)
		{
			facePoints[i].x = pFacePoints[i].x;
			facePoints[i].y = pFacePoints[i].y;
		}
		point.x = ptIntersection.x;
		point.y = ptIntersection.y;
	}

	// If point is on the outside of any of the face edges, it is
	// outside the face.  
	// We can do this by taking the determinant of the following matrix:
	// | x0 y0 1 |
	// | x1 y1 1 |
	// | x2 y2 1 |
	// where (x0,y0) and (x1,y1) are points on the face edge and (x2,y2) 
	// is our test point.  If this value is positive, the test point is
	// "to the left" of the line.  To determine whether a point needs to
	// be "to the right" or "to the left" of the four lines to qualify as
	// inside the face, we need to see if the faces are specified in 
	// clockwise or counter-clockwise order (it could be either, since the
	// edge could be penetrating from either side).  To determine this, we
	// do the same test to see if the third point is "to the right" or 
	// "to the left" of the line formed by the first two points.
	// See http://forum.swarthmore.edu/dr.math/problems/scott5.31.96.html
	FLOAT x0, x1, x2, y0, y1, y2;
	x0 = facePoints[0].x;
	y0 = facePoints[0].y;
	x1 = facePoints[1].x;
	y1 = facePoints[1].y;
	x2 = facePoints[2].x;
	y2 = facePoints[2].y;
	BOOL bClockwise = FALSE;
	if ( x1*y2 - y1*x2 - x0*y2 + y0*x2 + x0*y1 - y0*x1 < 0 )
		bClockwise = TRUE;
	x2 = point.x;
	y2 = point.y;
	for( INT i = 0; i < 4; i++ )
	{
		x0 = facePoints[i].x;
		y0 = facePoints[i].y;
		if ( i < 3 )
		{
			x1 = facePoints[i+1].x;
			y1 = facePoints[i+1].y;
		}
		else
		{
			x1 = facePoints[0].x;
			y1 = facePoints[0].y;
		}
		if ( ( x1*y2 - y1*x2 - x0*y2 + y0*x2 + x0*y1 - y0*x1 > 0 ) == bClockwise )
			return FALSE;
	}

	// If we get here, the point is inside all four face edges, 
	// so it's inside the face.
	return TRUE;
}

void MatrixExract(D3DXVECTOR3& Scaling,D3DXVECTOR3& Position,
				  D3DXMATRIX& Rotation,D3DXMATRIX* pInMatrix)
{
	D3DXVECTOR3 A,B,C;
	A = D3DXVECTOR3(pInMatrix->_11,pInMatrix->_12,pInMatrix->_13);
	B = D3DXVECTOR3(pInMatrix->_21,pInMatrix->_22,pInMatrix->_23);
	C = D3DXVECTOR3(pInMatrix->_31,pInMatrix->_32,pInMatrix->_33);

	float Sx,Sy,Sz;
	Sx = D3DXVec3Length(&A);
	Sy = D3DXVec3Length(&B);
	Sz = D3DXVec3Length(&C);
	Scaling = D3DXVECTOR3(Sx,Sy,Sz);
	
	Position.x = pInMatrix->_41;
	Position.y = pInMatrix->_42;
	Position.z = pInMatrix->_43;
	
	D3DXMATRIX SrcMatrix = *pInMatrix;

	SrcMatrix._41 = 0;
	SrcMatrix._42 = 0;
	SrcMatrix._43 = 0;
	
	D3DXMATRIX matScale;
	D3DXMatrixScaling(&matScale,Sx,Sy,Sz);
	D3DXMatrixInverse(&matScale,NULL,&matScale);
	
	D3DXMatrixMultiply(&Rotation,&matScale,&SrcMatrix);

}

#include <tchar.h>

int TrimString(char *pszString)
{
	int nResult = false;

	char *pszOrign = pszString;
	char *pszFirst = NULL;
	char *pszLast  = NULL;

	if (!pszString)
		goto Exit0;


	while (*pszString)
	{
		if (_istspace(*pszString))
		{
			if (!pszLast)
				pszLast = pszString;
		}
		else
		{
			pszLast = NULL;

			if (!pszFirst)
				pszFirst = pszString;
		}

		pszString = _tcsinc(pszString);
	}

	if (!pszFirst)
		pszFirst = pszOrign;

	if (!pszLast)
		pszLast = pszString;

	if (pszLast > pszFirst)
	{
		memmove(pszOrign, pszFirst, pszLast - pszFirst);
	}

	pszOrign[pszLast - pszFirst] = '\0';

	nResult = true;
Exit0:
	return nResult;


}



BOOL EdgeIntersectsFaceSSE2( D3DXVECTOR3* pEdges, D3DXVECTOR3* pFacePoints, D3DXPLANE* pPlane );

//-----------------------------------------------------------------------------
// Name: CullObject()
// Desc: Determine the cullstate for an object bounding box (OBB).  
//       The algorithm is:
//       1) If any OBB corner pt is inside the frustum, return CS_INSIDE
//       2) Else if all OBB corner pts are outside a single frustum plane, 
//          return CS_OUTSIDE
//       3) Else if any frustum edge penetrates a face of the OBB, return 
//          CS_INSIDE_SLOW
//       4) Else if any OBB edge penetrates a face of the frustum, return
//          CS_INSIDE_SLOW
//       5) Else if any point in the frustum is outside any plane of the 
//          OBB, return CS_OUTSIDE_SLOW
//       6) Else return CS_INSIDE_SLOW
//-----------------------------------------------------------------------------
#define	shuf_01_01	 68	// 01000100
#define	shuf_23_23  238 // 11101110

_declspec (align(16)) const float Zero[] = {0.0f, 0.0f, 0.0f, 0.0f};

BOOL EdgesIntersectsFacesSSE2(D3DXVECTOR3 *pEdges, D3DXVECTOR3 *pFacePoints, D3DXPLANE* pPlanes)
{

	_declspec (align(16)) float buffer[44];  // tempoary 
	int	intersect;

	D3DXVECTOR3 *pFace;

	// set up six planes of the OBB
	__asm{
		mov esi, pPlanes
			lea	edx, buffer

			movups	xmm0, [esi]		// xmm0 = [d0,c0,b0,a0] 
			movups	xmm4, [esi+16]	// xmm4 = [d1,c1,b1,a1] 
			movaps	xmm1, xmm0		// xmm1 = [d0,c0,b0,a0] 

				unpcklps xmm0, xmm4		// xmm0 = [b1,b0,a1,a0]
				unpckhps xmm1, xmm4		// xmm1 = [d1,d0,c1,c0]    xmm4 free

				movups	xmm6, [esi+32]	// xmm6 = [d2,c2,b2,a2] 
				movups	xmm5, [esi+48]	// xmm5 = [d3,c3,b3,a3] 
				movaps	xmm3, xmm6		// xmm3 = [d2,c2,b2,a2]

					unpcklps xmm6, xmm5		// xmm6 = [b3,b2,a3,a2]
					unpckhps xmm3, xmm5		// xmm3 = [d3,d2,c3,c2]    xmm5 free

					movaps	xmm2, xmm0		// xmm2 = [b1,b0,a1,a0]
					shufps	xmm0, xmm6, shuf_01_01	// xmm0 = [a3,a2,a1,a0]    xmm6 free
					shufps	xmm2, xmm6, shuf_23_23	// xmm2 = [b3,b2,b1,b0]

					movaps	xmm4, xmm1		// xmm4 = [d1,d0,c1,c0]
					shufps	xmm1, xmm3, shuf_01_01	// xmm1 = [c3,c2,c1,c0]       
					shufps	xmm4, xmm3, shuf_23_23	// xmm4 = [d3,d2,d1,d0]		  

					movaps	[edx],	xmm0		// save [a3,a2,a1,a0]	  A0123
					movaps	[edx+16], xmm2		// save [b3,b2,b1,b0]     B0123

					movaps	[edx+32], xmm1		// save [c3,c2,c1,c0]	  C0123 
					movaps	[edx+48], xmm4		// save [d3,d2,d1,d0]	  D0123 

					movups	xmm0, [esi+64]	// xmm0 = [d4,c4,b4,a4] 
					movups	xmm5, [esi+80]	// xmm5 = [d5,c5,b5,a5] 
					movaps  xmm1, xmm0		// xmm1 = [d4,c4,b4,a4]

						unpcklps xmm0, xmm5		// xmm0 = [b5,b4,a5,a4]
						unpckhps xmm1, xmm5		// xmm1 = [d5,d4,c5,c4]	

						movaps	xmm2, xmm0		// xmm2 = [b5,b4,a5,a4]
						movaps	xmm3, xmm1		// xmm3 = [d5,d4,c5,c4]

						shufps	xmm0,xmm0,shuf_01_01	// xmm0 = [a5,a4,a5,a4]
						shufps	xmm2,xmm2,shuf_23_23	// xmm2 = [b5,b4,b5,b4]
						movaps	[edx+64], xmm0	// save [a5,a4,a5,a4]

						shufps	xmm1, xmm1,shuf_01_01	// xmm1 = [c5,c4,c5,c4]
						movaps	[edx+80], xmm2	// save b5,b4,b5,b4]

						shufps	xmm3, xmm3,shuf_23_23	// xmm3 = [d5,d4,d5,d4]
						movaps	[edx+96], xmm1	// save [c5,c4,c5,c4]

						movaps	[edx+112], xmm3	// save d5,d4,d5,d4]

	}

	for( INT iEdge = 0; iEdge < 12; iEdge++ )
	{

		//pFace = &face[0][0];
		pFace = pFacePoints;

		// check if eEdge intersect with pPlaneBounds[i], i = 0,1,2,3,
		__asm{
			mov		edi, pEdges
				lea	edx, buffer

				// first point of the edge
				movss	xmm5, [edi]		// xmm5 = [*,*,*,x0] 
				movss	xmm6, [edi+4]	// xmm6 = [*,*,*,y0]
				shufps	xmm5, xmm5, 0	// xmm5 = [x0,x0,x0,x0]
					movaps	xmm0, [edx]		// xmm0 = [a3,a2,a1,a0]

					movss	xmm7, [edi+8]	// xmm7 = [*,*,*,z0]
					shufps	xmm6, xmm6, 0	// xmm6 = [y0,y0,y0,y0]
						movaps	xmm1, [edx+16]	// xmm1 = [b3,b2,b1,b0]
						movaps	[edx+128],xmm5		// save [x0,x0,x0,x0]

							mulps	xmm5, xmm0		// xmm5 = [a3*x0,a2*x0,a1*x0,a0*x0]
							shufps xmm7, xmm7, 0	// xmm7 = [z0,z0,z0,z0]
							movaps	xmm2, [edx+32]	// xmm2 = [c3,c2,c1,c0]
							movaps	[edx+144],xmm6	// save [y0,y0,y0,y0]

								mulps	xmm6, xmm1		// xmm6 = [b3*y0,b2*y0,b1*y0,b0*y0]
								movaps	xmm3, [edx+48]	// xmm3 = [d3,d2,d1,d0]

								movaps	[edx+160],xmm7	// save [z0,z0,z0,z0]
									mulps	xmm7, xmm2		// xmm7 = [c3*z0,c2*z0,c1*z0,c0*z0]
									addps	xmm5, xmm6		// xmm5 = [a3*x0+b3*y0,...,a0*x0+b0*y0]
									addps	xmm7, xmm3		// xmm7 = [c3*z0+d3,...,c0*z0+d3]
									addps	xmm5, xmm7		// xmm5 = [dist3,dist2.dist1,dist0]
									//		= [a3*x0+b3*y0+c3*z0+d3,...,a0*x0+b0*y0+c0*z0+d3]
									// second point of the edge
									movss	xmm4, [edi+12]		// xmm4 = [*,*,*,x1]   for first point of the edge
									movss	xmm6, [edi+16]	// xmm6 = [*,*,*,y1]
									shufps	xmm4, xmm4, 0	// xmm5 = [x1,x1,x1,x1]

										movss	xmm7, [edi+24]	// xmm7 = [*,*,*,z1]
										shufps	xmm6, xmm6, 0	// xmm6 = [y1,y1,y1,y1]

											mulps	xmm0, xmm4		// xmm0 = [a3*x1,a2*x1,a1*x1,a0*x1]
											shufps xmm7, xmm7, 0	// xmm7 = [z1,z1,z1,z1]
											mulps	xmm1, xmm6		// xmm1 = [b3*y1,b2*y1,b1*y1,b0*y1]

											mulps	xmm2, xmm7		// xmm2 = [c3*z1,c2*z1,c1*z1,c0*z1]

											addps	xmm0, xmm1		// xmm0 = [a3*x1+b3*y1,...,a0*x1+b0*y1]
											addps	xmm2, xmm3		// xmm2 = [c3*z1+d3,...,c0*z1+d3]
											addps	xmm0, xmm2		// xmm4 = [Dist3,Dist2.Dist1,Dist0]
											//		= [a3*x1+b3*y1+c3*z1+d3,...,a0*x1+b0*y1+c0*z1+d3]

											mulps	xmm0, xmm5		// xmm0 = [Dist3*dist3, Dist3*dist2, Dist3*dist1, Dist3*dist0]

											cmpleps xmm0, Zero		// xmm0 = [Dist3*dist3 <= 0 ?,...,Dist3*dist0 <= 0 ?]

											movmskps eax, xmm0
											mov intersect, eax

		}

		for( INT iFace = 0; iFace < 4; iFace++ ){
			if(intersect & (1 << iFace )){
				if ( EdgeIntersectsFaceSSE2( pEdges, pFace, &pPlanes[iFace] ) ){
					return TRUE;
				}
			}
			pFace += 4;
		}

		// check if eEdge intersect with pPlaneBounds[i], i = 4,5
		_asm{
			lea	edx, buffer
				movaps	xmm0, [edx+128]	// xmm0 = [x0,x0,x0,x0]
				movaps	xmm1, [edx+144]	// xmm1 = [y0,y0,y0,y0]
				movaps	xmm2, [edx+160]	// xmm2 = [z0,z0,z0,z0]

				shufps	xmm0, xmm4,shuf_01_01	// xmm0 = [x1,x1,x0,x0]
					shufps	xmm1, xmm6,shuf_01_01	// xmm1 = [y1,y1,y0,y0]
					shufps	xmm2, xmm7,shuf_01_01	// xmm1 = [z1,z1,z0,z0]

					movaps	xmm3, [edx+64]	// xmm3 = [a5,a4,a5,a4]
					movaps	xmm4, [edx+80]	// xmm4 = [b5,b4,b5,b4]
					movaps	xmm5, [edx+96]	// xmm5 = [c5,c4,c5,c4]
					movaps	xmm6, [edx+112]	// xmm4 = [d5,d4,d5,d4]

					mulps	xmm0, xmm3		// xmm0 = [a5*x1,a4*x1,a5*x0,a4*x0]	
						mulps	xmm1, xmm4		// xmm1 = [b5*y1,b4*y1,b5*y0,b4*y0]
						mulps	xmm2, xmm5		// xmm2 = [c5*z1,c4*z1,c5*z0,c4*z0]

						addps	xmm0, xmm4		// xmm0 = [a5*x1+b5*y1,a4*x1+b4*y1,a5*x0+b5*y0,a4*x0+b4*y0]	
						addps	xmm2, xmm6		// xmm2 = [c5*z1+d5,c4*z1+d4,c5*z0+d5,c4*z0+d4]
						addps	xmm0, xmm2		// xmm0 = [Dist5,Dist4,dist5,dist4]
						movaps	xmm7, xmm0		// xmm7 = [Dist5,Dist4,dist5,dist4]
						psrldq  xmm0, 8			// xmm0 = [0,0,Dist5,Dist4]
						mulps	xmm7,xmm0		// xmm0 = [[0,0,Dist5*dist5,Dist4*dist5]
						movmskps eax, xmm0
						mov	intersect, eax			// save to intersect

		}

		for( INT iFace = 4; iFace < 5; iFace++ ){
			if(intersect & (1 << (iFace - 4) )){
				if ( EdgeIntersectsFaceSSE2( pEdges, pFace, &pPlanes[iFace] ) ){
					//return CS_INSIDE_SLOW;
					return TRUE;
				}
			}
			pFace += 4;
		}

		pEdges += 2;

	}

	return FALSE;
}//EdgesIntersectsFacesSSE2


DWORD/*GraphicsStruct::CULLSTATE*/ CullObject( GraphicsStruct::CULLINFO* pCullInfo, D3DXVECTOR3* pVecBounds, 
					D3DXPLANE* pPlaneBounds,bool IsWaterReflect )
{
	/*
	BYTE bOutside[8];
	ZeroMemory( bOutside, sizeof(bOutside) );

	// Check boundary vertices against all 6 frustum planes, 
	// and store result (1 if outside) in a bitfield
	for( int iPoint = 0; iPoint < 8; iPoint++ )
	{
	for( int iPlane = 0; iPlane < 6; iPlane++ )
	{
	if ( pCullInfo->planeFrustum[iPlane].a * pVecBounds[iPoint].x +
	pCullInfo->planeFrustum[iPlane].b * pVecBounds[iPoint].y +
	pCullInfo->planeFrustum[iPlane].c * pVecBounds[iPoint].z +
	pCullInfo->planeFrustum[iPlane].d < 0)
	{
	bOutside[iPoint] |= (1 << iPlane);
	}
	}
	// If any point is inside all 6 frustum planes, it is inside
	// the frustum, so the object must be rendered.
	if ( bOutside[iPoint] == 0 ){
	return CS_INSIDE;
	}
	}
	*/
	_declspec (align(16)) float buffer[32];  // temporary buffer
	int bOutside = 63;  // first six bits = 111111
	GraphicsStruct::CULLSTATE ret_flag = GraphicsStruct::CS_OUTSIDE;

	void *planeFrustum = NULL;
	if(!IsWaterReflect)
		planeFrustum = (void *)pCullInfo->planeFrustum;
	else 
		planeFrustum = (void *)pCullInfo->m_WaterReflectFrustum;
	// Check boundary vertices against all 6 frustum planes, 
	// and store result (1 if outside) in a bitfield

	__asm{
		//lea eax, bOutside
		mov	edi, pVecBounds
			mov	esi, planeFrustum
			lea	edx, buffer
			//push eax

			movups	xmm0, [esi]		// xmm0 = [d0,c0,b0,a0] 
			movups	xmm4, [esi+16]	// xmm4 = [d1,c1,b1,a1] 
			movaps	xmm1, xmm0		// xmm1 = [d0,c0,b0,a0] 

				unpcklps xmm0, xmm4		// xmm0 = [b1,b0,a1,a0]
				unpckhps xmm1, xmm4		// xmm1 = [d1,d0,c1,c0]    xmm4 free

				movups	xmm6, [esi+32]	// xmm6 = [d2,c2,b2,a2] 
				movups	xmm5, [esi+48]	// xmm5 = [d3,c3,b3,a3] 
				movaps	xmm3, xmm6		// xmm3 = [d2,c2,b2,a2]

					unpcklps xmm6, xmm5		// xmm6 = [b3,b2,a3,a2]
					unpckhps xmm3, xmm5		// xmm3 = [d3,d2,c3,c2]    xmm5 free

					movaps	xmm2, xmm0		// xmm2 = [b1,b0,a1,a0]
					shufps	xmm0, xmm6, shuf_01_01	// xmm0 = [a3,a2,a1,a0]    xmm6 free
					shufps	xmm2, xmm6, shuf_23_23	// xmm2 = [b3,b2,b1,b0]

					movaps	xmm4, xmm1		// xmm4 = [d1,d0,c1,c0]
					shufps	xmm1, xmm3, shuf_01_01	// xmm1 = [c3,c2,c1,c0]       
					shufps	xmm4, xmm3, shuf_23_23	// xmm4 = [d3,d2,d1,d0]		  

					movaps	[edx],	xmm0		// save [a3,a2,a1,a0]	  A0123
					movaps	[edx+16], xmm2		// save [b3,b2,b1,b0]     B0123

					movaps	[edx+32], xmm1		// save [c3,c2,c1,c0]	  C0123 
					movaps	[edx+48], xmm4		// save [d3,d2,d1,d0]	  D0123 

					movups	xmm0, [esi+64]	// xmm0 = [d4,c4,b4,a4] 
					movups	xmm5, [esi+80]	// xmm5 = [d5,c5,b5,a5] 
					movaps  xmm1, xmm0		// xmm1 = [d4,c4,b4,a4]
						unpcklps xmm0, xmm5		// xmm0 = [b5,b4,a5,a4]
						unpckhps xmm1, xmm5		// xmm1 = [d5,d4,c5,c4]	

						// 1.
						movaps	xmm2, xmm0		// xmm2 = [b5,b4,a5,a4]
						movaps	xmm3, xmm1		// xmm3 = [d5,d4,c5,c4]
						psrldq	xmm0, 8			// xmm0 = [0,0,b5,b4]			B4B5
						psrldq  xmm1, 8			// xmm1 = [0,0,d5,d4]			D4D5
						movaps	[edx+64], xmm2		// save [*,*,a5,a4]
						movaps	[edx+80], xmm0		// save [*,*,b4,a4]
						movaps	[edx+96], xmm3		// save [0,0,c5,c4]
						movaps	[edx+112], xmm1		// save [0,0,d5,d4]

						mov	ecx, 8
						//lea esi, bOutside

Next_point:
					movss	xmm5, [edi]		// xmm5 = [*,*,*,x]
					movss	xmm6, [edi+4]	// xmm6 = [*,*,*,y]
					shufps	xmm5, xmm5, 0	// xmm5 = [x,x,x,x]
						movaps	xmm0, [edx]		// xmm0 = [a3,a2,a1,a0]

						movss	xmm7, [edi+8]	// xmm7 = [*,*,*,z]
						shufps	xmm6, xmm6, 0	// xmm6 = [y,y,y,y]
							movaps	xmm1, [edx+16]	// xmm1 = [b3,b2,b1,b0]

							mulps	xmm0, xmm5		// xmm0 = [a3*x,a2*x,a1*x,a0*x]

								shufps xmm7, xmm7, 0	// xmm7 = [z,z,z,z]
								movaps	xmm2, [edx+32]	// xmm2 = [c3,c2,c1,c0]

								mulps	xmm1, xmm6		// xmm1 = [b3*y,b2*y,b1*y,b0*y]
									movaps	xmm3, [edx+48]	// xmm3 = [d3,d2,d1,d0]

									mulps	xmm2, xmm7		// xmm2 = [c3*z,c2*z,c1*z,c0*z]
										addps	xmm0, xmm1		// xmm0 = [a3*x+b3*y,...,a0*x+b0*y]

										addps	xmm2, xmm3		// xmm2 = [c3*z+d3,...,c0*z+d3]

										movaps	xmm1, [edx+64]	// xmm1 = [*,*,a5,a4]

										addps	xmm0, xmm2		// xmm0 = [a3*x+b3*y+c3*z+d3,...,a0*x+b0*y+c0*z+d3]
											movaps	xmm2, [edx+80]	// xmm2 = [*,*,b5,b4]

											cmpltps xmm0, Zero		// xmm0 = [a3*x+b3*y+c3*z+d3 < 0 ?,...,a0*x+b0*y+c0*z+d0 < 0 ?]
												movaps	xmm3, [edx+96]	// xmm3 = [*,*,c5,c4]

												mulps	xmm5, xmm1		// xmm5 = [*,*,a5*x, a4*x]
													movaps	xmm4, [edx+112]	// xmm4 = [*,*,d5,d4]

													mulps	xmm6, xmm2		// xmm6 = [*,*,b5*y, b4*y]
														movmskps eax, xmm0

														mulps	xmm7, xmm3		// xmm7 = [*,*,c5*z, c4*z]

														addps	xmm5,xmm6
														addps	xmm7, xmm4
														addps	xmm5, xmm7

														cmpltps xmm5, Zero		// xmm0 = [*,*,a5*x+b5*y+c5*z+d5 < 0 ?,a4*x+b4*y+c4*z+d4 < 0 ?]

														//push	esi
														movmskps esi, xmm5
														and	esi, 3
														shl	esi, 4
														or	eax, esi
														cmp	eax, 0
														//pop esi

														jg	contiune

														mov	eax,1  // GraphicsStruct::CS_INSIDE
														mov ret_flag, eax
														jmp done			// early exit, return CS_INSIDE

contiune:
													mov esi, bOutside
														and esi, eax		// and six bit of the current bOutside to global bOutside
														mov bOutside, esi   

														add	edi, 12
														sub	ecx, 1
														jne	Next_point
done:

	}

	if(ret_flag == GraphicsStruct::CS_INSIDE ){
		return ret_flag;
	}

	// If all points are outside any single frustum plane, the object is
	// outside the frustum
	//if ( (bOutside[0] & bOutside[1] & bOutside[2] & bOutside[3] & 
	//bOutside[4] & bOutside[5] & bOutside[6] & bOutside[7]) != 0 )
	if(bOutside != 0)
	{
		return GraphicsStruct::CS_OUTSIDE;
	}

	// Now see if any of the frustum edges penetrate any of the faces of
	// the bounding box
	D3DXVECTOR3 edge[12][2] = 
	{
		pCullInfo->vecFrustum[0], pCullInfo->vecFrustum[1], // front bottom
			pCullInfo->vecFrustum[2], pCullInfo->vecFrustum[3], // front top
			pCullInfo->vecFrustum[0], pCullInfo->vecFrustum[2], // front left
			pCullInfo->vecFrustum[1], pCullInfo->vecFrustum[3], // front right
			pCullInfo->vecFrustum[4], pCullInfo->vecFrustum[5], // back bottom
			pCullInfo->vecFrustum[6], pCullInfo->vecFrustum[7], // back top
			pCullInfo->vecFrustum[4], pCullInfo->vecFrustum[6], // back left
			pCullInfo->vecFrustum[5], pCullInfo->vecFrustum[7], // back right
			pCullInfo->vecFrustum[0], pCullInfo->vecFrustum[4], // left bottom
			pCullInfo->vecFrustum[2], pCullInfo->vecFrustum[6], // left top
			pCullInfo->vecFrustum[1], pCullInfo->vecFrustum[5], // right bottom
			pCullInfo->vecFrustum[3], pCullInfo->vecFrustum[7], // right top
	};
	D3DXVECTOR3 face[6][4] =
	{
		pVecBounds[0], pVecBounds[2], pVecBounds[3], pVecBounds[1], // front
			pVecBounds[4], pVecBounds[5], pVecBounds[7], pVecBounds[6], // back
			pVecBounds[0], pVecBounds[4], pVecBounds[6], pVecBounds[2], // left
			pVecBounds[1], pVecBounds[3], pVecBounds[7], pVecBounds[5], // right
			pVecBounds[2], pVecBounds[6], pVecBounds[7], pVecBounds[3], // top
			pVecBounds[0], pVecBounds[4], pVecBounds[5], pVecBounds[1], // bottom
	};

	/*
	D3DXVECTOR3* pEdge;
	D3DXVECTOR3* pFace;
	pEdge = &edge[0][0];
	pFace = &face[0][0];
	for( INT iEdge = 0; iEdge < 12; iEdge++ )
	{

	pFace = &face[0][0];
	for( INT iFace = 0; iFace < 6; iFace++ )
	{
	if ( EdgeIntersectsFace( pEdge, pFace, &pPlaneBounds[iFace] ) )
	{
	return CS_INSIDE_SLOW;
	}
	pFace += 4;
	}
	pEdge += 2;

	}

	*/

	if(EdgesIntersectsFacesSSE2((D3DXVECTOR3 *)edge, (D3DXVECTOR3 *)face, pPlaneBounds))
		return GraphicsStruct::CS_INSIDE_SLOW;


	// Now see if any of the bounding box edges penetrate any of the faces of
	// the frustum
	D3DXVECTOR3 edge2[12][2] = 
	{
		pVecBounds[0], pVecBounds[1], // front bottom
			pVecBounds[2], pVecBounds[3], // front top
			pVecBounds[0], pVecBounds[2], // front left
			pVecBounds[1], pVecBounds[3], // front right
			pVecBounds[4], pVecBounds[5], // back bottom
			pVecBounds[6], pVecBounds[7], // back top
			pVecBounds[4], pVecBounds[6], // back left
			pVecBounds[5], pVecBounds[7], // back right
			pVecBounds[0], pVecBounds[4], // left bottom
			pVecBounds[2], pVecBounds[6], // left top
			pVecBounds[1], pVecBounds[5], // right bottom
			pVecBounds[3], pVecBounds[7], // right top
	};
	D3DXVECTOR3 face2[6][4] =
	{
		pCullInfo->vecFrustum[0], pCullInfo->vecFrustum[2], pCullInfo->vecFrustum[3], pCullInfo->vecFrustum[1], // front
			pCullInfo->vecFrustum[4], pCullInfo->vecFrustum[5], pCullInfo->vecFrustum[7], pCullInfo->vecFrustum[6], // back
			pCullInfo->vecFrustum[0], pCullInfo->vecFrustum[4], pCullInfo->vecFrustum[6], pCullInfo->vecFrustum[2], // left
			pCullInfo->vecFrustum[1], pCullInfo->vecFrustum[3], pCullInfo->vecFrustum[7], pCullInfo->vecFrustum[5], // right
			pCullInfo->vecFrustum[2], pCullInfo->vecFrustum[6], pCullInfo->vecFrustum[7], pCullInfo->vecFrustum[3], // top
			pCullInfo->vecFrustum[0], pCullInfo->vecFrustum[4], pCullInfo->vecFrustum[5], pCullInfo->vecFrustum[1], // bottom
	};

	/*
	pEdge = &edge2[0][0];
	pFace = &face2[0][0];

	for( INT iEdge = 0; iEdge < 12; iEdge++ )
	{
	pFace = &face2[0][0];
	for( INT iFace = 0; iFace < 6; iFace++ )
	{
	if ( EdgeIntersectsFace( pEdge, pFace, &pCullInfo->planeFrustum[iFace] ) )
	{
	return CS_INSIDE_SLOW;
	}
	pFace += 4;
	}
	pEdge += 2;
	}
	*/
	if(EdgesIntersectsFacesSSE2((D3DXVECTOR3 *)edge2, (D3DXVECTOR3 *)face2, pCullInfo->planeFrustum))
		return GraphicsStruct::CS_INSIDE_SLOW;

	// Now see if frustum is contained in bounding box
	// If any frustum corner point is outside any plane of the bounding box,
	// the frustum is not contained in the bounding box, so the object
	// is outside the frustum
	for( INT iPlane = 0; iPlane < 6; iPlane++ )
	{
		if ( pPlaneBounds[iPlane].a * pCullInfo->vecFrustum[0].x +
			pPlaneBounds[iPlane].b * pCullInfo->vecFrustum[0].y +
			pPlaneBounds[iPlane].c * pCullInfo->vecFrustum[0].z +
			pPlaneBounds[iPlane].d  < 0 )
		{
			return GraphicsStruct::CS_OUTSIDE_SLOW;
		}
	}

	// Bounding box must contain the frustum, so render the object
	//return GraphicsStruct::CS_INSIDE_SLOW;
	return GraphicsStruct::CS_OUTSIDE;

}


BOOL EdgeIntersectsFaceSSE2( D3DXVECTOR3* pEdges, D3DXVECTOR3* pFacePoints, 
							D3DXPLANE* pPlane )
{
	// Find point of intersection between edge and face plane (if they're
	// parallel, edge does not intersect face and D3DXPlaneIntersectLine 
	// returns NULL)
	D3DXVECTOR3 ptIntersection;
	if ( NULL == D3DXPlaneIntersectLine( &ptIntersection, pPlane, &pEdges[0], &pEdges[1] ) )
		return FALSE;

	// Project onto a 2D plane to make the pt-in-poly test easier
	FLOAT fAbsA = (pPlane->a > 0 ? pPlane->a : -pPlane->a);
	FLOAT fAbsB = (pPlane->b > 0 ? pPlane->b : -pPlane->b);
	FLOAT fAbsC = (pPlane->c > 0 ? pPlane->c : -pPlane->c);
	D3DXVECTOR2 facePoints[4];
	D3DXVECTOR2 point;
	if ( fAbsA > fAbsB && fAbsA > fAbsC )
	{
		// Plane is mainly pointing along X axis, so use Y and Z
		for( INT i = 0; i < 4; i++)
		{
			facePoints[i].x = pFacePoints[i].y;
			facePoints[i].y = pFacePoints[i].z;
		}
		point.x = ptIntersection.y;
		point.y = ptIntersection.z;
	}
	else if ( fAbsB > fAbsA && fAbsB > fAbsC )
	{
		// Plane is mainly pointing along Y axis, so use X and Z
		for( INT i = 0; i < 4; i++)
		{
			facePoints[i].x = pFacePoints[i].x;
			facePoints[i].y = pFacePoints[i].z;
		}
		point.x = ptIntersection.x;
		point.y = ptIntersection.z;
	}
	else
	{
		// Plane is mainly pointing along Z axis, so use X and Y
		for( INT i = 0; i < 4; i++)
		{
			facePoints[i].x = pFacePoints[i].x;
			facePoints[i].y = pFacePoints[i].y;
		}
		point.x = ptIntersection.x;
		point.y = ptIntersection.y;
	}

	// If point is on the outside of any of the face edges, it is
	// outside the face.  
	// We can do this by taking the determinant of the following matrix:
	// | x0 y0 1 |
	// | x1 y1 1 |
	// | x2 y2 1 |
	// where (x0,y0) and (x1,y1) are points on the face edge and (x2,y2) 
	// is our test point.  If this value is positive, the test point is
	// "to the left" of the line.  To determine whether a point needs to
	// be "to the right" or "to the left" of the four lines to qualify as
	// inside the face, we need to see if the faces are specified in 
	// clockwise or counter-clockwise order (it could be either, since the
	// edge could be penetrating from either side).  To determine this, we
	// do the same test to see if the third point is "to the right" or 
	// "to the left" of the line formed by the first two points.
	// See http://forum.swarthmore.edu/dr.math/problems/scott5.31.96.html
	FLOAT x0, x1, x2, y0, y1, y2;
	x0 = facePoints[0].x;
	y0 = facePoints[0].y;
	x1 = facePoints[1].x;
	y1 = facePoints[1].y;
	x2 = facePoints[2].x;
	y2 = facePoints[2].y;
	BOOL bClockwise = FALSE;
	if ( x1*y2 - y1*x2 - x0*y2 + y0*x2 + x0*y1 - y0*x1 < 0 )
		bClockwise = TRUE;
	x2 = point.x;
	y2 = point.y;
	for( INT i = 0; i < 4; i++ )
	{
		x0 = facePoints[i].x;
		y0 = facePoints[i].y;
		if ( i < 3 )
		{
			x1 = facePoints[i+1].x;
			y1 = facePoints[i+1].y;
		}
		else
		{
			x1 = facePoints[0].x;
			y1 = facePoints[0].y;
		}
		if ( ( x1*y2 - y1*x2 - x0*y2 + y0*x2 + x0*y1 - y0*x1 > 0 ) == bClockwise )
			return FALSE;
	}

	// If we get here, the point is inside all four face edges, 
	// so it's inside the face.
	return TRUE;
}

float PointDistanceLine(CONST D3DXVECTOR3 *pPoint,
						CONST D3DXVECTOR3 *pV1,
						CONST D3DXVECTOR3 *pV2)
{
	D3DXPLANE Plane;
	D3DXPlaneFromPointNormal(&Plane,pPoint,&(*pV2-*pV1));
	D3DXVECTOR3 Inter;
	D3DXPlaneIntersectLine(&Inter,&Plane,pV1,pV2);
	return D3DXVec3Length(&(Inter - *pPoint));
}



//
//void GetLine2D(float* pfA,float* pfB,float* pfC,D3DXVECTOR2& A,D3DXVECTOR2& B)
//{
//	D3DXVECTOR2 N = B - A;
//	D3DXVec2Normalize(&N,&N);
//	(*pfA) = N.x;
//	(*pfB) = N.y;
//	(*pfC) = - (N.x * A.x + N.y * A.y); 
//}







//////////////////////////////////////////////////////////////////////////

inline bool IsVertexInLine(D3DXVECTOR3& s, D3DXVECTOR3& d, D3DXVECTOR3& v)
{
    D3DXVECTOR3 vd = v - s;
    D3DXVECTOR3 vs = v - d;

    D3DXVec3Normalize(&vd, &vd);
    D3DXVec3Normalize(&vs, &vs);

    if (D3DXVec3Dot(&vd, &vs) < -0.5f)
        return true;

    return false;
}

inline bool IsVertexInSphere(D3DXVECTOR3& vCenter,
                             float        fRadius,
                             D3DXVECTOR3& vTriA
                             )
{
    if (D3DXVec3LengthSq(&(vTriA - vCenter)) < (fRadius * fRadius))
        return true;

    return false;
}

inline bool IsLineCrossSphere(D3DXVECTOR3& vCenter,
                              float        fRadius,
                              D3DXVECTOR3& vStr,
                              D3DXVECTOR3& vDst,
                              D3DXVECTOR3* pInst
                              )
{
    D3DXVECTOR3 Inter;
    D3DXPLANE   Plane;

    D3DXPlaneFromPointNormal(&Plane, &vCenter, &(vDst - vStr));

    if (D3DXPlaneIntersectLine(&Inter, &Plane, &vStr, &vDst))
    {
        if (pInst)
            *pInst = Inter;

        if (D3DXVec3LengthSq(&(Inter - vCenter)) > (fRadius * fRadius))
            return false;

        D3DXVECTOR3 vI2S = Inter - vStr;
        D3DXVECTOR3 vI2D = Inter - vDst;

        D3DXVec3Normalize(&vI2S, &vI2S);
        D3DXVec3Normalize(&vI2D, &vI2D);

        if (D3DXVec3Dot(&vI2S, &vI2D) < -0.5f)
            return true;
    }

    return false;
}

inline bool IsVertexInTriangle(D3DXVECTOR3& vVertex,
                               D3DXVECTOR3& vTriA,
                               D3DXVECTOR3& vTriB,
                               D3DXVECTOR3& vTriC
                               )
{
    D3DXVECTOR3 vNormal;
    D3DXVECTOR3 vCross;

    D3DXVec3Cross(&vNormal, &(vTriB - vTriA), &(vTriC - vTriA));
    D3DXVec3Normalize(&vNormal, &vNormal);

    D3DXVec3Cross(&vCross, &(vTriB - vTriA), &(vVertex - vTriA));
    D3DXVec3Normalize(&vCross, &vCross);

    if (D3DXVec3Dot(&vCross, &vNormal) < -0.5f)
        return false;

    D3DXVec3Cross(&vCross, &(vTriC - vTriB), &(vVertex - vTriB));
    D3DXVec3Normalize(&vCross, &vCross);

    if (D3DXVec3Dot(&vCross, &vNormal) < -0.5f)
        return false;

    D3DXVec3Cross(&vCross, &(vTriA - vTriC), &(vVertex - vTriC));
    D3DXVec3Normalize(&vCross, &vCross);

    if (D3DXVec3Dot(&vCross, &vNormal) < -0.5f)
        return false;

    return true;
}

bool IsSphereCrossTriangle(D3DXVECTOR3& vCenter,
                           float        fRadius,
                           D3DXVECTOR3& vTriA,
                           D3DXVECTOR3& vTriB,
                           D3DXVECTOR3& vTriC,
                           bool         bCW,
                           ObstructdInfo* pInfo
)
{
    ASSERT(pInfo);

    D3DXVECTOR3 normal;
    D3DXPLANE   panel;
    D3DXVECTOR3 vDest;
    D3DXVECTOR3 vInst;

    D3DXPlaneFromPoints(&panel, &vTriA, &vTriB, &vTriC);

    normal.x = panel.a;
    normal.y = panel.b;
    normal.z = panel.c;

    if (bCW)
        normal *= -1.f;

    vDest = vCenter - normal * fRadius;

    if (D3DXPlaneIntersectLine(&vInst, &panel, &vCenter, &vDest) == NULL)
        return false;

    D3DXVECTOR3 vIst2Cer  = vCenter - vInst;
    float       fDistance = D3DXVec3Length(&vIst2Cer);

    if (fDistance > fRadius)
        return false;

    //if (!IsVertexInLine(vCenter, vDest, vInst))
    //    return false;

    // ----- case 1 -----

    if (IsVertexInTriangle(vInst, vTriA, vTriB, vTriC))
    {        
        pInfo->ObsType = CROSS_PIERCE;
        pInfo->vUpdate = vCenter + normal * (fRadius - fDistance + 0.5f);

        return true;
    }


    // ----- case 2 -----

    if (IsVertexInSphere(vCenter, fRadius, vTriA))   
    {
        /*vIst2Cer = vCenter - vTriA;
        fDistance = D3DXVec3Length(&vIst2Cer);
        D3DXVec3Normalize(&vIst2Cer, &vIst2Cer);
        
        pInfo->ObsType = CROSS_INSIDE;
        pInfo->vUpdate = vCenter + vIst2Cer * (fRadius - fDistance + 0.5f);*/

        pInfo->ObsType = CROSS_INSIDE;
        pInfo->vUpdate = vCenter + normal * (fRadius - fDistance + 0.5f);

        return true;
    }

    if (IsVertexInSphere(vCenter, fRadius, vTriB))   
    {
        /*vIst2Cer = vCenter - vTriB;
        fDistance = D3DXVec3Length(&vIst2Cer);
        D3DXVec3Normalize(&vIst2Cer, &vIst2Cer);

        pInfo->ObsType = CROSS_INSIDE;
        pInfo->vUpdate = vCenter + vIst2Cer * (fRadius - fDistance + 0.5f);*/

        pInfo->ObsType = CROSS_INSIDE;
        pInfo->vUpdate = vCenter + normal * (fRadius - fDistance + 0.5f);

        return true;
    }

    if (IsVertexInSphere(vCenter, fRadius, vTriC))   
    {
        /*vIst2Cer = vCenter - vTriC;
        fDistance = D3DXVec3Length(&vIst2Cer);
        D3DXVec3Normalize(&vIst2Cer, &vIst2Cer);

        pInfo->ObsType = CROSS_INSIDE;
        pInfo->vUpdate = vCenter + vIst2Cer * (fRadius - fDistance + 0.5f);*/

        pInfo->ObsType = CROSS_INSIDE;
        pInfo->vUpdate = vCenter + normal * (fRadius - fDistance + 0.5f);

        return true;
    }



    // ----- case 3 ----- 


    if (IsLineCrossSphere(vCenter, fRadius, vTriA, vTriB, &vInst))
    {
        /*vIst2Cer = vCenter - vInst;
        fDistance = D3DXVec3Length(&vIst2Cer);
        D3DXVec3Normalize(&vIst2Cer, &vIst2Cer);

        pInfo->ObsType = CROSS_TANGENCY;
        pInfo->vUpdate = vCenter + vIst2Cer * (fRadius - fDistance + 0.5f);*/

        pInfo->ObsType = CROSS_TANGENCY;
        pInfo->vUpdate = vCenter + normal * (fRadius - fDistance + 0.5f);

        return true;
    }

    if (IsLineCrossSphere(vCenter, fRadius, vTriB, vTriC, &vInst))
    {
        /*vIst2Cer = vCenter - vInst;
        fDistance = D3DXVec3Length(&vIst2Cer);
        D3DXVec3Normalize(&vIst2Cer, &vIst2Cer);

        pInfo->ObsType = CROSS_TANGENCY;
        pInfo->vUpdate = vCenter + vIst2Cer * (fRadius - fDistance + 0.5f);*/

        pInfo->ObsType = CROSS_TANGENCY;
        pInfo->vUpdate = vCenter + normal * (fRadius - fDistance + 0.5f);

        return true;
    }

    if (IsLineCrossSphere(vCenter, fRadius, vTriC, vTriA, &vInst))
    {
       /* vIst2Cer = vCenter - vInst;
        fDistance = D3DXVec3Length(&vIst2Cer);
        D3DXVec3Normalize(&vIst2Cer, &vIst2Cer);

        pInfo->ObsType = CROSS_TANGENCY;
        pInfo->vUpdate = vCenter + vIst2Cer * (fRadius - fDistance + 0.5f);*/

        pInfo->ObsType = CROSS_TANGENCY;
        pInfo->vUpdate = vCenter + normal * (fRadius - fDistance + 0.5f);

        return true;
    }


    return false;
}
