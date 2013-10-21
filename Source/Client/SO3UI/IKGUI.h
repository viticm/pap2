////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IKGUI.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2007-11-30 10:37:08
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IKGUI_H_
#define _INCLUDE_IKGUI_H_

////////////////////////////////////////////////////////////////////////////////

struct KGUI_EVENT
{
	LPCSTR pszName;
	DWORD dwID;
};

interface IKG3DUI;
interface IKG3DEngineManager;
interface IKG3DResourceManager;
interface IKSO3RepresentHandler;
struct KGUI_INIT_PARAM
{
	HINSTANCE hInst;
	HWND hWnd;
	IKG3DUI* p3DUI;
	IKG3DEngineManager* p3DEngineManager;
	IKG3DResourceManager* p3DModelManager;
	IKSO3RepresentHandler* pRepresent;
};

struct lua_State;
struct luaL_reg;
struct KLuaConstList;
interface IKGUI
{
	virtual int Init(const KGUI_INIT_PARAM *pParam);
	virtual void Exit() = 0;

	virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
	virtual void Active() = 0;
	virtual void Paint() = 0;

	virtual lua_State* GetLua() = 0;
	//当然，也可以取得lua_State，自己写函数注册这些变量
	virtual void RegisterMetaTable(LPCSTR pszTable, luaL_reg const* pMetaFunc) = 0;
	virtual void RegisterLibTable(LPCSTR pszTable, luaL_reg const* pLibFunc) = 0;
	virtual void RegisterFunctions(luaL_reg const* pFunc) = 0;
	virtual void RegisterConstList(KLuaConstList const* pConstList) = 0;

	virtual void RegisterEvent(const KGUI_EVENT *pEventList) = 0;
	virtual void ClearArgs() = 0;
	virtual void PushArg(bool bParam) = 0;
	virtual void PushArg(int nParam) = 0;
	virtual void PushArg(DWORD dwParam) = 0;
	virtual void PushArg(float fParam) = 0;
	virtual void PushArg(double dblParam) = 0;
	virtual void PushArg(LPCSTR pcszParam) = 0;
	virtual void FireEvent(DWORD dwEvent, DWORD dwCookie) = 0;

};

#define	FN_CREATE_KGUI	"CreateKGUI"
typedef IKGUI*	(*fnCreateKGUI)();

#ifdef KGUI_EXPORTS
extern "C" __declspec(dllexport) IKGUI* CreateKGUI();
#else
extern "C" __declspec(dllimport) IKGUI* CreateKGUI();
#endif

#endif //_INCLUDE_IKGUI_H_
