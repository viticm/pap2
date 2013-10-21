#include "stdafx.h"
#include ".\kcollisionutil.h"

namespace Physics
{

	/*
BOOL SpcyPlaneIntersect( const LPRIGIDBODY pRigid, const D3DXPLANE & Plane, D3DXVECTOR3& Inter, float& PenetrationDepth )
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
			Inter = pos[0] - Dist[0]*N;
			PenetrationDepth = radius - Dist[0];
			return TRUE;
		}
		else
			if ( ( Dist[1] < Dist[0] )&&( Dist[1] <=radius ) )
			{
				Inter = pos[1] - Dist[1]*N;
				PenetrationDepth = radius - Dist[1];
				return TRUE;
			}
			else 
				if ( ( Dist[1] == Dist[0] )&&( Dist[1] <=radius ) )
				{
					Inter = CtPos - Dist[1]*N;
					PenetrationDepth = radius - Dist[0];
					return TRUE;
				}

	}

	return FALSE;
}

*/
};