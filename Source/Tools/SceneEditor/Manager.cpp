// Manager.cpp: implementation of the Manager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "Manager.h"
#include ".\manager.h"
#include "RInterface.h"

#include "KSO3World.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define FREELIBRARY(hLib) { if (hLib) { FreeLibrary(hLib); (hLib) = NULL;}}
#define TERMINATEINTERFACE(i) { if (i) { (i)->Terminate(); (i) = NULL;}}

HWND  g_hBaseWnd;
HWND  g_hRenderWnd;

CreateInterfaceFn SoundIFactory;

KGraphicsTool        g_cGraphicsTool;
IMusicApplication	*g_pSoundSys;

BOOL     g_Mouse_L;
BOOL     g_Mouse_M;
BOOL     g_Mouse_R;

BYTE     g_KeyState[256];
WORD     g_Def_FontHeight;
TCHAR    g_Def_FontName[80];
TCHAR    g_Def_AppDirectory[256];
TCHAR    g_Def_ModelDirectory[256];
TCHAR    g_Def_TextureDirectory[256];
TCHAR    g_Def_ModelTextureDirectory[256];

//KEngineManager g_cEngineManager;
KSceneTable    g_SceneTable;
KModelTable    g_cMeshTable;
KTextureTable           g_cTextureTable;
KComponentObjectTable   g_cObjectTable;

BOOL g_bSafeMode = false;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KEngineManager::KEngineManager()
{
	g_Mouse_L = FALSE;
	g_Mouse_M = FALSE;
	g_Mouse_R = FALSE;

	g_pSoundSys = NULL;
	m_hSoundDll = NULL;

	g_hBaseWnd = NULL;
	g_hRenderWnd = NULL;
	g_SceneTable.SetManager(this);  
	ZeroMemory(g_KeyState,sizeof(BYTE)*256);
	g_Def_FontHeight = 12;
	wsprintf(g_Def_FontName,"宋体");
	GetCurrentDirectory(256,g_Def_AppDirectory);
	wsprintf(g_Def_ModelDirectory,"%s\\Models",g_Def_AppDirectory);
	wsprintf(g_Def_TextureDirectory,"%s\\Textures",g_Def_AppDirectory);
	wsprintf(g_Def_ModelTextureDirectory,"%s\\Models\\Texture",g_Def_ModelDirectory);
	SetCurrentDirectory(g_Def_ModelDirectory);

}

KEngineManager::~KEngineManager()
{
	g_hBaseWnd = NULL;
	g_hRenderWnd = NULL;
}

HRESULT KEngineManager::InitializeWindowed(HWND hBaseWindow, HWND hRenderWindow)
{
	g_hBaseWnd = hBaseWindow;
	g_hRenderWnd = hRenderWindow;
	
	
	if (FAILED(g_GraphiceEngine.InitializeWindowed(hBaseWindow,hRenderWindow)))
		return E_FAIL;
	
	//CreateDefaultSceneTable();
	//g_GraphiceEngine.SetCurScene(g_SceneTable.m_lpCurScene);  
	if (FAILED(InitSoundSystem()))
		return E_FAIL;
	//g_pSoundSys->PlayAmusic("Music/fallingstar.mid",PlayNormal);

	LoadConfig("");

	g_cFileFinder.AddPath(g_Def_AppDirectory,TRUE);
	
	g_SetRootPath(g_Def_AppDirectory);
//游戏世界初始化
	BOOL bRetCode = FALSE;

	bRetCode = g_SO3World.Init(0);
	KG_PROCESS_ERROR(bRetCode);

	return S_OK;

Exit0:

	return S_OK;
}

HRESULT KEngineManager::LoadConfig(LPSTR pFileName)
{
	TCHAR Name[256];

	
	wsprintf(Name,"%s\\配置文件\\Config.ini",g_Def_AppDirectory);
	IIniFile* pIni = g_OpenIniFile(Name);
	if(!pIni)
	{
		MessageBox(g_hBaseWnd,Name,"Load Failed!",MB_ICONSTOP);
		return E_FAIL;
	}	
	pIni->GetString("MODEL_PATH","MainPath     ","模型文件",Name,256);
	
	TrimString(Name);
	wsprintf(g_Def_ModelDirectory,"%s\\%s",g_Def_AppDirectory,Name);
	wsprintf(g_Def_ModelTextureDirectory,"%sTexture",g_Def_ModelDirectory);
	pIni->Release();
	return S_OK;
}

HRESULT KEngineManager::Render()
{
	return g_GraphiceEngine.Render(); 
}

HRESULT KEngineManager::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	g_GraphiceEngine.SetCurScene(g_SceneTable.m_lpCurScene);  

	if (g_SceneTable.m_lpCurScene!=NULL)
	{
		g_SceneTable.m_lpCurScene->ProcessMessage(message,wParam,lParam) ;
	}
	return S_OK;
}

HRESULT KEngineManager::CreateDefaultSceneTable()
{
	LPSCENE pScene = NULL;
	if (SUCCEEDED(g_SceneTable.Get1NewScene(1,&pScene)))
	{
		g_SceneTable.m_dCurSceneID = pScene->ID;
		g_SceneTable.RefreshCurScene(); 
	}
	return S_OK;
}

HRESULT KEngineManager::SetRenderWindow(HWND hRenderWindow)
{
	g_GraphiceEngine.SetRenderWindow(hRenderWindow);
	return S_OK;
}

LPSCENE KEngineManager::Create1NewScenceEditorScene(void)
{
	LPSCENE pScene = NULL;
	if (SUCCEEDED(g_SceneTable.Get1NewScene(1,&pScene)))
	{
		g_SceneTable.m_dCurSceneID = pScene->ID;
		g_SceneTable.RefreshCurScene(); 
		return pScene;
	}
	else
	{
		//return NULL;
		g_SceneTable.m_dCurSceneID = pScene->ID;
		g_SceneTable.RefreshCurScene(); 
		return pScene;
	}

}

LPSCENE KEngineManager::Create1NewObjectEditorScene(void)
{
	LPSCENE pScene = NULL;
	if (SUCCEEDED(g_SceneTable.Get1NewScene(SCENE_OBJECTEDITOR,&pScene)))
	{
		g_SceneTable.m_dCurSceneID = pScene->ID;
		g_SceneTable.RefreshCurScene(); 
	}
	return pScene;
}

HRESULT KEngineManager::SetCurScene(LPSCENE pScene)
{
	if (pScene==NULL)
		return E_FAIL;
    g_SceneTable.m_dCurSceneID = pScene->ID;
	g_SceneTable.RefreshCurScene(); 
	g_GraphiceEngine.SetCurScene(g_SceneTable.m_lpCurScene);  

	return S_OK;
}

LPSCENE KEngineManager::Create1NewMeshEditorScene(void)
{
	LPSCENE pScene = NULL;
	if (SUCCEEDED(g_SceneTable.Get1NewScene(2,&pScene)))
	{
		g_SceneTable.m_dCurSceneID = pScene->ID;
		g_SceneTable.RefreshCurScene(); 
	}
	return pScene;
}

LPSCENE KEngineManager::Create1NewSwordTestScene(void)
{
	LPSCENE pScene = NULL;
	if (SUCCEEDED(g_SceneTable.Get1NewScene(SCENE_SWORDTEST,&pScene)))
	{
		g_SceneTable.m_dCurSceneID = pScene->ID;
		g_SceneTable.RefreshCurScene(); 
	}
	return pScene;
}

LPSCENE KEngineManager::Create1NewGameWorldScene(void)
{
	LPSCENE pScene = NULL;
	if (SUCCEEDED(g_SceneTable.Get1NewScene(SCENE_DEFAULT,&pScene)))
	{
		g_SceneTable.m_dCurSceneID = pScene->ID;
		g_SceneTable.RefreshCurScene(); 
	}
	return pScene;
}

HRESULT KEngineManager::InitSoundSystem()
{
    char szPath[MAX_PATH];
    strcpy(szPath, g_Def_AppDirectory);
    strcat(szPath, "\\");
    strcat(szPath, "MusicCom.dll");
	// 载入SpecialFx.dll
	m_hSoundDll = LoadLibrary(szPath);
	if ( !m_hSoundDll )
	{
		MessageBox( g_hBaseWnd, "Failed loading MusicCom.dll", "Error", MB_OK );
		return E_FAIL;
	}

	// 查询IClient接口实例
	SoundIFactory = Sys_GetFactory( m_hSoundDll );
	if ( SoundIFactory )
		g_pSoundSys = (IMusicApplication*)SoundIFactory(IMUSIC_INTERFACE_VERSION);
	if ( !g_pSoundSys )
	{
		MessageBox( g_hBaseWnd, "Failed find interface IMUSIC_INTERFACE_VERSION!", "Error", MB_OK );
		//m_pLog->PrintLog("[Game Init]Failed find interface IMUSIC_INTERFACE_VERSION!");
		return E_FAIL;
	}

	g_pSoundSys->Init();

	return TRUE;
}

HRESULT KEngineManager::Exit(void)
{
	g_SceneTable.CleanUp();	
	g_GraphiceEngine.CleanUp();
	g_cGraphicsTool.CleanUp();

	if(g_pSoundSys)
	{
		g_pSoundSys->Terminate();
		g_pSoundSys = NULL;
	}
	return E_NOTIMPL;
}

HRESULT KEngineManager::Reset(void)
{
	g_SceneTable.CleanUp();	
	g_GraphiceEngine.Reset();

	return S_OK;
}
