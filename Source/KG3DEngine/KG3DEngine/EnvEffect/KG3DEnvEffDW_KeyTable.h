////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DEnvEffDW_KeyTable.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-12-15 17:21:50
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DENVEFFDW_KEYTABLE_H_
#define _INCLUDE_KG3DENVEFFDW_KEYTABLE_H_



#include "KG3DEnvEffDW_KeyBase.h"
#include "KG3DRTTIType.h"
#include "IEKG3DEnvEffDynamicWeather.h"
#include "KGINIFileHelpers.h"
#include "KGCustomAllocators.h"
////////////////////////////////////////////////////////////////////////////////
namespace KG3D_ENVIRONMENT_EFFECT
{
	template<class _TypeKey>
	class TKG3DDWGC_KeyTable : public KG3DDWGC_KeyTableBase1
	{
	public:
		struct elem_type 
		{
			_TypeKey	m_Key;
			size_t		m_Ref;
			elem_type():m_Ref(1){}
			elem_type(const elem_type& Other):m_Ref(1),m_Key(Other.m_Key){}
			elem_type& operator=(const elem_type& Other){m_Key = Other.m_Key; m_Ref = Other.m_Ref;}
		};

		typedef KG_CUSTOM_HELPERS::FAST_ALLOC::index_fast_vec<elem_type>	key_alloc;
		typedef typename key_alloc::handle_type key_handle_type;//实际是size_t

		//map中保留的是Handle，然后由_TypeKeyAlloc分配真正的空间给Key，复制的时候就可以用Index来复制了
		
		typedef std::map<DWORD, key_handle_type> _TypeCan;
		typedef typename _TypeCan::iterator iterator;
		typedef typename _TypeCan::const_iterator const_iterator;
		typedef typename _TypeCan::value_type value_type;

		struct load_struct 
		{
			LPCTSTR pPreface;
			LPCTSTR pEnd;
			std::map<key_handle_type, key_handle_type>* pMapHandleHandle;//存的时候对应的未必能读的时候对应，所以要用Map来保证
		};
	private:
		_TypeCan	m_Container;
		key_alloc&	m_Alloc;
	private:
		static DWORD GetHandle(INT nRegionIndex, INT nHourIndex)			{_ASSERTE(CheckIsRegionIndexValid(nRegionIndex) && CheckIsHourIndexValid(nHourIndex));return (nRegionIndex << 16) | (nHourIndex & 0x0000ffff);}
		static INT GetRegionFromHandle(DWORD Handle){return (INT)(Handle >> 16);}
		static INT GetHourFromHandle(DWORD Handle){return (INT)(Handle & 0x0000ffff);}
		static KG3DENVEFF_DWGCKEY_HANDLE GetRegionHourFromHandle(DWORD Handle)	{return KG3DENVEFF_DWGCKEY_HANDLE(Handle >> 16, Handle & 0x0000ffff);}
		
		BOOL	GetHandleFromString(LPCTSTR tcsString, size_t BufferSize, DWORD* pHandle);
		BOOL	GetStringFromHandle(DWORD Handle, LPTSTR tcsString, size_t BufferSize);
		KG3DDWGC_KeyBase* New(DWORD Handle);
		HRESULT			  Del(DWORD Handle);
		iterator		  DelImp(iterator it);
		KG3DDWGC_KeyBase& GetImp( const_iterator it );
		iterator		GetFirstItOfRegion(INT Region);
		iterator		GetNextItOfRegion(iterator it);
	public:
		enum{DWGCKEY_TYPE = _TypeKey::DWGCKEY_TYPE,};
		enum{max_key_name_size = 64, max_key_value_size = 64,};
		static const std::tstring&	GetSaveSectionName();
	private:	//封闭拷贝
		TKG3DDWGC_KeyTable(const TKG3DDWGC_KeyTable&);
		TKG3DDWGC_KeyTable& operator=(const TKG3DDWGC_KeyTable&);
	public:
		TKG3DDWGC_KeyTable(key_alloc& Alloc);
		~TKG3DDWGC_KeyTable(){this->Clear();}
		
		
		virtual KG3DDWGC_KeyBase*	New(KG3DENVEFF_DWGCKEY_HANDLE KeyHandle);
		virtual HRESULT				Del(KG3DENVEFF_DWGCKEY_HANDLE KeyHandle);
		virtual KG3DDWGC_KeyBase*	Get(KG3DENVEFF_DWGCKEY_HANDLE KeyHandle);

		virtual HRESULT				Clear();
		virtual ULONG				Size()const{return (ULONG)m_Container.size();}
		virtual DWORD				GetType()const{return DWGCKEY_TYPE;}

		//单独对Region的管理
		virtual HRESULT				DelRegion(INT nRegion);
		virtual HRESULT				MoveAllKeysInRegionToNewRegion(INT nRegion, INT nNewRegion);
		virtual ULONG				GetKeyCountOfRegion(INT Region);
		virtual ULONG				IsAnyKeyInRegion(INT Region){return GetFirstItOfRegion(Region) == m_Container.end() ? 0 : 1;}
		//<接口>
		virtual HRESULT GetFirstKeyHandle(KG3DENVEFF_DWGCKEY_HANDLE* pHandle, LPVOID pExtraInfo);
		virtual HRESULT GetNextKeyHandle(KG3DENVEFF_DWGCKEY_HANDLE PrevHandle, KG3DENVEFF_DWGCKEY_HANDLE* pNextHandle, LPVOID pExtraInfo);
		virtual HRESULT GetFirstKeyHandleOfRegion(INT Region, KG3DENVEFF_DWGCKEY_HANDLE* pHandle, LPVOID pExtraInfo);
		virtual HRESULT GetNextKeyHandleOfRegion(INT Region, KG3DENVEFF_DWGCKEY_HANDLE PrevHandle, KG3DENVEFF_DWGCKEY_HANDLE* pNextHandle, LPVOID pExtraInfo);
		virtual HRESULT GetPrevKeyHandle(KG3DENVEFF_DWGCKEY_HANDLE KeyHandle, KG3DENVEFF_DWGCKEY_HANDLE* pPrevHandle);
		//</接口>
			
		//插值
		virtual	HRESULT GetInterpKey(KG3DENVEFF_DWGCKEY_HANDLE KeyHandle, KG3DDWGC_KeyBase& RetKey);
		virtual std::auto_ptr<KG3DDWGC_KeyBase> NewKey(){return std::auto_ptr<KG3DDWGC_KeyBase>(new _TypeKey);}

		//拷贝
		virtual HRESULT CopyTo(KG3DENVEFF_DWGCKEY_HANDLE Src, KG3DDWGC_KeyTableBase1& DescTable, KG3DENVEFF_DWGCKEY_HANDLE Desc, BOOL bByValueOrByRef);
		
		virtual HRESULT SaveMapSetting(IIniFile* pIniFile, LPCTSTR pPreface);
		virtual HRESULT LoadMapSetting(IIniFile* pIniFile, LPCTSTR pPreface);

#if defined(DEBUG) | defined(_DEBUG)
		size_t	GetAllocElemCount_Plus_RefElemCount();
		void	TestPrivateFunctions();
#endif
		
	};	

	
	template<class _TypeKey>
	HRESULT TKG3DDWGC_KeyTable<_TypeKey>::CopyTo( KG3DENVEFF_DWGCKEY_HANDLE Src, KG3DDWGC_KeyTableBase1& DescTable, KG3DENVEFF_DWGCKEY_HANDLE Desc, BOOL bByValueOrByRef )
	{
		TKG3DDWGC_KeyTable<_TypeKey>* pDescTable = dynamic_cast<TKG3DDWGC_KeyTable<_TypeKey>* >(&DescTable);
		KG_PROCESS_ERROR(pDescTable);//存在且目标Table和现在的Table是同类型的
		KG_PROCESS_ERROR(CheckIsKeyHandleValid(Src) && CheckIsKeyHandleValid(Desc));
		if(pDescTable == this)
			KG_PROCESS_ERROR(Src != Desc);
		/*
		不允许覆盖，因为如果一个Key原来是个引用，然后用值拷贝覆盖掉的话，问题就大了
		*/
		KG_PROCESS_ERROR(! pDescTable->Get(Desc));	
#if defined(DEBUG) | defined(_DEBUG)
		if(pDescTable != this)
			_ASSERTE(&pDescTable->m_Alloc == &this->m_Alloc);
#endif
		
		if (bByValueOrByRef)//烤值，这样最简单了
		{
			//这个一定要在前面，因为New可能引起内存重分配
			_TypeKey* pDesc = dynamic_cast<_TypeKey*>(pDescTable->New(Desc));
			KG_PROCESS_ERROR(pDesc);

			_TypeKey* pSrc = dynamic_cast<_TypeKey*>(this->Get(Src));
			KG_PROCESS_ERROR(pSrc);
			
			*pDesc = *pSrc;
			goto Exit1;
		}
		else	//烤引用
		{
			DWORD HandleSrc = GetHandle(Src.m_Region, Src.m_Hour);
			iterator itSrc = m_Container.find(HandleSrc);
			KG_PROCESS_ERROR(itSrc != m_Container.end());
			elem_type& elem = m_Alloc.get(itSrc->second);
			_ASSERTE(! m_Alloc.is_error_elem(elem));
			_ASSERTE(elem.m_Ref > 0);

			//上面包含了目标Table和this相等的情况，而Alloc在同 类型的Table中是共享的
			{
				DWORD HandleDesc = pDescTable->GetHandle(Desc.m_Region, Desc.m_Hour);

				_TypeCan::_Pairib Pib = pDescTable->m_Container.insert(value_type(HandleDesc, itSrc->second));
				KG_PROCESS_ERROR(Pib.second);

				++elem.m_Ref;
			}
			goto Exit1;
		}
Exit1:
		return S_OK;
Exit0:
		return E_FAIL;
	}
		//////////////////////////////////////////////////////////////////////////
	template<class _TypeKey>
	TKG3DDWGC_KeyTable<_TypeKey>::TKG3DDWGC_KeyTable(typename TKG3DDWGC_KeyTable<_TypeKey>::key_alloc& Alloc)
		:m_Alloc(Alloc)
	{	
		_KG3D_DEBUG_STATIC_CHECK_T(KG3DENVEFFRI_REGION_MAX <= SHRT_MAX && KG3DENVEFFTI_HOUR_MAX <= SHRT_MAX, HandleSizeNotEnough);
	}
	//////////////////////////////////////////////////////////////////////////
	template<class _TypeKey>
	HRESULT TKG3DDWGC_KeyTable<_TypeKey>::GetNextKeyHandleOfRegion( INT Region, KG3DENVEFF_DWGCKEY_HANDLE PrevHandle, KG3DENVEFF_DWGCKEY_HANDLE* pNextHandle, LPVOID pExtraInfo )
	{
		iterator it = m_Container.find(GetHandle(PrevHandle.m_Region, PrevHandle.m_Hour));
		if (it != m_Container.end() && pNextHandle && PrevHandle.m_Region == Region)
		{
			it = GetNextItOfRegion(it);
			if (it != m_Container.end())
			{
				*pNextHandle = GetRegionHourFromHandle(it->first);
				if(pExtraInfo)
					*reinterpret_cast<key_handle_type*>(pExtraInfo) = it->second;
				return S_OK;
			}
		}
		return E_FAIL;
	}
	//////////////////////////////////////////////////////////////////////////
	template<class _TypeKey>
	HRESULT TKG3DDWGC_KeyTable<_TypeKey>::GetFirstKeyHandleOfRegion( INT Region, KG3DENVEFF_DWGCKEY_HANDLE* pHandle, LPVOID pExtraInfo )
	{
		iterator it = GetFirstItOfRegion(Region);
		if (it != m_Container.end() && pHandle)
		{
			*pHandle = GetRegionHourFromHandle(it->first);
			if(pExtraInfo)
				*reinterpret_cast<key_handle_type*>(pExtraInfo) = it->second;
			return S_OK;
		}
		return E_FAIL;
	}
	//////////////////////////////////////////////////////////////////////////
	template<class _TypeKey>
	HRESULT KG3D_ENVIRONMENT_EFFECT::TKG3DDWGC_KeyTable<_TypeKey>::GetPrevKeyHandle( KG3DENVEFF_DWGCKEY_HANDLE KeyHandle, KG3DENVEFF_DWGCKEY_HANDLE* pPrevHandle )
	{
		KG_PROCESS_ERROR(CheckIsKeyHandleValid(KeyHandle) && pPrevHandle);
		{
			DWORD Handle = this->GetHandle(KeyHandle.m_Region, KeyHandle.m_Hour);
			size_t size = m_Container.size();
			if (size == 1)
			{
				iterator it = m_Container.begin();
				KG_PROCESS_ERROR(it->first < Handle);
				*pPrevHandle = this->GetRegionHourFromHandle(it->first);
			}
			else
			{
				iterator it = m_Container.lower_bound(Handle);
				KG_PROCESS_ERROR(it != m_Container.begin());
				--it;
				*pPrevHandle = this->GetRegionHourFromHandle(it->first);
			}
			return S_OK;
			
		}
Exit0:
		return E_FAIL;
	}
	//////////////////////////////////////////////////////////////////////////
	template<class _TypeKey>
	HRESULT TKG3DDWGC_KeyTable<_TypeKey>::GetInterpKey( KG3DENVEFF_DWGCKEY_HANDLE KeyHandle, KG3DDWGC_KeyBase& RetKey )
	{
		INT Region = KeyHandle.m_Region;
		INT Hour = KeyHandle.m_Hour;
		KG3DDWGC_KeyBase* pPrevKey = NULL;
		KG3DDWGC_KeyBase* pNextKey = NULL;
		INT PrevHour = KG3DENVEFFTI_HOUR_INVALID;
		INT NextHour = KG3DENVEFFTI_HOUR_INVALID;
		KG_PROCESS_ERROR(CheckIsKeyHandleValid(KeyHandle));
		{
			//有现成的Key就不用插值了
			KG3DDWGC_KeyBase* pKey = this->Get(KeyHandle);
			if(pKey)
			{
				_TypeKey* p = dynamic_cast<_TypeKey*>(pKey);
				_ASSERTE(p);
				_TypeKey* pRet = dynamic_cast<_TypeKey*>(&RetKey);
				*pRet = *p;
				return S_OK;
			}
		}
		//来到这的话，在KeyHandle的位置一定是没有Key的
		{
			KG_PROCESS_ERROR(this->IsAnyKeyInRegion(Region));//保证至少有一个Key

			//用upperbound来找到下一个Key
			DWORD Handle = this->GetHandle(Region, Hour);
			iterator it = m_Container.upper_bound(Handle);
			if(it != m_Container.begin())
				--it;

			//注意Hour是循环的，24的下1小时是1
			if(GetRegionFromHandle(it->first) < Region)
				++it;
#if defined(DEBUG) | defined(_DEBUG)
			_ASSERTE(it != m_Container.end());
			_ASSERTE(GetRegionFromHandle(it->first) == Region);
#endif
			
			if(GetHourFromHandle(it->first) > Hour)
			{
				pNextKey = &this->GetImp(it);
				NextHour = GetHourFromHandle(it->first);

				bool bNeedToFindPrevInEndOfTheRegion = false;
				if(it == m_Container.begin())
				{
					bNeedToFindPrevInEndOfTheRegion = true;
				}
				else
				{
					--it;
					if(GetRegionFromHandle(it->first) == Region)
					{
						pPrevKey = &this->GetImp(it);
						PrevHour = GetHourFromHandle(it->first);
					}
					else	
					{
						bNeedToFindPrevInEndOfTheRegion = true;
					}
				}

				//从这个Region的最后开始找
				if(bNeedToFindPrevInEndOfTheRegion)
				{
					iterator itEndOfRegion = m_Container.upper_bound(this->GetHandle(Region, KG3DENVEFFTI_HOUR_MAX));
					--itEndOfRegion;
					_ASSERTE(GetRegionFromHandle(itEndOfRegion->first) == Region);

					pPrevKey = &this->GetImp(itEndOfRegion);
					PrevHour = GetHourFromHandle(itEndOfRegion->first);
				}
			}
			else
			{
				pPrevKey = &this->GetImp(it);
				PrevHour = GetHourFromHandle(it->first);

				bool bNeedToFindNextInTheBeginOfTheRegion = false;
				iterator itLastElem = m_Container.end();
				--itLastElem;
				if(it == itLastElem)
				{
					bNeedToFindNextInTheBeginOfTheRegion = true;
				}
				else
				{
					++it;
					if(GetRegionFromHandle(it->first) == Region)
					{
						pNextKey = &this->GetImp(it);
						NextHour = GetHourFromHandle(it->first);
					}
					else
					{
						bNeedToFindNextInTheBeginOfTheRegion = true;
					}
				}

				//从这个Region的开头开始找
				if (bNeedToFindNextInTheBeginOfTheRegion)
				{
					iterator itBegin = GetFirstItOfRegion(Region);
					_ASSERTE(itBegin != m_Container.end());

					pNextKey = &this->GetImp(itBegin);
					NextHour = GetHourFromHandle(itBegin->first);
				}
			}
			_ASSERTE(pPrevKey && pNextKey);
			_ASSERTE(CheckIsHourIndexValid(PrevHour) && CheckIsHourIndexValid(NextHour));
			if (pPrevKey == pNextKey)
			{
				_ASSERTE(GetKeyCountOfRegion(Region) == 1);
				_TypeKey* p = dynamic_cast<_TypeKey*>(pPrevKey);
				_ASSERTE(p);
				_TypeKey* pRet = dynamic_cast<_TypeKey*>(&RetKey);
				*pRet = *p;

			}
			else
			{
				INT GapBetweenHourAndPrevHour = Hour > PrevHour ? Hour - PrevHour 
					: Hour + KG3DENVEFFTI_HOUR_IN_A_DAY - PrevHour;
				INT GapBetweenNextHourAndPrevHour = NextHour > PrevHour ? NextHour - PrevHour 
					: NextHour + KG3DENVEFFTI_HOUR_IN_A_DAY - PrevHour;
				_ASSERTE(GapBetweenHourAndPrevHour > 0);
				_ASSERTE(GapBetweenNextHourAndPrevHour > 0);
				float fPercent = (float)GapBetweenHourAndPrevHour / (float)GapBetweenNextHourAndPrevHour;
				_ASSERTE(fPercent < 1+ FLT_EPSILON);
				bool bRet = RetKey.CalculateInterp(*pPrevKey, *pNextKey, fPercent);
				KG_PROCESS_ERROR(bRet);
			}
		}

		return S_OK;
Exit0:
		return E_FAIL;
	}
	//////////////////////////////////////////////////////////////////////////
	template<class _TypeKey>
	HRESULT TKG3DDWGC_KeyTable<_TypeKey>::GetNextKeyHandle( KG3DENVEFF_DWGCKEY_HANDLE PrevHandle, KG3DENVEFF_DWGCKEY_HANDLE* pNextHandle, LPVOID pExtraInfo )
	{
		if(! pNextHandle)
			return E_INVALIDARG;
		KG_PROCESS_ERROR(! m_Container.empty() && CheckIsKeyHandleValid(PrevHandle));
		{
			_TypeCan::iterator it = m_Container.upper_bound(GetHandle(PrevHandle.m_Region, PrevHandle.m_Hour));
			if(it != m_Container.end())
			{
				*pNextHandle = GetRegionHourFromHandle(it->first);

				_ASSERTE(*pNextHandle != PrevHandle);
				if (pExtraInfo)
				{
					size_t* p = reinterpret_cast<size_t*>(pExtraInfo);
					*p = it->second;
#if defined(_DEBUG) | defined(DEBUG)
					elem_type& elem = m_Alloc.get(it->second);
					_ASSERTE(! m_Alloc.is_error_elem(elem));
#endif
				}
				return S_OK;
			}
		}
Exit0:
		return E_FAIL;
	}
	//////////////////////////////////////////////////////////////////////////
	template<class _TypeKey>
	HRESULT TKG3DDWGC_KeyTable<_TypeKey>::GetFirstKeyHandle( KG3DENVEFF_DWGCKEY_HANDLE* pHandle, LPVOID pExtraInfo)
	{
		if(! pHandle)
			return E_INVALIDARG;
		KG_PROCESS_ERROR(! m_Container.empty());
		{
			_TypeCan::iterator it = m_Container.begin();
			*pHandle = GetRegionHourFromHandle(it->first);
			if(pExtraInfo)
			{
				size_t* p = reinterpret_cast<size_t*>(pExtraInfo);
				*p = it->second;
#if defined(_DEBUG) | defined(DEBUG)
				elem_type& elem = m_Alloc.get(it->second);
				_ASSERTE(! m_Alloc.is_error_elem(elem));
#endif
			}
			return S_OK;
		}
Exit0:
		return E_FAIL;
	}
	//////////////////////////////////////////////////////////////////////////
#if defined(DEBUG) | defined(_DEBUG)
	template<class _TypeKey>
	size_t TKG3DDWGC_KeyTable<_TypeKey>::GetAllocElemCount_Plus_RefElemCount()
	{
		size_t AllElems = 0;
		key_handle_type handle = m_Alloc.get_first_handle();
		while(m_Alloc.is_valid_handle(handle))
		{
			elem_type& elem = m_Alloc.get(handle);
			_ASSERTE(! m_Alloc.is_error_elem(elem));
			AllElems+= elem.m_Ref;
			handle = m_Alloc.get_next_handle(handle);
		}
		return AllElems;
	}
	//////////////////////////////////////////////////////////////////////////
	template<class _TypeKey>
	void TKG3DDWGC_KeyTable<_TypeKey>::TestPrivateFunctions()
	{
		{
			DWORD Handle;
			TCHAR* TestStr[] = {_T("125_12"), _T("125"), _T("_12"), _T("aa_12"), _T("")};
			BOOL bRet = this->GetHandleFromString(TestStr[0], _tcslen(TestStr[0])+1, &Handle);
			_ASSERTE(bRet);
			_ASSERTE(Handle == (((DWORD)125 << 16) |  (DWORD)12));

			for(size_t i = 1; i < _countof(TestStr); ++i)
			{
				bRet = this->GetHandleFromString(TestStr[i], _tcslen(TestStr[i])+1, &Handle);
				_ASSERTE(! bRet);
			}
		}
	}
#endif
	//////////////////////////////////////////////////////////////////////////
	template<class _TypeKey>
	KG3DDWGC_KeyBase* TKG3DDWGC_KeyTable<_TypeKey>::New( DWORD Handle )
	{
		KG_PROCESS_ERROR(m_Alloc.size() < (size_t)INT_MAX && m_Container.size() < (size_t)INT_MAX);
		{
			KG_PROCESS_ERROR(m_Container.end() == m_Container.find(Handle));//已存在就不能New了

			static std::pair<DWORD, key_handle_type> DefaultPair(0, 0);
#if defined(_DEBUG) | defined(DEBUG)
			size_t oldSize = m_Alloc.size();
#endif

			key_handle_type key_handle = m_Alloc.push(m_Alloc.DefaultElem());
			KG_PROCESS_ERROR(m_Alloc.is_valid_handle(key_handle));


			DefaultPair.first = Handle;
			DefaultPair.second = key_handle;

#if defined(_DEBUG) | defined(DEBUG)
			{
				_ASSERTE(m_Alloc.size() == oldSize+1);
				_ASSERTE(! m_Alloc.is_error_elem(m_Alloc.get(key_handle)));
			}
#endif

			std::pair<iterator, bool> stRet = m_Container.insert(DefaultPair);
			if(! stRet.second)
			{
				m_Alloc.erase(key_handle);
				goto Exit0;
			}
			return &(m_Alloc.get(key_handle).m_Key);
		}
Exit0:
		return NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	template<class _TypeKey>
	HRESULT TKG3DDWGC_KeyTable<_TypeKey>::Del( DWORD Handle )
	{
		_TypeCan::iterator it = m_Container.find(Handle);
		KG_PROCESS_ERROR(it != m_Container.end());
		DelImp(it);
		return S_OK;
Exit0:
		return E_FAIL;
	}
	//////////////////////////////////////////////////////////////////////////
	template<class _TypeKey>
	typename TKG3DDWGC_KeyTable<_TypeKey>::iterator TKG3DDWGC_KeyTable<_TypeKey>::DelImp(typename TKG3DDWGC_KeyTable<_TypeKey>::iterator it )
	{
		key_handle_type key_handle = it->second;

		_ASSERTE(m_Alloc.is_valid_handle(key_handle));
		elem_type& elem = m_Alloc.get(key_handle);
		_ASSERTE(! m_Alloc.is_error_elem(elem));

		if(elem.m_Ref > 1)
			--elem.m_Ref;
		else
			m_Alloc.erase(key_handle);

		return m_Container.erase(it);//MS的map的erase删除之后会返回下一个it，这是不符合std规范的，但很好用
	}
	//////////////////////////////////////////////////////////////////////////
	template<class _TypeKey>
	typename TKG3DDWGC_KeyTable<_TypeKey>::iterator TKG3DDWGC_KeyTable<_TypeKey>::GetFirstItOfRegion( INT Region )
	{
		KG_PROCESS_ERROR(CheckIsRegionIndexValid(Region));
		{
			DWORD Handle = GetHandle(Region, KG3DENVEFFTI_HOUR_MIN);
			_TypeCan::iterator it = m_Container.lower_bound(Handle);//注意到Region_Hour是有顺序的
			KG_PROCESS_ERROR(it != m_Container.end());

			KG3DENVEFF_DWGCKEY_HANDLE KeyHandle = GetRegionHourFromHandle(it->first);
			if(KeyHandle.m_Region == Region)
			{
				return it;
			}
			_ASSERTE(KeyHandle.m_Region > Region);
		}
Exit0:
		return m_Container.end();
	}
	//////////////////////////////////////////////////////////////////////////
	template<class _TypeKey>
	 typename TKG3DDWGC_KeyTable<_TypeKey>::iterator TKG3DDWGC_KeyTable<_TypeKey>::GetNextItOfRegion( typename TKG3DDWGC_KeyTable<_TypeKey>::iterator it )
	{
		_ASSERTE(it != m_Container.end());
		_TypeCan::iterator itReturn = it;
		++itReturn;
		KG_PROCESS_ERROR(itReturn != m_Container.end());
		{
			KG3DENVEFF_DWGCKEY_HANDLE KeyHandle = GetRegionHourFromHandle(it->first);
			KG3DENVEFF_DWGCKEY_HANDLE KeyHandleNext = GetRegionHourFromHandle(itReturn->first);
			if (KeyHandle.m_Region == KeyHandleNext.m_Region)
			{
				return itReturn;
			}
		}
Exit0:
		return m_Container.end();
	}
	//////////////////////////////////////////////////////////////////////////
	template<class _TypeKey>
	HRESULT TKG3DDWGC_KeyTable<_TypeKey>::DelRegion( INT nRegion )
	{
		_TypeCan::iterator it = GetFirstItOfRegion(nRegion);
		while (it != m_Container.end())
		{
			iterator itNext = GetNextItOfRegion(it);
			DelImp(it);
			it = itNext;
		}
		_ASSERTE(GetFirstItOfRegion(nRegion) == m_Container.end());	
		return S_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	template<class _TypeKey>
	HRESULT KG3D_ENVIRONMENT_EFFECT::TKG3DDWGC_KeyTable<_TypeKey>::MoveAllKeysInRegionToNewRegion( INT nRegion, INT nNewRegion )
	{
		KG_PROCESS_ERROR(CheckIsRegionIndexValid(nRegion) && CheckIsRegionIndexValid(nNewRegion));
		KG_PROCESS_ERROR(! this->IsAnyKeyInRegion(nNewRegion));
		{
			//std::pair 不能复制，只能暂时用自己定义的struct了
			struct TypePir  
			{
				DWORD first;
				key_handle_type second;
				TypePir(DWORD A, key_handle_type B):first(A),second(B){}
				operator value_type(){return value_type(first, second);}
			};
			typedef std::vector<TypePir> TypeVecPir;
			TypeVecPir vecValue;
			//光移动key_handle_type，而不用移动实际分配的Key

#if defined(_DEBUG) | defined(DEBUG)
			size_t OldSize = m_Container.size();
#endif

			_TypeCan::iterator it = GetFirstItOfRegion(nRegion);
			if(it == m_Container.end())//一个都没有，那么也算成功了
				return S_FALSE;

			while (it != m_Container.end())
			{
				KG3DENVEFF_DWGCKEY_HANDLE RegionHour = GetRegionHourFromHandle(it->first);
				vecValue.push_back(TypePir(GetHandle(nNewRegion, RegionHour.m_Hour), it->second));

				iterator itNext = GetNextItOfRegion(it);
				m_Container.erase(it);
				it = itNext;
			}
			_ASSERTE(GetFirstItOfRegion(nRegion) == m_Container.end());	
			
			m_Container.insert(vecValue.begin(), vecValue.end());
			
			_ASSERTE(OldSize == m_Container.size());
			_ASSERTE(GetFirstItOfRegion(nNewRegion) != m_Container.end());
			return S_OK;
		}
Exit0:
		return E_FAIL;
	}
	//////////////////////////////////////////////////////////////////////////
	template<class _TypeKey>
	ULONG KG3D_ENVIRONMENT_EFFECT::TKG3DDWGC_KeyTable<_TypeKey>::GetKeyCountOfRegion( INT Region )
	{
		ULONG uCount = 0;
		iterator it = this->GetFirstItOfRegion(Region);
		KG_PROCESS_ERROR(it != m_Container.end());
		{
			DWORD LastPossibleHandleOfThisRegion = this->GetHandle(Region, KG3DENVEFFTI_HOUR_MAX);
			iterator itNext = m_Container.upper_bound(LastPossibleHandleOfThisRegion);//可以为end();
			for (; it != itNext; ++it)
			{
#if defined(_DEBUG) | defined(DEBUG)
				KG3DENVEFF_DWGCKEY_HANDLE KeyHandle = GetRegionHourFromHandle(it->first);
				_ASSERTE(KeyHandle.m_Region == Region);
#endif
				++uCount;//向前进，看看能找到多少个Region
			}
		}
Exit0:
		return uCount;
	}
	//////////////////////////////////////////////////////////////////////////
	template<class _TypeKey>
	HRESULT TKG3DDWGC_KeyTable<_TypeKey>::Clear()
	{
		for (iterator it = m_Container.begin(); it != m_Container.end();)
		{
			it = this->DelImp(it);
		}
		_ASSERTE(m_Container.size() == 0);
		//m_Alloc.clear();Alloc在Table间共享，可不能随便的Clear
		m_Container.clear();
		return S_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	template<class _TypeKey>
	KG3DDWGC_KeyBase* TKG3DDWGC_KeyTable<_TypeKey>::New(KG3DENVEFF_DWGCKEY_HANDLE KeyHandle)
	{
		if(! CheckIsKeyHandleValid(KeyHandle))
			return NULL;
		DWORD Handle = GetHandle(KeyHandle.m_Region, KeyHandle.m_Hour);
		return this->New(Handle);
	}
	//////////////////////////////////////////////////////////////////////////
	template<class _TypeKey>
	HRESULT TKG3DDWGC_KeyTable<_TypeKey>::Del(KG3DENVEFF_DWGCKEY_HANDLE KeyHandle)
	{
		if(! CheckIsKeyHandleValid(KeyHandle))
			return E_FAIL;
		DWORD Handle = GetHandle(KeyHandle.m_Region, KeyHandle.m_Hour);
		return this->Del(Handle);
	}
	//////////////////////////////////////////////////////////////////////////
	template<class _TypeKey>
	KG3DDWGC_KeyBase* TKG3DDWGC_KeyTable<_TypeKey>::Get(KG3DENVEFF_DWGCKEY_HANDLE KeyHandle)
	{
		if(! CheckIsKeyHandleValid(KeyHandle))
			return NULL;
		DWORD Handle = GetHandle(KeyHandle.m_Region, KeyHandle.m_Hour);
		const_iterator it = m_Container.find(Handle);
		if (it != m_Container.end())
		{
			return &GetImp(it);
		}
		return NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	template<class _TypeKey>
	KG3DDWGC_KeyBase& TKG3DDWGC_KeyTable<_TypeKey>::GetImp(typename TKG3DDWGC_KeyTable<_TypeKey>::const_iterator it )
	{
		key_handle_type key_handle = it->second;
		_ASSERTE(m_Alloc.is_valid_handle(key_handle));
		elem_type& elem = m_Alloc.get(key_handle);
		_ASSERTE(! m_Alloc.is_error_elem(elem));

		return elem.m_Key;
	}
	//////////////////////////////////////////////////////////////////////////
	/*
	template<class _TypeKey>
		HRESULT KG3D_ENVIRONMENT_EFFECT::TKG3DDWGC_KeyTable<_TypeKey>::EnumElems( TypeFuncDWGCKeyTableEnum Func, DWORD_PTR ParamForFunc)
		{
			HRESULT hr = E_FAIL;
			_TypeCan::const_iterator it = m_Container.begin();
			_TypeCan::const_iterator itEnd = m_Container.end();
			std::pair<INT, INT> RegionHour;
			for (;it != itEnd; ++it)
			{
	#if defined(_DEBUG) | defined(DEBUG)
				key_handle_type key_handle = it->second;
				_ASSERTE(m_Alloc.is_valid_handle(key_handle));
				elem_type& elem = m_Alloc.get(key_handle);
				_ASSERTE(! m_Alloc.is_error_elem(elem));
	#endif
				RegionHour = GetRegionHourFromHandle(it->first);
				hr = (*Func)(KG3DENVEFF_DWGCKEY_HANDLE(RegionHour.first, RegionHour.second), static_cast<DWORD_PTR>(key_handle), ParamForFunc);
				KG_COM_PROCESS_ERROR(hr);
			}
	Exit0:
			return hr;
		}*/
	
	//////////////////////////////////////////////////////////////////////////
	
	template<class _TypeKey>
	const std::tstring& TKG3DDWGC_KeyTable<_TypeKey>::GetSaveSectionName()
	{
		struct NameInitializer 
		{
			std::tstring str;
			NameInitializer()
			{
				TCHAR szTableKeyName[max_key_name_size];
				const TCHAR* pKeyTypeName = g_cKG3DRTTITypeManager.GetName(DWGCKEY_TYPE);//表的总名称
				_ASSERTE(pKeyTypeName);	
#if defined(DEBUG) | defined(_DEBUG)
				int nRet = 
#endif
					_stprintf_s(szTableKeyName, "KG3DDWGC_Key_%s_TABLE", pKeyTypeName);
				_ASSERTE(nRet > 0);
				str = szTableKeyName;
			}
		};
		static NameInitializer Temp;
		return Temp.str;
	}	

	//////////////////////////////////////////////////////////////////////////
	template<class _TypeKey>
	BOOL TKG3DDWGC_KeyTable<_TypeKey>::GetStringFromHandle( DWORD Handle, LPTSTR tcsString, size_t BufferSize)
	{
		if(! tcsString)
			return FALSE;
		KG3DENVEFF_DWGCKEY_HANDLE RegionHour = this->GetRegionHourFromHandle(Handle);
		int nRet = _stprintf_s(tcsString, BufferSize, _T("%d_%d"), RegionHour.m_Region, RegionHour.m_Hour);
		return nRet > 0;
	}
	//////////////////////////////////////////////////////////////////////////
	template<class _TypeKey>
	BOOL TKG3DDWGC_KeyTable<_TypeKey>::GetHandleFromString( LPCTSTR tcsString, size_t BufferSize, DWORD* pHandle )
	{
		KG_PROCESS_ERROR(tcsString && pHandle);
		{
			LPCTSTR pHour = _tcschr(tcsString, _T('_'));

			KG_PROCESS_ERROR(pHour);
			pHour += 1;
			KG_PROCESS_ERROR(static_cast<size_t>(pHour - tcsString) < BufferSize);
			KG_PROCESS_ERROR(BufferSize > _tcslen(tcsString));

			TCHAR tcsRegion[MAX_PATH];
			errno_t err = _tcsncpy_s(tcsRegion, tcsString, (pHour - tcsString));
			KG_PROCESS_ERROR(err == 0);
			tcsRegion[pHour - tcsString - 1] = '\0';

			INT nRegion = 0;
			INT nHour = 0;
			BOOL bRet1 = KG_CUSTOM_HELPERS::TStringToValue(tcsRegion, nRegion);
			BOOL bRet2 = KG_CUSTOM_HELPERS::TStringToValue(pHour, nHour);
			if(bRet1 && bRet2)
			{
				if(! CheckIsRegionIndexValid(nRegion))
					bRet1 = FALSE;
				if(! CheckIsHourIndexValid(nHour))
					bRet2 = FALSE;
				if (bRet1 && bRet2)
				{
					*pHandle = this->GetHandle(nRegion, nHour);
				}

			}
			return bRet1 && bRet2;
		}
Exit0:
		return FALSE;
	}
	//////////////////////////////////////////////////////////////////////////
	template<class _TypeKey>
	HRESULT TKG3DDWGC_KeyTable<_TypeKey>::SaveMapSetting( IIniFile* pIniFile, LPCTSTR pPreface)
	{
		KG_PROCESS_ERROR(pIniFile);
		{
			_ASSERTE(pPreface);
			const std::tstring& tcsTableName = this->GetSaveSectionName();
			
			TCHAR tczKeyName[MAX_PATH];
			ZeroMemory(tczKeyName, sizeof(tczKeyName));
			TCHAR tczKeyValue[MAX_PATH];
			ZeroMemory(tczKeyValue, sizeof(tczKeyValue));
			size_t ContainerSavedElems = 0;
			
			//把前缀加到KeyName里面
			size_t	KeyNameBios = _tcslen(pPreface);
			{
#if defined(DEBUG) | defined(_DEBUG)
				errno_t erro = 
#endif
					_tcscpy_s(tczKeyName, pPreface);
				_ASSERTE(erro == 0);
			}

			//保存Table本身

			for (_TypeCan::iterator it = m_Container.begin(); it != m_Container.end(); ++it)
			{
#if defined(DEBUG) | defined(_DEBUG)
				BOOL bRet = 
#endif
					this->GetStringFromHandle(it->first, tczKeyName + KeyNameBios, _countof(tczKeyName) - KeyNameBios);//贴在后面写
				_ASSERTE(bRet);
				if(KG_CUSTOM_HELPERS::SaveINIFileData(pIniFile, tcsTableName.c_str()
					, tczKeyName, static_cast<UINT>(it->second)))
				{
					++ContainerSavedElems;
				}
			}
			
			if (ContainerSavedElems != m_Container.size())
			{
				_KG3D_DEBUG_OUTPUT1("动态天气KeyTable保存不完全成功, 类型：%s", tcsTableName.c_str());
				KGLogPrintf(KGLOG_WARNING, "动态天气KeyTable保存不完全成功, 类型：%s", tcsTableName.c_str());
			}
			//保存两个容器的大小作为加载成功与否的参考
			_stprintf_s(tczKeyName, _T("%s_CSize"), pPreface);
			KG_CUSTOM_HELPERS::SaveINIFileData(pIniFile, tcsTableName.c_str(), tczKeyName, (UINT)(ContainerSavedElems));
		}
		return S_OK;
Exit0:
		
		return E_FAIL;
	}
	//////////////////////////////////////////////////////////////////////////
	template<class _TypeKey>
	HRESULT TKG3DDWGC_KeyTable<_TypeKey>::LoadMapSetting( IIniFile* pIniFile, LPCTSTR  pStruct)
	{
		_ASSERTE(pStruct);
		load_struct* pLoadStruct = reinterpret_cast<load_struct*>(const_cast<LPTSTR>(pStruct));
		_ASSERTE(pLoadStruct->pEnd && pLoadStruct->pPreface && pLoadStruct->pMapHandleHandle);
		KG_PROCESS_ERROR(pIniFile);
		{
			HRESULT hr = this->Clear();
			_ASSERTE(SUCCEEDED(hr));
			(void)hr;//避免Warning

			const std::tstring& tcsTableName = this->GetSaveSectionName();
			
			TCHAR tczKeyName[MAX_PATH];
			ZeroMemory(tczKeyName, sizeof(tczKeyName));
			TCHAR tczKeyValue[MAX_PATH];
			ZeroMemory(tczKeyValue, sizeof(tczKeyValue));
			TCHAR tczKeyNameOfSize[MAX_PATH];
			ZeroMemory(tczKeyNameOfSize, sizeof(tczKeyNameOfSize));

			size_t KeyNameBios = _tcslen(pLoadStruct->pPreface);//预处理一下KeyName去掉前缀之后的偏移

			//读入容器大小
			UINT ContainerSavedElems = 0;
			_stprintf_s(tczKeyNameOfSize, _T("%s_CSize"), pLoadStruct->pPreface);
			BOOL bRet = KG_CUSTOM_HELPERS::LoadINIFileData<UINT>(pIniFile, tcsTableName.c_str(), tczKeyNameOfSize, 0, &ContainerSavedElems);
			KG_PROCESS_ERROR(bRet);
			KG_PROCESS_SUCCESS(ContainerSavedElems == 0);

			KG_PROCESS_ERROR(0 == _tcscpy_s(tczKeyName, pLoadStruct->pPreface));
			while(KG_CUSTOM_HELPERS::GetNextKey(pIniFile, tcsTableName.c_str(), tczKeyName, tczKeyName))
			{
				if(0 == _tcscmp(tczKeyName, tczKeyNameOfSize))
					break;
				if(! KG_CUSTOM_HELPERS::LoadINIFileData(pIniFile, tcsTableName.c_str(), tczKeyName,  _T(""), tczKeyValue))
					continue;

				UINT KeyHandleOfTheKeyName = 0;
				BOOL bRet = KG_CUSTOM_HELPERS::TStringToValue(tczKeyValue, KeyHandleOfTheKeyName);
				if(! bRet)
					continue;

				std::map<key_handle_type, key_handle_type>::iterator it = pLoadStruct->pMapHandleHandle->find(KeyHandleOfTheKeyName);
				if(it == pLoadStruct->pMapHandleHandle->end())
					continue;
				
				key_handle_type KeyHandle = it->second;
				elem_type& elem = m_Alloc.get(KeyHandle);
				_ASSERTE(! m_Alloc.is_error_elem(elem));

				DWORD Handle = 0;
				bRet = this->GetHandleFromString(tczKeyName + KeyNameBios, _countof(tczKeyName) - KeyNameBios, &Handle);
				if(! bRet)
					continue;

				_TypeCan::_Pairib pib = m_Container.insert(_TypeCan::value_type(Handle, KeyHandle));
				if(! pib.second)
					continue;
				++elem.m_Ref;
			}
				
			KG_PROCESS_ERROR((size_t)ContainerSavedElems == m_Container.size());			
		}
Exit1:
		return S_OK;
Exit0:
		_KG3D_DEBUG_OUTPUT2("动态天气KeyTable读取不完全成功, 类型：%u，Table序列：%s", this->GetType(), pLoadStruct->pPreface);
		KGLogPrintf(KGLOG_WARNING, "动态天气KeyTable读取不完全成功, 类型：%u，Table序列：%s", this->GetType(), pLoadStruct->pPreface);
		return E_FAIL;
	}
};

#endif //_INCLUDE_KG3DENVEFFDW_KEYTABLE_H_
