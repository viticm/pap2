#include "KGPostRenderTestingMgr.h"
#include "KGTestEffect.h"

const TCHAR ONE_EFFECT_RESULT_FILE[] = "TestSingleEffectResult.tab";
const TCHAR MUTIL_EFFECT_RESULT_FILE[] = "TestMutilEffectResult.tab";

KGPostRenderTestingMgr::KGPostRenderTestingMgr(void)
{
	m_hEngineDLL	= NULL;
	m_pEngineMgr	= NULL;
	m_hRenderWindow = NULL;
	m_szClientPath[0] = '\0';
	m_szResultPath[0] = '\0';
	m_nFrame = 0;
	m_bKMemoryInited = false;
}

KGPostRenderTestingMgr::~KGPostRenderTestingMgr(void)
{
	UnInit();
}

int KGPostRenderTestingMgr::Init(const TCHAR cszClientPath[], const HWND chWindow)
{
	int nRetCode = false;
	int nResult  = false;
	HRESULT hrRetCode = E_FAIL;

	_ASSERTE(chWindow);

	if (!m_bKMemoryInited)
	{
		nRetCode = KMemory::Initialize("KGPostRenderTesting.memory");
		KGLOG_PROCESS_ERROR(nRetCode);
		m_bKMemoryInited = true;
	}
	if (cszClientPath[0] == '\0')
	{
		nRetCode = strncpy_s(m_szClientPath, sizeof(m_szClientPath), g_szApplicationPath, strlen(g_szApplicationPath));
		KGLOG_PROCESS_ERROR(nRetCode == 0);
	}
	else
	{
		nRetCode = strncpy_s(m_szClientPath, sizeof(m_szClientPath), cszClientPath, strlen(cszClientPath));
		KGLOG_PROCESS_ERROR(nRetCode == 0);
	}

	nRetCode = KGTestCommon::FormatPath(m_szClientPath, sizeof(m_szClientPath));
	KGLOG_PROCESS_ERROR(nRetCode);

	m_hRenderWindow = chWindow;

	//加载引擎动态库
	nRetCode = LoadEngineDLL();
	KGLOG_PROCESS_ERROR(nRetCode);

	hrRetCode = m_pEngineMgr->GetEngineOption(&m_EngineOption);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	nRetCode = KGTestCommon::MakeResultPath(TEST_PROJECT_NAME, m_szResultPath, sizeof(m_szResultPath));
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = LoadTestCaseFromFile();
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	if (!nResult)
	{
		UnInit();
	}
	return nResult;
}

int KGPostRenderTestingMgr::LoadEngineDLL()
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hrRetCode = E_FAIL;
	TCHAR szDllFile[MAX_PATH] = {0};

	g_SetRootPath(m_szClientPath);
	SetCurrentDirectory(m_szClientPath);

	nRetCode = _snprintf_s(
		szDllFile,
		sizeof(szDllFile) - 1,        
		"%s%s",
		g_szApplicationPath,
		ENGINE_DLL_NAME
		);
	KGLOG_PROCESS_ERROR(nRetCode > 0);

	nRetCode = _access(szDllFile, 0);
	if (nRetCode != 0)
	{
		nRetCode = _snprintf_s(
			szDllFile,
			sizeof(szDllFile) - 1,        
			"%s%s",
			m_szClientPath,
			ENGINE_DLL_NAME
			);
		KGLOG_PROCESS_ERROR(nRetCode > 0);
	}
	m_hEngineDLL = LoadLibrary(szDllFile);
	KGLOG_PROCESS_ERROR(m_hEngineDLL);

	typedef HRESULT (*pfnGet3DEngineInterface)(void** ppvEngineManager);
	pfnGet3DEngineInterface Get3DEngineInterface;

	Get3DEngineInterface = (pfnGet3DEngineInterface)GetProcAddress(m_hEngineDLL, "Get3DEngineInterface");
	Get3DEngineInterface((void**)&m_pEngineMgr);
	KGLOG_PROCESS_ERROR(m_pEngineMgr);

	hrRetCode = m_pEngineMgr->Init(0, KG3DENG_CLIENT, m_hRenderWindow, m_hRenderWindow);
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

void KGPostRenderTestingMgr::UnInit()
{
	int nRetCode = false;
	HRESULT hrRetCode = E_FAIL;

	for (size_t i = 0; i < m_vecTestCases.size(); i++)
	{
		if (m_vecTestCases[i])
		{
			m_vecTestCases[i]->UnInit();
			SAFE_DELETE(m_vecTestCases[i]);
		}
	}
	m_vecTestCases.clear();
	m_vecEffectType.clear();
	m_vecTestFrame.clear();

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
	if (m_bKMemoryInited)
	{
		KMemory::Finalize();
		m_bKMemoryInited = false;
	}
}

int KGPostRenderTestingMgr::Run()
{
	HRESULT hrRetCode = E_FAIL;
	int nRetCode	  = false;
	int nResult		  = false;
	

 	for (size_t i = 0; i < m_vecTestCases.size(); i++)
 	{
 		KGTestEffect* pTestEffect = NULL;
 		pTestEffect = m_vecTestCases[i];
 		if (pTestEffect)
 		{
 			nRetCode = pTestEffect->Run();
			if (!nRetCode)
			{
				KGLogPrintf(KGLOG_ERR, "%s Testing run failed.", pTestEffect->GetCaseName().c_str());
			}
 			pTestEffect->UnInit();
 			SAFE_DELETE(pTestEffect);
 			hrRetCode = m_pEngineMgr->SetEngineOption(&m_EngineOption);
 			KGLOG_COM_PROCESS_ERROR(hrRetCode);
 		}
 	}
 	m_vecTestCases.clear();

	nResult = true;
Exit0:
	return nResult;
}

int KGPostRenderTestingMgr::LoadTestCaseFromFile()
{
	int nRetCode = false;
	int nResult  = false;
	DWORD dwResult = 0;
	int nCaseIndex = 1;
	TCHAR szConfigFile[MAX_PATH] = {0};
	TCHAR szResultFile[MAX_PATH] = {0};
	TCHAR szDefaultMap[MAX_PATH] = {0};
	int nDefaultTotalTestFrame = 0;
	D3DXVECTOR3 vDefaultCameraPos = D3DXVECTOR3(0, 0, 0);
	D3DXVECTOR3 vDefaultLookAtPos = D3DXVECTOR3(0, 0, 0);
	string strFileName;

	//配置文件
	nRetCode = _sntprintf_s(
		szConfigFile,
		sizeof(szConfigFile),
		sizeof(szConfigFile) - 1,
		_T("%sCaseConfig.ini"),
		g_szApplicationPath
		);
	KGLOG_PROCESS_ERROR(nRetCode > 0);

	//读取默认设置
	float fPos = 0;
	TCHAR szFloatValue[MAX_PATH] = {0};
	//地图场景
	::GetPrivateProfileString("DefaultSetting", _T("TestMap"), "", szDefaultMap, sizeof(szDefaultMap), szConfigFile);
	//测试总帧数
	nDefaultTotalTestFrame = ::GetPrivateProfileInt("DefaultSetting", _T("TotalTestFrame"), 0, szConfigFile);
	KGLOG_PROCESS_ERROR(nDefaultTotalTestFrame > 0);
	//摄像机位置
	::GetPrivateProfileString("DefaultSetting", "CameraPosX", "0.0", szFloatValue, sizeof(szFloatValue), szConfigFile);
	sscanf(szFloatValue, "%f", &fPos);
	vDefaultCameraPos.x = fPos;

	::GetPrivateProfileString("DefaultSetting", "CameraPosY", "0.0", szFloatValue, sizeof(szFloatValue), szConfigFile);
	sscanf(szFloatValue, "%f", &fPos);
	vDefaultCameraPos.y = fPos;

	::GetPrivateProfileString("DefaultSetting", "CameraPosZ", "0.0", szFloatValue, sizeof(szFloatValue), szConfigFile);
	sscanf(szFloatValue, "%f", &fPos);
	vDefaultCameraPos.z = fPos;
	//摄像机朝向
	::GetPrivateProfileString("DefaultSetting", "LookAtPosX", "0.0", szFloatValue, sizeof(szFloatValue), szConfigFile);
	sscanf(szFloatValue, "%f", &fPos);
	vDefaultLookAtPos.x = fPos;

	::GetPrivateProfileString("DefaultSetting", "LookAtPosY", "0.0", szFloatValue, sizeof(szFloatValue), szConfigFile);
	sscanf(szFloatValue, "%f", &fPos);
	vDefaultLookAtPos.y = fPos;

	::GetPrivateProfileString("DefaultSetting", "LookAtPosZ", "0.0", szFloatValue, sizeof(szFloatValue), szConfigFile);
	sscanf(szFloatValue, "%f", &fPos);
	vDefaultLookAtPos.z = fPos;

	//读取用例配置
	for (;;nCaseIndex++)
	{
		KGTestEffect* pTestCase = NULL;
		int nTypeIndex		= 1;
		int nFrameIndex		= 1;
		int nTotalTestFrame = 0;
		DWORD dwEffectType	= 0;
		TCHAR szMapFullPath[MAX_PATH] = {0};
		TCHAR szTestMapName[MAX_PATH] = {0};
		TCHAR szStringValue[MAX_PATH] = {0};
		TCHAR szCaseName[MAX_PATH]	  = {0};
		string strTestMapName;

		nRetCode = _sntprintf_s(
			szCaseName,
			sizeof(szCaseName),
			sizeof(szCaseName) - 1,
			_T("Case_%d"),
			nCaseIndex
			);
		KGLOG_PROCESS_ERROR(nRetCode > 0);

		//读取特效类型EffectType_n
		for (;;nTypeIndex++)
		{
			TCHAR szTypeName[MAX_PATH] = {0};
			nRetCode = _sntprintf_s(
				szTypeName,
				sizeof(szTypeName),
				sizeof(szTypeName) - 1,
				_T("EffectType_%d"),
				nTypeIndex
				);
			KGLOG_PROCESS_ERROR(nRetCode > 0);
			dwResult = ::GetPrivateProfileString(szCaseName, szTypeName, "", szStringValue, sizeof(szStringValue), szConfigFile);
			if(!dwResult && nTypeIndex != 1)
			{
				break;
			}
			else if (!dwResult && nTypeIndex == 1)
			{
				KG_PROCESS_SUCCESS(true);
			}

			nRetCode = MakeupEffectType(szStringValue, &dwEffectType);
			KGLOG_PROCESS_ERROR(nRetCode);
		}
		if (nTypeIndex > 2)
		{
			strFileName = MUTIL_EFFECT_RESULT_FILE;
		}
		else
		{
			strFileName = ONE_EFFECT_RESULT_FILE;
		}
	
		//读取测试地图名称
		dwResult = ::GetPrivateProfileString(szCaseName, _T("TestMap"), "", szTestMapName, sizeof(szTestMapName), szConfigFile);
		if (szTestMapName[0] == '\0')
		{
			if (szDefaultMap[0] == '\0')
			{
				KGLOG_PROCESS_ERROR(false);
			}
			else
			{
				strTestMapName = szDefaultMap;
			}
		}
		else
		{
			strTestMapName = szTestMapName;
		}

		nTotalTestFrame = ::GetPrivateProfileInt(szCaseName, _T("TotalTestFrame"), 0, szConfigFile);
		if (nTotalTestFrame == 0)
		{
			nTotalTestFrame = nDefaultTotalTestFrame;
		}

		
		//创建并初始化特效测试用例
		pTestCase = new KGTestEffect();
		KGLOG_PROCESS_ERROR(pTestCase);
		m_vecTestCases.push_back(pTestCase);
		pTestCase->SetDefaultCameraPos(vDefaultCameraPos);
		pTestCase->SetDefaultLookAtPos(vDefaultLookAtPos);
		//地图名称
		nRetCode = _sntprintf_s(
			szMapFullPath,
			sizeof(szMapFullPath),
			sizeof(szMapFullPath) - 1,
			_T("%s%s"),
			m_szClientPath,
			strTestMapName.c_str()
			);
		KGLOG_PROCESS_ERROR(nRetCode > 0);

		//输出文件
		nRetCode = _sntprintf_s(
			szResultFile,
			sizeof(szResultFile),
			sizeof(szResultFile) - 1,
			_T("%s%s"),
			m_szResultPath,
			strFileName.c_str()
			);
		KGLOG_PROCESS_ERROR(nRetCode > 0);

		nRetCode = pTestCase->Init(m_pEngineMgr, m_hRenderWindow, szResultFile, szMapFullPath, dwEffectType, nTotalTestFrame);
		KGLOG_PROCESS_ERROR(nRetCode);
		//读取每个测试帧的配置（TestFrame_n）
		for (;;nFrameIndex++)
		{
			int nTestFrame = 0;
			string strKey;
			float fCameraPosX = 0.0f;
			float fCameraPosY = 0.0f;
			float fCameraPosZ = 0.0f;
			float fLookAtPosX = 0.0f;
			float fLookAtPosY = 0.0f;
			float fLookAtPosZ = 0.0f;
			TCHAR szFloatValue[MAX_PATH] = {0};
			TCHAR szFrameName[MAX_PATH] = {0};
			nRetCode = _sntprintf_s(
				szFrameName,
				sizeof(szFrameName),
				sizeof(szFrameName) - 1,
				_T("TestFrame_%d"),
				nFrameIndex
				);
			KGLOG_PROCESS_ERROR(nRetCode > 0);
			nTestFrame = ::GetPrivateProfileInt(szCaseName, szFrameName, -1, szConfigFile);
			if(nTestFrame == -1)
			{
				break;
			}
			KGLOG_PROCESS_ERROR(nTestFrame > 0);

			strKey = string(_T("CameraPosX_")) + KGTestCommon::IntToString(nFrameIndex);
			::GetPrivateProfileString(szCaseName, strKey.c_str(), "0.0", szFloatValue, sizeof(szFloatValue), szConfigFile);
			sscanf(szFloatValue, "%f", &fCameraPosX);

			strKey = string(_T("CameraPosY_")) + KGTestCommon::IntToString(nFrameIndex);
 			::GetPrivateProfileString(szCaseName, strKey.c_str(), "0.0", szFloatValue, sizeof(szFloatValue), szConfigFile);
 			sscanf(szFloatValue, "%f", &fCameraPosY);
 
			strKey = string(_T("CameraPosZ_")) + KGTestCommon::IntToString(nFrameIndex);
 			::GetPrivateProfileString(szCaseName, strKey.c_str(), "0.0", szFloatValue, sizeof(szFloatValue), szConfigFile);
 			sscanf(szFloatValue, "%f", &fCameraPosZ);
 
			strKey = string(_T("LookAtPosX_")) + KGTestCommon::IntToString(nFrameIndex);
 			::GetPrivateProfileString(szCaseName, strKey.c_str(), "0.0", szFloatValue, sizeof(szFloatValue), szConfigFile);
 			sscanf(szFloatValue, "%f", &fLookAtPosX);
 
			strKey = string(_T("LookAtPosY_")) + KGTestCommon::IntToString(nFrameIndex);
 			::GetPrivateProfileString(szCaseName, strKey.c_str(), "0.0", szFloatValue, sizeof(szFloatValue), szConfigFile);
 			sscanf(szFloatValue, "%f", &fLookAtPosY);
 
			strKey = string(_T("LookAtPosZ_")) + KGTestCommon::IntToString(nFrameIndex);
 			::GetPrivateProfileString(szCaseName, strKey.c_str(), "0.0", szFloatValue, sizeof(szFloatValue), szConfigFile);
 			sscanf(szFloatValue, "%f", &fLookAtPosZ);

			nRetCode = pTestCase->AddCaseData(
				nTestFrame,
				D3DXVECTOR3(fCameraPosX, fCameraPosY, fCameraPosZ),
				D3DXVECTOR3(fLookAtPosX, fLookAtPosY, fLookAtPosZ)
			);
			KGLOG_PROCESS_ERROR(nRetCode);
		}
	}
	
Exit1:
	nResult = true;
Exit0:
	if (!nResult)
	{
		for (size_t i = 0; i < m_vecTestCases.size(); i++)
		{
			if (m_vecTestCases[i])
			{
				m_vecTestCases[i]->UnInit();
				SAFE_DELETE(m_vecTestCases[i]);
			}
		}
		m_vecTestCases.clear();
	}
	return nResult;
}

int KGPostRenderTestingMgr::MakeupEffectType(const TCHAR cszTypeName[], DWORD* dwTypeValue)
{
	int nRetCode = false;
	int nResult  = false;
	TCHAR szTypeName[MAX_PATH] = {0};

	_ASSERTE(cszTypeName);
	KG_PROCESS_ERROR(cszTypeName[0] != '\0');

	nRetCode = _tcsncpy_s(szTypeName, sizeof(szTypeName), cszTypeName, strlen(cszTypeName));
	KGLOG_PROCESS_ERROR(nRetCode == 0);

	nRetCode = _mbslwr_s((unsigned char *)szTypeName, sizeof(szTypeName));//筞
	KGLOG_PROCESS_ERROR(nRetCode == 0);
	
	if (_tcscmp(szTypeName, "no_effect") == 0)
	{
		(*dwTypeValue) = 0;
	}
	else if (_tcscmp(szTypeName, "hdr") == 0)
	{
		(*dwTypeValue) |= POST_EFFECT_HDR;
	}
	else if (_tcscmp(szTypeName, "bloom") == 0)
	{
		(*dwTypeValue) |= POST_EFFECT_BLOOM;
	}
	else if (_tcscmp(szTypeName, "shockwave") == 0)
	{
		(*dwTypeValue) |= POST_EFFECT_SOCKWARE;
	}
	else if (_tcscmp(szTypeName, "shadow") == 0)
	{
		(*dwTypeValue) |= POST_EFFECT_SHADOW;
	}
	else if (_tcscmp(szTypeName, "dof") == 0)
	{
		(*dwTypeValue) |= POST_EFFECT_DOF;
	}
	else if (_tcscmp(szTypeName, "godray") == 0)
	{
		(*dwTypeValue) |= POST_EFFECT_GODRAY;
	}
	else if (_tcscmp(szTypeName, "aoe") == 0)
	{
		(*dwTypeValue) |= POST_EFFECT_AOE;
	}
	else
	{
		_ASSERTE(false);
	}

	nResult = true;
Exit0:
	return nResult;
}