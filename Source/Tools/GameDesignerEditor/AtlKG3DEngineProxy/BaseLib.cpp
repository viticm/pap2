// BaseLib.cpp : Implementation of CBaseLib

#include "stdafx.h"
#include "BaseLib.h"
#include "Engine/KMemory.h"

BOOL g_bMemoryInitFlag = FALSE;

// CBaseLib

STDMETHODIMP CBaseLib::InitLog(BSTR szIdentity, LONG nMaxlineEachFile, LONG Options, BSTR szPath)
{
	KGLOG_PARAM LogParam;
	strcpy(LogParam.szIdent, BSTR_TO_STRING(szIdentity));
	LogParam.nMaxLineEachFile = nMaxlineEachFile;
	LogParam.Options = (KGLOG_OPTIONS)Options;
	strcpy(LogParam.szPath, BSTR_TO_STRING(szPath));

	if (!KGLogInit(LogParam, NULL))
	{
		::MessageBox(NULL, "Log文件初始化失败!", NULL, NULL);
	}

	KGLogPrintf(KGLOG_ERR, "AHPHO : loginit suc");

	return S_OK;
}

STDMETHODIMP CBaseLib::InitMemory(BSTR memoryName)
{
	g_bMemoryInitFlag = KMemory::Initialize("SceneEditor.memory");

	return S_OK;
}

STDMETHODIMP CBaseLib::UnInitMemory()
{
	if (g_bMemoryInitFlag)
	{
		KMemory::Finalize();
		g_bMemoryInitFlag = FALSE;
	}

	return S_OK;
}