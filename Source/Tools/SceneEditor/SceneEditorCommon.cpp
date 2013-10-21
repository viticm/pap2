#include "StdAfx.h"
#include "SceneEditorCommon.h"

#include "..\..\KG3DEngine\KG3DEngine\KG3DEngine.h"
#include "..\..\KG3DEngine\KG3DEngine\KG3DTemplates.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


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
	IKG3DRTTITypeManager* g_pKG3DRTTITypeManager = NULL;///<RTTI_INTERFACE>
	IKG3DEngineManager* g_GetEngineManager()
	{
		return dynamic_cast<IKG3DEngineManager*>(g_GetEngineIEInterface());
	}

#else
	IEKG3DEngineManager* g_pEngineManager = NULL;
	IEKG3DGraphicsTool* g_pGraphicsTool = NULL;
	IKG3DRTTITypeManager* g_pKG3DRTTITypeManager = NULL;///<RTTI_INTERFACE>
	IKG3DEngineManager* g_GetEngineManager()
	{
		return dynamic_cast<IKG3DEngineManager*>(g_pEngineManager);
	}

#endif // _DEBUG


IKG3DResourceManager* g_GetObjManager()
{
	IKG3DResourceManager* pTable = NULL;
	g_pEngineManager->GetRepresentObjectTable(&pTable);
	_ASSERTE(pTable);
	return pTable;
}

IKG3DResourceManager* g_GetModelManager()
{
    IKG3DResourceManager* pTable = g_pEngineManager->Get3DModelTable();
    return pTable;
}

TCHAR g_szDefWorkDirectory[256];//工作目录
TCHAR g_szDefExeDirectory[MAX_PATH];

LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
BYTE*     g_SceneEditorKeyState = NULL;

//TCHAR g_strWeaponDirectory[MAX_PATH];
//TCHAR g_strItemDirectory[MAX_PATH];


void InitGlobalVars(IEKG3DEngineManager* pInterface)
{
	g_pEngineManager = pInterface;
	g_pEngineManager->GetGraphicsTool(&g_pGraphicsTool);
	g_pEngineManager->GetDefWorkDirectory(g_szDefWorkDirectory, sizeof(g_szDefWorkDirectory));
	g_pEngineManager->GetD3DDevice(&g_pd3dDevice);
	g_pEngineManager->GetKeyStateBuffer(&g_SceneEditorKeyState);
	g_pEngineManager->GetKG3DRTTITypeManager(&g_pKG3DRTTITypeManager);///<RTTI_INTERFACE>
	_ASSERTE(g_pKG3DRTTITypeManager);

	//g_pEngineManager->GetWeaponDirectory(g_strWeaponDirectory, MAX_PATH);
	//g_pEngineManager->GetItemDirectory(g_strItemDirectory, MAX_PATH);
}

BOOL Init3DEngine(unsigned uMode, unsigned int uFlag, HWND hBaseWindow, HWND hRenderWindow)
{
	ATLTRACE("#####################3DEngine Initializing!##################\r\n");
#ifdef _DEBUG
	HRESULT hr = g_3dEngineDll.Init("KG3DEngineD.dll");
	if(FAILED(hr))
		return FALSE;
#else
	HRESULT hr = g_3dEngineDll.Init("KG3DEngine.dll");
	if(FAILED(hr))
		return FALSE;
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
	if(NULL != g_pEngineManager)
		g_pEngineManager->UnInit();
	g_3dEngineDll.UnInit();
	g_pEngineManager = NULL;
	ATLTRACE("#####################3DEngine Uninitialized!#################\r\n");
}

HRESULT CDllHolder::Init( LPCTSTR lpstrDllName )
{
	_ASSERTE(NULL != lpstrDllName);
	TCHAR dir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,dir);

	wsprintf(dir,"%s%s",g_szDefExeDirectory,lpstrDllName);

	m_hMod = LoadLibrary(dir);
	if(! m_hMod)
	{
		std::tstring strTemp = _T("动态库加载失败");
		strTemp += dir;
		strTemp += _T("\n");
		::OutputDebugString(strTemp.c_str());
		KGLogPrintf(KGLOG_ERR, "%s", strTemp.c_str());
		AfxMessageBox(strTemp.c_str());
		return E_FAIL;
	}

	if (m_hMod)
		m_strDllName = lpstrDllName;
	return S_OK;
}

void CDllHolder::UnInit()
{
	if (m_hMod)
	{
		FreeLibrary(m_hMod);
		m_hMod = NULL;

#if defined(DEBUG) | defined(_DEBUG)
		std::tstring strOut = _T("以下DLL被正确释放:");
		strOut += m_strDllName;
		::OutputDebugString(strOut.c_str());
#endif
	}
}