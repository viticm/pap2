#pragma once

#include "../../../Include/kg3dengine/kg3dinterface.h"
#include "KG3DTemplates.h"
#include "KG3DScene.h"

class KG3DSceneTable :
	public TKG3DResourceManagerBase<KG3DScene>
{
private:
	HRESULT Get1NewResourcePointer(KG3DScene** ppT,DWORD Type,DWORD_PTR Option);
public:
	HRESULT AutoReload();

	KG3DSceneTable(void);
	virtual ~KG3DSceneTable(void);
};


extern KG3DSceneTable g_cSceneTable;