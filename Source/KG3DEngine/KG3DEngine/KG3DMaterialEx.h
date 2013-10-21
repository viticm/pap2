#pragma once
#include "kg3dmaterial.h"


class KG3DMaterialEx :
	public KG3DMaterial
{
public:
	KG3DMaterialEx(void);
	virtual ~KG3DMaterialEx(void);
	HRESULT GetSubMaterial(unsigned int nIndex, KG3DMaterialSubset** pMaterial);
};

class KG3DMaterial_Helper_AutoMapFinder
{
public:
	static HRESULT FindDiffuseMapForEffect(KG3DMaterial* pMaterial, int nIndex, TCHAR* strRetFileName);
	static HRESULT FindSpcularMapForEffect(KG3DMaterial* pMaterial, int nIndex, TCHAR* strRetFileName);
	static HRESULT FindAmbientMapForEffect(KG3DMaterial* pMaterial, int nIndex, TCHAR* strRetFileName);
	static HRESULT FindNormalMapForEffect(KG3DMaterial* pMaterial,  int nIndex,	TCHAR* strRetFileName);
};
