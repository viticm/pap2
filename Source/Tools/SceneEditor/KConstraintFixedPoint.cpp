/*
**************************************************************************
*
* Project		: SceneEditor
* FileName		: KConstraintFixedPoint.h
* Coded by		: HJQ
* Date			: 2004-07-02
* Comment	    : 相关点约束子类
* Copyright 2004 Kingsoft Corp.
* All Rights Reserved.
*
**************************************************************************
*/
#include "stdafx.h"
#include ".\KConstraintFixedPoint.h"

namespace Physics
{

KConstraintFixedPoint::KConstraintFixedPoint(void)
{
}

KConstraintFixedPoint::~KConstraintFixedPoint(void)
{
}

int KConstraintFixedPoint::Initialise(LPRIGIDBODY pRigid1, D3DXVECTOR3 Rigid1Pos, LPRIGIDBODY pRigid2, D3DXVECTOR3 Rigid2Pos, float MaxDist )
{
	m_pRigid1 = pRigid1;
	m_pRigid2 = pRigid2;
	m_Rigid1Pos = Rigid1Pos;
	m_Rigid2Pos = Rigid2Pos;
	m_fMaxDist = MaxDist;
	return 0;
}

int KConstraintFixedPoint::PreApply(float TinyTime)
{
	D3DXVec3TransformCoord( &m_R1, &m_Rigid1Pos, &m_pRigid1->m_matOrient );
	D3DXVec3TransformCoord( &m_R2, &m_Rigid2Pos, &m_pRigid2->m_matOrient );

	const D3DXVECTOR3 WorldPos1 = m_pRigid1->m_Position + m_R1;
	const D3DXVECTOR3 WorldPos2 = m_pRigid2->m_Position + m_R2;

//	m_WorldPos = 0.5f * ( WorldPos1 + WorldPos2 );
	m_CurRelPos1 = WorldPos1 - WorldPos2;
	return 0;
}

BOOL KConstraintFixedPoint::Apply(float TinyTime)
{
	m_pRigid1->m_Position -= 0.5f*m_CurRelPos1;	
	m_pRigid2->m_Position += 0.5f*m_CurRelPos1;

	return FALSE;

}

};