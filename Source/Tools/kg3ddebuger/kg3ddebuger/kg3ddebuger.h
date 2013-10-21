// kg3ddebuger.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "KG3DInterface.h"


// Ckg3ddebugerApp:
// See kg3ddebuger.cpp for the implementation of this class
//

enum 
{
    Call_Get_Render = 0,
    Call_Set_Render,
    Call_Get_FrameMove,
    Call_Set_FrameMove,
    Call_Get_Render_Param,
    Call_Set_Render_Param
};

struct RemoveParam
{
    HMODULE (WINAPI *LoadLibraryA)(LPCSTR);
    BOOL (WINAPI *FreeLibrary)(HMODULE);
    FARPROC (WINAPI *GetProcAddress)(HMODULE, LPCSTR);

    char  LibName[128];
    char  PrcName[128];
    
    DWORD CallType;
    DWORD Param1;
    DWORD Param2;

};

class Ckg3ddebugerApp : public CWinApp
{
public:
	Ckg3ddebugerApp();
    virtual ~Ckg3ddebugerApp();

// Overrides
	public:
	virtual BOOL InitInstance();

    HRESULT SetRenderOption(RENDER_OPTION opt, BOOL bEnable);
    BOOL    GetRenderOption(RENDER_OPTION opt);

    HRESULT SetFrameMoveOption(FRAMEMOVE_OPTION opt, BOOL bEnable);
    BOOL    GetFrameMoveOption(FRAMEMOVE_OPTION opt);

    HRESULT SetRenderParam(RENDER_PARAM opt, DWORD Value);
    DWORD   GetRenderParam(RENDER_PARAM opt);
    

    void    Release();

    HANDLE  m_hHookProcess;
    HMODULE m_hHookModule;
    CString m_strHookName;    

    RemoveParam m_RemoveParam;

    void*   m_pRemoveCode;
    void*   m_pRemoveData;

	DECLARE_MESSAGE_MAP()
};


extern Ckg3ddebugerApp theApp;