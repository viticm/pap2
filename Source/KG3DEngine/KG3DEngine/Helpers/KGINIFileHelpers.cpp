////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGINIFileHelpers.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-6-22 16:09:08
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "KGINIFileHelpers.h"
#include "KGMathFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define FALSE_ASSERT_RETURN(condition)	\
{	\
	if(!(condition))	\
		return false;	\
}NULL
////////////////////////////////////////////////////////////////////////////////
namespace KG_CUSTOM_HELPERS
{
	//INT////////////////////////////////////////////////////////////////////////
	bool Private::SaveINIFileHelper(IIniFile* pIniFile, const char* szSection, const char* szKeyName, INT RetData )
	{
		FALSE_ASSERT_RETURN(pIniFile&&szSection&&szKeyName);
		INT nRet = pIniFile->WriteInteger(szSection, szKeyName, RetData);
		return nRet ? true : false;
	}

	bool Private::LoadINIFileHelper(IIniFile* pIniFile, const char* szSection, const char* szKeyName, INT _DefaultData, INT* RetData )
	{
		FALSE_ASSERT_RETURN(pIniFile&&szSection&&szKeyName&&RetData);
		INT nRet = pIniFile->GetInteger(szSection, szKeyName, _DefaultData, RetData);
		return nRet ? true : false;
	}
	//UINT//////////////////////////////////////////////////////////////////////////
	bool Private::SaveINIFileHelper(IIniFile* pIniFile, const char* szSection, const char* szKeyName, UINT DataRet)
	{
		TCHAR tcsTemp[MAX_PATH];
		ZeroMemory(tcsTemp, _countof(tcsTemp));
		FALSE_ASSERT_RETURN(pIniFile&&szSection&&szKeyName);
		FALSE_ASSERT_RETURN(_stprintf_s(tcsTemp, "%u", DataRet) >= 0);
		FALSE_ASSERT_RETURN(pIniFile->WriteString(szSection, szKeyName, tcsTemp));
		return true;
	}
	bool Private::LoadINIFileHelper(IIniFile* pIniFile, const char* szSection, const char* szKeyName, UINT DefaultData, UINT* DataRet)
	{
		//注意，这里是非Unicode的
		TCHAR tcsBuffer[MAX_PATH];
		UINT Value  = 0;
		ZeroMemory(tcsBuffer, _countof(tcsBuffer));
		FALSE_ASSERT_RETURN(pIniFile&&szSection&&szKeyName&&DataRet);
		FALSE_ASSERT_RETURN(pIniFile->GetString(szSection, szKeyName, "", tcsBuffer, _countof(tcsBuffer)));
		if (0 == _tcscmp(tcsBuffer, _T("0")))
		{
			Value = 0;
			goto Exit1;
		}
		Value = _tcstoul(tcsBuffer, NULL, 10);
		errno_t err = 0;
		_get_errno(&err);
		FALSE_ASSERT_RETURN(Value != 0 && err != ERANGE);
Exit1:
		*DataRet = Value;
		return true;
	}
	//DWORD_PTR
	//bool Private::SaveINIFileHelper( IIniFile* pIniFile, const char* szSection, const char* szKeyName, DWORD_PTR DataRet )
	//{
	//	_ASSERTE(NULL);//没有写完的
	//	return false;
	//}
	//bool Private::LoadINIFileHelper( IIniFile* pIniFile, const char* szSection, const char* szKeyName, DWORD_PTR DefaultData, DWORD_PTR* DataRet )
	//{
	//	_ASSERTE(NULL);
	//	return false;
	//}
	//D3DCOLOR////////////////////////////////////////////////////////////////////////
	bool Private::SaveINIFileHelper( IIniFile* pIniFile, const char* szSection, const char* szKeyName, D3DCOLOR RetData )
	{
		FALSE_ASSERT_RETURN(pIniFile&&szSection&&szKeyName);
		int ARGB[4];
		ARGB[0] = D3DCOLOR_GETALPHA(RetData);
		ARGB[1] = D3DCOLOR_GETRED(RetData);
		ARGB[2] = D3DCOLOR_GETGREEN(RetData);
		ARGB[3] = D3DCOLOR_GETBLUE(RetData);
		FALSE_ASSERT_RETURN(pIniFile->WriteMultiInteger(szSection, szKeyName, ARGB, 4));
		return true;
	}
	bool Private::LoadINIFileHelper( IIniFile* pIniFile, const char* szSection, const char* szKeyName, D3DCOLOR DefaultData, D3DCOLOR* DataRet )
	{
		FALSE_ASSERT_RETURN(pIniFile&&szSection&&szKeyName&&DataRet);
		int ARGB[4];
		memset(ARGB, 0, sizeof(ARGB));
		INT nRet = pIniFile->GetMultiInteger(szSection, szKeyName, ARGB, 4);
		if (nRet)
		{
			*DataRet = D3DCOLOR_ARGB(ARGB[0], ARGB[1], ARGB[2], ARGB[3]);	
			return true;
		}
		//*DataRet = DefaultData;///不理会默认值???
		*DataRet = DefaultData;
		return false;
	}
	
	//bool//////////////////////////////////////////////////////////////////////////
	bool Private::SaveINIFileHelper( IIniFile* pIniFile, const char* szSection, const char* szKeyName, bool DataRet )
	{
		FALSE_ASSERT_RETURN(pIniFile&&szSection&&szKeyName);
		FALSE_ASSERT_RETURN(pIniFile->WriteInteger(szSection, szKeyName, DataRet? 1 : 0));
		return true;
	}
	bool Private::LoadINIFileHelper( IIniFile* pIniFile, const char* szSection, const char* szKeyName, bool DefaultData, bool* DataRet )
	{
		FALSE_ASSERT_RETURN(pIniFile&&szSection&&szKeyName&&DataRet);
		INT nTempResult = 0;
		INT nRet = pIniFile->GetInteger(szSection, szKeyName, DefaultData, &nTempResult);
		if (nRet)
		{
			*DataRet = nTempResult ? true : false;
			return true;
		}
		return false;
	}
	//FLOAT////////////////////////////////////////////////////////////////////////
	bool Private::SaveINIFileHelper( IIniFile* pIniFile, const char* szSection, const char* szKeyName, FLOAT DataRet )
	{
		FALSE_ASSERT_RETURN(pIniFile&&szSection&&szKeyName);
		FALSE_ASSERT_RETURN(pIniFile->WriteFloat(szSection, szKeyName, DataRet));
		return true;
	}
	bool Private::LoadINIFileHelper( IIniFile* pIniFile, const char* szSection, const char* szKeyName, FLOAT DefaultData, FLOAT* DataRet )
	{
		FALSE_ASSERT_RETURN(pIniFile&&szSection&&szKeyName&&DataRet);
		FLOAT nTempResult = 0;
		INT nRet = pIniFile->GetFloat(szSection, szKeyName, DefaultData, &nTempResult);
		if (nRet)
		{
			*DataRet = nTempResult;
			return true;
		}
		return false;
	}
	//String////////////////////////////////////////////////////////////////////////
	bool Private::SaveINIFileHelper( IIniFile* pIniFile, const char* szSection, const char* szKeyName, LPCSTR lpStr )
	{
		FALSE_ASSERT_RETURN(pIniFile&&szSection&&szKeyName&&lpStr);
		return pIniFile->WriteString(szSection, szKeyName, lpStr)?true:false;
	}

	bool Private::LoadINIFileHelper( IIniFile* pIniFile, const char* szSection, const char* szKeyName, LPCSTR DefaultData, LPSTR lpStr, unsigned int uBufferSize )
	{
		FALSE_ASSERT_RETURN(pIniFile&&szSection&&szKeyName&&lpStr);//&&DefaultData可以为NULL
		return pIniFile->GetString(szSection, szKeyName, DefaultData, lpStr, uBufferSize)?true:false;
	}	

	
};
