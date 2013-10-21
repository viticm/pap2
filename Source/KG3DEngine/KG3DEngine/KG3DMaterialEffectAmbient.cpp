#include "stdafx.h"
#include "KG3DMaterialEffectAmbient.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

KG3DMaterialEffectAmbient::KG3DMaterialEffectAmbient()
{

}

KG3DMaterialEffectAmbient::~KG3DMaterialEffectAmbient()
{

}

//HRESULT KG3DMaterialEffectAmbient::SetParametersToMaterial(KG3DMaterial* pMaterial)
//{
//	HRESULT hr = E_FAIL;
//
//	KG_PROCESS_ERROR(pMaterial);
//
//	hr = AutoMapSetter(pMaterial);
//
//	KGLOG_COM_PROCESS_ERROR(hr);
//Exit0:
//	return hr;
//}
//
//HRESULT KG3DMaterialEffectAmbient::AutoMapSetter(KG3DMaterial* pMaterial)
//{
//	HRESULT hr = E_FAIL;
//	TCHAR strDrive[MAX_PATH];
//	TCHAR strPath[MAX_PATH];
//	TCHAR strFileName[MAX_PATH];
//	TCHAR strAmbientMap[MAX_PATH];
//
//	KG3DTexture* pTexture = NULL;
//
//	KG_PROCESS_ERROR(pMaterial);
//	
//	_splitpath(pMaterial->m_scName.c_str(), 
//		strDrive, 
//		strPath,
//		strFileName,
//		NULL);
//
//	sprintf(strAmbientMap, 
//		"%s%s%s.tga", 
//		strDrive, 
//		strPath,
//		strFileName);
//	
//	hr = g_cTextureTable.LoadResourceFromFile(strFileName,
//		0,
//		0,
//		(IKG3DResourceBase**)&pTexture);
//
//	KGLOG_COM_PROCESS_ERROR(hr);
//	
//	for(DWORD i = 0; i < pMaterial->m_dwNumMaterials; i++)
//	{
//		pMaterial->m_pMaterials[i].m_scTextureName[KG3DMaterial::TEX_MAP_AMBIENT] = strFileName;
//		pMaterial->m_pMaterials[i].m_scTextureName[KG3DMaterial::TEX_MAP_AMBIENT] = TEXTURE_TYPE_AMBIENT;
//		pMaterial->m_pMaterials[i].m_dwTextureIDs[KG3DMaterial::TEX_MAP_AMBIENT] = pTexture->GetID();
//	}
//
//	
//Exit0:
//	return hr;
//}
//
//HRESULT KG3DMaterialEffectAmbient::SetSubsetAmbient()