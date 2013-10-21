////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DMathFunctions.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-9-23 14:26:09
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DMATHFUNCTIONS_H_
#define _INCLUDE_KG3DMATHFUNCTIONS_H_

////////////////////////////////////////////////////////////////////////////////
namespace KG3D_CUSTOM_HELPERS
{
	BOOL IntersectRayTri(const D3DXVECTOR3& vRaySrc, const D3DXVECTOR3& vRayNormalizedDir
						, D3DXVECTOR3& p0, D3DXVECTOR3& p1, D3DXVECTOR3& p2
						, FLOAT* pU, FLOAT* pV, FLOAT* pDist);//U,V是三角坐标，pDist是交点距离,如果反向没有交点
	BOOL IsUVWithinTri(FLOAT U, FLOAT V);
};

#endif //_INCLUDE_KG3DMATHFUNCTIONS_H_
