#ifndef UI_SCRIPT_K_REPRESENT_SCRIPT_TABLE_H
#define UI_SCRIPT_K_REPRESENT_SCRIPT_TABLE_H

#include "SO3Represent/SO3RepresentInterface.h"

class KRepresentScriptTable
{
public:
	static void Load();

private:
	static int LuaResetActiveObject(Lua_State* L);

	static int LuaCamera_Zoom(Lua_State* L);
	static int LuaCamera_BeginDrag(Lua_State* L);
	static int LuaCamera_EndDrag(Lua_State* L);
	static int LuaCamera_HasDragged(Lua_State* L);
    static int LuaCamera_SetParams(Lua_State* L);
    static int LuaCamera_GetParams(Lua_State* L);
    static int LuaCamera_SetRTParams(Lua_State* L);
    static int LuaCamera_GetRTParams(Lua_State* L);
    static int LuaCamera_EnableControl(Lua_State* L);
    static int LuaCamera_ToggleControl(Lua_State* L);
    static int LuaCamera_SetView(Lua_State* L);
    static int LuaCamera_TurnToFaceDirection(Lua_State* L);
    static int LuaCamera_GetYawPitchRoll(Lua_State* L);
    static int LuaCamera_SwitchMode(Lua_State* L);
    static int LuaCamera_IsCharacterMoving(Lua_State* L);
    static int LuaCamera_MouseControlMoveEnable(Lua_State* L);
    static int LuaCamera_LockControl(Lua_State* L);
    static int LuaGetCameraInitParams(Lua_State* L);

    static int LuaCamera_StartFollow(Lua_State* L);
    static int LuaCamera_StopFollow(Lua_State* L);
    static int LuaCamera_IsFollowing(Lua_State* L);

    static int LuaCamera_AutoMoveToPoint(Lua_State* L);
    static int LuaCamera_AutoMoveToTarget(Lua_State* L);
    static int LuaCamera_IsAutoMoving(Lua_State* L);

    static int LuaCursor_ShowCursorEffect(Lua_State* L);

	static int LuaScene_New(Lua_State* L);
	static int LuaScene_Load(Lua_State* L);
	static int LuaScene_Delete(Lua_State* L);

	static int LuaScene_AddOutputWindow(Lua_State* L);
	static int LuaScene_RemoveOutputWindow(Lua_State* L);

	static int LuaScene_SelectObject(Lua_State* L);
	static int LuaScene_SelectGround(Lua_State* L);
    static int LuaScene_GetCharacterTop(Lua_State* L);

    static int LuaScene_ScenePointToScreenPoint(Lua_State* L);
	static int LuaScene_GameWorldPositionToScenePosition(Lua_State* L);
	static int LuaScene_ScenePositionToGameWorldPosition(Lua_State* L);

	static int LuaSceneObject_SetAlpha(Lua_State* L);
	static int LuaSceneObject_SetBrightness(Lua_State* L);

	static int LuaSceneObject_SetSelectable(Lua_State* L);

	static int LuaSelection_ShowSFX(Lua_State* L);
	static int LuaSelection_HideSFX(Lua_State* L);

	static int LuaTargetSelection_AttachSceneObject(Lua_State* L);
	static int LuaTargetSelection_DetachSceneObject(Lua_State* L);
	static int LuaTargetSelection_ShowSFX(Lua_State* L);
	static int LuaTargetSelection_HideSFX(Lua_State* L);

	static int LuaPlayer_GetAnimationResource(Lua_State* L);
	static int LuaPlayer_GetEquipResource(Lua_State* L);

    static int LuaPlayer_GetRidesAnimationResource(Lua_State* L);
    static int LuaPlayer_GetRidesEquipResource(Lua_State* L);

	static int LuaNPC_GetAnimationResource(Lua_State* L);
	static int LuaNPC_GetEquipResource(Lua_State* L);

    static int LuaGlobal_SetTopHeadFlag(Lua_State* L);
    static int LuaGlobal_UpdateHeadTopPosition(Lua_State* L);

	static int LuaSence_GetMinimapLayer(Lua_State* L);
	static int LuaSence_GetRegionInfo(Lua_State* L);

	static int LuaSceneObject_SetTitleEffect(Lua_State* L);

    static int LuaRepresentCommand(Lua_State* L);

	static int LuaCharacter_PlayAnimation(Lua_State* L);
	static int LuaCharacter_StopAnimation(Lua_State* L);

	static int LuaCharacter_PlaySound(Lua_State* L);
	static int LuaCharacter_StopSound(Lua_State* L);
};

#endif // UI_SCRIPT_K_REPRESENT_SCRIPT_TABLE_H

