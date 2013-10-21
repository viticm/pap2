#include "StdAfx.h"
#include "KG3DAnimationController.h"
#include "KG3DGraphicsTool.h"
#include "KG3DModel.h"
#include "KG3DModelST.h"
#include "KG3DBip.h"
#include "kg3dcliptable.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern DWORD g_dwMainThreadID;

UpdateAnimationInfo::UpdateAnimationInfo()
{
    ClipType = ANIMATION_NONE;
	pMatrix = NULL;
    pMesh = NULL;
    pMeshSrc = NULL;
	bNeedUpdateToWorld = FALSE;
};

KG3DAnimationController::KG3DAnimationController(void)
{
	Init();
}

KG3DAnimationController::~KG3DAnimationController(void)
{	
	if(m_lpCommands)
	{
		m_lpCommands->UnInit();
	}
	SAFE_DELETE(m_lpCommands);
	IKG3DAnimationEventHandler *pHandler = GetEventHandler();
	UnregisterEventHandler(pHandler);
	Detach();
}

void KG3DAnimationController::Attach(IEKG3DClip* pClip)
{
	SAFE_RELEASE(m_pClip);
	m_pClip = dynamic_cast<KG3DClip *>(pClip);
    if (m_pClip)
    {
        m_pClip->AddRef();
    }

}

HRESULT KG3DAnimationController::GetCurAnimation(IEKG3DClip **ppClip)
{
    HRESULT hrResult = E_FAIL;

    KG_PROCESS_ERROR(ppClip);

    *ppClip = m_pClip; 
    KG_PROCESS_ERROR(*ppClip);

    hrResult = S_OK;
Exit0:
    return hrResult;
};

HRESULT KG3DAnimationController::Detach()
{
	SAFE_RELEASE(m_pClip);
	SAFE_RELEASE(m_AnimationInfo.pMesh);
	m_AnimationInfo.bNeedUpdateToWorld = FALSE;

    return S_OK;
}

void KG3DAnimationController::UpdateNotifier()
{
    std::set<IKG3DAnimationControllerUpdateNotifier*> Notifiers;
    std::set<IKG3DAnimationControllerUpdateNotifier*>::iterator itNotifier;

    Notifiers = m_Notifiers;
    itNotifier = Notifiers.begin();
    while (itNotifier != Notifiers.end())
    {
        (*itNotifier)->AddRef();
        ++itNotifier;
    }

    itNotifier = Notifiers.begin();
    while (itNotifier != Notifiers.end())
    {
        (*itNotifier)->FrameMove(this);
        (*itNotifier)->Release();
        ++itNotifier;
    }

    return;
}

void KG3DAnimationController::UpdateAnimation(
    KG3DAnimationUpdateExtraInfoCollector *pCollector, 
	BOOL bUpdateRenderData,
	BOOL bEnableCallBack)
{
    HRESULT hrRetCode = E_FAIL;
    DWORD dwFrame = 0;
    DWORD dwFrameTo = 0;
    float fWeight = 0.0f;
    BOOL bOnFinish = FALSE;

    KG_PROCESS_ERROR(m_pClip && m_pClip->IsLoad());

    if (!m_bPause)
    {
        DWORD dwNowTime = g_cGraphicsTool.GetNowTime();

        ASSERT(dwNowTime >= m_dwLastUpdateTime);
        m_dwAniPlayTime += dwNowTime - m_dwLastUpdateTime;
        m_dwLastUpdateTime = dwNowTime;
    }

    hrRetCode = m_pClip->GetCurFrame(
        (DWORD)(m_dwAniPlayTime * m_fSpeed), m_dwPlayType, 
        &dwFrame, &dwFrameTo, &fWeight
    );
    KG_COM_PROCESS_ERROR(hrRetCode);

    BOOL bVisible = TRUE;
    if (m_pModel)
        bVisible = m_pModel->IsVisible();
	if (bUpdateRenderData && bVisible)
	{
		hrRetCode = m_pClip->UpdateClip(
			&m_AnimationInfo,
			pCollector,
			dwFrame,
			dwFrameTo,
			fWeight
		);
		KG_COM_PROCESS_ERROR(hrRetCode);
	}

	if (bEnableCallBack)
	{
        m_dwLastFrame = m_dwCurrentFrame;
        m_dwCurrentFrame = dwFrame;
		UpdateNotifier();
	}

	bOnFinish = (dwFrame == dwFrameTo);
	if (m_bOnFinish != bOnFinish)
	{
		m_bOnFinish = bOnFinish;
		if (bEnableCallBack && (m_dwPlayType == ENU_ANIMATIONPLAY_ONCE))
			OnAnimationFinish();
	}

Exit0:
	return;
}

void KG3DAnimationController::_SeekAnimation(DWORD dwSeekType, int nOffset)
{
    HRESULT hrRetCode = E_FAIL;
    DWORD dwFrame = 0;
    DWORD dwFrameTo = 0;
    float fWeight = 0;

    KG_ASSERT_EXIT(dwSeekType == ANIMATION_SEEK_SET);
    KG_ASSERT_EXIT(nOffset >= 0);

    m_dwAniPlayTime = (DWORD)nOffset;
    m_dwLastUpdateTime = g_cGraphicsTool.GetNowTime();

    hrRetCode = m_pClip->GetCurFrame(
        (DWORD)(m_dwAniPlayTime * m_fSpeed), m_dwPlayType, &dwFrame, &dwFrameTo, &fWeight
    );
    KG_COM_PROCESS_ERROR(hrRetCode);

    m_dwLastFrame = m_dwCurrentFrame;
    m_dwCurrentFrame = dwFrame;

Exit0:
    return;
}

void KG3DAnimationController::StartAnimation(DWORD PlayType, DWORD dwOffsetTime, float fSpeed)
{
    ASSERT(fSpeed > 0);
    m_fSpeed = fSpeed;
	m_dwPlayType = PlayType;
	m_dwLastFrame = 0;
    m_dwCurrentFrame = 0;
    m_bPause = false;
    m_dwLastUpdateTime = g_cGraphicsTool.GetNowTime();
    m_dwAniPlayTime = dwOffsetTime;
    m_AnimationInfo.ClipType = m_dwPlayType;
    m_bOnFinish = FALSE;
}

int KG3DAnimationController::GetPlayTime()
{
    return (int)m_dwAniPlayTime;
}

void KG3DAnimationController::SetSpeed(float fSpeed)
{
    ASSERT(fSpeed > 0);
    m_fSpeed = fSpeed;
}

float KG3DAnimationController::GetSpeed()
{
	return m_fSpeed;
}

BOOL KG3DAnimationController::IsPaused()
{
	return m_bPause;
}

void KG3DAnimationController::SeekAnimation(DWORD dwSeekType, int nOffset)
{
	if(m_pClip && m_pClip->IsLoad())
	{
		_SeekAnimation(dwSeekType, nOffset);
	}
	else
	{
		if(!m_lpCommands)
		{
			m_lpCommands = new KG3DFuctionCommand;
		}

		KG3DModelCommand_SeekAnimation* pSeek = new KG3DModelCommand_SeekAnimation(0, ANICTL_PRIMARY, dwSeekType, (float)nOffset);
		m_lpCommands->AddCommand(pSeek,TRUE);
	}
}

HRESULT KG3DAnimationController::SetAnimation(IEKG3DClip* pClip)
{
    HRESULT hResult = E_FAIL;
	KG_PROCESS_ERROR(pClip);
	SAFE_RELEASE(m_pClip);
	m_pClip = static_cast<KG3DClip *>(pClip);
	m_pClip->AddRef();
    hResult = S_OK;
Exit0:
	return hResult;
}

BOOL KG3DAnimationController::HasAnimation()
{
    return (m_pClip ? TRUE : FALSE);
}

LPCSTR KG3DAnimationController::GetAnimationFileName()
{
	LPCSTR strReturn = NULL;
	
	KG_PROCESS_ERROR(m_pClip);
	strReturn = m_pClip->m_scName.c_str();

Exit0:
	return strReturn;
}

void KG3DAnimationController::SetModel(KG3DModel* pModel)
{
	m_pModel = pModel;
}

DWORD KG3DAnimationController::GetMotionExtraInfo()
{
    return m_dwExtraMotionTypeInfo;
}

void KG3DAnimationController::SetMotionExtraInfo(DWORD dwMotionExtraInfo)
{
    m_dwExtraMotionTypeInfo = dwMotionExtraInfo;
}

void KG3DAnimationController::Clone(KG3DAnimationController& Dest) const
{
    //不会复制eventhandler
	SAFE_RELEASE(Dest.m_pClip);
    Dest.m_fSpeed = m_fSpeed;
    Dest.m_dwPlayType = m_dwPlayType;
    Dest.m_dwExtraMotionTypeInfo = m_dwExtraMotionTypeInfo;
    Dest.m_Notifiers = m_Notifiers;
    Dest.m_dwCurrentFrame = m_dwCurrentFrame;
    Dest.m_dwLastFrame = m_dwLastFrame;
	Dest.m_pUserdata = m_pUserdata;
	Dest.m_pClip = m_pClip;
	if (m_pClip)
	{
		m_pClip->AddRef();
    }

    Dest.m_bPause = m_bPause;
    Dest.m_dwLastUpdateTime = m_dwLastUpdateTime;
    Dest.m_dwAniPlayTime = m_dwAniPlayTime;
    Dest.m_bOnFinish = m_bOnFinish;
}

HRESULT KG3DAnimationController::RegistereEventHandler(IKG3DAnimationEventHandler* pHandler)
{
	UnregisterEventHandler(GetEventHandler());
	_ASSERTE(!g_cClipTable.GetEventCenter()->FindEventHandler(this));
	g_cClipTable.GetEventCenter()->RegistereEventHandler(this, pHandler);
	return S_OK;
}

HRESULT KG3DAnimationController::UnregisterEventHandler(IKG3DAnimationEventHandler* pHandler)
{
	g_cClipTable.GetEventCenter()->UnregisterEventHandler(this,pHandler);
    return S_OK;
}

IKG3DModel* KG3DAnimationController::GetModel()
{
    return m_pModel;
}

IKG3DAnimationEventHandler* KG3DAnimationController::GetEventHandler()
{
	return g_cClipTable.GetEventCenter()->FindEventHandler(this);
}

void KG3DAnimationController::AddAnimationControllerUpdateNotifier(IKG3DAnimationControllerUpdateNotifier* pNotifier)
{
	m_Notifiers.insert(pNotifier);
}

void KG3DAnimationController::RemoveAnimationControllerUpdateNotifier(IKG3DAnimationControllerUpdateNotifier* pNotifier)
{
	m_Notifiers.erase(pNotifier);
}

void KG3DAnimationController::SetUserdata(PVOID pUserdata)
{
	m_pUserdata = pUserdata;
}

PVOID KG3DAnimationController::GetUserdata()
{
	return m_pUserdata;
}

void KG3DAnimationController::Init()
{
	m_dwPlayType = ENU_ANIMATIONPLAY_CIRCLE;
	m_fSpeed = 1.0f;
	m_pClip = NULL;
	m_pModel = NULL;
	m_dwExtraMotionTypeInfo = TYPE_ALLPART;

	m_dwLastFrame = 0;
	m_dwCurrentFrame = 0;
	m_pUserdata = NULL;
	m_Priority = ANICTL_PRIMARY;
	m_lpCommands = NULL;
	Detach();

    m_bPause = false;
    m_dwLastUpdateTime = 0;
    m_dwAniPlayTime = 0;
    m_bOnFinish = TRUE;
}

void KG3DAnimationController::OnAnimationFinish()
{
    enuAnimationControllerPriority Priority = m_Priority;
    if (m_Priority == ANICTL_SECONDARY)
    {
        KG_PROCESS_ERROR(m_pModel);
        m_pModel->OnSecondaryAnimationFinish(this);
    }
    IKG3DAnimationEventHandler *piEventHandler = GetEventHandler();
    if (piEventHandler)
    {
        m_Priority = Priority;
        piEventHandler->OnPlayAnimationFinish(this);
    }

Exit0:
	return;
}

enuAnimationControllerPriority KG3DAnimationController::GetPriority()
{
	return m_Priority;
}

void KG3DAnimationController::FrameMove()
{
	if(m_pClip && m_pClip->IsLoad())
	{
		ProcessCommands();
	}
}

BOOL KG3DAnimationController::OnProcessCommands(KG3DModelCommandBase* pCmd)
{
	BOOL bSucceed = FALSE;

	switch(pCmd->m_eType)
	{
	case CMD_SeekAnimation:
		{
			KG3DModelCommand_SeekAnimation *pSeek = static_cast<KG3DModelCommand_SeekAnimation*>(pCmd);
			_SeekAnimation(pSeek->m_dwSeekType,
				(int)pSeek->m_fOffset);
			bSucceed = TRUE;
			break;
		}
	default:
		ASSERT(0);
	}
	return bSucceed;
}

HRESULT KG3DAnimationController::ProcessCommands()
{
	if(m_lpCommands)
		return m_lpCommands->ProcessCommandsForUser(this);
	return S_OK;
}

int KG3DAnimationController::GetSpanTime()
{
	//获得相对于Clip开始的时间
	int nReturn = 0;
	int nCycleTime = GetCycleTime();
	KG_PROCESS_ERROR(nCycleTime != 0);

	nReturn = (int)(m_dwAniPlayTime * m_fSpeed) % nCycleTime;
Exit0:
	return nReturn;
}

int KG3DAnimationController::GetCycleTime()
{
	int nReturn = 0;
	KG_PROCESS_ERROR(m_pClip && m_pClip->IsLoad());

	DWORD dwLength = m_pClip->GetLength();
	ASSERT(dwLength > 0);

	nReturn = static_cast<int>(dwLength);
Exit0:
	return nReturn;
}

DWORD KG3DAnimationController::GetPlayType()
{
	return m_dwPlayType;
}

void KG3DAnimationController::PauseAnimation(BOOL bPause)
{
    m_bPause = bPause;
}

void KG3DAnimationController::SetUpdateMatrix(D3DXMATRIX *pcUpdateMatrix)
{
    m_AnimationInfo.pMatrix = pcUpdateMatrix;
}

//////////////////////////////////////////////////////////////////////////
KG3DEventCenter::KG3DEventCenter()
{
	Init();
}

KG3DEventCenter::~KG3DEventCenter()
{
	UnInit();
}

HRESULT KG3DEventCenter::Init()
{
	return S_OK;
}

HRESULT KG3DEventCenter::UnInit()
{
	m_mapEvent.clear();
	return S_OK;
}

HRESULT KG3DEventCenter::RegistereEventHandler (PVOID pUser,IKG3DAnimationEventHandler* pHandler)
{
	m_Lock.Lock();
	
	m_mapEvent[pUser] = pHandler;

	m_Lock.Unlock();

	return S_OK;
}

HRESULT KG3DEventCenter::UnregisterEventHandler(PVOID pUser,IKG3DAnimationEventHandler* pHandler)
{
	m_Lock.Lock();

	map<PVOID,IKG3DAnimationEventHandler*>::iterator ifind = m_mapEvent.find(pUser);
	if(ifind!=m_mapEvent.end())
	{
		m_mapEvent.erase(ifind);
	}

	m_Lock.Unlock();

	return S_OK;
}

IKG3DAnimationEventHandler* KG3DEventCenter::FindEventHandler(PVOID pUser)
{
	IKG3DAnimationEventHandler* pOut = NULL;
	m_Lock.Lock();

	map<PVOID,IKG3DAnimationEventHandler*>::iterator ifind = m_mapEvent.find(pUser);
	if(ifind!=m_mapEvent.end())
	{
		pOut = ifind->second;
	}

	m_Lock.Unlock();

	return pOut;
}

HRESULT KG3DEventCenter::RemoveEventHandler(IKG3DAnimationEventHandler* pHandler)
{
	m_Lock.Lock();

	map<PVOID,IKG3DAnimationEventHandler*>::iterator i = m_mapEvent.begin();
	map<PVOID,IKG3DAnimationEventHandler*>::iterator iend = m_mapEvent.end();
	while (i!=iend)
	{
		IKG3DAnimationEventHandler* pH = i->second;
		if(pH == pHandler)
		{
			i = m_mapEvent.erase(i);
		}
		else
		{
			i++;
		}
	}

	m_Lock.Unlock();

	return S_OK;
}

HRESULT KG3DEventCenter::OnPlayAnimationFinish(PVOID pUser,IKG3DAnimationController* pController)
{
	HRESULT hr = E_FAIL;
	IKG3DAnimationEventHandler* pHandler = FindEventHandler(pUser);
	KGLOG_PROCESS_ERROR(pHandler);

	hr = pHandler->OnPlayAnimationFinish(pController);
Exit0:
	return hr;
}

HRESULT KG3DEventCenter::OnTagPlayingSFX(PVOID pUser,IKG3DAnimationController* pController, IKG3DModel* pModel, TagSFXCallBackExtraInfo* pCallBackExtraInfo)
{
	HRESULT hr = E_FAIL;
	IKG3DAnimationEventHandler* pHandler = FindEventHandler(pUser);
	KGLOG_PROCESS_ERROR(pHandler);

	hr = pHandler->OnTagPlayingSFX(pController,pModel,pCallBackExtraInfo);
Exit0:
	return hr;
}

HRESULT KG3DEventCenter::OnMotionNotify(PVOID pUser,IKG3DAnimationController* pController, DWORD dwMotionInfo, TagMotionCallBackExtraInfo* pMotionCallBackExtraInfo)
{
	HRESULT hr = E_FAIL;
	IKG3DAnimationEventHandler* pHandler = FindEventHandler(pUser);
	KGLOG_PROCESS_ERROR(pHandler);

	hr = pHandler->OnMotionNotify(pController,dwMotionInfo,pMotionCallBackExtraInfo);
Exit0:
	return hr;
}