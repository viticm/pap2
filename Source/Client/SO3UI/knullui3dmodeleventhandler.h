#ifndef UI_CASE_K_NULL_UI_3D_MODEL_EVENT_HANDLER_H
#define UI_CASE_K_NULL_UI_3D_MODEL_EVENT_HANDLER_H

#include "KG3DEngine/KG3DInterface.h"

class KNullUI3DModelEventHandler : public IKG3DAnimationEventHandler
{
public:
	virtual ~KNullUI3DModelEventHandler();

	virtual HRESULT OnPlayAnimationFinish(IKG3DAnimationController* pController);
	virtual HRESULT OnTagPlayingSFX(IKG3DAnimationController* pController, IKG3DModel* pModelSFX, TagSFXCallBackExtraInfo* pExtraInfo);
	virtual HRESULT OnMotionNotify(IKG3DAnimationController* pController, DWORD dwMotionInfo, TagMotionCallBackExtraInfo *pMotionCallBackExtraInfo);
};

#endif // UI_CASE_K_NULL_UI_3D_MODEL_EVENT_HANDLER_H


