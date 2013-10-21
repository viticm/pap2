////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DEnvEffect.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong 
//  Create Date : 2006-12-8 9:44:13
//  Comment     : ///Type和ID必须由创建者自己设定，不然就没法使用  
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

///编译宏选择
#include "KG3DEnvEffCommonDef.h"
#include "KG3DEnvEffect.h"
#include "KGINIFileHelpers.h"
#include "KG3DRTTIType.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace KG3D_ENVIRONMENT_EFFECT
{
	//KG3DEnvEffect::KG3DEnvEffectClassProperty	 KG3DEnvEffect::ms_TypeTree[] = {
	//	KG3DEnvEffect::KG3DEnvEffectClassProperty(KG3DTYPE_ENVEFF_SKYSYSTEM_SYSTEM, 0, KG3DTYPE_ENVEFF_INVALID_EFFECT, -1, false),//注意ParentIndex并不能直接用于这个数组的索引
	//	KG3DEnvEffect::KG3DEnvEffectClassProperty(KG3DTYPE_ENVEFF_SKYSYSTEM_SKYBOX, -1, KG3DTYPE_ENVEFF_SKYSYSTEM_SYSTEM, 0, true),
	//	KG3DEnvEffect::KG3DEnvEffectClassProperty(KG3DTYPE_ENVEFF_SKYSYSTEM_FARMOUNTAIN, -1, KG3DTYPE_ENVEFF_SKYSYSTEM_SYSTEM, 0, true),
	//	KG3DEnvEffect::KG3DEnvEffectClassProperty(KG3DTYPE_ENVEFF_SKYSYSTEM_CLOUD, -1, KG3DTYPE_ENVEFF_SKYSYSTEM_SYSTEM, 0, true),
	//	KG3DEnvEffect::KG3DEnvEffectClassProperty(KG3DTYPE_ENVEFF_FOG, 1, KG3DTYPE_ENVEFF_INVALID_EFFECT, -1, true),
	//	KG3DEnvEffect::KG3DEnvEffectClassProperty(KG3DTYPE_ENVEFF_LENSFLARE_SUN, 2, KG3DTYPE_ENVEFF_INVALID_EFFECT, -1, true),
	//	KG3DEnvEffect::KG3DEnvEffectClassProperty(KG3DTYPE_ENVEFF_LENSFLARE_MOON, 3, KG3DTYPE_ENVEFF_INVALID_EFFECT, -1, true),
	//	KG3DEnvEffect::KG3DEnvEffectClassProperty(KG3DTYPE_ENVEFF_LIGHTSET, 4, KG3DTYPE_ENVEFF_INVALID_EFFECT, -1, true),
	//	KG3DEnvEffect::KG3DEnvEffectClassProperty(KG3DTYPE_ENVEFF_LIGHT, -1, KG3DTYPE_ENVEFF_LIGHTSET, 4, true),
	//	KG3DEnvEffect::KG3DEnvEffectClassProperty(KG3DTYPE_ENVEFF_DYNAMIC_WEATHER, 5, KG3DTYPE_ENVEFF_INVALID_EFFECT, -1, false),
	//};

    HRESULT KG3DEnvEffect::OnLostDevice()
    { 
        return S_OK;
    }

    HRESULT KG3DEnvEffect::OnResetDevice()
    {
        return S_OK; 
    }

    HRESULT KG3DEnvEffect::PostRender()
    {
        return S_OK;
    }

	//////////////////////////////////////////////////////////////////////////
	/*unsigned KG3DEnvEffect::GetTypeCount()
	{
		return _countof(ms_TypeTree);
	}*/
	/*int KG3DEnvEffect::GetTopEnvEffCount()
	{
		static int localTypeTreeRootNodeCount = 0;
		if (localTypeTreeRootNodeCount == 0)
		{
			int nTempCount = 0;
			for (int i = 0; i < _countof(ms_TypeTree); i++)
			{
				if(ms_TypeTree[i].m_dwParentType == KG3DTYPE_ENVEFF_INVALID_EFFECT)
				{
					nTempCount++;
					_ASSERTE(ms_TypeTree[i].m_nParentIndex == -1);
				}
			}
			localTypeTreeRootNodeCount = nTempCount;
		}
		_ASSERTE(localTypeTreeRootNodeCount > 0);
		return localTypeTreeRootNodeCount;
	}*/
	//KG3DEnvEffect::KG3DEnvEffectClassProperty& KG3DEnvEffect::GetTopEnvEffTypeProperty( int nIndex )
	//{
	//	//静态的缓存Top的部分
	//	static KG3DEnvEffectClassProperty localTreeRootNodes[_countof(ms_TypeTree)];
	//	static int localTreeRootNodesAdded = 0;
	//	if (localTreeRootNodesAdded == 0)
	//	{
	//		ZeroMemory(localTreeRootNodes, sizeof(KG3DEnvEffectClassProperty)*_countof(ms_TypeTree));
	//		int nAdded = 0;
	//		for (int i = 0; i < _countof(ms_TypeTree); i++)
	//		{
	//			if (ms_TypeTree[i].m_dwParentType == KG3DTYPE_ENVEFF_INVALID_EFFECT)//无父亲，那就是Top的
	//			{
	//				_ASSERTE(ms_TypeTree[i].m_nParentIndex == -1);
	//				localTreeRootNodes[nAdded] = ms_TypeTree[i];
	//				nAdded++;
	//			}
	//		}
	//		localTreeRootNodesAdded = nAdded;
	//		_ASSERTE(localTreeRootNodesAdded == GetTopEnvEffCount());
	//	}
	//	_ASSERTE(nIndex >= 0 && nIndex < localTreeRootNodesAdded && localTreeRootNodes[nIndex].m_dwType != 0);
	//	return localTreeRootNodes[nIndex];
	//}	
	
	/*DWORD KG3DEnvEffect::GetTopEnvEffTypeByOrder( int nIndex )
	{
		if (nIndex < 0 || nIndex >= GetTopEnvEffCount())
		{
			return 0;
		}
		KG3DEnvEffectClassProperty& Property = GetTopEnvEffTypeProperty(nIndex);
		_ASSERTE(Property.m_dwType != 0 
			&& Property.m_dwParentType == KG3DTYPE_ENVEFF_INVALID_EFFECT && Property.m_nParentIndex == -1);
		return Property.m_dwType;
	}*/

	//unsigned KG3DEnvEffect::GetTypePos( DWORD dwType )
	//{
	//	//二分查找
	//	unsigned uLower = 0;
	//	unsigned uUpper = _countof(ms_TypeTree);
	//	while (uLower <= uUpper)
	//	{
	//		unsigned uMiddle = (uLower + uUpper)/2;
	//		DWORD dwTypeMid = ms_TypeTree[uMiddle].m_dwType;
	//		if(dwTypeMid == dwType)
	//		{
	//			return uMiddle; 
	//		}
	//		if (dwType < dwTypeMid)
	//		{
	//			uUpper = uMiddle - 1;
	//		} 
	//		else
	//		{
	//			uLower = uMiddle + 1;
	//		}
	//	}
	//	return UINT_MAX;
	//}

	/*unsigned KG3DEnvEffect::GetTypeHandleByOrder( unsigned order )
	{
		return order;
	}*/

//	VOID KG3DEnvEffect::TypePropertyTreeTest(unsigned uInnerArraySize)
//	{
//		for (int i  = 0; i < _countof(ms_TypeTree); i++)
//		{
//			_ASSERTE(ms_TypeTree[i].m_dwType != 0);
//			if (ms_TypeTree[i].m_dwParentType == KG3DTYPE_ENVEFF_INVALID_EFFECT)///保证没有越界的元素
//			{
//				_ASSERTE(ms_TypeTree[i].m_nIndex >= 0 
//					&& ms_TypeTree[i].m_nIndex < _countof(ms_TypeTree) 
//					&& ms_TypeTree[i].m_nIndex < (int)uInnerArraySize);
//				
//				_ASSERTE(ms_TypeTree[i].m_dwParentType == KG3DTYPE_ENVEFF_INVALID_EFFECT);
//				_ASSERTE(ms_TypeTree[i].m_nParentIndex == -1);
//			}
//			else
//			{
//				_ASSERTE(ms_TypeTree[i].m_nIndex == -1);
//				
//				_ASSERTE(ms_TypeTree[i].m_nParentIndex >= 0 && ms_TypeTree[i].m_nParentIndex < (int)uInnerArraySize);
//
//				for (int x = 0; x < _countof(ms_TypeTree); x++)
//				{
//					if (ms_TypeTree[x].m_dwType == ms_TypeTree[i].m_dwParentType)
//					{
//						_ASSERTE(ms_TypeTree[i].m_nParentIndex == ms_TypeTree[x].m_nIndex);
//					}
//				}				
//			}
//		}
//		for (size_t i = 0, j = 1; i < _countof(ms_TypeTree) - 1; i++, j++)///要减1，因为是前后比较
//		{
//			_ASSERTE(ms_TypeTree[i].m_dwType < ms_TypeTree[j].m_dwType);///保证他们是恒增的
//		}
//		for (size_t i = 0; i < _countof(ms_TypeTree); i++)
//		{
//			_ASSERTE(i == GetTypePos(ms_TypeTree[i].m_dwType));//检查函数结果
//		}
//
//		for (unsigned i = 0; i < _countof(ms_TypeTree); i++)
//		{
//#if defined(_DEBUG) | defined(DEBUG)
//			unsigned Handle = GetTypeHandle(ms_TypeTree[i].m_dwType);
//#else
//			GetTypeHandle(ms_TypeTree[i].m_dwType);
//#endif
//				
//			_ASSERTE(Handle == i);
//			_ASSERTE(IsValidTypeHandle(i));
//			_ASSERTE(IsTypeValid(ms_TypeTree[i].m_dwType));
//
//			if (ms_TypeTree[i].m_dwParentType == KG3DTYPE_ENVEFF_INVALID_EFFECT)
//			{
//				_ASSERTE(IsTypeTopEnvEff(Handle));
//				//_ASSERTE(GetTopTypeIndex(Handle) == ms_TypeTree[i].m_nIndex);
//			}
//			else
//			{
//				//_ASSERTE(GetTypeParentIndex(Handle) == ms_TypeTree[i].m_nParentIndex);
//				//_ASSERTE(GetTypeParentType(Handle) == ms_TypeTree[i].m_dwParentType);
//			}
//		}
//
//	}


	//////////////////////////////////////////////////////////////////////////
	KG3DEnvEffect::KG3DEnvEffect(int nID, KG3DENVEFF_TYPE emType)
		:m_nID(-1)
		,m_dwType(0)
		//,m_dwState(0)
		//,m_pKeyTable(NULL)
	{		
#if defined(DEBUG) | defined(_DEBUG)
#if _KG_DEBUG_ENABLE_LEAK_TEST == 1
		///<LEAK_TEST>
		CHAR* __pLeakTest1 = new CHAR[4];///内存泄漏测试
		INT* __pLeakTest2 = new INT;///内存泄漏测试
#endif
#endif

		if (m_nID == -1 && m_dwType == 0)
		{
			m_nID = nID;
			m_dwType = emType;

			m_BitSet.SetTrue<EM_IS_ID_AND_TYPE_INITIALIZED>();
			_ASSERTE(m_BitSet.Is<EM_IS_ID_AND_TYPE_INITIALIZED>());
			this->Enable(TRUE);
			_ASSERTE(this->IsEnable());

			KG3DEnvEffectClassProperty* pProperty = g_GetEnvEffectClassLibrary().GetProperty(m_dwType);
			
#if defined(DEBUG) | defined(_DEBUG)
			{
#ifdef KG_ENABLE_TDD
				if(! TDD::IsTDDWorking())	//在TDD运行的时候，Property还没有初始化
				{
					_ASSERTE(NULL != pProperty);
				}
#else
				_ASSERTE(NULL != pProperty);
#endif
			}
#endif

			if (NULL != pProperty)
			{
				if(pProperty->m_bIsDWGCAvailable)
				{
					m_BitSet.SetTrue<EM_IS_DWGC_AVAILABLE>();
					this->SetEnableControlByDynamicWeather(TRUE);//有关键帧的效果，默认允许DynamicWeather控制

					_ASSERTE(this->IsEnableControlByDynamicWeather());
					_ASSERTE(this->IsDWGCAvailable());
				}
				else
				{
					_ASSERTE(! this->IsEnableControlByDynamicWeather());
					_ASSERTE(! this->IsDWGCAvailable());
				}
			}			
			return;
		}
		_ASSERTE(NULL&&"非法多次调用，这个东西是由Environment处理的");	
		
	}
	void KG3DEnvEffect::SetEnableControlByDynamicWeather( BOOL bEnable )
	{
		if (this->IsDWGCAvailable())//有关键帧的才能被DW控制
		{
			m_BitSet.Set<EM_ENABLE_CONTROL_BY_DW>(bEnable);
		}
	}
	KG3DEnvEffect::~KG3DEnvEffect()
	{
		if (m_pKeyTable)
		{
			if(m_pKeyTableGenerator)
			{
				bool bRet = m_pKeyTableGenerator->ReleaseTable(GetPtr(m_pKeyTable));
				_ASSERTE(bRet);
				(void)bRet;
			}
		}
	}
	BOOL KG3DEnvEffect::SetEnable( BOOL bEnable )
	{
		KG_PROCESS_ERROR(m_BitSet.Is<EM_IS_ID_AND_TYPE_INITIALIZED>());
		m_BitSet.Set<EM_ENABLE>(bEnable);
		_ASSERTE(bEnable ? m_BitSet.Is<EM_ENABLE>() : ! m_BitSet.Is<EM_ENABLE>());
Exit0:
		return IsEnable();
	}
	HRESULT	KG3DEnvEffect::Enable(BOOL bEnable)
	{
		SetEnable(bEnable);
		if (bEnable)
		{
			KG_PROCESS_ERROR(this->IsEnable());
		}
		else
		{
			KG_PROCESS_ERROR(! this->IsEnable());
		}
		return S_OK;
Exit0:
		return E_FAIL;
	}
	HRESULT KG3DEnvEffect::IsEnable(BOOL* pbIsEnable)
	{
		if (NULL == pbIsEnable)
		{
			return E_INVALIDARG;
		}
		*pbIsEnable = IsEnable();
		return S_OK;
	}

	
	/*HRESULT	KG3DEnvEffect::GetType(DWORD* pdwType)
	{
		if (NULL == pdwType)
		{
			return E_INVALIDARG;
		}
		*pdwType = m_dwType;
		return S_OK;
	}*/
	HRESULT KG3DEnvEffect::GetEnvEffName(LPTSTR tszName, INT nBufferSize)
	{
		if (NULL == tszName || nBufferSize < MAX_NAME)
		{
			return E_INVALIDARG;
		}
		KG3DRTTITypeManager* pGlobleRTTIManager = g_cGetRTTITypeManager();
		KG3DRTTIType* pTempType = NULL;
		KG_PROCESS_ERROR(pGlobleRTTIManager);
		pTempType = pGlobleRTTIManager->GetType(this->GetType());
		KG_PROCESS_ERROR(pTempType);
		_tcscpy_s(tszName, nBufferSize, pTempType->Name());
		return S_OK;
Exit0:
		return E_FAIL;
	}	
	HRESULT	KG3DEnvEffect::GetID(int*	nID)
	{
		if (NULL == nID)
		{
			return E_INVALIDARG;
		}
		*nID = m_nID;
		return S_OK;
	}

	HRESULT KG3DEnvEffect::GetSubEffect( INT nIndex, IEKG3DEnvEffect** ppEnvEffect, KG3DENVEFF_HANDLE* pHandle)
	{
		
		{
			KG3DEnvEffect* pEnvEffect = this->GetSubEffect(nIndex, pHandle);
			KG_PROCESS_ERROR(pEnvEffect);
#if defined(DEBUG) | defined(_DEBUG)
			if (pHandle)
			{
				_ASSERTE(pHandle->m_dwType != KG3DTYPE_ENVEFF_INVALID_EFFECT);
			}
#endif
			if(ppEnvEffect)
				*ppEnvEffect = pEnvEffect;
		}
		return S_OK;
Exit0:
		return E_FAIL;
	}

	HRESULT KG3DEnvEffect::GetSubEffectCount( INT* pnCount )
	{
		KG_PROCESS_ERROR(pnCount);
		*pnCount = this->GetSubEffectCount();
		return S_OK;
Exit0:
		return E_FAIL;
	}

	HRESULT KG3DEnvEffect::ToSolidInterface( LPVOID* pEffectInterface )
	{
		_ASSERTE(NULL&&"这个方法应该被覆盖的");
		if (pEffectInterface)
		{
			*pEffectInterface = NULL;
		}
		return E_FAIL;
	}	

	HRESULT KG3DEnvEffect::EnableControlByDynamicWeather( BOOL bEnable )
	{
		KG_PROCESS_ERROR(m_dwType != KG3DTYPE_ENVEFF_DYNAMIC_WEATHER);///动态天气是不能被自己控制的
		{
			SetEnableControlByDynamicWeather(bEnable);
			
			INT nSubEffectCount = this->GetSubEffectCount();
			if (nSubEffectCount > 0)
			{
				for (INT i = 0; i < nSubEffectCount; i++)
				{
					KG3DEnvEffect* pSubEffect = this->GetSubEffect(i);
					_ASSERTE(pSubEffect && pSubEffect != this);
					pSubEffect->EnableControlByDynamicWeather(bEnable);
				}
			}
		}
		return S_OK;
Exit0:
		return E_FAIL;
	}

	HRESULT KG3DEnvEffect::IsEnableControlByDynamicWeather( BOOL* pbEnable ) ///这个可以不在派生类中覆盖
	{
		if (NULL == pbEnable)
		{
			return E_FAIL;
		}
		*pbEnable = IsEnableControlByDynamicWeather() ? TRUE : FALSE;
#if defined(_DEBUG) | defined(DEBUG)
		if (m_dwType == KG3DTYPE_ENVEFF_DYNAMIC_WEATHER)
		{
			_ASSERTE(*pbEnable == FALSE);
		}
#endif
		return S_OK;
	}

	KG3DDWGC_KeyTableBase* KG3DEnvEffect::GetKeyTable()
	{
#if defined(DEBUG) | defined(_DEBUG)
		if(m_pKeyTable)
		{
			_ASSERTE(m_pKeyTableGenerator);		
		}
#endif
		return GetPtr(m_pKeyTable);
	}

	KG3DDWGC_KeyTableBase* KG3DEnvEffect::SetKeyTable( KG3DDWGC_KeyTableGeneratorBase& Gen, KG3DDWGC_KeyTableBase& KeyTable )
	{
		_ASSERTE(this->IsDWGCAvailable());
		KG_PROCESS_ERROR(this->IsDWGCAvailable());
		_ASSERTE(this->GetType() == Gen.GetType());
		_ASSERTE(! m_pKeyTableGenerator);
		_ASSERTE(this->GetType() == KeyTable.GetType());
		_ASSERTE(! m_pKeyTable);
		m_pKeyTable = &KeyTable;
		m_pKeyTableGenerator = &Gen;
		_ASSERTE(m_pKeyTableGenerator);
		return GetPtr(m_pKeyTable);
Exit0:
		_ASSERTE(! m_pKeyTable);
		return NULL;
	}

	std::tstring KG3DEnvEffect::GetHandleString( KG3DENVEFF_HANDLE Handle )
	{
		TCHAR Temp[MAX_PATH] = _T("");
		_stprintf_s(Temp, _T("%u_%d"), Handle.m_dwType, Handle.m_nIndex);
		return Temp;
	}

	KG3DENVEFF_HANDLE KG3DEnvEffect::GetHandleFromString( LPCTSTR Str )
	{
		TCHAR TempStr[MAX_PATH] = _T("");
		errno_t erro = _tcscpy_s(TempStr, Str);
		KG_PROCESS_ERROR(erro == 0);
		{
			TCHAR* p = TempStr;
			size_t Len = _tcslen(TempStr);
			for (size_t i = 0; *p != '\0'; ++i, ++p)
			{
				if(*p == '_')
					break;
			}
			KG_PROCESS_ERROR(p != TempStr);
			*(p) = '\0';
			KG3DENVEFF_HANDLE Temp;
			UINT TempType;
			BOOL bRet = KG_CUSTOM_HELPERS::TStringToValue(TempStr, static_cast<INT>(p - TempStr + 1), TempType);
			KG_PROCESS_ERROR(bRet);
			Temp.m_dwType = static_cast<DWORD>(TempType);
			bRet = KG_CUSTOM_HELPERS::TStringToValue(p+1, (INT)Len - static_cast<INT>(p - Str), Temp.m_nIndex);
			KG_PROCESS_ERROR(bRet);

			return Temp;
		}
Exit0:
		return KG3DENVEFF_HANDLE(KG3DTYPE_ENVEFF_INVALID_EFFECT, 0);

	}

	ULONG KG3DEnvEffect::GetState(size_t WhichState)
	{
		return m_BitSet.GetBOOL(WhichState) ? TRUE : FALSE;
	}
	
	
	
#if defined(KG_ENABLE_TDD)
KG_TEST_BEGIN(KG3DEnvEffect)
		{
			KG3DENVEFF_HANDLE TempHandle(KG3DTYPE_ENVEFF_RAIN, 1);


			std::tstring Str= KG3DEnvEffect::GetHandleString(TempHandle);
			_ASSERTE(Str == std::tstring(_T("2080_1")));

			_ASSERTE(KG3DEnvEffect::GetHandleFromString(Str.c_str()) == TempHandle);

			//_KG3D_DEBUG_REPORT("KG3DEnvEffect RunTime Test Passed, Congratulations!");
		}
KG_TEST_END(KG3DEnvEffect)
#endif

	KG3DEnvEffectDummy::KG3DEnvEffectDummy() 
	:KG3DEnvEffect(0, KG3DTYPE_ENVEFF_DUMMY)
	{

	}

//////////////////////////////////////////////////////////////////////////
	KG3DEnvEffectClassLibrary::KG3DEnvEffectClassLibrary()
	:m_uCount(0)
	{
		ZeroMemory(m_arrayProperties, _countof(m_arrayProperties) * sizeof(KG3DEnvEffectClassProperty));		
	}

	ULONG KG3DEnvEffectClassLibrary::GetTypeCount()
	{
		return static_cast<ULONG>(m_uCount);
	}

	KG3DEnvEffectClassProperty* KG3DEnvEffectClassLibrary::GetProperty( DWORD dwType )
	{
		for (size_t i = 0; i < _countof(m_arrayProperties); ++i)
		{
			if (m_arrayProperties[i].m_dwType == dwType)
			{
				return &m_arrayProperties[i];
			}
		}
		return NULL;
	}

	BOOL KG3DEnvEffectClassLibrary::IsTypeTop( DWORD dwType )
	{
		KG3DEnvEffectClassProperty* pProperty = GetProperty(dwType);
		if (NULL != pProperty)
		{
			return pProperty->m_dwParentType == KG3DTYPE_ENVEFF_INVALID_EFFECT;
		}
		return FALSE;
	}

	KG3DEnvEffectClassProperty* KG3DEnvEffectClassLibrary::GetFirstTypeProperty()
	{
		return &m_arrayProperties[0];
	}

	KG3DEnvEffectClassProperty* KG3DEnvEffectClassLibrary::GetNextTypeProperty( KG3DEnvEffectClassProperty* pPreProperty )
	{
		if(NULL != pPreProperty && pPreProperty < static_cast<KG3DEnvEffectClassProperty*>(m_arrayProperties) + _countof(m_arrayProperties))
		{
			return pPreProperty + 1;
		}
		return NULL;
	}

	BOOL KG3DEnvEffectClassLibrary::RegisterType( KG3DEnvEffectClassProperty& propertyInput )
	{
		_ASSERTE(m_uCount < _countof(m_arrayProperties));
		if(m_uCount > _countof(m_arrayProperties) - 1)
			return FALSE;
		m_arrayProperties[m_uCount] = propertyInput;
		++m_uCount;
		return TRUE;
	}

	KG3DEnvEffectClassLibrary& g_GetEnvEffectClassLibrary()
	{
		static KG3DEnvEffectClassLibrary s_instance;
		return s_instance;
	}

#if defined(KG_ENABLE_TDD)
KG_TEST_BEGIN(KG3DEnvEffectClassLibrary)
{
	KG3DEnvEffectClassLibrary testLib;
	_ASSERTE(0 == testLib.GetTypeCount());

	KG3DEnvEffectClassProperty property1 = {KG3DTYPE_ENVEFF_SKYSYSTEM_SYSTEM, KG3DTYPE_ENVEFF_INVALID_EFFECT, false};
	KG3DEnvEffectClassProperty property2 = {KG3DTYPE_ENVEFF_SKYSYSTEM_CLOUD, KG3DTYPE_ENVEFF_SKYSYSTEM_SYSTEM, true};

	BOOL bRet = testLib.RegisterType(property1);
	_ASSERTE(bRet);
	bRet = testLib.RegisterType(property2);
	_ASSERTE(bRet);

	_ASSERTE(2 == testLib.GetTypeCount());
	KG3DEnvEffectClassProperty* pPrpt = testLib.GetFirstTypeProperty();
	_ASSERTE(NULL != pPrpt && pPrpt->m_dwType == KG3DTYPE_ENVEFF_SKYSYSTEM_SYSTEM);
	pPrpt = testLib.GetNextTypeProperty(pPrpt);
	_ASSERTE(NULL != pPrpt && pPrpt->m_dwType == KG3DTYPE_ENVEFF_SKYSYSTEM_CLOUD);


	_ASSERTE(testLib.IsTypeTop(KG3DTYPE_ENVEFF_SKYSYSTEM_SYSTEM));
	_ASSERTE(! testLib.IsTypeTop(KG3DTYPE_ENVEFF_SKYSYSTEM_CLOUD));

	_ASSERTE(testLib.GetProperty(KG3DTYPE_ENVEFF_SKYSYSTEM_SYSTEM) 
		&& testLib.GetProperty(KG3DTYPE_ENVEFF_SKYSYSTEM_SYSTEM)->m_dwType == KG3DTYPE_ENVEFF_SKYSYSTEM_SYSTEM);
}
KG_TEST_END(KG3DEnvEffectClassLibrary)
#endif
};
////////////////////////////////////////////////////////////////////////////////

