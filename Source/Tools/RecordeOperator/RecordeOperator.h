// RecordeOperator.h : main header file for the RecordeOperator application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CRecordeOperatorApp:
// See RecordeOperator.cpp for the implementation of this class
//

class CRecordeOperatorApp : public CWinApp
{
public:
	CRecordeOperatorApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
// Implementation
	afx_msg void OnAppAbout();
	afx_msg void OnViewProgressBar();
	afx_msg void OnCheckViewProgressBar(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()
protected:
	BOOL bIsCheckedViewProgressBar;
};

extern CRecordeOperatorApp theApp;