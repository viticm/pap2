#include "stdafx.h"
#include "SO3UI/IKGUI.h"
#include "./k3denginescripttable.h"
#include "./kui3dmodeleventhandler.h"
#include "./knullui3dmodeleventhandler.h"
#include "./SO3UI.h"

HRESULT KUI3DModelEventHandler::OnPlayAnimationFinish(IKG3DAnimationController* pController)
{
	lua_State* L = NULL; 
	IKG3DModel* p3DModel = NULL;

	KGLOG_PROCESS_ERROR(pController);

	p3DModel = pController->GetModel();
	KGLOG_PROCESS_ERROR(p3DModel);

	L = g_pSO3UI->m_pUI->GetLuaState();
	KGLOG_PROCESS_ERROR(L);

	lua_pushstring(L, "argu");
	if (!(K3DEngineScriptTable::NewUserData(L, p3DModel)))
		lua_pushnil(L);
	lua_settable(L, LUA_GLOBALSINDEX);
	
	g_pSO3UI->m_pUI->FireEvent("KG3D_PLAY_ANIMAION_FINISHED", 0);

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KUI3DModelEventHandler::OnTagPlayingSFX(IKG3DAnimationController* pController, IKG3DModel* pModelSFX, TagSFXCallBackExtraInfo* pExtraInfo)
{
	return E_NOTIMPL;
}

HRESULT KUI3DModelEventHandler::OnMotionNotify(IKG3DAnimationController* pController, DWORD dwMotionInfo, TagMotionCallBackExtraInfo *pMotionCallBackExtraInfo)
{
    return S_OK;
}


IKG3DAnimationEventHandler& KUI3DModelEventHandler::GetSelf()
{
	static KUI3DModelEventHandler s_self;
	return s_self;
}


