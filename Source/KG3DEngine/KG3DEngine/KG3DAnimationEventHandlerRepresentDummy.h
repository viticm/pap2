#pragma once
#include "kg3dinterface.h"

class KG3DAnimationEventHandlerRepresentDummy :
    public IKG3DAnimationEventHandler
{
public:
    KG3DAnimationEventHandlerRepresentDummy(void);
    ~KG3DAnimationEventHandlerRepresentDummy(void);
    virtual HRESULT OnPlayAnimationFinish(IKG3DAnimationController* pController);
    virtual HRESULT OnTagPlayingSFX(IKG3DAnimationController* pController, IKG3DModel* pModel, TagSFXCallBackExtraInfo* pCallBackExtraInfo);
    virtual HRESULT OnMotionNotify(IKG3DAnimationController* pController, DWORD dwMotionInfo, TagMotionCallBackExtraInfo *pMotionCallBackExtraInfo);
    IKG3DModel* m_pModel;
};

extern KG3DAnimationEventHandlerRepresentDummy* g_pRepresentEventHandlerDummy;
