// TextureTable.cpp: implementation of the KTextureTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "TextureTable.h"
#include ".\texturetable.h"
#include "MovieTexture.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KTextureTable::KTextureTable()
{
	m_dMaxNumTexture = 0;
	m_dNumTexture = 0;
	m_lpTextures = NULL;
}

KTextureTable::~KTextureTable()
{

}

HRESULT KTextureTable::Get1NewTexturePointer(LPTEXTURE **pppTexture)
{
	if (m_dMaxNumTexture ==0 )
	{
		m_lpTextures = new LPTEXTURE[5];
		ZeroMemory(m_lpTextures ,sizeof(LPTEXTURE)*5);
		m_dMaxNumTexture = 5;
	}
	if (m_dNumTexture >= m_dMaxNumTexture)
	{
		LPTEXTURE* Temp = m_lpTextures;
		m_dMaxNumTexture +=5;
		m_lpTextures = new LPTEXTURE[m_dMaxNumTexture];
		ZeroMemory(m_lpTextures ,sizeof(LPTEXTURE)*m_dMaxNumTexture);
		for(DWORD i=0;i<m_dMaxNumTexture-5 ;i++) m_lpTextures[i] = Temp[i];
		delete [] Temp;
	}	
	m_dNumTexture ++;
	(*pppTexture) = &m_lpTextures[ m_dNumTexture -1 ];

	return S_OK;
}

HRESULT KTextureTable::Get1NewTexture(LPTEXTURE *ppTexture, DWORD Type)
{
	LPTEXTURE * NewPointer = NULL;
	if (FAILED(Get1NewTexturePointer(&NewPointer)))
		return E_FAIL;
	switch(Type)
	{
	case 0:
		{
			(*NewPointer) = new KTexture;
			(*NewPointer)->Initialize(this); 
			break;
		}
	case 1:
		{
			(*NewPointer) = new KAnimateTexture;
			(*NewPointer)->Initialize(this); 
			break;
		}
	case 2:
		{
			(*NewPointer) = new KMovieTexture;
			//Do not need to be initialized;
			break;
		}
	}
	(*NewPointer)->ID = m_dNumTexture;
	(*ppTexture) = (*NewPointer);
	return S_OK;
}

HRESULT KTextureTable::LoadFromFile(DWORD *pTextureID, LPSTR pFileName)
{
	TCHAR Name[256];

	TCHAR path_buffer[256];
	TCHAR drive[256];
	TCHAR dir[256];
	TCHAR fname[256];
	TCHAR ext[256];

	wsprintf(path_buffer,"%s",pFileName);
	_splitpath( path_buffer, drive, dir, fname, ext );

	if ( (!_stricmp(ext, ".avi")) || (!_stricmp(ext, ".wmv")) || (!_stricmp(ext,".rmvb")))
	{
		if ( FAILED( LoadMovieFromFile(pTextureID, path_buffer)) )
			return E_FAIL;
		return S_OK;
	}

	if(SUCCEEDED(g_cFileFinder.FindFile(Name,pFileName)))
	{
		HRESULT hr = S_OK;
		for(DWORD i=0;i<m_dNumTexture;i++) 
		{
			LPTEXTURE pTexture = m_lpTextures[i] ;
			if (pTexture->m_lpFileName!=NULL)
			{
				if (strcmp(pTexture->m_lpFileName,Name)==0)
				{
					(*pTextureID) = pTexture->ID;
					return S_OK;
				}
			}
			else
				continue;
		}

		LPTEXTURE pTexture = NULL ;
		
		if (_stricmp(ext,".spr")==0)
		{
			if (FAILED(Get1NewTexture(&pTexture,1)))
				return E_FAIL;
		}
		else
		{
			if (FAILED(Get1NewTexture(&pTexture,0)))
				return E_FAIL;
		}

		if (FAILED(hr = pTexture->LoadFromFile(Name)))
		{
			//pTexture->CleanUp();
			SAFE_DELETE(pTexture); 
			m_dNumTexture--; 
			return hr;
		}
		(*pTextureID) = pTexture->ID;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT KTextureTable::LoadCubeFromFile(DWORD *pTextureID, LPSTR pFileName)
{
	HRESULT hr = S_OK;
	for(DWORD i=0;i<m_dNumTexture;i++) 
	{
		LPTEXTURE pTexture = m_lpTextures[i] ;
		if (pTexture->m_lpFileName!=NULL)
		{
			if (strcmp(pTexture->m_lpFileName,pFileName)==0)
			{
				(*pTextureID) = pTexture->ID;
				return S_OK;
			}
		}
		else
			continue;
	}

	LPTEXTURE pTexture = NULL ;

	TCHAR Name[256];
	TCHAR path_buffer[256];
	TCHAR drive[256];
	TCHAR dir[256];
	TCHAR fname[256];
	TCHAR ext[256];

	wsprintf(path_buffer,"%s",pFileName);
	_splitpath( path_buffer, drive, dir, fname, ext );
	if (_stricmp(ext,".spr")==0)
	{
		if (FAILED(Get1NewTexture(&pTexture,1)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(Get1NewTexture(&pTexture,0)))
			return E_FAIL;
	}

	if (FAILED(hr = pTexture->LoadCubeMap(pFileName)))
	{
		wsprintf(Name,"%s\\%s%s",g_Def_ModelTextureDirectory,fname,ext);

		if(FAILED(hr = pTexture->LoadCubeMap(Name)))
		{
			delete [] pTexture; 
			m_dNumTexture--; 
			return hr;
		}
	}
	(*pTextureID) = pTexture->ID;
	return S_OK;
}

HRESULT KTextureTable::LoadMovieFromFile(DWORD* pTextureID, TCHAR* pFileName)
{
	HRESULT hr = S_OK;
	for(DWORD i=0;i<m_dNumTexture;i++) 
	{
		LPTEXTURE pTexture = m_lpTextures[i] ;
		if (pTexture->m_lpFileName!=NULL)
		{
			if (strcmp(pTexture->m_lpFileName,pFileName)==0)
			{
				(*pTextureID) = pTexture->ID;
				return S_OK;
			}
		}
		else
			continue;
	}

	LPTEXTURE pTexture = NULL ;

	//TCHAR Name[256];
	TCHAR path_buffer[256];
	TCHAR drive[256];
	TCHAR dir[256];
	TCHAR fname[256];
	TCHAR ext[256];

	wsprintf(path_buffer,"%s",pFileName);
	_splitpath( path_buffer, drive, dir, fname, ext );
	if (FAILED(Get1NewTexture(&pTexture,2)))
		return E_FAIL;
	KMovieTexture* pMovieTexture = (KMovieTexture*)pTexture;
	pMovieTexture->Initialize(pFileName, FALSE);
	(*pTextureID) = pTexture->ID;
	pTexture->m_lpFileName = new TCHAR[256];
	wsprintf(pTexture->m_lpFileName, pFileName);
	pMovieTexture->Run();
	return S_OK;
}

HRESULT KTextureTable::LoadVolumeFromFile(DWORD *pTextureID, LPSTR pFileName)
{
	HRESULT hr = S_OK;
	for(DWORD i=0;i<m_dNumTexture;i++) 
	{
		LPTEXTURE pTexture = m_lpTextures[i] ;
		if (pTexture->m_lpFileName!=NULL)
		{
			if (strcmp(pTexture->m_lpFileName,pFileName)==0)
			{
				(*pTextureID) = pTexture->ID;
				return S_OK;
			}
		}
		else
			continue;
	}

	LPTEXTURE pTexture = NULL ;

	TCHAR Name[256];
	TCHAR path_buffer[256];
	TCHAR drive[256];
	TCHAR dir[256];
	TCHAR fname[256];
	TCHAR ext[256];

	wsprintf(path_buffer,"%s",pFileName);
	_splitpath( path_buffer, drive, dir, fname, ext );
	if (_stricmp(ext,".spr")==0)
	{
		if (FAILED(Get1NewTexture(&pTexture,1)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(Get1NewTexture(&pTexture,0)))
			return E_FAIL;
	}

	if (FAILED(hr = pTexture->LoadVolumeMap(pFileName)))
	{
		wsprintf(Name,"%s\\%s%s",g_Def_ModelTextureDirectory,fname,ext);

		if(FAILED(hr = pTexture->LoadVolumeMap(Name)))
		{
			delete [] pTexture; 
			m_dNumTexture--; 
			return hr;
		}
	}
	(*pTextureID) = pTexture->ID;
	return S_OK;
}

HRESULT KTextureTable::GetTexture(LPTEXTURE *ppTexture, DWORD ID)
{
	if ((ID>m_dNumTexture)||(ID==0)) return E_FAIL;
	(*ppTexture) = m_lpTextures[ID-1];
	return S_OK;
}

HRESULT KTextureTable::CleanUp()
{
	for(DWORD i=1;i<=m_dNumTexture;i++)
	{
		LPTEXTURE pText = NULL;
		if (SUCCEEDED(GetTexture(&pText,i)))
		{
			pText->CleanUp();
		}
		SAFE_DELETE(pText);
	}
	m_dNumTexture = 0;
	m_dMaxNumTexture = 0;
	SAFE_DELETE_ARRAY(m_lpTextures);

	return S_OK;
}

HRESULT KTextureTable::SetTexture(DWORD Index, DWORD TextureID)
{
	LPTEXTURE pTexture = NULL;
	if (FAILED(this->GetTexture(&pTexture,TextureID)))
	{
		return E_FAIL;
	}
	g_pd3dDevice->SetTexture(Index,pTexture->m_lpTexture);
	return S_OK;
}

HRESULT KTextureTable::InvalidateDeviceObjects(void)
{
	for(DWORD i=1;i<=m_dNumTexture;i++)
	{
		LPTEXTURE pText = NULL;
		if (SUCCEEDED(GetTexture(&pText,i)))
		{
			pText->InvalidateDeviceObjects();
		}
	}
	return E_NOTIMPL;
}

HRESULT KTextureTable::RestoreDeviceObjects(void)
{
	for(DWORD i=1;i<=m_dNumTexture;i++)
	{
		LPTEXTURE pText = NULL;
		if (SUCCEEDED(GetTexture(&pText,i)))
		{
			pText->RestoreDeviceObjects();
		}
	}
	return S_OK;
}

HRESULT KTextureTable::ReloadFromFile()
{
	for(DWORD i=1;i<=m_dNumTexture;i++)
	{
		LPTEXTURE pText = NULL;
		if (SUCCEEDED(GetTexture(&pText,i)))
		{
			pText->ReloadFromFile();
		}
	}
	return S_OK;
}

