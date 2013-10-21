////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_JX3LauncherApp.h
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-10-29  15:07:53
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"


class KG_JX3LauncherApp : public CWinApp
{
public:
	KG_JX3LauncherApp();

public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern KG_JX3LauncherApp theApp;