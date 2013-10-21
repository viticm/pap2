#include "stdafx.h"
#include "SO3UI/IKGUI.h"
#include "KG3DEngine/KG3DInterface.h"
#include "./SO3Represent/SO3RepresentHandler.h"
#include "./kui3dmodeleventhandler.h"
#include "./k3denginescripttable.h"
#include "./kscript.h"
#include "./SO3UI.h"
#include "../CheckGameCheatLib/CheckGameCheat.h"

extern BOOL HastaLaVistaBaby(LPCSTR account, LPCSTR role, LPCSTR region, LPCSTR server);


void K3DEngineScriptTable::Load()
{
	static luaL_reg const s_a3DEngineLibTable[] =
	{
		{ "GetModelMgr",					Lua3DEngine_GetModelMgr },
		{ "GetScene",						Lua3DEngine_GetScene },
		{ "NewScene",						Lua3DEngine_NewScene },
		{ "DeleteScene",					Lua3DEngine_DeleteScene },
		{ "NewRegionInfoMgr",				Lua3DEngine_NewRegionInfoMgr },
		{ "DeleteRegionInfoMgr",			Lua3DEngine_DeleteRegionInfoMgr },
		{ "Get3DEngineOption",				Lua3DEngine_Get3DEngineOption },
		{ "Get3DEngineCaps",				Lua3DEngine_Get3DEngineCaps },
		{ "Set3DEngineOption",				Lua3DEngine_Set3DEngineOption },
        { "ExecuteCommand",                 Lua3DEngine_ExecuteCommand },
        { "Get3DEngineOptionCaps",          Lua3DEngine_Get3DEngineOptionCaps},

		{ NULL,								NULL },
	};

	static luaL_reg const s_aResourceManagerMetaTable[] =
	{
		{ "NewModel",						LuaModelMgr_NewModel },
		{ NULL,								NULL },
	};

	static luaL_reg const s_aModelMetaTable[] =
	{
		{ "RegisterEventHandler",			LuaModel_RegisterEventHandler },
		{ "UnregisterEventHandler",			LuaModel_UnregisterEventHandler },
		{ "PlayAnimation",					LuaModel_PlayAnimation },
		{ "LoadMaterialFromFile",			LuaModel_LoadMaterialFromFile },
		{ "SetTranslation",					LuaModel_SetTranslation },
		{ "SetRotation",					LuaModel_SetRotation },
		{ "SetScaling",						LuaModel_SetScaling },
		{ "GetTranslation",					LuaModel_GetTranslation },
		{ "GetRotation",					LuaModel_GetRotation },
		{ "GetScaling",						LuaModel_GetScaling },
		{ "BindToSocket",					LuaModel_BindToSocket },
		{ "BindToBone",						LuaModel_BindToBone },
		{ "UnbindFromOther",				LuaModel_UnbindFromOther },
		{ "SetAlpha",						LuaModel_SetAlpha },
		{ "SetHighLightLevel",				LuaModel_SetHighLightLevel },
		{ "GetHightLightLevel",				LuaModel_GetHightLightLevel },
		{ "GetAlpha",						LuaModel_GetAlpha },
		{ "GetAnimationController",			LuaModel_GetAnimationController },
		{ "GetSFXSuede",					LuaModel_GetSFXSuede },
		{ "Release",						LuaModel_Release },
		{ "__eq",							LuaModel_eq },
		{ "Attach",							LuaModel_Attach },
		{ "Detach",							LuaModel_Detach },
		{ "SetYaw",							LuaModel_SetYaw },
        { "SetFaceMotionMode",              LuaModel_SetFaceMotionMode },
        { "SetDetail",                      LuaModel_SetDetail },

		{ NULL,								NULL },
	};

	static luaL_reg const s_aCameraMetaTable[] =
	{
		{ "GetPosition",					LuaCamera_GetPosition },
		{ "SetPosition",					LuaCamera_SetPosition },
		{ "SetLookAtPosition",				LuaCamera_SetLookAtPosition },
		{ "GetLookAtPosition",				LuaCamera_GetLookAtPosition },
		{ "GetUpDirection",					LuaCamera_GetUpDirection },
		{ "SetUpDirection",					LuaCamera_SetUpDirection },
		{ "GetCurrentState",				LuaCamera_GetCurrentState },
		{ "UpdateCamera",					LuaCamera_UpdateCamera },
		{ "SetPerspective",					LuaCamera_SetPerspective },
		{ "SetOrthogonal",					LuaCamera_SetOrthogonal },

		{ NULL,								NULL },
	};

	static luaL_reg const s_aCameraMovementMetaTable[] =
	{
		{ "GetCurrentAnimation",			LuaCameraMovement_GetCurrentAnimation },
		{ "SetCurrentAnimation",			LuaCameraMovement_SetCurrentAnimation },
		{ "GetAnimationCount",				LuaCameraMovement_GetAnimationCount },
		{ "FindAnimation",					LuaCameraMovement_FindAnimation },
		{ "Clear",							LuaCameraMovement_Clear },
		{ "LoadFromFile",					LuaCameraMovement_LoadFromFile },

		{ NULL,								NULL },
	};

	static luaL_reg const s_aCameraAnimationMetaTable[] =
	{
		{ "Play",							LuaCameraAnimation_Play },
		{ "Stop",							LuaCameraAnimation_Stop },
		{ "Pause",							LuaCameraAnimation_Pause },
		{ "GetName",						LuaCameraAnimation_GetName },
		{ "SetName",						LuaCameraAnimation_SetName },
		{ "IsPlaying",						LuaCameraAnimation_IsPlaying },

		{ NULL,								NULL },
	};

	static luaL_reg const s_aLightMetaTable[] =
	{
		{ NULL,									NULL },
	};

	static luaL_reg const s_aSceneMetaTable[] =
	{
		{ "EnablePostRenderEffect",				LuaScene_EnablePostRenderEffect },
		{ "AddRenderEntity",					LuaScene_AddRenderEntity },
		{ "RemoveRenderEntity",					LuaScene_RemoveRenderEntity },
		{ "GetCamera",							LuaScene_GetCamera },
		{ "GetCameraMovement",					LuaScene_GetCameraMovement },
		{ "GetLoadingProcess",					LuaScene_GetLoadingProcess },
        { "SetFocus",                           LuaScene_SetFocus },

		{ NULL,									NULL },
	};

	static luaL_reg const s_aModelSetMetaTable[] =
	{
		{ NULL,									NULL },
	};

	static luaL_reg const s_aAnimationControllerMetaTable[] =
	{
		{ NULL,									NULL },
	};

	static luaL_reg const s_aRegionInfoMgrMetaTable[] =
	{
		{ "GetData",							LuaRegionInfoMgr_GetData },
		{ NULL,									NULL },
	};

	static luaL_reg s_aGlobalFunctionTable[] =
	{
        { "k3dcmd", Lua3DEngine_ExecuteCommand },
        { "Login_PostRoleLogin", Lua_Login_PostRoleLogin },

		{ NULL, NULL },
	};

	g_pSO3UI->m_pUI->RegisterLibTable("KG3DEngine", s_a3DEngineLibTable);

	g_pSO3UI->m_pUI->RegisterMetaTable("IKG3DResourceManager", s_aResourceManagerMetaTable);
	g_pSO3UI->m_pUI->RegisterMetaTable("IKG3DModel", s_aModelMetaTable);
	g_pSO3UI->m_pUI->RegisterMetaTable("IKG3DCamera",	s_aCameraMetaTable);
	g_pSO3UI->m_pUI->RegisterMetaTable("IKG3DSceneCameraMovement", s_aCameraMovementMetaTable);
	g_pSO3UI->m_pUI->RegisterMetaTable("IKG3DSceneCameraAnimation", s_aCameraAnimationMetaTable);
	g_pSO3UI->m_pUI->RegisterMetaTable("IKG3DLight", s_aLightMetaTable);
	g_pSO3UI->m_pUI->RegisterMetaTable("IKG3DScene", s_aSceneMetaTable);
	g_pSO3UI->m_pUI->RegisterMetaTable("IKG3DModelSet", s_aModelSetMetaTable);
	g_pSO3UI->m_pUI->RegisterMetaTable("IKG3DAnimationController", s_aAnimationControllerMetaTable);
	g_pSO3UI->m_pUI->RegisterMetaTable("IKG3DRegionInfoManager", s_aRegionInfoMgrMetaTable);

	g_pSO3UI->m_pUI->RegisterFunctions(s_aGlobalFunctionTable);
}


int K3DEngineScriptTable::Lua3DEngine_GetModelMgr(lua_State* L)
{
	BOOL bRetCode = FALSE;

	ASSERT(L);

	bRetCode = NewUserData(L, g_pSO3UI->m_p3DModelManager);
	KGLOG_PROCESS_ERROR(bRetCode);
	return 1;
Exit0:
	return 0;
}


int K3DEngineScriptTable::Lua3DEngine_GetScene(lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	DWORD dwSceneID = UINT_MAX;
	IKG3DScene* p3DScene = NULL;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 1;
	KGLOG_PROCESS_ERROR(bRetCode);

	dwSceneID = static_cast<DWORD>(lua_tonumber(L, 1));

	hr = g_pSO3UI->m_pRepresent->GetScene(dwSceneID, &p3DScene);
	KGLOG_COM_PROCESS_ERROR(hr);

	bRetCode = NewUserData(L, p3DScene);
	KGLOG_PROCESS_ERROR(bRetCode);

	return 1;
Exit0:
	return 0;
}


int K3DEngineScriptTable::Lua3DEngine_NewScene(lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	IKG3DScene* p3DScene = NULL;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 0;
	KGLOG_PROCESS_ERROR(bRetCode);

	hr = g_pSO3UI->m_p3DUI->CreateScene(&p3DScene);
	KGLOG_COM_PROCESS_ERROR(hr);

	bRetCode = NewUserData(L, p3DScene);
	KGLOG_PROCESS_ERROR(bRetCode);

	return 1;
Exit0:
	if (p3DScene)
		g_pSO3UI->m_p3DUI->UnloadScene(p3DScene);
	return 0;
}


int K3DEngineScriptTable::Lua3DEngine_DeleteScene(lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	IKG3DScene* p3DScene = NULL;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 1;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DScene = ToIKG3DScene(L, 1);
	KGLOG_PROCESS_ERROR(p3DScene);

	hr = g_pSO3UI->m_p3DUI->UnloadScene(p3DScene);
	KGLOG_COM_CHECK_ERROR(hr);

	return 0;
Exit0:
	return 0;
}


int K3DEngineScriptTable::Lua3DEngine_NewRegionInfoMgr(lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	LPCSTR szFilePath = NULL;
	IKG3DRegionInfoManager* p3DRegionInfoMgr = NULL;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 1;
	KGLOG_PROCESS_ERROR(bRetCode);

	szFilePath = lua_tostring(L, 1);
	KGLOG_PROCESS_ERROR(szFilePath);

	hr = g_pSO3UI->m_p3DEngineManager->GetRegionInfoManager(szFilePath, 0, 0, &p3DRegionInfoMgr);
	KGLOG_COM_PROCESS_ERROR(hr);

	ASSERT(p3DRegionInfoMgr);

	bRetCode = NewUserData(L, p3DRegionInfoMgr);
	KGLOG_PROCESS_ERROR(bRetCode);

	return 1;
Exit0:
	if (p3DRegionInfoMgr)
		p3DRegionInfoMgr->Release();
	return 0;
}


int K3DEngineScriptTable::Lua3DEngine_DeleteRegionInfoMgr(lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	IKG3DRegionInfoManager* p3DRegionInfoMgr = NULL;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 1;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DRegionInfoMgr = ToIKG3DRegionInfoManager(L, 1);
	KGLOG_PROCESS_ERROR(p3DRegionInfoMgr);

	p3DRegionInfoMgr->Release();

	return 0;
Exit0:
	return 0;
}

int K3DEngineScriptTable::Lua3DEngine_Get3DEngineCaps(lua_State* L)
{
	HRESULT hr = E_FAIL;
	int nColorCurveStypeNum = 0;
	KG3DEngineCaps Caps;

	hr = g_pSO3UI->m_p3DEngineManager->GetEngineCaps(&Caps);
	KGLOG_COM_PROCESS_ERROR(hr);

	Lua_NewTable(L);

	Lua_PushString(L, "bShadow_None");
	Lua_PushBoolean(L, Caps.IsShadowTypeAvailable[EM_MODEL_SHADOW_TYPE_NONE]);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "bShadow_Low");
	Lua_PushBoolean(L, Caps.IsShadowTypeAvailable[EM_MODEL_SHADOW_TYPE_LOW]);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "bShadow_Middle");
	Lua_PushBoolean(L, Caps.IsShadowTypeAvailable[EM_MODEL_SHADOW_TYPE_MIDDLE]);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "bShadow_High");
	Lua_PushBoolean(L, Caps.IsShadowTypeAvailable[EM_MODEL_SHADOW_TYPE_HIGH]);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "ColorCurveStype");
	Lua_NewTable(L);
	nColorCurveStypeNum = g_pSO3UI->m_p3DEngineManager->GetColorCurveStypeNum();
	for (int i = 0; i < nColorCurveStypeNum; ++i)
	{
		char szBuffer[MAX_PATH];
		szBuffer[0] = '\0';
		g_pSO3UI->m_p3DEngineManager->GetColorCurveStypeName(szBuffer, i);
		szBuffer[MAX_PATH - 1] = '\0';
		Lua_PushNumber(L, i + 1);
		Lua_PushString(L, szBuffer);
		Lua_SetTable(L, -3);
	}
	Lua_SetTable(L, -3);

	return 1;
Exit0:
	return 0;
}

int K3DEngineScriptTable::Lua_Login_PostRoleLogin(lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	LPCSTR szAccount = NULL;
	LPCSTR szRole = NULL;
	LPCSTR szRegion = NULL;
	LPCSTR szServer = NULL;

	ASSERT(L);

	szAccount = static_cast<LPCSTR>(lua_tostring(L, 1));
	KGLOG_PROCESS_ERROR(szAccount);

	szRole = static_cast<LPCSTR>(lua_tostring(L, 2));
	KGLOG_PROCESS_ERROR(szRole);

	szRegion = static_cast<LPCSTR>(lua_tostring(L, 3));
	KGLOG_PROCESS_ERROR(szRegion);

	szServer = static_cast<LPCSTR>(lua_tostring(L, 4));
	KGLOG_PROCESS_ERROR(szServer);

	HastaLaVistaBaby(szAccount, szRole, szRegion, szServer);

	return 0;
Exit0:
	return 0;  
}

int K3DEngineScriptTable::LuaGet3DEngineOption(lua_State* L, int nIndex, KG3DEngineOption *pRetInfo)
{
    int nResult = false;
    HRESULT hr = E_FAIL;

    ASSERT(L);
    ASSERT(pRetInfo);

    KGLOG_PROCESS_ERROR(lua_istable(L, nIndex));
    
    hr = g_pSO3UI->m_p3DEngineManager->GetEngineOption(pRetInfo);
    KGLOG_COM_PROCESS_ERROR(hr);

    lua_getfield(L, nIndex, "nWaterDetail");
    if (!(lua_isnil(L, -1)))
        pRetInfo->nWaterDetail = (int)Lua_ValueToNumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, nIndex, "bCpuSkin");
    if (!(lua_isnil(L, -1)))
        pRetInfo->bCpuSkin = Lua_ValueToBoolean(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, nIndex, "bFlexBodySmooth");
    if (!(lua_isnil(L, -1)))
        pRetInfo->bFlexBodySmooth = Lua_ValueToBoolean(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, nIndex, "bRenderGrass");
    if (!(lua_isnil(L, -1)))
        pRetInfo->bRenderGrass = Lua_ValueToBoolean(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, nIndex, "bGrassAnimation");
    if (!(lua_isnil(L, -1)))
        pRetInfo->bGrassAnimation = Lua_ValueToBoolean(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, nIndex, "nTextureScale");
    if (!(lua_isnil(L, -1)))
        pRetInfo->nTextureScale = (int)Lua_ValueToNumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, nIndex, "bAmbientShaderOnly");
    if (!(lua_isnil(L, -1)))
        pRetInfo->bAmbientShaderOnly = Lua_ValueToBoolean(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, nIndex, "bPostEffectEnable");
    if (!(lua_isnil(L, -1)))
        pRetInfo->bPostEffectEnable = Lua_ValueToBoolean(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, nIndex, "bBloomEnable");
    if (!(lua_isnil(L, -1)))
        pRetInfo->bBloomEnable = Lua_ValueToBoolean(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, nIndex, "bShockWaveEnable");
    if (!(lua_isnil(L, -1)))
        pRetInfo->bShockWaveEnable = Lua_ValueToBoolean(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, nIndex, "bHSIEnable");
    if (!(lua_isnil(L, -1)))
        pRetInfo->bHSIEnable = Lua_ValueToBoolean(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, nIndex, "bMotionBlur");
    if (!(lua_isnil(L, -1)))
        pRetInfo->bMotionBlur = Lua_ValueToBoolean(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, nIndex, "fCameraDistance");
    if (!(lua_isnil(L, -1)))
        pRetInfo->fCameraDistance = (float)Lua_ValueToNumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, nIndex, "nSampMipFilter");
    if (!(lua_isnil(L, -1)))
        pRetInfo->nSampMipFilter = (int)Lua_ValueToNumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, nIndex, "nSampMinFilter");
    if (!(lua_isnil(L, -1)))
        pRetInfo->nSampMinFilter = (int)Lua_ValueToNumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, nIndex, "nSampMagFilter");
    if (!(lua_isnil(L, -1)))
        pRetInfo->nSampMagFilter = (int)Lua_ValueToNumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, nIndex, "dwMaxAnisotropy");
    if (!(lua_isnil(L, -1)))
        pRetInfo->dwMaxAnisotropy = (DWORD)Lua_ValueToNumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, nIndex, "bGrassAlphaBlend");
    if (!(lua_isnil(L, -1)))
        pRetInfo->bGrassAlphaBlend = Lua_ValueToBoolean(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, nIndex, "bGodRay");
    if (!(lua_isnil(L, -1)))
        pRetInfo->bGodRay = Lua_ValueToBoolean(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, nIndex, "bSSAO");
    if (!(lua_isnil(L, -1)))
        pRetInfo->bSSAO = Lua_ValueToBoolean(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, nIndex, "bCurveCMYK");
    if (!(lua_isnil(L, -1)))
        pRetInfo->bCurveCMYK = Lua_ValueToBoolean(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, nIndex, "nActiveCurveStype");
    if (!(lua_isnil(L, -1)))
        pRetInfo->nActiveCurveStype = (int)Lua_ValueToNumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, nIndex, "nShadowType");
    if (!(lua_isnil(L, -1)))
        pRetInfo->ModelShadowType = (int)Lua_ValueToNumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, nIndex, "nVegetationDensity");
    if (!(lua_isnil(L, -1)))
        pRetInfo->nVegetationDensity = (int)Lua_ValueToNumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, nIndex, "nTerrainDetail");
    if (!(lua_isnil(L, -1)))
        pRetInfo->nTerrainDetail = (int)Lua_ValueToNumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, nIndex, "fCameraAngle");
    if (!(lua_isnil(L, -1)))
        pRetInfo->fCameraAngle = (float)Lua_ValueToNumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, nIndex, "bEnableDynamicConverMap");
    if (!(lua_isnil(L, -1)))
        pRetInfo->bEnableDynamicConverMap = (BOOL)Lua_ValueToBoolean(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, nIndex, "nForceShaderModel");
    if (!(lua_isnil(L, -1)))
        pRetInfo->nForceShaderModel = (int)Lua_ValueToNumber(L, -1);
    lua_pop(L, 1);
    
    lua_getfield(L, nIndex, "nForceShaderModel");
    if (!(lua_isnil(L, -1)))
        pRetInfo->nForceShaderModel = (int)Lua_ValueToNumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, nIndex, "bHDR");
    if (!(lua_isnil(L, -1)))
        pRetInfo->bHDR = (BOOL)Lua_ValueToBoolean(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, nIndex, "bDOF");
    if (!(lua_isnil(L, -1)))
        pRetInfo->bDOF = (BOOL)Lua_ValueToBoolean(L, -1);
    lua_pop(L, 1);

    nResult = true;
Exit0:
    return nResult;    
}

int K3DEngineScriptTable::Lua3DEngine_Get3DEngineOptionCaps(lua_State* L)
{
    ASSERT(L);

    HRESULT hr = E_FAIL;
    float fRetCode  = 0;
    int   nRetCode  = false;
    DWORD dwRetCode = 0;
    int *pArray = NULL;
    int nCount  = 0;
    int nSize   = 0;
    int nColorCurveStypeNum = 0;
    
    KG3DEngineOption Info;
    std::vector<KADAPTERMODE> *pAdapterMode = NULL;
    std::vector<int> *pShaderModel = NULL;
    std::vector<int> *pMultiSampleType = NULL;
    IKG3DEngineOptionCaps *pOptionCaps = NULL;
    
    KGLOG_PROCESS_ERROR(lua_gettop(L) == 1);
    KGLOG_PROCESS_ERROR(lua_istable(L, 1));

    pOptionCaps = g_pSO3UI->m_p3DEngineManager->CreateIKG3DEngineOptionCaps();
    KGLOG_PROCESS_ERROR(pOptionCaps);
    
    nRetCode = LuaGet3DEngineOption(L, 1, &Info);
    KGLOG_PROCESS_ERROR(nRetCode);
    
    hr = pOptionCaps->GetEngineOptionCaps(Info);
    KGLOG_COM_PROCESS_ERROR(hr);

    pAdapterMode = pOptionCaps->GetAdapterModes();
    KGLOG_PROCESS_ERROR(pAdapterMode);

    pMultiSampleType = pOptionCaps->GetMultiSampleType();
    KGLOG_PROCESS_ERROR(pMultiSampleType);

    pArray = pOptionCaps->GetModelShadowType();
    KGLOG_PROCESS_ERROR(pArray);

    pShaderModel = pOptionCaps->GetShaderModel();
    KGLOG_PROCESS_ERROR(pShaderModel);

    Lua_NewTable(L); 
    
    Lua_PushString(L, "aAdapterModes");
    Lua_NewTable(L); 

    nSize = (int)(*pAdapterMode).size();
    for (int i = 0; i < nSize; i++)
    {
        Lua_PushNumber(L, i + 1);
        Lua_NewTable(L);

        Lua_PushString(L, "nWidth");
        Lua_PushNumber(L, (*pAdapterMode)[i].uWidth);
        Lua_SetTable(L, -3);

        Lua_PushString(L, "nHeight");
        Lua_PushNumber(L, (*pAdapterMode)[i].uHeight);
        Lua_SetTable(L, -3);

        Lua_PushString(L, "uRefreshRates");
        Lua_NewTable(L);
        for (int j = 0; j < 32; j++)
        {
            if ((*pAdapterMode)[i].uRefreshRates[j] != 0)
            {
                Lua_PushNumber(L, j + 1);
                Lua_PushNumber(L, (*pAdapterMode)[i].uRefreshRates[j]);
                Lua_SetTable(L, -3);
            }
        }
        Lua_SetTable(L, -3);

        Lua_SetTable(L, -3);
    }
    Lua_SetTable(L, -3);

    Lua_PushString(L, "aMultiSampleType");
    Lua_NewTable(L); 
    

    nSize = (int)(*pMultiSampleType).size();
    for (int i = 0; i < nSize; i++)
    {
        Lua_PushNumber(L, i + 1);
        Lua_PushNumber(L, (*pMultiSampleType)[i]);
        Lua_SetTable(L, -3);
    }
    Lua_SetTable(L, -3);
    

    Lua_PushString(L, "bShadowNoneEnable");
    Lua_PushBoolean(L, (pArray[0] == -1 ? false : true));
    Lua_SetTable(L, -3);

    Lua_PushString(L, "bShadowLowEnable");
    Lua_PushBoolean(L, (pArray[1] == -1 ? false : true));
    Lua_SetTable(L, -3);

    Lua_PushString(L, "bShadowMiddleEanble");
    Lua_PushBoolean(L, (pArray[2] == -1 ? false : true));
    Lua_SetTable(L, -3);

    Lua_PushString(L, "bShadowHighEnable");
    Lua_PushBoolean(L, (pArray[3] == -1 ? false : true));
    Lua_SetTable(L, -3);

    nRetCode = pOptionCaps->GetRenderGrass();
    Lua_PushString(L, "bRenderGrassEnable");
    Lua_PushBoolean(L, nRetCode);
    Lua_SetTable(L, -3);

    nRetCode = pOptionCaps->GetGrassAnimation();
    Lua_PushString(L, "bGrassAnimationEnable");
    Lua_PushBoolean(L, nRetCode);
    Lua_SetTable(L, -3);

    nRetCode = pOptionCaps->GetGrassAlphaBlend();
    Lua_PushString(L, "bGrassAlphaBlendEnable");
    Lua_PushBoolean(L, nRetCode);
    Lua_SetTable(L, -3);

    nRetCode = pOptionCaps->GetMinTextureScale();
    Lua_PushString(L, "nMinTextureScale");
    Lua_PushNumber(L, nRetCode);
    Lua_SetTable(L, -3);

    nRetCode = pOptionCaps->GetMaxTextureScale();
    Lua_PushString(L, "nMaxTextureScale");
    Lua_PushNumber(L, nRetCode);
    Lua_SetTable(L, -3);

    fRetCode = pOptionCaps->GetMinCameraDistance();
    Lua_PushString(L, "fMinCameraDistance");
    Lua_PushNumber(L, fRetCode);
    Lua_SetTable(L, -3);

    fRetCode = pOptionCaps->GetMaxCameraDistance();
    Lua_PushString(L, "fMaxCameraDistance");
    Lua_PushNumber(L, fRetCode);
    Lua_SetTable(L, -3);

    fRetCode = pOptionCaps->GetMinCameraAngle();
    Lua_PushString(L, "fMinCameraAngle");
    Lua_PushNumber(L, fRetCode);
    Lua_SetTable(L, -3);

    fRetCode = pOptionCaps->GetMaxCameraAngle();
    Lua_PushString(L, "fMaxCameraAngle");
    Lua_PushNumber(L, fRetCode);
    Lua_SetTable(L, -3);

    nRetCode = pOptionCaps->GetPostEffectEnable();
    Lua_PushString(L, "bPostEffectEnable");
    Lua_PushBoolean(L, nRetCode);
    Lua_SetTable(L, -3);

    nRetCode = pOptionCaps->GetBloomEnable();
    Lua_PushString(L, "bBloomEnable");
    Lua_PushBoolean(L, nRetCode);
    Lua_SetTable(L, -3);

    nRetCode = pOptionCaps->GetHDREnable();
    Lua_PushString(L, "bHDREnable");
    Lua_PushBoolean(L, nRetCode);
    Lua_SetTable(L, -3);

    nRetCode = pOptionCaps->GetDOFEnable();
    Lua_PushString(L, "bDOFEnable");
    Lua_PushBoolean(L, nRetCode);
    Lua_SetTable(L, -3);

    nRetCode = pOptionCaps->GetShockWaveEnable();
    Lua_PushString(L, "bShockWaveEnable");
    Lua_PushBoolean(L, nRetCode);
    Lua_SetTable(L, -3);

    nRetCode = pOptionCaps->GetMotionBlur();
    Lua_PushString(L, "bMotionBlurEnable");
    Lua_PushBoolean(L, nRetCode);
    Lua_SetTable(L, -3);

    nRetCode = pOptionCaps->GetGodRay();
    Lua_PushString(L, "bGodRayEnable");
    Lua_PushBoolean(L, nRetCode);
    Lua_SetTable(L, -3);

    nRetCode = pOptionCaps->GetSSAO();
    Lua_PushString(L, "bSSAOEnable");
    Lua_PushBoolean(L, nRetCode);
    Lua_SetTable(L, -3);
    
    nRetCode = pOptionCaps->GetEnableDynamicConverMap();
    Lua_PushString(L, "bDynamicConverMapEnbale");
    Lua_PushBoolean(L, nRetCode);
    Lua_SetTable(L, -3);

    nRetCode = pOptionCaps->GetCpuSkin();
    Lua_PushString(L, "bCpuSkinEnable");
    Lua_PushBoolean(L, nRetCode);
    Lua_SetTable(L, -3);

    nRetCode = pOptionCaps->GetFlexBodySmooth();
    Lua_PushString(L, "bFlexBodySmoothEnable");
    Lua_PushBoolean(L, nRetCode);
    Lua_SetTable(L, -3);
    
    nRetCode = pOptionCaps->GetMinTerrainDetail();
    Lua_PushString(L, "nMinTerrainDetail");
    Lua_PushNumber(L, nRetCode);
    Lua_SetTable(L, -3);

    nRetCode = pOptionCaps->GetMaxTerrainDetail();
    Lua_PushString(L, "nMaxTerrainDetail");
    Lua_PushNumber(L, nRetCode);
    Lua_SetTable(L, -3);

    nRetCode = pOptionCaps->GetMinVegetationDensity();
    Lua_PushString(L, "nMinVegetationDensity");
    Lua_PushNumber(L, nRetCode);
    Lua_SetTable(L, -3);

    nRetCode = pOptionCaps->GetMaxVegetationDensity();
    Lua_PushString(L, "nMaxVegetationDensity");
    Lua_PushNumber(L, nRetCode);
    Lua_SetTable(L, -3);

    nRetCode = pOptionCaps->GetMinWaterDetail();
    Lua_PushString(L, "nMinWaterDetail");
    Lua_PushNumber(L, nRetCode);
    Lua_SetTable(L, -3);

    nRetCode = pOptionCaps->GetMaxWaterDetail();
    Lua_PushString(L, "nMaxWaterDetail");
    Lua_PushNumber(L, nRetCode);
    Lua_SetTable(L, -3);

    dwRetCode = pOptionCaps->GetMinAnisotropy();
    Lua_PushString(L, "dwMinAnisotropy");
    Lua_PushNumber(L, dwRetCode);
    Lua_SetTable(L, -3);

    dwRetCode = pOptionCaps->GetMaxAnisotropy();
    Lua_PushString(L, "dwMaxAnisotropy");
    Lua_PushNumber(L, dwRetCode);
    Lua_SetTable(L, -3);

    Lua_PushString(L, "aShaderModel");
    Lua_NewTable(L); 

    nSize = (int)(*pShaderModel).size();
    for (int i = 0; i < nSize; i++)
    {
        Lua_PushNumber(L, i + 1);
        Lua_PushNumber(L, (*pShaderModel)[i]);
        Lua_SetTable(L, -3);
    }
    Lua_SetTable(L, -3);
    
    Lua_PushString(L, "ColorCurveStype");
    Lua_NewTable(L);
    nColorCurveStypeNum = g_pSO3UI->m_p3DEngineManager->GetColorCurveStypeNum();
    for (int i = 0; i < nColorCurveStypeNum; ++i)
    {
        char szBuffer[MAX_PATH];
        szBuffer[0] = '\0';
        g_pSO3UI->m_p3DEngineManager->GetColorCurveStypeName(szBuffer, i);
        szBuffer[MAX_PATH - 1] = '\0';
        Lua_PushNumber(L, i + 1);
        Lua_PushString(L, szBuffer);
        Lua_SetTable(L, -3);
    }
    Lua_SetTable(L, -3);

    g_pSO3UI->m_p3DEngineManager->RemoveKG3DEngineOptionCaps(pOptionCaps);
    return 1;
Exit0:
    return 0;
}

int K3DEngineScriptTable::Lua3DEngine_Get3DEngineOption(lua_State* L)
{
	HRESULT hr = E_FAIL;
	KG3DEngineOption Info;

	ASSERT(L);

	hr = g_pSO3UI->m_p3DEngineManager->GetEngineOption(&Info);
	KGLOG_COM_PROCESS_ERROR(hr);

	Lua_NewTable(L);
    
	Lua_PushString(L, "nSampMipFilter");
	Lua_PushNumber(L, Info.nSampMipFilter);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "nSampMinFilter");
	Lua_PushNumber(L, Info.nSampMinFilter);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "nSampMagFilter");
	Lua_PushNumber(L, Info.nSampMagFilter);
	Lua_SetTable(L, -3);

    Lua_PushString(L, "dwMaxAnisotropy");
    Lua_PushNumber(L, Info.dwMaxAnisotropy);
    Lua_SetTable(L, -3);

	Lua_PushString(L, "nWaterDetail");
	Lua_PushNumber(L, Info.nWaterDetail);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "bCpuSkin");
	Lua_PushBoolean(L, Info.bCpuSkin);
	Lua_SetTable(L, -3);
	
	Lua_PushString(L, "bFlexBodySmooth");
	Lua_PushBoolean(L, Info.bFlexBodySmooth);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "bRenderGrass");
	Lua_PushBoolean(L, Info.bRenderGrass);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "bGrassAnimation");
	Lua_PushBoolean(L, Info.bGrassAnimation);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "bGrassAlphaBlend");
	Lua_PushBoolean(L, Info.bGrassAlphaBlend);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "nTextureScale");
	Lua_PushNumber(L, Info.nTextureScale);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "bAmbientShaderOnly");
	Lua_PushBoolean(L, Info.bAmbientShaderOnly);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "bPostEffectEnable");
	Lua_PushBoolean(L, Info.bPostEffectEnable);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "bBloomEnable");
	Lua_PushBoolean(L, Info.bBloomEnable);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "bShockWaveEnable");
	Lua_PushBoolean(L, Info.bShockWaveEnable);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "bHSIEnable");
	Lua_PushBoolean(L, Info.bHSIEnable);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "bMotionBlur");
	Lua_PushBoolean(L, Info.bMotionBlur);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "bGodRay");
	Lua_PushBoolean(L, Info.bGodRay);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "bSSAO");
	Lua_PushBoolean(L, Info.bSSAO);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "bCurveCMYK");
	Lua_PushBoolean(L, Info.bCurveCMYK);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "nActiveCurveStype");
	Lua_PushNumber(L, Info.nActiveCurveStype);
	Lua_SetTable(L, -3);	

	Lua_PushString(L, "nShadowType");
	Lua_PushNumber(L, Info.ModelShadowType);
	Lua_SetTable(L, -3);	

	Lua_PushString(L, "fCameraDistance");
	Lua_PushNumber(L, Info.fCameraDistance);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "nVegetationDensity");
	Lua_PushNumber(L, Info.nVegetationDensity);
	Lua_SetTable(L, -3);	

	Lua_PushString(L, "nTerrainDetail");
	Lua_PushNumber(L, Info.nTerrainDetail);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "fCameraAngle");
	Lua_PushNumber(L, Info.fCameraAngle);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "bEnableDynamicConverMap");
	Lua_PushBoolean(L, Info.bEnableDynamicConverMap);
	Lua_SetTable(L, -3);	

    Lua_PushString(L, "nForceShaderModel");
    Lua_PushNumber(L, Info.nForceShaderModel);
    Lua_SetTable(L, -3);

    Lua_PushString(L, "bHDR");
    Lua_PushBoolean(L, Info.bHDR);
    Lua_SetTable(L, -3);

    Lua_PushString(L, "bDOF");
    Lua_PushBoolean(L, Info.bDOF);
    Lua_SetTable(L, -3);

	return 1;
Exit0:
	return 0;
}

int K3DEngineScriptTable::Lua3DEngine_Set3DEngineOption(lua_State* L)
{
    ASSERT(L);
    HRESULT hr = E_FAIL;
    int nRetCode = false;
    KG3DEngineOption Info;

	KGLOG_PROCESS_ERROR(lua_gettop(L) == 1);
	KGLOG_PROCESS_ERROR(lua_istable(L, 1));

    nRetCode = LuaGet3DEngineOption(L, 1, &Info);
    KGLOG_PROCESS_ERROR(nRetCode);
    
	hr = g_pSO3UI->m_p3DEngineManager->SetEngineOption(&Info);
	KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
	return 0;
}

int K3DEngineScriptTable::Lua3DEngine_ExecuteCommand(lua_State* L)
{
    int nRetCode = false;
    HRESULT hr = E_FAIL;
    LPCSTR szCommand = NULL;

    KGLOG_PROCESS_ERROR(lua_gettop(L) == 1);

    szCommand = lua_tostring(L, 1);
    KGLOG_PROCESS_ERROR(szCommand);

    hr = g_pSO3UI->m_p3DEngineManager->ExecuteCommand(szCommand);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
    return 0;
}

int K3DEngineScriptTable::LuaScene_EnablePostRenderEffect(lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	IKG3DScene* p3DScene = NULL;
	int nParamCount = 0;
	int nOutputWindowID = -1;
	int nPostRenderEffectType = 0;
	BOOL bEnable = FALSE;

	ASSERT(L);

	nParamCount = lua_gettop(L);

	bRetCode = nParamCount ==3 || nParamCount == 4;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DScene = ToIKG3DScene(L, 1);
	KGLOG_PROCESS_ERROR(p3DScene);

	if (nParamCount == 3)
	{
		nOutputWindowID = static_cast<int>(lua_tonumber(L, 2));
		if (lua_isboolean(L, 3))
			bEnable = lua_toboolean(L, 3);
		else
			bEnable = (BOOL)(int)lua_tonumber(L, 3);

		hr = p3DScene->EnablePostRenderEffect(nOutputWindowID, bEnable);
		KGLOG_COM_PROCESS_ERROR(hr);
	}
	else
	{
		nOutputWindowID = static_cast<int>(lua_tonumber(L, 2));
		nPostRenderEffectType = static_cast<int>(lua_tonumber(L, 3));
		if (lua_isboolean(L, 4))
			bEnable = lua_toboolean(L, 4);
		else
			bEnable = (BOOL)(int)lua_tonumber(L, 4);

		hr = p3DScene->EnablePostRenderEffect(nOutputWindowID, (PostRenderEffectType)nPostRenderEffectType, bEnable);
		KGLOG_COM_PROCESS_ERROR(hr);
	}

	return 0;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaScene_AddRenderEntity(lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	IKG3DScene* p3DScene = NULL;
	IKG3DModel* p3DModel = NULL;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 2;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DScene = ToIKG3DScene(L, 1);
	KGLOG_PROCESS_ERROR(p3DScene);

	p3DModel = ToIKG3DModel(L, 2);
	KGLOG_PROCESS_ERROR(p3DModel);

	hr = p3DScene->AddRenderEntity(p3DModel, SCENEENTITY_MODEL, FALSE,TRUE);
	KGLOG_COM_PROCESS_ERROR(hr);

	return 0;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaScene_RemoveRenderEntity(lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	IKG3DScene* p3DScene = NULL;
	IKG3DModel* p3DModel = NULL;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 2;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DScene = ToIKG3DScene(L, 1);
	KGLOG_PROCESS_ERROR(p3DScene);

	p3DModel = ToIKG3DModel(L, 2);
	KGLOG_PROCESS_ERROR(p3DModel);

	hr = p3DScene->RemoveRenderEntity(p3DModel, SCENEENTITY_MODEL, FALSE);
	KGLOG_COM_PROCESS_ERROR(hr);

	return 0;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaScene_GetCamera(lua_State* L)
{
	BOOL bRetCode = FALSE;
	IKG3DScene* p3DScene = NULL;
	IKG3DCamera* p3DCamera = NULL;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 1;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DScene = ToIKG3DScene(L, 1);
	KGLOG_PROCESS_ERROR(p3DScene);

	p3DCamera = p3DScene->GetCurrentCamera();
	KGLOG_PROCESS_ERROR(p3DCamera);

	bRetCode = NewUserData(L, p3DCamera);
	KGLOG_PROCESS_ERROR(bRetCode);

	return 1;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaScene_GetCameraMovement(lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	IKG3DScene* p3DScene = NULL;
	IKG3DSceneCameraMovement* p3DCameraMovement = NULL;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 1;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DScene = ToIKG3DScene(L, 1);
	KGLOG_PROCESS_ERROR(p3DScene);

	hr = p3DScene->GetCameraMovement(&p3DCameraMovement);
	KGLOG_COM_PROCESS_ERROR(hr);
	KGLOG_PROCESS_ERROR(p3DCameraMovement);

	bRetCode = NewUserData(L, p3DCameraMovement);
	KGLOG_PROCESS_ERROR(bRetCode);

	return 1;
Exit0:
	return 0;
}

int K3DEngineScriptTable::LuaScene_GetLoadingProcess(lua_State* L)
{
	BOOL bRetCode = FALSE;
	IKG3DScene* p3DScene = NULL;
	float fPercentage = 1.0f;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 1;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DScene = ToIKG3DScene(L, 1);
	KGLOG_PROCESS_ERROR(p3DScene);

	fPercentage = p3DScene->GetLoadingProcess();

Exit0:
	Lua_PushNumber(L, fPercentage);
	return 1;
}

int K3DEngineScriptTable::LuaScene_SetFocus(lua_State* L)
{
    int nRetCode = false;
    IKG3DScene* p3DScene = NULL;
    D3DXVECTOR3 vFocus;

    ASSERT(L);

    nRetCode = lua_gettop(L) == 4;
    KGLOG_PROCESS_ERROR(nRetCode);

    p3DScene = ToIKG3DScene(L, 1);
    KGLOG_PROCESS_ERROR(p3DScene);

    vFocus.x = (float)lua_tonumber(L, 2);
    vFocus.y = (float)lua_tonumber(L, 3);
    vFocus.z = (float)lua_tonumber(L, 4);

    p3DScene->SetFocus(vFocus);

Exit0:
    return 0;
}

int K3DEngineScriptTable::LuaModel_RegisterEventHandler(lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	IKG3DModel* p3DModel = NULL;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 1;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DModel = ToIKG3DModel(L, 1);
	KGLOG_PROCESS_ERROR(p3DModel);

	IKG3DAnimationController* pController = p3DModel->GetAnimationController(ANICTL_PRIMARY);
	KG_PROCESS_ERROR(pController);

	hr = pController->RegistereEventHandler(&KUI3DModelEventHandler::GetSelf());
	KGLOG_COM_PROCESS_ERROR(hr);

	return 0;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaModel_UnregisterEventHandler(lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	IKG3DModel* p3DModel = NULL;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 1;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DModel = ToIKG3DModel(L, 1);
	KGLOG_PROCESS_ERROR(p3DModel);

	IKG3DAnimationController* pController = p3DModel->GetAnimationController(ANICTL_PRIMARY);
	KG_PROCESS_ERROR(pController);

	hr = pController->UnregisterEventHandler(&KUI3DModelEventHandler::GetSelf());
	KG_COM_PROCESS_ERROR(hr);

	return 0;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaModel_PlayAnimation(lua_State* L)
{
	int nRetCode = false;
    int nResult = false;
	HRESULT hr = E_FAIL;
	IKG3DModel* p3DModel = NULL;
    IKG3DAnimationController* p3DAnimationController = NULL;
	DWORD dwPlayType = 0;
	LPCSTR szAnimationName = NULL;
	LPCSTR szPlayType = NULL;
	float fSpeed = 1.0f;
	int nOffsetTime = 0;
	float fTweenTime = 100.0f;
    BOOL bPlayLast = FALSE;

	ASSERT(L);

	nRetCode = lua_gettop(L) == 5;
	KGLOG_PROCESS_ERROR(nRetCode);

	p3DModel = ToIKG3DModel(L, 1);
	KGLOG_PROCESS_ERROR(p3DModel);

	szPlayType = lua_tostring(L, 2);
	KGLOG_PROCESS_ERROR(szPlayType);

	if (strcmp(szPlayType, "loop") == 0)
    {
		dwPlayType = ENU_ANIMATIONPLAY_CIRCLE;
    }
	else if (strcmp(szPlayType, "once") == 0)
    {
		dwPlayType = ENU_ANIMATIONPLAY_ONCE;
    }
	else if (strcmp(szPlayType, "last") == 0)
    {
		dwPlayType = ENU_ANIMATIONPLAY_ONCE;
        bPlayLast = TRUE;
    }
	else
    {
		dwPlayType = ENU_ANIMATIONPLAY_CIRCLE;
    }

	szAnimationName = lua_tostring(L, 3);
	KGLOG_PROCESS_ERROR(szAnimationName);

	fSpeed = (float)lua_tonumber(L, 4);

	nOffsetTime = (int)lua_tonumber(L, 5);

	hr = p3DModel->PlayAnimation(dwPlayType, szAnimationName, fSpeed, nOffsetTime, (PVOID)&fTweenTime, NULL, ANICTL_PRIMARY);
	KGLOG_COM_PROCESS_ERROR(hr);

    if (bPlayLast)
    {
        p3DAnimationController = p3DModel->GetAnimationController(ANICTL_PRIMARY);
        KGLOG_PROCESS_ERROR(p3DAnimationController);

        p3DAnimationController->SeekAnimation(ANIMATION_SEEK_SET, ANIMATION_SEEK_OFFSET_END);
    }

	nResult = true;
Exit0:
    if (!nResult)
    {
        KGLogPrintf(KGLOG_ERR, "SO3UI K3DEngineScriptTable::LuaModel_PlayAnimation(%s, %s, %f, %d)\n", 
            szAnimationName, szPlayType, fSpeed, nOffsetTime);
    }
	return 0;
}


int K3DEngineScriptTable::LuaModel_LoadMaterialFromFile(lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	IKG3DModel* p3DModel = NULL;
	LPCSTR szFilePath = NULL;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 2;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DModel = ToIKG3DModel(L, 1);
	KGLOG_PROCESS_ERROR(p3DModel);

	szFilePath = static_cast<LPCSTR>(lua_tostring(L, 2));
	KGLOG_PROCESS_ERROR(szFilePath);

	hr = p3DModel->LoadMaterialFromFile(szFilePath, MLO_TEXTURE_MULTI);
	KGLOG_COM_PROCESS_ERROR(hr);

	return 0;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaModel_SetTranslation(lua_State* L)
{
	BOOL bRetCode = FALSE;
	IKG3DModel* p3DModel = NULL;
	D3DXVECTOR3 vPosition;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 4;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DModel = ToIKG3DModel(L, 1);
	KGLOG_PROCESS_ERROR(p3DModel);

	vPosition.x = static_cast<float>(lua_tonumber(L, 2));
	vPosition.y = static_cast<float>(lua_tonumber(L, 3));
	vPosition.z = static_cast<float>(lua_tonumber(L, 4));

	p3DModel->SetTranslation(&vPosition);

	return 0;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaModel_SetRotation(lua_State* L)
{
	BOOL bRetCode = FALSE;
	IKG3DModel* p3DModel = NULL;
	D3DXQUATERNION qRotation;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 5;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DModel = ToIKG3DModel(L, 1);
	KGLOG_PROCESS_ERROR(p3DModel);

	qRotation.x = static_cast<float>(lua_tonumber(L, 2));
	qRotation.y = static_cast<float>(lua_tonumber(L, 3));
	qRotation.z = static_cast<float>(lua_tonumber(L, 4));
	qRotation.w = static_cast<float>(lua_tonumber(L, 5));

	p3DModel->SetRotation(&qRotation);

	return 0;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaModel_SetScaling(lua_State* L)
{
	BOOL bRetCode = FALSE;
	IKG3DModel* p3DModel = NULL;
	D3DXVECTOR3 vScale;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 4;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DModel = ToIKG3DModel(L, 1);
	KGLOG_PROCESS_ERROR(p3DModel);

	vScale.x = static_cast<float>(lua_tonumber(L, 2));
	vScale.y = static_cast<float>(lua_tonumber(L, 3));
	vScale.z = static_cast<float>(lua_tonumber(L, 4));

	p3DModel->SetScaling(&vScale);

	return 0;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaModel_GetTranslation(lua_State* L)
{
	BOOL bRetCode = FALSE;
	IKG3DModel* p3DModel = NULL;
	D3DXVECTOR3 vPosition;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 1;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DModel = ToIKG3DModel(L, 1);
	KGLOG_PROCESS_ERROR(p3DModel);

	p3DModel->GetTranslation(&vPosition);

	lua_pushnumber(L, vPosition.x);
	lua_pushnumber(L, vPosition.y);
	lua_pushnumber(L, vPosition.z);
	return 3;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaModel_GetRotation(lua_State* L)
{
	BOOL bRetCode = FALSE;
	IKG3DModel* p3DModel = NULL;
	D3DXQUATERNION qRotation;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 1;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DModel = ToIKG3DModel(L, 1);
	KGLOG_PROCESS_ERROR(p3DModel);

	p3DModel->GetRotation(&qRotation);

	lua_pushnumber(L, qRotation.x);
	lua_pushnumber(L, qRotation.y);
	lua_pushnumber(L, qRotation.z);
	lua_pushnumber(L, qRotation.w);
	return 4;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaModel_GetScaling(lua_State* L)
{
	BOOL bRetCode = FALSE;
	IKG3DModel* p3DModel = NULL;
	D3DXVECTOR3 vScale;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 1;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DModel = ToIKG3DModel(L, 1);
	KGLOG_PROCESS_ERROR(p3DModel);

	p3DModel->GetScaling(&vScale);

	lua_pushnumber(L, vScale.x);
	lua_pushnumber(L, vScale.y);
	lua_pushnumber(L, vScale.z);
	return 3;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaModel_BindToSocket(lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	IKG3DModel* p3DModel = NULL;
	IKG3DModel* p3DModelBindTo = NULL;
	LPCSTR szSocket = NULL;
	int nParamCount = 0;

	ASSERT(L);

	nParamCount = lua_gettop(L);

	bRetCode = nParamCount == 2 || nParamCount == 3;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DModel = ToIKG3DModel(L, 1);
	KGLOG_PROCESS_ERROR(p3DModel);

	p3DModelBindTo = ToIKG3DModel(L, 2);
	KGLOG_PROCESS_ERROR(p3DModelBindTo);

	if (nParamCount == 3)
	{
		szSocket = static_cast<LPCSTR>(lua_tostring(L, 3));
		KGLOG_PROCESS_ERROR(szSocket);
	}
    else
    {
        szSocket = "";
    }

    if (strcmp("S_Face", szSocket) == 0)
    {
        p3DModel->SetFaceMotionMode();
    }

    hr = p3DModel->BindToSocket(p3DModelBindTo, szSocket);
	KGLOG_COM_PROCESS_ERROR(hr);

	return 0;
Exit0:
	KGLogPrintf(KGLOG_ERR, "SO3UI K3DEngineScriptTable::LuaModel_BindToSocket(%.8x, %.8x, %s)\n",
		p3DModel, p3DModelBindTo, szSocket ? szSocket : "");
	return 0;
}


int K3DEngineScriptTable::LuaModel_BindToBone(lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	IKG3DModel* p3DModel = NULL;
	IKG3DModel* p3DModelBindTo = NULL;
	LPCSTR szBone = NULL;
	int nParamCount = 0;

	ASSERT(L);

	nParamCount = lua_gettop(L);

	bRetCode = nParamCount == 2 || nParamCount == 3;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DModel = ToIKG3DModel(L, 1);
	KGLOG_PROCESS_ERROR(p3DModel);

	p3DModelBindTo = ToIKG3DModel(L, 2);
	KGLOG_PROCESS_ERROR(p3DModelBindTo);

	if (nParamCount == 3)
	{
		szBone = static_cast<LPCSTR>(lua_tostring(L, 3));
		KGLOG_PROCESS_ERROR(szBone);
	}
    else
    {
        szBone = "";
    }

	hr = p3DModel->BindToBone(p3DModelBindTo, szBone);
	KGLOG_COM_PROCESS_ERROR(hr);

	return 0;
Exit0:
	KGLogPrintf(KGLOG_ERR, "SO3UI K3DEngineScriptTable::LuaModel_BindToBone(%.8x, %.8x, %s)\n",
		p3DModel, p3DModelBindTo, szBone ? szBone : "");
	return 0;
}


int K3DEngineScriptTable::LuaModel_UnbindFromOther(lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	IKG3DModel* p3DModel = NULL;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 1;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DModel = ToIKG3DModel(L, 1);
	KGLOG_PROCESS_ERROR(p3DModel);

	hr = p3DModel->UnBindFromOther();
	KGLOG_COM_PROCESS_ERROR(hr);

	return 0;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaModel_SetAlpha(lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	IKG3DModel* p3DModel = NULL;
	float fAlpha = 0.0f;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 2;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DModel = ToIKG3DModel(L, 1);
	KGLOG_PROCESS_ERROR(p3DModel);

	fAlpha = static_cast<float>(lua_tonumber(L, 2));

	hr = p3DModel->SetAlpha(fAlpha, TRUE);
	KGLOG_COM_PROCESS_ERROR(hr);

	return 0;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaModel_SetHighLightLevel(lua_State* L)
{
	BOOL bRetCode = FALSE;
	IKG3DModel* p3DModel = NULL;
	float fIllumination = 0.0f;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 2;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DModel = ToIKG3DModel(L, 1);
	KGLOG_PROCESS_ERROR(p3DModel);

	fIllumination = static_cast<float>(lua_tonumber(L, 2));

	p3DModel->SetHighLightLevel(fIllumination, TRUE);

	return 0;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaModel_GetHightLightLevel(lua_State* L)
{
	BOOL bRetCode = FALSE;
	IKG3DModel* p3DModel = NULL;
	float fIllumination = 0.0f;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 1;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DModel = ToIKG3DModel(L, 1);
	KGLOG_PROCESS_ERROR(p3DModel);

	fIllumination = p3DModel->GetHightLightLevel();

	lua_pushnumber(L, fIllumination);
	return 1;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaModel_GetAlpha(lua_State* L)
{
	BOOL bRetCode = FALSE;
	IKG3DModel* p3DModel = NULL;
	float fAlpha = 0.0f;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 1;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DModel = ToIKG3DModel(L, 1);
	KGLOG_PROCESS_ERROR(p3DModel);

	fAlpha = p3DModel->GetAlpha();

	lua_pushnumber(L, fAlpha);
	return 1;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaModel_GetAnimationController(lua_State* L)
{
	BOOL bRetCode = FALSE;
	IKG3DModel* p3DModel = NULL;
	IKG3DAnimationController* p3DModelAnimationController = NULL;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 1;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DModel = ToIKG3DModel(L, 1);
	KGLOG_PROCESS_ERROR(p3DModel);

	p3DModelAnimationController = p3DModel->GetAnimationController(ANICTL_PRIMARY);
	KGLOG_PROCESS_ERROR(p3DModelAnimationController);

	bRetCode = NewUserData(L, p3DModelAnimationController);
	KGLOG_PROCESS_ERROR(bRetCode);

	return 1;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaModel_GetSFXSuede(lua_State* L)
{
	BOOL bRetCode = FALSE;
	IKG3DModel* p3DModel = NULL;
	IKG3DSFXSuede* p3DModelSFXSuede = NULL;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 1;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DModel = ToIKG3DModel(L, 1);
	KGLOG_PROCESS_ERROR(p3DModel);

	p3DModelSFXSuede = p3DModel->GetSFXSuede();
	KGLOG_PROCESS_ERROR(p3DModelSFXSuede);

	bRetCode = NewUserData(L, p3DModelSFXSuede);
	KGLOG_PROCESS_ERROR(bRetCode);

	return 1;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaModel_Release(lua_State* L)
{
	BOOL bRetCode = FALSE;
	IKG3DModel* p3DModel = NULL;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 1;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DModel = ToIKG3DModel(L, 1);
	KGLOG_PROCESS_ERROR(p3DModel);

	p3DModel->Release();

	return 0;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaModel_eq(lua_State* L)
{
	BOOL bRetCode = FALSE;
	BOOL bResult = FALSE;
	IKG3DModel* p3DModelLhs = NULL;
	IKG3DModel* p3DModelRhs = NULL;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 2;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DModelLhs = ToIKG3DModel(L, 1);
	KGLOG_PROCESS_ERROR(p3DModelLhs);

	p3DModelRhs = ToIKG3DModel(L, 2);
	KGLOG_PROCESS_ERROR(p3DModelRhs);

	bResult = p3DModelLhs == p3DModelRhs;
	
Exit0:
	lua_pushboolean(L, bResult);
	return 1;
}


int K3DEngineScriptTable::LuaModel_Attach(lua_State* L)
{
	BOOL bRetCode = FALSE;
	IKG3DModel* p3DModelLhs = NULL;
	IKG3DModel* p3DModelRhs = NULL;
	int nIndex = -1;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 2;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DModelLhs = ToIKG3DModel(L, 1);
	KGLOG_PROCESS_ERROR(p3DModelLhs);

	p3DModelRhs = ToIKG3DModel(L, 2);
	KGLOG_PROCESS_ERROR(p3DModelRhs);

	nIndex = p3DModelLhs->AttachModel(p3DModelRhs);
	KGLOG_PROCESS_ERROR(nIndex != -1);

	return 0;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaModel_Detach(lua_State* L)
{
	BOOL bRetCode = FALSE;
	IKG3DModel* p3DModelLhs = NULL;
	IKG3DModel* p3DModelRhs = NULL;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 2;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DModelLhs = ToIKG3DModel(L, 1);
	KGLOG_PROCESS_ERROR(p3DModelLhs);

	p3DModelRhs = ToIKG3DModel(L, 2);
	KGLOG_PROCESS_ERROR(p3DModelRhs);

	p3DModelLhs->DetachModel(p3DModelRhs);

	return 0;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaModel_SetYaw(lua_State* L)
{
	D3DXVECTOR3 const AXIS_YAW_LH = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	BOOL bRetCode = FALSE;
	IKG3DModel* p3DModel = NULL;
	D3DXQUATERNION qRotation;
	float fYaw = 0.0f;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 2;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DModel = ToIKG3DModel(L, 1);
	KGLOG_PROCESS_ERROR(p3DModel);

	fYaw = (float)lua_tonumber(L, 2);

	D3DXQuaternionRotationAxis(&qRotation, &AXIS_YAW_LH, fYaw);

	p3DModel->SetRotation(&qRotation);

	return 0;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaModel_SetFaceMotionMode(lua_State* L)
{
	int nRetCode = false;
    int nResult = 0;
	IKG3DModel* p3DModel = NULL;

	ASSERT(L);

	nRetCode = lua_gettop(L) == 1;
	KGLOG_PROCESS_ERROR(nRetCode);

	p3DModel = ToIKG3DModel(L, 1);
	KGLOG_PROCESS_ERROR(p3DModel);

	p3DModel->SetFaceMotionMode();

Exit0:
	return nResult;
}


int K3DEngineScriptTable::LuaModel_SetDetail(lua_State* L)
{
	int nRetCode = false;
    int nResult = 0;
	IKG3DModel* p3DModel = NULL;
    int nColorChannelTable = 0;
    int nColorChannel = 0;

	ASSERT(L);

	nRetCode = lua_gettop(L) == 3;
	KGLOG_PROCESS_ERROR(nRetCode);

	p3DModel = ToIKG3DModel(L, 1);
	KGLOG_PROCESS_ERROR(p3DModel);

    nColorChannelTable = (int)lua_tonumber(L, 2);
    nColorChannel = (int)lua_tonumber(L, 3);

	p3DModel->SetDetail(nColorChannelTable, nColorChannel);

Exit0:
	return nResult;
}


int K3DEngineScriptTable::LuaCamera_GetPosition(lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	IKG3DCamera* p3DCamera = NULL;
	D3DXVECTOR3 vPosition;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 1;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DCamera = ToIKG3DCamera(L, 1);
	KGLOG_PROCESS_ERROR(p3DCamera);

	hr = p3DCamera->GetPosition(&vPosition);
	KGLOG_COM_PROCESS_ERROR(hr);

	lua_pushnumber(L, vPosition.x);
	lua_pushnumber(L, vPosition.y);
	lua_pushnumber(L, vPosition.z);
	return 3;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaCamera_SetPosition(lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	IKG3DCamera* p3DCamera = NULL;
	D3DXVECTOR3 vPosition;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 4;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DCamera = ToIKG3DCamera(L, 1);
	KGLOG_PROCESS_ERROR(p3DCamera);

	vPosition.x = static_cast<float>(lua_tonumber(L, 2));
	vPosition.y = static_cast<float>(lua_tonumber(L, 3));
	vPosition.z = static_cast<float>(lua_tonumber(L, 4));

	hr = p3DCamera->SetPosition(vPosition);
	KGLOG_COM_PROCESS_ERROR(hr);

	return 0;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaCamera_SetLookAtPosition(lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	IKG3DCamera* p3DCamera = NULL;
	D3DXVECTOR3 vPosition;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 4;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DCamera = ToIKG3DCamera(L, 1);
	KGLOG_PROCESS_ERROR(p3DCamera);

	vPosition.x = static_cast<float>(lua_tonumber(L, 2));
	vPosition.y = static_cast<float>(lua_tonumber(L, 3));
	vPosition.z = static_cast<float>(lua_tonumber(L, 4));

	hr = p3DCamera->SetLookAtPosition(vPosition);
	KGLOG_COM_PROCESS_ERROR(hr);

	return 0;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaCamera_GetLookAtPosition(lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	IKG3DCamera* p3DCamera = NULL;
	D3DXVECTOR3 vPosition;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 1;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DCamera = ToIKG3DCamera(L, 1);
	KGLOG_PROCESS_ERROR(p3DCamera);

	hr = p3DCamera->GetLookAtPosition(&vPosition);
	KGLOG_COM_PROCESS_ERROR(hr);

	lua_pushnumber(L, vPosition.x);
	lua_pushnumber(L, vPosition.y);
	lua_pushnumber(L, vPosition.z);
	return 3;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaCamera_GetUpDirection(lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	IKG3DCamera* p3DCamera = NULL;
	D3DXVECTOR3 vDirection;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 1;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DCamera = ToIKG3DCamera(L, 1);
	KGLOG_PROCESS_ERROR(p3DCamera);

	hr = p3DCamera->GetUpDirection(&vDirection);
	KGLOG_COM_PROCESS_ERROR(hr);

	lua_pushnumber(L, vDirection.x);
	lua_pushnumber(L, vDirection.y);
	lua_pushnumber(L, vDirection.z);
	return 3;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaCamera_SetUpDirection(lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	IKG3DCamera* p3DCamera = NULL;
	D3DXVECTOR3 vDirection;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 4;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DCamera = ToIKG3DCamera(L, 1);
	KGLOG_PROCESS_ERROR(p3DCamera);

	vDirection.x = static_cast<float>(lua_tonumber(L, 2));
	vDirection.y = static_cast<float>(lua_tonumber(L, 3));
	vDirection.z = static_cast<float>(lua_tonumber(L, 4));

	hr = p3DCamera->SetUpDirection(vDirection);
	KGLOG_COM_PROCESS_ERROR(hr);

	return 0;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaCamera_GetCurrentState(lua_State* L)
{
	BOOL bRetCode = FALSE;
	IKG3DCamera* p3DCamera = NULL;
	DWORD dwCurrentState = 0;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 1;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DCamera = ToIKG3DCamera(L, 1);
	KGLOG_PROCESS_ERROR(p3DCamera);

	dwCurrentState = p3DCamera->GetCurrentState();

	lua_pushnumber(L, dwCurrentState);
	return 1;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaCamera_UpdateCamera(lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	IKG3DCamera* p3DCamera = NULL;
	D3DXVECTOR3 vPosition;
	D3DXVECTOR3 vAxes;
	float fLookAtOffset = 0.0f;
	float fTargetSpeed = 0.0f;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 9;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DCamera = ToIKG3DCamera(L, 1);
	KGLOG_PROCESS_ERROR(p3DCamera);

	vPosition.x = (float)lua_tonumber(L, 2);
	vPosition.y = (float)lua_tonumber(L, 3);
	vPosition.z = (float)lua_tonumber(L, 4);
	vAxes.x = (float)lua_tonumber(L, 5);
	vAxes.y = (float)lua_tonumber(L, 6);
	vAxes.z = (float)lua_tonumber(L, 7);
	fLookAtOffset = (float)lua_tonumber(L, 8);
	fTargetSpeed = (float)lua_tonumber(L, 9);

	hr = p3DCamera->UpdateCamera(vPosition, vAxes, fLookAtOffset, fTargetSpeed);
	KGLOG_COM_PROCESS_ERROR(hr);


	return 0;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaCamera_SetPerspective(lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	IKG3DCamera* p3DCamera = NULL;
    float fFovy = 0.0f;
    float fAspect = 0.0f;
    float fZNear = 0.0f;
    float fZFar = 0.0f;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 5;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DCamera = ToIKG3DCamera(L, 1);
	KGLOG_PROCESS_ERROR(p3DCamera);

	hr = p3DCamera->GetGlobalPerspective(&fFovy, &fAspect, &fZNear, &fZFar);
	KGLOG_COM_PROCESS_ERROR(hr);

	if (lua_isnumber(L, 2))
		fFovy = (float)(lua_tonumber(L, 2));
	if (lua_isnumber(L, 3))
		fAspect = (float)(lua_tonumber(L, 3));
	if (lua_isnumber(L, 4))
		fZNear = (float)(lua_tonumber(L, 4));
	if (lua_isnumber(L, 5))
		fZFar = (float)(lua_tonumber(L, 5));

	hr = p3DCamera->SetGlobalPerspective(fFovy, fAspect, fZNear, fZFar);
	KGLOG_COM_PROCESS_ERROR(hr);

	return 0;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaCamera_SetOrthogonal(lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	IKG3DCamera* p3DCamera = NULL;
    float fWidth = 0.0f;
    float fHeight = 0.0f;
    float fZNear = 0.0f;
    float fZFar = 0.0f;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 5;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DCamera = ToIKG3DCamera(L, 1);
	KGLOG_PROCESS_ERROR(p3DCamera);

	hr = p3DCamera->GetGlobalOrthogonal(&fWidth, &fHeight, &fZNear, &fZFar);
	KGLOG_COM_PROCESS_ERROR(hr);

	if (lua_isnumber(L, 2))
		fWidth = (float)(lua_tonumber(L, 2));
	if (lua_isnumber(L, 3))
		fHeight = (float)(lua_tonumber(L, 3));
	if (lua_isnumber(L, 4))
		fZNear = (float)(lua_tonumber(L, 4));
	if (lua_isnumber(L, 5))
		fZFar = (float)(lua_tonumber(L, 5));

	hr = p3DCamera->SetGlobalOrthogonal(fWidth, fHeight, fZNear, fZFar);
	KGLOG_COM_PROCESS_ERROR(hr);

	return 0;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaCameraMovement_GetCurrentAnimation(lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	IKG3DSceneCameraMovement* p3DCameraMovement = NULL;
	IKG3DSceneCameraAnimation* p3DCameraAnimation = NULL;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 1;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DCameraMovement = ToIKG3DSceneCameraMovement(L, 1);
	KGLOG_PROCESS_ERROR(p3DCameraMovement);

	hr = p3DCameraMovement->GetCurCamAnimation(&p3DCameraAnimation);
	KGLOG_COM_PROCESS_ERROR(hr);

	bRetCode = NewUserData(L, p3DCameraAnimation);
	KG_PROCESS_ERROR(bRetCode);

	return 1;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaCameraMovement_SetCurrentAnimation(lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	IKG3DSceneCameraMovement* p3DCameraMovement = NULL;
	int nCameraAnimationIndex = 0;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 2;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DCameraMovement = ToIKG3DSceneCameraMovement(L, 1);
	KGLOG_PROCESS_ERROR(p3DCameraMovement);

	nCameraAnimationIndex = (int)lua_tonumber(L, 2);

	hr = p3DCameraMovement->SetCurCamAnimation(nCameraAnimationIndex);
	KGLOG_COM_PROCESS_ERROR(hr);

	return 0;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaCameraMovement_GetAnimationCount(lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	IKG3DSceneCameraMovement* p3DCameraMovement = NULL;
	DWORD dwCameraAnimationCount = 0;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 1;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DCameraMovement = ToIKG3DSceneCameraMovement(L, 1);
	KGLOG_PROCESS_ERROR(p3DCameraMovement);

	hr = p3DCameraMovement->GetAnimationCount(&dwCameraAnimationCount);
	KGLOG_COM_PROCESS_ERROR(hr);

	lua_pushnumber(L, dwCameraAnimationCount);
	return 1;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaCameraMovement_FindAnimation(lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	IKG3DSceneCameraMovement* p3DCameraMovement = NULL;
	LPCSTR szCameraAnimation = NULL;
	int nCameraAnimationIndex = 0;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 2;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DCameraMovement = ToIKG3DSceneCameraMovement(L, 1);
	KGLOG_PROCESS_ERROR(p3DCameraMovement);

	szCameraAnimation = lua_tostring(L, 2);
	KGLOG_PROCESS_ERROR(szCameraAnimation && szCameraAnimation[0] != '\0');

	hr = p3DCameraMovement->FindCamAniByName(szCameraAnimation, 1, &nCameraAnimationIndex);
	KGLOG_COM_PROCESS_ERROR(hr);

	lua_pushnumber(L, nCameraAnimationIndex);
	return 1;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaCameraMovement_Clear(lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	IKG3DSceneCameraMovement* p3DCameraMovement = NULL;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 1;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DCameraMovement = ToIKG3DSceneCameraMovement(L, 1);
	KGLOG_PROCESS_ERROR(p3DCameraMovement);

	hr = p3DCameraMovement->Clear();
	KGLOG_COM_PROCESS_ERROR(hr);

	return 0;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaCameraMovement_LoadFromFile(lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	IKG3DSceneCameraMovement* p3DCameraMovement = NULL;
	LPCSTR szFilePath = NULL;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 2;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DCameraMovement = ToIKG3DSceneCameraMovement(L, 1);
	KGLOG_PROCESS_ERROR(p3DCameraMovement);

	szFilePath = lua_tostring(L, 2);
	KGLOG_PROCESS_ERROR(szFilePath && szFilePath[0] != '\0');

	hr = p3DCameraMovement->LoadFromFile((LPSTR)szFilePath);
	KGLOG_COM_PROCESS_ERROR(hr);

	return 0;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaCameraAnimation_Play(lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	IKG3DSceneCameraAnimation* p3DCameraAnimation = NULL;
	BOOL bLoop = FALSE;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 2;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DCameraAnimation = ToIKG3DSceneCameraAnimation(L, 1);
	KGLOG_PROCESS_ERROR(p3DCameraAnimation);

	if (Lua_IsBoolean(L, 2))
		bLoop = lua_toboolean(L, 2);
	else
		bLoop = (BOOL)(int)lua_tonumber(L, 2);

	p3DCameraAnimation->StartPlay(bLoop);

	return 0;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaCameraAnimation_Stop(lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	IKG3DSceneCameraAnimation* p3DCameraAnimation = NULL;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 1;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DCameraAnimation = ToIKG3DSceneCameraAnimation(L, 1);
	KGLOG_PROCESS_ERROR(p3DCameraAnimation);

	hr = p3DCameraAnimation->StopPlay();
	KGLOG_COM_PROCESS_ERROR(hr);

	return 0;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaCameraAnimation_Pause(lua_State* L)
{
	BOOL bRetCode = FALSE;
    HRESULT hr = E_FAIL;
	IKG3DSceneCameraAnimation* p3DCameraAnimation = NULL;
    IKG3DSceneCameraKeyFrame Keyframe;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 1;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DCameraAnimation = ToIKG3DSceneCameraAnimation(L, 1);
	KGLOG_PROCESS_ERROR(p3DCameraAnimation);

	hr = p3DCameraAnimation->PausePlay(&Keyframe, NULL);
    KGLOG_COM_PROCESS_ERROR(hr);

    lua_newtable(L);

    // position
    lua_newtable(L);

    lua_pushnumber(L, Keyframe.vPosition.x);
    lua_setfield(L, -2, "x");

    lua_pushnumber(L, Keyframe.vPosition.y);
    lua_setfield(L, -2, "y");

    lua_pushnumber(L, Keyframe.vPosition.z);
    lua_setfield(L, -2, "z");

    lua_setfield(L, -2, "position");

    // lookat
    lua_newtable(L);

    lua_pushnumber(L, Keyframe.vLookAt.x);
    lua_setfield(L, -2, "x");

    lua_pushnumber(L, Keyframe.vLookAt.y);
    lua_setfield(L, -2, "y");

    lua_pushnumber(L, Keyframe.vLookAt.z);
    lua_setfield(L, -2, "z");

    lua_setfield(L, -2, "lookat");

    // tangent position
    lua_newtable(L);

    lua_pushnumber(L, Keyframe.vPositionTangent.x);
    lua_setfield(L, -2, "x");

    lua_pushnumber(L, Keyframe.vPositionTangent.y);
    lua_setfield(L, -2, "y");

    lua_pushnumber(L, Keyframe.vPositionTangent.z);
    lua_setfield(L, -2, "z");

    lua_setfield(L, -2, "tangentposition");

    // tangent lookat
    lua_newtable(L);

    lua_pushnumber(L, Keyframe.vLookAtTangent.x);
    lua_setfield(L, -2, "x");

    lua_pushnumber(L, Keyframe.vLookAtTangent.y);
    lua_setfield(L, -2, "y");

    lua_pushnumber(L, Keyframe.vLookAtTangent.z);
    lua_setfield(L, -2, "z");

    lua_setfield(L, -2, "tangentlookat");

	return 1;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaCameraAnimation_GetName(lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	IKG3DSceneCameraAnimation* p3DCameraAnimation = NULL;
	LPCSTR szName = NULL;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 1;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DCameraAnimation = ToIKG3DSceneCameraAnimation(L, 1);
	KGLOG_PROCESS_ERROR(p3DCameraAnimation);

	hr = p3DCameraAnimation->GetName(&szName);
	KGLOG_COM_PROCESS_ERROR(hr);

	KG_ASSERT_EXIT(szName && szName[0] != '\0');

	lua_pushstring(L, szName);
	return 1;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaCameraAnimation_SetName(lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	IKG3DSceneCameraAnimation* p3DCameraAnimation = NULL;
	LPCSTR szName = NULL;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 2;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DCameraAnimation = ToIKG3DSceneCameraAnimation(L, 1);
	KGLOG_PROCESS_ERROR(p3DCameraAnimation);

	szName = lua_tostring(L, 2);
	KGLOG_PROCESS_ERROR(szName && szName[0] != '\0');

	hr = p3DCameraAnimation->SetName(szName);
	KGLOG_COM_PROCESS_ERROR(hr);

	return 0;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaCameraAnimation_IsPlaying(lua_State* L)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;
	IKG3DSceneCameraAnimation* p3DCameraAnimation = NULL;
	BOOL bIsPlaying = FALSE;

	ASSERT(L);

	bRetCode = lua_gettop(L) == 1;
	KGLOG_PROCESS_ERROR(bRetCode);

	p3DCameraAnimation = ToIKG3DSceneCameraAnimation(L, 1);
	KGLOG_PROCESS_ERROR(p3DCameraAnimation);

	hr = p3DCameraAnimation->GetPlayState(&bIsPlaying);
	KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
	lua_pushboolean(L, bIsPlaying);
	return 1;
}


int K3DEngineScriptTable::LuaRegionInfoMgr_GetData(lua_State* L)
{
	HRESULT hr = E_FAIL;
	D3DXVECTOR3 vPos;
	DWORD dwData = 0;
	IKG3DRegionInfoManager* p3DRegionInfoMgr = NULL;

	ASSERT(L);

	KGLOG_PROCESS_ERROR(lua_gettop(L) == 4);

	p3DRegionInfoMgr = ToIKG3DRegionInfoManager(L, 1);
	KGLOG_PROCESS_ERROR(p3DRegionInfoMgr);

	vPos.x = (float)lua_tonumber(L, 2);
	vPos.y = (float)lua_tonumber(L, 3);
	vPos.z = (float)lua_tonumber(L, 4);
	
	hr = p3DRegionInfoMgr->GetRegionInfoData(vPos, &dwData);
	KGLOG_COM_PROCESS_ERROR(hr);

	lua_pushnumber(L, dwData);
	return 1;
Exit0:
	return 0;
}


int K3DEngineScriptTable::LuaModelMgr_NewModel(lua_State* L)
{
	int nRetCode = false;
    int nResult = 0;
	HRESULT hr = E_FAIL;
	LPCSTR szFilePath = NULL;
	IKG3DModel* p3DModel = NULL;
	IKG3DResourceManager* p3DModelMgr = NULL;

	ASSERT(L);

	KGLOG_PROCESS_ERROR(lua_gettop(L) == 2);

	p3DModelMgr = ToIKG3DResourceManager(L, 1);
	KGLOG_PROCESS_ERROR(p3DModelMgr);

	szFilePath = lua_tostring(L, 2);
	KGLOG_PROCESS_ERROR(szFilePath && szFilePath[0] != '\0');

	hr = p3DModelMgr->LoadResourceFromFile(szFilePath, 0, MLO_MULTI|MLO_MODEL_LOD, (IKG3DResourceBase**)&p3DModel);
	KGLOG_COM_PROCESS_ERROR(hr);

	nRetCode = NewUserData(L, p3DModel);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = 1;
Exit0:
    if (nResult == 0)
    {
	    SAFE_RELEASE(p3DModel);

        KGLogPrintf(KGLOG_ERR, "K3DEngineScriptTable::LuaModelMgr_NewModel(%s)\n", szFilePath);
    }
	return nResult;
}


bool K3DEngineScriptTable::NewUserData(lua_State* L, IKG3DModel* pUserData)
{
	return TNewUserData(L, pUserData, "IKG3DModel");
}


bool K3DEngineScriptTable::NewUserData(lua_State* L, IKG3DLight* pUserData)
{
	return TNewUserData(L, pUserData, "IKG3DLight");
}


bool K3DEngineScriptTable::NewUserData(lua_State* L, IKG3DScene* pUserData)
{
	return TNewUserData(L, pUserData, "IKG3DScene");
}


bool K3DEngineScriptTable::NewUserData(lua_State* L, IKG3DCamera* pUserData)
{
	return TNewUserData(L, pUserData, "IKG3DCamera");
}


bool K3DEngineScriptTable::NewUserData(lua_State* L, IKG3DModelSet* pUserData)
{
	return TNewUserData(L, pUserData, "IKG3DModelSet");
}


bool K3DEngineScriptTable::NewUserData(lua_State* L, IKG3DAnimationController* pUserData)
{
	return TNewUserData(L, pUserData, "IKG3DAnimationController");
}


bool K3DEngineScriptTable::NewUserData(lua_State* L, IKG3DSFXSuede* pUserData)
{
	return TNewUserData(L, pUserData, "IKG3DSFXSuede");
}


bool K3DEngineScriptTable::NewUserData(lua_State* L, IKG3DResourceManager* pUserData)
{
	return TNewUserData(L, pUserData, "IKG3DResourceManager");
}


bool K3DEngineScriptTable::NewUserData(lua_State* L, IKG3DSceneCameraMovement* pUserData)
{
	return TNewUserData(L, pUserData, "IKG3DSceneCameraMovement");
}


bool K3DEngineScriptTable::NewUserData(lua_State* L, IKG3DSceneCameraAnimation* pUserData)
{
	return TNewUserData(L, pUserData, "IKG3DSceneCameraAnimation");
}


bool K3DEngineScriptTable::NewUserData(lua_State* L, IKG3DRegionInfoManager* pUserData)
{
	return TNewUserData(L, pUserData, "IKG3DRegionInfoManager");
}


IKG3DModel* K3DEngineScriptTable::ToIKG3DModel(lua_State* L, int nIndex)
{
	IKG3DModel** ppUserData = (IKG3DModel**)lua_touserdata(L, nIndex);
	return ppUserData ? *ppUserData : NULL;
}


IKG3DCamera* K3DEngineScriptTable::ToIKG3DCamera(lua_State* L, int nIndex)
{
	IKG3DCamera** ppUserData = (IKG3DCamera**)lua_touserdata(L, nIndex);
	return ppUserData ? *ppUserData : NULL;
}


IKG3DLight* K3DEngineScriptTable::ToIKG3DLight(lua_State* L, int nIndex)
{
	IKG3DLight** ppUserData = (IKG3DLight**)lua_touserdata(L, nIndex);
	return ppUserData ? *ppUserData : NULL;
}


IKG3DScene* K3DEngineScriptTable::ToIKG3DScene(lua_State* L, int nIndex)
{
	IKG3DScene** ppUserData = (IKG3DScene**)lua_touserdata(L, nIndex);
	return ppUserData ? *ppUserData : NULL;
}


IKG3DModelSet* K3DEngineScriptTable::ToIKG3DModelSet(lua_State* L, int nIndex)
{
	IKG3DModelSet** ppUserData = (IKG3DModelSet**)lua_touserdata(L, nIndex);
	return ppUserData ? *ppUserData : NULL;
}


IKG3DAnimationController* K3DEngineScriptTable::ToIKG3DAnimationController(lua_State* L, int nIndex)
{
	IKG3DAnimationController** ppUserData = (IKG3DAnimationController**)lua_touserdata(L, nIndex);
	return ppUserData ? *ppUserData : NULL;
}


IKG3DResourceManager* K3DEngineScriptTable::ToIKG3DResourceManager(lua_State* L, int nIndex)
{
	IKG3DResourceManager** ppUserData = (IKG3DResourceManager**)lua_touserdata(L, nIndex);
	return ppUserData ? *ppUserData : NULL;
}

IKG3DSceneCameraMovement* K3DEngineScriptTable::ToIKG3DSceneCameraMovement(lua_State* L, int nIndex)
{
	IKG3DSceneCameraMovement** ppUserData = (IKG3DSceneCameraMovement**)lua_touserdata(L, nIndex);
	return ppUserData ? *ppUserData : NULL;
}

IKG3DSceneCameraAnimation* K3DEngineScriptTable::ToIKG3DSceneCameraAnimation(lua_State* L, int nIndex)
{
	IKG3DSceneCameraAnimation** ppUserData = (IKG3DSceneCameraAnimation**)lua_touserdata(L, nIndex);
	return ppUserData ? *ppUserData : NULL;
}

IKG3DRegionInfoManager* K3DEngineScriptTable::ToIKG3DRegionInfoManager(lua_State* L, int nIndex)
{
	IKG3DRegionInfoManager** ppUserData = (IKG3DRegionInfoManager**)lua_touserdata(L, nIndex);
	return ppUserData ? *ppUserData : NULL;
}
