#pragma once
#include "Engine/KMutex.h"
#include "IEKG3DSfx.h"
#include "KG3DTemplates.h"

class KG3DModel;
class KG3DClip;
class KG3DAnimationController;
class KG3DAnimationTagContainer;
class KG3DMaterial;

enum KG3DModelCMD
{
	CMD_NONE,
	CMD_ATTACH,
	CMD_DetachModel,
	CMD_COMPUTEBBOX,
	CMD_PlayAnimation,
	CMD_PlaySplitAnimation,
	CMD_BindToSocket,
	CMD_BindToBone,
	CMD_SetPhysicsScene,
	CMD_SeekAnimation,
	CMD_SetDetail,
	CMD_SetMaterial,
	CMD_KG3DModel_LoadMaterialFromFile,
    CMD_BIP_PlayAnimation,
	CMD_KG3DModelST_SetBoneTransform,
	CMD_SetLod,
	CMD_LAST,
};

struct KG3DModelCommandBase
{
	KG3DModelCMD m_eType;

	virtual ~KG3DModelCommandBase()
	{
		;
	}
};

struct KG3DModelCommand_Attach : public KG3DModelCommandBase
{
	KG3DModel* m_lpModel;
	KG3DModelCommand_Attach(KG3DModel* pModel);
	~KG3DModelCommand_Attach();
};

struct KG3DModelCommand_SetLod : public KG3DModelCommandBase
{
	float m_fLod;
	KG3DModelCommand_SetLod(float fLod)
	{
		m_eType = CMD_SetLod;
		m_fLod = fLod;
	}
};

struct KG3DModelCommand_DetachModel : public KG3DModelCommandBase
{
	KG3DModel* m_lpModel;
	KG3DModelCommand_DetachModel(KG3DModel* pModel);
	~KG3DModelCommand_DetachModel();
};

struct KG3DModelCommand_ComputeBBox : public KG3DModelCommandBase
{
	KG3DModelCommand_ComputeBBox()
	{
		m_eType = CMD_COMPUTEBBOX;
	}
};

struct KG3DModelCommand_KG3DModelST_SetBoneTransform : public KG3DModelCommandBase
{
	string m_BoneName;
	float  m_fBoneTrans;

	KG3DModelCommand_KG3DModelST_SetBoneTransform(LPCSTR pName,float fTans)
	{
		m_eType = CMD_KG3DModelST_SetBoneTransform;
		m_BoneName = pName;
		m_fBoneTrans = fTans;
	}
};

struct KG3DModelCommand_KG3DModel_LoadMaterialFromFile : public KG3DModelCommandBase
{
	string m_szFileName;
	DWORD  m_dwOption;

	KG3DModelCommand_KG3DModel_LoadMaterialFromFile(LPCSTR strFileName, DWORD Option)
	{
		m_eType = CMD_KG3DModel_LoadMaterialFromFile;
		m_szFileName = strFileName;
		m_dwOption   = Option;
	}

};
struct KG3DModelCommand_PlaySplitAnimation : public KG3DModelCommandBase
{
	string m_strAnimationName;
	DWORD  m_dwSplitType;
	DWORD  m_dwExtraInfo;
	DWORD  m_dwPlayType; 
	FLOAT  m_fSpeed;
	int    m_nOffsetTime;
	PVOID  m_pReserved;
	TweenTimeSpan Span;
	PVOID  m_pUserdata;
	enuAnimationControllerPriority m_Priority;

	KG3DAnimationTagContainer* m_pTag;
	KG3DClip* m_pClip; 
	enuModelPlayAnimationType m_ePlayAnimationType;

	KG3DModelCommand_PlaySplitAnimation(LPCSTR strAnimationName,
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
		KG3DClip* pClip);
	~KG3DModelCommand_PlaySplitAnimation();
};

struct KG3DSfxCommand_PlayAnimation : public KG3DModelCommandBase
{
    SFX_ANIMATION_PLAY  dwPlayType;
    FLOAT  fSpeed; 
    int    nOffsetTime; 
 
    KG3DSfxCommand_PlayAnimation(SFX_ANIMATION_PLAY Type, FLOAT Speed, int OffsetTime)
        : dwPlayType(Type), fSpeed(Speed), nOffsetTime(OffsetTime)
    {
        m_eType = CMD_PlayAnimation;
    }
};

struct KG3DModelCommand_PlayAnimation : public KG3DModelCommandBase
{
	DWORD  m_dwPlayType;
	string m_strAnimationName; 
	FLOAT  m_fSpeed; 
	int    m_nOffsetTime; 
	PVOID  m_pReserved;
	TweenTimeSpan Span;
	PVOID  m_pUserdata;
	enuAnimationControllerPriority m_Priority;

	KG3DAnimationTagContainer* m_pTag;
	KG3DClip* m_pClip; 
	enuModelPlayAnimationType m_ePlayAnimationType;

	KG3DModelCommand_PlayAnimation(DWORD dwPlayType, 
		LPCTSTR strAnimationName, 
		FLOAT fSpeed, 
		int   nOffsetTime, 
		PVOID pReserved,
		PVOID pUserdata,
		enuAnimationControllerPriority Priority,
		enuModelPlayAnimationType Type,
		KG3DAnimationTagContainer* pTagContainerNew,
		KG3DClip* pClip);
	~KG3DModelCommand_PlayAnimation();
};

struct KG3DModelCommand_BindToSocket : public KG3DModelCommandBase
{
	KG3DModel* m_lpModel;
	string     m_cszSocketName;
	int        m_nTryCount;

	KG3DModelCommand_BindToSocket(KG3DModel *pTemp, const char cszSocketName[])
	{
		m_eType = CMD_BindToSocket;
		m_lpModel = pTemp;
		m_cszSocketName = cszSocketName;
		m_nTryCount = 0;
	}
	virtual ~KG3DModelCommand_BindToSocket();
};

struct KG3DModelCommand_BindToBone : public KG3DModelCommandBase
{
	KG3DModel* m_lpModel;
	string     m_cszBoneName;

	KG3DModelCommand_BindToBone(KG3DModel *pTemp, const char cszBoneName[])
	{
		m_eType = CMD_BindToBone;
		m_lpModel = pTemp;
		m_cszBoneName = cszBoneName;
	}
	virtual ~KG3DModelCommand_BindToBone();
};

class KG3DPhysiscScene;
struct KG3DModelCommand_SetPhysicsScene : public KG3DModelCommandBase
{
	KG3DPhysiscScene* m_pScene;
	KG3DModelCommand_SetPhysicsScene(KG3DPhysiscScene *pScene)
	{
		m_eType = CMD_SetPhysicsScene;
		m_pScene = pScene;
	}
};

struct KG3DModelCommand_SetDetail : public KG3DModelCommandBase
{
	int m_nTable;
	int m_nDefine;
	KG3DModelCommand_SetDetail(int nTable,int nDetail)
	{
		m_eType = CMD_SetDetail;
		m_nTable  = nTable;
		m_nDefine = nDetail;
	}
};

struct KG3DModelCommand_SetMaterial : public KG3DModelCommandBase
{
	KG3DMaterial* m_lpMlt;
	KG3DModelCommand_SetMaterial(KG3DMaterial* pMtl);
	~KG3DModelCommand_SetMaterial();
};

struct KG3DModelCommand_SeekAnimation : public KG3DModelCommandBase
{
	DWORD m_dwSplitType;
	enuAnimationControllerPriority m_Prority;
	DWORD m_dwSeekType;
	float m_fOffset;

	KG3DModelCommand_SeekAnimation(DWORD dwSplitType, 
		enuAnimationControllerPriority nPrority,
		DWORD dwSeekType, 
		float fOffset)
	{
		m_eType = CMD_SeekAnimation;
		m_dwSplitType = dwSplitType;
		m_Prority = nPrority;
		m_dwSeekType = dwSeekType;
		m_fOffset = fOffset;
	}
};

struct KG3DModelCommand_Bip_PlayAnimation : public KG3DModelCommandBase
{
	KG3DClip* m_pClip; 
	DWORD     m_PlayType; 
	int       m_nOffsetTime;
    float     m_fSpeed;
	TweenTimeSpan m_Span;

	enuAnimationControllerPriority m_Priority;
	KG3DAnimationController* m_pControllers;

	KG3DModelCommand_Bip_PlayAnimation(KG3DClip* pClip, 
		DWORD PlayType, 
		int nOffsetTime,
		float fSpeed, 
		TweenTimeSpan Span,
		enuAnimationControllerPriority Priority,
		KG3DAnimationController* pControllers);
	~KG3DModelCommand_Bip_PlayAnimation();

};

struct KG3DModelCommand_KG3DAnimationSplitter_PlayAnimation : public KG3DModelCommandBase
{
	KG3DAnimationTagContainer* m_pTag;
	KG3DClip* m_pClip; 
	enuModelPlayAnimationType m_ePlayAnimationType;

	string       m_strAnimationName;
	unsigned int m_uPresetIndex    ;
	DWORD        m_dwSplitType;
	DWORD        m_dwPlayType ;
	FLOAT        m_fSpeed     ;
	int          m_nOffsetTime;
	PVOID        m_pReserved  ;
	PVOID        m_pUserdat   ;
	TweenTimeSpan m_TweenTimeSpan;
	KG3DAnimationController *m_pControllerSrc;
    enuAnimationControllerPriority m_Priority;
	KG3DModelCommand_KG3DAnimationSplitter_PlayAnimation(
		enuModelPlayAnimationType eAniType,
		KG3DAnimationTagContainer* pTag,
		KG3DClip* pClip,
		LPCSTR strAnimationName, 
		unsigned int uPresetIndex,
		DWORD dwSplitType,
		DWORD dwPlayType,
		FLOAT fSpeed,
		int nOffsetTime,
		KG3DAnimationController *pControllerSrc,
		PVOID pReserved,
		PVOID pUserdata,
		enuAnimationControllerPriority Priority);
	~KG3DModelCommand_KG3DAnimationSplitter_PlayAnimation();

};
struct KG3DModelCommand_KG3DAnimationSplitter_PlayAnimationChange : public KG3DModelCommandBase
{
	KG3DAnimationTagContainer* m_pTag;
	KG3DClip* m_pClip; 
	enuModelPlayAnimationType m_ePlayAnimationType;

	string       m_strAnimationName;
	unsigned int m_uPresetIndex    ;
	DWORD        m_dwSplitType;
	DWORD        m_dwPlayType ;
	FLOAT        m_fSpeed     ;
	int          m_nOffsetTime;
	PVOID        m_pReserved  ;
	PVOID        m_pUserdat   ;
	KG3DAnimationController *m_pControllerSrc;

	KG3DModelCommand_KG3DAnimationSplitter_PlayAnimationChange(
		enuModelPlayAnimationType eAniType,
		KG3DAnimationTagContainer* pTag,
		KG3DClip* pClip,
		LPCSTR strAnimationName, 
		unsigned int uPresetIndex,
		DWORD dwSplitType,
		DWORD dwPlayType,
		FLOAT fSpeed,
		int nOffsetTime,
		KG3DAnimationController *pControllerSrc,
		PVOID pReserved,
		PVOID pUserdata);
	~KG3DModelCommand_KG3DAnimationSplitter_PlayAnimationChange();

};

struct KG3DModelCommand_KG3DAnimationSplitter_PlayAnimationSecondary : public KG3DModelCommandBase
{
	KG3DAnimationTagContainer* m_pTag;
	KG3DClip* m_pClip; 
	enuModelPlayAnimationType m_ePlayAnimationType;

	string       m_strAnimationName;
	DWORD        m_dwSplitType;
	DWORD        m_dwPlayType ;
	FLOAT        m_fSpeed     ;
	int          m_nOffsetTime;
	PVOID        m_pReserved  ;
	PVOID        m_pUserdat   ;
	KG3DAnimationController *m_pControllerSrc;

	KG3DModelCommand_KG3DAnimationSplitter_PlayAnimationSecondary(
		enuModelPlayAnimationType eAniType,
		KG3DAnimationTagContainer* pTag,
		KG3DClip* pClip,
		LPCSTR strAnimationName, 
		DWORD dwSplitType,
		DWORD dwPlayType,
		FLOAT fSpeed,
		int nOffsetTime,
		KG3DAnimationController *pControllerSrc,
		PVOID pReserved,
		PVOID pUserdata);
	~KG3DModelCommand_KG3DAnimationSplitter_PlayAnimationSecondary();

};

class KG3DFuctionCommand
{
private:
	KMutex m_Lock;
	list<KG3DModelCommandBase*>m_listCmd;
	list<KG3DModelCommandBase*>m_listCmdNoBlock;
public:

public:
	template <class T> HRESULT ProcessCommandsForUser(T* pUser)
	{
		DWORD dwID = ::GetCurrentThreadId();
		KG_PROCESS_ERROR(dwID == g_dwMainThreadID);

		ASSERT(m_listCmd.size()<500);

		m_Lock.Lock();
		{
			while (m_listCmd.size())
			{

				KG3DModelCommandBase* pCmd = *m_listCmd.begin();

				BOOL bSucceed = pUser->OnProcessCommands(pCmd);

				if(bSucceed)
				{
					SAFE_DELETE(pCmd);
					m_listCmd.pop_front();
				}
				else
				{
					break;
				}
			}
		}
		{
			list<KG3DModelCommandBase*>::iterator i = m_listCmdNoBlock.begin();
			list<KG3DModelCommandBase*>::iterator iend = m_listCmdNoBlock.end();
			while (i!=iend)
			{
				KG3DModelCommandBase* pCmd = *i;

				BOOL bSucceed = pUser->OnProcessCommands(pCmd);

				if(bSucceed)
				{
					SAFE_DELETE(pCmd);
					i = m_listCmdNoBlock.erase(i);
				}
				else
				{
					i++;
				}
			}
		}
		m_Lock.Unlock();

Exit0:
		return S_OK;
	}

	HRESULT GetAnimationFileName(LPTSTR strFileName, size_t tBufferSize);


	HRESULT AddCommand(KG3DModelCommandBase* pCMD,BOOL bBlock);
	BOOL    HasCommand();

	HRESULT UnInit();
	KG3DFuctionCommand(void);
	~KG3DFuctionCommand(void);
};
