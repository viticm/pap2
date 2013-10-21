/*
**************************************************************************
*
* Project		: SceneEditor
* FileName		: KCollisionInfo.cpp
* Coded by		: HJQ
* Date			: 2004-07-02
* Comment	    : Åö×²ÐÅÏ¢Àà
* Copyright 2004 Kingsoft Corp.
* All Rights Reserved.
*
**************************************************************************
*/
#include "stdafx.h"
#include ".\kcollisioninfo.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

namespace Physics
{
KCollisionInfo::KCollisionInfo(void)
{
}

KCollisionInfo::KCollisionInfo ( LPRIGIDBODY pRigid1, LPRIGIDBODY pRigid2, D3DXPLANE* pPlane, const D3DXVECTOR3& DirTo1, const D3DXVECTOR3& ColPosition, float PenetrationDepth )
{
	m_pRigid1 = pRigid1;
	m_pRigid2 = pRigid2;
	m_pPlane = pPlane;

	m_DirTo1 = DirTo1;
	D3DXVec3Normalize( &m_DirTo1, &m_DirTo1 );
	m_ColPosition = ColPosition;
	m_fPenetrationDepth = PenetrationDepth;

	m_fDenominator = 0.0f;
}

KCollisionInfo::~KCollisionInfo(void)
{
}

};