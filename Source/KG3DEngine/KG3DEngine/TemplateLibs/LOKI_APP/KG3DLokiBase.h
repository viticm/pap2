////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DLokiBase.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-4-16 19:35:12
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DLOKIBASE_H_
#define _INCLUDE_KG3DLOKIBASE_H_

////////////////////////////////////////////////////////////////////////////////
/************************************************************************/
/*LOKI库是一个验证C++前沿技术的库————很好用，但是简直就是个魔方一样的东西
这里照抄了TypeList，然后做出来的Convertor是一个利用虚表指针强制覆盖的办法，实现了
一个把任意Struct转换为一堆字符串的模板。
结构不是很好，但因为析构函数会利用TypeList重新还原元素类型，
而基本检查也证明是没有资源泄漏的，在动态天气的关键帧编辑中暂时就这样用着了。
*/
/************************************************************************/
namespace LOKI_APP
{
	template<int> struct CompileTimeError;
	template<> struct CompileTimeError<true> {};
	//////////////////////////////////////////////////////////////////////////
#define LOKI_STATIC_CHECK(expr, msg) \
	{ LOKI_APP::CompileTimeError<((expr) != 0)> ERROR_##msg; (void)ERROR_##msg; }

#define LOKI_APP_STRINGER(name) (_T(#name))
	//////////////////////////////////////////////////////////////////////////
	namespace Private
	{
		///能否从T转换成U,(前者到后者)
		template <class T, class U>
		struct ConversionHelper
		{
			typedef char Small;
			struct Big { char dummy[2]; };
			static Big   Test(...);
			static Small Test(U);
			static T MakeT();
		};
	};
	template <class T, class U>
	struct Conversion
	{
		typedef Private::ConversionHelper<T, U> H;
#ifndef __MWERKS__
		enum { exists = sizeof(typename H::Small) == sizeof(H::Test(H::MakeT())) };
#else
		enum { exists = false };
#endif
		enum { exists2Way = exists && Conversion<U, T>::exists };
		enum { sameType = false };
	};

	template <class T>
	struct Conversion<T, T>    
	{
		enum { exists = 1, exists2Way = 1,sameType = 1 };
	};

	template <class T>
	struct Conversion<void, T>    
	{
		enum { exists = 1, exists2Way = 0,sameType = 0 };
	};

	template <class T>
	struct Conversion<T, void>    
	{
		enum { exists = 1, exists2Way = 0,sameType = 0 };
	};

	template <>
	class Conversion<void, void>    
	{
	public:
		enum { exists = 1, exists2Way = 1,sameType = 1 };
	};
	//////////////////////////////////////////////////////////////////////////
#define  CAN_CONVER_TO(From, To) (::LOKI_APP::Conversion<const From, const To>::exists)
	//////////////////////////////////////////////////////////////////////////
#define SUPERSUBCLASS(T, U) \
	(::LOKI_APP::Conversion<const U*, const T*>::exists && \
	!::LOKI_APP::Conversion<const T*, const void*>::sameType)
	//////////////////////////////////////////////////////////////////////////
#define SUPERSUBCLASS_STRICT(T, U) \
	(SUPERSUBCLASS(T, U) && \
	!::LOKI_APP::Conversion<const T, const U>::sameType)
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	class NullType
	{

	};
	//////////////////////////////////////////////////////////////////////////
	template <int v>
	struct TInt2Type
	{
		enum { value = v };
	};
	//////////////////////////////////////////////////////////////////////////
	template <typename T>
	struct TType2Type
	{
		typedef T OriginalType;
	};	
	////////////////////////////////////////////////////////////////////////////////
	template <bool flag, typename T, typename U>
	struct TSelect
	{
		typedef T Result;
	};
	template <typename T, typename U>
	struct TSelect<false, T, U>
	{
		typedef U Result;
	};
};

#endif //_INCLUDE_KG3DLOKIBASE_H_
