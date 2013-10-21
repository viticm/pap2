////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DGraphicsFunctions.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-12-31 10:37:16
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG3DGraphicsFunctions.h"
#include "IEKG3DSceneOutputWnd.h"
#include "IEKG3DCamera.h"
#include "KG3DCamera.h"
#include "KG3DFrustum.h"

////////////////////////////////////////////////////////////////////////////////


HRESULT ComputeFrustrumBySelectionRect( RECT rectSel, IEKG3DSceneOutputWnd& wndOut, KG3DFrustum& frustum )
{
	HRESULT hr = E_FAIL;

	const D3DVIEWPORT9* pViewPort = wndOut.GetViewPort();
	KG_PROCESS_ERROR(NULL != pViewPort);

	IEKG3DCamera* pCamera = wndOut.GetIECamera();
	KG_PROCESS_ERROR(NULL != pCamera);
	{
		D3DXMATRIX matInv;

		//计算屏幕坐标的整个转换流程的矩阵的逆矩阵
		{
			D3DXMATRIX matMatrixViewPort;

			::D3DXMatrixViewPort(*pViewPort, matMatrixViewPort);

			pCamera->ComputeMatrix();
			D3DXMATRIX matView, matProj;
			hr = pCamera->GetMatrix(&matView, &matProj);				
			KG_COM_PROCESS_ERROR(hr);

			D3DXMATRIX matViewProjViewport = matView * matProj * matMatrixViewPort;
			D3DXMATRIX* pRet = D3DXMatrixInverse(&matInv, NULL, &matViewProjViewport);
			KG_PROCESS_ERROR(NULL != pRet);
		}


		FLOAT fMaxX = (FLOAT)max(rectSel.left, rectSel.right);
		FLOAT fMinX = (FLOAT)min(rectSel.left, rectSel.right);
		FLOAT fMaxY = (FLOAT)max(rectSel.top, rectSel.bottom);
		FLOAT fMinY = (FLOAT)min(rectSel.top, rectSel.bottom);

		FLOAT fZ = pViewPort->MinZ;

		D3DXVECTOR3 vTopLeft(fMinX, fMinY, fZ);
		D3DXVECTOR3 vTopRight(fMaxX, fMinY, fZ);
		D3DXVECTOR3 vBottomLeft(fMinX, fMaxY, fZ);
		D3DXVECTOR3 vBottomRight(fMaxX, fMaxY, fZ);

		D3DXVec3TransformCoord(&vTopLeft, &vTopLeft, &matInv);
		D3DXVec3TransformCoord(&vTopRight, &vTopRight, &matInv);
		D3DXVec3TransformCoord(&vBottomLeft, &vBottomLeft, &matInv);
		D3DXVec3TransformCoord(&vBottomRight, &vBottomRight, &matInv);

		_KG3D_DEBUG_SUPPERSUB_POINTER_EQUAL(IEKG3DCamera, KG3DCamera);	//如果这里失败的话，检查所有KG3DCamera到IEKG3DCamera的强转，他们全部都失败了
		hr = frustum.CreateByPointGate(*static_cast<KG3DCamera*>(pCamera)
			,vBottomLeft, vBottomRight, vTopLeft, vTopRight);
		KG_COM_PROCESS_ERROR(hr);

		return S_OK;
	}
Exit0:
	return E_FAIL;
}