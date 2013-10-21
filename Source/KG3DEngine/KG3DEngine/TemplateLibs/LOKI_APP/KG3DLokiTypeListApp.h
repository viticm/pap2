////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DLokiTypeListApp.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-4-30 10:03:00
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DLOKITYPELISTAPP_H_
#define _INCLUDE_KG3DLOKITYPELISTAPP_H_

////////////////////////////////////////////////////////////////////////////////
#include "KG3DLokiBase.h"
#include "KG3DLokiTypeTrait.h"
namespace LOKI_APP
{
	namespace TL_APP
	{
		//////////////////////////////////////////////////////////////////////////
		template<class _TypeList>
		struct AddElementToBuffer;

		template<typename _TYPE1, typename _TYPE2>
		struct AddElementToBuffer<TTypeList<_TYPE1, _TYPE2> >
		{
			AddElementToBuffer(void* pBuffer, int nArraySize)
			{
				_ASSERTE(pBuffer&& nArraySize >= sizeof(_TYPE1));
				_ASSERTE(*(char*)pBuffer == NULL);
				new(pBuffer) _TYPE1;///placement new
				AddElementToBuffer<_TYPE2>((char*)pBuffer+sizeof(_TYPE1), nArraySize-sizeof(_TYPE1));
			}
		};
		template<>
		struct AddElementToBuffer<NullType>
		{
			AddElementToBuffer(void* pBuffer, int nArraySize)
			{
				_ASSERTE(nArraySize >= 0);
			}
		};
		//////////////////////////////////////////////////////////////////////////
		template<class _ListType>
		struct DelElementFromBuffer;

		template<typename _TYPE1, typename _TYPE2>
		struct DelElementFromBuffer<TTypeList<_TYPE1, _TYPE2> >
		{
			typedef TTypeList<_TYPE1, _TYPE2> typelist_type;
			DelElementFromBuffer(void* pBuffer, int nArraySize)
			{
				_ASSERTE(pBuffer && nArraySize >= sizeof(_TYPE1));
				_TYPE1* pTemp = (_TYPE1*)pBuffer;
				pTemp->~_TYPE1();///placement new 强制释放语法
				DelElementFromBuffer<_TYPE2>((char*)pBuffer + sizeof(_TYPE1), nArraySize - sizeof(_TYPE1));
				ZeroMemory(pBuffer, sizeof(_TYPE1));
			}
		};

		template<>
		struct DelElementFromBuffer<NullType>
		{
			DelElementFromBuffer(void* pBuffer, int nArraySize)
			{
				_ASSERTE(nArraySize >= 0);
			}
		};
		//////////////////////////////////////////////////////////////////////////
		template<class _TypeList, typename _VirtualType>
		struct TVirtualTypeChecker;

		template<typename _VirtualType>
		struct TVirtualTypeChecker<NullType, _VirtualType> 
		{
			enum{result = false,};
		};

		template<typename _Type1, typename _Type2, typename _VirtualType>
		struct TVirtualTypeChecker<TTypeList<_Type1, _Type2>, _VirtualType>
		{
		private:
			enum{ temp = SUPERSUBCLASS(_VirtualType, _Type1), };
		public:
			enum{ result = temp && TVirtualTypeChecker<_Type2, _VirtualType>::result,};
		};

		template<typename _Type1, typename _VirtualType>
		struct TVirtualTypeChecker<TTypeList<_Type1, NullType>, _VirtualType>
		{
			enum{ result  = SUPERSUBCLASS(_VirtualType, _Type1), };
		};
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		namespace Private
		{
			template<typename _Type> struct MakePointerType
			{
				typedef _Type* Result;
			};
			template<typename _Type>
			struct MakePointerType<_Type*>
			{
				typedef _Type* Result;
			};
		};
		///检查指针强转是否会成功
		template<class _TypeList, class _TypeToCheck>	///必须强制运行一次,不能编译器决定
		struct TTypePtrStaticConvertChecker;

		template<typename _Type1, typename _Type2, typename _TypeToCheck>
		struct TTypePtrStaticConvertChecker<TTypeList<_Type1, _Type2>, _TypeToCheck >
		{
		protected:
			typedef typename Private::MakePointerType<_Type1>::Result T1PointerType;
			typedef typename LOKI_APP::TTypeTraits<_TypeToCheck>::NonConstType TToCheckNonConstType;
			typedef typename Private::MakePointerType<_TypeToCheck>::Result TToCheckPointerType;
			bool m_bResult;
		public:
			TTypePtrStaticConvertChecker()
				:m_bResult(false)
			{
				TTypePtrStaticConvertChecker<_Type2, _TypeToCheck> NextType;
				if (NextType)
				{
					m_bResult = (void*)(static_cast<TToCheckPointerType>((T1PointerType)(0x00000001))) == (void*)(0x00000001);
				}			
			}
			operator bool(){return m_bResult;}
		};

		template<typename _Type1, typename _TypeToCheck>
		struct TTypePtrStaticConvertChecker<TTypeList<_Type1, NullType>, _TypeToCheck >
		{
		protected:
			typedef typename Private::MakePointerType<_Type1>::Result T1PointerType;
			typedef typename LOKI_APP::TTypeTraits<_TypeToCheck>::NonConstType TToCheckNonConstType;
			typedef typename Private::MakePointerType<_TypeToCheck>::Result TToCheckPointerType;
			bool m_bResult;
		public:
			TTypePtrStaticConvertChecker()
				:m_bResult(false)
			{
				m_bResult = (void*)(static_cast<TToCheckPointerType>((T1PointerType)(0x00000001))) == (void*)(0x00000001);
			}
			operator bool(){return m_bResult;}
		};
	};///TL_APP
};///LOKI_APP

#endif //_INCLUDE_KG3DLOKITYPELISTAPP_H_
