#ifndef UI_SCRIPT_K_3D_ENGINE_SCRIPT_TABLE_H
#define UI_SCRIPT_K_3D_ENGINE_SCRIPT_TABLE_H


class IKG3DModel;
class IKG3DCamera;
class IKG3DLight;
class IKG3DScene;
class IKG3DModelSet;
class IKG3DAnimationController;
class IKG3DSFXSuede;
class IKG3DResourceManager;
class IKG3DSceneCameraMovement;
class IKG3DSceneCameraAnimation;
class IKG3DRegionInfoManager;
class KG3DEngineOption;

class K3DEngineScriptTable
{
public:
    static void Load();

	static bool NewUserData(lua_State* L, IKG3DModel* pUserData);
	static bool NewUserData(lua_State* L, IKG3DLight* pUserData);
	static bool NewUserData(lua_State* L, IKG3DScene* pUserData);
	static bool NewUserData(lua_State* L, IKG3DCamera* pUserData);
	static bool NewUserData(lua_State* L, IKG3DModelSet* pUserData);
	static bool NewUserData(lua_State* L, IKG3DAnimationController* pUserData);
	static bool NewUserData(lua_State* L, IKG3DSFXSuede* pUserData);
	static bool NewUserData(lua_State* L, IKG3DResourceManager* pUserData);
	static bool NewUserData(lua_State* L, IKG3DSceneCameraMovement* pUserData);
	static bool NewUserData(lua_State* L, IKG3DSceneCameraAnimation* pUserData);
	static bool NewUserData(lua_State* L, IKG3DRegionInfoManager* pUserData);

	static IKG3DModel* ToIKG3DModel(lua_State* L, int nIndex);
	static IKG3DLight* ToIKG3DLight(lua_State* L, int nIndex);
	static IKG3DScene* ToIKG3DScene(lua_State* L, int nIndex);
	static IKG3DCamera* ToIKG3DCamera(lua_State* L, int nIndex);
	static IKG3DModelSet* ToIKG3DModelSet(lua_State* L, int nIndex);
	static IKG3DAnimationController* ToIKG3DAnimationController(lua_State* L, int nIndex);
	static IKG3DResourceManager* ToIKG3DResourceManager(lua_State* L, int nIndex);
	static IKG3DSceneCameraMovement* ToIKG3DSceneCameraMovement(lua_State* L, int nIndex);
	static IKG3DSceneCameraAnimation* ToIKG3DSceneCameraAnimation(lua_State* L, int nIndex);
	static IKG3DRegionInfoManager* ToIKG3DRegionInfoManager(lua_State* L, int nIndex);

    static int LuaGet3DEngineOption(lua_State* L, int nIndex, KG3DEngineOption *pRetInfo);
	static int Lua_Login_PostRoleLogin(lua_State* L); // 其实是检测外挂

private:
	static int Lua3DEngine_GetModelMgr(lua_State* L);
	static int Lua3DEngine_GetScene(lua_State* L);
	static int Lua3DEngine_NewScene(lua_State* L);
	static int Lua3DEngine_DeleteScene(lua_State* L);
	static int Lua3DEngine_NewRegionInfoMgr(lua_State* L);
	static int Lua3DEngine_DeleteRegionInfoMgr(lua_State* L);
	static int Lua3DEngine_Get3DEngineCaps(lua_State* L);
	static int Lua3DEngine_Get3DEngineOption(lua_State* L);
    
    static int Lua3DEngine_Get3DEngineOptionCaps(lua_State* L);
	static int Lua3DEngine_Set3DEngineOption(lua_State* L);
	static int Lua3DEngine_ExecuteCommand(lua_State* L);

	static int LuaModelMgr_NewModel(lua_State* L);

	static int LuaScene_EnablePostRenderEffect(lua_State* L);
	static int LuaScene_AddRenderEntity(lua_State* L);
	static int LuaScene_RemoveRenderEntity(lua_State* L);
	static int LuaScene_GetCamera(lua_State* L);
	static int LuaScene_GetCameraMovement(lua_State* L);
    static int LuaScene_GetLoadingProcess(lua_State* L);
    static int LuaScene_SetFocus(lua_State* L);

	static int LuaModel_RegisterEventHandler(lua_State* L);
	static int LuaModel_UnregisterEventHandler(lua_State* L);
	static int LuaModel_PlayAnimation(lua_State* L);
	static int LuaModel_LoadMaterialFromFile(lua_State* L);
	static int LuaModel_SetTranslation(lua_State* L);
	static int LuaModel_SetRotation(lua_State* L);
	static int LuaModel_SetScaling(lua_State* L);
	static int LuaModel_GetTranslation(lua_State* L);
	static int LuaModel_GetRotation(lua_State* L);
	static int LuaModel_GetScaling(lua_State* L);
	static int LuaModel_BindToSocket(lua_State* L);
	static int LuaModel_BindToBone(lua_State* L);
	static int LuaModel_UnbindFromOther(lua_State* L);
	static int LuaModel_SetAlpha(lua_State* L);
	static int LuaModel_SetHighLightLevel(lua_State* L);
	static int LuaModel_GetHightLightLevel(lua_State* L);
	static int LuaModel_GetAlpha(lua_State* L);
	static int LuaModel_GetAnimationController(lua_State* L);
	static int LuaModel_GetSFXSuede(lua_State* L);
	static int LuaModel_Release(lua_State* L);
	static int LuaModel_eq(lua_State* L);
	static int LuaModel_Attach(lua_State* L);
	static int LuaModel_Detach(lua_State* L);
	static int LuaModel_SetYaw(lua_State* L);
    static int LuaModel_SetFaceMotionMode(lua_State* L);
    static int LuaModel_SetDetail(lua_State* L);

	static int LuaCamera_GetPosition(lua_State* L);
	static int LuaCamera_SetPosition(lua_State* L);
	static int LuaCamera_SetLookAtPosition(lua_State* L);
	static int LuaCamera_GetLookAtPosition(lua_State* L);
	static int LuaCamera_GetUpDirection(lua_State* L);
	static int LuaCamera_SetUpDirection(lua_State* L);
	static int LuaCamera_GetCurrentState(lua_State* L);
	static int LuaCamera_UpdateCamera(lua_State* L);
	static int LuaCamera_SetPerspective(lua_State* L);
	static int LuaCamera_SetOrthogonal(lua_State* L);

	static int LuaCameraMovement_GetCurrentAnimation(lua_State* L);
	static int LuaCameraMovement_SetCurrentAnimation(lua_State* L);
	static int LuaCameraMovement_GetAnimationCount(lua_State* L);
	static int LuaCameraMovement_FindAnimation(lua_State* L);
	static int LuaCameraMovement_Clear(lua_State* L);
	static int LuaCameraMovement_LoadFromFile(lua_State* L);

	static int LuaCameraAnimation_Play(lua_State* L);
	static int LuaCameraAnimation_Stop(lua_State* L);
	static int LuaCameraAnimation_Pause(lua_State* L);
	static int LuaCameraAnimation_GetName(lua_State* L);
	static int LuaCameraAnimation_SetName(lua_State* L);
	static int LuaCameraAnimation_IsPlaying(lua_State* L);

	static int LuaRegionInfoMgr_GetData(lua_State* L);
};

#endif // UI_SCRIPT_K_3D_ENGINE_SCRIPT_TABLE_H

