/*
**************************************************************************
*
* Project		: SceneEditor
* FileName		: KRigidBody.cpp
* Coded by		: HJQ
* Date			: 2004-05-27
* Comment	    : 刚体类
* Copyright 2004 Kingsoft Corp.
* All Rights Reserved.
*
**************************************************************************
*/
// KRigidBody.cpp: implementation of the KRigidBody class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sceneeditor.h"
#include "KRigidBody.h"
//#include ".\krigidbody.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace Physics
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


KRigidBody::KRigidBody()
{
	pSp = new _SPCY ;
	pSp->CtrPosition = D3DXVECTOR3(0,0,0);
	pSp->Height = 0.0f; //半高
	pSp->Radius = 1.0f;
	pSp->Normal = D3DXVECTOR3(0,1,0);

	m_LineVel = D3DXVECTOR3(0,0,0);
//	m_LineAcc = D3DXVECTOR3(0,0,0);
	m_AngVel = D3DXVECTOR3(0,0,0);
//	m_RotAxis = D3DXVECTOR3(0,1,0);

	m_Position = D3DXVECTOR3(0,0,0);
	m_RelPos = D3DXVECTOR3(0,0,0);
	m_RelNormal = D3DXVECTOR3(0,1,0);

	m_fMass = 1.0f;
	m_fInvMass = 1.0f;
	m_fInertia = 0.4f;
	m_fInvInertia = 2.5f;

	m_Force = D3DXVECTOR3(0,0,0);
	m_Torque = D3DXVECTOR3(0,0,0);

	D3DXMatrixIdentity(&m_matOrient);
//	D3DXMatrixIdentity(&m_matInertia);
//	D3DXMatrixIdentity(&m_matInvInertia);
//	D3DXMatrixIdentity(&m_matWorldI);
//	D3DXMatrixIdentity(&m_matWorldInvI);

//#ifdef USING_FRICTION
	m_fStaticFriction = 0.30f;
	m_fDynamicFriction = 0.30f;
//#endif

	//m_bImmovable = FALSE;
}


KRigidBody::~KRigidBody()
{
	//SAFE_DELETE( pSp );

	if ( pSp )
	{
		delete pSp;
		pSp = NULL;
	}

	vecCollision.clear();
	vecNonCollidable.clear();
}

/**
  * @brief 设置刚体
  * @param Pos  质心坐标
  * @param fMass  刚体质量
  * @param fInertia  刚体转动惯量
  */
int KRigidBody::Initialise( D3DXVECTOR3 Pos, float fMass, float fInertia )
{
	m_Position = Pos;
	SetMass( fMass );
	SetInertia( fInertia );
//	m_matOrient = matOrient;
//	D3DXMatrixTranslation( &m_matOrient, Pos.x, Pos.y, Pos.z );

	m_RelNormal = pSp->Normal;
	m_RelPos = pSp->CtrPosition - m_Position;

	return 0;
}

int KRigidBody::Initialise( D3DXVECTOR3 Pos , float fMass )
{
	m_Position = Pos;
	SetMass( fMass );

	float fInertia = 0.0f;
	if( pSp->Height == 0.0f )
	{
		fInertia = 0.4f * fMass * pSp->Radius * pSp->Radius;
		SetInertia( fInertia );
	}
	else 
		if( pSp->Height > 0.0f )
		{
			fInertia = 0.08333f * fMass * (pSp->Radius + pSp->Height) * (pSp->Radius + pSp->Height);
			SetInertia( fInertia );
		}

	m_RelNormal = pSp->Normal;
	m_RelPos = pSp->CtrPosition - m_Position;

	return 0;
}

/**
  * @brief 设置pSp
  * @param Pos  pSp中心点坐标
  * @param Normal  pSp法向
  * @param Height  pSp半高
  * @param Radius  pSp半径
  */
int KRigidBody::SetSP(D3DXVECTOR3 Pos, D3DXVECTOR3 Normal, float Height, float Radius)
{
	if ( ( Height >= 0 )&&( Radius > 0 ) )
	{
		pSp->CtrPosition = Pos;
		pSp->Height = Height; 
		pSp->Radius = Radius;
		pSp->Normal = Normal;
	}

	return 0;
}


/**
  * @brief 设置质量
  * @param mass  质量值
  */
int KRigidBody::SetMass(float mass)
{
	if ( mass > 0 )
	{
		m_fMass = mass;
		m_fInvMass = ( 1.0f / mass ) ;	
	}

	return 0;
}


/**
  * @brief 设置转动惯量
  * @param inertia  转动惯量值
  */
int KRigidBody::SetInertia(float inertia)
{
	if ( inertia > 0 )
	{
		m_fInertia = inertia;
		m_fInvInertia = ( 1.0f / inertia );
	}

	return 0;
}

/*
int KRigidBody::SetInertia(float Ixx, float Iyy, float Izz)
{
	m_matInertia._11 = Ixx;
	m_matInertia._22 = Iyy;
	m_matInertia._33 = Izz;

	m_matInvInertia._11 = SafeInvNum( Ixx );
	m_matInvInertia._22 = SafeInvNum( Iyy );
	m_matInvInertia._33 = SafeInvNum( Izz );

	return 0;
}
*/

/**
  * @brief 在质心作用线性冲量于刚体
  * @param Impulse  冲量
  */
int KRigidBody::ApplyWorldImpulse(const D3DXVECTOR3& Impulse)
{
	m_LineVel += Impulse * m_fInvMass;

	return 0;
}


/**
  * @brief 作用角冲量于刚体
  * @param Impulse  冲量
  */
int KRigidBody::ApplyWorldAngImpulse(const D3DXVECTOR3& Impulse)
{
	m_AngVel += Impulse*m_fInvInertia;
	return 0;
}


/**
  * @brief 在点Pos作用冲量于刚体
  * @param Impulse  冲量
  * @param Pos 冲量作用点
  */
int KRigidBody::ApplyWorldImpulse(const D3DXVECTOR3& Impulse, const D3DXVECTOR3& Pos)
{
	m_LineVel += Impulse * m_fInvMass;
	
	D3DXVECTOR3 torque;
	D3DXVec3Cross( &torque, &(Pos - m_Position), &Impulse );
	m_AngVel += ( torque * m_fInvInertia );

	return 0;
}

/**
  * @brief 更新刚体的速度
  * @param Tinytime 时间间隔
  */
int  KRigidBody::UpdateVelocity( float TinyTime, float factor)
{
	//if (m_bImmovable)
	//	return 0;

	m_LineVel += m_fInvMass * TinyTime*m_Force;
	m_LineVel -= factor * m_LineVel;
	m_AngVel  += m_fInvInertia * TinyTime*m_Torque; 
	m_AngVel -= factor * m_AngVel;
	return 0;
}

/**
  * @brief 更新刚体的位置
  * @param Tinytime 时间间隔
  */
int  KRigidBody::UpdatePostion( float TinyTime )
{
/*
	TinyTime = 0.1;

	D3DXVECTOR3 TinyPos = TinyTime*m_LineVel;
	m_Position += TinyPos;

	D3DXMATRIX matRot;
	D3DXMATRIX matTran;
	D3DXVECTOR3 Axis;
	D3DXVec3Normalize( &Axis, &m_AngVel);

	float theta = D3DXVec3Length( &m_AngVel ) * TinyTime;
//	float theta = 3.14f / 20.0f;
	D3DXMatrixTranslation( &matTran, TinyPos.x, TinyPos.y, TinyPos.z );
	m_matOrient = matTran * m_matOrient ;
    D3DXMatrixRotationAxis( &matRot, &Axis, theta ); 
	m_matOrient = matRot * m_matOrient;


	D3DXVec3TransformCoord( &pSp->CtrPosition, &m_RelPos, &m_matOrient );
	D3DXVECTOR3 n;
	D3DXVec3TransformCoord( &n, &m_RelNormal, &m_matOrient );
	pSp->Normal = n - pSp->CtrPosition;
	D3DXVec3Normalize(&pSp->Normal,&pSp->Normal);

*/
//	TinyTime = 0.1;

/////////////////////////////////////////////////////////////////////////////////////////
	//if (m_bImmovable)
	//	return 0;

	D3DXVECTOR3 TinyPos = TinyTime*m_LineVel;
	m_Position += TinyPos;

	D3DXMATRIX matRot;
	D3DXVECTOR3 Axis;
	D3DXVec3Normalize( &Axis, &m_AngVel);
	float theta = D3DXVec3Length( &m_AngVel ) * TinyTime;
    D3DXMatrixRotationAxis( &matRot, &Axis, theta ); 
	m_matOrient = matRot * m_matOrient;


	D3DXVec3TransformCoord( &pSp->CtrPosition, &m_RelPos, &m_matOrient );
	D3DXVec3TransformCoord( &pSp->Normal, &m_RelNormal, &m_matOrient );
	pSp->CtrPosition += m_Position;
	
//	D3DXMATRIX matInvOriect;
//	D3DXMatrixInverse(&matInvOriect,&m_matOrient);

	return 0;
}

/**
  * @brief 清除刚体所受外力
  */
int KRigidBody::ClearForce(void)
{
	m_Force = D3DXVECTOR3(0,0,0);
	m_Torque = D3DXVECTOR3(0,0,0);
	return 0;
}

};