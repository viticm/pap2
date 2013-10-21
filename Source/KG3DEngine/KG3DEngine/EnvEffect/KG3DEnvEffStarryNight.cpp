////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DEnvEffStarryNight.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-2-26 10:09:57
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "KG3DEnvEffStarryNight.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace KG3D_ENVIRONMENT_EFFECT
{
	KG3DEnvEffStarryNight::KG3DEnvEffStarryNight()
		:KG3DEnvEffect(0, KG3DTYPE_ENVEFF_STARRY_NIGHT)
	{

	}

	HRESULT KG3DEnvEffStarryNight::FrameMove() 
	{
		return E_FAIL;
	}
	HRESULT KG3DEnvEffStarryNight::Render(const KG3DEnvEffRenderContext& renderContext)
	{
		return E_FAIL;
	}

	HRESULT KG3DEnvEffStarryNight::SaveMapSetting( IIniFile* pIniFile, LPCTSTR pSection )
	{
		return E_FAIL;
	}

	HRESULT KG3DEnvEffStarryNight::LoadMapSetting( IIniFile* pIniFile, LPCTSTR pSection )
	{
		return E_FAIL;
	} 

	HRESULT KG3DEnvEffStarryNight::ToSolidInterface( LPVOID* pEffectInterface )
	{
		if (NULL != pEffectInterface)
		{
			IEKG3DEnvEffStarryNight* pLowCastPointer = this;

			*pEffectInterface = reinterpret_cast<LPVOID>(pLowCastPointer);
			return S_OK;
		}
		return E_FAIL;
	}

	KG3DEnvEffect* KG3DEnvEffStarryNight::Build( LPCTSTR lpFile, DWORD_PTR wParam, DWORD_PTR lParam )
	{
		IEKG3DEnvEffStarryNightDesc desc;
		SetDefaultDesc(desc);
		KG3DEnvEffStarryNight* p = new KG3DEnvEffStarryNight;
		KG_PROCESS_ERROR(NULL != p);
Exit0:
		return NULL;
	}

	VOID KG3DEnvEffStarryNight::SetDefaultDesc( IEKG3DEnvEffStarryNightDesc& desc )
	{
		_tcscpy_s(desc.strTexture, _T("data\\public\\defaultscene\\starryNight.tga"));
		desc.fWidth = 5000000;
		desc.fHeight = 500000;
	}

	HRESULT KG3DEnvEffStarryNight::GetDescHandler( IEKG3DEnvEffDescHandler** pDescHandler )
	{
		return S_OK;
	}
	bool KG3DDWGC_Key_StarryNight::CalculateInterp( const KG3DDWGC_KeyBase& BaseKeyBefore, const KG3DDWGC_KeyBase& BaseKeyAfter, float fPercent)
	{
		return false;
	}

	bool KG3DDWGC_Key_StarryNight::SaveMapSetting( IIniFile* pIniFile, LPCTSTR pSection )
	{
		return false;
	}

	bool KG3DDWGC_Key_StarryNight::LoadMapSetting( IIniFile* pIniFile, LPCTSTR pSection )
	{
		return false;
	}

	void KG3DDWGC_Key_StarryNight::DebugPrint( std::ostringstream& OSS )
	{
		return;
	}



	HRESULT KG3DEnvEffStarryNight::DescHandler::GetDefDesc( IEKG3DEnvEffDescBase* pDesc, size_t uSizeOfDesc )
	{
		_ASSERTE(NULL != pDesc);
		if(NULL == pDesc)
			return E_FAIL;

		_ASSERTE(pDesc->dwType == DWGC_TYPE && uSizeOfDesc == sizeof(TypeDesc));
		if (uSizeOfDesc != sizeof(TypeDesc))
			return E_FAIL;

		TypeDesc& desc = (TypeDesc&)(*pDesc);
		_tcscpy_s(desc.strTexture, _T("data\\public\\defaultscene\\starryNight.tga"));
		desc.fWidth = 5000000;
		desc.fHeight = 500000;
		return S_OK;
	}

	HRESULT KG3DEnvEffStarryNight::DescHandler::GetDescParamTable( TypeParamDescEnumer** pEnumer, size_t* puKeyParamCount )
	{
		static IEKG3DParamDesc s_paramDescArray[] = 
		{
			{KG3DTYPE_FLOAT, _T("Width"), offsetof(TypeDesc, fWidth), sizeof(FLOAT)},
			{KG3DTYPE_FLOAT, _T("Height"), offsetof(TypeDesc, fHeight), sizeof(FLOAT)},
			{KG3DTYPE_PATH, _T("TexPath"), offsetof(TypeDesc, strTexture), sizeof(TypePathBuffer)},
		};
		static size_t s_uKeyParamCount = 2;
		static KGCH::TFrontAccessProxyArray<IEKG3DParamDesc> s_Proxy(s_paramDescArray);

		_ASSERTE(s_uKeyParamCount < _countof(s_paramDescArray));
		_ASSERTE(s_paramDescArray[s_uKeyParamCount-1].uOffset + s_paramDescArray[s_uKeyParamCount-1].uSize <= sizeof(TypeDesc));

		if (NULL != pEnumer)
		{
			*pEnumer = &s_Proxy;
		}

		if (NULL != puKeyParamCount)
		{
			*puKeyParamCount = s_uKeyParamCount;
		}
		return S_OK;
	}
};
////////////////////////////////////////////////////////////////////////////////

