////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DLokiStructInterface.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-4-16 19:38:39
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DLOKISTRUCTINTERFACE_H_
#define _INCLUDE_KG3DLOKISTRUCTINTERFACE_H_

#include "./KG3DLokiTypeList.h"
#include "KGStringFunctions.h"

////////////////////////////////////////////////////////////////////////////////
#define MAKE_DESC_ARRAY_LIST_1(value) LOKI_APP_STRINGER(value)
#define MAKE_DESC_ARRAY_LIST_2(v1, v2)	\
	MAKE_DESC_ARRAY_LIST_1(v1),	\
	MAKE_DESC_ARRAY_LIST_1(v2)
#define MAKE_DESC_ARRAY_LIST_3(v1, v2, v3)	\
	MAKE_DESC_ARRAY_LIST_1(v1),	\
	MAKE_DESC_ARRAY_LIST_2(v2, v3)
#define MAKE_DESC_ARRAY_LIST_4(v1, v2, v3, v4)	\
	MAKE_DESC_ARRAY_LIST_1(v1),	\
	MAKE_DESC_ARRAY_LIST_3(v2, v3, v4)
#define MAKE_DESC_ARRAY_LIST_5(v1, v2, v3, v4, v5)	\
	MAKE_DESC_ARRAY_LIST_1(v1),	\
	MAKE_DESC_ARRAY_LIST_4(v2, v3, v4, v5)
#define MAKE_DESC_ARRAY_LIST_6(v1, v2, v3, v4, v5, v6)	\
	MAKE_DESC_ARRAY_LIST_1(v1),	\
	MAKE_DESC_ARRAY_LIST_5(v2, v3, v4, v5, v6)
#define MAKE_DESC_ARRAY_LIST_7(v1, v2, v3, v4, v5, v6, v7)	\
	MAKE_DESC_ARRAY_LIST_1(v1),	\
	MAKE_DESC_ARRAY_LIST_6(v2, v3, v4, v5, v6, v7)
#define MAKE_DESC_ARRAY_LIST_8(v1, v2, v3, v4, v5, v6, v7, v8)	\
	MAKE_DESC_ARRAY_LIST_1(v1),	\
	MAKE_DESC_ARRAY_LIST_7(v2, v3, v4, v5, v6, v7, v8)

#define MAKE_DESC_ARRAY_1(name, value) LPCTSTR name[] = { \
	MAKE_DESC_ARRAY_LIST_1(value) }
#define MAKE_DESC_ARRAY_2(name, v1, v2) LPCTSTR name[] = { \
	MAKE_DESC_ARRAY_LIST_2(v1, v2) }
#define MAKE_DESC_ARRAY_3(name, v1, v2, v3) LPCTSTR name[] = { \
	MAKE_DESC_ARRAY_LIST_3(v1, v2, v3) }
#define MAKE_DESC_ARRAY_4(name, v1, v2, v3, v4) LPCTSTR name[] = { \
	MAKE_DESC_ARRAY_LIST_4(v1, v2, v3, v4) }
#define MAKE_DESC_ARRAY_5(name, v1, v2, v3, v4, v5) LPCTSTR name[] = { \
	MAKE_DESC_ARRAY_LIST_5(v1, v2, v3, v4, v5) }
#define MAKE_DESC_ARRAY_6(name, v1, v2, v3, v4, v5, v6) LPCTSTR name[] = { \
	MAKE_DESC_ARRAY_LIST_6(v1, v2, v3, v4, v5, v6) }
#define MAKE_DESC_ARRAY_7(name, v1, v2, v3, v4, v5, v6, v7) LPCTSTR name[] = { \
	MAKE_DESC_ARRAY_LIST_7(v1, v2, v3, v4, v5, v6, v7) }
#define MAKE_DESC_ARRAY_8(name, v1, v2, v3, v4, v5, v6, v7, v8) LPCTSTR name[] = { \
	MAKE_DESC_ARRAY_LIST_8(v1, v2, v3, v4, v5, v6, v7, v8) }

//////////////////////////////////////////////////////////////////////////
namespace LOKI_APP
{
	namespace StructInterface
	{	
		using namespace TL;

		//////////////////////////////////////////////////////////////////////////
		/*
		struct ElementBase
		{
		virtual INT* Value()const = 0;
		virtual VOID Value(INT* nValue) = 0;
		virtual std::tstring Value2String()const = 0;
		virtual BOOL String2Value(std::tstring& strSource) = 0;
		virtual	LPCTSTR  GetDescription()const = 0;
		virtual INT GetContainedTypeSize()const = 0;
		};*/
		using KG_CUSTOM_HELPERS::TValueStringConvertor;
		//////////////////////////////////////////////////////////////////////////
		template<typename _Type>
		class	TElement;

		template<typename _Type>
		class TElement //: public ElementBase
		{
		public:
			typedef _Type value_type;
		private:
			LPCTSTR	m_tczDescription;
			value_type* m_pValue;
		private:
			TElement(const TElement& Other)
				:m_pValue(NULL)
				//,m_tczDescription(Other.m_tczDescription)
			{
				m_pValue = new value_type;_ASSERTE(m_pValue);
				*m_pValue = *Other.m_pValue;
			}
			void operator=(const TElement& Other);
		public:
			TElement()
				:m_tczDescription(NULL)
				,m_pValue(NULL)
			{

			}
			TElement(LPCTSTR czName)
				:m_pValue(NULL)
				,m_tczDescription(czName)
			{

			}
			/*
			virtual ReleaseResources()
			{

			}*/			
			virtual ~TElement()///虚析构，很重要
			{
				LOKI_STATIC_CHECK((sizeof(INT*)==sizeof(value_type*)), TElement_TypeSizeUnMatch);
				SAFE_DELETE(m_pValue);
			}			
			VOID SetDescription(LPCTSTR tczDes)
			{
				_ASSERTE(tczDes);
				m_tczDescription = tczDes;
			}

			virtual LPCTSTR  GetDescription()const
			{
				return m_tczDescription;
			}			
			virtual VOID GetValue(INT* pValue)const
			{
				_ASSERTE(m_pValue && "必须先设了值才能取\n");
				*(value_type*)pValue = *m_pValue;
			}			
			virtual VOID SetValue(const INT* pValue)
			{
				if (!m_pValue)
				{
					m_pValue = new value_type;
				}
				_ASSERTE(m_pValue);
				*m_pValue = *(const value_type*)(pValue);
			}
			virtual BOOL String2Value(std::tstring& strSource)
			{
				TValueStringConvertor<_Type> tempConvertor;
				if (!m_pValue)
				{
					m_pValue = new value_type;
				}
				_ASSERTE(m_pValue);
				return tempConvertor.String2Value(strSource, *m_pValue);
			}
			virtual std::tstring Value2String()const
			{
				_ASSERTE(m_pValue && "没有向里面灌数据，然后又请求数据是错的,先用Value方法\n");
				TValueStringConvertor<_Type> tempConvertor;
				return tempConvertor.Value2String(*m_pValue);
			}
			virtual INT GetContainedTypeSize()const
			{
				LOKI_STATIC_CHECK((sizeof(value_type) % sizeof(INT) == 0), TElement_GetContainedTypeSize);
				return sizeof(value_type);
			}
		};		
		//////////////////////////////////////////////////////////////////////////
		template<class _ListType>
		class TElementConvertor;

		//////////////////////////////////////////////////////////////////////////
		template<class _Container>
		struct AddElementsToContainer;

		template<typename _TYPE1, typename _TYPE2>
		struct AddElementsToContainer<TTypeList<_TYPE1, _TYPE2> >
		{
			typedef TTypeList<_TYPE1, _TYPE2> typelist_type;
			AddElementsToContainer(TElement<INT>* pElementArray, int nArraySize, LPCTSTR* pDesArray, int nDescArraySize)
			{
				_ASSERTE(pElementArray && pDesArray);
				_ASSERTE(nArraySize == TTypeCount<typelist_type>::result && nArraySize == nDescArraySize);
				LOKI_STATIC_CHECK(sizeof(TElement<INT>) == sizeof(TElement<_TYPE1>), TElement_SizeUnMatch);

				//TElement<_TYPE1> TempElement(*pDesArray);
				//memcpy_s(pElementArray, sizeof(TElement<_TYPE1>), &TempElement, sizeof(TElement<_TYPE1>));
				new(pElementArray) TElement<_TYPE1>(*pDesArray);///placement new
				AddElementsToContainer<_TYPE2>(pElementArray+1, nArraySize-1, pDesArray+1, nDescArraySize-1);
			}
		};
		template<>
		struct AddElementsToContainer<NullType>
		{
			AddElementsToContainer(TElement<INT>* pElementArray, int nArraySize, LPCTSTR* pDesArray, int nDescArraySize)
			{
				_ASSERTE(nArraySize == 0 && nArraySize == nDescArraySize);
			}
		};
		//////////////////////////////////////////////////////////////////////////
		template<class _ListType>
		struct DelElementsFromContainer;

		template<typename _TYPE1, typename _TYPE2>
		struct DelElementsFromContainer<TTypeList<_TYPE1, _TYPE2> >
		{
			typedef TTypeList<_TYPE1, _TYPE2> typelist_type;
			DelElementsFromContainer(TElement<INT>* pElementArray, int nArraySize)
			{
				_ASSERTE(pElementArray);
				_ASSERTE(nArraySize == TTypeCount<typelist_type>::result);
				LOKI_STATIC_CHECK(sizeof(TElement<INT>) == sizeof(TElement<_TYPE1>), TElement_SizeUnMatch);
				TElement<_TYPE1>* pTemp = (TElement<_TYPE1>*)pElementArray;
				pTemp->~TElement<_TYPE1>();///placement new 强制释放语法
				DelElementsFromContainer<_TYPE2>(pElementArray + 1, nArraySize - 1);
			}
		};

		template<>
		struct DelElementsFromContainer<NullType>
		{
			DelElementsFromContainer(TElement<INT>* pElementArray, int nArraySize)
			{
				_ASSERTE(nArraySize == 0);
			}
		};
		//////////////////////////////////////////////////////////////////////////
		template<typename _TYPE1, typename _TYPE2>
		class TElementConvertor<TTypeList<_TYPE1, _TYPE2> >
		{
		public:
			typedef TElement<INT> element_type;
			typedef TTypeList<_TYPE1, _TYPE2> typelist_type;
			enum
			{
				ELEMENT_COUT = TTypeCount<typelist_type>::result,
				ALL_ELEMENT_SIZE = TAllTypeSize<typelist_type>::result,
			};
		private:
			char m_ElementArray[ELEMENT_COUT*sizeof(element_type)];
		public:
			TElementConvertor(LPCTSTR* _DescriptionArray, INT nArraySize)
			{
				memset(m_ElementArray, 0, sizeof(m_ElementArray));
				LOKI_STATIC_CHECK(sizeof(element_type)%sizeof(INT) == 0, TElementConvertor_Element_Size_UnMatch);///确保Placement New使用时候边界对齐
				AddElementsToContainer<typelist_type>((element_type*)m_ElementArray, ELEMENT_COUT, _DescriptionArray, nArraySize);
				_ASSERTE(nArraySize == ELEMENT_COUT);
				_ASSERTE(nArraySize >= 1);
			}
			~TElementConvertor()///哑元实现，仅仅是为了检查
			{
				DelElementsFromContainer<typelist_type>((element_type*)m_ElementArray, ELEMENT_COUT);
			}
			inline INT Size()
			{
				return ELEMENT_COUT;
			}		
		public:
			LPCTSTR ElementDescription(INT nIndex)
			{
				_ASSERTE(nIndex >= 0 && nIndex < ELEMENT_COUT);
				return ((element_type*)m_ElementArray)[nIndex].GetDescription();
			}
			std::tstring Value2String(INT nIndex)
			{
				_ASSERTE(nIndex >= 0 && nIndex < ELEMENT_COUT);
				return ((element_type*)m_ElementArray)[nIndex].Value2String();
			}
			BOOL String2Value(INT nIndex, std::tstring Source)
			{
				_ASSERTE(nIndex >= 0 && nIndex < ELEMENT_COUT);
				return ((element_type*)m_ElementArray)[nIndex].String2Value(Source);
			}
			VOID SetValue(INT nIndex, const INT* pValue)
			{
				_ASSERTE(nIndex >= 0 && nIndex < ELEMENT_COUT);
				((element_type*)m_ElementArray)[nIndex].SetValue(pValue);
			}
			VOID GetValue(INT nIndex, INT* pValue)
			{
				_ASSERTE(nIndex >= 0 && nIndex < ELEMENT_COUT);
				((element_type*)m_ElementArray)[nIndex].GetValue(pValue);
			}
			template<unsigned int nBios, class _StructType>
			VOID SetValue(const _StructType* pStruct, INT nIndex)
			{
				LOKI_STATIC_CHECK(((sizeof(_StructType) - nBios) == ALL_ELEMENT_SIZE), TElementContainer_InputStruct_Size);
				LOKI_STATIC_CHECK((nBios % sizeof(INT)) == 0, TElementContainer_nBiosWrong);
				const INT* pnStruct = reinterpret_cast<const INT*>(pStruct);
				_ASSERTE(pnStruct && nIndex < ELEMENT_COUT);
				pnStruct+=(nBios/sizeof(INT));///有虚表或者不从第一个元素开始的话加一个偏移值
				///跳到元素开始
				int nTempSize = 0 ;
				for (int i = 0; i < nIndex; i++)
				{
					nTempSize += ((element_type*)m_ElementArray)[i].GetContainedTypeSize();
				}
				pnStruct += (nTempSize / sizeof(INT));
				SetValue(nIndex, pnStruct);
			}
			template<unsigned int nBios, class _StructType>
			VOID GetValue(_StructType* pStruct, INT nIndex)
			{
				LOKI_STATIC_CHECK(((sizeof(_StructType) - nBios) == ALL_ELEMENT_SIZE), TElementContainer_InputStruct_Size);
				LOKI_STATIC_CHECK((nBios % sizeof(INT)) == 0, TElementContainer_nBiosWrong);
				INT* pnStruct = reinterpret_cast<INT*>(pStruct);
				_ASSERTE(pnStruct && nIndex < ELEMENT_COUT);
				pnStruct+=(nBios/sizeof(INT));///有虚表或者不从第一个元素开始的话加一个偏移值
				///跳到元素开始
				int nTempSize = 0 ;
				for (int i = 0; i < nIndex; i++)
				{
					nTempSize += ((element_type*)m_ElementArray)[i].GetContainedTypeSize();
				}
				pnStruct += (nTempSize / sizeof(INT));
				GetValue(nIndex, pnStruct);
			}

		};
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
	};
	///StructInterface
};
#endif //_INCLUDE_KG3DLOKISTRUCTINTERFACE_H_
