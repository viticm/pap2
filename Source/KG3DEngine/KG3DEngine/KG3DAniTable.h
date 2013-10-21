#pragma once
#include "kg3dtemplates.h"
#include "KG3DAni.h"

class KG3DAniTable :
	public TKG3DResourceManagerBase<KG3DAni>
{
public:
	KG3DAniTable(void);
	~KG3DAniTable(void);
	virtual HRESULT Init();
};

extern KG3DAniTable g_cAni2Table;
