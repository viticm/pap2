#pragma once

class KG3DDataFlowArc;

class KG3DDataFlowArcTable
{
protected:
	HRESULT UnInit();
	map<UINT,KG3DDataFlowArc*>m_mapArc;
	UINT m_uCurrentHandle;
	UINT GetCurrentHandle()
	{
		return m_uCurrentHandle++;
	}
public:
	HRESULT FindArc(KG3DDataFlowArc** ppArc,UINT uHandle);
	HRESULT ReleaseArc(UINT uHandle);
	HRESULT NewOneArc(UINT* puHandle);
	HRESULT GetAllArc(vector<UINT>* pvecHandle);

public:
	KG3DDataFlowArcTable(void);
	virtual ~KG3DDataFlowArcTable(void);
};
