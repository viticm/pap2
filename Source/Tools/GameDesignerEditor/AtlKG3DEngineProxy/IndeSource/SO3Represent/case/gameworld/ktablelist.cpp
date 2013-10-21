#include "stdafx.h"
#include "SO3World/IKGSO3WorldClient.h"
#include "../../../SO3World/KPlayer.h"
#include "../../case/actionobject/krldoodad.h"
#include "../../case/actionobject/krlcharacter.h"
#include "../../SO3Represent.h"
#include "./ktablelist.h"
#include "./ktabletransform.h"

namespace 
{
    enum { FILE_NAME_LEN = 64 };
    enum { BODY_TYPE_FILE_NAME_LEN = 8 };

    static LPCTSTR g_aszRidesSocketName[MODEL_RIDES_SOCKET_END] =
    {
        _T("S_Hs"),         //MODEL_RIDES_SOCKET_MAIN 
        _T("S_Head"),		//MODEL_RIDES_SOCKET_1,
        _T("S_Chest"),		//MODEL_RIDES_SOCKET_2,
        _T("S_Saddle"),		//MODEL_RIDES_SOCKET_3,
        _T("S_Foot"),		//MODEL_RIDES_SOCKET_4,
    };

    static LPCTSTR g_aszNpcSocketName[RL_NPC_SOCKET_COUNT] =
    {
        _T("S_Face"),       // RL_NPC_SOCKET_FACE
        _T("S_Hat"),        // RL_NPC_SOCKET_HAT,
        _T("S_LH"),			// RL_NPC_SOCKET_LH,
        _T("S_LP"),			// RL_NPC_SOCKET_LP,
        _T("S_LC"),			// RL_NPC_SOCKET_LC
        _T("S_RH"),			// RL_NPC_SOCKET_RH,
        _T("S_RP"),			// RL_NPC_SOCKET_RP,
        _T("S_RC"),			// RL_NPC_SOCKET_RC
        _T("S_Long"),		// RL_NPC_SOCKET_LONG,
        _T("S_Spine"),		// RL_NPC_SOCKET_SPINE,
        _T("S_Spine2"),		// RL_NPC_SOCKET_SPINE2,
    };

    TCHAR g_szPlayerSkillCasterConfigFiles[rtTotal][FILE_NAME_LEN];
    TCHAR g_szPlayerAnimationFiles[rtTotal][FILE_NAME_LEN];
    TCHAR g_szPlayerAnimationAdjustFiles[rtTotal][FILE_NAME_LEN];
    TCHAR g_szPlayerAnimationTerrainAdjustFiles[rtTotal][FILE_NAME_LEN];
    TCHAR g_szPlayerAnimationFusionFiles[rtTotal][FILE_NAME_LEN];
    TCHAR g_szPlayerAnimationIndexFiles[rtTotal][FILE_NAME_LEN];
    TCHAR g_szPlayerRandomAnimationFiles[rtTotal][FILE_NAME_LEN];
    TCHAR g_szPlayerSequenceAnimationFiles[rtTotal][FILE_NAME_LEN];
    TCHAR g_szPlayerModelSFXFile[rtTotal][CHARACTER_EQUIP_SFX_FILE_COUNT][FILE_NAME_LEN];
    TCHAR g_szPlayerModelFiles[rtTotal][CHARACTER_EQUIP_FILE_COUNT][FILE_NAME_LEN];

    TCHAR g_szNpcSkillCasterConfigFile[FILE_NAME_LEN];
    TCHAR g_szMissilePhaseTypes[RL_MISSILE_PHASE_TYPE_COUNT][FILE_NAME_LEN];
    TCHAR g_szMissileParamTypes[RL_MISSILE_PARAM_TYPE_COUNT][FILE_NAME_LEN];
    TCHAR g_szRidesEquip[MODEL_RIDES_EQUIP_COUNT][FILE_NAME_LEN];
    TCHAR g_szRidesSocket[MODEL_RIDES_SOCKET_COUNT][FILE_NAME_LEN];
    TCHAR g_szWeaponName[RL_WEAPON_COUNT][FILE_NAME_LEN];
    TCHAR g_szAnimationSectionName[RL_ANIMATION_TABLE_COUNT][FILE_NAME_LEN];
    TCHAR g_szAniTableFileName[RL_SCHOOL_COUNT][FILE_NAME_LEN];
    TCHAR g_szSheathType[RL_SHEATH_COUNT][FILE_NAME_LEN];
    TCHAR g_szSheathSocketLH[RL_SHEATH_COUNT][FILE_NAME_LEN];
    TCHAR g_szSheathSocketRH[RL_SHEATH_COUNT][FILE_NAME_LEN];
}

KTableList::KTableList()
{
}

KTableList::~KTableList()
{
}

BOOL KTableList::Init()
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;
    LPCTSTR szFilePath = NULL;

    // Sound Table
    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_COMMON_SOUND_TABLE);
    KGLOG_PROCESS_ERROR(szFilePath);

    nRetCode = m_SoundDataTab.Load(szFilePath, "iffs");
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pSoundModel = (KRLSoundModel*)m_SoundDataTab.Data(m_nSoundlCount);
    KGLOG_PROCESS_ERROR(m_pSoundModel);

    // Relation Model
    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_COMMON_FORCE_RELATION_TABLE);
    KGLOG_PROCESS_ERROR(szFilePath);

    nRetCode = m_RelationTab.Load(szFilePath, "isifs");
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pRelationModel = (KForceRelationModel*)m_RelationTab.Data(m_nRelationCount);
    KGLOG_PROCESS_ERROR(m_pRelationModel && m_nRelationCount == RL_FORCE_RELATION_COUNT);

    // Global Height Model
    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_COMMON_NUMBER);
    KGLOG_PROCESS_ERROR(szFilePath);

    nRetCode = m_NumberTab.Load(szFilePath, "isf");
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pNumberModel = (KRLNumberModel*)m_NumberTab.Data(m_nNumberCount);
    KGLOG_PROCESS_ERROR(m_pNumberModel);

    // Global Effect Model
    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_COMMON_GLOBALL_EFFECT);
    KGLOG_PROCESS_ERROR(szFilePath);

    nRetCode = m_GlobalEffectTab.Load(szFilePath, "isiffs");
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pGlobalEffectModel = (KGlobalEffectModel*)m_GlobalEffectTab.Data(m_nGlobalEffectCount);
    KGLOG_PROCESS_ERROR(m_pGlobalEffectModel);

    // Cursor Effect Model
    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_COMMON_CURSOR_EFFECT);
    KGLOG_PROCESS_ERROR(szFilePath);

    nRetCode = m_CursorEffectTab.Load(szFilePath, "isiffs");
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pCursorEffectModel = (KRLCursorEffectModel*)m_CursorEffectTab.Data(m_nCursorEffectCount);
    KGLOG_PROCESS_ERROR(m_pCursorEffectModel);

    // Missile Config
    nRetCode = LoadMissileConfig();
    KGLOG_PROCESS_ERROR(nRetCode);
    
    // Line Missile
    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_MISSILE_DIRECTORY, g_szMissilePhaseTypes[RL_MISSILE_PHASE_TYPE_LINE]);
    KGLOG_PROCESS_ERROR(szFilePath);

    nRetCode = m_PhaseLineTab.Load(szFilePath, "isfffissssiiiiiiissiiiiiiiifffii");
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pPhaseLineModel = (KMissilePhaseLineModel*)m_PhaseLineTab.Data(m_nPhaseLineCount);
    KGLOG_PROCESS_ERROR(m_pPhaseLineModel);

    // Circle Missile
    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_MISSILE_DIRECTORY, g_szMissilePhaseTypes[RL_MISSILE_PHASE_TYPE_CIRCLE]);
    KGLOG_PROCESS_ERROR(szFilePath);

    nRetCode = m_PhaseCircleTab.Load(szFilePath, "isfffissssiiiiiiissiiiiiiiiiiiiffii");
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pPhaseCircleModel = (KMissilePhaseCircleModel*)m_PhaseCircleTab.Data(m_nPhaseCircleCount);
    KGLOG_PROCESS_ERROR(m_pPhaseCircleModel);

    // FixedTrack Missile
    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_MISSILE_DIRECTORY, g_szMissilePhaseTypes[RL_MISSILE_PHASE_TYPE_FIXED_TRACK]);
    KGLOG_PROCESS_ERROR(szFilePath);

    nRetCode = m_PhaseFixedTrackTab.Load(szFilePath, "isfffissssiiiiiiissiiiiiiiii");
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pPhaseFixedTrackModel = (KMissilePhaseFixedTrackModel*)m_PhaseFixedTrackTab.Data(m_nPhaseFixedTrackCount);
    KGLOG_PROCESS_ERROR(m_pPhaseFixedTrackModel);

    // Lightning Missile
    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_MISSILE_DIRECTORY, g_szMissilePhaseTypes[RL_MISSILE_PHASE_TYPE_LIGHTNING]);
    KGLOG_PROCESS_ERROR(szFilePath);

    nRetCode = m_PhaseFulminationTab.Load(szFilePath, "isfffissssiiiiiiissif");
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pPhaseFulminationModel = (KMissilePhaseFulminationModel*)m_PhaseFulminationTab.Data(m_nPhaseFulminationCount);
    KGLOG_PROCESS_ERROR(m_pPhaseFulminationModel);

    // Missile model param
    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_MISSILE_DIRECTORY, g_szMissileParamTypes[RL_MISSILE_PARAM_TYPE_MODEL]);
    KGLOG_PROCESS_ERROR(szFilePath);

    nRetCode = m_MissileModelParamTab.Load(szFilePath, "isfffiissssi");
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pMissileModelParam = (KRLMissileModelParam*)m_MissileModelParamTab.Data(m_nMissileModelParamCount);
    KGLOG_PROCESS_ERROR(m_pMissileModelParam);

    // Missile homing param
    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_MISSILE_DIRECTORY, g_szMissileParamTypes[RL_MISSILE_PARAM_TYPE_HOMING]);
    KGLOG_PROCESS_ERROR(szFilePath);

    nRetCode = m_MissileHomingParamTab.Load(szFilePath, "isssffff");
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pMissileHomingParam = (KRLMissileHomingParam*)m_MissileHomingParamTab.Data(m_nMissileHomingParamCount);
    KGLOG_PROCESS_ERROR(m_pMissileHomingParam);

    // Missile timeline param
    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_MISSILE_DIRECTORY, g_szMissileParamTypes[RL_MISSILE_PARAM_TYPE_TIMELINE]);
    KGLOG_PROCESS_ERROR(szFilePath);

    nRetCode = m_MissileTimelineParamTab.Load(szFilePath, "isf");
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pMissileTimelineParam = (KRLMissileTimelineParam*)m_MissileTimelineParamTab.Data(m_nMissileTimelineParamCount);
    KGLOG_PROCESS_ERROR(m_pMissileTimelineParam);

    // Missile y axis param
    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_MISSILE_DIRECTORY, g_szMissileParamTypes[RL_MISSILE_PARAM_TYPE_TIMELINE]);
    KGLOG_PROCESS_ERROR(szFilePath);

    nRetCode = m_MissileYAxisParamTab.Load(szFilePath, "isf");
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pMissileYAxisParam = (KRLMissileYAxisParam*)m_MissileYAxisParamTab.Data(m_nMissileYAxisParamCount);
    KGLOG_PROCESS_ERROR(m_pMissileYAxisParam);

    // Missile Model
    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_SKILL_MISSILE_TABLE);
    KGLOG_PROCESS_ERROR(szFilePath);

    nRetCode = m_MissileTab.Load(szFilePath, "isiiiiiiiiiiii");
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pMissileModel = (KRLMissileModel*)m_MissileTab.Data(m_nMissileCount);
    KGLOG_PROCESS_ERROR(m_pMissileModel);

    // Skill Buff
    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_SKILL_BUFF_MODEL);
    KGLOG_PROCESS_ERROR(szFilePath);

    nRetCode = m_SkillBuffTab.Load(szFilePath, "iisssffffffiis");
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pSkillBuffModel = (KSkillBuffModel*)m_SkillBuffTab.Data(m_nSkillBuffCount);
    KGLOG_PROCESS_ERROR(m_pSkillBuffModel);

    // Skill Effect
    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_SKILL_EFFECT_MODEL);
    KGLOG_PROCESS_ERROR(szFilePath);

    nRetCode = m_SkillEffectTab.Load(szFilePath, "isiifss");
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pSkillEffectModel = (KSkillEffectModel*)m_SkillEffectTab.Data(m_nSkillEffectCount);
    KGLOG_PROCESS_ERROR(m_pSkillEffectModel);

    // Skill Result
    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_SKILL_RESULT_TABLE);
    KGLOG_PROCESS_ERROR(szFilePath);

    nRetCode = m_SkillResultTab.Load(szFilePath, "iiis");
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pSkillResultModel = (KSkillResultModel*)m_SkillResultTab.Data(m_nSkillResultCount);
    KGLOG_PROCESS_ERROR(m_pSkillResultModel);

    // Skill Chain
    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_SKILL_CHAIN_MODEL);
    KGLOG_PROCESS_ERROR(szFilePath);

    nRetCode = m_SkillChainTab.Load(szFilePath, "iisss");
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pSkillChainModel = (KSkillChainModel*)m_SkillChainTab.Data(m_nSkillChainCount);
    KGLOG_PROCESS_ERROR(m_pSkillChainModel);

    // Skill Caster
    hr = LoadSkillCastModel();
    KGLOG_COM_PROCESS_ERROR(hr);
    
    // Doodad Model
    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_DOODAD_MODEL);
    KGLOG_PROCESS_ERROR(szFilePath);

    nRetCode = m_DoodadTab.Load(szFilePath, "iiiiiiiiiiffiiisssissssissssissssissssissssissssissssissssissssissssis");
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pDoodadModel = (KRLDoodadModel*)m_DoodadTab.Data(m_nDoodadCount);
    KGLOG_PROCESS_ERROR(m_pDoodadModel);

    // Doodad Effect
    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_DOODAD_EFFECT_MODEL);
    KGLOG_PROCESS_ERROR(szFilePath);

    nRetCode = m_DoodadEffectTab.Load(szFilePath, "isfifs");
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pDoodadEffectModel = (KDoodadEffectModel*)m_DoodadEffectTab.Data(m_nDoodadEffectCount);
    KGLOG_PROCESS_ERROR(m_pDoodadEffectModel);

    // Ride Model
    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_RIDES_MODEL);
    KGLOG_PROCESS_ERROR(szFilePath);

    nRetCode = m_RidesTab.Load(szFilePath, "ifisfffiiissiissii");
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pRidesModel = (KRidesModel*)m_RidesTab.Data(m_nRidesCount);
    KGLOG_PROCESS_ERROR(m_pRidesModel);

    // Rides Equip and Socket
    hr = LoadRidesEquipAndSocketModel();
    KGLOG_COM_PROCESS_ERROR(hr);
    
    // Rides Animation
    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_RIDES_ANIMATION_MODEL);
    KGLOG_PROCESS_ERROR(szFilePath);

    nRetCode = m_RidesAnimationTab.Load(szFilePath, "iiififfsss");
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pRidesAnimationModel = (KRidesAnimationModel*)m_RidesAnimationTab.Data(m_nRidesAnimationCount);
    KGLOG_PROCESS_ERROR(m_pRidesAnimationModel);

    // Rides Animation Fusion
    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_RIDES_ANIMATION_FUSION);
    KGLOG_PROCESS_ERROR(szFilePath);

    nRetCode = m_RidesFusionTab.Load(szFilePath, "iiisi");
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pRidesFusionModel = (KRidesAnimationFusion*)m_RidesFusionTab.Data(m_nRidesFusionCount);
    KGLOG_PROCESS_ERROR(m_pRidesFusionModel);

    // Rides Road
    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_RIDES_FOR_ROAD);
    KGLOG_PROCESS_ERROR(szFilePath);

    nRetCode = m_RidesTrackTab.Load(szFilePath, "ii");
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pRidesTrackModel = (KRidesForTrack*)m_RidesTrackTab.Data(m_nRidesTrackCount);
    KGLOG_PROCESS_ERROR(m_pRidesTrackModel);

    // NPC Model
    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_NPC_MODEL);
    KGLOG_PROCESS_ERROR(szFilePath);

    nRetCode = m_NpcTab.Load(szFilePath, "issiffisssssssssssssssssssssssiiiiffffiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii");
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pNpcModel = (KRLNpcModel*)m_NpcTab.Data(m_nNpcCount);
    KGLOG_PROCESS_ERROR(m_pNpcModel);

    KRLNpcModel::Init(m_pNpcModel, m_nNpcCount);

    // NPC Animation
    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_NPC_ANIMATION_MODEL);
    KGLOG_PROCESS_ERROR(szFilePath);

    nRetCode = m_NpcAnimationTab.Load(szFilePath, "iiiiiiiffifssissssssssssssssssssssssss");
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pNpcAnimationModel = (KNpcAnimationModel*)m_NpcAnimationTab.Data(m_nNpcAnimationCount);
    KGLOG_PROCESS_ERROR(m_pNpcAnimationModel);

    // NPC Animation Adjust
    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_NPC_ANIMATION_ADJUST_TABLE);
    KGLOG_PROCESS_ERROR(szFilePath);

    nRetCode = m_NpcAnimationAdjustTab.Load(szFilePath, "iiiiiii");
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pNpcAnimationAdjustModel = (KRLNpcAdjustAnimationIndex*)m_NpcAnimationAdjustTab.Data(m_nNpcAnimationAdjustCount);
    KGLOG_PROCESS_ERROR(m_pNpcAnimationAdjustModel);

    // NPC Animation Fusion
    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_NPC_ANIMATION_FUSION_TABLE);
    KGLOG_PROCESS_ERROR(szFilePath);

    nRetCode = m_NpcFusionTab.Load(szFilePath, "iisi");
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pNpcFusionModel = (KRLAnimationFusion*)m_NpcFusionTab.Data(m_nNpcFusionCount);
    KGLOG_PROCESS_ERROR(m_pNpcFusionModel);

    // NPC Serial Animation
    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_NPC_SERIAL_ANIMATION_TABLE);
    KGLOG_PROCESS_ERROR(szFilePath);

    nRetCode = m_NpcSerialTab.Load(szFilePath, "isiii");
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pNpcSerialModel = (KNpcSerialAnimation*)m_NpcSerialTab.Data(m_nNpcSerialCount);
    KGLOG_PROCESS_ERROR(m_pNpcSerialModel);

    // NPC Animation Relation Table
    hr = LoadNpcAnimationRelationTable();
    KGLOG_COM_PROCESS_ERROR(hr);

    // Player Sheath, Player Equip Model and Equip SFX Model
    nRetCode = LoadPlayerAllModel();
    KGLOG_PROCESS_ERROR(nRetCode);

    // Player Animation Ini
    hr = LoadPlayerConfigFile();
    KGLOG_COM_PROCESS_ERROR(hr);

    // Player Animation Table
    nRetCode = LoadPlayerAnimationTable();
    KGLOG_PROCESS_ERROR(nRetCode);

    // Player All Animation Model
    hr = LoadPlayerAllAnimationModel();
    KGLOG_COM_PROCESS_ERROR(hr);
    
    // Player Animation Relation Table
    hr = LoadPlayerAnimationRelationTable();
    KGLOG_COM_PROCESS_ERROR(hr);

    // Player Buff Animation Adjust Table
    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_BUFF_ANIMATION_ADJUST_TABLE);
    KGLOG_PROCESS_ERROR(szFilePath);

    nRetCode = m_PlayerBuffTab.Load(szFilePath, "iiii");
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pPlayerBuffModel = (KPlayerBuffAnimation*)m_PlayerBuffTab.Data(m_nPlayerBuffCount);
    KGLOG_PROCESS_ERROR(m_pPlayerBuffModel);

    // Player Serial Animation Table
    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_SERIAL_ANIMATION_TABLE);
    KGLOG_PROCESS_ERROR(szFilePath);

    nRetCode = m_PlayerSerialTab.Load(szFilePath, "isiii");
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pPlayerSerialModel = (KPlayerSerialAnimation*)m_PlayerSerialTab.Data(m_nPlayerSerialCount);
    KGLOG_PROCESS_ERROR(m_pPlayerSerialModel);

    // Player Craft Animation Table
    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_CRAFT_ANIMATION_TABLE);
    KGLOG_PROCESS_ERROR(szFilePath);

    nRetCode = m_PlayerCraftTab.Load(szFilePath, "ii");
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pPlayerCraftModel = (KPlayerCraftAnimation*)m_PlayerCraftTab.Data(m_nPlayerCraftCount);
    KGLOG_PROCESS_ERROR(m_pPlayerCraftModel);

    // Player Common Info Table
    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_TABLE);
    KGLOG_PROCESS_ERROR(szFilePath);

    nRetCode =  m_PlayerTab.Load(szFilePath, "isfssiffffsfsfsfsfsfsfsfsfsfsfsf");
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pPlayerModel = (KRLPlayerModel*)m_PlayerTab.Data(m_nPlayerCount);
    KGLOG_PROCESS_ERROR(m_pPlayerModel);


    // uniform table
    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_UNIFORM_TABLE);
    KGLOG_PROCESS_ERROR(szFilePath);

    nRetCode = m_PlayerUniformTab.Load(szFilePath, "isiiiiiiiiiiiiiiiiiiiiiiiiiiiii");
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pPlayerUniformModel = (KRLPlayerUniformModel*)m_PlayerUniformTab.Data(m_nPlayerUniformCount);
    KGLOG_PROCESS_ERROR(m_pPlayerUniformModel);

    // roletype uniform table
    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_ROLETYPE_UNIFORM_TABLE);
    KGLOG_PROCESS_ERROR(szFilePath);

    nRetCode =  m_PlayerRoleTypeUniformTab.Load(szFilePath, "isiiiiiiiiiiiiiiiiiiiiiiiiiiiii");
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pPlayerRoleTypeUniformModel = (KRLPlayerUniformModel*)m_PlayerRoleTypeUniformTab.Data(m_nPlayerRoleTypeUniformCount);
    KGLOG_PROCESS_ERROR(m_pPlayerRoleTypeUniformModel);

    // force uniform table
    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_FORCE_UNIFORM_TABLE);
    KGLOG_PROCESS_ERROR(szFilePath);

    nRetCode =  m_PlayerForceUniformTab.Load(szFilePath, "isiiiiiiiiiiiiiiiiiiiiiiiiiiiii");
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pPlayerForceUniformModel = (KRLPlayerUniformModel*)m_PlayerForceUniformTab.Data(m_nPlayerForceUniformCount);
    KGLOG_PROCESS_ERROR(m_pPlayerForceUniformModel);

    // npc uniform
    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_NPC_UNIFORM_MODEL);
    KGLOG_PROCESS_ERROR(szFilePath);

    nRetCode = m_NpcUniformTab.Load(szFilePath, "isi");
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pNpcUniformModel = (KRLNpcUniformModel*)m_NpcUniformTab.Data(m_nNpcUniformCount);
    KGLOG_PROCESS_ERROR(m_pNpcUniformModel);

    // doodad uniform
    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_DOODAD_UNIFORM_MODEL);
    KGLOG_PROCESS_ERROR(szFilePath);

    nRetCode = m_DoodadUniformTab.Load(szFilePath, "isi");
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pDoodadUniformModel = (KRLDoodadUniformModel*)m_DoodadUniformTab.Data(m_nDoodadUniformCount);
    KGLOG_PROCESS_ERROR(m_pDoodadUniformModel);

    // optimization uniform
    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_COMMON_OPTIMIZATION_TABLE);
    KGLOG_PROCESS_ERROR(szFilePath);

    nRetCode = m_OptimizationTab.Load(szFilePath, "iiiiiiiiiiiiiiiiiiiiiiiiiiii");
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pOptimizationModel = (KRLOptimizationModel*)m_OptimizationTab.Data(m_nOptimizationCount);
    KGLOG_PROCESS_ERROR(m_pOptimizationModel);

    nResult = true;
Exit0:
    return nResult;
}

void KTableList::Exit()
{
    m_NpcAnimationTrack.Exit();

    for (size_t nTrack = 0; nTrack < RL_ANIMATION_TRACK_COUNT; ++nTrack)
    {
        m_PlayerAnimationTracks[nTrack].Exit();
    }
}

BOOL KTableList::Reset()
{
    int nRetCode = false;

    Exit();

    nRetCode = Init();
    KGLOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    Exit();
    return FALSE;
}

DWORD KTableList::GetRidesIDByTrackID(DWORD dwTrackID) const
{
    KRidesForTrack Found = { dwTrackID };
    KRidesForTrack const* pResult = std::lower_bound(m_pRidesTrackModel, m_pRidesTrackModel + m_nRidesTrackCount, Found);
    if (pResult != m_pRidesTrackModel + m_nRidesTrackCount && pResult->dwTrackID == dwTrackID)
        return pResult->dwRidesID;
    return 0;
}

DWORD KTableList::GetPlayerAnimationIndexID(RL_SCHOOL nSchool, RL_ANIMATION_TABLE nAnimationTable, RL_WEAPON nWeapon) const
{
    return m_dwPlayerAnimationIndexIDs[nSchool][nAnimationTable][nWeapon];
}

DWORD KTableList::GetPlayerAnimationTrackType(DWORD dwKindID, RL_ANIMATION_TRACK nAnimationTrack)
{
    DWORD dwAnimationTrackType = 0;

    KGLOG_PROCESS_ERROR(dwKindID < m_PlayerAnimationTracks[nAnimationTrack].nTrackType);

    dwAnimationTrackType = m_PlayerAnimationTracks[nAnimationTrack].pTrackType[dwKindID];

Exit0:
    return dwAnimationTrackType;
}

DWORD KTableList::GetNPCAnimationTrackType(DWORD dwKindID)
{
    DWORD dwAnimationTrackType = 0;

    KGLOG_PROCESS_ERROR(dwKindID < m_NpcAnimationTrack.nTrackType);

    dwAnimationTrackType = m_NpcAnimationTrack.pTrackType[dwKindID];

Exit0:
    return dwAnimationTrackType;
}

int KTableList::GetCharacterAnimationRelation(BOOL bPlayer, DWORD dwPreviousAnimationKindID, DWORD dwCurrentAnimationKindID, RL_ANIMATION_TRACK nAnimationTrack)
{
    DWORD dwAnimationRelation = 0;

    if (bPlayer)
    {
        KGLOG_PROCESS_ERROR(dwPreviousAnimationKindID < m_PlayerAnimationTracks[nAnimationTrack].nTrackType);
        KGLOG_PROCESS_ERROR(dwCurrentAnimationKindID < m_PlayerAnimationTracks[nAnimationTrack].nTrackType);

        dwAnimationRelation = m_PlayerAnimationTracks[nAnimationTrack].pRelation[m_PlayerAnimationTracks[nAnimationTrack].nTrackType * dwPreviousAnimationKindID + dwCurrentAnimationKindID];
    }
    else
    {
        KGLOG_PROCESS_ERROR(dwPreviousAnimationKindID < m_NpcAnimationTrack.nTrackType);
        KGLOG_PROCESS_ERROR(dwCurrentAnimationKindID < m_NpcAnimationTrack.nTrackType);

        dwAnimationRelation = m_NpcAnimationTrack.pRelation[m_NpcAnimationTrack.nTrackType * dwPreviousAnimationKindID + dwCurrentAnimationKindID];
    }

Exit0:
    return dwAnimationRelation;
}

int KTableList::HitAnimationPlayRate(float fRatio) const
{
    ASSERT(fRatio >= FLT_EPSILON);

    DWORD dwMaxRatioCount = (DWORD)(fRatio * 10000);

    DWORD dwRatio = KG_Rand() % 10000;

    return dwRatio < dwMaxRatioCount;
}

float KTableList::GetNumber(DWORD dwID) const
{
    KRLNumberModel Found = { dwID };
    KRLNumberModel const* pResult = std::lower_bound(m_pNumberModel, m_pNumberModel + m_nNumberCount, Found);
    if (pResult != m_pNumberModel + m_nNumberCount && pResult->dwID == dwID)
        return pResult->fHeight;
    return 0.0f;
}

DWORD KTableList::GetRidesAnimationFusion(DWORD dwRidesRepresentID, DWORD dwPreviousAnimationID, DWORD dwCurrentAnimationID) const
{
    KRidesAnimationFusion Found = { dwRidesRepresentID, dwPreviousAnimationID, dwCurrentAnimationID };
    KRidesAnimationFusion const* pResult = std::lower_bound(m_pRidesFusionModel, m_pRidesFusionModel + m_nRidesFusionCount, Found);
    if (pResult != m_pRidesFusionModel + m_nRidesFusionCount && 
        pResult->dwRidesRepresentID == dwRidesRepresentID && 
        pResult->dwPreviousAnimationID == dwPreviousAnimationID && 
        pResult->dwCurrentAnimationID == dwCurrentAnimationID)
    {
        return pResult->dwFusionTime;
    }
    return 200;
}

float KTableList::GetNpcModelHeight(DWORD dwRepresentID) const
{
    KRLNpcModel Found = { dwRepresentID };
    KRLNpcModel const* pResult = std::lower_bound(m_pNpcModel, m_pNpcModel + m_nNpcCount, Found);
    if (pResult != m_pNpcModel + m_nNpcCount && pResult->dwRepresentID == dwRepresentID)
        return pResult->fHeight;
    return 0.0f;
}

DWORD KTableList::GetNpcAnimationFusion(DWORD dwPreviousAnimationID, DWORD dwCurrentAnimationID) const
{
    KRLAnimationFusion Found = { dwPreviousAnimationID, dwCurrentAnimationID };
    KRLAnimationFusion const* pResult = std::lower_bound(m_pNpcFusionModel, m_pNpcFusionModel + m_nNpcFusionCount, Found);
    if (pResult != m_pNpcFusionModel + m_nNpcFusionCount && 
        pResult->dwPreviousAnimationID == dwPreviousAnimationID && 
        pResult->dwCurrentAnimationID == dwCurrentAnimationID)
    {
        return pResult->dwFusionTime;
    }
    return 200;
}

DWORD KTableList::GetPlayerAnimationFusion(ROLE_TYPE nRoleType, DWORD dwPreviousAnimationID, DWORD dwCurrentAnimationID) const
{
    ASSERT(nRoleType >= 0 && nRoleType < rtTotal);

    KRLAnimationFusion Found = { dwPreviousAnimationID, dwCurrentAnimationID };
    KPlayerAnimationFusionData const& ad = m_RoleTypeData[nRoleType].AnimationFusionData;
    KRLAnimationFusion const* pResult = std::lower_bound(ad.pModel, ad.pModel + ad.nCount, Found);
    if (pResult != ad.pModel + ad.nCount && pResult->dwPreviousAnimationID == dwPreviousAnimationID && pResult->dwCurrentAnimationID == dwCurrentAnimationID)
        return pResult->dwFusionTime;
    return 200;
}

KRLSoundModel const* KTableList::GetSoundModel(DWORD dwSoundID) const
{
    KRLSoundModel Found = { dwSoundID };
    KRLSoundModel const* pResult = std::lower_bound(m_pSoundModel, m_pSoundModel + m_nSoundlCount, Found);
    if (pResult != m_pSoundModel + m_nSoundlCount && pResult->dwSoundID == dwSoundID)
        return pResult;

    KFilePath::TabError("KTableList::GetSoundModel(SoundID %u) %s\n", dwSoundID, g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_COMMON_SOUND_TABLE));
    return NULL;
}

KGlobalEffectModel const* KTableList::GetGlobalEffectModel(DWORD dwEffectID) const
{
    KGlobalEffectModel Found = { dwEffectID };
    KGlobalEffectModel const* pResult = std::lower_bound(m_pGlobalEffectModel, m_pGlobalEffectModel + m_nGlobalEffectCount, Found);
    if (pResult != m_pGlobalEffectModel + m_nGlobalEffectCount && pResult->dwEffectID == dwEffectID)
        return pResult;

    KFilePath::TabError("KTableList::GetGlobalEffectModel(ID %u) %s\n", dwEffectID, g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_COMMON_GLOBALL_EFFECT));
    return NULL;
}

KRLCursorEffectModel const* KTableList::GetCursorEffectModel(DWORD dwEffectID) const
{
    KRLCursorEffectModel Found = { dwEffectID };
    KRLCursorEffectModel const* pResult = std::lower_bound(m_pCursorEffectModel, m_pCursorEffectModel + m_nCursorEffectCount, Found);
    if (pResult != m_pCursorEffectModel + m_nCursorEffectCount && pResult->dwEffectID == dwEffectID)
        return pResult;

    KFilePath::TabError("KTableList::GetCursorEffectModel(ID %u) %s\n", dwEffectID, g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_COMMON_CURSOR_EFFECT));
    return NULL;
}

KForceRelationModel const* KTableList::GetForceRelationModel(int nForceRelationType) const
{
    KG_PROCESS_ERROR(nForceRelationType >= 0 && nForceRelationType < RL_FORCE_RELATION_COUNT);
    return &m_pRelationModel[nForceRelationType];
Exit0:
    return NULL;
}

KRLMissileModel const* KTableList::GetMissileModel(DWORD dwMissileID) const
{
    KRLMissileModel Found = { dwMissileID };
    KRLMissileModel const* pResult = std::lower_bound(m_pMissileModel, m_pMissileModel + m_nMissileCount, Found);
    if (pResult != m_pMissileModel + m_nMissileCount && pResult->dwMissileID == dwMissileID)
        return pResult;

    KFilePath::TabError("KTableList::GetMissileModel(Missile %u) %s\n", dwMissileID, g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_SKILL_MISSILE_TABLE));
    return NULL;
}

KRLMissileModelParam const* KTableList::GetMissileModelParam(DWORD dwMissileParamID) const
{
    KRLMissileModelParam Found = { dwMissileParamID };
    KRLMissileModelParam const* pResult = std::lower_bound(m_pMissileModelParam, m_pMissileModelParam + m_nMissileModelParamCount, Found);
    if (pResult != m_pMissileModelParam + m_nMissileModelParamCount && pResult->dwMissileParamID == dwMissileParamID)
        return pResult;

    KFilePath::TabError("KTableList::GetMissileModelParam(MissileParam %u) %s\n", dwMissileParamID, g_szMissileParamTypes[RL_MISSILE_PARAM_TYPE_MODEL]);
    return NULL;
}

KRLMissileHomingParam const* KTableList::GetMissileHomingParam(DWORD dwMissileParamID) const
{
    KRLMissileHomingParam Found = { dwMissileParamID };
    KRLMissileHomingParam const* pResult = std::lower_bound(m_pMissileHomingParam, m_pMissileHomingParam + m_nMissileHomingParamCount, Found);
    if (pResult != m_pMissileHomingParam + m_nMissileHomingParamCount && pResult->dwMissileParamID == dwMissileParamID)
        return pResult;

    KFilePath::TabError("KTableList::GetMissileHomingParam(MissileParam %u) %s\n", dwMissileParamID, g_szMissileParamTypes[RL_MISSILE_PARAM_TYPE_HOMING]);
    return NULL;
}

KRLMissileTimelineParam const* KTableList::GetMissileTimelineParam(DWORD dwMissileParamID) const
{
    KRLMissileTimelineParam Found = { dwMissileParamID };
    KRLMissileTimelineParam const* pResult = std::lower_bound(m_pMissileTimelineParam, m_pMissileTimelineParam + m_nMissileTimelineParamCount, Found);
    if (pResult != m_pMissileTimelineParam + m_nMissileTimelineParamCount && pResult->dwMissileParamID == dwMissileParamID)
        return pResult;

    KFilePath::TabError("KTableList::GetMissileTimelineParam(MissileParam %u) %s\n", dwMissileParamID, g_szMissileParamTypes[RL_MISSILE_PARAM_TYPE_TIMELINE]);
    return NULL;
}

KRLMissileYAxisParam const* KTableList::GetMissileYAxisParam(DWORD dwMissileParamID) const
{
    KRLMissileYAxisParam Found = { dwMissileParamID };
    KRLMissileYAxisParam const* pResult = std::lower_bound(m_pMissileYAxisParam, m_pMissileYAxisParam + m_nMissileYAxisParamCount, Found);
    if (pResult != m_pMissileYAxisParam + m_nMissileYAxisParamCount && pResult->dwMissileParamID == dwMissileParamID)
        return pResult;

    KFilePath::TabError("KTableList::GetMissileYAxisParam(MissileParam %u) %s\n", dwMissileParamID, g_szMissileParamTypes[RL_MISSILE_PARAM_TYPE_Y_AXIS]);
    return NULL;
}

KSkillBuffModel const* KTableList::GetSkillBuffModel(DWORD dwPart, DWORD dwSkillBuffID) const
{
    KSkillBuffModel Found = { dwPart, dwSkillBuffID };
    KSkillBuffModel const* pResult = std::lower_bound(m_pSkillBuffModel, m_pSkillBuffModel + m_nSkillBuffCount, Found);
    if (pResult != m_pSkillBuffModel + m_nSkillBuffCount && pResult->dwPart == dwPart && pResult->dwSkillBuffID == dwSkillBuffID)
        return pResult;

    KFilePath::TabError("KTableList::GetSkillBuffModel(Part %u, SkillBuffID %u) %s\n", 
        dwPart, dwSkillBuffID, g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_SKILL_BUFF_MODEL));
    return NULL;
}

KSkillCasterModel const* KTableList::GetPlayerSkillCasterModel(DWORD dwSkillID, DWORD dwSkillLevel, ROLE_TYPE nRoleType) const
{
    KSkillCasterModel Found = { dwSkillID, dwSkillLevel };
    KSkillCasterModel const* pResult = NULL;
    pResult = std::lower_bound(m_pPlayerSkillCasterModel[nRoleType], m_pPlayerSkillCasterModel[nRoleType] + m_nPlayerSkillCasterCount[nRoleType], Found);
    if (pResult != m_pPlayerSkillCasterModel[nRoleType] + m_nPlayerSkillCasterCount[nRoleType] && 
        pResult->dwSkillID == dwSkillID && pResult->dwSkillLevel == dwSkillLevel)
    {
        return pResult;
    }

    // 找不到查找等级为1的技能
    Found.dwSkillLevel = 1;
    pResult = std::lower_bound(m_pPlayerSkillCasterModel[nRoleType], m_pPlayerSkillCasterModel[nRoleType] + m_nPlayerSkillCasterCount[nRoleType], Found);
    if (pResult != m_pPlayerSkillCasterModel[nRoleType] + m_nPlayerSkillCasterCount[nRoleType] && 
        pResult->dwSkillID == dwSkillID && pResult->dwSkillLevel == 1)
    {
        return pResult;
    }

    KFilePath::TabError("KTableList::GetPlayerSkillCasterModel(SkillID %u, SkillLevel %u, RoleType %d) %s\n", dwSkillID, dwSkillLevel, 
        nRoleType, g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_SKILL_DIRECTORY, g_szPlayerSkillCasterConfigFiles[nRoleType]));
    return NULL;
}

KSkillCasterModel const* KTableList::GetNpcSkillCasterModel(DWORD dwSkillID, DWORD dwSkillLevel, DWORD dwRepresentID) const
{
    KNpcSkillCasterModel Found;
    KNpcSkillCasterModel const* pResult = NULL;
    Found.dwRepresentID = dwRepresentID;
    Found.Model.dwSkillID = dwSkillID;
    Found.Model.dwSkillLevel = dwSkillLevel;
    pResult = std::lower_bound(m_pNpcSkillCasterModel, m_pNpcSkillCasterModel + m_nNpcSkillCasterCount, Found);
    if (pResult != m_pNpcSkillCasterModel + m_nNpcSkillCasterCount && pResult->dwRepresentID == dwRepresentID && pResult->Model.dwSkillID == dwSkillID && pResult->Model.dwSkillLevel == dwSkillLevel)
        return &pResult->Model;

    // 找不到查找等级为1的技能
    Found.Model.dwSkillLevel = 1;
    pResult = std::lower_bound(m_pNpcSkillCasterModel, m_pNpcSkillCasterModel + m_nNpcSkillCasterCount, Found);
    if (pResult != m_pNpcSkillCasterModel + m_nNpcSkillCasterCount && pResult->dwRepresentID == dwRepresentID && pResult->Model.dwSkillID == dwSkillID && pResult->Model.dwSkillLevel == 1)
        return &pResult->Model;

    KFilePath::TabError("KTableList::GetNpcSkillCasterModel(SkillID %u, SkillLevel %u, RepresentID %u) %s\n", dwSkillID, 
        dwSkillLevel, dwRepresentID, g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_SKILL_DIRECTORY, g_szNpcSkillCasterConfigFile));
    return NULL;
}

KSkillEffectModel const* KTableList::GetSkillEffectModel(DWORD dwEffectID) const
{
    KSkillEffectModel Found = { dwEffectID };
    KSkillEffectModel const* pResult = NULL;
    pResult = std::lower_bound(m_pSkillEffectModel, m_pSkillEffectModel + m_nSkillEffectCount, Found);
    if (pResult != m_pSkillEffectModel + m_nSkillEffectCount && pResult->dwEffectID == dwEffectID)
        return pResult;

    KFilePath::TabError("KTableList::GetSkillEffectModel(EffectID %u) %s\n", dwEffectID,
        g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_SKILL_EFFECT_MODEL));
    return NULL;
}

KSkillResultModel const* KTableList::GetSkillResultModel(DWORD dwEffectResultID, DWORD dwEffectType) const
{
    KSkillResultModel Found = { dwEffectResultID, dwEffectType };
    KSkillResultModel const* pResult = NULL;
    pResult = std::lower_bound(m_pSkillResultModel, m_pSkillResultModel + m_nSkillResultCount, Found);
    if (pResult != m_pSkillResultModel + m_nSkillResultCount && pResult->dwEffectResultID == dwEffectResultID && pResult->dwEffectType == dwEffectType)
        return pResult;

    // 找不到查找类型为0的特效
    Found.dwEffectType = 0;
    pResult = std::lower_bound(m_pSkillResultModel, m_pSkillResultModel + m_nSkillResultCount, Found);
    if (pResult != m_pSkillResultModel + m_nSkillResultCount && pResult->dwEffectResultID == dwEffectResultID && pResult->dwEffectType == 0)
        return pResult;

    KFilePath::TabError("KTableList::GetSkillResultModel(EffectResultID %u, EffectType %u) %s\n", dwEffectResultID, dwEffectType,
        g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_SKILL_EFFECT_MODEL));
    return NULL;
}

KSkillChainModel const* KTableList::GetSkillChainModel(DWORD dwSkillID, DWORD dwSkillLevel) const
{
    KSkillChainModel Found = { dwSkillID, dwSkillLevel };
    KSkillChainModel const* pResult = NULL;
    pResult = std::lower_bound(m_pSkillChainModel, m_pSkillChainModel + m_nSkillChainCount, Found);
    if (pResult != m_pSkillChainModel + m_nSkillChainCount && pResult->dwSkillID == dwSkillID && pResult->dwSkillLevel == dwSkillLevel)
        return pResult;

    // 找不到查找等级为1的闪电
    Found.dwSkillLevel = 1;
    pResult = std::lower_bound(m_pSkillChainModel, m_pSkillChainModel + m_nSkillChainCount, Found);
    if (pResult != m_pSkillChainModel + m_nSkillChainCount && pResult->dwSkillID == dwSkillID && pResult->dwSkillLevel == 1)
        return pResult;

    KFilePath::TabError("KTableList::GetSkillChainModel(dwSkillID %u, dwSkillLevel %u) %s\n", dwSkillID, dwSkillLevel,
        g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_SKILL_CHAIN_MODEL));
    return NULL;
}

KRLDoodadModel const* KTableList::GetDoodadModel(DWORD dwRepresentID) const
{
    KRLDoodadModel Found = { dwRepresentID };
    KRLDoodadModel const* pResult = std::lower_bound(m_pDoodadModel, m_pDoodadModel + m_nDoodadCount, Found);
    if (pResult != m_pDoodadModel + m_nDoodadCount && pResult->dwRepresentID == dwRepresentID)
        return pResult;

    KFilePath::TabError("KTableList::GetDoodadModel(RepresentID %u) %s\n", dwRepresentID, 
        g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_DOODAD_MODEL));
    return NULL;
}

KDoodadEffectModel const* KTableList::GetEffectModel(DWORD dwEffectID) const
{
    KDoodadEffectModel Found = { dwEffectID };
    KDoodadEffectModel const* pResult = std::lower_bound(m_pDoodadEffectModel, m_pDoodadEffectModel + m_nDoodadEffectCount, Found);
    if (pResult != m_pDoodadEffectModel + m_nDoodadEffectCount && pResult->dwEffectID == dwEffectID)
        return pResult;

    KFilePath::TabError("KTableList::GetEffectModel(EffectID %u) %s\n", dwEffectID,
        g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_DOODAD_EFFECT_MODEL));
    return NULL;
}

KRidesModel const* KTableList::GetRidesModel(DWORD dwRepresentID) const
{
    KRidesModel Found = { dwRepresentID };
    KRidesModel const* pResult = std::lower_bound(m_pRidesModel, m_pRidesModel + m_nRidesCount, Found);
    if (pResult != m_pRidesModel + m_nRidesCount && pResult->dwRepresentID == dwRepresentID)
        return pResult;

    KFilePath::TabError("KTableList::GetRidesModel(RepresentID %u) %s\n", dwRepresentID, 
        g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_RIDES_MODEL));
    return NULL;
}

KRidesEquipModel const* KTableList::GetRidesEquipModel(int nIndex, DWORD dwRidesID, DWORD dwModelID) const
{
    KRidesEquipModel Found = { dwRidesID, dwModelID };

    ASSERT(nIndex < MODEL_RIDES_EQUIP_COUNT);

    KRidesEquipModel const* pResult = std::lower_bound(m_pRidesEquipModel[nIndex], m_pRidesEquipModel[nIndex] + m_nRidesEquipCount[nIndex], Found);
    if (pResult != m_pRidesEquipModel[nIndex] + m_nRidesEquipCount[nIndex] && pResult->dwRidesID == dwRidesID && pResult->dwModelID == dwModelID)
        return pResult;

    KFilePath::TabError("KTableList::GetRidesEquipModel(Index %d, RidesID %u, ModelID %u) %s\n", nIndex, dwRidesID, dwModelID,
        g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_RIDES_DIRECTORY, g_szRidesEquip[nIndex]));
    return NULL;
}

KRidesEquipModel const* KTableList::GetRidesSocketModel(int nIndex, DWORD dwRidesID, DWORD dwModelID) const
{
    KRidesEquipModel Found = { dwRidesID, dwModelID };

    ASSERT(nIndex < MODEL_RIDES_SOCKET_COUNT);

    KRidesEquipModel const* pResult = std::lower_bound(m_pRidesSocketModel[nIndex], m_pRidesSocketModel[nIndex] + m_nRidesSocketCount[nIndex], Found);
    if (pResult != m_pRidesSocketModel[nIndex] + m_nRidesSocketCount[nIndex] && pResult->dwRidesID == dwRidesID && pResult->dwModelID == dwModelID)
        return pResult;

    KFilePath::TabError("KTableList::GetRidesSocketModel(Index %d, RidesID %u, ModelID %u) %s\n", nIndex, dwRidesID, dwModelID,
        g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_RIDES_DIRECTORY, g_szRidesSocket[nIndex]));
    return NULL;
}

KRidesAnimationModel const* KTableList::GetRidesAnimationModel(DWORD dwRepresentID, DWORD dwAnimationID) const
{
    KRidesAnimationModel Found = { dwRepresentID, dwAnimationID };
    KRidesAnimationModel const* pResult = std::lower_bound(m_pRidesAnimationModel, m_pRidesAnimationModel + m_nRidesAnimationCount, Found);
    if (pResult != m_pRidesAnimationModel + m_nRidesAnimationCount && pResult->dwRepresentID == dwRepresentID && pResult->dwAnimationID == dwAnimationID)
        return pResult;

    KFilePath::TabError("KTableList::GetRidesAnimationModel(RepresentID %u, AniID %u) %s\n", 
        dwRepresentID, dwAnimationID, g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_RIDES_ANIMATION_MODEL));
    return NULL;
}

KNpcAnimationModel const* KTableList::GetNpcAnimationModel(DWORD dwRepresentID, DWORD dwAnimationID) const
{
    KNpcAnimationModel Found = { dwRepresentID, dwAnimationID };
    KNpcAnimationModel const* pResult = std::lower_bound(m_pNpcAnimationModel, m_pNpcAnimationModel + m_nNpcAnimationCount, Found);
    if (pResult != m_pNpcAnimationModel + m_nNpcAnimationCount && pResult->dwRepresentID == dwRepresentID && pResult->dwAnimationID == dwAnimationID)
        return pResult;

    KFilePath::TabError("KTableList::GetNpcAnimationModel(RepresentID %u, AnimationID %u) %s\n", 
        dwRepresentID, dwAnimationID, g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_NPC_ANIMATION_MODEL));
    return NULL;
}

KRLNpcAdjustAnimationIndex const* KTableList::GetNpcAdjustAnimationModel(DWORD dwRepresentID, DWORD dwAnimationID) const
{
    KRLNpcAdjustAnimationIndex Found = { dwRepresentID, dwAnimationID };
    KRLNpcAdjustAnimationIndex const* pResult = std::lower_bound(m_pNpcAnimationAdjustModel, m_pNpcAnimationAdjustModel + m_nNpcAnimationAdjustCount, Found);
    if (pResult != m_pNpcAnimationAdjustModel + m_nNpcAnimationAdjustCount && pResult->dwRepresentID == dwRepresentID && pResult->dwAnimationID == dwAnimationID)
        return pResult;
    return NULL;
}

KNpcSerialAnimation const* KTableList::GetNpcSerialAnimationModel(DWORD dwSerialID) const
{
    KNpcSerialAnimation Found = { dwSerialID };
    KNpcSerialAnimation const* pResult = std::lower_bound(m_pNpcSerialModel, m_pNpcSerialModel + m_nNpcSerialCount, Found);
    if (pResult != m_pNpcSerialModel + m_nNpcSerialCount && pResult->dwSerialID == dwSerialID)
        return pResult;

    KFilePath::TabError("KTableList::GetNpcSerialAnimationModel(SerialID %u) %s\n", dwSerialID,
        g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_NPC_SERIAL_ANIMATION_TABLE));
    return NULL;
}

KRLNpcModel const* KTableList::GetNpcModel(DWORD dwRepresentID) const
{
    KRLNpcModel Found = { dwRepresentID };
    KRLNpcModel const* pResult = std::lower_bound(m_pNpcModel, m_pNpcModel + m_nNpcCount, Found);
    if (pResult != m_pNpcModel + m_nNpcCount && pResult->dwRepresentID == dwRepresentID)
        return pResult;

    KFilePath::TabError("KTableList::GetNpcModel(RepresentID %u) %s\n", dwRepresentID,
        g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_NPC_MODEL));
    return NULL;
}

KRLPlayerModel const* KTableList::GetPlayerModel(ROLE_TYPE nRoleType) const
{
    ASSERT(nRoleType >= 0 && nRoleType < rtTotal);
    ASSERT(m_pPlayerModel[nRoleType].nRoleType == nRoleType);
    return &m_pPlayerModel[nRoleType];
}

KRLPlayerAnimationModel const* KTableList::GetPlayerAnimationModel(DWORD dwAnimationID, ROLE_TYPE nRoleType) const
{
    KRLPlayerAnimationModel Found = { dwAnimationID };
    KPlayerAnimationData const& ad = m_RoleTypeData[nRoleType].AnimationData;
    KRLPlayerAnimationModel const* pResult = std::lower_bound(ad.pModel, ad.pModel + ad.nCount, Found);
    if (pResult != ad.pModel + ad.nCount && pResult->dwAnimationID == dwAnimationID)
        return pResult;

    KFilePath::TabError("KTableList::GetPlayerAnimationModel(AnimationID %u, RoleType %d) %s\n", dwAnimationID, nRoleType, 
        g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_DIRECTORY, g_szPlayerAnimationFiles[nRoleType]));
    return NULL;
}

KRLAnimationIndex const* KTableList::GetPlayerAnimationIndex(DWORD dwAnimationIndexID, ROLE_TYPE nRoleType) const
{
    KRLAnimationIndex Found = { dwAnimationIndexID };
    KRLAnimationIndexData const& ad = m_RoleTypeData[nRoleType].AnimationIndexData;
    KRLAnimationIndex const* pResult = std::lower_bound(ad.pModel, ad.pModel + ad.nCount, Found);
    if (pResult != ad.pModel + ad.nCount && pResult->dwAnimationIndexID == dwAnimationIndexID)
        return pResult;

#if 0
    // TODO: 目前还是作为修正表，是可选的
    KFilePath::TabError("KTableList::GetPlayerAnimationIndex(AnimationID %u, RoleType %d) %s\n", dwAnimationIndexID, nRoleType, 
        g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_DIRECTORY, g_szPlayerAnimationIndexFiles[nRoleType]));
#endif
    return NULL;
}

KRLRandomAnimation const* KTableList::GetPlayerRandomAnimation(DWORD dwRandomAnimationID, ROLE_TYPE nRoleType) const
{
    KRLRandomAnimation Found = { dwRandomAnimationID };
    KRLRandomAnimationData const& ad = m_RoleTypeData[nRoleType].RandomAnimationData;
    KRLRandomAnimation const* pResult = std::lower_bound(ad.pModel, ad.pModel + ad.nCount, Found);
    if (pResult != ad.pModel + ad.nCount && pResult->dwRandomAnimationID == dwRandomAnimationID)
        return pResult;

    KFilePath::TabError("KTableList::GetPlayerRandomAnimation(AnimationID %u, RoleType %d) %s\n", dwRandomAnimationID, nRoleType, 
        g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_DIRECTORY, g_szPlayerRandomAnimationFiles[nRoleType]));
    return NULL;
}

KRLSequenceAnimation const* KTableList::GetPlayerSequenceAnimation(DWORD dwSequenceAnimationID, ROLE_TYPE nRoleType) const
{
    KRLSequenceAnimation Found = { dwSequenceAnimationID };
    KRLSequenceAnimationData const& ad = m_RoleTypeData[nRoleType].SequenceAnimationData;
    KRLSequenceAnimation const* pResult = std::lower_bound(ad.pModel, ad.pModel + ad.nCount, Found);
    if (pResult != ad.pModel + ad.nCount && pResult->dwSequenceAnimationID == dwSequenceAnimationID)
        return pResult;

    KFilePath::TabError("KTableList::GetPlayerSequenceAnimation(AnimationID %u, RoleType %d) %s\n", dwSequenceAnimationID, nRoleType, 
        g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_DIRECTORY, g_szPlayerSequenceAnimationFiles[nRoleType]));
    return NULL;
}

KPlayerBuffAnimation const* KTableList::GetPlayerBuffAnimation(DWORD dwPart, DWORD dwBuffID, DWORD dwAnimationID) const
{
    KPlayerBuffAnimation Found = { dwPart, dwBuffID, dwAnimationID };
    KPlayerBuffAnimation const* pResult = std::lower_bound(m_pPlayerBuffModel, m_pPlayerBuffModel + m_nPlayerBuffCount, Found);
    if (pResult != m_pPlayerBuffModel + m_nPlayerBuffCount && pResult->dwPart == dwPart && pResult->dwBuffID == dwBuffID && pResult->dwAnimationID == dwAnimationID)
        return pResult;
    return NULL;
}

KPlayerSerialAnimation const* KTableList::GetPlayerSerialAnimation(DWORD dwSerialID) const
{
    KPlayerSerialAnimation Found = { dwSerialID };
    KPlayerSerialAnimation const* pResult = std::lower_bound(m_pPlayerSerialModel, m_pPlayerSerialModel + m_nPlayerSerialCount, Found);
    if (pResult != m_pPlayerSerialModel + m_nPlayerSerialCount && pResult->dwSerialID == dwSerialID)
        return pResult;

    KFilePath::TabError("KTableList::GetPlayerSerialAnimation(SerialID %u) %s\n", dwSerialID,
        g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_SERIAL_ANIMATION_TABLE));
    return NULL;
}

KPlayerCraftAnimation const* KTableList::GetPlayerCraftAnimation(int nCraftID) const
{
    KPlayerCraftAnimation Found = { nCraftID };
    KPlayerCraftAnimation const* pResult = std::lower_bound(m_pPlayerCraftModel, m_pPlayerCraftModel + m_nPlayerCraftCount, Found);
    if (pResult != m_pPlayerCraftModel + m_nPlayerCraftCount && pResult->nCraftID == nCraftID)
        return pResult;

    KFilePath::TabError("KTableList::GetPlayerCraftAnimation(AniID %u) %s\n", nCraftID,
        g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_CRAFT_ANIMATION_TABLE));
    return NULL;
}

KRLPlayerAdjustAnimationIndex const* KTableList::GetPlayerAdjustAnimation(DWORD dwAnimationID, ROLE_TYPE nRoleType) const
{
    KRLPlayerAdjustAnimationIndex Found = { dwAnimationID };
    KPlayerAdjustAnimationData const& ad = m_RoleTypeData[nRoleType].AdjustAnimationData;
    KRLPlayerAdjustAnimationIndex const* pResult = std::lower_bound(ad.pModel, ad.pModel + ad.nCount, Found);
    if (pResult != ad.pModel + ad.nCount && pResult->dwAnimationID == dwAnimationID)
        return pResult;
    return NULL;
}

KRLTerrainAdjustAnimation const* KTableList::GetPlayerTerrainAdjustAnimation(DWORD dwAnimationID, DWORD dwTerrainID, ROLE_TYPE nRoleType) const
{
    KRLTerrainAdjustAnimation Found = { dwAnimationID, dwTerrainID };
    KRLTerrainAdjustAnimationData const& ad = m_RoleTypeData[nRoleType].TerrainAdjustAnimationData;
    KRLTerrainAdjustAnimation const* pResult = std::lower_bound(ad.pModel, ad.pModel + ad.nCount, Found);
    if (pResult != ad.pModel + ad.nCount && pResult->dwAnimationID == dwAnimationID && pResult->dwTerrainID == dwTerrainID)
        return pResult;
    return NULL;
}

KPlayerChestEquipModel const* KTableList::GetPlayerChestEquipModel(DWORD dwRepresentEquipID, ROLE_TYPE nRoleType) const
{
    KPlayerChestEquipModel Found = { dwRepresentEquipID };
    KPlayerChestData const& ced = m_RoleTypeData[nRoleType].ChestData;
    KPlayerChestEquipModel const* pResult = std::lower_bound(ced.pModel, ced.pModel + ced.nCount, Found);
    if (pResult != ced.pModel + ced.nCount && pResult->dwRepresentID == dwRepresentEquipID)
        return pResult;

    KFilePath::TabError("KTableList::GetPlayerChestEquipModel(RepresentEquipID %u, RoleType %d) %s\n", dwRepresentEquipID, nRoleType, 
        g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_EQUIP_DIRECTORY, g_szPlayerModelFiles[nRoleType][CHARACTER_EQUIP_FILE_CHEST]));
    return NULL;
}

KPlayerEquipModel const* KTableList::GetPlayerWaistModel(DWORD dwRepresentEquipID, ROLE_TYPE nRoleType) const
{
    KPlayerEquipModel Found = { dwRepresentEquipID };
    KPlayerEquipData const& ed = m_RoleTypeData[nRoleType].EquipData[MODEL_PLAYER_EQUIP_WAIST];
    KPlayerEquipModel const* pResult = std::lower_bound(ed.pModel, ed.pModel + ed.nCount, Found);
    if (pResult != ed.pModel + ed.nCount && pResult->dwRepresentID == dwRepresentEquipID)
        return pResult;

    KFilePath::TabError("KTableList::GetPlayerWaistModel(RepresentEquipID %u, RoleType %d) %s.\n", dwRepresentEquipID, nRoleType, 
        g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_EQUIP_DIRECTORY, g_szPlayerModelFiles[nRoleType][CHARACTER_EQUIP_FILE_WAIST]));
    return NULL;
}

KPlayerEquipModel const* KTableList::GetPlayerPantsModel(DWORD dwRepresentEquipID, ROLE_TYPE nRoleType) const
{
    KPlayerEquipModel Found = { dwRepresentEquipID };
    KPlayerEquipData const& ed = m_RoleTypeData[nRoleType].EquipData[MODEL_PLAYER_EQUIP_PANTS];
    KPlayerEquipModel const* pResult = std::lower_bound(ed.pModel, ed.pModel + ed.nCount, Found);
    if (pResult == ed.pModel + ed.nCount || pResult->dwRepresentID != dwRepresentEquipID)
    {
        KFilePath::TabError("KTableList::GetPlayerPantsModel(RepresentEquipID %u, RoleType %d) %s.\n", dwRepresentEquipID, nRoleType, 
            g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_EQUIP_DIRECTORY, g_szPlayerModelFiles[nRoleType][CHARACTER_EQUIP_FILE_PANTS]));
        return NULL;
    }
    return pResult;
}

KPlayerEquipModel const* KTableList::GetPlayerBangleModel(DWORD dwRepresentEquipID, ROLE_TYPE nRoleType) const
{
    KPlayerEquipModel Found = { dwRepresentEquipID };
    KPlayerEquipData const& ed = m_RoleTypeData[nRoleType].EquipData[MODEL_PLAYER_EQUIP_BANGLE];
    KPlayerEquipModel const* pResult = std::lower_bound(ed.pModel, ed.pModel + ed.nCount, Found);
    if (pResult != ed.pModel + ed.nCount && pResult->dwRepresentID == dwRepresentEquipID)
        return pResult;

    KFilePath::TabError("KTableList::GetPlayerBangleModel(RepresentEquipID %u, RoleType %d) %s.\n", dwRepresentEquipID, nRoleType, 
        g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_EQUIP_DIRECTORY, g_szPlayerModelFiles[nRoleType][CHARACTER_EQUIP_FILE_BANGLE]));
    return NULL;
}

KPlayerWeaponModel const* KTableList::GetPlayerWeaponModel(DWORD dwRepresentEquipID, ROLE_TYPE nRoleType) const
{
    KPlayerWeaponModel Found = { dwRepresentEquipID };
    KPlayerWeaponData const& wd = m_RoleTypeData[nRoleType].WeaponData;
    KPlayerWeaponModel const* pResult = std::lower_bound(wd.pModel, wd.pModel + wd.nCount, Found);
    if (pResult != wd.pModel + wd.nCount && pResult->dwRepresentID == dwRepresentEquipID)
        return pResult;

    KFilePath::TabError("KTableList::GetPlayerWeaponModel(RepresentEquipID %u, RoleType %d) %s.\n", dwRepresentEquipID, nRoleType,
        g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_EQUIP_DIRECTORY, g_szPlayerModelFiles[nRoleType][CHARACTER_EQUIP_FILE_MELEE_WEAPON]));
    return NULL;
}

KPlayerEquipModel const* KTableList::GetPlayerBackExtendModel(DWORD dwRepresentEquipID, ROLE_TYPE nRoleType) const
{
    if (dwRepresentEquipID == 0)
        return NULL;

    KPlayerEquipModel Found = { dwRepresentEquipID };
    KPlayerEquipData const& sd = m_RoleTypeData[nRoleType].SocketData[MODEL_PLAYER_SOCKET_BACK_EXTEND - MODEL_PLAYER_SOCKET_OTHERS_BEGIN];
    KPlayerEquipModel const* pResult = std::lower_bound(sd.pModel, sd.pModel + sd.nCount, Found);
    if (pResult != sd.pModel + sd.nCount && pResult->dwRepresentID == dwRepresentEquipID)
        return pResult;

    KFilePath::TabError("KTableList::GetPlayerBackExtendModel(RepresentEquipID %u, RoleType %d) %s.\n", dwRepresentEquipID, nRoleType,
        g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_EQUIP_DIRECTORY, g_szPlayerModelFiles[nRoleType][CHARACTER_EQUIP_FILE_BACK_EXTEND]));
    return NULL;
}

KPlayerEquipModel const* KTableList::GetPlayerWaistExtendModel(DWORD dwRepresentEquipID, ROLE_TYPE nRoleType) const
{
    if (dwRepresentEquipID == 0)
        return NULL;

    KPlayerEquipModel Found = { dwRepresentEquipID };
    KPlayerEquipData const& sd = m_RoleTypeData[nRoleType].SocketData[MODEL_PLAYER_SOCKET_WAIST_EXTEND - MODEL_PLAYER_SOCKET_OTHERS_BEGIN];
    KPlayerEquipModel const* pResult = std::lower_bound(sd.pModel, sd.pModel + sd.nCount, Found);
    if (pResult != sd.pModel + sd.nCount && pResult->dwRepresentID == dwRepresentEquipID)
        return pResult;

    KFilePath::TabError("KTableList::GetPlayerWaistExtendModel(RepresentEquipID %u, RoleType %d) %s.\n", dwRepresentEquipID, nRoleType,
        g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_EQUIP_DIRECTORY, g_szPlayerModelFiles[nRoleType][CHARACTER_EQUIP_FILE_WAIST_EXTEND]));
    return NULL;
}

KPlayerEquipModel const* KTableList::GetPlayerFaceModel(DWORD dwRepresentEquipID, ROLE_TYPE nRoleType) const
{
    if (dwRepresentEquipID == 0)
        return NULL;

    KPlayerEquipModel Found = { dwRepresentEquipID };
    KPlayerEquipData const& sd = m_RoleTypeData[nRoleType].SocketData[MODEL_PLAYER_SOCKET_FACE - MODEL_PLAYER_SOCKET_OTHERS_BEGIN];
    KPlayerEquipModel const* pResult = std::lower_bound(sd.pModel, sd.pModel + sd.nCount, Found);
    if (pResult != sd.pModel + sd.nCount && pResult->dwRepresentID == dwRepresentEquipID)
        return pResult;

    KFilePath::TabError("KTableList::GetPlayerFaceModel(RepresentEquipID %u, RoleType %d) %s.\n", dwRepresentEquipID, nRoleType, 			
        g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_EQUIP_DIRECTORY, g_szPlayerModelFiles[nRoleType][CHARACTER_EQUIP_FILE_FACE]));
    return NULL;
}

KPlayerHatModel const* KTableList::GetPlayerHatModel(DWORD dwRepresentEquipID, ROLE_TYPE nRoleType) const
{
    if (dwRepresentEquipID == 0)
        return NULL;

    KPlayerHatModel Found = { dwRepresentEquipID };
    KPlayerHatData const& hd = m_RoleTypeData[nRoleType].HatData;
    KPlayerHatModel const* pResult = std::lower_bound(hd.pModel, hd.pModel + hd.nCount, Found);
    if (pResult != hd.pModel + hd.nCount && pResult->dwRepresentID == dwRepresentEquipID)
        return pResult;

    KFilePath::TabError("KTableList::GetPlayerHatModel(RepresentEquipID %u, RoleType %d) %s\n", dwRepresentEquipID, nRoleType, 
        g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_EQUIP_DIRECTORY, g_szPlayerModelFiles[nRoleType][CHARACTER_EQUIP_FILE_HAT]));
    return NULL;
}

int KTableList::GetOriginalHairIndex(DWORD dwHeadID, DWORD *pdwHeadformID, DWORD *pdwBangID, DWORD *pdwPlaitID) const
{
    int nResult = false;
    KPlayerHeadIndex Found = { dwHeadID };
    KPlayerHeadIndex const *pResult = NULL;

    KG_ASSERT_EXIT(pdwHeadformID);
    KG_ASSERT_EXIT(pdwBangID);
    KG_ASSERT_EXIT(pdwPlaitID);

    pResult = std::lower_bound(m_pHeadIndexModel, m_pHeadIndexModel + m_uHeadIndexCount, Found);
    KG_PROCESS_ERROR(pResult != m_pHeadIndexModel + m_uHeadIndexCount && pResult->dwHeadID == dwHeadID);

    *pdwHeadformID = pResult->dwHeadformID;
    *pdwBangID = pResult->dwBangID;
    *pdwPlaitID = pResult->dwPlaitID;

    nResult = true;
Exit0:
    if (!nResult)
    {
        KFilePath::TabError(
            "KTableList::GetOriginalHairIndex(HeadID %u) %s\n", dwHeadID, 
            g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_EQUIP_DIRECTORY, "Head_Index.txt")
        );
    }
    return nResult;
}

int KTableList::GetFinallyHairIndex(DWORD dwOriginalHairID, int nHatType, DWORD *pdwFinallyHairID) const
{
    int nResult = false;
    KPlayerHairHatRelation Found = { dwOriginalHairID };
    KPlayerHairHatRelation const *pResult = NULL;

    KG_ASSERT_EXIT(pdwFinallyHairID);

    pResult = std::lower_bound(
        m_pHairHatRelationModel,
        m_pHairHatRelationModel + m_uHairHatRelationCount,
        Found
    );
    KG_PROCESS_ERROR(
        pResult != m_pHairHatRelationModel + m_uHairHatRelationCount &&
        pResult->dwOriginalHairID == dwOriginalHairID
    );

    *pdwFinallyHairID = pResult->dwFinallyHairIDs[nHatType];

    nResult = true;
Exit0:
    if (!nResult)
    {
        KFilePath::TabError("KTableList::GetFinallyHairIndex(OriginalHairID %u, HatType %d) %s\n", dwOriginalHairID, nHatType, 
            g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_EQUIP_DIRECTORY, "Hair_Hat_Relation.txt"));
    }
    return nResult;
}

KPlayerHairModel const* KTableList::GetPlayerHairModel(DWORD dwRepresentEquipID, ROLE_TYPE nRoleType) const
{
    KPlayerHairModel Found = { dwRepresentEquipID };
    KPlayerHairData const& hd = m_RoleTypeData[nRoleType].HairData;
    KPlayerHairModel const* pResult = std::lower_bound(hd.pModel, hd.pModel + hd.nCount, Found);
    if (pResult != hd.pModel + hd.nCount && pResult->dwRepresentID == dwRepresentEquipID)
        return pResult;

    KFilePath::TabError("KTableList::GetPlayerHairModel(RepresentEquipID %u, RoleType %d) %s\n", dwRepresentEquipID, nRoleType, 
        g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_EQUIP_DIRECTORY, g_szPlayerModelFiles[nRoleType][CHARACTER_EQUIP_FILE_HAIR]));
    return NULL;
}

KPlayerEquipSFXModel const* KTableList::GetPlayerSFXModel(DWORD dwRepresentEquipID, DWORD dwRepresentEnchantID, int nEquipPart, ROLE_TYPE nRoleType) const
{
    int nResult = false;
    KPlayerEquipSFXModel Found;
    KPlayerEquipSFXModel const* pResult = NULL;

    KG_PROCESS_ERROR(dwRepresentEquipID > 0);

    Found.dwEquipID = dwRepresentEquipID;
    Found.dwEnchantID = dwRepresentEnchantID;

    KG_ASSERT_EXIT(nRoleType >= 0 && nRoleType < _countof(m_RoleTypeData));
    KG_ASSERT_EXIT(nEquipPart >= 0 && nEquipPart < _countof(m_RoleTypeData[nRoleType].EquipSFXData));

    KPlayerEquipSFXData const& esd = m_RoleTypeData[nRoleType].EquipSFXData[nEquipPart];

    pResult = std::lower_bound(esd.pModel, esd.pModel + esd.nCount, Found);
    KG_PROCESS_ERROR(pResult != esd.pModel + esd.nCount);
    KG_PROCESS_ERROR(pResult->dwEquipID == dwRepresentEquipID);
    KG_PROCESS_ERROR(pResult->dwEnchantID == dwRepresentEnchantID);

    nResult = true;
Exit0:
    if (!nResult)
    {
        pResult = NULL;

        if (dwRepresentEnchantID)
        {
            if (nRoleType >= 0 && nRoleType < _countof(m_RoleTypeData) && nEquipPart >= 0 && nEquipPart < _countof(m_RoleTypeData[nRoleType].EquipSFXData))
            {
				LPCTSTR szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_EQUIP_SFX_DIRECTORY, g_szPlayerModelSFXFile[nRoleType][nEquipPart]);
                KFilePath::TabError("KTableList::GetPlayerSFXModel(RepresentEquipID %u, RepresentEnchantID %u, EquipPart %d, RoleType &d) %s\n", 
                    dwRepresentEquipID, dwRepresentEnchantID, nEquipPart, nRoleType, szFilePath);
            }
        }
    }
    return pResult;
}

KRLPlayerUniformModel const* KTableList::GetPlayerUniformModel(DWORD dwUniformID) const
{
    KRLPlayerUniformModel Found = { dwUniformID };

    KRLPlayerUniformModel const* pResult = std::lower_bound(m_pPlayerUniformModel, m_pPlayerUniformModel + m_nPlayerUniformCount, Found);
    if (pResult != m_pPlayerUniformModel + m_nPlayerUniformCount && pResult->dwID == dwUniformID)
        return pResult;

    KFilePath::TabError("KTableList::GetPlayerUniformModel(dwUniformID %u) %s\n", dwUniformID, 
        g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_UNIFORM_TABLE));
    return NULL;
}

KRLPlayerUniformModel const* KTableList::GetPlayerRoleTypeUniformModel(ROLE_TYPE nRoleType) const
{
    KRLPlayerUniformModel Found = { nRoleType };

    KRLPlayerUniformModel const* pResult = std::lower_bound(m_pPlayerRoleTypeUniformModel, m_pPlayerRoleTypeUniformModel + m_nPlayerRoleTypeUniformCount, Found);
    if (pResult != m_pPlayerRoleTypeUniformModel + m_nPlayerRoleTypeUniformCount && pResult->dwID == nRoleType)
        return pResult;

    KFilePath::TabError("KTableList::GetPlayerRoleTypeUniformModel(nRoleType %u) %s\n", nRoleType, 
        g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_ROLETYPE_UNIFORM_TABLE));
    return NULL;
}

KRLPlayerUniformModel const* KTableList::GetPlayerForceUniformModel(DWORD dwForceID) const
{
    KRLPlayerUniformModel Found = { dwForceID };

    KRLPlayerUniformModel const* pResult = std::lower_bound(m_pPlayerForceUniformModel, m_pPlayerForceUniformModel + m_nPlayerForceUniformCount, Found);
    if (pResult != m_pPlayerForceUniformModel + m_nPlayerForceUniformCount && pResult->dwID == dwForceID)
        return pResult;

    KFilePath::TabError("KTableList::GetPlayerForceUniformModel(dwForceID %u) %s\n", dwForceID, 
        g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_FORCE_UNIFORM_TABLE));
    return NULL;
}

KRLNpcUniformModel const* KTableList::GetNpcUniformModel(DWORD dwUniformID) const
{
    KRLNpcUniformModel Found = { dwUniformID };

    KRLNpcUniformModel const* pResult = std::lower_bound(m_pNpcUniformModel, m_pNpcUniformModel + m_nNpcUniformCount, Found);
    if (pResult != m_pNpcUniformModel + m_nNpcUniformCount && pResult->dwID == dwUniformID)
        return pResult;

    KFilePath::TabError("KTableList::GetNpcUniformModel(dwUniformID %u) %s\n", dwUniformID, 
        g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_NPC_UNIFORM_MODEL));
    return NULL;
}

KRLDoodadUniformModel const* KTableList::GetDoodadUniformModel(DWORD dwUniformID) const
{
    KRLDoodadUniformModel Found = { dwUniformID };

    KRLDoodadUniformModel const* pResult = std::lower_bound(m_pDoodadUniformModel, m_pDoodadUniformModel + m_nDoodadUniformCount, Found);
    if (pResult != m_pDoodadUniformModel + m_nDoodadUniformCount && pResult->dwID == dwUniformID)
        return pResult;

    KFilePath::TabError("KTableList::GetDoodadUniformModel(dwUniformID %u) %s\n", dwUniformID, 
        g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_DOODAD_UNIFORM_MODEL));
    return NULL;
}

KRLOptimizationModel const* KTableList::GetOptimizationModel(DWORD dwLevel) const
{
    KRLOptimizationModel Found = { dwLevel };

    KRLOptimizationModel const* pResult = std::lower_bound(m_pOptimizationModel, m_pOptimizationModel + m_nOptimizationCount, Found);
    if (pResult != m_pOptimizationModel + m_nOptimizationCount && pResult->dwLevel == dwLevel)
        return pResult;

    KFilePath::TabError("KTableList::GetOptimizationModel(dwLevel %u) %s\n", dwLevel, 
        g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_COMMON_OPTIMIZATION_TABLE));
    return NULL;
}

// Private Function
int KTableList::LoadMissileConfig()
{
    int nResult = false;
    LPCTSTR szFilePath = NULL;
    KSmartIniFile Ini;
    TCHAR szKey[64];
    int nValue = 0;

    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_MISSILE_PHASE_MODEL);
    KGLOG_PROCESS_ERROR(szFilePath);

    Ini = g_OpenIniFile(szFilePath);
    KGLOG_PROCESS_ERROR(Ini);

    Ini->GetInteger("MISSILE_PHASE_FILE", "Count", 0, &nValue);
    KGLOG_PROCESS_ERROR(nValue == RL_MISSILE_PHASE_TYPE_COUNT);

    for (size_t nIndex = 0; nIndex != RL_MISSILE_PHASE_TYPE_COUNT; ++nIndex)
    {
        _sntprintf(szKey, _countof(szKey), _T("Item_%d"), nIndex);
        Ini->GetString("MISSILE_PHASE_FILE", szKey, "", g_szMissilePhaseTypes[nIndex], FILE_NAME_LEN);
    }

    Ini->GetInteger("MISSILE_PARAM_FILE", "Count", 0, &nValue);
    KGLOG_PROCESS_ERROR(nValue == RL_MISSILE_PARAM_TYPE_COUNT);

    for (size_t nIndex = 0; nIndex != RL_MISSILE_PARAM_TYPE_COUNT; ++nIndex)
    {
        _sntprintf(szKey, _countof(szKey), _T("Item_%d"), nIndex);
        Ini->GetString("MISSILE_PARAM_FILE", szKey, "", g_szMissileParamTypes[nIndex], FILE_NAME_LEN);
    }

    nResult = true;
Exit0:
    return nResult;
}

HRESULT KTableList::LoadSkillCastModel()
{
    int nRetCode = false;
    LPCTSTR szFilePath = NULL;
    KSmartIniFile Ini;
    TCHAR szKey[64];
    int nValue = 0;

    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_SKILL_CASTER_MODEL);
    KGLOG_PROCESS_ERROR(szFilePath);

    Ini = g_OpenIniFile(szFilePath);
    KGLOG_PROCESS_ERROR(Ini);

    // Player Skill Caster
    Ini->GetInteger("PLAYER_CONFIG_FILE_NAME", "Count", 0, &nValue);
    KGLOG_PROCESS_ERROR(nValue == rtTotal);

    for (int nRoleType = 0; nRoleType != rtTotal; ++nRoleType)
    {
        _sntprintf(szKey, _countof(szKey), _T("Item_%d"), nRoleType);
        g_szPlayerSkillCasterConfigFiles[nRoleType][FILE_NAME_LEN - 1] = _T('\0');
        Ini->GetString("PLAYER_CONFIG_FILE_NAME", szKey, "", g_szPlayerSkillCasterConfigFiles[nRoleType], FILE_NAME_LEN);

        szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_SKILL_DIRECTORY, g_szPlayerSkillCasterConfigFiles[nRoleType]);
        KGLOG_PROCESS_ERROR(szFilePath);

        nRetCode = m_PlayerSkillCasterTab[nRoleType].Load(szFilePath, "iiiiiiiiiiiiiiiiiiiiiiiiiiiiifsi");
        KGLOG_PROCESS_ERROR(nRetCode);

        m_pPlayerSkillCasterModel[nRoleType] = (KSkillCasterModel*)m_PlayerSkillCasterTab[nRoleType].Data(m_nPlayerSkillCasterCount[nRoleType]);
        KGLOG_PROCESS_ERROR(m_pPlayerSkillCasterModel[nRoleType]);
    }

    // NPC Skill Caster
    g_szNpcSkillCasterConfigFile[FILE_NAME_LEN - 1] = _T('\0');
    Ini->GetString("NPC_CONFIG_FILE_NAME", "File", "", g_szNpcSkillCasterConfigFile, FILE_NAME_LEN);

    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_SKILL_DIRECTORY, g_szNpcSkillCasterConfigFile);
    KGLOG_PROCESS_ERROR(szFilePath);

    nRetCode = m_NpcSkillCasterTab.Load(szFilePath, "iiiiiiiiiiiiiiiiiiiiiiiiiiiiiifsi");
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pNpcSkillCasterModel = (KNpcSkillCasterModel*)m_NpcSkillCasterTab.Data(m_nNpcSkillCasterCount);
    KGLOG_PROCESS_ERROR(m_pNpcSkillCasterModel);

    return S_OK;
Exit0:
    return E_FAIL;
}

HRESULT KTableList::LoadRidesEquipAndSocketModel()
{
    int nRetCode = false;
    //LPCTSTR szFilePath = NULL;
    //KSmartIniFile Ini;
    //TCHAR szKey[64];
    //int nValue = 0;

    //szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_RIDES_EQUIP_MODEL);
    //KGLOG_PROCESS_ERROR(szFilePath);

    //Ini = g_OpenIniFile(szFilePath);
    //KGLOG_PROCESS_ERROR(Ini);

    //Ini->GetInteger("EQUIP_CONFIG_FILE_NAME", "Count", 0, &nValue);
    //KGLOG_PROCESS_ERROR(nValue == MODEL_RIDES_EQUIP_COUNT);

    //Ini->GetInteger("SOCKET_CONFIG_FILE_NAME", "Count", 0, &nValue);
    //KGLOG_PROCESS_ERROR(nValue == MODEL_RIDES_SOCKET_COUNT);

    //// Rides Equip
    //for (int nIndex = 0; nIndex < MODEL_RIDES_EQUIP_COUNT; ++nIndex)
    //{
    //    _sntprintf(szKey, _countof(szKey), _T("Item_%d"), nIndex);
    //    g_szRidesEquip[nIndex][FILE_NAME_LEN - 1] = _T('\0');
    //    Ini->GetString("EQUIP_CONFIG_FILE_NAME", szKey, "", g_szRidesEquip[nIndex], FILE_NAME_LEN);

    //    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_RIDES_DIRECTORY, g_szRidesEquip[nIndex]);
    //    KGLOG_PROCESS_ERROR(szFilePath[0] != _T('\0'));

    //    if (!g_IsFileExist(szFilePath))
    //        continue;

    //    nRetCode = m_RidesEquipTab[nIndex].Load(szFilePath, "iiifss");
    //    KGLOG_PROCESS_ERROR(nRetCode);

    //    m_pRidesEquipModel[nIndex] = (KRidesEquipModel*)m_RidesEquipTab[nIndex].Data(m_nRidesEquipCount[nIndex]);
    //    KGLOG_PROCESS_ERROR(m_pRidesEquipModel[nIndex]);
    //}

    //// Rides Socket
    //for (int nIndex = 0; nIndex < MODEL_RIDES_SOCKET_COUNT; ++nIndex)
    //{
    //    _sntprintf(szKey, _countof(szKey), _T("Item_%d"), nIndex);
    //    g_szRidesSocket[nIndex][FILE_NAME_LEN - 1] = _T('\0');
    //    Ini->GetString("SOCKET_CONFIG_FILE_NAME", szKey, "", g_szRidesSocket[nIndex], FILE_NAME_LEN);

    //    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_RIDES_DIRECTORY, g_szRidesSocket[nIndex]);
    //    KGLOG_PROCESS_ERROR(szFilePath[0] != _T('\0'));

    //    if (!g_IsFileExist(szFilePath))
    //        continue;

    //    nRetCode = m_RidesSocketTab[nIndex].Load(szFilePath, "iiifss");
    //    KGLOG_PROCESS_ERROR(nRetCode);

    //    m_pRidesSocketModel[nIndex] = (KRidesEquipModel*)m_RidesSocketTab[nIndex].Data(m_nRidesSocketCount[nIndex]);
    //    KGLOG_PROCESS_ERROR(m_pRidesSocketModel[nIndex]);
    //}

    return S_OK;
Exit0:
    return E_FAIL;
}

int KTableList::LoadPlayerAllModel()
{
    // TODO: 代码整理

    int nRetCode = false;
    int nResult = false;
    LPCTSTR szFilePath = NULL;
    LPCTSTR szConfigFilePath = NULL;
    KSmartIniFile Ini;
    HRESULT hr = E_FAIL;
    TCHAR szKey[64];
    int nValue = 0;
    TCHAR szBodyName[FILE_NAME_LEN];
    TCHAR szSFXName[FILE_NAME_LEN];

    szConfigFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_EQUIP_MODEL);
    KGLOG_PROCESS_ERROR(szConfigFilePath);

    Ini = g_OpenIniFile(szConfigFilePath);
    KGLOG_PROCESS_ERROR(Ini);

    Ini->GetInteger("SHEATH_TYPE", "Count", 0, &nValue);
    KGLOG_PROCESS_ERROR(nValue == RL_SHEATH_COUNT);

    Ini->GetInteger("SHEATH_SOCKET_LH", "Count", 0, &nValue);
    KGLOG_PROCESS_ERROR(nValue == RL_SHEATH_COUNT);

    Ini->GetInteger("SHEATH_SOCKET_RH", "Count", 0, &nValue);
    KGLOG_PROCESS_ERROR(nValue == RL_SHEATH_COUNT);

    Ini->GetInteger("BODY_TYPE_FILE_NAME", "Count", 0, &nValue);
    KGLOG_PROCESS_ERROR(nValue == rtTotal);

    Ini->GetInteger("EQUIP_CONFIG_FILE_NAME", "Count", 0, &nValue);
    KGLOG_PROCESS_ERROR(nValue == CHARACTER_EQUIP_FILE_COUNT);

    Ini->GetInteger("BODY_TYPE_FILE_NAME", "Count", 0, &nValue);
    KGLOG_PROCESS_ERROR(nValue == rtTotal);

    Ini->GetInteger("EQUIP_SFX_CONFIG_FILE_NAME", "Count", 0, &nValue);
    KGLOG_PROCESS_ERROR(nValue == CHARACTER_EQUIP_SFX_FILE_COUNT);

    for (int nIndex = 0; nIndex < RL_SHEATH_COUNT; ++nIndex)
    {
        _sntprintf(szKey, _countof(szKey), _T("Item_%d"), nIndex);
        g_szSheathType[nIndex][FILE_NAME_LEN - 1] = _T('\0');

        Ini->GetString("SHEATH_TYPE", szKey, "", g_szSheathType[nIndex], FILE_NAME_LEN);
        KGLOG_PROCESS_ERROR(g_szSheathType[nIndex][FILE_NAME_LEN - 1] == _T('\0'));
    }

    for (int nIndex = 0; nIndex < RL_SHEATH_COUNT; ++nIndex)
    {
        _sntprintf(szKey, _countof(szKey), _T("Item_%d"), nIndex);
        g_szSheathSocketLH[nIndex][FILE_NAME_LEN - 1] = _T('\0');

        Ini->GetString("SHEATH_SOCKET_LH", szKey, "", g_szSheathSocketLH[nIndex], FILE_NAME_LEN);
        KGLOG_PROCESS_ERROR(g_szSheathSocketLH[nIndex][FILE_NAME_LEN - 1] == _T('\0'));

        _sntprintf(szKey, _countof(szKey), _T("Item_%d"), nIndex);
        g_szSheathSocketRH[nIndex][FILE_NAME_LEN - 1] = _T('\0');

        Ini->GetString("SHEATH_SOCKET_RH", szKey, "", g_szSheathSocketRH[nIndex], FILE_NAME_LEN);
        KGLOG_PROCESS_ERROR(g_szSheathSocketRH[nIndex][FILE_NAME_LEN - 1] == _T('\0'));
    }
    
    // Load Player Equip Model
    for (int nRoleType = 0; nRoleType < _countof(m_RoleTypeData); ++nRoleType)
    {
        _sntprintf(szKey, _countof(szKey), _T("Item_%d"), nRoleType);
        szBodyName[FILE_NAME_LEN - 1] = _T('\0');

        Ini->GetString("BODY_TYPE_FILE_NAME", szKey, "", szBodyName, BODY_TYPE_FILE_NAME_LEN);
        KGLOG_PROCESS_ERROR(szBodyName[FILE_NAME_LEN - 1] == _T('\0'));

        for (int nEquipFile = 0; nEquipFile < CHARACTER_EQUIP_FILE_COUNT; ++nEquipFile)
        {
            _sntprintf(szKey, _countof(szKey), _T("Item_%d"), nEquipFile);
            g_szPlayerModelFiles[nRoleType][nEquipFile][FILE_NAME_LEN - 1] = _T('\0');

            Ini->GetString("EQUIP_CONFIG_FILE_NAME", szKey, "", g_szPlayerModelFiles[nRoleType][nEquipFile], FILE_NAME_LEN);

            _sntprintf(g_szPlayerModelFiles[nRoleType][nEquipFile], FILE_NAME_LEN, _T("%s_%s.txt"), g_szPlayerModelFiles[nRoleType][nEquipFile], szBodyName);
            KGLOG_PROCESS_ERROR(g_szPlayerModelFiles[nRoleType][nEquipFile][FILE_NAME_LEN - 1] == _T('\0'));

            switch (nEquipFile)
            {
            case CHARACTER_EQUIP_FILE_MELEE_WEAPON:
                {
                    KPlayerWeaponData& wd = m_RoleTypeData[nRoleType].WeaponData;

                    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_EQUIP_DIRECTORY, g_szPlayerModelFiles[nRoleType][nEquipFile]);
                    KGLOG_PROCESS_ERROR(szFilePath[0] != _T('\0'));

                    nRetCode = wd.Tab.Load(szFilePath, "iifiiiiiiiiiiissssssssss");
                    KGLOG_PROCESS_ERROR(nRetCode);

                    wd.pModel = (KPlayerWeaponModel*)wd.Tab.Data(wd.nCount);
                    KGLOG_PROCESS_ERROR(wd.pModel);
                }
                break;
            case CHARACTER_EQUIP_FILE_CHEST:
                {
                    KPlayerChestData& cd = m_RoleTypeData[nRoleType].ChestData;

                    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_EQUIP_DIRECTORY, g_szPlayerModelFiles[nRoleType][nEquipFile]);
                    KGLOG_PROCESS_ERROR(szFilePath[0] != _T('\0'));

                    nRetCode = cd.Tab.Load(szFilePath, "ifiiiiiiiiiisssi");
                    KGLOG_PROCESS_ERROR(nRetCode);

                    cd.pModel = (KPlayerChestEquipModel*)cd.Tab.Data(cd.nCount);
                    KGLOG_PROCESS_ERROR(cd.pModel);
                }
                break;
            case CHARACTER_EQUIP_FILE_HAIR:
                {
                    KPlayerHairData& hd = m_RoleTypeData[nRoleType].HairData;

                    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_EQUIP_DIRECTORY, g_szPlayerModelFiles[nRoleType][nEquipFile]);
                    KGLOG_PROCESS_ERROR(szFilePath[0] != _T('\0'));

                    nRetCode = hd.Tab.Load(szFilePath, "ifiiiiiiiiiisssi");
                    KGLOG_PROCESS_ERROR(nRetCode);

                    hd.pModel = (KPlayerHairModel*)hd.Tab.Data(hd.nCount);
                    KGLOG_PROCESS_ERROR(hd.pModel);
                }
                break;
            case CHARACTER_EQUIP_FILE_BACK_EXTEND:
            case CHARACTER_EQUIP_FILE_WAIST_EXTEND:
            case CHARACTER_EQUIP_FILE_FACE:
                {
                    int nIndex = nEquipFile - CHARACTER_EQUIP_FILE_BACK_EXTEND;

                    ASSERT(nIndex >= 0 && nIndex < _countof(m_RoleTypeData[nRoleType].SocketData));
                    KPlayerEquipData& sd = m_RoleTypeData[nRoleType].SocketData[nIndex];

                    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_EQUIP_DIRECTORY, g_szPlayerModelFiles[nRoleType][nEquipFile]);
                    KGLOG_PROCESS_ERROR(szFilePath[0] != _T('\0'));

                    nRetCode = sd.Tab.Load(szFilePath, "ifiiiiiiiiiisss");
                    KGLOG_PROCESS_ERROR(nRetCode);

                    sd.pModel = (KPlayerEquipModel*)sd.Tab.Data(sd.nCount);
                    KGLOG_PROCESS_ERROR(sd.pModel);
                }
                break;
            case CHARACTER_EQUIP_FILE_HAT:
                {
                    KPlayerHatData& hd = m_RoleTypeData[nRoleType].HatData;

                    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_EQUIP_DIRECTORY, g_szPlayerModelFiles[nRoleType][nEquipFile]);
                    KGLOG_PROCESS_ERROR(szFilePath[0] != _T('\0'));

                    nRetCode = hd.Tab.Load(szFilePath, "ifiiiiiiiiiisssi");
                    KGLOG_PROCESS_ERROR(nRetCode);

                    hd.pModel = (KPlayerHatModel*)hd.Tab.Data(hd.nCount);
                    KGLOG_PROCESS_ERROR(hd.pModel);
                }
                break;
            default:
                {
                    int nIndex = nEquipFile - CHARACTER_EQUIP_FILE_CHEST;

                    ASSERT(nIndex >= 0 && nIndex < _countof(m_RoleTypeData[nRoleType].EquipData));
                    KPlayerEquipData& ed = m_RoleTypeData[nRoleType].EquipData[nIndex];

                    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_EQUIP_DIRECTORY, g_szPlayerModelFiles[nRoleType][nEquipFile]);
                    KGLOG_PROCESS_ERROR(szFilePath[0] != _T('\0'));

                    nRetCode = ed.Tab.Load(szFilePath, "ifiiiiiiiiiisss");
                    KGLOG_PROCESS_ERROR(nRetCode);

                    ed.pModel = (KPlayerEquipModel*)ed.Tab.Data(ed.nCount);
                    KGLOG_PROCESS_ERROR(ed.pModel);
                }
                break;
            }
        }
    }

    // Load head index
    {
        szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_EQUIP_DIRECTORY, "Head_Index.txt");
        KGLOG_PROCESS_ERROR(szFilePath[0] != _T('\0'));

        nRetCode = m_HeadIndexTab.Load(szFilePath, "iiii");
        KGLOG_PROCESS_ERROR(nRetCode);

        m_pHeadIndexModel = (KPlayerHeadIndex*)m_HeadIndexTab.Data(m_uHeadIndexCount);
        KGLOG_PROCESS_ERROR(m_pHeadIndexModel);
    }

    // Load the relation of hair and hat  
    {
        szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_EQUIP_DIRECTORY, "Hair_Hat_Relation.txt");
        KGLOG_PROCESS_ERROR(szFilePath[0] != _T('\0'));

        nRetCode = m_HairHatRelationTab.Load(szFilePath, "iiiii");
        KGLOG_PROCESS_ERROR(nRetCode);

        m_pHairHatRelationModel = 
            (KPlayerHairHatRelation*)m_HairHatRelationTab.Data(m_uHairHatRelationCount);
        KGLOG_PROCESS_ERROR(m_pHairHatRelationModel);
    }
   
    // Load Player Equip SFX Model
    for (int nRoleType = 0; nRoleType != rtTotal; ++nRoleType)
    {
        _sntprintf(szKey, _countof(szKey), _T("Item_%d"), nRoleType);
        szBodyName[FILE_NAME_LEN - 1] = _T('\0');

        Ini->GetString("BODY_TYPE_FILE_NAME", szKey, "", szBodyName, BODY_TYPE_FILE_NAME_LEN);
        KGLOG_PROCESS_ERROR(szBodyName[FILE_NAME_LEN - 1] == _T('\0'));

        for (int nFileIndex = 0; nFileIndex < _countof(m_RoleTypeData[nRoleType].EquipSFXData); ++nFileIndex)
        {   
            LPTSTR szModelSFXFile = NULL;

            ASSERT(nFileIndex >= 0 && nFileIndex < _countof(m_RoleTypeData[nRoleType].EquipSFXData));
            KPlayerEquipSFXData& esd = m_RoleTypeData[nRoleType].EquipSFXData[nFileIndex];

            szModelSFXFile = g_szPlayerModelSFXFile[nRoleType][nFileIndex];

            _sntprintf(szKey, _countof(szKey), _T("Item_%d"), nFileIndex);
            szKey[_countof(szKey) - 1] = _T('\0');

            Ini->GetString("EQUIP_SFX_CONFIG_FILE_NAME", szKey, "", szSFXName, _countof(szSFXName));
            szSFXName[_countof(szSFXName) - 1] = _T('\0');

            _sntprintf(szModelSFXFile, FILE_NAME_LEN, _T("%s_%s.txt"), szSFXName, szBodyName);
            KGLOG_PROCESS_ERROR(szModelSFXFile[FILE_NAME_LEN - 1] == _T('\0'));

            szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_EQUIP_SFX_DIRECTORY, szModelSFXFile);
            KGLOG_PROCESS_ERROR(szFilePath[0] != _T('\0'));

            if (g_IsFileExist(szFilePath))
            {
                nRetCode = esd.Tab.Load(szFilePath, "iiss");
                KGLOG_PROCESS_ERROR(nRetCode);

                esd.pModel = (KPlayerEquipSFXModel*)esd.Tab.Data(esd.nCount);
                KGLOG_PROCESS_ERROR(esd.pModel);
            }
            else
            {
                esd.pModel = NULL;
                esd.nCount = 0;
            }
        }
    }

    nResult = true;
Exit0:
    return nResult;
}

HRESULT KTableList::LoadPlayerConfigFile()
{
    LPCTSTR szFilePath = NULL;
    int nRetCode = false;
    TCHAR szKey[64];
    KSmartIniFile Ini;
    int nValue = 0;

    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_ANIMATION_MODEL);
    KGLOG_PROCESS_ERROR(szFilePath);

    Ini = g_OpenIniFile(szFilePath);
    KGLOG_PROCESS_ERROR(Ini);

    Ini->GetInteger("MODEL_ANI", "Count", 0, &nValue);
    KGLOG_PROCESS_ERROR(nValue == _countof(g_szAnimationSectionName));

    Ini->GetInteger("MODEL_WEAPON", "Count", 0, &nValue);
    KGLOG_PROCESS_ERROR(nValue == _countof(g_szWeaponName));

    Ini->GetInteger("ANIMATION_CONFIG_FILE_NAME", "Count", 0, &nValue);
    KGLOG_PROCESS_ERROR(nValue == _countof(g_szPlayerAnimationFiles));

    Ini->GetInteger("ANIMATION_ADJUST_CONFIG_FILE_NAME", "Count", 0, &nValue);
    KGLOG_PROCESS_ERROR(nValue == _countof(g_szPlayerAnimationTerrainAdjustFiles));

    Ini->GetInteger("ANIMATION_TERRAIN_ADJUST_CONFIG_FILE_NAME", "Count", 0, &nValue);
    KGLOG_PROCESS_ERROR(nValue == _countof(g_szPlayerAnimationAdjustFiles));

    Ini->GetInteger("ANIMATION_FUSION_CONFIG_FILE_NAME", "Count", 0, &nValue);
    KGLOG_PROCESS_ERROR(nValue == _countof(g_szPlayerAnimationFusionFiles));

    Ini->GetInteger("ANIMATION_INDEX_CONFIG_FILE_NAME", "Count", 0, &nValue);
    KGLOG_PROCESS_ERROR(nValue == _countof(g_szPlayerAnimationIndexFiles));

    Ini->GetInteger("RANDOM_ANIMATION_CONFIG_FILE_NAME", "Count", 0, &nValue);
    KGLOG_PROCESS_ERROR(nValue == _countof(g_szPlayerRandomAnimationFiles));

    Ini->GetInteger("SEQUENCE_ANIMATION_CONFIG_FILE_NAME", "Count", 0, &nValue);
    KGLOG_PROCESS_ERROR(nValue == _countof(g_szPlayerSequenceAnimationFiles));

    Ini->GetInteger("ANIMATION_TABLE_FILE_NAME", "Count", 0, &nValue);
    KGLOG_PROCESS_ERROR(nValue == _countof(g_szAniTableFileName));

    for (int nIndex = 0; nIndex != RL_ANIMATION_TABLE_COUNT; ++nIndex)
    {
        _sntprintf(szKey, _countof(szKey), _T("Item_%d"), nIndex);

        g_szAnimationSectionName[nIndex][FILE_NAME_LEN - 1] = _T('\0');
        Ini->GetString("MODEL_ANI", szKey, "", g_szAnimationSectionName[nIndex], FILE_NAME_LEN);

        nRetCode = g_szAnimationSectionName[nIndex][FILE_NAME_LEN - 1] == _T('\0');
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    for (int nIndex = 0; nIndex != RL_WEAPON_COUNT; ++nIndex)
    {
        _sntprintf(szKey, _countof(szKey), _T("Item_%d"), nIndex);

        g_szWeaponName[nIndex][FILE_NAME_LEN - 1] = _T('\0');
        Ini->GetString("MODEL_WEAPON", szKey, "", g_szWeaponName[nIndex], FILE_NAME_LEN);

        nRetCode = g_szWeaponName[nIndex][FILE_NAME_LEN - 1] == _T('\0');
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    for (int nRoleType = 0; nRoleType != rtTotal; ++nRoleType)
    {
        _sntprintf(szKey, _countof(szKey), _T("Item_%d"), nRoleType);

        g_szPlayerAnimationFiles[nRoleType][FILE_NAME_LEN - 1] = _T('\0');
        Ini->GetString("ANIMATION_CONFIG_FILE_NAME", szKey, "", g_szPlayerAnimationFiles[nRoleType], FILE_NAME_LEN);

        nRetCode = g_szPlayerAnimationFiles[nRoleType][FILE_NAME_LEN - 1] == _T('\0');
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    for (int nRoleType = 0; nRoleType != rtTotal; ++nRoleType)
    {
        _sntprintf(szKey, _countof(szKey), _T("Item_%d"), nRoleType);

        g_szPlayerAnimationAdjustFiles[nRoleType][FILE_NAME_LEN - 1] = _T('\0');
        Ini->GetString("ANIMATION_ADJUST_CONFIG_FILE_NAME", szKey, "", g_szPlayerAnimationAdjustFiles[nRoleType], FILE_NAME_LEN);

        nRetCode = g_szPlayerAnimationAdjustFiles[nRoleType][FILE_NAME_LEN - 1] == _T('\0');
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    for (int nRoleType = 0; nRoleType != rtTotal; ++nRoleType)
    {
        _sntprintf(szKey, _countof(szKey), _T("Item_%d"), nRoleType);

        g_szPlayerAnimationTerrainAdjustFiles[nRoleType][FILE_NAME_LEN - 1] = _T('\0');
        Ini->GetString("ANIMATION_TERRAIN_ADJUST_CONFIG_FILE_NAME", szKey, "", g_szPlayerAnimationTerrainAdjustFiles[nRoleType], FILE_NAME_LEN);

        nRetCode = g_szPlayerAnimationTerrainAdjustFiles[nRoleType][FILE_NAME_LEN - 1] == _T('\0');
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    for (int nRoleType = 0; nRoleType != rtTotal; ++nRoleType)
    {
        _sntprintf(szKey, _countof(szKey), _T("Item_%d"), nRoleType);

        g_szPlayerAnimationFusionFiles[nRoleType][FILE_NAME_LEN - 1] = _T('\0');
        Ini->GetString("ANIMATION_FUSION_CONFIG_FILE_NAME", szKey, "", g_szPlayerAnimationFusionFiles[nRoleType], FILE_NAME_LEN);

        nRetCode = g_szPlayerAnimationFusionFiles[nRoleType][FILE_NAME_LEN - 1] == _T('\0');
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    for (int nRoleType = 0; nRoleType != rtTotal; ++nRoleType)
    {
        _sntprintf(szKey, _countof(szKey), _T("Item_%d"), nRoleType);

        g_szPlayerAnimationIndexFiles[nRoleType][FILE_NAME_LEN - 1] = _T('\0');
        Ini->GetString("ANIMATION_INDEX_CONFIG_FILE_NAME", szKey, "", g_szPlayerAnimationIndexFiles[nRoleType], FILE_NAME_LEN);

        nRetCode = g_szPlayerAnimationIndexFiles[nRoleType][FILE_NAME_LEN - 1] == _T('\0');
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    for (int nRoleType = 0; nRoleType != rtTotal; ++nRoleType)
    {
        _sntprintf(szKey, _countof(szKey), _T("Item_%d"), nRoleType);

        g_szPlayerRandomAnimationFiles[nRoleType][FILE_NAME_LEN - 1] = _T('\0');
        Ini->GetString("RANDOM_ANIMATION_CONFIG_FILE_NAME", szKey, "", g_szPlayerRandomAnimationFiles[nRoleType], FILE_NAME_LEN);

        nRetCode = g_szPlayerRandomAnimationFiles[nRoleType][FILE_NAME_LEN - 1] == _T('\0');
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    for (int nRoleType = 0; nRoleType != rtTotal; ++nRoleType)
    {
        _sntprintf(szKey, _countof(szKey), _T("Item_%d"), nRoleType);

        g_szPlayerSequenceAnimationFiles[nRoleType][FILE_NAME_LEN - 1] = _T('\0');
        Ini->GetString("SEQUENCE_ANIMATION_CONFIG_FILE_NAME", szKey, "", g_szPlayerSequenceAnimationFiles[nRoleType], FILE_NAME_LEN);

        nRetCode = g_szPlayerSequenceAnimationFiles[nRoleType][FILE_NAME_LEN - 1] == _T('\0');
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    for (int nIndex = 0; nIndex != RL_SCHOOL_COUNT; ++nIndex)
    {
        _sntprintf(szKey, _countof(szKey), _T("Item_%d"), nIndex);

        g_szAniTableFileName[nIndex][FILE_NAME_LEN - 1] = _T('\0');
        Ini->GetString("ANIMATION_TABLE_FILE_NAME", szKey, "", g_szAniTableFileName[nIndex], FILE_NAME_LEN);

        nRetCode = g_szAniTableFileName[nIndex][FILE_NAME_LEN - 1] == _T('\0');
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    return S_OK;
Exit0:
    return E_FAIL;
}

HRESULT KTableList::LoadNpcAnimationRelationTable()
{
    HRESULT hrResult = E_FAIL;
    LPCTSTR szFilePath = NULL;
    KSmartTabFile Tab;
    int nValue = 0;
    int nKind  = 0;
    int nTabWidth = 0;
    int nTabHeight = 0;

    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_NPC_ANIMATION_RELATION_TABLE);
    KGLOG_PROCESS_ERROR(szFilePath);

    Tab = g_OpenTabFile(szFilePath);
    KGLOG_PROCESS_ERROR(Tab);

    Tab->GetInteger(1, 1, 0, &nKind);

    nTabWidth = Tab->GetWidth();
    nTabHeight = Tab->GetHeight();

    KGLOG_PROCESS_ERROR(nKind + 2 == nTabWidth && nKind + 1 == nTabHeight);

    m_NpcAnimationTrack.nTrackType = nKind;

    ASSERT(m_NpcAnimationTrack.pTrackType == NULL);

    m_NpcAnimationTrack.pTrackType = (PDWORD)malloc(sizeof(DWORD) * nKind);
    KGLOG_PROCESS_ERROR(m_NpcAnimationTrack.pTrackType);

    ASSERT(m_NpcAnimationTrack.pRelation == NULL);

    m_NpcAnimationTrack.pRelation = (int*)malloc(sizeof(int) * nKind * nKind);
    KGLOG_PROCESS_ERROR(m_NpcAnimationTrack.pRelation);

    for (int nRow = 2; nRow <= nKind + 1; ++nRow)
    {
        Tab->GetInteger(nRow, 2, 0, &nValue);

        m_NpcAnimationTrack.pTrackType[nRow - 2] = nValue;

        for (int nColumn = 3; nColumn <= nKind + 2; ++nColumn)
        {
            Tab->GetInteger(nRow, nColumn, 0, &nValue);

            m_NpcAnimationTrack.pRelation[nKind * (nRow - 2) + (nColumn - 3)] = nValue;
        }
    }

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        SAFE_FREE(m_NpcAnimationTrack.pRelation);
        SAFE_FREE(m_NpcAnimationTrack.pTrackType);

        m_NpcAnimationTrack.nTrackType = 0;

        KGLogPrintf(KGLOG_ERR, "KTableList::LoadNpcAnimationRelationTable() %d %d %s\n", nTabWidth, nTabHeight, szFilePath);
    }
    return hrResult;
}

int KTableList::LoadPlayerAnimationTable()
{
    int nRetCode = false;
    int nResult = false;
    LPCTSTR szFilePath = NULL;
    KSmartTabFile sTab;
    int nValue = 0;
    int nRow = 0;
    int nColumn = 0;

    memset(m_dwPlayerAnimationIndexIDs, 0, sizeof(m_dwPlayerAnimationIndexIDs));

    for (int nIndex = 0; nIndex != RL_SCHOOL_COUNT; ++nIndex)
    {
        szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_DIRECTORY, g_szAniTableFileName[nIndex]);
        KGLOG_PROCESS_ERROR(szFilePath);

        if (!g_IsFileExist(szFilePath))
            continue;

        sTab = g_OpenTabFile(szFilePath);
        KGLOG_PROCESS_ERROR(sTab);

        for (nRow = 0; nRow != RL_ANIMATION_TABLE_COUNT; ++nRow)
        {
            for (nColumn = 0; nColumn != RL_WEAPON_COUNT; ++nColumn)
            {
                nRetCode = sTab->GetInteger(g_szAnimationSectionName[nRow], g_szWeaponName[nColumn], 0, &nValue);
                KGLOG_PROCESS_ERROR(nRetCode == 1);

                m_dwPlayerAnimationIndexIDs[nIndex][nRow][nColumn] = (DWORD)nValue;
            }
        }
    }

    nResult = true;
Exit0:
    if (!nResult)
    {
        KGLogPrintf(KGLOG_ERR, "KTableList::LoadPlayerAnimationTable() %s %s %s\n", g_szAnimationSectionName[nRow], g_szWeaponName[nColumn], szFilePath);
    }
    return nResult;
}

HRESULT KTableList::LoadPlayerAllAnimationModel()
{
    int nRetCode = false;
    HRESULT hrResult = E_FAIL;
    LPCTSTR szFilePath = NULL;

    // Player animation index
    for (int nRoleType = 0; nRoleType != rtTotal; ++nRoleType)
    {
        KRLAnimationIndexData& ad = m_RoleTypeData[nRoleType].AnimationIndexData;

        szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_DIRECTORY, g_szPlayerAnimationIndexFiles[nRoleType]);
        KGLOG_PROCESS_ERROR(szFilePath);

        nRetCode = ad.Tab.Load(szFilePath, "isii");
        KGLOG_PROCESS_ERROR(nRetCode);

        ad.pModel = (KRLAnimationIndex*)ad.Tab.Data(ad.nCount);
        KGLOG_PROCESS_ERROR(ad.pModel);
    }

    // Player random animation
    for (int nRoleType = 0; nRoleType != rtTotal; ++nRoleType)
    {
        KRLRandomAnimationData& ad = m_RoleTypeData[nRoleType].RandomAnimationData;

        szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_DIRECTORY, g_szPlayerRandomAnimationFiles[nRoleType]);
        KGLOG_PROCESS_ERROR(szFilePath);

        nRetCode = ad.Tab.Load(szFilePath, "isiiiiiiii");
        KGLOG_PROCESS_ERROR(nRetCode);

        ad.pModel = (KRLRandomAnimation*)ad.Tab.Data(ad.nCount);
        KGLOG_PROCESS_ERROR(ad.pModel);

        nRetCode = InitRandomAnimation(ad.pModel, ad.nCount);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    // Player sequence animation index
    for (int nRoleType = 0; nRoleType != rtTotal; ++nRoleType)
    {
        KRLSequenceAnimationData& ad = m_RoleTypeData[nRoleType].SequenceAnimationData;

        szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_DIRECTORY, g_szPlayerSequenceAnimationFiles[nRoleType]);
        KGLOG_PROCESS_ERROR(szFilePath);

        nRetCode = ad.Tab.Load(szFilePath, "isiiiiiiii");
        KGLOG_PROCESS_ERROR(nRetCode);

        ad.pModel = (KRLSequenceAnimation*)ad.Tab.Data(ad.nCount);
        KGLOG_PROCESS_ERROR(ad.pModel);
    }

    // Player animation model
    for (int nRoleType = 0; nRoleType != rtTotal; ++nRoleType)
    {
        KPlayerAnimationData& ad = m_RoleTypeData[nRoleType].AnimationData;

        szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_DIRECTORY, g_szPlayerAnimationFiles[nRoleType]);
        KGLOG_PROCESS_ERROR(szFilePath);

        nRetCode = ad.Tab.Load(szFilePath, "iiiffis");
        KGLOG_PROCESS_ERROR(nRetCode);

        ad.pModel = (KRLPlayerAnimationModel*)ad.Tab.Data(ad.nCount);
        KGLOG_PROCESS_ERROR(ad.pModel);
    }

    // Player animation adjust
    for (int nRoleType = 0; nRoleType != rtTotal; ++nRoleType)
    {
        KPlayerAdjustAnimationData& ad = m_RoleTypeData[nRoleType].AdjustAnimationData;

        szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_DIRECTORY, g_szPlayerAnimationAdjustFiles[nRoleType]);
        KGLOG_PROCESS_ERROR(szFilePath);

        nRetCode = ad.Tab.Load(szFilePath, "iiiiii");
        KGLOG_PROCESS_ERROR(nRetCode);

        ad.pModel = (KRLPlayerAdjustAnimationIndex*)ad.Tab.Data(ad.nCount);
        KGLOG_PROCESS_ERROR(ad.pModel);
    }

    // Player terrain animation adjust
    for (int nRoleType = 0; nRoleType != rtTotal; ++nRoleType)
    {
        KRLTerrainAdjustAnimationData& ad = m_RoleTypeData[nRoleType].TerrainAdjustAnimationData;

        szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_DIRECTORY, g_szPlayerAnimationTerrainAdjustFiles[nRoleType]);
        KGLOG_PROCESS_ERROR(szFilePath);

        nRetCode = ad.Tab.Load(szFilePath, "iisi");
        KGLOG_PROCESS_ERROR(nRetCode);

        ad.pModel = (KRLTerrainAdjustAnimation*)ad.Tab.Data(ad.nCount);
        KGLOG_PROCESS_ERROR(ad.pModel);
    }

    // Player animation fusion
    for (int nRoleType = 0; nRoleType != rtTotal; ++nRoleType)
    {
        KPlayerAnimationFusionData& ad = m_RoleTypeData[nRoleType].AnimationFusionData;

        szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_PLAYER_DIRECTORY, g_szPlayerAnimationFusionFiles[nRoleType]);
        KGLOG_PROCESS_ERROR(szFilePath);

        nRetCode = ad.Tab.Load(szFilePath, "iisi");
        KGLOG_PROCESS_ERROR(nRetCode);

        ad.pModel = (KRLAnimationFusion*)ad.Tab.Data(ad.nCount);
        KGLOG_PROCESS_ERROR(ad.pModel);
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KTableList::LoadPlayerAnimationRelationTable(RL_ANIMATION_TRACK nAnimationTrack, int nFileIndex)
{
    HRESULT hrResult = E_FAIL;
    LPCTSTR szFilePath = NULL;
    KSmartTabFile Tab;
    int nValue = 0;
    int nKind = 0;

    szFilePath = g_pRL->m_FilePath.GetFilePath(nFileIndex);
    KGLOG_PROCESS_ERROR(szFilePath);

    Tab = g_OpenTabFile(szFilePath);
    KGLOG_PROCESS_ERROR(Tab);

    Tab->GetInteger(1, 1, 0, &nKind);
    KGLOG_PROCESS_ERROR(nKind == Tab->GetWidth() - 2 && nKind == Tab->GetHeight() - 1);

    m_PlayerAnimationTracks[nAnimationTrack].nTrackType = nKind;

    ASSERT(m_PlayerAnimationTracks[nAnimationTrack].pTrackType == NULL);

    m_PlayerAnimationTracks[nAnimationTrack].pTrackType = (PDWORD)malloc(sizeof(DWORD) * nKind);
    KGLOG_PROCESS_ERROR(m_PlayerAnimationTracks[nAnimationTrack].pTrackType);

    ASSERT(m_PlayerAnimationTracks[nAnimationTrack].pRelation == NULL);

    m_PlayerAnimationTracks[nAnimationTrack].pRelation = (int*)malloc(sizeof(int) * nKind * nKind);
    KGLOG_PROCESS_ERROR(m_PlayerAnimationTracks[nAnimationTrack].pRelation);

    for (int nRow = 2; nRow <= nKind + 1; ++nRow)
    {
        Tab->GetInteger(nRow, 2, 0, &nValue);

        m_PlayerAnimationTracks[nAnimationTrack].pTrackType[nRow - 2] = nValue;

        for (int nColumn = 3; nColumn <= nKind + 2; ++nColumn)
        {
            Tab->GetInteger(nRow, nColumn, 0, &nValue);

            m_PlayerAnimationTracks[nAnimationTrack].pRelation[nKind * (nRow - 2) + (nColumn - 3)] = nValue;
        }
    }

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        SAFE_FREE(m_PlayerAnimationTracks[nAnimationTrack].pRelation);
        SAFE_FREE(m_PlayerAnimationTracks[nAnimationTrack].pTrackType);

        m_PlayerAnimationTracks[nAnimationTrack].nTrackType = 0;
    }
    return hrResult;
}

HRESULT KTableList::LoadPlayerAnimationRelationTable()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    int nLoadTrackLow = false;
    int nLoadTrackHigh = false;

    hr = LoadPlayerAnimationRelationTable(RL_ANIMATION_TRACK_DOWN, KFilePath::FILE_PATH_PLAYER_ANIMATION_RELATION_DOWN_TABLE);
    KGLOG_COM_PROCESS_ERROR(hr);

    nLoadTrackLow = true;

    hr = LoadPlayerAnimationRelationTable(RL_ANIMATION_TRACK_UP, KFilePath::FILE_PATH_PLAYER_ANIMATION_RELATION_UP_TABLE);
    KGLOG_COM_PROCESS_ERROR(hr);

    nLoadTrackHigh = true;

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (nLoadTrackHigh)
        {
            m_PlayerAnimationTracks[RL_ANIMATION_TRACK_UP].Exit();
        }
        if (nLoadTrackHigh)
        {
            m_PlayerAnimationTracks[RL_ANIMATION_TRACK_DOWN].Exit();
        }
    }
    return hrResult;
}

// Common Function
LPCTSTR	GetRidesSocketName(size_t nIndex)
{
    KGLOG_PROCESS_ERROR(nIndex < MODEL_RIDES_SOCKET_END);

    return g_aszRidesSocketName[nIndex];
Exit0:
    return NULL;
}

LPCTSTR	GetNpcSocketName(size_t nIndex)
{
    KGLOG_PROCESS_ERROR(nIndex >= 0 && nIndex < RL_NPC_SOCKET_COUNT);

    return g_aszNpcSocketName[nIndex];
Exit0:
    return NULL;
}

LPCTSTR	GetPlayerSheathSocketLH(int nSheathSocket)
{
    ASSERT(nSheathSocket >= 0 && nSheathSocket < RL_SHEATH_COUNT);
    return g_szSheathSocketLH[nSheathSocket];
}

LPCTSTR	GetPlayerSheathSocketRH(int nSheathSocket)
{
    ASSERT(nSheathSocket >= 0 && nSheathSocket < RL_SHEATH_COUNT);
    return g_szSheathSocketRH[nSheathSocket];
}

KDoodad* GetDoodad(DWORD dwDoodad)
{
    return g_pRL->m_pSO3WorldClient->GetDoodad(dwDoodad);
}

KPlayer* GetPlayer(DWORD dwPlayer)
{
    return g_pRL->m_pSO3WorldClient->GetPlayer(dwPlayer);
}

KNpc* GetNpc(DWORD dwNpc)
{
    return g_pRL->m_pSO3WorldClient->GetNpc(dwNpc);
}

