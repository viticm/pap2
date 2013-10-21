////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DLokiTypeStruct.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-4-28 15:31:21
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DLOKITYPESTRUCT_H_
#define _INCLUDE_KG3DLOKITYPESTRUCT_H_

#include "KG3DLokiBase.h"
#include "KG3DLokiTypeList.h"
#include "KG3DLokiTypeListApp.h"
////////////////////////////////////////////////////////////////////////////////
namespace LOKI_APP
{
	
	namespace TS
	{
		using TL::TTypeList;

		namespace Private
		{
			template<class _TypeList>
			struct TSizeChecker;

			template<typename _Type1, typename _Type2>
			struct TSizeChecker<TTypeList<_Type1, _Type2> >
			{
				enum
				{
					TypeSize = sizeof(_Type1),
					result = ((TypeSize == TSizeChecker<_Type2>::TypeSize) && TSizeChecker<_Type2>::result),
				};
			};

			template<typename _Type1>
			struct  TSizeChecker<TTypeList<_Type1, NullType> >
			{
				enum
				{
					TypeSize = sizeof(_Type1),
					result = true,
				};
			};
			//////////////////////////////////////////////////////////////////////////

		};
		///大小一样的才可以
		template<class _TypeList>
		struct TLStruct;

		template<typename _Type1, typename _Type2>
		struct TLStruct<TTypeList<_Type1, _Type2> >
		{
		protected:
			typedef TTypeList<_Type1, _Type2> TYPE_LIST;
			enum
			{
				ELEMENT_COUNT = TL::TTypeCount<TYPE_LIST>::result,
				ALL_ELEMENT_SIZE = TL::TAllTypeSize<TYPE_LIST>::result,
			};
			char m_pElementArray[ALL_ELEMENT_SIZE];
		public:
			
			TLStruct()
			{
				ZeroMemory(m_pElementArray, sizeof(m_pElementArray));
				*(DWORD*)m_pElementArray = 0x12345678;
			};
			~TLStruct()
			{
				_ASSERTE(*(DWORD*)m_pElementArray == 0x12345678 && "Haven't UnInit After Init.");
			};
			bool Init()
			{
				bool bRet = false;
				try
				{
					*(DWORD*)m_pElementArray = 0;
					TL_APP::AddElementToBuffer<TYPE_LIST >(m_pElementArray, _countof(m_pElementArray)*sizeof(char));
					bRet = true;
					///这个时候m_pElementArray的第一个DWORD肯定被破坏掉了，这样就能表示创建成功了
				}
				catch (...)
				{
					
				}
				return bRet;
			}
			bool UnInit()
			{
				bool bRet = false;
				try
				{
					TL_APP::DelElementFromBuffer<TYPE_LIST >(m_pElementArray, _countof(m_pElementArray)*sizeof(char));
					bRet = true;
					*(DWORD*)m_pElementArray = 0x12345678;
					///如果这里失败了话就......，这个类析构的时候会断下来了。
				}
				catch (...)
				{
					
				}
				return bRet;
			}
			template<unsigned int _nWhichElement>
			inline typename TL::TTypeAt<TYPE_LIST, _nWhichElement>::Result& GetElement()
			{
				typedef typename TL::TTypeAt<TYPE_LIST, _nWhichElement>::Result ReturnType;
				enum
				{ 
					BIOS = TL::TTypeSizeSum<TYPE_LIST, _nWhichElement>::result,
					LAST_ELEMENT_SIZE =sizeof(typename TL::TTypeAt<TYPE_LIST, ELEMENT_COUNT - 1>::Result),
				};
				LOKI_STATIC_CHECK(BIOS >=0 && BIOS <= ALL_ELEMENT_SIZE - LAST_ELEMENT_SIZE, PointerBiosOverFlow );
				_ASSERTE(*(char*)(m_pElementArray+BIOS) && "没有初始化就用了");
				return *(ReturnType*)(m_pElementArray+BIOS);
			}
			inline int GetCount(){return ELEMENT_COUNT;}
			
			template<typename _VirtualType>
			inline _VirtualType& GetVirtualElement(int nIndex)///用这个版本必须所有的Type大小一致,且可以隐式转换才可以
			{
				_ASSERTE(nIndex >= 0 && nIndex < ELEMENT_COUNT);
				LOKI_STATIC_CHECK(Private::TSizeChecker<TYPE_LIST>::result, Size_Unmatch);
				LOKI_STATIC_CHECK(sizeof(char)==1, CharSize_Unmatch);
				enum{IsVirtual = TL_APP::TVirtualTypeChecker<TYPE_LIST, _VirtualType>::result,};
				LOKI_STATIC_CHECK(IsVirtual, IsNotVirtualType);

				///检查一次是否可以静态强转,///只有这种指针强转约定,必须至少动态执行一次.
				static bool bCanStaticCast = TL_APP::TTypePtrStaticConvertChecker<TYPE_LIST, _VirtualType>();
				_ASSERTE(bCanStaticCast);

				int nBios = sizeof(_Type1)*nIndex;
				_ASSERTE(*(char*)(m_pElementArray+nBios) && "没有初始化就用了");
				return *(_VirtualType*)(m_pElementArray+nBios);
			}
		};
		//////////////////////////////////////////////////////////////////////////
	};
	
};

#endif //_INCLUDE_KG3DLOKITYPESTRUCT_H_
