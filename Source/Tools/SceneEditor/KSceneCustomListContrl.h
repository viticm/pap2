////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneCustomListContrl.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-11-23 9:30:38
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KSCENECUSTOMLISTCONTRL_H_
#define _INCLUDE_KSCENECUSTOMLISTCONTRL_H_
//#include "SceneEditorCommon.h"
#include "IEKG3DTypes.h"
#include "KGHelperClasses.h"
#include "KGStringFunctionsEx.h" 
////////////////////////////////////////////////////////////////////////////////
#pragma warning(disable : 4121)	//如果不这样的话，在用成员指针的地方会出'symbol' : alignment of a member was sensitive to packing，Pack敏感
namespace KSCENEEDITOR_HELPERCLASSES
{
	namespace COMMON_CTRL
	{		
		//用virtual接口可以强制Policy的实现，因为只是接口限制，所以不需要纯虚析构，Release情况下，virtual可以去掉以加速
		template<class _TypeContainer>
		struct ContainerGetterBase 
		{
#if defined(_DEBUG) | defined(DEBUG)
			virtual _TypeContainer* GetContainer() = 0;
#endif
		};
		template<class _TypeContainer, typename _TypeElem>
		struct  ElemGetter
		{
			typedef HRESULT (_TypeContainer::*_MemFunc_MemberCountGetter)(INT* pCount);
			typedef HRESULT (_TypeContainer::*_MemFunc_MemberGetter)(INT nIndex, _TypeElem* pMember);
			typedef HRESULT (_TypeContainer::*_MemFunc_AllMemberGetter)(_TypeElem* pMember, INT nBufferCount);

#if defined(_DEBUG) | defined(DEBUG)
			virtual INT	GetElemCount(_TypeContainer* pContainer) = 0;
			virtual HRESULT UpdateElem(_TypeContainer* pContainer) = 0;			
			virtual HRESULT GetElem(_TypeContainer* pContainer, INT nIndex, _TypeElem* pElem) = 0;	//_TypeElem必须是可以复制的类型
#endif
		};
		template<class _TypeContainer, typename _TypeElem>
		struct NameGetter 
		{
#if defined(_DEBUG) | defined(DEBUG)
			virtual HRESULT	GetName(_TypeContainer* pContainer, INT nIndex, _TypeElem* pElem, CString& csRet) = 0;
#endif
		};
		//取得Container的策略
		template<DWORD dwType1, DWORD dwType2>
		class GetContainerByDWORD	//策略1，用两个DWORD
			: public ContainerGetterBase<typename IEKG3DTYPE::TKG3DDWORD_TO_IEINTERFACE<dwType1, dwType2>::Result* >
		{
		public:
			DWORD_PTR m_WParam;//利用这两个就可以更改Container对象
			DWORD_PTR m_LParam;
		
			typedef typename IEKG3DTYPE::TKG3DDWORD_TO_IEINTERFACE<dwType1, dwType2>::Result* TypeReturnType;
	
			GetContainerByDWORD():m_WParam(NULL), m_LParam(NULL){};
			GetContainerByDWORD(DWORD_PTR WParam, DWORD_PTR LParam):m_WParam(WParam),m_LParam(LParam){}
			GetContainerByDWORD(const GetContainerByDWORD& Other):m_WParam(Other.m_WParam),m_LParam(Other.m_LParam){}
			GetContainerByDWORD& operator=(const GetContainerByDWORD& Other){m_WParam = Other.m_WParam; m_LParam = Other.m_LParam;return *this;}

			//ContainerPolicy必须实现的方法
			TypeReturnType GetContainer()
			{
				if (g_pEngineManager)
				{
					LPVOID	pInterface;
					HRESULT hr = g_pEngineManager->GetIEInterface(dwType1, dwType2, m_WParam, m_LParam, &pInterface);
					if (SUCCEEDED (hr))
						return static_cast<TypeReturnType>(pInterface);
				}	
				return NULL;
			}
		};

		template<class _TypeInterface>
		class	GetContainerByInterfaceName
			: public ContainerGetterBase<_TypeInterface>
		{
		public:
			DWORD_PTR m_WParam;
			DWORD_PTR m_LParam;
		
			typedef _TypeInterface* TypeReturnType;
			typedef IEKG3DTYPE::TKG3DIEINTERFACE_TO_DWORD<_TypeInterface> TypeReturnTypeTrait;
			enum{ _DW1 = TypeReturnTypeTrait::_type_dword1, _DW2 = TypeReturnTypeTrait::_type_dword2};

			GetContainerByInterfaceName():m_WParam(NULL), m_LParam(NULL){}
			GetContainerByInterfaceName(DWORD_PTR WParam, DWORD_PTR LParam):m_WParam(WParam),m_LParam(LParam){}
			GetContainerByInterfaceName(const GetContainerByInterfaceName& Other):m_WParam(Other.m_WParam),m_LParam(Other.m_LParam){}
			GetContainerByInterfaceName& operator=(const GetContainerByInterfaceName& Other){m_WParam = Other.m_WParam; m_LParam = Other.m_LParam;return *this;}

			TypeReturnType GetContainer()
			{
				if (g_pEngineManager)
				{
					LPVOID	pInterface;
					HRESULT hr = g_pEngineManager->GetIEInterface(_DW1, _DW2, m_WParam, m_LParam, &pInterface);
					if (SUCCEEDED (hr))
						return static_cast<TypeReturnType>(pInterface);	
				}
				return NULL;
			}
		};

		//取得元素的策略
		template<class _TypeContainer, typename _TypeElem>
		class GetElemDynamic	//策略1，随机读取
			: public ElemGetter<_TypeContainer, _TypeElem>
		{
		public:
			typedef typename ElemGetter<_TypeContainer, _TypeElem>::_MemFunc_MemberCountGetter _MemFunc_MemberCountGetter;
			typedef typename ElemGetter<_TypeContainer, _TypeElem>::_MemFunc_MemberGetter _MemFunc_MemberGetter;
			
		private:
			union
			{
				_MemFunc_MemberCountGetter m_MemberCountGetter;
				DWORD_PTR	A;	//撑位用的，避免C4121描述的那种由于Pack造成size不一致的情况
			};
			union
			{
				_MemFunc_MemberGetter m_MemGetter;
				DWORD_PTR	A;
			};
	
		public:
			GetElemDynamic(_MemFunc_MemberCountGetter A, _MemFunc_MemberGetter B):m_MemberCountGetter(A), m_MemGetter(B){}
			GetElemDynamic(const GetElemDynamic& Other):m_MemberCountGetter(Other.m_MemberCountGetter),m_MemGetter(Other.m_MemGetter){}
			GetElemDynamic& operator=(const GetElemDynamic& Other){m_MemberCountGetter = Other.m_MemberCountGetter;m_MemGetter = Other.m_MemGetter;}

			//ElemGetter Policy必须实现的方法
			HRESULT UpdateElem(_TypeContainer* pContainer){return S_OK;}
			INT	GetElemCount(_TypeContainer* pContainer)
			{
				_ASSERTE(m_MemberCountGetter && pContainer);
				INT nCount = 0;
				HRESULT hr = (pContainer->*m_MemberCountGetter)(&nCount);
				if(FAILED(hr))
					return -1;
				return nCount;
			}
			HRESULT GetElem(_TypeContainer* pContainer, INT nIndex, _TypeElem* pElem)
			{
				_ASSERTE(pContainer && m_MemGetter && pElem);
				return (pContainer->*m_MemGetter)(nIndex, pElem);
			}

		};

		template<class _TypeContainer, typename _TypeElem, unsigned _MaxElem = 100>
		class GetElemOnceAll	//策略2，一次载入所有的元素，然后分次获取。里面是动态分配的内存小心使用，小心分配了过多内存
			: public ElemGetter<_TypeContainer, _TypeElem>
		{
		public:
			typedef typename ElemGetter<_TypeContainer, _TypeElem>::_MemFunc_MemberCountGetter _MemFunc_MemberCountGetter;
			typedef typename ElemGetter<_TypeContainer, _TypeElem>::_MemFunc_AllMemberGetter _MemFunc_AllMemberGetter;
		private:
			_TypeElem* m_pElem;
			INT	m_nCount;
			
			union
			{
				_MemFunc_MemberCountGetter m_MemberCountGetter;
				DWORD_PTR A;
			};
			
			union
			{
				_MemFunc_AllMemberGetter	m_MemberAllMemberGetter;
				DWORD_PTR A;
			};
			

			enum{max_elem = _MaxElem,};
		public:

			GetElemOnceAll(_MemFunc_MemberCountGetter A, _MemFunc_AllMemberGetter B):m_pElem(NULL), m_nCount(0),m_MemberCountGetter(A),m_MemberAllMemberGetter(B){}
			GetElemOnceAll(const GetElemOnceAll& Other):m_pElem(NULL),m_nCount(0),m_MemberCountGetter(Other.m_MemberCountGetter),m_MemberAllMemberGetter(Other.m_MemberAllMemberGetter){}
			GetElemOnceAll& operator=(const GetElemOnceAll& Other)
			{
				m_MemberCountGetter = Other.m_MemberCountGetter;
				m_MemberAllMemberGetter = Other.m_MemberAllMemberGetter;
				SAFE_DELETE_ARRAY(m_pElem);
				m_nCount = 0;
			}
			~GetElemOnceAll(){SAFE_DELETE_ARRAY(m_pElem);}

			//ElemGetter Policy必须实现的方法
			INT	GetElemCount(_TypeContainer* pContainer)
			{
				_ASSERTE(m_MemberCountGetter && pContainer);
				INT nCount = 0;
				HRESULT hr = (pContainer->*m_MemberCountGetter)(&nCount);
				if(FAILED(hr))
					return -1;
				return nCount;
			}
			HRESULT UpdateElem(_TypeContainer* pContainer)//刷新内部元素，如果不是一次载入的策略的话，可以什么都不做
			{
				_ASSERTE(pContainer && m_MemberCountGetter && m_MemberAllMemberGetter);
				SAFE_DELETE_ARRAY(m_pElem);
				m_nCount = 0;
				
				HRESULT hr = E_FAIL;
				INT nSpaceToAlloc = this->GetElemCount(pContainer);
				
				KG_PROCESS_SUCCESS(nSpaceToAlloc == 0);
				KG_PROCESS_ERROR(nSpaceToAlloc > 0 && nSpaceToAlloc <= max_elem);
				{
					_KG3D_DEBUG_STATIC_CHECK_T(sizeof(_TypeElem) <= sizeof(DWORD_PTR)*10, SizeTooLargeToUseThisTemplate);//不允许Element过大
					
					m_pElem = new _TypeElem[nSpaceToAlloc];
					KG_PROCESS_ERROR(m_pElem);
					m_nCount = nSpaceToAlloc;

					hr = (pContainer->*m_MemberAllMemberGetter)(m_pElem, nSpaceToAlloc);
					KG_COM_PROCESS_ERROR(hr);
				}
Exit1:
				return S_OK;
Exit0:
				SAFE_DELETE_ARRAY(m_pElem);
				m_nCount = 0;
				return E_FAIL;
			}
			HRESULT GetElem(_TypeContainer* pContainer, INT nIndex, _TypeElem* pElem)
			{
				_ASSERTE(pContainer && pElem);
				if (! m_pElem || nIndex < 0 || nIndex >= m_nCount)
					return E_FAIL;
				*pElem = m_pElem[nIndex];
				return S_OK;
			}
		};

		//取得元素名称的策略
		template<class _TypeContainer, typename _TypeElem>//策略1，从内部得到
		class GetNameByMemberFunc
			: public NameGetter<_TypeContainer, _TypeElem>
		{
		public:
			typedef HRESULT (_TypeContainer::*_MemFunc_MemberNameGetter)(INT nIndex, LPCTSTR czNameBuffer, INT nBufferSize);
		private:
			_MemFunc_MemberNameGetter m_MemberFunc;
		public:
			GetNameByMemberFunc(_MemFunc_MemberNameGetter Func):m_MemberFunc(Func){}
			GetNameByMemberFunc(const GetNameByMemberFunc& Other):m_MemberFunc(Other.m_MemberFunc){}
			GetNameByMemberFunc& operator=(const GetNameByMemberFunc& Other){m_MemberFunc = Other.m_MemberFunc;}

			//作为GetName Policy必须实现的方法
			HRESULT	GetName(_TypeContainer* pContainer, INT nIndex, _TypeElem* pElem, CString& csRet)
			{
				_ASSERTE(pContainer && nIndex >= 0 && czNameBuffer && m_MemberFunc);
				HRESULT hr = (pContainer->*m_MemberFunc)(nIndex, csRet.GetBuffer(MAX_PATH), MAX_PATH);
				csRet.ReleaseBuffer();
				return hr;
			}
		};
		template<typename _TypeContainer, typename _TypeElem>
		class GetNameByDefault	//策略2，用默认名称加一个序列号
			: public NameGetter<_TypeContainer, _TypeElem>
		{
		private:
			CString m_DefaultName;
		public:
			GetNameByDefault(){}//m_DefaultName就是""
			GetNameByDefault(LPCTSTR lpDefName):m_DefaultName(lpDefName){}
			GetNameByDefault(const GetNameByDefault& Other):m_DefaultName(Other.m_DefaultName){}
			GetNameByDefault& operator= (const GetNameByDefault& Other){m_DefaultName = Other.m_DefaultName;}

			HRESULT	GetName(_TypeContainer* pContainer, INT nIndex, _TypeElem* pElem, CString& csRet)
			{
				_ASSERTE(nIndex >= 0);
				TCHAR csTemp[MAX_PATH];
				if (! KG_CUSTOM_HELPERS::TValueToString(nIndex, csTemp))
					return E_FAIL;
				csRet = m_DefaultName;
				csRet += csTemp;
				return S_OK;
			}
		};
		
		template<class _TypeContainer, class _TypeElem>
		class GetNameByValue	//策略3，用值转换，要求值能够是TValueToString能接受的类型
			: public NameGetter<_TypeContainer, _TypeElem>
		{
		public:
			HRESULT	GetName(_TypeContainer* pContainer, INT nIndex, _TypeElem* pElem, CString& csRet)
			{
				_ASSERTE(pElem);
				_KG3D_DEBUG_STATIC_CHECK_T(sizeof(_TypeElem) <= sizeof(DWORD_PTR), ElemSizeTooLarge);
				return KG_CUSTOM_HELPERS::TValueToString(*pElem, csRet) ? S_OK : E_FAIL;
			}
				 
		};

		
		// KSceneCustomListContrlBase

		class KSceneCustomListContrlBase : public CListBox
		{
			DECLARE_DYNAMIC(KSceneCustomListContrlBase)

		public:
			KSceneCustomListContrlBase();
			virtual ~KSceneCustomListContrlBase();
			int SetCurSel(int nSelect);//不管3721都向父窗口发SelChange消息

		protected:
			DECLARE_MESSAGE_MAP()
		};//KSceneCustomListContrlBase

		
		
		template<class _TypeContainer
					, class _TypeElem
					, class _ContainerGetter = GetContainerByInterfaceName<_TypeContainer>
					, class _ElemGetter = GetElemDynamic<_TypeContainer, _TypeElem>
					, class _NameGetter = GetNameByValue<_TypeContainer, _TypeElem>
				>
		class KSceneCustomListContrl : public KSceneCustomListContrlBase
		{
		private:
			_ContainerGetter	m_ContainerGetter;
			_ElemGetter	m_ElemGetter;
			_NameGetter	m_NameGetter;

			//吸取类型信息——是否一个指针类型
			template<typename _Type>
			struct TIsAPointerType{enum{result = false,};};
			template<typename _Type>
			struct TIsAPointerType<_Type*>{enum{result = true};};
			template<typename _Type>
			struct TIsAPointerType<const _Type*>{enum{result = false};};
		private:
			HRESULT GetElemValueByCurSelPrivate(_TypeElem* pElemToGet)
			{
				_TypeContainer* pContainer = m_ContainerGetter.GetContainer();
				KG_PROCESS_ERROR(pContainer);
				{
					INT nIndex = this->GetCurSel();
					KG_PROCESS_ERROR(nIndex >= 0);

					INT nElemCount = m_ElemGetter.GetElemCount(pContainer);
					KG_PROCESS_ERROR(nIndex < nElemCount );

					return m_ElemGetter.GetElem(pContainer, nIndex, pElemToGet);
				}
Exit0:
				return E_FAIL;
			}
			template<bool>
			_TypeElem GetElemValueByCurSel_ByThatIsTypeAPointer(){return NULL;}

			template<>
			_TypeElem GetElemValueByCurSel_ByThatIsTypeAPointer<true>()
			{
				_TypeElem ElemTemp;
				if(SUCCEEDED(GetElemValueByCurSelPrivate(&ElemTemp)))
					return ElemTemp;
				return NULL;
			}
		public:
			KSceneCustomListContrl(){}
			KSceneCustomListContrl(const _ContainerGetter& containerGetter
									, const _ElemGetter& elemGetter
									, const _NameGetter& nameGetter)
									:m_ContainerGetter(containerGetter)
									,m_ElemGetter(elemGetter)
									,m_NameGetter(nameGetter)
			{

			}
			_ContainerGetter& GetContainerGetter(){return m_ContainerGetter;}
			_ElemGetter& GetElemGetter(){return m_ElemGetter;}
			_NameGetter& GetNameGetter(){return m_NameGetter;}

			VOID Clear(){this->ResetContent();}
			HRESULT Reload()
			{
				_KG3D_DEBUG_STATIC_CHECK_T(sizeof(_TypeElem) <= sizeof(DWORD_PTR) * 10, SizeTooLarge);
				Clear();
				HRESULT hr = E_FAIL;
				INT nElemCount = 0;
				_TypeElem ElemTemp;
				CString csTemp;

				_TypeContainer* pContainer = m_ContainerGetter.GetContainer();
				KG_PROCESS_ERROR(pContainer);

				hr = m_ElemGetter.UpdateElem(pContainer);
				KG_COM_PROCESS_ERROR(hr);

				nElemCount = m_ElemGetter.GetElemCount(pContainer);
				KG_PROCESS_SUCCESS(nElemCount == 0);
				KG_PROCESS_ERROR(nElemCount > 0);
				
				for(INT i = 0; i < nElemCount; i++)
				{
					hr = m_ElemGetter.GetElem(pContainer, i, &ElemTemp);
					if (SUCCEEDED(hr))
					{
						hr = m_NameGetter.GetName(pContainer, i, &ElemTemp, csTemp);
						if (SUCCEEDED(hr))
						{
							int nAddedIndex = this->AddString(csTemp);
							if (nAddedIndex >= 0)
							{
								this->SetItemData(nAddedIndex, i);///只存放Index，以后随时可以用m_ElemGetter来重新取得数据
							}
							else
							{
								hr = E_FAIL;
							}
						}
					}
#if defined(_DEBUG) | defined(DEBUG)
					if(FAILED(hr))
					{
						_ASSERTE(NULL && "读取失败");
					}
#endif
				}//End of for
				if (this->GetCount() > 0)
				{
					this->SetCurSel(0);
				}

Exit1:
				return S_OK;
Exit0:
				return E_FAIL;
			}
			HRESULT GetElemValueByCurSel(_TypeElem* pElemToGet){return GetElemValueByCurSelPrivate(pElemToGet);}
			//下面这个版本方便指针型的Elem的GetSet
			_TypeElem GetElemValueByCurSel(){return GetElemValueByCurSel_ByThatIsTypeAPointer<TIsAPointerType<_TypeElem>::result >();}//如果TypeElem不是指针类型就会报错
		private:
			KSceneCustomListContrl(const KSceneCustomListContrl&);
			KSceneCustomListContrl& operator=(const KSceneCustomListContrl&);

		};


	};//COMMON_CTRL
	
};///KSCENEEDITOR_HELPERCLASSES
namespace KSH = KSCENEEDITOR_HELPERCLASSES;
#pragma warning(default : 4121)
#endif //_INCLUDE_KSCENECUSTOMLISTCONTRL_H_
