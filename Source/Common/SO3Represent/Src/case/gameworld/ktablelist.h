#ifndef RL_GAMEWORLD_TABLE_KTABLELIST_H
#define RL_GAMEWORLD_TABLE_KTABLELIST_H

#include "../gameworld/ktabledefine.h"
#include "../../base/file/kbintab.h"
#include "../../base/file/ikg_tabfile.h"

class KTableList
{
public:
    KTableList();
    ~KTableList();

    BOOL Init();
    void Exit();
    BOOL Reset();

    DWORD GetRidesIDByTrackID(DWORD dwTrackID) const;
    DWORD GetPlayerAnimationIndexID(RL_SCHOOL nSchool, RL_ANIMATION_TABLE nAnimationTable, RL_WEAPON nWeapon) const;
    DWORD GetPlayerAnimationTrackType(DWORD dwKindID, RL_ANIMATION_TRACK nAnimationTrack);
    DWORD GetNPCAnimationTrackType(DWORD dwKindID);
    int   GetCharacterAnimationRelation(BOOL bPlayer, DWORD dwPreviousAnimationKindID, DWORD dwCurrentAnimationKindID, RL_ANIMATION_TRACK nAnimationTrack);
    int   HitAnimationPlayRate(float ffRatio) const;
    float GetNumber(DWORD dwID) const;
    DWORD GetRidesAnimationFusion(DWORD dwRidesRepresentID, DWORD dwPreviousAnimationID, DWORD dwCurrentAnimationID) const;
    float GetNpcModelHeight(DWORD dwRepresentID) const;
    DWORD GetNpcAnimationFusion(DWORD dwPreviousAnimationID, DWORD dwAnimationAnimationID) const;
    DWORD GetPlayerAnimationFusion(ROLE_TYPE nRoleType, DWORD dwPreviousAnimationID, DWORD dwCurrentAnimationID) const;

    KRLSoundModel          const* GetSoundModel(DWORD dwSoundID) const;
    KGlobalEffectModel     const* GetGlobalEffectModel(DWORD dwEffectID) const;
    KForceRelationModel    const* GetForceRelationModel(int nForceRelationType) const;
    KRLCursorEffectModel const* GetCursorEffectModel(DWORD dwEffectID) const;

    KRLMissileModel const* GetMissileModel(DWORD dwMissileID) const;
    KRLMissileModelParam const* GetMissileModelParam(DWORD dwMissileParamID) const;
    KRLMissileHomingParam const* GetMissileHomingParam(DWORD dwMissileParamID) const;
    KRLMissileTimelineParam const* GetMissileTimelineParam(DWORD dwMissileParamID) const;
    KRLMissileYAxisParam const* GetMissileYAxisParam(DWORD dwMissileParamID) const;

    KSkillBuffModel        const* GetSkillBuffModel(DWORD dwPart, DWORD dwSkillBuffID) const;
    KSkillCasterModel const* GetPlayerSkillCasterModel(DWORD dwSkillID, DWORD dwSkillLevel, ROLE_TYPE nRoleType) const;
    KSkillCasterModel      const* GetNpcSkillCasterModel(DWORD dwSkillID, DWORD dwSkillLevel, DWORD dwRepresentID) const;
    KSkillEffectModel      const* GetSkillEffectModel(DWORD dwEffectID) const;
    KSkillResultModel      const* GetSkillResultModel(DWORD dwEffectResultID, DWORD dwEffectType) const;
    KSkillChainModel       const* GetSkillChainModel(DWORD dwSkillID, DWORD dwSkillLevel) const;
    KRLDoodadModel const* GetDoodadModel(DWORD dwRepresentID) const;
    KDoodadEffectModel     const* GetEffectModel(DWORD dwEffectID) const;
    KRidesModel            const* GetRidesModel(DWORD dwRepresentID) const;
    KRidesEquipModel const* GetRidesEquipModel(int nIndex, DWORD dwModelID) const;
    KRidesSocketModel const* GetRidesSocketModel(int nIndex, DWORD dwModelID) const;
    KRidesAnimationModel   const* GetRidesAnimationModel(DWORD dwRepresentID, DWORD dwAnimationID) const;
    KRLNpcModel const* GetNpcModel(DWORD dwRepresentID) const;
    int GetNpcAnimationModel(DWORD dwRepresentID, DWORD dwAnimationID, KNpcAnimationModel *pRetRsult);
    KRLNpcAdjustAnimationIndex const* GetNpcAdjustAnimationModel(DWORD dwRepresentID, DWORD dwAnimationID) const;
    KNpcSerialAnimation const* GetNpcSerialAnimationModel(DWORD dwSerialID) const;
    KRLPlayerModel const* GetPlayerModel(ROLE_TYPE nRoleType) const;
    KRLPlayerAnimationModel const* GetPlayerAnimationModel(DWORD dwAnimationID, ROLE_TYPE nRoleType) const;
    KRLAnimationIndex const* GetPlayerAnimationIndex(DWORD dwAnimationIndexID, ROLE_TYPE nRoleType) const;
    KRLRandomAnimation const* GetPlayerRandomAnimation(DWORD dwRandomAnimationID, ROLE_TYPE nRoleType) const;
    KRLSequenceAnimation const* GetPlayerSequenceAnimation(DWORD dwSequenceAnimationID, ROLE_TYPE nRoleType) const;
    KPlayerBuffAnimation const* GetPlayerBuffAnimation(DWORD dwPart, DWORD dwBuffID, DWORD dwAnimationID) const;
    KPlayerSerialAnimation const* GetPlayerSerialAnimation(DWORD dwSerialID) const;
    KPlayerCraftAnimation  const* GetPlayerCraftAnimation(int nCraftID) const;
    KRLPlayerAdjustAnimationIndex const* GetPlayerAdjustAnimation(DWORD dwAnimationID, ROLE_TYPE nRoleType) const;
    KRLTerrainAdjustAnimation const* GetPlayerTerrainAdjustAnimation(DWORD dwAnimationID, DWORD dwTerrainID, ROLE_TYPE nRoleType) const;
    KPlayerChestEquipModel const* GetPlayerChestEquipModel(DWORD dwRepresentEquipID, ROLE_TYPE nRoleType) const;
    int GetOriginalHairIndex(DWORD dwHeadID, DWORD *pdwHeadformID, DWORD *pdwBangID, DWORD *pdwPlaitID) const;
    int GetFinallyHairIndex(DWORD dwOriginalHairID, int nHatType, DWORD *pdwFinallyHairID) const;
    KPlayerHairModel const* GetPlayerHairModel(DWORD dwRepresentEquipID, ROLE_TYPE nRoleType) const;
    KPlayerEquipModel const* GetPlayerWaistModel(DWORD dwRepresentEquipID, ROLE_TYPE nRoleType) const;
    KPlayerEquipModel const* GetPlayerPantsModel(DWORD dwRepresentEquipID, ROLE_TYPE nRoleType) const;
    KPlayerEquipModel const* GetPlayerBangleModel(DWORD dwRepresentEquipID, ROLE_TYPE nRoleType) const;
    KPlayerWeaponModel const* GetPlayerWeaponModel(DWORD dwRepresentEquipID, ROLE_TYPE nRoleType) const;
    KPlayerEquipModel const* GetPlayerBackExtendModel(DWORD dwRepresentEquipID, ROLE_TYPE nRoleType) const;
    KPlayerEquipModel const* GetPlayerWaistExtendModel(DWORD dwRepresentEquipID, ROLE_TYPE nRoleType) const;
    KPlayerEquipModel const* GetPlayerFaceModel(DWORD dwRepresentEquipID, ROLE_TYPE nRoleType) const;
    KPlayerHatModel const* GetPlayerHatModel(DWORD dwRepresentEquipID, ROLE_TYPE nRoleType) const;
    KPlayerEquipSFXModel const* GetPlayerSFXModel(DWORD dwRepresentEquipID, DWORD dwRepresentEnchantID, int nEquipPart, ROLE_TYPE nRoleType) const;

    KRLPlayerUniformModel const* GetPlayerUniformModel(DWORD dwUniformID) const;
    KRLPlayerUniformModel const* GetPlayerRoleTypeUniformModel(ROLE_TYPE nRoleType) const;
    KRLPlayerUniformModel const* GetPlayerForceUniformModel(DWORD dwForceID) const;

    KRLNpcUniformModel const* GetNpcUniformModel(DWORD dwUniformID) const;

    KRLDoodadUniformModel const* GetDoodadUniformModel(DWORD dwUniformID) const;

    KRLOptimizationModel const* GetOptimizationModel(DWORD dwLevel) const;

    KRLPlayerEquipSFXSequence const* GetPlayerEquipSFXSequence(DWORD dwEquipType, ROLE_TYPE nRoleType, DWORD dwEquipID, DWORD dwSequenceID) const;
    KRLPlayerEquipSFXSequenceIndex const* GetPlayerEquipSFXSequenceIndex(DWORD dwEquipType) const;
    KRLPlayerEquipSFXModelParam const* GetPlayerEquipSFXModelParam(DWORD dwSFXID) const;

private:    
    int LoadMissileConfig();
    HRESULT LoadSkillCastModel();
    HRESULT LoadRidesEquipAndSocketModel();
    HRESULT LoadNpcAnimationRelationTable();
    int LoadPlayerAllModel();
    HRESULT LoadPlayerConfigFile();
    int LoadPlayerAnimationTable();
    HRESULT LoadPlayerAllAnimationModel();
    HRESULT LoadPlayerAnimationRelationTable();
    HRESULT LoadPlayerAnimationRelationTable(RL_ANIMATION_TRACK nAnimationTrack, int nFileIndex);
    int LoadPlayerEquipSFXSequence();

private:
    struct MAP_ID
    {
        DWORD dwValue1;
        DWORD dwValue2;

        bool operator<(MAP_ID const& rhs) const
        {
            if (dwValue1 == rhs.dwValue1)
                return dwValue2 < rhs.dwValue2;
            return dwValue1 < rhs.dwValue1;
        }
    };

    struct KAnimationTrackData
    {
        KAnimationTrackData() : pRelation(NULL), pTrackType(NULL), nTrackType(0) 
        {        
        }

        ~KAnimationTrackData()
        {
            ASSERT(pRelation == NULL);
            ASSERT(pTrackType == NULL);

            ASSERT(nTrackType == 0);
        }

        void Exit()
        {
            SAFE_FREE(pRelation);
            SAFE_FREE(pTrackType);

            nTrackType = 0;
        }

        int* pRelation;
        PDWORD pTrackType;
        size_t nTrackType;
    };

    // Sound
    KBinTab m_SoundDataTab;
    KRLSoundModel* m_pSoundModel;
    size_t m_nSoundlCount;

    KBinTab	m_NumberTab;
    KRLNumberModel* m_pNumberModel;
    size_t m_nNumberCount;

    KBinTab	m_GlobalEffectTab;
    KGlobalEffectModel* m_pGlobalEffectModel;
    size_t m_nGlobalEffectCount;

    KBinTab	m_CursorEffectTab;
    KRLCursorEffectModel* m_pCursorEffectModel;
    size_t m_nCursorEffectCount;

    // Relation
    KBinTab	m_RelationTab;
    KForceRelationModel* m_pRelationModel;
    size_t m_nRelationCount;

    // Missile
    KBinTab	m_PhaseLineTab;
    KMissilePhaseLineModel* m_pPhaseLineModel;
    size_t m_nPhaseLineCount;

    KBinTab	m_PhaseCircleTab;
    KMissilePhaseCircleModel* m_pPhaseCircleModel;
    size_t m_nPhaseCircleCount;

    KBinTab	m_PhaseFixedTrackTab;
    KMissilePhaseFixedTrackModel* m_pPhaseFixedTrackModel;
    size_t m_nPhaseFixedTrackCount;

    KBinTab	m_PhaseFulminationTab;
    KMissilePhaseFulminationModel* m_pPhaseFulminationModel;
    size_t m_nPhaseFulminationCount;

    KBinTab	m_MissileModelParamTab;
    KRLMissileModelParam* m_pMissileModelParam;
    size_t m_nMissileModelParamCount;

    KBinTab	m_MissileHomingParamTab;
    KRLMissileHomingParam* m_pMissileHomingParam;
    size_t m_nMissileHomingParamCount;

    KBinTab	m_MissileTimelineParamTab;
    KRLMissileTimelineParam* m_pMissileTimelineParam;
    size_t m_nMissileTimelineParamCount;

    KBinTab	m_MissileYAxisParamTab;
    KRLMissileYAxisParam* m_pMissileYAxisParam;
    size_t m_nMissileYAxisParamCount;

    KBinTab	m_MissileTab;
    KRLMissileModel* m_pMissileModel;
    size_t m_nMissileCount;

    // Skill
    KBinTab	m_SkillBuffTab;
    KSkillBuffModel* m_pSkillBuffModel;
    size_t m_nSkillBuffCount;

    KBinTab m_PlayerSkillCasterTab[rtTotal];
    KSkillCasterModel* m_pPlayerSkillCasterModel[rtTotal];
    size_t m_nPlayerSkillCasterCount[rtTotal];

    KBinTab m_NpcSkillCasterTab;
    KNpcSkillCasterModel* m_pNpcSkillCasterModel;
    size_t m_nNpcSkillCasterCount;

    KBinTab m_SkillEffectTab;
    KSkillEffectModel* m_pSkillEffectModel;
    size_t m_nSkillEffectCount;

    KBinTab m_SkillResultTab;
    KSkillResultModel* m_pSkillResultModel;
    size_t m_nSkillResultCount;

    KBinTab m_SkillChainTab;
    KSkillChainModel* m_pSkillChainModel;
    size_t m_nSkillChainCount;

    // Doodad
    KBinTab	m_DoodadTab;
    KRLDoodadModel* m_pDoodadModel;
    size_t m_nDoodadCount;

    KBinTab	m_DoodadEffectTab;
    KDoodadEffectModel* m_pDoodadEffectModel;
    size_t m_nDoodadEffectCount;

    // Rides
    KBinTab	m_RidesTab;
    KRidesModel* m_pRidesModel;
    size_t m_nRidesCount;

    KBinTab	m_RidesEquipTab[MODEL_RIDES_EQUIP_COUNT];
    KRidesEquipModel* m_pRidesEquipModel[MODEL_RIDES_EQUIP_COUNT];
    size_t m_nRidesEquipCount[MODEL_RIDES_EQUIP_COUNT];

    KBinTab	m_RidesSocketTab[MODEL_RIDES_SOCKET_COUNT];
    KRidesSocketModel* m_pRidesSocketModel[MODEL_RIDES_SOCKET_COUNT];
    size_t m_nRidesSocketCount[MODEL_RIDES_SOCKET_COUNT];

    KBinTab	m_RidesAnimationTab;
    KRidesAnimationModel* m_pRidesAnimationModel;
    size_t m_nRidesAnimationCount;

    KBinTab	m_RidesFusionTab;
    KRidesAnimationFusion* m_pRidesFusionModel;
    size_t m_nRidesFusionCount;

    KBinTab	m_RidesTrackTab;
    KRidesForTrack* m_pRidesTrackModel;
    size_t m_nRidesTrackCount;

    // NPC
    KBinTab	m_NpcTab;
    KRLNpcModel* m_pNpcModel;
    size_t m_nNpcCount;

    IKG_TabFile *m_piNpcAnimationFile;

    KBinTab	m_NpcAnimationAdjustTab;
    KRLNpcAdjustAnimationIndex* m_pNpcAnimationAdjustModel;
    size_t m_nNpcAnimationAdjustCount;

    KBinTab	m_NpcFusionTab;
    KRLAnimationFusion* m_pNpcFusionModel;
    size_t m_nNpcFusionCount;

    KBinTab	m_NpcSerialTab;
    KNpcSerialAnimation* m_pNpcSerialModel;
    size_t m_nNpcSerialCount;

    KAnimationTrackData m_NpcAnimationTrack;

    // Player
    struct KPlayerEquipData
    {
        KBinTab	Tab;
        KPlayerEquipModel* pModel;
        size_t nCount;
    };

    struct KPlayerWeaponData
    {
        KBinTab	Tab;
        KPlayerWeaponModel* pModel;
        size_t nCount;
    };

    struct KPlayerChestData
    {
        KBinTab	Tab;
        KPlayerChestEquipModel* pModel;
        size_t nCount;
    };

    struct KPlayerHatData
    {
        KBinTab	Tab;
        KPlayerHatModel* pModel;
        size_t nCount;
    };

    struct KPlayerHairData
    {
        KBinTab	Tab;
        KPlayerHairModel* pModel;
        size_t nCount;
    };

    struct KPlayerEquipSFXData
    {
        KBinTab	Tab;
        KPlayerEquipSFXModel* pModel;
        size_t nCount;
    };

    struct KPlayerAnimationData
    {
        KBinTab	Tab;
        KRLPlayerAnimationModel* pModel;
        size_t nCount;
    };

    struct KPlayerAdjustAnimationData
    {
        KBinTab	Tab;
        KRLPlayerAdjustAnimationIndex* pModel;
        size_t nCount;
    };

    struct KPlayerAnimationFusionData
    {
        KBinTab	Tab;
        KRLAnimationFusion* pModel;
        size_t nCount;
    };

    struct KRLAnimationIndexData
    {
        KBinTab	Tab;
        KRLAnimationIndex* pModel;
        size_t nCount;
    };

    struct KRLRandomAnimationData
    {
        KBinTab	Tab;
        KRLRandomAnimation* pModel;
        size_t nCount;
    };

    struct KRLSequenceAnimationData
    {
        KBinTab	Tab;
        KRLSequenceAnimation* pModel;
        size_t nCount;
    };

    struct KRLPlayerEquipSFXSequenceData
    {
        KBinTab	Tab;
        KRLPlayerEquipSFXSequence* pModel;
        size_t nCount;
    };

    struct KRLTerrainAdjustAnimationData
    {
        KBinTab	Tab;
        KRLTerrainAdjustAnimation* pModel;
        size_t nCount;
    };

    struct KRLRoleTypeData
    {
        KPlayerEquipData EquipData[MODEL_PLAYER_EQUIP_COUNT];
        KPlayerEquipSFXData EquipSFXData[CHARACTER_EQUIP_SFX_FILE_COUNT];
        KPlayerEquipData SocketData[MODEL_PLAYER_ONLY_SOCKET_COUNT];

        KPlayerWeaponData WeaponData;
        KPlayerChestData ChestData;
        KPlayerHatData HatData;
        KPlayerHairData HairData;

        KRLAnimationIndexData AnimationIndexData;
        KRLRandomAnimationData RandomAnimationData;
        KRLSequenceAnimationData SequenceAnimationData;
        KPlayerAnimationData AnimationData;
        KPlayerAnimationFusionData AnimationFusionData;
        KPlayerAdjustAnimationData AdjustAnimationData;
        KRLTerrainAdjustAnimationData TerrainAdjustAnimationData;
    };

    KRLRoleTypeData m_RoleTypeData[rtTotal];

    KRLPlayerEquipSFXSequenceData m_PlayerEquipSFXSequenceData[perRepresentCount][rtTotal];

    KBinTab	m_PlayerBuffTab;
    KPlayerBuffAnimation* m_pPlayerBuffModel;
    size_t m_nPlayerBuffCount;

    KBinTab	m_PlayerSerialTab;
    KPlayerSerialAnimation* m_pPlayerSerialModel;
    size_t m_nPlayerSerialCount;

    KBinTab	m_PlayerCraftTab;
    KPlayerCraftAnimation* m_pPlayerCraftModel;
    size_t m_nPlayerCraftCount;

    KBinTab	m_PlayerTab;
    KRLPlayerModel* m_pPlayerModel;
    size_t m_nPlayerCount;

    KAnimationTrackData m_PlayerAnimationTracks[RL_ANIMATION_TRACK_COUNT];

    DWORD m_dwPlayerAnimationIndexIDs[RL_SCHOOL_COUNT][RL_ANIMATION_TABLE_COUNT][RL_WEAPON_COUNT];

    // Uniform

    KBinTab	m_PlayerUniformTab;
    KRLPlayerUniformModel* m_pPlayerUniformModel;
    size_t m_nPlayerUniformCount;

    KBinTab	m_PlayerRoleTypeUniformTab;
    KRLPlayerUniformModel* m_pPlayerRoleTypeUniformModel;
    size_t m_nPlayerRoleTypeUniformCount;

    KBinTab	m_PlayerForceUniformTab;
    KRLPlayerUniformModel* m_pPlayerForceUniformModel;
    size_t m_nPlayerForceUniformCount;

    KBinTab	m_NpcUniformTab;
    KRLNpcUniformModel* m_pNpcUniformModel;
    size_t m_nNpcUniformCount;

    KBinTab	m_DoodadUniformTab;
    KRLDoodadUniformModel* m_pDoodadUniformModel;
    size_t m_nDoodadUniformCount;

    KBinTab	m_OptimizationTab;
    KRLOptimizationModel* m_pOptimizationModel;
    size_t m_nOptimizationCount;

    KBinTab m_PlayerEquipSFXSequenceIndexTab;
    KRLPlayerEquipSFXSequenceIndex* m_pPlayerEquipSFXSequenceIndexModel;
    size_t m_nPlayerEquipSFXSequenceIndexCount;

    KBinTab m_PlayerEquipSFXModelParamTab;
    KRLPlayerEquipSFXModelParam* m_pPlayerEquipSFXModelParamModel;
    size_t m_nPlayerEquipSFXModelParamCount;

    KBinTab m_HairHatRelationTab;
    KPlayerHairHatRelation* m_pHairHatRelationModel;
    size_t m_uHairHatRelationCount;

    KBinTab m_HeadIndexTab;
    KPlayerHeadIndex* m_pHeadIndexModel;
    size_t m_uHeadIndexCount;
};

LPCTSTR GetNpcSocketName(size_t nIndex);
LPCTSTR GetPlayerSheathSocketLH(int nSheathSocket);
LPCTSTR GetPlayerSheathSocketRH(int nSheathSocket);

KDoodad* GetDoodad(DWORD dwDoodad);
KPlayer* GetPlayer(DWORD dwPlayer);
KNpc* GetNpc(DWORD dwNpc);

#endif //RL_GAMEWORLD_TABLE_KTABLELIST_H
