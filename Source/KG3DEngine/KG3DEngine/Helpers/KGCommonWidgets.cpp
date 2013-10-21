////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGCommonWidgets.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-7-25 9:38:32
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KGCommonWidgets.h"
#include "KGTester.h"
#if defined(KG_ENABLE_TDD)
KG_TEST_BEGIN(KGCommonWidgets)
{
	//Supper sub
	{
		struct AClass{};

		struct BClass{INT m_A;};

		struct CClass : public AClass {};

		struct DClass : public BClass, public AClass {};

		bool bRet = _KG3D_IS_SUPPERSUB_POINTER_EQUAL(AClass, CClass);
		_ASSERTE(bRet);
		bRet = _KG3D_IS_SUPPERSUB_POINTER_EQUAL(AClass, BClass);
		_ASSERTE(! bRet);
		bRet = _KG3D_IS_SUPPERSUB_POINTER_EQUAL(AClass, DClass);
		_ASSERTE(! bRet);
		bRet = _KG3D_IS_SUPPERSUB_POINTER_EQUAL(BClass, DClass);
		_ASSERTE(bRet);
	}
	//KGIsVirtualPointerValid
	{
		struct BClass{};
		BClass b;
		_ASSERTE(! KGIsVirtualPointerValid(&b));

		struct VClass{virtual ~VClass(){}};
		VClass v;
		_ASSERTE(KGIsVirtualPointerValid(&v));

		CHAR Buffer[sizeof(VClass)];
		ZeroMemory(Buffer, sizeof(Buffer));
		VClass* p = (VClass*)Buffer;
		_ASSERTE(! KGIsVirtualPointerValid(p));
	}
	//BOOL2bool
	{
		_ASSERTE(true == BOOL2bool(TRUE));
		_ASSERTE(true == BOOL2bool(2));
		_ASSERTE(false == BOOL2bool(FALSE));
	}
	//PERROR系列
	{
		struct LogObj 
		{
			TCHAR m_str[1024];
			void logFunc(DWORD dwLogLevel, LPCTSTR logString, ...)	
			{
				//下面是不定参数传递语法
				va_list vList;
				va_start(vList, logString);

				_stprintf_s(m_str, logString, vList);

				va_end(vList);
			}
			static bool testPERROR(bool bError)
			{
				PERROR(bError);
				return true;
Exit0:
				return false;
			}
			static bool testPCOMERROR(HRESULT hrIn)
			{
				HRESULT hr = E_NOTIMPL;
				PCOMERROR(hrIn);
				_ASSERTE(hr == hrIn);
				return true;
Exit0:
				_ASSERTE(hr == hrIn);
				return false;
			}
			static bool testPERROR_LOG(bool bError, LogObj& logObj)
			{
				PERROR_BASE(bError, logObj.logFunc(0, _T("bError")));
				return true;
Exit0:
				return false;
			}
			static bool testPCOMERROR_LOG(HRESULT hrIn, LogObj& logObj)
			{
				HRESULT hr = E_NOTIMPL;
				PCOMERROR_BASE(hrIn, logObj.logFunc(0, _T("bError")));
				_ASSERTE(hr == hrIn);
				return true;
Exit0:
				_ASSERTE(hr == hrIn);
				return false;
			}
			static void testMultipleProcess()
			{
				//检查有没有多次执行的情况
				int i = 1;
				HRESULT hr = S_OK;
				PERROR(++i);
				PCOMERROR(++*(ULONG*)&hr);
				PERROR_BASE(++i, NULL);
				PCOMERROR_BASE(++*(ULONG*)&hr, NULL);

				_ASSERTE(3== i);
				_ASSERTE(0x00000002 == (ULONG)hr);
Exit0:
				return;
			}
		};
		
		LogObj logObj;
		logObj.m_str[0] = _T('\0');

		_ASSERTE(LogObj::testPERROR(true) && 0 == _tcslen(logObj.m_str));
		_ASSERTE(! LogObj::testPERROR(false) && 0 == _tcslen(logObj.m_str));

		_ASSERTE(LogObj::testPCOMERROR(S_OK) && 0 == _tcslen(logObj.m_str));

		_ASSERTE(! LogObj::testPCOMERROR(E_FAIL) && 0 == _tcslen(logObj.m_str));

		_ASSERTE(LogObj::testPERROR_LOG(true, logObj) && 0 == _tcslen(logObj.m_str));
		_ASSERTE(! LogObj::testPERROR_LOG(false, logObj) && 0 != _tcslen(logObj.m_str));

		logObj.m_str[0] = _T('\0');

		_ASSERTE(LogObj::testPCOMERROR_LOG(S_OK, logObj) && 0 == _tcslen(logObj.m_str));
		_ASSERTE(! LogObj::testPCOMERROR_LOG(E_FAIL, logObj) && 0 != _tcslen(logObj.m_str));

		LogObj::testMultipleProcess();
	}
}
KG_TEST_END(KGCommonWidgets)
#endif

BOOL Private::KGIsVirtualPointerValidHelper( LPVOID p )
{
	_ASSERTE(NULL != p);
	return _kg_to_bigger_size_cast<LPVOID>(*(INT*)p) != NULL;
}

////////////////////////////////////////////////////////////////////////////////

