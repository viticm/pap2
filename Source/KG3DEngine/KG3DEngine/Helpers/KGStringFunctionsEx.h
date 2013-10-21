////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGStringFunctionEx.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-6-22 14:47:26
//  Comment     : 这个是给编辑器用的，CString在引擎中没有定义
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGSTRINGFUNCTIONEX_H_
#define _INCLUDE_KGSTRINGFUNCTIONEX_H_
#include "KGStringFunctions.h"
////////////////////////////////////////////////////////////////////////////////
namespace KG_CUSTOM_HELPERS
{
	template<typename _Type>
	inline BOOL TValueToString(_Type& Value, CString& String)
	{
		enum{buffer_size = TValueStringMinBufferSize<_Type>::Result, };
		BOOL bRet =  TValueToString(Value, String.GetBuffer(buffer_size), buffer_size);
		String.ReleaseBuffer();
		return bRet;
	}
	template<typename _Type>
	inline BOOL TStringToValue(CString& String, _Type& Value)
	{
		return TStringToValue((LPCTSTR)String, String.GetLength(), Value);
	}
};
namespace KGCH = KG_CUSTOM_HELPERS;
#endif //_INCLUDE_KGSTRINGFUNCTIONEX_H_
