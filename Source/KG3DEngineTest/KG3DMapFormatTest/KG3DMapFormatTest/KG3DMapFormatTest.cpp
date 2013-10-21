#include "KG3DMapFormatTest.h"
#include "IEEditor.h"
#include "IEEditorTypes.h"
#include "IEKG3DScene.h"
#include "IEOther.h"
#include "KMemory.h"

//加载并提供调用旧引擎接口的DLL
#ifdef _DEBUG
const char OLD_ENGINE_CTL_NAME[] = "KGOldEngineControllerD.dll";
#else
const char OLD_ENGINE_CTL_NAME[] = "KGOldEngineController.dll";
#endif

const char CASE_SETTING_FILE[]	= "CaseSetting.ini";	//用例配置文件

const char CASE_PASS[]		= "【Case Pass】";
const char CASE_FAILED[]	= "【Case Failed】";
const char TEXT_TEST_MAP[]= "TestMap: ";

//config.ini相关
const char CONFIG_SECTION[] = "ENGINEOPTION";
const char CONFIG_KEY[]		= "nPackFile";
const char CONFIG_FILE[]	= "config.ini";
//对应nPackFile的值
const int  PACKFILE_0 = 0;
const int  PACKFILE_1 = 1;
const int  PACKFILE_2 = 2;

const char TEST_MAP_NAME[]  = "Test.map";
const int  EM_BUFFER_SIZE	= 1024 * 1024 * 4;			//读取地图数据的缓冲大小，参照KG3DMemoryFile

//新格式地图名称列表（现在内网未有新格式地图，此地图是手动创建用于测试）
const int    MAX_MAPNAME_LENGTH	= 20;					//地图名称长度
const char	 NEW_MAP_NAME[][MAX_MAPNAME_LENGTH] = 
{
	"NewFormatMap"
};

bool KG3DMapFormatTest::m_bTransformNew;

KG3DMapFormatTest::KG3DMapFormatTest(void)
{
	ZeroMemory(&m_CaseSetting, sizeof(CaseSetting));
	m_bKMemoryInited = false;
	m_bNeedToDelKey  = false;
	m_bTransformNew  = false;

	m_szConfigFullName[0]	= '\0';
	m_szClientPath[0]		= '\0';
	m_szTestMapFullName[0]	= '\0';
	m_vecOldMapList.clear();
	m_vecNewMapList.clear();
	m_vecTestResult.clear();

	m_pWorkThread = NULL;
}

KG3DMapFormatTest::~KG3DMapFormatTest(void)
{
	UnInit();
}

int KG3DMapFormatTest::Init(const char cszClientPath[])
{
	int nRetCode = false;
	int nResult  = false;
	char szValue[MAX_PATH] = {0};

	KGTestCommon::OutInfo("Init Testing");
	if (cszClientPath == NULL)
	{
		nRetCode = strncpy_s(m_szClientPath, sizeof(m_szClientPath) - 1, g_szApplicationPath, strlen(g_szApplicationPath));
		KGLOG_PROCESS_ERROR(nRetCode == 0);
	}
	else
	{
		nRetCode = strncpy_s(m_szClientPath, sizeof(m_szClientPath) - 1, cszClientPath, strlen(cszClientPath));
		KGLOG_PROCESS_ERROR(nRetCode == 0);
	}

	nRetCode = KGTestCommon::FormatPath(m_szClientPath, sizeof(m_szClientPath));
	KGLOG_PROCESS_ERROR(nRetCode);
	
	//初始化配置文件
	nRetCode = _snprintf_s(
		m_szConfigFullName,
		sizeof(m_szConfigFullName),
		sizeof(m_szConfigFullName) - 1,        
		"%s%s",
		g_szApplicationPath,
		CONFIG_FILE
		);
	KGLOG_PROCESS_ERROR(nRetCode > 0);
	nRetCode = GetPrivateProfileString(CONFIG_SECTION, CONFIG_KEY, "", szValue, sizeof(szValue), m_szConfigFullName);
	if(!nRetCode)
	{
		nRetCode = WritePrivateProfileString(CONFIG_SECTION, CONFIG_KEY, "0", m_szConfigFullName);
		KGLOG_PROCESS_ERROR(nRetCode);
		m_bNeedToDelKey = true;
	}
	//构造测试地图场景文件名
	nRetCode = _snprintf_s(
		m_szTestMapFullName,
		sizeof(m_szTestMapFullName),
		sizeof(m_szTestMapFullName) - 1,        
		"%sTemp\\%s",
		g_szApplicationPath,
		TEST_MAP_NAME
		);
	KGLOG_PROCESS_ERROR(nRetCode > 0);
	//初始化内存管理
	if (!m_bKMemoryInited)
	{
		nRetCode = KMemory::Initialize("KG3DMapFormatTest.memory");	
		KGLOG_PROCESS_ERROR(nRetCode);
		m_bKMemoryInited = true;
	}

	//读取用例配置
	nRetCode = LoadCaseSetting();
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_EngineHelper.Init(m_szClientPath);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	if (!nResult)
	{
		UnInit();
	}
	return nResult;
}

void KG3DMapFormatTest::UnInit()
{
	int nRetCode = false;

	nRetCode = m_EngineHelper.UnInit();
	KGLOG_CHECK_ERROR(nRetCode);

	if (m_bNeedToDelKey)
	{
		nRetCode = WritePrivateProfileString(CONFIG_SECTION, CONFIG_KEY, NULL, m_szConfigFullName);
		KGLOG_CHECK_ERROR(nRetCode);
		m_bNeedToDelKey = false;
	}
	if (m_bKMemoryInited)
	{
		KMemory::Finalize();
		m_bKMemoryInited = false;
	}
}

int KG3DMapFormatTest::CompareMapData(IKG3DScene* pScene_1, IKG3DScene* pScene_2)
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hrRetCode = E_FAIL;
	IEKG3DSceneSceneEditor*		pIEEditor_1		= NULL;
	IEKG3DSceneSceneEditor*		pIEEditor_2		= NULL;
	IEKG3DSceneOutDoorSpaceMgr* pOutDoorMgr_1	= NULL;
	IEKG3DSceneOutDoorSpaceMgr* pOutDoorMgr_2	= NULL;

	KG_ASSERT_EXIT(pScene_1);
	KG_ASSERT_EXIT(pScene_2);

	hrRetCode = ((IEKG3DScene*)pScene_1)->GetSceneSceneEditor(&pIEEditor_1);
	KG_COM_PROCESS_ERROR(hrRetCode);

	pOutDoorMgr_1 = pIEEditor_1->GetIEKG3DSceneOutDoorSpaceMgr();
	KG_PROCESS_ERROR(pOutDoorMgr_1);

	hrRetCode = ((IEKG3DScene*)pScene_2)->GetSceneSceneEditor(&pIEEditor_2);
	KG_COM_PROCESS_ERROR(hrRetCode);

	pOutDoorMgr_2 = pIEEditor_2->GetIEKG3DSceneOutDoorSpaceMgr();
	KG_PROCESS_ERROR(pOutDoorMgr_2);

	nRetCode = CompareEntityInfo(pOutDoorMgr_1, pOutDoorMgr_2);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = CompareTerrainInfo(pOutDoorMgr_1, pOutDoorMgr_2);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = CompareSceneBlockInfo(pOutDoorMgr_1, pOutDoorMgr_2);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = CompareEntityData(pOutDoorMgr_1, pOutDoorMgr_2);
	KG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

int KG3DMapFormatTest::ValidateLoadOldMapByDifferEngine(const char cszMapName[])
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hrRetCode = E_FAIL;
	HMODULE					hOldEngineCtlerDll	= NULL;
	IKG3DEngineController*	pIController		= NULL;
	IKG3DScene*				pOldEngineScene		= NULL;
	IKG3DScene*				pNewEngineScene		= NULL;
	int nMapNameIndex = 0;
	char szMapFullName[MAX_PATH]  = {0};
	char szOldEngineDll[MAX_PATH] = {0};
	CaseInfo caseInfo;

	KGTestCommon::OutInfo("【ValidateLoadOldMapByDifferEngine】");
	caseInfo.strCaseName = "【ValidateLoadOldMapByDifferEngine】";
	caseInfo.strCaseInfo = string(TEXT_TEST_MAP) + string(cszMapName);

	//加载旧的引擎的封装DLL，以便使用旧引擎接口获取数据
	nRetCode = _snprintf_s(
		szOldEngineDll,
		sizeof(szOldEngineDll),
		sizeof(szOldEngineDll) - 1,        
		"%sOldEngineDll\\%s",
		g_szApplicationPath,
		ENGINE_DLL_NAME
		);
	KGLOG_PROCESS_ERROR(nRetCode > 0);

 	hOldEngineCtlerDll = LoadLibrary(OLD_ENGINE_CTL_NAME);
 	KGLOG_PROCESS_ERROR(hOldEngineCtlerDll);

 	typedef HRESULT (*pfnGet3DEngineController)(void** ppvEngineController);
 	pfnGet3DEngineController Get3DEngineController;

	Get3DEngineController = (pfnGet3DEngineController)GetProcAddress(hOldEngineCtlerDll, "GetController");
	Get3DEngineController((void**)&pIController);
 	KGLOG_PROCESS_ERROR(pIController);
 
 	nRetCode = pIController->Init(m_szClientPath, szOldEngineDll);
 	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = _snprintf_s(
		szMapFullName,
		sizeof(szMapFullName),
		sizeof(szMapFullName) - 1,        
		"%sdata\\source\\maps\\%s\\%s.map",
		m_szClientPath,
		cszMapName,
		cszMapName
		);
	KGLOG_PROCESS_ERROR(nRetCode > 0);

	//加载新引擎
	nRetCode = m_EngineHelper.LoadEngineDLL();
	KGLOG_PROCESS_ERROR(nRetCode);

	//新引擎加载旧格式地图场景信息
	nRetCode = m_EngineHelper.LoadMapInfo(szMapFullName, &pNewEngineScene);
	KGLOG_PROCESS_ERROR(nRetCode);

	//旧引擎加载旧格式地图场景信息
	nRetCode = pIController->LoadMapInfo(szMapFullName);
	KGLOG_PROCESS_ERROR(nRetCode);

	KGTestCommon::OutInfo("·Loaded map by Old engine");

	//比较地图场景信息的handle
	nRetCode = CompareDifferEngineMapData(pNewEngineScene, pIController);
	KGLOG_PROCESS_ERROR(nRetCode);

	KGTestCommon::OutInfo(CASE_PASS);
	caseInfo.strCaseResult = CASE_PASS;

	nResult = true;
Exit0:
	if (!nResult)
	{
		KGTestCommon::OutInfo(CASE_FAILED);
		caseInfo.strCaseResult = CASE_FAILED;
	}
	m_vecTestResult.push_back(caseInfo);
	if (pIController)
	{
		pIController->UnInit();
		pIController = NULL;
	}
	if (hOldEngineCtlerDll)
	{
		nRetCode = FreeLibrary(hOldEngineCtlerDll);
		KGLOG_CHECK_ERROR(nRetCode);
		hOldEngineCtlerDll = NULL;
	}
	if (pNewEngineScene)
	{
		nRetCode = m_EngineHelper.RemoveScene(pNewEngineScene);
		KGLOG_CHECK_ERROR(nRetCode);
		pNewEngineScene = NULL;
	}
	nRetCode = m_EngineHelper.UnLoadEngineDLL();
	KGLOG_CHECK_ERROR(nRetCode);
	return nResult;
}

int KG3DMapFormatTest::ValidateSaveCreatedMap(int nPackFileSign)
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hrRetCode = E_FAIL;
	IKG3DScene* pCreatedScene = NULL;
	IKG3DScene* pSavedScene   = NULL;
	string strValue;
	string strCaseName;
	char szOldValue[MAX_PATH] = {0};
	bool bGotValue = false;
	int nModelToCreate = 0;
	CaseInfo caseInfo;

	switch(nPackFileSign)
	{
		case PACKFILE_0:
		{
			strValue = "0";
			strCaseName = "【ValidateSaveCreatedMap(Create Old Format Map)】"; 
		}
		break;
		case PACKFILE_1:
		{
			strValue = "1";
			strCaseName = "【ValidateSaveCreatedMap(Create New Format Map)】"; 
		}
		break;
		default:
			KGLOG_PROCESS_ERROR(false);
	}

	KGTestCommon::OutInfo(strCaseName.c_str());
	caseInfo.strCaseName = strCaseName;

	//准备测试环境
	nRetCode = GetPrivateProfileString(CONFIG_SECTION, CONFIG_KEY, "", szOldValue, sizeof(szOldValue), m_szConfigFullName);
	KGLOG_PROCESS_ERROR(nRetCode);
	bGotValue = true;
	nRetCode = WritePrivateProfileString(CONFIG_SECTION, CONFIG_KEY, strValue.c_str(), m_szConfigFullName);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_EngineHelper.LoadEngineDLL();
	KGLOG_PROCESS_ERROR(nRetCode);

	//创建
	nModelToCreate = KGTestCommon::GetRandom(m_CaseSetting.nMinModelToCreate, m_CaseSetting.nMaxModelToCreate);
	caseInfo.strCaseInfo = "Add Model Num: " + KGTestCommon::IntToString(nModelToCreate);
	nRetCode = m_EngineHelper.CreateMap(&pCreatedScene, m_CaseSetting.szResourcePath, nModelToCreate);
	KGLOG_PROCESS_ERROR(nRetCode);
	//保存
	nRetCode = m_EngineHelper.SaveMap(m_szTestMapFullName, pCreatedScene);
	KGLOG_PROCESS_ERROR(nRetCode);
	//加载保存后的地图场景
	nRetCode = m_EngineHelper.LoadMapInfo(m_szTestMapFullName, &pSavedScene);
	KGLOG_PROCESS_ERROR(nRetCode);
	//验证新格式的文件
	if (nPackFileSign == PACKFILE_1)
	{
		nRetCode = CheckNewFormatFile(m_szTestMapFullName);
		KGLOG_PROCESS_ERROR(nRetCode);
	}
	//比较场景数据
	nRetCode = CompareMapData(pCreatedScene, pSavedScene);
	KGLOG_PROCESS_ERROR(nRetCode);

	KGTestCommon::OutInfo(CASE_PASS);
	caseInfo.strCaseResult = CASE_PASS;

	nResult = true;
Exit0:
	if (!nResult)
	{
		KGTestCommon::OutInfo(CASE_FAILED);
		caseInfo.strCaseResult = CASE_FAILED;
	}
	m_vecTestResult.push_back(caseInfo);
	if (pCreatedScene)
	{
		nRetCode = m_EngineHelper.RemoveScene(pCreatedScene);
		KGLOG_CHECK_ERROR(nRetCode);
		pCreatedScene = NULL;
	}
	if (pSavedScene)
	{
		nRetCode = m_EngineHelper.RemoveScene(pSavedScene);
		KGLOG_CHECK_ERROR(nRetCode);
		pSavedScene = NULL;
	}
	nRetCode = m_EngineHelper.UnLoadEngineDLL();
	KGLOG_CHECK_ERROR(nRetCode);
	nRetCode = CleanSaveMap(m_szTestMapFullName);
	KGLOG_CHECK_ERROR(nRetCode);
	if (bGotValue)
	{
		nRetCode = WritePrivateProfileString(CONFIG_SECTION, CONFIG_KEY, szOldValue, m_szConfigFullName);
		KGLOG_CHECK_ERROR(nRetCode);
	}
	return nResult;
}

int KG3DMapFormatTest::Run()
{
	int nResult  = false;
	int nRetCode = false;
	int nMapNameIndex = 0;

	if (m_CaseSetting.nTestingAllMap)
	{
		for (size_t i = 0; i < m_vecOldMapList.size(); i++)
		{
			//旧引擎加载所有旧格式场景与新引擎加载旧格式场景的数据验证
			nRetCode = ValidateLoadOldMapByDifferEngine(m_vecOldMapList[i].c_str());
			KGLOG_PROCESS_ERROR(nRetCode);
			//所有旧格式场景的重存测试
			ValidateSaveExistMap(PACKFILE_0, m_vecOldMapList[i].c_str());
			//验证所有旧格式地图转换为新格式地图
			ValidateOldFormatTransformNew(m_vecOldMapList[i].c_str(), true);
			//验证所有旧格式地图不转换为新格式地图
			ValidateOldFormatTransformNew(m_vecOldMapList[i].c_str(), false);
		}
		for (size_t i = 0; i < m_vecNewMapList.size(); i++)
		{
			//所有新格式场景的重存测试
			ValidateSaveExistMap(PACKFILE_1, m_vecNewMapList[i].c_str());
		}
	}
	else
	{
		if (m_vecOldMapList.size())
		{
			//旧引擎加载旧格式场景与新引擎加载旧格式场景的数据验证
			nMapNameIndex = KGTestCommon::GetRandom(0, (int)m_vecOldMapList.size());
			nRetCode = ValidateLoadOldMapByDifferEngine(m_vecOldMapList[nMapNameIndex].c_str());
			KGLOG_PROCESS_ERROR(nRetCode);

			//随机旧格式场景的重存测试
			nMapNameIndex = KGTestCommon::GetRandom(0, (int)m_vecOldMapList.size());
			ValidateSaveExistMap(PACKFILE_0, m_vecOldMapList[nMapNameIndex].c_str());

			//验证随机旧格式地图转换为新格式地图
			nMapNameIndex = KGTestCommon::GetRandom(0, (int)m_vecOldMapList.size());
			ValidateOldFormatTransformNew(m_vecOldMapList[nMapNameIndex].c_str(), true);

			//验证随机旧格式地图不转换为新格式地图
			ValidateOldFormatTransformNew(m_vecOldMapList[nMapNameIndex].c_str(), false);
		}
		
		if (m_vecNewMapList.size())
		{
			//随机新格式场景的重存测试
			nMapNameIndex = KGTestCommon::GetRandom(0, (int)m_vecNewMapList.size());
			ValidateSaveExistMap(PACKFILE_1, m_vecNewMapList[nMapNameIndex].c_str());
		}
	}

	//创建旧格式场景的测试
	ValidateSaveCreatedMap(PACKFILE_0);

	//创建新格式场景的测试
	ValidateSaveCreatedMap(PACKFILE_1);

	nResult = true;
Exit0:
	//输出用例整体执行结果
	nRetCode = OutputResult();
	KGLOG_CHECK_ERROR(nRetCode);
	return nResult;
}

int KG3DMapFormatTest::CompareDifferEngineMapData(IKG3DScene* pNewEngineScene, IKG3DEngineController* pOldEngineController)
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hrRetCode = E_FAIL;
	IEKG3DSceneSceneEditor*		pNewEngineIEEditor	   = NULL;
	IEKG3DSceneOutDoorSpaceMgr*	pNewEngineSceneOutDoor = NULL;

	KG_ASSERT_EXIT(pNewEngineScene);
	KG_ASSERT_EXIT(pOldEngineController);

	hrRetCode = ((IEKG3DScene*)pNewEngineScene)->GetSceneSceneEditor(&pNewEngineIEEditor);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	pNewEngineSceneOutDoor = pNewEngineIEEditor->GetIEKG3DSceneOutDoorSpaceMgr();
	KGLOG_PROCESS_ERROR(pNewEngineSceneOutDoor);

	nRetCode = CompareDifferEngineEntityInfo(pNewEngineSceneOutDoor, pOldEngineController);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = CompareDifferEngineTerrainInfo(pNewEngineSceneOutDoor, pOldEngineController);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

int KG3DMapFormatTest::CleanSaveMap(const char cszMapName[])
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hrRetCode = E_FAIL;
	char* pszResult = NULL;
	char szPath[MAX_PATH] = {0};

	KG_ASSERT_EXIT(cszMapName);
	KGLOG_PROCESS_ERROR(cszMapName[0] != '\0');

	nRetCode = strncpy_s(szPath, sizeof(szPath) - 1, cszMapName, strlen(cszMapName));
	KGLOG_PROCESS_ERROR(nRetCode == 0);
	pszResult = strrchr(szPath, '\\');
	KGLOG_PROCESS_ERROR(pszResult);
	*(pszResult + 1) = '\0';
	pszResult = NULL;

	nRetCode = KGTestCommon::CleanPath(szPath);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = ::RemoveDirectory(szPath);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

int KG3DMapFormatTest::CompareEntityInfo(IEKG3DSceneOutDoorSpaceMgr* pOutDoorMgr_1, IEKG3DSceneOutDoorSpaceMgr* pOutDoorMgr_2)
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hrRetCode = E_FAIL;
	
	struct EntityInfoData
	{
		DWORD dwDataType;
		DWORD dwLength;
		PVOID pData;
	};
	EntityInfoData Data_1;
	EntityInfoData Data_2;
	ZeroMemory(&Data_1, sizeof(EntityInfoData));
	ZeroMemory(&Data_2, sizeof(EntityInfoData));
	vector<UINT> vecEntityHandle_1;
	vector<UINT> vecEntityHandle_2;

	KG_ASSERT_EXIT(pOutDoorMgr_1);
	KG_ASSERT_EXIT(pOutDoorMgr_2);

	KGTestCommon::OutInfo("·CompareEntityInfo");

	Data_1.pData = new BYTE[EM_BUFFER_SIZE];
	KGLOG_PROCESS_ERROR(Data_1.pData);
	Data_2.pData = new BYTE[EM_BUFFER_SIZE];
	KGLOG_PROCESS_ERROR(Data_2.pData);

	hrRetCode = pOutDoorMgr_1->GetAllEntityInfoHandle(&vecEntityHandle_1);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);
	hrRetCode = pOutDoorMgr_2->GetAllEntityInfoHandle(&vecEntityHandle_2);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	if (vecEntityHandle_1.size() != vecEntityHandle_2.size())
	{
		KGLogPrintf(KGLOG_ERR, "EntityInfo Handle List Size Differ.");
		KG_PROCESS_ERROR(false);
	}
	for (size_t i = 0; i < vecEntityHandle_1.size(); i++)
	{
		//比较handle
		char szResult[MAX_PATH] = {0};
		UINT uEntityHandle_1 = vecEntityHandle_1[i];
		UINT uEntityHandle_2 = vecEntityHandle_2[i];
		if (uEntityHandle_1 != uEntityHandle_2)
		{
			nRetCode = _snprintf_s(
				szResult,
				sizeof(szResult),
				sizeof(szResult) - 1,        
				"EntityInfo Handle Differ.(Index = %d Handle1 = %d Handle2 = %d)",
				i,
				uEntityHandle_1,
				uEntityHandle_2
			);
			KGLOG_PROCESS_ERROR(nRetCode > 0);
			KGLogPrintf(KGLOG_ERR, szResult);
			KG_PROCESS_ERROR(false);
		}
		else
		{
			//比较数据
			hrRetCode = pOutDoorMgr_1->GetEntityInfoformation(
				uEntityHandle_1, 
				&Data_1.dwDataType,
				&Data_1.dwLength,
				Data_1.pData,
				EM_BUFFER_SIZE);
			KGLOG_COM_PROCESS_ERROR(hrRetCode);

			hrRetCode = pOutDoorMgr_2->GetEntityInfoformation(
				uEntityHandle_2, 
				&Data_2.dwDataType,
				&Data_2.dwLength,
				Data_2.pData,
				EM_BUFFER_SIZE);
			KGLOG_COM_PROCESS_ERROR(hrRetCode);

			if (Data_1.dwDataType != Data_2.dwDataType ||
				Data_1.dwLength != Data_2.dwLength ||
				!CompareDataBuffer(Data_1.pData, Data_2.pData, Data_1.dwLength))
			{
				nRetCode = _snprintf_s(
					szResult,
					sizeof(szResult),
					sizeof(szResult) - 1,        
					"EntityInfoData Differ. Index = %d Handle = %d",
					i,
					uEntityHandle_1
					);
				KGLOG_PROCESS_ERROR(nRetCode > 0);
				KGLogPrintf(KGLOG_ERR, szResult);
				KG_PROCESS_ERROR(false);
			}
		}
	}
	KGTestCommon::OutInfo(("·Num:" + KGTestCommon::IntToString((int)vecEntityHandle_1.size())).c_str());

	nResult = true;
Exit0:
	vecEntityHandle_1.clear();
	vecEntityHandle_2.clear();
	if (Data_1.pData)
	{
		SAFE_DELETE_ARRAY(Data_1.pData);
	}
	if (Data_2.pData)
	{
		SAFE_DELETE_ARRAY(Data_2.pData);
	}
	return nResult;
}

int KG3DMapFormatTest::CompareDataBuffer(PVOID pData_1, PVOID pData_2, DWORD dwLenth)
{
	int nResult  = false;
	int nRetCode = false;

	KG_ASSERT_EXIT(pData_1);
	KG_ASSERT_EXIT(pData_2);

	BYTE* pbyData_1 = (BYTE*)pData_1;
	BYTE* pbyData_2 = (BYTE*)pData_2;

	for (DWORD i = 0; i < dwLenth; i++)
	{
		KGLOG_PROCESS_ERROR(pbyData_1[i] == pbyData_2[i]);
	}

	nResult = true;
Exit0:
	return nResult;
}

int KG3DMapFormatTest::CompareTerrainInfo(IEKG3DSceneOutDoorSpaceMgr* pOutDoorMgr_1, IEKG3DSceneOutDoorSpaceMgr* pOutDoorMgr_2)
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hrRetCode = E_FAIL;

	struct TerrainInfoData
	{
		DWORD dwDataType;
		DWORD dwLength;
		PVOID pData;
	};
	TerrainInfoData Data_1;
	TerrainInfoData Data_2;
	ZeroMemory(&Data_1, sizeof(TerrainInfoData));
	ZeroMemory(&Data_2, sizeof(TerrainInfoData));
	vector<UINT> vecTerrainHandle_1;
	vector<UINT> vecTerrainHandle_2;

	KG_ASSERT_EXIT(pOutDoorMgr_1);
	KG_ASSERT_EXIT(pOutDoorMgr_2);

	KGTestCommon::OutInfo("·CompareTerrainInfo");

	Data_1.pData = new BYTE[EM_BUFFER_SIZE];
	KGLOG_PROCESS_ERROR(Data_1.pData);
	Data_2.pData = new BYTE[EM_BUFFER_SIZE];
	KGLOG_PROCESS_ERROR(Data_2.pData);

	hrRetCode = pOutDoorMgr_1->GetAllTerrainInfoHandle(&vecTerrainHandle_1);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);
	hrRetCode = pOutDoorMgr_2->GetAllTerrainInfoHandle(&vecTerrainHandle_2);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	if (vecTerrainHandle_1.size() != vecTerrainHandle_2.size())
	{
		KGLogPrintf(KGLOG_ERR, "TerrainInfo Handle List Size Differ.");
		KG_PROCESS_ERROR(false);
	}
	for (size_t i = 0; i < vecTerrainHandle_1.size(); i++)
	{
		//比较handle
		char szResult[MAX_PATH] = {0};
		UINT uTerrainHandle_1 = vecTerrainHandle_1[i];
		UINT uTerrainHandle_2 = vecTerrainHandle_2[i];
		if (uTerrainHandle_1 != uTerrainHandle_2)
		{
			nRetCode = _snprintf_s(
				szResult,
				sizeof(szResult),
				sizeof(szResult) - 1,        
				"TerrainInfo Handle Differ.(Index = %d Handle1 = %d Handle2 = %d)",
				i,
				uTerrainHandle_1,
				uTerrainHandle_2
			);
			KGLOG_PROCESS_ERROR(nRetCode > 0);
			KGLogPrintf(KGLOG_ERR, szResult);
			KG_PROCESS_ERROR(false);
		}
		else
		{
			//比较handle对应的数据
			hrRetCode = pOutDoorMgr_1->GetTerrainInfoformation(
				uTerrainHandle_1, 
				&Data_1.dwDataType,
				&Data_1.dwLength,
				Data_1.pData);
			KGLOG_COM_PROCESS_ERROR(hrRetCode);

			hrRetCode = pOutDoorMgr_2->GetTerrainInfoformation(
				uTerrainHandle_2, 
				&Data_2.dwDataType,
				&Data_2.dwLength,
				Data_2.pData);
			KGLOG_COM_PROCESS_ERROR(hrRetCode);

			if (Data_1.dwDataType != Data_2.dwDataType ||
				Data_1.dwLength != Data_2.dwLength ||
				!CompareDataBuffer(Data_1.pData, Data_2.pData, Data_1.dwLength))
			{
				nRetCode = _snprintf_s(
					szResult,
					sizeof(szResult),
					sizeof(szResult) - 1,        
					"TerrainInfoData Differ. Index = %d Handle = %d",
					i,
					uTerrainHandle_1
					);
				KGLOG_PROCESS_ERROR(nRetCode > 0);
				KGLogPrintf(KGLOG_ERR, szResult);
				KG_PROCESS_ERROR(false);
			}

		}
	}
	KGTestCommon::OutInfo(("·Finish Num:" + KGTestCommon::IntToString((int)vecTerrainHandle_1.size())).c_str());

	nResult = true;
Exit0:
	vecTerrainHandle_1.clear();
	vecTerrainHandle_2.clear();
	if (Data_1.pData)
	{
		SAFE_DELETE_ARRAY(Data_1.pData);
	}
	if (Data_2.pData)
	{
		SAFE_DELETE_ARRAY(Data_2.pData);
	}
	return nResult;
}

int KG3DMapFormatTest::CompareSceneBlockInfo(IEKG3DSceneOutDoorSpaceMgr* pOutDoorMgr_1, IEKG3DSceneOutDoorSpaceMgr* pOutDoorMgr_2)
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hrRetCode = E_FAIL;

	struct SceneBlockData
	{
		DWORD dwLength;
		PVOID pData;
	};
	SceneBlockData Data_1;
	SceneBlockData Data_2;
	ZeroMemory(&Data_1, sizeof(SceneBlockData));
	ZeroMemory(&Data_2, sizeof(SceneBlockData));
	vector<SceneBlockIndex> vecSceneBlockIndex_1;
	vector<SceneBlockIndex> vecSceneBlockIndex_2;

	KG_ASSERT_EXIT(pOutDoorMgr_1);
	KG_ASSERT_EXIT(pOutDoorMgr_2);

	KGTestCommon::OutInfo("·CompareSceneBlockInfo");

	Data_1.pData = new BYTE[EM_BUFFER_SIZE];
	KGLOG_PROCESS_ERROR(Data_1.pData);
	Data_2.pData = new BYTE[EM_BUFFER_SIZE];
	KGLOG_PROCESS_ERROR(Data_2.pData);

	hrRetCode = pOutDoorMgr_1->GetAllSceneBlock(&vecSceneBlockIndex_1);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);
	hrRetCode = pOutDoorMgr_2->GetAllSceneBlock(&vecSceneBlockIndex_2);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	if (vecSceneBlockIndex_1.size() != vecSceneBlockIndex_2.size())
	{
		KGLogPrintf(KGLOG_ERR, "SceneBlockIndex List Size Differ.");
		KG_PROCESS_ERROR(false);
	}
	for (size_t i = 0; i < vecSceneBlockIndex_1.size(); i++)
	{
		//比较handle
		char szResult[MAX_PATH] = {0};
		SceneBlockIndex* pSceneBlockIndex_1 = &vecSceneBlockIndex_1[i];
		SceneBlockIndex* pSceneBlockIndex_2 = &vecSceneBlockIndex_2[i];
		if (pSceneBlockIndex_1->nLevel != pSceneBlockIndex_2->nLevel ||
			pSceneBlockIndex_1->Index.x != pSceneBlockIndex_2->Index.x ||
			pSceneBlockIndex_1->Index.y != pSceneBlockIndex_2->Index.y)
		{
			nRetCode = _snprintf_s(
				szResult,
				sizeof(szResult),
				sizeof(szResult) - 1,        
				"SceneBlock Index Differ.(Index = %d BlockIndex1.nLevel = %d BlockIndex2.nLevel = %d"
				"BlockIndex1.Index.x = %d BlockIndex2.Index.x = %d BlockIndex1.Index.y = %d BlockIndex2.Index.y = %d",
				i,
				pSceneBlockIndex_1->nLevel,
				pSceneBlockIndex_2->nLevel,
				pSceneBlockIndex_1->Index.x,
				pSceneBlockIndex_2->Index.x,
				pSceneBlockIndex_1->Index.y,
				pSceneBlockIndex_2->Index.y
				);
			KGLOG_PROCESS_ERROR(nRetCode > 0);
			KGLogPrintf(KGLOG_ERR, szResult);
			KG_PROCESS_ERROR(false);
		}
		else
		{
			//比较handle对应的数据
			hrRetCode = pOutDoorMgr_1->GetSceneBlockData(
				pSceneBlockIndex_1->nLevel,
				pSceneBlockIndex_1->Index,
				Data_1.dwLength,
				Data_1.pData);
			KGLOG_COM_PROCESS_ERROR(hrRetCode);

			hrRetCode = pOutDoorMgr_2->GetSceneBlockData(
				pSceneBlockIndex_2->nLevel,
				pSceneBlockIndex_2->Index,
				Data_2.dwLength,
				Data_2.pData);
			KGLOG_COM_PROCESS_ERROR(hrRetCode);

			if (Data_1.dwLength != Data_2.dwLength ||
				!CompareDataBuffer(Data_1.pData, Data_2.pData, Data_1.dwLength))
			{
				nRetCode = _snprintf_s(
					szResult,
					sizeof(szResult),
					sizeof(szResult) - 1,        
					"SceneBlockData Differ. Index = %d BlockIndex.nLevel = %d BlockIndex.Index.x = %d BlockIndex.Index.y = %d",
					i,
					pSceneBlockIndex_1->nLevel,
					pSceneBlockIndex_1->Index.x,
					pSceneBlockIndex_1->Index.y
					);
				KGLOG_PROCESS_ERROR(nRetCode > 0);
				KGLogPrintf(KGLOG_ERR, szResult);
				KG_PROCESS_ERROR(false);
			}

		}
	}
	KGTestCommon::OutInfo(("·Finish Num:" + KGTestCommon::IntToString((int)vecSceneBlockIndex_1.size())).c_str());

	nResult = true;
Exit0:
	vecSceneBlockIndex_1.clear();
	vecSceneBlockIndex_2.clear();
	if (Data_1.pData)
	{
		SAFE_DELETE_ARRAY(Data_1.pData);
	}
	if (Data_2.pData)
	{
		SAFE_DELETE_ARRAY(Data_2.pData);
	}
	return nResult;
}


int KG3DMapFormatTest::ValidateSaveExistMap(int nPackFileSign, const char cszMapName[])
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hrRetCode = E_FAIL;
	int nMapNameIndex = 0;
	IKG3DScene* pExistScene = NULL;
	IKG3DScene* pSourceScene = NULL;
	string strCaseName;
	char szMapPath[MAX_PATH]			= {0};
	char szMapFullName[MAX_PATH]		= {0};
	char szTestMapPath[MAX_PATH]		= {0};
	char szTestMapFullName[MAX_PATH]	= {0};
	char szOldValue[MAX_PATH]			= {0};
	bool bGotValue = false;
	CaseInfo caseInfo;

	switch(nPackFileSign)
	{
		case PACKFILE_0:
			{
				strCaseName = "【ValidateSaveExistMap(Save Old Format Map)】";
				nRetCode = _snprintf_s(
					szMapPath,
					sizeof(szMapPath),
					sizeof(szMapPath) - 1,        
					"%sdata\\source\\maps\\%s\\",
					m_szClientPath,
					cszMapName
					);
				KGLOG_PROCESS_ERROR(nRetCode > 0);
			}
			break;
		case PACKFILE_1:
			{
				strCaseName = "【ValidateSaveExistMap(Save New Format Map)】";
				nRetCode = _snprintf_s(
					szMapPath,
					sizeof(szMapPath),
					sizeof(szMapPath) - 1,        
					"%s%s\\",
					g_szApplicationPath,
					cszMapName
					);
				KGLOG_PROCESS_ERROR(nRetCode > 0);
			}
			break;
		default:
			KGLOG_PROCESS_ERROR(false);
	}
	KGTestCommon::OutInfo(strCaseName.c_str());
	caseInfo.strCaseName = strCaseName;
	caseInfo.strCaseInfo = string(TEXT_TEST_MAP) + string(cszMapName);

	nRetCode = _snprintf_s(
		szTestMapPath,
		sizeof(szTestMapPath),
		sizeof(szTestMapPath) - 1,        
		"%sTemp\\",
		g_szApplicationPath
		);
	KGLOG_PROCESS_ERROR(nRetCode > 0);

	KGTestCommon::OutInfo("·Copying Map Files");
	nRetCode = KGFileManager::Instance()->CopyPathFiles(szMapPath, szTestMapPath);
	KGLOG_PROCESS_ERROR(nRetCode);

	//准备测试环境
	nRetCode = GetPrivateProfileString(CONFIG_SECTION, CONFIG_KEY, "", szOldValue, sizeof(szOldValue), m_szConfigFullName);
	KGLOG_PROCESS_ERROR(nRetCode);
	bGotValue = true;
	nRetCode = WritePrivateProfileString(CONFIG_SECTION, CONFIG_KEY, "0", m_szConfigFullName);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_EngineHelper.LoadEngineDLL();
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = _snprintf_s(
		szTestMapFullName,
		sizeof(szTestMapFullName),
		sizeof(szTestMapFullName) - 1,        
		"%s%s.map",
		szTestMapPath,
		cszMapName
		);
	KGLOG_PROCESS_ERROR(nRetCode > 0);

	//加载地图场景信息
	nRetCode = m_EngineHelper.LoadMapData(szTestMapFullName, &pExistScene);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_EngineHelper.SaveMap(szTestMapFullName, pExistScene);
	KGLOG_PROCESS_ERROR(nRetCode);

	//验证新格式的文件
	if (nPackFileSign == PACKFILE_1)
	{
		nRetCode = CheckNewFormatFile(szTestMapFullName);
		KGLOG_PROCESS_ERROR(nRetCode);
	}
	//比较场景数据
	nRetCode = _snprintf_s(
		szMapFullName,
		sizeof(szMapFullName),
		sizeof(szMapFullName) - 1,        
		"%s%s.map",
		szMapPath,
		cszMapName
		);
	KGLOG_PROCESS_ERROR(nRetCode > 0);

	nRetCode = m_EngineHelper.LoadMapInfo(szMapFullName, &pSourceScene);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = CompareMapData(pSourceScene, pExistScene);
	KGLOG_PROCESS_ERROR(nRetCode);

	//比较场景文件
	nRetCode = CompareMapFiles(szMapPath, szTestMapPath);
	KGLOG_PROCESS_ERROR(nRetCode);

	KGTestCommon::OutInfo(CASE_PASS);
	caseInfo.strCaseResult = CASE_PASS;

	nResult = true;
Exit0:
	if (!nResult)
	{
		KGTestCommon::OutInfo(CASE_FAILED);
		caseInfo.strCaseResult = CASE_FAILED;
	}
	m_vecTestResult.push_back(caseInfo);
	if (pExistScene)
	{
		nRetCode = m_EngineHelper.RemoveScene(pExistScene);
		KGLOG_CHECK_ERROR(nRetCode);
	}
	if (pSourceScene)
	{
		nRetCode = m_EngineHelper.RemoveScene(pSourceScene);
		KGLOG_CHECK_ERROR(nRetCode);
	}
	nRetCode = m_EngineHelper.UnLoadEngineDLL();
	KGLOG_CHECK_ERROR(nRetCode);
	nRetCode = CleanSaveMap(szTestMapPath);
	KGLOG_CHECK_ERROR(nRetCode);
	if (bGotValue)
	{
		nRetCode = WritePrivateProfileString(CONFIG_SECTION, CONFIG_KEY, szOldValue, m_szConfigFullName);
		KGLOG_CHECK_ERROR(nRetCode);
	}
	return nResult;
}

int KG3DMapFormatTest::CompareDifferEngineEntityInfo(IEKG3DSceneOutDoorSpaceMgr* pNewEngineOutDoorMgr, IKG3DEngineController* pOldEngineController)
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hrRetCode = E_FAIL;
	struct EntityInfoData
	{
		DWORD dwDataType;
		DWORD dwLength;
		PVOID pData;
	};
	EntityInfoData NewEngineEntityData;
	EntityInfoData OldEngineEntityData;

	ZeroMemory(&NewEngineEntityData, sizeof(EntityInfoData));
	ZeroMemory(&OldEngineEntityData, sizeof(EntityInfoData));
	vector<UINT> vecOldEngineMapInfoHandle;
	vector<UINT> vecNewEngineMapInfoHandle;

	KG_ASSERT_EXIT(pNewEngineOutDoorMgr);
	KG_ASSERT_EXIT(pOldEngineController);

	//比较EntityInfoHandle
	KGTestCommon::OutInfo("·Compare EntityInfo");

	NewEngineEntityData.pData = new BYTE[EM_BUFFER_SIZE];
	KGLOG_PROCESS_ERROR(NewEngineEntityData.pData);
	OldEngineEntityData.pData = new BYTE[EM_BUFFER_SIZE];
	KGLOG_PROCESS_ERROR(OldEngineEntityData.pData);

	hrRetCode = pOldEngineController->GetAllEntityInfoHandle(&vecOldEngineMapInfoHandle);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	hrRetCode = pNewEngineOutDoorMgr->GetAllEntityInfoHandle(&vecNewEngineMapInfoHandle);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	KGLOG_PROCESS_ERROR(vecOldEngineMapInfoHandle.size() == vecNewEngineMapInfoHandle.size());
	for (size_t i = 0; i < vecOldEngineMapInfoHandle.size(); i++)
	{
		char szResult[MAX_PATH] = {0};
		UINT uOldEngineMapInfoHandle = vecOldEngineMapInfoHandle[i];
		UINT uNewEngineMapInfoHandle = vecNewEngineMapInfoHandle[i];
		if (uOldEngineMapInfoHandle != uNewEngineMapInfoHandle)
		{
			nRetCode = _snprintf_s(
				szResult,
				sizeof(szResult),
				sizeof(szResult) - 1,        
				"EntityInfo Handle Differ.(Index = %d OldEngineMapInfoHandle = %d NewEngineMapInfoHandle = %d)",
				i,
				uOldEngineMapInfoHandle,
				uNewEngineMapInfoHandle
				);
			KGLOG_PROCESS_ERROR(nRetCode > 0);
			KGLogPrintf(KGLOG_ERR, szResult);
			KG_PROCESS_ERROR(false);
		}
		else
		{
			hrRetCode = pNewEngineOutDoorMgr->GetEntityInfoformation(
				uNewEngineMapInfoHandle,
				&NewEngineEntityData.dwDataType,
				&NewEngineEntityData.dwLength,
				NewEngineEntityData.pData,
				EM_BUFFER_SIZE);
			KGLOG_COM_PROCESS_ERROR(hrRetCode);

			hrRetCode = pOldEngineController->GetEntityInfoformation(
				uOldEngineMapInfoHandle,
				&OldEngineEntityData.dwDataType,
				&OldEngineEntityData.dwLength,
				OldEngineEntityData.pData,
				EM_BUFFER_SIZE);
			KGLOG_COM_PROCESS_ERROR(hrRetCode);

			if (NewEngineEntityData.dwDataType != OldEngineEntityData.dwDataType ||
				NewEngineEntityData.dwLength != OldEngineEntityData.dwLength ||
				!CompareDataBuffer(NewEngineEntityData.pData, OldEngineEntityData.pData, OldEngineEntityData.dwLength))
			{
				nRetCode = _snprintf_s(
					szResult,
					sizeof(szResult),
					sizeof(szResult) - 1,        
					"EntityInfoData Differ. Index = %d Handle = %d",
					i,
					uNewEngineMapInfoHandle
					);
				KGLOG_PROCESS_ERROR(nRetCode > 0);
				KGLogPrintf(KGLOG_ERR, szResult);
				KG_PROCESS_ERROR(false);
			}
		}
	}

	KGTestCommon::OutInfo(("·Num:" + KGTestCommon::IntToString((int)vecOldEngineMapInfoHandle.size())).c_str());

	nResult = true;
Exit0:
	vecOldEngineMapInfoHandle.clear();
	vecNewEngineMapInfoHandle.clear();
	if (NewEngineEntityData.pData)
	{
		SAFE_DELETE_ARRAY(NewEngineEntityData.pData);
	}
	if (OldEngineEntityData.pData)
	{
		SAFE_DELETE_ARRAY(OldEngineEntityData.pData);
	}
	return nResult;
}

int KG3DMapFormatTest::CompareDifferEngineTerrainInfo( IEKG3DSceneOutDoorSpaceMgr* pNewEngineOutDoorMgr, IKG3DEngineController* pOldEngineController )
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hrRetCode = E_FAIL;
	struct TerrainInfoData
	{
		DWORD dwDataType;
		DWORD dwLength;
		PVOID pData;
	};
	TerrainInfoData NewEngineTerrainData;
	TerrainInfoData OldEngineTerrainData;

	ZeroMemory(&NewEngineTerrainData, sizeof(TerrainInfoData));
	ZeroMemory(&OldEngineTerrainData, sizeof(TerrainInfoData));
	vector<UINT> vecOldEngineMapInfoHandle;
	vector<UINT> vecNewEngineMapInfoHandle;
	
	KG_ASSERT_EXIT(pNewEngineOutDoorMgr);
	KG_ASSERT_EXIT(pOldEngineController);

	//比较TerrainInfoHandle
	KGTestCommon::OutInfo("·Compare TerrainInfo");

	NewEngineTerrainData.pData = new BYTE[EM_BUFFER_SIZE];
	KGLOG_PROCESS_ERROR(NewEngineTerrainData.pData);
	OldEngineTerrainData.pData = new BYTE[EM_BUFFER_SIZE];
	KGLOG_PROCESS_ERROR(OldEngineTerrainData.pData);

	hrRetCode = pOldEngineController->GetAllTerrainInfoHandle(&vecOldEngineMapInfoHandle);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	hrRetCode = pNewEngineOutDoorMgr->GetAllTerrainInfoHandle(&vecNewEngineMapInfoHandle);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	KGLOG_PROCESS_ERROR(vecOldEngineMapInfoHandle.size() == vecNewEngineMapInfoHandle.size());
	for (size_t i = 0; i < vecOldEngineMapInfoHandle.size(); i++)
	{
		char szResult[MAX_PATH] = {0};
		UINT uOldEngineMapInfoHandle = vecOldEngineMapInfoHandle[i];
		UINT uNewEngineMapInfoHandle = vecNewEngineMapInfoHandle[i];
		if (uOldEngineMapInfoHandle != uNewEngineMapInfoHandle)
		{
			nRetCode = _snprintf_s(
				szResult,
				sizeof(szResult),
				sizeof(szResult) - 1,        
				"TerrainInfo Handle Differ.(Index = %d OldEngineMapInfoHandle = %d NewEngineMapInfoHandle = %d)",
				i,
				uOldEngineMapInfoHandle,
				uNewEngineMapInfoHandle
				);
			KGLOG_PROCESS_ERROR(nRetCode > 0);
			KGLogPrintf(KGLOG_ERR, szResult);
			KG_PROCESS_ERROR(false);
		}
		else
		{
			hrRetCode = pNewEngineOutDoorMgr->GetTerrainInfoformation(
				uNewEngineMapInfoHandle,
				&NewEngineTerrainData.dwDataType,
				&NewEngineTerrainData.dwLength,
				NewEngineTerrainData.pData
				);
			KGLOG_COM_PROCESS_ERROR(hrRetCode);

			hrRetCode = pOldEngineController->GetTerrainInfoformation(
				uOldEngineMapInfoHandle,
				&OldEngineTerrainData.dwDataType,
				&OldEngineTerrainData.dwLength,
				OldEngineTerrainData.pData
				);
			KGLOG_COM_PROCESS_ERROR(hrRetCode);

			if (NewEngineTerrainData.dwDataType != OldEngineTerrainData.dwDataType ||
				NewEngineTerrainData.dwLength != OldEngineTerrainData.dwLength ||
				!CompareDataBuffer(NewEngineTerrainData.pData, OldEngineTerrainData.pData, OldEngineTerrainData.dwLength))
			{
				nRetCode = _snprintf_s(
					szResult,
					sizeof(szResult),
					sizeof(szResult) - 1,        
					"TerrainInfoData Differ. Index = %d Handle = %d",
					i,
					uNewEngineMapInfoHandle
					);
				KGLOG_PROCESS_ERROR(nRetCode > 0);
				KGLogPrintf(KGLOG_ERR, szResult);
				KG_PROCESS_ERROR(false);
			}
		}
	}

	KGTestCommon::OutInfo(("·Num:" + KGTestCommon::IntToString((int)vecOldEngineMapInfoHandle.size())).c_str());

	nResult = true;
Exit0:
	vecOldEngineMapInfoHandle.clear();
	vecNewEngineMapInfoHandle.clear();
	if (NewEngineTerrainData.pData)
	{
		SAFE_DELETE_ARRAY(NewEngineTerrainData.pData);
	}
	if (OldEngineTerrainData.pData)
	{
		SAFE_DELETE_ARRAY(OldEngineTerrainData.pData);
	}
	return nResult;
	
}

int KG3DMapFormatTest::CompareEntityData(IEKG3DSceneOutDoorSpaceMgr* pOutDoorMgr_1, IEKG3DSceneOutDoorSpaceMgr* pOutDoorMgr_2)
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hrRetCode = E_FAIL;

	map<UINT,KGEntityData> vecEntityData_1;
	map<UINT,KGEntityData>::iterator iterEntityData_1;
	map<UINT,KGEntityData> vecEntityData_2;
	map<UINT,KGEntityData>::iterator iterEntityData_2;

	KG_ASSERT_EXIT(pOutDoorMgr_1);
	KG_ASSERT_EXIT(pOutDoorMgr_2);

	KGTestCommon::OutInfo("·CompareEntityData");

	hrRetCode = pOutDoorMgr_1->GetEntityData(&vecEntityData_1);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);
	hrRetCode = pOutDoorMgr_2->GetEntityData(&vecEntityData_2);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	if (vecEntityData_1.size() != vecEntityData_2.size())
	{
		KGLogPrintf(KGLOG_ERR, "EntityData List Size Differ.");
		KG_PROCESS_ERROR(false);
	}
	for (iterEntityData_1 = vecEntityData_1.begin(), iterEntityData_2 = vecEntityData_2.begin();
		(iterEntityData_1 != vecEntityData_1.end()) && (iterEntityData_2 != vecEntityData_2.end());
		++iterEntityData_1, ++iterEntityData_2)
	{
		//compare handle
		char szResult[MAX_PATH] = {0};
		UINT uEntityData_1 = (*iterEntityData_1).first;
		UINT uEntityData_2 = (*iterEntityData_2).first;
		if (uEntityData_1 != uEntityData_2)
		{
			nRetCode = _snprintf_s(
				szResult,
				sizeof(szResult),
				sizeof(szResult) - 1,        
				"EntityData Handle Differ.(Handle1 = %d Handle2 = %d)",
				uEntityData_1,
				uEntityData_2
				);
			KGLOG_PROCESS_ERROR(nRetCode > 0);
			KGLogPrintf(KGLOG_ERR, szResult);
			KG_PROCESS_ERROR(false);
		}
		else
		{
	

			KGEntityData* pEntityData_1 = &((*iterEntityData_1).second);
			KGEntityData* pEntityData_2 = &((*iterEntityData_2).second);
			if (pEntityData_1->uEntityInfoHandle != pEntityData_2->uEntityInfoHandle ||
				pEntityData_1->m_Scaling != pEntityData_2->m_Scaling ||
				pEntityData_1->m_Rotation != pEntityData_2->m_Rotation ||
				pEntityData_1->m_Translation != pEntityData_2->m_Translation ||
				pEntityData_1->m_vWorldPos != pEntityData_2->m_vWorldPos ||
				pEntityData_1->m_vBox_A != pEntityData_2->m_vBox_A ||
				pEntityData_1->m_vBox_B != pEntityData_2->m_vBox_B
				)
			{
				nRetCode = _snprintf_s(
					szResult,
					sizeof(szResult),
					sizeof(szResult) - 1,        
					"EntityData Differ. Handle = %d",
					uEntityData_1
					);
				KGLOG_PROCESS_ERROR(nRetCode > 0);
				KGLogPrintf(KGLOG_ERR, szResult);
				KG_PROCESS_ERROR(false);
			}
		}
	}
	KGTestCommon::OutInfo(("·Num:" + KGTestCommon::IntToString((int)vecEntityData_1.size())).c_str());

	nResult = true;
Exit0:
	vecEntityData_1.clear();
	vecEntityData_2.clear();
	return nResult;
}

int KG3DMapFormatTest::ValidateOldFormatTransformNew(const char cszOldMapName[], bool bTransformNew)
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hrRetCode = E_FAIL;
	int nMapNameIndex = 0;
	IKG3DScene* pExistScene = NULL;
	IKG3DScene* pSavedScene = NULL;
	char szMapFullName[MAX_PATH] = {0};
	char szOldValue[MAX_PATH]	 = {0};
	bool bGotValue = false;
	string strCaseName;
	CaseInfo caseInfo;

	KG_ASSERT_EXIT(cszOldMapName);
	KG_ASSERT_EXIT(cszOldMapName[0] != '\0');

	if (bTransformNew)
	{
		strCaseName = "【ValidateOldFormatTransformNew(Transform)】";
	}
	else
	{
		strCaseName = "【ValidateOldFormatTransformNew(No Transform)】";
	}
	KGTestCommon::OutInfo(strCaseName.c_str());
	caseInfo.strCaseName = strCaseName;
	caseInfo.strCaseInfo = string(TEXT_TEST_MAP) + string(cszOldMapName);

	nRetCode = _snprintf_s(
		szMapFullName,
		sizeof(szMapFullName),
		sizeof(szMapFullName) - 1,        
		"%sdata\\source\\maps\\%s\\%s.map",
		m_szClientPath,
		cszOldMapName,
		cszOldMapName
	);
	KGLOG_PROCESS_ERROR(nRetCode > 0);

	//准备测试环境
	nRetCode = GetPrivateProfileString(CONFIG_SECTION, CONFIG_KEY, "", szOldValue, sizeof(szOldValue), m_szConfigFullName);
	KGLOG_PROCESS_ERROR(nRetCode);
	bGotValue = true;
	nRetCode = WritePrivateProfileString(CONFIG_SECTION, CONFIG_KEY, "2", m_szConfigFullName);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_EngineHelper.LoadEngineDLL();
	KGLOG_PROCESS_ERROR(nRetCode);

	//加载地图场景信息
	nRetCode = m_EngineHelper.LoadMapData(szMapFullName, &pExistScene);
	KGLOG_PROCESS_ERROR(nRetCode);
	//创建监听MessageBox窗口的线程
	m_bTransformNew = bTransformNew;
	nRetCode = CreateWindowFinderThread();
	KGLOG_PROCESS_ERROR(nRetCode);
	//重存
	nRetCode = m_EngineHelper.SaveMap(m_szTestMapFullName, pExistScene);
	KGLOG_PROCESS_ERROR(nRetCode);
	//关闭监听线程
	nRetCode = DestroyThread();
	KGLOG_PROCESS_ERROR(nRetCode);
	//重新加载
	nRetCode = m_EngineHelper.LoadMapInfo(m_szTestMapFullName, &pSavedScene);
	KGLOG_PROCESS_ERROR(nRetCode);
	//验证新格式的文件
	nRetCode = CheckNewFormatFile(m_szTestMapFullName);
	KGLOG_PROCESS_ERROR(nRetCode);
	//验证地图版本是否为新格式版本
	nRetCode = CheckMapVersion(m_szTestMapFullName, MAP_FILE_VERSION_PACKREGION);
	KGLOG_PROCESS_ERROR(nRetCode);
	//验证数据
	nRetCode = CompareMapData(pExistScene, pSavedScene);
	KGLOG_PROCESS_ERROR(nRetCode);

	KGTestCommon::OutInfo(CASE_PASS);
	caseInfo.strCaseResult = CASE_PASS;

	nResult = true;
Exit0:
	if (!nResult)
	{
		KGTestCommon::OutInfo(CASE_FAILED);
		caseInfo.strCaseResult = CASE_FAILED;
	}
	m_vecTestResult.push_back(caseInfo);
	if (pExistScene)
	{
		nRetCode = m_EngineHelper.RemoveScene(pExistScene);
		KGLOG_CHECK_ERROR(nRetCode);
	}
	if (pSavedScene)
	{
		nRetCode = m_EngineHelper.RemoveScene(pSavedScene);
		KGLOG_CHECK_ERROR(nRetCode);
	}
	nRetCode = m_EngineHelper.UnLoadEngineDLL();
	KGLOG_CHECK_ERROR(nRetCode);
	nRetCode = CleanSaveMap(m_szTestMapFullName);
	KGLOG_CHECK_ERROR(nRetCode);
	if (bGotValue)
	{
		nRetCode = WritePrivateProfileString(CONFIG_SECTION, CONFIG_KEY, szOldValue, m_szConfigFullName);
		KGLOG_CHECK_ERROR(nRetCode);
	}
	return nResult;
}

int KG3DMapFormatTest::CheckNewFormatFile(const char cszMapName[])
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hrRetCode = E_FAIL;
	char* pszResult = NULL;
	char szPath[MAX_PATH]		   = {0};
	char szNewFormatFile[MAX_PATH] = {0};
	char szDrive[MAX_PATH]		   = {0};
	char szDir[MAX_PATH]		   = {0};
	char szFName[MAX_PATH]		   = {0};
	char szExt[MAX_PATH]		   = {0};

	KG_ASSERT_EXIT(cszMapName);
	KGLOG_PROCESS_ERROR(cszMapName[0] != '\0');

	nRetCode = _splitpath_s(cszMapName, szDrive, sizeof(szDrive), szDir, sizeof(szDir), szFName, sizeof(szFName), szExt, sizeof(szExt));
	KGLOG_PROCESS_ERROR(nRetCode == 0);

	//map.EntityInfo
	nRetCode = _snprintf_s(
		szNewFormatFile,
		sizeof(szNewFormatFile),
		sizeof(szNewFormatFile) - 1,        
		"%s%s%sFile\\map.EntityInfo",
		szDrive,
		szDir,
		szFName
		);
	KGLOG_PROCESS_ERROR(nRetCode > 0);
	nRetCode = g_IsFileExist(szNewFormatFile);
	KGLOG_PROCESS_ERROR(nRetCode);

	//map.TerrainInfo
	nRetCode = _snprintf_s(
		szNewFormatFile,
		sizeof(szNewFormatFile),
		sizeof(szNewFormatFile) - 1,        
		"%s%s%sFile\\map.TerrainInfo",
		szDrive,
		szDir,
		szFName
		);
	KGLOG_PROCESS_ERROR(nRetCode > 0);
	nRetCode = g_IsFileExist(szNewFormatFile);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;	
}

int KG3DMapFormatTest::LoadMapList()
{
	int nResult = false; 
	int nRetCode = false;
	ITabFile* piTableFile = NULL;
	int nHeight = -1;
	char szMapListFile[MAX_PATH] = {0};
	
	nRetCode = _snprintf_s(
		szMapListFile, 
		sizeof(szMapListFile),
		sizeof(szMapListFile) - 1,
		"%ssettings\\MapList.tab",
		m_szClientPath
		);
	KGLOG_PROCESS_ERROR(nRetCode > 0);

	m_vecOldMapList.clear();

	piTableFile = g_OpenTabFile(szMapListFile);
	KGLOG_PROCESS_ERROR(piTableFile && "Open MapList.tab failed!");

	nHeight = piTableFile->GetHeight();
	KGLOG_PROCESS_ERROR(nHeight >= 2);

	for (int i = 2; i <= nHeight; i++)
	{
		int nMapID = 0;
		char szMapName[MAX_PATH]	 = {0};
		
		nRetCode = piTableFile->GetInteger(i, "ID", 0, &nMapID);
		KGLOG_PROCESS_ERROR(nRetCode == 1);

		nRetCode = piTableFile->GetString(i, "Name", "", szMapName, sizeof(szMapName));
		KGLOG_PROCESS_ERROR(nRetCode == 1);
		KGLOG_PROCESS_ERROR(szMapName[0]);

		if (!strcmp(szMapName, "测试1"))
		{
			continue;
		}

		if (nMapID <= m_CaseSetting.nMaxOldMapID)
		{
			m_vecOldMapList.push_back(string(szMapName));
		}
		else
		{
			//当添加新格式场景时，在这里把新场景名称保存到m_vecNewMapList中
			_ASSERTE(false);
		}
	}
	//临时的新格式场景用于测试
	m_vecNewMapList.push_back(NEW_MAP_NAME[0]);

	nResult = true;
Exit0:
	if (piTableFile)
	{
		SAFE_RELEASE(piTableFile)
	} 
	return nResult;
}

int KG3DMapFormatTest::OutputResult()
{
	int nResult = false; 
	int nRetCode = false;
	char szLogName[MAX_PATH]	= {0};
	char szResultFile[MAX_PATH] = {0};
	char* pszResult = NULL;
	FILE* pFile = NULL;
	string strWrite;

	nRetCode = KGTestCommon::FindLogFile(g_szLogPath, szLogName, sizeof(szLogName));
	KGLOG_PROCESS_ERROR(nRetCode);

	pszResult = strrchr(szLogName, '.');
	KGLOG_PROCESS_ERROR(pszResult);
	*pszResult = '\0';
	pszResult = NULL;

	nRetCode = _snprintf_s(
		szResultFile, 
		sizeof(szResultFile),
		sizeof(szResultFile) - 1,
		"%s_Result.txt",
		szLogName
		);
	KGLOG_PROCESS_ERROR(nRetCode > 0);
	
	pFile = fopen(szResultFile, "w");
	KGLOG_PROCESS_ERROR(pFile);

	strWrite = "Test Case Result:\n";
	fwrite(strWrite.c_str(), strWrite.length(), 1, pFile);
	for (size_t i = 0; i < m_vecTestResult.size(); i++)
	{
		CaseInfo* pCaseInfo = &m_vecTestResult[i];
		strWrite = "\n\t" + pCaseInfo->strCaseName + "\n";
		fwrite(strWrite.c_str(), strWrite.length(), 1, pFile);
		strWrite = "\t        " + pCaseInfo->strCaseInfo + "\n";
		fwrite(strWrite.c_str(), strWrite.length(), 1, pFile);
		strWrite = "\t" + pCaseInfo->strCaseResult + "\n";
		fwrite(strWrite.c_str(), strWrite.length(), 1, pFile);
	}

	nResult = true;
Exit0:
	if (pFile)
	{
		fclose(pFile);
	}
	return nResult;
}

int KG3DMapFormatTest::LoadCaseSetting()
{
	int nResult = false; 
	int nRetCode = false;
	char szSettingFile[MAX_PATH] = {0};

	ZeroMemory(&m_CaseSetting, sizeof(CaseSetting));

	nRetCode = _snprintf_s(
		szSettingFile, 
		sizeof(szSettingFile),
		sizeof(szSettingFile) - 1,
		"%s%s",
		g_szApplicationPath,
		CASE_SETTING_FILE
		);
	KGLOG_PROCESS_ERROR(nRetCode > 0);

	//nTestingAllMap=1时，测试m_vecOldMapList和m_vecNewMapList里的所有地图，
	//nTestingAllMap=0时，从m_vecOldMapList和m_vecNewMapList里随机一张取地图进行测试
	m_CaseSetting.nTestingAllMap = GetPrivateProfileInt("SETTING", "TestingAllMap", -1, szSettingFile);
	KGLOG_PROCESS_ERROR(m_CaseSetting.nTestingAllMap != -1);

	m_CaseSetting.nMaxModelToCreate = GetPrivateProfileInt("SETTING", "MaxModelToCreate", -1, szSettingFile);
	KGLOG_PROCESS_ERROR(m_CaseSetting.nMaxModelToCreate != -1);

	m_CaseSetting.nMinModelToCreate = GetPrivateProfileInt("SETTING", "MinModelToCreate", -1, szSettingFile);
	KGLOG_PROCESS_ERROR(m_CaseSetting.nMinModelToCreate != -1);

	if (m_CaseSetting.nMaxModelToCreate < m_CaseSetting.nMinModelToCreate)
	{
		KGLOG_PROCESS_ERROR("Load CaseSetting.ini Error: nMaxModelToCreate less than nMaxModelToCreate");
	}

	m_CaseSetting.nMaxOldMapID = GetPrivateProfileInt("SETTING", "MaxOldMapID", -1, szSettingFile);
	KGLOG_PROCESS_ERROR(m_CaseSetting.nMaxOldMapID != -1);

	nRetCode = GetPrivateProfileString("SETTING", "ResourcePath", "", m_CaseSetting.szResourcePath, sizeof(m_CaseSetting.szResourcePath), szSettingFile);
	KGLOG_PROCESS_ERROR(nRetCode && m_CaseSetting.szResourcePath[0]);

	{
		char szKey[MAX_PATH]	 = {0};
		char szSection[MAX_PATH] = "MAPLIST";
		char szMapName[MAX_PATH] = {0};
		bool bHasMapList = false;
		for (int i = 1;; i++)
		{
			nRetCode = _snprintf_s(
				szKey, 
				sizeof(szKey),
				sizeof(szKey) - 1,
				"OldMap_%02d",
				i
				);
			KGLOG_PROCESS_ERROR(nRetCode > 0);
			nRetCode = GetPrivateProfileString(szSection, szKey, "", szMapName, sizeof(szMapName), szSettingFile);
			if (szMapName[0] == '\0')
			{
				break;
			}
			m_vecOldMapList.push_back(szMapName);
			bHasMapList = true;
		}
		for (int i = 1;; i++)
		{
			nRetCode = _snprintf_s(
				szKey, 
				sizeof(szKey),
				sizeof(szKey) - 1,
				"NewMap_%02d",
				i
				);
			KGLOG_PROCESS_ERROR(nRetCode > 0);
			nRetCode = GetPrivateProfileString(szSection, szKey, "", szMapName, sizeof(szMapName), szSettingFile);
			if (szMapName[0] == '\0')
			{
				break;
			}
			m_vecNewMapList.push_back(szMapName);
			bHasMapList = true;
		}
		if (!bHasMapList)
		{
			//读取地图列表信息
			nRetCode = LoadMapList();
			KGLOG_PROCESS_ERROR(nRetCode);
		}
	}

	nResult = true;
Exit0:
	return nResult;
}

int KG3DMapFormatTest::CompareMapFiles(const char cszMapPath_1[], const char cszMapPath_2[])
{
	int nResult = false; 
	int nRetCode = false;
	char szResult[MAX_PATH] = {0};
	int nComparedFiles = 0;

	KG_ASSERT_EXIT(cszMapPath_1);
	KGLOG_PROCESS_ERROR(cszMapPath_1[0] != '\0');
	KG_ASSERT_EXIT(cszMapPath_2);
	KGLOG_PROCESS_ERROR(cszMapPath_2[0] != '\0');

	KGTestCommon::OutInfo("·CompareMapFiles");
	nComparedFiles = KGFileManager::Instance()->ComparePathFiles(cszMapPath_1, cszMapPath_2);
	if (nComparedFiles == 0)
	{
		nRetCode = _snprintf_s(
			szResult, 
			sizeof(szResult),
			sizeof(szResult) - 1,
			"Path files compare differ. Path_1=%s Path_2=%s",
			cszMapPath_1,
			cszMapPath_2
			);
		KGLOG_PROCESS_ERROR(nRetCode > 0);
		KGLogPrintf(KGLOG_ERR, szResult);
		KG_PROCESS_ERROR(false);
	}
	KGTestCommon::OutInfo(("·Num:" + KGTestCommon::IntToString((int)nComparedFiles)).c_str());
	
	nResult = true;
Exit0:
	return nResult;
}
int KG3DMapFormatTest::CreateWindowFinderThread()
{
	int nRetCode = false;
	int nResult  = false;
	m_pWorkThread = new KThread();
	nRetCode = m_pWorkThread->Create(FinderRun, this);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

void KG3DMapFormatTest::FinderRun(void *pvArg)
{
	HWND hWindow = NULL;
	HWND hButton = NULL;
	int nID = IDNO;

	while(!hWindow)
	{
		hWindow = ::FindWindow(NULL, "提示");
		::Sleep(1000);
	}
	if (m_bTransformNew)
	{
		nID = IDYES;
	}
	hButton = GetDlgItem(hWindow, nID);   
	::SendMessage(hButton,WM_LBUTTONDOWN,0,0);
	::SendMessage(hButton,WM_LBUTTONUP,0,0);
}

int KG3DMapFormatTest::DestroyThread()
{
	int nRetCode = false;
	int nResult  = false;

	nRetCode = m_pWorkThread->Destroy();
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	SAFE_DELETE(m_pWorkThread);
	return nResult;
}

int KG3DMapFormatTest::CheckMapVersion(const char cszMapName[], DWORD dwCheckVersion)
{
	int nRetCode = false;
	int nResult  = false;
	IFile* pMapFile = NULL;
	struct MapFileHeader
	{
		DWORD dwMask;
		DWORD dwVersion;
		UINT  uCurrentEntityID;
		UINT  uCurrentEntityInformationID;
		UINT  uCurrentTerrainInformationID;
		DWORD dwNumEntity;
		DWORD dwNumEntityInformation;
		DWORD dwNumTerrainInformation;
		DWORD dwNumSceneBlock[4];
		DWORD dwExtend[128];
	};
	MapFileHeader Header;
	size_t uSize = 0;
	char szResult[MAX_PATH] = {0};

	KG_ASSERT_EXIT(cszMapName);
	KGLOG_PROCESS_ERROR(cszMapName[0] != '\0');

	pMapFile = g_OpenFile(cszMapName);
	KGLOG_PROCESS_ERROR(pMapFile);

	uSize = pMapFile->Read(&Header, sizeof(Header));
	KGLOG_PROCESS_ERROR(uSize == sizeof(Header));

	if (Header.dwVersion != dwCheckVersion)
	{
		nRetCode = _snprintf_s(
			szResult,
			sizeof(szResult),
			sizeof(szResult) - 1,        
			"Map Version Differ. Map Version = %d Check Version = %d",
			Header.dwVersion,
			dwCheckVersion
			);
		KGLOG_PROCESS_ERROR(nRetCode > 0);
		KGLogPrintf(KGLOG_ERR, szResult);
		KG_PROCESS_ERROR(false);
	}

	nResult = true;
Exit0:
	KG_COM_RELEASE(pMapFile);
	return nResult;
}