#include "KG3DEngineController.h"
#include "IEEditor.h"

KG3DEngineController g_cKG3DEngineController;


KG3DEngineController::KG3DEngineController(void)
{
	m_hEngineDLL	= NULL;
	m_pEngineMgr	= NULL;
	m_pScene		= NULL;
	m_pIEOutDoorMgr = NULL;
}

KG3DEngineController::~KG3DEngineController(void)
{
	UnInit();
}

int KG3DEngineController::Init(const char cszClientPath[], const char cszDLLName[])
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hrRetCode = E_FAIL;
	HWND hWindow = NULL;

	KG_ASSERT_EXIT(cszClientPath);
	KG_PROCESS_ERROR(cszClientPath[0] != '\0');
	KG_ASSERT_EXIT(cszDLLName);
	KG_PROCESS_ERROR(cszDLLName[0] != '\0');

	g_SetRootPath(cszClientPath);

	m_hEngineDLL = LoadLibrary(cszDLLName);
	KGLOG_PROCESS_ERROR(m_hEngineDLL);

	hWindow = ::GetDesktopWindow();
	KGLOG_PROCESS_ERROR(hWindow);

	typedef HRESULT (*pfnGet3DEngineInterface)(void** ppvEngineManager);
	pfnGet3DEngineInterface Get3DEngineInterface;

	Get3DEngineInterface = (pfnGet3DEngineInterface)GetProcAddress(m_hEngineDLL, "Get3DEngineInterface");
	Get3DEngineInterface((void**)&m_pEngineMgr);
	KGLOG_PROCESS_ERROR(m_pEngineMgr);

	hrRetCode = m_pEngineMgr->Init(0, KG3DENG_CLIENT, hWindow, hWindow);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	nResult = true;
Exit0:
	if (!nResult)
	{
		UnInit();
	}
	return nResult;
}

void KG3DEngineController::UnInit()
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hrRetCode = E_FAIL;

	nRetCode = RemoveMap();
	KGLOG_CHECK_ERROR(nRetCode);
	if (m_pEngineMgr)
	{
		hrRetCode = m_pEngineMgr->UnInit();
		KGLOG_COM_CHECK_ERROR(hrRetCode);
		m_pEngineMgr = NULL;
	}
	if (m_hEngineDLL)
	{
		nRetCode = FreeLibrary(m_hEngineDLL);
		KGLOG_CHECK_ERROR(nRetCode);
		m_hEngineDLL = NULL;
	}

}

int KG3DEngineController::FormatPath(char* pszPath, const size_t cuSize)
{
	int nResult  = false;
	int nRetCode = false;
	char* pRetCode = NULL;
	size_t uPathLength = 0;

	KG_ASSERT_EXIT(pszPath);
	pszPath[cuSize - 1] = '\0';

	pRetCode = strchr(pszPath, '/');
	KGLOG_PROCESS_ERROR(pRetCode == NULL);

	uPathLength = strnlen(pszPath, cuSize);
	KGLOG_PROCESS_ERROR(uPathLength > 0);
	KGLOG_PROCESS_ERROR(uPathLength <= cuSize - 1);
	KGLOG_PROCESS_ERROR(uPathLength <= _MAX_DIR - 1);

	if (pszPath[uPathLength - 1] != '\\')
	{
		pszPath[uPathLength] = '\\';
		pszPath[uPathLength + 1] = '\0';
	}

	nResult = true;
Exit0:
	return nResult;
}

int KG3DEngineController::LoadMapInfo(const char cszMapName[])
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hrRetCode = E_FAIL;
	IEKG3DSceneSceneEditor*		pIEEditor     = NULL;

	KG_ASSERT_EXIT(cszMapName);
	KGLOG_PROCESS_ERROR(cszMapName[0] != '\0');

#if defined(DEBUG) | defined(_DEBUG)
	KG3D_GAME_INIT_CONTEXT Context;
	m_pEngineMgr->InitGameContext(&Context);
#endif
	hrRetCode = m_pEngineMgr->NewOneScene(SCENETYPE_SCENEEDITOR, &m_pScene);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	hrRetCode = ((IEKG3DScene*)m_pScene)->GetSceneSceneEditor(&pIEEditor);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	m_pIEOutDoorMgr = pIEEditor->GetIEKG3DSceneOutDoorSpaceMgr();
	KGLOG_PROCESS_ERROR(m_pIEOutDoorMgr);

	hrRetCode = m_pIEOutDoorMgr->LoadFromFile(cszMapName, 0, 0);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	nResult = true;
Exit0:
	return nResult;
}

int KG3DEngineController::RemoveMap()
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hrRetCode = E_FAIL;

	if (m_pScene)
	{
		hrRetCode = m_pEngineMgr->RemoveScene(m_pScene);
		KGLOG_COM_PROCESS_ERROR(hrRetCode);
		m_pScene = NULL;
		m_pIEOutDoorMgr = NULL;
	}

	nResult = true;
Exit0:
	return nResult;
}

HRESULT KG3DEngineController::GetAllTerrainInfoHandle(vector<UINT>* vecHandle)
{
	HRESULT hrRetCode = E_FAIL;
	HRESULT hrResult  = E_FAIL;

	KG_ASSERT_EXIT(vecHandle);

	if (m_pIEOutDoorMgr)
	{
		hrRetCode = m_pIEOutDoorMgr->GetAllTerrainInfoHandle(vecHandle);
		KGLOG_COM_PROCESS_ERROR(hrRetCode);
	}

	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KG3DEngineController::GetAllEntityInfoHandle(vector<UINT>* vecHandle)
{
	HRESULT hrRetCode = E_FAIL;
	HRESULT hrResult  = E_FAIL;

	KG_ASSERT_EXIT(vecHandle);

	if (m_pIEOutDoorMgr)
	{
		hrRetCode = m_pIEOutDoorMgr->GetAllEntityInfoHandle(vecHandle);
		KGLOG_COM_PROCESS_ERROR(hrRetCode);
	}

	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KG3DEngineController::GetEntityInfoformation(UINT uEntityInfoHandle,DWORD* pdwDataType,DWORD* pdwLength,PVOID pData,DWORD dwMaxLen)
{
	HRESULT hrRetCode = E_FAIL;
	HRESULT hrResult  = E_FAIL;

	KG_ASSERT_EXIT(pdwDataType);
	KG_ASSERT_EXIT(pdwLength);
	KG_ASSERT_EXIT(pdwLength);
	KG_ASSERT_EXIT(pData);

	if (m_pIEOutDoorMgr)
	{
		hrRetCode = m_pIEOutDoorMgr->GetEntityInfoformation(uEntityInfoHandle, pdwDataType, pdwLength, pData, dwMaxLen);
		KGLOG_COM_PROCESS_ERROR(hrRetCode);
	}

	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KG3DEngineController::GetTerrainInfoformation(UINT uEntityInfoHandle,DWORD* pdwDataType,DWORD* pdwLength,PVOID pData)
{
	HRESULT hrRetCode = E_FAIL;
	HRESULT hrResult  = E_FAIL;

	KG_ASSERT_EXIT(pdwDataType);
	KG_ASSERT_EXIT(pdwLength);
	KG_ASSERT_EXIT(pdwLength);
	KG_ASSERT_EXIT(pData);

	if (m_pIEOutDoorMgr)
	{
		hrRetCode = m_pIEOutDoorMgr->GetTerrainInfoformation(uEntityInfoHandle, pdwDataType, pdwLength, pData);
		KGLOG_COM_PROCESS_ERROR(hrRetCode);
	}

	hrResult = S_OK;
Exit0:
	return hrResult;
}