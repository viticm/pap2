// JX3Activator.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CJX3ActivatorApp:
// See JX3Activator.cpp for the implementation of this class
//

class CJX3ActivatorApp : public CWinApp
{
public:
	CJX3ActivatorApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CJX3ActivatorApp theApp;