#include "StdAfx.h"
#include "KG3DRepresentObjectVisibleEntitySet.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

BOOL g_bHideVisibleSetEdgeMesh = FALSE;

KG3DRepresentObjectVisibleEntitySet::KG3DRepresentObjectVisibleEntitySet(void)
{
	m_dwSetDepth = 0;
	m_dwInsideLayerID = 0;
	m_dwOutsideLayerID = 0;
}

KG3DRepresentObjectVisibleEntitySet::~KG3DRepresentObjectVisibleEntitySet(void)
{
}

HRESULT KG3DRepresentObjectVisibleEntitySet::LoadEdgeModel(LPSTR pModelName)
{
	HRESULT hr = SetModel(0,pModelName,"","",TRUE);
	return hr;
}

HRESULT KG3DRepresentObjectVisibleEntitySet::Render(DWORD dwOption)
{
	
	if(!g_bHideVisibleSetEdgeMesh)
	{
		for (DWORD i = 0; i < GetModelCount(); i++)
		{
			KG3DModel* pModel = GetModel(i);
			if(NULL != pModel)
			{
				pModel->Render(0,NULL);
			}
		}
	}

	return S_OK;
}

BOOL KG3DRepresentObjectVisibleEntitySet::IsPositionInSetDirection(D3DXVECTOR3& Position,D3DXVECTOR3& Direction)
{
	D3DXVECTOR3 Inter[256];
	DWORD dwCount = 0;
	BOOL bHit = FALSE;
	BOOL bUp = FALSE;
	BOOL bDown = FALSE;

	bHit = KG3DRepresentObject::GetRayIntersect(dwCount,Inter,256,Position,Direction);
	if (!bHit)
		return FALSE;
	for(DWORD i=0;i<dwCount;i++)
	{
		D3DXVECTOR3 V = Inter[i] - Position;
		float fdot = D3DXVec3Dot(&V,&Direction);
		if(fdot>=0)
			bUp = TRUE;
		else
			bDown = TRUE;
	}
	if ((bUp)&&(bDown))
		return TRUE;
	return FALSE;
}

BOOL KG3DRepresentObjectVisibleEntitySet::IsPositionInSet(D3DXVECTOR3& Position)
{
	D3DXVECTOR3 Direction;

	Direction = D3DXVECTOR3(0,1,0);
	BOOL bY = IsPositionInSetDirection(Position,Direction);
	Direction = D3DXVECTOR3(1,0,0);
	BOOL bX = IsPositionInSetDirection(Position,Direction);
	Direction = D3DXVECTOR3(0,0,1);
	BOOL bZ = IsPositionInSetDirection(Position,Direction);

	if ((bX)&&(bY)&&(bZ))
		return TRUE;

	return FALSE;
}

BOOL KG3DRepresentObjectVisibleEntitySet::GetRayIntersect(DWORD& dwCount,D3DXVECTOR3* pInterPoint,DWORD dwMaxNum,D3DXVECTOR3 Origin,D3DXVECTOR3 Direction)
{
	if(g_bHideVisibleSetEdgeMesh)
	{
		return FALSE;
	}
	else
	{
		return KG3DRepresentObject::GetRayIntersect(dwCount,pInterPoint,dwMaxNum,Origin,Direction);
	}
}

HRESULT KG3DRepresentObjectVisibleEntitySet::RenderShadowMap(LPD3DXEFFECT &lpEffect,const D3DXMATRIX &matVP,const D3DXMATRIX &matWVP)
{
	return S_OK;
}