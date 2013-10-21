////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DRepresentObjectTrans.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-10-27 11:19:45
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG3DRepresentObjectTrans.h"

struct KG3DRepresentObjectTransformVisitorSolid : public KG3DRepresentObjectTransformVisitor 
{
	D3DXMATRIX m_matTrans;
	KG3DRepresentObjectTransformVisitorSolid()
	{
		D3DXMatrixIdentity(&m_matTrans);
	}
	virtual HRESULT Accept(KG3DRepresentObject& Obj) = 0;
	virtual	HRESULT SetMatrix(const D3DXMATRIX& matTrans)
	{
		m_matTrans = matTrans;
		return S_OK;
	}
};

struct KG3DRepresentObjectRotationVisitorSolid : public KG3DRepresentObjectTransformVisitorSolid
{
	virtual HRESULT Accept(KG3DRepresentObject& Obj)
	{
		const D3DXMATRIX& LocalMatrix = Obj.GetMatrix();
		D3DXMATRIX LocalMatScaleRotation = LocalMatrix;
		
		/*
		只要改Local矩阵就可以了，World矩阵会在每帧更新的。Parent的部分让它自己乘就好了
		而Local矩阵只需要提取出Translation的部分，然后旋转，然后再把Translation的部分放回去，参考
		《3D数学基础:图形与游戏开发》的P124

		相当于 LocalMat * (T^-1) * R * (T)
		*/

		//把移动的部分去掉，并记录下来
		D3DXVECTOR3& vecTransPart = D3DXMatrixGetTranslationPart(LocalMatScaleRotation);
		D3DXVECTOR3 vecTrans = vecTransPart;
		vecTransPart = D3DXVECTOR3(0,0,0);

		/*现在LocalMatScaleRotation只剩下原来的Scale和Rotation的部分了
		相当于LoalMatScaleRotation * (T^-1)*/

		LocalMatScaleRotation *= m_matTrans;	//旋转
		vecTransPart = vecTrans;				//然后再次把变换的部分覆盖回去，这样子比用逆矩阵快多了

		Obj.SetMatrix(LocalMatScaleRotation);

		Obj.m_bSleep = FALSE;
		Obj.FrameMove();//<SLEEP_REFRACTOR>

		return S_OK;
	}
};

KG3DRepresentObjectTransformVisitor& g_GetRepresentObjectRotationVisitor()
{
	static KG3DRepresentObjectRotationVisitorSolid stInstance;
	return stInstance;
}
////////////////////////////////////////////////////////////////////////////////

struct KG3DRepresentObjectTranslationVisitorSolid : public KG3DRepresentObjectTransformVisitorSolid
{
	virtual HRESULT Accept(KG3DRepresentObject& Obj)
	{
		const D3DXMATRIX& LocalMatrix = Obj.GetMatrix();
		D3DXMATRIX LocalMatScaleRotation = LocalMatrix * m_matTrans;		

		Obj.SetMatrix(LocalMatScaleRotation);

		Obj.m_bSleep = FALSE;
		Obj.FrameMove();//<SLEEP_REFRACTOR>

		return S_OK;
	}
};

KG3DRepresentObjectTransformVisitor& g_GetRepresentObjectTranslationVisitor()
{
	static 	KG3DRepresentObjectTranslationVisitorSolid stInstance;
	return stInstance;
}
//////////////////////////////////////////////////////////////////////////
struct KG3DRepresentObjectScalingVisitorSolid : public KG3DRepresentObjectTransformVisitorSolid
{
	virtual HRESULT Accept(KG3DRepresentObject& Obj)
	{
		const D3DXMATRIX& LocalMatrix = Obj.GetMatrix();
		D3DXMATRIX LocalMatScaleRotation = LocalMatrix;

		//把移动的部分去掉，并记录下来
		D3DXVECTOR3& vecTransPart = D3DXMatrixGetTranslationPart(LocalMatScaleRotation);
		D3DXVECTOR3 vecTrans = vecTransPart;
		vecTransPart = D3DXVECTOR3(0,0,0);

		LocalMatScaleRotation = m_matTrans * LocalMatScaleRotation;	//旋转
		D3DXMatrixGetTranslationPart(LocalMatScaleRotation) = vecTrans;		//然后再次把变换的部分覆盖回去，这样子比用逆矩阵快多了

		Obj.SetMatrix(LocalMatScaleRotation);

		Obj.m_bSleep = FALSE;
		Obj.FrameMove();//<SLEEP_REFRACTOR>
		return S_OK;
	}
};

KG3DRepresentObjectTransformVisitor& g_GetRepresentObjectScalingVisitor()
{
	static 	KG3DRepresentObjectScalingVisitorSolid stInstance;
	return stInstance;
}

struct KG3DRepresentObjectTransVisitorSolid : public KG3DRepresentObjectTransformVisitorSolid
{
	virtual HRESULT Accept(KG3DRepresentObject& Obj)
	{
		const D3DXMATRIX& matLocal = Obj.GetMatrix();
		
		D3DXMATRIX matTrans = matLocal * m_matTrans;

		Obj.SetMatrix(matTrans);

		Obj.m_bSleep = FALSE;
		Obj.FrameMove();//<SLEEP_REFRACTOR>

		return S_OK;
	}
};

KG3DRepresentObjectTransformVisitor& g_GetRepresentObjectTransVisitor()
{
	static KG3DRepresentObjectTransVisitorSolid s_Instance;
	return s_Instance;
}