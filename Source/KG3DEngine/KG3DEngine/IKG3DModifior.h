#pragma once
#include "../../../Include/kg3dengine/kg3dinterface.h"


class IKG3DModifior
{
public:
	IKG3DModifior(void);
	virtual ~IKG3DModifior(void);

	virtual HRESULT Apply();
	virtual HRESULT UnApply();
};

