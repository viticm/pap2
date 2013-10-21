//////////////////////////////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
//  FileName    :   KGMapResSearchDlg.h    
//  Version     :   1.0
//  Creater     :   dinglong
//  Date        :   2009-8-27 10:00
//  Comment     :   地图场景资源搜索工具主窗口对话框
//
//////////////////////////////////////////////////////////////////////////////////////
#ifndef _KGMapResSearchDlg_H__
#define _KGMapResSearchDlg_H__

#include "KGMapResSearch.h"

class KGMapResSearchDlg
{
public:
	KGMapResSearchDlg(void);
	~KGMapResSearchDlg(void);

	int  Init(HINSTANCE hInstance, IKG3DEngineManager* pEngineMgr, const TCHAR cszClientPath[]);

private:
	static LRESULT CALLBACK DialogProc(HWND hDlg,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam);
	static KGMapResSearch m_MapResSearch;
	static HINSTANCE m_hInstance;
	
};

#endif