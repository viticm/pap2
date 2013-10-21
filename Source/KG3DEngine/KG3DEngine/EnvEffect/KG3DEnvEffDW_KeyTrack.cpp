////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DEnvEffDW_KeyTrack.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-4-28 14:22:04
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

///编译宏选择
#include "KG3DEnvEffCommonDef.h"

#include "KG3DSkySystem.h"
#include "KG3DEnvEffFog.h"
#include "KG3DMeshLensFlare.h"
#include "KG3DEnvEffLight.h"
#include "KG3DEnvEffLightSet.h"
#include "KG3DEnvEffStarryNight.h"

#include "KG3DEnvEffDW_KeyTrack.h"
#include "IEKG3DEnvironment.h"
#include "IEKG3DEnvEffDynamicWeather.h"
#include "KG3DEnvEffDW_HelperClasses.h"

#include "KG3DEnvEffDW_KeyTableGenerator.h"
//#include "FileType.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
////////////////////////////////////////////////////////////////////////////////
namespace
{
	inline int GetNextHour(int nHour)
	{
		_ASSERT(nHour >= KG3DENVEFFTI_HOUR_MIN && nHour <= KG3DENVEFFTI_HOUR_MAX);
		++nHour;
		return (nHour > KG3DENVEFFTI_HOUR_MAX) ? KG3DENVEFFTI_HOUR_MIN : nHour; 
	}
	inline int GetPrevHour(int nHour)
	{
		_ASSERT(nHour >= KG3DENVEFFTI_HOUR_MIN && nHour <= KG3DENVEFFTI_HOUR_MAX);
		--nHour;
		return (nHour < KG3DENVEFFTI_HOUR_MIN) ? KG3DENVEFFTI_HOUR_MAX : nHour;
	}	
};
namespace KG3D_ENVIRONMENT_EFFECT
{
	template<typename _KeyType>
	struct TypeTableAdder 
	{
		TypeTableAdder(KG3DDWGC_KeyTrack::TypeTableGens& TableGenContainer)
		{
			enum
			{
				DWGCKEY_TYPE = _KeyType::DWGCKEY_TYPE,
			};
			_KG3D_DEBUG_STATIC_CHECK_T(DWGCKEY_TYPE > KG3DTYPE_ENVEFF_BEGIN && DWGCKEY_TYPE < KG3DTYPE_ENVEFF_END, TypeError);

			//_ASSERTE(KG3DEnvEffect::IsTypeDWGCAvailable(KG3DEnvEffect::GetTypeHandle(DWGCKEY_TYPE)));
#ifdef KG_ENABLE_TDD
			if(! TDD::IsTDDWorking())
#endif
			{
				_ASSERTE(NULL != g_GetEnvEffectClassLibrary().GetProperty(DWGCKEY_TYPE)
					&& g_GetEnvEffectClassLibrary().GetProperty(DWGCKEY_TYPE)->m_bIsDWGCAvailable);
			}

			size_t insertPos = KG3DDWGC_KeyTrack::GetVecPos(DWGCKEY_TYPE);
			_ASSERTE(TableGenContainer.size() > insertPos);
			KG3DDWGC_KeyTableGeneratorBase* pKeyTableGen = new TKG3DDWGC_KeyTableGenerator<TKG3DDWGC_KeyTable<_KeyType> >();
			if(pKeyTableGen)
				TableGenContainer[insertPos] = pKeyTableGen;
		}
	};	

	VOID KG3DDWGC_KeyTrack::AddKeyTables()
	{
		_KG3D_DEBUG_STATIC_CHECK_T(all_enveff_count < 110, EnvEffBeginEndError);
		_ASSERTE(m_vecTableGens.size() == all_enveff_count);
		
#define ADD_ENVEFF_TO_TABLE_CONTAINER(keytype)	{TypeTableAdder<keytype>(this->m_vecTableGens);}NULL
		///<KG3DEnvEffect SubClass Register Point>
		ADD_ENVEFF_TO_TABLE_CONTAINER(KG3DDWGC_Key_SkyBox);
		ADD_ENVEFF_TO_TABLE_CONTAINER(KG3DDWGC_Key_FarMount);
		ADD_ENVEFF_TO_TABLE_CONTAINER(KG3DDWGC_Key_Cloud);
		//ADD_ENVEFF_TO_TABLE_CONTAINER(KG3DDWGC_Key_SkySystem);
		ADD_ENVEFF_TO_TABLE_CONTAINER(KG3DDWGC_Key_Fog);
		ADD_ENVEFF_TO_TABLE_CONTAINER(KG3DDWGC_Key_LensFlareSun);
		ADD_ENVEFF_TO_TABLE_CONTAINER(KG3DDWGC_Key_LensFlareMoon);
		ADD_ENVEFF_TO_TABLE_CONTAINER(KG3DDWGC_Key_Light);
		ADD_ENVEFF_TO_TABLE_CONTAINER(KG3DDWGC_Key_LightSet);
		//ADD_ENVEFF_TO_TABLE_CONTAINER(KG3DDWGC_Key_StarryNight);
	}

	


	KG3DDWGC_KeyTrack::KG3DDWGC_KeyTrack(KG3DEnvEffListBase& List)
		:m_vecTableGens((size_t)(all_enveff_count), NULL)//填入N个空指针
		,m_EnvEffList(List)
	{
		AddKeyTables();
	}
	
	static KG3DEnvEffListDummy stEnvEffListDummy;
	KG3DDWGC_KeyTrack::KG3DDWGC_KeyTrack()
		:m_vecTableGens((size_t)(all_enveff_count), NULL)//填入N个空指针
		,m_EnvEffList(stEnvEffListDummy)
	{
		AddKeyTables();		
	}
	KG3DDWGC_KeyTrack::~KG3DDWGC_KeyTrack()
	{
		KG_CUSTOM_HELPERS::TContainerDelete(m_vecTableGens);//记得检查虚析构，虚析构是必须的
	}
	
	size_t KG3DDWGC_KeyTrack::GetAllTableCount()
	{
		size_t Ret = 0;
		for (TypeTableGens::iterator it = m_vecTableGens.begin(); it != m_vecTableGens.end(); ++it)
		{
			KG3DDWGC_KeyTableGeneratorBase* p = *it;
			if(p)
				Ret +=  p->Size();
		}
		return Ret;
	}

	VOID KG3DDWGC_KeyTrack::GetTypesOfAllExistTables( std::vector<DWORD>& vecResult )
	{
		for (TypeTableGens::iterator it = m_vecTableGens.begin(); it != m_vecTableGens.end(); ++it)
		{
			KG3DDWGC_KeyTableGeneratorBase* pTableGen = *it;
			if(! pTableGen)
				continue;
			DWORD dwType = pTableGen->GetType();
			_ASSERTE(dwType != 0);
			vecResult.push_back(dwType);
		}
	}

	KG3DDWGC_KeyTableBase* KG3DDWGC_KeyTrack::GetTable(KG3DENVEFF_HANDLE Handle)
	{
		KG_PROCESS_ERROR(Handle.m_dwType > KG3DTYPE_ENVEFF_BEGIN && Handle.m_dwType < KG3DTYPE_ENVEFF_END);
		{
#if defined(DEBUG) | defined(_DEBUG)	//用于给cpp中的测试运行的代码
			if (&m_EnvEffList == &stEnvEffListDummy)
			{
				KG3DDWGC_KeyTableGeneratorBase* pGen = GetTableGen(Handle.m_dwType);
				if(! pGen)
					return NULL;
				if (pGen->Size() == 0)
				{
					return pGen->RequestANewTable();
				}
				return &pGen->GetTable(0);
				
			}
#endif
			KG3DEnvEffect* pEffect = m_EnvEffList.GetEnvEffect(Handle.m_dwType, Handle.m_nIndex);
			KG_PROCESS_ERROR(pEffect);

			KG3DDWGC_KeyTableBase* pRet = pEffect->GetKeyTable();
			if(pRet)
				return pRet;

			//如果当前Effect没有绑定Table，那么加一个上去
			KG3DDWGC_KeyTableGeneratorBase* pGen = GetTableGen(Handle.m_dwType);
			KG_PROCESS_ERROR(pGen);

			pRet = pGen->RequestANewTable();
			KG_PROCESS_ERROR(pRet);

			return pEffect->SetKeyTable(*pGen, *pRet);
		}
Exit0:
		return NULL;
	}

	KG3DDWGC_KeyTableGeneratorBase* KG3DDWGC_KeyTrack::GetTableGen( DWORD dwType )
	{
		size_t uPos = GetVecPos(dwType);
		KG_PROCESS_ERROR(uPos < m_vecTableGens.size());
		return m_vecTableGens[uPos];
Exit0:
		return NULL;
	}
	//<Region管理>
	HRESULT KG3DDWGC_KeyTrack::AddRegionIndex( INT RegionIndex )
	{
		if(! CheckIsRegionIndexValid(RegionIndex))
			return E_FAIL;
		_ASSERTE(m_setRegionIndex.size() < KG3DENVEFFRI_REGION_COUNT_MAX);
		TypeRegionIndexSet::_Pairib pib = m_setRegionIndex.insert(RegionIndex); return pib.second ? S_OK : E_FAIL;
	}

	HRESULT KG3DDWGC_KeyTrack::DelRegionIndex( INT RegionIndex )
	{
		TypeRegionIndexSet::iterator itFinded = m_setRegionIndex.find(RegionIndex);
		KG_PROCESS_ERROR(itFinded != m_setRegionIndex.end());
		{
			TypeTableGens::iterator itEnd = m_vecTableGens.end();
			for (TypeTableGens::iterator it = m_vecTableGens.begin(); it != itEnd; ++it)
			{
				KG3DDWGC_KeyTableGeneratorBase* pTableGen = *it;
				if(! pTableGen)
					continue;
				for (size_t i = 0; i < pTableGen->Size(); ++i)
				{
					pTableGen->GetTable(i).DelRegion(RegionIndex);
				}
			}
		}
		m_setRegionIndex.erase(itFinded);
		return S_OK;
Exit0:
		return E_FAIL;
	}
	HRESULT KG3DDWGC_KeyTrack::ModifyRegionIndex( INT RegionIndex, INT NewIndex )
	{
		HRESULT hr = E_FAIL;
		TypeRegionIndexSet::iterator itOld = m_setRegionIndex.find(RegionIndex);
		TypeRegionIndexSet::iterator itNew = m_setRegionIndex.find(RegionIndex);
		KG_PROCESS_ERROR(itOld != m_setRegionIndex.end());
		KG_PROCESS_ERROR(itNew != m_setRegionIndex.end());
		KG_PROCESS_ERROR(CheckIsRegionIndexValid(RegionIndex) && CheckIsRegionIndexValid(NewIndex));
		{
			TypeTableGens::iterator itEnd = m_vecTableGens.end();
			for (TypeTableGens::iterator it = m_vecTableGens.begin(); it != itEnd; ++it)
			{
				KG3DDWGC_KeyTableGeneratorBase* pTableGen = *it;
				if(! pTableGen)
					continue;
				for (size_t i = 0; i < pTableGen->Size(); ++i)
				{
					KG_PROCESS_ERROR(! pTableGen->GetTable(i).IsAnyKeyInRegion(NewIndex));
				}
			}

			for (TypeTableGens::iterator it = m_vecTableGens.begin(); it != itEnd; ++it)
			{
				KG3DDWGC_KeyTableGeneratorBase* pTableGen = *it;
				if(! pTableGen)
					continue;
				for (size_t i = 0; i < pTableGen->Size(); ++i)
				{
					hr = pTableGen->GetTable(i).MoveAllKeysInRegionToNewRegion(RegionIndex, NewIndex);
					_ASSERTE(SUCCEEDED(hr));
				}
			}
		}
		m_setRegionIndex.erase(itOld);
		m_setRegionIndex.insert(NewIndex);
		return S_OK;
Exit0:
		return E_FAIL;
	}

	HRESULT KG3DDWGC_KeyTrack::ClearRegionIndices()
	{
		m_setRegionIndex.clear();
		TypeTableGens::iterator itEnd = m_vecTableGens.end();
		for (TypeTableGens::iterator it = m_vecTableGens.begin(); it != itEnd; ++it)
		{
			KG3DDWGC_KeyTableGeneratorBase* pTableGen = *it;
			if(! pTableGen)
				continue;
			for (size_t i = 0; i < pTableGen->Size(); ++i)
			{
				pTableGen->GetTable(i).Clear();
			}
		}

		return S_OK;
	}

	HRESULT KG3DDWGC_KeyTrack::GetFirstRegionIndex( INT* pRegionIndex )
	{
		if(pRegionIndex && ! m_setRegionIndex.empty())
		{
			*pRegionIndex = *(m_setRegionIndex.begin());
			return S_OK;
		}
		return E_FAIL;
	}

	HRESULT KG3DDWGC_KeyTrack::GetNextRegionIndex( INT PrevRegionIndex, INT* pNextRegionIndex )
	{
		TypeRegionIndexSet::iterator it = m_setRegionIndex.find(PrevRegionIndex);
		if (pNextRegionIndex && it != m_setRegionIndex.end())
		{
			++it;
			if(it != m_setRegionIndex.end())
			{
				*pNextRegionIndex = *it;
				return S_OK;
			}
		}
		return E_FAIL;
	}

	KG3DDWGC_KeyBase* KG3DDWGC_KeyTrack::NewKey( KG3DENVEFF_HANDLE Handle, KG3DENVEFF_DWGCKEY_HANDLE KeyHandle )
	{
		KG_PROCESS_ERROR(IsRegionAllowed(KeyHandle.m_Region));//限制不能随便增加Region，不然在表中表现出来很麻烦
		{
			KG3DDWGC_KeyTableBase* pKeyTable = GetTable(Handle);
			KG_PROCESS_ERROR(pKeyTable);
			return pKeyTable->New(KeyHandle);
		}
Exit0:
		return NULL;
	}

	KG3DDWGC_KeyBase* KG3DDWGC_KeyTrack::GetKey( KG3DENVEFF_HANDLE Handle, KG3DENVEFF_DWGCKEY_HANDLE KeyHandle )
	{
		KG3DDWGC_KeyTableBase* pKeyTable = GetTable(Handle);
		KG_PROCESS_ERROR(pKeyTable);
		return pKeyTable->Get(KeyHandle);
Exit0:
		return NULL;
	}
	std::auto_ptr<KG3DDWGC_KeyBase> KG3DDWGC_KeyTrack::GetInterpKey(KG3DENVEFF_HANDLE Handle, KG3DENVEFF_DWGCKEY_HANDLE KeyHandle)
	{
		KG_PROCESS_ERROR(CheckIsKeyHandleValid(KeyHandle));
		{
			KG3DDWGC_KeyTableBase* pKeyTable = GetTable(Handle);
			KG_PROCESS_ERROR(pKeyTable);
			std::auto_ptr<KG3DDWGC_KeyBase> auto_p= pKeyTable->NewKey();
			_ASSERTE(auto_p.get());
			HRESULT hr = pKeyTable->GetInterpKey(KeyHandle, *auto_p);
			KG_COM_PROCESS_ERROR(hr);
			return auto_p;
		}
Exit0:
		return std::auto_ptr<KG3DDWGC_KeyBase>(NULL);
	}
	VOID KG3DDWGC_KeyTrack::DelKey(KG3DENVEFF_HANDLE Handle, KG3DENVEFF_DWGCKEY_HANDLE KeyHandle )
	{
		KG3DDWGC_KeyTableBase* pKeyTable = GetTable(Handle);
		KG_PROCESS_ERROR(pKeyTable);
		pKeyTable->Del(KeyHandle);
Exit0:
		return;
	}

	ULONG KG3DDWGC_KeyTrack::GetKeyCount(KG3DENVEFF_HANDLE Handle)
	{
		KG3DDWGC_KeyTableBase* pKeyTable = GetTable(Handle);
		if(pKeyTable)
			return pKeyTable->Size();
		return 0;
	}

	HRESULT KG3DDWGC_KeyTrack::GetFirstKeyHandle(KG3DENVEFF_HANDLE Handle, KG3DENVEFF_DWGCKEY_HANDLE* pKeyHandle, LPVOID pExtraInfo )
	{
		KG3DDWGC_KeyTableBase* pKeyTable = GetTable(Handle);
		if(pKeyTable)
			return pKeyTable->GetFirstKeyHandle(pKeyHandle, pExtraInfo);
		return E_INVALIDARG;
	}

	HRESULT KG3DDWGC_KeyTrack::GetNextKeyHandle(KG3DENVEFF_HANDLE Handle, KG3DENVEFF_DWGCKEY_HANDLE PrevHandle, KG3DENVEFF_DWGCKEY_HANDLE* pNextHandle, LPVOID pExtraInfo )
	{
		KG3DDWGC_KeyTableBase* pKeyTable = GetTable(Handle);
		if(pKeyTable)
			return pKeyTable->GetNextKeyHandle(PrevHandle, pNextHandle, pExtraInfo);
		return E_INVALIDARG;
	}

	VOID KG3DDWGC_KeyTrack::ClearKey(KG3DENVEFF_HANDLE Handle)
	{
		KG3DDWGC_KeyTableBase* pKeyTable = GetTable(Handle);
		if(pKeyTable)
			pKeyTable->Clear();
		return;
	}

	HRESULT KG3DDWGC_KeyTrack::GetFirstKeyHandleOfRegion( KG3DENVEFF_HANDLE Handle, INT Region, KG3DENVEFF_DWGCKEY_HANDLE* pHandle, LPVOID pExtraInfo )
	{
		KG3DDWGC_KeyTableBase* pKeyTable = GetTable(Handle);
		if(pKeyTable)
			return pKeyTable->GetFirstKeyHandleOfRegion(Region, pHandle, pExtraInfo);
		return E_INVALIDARG;
	}

	HRESULT KG3DDWGC_KeyTrack::GetNextKeyHandleOfRegion( KG3DENVEFF_HANDLE Handle, INT Region, KG3DENVEFF_DWGCKEY_HANDLE PrevHandle, KG3DENVEFF_DWGCKEY_HANDLE* pNextHandle, LPVOID pExtraInfo )
	{
		KG3DDWGC_KeyTableBase* pKeyTable = GetTable(Handle);
		if(pKeyTable)
			return pKeyTable->GetNextKeyHandleOfRegion(Region, PrevHandle, pNextHandle, pExtraInfo);
		return E_INVALIDARG;
	}

	HRESULT KG3DDWGC_KeyTrack::GetPrevKeyHandle(KG3DENVEFF_HANDLE Handle, KG3DENVEFF_DWGCKEY_HANDLE KeyHandle, KG3DENVEFF_DWGCKEY_HANDLE* pPrevHandle )
	{
		KG3DDWGC_KeyTableBase* pKeyTable = GetTable(Handle);
		if(pKeyTable)
			return pKeyTable->GetPrevKeyHandle(KeyHandle, pPrevHandle);
		return E_INVALIDARG;
	}

	std::auto_ptr<KG3DDWGC_KeyBase> KG3DDWGC_KeyTrack::NewAKey(KG3DENVEFF_HANDLE Handle)
	{
		KG3DDWGC_KeyTableBase* pKeyTable = GetTable(Handle);
		if(pKeyTable)
			return pKeyTable->NewKey();
		return std::auto_ptr<KG3DDWGC_KeyBase>(NULL);
	}
	HRESULT KG3DDWGC_KeyTrack::CopyTo(KG3DENVEFF_HANDLE SrcHandle, KG3DENVEFF_DWGCKEY_HANDLE KeySrc, KG3DENVEFF_HANDLE DescHandle, KG3DENVEFF_DWGCKEY_HANDLE KeyDesc, BOOL bByValueOrRef )
	{
		if(SrcHandle.m_dwType != DescHandle.m_dwType)
			return E_INVALIDARG;
		KG3DDWGC_KeyTableBase* pKeyTableSrc = GetTable(SrcHandle);
		KG3DDWGC_KeyTableBase* pKeyTableDesc = GetTable(DescHandle);
		if(! pKeyTableSrc || ! pKeyTableDesc)
			return E_INVALIDARG;
		return pKeyTableSrc->CopyTo(KeySrc, *pKeyTableDesc, KeyDesc, bByValueOrRef);
	}
	std::tstring& KG3DDWGC_KeyTrack::GetSaveSectionName()
	{
		static std::tstring tcsSaveSectionName = _T("KG3DDWGC_KeyTrack");
		return tcsSaveSectionName;
	}

	HRESULT KG3DDWGC_KeyTrack::SaveMapSetting( IIniFile* pIniFile, LPCTSTR )
	{
		if(! pIniFile)
			return E_INVALIDARG;

		//从EnvEff的数量来保证EnvEff和Table的11对应关系，没有Table的，补上一个空的Table
		{
			std::vector<DWORD> vecTypes;
			m_EnvEffList.GetExistEnvEffectTypes(vecTypes);
			for (size_t i = 0; i < vecTypes.size(); ++i)
			{
				INT EffCount = m_EnvEffList.GetEnvEffectCount(vecTypes[i]);
				KG3DDWGC_KeyTableGeneratorBase* pGen = this->GetTableGen(vecTypes[i]);
				if (! pGen)
					continue;
				_ASSERTE(static_cast<size_t>(EffCount) >= pGen->Size());

				std::vector<KG3DDWGC_KeyTableBase*> vecTablesInEffects;
				vecTablesInEffects.reserve(static_cast<size_t>(EffCount));

				for (INT x = 0; x < EffCount; ++x)
				{
					KG3DEnvEffect* pEff = m_EnvEffList.GetEnvEffect(vecTypes[i], static_cast<int>(x));
					_ASSERTE(pEff);

					KG3DDWGC_KeyTableBase* pTable = pEff->GetKeyTable();

					if(pTable)
					{
						vecTablesInEffects.push_back(pTable);
						continue;
					}

					KG3DDWGC_KeyTableBase* pNewKeyTable = pGen->RequestANewTable();
					if(! pNewKeyTable)
						continue;

					KG3DDWGC_KeyTableBase* pTableNew = pEff->SetKeyTable(*pGen, *pNewKeyTable);
					if (pTableNew)
					{
						vecTablesInEffects.push_back(pTableNew);
					}
					
					
				}
				
				_ASSERTE(static_cast<size_t>(EffCount) == pGen->Size());
				pGen->SortTables(vecTablesInEffects);
			}
		}

		//保存内部复数的TableGen
		TypeTableGens::iterator itEnd = m_vecTableGens.end();
		for (TypeTableGens::iterator it = m_vecTableGens.begin(); it != itEnd; ++it)
		{
			KG3DDWGC_KeyTableGeneratorBase* pTableGen = *it;
			if(! pTableGen)
				continue;
			HRESULT hr = pTableGen->SaveMapSetting(pIniFile, NULL);
			if (FAILED(hr))
			{
				_KG3D_DEBUG_OUTPUT1("以下类型的动态天气KeyTable保存失败：%u", pTableGen->GetType());
				KGLogPrintf(KGLOG_WARNING, "以下类型的动态天气KeyTable保存失败：%u", pTableGen->GetType());
			}
		}

		

		//保存Indices
		pIniFile->WriteInteger(this->GetSaveSectionName().c_str(), "IndicesCount", static_cast<INT>(m_setRegionIndex.size()));
		
		if (! m_setRegionIndex.empty())
		{
			size_t IndexArraySize = m_setRegionIndex.size();
			INT* pIndicesArray = new INT[IndexArraySize];
			if (pIndicesArray)
			{
				size_t i = 0;
				for (TypeRegionIndexSet::iterator it = m_setRegionIndex.begin(); it != m_setRegionIndex.end(); ++it, ++i)
				{
					_ASSERTE(i < IndexArraySize);
					pIndicesArray[i] = *it;
				}

				pIniFile->WriteStruct(this->GetSaveSectionName().c_str(), _T("Indices"), pIndicesArray, sizeof(INT)*static_cast<UINT>(IndexArraySize));//这个可能有问题，旧的Base库只能写16个

				delete[]	pIndicesArray;
			}
		}
		
		return S_OK;
	}

	HRESULT KG3DDWGC_KeyTrack::LoadMapSetting( IIniFile* pIniFile, LPCTSTR )
	{
		if(! pIniFile)
			return E_INVALIDARG;
		TypeTableGens::iterator itEnd = m_vecTableGens.end();
		for (TypeTableGens::iterator it = m_vecTableGens.begin(); it != itEnd; ++it)
		{
			KG3DDWGC_KeyTableGeneratorBase* pTableGen = *it;
			if(! pTableGen)
				continue;

			HRESULT hr = pTableGen->LoadMapSetting(pIniFile, NULL);
			if (FAILED(hr))
			{
				_KG3D_DEBUG_OUTPUT1("以下类型的动态天气KeyTable读取失败：%u", pTableGen->GetType());
				KGLogPrintf(KGLOG_WARNING, "以下类型的动态天气KeyTable读取失败：%u", pTableGen->GetType());
			}			
		}

		//要保证DW是最后读取的
		
		//从EnvEff的数量来保证EnvEff和Table的11对应关系

		{
			std::vector<DWORD> vecTypes;
			m_EnvEffList.GetExistEnvEffectTypes(vecTypes);
			for (size_t i = 0; i < vecTypes.size(); ++i)
			{
				INT EffCount = m_EnvEffList.GetEnvEffectCount(vecTypes[i]);
				KG3DDWGC_KeyTableGeneratorBase* pGen = this->GetTableGen(vecTypes[i]);
				if (pGen)
				{
					if(static_cast<size_t>(EffCount) <= pGen->Size())	//数量相等，1，1对应，那么就每个插入对应的KeyTable
					{
						for (int x = 0; x < EffCount; ++x)
						{
							KG3DEnvEffect* pEff = m_EnvEffList.GetEnvEffect(vecTypes[i], x);
							_ASSERTE(pEff);

							pEff->SetKeyTable(*pGen, pGen->GetTable(x));
						}

						if (static_cast<size_t>(EffCount) < pGen->Size())//如果Table有多余的，删除掉
						{
							for (size_t x = static_cast<size_t>(EffCount); static_cast<size_t>(EffCount) < pGen->Size();)
							{
								KG3DDWGC_KeyTableBase& tableTemp = pGen->GetTable(x);
#if defined(DEBUG) | defined(_DEBUG)
								size_t uOldSize = pGen->Size();
#endif
								pGen->ReleaseTable(&tableTemp);
								_ASSERTE(uOldSize == pGen->Size() + 1);
							}
						}
					}
					else
					{
						_KG3D_DEBUG_OUTPUT1("动态天气KeyTrack，类型：%u的KeyTable的数量和EnvEffect的数量无法匹配\n", vecTypes[i]);
						KGLogPrintf(KGLOG_WARNING, "动态天气KeyTrack，类型：%u的KeyTable的数量和EnvEffect的数量无法匹配\n", vecTypes[i]);

					}
				}
			}
		}


		INT IndexArraySize = 0;
		pIniFile->GetInteger(this->GetSaveSectionName().c_str(), "IndicesCount", 0, &IndexArraySize);//这个可能有问题，旧的Base库只能写16个
		m_setRegionIndex.clear();

		if (IndexArraySize > 0 && IndexArraySize < KG3DENVEFFRI_REGION_COUNT_MAX)
		{
			INT* pIndicesArray = new INT[IndexArraySize];
			if (pIndicesArray)
			{
				INT nRet = pIniFile->GetStruct(this->GetSaveSectionName().c_str(), "Indices", pIndicesArray, sizeof(INT)*IndexArraySize);
				if (nRet)
				{
					for (int i = 0; i < IndexArraySize; ++i)
					{
						this->AddRegionIndex(pIndicesArray[i]);
						//m_setRegionIndex.insert(pIndicesArray[i]);
					}

					delete[] pIndicesArray;
				}
				if (m_setRegionIndex.size() != (size_t)IndexArraySize)
				{
					_KG3D_DEBUG_OUTPUT0("动态天气KeyTrack，Region的索引读取错误\n");
					KGLogPrintf(KGLOG_WARNING, "动态天气KeyTrack，Region的索引读取错误\n");
				}
			}
		}

		return S_OK;

	}

	
	
	
	/************************************************************************/
	/* 
	*/
	/************************************************************************/
#if defined(KG_ENABLE_TDD)
	class KG3DDWGC_KeyTrackTester
	{
	public:
		KG3DDWGC_KeyTrackTester()
		{

			KG3DDWGC_KeyTrack TempKeyTrack;//已经全部Type加进去了


			DWORD TypeAddedArray[] = 
			{
				KG3DTYPE_ENVEFF_SKYSYSTEM_SKYBOX,
				KG3DTYPE_ENVEFF_SKYSYSTEM_FARMOUNTAIN,
				KG3DTYPE_ENVEFF_SKYSYSTEM_CLOUD,
				//KG3DTYPE_ENVEFF_SKYSYSTEM_SYSTEM,
				KG3DTYPE_ENVEFF_FOG,
				KG3DTYPE_ENVEFF_LENSFLARE_SUN,
				KG3DTYPE_ENVEFF_LENSFLARE_MOON,
				KG3DTYPE_ENVEFF_LIGHT,
				KG3DTYPE_ENVEFF_LIGHTSET,
				//KG3DTYPE_ENVEFF_STARRY_NIGHT,
			};

			{
				typedef std::vector<DWORD> TypeVec;
				TypeVec vecOfTypes;
				TempKeyTrack.GetTypesOfAllExistTables(vecOfTypes);
				_ASSERTE(vecOfTypes.size() > 0);
				_ASSERTE(vecOfTypes.size() == _countof(TypeAddedArray));
			}


			//GetTable
			{
				for (size_t i = 0; i < _countof(TypeAddedArray); ++i )
				{
					KG3DDWGC_KeyTableBase* pTable = TempKeyTrack.GetTable(KG3DENVEFF_HANDLE(TypeAddedArray[i], 0));
					_ASSERTE(pTable);
					DWORD dwType = 0;
					pTable->GetType(&dwType);
					_ASSERTE(dwType == TypeAddedArray[i]);
					//准备一些数据
					pTable->New(KG3DENVEFF_DWGCKEY_HANDLE(1,1));
					pTable->New(KG3DENVEFF_DWGCKEY_HANDLE(0,256));
					pTable->New(KG3DENVEFF_DWGCKEY_HANDLE(127,12));
					pTable->New(KG3DENVEFF_DWGCKEY_HANDLE(255,24));
					_ASSERTE(pTable->Size() == 3);
				}
			}
			//RegionIndex
			{

				INT RegionIndexArray[] = {1, 127, 255,};

				//Add, Count
				{
					_ASSERTE(FAILED(TempKeyTrack.AddRegionIndex(256)));
					_ASSERTE(FAILED(TempKeyTrack.AddRegionIndex(0)));

					for (size_t i = 0; i < _countof(RegionIndexArray); ++i)
					{
						_ASSERTE(SUCCEEDED(TempKeyTrack.AddRegionIndex(RegionIndexArray[i])));
					}

					_ASSERTE(TempKeyTrack.GetRegionIndicesCount() == _countof(RegionIndexArray));
				}
				//Traverse
				{
					std::vector<INT>	TempVec;
					INT nRegion = 0;
					HRESULT hr = TempKeyTrack.GetFirstRegionIndex(&nRegion);
					size_t i = 0;
					while (SUCCEEDED(hr))
					{
						TempVec.push_back(nRegion);
						_ASSERTE(i < _countof(RegionIndexArray));
						_ASSERTE(RegionIndexArray[i] == nRegion);
						hr = TempKeyTrack.GetNextRegionIndex(nRegion, &nRegion);
						++i;
					}
					_ASSERTE(TempVec.size() == _countof(RegionIndexArray));
				}

				//Del

				{
					for (size_t i = 0; i < _countof(TypeAddedArray); ++i)
					{
						KG3DDWGC_KeyTableBase* pTable = TempKeyTrack.GetTable(KG3DENVEFF_HANDLE(TypeAddedArray[i], 0));
						_ASSERTE(pTable);
						_ASSERTE(pTable->GetKeyCountOfRegion(127) != 0);
					}
					_ASSERTE(SUCCEEDED(TempKeyTrack.DelRegionIndex(127)));
					for (size_t i = 0; i < _countof(TypeAddedArray); ++i)
					{
						KG3DDWGC_KeyTableBase* pTable = TempKeyTrack.GetTable(KG3DENVEFF_HANDLE(TypeAddedArray[i], 0));
						_ASSERTE(pTable);
						_ASSERTE(pTable->GetKeyCountOfRegion(127) == 0);
						_ASSERTE(pTable->GetKeyCountOfRegion(255) != 0);
						_ASSERTE(pTable->GetKeyCountOfRegion(1) != 0);
						_ASSERTE(TempKeyTrack.GetRegionIndicesCount() == 2);

					}
				}
				//MoveRegionIndex
				{
					ULONG uKeyCount = TempKeyTrack.GetTable(KG3DENVEFF_HANDLE(KG3DTYPE_ENVEFF_FOG, 0))->GetKeyCountOfRegion(1);
					HRESULT hr = TempKeyTrack.ModifyRegionIndex(1, 255);
					_ASSERTE(FAILED(hr));
					hr = TempKeyTrack.ModifyRegionIndex(1, 256);
					_ASSERTE(FAILED(hr));
					hr = TempKeyTrack.ModifyRegionIndex(2, 255);
					_ASSERTE(FAILED(hr));
					hr = TempKeyTrack.ModifyRegionIndex(0, 255);
					_ASSERTE(FAILED(hr));
					hr = TempKeyTrack.ModifyRegionIndex(1, 127);
					_ASSERTE(SUCCEEDED(hr));
					_ASSERTE(TempKeyTrack.GetRegionIndicesCount() == 2);
					_ASSERTE(0 == TempKeyTrack.GetTable(KG3DENVEFF_HANDLE(KG3DTYPE_ENVEFF_FOG, 0))->GetKeyCountOfRegion(1));
					_ASSERTE(uKeyCount == TempKeyTrack.GetTable(KG3DENVEFF_HANDLE(KG3DTYPE_ENVEFF_FOG, 0))->GetKeyCountOfRegion(127));

				}
				//Clear
				{
					_ASSERTE(SUCCEEDED(TempKeyTrack.ClearRegionIndices()));
					for (size_t i = 0; i < _countof(TypeAddedArray); ++i)
					{
						KG3DDWGC_KeyTableBase* pTable = TempKeyTrack.GetTable(KG3DENVEFF_HANDLE(TypeAddedArray[i], 0));
						_ASSERTE(pTable);
						_ASSERTE(pTable->GetKeyCountOfRegion(127) == 0);
						_ASSERTE(pTable->GetKeyCountOfRegion(255) == 0);
						_ASSERTE(pTable->GetKeyCountOfRegion(1) == 0);
						_ASSERTE(pTable->Size() == 0);
					}
				}
				_ASSERTE(TempKeyTrack.GetRegionIndicesCount() == 0);
			}
			//Key操作
			{

				KG3DENVEFF_DWGCKEY_HANDLE HandleArray[]= {
					KG3DENVEFF_DWGCKEY_HANDLE(1,1),
					KG3DENVEFF_DWGCKEY_HANDLE(127,12),
					KG3DENVEFF_DWGCKEY_HANDLE(255, 24),
				};
				TempKeyTrack.AddRegionIndex(0);
				TempKeyTrack.AddRegionIndex(1);
				TempKeyTrack.AddRegionIndex(127);
				TempKeyTrack.AddRegionIndex(255);
				TempKeyTrack.AddRegionIndex(256);
				_ASSERTE(TempKeyTrack.GetRegionIndicesCount() == 3);

				//NewKey
				{
					_ASSERTE(! TempKeyTrack.NewKey(KG3DENVEFF_HANDLE(KG3DTYPE_ENVEFF_FOG, 0), KG3DENVEFF_DWGCKEY_HANDLE(0,0)));
					_ASSERTE(! TempKeyTrack.NewKey(KG3DENVEFF_HANDLE(KG3DTYPE_ENVEFF_FOG, 0), KG3DENVEFF_DWGCKEY_HANDLE(1,0)));
					_ASSERTE(! TempKeyTrack.NewKey(KG3DENVEFF_HANDLE(KG3DTYPE_ENVEFF_FOG, 0), KG3DENVEFF_DWGCKEY_HANDLE(1,25)));
					_ASSERTE(! TempKeyTrack.NewKey(KG3DENVEFF_HANDLE(KG3DTYPE_ENVEFF_FOG, 0), KG3DENVEFF_DWGCKEY_HANDLE(125,12)));
					_ASSERTE(! TempKeyTrack.NewKey(KG3DENVEFF_HANDLE(KG3DTYPE_ENVEFF_FOG, 0), KG3DENVEFF_DWGCKEY_HANDLE(126,25)));
					_ASSERTE(! TempKeyTrack.NewKey(KG3DENVEFF_HANDLE(KG3DTYPE_ENVEFF_FOG, 0), KG3DENVEFF_DWGCKEY_HANDLE(254,12)));//254不再Region表中，应该不让New
					for (size_t i = 0; i < _countof(HandleArray); ++i)
					{
						TempKeyTrack.NewKey(KG3DENVEFF_HANDLE(KG3DTYPE_ENVEFF_RAIN,0), HandleArray[i]);
					}
					_ASSERTE(TempKeyTrack.GetKeyCount(KG3DENVEFF_HANDLE(KG3DTYPE_ENVEFF_FOG,0)) == 0);
					for (size_t i = 0; i < _countof(HandleArray); ++i)
					{
						TempKeyTrack.NewKey(KG3DENVEFF_HANDLE(KG3DTYPE_ENVEFF_FOG, 0), HandleArray[i]);
					}
					_ASSERTE(TempKeyTrack.GetKeyCount(KG3DENVEFF_HANDLE(KG3DTYPE_ENVEFF_FOG,0)) == _countof(HandleArray)+0);
				}
				//DelKey
				{
					for (size_t i = 1; i < _countof(HandleArray); ++i)
					{
						TempKeyTrack.DelKey(KG3DENVEFF_HANDLE(KG3DTYPE_ENVEFF_FOG,0), HandleArray[i]);
					}
					_ASSERTE(TempKeyTrack.GetKeyCount(KG3DENVEFF_HANDLE(KG3DTYPE_ENVEFF_FOG,0)) == 1);
				}

			}
			//_KG3D_DEBUG_REPORT("KG3DDWGC_KeyTrack All Test Passed, Congratulations!");

		}
	};

KG_TEST_BEGIN(KG3DEnvEffDW_KeyTrack)
{
	KG3DDWGC_KeyTrackTester stKeyTrackTester;
}
KG_TEST_END(KG3DEnvEffDW_KeyTrack)
#endif
	
	
	
};


////////////////////////////////////////////////////////////////////////////////

