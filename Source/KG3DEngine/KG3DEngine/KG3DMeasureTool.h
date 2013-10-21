#pragma once

class KG3DCamera;

class KG3DMeasureTool
{
private:
	enum MeasureType
	{
		MEASEURE_POINT = 0,
		MEASEURE_LINE = 1,
	};

	MeasureType m_eType;

public:
	D3DXVECTOR3 m_vPositions[8];

	HRESULT Render(KG3DCamera* pCamera,D3DVIEWPORT9& viewport);
	HRESULT CreateTwoPointLine(D3DXVECTOR3& vA,D3DXVECTOR3& vB);

	KG3DMeasureTool(void);
	virtual ~KG3DMeasureTool(void);
};
