#ifndef UI_CASE_K_UI_3D_MODEL_EVENT_HANDLER_H
#define UI_CASE_K_UI_3D_MODEL_EVENT_HANDLER_H

#include "KG3DEngine/KG3DInterface.h"

class KUI3DModelEventHandler : public IKG3DAnimationEventHandler
{
public:
	virtual HRESULT OnPlayAnimationFinish(IKG3DAnimationController* pController);
	virtual HRESULT OnTagPlayingSFX(IKG3DAnimationController* pController, IKG3DModel* pModelSFX, TagSFXCallBackExtraInfo* pExtraInfo);
    virtual HRESULT OnMotionNotify(IKG3DAnimationController* pController, DWORD dwMotionInfo, TagMotionCallBackExtraInfo *pMotionCallBackExtraInfo);

	static IKG3DAnimationEventHandler& GetSelf();
};

#endif // UI_CASE_K_UI_3D_MODEL_EVENT_HANDLER_H


