#ifndef CASE_ACTION_OBJECT_KRLDOODAD_H
#define CASE_ACTION_OBJECT_KRLDOODAD_H

#include "./krlsceneobject.h"
#include "../gameworld/ktablelist.h"
#include "../gameworld/krldoodadframedata.h"
#include "../render/KAnimationModel.h"

class IKG3DModel;
class IKG3DScene;
class KGameWorldNpc;
class KRLRemoteCharacter;

class KRLDoodad : public KRLSceneObject
{
	friend class KRLDoodadMgr;

public:
	enum { INVALID_OBJECT_ID = UINT_MAX };

public:
	KRLDoodad();
	virtual ~KRLDoodad();

	void* operator new(size_t nSize, std::nothrow_t const&) throw();
	void operator delete(void* pData, std::nothrow_t const&) throw();

	void* operator new(size_t nSize);
	void operator delete(void* pData) throw();

	int Init(KDoodad* pDoodad, KRLScene* pRLScene);
    int InitCorpse(KDoodad* pDoodad, KRLScene* pRLScene, KRLRemoteCharacter* pRLRemoteCharacter);
	int Reset();

    int UpdateDisplayData(BOOL bForceUpdate);
    int UpdateState(BOOL bForceUpdate);

    int SetState(RL_DOODAD_STATE nState);

    HRESULT UpdateLootList();
    HRESULT UpdateQuestEffect();

	int Update(double fTime, double fTimeLast, DWORD dwGameLoop, BOOL bFrame);

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

	KRLDoodad(KRLDoodad const& rhs);
	KRLDoodad& operator=(KRLDoodad& rhs);

    int UpdateState(RL_DOODAD_STATE nState, BOOL bForceUpdate);

    int PlayAnimation(RL_DOODAD_ANIMATION nAni);

    int PlayCorpseAnimation(DWORD dwAnimationID);
    int PlayDoodadAnimation(RL_DOODAD_ANIMATION nAni, KRLDoodadModel const& RLDoodadModel);

    HRESULT LoadCorpseMDL(BOOL bForceUpdate);
    HRESULT LoadCorpseSocket();
    HRESULT LoadCorpseSocket(KNpcAnimationModel const* pAnimationModel);
	HRESULT LoadCorpseSocket(size_t nSocketIndex, LPCTSTR szMeshFilePath, LPCTSTR szMtlFilePath, float fSocketScale, int nColorChannelTable, int nColorChannel);

    HRESULT LoadDoodadMDL(RL_DOODAD_ANIMATION nAni, KRLDoodadModel const& Model);

    HRESULT LoadDoodadEffect(DWORD dwEffectID);
    HRESULT UnloadDoodadEffect(DWORD dwEffectID);

    int UpdatePosition();

    int SetSelectable(BOOL bSelectable);

    int GetTerrainSlopeRotationLH(D3DXQUATERNION& qRotationLH);

public:
    KAnimationModel m_RenderData;
	KRLDoodadFrameData m_FrameData;

private:
    BOOL m_bCorpse;
    BOOL m_bOnFloor;
    BOOL m_bSelectable;
	RL_DOODAD_STATE	m_nState;

    DWORD m_dwRepresentID;
};

#endif // CASE_ACTION_OBJECT_KRLDOODAD_H