// Manager.h: interface for the Manager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MANAGER_H__9E441CAB_1391_48D3_909C_CCE958896882__INCLUDED_)
#define AFX_MANAGER_H__9E441CAB_1391_48D3_909C_CCE958896882__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SceneTable.h"	
#include "IMusicApplication.h"

class KEngineManager  
{
public:
	HRESULT Reset(void);
	HRESULT LoadConfig(LPSTR pFileName);

	HRESULT InitSoundSystem();
	HRESULT SetRenderWindow(HWND hRenderWindow);
	HRESULT CreateDefaultSceneTable();
	HRESULT ProcessMessage( UINT message, WPARAM wParam, LPARAM lParam);
	HRESULT Render();
	HRESULT InitializeWindowed(HWND hBaseWindow,HWND hRenderWindow);

	KEngineManager();
	virtual ~KEngineManager();

	LPSCENE Create1NewObjectEditorScene(void);
	LPSCENE Create1NewSwordTestScene(void);
	LPSCENE Create1NewScenceEditorScene(void);
	HRESULT SetCurScene(LPSCENE pScene);
	LPSCENE Create1NewMeshEditorScene(void);
	LPSCENE Create1NewGameWorldScene(void);

	HINSTANCE	m_hSoundDll;

	HRESULT Exit(void);
};

extern HWND  g_hBaseWnd;
extern HWND  g_hRenderWnd;

extern BOOL     g_Mouse_L;
extern BOOL     g_Mouse_M;
extern BOOL     g_Mouse_R;

extern BYTE     g_KeyState[256];
extern WORD     g_Def_FontHeight;
extern TCHAR    g_Def_FontName[80];
extern TCHAR    g_Def_AppDirectory[256];
extern TCHAR    g_Def_ModelDirectory[256];

extern IMusicApplication	*g_pSoundSys;
//extern KEngineManager       g_cEngineManager;
extern KSceneTable          g_SceneTable;

extern KModelTable             g_cMeshTable;
extern KTextureTable           g_cTextureTable;
extern KComponentObjectTable   g_cObjectTable;

extern BOOL g_bSafeMode;
#endif // !defined(AFX_MANAGER_H__9E441CAB_1391_48D3_909C_CCE958896882__INCLUDED_)
