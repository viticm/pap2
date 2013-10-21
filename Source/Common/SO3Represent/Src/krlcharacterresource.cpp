#include "stdafx.h"
#include "./krlcharacterresource.h"
#include "./case/gameworld/ktabledefine.h"
#include "./case/gameworld/ktabletransform.h"
#include "./SO3Represent.h"
#include "../../Common/SO3World/Src/KPlayer.h"
#include "SO3World/IKGSO3WorldClient.h"
#include "SO3World/KSO3World.h"

static RL_LOAD_OPTIMIZATION GetRLModelLoadOptimizationVisible(DWORD dwCharacterID)
{
    RL_FORCE_RELATION nRLForceRelation = RL_FORCE_RELATION_INVALID;
    RL_LOAD_OPTIMIZATION nLoadOptimization = RL_MODEL_OPTIMIZATION_LOAD_NORMAL;

    if (IS_PLAYER(dwCharacterID))
    {
        nRLForceRelation = GetRLForceRelation(dwCharacterID, g_pRL->m_pSO3World->m_dwClientPlayerID);

        switch (nRLForceRelation)
        {
        case RL_FORCE_RELATION_INVALID:
            nLoadOptimization = RL_MODEL_OPTIMIZATION_LOAD_NONE;
            break;
        case RL_FORCE_RELATION_ENEMY:
            if (g_pRL->m_CurrentOptimizationOption.bHideEnemyPlayer)
                nLoadOptimization = RL_MODEL_OPTIMIZATION_LOAD_NONE;
            break;
        case RL_FORCE_RELATION_NEUTRALITY:
            if (g_pRL->m_CurrentOptimizationOption.bHideNeutralityPlayer)
                nLoadOptimization = RL_MODEL_OPTIMIZATION_LOAD_NONE;
            break;
        case RL_FORCE_RELATION_PARTY:
            if (g_pRL->m_CurrentOptimizationOption.bHidePartyPlayer)
                nLoadOptimization = RL_MODEL_OPTIMIZATION_LOAD_NONE;
            break;
        case RL_FORCE_RELATION_ALLY:
            if (g_pRL->m_CurrentOptimizationOption.bHideAllyPlayer)
                nLoadOptimization = RL_MODEL_OPTIMIZATION_LOAD_NONE;
            break;
        case RL_FORCE_RELATION_SELF:
            if (g_pRL->m_CurrentOptimizationOption.bHideSelf)
                nLoadOptimization = RL_MODEL_OPTIMIZATION_LOAD_NONE;
            break;
        case RL_FORCE_RELATION_NONE:
        case RL_FORCE_RELATION_ALL:
            break;
        default:
            ASSERT(0);
            break;
        }
    }
    else
    {
        nRLForceRelation = GetRLForceRelation(dwCharacterID, g_pRL->m_pSO3World->m_dwClientPlayerID);

        switch (nRLForceRelation)
        {
        case RL_FORCE_RELATION_INVALID:
            nLoadOptimization = RL_MODEL_OPTIMIZATION_LOAD_NONE;
            break;
        case RL_FORCE_RELATION_ENEMY:
            if (g_pRL->m_CurrentOptimizationOption.bHideEnemyNpc)
                nLoadOptimization = RL_MODEL_OPTIMIZATION_LOAD_NONE;
            break;
        case RL_FORCE_RELATION_NEUTRALITY:
            if (g_pRL->m_CurrentOptimizationOption.bHideNeutralityNpc)
                nLoadOptimization = RL_MODEL_OPTIMIZATION_LOAD_NONE;
            break;
        case RL_FORCE_RELATION_PARTY:
            if (g_pRL->m_CurrentOptimizationOption.bHidePartyNpc)
                nLoadOptimization = RL_MODEL_OPTIMIZATION_LOAD_NONE;
            break;
        case RL_FORCE_RELATION_ALLY:
            if (g_pRL->m_CurrentOptimizationOption.bHideAllyNpc)
                nLoadOptimization = RL_MODEL_OPTIMIZATION_LOAD_NONE;
            break;
        case RL_FORCE_RELATION_SELF:
            ASSERT(0);
            break;
        case RL_FORCE_RELATION_NONE:
        case RL_FORCE_RELATION_ALL:
            break;
        default:
            ASSERT(0);
            break;
        }
    }

    return nLoadOptimization;
}

static RL_LOAD_OPTIMIZATION GetRLPlayerModeLoadOptimization(KPlayer* pPlayer)
{
    int nResult = false;
    RL_LOAD_OPTIMIZATION nLoadOptimization = RL_MODEL_OPTIMIZATION_LOAD_NONE;

    KGLOG_PROCESS_ERROR(g_pRL->m_pSO3World);

    if (g_pRL->m_pSO3World->m_dwClientPlayerID == pPlayer->m_dwID && !g_pRL->m_OptimizationOption.bSelfUniform)
    {
        nLoadOptimization = RL_MODEL_OPTIMIZATION_LOAD_NORMAL;
        goto Exit1;
    }

    if (g_pRL->m_CurrentOptimizationOption.bPlayerUniqueUniform)
    {
        nLoadOptimization = RL_MODEL_OPTIMIZATION_LOAD_UNIQUE_UNIFORM;
        goto Exit1;
    }
    
    if (g_pRL->m_CurrentOptimizationOption.bPlayerGenderUniform)
    {
        nLoadOptimization = RL_MODEL_OPTIMIZATION_LOAD_GENDER_UNIFORM;
        goto Exit1;
    }

    if (g_pRL->m_CurrentOptimizationOption.bPlayerRoleTypeUniform)
    {
        nLoadOptimization = RL_MODEL_OPTIMIZATION_LOAD_ROLE_TYPE_UNIFORM;
        goto Exit1;
    }

    if (g_pRL->m_CurrentOptimizationOption.bPlayerForceUniform)
    {
        nLoadOptimization = RL_MODEL_OPTIMIZATION_LOAD_FORCE_UNIFORM;
        goto Exit1;
    }

    nLoadOptimization = RL_MODEL_OPTIMIZATION_LOAD_NORMAL;

Exit1:
    nResult = true;
Exit0:
    return nLoadOptimization;
}

static RL_LOAD_OPTIMIZATION GetRLNpcModeLoadOptimization()
{
    if (g_pRL->m_CurrentOptimizationOption.bNpcUniform)
        return RL_MODEL_OPTIMIZATION_LOAD_UNIQUE_UNIFORM;

    return RL_MODEL_OPTIMIZATION_LOAD_NORMAL;
}

static RL_LOAD_OPTIMIZATION GetRLModelLoadOptimization(KCharacter* pCharacter)
{
    RL_LOAD_OPTIMIZATION nLoadOptimization = RL_MODEL_OPTIMIZATION_LOAD_NORMAL;

    KG_ASSERT_EXIT(pCharacter);

    nLoadOptimization = GetRLModelLoadOptimizationVisible(pCharacter->m_dwID);
    KG_PROCESS_SUCCESS(nLoadOptimization == RL_MODEL_OPTIMIZATION_LOAD_NONE);

    if (IS_PLAYER(pCharacter->m_dwID))
    {
        nLoadOptimization = GetRLPlayerModeLoadOptimization((KPlayer*)pCharacter);
    }
    else
    {
        nLoadOptimization = GetRLNpcModeLoadOptimization();
    }

Exit1:
Exit0:
    return nLoadOptimization;
}

static KRLPlayerUniformModel const* GetPlayerUniformModel(RL_LOAD_OPTIMIZATION nLoadOptimization, CHARACTER_GENDER nGender, ROLE_TYPE nRoleType, DWORD dwForceID)
{
    KRLPlayerUniformModel const* pRLUniformModel = NULL;

    switch (nLoadOptimization)
    {
    case RL_MODEL_OPTIMIZATION_LOAD_NORMAL:
    case RL_MODEL_OPTIMIZATION_LOAD_NONE:
        break;
    case RL_MODEL_OPTIMIZATION_LOAD_UNIQUE_UNIFORM:
        pRLUniformModel = g_pRL->m_TableList.GetPlayerUniformModel(RL_PLAYER_UNIFORM_UNIQUE);
        break;
    case RL_MODEL_OPTIMIZATION_LOAD_GENDER_UNIFORM:
        pRLUniformModel = g_pRL->m_TableList.GetPlayerUniformModel(nGender);
        break;
    case RL_MODEL_OPTIMIZATION_LOAD_ROLE_TYPE_UNIFORM:
        pRLUniformModel = g_pRL->m_TableList.GetPlayerRoleTypeUniformModel(nRoleType);
        break;
    case RL_MODEL_OPTIMIZATION_LOAD_FORCE_UNIFORM:
        pRLUniformModel = g_pRL->m_TableList.GetPlayerForceUniformModel(dwForceID);
        break;
    default:
        ASSERT(0);
        break;
    }

    return pRLUniformModel;
}

enum RL_NPC_MODEL_SELECTABLE_OPTION
{
    RL_NPC_MODEL_SELECTABLE_OPTION_NONE,
    RL_NPC_MODEL_SELECTABLE_OPTION_SELECTABLE,
    RL_NPC_MODEL_SELECTABLE_OPTION_TABLE,
};

static int GetPlayerSocket(KRepresentPlayerEquipResource const& EquipResource, 
                           RL_LOAD_OPTIMIZATION nLoadOptimization, 
                           KRLSocketResource SocketParams[CHARACTER_SOCKET_COUNT])
{
    int nResult = false;
    size_t nSocketIndex = 0;

    KG_PROCESS_SUCCESS(nLoadOptimization == RL_MODEL_OPTIMIZATION_LOAD_NONE);

    for (int nIndex = MODEL_PLAYER_SOCKET_BEGIN; nIndex != MODEL_PLAYER_SOCKET_END; ++nIndex)
    {
        ASSERT(nSocketIndex < CHARACTER_SOCKET_COUNT);

        _tcsncpy_s(SocketParams[nSocketIndex].szMeshFilePath, EquipResource.Parts[nIndex].szMeshFilePath, _TRUNCATE);
        _tcsncpy_s(SocketParams[nSocketIndex].szMtlFilePath, EquipResource.Parts[nIndex].szMtlFilePath, _TRUNCATE);
        _tcsncpy_s(SocketParams[nSocketIndex].szSFX1FilePath, EquipResource.Parts[nIndex].szSFX1FilePath, _TRUNCATE);
        _tcsncpy_s(SocketParams[nSocketIndex].szSFX2FilePath, EquipResource.Parts[nIndex].szSFX2FilePath, _TRUNCATE);
        _tcsncpy_s(SocketParams[nSocketIndex].szSocketName, EquipResource.Parts[nIndex].szSocketName, _TRUNCATE);

        SocketParams[nSocketIndex].fMeshScale = EquipResource.Parts[nIndex].fMeshScale;
        SocketParams[nSocketIndex].nColorChannel = EquipResource.Parts[nIndex].nColorChannel;

        ++nSocketIndex;
    }

    // 刀鞘部分
    for (int nIndex = MODEL_PLAYER_SOCKET_WEAPON_BEGIN; nIndex != MODEL_PLAYER_SOCKET_WEAPON_END; ++nIndex)
    {
        ASSERT(nSocketIndex < CHARACTER_SOCKET_COUNT);

        _tcsncpy_s(SocketParams[nSocketIndex].szMeshFilePath, EquipResource.Parts[nIndex].szSheathMeshFilePath, _TRUNCATE);
        _tcsncpy_s(SocketParams[nSocketIndex].szMtlFilePath, EquipResource.Parts[nIndex].szSheathMtlFilePath, _TRUNCATE);
        _tcsncpy_s(SocketParams[nSocketIndex].szSFX1FilePath, _T(""), _TRUNCATE);
        _tcsncpy_s(SocketParams[nSocketIndex].szSFX2FilePath, _T(""), _TRUNCATE);
        _tcsncpy_s(SocketParams[nSocketIndex].szSocketName, EquipResource.Parts[nIndex].szSocketName, _TRUNCATE);

        SocketParams[nSocketIndex].fMeshScale = EquipResource.Parts[nIndex].fMeshScale;
        SocketParams[nSocketIndex].nColorChannel = EquipResource.Parts[nIndex].nColorChannel;

        ++nSocketIndex;
    }

Exit1:
    nResult = true;
    return nResult;
}

static int GetNpcSocket(DWORD dwModelID, RL_LOAD_OPTIMIZATION nLoadOptimization, 
                        KRLSocketResource SocketParams[CHARACTER_SOCKET_COUNT])
{
    int nResult = false;
    KRLNpcModel const* pModel = NULL;
    float fSocketScale = 1.0f;
    int nColorChannel = 0;
    size_t nSocketIndex = 0;

    KG_PROCESS_SUCCESS(nLoadOptimization == RL_MODEL_OPTIMIZATION_LOAD_NONE);

    pModel = g_pRL->m_TableList.GetNpcModel(dwModelID);
    KGLOG_PROCESS_ERROR(pModel);

    fSocketScale = pModel->fSocketScale;
    nColorChannel = pModel->nColorChannel;

    for (int nIndex = 0; nIndex != RL_NPC_SOCKET_COUNT; ++nIndex)
    {
        ASSERT(nSocketIndex < CHARACTER_SOCKET_COUNT);

        _tcsncpy_s(SocketParams[nSocketIndex].szMeshFilePath, pModel->Sockets[nIndex].szMeshFilePath, _TRUNCATE);
        _tcsncpy_s(SocketParams[nSocketIndex].szMtlFilePath, pModel->Sockets[nIndex].szMtlFilePath, _TRUNCATE);
        _tcsncpy_s(SocketParams[nSocketIndex].szSFX1FilePath, _T(""), _TRUNCATE);
        _tcsncpy_s(SocketParams[nSocketIndex].szSFX2FilePath, _T(""), _TRUNCATE);
        _tcsncpy_s(SocketParams[nSocketIndex].szSocketName, GetNpcSocketName(nIndex), _TRUNCATE);

        SocketParams[nSocketIndex].fMeshScale = nIndex == RL_NPC_SOCKET_FACE ? 1.0f : fSocketScale;
        SocketParams[nSocketIndex].nColorChannel = nColorChannel;

        ++nSocketIndex;
    }

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

static DWORD GetNpcModelID(DWORD dwModelID, RL_LOAD_OPTIMIZATION nLoadOptimization)
{
    RL_NPC_UNIFORM nNpcUniform = RL_NPC_UNIFORM_NONE;
    KRLNpcUniformModel const* pRLNpcUniformModel = NULL;

    switch (nLoadOptimization)
    {
    case RL_MODEL_OPTIMIZATION_LOAD_NONE:
        nNpcUniform = RL_NPC_UNIFORM_TRANSPARENCY;
        break;
    case RL_MODEL_OPTIMIZATION_LOAD_UNIQUE_UNIFORM:
        nNpcUniform = RL_NPC_UNIFORM_UNIQUE;
        break;
    }

    if (nNpcUniform != RL_NPC_UNIFORM_NONE)
    {
        pRLNpcUniformModel = g_pRL->m_TableList.GetNpcUniformModel(nNpcUniform);
        KGLOG_PROCESS_ERROR(pRLNpcUniformModel);

        dwModelID = pRLNpcUniformModel->dwRepresentID;
    }

Exit0:
    return dwModelID;
}

static int GetNpcModel(DWORD dwModelID, RL_LOAD_OPTIMIZATION nLoadOptimization, 
                       RL_NPC_MODEL_SELECTABLE_OPTION nSelectableOption, KRLCharacterEquipResource* pResource)
{
    int nRetCode = false;
    int nResult = false;
    KRLNpcModel const* pRLNpcModel = NULL;

    pRLNpcModel = g_pRL->m_TableList.GetNpcModel(dwModelID); 
    KGLOG_PROCESS_ERROR(pRLNpcModel);

    // Mdl
    pResource->MDL.szMDLFilePath = pRLNpcModel->szMainModelFile;
    pResource->MDL.fMDLScale = pRLNpcModel->fModelScale;

    pResource->bSplitModel = pRLNpcModel->bSplitModel;

    switch (nSelectableOption)
    {
    case RL_NPC_MODEL_SELECTABLE_OPTION_NONE:
        pResource->MDL.bSelectable = FALSE;
        break;
    case RL_NPC_MODEL_SELECTABLE_OPTION_SELECTABLE:
        pResource->MDL.bSelectable = TRUE;
        break;
    case RL_NPC_MODEL_SELECTABLE_OPTION_TABLE:
        pResource->MDL.bSelectable = pRLNpcModel->bSelectable;
        break;
    default:
        ASSERT(0);
        break;
    }

    // Socket
    nRetCode = GetNpcSocket(dwModelID, nLoadOptimization, pResource->SocketParams);
    KGLOG_PROCESS_ERROR(nRetCode);

    // Color channel
    pResource->nColorChannelTable = pRLNpcModel->nColorChannelTable;
    pResource->nColorChannel = pRLNpcModel->nColorChannel;

    // Skill effect
    pResource->dwEffectType = pRLNpcModel->dwEffectType;
    pResource->fEffectScale = pRLNpcModel->fEffectScale;

    pResource->nTerrainSlope = (RL_TERRAIN_SLOPE)pRLNpcModel->nTerrainSlope;
    pResource->fSelectionScale = pRLNpcModel->fSelectionScale;

    pResource->nObject = RL_OBJECT_NPC;

    nResult = true;
Exit0:
    return nResult;
}

static int GetPlayerRepresentIDs(KPlayer* pPlayer, DWORD dwRepresentIDs[perRepresentCount])
{
    int nResult = false;
    IKGSO3WorldClient* pSO3WorldClient = NULL;

	ASSERT(pPlayer);
	
    pSO3WorldClient = g_pRL->m_pSO3WorldClient;
    KGLOG_PROCESS_ERROR(pSO3WorldClient);

    for (int nIndex = 0; nIndex != perRepresentCount; ++nIndex)
    {
        dwRepresentIDs[nIndex] = pSO3WorldClient->GetPlayerEquipRepresent(pPlayer, nIndex);
    }

    nResult = true;
Exit0:
    return nResult;
}

static int GetWeaponParam(ROLE_TYPE nRoleType, DWORD dwWeaponRepresentID, RL_WEAPON* pnWeapon, RL_SHEATH* pnSheath)
{
    int nResult = false;
    KPlayerWeaponModel const* pWeaponModel = NULL;

    KGLOG_PROCESS_ERROR(pnWeapon);
    KGLOG_PROCESS_ERROR(pnSheath);

    if (nRoleType != rtInvalid)
    {
        pWeaponModel = g_pRL->m_TableList.GetPlayerWeaponModel(dwWeaponRepresentID, nRoleType);
        KG_PROCESS_ERROR(pWeaponModel);

        *pnWeapon = (RL_WEAPON)pWeaponModel->nWeaponType;
        *pnSheath = (RL_SHEATH)pWeaponModel->nSheathSocket;
    }
    else
    {
        *pnWeapon = RL_WEAPON_NONE;
        *pnSheath = RL_SHEATH_NONE;
    }

    nResult = true;
Exit0:
    return nResult;
}

static int GetPlayerModel(KPlayer* pPlayer, 
                          RL_LOAD_OPTIMIZATION nLoadOptimization, 
                          RL_NPC_MODEL_SELECTABLE_OPTION nSelectableOption, 
                          KRLCharacterEquipResource* pResource)
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;
    KRLPlayerModel const* pRLPlayerModel = NULL;
    KRLCharacterModelResource* pRLModelResource = NULL;
    KPlayerChestEquipModel const* pChestEquipModel = NULL;
    KRepresentPlayerEquipResource EquipResource;
    KRLPlayerUniformModel const* pRLUniformModel = NULL;
    ROLE_TYPE nRoleType = rtInvalid;
    CHARACTER_GENDER nGender = cgMale;
    DWORD dwForceID = 0;

    if (nLoadOptimization != RL_MODEL_OPTIMIZATION_LOAD_UNIQUE_UNIFORM)
    {
        nRoleType = pPlayer->m_eRoleType;
        nGender = pPlayer->m_eGender;
        dwForceID = pPlayer->m_dwForceID;
    }

    pRLUniformModel = GetPlayerUniformModel(nLoadOptimization, nGender, nRoleType, dwForceID);
    if (pRLUniformModel)
    {
        pResource->Player.dwSkillID = pRLUniformModel->dwSkillID;
        pResource->Player.dwSkillLevel = pRLUniformModel->dwSkillLevel;

        memcpy(pResource->Player.dwRepresentIDs, pRLUniformModel->dwRepresentIDs, sizeof(pResource->Player.dwRepresentIDs));
    }
    else
    {
        nRetCode = GetPlayerRepresentIDs(pPlayer, pResource->Player.dwRepresentIDs);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nRetCode = GetWeaponParam(nRoleType, pResource->Player.dwRepresentIDs[perWeaponStyle], 
        &pResource->Player.nWeaponType, &pResource->Player.nSheathSocket);
    KGLOG_PROCESS_ERROR(nRetCode);

    // Mdl
    pRLPlayerModel = g_pRL->m_TableList.GetPlayerModel(nRoleType);
    KGLOG_PROCESS_ERROR(pRLPlayerModel);

    pResource->MDL.szMDLFilePath = pRLPlayerModel->szMDLFilePath;
    pResource->MDL.fMDLScale = pRLPlayerModel->fModelScale;

    pResource->bSplitModel = TRUE;

    switch (nSelectableOption)
    {
    case RL_NPC_MODEL_SELECTABLE_OPTION_NONE:
        pResource->MDL.bSelectable = FALSE;
        break;
    case RL_NPC_MODEL_SELECTABLE_OPTION_SELECTABLE:
        pResource->MDL.bSelectable = TRUE;
        break;
    case RL_NPC_MODEL_SELECTABLE_OPTION_TABLE:
        ASSERT(0);
        break;
    default:
        ASSERT(0);
        break;
    }

    // Color channel
    pResource->nColorChannelTable = nRoleType; // 体型偏色表ID跟逻辑体型ID一致
    pResource->nColorChannel = 0;

    // Models
    hr = GetPlayerEquipModelResource(pResource->Player.dwRepresentIDs, nRoleType, &EquipResource);
    KGLOG_COM_PROCESS_ERROR(hr);

    pRLModelResource = pResource->Player.Models;

    ASSERT(_countof(pResource->Player.Models) == MODEL_PLAYER_EQUIP_END - MODEL_PLAYER_EQUIP_BEGIN);

    for (int nIndex = 0; nIndex != _countof(pResource->Player.Models); ++nIndex)
    {
        pRLModelResource->szMeshFilePath = EquipResource.Parts[nIndex].szMeshFilePath;
        pRLModelResource->szMtlFilePath = EquipResource.Parts[nIndex].szMtlFilePath;
        pRLModelResource->szSFX1FilePath = EquipResource.Parts[nIndex].szSFX1FilePath;
        pRLModelResource->szSFX2FilePath = EquipResource.Parts[nIndex].szSFX2FilePath;
        pRLModelResource->fMeshScale = EquipResource.Parts[nIndex].fMeshScale;
        pRLModelResource->nColorChannel = EquipResource.Parts[nIndex].nColorChannel;

        ++pRLModelResource;
    }

    // Skill effect
    pChestEquipModel = g_pRL->m_TableList.GetPlayerChestEquipModel(pResource->Player.dwRepresentIDs[perChestStyle], nRoleType);
    if (pChestEquipModel)
    {
        pResource->dwEffectType = pChestEquipModel->dwEffectType;
        pResource->fEffectScale = pChestEquipModel->fScale;
    }
    else
    {
        pResource->dwEffectType = 0;
        pResource->fEffectScale = 1.0f;
    }

    // Socket
    nRetCode = GetPlayerSocket(EquipResource, nLoadOptimization, pResource->SocketParams);
    KGLOG_PROCESS_ERROR(nRetCode);

    pResource->Player.nRoleType = nRoleType;

    pResource->nTerrainSlope = RL_TERRAIN_SLOPE_NONE;

    pResource->fSelectionScale = 1.0f;

    pResource->nObject = RL_OBJECT_PLAYER;

    nResult = true;
Exit0:
    return nResult;
}

int GetCharacterModel(KRLResourceLimit* pOptimization, KCharacter* pCharacter, KRLCharacterEquipResource* pResource)
{
    int nRetCode = false;
    int nResult = false;
    RL_LOAD_OPTIMIZATION nLoadOptimization = RL_MODEL_OPTIMIZATION_LOAD_NORMAL;
    RL_NPC_MODEL_SELECTABLE_OPTION nSelectableOption = RL_NPC_MODEL_SELECTABLE_OPTION_SELECTABLE;
    DWORD dwModelID = 0;
    BOOL bPlayer = FALSE;
    BOOL bLocalPlayer = FALSE;

    KGLOG_PROCESS_ERROR(pResource);

    if (pCharacter)
    {
        bPlayer = IS_PLAYER(pCharacter->m_dwID);

        if (bPlayer)
            bLocalPlayer = pCharacter->m_dwID == g_pRL->m_pSO3World->m_dwClientPlayerID;

        nLoadOptimization = GetRLModelLoadOptimization(pCharacter);

        if (bPlayer)
        {
            KPlayer* pPlayer = (KPlayer*)pCharacter;

            if (bLocalPlayer)
                nSelectableOption = RL_NPC_MODEL_SELECTABLE_OPTION_NONE;
            else
                nSelectableOption = RL_NPC_MODEL_SELECTABLE_OPTION_SELECTABLE;

            if (pCharacter->m_eMoveState != cmsOnAutoFly && pPlayer->m_dwModelID != 0 ||
                nLoadOptimization == RL_MODEL_OPTIMIZATION_LOAD_NONE)
            {
                dwModelID = GetNpcModelID(pPlayer->m_dwModelID, nLoadOptimization);

                nRetCode = GetNpcModel(dwModelID, nLoadOptimization, nSelectableOption, pResource);
                KGLOG_PROCESS_ERROR(nRetCode);

                nRetCode = GetPlayerRepresentIDs(pPlayer, pResource->Player.dwRepresentIDs);
                KGLOG_PROCESS_ERROR(nRetCode);

                pResource->Player.nRoleType = rtInvalid;
                pResource->Player.nWeaponType = RL_WEAPON_NONE;
                pResource->Player.nSheathSocket = RL_SHEATH_NONE;
                pResource->Player.dwSkillID = 0;
                pResource->Player.dwSkillLevel = 0;

                for (int nIndex = 0; nIndex != _countof(pResource->Player.Models); ++nIndex)
                {
                    KRLCharacterModelResource* pRLModelResource = pResource->Player.Models + nIndex;

                    pRLModelResource->szMeshFilePath = _T("");
                    pRLModelResource->szMtlFilePath = _T("");
                    pRLModelResource->szSFX1FilePath = _T("");
                    pRLModelResource->szSFX2FilePath = _T("");
                    pRLModelResource->fMeshScale = 1.0f;
                    pRLModelResource->nColorChannel = 0;
                }
            }
            else
            {
                nRetCode = GetPlayerModel(pPlayer, nLoadOptimization, nSelectableOption, pResource);
                KGLOG_PROCESS_ERROR(nRetCode);
            }

            pResource->fModelHeight = (float)pCharacter->m_nHeight * ((float)LOGICAL_CELL_CM_LENGTH / CELL_LENGTH / 8.0f);
        }
        else
        {
            nSelectableOption = RL_NPC_MODEL_SELECTABLE_OPTION_TABLE;

            dwModelID = GetNpcModelID(pCharacter->m_dwModelID, nLoadOptimization);

            nRetCode = GetNpcModel(dwModelID, nLoadOptimization, nSelectableOption, pResource);
            KGLOG_PROCESS_ERROR(nRetCode);

            pResource->fModelHeight = g_pRL->m_TableList.GetNpcModelHeight(pCharacter->m_dwModelID);
        }
    }
    else
    {
        nSelectableOption = RL_NPC_MODEL_SELECTABLE_OPTION_NONE;
        nLoadOptimization = RL_MODEL_OPTIMIZATION_LOAD_NORMAL;

        dwModelID = 0;

        nRetCode = GetNpcModel(dwModelID, nLoadOptimization, nSelectableOption, pResource);
        KGLOG_PROCESS_ERROR(nRetCode);

        pResource->fModelHeight = g_pRL->m_TableList.GetNpcModelHeight(0);
    }

    pResource->dwModelID = dwModelID;
    pResource->nLoadOptimization = nLoadOptimization;

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (pResource)
        {
            memset(pResource, 0, sizeof(KRLCharacterEquipResource));
        }
    }
    return nResult;
}

