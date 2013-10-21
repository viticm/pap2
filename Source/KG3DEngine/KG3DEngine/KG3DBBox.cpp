////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DBBox.cpp
//  Version     : 1.0
//  Creator     : 
//  Create Date : 2008-8-14 10:41:21
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG3DBBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BBox::BBox()
{
	A = D3DXVECTOR3(0,0,0);
	B = D3DXVECTOR3(0,10,0);
	bInit = FALSE;
}

//BBox::BBox(FRECT Rect)
//{
//	D3DXVECTOR2 rA,rC;
//	Rect.ToPosition(&rA,&rC);
//
//	A = D3DXVECTOR3(rA.x,0,rA.y);
//	B = D3DXVECTOR3(rC.x,10,rC.y);
//	bInit = TRUE;
//}

BBox::BBox(D3DXVECTOR3 PosA,D3DXVECTOR3 PosB)
{
	A = PosA;
	B = PosB;
	bInit = TRUE;
}

void BBox::operator = (const BBox& Box)
{
	A = Box.A;
	B = Box.B;
	bInit = Box.bInit;
}

BOOL BBox::operator == (const BBox& Box)
{
	float x,y,z;
	x = A.x - Box.A.x;
	if (x<0)
		x = -x;
	y = A.y - Box.A.y;
	if (y<0)
		y = -y;
	z = A.z - Box.A.z;
	if (z<0)
		z = -z;
	if (x<FLT_EPSILON && y < FLT_EPSILON && z < FLT_EPSILON)
	{
		return TRUE;
	}
	return FALSE;
}

//FRECT BBox::ToRect()
//{
//	FRECT Rect;
//	Rect.Left = A.x;
//	Rect.Width = B.x - A.x;
//	Rect.Bottom = A.z;
//	Rect.Height = B.z - A.z;
//	return Rect;
//}
BBox& BBox::operator+= (const BBox& oth) {

	AddPosition(oth.A);
	AddPosition(oth.B);

	return *this;

} 
D3DXVECTOR3 BBox::GetCenter() const
{
	return (A+B)*0.5f;
}

float BBox::GetLength()const
{
	return (B.x - A.x);
}
float BBox::GetWidth()const
{
	return (B.z - A.z);
}
float BBox::GetHeight()const
{
	return (B.y - A.y);
}

void BBox::Clear()
{
	A = D3DXVECTOR3(0,0,0);
	B = D3DXVECTOR3(0,10,0);
	bInit = FALSE;
}

void BBox::AddPosition(D3DXVECTOR3 Pos)
{
	if(!bInit)
	{
		A = Pos;
		B = Pos;
		bInit = TRUE;
	}
	else
	{
		if (A.x > Pos.x) A.x = Pos.x;
		if (A.y > Pos.y) A.y = Pos.y;
		if (A.z > Pos.z) A.z = Pos.z;

		if (B.x < Pos.x) B.x = Pos.x;
		if (B.y < Pos.y) B.y = Pos.y;
		if (B.z < Pos.z) B.z = Pos.z;
	}
}

void BBox::AddY( FLOAT Y )
{
	if(Y < A.y)
		A.y = Y;
	if(Y > B.y)
		B.y = Y;
}

void BBox::AddY( const BBox& Box )
{
	AddY(Box.A.y);
	AddY(Box.B.y);
}
void BBox::AddPosition(D3DXVECTOR3* Pos,int nNum)
{
	if(!bInit)
	{
		A = Pos[0];
		B = Pos[0];
		bInit = TRUE;
	}

	for(int i=0;i<nNum;i++)
	{
		if (A.x > Pos[i].x) A.x = Pos[i].x;
		if (A.y > Pos[i].y) A.y = Pos[i].y;
		if (A.z > Pos[i].z) A.z = Pos[i].z;

		if (B.x < Pos[i].x) B.x = Pos[i].x;
		if (B.y < Pos[i].y) B.y = Pos[i].y;
		if (B.z < Pos[i].z) B.z = Pos[i].z;
	}

}

void BBox::AddPosition(D3DXVECTOR3 PosA,D3DXVECTOR3 PosB,D3DXMATRIX * pMatrix)
{
	D3DXVECTOR3 BoxPos[8];
	BBox NewBox(PosA,PosB);
	NewBox.TransformCoord(BoxPos,pMatrix);
	AddPosition(BoxPos,8);
}

void BBox::Extract(D3DXVECTOR3* pPositions)const
{
	float X_D = A.x;
	float Y_D = A.y;
	float Z_D = A.z;
	float X_U = B.x;
	float Y_U = B.y;
	float Z_U = B.z;

	pPositions[0] = D3DXVECTOR3(X_D,Y_U,Z_D);//A1 front_top_left		0
	pPositions[1] = D3DXVECTOR3(X_U,Y_U,Z_D);//B1 front_top_right		1
	pPositions[2] = D3DXVECTOR3(X_U,Y_U,Z_U);//C1 back_top_right		2
	pPositions[3] = D3DXVECTOR3(X_D,Y_U,Z_U);//D1 back_top_left			3
	pPositions[4] = D3DXVECTOR3(X_D,Y_D,Z_D);//A2 front_bottom_left		4
	pPositions[5] = D3DXVECTOR3(X_U,Y_D,Z_D);//B2 front_bottom_right	5
	pPositions[6] = D3DXVECTOR3(X_U,Y_D,Z_U);//C2 back_bottom_right		6
	pPositions[7] = D3DXVECTOR3(X_D,Y_D,Z_U);//D2 back_bottom_left		7
}

void BBox::Transform(const D3DXMATRIX& mat)
{
    D3DXVECTOR3 vPos[8];
    TransformCoord(vPos, &mat);
    AddPosition(vPos, 8);
}

void  BBox::TransformCoord(D3DXVECTOR3* pPositions,
						   const D3DXMATRIX * pMatrix)const
{
	Extract(pPositions);
	for(DWORD i=0;i<8;i++)
	{
		D3DXVec3TransformCoord(&pPositions[i],&pPositions[i],pMatrix);
	}
	return;
}

void BBox::Project(D3DXVECTOR3* pPositions,
				   D3DVIEWPORT9 *pViewport,
				   D3DXMATRIX *pProjection,
				   D3DXMATRIX *pView,
				   D3DXMATRIX *pWorld)
{
	Extract(pPositions);
	for(DWORD i=0;i<8;i++)
	{
		D3DXVec3Project(&pPositions[i],&pPositions[i],
			pViewport,pProjection,pView,pWorld);
	}
}

bool BBox::IsTwoBoxIntersect(const BBox &aabb) 
{
	if(A.x > aabb.B.x || B.x < aabb.A.x)
		return false;
	if(A.y > aabb.B.y || B.y < aabb.A.y)
		return false;
	if(A.z > aabb.B.z || B.z < aabb.A.z)
		return false;

	return true;
}

bool BBox::IsIntersectLine2D(float a, float b, float c, D3DXVECTOR2& s, D3DXVECTOR2& d)
{
	if (s.x >= A.x && s.x <= B.x && s.y >= A.z && s.y <= B.z)
		return true;

	if (d.x >= A.x && d.x <= B.x && d.y >= A.z && d.y <= B.z)
		return true;

	float min_x = min(s.x, d.x);
	float max_x = max(s.x, d.x);
	float min_y = min(s.y, d.y);
	float max_y = max(s.y, d.y);

	float y = 0.f;
	float x = 0.f;

	x = A.x;
	y = (-a * x - c) / b;
	if (y >= A.z && y <= B.z && x >= min_x && x <= max_x && y >= min_y && y <= max_y)
		return true;

	x = B.x;
	y = (-a * x - c) / b;
	if (y >= A.z && y <= B.z && x >= min_x && x <= max_x && y >= min_y && y <= max_y)
		return true;

	y = A.z;
	x = (-b * y - c) / a;
	if (x >= A.x && x <= B.x && x >= min_x && x <= max_x && y >= min_y && y <= max_y)
		return true;

	y = B.z;
	x = (-b * y - c) / a;
	if (x >= A.x && x <= B.x && x >= min_x && x <= max_x && y >= min_y && y <= max_y)
		return true;

	return false;
}




void BBox::GetXZRect(D3DXVECTOR2& vA,
					 D3DXVECTOR2& vC)
{
	vA.x = A.x;
	vA.y = A.z;
	vC.x = B.x;
	vC.y = B.z;
}




bool BBox::IsPointInside(const D3DXVECTOR3& vec3Point)
{
	if (vec3Point.x < min(A.x, B.x) || vec3Point.x > max(A.x, B.x))
		return false;

	if (vec3Point.y < min(A.y, B.y) || vec3Point.y > max(A.y, B.y))
		return false;

	if (vec3Point.z < min(A.z, B.z) || vec3Point.z > max(A.z, B.z))
		return false;

	return true;
}

BOOL BBox::IsPointInside2D(float fx,float fz)
{
	if (fx < A.x || fx > B.x)
		return FALSE;

	if (fz < A.z || fz > B.z)
		return FALSE;

	return TRUE;
}

HRESULT BBox::ComputeOrthoCameraWithLight(D3DXVECTOR3& vCameraPos,D3DXVECTOR3& vCameraFront,
										  float& fWidth,float& fHeight,float& fFar ,
										  const D3DXVECTOR3& vLightDirction)
{
	D3DXVECTOR3 BoxPos[8];

	this->Extract(BoxPos);
	D3DXVECTOR3 C = B - A;
	float Length = D3DXVec3Length(&C);
	D3DXVECTOR3 Center = GetCenter();
	D3DXVECTOR3 Org = Center - vLightDirction * Length * 5.0f;

	D3DXMATRIX matView,matProj;
	C = D3DXVECTOR3(0.0f,1.0f,0.0f);
	D3DXMatrixLookAtLH(&matView,&Org,&Center,&D3DXVECTOR3(0,1,0));
	this->TransformCoord(BoxPos,&matView);

	float w = 0;
	float h = 0;
	float f = 0;

	for(int i = 0; i < 8; ++i)
	{
		w = max(w,abs(BoxPos[i].x));
		h = max(h,abs(BoxPos[i].y));
		f = max(f,abs(BoxPos[i].z));
	}

	vCameraPos = Org;
	vCameraFront = vLightDirction;

	fWidth = w;
	fHeight = h;
	fFar = f;

	return S_OK;
}


BOOL BBox::IsRayIntersect3D(D3DXVECTOR3& Inter,const D3DXVECTOR3& rStartPos,const D3DXVECTOR3& rDir,
							D3DXMATRIX* pMatrix/* = NULL */,float* pfOutHitDis/* = NULL */)
{

	_ASSERTE(NULL != rStartPos && NULL != rDir);
	FLOAT fTempRet = 0;
	BOOL bRet = this->RayIntersection(rStartPos, rDir, &fTempRet, pMatrix);
	if (bRet)
	{
		Inter = rStartPos + rDir * fTempRet;
		if (NULL != pfOutHitDis)
		{
			*pfOutHitDis = fTempRet;
		}
	}
	return bRet;

	//下面的不再用，改用RayIntersection里面的算法，速度高的多

	/*BOOL bHit = FALSE;
	float fMinDis = 1000000000.0f;
	float fHitDis = 1000000000.0f;
	D3DXVECTOR3 BoxPos[8];
	Extract(BoxPos);
	if (pMatrix)
	{
		for (int i =0 ; i < 8 ; i++)
		{
			D3DXVec3TransformCoord(&BoxPos[i],&BoxPos[i],pMatrix);
		}
	}

	if(D3DXIntersectTri(&BoxPos[0],&BoxPos[1],&BoxPos[2],&rStartPos,&rDir,NULL,NULL,&fMinDis))
	{
		bHit = TRUE;
		if(fHitDis > fMinDis)
			fHitDis = fMinDis;
	}
	if(D3DXIntersectTri(&BoxPos[0],&BoxPos[3],&BoxPos[2],&rStartPos,&rDir,NULL,NULL,&fMinDis))
	{
		bHit = TRUE;
		if(fHitDis > fMinDis)
			fHitDis = fMinDis;
	}
	if(D3DXIntersectTri(&BoxPos[0],&BoxPos[1],&BoxPos[4],&rStartPos,&rDir,NULL,NULL,&fMinDis))
	{
		bHit = TRUE;
		if(fHitDis > fMinDis)
			fHitDis = fMinDis;
	}
	if(D3DXIntersectTri(&BoxPos[1],&BoxPos[4],&BoxPos[5],&rStartPos,&rDir,NULL,NULL,&fMinDis))
	{
		bHit = TRUE;
		if(fHitDis > fMinDis)
			fHitDis = fMinDis;
	}
	if(D3DXIntersectTri(&BoxPos[1],&BoxPos[5],&BoxPos[6],&rStartPos,&rDir,NULL,NULL,&fMinDis))
	{
		bHit = TRUE;
		if(fHitDis > fMinDis)
			fHitDis = fMinDis;
	}
	if(D3DXIntersectTri(&BoxPos[1],&BoxPos[2],&BoxPos[6],&rStartPos,&rDir,NULL,NULL,&fMinDis))
	{
		bHit = TRUE;
		if(fHitDis > fMinDis)
			fHitDis = fMinDis;
	}
	if(D3DXIntersectTri(&BoxPos[2],&BoxPos[6],&BoxPos[7],&rStartPos,&rDir,NULL,NULL,&fMinDis))
	{
		bHit = TRUE;
		if(fHitDis > fMinDis)
			fHitDis = fMinDis;
	}
	if(D3DXIntersectTri(&BoxPos[2],&BoxPos[3],&BoxPos[7],&rStartPos,&rDir,NULL,NULL,&fMinDis))
	{
		bHit = TRUE;
		if(fHitDis > fMinDis)
			fHitDis = fMinDis;
	}
	if(D3DXIntersectTri(&BoxPos[0],&BoxPos[4],&BoxPos[7],&rStartPos,&rDir,NULL,NULL,&fMinDis))
	{
		bHit = TRUE;
		if(fHitDis > fMinDis)
			fHitDis = fMinDis;
	}
	if(D3DXIntersectTri(&BoxPos[0],&BoxPos[3],&BoxPos[7],&rStartPos,&rDir,NULL,NULL,&fMinDis))
	{
		bHit = TRUE;
		if(fHitDis > fMinDis)
			fHitDis = fMinDis;
	}
	if(D3DXIntersectTri(&BoxPos[4],&BoxPos[5],&BoxPos[6],&rStartPos,&rDir,NULL,NULL,&fMinDis))
	{
		bHit = TRUE;
		if(fHitDis > fMinDis)
			fHitDis = fMinDis;
	}
	if(D3DXIntersectTri(&BoxPos[4],&BoxPos[7],&BoxPos[6],&rStartPos,&rDir,NULL,NULL,&fMinDis))
	{
		bHit = TRUE;
		if(fHitDis > fMinDis)
			fHitDis = fMinDis;
	}
	if (pfOutHitDis)
	{
		*pfOutHitDis = fHitDis;
	}

	Inter = rStartPos + fHitDis * rDir;
	return bHit;*/

}

void BBox::GetPlane(D3DXPLANE* pPlane)
{
	D3DXVECTOR3 BoxPos[8];
	this->Extract(BoxPos);
	D3DXPlaneFromPoints(&pPlane[0],&BoxPos[0],&BoxPos[1],&BoxPos[5]);  //front
	D3DXPlaneFromPoints(&pPlane[1],&BoxPos[2],&BoxPos[3],&BoxPos[7]);  //back
	D3DXPlaneFromPoints(&pPlane[2],&BoxPos[0],&BoxPos[1],&BoxPos[2]);  //top
	D3DXPlaneFromPoints(&pPlane[3],&BoxPos[4],&BoxPos[5],&BoxPos[6]);  //bottom
	D3DXPlaneFromPoints(&pPlane[4],&BoxPos[0],&BoxPos[4],&BoxPos[7]);  //left
	D3DXPlaneFromPoints(&pPlane[5],&BoxPos[1],&BoxPos[5],&BoxPos[6]);  //right

}

bool BBox::IsIntersectRect( D3DXVECTOR2& vA,D3DXVECTOR2& vC )
{
	if((vA.x <= B.x)&&(vA.y <= B.z)&&
		(vC.x >= A.x)&&(vC.y >= A.z))
		return true;
	else
		return false;
}

BOOL BBox::RayIntersection( const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vNormalizedDir, FLOAT* pfDistance, const D3DXMATRIX* pMatrix /*= NULL*/ )
{
	if(NULL == pMatrix)
	{
		return RayIntersectionPrivate(vSrc, vNormalizedDir, pfDistance, TRUE);
	}

	//可以用有向有界箱的算法加速，不过这里先悠着点，有空再写
	D3DXMATRIX MatrixInverse;
	D3DXMATRIX* pInversedMatrix = D3DXMatrixInverse(&MatrixInverse, NULL, pMatrix);

	if (NULL == pInversedMatrix)//变换失败了
		return FALSE;

	D3DXVECTOR3 vNewSrc, vNewNormalizedDir;
	D3DXVec3TransformCoord(&vNewSrc, &vSrc, &MatrixInverse);
	D3DXVec3TransformCoord(&vNewSrc, &vNormalizedDir, &MatrixInverse);

	FLOAT fNewLength = D3DXVec3Length(&vNewNormalizedDir);

	if (fNewLength < FLT_EPSILON)//用了个很奇怪的变换才会这样吧。。。。。
		return FALSE;

	//变换之后要重新归一化
	D3DXVec3Normalize(&vNewNormalizedDir, &vNewNormalizedDir);

	FLOAT fRetTemp = FLT_MAX;
	BOOL bRet = RayIntersectionPrivate(vNewSrc, vNewNormalizedDir, &fRetTemp, TRUE);
	if (bRet)
	{
		if (pfDistance)
		{
			*pfDistance = fRetTemp * 1 / fNewLength;//原来的Normal的Length肯定是1的
		}
	}
	return bRet;
}
//BOOL BBox::RayIntersection2D( const D3DXVECTOR2& vSrc2D, const D3DXVECTOR2& vNormalizedDir2D, FLOAT* pfDistance )
//{
//	return this->RayIntersection(D3DXVECTOR3(vSrc2D.x, 0, vSrc2D.y)
//		, D3DXVECTOR3(vNormalizedDir2D.x, 0, vNormalizedDir2D.y)
//		, pfDistance
//		);
//}
BOOL BBox::RayIntersectionPrivate( const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vNormalizedDir, FLOAT* pfDistance, BOOL bCheckZ )
{
	//在《计算机图形学 几何工具算法详解》里面抄了一段(P461-线性对象与轴对齐有界箱)。不需要用D3DXIntersectTri。
	//下面的T都是直线隐式P(t) = vSrc + vNormalizedDir * t的T
	FLOAT tNear = -FLT_MAX;//FLT_MIN居然不是负数，怎么想的。。。。。。
	FLOAT tFar = FLT_MAX;

	//下面要野蛮的访问D3DXVECTOR3里面的成员
	_ASSERTE(sizeof(D3DXVECTOR3) == sizeof(FLOAT)*3);
	_ASSERTE(_offsetof(D3DXVECTOR3, x) == 0 
		&& _offsetof(D3DXVECTOR3, y) == sizeof(FLOAT)
		&& _offsetof(D3DXVECTOR3, z) == sizeof(FLOAT)*2);

	const INT nBoundary = bCheckZ ? 3 : 2;
	for (int i = 0; i < nBoundary; ++i)
	{
		/*
		在各个轴向上延伸，那么可以认为x,y,z轴上，Box形成了6个面。
		那么分别和6个面求交，交点Tx = (Xmax - vSRCx)/ vDIRx。其它轴同理
		而且可以从近交点和远交点的位置判断出交点在盒子上还是盒子外。
		*/
		FLOAT PlaneMin = ((const FLOAT*)&A)[i];
		FLOAT PlaneMax = ((const FLOAT*)&B)[i];
		if (PlaneMin > PlaneMax)
		{
			FLOAT fTemp = PlaneMin;
			PlaneMin = PlaneMax;
			PlaneMax = fTemp;
		}
		FLOAT SrcP = ((const FLOAT*)&vSrc)[i];
		FLOAT DirP = ((const FLOAT*)&vNormalizedDir)[i];

		//检查平行的情况，如果平行，且起点在两个面外，当然不会相交
		if (abs(DirP) < 0.001)
		{
			if (SrcP < PlaneMin || SrcP > PlaneMax)
				return FALSE;
			else
				continue;
		}

		FLOAT TMin = (PlaneMin - SrcP) / DirP;	//上面的检查也顺便避免了0除
		FLOAT TMax = (PlaneMax - SrcP) / DirP;

		if (TMin > TMax)
		{
			FLOAT Temp = TMin;
			TMin = TMax;
			TMax = Temp;
		}

		if (TMin > tNear)
			tNear = TMin;

		if (TMax < tFar)
			tFar = TMax;


		if (tNear > tFar)//交叉相交，线飞过Box
			return FALSE;

		if (tFar < 0)//起点在另外一边
			return FALSE;

	}

	FLOAT fRet = tNear > 0 ? tNear : tFar;
	_ASSERTE(fRet > -FLT_EPSILON);

	if (NULL != pfDistance)
	{
		*pfDistance = fRet;		
	}

	return TRUE;
}
//本来轴向有界合的两个Vector是应该叫min,max的，这样在比较的时候能省不少指令
BOOL BBox::IsSorted()
{
	return A.x <= B.x && A.y <= B.y && A.z <= B.z;
}

BOOL BBox::IsPointInsideSorted( const D3DXVECTOR3& vPoint )
{
	_ASSERTE(IsSorted());
	if (vPoint.x < A.x || vPoint.x > B.x)
		return FALSE;
	if(vPoint.y < A.y || vPoint.y > B.y)
		return FALSE;
	if(vPoint.z < A.z || vPoint.z > B.z)
		return FALSE;

	return TRUE;	
}

float BBox::GetDiameter()const
{
	return D3DXVec3Length(&(B - A));
}

#if defined(KG_ENABLE_TDD)
KG_TEST_BEGIN(BBox)
{
	//测试RayIntersection
	{
		struct RayIntersectionBoxTestCase 
		{
			D3DXVECTOR3 vSrc;
			D3DXVECTOR3 vNormalizedDir;
			BOOL		bRet;
			FLOAT		fRet;
		};

		BBox Box(D3DXVECTOR3(0,0,0), D3DXVECTOR3(4,4,4));
		RayIntersectionBoxTestCase TestCaseArray[] = 
		{
			{D3DXVECTOR3(-4,0,0), D3DXVECTOR3(1,0,0), TRUE, 4}
			,{D3DXVECTOR3(1,4,1), D3DXVECTOR3(0,-1,0), TRUE, 4}
			,{D3DXVECTOR3(1,0,1), D3DXVECTOR3(1,0,0), TRUE, 3}
			,{D3DXVECTOR3(1,1,-1), D3DXVECTOR3(0,0,1), TRUE, 1}
			,{D3DXVECTOR3(0,-1,0), D3DXVECTOR3(0,1,0), TRUE, 1}
			,{D3DXVECTOR3(-4,0,0), D3DXVECTOR3(1,0,1), TRUE, 4 * sqrt(2.0f)}
			,{D3DXVECTOR3(-4,0,0), D3DXVECTOR3(1,0,1.1f), FALSE, 0}
		};

		
		for (size_t i = 0; i < _countof(TestCaseArray); ++i)
		{
			D3DXVec3Normalize(&TestCaseArray[i].vNormalizedDir, &TestCaseArray[i].vNormalizedDir);
			//3D的碰撞
			{
				FLOAT fDistane = FLT_MAX;
				BOOL bRet = Box.RayIntersection(TestCaseArray[i].vSrc
					, TestCaseArray[i].vNormalizedDir, &fDistane, NULL);
				if(TestCaseArray[i].bRet)
				{
					_ASSERTE(bRet && MT_FLOAT_EQUAL_BIAS(TestCaseArray[i].fRet, fDistane, 0.0001));
				}
				else
				{
					_ASSERTE(! bRet);
				}
			}
		}
	}

	//测试AddY
	{
		BBox TempBox(D3DXVECTOR3(-10,-10,-10), D3DXVECTOR3(10, 10, 10));
		//应该没有改变
		TempBox.AddY(10);
		_ASSERTE(MT_FLOAT_EQUAL(TempBox.B.y, 10));
		TempBox.AddY(-10);
		_ASSERTE(MT_FLOAT_EQUAL(TempBox.A.y, -10));
		TempBox.AddY(TempBox);
		_ASSERTE(MT_FLOAT_EQUAL(TempBox.A.y, -10));
		//撑大
		TempBox.AddY(20);
		_ASSERTE(MT_FLOAT_EQUAL(TempBox.B.y, 20));
		_ASSERTE(MT_FLOAT_EQUAL(TempBox.B.x, 10) && MT_FLOAT_EQUAL(TempBox.B.z, 10));
		TempBox.AddY(-20);
		_ASSERTE(MT_FLOAT_EQUAL(TempBox.A.y, -20));
		_ASSERTE(MT_FLOAT_EQUAL(TempBox.A.x, -10) && MT_FLOAT_EQUAL(TempBox.A.z, -10));

		//一边撑大
		{
			BBox TempBoxToAdd(D3DXVECTOR3(-5,-5,-5), D3DXVECTOR3(30,30,30));
			TempBox.AddY(TempBoxToAdd);
			_ASSERTE(MT_FLOAT_EQUAL(TempBox.B.y, 30));
			_ASSERTE(MT_FLOAT_EQUAL(TempBox.A.x, -10) && MT_FLOAT_EQUAL(TempBox.A.z, -10));
		}

		//双边撑大
		{
			BBox TempBoxToAdd(D3DXVECTOR3(-50,-50,-50), D3DXVECTOR3(50,50,50));
			TempBox.AddY(TempBoxToAdd);
			_ASSERTE(MT_FLOAT_EQUAL(TempBox.B.y, 50));
			_ASSERTE(MT_FLOAT_EQUAL(TempBox.A.x, -10) && MT_FLOAT_EQUAL(TempBox.A.z, -10));
		}

	}
	//测试IsPointInsideSorted
	{
		BBox BoxTemp(D3DXVECTOR3(3,3,3), D3DXVECTOR3(1,1,1));
		_ASSERTE(! BoxTemp.IsSorted());
		BoxTemp.A.y = -3;
		_ASSERTE(! BoxTemp.IsSorted());
		BoxTemp.A.x = -3;
		_ASSERTE(! BoxTemp.IsSorted());
		BoxTemp.A.z = -3;
		_ASSERTE(BoxTemp.IsSorted());

		_ASSERTE(BoxTemp.IsPointInsideSorted(D3DXVECTOR3(0,0,0)));
		_ASSERTE(! BoxTemp.IsPointInsideSorted(D3DXVECTOR3(1+FLT_EPSILON,0,0)));
		_ASSERTE(! BoxTemp.IsPointInsideSorted(D3DXVECTOR3(0,1+FLT_EPSILON,0)));
		_ASSERTE(! BoxTemp.IsPointInsideSorted(D3DXVECTOR3(0,0,-3.001f)));
	}
	//_KG3D_DEBUG_OUTPUT0("GraphicsStruct Runtime Test Ok\n");

}
KG_TEST_END(BBox)
#endif

////////////////////////////////////////////////////////////////////////////////

