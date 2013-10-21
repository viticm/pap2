////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGINIFileHelpers.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-6-22 15:11:31
//  Comment     : Base中的IIniFile是非Unicode的，下面的函数利用模板显式要求参数
/*
不要用Private中的函数，那些函数是用函数重载特性服务于上层的。而且Private中的是非Unicode的
要用的话，这样：
SaveINIFileData<DWORD>(pIniFile, szSection, szKeyName, DataRet);
LoadINIFileData<DWORD>(pIniFile, szSection, szKeyName, DefaultData, &DataRet);

每次都显式指定模板参数，避免错误的匹配（如果用函数重载的话，编译器会自做主张给你分配一个函数）
支持Unicode，在上层使用了UnicodeConvertor来进行自动转换
*/
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGINIFILEHELPER_H_
#define _INCLUDE_KGINIFILEHELPER_H_
#include "KGStringFunctionsBase.h"
////////////////////////////////////////////////////////////////////////////////
class IIniFile;
namespace KG_CUSTOM_HELPERS
{
	namespace Private
	{
		bool SaveINIFileHelper(IIniFile* pIniFile, const char* szSection, const char* szKeyName, D3DCOLOR DataRet);
		bool LoadINIFileHelper(IIniFile* pIniFile, const char* szSection, const char* szKeyName, D3DCOLOR DefaultData, D3DCOLOR* DataRet);
		bool SaveINIFileHelper(IIniFile* pIniFile, const char* szSection, const char* szKeyName, INT DataRet);
		bool LoadINIFileHelper(IIniFile* pIniFile, const char* szSection, const char* szKeyName, INT DefaultData, INT* DataRet);
		bool SaveINIFileHelper(IIniFile* pIniFile, const char* szSection, const char* szKeyName, bool DataRet);
		bool LoadINIFileHelper(IIniFile* pIniFile, const char* szSection, const char* szKeyName, bool DefaultData, bool* DataRet);
		bool SaveINIFileHelper(IIniFile* pIniFile, const char* szSection, const char* szKeyName, FLOAT DataRet);
		bool LoadINIFileHelper(IIniFile* pIniFile, const char* szSection, const char* szKeyName, FLOAT DefaultData, FLOAT* DataRet);
		bool SaveINIFileHelper(IIniFile* pIniFile, const char* szSection, const char* szKeyName, UINT DataRet);
		bool LoadINIFileHelper(IIniFile* pIniFile, const char* szSection, const char* szKeyName, UINT DefaultData, UINT* DataRet);
		//bool SaveINIFileHelper(IIniFile* pIniFile, const char* szSection, const char* szKeyName, DWORD_PTR DataRet);
		//bool LoadINIFileHelper(IIniFile* pIniFile, const char* szSection, const char* szKeyName, DWORD_PTR DefaultData, DWORD_PTR* DataRet);
		bool SaveINIFileHelper(IIniFile* pIniFile, const char* szSection, const char* szKeyName, LPCSTR lpStr);
		bool LoadINIFileHelper(IIniFile* pIniFile, const char* szSection, const char* szKeyName, LPCSTR DefaultData, LPSTR lpStr, unsigned int uBufferSize);
	};
	//模板的形式，用于Unicode包装，把Unicode传递到底层的非Unicode代码中，同时得到模板类型安全
	template<class _SaveType>
	inline bool SaveINIFileData(IIniFile* pIniFile, LPCTSTR szSection, LPCTSTR szKeyName, const _SaveType DataRet)
	{
#if defined(_UNICODE) | defined(UNICODE)
		UnicodeConvertor CvtSection, CvtKeyName, CvtDataRet;
		return Private::SaveINIFileHelper(pIniFile, CvtSection.ToA(szSection), CvtKeyName.ToA(szKeyName), DataRet);
#else
		return Private::SaveINIFileHelper(pIniFile, szSection, szKeyName, DataRet);
#endif	
	}
	template<class _LoadType>
	inline bool LoadINIFileData(IIniFile* pIniFile, LPCTSTR szSection, LPCTSTR szKeyName, const _LoadType DefaultData, _LoadType* DataRet)
	{
#if defined(_UNICODE) | defined(UNICODE)
		UnicodeConvertor CvtSection, CvtKeyName;
		return Private::LoadINIFileHelper(pIniFile, CvtSection.ToA(szSection), CvtKeyName.ToA(szKeyName), DefaultData, DataRet);
#else
		return Private::LoadINIFileHelper(pIniFile, szSection, szKeyName, DefaultData, DataRet);
#endif
		
	}
	//<String Load Save>
	///因为String的多了一个参数，所以就不能用上面的方式
	inline bool SaveINIFileData(IIniFile* pIniFile, LPCTSTR tszSection, LPCTSTR tszKeyName, LPCTSTR lpStr)
	{
#if defined(_UNICODE) | defined(UNICODE)
		UnicodeConvertor CvtSection, CvtKeyName, CvtString;
		return Private::SaveINIFileHelper(pIniFile, CvtSection.ToA(tszSection), CvtKeyName.ToA(tszKeyName), CvtString.ToA(lpStr))
#else
		return Private::SaveINIFileHelper(pIniFile, tszSection, tszKeyName, lpStr);
#endif
	}
	inline bool LoadINIFileData(IIniFile* pIniFile, LPCTSTR tszSection, LPCTSTR tszKeyName, LPCTSTR DefaultData, LPTSTR lpStr, unsigned int uBufferSize)//Default可以为NULL
	{
#if defined(_UNICODE) | defined(UNICODE)
		UnicodeConvertor CvtSection, CvtKeyName, CvtDefault, CvtString;
		if(Private::LoadINIFileHelper(pIniFile, CvtSection.ToA(tszSection), CvtKeyName.ToA(tszKeyName), CvtDefault.ToA(DefaultData), CvtString.CreateCharBuffer(MAX_PATH), MAX_PATH))
		{
			if(CvtDefault.SelfToW(lpStr), uBufferSize)
				return true;
		}
		return false;
#else
		return Private::LoadINIFileHelper(pIniFile, tszSection, tszKeyName, DefaultData, lpStr, uBufferSize);
#endif
	}
	template<size_t _Size>
	inline bool LoadINIFileData(IIniFile* pIniFile, LPCTSTR tszSection, LPCTSTR tszKeyname, LPCTSTR DefaultData, TCHAR (&lpStr)[_Size])//Default可以为NULL
	{
		return LoadINIFileData(pIniFile, tszSection, tszKeyname, DefaultData, lpStr, _Size);
	}
	inline bool LoadINIFileData(IIniFile* pIniFile, LPCTSTR tszSection, LPCTSTR tszKeyname, LPCTSTR DefaultData, std::tstring& strRet)//Default可以为NULL
	{
		TCHAR tszRet[MAX_PATH];
		bool bRet = LoadINIFileData(pIniFile, tszSection, tszKeyname, DefaultData, tszRet);
		if (bRet)
			strRet = tszRet;
		return bRet;
	}
	//</String Load Save>
	//////////////////////////////////////////////////////////////////////////
	inline int IsSectionExist(IIniFile* pIniFile, LPCTSTR lpSection)
	{
		_ASSERTE(pIniFile);
#if defined(_UNICODE) | defined(UNICODE)
		UnicodeConvertor CvtSection;
		return pIniFile->IsSectionExist(CvtSection.ToA(lpSection));
#else
		return pIniFile->IsSectionExist(lpSection);
#endif
	}
	template<size_t _Size>
	inline int GetNextKey(IIniFile* pIniFile, LPCTSTR tszSection, LPCTSTR tszKeyName, TCHAR (&tszRetNextKey)[_Size])
	{
		_ASSERTE(pIniFile);
#if defined(_UNICODE) | defined(UNICODE)
		UnicodeConvertor CvtSection, CvtKeyName, CvtRetNextKey;
		int nRet =  pIniFile->IsSectionExist(CvtSection.ToA(tszSection), CvtKeyName.ToA(tszKeyName), CvtRetNextKey.CreateCharBuffer(_Size));
		if (CvtRetNextKey.SelfToW(tszRetNextKey))
			return nRet;
		return FALSE;
#else
		return pIniFile->GetNextKey(tszSection, tszKeyName, tszRetNextKey);
#endif
	}
	inline IIniFile* OpenIniFile(LPCTSTR lpName, int ForceUnpakFile = false, int ForWrite = false)
	{
		UnicodeConvertor Cvt;
		return g_OpenIniFile(Cvt.ToA(lpName), ForceUnpakFile, ForWrite);
	}
	inline IIniFile* CreateIniFile(LPCTSTR lpName)
	{
		UnicodeConvertor Cvt;
		IFile* p = g_CreateFile(Cvt.ToA(lpName));
		SAFE_RELEASE(p);
		return OpenIniFile(lpName, false, true);
	}
	inline VOID EraseSection(IIniFile* pFile, LPCTSTR tcsSetion)
	{
		UnicodeConvertor Cvt;
		pFile->EraseSection(Cvt.ToA(tcsSetion));
	}
};
namespace KGCH = KG_CUSTOM_HELPERS;
#endif //_INCLUDE_KGINIFILEHELPER_H_
