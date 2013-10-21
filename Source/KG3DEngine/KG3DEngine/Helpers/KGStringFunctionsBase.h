////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGStringFunctionsBase.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-7-12 14:27:34
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGSTRINGFUNCTIONSBASE_H_
#define _INCLUDE_KGSTRINGFUNCTIONSBASE_H_

#if defined(_DEBUG) | defined(DEBUG)
#define HEADER_NEW_X new(__FILE__, __LINE__)
#else
#define HEADER_NEW_X new
#endif

////////////////////////////////////////////////////////////////////////////////
namespace KG_CUSTOM_HELPERS
{
	namespace	Private
	{
		static const unsigned uCodePage = ::GetACP();
	};
	/************************************************************************/
	///已经做过黑盒测试，可以放心使用，至少是不会当机的。
	/*用于简化Unicode和非Unicode字符的转换，会自动生成Buffer，并在生命周期内自动释放。 
	基本是参考了atlconv.h里面的ATL的A2W和W2A宏的实现。难道这个是我又发明的轮子了？
	写这个的理由是有的老代码只支持char，但咱们引擎以后肯定要支持Unicode的，以前的代码就不管了
	而现在新代码就不如一早就开始写Unicode。
	在引擎中又没有ATL支持，不能用A2W之类的，MultiByteToWideChar什么的没人记得那么多参数，所以
	下面的这个类应该还是有存在理由的

	使用方式:
	WChar* xxx = xxx;
	UnicodeConvertor AConvertor;
	DoSomething(AConvertor.ToA(xxx));

	注意生命周期控制Convertor析构的时候会自动释放内存，而且一个Convertor只能各转换一次Char和WChar

	当作为输入参数用，不会写字符串的时候用ToA和ToW来转换就好了
	如果字符串要用来写的，如：原来要用一个char的buffer要调用函数，现在函数要要转换成UniCode的方式

	///原来函数声明为void GetName(char* pRet, int nBufferSize);
	char ABuffer[MAX_PATH];
	GetName(ABuffer, MAX_PATH);

	///现在函数声明为void GetName(WCHAR* pRet, int nBufferSize);
	///则那么改成这样子:
	char ABuffer[MAX_PATH];
	UnicodeConvertor RetString;
	GetName(RetString.CreateCharBuffer(MAX_PATH), MAX_PATH);
	//这样GetName的结果就写到Convertor里面分配的字符串了，
	//然后用SelfToA/SelfToW重新得到字符串
	RetString.SelfToA(ABuffer);

	070814	加上ToT的版本，不管什么字符串，只要用ToT就能自动兼容Unicode
	*/
	/************************************************************************/
	//下面是依赖的文件,视乎工程，应该在stdafx中加上
	//#include "crtdbg.h"
	//#include <windows.h>

	class UnicodeConvertor
	{
		enum{max_size = 260,/*MAXPATH*/};
	public:
		UnicodeConvertor();
		UnicodeConvertor(UINT CodePage, DWORD dwFlag = 0):m_uWideCharBufferSize(0), m_uCharBufferSize(0), m_uCodePage(CodePage), m_dwFlag(dwFlag){}
		~UnicodeConvertor();

		size_t GetCharBufferSize(){return m_uCharBufferSize;}
		size_t GetWCharBufferSize(){return m_uWideCharBufferSize;}

		void SetCodePage(UINT CodePage){m_uCodePage = CodePage;}
		void SetFlag(DWORD dwFlag){m_dwFlag = dwFlag;}

		CHAR* GetCharBuffer(){return m_CBuffer;}
		WCHAR* GetWCharBuffer(){return m_WBuffer;}
		CHAR* CreateCharBuffer(size_t uSize);//用于读取
		WCHAR* CreateWCharBuffer(size_t uSize);//用于读取
		void ClearCharBuffer(){m_uCharBufferSize = 0;}
		void ClearWCharBuffer(){m_uWideCharBufferSize = 0;}
		size_t	GetCapacity(){return max_size;}
		///注意sizeof(WCHAR)不一定是2
		inline WCHAR* ToW(const CHAR* lpChar, size_t uCharSize, WCHAR* lpWBuffer, size_t uBufferSizeInWChar)
		{
			return this->ToWHelper(lpChar, uCharSize, lpWBuffer, uBufferSizeInWChar, m_uCodePage, m_dwFlag);
		}
		WCHAR* ToW(const CHAR* lpChar, WCHAR* lpWBuffer, size_t uBufferSizeInWChar)
		{
			return this->ToWHelper(lpChar, strlen(lpChar)+1, lpWBuffer, uBufferSizeInWChar, m_uCodePage, m_dwFlag);
		}
		template<size_t _Size>
		WCHAR* ToW(const CHAR* lpChar, WCHAR (&lpWBuffer)[_Size])
		{
			return this->ToWHelper(lpChar, strlen(lpChar)+1, lpWBuffer, _Size, m_uCodePage, m_dwFlag);
		}
		WCHAR* ToW(WCHAR* p)const
		{
			return p;
		}
		const WCHAR* ToW(const WCHAR* p)const
		{
			return p;
		}

		CHAR* ToA(const WCHAR* lpWChar, size_t uWCharSize, CHAR* lpBuffer, size_t uBufferSize)
		{
			return this->ToAHelper(lpWChar, uWCharSize, lpBuffer, uBufferSize, m_uCodePage, m_dwFlag);
		}
		CHAR* ToA(const WCHAR* lpWChar, CHAR* lpBuffer, size_t uBufferSize)
		{
			return this->ToAHelper(lpWChar, wcslen(lpWChar)+1, lpBuffer, uBufferSize, m_uCodePage, m_dwFlag);
		}
		template<size_t _Size>
		CHAR* ToA(const WCHAR* lpWChar, CHAR (&lpBuffer)[_Size])
		{
			return this->ToAHelper(lpWChar, wcslen(lpWChar)+1, lpBuffer, _Size, m_uCodePage, m_dwFlag);
		}
		CHAR* ToA(CHAR* lpChar)const
		{
			return lpChar;
		}
		const CHAR* ToA(const CHAR* lpChar)const
		{
			return lpChar;
		}

		WCHAR* ToW(const CHAR* lpChar);
		CHAR* ToA(const WCHAR* lpWChar);

		const TCHAR* ToT(const char* lpChar);
		const TCHAR* ToT(const WCHAR* lpWChar);

		TCHAR* ToT(const char* lpChar, size_t uCharSize, TCHAR* lpTChar, size_t uTCharSize);;
		TCHAR* ToT(const WCHAR* lpWChar, size_t uWCharSize, TCHAR* lpTChar, size_t uTCharSize);;

		CHAR* SelfToA(){return this->ToA(GetWCharBuffer());}///自身的再次转换
		WCHAR* SelfToW(){return this->ToW(GetCharBuffer());}///自身的再次转换
		CHAR* SelfToA(CHAR* lpBuffer, size_t uBufferSize)
		{
			return this->ToAHelper(GetWCharBuffer(), GetWCharBufferSize(), lpBuffer, uBufferSize);
		}
		template<size_t _Size>
		CHAR* SelfToA(CHAR (&lpBuffer)[_Size])
		{
			return this->ToAHelper(GetWCharBuffer(), GetWCharBufferSize(), lpBuffer, _Size);
		}
		WCHAR* SelfToW(WCHAR* lpBuffer, size_t uBufferSize)
		{
			return this->ToWHelper(GetCharBuffer(), GetCharBufferSize(), lpBuffer, uBufferSize);
		}
		template<size_t _Size>
		WCHAR* SelfToW(WCHAR (&lpBuffer)[_Size])
		{
			return this->ToWHelper(GetCharBuffer(), GetCharBufferSize(), lpBuffer, _Size);
		}
	private:
		//WCHAR* m_lpWideChar;
		//CHAR*	m_lpChar;
		size_t m_uWideCharBufferSize;
		size_t m_uCharBufferSize;
		UINT m_uCodePage;
		DWORD m_dwFlag;

		
		enum{special_code = 0x12345678,};

		WCHAR	m_WBuffer[max_size];
		CHAR	m_CBuffer[max_size*2];//中文字符的strlen就是两个两个算的
	private:
		UnicodeConvertor(const UnicodeConvertor&);
		UnicodeConvertor& operator=(const UnicodeConvertor&);
	private:
		//下面两个是核心函数，用于去除API那两个函数的不定性
		//所有WideCharSize或者Length都以WideChar个数计算而不是sizeof，而所有Buffer大小都必须计算入Null结尾
		WCHAR* ToWHelper(const CHAR* lpChar, size_t uCharSize, WCHAR* lpWBuffer
			, size_t uBufferSizeInWChar, UINT CodePage = special_code
			, DWORD dwFlag = 0 /*MB_PRECOMPOSED也是可以的*/);
		CHAR* ToAHelper(const WCHAR* lpWChar, size_t uWCharSize
			, CHAR* lpABuffer, size_t uBufferSize, UINT CodePage = special_code
			, DWORD dwFlag = 0/*WC_COMPOSITECHECK*/);
	};
};

namespace KGCH = KG_CUSTOM_HELPERS;
#endif //_INCLUDE_KGSTRINGFUNCTIONSBASE_H_
