#include "StdAfx.h"
#include "SceneEditorCommon.h"

#include "KG3DEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


class CDllHolder
{
public:
	CDllHolder()
	{
		m_hMod = NULL;
	}
	void Init(LPCTSTR lpstrDllName)
	{
		m_hMod = LoadLibrary(lpstrDllName);
		_ASSERTE(m_hMod);
	}
	void UnInit()
	{
		if (m_hMod)
		{
			FreeLibrary(m_hMod);
			m_hMod = NULL;
		}
	}

	~CDllHolder()
	{
		UnInit();
	}
	HMODULE GetDllHandle()
	{
		return m_hMod;
	}
private:
	HMODULE m_hMod;
};

CDllHolder g_3dEngineDll;

#ifdef _DEBUG
	CEngineManagerHelper g_pEngineManager;

	void CEngineManagerHelper::operator = (IEKG3DEngineManager* p)
	{
		//_ASSERTE(p);
		m_pEngineManager = p;
	}

	IEKG3DEngineManager* CEngineManagerHelper::GetInterface()
	{
		return m_pEngineManager;
	}


	IEKG3DGraphicsTool* g_pGraphicsTool = NULL;
	IKG3DEngineManager* g_GetEngineManager()
	{
		return dynamic_cast<IKG3DEngineManager*>(g_pEngineManager.GetInterface());
	}

#else
	IEKG3DEngineManager* g_pEngineManager = NULL;
	IEKG3DGraphicsTool* g_pGraphicsTool = NULL;
	IKG3DEngineManager* g_GetEngineManager()
	{
		return dynamic_cast<IKG3DEngineManager*>(g_pEngineManager);
	}

#endif // _DEBUG


IKG3DResourceManager* g_GetModelManager()
{
	IKG3DResourceManager* pTable = NULL;
	g_pEngineManager->GetRepresentObjectTable(&pTable);
	_ASSERTE(pTable);
	return pTable;
}


TCHAR g_szDefAppDirectory[256];
LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
BYTE*     g_SceneEditorKeyState = NULL;

//TCHAR g_strWeaponDirectory[MAX_PATH];
//TCHAR g_strItemDirectory[MAX_PATH];


void InitGlobalVars(IEKG3DEngineManager* pInterface)
{
	g_pEngineManager = pInterface;
	g_pEngineManager->GetGraphicsTool(&g_pGraphicsTool);
	g_pEngineManager->GetDefAppDirectory(g_szDefAppDirectory, sizeof(g_szDefAppDirectory));
	g_pEngineManager->GetD3DDevice(&g_pd3dDevice);
	g_pEngineManager->GetKeyStateBuffer(&g_SceneEditorKeyState);

	//g_pEngineManager->GetWeaponDirectory(g_strWeaponDirectory, MAX_PATH);
	//g_pEngineManager->GetItemDirectory(g_strItemDirectory, MAX_PATH);
}

BOOL Init3DEngine(unsigned uMode, unsigned int uFlag, HWND hBaseWindow, HWND hRenderWindow)
{
	ATLTRACE("#####################3DEngine Initializing!##################\r\n");
	TCHAR szBuf[MAX_PATH + 1];

	GetModuleFileName(GetModuleHandle("atlax.dll"), szBuf, MAX_PATH);
#ifdef _DEBUG
	SetCurrentDirectory("E:\\work\\Sword3\\Sword3-Products\\trunk\\client\\");
	g_3dEngineDll.Init("KG3DEngineD.dll");
#else
	g_3dEngineDll.Init("KG3DEngine.dll");
#endif
	typedef HRESULT (*pfnGet3DEngineInterface)(void** pEngineManager);

	HMODULE h = g_3dEngineDll.GetDllHandle();
	_ASSERTE(h);
	if (!h) return FALSE;

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
