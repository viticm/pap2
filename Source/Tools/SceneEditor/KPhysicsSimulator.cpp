/*
**************************************************************************
*
* Project		: SceneEditor
* FileName		: KPhysicsSimulator.cpp
* Coded by		: HJQ
* Date			: 2004-05-27
* Comment	    : 物理模拟系统
* Copyright 2004 Kingsoft Corp.
* All Rights Reserved.
*
**************************************************************************
*/

// KPhysicsSimulator.cpp: implementation of the KPhysicsSimulator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sceneeditor.h"
#include "KPhysicsSimulator.h"
//#include ".\kphysicssimulator.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace Physics
{

//#define NoPenetration


KPhysicsSimulator::KPhysicsSimulator()
{
	m_dwLastTime = 0;
	m_bAddingGravity = TRUE;
	m_bUsingFriction = FALSE;
//	m_bNonePenetration = FALSE;
	m_fEpsilon = 1.0f;
}

KPhysicsSimulator::~KPhysicsSimulator()
{
	listRigidBody.clear();
	listConstraint.clear();
	vecCollisionInfo.clear();
	listPlane.clear();
	vecStoredData.clear();
}

/**
  * @brief 为场景添加一个刚体
  */
HRESULT KPhysicsSimulator::AddRigidBody(LPRIGIDBODY pRigid)
{
	listRigidBody.remove(pRigid);
	listRigidBody.push_back(pRigid);
	return S_OK;
}


/**
  * @brief 从场景中删去一个刚体
  */
HRESULT KPhysicsSimulator::RemoveRigidBody(LPRIGIDBODY pRigid)
{
	listRigidBody.remove(pRigid);
	return S_OK;
}

/**
  * @brief 为场景添加一个约束
  */
HRESULT KPhysicsSimulator::AddConstraint(KConstraint* pConstraint)
{
	listConstraint.remove(pConstraint);
	listConstraint.push_back(pConstraint);
	return S_OK;
}


/**
  * @brief 从场景中删去一个约束
  */
HRESULT KPhysicsSimulator::RemoveConstraint(KConstraint* pConstraint)
{
	listConstraint.remove(pConstraint);
	return S_OK;
}


HRESULT KPhysicsSimulator::FrameMove()
{
	DWORD NowTime = timeGetTime();
	float TinyTime = 0;
	if(m_dwLastTime!=0)
	{
		TinyTime = (NowTime - m_dwLastTime)*0.001f;
	}

	//begin 模拟圆周运动
//	SimulationTest( TinyTime );
	//end 模拟圆周运动
	if ( TinyTime < 1 )
	{
//	PlaneCollisionSimulation( TinyTime );
//	TestRotation( TinyTime );
//	TestConstraintMaxDist( TinyTime );
//	TestFixedPoint( TinyTime );
	DoTimeStep( TinyTime );
	}

//	CollisionSimulation( TinyTime );

	m_dwLastTime = NowTime;
	return S_OK;
}

HRESULT KPhysicsSimulator::Render()
{
	list<LPRIGIDBODY>::iterator i = listRigidBody.begin();
	while(i!=listRigidBody.end())
	{
		LPRIGIDBODY pRigid = *i;

		//g_cGraphicsTool.DrawPosition(&pRigid->CtrPosition);
		//g_cGraphicsTool.DrawCircleFace( pRigid->CtrPosition, v1, 5.0f, 1, 0xffff0000, 1, 1.0);
		//g_cGraphicsTool.DrawSphere( pRigid->CtrPosition, 1.0f, 0, 0xffff0000 );

		g_cGraphicsTool.DrawSP( pRigid->pSp );
		//g_cGraphicsTool.DrawPosition( &pRigid->pSp->CtrPosition );
	
		//if( vecCollisionInfo.size()>=1 )
		//{
		//	g_cGraphicsTool.DrawPosition( &vecCollisionInfo[0].m_ColPosition );
		//}

		i++;
	}

	return S_OK;
}

/**
  * @brief 用于判断两点是否在某平面的异侧(辅助函数)
  * @param s 平面上一定点
  * @param normal 平面的法向量
  * @return TRUE 表示两点在平面的异侧
  */
BOOL KPhysicsSimulator::CheckVetexStatus(const D3DXVECTOR3& normal, const D3DXVECTOR3& point,const D3DXVECTOR3& vec1,const D3DXVECTOR3& vec2)
{

	FLOAT D=D3DXVec3Dot( &normal, &point ); //平面到原点的距离
	FLOAT D1=D3DXVec3Dot( &normal, &vec1 ); 
	FLOAT D2=D3DXVec3Dot( &normal, &vec2 ); 

	
	if ( (D1<D)&&(D2>D) || (D1>D)&&(D2<D) )
		return TRUE;
	
	return FALSE;
}

/**
  * @brief 本函数仅仅检测两刚体是否相交(如果XMesh.cpp中的函数UpdateFlexibleBone()不再使用本函数,可删除本函数)
  */
BOOL KPhysicsSimulator::CheckCollision(LPRIGIDBODY pRigid1, LPRIGIDBODY pRigid2)
{

	float r1 = pRigid1->pSp->Radius + pRigid1->pSp->Height ;
	float r2 = pRigid2->pSp->Radius + pRigid2->pSp->Height ;

	if( !CheckSphereIntersect( pRigid1->pSp->CtrPosition, r1, pRigid2->pSp->CtrPosition, r2 ) ) //判断两刚体的球包围盒是否接触
	{
		return FALSE;
	}
	else
	{
		if( ( pRigid1->pSp->Height == 0 ) && ( pRigid2->pSp->Height == 0 ) )  // 两个刚体均为球体，中心点距离等于半径之和
		{
			return TRUE;
		}
		else 
			if( pRigid1->pSp->Height == 0 )  //球体与LSS接触情况
			{

				return SphereSpcyIntersect( pRigid2->pSp, pRigid1->pSp->CtrPosition, pRigid1->pSp->Radius );

			}
			else 
				if ( pRigid2->pSp->Height == 0 ) //LSS与球体接触情况
				{
					return SphereSpcyIntersect( pRigid1->pSp, pRigid2->pSp->CtrPosition, pRigid2->pSp->Radius );
				}
				else // LSS与LSS接触情况
				{

					LPAABB A = new _AABB;
					LPAABB B = new _AABB;

					CreateAABB( A, pRigid1->pSp );
					CreateAABB( B, pRigid2->pSp );

					BOOL bAABBInter = FALSE;
					bAABBInter = AABBIntersect( A, B );

					SAFE_DELETE( A );
					SAFE_DELETE( B );

					if( bAABBInter ) //AABB包围盒相交测试
					{
						D3DXVECTOR3 vec[2][2];
						vec[0][0] = pRigid1->pSp->CtrPosition + pRigid1->pSp->Height * pRigid1->pSp->Normal;
						vec[0][1] = pRigid1->pSp->CtrPosition - pRigid1->pSp->Height * pRigid1->pSp->Normal;
						vec[1][0] = pRigid2->pSp->CtrPosition + pRigid2->pSp->Height * pRigid2->pSp->Normal;
						vec[1][1] = pRigid2->pSp->CtrPosition - pRigid2->pSp->Height * pRigid2->pSp->Normal;

						BOOL mParal;
						float dist = LineDist( vec[0][0], vec[0][1], vec[1][0], vec[1][1], mParal );

						if( dist > ( pRigid1->pSp->Radius + pRigid2->pSp->Radius ) )
							return FALSE;
						else
						{
							//球面与球面的接触情况
							D3DXVECTOR3 vecDist;

							for ( int i = 0; i < 2; i++ )
								for ( int j = 0; j < 2; j++ )
								{
									vecDist = vec[0][i] - vec[1][j];
									if ( D3DXVec3Length( &vecDist ) <= ( pRigid1->pSp->Radius + pRigid2->pSp->Radius ) )
										return TRUE;
								}


							if( ( dist < 0.0f ) && ( dist > - 0.0f ) ) // 两轴共面相交, 球与LSS接触情况
							{

								if( LineSegIntersect( vec[0][0], vec[0][1], vec[1][0], vec[1][1] ) )
								{
									return TRUE;
								}
								else
								{
									//
									D3DXVECTOR3 vecDist;

									float Dist = 0.0f;

									for ( int i = 0; i < 2; i++ )
										for ( int j = 0; j < 2; j++ )
										{
											Dist = VertexLineDist( vec[i][j], vec[1-i][0], vec[1-i][1] );
											if ( ( Dist <= ( pRigid1->pSp->Radius + pRigid2->pSp->Radius ) ) &&
												IsAcuteAngle( vec[i][j], vec[1-i][0], vec[1-i][1] ) &&
												IsAcuteAngle( vec[i][j], vec[1-i][1], vec[1-i][0] ) )
												return TRUE;
										}

										return FALSE;
								}

							}
							else 
								if( mParal ) //两轴共面平行
								{
									if( ( IsAcuteAngle( vec[0][0], vec[1][0], vec[1][1] ) &&  IsAcuteAngle( vec[0][0], vec[1][1], vec[1][0] ) ) ||
										( IsAcuteAngle( vec[1][0], vec[1][0], vec[1][1] ) &&  IsAcuteAngle( vec[1][0], vec[1][1], vec[1][0] ) ) )
										return TRUE;
									else 
									{
										return FALSE;
									}

								}
								else //两轴异面
								{
									if( CheckSeparateVertex( vec[0][0], vec[0][1], vec[1][0], vec[1][1] ) )
										return TRUE;
									else //转化为球与LSS
									{

										//
										D3DXVECTOR3 vecDist;
										float Dist = 0.0f;
		
										for ( int i = 0; i < 2; i++ )
											for ( int j = 0; j < 2; j++ )
											{
												Dist = VertexLineDist( vec[i][j], vec[1-i][0], vec[1-i][1] );
												if ( ( Dist <= ( pRigid1->pSp->Radius + pRigid2->pSp->Radius ) ) &&
													  IsAcuteAngle( vec[i][j], vec[1-i][0], vec[1-i][1] ) &&
													  IsAcuteAngle( vec[i][j], vec[1-i][1], vec[1-i][0] ) )
												return TRUE;
											}


										return FALSE;

									}

								}

						}

					}
					else
					{
						return FALSE;
					}

				}

	}

	return FALSE;

}



/**
  * @brief 用于判断两球体是否相交
  * @return TRUE 表示两球体相交
  */
BOOL KPhysicsSimulator::CheckSphereIntersect( const D3DXVECTOR3& vec1,  float r1, const D3DXVECTOR3& vec2,  float r2)
{

	D3DXVECTOR3 v0;
	D3DXVec3Subtract( &v0, &vec1, &vec2 );
	
	float d = D3DXVec3Length( &v0 );
	
	if ( d <= ( r1 + r2 ) )
		return TRUE;

	return FALSE;
}

/**
  * @brief 用于判断球体与某AABB包围盒相交
  * @return TRUE 表示相交
  */
BOOL KPhysicsSimulator::SphereAABBIntersect( const D3DXVECTOR3& ct,  float r, const LPAABB A)
{
	float d = 0.0f;

	if( ct.x < A->min.x ) 
		d += ( ct.x - A->min.x )*( ct.x - A->min.x );
	else if( ct.x > A->max.x ) 
		d += ( ct.x - A->max.x )*( ct.x - A->max.x );

	if( ct.y < A->min.y ) 
		d += ( ct.y - A->min.y )*( ct.y - A->min.y );
	else if( ct.y > A->max.y ) 
		d += ( ct.y - A->max.y )*( ct.y - A->max.y );

	if( ct.z < A->min.z ) 
		d += ( ct.z - A->min.z )*( ct.z - A->min.z );
	else if( ct.z > A->max.z ) 
		d += ( ct.z - A->max.z )*( ct.z - A->max.z );

	if( d > r*r )
		return FALSE;

	return TRUE;
}

/**
  * @brief 用于判断两AABB包围盒是否相交
  * @return TRUE 表示相交
  */
BOOL KPhysicsSimulator::AABBIntersect( const LPAABB A, const LPAABB B)
{

	if( ( A->min.x > B->max.x ) || ( B->min.x > A->max.x ) ) return FALSE;
	if( ( A->min.y > B->max.y ) || ( B->min.y > A->max.y ) ) return FALSE;
	if( ( A->min.z > B->max.z ) || ( B->min.z > A->max.z ) ) return FALSE;

	return TRUE;
}

/**
  * @brief 创建Spcy的AABB包围盒
  */
BOOL KPhysicsSimulator::CreateAABB(LPAABB A, KRigidBody::LPSPCY pSp)
{
	D3DXVECTOR3 Vmax,Vmin;
	D3DXVECTOR3 v1,v2;
	float r = pSp->Radius;
	v1 = pSp->CtrPosition + pSp->Height*pSp->Normal;
	v2 = pSp->CtrPosition - pSp->Height*pSp->Normal;

	
	D3DXVec3Maximize( &Vmax, &v1, &v2 );
	D3DXVec3Minimize( &Vmin, &v1, &v2 );

	A->max = Vmax + D3DXVECTOR3 ( r, r, r );
	A->min = Vmin - D3DXVECTOR3 ( r, r, r );

	return 0;

}

/**
  * @brief 求某点到一直线的距离
  * @param V0 某定点
  * @param LV1 LV2 直线上不重合的两点
  * @return 返回点到直线的距离值
  */
float KPhysicsSimulator::VertexLineDist(const D3DXVECTOR3& V0, const D3DXVECTOR3& LV1, const D3DXVECTOR3& LV2) 
{
	D3DXVECTOR3 v1v0;
	D3DXVECTOR3 v1v2;
	D3DXVECTOR3 vDot;
	D3DXVECTOR3 vResult;
	float dist = 0.0f;

	v1v0 = V0 - LV1 ;
	v1v2 = LV2 - LV1;

	D3DXVec3Normalize( &v1v2, &v1v2 );
	vDot = D3DXVec3Dot( &v1v0, &v1v2)*v1v2;
	vResult = v1v0 - vDot;
	dist = D3DXVec3Length(&vResult);

	return dist;

}


/**
  * @brief 求两直线的距离
  * @param V0 某定点
  * @param L1V1 L1V2 直线1不重合的两点
  * @param L2V1 L2V2 直线2不重合的两点
  * @return 两直线共面相交时，计算结果为0,两直线平行时，返回paral为TRUE
  */
float KPhysicsSimulator::LineDist(const D3DXVECTOR3& L1V1, const D3DXVECTOR3& L1V2, const D3DXVECTOR3& L2V1, const D3DXVECTOR3& L2V2, BOOL & paral) 
{
	D3DXVECTOR3 L1;
	D3DXVECTOR3 L2;
	D3DXVECTOR3 vCross;
	paral = FALSE;

	L1 = L1V2 - L1V1;
	L2 = L2V2 - L2V1;
		
	D3DXVec3Cross( &vCross, &L1, &L2 );

//	if( IsZeroVector( vCross ) ) // 两向量平行
	if( D3DXVec3Length( &vCross ) <= 0.01f ) // 两向量平行
	{
		paral = TRUE;
		return VertexLineDist( L1V1, L2V1, L2V2 );
	}
	else 
	{
		D3DXVECTOR3 L1V1L2V1;
		float mDot = 0.0f;
		
		L1V1L2V1 = L2V1 - L1V1;

		D3DXVec3Normalize( &vCross, &vCross );
		
		mDot = D3DXVec3Dot( &L1V1L2V1, &vCross );
		

		return fabs( mDot );  //异面直线距离

	}
}

//BOOL KPhysicsSimulator::IsZeroVector(const D3DXVECTOR3& vec)
//{
//	if( ( vec.x < 0.0f ) && ( vec.x > - 0.0f ) )
//		if( ( vec.y < 0.0f ) && ( vec.y > - 0.0f ) )
//			if( ( vec.z < 0.0f ) && ( vec.z > - 0.0f ) )
//				return TRUE;
//
//	return FALSE;
//}
//


/**
  * @brief 判断一个角是否为锐角（角vec1 vec2 vec3）
  * @return TRUE 表示是锐角
  */
BOOL KPhysicsSimulator::IsAcuteAngle(const D3DXVECTOR3& vec1, const D3DXVECTOR3& vec2, const D3DXVECTOR3& vec3)
{
	D3DXVECTOR3 vt1,vt2;
	float mDot = 0.0f ;
	vt1 = vec1 - vec2;
	vt2 = vec3 - vec2;
	mDot = D3DXVec3Dot( &vt1, &vt2 );
	
	if( mDot >= 0 )
		return TRUE;
	else
		return FALSE;
}

/**
  * @brief 判断四个点的异面情况
  */
BOOL KPhysicsSimulator::CheckSeparateVertex(const D3DXVECTOR3& v1, const D3DXVECTOR3& v2, const D3DXVECTOR3& v3, const D3DXVECTOR3& v4)
{

	D3DXVECTOR3 L1;
	D3DXVECTOR3 L2;
	D3DXVECTOR3 n1;
	D3DXVECTOR3 n2;
	D3DXVECTOR3 vCross;

	L1 = v2 - v1;
	L2 = v4 - v3;
	
	D3DXVec3Cross( &vCross, &L1, &L2);
	D3DXVec3Cross( &n1, &L1, &vCross );
	D3DXVec3Cross( &n2, &L2, &vCross );

	D3DXVec3Normalize( &n1, &n1 );
	D3DXVec3Normalize( &n2, &n2 );

	if ( CheckVetexStatus( n1, v1, v3, v4 ) && CheckVetexStatus( n2, v3, v1, v2 ) )
		return TRUE;
	else
		return FALSE;
}

/**
  * @brief 判断球体与刚体Spcy的相交情况
  */
BOOL KPhysicsSimulator::SphereSpcyIntersect(KRigidBody::LPSPCY pSp, const D3DXVECTOR3& vecCenter, float radius)
{
	LPAABB A = new _AABB;
	CreateAABB( A, pSp );

	BOOL bSphereAABBInter = FALSE;
	bSphereAABBInter = SphereAABBIntersect( vecCenter, radius, A );

	SAFE_DELETE( A );
	
	if( ! bSphereAABBInter ) 
		return FALSE;
	else
	{
		D3DXVECTOR3 vec[2];
		
		vec[0] = pSp->CtrPosition + pSp->Height * pSp->Normal;
		vec[1] = pSp->CtrPosition - pSp->Height * pSp->Normal;
		

		//球面与球面的接触情况
		D3DXVECTOR3 vecDist;

		for ( int i = 0; i < 2; i++ )
		{
			vecDist = vec[i] - vecCenter;
			if ( D3DXVec3Length( &vecDist ) <= ( pSp->Radius + pSp->Radius ) )
			return TRUE;
		}

		float Dist = 0.0f;

		Dist = VertexLineDist( vecCenter, vec[0], vec[1] );

		if ( ( Dist <= ( pSp->Radius + pSp->Radius ) ) &&
			IsAcuteAngle( vecCenter, vec[0], vec[1] ) &&
			IsAcuteAngle( vecCenter, vec[1], vec[0] ) )

			return TRUE;

		return FALSE;
	}

}

///**
//  * @brief 判断球体与刚体Spcy的相交情况
//  * @param D3DXVECTOR3& vect返回相交切面的法向量
//  */
//BOOL KPhysicsSimulator::SphereSpcyIntersectWithVect(KRigidBody::LPSPCY pSp, const D3DXVECTOR3& vecCenter, float radius, D3DXVECTOR3& vect )
//{
//	LPAABB A = new _AABB;
//	CreateAABB( A, pSp );
//	
//	BOOL bSphereAABBInter = FALSE;
//	bSphereAABBInter = SphereAABBIntersect( vecCenter, radius, A );
//
//	SAFE_DELETE( A );
//
//	if( !bSphereAABBInter ) 
//		return FALSE;
//	else
//	{
//		D3DXVECTOR3 vec[2];
//		
//		vec[0] = pSp->CtrPosition + pSp->Height * pSp->Normal;
//		vec[1] = pSp->CtrPosition - pSp->Height * pSp->Normal;
//		
//
//		//球面与球面的接触情况
//		D3DXVECTOR3 vecDist;
//
//		for ( int i = 0; i < 2; i++ )
//		{
//			vecDist = vec[i] - vecCenter;
//			if ( D3DXVec3Length( &vecDist ) <= ( pSp->Radius + pSp->Radius ) )
//			return TRUE;
//		}
//
//		float Dist = 0.0f;
//
//		//球面与LSS接触情况
//		Dist = VertexLineDist( vecCenter, vec[0], vec[1] );
//
//		if ( ( Dist <= ( pSp->Radius + radius ) ) &&
//			IsAcuteAngle( vecCenter, vec[0], vec[1] ) &&
//			IsAcuteAngle( vecCenter, vec[1], vec[0] ) )
//		{
//
//			D3DXVECTOR3 vTmp1, vTmp2;
//			vTmp1 = vec[1] - vec[0];
//			vTmp2 = vecCenter - vec[0];
//			D3DXVec3Cross( &vect, &vTmp1, &vTmp2 );
//			D3DXVec3Cross( &vect, &vTmp1, &vect );
//			D3DXVec3Normalize( &vect, &vect );
//
//			return TRUE;
//		}
//
//		return FALSE;
//	}
//
//}




//
//BOOL KPhysicsSimulator::CheckCollisionWithVect( LPRIGIDBODY pRigid1, LPRIGIDBODY pRigid2, D3DXVECTOR3& vect )
//{
//
//	float r1 = pRigid1->pSp->Radius + pRigid1->pSp->Height ;
//	float r2 = pRigid2->pSp->Radius + pRigid2->pSp->Height ;
//
//	if( !CheckSphereIntersect( pRigid1->pSp->CtrPosition, r1, pRigid2->pSp->CtrPosition, r2 ) ) //判断两刚体的球包围盒是否接触
//	{
//		return FALSE;
//	}
//	else
//	{
//		if( ( pRigid1->pSp->Height == 0 ) && ( pRigid2->pSp->Height == 0 ) )  // 两个刚体均为球体，中心点距离等于半径之和
//		{
//			vect = pRigid1->pSp->CtrPosition - pRigid2->pSp->CtrPosition;
//			D3DXVec3Normalize( &vect, &vect );
//			return TRUE;
//		}
//		else 
//			if( pRigid1->pSp->Height == 0 )  //球体与LSS接触情况
//			{
//
//				return SphereSpcyIntersectWithVect( pRigid2->pSp, pRigid1->pSp->CtrPosition, pRigid1->pSp->Radius, vect );
//
//			}
//			else 
//				if ( pRigid2->pSp->Height == 0 ) //LSS与球体接触情况
//				{
//					return SphereSpcyIntersectWithVect( pRigid1->pSp, pRigid2->pSp->CtrPosition, pRigid2->pSp->Radius, vect );
//				}
//				else // LSS与LSS接触情况
//				{
//
//					LPAABB A = new _AABB;
//					LPAABB B = new _AABB;
//
//					CreateAABB( A, pRigid1->pSp );
//					CreateAABB( B, pRigid2->pSp );
//
//					BOOL bAABBInter = FALSE;
//					bAABBInter = AABBIntersect( A, B );
//
//					SAFE_DELETE( A );
//					SAFE_DELETE( B );
//
//					if( bAABBInter ) //AABB包围盒相交测试
//					{
//						D3DXVECTOR3 vec[2][2];
//						vec[0][0] = pRigid1->pSp->CtrPosition + pRigid1->pSp->Height * pRigid1->pSp->Normal;
//						vec[0][1] = pRigid1->pSp->CtrPosition - pRigid1->pSp->Height * pRigid1->pSp->Normal;
//						vec[1][0] = pRigid2->pSp->CtrPosition + pRigid2->pSp->Height * pRigid2->pSp->Normal;
//						vec[1][1] = pRigid2->pSp->CtrPosition - pRigid2->pSp->Height * pRigid2->pSp->Normal;
//
//						BOOL mParal;
//						float dist = LineDist( vec[0][0], vec[0][1], vec[1][0], vec[1][1], mParal );
//
//						if( dist > ( pRigid1->pSp->Radius + pRigid2->pSp->Radius ) )
//							return FALSE;
//						else
//						{
//							//球面与球面的接触情况
//							D3DXVECTOR3 vecDist;
//
//							for ( int i = 0; i < 2; i++ )
//								for ( int j = 0; j < 2; j++ )
//								{
//									vecDist = vec[0][i] - vec[1][j];
//									if ( D3DXVec3Length( &vecDist ) <= ( pRigid1->pSp->Radius + pRigid2->pSp->Radius ) )
//									{
//										vect = vecDist;
//										D3DXVec3Normalize( &vect, &vect );
//										return TRUE;
//									}
//								}
//
//
//							if( ( dist < 0.0f ) && ( dist > - 0.0f ) ) // 两轴共面相交, 球与LSS接触情况
//							{
//				
//								//
//								if( LineSegIntersect( vec[0][0], vec[0][1], vec[1][0], vec[1][1] ) )
//								{
//									return TRUE;
//								}
//								else
//								{
//								D3DXVECTOR3 vecDist;
//
//								float Dist = 0.0f;
//
//								for ( int i = 0; i < 2; i++ )
//									for ( int j = 0; j < 2; j++ )
//									{
//										Dist = VertexLineDist( vec[i][j], vec[1-i][0], vec[1-i][1] );
//										if ( ( Dist <= ( pRigid1->pSp->Radius + pRigid2->pSp->Radius ) ) &&
//											 IsAcuteAngle( vec[i][j], vec[1-i][0], vec[1-i][1] ) &&
//											 IsAcuteAngle( vec[i][j], vec[1-i][1], vec[1-i][0] ) )
//										{
//											D3DXVECTOR3 vTmp1, vTmp2;
//											vTmp1 = vec[1-i][1] - vec[1-i][0];
//											vTmp2 = vec[i][j] - vec[1-i][0];
//											D3DXVec3Cross( &vect, &vTmp1, &vTmp2 );
//											D3DXVec3Cross( &vect, &vTmp1, &vect );
//											D3DXVec3Normalize( &vect, &vect );
//
//											return TRUE;
//										}
//										int ll=0;
//									}
//
//								return FALSE;
//								}
//
//							}
//							else 
//								if( mParal ) //两轴共面平行
//								{
///*									BOOL flag = TRUE;
//
//
//									flag = IsAcuteAngle( vec[0][0], vec[1][0], vec[1][1] );
//
//									flag = IsAcuteAngle( vec[0][0], vec[1][1], vec[1][0] );
//
//									flag = IsAcuteAngle( vec[1][0], vec[1][0], vec[1][1] );
//
//									flag = IsAcuteAngle( vec[1][0], vec[1][1], vec[1][0] );
//
//*/
//									if( ( IsAcuteAngle( vec[0][0], vec[1][0], vec[1][1] ) &&  IsAcuteAngle( vec[0][0], vec[1][1], vec[1][0] ) ) ||
//										( IsAcuteAngle( vec[0][1], vec[1][0], vec[1][1] ) &&  IsAcuteAngle( vec[0][1], vec[1][1], vec[1][0] ) ) )
//									{
//										D3DXVECTOR3 vTmp1, vTmp2;
////										vTmp1 = vec[1][1] - vec[1][0];
////										vTmp2 = vec[0][0] - vec[1][0];
//										vTmp1 = pRigid1->pSp->Normal;
//										vTmp2 = vec[1][0] - vec[0][0];
////										vTmp2 = pRigid2->pSp->Normal;
//
//										D3DXVec3Cross( &vect, &vTmp1, &vTmp2 );
//										D3DXVec3Cross( &vect, &vect, &vTmp1 );
//										D3DXVec3Normalize( &vect, &vect );
//
//										return TRUE;
//									}
//									else 
//									{
//										return FALSE;
//									}
//
//								}
//								else //两轴异面
//								{
//									if( CheckSeparateVertex( vec[0][0], vec[0][1], vec[1][0], vec[1][1] ) )
//									{
//										D3DXVec3Cross( &vect, &pRigid1->pSp->Normal, &pRigid2->pSp->Normal );
//										D3DXVec3Normalize( &vect, &vect );
//
//										return TRUE;
//									}
//									else //转化为球与LSS
//									{
//
//										//
//										D3DXVECTOR3 vecDist;
//										float Dist = 0.0f;
//		
//										for ( int i = 0; i < 2; i++ )
//											for ( int j = 0; j < 2; j++ )
//											{
//												Dist = VertexLineDist( vec[i][j], vec[1-i][0], vec[1-i][1] );
//												if ( ( Dist <= ( pRigid1->pSp->Radius + pRigid2->pSp->Radius ) ) &&
//													  IsAcuteAngle( vec[i][j], vec[1-i][0], vec[1-i][1] ) &&
//													  IsAcuteAngle( vec[i][j], vec[1-i][1], vec[1-i][0] ) )
//												{
//													D3DXVECTOR3 vTmp1, vTmp2;
//													vTmp1 = vec[1-i][1] - vec[1-i][0];
//													vTmp2 = vec[i][j] - vec[1-i][0];
//													D3DXVec3Cross( &vect, &vTmp1, &vTmp2 );
//													D3DXVec3Cross( &vect, &vTmp1, &vect );
//													D3DXVec3Normalize( &vect, &vect );
//
//													return TRUE;
//												}
//											}
//
//
//										return FALSE;
//
//									}
//
//								}
//
//						}
//
//					}
//					else
//					{
//						return FALSE;
//					}
//
//				}
//
//	}
//
//	return FALSE;
//
//
//}




//
//void KPhysicsSimulator::CollisionSimulation( float TinyTime)
//{
//	list<LPRIGIDBODY>::iterator i = listRigidBody.begin();
//	
//	
//
//	while(i!=listRigidBody.end())
//	{
//		LPRIGIDBODY pRigid = *i;
//		//////////////////////////////////////////////////////////////////////////
//	
//
//			
//		//pRigid->Speed.y += (-98.0f*TinyTime);
//
////		pRigid->m_LineAcc = D3DXVECTOR3 ( 0, 0, 0 );
////		pRigid->m_LineVel += ( pRigid->m_LineAcc*TinyTime );
//		D3DXVECTOR3 vecPrePosition = pRigid->pSp->CtrPosition;
////		pRigid->pSp->CtrPosition += (pRigid->m_LineVel*TinyTime + 0.5*pRigid->m_LineAcc*TinyTime*TinyTime);
//
////		if(pRigid->pSp->CtrPosition.y<=0)
////		{
////			pRigid->m_LineVel.y *= -1.0f;
////			pRigid->pSp->CtrPosition.y = 0 ;
////		}
//
//		/*
//		if ( ( pRigid->pSp->CtrPosition.y - fabs( pRigid->pSp->Height * pRigid->pSp->Normal.y ) - pRigid->pSp->Radius ) <= 0 )
//		{
//			pRigid->m_LineVel.y *= -1.0f;
//			pRigid->pSp->CtrPosition.y = fabs( pRigid->pSp->Height * pRigid->pSp->Normal.y) +  pRigid->pSp->Radius ;
//		}
//		*/
//
//		//pRigid->pSp->YawPitchRoll.y = timeGetTime()*0.01f;
//		//pRigid->pSp->YawPitchRoll.z = timeGetTime()*0.01f;
//		// code by HJQ
//		
//		list<LPRIGIDBODY>::iterator j = i; // code by HJQ
//
//		j++;
//
//		while(j!=listRigidBody.end())
//		{
//			LPRIGIDBODY pRigid2 = *j;
//
////			float r1 = pRigid->pSp->Radius + pRigid->pSp->Height ;
////			float r2 = pRigid2->pSp->Radius + pRigid2->pSp->Height ;
//
////			if( j!=i && CheckSphereCollision( &pRigid->pSp->CtrPosition, r1, &pRigid2->pSp->CtrPosition, r2 )  )
//			D3DXVECTOR3 vect = D3DXVECTOR3( 0, 0, 0 );
//
//			if( j!=i && CheckCollisionWithVect( pRigid, pRigid2, vect )  )
//			{
////				pRigid->Speed = D3DXVECTOR3( 0, 0, 0);
////				pRigid2->Speed = D3DXVECTOR3( 0, 0, 0);
//
////				pRigid->m_LineVel.y*= -1.0f;
////				pRigid2->m_LineVel.y*= -1.0f;
////				D3DXVECTOR3 vect;
//				D3DXVECTOR3 VelAcc[2][2];
//				float  fDot1= 0.0f;
//				float  fDot2= 0.0f;
//
//				
//
//				//vect方向由 pRigid 指向 pRigid2
////				vect = pRigid->pSp->CtrPosition - pRigid2->pSp->CtrPosition;
////				D3DXVec3Normalize( &vect, &vect );
////				if( D3DXVec3Dot( &pRigid->m_LineVel, &pRigid2->m_LineVel ) > 0)
////				{
//
////				VelAcc[0][0] = D3DXVec3Dot( &pRigid->m_LineVel, &vect) * vect;
////				VelAcc[0][1] = pRigid->m_LineVel - VelAcc[0][0];
////				VelAcc[1][0] = D3DXVec3Dot( &pRigid2->m_LineVel, &vect) * vect;
////				VelAcc[1][1] = pRigid2->m_LineVel - VelAcc[1][0];
//
//				fDot1 = D3DXVec3Dot( &pRigid->m_LineVel, &vect);
//				fDot2 = D3DXVec3Dot( &pRigid2->m_LineVel, &vect);
//
////				if ( fDot1 > 0 || fDot2 < 0 )
////				{
//				VelAcc[0][0] = fDot1 * vect;
//				VelAcc[0][1] = pRigid->m_LineVel - VelAcc[0][0];
//				VelAcc[1][0] = fDot2 * vect;
//				VelAcc[1][1] = pRigid2->m_LineVel - VelAcc[1][0];
//
////				D3DXVECTOR3 vecTmp = D3DXVECTOR3( 0, 0, 0 );
////				vecTmp = VelAcc[0][0];
////				VelAcc[0][0] = VelAcc[1][0];
////				VelAcc[1][0] = vecTmp;
//
//
////begin 测试使用碰撞恢复系数
//				float e = 0.8f;
//				
//
//				CalCollisionVelocity( VelAcc[0][0], pRigid->m_fMass, VelAcc[1][0], pRigid2->m_fMass, e );
////end
////				VelAcc[0][0] *= -1.0f;
////				VelAcc[1][0] *= -1.0f;
////				VelAcc[0][0] = VelAcc[1][0] = 0.5 * ( VelAcc[0][0] + VelAcc[1][0] );
//
//				pRigid->m_LineVel = VelAcc[0][0] + VelAcc[0][1];
//				pRigid2->m_LineVel = VelAcc[1][0] + VelAcc[1][1];
////				}
////				}
//				
//				pRigid->pSp->CtrPosition = vecPrePosition; //这条语句有问题， 保留或去除都会有问题
////				}
//			}
//				
//			j++;
//		}
//	
//		// end code
//        i++;
//	}
//
//}

//void KPhysicsSimulator::SimulationTest( float TinyTime)
//{
//	/* 测试IsSphereIntersectTriangle( &inter, &ct, r, &v[0], &v[2], &v[1] );
//	D3DXVECTOR3 inter;
//	D3DXVECTOR3 ct = D3DXVECTOR3( 3,2,2 );
//	float r=2;
//	D3DXVECTOR3 v[3];
//
//	v[0] = D3DXVECTOR3( 3,0,0 );
//	v[1] = D3DXVECTOR3( -3,0,0 );
//	v[2] = D3DXVECTOR3( 0,0,5.2 );
//
//   BOOL test = FALSE;
//   test = 	IsSphereIntersectTriangle( &inter, &ct, r, &v[0], &v[2], &v[1] );
//
//   int ii = 0;
//*/
//
//	/*
//	list<LPRIGIDBODY>::iterator i = listRigidBody.begin();
//
//	D3DXVECTOR3 RCenter;
//	RCenter = D3DXVECTOR3( 0, 0, 0 );
//
//	while(i!=listRigidBody.end())
//	{
//		LPRIGIDBODY pRigid = *i;
//
//		D3DXVECTOR3 RNormal;
//
//		RNormal = RCenter -  pRigid->pSp->CtrPosition;
//		D3DXVec3Normalize( &RNormal, &RNormal );
//
//		pRigid->facc = RNormal*100;
//
//		pRigid->m_LineVel += ( pRigid->facc*TinyTime );
//		pRigid->pSp->CtrPosition += (pRigid->m_LineVel*TinyTime + 0.5*pRigid->facc*TinyTime*TinyTime);
//
//		i++;
//	}
//	*/
//	list<LPRIGIDBODY>::iterator i = listRigidBody.begin();
//
//	D3DXVECTOR3 RCenter;
//	RCenter = D3DXVECTOR3( 0, 0, 0 );
//
//	while(i!=listRigidBody.end())
//	{
//
//		LPRIGIDBODY pRigid = *i;
//
////		pRigid->SetSP( RCenter, D3DXVECTOR3( 0, 1, 0 ), 50.0f, 20.0f );
//		StepRigidBody( pRigid, TinyTime );
//
////		D3DXVECTOR3 RNormal;
//
////		RNormal = RCenter -  pRigid->pSp->CtrPosition;
////		D3DXVec3Normalize( &RNormal, &RNormal );
///*
//		D3DXVECTOR3 Axis( 0, 0, 1 );
//		D3DXVECTOR3 vTrans( 200, 0, 0 );
//		D3DXMATRIX mat1;
//		D3DXMATRIX Trans1, Trans2;
////		D3DXMATRIX matCenter;
//
////		matCenter._11 = pRigid->pSp->CtrPosition.x;
////		matCenter._12 = pRigid->pSp->CtrPosition.y;
////		matCenter._13 = pRigid->pSp->CtrPosition.z;
////		matCenter._14 = 1.0f;
////		matCenter._21 = pRigid->pSp->Normal.x;
////		matCenter._22 = pRigid->pSp->Normal.y;
////		matCenter._23 = pRigid->pSp->Normal.z;
////		matCenter._24 = 1.0f;
//
//		D3DXMatrixTranslation( &Trans1, -vTrans.x, -vTrans.y, -vTrans.z );
//		D3DXMatrixTranslation( &Trans2, vTrans.x, vTrans.y, vTrans.z );
//		D3DXMatrixRotationAxis( &mat1, &Axis, 0.1 ); 
//		D3DXMatrixMultiply(&mat1, &Trans1, &mat1 );
//		D3DXMatrixMultiply(&mat1, &mat1, &Trans2 );
//		*/
////		mat1._41 = 0.0f;  mat1._42 = 1.0f; mat1._43 = 0.0f;
//       	
////		D3DXMatrixMultiply( &matCenter, &matCenter, &mat1 );
//		
////		pRigid->pSp->CtrPosition.x = matCenter._11;
////		pRigid->pSp->CtrPosition.y = matCenter._12;
////		pRigid->pSp->CtrPosition.z = matCenter._13;
//		
////		pRigid->pSp->Normal.x = matCenter._21;
////		pRigid->pSp->Normal.y = matCenter._22;
////		pRigid->pSp->Normal.z = matCenter._23;
//
///*
//		D3DXVECTOR3 NewNormal( 0, 0, 0 );
//		NewNormal = pRigid->pSp->CtrPosition + pRigid->pSp->Normal;
//
//		D3DXVec3TransformCoord( &pRigid->pSp->CtrPosition, &pRigid->pSp->CtrPosition, &mat1 );
//		D3DXVec3TransformCoord( &NewNormal, &NewNormal, &mat1 );
//
//		NewNormal = NewNormal - pRigid->pSp->CtrPosition;
//		pRigid->pSp->Normal = NewNormal;
//		D3DXVec3Normalize( &pRigid->pSp->Normal, &pRigid->pSp->Normal);
//*/
////		pRigid->facc = RNormal*100;
//
////		pRigid->m_LineVel += ( pRigid->facc*TinyTime );
////		pRigid->pSp->CtrPosition += (pRigid->m_LineVel*TinyTime + 0.5*pRigid->facc*TinyTime*TinyTime);
//
//		i++;
//	}
//	
//}
//


///**
//  * @brief 根据碰撞恢复系数e，计算两物体碰撞后的速度，假定两物体原速度在一直线上。
//  */
//void KPhysicsSimulator::CalCollisionVelocity( D3DXVECTOR3& va , float ma , D3DXVECTOR3& vb , float mb, float e)
//{
//	D3DXVECTOR3 velo;	
//
//	velo = ( ma*va + mb*vb ) / ( ma + mb );
//
//	va = ( 1 + e ) * velo - e * va;
//	vb = ( 1 + e ) * velo - e * vb;
//
//	//另一等价计算公式
//	// va2 = ((e+1)*mb*vb1+va1(ma - e*mb))/(ma+mb)
//	// vb2 = ((e+1)*ma*va1+vb1(ma - e*mb))/(ma+mb)
//
//}


///**
//  * @brief 根据刚体的线速度和角速度，计算下一步的位置。
//  * @param TinyTime 时间步
//  */
//int KPhysicsSimulator::StepRigidBody(LPRIGIDBODY pRigid,  float TinyTime)
//{
//	/*
////	pRigid->RotAxis = D3DXVECTOR3( 1, 0, 0  );
////	pRigid->m_AngVel = D3DXVECTOR3( 1, 0, 0 );
////	pRigid->m_LineVel = D3DXVECTOR3( 0, 0, 0 );
//
//	pRigid->pSp->CtrPosition += pRigid->m_LineVel*TinyTime;
//
//
//	D3DXVECTOR3 Axis = pRigid->RotAxis;
//	D3DXVECTOR3 vTrans = pRigid->pSp->CtrPosition;
//	D3DXVECTOR3 frv; //角速度沿RotAxis方向的分量。
//	float theta = 0.0f;
//	float k = 0.0f;
//
//	D3DXMATRIX mat1;
//	D3DXMATRIX Trans1, Trans2;
//	
//	k = D3DXVec3Dot( &Axis, &pRigid->m_AngVel );
//	frv = k*Axis;
//	theta = k * TinyTime;
//
//	D3DXMatrixTranslation( &Trans1, -vTrans.x, -vTrans.y, -vTrans.z );
//	D3DXMatrixTranslation( &Trans2, vTrans.x, vTrans.y, vTrans.z );
//	D3DXMatrixRotationAxis( &mat1, &Axis, theta ); 
//	D3DXMatrixMultiply(&mat1, &Trans1, &mat1 );
//	D3DXMatrixMultiply(&mat1, &mat1, &Trans2 );
//
//	D3DXVECTOR3 NewNormal( 0, 0, 0 );
//	NewNormal = pRigid->pSp->CtrPosition + pRigid->pSp->Normal;
//
//	D3DXVec3TransformCoord( &pRigid->pSp->CtrPosition, &pRigid->pSp->CtrPosition, &mat1 );
//	D3DXVec3TransformCoord( &NewNormal, &NewNormal, &mat1 );
//
//	NewNormal = NewNormal - pRigid->pSp->CtrPosition;
//	D3DXVec3Normalize( &NewNormal, &NewNormal);
//	pRigid->pSp->Normal = NewNormal;
//	return 0;
//*/
//	pRigid->pSp->CtrPosition += pRigid->m_LineVel*TinyTime;
//
//	float k = 0.0f;
//	k = D3DXVec3Length( &pRigid->m_AngVel );
//
//	if( k != 0 )
//	{
//		D3DXVECTOR3 Axis ;
//		D3DXVec3Normalize( &Axis, &pRigid->m_AngVel ); 
//		
//		D3DXVECTOR3 vTrans = pRigid->pSp->CtrPosition;
////		D3DXVECTOR3 frv; //角速度沿RotAxis方向的分量。
//		float theta = 0.0f;
//
//		D3DXMATRIX mat1;
//		D3DXMATRIX Trans1, Trans2;
//
////		k = D3DXVec3Dot( &Axis, &pRigid->m_AngVel );
////		frv = k*Axis;
//		theta = k * TinyTime;
//
//		D3DXMatrixTranslation( &Trans1, -vTrans.x, -vTrans.y, -vTrans.z );
//		D3DXMatrixTranslation( &Trans2, vTrans.x, vTrans.y, vTrans.z );
//		D3DXMatrixRotationAxis( &mat1, &Axis, theta ); 
//		D3DXMatrixMultiply(&mat1, &Trans1, &mat1 );
//		D3DXMatrixMultiply(&mat1, &mat1, &Trans2 );
//
//		D3DXVECTOR3 NewNormal( 0, 0, 0 );
//		NewNormal = pRigid->pSp->CtrPosition + pRigid->pSp->Normal;
//
//		D3DXVec3TransformCoord( &pRigid->pSp->CtrPosition, &pRigid->pSp->CtrPosition, &mat1 );
//		D3DXVec3TransformCoord( &NewNormal, &NewNormal, &mat1 );
//
//		NewNormal = NewNormal - pRigid->pSp->CtrPosition;
//		D3DXVec3Normalize( &NewNormal, &NewNormal);
//		pRigid->pSp->Normal = NewNormal;
//	}
//	return 0;
//}


///**
//  * @brief 用于判断刚体Spcy和平面的相交情况
//  * @param vect 平面上一定点
//  * @param normal 平面的法向量
//  * @return 1 表示相交。
//  */
//
//BOOL KPhysicsSimulator::SpcyPlaneIntersect(LPRIGIDBODY pRigid, const D3DXVECTOR3& vect, const D3DXVECTOR3& normal)
//{
//	if( pRigid->pSp->Height == 0 ) //刚体是球体的情况
//	{
//		D3DXVECTOR3 vt;
//		float d = 0.0f;
//
//		vt = pRigid->pSp->CtrPosition - vect;
//		
//		d = fabs( D3DXVec3Dot( &vt, &normal ) );
//
//		if ( d <= pRigid->pSp->Radius ) 
//		{
//			return TRUE;
//		}
//	}
//	else
//	{
//		D3DXVECTOR3 vt[2];
//		float d1, d2;
//		
//		vt[0] = pRigid->pSp->CtrPosition + pRigid->pSp->Normal * pRigid->pSp->Height - vect;
//		vt[1] = pRigid->pSp->CtrPosition - pRigid->pSp->Normal * pRigid->pSp->Height - vect;
//
//		d1 = D3DXVec3Dot( &vt[0], &normal );
//		d2 = D3DXVec3Dot( &vt[1], &normal );
//
////慎用此类判断d1*d2 <= 0
//		if ( ( d1*d2 <= 0 ) || ( fabs(d1) <= pRigid->pSp->Radius ) || ( fabs(d2) <= pRigid->pSp->Radius ) )
//		{
//			return TRUE;
//		}
//	
//	}
//
//	return FALSE;
//}

BOOL KPhysicsSimulator::IsCollidable( LPRIGIDBODY pRigid, vector<LPRIGIDBODY> & nc )
{
	if ( pRigid == NULL )
		return FALSE;

	if ( nc.empty() )
		return TRUE;
	
	vector<LPRIGIDBODY>::iterator iPos;
	iPos = find( nc.begin(), nc.end(), pRigid );

	if ( iPos != nc.end() )
		return FALSE;

	return TRUE;

}


/**
  * @brief 用于屏蔽两刚体的碰撞检测
  */
int KPhysicsSimulator::DisableCollision( LPRIGIDBODY pRigid1, LPRIGIDBODY pRigid2 )
{
	if ( ( pRigid1==NULL )||( pRigid2 == NULL ) )
		return -1;
	
	pRigid1->NonCollidable().push_back(pRigid2);
	pRigid2->NonCollidable().push_back(pRigid1);

	return 0;
}


//检测刚体与平面相交的交点有问题
BOOL KPhysicsSimulator::SpcyPlaneIntersect( const LPRIGIDBODY pRigid, D3DXPLANE & Plane, D3DXVECTOR3& Inter, float& PenetrationDepth )
{
	float height = pRigid->pSp->Height;
	float radius = pRigid->pSp->Radius;
	D3DXVECTOR3 N;
//	D3DXVECTOR3 point;
	N.x = Plane.a;
	N.y = Plane.b;
	N.z = Plane.c;
	D3DXVec3Normalize( &N, &N );
	
	D3DXVECTOR3 CtPos = pRigid->pSp->CtrPosition ;
	D3DXVECTOR3 normal = pRigid->pSp->Normal ;

//	point = D3DXVECTOR3( 0,0, -Plane.d * SafeInvNum(Plane.c) );

	if ( height < 0.0f )
		return FALSE;

	if ( radius < 0.0f )
		return FALSE;

	if ( height == 0 )
	{

		float Dist = D3DXPlaneDotCoord( &Plane, &CtPos );

		if( Dist <= radius )
		{
			Inter = CtPos - Dist * N;
			PenetrationDepth = radius - Dist;
			return TRUE;
		}

	}
	else
	{
		D3DXVECTOR3 pos[2];
		pos[0] = CtPos + height * normal;
		pos[1] = CtPos - height * normal;

		float Dist[2];
		Dist[0] = D3DXPlaneDotCoord( &Plane, &pos[0] );
		Dist[1] = D3DXPlaneDotCoord( &Plane, &pos[1] );

		if ( ( Dist[0] < Dist[1] )&&( Dist[0] <=radius ) )
		{
//			Inter = pos[0] - Dist[0]*N;
			Inter = pos[0] - radius*N;
			PenetrationDepth = radius - Dist[0];

//			D3DXVECTOR3 realpos = Inter - CtPos;
			return TRUE;
		}
		else
			if ( ( Dist[1] < Dist[0] )&&( Dist[1] <=radius ) )
			{
//				Inter = pos[1] - Dist[1]*N;
				Inter = pos[1] - radius*N;
				PenetrationDepth = radius - Dist[1];
//				D3DXVECTOR3 realpos = Inter - CtPos;

				return TRUE;
			}
			else 
				if ( ( Dist[1] == Dist[0] )&&( Dist[1] <=radius ) )
				{
//					Inter = CtPos - Dist[1]*N;
					Inter = pos[1] - radius*N;
					PenetrationDepth = radius - Dist[0];
					return TRUE;
				}

	}

	return FALSE;
}


/**
  * @brief 两刚体的相交测试
  * @param pRigid1 刚体1
  * @param pRigid1 刚体2
  * @param Inter 相交点(注意,求解出的不是真实的相交点),作为一个返回值
  * @param DirTo1 碰撞方向, 指向刚体1,作为一个返回值
  * @param PenetrationDepth 嵌入深度(注意,求解出的不是真实的嵌入深度),作为一个返回值
  */
//SpcyIntersect()相交测试可以正常检测到相交情况
//碰撞点Inter的求解是存在问题的,而碰撞点的求解影响到碰撞响应,
//嵌入深度PenetrationDepth的求解也是存在问题的
BOOL KPhysicsSimulator::SpcyIntersect( const LPRIGIDBODY pRigid1, const LPRIGIDBODY pRigid2, D3DXVECTOR3& Inter, D3DXVECTOR3& DirTo1, float& PenetrationDepth )
{
	float fMaxDist = pRigid1->pSp->Radius + pRigid2->pSp->Radius;
	float R1 = pRigid1->pSp->Radius;
	float R2 = pRigid2->pSp->Radius;

	float fLen = 0.0f;
	D3DXVECTOR3 Diff = pRigid1->pSp->CtrPosition - pRigid2->pSp->CtrPosition;
	fLen = D3DXVec3Length( &Diff );

	if ( fLen > ( fMaxDist+ pRigid1->pSp->Height +pRigid2->pSp->Height ) )
	{
		return FALSE;
	}
	else 
		if ( ( pRigid1->pSp->Height == 0 ) && ( pRigid2->pSp->Height == 0 ) )
		{
			float fOverlap = fMaxDist - fLen;
			D3DXVec3Normalize( &Diff, &Diff );
			Inter = pRigid1->pSp->CtrPosition - Diff * ( R1 - fOverlap * 0.5f );

//			PenetrationDepth = 0.0f;
			PenetrationDepth = fOverlap;

			return TRUE;
		}
		else 
			if ( pRigid1->pSp->Height == 0 ) 
			{
				BOOL bSpcySphereIntersect = SpcySphereIntersect( pRigid2, pRigid1, Inter, DirTo1, PenetrationDepth );
				return bSpcySphereIntersect;

			}
			else 
				if ( pRigid2->pSp->Height == 0 )
				{
					BOOL bSpcySphereIntersect = SpcySphereIntersect( pRigid1, pRigid2, Inter, DirTo1, PenetrationDepth );
					return bSpcySphereIntersect;
				}
				else
				{
					//AABB包围盒检测
					LPAABB A = new _AABB;
					LPAABB B = new _AABB;

					CreateAABB( A, pRigid1->pSp );
					CreateAABB( B, pRigid2->pSp );

					BOOL bAABBInter = FALSE;
					bAABBInter = AABBIntersect( A, B );

					SAFE_DELETE( A );
					SAFE_DELETE( B );

					if( !bAABBInter ) //AABB包围盒相交测试
						return FALSE;
					else
					{
						D3DXVECTOR3 vec[2][2];
						vec[0][0] = pRigid1->pSp->CtrPosition + pRigid1->pSp->Height * pRigid1->pSp->Normal;
						vec[0][1] = pRigid1->pSp->CtrPosition - pRigid1->pSp->Height * pRigid1->pSp->Normal;
						vec[1][0] = pRigid2->pSp->CtrPosition + pRigid2->pSp->Height * pRigid2->pSp->Normal;
						vec[1][1] = pRigid2->pSp->CtrPosition - pRigid2->pSp->Height * pRigid2->pSp->Normal;

						BOOL mParal;
						float dist = LineDist( vec[0][0], vec[0][1], vec[1][0], vec[1][1], mParal );

						if( dist > fMaxDist )
							return FALSE;
						else
						{
							//球面与球面的接触情况

							for ( int i = 0; i < 2; i++ )
								for ( int j = 0; j < 2; j++ )
								{
									D3DXVECTOR3 Diff = vec[0][i] - vec[1][j];
									D3DXVECTOR3 Diff2 = vec[0][i] - vec[1][j];

									float fLen = D3DXVec3LengthSq( &Diff ) ;
									float fLen2 = D3DXVec3LengthSq( &Diff2 );
										
									if ( ( fLen < fMaxDist*fMaxDist ) && ( fLen2 < fMaxDist*fMaxDist ) )
									{
										fLen = sqrtf( fLen );
										float fOverlap = fMaxDist - fLen;
										D3DXVec3Normalize( &Diff, &Diff );
										Inter = pRigid1->pSp->CtrPosition - Diff * ( R1 - fOverlap * 0.5f );

										PenetrationDepth = fOverlap;

										return TRUE;

									}
									else if ( fLen < fMaxDist*fMaxDist )
									{
										fLen = sqrtf( fLen );
										float fOverlap = fMaxDist - fLen;
										D3DXVec3Normalize( &Diff, &Diff );
										Inter = vec[0][i] - Diff * ( R1 - fOverlap * 0.5f );

										PenetrationDepth = fOverlap;

										return TRUE;
									}
								}

								// 两轴共面相交
								if( ( dist < 0.0f ) && ( dist > - 0.0f ) ) 
								{
									if( LineSegIntersect( vec[0][0], vec[0][1], vec[1][0], vec[1][1] ) )
									{
										D3DXVECTOR3 vTmp1, vTmp2;
										D3DXVECTOR3 vect;
										D3DXVECTOR3 Diff;
										vTmp1 = pRigid1->pSp->Normal;
										vTmp2 = pRigid2->pSp->Normal;
										D3DXVec3Cross( &vect, &vTmp2 ,&vTmp1);

										D3DXVec3Cross( &vect, &vTmp2, &vect );
										D3DXPLANE  plane;
										D3DXPlaneFromPointNormal( &plane, &vec[1][0], &vect );
										D3DXPlaneIntersectLine( &Inter, &plane, &vec[0][0], &vec[0][1] );

										PenetrationDepth = fMaxDist;

										return TRUE;
									}
									else //球与LSS相交
									{
										float Dist = 0.0f;

										for ( int i = 0; i < 2; i++ )
											for ( int j = 0; j < 2; j++ )
											{
												Dist = VertexLineDist( vec[i][j], vec[1-i][0], vec[1-i][1] );
												if ( ( Dist <= fMaxDist ) &&
													IsAcuteAngle( vec[i][j], vec[1-i][0], vec[1-i][1] ) &&
													IsAcuteAngle( vec[i][j], vec[1-i][1], vec[1-i][0] ) )
												{
													D3DXVECTOR3 vTmp1, vTmp2;
													D3DXVECTOR3 Diff;
													vTmp1 = vec[1-i][1] - vec[1-i][0];
													vTmp2 = vec[i][j] - vec[1-i][0];
													D3DXVec3Normalize( &vTmp1, &vTmp1 );
													Diff = vec[i][j] - D3DXVec3Dot( &vTmp2, &vTmp1 )* vTmp1;								

													float fLen = Dist;
													float fOverlap = fMaxDist - fLen;
													D3DXVec3Normalize( &Diff, &Diff );

													if ( i == 0 )
														Inter = vec[i][j] - Diff * ( R1 - fOverlap * 0.5f );

													if ( i == 1 )
														Inter = vec[i][j] - Diff * ( R2 - fOverlap * 0.5f );

													PenetrationDepth = fOverlap;

													return TRUE;
												}
											}

											return FALSE;
									}

								}
								else 
									if( mParal ) //两轴共面平行
									{
										for( int i=0; i<2; i++ )
										{
											if ( IsAcuteAngle( vec[0][i], vec[1][0], vec[1][1] ) &&  IsAcuteAngle( vec[0][i], vec[1][1], vec[1][0] ) ) 
											{
												D3DXVECTOR3 vTmp1, vTmp2;
												D3DXVECTOR3 Diff;
												vTmp1 = vec[1][1] - vec[1][0];
												vTmp2 = vec[0][i] - vec[1][0];
												D3DXVec3Normalize( &vTmp1, &vTmp1 );
												Diff = vec[0][i] - D3DXVec3Dot( &vTmp2, &vTmp1 )* vTmp1;

												float fLen = dist;
												float fOverlap = fMaxDist - fLen;
												D3DXVec3Normalize( &Diff, &Diff );
												Inter = vec[0][i] - Diff * ( R1 - fOverlap * 0.5f );

												PenetrationDepth = fOverlap;
												return TRUE;

											}
										}

										return FALSE;

									}
									else //两轴异面
									{
										if( CheckSeparateVertex( vec[0][0], vec[0][1], vec[1][0], vec[1][1] ) )
										{
											D3DXVECTOR3 vTmp1, vTmp2;
											D3DXVECTOR3 vect;
											D3DXVECTOR3 Diff;
											vTmp1 = pRigid1->pSp->Normal;
											vTmp2 = pRigid2->pSp->Normal;
											D3DXVec3Cross( &vect, &vTmp2 ,&vTmp1);

											D3DXVec3Normalize( &vect, &vect );

											D3DXVec3Cross( &vect, &vTmp2, &vect );
											D3DXPLANE  plane;
											D3DXPlaneFromPointNormal( &plane, &vec[1][0], &vect );
											D3DXPlaneIntersectLine( &Inter, &plane, &vec[0][0], &vec[0][1] );

											PenetrationDepth = fMaxDist;

											return TRUE;
										}
										else //转化为球与LSS
										{

											D3DXVECTOR3 vecDist;
											float Dist = 0.0f;

											for ( int i = 0; i < 2; i++ )
												for ( int j = 0; j < 2; j++ )
												{
													Dist = VertexLineDist( vec[i][j], vec[1-i][0], vec[1-i][1] );
													if ( ( Dist <= fMaxDist ) &&
														IsAcuteAngle( vec[i][j], vec[1-i][0], vec[1-i][1] ) &&
														IsAcuteAngle( vec[i][j], vec[1-i][1], vec[1-i][0] ) )
													{
														D3DXVECTOR3 vTmp1, vTmp2;
														D3DXVECTOR3 Diff;
														vTmp1 = vec[1-i][1] - vec[1-i][0];
														vTmp2 = vec[i][j] - vec[1-i][0];
														D3DXVec3Normalize( &vTmp1, &vTmp1 );
														Diff = vec[i][j] - D3DXVec3Dot( &vTmp2, &vTmp1 )* vTmp1;								

														float fLen = Dist;
														float fOverlap = fMaxDist - fLen;
														D3DXVec3Normalize( &Diff, &Diff );

														if ( i == 0 )
															Inter = vec[i][j] - Diff * ( R1 - fOverlap * 0.5f );

														if ( i == 1 )
															Inter = vec[i][j] - Diff * ( R2 - fOverlap * 0.5f );

														PenetrationDepth = fOverlap;

														return TRUE;
													}
												}


												return FALSE;

										}

									}

						}

					}
				}
				return FALSE;
}



/**
  * @brief 刚体与球体的相交测试(第二个刚体为球体)
  * @param pRigid1 刚体
  * @param pRigid1 球体
  * @param Inter 相交点(注意,求解出的不是真实的相交点),作为一个返回值
  * @param DirTo1 碰撞方向, 指向刚体1,作为一个返回值
  * @param PenetrationDepth 嵌入深度(注意,求解出的不是真实的嵌入深度),作为一个返回值
  */
BOOL KPhysicsSimulator::SpcySphereIntersect( const LPRIGIDBODY pRigid1, const LPRIGIDBODY pRigid2, D3DXVECTOR3& Inter, D3DXVECTOR3& DirTo1, float& PenetrationDepth )
{
	float fMaxDist = pRigid1->pSp->Radius + pRigid2->pSp->Radius;
	float R1 = pRigid1->pSp->Radius;
	float R2 = pRigid2->pSp->Radius;
	D3DXVECTOR3 vecCenter = pRigid2->pSp->CtrPosition;

	LPAABB A = new _AABB;
	CreateAABB( A, pRigid1->pSp );
	BOOL bSphereAABBInter = FALSE;
	bSphereAABBInter = SphereAABBIntersect( pRigid2->pSp->CtrPosition, R2, A );
	SAFE_DELETE( A );

	if( !bSphereAABBInter ) 
		return FALSE;
	else
	{
		D3DXVECTOR3 vec[2];
		
		vec[0] = pRigid1->pSp->CtrPosition + pRigid1->pSp->Height * pRigid1->pSp->Normal;
		vec[1] = pRigid1->pSp->CtrPosition - pRigid1->pSp->Height * pRigid1->pSp->Normal;
		
		//球面与球面的接触情况
		for ( int i = 0; i < 2; i++ )
		{
			D3DXVECTOR3 Diff = vec[i] - vecCenter;

			float fLen = D3DXVec3LengthSq( &Diff ) ;

			if ( fLen <= fMaxDist*fMaxDist )
			{
				fLen = sqrtf( fLen );
				float fOverlap = fMaxDist - fLen;
				D3DXVec3Normalize( &Diff, &Diff );
				Inter = vec[i] - Diff * ( R1 - fOverlap * 0.5f );

				PenetrationDepth = fOverlap;
				return TRUE;
			}
		} // end for



		//球面与LSS接触情况
		float Dist = 0.0f;
		Dist = VertexLineDist( vecCenter, vec[0], vec[1] );

		if ( ( Dist <= fMaxDist ) &&
			IsAcuteAngle( vecCenter, vec[0], vec[1] ) &&
			IsAcuteAngle( vecCenter, vec[1], vec[0] ) )
		{
			D3DXVECTOR3 vTmp1, vTmp2;
			D3DXVECTOR3 Diff;
			vTmp1 = pRigid1->pSp->Normal;
			vTmp2 = vecCenter - vec[1];
//			vTmp1 = vec[1] - vec[0];
//			vTmp2 = vecCenter - vec[0];
			D3DXVec3Normalize(  &vTmp1, &vTmp1 );

			Diff = vecCenter - D3DXVec3Dot( &vTmp2, &vTmp1 )* vTmp1;								

			float fLen = Dist;
			float fOverlap = fMaxDist - fLen;
			D3DXVec3Normalize( &Diff, &Diff );

			Inter = vecCenter - Diff * ( R2 - fOverlap * 0.5f );

			PenetrationDepth = fOverlap;

			return TRUE;
		}
	}
	return FALSE;
}

/*
BOOL KPhysicsSimulator::SphereIntersect( const LPRIGIDBODY pRigid1, const LPRIGIDBODY pRigid2, D3DXVECTOR3& Inter, D3DXVECTOR3& DirTo1, float& PenetrationDepth )
{
	return FALSE;
}
*/

//BOOL KPhysicsSimulator:: SpcyPlaneIntersect1(LPRIGIDBODY pRigid, const D3DXVECTOR3& point, const D3DXVECTOR3& normal, D3DXVECTOR3& inter )
//{
//	if( pRigid->pSp->Height == 0 ) //刚体是球体的情况
//	{
//		D3DXVECTOR3 vt;
//		float d = 0.0f;
//
//		vt = pRigid->pSp->CtrPosition - point;
//		
//		d = D3DXVec3Dot( &vt, &normal );
//
//		if ( ( d <= pRigid->pSp->Radius ) && ( d >= - pRigid->pSp->Radius ) )
//		{
//			inter = pRigid->pSp->CtrPosition - d * normal;
//			return TRUE;
//		}
//	}
//	else
//	{
//		D3DXVECTOR3 vt[2][2];
//		float d1, d2;
//		
//		vt[0][0] = pRigid->pSp->CtrPosition + pRigid->pSp->Normal * pRigid->pSp->Height;
//		vt[0][1] = pRigid->pSp->CtrPosition - pRigid->pSp->Normal * pRigid->pSp->Height;
//		
//		vt[1][0] = vt[0][0] - point;
//		vt[1][1] = vt[0][1] - point;
//
//		d1 = D3DXVec3Dot( &vt[1][0], &normal );
//		d2 = D3DXVec3Dot( &vt[1][1], &normal );
//
//		if ( d1*d2 <= 0 )
//		{
//			D3DXPLANE plane1;
//			D3DXPlaneFromPointNormal( &plane1, &point, &normal );
//			D3DXPlaneIntersectLine( &inter, &plane1, &vt[0][0], &vt[1][1] );
//			return TRUE;
//		}
//		else 
//			if( fabs(d1) <= pRigid->pSp->Radius )
//			{
//				inter = vt[0][0] - d1 * normal;
//				return TRUE;
//			}
//			else
//				if( fabs(d2) <= pRigid->pSp->Radius )
//				{
//					inter = vt[0][1] - d2 * normal;
//					return TRUE;
//				}
//				else
//					return FALSE;
//	}
//	return FALSE;
//}


/**
  * @brief 线段的相交测试
  * @param q1 线段1上的端点1
  * @param q2 线段1上的端点2
  * @param p1 线段2上的端点1
  * @param p2 线段2上的端点2
  */
BOOL KPhysicsSimulator::LineSegIntersect(const D3DXVECTOR3& q1, const D3DXVECTOR3& q2, const D3DXVECTOR3& p1, const D3DXVECTOR3& p2)
{
	
	LPAABB A = new _AABB;
	LPAABB B = new _AABB;

	D3DXVec3Maximize( &A->max, &q1, &q2 );
	D3DXVec3Minimize( &A->min, &q1, &q2 );
	D3DXVec3Maximize( &B->max, &p1, &p2 );
	D3DXVec3Minimize( &B->min, &p1, &p2 );
	
	BOOL bAABBInter = FALSE;
	bAABBInter = AABBIntersect( A, B );

	SAFE_DELETE( A );
	SAFE_DELETE( B );

	if( bAABBInter ) //AABB包围盒相交测试
	{
		return FALSE;
	}
	else
	{
		D3DXVECTOR3 p1q1 = p1 - q1;
		D3DXVECTOR3 q2q1 = q2 - q1;
		D3DXVECTOR3 p2q1 = p2 - q1;

		D3DXVECTOR3 q1p1 = q1 - p1;
		D3DXVECTOR3 p2p1 = p2 - p1;
		D3DXVECTOR3 q2p1 = q2 - p1;

		D3DXVECTOR3 vCross[2];
		float d = 0.0f;

		D3DXVec3Cross( &vCross[0], &p1q1, &q2q1 );
		D3DXVec3Cross( &vCross[1], &q2q1, &p2q1 );
		d = D3DXVec3Dot( &vCross[0], &vCross[1] );
		if ( d >= 0 )
		{
			D3DXVec3Cross( &vCross[0], &q1p1, &p2p1 );
			D3DXVec3Cross( &vCross[1], &p2p1, &q2p1 );
			d = D3DXVec3Dot( &vCross[0], &vCross[1] );
			if ( d >=  0 )
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}


//BOOL KPhysicsSimulator::LineSegSpcyIntersect(LPRIGIDBODY pRigid, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2)
//{
//	float fR = pRigid->pSp->Radius;
//	
//	if( pRigid->pSp->Height == 0 ) //刚体是球体的情况
//	{
//
//		float d = 0.0f;
//		D3DXVECTOR3 v0 = pRigid->pSp->CtrPosition;
//
//		d = VertexLineDist( v0, v1, v2 );
//		
//		if ( d > fR ) 
//		{
//			return FALSE;
//		}
//		else
//			if ( IsAcuteAngle( v0, v1, v2 ) && IsAcuteAngle( v0, v2, v1 ) )
//			{
//				return TRUE;
//			}
//			else
//				if ( VertexInSpcy( pRigid, v1) || VertexInSpcy( pRigid, v2) ) 
//				{
//					return TRUE;
//				}
//	}
//	else
//	{
//		D3DXVECTOR3 vec[2][2];
//		vec[0][0] = pRigid->pSp->CtrPosition + pRigid->pSp->Height * pRigid->pSp->Normal;
//		vec[0][1] = pRigid->pSp->CtrPosition - pRigid->pSp->Height * pRigid->pSp->Normal;
//		vec[1][0] = v1;
//		vec[1][1] = v2;
// 
//		float d = 0.0f;
//		BOOL mParal = FALSE;
//
//		d = LineDist( vec[0][0], vec[0][1], vec[1][0], vec[1][1], mParal );
//
//		if ( d > fR ) 
//		{
//			return FALSE;
//		}
//		else 
//		{
//			D3DXVECTOR3 vecDist;
//
//			for ( int i = 0; i < 2; i++ )
//				for ( int j = 0; j < 2; j++ )
//				{
//					vecDist = vec[0][i] - vec[1][j];
//					if ( D3DXVec3Length( &vecDist ) <= fR )
//						return TRUE;
//				}
//
//				if( ( d < 0.0f ) && ( d > - 0.0f ) )  //共面相交
//				{
//
//					if( LineSegIntersect( vec[0][0], vec[0][1], vec[1][0], vec[1][1] ) )
//					{
//						return TRUE;
//					}
//					else
//					{
//						//
//						D3DXVECTOR3 vecDist;
//
//						float Dist = 0.0f;
//
//						for ( int i = 0; i < 2; i++ )
//							for ( int j = 0; j < 2; j++ )
//							{
//								Dist = VertexLineDist( vec[i][j], vec[1-i][0], vec[1-i][1] );
//								if ( ( Dist <= fR ) &&
//									IsAcuteAngle( vec[i][j], vec[1-i][0], vec[1-i][1] ) &&
//									IsAcuteAngle( vec[i][j], vec[1-i][1], vec[1-i][0] ) )
//									return TRUE;
//							}
//
//							return FALSE;
//					}
//
//				}
//				else 
//					if( mParal ) //两轴共面平行
//					{
//						if( ( IsAcuteAngle( vec[0][0], vec[1][0], vec[1][1] ) &&  IsAcuteAngle( vec[0][0], vec[1][1], vec[1][0] ) ) ||
//							( IsAcuteAngle( vec[1][0], vec[1][0], vec[1][1] ) &&  IsAcuteAngle( vec[1][0], vec[1][1], vec[1][0] ) ) )
//							return TRUE;
//						else 
//						{
//							return FALSE;
//						}
//
//					}
//					else //两轴异面
//					{
//						if( CheckSeparateVertex( vec[0][0], vec[0][1], vec[1][0], vec[1][1] ) )
//							return TRUE;
//						else //转化为球与LSS
//						{
//
//							//
//							D3DXVECTOR3 vecDist;
//							float Dist = 0.0f;
//
//							for ( int i = 0; i < 2; i++ )
//								for ( int j = 0; j < 2; j++ )
//								{
//									Dist = VertexLineDist( vec[i][j], vec[1-i][0], vec[1-i][1] );
//									if ( ( Dist <= fR ) &&
//										IsAcuteAngle( vec[i][j], vec[1-i][0], vec[1-i][1] ) &&
//										IsAcuteAngle( vec[i][j], vec[1-i][1], vec[1-i][0] ) )
//										return TRUE;
//								}
//								return FALSE;
//
//						}
//
//					}
//		}
//
//	}
//	return FALSE;
//}



//
//BOOL KPhysicsSimulator::TriSpcyIntersect(LPRIGIDBODY pRigid, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2, const D3DXVECTOR3& v3)
//{
///*
//算法一： 设N为pRigid的中轴直线， M为中轴线段， A[2]为pRigid轴线段两端点
//      if( v[i] in pRigid )
//		return true;
//	  else if ( N in  三角形中)
//	  {
//		if( M in 三角形中 )
//			return true；
//		else if ( A[i] )
//		    
//
//	  }
//	  
//
//
//*/
//    return FALSE;
//}
//
//


/**
  * @brief 用于判断一点是否在一个刚体内
  */
BOOL KPhysicsSimulator::VertexInSpcy(LPRIGIDBODY pRigid, const D3DXVECTOR3& v1)
{
	if( pRigid->pSp->Height == 0 ) //刚体是球体的情况
	{
		D3DXVECTOR3 vecTemp = v1 - pRigid->pSp->CtrPosition;
		float d = D3DXVec3Length( &vecTemp );

		if ( d <= pRigid->pSp->Radius )
			return TRUE;
	}
	else
	{
		D3DXVECTOR3 vec[2];
		vec[0] = pRigid->pSp->CtrPosition + pRigid->pSp->Height * pRigid->pSp->Normal;
		vec[1] = pRigid->pSp->CtrPosition - pRigid->pSp->Height * pRigid->pSp->Normal;

		for ( int i=0; i<2; i++ )
		{
			D3DXVECTOR3 vecTemp = v1 - vec[i];
			float d = D3DXVec3Length( &vecTemp );

			if ( d <= pRigid->pSp->Radius )
				return TRUE;
		}

		float Dist = VertexLineDist( v1, vec[0], vec[1] ); 
		if ( ( Dist <= ( pRigid->pSp->Radius) ) &&
			IsAcuteAngle( v1, vec[0], vec[1] ) &&
			IsAcuteAngle( v1, vec[1], vec[0] ) )
			return TRUE;
	}
	return FALSE;
}


//
//void KPhysicsSimulator::PlaneCollisionSimulation(float Tinytime)
//{
///*
//	list<LPRIGIDBODY>::iterator i = listRigidBody.begin();
//
//	D3DXVECTOR3 point = D3DXVECTOR3( 0, 0, 0 );
//	D3DXVECTOR3 normal = D3DXVECTOR3( 0, 1, 0 );
//
//
//	while(i!=listRigidBody.end())
//	{
//
//		LPRIGIDBODY pRigid = *i;
//
//		D3DXVECTOR3 inter;
//		BOOL bInter = FALSE;
//
//		float f_J = (pRigid->pSp->Height+pRigid->pSp->Radius) * (pRigid->pSp->Height+pRigid->pSp->Radius);
////		f_J = 1/f_J;
//
//		
//		pRigid->m_LineVel += 90*Tinytime*D3DXVECTOR3( 0, -1 , 0);
//
//		bInter = SpcyPlaneIntersect1( pRigid, point, normal, inter );
//		if ( bInter )
//		{
//			D3DXVECTOR3 vel[2]; 
//			float k = D3DXVec3Dot( &pRigid->m_LineVel, &normal ); 
//			vel[0] = pRigid->m_LineVel - k*normal ;
//			vel[1] = 0.6*fabs(k) * normal;
//			pRigid->m_LineVel = (vel[0] + vel[1] );
//
//			D3DXVECTOR3 q = inter - pRigid->pSp->CtrPosition;
//			D3DXVECTOR3 pt = 1.5 * vel[1];
//			D3DXVECTOR3 dCross;
//			D3DXVec3Cross( &dCross, &q, &pt );
//
//			pRigid->m_AngVel += ( dCross/f_J );
//
//		}
//
//		StepRigidBody( pRigid, Tinytime );
//
//		i++;
//	}
//*/
//
//	list<LPRIGIDBODY>::iterator i = listRigidBody.begin();
//
//	D3DXVECTOR3 point = D3DXVECTOR3( 0, 0, 0 );
//	D3DXVECTOR3 normal = D3DXVECTOR3( 0, 1, 0 );
//
//
//
//	while(i!=listRigidBody.end())
//	{
//
//		LPRIGIDBODY pRigid = *i;
//
//		D3DXVECTOR3 inter;
//		BOOL bInter = FALSE;
//
//		float f_J = ( (pRigid->pSp->Height+pRigid->pSp->Radius) * (pRigid->pSp->Height+pRigid->pSp->Radius) ) /12 ;
//
////		f_J = 1/f_J;
//
//		float d = 0.0f ;
//		D3DXVECTOR3 vt;
//		vt = pRigid->pSp->CtrPosition - point;
//		d = D3DXVec3Dot( &vt, &normal);
//		if( fabs(d) >= pRigid->pSp->Radius)
//		{
//
//
//			pRigid->m_LineVel += 90*Tinytime*D3DXVECTOR3( 0, -1 , 0);
//
//			bInter = SpcyPlaneIntersect1( pRigid, point, normal, inter );
//
//
//
//			if ( bInter )
//			{
//				D3DXVECTOR3 q = inter - pRigid->pSp->CtrPosition;
//				D3DXVECTOR3 vel_p;
//				D3DXVec3Cross( &vel_p, &pRigid->m_AngVel, &q );
//				vel_p = vel_p + pRigid->m_LineVel;
//
//				//			vel_p = pRigid->m_LineVel;
//
//				float vrel = D3DXVec3Dot( &normal, &vel_p);
//
//				if( vrel < 0 ) 
//				{
//					D3DXVECTOR3 vel;
//					D3DXVec3Cross( &vel, &q, &normal );
//					vel = vel / f_J;
//
//					D3DXVec3Cross( &vel, &vel, &q ); 
//
//					float j = 0.0f;
//
//					j = D3DXVec3Dot( &normal, &vel ) + 1;
//					j = (-2*vrel) / j;
//
//					D3DXVECTOR3 impulse;
//
//					impulse = j*normal;
//
//					D3DXVECTOR3 torque;
//
//					D3DXVec3Cross( &torque, &q, &impulse );
//
//					pRigid->m_LineVel = pRigid->m_LineVel + impulse;
//					pRigid->m_AngVel = pRigid->m_AngVel + torque / f_J ;
//
//				//	kk++;
//
//				}
//
//			}
//
//			//if ( kk < 2 )
//			//{
//			StepRigidBody( pRigid, Tinytime );
//		//	}
//		}
//
//		i++;
//	}
//
//}

//
//void KPhysicsSimulator::TestConstraintMaxDist(float Tinytime)
//{
//
///*
//	list<LPRIGIDBODY>::iterator i = listRigidBody.begin();
//
//	while(i!=listRigidBody.end())
//	{
//		LPRIGIDBODY pRigid = *i;		
//		StepRigidBody( pRigid, Tinytime );
//		i++;
//	}
//*/
//
//	list<KConstraint*>::iterator j = listConstraint.begin();
//	
//
//	while(j!=listConstraint.end())
//	{
//		KConstraint* pConstraint = *j;
//		pConstraint->PreApply(Tinytime);
//		pConstraint->Apply(Tinytime);
//		j++;
//	}
//
//
//	list<LPRIGIDBODY>::iterator i = listRigidBody.begin();
//
//	while(i!=listRigidBody.end())
//	{
//		LPRIGIDBODY pRigid = *i;
//		pRigid->UpdateVelocity( Tinytime );
//		pRigid->UpdatePostion( Tinytime );
////		StepRigidBody( pRigid, Tinytime);
//		i++;
//	}
//
//}



//void KPhysicsSimulator::TestFixedPoint(float Tinytime)
//{
//	list<LPRIGIDBODY>::iterator i = listRigidBody.begin();
//
//	while(i!=listRigidBody.end())
//	{
//		LPRIGIDBODY pRigid = *i;
//		pRigid->UpdateVelocity( Tinytime );
//		pRigid->UpdatePostion( Tinytime );
//		i++;
//	}
//
//	list<KConstraint*>::iterator j = listConstraint.begin();
//
//	while(j!=listConstraint.end())
//	{
//		KConstraint* pConstraint = *j;
//		pConstraint->PreApply(Tinytime);
//		pConstraint->Apply(Tinytime);
//		j++;
//	}
//}



//bool RotFlag = false;
//
//void KPhysicsSimulator::TestRotation(float TinyTime)
//{
//		D3DXVECTOR3 pos = D3DXVECTOR3(1,1,1);
//		D3DXVECTOR3 ctpos = D3DXVECTOR3(0,0,1);
//		D3DXMATRIX ort;
//		D3DXMatrixIdentity( &ort );
//		D3DXMatrixTranslation( &ort, pos.x, pos.y, pos.z );
//		D3DXVECTOR3 vr1;
//		D3DXVECTOR3 vr2;
//		D3DXVec3TransformCoord( &vr1, &ctpos, &ort );
//		
//
//		float theta = - 3.14f / 2;
//
//		D3DXMATRIX mat1;
//		D3DXVECTOR3 Axis = D3DXVECTOR3(0,1,0);
//
//		D3DXMatrixRotationAxis( &mat1, &Axis, theta ); 
//		ort = mat1 * ort;
//		D3DXVec3TransformCoord( &vr1, &ctpos, &ort );
//
//}

/**
  * @brief 更新所有刚体的速度
  * @param Tinytime 时间间隔
  */
int KPhysicsSimulator::UpdateAllVelocities(float Tinytime)
{
	list<LPRIGIDBODY>::iterator i = listRigidBody.begin();

	while(i!=listRigidBody.end())
	{
		LPRIGIDBODY pRigid = *i;
		pRigid->UpdateVelocity( Tinytime );
//		pRigid->UpdateVelocity( Tinytime, 0.005f );
//		pRigid->m_LineVel += 90*Tinytime*D3DXVECTOR3( 0, -1 , 0);
		i++;
	}

	return 0;
}

/**
  * @brief 更新所有刚体的位置
  * @param Tinytime 时间间隔
  */
int KPhysicsSimulator::UpdateAllPositions(float Tinytime)
{
	list<LPRIGIDBODY>::iterator i = listRigidBody.begin();

	while(i!=listRigidBody.end())
	{
		LPRIGIDBODY pRigid = *i;
		pRigid->UpdatePostion( Tinytime );
		
//		g_cGraphicsTool.DrawPosition( &pRigid->m_Position );

		//StepRigidBody( pRigid, Tinytime );
		i++;
	}

	return 0;
}


/**
  * @brief 所有刚体的外力清零
  */
int KPhysicsSimulator::ClearAllForces(void)
{
	list<LPRIGIDBODY>::iterator i = listRigidBody.begin();

	while(i!=listRigidBody.end())
	{
		LPRIGIDBODY pRigid = *i;
		pRigid->ClearForce();
		i++;
	}

	return 0;
}


/**
  * @brief 为所有刚体添加外力
  */
int KPhysicsSimulator:: GetAllForces(void)
{
	if(m_bAddingGravity)
	{
		list<LPRIGIDBODY>::iterator i = listRigidBody.begin();

		while(i!=listRigidBody.end())
		{
			LPRIGIDBODY pRigid = *i;


			if (pRigid->m_Position.y > pRigid->pSp->Radius)
			{

				pRigid->m_Force = pRigid->m_fMass * D3DXVECTOR3( 0.0f, -9.80f, 0.0f ) ;			

			}



			i++;
		}
	}

	return 0;
}


/**
  * @brief 检测所有的碰撞
  */
int KPhysicsSimulator::DetectAllCollisions(void)
{
	vecCollisionInfo.clear();
	
	list<LPRIGIDBODY>::iterator i = listRigidBody.begin();

	while(i!=listRigidBody.end())
	{
		LPRIGIDBODY pRigid = *i;
		pRigid->Collision().clear();
		i++;
	}

	DetectCollisions();

	return 0;
}



/**
  * @brief 根据平面列表和刚体列表进行碰撞检测
  */
int KPhysicsSimulator::DetectCollisions(void)
{

	list<LPRIGIDBODY>::iterator i = listRigidBody.begin();
	list<D3DXPLANE>::iterator j = listPlane.begin();

	while(i!=listRigidBody.end())
	{
		LPRIGIDBODY pRigid = *i;

		j = listPlane.begin();
		while( j!= listPlane.end() )
		{
			DetectHeightMapCollision( pRigid, *j );
			j++;
		}
		i++;
	}


	i = listRigidBody.begin();
	list<LPRIGIDBODY>::iterator k = listRigidBody.begin();

	while(i!=listRigidBody.end())
	{
		LPRIGIDBODY pRigid1 = *i;
		vector<LPRIGIDBODY>& iNC = pRigid1->NonCollidable();

		k = i;
		k++;

		while(k!=listRigidBody.end())
		{
			LPRIGIDBODY pRigid2 = *k;
			if( IsCollidable( pRigid2, iNC ))
			{
				DetectBodyCollision( pRigid1, pRigid2 );
			}
			k++;
		}
		i++;
	}

	return 0;
}


/**
  * @brief 刚体与平面的碰撞检测，记录碰撞信息KCollisionInfo，
  */
int KPhysicsSimulator::DetectHeightMapCollision( LPRIGIDBODY pRigid, D3DXPLANE& plane )
{
	D3DXVECTOR3 nor = D3DXVECTOR3(0,0,0);
	D3DXVECTOR3 point = D3DXVECTOR3(0,0,0);
	D3DXVECTOR3 inter = D3DXVECTOR3(0,0,0);
	nor.x = plane.a;
	nor.y = plane.b;
	nor.z = plane.c;

	point = D3DXVECTOR3( 0,0, -plane.d * SafeInvNum(plane.c) );

	float depth = 0.0f;
	BOOL bInter = SpcyPlaneIntersect( pRigid, plane, inter, depth );

	if ( bInter )
	{
		vecCollisionInfo.push_back( KCollisionInfo( pRigid, NULL, &plane, nor, inter, depth ) );
		pRigid->Collision().push_back( vecCollisionInfo.size() - 1);
	}

//	g_cGraphicsTool.DrawPosition( &inter );
	return 0;
}



/**
  * @brief 两刚体碰撞检测，记录碰撞信息KCollisionInfo，
  */
int KPhysicsSimulator::DetectBodyCollision( LPRIGIDBODY pRigid1, LPRIGIDBODY pRigid2)
{

	D3DXVECTOR3 Inter = D3DXVECTOR3(0,0,0);
	D3DXVECTOR3 DirTo1 = D3DXVECTOR3(0,0,0);
	float depth = 0.0f;

	BOOL bInter = SpcyIntersect( pRigid1, pRigid2, Inter, DirTo1, depth );

	if ( bInter )
	{

		D3DXVECTOR3 v1, v2;
		D3DXVec3Cross( &v1, &pRigid1->m_AngVel, &( Inter - pRigid1->m_Position ) );
		v1 += pRigid1->m_LineVel;
		D3DXVec3Cross( &v2, &pRigid2->m_AngVel, &( Inter - pRigid2->m_Position ) );
		v2 += pRigid2->m_LineVel;
		DirTo1 = v2 - v1;
		D3DXVec3Normalize( &DirTo1, &DirTo1 );

		vecCollisionInfo.push_back( KCollisionInfo( pRigid1, pRigid2, NULL , DirTo1, Inter, depth ) );
		pRigid1->Collision().push_back( vecCollisionInfo.size() - 1);
		pRigid2->Collision().push_back( vecCollisionInfo.size() - 1);
	}

	return 0;
}


/**
  * @brief 根据碰撞信息KCollisionInfo，预处理碰撞响应，用于计算碰撞冲量所需的分母值
  * @param Tinytime 时间间隔
  * @param collision 碰撞信息
  */
int KPhysicsSimulator::PreProcessCollision( KCollisionInfo& collision, float TinyTime )
{
	LPRIGIDBODY pRigid1 = collision.m_pRigid1;
	LPRIGIDBODY pRigid2 = NULL;

	if( collision.m_pRigid2 )
	{
		pRigid2 = collision.m_pRigid2;
	}


////#ifdef NoPenetration
//	if(m_bNonePenetration)
//	{
//		float AllowedPenetration = 0.0f;
//		float TimeScale = 5*TinyTime;
//
//		if ( collision.m_fPenetrationDepth > AllowedPenetration )
//		{
//			collision.m_VrExtra =  ( collision.m_fPenetrationDepth - AllowedPenetration ) * SafeInvNum( TimeScale ) ; 
//		}
//		else 
//			collision.m_VrExtra = 0.0f;
//	}
////#endif


	if( pRigid2 )
	{
		collision.m_R1 = collision.m_ColPosition - pRigid1->m_Position;
		collision.m_R2 = collision.m_ColPosition - pRigid2->m_Position;

		const D3DXVECTOR3& N = collision.m_DirTo1;

		D3DXVECTOR3 dCross[2];
		D3DXVec3Cross( &dCross[0], &collision.m_R1, &N);
		D3DXVec3Cross( &dCross[1], &collision.m_R2, &N);
		dCross[0] *= pRigid1->m_fInvInertia;
		dCross[1] *= pRigid2->m_fInvInertia;

		D3DXVec3Cross( &dCross[0], &dCross[0], &collision.m_R1);
		D3DXVec3Cross( &dCross[1], &dCross[1], &collision.m_R2);
		collision.m_fDenominator = pRigid1->m_fInvMass + D3DXVec3Dot( &dCross[0], &N ) + pRigid2->m_fInvMass + D3DXVec3Dot( &dCross[1], &N );

//#ifdef USING_FRICTION
		if(m_bUsingFriction)
		{
			collision.m_fStaticFriction = 0.5f * (pRigid1->m_fStaticFriction + pRigid2->m_fStaticFriction);
			collision.m_fDynamicFriction = 0.5f * (pRigid1->m_fDynamicFriction + pRigid2->m_fDynamicFriction);
		}
//#endif
	}
	else
	{
		collision.m_R1 = collision.m_ColPosition - pRigid1->m_Position;

		const D3DXVECTOR3& N = collision.m_DirTo1;

		D3DXVECTOR3 dCross;
		D3DXVec3Cross( &dCross, &collision.m_R1, &N);
		dCross *= pRigid1->m_fInvInertia;

		D3DXVec3Cross( &dCross, &dCross, &collision.m_R1);
		collision.m_fDenominator = pRigid1->m_fInvMass + D3DXVec3Dot( &dCross, &N );

//#ifdef USING_FRICTION
		if(m_bUsingFriction)
		{
			collision.m_fStaticFriction = pRigid1->m_fStaticFriction; 
			collision.m_fDynamicFriction = pRigid1->m_fDynamicFriction;
		}
//#endif

	}

    return 0;
}

/**
  * @brief 根据碰撞信息KCollisionInfo，实现碰撞响应
  * @param Tinytime 时间间隔
  * @param collision 碰撞信息
  * @param epsilon 碰撞恢复系数(0~1之间)
  */
int KPhysicsSimulator::ProcessCollision( KCollisionInfo& collision, float TinyTime, float epsilon)
{
//	g_cGraphicsTool.DrawPosition( &collision.m_pRigid1->m_Position );

	LPRIGIDBODY pRigid1 = collision.m_pRigid1;
	LPRIGIDBODY pRigid2 = NULL;

	if( collision.m_pRigid2 )
	{
		pRigid2 = collision.m_pRigid2;
	}

	if ( collision.m_fDenominator < 0.001f )
		return -1;

	const D3DXVECTOR3& N = collision.m_DirTo1;

	float NormalVel = 0.0f;

	if( pRigid2 )
	{
		D3DXVECTOR3 dCross[2];
		D3DXVec3Cross( &dCross[0], &pRigid1->m_AngVel, &collision.m_R1);
		D3DXVec3Cross( &dCross[1], &pRigid2->m_AngVel, &collision.m_R2);

		dCross[0] += pRigid1->m_LineVel;
		dCross[1] += pRigid2->m_LineVel;

//		if ( D3DXVec3Dot( &dCross[0], &N ) >= 0 )
//			return -1;

		NormalVel = D3DXVec3Dot( &(dCross[0] - dCross[1]) , &N );

	}
	else
	{
		D3DXVECTOR3 dCross;
		D3DXVec3Cross( &dCross, &pRigid1->m_AngVel, &collision.m_R1);

		dCross += pRigid1->m_LineVel;


//		if ( D3DXVec3Dot( &dCross, &N ) >= 0 )
//			return -1;

		NormalVel = D3DXVec3Dot( &dCross, &N );
	}

	float FinalNormalVel = -epsilon * NormalVel;

////#ifdef NoPenetration
//	if(m_bNonePenetration)
//	{
//		float Extra = collision.m_VrExtra - FinalNormalVel;
//		if ( Extra > 0.0f )
//			FinalNormalVel += Extra;
//	}
////#endif

	float DeltaVel = FinalNormalVel - NormalVel;

	if ( DeltaVel < 0.001f ) 
		return -1;

	float Numerator = DeltaVel;

	float NormalImpulse = Numerator / collision.m_fDenominator;

	pRigid1->ApplyWorldImpulse( NormalImpulse*N, collision.m_ColPosition );

	if ( pRigid2 )
		pRigid2->ApplyWorldImpulse( - NormalImpulse*N, collision.m_ColPosition );



	//#ifdef USING_FRICTION		
	if(m_bUsingFriction)
	{
		//For friction
		D3DXVECTOR3 VrNew1;
		D3DXVec3Cross( &VrNew1, &pRigid1->m_AngVel, &collision.m_R1 );
		VrNew1 += pRigid1->m_LineVel;

		if( pRigid2 )
		{
			D3DXVECTOR3 VrNew2;
			D3DXVec3Cross( &VrNew2, &pRigid2->m_AngVel, &collision.m_R2 );
			VrNew2 += pRigid2->m_LineVel;
			VrNew1 -= VrNew2;
		}

		D3DXVECTOR3 TangentVel = VrNew1 - D3DXVec3Dot( &VrNew1, &N ) * N;
		float TangentSpeed = D3DXVec3Length( &TangentVel );

		if ( TangentSpeed > 0.0f )
		{
			D3DXVECTOR3 T;
			D3DXVec3Normalize( &T, &TangentVel);
			T = -T;

			Numerator = TangentSpeed;
			float Denominator;

			if( pRigid2 )
			{
				D3DXVECTOR3 tmpCross[2];
				D3DXVec3Cross( &tmpCross[0], &collision.m_R1, &T);
				D3DXVec3Cross( &tmpCross[1], &collision.m_R2, &T);
				tmpCross[0] *= pRigid1->m_fInvInertia;
				tmpCross[1] *= pRigid2->m_fInvInertia;

				D3DXVec3Cross( &tmpCross[0], &tmpCross[0], &collision.m_R1);
				D3DXVec3Cross( &tmpCross[1], &tmpCross[1], &collision.m_R2);
				Denominator = pRigid1->m_fInvMass + D3DXVec3Dot( &tmpCross[0], &T ) + pRigid2->m_fInvMass + D3DXVec3Dot( &tmpCross[1], &T );

			}
			else
			{
				D3DXVECTOR3 tmpCross;
				D3DXVec3Cross( &tmpCross, &collision.m_R1, &T);
				tmpCross *= pRigid1->m_fInvInertia;

				D3DXVec3Cross( &tmpCross, &tmpCross, &collision.m_R1);
				Denominator = pRigid1->m_fInvMass + D3DXVec3Dot( &tmpCross, &T );
			}

			if ( Denominator > 0.0f )
			{ 
				float ImpulseToReverse = Numerator / Denominator ;
				float StaticFriction = collision.m_fStaticFriction;
				float DynamicFriction = collision.m_fDynamicFriction;

				float ImpulseFromNormalImpulse = StaticFriction * NormalImpulse;
				float FrictionImpulse;

				if (ImpulseToReverse < ImpulseFromNormalImpulse)
					FrictionImpulse = ImpulseToReverse;
				else
					FrictionImpulse = DynamicFriction * NormalImpulse;

				pRigid1->ApplyWorldImpulse( FrictionImpulse*T, collision.m_ColPosition );

				if ( pRigid2 )
					pRigid2->ApplyWorldImpulse( - FrictionImpulse*T, collision.m_ColPosition );

			}
		}
		//#endif
	}

	return 0;
}


/**
  * @brief 碰撞处理，包括碰撞检测和碰撞响应
  * @param Tinytime 时间间隔
  */
int KPhysicsSimulator::HandleAllCollisions(float TinyTime)
{
//
	int num_bodies = listRigidBody.size();

	vecStoredData.resize(num_bodies);
	list<LPRIGIDBODY>::iterator iRigid = listRigidBody.begin();
	int step = 0;
	while(iRigid!=listRigidBody.end())
	{
		LPRIGIDBODY pRigid = *iRigid;
		vecStoredData[step].m_AngVel = pRigid->m_AngVel;
		vecStoredData[step].m_LineVel = pRigid->m_LineVel;
		vecStoredData[step].m_matOrient = pRigid->m_matOrient;
		vecStoredData[step].m_Position = pRigid->m_Position;


		iRigid++;
		step++;
	}

	UpdateAllVelocities(TinyTime);
	UpdateAllPositions(TinyTime);
//

	DetectAllCollisions();

//	int testsize = vecCollisionInfo.size();
//	

	iRigid = listRigidBody.begin();
	step = 0;

	while(iRigid!=listRigidBody.end())
	{
		LPRIGIDBODY pRigid = *iRigid;
		pRigid->m_AngVel = vecStoredData[step].m_AngVel;
		pRigid->m_LineVel = vecStoredData[step].m_LineVel;

		iRigid++;
		step++;
	}

//
	iRigid = listRigidBody.begin();
	step = 0;

	while(iRigid!=listRigidBody.end())
	{
		LPRIGIDBODY pRigid = *iRigid;
		pRigid->m_matOrient = vecStoredData[step].m_matOrient;
		pRigid->m_Position = vecStoredData[step].m_Position;
		iRigid++;
		step++;
	}

//
	vector<KCollisionInfo>::iterator iColInfo = vecCollisionInfo.begin();

	while(iColInfo!=vecCollisionInfo.end())
	{
		KCollisionInfo CollisionInfo = *iColInfo;
		PreProcessCollision( CollisionInfo, TinyTime );
//		ProcessCollision( CollisionInfo, TinyTime, 0.65f );
		ProcessCollision( CollisionInfo, TinyTime, m_fEpsilon );
		iColInfo++;
	}

//	
	vecStoredData.clear();
  
	return 0;
}


///**
//  * @brief 根据碰撞信息分离互相嵌入状态的刚体
//  * @@param collision 碰撞信息
//  * @@param factor 分离的系数,要小于1.0f
//  */
//int KPhysicsSimulator::SeparateObjects( KCollisionInfo& collision, float factor )
//{
//	LPRIGIDBODY pRigid1 = collision.m_pRigid1;
//	
//	if( pRigid1->m_bImmovable )
//		return 0;
//
//	if ( ( collision.m_pRigid2 == NULL ) || ( collision.m_pRigid2->m_bImmovable ) )
//	{
//		D3DXVECTOR3 delta = factor*collision.m_fPenetrationDepth*collision.m_DirTo1;
//		D3DXVECTOR3 orig = pRigid1->m_Position;
//		pRigid1->m_Position +=  delta;
//
//		int nCollNum = pRigid1->Collision().size();
//
//		for( int i=0; i<nCollNum; i++ )
//		{
//			KCollisionInfo& CollInfo = vecCollisionInfo[ pRigid1->Collision()[i] ];
//			if ( CollInfo.m_pRigid1 == pRigid1 )
//				CollInfo.m_fPenetrationDepth -= D3DXVec3Dot( &delta, &CollInfo.m_DirTo1 );
//			else
//				CollInfo.m_fPenetrationDepth += D3DXVec3Dot( &delta, &CollInfo.m_DirTo1 );
//		}
//	}
//
//	return 0;
//}


//
//int KPhysicsSimulator::HandleAllConstraints(float TinyTime)
//{
//	const int nCollNum = vecCollisionInfo.size();
//
//	for ( int step=0; step<10; step++ )
//	{
//		BOOL bGotOne = FALSE;
//
//		for ( int i=0; i< nCollNum; i++ )
//		{
//			if( vecCollisionInfo[i].m_fPenetrationDepth > 0.0001f )
//			{
//				SeparateObjects( vecCollisionInfo[i], 0.008f);
//				bGotOne = TRUE;
//			}
//		}
//
//		if(!bGotOne)
//			break;
//	}
//
//	return 0;
//}

/**
  * @brief 计算每时间步的物理系统的状态
  * @param Tinytime 时间间隔
  */
int KPhysicsSimulator::DoTimeStep(float Tinytime)
{
	GetAllForces();
//	DetectAllCollisions();
	HandleAllCollisions(Tinytime);
	UpdateAllVelocities(Tinytime);
	ClearAllForces();
//	HandleAllConstraints(Tinytime);
	UpdateAllPositions(Tinytime);

	return 0;
}

};