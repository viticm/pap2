////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_JX3MultiVersionSetupApp.h
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-10-22  16:30:50
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

class KG_JX3MultiVersionSetupApp : public CWinApp
{
public:
	KG_JX3MultiVersionSetupApp();

	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern KG_JX3MultiVersionSetupApp theApp;
