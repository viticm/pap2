#include "stdafx.h"
#include "./krlresourcelimit.h"
#include "./SO3Represent.h"
#include "./case/actionobject/krlscene.h"
#include "./case/actionobject/krlcamera.h"
#include "./case/actionobject/krltarget.h"
#include "./case/actionobject/krlremotecharacter.h"
#include "./case/actionobject/krldoodad.h"
#include "SO3World/IKGSO3WorldClient.h"
#include "SO3World/KSO3World.h"

int InitCharacterResourceLimit(KRLResourceLimit* pResourceLimit)
{
    int nRetCode = false;
    int nResult = false;

    KGLOG_PROCESS_ERROR(pResourceLimit);

    pResourceLimit->bEnable = FALSE;

    pResourceLimit->nCharacterVisibleCount = 0;
    pResourceLimit->nCharacterMaxVisibleCount = 32;

    pResourceLimit->pRLRemoteCharacterOptimizationEvaluators = NULL;
    pResourceLimit->nRLRemoteCharacterOptimizationEvaluatorCount = 0;
    pResourceLimit->nRLRemoteCharacterOptimizationEvaluatorMaxCount = 0;

    nResult = true;
Exit0:
    return nResult;
}

int UnInitCharacterResourceLimit(KRLResourceLimit* pResourceLimit)
{
    int nResult = false;

    KGLOG_PROCESS_ERROR(pResourceLimit);

    SAFE_FREE(pResourceLimit->pRLRemoteCharacterOptimizationEvaluators);

    nResult = true;
Exit0:
    return nResult;
}

static int GetLocalCharacterPosition(KRLScene* pRLScene, D3DXVECTOR3* pvPosition)
{
    int nRetCode = false;
    int nResult = false;
    KRLCamera* pRLCamera = NULL;

    ASSERT(pRLScene);
    ASSERT(pvPosition);

    pRLCamera = pRLScene->m_pRLCamera;
    KGLOG_PROCESS_ERROR(pRLCamera);

    pRLCamera->m_CharacterCamera.m_RLLocalCharacter.m_RLCharacter.GetPosition(*pvPosition);

    nResult = true;
Exit0:
    return nResult;
}

struct KRLRemoteCharacterOptimizationEvaluator
{
    float fDistance;
    BOOL bGC;
    BOOL bPlayer;
    KRLRemoteCharacter* pRLRemoteCharacter;
};

static int GetRemoteCharacterDistance(KRLRemoteCharacter* pRLRemoteCharacter, 
                                      D3DXVECTOR3 vLocalCharacterPosition, float* pfDistance)
{
    int nRetCode = false;
    int nResult = false;
    D3DXVECTOR3 vRemoteCharacterPosition;
    D3DXVECTOR3 vDistance;

    ASSERT(pRLRemoteCharacter);
    ASSERT(pfDistance);

    pRLRemoteCharacter->m_RLCharacter.GetPosition(vRemoteCharacterPosition);

    vDistance = vRemoteCharacterPosition - vLocalCharacterPosition;
    
    *pfDistance = D3DXVec3Length(&vDistance);

    nResult = true;
    return nResult;
}

static int PrepareRLRemoteCharacterEvaluatorBuffer(KRLResourceLimit* pResourceLimit, size_t nRLRemoteCharacterCount)
{
    int nResult = false;

    ASSERT(pResourceLimit);

    if (nRLRemoteCharacterCount > pResourceLimit->nRLRemoteCharacterOptimizationEvaluatorMaxCount)
    {
        pResourceLimit->nRLRemoteCharacterOptimizationEvaluatorCount = 0;
        pResourceLimit->nRLRemoteCharacterOptimizationEvaluatorMaxCount = 0;
        SAFE_FREE(pResourceLimit->pRLRemoteCharacterOptimizationEvaluators);

        pResourceLimit->pRLRemoteCharacterOptimizationEvaluators = 
            (KRLRemoteCharacterOptimizationEvaluator*)malloc(sizeof(KRLRemoteCharacterOptimizationEvaluator) * nRLRemoteCharacterCount);
        KGLOG_PROCESS_ERROR(pResourceLimit->pRLRemoteCharacterOptimizationEvaluators);

        pResourceLimit->nRLRemoteCharacterOptimizationEvaluatorMaxCount = nRLRemoteCharacterCount;
    }

    pResourceLimit->nRLRemoteCharacterOptimizationEvaluatorCount = nRLRemoteCharacterCount;

    nResult = true;
Exit0:
    return nResult;
}

static int FillRLRemoteCharacterEvaluator(KRLRemoteCharacterOptimizationEvaluator* pEvaluator, 
                                          KRLRemoteCharacter* pRLRemoteCharacter, 
                                          D3DXVECTOR3 vLocalCharacterPosition,
                                          DWORD dwTargetID,
                                          BOOL bGC)
{
    int nRetCode = false;
    int nResult = false;
    IKGSO3WorldClient* pSO3WorldClient = NULL;
    DWORD dwCharacterID = 0;
    BOOL bIsPartyMember = FALSE;

    ASSERT(pEvaluator);
    ASSERT(pRLRemoteCharacter);

    pSO3WorldClient = g_pRL->m_pSO3WorldClient;
    KGLOG_PROCESS_ERROR(pSO3WorldClient);

    dwCharacterID = pRLRemoteCharacter->m_RLCharacter.GetObjectID();

    if (IS_PLAYER(dwCharacterID))
        pSO3WorldClient->IsPartyMember(g_pRL->m_pSO3World->m_dwClientPlayerID, dwCharacterID, &bIsPartyMember);

    if (bIsPartyMember)
    {
        pEvaluator->fDistance = 0.0f;
    }
    else
    {
        if (dwTargetID != 0 && dwCharacterID == dwTargetID)
        {
            pEvaluator->fDistance = 0.0f;
        }
        else
        {
            nRetCode = GetRemoteCharacterDistance(pRLRemoteCharacter, vLocalCharacterPosition, &pEvaluator->fDistance);
            KGLOG_PROCESS_ERROR(nRetCode);
        }
    }

    pEvaluator->bGC = bGC;
    pEvaluator->bPlayer = pRLRemoteCharacter->m_RLCharacter.m_EquipResource.nObject == RL_OBJECT_PLAYER;
    pEvaluator->pRLRemoteCharacter = pRLRemoteCharacter;

    nResult = true;
Exit0:
    return nResult;
}

static int FillRLRemoteCharacterEvaluators(KRLResourceLimit* pResourceLimit, KRLScene* pRLScene, D3DXVECTOR3 vLocalCharacterPosition)
{
    int nRetCode = false;
    int nResult = false;
    size_t nRLRemoteCharacterCount = 0;
    size_t nPlayerAndTargetCount = 0;
    KRLRemoteCharacterOptimizationEvaluator* pEvaluator = NULL;
    RL_OJBECT nTargetType = RL_OBJECT_UNKNOWN;
    DWORD dwTargetID = 0;

    ASSERT(pResourceLimit);
    ASSERT(pRLScene);

    KRLRemoteCharacterMgr::KPtrMap& Characters = pRLScene->m_CharacterMgr.m_aCharacter;
    KRLRemoteCharacterMgr::KPtrMap& GCCharacters = pRLScene->m_CharacterGCMgr.m_aCharacter;

    // prepare evaluators
    nRLRemoteCharacterCount += Characters.size();
    nRLRemoteCharacterCount += GCCharacters.size();

    KG_PROCESS_SUCCESS(nRLRemoteCharacterCount == 0);

    nRetCode = PrepareRLRemoteCharacterEvaluatorBuffer(pResourceLimit, nRLRemoteCharacterCount);
    KGLOG_PROCESS_ERROR(nRetCode);

    // target
    nTargetType = pRLScene->m_pRLTarget->m_nObjectType;
    if (nTargetType == RL_OBJECT_PLAYER || nTargetType == RL_OBJECT_NPC)
    {
        dwTargetID = pRLScene->m_pRLTarget->m_dwObjectID;
    }

    pEvaluator = pResourceLimit->pRLRemoteCharacterOptimizationEvaluators;

    for (KRLRemoteCharacterMgr::KPtrMap::iterator it = Characters.begin(); it != Characters.end(); ++it)
    {
        KRLRemoteCharacter* pRLRemoteCharacter = it->second;

        if (pRLRemoteCharacter->m_RLCharacter.m_EquipResource.nObject == RL_OBJECT_PLAYER ||
            pRLRemoteCharacter->m_RLCharacter.GetObjectID() == dwTargetID)
        {
            nRetCode = FillRLRemoteCharacterEvaluator(pEvaluator, pRLRemoteCharacter, vLocalCharacterPosition, dwTargetID, FALSE);
            KGLOG_PROCESS_ERROR(nRetCode);

            ++pEvaluator;
            ++nPlayerAndTargetCount;
        }
    }

    for (KRLRemoteCharacterMgr::KPtrMap::iterator it = GCCharacters.begin(); it != GCCharacters.end(); ++it)
    {
        KRLRemoteCharacter* pRLRemoteCharacter = it->second;

        if (pRLRemoteCharacter->m_RLCharacter.m_EquipResource.nObject == RL_OBJECT_PLAYER ||
            pRLRemoteCharacter->m_RLCharacter.GetObjectID() == dwTargetID)
        {
            nRetCode = FillRLRemoteCharacterEvaluator(pEvaluator, pRLRemoteCharacter, vLocalCharacterPosition, dwTargetID, TRUE);
            KGLOG_PROCESS_ERROR(nRetCode);

            ++pEvaluator;
            ++nPlayerAndTargetCount;
        }
    }

    pResourceLimit->nRLRemoteCharacterOptimizationEvaluatorCount = nPlayerAndTargetCount;

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

static int KRLRemoteCharacterOptimizationEvaluatorCompare(const void* lhs, const void* rhs) 
{
    KRLRemoteCharacterOptimizationEvaluator* pLhs = (KRLRemoteCharacterOptimizationEvaluator*)lhs;
    KRLRemoteCharacterOptimizationEvaluator* pRhs = (KRLRemoteCharacterOptimizationEvaluator*)rhs;

    // TODO:

    return pLhs->fDistance > pRhs->fDistance ? 1 : -1; 
}

static int EvaluateOptimization(KRLResourceLimit* pResourceLimit)
{
    int nRetCode = false;
    int nResult = false;

    qsort(
        pResourceLimit->pRLRemoteCharacterOptimizationEvaluators, 
        pResourceLimit->nRLRemoteCharacterOptimizationEvaluatorCount, 
        sizeof(KRLRemoteCharacterOptimizationEvaluator),
        KRLRemoteCharacterOptimizationEvaluatorCompare);

    nResult = true;
    return nResult;
}

static int ApplyCharacterResourceOptimization(KRLResourceLimit* pResourceLimit)
{
    int nRetCode = false;
    int nResult = false;
    KRLRemoteCharacterOptimizationEvaluator* pEvaluator = NULL;
    RL_LOAD_OPTIMIZATION nLoadOptimization = RL_MODEL_OPTIMIZATION_LOAD_NONE;
    size_t nCharacterVisibleCount = 0;
    size_t nCount = 0;

    KG_PROCESS_SUCCESS(pResourceLimit->nRLRemoteCharacterOptimizationEvaluatorCount == 0);

    pResourceLimit->nCharacterVisibleCount = 0;

    nCount = min(pResourceLimit->nCharacterMaxVisibleCount, pResourceLimit->nRLRemoteCharacterOptimizationEvaluatorCount);

    for (size_t nIndex = 0; nIndex < nCount; ++nIndex)
    {
        pEvaluator = pResourceLimit->pRLRemoteCharacterOptimizationEvaluators + nIndex;

        nLoadOptimization = pEvaluator->pRLRemoteCharacter->m_RLCharacter.m_EquipResource.nLoadOptimization;

        if (nLoadOptimization == RL_MODEL_OPTIMIZATION_LOAD_NONE)
        {
            if (pEvaluator->bGC)
                continue;

            nRetCode = pEvaluator->pRLRemoteCharacter->UpdateDisplayData(FALSE);
            KGLOG_PROCESS_ERROR(nRetCode);
        }

        ++pResourceLimit->nCharacterVisibleCount;
    }

    for (size_t nIndex = nCount; nIndex < pResourceLimit->nRLRemoteCharacterOptimizationEvaluatorCount; ++nIndex)
    {
        pEvaluator = pResourceLimit->pRLRemoteCharacterOptimizationEvaluators + nIndex;

        nLoadOptimization = pEvaluator->pRLRemoteCharacter->m_RLCharacter.m_EquipResource.nLoadOptimization;

        if (nLoadOptimization != RL_MODEL_OPTIMIZATION_LOAD_NONE)
        {
            if (pEvaluator->bGC)
                continue;

            nRetCode = pEvaluator->pRLRemoteCharacter->UpdateDisplayData(FALSE);
            KGLOG_PROCESS_ERROR(nRetCode);
        }
    }

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

static int UpdateCharacterResourceLimit(KRLResourceLimit* pResourceLimit, KRLScene* pRLScene)
{
    int nRetCode = false;
    int nResult = false;
    D3DXVECTOR3 vLocalCharacterPosition;

    nRetCode = GetLocalCharacterPosition(pRLScene, &vLocalCharacterPosition);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = FillRLRemoteCharacterEvaluators(pResourceLimit, pRLScene, vLocalCharacterPosition);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = EvaluateOptimization(pResourceLimit);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = ApplyCharacterResourceOptimization(pResourceLimit);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int UpdateCharacterResourceLimit(KRLResourceLimit* pResourceLimit)
{
    int nRetCode = false;
    int nResult = false;
    KRLScene* pRLScene = NULL;
    KRLScenePtrArray& RLScenePtrArray = g_pRL->m_SceneMgr.m_apScene;

    StartProfile(&g_pRL->m_Profile, KRLPROFILE_RESOURCE_LIMIT);

    KG_PROCESS_SUCCESS(!pResourceLimit->bEnable);

    for (KRLScenePtrArray::iterator itScene = RLScenePtrArray.begin(); itScene != RLScenePtrArray.end(); ++itScene)
    {
        pRLScene = *itScene;

        nRetCode = UpdateCharacterResourceLimit(pResourceLimit, pRLScene);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

Exit1:
    nResult = true;
Exit0:
    StopProfile(&g_pRL->m_Profile, KRLPROFILE_RESOURCE_LIMIT);
    return nResult;
}
