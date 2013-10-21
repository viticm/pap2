////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGStringFunctions.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-6-22 10:50:03
//  Comment     : 
/*
主要是万用字符串和数值的转换
atoi, ftoi之类的函数加上unicode版本，加上错误检查的话，代码就会很复杂且难记：
如_ttoi, wtoi之类，返回值也不统一，有的还没有错误返回的定义。
所以统一做成TValueToString 和 TStringToValue
同时兼容C String, std::string, CString（在KGStringFunctionsEx.h，因为引擎是没有CString定义的）
用法是
INT XX = xx;
std::tstring st;	(tstring在UNICODE下面会自动变成wstring)
BOOL bRet = TValueToString(xx, st);
bRet = TStringToValue(st, xx);
或者
TCHAR czBuffer[100];
BOOL bRet = TValueToString(xx, czBuffer, _countof(czBuffer));
bRet = TStringToValue(czBuffer, _coutof(czBuffer), xx);

CString的用法和std::tstring一致

所有Buffer长度包含NULL结尾
目前只能使用INT ,FLOAT, vector<INT>, UCHAR,D3DCOLOR几种。其它没有定义
使用的时候会报错。
有兴趣的话，各位可以自己补上DOUBLE，只要在重载的那堆函数后面加上就好了
*/
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGSTRINGFUNCTIONS_H_
#define _INCLUDE_KGSTRINGFUNCTIONS_H_

#include "KGCommonWidgets.h"
#include <limits>

///stdafx.h包含，小心耦合
////////////////////////////////////////////////////////////////////////////////
namespace KG_CUSTOM_HELPERS
{
	//全部改成用重载实现
	template<typename _Type>
	struct TValueStringMinBufferSize
	{
		enum{ Result = std::numeric_limits<_Type>::digits10 + 3,};///3包括以下部分，可能有一个负号，可以有点，可能有\0
	};
	template<>
	struct TValueStringMinBufferSize<FLOAT> 
	{
		enum{Result = TValueStringMinBufferSize<DOUBLE>::Result,};///因为FLOAT经常被隐式转换成为Double，所以要这样	
	};
	template<>
	struct TValueStringMinBufferSize<D3DCOLOR> 
	{
		enum{Result = 3 * 4 + 8, };///4个三位的整数，再给用户8个胡乱填的分隔符号	
	};
	template<>
	struct TValueStringMinBufferSize<UCHAR>
	{
		enum{Result = TValueStringMinBufferSize<INT>::Result,};
	};
	template<>
	struct  TValueStringMinBufferSize<D3DXVECTOR3>
	{
		enum{Result = TValueStringMinBufferSize<FLOAT>::Result * 3 + 3};
	};
	template<typename _Type>
	struct TValueStringMinBufferSize<std::vector<_Type> > 
	{
		enum
		{
			MAX_ELEMENT = 50,
			MAX_LENGTH = 1024,
			POSIBLE_LENGTH = (TValueStringMinBufferSize<_Type>::Result + 2)*MAX_ELEMENT,///2是分隔符号
			Result = POSIBLE_LENGTH > MAX_LENGTH? MAX_LENGTH : POSIBLE_LENGTH,
		};
	};
	//////////////////////////////////////////////////////////////////////////
	/************************************************************************/
	/* 万能字符串——类型转换器。有函数TValueToString/TStringToValue和
	类TValueStringConvertor两种模板。视乎在普通函数中使用或者在模板中使用
	选择一种。而函数类型的TValueToString/TStringToValue也重载了LPCTSTR与std::string两种类型
	tstring是自定义宏，兼容UNICODE的std::wstring
	*/
	/************************************************************************/
	//D3DCOLOR////////////////////////////////////////////////////////////////////////
	BOOL TValueToString(D3DCOLOR Value, LPTSTR tcsBuffer, INT nSize);
	BOOL TStringToValue(LPCTSTR tcsBuffer, INT nSize, D3DCOLOR& Value);
	//INT////////////////////////////////////////////////////////////////////////
	BOOL TValueToString(INT Value, LPTSTR tcsBuffer, INT nSize);
	BOOL TStringToValue(LPCTSTR tcsBuffer, INT nSize, INT& Value);
	//UINT//////////////////////////////////////////////////////////////////////////
	BOOL TValueToString(UINT Value, LPTSTR tcsBuffer, INT nSize);
	BOOL TStringToValue(LPCTSTR tcsBuffer, INT nSize, UINT& Value);
	//UCHAR////////////////////////////////////////////////////////////////////////
	BOOL TValueToString(UCHAR Value, LPTSTR tcsBuffer, INT nSize);
	BOOL TStringToValue(LPCTSTR tcsBuffer, INT nSize, UCHAR& Value);
	//FLOAT////////////////////////////////////////////////////////////////////////
	BOOL TValueToString(FLOAT Value, LPTSTR tcsBuffer, INT nSize);
	BOOL TStringToValue(LPCTSTR tcsBuffer, INT nSize, FLOAT& Value);
	//D3DXVECTOR3//////////////////////////////////////////////////////////////////////////
	BOOL TValueToString(D3DXVECTOR3 Value, LPTSTR tcsBuffer, INT nSize);
	BOOL TStringToValue(LPCTSTR tcsBuffer, INT nSize, D3DXVECTOR3& Value);
	//VECTOR////////////////////////////////////////////////////////////////////////
	template<typename _Type>
	static BOOL TValueToString(std::vector<_Type>& Value, LPTSTR tcsBuffer, INT nSize)
	{
		std::tstring tempString;
		std::tstring typeConverString;
		INT nRet = -1;

		///别想用这个东东翻译vector<vector<INT> >之类的东西，虽然不会错，但太复杂了，脱离设计要求
		_KG3D_DEBUG_STATIC_CHECK_T(sizeof(_Type) <= sizeof(INT), ValueStringConvertor_vector__TYPE_Element_Illegal);

		KG_PROCESS_ERROR(tcsBuffer && nSize >= TValueStringMinBufferSize<std::vector<_Type> >::Result);		
		for (std::vector<_Type>::iterator it = Value.begin(); it != Value.end(); it++)
		{
			_Type& AElement = *it;
			if (TValueToString(AElement, typeConverString))
			{
				tempString += typeConverString;
				tempString += _T(",");
			}
			KG_PROCESS_ERROR((int)tempString.length() < nSize);
		}
		KG_PROCESS_ERROR((int)tempString.length() < nSize);
		nRet = _tcscpy_s(tcsBuffer, nSize, tempString.c_str());
		KG_PROCESS_ERROR(nRet == 0);
		return TRUE;
Exit0:
		return FALSE;
	}
	template<typename _Type>
	static BOOL TStringToValue(LPCTSTR tcsBuffer, INT nSize, std::vector<_Type>& Value)
	{
		///别想用这个东东翻译vector<vector<INT> >之类的东西，虽然不会错，但太复杂了，脱离设计要求
		_KG3D_DEBUG_STATIC_CHECK_T(sizeof(_Type) == sizeof(INT), ValueStringConvertor_vector__TYPE_Element_Illegal);

		///复制字符串，创建临时元素数组
		int nSourceSize = nSize;
		int nValueSize = static_cast<int>(Value.size());
		int nRet = -1;
		TCHAR* pSource = NULL;
		std::vector<_Type> TempVec;

		KG_PROCESS_ERROR(nSourceSize < TValueStringMinBufferSize<std::vector<_Type> >::Result);///这个要少于，太长了就放弃翻译了

		{
			int nSourceBufferSize = nSourceSize +1;
			pSource = new TCHAR[nSourceBufferSize];

			KG_PROCESS_ERROR(pSource);
			nRet = _tcscpy_s(pSource, nSourceBufferSize, tcsBuffer);
		}

		///开始标准的strtok分析，每个分析出来的东东压入临时数组
		KG_PROCESS_ERROR(nRet == 0);
		{
			TCHAR sep[] = _T(" ,\t\n");
			TCHAR* pToken = NULL;
			TCHAR* pNextToken = NULL;
			pToken = _tcstok_s(pSource, sep, &pNextToken);
			BOOL bRet = FALSE;
			while(pToken)///如果不能翻译出所有的元素，那就是错的
			{
				std::tstring strTokenTemp(pToken);
				_Type TempElement;
				bRet = TStringToValue(strTokenTemp, TempElement);
				KG_PROCESS_ERROR(bRet);
				TempVec.push_back(TempElement);
				pToken = _tcstok_s(NULL, sep, &pNextToken);
			}	
		}
		if (nValueSize != 0 && (INT)(TempVec.size()) < nValueSize)///已经有元素了,却不能翻译出所有的元素
		{
			goto Exit0;
		}
		///结果压入Value
		Value.clear();
		for (int i= 0; i < (int)TempVec.size(); i++)
		{
			Value.push_back(TempVec[i]);
		}
		SAFE_DELETE_ARRAY(pSource);
		//SAFE_DELETE_ARRAY(pElementsArray);
		return TRUE;
Exit0:
		SAFE_DELETE_ARRAY(pSource);
		//SAFE_DELETE_ARRAY(pElementsArray);
		return FALSE;
	}	
	//模板版本////////////////////////////////////////////////////////////////////////
	template<typename _Type, size_t _Size>
	inline BOOL TValueToString(_Type Value, TCHAR (&String)[_Size])
	{
		return TValueToString(Value, (LPTSTR)String, _Size);
	}
	template<typename _Type, size_t _Size>
	inline BOOL TStringToValue(const TCHAR (&String)[_Size], _Type& Value)
	{
		return TStringToValue((LPCTSTR)String, _Size, Value);
	}
	template<typename _Type, size_t _Size>
	inline BOOL TValueToString(std::vector<_Type>& Value, TCHAR (&String)[_Size])///对于vector类型，参数用引用
	{
		return TValueToString(Value, (LPTSTR)String, _Size);
	}
	template<typename _Type>
	inline BOOL TValueToString(_Type Value, std::tstring& String)
	{
		TCHAR Temp[TValueStringMinBufferSize<_Type>::Result];
		BOOL bRet = TValueToString(Value, Temp, _countof(Temp));
		if (bRet)
			String = Temp;
		return bRet;
	}
	template<typename _Type>
	inline BOOL TStringToValue(const std::tstring& String, _Type& Value)
	{
		return TStringToValue(String.c_str(), (INT)String.length(), Value);
	}
	template<typename _Type>
	inline BOOL TValueToString(std::vector<_Type>& Value, std::tstring& String)///对于vector类型，参数用引用
	{
		TCHAR Temp[TValueStringMinBufferSize<_Type>::Result];
		BOOL bRet = TValueToString(Value, Temp, _countof(Temp));
		if(bRet)
			String = Temp;
		return bRet;
	}
	//////////////////////////////////////////////////////////////////////////
	template<typename _Type> 
	struct TValueStringConvertor
	{
		std::tstring Value2String(_Type Value)
		{
			TCHAR Temp[TValueStringMinBufferSize<_Type>::Result];
#if defined(_DEBUG) | defined(DEBUG)
			BOOL bRet = ::KG_CUSTOM_HELPERS::TValueToString(Value, Temp, _countof(Temp));
			_KG3D_DEBUG_FAIL_REPORT(bRet && _T("TValueStringConvertor::Value2String"));
#else
			::KG_CUSTOM_HELPERS::TValueToString(Value, Temp, _countof(Temp));
#endif			
			return Temp;
		}
		BOOL String2Value(std::tstring& strSource, _Type& Value)
		{
			return ::KG_CUSTOM_HELPERS::TStringToValue(strSource, Value);
		}
	};

	//////////////////////////////////////////////////////////////////////////

	extern VOID funcTestValueToString();	

	//字符串提取,去除制定的范围和TrimString中的字符再返回///////////////////////////////
	std::tstring GetTrimedString( LPCTSTR tczSource/*<=512*/, TCHAR tcFrom, TCHAR tcTo, LPCTSTR tczTrimSting /*= NULL/*<=5*/ );
	bool StrReplaceExtension(LPCTSTR lpIn, LPCTSTR NewExtension, LPTSTR lpOut, size_t Size);//替换后缀
	template<size_t Size>
	bool StrReplaceExtension(LPCTSTR lpIn, LPCTSTR NewExtension, TCHAR (&lpOut)[Size])
	{
		return StrReplaceExtension(lpIn, NewExtension, lpOut, Size);
	}
	
	bool IsDirSpliter(TCHAR cTemp);//是否目录分隔符（/或者\\）

	struct StrPathSplited 
	{
		TCHAR driver[MAX_PATH];
		TCHAR dir[MAX_PATH];
		TCHAR fileName[MAX_PATH];
		TCHAR ext[MAX_PATH];

		StrPathSplited();
		StrPathSplited(const StrPathSplited& anotherObj);
		StrPathSplited& operator=(const StrPathSplited& anotherObj);

		template<size_t uBufferSize>
		BOOL GetCatedPath(TCHAR (&pathBuffer)[uBufferSize])const//如果有driver就包含driver，否则就以路径开始，这样子同时能够处理相对路径
		{
			return GetCatedPath(pathBuffer, uBufferSize);
		}
		BOOL GetCatedPath(LPTSTR pathBuffer, size_t uBufferSize) const;//如果有driver就包含driver，否则就以路径开始，这样子同时能够处理相对路径
		BOOL GetCatedDirectory(LPTSTR dirBuffer, size_t uBufferSize) const;	//即"C:/Program File/dir"这样子，最后面保证没有斜杠，同样有driver就包含driver

	};
	enum
	{
		em_path_split_driver = 0x01,
		em_path_split_dir = 0x02,
		em_path_split_filename = 0x04,
		em_path_split_ext = 0x08,
		em_path_split_all_without_driver = em_path_split_dir | em_path_split_filename | em_path_split_ext,
		em_path_split_all = em_path_split_driver | em_path_split_dir | em_path_split_filename | em_path_split_ext,
	};
	BOOL StrPathSplit(LPCTSTR path, StrPathSplited& retStruct, DWORD dwMask = em_path_split_all);
	BOOL StrReplace(LPTSTR strSrc, TCHAR cCharToBeReplaced, TCHAR cDesChar);
	BOOL StrReplace(std::tstring& strSrc, TCHAR cCharToBeReplaced, TCHAR cDesChar);

	BOOL StrPathToLowerAndReplaceSlash(std::tstring& pathSrc);

	BOOL StrPathGetFullFileName(LPCTSTR strFilePathOrFileName, std::tstring& retString);	//包括文件名，点，和后缀
};
//KG_CUSTOM_HELPERS
namespace KGCH = KG_CUSTOM_HELPERS;
#endif //_INCLUDE_KGSTRINGFUNCTIONS_H_
