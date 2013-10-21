// KG_D3DMemoryLeakAnalyzer.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CKG_D3DMemoryLeakAnalyzerApp:
// See KG_D3DMemoryLeakAnalyzer.cpp for the implementation of this class
//

class CKG_D3DMemoryLeakAnalyzerApp : public CWinApp
{
public:
	CKG_D3DMemoryLeakAnalyzerApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CKG_D3DMemoryLeakAnalyzerApp theApp;