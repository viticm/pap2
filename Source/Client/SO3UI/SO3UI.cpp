////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : SO3UI.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2007-11-29 10:09:12
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "SO3UI/ISO3UI.h"
#include "SO3UI/IKGUI.h"
#include "ISO3Interaction.h"
#include "./KG3DEngine/KG3DInterface.h"
#include "./SO3Represent/SO3RepresentHandler.h"
#include "./SO3UI.h"
#include "./kgameworldmgr.h"
#include "./KGameWorldTextEncoder.h"
#include "./kgameworlduihandler.h"
#include "./kschemescripttable.h"
#include "./k3denginescripttable.h"
#include "./krepresentscripttable.h"
#include "./kdolbyaxonscripttable.h"

KSO3UI* g_pSO3UI = NULL;

KSO3UI::KSO3UI()
: m_pUI(NULL)
, m_hInst(NULL)
, m_hWnd(NULL)
, m_p3DUI(NULL)
, m_pInteraction(NULL)
, m_p3DEngineManager(NULL)
, m_p3DModelManager(NULL)
, m_pRepresent(NULL)
, m_hUIDll(NULL)
, m_hInteraction(NULL)
{
}

KSO3UI::~KSO3UI()
{
}

LRESULT KSO3UI::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return m_pUI->WndProc(uMsg, wParam, lParam);
}

void KSO3UI::Active()
{
	m_pInteraction->Active();
	m_pUI->Active();
}

void KSO3UI::Paint()
{
	m_pUI->Paint();
}

void KSO3UI::BeginPaint()
{
    m_pUI->FireEvent("RENDER_FRAME_UPDATE", 0);
}

void KSO3UI::EndPaint()
{
}

int KSO3UI::Init(const SO3UI_INIT_PARAM *pParam)
{
	int nResult = false;
	int nRetCode = false;

	KGUI_INIT_PARAM Param;
	fnCreateKGUI pfnCreateKGUI = NULL;
	fnCreateSO3Interaction pfnCreateSO3Interaction = NULL;

	KGLOG_PROCESS_ERROR(pParam && "KSO3UI::Init error: pParam is null!");
	KGLOG_PROCESS_ERROR(pParam->hInst);
	KGLOG_PROCESS_ERROR(pParam->hWnd);
	KGLOG_PROCESS_ERROR(pParam->p3DUI);
	KGLOG_PROCESS_ERROR(pParam->p3DEngineManager);
	KGLOG_PROCESS_ERROR(pParam->p3DModelManager);
	KGLOG_PROCESS_ERROR(pParam->pRepresent);

	m_hInst = pParam->hInst;
	m_hWnd = pParam->hWnd;
	m_p3DUI = pParam->p3DUI;
	m_p3DEngineManager = pParam->p3DEngineManager;
	m_p3DModelManager = pParam->p3DModelManager;
	m_pRepresent = pParam->pRepresent;

    strncpy_s(m_szIceClientLibDllPath, _countof(m_szIceClientLibDllPath), pParam->szIceClientLibDllPath, _TRUNCATE);

    // Init KGUI
    {
        Param.hWnd = pParam->hWnd;
        Param.hInst = pParam->hInst;
        Param.p3DUI = pParam->p3DUI;
        Param.p3DEngineManager = pParam->p3DEngineManager;
        Param.p3DModelManager = pParam->p3DModelManager;
        Param.pSoundShellHolder = pParam->pSoundShellHolder;
        Param.phwndEmbedWebPage = pParam->phwndEmbedWebPage;
        Param.uCodePage = pParam->uCodePage;

        strncpy_s(Param.szKLVideoDllPath, _countof(Param.szKLVideoDllPath), pParam->szKLVideoDllPath, _TRUNCATE);
        strncpy_s(Param.szConfigPath, _countof(Param.szConfigPath), pParam->szConfigPath, _TRUNCATE);
        strncpy_s(Param.szVersionPath, _countof(Param.szVersionPath), pParam->szVersionPath, _TRUNCATE);

        m_hUIDll = ::LoadLibraryA(pParam->szKGUIDllPath);
        KGLOG_PROCESS_ERROR(m_hUIDll && "load KGUID.dll failed!");

        pfnCreateKGUI = (fnCreateKGUI)GetProcAddress(m_hUIDll, FN_CREATE_KGUI);
        KGLOG_PROCESS_ERROR(pfnCreateKGUI && "get create kgui fun failed!");

        m_pUI = pfnCreateKGUI();
        KGLOG_PROCESS_ERROR(m_pUI && "CreateKGUI failed!");

        nRetCode = m_pUI->Init(&Param);
        KGLOG_PROCESS_ERROR(nRetCode && "kg ui init failed!");
    }

	nRetCode = g_GameWorldMgr.Init(pParam->fnAttribute, pParam->fnRequire);
	KGLOG_PROCESS_ERROR(nRetCode && "game world mgr init failed!");

	nRetCode = KGameWorldTextEncoder::Init();
	KGLOG_PROCESS_ERROR(nRetCode && "game world text encoder init failed!");

	{
        m_hInteraction = ::LoadLibraryA(pParam->szJX3InteractionDllPath);
		KGLOG_PROCESS_ERROR(m_hInteraction && "load JX3InteractionD.dll failed!");

        pfnCreateSO3Interaction = (fnCreateSO3Interaction)GetProcAddress(m_hInteraction, FN_CREATE_SO3INTERACTION);
		KGLOG_PROCESS_ERROR(pfnCreateSO3Interaction && "get create so3 interaction fun failed!");

		m_pInteraction = pfnCreateSO3Interaction();
		KGLOG_PROCESS_ERROR(m_pInteraction && "CreateKGUI failed!");

		nRetCode = m_pInteraction->Init();
		KGLOG_PROCESS_ERROR(nRetCode && "interaction init failed!");
	}

	nRetCode = g_GameWorldUIHandler.Init();
	KGLOG_PROCESS_ERROR(nRetCode && "game ui handler init failed!");

    nRetCode = KDolbyAxonScriptTable::Init();
    KGLOG_PROCESS_ERROR(nRetCode);

	KSchemeScriptTable::Load();
	K3DEngineScriptTable::Load();
	KRepresentScriptTable::Load();
    KDolbyAxonScriptTable::Load();

	nResult = true;
Exit0:
	if (!nResult)
		UnInit();
	return nResult;
}

int KSO3UI::NeedReset()
{
	return m_pUI->NeedReset();
}

void KSO3UI::Reset()
{
	m_pUI->Reset();
	g_GameWorldUIHandler.SetInLogin(FALSE);

	KSchemeScriptTable::Load();
	K3DEngineScriptTable::Load();
	KRepresentScriptTable::Load();
    KDolbyAxonScriptTable::Load();
}

void KSO3UI::Run()
{
	return m_pUI->Run();
}

void KSO3UI::UnInit()
{
    KDolbyAxonScriptTable::UnInit();

	g_GameWorldUIHandler.Exit();

	if (m_pUI)
	{
		m_pUI->Exit();
		m_pUI = NULL;
	}
	if (m_hUIDll)
	{
		::FreeLibrary(m_hUIDll);
		m_hUIDll = NULL;
	}

	if (m_pInteraction)
	{
		m_pInteraction->Exit();
		m_pInteraction = NULL;
	}
	if (m_hInteraction)
	{
		::FreeLibrary(m_hInteraction);
		m_hInteraction = NULL;
	}

	m_hInst = NULL;
	m_hWnd = NULL;

	m_p3DUI = NULL;
	m_p3DEngineManager = NULL;
	m_p3DModelManager = NULL;
	m_pRepresent = NULL;

	KGameWorldTextEncoder::Exit();
	g_GameWorldMgr.Exit();
}

void KSO3UI::Exit()
{
    delete this;
}

IKGUI* KSO3UI::GetIKGUI()
{
    return m_pUI;
}

IKSO3GameWorldUIHandler &KSO3UI::GetGameWorldUIHandler()
{
	return g_GameWorldUIHandler;
}


void KSO3UI::GetCursorPosition(int *pnX, int *pnY)
{
	POINT pt;
	::GetCursorPos(&pt);
	::ScreenToClient(m_hWnd, &pt);

	if (pnX)
		*pnX = pt.x;
	if (pnY)
		*pnY = pt.y;
}

void KSO3UI::SetSoundShell(IKG3DSoundShell *pShell)
{
	if (m_pUI)
	{
		m_pUI->SetSoundShell(pShell);
	}
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	return TRUE;
}

extern "C" __declspec(dllexport) ISO3UI* CreateSO3UI()
{
	g_pSO3UI = new(std::nothrow) KSO3UI;
	return g_pSO3UI;
}
