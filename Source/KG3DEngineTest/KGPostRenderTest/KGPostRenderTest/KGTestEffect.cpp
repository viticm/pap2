#include "KGTestEffect.h"
#include "IEEditor.h"
#include "IEKG3DScene.h"

const int VALID_BEGIN_FRAME = 100;		//第100帧后才算是有效的测试数据（先渲染100帧后再进行数据收集）
int KGTestEffect::m_nResultLine = 1;

KGTestEffect::KGTestEffect()
{
	m_nCurrentFrame			= 1;
	m_dwEffectType			= 0;
	m_dwTestingBeginTime	= 0;
	m_nTestFrameIndex		= 0;
	m_szTestSceneName[0]	= _T('\0');
	m_szResultFile[0]		= _T('\0');
	m_pEngineMgr	= NULL;
	m_pScene		= NULL;
	m_pCurOutputWnd = NULL;
	m_hRenderWindow = NULL;
	m_pFile			= NULL;
	m_CaseData.nDefaultTotalTestFrame = 500;
	m_CaseData.vDefaultCameraPos = D3DXVECTOR3(0, 0, 0);
	m_CaseData.vDefaultLookAtPos = D3DXVECTOR3(0, 0, 0);
	m_CaseData.vecTestFrame.clear();
	m_CaseData.vecCameraPos.clear();
	m_CaseData.vecLookAtPos.clear();
	memset(&m_CurrentFrameData, 0, sizeof(FrameData));
}

KGTestEffect::~KGTestEffect()
{
	UnInit();
}

int  KGTestEffect::Init(IKG3DEngineManager*		cpEngineMgr,
						const HWND				chRenderWindow,
						const TCHAR				cszResultFile[],
						const TCHAR				cszMapName[],
						const DWORD				cdwEffectType,
						const int				cnTotalFrame)
{
	int nRetCode = false;
	int nResult  = false;
	TCHAR* pszResult = NULL;

	KG_ASSERT_EXIT(cpEngineMgr);
	_ASSERTE(cszMapName);
	_ASSERTE(cszResultFile);

	nRetCode = _tcsncpy_s(m_szTestSceneName, sizeof(m_szTestSceneName), cszMapName, _tcslen(cszMapName));
	KGLOG_PROCESS_ERROR(nRetCode == 0);
	nRetCode = _tcsncpy_s(m_szResultFile, sizeof(m_szTestSceneName), cszResultFile, _tcslen(cszResultFile));
	KGLOG_PROCESS_ERROR(nRetCode == 0);

	nRetCode = _tcsncpy_s(m_szResultPath, sizeof(m_szResultPath), cszResultFile, _tcslen(cszResultFile));
	KGLOG_PROCESS_ERROR(nRetCode == 0);
	pszResult = _tcsrchr(m_szResultPath, _T('\\'));
	KGLOG_PROCESS_ERROR(pszResult);
	pszResult++;
	(*pszResult) = _T('\0');
	pszResult = NULL;

	m_dwEffectType = cdwEffectType;
	m_pEngineMgr = cpEngineMgr;
	m_hRenderWindow = chRenderWindow;
	if (cnTotalFrame > VALID_BEGIN_FRAME)
	{
		m_CaseData.nDefaultTotalTestFrame = cnTotalFrame;
	}

	nRetCode = MakeupCaseName();
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}	

int KGTestEffect::Run()
{
	HRESULT hrRetCode = E_FAIL;
	int nRetCode = false;
	int nResult  = false;
	MSG msg = {0};

	nRetCode = LoadScene();
	KGLOG_PROCESS_ERROR(nRetCode);
	nRetCode = DisableAllEffect();
	KGLOG_PROCESS_ERROR(nRetCode);
 	nRetCode = EnableEffect(true);
 	KGLOG_PROCESS_ERROR(nRetCode);
	//检查特效设置情况
	nRetCode = TestSetEffectProcess();
	KGLOG_PROCESS_ERROR(nRetCode);

	//消息循环
	while (m_nCurrentFrame != m_CaseData.nDefaultTotalTestFrame)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			switch (msg.message)
			{
			case WM_QUIT:
				KG_PROCESS_SUCCESS(true);
			default:
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		//帧前事件
		nRetCode = BeforeFrame();
		KGLOG_PROCESS_ERROR(nRetCode);
		//渲染帧
		nRetCode = OnFrameMove();
		KGLOG_PROCESS_ERROR(nRetCode);
		//帧后事件
		nRetCode = AfterFrame();
		KGLOG_PROCESS_ERROR(nRetCode);

		m_nCurrentFrame++;
	}

Exit1:
	nResult = true;
Exit0:
	nRetCode = DisableAllEffect();
	KGLOG_CHECK_ERROR(nRetCode);
	nRetCode = RemoveScene();
	KGLOG_CHECK_ERROR(nRetCode);
	return nResult;
}

int KGTestEffect::AnalyseFrameData()
{
	int nRetCode = false;
	int nResult  = false;

	vector<int>::iterator iter;
	iter = find(m_CaseData.vecTestFrame.begin(), m_CaseData.vecTestFrame.end(), m_nCurrentFrame);
	if (iter != m_CaseData.vecTestFrame.end())
	{
		//Frame Data Collection
		m_CurrentFrameData.nFrameCount	= m_nCurrentFrame;
		m_CurrentFrameData.nAverageFPS	= m_nCurrentFrame / ((m_CurrentFrameData.dwEndTime - m_dwTestingBeginTime) / 1000.0f) ;
		m_CurrentFrameData.nFPS			= 1.0f / ((m_CurrentFrameData.dwEndTime - m_CurrentFrameData.dwBeginTime) / 1000.0f);
		m_CurrentFrameData.dwLastTime	= m_CurrentFrameData.dwEndTime - m_CurrentFrameData.dwBeginTime;

		nRetCode = OutputResult();
		KGLOG_PROCESS_ERROR(nRetCode);
	}

	nResult = true;
Exit0:
	return nResult;
}

int KGTestEffect::OutputResult()
{
	int nRetCode = false;
	int nResult  = false;
	HRESULT hrRetCode = E_FAIL;
	TCHAR szOutput[MAX_PATH * 2] = {0};
	TCHAR szPicName[MAX_PATH]    = {0};
	TCHAR szResultPath[MAX_PATH] = {0};

	_ASSERTE(m_CurrentFrameData.nFrameCount != 0);

	//Screenshot
	nRetCode = _sntprintf_s(
		szPicName,
		sizeof(szPicName),
		sizeof(szPicName) - 1,
		_T("%s%s_Frame_%d.jpg"),
		m_szResultPath,
		m_strCaseName.c_str(),
		m_CurrentFrameData.nFrameCount
		);
	//Picture Name(e.g. D:\Result\HDR_Frame_100.jpg)
	KGLOG_PROCESS_ERROR(nRetCode > 0);
	hrRetCode = m_pEngineMgr->Get3DUI()->ScreenShot(szPicName, D3DXIFF_JPG, 100);
	KGLOG_COM_PROCESS_ERROR(nRetCode);

	if (m_nTestFrameIndex == 0)//第一个测试帧
	{
		m_pFile = g_OpenTabFile(m_szResultFile, true, true);
		KGLOG_PROCESS_ERROR(m_pFile);

		nRetCode = _sntprintf_s(
			szOutput,
			sizeof(szOutput),
			sizeof(szOutput) - 1,
			_T("\n%s Testing Result\n"),
			m_strCaseName.c_str()
			);
		KGLOG_PROCESS_ERROR(nRetCode > 0);

		nRetCode = m_pFile->WriteString(m_nResultLine, 1, szOutput, (UINT)_tcslen(szOutput));
		KGLOG_PROCESS_ERROR(nRetCode);
		m_nResultLine++;
		for (size_t i = 0; i < m_vecTestProcessResult.size(); i++)
		{
			nRetCode = m_pFile->WriteString(m_nResultLine, 1, m_vecTestProcessResult[i].c_str(), (UINT)m_vecTestProcessResult[i].length());
			KGLOG_PROCESS_ERROR(nRetCode);
			m_nResultLine++;
		}
		nRetCode = m_pFile->WriteString(m_nResultLine, 1, _T("Frame"), (UINT)_tcslen(_T("Frame")));
		KGLOG_PROCESS_ERROR(nRetCode);
		nRetCode = m_pFile->WriteString(m_nResultLine, 2, _T("Average(FPS)"), (UINT)_tcslen(_T("Average(FPS)")));
		KGLOG_PROCESS_ERROR(nRetCode);
		nRetCode = m_pFile->WriteString(m_nResultLine, 3, _T("Current(FPS)"), (UINT)_tcslen(_T("Current(FPS)")));
		KGLOG_PROCESS_ERROR(nRetCode);
		nRetCode = m_pFile->WriteString(m_nResultLine, 4, _T("BeginTime(ms)"), (UINT)_tcslen(_T("BeginTime(ms)")));
		KGLOG_PROCESS_ERROR(nRetCode);
		nRetCode = m_pFile->WriteString(m_nResultLine, 5, _T("LastTime(ms)"), (UINT)_tcslen(_T("LastTime(ms)")));
		KGLOG_PROCESS_ERROR(nRetCode);
		nRetCode = m_pFile->WriteString(m_nResultLine, 6, _T("EndTime(ms)"), (UINT)_tcslen(_T("EndTime(ms)")));
		KGLOG_PROCESS_ERROR(nRetCode);
		nRetCode = m_pFile->WriteString(m_nResultLine, 7, _T("Screenshot"), (UINT)_tcslen(_T("Screenshot")));
		KGLOG_PROCESS_ERROR(nRetCode);
		m_nResultLine++;
	}

	//Frame-Data result write
	nRetCode = m_pFile->WriteInteger(m_nResultLine, 1, m_CurrentFrameData.nFrameCount);
	KGLOG_PROCESS_ERROR(nRetCode);
	nRetCode = m_pFile->WriteFloat(m_nResultLine, 2, m_CurrentFrameData.nAverageFPS);
	KGLOG_PROCESS_ERROR(nRetCode);
	nRetCode = m_pFile->WriteFloat(m_nResultLine, 3, m_CurrentFrameData.nFPS);
	KGLOG_PROCESS_ERROR(nRetCode);
	nRetCode = m_pFile->WriteInteger(m_nResultLine, 4, m_CurrentFrameData.dwBeginTime);
	KGLOG_PROCESS_ERROR(nRetCode);
	nRetCode = m_pFile->WriteInteger(m_nResultLine, 5, m_CurrentFrameData.dwLastTime);
	KGLOG_PROCESS_ERROR(nRetCode);
	nRetCode = m_pFile->WriteInteger(m_nResultLine, 6, m_CurrentFrameData.dwEndTime);
	KGLOG_PROCESS_ERROR(nRetCode);
	nRetCode = _sntprintf_s(
		szPicName,
		sizeof(szPicName),
		sizeof(szPicName) - 1,
		_T("%s_Frame_%d.jpg"),
		m_strCaseName.c_str(),
		m_CurrentFrameData.nFrameCount
		);
	nRetCode = m_pFile->WriteString(m_nResultLine, 7, szPicName, (UINT)_tcslen(szPicName));
	KGLOG_PROCESS_ERROR(nRetCode);
	m_nResultLine++;
	//Save result
	nRetCode = m_pFile->Save(m_szResultFile);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	m_nTestFrameIndex++;
	memset(&m_CurrentFrameData, 0, sizeof(FrameData));
	if (!nResult)
	{
		if (m_pFile)
		{
			m_pFile->Release();
			m_pFile = NULL;
		}
	}
	return nResult;
}

int KGTestEffect::MakeupCaseName()
{
	int nRetCode = false;
	int nResult  = false;

	m_strCaseName.clear();

	if (m_dwEffectType == 0)
	{
		m_strCaseName = _T("No_Effect");
	}
	if (HasEffect(POST_EFFECT_HDR))
	{
		m_strCaseName += _T("HDR");
	}
	if (HasEffect(POST_EFFECT_BLOOM))
	{
		if (!m_strCaseName.empty())
		{
			m_strCaseName += _T("_");
		}
		m_strCaseName += _T("BLOOM");
	}
	if (HasEffect(POST_EFFECT_SHADOW))
	{
		if (!m_strCaseName.empty())
		{
			m_strCaseName += _T("_");
		}
		m_strCaseName += _T("SHADOW");
	}
	if (HasEffect(POST_EFFECT_SOCKWARE))
	{
		if (!m_strCaseName.empty())
		{
			m_strCaseName += _T("_");
		}
		m_strCaseName += _T("SHOCKWARE");
	}
	if (HasEffect(POST_EFFECT_GODRAY))
	{
		if (!m_strCaseName.empty())
		{
			m_strCaseName += _T("_");
		}
		m_strCaseName += _T("GODRAY");
	}
	if (HasEffect(POST_EFFECT_DOF))
	{
		if (!m_strCaseName.empty())
		{
			m_strCaseName += _T("_");
		}
		m_strCaseName += _T("DOF");
	}
	if (HasEffect(POST_EFFECT_AOE))
	{
		if (!m_strCaseName.empty())
		{
			m_strCaseName += _T("_");
		}
		m_strCaseName += _T("AOE");
	}

	nResult = true;
Exit0:
	if (!nResult)
	{
		m_strCaseName.clear();
	}
	return nResult;
}

int KGTestEffect::LoadScene()
{
	int nRetCode	  = false;
	int nResult		  = false;
	HRESULT hrRetCode = E_FAIL;

	IEKG3DScene* pIEScene			  = NULL;
	IEKG3DSceneSceneEditor* pIEEditor = NULL;
	int nOutputWindowID = 0;
	KG3DSCENE_INITSTRUCT SceneStruct;

#if defined(DEBUG) | defined(_DEBUG)
	KG3D_GAME_INIT_CONTEXT Context;
	m_pEngineMgr->InitGameContext(&Context);
#endif

	hrRetCode = m_pEngineMgr->NewOneScene(SCENETYPE_SCENEEDITOR, &m_pScene);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	hrRetCode = m_pScene->LoadFromFileEx(
		m_szTestSceneName,
		0, 0, 
		reinterpret_cast<DWORD_PTR>(&SceneStruct), 0);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	hrRetCode = m_pScene->AddOutputWindow(_T("Test Scene"), m_hRenderWindow, 0, 0, &nOutputWindowID);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	pIEScene = (IEKG3DScene*)m_pScene;
	hrRetCode = pIEScene->GetSceneSceneEditor(&pIEEditor);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	hrRetCode = pIEEditor->GetCurOutputWnd(&m_pCurOutputWnd);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	nResult = true;
Exit0:
	if (!nResult)
	{
		if (m_pScene)
		{
			m_pEngineMgr->RemoveScene(m_pScene);
			m_pScene = NULL;
		}
	}
	return nResult;
}

int KGTestEffect::RemoveScene()
{
	HRESULT hrRetCode = E_FAIL;
	int nResult  = false;

	if (m_pScene)
	{
		hrRetCode = m_pEngineMgr->RemoveScene(m_pScene);
		KGLOG_COM_PROCESS_ERROR(hrRetCode);
		m_pScene = NULL;
	}

	nResult = true;
Exit0:
	return nResult;
}

int KGTestEffect::EnableEffect(bool bEnable)
{
	int nResult		  = false;
	HRESULT hrRetCode = E_FAIL;
 	hrRetCode = m_pCurOutputWnd->ResetDevice(bEnable, m_pCurOutputWnd->GetViewPort()->Width, m_pCurOutputWnd->GetViewPort()->Height);
 	KGLOG_COM_PROCESS_ERROR(hrRetCode);
	if (HasEffect(POST_EFFECT_HDR))
	{
		hrRetCode = m_pCurOutputWnd->EnablePostEffect(POST_EFFECT_HDR, bEnable);
		KGLOG_COM_PROCESS_ERROR(hrRetCode);
	}
	if (HasEffect(POST_EFFECT_BLOOM))
	{
		hrRetCode = m_pCurOutputWnd->EnablePostEffect(POST_EFFECT_BLOOM, bEnable);
		KGLOG_COM_PROCESS_ERROR(hrRetCode);
	}
	if (HasEffect(POST_EFFECT_SHADOW))
	{
		hrRetCode = m_pCurOutputWnd->EnablePostEffect(POST_EFFECT_SHADOW, bEnable);
		KGLOG_COM_PROCESS_ERROR(hrRetCode);
	}
	if (HasEffect(POST_EFFECT_SOCKWARE))
	{
		hrRetCode = m_pCurOutputWnd->EnablePostEffect(POST_EFFECT_SOCKWARE, bEnable);
		KGLOG_COM_PROCESS_ERROR(hrRetCode);
	}
	if (HasEffect(POST_EFFECT_GODRAY))
	{
		hrRetCode = m_pCurOutputWnd->EnablePostEffect(POST_EFFECT_GODRAY, bEnable);
		KGLOG_COM_PROCESS_ERROR(hrRetCode);
	}
	if (HasEffect(POST_EFFECT_DOF))
	{
		hrRetCode = m_pCurOutputWnd->EnablePostEffect(POST_EFFECT_DOF, bEnable);
		KGLOG_COM_PROCESS_ERROR(hrRetCode);
	}
	if (HasEffect(POST_EFFECT_AOE))
	{
		hrRetCode = m_pCurOutputWnd->EnablePostEffect(POST_EFFECT_AOE, bEnable);
		KGLOG_COM_PROCESS_ERROR(hrRetCode);
	}

	nResult = true;
Exit0:
	return nResult;
}

void KGTestEffect::UnInit()
{
	if (m_pFile)
	{
		m_pFile->Release();
		m_pFile = NULL;
	}
}

int KGTestEffect::SetupScene()
{
	int nRetCode		  = false;
	int nResult			  = false;
	HRESULT hrRetCode	  = E_FAIL;
	IKG3DCamera* piCamera = NULL;

	vector<int>::iterator iter;
	iter = find(m_CaseData.vecTestFrame.begin(), m_CaseData.vecTestFrame.end(), m_nCurrentFrame);
	if (iter != m_CaseData.vecTestFrame.end())
	{
 		piCamera = m_pScene->GetCurrentCamera();
 		KGLOG_PROCESS_ERROR(piCamera);
	 
 		hrRetCode = piCamera->SetPosition(m_CaseData.vecCameraPos[m_nTestFrameIndex], false);
 		KGLOG_COM_PROCESS_ERROR(hrRetCode);
	 
 		hrRetCode = piCamera->SetLookAtPosition(m_CaseData.vecLookAtPos[m_nTestFrameIndex], false);
 		KGLOG_COM_PROCESS_ERROR(hrRetCode);
	}

	nResult = true;
Exit0:
	return nResult;
}

int KGTestEffect::AddCaseData(int nTestFrame,
							  D3DXVECTOR3 vCameraPos,
							  D3DXVECTOR3 vLookAtPos)
{
	int nResult = false;
	
	KGLOG_PROCESS_ERROR(nTestFrame > VALID_BEGIN_FRAME);
	m_CaseData.vecTestFrame.push_back(nTestFrame);
	if (vCameraPos != D3DXVECTOR3(0, 0, 0))
	{
		m_CaseData.vDefaultCameraPos = vCameraPos;
	}
	m_CaseData.vecCameraPos.push_back(m_CaseData.vDefaultCameraPos);
	if (vLookAtPos != D3DXVECTOR3(0, 0, 0))
	{
		m_CaseData.vDefaultLookAtPos = vLookAtPos;
	}
	m_CaseData.vecLookAtPos.push_back(m_CaseData.vDefaultLookAtPos);

	nResult = true;
Exit0:
	return nResult;
}

int KGTestEffect::TestSetEffectProcess()
{		
	int nResult = false;
	HRESULT hrRetCode = E_FAIL;
	KG3DEngineOption EngineOption;

	hrRetCode = m_pEngineMgr->GetEngineOption(&EngineOption);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	if (EngineOption.bHDR)
	{
		m_vecTestProcessResult.push_back("KG3DEngineOption::bHDR == true");
	}

	if (EngineOption.bBloomEnable)
	{
		m_vecTestProcessResult.push_back("KG3DEngineOption::bBloomEnable == true");
	}

	if (EngineOption.ModelShadowType == EM_MODEL_SHADOW_TYPE_HIGH)
	{
		m_vecTestProcessResult.push_back("KG3DEngineOption::ModelShadowType == EM_MODEL_SHADOW_TYPE_HIGH");
	}

	if (EngineOption.bShockWaveEnable)
	{
		m_vecTestProcessResult.push_back("KG3DEngineOption::bShockWaveEnable == true");
	}

	if (EngineOption.bGodRay)
	{
		m_vecTestProcessResult.push_back("KG3DEngineOption::bGodRay == true");
	}

	if (EngineOption.bDOF)
	{
		m_vecTestProcessResult.push_back("KG3DEngineOption::bDOF == true");
	}

	nResult = true;
Exit0:
	return nResult;
}

int KGTestEffect::BeforeFrame()
{
	int nResult		  = false;
	HRESULT hrRetCode = E_FAIL;

	if (m_nCurrentFrame == VALID_BEGIN_FRAME)
	{
		m_dwTestingBeginTime = timeGetTime();
	}
	hrRetCode = SetupScene();
	KGLOG_COM_PROCESS_ERROR(hrRetCode);
	m_CurrentFrameData.dwBeginTime = timeGetTime();

	nResult = true;
Exit0:
	return nResult;
}

int KGTestEffect::AfterFrame()
{
	int nResult  = false;
	int nRetCode = false;
	
	m_CurrentFrameData.dwEndTime = timeGetTime();
	nRetCode = AnalyseFrameData();
	KGLOG_PROCESS_ERROR(nRetCode);
	
	nResult = true;
Exit0:
	return nResult;
}

int KGTestEffect::DisableAllEffect()
{
	int nResult		  = false;
	HRESULT hrRetCode = E_FAIL;

	hrRetCode = m_pCurOutputWnd->ResetDevice(false, m_pCurOutputWnd->GetViewPort()->Width, m_pCurOutputWnd->GetViewPort()->Height);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	hrRetCode = m_pCurOutputWnd->EnablePostEffect(POST_EFFECT_HDR, false);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	hrRetCode = m_pCurOutputWnd->EnablePostEffect(POST_EFFECT_BLOOM, false);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	hrRetCode = m_pCurOutputWnd->EnablePostEffect(POST_EFFECT_SHADOW, false);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	hrRetCode = m_pCurOutputWnd->EnablePostEffect(POST_EFFECT_SOCKWARE, false);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	hrRetCode = m_pCurOutputWnd->EnablePostEffect(POST_EFFECT_GODRAY, false);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	hrRetCode = m_pCurOutputWnd->EnablePostEffect(POST_EFFECT_DOF, false);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	hrRetCode = m_pCurOutputWnd->EnablePostEffect(POST_EFFECT_AOE, false);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	nResult = true;
Exit0:
	return nResult;
}

int KGTestEffect::OnFrameMove()
{
	int nResult  = false;
	HRESULT hrRetCode = E_FAIL;
	D3DXVECTOR3 vLookat;

	hrRetCode = m_pScene->GetCurrentCamera()->GetLookAtPosition(&vLookat);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);
	hrRetCode = m_pScene->SetFocus(vLookat);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);
	hrRetCode = m_pEngineMgr->FrameMove();
	KGLOG_COM_PROCESS_ERROR(hrRetCode);
	hrRetCode = m_pEngineMgr->Render();
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	nResult = true;
Exit0:
	return nResult;
}