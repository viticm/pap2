////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DMathFunctions.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-9-23 14:26:06
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG3DMathFunctions.h"


////////////////////////////////////////////////////////////////////////////////

namespace KG3D_CUSTOM_HELPERS
{
	BOOL IntersectRayTri( const D3DXVECTOR3& vRaySrc, const D3DXVECTOR3& vRayNormalizedDir , D3DXVECTOR3& p0, D3DXVECTOR3& p1,			D3DXVECTOR3& p2 , FLOAT* pU, FLOAT* pV, FLOAT* pDist )
	{
		//暂时先用着D3DX的版本，其实可以自己写的
		return D3DXIntersectTri(&p0, &p1, &p2, &vRaySrc, &vRayNormalizedDir, pU, pV, pDist);
	}

	BOOL IsUVWithinTri(FLOAT U, FLOAT V)
	{
		return 0 <= U && U <= 1 && 0 <= V && V <= 1 && U + V <= 1;
	}
};

#if defined(KG_ENABLE_TDD)
KG_TEST_BEGIN(IntersectRayTri)
{
	D3DXVECTOR3 Tri[3] = 
	{
		D3DXVECTOR3(0,0,1),
		D3DXVECTOR3(0,0,0),
		D3DXVECTOR3(1,0,0),
	};

	struct CaseStruct
	{
		D3DXVECTOR3 vRaySrc;
		D3DXVECTOR3 vRayNormalizedDir;
		
		BOOL bIntersectTri;
		FLOAT fDist;
	};

	FLOAT squareTwo = sqrt(2.f);

	CaseStruct Cases[] = 
	{
		{D3DXVECTOR3(0,1,0), D3DXVECTOR3(0,-1,0), TRUE, 1},
		{D3DXVECTOR3(0.5f,-1,0.5f), D3DXVECTOR3(0,1,0), TRUE, 1},
		{D3DXVECTOR3(0.01f,0.5f,0.01f), D3DXVECTOR3(1,-1,0), TRUE, 0.5f * squareTwo},
		{D3DXVECTOR3(0,1,0), D3DXVECTOR3(1,0,0), FALSE, 0},
	};

	FLOAT U, V, fDist;
	for (size_t i = 0; i < _countof(Cases); ++i)
	{
		D3DXVec3Normalize(&Cases[i].vRayNormalizedDir, &Cases[i].vRayNormalizedDir);
		BOOL bRet = KG3D_CUSTOM_HELPERS::IntersectRayTri(Cases[i].vRaySrc, Cases[i].vRayNormalizedDir,
			Tri[0], Tri[1], Tri[2], &U, &V, &fDist);
		_ASSERTE(bRet == Cases[i].bIntersectTri);
		if (bRet)
		{
			_ASSERTE(KG3D_CUSTOM_HELPERS::IsUVWithinTri(U,V));
			_ASSERTE(MT_FLOAT_EQUAL(fDist, Cases[i].fDist));
		}
	}
}
KG_TEST_END(IntersectRayTri)
#endif
