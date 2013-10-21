#pragma once
#include "kg3dexportnodeselector.h"

class KG3DObject :
	public KG3DExportNodeSelector
{
public:
	KG3DObject(Interface* pIP);
	~KG3DObject(void);
};
