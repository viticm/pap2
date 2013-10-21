#ifndef KRLCHARACTERRESOURCE_H
#define KRLCHARACTERRESOURCE_H

#include "./case/render/krlresource.h"
#include "./common/kmathtools.h"

enum RL_LOAD_OPTIMIZATION
{
    RL_MODEL_OPTIMIZATION_LOAD_NONE,
    RL_MODEL_OPTIMIZATION_LOAD_NORMAL,
    RL_MODEL_OPTIMIZATION_LOAD_UNIQUE_UNIFORM,
    RL_MODEL_OPTIMIZATION_LOAD_GENDER_UNIFORM,
    RL_MODEL_OPTIMIZATION_LOAD_ROLE_TYPE_UNIFORM,
    RL_MODEL_OPTIMIZATION_LOAD_FORCE_UNIFORM,
};

enum RL_PLAYER_UNIFORM
{
    RL_PLAYER_UNIFORM_UNIQUE,

    RL_PLAYER_UNIFORM_MALE,
    RL_PLAYER_UNIFORM_FEMALE,
};

enum RL_PLAYER_ROLETYPE_UNIFORM
{
    RL_PLAYER_ROLETYPE_UNIFORM_STANDARD_MALE,
    RL_PLAYER_ROLETYPE_UNIFORM_STANDARD_FEMALE,

    RL_PLAYER_ROLETYPE_UNIFORM_STRONG_MALE,
    RL_PLAYER_ROLETYPE_UNIFORM_SEXY_FEMALE,

    RL_PLAYER_ROLETYPE_UNIFORM_LITTLE_BOY,
    RL_PLAYER_ROLETYPE_UNIFORM_LITTLE_GIRL,
};

enum RL_NPC_UNIFORM
{
    RL_NPC_UNIFORM_NONE,
    RL_NPC_UNIFORM_TRANSPARENCY,
    RL_NPC_UNIFORM_TRANSPARENCY_CORPSE,
    RL_NPC_UNIFORM_TRANSPARENCY_UNSELECTABLE,
    RL_NPC_UNIFORM_UNIQUE,
};

struct KRLResourceLimit;

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
    TCHAR szMeshFilePath[RL_FILE_PATH_LENGTH];
    TCHAR szMtlFilePath[RL_FILE_PATH_LENGTH];
    TCHAR szSFX1FilePath[RL_FILE_PATH_LENGTH];
    TCHAR szSFX2FilePath[RL_FILE_PATH_LENGTH];
    TCHAR szSocketName[RL_SOCKET_NAME_LENGTH];

    float fMeshScale;
    int nColorChannel;
    int nGlobalColorChannel;

    KRLSocketResource()
    {
        szMeshFilePath[0] = _T('\0');
        szMtlFilePath[0] = _T('\0');
        szSFX1FilePath[0] = _T('\0');
        szSFX2FilePath[0] = _T('\0');
        szSocketName[0] = _T('\0');

        fMeshScale = 1.0f;
        nColorChannel = 0;
        nGlobalColorChannel = 0;
    };
};

struct KRLRidesEquipResource
{
    TCHAR szMeshFilePath[RL_FILE_PATH_LENGTH];
    TCHAR szMtlFilePath[RL_FILE_PATH_LENGTH];

    float fMeshScale;
    int nColorChannel;
    int nGlobalColorChannel;

    KRLRidesEquipResource()
    {
        szMeshFilePath[0] = _T('\0');
        szMtlFilePath[0] = _T('\0');

        fMeshScale = 1.0f;
        nColorChannel = 0;
        nGlobalColorChannel = 0;
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

int GetCharacterModel(KRLResourceLimit* pOptimization, KCharacter* pCharacter, KRLCharacterEquipResource* pResource);

#endif // KRLCHARACTERRESOURCE_H

