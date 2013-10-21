////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DCullerTop.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-8-14 10:20:00
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG3DCullerTop.h"
#include "IEKG3DCamera.h"
#include "KG3DRepresentObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


BOOL KG3DCullerTopOrthogonal::Cull( KG3DRepresentObject& Obj )
{
	return m_Box.IsTwoBoxIntersect(Obj.GetBBox());	
}


KG3DCullerTopOrthogonal::KG3DCullerTopOrthogonal( const KG3DCameraOrthogonalData& Data,  const D3DXVECTOR3& CamPos )
{
	FLOAT ZHalf = abs(Data.fHeight)/2;
	FLOAT XHalf = abs(Data.fWidth)/2;
	m_Box.A = CamPos + D3DXVECTOR3(-XHalf, -abs(Data.fzFar), -ZHalf);
	m_Box.B = CamPos + D3DXVECTOR3(XHalf, -abs(Data.fzNear), ZHalf);
}

#if defined(KG_ENABLE_TDD)
KG_TEST_BEGIN(KG3DCullerTopOrthogonal)
{
	struct TestCuller : public KG3DCullerTopOrthogonal
	{
		TestCuller(const KG3DCameraOrthogonalData& Data, const D3DXVECTOR3& CamPos)
			:KG3DCullerTopOrthogonal(Data, CamPos){}
		using KG3DCullerTopOrthogonal::m_Box;
	};
	KG3DCameraOrthogonalData Data;
	Data.fzNear = 10;
	Data.fzFar = 1000;
	Data.fWidth = 64000;
	Data.fHeight = 6400;
	D3DXVECTOR3 vCamPos(64000/2, 1000, 6400/2);
	TestCuller Culler(Data, vCamPos);
	_ASSERTE(Culler.m_Box == BBox(D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(6400/2, 990, 6400/2)));
}
KG_TEST_END(KG3DCullerTopOrthogonal)
#endif