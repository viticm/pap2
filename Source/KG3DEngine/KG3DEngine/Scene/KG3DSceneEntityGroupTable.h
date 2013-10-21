#pragma once

#include "../../../Include/kg3dengine/kg3dinterface.h"
#include "KG3DTemplates.h"
#include "KG3DSceneEntityGroup.h"

class KG3DSceneEntityGroupTable : public
	TKG3DResourceManagerBase<KG3DSceneEntityGroup>
{
public:
	KG3DSceneEntityGroupTable(void);
public:
	virtual ~KG3DSceneEntityGroupTable(void);
};

extern KG3DSceneEntityGroupTable g_cSceneEntityGroupTable;

