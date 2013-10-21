#include "stdafx.h"
#include <conio.h>
#include "Global.h"
#include "SO3GlobalDef.h"
#include "KSO3World.h"
#include "KObjectManager.h"
#include "KScene.h"
#include "KRegion.h"
#include "KMath.h"
#include "KAttrModifier.h"
//#include "KSkill.h"
//#include "KDoodad.h"
//#include "KSkillManager.h"
#include "KWorldSettings.h"
#include "KScriptCenter.h"
#include "KItemLib.h"
#include "KCharacter.h"
//#include "KCharacterActionList.h"
#include "KMonster.h"
#include "KPlayerEyeshot.h"

// 移动时重设寻路状态的临界值
#define DEF_MIN_PATHFIND_DISTANCE	((MOVE_TARGET_RANGE * 0.7) * (MOVE_TARGET_RANGE * 0.7))

// 这个值表示每DAMAGE_2_MANA_RATE点内力抵消1点伤害
#define DAMAGE_2_MANA_RATE   2

#define CORPSE_DISPPEAR_TIME	(GAME_FPS * 60 * 2)

//每个人对Npc进行对话操作的时候
#define MAX_IDLE_FRAME_ON_DIALOGUE (GAME_FPS * 10)

KCharacter::KCharacter()
{
}

KCharacter::~KCharacter()
{
}

BOOL KCharacter::Init(void)
{
	BOOL bRetCode = FALSE;

	bRetCode = KSceneObject::Init();
	KGLOG_PROCESS_ERROR(bRetCode);

	m_nHeight = POINT_PER_ALTITUDE * 14;

	memset(m_szName, 0, sizeof(m_szName));

	m_eGender							    = cgMale;
	m_eKind								    = ckInvalid;
	m_eRace								    = crHuman;
	m_dwForceID							    = 0;			// 默认值，为玩家初进游戏时的所属势力
	m_nLevel							    = 0;
	m_nExperience						    = 0;
	m_nMoral							    = 0;
	m_dwCharacterAction						= 0;

    // ---------- 移动相关 ------------------->
	m_eMoveState                            = cmsOnStand;

    m_nVelocityXY                           = 0;
    m_nDirectionXY                          = 0;
    m_nVelocityZ                            = 0;

	m_nDestX                                = 0;
    m_nDestY                                = 0;

    m_nConvergenceSpeed                     = 0;

	m_nWalkSpeed                            = 0;
	m_nRunSpeed                             = 0;
    m_nSwimSpeed                            = 0;

	m_nWalkSpeedBase                        = 0;      
	m_nRunSpeedBase                         = 0;     
    m_nSwimSpeedBase                        = 0;

	m_nMoveSpeedPercent                     = 0;
                                    
	m_nJumpCount                            = 0;
	m_nMaxJumpCount                         = 1;

    m_nCurrentGravity                       = DEFAULT_GRAVITY;
	m_nGravity                              = DEFAULT_GRAVITY;
    m_nGravityBase                          = DEFAULT_GRAVITY;
    m_nGravityPercent                       = 0;
    m_nDropDefence                          = 0;

    m_nJumpSpeedXY                          = 0;      
    m_nJumpSpeedXYBase                      = 0;
    m_nJumpSpeedXYPercent                   = 0;

    m_nJumpSpeedZ                           = 0;     
    m_nJumpSpeedZBase                       = 0;
    m_nJumpSpeedZPercent                    = 0;

    m_nDivingCount                          = 0;
    m_nDivingFrame                          = GAME_FPS * 36;         
    m_nDivingFrameBase                      = GAME_FPS * 36;
    m_nDivingFramePercent                   = 0; 

    m_nSwimAbility                          = 0;
    m_nWaterFlyAbility                      = 0;  

    m_bTryPathState                         = false;
    m_nTryPathSide                          = 0;
    m_nTryPathDestX                         = 0;
    m_nTryPathDestY                         = 0;

    m_nSubjectiveMoveDirection              = 0;

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
	m_nCurrentLife                          = 0;
	m_nMaxLife                              = 0;
	m_nMaxLifeBase                          = 0;
	m_nMaxLifePercentAdd                    = 0;
	m_nMaxLifeAdditional                    = 0;
	m_nLifeReplenish                        = 0;
    m_nSpunkLifeReplenish                   = 0;
	m_nLifeReplenishPercent                 = 0;

	// ---------- 内力 ----------------------->
	m_nCurrentMana                          = 0;
	m_nMaxMana                              = 0;
	m_nMaxManaBase                          = 0;
	m_nMaxManaPercentAdd                    = 0;
	m_nMaxManaAdditional                    = 0;
	m_nManaReplenish                        = 0;
    m_nSpunkManareplenish                   = 0;
	m_nManaReplenishPercent                 = 0;

	// ---------- 怒气 ----------------------->
	m_nCurrentRage                          = 0;
	m_nMaxRage                              = 0;
	m_nMaxRageBase                          = 0;
	m_nMaxRagePercentAdd                    = 0;
	m_nMaxRageAdditional                    = 0;
	m_nRageReplenish                        = 0;
	m_nRageReplenishPercent                 = 0;
   
    // ---------- 杂项 ----------------------->
	m_nDodge                                = 0;
    m_nAgilityDodege                        = 0;
	m_bFightState                           = false;
	m_bSheathFlag                           = false;
    m_nPositiveShield                       = 0;     
    m_nNegativeShield                       = 0;     
	m_nGlobalBlock                          = 0;
	m_nGlobalDamageAbsorb                   = 0;     
	m_nGlobalDamageManaShield               = 0; 

	m_nStealLifePercent                     = 0;
	m_nStealManaPercent                     = 0;

	m_nBeatBackRate                         = KILO_NUM;
	m_nStunStrikeRate                       = 0;
	m_nKnockedBackRate                      = 0;
	m_nKnockedOffRate                       = 0;

	memset(m_nPhysicsAttackPower, 0, sizeof(m_nPhysicsAttackPower));
	memset(m_nMagicAttackPower, 0, sizeof(m_nMagicAttackPower));

	// -------------- 外功 ------------------------------------------------------>

	m_nMeleeWeaponDamageBase                = 0;
	m_nMeleeWeaponDamageRand                = 0;

	m_nRangeWeaponDamageBase                = 0;
	m_nRangeWeaponDamageRand                = 0;

	m_nSkillPhysicsDamage                   = 0;
    m_nSkillPhysicsDamageRand               = 0;
	m_nSkillPhysicsDamagePercent            = 0;
	m_nPhysicsAttackPowerBase               = 0;
	m_nPhysicsDamageCoefficient             = 0;

	m_nPhysicsAttackHit                     = 0;
    m_nAgilityCriticalStrike                = 0;
	m_nPhysicsCriticalStrike                = 0; 

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

	// ------------- 内功 --------------------------------->
	m_nSpunkAttackPower                     = 0;
	m_nMeleeSpunkAttackPowerPercent         = 0;
    m_nMeleeSpunkAttackPower                = 0;
	m_nRangeSpunkAttackPowerPercent         = 0;
    m_nRangeSpunkAttackPower                = 0;
    m_nSpiritCriticalStrike                 = 0;
	// ------------- 阳性内功 ----------------------------->
	m_nSkillSolarDamage                     = 0;
    m_nSkillSolarDamageRand                 = 0;
	m_nSkillSolarDamagePercent              = 0;	
	m_nSolarAttackPowerBase                 = 0;
	m_nSolarDamageCoefficient               = 0;

	m_nSolarMagicHit                        = 0;               
	m_nSolarCriticalStrike                  = 0;         

	m_nSolarMagicResistPercent              = 0;
	m_nSolarMagicShield                     = 0;		

	m_nSolarMagicReflection                 = 0;		
	m_nSolarMagicReflectionPercent          = 0;	

	m_nSolarMagicBlock                      = 0;             

	m_nSolarDamageAbsorb                    = 0;     
	m_nSolarDamageManaShield                = 0; 

	// ------------- 中性内功 ----------------------------->
	m_nSkillNeutralDamage                   = 0;
    m_nSkillNeutralDamageRand               = 0;
	m_nSkillNeutralDamagePercent            = 0;	
	m_nNeutralAttackPowerBase               = 0;
	m_nNeutralDamageCoefficient             = 0;

	m_nNeutralMagicHit                      = 0;               
	m_nNeutralCriticalStrike                = 0;         

	m_nNeutralMagicResistPercent            = 0;

	m_nNeutralMagicShield                   = 0;

	m_nNeutralMagicReflection               = 0;		
	m_nNeutralMagicReflectionPercent        = 0;	

	m_nNeutralMagicBlock                    = 0;             

	m_nNeutralDamageAbsorb                  = 0;     
	m_nNeutralDamageManaShield              = 0; 

	// ------------- 阴性内功 ------------------------------->
	m_nSkillLunarDamage                     = 0;
    m_nSkillLunarDamageRand                 = 0;
	m_nSkillLunarDamagePercent              = 0;	
	m_nLunarAttackPowerBase                 = 0;
	m_nLunarDamageCoefficient               = 0;

	m_nLunarMagicHit                        = 0;               
	m_nLunarCriticalStrike                  = 0;         

	m_nLunarMagicResistPercent              = 0;

	m_nLunarMagicShield                     = 0;

	m_nLunarMagicReflection                 = 0;		
	m_nLunarMagicReflectionPercent          = 0;	

	m_nLunarMagicBlock                      = 0;             

	m_nLunarDamageAbsorb                    = 0;     
	m_nLunarDamageManaShield                = 0; 


	// ------------- 毒性 ------------------------------------>

	m_nSkillPoisonDamage                    = 0;
    m_nSkillPoisonDamageRand                = 0;
	m_nSkillPoisonDamagePercent             = 0;	
	m_nPoisonAttackPowerBase                = 0;
	m_nPoisonDamageCoefficient              = 0;

	m_nPoisonMagicHit                       = 0;               
	m_nPoisonCriticalStrike                 = 0;         

	m_nPoisonMagicResistPercent             = 0;

	m_nPoisonMagicShield                    = 0;

	m_nPoisonMagicReflection                = 0;		
	m_nPoisonMagicReflectionPercent         = 0;	

    m_nPoisonMagicBlock                     = 0;             

    m_nPoisonDamageAbsorb                   = 0;     
    m_nPoisonDamageManaShield               = 0; 

	// ----------- 治疗(生命回复) ---------------------------->
	m_nTherapyPowerBase                     = 0;
	m_nSkillTherapy                         = 0;
    m_nSkillTherapyRand                     = 0;
	m_nSkillTherapyPercent                  = 0; 
	m_nTherapyCoefficient                   = 0;

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

	m_dwRepresentState                      = 0;

    m_bOnTalk                               = false;

//	m_nStunCounter                          = 0;
//	m_nHaltCounter                          = 0;
//	m_nMoveFrameCounter                     = 0;
                                   
	m_ThreatList.Init(this);
    
//	m_pSkillAttributeParam                  = NULL;
//    m_pSrcSkillCalculateResult              = NULL;
//    m_pDstSkillCalculateResult              = NULL;
	
//	m_BuffList.Init(this);

	m_nNextReplenishFrame = 0;

//	m_OTActionParam.eActionType = otActionIdle;

//	memset(&m_AutoCastSkillParam, 0, sizeof(m_AutoCastSkillParam));

	return TRUE;
Exit0:

	return FALSE;
}

BOOL KCharacter::UnInit(void)
{	
	m_ThreatList.UnInit();
//	m_AIController.UnInit();
//	m_BuffList.UnInit();
	return KSceneObject::UnInit();
}

BOOL KCharacter::Activate(void)
{
	BOOL bRetCode = 0;


	//处理本循环的指令，对于玩家来说是指令列表，对于NPC来说是调用NPCAI
	bRetCode = GetCommand();
	KGLOG_PROCESS_ERROR(bRetCode);


	RunStateMachine();
    // m_pScene指针为空表示自己被删除了
    KG_PROCESS_ERROR(m_pScene);

	ProcessMove();
    // m_pScene指针为空表示自己被删除了
    KG_PROCESS_ERROR(m_pScene);

	return TRUE;
Exit0:
	return FALSE;
}


//处理角色的状态机
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
    case cmsOnMeditate:
        OnMeditate();
        break;
	case cmsOnWalk:
		OnWalk();
		break;
	case cmsOnRun:
		OnRun();
		break;
    case cmsOnSwim:
        break;
	case cmsOnDash:
	case cmsOnKnockedDown:
	case cmsOnKnockedBack:
		OnPassiveMove();
        break;
	case cmsOnKnockedOff:
		break;
	case cmsOnStun:
		OnStun();
        break;
	case cmsOnHalt:
		OnHalt();
        break;
	case cmsOnDeath:
		OnDeath();
		break;
    case cmsOnJump:
        OnJump();
        break;
	default:
		ASSERT(false);
		break;
	}

}



BOOL KCharacter::GetRepresentState(KBUFF_REPRESENT_POS nPos, DWORD&   rdwRepresentID)
{
	BOOL bResult = false;


	switch (nPos)
	{
	case brpInvalid:
		break;
	case brpTrample:
		{
			DWORD dwMask = 0x0000001F;
			rdwRepresentID = m_dwRepresentState & dwMask;
		}
		break;
	case brpBody:
		{
			DWORD dwMask = 0x000003E0;
			rdwRepresentID = m_dwRepresentState & dwMask;
			rdwRepresentID = rdwRepresentID >> 5;
		}
		break;
	case brpTopHead:
		{
			DWORD dwMask = 0x00007C00;
			rdwRepresentID = m_dwRepresentState & dwMask;
			rdwRepresentID = rdwRepresentID >> 10;
		}
		break;
	default:
		KGLOG_PROCESS_ERROR(!"Unexpected KREPRESENT_POS nPos !");
	}

	bResult = true;
Exit0:
	return bResult;
}

BOOL KCharacter::Stop(void)
{
	BOOL bResult = false;

    KG_PROCESS_ERROR(m_eMoveState != cmsOnStand);

    m_nConvergenceSpeed = 0;
	DoStand();


    bResult = true;
Exit0:
	return bResult;
}

BOOL KCharacter::EmendateDestCoordinate(int& nDestX, int& nDestY)
{
	BOOL      bResult   = false;

	KGLOG_PROCESS_ERROR(m_nX >= 0 && m_nX <= MAX_X_COORDINATE);
	KGLOG_PROCESS_ERROR(m_nY >= 0 && m_nY <= MAX_Y_COORDINATE);

	if (nDestX < 0)
	{
		nDestY = m_nY + (nDestY - m_nY) * (m_nX - 0) / (m_nX - nDestX);
		nDestX = 0;
	}

	if (nDestX > MAX_X_COORDINATE)
	{
		nDestY = m_nY + (nDestY - m_nY) * (MAX_X_COORDINATE - m_nX) / (nDestX - m_nX);
		nDestX = MAX_X_COORDINATE;
	}

	if (nDestY < 0)
	{
		nDestX = m_nX + (nDestX - m_nX) * (m_nY - 0) / (m_nY - nDestY);
		nDestY = 0;
	}

	if (nDestY > MAX_Y_COORDINATE)
	{
		nDestX = m_nX + (nDestX - m_nX) * (MAX_Y_COORDINATE - m_nY) / (nDestY - m_nY);
		nDestY = MAX_Y_COORDINATE;
	}

	bResult = true;
Exit0:
	return bResult;
}

BOOL KCharacter::Stand(BOOL bForceMove /* = false*/)
{
    BOOL bResult  = false;
    BOOL bRetCode = false;

	if (m_eMoveState != cmsOnSit && m_eMoveState != cmsOnMeditate && m_eMoveState != cmsOnSwim)
	{
        // 只有坐着或者打坐的时候,才能调Stand切换到站立状态
        // 在跑动时,是停止在目标点时自动切换状态到Stand的,不走这个函数
		goto Exit0;
	}

    m_nConvergenceSpeed = 0;

    DoStand();

    bResult = true;
Exit0:
    return bResult;
}

BOOL KCharacter::WalkTo(int nDestX, int nDestY, BOOL bForceMove/* = false */)
{
    BOOL bResult            = false;
	BOOL bRetCode           = false;
    int  nConvergenceSpeed  = 0;
    int  nVelocityXY        = 0;
    int  nDirectionXY       = 0;

	if (
        m_eMoveState != cmsOnWalk && m_eMoveState != cmsOnRun && m_eMoveState != cmsOnStand &&
        m_eMoveState != cmsOnSit && m_eMoveState != cmsOnMeditate &&  m_eMoveState != cmsOnJump
    )
	{
		goto Exit0;
	}

    nConvergenceSpeed = m_nWalkSpeed;
    nVelocityXY       = m_nVelocityXY;
    nDirectionXY      = m_nDirectionXY;


    bRetCode = EmendateDestCoordinate(nDestX, nDestY);
    KGLOG_PROCESS_ERROR(bRetCode);

    if (nDestX == m_nX && nDestY == m_nY)
    {
        // 只有在走/跑的状态下,才可以通过"走向当前点"来停止移动
        // 注意,跳跃状态不理会所谓的"停止"指令,落地后自然会停止
        KG_PROCESS_ERROR(m_eMoveState == cmsOnWalk || m_eMoveState == cmsOnRun);
        nConvergenceSpeed = 0;
        nVelocityXY       = 0;
        m_eMoveState      = cmsOnStand;
        goto Exit1;
    }
    else
    {
        if (m_eMoveState == cmsOnJump)
        {
            // 在跳跃状态(立定跳)下,允许在空中作一个很小的位移(限一次)
            if (m_nVelocityXY == 0)
            {
                nVelocityXY         = m_nWalkSpeed * VELOCITY_ZOOM_COEFFICIENT;
                nDirectionXY        = g_GetDirection(m_nX, m_nY, nDestX, nDestY);
                nConvergenceSpeed   = m_nWalkSpeed;
                goto Exit1;
            }
            goto Exit0;
        }  

        nVelocityXY  = nConvergenceSpeed * VELOCITY_ZOOM_COEFFICIENT;
        nDirectionXY = g_GetDirection(m_nX, m_nY, nDestX, nDestY);
    }

    if (m_bTryPathState)
    {
        int nDistance = g_GetDistance2(m_nTryPathDestX, m_nTryPathDestY, nDestX, nDestY);
        if (nDistance > MOVE_DEST_RANGE * MOVE_DEST_RANGE * 2)
        {
            m_bTryPathState = false;
        }
    }


	DoWalk();

Exit1:
    ASSERT(nVelocityXY >= 0 && nVelocityXY <= MAX_ZOOM_VELOCITY);
    ASSERT(nConvergenceSpeed >= 0 && nConvergenceSpeed <= MAX_VELOCITY_XY);

    m_nDestX                = nDestX;
    m_nDestY                = nDestY;
    m_nVelocityXY           = nVelocityXY;
    m_nDirectionXY          = nDirectionXY;
    m_nConvergenceSpeed     = nConvergenceSpeed;



    bResult = true;
Exit0:
	return bResult;
}

BOOL KCharacter::RunTo(int nDestX, int nDestY, BOOL bForceMove/* = false */)
{
    BOOL bResult            = false;
	BOOL bRetCode           = false;
    int  nConvergenceSpeed  = 0;
    int  nVelocityXY        = 0;
    int  nDirectionXY       = 0;

	if (
        m_eMoveState != cmsOnWalk && m_eMoveState != cmsOnRun && m_eMoveState != cmsOnStand &&
        m_eMoveState != cmsOnSit && m_eMoveState != cmsOnMeditate &&  m_eMoveState != cmsOnJump
    )
	{
		goto Exit0;
	}

    nConvergenceSpeed = m_nRunSpeed;
    nVelocityXY       = m_nVelocityXY;
    nDirectionXY      = m_nDirectionXY;

    bRetCode = EmendateDestCoordinate(nDestX, nDestY);
    KGLOG_PROCESS_ERROR(bRetCode);

    if (nDestX == m_nX && nDestY == m_nY)
    {
        // 只有在走/跑的状态下,才可以通过"走向当前点"来停止移动
        // 注意,跳跃状态不理会所谓的"停止"指令,落地后自然会停止
        KG_PROCESS_ERROR(m_eMoveState == cmsOnWalk || m_eMoveState == cmsOnRun);
        nConvergenceSpeed = 0;
        nVelocityXY       = 0;
        m_eMoveState      = cmsOnStand;
        goto Exit1;
    }
    else
    {
        int nDestDirection      = g_GetDirection(m_nX, m_nY, nDestX, nDestY);
        int nDirectionMargin    = 0;

        if (m_eMoveState == cmsOnJump)
        {
            // 在跳跃状态(立定跳)下,允许在空中作一个很小的位移(限一次)
            if (m_nVelocityXY == 0)
            {
                nVelocityXY         = m_nWalkSpeed * VELOCITY_ZOOM_COEFFICIENT;
                nDirectionXY        = nDestDirection;
                nConvergenceSpeed   = m_nWalkSpeed;
                goto Exit1;
            }
            goto Exit0;
        }

        nVelocityXY  = nConvergenceSpeed * VELOCITY_ZOOM_COEFFICIENT;
        nDirectionXY = nDestDirection;

        nDirectionMargin = nDestDirection - m_nFaceDirection;
        if (nDirectionMargin > DIRECTION_COUNT / 2)
        {
            nDirectionMargin = DIRECTION_COUNT - nDirectionMargin;
        }
        else if (nDirectionMargin < - DIRECTION_COUNT / 2)
        {
            nDirectionMargin = nDirectionMargin + DIRECTION_COUNT;
        }

        ASSERT(nDirectionMargin >= -DIRECTION_COUNT / 2 && nDirectionMargin <= DIRECTION_COUNT / 2);

        if (abs(nDirectionMargin) > 80)
        {
            nConvergenceSpeed /= 2;
        }
    } 

    if (m_bTryPathState)
    {
        int nDistance = g_GetDistance2(m_nTryPathDestX, m_nTryPathDestY, nDestX, nDestY);
        if (nDistance > MOVE_DEST_RANGE * MOVE_DEST_RANGE * 2)
        {
            m_bTryPathState = false;
        }
    }

	DoRun();

Exit1:
    ASSERT(nVelocityXY >= 0 && nVelocityXY <= MAX_ZOOM_VELOCITY);
    ASSERT(nConvergenceSpeed >= 0 && nConvergenceSpeed <= MAX_VELOCITY_XY);

    m_nDestX                = nDestX;
    m_nDestY                = nDestY;
    m_nVelocityXY           = nVelocityXY;
    m_nDirectionXY          = nDirectionXY;
    m_nConvergenceSpeed     = nConvergenceSpeed;



    bResult = true;
Exit0:
	return bResult;
}

BOOL KCharacter::SwimTo(int nDestX, int nDestY, int nDestZ, BOOL bForceMove /* = false*/)
{
    BOOL bResult              = false;
    BOOL bRetCode             = false;
    int  nAlpha               = 0; // 自己->目标,水平面上的角度
    int  nBeta                = 0; // 自己->目标,垂直面上的角度
    int  nDistance            = 0; // 目标点距离的水平投影
    int  nConvergenceSpeed    = 0;
    int  nVelocityZ           = 0;
    int  nPosZ                = 0;
    int  nDestDirection       = 0;

    ASSERT(m_pCell);

    // 玩家不能主动切换到游泳状态,只能到水里后自动切换
    KG_PROCESS_ERROR(m_eMoveState == cmsOnSwim);

    bRetCode = EmendateDestCoordinate(nDestX, nDestY);
    KGLOG_PROCESS_ERROR(bRetCode);

    if (m_nX == nDestX && m_nY == nDestY)
    {
        nConvergenceSpeed   = 0;
        nVelocityZ          = m_nVelocityZ;
        nDestDirection      = m_nDirectionXY;
    }
    else
    {
        int nDirectionMargin    = 0;

        nDestDirection          = g_GetDirection(m_nX, m_nY, nDestX, nDestY);
        nAlpha                  = nDestDirection;
        if (abs(nDestX - m_nX) >= abs(nDestY - m_nY))
        {
            nDistance = (nDestX - m_nX) * SIN_COS_NUMBER / g_Cos(nAlpha);
        }
        else
        {
            nDistance = (nDestY - m_nY) * SIN_COS_NUMBER / g_Sin(nAlpha);
        }
        nPosZ      = m_pCell->m_wHighLayer * POINT_PER_ALTITUDE + m_nZ;
        nBeta      = g_GetDirection(0, 0, XYPOINT_TO_ZPOINT(nDistance), nDestZ - nPosZ);

#ifdef _CLIENT
        cprintf("Beta = %d, pitch = %d\n", nBeta * 360 / 256, m_nPitchDirection * 360 / 256);
#endif

        nConvergenceSpeed = m_nSwimSpeed * g_Cos(nBeta) / SIN_COS_NUMBER;
        nVelocityZ        = XYPOINT_TO_ZPOINT(m_nSwimSpeed) * g_Sin(nBeta) / SIN_COS_NUMBER;

        nDirectionMargin = nDestDirection - m_nFaceDirection;
        if (nDirectionMargin > DIRECTION_COUNT / 2)
        {
            nDirectionMargin = DIRECTION_COUNT - nDirectionMargin;
        }
        else if (nDirectionMargin < - DIRECTION_COUNT / 2)
        {
            nDirectionMargin = nDirectionMargin + DIRECTION_COUNT;
        }

        ASSERT(nDirectionMargin >= -DIRECTION_COUNT / 2 && nDirectionMargin <= DIRECTION_COUNT / 2);

        if (abs(nDirectionMargin) > 80)
        {
            nConvergenceSpeed /= 2;
        }
    }


    MAKE_IN_RANGE(nVelocityZ, MIN_VELOCITY_Z, MAX_VELOCITY_Z);
    ASSERT(nConvergenceSpeed >= 0 && nConvergenceSpeed <= MAX_VELOCITY_XY);

    m_nDestX                    = nDestX;
    m_nDestY                    = nDestY;
    m_nConvergenceSpeed         = nConvergenceSpeed;
    m_nVelocityZ                = nVelocityZ;

    DoSwim();

    bResult = true;
Exit0:
    return bResult;
}

BOOL KCharacter::Jump(BOOL bStandJump, int nJumpDirection, BOOL bForceMove/* = false */)
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

    ASSERT(nJumpDirection >= 0 && nJumpDirection < DIRECTION_COUNT);

	if (
        m_eMoveState != cmsOnWalk && m_eMoveState != cmsOnRun && m_eMoveState != cmsOnStand &&
        m_eMoveState != cmsOnJump && m_eMoveState != cmsOnSwim
    )
	{
		goto Exit0;
	}


    nDestDirection      = bStandJump ? m_nFaceDirection : nJumpDirection;
    nVelocityXY         = bStandJump ? 0 : (m_nJumpSpeedXY * VELOCITY_ZOOM_COEFFICIENT);
    nDirectionXY        = nDestDirection;
    nConvergenceSpeed   = bStandJump ? 0 : m_nJumpSpeedXY;
    nVelocityZ          = m_nJumpSpeedZ;


    nDirectionMargin = nDestDirection - m_nFaceDirection;
    if (nDirectionMargin > DIRECTION_COUNT / 2)
    {
        nDirectionMargin = DIRECTION_COUNT - nDirectionMargin;
    }
    else if (nDirectionMargin < - DIRECTION_COUNT / 2)
    {
        nDirectionMargin = nDirectionMargin + DIRECTION_COUNT;
    }

    ASSERT(nDirectionMargin >= -DIRECTION_COUNT / 2 && nDirectionMargin <= DIRECTION_COUNT / 2);

    if (abs(nDirectionMargin) > 80)
    {
        nVelocityXY         /= 2;
        nConvergenceSpeed   /= 2;
    }

    nDestX              = m_nX + MOVE_DEST_RANGE * g_Cos(nDestDirection) / SIN_COS_NUMBER;
    nDestY              = m_nY + MOVE_DEST_RANGE * g_Sin(nDestDirection) / SIN_COS_NUMBER;

    bRetCode = EmendateDestCoordinate(nDestX, nDestY);
    KGLOG_PROCESS_ERROR(bRetCode);

    if (m_pCell->m_BaseInfo.dwCellType != ctWater || m_nZ > 0)
    {
        // 在水中不加跳跃计数
        KG_PROCESS_ERROR(m_nJumpCount < m_nMaxJumpCount);
        m_nJumpCount++;
    }

    DoJump();

    ASSERT(nVelocityXY >= 0 && nVelocityXY <= MAX_ZOOM_VELOCITY);
    ASSERT(nVelocityZ >= MIN_VELOCITY_Z && nVelocityZ <= MAX_VELOCITY_Z);

    m_nDestX            = nDestX;
    m_nDestY            = nDestY;

    m_nVelocityXY       = nVelocityXY;
    m_nConvergenceSpeed = nConvergenceSpeed;
    m_nDirectionXY      = nDirectionXY;
    m_nVelocityZ        = nVelocityZ;



    bResult = true;
Exit0:
	return bResult;
}

BOOL KCharacter::SitDown(BOOL bForceMove/* = false*/)
{
    BOOL bResult  = false;
    BOOL bRetCode = false;
    
    KG_PROCESS_ERROR(m_eMoveState == cmsOnStand);

    // 悬空不能坐
    KG_PROCESS_ERROR(m_nZ == 0);

    // 在打滑的地方不能坐
    //KG_PROCESS_ERROR(m_pCell->m_BaseInfo.m_SlipDirection >= ALL_DIRECTION);

    // 不能坐在水里面
    KG_PROCESS_ERROR(m_pCell->m_BaseInfo.dwCellType != ctWater);

    DoSit();

    bResult = true;
Exit0:
    return bResult;
}

BOOL KCharacter::Meditate()
{
    BOOL bResult  = false;
    BOOL bRetCode = false;
    
    KG_PROCESS_ERROR(m_eMoveState == cmsOnStand);

    // 悬空不能打坐
    KG_PROCESS_ERROR(m_nZ == 0);

    // 在打滑的地方不能打坐
    //KG_PROCESS_ERROR(m_pCell->m_BaseInfo.m_SlipDirection >= ALL_DIRECTION);

    // 不能坐在水里面
    KG_PROCESS_ERROR(m_pCell->m_BaseInfo.dwCellType != ctWater);

    DoMeditate();

    bResult = true;
Exit0:
    return bResult;
}

BOOL KCharacter::Dash(int nDirection, int nDistance, int nVelocityXY)
{
    BOOL bResult  = false;
    BOOL bRetCode = false;
    int  nDestX   = 0;
    int  nDestY   = 0;

    ASSERT(nDirection >= 0 && nDirection <= DIRECTION_COUNT);

	if (m_eMoveState != cmsOnWalk && m_eMoveState != cmsOnRun && m_eMoveState != cmsOnStand && m_eMoveState != cmsOnJump)
    {
	    goto Exit0;
    }

	nDestX = m_nX + nDistance * g_Cos(nDirection) / SIN_COS_NUMBER;
	nDestY = m_nY + nDistance * g_Sin(nDirection) / SIN_COS_NUMBER;

    bRetCode = EmendateDestCoordinate(nDestX, nDestY);
    KGLOG_PROCESS_ERROR(bRetCode);

    MAKE_IN_RANGE(nVelocityXY, 0, MAX_VELOCITY_XY);

	m_nConvergenceSpeed = nVelocityXY;
    m_nVelocityXY       = nVelocityXY;
    m_nDirectionXY      = nDirection;
    m_nDestX            = nDestX;
    m_nDestY            = nDestY;

	DoDash();


    bResult = true;
Exit0:
	return bResult;
}

BOOL KCharacter::KnockedDown(int nFrame)
{
    BOOL    bResult     = false;
	//BOOL    bRetCode    = false;


	//if (
 //       m_eMoveState != cmsOnWalk && m_eMoveState != cmsOnRun && m_eMoveState != cmsOnStand &&  
 //       m_eMoveState != cmsOnJump && m_eMoveState != cmsOnSwim &&
 //       m_eMoveState != cmsOnMeditate && m_eMoveState != cmsOnSit
 //   )
 //   {
	//	goto Exit0;
 //   }

 //   m_nMoveFrameCounter = g_SO3World.m_nGameLoop + nFrame;

	//DoKnockedDown();

 //   DoActionIdle();         // 中断当前持续一段时间的行为
 //   DoAutoCastSkillIdle();  // 中断技能自动释放

    bResult = true;
Exit0:
	return bResult;
}

BOOL KCharacter::KnockedBack(int nDirection, int nDistance, int nVelocityXY)
{
    BOOL    bResult    = FALSE;
	//BOOL    bRetCode   = FALSE;
 //   int     nDestX     = 0;
 //   int     nDestY     = 0;

	//if (
 //       m_eMoveState != cmsOnStand && m_eMoveState != cmsOnWalk && 
 //       m_eMoveState != cmsOnRun && m_eMoveState != cmsOnJump &&
 //       m_eMoveState != cmsOnMeditate && m_eMoveState != cmsOnSit &&
 //       m_eMoveState != cmsOnSwim
 //   )
 //   {
 //       goto Exit0;
 //   }

	//nDestX = m_nX + nDistance * g_Cos(nDirection) / SIN_COS_NUMBER;
	//nDestY = m_nY + nDistance * g_Sin(nDirection) / SIN_COS_NUMBER;

 //   bRetCode = EmendateDestCoordinate(nDestX, nDestY);
 //   KG_PROCESS_ERROR(bRetCode);
 //   
	//m_nDestX                    = nDestX;
	//m_nDestY                    = nDestY;

 //   m_nConvergenceSpeed = nVelocityXY;
 //   MAKE_IN_RANGE(m_nConvergenceSpeed, 0, MAX_VELOCITY_XY);
 //   
	//DoKnockedBack();

 //   DoActionIdle();         // 中断当前持续一段时间的行为
 //   DoAutoCastSkillIdle();  // 中断技能自动释放

    bResult = TRUE;
Exit0:
	return bResult;
}

BOOL KCharacter::KnockedOff(int nDirection, int nDistance, int nVelocityXY)
{
	BOOL    bRetCode   = false;
    BOOL    bResult    = false;
 //   int     nVelocityZ = 0;
 //   int     nDestX     = 0;
 //   int     nDestY     = 0;

	//if (
 //       m_eMoveState != cmsOnStand && m_eMoveState != cmsOnWalk && 
 //       m_eMoveState != cmsOnRun && m_eMoveState != cmsOnJump &&
 //       m_eMoveState != cmsOnMeditate && m_eMoveState != cmsOnSit &&
 //       m_eMoveState != cmsOnSwim
 //   )
 //   {
 //       goto Exit0;
 //   }

	//nDestX = m_nX + nDistance * g_Cos(nDirection) / SIN_COS_NUMBER;
	//nDestY = m_nY + nDistance * g_Sin(nDirection) / SIN_COS_NUMBER;

 //   bRetCode = EmendateDestCoordinate(nDestX, nDestY);
 //   KG_PROCESS_ERROR(bRetCode);

	//m_nDestX                    = nDestX;
	//m_nDestY                    = nDestY;

 //   MAKE_IN_RANGE(nVelocityXY, 1, MAX_VELOCITY_XY);

 //   nVelocityZ    = m_nCurrentGravity * nDistance / nVelocityXY / 2;
 //   MAKE_IN_RANGE(nVelocityZ, 0, MAX_VELOCITY_Z);

	//DoKnockedOff();


 //   DoActionIdle();         // 中断当前持续一段时间的行为
 //   DoAutoCastSkillIdle();  // 中断技能自动释放

    bResult = true;
Exit0:
    return bResult;
}

BOOL KCharacter::Stun(void)
{
    BOOL bResult  = false;
    BOOL bRetCode = false;

	//if (
 //       m_eMoveState != cmsOnStand && m_eMoveState != cmsOnWalk && 
 //       m_eMoveState != cmsOnRun && m_eMoveState != cmsOnJump &&
 //       m_eMoveState != cmsOnMeditate && m_eMoveState != cmsOnSit &&
 //       m_eMoveState != cmsOnHalt && m_eMoveState != cmsOnSwim
 //   )
 //   {
 //       goto Exit0;
 //   }

 //   DoStun();

 //   m_nConvergenceSpeed = 0;


 //   DoActionIdle();         // 中断当前持续一段时间的行为
 //   DoAutoCastSkillIdle();  // 中断技能自动释放

    bResult = true;
Exit0:
    return bResult;
}

BOOL KCharacter::Halt(void)
{
    BOOL bResult = false;

	if (
        m_eMoveState != cmsOnStand && m_eMoveState != cmsOnWalk && 
        m_eMoveState != cmsOnRun && m_eMoveState != cmsOnJump &&
        m_eMoveState != cmsOnMeditate && m_eMoveState != cmsOnSit &&
        m_eMoveState != cmsOnSwim
    )
    {
        goto Exit0;
    }

    // 切换状态
    DoHalt();

    m_nConvergenceSpeed = 0;
    bResult = true;
Exit0:
    return bResult;
}

BOOL KCharacter::Die(KCharacter* pKiller)
{
    BOOL        bResult             = false;
	BOOL        bRetCode            = FALSE;
    int         nDeathPunishTime    = 0;

    KGLogPrintf(KGLOG_DEBUG, "Character (%u, \"%s\") dead !\n", m_dwID, m_szName);


    m_nConvergenceSpeed = 0;
	DoDeath(GAME_FPS * 2);

Exit1:
    bResult = true;
Exit0:
	return bResult;
}


BOOL KCharacter::Revive()
{
    KGLogPrintf(KGLOG_DEBUG, "Character (%u, \"%s\") revived !\n", m_dwID, m_szName);

    return true;
}

//说话（屏幕近聊）
BOOL KCharacter::Say(char* pszText)
{
	BOOL bRetCode = FALSE;

	KGLOG_PROCESS_ERROR(pszText);
	return TRUE;

Exit0:

	return FALSE;
}

BOOL KCharacter::Action(DWORD dwCharacterID, BYTE byActionType)
{
	BOOL bRetCode = FALSE;
	KCharacter* pCharacter = NULL;
//	KCharacterAction* pCharacterAction = NULL;
	KGLOG_PROCESS_ERROR(byActionType >= 0);



	bRetCode = TRUE;
Exit0:
	return bRetCode;
}

BOOL KCharacter::Turn(int nNewDirection)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;

	ASSERT(nNewDirection >= 0 && nNewDirection < DIRECTION_COUNT);
    
    KG_PROCESS_ERROR(
        m_eMoveState == cmsOnStand || m_eMoveState == cmsOnWalk || 
        m_eMoveState == cmsOnRun || m_eMoveState == cmsOnJump || m_eMoveState == cmsOnSwim
    );

	m_nFaceDirection = nNewDirection;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KCharacter::Pitch(int nNewDirection)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;

	ASSERT(nNewDirection >= 0 && nNewDirection < DIRECTION_COUNT);
    
	m_nPitchDirection = nNewDirection;

    // 目前没有专门同步俯仰角的协议,移动指令(游泳)会带上俯仰角

    bResult = true;
Exit0:
    return bResult;
}

// 开启
//BOOL KCharacter::Open(KDoodad* pDoodad)
//{
//	KGLOG_PROCESS_ERROR(pDoodad);
//	int  nOpenFrames = pDoodad->GetOpenFrame();
//	DoPickPrepare(pDoodad, nOpenFrames);
//	return TRUE;
//Exit0:
//	return FALSE;
//}

BOOL KCharacter::MoveTo(int nX, int nY, int nZ)
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

	ASSERT(nX >= 0);
	ASSERT(nY >= 0);
    ASSERT(m_pScene);
    ASSERT(m_pRegion);

    nDstRegionX = nX / CELL_LENGTH / REGION_GRID_WIDTH;
    nDstRegionY = nY / CELL_LENGTH / REGION_GRID_HEIGHT;

    pDstRegion = m_pScene->GetRegion(nDstRegionX, nDstRegionY);    
    if (!pDstRegion && m_dwID == m_pOwner->m_pRole->m_dwID)
    {
        bRetCode = m_pScene->ClientLoadRegion(nDstRegionX, nDstRegionY);
        KGLOG_PROCESS_ERROR(bRetCode);

        pDstRegion = m_pScene->GetRegion(nDstRegionX, nDstRegionY);
        KGLOG_PROCESS_ERROR(pDstRegion);
    }
    if (m_dwID != m_pOwner->m_pRole->m_dwID)
    {
        bRetCode = m_pOwner->m_Eyeshot.IsVisible(pDstRegion);
        KGLOG_PROCESS_ERROR(bRetCode);
    }    

    nDstCellX = nX / CELL_LENGTH % REGION_GRID_WIDTH;
    nDstCellY = nY / CELL_LENGTH % REGION_GRID_HEIGHT;    

    pDstCell = pDstRegion->GetProximalCellForObj(nDstCellX, nDstCellY, nZ, m_nHeight, &nRelativeZ);
    KGLOG_PROCESS_ERROR(pDstCell);    

    if (pDstRegion != m_pRegion)
    {
        bRetCode = ChangeRegion(pDstRegion);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    if (pDstCell != m_pCell)
    {
        bRetCode = ChangeCell(pDstCell);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

	m_nX     = nX;
	m_nY     = nY;
    m_nZ     = nRelativeZ;

    m_nXCell = nDstCellX;
    m_nYCell = nDstCellY;

    bResult = true;
Exit0:
	return bResult;
}

BOOL KCharacter::ApplyAttribute(KAttribute* pAttr)
{
	while (pAttr != NULL)
	{
		KGLOG_PROCESS_ERROR(pAttr->nKey > atInvalid && pAttr->nKey < atTotal);

        if (g_AttributeProcessFunctions[pAttr->nKey])
        {
		    g_AttributeProcessFunctions[pAttr->nKey](this, true, pAttr->nValue1, pAttr->nValue2);
        }
        else
        {
            KGLogPrintf(KGLOG_WARNING, "[ApplyAttribute] Warning: g_AttributeProcessFunctions[%d] == NULL !\n", pAttr->nKey);
        }
		pAttr = pAttr->pNext;
	}

	return TRUE;

Exit0:

	return FALSE;
}

BOOL KCharacter::UnApplyAttribute(KAttribute* pAttr)
{
	while (pAttr != NULL)
	{
		KGLOG_PROCESS_ERROR(pAttr->nKey > atInvalid && pAttr->nKey < atTotal);

        if (g_AttributeProcessFunctions[pAttr->nKey])
        {
		    g_AttributeProcessFunctions[pAttr->nKey](this, false, pAttr->nValue1, pAttr->nValue2);
        }
        else
        {
            KGLogPrintf(KGLOG_WARNING, "[ApplyAttribute] Warning: g_AttributeProcessFunctions[%d] == NULL !\n", pAttr->nKey);
        }

		pAttr = pAttr->pNext;
	}

	return TRUE;

Exit0:

	return FALSE;
}

void KCharacter::AttribReplenish()
{
    if (m_pOwner->m_nGameLoop < m_nNextReplenishFrame)
        return;
    
    if (m_eMoveState == cmsOnDeath)
        return;


    if (!m_bFightState)
    {
        // 怒气其实是衰减,在非战斗状态才会衰减
        m_nCurrentRage      += m_nRageReplenish;
        m_nCurrentRage      += m_nMaxRage * m_nRageReplenishPercent / KILO_NUM;
        
        // 在非战斗中, 才恢复生命和内力
        m_nCurrentLife      += m_nLifeReplenish;
        m_nCurrentLife      += m_nMaxLife * m_nLifeReplenishPercent / KILO_NUM;

        m_nCurrentMana      += m_nManaReplenish;
        m_nCurrentMana      += m_nMaxMana * m_nManaReplenishPercent / KILO_NUM;
    }

    m_nNextReplenishFrame = m_pOwner->m_nGameLoop + GAME_FPS;
}

void KCharacter::ReviseAttribValue()
{
    AttribReplenish();

    MAKE_IN_RANGE(m_nCurrentLife, 0, m_nMaxLife);
    MAKE_IN_RANGE(m_nCurrentMana, 0, m_nMaxMana);
    MAKE_IN_RANGE(m_nCurrentRage, 0, m_nMaxRage);
}


DWORD KCharacter::GetMapID()
{
	BOOL bRetCode = FALSE;

	KG_PROCESS_ERROR(m_pScene);

	return m_pScene->m_dwMapID;
Exit0:
	return 0;
}

BOOL KCharacter::OnPassiveMove(void)
{
	if (m_pOwner->m_nGameLoop >= m_nMoveFrameCounter)
	{
		DoStand();
	}

    return true;
}

BOOL KCharacter::OnDeath(void)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    KNpc*       pNpc        = NULL;
    KDoodad*    pDoodad     = NULL;
    KLootList*  pLootList   = NULL;
    int         nItemCount  = 0;

    ASSERT(m_eMoveState == cmsOnDeath);
    ASSERT(m_pScene);


Exit1:
    bResult = true;
Exit0:


    return bResult;
}

void KCharacter::DoStand()
{
	m_eMoveState  = cmsOnStand;   
}

void KCharacter::DoWalk()
{
    m_eMoveState = cmsOnWalk;
}

void KCharacter::DoRun()
{
   m_eMoveState = cmsOnRun;
}

void KCharacter::DoSwim()
{
    m_eMoveState = cmsOnSwim;
}

void KCharacter::DoJump()
{
    m_eMoveState = cmsOnJump;
}

void KCharacter::DoSit()
{
	m_eMoveState = cmsOnSit;
}

void KCharacter::DoMeditate()
{
    m_eMoveState = cmsOnMeditate;   
}


void KCharacter::DoDash()
{
	m_eMoveState = cmsOnDash;
}

void KCharacter::DoKnockedDown()
{
	m_eMoveState = cmsOnKnockedDown;
}

void KCharacter::DoKnockedBack()
{
	m_eMoveState = cmsOnKnockedBack;
}

void KCharacter::DoKnockedOff()
{
	m_eMoveState = cmsOnKnockedOff;
}

void KCharacter::DoStun(void)
{
    m_eMoveState = cmsOnStun;
}

void KCharacter::DoHalt(void)
{
    m_eMoveState = cmsOnHalt;
}
void KCharacter::DoDeath(int nDelayFrame)
{
	m_eMoveState        = cmsOnDeath;
    m_nMoveFrameCounter = m_pOwner->m_nGameLoop + nDelayFrame;
    m_nDivingCount      = 0;
//    m_AIController.Stop();
}

BOOL KCharacter::ForwardPosition(int nForwardFrame)
{
	while (nForwardFrame > 0)
	{
        ProcessMove();        
        if (!m_pCell) // m_pCell为空意味着在Move中被删除了
            break;


		nForwardFrame--;
	}

	return true;
}

BOOL KCharacter::UpdateMoveParameter()
{
    BOOL bResult = false;

    switch (m_eMoveState)
    {
    case cmsOnStand:
        break;
    case cmsOnWalk:
        m_nConvergenceSpeed = m_nWalkSpeed;
        break;
    case cmsOnRun:
        // 注,本来这里做完整的话,还要考虑"如果是后退的话,速度只有一半",但似乎带来的好处不大,无所谓,不做了
        m_nConvergenceSpeed = m_nRunSpeed;
    case cmsOnJump:
        break;
    case cmsOnSwim:

        break;
    default:
        break;
    }

    m_nCurrentGravity = m_nGravity;

    // 在玩家Load的时候,还没有Add到Scene中去,但这个时候m_pScene已经不是空的了
    // 这个时候是不能发同步广播的
    KG_PROCESS_ERROR(m_pRegion);
    

    bResult = true;
Exit0:
    return bResult;
}


void KCharacter::ProcessMove()
{
    ProcessVerticalMove();
    KG_PROCESS_ERROR(m_pScene);

    switch (m_eMoveState)
    {
    case cmsOnStand:
    case cmsOnSit:
    case cmsOnMeditate:
        ProcessFriction();
        ProcessGradientSlip();
        ProcessHorizontalMove();
        break;
    case cmsOnWalk:
    case cmsOnRun:
    case cmsOnSwim:
    case cmsOnKnockedBack:
    case cmsOnKnockedOff:
        ProcessFriction();
        ProcessGradientSlip();
        ProcessAcceleration();
        ProcessHorizontalMove();
        break;
    case cmsOnJump:
        ProcessHorizontalMove();
        break;
    case cmsOnDeath:
        ProcessGradientSlip();
        ProcessHorizontalMove();
        break;
    case cmsOnKnockedDown:
    case cmsOnStun:
    case cmsOnHalt:
        ProcessFriction();
        ProcessGradientSlip();
        break;
    default:
        ASSERT(false);
        goto Exit0;
    }

    KG_PROCESS_ERROR(m_pScene);

Exit0:  
	return;
}

void KCharacter::ProcessFriction()
{
    int     nVelocityX                      = 0;
    int     nVelocityY                      = 0;
    int     nFrictionCoefficient            = 0;    // 摩擦系数,基数1024
    int     nFrictionAcceleration           = 3 * VELOCITY_ZOOM_COEFFICIENT / 2;
    int     nGradient                       = 0;

    KG_PROCESS_ERROR(m_nVelocityXY > 0);

    nGradient = m_pCell->GetGradientDegree();

    nVelocityX = m_nVelocityXY * g_Cos(m_nDirectionXY) / SIN_COS_NUMBER;
    nVelocityY = m_nVelocityXY * g_Sin(m_nDirectionXY) / SIN_COS_NUMBER;

    if (m_pCell->m_BaseInfo.dwCellType == ctPlain)
    {
        if (m_nZ < XYPOINT_TO_ZPOINT(CELL_LENGTH))
        {
            nFrictionCoefficient = 1024;
        }
    }
    else if (m_pCell->m_BaseInfo.dwCellType == ctWater)
    {
        if (m_nZ <= 0)
        {
            nFrictionCoefficient = 512;
        }
    }
    else if (m_pCell->m_BaseInfo.dwCellType == ctIce)
    {
        if (m_nZ < XYPOINT_TO_ZPOINT(CELL_LENGTH / 2))
        {
            nFrictionCoefficient = 128;
        }
    }

    KG_PROCESS_ERROR(nFrictionCoefficient > 0);

    nFrictionAcceleration = nFrictionAcceleration * g_Cos(nGradient) / SIN_COS_NUMBER;

    nFrictionAcceleration = nFrictionAcceleration * nFrictionCoefficient / KILO_NUM;

    m_nVelocityXY -= nFrictionAcceleration;

    if (m_nVelocityXY < 0)
    {
        m_nVelocityXY = 0;
    }

Exit0:
    return;
}

void KCharacter::ProcessGradientSlip()
{
    int     nVelocityX                      = 0;
    int     nVelocityY                      = 0;
    int     nGradientDegree                 = 0;
    int     nGradientDirection              = 0;
    int     nGravityXY                      = 0;
    int     nGradientX                      = 0;
    int     nGradientY                      = 0;

    KG_PROCESS_ERROR(m_nZ < XYPOINT_TO_ZPOINT(CELL_LENGTH));

    nGradientDegree     = m_pCell->GetGradientDegree();
    nGradientDirection  = m_pCell->GetGradientDirection();

    KG_PROCESS_ERROR(nGradientDegree > 2 * DIRECTION_COUNT / 4 / 8);

    nVelocityX = m_nVelocityXY * g_Cos(m_nDirectionXY) / SIN_COS_NUMBER;
    nVelocityY = m_nVelocityXY * g_Sin(m_nDirectionXY) / SIN_COS_NUMBER;

    nGravityXY          = ZPOINT_TO_XYPOINT(
        VELOCITY_ZOOM_COEFFICIENT * m_nCurrentGravity * 
        g_Sin(nGradientDegree) / SIN_COS_NUMBER
    );
    nGradientX          = nGravityXY * g_Cos(nGradientDirection) / SIN_COS_NUMBER;             
    nGradientY          = nGravityXY * g_Sin(nGradientDirection) / SIN_COS_NUMBER;            

    nVelocityX += nGradientX;
    nVelocityY += nGradientY;

    m_nVelocityXY   = (int)sqrt((float)(nVelocityX * nVelocityX + nVelocityY * nVelocityY));
    MAKE_IN_RANGE(m_nVelocityXY, 0, MAX_ZOOM_VELOCITY);

    if (nVelocityX || nVelocityY)
    {
        m_nDirectionXY  = g_GetDirection(0, 0, nVelocityX, nVelocityY);
    }

Exit0:
    return;
}

void KCharacter::ProcessAcceleration()
{
    int     nVelocityX                      = 0;
    int     nVelocityY                      = 0;
    int     nInitiativeAcceleration         = 4 * VELOCITY_ZOOM_COEFFICIENT;    // 主动加速度,注意,还要受斜坡以及,摩擦系数影响
    int     nAccelerationX                  = 0;
    int     nAccelerationY                  = 0;
    int     nDestDirection                  = 0;
    int     nGradient                       = 0;
    int     nProjectVelocity                = 0;
    int     nConvergenceSpeed               = m_nConvergenceSpeed * VELOCITY_ZOOM_COEFFICIENT;
    int     nSlipDirection                  = 0;

    KG_PROCESS_ERROR(m_nConvergenceSpeed > 0);

    if (m_nX == m_nDestX && m_nY == m_nDestY)
    {
        nDestDirection = m_nDirectionXY;
    }
    else
    {
        nDestDirection = g_GetDirection(m_nX, m_nY, m_nDestX, m_nDestY);
    }
    
    nGradient = m_pCell->GetGradientDegree();
    nSlipDirection = m_pCell->GetGradientDirection();

    nProjectVelocity = m_nVelocityXY * g_Cos(m_nDirectionXY - nDestDirection) / SIN_COS_NUMBER;

    KG_PROCESS_ERROR(nProjectVelocity < nConvergenceSpeed);

    if (nProjectVelocity + nInitiativeAcceleration > nConvergenceSpeed)
    {
        nInitiativeAcceleration = nConvergenceSpeed - nProjectVelocity;
    }

    nVelocityX = m_nVelocityXY * g_Cos(m_nDirectionXY) / SIN_COS_NUMBER;
    nVelocityY = m_nVelocityXY * g_Sin(m_nDirectionXY) / SIN_COS_NUMBER;

    nAccelerationX = nInitiativeAcceleration * g_Cos(nDestDirection) / SIN_COS_NUMBER;

    nAccelerationY = nInitiativeAcceleration * g_Sin(nDestDirection) / SIN_COS_NUMBER;

    nVelocityX += nAccelerationX;
    nVelocityY += nAccelerationY;

    m_nVelocityXY   = (int)sqrt((float)(nVelocityX * nVelocityX + nVelocityY * nVelocityY));

    MAKE_IN_RANGE(m_nVelocityXY, 0, MAX_ZOOM_VELOCITY);

    if (nVelocityX || nVelocityY)
    {
        m_nDirectionXY  = g_GetDirection(0, 0, nVelocityX, nVelocityY);
    }
   
Exit0:
    return;
}

void KCharacter::ProcessHorizontalMove()
{
    int             nRetCode        = false;
    int             nMoveDistance   = 0;

    nMoveDistance = m_nVelocityXY / VELOCITY_ZOOM_COEFFICIENT;

    if (m_nZ < XYPOINT_TO_ZPOINT(CELL_LENGTH / 2) && m_pCell->m_BaseInfo.dwCellType != ctWater)
    {
        int nGradient = m_pCell->GetGradientDegree();
        nMoveDistance = nMoveDistance * g_Cos(nGradient) / SIN_COS_NUMBER;
    }

    while (nMoveDistance >= CELL_LENGTH)
    {
        CellMove(CELL_LENGTH, m_nDirectionXY);
        KG_PROCESS_ERROR(m_pScene);
        nMoveDistance -= CELL_LENGTH;
    }

    if (nMoveDistance > 0)
    {
        CellMove(nMoveDistance, m_nDirectionXY);
        KG_PROCESS_ERROR(m_pScene);
        nMoveDistance = 0;
    }

Exit0:
	return;
}

BOOL KCharacter::CellMove(int nStepLength, int nDirection)
{
    BOOL bResult            = false;
    int  nRetCode           = false;
    int  nObstacleNormal    = -1;
    int  nObstacleDirection = -1;
    int  nMoveRetCode       = mrInvalid;
    int  nTrySpeed          = 0;   
    int  nTryDirection      = 0;


    if (
        m_eMoveState == cmsOnWalk || m_eMoveState == cmsOnRun || m_eMoveState == cmsOnSwim ||
        m_eMoveState == cmsOnKnockedBack || m_eMoveState == cmsOnKnockedOff ||
        m_eMoveState == cmsOnDash
    )
    {
        int nDistance = g_GetDistance2(m_nX, m_nY, m_nDestX, m_nDestY);

        if (nDistance <= CELL_LENGTH * CELL_LENGTH / 4)
        {
            m_nVelocityXY       = 0;
            m_nConvergenceSpeed = 0;
            DoStand();
            nMoveRetCode        = mrReachTarget;
            goto Exit1;
        }
    }

    nMoveRetCode = TryMove(nStepLength, nDirection, &nObstacleNormal);
    KG_PROCESS_SUCCESS(nMoveRetCode == mrSucceed);

    KG_PROCESS_ERROR(nMoveRetCode == mrObstacle);
    
    // 必须在能够取得障碍法线方向的时候才能绕路
    KG_PROCESS_ERROR(nObstacleNormal != -1);

    ASSERT(nObstacleNormal >= 0 && nObstacleNormal <= DIRECTION_COUNT);

    // 只有在这些状态下在自动绕路
    KG_PROCESS_ERROR(m_eMoveState == cmsOnWalk || m_eMoveState == cmsOnRun || m_eMoveState == cmsOnSwim || m_eMoveState == cmsOnDash);

    // 遇到障碍时自动绕路,注意,对玩家而言,是根据前方障碍方向判断应该往那个方向绕路,而对NPC而言,只是根据障碍方向
    // 判断应该优先往哪个方向绕路

    // 法线方向加90度,也就是障碍的切线方向
    nRetCode = g_Cos(nDirection - nObstacleNormal);
    if (nRetCode >= 0)
    {
        nObstacleDirection = nObstacleNormal - DIRECTION_COUNT / 4;
    }
    else
    {
        nObstacleDirection = nObstacleNormal + DIRECTION_COUNT / 4;
    }

    // 注,这里取余弦值是为了判别应该往哪边绕路
    nRetCode = g_Cos(nDirection - nObstacleDirection);
    KG_PROCESS_ERROR(nRetCode != 0);

    if (nRetCode >= 0)
    {
        // 从切线方向开始尝试,顺时针绕90

        nTrySpeed  = nStepLength * g_Cos(nDirection - nObstacleDirection) / SIN_COS_NUMBER;
        ASSERT(nTrySpeed >= 0);
        if (nTrySpeed == 0)
        {
            nTrySpeed = 1;
        }
        
        nTryDirection = nObstacleNormal + DIRECTION_COUNT / 4;
        while (nTryDirection > nObstacleNormal)
        {
            nMoveRetCode = TryMove(nTrySpeed, nTryDirection, NULL);
            if (nMoveRetCode == mrSucceed)
                break;
            KG_PROCESS_ERROR(nMoveRetCode == mrObstacle);
            nTryDirection -= 16;
        }
        KG_PROCESS_ERROR(nTryDirection > nObstacleNormal);
        m_nTryPathSide = 0;
    }
    else
    {
        nTrySpeed  = -nStepLength * g_Cos(nDirection - nObstacleDirection) / SIN_COS_NUMBER;
        ASSERT(nTrySpeed >= 0);
        if (nTrySpeed == 0)
        {
            nTrySpeed = 1;
        }
        
        nTryDirection = nObstacleNormal - DIRECTION_COUNT / 4;
        while (nTryDirection < nObstacleNormal)
        {
            nMoveRetCode = TryMove(nTrySpeed, nTryDirection, NULL);
            if (nMoveRetCode == mrSucceed)
                break;
            KG_PROCESS_ERROR(nMoveRetCode == mrObstacle);
            nTryDirection += 16;
        }
        KG_PROCESS_ERROR(nTryDirection < nObstacleNormal);
        m_nTryPathSide = 1;
    }

    m_bTryPathState = true;
    m_nTryPathDestX = m_nDestX;
    m_nTryPathDestY = m_nDestY;

Exit1:
    bResult = true;
Exit0:

    switch (nMoveRetCode)
    {
    case mrSucceed:
        break;
    case mrObstacle:
//        m_AIController.OnPathResult(false);
        break;
    case mrDeleteSelf:
        break;
    case mrOutOfMap:
        m_nConvergenceSpeed = 0;
        break;
    case mrReachTarget:
        //m_AIController.OnPathResult(true);
        break;
    default:
        break;
    }

    return bResult;
}

int KCharacter::TryMove(int nStepLength, int nDirection, int* pnObstacleDirection)
{
    int             nResult             = mrInvalid;
    int             nRetCode            = false;
    int             nMargin             = 0;
    int             nDestX              = 0;
    int             nDestY              = 0;
    int             nDestCellX          = 0;
    int             nDestCellY          = 0;
    KCell*          pDestCell           = NULL;
    int             nDestRegionX        = 0;
    int             nDestRegionY        = 0;
    KRegion*        pDestRegion         = NULL;
	int				nFootZ			    = 0;
    KCell*          pCellNode           = NULL;
    int             nRelativeZ          = 0;
    int             nDeltaX             = 0;
    int             nDeltaY             = 0;
    int             nClimbAbility       = 0;

    ASSERT(m_pCell);
    ASSERT(m_pRegion);
    ASSERT(m_pScene);

    nDeltaX         = nStepLength * g_Cos(nDirection) / SIN_COS_NUMBER;
    nDeltaY         = nStepLength * g_Sin(nDirection) / SIN_COS_NUMBER;
      
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
    if (pDestRegion)
    {
        nRetCode = m_pOwner->m_Eyeshot.IsVisible(pDestRegion);
    }
    if (!pDestRegion || !nRetCode)
    {
        KGLOG_PROCESS_ERROR(m_dwID != m_pOwner->m_pRole->m_dwID);

        if (g_IsPlayer(m_dwID))
		{
			nRetCode = g_SO3World.RemovePlayer((KPlayer*)this);
			KGLOG_PROCESS_ERROR(nRetCode);

			nRetCode = m_pOwner->m_Eyeshot.DeletePlayer((KPlayer*)this);
			KGLOG_PROCESS_ERROR(nRetCode);
		}
		else
		{
			nRetCode = g_SO3World.RemoveNpc((KNpc*)this);
			KGLOG_PROCESS_ERROR(nRetCode);

			nRetCode = m_pOwner->m_Eyeshot.DeleteNpc((KNpc*)this);
			KGLOG_PROCESS_ERROR(nRetCode);
		}
        nResult = mrDeleteSelf;
        goto Exit0;
    }

	nFootZ = m_pCell->m_wHighLayer * POINT_PER_ALTITUDE + m_nZ;

    if (g_IsPlayer(m_dwID))
    {
        nClimbAbility = CLIMB_ABILITY;
        if (m_pCell->m_BaseInfo.dwCellType == ctWater)
        {
            nClimbAbility = CLIMB_ABILITY * 2;
        }
    }
    else
    {
        nClimbAbility = CLIMB_ABILITY * 3;
    }

	pDestCell = pDestRegion->GetAdaptedCellForObj(
        nDestCellX, nDestCellY, nFootZ, nClimbAbility, m_nHeight, 
        &nRelativeZ, pnObstacleDirection
    );
    KG_PROCESS_ERROR_RET_CODE(pDestCell, mrObstacle);

	m_nX = nDestX;
	m_nY = nDestY;

    m_nZ = nRelativeZ;

	m_nXCell = nDestCellX;
	m_nYCell = nDestCellY;

	if (pDestRegion != m_pRegion)
	{
		nRetCode = ChangeRegion(pDestRegion);
		KGLOG_PROCESS_ERROR(nRetCode);
	}

    if (pDestCell != m_pCell)
    {
	    nRetCode = ChangeCell(pDestCell);
	    KGLOG_PROCESS_ERROR(nRetCode);
    }

Exit1:
    nResult = mrSucceed;
Exit0:        
    return nResult;
}


void KCharacter::ProcessVerticalMove()
{
    KCell*    pCellNode           = NULL;
    KCell*    pNextCell           = NULL;
    int       nWaterline          = 0;
    int       nEarthPos           = 0;
    int       nFootPos            = 0;
    int       nGradient           = 0;
    int       nSwimWaterline      = m_nHeight * 3 / 4;

    ASSERT(m_pCell);

    nGradient = m_pCell->GetGradientDegree();

    // 取得吃水深度
    nWaterline = GetWaterline();
    // 取得地表高度
    nEarthPos  = GetEarthPos();
    // 计算自己的绝对Z坐标
    nFootPos   = m_pCell->m_wHighLayer * POINT_PER_ALTITUDE + m_nZ;

    if (m_pCell->m_BaseInfo.dwCellType == ctWater)
    {
        if (m_nZ > 0)
        {
            m_nVelocityZ -= m_nCurrentGravity;
        }
        else
        {
            if (nFootPos > nEarthPos)
            {
                // 在游泳状态是不考虑重力加速度的,这里判断吃水深度,是要保证人物
                // 能够沉到游泳临界水深以下,否则人物就会悬浮,无法切换为游泳
                if (nWaterline <= nSwimWaterline && m_eMoveState != cmsOnSwim)
                {
                    m_nVelocityZ -= m_nCurrentGravity / 8;
                }
            }
        }
    }
    else
    {
        if (m_nZ > 0)
        {
            m_nVelocityZ -= m_nCurrentGravity;
        }
    }

    MAKE_IN_RANGE(m_nVelocityZ, MIN_VELOCITY_Z, MAX_VELOCITY_Z);

    if (m_eMoveState == cmsOnSwim)
    {
        // 游泳的时候不能游到半身高水深以上(在表现上看起来还是可以在水面上游泳的)
        if (m_nZ + m_nVelocityZ > -m_nHeight / 2 && m_nVelocityZ > 0)
        {
            m_nVelocityZ = 0;
        }
    }

    m_nZ += m_nVelocityZ;

	// ---- 碰顶检测 ---------------------------------
    // 取下一个非水的Cell

    pCellNode = m_pCell->m_pNext;
    while (pCellNode)
    {
        if (pCellNode->m_BaseInfo.dwCellType != ctWater)
        {
            pNextCell = pCellNode;
            break;
        }
        pCellNode = pCellNode->m_pNext;
    }

	if (pNextCell)
	{
		int nMaxZ   = pNextCell->m_wLowLayer * POINT_PER_ALTITUDE;
        int nMyTop  = m_pCell->m_wHighLayer * POINT_PER_ALTITUDE + m_nZ + m_nHeight;

        // 碰顶
		if (nMyTop > nMaxZ)
		{
			m_nZ         = nMaxZ - m_nHeight - m_pCell->m_wHighLayer * POINT_PER_ALTITUDE;
			m_nVelocityZ = 0;
		}	
	}

    // --- 游泳状态检测 ------------------------------

    if (nWaterline >= m_nHeight)
    {
        // 入水开始计时...
        if (m_eMoveState != cmsOnDeath)
        {
            // 死亡状态下就不要加计数器了
            if (m_nDivingCount > m_nDivingFrame && m_nDivingFrame > 0)
            {
                m_nCurrentLife = m_nMaxLife * (2 * m_nDivingFrame - m_nDivingCount) / m_nDivingFrame;
                #ifdef _SERVER
                if (m_eMoveState != cmsOnDeath && m_nCurrentLife <= 0)
                {
                    Die(this);
                }
                #endif
            }
            m_nDivingCount++;
        }
    }
    else
    {
        if (m_nDivingCount > 0)
        {
            m_nDivingCount -= 2;
        }

        if (m_pCell->m_BaseInfo.dwCellType != ctWater)
        {
            m_nDivingCount = 0;
        }
    }

    // 游泳状态切换
    switch (m_eMoveState)
    {
    case cmsOnStand:
    case cmsOnWalk:
    case cmsOnRun:
        if (nWaterline >= nSwimWaterline)
        {
            m_nConvergenceSpeed = 0;
            m_eMoveState        = cmsOnSwim;
        }
        break;

    case cmsOnJump:
        if (m_nVelocityZ < 0 && nWaterline >= nSwimWaterline)
        {
            m_nConvergenceSpeed = 0;
            m_nVelocityZ        = 0;
            m_eMoveState        = cmsOnSwim;
        }
        break;

    case cmsOnSwim:
        nWaterline = GetWaterline();
        if (nWaterline < m_nHeight / 2)
        {
            m_nConvergenceSpeed = 0;
            m_eMoveState        = cmsOnStand;
        }
        break;
    default:
        break;
    }

    // --------- 检查悬空状态 -------------------------

    // 在走/跑状态时,如果悬空超过3米,自动转为跳跃状态
    if (m_eMoveState == cmsOnWalk || m_eMoveState == cmsOnRun || m_eMoveState == cmsOnSwim || m_eMoveState == cmsOnStand)
    {
        if (m_nZ > XYPOINT_TO_ZPOINT(CELL_LENGTH * 2 * 3))
        {
            m_eMoveState    = cmsOnJump;   
            m_nJumpCount++;
        }
    }

    // ------------------------------------------------



    
    // 落地检测: 下落的时候速度(m_nVelocityZ)是负值
    if (m_pCell->m_wHighLayer * POINT_PER_ALTITUDE + m_nZ <= nEarthPos)
    {	    
        if (m_eMoveState == cmsOnJump)
        {
            m_nConvergenceSpeed = 0;
            m_nVelocityXY       = 0;
            m_eMoveState        = cmsOnStand;
            //cprintf("jump to stand !\n");
        }

        m_nJumpCount    = 0;
		m_nZ            = nEarthPos - m_pCell->m_wHighLayer * POINT_PER_ALTITUDE;

	    m_nVelocityZ    = 0;
	}

Exit0:
	return;
}


int  KCharacter::GetWaterline()
{
    int     nResult     = 0;
    int     nFootPosZ   = 0;

    ASSERT(m_pCell);
    KG_PROCESS_ERROR(m_pCell->m_BaseInfo.dwCellType == ctWater);

    nFootPosZ = m_pCell->m_wHighLayer * POINT_PER_ALTITUDE + m_nZ;

    KG_PROCESS_ERROR(nFootPosZ >= m_pCell->m_wLowLayer * POINT_PER_ALTITUDE);

    KG_PROCESS_ERROR(nFootPosZ < m_pCell->m_wHighLayer * POINT_PER_ALTITUDE);

    nResult = m_pCell->m_wHighLayer * POINT_PER_ALTITUDE - nFootPosZ; // -m_nZ
Exit0:
    ASSERT(nResult >= 0);
    return nResult;
}

int KCharacter::GetEarthPos()
{
    int nResult     = 0;
    int nEarthPosZ  = 0;

    ASSERT(m_pCell);
    ASSERT(m_pRegion);

    nEarthPosZ = m_pCell->m_wHighLayer * POINT_PER_ALTITUDE;
    if (m_pCell->m_BaseInfo.dwCellType == ctWater)
    {
        KCell* pCellNode = m_pRegion->GetLowestObstacle(m_nXCell, m_nYCell);
        KGLOG_PROCESS_ERROR(pCellNode);
        while (pCellNode && pCellNode->m_pNext != m_pCell)
        {
            pCellNode = pCellNode->m_pNext;
        }
        KGLOG_PROCESS_ERROR(pCellNode);
        nEarthPosZ = pCellNode->m_wHighLayer * POINT_PER_ALTITUDE;
    }

    nResult = nEarthPosZ;
Exit0:
    return nResult;
}

int KCharacter::GetSubjectiveDirection()
{
    if (m_nConvergenceSpeed > 0)
    {
         m_nSubjectiveMoveDirection = g_GetDirection(m_nX, m_nY, m_nDestX, m_nDestY);
    }
    return m_nSubjectiveMoveDirection;
}




int g_GetRelation(KCharacter* pSrcCharacter, KCharacter* pDstCharacter)
{
	int nResult = sortNone;

	KGLOG_PROCESS_ERROR(pSrcCharacter);
	KGLOG_PROCESS_ERROR(pDstCharacter);

	if (pSrcCharacter->m_eKind == ckPlayer)
	{
		KPlayer* pSrcPlayer = dynamic_cast<KPlayer*>(pSrcCharacter);
		KGLOG_PROCESS_ERROR(pSrcPlayer);
			
		if (pDstCharacter->m_eKind == ckPlayer)
		{
			KPlayer* pDstPlayer = dynamic_cast<KPlayer*>(pDstCharacter);
			KGLOG_PROCESS_ERROR(pDstPlayer);

			nResult = pSrcPlayer->GetPlayerRelation(pDstPlayer);
		}
		else
		{
			KNpc* pDstNpc = dynamic_cast<KNpc*>(pDstCharacter);
			KGLOG_PROCESS_ERROR(pDstNpc);

			nResult = pSrcPlayer->GetNpcRelation(pDstNpc);
		}
	}
	else
	{
		KNpc* pSrcNpc = dynamic_cast<KNpc*>(pSrcCharacter);
		KGLOG_PROCESS_ERROR(pSrcNpc);

		if (pDstCharacter->m_eKind == ckPlayer)
		{
			KPlayer* pDstPlayer = dynamic_cast<KPlayer*>(pDstCharacter);
			KGLOG_PROCESS_ERROR(pDstPlayer);

			nResult = pSrcNpc->GetPlayerRelation(pDstPlayer);
		}
		else
		{
			KNpc* pDstNpc = dynamic_cast<KNpc*>(pDstCharacter);
			KGLOG_PROCESS_ERROR(pDstNpc);

			nResult = pSrcNpc->GetNpcRelation(pDstNpc);
		}
	}

Exit0:
	return nResult;
}

