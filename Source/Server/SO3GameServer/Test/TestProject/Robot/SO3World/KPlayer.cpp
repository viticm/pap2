#include "stdafx.h"
#include <conio.h>
#include "KPlayer.h"

#include "Common/CRC32.h"
#include "KSO3World.h"
#include "KMath.h"
#include "KScriptCenter.h"
#include "KObjectManager.h"
#include "KWorldSettings.h"
#include "KRegion.h"
#include "KScene.h"
#include "KDoodad.h"
#include "KRoleDBDataDef.h"
#include "KAttrModifier.h"
//#include "KSkillManager.h"
//#include "KSkill.h"
#include "Engine/KG_Time.h"
#include "KPlayerClient.h"
#include "KMonster.h"

BYTE     KPlayer::s_byTalkData[];
unsigned KPlayer::s_uTalkDataSize = 0;
DEFINE_LUA_CLASS_BEGIN(KPlayer)
DEFINE_LUA_CLASS_END(KNpc)

KPlayer::KPlayer()
{
    m_dwPartyID         = INVALID_PARTY_ID;
    m_bPartyLeaderFlag  = false;
}

KPlayer::~KPlayer()
{
}

BOOL KPlayer::Init(void)
{
	BOOL bRetCode = FALSE;

	bRetCode = KCharacter::Init();
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = m_ItemList.Init(this);
	KGLOG_PROCESS_ERROR(bRetCode);

	m_eKind = ckPlayer;
	m_nLevel = 0;  

	m_eCamp             = cNeutral;
    m_dwPartyID         = INVALID_PARTY_ID;
    m_bPartyLeaderFlag  = false;
	m_eGameStatus       = gsInvalid;

	m_eRoleType	        = rtInvalid;
    m_bFightState       = false;
    m_nLevel            = 0;

	m_nDoubleExp        = 0;
    
	//m_nCurrentStamina		= 0;
	//m_nStaminaReplenish		= 0;
	//m_nStaminaReplenishNextFrame = 0;
	memset(m_wRepresentId, 0, sizeof(m_wRepresentId));

//    m_SkillList.Init(this);

	bRetCode = m_ReputeList.Init(this);
	KGLOG_PROCESS_ERROR(bRetCode);

	return TRUE;

Exit0:

	return FALSE;
}

BOOL KPlayer::UnInit(void)
{
	BOOL bRetCode = FALSE;
	BOOL bResult = FALSE;

//    m_SkillList.UnInit();

	bRetCode = m_ItemList.UnInit();
	KGLOG_CHECK_ERROR(bRetCode);

	bRetCode = KCharacter::UnInit();
	KGLOG_CHECK_ERROR(bRetCode);

	bResult = TRUE;
//Exit0:
	return bResult;
}

BOOL KPlayer::Activate(void)
{
	BOOL bRetCode = FALSE;

	bRetCode = KSceneObject::CheckGameLoop();
	if (!bRetCode)
		return TRUE;

	KGLOG_PROCESS_ERROR(m_eGameStatus != gsInvalid && m_eGameStatus != gsSyncData &&
		m_eGameStatus != gsSearchMap && m_eGameStatus != gsTransferData);

	//if (m_nConnIndex != -1)
	//{
	//	bRetCode = SyncChangeRegion();
	//	KGLOG_PROCESS_ERROR(bRetCode);
	//}

//    DoCycleSynchronous();
//	ReplenishAttribute();
	KCharacter::Activate();
    // m_pScene指针为空表示自己被删除了
    KG_PROCESS_ERROR(m_pScene);
	return TRUE;
Exit0:
	return FALSE;
}

void KPlayer::SetDisplayData(
    char szRoleName[_NAME_LEN], int nRoleType,
    WORD wRepresentId[REPRESENT_COUNT], int nLevel
)
{

	m_eRoleType        = (ROLE_TYPE)nRoleType;
	m_nLevel		   = nLevel;

	strncpy(m_szName, szRoleName, _NAME_LEN);
	m_szName[_NAME_LEN - 1] = 0;

	memcpy(m_wRepresentId, wRepresentId, sizeof(m_wRepresentId));
}


void KPlayer::AddExp(int nExpIncrement, BOOL bUseDoubleExp)
{
	BOOL            bRetCode        = false;
    KLevelUpData*   pLevelUpData    = NULL;

	// 如果达到最高等级，则不允许加经验
	KG_PROCESS_ERROR(m_nLevel < MAX_PLAYER_LEVEL);

	m_nExperience += nExpIncrement;

	if (m_nDoubleExp > 0)
	{
		if (m_nDoubleExp > nExpIncrement)
		{
			m_nExperience += nExpIncrement;
			m_nDoubleExp -= nExpIncrement;
		}
		else
		{
			m_nExperience += m_nDoubleExp;
			m_nDoubleExp = 0;
		}
	}

	pLevelUpData = g_WorldSettings.m_LevelUpList.GetLevelUpData(m_eRoleType, m_nLevel);
	KGLOG_PROCESS_ERROR(pLevelUpData);

	while (m_nExperience >= pLevelUpData->nExperience && m_nLevel < MAX_PLAYER_LEVEL)
	{
		m_nExperience -= pLevelUpData->nExperience;

		setLevel(m_nLevel + 1);
        m_nCurrentLife = m_nMaxLife;
        m_nCurrentMana = m_nMaxMana;

		pLevelUpData = g_WorldSettings.m_LevelUpList.GetLevelUpData(m_eRoleType, m_nLevel);
		KGLOG_PROCESS_ERROR(pLevelUpData);
	}

Exit0:
	return;
}

void KPlayer::setLevel(int nLevel)
{    
    KLevelUpData* pLevelUpData = NULL;

    if (m_nLevel > 0)
    {
	    pLevelUpData = g_WorldSettings.m_LevelUpList.GetLevelUpData(m_eRoleType, m_nLevel);   
        KGLOG_PROCESS_ERROR(pLevelUpData);

        g_AttributeProcessFunctions[atStrengthBase](this, false, pLevelUpData->nStrength, 0);
        g_AttributeProcessFunctions[atAgilityBase](this, false, pLevelUpData->nAgility, 0);
        g_AttributeProcessFunctions[atVitalityBase](this, false, pLevelUpData->nVigor, 0);
        g_AttributeProcessFunctions[atSpiritBase](this, false, pLevelUpData->nSpirit, 0);
        g_AttributeProcessFunctions[atSpunkBase](this, false, pLevelUpData->nSpunk, 0);
        g_AttributeProcessFunctions[atMaxLifeBase](this, false, pLevelUpData->nMaxLife, 0);
        g_AttributeProcessFunctions[atMaxManaBase](this, false, pLevelUpData->nMaxMana, 0);
        g_AttributeProcessFunctions[atMaxRageBase](this, false, pLevelUpData->nMaxRage, 0);
        g_AttributeProcessFunctions[atWalkSpeedBase](this, false, pLevelUpData->nWalkSpeed, 0);
        g_AttributeProcessFunctions[atRunSpeedBase](this, false, pLevelUpData->nRunSpeed, 0);
        g_AttributeProcessFunctions[atSwimSpeedBase](this, false, pLevelUpData->nSwimSpeed, 0);
        g_AttributeProcessFunctions[atJumpSpeedXYBase](this, false, pLevelUpData->nJumpSpeedXY, 0);
        g_AttributeProcessFunctions[atJumpSpeedZBase](this, false, pLevelUpData->nJumpSpeedZ, 0);

        // ------------ 属性回复 ----------------------------
        g_AttributeProcessFunctions[atLifeReplenish](this, false, pLevelUpData->nLifeReplenish, 0);
        g_AttributeProcessFunctions[atLifeReplenishPercent](this, false, pLevelUpData->nLifeReplenishPercent, 0);
        g_AttributeProcessFunctions[atManaReplenish](this, false, pLevelUpData->nManaReplenish, 0);
        g_AttributeProcessFunctions[atManaReplenishPercent](this, false, pLevelUpData->nManaReplenishPercent, 0);
        g_AttributeProcessFunctions[atRageReplenish](this, false, pLevelUpData->nRageReplenish, 0);
        g_AttributeProcessFunctions[atRageReplenishPercent](this, false, pLevelUpData->nRageReplenishPercent, 0);
        //g_AttributeProcessFunctions[atStaminaReplenish](this, false, pLevelUpData->nStaminaReplenish, 0);
        //g_AttributeProcessFunctions[atStaminaReplenishPercent](this, false, pLevelUpData->nStaminaReplenishPercent, 0);

        // ------------- 技能命中值 -----------------------------
        g_AttributeProcessFunctions[atPhysicsAttackHit](this, false, pLevelUpData->nPhysicsAttackHit, 0);
        g_AttributeProcessFunctions[atSolarMagicHit](this, false, pLevelUpData->nSolarMagicHit, 0);
        g_AttributeProcessFunctions[atNeutralMagicHit](this, false, pLevelUpData->nNeutralMagicHit, 0);
        g_AttributeProcessFunctions[atLunarMagicHit](this, false, pLevelUpData->nLunarMagicHit, 0);
        g_AttributeProcessFunctions[atPoisonMagicHit](this, false, pLevelUpData->nPoisonMagicHit, 0);

        // ------------- 技能暴击值 -----------------------------
        g_AttributeProcessFunctions[atPhysicsCriticalStrike](this, false, pLevelUpData->nPhysicsCriticalStrike, 0);
        g_AttributeProcessFunctions[atSolarCriticalStrike](this, false, pLevelUpData->nSolarCriticalStrike, 0);
        g_AttributeProcessFunctions[atNeutralCriticalStrike](this, false, pLevelUpData->nNeutralCriticalStrike, 0);
        g_AttributeProcessFunctions[atLunarCriticalStrike](this, false, pLevelUpData->nLunarCriticalStrike, 0);
        g_AttributeProcessFunctions[atPoisonCriticalStrike](this, false, pLevelUpData->nPoisonCriticalStrike, 0);
        
        g_AttributeProcessFunctions[atNoneWeaponAttackSpeedBase](this, false, pLevelUpData->nNoneWeaponAttackSpeedBase, 0);
        
        g_AttributeProcessFunctions[atPhysicsDefenceMax](this, false, pLevelUpData->nMaxPhysicsDefence, 0);
    }

	pLevelUpData = g_WorldSettings.m_LevelUpList.GetLevelUpData(m_eRoleType, nLevel);
	KGLOG_PROCESS_ERROR(pLevelUpData);

    g_AttributeProcessFunctions[atStrengthBase](this, true, pLevelUpData->nStrength, 0);
    g_AttributeProcessFunctions[atAgilityBase](this, true, pLevelUpData->nAgility, 0);
    g_AttributeProcessFunctions[atVitalityBase](this, true, pLevelUpData->nVigor, 0);
    g_AttributeProcessFunctions[atSpiritBase](this, true, pLevelUpData->nSpirit, 0);
    g_AttributeProcessFunctions[atSpunkBase](this, true, pLevelUpData->nSpunk, 0);
    g_AttributeProcessFunctions[atMaxLifeBase](this, true, pLevelUpData->nMaxLife, 0);
    g_AttributeProcessFunctions[atMaxManaBase](this, true, pLevelUpData->nMaxMana, 0);
    g_AttributeProcessFunctions[atMaxRageBase](this, true, pLevelUpData->nMaxRage, 0);
    g_AttributeProcessFunctions[atWalkSpeedBase](this, true, pLevelUpData->nWalkSpeed, 0);
    g_AttributeProcessFunctions[atRunSpeedBase](this, true, pLevelUpData->nRunSpeed, 0);
    g_AttributeProcessFunctions[atSwimSpeedBase](this, true, pLevelUpData->nSwimSpeed, 0);
    g_AttributeProcessFunctions[atJumpSpeedXYBase](this, true, pLevelUpData->nJumpSpeedXY, 0);
    g_AttributeProcessFunctions[atJumpSpeedZBase](this, true, pLevelUpData->nJumpSpeedZ, 0);

    // ------------ 属性回复 ----------------------------
    g_AttributeProcessFunctions[atLifeReplenish](this, true, pLevelUpData->nLifeReplenish, 0);
    g_AttributeProcessFunctions[atLifeReplenishPercent](this, true, pLevelUpData->nLifeReplenishPercent, 0);
    g_AttributeProcessFunctions[atManaReplenish](this, true, pLevelUpData->nManaReplenish, 0);
    g_AttributeProcessFunctions[atManaReplenishPercent](this, true, pLevelUpData->nManaReplenishPercent, 0);
    g_AttributeProcessFunctions[atRageReplenish](this, true, pLevelUpData->nRageReplenish, 0);
    g_AttributeProcessFunctions[atRageReplenishPercent](this, true, pLevelUpData->nRageReplenishPercent, 0);
    //g_AttributeProcessFunctions[atStaminaReplenish](this, true, pLevelUpData->nStaminaReplenish, 0);
    //g_AttributeProcessFunctions[atStaminaReplenishPercent](this, true, pLevelUpData->nStaminaReplenishPercent, 0);

    // ------------- 技能命中值 -------------------------
    g_AttributeProcessFunctions[atPhysicsAttackHit](this, true, pLevelUpData->nPhysicsAttackHit, 0);
    g_AttributeProcessFunctions[atSolarMagicHit](this, true, pLevelUpData->nSolarMagicHit, 0);
    g_AttributeProcessFunctions[atNeutralMagicHit](this, true, pLevelUpData->nNeutralMagicHit, 0);
    g_AttributeProcessFunctions[atLunarMagicHit](this, true, pLevelUpData->nLunarMagicHit, 0);
    g_AttributeProcessFunctions[atPoisonMagicHit](this, true, pLevelUpData->nPoisonMagicHit, 0);
    // --------------------------------------------------
    g_AttributeProcessFunctions[atPhysicsCriticalStrike](this, true, pLevelUpData->nPhysicsCriticalStrike, 0);
    g_AttributeProcessFunctions[atSolarCriticalStrike](this, true, pLevelUpData->nSolarCriticalStrike, 0);
    g_AttributeProcessFunctions[atNeutralCriticalStrike](this, true, pLevelUpData->nNeutralCriticalStrike, 0);
    g_AttributeProcessFunctions[atLunarCriticalStrike](this, true, pLevelUpData->nLunarCriticalStrike, 0);
    g_AttributeProcessFunctions[atPoisonCriticalStrike](this, true, pLevelUpData->nPoisonCriticalStrike, 0);
    
    g_AttributeProcessFunctions[atNoneWeaponAttackSpeedBase](this, true, pLevelUpData->nNoneWeaponAttackSpeedBase, 0);

    g_AttributeProcessFunctions[atPhysicsDefenceMax](this, true, pLevelUpData->nMaxPhysicsDefence, 0);

	// ---------- 神,生活技能用 --------------------------------->
	m_nStaminaReplenish = pLevelUpData->nStaminaReplenish;
	// -----------------------------------------------------------
    m_nHeight = pLevelUpData->nHeight;
	m_nLevel  = nLevel;

    // ------------ 气 ------------------------------------------>
    SetQi(m_nCurrentQi);
    
    // 下面两个魔法属性对应的值, 会随玩家的等级改变而改变
    g_AttributeProcessFunctions[atMeleeSpunkAttackPower](this, true, 0, 0);  
    g_AttributeProcessFunctions[atRangeSpunkAttackPower](this, true, 0, 0);  

Exit0:

	return;
}

//检测角色当前所在格子是否有Trap脚本
BOOL KPlayer::CheckTrap(KCell* pOldCell)
{
	BOOL bRetCode = FALSE;

	KGLOG_PROCESS_ERROR(m_pCell);
	return TRUE;
Exit0:
	return FALSE;
}

//处理本循环的指令，对于玩家来说是指令列表
BOOL KPlayer::GetCommand(void)
{


	return TRUE;
}

BOOL KPlayer::ChangeRegion(KRegion* pDestRegion)
{
	BOOL bRetCode = FALSE;
	KUpdateAllDoodadFunc UpdateAllDoodadFunc;

	KGLOG_PROCESS_ERROR(m_pRegion);
	KGLOG_PROCESS_ERROR(pDestRegion);

	KGLogPrintf(KGLOG_INFO, "Player %d ChangeRegion from (%d, %d) to (%d, %d)", 
		m_dwID, m_pRegion->m_nRegionX, m_pRegion->m_nRegionY,
		pDestRegion->m_nRegionX, pDestRegion->m_nRegionY);

	m_pRegion = pDestRegion;

	return TRUE;

Exit0:

	return FALSE;
}


BOOL KPlayer::ChangeCell(KCell* pCell)
{
	BOOL bRetCode = FALSE;

	ASSERT(pCell);

	KCell* pOldCell = m_pCell;

	m_pCell = pCell;

	bRetCode = CheckTrap(pOldCell);
	KGLOG_PROCESS_ERROR(bRetCode);

	return TRUE;

Exit0:

	return FALSE;
}

//出错时,输出Player的信息
void KPlayer::Dump(void)
{
	return;
}

//保存现在的玩家坐标
BOOL KPlayer::SaveCurrentPosition(void)
{
	KGLOG_PROCESS_ERROR(m_pScene);

	m_OldPosition.m_nX = m_nX;
	m_OldPosition.m_nY = m_nY;
	m_OldPosition.m_nZ = m_nZ;
	m_OldPosition.m_dwMapID = m_pScene->m_dwMapID;
	m_OldPosition.m_dwMapCopyIndex = m_pScene->m_dwMapCopyIndex;

	return TRUE;

Exit0:

	return FALSE;
}

//恢复玩家的原始位置
BOOL KPlayer::RestorePosition(void)
{
	BOOL bRetCode = FALSE;

	//获取原地图指针
	KScene* pDestScene = g_SO3World.GetScene(m_OldPosition.m_dwMapID, 
		m_OldPosition.m_dwMapCopyIndex);
	KGLOG_PROCESS_ERROR(pDestScene);

	//恢复原坐标
	m_nX = m_OldPosition.m_nX;
	m_nY = m_OldPosition.m_nY;
	m_nZ = m_OldPosition.m_nZ;
	m_nXCell = m_nX / CELL_LENGTH % REGION_GRID_WIDTH;
	m_nYCell = m_nY / CELL_LENGTH % REGION_GRID_HEIGHT;

	//添加到旧地图上
	bRetCode = pDestScene->AddPlayer(this);
	KGLOG_PROCESS_ERROR(bRetCode);

	return TRUE;

Exit0:

	return FALSE;
}

BOOL KPlayer::AddLogOutQi(time_t TotalLogOutTime)
{
    BOOL        bRetCode          = false;
    int         nQiAdd            = 0;
    int         nQiReplenishCycle = 12 * 60;


    bRetCode = TRUE;
Exit0:
    return bRetCode;
}

BOOL KPlayer::SetQi(int nQi)
{
    BOOL          bResult       = false;
    KLevelUpData* pLevelUpData  = NULL;

    pLevelUpData = g_WorldSettings.m_LevelUpList.GetLevelUpData(m_eRoleType, m_nLevel);
    KGLOG_PROCESS_ERROR(pLevelUpData);
    KGLOG_PROCESS_ERROR(pLevelUpData->nMaxQi);

    m_nCurrentQi = nQi;
    MAKE_IN_RANGE(m_nCurrentQi, 0, pLevelUpData->nMaxQi);

#ifdef _SERVER
    g_PlayerServer.DoUpdatePlayerQi(m_nConnIndex, m_nCurrentQi);
#endif

    bResult = true;
Exit0:
    return bResult;
}

int KPlayer::GetCDValue(DWORD dwCooldownID)
{
    int nResult  = 0;

    nResult = g_WorldSettings.m_CoolDownList.GetCoolDownValue(dwCooldownID);
    //KG_ASSERT_EXIT(nDuration > 0);

    // TODO: 查玩家天赋,某些天赋可以减少某些Cool down时间
    // ... ...
    
    return nResult;
}

BOOL KPlayer::MountKungfu(DWORD dwID, DWORD dwLevel)
{
    BOOL            bResult     = false;
    int             nRetCode    = false;
    KSkill*         pKungfu     = NULL;

//    KGLOG_PROCESS_ERROR(m_SkillList.m_dwMountKungfuID == INVALID_SKILL_ID);

//    pKungfu = g_SkillManager.GetSkill_K(dwID, dwLevel);
    //KGLOG_PROCESS_ERROR(pKungfu);

    //KGLOG_PROCESS_ERROR(pKungfu->m_pBaseInfo->bIsMountAble);

    //ApplyAttribute(pKungfu->m_pAttrbuteEffectToSelfAndRollback);

    //m_SkillList.m_dwMountKungfuID       = dwID;
    //m_SkillList.m_dwMountKungfuLevel    = dwLevel;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayer::UmountKungfu()
{
    BOOL    bResult  = false;
    BOOL    bRetCode = false;
 //   KSkill* pKungfu  = NULL;

    //if (m_SkillList.m_dwMountKungfuID != INVALID_SKILL_ID)
    //{
    //    pKungfu = g_SkillManager.GetSkill_K(m_SkillList.m_dwMountKungfuID, m_SkillList.m_dwMountKungfuLevel);
    //    KGLOG_PROCESS_ERROR(pKungfu);

    //    UnApplyAttribute(pKungfu->m_pAttrbuteEffectToSelfAndRollback);
    //}

    //m_SkillList.m_dwMountKungfuID       = INVALID_SKILL_ID;
    //m_SkillList.m_dwMountKungfuLevel    = 0;

    bResult = true;
Exit0:
    return bResult;
}

// ------ Talk about --------------------------------------------------->
BOOL KPlayer::CopyTalkData(unsigned uSize, BYTE* pbyData)
{
    BOOL bResult = FALSE;

    KGLOG_PROCESS_ERROR(m_dwID == m_pOwner->m_pRole->m_dwID);
    KGLOG_PROCESS_ERROR(uSize > 0 && uSize <= MAX_CHAT_TEXT_LEN);
    KGLOG_PROCESS_ERROR(pbyData);

    memcpy(s_byTalkData, pbyData, uSize);
    s_uTalkDataSize = uSize;

Exit0:
    return bResult;
}


BOOL KPlayer::TalkToOne(const char cszTalkTo[], unsigned uTalkDataSize, BYTE* pbyTalkData)
{
	BOOL bRetCode = FALSE;

    KGLOG_PROCESS_ERROR(cszTalkTo);
    KGLOG_PROCESS_ERROR(uTalkDataSize > 0);
    KGLOG_PROCESS_ERROR(pbyTalkData);

    bRetCode = m_pOwner->m_PlayerClient.DoPlayerTalkToOne(cszTalkTo, uTalkDataSize, pbyTalkData);

Exit0:
	return bRetCode;
}

BOOL KPlayer::TalkToMany(PLAYER_TALK_RANGE eTalkRange, unsigned uTalkDataSize, BYTE* pbyTalkData)
{
	BOOL bRetCode = FALSE;

    KGLOG_PROCESS_ERROR(eTalkRange > trInvalid && eTalkRange < trTotal);
    KGLOG_PROCESS_ERROR(uTalkDataSize > 0);
    KGLOG_PROCESS_ERROR(pbyTalkData);

    // PLAYER_TALK_RANGE will send as a BYTE, make sure its value is in rang 0 ~ 255.
    KGLOG_PROCESS_ERROR(trInvalid >= 0 && trTotal < 256);

    bRetCode = m_pOwner->m_PlayerClient.DoPlayerTalkToMany((BYTE)eTalkRange, uTalkDataSize, pbyTalkData);

Exit0:
	return bRetCode;
}
// <----- Talk about ----------------------------------------------------
 
int KPlayer::GetNpcRelation(KNpc* pTarget)
{
	int nRelation = 0;
	//BOOL bFightFlag = FALSE;
	//int nReputation = 0;

	//ASSERT(!g_IsPlayer(pTarget->m_dwID));

	//nReputation = m_ReputeList.GetReputation(pTarget->m_dwForceID);

	//if (nReputation >= DIALOG_REPUTE)
	//{
	//	nRelation |= sortDialog;
	//}

	//if (nRelation >= ALLY_REPUTE)
	//{
	//	nRelation |= sortAlly;
	//}

	//bFightFlag = m_ReputeList.GetFightFlag(pTarget->m_dwForceID);
	//if (bFightFlag)
	//{
	//	nRelation |= sortEnemy;
	//	nRelation &= ~sortAlly;
	//	nRelation &= ~sortDialog;
	//}

	return nRelation;
}

int KPlayer::GetPlayerRelation(KPlayer* pTarget)
{
	int nRelation = 0;
	///*ASSERT(g_IsPlayer(pTarget->m_dwID));
	//int nValue = g_WorldSettings.m_RelationCampList.GetRelation(m_eCamp, pTarget->m_eCamp);
	//switch(nValue)
	//{
	//case -1:
	//	nRelation |= sortEnemy;
	//	nRelation &= ~sortAlly;
	//	break;
	//case 0:
	//	nRelation |= sortNone;
	//	break;
	//case 1:
	//	nRelation |= sortAlly;
	//	break;
	//default:
	//	break;
	//}
	//if (m_ThreatList.IsInThreatList(pTarget->m_dwID))
	//{
	//	nRelation |= sortEnemy;
	//	nRelation &= ~sortAlly;
	//}

	//if (m_PK.GetMode() == pkmKiller)
	//{
	//	nRelation |= sortEnemy;
	//	nRelation &= ~sortAlly;
	//}

	//if (m_PK.GetTarget() == pTarget)
	//{
	//	nRelation |= sortEnemy;
	//	nRelation &= ~sortAlly;
	//}

	//if (m_dwPartyID == pTarget->m_dwPartyID && m_dwPartyID != INVALID_PARTY_ID)
	//{
	//	nRelation |= sortParty;
	//	nRelation |= sortAlly;
	//	nRelation &= ~sortEnemy;
	//}
	//
	//if (this == pTarget)
	//{
	//	nRelation |= sortSelf;
	//	nRelation |= sortAlly;
	//}*/

	return nRelation;
}

BOOL KPlayer::SwitchSelectTarget(DWORD dwSkillSrcID)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    TARGET_TYPE eTargetType = ttInvalid;
    KCharacter* pCaster     = NULL;

    eTargetType = m_SelectTarget.GetTargetType();
    KG_PROCESS_ERROR(eTargetType == ttNoTarget);
    if (g_IsPlayer(dwSkillSrcID))
    {
        int nRelation = 0;
        pCaster = (KCharacter*)m_pOwner->m_Eyeshot.GetPlayer(dwSkillSrcID);
        KG_PROCESS_ERROR(pCaster);
        nRelation = ((KPlayer*)pCaster)->GetPlayerRelation(this);
        KG_PROCESS_ERROR(nRelation & sortEnemy);
        bRetCode = m_SelectTarget.SetTarget(pCaster);
        KGLOG_PROCESS_ERROR(bRetCode);

        eTargetType = ttPlayer;
    }
    else    
    {
        int nRelation = 0;
        pCaster = (KCharacter*)m_pOwner->m_Eyeshot.GetNpc(dwSkillSrcID);
        KG_PROCESS_ERROR(pCaster);
        nRelation = ((KNpc*)pCaster)->GetPlayerRelation(this);
        KG_PROCESS_ERROR(nRelation & sortEnemy);
        bRetCode = m_SelectTarget.SetTarget(pCaster);
        KGLOG_PROCESS_ERROR(bRetCode);

        eTargetType = ttNpc;
    }


    bResult = true;
Exit0:
    return bResult;
}

int KPlayer::CanRevive()
{
    return (int)(time(NULL) > m_ReviveParam.nLastSituReviveTime);
}

