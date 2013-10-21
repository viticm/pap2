#include "stdafx.h"
#include "KSO3World.h"

#ifdef _PERFORMANCE_OPTIMIZATION
    DWORD g_dwGameFps = 16;
#endif

BOOL KWorldSettings::Init(void)
{
    BOOL bResult                        = false;
	BOOL bRetCode                       = false;
    BOOL bConstListInitFlag             = false;
    BOOL bNpcTemplateListInitFlag       = false;
    BOOL bDoodadTemplateListInitFlag    = false;
    BOOL bQuestInfoListInitFlag         = false;
    BOOL bNpcClassListInitFlag          = false;
    BOOL bDoodadClassListInitFlag       = false;
    BOOL bPatrolPathListInitFlag        = false;
    BOOL bLevelUpListInitFlag           = false;
    BOOL bCoolDownListInitFlag          = false;
    BOOL bWeaponTypeListInitFlag        = false;
    BOOL bReputeLimitInitFlag           = false;
    BOOL bCharacterActionListInitFlag   = false;
    BOOL bSmartDialogListInitFlag       = false;
    BOOL bOrderManagerInitFlag          = false;
    BOOL bNpcTeamListInitFlag           = false;
    BOOL bAchievementInfoListFlag       = false;
    BOOL bDesignationListInitFlag       = false;
    BOOL bGameCardInfoListInitFlag      = false;

	bRetCode = m_ConstList.Init();
	KGLOG_PROCESS_ERROR(bRetCode);
    bConstListInitFlag = true;

	bRetCode = m_NpcTemplateList.Init();
	KGLOG_PROCESS_ERROR(bRetCode);
    bNpcTemplateListInitFlag = true;

	bRetCode = m_DoodadTemplateList.Init();
	KGLOG_PROCESS_ERROR(bRetCode);
    bDoodadTemplateListInitFlag = true;

	bRetCode = m_MapListFile.Init();
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = m_QuestInfoList.Init();
	KGLOG_PROCESS_ERROR(bRetCode);
    bQuestInfoListInitFlag = true;

	bRetCode = m_NpcClassList.Init();
	KGLOG_PROCESS_ERROR(bRetCode);
    bNpcClassListInitFlag = true;

	bRetCode = m_DoodadClassList.Init();
	KGLOG_PROCESS_ERROR(bRetCode);
    bDoodadClassListInitFlag = true;

	bRetCode = m_PatrolPathList.Init();
	KGLOG_PROCESS_ERROR(bRetCode);
    bPatrolPathListInitFlag = true;

	bRetCode = m_LevelUpList.Init();
	KGLOG_PROCESS_ERROR(bRetCode);
    bLevelUpListInitFlag = true;

    bRetCode = m_CoolDownList.Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bCoolDownListInitFlag = true;

    bRetCode = m_WeaponTypeList.Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bWeaponTypeListInitFlag = true;

	bRetCode = m_RelationCampList.Init(RELATION_CAMP_FILE_NAME);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = m_RelationForceList.Init(RELATION_FORCE_FILE_NAME);
	KGLOG_PROCESS_ERROR(bRetCode);
    
	bRetCode = m_ForceDefaultList.Init();
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = m_ReputeLimit.Init();
	KGLOG_PROCESS_ERROR(bRetCode);
    bReputeLimitInitFlag = true;

	bRetCode = m_CharacterActionList.Init();
	KGLOG_PROCESS_ERROR(bRetCode);
    bCharacterActionListInitFlag = true;

	bRetCode = m_SmartDialogList.Init();
	KGLOG_PROCESS_ERROR(bRetCode);
    bSmartDialogListInitFlag = true;

#ifdef _SERVER
    bRetCode = m_GMList.Load();
    KGLOG_PROCESS_ERROR(bRetCode);
#endif

	bRetCode = m_OrderManager.Init();
	KGLOG_PROCESS_ERROR(bRetCode);
    bOrderManagerInitFlag = true;

	bRetCode = m_NpcTeamList.Init();
	KGLOG_PROCESS_ERROR(bRetCode);
    bNpcTeamListInitFlag = true;

    bRetCode = m_AchievementInfoList.Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bAchievementInfoListFlag = true;

    bRetCode = m_DesignationList.Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bDesignationListInitFlag = true;

    bRetCode = m_GameCardInfoList.Init();
    //KGLOG_PROCESS_ERROR(bRetCode);
    //bGameCardInfoListInitFlag = true;

    bResult = true;
Exit0:
    if (!bResult)
    {
        if (bGameCardInfoListInitFlag)
        {
            m_GameCardInfoList.UnInit();
            bGameCardInfoListInitFlag = false;
        }

        if (bDesignationListInitFlag)
        {
            m_DesignationList.UnInit();
            bDesignationListInitFlag = false;
        }

        if (bAchievementInfoListFlag)
        {
            m_AchievementInfoList.UnInit();
            bAchievementInfoListFlag = false;
        }

        if (bNpcTeamListInitFlag)
        {
            m_NpcTeamList.UnInit();
            bNpcTeamListInitFlag = false;
        }

        if (bOrderManagerInitFlag)
        {
            m_OrderManager.UnInit();
            bOrderManagerInitFlag = false;
        }
      
        if (bSmartDialogListInitFlag)
        {
            m_SmartDialogList.UnInit();
            bSmartDialogListInitFlag = false;
        }

        if (bCharacterActionListInitFlag)
        {
            m_CharacterActionList.UnInit();
            bCharacterActionListInitFlag = false;
        }

        if (bReputeLimitInitFlag)
        {
            m_ReputeLimit.UnInit();
            bReputeLimitInitFlag = false;
        }

        if (bWeaponTypeListInitFlag)
        {
            m_WeaponTypeList.UnInit();
            bWeaponTypeListInitFlag = false;
        }

        if (bCoolDownListInitFlag)
        {
            m_CoolDownList.UnInit();
            bCoolDownListInitFlag = false;
        }

        if (bLevelUpListInitFlag)
        {
            m_LevelUpList.UnInit();
            bLevelUpListInitFlag = false;
        }

        if (bPatrolPathListInitFlag)
        {
            m_PatrolPathList.UnInit();
            bPatrolPathListInitFlag = false;
        }

        if (bDoodadClassListInitFlag)
        {
            m_DoodadClassList.UnInit();
            bDoodadClassListInitFlag = false;
        }

        if (bNpcClassListInitFlag)
        {
            m_NpcClassList.UnInit();
            bNpcClassListInitFlag = false;
        }

        if (bQuestInfoListInitFlag)
        {
            m_QuestInfoList.UnInit();
            bQuestInfoListInitFlag = false;
        }
        
        if (bDoodadTemplateListInitFlag)
        {
            m_DoodadTemplateList.UnInit();
            bDoodadTemplateListInitFlag = false;
        }

        if (bNpcTemplateListInitFlag)
        {
            m_NpcTemplateList.UnInit();
            bNpcTemplateListInitFlag = false;
        }

        if (bConstListInitFlag)
        {
            m_ConstList.UnInit();
            bConstListInitFlag = false;
        }
    }
	return bResult;
}

BOOL KWorldSettings::UnInit(void)
{
	m_NpcTeamList.UnInit();
	m_OrderManager.UnInit();

	m_SmartDialogList.UnInit();
	m_CharacterActionList.UnInit();
    m_ReputeLimit.UnInit();
    m_WeaponTypeList.UnInit();
    m_CoolDownList.UnInit();
    m_LevelUpList.UnInit();
	m_DoodadTemplateList.UnInit();
	m_NpcTemplateList.UnInit();
	m_QuestInfoList.UnInit();
	m_DoodadClassList.UnInit();
	m_NpcClassList.UnInit();
	m_PatrolPathList.UnInit();
	m_ConstList.UnInit();
	m_AchievementInfoList.UnInit();
    m_DesignationList.UnInit();
    m_GameCardInfoList.UnInit();

	return true;
}
