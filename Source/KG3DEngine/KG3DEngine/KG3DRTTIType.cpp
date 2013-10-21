////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DRTTIType.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-2-28 10:38:29
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "KG3DRTTIType.h"
#include "IEKG3DEnvironment.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////
KG3DRTTITypeManager g_cKG3DRTTITypeManager;
KG3DRTTITypeManager* g_cGetRTTITypeManager()
{
	return &g_cKG3DRTTITypeManager;
}

KG3DRTTITypeManager::KG3DRTTITypeManager()
	:m_nEnvEffectTypeCount(0)
{
#define ADD_RTTI(a, b) ContainerType::insert(ContainerType::value_type(a, KG3DRTTIType(a, b)));m_nEnvEffectTypeCount = (INT)ContainerType::size()
	///注意这里的名字会影响读取关键帧，不要随便更改
	ADD_RTTI(KG3DTYPE_ENVEFF_SKYSYSTEM_SYSTEM, "SkySystem");
	ADD_RTTI(KG3DTYPE_ENVEFF_SKYSYSTEM_SKYBOX, "SkyBox");
	ADD_RTTI(KG3DTYPE_ENVEFF_SKYSYSTEM_FARMOUNTAIN, "FarMount");
	ADD_RTTI(KG3DTYPE_ENVEFF_SKYSYSTEM_CLOUD, "Cloud");
	ADD_RTTI(KG3DTYPE_ENVEFF_FOG, "Fog");
	ADD_RTTI(KG3DTYPE_ENVEFF_LENSFLARE_SUN, "LensflareSun");
	ADD_RTTI(KG3DTYPE_ENVEFF_LENSFLARE_MOON, "LensflareMoon");
	ADD_RTTI(KG3DTYPE_ENVEFF_LIGHTSET, "LightSet");
	ADD_RTTI(KG3DTYPE_ENVEFF_LIGHT, "Light");
	ADD_RTTI(KG3DTYPE_ENVEFF_RAIN, "Rain");
	ADD_RTTI(KG3DTYPE_ENVEFF_DYNAMIC_WEATHER, "DynamicWeather");
	ADD_RTTI(KG3DTYPE_REGION_INFO_MANAGER, "RegionInfoManager");
	ADD_RTTI(KG3DTYPE_ENVEFF_STARRY_NIGHT, "StarryNight");
	ADD_RTTI(KG3DTYPE_ENVEFF_WIND, "Wind");
	///<KG3DEnvEffect SubClass Register Point>
}

HRESULT KG3DRTTITypeManager::GetRTTIType( DWORD dwType, IKG3DRTTIType** pRetType )
{
	if (!pRetType)
	{
		return E_INVALIDARG;
	}
	*pRetType = GetType(dwType);
	return S_OK;
}

KG3DRTTIType* KG3DRTTITypeManager::GetType( DWORD dwType )
{
	ContainerType::iterator it = ContainerType::find(dwType);
	if (it != ContainerType::end())
	{
		return &(it->second);
	}
	return NULL;
}

const TCHAR* KG3DRTTITypeManager::GetName( DWORD dwType )
{
	KG3DRTTIType* pType = GetType(dwType);
	if (pType)
	{
		return pType->Name();
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
KG3DRTTIType::KG3DRTTIType(DWORD dwType, LPCTSTR tczName)
{
	_ASSERTE(tczName);
	m_DescStruct.m_dwType = dwType;
	_tcscpy_s(m_DescStruct.m_tczName, _countof(m_DescStruct.m_tczName), tczName);
}

KG3DRTTIType::KG3DRTTIType( const KG3DRTTIType& OtherStruct)
{
	m_DescStruct = OtherStruct.m_DescStruct;
}
KG3DRTTIType& KG3DRTTIType::operator=(const KG3DRTTIType& OtherStruct)
{
	m_DescStruct = OtherStruct.m_DescStruct;
	return *this;
}
KG3DRTTIType::~KG3DRTTIType()
{

}

bool KG3DRTTIType::operator==(const KG3DRTTIType& OtherStruct)const
{
	if (m_DescStruct.m_dwType == OtherStruct.m_DescStruct.m_dwType)
	{
		return true;
	}
	return false;
}
bool KG3DRTTIType::operator!=(const KG3DRTTIType& OtherStruct)const
{
	if (m_DescStruct.m_dwType != OtherStruct.m_DescStruct.m_dwType)
	{
		return true;
	}
	return false;
}
bool KG3DRTTIType::before(const KG3DRTTIType& OtherStruct)const
{
	return true;///预留
}

const TCHAR* KG3DRTTIType::Name()const
{
	return m_DescStruct.m_tczName;
}

HRESULT KG3DRTTIType::GetInfo( IKG3DRTTITypeDescStruct* pDescStruct )
{
	if (!pDescStruct)
	{
		return E_INVALIDARG;
	}
	*pDescStruct = m_DescStruct;
	return S_OK;
}

HRESULT KG3DRTTIType::IsEqual( IKG3DRTTIType* pOtherType )
{
	KG_PROCESS_ERROR(pOtherType);
	KG3DRTTIType* pType = static_cast<KG3DRTTIType*>(pOtherType);
	if (pType == this || pType->m_DescStruct.m_dwType == m_DescStruct.m_dwType)
	{
		return S_OK;
	}
Exit0:
	return E_FAIL;
}

HRESULT KG3DRTTIType::GetName( LPTSTR tczName, INT nBufferSize)
{
	INT nRet = -1;
	KG_PROCESS_ERROR(tczName && nBufferSize >= _countof(m_DescStruct.m_tczName));
	nRet = _tcscpy_s(tczName, nBufferSize, m_DescStruct.m_tczName);
	KG_PROCESS_ERROR(nRet == 0);
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DRTTIType::GetType( DWORD* pdwType )
{
	if (pdwType)
	{
		*pdwType = m_DescStruct.m_dwType;
		return S_OK;
	}
	return E_FAIL;
}
