#include "StdAfx.h"
#include "KG3DFuctionCommand.h"
#include "kg3dmodel.h"
#include "KG3DAnimationTagContainer.h"

extern DWORD g_dwMainThreadID;
KG3DFuctionCommand::KG3DFuctionCommand(void)
{
}

KG3DFuctionCommand::~KG3DFuctionCommand(void)
{
	UnInit();
}

HRESULT KG3DFuctionCommand::AddCommand(KG3DModelCommandBase* pCMD,BOOL bBlock)
{
	m_Lock.Lock();

	if(bBlock)
		m_listCmd.push_back(pCMD);
	else
		m_listCmdNoBlock.push_back(pCMD);

	m_Lock.Unlock();


	ASSERT(m_listCmd.size()<400);

	return S_OK;
}

BOOL KG3DFuctionCommand::HasCommand()
{
	return m_listCmd.size()>0 || m_listCmdNoBlock.size()>0;
}


KG3DModelCommand_Attach::KG3DModelCommand_Attach(KG3DModel* pModel)
{
	m_eType = CMD_ATTACH;
	m_lpModel = pModel;
	m_lpModel->AddRef();
}

KG3DModelCommand_Attach::~KG3DModelCommand_Attach()
{
	SAFE_RELEASE(m_lpModel);
};

KG3DModelCommand_DetachModel::KG3DModelCommand_DetachModel(KG3DModel* pModel)
{
	m_eType = CMD_DetachModel;
	m_lpModel = pModel;
	m_lpModel->AddRef();
}

KG3DModelCommand_DetachModel::~KG3DModelCommand_DetachModel()
{
	SAFE_RELEASE(m_lpModel);
};

KG3DModelCommand_BindToSocket::~KG3DModelCommand_BindToSocket()
{
	SAFE_RELEASE(m_lpModel);
}

KG3DModelCommand_BindToBone::~KG3DModelCommand_BindToBone()
{
	SAFE_RELEASE(m_lpModel);
}

HRESULT KG3DFuctionCommand::UnInit()
{
	m_Lock.Lock();

	list<KG3DModelCommandBase*>::iterator i = m_listCmd.begin();
	list<KG3DModelCommandBase*>::iterator iend = m_listCmd.end();
	while (i!=iend)
	{
		KG3DModelCommandBase* pCmd = *i;
		SAFE_DELETE(pCmd);
		i++;
	}
	m_listCmd.clear();

	//////////////////////////////////////////////////////////////////////////
	{
		list<KG3DModelCommandBase*>::iterator i = m_listCmdNoBlock.begin();
		list<KG3DModelCommandBase*>::iterator iend = m_listCmdNoBlock.end();
		while (i!=iend)
		{
			KG3DModelCommandBase* pCmd = *i;

			SAFE_DELETE(pCmd);
			i++;
		}
		m_listCmdNoBlock.clear();
	}

	m_Lock.Unlock();

	return S_OK;
}


KG3DModelCommand_Bip_PlayAnimation::KG3DModelCommand_Bip_PlayAnimation(KG3DClip* pClip, 
								   DWORD PlayType, 
								   int nOffsetTime,
								   float fSpeed, 
								   TweenTimeSpan Span,
								   enuAnimationControllerPriority Priority,
								   KG3DAnimationController* pControllers)
{
	m_eType = CMD_BIP_PlayAnimation;
	m_pClip = pClip;
	m_pClip->AddRef();

	m_PlayType = PlayType;
	m_nOffsetTime = nOffsetTime;
	m_fSpeed = fSpeed;
	m_Span = Span;
	m_Priority = Priority;
	m_pControllers = pControllers;
}

KG3DModelCommand_Bip_PlayAnimation::~KG3DModelCommand_Bip_PlayAnimation()
{
	SAFE_RELEASE(m_pClip);
}

KG3DModelCommand_PlayAnimation::KG3DModelCommand_PlayAnimation(DWORD dwPlayType, 
															   LPCTSTR strAnimationName, 
															   FLOAT fSpeed, 
															   int   nOffsetTime, 
															   PVOID pReserved,
															   PVOID pUserdata,
															   enuAnimationControllerPriority Priority,
															   enuModelPlayAnimationType Type,
															   KG3DAnimationTagContainer* pTagContainerNew,
															   KG3DClip* pClip)
{
	m_eType = CMD_PlayAnimation;
	m_dwPlayType = dwPlayType;
	m_strAnimationName = strAnimationName;
	m_fSpeed = fSpeed;
	m_nOffsetTime = nOffsetTime; 
	m_pReserved = pReserved;
	if(pReserved)
		Span = (*(TweenTimeSpan*)pReserved);
	m_pUserdata = pUserdata;
	m_Priority =  Priority;
	m_ePlayAnimationType = Type;
	m_pTag = pTagContainerNew;
	if(m_pTag)
		m_pTag->AddRef();
	m_pClip = pClip;
	if(m_pClip)
		m_pClip->AddRef();
}

KG3DModelCommand_PlayAnimation::~KG3DModelCommand_PlayAnimation()
{
	SAFE_RELEASE(m_pTag);
	SAFE_RELEASE(m_pClip);
}

KG3DModelCommand_PlaySplitAnimation::KG3DModelCommand_PlaySplitAnimation(LPCSTR strAnimationName,
									DWORD dwSplitType,
									DWORD dwExtraInfo,
									DWORD dwPlayType, 
									FLOAT fSpeed,
									int   nOffsetTime,
									PVOID pReserved,
									PVOID pUserdata,
									enuAnimationControllerPriority Priority,
									enuModelPlayAnimationType Type,
									KG3DAnimationTagContainer* pTagContainerNew,
									KG3DClip* pClip)
{
	m_eType = CMD_PlaySplitAnimation;
	m_strAnimationName = strAnimationName;
	m_dwSplitType = dwSplitType;
	m_dwExtraInfo = dwExtraInfo;
	m_dwPlayType  = dwPlayType;
	m_fSpeed      = fSpeed;
	m_nOffsetTime = nOffsetTime;
	m_pReserved   = pReserved;
	if(pReserved)
		Span = (*(TweenTimeSpan*)pReserved);
	m_pUserdata   = pUserdata;
	m_Priority    = Priority;
	m_ePlayAnimationType = Type;
	m_pTag = pTagContainerNew;
	if(m_pTag)
		m_pTag->AddRef();
	m_pClip = pClip;
	if(m_pClip)
		m_pClip->AddRef();
}

KG3DModelCommand_PlaySplitAnimation::~KG3DModelCommand_PlaySplitAnimation()
{
	SAFE_RELEASE(m_pTag);
	SAFE_RELEASE(m_pClip);
}

KG3DModelCommand_SetMaterial::KG3DModelCommand_SetMaterial(KG3DMaterial* pMtl)
{
	m_eType = CMD_SetMaterial;
	m_lpMlt  = pMtl;
	m_lpMlt->AddRef();
}

KG3DModelCommand_SetMaterial::~KG3DModelCommand_SetMaterial()
{
	SAFE_RELEASE(m_lpMlt);
}

HRESULT KG3DFuctionCommand::GetAnimationFileName(LPTSTR strFileName, size_t tBufferSize)
{
	HRESULT hr = E_FAIL;

	m_Lock.Lock();

	list<KG3DModelCommandBase*>::reverse_iterator it = m_listCmd.rend();
	list<KG3DModelCommandBase*>::reverse_iterator itEnd = m_listCmd.rbegin();


	while (it != itEnd)
	{
		if ((*it)->m_eType == CMD_PlayAnimation)
		{
			KG3DModelCommand_PlayAnimation *pPlayAnimation = static_cast<KG3DModelCommand_PlayAnimation*>((*it));
			strcpy_s(strFileName, tBufferSize, pPlayAnimation->m_strAnimationName.c_str());
			goto Exit1;
		}
		it++;
	}
	goto Exit0;

Exit1:
	hr = S_OK; 
Exit0:
	m_Lock.Unlock();
	return hr;
}
