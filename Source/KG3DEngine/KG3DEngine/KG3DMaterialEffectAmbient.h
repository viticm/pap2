#pragma once
#include "KG3DMaterialEffectSetter.h"

class KG3DMaterialEffectAmbient: public KG3DMaterialEffectSetter
{
public:
	KG3DMaterialEffectAmbient();
	virtual ~KG3DMaterialEffectAmbient();
	//virtual HRESULT SetParametersToMaterial(KG3DMaterial* pMaterial);
};