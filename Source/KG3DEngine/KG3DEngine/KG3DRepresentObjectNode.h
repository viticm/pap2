#pragma once
#include "kg3drepresentobject.h"
class KG3DRepresentObjectWall;

class KG3DRepresentObjectNode ://节点，用于道路，河流等的编辑修改
	public KG3DRepresentObject
{
private:
	float m_fNodeRadius;
	DWORD m_dwNodeType;
	PVOID m_pPointer;
	//KG3DTerrainRoad* m_lpTerrainRoad;   //指向此节点所属的路
	//KG3DTerrainRoadNode* m_lpTerrainRoadNode;

	KG3DRepresentObjectWall* m_lpWall;
	int m_nWallIndex;
	int m_nPointIndex;

public:
	virtual HRESULT Copy(KG3DRepresentObject* pSrcObject);

	virtual HRESULT Render(DWORD dwOption);
	virtual BOOL IsRayIntersect(D3DXVECTOR3&Inter,D3DXVECTOR3 Origin,D3DXVECTOR3 Direction);
	virtual HRESULT ComputeBBox();
	virtual HRESULT ComputeDynamicBBox();
	virtual HRESULT SetTranslation(const D3DXVECTOR3* pValue);
	virtual HRESULT SetScaling(const D3DXVECTOR3* pScale);
	virtual HRESULT OnDeleteFromScene(KG3DSceneSceneEditor* pEditor);
	HRESULT Refresh();
	void    SetNodeType(DWORD dwType);
	DWORD   GetNodeType();
	HRESULT SetPointer(PVOID pPointer);
	HRESULT GetPointer(PVOID* ppPointer);

	HRESULT SetWall(KG3DRepresentObjectWall* pWall,int nWallIndex,int nPointIndex);
	HRESULT GetWall(KG3DRepresentObjectWall*& pWall,int& nWallIndex,int& nPointIndex);

public:
	KG3DRepresentObjectNode(void);
	virtual ~KG3DRepresentObjectNode(void);
};
