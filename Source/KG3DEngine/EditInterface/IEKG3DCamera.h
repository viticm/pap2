////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEKG3DCamera.h
//  Version     : 1.0
//  Creator     : Zhao Xinyu
//  Create Date : 2008-8-14 10:32:23
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IEKG3DCAMERA_H_
#define _INCLUDE_IEKG3DCAMERA_H_
#include "KG3DInterface.h"
////////////////////////////////////////////////////////////////////////////////
struct KG3DCameraPersepectiveData
{
	float fFovy;
	float fAspect;
	float fzNear;
	float fzFar;
	KG3DCameraPersepectiveData()
	{
		fFovy = D3DX_PI / 6;
		fAspect = 1.0F;
		fzNear = 1.0;
		fzFar = 25000;
	}
};
struct KG3DCameraOrthogonalData	//用非透视矩阵的时候用这个 
{
	float fWidth;
	float fHeight;
	float fzNear;
	float fzFar;
	KG3DCameraOrthogonalData()
	{
		fWidth = 800;
		fHeight = 600;
		fzNear = 10;
		fzFar = 10000;
	}
};

//方法已经在基类全部定义过了
interface IEKG3DCamera : public IKG3DCamera
{
	virtual HRESULT ComputeMatrix() = 0;
	virtual HRESULT GetMatrix(D3DXMATRIX* pMatrixView, D3DXMATRIX* pMatrixProjection) = 0;//可以任意取其中一个或者两个
};

#endif //_INCLUDE_IEKG3DCAMERA_H_
