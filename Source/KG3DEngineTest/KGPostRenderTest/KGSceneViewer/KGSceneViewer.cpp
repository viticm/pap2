#include "KGSceneViewer.h"
#include "IEKG3DScene.h"
#include "IEKG3DSceneOutputWnd.h"
#include "IEOther.h"
#include "KMemory.h"

const D3DXVECTOR3 g_cCameraInitPos(5414.52, 189.86, 7758.77);
const D3DXVECTOR3 g_cLookUpInitPos(6297.56, 289.74, 7248.93);

KGSceneViewer::KGSceneViewer(void)
{
	m_bEnableEffect  = false;
	m_bRButton		 = false;
	m_bKMemoryInited = false;
	m_hWindow		= NULL;
	m_pEngineMgr	= NULL;
	m_hEngineDLL	= NULL;
	m_pScene		= NULL;
	m_pCurOutputWnd = NULL;
	m_ptLastMousePos.x	  = 0;
	m_ptLastMousePos.y	  = 0;
	m_ptCurrentMousePos.x = 0;
	m_ptCurrentMousePos.y = 0;
	m_nPictureIndex		  = 0;
	m_szSceneInfo[0]  = '\0';
	m_szResultPath[0] = '\0';
}

KGSceneViewer::~KGSceneViewer(void)
{
	UnInit();
}
int KGSceneViewer::Init(HWND hWindow)
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hRetCode = E_FAIL;

	KGLOG_PROCESS_ERROR(hWindow);
	m_hWindow = hWindow;

	nRetCode = KGTestCommon::MakeResultPath(TEST_PROJECT_NAME, m_szResultPath, sizeof(m_szResultPath));
	KGLOG_PROCESS_ERROR(nRetCode);

	if (!m_bKMemoryInited)
	{
		nRetCode = KMemory::Initialize("KGSceneViewer.memory");
		KGLOG_PROCESS_ERROR(nRetCode);
		m_bKMemoryInited = true;
	}

#if defined(DEBUG) | defined(_DEBUG)
	g_SetRootPath("D:\\TestClient");
	::SetCurrentDirectory("D:\\TestClient");
#else
	g_SetRootPath(g_szApplicationPath);
	::SetCurrentDirectory(g_szApplicationPath);
#endif

  	m_hEngineDLL = LoadLibrary(ENGINE_DLL_NAME);
  	KGLOG_PROCESS_ERROR(m_hEngineDLL);

	typedef HRESULT (*pfnGet3DEngineInterface)(void** ppvEngineManager);
	pfnGet3DEngineInterface Get3DEngineInterface;

	Get3DEngineInterface = (pfnGet3DEngineInterface)GetProcAddress(m_hEngineDLL, "Get3DEngineInterface");
	Get3DEngineInterface((void**)&m_pEngineMgr);
	KGLOG_PROCESS_ERROR(m_pEngineMgr);

	hRetCode = m_pEngineMgr->Init(0, KG3DENG_CLIENT, m_hWindow, hWindow);
	KGLOG_COM_PROCESS_ERROR(hRetCode);

	nResult = true;
Exit0:
	if (!nResult)
	{
		UnInit();
	}
	return nResult;
}

int KGSceneViewer::Run()
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hRetCode = E_FAIL;
	D3DXVECTOR3 vLookat(0, 0, 0);

	if (m_pScene)
	{
		hRetCode = m_pScene->GetCurrentCamera()->GetLookAtPosition(&vLookat);
		KGLOG_COM_PROCESS_ERROR(hRetCode);
		hRetCode = m_pScene->SetFocus(vLookat);
		KGLOG_COM_PROCESS_ERROR(hRetCode);
		hRetCode = m_pEngineMgr->FrameMove();
		KGLOG_COM_PROCESS_ERROR(hRetCode);
		hRetCode = m_pEngineMgr->Render();
		KGLOG_COM_PROCESS_ERROR(hRetCode);
	}

	nResult = true;
Exit0:
	return nResult;
}

int KGSceneViewer::KeyListener(UINT_PTR uKeyValue)
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hRetCode = E_FAIL;
	IKG3DCamera* pICamera = NULL;
	static float s_fCarameSpeedIncDelta = 200.0f;
	D3DXMATRIX Matrix;
	D3DXVECTOR3 CameraPos;
	D3DXVECTOR3 LookAt;
	D3DXVECTOR3 PosDeta(0, 0, 0);
	D3DXVECTOR3 Front(0, 0, 0);
	D3DXVECTOR3 Right(0, 0, 0);
	TCHAR szPicName[MAX_PATH] = {0};

	pICamera = m_pScene->GetCurrentCamera();
	KGLOG_PROCESS_ERROR(pICamera);

	hRetCode = pICamera->GetPosition(&CameraPos);
	KGLOG_COM_PROCESS_ERROR(hRetCode);

	hRetCode = pICamera->GetLookAtPosition(&LookAt);
	KGLOG_COM_PROCESS_ERROR(hRetCode);

	switch (uKeyValue)
	{
	case '1':
		m_bEnableEffect = !m_bEnableEffect;
		hRetCode = m_pCurOutputWnd->EnablePostEffect(POST_EFFECT_HDR, m_bEnableEffect);
		KGLOG_COM_PROCESS_ERROR(hRetCode);
		break;
	case '2':
		m_bEnableEffect = !m_bEnableEffect;
		hRetCode = m_pCurOutputWnd->EnablePostEffect(POST_EFFECT_SHADOW, m_bEnableEffect);
		KGLOG_COM_PROCESS_ERROR(hRetCode);
		break;
	case '3':
		m_bEnableEffect = !m_bEnableEffect;
		hRetCode = m_pCurOutputWnd->EnablePostEffect(POST_EFFECT_GODRAY, m_bEnableEffect);
		KGLOG_COM_PROCESS_ERROR(hRetCode);
		break;
	case '4':
		m_bEnableEffect = !m_bEnableEffect;
		hRetCode = m_pCurOutputWnd->EnablePostEffect(POST_EFFECT_AOE, m_bEnableEffect);
		KGLOG_COM_PROCESS_ERROR(hRetCode);
		break;
	case '5':
		m_bEnableEffect = !m_bEnableEffect;
		hRetCode = m_pCurOutputWnd->EnablePostEffect(POST_EFFECT_SOCKWARE, m_bEnableEffect);
		KGLOG_COM_PROCESS_ERROR(hRetCode);
		break;
	case '6':
		m_bEnableEffect = !m_bEnableEffect;
		hRetCode = m_pCurOutputWnd->EnablePostEffect(POST_EFFECT_DOF, m_bEnableEffect);
		KGLOG_COM_PROCESS_ERROR(hRetCode);
		break;
	case 'C':
		nRetCode = _snprintf_s(
			m_szSceneInfo,
			sizeof(m_szSceneInfo),
			sizeof(m_szSceneInfo) - 1,
			"CameraPosX=%.2f CameraPosY=%.2f CameraPosZ=%.2f "
			"LookAtPosX=%.2f LookAtPosY=%.2f LookAtPosZ=%.2f",
			CameraPos.x,
			CameraPos.y,
			CameraPos.z,
			LookAt.x,
			LookAt.y,
			LookAt.z
		);
		KGLOG_COM_PROCESS_ERROR(nRetCode > 0);

		if (::OpenClipboard(NULL)) 
		{ 
			HGLOBAL hClipbuffer = NULL; 
			TCHAR* pBuffer		= NULL; 

			nRetCode = ::EmptyClipboard();
			KGLOG_PROCESS_ERROR(nRetCode);

			hClipbuffer = ::GlobalAlloc(GMEM_DDESHARE, strlen(m_szSceneInfo) + 1); 
			KGLOG_PROCESS_ERROR(hClipbuffer);

			pBuffer = (char*)::GlobalLock(hClipbuffer); 
			KGLOG_PROCESS_ERROR(pBuffer);

			strcpy(pBuffer, LPCSTR(m_szSceneInfo)); 
			::GlobalUnlock(hClipbuffer); 
			::SetClipboardData(CF_TEXT, hClipbuffer); 
			::CloseClipboard(); 
		} 
		break;
	case 'P':
		nRetCode = _sntprintf_s(
			szPicName,
			sizeof(szPicName),
			sizeof(szPicName) - 1,
			_T("%sTestPic_%d.jpg"),
			m_szResultPath,
			m_nPictureIndex
			);
		KGLOG_PROCESS_ERROR(nRetCode > 0);
		hRetCode = m_pEngineMgr->Get3DUI()->ScreenShot(szPicName, D3DXIFF_JPG, 100);
		KGLOG_COM_PROCESS_ERROR(nRetCode);
		m_nPictureIndex++;
		break;
	case 'A':
		hRetCode = pICamera->GetRight(&Right);
		KGLOG_COM_PROCESS_ERROR(hRetCode);

		D3DXVec3Normalize(&Right, &Right);
		CameraPos += D3DXVECTOR3(Right.x, 0, Right.z) * (-s_fCarameSpeedIncDelta);
		LookAt += D3DXVECTOR3(Right.x, 0, Right.z) * (-s_fCarameSpeedIncDelta);
		break;
	case 'D':
		hRetCode = pICamera->GetRight(&Right);
		KGLOG_COM_PROCESS_ERROR(hRetCode);

		D3DXVec3Normalize(&Right, &Right);
		CameraPos += D3DXVECTOR3(Right.x, 0, Right.z) * s_fCarameSpeedIncDelta;
		LookAt += D3DXVECTOR3(Right.x, 0, Right.z) * s_fCarameSpeedIncDelta;
		break;

	case 'W':
		hRetCode = pICamera->GetFront(&Front);
		KGLOG_COM_PROCESS_ERROR(hRetCode);

		D3DXVec3Normalize(&Front, &Front);
		PosDeta = Front * s_fCarameSpeedIncDelta;
		CameraPos += PosDeta;
		LookAt    += PosDeta;
		break;
	case 'S':
		hRetCode = pICamera->GetFront(&Front);
		KGLOG_COM_PROCESS_ERROR(hRetCode);

		D3DXVec3Normalize(&Front, &Front);
		PosDeta = Front * s_fCarameSpeedIncDelta;
		CameraPos -= PosDeta;
		LookAt    -= PosDeta;
		break;
	default:
		KG_PROCESS_SUCCESS(true);
	}

	hRetCode = pICamera->UpdateCamera(CameraPos, LookAt, 0, 0);
	KGLOG_COM_PROCESS_ERROR(hRetCode);

Exit1:
	nResult = true;
Exit0:
	return nResult;
}

int KGSceneViewer::SetMouseState(UINT uMsg, LPARAM lParam)
{
	int nResult  = false;
	int nRetCode = false;
	int xPos = LOWORD(lParam); 
	int yPos = HIWORD(lParam);

	if (uMsg == WM_RBUTTONDOWN)
	{
		m_bRButton = true;
		m_ptLastMousePos.x = xPos;
		m_ptLastMousePos.y = yPos;
	}
	else if (uMsg == WM_RBUTTONUP)
	{
		m_bRButton = false;
	}
	nResult = true;
Exit0:
	return nResult;
}

int KGSceneViewer::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	int nResult  = false;
	int nRetCode = false;
	int nOffsetX = 0;
	int nOffsetY = 0;
	IKG3DCamera* pICamera = NULL;
	HRESULT hRetCode = E_FAIL;
	D3DXMATRIX Matrix;
	D3DXVECTOR3 vLookAtPos(0, 0, 0);
	D3DXVECTOR3 vRight(0, 0, 0);
	D3DXVECTOR3 vDirection(0, 0, 0);
	D3DXVECTOR3 vUp(0, 0, 0);
	D3DXVECTOR3 vPos(0, 0, 0);

	if(m_bRButton)
	{
		int fwKeys = (int)wParam; 
		int xPos = LOWORD(lParam); 
		int yPos = HIWORD(lParam);

		pICamera = m_pScene->GetCurrentCamera();
		KGLOG_PROCESS_ERROR(pICamera);
		hRetCode = pICamera->GetLookAtPosition(&vLookAtPos);
		KGLOG_COM_PROCESS_ERROR(hRetCode);
		hRetCode = pICamera->GetRight(&vRight);
		KGLOG_COM_PROCESS_ERROR(hRetCode);
		hRetCode = pICamera->GetUpDirection(&vUp);
		KGLOG_COM_PROCESS_ERROR(hRetCode);
		hRetCode = pICamera->GetPosition(&vPos);
		KGLOG_COM_PROCESS_ERROR(hRetCode);
		nOffsetX = xPos - m_ptLastMousePos.x;
		nOffsetY = yPos - m_ptLastMousePos.y;

		//yaw
		D3DXMatrixRotationY(&Matrix, nOffsetX * 0.005f);
		vLookAtPos = D3DXVECTOR3(vLookAtPos.x - vPos.x, vLookAtPos.y, vLookAtPos.z - vPos.z);
		D3DXVec3TransformCoord(&vLookAtPos, &vLookAtPos, &Matrix);
		vLookAtPos = D3DXVECTOR3(vLookAtPos.x + vPos.x, vLookAtPos.y, vLookAtPos.z + vPos.z);

		//pitch
 		D3DXMatrixRotationAxis(&Matrix, &vRight, nOffsetY * 0.005f);
		vLookAtPos = D3DXVECTOR3(vLookAtPos.x - vPos.x, vLookAtPos.y, vLookAtPos.z - vPos.z);
		D3DXVec3TransformCoord(&vLookAtPos, &vLookAtPos, &Matrix);
		vLookAtPos = D3DXVECTOR3(vLookAtPos.x + vPos.x, vLookAtPos.y, vLookAtPos.z + vPos.z);

		hRetCode = pICamera->UpdateCamera(vPos, vLookAtPos, 0, 0);
		KGLOG_COM_PROCESS_ERROR(hRetCode);
		
		m_ptLastMousePos.x = xPos;
		m_ptLastMousePos.y = yPos;
	}

	nResult = true;
Exit0:
	return nResult;
}

void KGSceneViewer::UnInit()
{
	if (m_pScene)
	{
		m_pEngineMgr->RemoveScene(m_pScene);
		m_pScene = NULL;
	}
	if (m_pEngineMgr)
	{
		m_pEngineMgr->UnInit();
		m_pEngineMgr = NULL;
	}
	if (m_hEngineDLL)
	{
		FreeLibrary(m_hEngineDLL);
		m_hEngineDLL = NULL;
	}
	if (m_bKMemoryInited)
	{
		KMemory::Finalize();
		m_bKMemoryInited = false;
	}
}

int KGSceneViewer::LoadScene(const TCHAR cszMapPathName[])
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hRetCode = E_FAIL;
	int nOutputWindowID= 0;
	IKG3DCamera* pICamera = NULL;
	IEKG3DSceneSceneEditor* pIEEditor = NULL;
	IEKG3DScene* pEScene = NULL;
	KG3DSCENE_INITSTRUCT TempStruct;

#if defined(DEBUG) | defined(_DEBUG)
	KG3D_GAME_INIT_CONTEXT Context;
	m_pEngineMgr->InitGameContext(&Context);
#endif

	if (m_pScene)
	{
		hRetCode = m_pEngineMgr->RemoveScene(m_pScene);
		KGLOG_COM_PROCESS_ERROR(hRetCode);
	}

	hRetCode = m_pEngineMgr->NewOneScene(SCENETYPE_SCENEEDITOR, &m_pScene);
	KGLOG_COM_PROCESS_ERROR(hRetCode);

	hRetCode = m_pScene->LoadFromFileEx(
		cszMapPathName,
		0, 0, 
		reinterpret_cast<DWORD_PTR>(&TempStruct), 0);
	KGLOG_COM_PROCESS_ERROR(hRetCode);

	hRetCode = m_pScene->AddOutputWindow("Scene", m_hWindow, 0, 0, &nOutputWindowID);
	KGLOG_COM_PROCESS_ERROR(hRetCode);

	pICamera = m_pScene->GetCurrentCamera();
	KGLOG_PROCESS_ERROR(pICamera);

	hRetCode = pICamera->SetPosition(g_cCameraInitPos, false);
	KGLOG_COM_PROCESS_ERROR(hRetCode);

	hRetCode = pICamera->SetLookAtPosition(g_cLookUpInitPos, false);
	KGLOG_COM_PROCESS_ERROR(hRetCode);

	pEScene = static_cast<IEKG3DScene*>(m_pScene);
	hRetCode = pEScene->GetSceneSceneEditor(&pIEEditor);
	KGLOG_COM_PROCESS_ERROR(hRetCode);

	hRetCode = pIEEditor->GetCurOutputWnd(&m_pCurOutputWnd);
	KGLOG_COM_PROCESS_ERROR(hRetCode);

	hRetCode =  m_pCurOutputWnd->ResetDevice(true, m_pCurOutputWnd->GetViewPort()->Width, m_pCurOutputWnd->GetViewPort()->Height);
	KGLOG_COM_PROCESS_ERROR(hRetCode);

	hRetCode =  m_pCurOutputWnd->EnablePostEffect(POST_EFFECT_HDR, false);
	KGLOG_COM_PROCESS_ERROR(hRetCode);
	hRetCode =  m_pCurOutputWnd->EnablePostEffect(POST_EFFECT_GODRAY, false);
	KGLOG_COM_PROCESS_ERROR(hRetCode);
	hRetCode =  m_pCurOutputWnd->EnablePostEffect(POST_EFFECT_AOE, false);
	KGLOG_COM_PROCESS_ERROR(hRetCode);
	hRetCode =  m_pCurOutputWnd->EnablePostEffect(POST_EFFECT_DOF, false);
	KGLOG_COM_PROCESS_ERROR(hRetCode);
	hRetCode =  m_pCurOutputWnd->EnablePostEffect(POST_EFFECT_SHADOW, false);
	KGLOG_COM_PROCESS_ERROR(hRetCode);
	hRetCode =  m_pCurOutputWnd->EnablePostEffect(POST_EFFECT_BLOOM, false);
	KGLOG_COM_PROCESS_ERROR(hRetCode);
	hRetCode =  m_pCurOutputWnd->EnablePostEffect(POST_EFFECT_SHADOW, false);
	KGLOG_COM_PROCESS_ERROR(hRetCode);

Exit1:
	nResult = true;
Exit0:
	if (!nResult)
	{
		if (m_pScene)
		{
			m_pEngineMgr->RemoveScene(m_pScene);
		}
	}
	return nResult;
}	
