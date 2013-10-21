// JX3NewVersionSetup.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CJX3NewVersionSetupApp:
// See JX3NewVersionSetup.cpp for the implementation of this class
//

class CJX3NewVersionSetupApp : public CWinApp
{
public:
	CJX3NewVersionSetupApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CJX3NewVersionSetupApp theApp;