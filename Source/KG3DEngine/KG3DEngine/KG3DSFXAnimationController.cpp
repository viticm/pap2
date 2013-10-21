////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSFXAnimationController.cpp
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-10-24 16:52:21
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KG3DSFXAnimationController.h"
#include "KG3DGraphicsTool.h"
#include "KG3DSFX.h"
#include "kg3dcliptable.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

KG3DSFXAnimationController::KG3DSFXAnimationController(KG3DSFX* pSFX)
{
    m_pSFX = pSFX;
    m_bAnimationPaused = FALSE;
    m_dwPlayType = SFX_AP_LOOP;
    m_fSpeed = 0.0f;
    m_dwAnimationStartTime = 0;
	//m_pEventHandler = NULL;
	m_dwExtraMotionTypeInfo = TYPE_SFX;
}

KG3DSFXAnimationController::~KG3DSFXAnimationController()
{
}

void KG3DSFXAnimationController::AddAnimationControllerUpdateNotifier(IKG3DAnimationControllerUpdateNotifier *)
{

}

void KG3DSFXAnimationController::RemoveAnimationControllerUpdateNotifier(IKG3DAnimationControllerUpdateNotifier *)
{

}

void KG3DSFXAnimationController::PauseAnimation(BOOL bPause)
{
    if (m_pSFX)
    {
        if (bPause)
        {
            m_pSFX->PlayAnimation((SFX_ANIMATION_PLAY)SFX_AP_PAUSE);
        }
        else
        {
            m_pSFX->PlayAnimation((SFX_ANIMATION_PLAY)m_dwPlayType);
        }
    }
}

void KG3DSFXAnimationController::StartAnimation(DWORD dwPlayType, 
                                                DWORD dwOffsetTime, 
                                                float fSpeed)
{
    m_dwAnimationStartTime = g_cGraphicsTool.GetNowTime();
    m_bAnimationPaused = FALSE;
    m_fSpeed = fSpeed;
    switch (dwPlayType)
    {
    case ENU_ANIMATIONPLAY_CIRCLE:
    case ENU_ANIMATIONPLAY_ADDCURRENT:
        m_dwPlayType = SFX_AP_LOOP;
        break;
    case ENU_ANIMATIONPLAY_ONCE:
    case ENU_ANIMATIONPLAY_ADDCURRENT_ONCE:
        m_dwPlayType = SFX_AP_ONE_TIME;
        break;
    default:
        break;
    }
    if (m_pSFX)
        m_pSFX->PlayAnimation((SFX_ANIMATION_PLAY)m_dwPlayType);
}


HRESULT KG3DSFXAnimationController::RegistereEventHandler(IKG3DAnimationEventHandler* pHandler)
{
	IKG3DAnimationEventHandler *pCurrentHandler = GetEventHandler();
	UnregisterEventHandler(pCurrentHandler);
	_ASSERTE(!g_cClipTable.GetEventCenter()->FindEventHandler(this));
	return g_cClipTable.GetEventCenter()->RegistereEventHandler(this,pHandler);
}

int KG3DSFXAnimationController::GetCycleTime()
{
	return 0;
}

HRESULT KG3DSFXAnimationController::UnregisterEventHandler(IKG3DAnimationEventHandler* pHandler)
{
	return g_cClipTable.GetEventCenter()->UnregisterEventHandler(this,pHandler);
}

BOOL KG3DSFXAnimationController::IsPaused()
{
    return m_bAnimationPaused;
}

void KG3DSFXAnimationController::SetSpeed(float fSpeed)
{
    m_fSpeed = fSpeed;
}

IKG3DModel* KG3DSFXAnimationController::GetModel()
{
	return m_pSFX;
}

float KG3DSFXAnimationController::GetSpeed()
{
    return m_fSpeed;
}

void KG3DSFXAnimationController::SeekAnimation(DWORD dwSeekType, int nOffset)
{
    HRESULT hrRetCode = E_FAIL;

    KG_PROCESS_ERROR(m_pSFX);

    DWORD dwFrameTime = m_pSFX->GetFrameInterval();

    ASSERT(dwSeekType == ANIMATION_SEEK_SET);
    hrRetCode = m_pSFX->SetCurrentFrame(static_cast<float>(nOffset) / dwFrameTime);
    KG_COM_PROCESS_ERROR(hrRetCode);

Exit0:
    return;
}

BOOL KG3DSFXAnimationController::HasAnimation()
{
    return TRUE;
}

LPCSTR KG3DSFXAnimationController::GetAnimationFileName()
{
    return NULL;
}

DWORD KG3DSFXAnimationController::GetMotionExtraInfo()
{
	return m_dwExtraMotionTypeInfo;
}

void KG3DSFXAnimationController::SetMotionExtraInfo(DWORD dwMotionExtraInfo)
{
	m_dwExtraMotionTypeInfo = dwMotionExtraInfo;
}

DWORD KG3DSFXAnimationController::GetPlayType()
{
	return m_dwPlayType;
}





KG3DSFXSuedeAnimationController::KG3DSFXSuedeAnimationController(KG3DSFXSuede* pSFXSuede)
{
    m_pSFXSuede = pSFXSuede;
    m_bAnimationPaused = FALSE;
    m_dwPlayType = ENU_ANIMATIONPLAY_CIRCLE;
    m_fSpeed = 0.0f;
    m_fAnimationStartTime = 0.0f;
	m_nPlayCount = 0;
	m_nFinishCnt = 0;
	m_dwExtraMotionTypeInfo = TYPE_SFX;
	//m_pEventHandler = NULL;
}


enuAnimationControllerPriority KG3DSFXSuedeAnimationController::GetPriority()
{
	return ANICTL_PRIMARY;
}

enuAnimationControllerPriority KG3DSFXAnimationController::GetPriority()
{
	return ANICTL_PRIMARY;
}

KG3DSFXSuedeAnimationController::~KG3DSFXSuedeAnimationController()
{
}

void KG3DSFXSuedeAnimationController::PauseAnimation(BOOL bPause)
{
    if (m_pSFXSuede)
    {
        for (DWORD i = 0; i < m_pSFXSuede->m_vecBindInfo.size(); i++)
            m_pSFXSuede->m_vecBindInfo[i].pModel->GetAnimationController(ANICTL_PRIMARY)->PauseAnimation(bPause);
    }
    m_bAnimationPaused = bPause;
}

void KG3DSFXSuedeAnimationController::StartAnimation(DWORD dwPlayType, 
                                                     DWORD dwOffsetTime, 
                                                     float fSpeed)
{
    m_bAnimationPaused = FALSE;
	if (m_pSFXSuede)
    {
        for (DWORD i = 0; i < m_pSFXSuede->m_vecBindInfo.size(); i++)
            m_pSFXSuede->m_vecBindInfo[i].pModel->GetAnimationController(ANICTL_PRIMARY)->StartAnimation(dwPlayType, dwOffsetTime, fSpeed);
    }
}

BOOL KG3DSFXSuedeAnimationController::IsPaused()
{
    return m_bAnimationPaused;
}

void KG3DSFXSuedeAnimationController::SetSpeed(float fSpeed)
{
    if (m_pSFXSuede)
    {
        for (DWORD i = 0; i < m_pSFXSuede->m_vecBindInfo.size(); i++)
            m_pSFXSuede->m_vecBindInfo[i].pModel->GetAnimationController(ANICTL_PRIMARY)->SetSpeed(fSpeed);
    }
    m_fSpeed = fSpeed;
}

float KG3DSFXSuedeAnimationController::GetSpeed()
{
    return m_fSpeed;
}

void KG3DSFXSuedeAnimationController::SeekAnimation(DWORD dwSeekType, int nOffset)
{
    if (m_pSFXSuede)
    {
        for (DWORD i = 0; i < m_pSFXSuede->m_vecBindInfo.size(); i++)
            m_pSFXSuede->m_vecBindInfo[i].pModel->GetAnimationController(ANICTL_PRIMARY)->SeekAnimation(dwSeekType, nOffset);
    }
}

BOOL KG3DSFXSuedeAnimationController::HasAnimation()
{
    return TRUE;
}

float KG3DSFXSuedeAnimationController::GetDuration()
{
    return 0.0f;
}

LPCSTR KG3DSFXSuedeAnimationController::GetAnimationFileName()
{
    return NULL;
}

DWORD KG3DSFXSuedeAnimationController::GetPlayType()
{
	return m_dwPlayType;
}

HRESULT KG3DSFXSuedeAnimationController::RegistereEventHandler(IKG3DAnimationEventHandler* pHandler)
{
	return g_cClipTable.GetEventCenter()->RegistereEventHandler(this,pHandler);
//	HRESULT hr = E_FAIL;
//	KG_PROCESS_ERROR(pHandler);
//	m_pEventHandler = pHandler;
//	hr = S_OK;
//Exit0:
//	return hr;
}

HRESULT KG3DSFXSuedeAnimationController::UnregisterEventHandler(IKG3DAnimationEventHandler* pHandler)
{
	return g_cClipTable.GetEventCenter()->UnregisterEventHandler(this,pHandler);
//	HRESULT hr = E_FAIL;
//	KG_PROCESS_ERROR(pHandler == m_pEventHandler);
//	m_pEventHandler = NULL;
//	hr = S_OK;
//Exit0:
//	return hr;
}


BOOL KG3DSFXSuedeAnimationController::OnFinishPlay()
{
	++m_nFinishCnt;
	
	if (m_nFinishCnt == m_nPlayCount)
	{
		g_cClipTable.GetEventCenter()->OnPlayAnimationFinish(this,this);
		//if (m_pEventHandler)
		//	m_pEventHandler->OnPlayAnimationFinish(this);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void KG3DSFXSuedeAnimationController::AddAnimationControllerUpdateNotifier(IKG3DAnimationControllerUpdateNotifier *)
{

}

void KG3DSFXSuedeAnimationController::RemoveAnimationControllerUpdateNotifier(IKG3DAnimationControllerUpdateNotifier *)
{

}



DWORD KG3DSFXSuedeAnimationController::GetMotionExtraInfo()
{
	return m_dwExtraMotionTypeInfo;
}

void KG3DSFXSuedeAnimationController::SetMotionExtraInfo(DWORD dwMotionExtraInfo)
{
	_ASSERTE(dwMotionExtraInfo == IKG3DAnimationController::TYPE_SFX);
	m_dwExtraMotionTypeInfo = dwMotionExtraInfo;
}

IKG3DModel* KG3DSFXSuedeAnimationController::GetModel()
{
	return m_pSFXSuede;
}

IKG3DAnimationEventHandler* KG3DSFXSuedeAnimationController::GetEventHandler()
{
	return g_cClipTable.GetEventCenter()->FindEventHandler(this);
}

IKG3DAnimationEventHandler* KG3DSFXAnimationController::GetEventHandler()
{
	return g_cClipTable.GetEventCenter()->FindEventHandler(this);
}

void KG3DSFXAnimationController::Clone(KG3DSFXAnimationController* pSrc)
{
	m_dwAnimationStartTime = pSrc->m_dwAnimationStartTime;
	m_bAnimationPaused = pSrc->m_bAnimationPaused;
	m_pSFX = pSrc->m_pSFX;
	m_dwPlayType = pSrc->m_dwPlayType;
	m_fSpeed = pSrc->m_fSpeed;
	m_dwExtraMotionTypeInfo = pSrc->m_dwExtraMotionTypeInfo;
	g_cClipTable.GetEventCenter()->RegistereEventHandler(this,
		g_cClipTable.GetEventCenter()->FindEventHandler(pSrc) );
}

void KG3DSFXAnimationController::SetUserdata(PVOID pUserdata)
{
	KG_USE_ARGUMENT(pUserdata);
}

PVOID KG3DSFXAnimationController::GetUserdata()
{
	return NULL;
}

void KG3DSFXSuedeAnimationController::SetUserdata(PVOID pUserdata)
{
	KG_USE_ARGUMENT(pUserdata);
}

PVOID KG3DSFXSuedeAnimationController::GetUserdata()
{
	return NULL;
}

int KG3DSFXSuedeAnimationController::GetCycleTime()
{
	return 0;
}