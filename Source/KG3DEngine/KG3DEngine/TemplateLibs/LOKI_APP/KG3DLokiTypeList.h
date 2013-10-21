////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DLokiTypeList.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-4-16 19:34:13
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DLOKITYPELIST_H_
#define _INCLUDE_KG3DLOKITYPELIST_H_
#include "./KG3DLokiBase.h"
#include <map>
////////////////////////////////////////////////////////////////////////////////

namespace LOKI_APP
{
	namespace TL
	{
		//////////////////////////////////////////////////////////////////////////
		template<typename _TYPE1, typename _TYPE2>
		class TTypeList
		{
		public:
			typedef _TYPE1 inner_T1;
			typedef _TYPE2 inner_T2;
		};
#define TYPELIST_1(T1) ::LOKI_APP::TL::TTypeList<T1, ::LOKI_APP::NullType>
#define TYPELIST_2(T1, T2) ::LOKI_APP::TL::TTypeList<T1, TYPELIST_1(T2) >
#define TYPELIST_3(T1, T2, T3) ::LOKI_APP::TL::TTypeList<T1, TYPELIST_2(T2, T3) >
#define TYPELIST_4(T1, T2, T3, T4) ::LOKI_APP::TL::TTypeList<T1, TYPELIST_3(T2, T3, T4) >
#define TYPELIST_5(T1, T2, T3, T4, T5) ::LOKI_APP::TL::TTypeList<T1, TYPELIST_4(T2, T3, T4, T5) >
#define TYPELIST_6(T1, T2, T3, T4, T5, T6) ::LOKI_APP::TL::TTypeList<T1, TYPELIST_5(T2, T3, T4, T5, T6) >
#define TYPELIST_7(T1, T2, T3, T4, T5, T6, T7) ::LOKI_APP::TL::TTypeList<T1, TYPELIST_6(T2, T3, T4, T5, T6, T7) >
#define TYPELIST_8(T1, T2, T3, T4, T5, T6, T7, T8) ::LOKI_APP::TL::TTypeList<T1, TYPELIST_7(T2, T3, T4, T5, T6, T7, T8) >
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		template <class TList>
		struct TTypeCount;
		template <>
		struct TTypeCount<NullType>
		{
			enum { result = 0 };
		};
		template <class _TYPE1, class _TYPE2>
		struct TTypeCount< TTypeList<_TYPE1, _TYPE2> >
		{
			enum { result = 1 + TTypeCount<_TYPE2>::result };
		};
		//////////////////////////////////////////////////////////////////////////
		template <class _TYPELIST, unsigned int index>
		struct TTypeAt;

		template <class _TYPE1, class _TYPE2>	///注意:这种带const变量的应该先声明,因为这算是index的特化版本,而不是i的特化版本
		struct TTypeAt<TTypeList<_TYPE1, _TYPE2>, 0>
		{
			typedef _TYPE1 Result;
		};

		template <class _TYPE1, class _TYPE2, unsigned int i>
		struct TTypeAt<TTypeList<_TYPE1, _TYPE2>, i>
		{
			typedef typename TTypeAt<_TYPE2, i-1>::Result Result;
		};
		//////////////////////////////////////////////////////////////////////////
		template <class _TypeList, class T> struct TIndexOf;

		template <class T>
		struct TIndexOf<NullType, T>
		{
			enum { result = -1 };
		};

		template <class T, class Tail>
		struct TIndexOf<TTypeList<T, Tail>, T>
		{
			enum { result = 0 };
		};

		template <class Head, class Tail, class T>
		struct TIndexOf<TTypeList<Head, Tail>, T>
		{
		private:
			enum { temp = TIndexOf<Tail, T>::result };
		public:
			enum { result = (temp == -1 ? -1 : 1 + temp) };
		};
		

		
		//////////////////////////////////////////////////////////////////////////
		///记得这个是不算NullType的
		template <class _TYPELIST>
		struct TAllTypeSize;

		template <class _TYPE1, class _TYPE2>
		struct TAllTypeSize<TTypeList<_TYPE1, _TYPE2> >
		{
			enum{ result = sizeof(_TYPE1) + TAllTypeSize<_TYPE2>::result, };
		};

		template <>
		struct TAllTypeSize<NullType>
		{
			enum{ result = 0, };
		};
		//////////////////////////////////////////////////////////////////////////
		///算出来的是第i个(0开始)之前所有的Type总大小
		template<class _TypeList, unsigned int i>
		struct TTypeSizeSum;

		template<class _Type1, class _Type2>
		struct TTypeSizeSum<TTypeList<_Type1, _Type2>, 0> 
		{
			enum{ result = 0, };
		};

		template<class _Type1, class _Type2, unsigned int i>
		struct TTypeSizeSum<TTypeList<_Type1, _Type2>, i> 
		{
			enum{ result = sizeof(_Type1) + TTypeSizeSum<_Type2, i - 1>::result,};
		};
	};///TL
	
	namespace UL
	{
		template<unsigned int _N1, typename _Type2>
		struct  TUintList
		{
			enum
			{
				Number = _N1, 
			};
			typedef _Type2 RestList;
		};
		//////////////////////////////////////////////////////////////////////////
#define UINTLIST_1(N1) ::LOKI_APP::UL::TUintList<N1, ::LOKI_APP::NullType>
#define UINTLIST_2(N1, N2) ::LOKI_APP::UL::TUintList<N1, UINTLIST_1(N2) >
#define UINTLIST_3(N1, N2, N3) ::LOKI_APP::UL::TUintList<N1, UINTLIST_2(N2, N3) >
#define UINTLIST_4(N1, N2, N3, N4) ::LOKI_APP::UL::TUintList<N1, UINTLIST_3(N2, N3, N4) >
#define UINTLIST_5(N1, N2, N3, N4, N5) ::LOKI_APP::UL::TUintList<N1, UINTLIST_4(N2, N3, N4, N5) >
#define UINTLIST_6(N1, N2, N3, N4, N5, N6) ::LOKI_APP::UL::TUintList<N1, UINTLIST_5(N2, N3, N4, N5, N6) >
#define UINTLIST_7(N1, N2, N3, N4, N5, N6, N7) ::LOKI_APP::UL::TUintList<N1, UINTLIST_6(N2, N3, N4, N5, N6, N7) >
#define UINTLIST_8(N1, N2, N3, N4, N5, N6, N7, N8) ::LOKI_APP::UL::TUintList<N1, UINTLIST_7(N2, N3, N4, N5, N6, N7, N8) >
		//////////////////////////////////////////////////////////////////////////
		template<typename _UintList, unsigned int _uIndex>
		struct TNumberAt;

		template<unsigned int _N1, typename _Type2>
		struct TNumberAt<TUintList<_N1, _Type2>, 0 >
		{
			enum{ result = _N1 };
		};

		template<unsigned int _N1, typename _Type2, unsigned int _uIndex>
		struct TNumberAt<TUintList<_N1, _Type2>, _uIndex>
		{
			enum{ result = TNumberAt<_Type2, _uIndex - 1>::result };
		};
		//////////////////////////////////////////////////////////////////////////
		template<typename _UintList, unsigned int _NType>
		struct TIndexOf;

		template<unsigned int _NType>
		struct TIndexOf<NullType, _NType> 
		{
			enum{result = 0x0fffffff,};///如果用到了就一定得到错误的Index
		};
		template<unsigned int _N1, typename _Type2, unsigned int _NType>
		struct TIndexOf<TUintList<_N1, _Type2>, _NType>
		{
			enum{result = _N1 == _NType ? 0 : 1+TIndexOf<_Type2, _NType>::result,};
		};
		//////////////////////////////////////////////////////////////////////////
		template<typename _UintList>
		struct TTypeCount;

		template<unsigned int _N1, typename _Type2>
		struct TTypeCount<TUintList<_N1, _Type2> >
		{
			enum{result = 1+ TTypeCount<_Type2>::result,};
		};

		template<>
		struct TTypeCount<NullType> 
		{
			enum{result = 0,};
		};
		/////////////////////////////////////////////////////////////////////////
		namespace Private
		{
			template<typename _TUintList>
			struct TIndexMapHelper;

			template<>
			struct TIndexMapHelper<NullType>
			{
				typedef map<unsigned int, int> InnerMap;
				TIndexMapHelper(InnerMap& ResourceMap, int nIndex){NULL;}
			};

			template<unsigned int _N1, typename _Type2>
			struct TIndexMapHelper<TUintList<_N1, _Type2> >
			{
				typedef map<unsigned int, int> InnerMap;
				TIndexMapHelper(InnerMap& ResourceMap, int nIndex)
				{
					ResourceMap[_N1] = nIndex;
					_ASSERTE(nIndex >=0);
					TIndexMapHelper<_Type2>(ResourceMap, nIndex + 1);
				}
			};
		};
		
		template<typename _TUintList>
		struct TIndexMap : protected std::map<unsigned int, int> 
		{
			typedef map<unsigned int, int> InnerMap;
			enum{type_count = UL::TTypeCount<_TUintList>::result};
			TIndexMap()
			{
				Private::TIndexMapHelper<_TUintList>(*this, 0);
				_ASSERTE(this->size() == type_count);
			}
			inline int operator[](unsigned int uKey)
			{
				iterator it = this->find(uKey);
				if(it == this->end())
					return -1;
				return it->second;
			}
		};
	};///UL
	namespace ML
	{
		///索引式TypeList
		template<class _TypeList, class _UintList>
		struct TMapTypeList
		{
			typedef _TypeList TypeList;
			typedef _UintList UintList;
		private:
			enum
			{
				type_count = TL::TTypeCount<TypeList>::result,
				uint_count = UL::TTypeCount<UintList>::result,
			};
			::LOKI_APP::CompileTimeError<type_count == uint_count> Error_TypeCountUnmatch;
		};

		template<class _TMapTypeList, unsigned int _NType>
		struct TTypeAt;

		template<class _TypeList, class _UintList, unsigned int _NType>
		struct TTypeAt<TMapTypeList<_TypeList, _UintList>, _NType>
		{
			///从UintList中找到_NType究竟是第几个Type，然后用TL的TTypeAt把TypeList中的Type取出来
			enum
			{
				TYPE_INDEX = UL::TIndexOf<_UintList, _NType>::result,
			};
			typedef typename TL::TTypeAt<_TypeList, TYPE_INDEX>::Result   Result;
		};
	};
};

#endif //_INCLUDE_KG3DLOKITYPELIST_H_
