#include "StdAfx.h"
#include "KSO3Client.h"
#include "KGatewayClient.h"
#include "SO3Represent/SO3RepresentInterface.h"
#include "SO3UI/ISO3UI.h"

//#define SO3CLIENT_PROFILE_3D
//#define SO3CLIENT_PROFILE_UI

KSO3Client::KSO3Client()
: m_dwStartTime(0)
, m_nLastBreatheTime(0)
, m_dwRenderTime(0)
, m_dwPrevFrameRelTime(0)
, m_p3DEngine(NULL)
, m_pRepresent(NULL)
, m_pUI(NULL)
, m_pSO3World(NULL)
, m_pSoundShell(NULL)
, m_bHasContentToPresent(FALSE)
{
    m_szExePath[0] = '\0';

    memset(&m_Config, 0, sizeof(m_Config));
    memset(&m_Version, 0, sizeof(m_Version));
    memset(&m_JX3Sdoa, 0, sizeof(m_JX3Sdoa));
}

KSO3Client::~KSO3Client()
{
}

int KSO3Client::OnLoop()
{
    HRESULT hr = E_FAIL;
    int nRetCode = false;
    int nResult = false;
    int nServerLoop = 0;
    DWORD dwAbsTime = 0;
    DWORD dwRelTime = 0;
    long long nLogicFrameTime = 0;
    long long nLogicFlowTime = 0;
    bool bActivate = false;
    IKG3DUI* p3DUI = NULL;

    g_GatewayClient.Activate();

    g_PlayerClient.Breathe();

    nServerLoop = g_PlayerClient.GetServerLoop();

    dwAbsTime = KG_GetTickCount();
    dwRelTime = dwAbsTime - m_dwStartTime;

    nLogicFrameTime = (long long)(g_pSO3World->m_nGameLoop - g_pSO3World->m_nStartLoop) * 1000;
    nLogicFlowTime = (long long)(dwAbsTime - g_pSO3World->m_dwStartTime) * GAME_FPS;

    while (
        (g_pSO3World->m_nGameLoop <= nServerLoop - GAME_FPS / 8 || nLogicFrameTime < nLogicFlowTime) &&
        (g_pSO3World->m_nGameLoop <= nServerLoop + GAME_FPS / 8))
    {
        g_pSO3World->Activate();

        g_PlayerClient.Breathe();

        nLogicFrameTime = (long long)(g_pSO3World->m_nGameLoop - g_pSO3World->m_nStartLoop) * 1000;
        nLogicFlowTime = (long long)(KG_GetTickCount() - g_pSO3World->m_dwStartTime) * GAME_FPS;

        bActivate = true;
    }

    if (bActivate)
    {
        dwAbsTime = KG_GetTickCount();

        nRetCode = UpdateStartTimeEvaluation(&g_PlayerClient.m_StartTimeEvaluation, dwAbsTime);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    if ((long long)dwAbsTime * GAME_FPS > m_nLastBreatheTime + 1000)
        UpdateUI(dwAbsTime);

    if (m_bHasContentToPresent)
    {
        p3DUI = m_p3DEngine->Get3DUI();
        KG_PROCESS_ERROR(p3DUI);

        hr = p3DUI->Present();
        KGLOG_COM_PROCESS_ERROR(hr);

        m_bHasContentToPresent = FALSE;
    }

    if (!m_bHasContentToPresent && (dwAbsTime - m_dwRenderTime > 8 || bActivate))
    {
        m_dwRenderTime = dwAbsTime;

        RedrawWindow(KGWin32App::GetHWND(), NULL, NULL, RDW_INTERNALPAINT | RDW_NOERASE);
    }

    MemoryLeakDetect();

    if (m_pUI->NeedReset())
        Reset();
	
    nResult = true;
Exit0:
    return nResult;
}

int KSO3Client::UpdateUI(DWORD dwTimeNow)
{
    int nRetCode = false;
    int nResult = false;

#ifdef SO3CLIENT_PROFILE_UI
    static int s_nCount = 0;
    static DWORD s_dwTotalTickCount = 0;

    DWORD dwTickCount = KG_GetTickCount();
#endif

    KG_PROCESS_SUCCESS(m_pUI == NULL);

    m_pUI->Active();

    m_nLastBreatheTime = (long long)dwTimeNow * GAME_FPS;

Exit1:
    nResult = true;

#ifdef SO3CLIENT_PROFILE_UI
    s_dwTotalTickCount += KG_GetTickCount() - dwTickCount;

    ++s_nCount;

    if ((s_nCount & 0xF) == 0)
    {
        printf("[client] UpdateUI %d %d %f\n", s_dwTotalTickCount, s_nCount, (double)s_dwTotalTickCount / s_nCount);
    }
#endif
    return nResult;
}

void KSO3Client::Paint()
{
    HRESULT hr = E_FAIL;
    IKG3DUI* p3DUI = NULL;
    BOOL bBeginRenderToScreen = FALSE;
    BOOL bBeginScene = FALSE;

    KG_PROCESS_SUCCESS(m_bHasContentToPresent);

    KG_PROCESS_ERROR(m_p3DEngine);

    m_pUI->BeginPaint();

    p3DUI = m_p3DEngine->Get3DUI();
    KG_PROCESS_ERROR(p3DUI);

    hr = p3DUI->BeginRenderToScreen();
    KG_COM_PROCESS_ERROR(hr);

    bBeginRenderToScreen = TRUE;

    hr = p3DUI->BeginScene();
    KGLOG_COM_PROCESS_ERROR(hr);

    bBeginScene = TRUE;

    if (m_pUI)
        m_pUI->Paint();

    if (m_JX3Sdoa.pSDOADx9)
    {
        hr = m_JX3Sdoa.pSDOADx9->RenderEx();
        KGLOG_COM_PROCESS_ERROR(hr);
    }

Exit0:
    if (hr == D3DERR_DRIVERINTERNALERROR || hr == D3DERR_INVALIDCALL)
    {
        KGLogPrintf(KGLOG_ERR, "JX3Client Paint %u.\n", hr);
        PostMessage(GetHWND(), WM_QUIT, 0, 0);
    }
    if (bBeginScene)
    {
        hr = p3DUI->EndScene();
        KGLOG_COM_CHECK_ERROR(hr);
    }
    if (bBeginRenderToScreen)
    {
        hr = p3DUI->EndRenderToScreen();
        KGLOG_COM_CHECK_ERROR(hr);

        m_pUI->EndPaint();
    }

    m_bHasContentToPresent = TRUE;
Exit1:
    return;
}

void KSO3Client::OnPaint()
{
#ifdef SO3CLIENT_PROFILE_3D
    static int s_nCount = 0;
    static DWORD s_dwTotalTickCount = 0;

    DWORD dwTickCount = KG_GetTickCount();
#endif

    if (m_pRepresent)
        m_pRepresent->GetRepresentHandelr()->Activate();

    if (m_p3DEngine)
        m_p3DEngine->FrameMove();

    Paint();

#ifdef SO3CLIENT_PROFILE_3D
    s_dwTotalTickCount += KG_GetTickCount() - dwTickCount;

    ++s_nCount;

    if ((s_nCount & 0xF) == 0)
    {
        printf("[client] OnPaint %d %d %f\n", s_dwTotalTickCount, s_nCount, (double)s_dwTotalTickCount / s_nCount);
    }
#endif
}

IKG3DSoundShell* KSO3Client::GetSoundShell()
{
	return m_pSoundShell;
}

