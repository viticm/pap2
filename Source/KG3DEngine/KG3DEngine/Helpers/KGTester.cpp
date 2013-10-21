////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGTester.cpp
//  Version     : 2.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-6-16 11:15:16
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KGTester.h"
#include <crtdbg.h>
#include <assert.h>
#include <vector>
#include <tchar.h>
#include <excpt.h>
#include <iostream>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

//<Config>
#if KG_TDD_PROGAMER_OR_AUTO==1
static int s_bRunTogetherOrRunAtOnce = FALSE;
#else
static int s_bRunTogetherOrRunAtOnce = TRUE;
#endif

static int	s_bIsTDDEnable = TRUE;	//用这个可以简单禁止TDD的运行

enum
{
	em_tdd_to_console,
	em_tdd_to_out,
};

#if KG_TDD_PROGAMER_OR_AUTO==1
static int s_nReportMode = em_tdd_to_out;
#else
static int s_nReportMode = em_tdd_to_console;	//用这个可以定义输出方式
#endif
//</Config>

#ifndef S_OK
#define S_OK ((HRESULT)0x00000000L)
#endif

#ifndef tcout
#if defined(UNICODE) | defined(_UNICODE)
#define tcout wcout
#else
#define tcout cout
#endif
#endif




#if defined(DEBUG) | defined(_DEBUG)
namespace TDD
{
	void BeginTDD()
	{
		struct AClass 
		{
			AClass()
			{
#if KG_TDD_USE_CHINESE			//如果Unitcode下std::cout要允许输出中文。需要下面这两句，不然无法输出中文
#if defined(UNICODE) | defined(_UNICODE)
				std::locale loc(("chs"));
				std::tcout.imbue(loc);
#endif
#endif
				/*	//可以通过更改这些标志来改变Report的方式
				int nMode = _CRTDBG_MODE_DEBUG;
				_HFILE hFile = _CRTDBG_FILE_STDERR;
				switch(s_nReportMode)
				{
				case em_tdd_to_console:
					{
						nMode = _CRTDBG_MODE_FILE;
						hFile = _CRTDBG_FILE_STDERR;
					}
					break;
				default:
				    break;
				}

				_CrtSetReportMode(_CRT_WARN, nMode);
				_CrtSetReportFile(_CRT_WARN, hFile);
				_CrtSetReportMode(_CRT_WARN, nMode);
				_CrtSetReportFile(_CRT_WARN, hFile);*/
				
				TDD::Report(_CRT_WARN, _T("KG TDD Available"), _T(""), 0);
			}
		};
		static AClass a;
		a;
	}

	////////////////////////////////////////////////////////////////////////////////
	
	LPCTSTR LevelToString(int level)
	{
/*
#define _CRT_WARN           0
#define _CRT_ERROR          1
#define _CRT_ASSERT         2
#define _CRT_ERRCNT			3
*/
		static LPCTSTR levelDesc[] = 
		{
			_T("WARN"),
			_T("ERROR"),
			_T("ASSERT"),
			_T("ERRCNT"),
		};
		_ASSERTE(level < _countof(levelDesc));
		return levelDesc[level];
	}

	int Report(int level, LPCTSTR report, LPCTSTR file, unsigned line)	//和_CrtDbgReport一致 -1错误，0无错误，1停下
	{
		if (s_nReportMode == em_tdd_to_console)
		{
			std::tcout << LevelToString(level) << (":") << report << _T(" File:")<< file << _T(" Line:")<< line << _T("\n");
			return -1;
		}
#if defined(UNICODE) | defined(_UNICODE)
		return _CrtDbgReportW(level, file, line, NULL, report);
#else
		return _CrtDbgReport(level, file, line, NULL, report);
#endif
	}

	static int stIsTDDWorking = false;
	int IsTDDWorking()
	{
		return stIsTDDWorking;
	}

	void SetTDDWorking( int bWorking )
	{
		stIsTDDWorking = bWorking;
	}

	class TestManager  
	{
		struct TypeElem 
		{
			TypeTestFunc func;
			LPCTSTR	unitName;
			LPCTSTR	file;
			unsigned	line;
		};
	public:
		void RegisterTest(TypeTestFunc func, LPCTSTR name, LPCTSTR fileName, unsigned line)
		{
			assert(NULL != func);
			TypeElem elem ={func, name, fileName, line};
			m_TestFuncs.push_back(elem);
		}
		void Run()
		{
			size_t Size = m_TestFuncs.size();
			for (size_t i = 0; i < Size; ++i)
			{
				TypeElem& elem = m_TestFuncs[i];
				TDD::RunTest(elem.func, NULL, elem.unitName, elem.file, elem.line);
			}
			TDD::Report(_CRT_WARN, _T("KG TDD All Success！"), _T(""), 0);
		}
	private:
		std::vector<TypeElem> m_TestFuncs;
	};

	TestManager& GetTestManager()
	{
		static TestManager stInstance;
		return stInstance;
	}

	extern int RunTogetherOrRunAtOnce()
	{
		return s_bRunTogetherOrRunAtOnce;
	}

	void ReportException(LPCTSTR exptType, LPCTSTR unitName, LPCTSTR file, unsigned line)
	{
		TCHAR str[256] = _T("");
		_tcscat_s(str, exptType);

#if KG_TDD_USE_CHINESE==1
		TCHAR strPrev[] = _T(" 异常,模块：");
		TCHAR strNext[] = _T("。请检查你的测试模块并单步找出异常位置");
#else
		TCHAR strPrev[] = _T(" Exception, module:");
		TCHAR strNext[] = _T(".Please check your test module, and local the cause by stepping");
#endif

		_tcscat_s(str,strPrev);
		_tcscat_s(str, unitName);
		_tcscat_s(str, strNext);
		str[_countof(str)-1] = _T('\0');
		TDD::Report(_CRT_ERROR, str, file, line); 
	}

	void TestFuncEHC(TypeTestFunc func, LPVOID pExtraParam, LPCTSTR unitName, LPCTSTR file, unsigned line)		 
	{								 
		try							 
		{
			if(s_bIsTDDEnable)
				(*func)(pExtraParam);
		}							 
		catch(...)					 
		{
			ReportException(_T("C++"), unitName, file, line);			
		}							 
	}								 
	void RunTest(TypeTestFunc func, LPVOID pExtraParam, LPCTSTR unitName, LPCTSTR file, unsigned line)			 
	{								 
		__try						 
		{
			TDD::BeginTDD();
			TDD::SetTDDWorking(true);		 
			TestFuncEHC(func, pExtraParam,unitName, file, line);					 
			TDD::SetTDDWorking(false);		 
		}									 
		__except(EXCEPTION_EXECUTE_HANDLER)	 
		{									 
			ReportException(_T("SEH"), unitName, file, line);
		}							 
	}
	void RegisterTest( TypeTestFunc func, LPCTSTR unitName, LPCTSTR fileName, unsigned line)
	{
		TDD::GetTestManager().RegisterTest(func, unitName, fileName, line);
	}
};

TDD_DLLAPI HRESULT RunTDD(void* reservedParam )
{
#if defined(KG_ENABLE_TDD)
#if KG_TDD_PROGAMER_OR_AUTO==0
	TDD::GetTestManager().Run();
#endif
#endif
	return S_OK;
}

#endif