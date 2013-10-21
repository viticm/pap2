#include "StdAfx.h"
#include "KG3DTerrainRegionInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KG3DTerrainRegionInterface::KG3DTerrainRegionInterface()
{
	
}

KG3DTerrainRegionInterface::~KG3DTerrainRegionInterface()
{
}


KG3DTerrainRegionDataBase::KG3DTerrainRegionDataBase()
{
	m_dwDetailBlendWidth = 0;
	m_lpDetailBlend = NULL;
	m_lTextureKey = 0;
}

KG3DTerrainRegionDataBase::~KG3DTerrainRegionDataBase()
{
	UnInit();
}

HRESULT KG3DTerrainRegionDataBase::Init()
{
	return S_OK;
}

HRESULT KG3DTerrainRegionDataBase::UnInit()
{
	m_dwDetailBlendWidth = 0;
	SAFE_DELETE_ARRAY(m_lpDetailBlend);
	m_lTextureKey = 0;

	return S_OK;
}

HRESULT KG3DTerrainRegionDataBase::CreateDetailBlendData(DWORD dwWidth)
{
	
	return S_OK;
}
