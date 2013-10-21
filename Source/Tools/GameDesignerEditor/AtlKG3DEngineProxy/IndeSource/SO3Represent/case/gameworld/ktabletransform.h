#ifndef RL_GAMEWORLD_TABLE_KTABLETRANSFORM_H
#define RL_GAMEWORLD_TABLE_KTABLETRANSFORM_H

#include "./ktabledefine.h"
#include "../render/kmodel.h"
#include "../render/kanimationmodel.h"
#include "../render/krlresource.h"
#include "../../common/kmathtools.h"
#include "../../base/missile/krepresentmissiledefine.h"

struct KSkillCasterModel;

struct KRLMDLResource
{
    LPCTSTR	szMDLFilePath;
    float fMDLScale;
    BOOL bSelectable;
};

struct KRLCharacterModelResource
{
    LPCTSTR	szMeshFilePath;
    LPCTSTR	szMtlFilePath;
    LPCTSTR szSFX1FilePath;
    LPCTSTR szSFX2FilePath;
    float fMeshScale;
    int nColorChannel;
};

struct KRLPlayerEquipResource
{
    ROLE_TYPE nRoleType;

    RL_WEAPON nWeaponType;
    RL_SHEATH nSheathSocket;

    DWORD dwSkillID;
    DWORD dwSkillLevel;

    DWORD dwRepresentIDs[perRepresentCount];

    KRLCharacterModelResource Models[MODEL_PLAYER_EQUIP_COUNT];
};

struct KRLSocketResource
{
    LPCTSTR	szMeshFilePath;
    LPCTSTR	szMtlFilePath;
    LPCTSTR szSFX1FilePath;
    LPCTSTR szSFX2FilePath;
    LPCTSTR szSocketName;
    float fMeshScale;
    int nColorChannel;

    KRLSocketResource()
    {
        szMeshFilePath = _T("");
        szMtlFilePath = _T("");
        szSFX1FilePath = _T("");
        szSFX2FilePath = _T("");
        szSocketName = _T("");
        fMeshScale = 1.0f;
        nColorChannel = 0;
    };
};

struct KRLCharacterEquipResource
{
    RL_OJBECT nObject;

    RL_LOAD_OPTIMIZATION nLoadOptimization;

    BOOL bSplitModel;
    DWORD dwModelID;

    RL_TERRAIN_SLOPE nTerrainSlope;

    float fModelHeight;
    float fSelectionScale;

    KRLMDLResource MDL;

    DWORD dwEffectType;
    float fEffectScale;

    int nColorChannelTable;
    int nColorChannel;

    KRLSocketResource SocketParams[CHARACTER_SOCKET_COUNT];

    union
    {
        KRLPlayerEquipResource Player;
    };
};

struct KSkillBuffParam
{
    LPCTSTR szBoneNameLeft;
    LPCTSTR szBoneNameRight;

    KRLSFXParam SFXParam;
};

struct KSkillCasterParam
{
    DWORD dwLockControlTime;
    DWORD dwPrepareAnimationID;
    DWORD dwCastSkillAnimationIDs[RL_SKILL_SEQUENCE_COUNT];
};

struct KSoundParam
{
    LPCTSTR szFile;
    float fMinRange;    // รื
    float fMaxRange;    // รื
};

struct KRLAnimationParam
{
    RL_SHEATH_SOCKET nSheathSocket;
    DWORD dwKindID;
    DWORD dwSplitType;
    float fRatio;

    union
    {
        KRLMonoAnimationParam MonoAnimationParam;
        KRLSplitAnimationParam SplitAnimationParam;
    };
};

struct KRLSerialAnimation
{
    DWORD dwPhaseAnimationIDs[3];
};

int GetRidesSocketModel(KRLCharacterEquipResource const& EquipResource, DWORD dwRidesID, KRLSocketResource* pSocket);
int GetNpcSocketByAnimation(KRLCharacterEquipResource const& EquipResource, DWORD dwAnimationID, KRLSocketResource* pSocket, BOOL& bLoadDefult);

int GetCharacterModel(KCharacter* pCharacter, KRLCharacterEquipResource* pResource);
int GetCharacterSkillCasterParam(KRLCharacterEquipResource const& EquipResource, DWORD dwSkillID, DWORD dwSkillLevel, KSkillCasterParam* pSkillCasterParam);
int GetCharacterSkillBuff(KRLCharacterEquipResource const& EquipResource, DWORD dwSkillBuffs[SKILL_BUFF_PART_COUNT], KSkillBuffParam* pSkillBuff);
int GetCharacterLevelUpSFX(KRLCharacterEquipResource const& EquipResource, REPRESENT_SFX nSFX, KRLSFXParam* pSFXParam);
int GetCharacterQuestMarkOrSkillBuffHeight(KCharacter* pCharacter, float& fHeight);
int GetCharacterAnimation(KRLCharacterEquipResource const& EquipResource, DWORD dwAnimationID, KRLAnimationParam* pAnimationParam);
int GetCharacterAnimationFusion(KRLCharacterEquipResource const& EquipResource, DWORD dwAnimationID, DWORD dwPrevAnimationID, PDWORD pdwTweenTime);
BOOL GetCharacterSerialAnimation(BOOL bPlayer, DWORD dwSerialID, KRLSerialAnimation* pSerialAnimation);
BOOL GetCharacterAdjustAnimationIndex(KRLCharacterParam const* pParam, DWORD dwAdjustID, KRLAdjustAnimationIndex* pAdjustAnimationIndex);
int GetCharacterAnimationIndex(KRLCharacterEquipResource const& EquipResource, RL_SCHOOL nSchool, RL_ANIMATION_TABLE nAnimationTable, RL_WEAPON nWeapon, KRLAdjustAnimationParam const* pAdjustAnimationParam, KRLAnimationIndex* pAnimationIndex);
DWORD GetCharacterRandomAnimation(KRLCharacterEquipResource const& EquipResource, DWORD dwRandomAnimationID);

void FillChestResource(KPlayerChestEquipModel const* pModel, KPlayerEquipSFXModel const* pSFXModel, KRepresentPlayerEquipPartResource* pPart);
void FillHairResource(KPlayerHairModel const* pModel, KRepresentPlayerEquipPartResource* pPart);
void FillHatResource(KPlayerHairModel const* pHeadformModel, KPlayerHatModel const* pHatModel, KPlayerEquipSFXModel const* pHatSFXModel, KRepresentPlayerEquipPartResource* pHatPart);
void FillWaistResource(KPlayerEquipModel const* pModel, KPlayerEquipSFXModel const* pSFXModel, KRepresentPlayerEquipPartResource* pPart);
void FillPantsResource(KPlayerEquipModel const* pModel, KRepresentPlayerEquipPartResource* pPart);
void FillBangleResource(KPlayerEquipModel const* pModel, KPlayerEquipSFXModel const* pSFXModel, KRepresentPlayerEquipPartResource* pPart);
void FillMeleeWeaponResource(KPlayerWeaponModel const* pModel, KPlayerEquipSFXModel const* pMeteorModel, KPlayerEquipSFXModel const* pFringeModel, KRepresentPlayerEquipPartResource* pPartLH, KRepresentPlayerEquipPartResource* pPartRH);
void FillBackExtendResource(KPlayerEquipModel const* pModel, KRepresentPlayerEquipPartResource* pPart);
void FillWaistExtendResource(KPlayerEquipModel const* pModel, KRepresentPlayerEquipPartResource* pPart);
void FillFaceResource(KPlayerEquipModel const* pModel, KRepresentPlayerEquipPartResource* pPart);

float GetPlayerBaseModelScale(KRLCharacterEquipResource const& EquipResource);
KSkillCasterModel const* GetCharacterSkillCasterModel(KCaster const& Caster, DWORD dwSkillID, DWORD dwSkillLevel);
int GetCharacterMissileID(DWORD dwSkillID, DWORD dwSkillLevel, KCaster const* pCaster, DWORD& dwMissileID);

int GetCharacterSkillCaster(KCharacter* pCharacter, KCaster* pCaster);

KForceRelationModel const* GetForceRelationModel(DWORD dwSrcCharacterID, DWORD dwDstCharacterID);

HRESULT LoadFixedTrackMissileFrameFile(int nIndex, D3DXVECTOR3* vPosition);
int GetSkillEffectID(DWORD* pEffectID, int* pEffectIndex, size_t nCount, KCaster const& Caster, DWORD dwSkillID, DWORD dwSkillLevel);
HRESULT GetDoodadModelResource(DWORD dwRepresentID, KRepresentDoodadResource* pResource);
HRESULT GetRidesEquipModelResource(DWORD dwRidesID, DWORD dwRepresentIDs[MODEL_RIDES_EQUIP_AND_SOCKET_COUNT], KRepresentRidesEquipResource* pResource);
HRESULT GetRidesAnimationModelResource(DWORD dwRidesID, DWORD dwAnimationID, KRepresentRidesAnimationResource* pResource);
HRESULT GetNpcEquipModelResource(DWORD dwRepresentID, KRepresentNPCEquipResource* pResource);
HRESULT GetNpcAnimationModelResource(DWORD dwRepresentID, DWORD dwAnimationID, KRepresentNPCAnimationResource* pResource);
HRESULT GetPlayerEquipModelResource(DWORD const dwRepresentIDs[perRepresentCount], ROLE_TYPE nRoleType, KRepresentPlayerEquipResource* pResource);
HRESULT GetPlayerAnimationModelResource(DWORD dwAnimationID, ROLE_TYPE nRoleType, KRepresentPlayerAnimationResource* pResource);

enuAniPlayMode AnimationPlayTypeRLTo3D(RL_ANIMATION_PLAY_TYPE nRLPlayType);

RL_FORCE_RELATION ForceRelationGWToRL(int nType);

int InitRandomAnimation(KRLRandomAnimation* pRandomAnimation, size_t nCount);
DWORD GetRandomAnimationID(KRLRandomAnimation const* pRandomAnimation);

#endif // RL_GAMEWORLD_TABLE_KTABLETRANSFORM_H