////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSFXAnimationController.h
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-10-24 16:52:38
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DSFXANIMATIONCONTROLLER_H_
#define _INCLUDE_KG3DSFXANIMATIONCONTROLLER_H_

#include "KG3DAnimationController.h"

class KG3DSFXSuede;
class KG3DSFX;
class KG3DSFXAnimationController : public KG3DAnimationController
{
public :
    KG3DSFXAnimationController(KG3DSFX* pSFX);
    ~KG3DSFXAnimationController();

    void   PauseAnimation(BOOL bPause);
    void   StartAnimation(DWORD dwPlayType, DWORD dwOffsetTime, float fSpeed);
    BOOL   IsPaused();
    void   SetSpeed(float fSpeed);
    float  GetSpeed();
    void   SeekAnimation(DWORD dwSeekType, int nOffset);
    BOOL   HasAnimation();
    LPCSTR GetAnimationFileName();
    int    GetPlayTime()  { return 0; }
    DWORD  GetStartTime() { return 0; }
	virtual int    GetCycleTime();
	virtual DWORD GetPlayType();
	virtual HRESULT RegistereEventHandler(IKG3DAnimationEventHandler* pHandler);
	virtual HRESULT UnregisterEventHandler(IKG3DAnimationEventHandler* pHandler);
	virtual DWORD GetMotionExtraInfo();
	virtual void SetMotionExtraInfo(DWORD dwMotionExtraInfo);
	virtual IKG3DModel* GetModel();
	virtual IKG3DAnimationEventHandler* GetEventHandler();
	virtual void Clone(KG3DSFXAnimationController* pSrc);
	virtual void AddAnimationControllerUpdateNotifier(IKG3DAnimationControllerUpdateNotifier *);
	virtual void RemoveAnimationControllerUpdateNotifier(IKG3DAnimationControllerUpdateNotifier *);
	virtual void SetUserdata(PVOID pUserdata);
	virtual PVOID GetUserdata();
	virtual enuAnimationControllerPriority GetPriority();
protected :
    DWORD         m_dwAnimationStartTime;
    BOOL          m_bAnimationPaused;
    KG3DSFX*      m_pSFX;
    DWORD         m_dwPlayType;
    FLOAT         m_fSpeed;
	DWORD           m_dwExtraMotionTypeInfo;
	//IKG3DAnimationEventHandler* m_pEventHandler;
};

class KG3DSFXSuedeAnimationController : public KG3DAnimationController
{
public :
    KG3DSFXSuedeAnimationController(KG3DSFXSuede* pSFXSuede);
    ~KG3DSFXSuedeAnimationController();

    void   PauseAnimation(BOOL bPause);
    void   StartAnimation(DWORD dwPlayType, DWORD dwOffsetTime, float fSpeed);
    BOOL   IsPaused();
    void   SetSpeed(float fSpeed);
    float  GetSpeed();
    void   SeekAnimation(DWORD dwSeekType, int nOffset);
    BOOL   HasAnimation();
    float  GetDuration();
    int    GetPlayTime()  { return 0; }  
    DWORD  GetStartTime() { return 0; }
	virtual int    GetCycleTime(); 
	virtual DWORD GetPlayType();
    LPCSTR GetAnimationFileName();
	virtual HRESULT RegistereEventHandler(IKG3DAnimationEventHandler* pHandler);
	virtual HRESULT UnregisterEventHandler(IKG3DAnimationEventHandler* pHandler);
	virtual DWORD GetMotionExtraInfo();
	virtual void SetMotionExtraInfo(DWORD dwMotionExtraInfo);
	virtual IKG3DModel* GetModel();
	virtual IKG3DAnimationEventHandler* GetEventHandler();
	BOOL	OnFinishPlay();
	virtual void AddAnimationControllerUpdateNotifier(IKG3DAnimationControllerUpdateNotifier *);
	virtual void RemoveAnimationControllerUpdateNotifier(IKG3DAnimationControllerUpdateNotifier *);
	virtual void SetUserdata(PVOID pUserdata);
	virtual PVOID GetUserdata();
	virtual enuAnimationControllerPriority GetPriority();

public :
	int			   m_nPlayCount;
	int			   m_nFinishCnt;

private :
    KG3DSFXSuede*  m_pSFXSuede;
    FLOAT          m_fAnimationStartTime;
    BOOL           m_bAnimationPaused;
    DWORD          m_dwPlayType;
    FLOAT          m_fSpeed;
	DWORD           m_dwExtraMotionTypeInfo;
	//IKG3DAnimationEventHandler* m_pEventHandler;
};


#endif //_INCLUDE_KG3DSFXANIMATIONCONTROLLER_H_
