////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGHelperClasses.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-4-10 11:07:51
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "KGStringFunctions.h"
#include "KGMathFunctions.h"
#include <errno.h>
#include "KGTester.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace KG_CUSTOM_HELPERS
{
#define  _KG_STRING_FUCTION_TEST_ENABLE  0
	VOID funcTestValueToString()
	{
#if _KG_STRING_FUCTION_TEST_ENABLE == 1

		TCHAR TempString[1024];
		BOOL bRet = FALSE;
		::setlocale(LC_ALL, "chs");

		{
			INT valueTemp = 0;
			INT arrayValue[] = {0, -1, INT_MAX, INT_MIN};
			LPCTSTR arrayStr[] = {_T("0"), _T("-1"), _T("2147483647"), _T("-2147483648")};
			for (int i =0; i < _countof(arrayValue); i++)
			{
				bRet = TValueToString(arrayValue[i], TempString, _countof(TempString));
				_ASSERTE(bRet);

				bRet = TStringToValue(arrayStr[i], (int)_tcslen(arrayStr[i])+1, valueTemp);
				_ASSERTE(bRet);

				_ASSERTE(valueTemp == arrayValue[i]);
			}
		}
		

		{
			UINT valueTemp = 0;
			UINT arrayValue[] = {0, (UINT)(-1), UINT_MAX, 1234};
			LPCTSTR arrayStr[] = {_T("0"), _T("-1"), _T("4294967295"), _T("1234")};
			for (int i =0; i < _countof(arrayValue); i++)
			{
				bRet = TValueToString(arrayValue[i], TempString, _countof(TempString));
				_ASSERTE(bRet);

				bRet = TStringToValue(arrayStr[i], (int)_tcslen(arrayStr[i])+1, valueTemp);
				_ASSERTE(bRet);

				_ASSERTE(valueTemp == arrayValue[i]);
			}
		}

		{
			FLOAT valueTemp = 0;
			FLOAT arrayValue[] = {0, -1, 0, FLT_MAX, FLT_MIN, 3.2f, -400.f};
			LPCTSTR arrayStr[] = {_T("0.0"), _T("-1.0"), _T("-0.0f"), _T("3.402823466e+38F"), _T("1.175494351e-38F"), _T("3.2"), _T("-400")};
			for (int i =0; i < _countof(arrayValue); i++)
			{
				bRet = TValueToString(arrayValue[i], TempString, _countof(TempString));
				_ASSERTE(bRet);

				bRet = TStringToValue(arrayStr[i], (int)_tcslen(arrayStr[i])+1, valueTemp);
				_ASSERTE(bRet);

				_ASSERTE(valueTemp == arrayValue[i]);
			}
		}

		{
			D3DCOLOR valueTemp = 0;
			D3DCOLOR arrayValue[] = {0x00000000, 0xff000000, 0xff00ff00, 0xffffffff};
			LPCTSTR arrayStr[] = {_T("0,0,0,0"), _T("255,0,0,0"), _T("255,0,255,0"), _T("255,255,255,255")};
			for (int i =0; i < _countof(arrayValue); i++)
			{
				bRet = TValueToString(arrayValue[i], TempString, _countof(TempString));
				_ASSERTE(bRet);

				bRet = TStringToValue(arrayStr[i], (int)_tcslen(arrayStr[i])+1, valueTemp);
				_ASSERTE(bRet);

				_ASSERTE(valueTemp == arrayValue[i]);
			}
		}
		{
			std::vector<INT> valueTemp;
			INT IntTestArray[] = {0, -1, INT_MAX, INT_MIN};
			std::vector<INT> vecTest(IntTestArray, IntTestArray + _countof(IntTestArray) - 1);
			
			bRet = TValueToString(vecTest, TempString, _countof(TempString));
			_ASSERTE(bRet);

			bRet = TStringToValue(TempString, (int)_tcslen(TempString)+1, valueTemp);
			_ASSERTE(bRet);

			_ASSERTE(valueTemp.size() == vecTest.size());
			for (size_t i = 0; i < valueTemp.size(); ++i)
			{
				_ASSERTE(valueTemp[i] == vecTest[i]);
			}
			
		}
		
		{
			D3DXVECTOR3 valueTemp(0,0,0);
			D3DXVECTOR3	arrayValue[] = {
				D3DXVECTOR3(FLT_MAX, FLT_MIN, FLT_MAX),
				D3DXVECTOR3(10, -10, 0),
			};

			LPCTSTR arrayStr[] = {_T("3.402823466e+38F, 1.175494351e-38F, 3.402823466e+38F")
				, _T("10.0f, -10.0f, 0.0f")};

			for (int i =0; i < _countof(arrayValue); i++)
			{
				bRet = TValueToString(arrayValue[i], TempString, _countof(TempString));
				_ASSERTE(bRet);

				bRet = TStringToValue(arrayStr[i], (int)_tcslen(arrayStr[i])+1, valueTemp);
				_ASSERTE(bRet);

				_ASSERTE(valueTemp == arrayValue[i]);
			}

		}
		
//#endif
#endif
	}

	//D3DColor////////////////////////////////////////////////////////////////////////
	BOOL TValueToString( D3DCOLOR Value, LPTSTR tcsBuffer, INT nSize )
	{		
		return D3DCOLOR2LPTSTR(Value, tcsBuffer, nSize);
	}
	BOOL TStringToValue( LPCTSTR tcsBuffer, INT nSize, D3DCOLOR& Value )
	{		
		return LPCTSTR2D3DCOLOR(tcsBuffer, nSize, Value);		
	}
	//INT////////////////////////////////////////////////////////////////////////
	BOOL TValueToString( INT Value, LPTSTR tcsBuffer, INT nSize )
	{
		KG_PROCESS_ERROR(tcsBuffer && nSize >= TValueStringMinBufferSize<INT>::Result);
		return _itot_s(Value, tcsBuffer, nSize, 10) == 0 ? TRUE : FALSE;
Exit0:
		return FALSE;
	}
	BOOL TStringToValue( LPCTSTR tcsBuffer, INT nSize, INT& Value )
	{
		KG_PROCESS_ERROR(tcsBuffer && nSize > 0);
		if (0 == _tcscmp(tcsBuffer, _T("0")))
		{
			Value = 0;
			goto Exit1;
		}
		_set_errno(0);
		Value = _ttoi(tcsBuffer);
		errno_t err = 0;
		_get_errno(&err);
		KG_PROCESS_ERROR(Value != 0 && err != ERANGE);
Exit1:
		return TRUE;
Exit0:
		return FALSE;
	}
	//UINT//////////////////////////////////////////////////////////////////////////
	BOOL TValueToString( UINT Value, LPTSTR tcsBuffer, INT nSize )
	{
		if (!tcsBuffer || nSize < TValueStringMinBufferSize<UINT>::Result)
		{
			return FALSE;
		}
		return _stprintf_s(tcsBuffer, nSize, _T("%u"), Value) >= 0 ? TRUE : FALSE;
	}
	BOOL TStringToValue( LPCTSTR tcsBuffer, INT nSize, UINT& Value )
	{
		KG_PROCESS_ERROR(tcsBuffer && nSize > 0);
		if (0 == _tcscmp(tcsBuffer, _T("0")))
		{
			Value = 0;
			goto Exit1;
		}
		_set_errno(0);
		Value = _tcstoul(tcsBuffer, NULL, 10);
		errno_t err = 0;
		_get_errno(&err);
		KG_PROCESS_ERROR(Value != 0 && err != ERANGE);
Exit1:
		return TRUE;
Exit0:
		return FALSE;
	}
	//UCHAR////////////////////////////////////////////////////////////////////////
	BOOL TValueToString( UCHAR Value, LPTSTR tcsBuffer, INT nSize )
	{
		INT nValue = static_cast<INT>(Value);
		return TValueToString(nValue, tcsBuffer, nSize);
	}
	BOOL TStringToValue( LPCTSTR tcsBuffer, INT nSize, UCHAR& Value )
	{
		INT nValue;
		BOOL bRet = TStringToValue(tcsBuffer, nSize, nValue);
		if (bRet && nValue >= 0 && nValue <= UCHAR_MAX)
		{
			Value = _kg_to_smaller_size_cast<UCHAR>(nValue);
			return TRUE;
		}
		return FALSE;
	}
	//FLOAT////////////////////////////////////////////////////////////////////////
	BOOL TValueToString( FLOAT Value, LPTSTR tcsBuffer, INT nSize )
	{
		if (!tcsBuffer ||nSize < TValueStringMinBufferSize<FLOAT>::Result)
		{
			return FALSE;
		}
		int nRet = _stprintf_s(tcsBuffer, nSize, _T("%e"), Value);
		return nRet >= 0? TRUE : FALSE;
	}	
	BOOL TStringToValue( LPCTSTR tcsBuffer, INT nSize, FLOAT& Value )
	{
		KG_PROCESS_ERROR(tcsBuffer  && nSize > 0);
		
		_set_errno(0);
		Value = static_cast<FLOAT>(_tstof(tcsBuffer));///错误为0
		errno_t err = 0;
		_get_errno(&err);
		KG_PROCESS_ERROR(err == 0);
		return TRUE;
Exit0:
		return FALSE;
	}

	//D3DXVECTOR3////////////////////////////////////////////////////////////////////////
	BOOL TValueToString( D3DXVECTOR3 Value, LPTSTR tcsBuffer, INT nSize )
	{
		if (!tcsBuffer ||nSize < TValueStringMinBufferSize<D3DXVECTOR3>::Result)
		{
			return FALSE;
		}
		int nRet = _stprintf_s(tcsBuffer, nSize, _T("%f,%f,%f"), Value.x, Value.y, Value.z);
		return nRet >= 0? TRUE : FALSE;
	}
	BOOL TStringToValue( LPCTSTR tcsBuffer, INT nSize, D3DXVECTOR3& Value )
	{
		KG_PROCESS_ERROR(tcsBuffer);
		{
			TCHAR pSource[MAX_PATH];
			_tcscpy_s(pSource, tcsBuffer);
			TCHAR sep[] = _T(" ,\t\n");
			TCHAR* pToken = NULL;
			TCHAR* pNextToken = NULL;
			pToken = _tcstok_s(pSource, sep, &pNextToken);
			BOOL bRet = FALSE;
			std::vector<FLOAT>	vecFLOAT;
			vecFLOAT.reserve(3);
			size_t uParsed = 0;
			while(pToken && uParsed < 3)///如果不能翻译出所有的元素，那就是错的
			{
				FLOAT TempElement;
				bRet = TStringToValue(pToken, (int)_tcslen(pToken)+1, TempElement);
				KG_PROCESS_ERROR(bRet);
				vecFLOAT.push_back(TempElement);
				pToken = _tcstok_s(NULL, sep, &pNextToken);
				++uParsed;
			}
			KG_PROCESS_ERROR(vecFLOAT.size() == 3);
			Value.x = vecFLOAT[0];
			Value.y = vecFLOAT[1];
			Value.z = vecFLOAT[2];
		}
		return TRUE;
Exit0:
		return FALSE;
	}
	
	/************************************************************************/
	/*
	下面是显式模板实例化语法，模板定义在头文件中。 显式模板实例化“据说”
	可以减少编译文件大小，但这里主要目的是把这两个函数实现得像普通函数一样
	————一个h文件声明，一个cpp定义。
	*/
	/************************************************************************/
	//VECTOR////////////////////////////////////////////////////////////////////////
	template
		BOOL TValueToString(std::vector<INT>& Value, LPTSTR tcsBuffer, INT nSize);
	template
		BOOL TStringToValue(LPCTSTR tcsBuffer, INT nSize, std::vector<INT>& Value);
	//////////////////////////////////////////////////////////////////////////
	std::tstring GetTrimedString( LPCTSTR tczSource/*<=512*/, TCHAR tcFrom, TCHAR tcTo, LPCTSTR tczTrimSting /*= NULL/*<=5*/ )
	{
		KG_PROCESS_ERROR(tczSource);
		{
			size_t uLength = _tcslen(tczSource);
			size_t uTrimStringLength = 0;
			if (tczTrimSting)
			{
				uTrimStringLength = _tcslen(tczTrimSting);
				KG_PROCESS_ERROR(uTrimStringLength > 0 && uTrimStringLength <= 5);
			}
			KG_PROCESS_ERROR(uLength > 0 && uLength <= 512);

			std::tstring Des(_T(""));
			Des.reserve(uLength+1);
			for(size_t i = 0; i < uLength; i++)
			{
				TCHAR TempChar = tczSource[i];
				if (TempChar >= tcFrom && TempChar <= tcTo)
					continue;
				if (tczTrimSting)
				{
					bool bWithinTrim = false;
					for (size_t x = 0; x < uLength; x++)
					{
						if(TempChar == tczTrimSting[i])
						{
							bWithinTrim = true;
							break; 
						}
					}
					if(bWithinTrim)
						continue;
				}
				Des += TempChar;
			}
			return Des;
		}

Exit0:
		return _T("");
	}
	
	bool StrReplaceExtension( LPCTSTR lpIn, LPCTSTR NewExtension, LPTSTR lpOut, size_t Size )
	{
		if(NULL == lpOut || NULL == NewExtension || NULL == lpIn)
			return false;
		size_t InSize = _tcslen(lpIn);
		size_t NewExtSize = _tcslen(NewExtension);

		if(InSize+1 > Size)
			return false;

		bool bExternsionHasDotHead = false;
		if(NewExtension[0] == '.')
		{
			bExternsionHasDotHead = true;
			NewExtSize -= 1;
		}
		if(NewExtSize > Size)
			return false;

		if (lpIn != lpOut)
		{
			if(_tcscpy_s(lpOut, Size, lpIn) != 0)
				return false;
		}

		TCHAR* pRet = _tcsrchr(lpOut, '.');
		if(pRet == 0)
			return false;
		
		size_t SizeLeft = Size - (pRet - lpOut) - 1;
		if(SizeLeft > Size)//溢出的时候会这样
			return false;

		if(SizeLeft < NewExtSize+1)
			return false;

		errno_t err = _tcscpy_s(pRet+1, SizeLeft, bExternsionHasDotHead ? NewExtension+1 : NewExtension);
		if(err != 0)
			return false;
		return true;	
	}

	BOOL StrPathSplit( LPCTSTR path, StrPathSplited& retStruct, DWORD dwMask /*= em_path_split_all*/)
	{
		_ASSERTE(NULL != path);
		errno_t errCode = _tsplitpath_s(path
			,(dwMask & em_path_split_driver) ? retStruct.driver : NULL
			, (dwMask & em_path_split_driver) ? _countof(retStruct.driver) : 0
			, (dwMask & em_path_split_dir) ? retStruct.dir : NULL
			, (dwMask & em_path_split_dir) ?_countof(retStruct.dir) : 0
			, (dwMask & em_path_split_filename) ? retStruct.fileName : NULL
			, (dwMask & em_path_split_filename) ? _countof(retStruct.fileName) : 0
			, (dwMask & em_path_split_ext) ? retStruct.ext : NULL
			, (dwMask & em_path_split_ext) ? _countof(retStruct.ext) : 0);
		bool bRet = errCode == 0;
		return bRet;
	}

	StrPathSplited::StrPathSplited()
	{
		ZeroMemory(this,sizeof(*this));
	}
	StrPathSplited::StrPathSplited(const StrPathSplited& anotherObj)
	{
		memcpy_s(this, sizeof(*this), &anotherObj, sizeof(*this));
	}

	
	bool IsDirSpliter(TCHAR cTemp)
	{
		return cTemp == _T('/') || cTemp == _T('\\');
	}

	BOOL StrPathSplited::GetCatedPath(LPTSTR pathBuffer, size_t uBufferSize)const
	{
		_ASSERTE(NULL != pathBuffer);

		_ASSERTE(_countof(dir) > _tcslen(dir)
				&&_countof(fileName) > _tcslen(fileName)
				&&_countof(ext) > _tcslen(ext)
				&& _T("字符串结尾不为0")
				);//这样比较不容易出错，_tcslen对于没有初始化的字符串会得到错误的结果，如全是“CD”的字符数组

		
		if (NULL == pathBuffer)
		{
			return FALSE;
		}

		errno_t errCode = _tcscpy_s(pathBuffer, uBufferSize, _T(""));
		_ASSERTE(0 == errCode);

		if (0 != _tcslen(driver))
		{
			errCode = _tcscat_s(pathBuffer, uBufferSize, driver);
			if(errCode != 0)
				return FALSE;
		}

		if (0 != _tcslen(driver))
		{
			if(! IsDirSpliter(dir[0]))
			{
				errCode = _tcscat_s(pathBuffer, uBufferSize, _T("/"));
				if(errCode != 0)
					return FALSE;
			}
		}
		
		if(0 == _tcslen(driver) && IsDirSpliter(dir[0]))//driver没有的话，不能以分隔符开头
		{
			errCode = _tcscat_s(pathBuffer, uBufferSize, dir + 1);
			if(errCode != 0)
				return FALSE;	
		}
		else
		{
			errCode = _tcscat_s(pathBuffer, uBufferSize, dir);
			if(errCode != 0)
				return FALSE;	
		}
		

		errCode = _tcscat_s(pathBuffer, uBufferSize, fileName);
		if(errCode != 0)
			return FALSE;

		errCode = _tcscat_s(pathBuffer, uBufferSize, ext);
		if(errCode != 0)
			return FALSE;
		
		return TRUE;
	}
	StrPathSplited& StrPathSplited::operator=(const StrPathSplited& anotherObj)
	{
		memcpy_s(this, sizeof(*this), &anotherObj, sizeof(*this));
		return *this;
	}

	BOOL StrPathSplited::GetCatedDirectory( LPTSTR pathBuffer, size_t uBufferSize )const
	{
		_ASSERTE(NULL != pathBuffer);

		_ASSERTE(_countof(dir) > _tcslen(dir)
			&& _T("字符串结尾不为0")
			);//这样比较不容易出错，_tcslen对于没有初始化的字符串会得到错误的结果，如全是“CD”的字符数组


		if (NULL == pathBuffer)
		{
			return FALSE;
		}

		errno_t errCode = _tcscpy_s(pathBuffer, uBufferSize, _T(""));
		_ASSERTE(0 == errCode);

		if (0 != _tcslen(driver))
		{
			errCode = _tcscat_s(pathBuffer, uBufferSize, driver);
			if(errCode != 0)
				return FALSE;
		}

		if (0 != _tcslen(driver))
		{
			if(! IsDirSpliter(dir[0]))
			{
				errCode = _tcscat_s(pathBuffer, uBufferSize, _T("/"));
				if(errCode != 0)
					return FALSE;
			}
		}

		if(0 == _tcslen(driver) && IsDirSpliter(dir[0]))//driver没有的话，不能以分隔符开头
		{
			errCode = _tcscat_s(pathBuffer, uBufferSize, dir + 1);
			if(errCode != 0)
				return FALSE;	
		}
		else
		{
			errCode = _tcscat_s(pathBuffer, uBufferSize, dir);
			if(errCode != 0)
				return FALSE;	
		}


		{
			size_t uLen = _tcslen(pathBuffer);
			if (uLen > 0)
			{
				if (IsDirSpliter(pathBuffer[uLen - 1]))
				{
					pathBuffer[uLen - 1] = _T('\0');
				}
			}
		}

		return TRUE;
	}

	BOOL StrReplace( LPTSTR strSrc, TCHAR cCharToBeReplaced, TCHAR cDesChar )
	{
		size_t uLength = _tcslen(strSrc);
		_ASSERTE(uLength < 1000 && _T("字符串太大，不能处理，或者字符串非法"));

		for (size_t i = 0; i < uLength; ++i)
		{
			if(strSrc[i] == cCharToBeReplaced)
				strSrc[i] = cDesChar;
		}

		return TRUE;
	}

	BOOL StrReplace( std::tstring& strSrc, TCHAR cCharToBeReplaced, TCHAR cDesChar )
	{
		size_t uLen = strSrc.size();
		for (size_t i = 0; i < uLen; ++i)
		{
			TCHAR& Ref = strSrc.at(i);
			if(Ref == cCharToBeReplaced)
				Ref = cDesChar;
		}

		return TRUE;
	}

	BOOL StrPathToLowerAndReplaceSlash( std::tstring& pathSrc )
	{
		BOOL bRet = StrReplace(pathSrc, _T('\\'), _T('/'));
		if(! bRet)
			return FALSE;

		std::transform(pathSrc.begin(), pathSrc.end(), pathSrc.begin(), tolower);
		return TRUE;
	}
	BOOL StrPathGetFullFileName(LPCTSTR strFilePathOrFileName, std::tstring& retString)
	{
		_ASSERTE(NULL != strFilePathOrFileName);
		StrPathSplited pathSplited;
		BOOL bRet = StrPathSplit(strFilePathOrFileName, pathSplited, em_path_split_filename | em_path_split_ext);
		if (bRet)
		{
			retString = pathSplited.fileName;
			retString += pathSplited.ext;
			_ASSERTE(NULL != _tcschr(pathSplited.ext, _T('.')) && _T("后缀中应该有点号的"));
		}
		return bRet;
	}
#if defined(KG_ENABLE_TDD)
	KG_TEST_BEGIN(KGStringFunctions)
	{
		//String Functions
		{
#if _KG_STRING_FUCTION_TEST_ENABLE == 1	//这两个测试比较耗时间，平时不要打开，改函数之后打开一次测试就好了
			funcTestValueToString();
#endif
		}
		//StrReplaceExtension
		{
			TCHAR StrSrc[] = _T("D:\\Kinsoft\\Data\\Map\\AMesh.mtl");
			TCHAR StrBuffer[_countof(StrSrc)];

			bool bRet = StrReplaceExtension(StrSrc, _T("bbc"), StrBuffer, _countof(StrBuffer));
			_ASSERTE(bRet);
			_ASSERTE(0 == _tcscmp(StrBuffer, _T("D:\\Kinsoft\\Data\\Map\\AMesh.bbc")));

			bRet = StrReplaceExtension(StrSrc, _T(".abc"), StrBuffer, _countof(StrBuffer));
			_ASSERTE(bRet);
			_ASSERTE(0 == _tcscmp(StrBuffer, _T("D:\\Kinsoft\\Data\\Map\\AMesh.abc")));

			bRet = StrReplaceExtension(NULL, _T("bbc"), StrBuffer, _countof(StrBuffer));
			_ASSERTE(! bRet);

			bRet = StrReplaceExtension(StrSrc, NULL, StrBuffer, _countof(StrBuffer));
			_ASSERTE(! bRet);

			bRet = StrReplaceExtension(StrSrc, _T("bbc"), NULL, _countof(StrBuffer));
			_ASSERTE(! bRet);

			bRet = StrReplaceExtension(StrSrc, _T("bbc"), StrBuffer, _countof(StrBuffer)-1);
			_ASSERTE(! bRet);

			bRet = StrReplaceExtension(StrSrc, _T("bbcc"), StrBuffer, _countof(StrBuffer));
			_ASSERTE(! bRet);
		}
		//StrPathSplit
		{
			//对全路径的处理
			{
				TCHAR pathCase[] = _T("d:/program files/sword3/client/so3client.exe");
				StrPathSplited pathSplited;
				BOOL bRet = StrPathSplit(pathCase, pathSplited);
				_ASSERTE(bRet);

				_ASSERTE(0 == _tcscmp(pathSplited.driver, _T("d:")));
				_ASSERTE(0 == _tcscmp(pathSplited.dir, _T("/program files/sword3/client/")));
				_ASSERTE(0 == _tcscmp(pathSplited.fileName, _T("so3client")));
				_ASSERTE(0 == _tcscmp(pathSplited.ext, _T(".exe")));

				TCHAR strPathBuffer[MAX_PATH];

				/*{
					bRet = pathSplited.GetRelativePath(strPathBuffer, _countof(strPathBuffer));
					_ASSERTE(bRet);
					_ASSERTE(0 == _tcscmp(strPathBuffer, _T("program files/sword3/client/so3client.exe")));
				}*/

				{
					bRet = pathSplited.GetCatedPath(strPathBuffer);
					_ASSERTE(bRet);

					_ASSERTE(0 == _tcscmp(strPathBuffer, pathCase));
				}

				{
					bRet = pathSplited.GetCatedDirectory(strPathBuffer, _countof(strPathBuffer));
					_ASSERTE(bRet);

					_ASSERTE(0 == _tcscmp(strPathBuffer, _T("d:/program files/sword3/client")));
				}

				pathCase[0] = _T('c');
				bRet = StrPathSplit(pathCase, pathSplited, em_path_split_all & (~em_path_split_driver));
				_ASSERTE(bRet);
				_ASSERTE(0 == _tcscmp(pathSplited.driver, _T("d:")));
			}
			//对相对路径的处理
			{
				TCHAR pathCase[] = _T("program files/sword3/client/so3client.exe");
				StrPathSplited pathSplited;
				BOOL bRet = StrPathSplit(pathCase, pathSplited, em_path_split_all_without_driver);

				_ASSERTE(0 == _tcscmp(pathSplited.dir, _T("program files/sword3/client/")));
				_ASSERTE(0 == _tcscmp(pathSplited.fileName, _T("so3client")));
				_ASSERTE(0 == _tcscmp(pathSplited.ext, _T(".exe")));

				TCHAR strPathBuffer[MAX_PATH];

				{
					bRet = pathSplited.GetCatedPath(strPathBuffer, _countof(strPathBuffer));
					_ASSERTE(bRet);
					_ASSERTE(0 == _tcscmp(strPathBuffer, _T("program files/sword3/client/so3client.exe")));
				}
				
				{
					bRet = pathSplited.GetCatedDirectory(strPathBuffer, _countof(strPathBuffer));
					_ASSERTE(bRet);
					_ASSERTE(0 == _tcscmp(strPathBuffer, _T("program files/sword3/client")));
				}
				
			}
		}
		//StrReplace
		{
			TCHAR strSource[] = _T("d:/program files/sword3/client/so3client.exe");
			TCHAR strDes[] = _T("d:\\program files\\sword3\\client\\so3client.exe");
			
			{
				TCHAR strTemp[_countof(strSource) + 1] = _T("");
				memcpy_s(strTemp, sizeof(strTemp), strSource, sizeof(strTemp));
				StrReplace(strTemp, _T('/'), _T('\\'));
				_ASSERTE(0 == _tcscmp(strTemp, strDes));
			}

			{
				std::tstring strTemp(strSource);
				StrReplace(strTemp, _T('/'), _T('\\'));
				_ASSERTE(0 == _tcscmp(strTemp.c_str(), strDes));
			}
		}
		//StrPathToLowerAndReplaceSlash
		{
			TCHAR strSource[] = _T("D:\\Program files\\Sword3\\Client\\So3client.exe");
			TCHAR strDes[] = _T("d:/program files/sword3/client/so3client.exe");

			{
				std::tstring strTemp(strSource);
				StrPathToLowerAndReplaceSlash(strTemp);
				_ASSERTE(0 == _tcscmp(strTemp.c_str(), strDes));
			}
		}
		//StrPathGetFullFileName
		{
			LPCTSTR pathArray[] = {
				_T("D:\\Program files\\Sword3\\Client\\So3client.exe"),
				_T("D:/Program files/Sword3/Client/So3client.exe"),
				_T("../../Sword3/Client/So3client.exe"),
				_T("So3client.exe"),
			};

			for (size_t i = 0; i < _countof(pathArray); ++i)
			{
				std::tstring strRet;
				BOOL bRet = StrPathGetFullFileName(pathArray[i], strRet);
				_ASSERTE(bRet);
				_ASSERTE(0 == _tcscmp(strRet.c_str(), _T("So3client.exe")));
			}
		}
	}
	KG_TEST_END(KGStringFunctions)

	
#endif
};