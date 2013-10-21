#include "KG3DEngineHelper.h"
#include "IEEditor.h"
#include "IEEditorTypes.h"
#include "IEKG3DScene.h"
#include "IEOther.h"

const char RESOURCE_TYPE[]  = ".mesh";
const float  DEFAULT_MAP_SIZE = 12800.0f;					//LoadDefaultTerrain的默认地图大小

KG3DEngineHelper::KG3DEngineHelper(void)
{
	m_hEngineDLL = NULL;
	m_pEngineMgr = NULL;
	m_szClientPath[0] = '\0';
	m_vecResNameList.clear();
}

KG3DEngineHelper::~KG3DEngineHelper(void)
{
}

int KG3DEngineHelper::LoadEngineDLL()
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hrRetCode = E_FAIL;
	HWND hWindow = NULL;
	char szEngineDll[MAX_PATH] = {0};

	nRetCode = UnLoadEngineDLL();
	KGLOG_PROCESS_ERROR(nRetCode);

	g_SetRootPath(m_szClientPath);

	nRetCode = _snprintf_s(
		szEngineDll,
		sizeof(szEngineDll),
		sizeof(szEngineDll) - 1,        
		"%s%s",
		g_szApplicationPath,
		ENGINE_DLL_NAME
		);
	KGLOG_PROCESS_ERROR(nRetCode > 0);

	nRetCode = g_IsFileExist(szEngineDll);
	if (!nRetCode)
	{
		nRetCode = _snprintf_s(
			szEngineDll,
			sizeof(szEngineDll),
			sizeof(szEngineDll) - 1,        
			"%s%s",
			m_szClientPath,
			ENGINE_DLL_NAME
			);
		KGLOG_PROCESS_ERROR(nRetCode > 0);
	}

	m_hEngineDLL = LoadLibrary(szEngineDll);
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
		if (m_pEngineMgr)
		{
			hrRetCode = m_pEngineMgr->UnInit();
			KGLOG_COM_CHECK_ERROR(hrRetCode);
			m_pEngineMgr = NULL;
		}
		if (m_hEngineDLL)
		{
			nRetCode = FreeLibrary(m_hEngineDLL);
			KGLOG_CHECK_ERROR(hrRetCode);
			m_hEngineDLL = NULL;
		}
	}
	return nResult;
}

int KG3DEngineHelper::UnLoadEngineDLL()
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hrRetCode = E_FAIL;

	if (m_pEngineMgr)
	{
		hrRetCode = m_pEngineMgr->UnInit();
		KGLOG_COM_PROCESS_ERROR(hrRetCode);
		m_pEngineMgr = NULL;
	}
	if (m_hEngineDLL)
	{
		nRetCode = FreeLibrary(m_hEngineDLL);
		KGLOG_PROCESS_ERROR(nRetCode);
		m_hEngineDLL = NULL;
	}

	nResult = true;
Exit0:
	return nResult;
}

int KG3DEngineHelper::CreateMap(IKG3DScene** pOutScene, const char cszResourcePath[], int nModelToCreate)
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hrRetCode = E_FAIL;
	IKG3DScene*				pScene		= NULL;
	IEKG3DSceneSceneEditor*	pIEEditor	= NULL;
	int nWinId = 0;
	KG3DTERRAIN_LOAD_PARAM TerrainParam;

	KG_ASSERT_EXIT(pOutScene);

	KGTestCommon::OutInfo("·Creating New Map");

#if defined(DEBUG) | defined(_DEBUG)
	KG3D_GAME_INIT_CONTEXT Context;
	m_pEngineMgr->InitGameContext(&Context);
#endif
	hrRetCode = m_pEngineMgr->NewOneScene(SCENETYPE_SCENEEDITOR, &pScene);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	HWND hWindow = ::GetDesktopWindow();
	KGLOG_PROCESS_ERROR(hWindow);

	hrRetCode = ((IEKG3DScene*)pScene)->GetSceneSceneEditor(&pIEEditor);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	hrRetCode = pIEEditor->AddOutputWindow("New Scene", hWindow, OUTPUTWND_SCENEEDITOR_MAIN, 0, &nWinId);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	hrRetCode = pIEEditor->LoadDefaultTerrain(&TerrainParam);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	{//随机添加模型
		int nModelNum = 0;
		int nModelNumInLine = 0;
		float fCellSize = 0.0f;
		char szResourcePath[MAX_PATH] = {0};

		KGTestCommon::OutInfo("·Add Model(Rendom)");

		if (m_vecResNameList.empty())
		{
			nRetCode = _snprintf_s(
				szResourcePath,
				sizeof(szResourcePath),
				sizeof(szResourcePath) - 1,        
				"%s%s",
				m_szClientPath,
				cszResourcePath
				);
			KGLOG_PROCESS_ERROR(nRetCode > 0);
			nRetCode = KGFileManager::Instance()->SearchFiles(szResourcePath, &m_vecResNameList, RESOURCE_TYPE);
			KGLOG_PROCESS_ERROR(nRetCode);
		}
		//打乱模型资源路径信息
		random_shuffle(m_vecResNameList.begin(), m_vecResNameList.end());

		//向场景添加模型
		nModelNumInLine = (int)ceil(sqrt((double)nModelToCreate));
		fCellSize = DEFAULT_MAP_SIZE / (nModelNumInLine + 1);

		for (int i = 0; i < nModelToCreate; i++)
		{
			IEKG3DRepresentObject* pObject    = NULL;
			IEKG3DRepresentObject* pNewObject = NULL;
			float fPosX = fCellSize * (i % nModelNumInLine + 1);
			float fPosZ = fCellSize * (i / nModelNumInLine + 1);

			hrRetCode = ((IEKG3DEngineManager*)m_pEngineMgr)->NewKG3DRepresentObject(
				REPRESENTOBJECT_DEFAULT, 0, 0, &pObject);
			KGLOG_COM_PROCESS_ERROR(hrRetCode);

			hrRetCode = pObject->SetModel(0, m_vecResNameList[i].c_str(), "", "", true);
			KGLOG_COM_PROCESS_ERROR(hrRetCode);

			hrRetCode = pObject->SetTranslation(&D3DXVECTOR3(fPosX, 0, fPosZ));
			KGLOG_COM_PROCESS_ERROR(hrRetCode);

			hrRetCode = pIEEditor->AddRepresentObject(pObject, EM_SCENE_ADD_OPTION_TO_FILE, &pNewObject);
			KGLOG_COM_PROCESS_ERROR(hrRetCode);
			//EM_SCENE_ADD_OPTION_TO_FILE表示希望将Object保存到文件，内部会复制一个新的Object放到场景里。因此可以删除原来创建的Object
			SAFE_RELEASE(pObject);
		}
	}
	*pOutScene = pScene;
	KGTestCommon::OutInfo("·Map Created");

	nResult = true;
Exit0:
	return nResult;
}

int KG3DEngineHelper::LoadMapInfo(const char cszMapName[], IKG3DScene** pOutScene)
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hrRetCode = E_FAIL;
	IEKG3DSceneSceneEditor*		pIEEditor     = NULL;
	IEKG3DSceneOutDoorSpaceMgr* pIEOutDoorMgr = NULL;
	IKG3DScene*					pLoadedScene  = NULL;

	KG_ASSERT_EXIT(cszMapName);
	KG_ASSERT_EXIT(pOutScene);
	KGLOG_PROCESS_ERROR(cszMapName[0] != '\0');

	KGTestCommon::OutInfo(("·Loading Map Info: " + string(cszMapName)).c_str());

#if defined(DEBUG) | defined(_DEBUG)
	KG3D_GAME_INIT_CONTEXT Context;
	m_pEngineMgr->InitGameContext(&Context);
#endif
	hrRetCode = m_pEngineMgr->NewOneScene(SCENETYPE_SCENEEDITOR, &pLoadedScene);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	hrRetCode = ((IEKG3DScene*)pLoadedScene)->GetSceneSceneEditor(&pIEEditor);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	pIEOutDoorMgr = pIEEditor->GetIEKG3DSceneOutDoorSpaceMgr();
	KGLOG_PROCESS_ERROR(pIEOutDoorMgr);

	hrRetCode = pIEOutDoorMgr->LoadFromFile(cszMapName, 0, 0);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	*pOutScene = pLoadedScene;
	KGTestCommon::OutInfo("·Map Info Loaded");

	nResult = true;
Exit0:
	return nResult;
}

int KG3DEngineHelper::LoadMapData(const char cszMapName[], IKG3DScene** pOutScene)
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hrRetCode = E_FAIL;
	IKG3DScene*				pLoadedScene = NULL;
	IEKG3DSceneSceneEditor*	pIEEditor	 = NULL;
	int nWinId = 0;
	HWND hWindow = NULL;
	KG3DSCENE_INITSTRUCT TempStruct;

	KG_ASSERT_EXIT(cszMapName);
	KG_ASSERT_EXIT(pOutScene);
	KGLOG_PROCESS_ERROR(cszMapName[0] != '\0');

	KGTestCommon::OutInfo(("·Loading Map Data: " + string(cszMapName)).c_str());

#if defined(DEBUG) | defined(_DEBUG)
	KG3D_GAME_INIT_CONTEXT Context;
	m_pEngineMgr->InitGameContext(&Context);
#endif
	hrRetCode = m_pEngineMgr->NewOneScene(SCENETYPE_SCENEEDITOR, &pLoadedScene);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	hWindow = ::GetDesktopWindow();
	KGLOG_PROCESS_ERROR(hWindow);

	hrRetCode = ((IEKG3DScene*)pLoadedScene)->GetSceneSceneEditor(&pIEEditor);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	hrRetCode = pIEEditor->AddOutputWindow("Scene", hWindow, 0, 0, &nWinId);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	hrRetCode = pLoadedScene->LoadFromFileEx(cszMapName, 0, 0, reinterpret_cast<DWORD_PTR>(&TempStruct), 0);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	*pOutScene = pLoadedScene;
	KGTestCommon::OutInfo("·Map Data Loaded");

	nResult = true;
Exit0:
	return nResult;
}

int KG3DEngineHelper::SaveMap(const char cszMapName[], IKG3DScene* pOutScene)
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hrRetCode = E_FAIL;
	char* pszResult = NULL;
	char szPath[MAX_PATH] = {0};

	KG_ASSERT_EXIT(cszMapName);
	KG_ASSERT_EXIT(pOutScene);

	KGTestCommon::OutInfo(("·Saving Map: " + string(cszMapName)).c_str());

	nRetCode = strncpy_s(szPath, sizeof(szPath) - 1, cszMapName, strlen(cszMapName));
	KGLOG_PROCESS_ERROR(nRetCode == 0);
	pszResult = strrchr(szPath, '\\');
	KGLOG_PROCESS_ERROR(pszResult);
	*(pszResult + 1) = '\0';
	pszResult = NULL;

	nRetCode = g_CreatePath(szPath);
	KGLOG_PROCESS_ERROR(nRetCode);

	hrRetCode = pOutScene->SaveToFile(cszMapName, EM_SAVE_OP_ALL);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	KGTestCommon::OutInfo("·Map Saved");

	nResult = true;
Exit0:
	return nResult;
}

int KG3DEngineHelper::RemoveScene(IKG3DScene* pScene)
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hrRetCode = E_FAIL;

	KG_ASSERT_EXIT(pScene);
	hrRetCode = m_pEngineMgr->RemoveScene(pScene);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	nResult = true;
Exit0:
	return nResult;
}

int KG3DEngineHelper::Init(const char cszClientPath[])
{
	int nResult  = false;
	int nRetCode = false;

	KG_ASSERT_EXIT(cszClientPath);
	KG_ASSERT_EXIT(cszClientPath[0] != '\0');

	nRetCode = strncpy_s(m_szClientPath, sizeof(m_szClientPath) - 1, cszClientPath, strlen(cszClientPath));
	KGLOG_PROCESS_ERROR(nRetCode == 0);

	nResult = true;
Exit0:
	return nResult;
}
int KG3DEngineHelper::UnInit()
{
	int nResult  = false;
	int nRetCode = false;

	nRetCode = UnLoadEngineDLL();
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}