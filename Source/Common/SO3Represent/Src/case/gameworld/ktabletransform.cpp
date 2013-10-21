#include "stdafx.h"
#include "SO3World/IKGSO3WorldClient.h"
#include "SO3World/KSO3World.h"
#include "../../Source/Common/SO3World/Src/KPlayer.h"
#include "../../case/render/kanimationmodel.h"
#include "../../SO3Represent.h"
#include "../actionobject/krlcharacter.h"
#include "../actionobject/krllocalcharacter.h"
#include "../actionobject/krlremotecharacter.h"
#include "./ktabletransform.h"
#include "./krlanimationtable.h"

static int GetForceRelation(DWORD dwSrcCharacterID, DWORD dwDstCharacterID)
{
    KCharacter* pSrcCharacter = NULL;
    KCharacter* pDstCharacter = NULL;
    int nRelation = sortInvalid;

    if (IS_PLAYER(dwSrcCharacterID))
    {
        pSrcCharacter = (KCharacter*)GetPlayer(dwSrcCharacterID);
        KG_PROCESS_ERROR(pSrcCharacter);
    }
    else
    {
        pSrcCharacter = (KCharacter*)GetNpc(dwSrcCharacterID);
        KG_PROCESS_ERROR(pSrcCharacter);
    }

    if (IS_PLAYER(dwDstCharacterID))
    {
        pDstCharacter = (KCharacter*)GetPlayer(dwDstCharacterID);
        KG_PROCESS_ERROR(pDstCharacter);
    }
    else
    {
        pDstCharacter = (KCharacter*)GetNpc(dwDstCharacterID);
        KG_PROCESS_ERROR(pDstCharacter);
    }
    
	KGLOG_PROCESS_ERROR(g_pRL->m_pSO3WorldClient);
	
    nRelation = g_pRL->m_pSO3WorldClient->GetRelation(pSrcCharacter, pDstCharacter);
Exit0:
    return nRelation;
}

RL_FORCE_RELATION GetRLForceRelation(DWORD dwSrcCharacterID, DWORD dwDstCharacterID)
{
    int nGWForceRelation = 0;
    RL_FORCE_RELATION nRLForceRelation = RL_FORCE_RELATION_INVALID;
    
    if (IS_PLAYER(dwSrcCharacterID) && IS_PLAYER(dwDstCharacterID))
    {
        // 玩家之间的关系，取的时候要反过来
        nGWForceRelation = GetForceRelation(dwDstCharacterID, dwSrcCharacterID);
    }
    else
    {
        nGWForceRelation = GetForceRelation(dwSrcCharacterID, dwDstCharacterID);
    }

    nRLForceRelation = ForceRelationGWToRL(nGWForceRelation);

    return nRLForceRelation;
}

int GetRidesEquipModel(DWORD dwRidesID, KRLCharacterEquipResource const& EquipResource, KRLRidesEquipResource* pEquip)
{
    int nRetCode = false;
    int nResult = false;
    DWORD dwRepresentID = 0;
    KRidesModel const* pRidesModel = NULL;
    KRidesEquipModel const* pModel = NULL;
    KRLRidesEquipResource* pBodyPart = NULL;
    KRLRidesEquipResource* pManePart = NULL;

    KGLOG_PROCESS_ERROR(pEquip);

    pRidesModel = g_pRL->m_TableList.GetRidesModel(dwRidesID);
    KGLOG_PROCESS_ERROR(pRidesModel);

    // 马身和鬃毛的资源是填在rides.txt里的
    pBodyPart = pEquip + MODEL_RIDES_EQUIP_BODY;
    pManePart = pEquip + MODEL_RIDES_EQUIP_MANE;

    _tcsncpy_s(pBodyPart->szMeshFilePath, pRidesModel->szBodyMesh, _TRUNCATE);
    _tcsncpy_s(pBodyPart->szMtlFilePath, pRidesModel->szBodyMaterial, _TRUNCATE);
    pBodyPart->fMeshScale = pRidesModel->fModelScale;
    pBodyPart->nGlobalColorChannel = pRidesModel->nBodyGlobalColorChannel;
    pBodyPart->nColorChannel = pRidesModel->nBodyColorChannel;

    _tcsncpy_s(pManePart->szMeshFilePath, pRidesModel->szManeMesh, _TRUNCATE);
    _tcsncpy_s(pManePart->szMtlFilePath, pRidesModel->szManeMaterial, _TRUNCATE);
    pManePart->fMeshScale = pRidesModel->fModelScale;
    pManePart->nGlobalColorChannel = pRidesModel->nManeGlobalColorChannel;
    pManePart->nColorChannel = pRidesModel->nManeColorChannel;

    for (int nIndex = MODEL_RIDES_EQUIP_HEAD; nIndex != MODEL_RIDES_EQUIP_COUNT; ++nIndex)
    {
        dwRepresentID = EquipResource.Player.dwRepresentIDs[nIndex - MODEL_RIDES_EQUIP_HEAD + perHorseAdornment1];
        if (dwRepresentID == 0)
            continue;

        pModel = g_pRL->m_TableList.GetRidesEquipModel(nIndex, dwRepresentID);
        if (pModel)
        {
            _tcsncpy_s(pEquip[nIndex].szMeshFilePath, pModel->szMesh, _TRUNCATE);
            _tcsncpy_s(pEquip[nIndex].szMtlFilePath, pModel->szMaterial, _TRUNCATE);

            pEquip[nIndex].fMeshScale = pModel->fScale;
            pEquip[nIndex].nGlobalColorChannel = pModel->nGlobalColorChannel;
            pEquip[nIndex].nColorChannel = pModel->nColorChannel;
        }
    }

    nResult = true;
Exit0:
    return nResult;
}

int GetRidesSocketModel(KRLCharacterEquipResource const& EquipResource, KRLSocketResource* pSocket)
{
    int nRetCode = false;
    int nResult = false;
    DWORD dwRepresentID = 0;
    KRidesSocketModel const* pModel = NULL;

    KGLOG_PROCESS_ERROR(pSocket);

    for (int nIndex = 0; nIndex != MODEL_RIDES_SOCKET_COUNT; ++nIndex)
    {
        dwRepresentID = EquipResource.Player.dwRepresentIDs[nIndex + perHorseAdornment4];
        if (dwRepresentID == 0)
            continue;

        pModel = g_pRL->m_TableList.GetRidesSocketModel(nIndex, dwRepresentID);
        if (pModel)
        {
            _tcsncpy_s(pSocket[nIndex].szMeshFilePath, pModel->szMesh, _TRUNCATE);
            _tcsncpy_s(pSocket[nIndex].szMtlFilePath, pModel->szMaterial, _TRUNCATE);
            _tcsncpy_s(pSocket[nIndex].szSocketName, pModel->szSocketName, _TRUNCATE);

            pSocket[nIndex].fMeshScale = pModel->fScale;
            pSocket[nIndex].nGlobalColorChannel = pModel->nGlobalColorChannel;
            pSocket[nIndex].nColorChannel = pModel->nColorChannel;
        }
    }

    nResult = true;
Exit0:
    return nResult;
}

int GetNpcSocketByAnimation(KRLCharacterEquipResource const& EquipResource, DWORD dwAnimationID, KRLSocketResource* pSocket, BOOL& bLoadDefult)
{
    int nResult = false;
    int nRetCode = false;
    size_t nSocketIndex = 0;
    KRLNpcModel const* pModel = NULL;
    KNpcAnimationModel NpcAnimation;

    ASSERT(EquipResource.nObject == RL_OBJECT_NPC);

    KG_PROCESS_ERROR(dwAnimationID);

    KG_PROCESS_SUCCESS(EquipResource.nLoadOptimization == RL_MODEL_OPTIMIZATION_LOAD_NONE);

    bLoadDefult = FALSE;

    pModel = g_pRL->m_TableList.GetNpcModel(EquipResource.dwModelID);
    KGLOG_PROCESS_ERROR(pModel);

    nRetCode = g_pRL->m_TableList.GetNpcAnimationModel(EquipResource.dwModelID, dwAnimationID, &NpcAnimation);
    KG_PROCESS_ERROR(nRetCode);

    bLoadDefult = NpcAnimation.bLoadDefaultSocket;

    for (int nIndex = 0; nIndex != RL_NPC_SOCKET_COUNT; ++nIndex)
    {
        KGLOG_PROCESS_ERROR(nSocketIndex < CHARACTER_SOCKET_COUNT);

        _tcsncpy_s(pSocket[nSocketIndex].szMeshFilePath, NpcAnimation.SocketInfo[nIndex].szMeshFilePath, _TRUNCATE);
        _tcsncpy_s(pSocket[nSocketIndex].szMtlFilePath, NpcAnimation.SocketInfo[nIndex].szMaterialFile, _TRUNCATE);
        _tcsncpy_s(pSocket[nSocketIndex].szSFX1FilePath, _T(""), _TRUNCATE);
        _tcsncpy_s(pSocket[nSocketIndex].szSFX2FilePath, _T(""), _TRUNCATE);
        _tcsncpy_s(pSocket[nSocketIndex].szSocketName, GetNpcSocketName(nIndex), _TRUNCATE);

        pSocket[nSocketIndex].fMeshScale = nIndex == RL_NPC_SOCKET_FACE ? 1.0f : pModel->fSocketScale;

        ++nSocketIndex;
    }

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

KSkillCasterModel const* GetCharacterSkillCasterModel(KCaster const& Caster, DWORD dwSkillID, DWORD dwSkillLevel)
{
    KSkillCasterModel const* pModel = NULL;

    switch (Caster.eTargetType)
    {
    case ttNpc:
        pModel = g_pRL->m_TableList.GetNpcSkillCasterModel(dwSkillID, dwSkillLevel, Caster.dwRepresentID);
        break;
    case ttPlayer:
        pModel = g_pRL->m_TableList.GetPlayerSkillCasterModel(dwSkillID, dwSkillLevel, Caster.nRoleType);
        break;
    default:
        ASSERT(0);
        break;
    };
    return pModel;
}


RL_FORCE_RELATION ForceRelationGWToRL(int nType)
{
    static int s_aGameWorldType[RL_FORCE_RELATION_COUNT] =
    {
        sortInvalid,
        sortEnemy,
        sortNeutrality,
        sortParty,
        sortAlly,
        sortSelf,
        sortNone,
        sortAll,
    };

    for (int nIndex = 0; nIndex < RL_FORCE_RELATION_COUNT; ++nIndex)
    {
        if (s_aGameWorldType[nIndex] & nType)
        {
            return (RL_FORCE_RELATION)nIndex;
        }
    }
    return RL_FORCE_RELATION_INVALID;
}

KForceRelationModel const* GetForceRelationModel(DWORD dwSrcCharacterID, DWORD dwDstCharacterID)
{
    KForceRelationModel const* pForceRelationModel = NULL;
    RL_FORCE_RELATION nRLForceRelation = RL_FORCE_RELATION_INVALID;
    
    nRLForceRelation = GetRLForceRelation(dwSrcCharacterID, dwDstCharacterID);

    pForceRelationModel = g_pRL->m_TableList.GetForceRelationModel(nRLForceRelation);
    KGLOG_PROCESS_ERROR(pForceRelationModel);

Exit0:
    return pForceRelationModel;
}

int GetCharacterSkillCasterParam(KRLCharacterEquipResource const& EquipResource, DWORD dwSkillID, DWORD dwSkillLevel, KSkillCasterParam* pSkillCasterParam)
{
    int nResult = false;
    KSkillCasterModel const* pSkillCasterModel = NULL;

    KG_ASSERT_EXIT(pSkillCasterParam);

    if (EquipResource.nObject == RL_OBJECT_PLAYER)
    {
        if (g_pRL->m_CurrentOptimizationOption.bSkillAnimationUniform)
        {
            dwSkillID = EquipResource.Player.dwSkillID;
            dwSkillLevel = EquipResource.Player.dwSkillLevel;
        }

        pSkillCasterModel = g_pRL->m_TableList.GetPlayerSkillCasterModel(dwSkillID, dwSkillLevel, EquipResource.Player.nRoleType);
        KG_PROCESS_ERROR(pSkillCasterModel);
    }
    else
    {
        ASSERT(EquipResource.nObject == RL_OBJECT_NPC || EquipResource.nObject == RL_OBJECT_UNKNOWN);

        pSkillCasterModel = g_pRL->m_TableList.GetNpcSkillCasterModel(dwSkillID, dwSkillLevel, EquipResource.dwModelID);
        KG_PROCESS_ERROR(pSkillCasterModel);
    }

    pSkillCasterParam->dwLockControlTime = pSkillCasterModel->dwLockControlTime;
    pSkillCasterParam->dwPrepareAnimationID = pSkillCasterModel->dwPrepareCastSkillAnimationID;

    memcpy(pSkillCasterParam->dwCastSkillAnimationIDs, pSkillCasterModel->dwCastSkillAnimationIDs, sizeof(pSkillCasterParam->dwCastSkillAnimationIDs));

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (pSkillCasterParam)
        {
            memset(pSkillCasterParam, 0, sizeof(KSkillCasterParam));
        }
    }
    return nResult;
}

int GetCharacterSkillBuff(KRLCharacterEquipResource const& EquipResource, DWORD dwSkillBuffs[SKILL_BUFF_PART_COUNT], KSkillBuffParam* pSkillBuff)
{
    int nResult = false;
    KSkillBuffModel const* pSkillBuffModel = NULL;

    if (EquipResource.nObject == RL_OBJECT_PLAYER)
    {
        for (int nIndex = 0; nIndex != SKILL_BUFF_PART_COUNT; ++nIndex)
        {
            pSkillBuffModel = g_pRL->m_TableList.GetSkillBuffModel(nIndex, dwSkillBuffs[nIndex]);
            KG_PROCESS_ERROR(pSkillBuffModel);

            pSkillBuff[nIndex].szBoneNameLeft = pSkillBuffModel->szBoneNameLeft;
            pSkillBuff[nIndex].szBoneNameRight = pSkillBuffModel->szBoneNameRight;
            pSkillBuff[nIndex].SFXParam.dwPlayType = AnimationPlayTypeRLTo3D((RL_ANIMATION_PLAY_TYPE)pSkillBuffModel->dwPlayType);
            pSkillBuff[nIndex].SFXParam.fSpeed = 1.0f;
            pSkillBuff[nIndex].SFXParam.szFilePath = pSkillBuffModel->szSFXFilePath;
            pSkillBuff[nIndex].SFXParam.szBoneName = _T("");

            switch (EquipResource.Player.nRoleType)
            {
            case rtInvalid:
                pSkillBuff[nIndex].SFXParam.fScale = 1.0f;
                break;
            case rtStandardMale:
                pSkillBuff[nIndex].SFXParam.fScale = pSkillBuffModel->fScale_m2;
                break;
            case rtStandardFemale:
                pSkillBuff[nIndex].SFXParam.fScale = pSkillBuffModel->fScale_f2;
                break;
            case rtStrongMale:
                pSkillBuff[nIndex].SFXParam.fScale = pSkillBuffModel->fScale_m3;
                break;
            case rtSexyFemale:
                pSkillBuff[nIndex].SFXParam.fScale = pSkillBuffModel->fScale_f3;
                break;
            case rtLittleBoy:
                pSkillBuff[nIndex].SFXParam.fScale = pSkillBuffModel->fScale_m1;
                break;
            case rtLittleGirl:
                pSkillBuff[nIndex].SFXParam.fScale = pSkillBuffModel->fScale_f1;
                break;
            default:
                ASSERT(0);
                pSkillBuff[nIndex].SFXParam.fScale = 1.0f;
                break;
            }
        }
    }
    else
    {
        KRLNpcModel const* pNpcModel = NULL;

        ASSERT(EquipResource.nObject == RL_OBJECT_NPC || EquipResource.nObject == RL_OBJECT_UNKNOWN);

        pNpcModel = g_pRL->m_TableList.GetNpcModel(EquipResource.dwModelID);
        KGLOG_PROCESS_ERROR(pNpcModel);

        for (int nIndex = 0; nIndex != SKILL_BUFF_PART_COUNT; ++nIndex)
        {
            pSkillBuffModel = g_pRL->m_TableList.GetSkillBuffModel(nIndex, dwSkillBuffs[nIndex]);
            KG_PROCESS_ERROR(pSkillBuffModel);

            pSkillBuff[nIndex].szBoneNameLeft = pSkillBuffModel->szBoneNameLeft;
            pSkillBuff[nIndex].szBoneNameRight = pSkillBuffModel->szBoneNameRight;
            pSkillBuff[nIndex].SFXParam.dwPlayType = AnimationPlayTypeRLTo3D((RL_ANIMATION_PLAY_TYPE)pSkillBuffModel->dwPlayType);
            pSkillBuff[nIndex].SFXParam.fScale = pNpcModel->fEffectScale;
            pSkillBuff[nIndex].SFXParam.fSpeed = 1.0f;
            pSkillBuff[nIndex].SFXParam.szFilePath = pSkillBuffModel->szSFXFilePath;
            pSkillBuff[nIndex].SFXParam.szBoneName = _T("");
        }
    }

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (pSkillBuff)
        {
            for (int nIndex = 0; nIndex != SKILL_BUFF_PART_COUNT; ++nIndex)
            {
                pSkillBuff[nIndex].szBoneNameLeft = _T("");
                pSkillBuff[nIndex].szBoneNameRight = _T("");
                pSkillBuff[nIndex].SFXParam.dwPlayType = ENU_ANIMATIONPLAY_ONCE;
                pSkillBuff[nIndex].SFXParam.fScale = 1.0f;
                pSkillBuff[nIndex].SFXParam.fSpeed = 1.0f;
                pSkillBuff[nIndex].SFXParam.szFilePath = _T("");
                pSkillBuff[nIndex].SFXParam.szBoneName = _T("");
            }
        }
    }
    return nResult;
}

int GetCharacterMissileID(DWORD dwSkillID, DWORD dwSkillLevel, KCaster const* pCaster, DWORD& dwMissileID)
{
    int nResult = false;

    if (pCaster->eTargetType == ttPlayer)
    {
        KSkillCasterModel const* pModel = NULL;

        pModel = g_pRL->m_TableList.GetPlayerSkillCasterModel(dwSkillID, dwSkillLevel, pCaster->nRoleType);
        KG_PROCESS_ERROR(pModel);

        dwMissileID = pModel->dwMissileID;
    }
    else
    {
        KSkillCasterModel const* pModel = NULL;

        pModel = g_pRL->m_TableList.GetNpcSkillCasterModel(dwSkillID, dwSkillLevel, pCaster->dwRepresentID);
        KG_PROCESS_ERROR(pModel);

        dwMissileID = pModel->dwMissileID;
    }

    nResult = true;
Exit0:
    return nResult;
}

int GetCharacterSkillCaster(KRLCharacter* pRLCharacter, KCaster* pCaster)
{
    int nRetCode = false;
    int nResult = false;
    KCharacter* pCharacter = NULL;

    KG_ASSERT_EXIT(pRLCharacter);
    KG_ASSERT_EXIT(pCaster);

    if (pRLCharacter->m_bCurrentPlayer)
    {
        KRLLocalCharacter* pRLLocalCharacter = NULL;
        KPlayer* pPlayer = NULL;
        
        pRLLocalCharacter = CONTAINING_RECORD(pRLCharacter, KRLLocalCharacter, m_RLCharacter);

        pCharacter = pRLLocalCharacter->m_FrameData.m_pCharacter;
        KGLOG_PROCESS_ERROR(pCharacter);

        pPlayer = (KPlayer*)pCharacter;

        pCaster->eTargetType = ttPlayer;
        pCaster->dwCharacterID = pPlayer->m_dwID;
        pCaster->nRoleType = pPlayer->m_eRoleType;
    }
    else
    {
        KRLRemoteCharacter* pRLRemoteCharacter = NULL;
        
        pRLRemoteCharacter = CONTAINING_RECORD(pRLCharacter, KRLRemoteCharacter, m_RLCharacter);

        pCharacter = pRLRemoteCharacter->m_FrameData.m_pCharacter;
        KGLOG_PROCESS_ERROR(pCharacter);

        pCaster->eTargetType = ttNpc;
        pCaster->dwCharacterID = pCharacter->m_dwID;
        pCaster->dwRepresentID = pCharacter->m_dwModelID;
    }

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (pCaster)
        {
            pCaster->eTargetType = ttNoTarget;
            pCaster->dwCharacterID = 0;
            pCaster->dwRepresentID = 0;
        }
    }
    return nResult;
}

int GetCharacterLevelUpSFX(KRLCharacterEquipResource const& EquipResource, REPRESENT_SFX nSFX, KRLSFXParam* pSFXParam)
{
    int nResult = false;
    KRLPlayerModel const* pPlayerModel = NULL;

    KGLOG_PROCESS_ERROR(pSFXParam);

    KGLOG_PROCESS_ERROR(EquipResource.nObject == RL_OBJECT_PLAYER);

    pPlayerModel = g_pRL->m_TableList.GetPlayerModel(EquipResource.Player.nRoleType);
    KGLOG_PROCESS_ERROR(pPlayerModel);        

    pSFXParam->dwPlayType =  ENU_ANIMATIONPLAY_ONCE;
    pSFXParam->fScale = pPlayerModel->SFXs[nSFX].fLevelUpScale;
    pSFXParam->fSpeed = 1.0f;
    pSFXParam->szFilePath = pPlayerModel->SFXs[nSFX].szLevelUpFilePath;
    pSFXParam->szBoneName = _T("");

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (pSFXParam)
        {
            pSFXParam->dwPlayType =  ENU_ANIMATIONPLAY_ONCE;
            pSFXParam->fScale = 1.0f;
            pSFXParam->fSpeed = 1.0f;
            pSFXParam->szFilePath = _T("");
            pSFXParam->szBoneName = _T("");
        }
    }
    return nResult;
}

int GetCharacterQuestMarkOrSkillBuffHeight(KCharacter* pCharacter, float& fHeight)
{
    int nRetCode = false;
    int nResult = false;

	KRL_CHARACTER_TYPE eCharacterType = CHARACTER_TYPE_BEGIN;

    KGLOG_PROCESS_ERROR(pCharacter);
	KGLOG_PROCESS_ERROR(g_pRL->m_pSO3World);

    fHeight = g_pRL->m_TableList.GetNumber(RL_NUMBER_CAPTION_MARK_SPACE_HEIGHT);

	if (IS_PLAYER(pCharacter->m_dwID))
	{
		if (g_pRL->m_pSO3World->m_dwClientPlayerID == pCharacter->m_dwID)
			eCharacterType = CHARACTER_TYPE_LOCAL_PLAYER;
		else
			eCharacterType = CHARACTER_TYPE_REMOTE_PLAYER;
	}
	else
	{
		eCharacterType = CHARACTER_TYPE_NPC;
	}

	ASSERT(RL_NUMBER_CAPTION_MARK_HEIGHT == CHARACTER_SFX_TITLE_MARK - CHARACTER_SFX_BBOX_BALLOON_BEGIN + RL_NUMBER_CAPTION_LIFE_HEIGHT);

	for (int nIndex = CHARACTER_SFX_BBOX_BALLOON_BEGIN; nIndex < CHARACTER_SFX_BBOX_BALLOON_END; nIndex++)
	{
		int nNumberIndex = nIndex - CHARACTER_SFX_BBOX_BALLOON_BEGIN + RL_NUMBER_CAPTION_LIFE_HEIGHT;
		BOOL bSFXEnabled = g_pRL->m_Option.bHeadSFXEnabled[eCharacterType][nIndex - CHARACTER_SFX_BBOX_BEGIN];
		
		switch (nIndex)
		{
		case CHARACTER_SFX_LIFE:
		case CHARACTER_SFX_TITLE_MARK:
            break;
		case CHARACTER_SFX_TONG:
			if (IS_PLAYER(pCharacter->m_dwID))
			{
				KPlayer* pPlayer = (KPlayer*)pCharacter;
				if (pPlayer->m_szTongName[0] == '\0')
				{
					bSFXEnabled = FALSE;
				}
			}
			break;
		case CHARACTER_SFX_TITLE:
			if (pCharacter->m_szTitle[0] == '\0')
			{
				bSFXEnabled = FALSE;
			}
			break;
		case CHARACTER_SFX_NAME:
			if (pCharacter->m_szName[0] == '\0')
			{
				bSFXEnabled = FALSE;
			}
			break;
		default:
			ASSERT(false);
		}
		
		if (bSFXEnabled)
		{
			fHeight += g_pRL->m_TableList.GetNumber(nNumberIndex);
		}
	}

    nResult = true;
Exit0:
    return nResult;
}

int GetCharacterAnimation(KRLCharacterEquipResource const& EquipResource, DWORD dwAnimationID, KRLAnimationParam* pAnimationParam)
{
    int nResult = false;
    int nRetCode = false;

    KGLOG_PROCESS_ERROR(pAnimationParam);

    if (EquipResource.nLoadOptimization == RL_MODEL_OPTIMIZATION_LOAD_NONE)
    {
        memset(pAnimationParam, 0, sizeof(KRLAnimationParam));

        pAnimationParam->MonoAnimationParam.szFilePath = _T("");
        pAnimationParam->fRatio = 1.0f;
    }
    else
    {
        if (EquipResource.nObject == RL_OBJECT_PLAYER)
        {
            KRLPlayerAnimationModel const* pPlayerAnimationModel = NULL;

            pPlayerAnimationModel = g_pRL->m_TableList.GetPlayerAnimationModel(dwAnimationID, EquipResource.Player.nRoleType);
            KG_PROCESS_ERROR(pPlayerAnimationModel);

            pAnimationParam->nSheathSocket = (RL_SHEATH_SOCKET)pPlayerAnimationModel->nSheathSocket;
            pAnimationParam->dwKindID = pPlayerAnimationModel->dwKindID;
            pAnimationParam->dwSplitType = static_cast<DWORD>(EquipResource.Player.nRoleType);
            pAnimationParam->fRatio = pPlayerAnimationModel->fRatio;

            pAnimationParam->SplitAnimationParam.dwPlayType = AnimationPlayTypeRLTo3D((RL_ANIMATION_PLAY_TYPE)pPlayerAnimationModel->dwAnimationType);
            pAnimationParam->SplitAnimationParam.fSpeed = pPlayerAnimationModel->fAnimationSpeed;
            pAnimationParam->SplitAnimationParam.dwTweenTime = 0;
            pAnimationParam->SplitAnimationParam.szFilePath = pPlayerAnimationModel->szAnimationFile;
        }
        else
        {
            KNpcAnimationModel NpcAnimation;

            nRetCode = g_pRL->m_TableList.GetNpcAnimationModel(EquipResource.dwModelID, dwAnimationID, &NpcAnimation);
            KG_PROCESS_ERROR(nRetCode);

            pAnimationParam->dwKindID = NpcAnimation.dwKindID;
            pAnimationParam->fRatio = 1.0f;

            pAnimationParam->MonoAnimationParam.dwPlayType = AnimationPlayTypeRLTo3D((RL_ANIMATION_PLAY_TYPE)NpcAnimation.dwAnimationType);
            pAnimationParam->MonoAnimationParam.nTrackType = (enuAnimationControllerPriority)g_pRL->m_TableList.GetNPCAnimationTrackType(NpcAnimation.dwKindID);
            pAnimationParam->MonoAnimationParam.fSpeed = NpcAnimation.fAnimationSpeed;
            pAnimationParam->MonoAnimationParam.dwTweenTime = 0;
            pAnimationParam->MonoAnimationParam.szFilePath = NpcAnimation.szAnimationFile;
        }
    }

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (pAnimationParam)
        {
            memset(pAnimationParam, 0, sizeof(KRLAnimationParam));

            pAnimationParam->MonoAnimationParam.szFilePath = _T("");
            pAnimationParam->fRatio = 1.0f;
        }
    }
    return nResult;
}

int GetCharacterAnimationFusion(KRLCharacterEquipResource const& EquipResource, DWORD dwAnimationID, DWORD dwPrevAnimationID, PDWORD pdwTweenTime)
{
    int nResult = false;

    KGLOG_PROCESS_ERROR(pdwTweenTime);

    if (g_pRL->m_CurrentOptimizationOption.bDisableAnimationBlend)
    {
        *pdwTweenTime = 0;
    }
    else
    {
        if (EquipResource.nObject == RL_OBJECT_PLAYER)
        {
            *pdwTweenTime = g_pRL->m_TableList.GetPlayerAnimationFusion(EquipResource.Player.nRoleType, dwPrevAnimationID, dwAnimationID);
        }
        else
        {
            KNpcAnimationModel const* pNpcAnimationModel = NULL;

            ASSERT(EquipResource.nObject == RL_OBJECT_NPC);

            KG_PROCESS_ERROR(EquipResource.dwModelID);

            *pdwTweenTime = g_pRL->m_TableList.GetNpcAnimationFusion(dwPrevAnimationID, dwAnimationID);
        }
    }

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (pdwTweenTime)
        {
            *pdwTweenTime = 0;
        }
    }
    return nResult;
}

BOOL GetCharacterSerialAnimation(BOOL bPlayer, DWORD dwSerialID, KRLSerialAnimation* pSerialAnimation)
{
    int nResult = false;

    if (bPlayer)
    {
        KPlayerSerialAnimation const* pPlayerSerialAnimation = NULL;

        pPlayerSerialAnimation = g_pRL->m_TableList.GetPlayerSerialAnimation(dwSerialID);
        KG_PROCESS_ERROR(pPlayerSerialAnimation);

        memcpy(pSerialAnimation->dwPhaseAnimationIDs, pPlayerSerialAnimation->dwPhaseAnimationIDs, sizeof(pSerialAnimation->dwPhaseAnimationIDs));
    }
    else
    {
        KNpcSerialAnimation const* pNpcSerialAnimation = NULL;

        pNpcSerialAnimation = g_pRL->m_TableList.GetNpcSerialAnimationModel(dwSerialID);
        KG_PROCESS_ERROR(pNpcSerialAnimation);

        memcpy(pSerialAnimation->dwPhaseAnimationIDs, pNpcSerialAnimation->dwPhaseAnimationIDs, sizeof(pSerialAnimation->dwPhaseAnimationIDs));
    }

    nResult = true;
Exit0:
    if (!nResult)
    {
        memset(pSerialAnimation, 0, sizeof(KRLSerialAnimation));
    }
    return nResult;
}

BOOL GetCharacterAdjustAnimationIndex(KRLCharacterParam const* pParam, DWORD dwAdjustID, KRLAdjustAnimationIndex* pAdjustAnimationIndex)
{
    int nResult = false;

    KG_ASSERT_EXIT(pParam);

    if (pParam->bPlayer)
    {
        KRLPlayerAdjustAnimationIndex const* pPlayerAdjustAnimationIndex = NULL;

        KG_PROCESS_ERROR(pParam->nRoleType != rtInvalid);

        pPlayerAdjustAnimationIndex = g_pRL->m_TableList.GetPlayerAdjustAnimation(dwAdjustID, pParam->nRoleType);
        KG_PROCESS_ERROR(pPlayerAdjustAnimationIndex);

        pAdjustAnimationIndex->dwJumpAnimationID = pPlayerAdjustAnimationIndex->dwJumpAnimationID;
        pAdjustAnimationIndex->dwSwimAnimationID = pPlayerAdjustAnimationIndex->dwSwimAnimationID;
        pAdjustAnimationIndex->dwRidesAnimationID = pPlayerAdjustAnimationIndex->dwRidesAnimationID;
        pAdjustAnimationIndex->dwSpeedupAnimationID = pPlayerAdjustAnimationIndex->dwSpeedupAnimationID;
        pAdjustAnimationIndex->dwWaterFlyAnimationID = pPlayerAdjustAnimationIndex->dwWaterFlyAnimationID;
    }
    else
    {
        KRLNpcAdjustAnimationIndex const* pNpcAdjustAnimationIndex = NULL;

        pNpcAdjustAnimationIndex = g_pRL->m_TableList.GetNpcAdjustAnimationModel(pParam->dwModelID, dwAdjustID);
        KG_PROCESS_ERROR(pNpcAdjustAnimationIndex);

        pAdjustAnimationIndex->dwJumpAnimationID = pNpcAdjustAnimationIndex->dwJumpAnimationID;
        pAdjustAnimationIndex->dwSwimAnimationID = pNpcAdjustAnimationIndex->dwSwimAnimationID;
        pAdjustAnimationIndex->dwRidesAnimationID = pNpcAdjustAnimationIndex->dwRidesAnimationID;
        pAdjustAnimationIndex->dwSpeedupAnimationID = pNpcAdjustAnimationIndex->dwSpeedupAnimationID;
        pAdjustAnimationIndex->dwWaterFlyAnimationID = pNpcAdjustAnimationIndex->dwWaterFlyAnimationID;
    }

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (pAdjustAnimationIndex)
        {
            memset(pAdjustAnimationIndex, 0, sizeof(KRLAdjustAnimationIndex));
        }
    }
    return nResult;
}

static RL_NPC_ANIMATION_TABLE AnimationTablePlayerToNpc(RL_ANIMATION_TABLE nAnimationTable)
{
    static RL_NPC_ANIMATION_TABLE const s_nAnimationTables[RL_ANIMATION_TABLE_COUNT] = 
    {
        RL_NPC_ANIMATION_TABLE_NONE,                   //RL_ANIMATION_TABLE_NONE,
        RL_NPC_ANIMATION_TABLE_IDLE,                   //RL_ANIMATION_TABLE_SLIP_FORWARD,
        RL_NPC_ANIMATION_TABLE_IDLE,                   //RL_ANIMATION_TABLE_SLIP_BACKWARD,
        RL_NPC_ANIMATION_TABLE_IDLE,                   //RL_ANIMATION_TABLE_SLIP_LEFT,
        RL_NPC_ANIMATION_TABLE_IDLE,                   //RL_ANIMATION_TABLE_SLIP_RIGHT,
        RL_NPC_ANIMATION_TABLE_SIT_UP,                 //RL_ANIMATION_TABLE_SIT_UP,
        RL_NPC_ANIMATION_TABLE_SIT_DOWN,               //RL_ANIMATION_TABLE_SIT_DOWN,
        RL_NPC_ANIMATION_TABLE_DEATH,                  //RL_ANIMATION_TABLE_DEATH,
        RL_NPC_ANIMATION_TABLE_RISE,                   //RL_ANIMATION_TABLE_RISE,
        RL_NPC_ANIMATION_TABLE_KNOCK_DOWN,             //RL_ANIMATION_TABLE_KNOCK_DOWN,
        RL_NPC_ANIMATION_TABLE_KNOCK_DOWN_IDLE,        //RL_ANIMATION_TABLE_KNOCK_DOWN_IDLE
        RL_NPC_ANIMATION_TABLE_KNOCK_DOWN_BE_HIT,      //RL_ANIMATION_TABLE_KNOCK_DOWN_BE_HIT
        RL_NPC_ANIMATION_TABLE_KNOCK_BACK,             //RL_ANIMATION_TABLE_KNOCK_BACK,
        RL_NPC_ANIMATION_TABLE_KNOCK_OFF,              //RL_ANIMATION_TABLE_KNOCK_OFF,
        RL_NPC_ANIMATION_TABLE_HALT,                   //RL_ANIMATION_TABLE_HALT,
        RL_NPC_ANIMATION_TABLE_FREEZE,                 //RL_ANIMATION_TABLE_FREEZE,
        RL_NPC_ANIMATION_TABLE_ENTRAP,                 //RL_ANIMATION_TABLE_ENTRAP,
        RL_NPC_ANIMATION_TABLE_RUN_FORWARD,            //RL_ANIMATION_TABLE_PULL,
        RL_NPC_ANIMATION_TABLE_DASH_FORWARD,           //RL_ANIMATION_TABLE_DASH_FORWARD,
        RL_NPC_ANIMATION_TABLE_DASH_BACKWARD,          //RL_ANIMATION_TABLE_DASH_BACKWARD,
        RL_NPC_ANIMATION_TABLE_DASH_LEFT,              //RL_ANIMATION_TABLE_DASH_LEFT,
        RL_NPC_ANIMATION_TABLE_DASH_RIGHT,             //RL_ANIMATION_TABLE_DASH_RIGHT,
        RL_NPC_ANIMATION_TABLE_DASH_FORWARD_IDLE,      //RL_ANIMATION_TABLE_DASH_FORWARD_IDLE,
        RL_NPC_ANIMATION_TABLE_DASH_BACKWARD_IDLE,     //RL_ANIMATION_TABLE_DASH_BACKWARD_IDLE,
        RL_NPC_ANIMATION_TABLE_DASH_LEFT_IDLE,         //RL_ANIMATION_TABLE_DASH_LEFT_IDLE,
        RL_NPC_ANIMATION_TABLE_DASH_RIGHT_IDLE,        //RL_ANIMATION_TABLE_DASH_RIGHT_IDLE,
        RL_NPC_ANIMATION_TABLE_REPULSED_FORWARD,       //RL_ANIMATION_TABLE_REPULSED_FORWARD,
        RL_NPC_ANIMATION_TABLE_REPULSED_BACKWARD,      //RL_ANIMATION_TABLE_REPULSED_BACKWARD,
        RL_NPC_ANIMATION_TABLE_REPULSED_LEFT,          //RL_ANIMATION_TABLE_REPULSED_LEFT,
        RL_NPC_ANIMATION_TABLE_REPULSED_RIGHT,         //RL_ANIMATION_TABLE_REPULSED_RIGHT,

        RL_NPC_ANIMATION_TABLE_SKID_FORWARD,           //RL_ANIMATION_TABLE_SKID_FORWARD,
        RL_NPC_ANIMATION_TABLE_SKID_BACKWARD,          //RL_ANIMATION_TABLE_SKID_BACKWARD,
        RL_NPC_ANIMATION_TABLE_SKID_LEFT,              //RL_ANIMATION_TABLE_SKID_LEFT,
        RL_NPC_ANIMATION_TABLE_SKID_RIGHT,             //RL_ANIMATION_TABLE_SKID_RIGHT,

        RL_NPC_ANIMATION_TABLE_TURN_LEFT,              //RL_ANIMATION_TABLE_TURN_LEFT,
        RL_NPC_ANIMATION_TABLE_TURN_RIGHT,             //RL_ANIMATION_TABLE_TURN_RIGHT,

        //跳跃
        RL_NPC_ANIMATION_TABLE_BEGIN_JUMP,              //RL_ANIMATION_TABLE_BEGIN_JUMP,
        RL_NPC_ANIMATION_TABLE_JUMPING,                 //RL_ANIMATION_TABLE_JUMPING,
        RL_NPC_ANIMATION_TABLE_END_JUMP_IDLE,           //RL_ANIMATION_TABLE_END_JUMP_IDLE,
        RL_NPC_ANIMATION_TABLE_BEGIN_DOUBLE_JUMP,       //RL_ANIMATION_TABLE_BEGIN_DOUBLE_JUMP,
        RL_NPC_ANIMATION_TABLE_DOUBLE_JUMPING,          //RL_ANIMATION_TABLE_DOUBLE_JUMPING,
        RL_NPC_ANIMATION_TABLE_END_DOUBLE_JUMP_IDLE,    //RL_ANIMATION_TABLE_END_DOUBLE_JUMP_IDLE,

        //跳跃
        RL_NPC_ANIMATION_TABLE_BEGIN_JUMP,              //RL_ANIMATION_TABLE_BEGIN_SWIM_JUMP,
        RL_NPC_ANIMATION_TABLE_JUMPING,                 //RL_ANIMATION_TABLE_SWIM_JUMPING,
        RL_NPC_ANIMATION_TABLE_END_JUMP_IDLE,           //RL_ANIMATION_TABLE_END_SWIM_JUMP_IDLE,
        RL_NPC_ANIMATION_TABLE_BEGIN_DOUBLE_JUMP,       //RL_ANIMATION_TABLE_BEGIN_SWIM_DOUBLE_JUMP,
        RL_NPC_ANIMATION_TABLE_DOUBLE_JUMPING,          //RL_ANIMATION_TABLE_SWIM_DOUBLE_JUMPING,
        RL_NPC_ANIMATION_TABLE_END_DOUBLE_JUMP_IDLE,    //RL_ANIMATION_TABLE_END_SWIM_DOUBLE_JUMP_IDLE,

        //插拔武器
        RL_NPC_ANIMATION_TABLE_IDLE,                   //RL_ANIMATION_TABLE_BEGIN_SHEATH_BACK,
        RL_NPC_ANIMATION_TABLE_IDLE,                   //RL_ANIMATION_TABLE_BEGIN_SHEATH_SIDE,
        RL_NPC_ANIMATION_TABLE_IDLE,                   //RL_ANIMATION_TABLE_BEGIN_UNSHEATH_BACK,
        RL_NPC_ANIMATION_TABLE_IDLE,                   //RL_ANIMATION_TABLE_BEGIN_UNSHEATH_SIDE,
        RL_NPC_ANIMATION_TABLE_IDLE,                   //RL_ANIMATION_TABLE_END_SHEATH_BACK,
        RL_NPC_ANIMATION_TABLE_IDLE,                   //RL_ANIMATION_TABLE_END_SHEATH_SIDE,
        RL_NPC_ANIMATION_TABLE_IDLE,                   //RL_ANIMATION_TABLE_END_UNSHEATH_BACK,
        RL_NPC_ANIMATION_TABLE_IDLE,                   //RL_ANIMATION_TABLE_END_UNSHEATH_SIDE,

        //待机
        RL_NPC_ANIMATION_TABLE_IDLE,                   //RL_ANIMATION_TABLE_IDLE,
        RL_NPC_ANIMATION_TABLE_IDLE,                   //RL_ANIMATION_TABLE_UNSHEATH_IDLE,
        RL_NPC_ANIMATION_TABLE_FIGHT_IDLE,             //RL_ANIMATION_TABLE_FIGHT_IDLE,

        //移动
        RL_NPC_ANIMATION_TABLE_SWIM_STAND,             //RL_ANIMATION_TABLE_SWIM_STAND,
        RL_NPC_ANIMATION_TABLE_SWIM_FORWARD,           //RL_ANIMATION_TABLE_SWIM_FORWARD,
        RL_NPC_ANIMATION_TABLE_SWIM_BACKWARD,          //RL_ANIMATION_TABLE_SWIM_BACKWARD,
        RL_NPC_ANIMATION_TABLE_SWIM_LEFT,              //RL_ANIMATION_TABLE_SWIM_LEFT,
        RL_NPC_ANIMATION_TABLE_SWIM_RIGHT,             //RL_ANIMATION_TABLE_SWIM_RIGHT,
        RL_NPC_ANIMATION_TABLE_WALK_FORWARD,           //RL_ANIMATION_TABLE_WALK_FORWARD,
        RL_NPC_ANIMATION_TABLE_WALK_BACKWARD,          //RL_ANIMATION_TABLE_WALK_BACKWARD,
        RL_NPC_ANIMATION_TABLE_WALK_FORWARD,           //RL_ANIMATION_TABLE_WALK_FIGHT_FORWARD,
        RL_NPC_ANIMATION_TABLE_WALK_BACKWARD,          //RL_ANIMATION_TABLE_WALK_FIGHT_BACKWARD,
        RL_NPC_ANIMATION_TABLE_RUN_FORWARD,            //RL_ANIMATION_TABLE_RUN_FORWARD,
        RL_NPC_ANIMATION_TABLE_RUN_BACKWARD,           //RL_ANIMATION_TABLE_RUN_BACKWARD,
        RL_NPC_ANIMATION_TABLE_RUN_FORWARD,            //RL_ANIMATION_TABLE_RUN_FIGHT_FORWARD,
        RL_NPC_ANIMATION_TABLE_RUN_BACKWARD,           //RL_ANIMATION_TABLE_RUN_FIGHT_BACKWARD,

        //被击
        RL_NPC_ANIMATION_TABLE_SKILL_PARRY,            //RL_ANIMATION_TABLE_PHYSICS_BLOCK,
        RL_NPC_ANIMATION_TABLE_SKILL_PARRY,            //RL_ANIMATION_TABLE_SOLAR_MAGIC_BLOCK,
        RL_NPC_ANIMATION_TABLE_SKILL_PARRY,            //RL_ANIMATION_TABLE_NEUTRAL_MAGIC_BLOCK,
        RL_NPC_ANIMATION_TABLE_SKILL_PARRY,            //RL_ANIMATION_TABLE_LUNAR_MAGIC_BLOCK,
        RL_NPC_ANIMATION_TABLE_SKILL_PARRY,            //RL_ANIMATION_TABLE_POISON_BLOCK,
        RL_NPC_ANIMATION_TABLE_BE_HIT,                 //RL_ANIMATION_TABLE_BE_HIT,
        RL_NPC_ANIMATION_TABLE_SKILL_DODGE,            //RL_ANIMATION_TABLE_DODGE,
        RL_NPC_ANIMATION_TABLE_SKILL_PARRY,            //RL_ANIMATION_TABLE_PARRY,
        RL_NPC_ANIMATION_TABLE_IDLE,                   //RL_ANIMATION_TABLE_CRITICAL_STRIKE,
        RL_NPC_ANIMATION_TABLE_BE_HIT,                 //RL_ANIMATION_TABLE_PHYSICS_DAMAGE,
        RL_NPC_ANIMATION_TABLE_BE_HIT,                 //RL_ANIMATION_TABLE_SOLAR_MAGIC_DAMAGE,
        RL_NPC_ANIMATION_TABLE_BE_HIT,                 //RL_ANIMATION_TABLE_NEUTRAL_MAGIC_DAMAGE,
        RL_NPC_ANIMATION_TABLE_BE_HIT,                 //RL_ANIMATION_TABLE_LUNAR_MAGIC_DAMAGE,
        RL_NPC_ANIMATION_TABLE_BE_HIT,                 //RL_ANIMATION_TABLE_POISON_DAMAGE,
        RL_NPC_ANIMATION_TABLE_IDLE,                   //RL_ANIMATION_TABLE_REFLECT_DAMAGE,
        RL_NPC_ANIMATION_TABLE_IDLE,                   //RL_ANIMATION_TABLE_HEAL,
        RL_NPC_ANIMATION_TABLE_IDLE,                   //RL_ANIMATION_TABLE_STEAL_LIFE,
        RL_NPC_ANIMATION_TABLE_IDLE,                   //RL_ANIMATION_TABLE_ABSORB_DAMAGE,
        RL_NPC_ANIMATION_TABLE_IDLE,                   //RL_ANIMATION_TABLE_SHIELD_DAMAGE,        
    };

    return s_nAnimationTables[nAnimationTable];
}

int GetCharacterAnimationIndex(KRLCharacterEquipResource const& EquipResource, RL_SCHOOL nSchool, RL_ANIMATION_TABLE nAnimationTable, RL_WEAPON nWeapon, 
                               KRLAdjustAnimationParam const* pAdjustAnimationParam, KRLAnimationIndex* pAnimationIndex)
{
    int nResult = false;

    KG_ASSERT_EXIT(pAnimationIndex);

    if (EquipResource.nObject == RL_OBJECT_PLAYER)
    {
        DWORD dwAnimationIndexID = 0;
        KRLAnimationIndex const* pSrcAnimationIndex = NULL;

        KG_PROCESS_ERROR(EquipResource.Player.nRoleType != rtInvalid);

        dwAnimationIndexID = g_pRL->m_TableList.GetPlayerAnimationIndexID(nSchool, nAnimationTable, nWeapon);

        dwAnimationIndexID = GetAdjustAnimationIndexID(dwAnimationIndexID, pAdjustAnimationParam);

        pSrcAnimationIndex = g_pRL->m_TableList.GetPlayerAnimationIndex(dwAnimationIndexID, EquipResource.Player.nRoleType);

        if (pSrcAnimationIndex)
        {
            memcpy(pAnimationIndex, pSrcAnimationIndex, sizeof(KRLAnimationIndex));
        }
        else
        {
            pAnimationIndex->dwAnimationIndexID = 0;
            pAnimationIndex->dwAnimationIndexType = RL_ANIMATION_INDEX_NONE;
            pAnimationIndex->dwID = dwAnimationIndexID;
        }
    }
    else
    {
        KRLNpcModel const* pNpcModel = NULL;
        
        ASSERT(EquipResource.nObject == RL_OBJECT_NPC || EquipResource.nObject == RL_OBJECT_UNKNOWN);

        pNpcModel = g_pRL->m_TableList.GetNpcModel(EquipResource.dwModelID);
        if (pNpcModel)
        {
            pAnimationIndex->dwAnimationIndexID = 0;
            pAnimationIndex->dwAnimationIndexType = RL_ANIMATION_INDEX_NONE;
            pAnimationIndex->dwID = KRLNpcModel::GetAnimationID(pNpcModel, AnimationTablePlayerToNpc(nAnimationTable));
        }
    }

    nResult = true;
Exit0:
    return nResult;
}

DWORD GetCharacterRandomAnimation(KRLCharacterEquipResource const& EquipResource, DWORD dwRandomAnimationID)
{
    DWORD dwAnimationID = dwRandomAnimationID;

    if (EquipResource.nObject == RL_OBJECT_PLAYER)
    {
        KRLRandomAnimation const* pRandomAnimation = NULL;

        KG_PROCESS_ERROR(EquipResource.Player.nRoleType != rtInvalid);

        pRandomAnimation = g_pRL->m_TableList.GetPlayerRandomAnimation(dwRandomAnimationID, EquipResource.Player.nRoleType);

        if (pRandomAnimation)
        {
            dwAnimationID = GetRandomAnimationID(pRandomAnimation);
        }
    }

Exit0:
    return dwAnimationID;
}

void FillChestResource(KPlayerChestEquipModel const* pModel, KPlayerEquipSFXModel const* pSFXModel, KRepresentPlayerEquipPartResource* pPart)
{
    pPart->szMeshFilePath = pModel->szMesh;
    pPart->szMtlFilePath = pModel->szMaterial;
    pPart->fMeshScale = pModel->fScale;
    pPart->nColorChannel = pModel->nColorChannel;

    if (pSFXModel)
    {
        pPart->szSFX1FilePath = pSFXModel->szSFXFileLeft;
        pPart->szSFX2FilePath = pSFXModel->szSFXFileRight;
    }    
}

void FillHairResource(KPlayerHairModel const* pModel, KRepresentPlayerEquipPartResource* pPart)
{
    pPart->szMeshFilePath = pModel->szMesh;
    pPart->szMtlFilePath = pModel->szMaterial;
    pPart->fMeshScale = pModel->fScale;
    pPart->nColorChannel = pModel->nColorChannel;
}

void FillHatResource(KPlayerHairModel const* pHeadformModel, KPlayerHatModel const* pHatModel, KPlayerEquipSFXModel const* pHatSFXModel, KRepresentPlayerEquipPartResource* pHatPart)
{
    // 光头要隐藏冠和帽子
    if (
         pHeadformModel && pHeadformModel->nType == HAIR_TYPE_BARE &&
         pHatModel && (pHatModel->nType == HAT_TYPE_CORONAL || pHatModel->nType == HAT_TYPE_HAT)
    )
    {
        return;
    }

    if (pHatModel)
    {
        pHatPart->szSocketName = pHatModel->szSocketName;
        pHatPart->szMeshFilePath = pHatModel->szMesh;
        pHatPart->szMtlFilePath = pHatModel->szMaterial;
        pHatPart->fMeshScale = pHatModel->fScale;
        pHatPart->nColorChannel = pHatModel->nColorChannel;

        if (pHatSFXModel)
        {
            pHatPart->szSFX1FilePath = pHatSFXModel->szSFXFileLeft;
            pHatPart->szSFX2FilePath = pHatSFXModel->szSFXFileRight;
        }
    }
}

void FillWaistResource(KPlayerEquipModel const* pModel, KPlayerEquipSFXModel const* pSFXModel, KRepresentPlayerEquipPartResource* pPart)
{
    pPart->szMeshFilePath = pModel->szMesh;
    pPart->szMtlFilePath = pModel->szMaterial;
    pPart->fMeshScale = pModel->fScale;
    pPart->nColorChannel = pModel->nColorChannel;

    if (pSFXModel)
    {
        pPart->szSFX1FilePath = pSFXModel->szSFXFileLeft;
        pPart->szSFX2FilePath = pSFXModel->szSFXFileRight;
    }
}

void FillPantsResource(KPlayerEquipModel const* pModel, KRepresentPlayerEquipPartResource* pPart)
{
    pPart->szMeshFilePath = pModel->szMesh;
    pPart->szMtlFilePath = pModel->szMaterial;
    pPart->fMeshScale = pModel->fScale;
    pPart->nColorChannel = pModel->nColorChannel;
}

void FillBangleResource(KPlayerEquipModel const* pModel, KPlayerEquipSFXModel const* pSFXModel, KRepresentPlayerEquipPartResource* pPart)
{
    pPart->szMeshFilePath = pModel->szMesh;
    pPart->szMtlFilePath = pModel->szMaterial;
    pPart->fMeshScale = pModel->fScale;
    pPart->nColorChannel = pModel->nColorChannel;

    if (pSFXModel)
    {
        pPart->szSFX1FilePath = pSFXModel->szSFXFileLeft;
        pPart->szSFX2FilePath = pSFXModel->szSFXFileRight;
    }
}

void FillMeleeWeaponResource(KPlayerWeaponModel const* pModel, KPlayerEquipSFXModel const* pMeteorModel, 
    KPlayerEquipSFXModel const* pFringeModel, KRepresentPlayerEquipPartResource* pPartLH, KRepresentPlayerEquipPartResource* pPartRH)
{
    pPartLH->szSocketName = pModel->ModelBaseLH.szSocketName;
    pPartLH->szMeshFilePath = pModel->ModelBaseLH.szMesh;
    pPartLH->szMtlFilePath = pModel->ModelBaseLH.szMaterial;
    pPartLH->szSheathSocketName = GetPlayerSheathSocketLH(pModel->nSheathSocket);
    pPartLH->szSheathMeshFilePath = pModel->ModelBaseLH.szMeshSheath;
    pPartLH->szSheathMtlFilePath = pModel->ModelBaseLH.szMaterialSheath;
    pPartLH->fMeshScale = pModel->fScale;
    pPartLH->nColorChannel = pModel->nColorChannel;

    pPartRH->szSocketName = pModel->ModelBaseRH.szSocketName;
    pPartRH->szMeshFilePath = pModel->ModelBaseRH.szMesh;
    pPartRH->szMtlFilePath = pModel->ModelBaseRH.szMaterial;
    pPartRH->szSheathSocketName = GetPlayerSheathSocketRH(pModel->nSheathSocket);
    pPartRH->szSheathMeshFilePath = pModel->ModelBaseRH.szMeshSheath;
    pPartRH->szSheathMtlFilePath = pModel->ModelBaseRH.szMaterialSheath;
    pPartRH->fMeshScale = pModel->fScale;
    pPartRH->nColorChannel = pModel->nColorChannel;

    if (pMeteorModel)
    {
        pPartLH->szSFX1FilePath = pMeteorModel->szSFXFileLeft;
        pPartRH->szSFX1FilePath = pMeteorModel->szSFXFileRight;
    }
    if (pFringeModel)
    {
        pPartLH->szSFX2FilePath = pFringeModel->szSFXFileLeft;
        pPartRH->szSFX2FilePath = pFringeModel->szSFXFileRight;
    }
}

void FillBackExtendResource(KPlayerEquipModel const* pModel, KRepresentPlayerEquipPartResource* pPart)
{
    pPart->szSocketName = pModel->szSocketName;
    pPart->szMeshFilePath = pModel->szMesh;
    pPart->szMtlFilePath = pModel->szMaterial;                
    pPart->fMeshScale = pModel->fScale;
    pPart->nColorChannel = pModel->nColorChannel;
}

void FillWaistExtendResource(KPlayerEquipModel const* pModel, KRepresentPlayerEquipPartResource* pPart)
{
    pPart->szSocketName = pModel->szSocketName;
    pPart->szMeshFilePath = pModel->szMesh;
    pPart->szMtlFilePath = pModel->szMaterial;                
    pPart->fMeshScale = pModel->fScale;
    pPart->nColorChannel = pModel->nColorChannel;
}

void FillFaceResource(KPlayerEquipModel const* pModel, KRepresentPlayerEquipPartResource* pPart)
{
    pPart->szSocketName = pModel->szSocketName;
    pPart->szMeshFilePath = pModel->szMesh;
    pPart->szMtlFilePath = pModel->szMaterial;                
    pPart->fMeshScale = pModel->fScale;
    pPart->nColorChannel = pModel->nColorChannel;
}

HRESULT LoadFixedTrackMissileFrameFile(int nIndex, D3DXVECTOR3* vPosition)
{
    LPCTSTR szFilePath = NULL;
    TCHAR szValue[MAX_PATH];
    TCHAR szKey[64];
    KSmartTabFile sTab;
    int nRow = 0;

    szFilePath = g_pRL->m_FilePath.GetFilePath(KFilePath::FILE_PATH_MISSILE_FIXED_TRACK_FRAME_TABLE);
    KGLOG_PROCESS_ERROR(szFilePath);

    sTab = g_OpenTabFile(szFilePath);
    KGLOG_PROCESS_ERROR(sTab);

    _itot(static_cast<int>(nIndex), szValue, 10);

    nRow = sTab->FindRow(szValue);
    KGLOG_PROCESS_ERROR(nRow > 0);

    for (int nFrame = 0; nFrame != MISSILE_FIXED_TRACK_FRAME_COUNT; ++nFrame)
    {
        _sntprintf(szKey, _countof(szKey), _T("%s_%d"), _T("FrameX"), nFrame);
        sTab->GetFloat(nRow, szKey, 0.0f, &vPosition[nFrame].x);

        _sntprintf(szKey, _countof(szKey), _T("%s_%d"), _T("FrameY"), nFrame);
        sTab->GetFloat(nRow, szKey, 0.0f, &vPosition[nFrame].y);

        _sntprintf(szKey, _countof(szKey), _T("%s_%d"), _T("FrameZ"), nFrame);
        sTab->GetFloat(nRow, szKey, 0.0f, &vPosition[nFrame].z);
    }

    return S_OK;
Exit0:
    return E_FAIL;
}


int GetSkillEffectID(DWORD* pEffectID, int* pEffectIndex, size_t nCount, KCaster const& Caster, DWORD dwSkillID, DWORD dwSkillLevel)
{
    int nResult = false;

    memset(pEffectID, 0, sizeof(DWORD) * nCount);

    switch (Caster.eTargetType)
    {
    case ttNpc:
        {
            KSkillCasterModel const* pSkillCasterModel = NULL;

            pSkillCasterModel = g_pRL->m_TableList.GetNpcSkillCasterModel(dwSkillID, dwSkillLevel, Caster.dwRepresentID);
            KG_PROCESS_ERROR(pSkillCasterModel);

            for (size_t nIndex = 0; nIndex < nCount; ++nIndex)
            {
                int const nSkillEffectResultIndex = pEffectIndex[nIndex];
                if (nSkillEffectResultIndex != -1)
                {
                    pEffectID[nIndex] = static_cast<DWORD>(pSkillCasterModel->dwSkillEffectResultID[nSkillEffectResultIndex]);
                    if (!pEffectID[nIndex] && nSkillEffectResultIndex != REPRESENT_SKILL_EFFECT_RESULT_MISS)
                    {
                        pEffectID[nIndex] = static_cast<DWORD>(pSkillCasterModel->dwSkillEffectResultID[REPRESENT_SKILL_EFFECT_RESULT_HIT]);
                    }
                }
            }
        }
        break;
    case ttPlayer:
        {
            KSkillCasterModel const* pSkillCasterModel = NULL;

            pSkillCasterModel = g_pRL->m_TableList.GetPlayerSkillCasterModel(dwSkillID, dwSkillLevel, Caster.nRoleType);
            KG_PROCESS_ERROR(pSkillCasterModel);

            for (size_t nIndex = 0; nIndex < nCount; ++nIndex)
            {
                int const nSkillEffectResultIndex = pEffectIndex[nIndex];
                if (nSkillEffectResultIndex != -1)
                {
                    pEffectID[nIndex] = static_cast<DWORD>(pSkillCasterModel->dwSkillEffectResultID[nSkillEffectResultIndex]);
                    if (!pEffectID[nIndex] && nSkillEffectResultIndex != REPRESENT_SKILL_EFFECT_RESULT_MISS)
                    {
                        pEffectID[nIndex] = static_cast<DWORD>(pSkillCasterModel->dwSkillEffectResultID[REPRESENT_SKILL_EFFECT_RESULT_HIT]);
                    }
                }
            }
        }
        break;
    };

    nResult = true;
Exit0:
    return nResult;
}

HRESULT GetDoodadModelResource(DWORD dwRepresentID, KRepresentDoodadResource* pResource)
{
    HRESULT hrResult = E_FAIL;
    KRLDoodadModel const* pModel = NULL;

    KGLOG_PROCESS_ERROR(pResource);

    memset(pResource, 0, sizeof(KRepresentDoodadResource));

    pModel = g_pRL->m_TableList.GetDoodadModel(dwRepresentID);
    KGLOG_PROCESS_ERROR(pModel);

    pResource->nFogVisible = pModel->nFogVisible;
    pResource->nColorChannelTable = pModel->nColorChannelTable;
    pResource->nColorChannel = pModel->nColorChannel;
    pResource->nVisiblePriority = pModel->nVisiblePriority;
    pResource->fModelScale = pModel->fModelScale;
    pResource->dwPlayerOpenAnimationID = pModel->dwPlayerOpenAnimationID;
    pResource->dwPlayerBeginPickAnimationID = pModel->dwPlayerBeginPickAnimationID;
    pResource->dwPlayerEndPickAnimationID = pModel->dwPlayerEndPickAnimationID;

    for (int nAni = 0; nAni != RL_DOODAD_ANIMATION_COUNT; ++nAni)
    {
        KRepresentDoodadAnimationResource& lhs = pResource->aAnimation[nAni];
        KRLDoodadModel::KRLAnimationParam const& rhs = pModel->Animations[nAni];

        lhs.szMeshFilePath = rhs.szMeshFilePath;
        lhs.szAniFilePath = rhs.szAniFilePath;
        lhs.szSFXFilePath = rhs.szSFXFilePath;
        lhs.dwAniPlayType = AnimationPlayTypeRLTo3D((RL_ANIMATION_PLAY_TYPE)rhs.dwAniPlayType);
    }

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (pResource)
        {
            memset(pResource, 0, sizeof(KRepresentDoodadResource));
        }
    }
    return hrResult;
}

HRESULT GetRidesEquipModelResource(DWORD dwRidesID, DWORD dwRepresentIDs[MODEL_RIDES_EQUIP_AND_SOCKET_COUNT], KRepresentRidesEquipResource* pResource)
{
    HRESULT hrResult = E_FAIL;
    KRidesModel const* pModel = NULL;
    KRidesEquipModel const* pEquip = NULL;
    KRidesSocketModel const* pSocket = NULL;
    KRepresentRidesEquipPartResource* pBodyPart = NULL;
    KRepresentRidesEquipPartResource* pManePart = NULL;

    KGLOG_PROCESS_ERROR(pResource);

    memset(pResource, 0, sizeof(KRepresentRidesEquipResource));

    // Load MDL
    pModel = g_pRL->m_TableList.GetRidesModel(dwRidesID);
    KG_PROCESS_SUCCESS(pModel == NULL);

    pResource->szMDLFilePath = pModel->szMDLFilePath;
    KG_PROCESS_SUCCESS(pResource->szMDLFilePath[0] == _T('\0'));

    // Load Equip

    // 马身和鬃毛的资源是填在rides.txt里的
    pBodyPart = pResource->Parts + MODEL_RIDES_EQUIP_BODY;
    pManePart = pResource->Parts + MODEL_RIDES_EQUIP_MANE;

    pBodyPart->szMeshFilePath = pModel->szBodyMesh;
    pBodyPart->szMtlFilePath = pModel->szBodyMaterial;
    pBodyPart->fMeshScale = pModel->fModelScale;
    pBodyPart->nGlobalColorChannel = pModel->nBodyGlobalColorChannel;
    pBodyPart->nColorChannel = pModel->nBodyColorChannel;

    pManePart->szMeshFilePath = pModel->szManeMesh;
    pManePart->szMtlFilePath = pModel->szManeMaterial;
    pManePart->fMeshScale = pModel->fModelScale;
    pManePart->nGlobalColorChannel = pModel->nManeGlobalColorChannel;
    pManePart->nColorChannel = pModel->nManeColorChannel;

    for (int nIndex = MODEL_RIDES_EQUIP_HEAD; nIndex < MODEL_RIDES_EQUIP_COUNT; ++nIndex)
    {
        KRepresentRidesEquipPartResource& part = pResource->Parts[nIndex];

        if (!dwRepresentIDs[nIndex])
            continue;

        pEquip = g_pRL->m_TableList.GetRidesEquipModel(nIndex, dwRepresentIDs[nIndex]);
        if (pEquip)
        {
            part.szMeshFilePath = pEquip->szMesh;
            part.szMtlFilePath = pEquip->szMaterial;
            part.fMeshScale = pEquip->fScale;
            part.nGlobalColorChannel = pEquip->nGlobalColorChannel;
            part.nColorChannel = pEquip->nColorChannel;
        }
    }

    // Load Socket
    for (int nIndex = 0; nIndex < MODEL_RIDES_SOCKET_COUNT; ++nIndex)
    {
        int nId = MODEL_RIDES_EQUIP_COUNT + nIndex;
        KRepresentRidesEquipPartResource& part = pResource->Parts[nId];

        if (!dwRepresentIDs[nId])
            continue;

        pSocket= g_pRL->m_TableList.GetRidesSocketModel(nIndex, dwRepresentIDs[nId]);
        if (pSocket)
        {
            part.szMeshFilePath = pSocket->szMesh;
            part.szMtlFilePath = pSocket->szMaterial;
            part.szSocketName = pSocket->szSocketName;
            part.fMeshScale = pSocket->fScale;
            part.nGlobalColorChannel = pSocket->nGlobalColorChannel;
            part.nColorChannel = pSocket->nColorChannel;
        }
    }

Exit1:
    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (pResource)
        {
            memset(pResource, 0, sizeof(KRepresentRidesEquipResource));
        }
    }
    return hrResult;
}

HRESULT GetRidesAnimationModelResource(DWORD dwRidesID, DWORD dwAnimationID, KRepresentRidesAnimationResource* pResource)
{
    HRESULT hrResult = E_FAIL;
    KRidesAnimationModel const* pAniModel = NULL;

    KGLOG_PROCESS_ERROR(pResource);

    memset(pResource, 0, sizeof(KRepresentRidesAnimationResource));

    KG_PROCESS_SUCCESS(!dwAnimationID);

    pAniModel = g_pRL->m_TableList.GetRidesAnimationModel(dwRidesID, dwAnimationID);
    KG_PROCESS_SUCCESS(pAniModel == NULL);

    pResource->dwAniPlayType      = AnimationPlayTypeRLTo3D((RL_ANIMATION_PLAY_TYPE)pAniModel->dwAnimationType);
    pResource->szAniFilePath      = pAniModel->szAnimationFile;
    pResource->fAniPlaySpeed      = pAniModel->fAnimationSpeed;
    pResource->szSFXFilePath      = pAniModel->szSFXFile;
    pResource->szSFXBoneName      = pAniModel->szBoneName;
    pResource->dwSFXPlayType      = pAniModel->dwSFXType;
    pResource->fSFXPlaySpeed      = pAniModel->fSFXSpeed;
    pResource->fSFXScale          = pAniModel->fSFXScale;
    pResource->fAniSoundRange     = 0.0f;
    pResource->szAniSoundFilePath = _T("");

Exit1:
    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (pResource)
        {
            memset(pResource, 0, sizeof(KRepresentRidesAnimationResource));
        }
    }
    return hrResult;
}

HRESULT GetNpcEquipModelResource(DWORD dwRepresentID, KRepresentNPCEquipResource* pResource)
{
    HRESULT hrResult = E_FAIL;
    KRLNpcModel const* pNpcModel = NULL;

    KGLOG_PROCESS_ERROR(pResource);

    memset(pResource, 0, sizeof(KRepresentNPCEquipResource));

    pNpcModel = g_pRL->m_TableList.GetNpcModel(dwRepresentID);
    KG_PROCESS_SUCCESS(pNpcModel == NULL);

    pResource->szMDLFilePath = pNpcModel->szMainModelFile;
    KG_PROCESS_SUCCESS(pResource->szMDLFilePath[0] == _T('\0'));

    pResource->nColorChannelTable = pNpcModel->nColorChannelTable;
    pResource->nColorChannel = pNpcModel->nColorChannel;
    pResource->fModelScale = pNpcModel->fModelScale;
    pResource->fSocketScale = pNpcModel->fSocketScale;
    pResource->nSoundDistance = 0;

    // Load socket
    for (int nIndex = 0; nIndex != RL_NPC_SOCKET_COUNT; ++nIndex)
    {
        //KRepresentNPCEquipPartResource& part = pResource->Parts[MODEL_DOODAD_EQUIP_COUNT + nIndex];
		KRepresentNPCEquipPartResource& part = pResource->Parts[nIndex];
        part.szMeshFilePath = pNpcModel->Sockets[nIndex].szMeshFilePath;
        if (part.szMeshFilePath[0] != _T('\0'))
            part.szMtlFilePath = pNpcModel->Sockets[nIndex].szMtlFilePath;
        else
            part.szMtlFilePath = _T("");
    }

Exit1:
    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (pResource)
        {
            memset(pResource, 0, sizeof(KRepresentNPCEquipResource));
        }
    }
    return hrResult;
}

HRESULT GetNpcAnimationModelResource(DWORD dwRepresentID, DWORD dwAnimationID, KRepresentNPCAnimationResource* pResource)
{
    HRESULT hrResult = E_FAIL;
    int nRetCode = false;
    KNpcAnimationModel NpcAnimation;

    KGLOG_PROCESS_ERROR(pResource);

    memset(pResource, 0, sizeof(KRepresentNPCAnimationResource));

    if (dwRepresentID)
    {
        nRetCode = g_pRL->m_TableList.GetNpcAnimationModel(dwRepresentID, dwAnimationID, &NpcAnimation);
        if (nRetCode)
		{
	        pResource->dwAniPlayType      = AnimationPlayTypeRLTo3D((RL_ANIMATION_PLAY_TYPE)NpcAnimation.dwAnimationType);
	        pResource->szAniFilePath      = NpcAnimation.szAnimationFile;
	        pResource->fAniPlaySpeed      = NpcAnimation.fAnimationSpeed;
	        pResource->szSFXFilePath      = NpcAnimation.szSFXFile;
	        pResource->szSFXBoneName      = NpcAnimation.szSFXBoneName;
	        pResource->dwSFXPlayType      = NpcAnimation.dwSFXType;
	        pResource->fSFXPlaySpeed      = NpcAnimation.fSFXSpeed;
	        pResource->fSFXScale          = NpcAnimation.fSFXScale;
	        pResource->szAniSoundFilePath = _T("");
	        pResource->fAniSoundRange     = 0.0f;
	  	}
    }

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (pResource)
        {
            memset(pResource, 0, sizeof(KRepresentNPCAnimationResource));
        }
    }
    return hrResult;
}

HRESULT GetPlayerAnimationModelResource(DWORD dwAnimationID, ROLE_TYPE nRoleType, KRepresentPlayerAnimationResource* pResource)
{
    HRESULT hrResult = E_FAIL;
    KRLPlayerAnimationModel const* pAnimationModel = NULL;

    KGLOG_PROCESS_ERROR(pResource);

    memset(pResource, 0, sizeof(KRepresentPlayerAnimationResource));

    pAnimationModel = g_pRL->m_TableList.GetPlayerAnimationModel(dwAnimationID, nRoleType);
    KG_PROCESS_SUCCESS(pAnimationModel == NULL);

    pResource->dwAniPlayType = AnimationPlayTypeRLTo3D((RL_ANIMATION_PLAY_TYPE)pAnimationModel->dwAnimationType);
    pResource->szAniFilePath = pAnimationModel->szAnimationFile;
    pResource->fAniPlaySpeed = pAnimationModel->fAnimationSpeed;

Exit1:
    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (pResource)
        {
            memset(pResource, 0, sizeof(KRepresentPlayerAnimationResource));
        }
    }
    return hrResult;
}

HRESULT GetPlayerEquipModelResource(DWORD const dwRepresentIDs[perRepresentCount], ROLE_TYPE nRoleType, KRepresentPlayerEquipResource* pResource)
{
    int nRetCode = false;

    KRLPlayerModel const* pModel = NULL;
    KPlayerChestEquipModel const* pChestModel = NULL;
    KPlayerHairModel const* pHeadformModel = NULL;
    KPlayerHairModel const* pBangModel = NULL;
    KPlayerHairModel const* pPlaitModel = NULL;
    KPlayerEquipModel const* pWaistModel = NULL;
    KPlayerEquipModel const* pPantsModel = NULL;
    KPlayerEquipModel const* pBangleModel = NULL;
    KPlayerWeaponModel const* pWeaponModel = NULL;
    KPlayerEquipModel const* pBackExtendModel = NULL;
    KPlayerEquipModel const* pWaistExtendModel = NULL;
    KPlayerEquipModel const* pFaceModel = NULL;
    KPlayerHatModel const* pHatModel = NULL;
    KPlayerEquipSFXModel const* pChestSFXModel = NULL;
    KPlayerEquipSFXModel const* pWaistSFXModel = NULL;
    KPlayerEquipSFXModel const* pBangleSFXModel = NULL;
    KPlayerEquipSFXModel const* pWeaponMeteorModel = NULL;
    KPlayerEquipSFXModel const* pWeaponFringeModel = NULL;
    KPlayerEquipSFXModel const* pHatSFXModel = NULL;
    BOOL bEquipHiddens[RL_EQUIP_HIDE_COUNT];
    BOOL bHiddens[RL_EQUIP_HIDE_COUNT][RL_EQUIP_HIDE_COUNT];
    DWORD dwHeadformID = 0;
    DWORD dwBangID = 0;
    DWORD dwPlaitID = 0;

    memset(bHiddens, 0, sizeof(bHiddens));
    memset(bEquipHiddens, 0, sizeof(bEquipHiddens));
    pResource->szMDLFilePath = _T("");
    for (size_t nIndex = 0; nIndex < MODEL_PLAYER_EQUIP_AND_SOCKET_COUNT; ++nIndex)
    {
        KRepresentPlayerEquipPartResource& part = pResource->Parts[nIndex];
        part.szMeshFilePath = _T("");
        part.szMtlFilePath = _T("");
        part.szSFX1FilePath = _T("");
        part.szSFX2FilePath = _T("");
        part.szSheathSocketName = _T("");
        part.szSheathMeshFilePath = _T("");
        part.szSheathMtlFilePath = _T("");
        part.szSocketName = _T("");
        part.fMeshScale = 1.0f;
        part.nColorChannel = 0;
    }

    g_pRL->m_TableList.GetOriginalHairIndex(
        dwRepresentIDs[perHairStyle],
        &dwHeadformID,
        &dwBangID,
        &dwPlaitID
    );

    pHatModel = g_pRL->m_TableList.GetPlayerHatModel(dwRepresentIDs[perHelmStyle], nRoleType);
    if (pHatModel)
    {
        g_pRL->m_TableList.GetFinallyHairIndex(dwHeadformID, pHatModel->nType, &dwHeadformID);

        g_pRL->m_TableList.GetFinallyHairIndex(dwBangID, pHatModel->nType, &dwBangID);

        g_pRL->m_TableList.GetFinallyHairIndex(dwPlaitID, pHatModel->nType, &dwPlaitID);
    }

    pHeadformModel = g_pRL->m_TableList.GetPlayerHairModel(dwHeadformID, nRoleType);
    pBangModel = g_pRL->m_TableList.GetPlayerHairModel(dwBangID, nRoleType);
    pPlaitModel = g_pRL->m_TableList.GetPlayerHairModel(dwPlaitID, nRoleType);
    pChestModel = g_pRL->m_TableList.GetPlayerChestEquipModel(dwRepresentIDs[perChestStyle], nRoleType);
    pWaistModel = g_pRL->m_TableList.GetPlayerWaistModel(dwRepresentIDs[perWaistStyle], nRoleType);
    pPantsModel = g_pRL->m_TableList.GetPlayerPantsModel(dwRepresentIDs[perBootsStyle], nRoleType);
    pBangleModel = g_pRL->m_TableList.GetPlayerBangleModel(dwRepresentIDs[perBangleStyle], nRoleType);
    pWeaponModel = g_pRL->m_TableList.GetPlayerWeaponModel(dwRepresentIDs[perWeaponStyle], nRoleType);
    pBackExtendModel = g_pRL->m_TableList.GetPlayerBackExtendModel(dwRepresentIDs[perBackExtend], nRoleType);
    pWaistExtendModel = g_pRL->m_TableList.GetPlayerWaistExtendModel(dwRepresentIDs[perWaistExtend], nRoleType);
    pFaceModel = g_pRL->m_TableList.GetPlayerFaceModel(dwRepresentIDs[perFaceStyle], nRoleType);
    pChestSFXModel = g_pRL->m_TableList.GetPlayerSFXModel(dwRepresentIDs[perChestStyle], dwRepresentIDs[perChestEnchant], CHARACTER_EQUIP_SFX_FILE_CHEST, nRoleType);

    pWaistSFXModel = g_pRL->m_TableList.GetPlayerSFXModel(dwRepresentIDs[perWaistStyle], dwRepresentIDs[perWaistEnchant], CHARACTER_EQUIP_SFX_FILE_WAIST, nRoleType);
    pBangleSFXModel = g_pRL->m_TableList.GetPlayerSFXModel(dwRepresentIDs[perBangleStyle], dwRepresentIDs[perBangleEnchant], CHARACTER_EQUIP_SFX_FILE_BANGLE, nRoleType);
    pWeaponMeteorModel = g_pRL->m_TableList.GetPlayerSFXModel(dwRepresentIDs[perWeaponStyle], dwRepresentIDs[perWeaponEnchant1], CHARACTER_EQUIP_SFX_FILE_RL_WEAPON_METEOR, nRoleType);
    pWeaponFringeModel = g_pRL->m_TableList.GetPlayerSFXModel(dwRepresentIDs[perWeaponStyle], dwRepresentIDs[perWeaponEnchant2], CHARACTER_EQUIP_SFX_FILE_RL_WEAPON_FRINGE, nRoleType);
    pHatSFXModel = g_pRL->m_TableList.GetPlayerSFXModel(dwRepresentIDs[perHelmStyle], dwRepresentIDs[perHelmEnchant], CHARACTER_EQUIP_SFX_FILE_HAT, nRoleType);

    if (pChestModel)
        memcpy(bHiddens[0], pChestModel->bHideFlag, sizeof(bHiddens[0]));
    if (pPantsModel)  
        memcpy(bHiddens[1], pPantsModel->bHideFlag, sizeof(bHiddens[1]));
    if (pHatModel)
        memcpy(bHiddens[2], pHatModel->bHideFlag, sizeof(bHiddens[2]));
    if (pFaceModel)
        memcpy(bHiddens[3], pFaceModel->bHideFlag, sizeof(bHiddens[3]));
    if (pBangleModel)
        memcpy(bHiddens[4], pBangleModel->bHideFlag, sizeof(bHiddens[4]));
    if (pWeaponModel)
        memcpy(bHiddens[5], pWeaponModel->bHideFlag, sizeof(bHiddens[5]));
    if (pWaistModel)
        memcpy(bHiddens[6], pWaistModel->bHideFlag, sizeof(bHiddens[6]));
    if (pBackExtendModel)
        memcpy(bHiddens[7], pBackExtendModel->bHideFlag, sizeof(bHiddens[7]));
    if (pWaistExtendModel)
        memcpy(bHiddens[8], pWaistExtendModel->bHideFlag, sizeof(bHiddens[8]));

    for (int i = 0; i < RL_EQUIP_HIDE_COUNT; ++i)
    {
        for (int j = i + 1; j < RL_EQUIP_HIDE_COUNT; ++j)
        {         
            if (bHiddens[i][j])
            {
                bEquipHiddens[j] = bHiddens[i][j];
            }
        }
    }

    // Load MDL
    pModel = g_pRL->m_TableList.GetPlayerModel(nRoleType);
    KG_PROCESS_SUCCESS(pModel == NULL);

    pResource->szMDLFilePath = pModel->szMDLFilePath;
    KG_PROCESS_SUCCESS(pResource->szMDLFilePath[0] == _T('\0'));

    if (pChestModel && !bEquipHiddens[RL_EQUIP_HIDE_CHEST])
    {
        FillChestResource(pChestModel, pChestSFXModel, &pResource->Parts[MODEL_PLAYER_EQUIP_CHEST]);
    }
    if (pHeadformModel)
    {
        FillHairResource(pHeadformModel, &pResource->Parts[MODEL_PLAYER_EQUIP_HEADFORM]);
    }
    if (pBangModel)
    {
        FillHairResource(pBangModel, &pResource->Parts[MODEL_PLAYER_EQUIP_BANG]);
    }
    if (pPlaitModel)
    {
        FillHairResource(pPlaitModel, &pResource->Parts[MODEL_PLAYER_EQUIP_PLAIT]);
    }
    if (pHatModel && !bEquipHiddens[RL_EQUIP_HIDE_HAT])
    {
        FillHatResource(pHeadformModel, pHatModel, pHatSFXModel, &pResource->Parts[MODEL_PLAYER_SOCKET_HAT]);
    }
    if (pWaistModel && !bEquipHiddens[RL_EQUIP_HIDE_WAIST])
    {
        FillWaistResource(pWaistModel, pWaistSFXModel, &pResource->Parts[MODEL_PLAYER_EQUIP_WAIST]);

        pResource->Parts[MODEL_PLAYER_EQUIP_WAIST].nColorChannel = dwRepresentIDs[perWaistColor];
    }
    if (pPantsModel && !bEquipHiddens[RL_EQUIP_HIDE_PANTS])
    {
        FillPantsResource(pPantsModel, &pResource->Parts[MODEL_PLAYER_EQUIP_PANTS]);
    }
    if (pBangleModel && !bEquipHiddens[RL_EQUIP_HIDE_BANGLE])
    {
        FillBangleResource(pBangleModel, pBangleSFXModel, &pResource->Parts[MODEL_PLAYER_EQUIP_BANGLE]);
    }
    if (pWeaponModel && !bEquipHiddens[RL_EQUIP_HIDE_MELEE_WEAPON])
    {
        FillMeleeWeaponResource(pWeaponModel, pWeaponMeteorModel, pWeaponFringeModel,
            &pResource->Parts[MODEL_PLAYER_SOCKET_WEAPON_LH], &pResource->Parts[MODEL_PLAYER_SOCKET_WEAPON_RH]);
    }
    if (pBackExtendModel && !bEquipHiddens[RL_EQUIP_HIDE_BACK_EXTEND])
    {
        FillBackExtendResource(pBackExtendModel, &(pResource->Parts[MODEL_PLAYER_SOCKET_BACK_EXTEND]));
    }
    if (pWaistExtendModel && !bEquipHiddens[RL_EQUIP_HIDE_WAIST_EXTEND])
    {
        FillWaistExtendResource(pWaistExtendModel, &pResource->Parts[MODEL_PLAYER_SOCKET_WAIST_EXTEND]);
    }
    if (pFaceModel && !bEquipHiddens[RL_EQUIP_HIDE_FACE])
    {
        FillFaceResource(pFaceModel, &pResource->Parts[MODEL_PLAYER_SOCKET_FACE]);
    }

    // 游戏逻辑颜色修正
    pResource->Parts[MODEL_PLAYER_SOCKET_HAT].nColorChannel = (int)dwRepresentIDs[perHelmColor];
    pResource->Parts[MODEL_PLAYER_EQUIP_CHEST].nColorChannel = (int)dwRepresentIDs[perChestColor];
    pResource->Parts[MODEL_PLAYER_EQUIP_WAIST].nColorChannel = (int)dwRepresentIDs[perWaistColor];
    pResource->Parts[MODEL_PLAYER_EQUIP_PANTS].nColorChannel = (int)dwRepresentIDs[perBootsColor];
    pResource->Parts[MODEL_PLAYER_EQUIP_BANGLE].nColorChannel = (int)dwRepresentIDs[perBangleColor];

Exit1:
    return S_OK;
}

enuAniPlayMode AnimationPlayTypeRLTo3D(RL_ANIMATION_PLAY_TYPE nRLPlayType)
{
    static enuAniPlayMode s_3DPlayTypes[RL_ANIMATION_PLAY_TYPE_COUNT] =
    {
        ENU_ANIMATIONPLAY_ADDCURRENT_ONCE,
        ENU_ANIMATIONPLAY_ADDCURRENT
    };

    return s_3DPlayTypes[nRLPlayType];
}

int InitRandomAnimation(KRLRandomAnimation* pRandomAnimation, size_t nCount)
{
    int nResult = false;
    
    KG_ASSERT_EXIT(pRandomAnimation);

    for (size_t nIndex = 0; nIndex < nCount; ++nIndex)
    {
        KRLRandomAnimationItem* pItem = pRandomAnimation->Items;

        for (size_t nItem = 1; nItem < RL_RANDOM_ANIMATION_COUNT; ++nItem)
        {
            pItem[nItem].dwRatio += pItem[nItem - 1].dwRatio;
        }

        ++pRandomAnimation;
    }

    nResult = true;
Exit0:
    return nResult;
}

DWORD GetRandomAnimationID(KRLRandomAnimation const* pRandomAnimation)
{
    DWORD dwAnimationID = 0;
    DWORD dwRatioTatal = 0;
    KRLRandomAnimationItem const* pItem = NULL;

    KG_ASSERT_EXIT(pRandomAnimation);

    pItem = pRandomAnimation->Items;

    dwRatioTatal = pItem[RL_RANDOM_ANIMATION_COUNT - 1].dwRatio;

    if (dwRatioTatal > 0)
    {
        DWORD dwRatio = KG_Rand() % dwRatioTatal;

        for (size_t nItem = 0; nItem < RL_RANDOM_ANIMATION_COUNT; ++nItem)
        {
            if (dwRatio < pItem->dwRatio)
            {
                dwAnimationID = pItem->dwAnimationID;
                break;
            }

            ++pItem;
        }
    }

Exit0:
    return dwAnimationID;
}

