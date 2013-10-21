#ifndef CASE_ACTION_OBJECT_KRLTARGET_H
#define CASE_ACTION_OBJECT_KRLTARGET_H

#include "../gameworld/ktablelist.h"

class IKG3DModel;
class KRLScene;

class KRLTarget : public IKG3DAnimationEventHandler
{
public:
	KRLTarget();
	virtual ~KRLTarget();

	HRESULT Reset();
	int Update(double fTime, double fTimeLast, DWORD dwGameLoop, BOOL bFrame);

	HRESULT Init(KRLScene* pRLScene);
	HRESULT Exit();

	HRESULT AttachSceneObject(DWORD dwObject, DWORD dwObjectType);
	HRESULT DetachSceneObject(DWORD dwObject, DWORD dwObjectType);
	
	HRESULT Show(int nForceRelationType);
	HRESULT Hide();

protected:
	HRESULT OnPlayAnimationFinish(IKG3DAnimationController* pController);
	virtual HRESULT OnTagPlayingSFX(IKG3DAnimationController* pController, IKG3DModel* pModelSFX, TagSFXCallBackExtraInfo* pExtraInfo);
    virtual HRESULT OnMotionNotify(IKG3DAnimationController* pController, DWORD dwMotionInfo, TagMotionCallBackExtraInfo* pMotionCallBackExtraInfo);

private:
	HRESULT LoadFile();
	HRESULT UnloadFile();

	HRESULT AdjustScale();

	float GetDefaultScale() const;
    float GetCharcterScale() const;

private:
	IKG3DModel* m_ap3DModel[RL_FORCE_RELATION_COUNT];
	KRLScene* m_pRLScene;

	float m_aModelDiagonal[RL_FORCE_RELATION_COUNT];

	DWORD m_dwAttachedSceneObjectID;
	DWORD m_dwAttachedSceneObjectType;

	int m_nRLForceRelation;

    D3DXVECTOR3 m_vPosition;
};

#endif // CASE_ACTION_OBJECT_KRLTARGET_H