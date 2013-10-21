////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGTester.h
//  Version     : 2.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-6-12 15:16:07
//  Comment     : 
/*
用法：
把本文件包含在stdafx.h中，前面包含上#include <crtdbg.h>

在CPP中
#if defined(KG_ENABLE_TDD)
KG_TEST_BEGIN(xxxx)
{
	写你的测试。关于成员和函数如何测试，请参考工程中已有的例子。
}
KG_TEST_END(xxxx)
#endif

用法2 ClassTest：1.5版本加入
如有一个类AClass。在类的前面插入
class AClass : public XX, public XX ...
{
	KG_TEST_DECLARE_CLASS_TEST(AClass)
public:
	...
};

在Cpp中
#if defined(KG_ENABLE_TDD)
KG_TEST_CLASS_TEST_BEGIN(AClass)
{
	写你的测试。和KG_TEST_BEGIN不同的是，可以随便Access类的私有成员
}
KG_TEST_CLASS_TEST_END(AClass)
#endif

因为在TDD中会使用桩函数，所以有可能依赖的函数中会抛出各种异常，包括结构化异常。
这可能是初始化的时候，函数环境不正常造成的，如全局的Device没有创建，也可能就是
程序缺陷，这都要检查。这两个宏能够捕捉c++异常和Windows结构异常，如果遇到异常报告
需要对测试代码进行单步以确定异常发生的位置。


不要注掉就算。可以暂时屏蔽部分测试代码，但以后要处理。忽略缺陷只能制造
麻烦。

这些宏全部是经过UNICODE，非UNICODE，DEBUG，RELEASE的组合环境测试的，
能够正确报告文件位置，正确捕捉两种缺陷

之所以要在宏外面包上#if defined是因为要用户明确这些宏是在什么时候起作用的，如果忘记写，
Release版本会编译不过。包装这些宏，让他们自动适应Release暂时认为不是个好主意，因为没有#if包住会
大段大段的和正常代码搅合在一起

已知问题：项目里面
C++/General/Debug Information format不能是Program Database for Edit & Continue (/ZI)
不然__FILE__宏和__LINE__宏不能混用，VS的Bug，参考Google

*/
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGTESTER_H_
#define _INCLUDE_KGTESTER_H_
#include <tchar.h>

//<Config>
#if defined(DEBUG) | defined(_DEBUG)
#ifndef KG_ENABLE_TDD
#define KG_ENABLE_TDD 1				//把define去掉就可以禁止所有的TDD
#endif
#endif

#define KG_TDD_PROGAMER_OR_AUTO	1	//程序员模式还是自动化测试模式

#define KG_TDD_USE_CHINESE	1		//使用中文与否，视乎系统和需求，如果要通用的话，最好就不要用中文，如果是确定的项目，用中文看的人会没有那么累
//</Config>

////////////////////////////////////////////////////////////////////////////////
#ifdef KG_TEST_BEGIN
#pragma message("KG_TEST_BEGIN宏被重复定义，检查头文件包含")
#undef KG_TEST_BEGIN
#endif

//windows.h应该在这个文件之前被包含，不然LPCTSTR的定义可能会冲突
#ifndef VOID	//这个标志是Windows定义文件有没有被包含的标志
#if defined(UNICODE) | defined(_UNICODE)
typedef  const wchar_t* LPCTSTR;
#else
typedef  const char* LPCTSTR;
#endif

#define HRESULT unsigned long
#endif

//下面的TFILE的做法是MSDN写的，用于让__FILE__宏兼容UNICODE版本,为了兼容非MFC程序，所有部分使用通用c++关键字
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

#if	KG_TDD_PROGAMER_OR_AUTO==1
#define KG_TDD_FAILED_IMMEDIATELY	1	//错误立刻当下来，还是谨报告
#else
#define KG_TDD_FAILED_IMMEDIATELY	0	
#endif

#ifndef KG_TDD_CHECK
#if KG_TDD_FAILED_IMMEDIATELY == 1		//如果是错误立刻当下来，那么用_CRT_ERROR触发Assert窗口，否则输出错误
#define KG_TDD_CHECK(condition)	{if(!(condition) && (1==TDD::Report(_CRT_ERROR, _T("Failed: ")_T(#condition)_T(""), TFILE, __LINE__)))_CrtDbgBreak();}NULL
#else
#define KG_TDD_CHECK(condition)	{if(!(condition))TDD::Report(_CRT_WARN, _T("Failed: ")_T(#condition)_T(""), TFILE, __LINE__);}NULL
#endif
#endif

#if defined(DEBUG) | defined(_DEBUG)	//TDD在Release下面是不能用的

namespace TDD
{
	extern int IsTDDWorking();

	typedef void (*TypeTestFunc)(LPVOID);

	extern int Report(int Level, LPCTSTR report, LPCTSTR file, unsigned line);
	extern void RunTest(TypeTestFunc func, LPVOID pExtraParam, LPCTSTR unitName, LPCTSTR file, unsigned line);
	extern int	RunTogetherOrRunAtOnce();	//是注册一起运行还是不注册立刻运行
	extern void RegisterTest(TypeTestFunc func, LPCTSTR unitName, LPCTSTR fileName, unsigned line);
};

#define TDD_MAIN_FUNC_NAME	_T("RunTDD")

#define TDD_DLLAPI extern "C" __declspec(dllexport)

typedef HRESULT (*TypeTDDMainFunc)(void*);

TDD_DLLAPI HRESULT RunTDD(void* reservedParam);	//TDD的主运行函数与主DLL导出函数

#define KG_TEST_MAKE_NAME(name)		KGTest##name
#define KG_TEST_MAKE_INSTANCE_NAME(name) KGTest##name##Instance

//////////////////////////////////////////////////////////////////////////

#define _PRIVATE_KG_TEST_BEGIN(name)	\
struct KG_TEST_MAKE_NAME(name)			\
{										\
	static void TestFunc(LPVOID)		\
	{

#define _PRIVATE_KG_TEST_END(name)		\
	}									\
	KG_TEST_MAKE_NAME(name)()			\
	{									\
		if(TDD::RunTogetherOrRunAtOnce())			\
		{											\
			TDD::RegisterTest(&TestFunc, _T(#name), TFILE, __LINE__);\
		}											\
		else										\
		{											\
			TDD::RunTest(&TestFunc, NULL, _T(#name), TFILE, __LINE__);\
		}											\
	}												\
};									

//////////////////////////////////////////////////////////////////////////

#define KG_TEST_BEGIN(name)	_PRIVATE_KG_TEST_BEGIN(name)
#define KG_TEST_END(name)		\
	_PRIVATE_KG_TEST_END(name)	\
	static KG_TEST_MAKE_NAME(name) KG_TEST_MAKE_INSTANCE_NAME(name);

//////////////////////////////////////////////////////////////////////////
#define  KG_TEST_MAKE_CLASS_TEST_NAME(classname)	KGTestClassTest##classname
#define  KG_TEST_MAKE_CLASS_TEST_INSTANCE_NAME(classname) KGTestClassTest##classname##Instance

#define KG_TEST_DECLARE_CLASS_TEST(classname)	\
	public:										\
		static void TestFunc(LPVOID);					\
	private:

#define KG_TEST_CLASS_TEST_BEGIN(classname)		\
	void classname::TestFunc(LPVOID)			\
	{											\

#define KG_TEST_CLASS_TEST_END(classname)		\
	}											\
	struct KG_TEST_MAKE_CLASS_TEST_NAME(classname)	\
	{												\
		KG_TEST_MAKE_CLASS_TEST_NAME(classname)()	\
		{											\
			if(TDD::RunTogetherOrRunAtOnce())		\
			{										\
				TDD::RegisterTest(&classname::TestFunc, _T(#classname), TFILE, __LINE__);	\
			}														\
			else													\
			{														\
				TDD::RunTest(&classname::TestFunc, NULL, _T(#classname), TFILE, __LINE__);	\
			}														\
		}															\
	};																\
	static KG_TEST_MAKE_CLASS_TEST_NAME(classname) KG_TEST_MAKE_CLASS_TEST_INSTANCE_NAME(classname);
#else
#define  KG_TEST_DECLARE_CLASS_TEST(classname)
#endif
//////////////////////////////////////////////////////////////////////////

//下面的宏用于自定义排除TDD情况下的错误输出
#if defined(KG_ENABLE_TDD)
#define KGLOG_PROCESS_ERROR_3D(condition)	\
{	\
	if(! (condition))	\
{	\
	if(! TDD::IsTDDWorking())	\
	KGLogPrintf(KGLOG_DEBUG,    \
	"Error:(%s) at line %d in %s\n", #condition, __LINE__, KG_FUNCTION);	\
	goto Exit0;	\
}	\
}NULL
#else
#define KGLOG_PROCESS_ERROR_3D(condition)	\
{	\
	if(! (condition))	\
	goto Exit0;	\
}NULL
#endif

#endif //_INCLUDE_KGTESTER_H_
