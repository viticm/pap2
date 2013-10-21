#include "stdafx.h"
#include "KPlayerServer.h"
#include "KScene.h"
#include "KPlayer.h"
#include "KSO3World.h"

#ifdef _CLIENT
#include "KPlayerClient.h"
#include "SO3UI/SO3GameWorldUIHandler.h"
#endif

int KCharacter::LuaAttach(Lua_State* L)
{
    BOOL        bResult         = false;
    BOOL        bRetCode        = false;
    DWORD       dwCharacterID   = 0;
    int         nDebugLevel     = 0;
    int         nTopIndex       = 0;
    KCharacter* pCharacter      = NULL;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);
    
    nDebugLevel = (int)Lua_ValueToNumber(L, 1);

    bRetCode = m_SelectTarget.GetTarget(&pCharacter);
    KGLOG_PROCESS_ERROR(bRetCode);

    KGLOG_PROCESS_ERROR(pCharacter);
    dwCharacterID = pCharacter->m_dwID;

    bRetCode = pCharacter->m_SkillDebug.Attach(m_dwID, nDebugLevel);
    KG_PROCESS_ERROR(bRetCode);

    pCharacter->m_SkillDebug.Trace(
        1, "[技能调试][%d]:[调试系统] 调试开始(等级%d)，目标:(%s, %u)。",
        g_pSO3World->m_nGameLoop, nDebugLevel, pCharacter->m_szName, pCharacter->m_dwID
    );

    bResult = true;
Exit0:
    lua_pushboolean(L, bResult);
    return 1;
}

int KCharacter::LuaDetach(Lua_State* L)
{
    BOOL        bResult         = false;
    BOOL        bRetCode        = false;
    KCharacter* pCharacter      = NULL;
    DWORD       dwCharacterID   = 0;
    int         nTopIndex       = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 0);

    bRetCode = m_SelectTarget.GetTarget(&pCharacter);
    KGLOG_PROCESS_ERROR(bRetCode);

    KGLOG_PROCESS_ERROR(pCharacter);
    dwCharacterID = pCharacter->m_dwID;

    pCharacter->m_SkillDebug.Trace(
        0, "[技能调试][%d]:[调试系统] 调试结束，目标:(%s, %u)。",
        g_pSO3World->m_nGameLoop, pCharacter->m_szName, pCharacter->m_dwID
    );

    bRetCode = pCharacter->m_SkillDebug.Detach(dwCharacterID);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    lua_pushboolean(L, bResult);
    return 1;
}

int KCharacter::LuaDetachAll(Lua_State* L)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;
    int  nTopIndex  = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 0);

    bRetCode = m_SkillDebug.DetachAll();
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    lua_pushboolean(L, bResult);
    return 1;
}

int KCharacter::LuaSetModelID(Lua_State* L)
{
	int     nRetCode  = 0;
    KNpc*   pNpc      = NULL;
    DWORD   dwModelID = 0;

    nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 1);

	dwModelID = (DWORD)Lua_ValueToNumber(L, 1);

    if (dwModelID == 0 && IS_NPC(m_dwID))
    {
        pNpc = (KNpc*)(this);
        if (pNpc->m_pTemplate)
        {
            dwModelID = pNpc->m_pTemplate->dwRepresentID[0];
        }
    }
    
    m_dwModelID = dwModelID;

#if defined(_SERVER)
	g_PlayerServer.DoCharacterChangeModel(this);
#endif

Exit0:
	return 0;
}

int KCharacter::LuaSetTarget(Lua_State* L)
{
	BOOL        bRetCode        = false;
    DWORD       dwID            = 0;
    TARGET_TYPE eTargetType     = ttInvalid;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	eTargetType = (TARGET_TYPE)(int)Lua_ValueToNumber(L, 1);
	dwID = (DWORD)Lua_ValueToNumber(L, 2);

    KG_PROCESS_ERROR(eTargetType != m_SelectTarget.m_eTargetType || dwID != m_SelectTarget.m_dwID);

    if (m_SelectTarget.m_eTargetType == ttNoTarget && eTargetType == ttNoTarget)
    {
        goto Exit0;
    }

    bRetCode = SelectTarget(eTargetType, dwID);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return 0;
}

int KCharacter::LuaGetTarget(Lua_State* L)
{
	BOOL            bRetCode        = false;
	TARGET_TYPE     eTargetType     = ttNoTarget;
    KSceneObject*   pSceneObject    = NULL;
	DWORD           dwTargetID      = 0;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 0);

    bRetCode = m_SelectTarget.GetTarget(&pSceneObject);
    KG_PROCESS_ERROR(bRetCode);

    KG_PROCESS_ERROR(pSceneObject && pSceneObject->m_pScene);
    
    eTargetType = m_SelectTarget.GetTargetType();
    dwTargetID  = pSceneObject->m_dwID;

Exit0:
	lua_pushinteger(L, eTargetType);
	lua_pushinteger(L, dwTargetID);
	return 2;
}

int KCharacter::LuaGetSkillTarget(Lua_State* L)
{
    BOOL            bRetCode        = false;
    TARGET_TYPE     eTargetType     = ttNoTarget;
    KSceneObject*   pSceneObject    = NULL;
    DWORD           dwTargetID      = 0;

    KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 0);

    bRetCode = m_SkillTarget.GetTarget(&pSceneObject);
    KG_PROCESS_ERROR(bRetCode);

    KG_PROCESS_ERROR(pSceneObject && pSceneObject->m_pScene);

    eTargetType = m_SkillTarget.GetTargetType();
    dwTargetID  = pSceneObject->m_dwID;

Exit0:
    lua_pushinteger(L, eTargetType);
    lua_pushinteger(L, dwTargetID);
    return 2;
}

int KCharacter::LuaCastSkillXYZ(Lua_State* L)
{
    int         nResult         = srcFailed;
	int         nRetCode		= false;
    int         nParamCount		= 0;
	DWORD       dwSkillID		= 0;
	DWORD       dwSkillLevel	= 0;
	KTarget		Target;
    KSkill*     pSkill          = NULL;
    int         nX              = 0;
    int         nY              = 0;
    int         nZ              = 0;
    KSKILL_RECIPE_KEY   RecipeKey;

    nParamCount = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nParamCount == 5);

	dwSkillID    = (DWORD)Lua_ValueToNumber(L, 1);
	dwSkillLevel = (DWORD)Lua_ValueToNumber(L, 2);
	nX           = (int)Lua_ValueToNumber(L, 3);
	nY           = (int)Lua_ValueToNumber(L, 4);
    nZ           = (int)Lua_ValueToNumber(L, 5);

    KGLOG_PROCESS_ERROR(nX >= 0);
    KGLOG_PROCESS_ERROR(nY >= 0);

	Target.SetTarget(nX, nY, nZ);

    nRetCode = g_pSO3World->m_SkillManager.GetSkillRecipeKey(&RecipeKey, dwSkillID, dwSkillLevel, this);
    KGLOG_PROCESS_ERROR(nRetCode);

    pSkill = g_pSO3World->m_SkillManager.GetSkill(RecipeKey);
    KGLOG_PROCESS_ERROR(pSkill);

//     pSkill = g_pSO3World->m_SkillManager.GetSkill_S(dwSkillID, dwSkillLevel);
//     KGLOG_PROCESS_ERROR(pSkill);

    nRetCode = pSkill->CheckTargetRange(this, Target);
    KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);
    
    // ----------------- 检查自身条件 ---------------------------

    nRetCode = pSkill->CheckSelfState(this);
    KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);

    if (IS_PLAYER(m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)this;
        nRetCode = pSkill->CheckWeaponRequest(pPlayer);
        KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);

        nRetCode = pSkill->CheckMountRequest(pPlayer);
        KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);

        nRetCode = pSkill->CheckCoolDown(pPlayer);
        KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);
    }
    //else: Npc无需检查

    // 检查当前是否处于其他OTAction状态
    KG_PROCESS_ERROR_RET_CODE(
        m_OTActionParam.eActionType == otActionIdle ||
        m_OTActionParam.eActionType == otActionSkillChannel, 
        srcInOTAction
    );

    nRetCode = pSkill->CheckCost(this);
    KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);

    // -----------------------------------------------------------    

#if defined(_SERVER)
	nRetCode = CastSkill(dwSkillID, dwSkillLevel, Target);
	KG_PROCESS_ERROR(nRetCode);
#endif

#if defined(_CLIENT)
	//发送指令给服务器
	nRetCode = g_PlayerClient.DoCharacterSkill(dwSkillID, dwSkillLevel, m_nFaceDirection, &Target);
	KGLOG_PROCESS_ERROR(nRetCode);
#endif	//_CLIENT

    nResult = srcSuccess;
Exit0:
#ifdef _CLIENT
    if (nResult != srcSuccess)
    {
        if (g_pGameWorldUIHandler)
        {
            KUIEventSkillCastRespond UINotify;
            UINotify.nRespondCode = nResult;
            g_pGameWorldUIHandler->OnSkillCastRespond(UINotify);

            KUIEventSkillCastRespondLog UIRespondLog;
            UIRespondLog.dwCaster   = m_dwID;
            UIRespondLog.dwSkillID  = dwSkillID;
            UIRespondLog.dwLevel    = dwSkillLevel;
            UIRespondLog.nRespond   = nResult;
            g_pGameWorldUIHandler->OnSkillCastRespondLog(UIRespondLog);
        }
    }
#endif
    Lua_PushNumber(L, nResult);
	return 1;
}

int KCharacter::LuaCastSkill(Lua_State* L)
{
    int         nResult         = srcFailed;
	int         nRetCode		= false;
    int         nParamCount		= 0;
	DWORD       dwSkillID		= 0;
	DWORD       dwSkillLevel	= 0;
    TARGET_TYPE eTargetType     = ttInvalid;
    DWORD       dwTargetID      = 0;
	KTarget		Target;
    KSkill*     pSkill          = NULL;
    KSKILL_RECIPE_KEY   RecipeKey;

    nParamCount = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nParamCount == 2 || nParamCount == 4);

	dwSkillID = (DWORD)Lua_ValueToNumber(L, 1);
	dwSkillLevel = (DWORD)Lua_ValueToNumber(L, 2);

    if (nParamCount == 4)
    {
        eTargetType = (TARGET_TYPE)(int)Lua_ValueToNumber(L, 3);
        dwTargetID  = (DWORD)Lua_ValueToNumber(L, 4);

        nRetCode = Target.SetTarget(eTargetType, dwTargetID);
        KG_PROCESS_ERROR_RET_CODE(nRetCode, srcNoTarget);
    }

    nRetCode = g_pSO3World->m_SkillManager.GetSkillRecipeKey(&RecipeKey, dwSkillID, dwSkillLevel, this);
    KGLOG_PROCESS_ERROR(nRetCode);

    pSkill = g_pSO3World->m_SkillManager.GetSkill(RecipeKey);
    KGLOG_PROCESS_ERROR(pSkill);

//     pSkill = g_pSO3World->m_SkillManager.GetSkill_S(dwSkillID, dwSkillLevel);
//     KGLOG_PROCESS_ERROR(pSkill);
    
    // ----------------- 检查自身条件 ---------------------------

    nRetCode = pSkill->CheckSelfState(this);
    KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);

    if (IS_PLAYER(m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)this;
        nRetCode = pSkill->CheckWeaponRequest(pPlayer);
        KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);
        nRetCode = pSkill->CheckCoolDown(pPlayer);
        KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);
        nRetCode = pSkill->CheckMountRequest(pPlayer);
        KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);
    }
    //else: Npc无需检查

    nRetCode = pSkill->CheckCost(this);
    KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);

    // 检查当前是否处于其他OTAction状态
    KG_PROCESS_ERROR_RET_CODE(
        m_OTActionParam.eActionType == otActionIdle ||
        m_OTActionParam.eActionType == otActionSkillChannel ||
        m_OTActionParam.eActionType == otActionPicking, 
        srcInOTAction
    );

    // -------------- 检查目标 -----------------------

    switch (pSkill->m_pBaseInfo->nCastMode)
    {
    case scmSector:
    case scmCasterArea:
    case scmCasterSingle:
    case scmTargetLeader:
    case scmTeamArea:
        nRetCode = Target.SetTarget((KCharacter*)this);
        KG_PROCESS_ERROR(nRetCode);
        break;
    case scmTargetSingle:
    case scmTargetArea:
    case scmTargetChain:
        if (nParamCount == 2)
        {
            eTargetType = m_SelectTarget.GetTargetType();
            if (eTargetType == ttNoTarget)
            {
                if (pSkill->m_pBaseInfo->bAutoSelectTarget)
                {
                    nRetCode = pSkill->AutoSelectTarget(this);
                    KG_PROCESS_ERROR_RET_CODE(nRetCode, srcNoTarget);
                }
                else
                {
                    KG_PROCESS_ERROR_RET_CODE(false, srcNoTarget);
                }
            }  
            Target = m_SelectTarget;
        }
        break;

    case scmPointArea:
    case scmPoint:
        // 这两种技能不会通过这里施放
        goto Exit0;
		break;
    default:
        goto Exit0;
        break;
    }


    if (
        pSkill->m_pBaseInfo->nCastMode == scmTargetSingle ||
        pSkill->m_pBaseInfo->nCastMode == scmTargetArea   ||
        pSkill->m_pBaseInfo->nCastMode == scmTargetChain
    )
    {
        nRetCode = pSkill->CheckTargetType(this, Target);
        KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);

        nRetCode = pSkill->CheckTargetState(Target);
        KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);

        nRetCode = pSkill->CheckTargetRange(this, Target);
        KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);
    }

#if defined(_SERVER)
	nRetCode = CastSkill(dwSkillID, dwSkillLevel, Target);
	KG_PROCESS_ERROR(nRetCode);
#endif

#if defined(_CLIENT)
	//发送指令给服务器
	nRetCode = g_PlayerClient.DoCharacterSkill(dwSkillID, dwSkillLevel, m_nFaceDirection, &Target);
	KGLOG_PROCESS_ERROR(nRetCode);
#endif	//_CLIENT

Exit1:
    nResult = srcSuccess;
Exit0:

#ifdef _CLIENT
    if (nResult != srcSuccess)
    {
        if (g_pGameWorldUIHandler)
        {
            KUIEventSkillCastRespond     UINotify;
            UINotify.nRespondCode = nResult;
            g_pGameWorldUIHandler->OnSkillCastRespond(UINotify);

            KUIEventSkillCastRespondLog UIRespondLog;
            UIRespondLog.dwCaster   = m_dwID;
            UIRespondLog.dwSkillID  = dwSkillID;
            UIRespondLog.dwLevel    = dwSkillLevel;
            UIRespondLog.nRespond   = nResult;
            g_pGameWorldUIHandler->OnSkillCastRespondLog(UIRespondLog);
        }
    }
#endif
    Lua_PushNumber(L, nResult);
	return 1;
}

int KCharacter::LuaAutoFly(Lua_State* L)
{
    int nResult     = 0;
    int nRetCode    = 0;
    int nFromNode   = 0;
    int nToCity     = 0;
    int nMiniCost   = INT_MAX;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 2);

    nFromNode   = (int)Lua_ValueToNumber(L, 1);
    nToCity     = (int)Lua_ValueToNumber(L, 2);

    if (IS_PLAYER(m_dwID))
    {
        KPlayer* pThis = (KPlayer*)this;
        nRetCode = g_pSO3World->m_RoadManager.IsReachable(
            &nMiniCost, nFromNode, nToCity, pThis->m_eCamp, pThis->m_dwTongID, 
            &(pThis->m_OpenRouteNodeList)
        );
        KG_PROCESS_ERROR(nRetCode == rtrcSuccess);
    }

    nRetCode = AutoFly(nFromNode, nToCity);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    Lua_PushNumber(L, nResult);
    Lua_PushNumber(L, nMiniCost);

    return 2;
}

int KCharacter::LuaStop(Lua_State* L)
{
	Stop();
	return 0;
}

void KCharacter::HostStand()
{
    int     nRetCode        = false;
    int     nCurrentState   = m_eMoveState;

#ifdef _CLIENT
    KGLOG_PROCESS_ERROR(m_dwID == g_pSO3World->m_dwClientPlayerID);
#endif

    nRetCode = Stand(true);
    KG_PROCESS_ERROR(nRetCode);

#ifdef _CLIENT
    nRetCode = g_PlayerClient.DoCharacterStand((KPlayer*)this, nCurrentState);
    KGLOG_PROCESS_ERROR(nRetCode);
#endif

Exit0:
	return;
}

int KCharacter::LuaStand(Lua_State* L)
{
    int     nRetCode    = false;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 0);

	HostStand();

Exit0:
	return 0;
}

int KCharacter::LuaWalkTo(Lua_State* L)
{
	int     nRetCode    = false;
	int     nDestX      = 0;
	int     nDestY      = 0;

	nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 2);

 	nDestX = (int)Lua_ValueToNumber(L, 1);
	nDestY = (int)Lua_ValueToNumber(L, 2);

    KGLOG_PROCESS_ERROR(nDestX >= 0);
    KGLOG_PROCESS_ERROR(nDestY >= 0);
    KGLOG_PROCESS_ERROR(nDestX <= MAX_X_COORDINATE);
    KGLOG_PROCESS_ERROR(nDestY <= MAX_Y_COORDINATE);

	nRetCode = WalkTo(nDestX, nDestY, true);
	KG_PROCESS_ERROR(nRetCode);

Exit0:
	return 0;
}

int KCharacter::LuaRunTo(Lua_State* L)
{
	int nRetCode        = false;
	int nDestX          = 0;
	int nDestY          = 0;

    nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 2);

    nDestX = (int)Lua_ValueToNumber(L, 1);
    nDestY = (int)Lua_ValueToNumber(L, 2);

    KGLOG_PROCESS_ERROR(nDestX >= 0);
    KGLOG_PROCESS_ERROR(nDestY >= 0);
    KGLOG_PROCESS_ERROR(nDestX <= MAX_X_COORDINATE);
    KGLOG_PROCESS_ERROR(nDestY <= MAX_Y_COORDINATE);

	nRetCode = RunTo(nDestX, nDestY, true);
	KG_PROCESS_ERROR(nRetCode);

Exit0:
	return 0;
}

int KCharacter::LuaSwimTo(Lua_State* L)
{
	int     nRetCode    = false;
	int     nDestX      = 0;
	int     nDestY      = 0;
    int     nDestZ      = 0;

	nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 3);

	nDestX = (int)Lua_ValueToNumber(L, 1);
	nDestY = (int)Lua_ValueToNumber(L, 2);
    nDestZ = (int)Lua_ValueToNumber(L, 3);

    KGLOG_PROCESS_ERROR(nDestX >= 0);
    KGLOG_PROCESS_ERROR(nDestY >= 0);
    KGLOG_PROCESS_ERROR(nDestX <= MAX_X_COORDINATE);
    KGLOG_PROCESS_ERROR(nDestY <= MAX_Y_COORDINATE);

	nRetCode = SwimTo(nDestX, nDestY, true);
	KG_PROCESS_ERROR(nRetCode);

Exit0:
	return 0;
}


void KCharacter::HostJump(BOOL bStandJump, int nDirectionXY)
{
    int     nRetCode        = false;
    int     nCurrentState   = m_eMoveState;

#ifdef _CLIENT 
    KGLOG_PROCESS_ERROR(m_dwID == g_pSO3World->m_dwClientPlayerID);
#endif

    nDirectionXY = nDirectionXY % DIRECTION_COUNT;
    if (nDirectionXY < 0)
    {
        nDirectionXY += DIRECTION_COUNT;
    }

	nRetCode = Jump(bStandJump, nDirectionXY, true);
	KG_PROCESS_ERROR(nRetCode);

#ifdef _CLIENT
	nRetCode = g_PlayerClient.DoCharacterJump((KPlayer*)this, nCurrentState, bStandJump, nDirectionXY);
	KGLOG_PROCESS_ERROR(nRetCode);
#endif	//_CLIENT

	return;
Exit0:
	return;
}

int KCharacter::LuaJump(Lua_State* L)
{
    int     nRetCode        = false;
    BOOL    bStandJump      = false;
    int     nDirectionXY    = 0;

    nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 2);

    bStandJump      = (BOOL)Lua_ValueToBoolean(L, 1);
    nDirectionXY    = (int)Lua_ValueToNumber(L, 2);

	HostJump(bStandJump, nDirectionXY);
    
Exit0:
	return 0;
}

int KCharacter::LuaDash(Lua_State* L)
{
    int nRetCode        = 0;
    int nFrame          = 0;
    int nDirectionXY    = 0;
	int nVelocityXY     = 0;
    int nVelocityZ      = 0;

    nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 4);

	nFrame          = (int)Lua_ValueToNumber(L, 1);
	nDirectionXY    = (int)Lua_ValueToNumber(L, 2);
    nVelocityXY     = (int)Lua_ValueToNumber(L, 3);
    nVelocityZ      = (int)Lua_ValueToNumber(L, 4);

	nRetCode = Dash(nFrame, nDirectionXY, nVelocityXY, nVelocityZ);
	KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
	return 0;
}


int KCharacter::LuaSitDown(Lua_State* L)
{
	int     nRetCode        = false;
    int     nCurrentState   = m_eMoveState;

    nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 0);

	nRetCode = SitDown(true);
	KG_PROCESS_ERROR(nRetCode);

#ifdef _CLIENT
	nRetCode = g_PlayerClient.DoCharacterSit((KPlayer*)this, nCurrentState);
	KGLOG_PROCESS_ERROR(nRetCode);
#endif	

Exit0:
	return 0;
}


int KCharacter::LuaPatrol(Lua_State* L)
{

	return 0;
}

int KCharacter::LuaDie(Lua_State* L)
{
#ifdef _SERVER
	GoToHell();
#endif
	return 0;
}

void KCharacter::HostTurnTo(int nNewDirection)
{
	int nRetCode        = false;

#ifdef _CLIENT
    KGLOG_PROCESS_ERROR(m_dwID == g_pSO3World->m_dwClientPlayerID);
#endif

	nRetCode = TurnTo(nNewDirection, true);
	KG_PROCESS_ERROR(nRetCode);

#if defined(_CLIENT)
	g_PlayerClient.DoSyncDirection((KPlayer*)this);
#endif

Exit0:
	return;
}

int KCharacter::LuaTurnTo(Lua_State* L)
{
	int nRetCode        = false;
    int nNewDirection   = 0;

    nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 1);

	nNewDirection = (int)Lua_ValueToNumber(L, 1);

	HostTurnTo(nNewDirection);

Exit0:
	return 0;
}

void KCharacter::HostPitchTo(int nNewDirection)
{
	int nRetCode        = false;

#ifdef _CLIENT
    KGLOG_PROCESS_ERROR(m_dwID == g_pSO3World->m_dwClientPlayerID);
#endif

	nRetCode = PitchTo(nNewDirection, true);
	KG_PROCESS_ERROR(nRetCode);

#if defined(_CLIENT)
	g_PlayerClient.DoSyncDirection((KPlayer*)this);
#endif

Exit0:
	return;
}

int KCharacter::LuaPitchTo(Lua_State* L)
{
	int nRetCode        = false;
    int nNewDirection   = 0;

    nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 1);

	nNewDirection = (int)Lua_ValueToNumber(L, 1);

	HostPitchTo(nNewDirection);

    // 目前俯仰操作不需要专门同步,游泳指令会带上俯仰角

Exit0:
	return 0;
}

int KCharacter::LuaGetAIType(Lua_State* L)
{
    int nRetCode    = 0;
    int nAIType     = 0;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 0);

    nAIType = m_AIVM.GetAIType();
    KGLOG_PROCESS_ERROR(m_AIData.nAIType == nAIType);

    Lua_PushNumber(L, nAIType);
    return 1;
Exit0:
    return 0;
}

int KCharacter::LuaSetAIType(Lua_State* L)
{
    int nRetCode    = 0;
	int nAIType     = 0;

    nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 1);

	nAIType = (int)Lua_ValueToNumber(L, 1);
    
    m_AIData.nAIType = nAIType;
    m_AIVM.Setup(this, nAIType);

Exit0:
	return 0;
}

int KCharacter::LuaSetAIDebug(Lua_State* L)
{
    int nRetCode    = 0;
    int nDebugCount = 0;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    nDebugCount = (int)Lua_ValueToNumber(L, 1);

    m_AIVM.m_nDebugCount = nDebugCount;
    m_AIVM.DebugAICurrentStateInfo();

Exit0:
    return 0;
}

int KCharacter::LuaSetAIProtectCampMarkAndForceMark(Lua_State* L)
{
    int nRetCode                = 0;
    DWORD dwProtectCampMark     = 0;
    DWORD dwProtectForceMark    = 0;

    KGLOG_PROCESS_ERROR(IS_NPC(m_dwID));

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 2);

    dwProtectCampMark   = (DWORD)Lua_ValueToNumber(L, 1);
    dwProtectForceMark  = (DWORD)Lua_ValueToNumber(L, 2);

    m_AIData.dwProtectCampMark  = dwProtectCampMark;
    m_AIData.dwProtectForceMark = dwProtectForceMark;

Exit0:
    return 0;
}

int KCharacter::LuaDoAction(Lua_State* L)
{
    BOOL        bRetCode        = false;
    DWORD       dwCharacterID   = 0;
    DWORD       dwAction        = 0;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	dwCharacterID   = (DWORD)Lua_ValueToNumber(L, 1);
	dwAction        = (DWORD)Lua_ValueToNumber(L, 2);
    
    // PK惩罚：不能与NPC对话
    if (dwAction == DIALOG_ACTION_ID && IS_NPC(dwCharacterID) && IS_PLAYER(m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)(this);
        KG_PROCESS_ERROR(!pPlayer->m_bCannotDialogWithNPC);
    }

#ifdef _SERVER
	bRetCode = Action(dwCharacterID, dwAction);
	KG_PROCESS_ERROR(bRetCode);
#else
    if (dwAction == DIALOG_ACTION_ID && IS_NPC(dwCharacterID))
	{
		KNpc* pNpc = g_pSO3World->m_NpcSet.GetObj(dwCharacterID);
		KG_PROCESS_ERROR(pNpc);

		//有的Npc不能对话
		KG_PROCESS_ERROR(pNpc->m_bDialogFlag);
	}

	g_PlayerClient.DoCharacterAction(dwCharacterID, dwAction);
#endif

Exit0:
	return 0;
}

#ifdef _CLIENT
int KCharacter::LuaGetBuffList(Lua_State* L)
{
    return m_BuffList._LuaGetBuffList(L);
}
#endif

int KCharacter::LuaCancelBuff(Lua_State* L)
{
    BOOL bResult    = false;
    int  nRetCdoe   = 0;
    int  nIndex     = 0;

    nRetCdoe = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCdoe == 1);

    nIndex = (int)Lua_ValueToNumber(L, 1);

#ifdef _SERVER
    nRetCdoe = m_BuffList.CancelBuff(nIndex);
    KG_PROCESS_ERROR(bResult);
#else // _CLIENT
    g_PlayerClient.DoCancelBuff(nIndex);
#endif // _SERVER

    bResult = true;
Exit0:
    lua_pushboolean(L, bResult);
    return 1;
}

int KCharacter::LuaSetSheath(Lua_State* L)
{
    int     nRetCode = 0;
    BOOL    bSheath  = false;

    nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 1);

	bSheath = (BOOL)Lua_ValueToNumber(L, 1);

#ifdef _CLIENT
    nRetCode = g_PlayerClient.DoSetSheathState(bSheath);
    KGLOG_CHECK_ERROR(nRetCode);
#endif

#ifdef _SERVER
    m_bSheathFlag = bSheath;
    g_PlayerServer.DoSyncSheathState(this);
#endif

Exit0:
    return 0;
}

int KCharacter::LuaGetMapID(Lua_State* L)
{
	BOOL bRetCode = false;
    DWORD dwMapID = 0;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 0);
	KG_PROCESS_ERROR(m_pScene);

	dwMapID = m_pScene->m_dwMapID;
	
	Lua_PushNumber(L, dwMapID);
	return 1;
Exit0:
	return 0;
}

int KCharacter::LuaCanDialog(Lua_State* L)
{
    BOOL        bResult  = false;
	BOOL        bRetCode = false;
    KPlayer*    pPlayer  = NULL;
    Luna<KPlayer>::KObjData* pObj = NULL;
    
	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	pObj = (Luna<KPlayer>::KObjData*)Lua_ValueToUserData(L, 1);
	KG_PROCESS_ERROR(pObj);

	pPlayer = pObj->pObj;
	KG_PROCESS_ERROR(pPlayer);

	bRetCode = CanDialog(pPlayer);
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

#ifdef _SERVER
int KCharacter::LuaAddBuff(Lua_State* L)
{
    BOOL                bResult             = false;
    int                 nTopIndex           = false;
    DWORD               dwCasterID          = 0;
    int                 nCasterLevel        = 0;
    DWORD               dwBuffID            = 0;
    int                 nBuffLevel          = 0;
    int                 nCount              = 0;
    int                 nFristActiveFrame   = 0;
    KBUFF_RECIPE_KEY    BuffRecipeKey;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex >= 4);

    dwCasterID      = (DWORD)Lua_ValueToNumber(L, 1);
    nCasterLevel    = (int)Lua_ValueToNumber(L, 2);
    dwBuffID        = (DWORD)Lua_ValueToNumber(L, 3);
    nBuffLevel      = (int)Lua_ValueToNumber(L, 4);

    if (nTopIndex >= 5)
    {
        nCount = (int)Lua_ValueToNumber(L, 5);
        KGLOG_PROCESS_ERROR(nCount >= 0);
    }

    if (nTopIndex >= 6)
    {
        nFristActiveFrame = (int)Lua_ValueToNumber(L, 6);
    }

    memset(&BuffRecipeKey, 0, sizeof(BuffRecipeKey));

    BuffRecipeKey.dwID      = dwBuffID;
    BuffRecipeKey.nLevel    = nBuffLevel;

    m_BuffList.CallBuff(dwCasterID, nCasterLevel, BuffRecipeKey, nCount, nFristActiveFrame);

    bResult = true;
Exit0:
    lua_pushboolean(L, bResult);
    return 1;
}

int KCharacter::LuaGetBuff(Lua_State* L)
{
    int                 nResult     = 0;
    int                 nTopIndex   = 0;
    DWORD               dwBuffID    = 0;
    int                 nBuffLevel  = 0;
    KBUFF_LIST_NODE*    pBuffInfo   = NULL;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2);

    dwBuffID    = (DWORD)Lua_ValueToNumber(L, 1);
    nBuffLevel  = (int)Lua_ValueToNumber(L, 2);

    pBuffInfo = m_BuffList.GetBuff(dwBuffID, nBuffLevel);
    KG_PROCESS_ERROR(pBuffInfo);

    nResult = pBuffInfo->LuaGetObj(L);
Exit0:
    return nResult;
}

int KCharacter::LuaDelBuff(Lua_State* L)
{
    BOOL    bResult     = false;
    int     nRetCode    = 0;
    DWORD   dwBuffID    = 0;
    int     nBuffLevel  = 0;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 2);

    dwBuffID    = (DWORD)Lua_ValueToNumber(L, 1);
    nBuffLevel  = (int)Lua_ValueToNumber(L, 2);

    nRetCode = m_BuffList.DelSingleBuff(dwBuffID, nBuffLevel);
    KG_PROCESS_ERROR(nRetCode);
    
    bResult = true;
Exit0:
    lua_pushboolean(L, bResult);
    return 1;
}

int KCharacter::LuaDelGroupBuff(Lua_State* L)
{
    BOOL    bResult     = false;
    int     nRetCode    = 0;
    DWORD   dwBuffID    = 0;
    int     nBuffLevel  = 0;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 2);

    dwBuffID    = (DWORD)Lua_ValueToNumber(L, 1);
    nBuffLevel  = (int)Lua_ValueToNumber(L, 2);

    nRetCode = m_BuffList.DelSingleGroupBuff(dwBuffID, nBuffLevel);
    KG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    lua_pushboolean(L, bResult);
    return 1;
}

int KCharacter::LuaDelMultiGroupBuffByFunctionType(Lua_State* L)
{
    BOOL    bResult         = false;
    int     nRetCode        = 0;
    DWORD   dwFunctionType  = 0;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwFunctionType    = (DWORD)Lua_ValueToNumber(L, 1);

    nRetCode = m_BuffList.DelMultiBuffByFunctionType(dwFunctionType, INT_MAX); // 任意强度都要清除。
    KG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    lua_pushboolean(L, bResult);
    return 1;
}

int KCharacter::LuaSetBuffNextActiveFrame(Lua_State* L)
{
    BOOL bResult          = false;
    int  nTopIndex        = 0;
    int  nBuffIndex       = 0;
    int  nNextActiveFrame = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2);

    nBuffIndex = (int)Lua_ValueToNumber(L, 1);
    nNextActiveFrame = (int)Lua_ValueToNumber(L, 2);

    m_BuffList.SetBuffNextActiveFrame(nBuffIndex, nNextActiveFrame);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);

    return 1;
}

int KCharacter::LuaSetBuffLeftActiveCount(Lua_State* L)
{
    BOOL bResult            = false;
    int  nTopIndex          = 0;
    int  nBuffIndex         = 0;
    int  nLeftActiveCount   = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2);

    nBuffIndex = (int)Lua_ValueToNumber(L, 1);
    nLeftActiveCount = (int)Lua_ValueToNumber(L, 2);
    KGLOG_PROCESS_ERROR(nLeftActiveCount > 0);

    m_BuffList.SetBuffLeftActiveCount(nBuffIndex, nLeftActiveCount);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);

    return 1;
}

#endif //_SERVER

#if defined(_SERVER)
int KCharacter::LuaClearAllThreat(Lua_State* L)
{
	//清理Character仇恨的脚本接口函数,不只是清除仇恨表,还清除被仇恨表
    BOOL    bResult         = false;
	BOOL    bRetCode        = false;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 0);

	bRetCode = m_SimpThreatList.ClearAllThreat();
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = m_SimpThreatList.ClearAllThreaten();
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KCharacter::LuaSetForceID(Lua_State* L)
{
	BOOL    bResult         = false;
    BOOL    bRetCode        = false;
    DWORD   dwForceID       = 0;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	dwForceID = (DWORD)Lua_ValueToNumber(L, 1);

	bRetCode = SetForceID(dwForceID);
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KCharacter::LuaSetPosition(Lua_State* L)
{
	BOOL    bRetCode    = false;
	int     nParamCount = 0;
	int     nX          = 0;
	int     nY          = 0;
	int     nZ          = 0;
    int     nMaxX       = 0;
    int     nMaxY       = 0;

    nParamCount = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nParamCount == 3);

	KGLOG_PROCESS_ERROR(m_pScene && m_pRegion);

	nX      = (int)Lua_ValueToNumber(L, 1);
	nY      = (int)Lua_ValueToNumber(L, 2);
	nZ      = (int)Lua_ValueToNumber(L, 3);

    nMaxX = m_pScene->GetRegionWidth() * REGION_GRID_WIDTH * CELL_LENGTH - 1;
    nMaxY = m_pScene->GetRegionHeight() * REGION_GRID_HEIGHT * CELL_LENGTH - 1;

    KGLOG_PROCESS_ERROR(nX >= 0 && nX <= nMaxX);
    KGLOG_PROCESS_ERROR(nY >= 0 && nY <= nMaxY);
    KGLOG_PROCESS_ERROR(nZ >= 0 && nZ <= MAX_Z_POINT);

	bRetCode = MoveTo(nX, nY, nZ, true, true);
	KGLOG_PROCESS_ERROR(bRetCode);

    if (IS_PLAYER(m_dwID))
    {
        ((KPlayer*)this)->m_nRecordCount = 0;
    }

Exit0:
	return 0;
}
#endif //_SERVER

int KCharacter::LuaFireAIEvent(Lua_State* L)
{
	BOOL    bRetCode    = false;
	int     nParamCount = 0;
    int     nEventID    = 0;
    DWORD   dwEventSrc  = ERROR_ID;
    int     nEventParam = 0;

    nParamCount = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nParamCount == 3);

    nEventID = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nEventID >= KAI_USER_EVENT);

    dwEventSrc = (DWORD)Lua_ValueToNumber(L, 2);

    nEventParam = (int)Lua_ValueToNumber(L, 3);

    m_AIVM.FireEvent(nEventID, dwEventSrc, nEventParam);

Exit0:
    return 0;
}

#ifdef _SERVER
int KCharacter::LuaSetCustomInteger4(Lua_State* L)
{
    BOOL    bRetCode       = false;
    int     nParamCount    = Lua_GetTopIndex(L);
    int     nOffset        = 0;
    int64_t nInput         = 0;
    int32_t nValue         = 0;

    assert(m_pScene);

    KGLOG_PROCESS_ERROR(nParamCount == 2);
    
    nOffset = (int)Lua_ValueToNumber(L, 1);
    nInput  = (int64_t)Lua_ValueToNumber(L, 2);
    
    KGLOG_PROCESS_ERROR(nInput >= INT_MIN && nInput <= INT_MAX);
    
    nValue = (int32_t)nInput;

    if (IS_PLAYER(m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)(this);

        bRetCode = pPlayer->m_CustomData.SetValue(nOffset, nValue);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    else
    {   
        bRetCode = m_pScene->m_NpcCustomDataTable[m_dwID].SetValue(nOffset, nValue);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

Exit0:
    return 0;
}

int KCharacter::LuaGetCustomInteger4(Lua_State* L)
{
    int     nResult        = 0;
    BOOL    bRetCode       = false;
    int     nParamCount    = Lua_GetTopIndex(L);
    int     nOffset        = 0;
    int32_t nValue         = 0;

    assert(m_pScene);

    KGLOG_PROCESS_ERROR(nParamCount == 1);

    nOffset = (int)Lua_ValueToNumber(L, 1);
    
    if (IS_PLAYER(m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)(this);

        bRetCode = pPlayer->m_CustomData.GetValue(nOffset, &nValue);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    else
    {
        KOBJ_CUSTOM_DATA_TABLE::iterator it = m_pScene->m_NpcCustomDataTable.find(m_dwID);
        if (it == m_pScene->m_NpcCustomDataTable.end())
        {
            goto Exit1;
        }

        bRetCode = it->second.GetValue(nOffset, &nValue);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    
Exit1:
    Lua_PushNumber(L, nValue);

    nResult = 1;
Exit0:
    return nResult;
}

int KCharacter::LuaSetCustomInteger2(Lua_State* L)
{
    BOOL    bRetCode       = false;
    int     nParamCount    = Lua_GetTopIndex(L);
    int     nOffset        = 0;
    int64_t nInput         = 0;
    int16_t nValue         = 0;

    assert(m_pScene);

    KGLOG_PROCESS_ERROR(nParamCount == 2);

    nOffset = (int)Lua_ValueToNumber(L, 1);
    nInput  = (int64_t)Lua_ValueToNumber(L, 2);
    
    KGLOG_PROCESS_ERROR(nInput >= SHRT_MIN && nInput <= SHRT_MAX);
    
    nValue = (int16_t)nInput;

    if (IS_PLAYER(m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)(this);

        bRetCode = pPlayer->m_CustomData.SetValue(nOffset, nValue);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    else
    {   
        bRetCode = m_pScene->m_NpcCustomDataTable[m_dwID].SetValue(nOffset, nValue);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

Exit0:
    return 0;
}

int KCharacter::LuaGetCustomInteger2(Lua_State* L)
{   
    int     nResult        = 0;
    BOOL    bRetCode       = false;
    int     nParamCount    = Lua_GetTopIndex(L);
    int     nOffset        = 0;
    int16_t nValue         = 0;

    assert(m_pScene);

    KGLOG_PROCESS_ERROR(nParamCount == 1);

    nOffset = (int)Lua_ValueToNumber(L, 1);

    if (IS_PLAYER(m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)(this);
        assert(pPlayer);

        bRetCode = pPlayer->m_CustomData.GetValue(nOffset, &nValue);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    else
    {
        KOBJ_CUSTOM_DATA_TABLE::iterator it = m_pScene->m_NpcCustomDataTable.find(m_dwID);
        if (it == m_pScene->m_NpcCustomDataTable.end())
        {
            goto Exit1;
        }

        bRetCode = it->second.GetValue(nOffset, &nValue);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

Exit1:
    Lua_PushNumber(L, nValue);

    nResult = 1;
Exit0:
    return nResult;
}

int KCharacter::LuaSetCustomInteger1(Lua_State* L)
{
    BOOL     bRetCode       = false;
    int      nParamCount    = Lua_GetTopIndex(L);
    int      nOffset        = 0;
    int64_t  nInput         = 0;
    int8_t   nValue         = 0;

    assert(m_pScene);

    KGLOG_PROCESS_ERROR(nParamCount == 2);

    nOffset = (int)Lua_ValueToNumber(L, 1);
    nInput  = (int64_t)Lua_ValueToNumber(L, 2);
    
    KGLOG_PROCESS_ERROR(nInput >= SCHAR_MIN && nInput <= SCHAR_MAX);
    
    nValue = (int8_t)nInput;

    if (IS_PLAYER(m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)(this);

        bRetCode = pPlayer->m_CustomData.SetValue(nOffset, nValue);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    else
    {   
        bRetCode = m_pScene->m_NpcCustomDataTable[m_dwID].SetValue(nOffset, nValue);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

Exit0:
    return 0;
}

int KCharacter::LuaGetCustomInteger1(Lua_State* L)
{
    int      nResult        = 0;
    BOOL     bRetCode       = false;
    int      nParamCount    = Lua_GetTopIndex(L);
    int      nOffset        = 0;
    int8_t   nValue         = 0;

    assert(m_pScene);

    KGLOG_PROCESS_ERROR(nParamCount == 1);

    nOffset = (int)Lua_ValueToNumber(L, 1);

    if (IS_PLAYER(m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)(this);

        bRetCode = pPlayer->m_CustomData.GetValue(nOffset, &nValue);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    else
    {
        KOBJ_CUSTOM_DATA_TABLE::iterator it = m_pScene->m_NpcCustomDataTable.find(m_dwID);
        if (it == m_pScene->m_NpcCustomDataTable.end())
        {
            goto Exit1;
        }

        bRetCode = it->second.GetValue(nOffset, &nValue);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

Exit1:
    Lua_PushNumber(L, nValue);

    nResult = 1;
Exit0:
    return nResult;
}

int KCharacter::LuaSetCustomUnsigned4(Lua_State* L)
{
    BOOL     bRetCode       = false;
    int      nParamCount    = Lua_GetTopIndex(L);
    int      nOffset        = 0;
    int32_t  uValue         = 0;
    int64_t  nInput         = 0;

    assert(m_pScene);

    KGLOG_PROCESS_ERROR(nParamCount == 2);

    nOffset = (int)Lua_ValueToNumber(L, 1);
    nInput  = (int64_t)Lua_ValueToNumber(L, 2);
    
    KGLOG_PROCESS_ERROR(nInput >= 0 && nInput <= UINT_MAX);
    
    uValue = (int32_t)nInput;

    if (IS_PLAYER(m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)(this);

        bRetCode = pPlayer->m_CustomData.SetValue(nOffset, uValue);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    else
    {   
        bRetCode = m_pScene->m_NpcCustomDataTable[m_dwID].SetValue(nOffset, uValue);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

Exit0:
    return 0;
}

int KCharacter::LuaGetCustomUnsigned4(Lua_State* L)
{
    int     nResult        = 0;
    BOOL    bRetCode       = false;
    int     nParamCount    = Lua_GetTopIndex(L);
    int     nOffset        = 0;
    int32_t uValue         = 0;

    assert(m_pScene);

    KGLOG_PROCESS_ERROR(nParamCount == 1);

    nOffset = (int)Lua_ValueToNumber(L, 1);

    if (IS_PLAYER(m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)(this);

        bRetCode = pPlayer->m_CustomData.GetValue(nOffset, &uValue);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    else
    {
        KOBJ_CUSTOM_DATA_TABLE::iterator it = m_pScene->m_NpcCustomDataTable.find(m_dwID);
        if (it == m_pScene->m_NpcCustomDataTable.end())
        {
            goto Exit1;
        }

        bRetCode = it->second.GetValue(nOffset, &uValue);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

Exit1:
    Lua_PushNumber(L, uValue);

    nResult = 1;
Exit0:
    return nResult;
}

int KCharacter::LuaSetCustomUnsigned2(Lua_State* L)
{
    BOOL     bRetCode       = false;
    int      nParamCount    = Lua_GetTopIndex(L);
    int      nOffset        = 0;
    uint16_t uValue         = 0;
    int64_t    nInput         = 0;

    KGLOG_PROCESS_ERROR(nParamCount == 2);

    nOffset = (int)Lua_ValueToNumber(L, 1);
    nInput  = (int64_t)Lua_ValueToNumber(L, 2);
    
    KGLOG_PROCESS_ERROR(nInput >= 0 && nInput <= USHRT_MAX);
    
    uValue = (uint16_t)nInput;

    if (IS_PLAYER(m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)(this);
        assert(pPlayer);

        bRetCode = pPlayer->m_CustomData.SetValue(nOffset, uValue);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    else
    {   
        assert(m_pScene);

        bRetCode = m_pScene->m_NpcCustomDataTable[m_dwID].SetValue(nOffset, uValue);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

Exit0:
    return 0;
}

int KCharacter::LuaGetCustomUnsigned2(Lua_State* L)
{
    int     nResult        = 0;
    BOOL    bRetCode       = false;
    int     nParamCount    = Lua_GetTopIndex(L);
    int     nOffset        = 0;
   uint16_t  uValue         = 0;

    assert(m_pScene);

    KGLOG_PROCESS_ERROR(nParamCount == 1);

    nOffset = (int)Lua_ValueToNumber(L, 1);

    if (IS_PLAYER(m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)(this);

        bRetCode = pPlayer->m_CustomData.GetValue(nOffset, &uValue);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    else
    {
        KOBJ_CUSTOM_DATA_TABLE::iterator it = m_pScene->m_NpcCustomDataTable.find(m_dwID);
        if (it == m_pScene->m_NpcCustomDataTable.end())
        {
            goto Exit1;
        }

        bRetCode = it->second.GetValue(nOffset, &uValue);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

Exit1:
    Lua_PushNumber(L, uValue);

    nResult = 1;
Exit0:
    return nResult;
}

int KCharacter::LuaSetCustomUnsigned1(Lua_State* L)
{
    BOOL      bRetCode       = false;
    int       nParamCount    = Lua_GetTopIndex(L);
    int       nOffset        = 0;
    uint8_t   uValue         = 0;
    int64_t   nInput         = 0;

    assert(m_pScene);

    KGLOG_PROCESS_ERROR(nParamCount == 2);

    nOffset = (int)Lua_ValueToNumber(L, 1);
    nInput  = (int64_t)Lua_ValueToNumber(L, 2);
        
    KGLOG_PROCESS_ERROR(nInput >= 0 && nInput <= UCHAR_MAX);

    uValue = (uint8_t)nInput;

    if (IS_PLAYER(m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)(this);

        bRetCode = pPlayer->m_CustomData.SetValue(nOffset, uValue);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    else
    {   
        bRetCode = m_pScene->m_NpcCustomDataTable[m_dwID].SetValue(nOffset, uValue);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

Exit0:
    return 0;
}

int KCharacter::LuaGetCustomUnsigned1(Lua_State* L)
{
    int     nResult        = 0;
    BOOL    bRetCode       = false;
    int     nParamCount    = Lua_GetTopIndex(L);
    int     nOffset        = 0;
    int8_t  uValue         = 0;

    assert(m_pScene);

    KGLOG_PROCESS_ERROR(nParamCount == 1);

    nOffset = (int)Lua_ValueToNumber(L, 1);

    if (IS_PLAYER(m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)(this);

        bRetCode = pPlayer->m_CustomData.GetValue(nOffset, &uValue);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    else
    {
        KOBJ_CUSTOM_DATA_TABLE::iterator it = m_pScene->m_NpcCustomDataTable.find(m_dwID);
        if (it == m_pScene->m_NpcCustomDataTable.end())
        {
            goto Exit1;
        }

        bRetCode = it->second.GetValue(nOffset, &uValue);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

Exit1:
    Lua_PushNumber(L, uValue);

    nResult = 1;
Exit0:
    return nResult;
}

int KCharacter::LuaSetCustomBoolean(Lua_State* L)
{
    BOOL     bRetCode       = false;
    int      nParamCount    = Lua_GetTopIndex(L);
    int      nOffset        = 0;
    BOOL     bInput         = false;

    assert(m_pScene);

    KGLOG_PROCESS_ERROR(nParamCount == 2);

    nOffset = (int)Lua_ValueToNumber(L, 1);
    bInput  = (BOOL)Lua_ValueToBoolean(L, 2);

    if (IS_PLAYER(m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)(this);

        bRetCode = pPlayer->m_CustomData.SetValue(nOffset, (int8_t)bInput);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    else
    {   
        bRetCode = m_pScene->m_NpcCustomDataTable[m_dwID].SetValue(nOffset, (int8_t)bInput);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

Exit0:
    return 0;
}

int KCharacter::LuaGetCustomBoolean(Lua_State* L)
{
    int     nResult        = 0;
    BOOL    bRetCode       = false;
    int     nParamCount    = Lua_GetTopIndex(L);
    int     nOffset        = 0;
    BOOL    bValue         = false;

    assert(m_pScene);

    KGLOG_PROCESS_ERROR(nParamCount == 1);

    nOffset = (int)Lua_ValueToNumber(L, 1);

    if (IS_PLAYER(m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)(this);

        bRetCode = pPlayer->m_CustomData.GetValue(nOffset, (int8_t*)&bValue);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    else
    {
        KOBJ_CUSTOM_DATA_TABLE::iterator it = m_pScene->m_NpcCustomDataTable.find(m_dwID);
        if (it == m_pScene->m_NpcCustomDataTable.end())
        {
            goto Exit1;
        }

        bRetCode = it->second.GetValue(nOffset, (int8_t*)&bValue);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

Exit1:
    Lua_PushBoolean(L, bValue);

    nResult = 1;
Exit0:
    return nResult;
}

int KCharacter::LuaSetCustomBitValue(Lua_State* L)
{
    BOOL    bRetCode       = false;
    int     nParamCount    = Lua_GetTopIndex(L);
    int     nByteOffset    = 0;
    int     nBitOffset     = 0;
    BOOL    bValue         = false;

    KGLOG_PROCESS_ERROR(nParamCount == 3);

    nByteOffset = (int)Lua_ValueToNumber(L, 1);
    nBitOffset  = (int)Lua_ValueToNumber(L, 2);
    bValue      = (BOOL)Lua_ValueToBoolean(L, 3);
        
    if (IS_PLAYER(m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)(this);

        bRetCode = pPlayer->m_CustomData.SetBit(nByteOffset * CHAR_BIT + nBitOffset, bValue);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    else
    {   
        assert(m_pScene);

        bRetCode = m_pScene->m_NpcCustomDataTable[m_dwID].SetBit(nByteOffset * CHAR_BIT + nBitOffset, bValue);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

Exit0:
    return 0;
}

int KCharacter::LuaGetCustomBitValue(Lua_State* L)
{
    int     nResult        = 0;
    BOOL    bRetCode       = false;
    int     nParamCount    = Lua_GetTopIndex(L);
    int     nByteOffset    = 0;
    int     nBitOffset     = 0;
    BOOL    bValue         = false;

    assert(m_pScene);

    KGLOG_PROCESS_ERROR(nParamCount == 2);

    nByteOffset = (int)Lua_ValueToNumber(L, 1);
    nBitOffset  = (int)Lua_ValueToNumber(L, 2);

    if (IS_PLAYER(m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)(this);

        bRetCode = pPlayer->m_CustomData.GetBit(nByteOffset * CHAR_BIT + nBitOffset, &bValue);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    else
    {
        KOBJ_CUSTOM_DATA_TABLE::iterator it = m_pScene->m_NpcCustomDataTable.find(m_dwID);
        if (it == m_pScene->m_NpcCustomDataTable.end())
        {
            goto Exit1;
        }

        bRetCode = it->second.GetBit(nByteOffset * CHAR_BIT + nBitOffset, &bValue);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

Exit1:
    Lua_PushBoolean(L, bValue);

    nResult = 1;
Exit0:
    return nResult;
}

int KCharacter::LuaSetCustomString(Lua_State* L)
{
    BOOL            bRetCode       = false;
    int             nParamCount    = Lua_GetTopIndex(L);
    int             nOffset        = 0;
    const char*     pcszValue      = NULL;

    assert(m_pScene);

    KGLOG_PROCESS_ERROR(nParamCount == 2);

    nOffset     = (int)Lua_ValueToNumber(L, 1);
    pcszValue    = Lua_ValueToString(L, 2);

    KGLOG_PROCESS_ERROR(pcszValue);

    if (IS_PLAYER(m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)(this);

        bRetCode = pPlayer->m_CustomData.SetString(nOffset, pcszValue);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    else
    {   
        bRetCode = m_pScene->m_NpcCustomDataTable[m_dwID].SetString(nOffset, pcszValue);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

Exit0:
    return 0;
}

int KCharacter::LuaGetCustomString(Lua_State* L)
{
    int             nResult        = 0;
    BOOL            bRetCode       = false;
    int             nParamCount    = Lua_GetTopIndex(L);
    int             nOffset        = 0;
    const char*     pcszValue      = "";

    assert(m_pScene);

    KGLOG_PROCESS_ERROR(nParamCount == 1);

    nOffset = (int)Lua_ValueToNumber(L, 1);

    if (IS_PLAYER(m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)(this);

        pcszValue = pPlayer->m_CustomData.GetString(nOffset);
        KGLOG_PROCESS_ERROR(pcszValue);
    }
    else
    {
        KOBJ_CUSTOM_DATA_TABLE::iterator it = m_pScene->m_NpcCustomDataTable.find(m_dwID);
        if (it == m_pScene->m_NpcCustomDataTable.end())
        {
            goto Exit1;
        }

        pcszValue = it->second.GetString(nOffset);
        KGLOG_PROCESS_ERROR(pcszValue);
    }

Exit1:
    Lua_PushString(L, pcszValue);

    nResult = 1;
Exit0:
    return nResult;
}
#endif

int KCharacter::LuaGetSkillPrepareState(Lua_State* L)
{
    int     nResult         = 0;
    BOOL    bRetCode        = false;
    BOOL    bIsPrepare      = false;
    DWORD   dwSkillID       = 0;
    DWORD   dwSkillLevel    = 0;
	FLOAT	fProgress		= 0.0f;

    assert(m_pScene);

    if (m_OTActionParam.eActionType == otActionSkillPrepare)
    {
        KSKILL_RECIPE_KEY   RecipeKey = m_OTActionParam.Data.SkillRecipePointer.GetRecipeKey();

        bIsPrepare      = true;
        dwSkillID       = RecipeKey.dwSkillID;
        dwSkillLevel    = RecipeKey.dwSkillLevel;
		if (m_OTActionParam.nPrepareFrame > 0)
		{
			fProgress = (FLOAT)(g_pSO3World->m_nGameLoop - (m_OTActionParam.nEndFrame - m_OTActionParam.nPrepareFrame)) 
				/ m_OTActionParam.nPrepareFrame;
		}
		else
		{
			fProgress = 1.0f;
		}
    }

    Lua_PushBoolean(L, bIsPrepare);
    Lua_PushNumber(L, dwSkillID);
    Lua_PushNumber(L, dwSkillLevel);
    Lua_PushNumber(L, fProgress);

    nResult = 4;
Exit0:
    return nResult;
}

#ifdef _SERVER
int KCharacter::LuaTraceSelectTargetThreat(Lua_State* L)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    int                     nTopIndex       = 0;
    TARGET_TYPE             eTargetType     = ttNoTarget;
    KCharacter*             pTarget         = NULL;
    const KSimpThreatNode*  cpThreatNode    = NULL;
    char*                   pszName         = NULL;
    size_t                  uDataSize       = 0;
    char                    szDebugData[MAX_CHAT_TEXT_LEN];

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 0);

    szDebugData[0] = tdbText;
    
    eTargetType = m_SelectTarget.GetTargetType();
    KG_PROCESS_ERROR(eTargetType == ttPlayer || eTargetType == ttNpc);

    bRetCode = m_SelectTarget.GetTarget(&pTarget);
    KG_PROCESS_ERROR(bRetCode);
    KG_PROCESS_ERROR(pTarget);

    cpThreatNode = pTarget->m_SimpThreatList.GetFirstThreat(thtMainThreat);
    KG_PROCESS_ERROR(cpThreatNode);

    pszName = (char*)GetName();
    KGLOG_PROCESS_ERROR(pszName);

    uDataSize = snprintf(&szDebugData[1], sizeof(szDebugData), "[Threat]: Begin\n");
    KGLOG_PROCESS_ERROR(uDataSize >= 0 && uDataSize < sizeof(szDebugData));
    szDebugData[uDataSize] = '\0';

    bRetCode = g_PlayerServer.DoTalkMessage(trDebugThreat, ERROR_ID, NULL, m_dwID, pszName, uDataSize + 1, (BYTE*)szDebugData);
    KGLOG_PROCESS_ERROR(bRetCode);

    while (cpThreatNode)
    {
        KCharacter* pThreatCharacter = NULL;

        pThreatCharacter = cpThreatNode->pCharacter;
        uDataSize = snprintf(
            &szDebugData[1], sizeof(szDebugData),
            "[Threat]: %s<%lu> - ThreatValue:%d, IsTop:%d\n",
            pThreatCharacter->GetName(), pThreatCharacter->m_dwID,
            cpThreatNode->nThreat[thtMainThreat], cpThreatNode->bTopFlag
        );
        KGLOG_PROCESS_ERROR(uDataSize >= 0 && uDataSize < sizeof(szDebugData));;
        szDebugData[uDataSize] = '\0';

        bRetCode = g_PlayerServer.DoTalkMessage(trDebugThreat, ERROR_ID, NULL, m_dwID, pszName, uDataSize + 1, (BYTE*)szDebugData);
        KGLOG_PROCESS_ERROR(bRetCode);

        cpThreatNode = pTarget->m_SimpThreatList.GetNextThreat(thtMainThreat, cpThreatNode);
    }

    uDataSize = snprintf(&szDebugData[1], sizeof(szDebugData), "[Threat]: End\n");
    KGLOG_PROCESS_ERROR(uDataSize >= 0 && uDataSize < sizeof(szDebugData));;
    szDebugData[uDataSize] = '\0';

    bRetCode = g_PlayerServer.DoTalkMessage(trDebugThreat, ERROR_ID, NULL, m_dwID, pszName, uDataSize + 1, (BYTE*)szDebugData);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}
#endif

int KCharacter::LuaSetAIOriginPos(Lua_State* L)
{
    BOOL bResult = false;

    KGLOG_PROCESS_ERROR(m_pScene);

    m_AIData.nOriginPosX        = m_nX;
    m_AIData.nOriginPosY        = m_nY;
    m_AIData.nOriginPosZ        = m_nZ;
    m_AIData.nOriginDirection   = m_nFaceDirection;

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KCharacter::LuaGetAIOriginPos(Lua_State* L)
{
    KGLOG_PROCESS_ERROR(m_pScene);

    Lua_PushNumber(L, m_AIData.nOriginPosX);
    Lua_PushNumber(L, m_AIData.nOriginPosY);
    Lua_PushNumber(L, m_AIData.nOriginPosZ);
    Lua_PushNumber(L, m_AIData.nOriginDirection);

    return 4;
Exit0:
    return 0;
}

#ifdef _SERVER
int KCharacter::LuaStealth(Lua_State* L)
{
    int     nTopIndex   = 0;
    BOOL    bStealthy   = false;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    bStealthy = (BOOL)Lua_ValueToBoolean(L, 1);

    Stealth(bStealthy);

Exit0:
    return 0;
}
#endif
