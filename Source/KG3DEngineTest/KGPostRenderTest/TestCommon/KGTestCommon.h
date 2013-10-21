//////////////////////////////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
//  FileName    :   KGTestCommon.h    
//  Version     :   1.0
//  Creater     :   dinglong
//  Date        :   2009-6-2 10:00
//  Comment     :   公共的测试辅助函数
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _KGTestCommon_H_
#define _KGTestCommon_H_

#include "stdafx.h"

#ifdef _DEBUG
const TCHAR ENGINE_DLL_NAME[] = _T("KG3DEngine_AutoTestD.dll");
#else
const TCHAR ENGINE_DLL_NAME[] = _T("KG3DEngine_AutoTest.dll");
#endif

const TCHAR LOG_NAME[] = _T("Logs");
extern TCHAR g_szApplicationPath[MAX_PATH];
extern TCHAR g_szLogPath[MAX_PATH];

namespace KGTestCommon
{
	std::string IntToString(const int nValue);
	//测试工程初始化
	int  TestingInit(const TCHAR cszProjectName[]);
	int  TestingUnInit(const TCHAR cszProjectName[]);
	//格式化目录
	int  FormatPath(TCHAR* pszPath, const size_t cuSize);
	//构造统一的输出目录
	int  MakeResultPath(const TCHAR cszProjectName[], TCHAR* pszResultPath, const size_t cuSize);
	//查找日志文件夹中最新的日志文件
	int  FindLogFile(const TCHAR cszLogPath[], TCHAR* pszLogName, const size_t cuSize);
};
#endif