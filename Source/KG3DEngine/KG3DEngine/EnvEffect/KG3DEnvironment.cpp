////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DEnvironment.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2006-11-29 16:40:15
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdAfx.h"

///编译宏选择
#include "KG3DEnvEffCommonDef.h"

#include "KG3DEnvironment.h"
#include "KG3DEnvEffList.h"
#include "KG3DSkySystem.h"
#include "KG3DEnvEffDynamicWeather.h"
#include "KG3DEnvEffFog.h"
#include "KG3DMeshLensFlare.h"
#include "KG3DEnvEffLightSet.h"
#include "KG3DEnvEffLight.h"
#include "KG3DEnvEffStarryNight.h"
#include "KG3DEnvEffWind.h"
#include "KG3DMessenger.h"
#include "KGINIFileHelpers.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

TCHAR stDefaultDynamicWeatherFilePath[] = _T("data/public/DynamicWeather/Default_Setting.ini");

using namespace KG3D_ENVIRONMENT_EFFECT;
////////////////////////////////////////////////////////////////////////////////
KG3DEnvironment::KG3DEnvironment()
:m_dwSettingType(0)
{
#if defined(DEBUG) | defined(_DEBUG)
#if _KG_DEBUG_ENABLE_LEAK_TEST == 1
	///<LEAK_TEST>
	CHAR* __pLeakTest1 = new CHAR[4];///内存泄漏测试
	INT* __pLeakTest2 = new INT;///内存泄漏测试
#endif
#endif
}

KG3DEnvironment::~KG3DEnvironment()
{
	UnInit();
}

HRESULT KG3DEnvironment::Init()
{
    return S_OK;
}

HRESULT KG3DEnvironment::UnInit()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

	hr = ClearEnvEffect();
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DEnvironment::OnLostDevice()
{
	/*KG3DEnvEffect* pEnvEffect = m_EnvEffList.ResetIterator();
	while (pEnvEffect)*/
	KGCH::IteratorHolder holder;
	for(KG3DEnvEffect* pEnvEffect = m_EnvEffList.GetFirstTopEffect(holder)
		; NULL != pEnvEffect
		; pEnvEffect = m_EnvEffList.GetNextTopEffect(holder))
	{
		pEnvEffect->OnLostDevice();
		//pEnvEffect = m_EnvEffList.NextIterator();
	}
	return S_OK;
}

HRESULT KG3DEnvironment ::OnResetDevice()
{
	/*KG3DEnvEffect* pEnvEffect = m_EnvEffList.ResetIterator();
	while (pEnvEffect)*/
	KGCH::IteratorHolder holder;
	for(KG3DEnvEffect* pEnvEffect = m_EnvEffList.GetFirstTopEffect(holder)
		; NULL != pEnvEffect
		; pEnvEffect = m_EnvEffList.GetNextTopEffect(holder))
	{
		pEnvEffect->OnResetDevice();
		//pEnvEffect = m_EnvEffList.NextIterator();
	}
	return S_OK;
}


HRESULT KG3DEnvironment::FrameMove()
{
	DWORD dwStartTime = timeGetTime();
	/*KG3DEnvEffect* pEnvEffect = m_EnvEffList.ResetIterator();
	while (pEnvEffect)*/
	KGCH::IteratorHolder holder;
	for(KG3DEnvEffect* pEnvEffect = m_EnvEffList.GetFirstTopEffect(holder)
		; NULL != pEnvEffect
		; pEnvEffect = m_EnvEffList.GetNextTopEffect(holder))
	{
		if (pEnvEffect->IsEnable())
		{
			pEnvEffect->FrameMove();
		}
		//pEnvEffect = m_EnvEffList.NextIterator();
	}

	DWORD dwCost = timeGetTime() - dwStartTime;
	if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost >= 20)
	{
		KGLogPrintf(KGLOG_ERR, "TimeOptimize %d KG3DEnvironment::FrameMove.",dwCost);
	}
	return S_OK;
}

HRESULT KG3DEnvironment::RenderSun()
{
    return m_Renderer.RenderSun(*this);        
}

const D3DXVECTOR4* KG3DEnvironment::GetSunPosition()
{
    return m_Renderer.GetSunPosition(*this);
}

HRESULT KG3DEnvironment::Render(DWORD dwFilter)
{
	return m_Renderer.Render(*this, dwFilter);	
}

HRESULT KG3DEnvironment::PostRender()
{
	return m_Renderer.PostRender(*this);
}

HRESULT KG3DEnvironment::SaveMapSetting( IIniFile* pIniFile )
{
	KG_PROCESS_ERROR(pIniFile);
	///保存有些什么Effect
	char tempSection[] = _STRINGER(KG3DEnvironment);
	char szKeyName[64];
	char szKeyValue[64];
	//////////////////////////////////////////////////////////////////////////
	
	int nWrited = 0;

	KG_CUSTOM_HELPERS::EraseSection(pIniFile, tempSection);

	/*KG3DEnvEffect* pEnvEffect = m_EnvEffList.ResetIterator();
	while (pEnvEffect)*/
	KGCH::IteratorHolder holder;
	for(KG3DEnvEffect* pEnvEffect = m_EnvEffList.GetFirstTopEffect(holder)
		; NULL != pEnvEffect
		; pEnvEffect = m_EnvEffList.GetNextTopEffect(holder))
	{
		{
			sprintf_s(szKeyName, sizeof(szKeyName), "EnvEff%d", nWrited++);		
			ULONG dwType = pEnvEffect->GetType();
			_ASSERTE(dwType != KG3DTYPE_ENVEFF_INVALID_EFFECT);
			sprintf_s(szKeyValue, sizeof(szKeyValue), "%d", (int)dwType);
			pIniFile->WriteString(tempSection, szKeyName, szKeyValue);	
		}
		//pEnvEffect = m_EnvEffList.NextIterator();
	}
	///保存各个Effect
	/*KG3DEnvEffect* pEnvEffect = m_EnvEffList.ResetIterator();
	while (pEnvEffect)*/
	KGCH::IteratorHolder holder2;
	for(KG3DEnvEffect* pEnvEffect = m_EnvEffList.GetFirstTopEffect(holder2)
		; NULL != pEnvEffect
		; pEnvEffect = m_EnvEffList.GetNextTopEffect(holder2))
	{
		pEnvEffect->SaveMapSetting(pIniFile, NULL);
		//pEnvEffect = m_EnvEffList.NextIterator();
	}
	return S_OK;
Exit0:
	return E_FAIL;
}
extern BOOL g_bClient;
HRESULT KG3DEnvironment::LoadMapSetting( IIniFile* pIniFile, LPCTSTR pSection )
{
#if defined(DEBUG) | defined(_DEBUG)
#ifdef KG_ENABLE_TDD
	m_EnvEffList.TDDTest();
#endif
#endif
	///新建的状态
	if (! pIniFile)
	{
		this->EnsureFogAndLight();				
	}
	///有文件的状态
	queue<DWORD> tempEraseQueue;
	KG_PROCESS_ERROR(pIniFile);
	///读取Effect
	char tempSection[] = _STRINGER(KG3DEnvironment);
	char szKeyName[64];
	char szKeyValue[64];
	*szKeyName = '\0';
	*szKeyValue = '\0';
	while (pIniFile->GetNextKey(tempSection, szKeyName, szKeyName))
	{
		pIniFile->GetString(tempSection, szKeyName, "", szKeyValue, sizeof(szKeyValue));
		if (*szKeyValue != '\0')
		{
			DWORD dwType = (DWORD)atoi(szKeyValue);
			if (dwType > KG3DTYPE_ENVEFF_END)
			{
				continue;///如果大于，肯定是无效的，什么也不做
			}
			if (dwType < KG3DTYPE_ENVEFF_BEGIN)///旧的EnvEffect从16开始，为了以后兼容别的RTTI机制，把EnvEffect提到2000开始
			{
				dwType += KG3DTYPE_ENVEFF_BEGIN;
			}

			this->AddEnvEffect(dwType, NULL, pIniFile, NULL);///注意因为DW永远比其他的EnvEffect大，所以他在列表中总是最后的
		}
	}	
	EnsureFogAndLight(pIniFile);
	if (g_cEngineOption.bEnableDefaultDynamicWeather && g_bClient)
	{
		EnsureDynamicWeather();
	}
	
	return S_OK;
Exit0:
	return E_FAIL;
}


VOID KG3DEnvironment::EnsureDynamicWeather()
{
	_ASSERTE(g_bClient && _T("只能在客户端用"));
	if(NULL != GetEnvEffect(KG3DTYPE_ENVEFF_DYNAMIC_WEATHER, 0)/*GetTopEnvEffect<KG3DTYPE_ENVEFF_DYNAMIC_WEATHER>()*/)
		return;

	IIniFile* pFile = g_OpenIniFile(stDefaultDynamicWeatherFilePath);
	if(NULL == pFile)
		return;

	HRESULT hr = AddEnvEffect(KG3DTYPE_ENVEFF_DYNAMIC_WEATHER, NULL, pFile, NULL);
	KG_USE_ARGUMENT(hr);

	SAFE_RELEASE(pFile);
}

HRESULT KG3DEnvironment::AddEnvEffect(DWORD dwType, IEKG3DEnvEffect** ppEnvEffect, IIniFile* pIniFile, LPCTSTR pSection)
{
	KG3DEnvEffect* pTempEnvEffect = NULL;

	HRESULT hr = E_FAIL;
	//KG_PROCESS_ERROR(NULL == m_EnvEffList.Find(dwType));//insert会再Find一次，这里先Assert一次。
	//检查是否可能加入的Effect种类太多了，纯粹的设计问题
	//_KG3D_DEBUG_SELF_ASSERTE(m_EnvEffList.GetValidElementCount()< KG3DENVEFF_MAX);
	
	switch(dwType)
	{
	case KG3DTYPE_ENVEFF_SKYSYSTEM_SYSTEM:
		{
			//_KG3D_DEBUG_FAIL_REPORT(_CrtCheckMemory());
			KG3DSkySystem* pSkySystem = new KG3DSkySystem;
			//_KG3D_DEBUG_FAIL_REPORT(_CrtCheckMemory());
			if (NULL != pSkySystem)
			{
				pTempEnvEffect = pSkySystem;				
			}
		}
		break;
	case KG3DTYPE_ENVEFF_DYNAMIC_WEATHER:
		{
			KG3DEnvEffDynamicWeather* pDynWeather  = new KG3DEnvEffDynamicWeather(&m_EnvEffList);///发生了向上转换，得到的是只读的版本		
			if(NULL != pDynWeather)
			{
				pTempEnvEffect = pDynWeather;///向上转换是一定安全的
			}			
		}		
		break;
	case KG3DTYPE_ENVEFF_FOG:
		{
			KG3DEnvEffFog* pFog = KGCommonBufferGetPtr(m_pFogBuffer);

			if (NULL != pFog)
			{
				pTempEnvEffect = pFog;				
			}
		}
		break;
	case KG3DTYPE_ENVEFF_LENSFLARE_SUN:
		{
			KG3DEnvEffLensFlareSun* pLensFlare = new KG3DEnvEffLensFlareSun;
			if (pLensFlare)
				pTempEnvEffect = pLensFlare;
		}
		break;
	case KG3DTYPE_ENVEFF_LENSFLARE_MOON:
		{
			KG3DEnvEffLensFlareMoon* pLenFlareMoon = new KG3DEnvEffLensFlareMoon;
			if (pLenFlareMoon)
				pTempEnvEffect = pLenFlareMoon;
		}
		break;
	case KG3DTYPE_ENVEFF_LIGHTSET:
		{
			KG3DEnvEffLightSet* pLightSet = KGCommonBufferGetPtr(m_pLightSetBuffer);
			if (pLightSet)
				pTempEnvEffect = pLightSet;
		}
		break;
	case KG3DTYPE_ENVEFF_LIGHT:
		{
			KG3DEnvEffLight* pLight = new KG3DEnvEffLight;
			if (pLight)
				pTempEnvEffect = pLight;
		}
		break;
    case KG3DTYPE_ENVEFF_WIND:
		{
			KG3DEnvEffWind* pWind = new KG3DEnvEffWind();
			if(NULL != pWind)
				pTempEnvEffect = pWind;
		}
		break;

	/*case KG3DTYPE_ENVEFF_STARRY_NIGHT:
		{
			KG3DEnvEffStarryNight* pStarryNight = new KG3DEnvEffStarryNight;
			if(NULL != pStarryNight)
				pTempEnvEffect = pStarryNight;
		}
		break;*/
		///<KG3DEnvEffect SubClass Register Point>
	default:
		goto Exit0;
		break;
	}	
	KG_PROCESS_ERROR(pTempEnvEffect != NULL);

	_ASSERTE(pTempEnvEffect->IsEnable() && pTempEnvEffect->GetType() == dwType);

	hr = pTempEnvEffect->LoadMapSetting(pIniFile, pSection);///根据pIniFile是否为NULL，进行新建或者是载入的操作，都会Init
	KG_COM_PROCESS_ERROR(hr);

	hr = m_EnvEffList.Insert(dwType, pTempEnvEffect);	
	KG_COM_PROCESS_ERROR(hr);
	if (NULL != ppEnvEffect)
	{
		*ppEnvEffect = pTempEnvEffect;
	}	
	return S_OK;
Exit0:///保证如果没有入列的必须释放
	_KG3D_DEBUG_SELF_OUTPUT1("KG3DEnvironment添加以下Effect失败：%d\n", dwType);

	if (dwType == KG3DTYPE_ENVEFF_LIGHTSET || dwType == KG3DTYPE_ENVEFF_FOG)
	{
		pTempEnvEffect = NULL;//这两种是从Buffer中得到的，不需要Delete
	}
	else
	{
		SAFE_DELETE(pTempEnvEffect);
	}
	
	if (NULL != ppEnvEffect)
	{
		*ppEnvEffect = NULL;
	}	
	return E_FAIL;
}
HRESULT KG3DEnvironment::DelEnvEffect(DWORD dwType, INT_PTR nIndex)
{
	//Fog和LightSet必须一直存在，不能完全删除，这里利用KGCommonBufferRecreate从新在CommonBuffer中创建出对象
	//指针不变
	if(dwType == KG3DTYPE_ENVEFF_FOG)
	{
		_ASSERTE(/*this->GetTopEnvEffect<KG3DTYPE_ENVEFF_FOG>()*/ this->GetEnvEffect(KG3DTYPE_ENVEFF_FOG, 0)== KGCommonBufferGetPtr(m_pFogBuffer));
		KGCommonBufferRecreate(m_pFogBuffer);
		m_pFogBuffer->LoadMapSetting(NULL, NULL);
	}
	else if (dwType == KG3DTYPE_ENVEFF_LIGHTSET)
	{
		_ASSERTE(/*this->GetTopEnvEffect<KG3DTYPE_ENVEFF_LIGHTSET>()*/this->GetEnvEffect(KG3DTYPE_ENVEFF_LIGHTSET, 0) == KGCommonBufferGetPtr(m_pLightSetBuffer));
		KGCommonBufferRecreate(m_pLightSetBuffer);
		m_pLightSetBuffer->LoadMapSetting(NULL, NULL);
	}
	else
	{
		/*KG3D_ENVIRONMENT_EFFECT::KG3DEnvEffect* pErase =  m_EnvEffList.Erase(dwType);
		KG_PROCESS_ERROR(pErase);
		delete pErase;*/
		KG3DEnvEffect* p = m_EnvEffList.Erase(dwType);
		SAFE_DELETE(p);
	}
	return S_OK;
//Exit0:
//	return E_FAIL;
}
KG3D_ENVIRONMENT_EFFECT::KG3DEnvEffect* KG3DEnvironment::GetEnvEffect(DWORD dwType, INT nIndex)///内部使用的话，直接强转就可以得到对应的实类
{
	return m_EnvEffList.GetEnvEffect(dwType, nIndex);	
}
HRESULT KG3DEnvironment::GetEnvEffect(DWORD dwType, INT nIndex, IEKG3DEnvEffect** ppEnvEffect)
{
	KG_PROCESS_ERROR(NULL != ppEnvEffect);
	KG3DEnvEffect* pTempEffect = GetEnvEffect(dwType, nIndex);
	KG_PROCESS_ERROR (NULL != pTempEffect);
	*ppEnvEffect = pTempEffect;
	return S_OK;
Exit0:
	if (NULL != ppEnvEffect)
	{
		*ppEnvEffect = NULL;
	}
	return E_FAIL;
}

HRESULT KG3DEnvironment::GetEnvEffectCount(DWORD dwType, INT* pnCount)
{
	KG_PROCESS_ERROR(pnCount);
	*pnCount = m_EnvEffList.GetEnvEffectCount(dwType);
	return S_OK;
Exit0:
	return E_FAIL;	
}


HRESULT KG3DEnvironment::EnableControlByDynWeather( DWORD dwType, BOOL bEnable )
{
	KG3DEnvEffect* pEffect = GetEnvEffect(dwType, 0);
	KG_PROCESS_ERROR(pEffect);
	KG_PROCESS_ERROR(pEffect->EnableControlByDynamicWeather(bEnable));
	return S_OK;
Exit0:
	return E_FAIL;
}


//HRESULT KG3DEnvironment::GetAllEnvEffectThatCanBeAddedCount( INT_PTR* pnResult )
//{
//
//	KG_PROCESS_ERROR(pnResult);
//	{
//		pnResult = KG3DEnvEffect::GetTopEnvEffCount();
//
//		return S_OK;
//	}	
//Exit0:
//	return E_FAIL;
//}
//HRESULT KG3DEnvironment::GetEnvEffectThatCanBeAdded( INT nIndex, DWORD* pdwType )
//{
//	KG_PROCESS_ERROR(pdwType);
//	{
//		DWORD dwType = KG3DEnvEffect::GetTopEnvEffTypeByOrder(nIndex);
//		KG_PROCESS_ERROR(dwType != 0);
//		*pdwType = dwType;
//	}
//	return S_OK;
//Exit0:
//	return E_FAIL;
//}

KG3DEnvEffectClassProperty* KG3DEnvironment::GetFirstClassProperty()
{
	return g_GetEnvEffectClassLibrary().GetFirstTypeProperty();
}
KG3DEnvEffectClassProperty* KG3DEnvironment::GetNextClassProperty(KG3DEnvEffectClassProperty* pPrev)
{
	return g_GetEnvEffectClassLibrary().GetNextTypeProperty(pPrev);
}

ULONG KG3DEnvironment::GetTopEnvEffectCount()
{
	/*KG_PROCESS_ERROR(pnCount);
	*pnCount = (INT)m_EnvEffList.GetValidElementCount();
	return S_OK;
Exit0:
	return E_FAIL;*/
	return m_EnvEffList.GetValidElementCount();
}

HRESULT KG3DEnvironment::NewEnvEffect( DWORD dwType, IEKG3DEnvEffect** ppEnvEffect )
{
	return this->AddEnvEffect(dwType, ppEnvEffect, NULL, NULL);
}

KG3D_ENVIRONMENT_EFFECT::KG3DEnvEffFog& KG3DEnvironment::GetFog()
{
	KG3D_ENVIRONMENT_EFFECT::KG3DEnvEffFog* pFog = KGCommonBufferGetPtr(m_pFogBuffer);
#if defined(DEBUG) | defined(_DEBUG)
	KG3DEnvEffect* p = m_EnvEffList.GetEnvEffect(KG3DTYPE_ENVEFF_FOG, 0);
	_ASSERTE(NULL == p || (NULL != p && p == pFog));//p可能是NULL，在Init的时候
#endif
//#if defined(_DEBUG) | defined(DEBUG)
//	KG3D_ENVIRONMENT_EFFECT::KG3DEnvEffFog* pFogA = GetTopEnvEffect<KG3DTYPE_ENVEFF_FOG>();
//	_ASSERTE(pFog);
//	if(pFogA)
//		_ASSERTE(pFogA == pFog);
//#endif
	return *pFog;
}

KG3D_ENVIRONMENT_EFFECT::KG3DEnvEffLightSet& KG3DEnvironment::GetLightSet()
{
	KG3D_ENVIRONMENT_EFFECT::KG3DEnvEffLightSet* pLightSet = KGCommonBufferGetPtr(m_pLightSetBuffer);
#if defined(DEBUG) | defined(_DEBUG)
	KG3DEnvEffect* p = m_EnvEffList.GetEnvEffect(KG3DTYPE_ENVEFF_LIGHTSET, 0);
	_ASSERTE(NULL == p || (NULL != p && p == pLightSet));//p可能是NULL，在Init的时候
#endif
//#if defined(_DEBUG) | defined(DEBUG)
//	KG3D_ENVIRONMENT_EFFECT::KG3DEnvEffLightSet* pLightSetA = GetTopEnvEffect<KG3DTYPE_ENVEFF_LIGHTSET>();
//	_ASSERTE(pLightSet);
//	if(pLightSetA)
//		_ASSERTE(pLightSetA == pLightSet);
//#endif
	return *pLightSet;
}

VOID KG3DEnvironment::EnsureFogAndLight(IIniFile* pIniFile /*= NULL*/)
{
	HRESULT hr = E_FAIL;

	if(NULL == this->GetEnvEffect(KG3DTYPE_ENVEFF_FOG, 0) /*!this->GetTopEnvEffect<KG3DTYPE_ENVEFF_FOG>()*/)
	{
		hr = this->AddEnvEffect(KG3DTYPE_ENVEFF_FOG, NULL, pIniFile, NULL);
		_ASSERTE(SUCCEEDED(hr));
	}

	if(NULL == this->GetEnvEffect(KG3DTYPE_ENVEFF_LIGHTSET, 0)/*! this->GetTopEnvEffect<KG3DTYPE_ENVEFF_LIGHTSET>()*/)
	{
		hr= this->AddEnvEffect(KG3DTYPE_ENVEFF_LIGHTSET, NULL, pIniFile, NULL);
		_ASSERTE(SUCCEEDED(hr));
	}

}

HRESULT KG3DEnvironment::ClearEnvEffect()
{
	//UINT Handle = 0;

	//for(KG3DEnvEffect* p = m_EnvEffList.ResetIterator(); NULL != p; p = m_EnvEffList.NextIterator())
	
	KGCH::IteratorHolder holder;
	for(KG3DEnvEffect* pEnvEffect = m_EnvEffList.GetFirstTopEffect(holder)
		; NULL != pEnvEffect
		; pEnvEffect = m_EnvEffList.GetNextTopEffect(holder))
	{
		if (pEnvEffect  != KGCommonBufferGetPtr(m_pFogBuffer) && pEnvEffect  != KGCommonBufferGetPtr(m_pLightSetBuffer))
		{
			SAFE_DELETE(pEnvEffect);
		}
	}

	m_EnvEffList.Clear();//里面会释放

	return S_OK;
}

HRESULT KG3DEnvironment::Save( LPCTSTR pFilePath )
{
	IIniFile* pFile = NULL;
	KG_PROCESS_ERROR(NULL != pFilePath);
	{
		pFile = g_OpenIniFile(pFilePath, false, true);
		if(NULL == pFile)
			pFile = g_CreateIniFile();
		KG_PROCESS_ERROR(NULL != pFile);

		this->SaveMapSetting(pFile);

		BOOL bSucceeded = pFile->Save(pFilePath) != 0;
		SAFE_RELEASE(pFile);
		return  bSucceeded ? S_OK : E_FAIL;
	}
Exit0:
	SAFE_RELEASE(pFile);
	return E_FAIL;
}

HRESULT KG3DEnvironment::Reload( LPCTSTR pFilePath )
{
	IIniFile* pFile = NULL;
	KG_PROCESS_ERROR(NULL != pFilePath);
	{
		pFile = g_OpenIniFile(pFilePath);
		KG_PROCESS_ERROR(NULL != pFile);

		this->ClearEnvEffect();
		HRESULT hr = this->LoadMapSetting(pFile, NULL);

		SAFE_RELEASE(pFile);
		return  hr;
	}
Exit0:
	SAFE_RELEASE(pFile);
	return E_FAIL;
}

//IEKG3DEnvEffect* KG3DEnvironment::ResetIterator()
//{
//	return this->m_EnvEffList.ResetIterator();
//}
//
//IEKG3DEnvEffect* KG3DEnvironment::NextIterator()
//{
//	return this->m_EnvEffList.NextIterator();
//}

HRESULT KG3DEnvironment::GetFirstTopEnvEffect(KGCH::IteratorHolder& itHolder, IEKG3DEnvEffect** ppEffect)
{
	IEKG3DEnvEffect* p = m_EnvEffList.GetFirstTopEffect(itHolder);
	if (NULL != p && NULL != ppEffect)
	{
		*ppEffect = p;
		return S_OK;
	}
	return E_FAIL;
}
HRESULT KG3DEnvironment::GetNextTopEnvEffect(KGCH::IteratorHolder& itHolder, IEKG3DEnvEffect** ppEffect)
{
	IEKG3DEnvEffect* p = m_EnvEffList.GetNextTopEffect(itHolder);
	if (NULL != p && NULL != ppEffect)
	{
		*ppEffect = p;
		return S_OK;
	}
	return E_FAIL;
}