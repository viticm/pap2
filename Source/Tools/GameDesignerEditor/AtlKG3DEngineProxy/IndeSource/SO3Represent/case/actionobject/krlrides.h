#ifndef CASE_ACTION_OBJECT_KRLRIDES_H
#define CASE_ACTION_OBJECT_KRLRIDES_H

#include "./krlscene.h"
#include "./krlsceneobject.h"
#include "../render/kanimationmodel.h"
#include "../gameworld/ktabletransform.h"

class KRLCharacter;

class KRLRides : public KRLSceneObject
{
public:
	KRLRides();
	virtual ~KRLRides();

	void* operator new(size_t nSize, std::nothrow_t const&) throw();
	void operator delete(void* pData, std::nothrow_t const&) throw();

	void* operator new(size_t nSize);
	void operator delete(void* pData) throw();

	int Init(KRLScene* pRLScene, BOOL bCurrentPlayer);
	void Reset();

    HRESULT DisplayDataChanged();

    HRESULT BindCharacter();
    HRESULT UnbindCharacter();

    HRESULT PlayRideAnimation(DWORD dwAnimationID, int nOffsetTime, enuAnimationControllerPriority nTrackType);
    HRESULT UpdateAnimationSpeed(float fSpeed);

	void UpdateRemote(double fTime, double fTimeLast, DWORD dwGameLoop, BOOL bFrame);

    void LoadSkillBuff(size_t nIndex, KSkillBuffParam SkillBuff);
    void UnloadSkillBuff(size_t nIndex);

protected:
	virtual HRESULT OnPositionChange(D3DXVECTOR3 const& vPosition);
	virtual HRESULT OnRotationLHChange(D3DXQUATERNION const& qRotationLH);
    virtual HRESULT OnAlphaChange(float fAlpha);
    virtual HRESULT OnGetBBox(D3DXVECTOR3& vMin, D3DXVECTOR3& vMax) const;
    virtual HRESULT OnPlayAnimationFinish(IKG3DAnimationController* pController);
    virtual HRESULT OnTagPlayingSFX(IKG3DAnimationController* pController, IKG3DModel* pModelSFX, TagSFXCallBackExtraInfo* pExtraInfo);
    virtual HRESULT OnMotionNotify(IKG3DAnimationController* pController, DWORD dwMotionInfo, TagMotionCallBackExtraInfo *pMotionCallBackExtraInfo);

private:
	void* operator new[](std::size_t, std::nothrow_t const&) throw();
	void operator delete[](void*, std::nothrow_t const&) throw();

	void* operator new[](std::size_t) throw(std::bad_alloc);
	void operator delete[](void*) throw();

	void* operator new(size_t, void*) throw();
	void operator delete(void*, void*) throw();

	void* operator new[](std::size_t, void*) throw();
	void operator delete[](void*, void*) throw();

	KRLRides(KRLRides const& rhs);
	KRLRides& operator=(KRLRides& rhs);

    HRESULT LoadMDL();
    HRESULT LoadDefaultSocket();
    HRESULT LoadSocket(int nIndex, KRLSocketResource const SocketParam);

    int InitPlayAnimation();

public:
    KRLCharacter* m_pRLCharacter;
    DWORD m_dwRepresentID;
    DWORD m_dwIdleAnimationID;
    DWORD m_dwSlopeType;
    KAnimationModel m_RenderData;

private:
    DWORD m_dwAnimationID;
};

#endif // CASE_ACTION_OBJECT_KRLRIDES_H