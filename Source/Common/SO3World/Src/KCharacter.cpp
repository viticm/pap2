#include "stdafx.h"
#include "KScene.h"
#include "KAttrModifier.h"
#include "KSkill.h"
#include "KCraft.h"
#include "KSO3World.h"

#ifdef _SERVER
#include "KPlayerServer.h"
#include "KRelayClient.h"
#include "KLootList.h"
#endif

#ifdef _CLIENT
#include "KPlayerClient.h"
#include "SO3Represent/SO3GameWorldRepresentHandler.h"
#include "SO3UI/SO3GameWorldUIHandler.h"
#endif
 
// 这个值表示每DAMAGE_2_MANA_RATE点内力抵消1点伤害
#define DAMAGE_2_MANA_RATE          2

#define THERAPY_TO_THREAT           2

#define CORPSE_DISPPEAR_TIME	    (GAME_FPS * 60 * 2)

//每个人对Npc进行对话操作的时候
#define MAX_IDLE_FRAME_ON_DIALOGUE  (GAME_FPS * 20)

#define RESIST_FEAR_INTERVAL        (GAME_FPS * 2)

#define CRITICAL_JUMP_HEIGHT        XYPOINT_TO_ZPOINT(CELL_LENGTH * 3)

#define REPULSED_DECELERATION       (5 * VELOCITY_ZOOM_COEFFICIENT)

DWORD       KCharacter::KCharacterAIData::dwAIEventSrc                              = ERROR_ID;
int         KCharacter::KCharacterAIData::nAIEventParam                             = 0;
int         KCharacter::KCharacterAIData::nTargetGroupCount                         = 0;
KCharacter* KCharacter::KCharacterAIData::TargetGroup[AI_TARGET_GROUP_MAX_COUNT]    = {NULL};

KCharacter::KCharacter()
{
#ifdef _SERVER
    m_pBulletListHead = NULL;
#endif
}

KCharacter::~KCharacter()
{
}

BOOL KCharacter::Init(void)
{
	BOOL bRetCode = false;

	bRetCode = KSceneObject::Init();
	KGLOG_PROCESS_ERROR(bRetCode);

	m_nHeight = POINT_PER_ALTITUDE * 14;

	memset(m_szName, 0, sizeof(m_szName));
#ifdef _CLIENT
	memset(m_szTitle, 0, sizeof(m_szTitle));
#endif
    m_eGender                               = cgGay;
	m_eKind								    = ckInvalid;
	m_eRace								    = crHuman;
	m_dwForceID							    = 0;    // 默认值，为玩家初进游戏时的所属势力
    m_dwGuardForceID                        = 0;
	m_nLevel							    = 0;
	m_dwCharacterAction						= 0;
    m_dwModelID                             = 0;

#ifdef _SERVER
    m_bStealthy                             = false;
    m_dwOwner                               = ERROR_ID;
#endif

    // ---------- 移动相关 ------------------->
	m_eMoveState                            = cmsOnStand;

    m_bSlip                                 = false;
    m_nJumpLimitFrame                       = 0;

    m_nVelocityXY                           = 0;
    m_nDirectionXY                          = 0;
    m_nVelocityZ                            = 0;

	m_nDestX                                = 0;
    m_nDestY                                = 0;

    m_nConvergenceSpeed                     = 0;

	m_nRunSpeed                             = 0;
	m_nRunSpeedBase                         = 0;
	m_nMoveSpeedPercent                     = 0;
                                    
	m_nJumpCount                            = 0;
	m_nMaxJumpCount                         = 1;

    m_nCurrentGravity                       = DEFAULT_GRAVITY;
	m_nGravity                              = DEFAULT_GRAVITY;
    m_nGravityBase                          = DEFAULT_GRAVITY;
    m_nGravityPercent                       = 0;
    m_nDropDefence                          = 0;

    m_nJumpSpeed                            = 0;     
    m_nJumpSpeedBase                        = 0;
    m_nJumpSpeedPercent                     = 0;

    m_nDivingCount                          = 0;
    m_nDivingFrame                          = GAME_FPS * 60;         
    m_nDivingFrameBase                      = GAME_FPS * 60;
    m_nDivingFramePercent                   = 0; 

    m_nWaterFlyAbility                      = 0;  

    m_bTryPathState                         = false;
    m_nTryPathSide                          = 0;
    m_nTryPathDestX                         = 0;
    m_nTryPathDestY                         = 0;

    m_nCurrentTrack                         = 0;
    m_nFromNode                             = 0;
    m_nTargetCity                           = 0;

#ifdef _SERVER
    m_bAutoTurnFlag                         = false;
#endif

	// ---------- 潜能 ----------------------->
    m_nCurrentStrength		                = 0;		
    m_nStrengthBase                         = 0;
    m_nStrengthBasePercentAdd               = 0;

    m_nCurrentAgility		                = 0;				
    m_nAgilityBase                          = 0;
    m_nAgilityBasePercentAdd                = 0;

    m_nCurrentVitality		                = 0;
    m_nVitalityBase                         = 0;
    m_nVitalityBasePercentAdd               = 0;

    m_nCurrentSpirit		                = 0;
    m_nSpiritBase                           = 0;
    m_nSpiritBasePercentAdd                 = 0;

    m_nCurrentSpunk                         = 0;                
    m_nSpunkBase                            = 0;
    m_nSpunkBasePercentAdd                  = 0;

	// ---------- 生命 ----------------------->
	m_nCurrentLife                          = INT_MAX / 2;
	m_nMaxLife                              = 0;
	m_nMaxLifeBase                          = 0;
	m_nMaxLifePercentAdd                    = 0;
	m_nMaxLifeAdditional                    = 0;
	m_nLifeReplenish                        = 0;
	m_nLifeReplenishPercent                 = 0;
    m_nLifeReplenishCoefficient             = 0;
    m_nLifeReplenishExt                     = 0;

	// ---------- 内力 ----------------------->
	m_nCurrentMana                          = INT_MAX / 2;
	m_nMaxMana                              = 0;
	m_nMaxManaBase                          = 0;
	m_nMaxManaPercentAdd                    = 0;
	m_nMaxManaAdditional                    = 0;
	m_nManaReplenish                        = 0;
	m_nManaReplenishPercent                 = 0;
    m_nManaReplenishCoefficient             = 0;
    m_nManaReplenishExt                     = 0;

    // ---------- 怒气 ----------------------->
    m_nCurrentRage                          = 0;
    m_nMaxRage                              = 0;
    m_nRageReplenish                        = 0;

    // ---------- 杂项 ----------------------->
	m_nDodge                                = 0;
    m_nDodgeBaseRate                        = 0;
	m_bFightState                           = false;
	m_bSheathFlag                           = true;
    m_bPositiveShield                       = false;     
    m_bNegativeShield                       = false;
    m_bSystemShield                         = false;
	m_nGlobalBlock                          = 0;
	m_nGlobalDamageAbsorb                   = 0;     
	m_nGlobalDamageManaShield               = 0; 

    m_nParryBaseRate                        = 0;
    m_nParry                                = 0;
    m_nParryBase                            = 0;
    m_nParryPercent                         = KILO_NUM;
    m_nParryValue                           = 0;
    m_nParryValueBase                       = 0;
    m_nParryValuePercent                    = KILO_NUM;
    m_nSense                                = 0;
    m_nStrain                               = 0;
    m_nStrainBase                           = 0;
    m_nStrainPercent                        = KILO_NUM;
    m_nToughnessBaseRate                    = 0;
    m_nToughness                            = 0;
    m_nToughnessBase                        = 0;
    m_nToughnessPercent                     = KILO_NUM;

    m_nDecriticalDamagePowerBaseKiloNumRate = 0;
    m_nDecriticalDamagePower                = 0;
    m_nDecriticalDamagePowerBase            = 0;
    m_nDecriticalDamagePowerPercent         = KILO_NUM;
    
    m_nTransferDamageToManaPercent          = 0;

    m_nModifyCostManaPercent                = KILO_NUM;

    m_dwAddDamageByDstMoveStateMask         = 0;
    m_nAddDamagePercentByDstMoveState       = 0;

    m_dwTransferDamageToCharacter           = 0;
    m_nTransferDamagePercent                = 0;

    m_dwTransferTherapyToCharacter          = 0;
    m_nTransferTherapyPercent               = 0;

    m_bImmortal                             = false;
    m_nMountingRate                         = 0;
    m_nAccumulateValue                      = 0;

	m_nDamageToLifeForSelf                  = 0;
	m_nDamageToManaForSelf                  = 0;

	m_nBeatBackRate                         = KILO_NUM;
	m_nStunStrikeRate                       = 0;
	m_nKnockedBackRate                      = KILO_NUM; // 默认 100% 击退。
	m_nKnockedOffRate                       = KILO_NUM; // 默认 100% 击飞。
    m_nKnockedDownRate                      = KILO_NUM; // 默认 100% 击倒
    m_nRepulsedRate                         = KILO_NUM; // 默认 100% 击滑步
    m_nPullRate                             = KILO_NUM; // 默认 100% 被拉

    // -->[破绽]
#ifdef _SERVER
    m_nUpperWeak                            = 0;
    m_nMiddleWeak                           = 0;
    m_nLowerWeak                            = 0;
#endif

    m_nUpperWeakShield                      = 0;
    m_nMiddleWeakShield                     = 0;
    m_nLowerWeakShield                      = 0;

    m_nUpperWeakLevel                      = swlNone;
    m_nMiddleWeakLevel                     = swlNone;
    m_nLowerWeakLevel                      = swlNone;
    // [破绽]<--

	// -------------- 外功 ------------------------------------------------------>

	m_nMeleeWeaponDamageBase                = 0;
	m_nMeleeWeaponDamageRand                = 0;

	m_nRangeWeaponDamageBase                = 0;
	m_nRangeWeaponDamageRand                = 0;

	m_nSkillPhysicsDamage                   = 0;
    m_nSkillPhysicsDamageRand               = 0;
	m_nSkillPhysicsDamagePercent            = 0;
	m_nPhysicsDamageCoefficient             = 0;

    m_nPhysicsAttackPower                   = 0;
    m_nPhysicsAttackPowerBase               = 0;
    m_nPhysicsAttackPowerPercent            = KILO_NUM;

    m_nPhysicsHitBaseRate                   = 0;
    m_nPhysicsHitValue                      = 0;
    m_nPhysicsCriticalStrike                = 0;
    m_nPhysicsCriticalStrikeBase            = 0;
    m_nPhysicsCriticalStrikeBaseRate        = 0;


    m_nPhysicsCriticalDamagePowerBaseKiloNumRate   = 0;
    m_nPhysicsCriticalDamagePower                  = 0;
    m_nPhysicsCriticalDamagePowerBase              = 0;
    m_nPhysicsCriticalDamagePowerPercent           = KILO_NUM;

    m_nPhysicsOvercome                      = 0;
    m_nPhysicsOvercomeBase                  = 0;
    m_nPhysicsOvercomePercent               = KILO_NUM;

	m_nPhysicsResistPercent                 = 0;	

	m_nPhysicsDefenceAdd                    = 0;       
	m_nPhysicsDefenceMax                    = 0;	

	m_nPhysicsShield                        = 0; 
	m_nPhysicsShieldBase                    = 0;      
	m_nPhysicsShieldPercent                 = 0;
	m_nPhysicsShieldAdditional              = 0;	

	m_nPhysicsReflection                    = 0;		
	m_nPhysicsReflectionPercent             = 0;

	m_nPhysicsBlock                         = 0;           

	m_nPhysicsDamageAbsorb                  = 0;    
	m_nPhysicsDamageManaShield              = 0; 

	// ------------- 阳性内功 ----------------------------->
	m_nSkillSolarDamage                     = 0;
    m_nSkillSolarDamageRand                 = 0;
	m_nSkillSolarDamagePercent              = 0;
	m_nSolarDamageCoefficient               = 0;

    m_nSolarAttackPower                     = 0;
    m_nSolarAttackPowerBase                 = 0;
    m_nSolarAttackPowerPercent              = KILO_NUM;

    m_nSolarHitBaseRate                     = 0;               
    m_nSolarHitValue                        = 0;               
    m_nSolarCriticalStrike                  = 0;         
    m_nSolarCriticalStrikeBase              = 0;         
    m_nSolarCriticalStrikeBaseRate          = 0;         

    m_nSolarCriticalDamagePowerBaseKiloNumRate   = 0;
    m_nSolarCriticalDamagePower                  = 0;
    m_nSolarCriticalDamagePowerBase              = 0;
    m_nSolarCriticalDamagePowerPercent           = KILO_NUM;

    m_nSolarOvercome                        = 0;
    m_nSolarOvercomeBase                    = 0;
    m_nSolarOvercomePercent                 = KILO_NUM;

	m_nSolarMagicResistPercent              = 0;
    m_nSolarMagicShield                     = 0;		
    m_nSolarMagicShieldBase                 = 0;		
    m_nSolarMagicShieldPercent              = KILO_NUM;		

	m_nSolarMagicReflection                 = 0;		
	m_nSolarMagicReflectionPercent          = 0;	

	m_nSolarMagicBlock                      = 0;             

	m_nSolarDamageAbsorb                    = 0;     
	m_nSolarDamageManaShield                = 0; 

	// ------------- 中性内功 ----------------------------->
	m_nSkillNeutralDamage                   = 0;
    m_nSkillNeutralDamageRand               = 0;
	m_nSkillNeutralDamagePercent            = 0;	
	m_nNeutralDamageCoefficient             = 0;

    m_nNeutralAttackPower                   = 0;
    m_nNeutralAttackPowerBase               = 0;
    m_nNeutralAttackPowerPercent            = KILO_NUM;

    m_nNeutralHitBaseRate                   = 0;               
    m_nNeutralHitValue                      = 0;               
    m_nNeutralCriticalStrike                = 0;        
    m_nNeutralCriticalStrikeBase            = 0;        
    m_nNeutralCriticalStrikeBaseRate        = 0;        

    m_nNeutralCriticalDamagePowerBaseKiloNumRate   = 0;
    m_nNeutralCriticalDamagePower                  = 0;
    m_nNeutralCriticalDamagePowerBase              = 0;
    m_nNeutralCriticalDamagePowerPercent           = KILO_NUM;

    m_nNeutralOvercome                      = 0;
    m_nNeutralOvercomeBase                  = 0;
    m_nNeutralOvercomePercent               = KILO_NUM;

	m_nNeutralMagicResistPercent            = 0;

    m_nNeutralMagicShield                   = 0;
    m_nNeutralMagicShieldBase               = 0;
    m_nNeutralMagicShieldPercent            = KILO_NUM;

	m_nNeutralMagicReflection               = 0;		
	m_nNeutralMagicReflectionPercent        = 0;	

	m_nNeutralMagicBlock                    = 0;             

	m_nNeutralDamageAbsorb                  = 0;     
	m_nNeutralDamageManaShield              = 0; 

	// ------------- 阴性内功 ------------------------------->
	m_nSkillLunarDamage                     = 0;
    m_nSkillLunarDamageRand                 = 0;
	m_nSkillLunarDamagePercent              = 0;	
	m_nLunarDamageCoefficient               = 0;

    m_nLunarAttackPower                     = 0;
    m_nLunarAttackPowerBase                 = 0;
    m_nLunarAttackPowerPercent              = KILO_NUM;

    m_nLunarHitBaseRate                     = 0;               
    m_nLunarHitValue                        = 0;               
    m_nLunarCriticalStrike                  = 0;         
    m_nLunarCriticalStrikeBase              = 0;         
    m_nLunarCriticalStrikeBaseRate          = 0;         

    m_nLunarCriticalDamagePowerBaseKiloNumRate   = 0;
    m_nLunarCriticalDamagePower                  = 0;
    m_nLunarCriticalDamagePowerBase              = 0;
    m_nLunarCriticalDamagePowerPercent           = KILO_NUM;

    m_nLunarOvercome                        = 0;
    m_nLunarOvercomeBase                    = 0;
    m_nLunarOvercomePercent                 = KILO_NUM;

	m_nLunarMagicResistPercent              = 0;

    m_nLunarMagicShield                     = 0;
    m_nLunarMagicShieldBase                 = 0;
    m_nLunarMagicShieldPercent              = KILO_NUM;

	m_nLunarMagicReflection                 = 0;		
	m_nLunarMagicReflectionPercent          = 0;	

	m_nLunarMagicBlock                      = 0;             

	m_nLunarDamageAbsorb                    = 0;     
	m_nLunarDamageManaShield                = 0; 


	// ------------- 毒性 ------------------------------------>

	m_nSkillPoisonDamage                    = 0;
    m_nSkillPoisonDamageRand                = 0;
	m_nSkillPoisonDamagePercent             = 0;	
	m_nPoisonDamageCoefficient              = 0;

    m_nPoisonAttackPower                    = 0;
    m_nPoisonAttackPowerBase                = 0;
    m_nPoisonAttackPowerPercent             = KILO_NUM;

    m_nPoisonHitBaseRate                    = 0;               
    m_nPoisonHitValue                       = 0;               
    m_nPoisonCriticalStrike                 = 0;         
    m_nPoisonCriticalStrikeBase             = 0;         
    m_nPoisonCriticalStrikeBaseRate         = 0;         

    m_nPoisonCriticalDamagePowerBaseKiloNumRate   = 0;
    m_nPoisonCriticalDamagePower                  = 0;
    m_nPoisonCriticalDamagePowerBase              = 0;
    m_nPoisonCriticalDamagePowerPercent           = KILO_NUM;

    m_nPoisonOvercome                       = 0;
    m_nPoisonOvercomeBase                   = 0;
    m_nPoisonOvercomePercent                = KILO_NUM;

	m_nPoisonMagicResistPercent             = 0;

    m_nPoisonMagicShield                    = 0;
    m_nPoisonMagicShieldBase                = 0;
    m_nPoisonMagicShieldPercent             = KILO_NUM;

	m_nPoisonMagicReflection                = 0;		
	m_nPoisonMagicReflectionPercent         = 0;	

    m_nPoisonMagicBlock                     = 0;             

    m_nPoisonDamageAbsorb                   = 0;     
    m_nPoisonDamageManaShield               = 0; 

	// ----------- 治疗(生命回复) ---------------------------->
    m_nTherapyPower                         = 0;
    m_nTherapyPowerBase                     = 0;
    m_nTherapyPowerPercent                  = KILO_NUM;
	m_nSkillTherapy                         = 0;
    m_nSkillTherapyRand                     = 0;
	m_nSkillTherapyPercent                  = 0; 
	m_nTherapyCoefficient                   = 0;
    m_nBeTherapyCoefficient                 = KILO_NUM;
    
	// ---------------------------------------

	m_nNoneWeaponAttackSpeedBase            = 0;
	m_nMeleeWeaponAttackSpeed               = 0;
	m_nMeleeWeaponAttackSpeedBase           = 0;
	m_nMeleeWeaponAttackSpeedPercentAdd     = 0;
	m_nMeleeWeaponAttackSpeedAdditional     = 0;

	m_nRangeWeaponAttackSpeed               = 0;
	m_nRangeWeaponAttackSpeedBase           = 0;
	m_nRangeWeaponAttackSpeedPercentAdd     = 0;
	m_nRangeWeaponAttackSpeedAdditional     = 0;

    m_nIgnoreOtherAlarmRange                = 0; 
    m_nSelfAlarmRangeRevise                 = 0;  
    m_nOtherAlarmRangeRevise                = 0; 
    m_nResistBehaveConstraintPercent        = 0;
    m_nUnarmCounter                         = 0;

    memset(m_nSilenceRefCounter, 0, sizeof(m_nSilenceRefCounter));
    memset(m_byRepresentState, 0, sizeof(m_byRepresentState));

    m_ImmunityFlag.reset();

    m_dwSkillCastFlag                       = 0;

    m_bOnTalk                               = false;

    m_nFearCounter                          = 0;
	m_nMoveFrameCounter                     = 0;
      
#ifdef _CLIENT
	m_ThreatList.Init(this);
#endif

    m_nMoveSyncFrame                        = 0;
    m_bMoveSyncSelf                         = false;

#ifdef _SERVER
    m_SimpThreatList.Init(this);

	m_nThreatTime                           = GAME_FPS * 60;

    m_nActiveBaseThreatCoefficient          = KILO_NUM;
    m_nActiveDamageThreatCoefficient        = KILO_NUM;
    m_nActiveTherapyThreatCoefficient       = KILO_NUM;
    m_nPassiveBaseThreatCoefficient         = KILO_NUM;
    m_nPassiveDamageThreatCoefficient       = KILO_NUM;
    m_nPassiveTherapyThreatCoefficient      = KILO_NUM;

    m_nOverThreatPercent                    = 120;

    m_bToDie                                = false;
    m_dwKillerID                            = ERROR_ID;

    m_dwBuffEffectSoleTargetID              = ERROR_ID;

    assert(!m_pBulletListHead);
    m_SkillEventVector.clear();
#endif //_SERVER

    m_nBattleFieldSide                      = INVALID_BATTLE_SIDE;

    m_AIData.nAIType                        = 0;

    m_AIData.nOriginPosX                    = -1;
    m_AIData.nOriginPosY                    = -1;
    m_AIData.nOriginPosZ                    = -1;
    m_AIData.nOriginDirection               = -1;
    
    m_AIData.nReturnPosX                    = -1;
    m_AIData.nReturnPosY                    = -1;
    m_AIData.nReturnPosZ                    = -1;

    m_AIData.nPatrolPathID                  = 0;
    m_AIData.pPatrolPath                    = NULL;
    m_AIData.nPatrolOrderIndex              = 0;

    m_AIData.nEscapeCount                   = 0;
    m_AIData.nCannotAttactCount             = 0;
    m_AIData.nSkillFailedCount              = 0;
    m_AIData.nSkillType                     = 0;
    m_AIData.dwRespondCharacterID           = 0;
    m_AIData.nAITotalDamageValue            = 0;
    m_AIData.dwProtectCampMark              = 0;
    m_AIData.dwProtectForceMark             = 0;
    m_AIData.bSpecialSelectTarget           = false;

    memset(m_AIData.nRecordSkillID, 0, sizeof(m_AIData.nRecordSkillID));
    memset(m_AIData.nRecordSkillLevel, 0, sizeof(m_AIData.nRecordSkillLevel));

    for (int i = 0; i < aittTotal; i++)
        m_AIData.ChoiceTarget[i].ClearTarget();

	m_pSkillAttributeParam                  = NULL;
    m_pSrcSkillCalculateResult              = NULL;
    m_pDstSkillCalculateResult              = NULL;

    m_nSkillStackDepth                      = 0;
	
    m_BuffList.Init(this);

	m_nNextReplenishFrame                   = 0;

	memset(&m_OTActionParam, 0, sizeof(m_OTActionParam));

	memset(&m_AutoCastSkillParam, 0, sizeof(m_AutoCastSkillParam));

    m_dwDirectCastMask                      = 0;

#ifdef _SERVER
    m_nLastBroadcastLife                    = -1;
#endif

    m_SelectTarget.ClearTarget();
    m_SkillTarget.ClearTarget();

    m_SkillDebug.Init(this);

	return true;
Exit0:

	return false;
}

void KCharacter::UnInit()
{
#ifdef _SERVER
    ClearBullet();
#endif

    m_BuffList.UnInit();

#ifdef _CLIENT
    m_ThreatList.UnInit();
#endif

#ifdef _SERVER
    m_SimpThreatList.UnInit();
    m_SkillEventVector.clear();
#endif
    m_SkillDebug.UnInit();

    memset(&m_AIData, 0, sizeof(m_AIData));

	KSceneObject::UnInit();
}

void KCharacter::Activate(void)
{
	BOOL bRetCode = 0;

    if (m_eMoveState != cmsOnDeath)
    {
        m_AIVM.Active();

        // m_pScene指针为空表示自己被删除了
        KG_PROCESS_ERROR(m_pScene);
    }

#if defined(_SERVER)
	ProcessAutoCastSkill();
#endif
	// 处理时间持续行为
	ProcessOTAction();

    m_BuffList.Activate();

    if (m_dwCharacterAction != 0)
    {
	    if (m_OTActionParam.eActionType != otActionIdle || m_eMoveState != cmsOnStand)
	    {
		    m_dwCharacterAction = 0;
	    }
    }

	CheckFightFlag();

#if defined(_SERVER)
    m_SimpThreatList.Activate();

	CheckBullet();

    CheckDie();

	ReviseAttribValue();
#endif	//_SERVER

	RunStateMachine();
    // m_pScene指针为空表示自己被删除了
    KG_PROCESS_ERROR(m_pScene);

	ProcessMove();
    // m_pScene指针为空表示自己被删除了
    KG_PROCESS_ERROR(m_pScene);

#ifdef _SERVER
    if (m_nMoveSyncFrame != 0 && g_pSO3World->m_nGameLoop >= m_nMoveSyncFrame)
    {
        g_PlayerServer.DoSyncMoveState(this, m_bMoveSyncSelf);
        m_bMoveSyncSelf  = false;
        m_nMoveSyncFrame = 0;
    }
#endif

    CheckFear();
	
Exit0:
	return;
}

// 处理角色的状态机
void KCharacter::RunStateMachine()
{
	switch (m_eMoveState)
	{
	case cmsOnStand:
		OnStand();
		break;
    case cmsOnSit:
        OnSit();
        break;
	case cmsOnWalk:
		OnWalk();
		break;
	case cmsOnRun:
		OnRun();
		break;
    case cmsOnFloat:
        break;
    case cmsOnSwim:
        break;
	case cmsOnDash:
        OnDash();
        break;
    case cmsOnPull:
        OnPull();
        break;
	case cmsOnKnockedDown:
	case cmsOnKnockedBack:
	case cmsOnKnockedOff:
		OnPassiveMove();
		break;
	case cmsOnHalt:
		OnHalt();
        break;
    case cmsOnFreeze:
        OnFreeze();
        break;
    case cmsOnEntrap:
        OnEntrap();
        break;
	case cmsOnDeath:
		OnDeath();
		break;
    case cmsOnJump:
        OnJump();
        break;
    case cmsOnSwimJump:
        OnSwimJump();
        break;
    case cmsOnAutoFly:
        break;
    case cmsOnRepulsed:
        OnRepulsed();
        break;
    case cmsOnRise:
        OnRise();
        break;
    case cmsOnSkid:
        OnSkid();
        break;
	default:
		assert(false);
		break;
	}

}

BOOL KCharacter::GetRepresentState(KBUFF_REPRESENT_POS ePos, DWORD& rdwRepresentID)
{
    BOOL bResult = false;

    KGLOG_PROCESS_ERROR(ePos == brpTrample || ePos == brpBody || ePos == brpTopHead);

    rdwRepresentID = m_byRepresentState[ePos];

    bResult = true;
Exit0:
	return bResult;
}


#ifdef _CLIENT
BOOL KCharacter::PrepareSkill(DWORD dwSkillID, DWORD dwSkillLevel, KTarget& rTarget, int nPrepareFrame, int EndFrame)
{
	BOOL                        bResult    = false;
    BOOL                        bRetCode   = false;
	KSkill*                     pSkill     = NULL;
    KSKILL_RECIPE_KEY           SkillRecipeKey;
    KSkillRecipePointer         SkillRecipePointer;
	
    bRetCode = g_pSO3World->m_SkillManager.GetSkillRecipeKey(&SkillRecipeKey, dwSkillID, dwSkillLevel, this);
    KGLOG_PROCESS_ERROR(bRetCode);

    pSkill = SkillRecipePointer.SetRecipeKey(SkillRecipeKey);
	KGLOG_PROCESS_ERROR(pSkill);

	m_SkillTarget = rTarget;

	DoSkillPrepare(SkillRecipePointer, nPrepareFrame, EndFrame);

    if (g_pGameWorldUIHandler)
    {
        if (m_dwID == g_pSO3World->m_dwClientPlayerID)
        {
            KUIEventSkillProgress    Param;
            // UI: 显示通道技进度条
            Param.nTotalFrame  = nPrepareFrame;
            Param.dwSkillID    = dwSkillID;
            Param.dwSkillLevel = dwSkillLevel;
            g_pGameWorldUIHandler->OnSkillPrepareProgress(Param);
        }

        KUIEventSkillCastLog CastLog;
        CastLog.dwCaster    = m_dwID;
        CastLog.dwSkillID   = dwSkillID;
        CastLog.dwLevel     = dwSkillLevel;
        g_pGameWorldUIHandler->OnSkillCastLog(CastLog);
    }

	bResult = true;
Exit0:
	return bResult;
}
#endif

#if defined(_SERVER)
SKILL_RESULT_CODE KCharacter::CastSkill(DWORD dwSkillID, DWORD dwSkillLevel, KTarget& rTarget)
{
	SKILL_RESULT_CODE   nResult           = srcFailed;
	int                 nRetCode          = false;
	KSkill*             pSkill            = NULL;
	int                 nPrepareFrame     = 0;
    KCharacter*         pEventTarget      = NULL;
    KCharacter*         pTarget           = NULL;
    KSKILL_RECIPE_KEY   RecipeKey;
    KSkillRecipePointer SkillRecipePointer;

    nRetCode = g_pSO3World->m_SkillManager.GetSkillRecipeKey(&RecipeKey, dwSkillID, dwSkillLevel, this);
    KGLOG_PROCESS_ERROR(nRetCode);

    pSkill = SkillRecipePointer.SetRecipeKey(RecipeKey);
    KG_PROCESS_ERROR_RET_CODE(pSkill, srcInvalidSkill);

    KGLOG_PROCESS_ERROR(pSkill->m_pBaseInfo->nCastMode != scmItem); // 对道具使用的技能不能走到该流程。

//     if (m_eMoveState == cmsOnDash)
//     {
//         Stop();
//     }

    // 如果正在释放通道技，则直接中断
    if (
        m_OTActionParam.eActionType == otActionSkillChannel ||
        m_OTActionParam.eActionType == otActionPicking
    )
    {
        g_PlayerServer.DoSkillBeatBack(this, 0, true);
        DoActionIdle();
    }

    if (m_OTActionParam.eActionType != otActionIdle)
    {
        if (IS_PLAYER(m_dwID))
        {
            KPlayer* pPlayer = (KPlayer*)this;
            g_PlayerServer.DoMessageNotify(pPlayer->m_nConnIndex, ectSkillErrorCode, srcInOTAction);
        }
        goto Exit0;
    }

	nRetCode = pSkill->CanCast(this, rTarget);
	KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, (SKILL_RESULT_CODE)nRetCode);

	m_SkillTarget   = rTarget;

    if (IS_PLAYER(m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)this;
        pSkill->ResetPublicCooldown(pPlayer);
    }

    // 检测技能是否需要自动转向
    if (pSkill->IsAutoTurn())
    {
        m_bAutoTurnFlag = true;
    }

    nPrepareFrame   = pSkill->m_nPrepareFrames;

	// 检测技能类型是否为瞬发
	if (nPrepareFrame > 0 && !(m_dwDirectCastMask & pSkill->m_pBaseInfo->dwCastMask))
	{
		nRetCode = g_PlayerServer.DoSkillPrepare(this, dwSkillID, dwSkillLevel, nPrepareFrame, m_SkillTarget);
		KGLOG_CHECK_ERROR(nRetCode);

		DoSkillPrepare(SkillRecipePointer, nPrepareFrame, g_pSO3World->m_nGameLoop + nPrepareFrame);
        goto Exit1;
	}

	pSkill->DoCost(this);

	nRetCode = IS_PLAYER(m_dwID);
	if (nRetCode)
	{
		KPlayer* pPlayer = (KPlayer*)this;
		pSkill->ResetNormalCooldown(pPlayer);
	}

	if (pSkill->m_pBaseInfo->bIsChannelSkill)
	{
		int nChannelFrame = pSkill->m_nChannelFrame;
		// ...
		DoSkillChannel(SkillRecipePointer, nChannelFrame);
	}

	nRetCode = g_PlayerServer.DoSkillCast(this, pSkill, m_SkillTarget, false);
	KGLOG_CHECK_ERROR(nRetCode);

    KG_PROCESS_SUCCESS(m_OTActionParam.eActionType == otActionSkillChannel);
    
	nResult = pSkill->Cast(this, this, rTarget, RecipeKey);
	KGLOG_PROCESS_ERROR(nResult == srcSuccess);

    // 技能释放事件(瞬发)

    switch (m_SkillTarget.GetTargetType())
    {
    case ttPlayer:
    case ttNpc:
        nRetCode = m_SkillTarget.GetTarget(&pEventTarget);
        KGLOG_PROCESS_ERROR(nRetCode);
        break;
    default:
        break;
    }

    m_AIVM.FireEvent(aevOnCastSkill, m_dwID, 0);

    OnSkillEvent(pSkill, seCast, m_SkillEventVector, this, pEventTarget);

    if (pEventTarget)
    {
        pEventTarget->OnSkillEvent(pSkill, seBeCast, pEventTarget->m_SkillEventVector, pEventTarget, this);
    }

Exit1:
	nResult = srcSuccess;
Exit0:
    if (nResult != srcSuccess)
    {
        m_AIVM.FireEvent(aevOnCastSkillFailed, m_dwID, 0);
    }
	return nResult;
}

SKILL_RESULT_CODE KCharacter::CastSkillSub(KCharacter* pLogicCaster, DWORD dwSkillID, DWORD dwSkillLevel, KTarget& rTarget)
{
	int               nResult           = srcFailed;
	int               nRetCode          = false;
	KSkill*           pSkill            = NULL;
	int               nPrepareFrame     = 0;
	KTarget           TargetBak         = m_SkillTarget;
    KSKILL_RECIPE_KEY RecipeKey;

    assert(pLogicCaster);

	// 获得技能指针

    nRetCode = g_pSO3World->m_SkillManager.GetSkillRecipeKey(&RecipeKey, dwSkillID, dwSkillLevel, this);
    KGLOG_PROCESS_ERROR(nRetCode);

    pSkill = g_pSO3World->m_SkillManager.GetSkill(RecipeKey);
	KG_PROCESS_ERROR_RET_CODE(pSkill, srcInvalidSkill);

    KGLOG_PROCESS_ERROR(pSkill->m_pBaseInfo->nCastMode != scmItem); // 对道具使用的技能不能走到该流程。

	// 切换目标和记录技能
	m_SkillTarget   = rTarget;

    if (pSkill->m_pBaseInfo->nCastMode == scmTargetLeader && IS_PLAYER(m_dwID))
    {
        KPlayer*    pPlayer     = (KPlayer*)pLogicCaster;
        DWORD       dwTeamID    = pPlayer->GetCurrentTeamID();
        KTeam*      pTeam       = NULL;
        int         nGroupIndex = 0;

        pTeam = g_pSO3World->m_TeamServer.GetTeam(dwTeamID);
        KGLOG_PROCESS_ERROR(pTeam);

        nGroupIndex = pPlayer->GetCurrentTeamGroupIndex();
        KGLOG_PROCESS_ERROR(nGroupIndex >= 0 && nGroupIndex < pTeam->nGroupNum);

        m_SkillTarget.SetTarget(ttPlayer, pTeam->MemberGroup[nGroupIndex].dwFormationLeader);
    }

    // 
	if (IS_PLAYER(pLogicCaster->m_dwID))
	{
		KPlayer* pPlayer = (KPlayer*)pLogicCaster; 
		nRetCode = pSkill->CheckWeaponRequest(pPlayer);
		KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);

		nRetCode = pSkill->CheckMountRequest(pPlayer);
		KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);
	}

	nRetCode = pSkill->CheckSelfState(pLogicCaster);
	KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);

    nRetCode = pSkill->CheckSelfBuff(pLogicCaster);
    KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);

    if (
        pSkill->m_pBaseInfo->nCastMode == scmTargetArea     ||
        pSkill->m_pBaseInfo->nCastMode == scmTargetChain    ||
        pSkill->m_pBaseInfo->nCastMode == scmTargetSingle   ||
        pSkill->m_pBaseInfo->nCastMode == scmTargetLeader
    )
    {
	    nRetCode = pSkill->CheckTargetState(m_SkillTarget);
	    KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);

        nRetCode = pSkill->CheckTargetType(this, m_SkillTarget);
        KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);

        nRetCode = pSkill->CheckTargetRange(this, m_SkillTarget);
        KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);

        nRetCode = pSkill->CheckTargetBuff(m_SkillTarget);
	    KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);
    }

    // Cast skill
	nRetCode = g_PlayerServer.DoSkillCast(this, pSkill, m_SkillTarget, true);
	KGLOG_CHECK_ERROR(nRetCode);

	nResult = pSkill->Cast(this, pLogicCaster, rTarget, RecipeKey);
	KGLOG_PROCESS_ERROR(nResult == srcSuccess);

    // 技能释放事件(子技能)
    {
        BOOL        bRetCode     = false;
        KCharacter* pEventTarget = NULL;

        switch (m_SkillTarget.GetTargetType())
        {
        case ttPlayer:
        case ttNpc:
            bRetCode = m_SkillTarget.GetTarget(&pEventTarget);
            KGLOG_PROCESS_ERROR(bRetCode);
            break;
        }

        if (pLogicCaster)
        {
            pLogicCaster->OnSkillEvent(pSkill, seCast, m_SkillEventVector, this, pEventTarget);
        }

        if (pEventTarget)
        {
            pEventTarget->OnSkillEvent(pSkill, seBeCast, pEventTarget->m_SkillEventVector, pEventTarget, this);
        }
    }

	nResult = srcSuccess;
Exit0:
    if (nResult != srcSuccess)
    {
        m_AIVM.FireEvent(aevOnCastSkillFailed, m_dwID, 0);
    }
	m_SkillTarget   = TargetBak;
	return (SKILL_RESULT_CODE)nResult;
}

SKILL_RESULT_CODE KCharacter::CastSkillAccumulate(DWORD dwSkillID, DWORD dwSkillLevel, KTarget& rTarget)
{
    int                 nResult             = srcFailed;
    int                 nRetCode            = false;
    int                 nInterval           = 0;
    KSkill*             pSkill              = NULL;
    DWORD               dwSubSkillID        = 0;
    DWORD               dwSubSkillLevel     = 0;
    KSKILL_RECIPE_KEY   RecipeKey;
    KSkillRecipePointer SkillRecipePointer;

    nRetCode = g_pSO3World->m_SkillManager.GetSkillRecipeKey(&RecipeKey, dwSkillID, dwSkillLevel, this);
    KGLOG_PROCESS_ERROR(nRetCode);

    pSkill = SkillRecipePointer.SetRecipeKey(RecipeKey);
    KGLOG_PROCESS_ERROR(pSkill);

	KG_PROCESS_ERROR_RET_CODE(pSkill->m_bIsAccumulate, srcInvalidSkill);

    KG_PROCESS_ERROR_RET_CODE(m_nAccumulateValue > 0, srcNotStartAccumulate);

    nInterval = m_nAccumulateValue - 1;
    assert(nInterval >= 0 && nInterval < MAX_ACCUMULATE_SUB_COUNT);

    dwSubSkillID = pSkill->m_SubsectionSkill[nInterval].dwSubSkillID;
    dwSubSkillLevel = pSkill->m_SubsectionSkill[nInterval].dwSubSkillLevel;

    m_nAccumulateValue = 0;

    if (IS_PLAYER(m_dwID))
    {
        g_PlayerServer.DoSyncAccumulateValue((KPlayer*)this);
    }

    nRetCode = CastSkill(dwSubSkillID, dwSubSkillLevel, rTarget);
    KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);

    nResult = srcSuccess;
Exit0:
    return (SKILL_RESULT_CODE)nResult;
}
#endif

#ifdef _CLIENT

SKILL_RESULT_CODE KCharacter::CastSkill(DWORD dwSkillID, DWORD dwSkillLevel, KTarget& rTarget, DWORD dwBulletID)
{
	SKILL_RESULT_CODE               nResult           = srcFailed;
	int                             nRetCode          = false;
	KSkill*                         pSkill            = NULL;
    KSKILL_RECIPE_KEY               RecipeKey;
    KSkillRecipePointer             SkillRecipePointer;

	// 获得技能指针
    nRetCode = g_pSO3World->m_SkillManager.GetSkillRecipeKey(&RecipeKey, dwSkillID, dwSkillLevel, this);
    KGLOG_PROCESS_ERROR(nRetCode);

    pSkill = SkillRecipePointer.SetRecipeKey(RecipeKey);
    KGLOG_PROCESS_ERROR(pSkill);

    KGLOG_PROCESS_ERROR(pSkill->m_pBaseInfo->nCastMode != scmItem); // 对道具使用的技能不能走到该流程。

	// 切换目标和记录技能
	m_SkillTarget   = rTarget;

	pSkill->Cast(this, this, rTarget, RecipeKey);

	if (pSkill->m_pBaseInfo->bIsChannelSkill)
	{
		int nChannelFrame = pSkill->m_nChannelFrame;
		// ... ...
		DoSkillChannel(SkillRecipePointer, nChannelFrame);

        if (g_pGameWorldUIHandler)
        {
            if (m_dwID == g_pSO3World->m_dwClientPlayerID)
            {
                KUIEventSkillProgress    Param;
                // UI: 显示通道技进度条
                Param.nTotalFrame  = nChannelFrame;
                Param.dwSkillID    = dwSkillID;
                Param.dwSkillLevel = dwSkillLevel;
                g_pGameWorldUIHandler->OnSkillChannelProgress(Param);
            }

            KUIEventSkillCastLog CastLog;
            CastLog.dwCaster    = m_dwID;
            CastLog.dwSkillID   = dwSkillID;
            CastLog.dwLevel     = dwSkillLevel;
            g_pGameWorldUIHandler->OnSkillCastLog(CastLog);
        }
	}

    if (g_pGameWorldRepresentHandler)
        g_pGameWorldRepresentHandler->OnCharacterCastSkill(this, &rTarget, pSkill, dwBulletID, true);

    if (g_pGameWorldUIHandler)
    {
        KUIEventSkillCastLog CastLog;
        CastLog.dwCaster    = m_dwID;
        CastLog.dwSkillID   = dwSkillID;
        CastLog.dwLevel     = dwSkillLevel;
        g_pGameWorldUIHandler->OnSkillCast(CastLog);
    }

Exit1:
	nResult = srcSuccess;
Exit0:
	return nResult;
}

SKILL_RESULT_CODE KCharacter::CastSkillSub(KCharacter* pLogicCaster, DWORD dwSkillID, DWORD dwSkillLevel, KTarget& rTarget, DWORD dwBulletID)
{
	SKILL_RESULT_CODE               nResult           = srcFailed;
	int                             nRetCode          = false;
	KSkill*                         pSkill            = NULL;
    KSKILL_RECIPE_KEY               SkillRecipeKey;

    nRetCode = g_pSO3World->m_SkillManager.GetSkillRecipeKey(&SkillRecipeKey, dwSkillID, dwSkillLevel, this);
    KGLOG_PROCESS_ERROR(nRetCode);

	// 获得技能指针
	pSkill = g_pSO3World->m_SkillManager.GetSkill(SkillRecipeKey);
	KG_PROCESS_ERROR_RET_CODE(pSkill, srcInvalidSkill);

    KGLOG_PROCESS_ERROR(pSkill->m_pBaseInfo->nCastMode != scmItem); // 对道具使用的技能不能走到该流程。

	// 切换目标和记录技能
	m_SkillTarget   = rTarget;

	pSkill->Cast(this, this, rTarget, SkillRecipeKey);

    if (g_pGameWorldRepresentHandler)
        g_pGameWorldRepresentHandler->OnCharacterCastSkill(this, &rTarget, pSkill, dwBulletID, false);
	
	nResult = srcSuccess;
Exit0:
	return nResult;
}

#endif  // _CLIENT

BOOL KCharacter::ProcessCommonSkillActive(const KSKILL_RECIPE_KEY& crSkillRecipeKey)
{
    int                     nResult             = false;
    int                     nRetCode            = false;
    int                     nWeaponType         = 0;
    DWORD                   dwCommonSkillID     = 0;
    BOOL                    bContionHit         = false;
    KSkill*                 pSkill              = NULL;
    KItem*                  pItem               = NULL;
    KPlayer*                pPlayer             = NULL;
    KAUTO_CAST_SKILL_PARAM  AutoCastSkillParam;
    KSKILL_RECIPE_KEY       NewSkillRecipeKey;
    KSKILL_RECIPE_KEY       OldSkillRecipeKey;

    KG_PROCESS_ERROR(IS_PLAYER(m_dwID));
    
    pPlayer = (KPlayer*)this;

    pSkill = g_pSO3World->m_SkillManager.GetSkill(crSkillRecipeKey);
    KGLOG_PROCESS_ERROR(pSkill);

    OldSkillRecipeKey = m_AutoCastSkillParam.SkillRecipePointer.GetRecipeKey();

    switch (pSkill->m_pBaseInfo->nCommonSkillActiveMode)
    {
    case scamInvalid: // 该怎么做什么，不管
        break;
    case scamNone:
        DoAutoCastSkillIdle();
        break;
    case scamMelee:
        nWeaponType = wdtFist;
        pItem = pPlayer->m_ItemList.GetItem(ibEquip, eitMeleeWeapon);
        if (pItem)
        {
            nWeaponType = pItem->m_Common.nDetail;
        }

        bContionHit                         = true;
        AutoCastSkillParam.bMelee           = true;
        AutoCastSkillParam.pnCastCycle      = &pPlayer->m_nMeleeWeaponAttackSpeed;
        AutoCastSkillParam.nPreCastFrame    = g_pSO3World->m_nGameLoop + pPlayer->m_nMeleeWeaponAttackSpeed;
        break;
    case scamRange:
        pItem = pPlayer->m_ItemList.GetItem(ibEquip, eitRangeWeapon);
        if (pItem)
        {
            nWeaponType = pItem->m_Common.nDetail;

            bContionHit                         = true;
            AutoCastSkillParam.bMelee           = false;
            AutoCastSkillParam.pnCastCycle      = &pPlayer->m_nRangeWeaponAttackSpeed;
            AutoCastSkillParam.nPreCastFrame    = g_pSO3World->m_nGameLoop + pPlayer->m_nRangeWeaponAttackSpeed;
        }
        break;
    default:
        KGLOG_PROCESS_ERROR(false && "CommonSkillActiveMode Error.");
    }

    KG_PROCESS_SUCCESS(!bContionHit);

    nRetCode = g_pSO3World->m_SkillManager.WeaponMapCommonSkill(nWeaponType, dwCommonSkillID);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = g_pSO3World->m_SkillManager.GetSkillRecipeKey(&NewSkillRecipeKey, dwCommonSkillID, 1, this);
    KGLOG_PROCESS_ERROR(nRetCode);

    KG_PROCESS_SUCCESS(OldSkillRecipeKey == NewSkillRecipeKey);

    AutoCastSkillParam.SkillRecipePointer.SetRecipeKey(NewSkillRecipeKey);

    DoAutoCastSkill(AutoCastSkillParam);

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

#ifdef _SERVER

BOOL KCharacter::ModifyThreat(THREAT_TYPE eType, int nThreatValue)
{
    BOOL        bResult     = true;
    BOOL        bRetCode    = true;
    KCharacter* pSkillSrc   = NULL;

    KGLOG_PROCESS_ERROR(m_pSkillAttributeParam);

    pSkillSrc = m_pSkillAttributeParam->pSkillSrc;
    KGLOG_PROCESS_ERROR(pSkillSrc);

    switch (eType)
    {
    case thtBaseThreat:
        nThreatValue = nThreatValue * m_pSkillAttributeParam->nBaseThreatCoefficient / KILO_NUM;

        bRetCode = m_SimpThreatList.ModifyThreat(thtBaseThreat, pSkillSrc, nThreatValue);
        KGLOG_CHECK_ERROR(bRetCode);

        break;

    case thtDamageThreat:
        nThreatValue = nThreatValue * m_pSkillAttributeParam->nDamageThreatCoefficient / KILO_NUM;

        bRetCode = m_SimpThreatList.ModifyThreat(thtDamageThreat, pSkillSrc, nThreatValue);
        KGLOG_CHECK_ERROR(bRetCode);

        break;

    case thtTherapyThreat:
        nThreatValue = nThreatValue * m_pSkillAttributeParam->nTherapyThreatCoefficient / KILO_NUM;

        bRetCode = m_SimpThreatList.ModifyThreat(thtTherapyThreat, pSkillSrc, nThreatValue);
        KGLOG_CHECK_ERROR(bRetCode);

        break;

    case thtMainThreat:

        bRetCode = m_SimpThreatList.ModifyThreat(thtMainThreat, pSkillSrc, nThreatValue);
        KGLOG_CHECK_ERROR(bRetCode);

        break;

    case thtSpecialThreat:

        bRetCode = m_SimpThreatList.ModifyThreat(thtSpecialThreat, pSkillSrc, nThreatValue);
        KGLOG_CHECK_ERROR(bRetCode);

        break;

    default:
        KGLogPrintf(KGLOG_ERR, "[Skill] Modify threat, unexpected threat type(%d)\n", eType);
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;
}

int KCharacter::AdjustThreat(THREAT_TYPE eTheatType, int nThreatValue)
{
    int     nResult     = 0;
    int     nRetThreat  = 0;

    switch (eTheatType)
    {
	case thtMainThreat:
    case thtSpecialThreat:
        nRetThreat = nThreatValue;
		break;

    case thtBaseThreat:
        nRetThreat = nThreatValue * m_nPassiveBaseThreatCoefficient / KILO_NUM;
        break;

    case thtDamageThreat:
        nRetThreat = nThreatValue * m_nPassiveDamageThreatCoefficient / KILO_NUM;
        break;
    
    case thtTherapyThreat:
        nRetThreat = nThreatValue * m_nPassiveTherapyThreatCoefficient / KILO_NUM;
        break;
	
    default:
        KGLogPrintf(KGLOG_ERR, "[Skill] Adjust threat, unexpected threat type(%d)\n", eTheatType);
        goto Exit0;
    }

    nResult = nRetThreat;
Exit0:
    return nResult;
}

#endif  // _SERVER

BOOL KCharacter::Stop(void)
{
	BOOL bResult = false;

    KG_PROCESS_ERROR(m_pScene);

	// 站立是无需Stop,死亡时不能Stop(否则会出现晕Debuff反应用会把死人"复活"的问题)
    KG_PROCESS_ERROR(
        m_eMoveState != cmsOnStand && m_eMoveState != cmsOnDeath &&
        m_eMoveState != cmsOnJump
    );

    m_nVelocityXY       = 0;
    m_nConvergenceSpeed = 0;
    
	SetMoveState(cmsOnStand);

#if defined(_SERVER)
	InvalidateMoveState(0, true);
#endif

    bResult = true;
Exit0:
	return bResult;
}

BOOL KCharacter::EmendateDestCoordinate(int& nDestX, int& nDestY)
{
	BOOL    bResult     = false;
    int     nDirection  = 0;
    int     nSin        = 0;
    int     nCos        = 0;

	KGLOG_PROCESS_ERROR(m_nX >= 0 && m_nX <= MAX_X_COORDINATE);
	KGLOG_PROCESS_ERROR(m_nY >= 0 && m_nY <= MAX_Y_COORDINATE);

    nDirection  = g_GetDirection(m_nX, m_nY, nDestX, nDestY);
    nSin        = g_Sin(nDirection);
    nCos        = g_Cos(nDirection);

	if (nDestX < 0)
	{
        if (nCos)
            nDestY -= (nDestX - 0) * nSin / nCos;

		nDestX = 0;
	}

	if (nDestX > MAX_X_COORDINATE)
	{
        if (nCos)
            nDestY -= (nDestX - MAX_X_COORDINATE) * nSin / nCos;

		nDestX = MAX_X_COORDINATE;
	}

	if (nDestY < 0)
	{
        if (nSin)
            nDestX -= (nDestY - 0) * nCos / nSin;

		nDestY = 0;
	}

	if (nDestY > MAX_Y_COORDINATE)
	{
        if (nSin)
            nDestX -= (nDestY - MAX_Y_COORDINATE) * nCos / nSin;

		nDestY = MAX_Y_COORDINATE;
	}

    MAKE_IN_RANGE(nDestX, 0, MAX_X_COORDINATE);
    MAKE_IN_RANGE(nDestY, 0, MAX_Y_COORDINATE);

	bResult = true;
Exit0:
	return bResult;
}

BOOL KCharacter::Stand(BOOL bSyncSelf)
{
    BOOL bResult            = false;
    BOOL bRetCode           = false;
    int  nGradientDirection = 0;

	if (m_eMoveState != cmsOnSit && m_eMoveState != cmsOnWalk && m_eMoveState != cmsOnRun)
	{
		goto Exit0;
	}

    m_nConvergenceSpeed = 0;
    m_nDestX            = m_nX;
    m_nDestY            = m_nY;

    if (m_bSlip)
    {
        if (m_pCell->m_BaseInfo.dwCellType == ctWater)
        {
            KCell* pCell = m_pRegion->GetPreCell(m_nXCell, m_nYCell, m_pCell);
            KGLOG_PROCESS_ERROR(pCell);

            nGradientDirection = pCell->GetGradientDirection();
        }
        else 
        {
            nGradientDirection = m_pCell->GetGradientDirection();
        }

        if (g_Cos(nGradientDirection - m_nDirectionXY) < 0)
        {
            m_nVelocityXY   = 0;
        }
    }
    else
    {
        m_nVelocityXY   = 0;
    }

    SetMoveState(cmsOnStand);

#ifdef _SERVER
    InvalidateMoveState(0, bSyncSelf);
#endif

    bResult = true;
Exit0:
    return bResult;
}

BOOL KCharacter::WalkTo(int nDestX, int nDestY, BOOL bBroadcast, int nWalkSpeed /* = 0 */)
{
    BOOL bResult            = false;
	BOOL bRetCode           = false;
    int  nVelocityXY        = 0;
    int  nDirectionXY       = 0;
    int  nConvergenceSpeed  = 0;
    int  nGradientDirection = 0;

	if (
        m_eMoveState != cmsOnWalk && m_eMoveState != cmsOnRun && m_eMoveState != cmsOnJump && 
        m_eMoveState != cmsOnStand && m_eMoveState != cmsOnSit
    )
	{
		goto Exit0;
	}

    bRetCode = EmendateDestCoordinate(nDestX, nDestY);
    KGLOG_PROCESS_ERROR(bRetCode);

    if (nWalkSpeed == 0)
    {
        nWalkSpeed = GetWalkSpeed();
    }

    if (nDestX == m_nX && nDestY == m_nY)
    {
        // 只有在走/跑的状态下,才可以通过"走向当前点"来停止移动
        // 注意,跳跃状态不理会所谓的"停止"指令,落地后自然会停止
        KG_PROCESS_ERROR(m_eMoveState == cmsOnWalk || m_eMoveState == cmsOnRun);
        nVelocityXY       = 0;
        nDirectionXY      = m_nDirectionXY;
        nConvergenceSpeed = 0;
        SetMoveState(cmsOnStand);
        goto Exit1;
    }
    else
    {
        if (m_eMoveState == cmsOnJump)
        {
            // 在跳跃状态(立定跳)下,允许在空中作一个很小的位移(限一次)
            if (m_nVelocityXY == 0 && !m_bSlip)
            {
                nVelocityXY         = GetWalkSpeed() * VELOCITY_ZOOM_COEFFICIENT;
                nDirectionXY        = g_GetDirection(m_nX, m_nY, nDestX, nDestY);
                nConvergenceSpeed   = GetWalkSpeed();
                goto Exit1;
            }
            goto Exit0;
        }  

        nVelocityXY         = nWalkSpeed * VELOCITY_ZOOM_COEFFICIENT;
        nDirectionXY        = g_GetDirection(m_nX, m_nY, nDestX, nDestY);
        nConvergenceSpeed   = nWalkSpeed;
    }

	SetMoveState(cmsOnWalk);

Exit1:
    MAKE_IN_RANGE(nVelocityXY, 0, MAX_ZOOM_VELOCITY);
    MAKE_IN_RANGE(nConvergenceSpeed, 0, MAX_VELOCITY_XY);

    m_nDestX                = nDestX;
    m_nDestY                = nDestY;
    m_nConvergenceSpeed     = nConvergenceSpeed;

    if (m_bSlip)
    {
        if (m_pCell->m_BaseInfo.dwCellType == ctWater)
        {
            KCell* pCell = m_pRegion->GetPreCell(m_nXCell, m_nYCell, m_pCell);
            KGLOG_PROCESS_ERROR(pCell);

            nGradientDirection = pCell->GetGradientDirection();
        }
        else 
        {
            nGradientDirection = m_pCell->GetGradientDirection();
        }

        if (nVelocityXY == 0 && g_Cos(nGradientDirection - m_nDirectionXY) < 0)
        {
            m_nVelocityXY           = nVelocityXY;
            m_nDirectionXY          = nDirectionXY;
        }
    }
    else
    {
        m_nVelocityXY           = nVelocityXY;
        m_nDirectionXY          = nDirectionXY;
    }

#ifdef _SERVER
    if (IS_NPC(m_dwID))
    {
        // NPC没有"后退",走路时自动转向目标点,不用发"转向"消息,移动指令会带上朝向
        m_nFaceDirection = g_GetDirection(m_nX, m_nY, nDestX, nDestY);
    }

    if (bBroadcast)
    {
        InvalidateMoveState(1, true);
    }
#endif

    bResult = true;
Exit0:
	return bResult;
}

BOOL KCharacter::RunTo(int nDestX, int nDestY, BOOL bBroadcast, int nRunSpeed /* = 0 */)
{
    BOOL bResult            = false;
	BOOL bRetCode           = false;
    int  nVelocityXY        = 0;
    int  nDirectionXY       = 0;
    int  nConvergenceSpeed  = 0;
    int  nGradientDirection = 0;

	if (
        m_eMoveState != cmsOnWalk && m_eMoveState != cmsOnRun && m_eMoveState != cmsOnJump &&
        m_eMoveState != cmsOnSit && m_eMoveState != cmsOnStand
    )
	{
		goto Exit0;
	}

    bRetCode = EmendateDestCoordinate(nDestX, nDestY);
    KGLOG_PROCESS_ERROR(bRetCode);

    if (nRunSpeed == 0)
    {
        nRunSpeed = m_nRunSpeed;
    }

    if (nDestX == m_nX && nDestY == m_nY)
    {
        // 只有在走/跑的状态下,才可以通过"走向当前点"来停止移动
        // 注意,跳跃状态不理会所谓的"停止"指令,落地后自然会停止
        KG_PROCESS_ERROR(m_eMoveState == cmsOnWalk || m_eMoveState == cmsOnRun);
        nVelocityXY       = 0;
        nDirectionXY      = m_nDirectionXY;
        nConvergenceSpeed = 0;
        SetMoveState(cmsOnStand);
        goto Exit1;
    }
    else
    {
        int nDestDirection = g_GetDirection(m_nX, m_nY, nDestX, nDestY);

        if (m_eMoveState == cmsOnJump)
        {
            // 在跳跃状态(立定跳)下,允许在空中作一个很小的位移(限一次)
            if (m_nVelocityXY == 0 && !m_bSlip)
            {
                nVelocityXY         = GetWalkSpeed() * VELOCITY_ZOOM_COEFFICIENT;
                nDirectionXY        = nDestDirection;
                nConvergenceSpeed   = GetWalkSpeed();
                goto Exit1;
            }
            goto Exit0;
        }

        nVelocityXY         = nRunSpeed * VELOCITY_ZOOM_COEFFICIENT;
        nDirectionXY        = nDestDirection;
        nConvergenceSpeed   = nRunSpeed;

        if (IS_PLAYER(m_dwID))
        {
            // 玩家后退时速度减半
            int nDirectionMargin = nDestDirection - m_nFaceDirection;

            if (nDirectionMargin > DIRECTION_COUNT / 2)
            {
                nDirectionMargin = DIRECTION_COUNT - nDirectionMargin;
            }
            else if (nDirectionMargin < - DIRECTION_COUNT / 2)
            {
                nDirectionMargin = nDirectionMargin + DIRECTION_COUNT;
            }

            assert(nDirectionMargin >= -DIRECTION_COUNT / 2 && nDirectionMargin <= DIRECTION_COUNT / 2);

            if (abs(nDirectionMargin) > 80)
            {
                nVelocityXY /= 2;
                nConvergenceSpeed /= 2;
            }
        }
    }

	SetMoveState(cmsOnRun);

Exit1:
    MAKE_IN_RANGE(nVelocityXY, 0, MAX_ZOOM_VELOCITY);
    MAKE_IN_RANGE(nConvergenceSpeed, 0, MAX_VELOCITY_XY);

    m_nDestX                = nDestX;
    m_nDestY                = nDestY;
    m_nConvergenceSpeed     = nConvergenceSpeed;

    if (m_bSlip)
    {
        if (m_pCell->m_BaseInfo.dwCellType == ctWater)
        {
            KCell* pCell = m_pRegion->GetPreCell(m_nXCell, m_nYCell, m_pCell);
            KGLOG_PROCESS_ERROR(pCell);

            nGradientDirection = pCell->GetGradientDirection();
        }
        else 
        {
            nGradientDirection = m_pCell->GetGradientDirection();
        }

        if (nVelocityXY == 0 && g_Cos(nGradientDirection - m_nDirectionXY) < 0)
        {
            m_nVelocityXY           = nVelocityXY;
            m_nDirectionXY          = nDirectionXY;
        }
    }
    else
    {
        m_nVelocityXY           = nVelocityXY;
        m_nDirectionXY          = nDirectionXY;
    }

#ifdef _SERVER
    if (IS_NPC(m_dwID))
    {
        // NPC没有"后退",走路时自动转向目标点,不用发"转向"消息,移动指令会带上朝向
        m_nFaceDirection = g_GetDirection(m_nX, m_nY, nDestX, nDestY);
    }

    if (bBroadcast)
    {
        InvalidateMoveState(1, true);
    }
#endif

    bResult = true;
Exit0:
	return bResult;
}

BOOL KCharacter::SwimTo(int nDestX, int nDestY, BOOL bBroadcast)
{
    BOOL bResult              = false;
    BOOL bRetCode             = false;
    int  nDistance            = 0; // 目标点距离的水平投影
    int  nConvergenceSpeed    = 0;
    int  nVelocityXY          = 0;
    int  nVelocityZ           = 0;
    int  nDirectionXY         = 0;

    assert(m_pCell);

    // 玩家不能主动切换到游泳状态,只能到水里后自动切换
    if (m_eMoveState != cmsOnSwim && m_eMoveState != cmsOnFloat)
    {
        goto Exit0;
    }

    bRetCode = EmendateDestCoordinate(nDestX, nDestY);
    KGLOG_PROCESS_ERROR(bRetCode);

    if (m_nX == nDestX && m_nY == nDestY)
    {
        nConvergenceSpeed   = 0;
        nVelocityXY         = 0;
        nVelocityZ          = 0;
        nDirectionXY        = m_nDirectionXY;
        SetMoveState(cmsOnFloat);
        goto Exit1;
    }
    else
    {
        int     nSwimSpeed = m_nRunSpeed / 2;
        int     nPitch     = 0;
        int     nArgValue  = 0;

        nDirectionXY            = g_GetDirection(m_nX, m_nY, nDestX, nDestY);

        if (abs(nDestX - m_nX) >= abs(nDestY - m_nY))
        {
            nDistance = (nDestX - m_nX) * SIN_COS_NUMBER / g_Cos(nDirectionXY);
        }
        else
        {
            nDistance = (nDestY - m_nY) * SIN_COS_NUMBER / g_Sin(nDirectionXY);
        }

        nArgValue = g_Cos(nDirectionXY - m_nFaceDirection);
        if (nArgValue > 0)
        {
            nPitch = m_nPitchDirection;
        }
        else if (nArgValue < 0)
        {
            nPitch = -m_nPitchDirection;
        }

        nConvergenceSpeed = nSwimSpeed;
        nVelocityXY       = nSwimSpeed * g_Cos(nPitch) / SIN_COS_NUMBER * VELOCITY_ZOOM_COEFFICIENT;
        nVelocityZ        = XYPOINT_TO_ZPOINT(nSwimSpeed * g_Sin(nPitch)) / SIN_COS_NUMBER;

        if (IS_PLAYER(m_dwID))
        {
            int nDirectionMargin = nDirectionXY - m_nFaceDirection;

            if (nDirectionMargin > DIRECTION_COUNT / 2)
            {
                nDirectionMargin = DIRECTION_COUNT - nDirectionMargin;
            }
            else if (nDirectionMargin < - DIRECTION_COUNT / 2)
            {
                nDirectionMargin = nDirectionMargin + DIRECTION_COUNT;
            }

            assert(nDirectionMargin >= -DIRECTION_COUNT / 2 && nDirectionMargin <= DIRECTION_COUNT / 2);

            if (abs(nDirectionMargin) > 80)
            {
                nConvergenceSpeed /= 2;
                nVelocityXY       /= 2;
                nVelocityZ        /= 2;
            }
        }
    }

    SetMoveState(cmsOnSwim);

Exit1:
    MAKE_IN_RANGE(nVelocityXY, 0, MAX_ZOOM_VELOCITY);
    MAKE_IN_RANGE(nConvergenceSpeed, 0, MAX_VELOCITY_XY);
    MAKE_IN_RANGE(nVelocityZ, MIN_VELOCITY_Z, MAX_VELOCITY_Z);

    m_nDestX                    = nDestX;
    m_nDestY                    = nDestY;

    m_nConvergenceSpeed         = nConvergenceSpeed;
    m_nVelocityXY               = nVelocityXY;
    m_nDirectionXY              = nDirectionXY;
    m_nVelocityZ                = nVelocityZ;

#ifdef _SERVER
    if (IS_NPC(m_dwID))
    {
        // NPC没有"后退",走路时自动转向目标点,不用发"转向"消息,移动指令会带上朝向
        m_nFaceDirection = nDirectionXY;
    }

    if (bBroadcast)
    {
        InvalidateMoveState(1, true);
    }
#endif

    bResult = true;
Exit0:
    return bResult;
}

BOOL KCharacter::Jump(BOOL bStandJump, int nJumpDirection, BOOL bSyncSelf)
{
    BOOL bResult                = false;
	BOOL bRetCode               = false;
    int  nDestX                 = 0;
    int  nDestY                 = 0;
    int  nDestDirection         = 0;
    int  nVelocityXY            = 0;
    int  nDirectionXY           = 0;
    int  nConvergenceSpeed      = 0;
    int  nVelocityZ             = 0;
    int  nDirectionMargin       = 0;
    int  nJumpSpeedXY           = 0;
    int  nSwimWaterline         = GET_SWIM_WATER_LINE(m_nHeight);
    int  nWaterLine             = GetWaterline();

    assert(nJumpDirection >= 0 && nJumpDirection < DIRECTION_COUNT);

	if (
        m_eMoveState != cmsOnWalk && m_eMoveState != cmsOnRun && 
        m_eMoveState != cmsOnStand && m_eMoveState != cmsOnJump && 
        m_eMoveState != cmsOnSwim && m_eMoveState != cmsOnFloat &&
        m_eMoveState != cmsOnSwimJump
    )
	{
		goto Exit0;
	}

    if (IS_PLAYER(m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)this;
        
        if (pPlayer->m_bOnHorse)
        {
            // 骑马不能立定跳
            KG_PROCESS_ERROR(!bStandJump);

            // 骑马不能往后跳
            if (g_Cos(nJumpDirection - m_nFaceDirection) < 0)
            {
                goto Exit0;
            }
        }
    }

    if (!bStandJump)
    {
        if (m_eMoveState == cmsOnWalk)
        {
            nJumpSpeedXY = GetWalkSpeed();
        }
        else if (m_eMoveState == cmsOnStand || m_eMoveState == cmsOnRun || m_eMoveState == cmsOnJump)
        {
            nJumpSpeedXY = m_nRunSpeed;
        }
        else if (m_eMoveState == cmsOnFloat || m_eMoveState == cmsOnSwim || m_eMoveState == cmsOnSwimJump)
        {
            nJumpSpeedXY = m_nRunSpeed / 2;
        }

        if (g_pSO3World->m_nGameLoop <= m_nJumpLimitFrame)
        {
            // 在打滑的一定时间以内,只能往斜坡下坡方向跳跃
            int nGradientDegree = m_pCell->GetGradientDegree();
            if (nGradientDegree > 0)
            {
                int nAlpha = m_pCell->GetGradientDirection();
                if (g_Cos(nAlpha - nJumpDirection) <= SIN_COS_NUMBER / 2)
                {
                    goto Exit0;
                }
            }
        }
    }

    nDestDirection      = bStandJump ? m_nFaceDirection : nJumpDirection;
    nVelocityXY         = nJumpSpeedXY * VELOCITY_ZOOM_COEFFICIENT;
    nDirectionXY        = nDestDirection;
    nConvergenceSpeed   = nJumpSpeedXY;
    nVelocityZ          = m_nJumpSpeed;

    nDirectionMargin = nDestDirection - m_nFaceDirection;
    if (nDirectionMargin > DIRECTION_COUNT / 2)
    {
        nDirectionMargin = DIRECTION_COUNT - nDirectionMargin;
    }
    else if (nDirectionMargin < - DIRECTION_COUNT / 2)
    {
        nDirectionMargin = nDirectionMargin + DIRECTION_COUNT;
    }

    assert(nDirectionMargin >= -DIRECTION_COUNT / 2 && nDirectionMargin <= DIRECTION_COUNT / 2);

    if (abs(nDirectionMargin) > 80)
    {
        nVelocityXY         /= 2;
        nConvergenceSpeed   /= 2;
    }

    nDestX              = m_nX + MOVE_DEST_RANGE * g_Cos(nDestDirection) / SIN_COS_NUMBER;
    nDestY              = m_nY + MOVE_DEST_RANGE * g_Sin(nDestDirection) / SIN_COS_NUMBER;

    bRetCode = EmendateDestCoordinate(nDestX, nDestY);
    KGLOG_PROCESS_ERROR(bRetCode);

    if (nWaterLine < nSwimWaterline)
    {
        KG_PROCESS_ERROR(m_nJumpCount < m_nMaxJumpCount && m_nJumpCount < MAX_JUMP_COUNT);
        m_nJumpCount++;

        SetMoveState(cmsOnJump);
    }
    else
    {
        SetMoveState(cmsOnSwimJump);
    }

    MAKE_IN_RANGE(nVelocityXY, 0, MAX_ZOOM_VELOCITY);
    MAKE_IN_RANGE(nConvergenceSpeed, 0, MAX_VELOCITY_XY);
    MAKE_IN_RANGE(nVelocityZ, MIN_VELOCITY_Z, MAX_VELOCITY_Z);

    m_nDestX            = nDestX;
    m_nDestY            = nDestY;

    m_nVelocityXY       = nVelocityXY;
    m_nConvergenceSpeed = nConvergenceSpeed;
    m_nDirectionXY      = nDirectionXY;
    m_nVelocityZ        = nVelocityZ;

#if defined(_SERVER)
    InvalidateMoveState(0, bSyncSelf);
#endif

    bResult = true;
Exit0:
	return bResult;
}

BOOL KCharacter::SitDown(BOOL bSyncSelf)
{
    BOOL bResult  = false;
    BOOL bRetCode = false;

    KG_PROCESS_ERROR(m_eMoveState == cmsOnStand);

    KG_PROCESS_ERROR(!m_bSlip);

    SetMoveState(cmsOnSit);

#if defined (_SERVER)
    InvalidateMoveState(0, bSyncSelf);
#endif

    bResult = true;
Exit0:
    return bResult;
}

BOOL KCharacter::AutoFly(int nFromNode, int nToCity)
{
    BOOL            bResult     = false;
    BOOL            bRetCode    = false;

#ifdef _SERVER
    if (IS_PLAYER(m_dwID))
    {
        KPlayer*    pThis        = (KPlayer*)this;
        int         nNextTrackID = 0;

        bRetCode = g_pSO3World->m_RoadManager.GetNextTrack(
            &nNextTrackID, 0,
            nFromNode, nToCity,
            pThis->m_eCamp, pThis->m_dwTongID,
            &(pThis->m_OpenRouteNodeList)
        );
        KGLOG_PROCESS_ERROR(nNextTrackID);
        KGLOG_PROCESS_ERROR(bRetCode);

        m_nCurrentTrack = nNextTrackID;
    }
    else
    {
        int nNextTrackID = 0;
        bRetCode = g_pSO3World->m_RoadManager.GetNextTrack(
            &nNextTrackID, 0,
            nFromNode, nToCity,
            cNeutral, 0,
            NULL
        );
        KGLOG_PROCESS_ERROR(nNextTrackID);
        KGLOG_PROCESS_ERROR(bRetCode);

        m_nCurrentTrack = nNextTrackID;
    }

    m_nMoveFrameCounter = 0;
    m_nFromNode         = nFromNode;
    m_nTargetCity       = nToCity;

    SetMoveState(cmsOnAutoFly);

    m_BuffList.CleanBuffByPersist();

    InvalidateMoveState(0, true);
#endif

    bResult = true;
Exit0:
    return bResult;
}

BOOL KCharacter::Dash(int nFrame, int nDirectionXY, int nVelocityXY, int nVelocityZ)
{
    BOOL bResult        = false;
    BOOL bRetCode       = false;

	if (
        m_eMoveState == cmsOnDeath || m_eMoveState == cmsOnKnockedDown || m_eMoveState == cmsOnKnockedBack ||
        m_eMoveState == cmsOnKnockedOff || m_eMoveState == cmsOnDash || m_eMoveState == cmsOnPull ||
        m_eMoveState == cmsOnRepulsed || m_eMoveState == cmsOnRise || m_eMoveState == cmsOnSkid ||
        m_eMoveState == cmsOnHalt || m_eMoveState == cmsOnFreeze || m_eMoveState == cmsOnEntrap ||
        m_eMoveState == cmsOnAutoFly
    )
    {
	    goto Exit0;
    }

    MAKE_IN_RANGE(nFrame, 1, UCHAR_MAX);
    MAKE_IN_RANGE(nVelocityXY, 1, MAX_VELOCITY_XY);
    MAKE_IN_RANGE(nVelocityZ, MIN_VELOCITY_Z, MAX_VELOCITY_Z);

    nDirectionXY = nDirectionXY % DIRECTION_COUNT;
    if (nDirectionXY < 0)
    {
        nDirectionXY += DIRECTION_COUNT;
    }

    m_nMoveFrameCounter = nFrame;
    m_nConvergenceSpeed = nVelocityXY;
    m_nVelocityXY       = nVelocityXY * VELOCITY_ZOOM_COEFFICIENT;
    m_nDirectionXY      = nDirectionXY;
    m_nVelocityZ        = nVelocityZ;

	SetMoveState(cmsOnDash);

#ifdef _SERVER
    InvalidateMoveState(0, true);
#endif

    bResult = true;
Exit0:
	return bResult;
}

BOOL KCharacter::Pull(int nFrame, int nDirectionXY, int nVelocityXY, int nVelocityZ)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;
    int  nDestX     = 0;
    int  nDestY     = 0;
    int  nDistance  = 0;

    if (
        m_eMoveState == cmsOnDeath || m_eMoveState == cmsOnKnockedDown || m_eMoveState == cmsOnKnockedBack ||
        m_eMoveState == cmsOnKnockedOff || m_eMoveState == cmsOnDash || m_eMoveState == cmsOnPull ||
        m_eMoveState == cmsOnRepulsed || m_eMoveState == cmsOnRise || m_eMoveState == cmsOnSkid ||
        m_eMoveState == cmsOnAutoFly
    )
    {
        goto Exit0;
    }

    MAKE_IN_RANGE(nFrame, 1, UCHAR_MAX);
    MAKE_IN_RANGE(nVelocityXY, 1, MAX_VELOCITY_XY);
    MAKE_IN_RANGE(nVelocityZ, MIN_VELOCITY_Z, MAX_VELOCITY_Z);

    nDirectionXY = nDirectionXY % DIRECTION_COUNT;
    if (nDirectionXY < 0)
    {
        nDirectionXY += DIRECTION_COUNT;
    }

    nDistance = nVelocityXY * nFrame;
    nDestX = m_nX + nDistance * g_Cos(nDirectionXY) / SIN_COS_NUMBER;
    nDestY = m_nY + nDistance * g_Sin(nDirectionXY) / SIN_COS_NUMBER;

    bRetCode = EmendateDestCoordinate(nDestX, nDestY);
    KGLOG_PROCESS_ERROR(bRetCode);

    m_nMoveFrameCounter = nFrame;
    m_nConvergenceSpeed = nVelocityXY;
    m_nVelocityXY       = nVelocityXY * VELOCITY_ZOOM_COEFFICIENT;
    m_nDirectionXY      = nDirectionXY;
    m_nVelocityZ        = nVelocityZ;

    SetMoveState(cmsOnPull);

#ifdef _SERVER
    InvalidateMoveState(0, true);
#endif

    bResult = true;
Exit0:
    return bResult;
}

BOOL KCharacter::Repulsed(int nFrame, int nDirectionXY, int nVelocityXY)
{
    BOOL    bResult = false;

    if (
        m_eMoveState == cmsOnDeath || m_eMoveState == cmsOnKnockedDown || m_eMoveState == cmsOnKnockedBack ||
        m_eMoveState == cmsOnKnockedOff || m_eMoveState == cmsOnDash || m_eMoveState == cmsOnPull ||
        m_eMoveState == cmsOnRepulsed || m_eMoveState == cmsOnRise || m_eMoveState == cmsOnSkid ||
        m_eMoveState == cmsOnAutoFly
    )
    {
        goto Exit0;
    }

    nDirectionXY = nDirectionXY % DIRECTION_COUNT;
    if (nDirectionXY < 0)
    {
        nDirectionXY += DIRECTION_COUNT;
    }

    MAKE_IN_RANGE(nVelocityXY, 0, MAX_ZOOM_VELOCITY);

    m_nMoveFrameCounter = nFrame;
    m_nConvergenceSpeed = nVelocityXY / VELOCITY_ZOOM_COEFFICIENT;
    m_nVelocityXY       = nVelocityXY;
    m_nDirectionXY      = nDirectionXY;

    SetMoveState(cmsOnRepulsed);

#ifdef _SERVER
    InvalidateMoveState(0, true);
#endif

    bResult = true;
Exit0:
    return bResult;
}

BOOL KCharacter::KnockedDown(int nFrame)
{
    BOOL    bResult     = false;
	BOOL    bRetCode    = false;

	if (
        m_eMoveState == cmsOnDeath || m_eMoveState == cmsOnKnockedDown || m_eMoveState == cmsOnKnockedBack ||
        m_eMoveState == cmsOnKnockedOff || m_eMoveState == cmsOnPull || m_eMoveState == cmsOnRepulsed ||
        m_eMoveState == cmsOnRise || m_eMoveState == cmsOnSkid || m_eMoveState == cmsOnDash ||
        m_eMoveState == cmsOnAutoFly
    )
    {
		goto Exit0;
    }

    // 这里限制帧数是为了同步时少占几个位
    KGLOG_PROCESS_ERROR(nFrame < UCHAR_MAX);

    m_nMoveFrameCounter = nFrame;
    m_nVelocityXY       = 0;
    m_nConvergenceSpeed = 0;
    m_nVelocityZ        = 0;
	SetMoveState(cmsOnKnockedDown);

#if defined(_SERVER)
	InvalidateMoveState(0, true);
#endif

    bResult = true;
Exit0:
	return bResult;
}

BOOL KCharacter::KnockedBack(int nDirection, int nDistance, int nVelocityXY)
{
    BOOL    bResult    = false;
	BOOL    bRetCode   = false;
    int     nDestX     = 0;
    int     nDestY     = 0;
    int     nFrame     = 0;

    if (
        m_eMoveState == cmsOnDeath || m_eMoveState == cmsOnKnockedDown || m_eMoveState == cmsOnKnockedBack ||
        m_eMoveState == cmsOnKnockedOff || m_eMoveState == cmsOnDash || m_eMoveState == cmsOnPull ||
        m_eMoveState == cmsOnRepulsed || m_eMoveState == cmsOnRise || m_eMoveState == cmsOnSkid ||
        m_eMoveState == cmsOnAutoFly
    )
    {
		goto Exit0;
    }

    KGLOG_PROCESS_ERROR(nDistance < MOVE_DEST_RANGE);

	nDestX = m_nX + nDistance * g_Cos(nDirection) / SIN_COS_NUMBER;
	nDestY = m_nY + nDistance * g_Sin(nDirection) / SIN_COS_NUMBER;

    bRetCode = EmendateDestCoordinate(nDestX, nDestY);
    KG_PROCESS_ERROR(bRetCode);

    MAKE_IN_RANGE(nVelocityXY, 1, MAX_VELOCITY_XY);

    nFrame = nDistance / nVelocityXY;
    KGLOG_PROCESS_ERROR(nFrame < UCHAR_MAX);

    m_nMoveFrameCounter = nFrame;
    m_nConvergenceSpeed = nVelocityXY;
    m_nVelocityXY       = nVelocityXY * VELOCITY_ZOOM_COEFFICIENT;
    m_nDirectionXY      = nDirection;
	m_nDestX            = nDestX;
	m_nDestY            = nDestY;
    
	SetMoveState(cmsOnKnockedBack);

#if defined(_SERVER)
	InvalidateMoveState(0, true);
#endif	

    bResult = true;
Exit0:
	return bResult;
}

BOOL KCharacter::KnockedOff(int nDirection, int nDistance, int nVelocityXY)
{
	BOOL    bRetCode   = false;
    BOOL    bResult    = false;
    int     nVelocityZ = 0;
    int     nDestX     = 0;
    int     nDestY     = 0;
    int     nFrame     = 0;

	if (
        m_eMoveState == cmsOnDeath || m_eMoveState == cmsOnKnockedDown || m_eMoveState == cmsOnKnockedBack ||
        m_eMoveState == cmsOnKnockedOff || m_eMoveState == cmsOnDash || m_eMoveState == cmsOnPull ||
        m_eMoveState == cmsOnRepulsed || m_eMoveState == cmsOnRise || m_eMoveState == cmsOnSkid ||
        m_eMoveState == cmsOnAutoFly
    )
    {
		goto Exit0;
    }

    KGLOG_PROCESS_ERROR(nDistance < MOVE_DEST_RANGE);

	nDestX = m_nX + nDistance * g_Cos(nDirection) / SIN_COS_NUMBER;
	nDestY = m_nY + nDistance * g_Sin(nDirection) / SIN_COS_NUMBER;

    bRetCode = EmendateDestCoordinate(nDestX, nDestY);
    KG_PROCESS_ERROR(bRetCode);

    MAKE_IN_RANGE(nVelocityXY, 1, MAX_VELOCITY_XY);

    nFrame = nDistance / nVelocityXY;
    KGLOG_PROCESS_ERROR(nFrame < UCHAR_MAX);

    m_nMoveFrameCounter = nFrame;
    m_nConvergenceSpeed = nVelocityXY;
    m_nVelocityXY       = nVelocityXY * VELOCITY_ZOOM_COEFFICIENT;
    m_nDirectionXY      = nDirection;
	m_nDestX            = nDestX;
	m_nDestY            = nDestY;

    nVelocityZ    = m_nCurrentGravity * nDistance / nVelocityXY / 2;
    MAKE_IN_RANGE(nVelocityZ, 0, MAX_VELOCITY_Z);

    m_nVelocityZ        = nVelocityZ;

	SetMoveState(cmsOnKnockedOff);

#if defined(_SERVER)
	InvalidateMoveState(0, true);
#endif

    bResult = true;
Exit0:
    return bResult;
}

BOOL KCharacter::Halt(void)
{
    BOOL bResult  = false;
    BOOL bRetCode = false;

	if (
        m_eMoveState == cmsOnDeath || m_eMoveState == cmsOnKnockedDown || m_eMoveState == cmsOnKnockedBack ||
        m_eMoveState == cmsOnKnockedOff || m_eMoveState == cmsOnDash || m_eMoveState == cmsOnPull ||
        m_eMoveState == cmsOnRepulsed || m_eMoveState == cmsOnRise || m_eMoveState == cmsOnSkid ||
        m_eMoveState == cmsOnAutoFly
    )
    {
		goto Exit0;
    }

    KG_PROCESS_ERROR(m_pRegion); // 玩家必须已经在场景中

    SetMoveState(cmsOnHalt);

    m_nVelocityXY       = 0;
    m_nConvergenceSpeed = 0;

#ifdef _SERVER
    InvalidateMoveState(0, true);
#endif

    bResult = true;
Exit0:
    return bResult;
}

BOOL KCharacter::Freeze()

{
    BOOL bResult = false;

    if (
        m_eMoveState == cmsOnDeath || m_eMoveState == cmsOnKnockedDown || m_eMoveState == cmsOnKnockedBack ||
        m_eMoveState == cmsOnKnockedOff || m_eMoveState == cmsOnHalt || m_eMoveState == cmsOnPull ||
        m_eMoveState == cmsOnRepulsed || m_eMoveState == cmsOnRise || m_eMoveState == cmsOnSkid ||
        m_eMoveState == cmsOnDash || m_eMoveState == cmsOnAutoFly
    )
    {
        goto Exit0;
    }

    KG_PROCESS_ERROR(m_pRegion);

    m_nVelocityXY       = 0;
    m_nConvergenceSpeed = 0;

    SetMoveState(cmsOnFreeze);

#ifdef _SERVER
    InvalidateMoveState(0, true);
#endif

    bResult = true;
Exit0:
    return bResult;
}

BOOL KCharacter::Entrap()
{
    BOOL bResult = false;

    if (
        m_eMoveState == cmsOnDeath || m_eMoveState == cmsOnKnockedDown || m_eMoveState == cmsOnKnockedBack ||
        m_eMoveState == cmsOnKnockedOff || m_eMoveState == cmsOnHalt || m_eMoveState == cmsOnFreeze ||
        m_eMoveState == cmsOnPull || m_eMoveState == cmsOnRepulsed || m_eMoveState == cmsOnRise ||
        m_eMoveState == cmsOnSkid || m_eMoveState == cmsOnDash || m_eMoveState == cmsOnAutoFly
    )
    {
        goto Exit0;
    }

    KG_PROCESS_ERROR(m_pRegion);

    m_nVelocityXY       = 0;
    m_nConvergenceSpeed = 0;

    SetMoveState(cmsOnEntrap);

#ifdef _SERVER
    InvalidateMoveState(0, true);
#endif

    bResult = true;
Exit0:
    return bResult;
}

BOOL KCharacter::CheckDie()
{
    BOOL        bResult             = false;
	BOOL        bRetCode            = false;

#if defined(_SERVER)
    KG_PROCESS_ERROR(m_bToDie);

	if (IS_PLAYER(m_dwID))
	{        
        KPlayer* pPlayer    = (KPlayer*)this;
        PK_STATE eState	    = pPlayer->m_PK.GetState();
        DWORD    dwTargetID = pPlayer->m_PK.GetTargetID();
        DWORD    dwLoserID  = pPlayer->m_PK.GetLoserID();
        KPlayer* pTarget    = g_pSO3World->m_PlayerSet.GetObj(dwTargetID);

        if (dwTargetID != ERROR_ID && (eState == pksDueling || eState == pksDuelOutOfRange))
        {
            if (dwLoserID == m_dwID && (dwTargetID == m_dwKillerID || m_dwID == m_dwKillerID))
            {
                pPlayer->m_PK.CallDuelEndScript(pkecBeat);

                pPlayer->m_PK.LossDuel();
                pPlayer->m_PK.SetLoserID(ERROR_ID);
                pPlayer->m_nCurrentLife = 1;

                if (pTarget && pTarget->m_bToDie) // 避免目标被随后的checkdie弄死
                {
                    pTarget->m_nCurrentLife = 1;
                    pTarget->m_bToDie = false;
                }

                goto Exit1;
            }
        }

		// 玩家死亡脚本
		pPlayer->m_nCurrentLife = 0;
        pPlayer->ProcessCampPK(m_dwKillerID);
		pPlayer->CallDeathScript(m_dwKillerID);

		if (m_nCurrentLife > 0)
			goto Exit1;

        pPlayer->m_AIVM.FireEvent(aevOnSelfKilled, m_dwKillerID, 0);

        pPlayer->m_nCurrentLife      = 1;
		pPlayer->m_nCurrentMana      = 0;

		pPlayer->m_dwTradingInviteSrc = ERROR_ID;
		pPlayer->m_dwTradingInviteDst = ERROR_ID;
	}
	else
    {
        KNpc*       pNpc    = (KNpc*)this;
        KPlayer*    pPlayer = NULL;

		//-------- AI Event:Death --------------------------------
		{
			KTarget rTarget;
			rTarget.ClearTarget();

			if (m_dwKillerID != ERROR_ID && IS_PLAYER(m_dwKillerID))
			{
				rTarget.SetTarget(ttPlayer, m_dwKillerID);
			}
			g_pSO3World->m_Settings.m_SmartDialogList.FireEvent(pNpc, ntatDeath, rTarget);
		}
		//--------------------------------------------------------

		pNpc->m_nCurrentLife = 0;
        pNpc->CallDeathScript(m_dwKillerID);
        pNpc->m_AIVM.FireEvent(aevOnSelfKilled, m_dwKillerID, 0);

		// 策划需求:如果调用死亡脚本后Npc的生命变成>0的,那么就不进入死亡流程
		if (pNpc->m_nCurrentLife > 0)
		{
			goto Exit1;
		}

        pPlayer = pNpc->GetDropTarget();
        if (pPlayer)
        {
            pNpc->LootExp(pPlayer);
            pNpc->LootPrestige(pPlayer);
            pNpc->LootContribution(pPlayer);
            pNpc->LootAchievement(pPlayer);
            pNpc->LootCampScore();

            pPlayer->m_AntiFarmer.RecordKillNpc(pNpc->m_dwTemplateID);
        }
        else
        {
            pNpc->ClearDropTarget();
        }
	}

    if (m_AIData.nPatrolPathID)
        m_pScene->m_AIPatrolGroupManager.LeaveGroup(m_AIData.nPatrolPathID, m_dwID);

#endif

    m_nMoveFrameCounter = g_pSO3World->m_Settings.m_ConstList.nCorpseGenerationFrame;
	SetMoveState(cmsOnDeath);
    
#ifdef _SERVER

    g_pSO3World->m_StatDataServer.UpdatePVEStat(m_dwID, m_dwKillerID);

    if (m_OTActionParam.eActionType == otActionRecipePrepare)
    {
        KCraft* pCraft  = NULL;

        pCraft = g_pSO3World->m_ProfessionManager.GetCraft(m_OTActionParam.ProfessionSkill.dwCraftID);
	    KGLOG_PROCESS_ERROR(pCraft);

        pCraft->CallBreakScriptFunction((KPlayer*)this, m_OTActionParam.ProfessionSkill.dwRecipeID);
    }
#endif

    DoActionIdle(); // 中断持续一段时间的技能，如吟唱
    DoAutoCastSkillIdle(); // 中断持续一段时间释放的自动技能，如自动攻击

#ifdef _SERVER
    g_PlayerServer.DoCharacterDeath(this, m_dwKillerID);

    m_BuffList.CleanBuffByPersist();

    m_SimpThreatList.ClearAllThreat();
    m_SimpThreatList.ClearAllThreaten();

    // 必须在发死亡状态同步包之后，否则界面会不正常。
    if (IS_PLAYER(m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)this;

        pPlayer->m_ReviveCtrl.bReviveByPlayer       = false;
        pPlayer->m_ReviveCtrl.bReviveInAltar        = false;
        pPlayer->m_ReviveCtrl.bReviveInSitu         = false;

        pPlayer->m_ReviveCtrl.nNextCheckReviveFrame = 0;
        pPlayer->m_ReviveCtrl.nReviveFrame          = 0;

        pPlayer->m_ReviveCtrl.dwReviver             = ERROR_ID;
        pPlayer->m_ReviveCtrl.nReviveLife           = 0;
        pPlayer->m_ReviveCtrl.nReviveMana           = 0;

        pPlayer->CheckRevive(true);
    }
#endif

    m_nConvergenceSpeed = 0;
    m_nVelocityXY       = 0;

Exit1:
    bResult = true;
Exit0:
#ifdef _SERVER
    m_bToDie = false;
#endif
	return bResult;
}

BOOL KCharacter::Action(DWORD dwCharacterID, DWORD dwActionType)
{
    BOOL                bResult             = false;
    BOOL                bRetCode            = false;
    KCharacter*         pCharacter          = NULL;
    KCharacterAction*   pCharacterAction    = NULL;
    DWORD               dwOldActionType     = 0;
    static char         szAutoDialog[MAX_NPC_DIALOG_LEN];

    // 注意,表情号为0表示中断表情动作,

    KGLOG_PROCESS_ERROR(m_pScene);
    KGLOG_PROCESS_ERROR(m_eMoveState != cmsOnDeath);

    dwOldActionType = m_dwCharacterAction;

    if (dwActionType == 0)
        goto Exit1;

#ifdef _SERVER
    pCharacterAction = g_pSO3World->m_Settings.m_CharacterActionList.GetAction(dwActionType);
    if (!pCharacterAction)
    {
        KGLogPrintf(KGLOG_ERR, "Can't Found Action %u by Character %u", dwActionType, dwCharacterID);
        goto Exit0;
    }

    if (IS_PLAYER(dwCharacterID))
        pCharacter = (KCharacter*)g_pSO3World->m_PlayerSet.GetObj(dwCharacterID);
    else
        pCharacter = (KCharacter*)g_pSO3World->m_NpcSet.GetObj(dwCharacterID);

    if (pCharacterAction->dwRange)
    {
        KG_PROCESS_ERROR(pCharacter);
        bRetCode = CanDoAction(pCharacter, dwActionType);
        KG_PROCESS_ERROR(bRetCode);
    }

    // 触发新AI事件
    if (pCharacter && pCharacter->m_pScene && IS_NPC(pCharacter->m_dwID) && pCharacter->m_AIVM.GetAIType() != 0)
    {
        if (dwActionType == DIALOG_ACTION_ID)
        {
            KNpc* pNpc = (KNpc*)pCharacter;
            if (pNpc->m_bDialogFlag)
                pCharacter->m_AIVM.FireEvent(aevOnDialog, m_dwID, 0);
        }
        else
        {
            pCharacter->m_AIVM.FireEvent(aevOnAction, m_dwID, (int)dwActionType);
        }
    }

#endif

Exit1:
    m_dwCharacterAction = dwActionType;
#ifdef _CLIENT
    //通知表现逻辑
    if (dwActionType)   // 通知开始
    {
        pCharacterAction = g_pSO3World->m_Settings.m_CharacterActionList.GetAction(dwActionType);
        KGLOG_PROCESS_ERROR(pCharacterAction);

        if (g_pGameWorldRepresentHandler)
        {
            g_pGameWorldRepresentHandler->OnCharacterPlayAnimation(this, pCharacterAction->dwAnimationID);
        }
    }
    else    // 通知结束
    {
        pCharacterAction = g_pSO3World->m_Settings.m_CharacterActionList.GetAction(dwOldActionType);
        KGLOG_PROCESS_ERROR(pCharacterAction);

        if (g_pGameWorldRepresentHandler)
        {
            g_pGameWorldRepresentHandler->OnCharacterPlayAnimationEnd(this, pCharacterAction->dwAnimationID);
        }
    }
#endif	//_CLIENT


#ifdef _SERVER
    g_PlayerServer.DoCharacterAction(this, dwCharacterID, dwActionType);
#endif

    bResult = true;
Exit0:
    return bResult;
}

BOOL KCharacter::TurnTo(int nNewDirection, BOOL bBroadcast)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;

    if (
        m_eMoveState != cmsOnStand && m_eMoveState != cmsOnWalk && m_eMoveState != cmsOnRun &&
        m_eMoveState != cmsOnJump && m_eMoveState != cmsOnSwimJump && m_eMoveState != cmsOnSwim &&
        m_eMoveState != cmsOnFloat && m_eMoveState != cmsOnSit
    )
    {
        goto Exit0;
    }

    nNewDirection = nNewDirection % DIRECTION_COUNT;
    if (nNewDirection < 0)
    {
        nNewDirection += DIRECTION_COUNT;
    }

    KG_PROCESS_ERROR(nNewDirection != m_nFaceDirection);

	m_nFaceDirection = nNewDirection;

    if (m_eMoveState == cmsOnSit)
    {
        m_nConvergenceSpeed = 0;
        m_nDestX            = m_nX;
        m_nDestY            = m_nY;

        SetMoveState(cmsOnStand);
    }

#ifdef _SERVER
    if (bBroadcast)
    {
        if (IS_PLAYER(m_dwID))
        {
            ((KPlayer*)this)->m_nRecordCount = 0;
        }
        InvalidateMoveState(1, true);
    }
#endif

    bResult = true;
Exit0:
    return bResult;
}

BOOL KCharacter::PitchTo(int nNewDirection, BOOL bBroadcast)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;

    if (m_eMoveState != cmsOnSwimJump && m_eMoveState != cmsOnSwim && m_eMoveState != cmsOnFloat)
    {
        goto Exit0;
    }

    nNewDirection = nNewDirection % DIRECTION_COUNT;
    if (nNewDirection < 0)
    {
        nNewDirection += DIRECTION_COUNT;
    }

    KG_PROCESS_ERROR(nNewDirection != m_nPitchDirection);

	m_nPitchDirection = nNewDirection;

#ifdef _SERVER
    if (bBroadcast)
    {
        if (IS_PLAYER(m_dwID))
        {
            ((KPlayer*)this)->m_nRecordCount = 0;
        }
        InvalidateMoveState(1, true);
    }
#endif

    bResult = true;
Exit0:
    return bResult;
}

BOOL KCharacter::MoveTo(int nX, int nY, int nZ, BOOL bIgnoreBlock, BOOL bBroadcast)
{
    BOOL        bResult             = false;
	BOOL        bRetCode            = false;
    KRegion*    pDstRegion          = NULL;
    int         nDstRegionX         = 0;
    int         nDstRegionY         = 0;
    KCell*      pDstCell            = NULL;
    int         nDstCellX           = 0;
    int         nDstCellY           = 0;
    int         nRelativeZ          = 0;
    int         nObstacleDirection  = -1;

	assert(nX >= 0);
	assert(nY >= 0);
    assert(m_pScene);
    assert(m_pRegion);


#ifdef _SERVER
    if (bBroadcast)
    {
	    g_PlayerServer.DoMoveCharacter(this, nX, nY, nZ, bIgnoreBlock);
    }
#endif

    nDstRegionX = nX / CELL_LENGTH / REGION_GRID_WIDTH;
    nDstRegionY = nY / CELL_LENGTH / REGION_GRID_HEIGHT;

#ifdef _CLIENT
    if (m_dwID == g_pSO3World->m_dwClientPlayerID)
    {
        // ValidateRegions一般是在主角切换Region的时候做的
        // 这里要做一下是因为Move距离可能很大,已经不再"附近"了
        bRetCode = m_pScene->ValidateRegions(nDstRegionX, nDstRegionY);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
#endif

    pDstRegion = m_pScene->GetRegion(nDstRegionX, nDstRegionY);
#ifdef _CLIENT
    if (pDstRegion == NULL && m_dwID != g_pSO3World->m_dwClientPlayerID)
    {
        if (IS_PLAYER(m_dwID))
        {
            g_pSO3World->DelPlayer((KPlayer*)this);
        }
        else
        {
            g_pSO3World->DeleteNpc((KNpc*)this);
        }
        goto Exit0;
    }
#endif

    KG_PROCESS_ERROR(pDstRegion);

    nDstCellX = nX / CELL_LENGTH % REGION_GRID_WIDTH;
    nDstCellY = nY / CELL_LENGTH % REGION_GRID_HEIGHT;    

    pDstCell = pDstRegion->GetProximalCellForObj(
        nDstCellX, nDstCellY, nZ, m_nHeight, true, bIgnoreBlock, &nRelativeZ
    );
    KGLOG_PROCESS_ERROR(pDstCell);    

    if (pDstRegion != m_pRegion)
    {
        bRetCode = ChangeRegion(pDstRegion);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

	m_nX     = nX;
	m_nY     = nY;
    m_nZ     = pDstCell->m_wHighLayer * POINT_PER_ALTITUDE + nRelativeZ;

    m_nXCell = nDstCellX;
    m_nYCell = nDstCellY;

    if (pDstCell != m_pCell)
    {
        SetCell(pDstCell);
    }

    bResult = true;
Exit0:
	return bResult;
}

BOOL KCharacter::GoToHell(DWORD dwKiller)
{
    BOOL    bResult = false;

    m_nCurrentLife  = 0;

#ifdef _SERVER
    const KSimpThreatNode*  cpThreatNode    = NULL;
    int                     nThreatCount    = 0;

    KG_PROCESS_ERROR(!m_bToDie);

    m_bToDie        = true;
    m_dwKillerID    = dwKiller;
    
    if (IS_PLAYER(m_dwID))
    {
        KPlayer* pPlayer    = (KPlayer*)this;
        DWORD    dwLoserID  = pPlayer->m_PK.GetLoserID(); //  可能之前已经设置过Loser了(多次"GoToHell")
        PK_STATE eState     = pPlayer->m_PK.GetPKState();

        if (dwLoserID == ERROR_ID && (eState == pksDueling || eState == pksDuelOutOfRange))
            pPlayer->m_PK.SetLoserID(m_dwID);
    }

    // 贡献度统计

    if (dwKiller != m_dwID && IS_PLAYER(m_dwID))
    {
        cpThreatNode = m_SimpThreatList.GetFirstThreat(thtMainThreat);

        while (cpThreatNode)
        {
            if (cpThreatNode->pCharacter && IS_PLAYER(cpThreatNode->pCharacter->m_dwID))
            {
                KPlayer* pPlayer = (KPlayer*)cpThreatNode->pCharacter;
                KGPQ_STATISTICS_INCREMENT   StatisticsIncrement(&g_pSO3World->m_PQManager, pPlayer->m_dwID, psiKillCount);

                pPlayer->m_PQList.Traverse(StatisticsIncrement);
            }

            nThreatCount++;
            cpThreatNode = m_SimpThreatList.GetNextThreat(thtMainThreat, cpThreatNode);
        }

        if (nThreatCount == 1)
        {
            cpThreatNode = m_SimpThreatList.GetFirstThreat(thtMainThreat);
            if (cpThreatNode->pCharacter && IS_PLAYER(cpThreatNode->pCharacter->m_dwID))
            {
                KPlayer* pPlayer = (KPlayer*)cpThreatNode->pCharacter;
                KGPQ_STATISTICS_INCREMENT   StatisticsIncrement(&g_pSO3World->m_PQManager, pPlayer->m_dwID, psiSoloCount);

                pPlayer->m_PQList.Traverse(StatisticsIncrement);
            }
        }

        if (m_dwKillerID && IS_PLAYER(m_dwKillerID))
        {
            KPlayer* pPlayer = g_pSO3World->m_PlayerSet.GetObj(m_dwKillerID);
            KGPQ_STATISTICS_INCREMENT   StatisticsIncrement(&g_pSO3World->m_PQManager, m_dwKillerID, psiDecapitateCount);

            if (pPlayer)
                pPlayer->m_PQList.Traverse(StatisticsIncrement);
        }
    }

    if (IS_PLAYER(m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)this;
        KGPQ_STATISTICS_INCREMENT   StatisticsIncrement(&g_pSO3World->m_PQManager, m_dwID, psiDeathCount);

        pPlayer->m_PQList.Traverse(StatisticsIncrement);
    }
#endif

    bResult = true;
Exit0:
    return bResult;
}

void KCharacter::ApplyAttribute(KAttribute* pAttr)
{
	while (pAttr)
	{
        CallAttributeFunction(pAttr->nKey, true, pAttr->nValue1, pAttr->nValue2);

		pAttr = pAttr->pNext;
	}
}

void KCharacter::UnApplyAttribute(KAttribute* pAttr)
{
	while (pAttr)
	{
        CallAttributeFunction(pAttr->nKey, false, pAttr->nValue1, pAttr->nValue2);

		pAttr = pAttr->pNext;
	}
}

BOOL KCharacter::CallAttributeFunction(int nKey, BOOL bApply, int nValue1, int nValue2)
{
    BOOL                bResult = false;
    ATTR_PROCESS_FUN    PFunc   = NULL;

    if (nKey <= atInvalid || nKey >= atTotal)
    {
        KGLogPrintf(KGLOG_ERR, "Unexpected attribute key %d !\n", nKey);
        goto Exit0;
    }

    PFunc = g_AttributeProcessFunctions[nKey];
    KG_PROCESS_ERROR(PFunc);

    PFunc(this, bApply, nValue1, nValue2);

    bResult = true;
Exit0:
    return bResult;
}

void KCharacter::UpdateAllState()
{
    UpdateWeaponAttackSpeed();
    UpdateBaseState();
    UpdateAllAttackPowerValue();
    UpdateAllShieldValue();
    UpdateAllCriticalStrikeValue();
    UpdateAllBattleState();
}

void KCharacter::UpdateWeaponAttackSpeed()
{
    KPlayer*    pPlayer     = NULL;
    KItem*      pItem       = NULL;

    KG_PROCESS_ERROR(IS_PLAYER(m_dwID));

    pPlayer = (KPlayer*)this;
    pItem = pPlayer->m_ItemList.GetItem(ibEquip, eitMeleeWeapon);

    if (pItem)
    {
        m_nMeleeWeaponAttackSpeed = 
            m_nMeleeWeaponAttackSpeedBase * KILO_NUM / max(1, (KILO_NUM + m_nMeleeWeaponAttackSpeedPercentAdd)) +
            m_nMeleeWeaponAttackSpeedAdditional;
    }
    else
    {
        m_nMeleeWeaponAttackSpeed = 
            m_nNoneWeaponAttackSpeedBase * KILO_NUM / max(1, (KILO_NUM + m_nMeleeWeaponAttackSpeedPercentAdd)) +
            m_nMeleeWeaponAttackSpeedAdditional;
    }

    MAKE_IN_RANGE(m_nMeleeWeaponAttackSpeed, 1, GAME_FPS * 20);

    m_nRangeWeaponAttackSpeed = 
        m_nRangeWeaponAttackSpeedBase * KILO_NUM / max(1, (KILO_NUM + m_nRangeWeaponAttackSpeedPercentAdd)) +
        m_nRangeWeaponAttackSpeedAdditional;

    MAKE_IN_RANGE(m_nRangeWeaponAttackSpeed, 1, GAME_FPS * 20);
Exit0:
    return;
}

void KCharacter::UpdateBaseState()
{
    int64_t llMaxLife = 0;
    int64_t llMaxMana = 0;
    llMaxLife = (int64_t)m_nMaxLifeBase * (KILO_NUM + (int64_t)m_nMaxLifePercentAdd) / KILO_NUM + (int64_t)m_nMaxLifeAdditional;

    llMaxMana = (int64_t)m_nMaxManaBase * (KILO_NUM + (int64_t)m_nMaxManaPercentAdd) / KILO_NUM + (int64_t)m_nMaxManaAdditional;

    if (IS_PLAYER(m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)this;

        llMaxLife += (int64_t)pPlayer->m_nCurrentVitality * (int64_t)pPlayer->m_nVitalityToMaxLifeCof / KILO_NUM;
        llMaxMana += (int64_t)pPlayer->m_nCurrentSpirit * (int64_t)pPlayer->m_nSpiritToMaxManaCof / KILO_NUM;
    }

    MAKE_IN_RANGE(llMaxLife, 1, MAX_LIFE_VALUE);
    MAKE_IN_RANGE(llMaxMana, 1, MAX_MANA_VALUE);

    m_nMaxLife = (int)llMaxLife;
    m_nMaxMana = (int)llMaxMana;
}

void KCharacter::UpdateAllAttackPowerValue()
{
    m_nPhysicsAttackPower = m_nPhysicsAttackPowerBase * m_nPhysicsAttackPowerPercent / KILO_NUM;

    m_nSolarAttackPower = m_nSolarAttackPowerBase * m_nSolarAttackPowerPercent / KILO_NUM;

    m_nNeutralAttackPower = m_nNeutralAttackPowerBase * m_nNeutralAttackPowerPercent / KILO_NUM;

    m_nLunarAttackPower = m_nLunarAttackPowerBase * m_nLunarAttackPowerPercent / KILO_NUM;

    m_nPoisonAttackPower = m_nPoisonAttackPowerBase * m_nPoisonAttackPowerPercent / KILO_NUM;

    m_nTherapyPower = m_nTherapyPowerBase * m_nTherapyPowerPercent / KILO_NUM;

    if (IS_PLAYER(m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)this;

        pPlayer->m_nPhysicsAttackPower += pPlayer->m_nCurrentVitality * pPlayer->m_nVitalityToPhysicsAttackPowerCof / KILO_NUM;
        pPlayer->m_nPhysicsAttackPower += pPlayer->m_nCurrentSpirit * pPlayer->m_nSpiritToPhysicsAttackPowerCof / KILO_NUM;
        pPlayer->m_nPhysicsAttackPower += pPlayer->m_nCurrentStrength * pPlayer->m_nStrengthToPhysicsAttackPowerCof / KILO_NUM;

        pPlayer->m_nSolarAttackPower += pPlayer->m_nCurrentVitality * pPlayer->m_nVitalityToSolarAttackPowerCof / KILO_NUM;
        pPlayer->m_nSolarAttackPower += pPlayer->m_nCurrentSpunk * pPlayer->m_nSpunkToSolarAttackPowerCof / KILO_NUM;

        pPlayer->m_nNeutralAttackPower += pPlayer->m_nCurrentVitality * pPlayer->m_nVitalityToNeutralAttackPowerCof / KILO_NUM;
        pPlayer->m_nNeutralAttackPower += pPlayer->m_nCurrentSpunk * pPlayer->m_nSpunkToNeutralAttackPowerCof / KILO_NUM;

        pPlayer->m_nLunarAttackPower += pPlayer->m_nCurrentVitality * pPlayer->m_nVitalityToLunarAttackPowerCof / KILO_NUM;
        pPlayer->m_nLunarAttackPower += pPlayer->m_nCurrentSpunk * pPlayer->m_nSpunkToLunarAttackPowerCof / KILO_NUM;

        pPlayer->m_nPoisonAttackPower += pPlayer->m_nCurrentVitality * pPlayer->m_nVitalityToPoisonAttackPowerCof / KILO_NUM;
        pPlayer->m_nPoisonAttackPower += pPlayer->m_nCurrentSpunk * pPlayer->m_nSpunkToPoisonAttackPowerCof / KILO_NUM;

        pPlayer->m_nTherapyPower += pPlayer->m_nCurrentVitality * pPlayer->m_nVitalityToTherapyPowerCof / KILO_NUM;
    }
}

void KCharacter::UpdateAllShieldValue()
{
    m_nPhysicsShield = m_nPhysicsShieldBase * (KILO_NUM + m_nPhysicsShieldPercent) / KILO_NUM + m_nPhysicsShieldAdditional;

    m_nSolarMagicShield = m_nSolarMagicShieldBase * m_nSolarMagicShieldPercent / KILO_NUM;

    m_nNeutralMagicShield = m_nNeutralMagicShieldBase * m_nNeutralMagicShieldPercent / KILO_NUM;

    m_nLunarMagicShield = m_nLunarMagicShieldBase * m_nLunarMagicShieldPercent / KILO_NUM;

    m_nPoisonMagicShield = m_nPoisonMagicShieldBase * m_nPoisonMagicShieldPercent / KILO_NUM;

    if (IS_PLAYER(m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)this;

        pPlayer->m_nPhysicsShield += pPlayer->m_nCurrentVitality * pPlayer->m_nVitalityToPhysicsShieldCof / KILO_NUM;

        pPlayer->m_nSolarMagicShield += pPlayer->m_nCurrentSpirit * pPlayer->m_nSpiritToSolarMagicShieldCof / KILO_NUM;

        pPlayer->m_nNeutralMagicShield += pPlayer->m_nCurrentSpirit * pPlayer->m_nSpiritToNeutralMagicShieldCof / KILO_NUM;

        pPlayer->m_nLunarMagicShield += pPlayer->m_nCurrentSpirit * pPlayer->m_nSpiritToLunarMagicShieldCof / KILO_NUM;

        pPlayer->m_nPoisonMagicShield += pPlayer->m_nCurrentSpirit * pPlayer->m_nSpiritToPoisonMagicShieldCof / KILO_NUM;
    }
}

void KCharacter::UpdateAllCriticalStrikeValue()
{
    m_nPhysicsCriticalStrike = m_nPhysicsCriticalStrikeBase;

    m_nSolarCriticalStrike = m_nSolarCriticalStrikeBase;

    m_nNeutralCriticalStrike = m_nNeutralCriticalStrikeBase;

    m_nLunarCriticalStrike = m_nLunarCriticalStrikeBase;

    m_nPoisonCriticalStrike = m_nPoisonCriticalStrikeBase;

    if (IS_PLAYER(m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)this;

        pPlayer->m_nPhysicsCriticalStrike += pPlayer->m_nCurrentSpirit * pPlayer->m_nSpiritToPhysicsCriticalStrikeCof / KILO_NUM;
        pPlayer->m_nPhysicsCriticalStrike += pPlayer->m_nCurrentStrength * pPlayer->m_nStrengthToPhysicsCriticalStrikeCof / KILO_NUM;
        pPlayer->m_nPhysicsCriticalStrike += pPlayer->m_nCurrentAgility * pPlayer->m_nAgilityToPhysicsCriticalStrikeCof / KILO_NUM;

        pPlayer->m_nSolarCriticalStrike += pPlayer->m_nCurrentSpirit * pPlayer->m_nSpiritToSolarCriticalStrikeCof / KILO_NUM;

        pPlayer->m_nNeutralCriticalStrike += pPlayer->m_nCurrentSpirit * pPlayer->m_nSpiritToNeutralCriticalStrikeCof / KILO_NUM;

        pPlayer->m_nLunarCriticalStrike += pPlayer->m_nCurrentSpirit * pPlayer->m_nSpiritToLunarCriticalStrikeCof / KILO_NUM;

        pPlayer->m_nPoisonCriticalStrike += pPlayer->m_nCurrentSpirit * pPlayer->m_nSpiritToPoisonCriticalStrikeCof / KILO_NUM;
    }
}

void KCharacter::UpdateAllBattleState()
{
    m_nParryValue = m_nParryValueBase * m_nParryValuePercent / KILO_NUM;

    if (IS_PLAYER(m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)this;

        pPlayer->m_nParryValue += pPlayer->m_nCurrentVitality * pPlayer->m_nVitalityToParryValueCof / KILO_NUM;
    }
}

#ifdef _CLIENT
void KCharacter::CheckFightFlag()
{
    BOOL                    bRetCode    = false;
	const KThreatLiteNode*	pThreat	    = m_ThreatList.GetFirstThreat();		
    const KThreatLiteNode*  pThreaten   = m_ThreatList.GetFirstThreaten();
	BOOL                    bFightState = false;
    KUIEventFightHint       FightHint;

	if (IS_PLAYER(m_dwID))
	{
		bFightState = (pThreaten != NULL) || (pThreat != NULL);
	}
	else
	{
		bFightState = (pThreat != NULL);
	}

    KG_PROCESS_ERROR(bFightState != m_bFightState);
    KG_PROCESS_ERROR(m_dwID == g_pSO3World->m_dwClientPlayerID);

	m_bFightState = bFightState;

    FightHint.bFight = bFightState;

    if (g_pGameWorldUIHandler)
        g_pGameWorldUIHandler->OnFightHint(FightHint); 

    if (bFightState && pThreaten)
    {
        KPlayer*    pPlayer     = (KPlayer*)this;
        TARGET_TYPE eTargetType = ttInvalid;

        bRetCode = IS_PLAYER(m_dwID);
        KGLOG_PROCESS_ERROR(bRetCode);

        eTargetType = pPlayer->m_SelectTarget.GetTargetType();
        KG_PROCESS_ERROR(eTargetType == ttNoTarget);

        bRetCode = pPlayer->SwitchSelectTarget(pThreaten->dwCharacterID);
        KG_PROCESS_ERROR(bRetCode);
    }

Exit0:
    return;
}
#endif

#ifdef _SERVER
void KCharacter::CheckFightFlag()
{
    const KSimpThreatNode*      cpThreat    = m_SimpThreatList.GetFirstThreat(thtMainThreat);
    const KSimpThreatenNode*    cpThreaten  = m_SimpThreatList.GetFirstThreaten();
	BOOL                        bFightState = false;

	if (IS_PLAYER(m_dwID))
		bFightState = (cpThreaten != NULL) || (cpThreat != NULL);
	else
		bFightState = (cpThreat != NULL);

    KG_PROCESS_ERROR(bFightState != m_bFightState);

    m_bFightState = bFightState;

    InvalidateMoveState(GAME_FPS / 4, false);

    // 如果切换成战斗状态,则自动拔剑,但是脱离战斗并不自动收起武器
    if (m_bFightState && m_bSheathFlag)
    {
        m_bSheathFlag = false;
        g_PlayerServer.DoSyncSheathState(this);
    }

    if (!bFightState)
    {
        // 从战斗状态切换为非战斗状态
        if ((IS_NPC(m_dwID)) && m_eMoveState != cmsOnDeath)
        {
            // Npc脱离战斗状态立即回复属性
            m_nCurrentLife      = m_nMaxLife;
            m_nCurrentMana      = m_nMaxMana;

            ((KNpc*)this)->ClearDropTarget();
        }
    }

    if (bFightState && IS_NPC(m_dwID))
    {
        KNpc* pNpc = (KNpc*)this;
        pNpc->m_nInFightGameLoop = g_pSO3World->m_nGameLoop;
    }

Exit0:
    return;
}
#endif

void KCharacter::AttribReplenish()
{
    int nLifeReplenish = 0;
    int nManaReplenish = 0;

    if (g_pSO3World->m_nGameLoop < m_nNextReplenishFrame)
        return;
    
    if (m_eMoveState == cmsOnDeath)
        return;

    if (!m_bFightState)
    {
        nLifeReplenish += m_nLifeReplenish;
        nManaReplenish += m_nManaReplenish;
    }

    nLifeReplenish += m_nLifeReplenish * m_nLifeReplenishCoefficient / KILO_NUM;
    nLifeReplenish += m_nMaxLife * m_nLifeReplenishPercent / KILO_NUM;
    nLifeReplenish += m_nLifeReplenishExt;

    nManaReplenish += m_nManaReplenish * m_nManaReplenishCoefficient / KILO_NUM;
    nManaReplenish += m_nMaxMana * m_nManaReplenishPercent / KILO_NUM;
    nManaReplenish += m_nManaReplenishExt;

    if (IS_PLAYER(m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)this;

        nManaReplenish += pPlayer->m_nCurrentSpirit * pPlayer->m_nSpiritToManaReplenishCof / KILO_NUM;
        nManaReplenish += pPlayer->m_nCurrentSpunk * pPlayer->m_nSpunkToManaReplenishCof / KILO_NUM;
    }

    if (nLifeReplenish > 0)
    {
        m_nCurrentLife += nLifeReplenish;
    }

    if (nManaReplenish > 0)
    {
        m_nCurrentMana += nManaReplenish;
    }

    m_nCurrentRage += m_nRageReplenish;

#ifdef _SERVER
    if (m_nUpperWeak > 0)
    {
        m_nUpperWeak = m_nUpperWeak * 9 / 10;
        MAKE_IN_RANGE(m_nUpperWeak, 0, WEAK_MAX);
        WEAK_VALUE_TO_LEVEL(m_nUpperWeakLevel, m_nUpperWeak);
    }

    if (m_nMiddleWeak > 0)
    {
        m_nMiddleWeak = m_nMiddleWeak * 9 / 10;
        MAKE_IN_RANGE(m_nMiddleWeak, 0, WEAK_MAX);
        WEAK_VALUE_TO_LEVEL(m_nMiddleWeakLevel, m_nMiddleWeak);
    }
    
    if (m_nLowerWeak > 0)
    {
        m_nLowerWeak = m_nLowerWeak * 9 / 10;
        MAKE_IN_RANGE(m_nLowerWeak, 0, WEAK_MAX);
        WEAK_VALUE_TO_LEVEL(m_nLowerWeakLevel, m_nLowerWeak);
    }
#endif

    m_nNextReplenishFrame = g_pSO3World->m_nGameLoop + GAME_FPS;
}

void KCharacter::ReviseAttribValue()
{
    AttribReplenish();

    MAKE_IN_RANGE(m_nCurrentLife, 1, m_nMaxLife); // 人物的血量最少是1
    MAKE_IN_RANGE(m_nCurrentMana, 0, m_nMaxMana);
    MAKE_IN_RANGE(m_nCurrentRage, 0, m_nMaxRage);
}

#ifdef _SERVER

void KCharacter::AddBullet(KSKILL_BULLET* pBullet)
{
    KSKILL_BULLET* pPreNode = NULL;
    KSKILL_BULLET* pNode    = NULL;

    assert(!pBullet->pNext);
    

    pNode = m_pBulletListHead;

    while (pNode)
    {
        if (pNode->nEndFrame > pBullet->nEndFrame)
        {
            break;
        }

        pPreNode = pNode;
        pNode = pNode->pNext;
    }

    if (pPreNode)
    {
        pBullet->pNext  = pPreNode->pNext;
        pPreNode->pNext = pBullet;
    }
    else
    {
        pBullet->pNext = m_pBulletListHead;
        m_pBulletListHead = pBullet;
    }
}

void KCharacter::ClearBullet()
{
    KSKILL_BULLET* pBullet = NULL;

    while (m_pBulletListHead)
    {
        pBullet = m_pBulletListHead;
        
        m_pBulletListHead = m_pBulletListHead->pNext;
        
        KMemory::Delete(pBullet);
        pBullet = NULL;
    }
}

void KCharacter::CheckBullet()
{
    KSKILL_BULLET* pBullet = NULL;

    KG_PROCESS_ERROR(m_pBulletListHead);

    KG_PROCESS_ERROR(m_pBulletListHead->nEndFrame <= g_pSO3World->m_nGameLoop);

    pBullet = m_pBulletListHead;

    m_pBulletListHead = m_pBulletListHead->pNext;

    ProcessBullet(pBullet);
   
    KMemory::Delete(pBullet);
    pBullet = NULL;
    
Exit0:
    return;
}

void KCharacter::ProcessBullet(KSKILL_BULLET *pBullet)
{
    KCharacter*      pCaster        = NULL;
    KSkill*          pSkill         = NULL;
    int              nX             = 0;
    int              nY             = 0;
    int              nZ             = 0;

    KGLOG_PROCESS_ERROR(pBullet);
       
    pCaster = pBullet->pSkillSrc;
    KGLOG_PROCESS_ERROR(pCaster);
    KG_PROCESS_ERROR(pCaster->m_dwID == pBullet->dwSkillSrcID);
    
    pSkill = (pBullet->SkillRecipePointer).GetPointer();
    KGLOG_PROCESS_ERROR(pSkill);

    switch (pSkill->m_pBaseInfo->nCastMode)
    {
    case scmSector:            
        assert(false); // 目前实现上,实际不会走到这儿!
        pSkill->ApplyOnSector(pBullet);
        break;
    case scmCasterArea:
        assert(false); // 目前实现上,实际不会走到这儿!
        pSkill->ApplyOnArea(pBullet, pCaster->m_nX, pCaster->m_nY);
        break;
    case scmItem:
        assert(false); // 都对物品释放了，不可能在人的身上
        break;
    case scmTargetArea:
        pSkill->ApplyOnSingle(pBullet);
        pSkill->ApplyOnArea(pBullet, m_nX, m_nY);
        break;
    case scmCasterSingle:
        assert(false); // 目前实现上,实际不会走到这儿!
        pSkill->ApplyOnSingle(pBullet);
        break;
    case scmTargetSingle:
        pSkill->ApplyOnSingle(pBullet);
        break;
    case scmTargetChain:
        pSkill->ApplyOnChain(pBullet);
        break;
    case scmPoint:
        pSkill->ApplyOnPoint(pBullet);
        break;
    case scmPointArea:
        pSkill->ApplyOnArea(pBullet, m_nX, m_nY);
        break;
    case scmTargetLeader:
        assert(false);
        break;
    case scmTeamArea:
        assert(false);
        break;
    default:
        KG_ASSERT_EXIT(false);
    }

Exit0:
    return;
}

KSKILL_HIT_RESULT KCharacter::JudgeSkillHit(KSKILL_BULLET* pBullet)
{
    int             nResult                 = shrInvalid;
    int             nRetCode                = false;
    int             nHitValue               = 0;
    int             nMissValue              = 0;
    int             nDodgeValue             = 0;
    int             nParryValue             = 0;
    int             nInsightValue           = 0;
    int             nCriticalStrikeValue    = 0;
    int             nCriticalStrikeBase     = 0;
    int             nDefCriticalStrikeValue = 0;
    int             nSkillKindType          = sktInvalid;
    int             nLevelMargin            = 0;
    int             nRandomValue            = 0;
    int             nCurrentInterval        = 0;
    BOOL            bSrcIsPlayer            = false;
    BOOL            bDstIsPlayer            = false;
    KSkill*         pSkill                  = NULL;
    KCharacter*     pSkillSrc               = NULL;
    int             nDenominator            = 0;

    pSkill = (pBullet->SkillRecipePointer).GetPointer();
    KGLOG_PROCESS_ERROR(pSkill);

    if (IS_PLAYER(pBullet->dwSkillSrcID))
    {
        pSkillSrc = (KCharacter*)g_pSO3World->m_PlayerSet.GetObj(pBullet->dwSkillSrcID);
    }
    else
    {
        pSkillSrc = (KCharacter*)g_pSO3World->m_NpcSet.GetObj(pBullet->dwSkillSrcID);
    }
    KGLOG_PROCESS_ERROR(pSkillSrc);

    nSkillKindType = pSkill->m_pBaseInfo->nKindType;

    if (pSkill->m_pBaseInfo->bIsExactHit)   // 一定命中，则跳过闪避，Miss和招架
    {
        goto Exit1;
    }

    switch (nSkillKindType)
    {
    case sktPhysics:
        nHitValue = pBullet->nPhysicsHitValue;
        break;
    case sktSolarMagic:
        nHitValue = pBullet->nSolarHitValue;
        break;
    case sktNeutralMagic:
        nHitValue = pBullet->nNeutralHitValue;
        break;
    case sktLunarMagic:
        nHitValue = pBullet->nLunarHitValue;
        break;
    case sktPoison:
        nHitValue = pBullet->nPoisonHitValue;
        break;
    default:
        break;
    }

    // -------------------- 计算等级差 -------------------------------------->>
        nLevelMargin = m_nLevel - pSkillSrc->m_nLevel;

    // -------------------- Miss -------------------------------------------->>

    bSrcIsPlayer = IS_PLAYER(pSkillSrc->m_dwID);
    bDstIsPlayer = IS_PLAYER(m_dwID);

    if (bSrcIsPlayer && !bDstIsPlayer) // Player -> Npc
    {
        if (nLevelMargin <= -3)
        {
            nHitValue = nHitValue + (-2 - nLevelMargin) * 100;
        } // [-2, 0] 不做运算
        else if (nLevelMargin >= 0 && nLevelMargin <= 2)
        {
            nHitValue = nHitValue - nLevelMargin * 250;
        }
        else if (nLevelMargin > 2)
        {
            nHitValue = nHitValue - 1000 * (nLevelMargin - 1);
        }
    }

    if (bSrcIsPlayer && bDstIsPlayer) // Player -> Player
    {
        nLevelMargin = min(nLevelMargin, 20);
        if (nLevelMargin <= -4)
        {
            nHitValue = nHitValue + (-3 - nLevelMargin) * 100;
        } // [-3, 0] 不做运算
        else if (nLevelMargin >= 0 && nLevelMargin <= 3)
        {
            nHitValue = nHitValue - nLevelMargin * 200;
        }
        else if (nLevelMargin > 3)
        {
            nHitValue = nHitValue - 350 * (nLevelMargin - 1);
        }
    }

    if (!bSrcIsPlayer && bDstIsPlayer) // Npc -> Player
    {
        nLevelMargin = min(nLevelMargin, 20);
        if (nLevelMargin <= -4)
        {
            nHitValue = nHitValue + (-3 - nLevelMargin) * 100;
        } // [-3, 0] 不做运算
        else if (nLevelMargin >= 0 && nLevelMargin <= 3)
        {
            nHitValue = nHitValue - nLevelMargin * 200;
        }
        else if (nLevelMargin > 3)
        {
            nHitValue = nHitValue - 350 * (nLevelMargin - 1);
        }
    }

    if (!bSrcIsPlayer && !bDstIsPlayer) // Npc -> Npc
    {
        if (nLevelMargin <= -4)
        {
            nHitValue = nHitValue + (-3 - nLevelMargin) * 100;
        } // [-3, 0] 不做运算
        else if (nLevelMargin >= 0 && nLevelMargin <= 3)
        {
            nHitValue = nHitValue - nLevelMargin * 200;
        }
        else if (nLevelMargin > 3)
        {
            nHitValue = nHitValue - 350 * (nLevelMargin - 1);
        }
    }

    MAKE_IN_RANGE(nHitValue, 1000, MAX_HIT_VALUE);

    nMissValue = MAX_HIT_VALUE - nHitValue;

    if (nSkillKindType == sktNone)
    {
        nMissValue = 0;
    }
    // <<----------------------------------------------------------------------

    if (nSkillKindType == sktPhysics && pSkill->m_pBaseInfo->nEffectType == scetHarmful) // 只有外功有躲闪和招架
    {
        MAKE_IN_RANGE(nLevelMargin, 0, 20);
        // -------------------- 躲闪值计算 -------------------------------------->>
        nDenominator = m_nDodge + 59 * (m_nLevel - nLevelMargin) + 380;
        KGLOG_PROCESS_ERROR(nDenominator > 0);
        nDodgeValue = m_nDodgeBaseRate + MAX_HIT_VALUE * m_nDodge / nDenominator;

        // -------------------- 招架值计算 -------------------------------------->>
        nDenominator = m_nParry + 30 * m_nLevel + 50;
        KGLOG_PROCESS_ERROR(nDenominator > 0);
        nParryValue = m_nParryBaseRate + MAX_HIT_VALUE * m_nParry / nDenominator;
    }

Exit1:
    // -------------------- 识破值计算 -------------------------------------->>
    if (bSrcIsPlayer && !bDstIsPlayer && pSkill->m_pBaseInfo->nEffectType == scetHarmful)   // Player -> Npc
    {
        int PlayerStrain = 0;
        KGLOG_PROCESS_ERROR(pSkillSrc->m_nLevel > 0);
        PlayerStrain    = MAX_HIT_VALUE * pSkillSrc->m_nStrain / (pSkillSrc->m_nStrain + 27 * pSkillSrc->m_nLevel);

        nInsightValue = m_nSense - PlayerStrain;
        MAKE_IN_RANGE(nInsightValue, 0, MAX_HIT_VALUE);
    }

    // -------------------- 会心值计算 -------------------------------------->>
    nCriticalStrikeValue 	= pBullet->nCriticalStrike;
	
    if (pSkill->m_pBaseInfo->nEffectType != scetBeneficial)     // 增益技能不计算自身御劲值
    {
        nDenominator = m_nToughness + 74 * m_nLevel + 320;
        KGLOG_PROCESS_ERROR(nDenominator > 0);
        nDefCriticalStrikeValue = m_nToughnessBaseRate + MAX_HIT_VALUE * m_nToughness / nDenominator;
    }

	nCriticalStrikeValue = max((nCriticalStrikeValue - nDefCriticalStrikeValue), 0);

    if (nSkillKindType == sktNone || !pSkill->m_pBaseInfo->bHasCriticalStrike)
    {
        nCriticalStrikeValue = 0;
    }

    // -------------------- 确定随机区间 ------------------------------------>>
    nRandomValue = g_Random(MAX_HIT_VALUE);

    KG_PROCESS_ERROR_RET_CODE(nRandomValue >= nMissValue, shrMiss);
    nRandomValue -= nMissValue;

    KG_PROCESS_ERROR_RET_CODE(nRandomValue >= nDodgeValue, shrDodge);
    nRandomValue -= nDodgeValue;

    KG_PROCESS_ERROR_RET_CODE(nRandomValue >= nParryValue, shrParry);
    nRandomValue -= nParryValue;

    KG_PROCESS_ERROR_RET_CODE(nRandomValue >= nInsightValue, shrInsight);
    nRandomValue -= nInsightValue;

    KG_PROCESS_ERROR_RET_CODE(nRandomValue >= nCriticalStrikeValue, shrCriticalStrike);
    nRandomValue -= nRandomValue;

    nResult = shrHit;
Exit0:
    return (KSKILL_HIT_RESULT)nResult; 
}

BOOL KCharacter::ApplyBullet(KSKILL_BULLET *pBullet)
{
    BOOL                        bResult                     = false;
    int                         nRetCode                    = false;
    int                         nSkillFunctionType          = sftInvalid;
    int                         nWeakValue                  = 0;
    int                         nDamageValue                = 0;
    BOOL                        bRollbackAttrib             = false;
    BOOL                        bRestoreOldParam            = false;
    BOOL                        bShield                     = false;
    BOOL                        bCriticalStrike             = false;
    BOOL                        bDstIsPlayer                = false;
    BOOL                        bSrcIsPlayer                = false;
    BOOL                        bBehitInOTAction            = false;
    KSkill*                     pSkill                      = NULL;
    KCharacter*                 pSkillSrc                   = NULL;

    KSKILL_CALCULATE_RESULT*    pSrcOldResult               = NULL;
    KSKILL_CALCULATE_RESULT*    pDstOldResult               = NULL;
    KSKILL_CALCULATE_RESULT     SrcResult;
    KSKILL_CALCULATE_RESULT     DstResult;

    KSKILL_ATTRIBUTE_PARAM*     pSrcOldSkillAttributeParam  = NULL;
    KSKILL_ATTRIBUTE_PARAM*     pDstOldSkillAttributeParam  = NULL;
    KSKILL_ATTRIBUTE_PARAM      SkillAttributeParam;

    KSKILL_HIT_RESULT           eHitResult                  = shrInvalid;
    KTarget                     Target;

    KG_PROCESS_ERROR(m_pScene);

    assert(pBullet);

    pSkill = (pBullet->SkillRecipePointer).GetPointer();
    KG_ASSERT_EXIT(pSkill);

    nRetCode = Target.SetTarget((KCharacter*)this);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = pSkill->CheckTargetState(Target);
    KG_PROCESS_ERROR(nRetCode == srcSuccess);

    if (IS_PLAYER(pBullet->dwSkillSrcID))
    {
        pSkillSrc = (KCharacter*)g_pSO3World->m_PlayerSet.GetObj(pBullet->dwSkillSrcID);
    }
    else
    {
        pSkillSrc = (KCharacter*)g_pSO3World->m_NpcSet.GetObj(pBullet->dwSkillSrcID);
    }

    assert(pSkillSrc);
    nRetCode = pSkill->CheckTargetType(pSkillSrc, Target);
    KG_PROCESS_ERROR(nRetCode == srcSuccess);

    bDstIsPlayer = IS_PLAYER(m_dwID);
    bSrcIsPlayer = IS_PLAYER(pSkillSrc->m_dwID);

    // -------------------- 给目标应用回滚的魔法属性 ------------------------>>
    ApplyAttribute(pSkill->m_pAttrbuteEffectToDestAndRollback);
    bRollbackAttrib = true;

    // -------------------- 保存旧的数据 ------------------------------------>>
    pSrcOldResult = m_pSrcSkillCalculateResult;
    pDstOldResult = m_pDstSkillCalculateResult;

    pSrcOldSkillAttributeParam = pSkillSrc->m_pSkillAttributeParam;
    pDstOldSkillAttributeParam = m_pSkillAttributeParam;

    bRestoreOldParam = true;

    // -------------------- 设置新的参数 ------------------------------------>>
    // 约定: 如果src和dst是同一个人，则运算结果存放到dst(this)上
    m_pSrcSkillCalculateResult = &SrcResult;
    m_pDstSkillCalculateResult = &DstResult;

    pSkillSrc->m_pSkillAttributeParam   = &SkillAttributeParam;
    m_pSkillAttributeParam              = &SkillAttributeParam;

    memset(&SkillAttributeParam, 0, sizeof(SkillAttributeParam));

    memset(&SrcResult, 0, sizeof(SrcResult));
    SrcResult.EffectSrc.eType = setSkill;
    SrcResult.EffectSrc.dwID  = pSkill->m_pBaseInfo->dwSkillID;
    SrcResult.EffectSrc.nLevel = pSkill->m_dwLevel;

    memset(&DstResult, 0, sizeof(DstResult));
    DstResult.EffectSrc.eType = setSkill;
    DstResult.EffectSrc.dwID  = pSkill->m_pBaseInfo->dwSkillID;
    DstResult.EffectSrc.nLevel = pSkill->m_dwLevel;

    // -------------------- 系统无敌，系统内部提供的 ------------------------>>
    if (m_bSystemShield)
    {
        m_pDstSkillCalculateResult->bShield = true;
        goto Exit1;
    }

    // -------------------- 设置一个0仇恨 ----------------------------------->>
    if (pSkill->m_pBaseInfo->nEffectType == scetHarmful)
    {
        if (pSkillSrc != this) // 不能自己仇恨自己
        {
            nRetCode = m_SimpThreatList.ModifyThreat(thtSpecialThreat, pSkillSrc, 0);
            KGLOG_CHECK_ERROR(nRetCode);
        }
    }
    
    if (pSkill->m_pBaseInfo->nEffectType == scetBeneficial)
    {
        nRetCode = m_SimpThreatList.ModifyThreaten(thtSpecialThreat, pSkillSrc, 0);
        KGLOG_CHECK_ERROR(nRetCode);
    }

    // -------------------- AI被击事件 -------------------------------------->>
    if (
        pSkill->m_pBaseInfo->nEffectType == scetHarmful &&  // 非减益效果的技能不通知
        pSkillSrc->m_dwID != m_dwID                         // 技能释放者不存在，或者技能释放者是自己，不通知
    )
    {
        if (pSkillSrc->m_dwID == m_SelectTarget.GetTargetID())
            m_SimpThreatList.UpdateKeepThreatField();

        this->m_AIData.ChoiceTarget[aittAttackTarget].SetTarget(pSkillSrc);
        m_AIVM.FireEvent(aevOnAttacked, pSkillSrc->m_dwID, 0);
    }

    // -------------------- 无敌判定 ---------------------------------------->>
    if (m_bPositiveShield && !pSkill->m_pBaseInfo->bIgnorePositiveShield)
    {
        bShield = true;
    }

    if (m_bNegativeShield && !pSkill->m_pBaseInfo->bIgnoreNegativeShield)
    {
        bShield = true;
    }

    if (bShield)
    {
        m_pDstSkillCalculateResult->bShield = true;
        goto Exit1;
    }

    // -------------------- 攻击结果判定 ------------------------------------>>
    eHitResult = JudgeSkillHit(pBullet);
    KGLOG_PROCESS_ERROR(eHitResult > shrInvalid && eHitResult < shrTotal);

    m_SkillDebug.Trace(
        1, "[技能调试][%d]:[技能流程] SkillResult:(%s-%u,%u) %d From(%s-%u) To(%s-%u)",
        g_pSO3World->m_nGameLoop, pSkill->m_pBaseInfo->szSkillName,
        pSkill->m_pBaseInfo->dwSkillID, pSkill->m_dwLevel,
        eHitResult,
        pSkillSrc->m_szName, pSkillSrc->m_dwID,
        m_szName, m_dwID
    );

    if (eHitResult == shrMiss)
    {
        m_pDstSkillCalculateResult->bHitTarget = false;
        m_pSrcSkillCalculateResult->bHitTarget = false;
        goto Exit1;
    }

    m_pDstSkillCalculateResult->bHitTarget = true;
    m_pSrcSkillCalculateResult->bHitTarget = true;

    if (eHitResult == shrDodge)
    {
        m_pDstSkillCalculateResult->bSkillDodge = true;
        goto Exit1;
    }

    if (eHitResult == shrParry)
    {
        m_pDstSkillCalculateResult->bParry = true;
        SkillAttributeParam.bParray = true;
    }

    if (eHitResult == shrInsight)
    {
        m_pDstSkillCalculateResult->bInsight = true;
        SkillAttributeParam.bInsight = true;
    }

    if (eHitResult == shrCriticalStrike)
    {
        m_pDstSkillCalculateResult->bCriticalStrike = true;
        bCriticalStrike = true;
    }

    // -------------------- 基础 -------------------------------------------->>

    if (pSkill->m_pBaseInfo->nEffectType == scetHarmful)
    {
        if (pSkillSrc != this) // 不能自己仇恨自己
        {
            int nThreat = pSkill->m_nBaseThreat * pBullet->nBaseThreatCoefficient / KILO_NUM;

            nRetCode = m_SimpThreatList.ModifyThreat(thtBaseThreat, pSkillSrc, nThreat);
            KGLOG_CHECK_ERROR(nRetCode);
        }
    }

    if (pSkill->m_pBaseInfo->nEffectType == scetBeneficial)
    {
        int nThreat = pSkill->m_nBaseThreat * pBullet->nBaseThreatCoefficient / KILO_NUM;

        nRetCode = m_SimpThreatList.ModifyThreaten(thtBaseThreat, pSkillSrc, nThreat);
        KGLOG_CHECK_ERROR(nRetCode);
    }

    // -------------------- 打下马的几率计算 -------------------------------->>
    if (bDstIsPlayer)
    {
        KPlayer* pDstPlayer = (KPlayer*)this;

        if (pSkill->m_nDismountingRate > 0 && pDstPlayer->m_bOnHorse)
        {
            int DismountingRate = (KILO_NUM - pDstPlayer->m_nMountingRate) * pSkill->m_nDismountingRate / KILO_NUM;
            int nRandom         = g_Random(KILO_NUM);

            if (nRandom < DismountingRate)
            {
                nRetCode = pDstPlayer->DownHorse();
                KGLOG_PROCESS_ERROR(nRetCode);
            }

            m_SkillDebug.Trace(
                2, "[技能调试][%d]:[技能流程] (%s-%u) 被击落:<%s> By %s-%u From %s-%u, 防落率:%d, 击落值:%d 随机值:%d",
                g_pSO3World->m_nGameLoop, m_szName, m_dwID, (nRandom < DismountingRate) ? "OK" : "FAILED",
                pSkill->m_pBaseInfo->szSkillName, pSkill->m_pBaseInfo->dwSkillID,
                pSkillSrc->m_szName, pSkillSrc->m_dwID,
                pDstPlayer->m_nMountingRate, pSkill->m_nDismountingRate, nRandom
            );
        }
    }

    // -------------------- 判断命中时是否为吟唱状态 ------------------------>>
    if (m_OTActionParam.eActionType == otActionSkillPrepare || m_OTActionParam.eActionType == otActionSkillChannel)
    {
        bBehitInOTAction = true;
    }

    // -------------------- 设置技能计算全局参数 ---------------------------->>
    SkillAttributeParam.SkillRecipePointer              = pBullet->SkillRecipePointer;
    SkillAttributeParam.pSkillSrc                       = pSkillSrc;
    SkillAttributeParam.dwSkillSrcID                    = pBullet->dwSkillSrcID;
    SkillAttributeParam.nSkillSrcLevel                  = pBullet->nSkillSrcLevel;
    SkillAttributeParam.pSkillDst                       = this;
    SkillAttributeParam.pBullet                         = pBullet;
    SkillAttributeParam.bCriticalStrike                 = bCriticalStrike;
    SkillAttributeParam.nPhysicsDamage                  = pBullet->nPhysicsDamage;
    SkillAttributeParam.nSolarDamage                    = pBullet->nSolarDamage;
    SkillAttributeParam.nNeutralDamage                  = pBullet->nNeutralDamage;
    SkillAttributeParam.nLunarDamage                    = pBullet->nLunarDamage;
    SkillAttributeParam.nPoisonDamage                   = pBullet->nPoisonDamage;
    SkillAttributeParam.nDamgeToLifeForSelf             = pBullet->nDamageToLifeForSelf;
    SkillAttributeParam.nDamgeToManaForSelf             = pBullet->nDamageToManaForSelf;
    SkillAttributeParam.nTherapy                        = pBullet->nTherapy;
    SkillAttributeParam.nCriDamagePower                 = pBullet->nCriticalStrikePower;
    SkillAttributeParam.nBaseThreatCoefficient          = pBullet->nBaseThreatCoefficient;
    SkillAttributeParam.nDamageThreatCoefficient        = pBullet->nDamageThreatCoefficient;
    SkillAttributeParam.nTherapyThreatCoefficient       = pBullet->nTherapyThreatCoefficient;
    SkillAttributeParam.bDot                            = false;
    SkillAttributeParam.nDamageAddPercent               = pSkill->m_nDamageAddPercent;
    SkillAttributeParam.dwAddDamageByDstMoveStateMask   = pBullet->dwAddDamageByDstMoveStateMask;
    SkillAttributeParam.nAddDamagePercentByDstMoveState = pBullet->nAddDamagePercentByDstMoveState;

    // -------------------- 目标不可回滚魔法属性应用 ------------------------>>
    ApplyAttribute(pSkill->m_pAttrbuteEffectToDestNotRollback);

    // -------------------- 破绽 -------------------------------------------->>
    switch(pSkill->m_nAttackWeakPos)
    {
    case swpUpper:
        nWeakValue = m_nUpperWeak;
        break;
    case swpMiddle:
        nWeakValue = m_nMiddleWeak;
        break;
    case swpLower:
        nWeakValue = m_nLowerWeak;
        break;
    case swpNone:
        nWeakValue = 0;
        break;
    }

    if (nWeakValue > 0 && nWeakValue <= WEAK_MINOR)
    {
        ApplyAttribute(pSkill->m_pAttrbuteEffectToDestOnMinorWeak);
    }
    else if (nWeakValue > WEAK_MINOR && nWeakValue <= WEAK_MAJOR)
    {
        ApplyAttribute(pSkill->m_pAttrbuteEffectToDestOnMajorWeak);
    }
    else if (nWeakValue > WEAK_MAJOR)
    {
        ApplyAttribute(pSkill->m_pAttrbuteEffectToDestOnMaxWeak);
    }

    // -------------------- 施法者自身应用不可回滚的魔法属性，不考虑会心 ---->>
    SkillAttributeParam.bCriticalStrike = false;

    pSkillSrc->ApplyAttribute(pSkill->m_pAttrbuteEffectToSelfNotRollback);

    // -------------------- 伤害统计 ---------------------------------------->>
    nRetCode = ConcludeResult(pBullet->dwBulletID, bCriticalStrike, &nDamageValue);
    KG_PROCESS_ERROR(nRetCode);

    // -------------------- Buff处理 ---------------------------------------->>
    for (int i = 0; i < sizeof(pSkill->m_BindBuffs) / sizeof(pSkill->m_BindBuffs[0]); i++)
    {
        KBUFF_RECIPE_KEY& rBuffRecipeKey = pSkill->m_BindBuffs[i].GetRecipeKey();

        if (rBuffRecipeKey.dwID != 0)
        {
            m_BuffList.CallBuff(pSkillSrc->m_dwID, pSkillSrc->m_nLevel, rBuffRecipeKey, 0, 0);
        }
    }

    // -------------------- 吟唱打退 ---------------------------------------->>
    if (pSkill->m_pBaseInfo->bCauseBeatBack && nDamageValue < 0)
    {
        int nBeatBackCount = m_OTActionParam.nBeatBackCount;
        int nFrame         = pSkill->m_nPrepareFrames + pSkill->m_nChannelFrame;

        CheckBeatBack();

        if (nBeatBackCount < m_OTActionParam.nBeatBackCount)
        {
            m_SkillDebug.Trace(
                2, "[技能调试][%d]:[技能流程] Skill:(%s-%u,%u) 打退:<成功>。已被打退<%d>次, Frame：%d/%d From(%s-%u) To(%s-%u)",
                g_pSO3World->m_nGameLoop,
                pSkill->m_pBaseInfo->szSkillName, pSkill->m_pBaseInfo->dwSkillID, pSkill->m_dwLevel,
                m_OTActionParam.nBeatBackCount,
                m_OTActionParam.nPrepareFrame, nFrame,
                pSkillSrc->m_szName, pSkillSrc->m_dwID,
                m_szName, m_dwID
            );
        }
    }

    // -------------------- 打断处理 ---------------------------------------->>
    if (pSkill->m_pBaseInfo->bCauseBeatBreak)
    {
        CheckBeatBreak(pSkill);
    }

    // -------------------- 装备损耗处理 ------------------------------------>>
    if (pSkill->m_pBaseInfo->bCauseAbradeEquipment)
    {
        if (IS_PLAYER(m_dwID))
        {
            KPlayer* pPlayer = (KPlayer*)this;
            pPlayer->m_ItemList.AbradeArmor();
        }

        if (IS_PLAYER(pSkillSrc->m_dwID))
        {
            KPlayer* pPlayer = (KPlayer*)pSkillSrc;
            int nWeaponGenre = 
                g_pSO3World->m_Settings.m_WeaponTypeList.GetWeaponGenre(pSkill->m_pBaseInfo->dwWeaponRequest);
            if (nWeaponGenre == swrtMeleeWeapon)
            {
                pPlayer->m_ItemList.AbradeItemAtEquipBox(eitMeleeWeapon);
            }    
             
            if (nWeaponGenre == swrtRangeWeapon)
            {
                pPlayer->m_ItemList.AbradeItemAtEquipBox(eitRangeWeapon);
            }
        }
    }

Exit1:

    g_PlayerServer.DoSkillEffectResult(pSkillSrc, this, this, m_pDstSkillCalculateResult, pBullet->dwBulletID);
    g_PlayerServer.DoSyncBehitRepresent(pSkillSrc, this, this, m_pDstSkillCalculateResult, pBullet->dwBulletID);

    bResult = true;

Exit0:

    // 恢复旧参数
    if (bRestoreOldParam)
    {
        m_pSrcSkillCalculateResult          = pSrcOldResult;
        m_pDstSkillCalculateResult          = pDstOldResult;

        pSkillSrc->m_pSkillAttributeParam   = pSrcOldSkillAttributeParam;
        m_pSkillAttributeParam              = pDstOldSkillAttributeParam;

        bRestoreOldParam = false;
    }

    if (bRollbackAttrib)
    {
        UnApplyAttribute(pSkill->m_pAttrbuteEffectToDestAndRollback);
        bRollbackAttrib = false;
    }

    // -------------------- 正常结束的最后，处理技能事件 -------------------->>
    if (bResult)
    {
        if (eHitResult == shrMiss)
        {
            pSkillSrc->OnSkillEvent(pSkill, seMiss, pBullet->SkillEventVector, pSkillSrc, this);
            OnSkillEvent(pSkill, seBeMiss, m_SkillEventVector, this, pSkillSrc);
        }

        if (eHitResult == shrDodge)
        {
            pSkillSrc->OnSkillEvent(pSkill, seBeDodge, pBullet->SkillEventVector, pSkillSrc, this);
            OnSkillEvent(pSkill, seDodge, m_SkillEventVector, this, pSkillSrc);
        }

        if (bBehitInOTAction)
        {
            pSkillSrc->OnSkillEvent(pSkill, seHitOTAction, pBullet->SkillEventVector, pSkillSrc, this);
            OnSkillEvent(pSkill, seBeHitOTAction, m_SkillEventVector, this, pSkillSrc);
        }

        if (eHitResult == shrHit || eHitResult == shrCriticalStrike)
        {
            pSkillSrc->OnSkillEvent(pSkill, seHit, pBullet->SkillEventVector, pSkillSrc, this);
            OnSkillEvent(pSkill, seBeHit, m_SkillEventVector, this, pSkillSrc);
        }

        if (eHitResult == shrCriticalStrike)
        {
            pSkillSrc->OnSkillEvent(pSkill, seCriticalStrike, pBullet->SkillEventVector, pSkillSrc, this);
            OnSkillEvent(pSkill, seBeCriticalStrike, m_SkillEventVector, this, pSkillSrc);
        }
    }

    return bResult;
}

BOOL KCharacter::ConcludeResult(DWORD dwBulletID, BOOL bCriticalStrikeFlag, int* pnRetDamageValue)
{
    BOOL        bResult                 = false;
    BOOL        bRetCode                = false;
    KCharacter* pSkillSrc               = NULL;
    KSkill*     pSkill                  = NULL;
    int         nDst_DeltaLife          = 0;
    int         nSrc_DeltaLife          = 0;
    int         nBeTherapyCoefficient   = 0;
    int         nDst_EffectiveTherapy   = 0;
    int         nSrc_EffectiveTherapy   = 0;
    int         nDst_EffectiveDamage    = 0;
    int         nSrc_EffectiveDamage    = 0;
    char        szSkill[64];

    assert(m_pSkillAttributeParam);
    assert(m_pSrcSkillCalculateResult);
    assert(m_pDstSkillCalculateResult);

    KGLOG_PROCESS_ERROR(pnRetDamageValue);

    pSkill    = m_pSkillAttributeParam->SkillRecipePointer.GetPointer();
    pSkillSrc = m_pSkillAttributeParam->pSkillSrc;
    
    nBeTherapyCoefficient = max(m_nBeTherapyCoefficient, 0);

    // 修正被治疗的数值，该数值此处被人物身上的被治疗加成比例影响
    m_pDstSkillCalculateResult->nSkillEffectResult[serTherapy] = 
        m_pDstSkillCalculateResult->nSkillEffectResult[serTherapy] * nBeTherapyCoefficient / KILO_NUM;

    // 伤害吸收,抵消之类的已经在算Damage的时候扣除了,所以这里算DeltaLife的时候不要再重复计算!
    nDst_DeltaLife = 0
        + m_pDstSkillCalculateResult->nSkillEffectResult[serStealLife]
        + m_pDstSkillCalculateResult->nSkillEffectResult[serTherapy]
        - m_pDstSkillCalculateResult->nSkillEffectResult[serPhysicsDamage]
        - m_pDstSkillCalculateResult->nSkillEffectResult[serSolarMagicDamage]
        - m_pDstSkillCalculateResult->nSkillEffectResult[serNeutralMagicDamage]
        - m_pDstSkillCalculateResult->nSkillEffectResult[serLunarMagicDamage]
        - m_pDstSkillCalculateResult->nSkillEffectResult[serPoisonDamage]
        - m_pDstSkillCalculateResult->nSkillEffectResult[serReflectiedDamage]
    ;

    szSkill[0] = '\0';
    if (pSkill)
    {
        snprintf(szSkill, sizeof(szSkill), "(%s-%lu,%lu)", pSkill->m_pBaseInfo->szSkillName, pSkill->m_pBaseInfo->dwSkillID, pSkill->m_dwLevel);
        szSkill[sizeof(szSkill) - 1] = '\0';
    }

    m_SkillDebug.Trace(
        1, "[技能调试][%d]:[伤害] %s 外:%d 阳:%d 中:%d 阴:%d 毒:%d To(%s-%u)",
        g_pSO3World->m_nGameLoop,
        szSkill,
        m_pDstSkillCalculateResult->nSkillEffectResult[serPhysicsDamage],
        m_pDstSkillCalculateResult->nSkillEffectResult[serSolarMagicDamage],
        m_pDstSkillCalculateResult->nSkillEffectResult[serNeutralMagicDamage],
        m_pDstSkillCalculateResult->nSkillEffectResult[serLunarMagicDamage],
        m_pDstSkillCalculateResult->nSkillEffectResult[serPoisonDamage],
        m_szName, m_dwID
    );

    if (pSkillSrc)
    {
        nSrc_DeltaLife = 0
            + m_pSrcSkillCalculateResult->nSkillEffectResult[serStealLife]
            + m_pSrcSkillCalculateResult->nSkillEffectResult[serTherapy]
            - m_pSrcSkillCalculateResult->nSkillEffectResult[serPhysicsDamage]
            - m_pSrcSkillCalculateResult->nSkillEffectResult[serSolarMagicDamage]
            - m_pSrcSkillCalculateResult->nSkillEffectResult[serNeutralMagicDamage]
            - m_pSrcSkillCalculateResult->nSkillEffectResult[serLunarMagicDamage]
            - m_pSrcSkillCalculateResult->nSkillEffectResult[serPoisonDamage]
            - m_pSrcSkillCalculateResult->nSkillEffectResult[serReflectiedDamage]
        ;                                                           
    }

    *pnRetDamageValue = nDst_DeltaLife;

    // ---------- 数值范围修正 ---------------------------------------------------------

    //MAKE_IN_RANGE(nDst_DeltaLife, -m_nCurrentLife, m_nMaxLife - m_nCurrentLife);

	m_nCurrentLife += nDst_DeltaLife;
   
    if (nDst_DeltaLife < 0)
        nDst_EffectiveDamage = -nDst_DeltaLife + min(0, m_nCurrentLife);

    if (nDst_DeltaLife > 0)
        nDst_EffectiveTherapy = nDst_DeltaLife - max(0, m_nCurrentLife - m_nMaxLife);

    if (m_nCurrentLife > m_nMaxLife && pSkillSrc) // 过量治疗
    {
        OnSkillEvent(pSkill, seBeOverHeal, m_SkillEventVector, this, pSkillSrc);
        pSkillSrc->OnSkillEvent(pSkill, seOverHeal, pSkillSrc->m_SkillEventVector, pSkillSrc, this);
    }

    MAKE_IN_RANGE(m_nCurrentLife, 0, m_nMaxLife);
    MAKE_IN_RANGE(m_nCurrentMana, 0, m_nMaxMana);
    
    if (pSkillSrc && pSkillSrc != this)
    {   
        // 如果src和dst是同一个人，那么加血或扣血都放在dst上，以免重复操作
        //MAKE_IN_RANGE(nSrc_DeltaLife, -pSkillSrc->m_nCurrentLife, pSkillSrc->m_nMaxLife - pSkillSrc->m_nCurrentLife);
        pSkillSrc->m_nCurrentLife += nSrc_DeltaLife;

        if (nSrc_DeltaLife < 0)
            nSrc_EffectiveDamage = -nSrc_DeltaLife + min(0, pSkillSrc->m_nCurrentLife);

        if (nSrc_DeltaLife > 0)
            nSrc_EffectiveTherapy = nSrc_DeltaLife - max(0, pSkillSrc->m_nCurrentLife - pSkillSrc->m_nMaxLife);

        MAKE_IN_RANGE(pSkillSrc->m_nCurrentLife, 0, pSkillSrc->m_nMaxLife);
        MAKE_IN_RANGE(pSkillSrc->m_nCurrentMana, 0, pSkillSrc->m_nMaxMana);
    }

    // ------------------------------------------------------------------------------
    // 如果受到伤害, 则被攻击者身上的眩晕效果就消失

    if (nDst_DeltaLife < 0)
    {
        OnDamage(pSkillSrc, -nDst_DeltaLife);
    }

    if (nSrc_DeltaLife < 0)
    {
        pSkillSrc->OnDamage(this, -nSrc_DeltaLife);
    }
    
	// ---------- process threat list -------------------------------------------------

	if (pSkillSrc && pSkillSrc != this && pSkill && nDst_DeltaLife < 0)
	{
        // src 使 dst 受到伤害,所以dst会增加对src的仇恨
	    int nThreat = ((-nDst_DeltaLife) * pSkill->m_nThreatRevisePercent / KILO_NUM) * 
            m_pSkillAttributeParam->nDamageThreatCoefficient / KILO_NUM;

        bRetCode = m_SimpThreatList.ModifyThreat(thtDamageThreat, pSkillSrc, nThreat);
        KGLOG_CHECK_ERROR(bRetCode);
	}
	else if (pSkillSrc && pSkill && nDst_EffectiveTherapy > 0)
	{
        // src 使 dst 受到治疗,所以仇恨dst的人会仇恨src
        int nThreat = (nDst_EffectiveTherapy * pSkill->m_nThreatRevisePercent / KILO_NUM) * 
            m_pSkillAttributeParam->nTherapyThreatCoefficient / KILO_NUM / THERAPY_TO_THREAT;

        bRetCode = m_SimpThreatList.ModifyThreaten(thtTherapyThreat, pSkillSrc, nThreat);
        KGLOG_CHECK_ERROR(bRetCode);
	}

	if (pSkillSrc && pSkillSrc != this && pSkill && nSrc_DeltaLife < 0)
	{
        // src 对 dst 发技能,反而使src受到伤害,这会使src仇恨dst
        int nThreat = ((-nSrc_DeltaLife) * pSkill->m_nThreatRevisePercent / KILO_NUM) * 
            m_pSkillAttributeParam->nDamageThreatCoefficient / KILO_NUM;

        bRetCode = pSkillSrc->m_SimpThreatList.ModifyThreat(thtDamageThreat, this, nThreat);
        KGLOG_CHECK_ERROR(bRetCode);
	}
	else if (pSkillSrc && pSkillSrc != this && pSkill && nSrc_EffectiveTherapy > 0)
	{
        // src 对 dst 发技能,使 src获得了治疗(实际可能是吸取生命之类的), 会使所有仇恨src的人更仇恨src(不能说一定是dst仇恨src).
        int nThreat = (nSrc_EffectiveTherapy * pSkill->m_nThreatRevisePercent / KILO_NUM) * 
            m_pSkillAttributeParam->nTherapyThreatCoefficient / KILO_NUM / THERAPY_TO_THREAT;

        bRetCode = pSkillSrc->m_SimpThreatList.ModifyThreaten(thtTherapyThreat, pSkillSrc, nThreat);
        KGLOG_CHECK_ERROR(bRetCode);
	}

	// ------------------------- sync skill result -------------------------------------

    // 可能会出现Caster 受到的伤害(比如反弹伤害)和生命增益(比如偷取生命)恰好抵消,但是这种几率太小了,不作处理
    // 在生命值变化量nSrc_DeltaLife为0时,不作同步,注意,技能释放者在伤害流程中只会受到"反弹伤害"和"偷取生命"影响
    // 并不存在其他的诸如"格挡"之内的效果
    // 如果src和dst是同一个人，那么结果是存放到dst上的，这里就不用多一个同步了
    if (nSrc_DeltaLife && (pSkillSrc != this))
    {     
		bRetCode = g_PlayerServer.DoSkillEffectResult(
            pSkillSrc, this, pSkillSrc,
            m_pSrcSkillCalculateResult, 0
        );

        bRetCode = g_PlayerServer.DoSyncBehitRepresent(
            pSkillSrc, this, pSkillSrc,
            m_pSrcSkillCalculateResult, 0
        );
    }

    // ------------------- check die ---------------------------------------------------

    if ((m_nCurrentLife <= 0) && m_bImmortal)
    {
        m_nCurrentLife = 1;
    }

    if ((m_eMoveState != cmsOnDeath) && (m_nCurrentLife <= 0))
    {
        DWORD dwKiller = ERROR_ID;
        char  szKiller[_NAME_LEN];

        szKiller[0] = '\0';

        if (pSkillSrc)
        {
            dwKiller = pSkillSrc->m_dwID;
            strncpy(szKiller, pSkillSrc->m_szName, sizeof(szKiller));
            szKiller[sizeof(szKiller) - 1] = '\0';
        }

        bRetCode = GoToHell(dwKiller);

        if (bRetCode && pSkillSrc)
        {
            OnSkillEvent(pSkill, seBeKill, m_SkillEventVector, this, pSkillSrc);
            pSkillSrc->OnSkillEvent(pSkill, seKill, pSkillSrc->m_SkillEventVector, pSkillSrc, this);
        }

        m_SkillDebug.Trace(
            1, "(%s-%u)死亡了, 因为%s-%u 的 %s对其造成了%d点伤害.",
            m_szName, m_dwID,
            szKiller, dwKiller,
            szSkill,
            nDst_DeltaLife
        );
    }

    if (pSkillSrc && (pSkillSrc->m_nCurrentLife <= 0) && pSkillSrc->m_bImmortal)
    {
        pSkillSrc->m_nCurrentLife = 1;
    }

    if (pSkillSrc && (pSkillSrc->m_eMoveState != cmsOnDeath) && (pSkillSrc->m_nCurrentLife <= 0))
    {
        pSkillSrc->GoToHell(m_dwID);
        m_SkillDebug.Trace(
            1, "(%s-%u)死亡了, 因为%s-%u的 %s对其造成了%d点伤害.",
            pSkillSrc->m_szName, pSkillSrc->m_dwID,
            pSkillSrc->m_szName, pSkillSrc->m_dwID,
            szSkill,
            nSrc_DeltaLife
        );
    }

    // ------------------- PQ Statistics -----------------------------------------------
    if (pSkillSrc && IS_PLAYER(pSkillSrc->m_dwID))
    {
        KPlayer*                    pSkillSrcPlayer = (KPlayer*)pSkillSrc;
        KGPQ_STATISTICS_INCREMENT   StatisticsIncrementHarm(&g_pSO3World->m_PQManager, pSkillSrcPlayer->m_dwID, psiHarmOutput, nDst_EffectiveDamage);
        KGPQ_STATISTICS_INCREMENT   StatisticsIncrementThreap(&g_pSO3World->m_PQManager, pSkillSrcPlayer->m_dwID, psiTreatOutput, nDst_EffectiveTherapy);

        pSkillSrcPlayer->m_PQList.Traverse(StatisticsIncrementHarm);
        pSkillSrcPlayer->m_PQList.Traverse(StatisticsIncrementThreap);
    }

    if (IS_PLAYER(m_dwID))
    {
        KPlayer*                    pSkillDstPlayer = (KPlayer*)this;
        KGPQ_STATISTICS_INCREMENT   StatisticsIncrement(&g_pSO3World->m_PQManager, pSkillDstPlayer->m_dwID, psiInjury, nDst_EffectiveDamage);

        pSkillDstPlayer->m_PQList.Traverse(StatisticsIncrement);
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KCharacter::ProcessKnockedDown(int nFrame)
{
    BOOL            bResult     = false;
    int             nApplyOdds  = 0;
    KCharacter*     pSkillSrc   = NULL;

    KG_ASSERT_EXIT(m_pSkillAttributeParam);
    pSkillSrc = m_pSkillAttributeParam->pSkillSrc;
    KGLOG_PROCESS_ERROR(pSkillSrc);

    nApplyOdds = (int)g_Random(KILO_NUM);

    if (nApplyOdds + m_nKnockedDownRate >= KILO_NUM)
    {
        KnockedDown(nFrame);
        m_BuffList.CleanBuffByPersist();
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KCharacter::ProcessKnockedBackExhale(int nDistance, int nVelocityXY)
{
    BOOL            bResult     = false;
    int             nApplyOdds  = 0;
    KCharacter*     pSkillSrc   = NULL;

    KG_ASSERT_EXIT(m_pSkillAttributeParam);
    pSkillSrc = m_pSkillAttributeParam->pSkillSrc;
    KGLOG_PROCESS_ERROR(pSkillSrc);

    nApplyOdds = (int)g_Random(KILO_NUM);

    if (nApplyOdds + m_nKnockedBackRate >= KILO_NUM)
    {
        int nDirection = g_GetDirection(pSkillSrc->m_nX, pSkillSrc->m_nY, m_nX, m_nY);

        KnockedBack(nDirection, nDistance, nVelocityXY);
        m_BuffList.CleanBuffByPersist();
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KCharacter::ProcessKnockedBackConvergence(int nDistance, int nVelocityXY)
{
    BOOL            bResult     = false;
    int             nApplyOdds  = 0;
    KCharacter*     pSkillSrc   = NULL;

    KG_ASSERT_EXIT(m_pSkillAttributeParam);
    pSkillSrc = m_pSkillAttributeParam->pSkillSrc;
    KGLOG_PROCESS_ERROR(pSkillSrc);

    nApplyOdds = (int)g_Random(KILO_NUM);

    if (nApplyOdds + m_nKnockedBackRate >= KILO_NUM)
    {
        int nDirection = (int)g_GetDirection(m_nX, m_nY, pSkillSrc->m_nX, pSkillSrc->m_nY);

        KnockedBack(nDirection, nDistance, nVelocityXY);
        m_BuffList.CleanBuffByPersist();
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KCharacter::ProcessKnockedBackRandom(int nDistance, int nVelocityXY)
{
    BOOL            bResult     = false;
    int             nApplyOdds  = 0;
    KCharacter*     pSkillSrc   = NULL;

    KG_ASSERT_EXIT(m_pSkillAttributeParam);
    pSkillSrc = m_pSkillAttributeParam->pSkillSrc;
    KGLOG_PROCESS_ERROR(pSkillSrc);

    nApplyOdds = (int)g_Random(KILO_NUM);

    if (nApplyOdds + m_nKnockedBackRate >= KILO_NUM)
    {
        int nDirection = (int)g_Random(DIRECTION_COUNT);

        KnockedBack(nDirection, nDistance, nVelocityXY);
        m_BuffList.CleanBuffByPersist();
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KCharacter::ProcessKnockedBackSrcFace(int nDistance, int nVelocityXY)
{
    BOOL        bResult     = false;
    int         nApplyOdds  = 0;
    KCharacter* pSkillSrc   = NULL;

    KG_ASSERT_EXIT(m_pSkillAttributeParam);
    pSkillSrc = m_pSkillAttributeParam->pSkillSrc;
    KGLOG_PROCESS_ERROR(pSkillSrc);

    nApplyOdds = (int)g_Random(KILO_NUM);

    if (nApplyOdds + m_nKnockedBackRate >= KILO_NUM)
    {
        int nDirection = pSkillSrc->m_nFaceDirection;

        KnockedBack(nDirection, nDistance, nVelocityXY);
        m_BuffList.CleanBuffByPersist();
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KCharacter::ProcessKnockedOffParabola(int nDistance, int nVelocityXY)
{
    BOOL        bResult     = false;
    int         nApplyOdds  = 0;
    KCharacter* pSkillSrc   = NULL;

    KGLOG_PROCESS_ERROR(m_pSkillAttributeParam);
    pSkillSrc = m_pSkillAttributeParam->pSkillSrc;
    KGLOG_PROCESS_ERROR(pSkillSrc);

    KGLOG_PROCESS_ERROR(nDistance > 0);
    KGLOG_PROCESS_ERROR(nVelocityXY > 0);

    nApplyOdds = (int)g_Random(KILO_NUM);

    if (nApplyOdds + m_nKnockedOffRate >= KILO_NUM)
    {
        int nDirection = g_GetDirection(pSkillSrc->m_nX, pSkillSrc->m_nY, m_nX, m_nY);
        
        KnockedOff(nDirection, nDistance, nVelocityXY);
        m_BuffList.CleanBuffByPersist();
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KCharacter::ProcessPull(KCharacter* pCharacter, int nVelocityXY)
{
    BOOL bResult            = false;
    BOOL bRetCode           = 0;
    int  nApplyOdds         = 0;
    int  nDestX             = 0;
    int  nDestY             = 0;
    int  nDistanceCharacter = 0;
    int  nDistancePull      = 0;
    int  nVelocityZ         = 0;
    int  nFramePull         = 0;
    int  nDirectionPull     = 0;

    KGLOG_PROCESS_ERROR(pCharacter);
    KGLOG_PROCESS_ERROR(nVelocityXY > 0);

    nApplyOdds = (int)g_Random(KILO_NUM);

    KG_PROCESS_SUCCESS(nApplyOdds + m_nPullRate < KILO_NUM); // 这里的免疫判断和击退一样的数值

    nDistanceCharacter = m_nTouchRange + pCharacter->m_nTouchRange + CELL_LENGTH * 2; // 两个人间隔两个CELL

    nDestX = pCharacter->m_nX + nDistanceCharacter * g_Cos(pCharacter->m_nDirectionXY) / SIN_COS_NUMBER;
    nDestY = pCharacter->m_nY + nDistanceCharacter * g_Sin(pCharacter->m_nDirectionXY) / SIN_COS_NUMBER;

    bRetCode = EmendateDestCoordinate(nDestX, nDestY);
    KGLOG_PROCESS_ERROR(bRetCode);

    nDistancePull = g_GetDistance2(m_nX, m_nY, nDestX, nDestY);

    nDistancePull = (int)sqrt((float)nDistancePull);

    nFramePull = nDistancePull / nVelocityXY;

    if (nFramePull)
    {
        nVelocityZ = (pCharacter->m_nZ - m_nZ) / nFramePull;
    }

    nDirectionPull = g_GetDirection(m_nX, m_nY, nDestX, nDestY);

    bRetCode = Pull(nFramePull, nDirectionPull, nVelocityXY, nVelocityZ);
    KG_PROCESS_ERROR(bRetCode);
    m_BuffList.CleanBuffByPersist();

Exit1:
    bResult = true;
Exit0:
    return 0;
}

#endif

BOOL KCharacter::ProcessRepulsed(int nFrame)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    KCharacter* pSkillSrc   = NULL;
    int         nDirection  = 0;
    int         nApplyOdds  = 0;
    int         nVelocityXY = 0;

    KGLOG_PROCESS_ERROR(m_pSkillAttributeParam);

    pSkillSrc = m_pSkillAttributeParam->pSkillSrc;
    KGLOG_PROCESS_ERROR(pSkillSrc);

    nApplyOdds = (int)g_Random(KILO_NUM);

    KG_PROCESS_SUCCESS(nApplyOdds + m_nRepulsedRate < KILO_NUM); // 这里的免疫判断和击退一样的数值

    nDirection  = g_GetDirection(pSkillSrc->m_nX, pSkillSrc->m_nY, m_nX, m_nY);
    nVelocityXY = REPULSED_DECELERATION * nFrame;

    bRetCode = Repulsed(nFrame, nDirection, nVelocityXY);
    KG_PROCESS_ERROR(bRetCode);

    m_BuffList.CleanBuffByPersist();

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KCharacter::ProcessFear(int nDistance)
{
    BOOL        bResult               = false;
#ifdef _SERVER
    BOOL        bRetCode              = false;
    int         nLevelMargin          = 0;
    KCharacter* pSkillSrc             = NULL;
    const int   RESIST_PERCENT[13]    = {0, 16, 16, 16, 32, 32, 64, 128, 256, 512, 576, 640, 706};

    if (m_pSkillAttributeParam)
    {
        pSkillSrc = m_pSkillAttributeParam->pSkillSrc;
        KGLOG_PROCESS_ERROR(pSkillSrc);

        nLevelMargin = m_nLevel - pSkillSrc->m_nLevel + 6;
        MAKE_IN_RANGE(nLevelMargin, 0, 12);

        m_nResistBehaveConstraintPercent = RESIST_PERCENT[nLevelMargin];
    }
    
    // m_AIController.DoEscape(nDistance, 100000);
    m_AIVM.FireEvent(aevOnInFear, m_dwID, 0);

    m_nMoveFrameCounter = RESIST_FEAR_INTERVAL;
#endif

    bResult = true;
Exit0:
    return bResult;
}

#ifdef _SERVER
BOOL KCharacter::ProcessRevive(int nLife, int nMana)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    KPlayer*    pPlayer     = NULL;
    KCharacter* pSkillSrc   = NULL;
    
    KG_PROCESS_ERROR(IS_PLAYER(m_dwID));
    KG_PROCESS_ERROR(m_eMoveState == cmsOnDeath);

    pPlayer = (KPlayer*)this;

    pSkillSrc = m_pSkillAttributeParam->pSkillSrc;
    KGLOG_PROCESS_ERROR(pSkillSrc);

    pPlayer->m_ReviveCtrl.dwReviver     = pSkillSrc->m_dwID;
    pPlayer->m_ReviveCtrl.nReviveLife   = nLife;
    pPlayer->m_ReviveCtrl.nReviveMana   = nMana;

    bRetCode = pSkillSrc->GetAbsoluteCoordinate(
        &pPlayer->m_ReviveCtrl.nX, &pPlayer->m_ReviveCtrl.nY, &pPlayer->m_ReviveCtrl.nZ
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    g_PlayerServer.DoSyncPlayerReviveCtrl(pPlayer);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KCharacter::ProcessTherapy(int nTherapyValue)
{
    int         nDeltaLife          = 0;
    KCharacter* pCaster             = NULL;
    int         nDenominatorSrc     = 0;
    int         nCriDamagePower     = 0;
    int         nCriticalStrikeCof  = 0;

    KGLOG_PROCESS_ERROR(m_pSkillAttributeParam);
    
    pCaster = m_pSkillAttributeParam->pSkillSrc;

    if (pCaster)
    {
        nDeltaLife = 
            m_pSkillAttributeParam->nTherapy + (pCaster->m_nSkillTherapy + nTherapyValue) * 
            (KILO_NUM + pCaster->m_nSkillTherapyPercent) / KILO_NUM;

        nDeltaLife = nDeltaLife * (KILO_NUM + pCaster->m_nTherapyCoefficient) / KILO_NUM;
    }
    else
    {   
        nDeltaLife = m_pSkillAttributeParam->nTherapy + nTherapyValue;

        nDeltaLife = nDeltaLife * (KILO_NUM + m_nTherapyCoefficient) / KILO_NUM;
    }

    nDeltaLife = nDeltaLife * (KILO_NUM + m_pSkillAttributeParam->nDamageAddPercent) / KILO_NUM;    // 技能秘笈加成

    if (m_pSkillAttributeParam->nStackNum > 1)
    {
        nDeltaLife = nDeltaLife * m_pSkillAttributeParam->nStackNum;
    }

    if (m_pSkillAttributeParam->bCriticalStrike)
    {
        if (pCaster)
        {
            nCriDamagePower 	= m_pSkillAttributeParam->nCriDamagePower;
            nCriticalStrikeCof 	= max(nCriDamagePower, 0);
        }
        else
        {
            nCriticalStrikeCof 	= 0;
        }
        
        nDeltaLife = nDeltaLife * 3 / 2 + nDeltaLife * nCriticalStrikeCof / KILO_NUM;
    }

    if (nDeltaLife > 0 && m_dwTransferTherapyToCharacter)
    {
        KCharacter* pCharacter = NULL;

        if (IS_PLAYER(m_dwTransferTherapyToCharacter))
        {
            pCharacter = g_pSO3World->m_PlayerSet.GetObj(m_dwTransferTherapyToCharacter);
        }
        else
        {
            pCharacter = g_pSO3World->m_NpcSet.GetObj(m_dwTransferTherapyToCharacter);
        }

        if (pCharacter && pCharacter->m_pScene && pCharacter->m_eMoveState != cmsOnDeath)
        {
            int nTransferTherapy = nDeltaLife * m_nTransferTherapyPercent / KILO_NUM;

            nTransferTherapy = max(0, nTransferTherapy);

            pCharacter->m_nCurrentLife += nTransferTherapy;

            if (nTransferTherapy > 0)
            {
                g_PlayerServer.DoCommonHealth(pCharacter, nTransferTherapy);
            }
        }
    }

    if (nDeltaLife < 0)
    {
        nDeltaLife = 0;
    }
    
    m_pDstSkillCalculateResult->nSkillEffectResult[serTherapy] += nDeltaLife;

Exit0:
    return true;
}

BOOL KCharacter::ProcessDamage(int nDamageType, int nDamageValue)
{
    BOOL                        bResult                 = false;
    BOOL                        bRetCode                = false;
    KSkill*                     pSkill                  = NULL;
    KCharacter*                 pSkillSrc               = NULL;
    KDAMAGE_CALCULATE_PARAM     DamageCalculateParam;
    int                         nPhysicsShield          = 0;
    int                         nSolarShield            = 0;
    int                         nNeutralShield          = 0;
    int                         nLunarShield            = 0;
    int                         nPoisonShield           = 0;
    int                         nSkillSrcLevel          = 0;
    int                         nDenominator            = 0;

    KGLOG_PROCESS_ERROR(m_pSkillAttributeParam);

    pSkill = m_pSkillAttributeParam->SkillRecipePointer.GetPointer();
    pSkillSrc = m_pSkillAttributeParam->pSkillSrc;

    nSkillSrcLevel = m_pSkillAttributeParam->nSkillSrcLevel;
    if (nSkillSrcLevel == 0)    // 和策划约定，取不到Src等级，就按自己的等级算。
        nSkillSrcLevel = m_nLevel;

    nSkillSrcLevel = max(nSkillSrcLevel, 1);    // 保证不会除0

    KGLOG_PROCESS_ERROR(m_pSrcSkillCalculateResult);
    KGLOG_PROCESS_ERROR(m_pDstSkillCalculateResult);

    if (pSkillSrc)
    {
        nPhysicsShield  = max((m_nPhysicsShield - pSkillSrc->m_nPhysicsOvercome), 0);
        nSolarShield    = max((m_nSolarMagicShield - pSkillSrc->m_nSolarOvercome), 0);
        nNeutralShield  = max((m_nNeutralMagicShield - pSkillSrc->m_nNeutralOvercome), 0);
        nLunarShield    = max((m_nLunarMagicShield - pSkillSrc->m_nLunarOvercome), 0);
        nPoisonShield   = max((m_nPoisonMagicShield - pSkillSrc->m_nPoisonOvercome), 0);
    }
    else
    {
        nPhysicsShield  = m_nPhysicsShield;
        nSolarShield    = m_nSolarMagicShield;
        nNeutralShield  = m_nNeutralMagicShield;
        nLunarShield    = m_nLunarMagicShield;
        nPoisonShield   = m_nPoisonMagicShield;
    }

    switch (nDamageType)
    {
    case sktPhysics:
        if (m_nGlobalBlock || m_nPhysicsBlock)
        {
            m_pDstSkillCalculateResult->bPhysicsBlock = true;

            if (pSkill && pSkillSrc)
            {
                if (m_pSkillAttributeParam->pBullet)
                {
                    KSKILL_BULLET* pBullet = m_pSkillAttributeParam->pBullet;
                    pSkillSrc->OnSkillEvent(pSkill, seBeBlock, pBullet->SkillEventVector, pSkillSrc, this);
                }
                else
                {
                    pSkillSrc->OnSkillEvent(pSkill, seBeBlock, pSkillSrc->m_SkillEventVector, pSkillSrc, this);
                }

                OnSkillEvent(pSkill, seBlock, m_SkillEventVector, this, pSkillSrc);
            }

            goto Exit1;
        }
        
        // -------------- physics ---------------------------------------       
        {
            DamageCalculateParam.nDamageValue                  = max(0, m_pSkillAttributeParam->nPhysicsDamage + nDamageValue);
            if (m_pSkillAttributeParam->bParray) // 被招架
            {
                int nParryDamage = m_nParryValue + m_nCurrentStrength / 4;

                m_pDstSkillCalculateResult->nSkillEffectResult[serParryDamage] = min(nParryDamage, DamageCalculateParam.nDamageValue);

                DamageCalculateParam.nDamageValue = max((DamageCalculateParam.nDamageValue - nParryDamage), 0);
            }

            DamageCalculateParam.nDstResistPercent             = m_nPhysicsResistPercent;
            // 以下几个数值是由策划的计算公式推导的
            nDenominator = m_nLevel * 270 + nPhysicsShield;
            KGLOG_PROCESS_ERROR(nDenominator > 0);
            DamageCalculateParam.nDstDefenceCoefficient        = min(
                KILO_NUM * nPhysicsShield / nDenominator + m_nPhysicsDefenceAdd, m_nPhysicsDefenceMax
            );

            DamageCalculateParam.pnDstMaxTypedDamageAbsorb     = &m_nPhysicsDamageAbsorb;
            DamageCalculateParam.pnDstMaxTypedDamageManaShield = &m_nPhysicsDamageManaShield;
            DamageCalculateParam.nDstPointReflection           = m_nPhysicsReflection;
            DamageCalculateParam.nDstPercentReflection         = m_nPhysicsReflectionPercent;
            DamageCalculateParam.pnDstTypedDamage              = &m_pDstSkillCalculateResult->nSkillEffectResult[serPhysicsDamage];
            DamageCalculateParam.nDstTypedDamageCoefficient    = m_nPhysicsDamageCoefficient;

            m_SkillDebug.Trace(
                2, "[技能调试][%d]:[外功伤害计算] 外功伤害:%d, 外功伤害减免:%d 防御系数:%d 伤害吸收:%d/%d 魔法盾:%d/%d 反弹:%d/%d 外功伤害加成:%d To(%s-%d)",
                g_pSO3World->m_nGameLoop, DamageCalculateParam.nDamageValue,
                DamageCalculateParam.nDstResistPercent, DamageCalculateParam.nDstDefenceCoefficient,
                m_nPhysicsDamageAbsorb, m_nGlobalDamageAbsorb, m_nPhysicsDamageManaShield, m_nGlobalDamageManaShield,
                m_nPhysicsReflection, m_nPhysicsReflectionPercent, m_nPhysicsDamageCoefficient, 
                m_szName, m_dwID
            );
        }

        break;

    case sktSolarMagic:
        if (m_nGlobalBlock || m_nSolarMagicBlock)
        {
            m_pDstSkillCalculateResult->bSolarMagicBlock = true;
            
            if (pSkill && pSkillSrc)
            {
                if (m_pSkillAttributeParam->pBullet)
                {
                    KSKILL_BULLET* pBullet = m_pSkillAttributeParam->pBullet;
                    pSkillSrc->OnSkillEvent(pSkill, seBeBlock, pBullet->SkillEventVector, pSkillSrc, this);
                }
                else
                {
                    pSkillSrc->OnSkillEvent(pSkill, seBeBlock, pSkillSrc->m_SkillEventVector, pSkillSrc, this);
                }
                
                OnSkillEvent(pSkill, seBlock, m_SkillEventVector, this, pSkillSrc);
            }

            goto Exit1;
        }
       
        {
            // 伤害抵抗
            int nRow                = 0;
            int nProbability        = 0;
            int nSolarMagicDefence  = 0;

            nDenominator = nSolarShield + 9 * nSkillSrcLevel;
            KGLOG_PROCESS_ERROR(nDenominator > 0);
            nRow = (200 * KILO_NUM * nSolarShield / nDenominator + 410) / KILO_NUM;

            nProbability = (int)g_Random(TEN_THOUSAND_NUM);
            nSolarMagicDefence = g_pSO3World->m_SkillManager.GetDefenceCoefficient(nRow, nProbability);

            KGLOG_PROCESS_ERROR(nSolarMagicDefence != -1);
            DamageCalculateParam.nDstDefenceCoefficient        = nSolarMagicDefence;

            // -------------- Solar ---------------------------------------
            DamageCalculateParam.nDamageValue                  = m_pSkillAttributeParam->nSolarDamage + nDamageValue;
            DamageCalculateParam.nDstResistPercent             = m_nSolarMagicResistPercent;

            DamageCalculateParam.pnDstMaxTypedDamageAbsorb     = &m_nSolarDamageAbsorb;        
            DamageCalculateParam.pnDstMaxTypedDamageManaShield = &m_nSolarDamageManaShield;
            DamageCalculateParam.nDstPointReflection           = m_nSolarMagicReflection;
            DamageCalculateParam.nDstPercentReflection         = m_nSolarMagicReflectionPercent;
            DamageCalculateParam.pnDstTypedDamage              = &m_pDstSkillCalculateResult->nSkillEffectResult[serSolarMagicDamage];
            DamageCalculateParam.nDstTypedDamageCoefficient    = m_nSolarDamageCoefficient;

            m_SkillDebug.Trace(
                2, "[技能调试][%d]:[阳性内功伤害结果] 阳性内功伤害:%d, 阳性内功伤害减免:%d 防御系数:%d 伤害吸收:%d/%d 魔法盾:%d/%d 反弹:%d/%d 阳性内功伤害加成:%d To(%s-%u)",
                g_pSO3World->m_nGameLoop, DamageCalculateParam.nDamageValue,
                DamageCalculateParam.nDstResistPercent, DamageCalculateParam.nDstDefenceCoefficient,
                m_nSolarDamageAbsorb, m_nGlobalDamageAbsorb, m_nSolarDamageManaShield, m_nGlobalDamageManaShield,
                m_nSolarMagicReflection, m_nSolarMagicReflectionPercent, m_nSolarDamageCoefficient, 
                m_szName, m_dwID
            );
        }
        break;

    case sktNeutralMagic:
        if (m_nGlobalBlock || m_nNeutralMagicBlock)
        {
            m_pDstSkillCalculateResult->bNeutralMagicBlock = true;

            if (pSkillSrc && pSkill)
            {
                if (m_pSkillAttributeParam->pBullet)
                {
                    KSKILL_BULLET* pBullet = m_pSkillAttributeParam->pBullet;
                    pSkillSrc->OnSkillEvent(pSkill, seBeBlock, pBullet->SkillEventVector, pSkillSrc, this);
                }
                else
                {
                    pSkillSrc->OnSkillEvent(pSkill, seBeBlock, pSkillSrc->m_SkillEventVector, pSkillSrc, this);
                }

                OnSkillEvent(pSkill, seBlock, m_SkillEventVector, this, pSkillSrc);
            }
            
            goto Exit1;   
        }

        {
            // 伤害抵抗
            int nRow                    = 0;
            int nProbability            = 0;
            int nNeutralMagicDefence    = 0;

            nDenominator = nNeutralShield + 9 * nSkillSrcLevel;
            KGLOG_PROCESS_ERROR(nDenominator > 0);
            nRow = (200 * KILO_NUM * nNeutralShield / nDenominator + 410) / KILO_NUM;

            nProbability = (int)g_Random(TEN_THOUSAND_NUM);
            nNeutralMagicDefence = g_pSO3World->m_SkillManager.GetDefenceCoefficient(nRow, nProbability);

            KGLOG_PROCESS_ERROR(nNeutralMagicDefence != -1);
            DamageCalculateParam.nDstDefenceCoefficient        = nNeutralMagicDefence;        
        
            // -------------- Neutral ---------------------------------------
            DamageCalculateParam.nDamageValue                  = m_pSkillAttributeParam->nNeutralDamage + nDamageValue;
            DamageCalculateParam.nDstResistPercent             = m_nNeutralMagicResistPercent;
            DamageCalculateParam.pnDstMaxTypedDamageAbsorb     = &m_nNeutralDamageAbsorb;        
            DamageCalculateParam.pnDstMaxTypedDamageManaShield = &m_nNeutralDamageManaShield;
            DamageCalculateParam.nDstPointReflection           = m_nNeutralMagicReflection;
            DamageCalculateParam.nDstPercentReflection         = m_nNeutralMagicReflectionPercent;
            DamageCalculateParam.pnDstTypedDamage              = &m_pDstSkillCalculateResult->nSkillEffectResult[serNeutralMagicDamage];
            DamageCalculateParam.nDstTypedDamageCoefficient    = m_nNeutralDamageCoefficient;

            m_SkillDebug.Trace(
                2, "[技能调试][%d]:[中性内功伤害结果] 中性内功伤害:%d, 中性内功伤害减免:%d 防御系数:%d 伤害吸收:%d/%d 魔法盾:%d/%d 反弹:%d/%d 中性内功伤害加成:%d To(%s-%u)",
                g_pSO3World->m_nGameLoop, DamageCalculateParam.nDamageValue,
                DamageCalculateParam.nDstResistPercent, DamageCalculateParam.nDstDefenceCoefficient,
                m_nNeutralDamageAbsorb, m_nGlobalDamageAbsorb, m_nNeutralDamageManaShield, m_nGlobalDamageManaShield,
                m_nNeutralMagicReflection, m_nNeutralMagicReflectionPercent, m_nNeutralDamageCoefficient, 
                m_szName, m_dwID
            );
        }
        break;

    case sktLunarMagic:
        if (m_nGlobalBlock || m_nLunarMagicBlock)
        {
            m_pDstSkillCalculateResult->bLunarMagicBlock = true;

            if (pSkillSrc && pSkill)
            {
                if (m_pSkillAttributeParam->pBullet)
                {
                    KSKILL_BULLET* pBullet = m_pSkillAttributeParam->pBullet;
                    pSkillSrc->OnSkillEvent(pSkill, seBeBlock, pBullet->SkillEventVector, pSkillSrc, this);
                }
                else
                {
                    pSkillSrc->OnSkillEvent(pSkill, seBeBlock, pSkillSrc->m_SkillEventVector, pSkillSrc, this);
                }

                OnSkillEvent(pSkill, seBlock, m_SkillEventVector, this, pSkillSrc);
            }

           goto Exit1;
        }
        {
            // 伤害抵抗
            int nRow                = 0;
            int nProbability        = 0;
            int nLunarMagicDefence  = 0;

            nDenominator = nLunarShield + 9 * nSkillSrcLevel;
            KGLOG_PROCESS_ERROR(nDenominator > 0);
            nRow = (200 * KILO_NUM * nLunarShield / nDenominator + 410) / KILO_NUM;

            nProbability = (int)g_Random(TEN_THOUSAND_NUM);
            nLunarMagicDefence = g_pSO3World->m_SkillManager.GetDefenceCoefficient(nRow, nProbability);

            KGLOG_PROCESS_ERROR(nLunarMagicDefence != -1);
            DamageCalculateParam.nDstDefenceCoefficient        = nLunarMagicDefence;
        
            // -------------- Lunar ---------------------------------------
            DamageCalculateParam.nDamageValue                  = m_pSkillAttributeParam->nLunarDamage + nDamageValue;
            DamageCalculateParam.nDstResistPercent             = m_nLunarMagicResistPercent;
            DamageCalculateParam.pnDstMaxTypedDamageAbsorb     = &m_nLunarDamageAbsorb;        
            DamageCalculateParam.pnDstMaxTypedDamageManaShield = &m_nLunarDamageManaShield;
            DamageCalculateParam.nDstPointReflection           = m_nLunarMagicReflection;
            DamageCalculateParam.nDstPercentReflection         = m_nLunarMagicReflectionPercent;
            DamageCalculateParam.pnDstTypedDamage              = &m_pDstSkillCalculateResult->nSkillEffectResult[serLunarMagicDamage];
            DamageCalculateParam.nDstTypedDamageCoefficient    = m_nLunarDamageCoefficient;

            m_SkillDebug.Trace(
                2, "[技能调试][%d]:[阴性内功伤害结果] 阴性内功伤害:%d, 阴性内功伤害减免:%d 防御系数:%d 伤害吸收:%d/%d 魔法盾:%d/%d 反弹:%d/%d 阴性内功伤害加成:%d To(%s-%u)",
                g_pSO3World->m_nGameLoop, DamageCalculateParam.nDamageValue,
                DamageCalculateParam.nDstResistPercent, DamageCalculateParam.nDstDefenceCoefficient,
                m_nLunarDamageAbsorb, m_nGlobalDamageAbsorb, m_nLunarDamageManaShield, m_nGlobalDamageManaShield,
                m_nLunarMagicReflection, m_nLunarMagicReflectionPercent, m_nLunarDamageCoefficient, 
                m_szName, m_dwID
            );
        }
        break;

    case sktPoison:
        if (m_nGlobalBlock || m_nPoisonMagicBlock)
        {
            m_pDstSkillCalculateResult->bPhysicsBlock = true;

            if (pSkillSrc && pSkill)
            {
                if (m_pSkillAttributeParam->pBullet)
                {
                    KSKILL_BULLET* pBullet = m_pSkillAttributeParam->pBullet;
                    pSkillSrc->OnSkillEvent(pSkill, seBeBlock, pBullet->SkillEventVector, pSkillSrc, this);
                }
                else
                {
                    pSkillSrc->OnSkillEvent(pSkill, seBeBlock, pSkillSrc->m_SkillEventVector, pSkillSrc, this);
                }

                OnSkillEvent(pSkill, seBlock, m_SkillEventVector, this, pSkillSrc);
            }

            goto Exit1;    
        }
        {
            // 伤害抵抗
            int nRow                = 0;
            int nProbability        = 0;
            int nPoisonMagicDefence = 0;

            nDenominator = nPoisonShield + 9 * nSkillSrcLevel;
            KGLOG_PROCESS_ERROR(nDenominator > 0);
            nRow = (200 * KILO_NUM * nPoisonShield / nDenominator + 410) / KILO_NUM;

            nProbability = (int)g_Random(TEN_THOUSAND_NUM);
            nPoisonMagicDefence = g_pSO3World->m_SkillManager.GetDefenceCoefficient(nRow, nProbability);

            KGLOG_PROCESS_ERROR(nPoisonMagicDefence != -1);
            DamageCalculateParam.nDstDefenceCoefficient        = nPoisonMagicDefence;
        
            // -------------- Poison ---------------------------------------
            DamageCalculateParam.nDamageValue                  = m_pSkillAttributeParam->nPoisonDamage + nDamageValue;
            DamageCalculateParam.nDstResistPercent             = m_nPoisonMagicResistPercent;
            DamageCalculateParam.pnDstMaxTypedDamageAbsorb     = &m_nPoisonDamageAbsorb;        
            DamageCalculateParam.pnDstMaxTypedDamageManaShield = &m_nPoisonDamageManaShield;
            DamageCalculateParam.nDstPointReflection           = m_nPoisonMagicReflection;
            DamageCalculateParam.nDstPercentReflection         = m_nPoisonMagicReflectionPercent;
            DamageCalculateParam.pnDstTypedDamage              = &m_pDstSkillCalculateResult->nSkillEffectResult[serPoisonDamage];
            DamageCalculateParam.nDstTypedDamageCoefficient    = m_nPoisonDamageCoefficient;

            m_SkillDebug.Trace(
                2, "[技能调试][%d]:[毒伤害结果] 毒伤害:%d, 毒伤害减免:%d 防御系数:%d 伤害吸收:%d/%d 魔法盾:%d/%d 反弹:%d/%d 毒伤害加成:%d To(%s-%u)",
                g_pSO3World->m_nGameLoop, DamageCalculateParam.nDamageValue,
                DamageCalculateParam.nDstResistPercent, DamageCalculateParam.nDstDefenceCoefficient,
                m_nPoisonDamageAbsorb, m_nGlobalDamageAbsorb, m_nPoisonDamageManaShield, m_nGlobalDamageManaShield,
                m_nPoisonMagicReflection, m_nPoisonMagicReflectionPercent, m_nPoisonDamageCoefficient, 
                m_szName, m_dwID
            );
        }
        break;
    default:
        goto Exit1;
    }

    if (m_pSkillAttributeParam->bInsight) // 识破针对所有伤害
    {
        int nInsightDamageValue = DamageCalculateParam.nDamageValue * 256 / KILO_NUM;

        m_pDstSkillCalculateResult->nSkillEffectResult[serInsightDamage] = DamageCalculateParam.nDamageValue - nInsightDamageValue;
        DamageCalculateParam.nDamageValue = nInsightDamageValue;
    }

    DamageCalculateParam.pSkillSrc                  = pSkillSrc;
    DamageCalculateParam.nDamageToLifeForSrc        = m_pSkillAttributeParam->nDamgeToLifeForSelf;
    DamageCalculateParam.nDamageToManaForSrc        = m_pSkillAttributeParam->nDamgeToManaForSelf;
    DamageCalculateParam.bCriticalStrikeFlag        = m_pSkillAttributeParam->bCriticalStrike;

    if (pSkill)
    {
        DamageCalculateParam.nDamageToLifeForSrcTeam   = pSkill->m_nDamageToLifeForParty;
        DamageCalculateParam.nDamageToManaForSrcTeam   = pSkill->m_nDamageToManaForParty;
    }

    if (m_pSkillAttributeParam->bDot)
    {
        DamageCalculateParam.bCriticalStrikeFlag    = false;
        DamageCalculateParam.nDstPointReflection    = 0;
        DamageCalculateParam.nDstPercentReflection  = 0;
    }

    MAKE_IN_RANGE(DamageCalculateParam.nDstResistPercent, 0, KILO_NUM);
    MAKE_IN_RANGE(DamageCalculateParam.nDstDefenceCoefficient, 0, KILO_NUM);

    DamageCalculateParam.nDamageValue = DamageCalculateParam.nDamageValue * (KILO_NUM + m_pSkillAttributeParam->nDamageAddPercent) / KILO_NUM;    // 技能秘笈加成

    if (m_pSkillAttributeParam->nStackNum > 1)
    {
        DamageCalculateParam.nDamageValue = DamageCalculateParam.nDamageValue * m_pSkillAttributeParam->nStackNum;
    }

    if (pSkillSrc && IS_PLAYER(pSkillSrc->m_dwID)) // 如果攻击方是Player则计算阵营对攻击的提升
    {
        KPlayer* pSrcPlayer         = (KPlayer*)pSkillSrc;
        int      nCampLevel         = g_pSO3World->m_CampInfo.GetCampLevel();
        int      nCampDamagePercent = 0;

        nCampDamagePercent = g_pSO3World->m_CampInfo.GetDamagePercent(pSrcPlayer->m_eCamp, nCampLevel);

        DamageCalculateParam.nDamageValue = DamageCalculateParam.nDamageValue * nCampDamagePercent / KILO_NUM;
    }

    if (IS_PLAYER(m_dwID)) // 如果被击方是Player则计算阵营对伤害减免的提升
    {
        KPlayer* pDstPlayer         = (KPlayer*)this;
        int      nCampLevel         = g_pSO3World->m_CampInfo.GetCampLevel();
        int      nCampShieldPercent = 0;

        nCampShieldPercent = g_pSO3World->m_CampInfo.GetShieldPercent(pDstPlayer->m_eCamp, nCampLevel);

        DamageCalculateParam.nDamageValue = DamageCalculateParam.nDamageValue * nCampShieldPercent / KILO_NUM;
    }

    bRetCode = CalculateDamage(DamageCalculateParam);
    KGLOG_PROCESS_ERROR(bRetCode);   

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KCharacter::CalculateDamage(const KDAMAGE_CALCULATE_PARAM &crParam)
{
    BOOL            bResult                   = false;
    int             nRetCode                  = 0;
    KCharacter*     pSkillSrc                 = crParam.pSkillSrc;
    KTeam*          pTeam                     = NULL;
    int             nDamageToDst              = 0;
    int             nReflectiedDamageToSrc    = 0;
    int             nGlobalDamageAbsorb       = 0;
    int             nTypedDamageAbsorb        = 0;
    int             nGlobalManaShieldDamage   = 0;
    int             nTypedManaShieldDamage    = 0;
    int             nReflectiedDamage         = 0;
    int             nDamageToLifeForSrc       = 0;
    int             nDamageToManaForSrc       = 0;
    int             nDamageToLifeForSrcTeam  = 0;
    int             nDamageToManaForSrcTeam  = 0;
    int             nCriDamagePower           = 0;
    int             nDecirDamagePower         = 0;
    int             nCriticalStrikeCof        = 0;
    int             nTransferDamageToMana     = 0;
    int             nDenominatorSrc           = 0;
    int             nDenominatorDst           = 0;
    DWORD           dwDstMoveStateMask        = 0;

    KGLOG_PROCESS_ERROR(m_pSkillAttributeParam);

    nDamageToDst = crParam.nDamageValue;

    if (pSkillSrc)
    {
        dwDstMoveStateMask = (1 << m_eMoveState);
        if (dwDstMoveStateMask & m_pSkillAttributeParam->dwAddDamageByDstMoveStateMask)
        {
            nDamageToDst = nDamageToDst * (KILO_NUM + m_pSkillAttributeParam->nAddDamagePercentByDstMoveState) / KILO_NUM; // 目标移动状态对伤害的加成
        }
    }

    nDamageToDst = nDamageToDst * (KILO_NUM - crParam.nDstResistPercent) / KILO_NUM;

    nDamageToDst = nDamageToDst * (KILO_NUM - crParam.nDstDefenceCoefficient) / KILO_NUM;

    if (crParam.bCriticalStrikeFlag)
    {
        if (pSkillSrc)
        {
            nDenominatorDst = 74 * m_nLevel + 320;
            KGLOG_PROCESS_ERROR(nDenominatorDst > 0);

            nCriDamagePower = m_pSkillAttributeParam->nCriDamagePower;

            nDecirDamagePower = m_nDecriticalDamagePowerBaseKiloNumRate + KILO_NUM * m_nDecriticalDamagePower / nDenominatorDst;

            nCriticalStrikeCof = max((nCriDamagePower - nDecirDamagePower), (-KILO_NUM / 2));
        }

        nDamageToDst = nDamageToDst * 3 / 2 + nDamageToDst * nCriticalStrikeCof / KILO_NUM;
    }

    // 使技能至少能造成1点伤害效果
    nDamageToDst = max(nDamageToDst, 1);

    // 无类型的伤害吸收
    nGlobalDamageAbsorb = min(m_nGlobalDamageAbsorb, nDamageToDst);
    if (nGlobalDamageAbsorb < 0)
    {
        nGlobalDamageAbsorb = 0;
    }

    nDamageToDst -= nGlobalDamageAbsorb;
    m_nGlobalDamageAbsorb -= nGlobalDamageAbsorb;
    
    // 有类型的伤害吸收
    nTypedDamageAbsorb = min(*crParam.pnDstMaxTypedDamageAbsorb, nDamageToDst);
    if (nTypedDamageAbsorb < 0)
    {
        nTypedDamageAbsorb = 0;
    }

    nDamageToDst -= nTypedDamageAbsorb;

    *crParam.pnDstMaxTypedDamageAbsorb -= nTypedDamageAbsorb;
   
    m_pDstSkillCalculateResult->nSkillEffectResult[serAbsorbDamage] += nGlobalDamageAbsorb + nTypedDamageAbsorb;

    // 无类型的内力抵伤害
    nGlobalManaShieldDamage = min(m_nGlobalDamageManaShield, nDamageToDst);
    nGlobalManaShieldDamage = min(nGlobalManaShieldDamage, m_nCurrentMana / DAMAGE_2_MANA_RATE);
    if (nGlobalManaShieldDamage < 0)
    {
        nGlobalManaShieldDamage = 0;
    }

    nDamageToDst -= nGlobalManaShieldDamage;
    m_nGlobalDamageManaShield -= nGlobalManaShieldDamage;
    m_nCurrentMana -= nGlobalManaShieldDamage * DAMAGE_2_MANA_RATE;

    // 有类型的内力抵伤害
    nTypedManaShieldDamage = min(*crParam.pnDstMaxTypedDamageManaShield, nDamageToDst);
    nTypedManaShieldDamage = min(nTypedManaShieldDamage, m_nCurrentMana / DAMAGE_2_MANA_RATE);
    if (nTypedManaShieldDamage < 0)
    {
        nTypedManaShieldDamage = 0;
    }

    nDamageToDst -= nTypedManaShieldDamage;
    *crParam.pnDstMaxTypedDamageManaShield -= nTypedManaShieldDamage;
    m_nCurrentMana -= nTypedManaShieldDamage * DAMAGE_2_MANA_RATE;
    
    m_pDstSkillCalculateResult->nSkillEffectResult[serShieldDamage] += nGlobalManaShieldDamage + nTypedManaShieldDamage;

    // 伤害反弹: 百分比反弹可以减少自己受到的伤害并施加给技能施放者，点数反弹只是把伤害施加给技能施放者，并不减少自己受到的伤害
    nReflectiedDamage = nDamageToDst * crParam.nDstPercentReflection / KILO_NUM;

    nReflectiedDamageToSrc += nReflectiedDamage + crParam.nDstPointReflection;
    if (nReflectiedDamageToSrc < 0)
    {
        nReflectiedDamageToSrc = 0;
    }

    m_pSrcSkillCalculateResult->nSkillEffectResult[serReflectiedDamage] += nReflectiedDamageToSrc;

    // 伤害转内力
    nTransferDamageToMana = nDamageToDst * m_nTransferDamageToManaPercent / KILO_NUM;
    if (nTransferDamageToMana < 0)
    {
        nTransferDamageToMana = 0;
    }
    m_nCurrentMana += nTransferDamageToMana;

    // 攻击者偷取生命(将伤害的一部分转化为自己的生命,并不增加目标受到的伤害)
    nDamageToLifeForSrc += nDamageToDst * crParam.nDamageToLifeForSrc / KILO_NUM;    
    if (nDamageToLifeForSrc < 0)
    {
        nDamageToLifeForSrc = 0;
    }

    m_pSrcSkillCalculateResult->nSkillEffectResult[serStealLife] += nDamageToLifeForSrc;

    // 攻击者偷取内力(将伤害的一部分转化为自己的内力,并不减少目标的内力)
    nDamageToManaForSrc += nDamageToDst * crParam.nDamageToManaForSrc / KILO_NUM;
    
    if (pSkillSrc)
    {
        pSkillSrc->m_nCurrentMana += nDamageToManaForSrc;
    }

    // 作用于小队伤害转化,只有Player才有队伍
    if (pSkillSrc && IS_PLAYER(pSkillSrc->m_dwID))
    {
        KPlayer* pPlayer        = (KPlayer*)pSkillSrc;
        DWORD    dwTeamID       = pPlayer->GetCurrentTeamID();
        // DamageToLife
        nDamageToLifeForSrcTeam += nDamageToDst * crParam.nDamageToLifeForSrcTeam / KILO_NUM;

        if (nDamageToLifeForSrcTeam < 0)
        {
            nDamageToLifeForSrcTeam = 0;
        }

        // DamageToMana
        nDamageToManaForSrcTeam += nDamageToDst * crParam.nDamageToManaForSrcTeam / KILO_NUM;

        if (nDamageToManaForSrcTeam < 0)
        {
            nDamageToManaForSrcTeam = 0;
        }

        // 作用到小队
        pTeam = g_pSO3World->m_TeamServer.GetTeam(dwTeamID);
        
        if (pTeam)
        {
            int nGroupIndex = pPlayer->GetCurrentTeamGroupIndex();
            KGLOG_PROCESS_ERROR(nGroupIndex >= 0 && nGroupIndex < pTeam->nGroupNum);

            for (
                KTEAM_MEMBER_LIST::iterator it = pTeam->MemberGroup[nGroupIndex].MemberList.begin();
                it != pTeam->MemberGroup[nGroupIndex].MemberList.end(); ++it
            )
            {
                KPlayer* pMemberPlayer = NULL;
                pMemberPlayer = g_pSO3World->m_PlayerSet.GetObj(it->dwMemberID);

                if (!pMemberPlayer) // 找不到队友，不影响其他队友的操作
                    continue;

                nRetCode = g_GetDistance3(
                    pMemberPlayer->m_nX, pMemberPlayer->m_nY, ZPOINT_TO_XYPOINT(pMemberPlayer->m_nZ),
                    m_nX, m_nY, ZPOINT_TO_XYPOINT(m_nZ)
                );

                if (nRetCode > MAX_PARTY_EFFECT_RANGE * MAX_PARTY_EFFECT_RANGE)
                    continue;

                pMemberPlayer->m_nCurrentLife += nDamageToLifeForSrcTeam;
                pMemberPlayer->m_nCurrentMana += nDamageToManaForSrcTeam;
            }
        }
        else // 没有队伍，只给自己加血
        {
            pSkillSrc->m_nCurrentLife += nDamageToLifeForSrcTeam;
            pSkillSrc->m_nCurrentMana += nDamageToManaForSrcTeam;
        }
    }

    if (nDamageToDst > 0 && m_dwTransferDamageToCharacter)
    {
        KCharacter* pCharacter = NULL;

        if (IS_PLAYER(m_dwTransferDamageToCharacter))
        {
            pCharacter = g_pSO3World->m_PlayerSet.GetObj(m_dwTransferDamageToCharacter);
        }
        else
        {
            pCharacter = g_pSO3World->m_NpcSet.GetObj(m_dwTransferDamageToCharacter);
        }

        if (pCharacter && pCharacter->m_pScene && pCharacter->m_eMoveState != cmsOnDeath)
        {
            int     nTransferDamage = nDamageToDst * m_nTransferDamagePercent / KILO_NUM;
            DWORD   dwKiller        = ERROR_ID;

            nTransferDamage = max(0, nTransferDamage);

            nDamageToDst = nDamageToDst - nTransferDamage;

            pCharacter->m_nCurrentLife -= nTransferDamage;

            if (nTransferDamage > 0)
            {
                g_PlayerServer.DoCommonHealth(pCharacter, -nTransferDamage);
            }

            if (pSkillSrc)
            {
                dwKiller = pSkillSrc->m_dwID;
            }

            if (pCharacter->m_nCurrentLife <= 0)
            {
                pCharacter->GoToHell(dwKiller);
            }
        }
    }

    if (nDamageToDst < 0)
    {
        nDamageToDst = 0;
    }

    nDamageToDst = nDamageToDst * (KILO_NUM + crParam.nDstTypedDamageCoefficient) / KILO_NUM;

    *(crParam.pnDstTypedDamage) += nDamageToDst;

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

void KCharacter::OnSkillEvent(
    KSkill* pSkill, int nEventType, KUSER_SKILL_EVENT_VECTOR SkillEvent, 
    KCharacter* pCaster, KCharacter* pTarget
)
{
    int             nRetCode            = false;
    KSKILL_EVENT*   pEventInfo          = NULL;
    KTarget         Target;
    KCharacter*     pSkillCaster        = NULL;
    KCharacter*     pSkillTarget        = NULL;
    DWORD           dwSkillEventMask1   = 0xFFFFFFFF;
    DWORD           dwSkillEventMask2   = 0xFFFFFFFF;
    KUSER_SKILL_EVENT_VECTOR::iterator it;
    KUSER_SKILL_EVENT_VECTOR::iterator itEnd;

    if (pSkill)
    {
        dwSkillEventMask1 = pSkill->m_pBaseInfo->dwSkillEventMask1;
        dwSkillEventMask2 = pSkill->m_pBaseInfo->dwSkillEventMask2;
    }

    itEnd = SkillEvent.end();
    for (it = SkillEvent.begin(); it != itEnd; ++it)
    {
        int nOdds = 0;

        pEventInfo = &(it->SkillEvent);
        if (pEventInfo->nEventType != nEventType)
            continue;

        if (!(dwSkillEventMask1 & pEventInfo->dwEventMask1) && !(dwSkillEventMask2 & pEventInfo->dwEventMask2))
            continue;

        nOdds = (int)g_Random(KILO_NUM);
        if (pEventInfo->nOdds + nOdds < KILO_NUM)
            continue;

        switch(pEventInfo->nCasterType)
        {
        case escttCaster:
            pSkillCaster = pCaster;
            break;
        case escttTarget:
            pSkillCaster = pTarget;
            break;
        default:
            assert(false);
            continue;
        }

        switch (pEventInfo->nTargetType)
        {
        case escttCaster:
            pSkillTarget = pCaster;
            break;
        case escttTarget:
            pSkillTarget = pTarget;
            break;
        default:
            assert(false);
            continue;
        }

        nRetCode = Target.SetTarget(pSkillTarget);
        KGLOG_PROCESS_ERROR(nRetCode);
        
        if (pSkill)
        {
            m_SkillDebug.Trace(
                2, "[技能调试][%d]:[技能事件] (%s-%u) 的 技能事件：%u 触发,将施放技能 (%u,%u) By %s-(%u,%d) From (%s-%u)",
                g_pSO3World->m_nGameLoop, m_szName, m_dwID,
                it->dwEventID, pEventInfo->dwSkillID, pEventInfo->dwSkillLevel,
                pSkill->m_pBaseInfo->szSkillName, pSkill->m_pBaseInfo->dwSkillID, pSkill->m_dwLevel,
                pSkillCaster->m_szName, pSkillCaster->m_dwID
            );
        }

        KG_ASSERT_EXIT(pSkillCaster);
        pSkillCaster->CastSkillSub(pSkillCaster, pEventInfo->dwSkillID, pEventInfo->dwSkillLevel, Target);
    }

Exit0:
    return;
}

void KCharacter::CheckBeatBreak(KSkill* pSkill)
{
    int         nBreakRate  = 0; 
    int         nRandNum    = 0;
    KSkill*     pOTSkill    = NULL;
    KCharacter* pSkillSrc   = NULL;
    
    KGLOG_PROCESS_ERROR(pSkill);
    KGLOG_PROCESS_ERROR(m_pSkillAttributeParam);

#ifdef _SERVER
    if (m_OTActionParam.eActionType == otActionUseItemPrepare)
    {   
        KPlayer* pPlayer    = NULL;
        KItem* pItem        = NULL;
        
        KGLOG_PROCESS_ERROR(IS_PLAYER(m_dwID));

        pPlayer = (KPlayer*)this;
        pItem = g_pSO3World->m_ItemSet.GetObj(pPlayer->m_OTActionParam.Data.dwItemID);
        KGLOG_PROCESS_ERROR(pItem);

        pPlayer->m_ItemList.OnBreakUseItem(pItem);
    }
#endif      
    
    KG_PROCESS_ERROR(
        m_OTActionParam.eActionType == otActionSkillChannel ||
        m_OTActionParam.eActionType == otActionSkillPrepare
    );

    pOTSkill = m_OTActionParam.Data.SkillRecipePointer.GetPointer();
    KGLOG_PROCESS_ERROR(pOTSkill);

    nBreakRate = pOTSkill->m_nBrokenRate * pSkill->m_nBreakRate / KILO_NUM;
    nRandNum = g_Random(KILO_NUM);   
    KG_PROCESS_ERROR(nBreakRate + nRandNum >= KILO_NUM);

    DoActionIdle();

#ifdef _SERVER
	g_PlayerServer.DoSkillBeatBack(this, 0, true);
#endif

    m_AIVM.FireEvent(aevOnBreakPerpare, m_dwID, 0);

    pSkillSrc = m_pSkillAttributeParam->pSkillSrc;
    KGLOG_PROCESS_ERROR(pSkillSrc);

    m_SkillDebug.Trace(
        1, "[技能调试][%d]:[技能流程] (%s-%u) 的 (%s-%u,%d) 被打断。By %s-(%u,%u) From %s-%u  打断概率:%d 被打断概率:%d 打断随机值:%d",
        g_pSO3World->m_nGameLoop, m_szName, m_dwID,
        pOTSkill->m_pBaseInfo->szSkillName, pOTSkill->m_pBaseInfo->dwSkillID, pOTSkill->m_dwLevel,
        pSkill->m_pBaseInfo->szSkillName, pSkill->m_pBaseInfo->dwSkillID, pSkill->m_dwLevel,
        pSkillSrc->m_szName, pSkillSrc->m_dwID,
        pSkill->m_nBreakRate, pOTSkill->m_nBrokenRate, nBreakRate
    );

Exit0:
    return;
}

void KCharacter::CheckBeatBack()
{
    int                 nRandNum            = 0;
    int                 nBeatBackFrame      = 0;
    int                 nBeatBackRate       = KILO_NUM;
    KSkill*             pOTSkill            = NULL;
    KSkill*             pSkill              = NULL;
    KCharacter*         pSkillSrc           = NULL;
    static const int    MAX_BEAT_BACK_COUNT = 5;
    static const int    BEAT_BACK_VALUE[MAX_BEAT_BACK_COUNT] = {20, 16, 12, 8, 4};

    KG_PROCESS_ERROR(m_OTActionParam.eActionType != otActionIdle);

    nRandNum = (int)g_Random(KILO_NUM);

    if (m_OTActionParam.eActionType == otActionSkillPrepare || m_OTActionParam.eActionType == otActionSkillChannel)
    {
        pOTSkill = m_OTActionParam.Data.SkillRecipePointer.GetPointer();
        KGLOG_PROCESS_ERROR(pOTSkill);

        nBeatBackRate = m_nBeatBackRate * pOTSkill->m_nBeatBackRate / KILO_NUM;
    }

    KG_PROCESS_ERROR(nBeatBackRate + nRandNum >= KILO_NUM);
    KG_PROCESS_ERROR(m_OTActionParam.nBeatBackCount >= 0);
    KG_PROCESS_ERROR(m_OTActionParam.nBeatBackCount < MAX_BEAT_BACK_COUNT);

    nBeatBackFrame = BEAT_BACK_VALUE[m_OTActionParam.nBeatBackCount];

    OnOTActionBeatBack(nBeatBackFrame);

    m_OTActionParam.nBeatBackCount++;

    assert(m_pSkillAttributeParam);
    pSkill = m_pSkillAttributeParam->SkillRecipePointer.GetPointer();
    pSkillSrc = m_pSkillAttributeParam->pSkillSrc;

    if (pSkill && pSkillSrc)
    {
        m_SkillDebug.Trace(
            2, "[技能调试][%d]:[技能流程] Skill:(%s-%u,%u) 打退:<成功>。已被打退<%d>次, Frame: %d From(%s-%u) To(%s-%u)",
            g_pSO3World->m_nGameLoop,
            pSkill->m_pBaseInfo->szSkillName, pSkill->m_pBaseInfo->dwSkillID, pSkill->m_dwLevel,
            m_OTActionParam.nBeatBackCount,
            nBeatBackFrame,
            pSkillSrc->m_szName, pSkillSrc->m_dwID,
            m_szName, m_dwID
        );
    }

Exit0:
    return;
}


#endif

void KCharacter::OnOTActionBeatBack(int nFrame)
{
    // TODO: 实现统一的打退机制

    switch (m_OTActionParam.eActionType)
    {
    case otActionIdle:
        break;
    case otActionSkillPrepare:
        if (m_OTActionParam.nEndFrame + nFrame - g_pSO3World->m_nGameLoop > m_OTActionParam.nPrepareFrame)
        {
            nFrame = g_pSO3World->m_nGameLoop + m_OTActionParam.nPrepareFrame - m_OTActionParam.nEndFrame;
            m_OTActionParam.nEndFrame = g_pSO3World->m_nGameLoop + m_OTActionParam.nPrepareFrame;
        }
        else
        {
            m_OTActionParam.nEndFrame += nFrame;
        }
#ifdef _SERVER
        g_PlayerServer.DoSkillBeatBack(this, nFrame);
#endif
        break;
    case otActionSkillChannel:
        if (m_OTActionParam.nEndFrame - nFrame < g_pSO3World->m_nGameLoop)
        {
            nFrame = m_OTActionParam.nEndFrame - g_pSO3World->m_nGameLoop;
            m_OTActionParam.nEndFrame = g_pSO3World->m_nGameLoop;
        }
        else
        {
            m_OTActionParam.nEndFrame -= nFrame;
        }
#ifdef _SERVER
        g_PlayerServer.DoSkillBeatBack(this, nFrame);
#endif
        break;
    case otActionPickPrepare:
		{
#ifdef _SERVER
			KDoodad* pDoodad = g_pSO3World->m_DoodadSet.GetObj(m_OTActionParam.Data.dwDoodadID);
			g_PlayerServer.DoSkillBeatBack(this, 0, true);
			if (pDoodad)
			{
				pDoodad->OnBreakPickPrepare();

                if (pDoodad->m_eKind != dkCorpse && IS_PLAYER(m_dwID))
                {
                    pDoodad->CallBreakScriptFunction((KPlayer*)this);
                }
			}
#endif
       		DoActionIdle();
            break;
		}
	case otActionPicking:
		break;
    case otActionUseItemPrepare:
#ifdef _SERVER
        {   
            KPlayer* pPlayer = NULL;
            KItem* pItem = NULL;
            
            KGLOG_PROCESS_ERROR(IS_PLAYER(m_dwID));

            pPlayer = (KPlayer*)this;
            pItem = g_pSO3World->m_ItemSet.GetObj(pPlayer->m_OTActionParam.Data.dwItemID);
            KGLOG_PROCESS_ERROR(pItem);

            pPlayer->m_ItemList.OnBreakUseItem(pItem);
        }
#endif      
        break;
	case otActionRecipePrepare:
        {
#ifdef _SERVER
            KCraft* pCraft  = NULL;

            pCraft = g_pSO3World->m_ProfessionManager.GetCraft(m_OTActionParam.ProfessionSkill.dwCraftID);
            KGLOG_PROCESS_ERROR(pCraft);
                
            pCraft->CallBreakScriptFunction((KPlayer*)this, m_OTActionParam.ProfessionSkill.dwRecipeID);

            g_PlayerServer.DoSkillBeatBack(this, 0, true);
#endif
            DoActionIdle();
            break;
        }
	case otActionCustomPrepare:		
	case otActionCustomChannel:
		//处理打断
#ifdef _SERVER
		g_PlayerServer.DoSkillBeatBack(this, 0, true);
#endif
		DoActionIdle();
		break;
    default:
        assert(false);
    }
Exit0:
    return;
}

void KCharacter::OnDamage(KCharacter* pAttackter, int nDamage)
{
    assert(nDamage > 0);
#ifdef _SERVER
    // 受到攻击的时候清除所有眩晕类Debuff
    m_BuffList.CleanBuffByType(btpStun);
    m_BuffList.CleanBuffByType(btpDaze);

    if (IS_NPC(m_dwID) && pAttackter && IS_PLAYER(pAttackter->m_dwID))
    {
        KNpc* pNpc = (KNpc*)this;        

        if (pNpc->m_dwDropTargetPlayerID == ERROR_ID)
        {
            pNpc->m_dwDropTargetPlayerID = pAttackter->m_dwID;
        }
    }
#endif
}

DWORD KCharacter::GetMapID()
{
	BOOL bRetCode = false;

	KG_PROCESS_ERROR(m_pScene);

	return m_pScene->m_dwMapID;
Exit0:
	return 0;
}

BOOL KCharacter::CheckFear()
{
    BOOL        bResult         = false;
    BOOL        bRetCode        = false;

    KG_PROCESS_ERROR(m_nFearCounter > 0);

#ifdef _SERVER
    if (m_nMoveFrameCounter > 0)
    {
        m_nMoveFrameCounter--;
    }
    else
    {
        int nOdds = g_Random(KILO_NUM);

        if (nOdds + m_nResistBehaveConstraintPercent >= KILO_NUM)
        {
            m_BuffList.CleanBuffByPersist();
            m_AIVM.FireEvent(aevOnOutFear, m_dwID, 0);
        }

        m_nMoveFrameCounter = RESIST_FEAR_INTERVAL;
    }
#endif

    bResult = true;
Exit0:
    return bResult;
}

BOOL KCharacter::OnPassiveMove(void)
{
	if (m_nMoveFrameCounter > 0)
    {
        m_nMoveFrameCounter--;
    }
    else
	{
        m_nVelocityXY       = 0;
        m_nConvergenceSpeed = 0;
        m_nVelocityZ        = 0;
        m_nMoveFrameCounter = g_pSO3World->m_Settings.m_ConstList.nRiseFrame;
		SetMoveState(cmsOnRise);
	}

    return true;
}

BOOL KCharacter::OnDash()
{
	if (m_nMoveFrameCounter > 0)
    {
        m_nMoveFrameCounter--;
    }
    else
    {
        m_nVelocityXY       = 0;
        m_nConvergenceSpeed = 0;
        m_nVelocityZ        = 0;
		SetMoveState(cmsOnStand);
	}

    return true;
}

BOOL KCharacter::OnPull()
{
    if (m_nMoveFrameCounter > 0)
    {
        m_nMoveFrameCounter--;
    }
    else
    {
        m_nVelocityXY       = 0;
        m_nConvergenceSpeed = 0;
        m_nVelocityZ        = 0;
        SetMoveState(cmsOnStand);
    }

    return true;
}

BOOL KCharacter::OnRepulsed()
{
    if (m_nMoveFrameCounter > 0)
    {
        m_nVelocityXY = m_nVelocityXY - REPULSED_DECELERATION;
        MAKE_IN_RANGE(m_nVelocityXY, 0, MAX_ZOOM_VELOCITY);
        m_nMoveFrameCounter--;
    }
    else
    {
        m_nVelocityXY       = 0;
        m_nConvergenceSpeed = 0;
        m_nVelocityZ        = 0;
        m_nMoveFrameCounter = g_pSO3World->m_Settings.m_ConstList.nSkidFrame;
        SetMoveState(cmsOnSkid);
    }

    return true;
}

BOOL KCharacter::OnDeath(void)
{
    BOOL            bResult     = false;
    BOOL            bRetCode    = false;
    KNpc*           pNpc        = NULL;
    KDoodad*        pDoodad     = NULL;
    KLootList*      pLootList   = NULL;
    KScene*         pScene      = NULL;
    KPlayer*        pDropTarget = NULL;

    assert(m_eMoveState == cmsOnDeath);
    assert(m_pScene);

#ifdef _SERVER
    if (m_nMoveFrameCounter > 0)
    {
        m_nMoveFrameCounter--;
        goto Exit1;
    }

    // 只有NPC才需要后面的这些处理
    KG_PROCESS_SUCCESS(IS_PLAYER(m_dwID));

    KG_PROCESS_SUCCESS(m_nZ - m_pCell->m_wHighLayer * POINT_PER_ALTITUDE > 0);

    pNpc = (KNpc*)this;

	pDoodad = pNpc->GenerateCorpse();
    KGLOG_PROCESS_ERROR(pDoodad);
    
    // 取掉落目标
	pDropTarget = pNpc->GetDropTarget();
    if (pDropTarget)
    {
        pLootList = pDoodad->GenerateLootListFromNpc(pDropTarget, pNpc);
        if (pLootList)
	    {
		    pDoodad->m_pLootList = pLootList;

		    if (pLootList->IsEmpty())
		    {
			    pDoodad->LootOver();
		    }
	    }
    }
    else // 无掉落目标
    {
        pDoodad->LootOver();
    }

    // 重生时间为0的,默认不加入重生队列
	if (!(pNpc->m_nReviveTime))
    {
		g_pSO3World->DeleteNpc(pNpc);
        KG_PROCESS_SUCCESS(true);
    }

    pScene = pNpc->m_pScene;
    KGLOG_PROCESS_ERROR(pScene);

    g_pSO3World->RemoveNpc(pNpc, true);

    if (pNpc->m_pTemplate->nReviveMinTime > 0)
    {
        int nReduceThreshold    = g_pSO3World->m_Settings.m_ConstList.nNpcReviveTimeReduceThreshold;
        int nIncreaseThreshold  = g_pSO3World->m_Settings.m_ConstList.nNpcReviveTimeIncreaseThreshold;
        int nReduceRate         = g_pSO3World->m_Settings.m_ConstList.nNpcReviveTimeReduceRate;
        int nIncreaseRate       = g_pSO3World->m_Settings.m_ConstList.nNpcReviveTimeIncreaseRate;

        if (pNpc->m_nInFightGameLoop - pNpc->m_nReviveGameLoop < nReduceThreshold)
            pNpc->m_nReviveTime -= pNpc->m_nReviveTime * nReduceRate / KILO_NUM;

        if (pNpc->m_nInFightGameLoop - pNpc->m_nReviveGameLoop > nIncreaseThreshold)
            pNpc->m_nReviveTime += pNpc->m_nReviveTime * nIncreaseRate / KILO_NUM;

        MAKE_IN_RANGE(pNpc->m_nReviveTime, pNpc->m_pTemplate->nReviveMinTime, pNpc->m_pTemplate->nReviveTime);
    }

    bRetCode = pScene->AddReviveNpc(pNpc, pNpc->m_nReviveTime);
    if (!bRetCode)
    {
	    g_pSO3World->DeleteNpc(pNpc);
        goto Exit0;
    }

#endif

Exit1:
    bResult = true;
Exit0:

#ifdef _SERVER
    if (!bResult)
    {
	    if (pDoodad)
	    {
            pDoodad->m_pLootList = NULL;
            g_pSO3World->DelDoodad(pDoodad);
            pDoodad = NULL;
	    }

	    if (pLootList)
	    {           
		    pLootList->UnInit();
		    KMemory::Delete(pLootList);
            pLootList = NULL;
	    }
    }
#endif

    return bResult;
}

BOOL KCharacter::OnRise()
{
    if (m_nMoveFrameCounter > 0)
    {
        m_nMoveFrameCounter--;
    }
    else
    {
        SetMoveState(cmsOnStand);
    }

    return true;
}

BOOL KCharacter::OnSkid()
{
    if (m_nMoveFrameCounter > 0)
    {
        m_nMoveFrameCounter--;
    }
    else
    {
        SetMoveState(cmsOnStand);
    }
    return true;
}

void KCharacter::SetMoveState(int nState)
{
    BOOL bUpdateQuestMark = false;

    assert(nState > cmsInvalid && nState < cmsTotal);

    switch (nState)
    {
    case cmsOnStand:
        #ifdef _CLIENT
        if (m_eMoveState == cmsOnAutoFly)
        { // 下交通的时候的事件
            if (g_pGameWorldUIHandler)
            {
                g_pGameWorldUIHandler->OnEndAutoFly();
            }
        }

        if (m_eMoveState == cmsOnDeath && IS_PLAYER(m_dwID))
        {
            bUpdateQuestMark = true;
        }
        #endif
        break;

    case cmsOnWalk:
    case cmsOnRun:
    case cmsOnJump:
    case cmsOnSwimJump:
    case cmsOnSit:
    case cmsOnDash:
    case cmsOnPull:
    case cmsOnHalt:
    case cmsOnFreeze:
    case cmsOnEntrap:
    case cmsOnSkid:
    case cmsOnRise:
    case cmsOnRepulsed:
        break;

    case cmsOnSwim:
    case cmsOnFloat:
        #ifdef _SERVER
        if (IS_PLAYER(m_dwID))
        {
            KPlayer* pPlayer = (KPlayer*)this;

            if (pPlayer->m_bOnHorse)
                pPlayer->DownHorse();
        }
        #else
        if (m_eMoveState == cmsOnDeath && IS_PLAYER(m_dwID))
        {
            bUpdateQuestMark = true;
        }
        #endif
        break;

    case cmsOnAutoFly:
        #ifdef _CLIENT
        if (m_eMoveState == cmsOnAutoFly)
        { // 从AutoFly切换到AutoFly的情况为切换地替
            if (g_pGameWorldUIHandler)
            {
                g_pGameWorldUIHandler->OnAutoFlySwitchMap();
            }
        }
        else
        { // 从其他状态切换到AutoFly的情况为上交通
            if (g_pGameWorldUIHandler)
            {
                g_pGameWorldUIHandler->OnStartAutoFly();
            }
        }
        #endif
        break;

    case cmsOnKnockedDown:
    case cmsOnKnockedBack:
    case cmsOnKnockedOff:
        #ifdef _CLIENT
        if (g_pGameWorldUIHandler)
        {
            g_pGameWorldUIHandler->OnOTActionProgressBreak(m_dwID);
        }
        #endif
        break;

    case cmsOnDeath:
        #ifdef _CLIENT
        if (IS_PLAYER(m_dwID))
        {
            bUpdateQuestMark = true;
        }
        #endif
        m_nDivingCount      = 0;
        break;

    default:
        assert(false);
        break;
    }

    m_eMoveState = (CHARACTER_MOVE_STATE)nState;

#ifdef _CLIENT
    if (bUpdateQuestMark)
    {
        KPlayer* pPlayer = (KPlayer*)this;

        pPlayer->m_QuestList.UpdateNpcQuestMark(-1);
    }
#endif
}

// ----------------- 持续一段时间的行为 ----------------------------->


BOOL KCharacter::DoOTAction(KOT_ACTION_PARAM& rParam)
{
	BOOL bRetCode = false;

	KG_PROCESS_ERROR(m_eMoveState != cmsOnDeath);

	switch (m_OTActionParam.eActionType)
	{
    case otActionIdle:
        break;
	case otActionPickPrepare:
		bRetCode = OnBreakPickPrepare(rParam);
		KG_PROCESS_ERROR(bRetCode);
		break;
	case otActionPicking:
		bRetCode = OnBreakPicking(rParam);
		KG_PROCESS_ERROR(bRetCode);
	    break;
	default:
#ifdef _SERVER
        if (m_dwID && IS_PLAYER(m_dwID))
        {
            KPlayer* pPlayer = (KPlayer*)this;
            g_PlayerServer.DoMessageNotify(pPlayer->m_nConnIndex, ectSkillErrorCode, srcInOTAction);
        }
#endif
        goto Exit0;
	}

    m_OTActionParam = rParam;
    m_OTActionParam.nBeatBackCount = 0;

	return true;
Exit0:
	return false;
}


void KCharacter::DoActionIdle(void)
{   
    // 切到Idle可能是因为正常的完成了,不见得就是"打断"
	m_OTActionParam.eActionType = otActionIdle;
}

void KCharacter::DoAutoCastSkill(KAUTO_CAST_SKILL_PARAM& param)
{
    m_AutoCastSkillParam.SkillRecipePointer = param.SkillRecipePointer;
    m_AutoCastSkillParam.bMelee             = param.bMelee;
    m_AutoCastSkillParam.pnCastCycle        = param.pnCastCycle;

#ifdef _SERVER 
    if (IS_PLAYER(m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)this;
        g_PlayerServer.DoSyncCommonSkillAutoCastState(
            pPlayer->m_nConnIndex, true, m_AutoCastSkillParam.bMelee
        );
    }
#endif

    return;
}

void KCharacter::DoAutoCastSkillIdle()
{
    KSkill* pOldSkill = NULL;

#ifdef _SERVER 
    if (IS_PLAYER(m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)this;

        g_PlayerServer.DoSyncCommonSkillAutoCastState(
            pPlayer->m_nConnIndex, false, m_AutoCastSkillParam.bMelee
        );
    }
#endif

    m_AutoCastSkillParam.SkillRecipePointer.ClearRecipeKey();
    return;
}

BOOL KCharacter::DoSkillPrepare(KSkillRecipePointer& rSkillRecipePointer, int nPrepareFrame, int nEndFrame)
{
	BOOL                bRetCode    = false;
    KOT_ACTION_PARAM    ActionParam;

    ActionParam.eActionType             = otActionSkillPrepare;
    ActionParam.Data.SkillRecipePointer = rSkillRecipePointer;
    ActionParam.nBeatBackCount          = 0;
    ActionParam.nEndFrame               = nEndFrame;
    ActionParam.nPrepareFrame           = nPrepareFrame;

    bRetCode = DoOTAction(ActionParam);
	KG_PROCESS_ERROR(bRetCode);

    m_AIVM.FireEvent(aevOnBeginPrepare, m_dwID, 0);

	return true;
Exit0:
	return false;
}

BOOL KCharacter::DoSkillChannel(KSkillRecipePointer& rSkillRecipePointer, int nChannelFrame)
{
	BOOL                bRetCode    = false;
    KSkill*             pSkill      = NULL;
    KOT_ACTION_PARAM    ActionParam;

    pSkill = rSkillRecipePointer.GetPointer();
    KG_PROCESS_ERROR(pSkill);

    ActionParam.eActionType             = otActionSkillChannel;
    ActionParam.Data.SkillRecipePointer = rSkillRecipePointer;
    ActionParam.nBeatBackCount          = 0;
    ActionParam.nEndFrame               = g_pSO3World->m_nGameLoop + nChannelFrame;
    ActionParam.nNextActFrame           = g_pSO3World->m_nGameLoop + pSkill->m_nChannelInterval;

	bRetCode = DoOTAction(ActionParam);
	KG_PROCESS_ERROR(bRetCode);

	return true;
Exit0:
	return false;
}

BOOL KCharacter::DoRecipePrepare(KProfessionSkill* pProfessionSkill, int nFrame)
{
    BOOL                bResult  = false;
	BOOL                bRetCode = false;
    KOT_ACTION_PARAM    ActionParam;

	KGLOG_PROCESS_ERROR(pProfessionSkill);

    ActionParam.eActionType     = otActionRecipePrepare;
	ActionParam.ProfessionSkill = *(pProfessionSkill);
    ActionParam.nBeatBackCount  = 0;
    ActionParam.nEndFrame       = g_pSO3World->m_nGameLoop + nFrame;
    ActionParam.nNextActFrame   = 0;

	bRetCode = DoOTAction(ActionParam);
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	return bResult;
}

BOOL KCharacter::DoPickPrepare(KDoodad* pDoodad, int nFrame)
{
	BOOL bRetCode = false;
    KOT_ACTION_PARAM ActionParam;

	KGLOG_PROCESS_ERROR(pDoodad);

    ActionParam.eActionType  = otActionPickPrepare;
    ActionParam.Data.dwDoodadID = pDoodad->m_dwID;
	ActionParam.nEndFrame    = g_pSO3World->m_nGameLoop + nFrame;

	bRetCode = DoOTAction(ActionParam);
	KG_PROCESS_ERROR(bRetCode);

	return true;
Exit0:
	return false;
}

BOOL KCharacter::DoPicking(KDoodad* pDoodad)
{
    BOOL                bResult     = false;
	BOOL                bRetCode    = false;
	KOT_ACTION_PARAM    ActionParam;

    assert(pDoodad);

	ActionParam.eActionType     = otActionPicking;
	ActionParam.Data.dwDoodadID = pDoodad->m_dwID;
	ActionParam.nEndFrame       = INT_MAX;

	bRetCode = DoOTAction(ActionParam);
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	return bResult;
}

BOOL KCharacter::DoCustomOTActionPrepare(BOOL bIsBreakable, int nFrame, KTarget& crTarget, DWORD dwScriptID)
{
	BOOL bRetCode = false;
	KOT_ACTION_PARAM ActionParam;

	ActionParam.eActionType = otActionCustomPrepare;
	ActionParam.Target = crTarget;
	ActionParam.nEndFrame = g_pSO3World->m_nGameLoop + nFrame;
	ActionParam.bIsBreakable = bIsBreakable;
	ActionParam.dwScriptID = dwScriptID;

	bRetCode = DoOTAction(ActionParam);
	KG_PROCESS_ERROR(bRetCode);

	return true;
Exit0:
	return false;
}

BOOL KCharacter::DoCustomOTActionChannel(BOOL bIsBreakable, int nFrame, KTarget& crTarget, DWORD dwScriptID)
{
	BOOL bRetCode = false;
	KOT_ACTION_PARAM ActionParam;

	ActionParam.eActionType = otActionCustomChannel;
	ActionParam.Target = crTarget;
	ActionParam.nEndFrame = g_pSO3World->m_nGameLoop + nFrame;
	ActionParam.bIsBreakable = bIsBreakable;
	ActionParam.dwScriptID = dwScriptID;

	bRetCode = DoOTAction(ActionParam);
	KG_PROCESS_ERROR(bRetCode);

	return true;
Exit0:
	return false;
}

void KCharacter::ProcessOTAction()
{
    switch (m_OTActionParam.eActionType)
    {
    case otActionIdle:
        // do nothing !
        break;
    case otActionSkillPrepare:
        OnSkillPrepare();
        break;
    case otActionSkillChannel:
        OnSkillChannel();
        break;
    case otActionRecipePrepare:
        OnRecipePrepare();
        break;
    case otActionPickPrepare:
        OnPickPrepare();
        break;
	case otActionPicking:
		OnPicking();
		break;
	case otActionCustomPrepare:
		OnCustomOTActionPrepare();
		break;
	case otActionCustomChannel:
		OnCustomOTActionChannel();
		break;
    case otActionUseItemPrepare:
        OnUseItemPrepare();
        break;
    default:
        assert(false);
    }
}

void KCharacter::ProcessAutoCastSkill()
{
    KSkill* pSkill      = NULL;
    int     nRetCode    = srcFailed;

#ifdef _SERVER 
    pSkill = m_AutoCastSkillParam.SkillRecipePointer.GetPointer();
    KG_PROCESS_ERROR(pSkill);

	if (m_eMoveState == cmsOnDeath)
	{
		DoAutoCastSkillIdle();
		goto Exit0;
	}
    
    nRetCode = m_SelectTarget.GetTargetType();
    if (nRetCode == ttNpc || nRetCode == ttPlayer)
    {
        KCharacter* pTarget = NULL;
        nRetCode = m_SelectTarget.GetTarget(&pTarget);
        if (!nRetCode || !pTarget || !pTarget->m_pScene || pTarget->m_eMoveState == cmsOnDeath)
        {
            // 如果目标已经不存在了,则停止
            DoAutoCastSkillIdle();
            goto Exit0;
        }
    }

    KG_PROCESS_ERROR(m_OTActionParam.eActionType == otActionIdle);

	KG_PROCESS_ERROR(
		g_pSO3World->m_nGameLoop - m_AutoCastSkillParam.nPreCastFrame > *m_AutoCastSkillParam.pnCastCycle
    );

    nRetCode = pSkill->CanCast(this, m_SelectTarget);
    
    // 普通攻击自动释放过程中,目标超出攻击范围后,并不停止在自动攻击    
    if (nRetCode != srcSuccess)
    {   
        if (nRetCode != srcTooCloseTarget && nRetCode != srcTooFarTarget && nRetCode != srcOutOfAngle)
        {
            DoAutoCastSkillIdle();
        }
        goto Exit0;
    }

    m_AutoCastSkillParam.nPreCastFrame = g_pSO3World->m_nGameLoop;

    CastSkill(pSkill->m_pBaseInfo->dwSkillID, pSkill->m_dwLevel, m_SelectTarget);
#endif

Exit0:
    return;
}

BOOL KCharacter::OnCustomOTActionPrepare()
{
    BOOL            bResult         = false;
	BOOL            bRetCode        = false;
    TARGET_TYPE     eTargetType     = ttInvalid;
    KSceneObject*   pSceneObject    = NULL;

	if (m_eMoveState != cmsOnStand && m_eMoveState != cmsOnFloat)
	{
#ifdef _SERVER
		//执行打断的脚本
		bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(m_OTActionParam.dwScriptID);
		if (bRetCode && g_pSO3World->m_ScriptCenter.IsFuncExist(m_OTActionParam.dwScriptID, SCRIPT_ON_CUSTOM_BREAK))
		{
			int nTopIndex = 0;
			g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);

			g_pSO3World->m_ScriptCenter.PushValueToStack((KPlayer*)this);

			g_pSO3World->m_ScriptCenter.CallFunction(m_OTActionParam.dwScriptID, SCRIPT_ON_CUSTOM_BREAK, 0);
			g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);
		}
#endif //_SERVER

#ifdef _CLIENT
        if (g_pGameWorldUIHandler)
        {
		    g_pGameWorldUIHandler->OnOTActionProgressBreak(m_dwID);
        }
#endif
		DoActionIdle();
		goto Exit1;
	}

	KG_PROCESS_SUCCESS(g_pSO3World->m_nGameLoop < m_OTActionParam.nEndFrame);

#if defined(_SERVER)
	//执行脚本
	bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(m_OTActionParam.dwScriptID);
	if (bRetCode && g_pSO3World->m_ScriptCenter.IsFuncExist(m_OTActionParam.dwScriptID, SCRIPT_ON_CUSTOM_EVENT))
	{
		int nTopIndex = 0;
		KTarget Target = m_OTActionParam.Target;

		g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);

		g_pSO3World->m_ScriptCenter.PushValueToStack((KPlayer*)this);

        eTargetType = Target.GetTargetType();
		switch (eTargetType)
		{
		case ttNoTarget:
			break;
		case ttNpc:
            bRetCode = Target.GetTarget(&pSceneObject);
            KGLOG_PROCESS_ERROR(bRetCode);
			if (pSceneObject)
			{
				g_pSO3World->m_ScriptCenter.PushValueToStack((KNpc*)pSceneObject);
			}
			break;
		case ttPlayer:
            bRetCode = Target.GetTarget(&pSceneObject);
            KGLOG_PROCESS_ERROR(bRetCode);
			if (pSceneObject)
			{
				g_pSO3World->m_ScriptCenter.PushValueToStack((KPlayer*)pSceneObject);
			}
			break;
		case ttDoodad:
            bRetCode = Target.GetTarget(&pSceneObject);
            KGLOG_PROCESS_ERROR(bRetCode);
			if (pSceneObject)
			{
				g_pSO3World->m_ScriptCenter.PushValueToStack((KDoodad*)pSceneObject);
			}
			break;
		default:
			KGLOG_PROCESS_ERROR(false);
		}

		g_pSO3World->m_ScriptCenter.CallFunction(m_OTActionParam.dwScriptID, SCRIPT_ON_CUSTOM_EVENT, 0);
		g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);
	}
#endif //_SERVER

	DoActionIdle();
Exit1:
	bResult = true;
Exit0:
	if(!bResult)
	{
		DoActionIdle();
	}
	return bResult;
}

BOOL KCharacter::OnCustomOTActionChannel()
{
    BOOL            bResult         = false;
	BOOL            bRetCode        = false;
    TARGET_TYPE     eTargetType     = ttInvalid;
    KSceneObject*   pSceneObject    = NULL;

	if (m_eMoveState != cmsOnStand && m_eMoveState != cmsOnFloat)
	{
#ifdef _SERVER
		//执行打断的脚本
		bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(m_OTActionParam.dwScriptID);
		if (bRetCode && g_pSO3World->m_ScriptCenter.IsFuncExist(m_OTActionParam.dwScriptID, SCRIPT_ON_CUSTOM_BREAK))
		{
			int nTopIndex = 0;
			g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);

			g_pSO3World->m_ScriptCenter.PushValueToStack((KPlayer*)this);

			g_pSO3World->m_ScriptCenter.CallFunction(m_OTActionParam.dwScriptID, SCRIPT_ON_CUSTOM_BREAK, 0);
			g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);
		}
#endif //_SERVER

#ifdef _CLIENT
        if (g_pGameWorldUIHandler)
        {
		    g_pGameWorldUIHandler->OnOTActionProgressBreak(m_dwID);
        }
#endif
		DoActionIdle();
		goto Exit1;
	}
    
	KG_PROCESS_SUCCESS(g_pSO3World->m_nGameLoop < m_OTActionParam.nEndFrame);

#if defined(_SERVER)
	//执行脚本
	bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(m_OTActionParam.dwScriptID);
	if (bRetCode && g_pSO3World->m_ScriptCenter.IsFuncExist(m_OTActionParam.dwScriptID, SCRIPT_ON_CUSTOM_EVENT))
	{
		int nTopIndex = 0;
		KTarget Target = m_OTActionParam.Target;
		eTargetType = Target.GetTargetType();

		g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);
		
		g_pSO3World->m_ScriptCenter.PushValueToStack((KPlayer*)this);
			
		switch(eTargetType)
		{
		case ttNoTarget:
			break;
		case ttNpc:
            bRetCode = Target.GetTarget(&pSceneObject);
            KGLOG_PROCESS_ERROR(bRetCode);
			if (pSceneObject)
			{
				g_pSO3World->m_ScriptCenter.PushValueToStack((KNpc*)pSceneObject);
			}
			break;
		case ttPlayer:
            bRetCode = Target.GetTarget(&pSceneObject);
            KGLOG_PROCESS_ERROR(bRetCode);
			if (pSceneObject)
			{
				g_pSO3World->m_ScriptCenter.PushValueToStack((KPlayer*)pSceneObject);
			}
			break;
		case ttDoodad:
            bRetCode = Target.GetTarget(&pSceneObject);
            KGLOG_PROCESS_ERROR(bRetCode);
			if (pSceneObject)
			{
				g_pSO3World->m_ScriptCenter.PushValueToStack((KDoodad*)pSceneObject);
			}
			break;
		default:
			KGLOG_PROCESS_ERROR(false);
		}

		g_pSO3World->m_ScriptCenter.CallFunction(m_OTActionParam.dwScriptID, SCRIPT_ON_CUSTOM_EVENT, 0);
		g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);
	}
#endif //_SERVER

	DoActionIdle();
Exit1:
	bResult = true;
Exit0:
	if(!bResult)
	{
		DoActionIdle();
	}
	return bResult;
}

void KCharacter::OnUseItemPrepare()
{
    BOOL                bRetCode        = false;
    KPlayer*            pPlayer         = NULL;
    KItem*              pItem           = NULL;

    KGLOG_PROCESS_ERROR(IS_PLAYER(m_dwID));
	pPlayer = (KPlayer*)this;

#ifdef _SERVER
	pItem = g_pSO3World->m_ItemSet.GetObj(m_OTActionParam.Data.dwItemID);
	if (pItem == NULL)
    {
        pPlayer->m_ItemList.OnBreakUseItem(pItem);
		goto Exit0;
    }

    if (m_eMoveState != cmsOnStand && m_eMoveState != cmsOnFloat && m_eMoveState != cmsOnSit)
	{
		pPlayer->m_ItemList.OnBreakUseItem(pItem);
		goto Exit0;
	}
    
    if (pItem->m_dwSkillID)
    {
        SKILL_RESULT_CODE   eRetCode  = srcFailed;
        KSkill*             pSkill    = NULL;
        KSKILL_RECIPE_KEY   RecipeKey;

        bRetCode = g_pSO3World->m_SkillManager.GetSkillRecipeKey(&RecipeKey, pItem->m_dwSkillID, pItem->m_dwSkillLevel, this);
        KGLOG_PROCESS_ERROR(bRetCode);
    	
	    pSkill = g_pSO3World->m_SkillManager.GetSkill(RecipeKey);
	    KGLOG_PROCESS_ERROR(pSkill);

        eRetCode = pSkill->CheckSelfState(this);
	    if (eRetCode != srcSuccess)
	    {
            pPlayer->m_ItemList.OnBreakUseItem(pItem);
		    goto Exit0;
	    }
    }
#endif

    KG_PROCESS_ERROR(g_pSO3World->m_nGameLoop >= m_OTActionParam.nEndFrame);
    
    pPlayer->m_ItemList.FinishUseItem(pItem);
    DoActionIdle();

Exit0:
	return;
}

#ifdef _SERVER
void KCharacter::DoAutoTurn()
{
    int           nRetCode     = false;
    int           nTargetX     = 0;
    int           nTargetY     = 0;
    int           nTargetZ     = 0;
    TARGET_TYPE   eTargetType  = ttInvalid;
    KSceneObject* pSceneObject = NULL;
    int           nDirection   = 0;

    KGLOG_PROCESS_ERROR(m_bAutoTurnFlag);

    eTargetType = m_SkillTarget.GetTargetType();
    switch(eTargetType)
    {
    case ttCoordination:
        nRetCode = m_SkillTarget.GetTarget(&nTargetX, &nTargetY, &nTargetZ);
        KG_PROCESS_ERROR(nRetCode);
        break;
    case ttPlayer:
    case ttNpc:
    case ttDoodad:
        nRetCode = m_SkillTarget.GetTarget(&pSceneObject);
        KG_PROCESS_ERROR(nRetCode);
        KG_PROCESS_ERROR(pSceneObject);
        KG_PROCESS_ERROR(pSceneObject->m_pCell);

        pSceneObject->GetAbsoluteCoordinate(&nTargetX, &nTargetY, &nTargetZ);
        break;
    default:
        goto Exit0;
    }
    
    KG_PROCESS_ERROR(m_nX != nTargetX || m_nY != nTargetY);

    nDirection = g_GetDirection(m_nX, m_nY, nTargetX, nTargetY);
    KG_PROCESS_ERROR(nDirection != m_nFaceDirection);

    nRetCode = TurnTo(nDirection, true);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void KCharacter::DoSitDown(KDoodad* pDoodad)
{
	BOOL    bRetCode           = false;
	int     nSitDownActionID   = 0;
	int     nX                 = 0;
	int     nY                 = 0;
	int     nZ                 = 0;

	KG_PROCESS_ERROR(pDoodad);
	KGLOG_PROCESS_ERROR(pDoodad->m_pScene == m_pScene);

	KG_PROCESS_ERROR(m_eMoveState == cmsOnStand || m_eMoveState == cmsOnWalk || m_eMoveState == cmsOnRun);

	bRetCode = pDoodad->GetAbsoluteCoordinate(&nX, &nY, &nZ);
    KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = MoveTo(nX, nY, nZ, false, true);
    KGLOG_PROCESS_ERROR(bRetCode);

    KGLOG_PROCESS_ERROR(m_pScene);

	TurnTo(pDoodad->m_nFaceDirection, true);
 
    SitDown(true);

Exit0:
	return;
}
#endif

BOOL KCharacter::OnSkillPrepare()
{
    BOOL    bResult     = false;
    int     nRetCode    = false;
    KSkill* pSkill      = NULL;
    
    pSkill = m_OTActionParam.Data.SkillRecipePointer.GetPointer();
    KGLOG_PROCESS_ERROR(pSkill);

    KGLOG_PROCESS_ERROR(pSkill->m_pBaseInfo->nCastMode != scmItem); // 对道具使用的技能不能走到该流程。

    nRetCode = pSkill->CheckSelfState(this);
    if (nRetCode != srcSuccess)
    {
#ifdef _CLIENT
        if (g_pGameWorldUIHandler)
        {
            g_pGameWorldUIHandler->OnOTActionProgressBreak(m_dwID);
        }
#endif

        if (IS_PLAYER(m_dwID))
        {
            KPlayer* pPlayer = (KPlayer*)this;
            pSkill->ClearPublicCooldown(pPlayer);
        }

        m_AIVM.FireEvent(aevOnBreakPerpare, m_dwID, 0);

        DoActionIdle();
        goto Exit1;
    }

#ifdef _SERVER
    if (m_bAutoTurnFlag)
    {
        DoAutoTurn();
    }
#endif

    KG_PROCESS_SUCCESS(g_pSO3World->m_nGameLoop < m_OTActionParam.nEndFrame);

#ifdef _SERVER
    nRetCode = pSkill->CanCastOnPrepare(this, m_SkillTarget);
    if (nRetCode != srcSuccess)
    {
        if (IS_PLAYER(m_dwID))
        {
            KPlayer* pPlayer = (KPlayer*)this;
            nRetCode = g_PlayerServer.DoMessageNotify(
				pPlayer->m_nConnIndex, ectSkillErrorCode, nRetCode);
            KGLOG_CHECK_ERROR(nRetCode);
        }

        m_AIVM.FireEvent(aevOnCastSkillFailed, m_dwID, 0);

        DoActionIdle();
        goto Exit1;
    }

    pSkill->DoCost(this);

    nRetCode = IS_PLAYER(m_dwID);
    if (nRetCode)
    {
        KPlayer* pPlayer = (KPlayer*)this;
        pSkill->ResetNormalCooldown(pPlayer);
    }

    if (pSkill->m_pBaseInfo->bIsChannelSkill)
    {
        int nChannelFrame = pSkill->m_nChannelFrame;
        // ... ...
        DoSkillChannel(m_OTActionParam.Data.SkillRecipePointer, nChannelFrame);
    }
    else
    {
        DoActionIdle();
    }

    nRetCode = g_PlayerServer.DoSkillCast(this, pSkill, m_SkillTarget, false);
    KGLOG_CHECK_ERROR(nRetCode); 

	nRetCode = pSkill->Cast(this, this, m_SkillTarget, m_OTActionParam.Data.SkillRecipePointer.GetRecipeKey());
    KGLOG_CHECK_ERROR(nRetCode == srcSuccess);

    m_AIVM.FireEvent(aevOnCastSkill, m_dwID, 0);

    // 技能释放事件(吟唱技)
    {
        BOOL        bRetCode     = false;
        KCharacter* pEventTarget = NULL;
        switch(m_SkillTarget.GetTargetType())
        {
        case ttPlayer:
        case ttNpc:
            bRetCode = m_SkillTarget.GetTarget(&pEventTarget);
            KGLOG_PROCESS_ERROR(bRetCode);
            break;
        }
        OnSkillEvent(pSkill, seCast, m_SkillEventVector, this, pEventTarget);
        if (pEventTarget)
        {
            pEventTarget->OnSkillEvent(pSkill, seBeCast, pEventTarget->m_SkillEventVector, pEventTarget, this);
        }
    }
#endif // _SERVER

#ifdef _CLIENT
    DoActionIdle();
#endif

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KCharacter::OnSkillChannel(void)
{
    BOOL    bResult  = false;
    int     nRetCode = false;
    KSkill *pSkill   = NULL;
    
    pSkill = m_OTActionParam.Data.SkillRecipePointer.GetPointer();
    KGLOG_PROCESS_ERROR(pSkill);
    
    KGLOG_PROCESS_ERROR(pSkill->m_pBaseInfo->bIsChannelSkill);

    nRetCode = pSkill->CheckSelfState(this);
    if (nRetCode != srcSuccess)
    {
#ifdef _CLIENT
        if (g_pGameWorldUIHandler)
        {
            g_pGameWorldUIHandler->OnOTActionProgressBreak(m_dwID);
        }
#endif
        DoActionIdle();
        goto Exit1;
    }

    if (g_pSO3World->m_nGameLoop > m_OTActionParam.nEndFrame)
    {
        DoActionIdle();
        goto Exit1;
    }

#ifdef _SERVER
    if (m_bAutoTurnFlag)
    {
        DoAutoTurn();
    }
#endif

    KG_PROCESS_SUCCESS(g_pSO3World->m_nGameLoop < m_OTActionParam.nNextActFrame);
    m_OTActionParam.nNextActFrame  = g_pSO3World->m_nGameLoop + pSkill->m_nChannelInterval;

#ifdef _SERVER
    nRetCode = CastSkillSub(this, pSkill->m_pBaseInfo->dwSkillID, pSkill->m_dwLevel, m_SkillTarget);
    KG_PROCESS_ERROR(nRetCode == srcSuccess);
#endif

Exit1:
    bResult = true;
Exit0:
    if (!bResult)
    {
#ifdef _SERVER
        g_PlayerServer.DoSkillBeatBack(this, 0, true);
#endif
        DoActionIdle();
    }
    return bResult;
}

BOOL KCharacter::OnRecipePrepare()
{
    BOOL                bResult             = false;
    int                 nRetCode            = false;
	KCraft*		        pCraft              = NULL;
	KProfessionSkill*   pProfessionSkill    = NULL;

	pProfessionSkill = &(m_OTActionParam.ProfessionSkill);
	KGLOG_PROCESS_ERROR(pProfessionSkill);

#ifdef _SERVER
	pCraft = g_pSO3World->m_ProfessionManager.GetCraft(pProfessionSkill->dwCraftID);
	KGLOG_PROCESS_ERROR(pCraft);
#endif
	
    if (m_eMoveState != cmsOnStand && m_eMoveState != cmsOnFloat)
    {
#ifdef _SERVER
        pCraft->CallBreakScriptFunction((KPlayer*)this, pProfessionSkill->dwRecipeID); 
#else
        if (g_pGameWorldUIHandler)
        {
            g_pGameWorldUIHandler->OnOTActionProgressBreak(m_dwID);
        }

		if (g_pGameWorldRepresentHandler)
            g_pGameWorldRepresentHandler->OnCharacterEndCraftAnimation(this, pProfessionSkill->dwCraftID);
#endif
        DoActionIdle();
        goto Exit1;
    }

    KG_PROCESS_SUCCESS(g_pSO3World->m_nGameLoop < m_OTActionParam.nEndFrame);

#ifdef _SERVER
    nRetCode = IS_PLAYER(m_dwID);
    KGLOG_PROCESS_ERROR(nRetCode);

	pCraft->Cast((KPlayer*)this, pProfessionSkill->dwRecipeID, pProfessionSkill->dwSourceItemID, pProfessionSkill->Target);
#endif

#ifdef _CLIENT
	if (g_pGameWorldRepresentHandler)
    {
        g_pGameWorldRepresentHandler->OnCharacterEndCraftAnimation(this, pProfessionSkill->dwCraftID);
    }
#endif 
    DoActionIdle();

Exit1:
    bResult = true;
Exit0:
    if (!bResult)
    {
        DoActionIdle();
    }
    return bResult;
}

BOOL KCharacter::OnPickPrepare()
{
    BOOL     bResult = false;
    KDoodad* pDoodad = NULL;

	pDoodad = g_pSO3World->m_DoodadSet.GetObj(m_OTActionParam.Data.dwDoodadID);
    KG_PROCESS_ERROR(pDoodad); 
	KG_PROCESS_ERROR(pDoodad->m_pScene);
    KGLOG_PROCESS_ERROR(pDoodad->m_pTemplate);

	if (m_eMoveState != cmsOnStand && m_eMoveState != cmsOnFloat)
	{
		//当移动状态机不为站立时，判断拾取动作已被打断
#ifdef _SERVER
		if (pDoodad->m_eKind != dkCorpse)
		{
			pDoodad->CallBreakScriptFunction((KPlayer*)this);
		}

        pDoodad->OnBreakPickPrepare();
#endif //_SERVER

#ifdef _CLIENT
        if (g_pGameWorldUIHandler)
        {
		    g_pGameWorldUIHandler->OnOTActionProgressBreak(m_dwID);
        }
#endif

		DoActionIdle();
	
        goto Exit1;
	}

    KG_PROCESS_SUCCESS(g_pSO3World->m_nGameLoop < m_OTActionParam.nEndFrame);
    // 若PickPrepare时间到，则成功操作Doodad，有掉落则进入Picking

#ifdef _SERVER
	if (IS_PLAYER(this->m_dwID))
	{
        KPlayer* pPlayer = (KPlayer*)this;

		bResult = pDoodad->PerOpen(pPlayer);
		KG_PROCESS_ERROR(bResult);

        if (pDoodad->m_pTemplate->m_bCanPick)
        {
            g_PlayerServer.DoOpenLootList(pPlayer->m_nConnIndex, pPlayer, pDoodad);
        }
	}
#endif

	if (!pDoodad->m_pTemplate->m_bCanPick)
	{
        if (pDoodad->m_eKind != dkDoor)
        {
            pDoodad->ChangeState(dsIdle);
        }
        DoActionIdle();

        goto Exit1;
	}

#ifdef _SERVER
    pDoodad->ChangeState(dsAfterHit);
#endif

#ifdef _CLIENT
	if (g_pGameWorldRepresentHandler)
    {
        g_pGameWorldRepresentHandler->OnCharacterBeginPickDoodad(this, pDoodad);
    }
#endif

	DoPicking(pDoodad);
	
Exit1:
    bResult = true;
Exit0:
    if (!bResult)
    {
#ifdef _CLIENT
        if (g_pGameWorldRepresentHandler && pDoodad)
        {	
            g_pGameWorldRepresentHandler->OnCharacterEndPickDoodad(this, pDoodad);
        }
        
        if (g_pGameWorldUIHandler)
        {
            g_pGameWorldUIHandler->OnOTActionProgressBreak(m_dwID);
        }
#endif
        DoActionIdle();
    }
	return bResult;
}

BOOL KCharacter::OnPicking()
{
	BOOL     bResult = false;
	KDoodad* pDoodad = NULL;

	pDoodad = g_pSO3World->m_DoodadSet.GetObj(m_OTActionParam.Data.dwDoodadID);
	KG_PROCESS_ERROR(pDoodad);
	KG_PROCESS_ERROR(pDoodad->m_pRegion);

    if (m_eMoveState != cmsOnStand && m_eMoveState != cmsOnFloat)
    {
        if (pDoodad->m_pScene)
        {
            pDoodad->ChangeState(dsIdle);
        }

#ifdef _SERVER
        if (pDoodad->m_pLootList)
        {
            pDoodad->Close();
        }
#else
        if (m_dwID == g_pSO3World->m_dwClientPlayerID)
        {
            if (g_pGameWorldUIHandler)
            {
                KUIEventCloseDoodad param;
                param.dwDoodadID = pDoodad->m_dwID;
                param.dwPlayerID = m_dwID;
                g_pGameWorldUIHandler->OnCloseDoodad(param);
            }

            if (g_pGameWorldRepresentHandler)
            {
                g_pGameWorldRepresentHandler->OnCharacterEndPickDoodad(this, pDoodad);
            }
        }
#endif
        goto Exit0;
    }

#ifdef _SERVER
    if (pDoodad->m_pLootList == NULL)
    {
        if (pDoodad->m_pScene)
        {
            pDoodad->ChangeState(dsIdle);
        }

        KGLOG_PROCESS_ERROR(IS_PLAYER(m_dwID));
        g_PlayerServer.DoFinishLoot((KPlayer*)this, pDoodad); // doodad没有lootlist，应该结束拾取
        goto Exit0;
    }
#endif	
	
    bResult = true;
Exit0:
    if (!bResult)
    {
        DoActionIdle();
    }
	return bResult;
}

//----------- 处理打断 ------------------->

BOOL KCharacter::OnBreakPickPrepare(KOT_ACTION_PARAM& rNewAction)
{
	KDoodad* pDoodad = NULL;
	BOOL bResult = false;

	pDoodad = g_pSO3World->m_DoodadSet.GetObj(m_OTActionParam.Data.dwDoodadID);
	KGLOG_PROCESS_ERROR(pDoodad);

	switch (rNewAction.eActionType)
	{
	case otActionPicking:
		break;
	case otActionIdle:
#ifdef _SERVER
		if (pDoodad->m_eKind != dkCorpse)
		{
			bResult = g_pSO3World->m_ScriptCenter.IsScriptExist(pDoodad->m_dwScriptID);
			if (bResult && g_pSO3World->m_ScriptCenter.IsFuncExist(pDoodad->m_dwScriptID, SCRIPT_ON_BREAK_OPEN))
			{
				// 先走脚本
				int nTopIndex = 0;
				g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);

				g_pSO3World->m_ScriptCenter.PushValueToStack(pDoodad);
				g_pSO3World->m_ScriptCenter.PushValueToStack((KPlayer*)this);

				g_pSO3World->m_ScriptCenter.CallFunction(pDoodad->m_dwScriptID, SCRIPT_ON_BREAK_OPEN, 0);

				g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);
			}
			KGLOG_PROCESS_ERROR(pDoodad->m_pRegion);
		}
#endif //_SERVER

#ifdef _CLIENT
        if (g_pGameWorldUIHandler)
        {
            g_pGameWorldUIHandler->OnOTActionProgressBreak(m_dwID);
        }
#endif
	    break;
	default:
		goto Exit0;
	}

#ifdef  _SERVER
	//处理Doodad打开的打断
	if (rNewAction.eActionType != otActionPicking)
	{
		pDoodad->ChangeState(dsIdle);
	}
#endif
	return true;
Exit0:
	return false;
}

BOOL KCharacter::OnBreakPicking(KOT_ACTION_PARAM& rNewAction)
{	
    BOOL     bResult = false;
	KDoodad* pDoodad = NULL;
	
	pDoodad = g_pSO3World->m_DoodadSet.GetObj(m_OTActionParam.Data.dwDoodadID);
	KGLOG_PROCESS_ERROR(pDoodad);
	KG_PROCESS_ERROR(pDoodad->m_pLootList);

	switch (rNewAction.eActionType)
	{
	case otActionIdle:
    case otActionPickPrepare:
		pDoodad->ChangeState(dsIdle);

#ifdef _SERVER
        pDoodad->Close();
#else
		if (g_pGameWorldRepresentHandler)
        {
            g_pGameWorldRepresentHandler->OnCharacterEndPickDoodad(this, pDoodad);
        }
        
        if (rNewAction.eActionType != otActionPickPrepare)
        {
            if (g_pGameWorldUIHandler)
            {
                if (m_dwID == g_pSO3World->m_dwClientPlayerID)
                {
                    KUIEventCloseDoodad param;
                    param.dwDoodadID = pDoodad->m_dwID;
                    param.dwPlayerID = m_dwID;
                    g_pGameWorldUIHandler->OnCloseDoodad(param);
                }
            }
        }
#endif    
        break;
	default:
		goto Exit0;
	}
	
	bResult = true;
Exit0:
	return bResult;
}

//<---------- 处理打断 --------------------

BOOL KCharacter::UpdateMoveParameter()
{
    BOOL bResult = false;

    if (m_eMoveState == cmsOnWalk)
    {
        if (m_nConvergenceSpeed != 0)
        {
            m_nVelocityXY       = m_nVelocityXY * GetWalkSpeed() / m_nConvergenceSpeed;
            m_nConvergenceSpeed = GetWalkSpeed();
        }
    }
    else if (m_eMoveState == cmsOnRun)
    {
        if (m_nConvergenceSpeed != 0)
        {
            m_nVelocityXY       = m_nVelocityXY * m_nRunSpeed / m_nConvergenceSpeed;
            m_nConvergenceSpeed = m_nRunSpeed;
        }
    }
    else if (m_eMoveState == cmsOnSwim)
    {
        if (m_nConvergenceSpeed != 0)
        {
            int nSwimSpeed = m_nRunSpeed / 2;
            m_nVelocityXY       = m_nVelocityXY * nSwimSpeed / m_nConvergenceSpeed;
            m_nConvergenceSpeed = nSwimSpeed;
        }
    }

    m_nCurrentGravity = m_nGravity;

    MAKE_IN_RANGE(m_nVelocityXY, 0, MAX_ZOOM_VELOCITY);
    MAKE_IN_RANGE(m_nConvergenceSpeed, 0, MAX_VELOCITY_XY);
    MAKE_IN_RANGE(m_nCurrentGravity, 0, MAX_GRAVITY);

#ifdef _SERVER
    if (IS_PLAYER(m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)this;

        pPlayer->m_nRecordCount = 0;

        g_PlayerServer.DoAdjustPlayerMove(pPlayer);
    }

    g_PlayerServer.DoSyncMoveParam(this);
#endif

    bResult = true;
Exit0:
    return bResult;
}

void KCharacter::InvalidateMoveState(int nDelayFrame, BOOL bMoveSyncSelf)
{
    int nRealSyncFrame = g_pSO3World->m_nGameLoop + nDelayFrame;

    if (m_nMoveSyncFrame == 0 || nRealSyncFrame < m_nMoveSyncFrame)
    {
        m_nMoveSyncFrame = nRealSyncFrame;
    }

    if (bMoveSyncSelf)
    {
        m_bMoveSyncSelf = true;
    }

#ifdef _SERVER
    if (IS_PLAYER(m_dwID))
    {
        ((KPlayer*)this)->m_nRecordCount = 0;
    }
#endif
}

void KCharacter::RecordPosition(KPOSITION_RECORD* pRecord)
{
    assert(pRecord);
    assert(m_pRegion);
    assert(m_pCell);

    pRecord->eMoveState           = m_eMoveState;
    pRecord->nX                   = m_nX;
    pRecord->nY                   = m_nY;
    pRecord->nZ                   = m_nZ;
    pRecord->nDestX               = m_nDestX;
    pRecord->nDestY               = m_nDestY;
    pRecord->nVelocityXY          = m_nVelocityXY;
    pRecord->nDirectionXY         = m_nDirectionXY;
    pRecord->nVelocityZ           = m_nVelocityZ;
    pRecord->nConvergenceSpeed    = m_nConvergenceSpeed;
    pRecord->nCurrentGravity      = m_nCurrentGravity;
    pRecord->nJumpCount           = m_nJumpCount;	
    pRecord->nFaceDirection       = m_nFaceDirection;
    pRecord->nDivingCount         = m_nDivingCount;  
    pRecord->bTryPathState        = m_bTryPathState;
    pRecord->nTryPathSide         = m_nTryPathSide;
    pRecord->bSlip                = m_bSlip;
    pRecord->nJumpLimitFrame      = m_nJumpLimitFrame;
    pRecord->nCurrentTrack        = m_nCurrentTrack;
    pRecord->nFromNode            = m_nFromNode;
    pRecord->nTargetCity          = m_nTargetCity;
    pRecord->nMoveFrameCount      = m_nMoveFrameCounter;
    pRecord->pRegion              = m_pRegion;
    pRecord->pCell                = m_pCell;
}

BOOL KCharacter::ReversePosition(KPOSITION_RECORD* pRecord)
{
    BOOL                bResult         = false;
    BOOL                bRetCode        = false;
    KRegion*            pDstRegion      = NULL;
    KCell*              pDstCell        = NULL;

    assert(pRecord);

    pDstRegion = pRecord->pRegion;

    if (pDstRegion != m_pRegion)
    {
        bRetCode = ChangeRegion(pDstRegion);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    m_nX     = pRecord->nX;
    m_nY     = pRecord->nY;
    m_nZ     = pRecord->nZ;

    m_nXCell = m_nX / CELL_LENGTH % REGION_GRID_WIDTH;
    m_nYCell = m_nY / CELL_LENGTH % REGION_GRID_HEIGHT;

    pDstCell = pRecord->pCell;

    if (pDstCell != m_pCell)
    {
        SetCell(pDstCell);
    }

    // 其实MoveTo可能导致自己被从场景移除
    KGLOG_PROCESS_ERROR(m_pScene);

    m_eMoveState          =   pRecord->eMoveState;  
    m_nDestX              =   pRecord->nDestX;
    m_nDestY              =   pRecord->nDestY;
    m_nVelocityXY         =   pRecord->nVelocityXY;
    m_nDirectionXY        =   pRecord->nDirectionXY;
    m_nVelocityZ          =   pRecord->nVelocityZ;
    m_nConvergenceSpeed   =   pRecord->nConvergenceSpeed;
    m_nCurrentGravity     =   pRecord->nCurrentGravity;
    m_nJumpCount          =	  pRecord->nJumpCount;   
    m_nFaceDirection      =   pRecord->nFaceDirection;   
    m_nDivingCount        =   pRecord->nDivingCount;    
    m_bTryPathState       =   pRecord->bTryPathState;
    m_nTryPathSide        =   pRecord->nTryPathSide;
    m_bSlip               =   pRecord->bSlip;
    m_nJumpLimitFrame     =   pRecord->nJumpLimitFrame;
    m_nCurrentTrack       =   pRecord->nCurrentTrack;
    m_nFromNode           =   pRecord->nFromNode;
    m_nTargetCity         =   pRecord->nTargetCity;
    m_nMoveFrameCounter   =   pRecord->nMoveFrameCount;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KCharacter::ForwardPosition(int nFrame)
{
    BOOL bResult = false;

    while (nFrame-- > 0)
    {
        RunStateMachine();
        KG_PROCESS_ERROR(m_pScene);

        ProcessMove();        
        KG_PROCESS_ERROR(m_pScene);

        if (IS_PLAYER(m_dwID))
        {
            ((KPlayer*)this)->MoveCtrl();
        }

#ifdef _SERVER
        if (IS_PLAYER(m_dwID))
        {
            ((KPlayer*)this)->m_nVirtualFrame++;
        }
#endif
    }

    bResult = true;
Exit0:
#ifdef _SERVER
    // 只要发生了快进,就不可能再次回滚,所以清楚掉历史记录
    if (IS_PLAYER(m_dwID))
    {
        ((KPlayer*)this)->m_nRecordCount = 0;
    }
#endif
    return bResult;
}

void KCharacter::ProcessMove()
{
    int nEarthPos = 0;

    if (m_eMoveState == cmsOnAutoFly)
    {
        ProcessAutoFly();
        goto Exit0;
    }

    if (m_pCell->m_BaseInfo.dwCellType == ctWater)
    {
        nEarthPos = m_pCell->m_wLowLayer * POINT_PER_ALTITUDE;
    }
    else
    {
        nEarthPos = m_pCell->m_wHighLayer * POINT_PER_ALTITUDE;
    }

    m_bSlip = CheckSlip();
    if ((!m_bSlip) && m_nConvergenceSpeed == 0)
    {
        m_nVelocityXY = 0;
    }

    if (m_bSlip)
    {
        if (m_pCell->m_BaseInfo.dwCellType != ctWater)
        {
            m_nJumpLimitFrame = g_pSO3World->m_nGameLoop + GAME_FPS * 2;
        }
        else if (m_nZ - m_pCell->m_wHighLayer * POINT_PER_ALTITUDE <= XYPOINT_TO_ZPOINT(CELL_LENGTH))
        {
            m_nJumpLimitFrame = 0;
        }
    }

    // ---------- 加速度计算 --------------------------
    
    if (
        m_eMoveState != cmsOnSwim && m_eMoveState != cmsOnFloat && 
        m_eMoveState != cmsOnDash && m_eMoveState != cmsOnPull
    )
    {
    	if (m_nZ - nEarthPos > 0)
        {
            // 悬空状态向下加速
            m_nVelocityZ -= m_nCurrentGravity;
            MAKE_IN_RANGE(m_nVelocityZ, MIN_VELOCITY_Z, MAX_VELOCITY_Z);
        }

        if (m_bSlip)
        {
            // 计算力的合成效果: 动摩擦力/重力分量
            // 注意,NPC是永远不会打滑的
            ProcessAcceleration();
        }
    }

    // 水平运动处理
    ProcessHorizontalMove();
    KG_PROCESS_ERROR(m_pScene);

    // 处理动态Cell引起的一些垂直变化
    ProcessDynamicObstacle();
    KG_PROCESS_ERROR(m_pScene);

    // 垂直运动处理
    ProcessVerticalMove();

Exit0:
    return;
}

enum AUTO_FLY_ERROR
{
    afeInvalid,

    afeSuccessed,
    afeFailed,
    afeError,

    afeTotal
};

void KCharacter::ProcessAutoFly()
{
    AUTO_FLY_ERROR  nResult         = afeInvalid;
    BOOL            bRetCode        = false;
    int             nFromNode       = 0;
    int             nCurrentNode    = 0;
    KTRACK*         pTrack          = NULL;
    KROUTE_NODE*    pCurrentNode    = NULL;
    KROUTE_NODE*    pToNode         = NULL;
    KROUTE_NODE*    pEndNode        = NULL;
    BOOL            bStop           = false;
    KPOSITION       TrackPoint;

    pTrack = g_pSO3World->m_RoadManager.GetTrackInfo(m_nCurrentTrack);
    KGLOG_PROCESS_ERROR_RET_CODE(pTrack, afeFailed);

    if (IS_PLAYER(m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)this;

        pEndNode = g_pSO3World->m_RoadManager.GetTargetNodeByCity(m_nTargetCity, pPlayer->m_eCamp);
        KGLOG_PROCESS_ERROR_RET_CODE(pEndNode, afeFailed);
    }
    else
    {
        pEndNode = g_pSO3World->m_RoadManager.GetTargetNodeByCity(m_nTargetCity, cNeutral);
        KGLOG_PROCESS_ERROR_RET_CODE(pEndNode, afeFailed);
    }

#ifdef _SERVER
    if (m_nMoveFrameCounter >= pTrack->nFrame)
    {
        nFromNode = m_nFromNode;

        KGLOG_PROCESS_ERROR_RET_CODE(nFromNode == pTrack->nFromNode || nFromNode == pTrack->nToNode, afeFailed);

        nCurrentNode = (pTrack->nFromNode == nFromNode) ? pTrack->nToNode : pTrack->nFromNode;

        m_nMoveFrameCounter = 0;
        // TODO: 调脚本

        pCurrentNode = g_pSO3World->m_RoadManager.GetNodeInfo(nCurrentNode);
        KGLOG_PROCESS_ERROR_RET_CODE(pCurrentNode, afeFailed);
        KGLOG_PROCESS_ERROR_RET_CODE(m_pScene, afeError);
        KGLOG_PROCESS_ERROR_RET_CODE(m_pScene->m_dwMapID == pCurrentNode->dwMapID, afeFailed);

        if (pCurrentNode == pEndNode)
        {
            m_nCurrentTrack = 0;
            Stop();
            
            m_BuffList.CleanBuffByPersist();

            MoveTo(pCurrentNode->nX, pCurrentNode->nY, pCurrentNode->nZ, true, false);
            goto Exit1;
        }

        if (IS_PLAYER(m_dwID)) // NPC没有开通交通的列表，没有势力和帮派的判断
        {
            KPlayer* pThis          = (KPlayer*)this;
            int      nNextTrackID   = 0;

            bRetCode = g_pSO3World->m_RoadManager.GetNextTrack(
                &(nNextTrackID),
                nFromNode, nCurrentNode, m_nTargetCity,
                pThis->m_eCamp, pThis->m_dwTongID,
                &(pThis->m_OpenRouteNodeList)
            );
            KG_PROCESS_ERROR_RET_CODE(nNextTrackID != 0, afeFailed);

            m_nCurrentTrack = nNextTrackID;
        }
        else
        {
            int      nNextTrackID   = 0;

            bRetCode = g_pSO3World->m_RoadManager.GetNextTrack(
                &(nNextTrackID),
                nFromNode, nCurrentNode, m_nTargetCity,
                cNeutral, 0,
                NULL
            );
            KG_PROCESS_ERROR_RET_CODE(nNextTrackID != 0, afeFailed);

            m_nCurrentTrack = nNextTrackID;
        }
        KG_PROCESS_ERROR_RET_CODE(bRetCode, afeFailed);

        m_nFromNode = nCurrentNode;

        pTrack = g_pSO3World->m_RoadManager.GetTrackInfo(m_nCurrentTrack);
        KG_PROCESS_ERROR_RET_CODE(bRetCode, afeFailed);

        pToNode = g_pSO3World->m_RoadManager.GetNodeInfo(pTrack->nToNode);
        if (m_nFromNode == pTrack->nToNode)
        {
            pToNode = g_pSO3World->m_RoadManager.GetNodeInfo(pTrack->nFromNode);
        }
        KG_PROCESS_ERROR_RET_CODE(pToNode, afeFailed);

        // 该死的，居然要切换地图！
        if (pCurrentNode->dwMapID != pToNode->dwMapID)
        {
            KPlayer* pThis = NULL;
            // 要保证切换地图后人物还会继续AutoFly，我们要在切换地图
            // 之前把人物的AutoFly参数都设置好。

            KG_PROCESS_ERROR_RET_CODE(IS_PLAYER(m_dwID), afeFailed);

            m_nMoveFrameCounter = 1;

            // 我们已经走了一帧了。规定所有的跨地图的Track都是1帧长
            // 度的。那么切换到目的地图后，会自动计算下一个路径点。
            pThis = (KPlayer*)this;

            // 在跨地图的时候,多同步一次,
            // 否则会导致客户端和服务器道路的不同步,Region出错.
            InvalidateMoveState(0, true);

            pThis->SwitchMap(pToNode->dwMapID, 0, pToNode->nX, pToNode->nY, pToNode->nZ);
            goto Exit1;
        }

        InvalidateMoveState(0, true);
    }
#endif

    KG_PROCESS_SUCCESS(m_nMoveFrameCounter >= pTrack->nFrame);

    bRetCode = g_pSO3World->m_RoadManager.GetPosition(
        &TrackPoint, m_nCurrentTrack, m_nMoveFrameCounter, m_nFromNode
    );
    if (!bRetCode)
    {
        m_nMoveFrameCounter++;
        goto Exit1;
    }


    bRetCode = MoveTo(TrackPoint.nX, TrackPoint.nY, TrackPoint.nZ, true, false);
    KG_PROCESS_ERROR_RET_CODE(bRetCode, afeFailed);

    if (m_nFaceDirection != TrackPoint.nDirection)
    {
        m_nFaceDirection = TrackPoint.nDirection;
        assert(m_nFaceDirection >= 0 && m_nFaceDirection < DIRECTION_COUNT);

#ifdef _CLIENT
        if (g_pGameWorldRepresentHandler)
        {
            g_pGameWorldRepresentHandler->OnCharacterTurnTo(this, true);
        }
#endif
    }

    m_nMoveFrameCounter++;
Exit1:

    nResult = afeSuccessed;
Exit0:
    if (nResult == afeFailed)
    {
        m_nMoveFrameCounter = 0;
        m_nCurrentTrack     = 0;

        Stop();
    }
    return;
}

int  KCharacter::GetFrictionDirection()
{
    int nFrictionDirection      = 0;
    int nMyV                    = 0;
    int nMyVx                   = 0;
    int nMyVy                   = 0;
    int nBeta                   = 0;
    int nSinBeta                = 0;
    int nCosBeta                = 0;
    int nCosThetaAlpha          = 0;
    int nMyVBase                = 0;
    int nTheta                  = 0;
    int nAlpha                  = 0;
    int nVx                     = 0;
    int nVy                     = 0;
    int nMoveFrictionDirection  = m_nDirectionXY + DIRECTION_COUNT / 2;

    nAlpha = m_pCell->GetGradientDirection();
    nBeta  = m_pCell->GetGradientDegree();

    switch (m_eMoveState)
    {
    case cmsOnWalk:
    case cmsOnRun:
        nMyV = m_nConvergenceSpeed;
        break;

    default:
        if (m_nVelocityXY > 0)
        {
            nFrictionDirection = nMoveFrictionDirection;
        }
        else
        {
            nFrictionDirection = nAlpha + DIRECTION_COUNT / 2;
        }
        goto Exit0;
    }

    if (m_nX == m_nDestX && m_nY == m_nDestY)
    {
        nFrictionDirection = nMoveFrictionDirection;
        goto Exit0;
    }

    nTheta = g_GetDirection(m_nX, m_nY, m_nDestX, m_nDestY);

    nCosBeta        = g_Cos(nBeta);
    nSinBeta        = g_Sin(nSinBeta);
    nCosThetaAlpha  = g_Cos(nTheta - nAlpha);

    // 注意,我们认为人能够跑多块,这个跑动能力只的是沿着坡面的"线速度",所以,要对nMyV进行斜坡分解,然后再与当前的
    // 瞬时速度进行合成,当然要注意,我们要计算的是摩擦力的方向,所以,瞬时速度的方向要取其反方向来进行计算

    nMyVBase = (int)sqrt(
        (float)SIN_COS_NUMBER * SIN_COS_NUMBER + (float)nCosThetaAlpha * nCosThetaAlpha / nCosBeta * nSinBeta / nCosBeta * nSinBeta
    );

    // 注意,"瞬时速度"是真实速度乘上了一个系数"VELOCITY_ZOOM_COEFFICIENT"的,所以要做速度合成,这里也要乘系数
    // 注意防止数值溢出
    nMyVx = VELOCITY_ZOOM_COEFFICIENT * nMyV * g_Cos(nTheta) / nMyVBase;
    nMyVy = VELOCITY_ZOOM_COEFFICIENT * nMyV * g_Sin(nTheta) / nMyVBase;

    nVx = m_nVelocityXY * g_Cos(nMoveFrictionDirection) / SIN_COS_NUMBER + nMyVx;
    nVy = m_nVelocityXY * g_Sin(nMoveFrictionDirection) / SIN_COS_NUMBER + nMyVy;

    nFrictionDirection = g_GetDirection(0, 0, nVx, nVy);
Exit0:
    return nFrictionDirection;
}


BOOL KCharacter::ProcessAcceleration()
{
    BOOL    bResult         = false;
    int     nTheta          = 0; 
    int     nFValue         = 0;
    int     nAlpha          = 0;
    int     nBeta           = 0;
    int     nBaseValue      = 0;
    int     nAFx            = 0;
    int     nAFy            = 0;
    int     nAFz            = 0;
    int     nGValue         = 0;
    int     nGxy            = 0;
    int     nAGx            = 0;
    int     nAGy            = 0;
    int     nAGz            = 0;
    int     nCosBeta        = 0;
    int     nCosThetaAlpha  = 0;
    int     nSinBeta        = 0;
    int     nVx             = 0;
    int     nVy             = 0;
    int     nWaterline      = GetWaterline();
    int     nFlotageZ       = 0;

    nAlpha = m_pCell->GetGradientDirection();
    nBeta  = m_pCell->GetGradientDegree();
    nTheta = GetFrictionDirection(); // 取摩擦力在水平面上投影的方向

    // 计算滑动摩擦力的大小(这里实际上是滑动摩擦力引起的加速度的大小)
    if (m_pCell->m_BaseInfo.dwCellType != ctWater)
    {
        nFValue = m_nCurrentGravity * g_Cos(nBeta) * m_pCell->GetDynamicFrictionCoefficient() / 
            SIN_COS_NUMBER / KILO_NUM;
    }
    else
    {
        // 水Cell要取其下面的地表的动摩擦系数
        KCell* pPreCell = m_pRegion->GetPreCell(m_nXCell, m_nYCell, m_pCell);
        KGLOG_PROCESS_ERROR(pPreCell);
        nFValue = m_nCurrentGravity * g_Cos(nBeta) * pPreCell->GetDynamicFrictionCoefficient() / 
            SIN_COS_NUMBER / KILO_NUM;
    }

    assert(nBeta >= 0 && nBeta < DIRECTION_COUNT / 4);

    if (IS_UNDER_WATER(nWaterline, m_nHeight))
    {
        nFlotageZ = -m_nCurrentGravity * FLOTAGE_COEFFICIENT / KILO_NUM;
    }

    // ----- 计算摩擦力在xyz方向上的分解引起的加速度分量 -----------
    nCosBeta = g_Cos(nBeta);
    nCosThetaAlpha = g_Cos(nTheta - nAlpha);
    nSinBeta = g_Sin(nBeta);

    nBaseValue = (int)sqrt(
        (float)SIN_COS_NUMBER * SIN_COS_NUMBER + (float)nCosThetaAlpha * nCosThetaAlpha / nCosBeta * nSinBeta / nCosBeta * nSinBeta
    );

    assert(nBaseValue > 0);

    // nFValue是由重力折算过来的,这里要换算为水平坐标单位,注意,这里要防止数值溢出
    nAFx = ZPOINT_TO_XYPOINT(VELOCITY_ZOOM_COEFFICIENT * nFValue) * g_Cos(nTheta) / nBaseValue;
    nAFy = ZPOINT_TO_XYPOINT(VELOCITY_ZOOM_COEFFICIENT * nFValue) * g_Sin(nTheta) / nBaseValue;

    if (nCosThetaAlpha < 0)
    {
        // 摩擦力朝上坡方向
        nAFz = -nFValue * nCosThetaAlpha * nSinBeta / nCosBeta / nBaseValue;
    }
    else
    {
        nAFz = nFValue * nCosThetaAlpha * nSinBeta / nCosBeta / nBaseValue;
    }

    nGValue = m_nCurrentGravity * nSinBeta / SIN_COS_NUMBER;
    nGxy = ZPOINT_TO_XYPOINT(VELOCITY_ZOOM_COEFFICIENT * nGValue * nCosBeta) / SIN_COS_NUMBER;
    nAGx = nGxy * g_Cos(nAlpha) / SIN_COS_NUMBER;
    nAGy = nGxy * g_Sin(nAlpha) / SIN_COS_NUMBER;
    nAGz = -nGValue * nSinBeta / SIN_COS_NUMBER;

    nVx = m_nVelocityXY * g_Cos(m_nDirectionXY) / SIN_COS_NUMBER;
    nVy = m_nVelocityXY * g_Sin(m_nDirectionXY) / SIN_COS_NUMBER;

    nVx += (nAFx + nAGx);
    nVy += (nAFy + nAGy);

    m_nVelocityXY = (int)sqrt((float)(nVx * nVx + nVy * nVy));
    m_nDirectionXY = g_GetDirection(0, 0, nVx, nVy);

    m_nVelocityZ += (nAFz + nAGz + nFlotageZ);

    MAKE_IN_RANGE(m_nVelocityXY, 0, MAX_ZOOM_VELOCITY);
    MAKE_IN_RANGE(m_nVelocityZ, MIN_VELOCITY_Z, MAX_VELOCITY_Z);

    bResult = true;
Exit0:
    return bResult;
}

void KCharacter::ProcessHorizontalMove()
{
    BOOL bIsNpc      = IS_NPC(m_dwID);
    BOOL bSmartMove  = (m_eMoveState == cmsOnRun) || (m_eMoveState == cmsOnWalk) || (m_eMoveState == cmsOnSwim);

    if (bIsNpc && bSmartMove)
    {
        ProcessHorizontalMoveSmart();
    }
    else
    {
        ProcessHorizontalMoveStupid();
    }
}

void KCharacter::ProcessHorizontalMoveStupid()
{
    int             nRetCode        = false;
    int             nMoveDistance   = 0;

    KG_PROCESS_ERROR(m_nVelocityXY);

    if (m_bSlip && m_nVelocityXY < VELOCITY_ZOOM_COEFFICIENT * 8)
        m_nVelocityXY = VELOCITY_ZOOM_COEFFICIENT * 8;

    nMoveDistance = m_nVelocityXY / VELOCITY_ZOOM_COEFFICIENT;

    if (m_eMoveState == cmsOnWalk || m_eMoveState == cmsOnRun || m_eMoveState == cmsOnDash || m_eMoveState == cmsOnPull)
    {
        if (!m_bSlip)
        {
            int nGradient = m_pCell->GetGradientDegree();
            nMoveDistance = nMoveDistance * g_Cos(nGradient) / SIN_COS_NUMBER;
        }
    }

    while (nMoveDistance >= CELL_LENGTH)
    {
        CellMoveStupid(CELL_LENGTH);
        KG_PROCESS_ERROR(m_pScene);
        nMoveDistance -= CELL_LENGTH;
    }

    if (nMoveDistance > 0)
    {
        CellMoveStupid(nMoveDistance);
        KG_PROCESS_ERROR(m_pScene);
        nMoveDistance = 0;
    }

Exit0:
	return;
}


void KCharacter::ProcessHorizontalMoveSmart()
{
    int             nRetCode        = false;
    int             nMoveDistance   = 0;

    KG_PROCESS_ERROR(m_nVelocityXY);

    nMoveDistance = m_nVelocityXY / VELOCITY_ZOOM_COEFFICIENT;

    if (m_eMoveState == cmsOnWalk || m_eMoveState == cmsOnRun || m_eMoveState == cmsOnDash || m_eMoveState == cmsOnPull)
    {
        if (!m_bSlip)
        {
            int nGradient = m_pCell->GetGradientDegree();
            nMoveDistance = nMoveDistance * g_Cos(nGradient) / SIN_COS_NUMBER;
        }
    }

    while (nMoveDistance >= CELL_LENGTH)
    {
        CellMoveSmart(CELL_LENGTH);
        KG_PROCESS_ERROR(m_pScene);
        nMoveDistance -= CELL_LENGTH;
    }

    if (nMoveDistance > 0)
    {
        CellMoveSmart(nMoveDistance);
        KG_PROCESS_ERROR(m_pScene);
        nMoveDistance = 0;
    }

Exit0:
	return;
}


BOOL KCharacter::CellMoveStupid(int nStepLength)
{
    BOOL bResult            = false;
    int  nRetCode           = false;
    int  nObstacleNormal    = -1;
    int  nObstacleDirection = -1;
    int  nMoveRetCode       = mrInvalid;
    int  nTrySpeed          = 0;   
    int  nTryDirection      = 0;
    int  nDestDirection     = 0;


    // 只有这些状态需要检测"到达目标",跳跃状态是检测"落地"切换状态的,不在这里进行
    // 其他状态下,比如站立状态下因为打滑而引起的移动,则无所谓"到达目标"
    if (
        m_eMoveState == cmsOnWalk || m_eMoveState == cmsOnRun || m_eMoveState == cmsOnSwim
    )
    {
        int nDistance = g_GetDistance2(m_nX, m_nY, m_nDestX, m_nDestY);

        if (nDistance <= CELL_LENGTH * CELL_LENGTH / 2)
        {
            m_nVelocityXY       = 0;
            m_nConvergenceSpeed = 0;

            if (m_eMoveState == cmsOnSwim)
            {
                SetMoveState(cmsOnFloat);
            }
            else
            {
                SetMoveState(cmsOnStand);
            }

            nMoveRetCode        = mrReachTarget;
            goto Exit1;
        }
    }

    nMoveRetCode = TryMove(nStepLength, m_nDirectionXY, &nObstacleNormal);
    KG_PROCESS_SUCCESS(nMoveRetCode == mrSucceed);

    KG_PROCESS_ERROR(nMoveRetCode == mrObstacle);

    // 这些状态下在需要寻路
    KG_PROCESS_ERROR(
        m_eMoveState == cmsOnWalk || m_eMoveState == cmsOnRun ||
        m_eMoveState == cmsOnSwim || m_eMoveState == cmsOnDash ||
        m_eMoveState == cmsOnPull
    );
    
    // 必须在能够取得障碍法线方向的时候才能寻路
    KG_PROCESS_ERROR(nObstacleNormal != -1);

    assert(nObstacleNormal >= 0 && nObstacleNormal < DIRECTION_COUNT);
    assert(m_nDirectionXY >= 0 && m_nDirectionXY < DIRECTION_COUNT);

    if (nObstacleNormal - m_nDirectionXY > DIRECTION_COUNT / 4)
    {
        nObstacleNormal -= DIRECTION_COUNT / 2;
    }
    else if (nObstacleNormal - m_nDirectionXY < -DIRECTION_COUNT / 4)
    {
        nObstacleNormal += DIRECTION_COUNT / 2;
    }


    // 遇到障碍时自动绕路,注意,对玩家而言,是根据前方障碍方向判断应该往那个方向绕路,而对NPC而言,只是根据障碍方向
    // 判断应该优先往哪个方向绕路

    // 法线方向加90度,也就是障碍的切线方向
    nRetCode = g_Sin(nObstacleNormal - m_nDirectionXY);    
    if (nRetCode >= 0)
    {
        nTryDirection   = nObstacleNormal - DIRECTION_COUNT / 4;
        nTrySpeed       = nStepLength * g_Cos(m_nDirectionXY - nTryDirection) / SIN_COS_NUMBER;

        assert(nTrySpeed >= 0);

        if (nTrySpeed < 1)
        {
            nTrySpeed = 1;
        }

        for (int i = 0; i < DIRECTION_COUNT / 4; i += DIRECTION_COUNT / 16)
        {
            nMoveRetCode = TryMove(nTrySpeed, nTryDirection - i, NULL);
            if (nMoveRetCode == mrSucceed)
            {
                break;
            }
            KG_PROCESS_ERROR(nMoveRetCode == mrObstacle);
        }
    }
    else
    {
        nTryDirection = nObstacleNormal + DIRECTION_COUNT / 4;
        nTrySpeed     = nStepLength * g_Cos(m_nDirectionXY - nTryDirection) / SIN_COS_NUMBER;

        assert(nTrySpeed >= 0);

        if (nTrySpeed < 1)
        {
            nTrySpeed = 1;
        }

        for (int i = 0; i < DIRECTION_COUNT / 4; i += DIRECTION_COUNT / 16)
        {
            nMoveRetCode = TryMove(nTrySpeed, nTryDirection + i, NULL);
            if (nMoveRetCode == mrSucceed)
            {
                break;
            }
            KG_PROCESS_ERROR(nMoveRetCode == mrObstacle);
        }
    }

    KG_PROCESS_ERROR(nMoveRetCode == mrSucceed);

Exit1:
    bResult = true;
Exit0:

    switch (nMoveRetCode)
    {
    case mrSucceed:
        break;

    case mrObstacle:
        m_nVelocityXY = 0;
        m_nConvergenceSpeed = 0;

        if (m_eMoveState == cmsOnWalk || m_eMoveState == cmsOnRun || m_eMoveState == cmsOnDash)
        {
            // 这些状态,在遇到障碍时即转为站立
            // 注意,跳跃状态是不要切为站立,否则会导致"攀悬崖"问题
            SetMoveState(cmsOnStand);
        }
        else if (m_eMoveState == cmsOnSwim)
        {
            SetMoveState(cmsOnFloat);
        }
        break;

    case mrDeleteSelf:
        break;

    case mrOutOfMap:
        if (
            m_eMoveState == cmsOnWalk || m_eMoveState == cmsOnRun || m_eMoveState == cmsOnJump ||
            m_eMoveState == cmsOnDash
        )
        {
            // 这些状态,在遇到障碍时即转为站立
            SetMoveState(cmsOnStand);
        }
        else if (m_eMoveState == cmsOnSwim)
        {
            SetMoveState(cmsOnFloat);
        }
        m_nConvergenceSpeed = 0;
        break;

    case mrReachTarget:
        break;

    default:
        break;
    }

    return bResult;
}

BOOL KCharacter::CellMoveSmart(int nStepLength)
{
    BOOL bResult            = false;
    int  nRetCode           = false;
    int  nObstacleNormal    = -1;
    int  nObstacleDirection = -1;
    int  nMoveRetCode       = mrInvalid;
    int  nTrySpeed          = 0;   
    int  nTryDirection      = 0;
    int  nDestDirection     = 0;


    // 只有这些状态需要检测"到达目标",跳跃状态是检测"落地"切换状态的,不在这里进行
    // 其他状态下,比如站立状态下因为打滑而引起的移动,则无所谓"到达目标"
    if (
        m_eMoveState == cmsOnWalk || m_eMoveState == cmsOnRun || m_eMoveState == cmsOnSwim ||
        m_eMoveState == cmsOnKnockedBack || m_eMoveState == cmsOnKnockedOff
    )
    {
        int nDistance = g_GetDistance2(m_nX, m_nY, m_nDestX, m_nDestY);

        if (nDistance <= CELL_LENGTH * CELL_LENGTH / 2)
        {
            m_nVelocityXY       = 0;
            m_nConvergenceSpeed = 0;

            if (m_eMoveState == cmsOnSwim)
            {
                SetMoveState(cmsOnFloat);
            }
            else
            {
                SetMoveState(cmsOnStand);
            }

            nMoveRetCode = mrReachTarget;
            goto Exit1;
        }
    }

    nDestDirection = g_GetDirection(m_nX, m_nY, m_nDestX, m_nDestY);

    if (!m_bTryPathState)
    {
        int nDelta = 0;

        nMoveRetCode = TryMove(nStepLength, nDestDirection, NULL);
        if (nMoveRetCode == mrSucceed)
        {
            m_nDirectionXY = nDestDirection;
            goto Exit1;
        }
        KG_PROCESS_ERROR(nMoveRetCode == mrObstacle);

        // 判断绕路方向
        for (nDelta = DIRECTION_COUNT / 16; nDelta < DIRECTION_COUNT / 2; nDelta += DIRECTION_COUNT / 16)
        {
            nTryDirection = nDestDirection - nDelta;

            nMoveRetCode = TryMove(nStepLength, nTryDirection, NULL);
            if (nMoveRetCode == mrSucceed)
            {
                m_bTryPathState = true;
                m_nTryPathSide  = 0;
                m_nDirectionXY = nTryDirection;
                break;
            }
            KG_PROCESS_ERROR(nMoveRetCode == mrObstacle);

            nTryDirection = nDestDirection + nDelta;

            nMoveRetCode = TryMove(nStepLength, nTryDirection, NULL);
            if (nMoveRetCode == mrSucceed)
            {
                m_bTryPathState = true;
                m_nTryPathSide  = 1;
                m_nDirectionXY = nTryDirection;
                break;
            }
            KG_PROCESS_ERROR(nMoveRetCode == mrObstacle);
        }

        KG_PROCESS_ERROR(nDelta < DIRECTION_COUNT / 2);

        goto Exit1;
    }

    // 在绕路状态:
    nMoveRetCode = TryMove(nStepLength, nDestDirection, NULL);
    if (nMoveRetCode == mrSucceed)
    {
        m_nDirectionXY  = nDestDirection;
        m_bTryPathState = false;
        goto Exit1;
    }
    KG_PROCESS_ERROR(nMoveRetCode == mrObstacle);

    if (m_nTryPathSide == 0)
    {
        int nDelta = 0;

        // 尝试继续往左边绕路
        for (nDelta = DIRECTION_COUNT / 16; nDelta < DIRECTION_COUNT / 2; nDelta += DIRECTION_COUNT / 16)
        {
            nTryDirection = nDestDirection - nDelta;

            nMoveRetCode = TryMove(nStepLength, nTryDirection, NULL);
            if (nMoveRetCode == mrSucceed)
            {
                m_nDirectionXY = nTryDirection;
                break;
            }
            KG_PROCESS_ERROR(nMoveRetCode == mrObstacle);
        }

        KG_PROCESS_ERROR(nDelta < DIRECTION_COUNT / 2);
    }
    else
    {
        int nDelta = 0;

        // 尝试继续往右边绕路
        for (nDelta = DIRECTION_COUNT / 16; nDelta < DIRECTION_COUNT / 2; nDelta += DIRECTION_COUNT / 16)
        {
            nTryDirection = nDestDirection + nDelta;

            nMoveRetCode = TryMove(nStepLength, nTryDirection, NULL);
            if (nMoveRetCode == mrSucceed)
            {
                m_nDirectionXY = nTryDirection;
                break;
            }
            KG_PROCESS_ERROR(nMoveRetCode == mrObstacle);
        }

        KG_PROCESS_ERROR(nDelta < DIRECTION_COUNT / 2);
    }

    KG_PROCESS_ERROR(nMoveRetCode == mrSucceed);

Exit1:
    bResult = true;
Exit0:

    m_nDirectionXY = m_nDirectionXY % DIRECTION_COUNT;
    if (m_nDirectionXY < 0)
    {
        m_nDirectionXY += DIRECTION_COUNT;
    }
    assert(m_nDirectionXY >= 0 && m_nDirectionXY < DIRECTION_COUNT);

    switch (nMoveRetCode)
    {
    case mrSucceed:
        break;

    case mrObstacle:
        m_nVelocityXY = 0;
        m_nConvergenceSpeed = 0;

        if (m_eMoveState == cmsOnWalk || m_eMoveState == cmsOnRun || m_eMoveState == cmsOnDash)
        {
            // 这些状态,在遇到障碍时即转为站立
            SetMoveState(cmsOnStand);
        }
        else if (m_eMoveState == cmsOnSwim)
        {
            SetMoveState(cmsOnFloat);
        }

        m_AIVM.FireEvent(aevOnPathFailed, m_dwID, 0);
        break;

    case mrDeleteSelf:
        break;

    case mrOutOfMap:
        if (
            m_eMoveState == cmsOnWalk || m_eMoveState == cmsOnRun || m_eMoveState == cmsOnJump ||
            m_eMoveState == cmsOnDash
        )
        {
            // 这些状态,在遇到障碍时即转为站立
            SetMoveState(cmsOnStand);
        }
        else if (m_eMoveState == cmsOnSwim)
        {
            SetMoveState(cmsOnFloat);
        }
        m_nConvergenceSpeed = 0;
        break;

    case mrReachTarget:
        m_AIVM.FireEvent(aevOnPathSuccess, m_dwID, 0);
        break;

    default:
        break;
    }

    return bResult;
}

int KCharacter::TryMove(int nStepLength, int nDirection, int* pnObstacleDirection)
{
    int             nResult                 = mrInvalid;
    int             nRetCode                = false;
    int             nMargin                 = 0;
    int             nDestX                  = 0;
    int             nDestY                  = 0;
    int             nDestCellX              = 0;
    int             nDestCellY              = 0;
    KCell*          pDestCell               = NULL;
    int             nDestRegionX            = 0;
    int             nDestRegionY            = 0;
    KRegion*        pDestRegion             = NULL;
    int             nRelativeZ              = 0;
    int             nDeltaX                 = 0;
    int             nDeltaY                 = 0;
    int             nClimbAbility           = 0;
    BOOL            bIsChase                = false;
    BOOL            bIgnoreDynamicObstacle  = false;

    assert(m_pCell);
    assert(m_pRegion);
    assert(m_pScene);

    nDeltaX         = nStepLength * g_Cos(nDirection) / SIN_COS_NUMBER;
    nDeltaY         = nStepLength * g_Sin(nDirection) / SIN_COS_NUMBER;

    if (!nDeltaX && !nDeltaY)
    {
        m_bSlip = false;
        goto Exit1;
    }

    nDestX          = m_nX + nDeltaX;
    nDestY          = m_nY + nDeltaY;

    KG_PROCESS_ERROR_RET_CODE(nDestX >= 0, mrOutOfMap);
    KG_PROCESS_ERROR_RET_CODE(nDestY >= 0, mrOutOfMap);

    KG_PROCESS_ERROR_RET_CODE(nDestX < m_pScene->m_nRegionWidth * REGION_GRID_WIDTH * CELL_LENGTH, mrOutOfMap);
    KG_PROCESS_ERROR_RET_CODE(nDestY < m_pScene->m_nRegionHeight * REGION_GRID_HEIGHT * CELL_LENGTH, mrOutOfMap);

    nDestCellX      = nDestX / CELL_LENGTH % REGION_GRID_WIDTH;
    nDestCellY      = nDestY / CELL_LENGTH % REGION_GRID_HEIGHT;

    nDestRegionX    = nDestX / CELL_LENGTH / REGION_GRID_WIDTH;
    nDestRegionY    = nDestY / CELL_LENGTH / REGION_GRID_HEIGHT;

    pDestRegion = m_pScene->GetRegion(nDestRegionX, nDestRegionY);
    if (!pDestRegion)
    {
#ifdef _CLIENT
        KGLOG_PROCESS_ERROR(m_dwID != g_pSO3World->m_dwClientPlayerID);

		if (IS_PLAYER(m_dwID))
		{
			nRetCode = g_pSO3World->DelPlayer((KPlayer*)this);
			KGLOG_PROCESS_ERROR(nRetCode);
		}
		else
		{
			nRetCode = g_pSO3World->DeleteNpc((KNpc*)this);
			KGLOG_PROCESS_ERROR(nRetCode);
		}
        nResult = mrDeleteSelf;
#endif
        goto Exit0;
    }

    nClimbAbility = CLIMB_ABILITY;
    
    bIsChase =  m_eMoveState == cmsOnRun || m_eMoveState == cmsOnWalk || m_eMoveState == cmsOnDash || m_eMoveState == cmsOnPull;
    if (m_bFightState && IS_NPC(m_dwID) && bIsChase)
    {
        nClimbAbility = INT_MAX / 2; // 防止运算时溢出

        if (m_pScene->m_bDoNotGoThroughRoof)    // 室内副本限制NPC的爬坡能力
            nClimbAbility = 10240;  // 20m
    }

    bIgnoreDynamicObstacle = IgnoreDynamicObstacle();

	pDestCell = pDestRegion->GetAdaptedCellForObj(
        nDestCellX, nDestCellY, m_nZ, nClimbAbility, 
        m_nHeight, bIgnoreDynamicObstacle,
        &nRelativeZ, pnObstacleDirection
    );
    KG_PROCESS_ERROR_RET_CODE(pDestCell, mrObstacle);

    if (nRelativeZ > 0 && nRelativeZ < CRITICAL_JUMP_HEIGHT)
    {
        if (m_eMoveState == cmsOnStand || m_eMoveState == cmsOnWalk || m_eMoveState == cmsOnRun)
        {
            // 注意,这个赋0操作会影响到打滑效果
            // 由于悬空是不打滑的,如果这里不赋0,则会造成斜坡上移动时打滑状态不连续
            nRelativeZ = 0;
        }
    }

	if (IS_NPC(m_dwID))
	{
		KNpc*       pNpc           = (KNpc*)this;
		int         nHeighMargin   = 0;

		if (pNpc->m_eMoveMode == mmGround && pDestCell->m_BaseInfo.dwCellType == ctWater)
		{
            int nWaterDeepth = (pDestCell->m_wHighLayer - pDestCell->m_wLowLayer) * POINT_PER_ALTITUDE;

            if (!(m_bFightState && bIsChase))
            {
                // 防止下水
                KG_PROCESS_ERROR_RET_CODE(nRelativeZ > -m_nHeight / 2, mrObstacle);
                KG_PROCESS_ERROR_RET_CODE(nWaterDeepth < m_nHeight / 2, mrObstacle);
            }
		}
		else if (pNpc->m_eMoveMode == mmWater)
		{
		    KG_PROCESS_ERROR_RET_CODE(nRelativeZ < -m_nHeight / 2, mrObstacle);
		}

		// 防止下悬崖
//         if (m_eMoveState != cmsOnJump)
//         {
//             int nCriticalHeight = min(CRITICAL_JUMP_HEIGHT, nClimbAbility);
//             nHeighMargin = POINT_PER_ALTITUDE * ((int)m_pCell->m_wHighLayer - (int)pDestCell->m_wHighLayer);
//             KG_PROCESS_ERROR_RET_CODE(nHeighMargin < nCriticalHeight, mrObstacle);
//         }
	}

    // 检测飞出房顶的情况
    if (m_pScene->m_bDoNotGoThroughRoof)
    {
        for (KCell* pCellNode = m_pRegion->GetNextCell(m_pCell, bIgnoreDynamicObstacle); pCellNode != NULL; pCellNode = m_pRegion->GetNextCell(pCellNode, bIgnoreDynamicObstacle))
        {
            if (pCellNode->m_BaseInfo.dwCellType != ctWater)
            {
                KG_PROCESS_ERROR_RET_CODE(pCellNode->m_wLowLayer * POINT_PER_ALTITUDE >= pDestCell->m_wHighLayer * POINT_PER_ALTITUDE + nRelativeZ + m_nHeight / 2, mrObstacle);
                KG_PROCESS_ERROR_RET_CODE((pCellNode->m_wLowLayer - m_pCell->m_wLowLayer) * POINT_PER_ALTITUDE >= m_nHeight, mrObstacle);
                break;
            }
        }
    }

	m_nX = nDestX;
	m_nY = nDestY;
    m_nZ = pDestCell->m_wHighLayer * POINT_PER_ALTITUDE + nRelativeZ;

	m_nXCell = nDestCellX;
	m_nYCell = nDestCellY;

	if (pDestRegion != m_pRegion)
	{
		nRetCode = ChangeRegion(pDestRegion);
		KGLOG_PROCESS_ERROR(nRetCode);
	}

    if (pDestCell != m_pCell)
    {
        SetCell(pDestCell);
    }

Exit1:
    nResult = mrSucceed;
Exit0:        
    return nResult;
}

BOOL KCharacter::IgnoreDynamicObstacle()
{
    BOOL   bResult   = false;
    KCell* pNextCell = NULL;

    assert(m_pCell);

    pNextCell = m_pCell->m_pNext;
    if (pNextCell && pNextCell->m_BaseInfo.dwDynamic)
    {
        KDynamicCell* pDynamicCell = (KDynamicCell*)pNextCell;
        BOOL          bCellEnable  = false;

        bCellEnable = m_pScene->GetDynamicObstacleState(pDynamicCell->m_wGroupIndex);
        if (bCellEnable)
        {
            int nNextLowerLayer = pNextCell->m_wLowLayer * POINT_PER_ALTITUDE;
            bResult = (nNextLowerLayer < m_nZ + m_nHeight);
        }
    }

     return bResult;
}

void KCharacter::ProcessDynamicObstacle()
{
    BOOL      bRetCode                = false;
    KCell*    pDestCell               = NULL;
    int       nRelativeZ              = 0;
    int       nObstacleDirection      = 0;

    assert(m_pCell);

    // 这里爬坡能力不考虑NPC/Player区别,因为这里本来就不是真的所谓"爬坡"
	pDestCell = m_pRegion->GetAdaptedCellForObj(
        m_nXCell, m_nYCell, m_nZ, CLIMB_ABILITY, 
        m_nHeight, false,
        &nRelativeZ, &nObstacleDirection
    );
    if (pDestCell && pDestCell != m_pCell)
    {
        m_nZ = pDestCell->m_wHighLayer * POINT_PER_ALTITUDE + nRelativeZ;

        SetCell(pDestCell);
    }
}

void KCharacter::ProcessVerticalMove()
{
    KCell*    pCellNode               = NULL;
    KCell*    pNextCell               = NULL;
    int       nWaterline              = 0;
    int       nEarthPos               = 0;
    int       nGradient               = 0;
    int       nSwimWaterline          = GET_SWIM_WATER_LINE(m_nHeight);
    BOOL      bIgnoreDynamicObstacle  = false;

    assert(m_pCell);

    // 在改变Z坐标之前先取得动态障碍状态
    bIgnoreDynamicObstacle = IgnoreDynamicObstacle();

    nGradient = m_pCell->GetGradientDegree();

    // 取得地表高度
    if (m_pCell->m_BaseInfo.dwCellType != ctWater)
        nEarthPos = m_pCell->m_wHighLayer * POINT_PER_ALTITUDE;
    else
        nEarthPos = m_pCell->m_wLowLayer * POINT_PER_ALTITUDE;

    if (m_eMoveState == cmsOnSwim && m_nVelocityZ > 0)
    {
        int nWaterPos = m_pCell->m_wHighLayer * POINT_PER_ALTITUDE;

        m_nZ += m_nVelocityZ;

        if (m_nZ > nWaterPos - nSwimWaterline)
        {
            m_nZ = nWaterPos - nSwimWaterline;
        }
    }
    else
    {
        if (m_nZ + m_nVelocityZ <= nEarthPos)
        {
            if (m_nVelocityZ < 0)
            {
                // 落地摔伤计算
                if (IS_PLAYER(m_dwID))
                    ProcessDropDamage();

                // 落地速度处理
                ProcessDropSpeed();
            }

            // 落地状态切换
            if (m_eMoveState == cmsOnJump)
            {
                if (!m_bSlip)
                {
                    m_nVelocityXY = 0;
                }
                m_nConvergenceSpeed = 0;
                SetMoveState(cmsOnStand);
            }
            
            m_nZ = nEarthPos;
        }
        else if (
            IS_NPC(m_dwID) && (m_eMoveState == cmsOnRun || m_eMoveState == cmsOnWalk || 
            m_eMoveState == cmsOnDash || m_eMoveState == cmsOnPull || m_eMoveState == cmsOnStand)
        )
        {
            m_nZ = nEarthPos;
            m_nVelocityZ = 0;
        }
        else
        {
            m_nZ += m_nVelocityZ;
        }
    }

	// --碰顶处理--
    pCellNode = m_pRegion->GetNextCell(m_pCell, bIgnoreDynamicObstacle);
    while (pCellNode)
    {
        if (pCellNode->m_BaseInfo.dwCellType != ctWater)
        {
            pNextCell = pCellNode;
            break;
        }
        pCellNode = m_pRegion->GetNextCell(pCellNode, bIgnoreDynamicObstacle);
    }

	if (pNextCell)
	{
		int nMaxZ   = pNextCell->m_wLowLayer * POINT_PER_ALTITUDE;
        int nMyTop  = m_nZ + m_nHeight;

		if (nMyTop > nMaxZ)
		{
			m_nZ = nMaxZ - m_nHeight;
			m_nVelocityZ = 0;
		}
	}

    // --触地处理--
    // 注意,一定要先处理碰顶,再处理触地,否则上面减去m_nHeight可能陷入地下
    if (m_nZ < nEarthPos)
    {
        m_nZ = nEarthPos;
    }

    if (IS_PLAYER(m_dwID))
    {
        ProcessDrowning();
    }

    // 取得吃水深度
    nWaterline = GetWaterline();
    if (IS_NPC(m_dwID))
        nWaterline = 0;

    // 游泳相关状态切换
    switch (m_eMoveState)
    {
    case cmsOnStand:
    case cmsOnWalk:
    case cmsOnRun:
    case cmsOnSit:
        if (nWaterline >= nSwimWaterline)
        {
            m_nConvergenceSpeed = 0;
            m_bSlip             = false;
            m_nVelocityXY       = 0;
            m_nVelocityZ        = 0;
            SetMoveState(cmsOnFloat);
        }
        break;

    case cmsOnJump:
        if (m_nVelocityZ < 0 && nSwimWaterline <= nWaterline)
        {
            m_nConvergenceSpeed = 0;
            m_nVelocityZ        = 0;
            m_nVelocityXY       = 0;
            SetMoveState(cmsOnFloat);
        }
        break;

    case cmsOnSwimJump:
        if (nSwimWaterline > nWaterline)
        {
            m_nJumpCount++;
            SetMoveState(cmsOnJump);
        }
        else
        {
            if (m_nVelocityZ <= 0)
            {
                m_nConvergenceSpeed = 0;
                m_nVelocityZ        = 0;
                m_nVelocityXY       = 0;
                SetMoveState(cmsOnFloat);
            }
        }
        break;

    case cmsOnSwim:
    case cmsOnFloat:
        if (nWaterline < nSwimWaterline)
        {
            //cprintf("water %d < %d, switch to stand !\n", nWaterline, nSwimWaterline);
            m_nConvergenceSpeed = 0;
            m_eMoveState        = cmsOnStand;
        }
        break;

    default:
        break;
    }


    // 在走/跑状态时,如果悬空超过3米,自动转为跳跃状态
    if (m_eMoveState == cmsOnWalk || m_eMoveState == cmsOnRun || m_eMoveState == cmsOnSwim || m_eMoveState == cmsOnStand)
    {
        if (IS_NPC(m_dwID)) // NPC 始终贴地，不跳跃。
            m_nZ = nEarthPos;

        if (m_nZ - m_pCell->m_wHighLayer * POINT_PER_ALTITUDE > CRITICAL_JUMP_HEIGHT)
        {
            m_eMoveState = cmsOnJump;   
            m_nJumpCount++;
        }
    }

    if (
        (
            m_eMoveState == cmsOnStand || m_eMoveState == cmsOnRun || m_eMoveState == cmsOnWalk || 
            m_eMoveState == cmsOnSwim || m_eMoveState == cmsOnFloat
        )
        ||
        (
            m_eMoveState == cmsOnJump && m_nWaterFlyAbility && 
            m_pCell->m_BaseInfo.dwCellType == ctWater && 
            m_nZ - m_pCell->m_wHighLayer * POINT_PER_ALTITUDE <= POINT_PER_ALTITUDE
        )
    )
    {
        m_nJumpCount = 0;
    }

Exit0:
	return;
}

void KCharacter::ProcessDrowning()
{
    int nWaterline          = GetWaterline();
	int nDivingCountLast    = m_nDivingCount;

    // 美术制作时,人物模型颈部高度大约是人物高度的87/100
    // 如果入水深度到嘴的时候开始出现呼吸条,大致按照9/10算

    if (IS_UNDER_WATER(nWaterline, m_nHeight))
    {
        // 入水开始计时...
        if (m_eMoveState != cmsOnDeath && m_nDivingFrame > 0)
        {
            m_nDivingCount++;

#ifdef _SERVER
            if (m_nDivingCount > m_nDivingFrame)
            {
                int nDrowningTime   = g_pSO3World->m_Settings.m_ConstList.nDrowningTime;
                int nLife           = 0;

                KGLOG_PROCESS_ERROR(nDrowningTime > 0);
                nLife = m_nMaxLife * (m_nDivingFrame + nDrowningTime - m_nDivingCount) / nDrowningTime;

                if (nLife <= m_nCurrentLife)
                {
                    m_nCurrentLife = nLife;
                }
                else
                {
                    if (m_nDivingCount % GAME_FPS == 0)
                    {
                        m_nCurrentLife--;
                    }
                }

                if (m_nCurrentLife <= 0)
                {
                    GoToHell();
                }
            }
#endif
        }
    }
    else
    {
        if (m_pCell->m_BaseInfo.dwCellType == ctWater)
        {
            m_nDivingCount = m_nDivingCount * 63 / 64;
        }
        else
        {
            m_nDivingCount = 0;
        }
    }

#ifdef _CLIENT
    if (g_pGameWorldUIHandler && !g_pSO3World->m_bForwardCharacterPosition)
    {
        if (g_pSO3World->m_dwClientPlayerID == m_dwID)
        {
            if (nDivingCountLast < 1)
            {
                if (m_nDivingCount > 0)
                {
                    //显示呼吸条
                    g_pGameWorldUIHandler->OnShowSwimmingProgress();
                }
            }
            else
            {
                if (m_nDivingCount < 1)
                {
                    //隐藏呼吸条
                    g_pGameWorldUIHandler->OnHideSwimmingProgress();
                }
            }
        }
    }
#endif

Exit0:
    return;
}

BOOL KCharacter::ProcessDropDamage()
{
    BOOL        bResult             = false;
    const int   DROP_DAMAGE_SPEED   = 443;
    int         nDropSpeed          = -m_nVelocityZ;
    int         nDropDefence        = m_nDropDefence;
    int         nDamage             = 0;

#ifdef _SERVER    
	KG_PROCESS_ERROR(m_eMoveState != cmsOnDeath);

    KG_PROCESS_ERROR(m_eMoveState != cmsOnDash); // 冲锋的时候不计算摔伤，防止向地面冲锋的时候摔死
	    
    KG_PROCESS_ERROR(nDropSpeed > DROP_DAMAGE_SPEED);

    MAKE_IN_RANGE(nDropDefence, 0, KILO_NUM);

    nDamage      = m_nMaxLife * max(5 * (nDropSpeed - DROP_DAMAGE_SPEED) - nDropDefence, 0) / KILO_NUM;

    if (nDamage > m_nMaxLife)
    {
        nDamage = m_nMaxLife;
    }

    m_nCurrentLife -= nDamage;

    g_PlayerServer.DoCommonHealth(this, -nDamage);

    if (m_nCurrentLife <= 0)
    {
        GoToHell();
    }
#endif

    bResult = true;
Exit0:
    return bResult;
}

BOOL KCharacter::ProcessDropSpeed()
{
    BOOL     bResult      = false;
    KCell*   pEarthCell   = NULL;
    int      nAlpha       = 0;
    int      nBeta        = 0;
    int      nVelocityZ   = 0;
    int      nVelocityXY  = 0;
    int      nSinBeta     = 0;
    int      nCosBeta     = 0;
    int      nVelocityX   = 0;
    int      nVelocityY   = 0;
    int      nDropXY      = 0;
    int      nDropXSub    = 0;
    int      nDropYSub    = 0;
    int      nSlipSpeed   = 0;

    pEarthCell = m_pCell;
    if (m_pCell->m_BaseInfo.dwCellType == ctWater)
    {
        pEarthCell = m_pRegion->GetPreCell(m_nXCell, m_nYCell, m_pCell);
    }
    KGLOG_PROCESS_ERROR(pEarthCell);

    nAlpha       = pEarthCell->GetGradientDirection();
    nBeta        = pEarthCell->GetGradientDegree();
    nSlipSpeed   = pEarthCell->GetCriticalSlipSpeed();

    if (nBeta <= DIRECTION_COUNT / 4 / 8)
    {
        // 这种情况可以简化处理
        m_nVelocityZ = 0;
        goto Exit0;
    }
    
    nSinBeta     = g_Sin(nBeta);
    nCosBeta     = g_Cos(nBeta);

    nVelocityZ  = m_nVelocityZ * nSinBeta / SIN_COS_NUMBER * nSinBeta / SIN_COS_NUMBER;
    nDropXY     = ZPOINT_TO_XYPOINT(m_nVelocityZ * nSinBeta * VELOCITY_ZOOM_COEFFICIENT) / SIN_COS_NUMBER * nCosBeta / SIN_COS_NUMBER;
    
    nDropXSub = nDropXY * g_Cos(nAlpha) / SIN_COS_NUMBER;
    nDropYSub = nDropXY * g_Sin(nAlpha) / SIN_COS_NUMBER;

    nVelocityX = m_nVelocityXY * g_Cos(m_nDirectionXY) / SIN_COS_NUMBER + nDropXSub;
    nVelocityY = m_nVelocityXY * g_Sin(m_nDirectionXY) / SIN_COS_NUMBER + nDropYSub;

    nVelocityXY = (int)sqrt((float)(nVelocityX * nVelocityX + nVelocityY * nVelocityY));

    if (nVelocityXY >= nSlipSpeed * VELOCITY_ZOOM_COEFFICIENT || m_bSlip)
    {
        m_nVelocityZ   = nVelocityZ;
        m_nVelocityXY  = nVelocityXY;

        MAKE_IN_RANGE(m_nVelocityXY, 0, MAX_ZOOM_VELOCITY);
        MAKE_IN_RANGE(m_nVelocityZ, MIN_VELOCITY_Z, MAX_VELOCITY_Z);

        m_nDirectionXY = g_GetDirection(0, 0, nVelocityX, nVelocityY);
    }
    else
    {
        m_nVelocityZ = 0;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KCharacter::CheckSlip()
{
    BOOL    bResult    = false;
    BOOL    bCellSlip  = false;
    KCell*  pCell      = m_pCell;

    KG_PROCESS_ERROR(IS_PLAYER(m_dwID));
    KG_PROCESS_ERROR(
        m_eMoveState == cmsOnRun || m_eMoveState == cmsOnStand || 
        m_eMoveState == cmsOnWalk || m_eMoveState == cmsOnSit || m_eMoveState == cmsOnJump
    );

    if (m_pCell->m_BaseInfo.dwCellType == ctWater)
    {
        KCell* pPreCell = m_pRegion->GetPreCell(m_nXCell, m_nYCell, m_pCell);
        if (pPreCell && pPreCell->m_BaseInfo.dwCellType != ctWater)
        {
            pCell = pPreCell;
        }
    }

    bCellSlip = pCell->CheckSlip();

    bResult = bCellSlip && (m_nZ - pCell->m_wHighLayer * POINT_PER_ALTITUDE <= 0 || m_bSlip);

Exit0:
    return bResult;
}

int  KCharacter::GetWaterline()
{
    int     nResult     = 0;

    assert(m_pCell);
    KG_PROCESS_ERROR(m_pCell->m_BaseInfo.dwCellType == ctWater);

    KG_PROCESS_ERROR(m_nZ - m_pCell->m_wHighLayer * POINT_PER_ALTITUDE <= 0);

    if (m_nZ < m_pCell->m_wLowLayer * POINT_PER_ALTITUDE)
    {
        m_nZ = m_pCell->m_wLowLayer * POINT_PER_ALTITUDE;
    }

    nResult = m_pCell->m_wHighLayer * POINT_PER_ALTITUDE - m_nZ;
Exit0:
    assert(nResult >= 0);
    return nResult;
}

int KCharacter::GetSubjectiveDirection()
{
    int nResult = 0;

    if (m_eMoveState == cmsOnDash)
    {
        nResult = m_nDirectionXY;
    }
    else if (m_eMoveState == cmsOnRun || m_eMoveState == cmsOnWalk || m_eMoveState == cmsOnSwim || m_eMoveState == cmsOnJump)
    {
        nResult = g_GetDirection(m_nX, m_nY, m_nDestX, m_nDestY);
    }
    else
    {
        nResult = m_nFaceDirection;
    }

    return nResult;
}

int KCharacter::GetSubjectiveVelocity()
{
    return m_nConvergenceSpeed;
}

BOOL KCharacter::CanDialog(KCharacter* pCharacter)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;
	
	bRetCode = CanDoAction(pCharacter, DIALOG_ACTION_ID);
	KG_PROCESS_ERROR(bRetCode);
	
	bResult = true;
Exit0:
	return bResult;
}

BOOL KCharacter::CanDoAction(KCharacter* pCharacter, DWORD dwAction)
{
    BOOL                bResult             = false;
	BOOL                bRetCode            = false;
	DWORD               dwDistance2         = 0;
	KCharacterAction*   pCharacterAction    = NULL;
    int                 nSelfZ              = 0;
    int                 nTargetZ            = 0;
		
	assert(pCharacter);
    KGLOG_PROCESS_ERROR(m_pCell);
    KGLOG_PROCESS_ERROR(pCharacter->m_pCell);

	pCharacterAction = g_pSO3World->m_Settings.m_CharacterActionList.GetAction(dwAction);
	KGLOG_PROCESS_ERROR(pCharacterAction);

    bRetCode = g_InRange(this, pCharacter, pCharacterAction->dwRange);
    KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	return bResult;
}

#ifdef _SERVER
void KCharacter::Stealth(BOOL bStealthy)
{
    KGLOG_PROCESS_ERROR(IS_PLAYER(m_dwID));

    m_bStealthy = bStealthy;
    
    KG_PROCESS_ERROR(m_pScene && m_pRegion);

    if (bStealthy)
    {
        g_PlayerServer.DoRemoveCharacter(this, false);
    }
    else
    {
        m_pScene->SetViewObjectRegisterFlag(m_pRegion, true);
        m_pScene->RegisterPlayerToArea((KPlayer*)this, m_pRegion);
    }
    
Exit0:
    return;
}
#endif


inline BOOL IsBattleEnemy(int nSelfSide, int nDestSide, DWORD dwRelationMask)
{
    BOOL bResult = false;
    int  nPos    = 0;

    KG_PROCESS_ERROR(nSelfSide >= 0);
    KG_PROCESS_ERROR(nDestSide >= 0);

    if (nSelfSide > nDestSide)  // 这个函数，是把一个4*4的矩阵压缩到一个DWORD里。
    {                           // 由于矩阵的数据是对称的，我们只取X <= Y的部分。
        int nTemp = nSelfSide;

        nSelfSide = nDestSide;
        nDestSide = nTemp;
    }

    for (int i = 1; i <= nDestSide; i++)
    {
        nPos += i;
    }
    nPos += nSelfSide; // 0开始的

    assert(nPos < sizeof(DWORD) * CHAR_BIT);

    bResult = (dwRelationMask & (1 << nPos));
Exit0:
    return bResult;
}


// 这个函数返回的是从pSrcCharacter到pDstCharacter的关系，某些关系是单向的(PK惩罚之红名)，
// 所以从pSrcCharacter到pDstCharacter和从pDstCharacter到pSrcCharacter的结果可能不一样
int g_GetRelation(KCharacter* pSrcCharacter, KCharacter* pDstCharacter)
{
	int     nResult         = sortNone;
    BOOL    bInBattleField  = false;

	assert(pSrcCharacter);
	assert(pDstCharacter);

    KGLOG_PROCESS_ERROR(pSrcCharacter->m_pScene);
    KGLOG_PROCESS_ERROR(pDstCharacter->m_pScene);

	if (IS_PLAYER(pSrcCharacter->m_dwID))
	{
		KPlayer* pSrcPlayer = (KPlayer*)(pSrcCharacter);
			
		if (IS_PLAYER(pDstCharacter->m_dwID))
		{
			KPlayer* pDstPlayer = (KPlayer*)(pDstCharacter);
			nResult = pSrcPlayer->GetPlayerRelation(pDstPlayer);
		}
		else
		{
			KNpc* pDstNpc = (KNpc*)(pDstCharacter);
			nResult = pSrcPlayer->GetNpcRelation(pDstNpc);
		}
	}
	else
	{
		KNpc* pSrcNpc = (KNpc*)(pSrcCharacter);

		if (IS_PLAYER(pDstCharacter->m_dwID))
		{
			KPlayer* pDstPlayer = (KPlayer*)(pDstCharacter);
			nResult = pSrcNpc->GetPlayerRelation(pDstPlayer);
		}
		else
		{
			KNpc* pDstNpc = (KNpc*)(pDstCharacter);
			nResult = pSrcNpc->GetNpcRelation(pDstNpc);
		}
	}

    // 只要有一个人在战场中，就应该进入判断。
    // 也许是GM之类的人，在战场里观看。
    bInBattleField =
        (pSrcCharacter->m_nBattleFieldSide > INVALID_BATTLE_SIDE && pSrcCharacter->m_nBattleFieldSide < MAX_BATTLE_SIDE) ||
        (pDstCharacter->m_nBattleFieldSide > INVALID_BATTLE_SIDE && pDstCharacter->m_nBattleFieldSide < MAX_BATTLE_SIDE);

    if (
        pSrcCharacter != pDstCharacter &&
        pDstCharacter->m_pScene == pSrcCharacter->m_pScene &&
        bInBattleField
    )
    {
        BOOL bRetCode = false;

        bRetCode = IsBattleEnemy(
            pSrcCharacter->m_nBattleFieldSide, pDstCharacter->m_nBattleFieldSide,
            pSrcCharacter->m_pScene->m_dwBattleRelationMask
        );
        if (bRetCode)
        {
            nResult |= sortEnemy;
            nResult &= ~sortAlly;
            nResult &= ~sortNeutrality;
        }
        else
        {
            nResult |= sortAlly;
            nResult &= ~sortEnemy;
            nResult &= ~sortNeutrality;
        }
    }


Exit0:
	return nResult;
}

BOOL KCharacter::SetForceID(DWORD dwForceID)
{
    BOOL    bResult         = false;
	BOOL    bRetCode        = false;

    KG_PROCESS_SUCCESS(dwForceID == m_dwForceID);

	m_dwForceID = dwForceID;

#ifdef _SERVER
	g_PlayerServer.DoSetForce(this);

    if (IS_PLAYER(m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)this;
        g_RelayClient.DoChangeRoleForceIDRequest(pPlayer);
    }
#endif //_SERVER

#ifdef _CLIENT
    if (g_pGameWorldUIHandler && g_pGameWorldRepresentHandler)
    {
	    if (m_dwID == g_pSO3World->m_dwClientPlayerID)
	    {
            KUIEventCurrentPlayerForceChanged param = { g_pSO3World->m_dwClientPlayerID };
		    KPlayer* pClientPlayer = (KPlayer*)this;
		    KGLOG_PROCESS_ERROR(pClientPlayer);

            pClientPlayer->m_QuestList.UpdateNpcQuestMark(-1); // 更新任务周围Npc的任务标记

            g_pGameWorldUIHandler->OnCurrentPlayerForceChanged(param); // 刷新Player的势力面板
            
            g_pGameWorldUIHandler->OnUpdateAllRelation();
            g_pGameWorldRepresentHandler->OnCharacterUpdateAllRelation();
	    }
        else
        {
            KUIEventUpdateRelation param = {m_dwID};

            g_pGameWorldRepresentHandler->OnCharacterUpdateTitleMark(this); // 更新任务周围Npc的任务标记
            
            g_pGameWorldUIHandler->OnUpdateRelation(param);
            g_pGameWorldRepresentHandler->OnCharacterUpdateRelation(this);
        }
    }
#endif //_CLIENT

Exit1:
	bResult = true;
Exit0:
	return bResult;
}

const char* KCharacter::GetName()
{
	BOOL bRetCode = false;

	if (IS_PLAYER(m_dwID))
	{
		//玩家
		return m_szName;
	}
	else
	{
		KNpc* pNpc = (KNpc*)this;

		KGLOG_PROCESS_ERROR(pNpc->m_pTemplate);
		return pNpc->m_pTemplate->szName;
	}
Exit0:
	return NULL;
}

BOOL KCharacter::SelectTarget(TARGET_TYPE eTargetType, DWORD dwTargetID)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    bRetCode = m_SelectTarget.SetTarget(eTargetType, dwTargetID);
    KG_PROCESS_ERROR(bRetCode);

#ifdef _SERVER
    if (IS_PLAYER(m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)this;
        g_PlayerServer.DoSetTarget(pPlayer, pPlayer);
    }

    g_PlayerServer.DoBroadcastTarget(this);
#else
    g_PlayerClient.DoRequestTarget(eTargetType, dwTargetID);

    g_pGameWorldUIHandler->OnUpdateSelectTarget();
#endif

    bResult = true;
Exit0:
    return bResult;
}

size_t PainText2TalkData(size_t uBuffSize, BYTE* pbyBuff, const char cszText[])
{
    size_t  uResult     = 0;
    BOOL    bRetCode    = false;
    size_t  uTextLen    = 0;
    size_t  uBuffLen    = 0;

    assert(uBuffSize);
    assert(pbyBuff);
    assert(cszText);

    uTextLen = strlen(cszText);
    uBuffLen = sizeof(BYTE) + uTextLen + sizeof('\0');

    KG_PROCESS_ERROR(uBuffLen <= uBuffSize);

    assert(tdbTotal <= UCHAR_MAX);

    *pbyBuff = (BYTE)tdbText;
    strcpy((char*)pbyBuff + sizeof(BYTE), cszText);

    uResult = uBuffLen;
Exit0:
    return uResult;
}

