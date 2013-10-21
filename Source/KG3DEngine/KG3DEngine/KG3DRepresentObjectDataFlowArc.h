#pragma once
#include "kg3drepresentobject.h"

class KG3DDataFlowArc;
class KG3DSceneDataFlowEditor;

class KG3DRepresentObjectDataFlowArc :
	public KG3DRepresentObject
{
protected:
	KG3DDataFlowArc* m_lpArc;

	vector<D3DXVECTOR3>m_vecInputNode;
	vector<D3DXVECTOR3>m_vecOutputNode;
	D3DXVECTOR3 m_vCenterA,m_vCenterB;

public:

	HRESULT SetArc(KG3DDataFlowArc* pArc)
	{
		m_lpArc = pArc;
		return S_OK;
	}
	HRESULT Refresh(KG3DSceneDataFlowEditor* pEditor);

	virtual HRESULT Render(DWORD dwOption);

	KG3DRepresentObjectDataFlowArc(void);
	virtual ~KG3DRepresentObjectDataFlowArc(void);
};
