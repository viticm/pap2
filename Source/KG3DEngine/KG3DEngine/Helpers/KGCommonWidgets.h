////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGCommonWidgets.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-6-22 9:50:02
//  Comment     : 下面这些宏，名字中有DEBUG表示只有DEBUG模式才会有效，如果有
/*	SELF表示项目设置中_KG3D_DEBUG_SELF_ENABLE_OUTPUT=1的时候才会出现，另外可以在
cpp文件中通过设置这个变量来打开SELF宏。
SELF主要的意义在于错误通常都是自己才知道怎么回事的，很多地方不应该写ASSERT，
因为断掉的话别人不知道怎么回事，而很多地方是可以写额外的代码来保证即使出错
也不影响程序运行的。不靠ASSERT的话，程序会更稳定（牺牲很小的错误检查的代价）。

//////////////////////////////////////////////////////////////////////////

_KG3D_DEBUG_STATIC_CHECK_T

可以静态检查条件，如果条件能编译期确定的话。
特别用在模板里面，其它地方也很好用
如_KG3D_DEBUG_STATIC_CHECK_T(sizeof(T) == sizeof(INT), SizeUnMatch)
如果条件失败，会报一个Error_SizeUnMatch变量不存在。

//////////////////////////////////////////////////////////////////////////

_kg_to_smaller_size_cast
_kg_to_bigger_size_cast

用法和static_cast一样
如：
INT x = 255;
UCHAR uC = _kg_to_smaller_size_cast<UCHAR>(x);
主要用于解决大小不匹配的变量强转，原理是低位拷贝。
如果大小不符合条件的话，如强转INT到DWORD_PTR，在编译的时候就报错
而_kg_to_bigger_size_cast用于小到大转换。

//////////////////////////////////////////////////////////////////////////
kg_bytes_of_array(Array)。
用于的到数组大小，如果不是一个数组，会报错，用于静态检查是否数组

//////////////////////////////////////////////////////////////////////////

_KG3D_CAN_CONVERT_TO
用于检查类型是否能够转换
如：
class TypeA
{
	operator int(){return 1;}
};
_KG3D_CAN_CONVER_TO(TypeA, int);	
返回是bool类型

//////////////////////////////////////////////////////////////////////////

_KG3D_SUPERSUBCLASS
用于检查两个类型是否存在父类子类的关系，相同类型也会成功
如：
class TypeA{};
class TypeB : public TypeA{};
_KG3D_SUPERSUBCLASS(TypeA, TypeB)

_KG3D_SUPERSUBCLASS_STRICT
用于检查两个类型是否存在父类子类关系，且不能是相同类型

//////////////////////////////////////////////////////////////////////////

_KG3D_DEBUG_SUPPERSUB_POINTER_EQUAL

用于检查指针向上转换的时候能否Void*强转
如:
struct A{}; class B : public A{};
这样如果在用void*传递指针的函数中，肯定没有问题:
void* GetB(){return m_B;}
A* pA = (A*)GetB();
但是如果多重继承，虚继承，之类的就不能这样。
如struct A{virtual ~A(){}}; struct B{virtual ~B(){}}
class C: public A, public B{}
void* GetB(){return m_C;}
B* pB = (C*)GetB();
这样肯定出问题，因为B的虚表在A后面，正好差一点点
转出来的指针就完全脱离C++机制了，调用的函数的行为不确定
实际使用的情况更复杂一些，但原理大约就这样了。

//////////////////////////////////////////////////////////////////////////

_KG3D_DEBUG_STATIC_ASSERTE

用于运行时对条件仅仅进行一次检查。原理是利用static变量


//////////////////////////////////////////////////////////////////////////
KG_DECLARE_NO_DELETE_OP();
用于接口，禁止delete接口指针

////////////////////////////////////////////////////////////////////////////////
*/
#ifndef _INCLUDE_KGCOMMONWIDGETS_H_
#define _INCLUDE_KGCOMMONWIDGETS_H_
//因为这个文件被多个文件包含，所以这个文件最好不要包含别的文件
///stdafx.h包含，小心耦合
////////////////////////////////////////////////////////////////////////////////

#define  _STRINGER(x) _T(#x)   ///这是一个可以把符号变成字符串的宏
#define  _STRINGERA(x) #x

//windows.h应该在这个文件之前被包含，不然LPCTSTR的定义可能会冲突
#ifndef VOID	//这个标志是Windows定义文件有没有被包含的标志
#if defined(UNICODE) | defined(_UNICODE)
typedef  const wchar_t* LPCTSTR;
#else
typedef  const char* LPCTSTR;
#endif
#endif

//下面的TFILE的做法是MSDN写的，用于让__FILE__宏兼容UNICODE版本
#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(__FILE__)


#ifndef TFILE
#if defined(UNICODE) | defined(_UNICODE)
#define TFILE	__WFILE__
#else
#define TFILE	__FILE__
#endif
#endif

/*下面的inline函数可以解决不能确保sizeof一个数组得到的是否数组大小而不是数组指针大小的问题。
如果是局部数组，明知sizeof结果是正确的情况下，不要用这个东西，用_countof(xx)*sizeof(xx);
这个是用于成员，等等声明离得很远，而且不确定会不会改的时候用的。
写法非常诡异，参考_countof得来的，Template验证项目中有完整的解释
*/
template <typename _CountofType, size_t _SizeOfArray>
char (*kg_bytes_of_array_helper(UNALIGNED _CountofType (&_Array)[_SizeOfArray]))[_SizeOfArray*sizeof(_CountofType)];
#define kg_bytes_of_array(_Array) sizeof(*kg_bytes_of_array_helper(_Array))

//#pragma warning(disable:4505)

#ifdef _UNICODE
#define tstring wstring
#else
#define tstring string
#endif

//////////////////////////////////////////////////////////////////////////
#ifdef HrV	//在DXUT中大量使用了类似这个宏的"V"，在Effect设参数等运算中非常有用
#undef HrV
#endif

#if defined(DEBUG) | defined(_DEBUG)
#define HrV(condition)	{hr = (condition); _ASSERTE(SUCCEEDED(hr));}NULL
#else
#define HrV(condition)	{hr = (condition);}NULL
#endif

#ifdef PERROR
#error PERROR is defined before this file, everything will work unproperly, please try fix this macro conflict
#endif

//下面的这些宏有类型检查，会比KG_PROCESS_ERROR系列更容易查错一些
#define PERROR_LOG_FUNC(conditionString)	KGLogPrintf(KGLOG_DEBUG, _T("LOG ERROR:  condition: %s, line: %d, function: %s"), conditionString, __LINE__, KG_FUNCTION)

#define PERROR_BASE(condition, funcIfError)	{if(!(condition)){(funcIfError); goto Exit0;}}NULL
#define PCOMERROR_BASE(condition, funcIfError)	{hr = (condition); PERROR_BASE(SUCCEEDED(hr), funcIfError);}NULL

#define	PERROR(condition)	PERROR_BASE(condition, NULL)
#define PCOMERROR(condition)	PCOMERROR_BASE(condition, NULL)

#define PERROR_LOG(condition)	PERROR_BASE(condition, PERROR_LOG_FUNC(#condition))

#define PCOMERROR_LOG(condition)	PCOMERROR_BASE(condition, PERROR_LOG_FUNC(_T("(hr)")#condition))	
////////////////////////////////////////////////////////////////////////////////
/************************************************************************/
// 注意下面这些宏，如果有DEBUG，就表示只有DEBUG下才能有效，有SELF的
// 表示有_KG3D_DEBUG_SELF_ENABLE_OUTPUT才有效
// STATIC表示静态检查才能用
/************************************************************************/
#if defined(_UNICODE) | defined(UNICODE)
#define _KG3D_RPTT0(errno, msg) _RPTW0(errno, msg)
#define _KG3D_RPTT1(errno, msg, arg1) _RPTW1(errno, msg, arg1)
#define _KG3D_RPTT2(errno, msg, arg1, arg2) _RPTW2(errno, msg, arg1, arg2)
#define _KG3D_RPTT3(errno, msg, arg1, arg2, arg3) _RPTW3(errno, msg, arg1, arg2, arg3)
#define _KG3D_RPTT4(errno, msg, arg1, arg2, arg3, arg4) _RPTW4(errno, msg, arg1, arg2, arg3, arg4)
#else
#define _KG3D_RPTT0(errno, msg) _RPT0(errno, msg)
#define _KG3D_RPTT1(errno, msg, arg1) _RPT1(errno, msg, arg1)
#define _KG3D_RPTT2(errno, msg, arg1, arg2) _RPT2(errno, msg, arg1, arg2)
#define _KG3D_RPTT3(errno, msg, arg1, arg2, arg3) _RPT3(errno, msg, arg1, arg2, arg3)
#define _KG3D_RPTT4(errno, msg, arg1, arg2, arg3, arg4) _RPT4(errno, msg, arg1, arg2, arg3, arg4)
#endif
//下面这种是用于Debug且必须打开_KG3D_DEBUG_ENABLE_OUTPUT开关才能使用的
#if _KG3D_DEBUG_SELF_ENABLE_OUTPUT == 1
#if defined(DEBUG) | defined(_DEBUG)
#define _KG3D_DEBUG_SELF_OUTPUT0(msg)	_KG3D_RPTT0(_CRT_WARN, msg)
#define _KG3D_DEBUG_SELF_OUTPUT1(msg, arg1)	_KG3D_RPTT1(_CRT_WARN, msg, arg1)
#define _KG3D_DEBUG_SELF_OUTPUT2(msg, arg1, arg2)		_KG3D_RPTT2(_CRT_WARN, msg, arg1, arg2)
#define _KG3D_DEBUG_SELF_OUTPUT3(msg, arg1, arg2, arg3)	_KG3D_RPTT3(_CRT_WARN, msg, arg1, arg2, arg3) 
#define _KG3D_DEBUG_SELF_OUTPUT4(msg, arg1, arg2, arg3, arg4)	 _KG3D_RPTT4(_CRT_WARN, msg, arg1, arg2, arg3, arg4)
#else
#define _KG3D_DEBUG_SELF_OUTPUT0(msg)	NULL
#define _KG3D_DEBUG_SELF_OUTPUT1(msg, arg1)	NULL
#define _KG3D_DEBUG_SELF_OUTPUT2(msg, arg1, arg2)		NULL
#define _KG3D_DEBUG_SELF_OUTPUT3(msg, arg1, arg2, arg3)	NULL
#define _KG3D_DEBUG_SELF_OUTPUT4(msg, arg1, arg2, arg3, arg4)	NULL
#endif
#else
#define _KG3D_DEBUG_SELF_OUTPUT0(msg)	NULL
#define _KG3D_DEBUG_SELF_OUTPUT1(msg, arg1)	NULL
#define _KG3D_DEBUG_SELF_OUTPUT2(msg, arg1, arg2)		NULL
#define _KG3D_DEBUG_SELF_OUTPUT3(msg, arg1, arg2, arg3)	NULL
#define _KG3D_DEBUG_SELF_OUTPUT4(msg, arg1, arg2, arg3, arg4)	NULL
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//下面这种只在Debug下面能使用
#if defined(DEBUG) | defined(_DEBUG)
#define _KG3D_DEBUG_OUTPUT0(msg)	_KG3D_RPTT0(_CRT_WARN, msg)
#define _KG3D_DEBUG_OUTPUT1(msg, arg1)	_KG3D_RPTT1(_CRT_WARN, msg, arg1)
#define _KG3D_DEBUG_OUTPUT2(msg, arg1, arg2)		_KG3D_RPTT2(_CRT_WARN, msg, arg1, arg2)
#define _KG3D_DEBUG_OUTPUT3(msg, arg1, arg2, arg3)	_KG3D_RPTT3(_CRT_WARN, msg, arg1, arg2, arg3) 
#define _KG3D_DEBUG_OUTPUT4(msg, arg1, arg2, arg3, arg4)	 _KG3D_RPTT4(_CRT_WARN, msg, arg1, arg2, arg3, arg4)
#else
#define _KG3D_DEBUG_OUTPUT0(msg)	NULL
#define _KG3D_DEBUG_OUTPUT1(msg, arg1)	NULL
#define _KG3D_DEBUG_OUTPUT2(msg, arg1, arg2)		NULL
#define _KG3D_DEBUG_OUTPUT3(msg, arg1, arg2, arg3)	NULL
#define _KG3D_DEBUG_OUTPUT4(msg, arg1, arg2, arg3, arg4)	 NULL
#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//下面的用于检查式断言,用于程序员自己调试自己的代码，如果关闭了_KG3D_DEBUG_ENABLE_OUTPUT开关，失败的表达式不会触发，
//只会在Output窗口打出来，程序继续运行，如果是在Release下的话，语句失效
#if defined(DEBUG) | defined(_DEBUG)
#if _KG3D_DEBUG_SELF_ENABLE_OUTPUT == 1
#define _KG3D_DEBUG_SELF_ASSERTE(condition)	_ASSERTE(condition)
#else
#define _KG3D_DEBUG_SELF_ASSERTE(condition)	\
	do	\
	{	\
	if (!(condition))	\
		{	\
		::OutputDebugString(_T("以下问题没有处理")_STRINGER(condition)_T("\n"));	\
		}	\
	}while(false)
#endif
#else
#define	_KG3D_DEBUG_SELF_ASSERTE(condition)	NULL 
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(UNICODE) | defined(_UNICODE)
#define _KG3D_DEBUG_CrtDgbReportT	_CrtDbgReportW
#else
#define _KG3D_DEBUG_CrtDgbReportT	_CrtDbgReport
#endif
//下面这种默认在错误的情况下就只会打印出错误，不会断下来
#if defined(DEBUG) | defined(_DEBUG)
#define _KG3D_DEBUG_FAIL_REPORT(condition)	\
	NULL;	\
	{	\
	if (!(condition))	\
		{	\
		::OutputDebugString(_T(":以下条件失败："));	\
		::_CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, NULL, _STRINGERA(condition));	\
		::OutputDebugString(_T("\n"));	\
		}	\
	}NULL;
#else
#define	_KG3D_DEBUG_FAIL_REPORT(condition)	NULL 
#endif
//////////////////////////////////////////////////////////////////////////
//有额外信息的版本
#if defined(DEBUG) | defined(_DEBUG)
#define _KG3D_DEBUG_FAIL_REPORT_MSG(condition, STRING)	\
	NULL;	\
	{	\
	if (!(condition))	\
		{	\
		::OutputDebugString(_T(":以下条件失败："));	\
		::_CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, NULL, _STRINGERA(STRING));	\
		::OutputDebugString(_T("\n"));	\
		}	\
	}NULL;
#else
#define	_KG3D_DEBUG_FAIL_REPORT_MSG(condition, STRING)	NULL 
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(DEBUG) | defined(_DEBUG)
#define _KG3D_DEBUG_REPORT(STRING)	::_CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, NULL,  _STRINGERA(STRING)"\n")
#else
#define	_KG3D_DEBUG_REPORT(STRING)	NULL 
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(DEBUG) | defined(_DEBUG)
#if _KG3D_DEBUG_SELF_ENABLE_OUTPUT == 1
#define _KG3D_DEBUG_SELF_REPORT(STRING)	_KG3D_DEBUG_REPORT(STRING)
#else
#define _KG3D_DEBUG_SELF_REPORT(STRING)	NULL
#endif	
#else
#define	_KG3D_DEBUG_SELF_REPORT(STRING)	NULL 
#endif
//////////////////////////////////////////////////////////////////////////
//下面这个是利用数组不能为零的编译前错误提示来检查condition
#if defined(DEBUG) | defined(_DEBUG)
#define  _KG3D_STATIC_CHECK(condition) \
	do	\
	{	\
	int _kg3d_assert_array[(condition)?1:0];	\
	} while(false)
#else
#define  _KG3D_STATIC_CHECK(condition) NULL
#endif
//////////////////////////////////////////////////////////////////////////
///利用Template的特性生成警告,注意false没有特化
template<int> struct _KG3D_DEBUG_CompileTimeError;
template<> struct _KG3D_DEBUG_CompileTimeError<true> {};

//////////////////////////////////////////////////////////////////////////
#if defined(DEBUG) | defined(_DEBUG)
#define _KG3D_DEBUG_STATIC_CHECK_T(expr, msg) \
{ _KG3D_DEBUG_CompileTimeError<((expr) != 0)> ERROR_##msg; (void)ERROR_##msg; }
#else
#define  _KG3D_DEBUG_STATIC_CHECK_T(expr, msg) NULL
#endif

//////////////////////////////////////////////////////////////////////////
#ifndef KG_DECLARE_NO_DELETE_OP
#if defined(DEBUG) | defined(_DEBUG)
#ifdef __cplusplus
#define KG_DECLARE_NO_DELETE_OP()	\
protected:	\
void operator delete(void* _Ptr) throw(){_ASSERTE(NULL && _T("必须使用Release来释放，又或者明确写出::delete p;"));}	\
void operator delete(void *, void *) throw( ){_ASSERTE(NULL && _T("必须使用Release来释放，又或者明确写出::delete p;"));}	\
void operator delete(void* _Ptr,const std::nothrow_t&) throw(){_ASSERTE(NULL && _T("必须使用Release来释放，又或者明确写出::delete p;"));}	\
void operator delete[](void* _Ptr) throw();	\
void operator delete[](void*, void *) throw();	\
void operator delete[](void* _Ptr, const std::nothrow_t&) throw()
#else
#define KG_DECLARE_NO_DELETE_OP()	void _unusable_void_func()
#endif
#else
#define KG_DECLARE_NO_DELETE_OP()	void _unusable_void_func()
#endif
#endif

//////////////////////////////////////////////////////////////////////////
/************************************************************************/
/*用于解决type cast truncation之类，大小不匹配的warning, 同时可以应付
64bit升级。原理是强制低位拷贝。如果大小不合要求，会在编译的时候就
弹出警告。
使用方法如下：
int nA = 10;
char cAChar = _kg_to_smaller_size_cast<char>(nA);
nA = _kg_to_bigger_size_cast<int>(cAChar);
*/
/************************************************************************/

template< typename _Ret, typename _Src> 
inline _Ret _kg_to_smaller_size_cast(_Src src)
{
	_KG3D_DEBUG_STATIC_CHECK_T(sizeof(_Src) >= sizeof(_Ret), _to_smaller_size_cast);
	return *(_Ret*)(&src);
}
template< typename _Ret, typename _Src>
inline _Ret _kg_to_bigger_size_cast(_Src src)
{
	_KG3D_DEBUG_STATIC_CHECK_T(sizeof(_Src) <= sizeof(_Ret), _to_equal_size_cast_helper_unmatch);
	_Ret aRet = 0;
	*(_Src*)(&aRet) = src;
	return aRet;
}

inline bool BOOL2bool(BOOL bValue)
{
	return !!bValue;
}

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

	//////////////////////////////////////////////////////////////////////////
	template <class T, class U>
	struct Conversion
	{
		typedef Private::ConversionHelper<T, U> H;
		//#ifndef __MWERKS__
		enum { exists = sizeof(typename H::Small) == sizeof(H::Test(H::MakeT())) };
		//#else
		//	enum { exists = false };
		//#endif
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
};

//////////////////////////////////////////////////////////////////////////
#define  _KG3D_CAN_CONVER_TO(From, To) (::Private::Conversion<const From, const To>::exists)
//////////////////////////////////////////////////////////////////////////
#define _KG3D_SUPERSUBCLASS(T, U) \
	(::Private::Conversion<const U*, const T*>::exists && \
	!::Private::Conversion<const T*, const void*>::sameType)
//////////////////////////////////////////////////////////////////////////
#define _KG3D_SUPERSUBCLASS_STRICT(T, U) \
	(_KG3D_SUPERSUBCLASS(T, U) && \
	!::Private::Conversion<const T, const U>::sameType)
//////////////////////////////////////////////////////////////////////////
namespace Private
{	
	template<class _Supper, class _Sub>
	bool ClassPointerStaticUpperCastChecker()
	{
		bool bSupperSubClassRelationShip = _KG3D_SUPERSUBCLASS(_Supper, _Sub);////检查是否超类子类关系
		bool bPointerEqual = (void*)(_Supper*)(0x00000001) == (void*)(_Supper*)(_Sub*)(0x00000001);
		return bSupperSubClassRelationShip && bPointerEqual;
	}

	struct StaticAsserter
	{
		StaticAsserter(bool condition){_ASSERTE(condition);}
	};
};
#if defined(_DEBUG) | defined(DEBUG)
#define _KG3D_DEBUG_STATIC_ASSERTE(condition) { static ::Private::StaticAsserter _SAssert(condition); }NULL
#else 
#define _KG3D_DEBUG_STATIC_ASSERTE(condition) NULL
#endif

#define  _KG3D_IS_SUPPERSUB_POINTER_EQUAL(supper, sub)	::Private::ClassPointerStaticUpperCastChecker<supper, sub>()

#if defined(_DEBUG) | defined(DEBUG)
#define _KG3D_DEBUG_SUPPERSUB_POINTER_EQUAL(supper, sub) _KG3D_DEBUG_STATIC_ASSERTE(_KG3D_IS_SUPPERSUB_POINTER_EQUAL(supper, sub))
#else
#define _KG3D_DEBUG_SUPPERSUB_POINTER_EQUAL(supper, sub) true
#endif


template<class Type>
__forceinline ULONG KGGetRef(Type* p)
{
	_ASSERTE(NULL != p);
	p->AddRef();
	return p->Release();
};

inline ULONG QueryRef(IUnknown* pUnknown)
{ 
	return KGGetRef(pUnknown);
}

template<class Type>
inline VOID KGReleaseUntillDestructed(Type*& p)
{
	if(NULL == p)
		return;
	ULONG uRef = KGGetRef(p);
	for (ULONG i = 0; i < uRef; ++i)
	{
		p->Release();
	}
	p = NULL;
}

namespace Private
{
	BOOL KGIsVirtualPointerValidHelper(LPVOID p);
};
template<class Type>
inline BOOL KGIsVirtualPointerValid(const Type* p)
{
	if(sizeof(Type) < sizeof(LPVOID))
		return FALSE;
	return Private::KGIsVirtualPointerValidHelper((LPVOID)p);
}

//#pragma warning(default:4505)
#endif //_INCLUDE_KGCOMMONWIDGETS_H_
