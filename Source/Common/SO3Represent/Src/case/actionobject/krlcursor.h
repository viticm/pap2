#ifndef CASE_ACTION_OBJECT_KRLCURSOR_H
#define CASE_ACTION_OBJECT_KRLCURSOR_H

#include "../gameworld/ktabledefine.h"
#include "KG3DEngine/KG3DInterface.h"

class KRLScene;

class KRLCursor : public IKG3DAnimationEventHandler
{
public:
	KRLCursor();
	virtual ~KRLCursor();

	int Update(double fTime, double fTimeLast, DWORD dwGameLoop, BOOL bFrame);

	HRESULT Init(KRLScene* pRLScene);
	HRESULT Exit();

	HRESULT Show(DWORD dwSkillID, DWORD dwSkillLevel, ROLE_TYPE nRoleType);
	HRESULT Hide();

	KRLScene* GetScene();

protected:
	virtual HRESULT OnPlayAnimationFinish(IKG3DAnimationController* pController);
	virtual HRESULT OnTagPlayingSFX(IKG3DAnimationController* pController, IKG3DModel* pModelSFX, TagSFXCallBackExtraInfo* pExtraInfo);
	virtual HRESULT OnMotionNotify(IKG3DAnimationController* pController, DWORD dwMotionInfo, TagMotionCallBackExtraInfo* pMotionCallBackExtraInfo);

	int UpdatePosition();
    
private:
	IKG3DModel* m_p3DModel;
	KRLScene*	m_pRLScene;
	BOOL		m_bVisible;
};
int GetRLCursorPosition(KRLScene* pRLScene, D3DXVECTOR3* pvPosition);
int ShowRLCursorEffect(KRLScene* pRLScene, KRLCursorEffectModel const* pcCursorEffectModel);

#endif // CASE_ACTION_OBJECT_KRLCURSOR_H