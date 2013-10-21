////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DShaderRCBridge.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-8-4 10:09:54
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "KG3DShaderRCBridge.h"
#include "KG3DEngineManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
////////////////////////////////////////////////////////////////////////////////


//<Connection Point to ShaderLibrary>	//对ShaderLibrary的文件引用就下面那么多了

#ifdef _IN_CPP_FILE_
#error	_IN_CPP_FILE_ should not be defined before here
#endif

#include "../../ShaderLibrary/ShaderLibrary/ShaderLibraryMain.h"
//</Connection Point to ShaderLibrary>

//HGLOBAL KG3DShaderRCBridge::LoadShaderRC(UINT uID, LPCVOID* ppData, DWORD* pSize )
//{
//	return ::ShaderLibraryLoadShader(uID, ppData, pSize);//ShaderLibrary的导入函数
//}

//VOID KG3DShaderRCBridge::TryInitResources()
//{
//	return;
//}

//UINT KG3DShaderRCBridge::GetShaderResourceID( LPCTSTR pFileName )
//{
//	/*_ASSERTE(NULL != pFileName && !s_mapFileName.empty());
//	TypeFileNameMap::iterator it = s_mapFileName.find(std::tstring(pFileName));
//	_ASSERTE(it != s_mapFileName.end());
//	if (it == s_mapFileName.end())
//	{
//		return 0;
//	}
//	return it->second;*/
//	return 0;
//}

//LPCTSTR KG3DShaderRCBridge::GetShaderResourcePath( UINT uResourceID )
//{
//	/*UINT uIndex = uResourceID - SHADER_RC_MIN_VALUE;
//	if (uIndex >= _countof(g_shaderRCArray))
//	{
//		return NULL;
//	}
//	return g_shaderRCArray[uIndex].strFilePath;*/
//
//	return NULL;
//}
//
//UINT KG3DShaderRCBridge::GetIDFromString( LPCTSTR filePathOrID )
//{
//	/*_ASSERTE(NULL != filePathOrID);
//	UINT	uID = 0;
//	BOOL bRet = KGCH::TStringToValue(filePathOrID, uID);
//	if(! bRet)
//		return 0;
//
//	_ASSERTE(0 != uID);
//	return uID;*/
//
//	return 0;
//}
//BOOL KG3DShaderRCBridge::IsIDValid(UINT uID)
//{
//	return 0 != uID;
//}
//BOOL KG3DShaderRCBridge::IsFilePath(LPCTSTR filePathOrID)
//{
//	return ! KG3DShaderRCBridge::IsIDValid(GetIDFromString(filePathOrID));
//}

//LPCTSTR KG3DShaderRCBridge::GetStringFromID( UINT uID, TCHAR uBuffer[MAX_PATH])
//{
//	/*BOOL bRet = KGCH::TValueToString(uID, (LPTSTR)uBuffer, (INT)MAX_PATH);
//	_ASSERTE(bRet);
//	if (! bRet)
//	{
//		return _T("0");
//	}
//
//	return uBuffer;*/
//	return NULL;
//}

struct KG3DShaderLibraryDummy : public IKG3DShaderLibrary//2009-8-25 暂时只在引擎中使用 
{
	virtual ULONG STDMETHODCALLTYPE AddRef(){return 1;}
	virtual ULONG STDMETHODCALLTYPE Release(){return 0;}

	virtual ULONG IsLibraryBuildedInUnicode(){return FALSE;}

	virtual HRESULT STDMETHODCALLTYPE GetShaderResourceA(LPCSTR , IKG3DShaderLibraryResource** )
	{
		return E_NOTIMPL;
	}
	virtual HRESULT STDMETHODCALLTYPE GetShaderResourceW(LPCWSTR , IKG3DShaderLibraryResource** )
	{
		return E_NOTIMPL;
	}
};

namespace
{
	LPCTSTR GetShaderLibraryFileName()
	{
#if defined(DEBUG) | defined(_DEBUG)
		return _T("KG3DShaderLibraryD.dll");
#else
		return _T("KG3DShaderLibrary.dll");
#endif

	}
};

struct ShaderLibraryHandle 
{
	HMODULE hModule;
	BOOL	bCanBeCreate;
	IKG3DShaderLibrary* pInterface;

	KG3DShaderLibraryDummy	dummyInterface;

	~ShaderLibraryHandle()
	{
		if (NULL != hModule)
		{
			KGLogPrintf(KGLOG_ERR, _T("%s没有进行主动释放:%s"), GetShaderLibraryFileName());
			::FreeLibrary(hModule);
			hModule = NULL;
		}
	}
};

static ShaderLibraryHandle g_shaderLibraryHandle = {NULL, true,NULL};

HRESULT KG3DShaderRCBridge::InitLibrary()
{
    int nRetCode = false;
    TCHAR szDllFilePath[MAX_PATH];

	_ASSERTE(NULL == g_shaderLibraryHandle.hModule && NULL == g_shaderLibraryHandle.pInterface);

	LPCTSTR strShaderLibraryFileName = GetShaderLibraryFileName();

	CHAR strFuncName[] = _STRINGERA(CreateKG3DShaderLibrary);
    {
        nRetCode = _sntprintf_s(szDllFilePath, _countof(szDllFilePath), _TRUNCATE, _T("%s%s"), g_szModuleFilePath, strShaderLibraryFileName) > 0;
        KGLOG_PROCESS_ERROR(nRetCode);

		g_shaderLibraryHandle.hModule = LoadLibrary(szDllFilePath);
		KG_PROCESS_ERROR(NULL != g_shaderLibraryHandle.hModule);

		FuncCreateKG3DShaderLibrary pCreateFunc = (FuncCreateKG3DShaderLibrary)::GetProcAddress(g_shaderLibraryHandle.hModule, strFuncName);
		KG_PROCESS_ERROR(NULL != pCreateFunc);

		HRESULT hr = pCreateFunc(&g_shaderLibraryHandle.pInterface);
		KGLOG_COM_PROCESS_ERROR(hr);

		_ASSERTE(NULL != g_shaderLibraryHandle.pInterface);

		TCHAR csTmp[MAX_PATH];
		_stprintf_s(csTmp, _T("%s加载成功\r\n"), strShaderLibraryFileName);
		::OutputDebugString(csTmp);
	}

	return S_OK;
Exit0:
	KGLogPrintf(KGLOG_ERR, _T("%s加载失败"), strShaderLibraryFileName);
	if (NULL == g_shaderLibraryHandle.pInterface)
	{
		if(NULL != g_shaderLibraryHandle.hModule)
		{
			::FreeLibrary(g_shaderLibraryHandle.hModule);
			g_shaderLibraryHandle.hModule = NULL;
		}
		g_shaderLibraryHandle.bCanBeCreate = FALSE;
	}
	return E_FAIL;
}

IKG3DShaderLibrary* KG3DShaderRCBridge::GetShaderLibraryInterface()
{	
	if (NULL != g_shaderLibraryHandle.pInterface)
	{
		return g_shaderLibraryHandle.pInterface;
	}
	return &g_shaderLibraryHandle.dummyInterface;
}

HRESULT KG3DShaderRCBridge::UninitLibrary()
{
	SAFE_RELEASE(g_shaderLibraryHandle.pInterface);

	if (NULL != g_shaderLibraryHandle.hModule)
	{
		::FreeLibrary(g_shaderLibraryHandle.hModule);
		g_shaderLibraryHandle.hModule = NULL;
	}
	return S_OK;
}

//#if defined(KG_ENABLE_TDD)
//KG_TEST_BEGIN(KG3DShaderRCBridge)
//{
//	using namespace KG3DShaderRCBridge;
//
//	std::tstring strTmp = _T("113");
//	_ASSERTE(GetIDFromString(strTmp.c_str()) == 113);
//
//	TCHAR strBuffer[MAX_PATH];
//	std::tstring str2 = GetStringFromID(113, strBuffer);
//	_ASSERTE(str2 == strTmp);
//}
//KG_TEST_END(KG3DShaderRCBridge)
//#endif