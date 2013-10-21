#pragma once
#include "../../../Include/kg3dengine/kg3dinterface.h"
#include "KG3DTemplates.h"
#include "./KG3DMaterial.h"

class KG3DMaterialTable : 
	public TKG3DResourceManagerBase<KG3DMaterial>
{
public:
	KG3DMaterialTable(void);
	virtual ~KG3DMaterialTable(void);
	virtual HRESULT Init();
};

extern KG3DMaterialTable g_cMaterialTable;


