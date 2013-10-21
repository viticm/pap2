////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DAnimationController.h
//  Version     : 1.0
//  Creator     : 
//  Create Date : 2006-11-1 9:58:42
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DANIMATIONCONTROLLER_H_
#define _INCLUDE_KG3DANIMATIONCONTROLLER_H_

////////////////////////////////////////////////////////////////////////////////

#include "KG3DInterface.h"
#include "IEKG3DAnimationController.h"
#include "KG3DAnimationStruct.h"
#include "KG3DFuctionCommand.h"

struct UpdateAnimationInfo
{
	DWORD ClipType;//enuClipType
	D3DXMATRIX* pMatrix;
	LPD3DXMESH  pMesh;
	LPD3DXMESH  pMeshSrc;
	BOOL        bNeedUpdateToWorld;
	UpdateAnimationInfo();
};

class KG3DAnimationController: 
    public IEKG3DAnimationController, public IKG3DAnimationController
{
	friend class KG3DClip;
	friend class KG3DFuctionCommand;

	KG3DFuctionCommand* m_lpCommands;
protected:
	HRESULT ProcessCommands();
	virtual BOOL OnProcessCommands(KG3DModelCommandBase* pCmd);
	void _SeekAnimation(DWORD dwSeekType, int nOffset);

public:
	KG3DAnimationController();
	~KG3DAnimationController();

	void FrameMove();

	void Init();
    void Clone(KG3DAnimationController& Dest) const;
	void SetModel(KG3DModel* pModel);
	void Attach(IEKG3DClip* pClip);
	virtual HRESULT Detach();
    void UpdateAnimation(
        KG3DAnimationUpdateExtraInfoCollector *pCollector,
		BOOL bUpdateRenderData,
		BOOL bEnableCallBack
    );
	virtual void PauseAnimation(BOOL bPause);
	void StartAnimation(DWORD dwPlayType, DWORD dwOffsetTime, float fSpeed);
	BOOL IsPaused();
	
	void SetSpeed(float fSpeed);
	float GetSpeed();

	virtual DWORD GetPlayType();
		
	void SeekAnimation(DWORD dwSeekType, int nOffset);

	UpdateAnimationInfo& GetUpateAnimationInfo() { return m_AnimationInfo; }
    void SetUpdateMatrix(D3DXMATRIX *pcUpdateMatrix);
	virtual HRESULT SetAnimation(IEKG3DClip* pClip);
	virtual BOOL HasAnimation();
	virtual LPCSTR GetAnimationFileName();
	void OnAnimationFinish();
	virtual HRESULT GetCurAnimation(IEKG3DClip **ppClip);
    KG3DClip *GetCurAnimation(){ return m_pClip;}
    virtual int GetPlayTime();
    virtual HRESULT RegistereEventHandler(IKG3DAnimationEventHandler* pHandler);
    virtual HRESULT UnregisterEventHandler(IKG3DAnimationEventHandler* pHandler);
    virtual DWORD GetMotionExtraInfo();
    virtual void SetMotionExtraInfo(DWORD dwMotionExtraInfo);
    virtual IKG3DModel* GetModel();

    virtual IKG3DAnimationEventHandler* GetEventHandler();

	virtual void AddAnimationControllerUpdateNotifier(IKG3DAnimationControllerUpdateNotifier* pNotifier);
	virtual void RemoveAnimationControllerUpdateNotifier(IKG3DAnimationControllerUpdateNotifier* pNotifier);
	virtual void SetUserdata(PVOID pUserdata);
	virtual PVOID GetUserdata();
	enuAnimationControllerPriority GetPriority();
	int   GetSpanTime();
	int   GetCycleTime();
	void  UpdateNotifier();
	DWORD           m_dwCurrentFrame;
	DWORD           m_dwLastFrame;
	PVOID			m_pUserdata;
	enuAnimationControllerPriority m_Priority;
	BOOL            m_bOnFinish;
protected:
	std::set<IKG3DAnimationControllerUpdateNotifier*> m_Notifiers;
	UpdateAnimationInfo m_AnimationInfo;
	float           m_fSpeed;
	DWORD           m_dwPlayType;
	KG3DClip*       m_pClip;
	KG3DModel*      m_pModel;
    DWORD           m_dwExtraMotionTypeInfo;
private:    
    BOOL m_bPause;
    DWORD m_dwLastUpdateTime;
    DWORD m_dwAniPlayTime;
};


class KG3DEventCenter : public IKG3DEnventCenter
{
	KMutex m_Lock;
	map<PVOID,IKG3DAnimationEventHandler*>m_mapEvent;

public:
	KG3DEventCenter();
	~KG3DEventCenter();

	HRESULT Init();
	HRESULT UnInit();

	HRESULT RegistereEventHandler (PVOID pUser,IKG3DAnimationEventHandler* pHandler);
	HRESULT UnregisterEventHandler(PVOID pUser,IKG3DAnimationEventHandler* pHandler);

	IKG3DAnimationEventHandler* FindEventHandler(PVOID pUser);

	HRESULT RemoveEventHandler(IKG3DAnimationEventHandler* pHandler);


	HRESULT OnPlayAnimationFinish(PVOID pUser,IKG3DAnimationController* pController);
	HRESULT OnTagPlayingSFX(PVOID pUser,IKG3DAnimationController* pController, IKG3DModel* pModel, TagSFXCallBackExtraInfo* pCallBackExtraInfo);
	HRESULT OnMotionNotify(PVOID pUser,IKG3DAnimationController* pController, DWORD dwMotionInfo, TagMotionCallBackExtraInfo* pMotionCallBackExtraInfo);

};

#endif //_INCLUDE_KG3DANIMATIONCONTROLLER_H_
