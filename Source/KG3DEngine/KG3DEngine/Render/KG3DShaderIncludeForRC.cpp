////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DShaderIncludeForRC.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-8-4 10:14:50
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "KG3DShaderIncludeForRC.h"
#include "KG3DShaderRCBridge.h"
#include <atlbase.h>

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
////////////////////////////////////////////////////////////////////////////////
HRESULT STDMETHODCALLTYPE KG3DShaderIncludeForRC::Open(D3DXINCLUDE_TYPE, LPCSTR pFileName, LPCVOID, LPCVOID *ppData, UINT *pBytes)
{
//	_ASSERTE(NULL != pFileName);
//
//	{
//		std::tstring strFileFullName;
//		BOOL bRet = KGCH::StrPathGetFullFileName(pFileName, strFileFullName);
//		_ASSERTE(bRet && _T("检查在shader中的include写错了些什么"));
//		KG_PROCESS_ERROR(bRet);
//
//		std::transform(strFileFullName.begin(), strFileFullName.end(), strFileFullName.begin(), tolower);
//		UINT uID = KG3DShaderRCBridge::GetShaderResourceID(strFileFullName.c_str());
//		KG_PROCESS_ERROR(0 != uID);
//
//		DWORD uBypes = 0;
//		LPCVOID	pData = NULL;
//		HGLOBAL hglobalTmp = KG3DShaderRCBridge::LoadShaderRC(uID, &pData, &uBypes);	//改成可以对头文件进行搜索
//		KG_PROCESS_ERROR(NULL != hglobalTmp);
//
//		if(NULL != pBytes)
//			*pBytes = uBypes; 
//		if(NULL != ppData)
//			*ppData = pData;
//		return S_OK;
//	}
//Exit0:
//	KGLogPrintf(KGLOG_ERR, _T("资源中没有包含如下文件，检查资源文件:%s"), pFileName);
//	return E_FAIL;

	IKG3DShaderLibraryResource* pResource  = NULL;
	IKG3DShaderLibrary* pLibrary = KG3DShaderRCBridge::GetShaderLibraryInterface();
	KG_PROCESS_ERROR(NULL != pLibrary && NULL != pFileName && NULL != ppData && NULL != pBytes);
	{
		HRESULT hr = pLibrary->GetShaderResourceA(pFileName, &pResource);
		
		KG_COM_PROCESS_ERROR(hr);
		_ASSERTE(pResource);

		_ASSERTE(m_vecResources.size() < em_max_include_files);
		m_vecResources.push_back(pResource);

		_ASSERTE(NULL != pResource->GetBuffer());
		*ppData = pResource->GetBuffer();
		*pBytes = pResource->GetBufferSize();
		return S_OK;
	}
Exit0:
	return E_FAIL;	
}
HRESULT STDMETHODCALLTYPE KG3DShaderIncludeForRC::Close(LPCVOID)
{
	/*if (NULL != m_hResource)
	{
		::FreeResource(m_hResource);
		m_hResource = NULL;
	}*/	

	//不尝试关闭资源。因为这个类可能同时打开了几个头文件，一一记录其资源比较麻烦，而且多个shader公用了一些头文件，不停的关闭打开会造成性能损失。所以不尝试FreeResource，系统会随dll的生命周期自己管理这些内存的，参考MSDN，不FreeResource是正常行为。在这个类析构的时候统一Free就好了

	return S_OK;
}

KG3DShaderIncludeForRC::KG3DShaderIncludeForRC()
{
	//KG3DShaderRCBridge::TryInitResources();
	m_vecResources.reserve(em_max_include_files);
}

KG3DShaderIncludeForRC::~KG3DShaderIncludeForRC()
{
	/*if (NULL != m_hResource)
	{
		::FreeResource(m_hResource);
		m_hResource = NULL;
	}*/
	for (size_t i = 0, iend = m_vecResources.size(); i < iend; ++i)
	{
		IKG3DShaderLibraryResource*& p = m_vecResources[i];
		SAFE_RELEASE(p);
	}
}


