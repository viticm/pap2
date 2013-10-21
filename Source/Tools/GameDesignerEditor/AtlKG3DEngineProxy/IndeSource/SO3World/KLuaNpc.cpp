#include "stdafx.h"
#include "KNpc.h"
#include "KScene.h"
#include "KPlayerServer.h"
#include "KPlayer.h"
#include "KSO3World.h"

#if defined(_SERVER)
int KNpc::LuaGetQuestString(Lua_State* L)
{
	char szTemp[1024];
	int nRealLen = 0;

	KGLOG_PROCESS_ERROR(m_pScene);

	nRealLen = g_pSO3World->m_Settings.m_QuestInfoList.GetNpcQuestString(
		m_pScene->m_dwMapID, m_dwTemplateID, szTemp, 1024);
	KG_PROCESS_ERROR(nRealLen > 0);

	Lua_PushString(L, szTemp);
	return 1;

Exit0:
	Lua_PushString(L, "");
	return 1;
}

int KNpc::LuaGetAutoDialogString(Lua_State* L)
{
	BOOL bRetCode = false;
	int nRealLen = 0;
	int nTopIndex = 0;
	DWORD dwPlayerID = 0;
	KPlayer* pPlayer = NULL;
	char szTemp[2048]; 

	nTopIndex = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nTopIndex == 1);
	KGLOG_PROCESS_ERROR(m_pScene);
	
	dwPlayerID = (DWORD)Lua_ValueToNumber(L, 1);
	pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

	bRetCode = GetAutoDialogString(pPlayer, szTemp, 2048);
	KGLOG_PROCESS_ERROR(bRetCode);

	Lua_PushString(L, szTemp);
	return 1;

Exit0:
	Lua_PushString(L, "");
	return 1;
}

int KNpc::LuaBindNpcShop(Lua_State* L)
{
    int     nRetCode  = 0;
    DWORD   dwShopTemplateID = 0;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwShopTemplateID = (DWORD)Lua_ValueToNumber(L, 1);
    nRetCode = g_pSO3World->m_ShopCenter.BindNpcShop(this, dwShopTemplateID);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
    return 0;
}

int KNpc::LuaUnbindNpcShop(Lua_State* L)
{
    BOOL    bRetCode    = false;

    bRetCode = g_pSO3World->m_ShopCenter.UnbindNpcShop(this);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return 0;
}

int KNpc::LuaGetShop(Lua_State *L)
{
    if (m_pShop)
    {
        return m_pShop->LuaGetObj(L);
    }
    else
    {
        Lua_PushNil(L);
        return 1;
    }
}

int	KNpc::LuaSetBank(Lua_State* L)
{
    BOOL    bResult         = false;
	int     nTopIndex       = 0;
	BOOL    bFlag           = false;

	nTopIndex = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nTopIndex == 1);

	bFlag = (BOOL)Lua_ValueToBoolean(L, 1);

	if (m_bBank != bFlag)
	{
		m_bBank = bFlag;
	}

	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KNpc::LuaSetScript(Lua_State *L)
{
    BOOL    bResult         = false;
	BOOL    bRetCode        = false;
	const char *pszTemp;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	pszTemp = Lua_ValueToString(L, 1);
	KGLOG_PROCESS_ERROR(pszTemp);
	
	bRetCode = SetScript(pszTemp);
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KNpc::LuaFollowTarget(Lua_State* L)
{
    BOOL        bResult  = false;
	BOOL        bRetCode = false;
	KCharacter* pTarget  = NULL;
    DWORD       dwID     = 0;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	dwID = (DWORD)Lua_ValueToNumber(L, 1);

	if (IS_PLAYER(dwID))
	{
		pTarget = g_pSO3World->m_PlayerSet.GetObj(dwID);
		KGLOG_PROCESS_ERROR(pTarget);
	}
	else
	{
		pTarget = g_pSO3World->m_NpcSet.GetObj(dwID);
		KGLOG_PROCESS_ERROR(pTarget);
	}

	// m_AIController.DoFollow(pTarget);

	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KNpc::LuaModifyThreat(Lua_State* L)
{
    BOOL        bResult         = false;
	BOOL        bRetCode        = false;
	KCharacter* pCharacter      = NULL;
    DWORD       dwCharacterID   = 0;
    int         nThreat         = 0;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	dwCharacterID = (DWORD)Lua_ValueToNumber(L, 1);
	nThreat = (int)Lua_ValueToNumber(L, 2);

	if (IS_PLAYER(dwCharacterID))
		pCharacter = g_pSO3World->m_PlayerSet.GetObj(dwCharacterID);
	else
		pCharacter = g_pSO3World->m_NpcSet.GetObj(dwCharacterID);
	
    KGLOG_PROCESS_ERROR(pCharacter);
    KG_PROCESS_ERROR(pCharacter->m_pScene);

	bRetCode = m_SimpThreatList.ModifyThreat(thtSpecialThreat, pCharacter, nThreat);
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KNpc::LuaSetPatrolPath(Lua_State* L)
{
    BOOL    bResult         = false;
	BOOL    bRetCode        = false;
    int     nPatrolPath     = 0;
    int     nNextPointIndex = 0;
    int     nTopIndex       = Lua_GetTopIndex(L); 
    int     nOrderIndex     = 0;

    KGLOG_PROCESS_ERROR(m_pScene);
	KGLOG_PROCESS_ERROR(nTopIndex == 2 || nTopIndex == 3);

	nPatrolPath = (int)Lua_ValueToNumber(L, 1);
	nNextPointIndex = (int)Lua_ValueToNumber(L, 2);
    if (nTopIndex == 3)
    {
        nOrderIndex = (int)Lua_ValueToNumber(L, 3);
        KGLOG_PROCESS_ERROR(nOrderIndex > 0);
    }


    if (m_AIData.nPatrolPathID)
    {
        m_pScene->m_AIPatrolGroupManager.LeaveGroup(m_AIData.nPatrolPathID, m_dwID);
    }

    m_AIData.nPatrolPathID = 0;
    m_AIData.pPatrolPath = NULL;
    m_AIData.nPatrolOrderIndex = 0;

	if (nPatrolPath)
	{
        KPatrolPath* pPatrolPath = g_pSO3World->m_Settings.m_PatrolPathList.GetPatrolPath(m_pScene->m_dwMapID, nPatrolPath);
        KGLOG_PROCESS_ERROR(pPatrolPath);

        if (nOrderIndex == 0)
            nOrderIndex = 1;

        m_AIData.nPatrolOrderIndex = nOrderIndex;
        m_AIData.nPatrolPathID = nPatrolPath;
        m_AIData.pPatrolPath = pPatrolPath;

        bRetCode = m_pScene->m_AIPatrolGroupManager.SetGroupPatrolPathIndex(nPatrolPath, nNextPointIndex);
        KGLOG_PROCESS_ERROR(bRetCode);

        m_pScene->m_AIPatrolGroupManager.JoinGroup(m_AIData.nPatrolPathID, m_dwID);

        if (m_pRegion)
            m_pRegion->SetActive();
	}

	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KNpc::LuaSetDisappearFrames(Lua_State* L)
{
	BOOL bRetCode        = false;
	int  nTopIndex       = 0;
    int  nFrames         = 0;
	BOOL bGotoReviveList = false;
	
	nTopIndex = Lua_GetTopIndex(L);

	KGLOG_PROCESS_ERROR((nTopIndex == 1) || (nTopIndex == 2));

	nFrames = (int)Lua_ValueToNumber(L, 1);
	if (nTopIndex == 2)
	{
		bGotoReviveList = (BOOL)Lua_ValueToNumber(L, 2);
	}

	bRetCode = SetDisappearFrames(nFrames, bGotoReviveList);
	KG_PROCESS_ERROR(bRetCode);

Exit0:
	Lua_PushBoolean(L, bRetCode);
	return 1;
}

int KNpc::LuaGotoReviveList(Lua_State* L)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 0);

	//bRetCode = GotoReviveList();
    bRetCode = SetDisappearFrames(1,true);
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KNpc::LuaGetTemplateReviveTime(Lua_State* L)
{
    int  nTopIndex      = 0;
    int  nReviveTime    = 0;

    nTopIndex = Lua_GetTopIndex(L);

    KGLOG_PROCESS_ERROR(nTopIndex == 0);

    nReviveTime = m_pTemplate->nReviveTime;

Exit0:
    Lua_PushNumber(L, nReviveTime);
    return 1;
}

int KNpc::LuaSetDialogFlag(Lua_State* L)
{
    int nRetCode = 0;

    nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 1);

	m_bDialogFlag = (BOOL)Lua_ValueToNumber(L, 1);

	g_PlayerServer.DoSyncNpcDialogFlag(this);

Exit0:
	return 0;
}

int KNpc::LuaSay(Lua_State* L)
{
    BOOL    bResult     = false;
	int     nRetCode    = false;
    int     nParamCount = 0;
    int     nRange      = 0;
    char*   pszText     = NULL;

    nParamCount = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nParamCount == 1 || nParamCount == 2);
    
    pszText = (char*)Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pszText);

    if (nParamCount == 2)
    {
        nRange = (int)Lua_ValueToNumber(L, 2);
        KGLOG_PROCESS_ERROR(nRange >= 0);
    }

	nRetCode = Say(pszText, nRange);
	KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KNpc::LuaYell(Lua_State* L)
{
    BOOL    bResult     = false;
	int     nRetCode    = false;
    char*   pszText     = NULL;

	nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 1);

	pszText = (char*)Lua_ValueToString(L, 1);
	KGLOG_PROCESS_ERROR(pszText);

	nRetCode = Yell(pszText);
	KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KNpc::LuaWhisper(Lua_State* L)
{
    BOOL        bResult     = false;
    int         nRetCode    = 0;
    DWORD       dwPlayerID  = ERROR_ID;
	KPlayer*    pPlayer     = NULL;
    char*       pszText     = NULL;

	nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 2);

	dwPlayerID = (DWORD)Lua_ValueToNumber(L, 1);
	pszText = (char*)Lua_ValueToString(L, 2);
	KGLOG_PROCESS_ERROR(pszText);
	
	pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

	nRetCode = Whisper(pPlayer, pszText);
	KGLOG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KNpc::LuaSayTo(Lua_State* L)
{
    BOOL        bResult     = false;
    int         nRetCode    = 0;
    DWORD       dwPlayerID  = ERROR_ID;
    KPlayer*    pPlayer     = NULL;
    char*       pszText     = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 2);

    dwPlayerID = (DWORD)Lua_ValueToNumber(L, 1);
    pszText = (char*)Lua_ValueToString(L, 2);
    KGLOG_PROCESS_ERROR(pszText);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    nRetCode = SayTo(pPlayer, pszText);
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KNpc::LuaYellTo(Lua_State* L)
{
    BOOL        bResult     = false;
    int         nRetCode    = 0;
    DWORD       dwPlayerID  = ERROR_ID;
    KPlayer*    pPlayer     = NULL;
    char*       pszText     = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 2);

    dwPlayerID = (DWORD)Lua_ValueToNumber(L, 1);
    pszText = (char*)Lua_ValueToString(L, 2);
    KGLOG_PROCESS_ERROR(pszText);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    nRetCode = YellTo(pPlayer, pszText);
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KNpc::LuaGetAISelectSkill(Lua_State* L)
{
    BOOL        bResult         = false;
    int         nRetCode        = 0;
    DWORD       dwSkillID       = 0;
    DWORD       dwSkillLevel    = 0;

    nRetCode = Lua_GetTopIndex(L);

    KG_PROCESS_ERROR(m_nSkillSelectIndex >= 0);
    KG_PROCESS_ERROR(m_nSkillSelectIndex < MAX_NPC_AI_SKILL);

    dwSkillID = m_pTemplate->dwSkillIDList[m_nSkillSelectIndex];
    dwSkillLevel = m_pTemplate->dwSkillLevelList[m_nSkillSelectIndex];

    bResult = true;
Exit0:
    Lua_PushNumber(L, dwSkillID);
    Lua_PushNumber(L, dwSkillLevel);
    return 2;
}
#endif //_SERVER

int KNpc::LuaIsSelectable(Lua_State* L)
{
    int  nRetCode       = 0;
	BOOL bIsSelectable  = false;

    nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 0);

    KGLOG_PROCESS_ERROR(m_pTemplate);

	bIsSelectable = m_pTemplate->bSelectable;
Exit0:
	Lua_PushBoolean(L, bIsSelectable);
	return 1;
}

int KNpc::LuaCanSeeLifeBar(Lua_State* L)
{
    int  nRetCode       = 0;
	BOOL bCanSeeLifeBar = false;

    nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 0);

    KGLOG_PROCESS_ERROR(m_pTemplate);

	bCanSeeLifeBar = m_pTemplate->bCanSeeLifeBar;
Exit0:
	Lua_PushBoolean(L, bCanSeeLifeBar);
	return 1;
}

int KNpc::LuaCanSeeName(Lua_State* L)
{
    int  nRetCode       = 0;
	BOOL bCanSeeName    = false;

    nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 0);

    KGLOG_PROCESS_ERROR(m_pTemplate);

	bCanSeeName = m_pTemplate->bCanSeeName;
Exit0:
	Lua_PushBoolean(L, bCanSeeName);
	return 1;
}

int KNpc::LuaGetNpcQuest(Lua_State* L)
{   
    int     nResult     = 0;
    BOOL    bRetCode    = false;
    std::vector<DWORD> vecQuestID;

    bRetCode = g_pSO3World->m_Settings.m_QuestInfoList.GetNpcQuest(m_dwTemplateID, &vecQuestID);
    KGLOG_PROCESS_ERROR(bRetCode);

    Lua_NewTable(L);
	
    for (size_t nIndex = 0; nIndex < vecQuestID.size(); ++nIndex)
    {
        Lua_PushNumber(L, nIndex + 1); // 下标从1开始
		Lua_PushNumber(L, vecQuestID[nIndex]);
        Lua_SetTable(L, -3);
    }

    nResult = 1;
Exit0:
    return nResult;
}

DEFINE_LUA_CLASS_BEGIN(KNpc)
	//注册属性
	REGISTER_LUA_DWORD_READONLY(KBaseObject, ID)

	REGISTER_LUA_INTEGER_READONLY(KSceneObject, X)
	REGISTER_LUA_INTEGER_READONLY(KSceneObject, Y)
	REGISTER_LUA_INTEGER_READONLY(KSceneObject, Z)
    REGISTER_LUA_FUNC(KSceneObject, GetAbsoluteCoordinate)
    REGISTER_LUA_INTEGER(KSceneObject, TouchRange)
	REGISTER_LUA_INTEGER_READONLY(KSceneObject, XCell)
	REGISTER_LUA_INTEGER_READONLY(KSceneObject, YCell)
	REGISTER_LUA_INTEGER(KSceneObject, Mass)
	REGISTER_LUA_INTEGER(KSceneObject, Width)
	REGISTER_LUA_INTEGER(KSceneObject, Length)
	REGISTER_LUA_INTEGER(KSceneObject, Height)
	REGISTER_LUA_INTEGER(KSceneObject, FaceDirection)
    REGISTER_LUA_INTEGER(KSceneObject, PitchDirection)

	REGISTER_LUA_STRING(KCharacter, Name)
	REGISTER_LUA_STRING(KCharacter, Title)
    REGISTER_LUA_ENUM(KCharacter, Gender)
	REGISTER_LUA_ENUM(KCharacter, Kind)
	REGISTER_LUA_DWORD(KCharacter, ClassID)
	REGISTER_LUA_ENUM(KCharacter, Race)
	REGISTER_LUA_DWORD(KCharacter, ForceID)
	REGISTER_LUA_INTEGER(KCharacter, Level)
    REGISTER_LUA_DWORD(KCharacter, ModelID)
	REGISTER_LUA_DWORD(KNpc, TemplateID)
    REGISTER_LUA_INTEGER(KNpc, ReviveTime)
	REGISTER_LUA_DWORD(KNpc, CharacterAction)
	REGISTER_LUA_BOOL(KNpc, DialogFlag)
	REGISTER_LUA_BOOL(KNpc, DisableAutoTurn)
    REGISTER_LUA_ENUM(KNpc, Species)

#ifdef _SERVER
	REGISTER_LUA_DWORD(KNpc, Owner)
#endif

    REGISTER_LUA_DWORD_READONLY(KNpc, DropTargetPlayerID)
    REGISTER_LUA_DWORD(KNpc, Employer)

    // ------------------ 移动相关 ---------------------------->
    REGISTER_LUA_ENUM_READONLY(KCharacter, MoveState)

	REGISTER_LUA_INTEGER_READONLY(KCharacter, VelocityXY)
	REGISTER_LUA_INTEGER_READONLY(KCharacter, DirectionXY)
	REGISTER_LUA_INTEGER_READONLY(KCharacter, VelocityZ)

	REGISTER_LUA_INTEGER_READONLY(KCharacter, DestX)
	REGISTER_LUA_INTEGER_READONLY(KCharacter, DestY)

    REGISTER_LUA_INTEGER_READONLY(KCharacter, RunSpeed)
	REGISTER_LUA_INTEGER_READONLY(KCharacter, RunSpeedBase)
	REGISTER_LUA_INTEGER_READONLY(KCharacter, MoveSpeedPercent)

    REGISTER_LUA_INTEGER_READONLY(KCharacter, MaxJumpCount)
    REGISTER_LUA_INTEGER_READONLY(KCharacter, Gravity)
    REGISTER_LUA_INTEGER_READONLY(KCharacter, GravityBase)
    REGISTER_LUA_INTEGER_READONLY(KCharacter, GravityPercent)

    REGISTER_LUA_INTEGER_READONLY(KCharacter, JumpSpeed)
    REGISTER_LUA_INTEGER_READONLY(KCharacter, JumpSpeedBase)
    REGISTER_LUA_INTEGER_READONLY(KCharacter, JumpSpeedPercent)

    REGISTER_LUA_INTEGER_READONLY(KCharacter, DropDefence)

    REGISTER_LUA_INTEGER_READONLY(KCharacter, DivingCount)         
    REGISTER_LUA_INTEGER_READONLY(KCharacter, DivingFrame)         
    REGISTER_LUA_INTEGER_READONLY(KCharacter, DivingFrameBase)
    REGISTER_LUA_INTEGER_READONLY(KCharacter, DivingFramePercent) 

    REGISTER_LUA_INTEGER_READONLY(KCharacter, WaterFlyAbility)

    // ------------------- 潜能 ------------------------------->
	REGISTER_LUA_INTEGER(KCharacter, CurrentStrength)
    REGISTER_LUA_INTEGER(KCharacter, StrengthBase)
    REGISTER_LUA_INTEGER(KCharacter, StrengthBasePercentAdd)
	REGISTER_LUA_INTEGER(KCharacter, CurrentAgility)
    REGISTER_LUA_INTEGER(KCharacter, AgilityBase)
    REGISTER_LUA_INTEGER(KCharacter, AgilityBasePercentAdd)
	REGISTER_LUA_INTEGER(KCharacter, CurrentVitality)
    REGISTER_LUA_INTEGER(KCharacter, VitalityBase)
    REGISTER_LUA_INTEGER(KCharacter, VitalityBasePercentAdd)
	REGISTER_LUA_INTEGER(KCharacter, CurrentSpirit)
    REGISTER_LUA_INTEGER(KCharacter, SpiritBase)
    REGISTER_LUA_INTEGER(KCharacter, SpiritBasePercentAdd)
    REGISTER_LUA_INTEGER(KCharacter, CurrentSpunk)
    REGISTER_LUA_INTEGER(KCharacter, SpunkBase)
    REGISTER_LUA_INTEGER(KCharacter, SpunkBasePercentAdd)

    // ------------------- 生命 ------------------------------->
	REGISTER_LUA_INTEGER(KCharacter, CurrentLife)
	REGISTER_LUA_INTEGER(KCharacter, MaxLife)
    REGISTER_LUA_INTEGER(KCharacter, MaxLifeBase)
	REGISTER_LUA_INTEGER(KCharacter, MaxLifePercentAdd)
	REGISTER_LUA_INTEGER(KCharacter, MaxLifeAdditional)
	REGISTER_LUA_INTEGER(KCharacter, LifeReplenish)
	REGISTER_LUA_INTEGER(KCharacter, LifeReplenishPercent)
    REGISTER_LUA_INTEGER(KCharacter, LifeReplenishCoefficient)
    REGISTER_LUA_INTEGER(KCharacter, LifeReplenishExt)

    // ------------------- 破绽 ------------------------------->
    REGISTER_LUA_INTEGER(KCharacter, UpperWeakLevel)
    REGISTER_LUA_INTEGER(KCharacter, MiddleWeakLevel)
    REGISTER_LUA_INTEGER(KCharacter, LowerWeakLevel)

    // ------------------- 内力 ------------------------------->
	REGISTER_LUA_INTEGER(KCharacter, CurrentMana)
	REGISTER_LUA_INTEGER(KCharacter, MaxMana)
    REGISTER_LUA_INTEGER(KCharacter, MaxManaBase)
	REGISTER_LUA_INTEGER(KCharacter, MaxManaPercentAdd)
	REGISTER_LUA_INTEGER(KCharacter, MaxManaAdditional)
	REGISTER_LUA_INTEGER(KCharacter, ManaReplenish)
	REGISTER_LUA_INTEGER(KCharacter, ManaReplenishPercent)
    REGISTER_LUA_INTEGER(KCharacter, ManaReplenishCoefficient)
    REGISTER_LUA_INTEGER(KCharacter, ManaReplenishExt)

    // -------------------- 战斗杂项 -------------------------->
	REGISTER_LUA_INTEGER(KCharacter, Dodge)
    REGISTER_LUA_INTEGER(KCharacter, DodgeBaseRate)
	REGISTER_LUA_INTEGER(KCharacter, Parry)
	REGISTER_LUA_INTEGER(KCharacter, ParryValue)
	REGISTER_LUA_INTEGER(KCharacter, Sense)
	REGISTER_LUA_INTEGER(KCharacter, Strain)
	REGISTER_LUA_INTEGER(KCharacter, Toughness)
	REGISTER_LUA_INTEGER(KCharacter, CriticalDamagePower)
	REGISTER_LUA_INTEGER(KCharacter, DecriticalDamagePower)
    REGISTER_LUA_BOOL(KCharacter, FightState)
    REGISTER_LUA_BOOL(KCharacter, SheathFlag)
    REGISTER_LUA_INTEGER(KCharacter, PositiveShield)
    REGISTER_LUA_INTEGER(KCharacter, NegativeShield)
    REGISTER_LUA_INTEGER(KCharacter, GlobalBlock)
    REGISTER_LUA_INTEGER(KCharacter, GlobalDamageAbsorb)
    REGISTER_LUA_INTEGER(KCharacter, GlobalDamageManaShield)
    REGISTER_LUA_INTEGER(KCharacter, DamageToLifeForSelf)
    REGISTER_LUA_INTEGER(KCharacter, DamageToManaForSelf)
    REGISTER_LUA_INTEGER(KCharacter, BeatBackRate)
    REGISTER_LUA_INTEGER(KCharacter, StunStrikeRate)
    REGISTER_LUA_INTEGER(KCharacter, KnockedOffRate)
    REGISTER_LUA_INTEGER(KCharacter, KnockedBackRate)

    // -------------------- 外功 ----------------------------->
	REGISTER_LUA_INTEGER(KCharacter, PhysicsAttackPower)
    REGISTER_LUA_INTEGER(KCharacter, PhysicsHitValue)
    REGISTER_LUA_INTEGER(KCharacter, PhysicsHitBaseRate)
	REGISTER_LUA_INTEGER(KCharacter, PhysicsCriticalStrike)
    REGISTER_LUA_INTEGER(KCharacter, PhysicsCriticalStrikeBaseRate)
    REGISTER_LUA_INTEGER(KCharacter, PhysicsResistPercent)
    REGISTER_LUA_INTEGER(KCharacter, PhysicsShield)
    REGISTER_LUA_INTEGER(KCharacter, PhysicsReflection)		
    REGISTER_LUA_INTEGER(KCharacter, PhysicsReflectionPercent)
    REGISTER_LUA_INTEGER(KCharacter, PhysicsDamageAbsorb)    
    REGISTER_LUA_INTEGER(KCharacter, PhysicsDamageManaShield)

    // -------------------- 阳性内功 ----------------------------->
    REGISTER_LUA_INTEGER(KCharacter, SolarAttackPower)
	REGISTER_LUA_INTEGER(KCharacter, SolarHitValue)
    REGISTER_LUA_INTEGER(KCharacter, SolarHitBaseRate)
    REGISTER_LUA_INTEGER(KCharacter, SolarCriticalStrike)
    REGISTER_LUA_INTEGER(KCharacter, SolarCriticalStrikeBaseRate)
	REGISTER_LUA_INTEGER(KCharacter, SolarMagicShield)
    REGISTER_LUA_INTEGER(KCharacter, SolarMagicResistPercent)
    REGISTER_LUA_INTEGER(KCharacter, SolarMagicReflection)		
    REGISTER_LUA_INTEGER(KCharacter, SolarMagicReflectionPercent)
    REGISTER_LUA_INTEGER(KCharacter, SolarMagicBlock)
    REGISTER_LUA_INTEGER(KCharacter, SolarDamageAbsorb)     
    REGISTER_LUA_INTEGER(KCharacter, SolarDamageManaShield)

    // -------------------- 中性内功 ----------------------------->
    REGISTER_LUA_INTEGER(KCharacter, NeutralAttackPower)
    REGISTER_LUA_INTEGER(KCharacter, NeutralHitValue)
    REGISTER_LUA_INTEGER(KCharacter, NeutralHitBaseRate)
    REGISTER_LUA_INTEGER(KCharacter, NeutralCriticalStrike)
    REGISTER_LUA_INTEGER(KCharacter, NeutralCriticalStrikeBaseRate)
    REGISTER_LUA_INTEGER(KCharacter, NeutralMagicShield)
    REGISTER_LUA_INTEGER(KCharacter, NeutralMagicResistPercent)
    REGISTER_LUA_INTEGER(KCharacter, NeutralMagicReflection)	
    REGISTER_LUA_INTEGER(KCharacter, NeutralMagicReflectionPercent)
    REGISTER_LUA_INTEGER(KCharacter, NeutralMagicBlock)
    REGISTER_LUA_INTEGER(KCharacter, NeutralDamageAbsorb) 
    REGISTER_LUA_INTEGER(KCharacter, NeutralDamageManaShield)

    // -------------------- 阴性内功 ----------------------------->
    REGISTER_LUA_INTEGER(KCharacter, LunarAttackPower)
    REGISTER_LUA_INTEGER(KCharacter, LunarHitValue)
    REGISTER_LUA_INTEGER(KCharacter, LunarHitBaseRate)
    REGISTER_LUA_INTEGER(KCharacter, LunarCriticalStrike)
    REGISTER_LUA_INTEGER(KCharacter, LunarCriticalStrikeBaseRate)
    REGISTER_LUA_INTEGER(KCharacter, LunarMagicShield)
    REGISTER_LUA_INTEGER(KCharacter, LunarMagicResistPercent)
    REGISTER_LUA_INTEGER(KCharacter, LunarMagicReflection)		
    REGISTER_LUA_INTEGER(KCharacter, LunarMagicReflectionPercent)	
    REGISTER_LUA_INTEGER(KCharacter, LunarMagicBlock)
    REGISTER_LUA_INTEGER(KCharacter, LunarDamageAbsorb)     
    REGISTER_LUA_INTEGER(KCharacter, LunarDamageManaShield) 

    // -------------------- 毒性 --------------------------------->
    REGISTER_LUA_INTEGER(KCharacter, PoisonAttackPower)
    REGISTER_LUA_INTEGER(KCharacter, PoisonHitValue)
    REGISTER_LUA_INTEGER(KCharacter, PoisonHitBaseRate)
    REGISTER_LUA_INTEGER(KCharacter, PoisonCriticalStrike)
    REGISTER_LUA_INTEGER(KCharacter, PoisonCriticalStrikeBaseRate)
    REGISTER_LUA_INTEGER(KCharacter, PoisonMagicShield)
    REGISTER_LUA_INTEGER(KCharacter, PoisonMagicResistPercent)
    REGISTER_LUA_INTEGER(KCharacter, PoisonMagicReflection)		
    REGISTER_LUA_INTEGER(KCharacter, PoisonMagicReflectionPercent)	
    REGISTER_LUA_INTEGER(KCharacter, PoisonMagicBlock)
    REGISTER_LUA_INTEGER(KCharacter, PoisonDamageAbsorb)     
    REGISTER_LUA_INTEGER(KCharacter, PoisonDamageManaShield) 

    // ------------------- 治疗 ---------------------------------->
    REGISTER_LUA_INTEGER(KCharacter, TherapyPower)
    REGISTER_LUA_INTEGER(KCharacter, SkillTherapy)

    // ------------------- 新手技能有关 -------------------------->

    REGISTER_LUA_INTEGER(KCharacter, NoneWeaponAttackSpeedBase)

    REGISTER_LUA_INTEGER(KCharacter, MeleeWeaponAttackSpeed)
    REGISTER_LUA_INTEGER(KCharacter, MeleeWeaponAttackSpeedBase)
    REGISTER_LUA_INTEGER(KCharacter, MeleeWeaponAttackSpeedPercentAdd)
    REGISTER_LUA_INTEGER(KCharacter, MeleeWeaponAttackSpeedAdditional)

    REGISTER_LUA_INTEGER(KCharacter, RangeWeaponAttackSpeed)
    REGISTER_LUA_INTEGER(KCharacter, RangeWeaponAttackSpeedBase)
    REGISTER_LUA_INTEGER(KCharacter, RangeWeaponAttackSpeedPercentAdd)
    REGISTER_LUA_INTEGER(KCharacter, RangeWeaponAttackSpeedAdditional)

    // -------------------------------------------------------------

    REGISTER_LUA_INTEGER(KNpc, Intensity)
	REGISTER_LUA_DWORD(KNpc, ScriptID)

    REGISTER_LUA_DWORD_READONLY(KCharacter, AIEventSrc)
    REGISTER_LUA_INTEGER_READONLY(KCharacter, AIEventParam)

#ifdef _SERVER
	REGISTER_LUA_DWORD(KNpc, ReliveID)
	REGISTER_LUA_DWORD(KNpc, RandomID)
	REGISTER_LUA_INTEGER(KNpc, NpcTeamID)
	REGISTER_LUA_INTEGER(KNpc, NpcTeamIndex)
#endif //_SERVER

	//注册函数

    REGISTER_LUA_FUNC(KCharacter, SetModelID)
    REGISTER_LUA_FUNC(KCharacter, CastSkillXYZ)

    REGISTER_LUA_FUNC(KCharacter, Stand)
	REGISTER_LUA_FUNC(KCharacter, WalkTo)
	REGISTER_LUA_FUNC(KCharacter, RunTo)
    REGISTER_LUA_FUNC(KCharacter, SwimTo)
	REGISTER_LUA_FUNC(KCharacter, Jump)
	REGISTER_LUA_FUNC(KCharacter, SitDown)
	REGISTER_LUA_FUNC(KCharacter, Dash)

	REGISTER_LUA_FUNC(KCharacter, Patrol)

	REGISTER_LUA_FUNC(KCharacter, Die)

	REGISTER_LUA_FUNC(KCharacter, TurnTo)
    REGISTER_LUA_FUNC(KCharacter, PitchTo)
	REGISTER_LUA_FUNC(KCharacter, DoAction)

    REGISTER_LUA_FUNC(KCharacter, SetTarget)
    REGISTER_LUA_FUNC(KCharacter, CastSkill)
    REGISTER_LUA_FUNC(KCharacter, GetAIType)
	REGISTER_LUA_FUNC(KCharacter, SetAIType)
    REGISTER_LUA_FUNC(KCharacter, SetAIDebug)

#ifdef _SERVER
	REGISTER_LUA_FUNC(KCharacter, SetPosition)
#endif

    REGISTER_LUA_FUNC(KCharacter, FireAIEvent)

	REGISTER_LUA_FUNC(KNpc, GetTarget)
	
#if defined(_SERVER)
    REGISTER_LUA_FUNC(KNpc, GetQuestString)
	REGISTER_LUA_FUNC(KNpc, GetAutoDialogString)
    REGISTER_LUA_FUNC(KNpc, BindNpcShop)
    REGISTER_LUA_FUNC(KNpc, UnbindNpcShop)
    REGISTER_LUA_FUNC(KNpc, GetShop)
	REGISTER_LUA_FUNC(KNpc, FollowTarget)
	REGISTER_LUA_FUNC(KNpc, ModifyThreat)
	REGISTER_LUA_FUNC(KNpc, SetPatrolPath)

	REGISTER_LUA_FUNC(KNpc, SetScript)

	REGISTER_LUA_FUNC(KCharacter, ClearAllThreat)
	
	REGISTER_LUA_FUNC(KCharacter, SetForceID)

	REGISTER_LUA_FUNC(KNpc, SetDisappearFrames)
	REGISTER_LUA_FUNC(KNpc, GotoReviveList)
    REGISTER_LUA_FUNC(KNpc, GetTemplateReviveTime)
#endif	//_SERVER

#ifdef _CLIENT
    REGISTER_LUA_FUNC(KNpc, GetBuffList)
#endif

    REGISTER_LUA_FUNC(KNpc, CancelBuff)

    REGISTER_LUA_FUNC(KNpc, SetSheath)

	REGISTER_LUA_FUNC(KNpc, GetScene)
	REGISTER_LUA_FUNC(KNpc, GetMapID)
	REGISTER_LUA_FUNC(KNpc, CanDialog)
	REGISTER_LUA_FUNC(KNpc, IsSelectable)
	REGISTER_LUA_FUNC(KNpc, CanSeeLifeBar)
	REGISTER_LUA_FUNC(KNpc, CanSeeName)
    REGISTER_LUA_FUNC(KNpc, GetNpcQuest)

#ifdef _SERVER
    REGISTER_LUA_FUNC(KNpc, AddBuff)

    REGISTER_LUA_FUNC(KNpc, GetBuff)

    REGISTER_LUA_FUNC(KNpc, DelBuff)
    REGISTER_LUA_FUNC(KNpc, DelGroupBuff)

    REGISTER_LUA_FUNC(KNpc, DelMultiGroupBuffByFunctionType)

    REGISTER_LUA_FUNC(KNpc, SetBuffNextActiveFrame)
    REGISTER_LUA_FUNC(KNpc, SetBuffLeftActiveCount)

	REGISTER_LUA_FUNC(KNpc, SetDialogFlag)

	REGISTER_LUA_FUNC(KNpc, Say)
	REGISTER_LUA_FUNC(KNpc, Yell)
	REGISTER_LUA_FUNC(KNpc, Whisper)
    REGISTER_LUA_FUNC(KNpc, SayTo)
    REGISTER_LUA_FUNC(KNpc, YellTo)
    REGISTER_LUA_FUNC(KNpc, GetAISelectSkill)

	REGISTER_LUA_FUNC(KNpc, SetBank)

    REGISTER_LUA_FUNC(KCharacter, Attach)
    REGISTER_LUA_FUNC(KCharacter, Detach)

    REGISTER_LUA_FUNC(KCharacter, SetCustomInteger4)
    REGISTER_LUA_FUNC(KCharacter, GetCustomInteger4)
    REGISTER_LUA_FUNC(KCharacter, SetCustomInteger2)
    REGISTER_LUA_FUNC(KCharacter, GetCustomInteger2)
    REGISTER_LUA_FUNC(KCharacter, SetCustomInteger1)
    REGISTER_LUA_FUNC(KCharacter, GetCustomInteger1)
    REGISTER_LUA_FUNC(KCharacter, SetCustomUnsigned4)
    REGISTER_LUA_FUNC(KCharacter, GetCustomUnsigned4)
    REGISTER_LUA_FUNC(KCharacter, SetCustomUnsigned2)
    REGISTER_LUA_FUNC(KCharacter, GetCustomUnsigned2)
    REGISTER_LUA_FUNC(KCharacter, SetCustomUnsigned1)
    REGISTER_LUA_FUNC(KCharacter, GetCustomUnsigned1)
    REGISTER_LUA_FUNC(KCharacter, SetCustomBoolean)
    REGISTER_LUA_FUNC(KCharacter, GetCustomBoolean)
    REGISTER_LUA_FUNC(KCharacter, SetCustomBitValue)
    REGISTER_LUA_FUNC(KCharacter, GetCustomBitValue)
    REGISTER_LUA_FUNC(KCharacter, SetCustomString)
    REGISTER_LUA_FUNC(KCharacter, GetCustomString)
#endif
    REGISTER_LUA_FUNC(KCharacter, GetSkillPrepareState)
    REGISTER_LUA_FUNC(KCharacter, SetAIOriginPos)
    REGISTER_LUA_FUNC(KCharacter, GetAIOriginPos)

DEFINE_LUA_CLASS_END(KNpc)

DEFINE_LUA_CLASS_BEGIN(KNpcTemplate)
	REGISTER_LUA_DWORD(KNpcTemplate, TemplateID)
	REGISTER_LUA_STRING(KNpcTemplate, Name)
	REGISTER_LUA_INTEGER(KNpcTemplate, Kind)
	REGISTER_LUA_DWORD(KNpcTemplate, ForceID)
	REGISTER_LUA_INTEGER(KNpcTemplate, Species)
	REGISTER_LUA_INTEGER(KNpcTemplate, Level)
	REGISTER_LUA_INTEGER(KNpcTemplate, Height)
DEFINE_LUA_CLASS_END(KNpcTemplate)
