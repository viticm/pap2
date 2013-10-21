// GameConfig.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// KGameConfigApp:
// 有关此类的实现，请参阅 GameConfig.cpp
//

class KGameConfigApp : public CWinApp
{
public:
	KGameConfigApp();

// 重写
	public:
	virtual BOOL InitInstance();

// 实现
private:
	int CanRun();

	int _CheckSystemMemory();
	int _GetCurrentVersionPath(TCHAR szVersionPath[], TCHAR* szRetVersinPath, unsigned uBuffSize);
	void _StringTrim(TCHAR szString[]);

	DECLARE_MESSAGE_MAP()
};

extern KGameConfigApp theApp;