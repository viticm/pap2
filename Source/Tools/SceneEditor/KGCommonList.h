////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGCommonList.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-4-5 20:36:18
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#pragma once

#ifndef _INCLUDE_KGCOMMONLIST_H_
#define _INCLUDE_KGCOMMONLIST_H_
#include "KGHelperClasses.h"
#include "KGStringFunctionsEx.h"
////////////////////////////////////////////////////////////////////////////////

namespace KSCENEEDITOR_HELPERCLASSES
{
	namespace COMMON_LIST
	{
		enum LOAD_STRATEGE
		{
			LOAD_STRATAGE_DYNAMIC = 0,	///随机存取类型
			LOAD_STRATAGE_LOADALL = 1,	///一次会载入所有类型
			LOAD_STRATAGE_ITERATOR = 2, ///以迭代器的形式顺序存取
		};
		enum GET_NAME_STRATEGE
		{
			GET_NAME_BY_DEFAULTNAME = 0,	///由默认名字加序号得到名字
			GET_NAME_BY_MEMBERFUNC = 1,		///由成员函数得到名字
			GET_NAME_BY_VALUE = 2,			///由数字翻译得到名字
		};
		template<class _Manager, class _Member, DWORD _LOAD_STRATEGE, DWORD _NAME_STRATEGE>
		struct TKGCommonListTrait 
		{
			typedef _Manager _Manager_Type;
			typedef _Member  _Member_Type;
			typedef _Manager* (*_Func_ManagerGetter)(DWORD_PTR wParam, DWORD_PTR lParam);
			typedef HRESULT (_Manager::*_MemFunc_MemberGetter)(INT nIndex, _Member* pMember);
			typedef HRESULT (_Manager::*_MemFunc_MemberCountGetter)(INT* pCount);
			typedef HRESULT (_Manager::*_MemFunc_AllMemberGetter)(_Member* pMember, INT nBufferCount);
			typedef HRESULT (_Manager::*_MemFunc_MemberNameGetter)(INT nIndex, LPCTSTR czNameBuffer, INT nBufferSize);
			//////////////////////////////////////////////////////////////////////////
			enum
			{
				LOAD_STRATEGE_TRAIT = _LOAD_STRATEGE,
				NAME_STRATEGE_TRAIT = _NAME_STRATEGE,
			};
			_Func_ManagerGetter m_FuncManagerGetter;
			_MemFunc_MemberGetter m_FuncMemberGetter;
			_MemFunc_MemberCountGetter m_FuncMemberCountGetter;
			_MemFunc_AllMemberGetter m_FuncAllMemberGetter;
			_MemFunc_MemberNameGetter m_FuncMemberNameGetter;
			LPCTSTR m_czDefaultName;
			//////////////////////////////////////////////////////////////////////////
			TKGCommonListTrait(_Func_ManagerGetter FuncManagerGetter
				, _MemFunc_MemberGetter FuncMemberGetter
				, _MemFunc_MemberCountGetter FuncMemberCountGetter
				, _MemFunc_AllMemberGetter FuncAllMemberGetter
				, _MemFunc_MemberNameGetter FuncMemberNameGetter
				, LPCTSTR czDefaultName)
				:m_FuncManagerGetter(FuncManagerGetter)
				,m_FuncMemberGetter(FuncMemberGetter)
				,m_FuncMemberCountGetter(FuncMemberCountGetter)
				,m_FuncAllMemberGetter(FuncAllMemberGetter)
				,m_FuncMemberNameGetter(m_FuncMemberNameGetter)
			{

			}
			TKGCommonListTrait(const TKGCommonListTrait& Other)
				:m_FuncManagerGetter(Other.m_FuncManagerGetter)
				,m_FuncMemberGetter(Other.m_FuncMemberGetter)
				,m_FuncMemberCountGetter(Other.m_FuncMemberCountGetter)
				,m_FuncAllMemberGetter(Other.m_FuncAllMemberGetter)
				,m_FuncMemberNameGetter(Other.m_FuncMemberNameGetter)
				,m_czDefaultName(Other.m_czDefaultName)
			{

			}
		};
		//////////////////////////////////////////////////////////////////////////


		//////////////////////////////////////////////////////////////////////////
		struct KGCommonListHelperBase
		{
		public:
			virtual ~KGCommonListHelperBase(){}
			virtual INT		GetCount() = 0;
			virtual BOOL	GetMemberData(INT nIndex, DWORD_PTR& dwRet) = 0;///出来的是一个Data的指针，需要立刻用
			virtual BOOL	Reset() = 0;
			virtual BOOL	GetName(INT nIndex, CString& csRet) = 0;
			virtual KGCommonListHelperBase* Clone() const= 0;
			virtual void	SetManagerGetterParam(DWORD_PTR WParam, DWORD_PTR LParam) = 0;
		};
		template<class _Trait>
		class TKGCommonListHelper : public KGCommonListHelperBase
		{
		public:
			typedef typename _Trait::_Manager_Type _Manager;
			typedef typename _Trait::_Member_Type  _Member;
		private:
			_Trait m_Trait;
			_Member* m_pMemberArray;
			INT	m_nMemberCount;
			DWORD_PTR m_dwGetManagerWParam;
			DWORD_PTR m_dwGetManagerLParam;
		public:
			enum
			{
				MAX_MEMBER = 1024,
			};			
			TKGCommonListHelper(const _Trait& Trait, DWORD_PTR dwGetManagerWParam = 0, DWORD_PTR dwGetManagerLParam = 0)
				:m_Trait(Trait)
				,m_pMemberArray(NULL)
				,m_nMemberCount(0)
				,m_dwGetManagerWParam(dwGetManagerWParam)
				,m_dwGetManagerLParam(dwGetManagerLParam)
			{

			}
			TKGCommonListHelper(const TKGCommonListHelper& Other)
				:m_Trait(Other.m_Trait)
				,m_pMemberArray(NULL)
				,m_nMemberCount(0)
				,m_dwGetManagerWParam(Other.m_dwGetManagerWParam)
				,m_dwGetManagerLParam(Other.m_dwGetManagerLParam)
			{

			}
			virtual ~TKGCommonListHelper()
			{
				SAFE_DELETE_ARRAY(m_pMemberArray);
			}
			virtual KGCommonListHelperBase* Clone()const
			{
				TKGCommonListHelper* pTemp = new TKGCommonListHelper(m_Trait);
				if (!pTemp)
				{
					return NULL;
				}
				return pTemp;
			}
			inline _Manager* GetManager()
			{
				_Manager* pManager = (*(m_Trait.m_FuncManagerGetter))(m_dwGetManagerWParam, m_dwGetManagerLParam);
				return pManager;
			}
			virtual INT GetCount()
			{
				_Manager* pManager = GetManager();
				KG_PROCESS_ERROR(pManager && m_Trait.m_FuncMemberCountGetter);
				{
					INT nRet;
					HRESULT hr = (pManager->*(m_Trait.m_FuncMemberCountGetter))(&nRet);
					KG_COM_PROCESS_ERROR(hr);
					_ASSERTE(nRet >=0);
					return nRet;
				}
Exit0:
				return -1;
			}
			virtual void	SetManagerGetterParam(DWORD_PTR WParam, DWORD_PTR LParam)
			{
				m_dwGetManagerWParam = WParam;
				m_dwGetManagerLParam = LParam;
			};
		private:
			BOOL TGetMemberData(KG_CUSTOM_HELPERS::TNumToType<LOAD_STRATAGE_LOADALL>, INT nIndex, DWORD_PTR& dwRet)
			{
				if (!m_pMemberArray)
				{
					BOOL bRet = GetAllMembers();
					KG_PROCESS_ERROR(bRet);
				}
				KG_PROCESS_ERROR(m_pMemberArray && nIndex < m_nMemberCount);
				_KG3D_DEBUG_STATIC_CHECK_T(sizeof(_Member*) <= sizeof(DWORD_PTR), TKGCommonListHelper_GetMember_SizeUnMatch);
				dwRet = (DWORD_PTR)(&m_pMemberArray[nIndex]);
				return TRUE;
Exit0:
				return FALSE;
			}			
			BOOL TGetMemberData(KG_CUSTOM_HELPERS::TNumToType<LOAD_STRATAGE_DYNAMIC>, INT nIndex, DWORD_PTR& dwRet)
			{
				static _Member Member;
				_Manager* pManager = GetManager();
				KG_PROCESS_ERROR(pManager && m_Trait.m_FuncMemberGetter);
				{
					HRESULT hr = (pManager->*(m_Trait.m_FuncMemberGetter))(nIndex, &Member);
					KG_COM_PROCESS_ERROR(hr);
					dwRet =  (DWORD_PTR)(&Member);
					return TRUE;
				}
Exit0:
				return FALSE;
			}
		public:
			virtual BOOL GetMemberData(INT nIndex, DWORD_PTR& dwRet)
			{
				return TGetMemberData(KG_CUSTOM_HELPERS::TNumToType<_Trait::LOAD_STRATEGE_TRAIT>(), nIndex, dwRet);
			}
			virtual BOOL Reset()
			{
				SAFE_DELETE_ARRAY(m_pMemberArray);
				m_nMemberCount = 0;
				return TRUE;
			}
			BOOL GetAllMembers()
			{
				_Manager* pManager = GetManager();
				_Member* pTempMemberArray = NULL;
				KG_PROCESS_ERROR(m_Trait.m_FuncAllMemberGetter && pManager);
				{
					INT nCount = GetCount();
					KG_PROCESS_ERROR(nCount >= 0 && nCount <= MAX_MEMBER);

					pTempMemberArray = new _Member[nCount];
					KG_PROCESS_ERROR(pTempMemberArray);
					ZeroMemory(pTempMemberArray, sizeof(_Member)*nCount);
					//////////////////////////////////////////////////////////////////////////

					m_nMemberCount = 0;
					HRESULT hr = (pManager->*(m_Trait.m_FuncAllMemberGetter))(pTempMemberArray, nCount);
					KG_COM_PROCESS_ERROR(hr);
					SAFE_DELETE_ARRAY(m_pMemberArray);
					m_pMemberArray = pTempMemberArray;
					m_nMemberCount = nCount;
					return TRUE;
				}				
Exit0:
				SAFE_DELETE_ARRAY(pTempMemberArray);
				return FALSE;
			}
			//////////////////////////////////////////////////////////////////////////
		private:
			///因为函数模板只能重载不能偏特化，所以要间接利用DWORDToType实现用DWORD类型的策略进行函数选择
			///好处是没有被选择的函数在类中不会被实例化，减少了类大小，同时也能避免Switch-Case
			BOOL TGetName(KG_CUSTOM_HELPERS::TNumToType<GET_NAME_BY_MEMBERFUNC>, INT nIndex, CString& csRet)
			{
				CString TempString;
				_Manager* pManager = GetManager();
				KG_PROCESS_ERROR(pManager && m_Trait.m_FuncMemberNameGetter);
				HRESULT hr = (pManager->*(m_Trait.m_FuncMemberNameGetter))(nIndex, TempString.GetBuffer(64), 64);
				TempString.ReleaseBuffer();
				KG_COM_PROCESS_ERROR(hr);
				csRet = TempString;
				return TRUE;
Exit0:
				return FALSE;	
			}
			BOOL TGetName(KG_CUSTOM_HELPERS::TNumToType<GET_NAME_BY_DEFAULTNAME>, INT nIndex, CString& csRet)
			{
				CString TempString;
				using namespace KG_CUSTOM_HELPERS;
				INT nBufferToGet = TValueStringMinBufferSize<INT>::Result;
				BOOL bRet = TValueToString(nIndex, TempString.GetBuffer(nBufferToGet), nBufferToGet);
				TempString.ReleaseBuffer();
				KG_PROCESS_ERROR(bRet);
				{
					CString csDefault(m_Trait.m_czDefaultName);
					csRet = csDefault + TempString;
					return TRUE;
				}
Exit0:
				return FALSE;		
			}
			BOOL TGetName(KG_CUSTOM_HELPERS::TNumToType<GET_NAME_BY_VALUE>, INT nIndex, CString& csRet)
			{	
				using namespace KG_CUSTOM_HELPERS;
				DWORD_PTR dwTemp = 0;
				BOOL bRet = this->GetMemberData(nIndex, dwTemp);
				KG_PROCESS_ERROR(bRet && dwTemp);
				{
					CString TempString;
					_Member* pMember = (_Member*)dwTemp;
					INT nBufferToGet = TValueStringMinBufferSize<_Member>::Result;
					bRet = TValueToString(*pMember, TempString.GetBuffer(nBufferToGet), nBufferToGet);
					TempString.ReleaseBuffer();
					KG_PROCESS_ERROR(bRet);
					csRet = TempString;
					return TRUE;
				}
Exit0:
				return FALSE;			
			};
			//////////////////////////////////////////////////////////////////////////
		public:
			virtual BOOL  GetName(INT nIndex, CString& csRet)
			{
				return TGetName(KG_CUSTOM_HELPERS::TNumToType<_Trait::NAME_STRATEGE_TRAIT>(), nIndex, csRet);
			}
		};
		//////////////////////////////////////////////////////////////////////////
		class KGCommonList : public CListBox
		{
			DECLARE_DYNAMIC(KGCommonList)
		private:
			KGCommonListHelperBase* m_pHelper;
		public:	
			KGCommonList(const KGCommonListHelperBase& Helper);
			virtual ~KGCommonList();
		private:
			VOID ReloadFuncBody();		
		public:
			VOID SetManagerGetterParam(DWORD_PTR WParam, DWORD_PTR LParam);
			VOID Reload()
			{
				ReloadFuncBody();				
			}

		protected:
			DECLARE_MESSAGE_MAP()
		};
	};///COMMON_LIST
};///KSCENEEDITOR_HELPERCLASSES
namespace KSH = KSCENEEDITOR_HELPERCLASSES;

#endif //_INCLUDE_KGCOMMONLIST_H_



// KGCommonList




