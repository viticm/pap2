#include "StdAfx.h"
#include "KG3DTerrainDetailTexture.h"
#include "KG3DTexture.h"
#include "KG3DTextureTable.h"
#include "KG3DMemoryFile.h"
#include "KG3DSceneOutDoorSpaceMgr.h"
#include "KG3DFileFinder.h"
#include "KG3DEngineManager.h"
#include "KG3DTerrainRepresentInfo.h"
#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void KG3DTerrainDetailTexture::ComputeUVTransformation()
{
	D3DXVECTOR3 U(1,0,0);
	D3DXVECTOR3 V(0,0,1);

	switch(m_byTexCoordIndex)
	{
	case 0://xz平面
		U = D3DXVECTOR3(1,0,0);
		V = D3DXVECTOR3(0,0,1);
		break;
	case 1://xy平面的
		U = D3DXVECTOR3(1,0,0);
		V = D3DXVECTOR3(0,1,0);
		break;
	case 2://yz平面
		U = D3DXVECTOR3(0,1,0);
		V = D3DXVECTOR3(0,0,1);
		break;
	}

	D3DXVec3Normalize(&U,&U);
	D3DXVec3Normalize(&V,&V);
	U *= m_fScaleU * 0.01F;
	V *= m_fScaleV * 0.01F;
	m_vUT = D3DXVECTOR4(U.x,U.y,U.z,0);
	m_vVT = D3DXVECTOR4(V.x,V.y,V.z,0);
}

KG3DTerrainDetailTexture::KG3DTerrainDetailTexture()
{
	m_fScaleU = 0.25F;
	m_fScaleV = 0.25F;
	m_vUT = D3DXVECTOR4(0,0,0,0);
	m_vVT = D3DXVECTOR4(0,0,0,0);

	m_lpDiffuseTexture = NULL;
	m_lpNormalTexture  = NULL;
	m_byTexCoordIndex = 0;

	m_bAssociateGrass = FALSE;

	m_dwMaterialType = INVALID_GROUNDTYPE;
	m_fEnvReflection = 0;
	m_fSpecular = 0;
	m_fEmissive = 0;

	memset(m_byGrassTexIndex,255,sizeof(m_byGrassTexIndex));
	memset(m_byPatternIndex,255,sizeof(m_byPatternIndex));
	ComputeUVTransformation();
}

KG3DTerrainDetailTexture::~KG3DTerrainDetailTexture()
{
	SAFE_RELEASE(m_lpDiffuseTexture);
	SAFE_RELEASE(m_lpNormalTexture );
}

HRESULT KG3DTerrainDetailTexture::SaveToInformationBuffer(KG3DMemoryFile* pFile)
{
	DWORD dwVersion = 2;
	pFile->write(&dwVersion,sizeof(DWORD));

	pFile->write(&m_byteIndex,sizeof(BYTE));

	CHAR szFileName[MAX_PATH];
	wsprintf(szFileName,"%s",m_szDiffuseFileName.c_str());
	pFile->write(szFileName,sizeof(CHAR)*MAX_PATH);

	pFile->write(&m_byTexCoordIndex,sizeof(BYTE));

	pFile->write(&m_fScaleU,sizeof(float));

	pFile->write(&m_fScaleV,sizeof(float));

	pFile->write(&m_vUT,sizeof(D3DXVECTOR4));

	pFile->write(&m_vVT,sizeof(D3DXVECTOR4));
	//新添加的，用于编辑器，记录关联到那些草贴图和草模型//////////add by huangjinshou 2008-7-25
	pFile->write(&m_bAssociateGrass,sizeof(BOOL));
	pFile->write(m_byGrassTexIndex,sizeof(m_byGrassTexIndex));
	pFile->write(m_byPatternIndex,sizeof(m_byPatternIndex));
	//////////////////////////////////////////////////////////////新添加材质属性
	pFile->write(&m_fEnvReflection,sizeof(float));

	pFile->write(&m_fSpecular,sizeof(float));

	pFile->write(&m_fEmissive,sizeof(float));
	return S_OK;
}


HRESULT KG3DTerrainDetailTexture::LoadFromInformationBuffer(KG3DMemoryFile* pFile)
{
	DWORD dwVersion = 0;
	TCHAR strLower[MAX_PATH];
	pFile->read(&dwVersion,sizeof(DWORD));

	pFile->read(&m_byteIndex,sizeof(BYTE));

	CHAR szFileName[MAX_PATH];
	pFile->read(&szFileName,sizeof(CHAR)*MAX_PATH);
	m_szDiffuseFileName = szFileName;

	SAFE_RELEASE(m_lpDiffuseTexture);
	g_cTextureTable.LoadResourceFromFile(szFileName,0,0/*TEXOP_RELOAD*/,(IKG3DResourceBase**)&m_lpDiffuseTexture);

	//MaterialType是地表贴图文件的HASH, 信息存在g_GetTerrainRepresentInfo()中
	strcpy_s(strLower, MAX_PATH, szFileName);
	_strlwr_s(strLower, MAX_PATH);
	m_dwMaterialType = g_FileNameHash(strLower);


	if(g_cEngineOption.bEnableGroundNormalMap)
	{
        char szHDPath1[MAX_PATH];
        char szHDPath2[MAX_PATH];

        g_cEngineManager.GetHDFilePath(szHDPath1, szHDPath2, szFileName);

        if (*szHDPath1)
        {
            SAFE_RELEASE(m_lpNormalTexture );
            g_cTextureTable.LoadResourceFromFile(szHDPath1,0,0/*TEXOP_RELOAD*/,(IKG3DResourceBase**)&m_lpNormalTexture );
        }

		//TCHAR strFile[MAX_PATH]; 
		//TCHAR strDrive[MAX_PATH];
		//TCHAR strPath[MAX_PATH];
		//TCHAR strExt[MAX_PATH];
		//_splitpath_s(szFileName, 
		//	strDrive, 
		//	MAX_PATH,
		//	strPath, 
		//	MAX_PATH, 
		//	strFile, 
		//	MAX_PATH, 
		//	strExt,
		//	MAX_PATH);
		//CHAR szNormalName[MAX_PATH];
		//wsprintf(szNormalName,"%s%s%s_n%s",strDrive,strPath,strFile,strExt);
		//SAFE_RELEASE(m_lpNormalTexture );
		//g_cTextureTable.LoadResourceFromFile(szNormalName,0,0/*TEXOP_RELOAD*/,(IKG3DResourceBase**)&m_lpNormalTexture );
	}

	pFile->read(&m_byTexCoordIndex,sizeof(BYTE));

	pFile->read(&m_fScaleU,sizeof(float));

	pFile->read(&m_fScaleV,sizeof(float));

	pFile->read(&m_vUT,sizeof(D3DXVECTOR4));

	pFile->read(&m_vVT,sizeof(D3DXVECTOR4));
	//新添加的，用于编辑器，记录关联到那些草贴图和草模型//////////add by huangjinshou 2008-7-25
	if(dwVersion >= 1)
	{
		pFile->read(&m_bAssociateGrass,sizeof(BOOL));
		pFile->read(m_byGrassTexIndex,sizeof(m_byGrassTexIndex));
		pFile->read(m_byPatternIndex,sizeof(m_byPatternIndex));
	}
	//////////////////////////////////////////////////////////////
	if(dwVersion == 2)
	{
		pFile->read(&m_fEnvReflection,sizeof(float));

		pFile->read(&m_fSpecular,sizeof(float));
		
		pFile->read(&m_fEmissive,sizeof(float));
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT KG3DTerrainDetailMaterial::SaveToInformationBuffer(KG3DMemoryFile* pFile)
{
	DWORD dwVersion = 0;
	pFile->write(&dwVersion,sizeof(DWORD));

	BYTE size = (BYTE)m_vecTextures.size();
	pFile->write(&size,sizeof(BYTE));

	for (BYTE i=0;i<size;i++)
	{
		BYTE TextureIndex = m_vecTextures[i];
		pFile->write(&TextureIndex,sizeof(BYTE));
	}
	return S_OK;
}

HRESULT KG3DTerrainDetailMaterial::LoadFromInformationBuffer(KG3DMemoryFile* pFile)
{
	DWORD dwVersion = 0;
	pFile->read(&dwVersion,sizeof(DWORD));

	BYTE size = 0;
	pFile->read(&size,sizeof(BYTE));

	m_vecTextures.clear();

	for (BYTE i=0;i<size;i++)
	{
		BYTE TextureIndex = 0;
		pFile->read(&TextureIndex,sizeof(BYTE));
		m_vecTextures.push_back(TextureIndex);
	}
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
HRESULT KG3DTerrainDetailMaterialMgr::Init()
{
	return S_OK;
}

HRESULT KG3DTerrainDetailMaterialMgr::UnInit()
{
	/*for (int i=0;i<256;i++)
	{
		SAFE_DELETE(m_szDetailTexture[i]);
	}	*/
	map<BYTE,KG3DTerrainDetailTexture*>::iterator i = m_mapDetailTexture.begin();
	while (i != m_mapDetailTexture.end())
	{
		SAFE_DELETE(i->second);
		i++;
	}
	m_mapDetailTexture.clear();

	for (int i=0;i<256;i++)
	{
		SAFE_DELETE(m_szDetailMaterial[i]);
	}
	m_byteCurrentTextureIndex = 0;
	m_byteCurrentMaterialIndex = 0;
//	m_uHandle = 0;
	return S_OK;
}

KG3DTerrainDetailMaterialMgr::KG3DTerrainDetailMaterialMgr()
{
	m_uHandle = 0;
	m_byteCurrentTextureIndex = 0;
	m_byteCurrentMaterialIndex = 0;
	//ZeroMemory(m_szDetailTexture,sizeof(KG3DTerrainDetailTexture*)*256);
	ZeroMemory(m_szDetailMaterial,sizeof(KG3DTerrainDetailMaterial*)*256);
	m_lpOutDoorMgr = NULL;//initialize,added by huchangyin
}
KG3DTerrainDetailMaterialMgr::KG3DTerrainDetailMaterialMgr(KG3DSceneOutDoorSpaceMgr* lpOutDoorMgr)
{
	m_uHandle = 0;
	m_byteCurrentTextureIndex = 0;
	m_byteCurrentMaterialIndex = 0;
	ZeroMemory(m_szDetailMaterial,sizeof(KG3DTerrainDetailMaterial*)*256);
	m_lpOutDoorMgr = lpOutDoorMgr;
}
KG3DTerrainDetailMaterialMgr::~KG3DTerrainDetailMaterialMgr()
{
	UnInit();
}

BYTE KG3DTerrainDetailMaterialMgr::GetCurrentTextureIndex()
{
	//map<BYTE,KG3DTerrainDetailTexture*>::iterator iFind  = m_mapDetailTexture.begin();
	while(m_mapDetailTexture.find(m_byteCurrentTextureIndex) != m_mapDetailTexture.end() )
	{
		m_byteCurrentTextureIndex++;
	}
	return m_byteCurrentTextureIndex++;
}
BYTE KG3DTerrainDetailMaterialMgr::GetCurrentMaterialIndex()
{
	return m_byteCurrentMaterialIndex++;
}

HRESULT KG3DTerrainDetailMaterialMgr::AddDetailTexture(KG3DTerrainDetailTexture** ppTex,LPCTSTR pFileName)
{
	HRESULT hr = E_FAIL;
	BOOL bInsertToMap = FALSE;
	TCHAR szRelativePath[MAX_PATH];
	KG3DTerrainDetailTexture* pNew = NULL;
	pNew= new KG3DTerrainDetailTexture;
	KGLOG_PROCESS_ERROR(pNew);

	BYTE index = GetCurrentTextureIndex();
	KGLOG_PROCESS_ERROR( index < 255);

	pNew->m_byteIndex = index;

	//m_szDetailTexture[index] = pNew;//无论贴图有没有读取成功，都要放到数组里
	m_mapDetailTexture.insert(std::pair<BYTE,KG3DTerrainDetailTexture*>(index, pNew));
	//(*ppTex) = pNew;
	
	wsprintf(szRelativePath,"%s", GetRetPath(pFileName));

	pNew->m_szDiffuseFileName = szRelativePath;
	bInsertToMap = TRUE;
	hr = g_cTextureTable.LoadResourceFromFile(szRelativePath, 0, 0, 
		(IKG3DResourceBase **)&pNew->m_lpDiffuseTexture);
	KGLOG_COM_PROCESS_ERROR(hr);
	
	{
		if (m_lpOutDoorMgr && m_lpOutDoorMgr->GetSceneDataInterface() )
		{
			DWORD dwLength = 0;
			KG3DMemoryFile* pFile = g_cMemoryFilePool.RequestResource();
			if(pFile)
			{
				SaveToInformationBuffer(dwLength,pFile);
				m_lpOutDoorMgr->GetSceneDataInterface()->Request_ModifyTerrainInformation(m_uHandle,Terrain_Info_DetailMtlMgr,dwLength,pFile->GetBuffer());
				g_cMemoryFilePool.FreeResource(pFile);
				pFile = NULL;
			}
		}
	}
	(*ppTex) = m_mapDetailTexture[index];
	return S_OK;
Exit0:
	if(pNew)
		SAFE_DELETE(pNew);
	if (bInsertToMap)
	{
		map<BYTE,KG3DTerrainDetailTexture*>::iterator iFind = m_mapDetailTexture.find(index);
		if (iFind != m_mapDetailTexture.end())
		{
			m_mapDetailTexture.erase(iFind);
		}
	}
	return hr;
}
HRESULT KG3DTerrainDetailMaterialMgr::DelDetailTexture(BYTE byIndex)
{
	//ASSERT(byIndex<256);
	BOOL bUpdate = FALSE;
	map<BYTE,KG3DTerrainDetailTexture*>::iterator iFind = m_mapDetailTexture.find(byIndex);
	if(iFind != m_mapDetailTexture.end())
	{
		SAFE_DELETE(m_mapDetailTexture[byIndex]);
		m_mapDetailTexture.erase(iFind);
		bUpdate = TRUE;
	}
	
	if (bUpdate && m_lpOutDoorMgr && m_lpOutDoorMgr->GetSceneDataInterface() )
	{
		DWORD dwLength = 0;
		KG3DMemoryFile* pFile = g_cMemoryFilePool.RequestResource();
		if(pFile)
		{
			SaveToInformationBuffer(dwLength,pFile);
			m_lpOutDoorMgr->GetSceneDataInterface()->Request_ModifyTerrainInformation(m_uHandle,Terrain_Info_DetailMtlMgr,dwLength,pFile->GetBuffer());
			g_cMemoryFilePool.FreeResource(pFile);
			pFile = NULL;
		}
	}
	return S_OK;
}

KG3DTerrainDetailTexture* KG3DTerrainDetailMaterialMgr::GetDetailTexture(BYTE Index)
{
	if (m_mapDetailTexture.size() == 0)
		return NULL;
	map<BYTE,KG3DTerrainDetailTexture*>::iterator iFind = m_mapDetailTexture.find(Index);
	if(iFind != m_mapDetailTexture.end())
		return m_mapDetailTexture[Index];
	else
		return m_mapDetailTexture[0];
}

HRESULT KG3DTerrainDetailMaterialMgr::Copy(KG3DTerrainDetailMaterialMgr* pSrcMgr) // Modify by huangjinhou 2008-6-11
{
	UnInit();

	m_byteCurrentTextureIndex = pSrcMgr->m_byteCurrentTextureIndex;
	m_byteCurrentMaterialIndex = pSrcMgr->m_byteCurrentMaterialIndex;
	map<BYTE,KG3DTerrainDetailTexture*>::iterator it = pSrcMgr->m_mapDetailTexture.begin();
	while(it != pSrcMgr->m_mapDetailTexture.end())
	{
		BYTE index = it->first;
		KG3DTerrainDetailTexture* pSrcTexture = it->second;
		if(pSrcTexture)
		{
			KG3DTerrainDetailTexture* pNew = new KG3DTerrainDetailTexture;
			KGLOG_PROCESS_ERROR(pNew);

			pNew->m_byteIndex = index;

			pNew->m_byTexCoordIndex = pSrcTexture->m_byTexCoordIndex;
			pNew->m_fScaleU = pSrcTexture->m_fScaleU;
			pNew->m_fScaleV = pSrcTexture->m_fScaleV;
			pNew->m_lpDiffuseTexture = pSrcTexture->m_lpDiffuseTexture;
			if(pNew->m_lpDiffuseTexture)
			{
				pNew->m_lpDiffuseTexture->AddRef();
			}
			pNew->m_szDiffuseFileName = pSrcTexture->m_szDiffuseFileName;
			pNew->m_vUT = pSrcTexture->m_vUT;
			pNew->m_vVT = pSrcTexture->m_vVT;

			m_mapDetailTexture.insert(std::pair<BYTE,KG3DTerrainDetailTexture*>(index, pNew));
		}
		it++;
	}
	for (int i=0;i<256;i++)
	{
		/*KG3DTerrainDetailTexture* pSrcTexture = pSrcMgr->m_szDetailTexture[i];

		if(pSrcTexture)
		{
			KG3DTerrainDetailTexture* pNew = new KG3DTerrainDetailTexture;
			KGLOG_PROCESS_ERROR(pNew);

			pNew->m_byteIndex = (BYTE)i;

			pNew->m_byTexCoordIndex = pSrcTexture->m_byTexCoordIndex;
			pNew->m_fScaleU = pSrcTexture->m_fScaleU;
			pNew->m_fScaleV = pSrcTexture->m_fScaleV;
			pNew->m_lpDiffuseTexture = pSrcTexture->m_lpDiffuseTexture;
			if(pNew->m_lpDiffuseTexture)
			{
				pNew->m_lpDiffuseTexture->AddRef();
			}
			pNew->m_szDiffuseFileName = pSrcTexture->m_szDiffuseFileName;
			pNew->m_vUT = pSrcTexture->m_vUT;
			pNew->m_vVT = pSrcTexture->m_vVT;

			m_szDetailTexture[i] = pNew;
		}*/

		KG3DTerrainDetailMaterial* pSrcMtl = pSrcMgr->m_szDetailMaterial[i];
		if (pSrcMtl)
		{
			KG3DTerrainDetailMaterial* pNew = new KG3DTerrainDetailMaterial;
			KGLOG_PROCESS_ERROR(pNew);

			pNew->m_vecTextures = pSrcMtl->m_vecTextures;

			m_szDetailMaterial[i] = pNew;
		}
	}
	return S_OK;

Exit0:
	return E_FAIL;
}

HRESULT KG3DTerrainDetailMaterialMgr::SaveToInformationBuffer(DWORD& dwLength,KG3DMemoryFile* pFile)
{
	//pFile->Reset();

	DWORD dwVersion = 0;
	pFile->write(&dwVersion,sizeof(DWORD));
	BYTE byNum = (BYTE)m_mapDetailTexture.size();
	pFile->write(&byNum,sizeof(BYTE));//m_byteCurrentTextureIndex

	pFile->write(&m_byteCurrentMaterialIndex,sizeof(BYTE));

	map<BYTE,KG3DTerrainDetailTexture*>::iterator it = m_mapDetailTexture.begin();
	while(it != m_mapDetailTexture.end())
	{
		KG3DTerrainDetailTexture* pTexture = it->second;
		if(pTexture)
		{
			BYTE mask = 1;
			pFile->write(&mask,sizeof(BYTE));

			pTexture->SaveToInformationBuffer(pFile);
		}
		else
		{
			BYTE mask = 0;
			pFile->write(&mask,sizeof(BYTE));
		}
		it++;
	}
	//for (int i=0;i<256;i++)
	//{
	//	KG3DTerrainDetailTexture* pTexture = m_szDetailTexture[i];
	//	if(pTexture)
	//	{
	//		BYTE mask = 1;
	//		pFile->write(&mask,sizeof(BYTE));

	//		pTexture->SaveToInformationBuffer(pFile);
	//	}
	//	else
	//	{
	//		BYTE mask = 0;
	//		pFile->write(&mask,sizeof(BYTE));
	//	}
	//}

	for (int i=0;i<256;i++)
	{
		KG3DTerrainDetailMaterial* pMtl = m_szDetailMaterial[i];
		if(pMtl)
		{
			BYTE mask = 1;
			pFile->write(&mask,sizeof(BYTE));

			pMtl->SaveToInformationBuffer(pFile);
		}
		else
		{
			BYTE mask = 0;
			pFile->write(&mask,sizeof(BYTE));
		}
	}
	dwLength = pFile->GetLength();
	return S_OK;
}
 
HRESULT KG3DTerrainDetailMaterialMgr::LoadFromInformationBuffer(DWORD dwLength,KG3DMemoryFile* pFile)//modify by huangjinshou 2008-6-11
{
	UnInit();

	//pFile->Reset();

	DWORD dwVersion = 0;
	pFile->read(&dwVersion,sizeof(DWORD));
	BYTE byNum = 0;
	pFile->read(&byNum,sizeof(BYTE));//m_byteCurrentTextureIndex

	pFile->read(&m_byteCurrentMaterialIndex,sizeof(BYTE));

	for (BYTE i=0;i<byNum;i++)
	{
		BYTE mask = 0;
		pFile->read(&mask,sizeof(BYTE));

		if(mask)
		{
			KG3DTerrainDetailTexture* pTexture =  new KG3DTerrainDetailTexture();//m_szDetailTexture[i] =			
			pTexture->LoadFromInformationBuffer(pFile);
			m_mapDetailTexture.insert(pair<BYTE,KG3DTerrainDetailTexture*>(pTexture->m_byteIndex,pTexture));
		}
	}

	for (int i=0;i<256;i++)
	{
		BYTE mask = 1;
		pFile->read(&mask,sizeof(BYTE));


		if(mask)
		{
			KG3DTerrainDetailMaterial* pMtl = m_szDetailMaterial[i] = new KG3DTerrainDetailMaterial();
			pMtl->LoadFromInformationBuffer(pFile);
		}
	}

	KGLOG_PROCESS_ERROR( dwLength == pFile->GetPos());
	return S_OK;
Exit0:
	return E_FAIL;
}


//////////////////////////////////////////////////////////////////////////

HRESULT KG3DTerrainDetailMaterialMgr::GetMaterial(int nIndex,LPSTR pName,int& nTextureCount)
{
	KGLOG_PROCESS_ERROR(nIndex>=0);
	KGLOG_PROCESS_ERROR(nIndex<=255);

	KG3DTerrainDetailMaterial* pMtl = m_szDetailMaterial[nIndex];
	KG_PROCESS_ERROR(pMtl);

	wsprintf(pName,"Mtl %d",nIndex);
	nTextureCount = (int)pMtl->m_vecTextures.size();
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DTerrainDetailMaterialMgr::GetTextureForMaterial(int nMtlIndex,int nIndex,int& nTextID)
{
	KGLOG_PROCESS_ERROR(nMtlIndex>=0);
	KGLOG_PROCESS_ERROR(nMtlIndex<=255);

	KG3DTerrainDetailMaterial* pMtl = m_szDetailMaterial[nMtlIndex];
	KG_PROCESS_ERROR(pMtl);

	KGLOG_PROCESS_ERROR(nIndex>=0);
	KGLOG_PROCESS_ERROR(nIndex < (int)pMtl->m_vecTextures.size());

	nTextID = pMtl->m_vecTextures[nIndex];
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DTerrainDetailMaterialMgr::SetCurrentPaintMtl(int nIndex)
{
	return S_OK;
}

HRESULT KG3DTerrainDetailMaterialMgr::SetCurrentPaintLevel(int nLevel)
{
	return S_OK;
}

HRESULT KG3DTerrainDetailMaterialMgr::GetAllDetailTexture(map<BYTE,std::string>* pmapDetailTex)
{
	typedef pair<BYTE,std::string> BYTE_STR;
	map<BYTE,KG3DTerrainDetailTexture*>::iterator i = m_mapDetailTexture.begin();
	while (i != m_mapDetailTexture.end())
	{
		BYTE index = i->first;
		KG3DTerrainDetailTexture* pTexture = i->second;
		pmapDetailTex->insert(BYTE_STR(index,pTexture->m_szDiffuseFileName));
		i++;
	}
	return S_OK;
}

HRESULT KG3DTerrainDetailMaterialMgr::ChangeDetailTexture(BYTE byIndex,LPCTSTR pFileName)
{
	//ASSERT(byIndex < 256);
	HRESULT hr = E_FAIL;
	TCHAR szRelativePath[MAX_PATH];
	map<BYTE,KG3DTerrainDetailTexture*>::iterator iFind = m_mapDetailTexture.find(byIndex);
	if (iFind == m_mapDetailTexture.end())
		return hr;

	KG3DTerrainDetailTexture* pNew = new KG3DTerrainDetailTexture;
	KGLOG_PROCESS_ERROR(pNew);

	SAFE_DELETE(m_mapDetailTexture[byIndex]);
	pNew->m_byteIndex = byIndex;

	m_mapDetailTexture[byIndex] = pNew;//无论贴图有没有读取成功，都要放到数组里
	
	wsprintf(szRelativePath,"%s", GetRetPath(pFileName));

	pNew->m_szDiffuseFileName = szRelativePath;
	hr = g_cTextureTable.LoadResourceFromFile(szRelativePath, 0, 0, 
		(IKG3DResourceBase **)&pNew->m_lpDiffuseTexture);
	KGLOG_COM_PROCESS_ERROR(hr);
	if (m_lpOutDoorMgr && m_lpOutDoorMgr->GetSceneDataInterface() )
	{
		DWORD dwLength = 0;
		KG3DMemoryFile* pFile = g_cMemoryFilePool.RequestResource();
		if(pFile)
		{
			SaveToInformationBuffer(dwLength,pFile);
			m_lpOutDoorMgr->GetSceneDataInterface()->Request_ModifyTerrainInformation(m_uHandle,Terrain_Info_DetailMtlMgr,dwLength,pFile->GetBuffer());
			g_cMemoryFilePool.FreeResource(pFile);
			pFile = NULL;
		}
	}
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DTerrainDetailMaterialMgr::SetDetailTextureCoordIndex(INT nIndex,BYTE byCoord)
{
	KG3DTerrainDetailTexture* pTexture = m_mapDetailTexture[(BYTE)nIndex];
	if(!pTexture)
		return E_FAIL;

	pTexture->m_byTexCoordIndex = byCoord;
	pTexture->ComputeUVTransformation();

	if (m_lpOutDoorMgr && m_lpOutDoorMgr->GetSceneDataInterface() )
	{
		DWORD dwLength = 0;
		KG3DMemoryFile* pFile = g_cMemoryFilePool.RequestResource();
		if(pFile)
		{
			SaveToInformationBuffer(dwLength,pFile);
			m_lpOutDoorMgr->GetSceneDataInterface()->Request_ModifyTerrainInformation(m_uHandle,Terrain_Info_DetailMtlMgr,dwLength,pFile->GetBuffer());
			g_cMemoryFilePool.FreeResource(pFile);
			pFile = NULL;
		}
	}
	return S_OK;
}

HRESULT KG3DTerrainDetailMaterialMgr::GetDetailTextureCoordIndex(INT nIndex,BYTE& byCoord)
{
	KG3DTerrainDetailTexture* pTexture = m_mapDetailTexture[(BYTE)nIndex];
	if(!pTexture)
		return E_FAIL;

	byCoord = pTexture->m_byTexCoordIndex;
	return S_OK;
}

HRESULT KG3DTerrainDetailMaterialMgr::SetDetailTextureUV(INT nIndex,FLOAT m_fScaleU,FLOAT m_fScaleV,FLOAT   fEnv,FLOAT   fSpecular,FLOAT   fEmissive)

{
	KG3DTerrainDetailTexture* pTexture = m_mapDetailTexture[(BYTE)nIndex];
	if(!pTexture)
		return E_FAIL;
	pTexture->m_fScaleU = m_fScaleU;
	pTexture->m_fScaleV = m_fScaleV;
	pTexture->m_fEnvReflection = fEnv;
	pTexture->m_fSpecular = fSpecular;
	pTexture->m_fEmissive = fEmissive;

	pTexture->ComputeUVTransformation();
	if (m_lpOutDoorMgr && m_lpOutDoorMgr->GetSceneDataInterface() )
	{
		DWORD dwLength = 0;
		KG3DMemoryFile* pFile = g_cMemoryFilePool.RequestResource();
		if(pFile)
		{
			SaveToInformationBuffer(dwLength,pFile);
			m_lpOutDoorMgr->GetSceneDataInterface()->Request_ModifyTerrainInformation(m_uHandle,Terrain_Info_DetailMtlMgr,dwLength,pFile->GetBuffer());
			g_cMemoryFilePool.FreeResource(pFile);
			pFile = NULL;
		}
	}
	return S_OK;
}

HRESULT KG3DTerrainDetailMaterialMgr::GetDetailTextureUV(INT nIndex,FLOAT* pfScaleU,FLOAT* pfScaleV,FLOAT* pfEnv,FLOAT* pfSpecular,FLOAT* pfEmissive )
{
	KG3DTerrainDetailTexture* pTexture = m_mapDetailTexture[(BYTE)nIndex];
	if(!pTexture)
		return E_FAIL;
	(*pfScaleU) = pTexture->m_fScaleU;
	(*pfScaleV) = pTexture->m_fScaleV;
	(*pfEnv)    = pTexture->m_fEnvReflection ;
	(*pfSpecular) = pTexture->m_fSpecular;
	(*pfEmissive) = pTexture->m_fEmissive;
	return S_OK;
}

HRESULT KG3DTerrainDetailMaterialMgr::GetDetailTextureGrassData(BYTE byIndex,BOOL* pbGrass,BYTE* pbyGrassTexIndex,BYTE* pbyPatternIndex)
{
	KG3DTerrainDetailTexture* pTexture = m_mapDetailTexture[byIndex];
	if(!pTexture)
		return E_FAIL;
	if(pbGrass)
		(*pbGrass) = pTexture->m_bAssociateGrass;
	if(pbyGrassTexIndex)
		memcpy(pbyGrassTexIndex,pTexture->m_byGrassTexIndex,sizeof(pTexture->m_byGrassTexIndex));
	if(pbyPatternIndex)
		memcpy(pbyPatternIndex,pTexture->m_byPatternIndex,sizeof(pTexture->m_byPatternIndex));
	return S_OK;
}
HRESULT KG3DTerrainDetailMaterialMgr::SetDetailTextureGrassData(BYTE byIndex,BOOL* pbGrass,BYTE* pbyGrassTexIndex,BYTE* pbyPatternIndex,BOOL bSave)
{
	KG3DTerrainDetailTexture* pTexture = m_mapDetailTexture[byIndex];
	if(!pTexture)
		return E_FAIL;
	if(pbGrass)
		pTexture->m_bAssociateGrass = (*pbGrass);
	if(pbyGrassTexIndex)
		memcpy(pTexture->m_byGrassTexIndex,pbyGrassTexIndex,sizeof(pTexture->m_byGrassTexIndex));
	if(pbyPatternIndex)
		memcpy(pTexture->m_byPatternIndex,pbyPatternIndex,sizeof(pTexture->m_byPatternIndex));
	if (bSave && m_lpOutDoorMgr && m_lpOutDoorMgr->GetSceneDataInterface() )
	{
		DWORD dwLength = 0;
		KG3DMemoryFile* pFile = g_cMemoryFilePool.RequestResource();
		if(pFile)
		{
			SaveToInformationBuffer(dwLength,pFile);
			m_lpOutDoorMgr->GetSceneDataInterface()->Request_ModifyTerrainInformation(m_uHandle,Terrain_Info_DetailMtlMgr,dwLength,pFile->GetBuffer());
			g_cMemoryFilePool.FreeResource(pFile);
			pFile = NULL;
		}
	}
	return S_OK;
}