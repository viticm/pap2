#pragma once
#include "../../../Include/kg3dengine/kg3dinterface.h"
#include "KG3DTransformation.h"
#include "kg3dstructs.h"

class KG3DModel;

using namespace std;
class KG3DEditGridBase : public KG3DTransformation
{
protected:
	struct _Grid
	{
		KG3DModel* pModel;
		DWORD          dwState;
		_Grid(KG3DModel* pM,DWORD dwS)
		{
			pModel = pM;
			dwState = dwS;
		}
	};
	vector<_Grid>m_vecGrid;
	DWORD m_dwState;

public:
	KG3DEditGridBase(void);
	virtual ~KG3DEditGridBase(void);

        DWORD GetState(){return m_dwState;}
	HRESULT CheckState(D3DXVECTOR3 Origin,D3DXVECTOR3 Direction);
	HRESULT OpenGridModel(LPSTR pFileName,DWORD dwState);
	virtual HRESULT Render();
	HRESULT UnInit();

};
