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

#ifndef _KGTestCommon_H__
#define _KGTestCommon_H__

#include "stdafx.h"

#ifndef EPSILON
#define EPSILON 0.00001f  // 用于判断浮点数是否等于0
#endif

#ifndef INV_SIGN_BIT
#define INV_SIGN_BIT 0x7FFFFFFF  // 正位
#endif

#ifdef _DEBUG
const char ENGINE_DLL_NAME[] = "KG3DEngine_AutoTestD.dll";
#else
const char ENGINE_DLL_NAME[] = "KG3DEngine_AutoTest.dll";
#endif

const char LOG_NAME[] = "Logs";
const int  MAX_LOG_LINE = MAX_PATH * 3;

extern char g_szApplicationPath[MAX_PATH];
extern char g_szLogPath[MAX_PATH];

namespace KGTestCommon
{
	string IntToString(const int nValue);
	float FastFabs(const float& pfIn);
	BOOL FloatEqual(float fNum1, float fNum2);
	BOOL D3DCOLORVALUEqual(D3DCOLORVALUE pCo1, D3DCOLORVALUE pCo2);
	BOOL D3DMATERIAL9Equal(D3DMATERIAL9 pMat1, D3DMATERIAL9 pMat2);

	//测试工程初始化
	int  TestingInit(const char cszProjectName[]);
	int  TestingUnit(const char cszProjectName[]);

	//格式化目录
	int  FormatPath(char* pszPath, const size_t cuSize);

	//构造统一的输出目录
	int  MakeResultPath(const char cszProjectName[], char* pszResultPath, const size_t cuSize);

	//查找日志文件夹中最新的日志文件
	int  FindLogFile(const char cszLogPath[], char* pszLogName, const size_t cuSize);

	//抽取子串
	//例：GetSubStr("abcdefg", "a", "g", pszSub, cuSize); -> pszSub = "bcdef"
	int  GetSubStr(const char cszSource[],
				   const char cszStart[],
				   const char cszEnd[],
				   char* pszSub,
				   const size_t cuSize);

	//输出运行时的信息到控制台和日志
	int  OutInfo(const char cszInfo[]);

	//清空目录
	int  CleanPath(const char cszPath[]);

	//获取范围内的随机数
	int  GetRandom(int nMin, int nMax);
};
#endif