#include "StdAfx.h"
#include "KG3DTextureTable.h"
#include "kg3dmaterialex.h"



#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


KG3DMaterialEx::KG3DMaterialEx(void)
{
}

KG3DMaterialEx::~KG3DMaterialEx(void)
{
}

HRESULT KG3DMaterialEx::GetSubMaterial(unsigned int nIndex, KG3DMaterialSubset** pMaterial)
{
	HRESULT hr = E_FAIL;
	if(nIndex < m_dwNumMaterials)
	{
		*pMaterial = KG3DMaterial::GetSubMaterial(nIndex);
		hr = S_OK;
	}
	return hr;
}


HRESULT KG3DMaterial_Helper_AutoMapFinder::FindNormalMapForEffect(KG3DMaterial* pMaterial,
																   int nIndex, 
																   TCHAR* strRetFileName)
{
	HRESULT hr = E_FAIL;
	DWORD dwNumMaterial = 0;
	KG3DMaterial::KG3DMaterialSubset* pSubMaterial = NULL;
	KG3DTexture* pTexture = NULL;

	KG_PROCESS_ERROR(nIndex >= 0);
	KG_PROCESS_ERROR(pMaterial);
	dwNumMaterial = pMaterial->GetNumMaterial();
	ASSERT((int)dwNumMaterial > nIndex);

	pMaterial->SetShaderType(KG3DMaterial::MAT_SHADER_TYPE_NORMAL_NOSKIN);
	pSubMaterial = pMaterial->GetSubMaterial(nIndex);
	KGLOG_PROCESS_ERROR(pSubMaterial);

	pTexture = pSubMaterial->m_lpTextures[2];
	KG_PROCESS_ERROR(NULL != pTexture);

	strcpy(strRetFileName, pTexture->m_scName.c_str());

	pSubMaterial->m_scTextureType[2] = "*MAP_NORMAL";
	
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DMaterial_Helper_AutoMapFinder::FindDiffuseMapForEffect(KG3DMaterial* pMaterial,
																   int nIndex, 
																   TCHAR* strRetFileName)
{
	HRESULT hr = E_FAIL;
	DWORD dwNumMaterial = 0;
	KG3DMaterial::KG3DMaterialSubset* pSubMaterial = NULL;
	KG3DTexture* pTexture = NULL;
	
	KG_PROCESS_ERROR(nIndex >= 0);
	KG_PROCESS_ERROR(pMaterial);
	dwNumMaterial = pMaterial->GetNumMaterial();
	ASSERT((int)dwNumMaterial > nIndex);
	
	pSubMaterial = pMaterial->GetSubMaterial(nIndex);
	KGLOG_PROCESS_ERROR(pSubMaterial);
	
	pTexture = pSubMaterial->m_lpTextures[0];
	KG_PROCESS_ERROR(NULL != pTexture);

	strcpy(strRetFileName, pTexture->m_scName.c_str());
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DMaterial_Helper_AutoMapFinder::FindSpcularMapForEffect(KG3DMaterial* pMaterial, int nIndex, TCHAR* strRetFileName)
{
	HRESULT hr = E_FAIL;
	DWORD dwNumMaterial = 0;
	KG3DMaterial::KG3DMaterialSubset* pSubMaterial = NULL;
	KG3DTexture* pTexture = NULL;
	size_t uFileNameLength = 0;
	KG_PROCESS_ERROR(pMaterial);

	dwNumMaterial = pMaterial->GetNumMaterial();
	KG_PROCESS_ERROR(static_cast<int>(dwNumMaterial) > nIndex);
	KG_PROCESS_ERROR(nIndex >= 0);

	pSubMaterial = pMaterial->GetSubMaterial(nIndex);
	KGLOG_PROCESS_ERROR(pSubMaterial);

	pTexture = pSubMaterial->m_lpTextures[0];
	KG_PROCESS_ERROR(NULL != pTexture);

	strcpy(strRetFileName, pTexture->m_scName.c_str());
	uFileNameLength = strlen(strRetFileName);
	while(uFileNameLength && strRetFileName[uFileNameLength] != '.')
		uFileNameLength--;
	if(uFileNameLength != 0)
	{
		strRetFileName[uFileNameLength] = '\0';
		strcat(strRetFileName, "_sh.tga");
	}
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DMaterial_Helper_AutoMapFinder::FindAmbientMapForEffect(KG3DMaterial* pMaterial, int nIndex, TCHAR* strRetFileName)
{
	HRESULT hr = E_FAIL;
	KG3DTexture* pTexture = NULL;
	TCHAR strDrive[MAX_PATH];
	TCHAR strPath[MAX_PATH];
	TCHAR strFileName[MAX_PATH];
	TCHAR strFileExt[MAX_PATH];
	TCHAR* strTexturePath = NULL;
	KG_PROCESS_ERROR(pMaterial);

	_splitpath(pMaterial->m_scName.c_str(), strDrive, strPath, strFileName, strFileExt);
	strRetFileName[0] = '\0';
	for(int i = 0; i < (int)pMaterial->m_dwNumMaterials; i++)
	{
		KG3DMaterial::KG3DMaterialSubset* pSubMtl = pMaterial->GetSubMaterial(i);
		if(!pSubMtl)
			continue;

		for(int j = 0; j < 8; j++)
		{
			pTexture = pSubMtl->m_lpTextures[j];
			if(pTexture)
			{
				strcpy(strRetFileName, pTexture->m_scName.c_str());
				_strlwr(strRetFileName);
				strTexturePath = strstr(strRetFileName, "\\texture\\");
				
				if(!strTexturePath)
				{
					//如果没有Texture这个目录在贴图路径，就认为所有的文件放在一起的
					sprintf(strRetFileName, "%s%s%s.tga", strDrive, strPath, strFileName);
				}
				else
				{
					strTexturePath[strlen("\\texture\\")] = '\0';
					strcat(strRetFileName, "光照图\\");
					strcat(strRetFileName, strFileName);
					strcat(strRetFileName, ".tga");
				}
				i = pMaterial->m_dwNumMaterials;
				break;
			}
		}
	}
	hr = S_OK;
Exit0:
	return hr;
}