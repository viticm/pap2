#include "stdafx.h"
#include "SO3UI/IKGUI.h"
#include "SO3World/Global.h"
#include "./k3denginescripttable.h"
#include "./krepresentscripttable.h"
#include "./SO3UI.h"


void KRepresentScriptTable::Load()
{
	static luaL_reg s_aGlobalFunctionTable[] =
	{
		{ "ResetActiveObject",								LuaResetActiveObject },

		{ "Camera_Zoom",									LuaCamera_Zoom },
		{ "Camera_BeginDrag",								LuaCamera_BeginDrag },
		{ "Camera_EndDrag",									LuaCamera_EndDrag },
		{ "Camera_HasDragged",								LuaCamera_HasDragged },
        { "Camera_SetParams",								LuaCamera_SetParams },
        { "Camera_GetParams",								LuaCamera_GetParams },
        { "Camera_SetRTParams",								LuaCamera_SetRTParams },
        { "Camera_GetRTParams",								LuaCamera_GetRTParams },
        { "Camera_EnableControl",							LuaCamera_EnableControl },
        { "Camera_ToggleControl",							LuaCamera_ToggleControl },
        { "Camera_SetView",                                 LuaCamera_SetView },
        { "TurnToFaceDirection",                            LuaCamera_TurnToFaceDirection },
        { "Camera_GetYawPitchRoll",							LuaCamera_GetYawPitchRoll },
        { "Camera_SwitchMode",                              LuaCamera_SwitchMode },
        { "IsCharacterMoving",                              LuaCamera_IsCharacterMoving },
        { "MouseControlMoveEnable",                         LuaCamera_MouseControlMoveEnable},
        { "Camera_LockControl",                             LuaCamera_LockControl },
        { "GetCameraInitParams", LuaGetCameraInitParams },

        { "StartFollow",                                    LuaCamera_StartFollow },
        { "StopFollow",                                     LuaCamera_StopFollow },
        { "IsFollowing",                                    LuaCamera_IsFollowing },

        { "AutoMoveToPoint",                                LuaCamera_AutoMoveToPoint },
        { "AutoMoveToTarget",                               LuaCamera_AutoMoveToTarget },
        { "IsAutoMoving",                                   LuaCamera_IsAutoMoving },

        { "ShowCursorEffect",                               LuaCursor_ShowCursorEffect },

		{ "Scene_New",										LuaScene_New },
		{ "Scene_Load",										LuaScene_Load },
		{ "Scene_Delete",									LuaScene_Delete },

		{ "Scene_AddOutputWindow",							LuaScene_AddOutputWindow },
		{ "Scene_RemoveOutputWindow",						LuaScene_RemoveOutputWindow },

		{ "Scene_SelectObject",								LuaScene_SelectObject },
		{ "Scene_SelectGround",								LuaScene_SelectGround },
        { "Scene_GetCharacterTop",							LuaScene_GetCharacterTop },

        { "Scene_ScenePointToScreenPoint",                  LuaScene_ScenePointToScreenPoint },
		{ "Scene_GameWorldPositionToScenePosition",			LuaScene_GameWorldPositionToScenePosition },
		{ "Scene_ScenePositionToGameWorldPosition",			LuaScene_ScenePositionToGameWorldPosition },

		{ "SceneObject_SetAlpha",							LuaSceneObject_SetAlpha },
		{ "SceneObject_SetBrightness",						LuaSceneObject_SetBrightness },

		{ "SceneObject_SetSelectable",						LuaSceneObject_SetSelectable },
        { "SceneObject_SetTitleEffect",                     LuaSceneObject_SetTitleEffect },

		// AOE 选择特效
		{ "Selection_ShowSFX",								LuaSelection_ShowSFX },
		{ "Selection_HideSFX",								LuaSelection_HideSFX },

		// 目标选择特效
		{ "TargetSelection_AttachSceneObject",				LuaTargetSelection_AttachSceneObject },
		{ "TargetSelection_DetachSceneObject",				LuaTargetSelection_DetachSceneObject },
		{ "TargetSelection_ShowSFX",						LuaTargetSelection_ShowSFX },
		{ "TargetSelection_HideSFX",						LuaTargetSelection_HideSFX },

		{ "Player_GetAnimationResource",					LuaPlayer_GetAnimationResource },
		{ "Player_GetEquipResource",						LuaPlayer_GetEquipResource },

        { "Player_GetRidesAnimationResource",               LuaPlayer_GetRidesAnimationResource },
        { "Player_GetRidesEquipResource",					LuaPlayer_GetRidesEquipResource },

		{ "NPC_GetAnimationResource",						LuaNPC_GetAnimationResource },
		{ "NPC_GetEquipResource",							LuaNPC_GetEquipResource },
		
        { "Global_SetTopHeadFlag",							LuaGlobal_SetTopHeadFlag },
        { "Global_UpdateHeadTopPosition",					LuaGlobal_UpdateHeadTopPosition },

		{ "GetMinimapLayer",								LuaSence_GetMinimapLayer },
		{ "GetRegionInfo",									LuaSence_GetRegionInfo },

        { "rlcmd",                                          LuaRepresentCommand },

		{ "Character_PlayAnimation",                        LuaCharacter_PlayAnimation },
		{ "Character_StopAnimation",                        LuaCharacter_StopAnimation },
		{ "Character_PlaySound",                            LuaCharacter_PlaySound },
		{ "Character_StopSound",                            LuaCharacter_StopSound },

		{ NULL,												NULL },
	};

	g_pSO3UI->m_pUI->RegisterFunctions(s_aGlobalFunctionTable);
}


int KRepresentScriptTable::LuaResetActiveObject(Lua_State* L)
{
	BOOL bRetCode = FALSE;
    HRESULT hr = E_FAIL;
	LPCSTR szName = NULL;

	ASSERT(L);

	bRetCode = Lua_GetTopIndex(L) == 0;
	KGLOG_PROCESS_ERROR(bRetCode);

	hr = g_pSO3UI->m_pRepresent->ResetActiveObject();
	KGLOG_COM_PROCESS_ERROR(hr);

	return 0;
Exit0:
	return 0;
}

int KRepresentScriptTable::LuaCamera_Zoom(Lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	DWORD dwScene = UINT_MAX;
	int nParamCount = 0;
	float fScale = 0.0f;

	ASSERT(L);

	nParamCount = Lua_GetTopIndex(L);

	bRetCode = nParamCount == 1 || nParamCount == 2;
	KGLOG_PROCESS_ERROR(bRetCode);

	if (nParamCount == 1)
	{
		hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwScene);
		KGLOG_COM_PROCESS_ERROR(hr);

		fScale = static_cast<float>(Lua_ValueToNumber(L, 1));
	}
	else
	{
		dwScene = static_cast<DWORD>(Lua_ValueToNumber(L, 1));
		fScale = static_cast<float>(Lua_ValueToNumber(L, 2));
	}

	hr = g_pSO3UI->m_pRepresent->ZoomCamera(dwScene, fScale);
	KGLOG_COM_PROCESS_ERROR(hr);

	return 0;
Exit0:
	return 0;
}


int KRepresentScriptTable::LuaCamera_BeginDrag(Lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	DWORD dwScene = UINT_MAX;
	int nParamCount = 0;
	int nX = 0;
	int nY = 0;

	ASSERT(L);

	nParamCount = Lua_GetTopIndex(L);

	bRetCode = nParamCount == 0 || nParamCount == 1;
	KGLOG_PROCESS_ERROR(bRetCode);

	if (nParamCount == 0)
	{
		hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwScene);
		KGLOG_COM_PROCESS_ERROR(hr);
	}
	else
	{
		dwScene = static_cast<DWORD>(Lua_ValueToNumber(L, 1));
	}

	hr = g_pSO3UI->m_pRepresent->BeginDragCamera(dwScene, nX, nY);
	KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
	lua_pushnumber(L, nX);
	lua_pushnumber(L, nY);
	return 2;
}


int KRepresentScriptTable::LuaCamera_EndDrag(Lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	DWORD dwScene = UINT_MAX;
	int nParamCount = 0;
	int nX = 0;
	int nY = 0;

	ASSERT(L);

	nParamCount = lua_gettop(L);

	bRetCode = nParamCount == 2 || nParamCount == 3;
	KGLOG_PROCESS_ERROR(bRetCode);

	if (nParamCount == 2)
	{
		hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwScene);
		KGLOG_COM_PROCESS_ERROR(hr);

		nX = (int)lua_tonumber(L, 1);
		nY = (int)lua_tonumber(L, 2);
	}
	else
	{
		dwScene = (DWORD)lua_tonumber(L, 1);
		nX = (int)lua_tonumber(L, 2);
		nY = (int)lua_tonumber(L, 3);
	}

	hr = g_pSO3UI->m_pRepresent->EndDragCamera(dwScene, nX, nY);
	KGLOG_COM_PROCESS_ERROR(hr);

	return 0;
Exit0:
	return 0;
}


int KRepresentScriptTable::LuaCamera_HasDragged(Lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	DWORD dwScene = UINT_MAX;
	int nParamCount = 0;
	BOOL bCamera = FALSE;
    BOOL bObject = FALSE;
    float fMaxMouseX = 0.0f;
    float fMaxMouseY = 0.0f;
    float fFinalMouseX = 0.0f;
    float fFinalMouseY = 0.0f;

	ASSERT(L);

	nParamCount = Lua_GetTopIndex(L);
	
	bRetCode = nParamCount == 0 || nParamCount == 1;
	KGLOG_PROCESS_ERROR(bRetCode);

	if (nParamCount == 0)
	{
		hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwScene);
		KGLOG_COM_PROCESS_ERROR(hr);
	}
	else
	{
		dwScene = static_cast<DWORD>(Lua_ValueToNumber(L, 1));
	}

    hr = g_pSO3UI->m_pRepresent->HasCameraDragged(
        dwScene,
        bCamera,
        bObject,
        fMaxMouseX,
        fMaxMouseY,
        fFinalMouseX,
        fFinalMouseY
    );
	KGLOG_COM_PROCESS_ERROR(hr);

	Lua_PushBoolean(L, bCamera);
    Lua_PushBoolean(L, bObject);
    Lua_PushNumber(L, fMaxMouseX);
    Lua_PushNumber(L, fMaxMouseY);
    Lua_PushNumber(L, fFinalMouseX);
    Lua_PushNumber(L, fFinalMouseY);
	return 6;
Exit0:
	return 0;
}


int KRepresentScriptTable::LuaCamera_SetParams(Lua_State* L)
{
    BOOL bRetCode = FALSE;
    HRESULT hr = E_FAIL;
    DWORD dwScene = UINT_MAX;
    int nParamCount = 0;
    KRepresentCameraParams Params;

    ASSERT(L);

    nParamCount = Lua_GetTopIndex(L);

    bRetCode = nParamCount == 5 || nParamCount == 6;
    KGLOG_PROCESS_ERROR(bRetCode);

    if (nParamCount == 5)
    {
        hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwScene);
        KGLOG_COM_PROCESS_ERROR(hr);

        Params.fDragSpeed = (float)lua_tonumber(L, 1);
        Params.fMaxCameraDistance = (float)lua_tonumber(L, 2);
        Params.fSpringResetSpeed = (float)lua_tonumber(L, 3);
        Params.fCameraResetSpeed = (float)lua_tonumber(L, 4);
		Params.nResetMode = (int)lua_tonumber(L, 5);
    }
    else
    {
        dwScene = (DWORD)lua_tonumber(L, 1);
        Params.fDragSpeed = (float)lua_tonumber(L, 2);
        Params.fMaxCameraDistance = (float)lua_tonumber(L, 3);
        Params.fSpringResetSpeed = (float)lua_tonumber(L, 4);
        Params.fCameraResetSpeed = (float)lua_tonumber(L, 5);
		Params.nResetMode = (int)lua_tonumber(L, 6);
    }

    hr = g_pSO3UI->m_pRepresent->SetCameraParams(dwScene, Params);
    KGLOG_COM_PROCESS_ERROR(hr);

    return 0;
Exit0:
    return 0;
}

int KRepresentScriptTable::LuaCamera_GetParams(Lua_State* L)
{
    BOOL bRetCode = FALSE;
    HRESULT hr = E_FAIL;
    DWORD dwScene = UINT_MAX;
    int nParamCount = 0;
    KRepresentCameraParams Params;

    ASSERT(L);

    nParamCount = Lua_GetTopIndex(L);

    bRetCode = nParamCount == 0 || nParamCount == 1;
    KGLOG_PROCESS_ERROR(bRetCode);

    if (nParamCount == 0)
    {
        hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwScene);
        KGLOG_COM_PROCESS_ERROR(hr);
    }
    else
    {
        dwScene = static_cast<DWORD>(Lua_ValueToNumber(L, 1));
    }

    hr = g_pSO3UI->m_pRepresent->GetCameraParams(dwScene, Params);
    KGLOG_COM_PROCESS_ERROR(hr);

    lua_pushnumber(L, Params.fDragSpeed);
    lua_pushnumber(L, Params.fMaxCameraDistance);
    lua_pushnumber(L, Params.fSpringResetSpeed);
    lua_pushnumber(L, Params.fCameraResetSpeed);
    lua_pushnumber(L, Params.nResetMode);
    return 5;
Exit0:
    return 0;
}

int KRepresentScriptTable::LuaCamera_SetRTParams(Lua_State* L)
{
    BOOL bRetCode = FALSE;
    HRESULT hr = E_FAIL;
    DWORD dwScene = UINT_MAX;
    int nParamCount = 0;
    KRepresentCameraRTParams Params;

    ASSERT(L);

    nParamCount = Lua_GetTopIndex(L);

    bRetCode = nParamCount == 3 || nParamCount == 4;
    KGLOG_PROCESS_ERROR(bRetCode);

    if (nParamCount == 3)
    {
        hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwScene);
        KGLOG_COM_PROCESS_ERROR(hr);

        Params.fCameraToObjectEyeScale = (float)lua_tonumber(L, 1);
        Params.fYaw = (float)lua_tonumber(L, 2);
        Params.fPitch = (float)lua_tonumber(L, 3);
    }
    else
    {
        dwScene = (DWORD)lua_tonumber(L, 1);
        Params.fCameraToObjectEyeScale = (float)lua_tonumber(L, 2);
        Params.fYaw = (float)lua_tonumber(L, 3);
        Params.fPitch = (float)lua_tonumber(L, 4);
    }

    hr = g_pSO3UI->m_pRepresent->SetCameraRTParams(dwScene, Params);
    KGLOG_COM_PROCESS_ERROR(hr);

    return 0;
Exit0:
    return 0;
}

int KRepresentScriptTable::LuaCamera_GetRTParams(Lua_State* L)
{
    BOOL bRetCode = FALSE;
    HRESULT hr = E_FAIL;
    DWORD dwScene = UINT_MAX;
    int nParamCount = 0;
    KRepresentCameraRTParams Params;

    ASSERT(L);

    nParamCount = Lua_GetTopIndex(L);

    bRetCode = nParamCount == 0 || nParamCount == 1;
    KGLOG_PROCESS_ERROR(bRetCode);

    if (nParamCount == 0)
    {
        hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwScene);
        KGLOG_COM_PROCESS_ERROR(hr);
    }
    else
    {
        dwScene = static_cast<DWORD>(Lua_ValueToNumber(L, 1));
    }

    hr = g_pSO3UI->m_pRepresent->GetCameraRTParams(dwScene, Params);
    KGLOG_COM_PROCESS_ERROR(hr);

    lua_pushnumber(L, Params.fCameraToObjectEyeScale);
    lua_pushnumber(L, Params.fYaw);
    lua_pushnumber(L, Params.fPitch);
    return 3;
Exit0:
    return 0;
}

int KRepresentScriptTable::LuaCamera_EnableControl(Lua_State* L)
{
    BOOL bRetCode = FALSE;
    HRESULT hr = E_FAIL;
    DWORD dwScene = UINT_MAX;
    int nParamCount = 0;
    size_t nIndex = 0;
    BOOL bEnable = FALSE;

    ASSERT(L);

    nParamCount = Lua_GetTopIndex(L);

    bRetCode = nParamCount == 2 || nParamCount == 3;
    KGLOG_PROCESS_ERROR(bRetCode);

    if (nParamCount == 2)
    {
        hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwScene);
        KGLOG_COM_PROCESS_ERROR(hr);

        nIndex = (size_t)lua_tonumber(L, 1);
		bEnable = lua_toboolean(L, 2);
    }
    else
    {
        dwScene = static_cast<DWORD>(Lua_ValueToNumber(L, 1));
        nIndex = (size_t)lua_tonumber(L, 2);
		bEnable = lua_toboolean(L, 3);
    }

    hr = g_pSO3UI->m_pRepresent->EnableControlCamera(dwScene, nIndex, bEnable);
    KGLOG_COM_PROCESS_ERROR(hr);

    return 0;
Exit0:
    return 0;
}

int KRepresentScriptTable::LuaCamera_ToggleControl(Lua_State* L)
{
    BOOL bRetCode = FALSE;
    HRESULT hr = E_FAIL;
    DWORD dwScene = UINT_MAX;
    int nParamCount = 0;
    size_t nIndex = 0;

    ASSERT(L);

    nParamCount = Lua_GetTopIndex(L);

    bRetCode = nParamCount == 1 || nParamCount == 2;
    KGLOG_PROCESS_ERROR(bRetCode);

    if (nParamCount == 1)
    {
        hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwScene);
        KGLOG_COM_PROCESS_ERROR(hr);

        nIndex = (size_t)lua_tonumber(L, 1);
    }
    else
    {
        dwScene = static_cast<DWORD>(Lua_ValueToNumber(L, 1));
        nIndex = (size_t)lua_tonumber(L, 2);
    }

    hr = g_pSO3UI->m_pRepresent->ToggleControlCamera(dwScene, nIndex);
    KGLOG_COM_PROCESS_ERROR(hr);

    return 0;
Exit0:
    return 0;
}

int KRepresentScriptTable::LuaCamera_SetView(Lua_State* L)
{
    BOOL bRetCode = FALSE;
    HRESULT hr = E_FAIL;
    DWORD dwScene = UINT_MAX;
    int nParamCount = 0;
    float fAngle= 0.0f;

    ASSERT(L);

    nParamCount = Lua_GetTopIndex(L);

    bRetCode = nParamCount == 1 || nParamCount == 2;
    KGLOG_PROCESS_ERROR(bRetCode);

    if (nParamCount == 1)
    {
        hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwScene);
        KGLOG_COM_PROCESS_ERROR(hr);

        fAngle = static_cast<float>(Lua_ValueToNumber(L, 1));
    }
    else
    {
        dwScene = static_cast<DWORD>(Lua_ValueToNumber(L, 1));
        fAngle = static_cast<float>(Lua_ValueToNumber(L, 2));
    }

    hr = g_pSO3UI->m_pRepresent->SetView(dwScene, fAngle);
    KGLOG_COM_PROCESS_ERROR(hr);

    return 0;
Exit0:
    return 0;
}

int KRepresentScriptTable::LuaCamera_TurnToFaceDirection(Lua_State* L)
{
    BOOL bRetCode = FALSE;
    HRESULT hr = E_FAIL;
    DWORD dwScene = UINT_MAX;
    int nParamCount = 0;

    ASSERT(L);

    nParamCount = Lua_GetTopIndex(L);

    bRetCode = nParamCount == 0 ||nParamCount == 1;
    KGLOG_PROCESS_ERROR(bRetCode);

    if (nParamCount == 0)
    {
        hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwScene);
        KGLOG_COM_PROCESS_ERROR(hr);
    }
    else
    {
        dwScene = static_cast<DWORD>(Lua_ValueToNumber(L, 1));
    }

    hr = g_pSO3UI->m_pRepresent->TurnToFaceDirection(dwScene);
    KGLOG_COM_PROCESS_ERROR(hr);

    return 0;
Exit0:
    return 0;
}

int KRepresentScriptTable::LuaCamera_GetYawPitchRoll(Lua_State* L)
{
    BOOL bRetCode = FALSE;
    HRESULT hr = E_FAIL;
    DWORD dwScene = UINT_MAX;
    float fYaw = 0.0f;
    float fPitch = 0.0f;
    float fRoll = 0.0f;
    int nParamCount = 0;

    ASSERT(L);

    nParamCount = Lua_GetTopIndex(L);

    bRetCode = nParamCount == 0;
    KGLOG_PROCESS_ERROR(bRetCode);

    hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwScene);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pSO3UI->m_pRepresent->GetCameraYawPitchRoll(dwScene, fYaw, fPitch, fRoll);
    KGLOG_COM_PROCESS_ERROR(hr);

    Lua_PushNumber(L, fYaw);
    Lua_PushNumber(L, fPitch);
    Lua_PushNumber(L, fRoll);
    return 3;
Exit0:
    return 0;
}

int KRepresentScriptTable::LuaCamera_SwitchMode(Lua_State* L)
{
    BOOL bRetCode = FALSE;
    HRESULT hr = E_FAIL;
    DWORD dwScene = UINT_MAX;
    int nParamCount = 0;
    int nCameraMode = 0;

    ASSERT(L);

    nParamCount = Lua_GetTopIndex(L);

    bRetCode = nParamCount == 1;
    KGLOG_PROCESS_ERROR(bRetCode);

    hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwScene);
    KGLOG_COM_PROCESS_ERROR(hr);

    nCameraMode = (int)lua_tonumber(L, 1);

    hr = g_pSO3UI->m_pRepresent->SwitchCameraMode(dwScene, nCameraMode);
    KGLOG_COM_PROCESS_ERROR(hr);

    return 0;
Exit0:
    return 0;
}

int KRepresentScriptTable::LuaCamera_IsCharacterMoving(Lua_State* L)
{
    BOOL bRetCode = FALSE;
    HRESULT hr = E_FAIL;
    DWORD dwScene = UINT_MAX;
    int nParamCount = 0;
    int nMoveStyle = 0;
    int nIsMoving = false;

    ASSERT(L);

    nParamCount = Lua_GetTopIndex(L);

    bRetCode = nParamCount == 1 || nParamCount == 2;
    KGLOG_PROCESS_ERROR(bRetCode);

    if (nParamCount == 1)
    {
        hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwScene);
        KGLOG_COM_PROCESS_ERROR(hr);

        nMoveStyle = static_cast<int>(Lua_ValueToNumber(L, 1));
    }
    else
    {
        dwScene = static_cast<DWORD>(Lua_ValueToNumber(L, 1));
        nMoveStyle = static_cast<int>(Lua_ValueToNumber(L, 2));
    }

    hr = g_pSO3UI->m_pRepresent->IsCharacterMoving(dwScene, nMoveStyle, &nIsMoving);
    KGLOG_COM_PROCESS_ERROR(hr);

    Lua_PushBoolean(L, nIsMoving ? true : false);

    return 1;
Exit0:
    return 0;
}

int KRepresentScriptTable::LuaCamera_MouseControlMoveEnable(Lua_State* L)
{
    BOOL bRetCode = FALSE;
    HRESULT hr = E_FAIL;
    DWORD dwScene = UINT_MAX;
    int nParamCount = 0;
    BOOL bEnable = FALSE;

    ASSERT(L);

    nParamCount = Lua_GetTopIndex(L);

    bRetCode = nParamCount == 1 || nParamCount == 2;
    KGLOG_PROCESS_ERROR(bRetCode);

    if (nParamCount == 1)
    {
        hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwScene);
        KGLOG_COM_PROCESS_ERROR(hr);

        bEnable = (BOOL)Lua_ValueToBoolean(L, 1);
    }
    else
    {
        dwScene = static_cast<DWORD>(Lua_ValueToNumber(L, 1));
        bEnable = (BOOL)Lua_ValueToBoolean(L, 2);
    }

    hr = g_pSO3UI->m_pRepresent->MouseControlMoveEnable(dwScene, bEnable);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
    return 0;
}

int KRepresentScriptTable::LuaCamera_LockControl(Lua_State* L)
{
    HRESULT hr = E_FAIL;
    DWORD dwScene = UINT_MAX;
    DWORD dwFrame = 0;
    int nParamCount = 0;

    ASSERT(L);

    nParamCount = lua_gettop(L);

    KGLOG_PROCESS_ERROR(nParamCount == 1 || nParamCount == 2);

    if (nParamCount == 1)
    {
        hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwScene);
        KGLOG_COM_PROCESS_ERROR(hr);

        dwFrame = (DWORD)lua_tonumber(L, 1);
    }
    else
    {
        dwScene = (DWORD)lua_tonumber(L, 1);
        dwFrame = (DWORD)lua_tonumber(L, 2);
    }

    hr = g_pSO3UI->m_pRepresent->LockControl(dwScene, dwFrame);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
    return 0;
}

int KRepresentScriptTable::LuaGetCameraInitParams(Lua_State* L)
{
    int nResult = 0;
    int nRetCode = false;
    int nCameraResetMode = 0;
    float fCameraDragSpeed = 0.0f;
    float fCameraMaxDistance = 0.0f;
    float fCameraResetSpeed = 0.0f;

    nRetCode = g_pSO3UI->m_pRepresent->GetCameraInitParams(
        &nCameraResetMode, &fCameraDragSpeed, &fCameraMaxDistance, &fCameraResetSpeed);
    KGLOG_PROCESS_ERROR(nRetCode);

    lua_newtable(L);

    lua_pushstring(L, "ResetMode");
    lua_pushnumber(L, nCameraResetMode);
    lua_settable(L, -3);

    lua_pushstring(L, "DragSpeed");
    lua_pushnumber(L, fCameraDragSpeed);
    lua_settable(L, -3);

    lua_pushstring(L, "MaxDistance");
    lua_pushnumber(L, fCameraMaxDistance);
    lua_settable(L, -3);

    lua_pushstring(L, "CameraResetSpeed");
    lua_pushnumber(L, fCameraResetSpeed);
    lua_settable(L, -3);
    
    nResult = 1;
Exit0:
    return nResult;
}

int KRepresentScriptTable::LuaCamera_StartFollow(Lua_State* L)
{
    HRESULT hr = E_FAIL;
    DWORD dwScene = UINT_MAX;
    int nParamCount = 0;
    DWORD dwTargetType = ttNoTarget;
    DWORD dwTargetID = 0;

    ASSERT(L);

    nParamCount = lua_gettop(L);

    KGLOG_PROCESS_ERROR(nParamCount == 2 || nParamCount == 3);

    if (nParamCount == 2)
    {
        hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwScene);
        KGLOG_COM_PROCESS_ERROR(hr);

        dwTargetType = (DWORD)lua_tonumber(L, 1);
        dwTargetID = (DWORD)lua_tonumber(L, 2);
    }
    else
    {
        dwScene = (DWORD)lua_tonumber(L, 1);
        dwTargetType = (DWORD)lua_tonumber(L, 2);
        dwTargetID = (DWORD)lua_tonumber(L, 3);
    }

    hr = g_pSO3UI->m_pRepresent->StartFollow(dwScene, dwTargetType, dwTargetID);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
    return 0;
}

int KRepresentScriptTable::LuaCamera_StopFollow(Lua_State* L)
{
    HRESULT hr = E_FAIL;
    DWORD dwScene = UINT_MAX;
    int nParamCount = 0;

    ASSERT(L);

    nParamCount = lua_gettop(L);

    KGLOG_PROCESS_ERROR(nParamCount == 0 || nParamCount == 1);

    if (nParamCount == 0)
    {
        hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwScene);
        KGLOG_COM_PROCESS_ERROR(hr);
    }
    else
    {
        dwScene = (DWORD)lua_tonumber(L, 1);
    }

    hr = g_pSO3UI->m_pRepresent->StopFollow(dwScene);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
    return 0;
}

int KRepresentScriptTable::LuaCamera_IsFollowing(Lua_State* L)
{
    HRESULT hr = E_FAIL;
    DWORD dwScene = UINT_MAX;
    BOOL bIsFollowing = FALSE;
    DWORD dwTargetType = ttNoTarget;
    DWORD dwTargetID = 0;
    int nParamCount = 0;

    ASSERT(L);

    nParamCount = lua_gettop(L);

    KGLOG_PROCESS_ERROR(nParamCount == 0 || nParamCount == 1);

    if (nParamCount == 0)
    {
        hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwScene);
        KGLOG_COM_PROCESS_ERROR(hr);
    }
    else
    {
        dwScene = (DWORD)lua_tonumber(L, 1);
    }

    hr = g_pSO3UI->m_pRepresent->IsFollowing(dwScene, &bIsFollowing, &dwTargetType, &dwTargetID);
    KGLOG_COM_PROCESS_ERROR(hr);

    Lua_PushBoolean(L, bIsFollowing);
    lua_pushnumber(L, dwTargetType);
    lua_pushnumber(L, dwTargetID);

    return 3;
Exit0:
    return 0;
}

int KRepresentScriptTable::LuaCamera_AutoMoveToPoint(Lua_State* L)
{
    HRESULT hr = E_FAIL;
    DWORD dwScene = UINT_MAX;
    int nParamCount = 0;
    D3DXVECTOR3 vPoint;

    ASSERT(L);

    nParamCount = lua_gettop(L);

    KGLOG_PROCESS_ERROR(nParamCount == 3 || nParamCount == 4);

    if (nParamCount == 3)
    {
        hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwScene);
        KGLOG_COM_PROCESS_ERROR(hr);

        vPoint.x = (float)lua_tonumber(L, 1);
        vPoint.y = (float)lua_tonumber(L, 2);
        vPoint.z = (float)lua_tonumber(L, 3);
    }
    else
    {
        dwScene = (DWORD)lua_tonumber(L, 1);
        vPoint.x = (float)lua_tonumber(L, 2);
        vPoint.y = (float)lua_tonumber(L, 3);
        vPoint.z = (float)lua_tonumber(L, 4);
    }

    hr = g_pSO3UI->m_pRepresent->AutoMoveToPoint(dwScene, vPoint);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
    return 0;
}

int KRepresentScriptTable::LuaCamera_AutoMoveToTarget(Lua_State* L)
{
    HRESULT hr = E_FAIL;
    DWORD dwScene = UINT_MAX;
    int nParamCount = 0;
    DWORD dwTargetType = ttNoTarget;
    DWORD dwTargetID = 0;

    ASSERT(L);

    nParamCount = lua_gettop(L);

    KGLOG_PROCESS_ERROR(nParamCount == 2 || nParamCount == 3);

    if (nParamCount == 2)
    {
        hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwScene);
        KGLOG_COM_PROCESS_ERROR(hr);

        dwTargetType = (DWORD)lua_tonumber(L, 1);
        dwTargetID = (DWORD)lua_tonumber(L, 2);
    }
    else
    {
        dwScene = (DWORD)lua_tonumber(L, 1);
        dwTargetType = (DWORD)lua_tonumber(L, 2);
        dwTargetID = (DWORD)lua_tonumber(L, 3);
    }

    hr = g_pSO3UI->m_pRepresent->AutoMoveToTarget(dwScene, dwTargetType, dwTargetID);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
    return 0;
}

int KRepresentScriptTable::LuaCamera_IsAutoMoving(Lua_State* L)
{
    HRESULT hr = E_FAIL;
    DWORD dwScene = UINT_MAX;
    int nParamCount = 0;
    BOOL bIsAutoMoving = FALSE;

    ASSERT(L);

    nParamCount = lua_gettop(L);

    KGLOG_PROCESS_ERROR(nParamCount == 0 || nParamCount == 1);

    if (nParamCount == 0)
    {
        hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwScene);
        KGLOG_COM_PROCESS_ERROR(hr);

    }
    else
    {
        dwScene = (DWORD)lua_tonumber(L, 1);
    }

    hr = g_pSO3UI->m_pRepresent->IsAutoMoving(dwScene, &bIsAutoMoving);
    KGLOG_COM_PROCESS_ERROR(hr);

    Lua_PushBoolean(L, bIsAutoMoving);

    return 1;
Exit0:
    return 0;
}

int KRepresentScriptTable::LuaCursor_ShowCursorEffect(Lua_State* L)
{
    HRESULT hr = E_FAIL;
    DWORD dwScene = UINT_MAX;
    int nParamCount = 0;
    DWORD dwCursorEffectID = 0;

    ASSERT(L);

    nParamCount = lua_gettop(L);

    KGLOG_PROCESS_ERROR(nParamCount == 1 || nParamCount == 2);

    if (nParamCount == 1)
    {
        hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwScene);
        KGLOG_COM_PROCESS_ERROR(hr);

        dwCursorEffectID = (DWORD)lua_tonumber(L, 1);
    }
    else
    {
        dwScene = (DWORD)lua_tonumber(L, 1);
        dwCursorEffectID = (DWORD)lua_tonumber(L, 2);
    }

    hr = g_pSO3UI->m_pRepresent->ShowCursorEffect(dwScene, dwCursorEffectID);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
    return 0;
}

int KRepresentScriptTable::LuaScene_New(Lua_State* L)
{
	BOOL bRetCode = FALSE;
    HRESULT hr = E_FAIL;
	DWORD dwSceneID = UINT_MAX;
	LPCSTR szFilePath = NULL;
	int nParamCount = 0;

	ASSERT(L);

	nParamCount = Lua_GetTopIndex(L);
	
	bRetCode = nParamCount == 1 || nParamCount == 2;
	KGLOG_PROCESS_ERROR(bRetCode);

	dwSceneID = (DWORD)Lua_ValueToNumber(L, 1);
	if (nParamCount == 2)
	{
		szFilePath = Lua_ValueToString(L, 2);
	}
	if (!szFilePath)
		szFilePath = "";

	hr = g_pSO3UI->m_pRepresent->NewScene(dwSceneID, SCENE_DISABLE_DYNAMICLOAD | SCENE_FIX_CAMRERANGLE, 0, 0, 0, szFilePath);
	KGLOG_COM_PROCESS_ERROR(hr);

	return 0;
Exit0:
	return 0;
}


int KRepresentScriptTable::LuaScene_Load(Lua_State* L)
{
	BOOL bRetCode = FALSE;
    HRESULT hr = E_FAIL;
	DWORD dwSceneID = UINT_MAX;
	LPCSTR szFilePath = NULL;
	int nParamCount = 0;

	ASSERT(L);

	nParamCount = Lua_GetTopIndex(L);
	
	bRetCode = nParamCount == 1 || nParamCount == 2;
	KGLOG_PROCESS_ERROR(bRetCode);

	dwSceneID = (DWORD)Lua_ValueToNumber(L, 1);
	if (nParamCount == 2)
	{
		szFilePath = Lua_ValueToString(L, 2);
	}
	if (!szFilePath)
		szFilePath = "";

	hr = g_pSO3UI->m_pRepresent->LoadScene(dwSceneID, SCENE_DISABLE_DYNAMICLOAD, 0, 0, 0, szFilePath);
	KGLOG_COM_PROCESS_ERROR(hr);

	return 0;
Exit0:
	return 0;
}


int KRepresentScriptTable::LuaScene_Delete(Lua_State* L)
{
	BOOL bRetCode = FALSE;
    HRESULT hr = E_FAIL;
	DWORD dwSceneID = UINT_MAX;

	ASSERT(L);

	bRetCode = Lua_GetTopIndex(L) == 1;
	KGLOG_PROCESS_ERROR(bRetCode);

	dwSceneID = static_cast<DWORD>(Lua_ValueToNumber(L, 1));

	hr = g_pSO3UI->m_pRepresent->DeleteScene(dwSceneID);
	KGLOG_COM_PROCESS_ERROR(hr);

	return 0;
Exit0:
	return 0;
}


int KRepresentScriptTable::LuaScene_AddOutputWindow(Lua_State* L)
{
	BOOL bRetCode = FALSE;
    HRESULT hr = E_FAIL;
	DWORD dwSceneID = UINT_MAX;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 1;
	KGLOG_PROCESS_ERROR(bRetCode);

	dwSceneID = static_cast<DWORD>(lua_tonumber(L, 1));

	hr = g_pSO3UI->m_pRepresent->AddSceneOutputWindow(dwSceneID, NULL);
	KGLOG_COM_PROCESS_ERROR(hr);

	return 0;
Exit0:
	return 0;
}


int KRepresentScriptTable::LuaScene_RemoveOutputWindow(Lua_State* L)
{
	BOOL bRetCode = FALSE;
    HRESULT hr = E_FAIL;
	DWORD dwSceneID = UINT_MAX;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 1;
	KGLOG_PROCESS_ERROR(bRetCode);

	dwSceneID = static_cast<DWORD>(lua_tonumber(L, 1));

	hr = g_pSO3UI->m_pRepresent->RemoveSceneOutputWindow(dwSceneID);
	KGLOG_COM_PROCESS_ERROR(hr);

	return 0;
Exit0:
	return 0;
}


int KRepresentScriptTable::LuaScene_SelectObject(Lua_State* L)
{
	BOOL bRetCode = FALSE;
    int nRetCode = false;
	HRESULT hr = E_FAIL;

	DWORD dwScene = UINT_MAX;
	int nParamCount = 0;
	int nX = 0;
	int nY = 0;
	float fXScale = 0.0f;
	float fYScale = 0.0f;
    KRepresentSceneObject Objects[256];
    size_t nObjectCount = 0;
    LPCSTR pszOption;

	ASSERT(L);

	nParamCount = lua_gettop(L);

	bRetCode = nParamCount == 1 || nParamCount == 2 || nParamCount == 3 || nParamCount == 4;
	KGLOG_PROCESS_ERROR(bRetCode);

	switch (nParamCount)
	{
	case 1:
        pszOption = lua_tostring(L, 1);
        KGLOG_PROCESS_ERROR(pszOption);

		hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwScene);
		KGLOG_COM_PROCESS_ERROR(hr);

		g_pSO3UI->GetCursorPosition(&nX, &nY);
		fXScale = (float)nX;
		fYScale = (float)nY;
		break;
	case 2:
        dwScene = (DWORD)(lua_tonumber(L, 1));
        pszOption = lua_tostring(L, 2);
        KGLOG_PROCESS_ERROR(pszOption);

		g_pSO3UI->GetCursorPosition(&nX, &nY);
		fXScale = (float)nX;
		fYScale = (float)nY;
		break;
	case 3:
		hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwScene);
		KGLOG_COM_PROCESS_ERROR(hr);

		fXScale = (float)(lua_tonumber(L, 1));
        fYScale = (float)(lua_tonumber(L, 2));
        pszOption = lua_tostring(L, 3);
        KGLOG_PROCESS_ERROR(pszOption);
		break;
	case 4:
		dwScene = (DWORD)(lua_tonumber(L, 1));
		fXScale = (float)(lua_tonumber(L, 2));
        fYScale = (float)(lua_tonumber(L, 3));
        pszOption = lua_tostring(L, 4);
        KGLOG_PROCESS_ERROR(pszOption);
		break;
	}

    lua_newtable(L);
    
    nRetCode = strcmp(pszOption, "nearest");
    if (nRetCode == 0)
    {
        nObjectCount = 1;
    }
    else
    {
        nObjectCount = _countof(Objects);
    }

	nObjectCount = g_pSO3UI->m_pRepresent->SelectPtInScene(dwScene, D3DXVECTOR2(fXScale, fYScale), Objects, nObjectCount);
	if (nObjectCount > 0)
	{
        for (unsigned i = 0; i < nObjectCount; i++)
        {
            int nType = g_pSO3UI->m_pRepresent->SceneObjectTypeToGameWorldObjectType(Objects[i].dwSceneObjectType);

            lua_pushnumber(L, i + 1);
            lua_newtable(L);

            lua_pushstring(L, "Type");
            lua_pushnumber(L, nType);
            lua_settable(L, -3);
            
            lua_pushstring(L, "ID");
            lua_pushnumber(L, Objects[i].dwSceneObjectID);
            lua_settable(L, -3);

            lua_settable(L, -3);
        }
	}
	else
	{
        lua_pushnumber(L, 1);
        lua_newtable(L);

        lua_pushstring(L, "Type");
        lua_pushnumber(L, ttNoTarget);
        lua_settable(L, -3);

        lua_pushstring(L, "ID");
        lua_pushnumber(L, 0);
        lua_settable(L, -3);

        lua_settable(L, -3);
	}

	return 1;
Exit0:
	return 0;
}

int KRepresentScriptTable::LuaScene_SelectGround(Lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	DWORD dwScene = UINT_MAX;
	int nParamCount = 0;
	int nX = 0;
	int nY = 0;
    int nGroundX = 0;
    int nGroundY = 0;
    int nGroundZ = 0;
	float fXScale = 0.0f;
	float fYScale = 0.0f;

	ASSERT(L);

	nParamCount = lua_gettop(L);

	bRetCode = nParamCount == 0 || nParamCount == 1 || nParamCount == 2 || nParamCount == 3;
	KGLOG_PROCESS_ERROR(bRetCode);

	switch (nParamCount)
	{
	case 0:
		hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwScene);
		KGLOG_COM_PROCESS_ERROR(hr);

		g_pSO3UI->GetCursorPosition(&nX, &nY);
		fXScale = (float)nX;
		fYScale = (float)nY;
		break;
	case 1:
		dwScene = (DWORD)(lua_tonumber(L, 1));
		g_pSO3UI->GetCursorPosition(&nX, &nY);
		fXScale = (float)nX;
		fYScale = (float)nY;
		break;
	case 2:
		hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwScene);
		KGLOG_COM_PROCESS_ERROR(hr);

		fXScale = (float)(lua_tonumber(L, 1));
		fYScale = (float)(lua_tonumber(L, 2));
		break;
	case 3:
		dwScene = (DWORD)(lua_tonumber(L, 1));
		fXScale = (float)(lua_tonumber(L, 2));
		fYScale = (float)(lua_tonumber(L, 3));
		break;
	}

	hr = g_pSO3UI->m_pRepresent->SelectPtInGround(dwScene, D3DXVECTOR2(fXScale, fYScale), nGroundX, nGroundY, nGroundZ);
	KGLOG_COM_PROCESS_ERROR(hr);

    lua_pushnumber(L, nGroundX);
    lua_pushnumber(L, nGroundY);
    lua_pushnumber(L, nGroundZ);
	return 3;
Exit0:
	return 0;
}

int KRepresentScriptTable::LuaScene_GetCharacterTop(Lua_State* L)
{
    BOOL bRetCode = FALSE;
    HRESULT hr = E_FAIL;
    DWORD dwCharacterID = 0;
    D3DXVECTOR3 vPosition;

    ASSERT(L);

	bRetCode = lua_gettop(L) == 1;
    KGLOG_PROCESS_ERROR(bRetCode);

    dwCharacterID = (DWORD)lua_tonumber(L, 1);

	hr = g_pSO3UI->m_pRepresent->GetCharacterTop(dwCharacterID, vPosition);    
	KG_COM_PROCESS_ERROR(hr);

    lua_pushnumber(L, vPosition.x);
    lua_pushnumber(L, vPosition.y);
    lua_pushnumber(L, vPosition.z);
    return 3;
Exit0:
	return 0;
}

int KRepresentScriptTable::LuaScene_ScenePointToScreenPoint(Lua_State* L)
{
    HRESULT hr = E_FAIL;
    DWORD dwScene = 0;
    D3DXVECTOR3 vSrceen;
    int nParamCount = 0;
    float fScreenX = 0.0f;
    float fScreenY = 0.0f;

    ASSERT(L);

    nParamCount = lua_gettop(L);
    KGLOG_PROCESS_ERROR(nParamCount == 3 || nParamCount == 4);

    if (nParamCount == 3)
    {
		hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwScene);
		KGLOG_COM_PROCESS_ERROR(hr);

        vSrceen.x = (float)lua_tonumber(L, 1);
        vSrceen.y = (float)lua_tonumber(L, 2);
        vSrceen.z = (float)lua_tonumber(L, 3);
    }
    else
    {
        dwScene = (DWORD)lua_tonumber(L, 1);
        vSrceen.x = (float)lua_tonumber(L, 2);
        vSrceen.y = (float)lua_tonumber(L, 3);
        vSrceen.z = (float)lua_tonumber(L, 4);
    }

	hr = g_pSO3UI->m_pRepresent->ScenePointToScreenPoint(dwScene, vSrceen, fScreenX, fScreenY);    
	KGLOG_COM_PROCESS_ERROR(hr);

    lua_pushnumber(L, fScreenX);
    lua_pushnumber(L, fScreenY);
    lua_pushboolean(L, hr == S_OK);

    return 3;
Exit0:
	return 0;
}

	
int KRepresentScriptTable::LuaScene_GameWorldPositionToScenePosition(Lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	DWORD dwScene = UINT_MAX;
	D3DXVECTOR3 vPosition;
	int nParamCount = 0;
	float fX = 0.0f;
	float fY = 0.0f;
	float fZ = 0.0f;
	BOOL bOnFloor = FALSE;

	ASSERT(L);

	nParamCount = Lua_GetTopIndex(L);
	
	bRetCode = nParamCount == 4 || nParamCount == 5;
	KGLOG_PROCESS_ERROR(bRetCode);

	if (nParamCount == 4)
	{
		hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwScene);
		KGLOG_COM_PROCESS_ERROR(hr);

		fX = (float)Lua_ValueToNumber(L, 1);
		fY = (float)Lua_ValueToNumber(L, 2);
		fZ = (float)Lua_ValueToNumber(L, 3);
		if (Lua_IsBoolean(L, 4))
			bOnFloor = (BOOL)Lua_ValueToBoolean(L, 4);
		else
			bOnFloor = (BOOL)(int)Lua_ValueToNumber(L, 4);
	}
	else
	{
		dwScene = (DWORD)Lua_ValueToNumber(L, 1);
		fX = (float)Lua_ValueToNumber(L, 2);
		fY = (float)Lua_ValueToNumber(L, 3);
		fZ = (float)Lua_ValueToNumber(L, 4);
		if (Lua_IsBoolean(L, 5))
			bOnFloor = (BOOL)Lua_ValueToBoolean(L, 5);
		else
			bOnFloor = (BOOL)(int)Lua_ValueToNumber(L, 5);
	}

	hr = g_pSO3UI->m_pRepresent->GameWorldPositionToScenePosition(vPosition, dwScene, fX, fY, fZ, bOnFloor);
	KGLOG_COM_PROCESS_ERROR(hr);

	Lua_PushNumber(L, vPosition.x);
	Lua_PushNumber(L, vPosition.y);
	Lua_PushNumber(L, vPosition.z);
	return 3;
Exit0:
	return 0;
}


int KRepresentScriptTable::LuaScene_ScenePositionToGameWorldPosition(Lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	DWORD dwScene = UINT_MAX;
	D3DXVECTOR3 vPosition;
	int nParamCount = 0;
	float fX = 0.0f;
	float fY = 0.0f;
	float fZ = 0.0f;

	ASSERT(L);

	nParamCount = Lua_GetTopIndex(L);
	
	bRetCode = nParamCount == 3 || nParamCount == 4;
	KGLOG_PROCESS_ERROR(bRetCode);

	if (nParamCount == 3)
	{
		hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwScene);
		KGLOG_COM_PROCESS_ERROR(hr);

		vPosition.x = static_cast<float>(Lua_ValueToNumber(L, 1));
		vPosition.y = static_cast<float>(Lua_ValueToNumber(L, 2));
		vPosition.z = static_cast<float>(Lua_ValueToNumber(L, 3));
	}
	else
	{
		dwScene = static_cast<DWORD>(Lua_ValueToNumber(L, 1));
		vPosition.x = static_cast<float>(Lua_ValueToNumber(L, 2));
		vPosition.y = static_cast<float>(Lua_ValueToNumber(L, 3));
		vPosition.z = static_cast<float>(Lua_ValueToNumber(L, 4));
	}

	hr = g_pSO3UI->m_pRepresent->ScenePositionToGameWorldPosition(dwScene, fX, fY, fZ, vPosition);
	KGLOG_COM_PROCESS_ERROR(hr);

	Lua_PushNumber(L, fX);
	Lua_PushNumber(L, fY);
	Lua_PushNumber(L, fZ);
	return 3;
Exit0:
	return 0;
}


int KRepresentScriptTable::LuaSceneObject_SetAlpha(Lua_State* L)
{
	int nRetCode = false;
    int nResult = false;
	HRESULT hr = E_FAIL;
    DWORD dwScene = 0;
	DWORD dwObject = 0;
	DWORD dwSceneObjectType = UINT_MAX;
    int nParamCount = 0;
	int nGameWorldObjectType = 0;
	float fScale = 0.0f;

	ASSERT(L);

    nParamCount = lua_gettop(L);
	KGLOG_PROCESS_ERROR(nParamCount == 3 || nParamCount == 4);
	
    if (nParamCount == 3)
	{
		hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwScene);
		KGLOG_COM_PROCESS_ERROR(hr);

        nGameWorldObjectType = (int)lua_tonumber(L, 1);
        dwObject = (DWORD)lua_tonumber(L, 2);
        fScale = (float)lua_tonumber(L, 3);
	}
	else
	{
        dwScene = (DWORD)lua_tonumber(L, 1);
        nGameWorldObjectType = (int)lua_tonumber(L, 2);
        dwObject = (DWORD)lua_tonumber(L, 3);
        fScale = (float)lua_tonumber(L, 4);
	}

	dwSceneObjectType = g_pSO3UI->m_pRepresent->GameWorldObjectTypeToSceneObjectType(nGameWorldObjectType);

	hr = g_pSO3UI->m_pRepresent->SetSceneObjectAlpha(dwScene, dwSceneObjectType, dwObject, fScale);
	KGLOG_COM_PROCESS_ERROR(hr);

	nResult = true;
Exit0:
	return 0;
}


int KRepresentScriptTable::LuaSceneObject_SetBrightness(Lua_State* L)
{
	int nRetCode = false;
    int nResult = false;
	HRESULT hr = E_FAIL;
    DWORD dwScene = 0;
	DWORD dwObject = 0;
	DWORD dwSceneObjectType = UINT_MAX;
    int nParamCount = 0;
	int nGameWorldObjectType = 0;
	float fScale = 0.0f;

	ASSERT(L);

    nParamCount = lua_gettop(L);

	KGLOG_PROCESS_ERROR(nParamCount == 3 || nParamCount == 4);
	
    if (nParamCount == 3)
	{
		hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwScene);
		KGLOG_COM_PROCESS_ERROR(hr);

        nGameWorldObjectType = (int)lua_tonumber(L, 1);
        dwObject = (DWORD)lua_tonumber(L, 2);
        fScale = (float)lua_tonumber(L, 3);
	}
	else
	{
        dwScene = (DWORD)lua_tonumber(L, 1);
        nGameWorldObjectType = (int)lua_tonumber(L, 2);
        dwObject = (DWORD)lua_tonumber(L, 3);
        fScale = (float)lua_tonumber(L, 4);
	}

	dwSceneObjectType = g_pSO3UI->m_pRepresent->GameWorldObjectTypeToSceneObjectType(nGameWorldObjectType);

	hr = g_pSO3UI->m_pRepresent->SetSceneObjectBrightness(dwScene, dwSceneObjectType, dwObject, fScale);
	KGLOG_COM_PROCESS_ERROR(hr);

	nResult = true;
Exit0:
	return 0;
}

int KRepresentScriptTable::LuaSceneObject_SetSelectable(Lua_State* L)
{
	int nRetCode = false;
    int nResult = false;
	HRESULT hr = E_FAIL;
    DWORD dwScene = 0;
	DWORD dwObject = 0;
	DWORD dwSceneObjectType = UINT_MAX;
    int nParamCount = 0;
	int nGameWorldObjectType = 0;
	int nSelectable = false;

	ASSERT(L);

    nParamCount = lua_gettop(L);
	KGLOG_PROCESS_ERROR(nParamCount == 3 || nParamCount == 4);
	
    if (nParamCount == 3)
	{
		hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwScene);
		KGLOG_COM_PROCESS_ERROR(hr);

        nGameWorldObjectType = (int)lua_tonumber(L, 1);
        dwObject = (DWORD)lua_tonumber(L, 2);
        nSelectable = (int)lua_tonumber(L, 3);
	}
	else
	{
        dwScene = (DWORD)lua_tonumber(L, 1);
        nGameWorldObjectType = (int)lua_tonumber(L, 2);
        dwObject = (DWORD)lua_tonumber(L, 3);
        nSelectable = (int)lua_tonumber(L, 4);
	}

	dwSceneObjectType = g_pSO3UI->m_pRepresent->GameWorldObjectTypeToSceneObjectType(nGameWorldObjectType);

	hr = g_pSO3UI->m_pRepresent->SetSceneObjectSelectable(dwScene, dwSceneObjectType, dwObject, nSelectable);
	KGLOG_COM_PROCESS_ERROR(hr);

	nResult = true;
Exit0:
	return 0;
}


int KRepresentScriptTable::LuaSelection_ShowSFX(Lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	DWORD dwScene = UINT_MAX;
	DWORD dwSkillID = UINT_MAX;
	DWORD dwSkillLevel = UINT_MAX;
	int nParamCount = 0;

	ASSERT(L);

	nParamCount = Lua_GetTopIndex(L);

	bRetCode = nParamCount == 2 || nParamCount == 3;
	KGLOG_PROCESS_ERROR(bRetCode);

	if (nParamCount == 2)
	{
		hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwScene);
		KGLOG_COM_PROCESS_ERROR(hr);

		dwSkillID = static_cast<DWORD>(Lua_ValueToNumber(L, 1));
		dwSkillLevel = static_cast<DWORD>(Lua_ValueToNumber(L, 2));
	}
	else
	{
		dwScene = static_cast<DWORD>(Lua_ValueToNumber(L, 1));
		dwSkillID = static_cast<DWORD>(Lua_ValueToNumber(L, 2));
		dwSkillLevel = static_cast<DWORD>(Lua_ValueToNumber(L, 3));
	}

	hr = g_pSO3UI->m_pRepresent->ShowSelectionSFX(dwScene, dwSkillID, dwSkillLevel);
	KGLOG_COM_PROCESS_ERROR(hr);

    return 0;
Exit0:
	return 0;
}

int KRepresentScriptTable::LuaSelection_HideSFX(Lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	DWORD dwScene = UINT_MAX;
	int nParamCount = 0;

	ASSERT(L);

	nParamCount = Lua_GetTopIndex(L);

	bRetCode = nParamCount == 0 || nParamCount == 1;
	KGLOG_PROCESS_ERROR(bRetCode);

	if (nParamCount == 0)
	{
		hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwScene);
		KGLOG_COM_PROCESS_ERROR(hr);
	}
	else
	{
		dwScene = static_cast<DWORD>(Lua_ValueToNumber(L, 1));
	}

	hr = g_pSO3UI->m_pRepresent->HideSelectionSFX(dwScene);
	KGLOG_COM_PROCESS_ERROR(hr);

	return 0;
Exit0:
	return 0;
}


int KRepresentScriptTable::LuaTargetSelection_AttachSceneObject(Lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	DWORD dwScene = UINT_MAX;
	DWORD dwObject = UINT_MAX;
	DWORD dwObjectType = UINT_MAX;
	int nTargetType = ttInvalid;
	int nParamCount = 0;

	ASSERT(L);

	nParamCount = Lua_GetTopIndex(L);

	bRetCode = nParamCount == 2 || nParamCount == 3;
	KGLOG_PROCESS_ERROR(bRetCode);

	if (nParamCount == 2)
	{
		hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwScene);
		KGLOG_COM_PROCESS_ERROR(hr);

		dwObject = static_cast<DWORD>(Lua_ValueToNumber(L, 1));
		nTargetType = static_cast<int>(Lua_ValueToNumber(L, 2));
	}
	else
	{
		dwScene = static_cast<DWORD>(Lua_ValueToNumber(L, 1));
		dwObject = static_cast<DWORD>(Lua_ValueToNumber(L, 2));
		nTargetType = static_cast<int>(Lua_ValueToNumber(L, 3));
	}

	dwObjectType = g_pSO3UI->m_pRepresent->GameWorldObjectTypeToSceneObjectType(nTargetType);

	hr = g_pSO3UI->m_pRepresent->AttachTargetSceneObject(dwScene, dwObject, dwObjectType);
	KGLOG_COM_PROCESS_ERROR(hr);

	return 0;
Exit0:
	return 0;
}


int KRepresentScriptTable::LuaTargetSelection_DetachSceneObject(Lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	DWORD dwScene = UINT_MAX;
	DWORD dwObject = UINT_MAX;
	DWORD dwObjectType = UINT_MAX;
	int nTargetType = ttInvalid;
	int nParamCount = 0;

	ASSERT(L);

	nParamCount = Lua_GetTopIndex(L);

	bRetCode = nParamCount == 2 || nParamCount == 3;
	KGLOG_PROCESS_ERROR(bRetCode);

	if (nParamCount == 2)
	{
		hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwScene);
		KGLOG_COM_PROCESS_ERROR(hr);

		dwObject = static_cast<DWORD>(Lua_ValueToNumber(L, 1));
		nTargetType = static_cast<int>(Lua_ValueToNumber(L, 2));
	}
	else
	{
		dwScene = static_cast<DWORD>(Lua_ValueToNumber(L, 1));
		dwObject = static_cast<DWORD>(Lua_ValueToNumber(L, 2));
		nTargetType = static_cast<int>(Lua_ValueToNumber(L, 3));
	}

	dwObjectType = g_pSO3UI->m_pRepresent->GameWorldObjectTypeToSceneObjectType(nTargetType);

	hr = g_pSO3UI->m_pRepresent->DetachTargetSceneObject(dwScene, dwObject, dwObjectType);
	KGLOG_COM_PROCESS_ERROR(hr);

	return 0;
Exit0:
	return 0;
}


int KRepresentScriptTable::LuaTargetSelection_ShowSFX(Lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	DWORD dwScene = UINT_MAX;
	int nForceRelationType = 0;
	int nParamCount = 0;

	ASSERT(L);

	nParamCount = Lua_GetTopIndex(L);

	bRetCode = nParamCount == 1 || nParamCount == 2;
	KGLOG_PROCESS_ERROR(bRetCode);

	if (nParamCount == 1)
	{
		hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwScene);
		KGLOG_COM_PROCESS_ERROR(hr);

		nForceRelationType = static_cast<int>(Lua_ValueToNumber(L, 1));
	}
	else
	{
		dwScene = static_cast<DWORD>(Lua_ValueToNumber(L, 1));
		nForceRelationType = static_cast<int>(Lua_ValueToNumber(L, 2));
	}

	hr = g_pSO3UI->m_pRepresent->ShowTargetSelectionSFX(dwScene, nForceRelationType);
	KGLOG_COM_PROCESS_ERROR(hr);

	return 0;
Exit0:
	return 0;
}


int KRepresentScriptTable::LuaTargetSelection_HideSFX(Lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	DWORD dwScene = UINT_MAX;
	int nParamCount = 0;

	ASSERT(L);

	nParamCount = Lua_GetTopIndex(L);

	bRetCode = nParamCount == 0 || nParamCount == 1;
	KGLOG_PROCESS_ERROR(bRetCode);

	if (nParamCount == 0)
	{
		hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwScene);
		KGLOG_COM_PROCESS_ERROR(hr);
	}
	else
	{
		dwScene = static_cast<DWORD>(Lua_ValueToNumber(L, 1));
	}

	hr = g_pSO3UI->m_pRepresent->HideTargetSelectionSFX(dwScene);
	KGLOG_COM_PROCESS_ERROR(hr);

	return 0;
Exit0:
	return 0;
}

int KRepresentScriptTable::LuaPlayer_GetAnimationResource(Lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
    ROLE_TYPE nRoleType = rtInvalid;
	DWORD dwAnimationID = 0;
	KRepresentPlayerAnimationResource Resource;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 2;
	KGLOG_PROCESS_ERROR(bRetCode);

	nRoleType = (ROLE_TYPE)(int)lua_tonumber(L, 1);
	dwAnimationID = (DWORD)lua_tonumber(L, 2);

	hr = g_pSO3UI->m_pRepresent->GetPlayerAnimationResource(nRoleType, dwAnimationID, &Resource);
	KG_COM_PROCESS_ERROR(hr);

	lua_pushstring(L, (Resource.szAniFilePath && Resource.szAniFilePath[0] != '\0') ? Resource.szAniFilePath : NULL);
	lua_pushstring(L, (Resource.szAniSoundFilePath && Resource.szAniSoundFilePath[0] != '\0') ? Resource.szAniSoundFilePath : NULL);
	lua_pushnumber(L, Resource.dwAniPlayType);
	lua_pushnumber(L, Resource.fAniPlaySpeed);

	return 4;
Exit0:
	return 0;
}


int KRepresentScriptTable::LuaPlayer_GetEquipResource(Lua_State* L)
{
	static LPCSTR s_szParts[MODEL_PLAYER_EQUIP_AND_ALL_SOCKET_COUNT] =
	{
		"CHEST", "HEADFORM", "WAIST", "PANTS", "BANGLE", "BANG", "PLAIT", "RL_WEAPON_LH", "RL_WEAPON_RH", "BACK_EXTEND", "WAIST_EXTEND", "FACE", "HAT", "SHEATH_LH", "SHEATH_RH",
	};

	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
    ROLE_TYPE nRoleType = rtInvalid;
	int nParamCount = 0;
	int nParamIndex = 1;
    int i = 0;
    int j = 0;
	DWORD dwRepresentIDs[perRepresentCount];
	KRepresentPlayerEquipResource Resource;

	ASSERT(L);

	nParamCount = lua_gettop(L);

	bRetCode = nParamCount == 1 + perRepresentCount;
	KGLOG_PROCESS_ERROR(bRetCode);

	nRoleType = (ROLE_TYPE)(int)lua_tonumber(L, nParamIndex++);

	for (size_t nIndex = 0; nIndex < perRepresentCount; ++nIndex)
	{
		dwRepresentIDs[nIndex] = (DWORD)lua_tonumber(L, nParamIndex++);
	}
    
	hr = g_pSO3UI->m_pRepresent->GetPlayerEquipResource(nRoleType, dwRepresentIDs, &Resource);
	KGLOG_COM_PROCESS_ERROR(hr);

	lua_newtable(L);

	lua_pushstring(L, "MDL");
	lua_pushstring(L, (Resource.szMDLFilePath && Resource.szMDLFilePath[0] != '\0') ? Resource.szMDLFilePath : NULL);
	lua_settable(L, -3);

	for (i = 0; i < MODEL_PLAYER_EQUIP_AND_SOCKET_COUNT; ++i)
	{
		KRepresentPlayerEquipPartResource const& part = Resource.Parts[i];

		lua_pushstring(L, s_szParts[i]);
		lua_newtable(L);

		lua_pushstring(L, "Socket");
		lua_pushstring(L, (part.szSocketName && part.szSocketName[0] != '\0') ? part.szSocketName : NULL);
		lua_settable(L, -3);

		lua_pushstring(L, "Mesh");
		lua_pushstring(L, (part.szMeshFilePath && part.szMeshFilePath[0] != '\0') ? part.szMeshFilePath : NULL);
		lua_settable(L, -3);

		lua_pushstring(L, "Mtl");
		lua_pushstring(L, (part.szMtlFilePath && part.szMtlFilePath[0] != '\0') ? part.szMtlFilePath : NULL);
		lua_settable(L, -3);

		lua_pushstring(L, "MeshScale");
		lua_pushnumber(L, part.fMeshScale);
		lua_settable(L, -3);

		lua_pushstring(L, "ColorChannel");
		lua_pushnumber(L, part.nColorChannel);
		lua_settable(L, -3);

		lua_pushstring(L, "SFX1");
		lua_pushstring(L, (part.szSFX1FilePath && part.szSFX1FilePath[0] != '\0') ? part.szSFX1FilePath : NULL);
		lua_settable(L, -3);

        lua_pushstring(L, "SFX2");
        lua_pushstring(L, (part.szSFX2FilePath && part.szSFX2FilePath[0] != '\0') ? part.szSFX2FilePath : NULL);
        lua_settable(L, -3);

		lua_settable(L, -3);
    }

    // 刀鞘部分
    for (j = MODEL_PLAYER_SOCKET_WEAPON_BEGIN; j != MODEL_PLAYER_SOCKET_WEAPON_END; ++j)
    {
        KRepresentPlayerEquipPartResource const& part = Resource.Parts[j];

        lua_pushstring(L, s_szParts[i++]);
        lua_newtable(L);

        lua_pushstring(L, "Socket");
        lua_pushstring(L, (part.szSheathSocketName && part.szSheathSocketName[0] != '\0') ? part.szSheathSocketName : NULL);
        lua_settable(L, -3);

        lua_pushstring(L, "Mesh");
        lua_pushstring(L, (part.szSheathMeshFilePath && part.szSheathMeshFilePath[0] != '\0') ? part.szSheathMeshFilePath : NULL);
        lua_settable(L, -3);

        lua_pushstring(L, "Mtl");
        lua_pushstring(L, (part.szSheathMtlFilePath && part.szSheathMtlFilePath[0] != '\0') ? part.szSheathMtlFilePath : NULL);
        lua_settable(L, -3);

        lua_pushstring(L, "MeshScale");
        lua_pushnumber(L, part.fMeshScale);
        lua_settable(L, -3);

		lua_pushstring(L, "ColorChannel");
		lua_pushnumber(L, part.nColorChannel);
		lua_settable(L, -3);

        lua_pushstring(L, "SFX1");
        lua_pushstring(L, NULL);
        lua_settable(L, -3);

        lua_pushstring(L, "SFX2");
        lua_pushstring(L, NULL);
        lua_settable(L, -3);

        lua_settable(L, -3);
    }

	return 1;
Exit0:
	return 0;
}

int KRepresentScriptTable::LuaPlayer_GetRidesAnimationResource(Lua_State* L)
{
    BOOL bRetCode = FALSE;
    HRESULT hr = E_FAIL;
    DWORD dwRidesID = 0;
    DWORD dwAnimationID = 0;
    KRepresentRidesAnimationResource Resource;

    ASSERT(L);

    bRetCode = lua_gettop(L) == 2;
    KGLOG_PROCESS_ERROR(bRetCode);

    dwRidesID = (DWORD)lua_tonumber(L, 1);
    dwAnimationID = (DWORD)lua_tonumber(L, 2);

    hr = g_pSO3UI->m_pRepresent->GetRidesAnimationResource(dwRidesID, dwAnimationID, &Resource);
    KG_COM_PROCESS_ERROR(hr);

    lua_pushstring(L, Resource.szAniFilePath ? (Resource.szAniFilePath[0] != '\0' ? Resource.szAniFilePath : NULL) : NULL);
    lua_pushstring(L, Resource.szAniSoundFilePath ? (Resource.szAniSoundFilePath[0] != '\0' ? Resource.szAniSoundFilePath : NULL) : NULL);
    lua_pushnumber(L, Resource.dwAniPlayType);
    lua_pushnumber(L, Resource.fAniPlaySpeed);
    lua_pushnumber(L, Resource.fAniSoundRange);

    lua_pushstring(L, Resource.szSFXFilePath ? (Resource.szSFXFilePath[0] != '\0' ? Resource.szSFXFilePath : NULL) : NULL);
    lua_pushstring(L, Resource.szSFXBoneName ? (Resource.szSFXBoneName[0] != '\0' ? Resource.szSFXBoneName : NULL) : NULL);
    lua_pushnumber(L, Resource.dwSFXPlayType);
    lua_pushnumber(L, Resource.fSFXPlaySpeed);
    lua_pushnumber(L, Resource.fSFXScale);

    return 10;
Exit0:
    return 0;
}

int KRepresentScriptTable::LuaPlayer_GetRidesEquipResource(Lua_State* L)
{
    static LPCSTR s_szParts[MODEL_RIDES_EQUIP_AND_SOCKET_COUNT] =
    {
        "Body", "Mane", "Head", "Chest", "Leg", "Pendant",
    };

    BOOL bRetCode = FALSE;
    HRESULT hr = E_FAIL;
    int nParamCount = 0;
    int nParamIndex = 1;
    int i = 0;
    int j = 0;
    DWORD dwRidesID = 0;
    DWORD dwRepresentIDs[MODEL_RIDES_EQUIP_AND_SOCKET_COUNT];
    KRepresentRidesEquipResource Resource;

    ASSERT(L);

    nParamCount = lua_gettop(L);

    bRetCode = nParamCount == MODEL_RIDES_EQUIP_AND_SOCKET_COUNT + 1;
    KGLOG_PROCESS_ERROR(bRetCode);

    dwRidesID = static_cast<DWORD>(lua_tonumber(L, nParamIndex++));

    for (size_t nIndex = 0; nIndex < MODEL_RIDES_EQUIP_AND_SOCKET_COUNT; ++nIndex)
    {
        dwRepresentIDs[nIndex] = static_cast<DWORD>(lua_tonumber(L, nParamIndex++));
    }

    hr = g_pSO3UI->m_pRepresent->GetRidesEquipResource(dwRidesID, dwRepresentIDs, &Resource);
    KGLOG_COM_PROCESS_ERROR(hr);

    lua_newtable(L);

    lua_pushstring(L, "MDL");
    lua_pushstring(L, Resource.szMDLFilePath ? (Resource.szMDLFilePath[0] != '\0' ? Resource.szMDLFilePath : NULL) : NULL);
    lua_settable(L, -3);

    for (i = 0; i < MODEL_RIDES_EQUIP_AND_SOCKET_COUNT; ++i)
    {
        KRepresentRidesEquipPartResource const& part = Resource.Parts[i];

        lua_pushstring(L, s_szParts[i]);
        lua_newtable(L);

        lua_pushstring(L, "Socket");
        lua_pushstring(L, part.szSocketName ? (part.szSocketName[0] != '\0' ? part.szSocketName : NULL) : NULL);
        lua_settable(L, -3);

        lua_pushstring(L, "Mesh");
        lua_pushstring(L, part.szMeshFilePath ? (part.szMeshFilePath[0] != '\0' ? part.szMeshFilePath : NULL) : NULL);
        lua_settable(L, -3);

        lua_pushstring(L, "Mtl");
        lua_pushstring(L, part.szMtlFilePath ? (part.szMtlFilePath[0] != '\0' ? part.szMtlFilePath : NULL) : NULL);
        lua_settable(L, -3);

        lua_pushstring(L, "MeshScale");
        lua_pushnumber(L, part.fMeshScale);
        lua_settable(L, -3);

        lua_pushstring(L, "ColorChannelTable");
        lua_pushnumber(L, part.nGlobalColorChannel);
        lua_settable(L, -3);

        lua_pushstring(L, "ColorChannel");
        lua_pushnumber(L, part.nColorChannel);
        lua_settable(L, -3);

        lua_settable(L, -3);
    }

    return 1;
Exit0:
    return 0;
}

int KRepresentScriptTable::LuaNPC_GetAnimationResource(Lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	DWORD dwRepresentID = 0;
	DWORD dwAnimationID = 0;
	KRepresentNPCAnimationResource Resource;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 2;
	KGLOG_PROCESS_ERROR(bRetCode);

	dwRepresentID = static_cast<DWORD>(lua_tonumber(L, 1));
	dwAnimationID = static_cast<DWORD>(lua_tonumber(L, 2));

	hr = g_pSO3UI->m_pRepresent->GetNPCAnimationResource(dwRepresentID, dwAnimationID, &Resource);
	KGLOG_COM_PROCESS_ERROR(hr);

	lua_pushstring(L, Resource.szAniFilePath ? (Resource.szAniFilePath[0] != '\0' ? Resource.szAniFilePath : NULL) : NULL);
	lua_pushstring(L, Resource.szAniSoundFilePath ? (Resource.szAniSoundFilePath[0] != '\0' ? Resource.szAniSoundFilePath : NULL) : NULL);
	lua_pushnumber(L, Resource.dwAniPlayType);
	lua_pushnumber(L, Resource.fAniPlaySpeed);
	lua_pushnumber(L, Resource.fAniSoundRange);

	lua_pushstring(L, Resource.szSFXFilePath ? (Resource.szSFXFilePath[0] != '\0' ? Resource.szSFXFilePath : NULL) : NULL);
	lua_pushstring(L, Resource.szSFXBoneName ? (Resource.szSFXBoneName[0] != '\0' ? Resource.szSFXBoneName : NULL) : NULL);
	lua_pushnumber(L, Resource.dwSFXPlayType);
	lua_pushnumber(L, Resource.fSFXPlaySpeed);
	lua_pushnumber(L, Resource.fSFXScale);

	return 10;
Exit0:
	return 0;
}


int KRepresentScriptTable::LuaNPC_GetEquipResource(Lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	DWORD dwRepresentID = 0;
	KRepresentNPCEquipResource Resource;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 1;
	KGLOG_PROCESS_ERROR(bRetCode);

	dwRepresentID = static_cast<DWORD>(lua_tonumber(L, 1));

	hr = g_pSO3UI->m_pRepresent->GetNPCEquipResource(dwRepresentID, &Resource);
	KGLOG_COM_PROCESS_ERROR(hr);

	lua_pushstring(L, Resource.szMDLFilePath ? (Resource.szMDLFilePath[0] != '\0' ? Resource.szMDLFilePath : NULL) : NULL);
	lua_pushnumber(L, Resource.fModelScale);
	lua_pushnumber(L, Resource.fSocketScale);

	for (size_t nIndex = 0; nIndex < RL_NPC_SOCKET_COUNT; ++nIndex)
	{
		KRepresentNPCEquipPartResource const& part = Resource.Parts[nIndex];

		lua_pushstring(L, part.szMeshFilePath ? (part.szMeshFilePath[0] != '\0' ? part.szMeshFilePath : NULL) : NULL);
		lua_pushstring(L, part.szMtlFilePath ? (part.szMtlFilePath[0] != '\0' ? part.szMtlFilePath : NULL) : NULL);
	}

	return 3 + RL_NPC_SOCKET_COUNT * 2;
Exit0:
	return 0;
}


int KRepresentScriptTable::LuaGlobal_SetTopHeadFlag(Lua_State* L)
{
    // TODO: rename

    BOOL bRetCode = FALSE;
    int nCharacterType = -1;
    int nIndex = -1;
    BOOL bShow = FALSE;

    ASSERT(L);

    bRetCode = Lua_GetTopIndex(L) == 3;
    KGLOG_PROCESS_ERROR(bRetCode);

    nCharacterType = static_cast<int>(Lua_ValueToNumber(L, 1));
    nIndex = static_cast<int>(Lua_ValueToNumber(L, 2));
	if (Lua_IsBoolean(L, 3))
		bShow = Lua_ValueToBoolean(L, 3);
	else
		bShow = (BOOL)(int)Lua_ValueToNumber(L, 3);

    g_pSO3UI->m_pRepresent->EnableHeadSFX(nCharacterType, nIndex, bShow);

    return 0;
Exit0:
    return 0;
}

int KRepresentScriptTable::LuaGlobal_UpdateHeadTopPosition(Lua_State* L)
{
    ASSERT(L);

    g_pSO3UI->m_pRepresent->UpdateHeadTopPosition();

    return 0;
}

int KRepresentScriptTable::LuaSence_GetMinimapLayer(Lua_State* L)
{
	int nLayer = 0;

	D3DVECTOR vPos;

	ASSERT(L);

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 4);

	DWORD dwSenceID = (DWORD)Lua_ValueToNumber(L, 1);
	vPos.x = (float)Lua_ValueToNumber(L, 2);
	vPos.y = (float)Lua_ValueToNumber(L, 3);
	vPos.z = (float)Lua_ValueToNumber(L, 4);

	nLayer = g_pSO3UI->m_pRepresent->GetMinimapLayer(dwSenceID, vPos);

Exit0:
	Lua_PushNumber(L, nLayer);
	return 1;
}

int KRepresentScriptTable::LuaSence_GetRegionInfo(Lua_State* L)
{
	D3DVECTOR vPos;
	DWORD dwData = 0;
	BOOL bMapID = FALSE;

	ASSERT(L);
	int nParam = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nParam == 4 || nParam == 5);
	if (nParam == 5)
	{
		if (lua_isboolean(L, 5))
			bMapID = (BOOL)Lua_ValueToBoolean(L, 5);
		else
			bMapID = (BOOL)Lua_ValueToNumber(L, 5);
	}

	DWORD dwSenceID = (DWORD)Lua_ValueToNumber(L, 1);
	vPos.x = (float)Lua_ValueToNumber(L, 2);
	vPos.y = (float)Lua_ValueToNumber(L, 3);
	vPos.z = (float)Lua_ValueToNumber(L, 4);

	if (bMapID)
		dwData = g_pSO3UI->m_pRepresent->GetRegionInfoByMapID(dwSenceID, vPos);
	else
		dwData = g_pSO3UI->m_pRepresent->GetRegionInfo(dwSenceID, vPos);

Exit0:
	Lua_PushNumber(L, dwData);
	return 1;
}

int KRepresentScriptTable::LuaSceneObject_SetTitleEffect(Lua_State* L)
{
    HRESULT hrRetCode = E_FAIL;
    DWORD dwScene = 0;
    DWORD dwObject = 0;
    DWORD dwGameWorldObjectType = 0;
    DWORD dwSceneObjectType = 0;
    DWORD dwEffectID = 0;
    int nParamCount = 0;
    int nParamIndex = 1;

    ASSERT(L);

    nParamCount = lua_gettop(L);
    KGLOG_PROCESS_ERROR(nParamCount == 3 || nParamCount == 4);

    if (nParamCount == 4)
    {
        dwScene = (DWORD)lua_tonumber(L, nParamIndex++);
    }
    else
    {
        hrRetCode = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwScene);
        KGLOG_COM_PROCESS_ERROR(hrRetCode);
    }

    dwGameWorldObjectType = (DWORD)lua_tonumber(L, nParamIndex++);
    dwObject = (DWORD)lua_tonumber(L, nParamIndex++);
    dwEffectID = (DWORD)lua_tonumber(L, nParamIndex++);

    dwSceneObjectType = g_pSO3UI->m_pRepresent->GameWorldObjectTypeToSceneObjectType(dwGameWorldObjectType);

    hrRetCode = g_pSO3UI->m_pRepresent->SetSceneObjectTitleEffect(dwScene, dwSceneObjectType, dwObject, dwEffectID);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

Exit0:
    return 0;
}

int KRepresentScriptTable::LuaRepresentCommand(Lua_State* L)
{
    HRESULT hr = E_FAIL;
    LPCSTR szCommand = NULL;

    KGLOG_PROCESS_ERROR(lua_gettop(L) == 1);

    szCommand = lua_tostring(L, 1);
    KGLOG_PROCESS_ERROR(szCommand);

    hr = g_pSO3UI->m_pRepresent->ExecuteCommand(szCommand, NULL);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
    return 0;
}

int KRepresentScriptTable::LuaCharacter_PlayAnimation(Lua_State* L)
{
    int nRetCode = false;
    HRESULT hr = E_FAIL;
    int nParamCount = 0;
    int nParamIndex = 1;
    DWORD dwSceneID = 0;
	DWORD dwActorID = 0;
	DWORD dwTargetID = 0;
	DWORD dwAnimationID = 0;
	BOOL bTurnToTarget = TRUE;

    nParamCount = lua_gettop(L);
    KGLOG_PROCESS_ERROR(nParamCount == 4 || nParamCount == 5);

    if (nParamCount == 5)
    {
        dwSceneID = (DWORD)lua_tonumber(L, nParamIndex++);
    }
    else
    {
        hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwSceneID);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

	dwActorID = (DWORD)lua_tonumber(L, nParamIndex++);
	dwTargetID = (DWORD)lua_tonumber(L, nParamIndex++);
	dwAnimationID = (DWORD)lua_tonumber(L, nParamIndex++);
	if (lua_isboolean(L, nParamIndex++))
		bTurnToTarget = (BOOL)lua_toboolean(L, nParamIndex);
	else
		bTurnToTarget = (BOOL)(int)lua_toboolean(L, nParamIndex);

    hr = g_pSO3UI->m_pRepresent->PlayAnimation(dwSceneID, dwActorID, dwTargetID, bTurnToTarget, dwAnimationID);
    KG_COM_PROCESS_ERROR(hr);

Exit0:
	return 0;
}

int KRepresentScriptTable::LuaCharacter_StopAnimation(Lua_State* L)
{
    int nRetCode = false;
    HRESULT hr = E_FAIL;
    int nParamCount = 0;
    int nParamIndex = 1;
    DWORD dwSceneID = 0;
	DWORD dwActorID = 0;

    nParamCount = lua_gettop(L);
    KGLOG_PROCESS_ERROR(nParamCount == 1 || nParamCount == 2);

    if (nParamCount == 2)
    {
        dwSceneID = (DWORD)lua_tonumber(L, nParamIndex++);
    }
    else
    {
        hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwSceneID);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

	dwActorID = (DWORD)lua_tonumber(L, nParamIndex++);

    hr = g_pSO3UI->m_pRepresent->StopAnimation(dwSceneID, dwActorID);
    KG_COM_PROCESS_ERROR(hr);

Exit0:
	return 0;
}

int KRepresentScriptTable::LuaCharacter_PlaySound(Lua_State* L)
{
    int nRetCode = false;
    HRESULT hr = E_FAIL;
    int nParamCount = 0;
    int nParamIndex = 1;
    DWORD dwSceneID = 0;
	DWORD dwActorID = 0;
	DWORD dwTargetID = 0;
	LPCSTR pcszSpeechFile = NULL;
	BOOL bTurnToTarget = TRUE;

    nParamCount = lua_gettop(L);
    KGLOG_PROCESS_ERROR(nParamCount == 4 || nParamCount == 5);

    if (nParamCount == 5)
    {
        dwSceneID = (DWORD)lua_tonumber(L, nParamIndex++);
    }
    else
    {
        hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwSceneID);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

	dwActorID = (DWORD)lua_tonumber(L, nParamIndex++);
	dwTargetID = (DWORD)lua_tonumber(L, nParamIndex++);

	pcszSpeechFile = lua_tostring(L, nParamIndex++);
	KGLOG_PROCESS_ERROR(pcszSpeechFile);

	if (lua_isboolean(L, nParamIndex++))
		bTurnToTarget = (BOOL)lua_toboolean(L, nParamIndex);
	else
		bTurnToTarget = (BOOL)(int)lua_toboolean(L, nParamIndex);

    hr = g_pSO3UI->m_pRepresent->PlaySpeech(dwSceneID, dwActorID, dwTargetID, bTurnToTarget, pcszSpeechFile);
    KG_COM_PROCESS_ERROR(hr);

Exit0:
	return 0;
}

int KRepresentScriptTable::LuaCharacter_StopSound(Lua_State* L)
{
    int nRetCode = false;
    HRESULT hr = E_FAIL;
    int nParamCount = 0;
    int nParamIndex = 1;
    DWORD dwSceneID = 0;
	DWORD dwActorID = 0;

    nParamCount = lua_gettop(L);
    KGLOG_PROCESS_ERROR(nParamCount == 1 || nParamCount == 2);

    if (nParamCount == 2)
    {
        dwSceneID = (DWORD)lua_tonumber(L, nParamIndex++);
    }
    else
    {
        hr = g_pSO3UI->m_pRepresent->GetCurrentSceneID(dwSceneID);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

	dwActorID = (DWORD)lua_tonumber(L, nParamIndex++);

    hr = g_pSO3UI->m_pRepresent->StopSpeech(dwSceneID, dwActorID);
    KG_COM_PROCESS_ERROR(hr);

Exit0:
	return 0;
}

