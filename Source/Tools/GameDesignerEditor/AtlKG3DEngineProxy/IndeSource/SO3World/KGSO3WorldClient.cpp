#include "stdafx.h"
#ifdef _CLIENT
#include <conio.h>
#include "KGSO3WorldClient.h"
#include "KSceneObject.h"
#include "KCharacter.h"
#include "KPlayer.h"
#include "KNpc.h"
#include "KDoodad.h"
#include "KPlayerClient.h"
#include "KSO3World.h"
#include "KLootList.h"

BOOL KGSO3WorldClientInterface::MoveCharacter(KCharacter* pCharacter, int nX, int nY, int nZ)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    bRetCode = pCharacter->MoveTo(nX, nY, nZ, false, false);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGSO3WorldClientInterface::MoveCharacter(KPlayer* pPlayer, int nX, int nY, int nZ)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    bRetCode = pPlayer->MoveTo(nX, nY, nZ, false, false);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGSO3WorldClientInterface::MoveCharacter(KNpc* pNpc, int nX, int nY, int nZ)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    bRetCode = pNpc->MoveTo(nX, nY, nZ, false, false);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGSO3WorldClientInterface::MoveCharacter(DWORD dwCharacterID, int nX, int nY, int nZ)
{
    BOOL        bResult    = false;
    BOOL        bRetCode   = false;
    KCharacter* pCharacter = NULL;

    assert(dwCharacterID);

    if (IS_PLAYER(dwCharacterID))
    {
        pCharacter = g_pSO3World->m_PlayerSet.GetObj(dwCharacterID);
    }
    else
    {
        pCharacter = g_pSO3World->m_NpcSet.GetObj(dwCharacterID);
    }
    KG_PROCESS_ERROR(pCharacter);

    bRetCode = pCharacter->MoveTo(nX, nY, nZ, false, false);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGSO3WorldClientInterface::GetAbsoluteCoordinate(KSceneObject* pSceneObject, int* pnX, int* pnY, int* pnZ)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    bRetCode = pSceneObject->GetAbsoluteCoordinate(pnX, pnY, pnZ);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGSO3WorldClientInterface::GetAbsoluteCoordinate(KCharacter* pCharacter, int* pnX, int* pnY, int* pnZ)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    bRetCode = pCharacter->GetAbsoluteCoordinate(pnX, pnY, pnZ);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGSO3WorldClientInterface::GetAbsoluteCoordinate(KPlayer* pPlayer, int* pnX, int* pnY, int* pnZ)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    bRetCode = pPlayer->GetAbsoluteCoordinate(pnX, pnY, pnZ);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGSO3WorldClientInterface::GetAbsoluteCoordinate(KNpc* pNpc, int* pnX, int* pnY, int* pnZ)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    bRetCode = pNpc->GetAbsoluteCoordinate(pnX, pnY, pnZ);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGSO3WorldClientInterface::GetAbsoluteCoordinate(KDoodad* pDoodad, int* pnX, int* pnY, int* pnZ)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    bRetCode = pDoodad->GetAbsoluteCoordinate(pnX, pnY, pnZ);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGSO3WorldClientInterface::GetAbsoluteCoordinateByCharacterID(DWORD dwID, int* pnX, int* pnY, int* pnZ)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    KCharacter* pCharacter  = NULL;

    if (IS_PLAYER(dwID))
    {
        pCharacter = g_pSO3World->m_PlayerSet.GetObj(dwID);
    }
    else
    {
        pCharacter = g_pSO3World->m_NpcSet.GetObj(dwID);
    }
    KG_PROCESS_ERROR(pCharacter);

    bRetCode = pCharacter->GetAbsoluteCoordinate(pnX, pnY, pnZ);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;    
}

BOOL KGSO3WorldClientInterface::GetAbsoluteCoordinateByDoodadID(DWORD dwID, int* pnX, int* pnY, int* pnZ)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    KDoodad*    pDoodad     = NULL;

    pDoodad = g_pSO3World->m_DoodadSet.GetObj(dwID);
    KG_PROCESS_ERROR(pDoodad);

    bRetCode = pDoodad->GetAbsoluteCoordinate(pnX, pnY, pnZ);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}



BOOL KGSO3WorldClientInterface::AddNpc(KScene* pScene, const KNPC_DATA& crNpcData)
{
    BOOL        bResult         = false;
    BOOL        bRetCode        = false;    
    KNpc*       pNpc            = NULL;
    KScene*     pCurrentScene   = NULL;

    pNpc = g_pSO3World->NewNpc();
    KGLOG_PROCESS_ERROR(pNpc);

    bRetCode = pNpc->Load(crNpcData);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = g_pSO3World->AddNpc(pNpc, pScene, crNpcData.nX, crNpcData.nY, crNpcData.nZ);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    if (!bResult)
    {
        if (pNpc)
        {
            g_pSO3World->DeleteNpc(pNpc);
            pNpc = NULL;
        }
    }
    return bResult;
}

BOOL KGSO3WorldClientInterface::DelNpc(KNpc* pNpc)
{
    BOOL        bResult         = false;
    BOOL        bRetCode        = false;

    bRetCode = g_pSO3World->DeleteNpc(pNpc);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}


KNpc* KGSO3WorldClientInterface::GetNpc(DWORD dwNpcID)
{
	return g_pSO3World->m_NpcSet.GetObj(dwNpcID);
}

BOOL KGSO3WorldClientInterface::SetDoodadScript(KDoodad* pDoodad, DWORD dwScriptID)
{
    pDoodad->m_dwScriptID = dwScriptID;
    return true;
}

BOOL KGSO3WorldClientInterface::GetDoodadScript(KDoodad *pDoodad, DWORD *pdwRetScriptID)
{
    *pdwRetScriptID = pDoodad->m_dwScriptID;
    return true;
}

KDoodad* KGSO3WorldClientInterface::GetDoodad(DWORD dwDoodadID)
{
	return g_pSO3World->m_DoodadSet.GetObj(dwDoodadID);
}

KPlayer* KGSO3WorldClientInterface::GetPlayer(DWORD dwPlayerID)
{
	return g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
}

void KGSO3WorldClientInterface::MoveCtrl(BOOL bMove, BOOL bWalk, int nDirection, int nTurn)
{
    KPlayer* pPlayer = NULL;
    BOOL     bChange = false;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    nDirection = nDirection % DIRECTION_COUNT;
    if (nDirection < 0)
    {
        nDirection += DIRECTION_COUNT;
    }

    nDirection /= (DIRECTION_COUNT / 8);

    if (nTurn < 0)
    {
        nTurn = -1;
    }

    if (nTurn > 0)
    {
        nTurn = 1;
    }

    bChange |= (bMove != pPlayer->m_MoveCtrl.bMove);
    bChange |= (bWalk != pPlayer->m_MoveCtrl.bWalk);
    bChange |= (nDirection != pPlayer->m_MoveCtrl.nDirection8);
    bChange |= (nTurn != pPlayer->m_MoveCtrl.nTurn);

    KG_PROCESS_ERROR(bChange);

    pPlayer->m_MoveCtrl.bMove       = bMove;
    pPlayer->m_MoveCtrl.bWalk       = bWalk;
    pPlayer->m_MoveCtrl.nDirection8 = nDirection;
    pPlayer->m_MoveCtrl.nTurn       = nTurn;

    g_PlayerClient.DoMoveCtrl(pPlayer, bMove, bWalk, nDirection, nTurn);

Exit0:
    return;
}

void KGSO3WorldClientInterface::Stop(KCharacter* pCharacter)
{
}

void KGSO3WorldClientInterface::Stand(KCharacter* pCharacter)
{
	assert(pCharacter);
	pCharacter->HostStand();
}

void KGSO3WorldClientInterface::RunTo(KCharacter* pCharacter, int nDestX, int nDestY)
{
}

void KGSO3WorldClientInterface::WalkTo(KCharacter* pCharacter, int nDestX, int nDestY)
{
}

void KGSO3WorldClientInterface::SwimTo(KCharacter* pCharacter, int nDestX, int nDestY, int nDestZ)
{
}

void KGSO3WorldClientInterface::Jump(KCharacter* pCharacter, BOOL bStandJump, int nDirectionXY)
{
	assert(pCharacter);

    if (pCharacter->m_eMoveState == cmsOnSit)
    {
        pCharacter->HostStand();
    }
    else
    {
	    pCharacter->HostJump(bStandJump, nDirectionXY);
    }
}

void KGSO3WorldClientInterface::TurnTo(KCharacter* pCharacter, int nNewDirection)
{
	assert(pCharacter);
	pCharacter->HostTurnTo(nNewDirection);
}

void KGSO3WorldClientInterface::PitchTo(KCharacter* pCharacter, int nNewDirection)
{
	assert(pCharacter);
	pCharacter->HostPitchTo(nNewDirection);
}


BOOL KGSO3WorldClientInterface::SetTarget(KTargetData* pTarget, TARGET_TYPE eType, DWORD dwID)
{
    BOOL    bResult    = false;
    BOOL    bRetCode   = false;
    KTarget Target;

    assert(pTarget);

    bRetCode = Target.SetTarget(eType, dwID);
    KG_PROCESS_ERROR(bRetCode);

    Target.GetTargetData(*pTarget);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGSO3WorldClientInterface::SetTarget(KTargetData* pTarget, KCharacter* pCharacter)
{
    BOOL    bResult    = false;
    BOOL    bRetCode   = false;
    KTarget Target;

    assert(pTarget);
    assert(pCharacter);

    bRetCode = Target.SetTarget(pCharacter);
    KG_PROCESS_ERROR(bRetCode);

    Target.GetTargetData(*pTarget);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGSO3WorldClientInterface::SetTarget(KTargetData* pTarget, KPlayer* pPlayer)
{
    BOOL    bResult    = false;
    BOOL    bRetCode   = false;
    KTarget Target;

    assert(pTarget);
    assert(pPlayer);

    bRetCode = Target.SetTarget(pPlayer);
    KG_PROCESS_ERROR(bRetCode);

    Target.GetTargetData(*pTarget);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGSO3WorldClientInterface::SetTarget(KTargetData* pTarget, KNpc* pNpc)
{
    BOOL    bResult    = false;
    BOOL    bRetCode   = false;
    KTarget Target;

    assert(pTarget);
    assert(pNpc);

    bRetCode = Target.SetTarget(pNpc);
    KG_PROCESS_ERROR(bRetCode);

    Target.GetTargetData(*pTarget);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGSO3WorldClientInterface::GetTarget(KTargetData* pTarget, KSceneObject** ppSceneObject)
{
    BOOL    bResult    = false;
    BOOL    bRetCode   = false;
    KTarget Target;

    assert(pTarget);
    assert(ppSceneObject);

    Target.SetTargetData(*pTarget);

	bRetCode = Target.GetTarget(ppSceneObject);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGSO3WorldClientInterface::GetTarget(KTargetData* pTarget, KCharacter** ppCharacter)
{
    BOOL    bResult    = false;
    BOOL    bRetCode   = false;
    KTarget Target;

    assert(pTarget);
    assert(ppCharacter);

    Target.SetTargetData(*pTarget);

    bRetCode = Target.GetTarget(ppCharacter);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGSO3WorldClientInterface::GetTarget(KTargetData* pTarget, int* pnX, int* pnY, int* pnZ)
{
    BOOL    bResult    = false;
    BOOL    bRetCode   = false;
    KTarget Target;

    assert(pTarget);

    Target.SetTargetData(*pTarget);

    bRetCode = Target.GetTarget(pnX, pnY, pnZ);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

DWORD KGSO3WorldClientInterface::GetPlayerEquipRepresent(KPlayer* pPlayer, int nEquipRepresentIndex)
{
    assert(nEquipRepresentIndex >= 0 && nEquipRepresentIndex < perRepresentCount);
    
    return pPlayer->m_wRepresentId[nEquipRepresentIndex];
}

BOOL KGSO3WorldClientInterface::GetCharacterMagicRepresent(KCharacter* pCharacter, KCHARACTER_MAGIC_REPRESENT* pMagicRepresent)
{
    return false;
}

BOOL KGSO3WorldClientInterface::IsClientLootListVisible(KDoodad* pDoodad)
{
	KPlayer* pPlayer = NULL;

    KG_PROCESS_ERROR(pDoodad);
    KG_PROCESS_ERROR(pDoodad->m_pLootList);

	pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KG_PROCESS_ERROR(pPlayer);

    return pDoodad->m_pLootList->IsVisible(pPlayer, pDoodad);
Exit0:
	return false;
}

BOOL KGSO3WorldClientInterface::IsClientQuestEffectVisible(KDoodad* pDoodad)
{
    BOOL     bResult    = false;
    BOOL     bRetCode   = false;
    KPlayer* pPlayer    = NULL;

    KG_PROCESS_ERROR(pDoodad);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KG_PROCESS_ERROR(pPlayer);

    KG_PROCESS_ERROR(pDoodad->m_eKind == dkQuest);

    bRetCode = pDoodad->HaveQuest(pPlayer);
    KG_PROCESS_ERROR(bRetCode);

    //return 查询Doodad是否是当前玩家的任务Doodad
    bResult = true;
Exit0:
    return bResult;
}

void KGSO3WorldClientInterface::StopFollow(KCharacter* pCharacter)
{
    if (pCharacter && IS_PLAYER(pCharacter->m_dwID))
    {
	    ((KPlayer*)pCharacter)->HostStopFollow();
    }
}

KScene* KGSO3WorldClientInterface::GetScene(DWORD dwSceneID)
{
	return g_pSO3World->m_SceneSet.GetObj(dwSceneID);
}

KSkill* KGSO3WorldClientInterface::GetSkill(DWORD dwSkillID, DWORD dwSkillLevel)
{
    return g_pSO3World->m_SkillManager.GetSkill_RAW(dwSkillID, dwSkillLevel);
}

KMapParams* KGSO3WorldClientInterface::GetMapParams(DWORD dwMapID)
{
	return g_pSO3World->m_Settings.m_MapListFile.GetMapParamByID(dwMapID);
}

KScene* KGSO3WorldClientInterface::GetClientScene()
{
	return g_pSO3World->GetScene(g_pSO3World->m_dwClientMapID, g_pSO3World->m_nClientMapCopyIndex);
}

int KGSO3WorldClientInterface::GetLogicDirection(int nX, int nY)
{
	return g_GetDirection(0, 0, nX, nY);
}

int KGSO3WorldClientInterface::GetSubjectiveDirection(KCharacter* pCharacter)
{
	assert(pCharacter);
	return pCharacter->GetSubjectiveDirection();
}

int KGSO3WorldClientInterface::GetRelation(KCharacter* pSrcCharacter, KCharacter* pDstCharacter)
{
    assert(pSrcCharacter);
    assert(pDstCharacter);
    return g_GetRelation(pSrcCharacter, pDstCharacter);
}

int KGSO3WorldClientInterface::GetSubjectiveVelocity(KCharacter* pCharacter)
{
	assert(pCharacter);
	return pCharacter->GetSubjectiveVelocity();
}

BOOL KGSO3WorldClientInterface::GetRepresentState(KCharacter* pCharacter, KBUFF_REPRESENT_POS nPos, DWORD& dwRepresentID)
{
	assert(pCharacter);
	return pCharacter->GetRepresentState(nPos, dwRepresentID);
}

BOOL KGSO3WorldClientInterface::TraversePlayerSet(KPlayerTraverser PlayerTraverser)
{
	g_pSO3World->m_PlayerSet.Traverse(PlayerTraverser);

    return true;
}

BOOL KGSO3WorldClientInterface::TraverseNpcSet(KNpcTraverser NpcTraverser)
{
	g_pSO3World->m_NpcSet.Traverse(NpcTraverser);
    return true;
}

BOOL KGSO3WorldClientInterface::TraverseDoodadSet(KDoodadTraverser DoodadTraverser)
{
    g_pSO3World->m_DoodadSet.Traverse(DoodadTraverser);
    return true;
}

int KGSO3WorldClientInterface::GetWaterline(KCharacter* pCharacter)
{
	assert(pCharacter);
	return pCharacter->GetWaterline();
}

KCHARACTER_TITLE_MARK KGSO3WorldClientInterface::GetCharacterTitleMark(KCharacter* pCharacter)
{
    KCHARACTER_TITLE_MARK Result;

    Result.eMarkType        = tmtNoMark;
    Result.nPartyMarkType   = 0;

    return Result;
}

time_t KGSO3WorldClientInterface::GetWorldTime()
{
    // 取游戏世界里面的时间,暂时用真实时间代替
    return ::time(NULL);
}

int KGSO3WorldClientInterface::BeginForwardCharacterPosition(KCharacter* pCharacter, KPOSITION_RECORD* pPositionRecord)
{
    int nResult = false;
    return nResult;
}

int KGSO3WorldClientInterface::EndForwardCharacterPosition(KCharacter* pCharacter, KPOSITION_RECORD* pPositionRecord)
{
    int nRetCode = false;
    int nResult = false;
    return nResult;
}

int KGSO3WorldClientInterface::ForwardCharacterPosition(KCharacter* pCharacter, int nFrame)
{
    int nRetCode = false;
    int nResult = false;

    KGLOG_PROCESS_ERROR(pCharacter);

	while (nFrame-- > 0)
	{
        pCharacter->RunStateMachine();
        KG_PROCESS_ERROR(pCharacter->m_pScene);

        pCharacter->ProcessMove();        
        KG_PROCESS_ERROR(pCharacter->m_pScene);

        if (IS_PLAYER(pCharacter->m_dwID))
        {
            ((KPlayer*)pCharacter)->MoveCtrl();
        }
	}

    nResult = true;
Exit0:
    return nResult;
}

IKGSO3WorldClient* QuerySO3WorldClientInterface()
{
    static KGSO3WorldClientInterface g_SO3WorldClientInterface;
    return &g_SO3WorldClientInterface;
}

#endif
