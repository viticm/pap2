//////////////////////////////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
//  FileName    :   KGPostRenderTestingMgr.h    
//  Version     :   1.0
//  Creater     :   dinglong
//  Date        :   2009-7-28 10:00
//  Comment     :   1.负责管理所有屏幕特效测试用例，进行用例的加载、初始化、执行、销毁的操作
//					2.为每个用例提供相同的测试环境
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _KGPostRenderTestingMgr_H_
#define _KGPostRenderTestingMgr_H_

#include "KGTestEffect.h"

const TCHAR TEST_PROJECT_NAME[] = "KGPostRenderTesting";

class KGPostRenderTestingMgr
{
public:
	KGPostRenderTestingMgr(void);
	~KGPostRenderTestingMgr(void);

public:
	int  Init(const TCHAR cszClientPath[], const HWND chWindow);
	void UnInit();
	int	 Run();	

private:
	int LoadEngineDLL();
	int LoadTestCaseFromFile();
	int MakeupEffectType(const TCHAR cszTypeName[], DWORD* dwTypeValue);

private:
	TCHAR m_szClientPath[MAX_PATH];
	TCHAR m_szResultPath[MAX_PATH];
	HMODULE m_hEngineDLL;
	IKG3DEngineManager* m_pEngineMgr;
	HWND m_hRenderWindow;
	KG3DEngineOption m_EngineOption;

	vector<int> m_vecEffectType;
	vector<int> m_vecTestFrame;
	vector<KGTestEffect*> m_vecTestCases;
	int m_nFrame;
	bool m_bKMemoryInited;
};

#endif