#include "StdAfx.h"
#include "SceneEditorCommon.h"
#include "KG3DEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

TCHAR g_EnginePath[MAX_PATH];
TCHAR g_szDefWorkDirectory[MAX_PATH];
CDllHolder g_3dEngineDll;

LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
BYTE*     g_SceneEditorKeyState = NULL;

//TCHAR g_strWeaponDirectory[MAX_PATH];
//TCHAR g_strItemDirectory[MAX_PATH];

IEKG3DEngineManager* g_pEngineManager = NULL;
IEKG3DGraphicsTool* g_pGraphicsTool = NULL;
IKG3DEngineManager* g_GetEngineManager()
{
	return dynamic_cast<IKG3DEngineManager*>(g_pEngineManager);
}


IKG3DResourceManager* g_GetModelManager()
{
	IKG3DResourceManager* pTable = NULL;
	g_pEngineManager->GetRepresentObjectTable(&pTable);
	_ASSERTE(pTable);
	return pTable;
}


void InitGlobalVars(IEKG3DEngineManager* pInterface)
{
	g_pEngineManager = pInterface;
	g_pEngineManager->GetGraphicsTool(&g_pGraphicsTool);
	//g_pEngineManager->GetDefAppDirectory(g_szDefAppDirectory, sizeof(g_szDefAppDirectory));
	g_pEngineManager->GetD3DDevice(&g_pd3dDevice);
	g_pEngineManager->GetKeyStateBuffer(&g_SceneEditorKeyState);

	//g_pEngineManager->GetWeaponDirectory(g_strWeaponDirectory, MAX_PATH);
	//g_pEngineManager->GetItemDirectory(g_strItemDirectory, MAX_PATH);
}

BOOL Init3DEngine(unsigned uMode, unsigned int uFlag, HWND hBaseWindow, HWND hRenderWindow, BSTR strEnginePath, BSTR strStartUpPath)
{
	ATLTRACE("#####################3DEngine Initializing!##################\r\n");
	
	TCHAR szCurDir[MAX_PATH];
	sprintf(szCurDir, BSTR_TO_STRING(strStartUpPath));
	//strcat(szCurDir, "/3DProxy/");
	//g_SetRootPath(szCurDir);
	//SetCurrentDirectory(szCurDir);
	
#ifdef _DEBUG
	strcat(szCurDir, "KG3DEngineD.dll");
#else
	strcat(szCurDir, "KG3DEngine.dll");
#endif
	g_3dEngineDll.Init(szCurDir);

	g_SetRootPath(BSTR_TO_STRING(strEnginePath)); // in Engine_lua5(D).dll
	lstrcpy(g_EnginePath, BSTR_TO_STRING(strEnginePath));
	SetCurrentDirectory(BSTR_TO_STRING(strEnginePath));
	lstrcpy(g_szDefWorkDirectory, g_EnginePath);

	HMODULE h = g_3dEngineDll.GetDllHandle();
	_ASSERTE(h);
	if (!h) return FALSE;
	
	if (lstrlen(g_EnginePath) <= 0)
	{
		HMODULE hDll = GetModuleHandle("atlax.dll");
		if (hDll)
		{
			TCHAR szBuf[MAX_PATH + 1];
			GetModuleFileName(hDll, szBuf, MAX_PATH);
			CString strTemp = szBuf;
			CString strDir = strTemp.Left(strTemp.ReverseFind('\\'));
			SetCurrentDirectory(strDir);
		}
	}
	else //
	{
		SetCurrentDirectory(g_EnginePath);
	}

	typedef HRESULT (*pfnGet3DEngineInterface)(void** pEngineManager);
	pfnGet3DEngineInterface Get3DEngineInterface = (pfnGet3DEngineInterface)GetProcAddress(h, "Get3DEngineInterface");
	_ASSERTE(Get3DEngineInterface);
	if (!Get3DEngineInterface) return FALSE;

	IEKG3DEngineManager* pInterface = NULL;
	HRESULT hret = Get3DEngineInterface((void**)&pInterface);
	if (FAILED(hret))
		return FALSE;
	else
	{
        if (FAILED(pInterface->Init(uMode, uFlag, hBaseWindow, hRenderWindow)))
        {
            return FALSE;
        }

        pInterface->SetAutoReloadTexture(TRUE);

		InitGlobalVars(pInterface);
		ATLTRACE("=================3DEngine Initialized!==================\r\n");
		return TRUE;
	}
}

void UnInit3DEngine()
{
	ATLTRACE("=====================3DEngine Uninitializing!================\r\n");
	g_3dEngineDll.UnInit();
	g_pEngineManager = NULL;
	ATLTRACE("#####################3DEngine Uninitialized!#################\r\n");
}
