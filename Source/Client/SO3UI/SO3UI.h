////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : SO3UI.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2007-11-29 10:09:00
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_SO3UI_H_
#define _INCLUDE_SO3UI_H_

#include "SO3UI/ISO3UI.h"

struct SO3UI_INIT_PARAM;
struct KLuaConstList;
struct luaL_reg;
struct IKGUI;

class IKSO3RepresentHandler;
class ISO3Interaction;
class KSO3UI : public ISO3UI
{
public:
	KSO3UI();
	virtual ~KSO3UI();

	virtual int Init(const SO3UI_INIT_PARAM *pParam);
    virtual void UnInit();
	virtual int NeedReset();
	virtual void Reset();
	virtual void Run();
	virtual void Exit();
	virtual void SetSoundShell(IKG3DSoundShell *pShell);

	virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void Active();
	virtual void Paint();
    virtual void BeginPaint();
    virtual void EndPaint();

    virtual IKGUI* GetIKGUI();

	virtual IKSO3GameWorldUIHandler& GetGameWorldUIHandler();

	void GetCursorPosition(int *pnX, int *pnY);

public:
	IKGUI* m_pUI;
	ISO3Interaction * m_pInteraction;

	HINSTANCE m_hInst;
	HWND m_hWnd;

	IKG3DUI* m_p3DUI;
	IKG3DEngineManager* m_p3DEngineManager;
	IKG3DResourceManager* m_p3DModelManager;
	IKSO3RepresentHandler* m_pRepresent;

    char m_szIceClientLibDllPath[MAX_PATH];

private:
	HMODULE m_hUIDll;
	HMODULE m_hInteraction;
};

extern KSO3UI* g_pSO3UI;

#endif //_INCLUDE_SO3UI_H_
