#include "stdafx.h"
#include "../../Source/Common/SO3World/Src/KPlayer.h"
#include "../../Source/Common/SO3World/Src/KNpc.h"
#include "../../Source/Common/SO3World/Src/KDoodad.h"
#include "./krlscene.h"
#include "./krlcamera.h"
#include "./krlcursor.h"
#include "./krltarget.h"
#include "../../SO3Represent.h"

KRLScene::KRLScene()
: m_pRLCamera(NULL)
, m_pRLCursor(NULL)
, m_pRLTarget(NULL)
, m_dwScene(0)
, m_p3DScene(NULL)
, m_nOutputWindowID(-1)
{
}

KRLScene::~KRLScene()
{
}

int KRLScene::Init()
{
	int nRetCode = false;
    int nResult = false;
    int nInitModelGCMgr = false;
    int nInitMissileMgr = false;
    int nInitCharacterMgr = false;
    int nInitRidesMgr = false;
    int nInitDoodadMgr = false;
    int nInitCharacterGCMgr = false;
    int nInitRidesGCMgr = false;
	int nInitDoodadGCMgr = false;
	int nInitCursor = false;
	int nInitTarget = false;

	nRetCode = m_ModelGCMgr.Init();
	KGLOG_PROCESS_ERROR(nRetCode);

    nInitModelGCMgr = true;

	nRetCode = m_MissileMgr.Init();
	KGLOG_PROCESS_ERROR(nRetCode);

    nInitMissileMgr = true;

    nRetCode = m_CharacterMgr.Init();
    KGLOG_PROCESS_ERROR(nRetCode);

    nInitCharacterMgr = true;

    nRetCode = m_RidesMgr.Init();
    KGLOG_PROCESS_ERROR(nRetCode);

    nInitRidesMgr = true;

	nRetCode = m_DoodadMgr.Init();
	KGLOG_PROCESS_ERROR(nRetCode);

    nInitDoodadMgr = true;

    nRetCode = m_CharacterGCMgr.Init();
    KGLOG_PROCESS_ERROR(nRetCode);

    nInitCharacterGCMgr = true;

    nRetCode = m_RidesGCMgr.Init();
    KGLOG_PROCESS_ERROR(nRetCode);

    nInitRidesGCMgr = true;

    nRetCode = m_DoodadGCMgr.Init();
	KGLOG_PROCESS_ERROR(nRetCode);

	nInitDoodadGCMgr = true;

    nRetCode = InitCursor();
	KGLOG_PROCESS_ERROR(nRetCode);

	nInitCursor = true;

    nRetCode = InitTarget();
	KGLOG_PROCESS_ERROR(nRetCode);

	nInitTarget = true;

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (nInitTarget)
        {
		    ExitTarget();
			nInitTarget = false;
		}

        if (nInitCursor)
        {
		    ExitCursor();
			nInitCursor = false;
		}

        if (nInitDoodadMgr)
	    {
		    m_DoodadMgr.Exit();
			nInitDoodadMgr = false;
		}
    
        if (nInitCharacterMgr)
        {
		    m_CharacterMgr.Exit();
			nInitCharacterMgr = false;
		}

        if (nInitRidesMgr)
        {
		    m_RidesMgr.Exit();
			nInitRidesMgr = false;
		}

        if (nInitMissileMgr)
	    {
		    m_MissileMgr.Exit();
			nInitMissileMgr = false;
		}

        if (nInitDoodadGCMgr)
        {
		    m_DoodadGCMgr.Exit();
			nInitDoodadGCMgr = false;
		}

        if (nInitCharacterGCMgr)
        {
		    m_CharacterGCMgr.Exit();
			nInitCharacterGCMgr = false;
		}

        if (nInitRidesGCMgr)
        {
		    m_RidesGCMgr.Exit();
			nInitRidesGCMgr = false;
		}

        if (nInitModelGCMgr)
        {
		    m_ModelGCMgr.Exit();
			nInitModelGCMgr = false;
		}
    }
	return nResult;
}

void KRLScene::Exit()
{
	ExitTarget();
	ExitCursor();

	m_DoodadMgr.Exit();
    m_DoodadGCMgr.Exit();

    m_CharacterMgr.Exit();
    m_CharacterGCMgr.Exit();

    m_RidesMgr.Exit();
    m_RidesGCMgr.Exit();

	m_MissileMgr.Exit();

    m_ModelGCMgr.Exit();
}

BOOL KRLScene::Reset()
{
	int nRetCode = false;

    nRetCode = m_CharacterMgr.Reset();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_RidesMgr.Reset();
    KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_DoodadMgr.Reset();
	KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_CharacterGCMgr.Reset();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_RidesGCMgr.Reset();
    KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_DoodadGCMgr.Reset();
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_MissileMgr.Reset();
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_ModelGCMgr.Reset();
	KGLOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL KRLScene::Activate(double fTime, double fTimeLast, DWORD dwGameLoop, BOOL bFrame)
{
    m_SkillEffectResult.Activate(fTime, fTimeLast, dwGameLoop, bFrame);

	m_MissileMgr.Activate(fTime, fTimeLast, dwGameLoop, bFrame);

    StartProfile(&g_pRL->m_Profile, KRLPROFILE_CHARACTER);
    m_CharacterMgr.Activate(fTime, fTimeLast, dwGameLoop, bFrame);
    StopProfile(&g_pRL->m_Profile, KRLPROFILE_CHARACTER);

    StartProfile(&g_pRL->m_Profile, KRLPROFILE_RIDES);
    m_RidesMgr.Activate(fTime, fTimeLast, dwGameLoop, bFrame);
    StopProfile(&g_pRL->m_Profile, KRLPROFILE_RIDES);

    StartProfile(&g_pRL->m_Profile, KRLPROFILE_DOODAD);
    m_DoodadMgr.Activate(fTime, fTimeLast, dwGameLoop, bFrame);
    StopProfile(&g_pRL->m_Profile, KRLPROFILE_DOODAD);

    StartProfile(&g_pRL->m_Profile, KRLPROFILE_CHARACTER_GC);
    m_CharacterGCMgr.Activate(fTime, fTimeLast, dwGameLoop, bFrame);
    StopProfile(&g_pRL->m_Profile, KRLPROFILE_CHARACTER_GC);

    StartProfile(&g_pRL->m_Profile, KRLPROFILE_RIDES_GC);
    m_RidesGCMgr.Activate(fTime, fTimeLast, dwGameLoop, bFrame);
    StopProfile(&g_pRL->m_Profile, KRLPROFILE_RIDES_GC);

    StartProfile(&g_pRL->m_Profile, KRLPROFILE_DOODAD_GC);
    m_DoodadGCMgr.Activate(fTime, fTimeLast, dwGameLoop, bFrame);
    StopProfile(&g_pRL->m_Profile, KRLPROFILE_DOODAD_GC);

	if (m_pRLCamera)
		m_pRLCamera->UpdateLocal(fTime, fTimeLast, dwGameLoop, bFrame);

	if (m_pRLCursor)
		m_pRLCursor->Update(fTime, fTimeLast, dwGameLoop, bFrame);

	if (m_pRLTarget)
		m_pRLTarget->Update(fTime, fTimeLast, dwGameLoop, bFrame);

	m_ModelGCMgr.Activate(fTime, fTimeLast, dwGameLoop, bFrame);

	return TRUE;
}

BOOL KRLScene::InitCamera()
{
	HRESULT hr = E_FAIL;
	KRLCamera* pCamera = NULL;
	IKG3DScene* p3DScene = NULL;
	IKG3DCamera* p3DCamera = NULL;

	KG_PROCESS_SUCCESS(m_p3DScene == NULL);

	ASSERT(m_pRLCamera == NULL);

	p3DCamera = m_p3DScene->GetCurrentCamera();
	KG_PROCESS_SUCCESS(p3DCamera == NULL);

	m_pRLCamera  = new(std::nothrow) KRLCamera;
	KGLOG_PROCESS_ERROR(m_pRLCamera);

    hr = m_pRLCamera->SetCamera(p3DCamera, m_p3DScene);
	KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
	return TRUE;
Exit0:
	ExitCamera();
	return FALSE;
}

void KRLScene::ExitCamera()
{
	SAFE_DELETE(m_pRLCamera);
}

BOOL KRLScene::InitCursor()
{
	HRESULT hr = E_FAIL;

	KG_PROCESS_SUCCESS(m_p3DScene == NULL);

	ASSERT(m_pRLCursor == NULL);

	m_pRLCursor  = new(std::nothrow) KRLCursor;
	KGLOG_PROCESS_ERROR(m_pRLCursor);

	hr = m_pRLCursor->Init(this);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = m_pRLCursor->Hide();
	KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
	return TRUE;
Exit0:
	ExitCursor();
	return FALSE;
}

void KRLScene::ExitCursor()
{
	HRESULT hr = E_FAIL;

	if (m_pRLCursor)
	{
		hr = m_pRLCursor->Exit();
		KGLOG_COM_CHECK_ERROR(hr);

		SAFE_DELETE(m_pRLCursor);
	}
}

BOOL KRLScene::InitTarget()
{
	HRESULT hr = E_FAIL;

	KG_PROCESS_SUCCESS(m_p3DScene == NULL);

	ASSERT(m_pRLTarget == NULL);

	m_pRLTarget  = new(std::nothrow) KRLTarget;
	KGLOG_PROCESS_ERROR(m_pRLTarget);

	hr = m_pRLTarget->Init(this);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = m_pRLTarget->Hide();
	KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
	return TRUE;
Exit0:
	ExitTarget();
	return FALSE;
}

void KRLScene::ExitTarget()
{
	HRESULT hr = E_FAIL;

	if (m_pRLTarget)
	{
		hr = m_pRLTarget->Exit();
		KGLOG_COM_CHECK_ERROR(hr);

		SAFE_DELETE(m_pRLTarget);
	}
}

KRLDoodad* GetRLDoodad(DWORD dwDoodad)
{
	KDoodad* pDoodad = GetDoodad(dwDoodad);
	return reinterpret_cast<KRLDoodad*>(pDoodad ? pDoodad->m_pRepresentObject : NULL);
}

KRLCharacter* GetRLCharacter(DWORD dwCharacter)
{
    KRLCharacter* pRLCharacter = NULL;

    if (IS_PLAYER(dwCharacter))
    {
    	KPlayer* pPlayer = GetPlayer(dwCharacter);
	    pRLCharacter = reinterpret_cast<KRLCharacter*>(pPlayer ? pPlayer->m_pRepresentObject : NULL);
    }
    else
    {
        KNpc* pNpc = GetNpc(dwCharacter);
        pRLCharacter = reinterpret_cast<KRLCharacter*>(pNpc ? pNpc->m_pRepresentObject : NULL);
    }

    return pRLCharacter;
}
