////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGStringFunctionsBase.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-9-1 17:24:34
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KGStringFunctionsBase.h"


////////////////////////////////////////////////////////////////////////////////
KG_CUSTOM_HELPERS::UnicodeConvertor::UnicodeConvertor() 
:m_uWideCharBufferSize(0)
, m_uCharBufferSize(0)
, m_uCodePage(special_code)
, m_dwFlag(0)
{

}
KG_CUSTOM_HELPERS::UnicodeConvertor::~UnicodeConvertor()
{
	m_uWideCharBufferSize = 0;
	m_uCharBufferSize = 0;
}
WCHAR* KG_CUSTOM_HELPERS::UnicodeConvertor::ToWHelper( const CHAR* lpChar, size_t uCharSize
													  , WCHAR* lpWBuffer, size_t uBufferSizeInWChar
													  , UINT CodePage /*= special_code*/
													  , DWORD dwFlag /*= 0 /*MB_PRECOMPOSED也是可以的*/ )
{
	_ASSERTE((void*)lpChar != (void*)lpWBuffer);
	_ASSERTE(lpChar && lpWBuffer);

	UINT uCodePageUse = CodePage  == special_code ? Private::uCodePage : CodePage;
	int nReturn = MultiByteToWideChar(uCodePageUse, dwFlag, lpChar, (int)uCharSize, lpWBuffer, (int)uBufferSizeInWChar);

	if (nReturn <= 0/* || nReturn == (int)ERROR_NO_UNICODE_TRANSLATION*/)
	{
#if defined(_DEBUG) | defined(DEBUG)
		DWORD dwErrorCode = ::GetLastError();
		_KG3D_DEBUG_OUTPUT1("ToWHelper Error:%d", dwErrorCode);
#endif
		return NULL;
	}

	return lpWBuffer;
}

CHAR* KG_CUSTOM_HELPERS::UnicodeConvertor::ToAHelper( const WCHAR* lpWChar, size_t uWCharSize
													 , CHAR* lpABuffer, size_t uBufferSize
													 , UINT CodePage /*= special_code*/
													 , DWORD dwFlag /*= 0/*WC_COMPOSITECHECK*/ )
{
	_ASSERTE((void*)lpWChar != (void*)lpABuffer);
	_ASSERTE(lpWChar && lpABuffer);

	UINT uCodePageUse = CodePage  == special_code ? Private::uCodePage : CodePage;

	int nReturn = WideCharToMultiByte(uCodePageUse, dwFlag, lpWChar, (int)uWCharSize, lpABuffer, (int)uBufferSize, NULL, NULL);

	if (nReturn <= 0)
	{
#if defined(_DEBUG) | defined(DEBUG)
		DWORD dwErrorCode = ::GetLastError();
		_KG3D_DEBUG_OUTPUT1("ToAHelper Error:%d", dwErrorCode);
#endif
		return NULL;
	}

	return lpABuffer;
}

CHAR* KG_CUSTOM_HELPERS::UnicodeConvertor::CreateCharBuffer( size_t uSize )
{
	if(uSize > max_size)
		return NULL;
	m_uCharBufferSize  = uSize;
	ZeroMemory(m_CBuffer, sizeof(CHAR)*_countof(m_CBuffer));
	return m_CBuffer;
}

WCHAR* KG_CUSTOM_HELPERS::UnicodeConvertor::CreateWCharBuffer( size_t uSize )
{
	if(uSize > max_size)
		return NULL;
	m_uWideCharBufferSize = uSize;
	ZeroMemory(m_WBuffer, sizeof(WCHAR)*_countof(m_WBuffer));
	return m_WBuffer;
}

WCHAR* KG_CUSTOM_HELPERS::UnicodeConvertor::ToW( const CHAR* lpChar )
{
	if(! lpChar)
		return NULL;

	size_t uSize = strlen(lpChar) + 1;	///注意下面的MultiByteToWideChar的Size是要求计入'\0'的
	if (uSize <= 0 || uSize > max_size)
		return NULL;

	WCHAR* lpWideCharBuffer = this->CreateWCharBuffer(uSize);
	if(! lpWideCharBuffer || lpWideCharBuffer == (WCHAR*)lpChar)
		return NULL;
	_ASSERTE(m_uWideCharBufferSize == uSize);

	_ASSERTE(m_uWideCharBufferSize > 0);
	if(! this->ToWHelper(lpChar, uSize, lpWideCharBuffer, m_uWideCharBufferSize, m_uCodePage, m_dwFlag)) 
	{
		ClearWCharBuffer();
		return NULL;
	}
	return lpWideCharBuffer;
}

CHAR* KG_CUSTOM_HELPERS::UnicodeConvertor::ToA( const WCHAR* lpWChar )
{
	if(! lpWChar)
		return NULL;

	size_t uSize = wcslen(lpWChar)+1;		///注意下面的WideCharToMultiByte的Size是要求计入'\0'的
	if (uSize <= 0 || uSize > max_size)
		return NULL;

	CHAR* lpCharBuffer = CreateCharBuffer(uSize*2);
	if(! lpCharBuffer)
		return NULL;
	_ASSERTE(m_uCharBufferSize ==  uSize*2);

	_ASSERTE(m_uCharBufferSize > 0);
	if(! this->ToAHelper(lpWChar, uSize, lpCharBuffer, m_uCharBufferSize, m_uCodePage, m_dwFlag))
	{
		ClearCharBuffer();
		return NULL;
	}
	return lpCharBuffer;
}

TCHAR* KG_CUSTOM_HELPERS::UnicodeConvertor::ToT( const WCHAR* lpWChar, size_t uWCharSize, TCHAR* lpTChar, size_t uTCharSize )
{
#if defined(_UNICODE) | defined(UNICODE)
	if(! lpWChar || ! lpTChar)
		return NULL;
	if (0 == wcscpy_s(lpTChar, uTCharSize, lpWChar))
	{
		return lpTChar;
	}
	return NULL;
#else
	return this->ToAHelper(lpWChar, uWCharSize, lpTChar, uTCharSize);
#endif
}

TCHAR* KG_CUSTOM_HELPERS::UnicodeConvertor::ToT( const char* lpChar, size_t uCharSize, TCHAR* lpTChar, size_t uTCharSize )
{
#if defined(_UNICODE) | defined(UNICODE)
	return this->ToWHelper(lpChar, uCharSize, lpTChar, uTCharSize);
#else
	if(! lpChar || !lpTChar)
		return NULL;
	if (0 == strcpy_s(lpTChar, uTCharSize, lpChar))
	{
		return lpTChar;
	}
	return NULL;
#endif
}

const TCHAR* KG_CUSTOM_HELPERS::UnicodeConvertor::ToT( const WCHAR* lpWChar )
{
#if defined(_UNICODE) | defined(UNICODE)
	return lpWChar;
#else
	return this->ToA(lpWChar);
#endif
}

const TCHAR* KG_CUSTOM_HELPERS::UnicodeConvertor::ToT( const char* lpChar )
{
#if defined(_UNICODE) | defined(UNICODE)	
	return this->ToW(lpChar);
#else
	return lpChar;
#endif
}