#pragma once
#include "kg3drepresentobject.h"

class KG3DRepresentObjectVisibleEntitySet :
	public KG3DRepresentObject,
	public IEKG3DRepresentObjectVisibleEntitySet
{
public:

	DWORD   m_dwSetDepth;//集合的深度,嵌套层数越多越深
	DWORD   m_dwInsideLayerID;//集合内部的物件层ID
	DWORD   m_dwOutsideLayerID;//集合外部的物件层ID

private:
	BOOL IsPositionInSetDirection(D3DXVECTOR3& Position,D3DXVECTOR3& Direction);

public:
	virtual DWORD   GetInsideLayerID()
	{
		return m_dwInsideLayerID;
	}
	virtual DWORD   GetOutsideLayerID()
	{
		return m_dwOutsideLayerID;
	}
	virtual HRESULT SetInsideLayerID(DWORD dwID)
	{
		m_dwInsideLayerID = dwID;
		return S_OK;
	}
	virtual HRESULT SetOutsideLayerID(DWORD dwID)
	{
		m_dwOutsideLayerID = dwID;
		return S_OK;
	}
	virtual DWORD   GetSetDepth()
	{
		return m_dwSetDepth;
	}
	virtual HRESULT SetSetDepth(DWORD dwDepth)
	{
		m_dwSetDepth = dwDepth;
		return S_OK;
	}
	virtual HRESULT RenderShadowMap(LPD3DXEFFECT &lpEffect,const D3DXMATRIX &matVP,const D3DXMATRIX &matWVP);
	virtual BOOL GetRayIntersect(DWORD& dwCount,D3DXVECTOR3* pInterPoint,DWORD dwMaxNum,D3DXVECTOR3 Origin,D3DXVECTOR3 Direction);

	virtual HRESULT Render(DWORD dwOption);

	HRESULT LoadEdgeModel(LPSTR pModelName);//加载内分界面模型

	BOOL    IsPositionInSet(D3DXVECTOR3& Position);
public:

	KG3DRepresentObjectVisibleEntitySet(void);
	virtual ~KG3DRepresentObjectVisibleEntitySet(void);
};

extern BOOL g_bHideVisibleSetEdgeMesh;