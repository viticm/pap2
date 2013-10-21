////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : ISO3UI.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2007-11-29 9:40:49
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_ISO3UI_H_
#define _INCLUDE_ISO3UI_H_

////////////////////////////////////////////////////////////////////////////////

typedef BOOL (*fnStringToID)(LPCTSTR, int&);

struct SO3UI_INIT_PARAM
{
	HINSTANCE hInst;
	HWND hWnd;

	//游戏逻辑改为DLL后，这两个参数将不存在，到时候保留一个游戏逻辑接口的指针，游戏逻辑的接口里面应该包含这两个方法。
	fnStringToID fnAttribute; //将游戏逻辑将attribute的key值变为id
	fnStringToID fnRequire; //将游戏逻辑将require的key值变为id
};

struct lua_State;
struct luaL_reg;
struct KLuaConstList;
interface IKSO3GameWorldUIHandler;
interface ISO3UI
{
	virtual int Init(const SO3UI_INIT_PARAM *pParam);
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

	virtual IKSO3GameWorldUIHandler &GetGameWorldUIHandler() = 0;
};

#define	FN_CREATE_SO3UI	"CreateSO3UI"
typedef ISO3UI*	(*fnCreateSO3UI)();

#ifdef SO3UI_EXPORTS
extern "C" __declspec(dllexport) ISO3UI* CreateSO3UI();
#else
extern "C" __declspec(dllimport) ISO3UI* CreateSO3UI();
#endif

#endif //_INCLUDE_ISO3UI_H_
