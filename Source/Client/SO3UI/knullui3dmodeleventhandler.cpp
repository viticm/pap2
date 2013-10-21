#include "stdafx.h"
#include "./knullui3dmodeleventhandler.h"


KNullUI3DModelEventHandler::~KNullUI3DModelEventHandler()
{
}


HRESULT KNullUI3DModelEventHandler::OnPlayAnimationFinish(IKG3DAnimationController* pController)
{
	return E_NOTIMPL;
}

HRESULT KNullUI3DModelEventHandler::OnTagPlayingSFX(IKG3DAnimationController* pController, IKG3DModel* pModelSFX, TagSFXCallBackExtraInfo* pExtraInfo)
{
	return E_NOTIMPL;
}

HRESULT KNullUI3DModelEventHandler::OnMotionNotify(IKG3DAnimationController* pController, DWORD dwMotionInfo, TagMotionCallBackExtraInfo *pMotionCallBackExtraInfo)
{
	return E_NOTIMPL;
}


