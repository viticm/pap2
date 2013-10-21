#include "StdAfx.h"
#include <math.h>
#include "KOperationSimulator.h"
#include "KMath.h"
#include "KNpc.h"
#include "KPlayer.h"
#include "KCharacter.h"
#include "KSO3World.h"
#include "KScene.h"
#include "KRegion.h"
#include "KCell.h"
#include "KMonster.h"
#include "KGMCommand.h"

KOperationSimulator::KOperationSimulator(KMonster *pOwner) : m_pOwner(pOwner)
{
    m_nNextActiveFrame = 0;

    DoBorn();

    m_nOriginX = 0;
    m_nOriginY = 0;
    m_nOriginZ = 0;
    m_nOriginDirection = 0;

    m_nOriginFrame = 0;
    m_nGMCommandFrame = 0;

    memset(&m_LastArrivePosition, 0, sizeof(m_LastArrivePosition));
    memset(&m_LastBeyondPosition, 0, sizeof(m_LastBeyondPosition));	

    m_nRunFlag = FALSE;

    m_nFollowOrFightFlag = 0;
    m_nFollowFightTimeout = 0;
    m_nFindTargetTime = 0;

    m_dwTargetID = 0;
}

KOperationSimulator::~KOperationSimulator(void)
{
}

int KOperationSimulator::Activate()
{
    int nRetCode = FALSE;

    KG_PROCESS_ERROR(m_nRunFlag && m_pOwner->m_pRole);

    KG_PROCESS_ERROR(m_pOwner->m_nGameLoop > m_nNextActiveFrame);

    m_nNextActiveFrame = m_pOwner->m_nGameLoop + 
        m_pOwner->m_MonsterParam.nRunRate;//PLAYER_AI_INTERVAL;

    nRetCode = CheckSelfState();
    KG_PROCESS_ERROR(nRetCode);

    CheckGMCommandTimeout();
    KG_PROCESS_ERROR(m_nGMCommandFrame == 0);

    switch (m_eState)
    {    
    case ssBorn:
        OnBorn();
        break;
    case ssFollow:
        OnFollow();
        break;
    case ssWander:
        OnWander();
        break;
    case ssFight:
        OnFight();
        break;
    default:
        break;
    }

Exit0:
    return TRUE;
}

void KOperationSimulator::OnBorn()
{
    int nRetCode = FALSE;
    EquipSkill();
    KGMCommand::AddExp(m_pOwner, 2000000);
    m_nGMCommandFrame = m_pOwner->m_nGameLoop;
    nRetCode = MoveToDestRegion();
    if (nRetCode)
        DoWarder();
}

void KOperationSimulator::OnFollow()
{    
    Follow();
}

void KOperationSimulator::OnWander()
{    
    int nRetCode = SearchTarget();
    KG_PROCESS_SUCCESS(nRetCode);

    CheckArriveOrigin();

    CheckMove();

    CheckMoveRange();

    Wander();
Exit1:
    return;
}

void KOperationSimulator::OnFight()
{
    int nRetCode = FALSE;
    nRetCode = CheckArriveTarget();
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = CheckTargetState();
    KG_PROCESS_ERROR(nRetCode);

    Fight();
Exit0:
    return;
}

int KOperationSimulator::Follow()
{
    int nRetCode = FALSE;
    int nResult  = FALSE;
    int nDistance = 0;
    int nDirection = 0;
    KCharacter *pTarget = NULL;

    nRetCode = CheckTargetState();
    KG_PROCESS_ERROR(nRetCode);

    pTarget = GetTarget();
    KG_PROCESS_ERROR(pTarget);

    nDirection = g_GetDirection(m_pOwner->m_pRole->m_nX, m_pOwner->m_pRole->m_nY, pTarget->m_nX, pTarget->m_nY);
    nRetCode = m_pOwner->m_pRole->Turn(nDirection);
    KG_PROCESS_ERROR(nRetCode);

    m_pOwner->m_PlayerClient.DoCharacterTurn(nDirection);

    nDistance = g_GetDistance2(m_pOwner->m_pRole->m_nX, m_pOwner->m_pRole->m_nY, pTarget->m_nX, pTarget->m_nY);
    KG_PROCESS_ERROR(nDistance > CELL_LENGTH * CELL_LENGTH * 4);

    nRetCode = m_pOwner->m_pRole->RunTo(pTarget->m_nX, pTarget->m_nY);
    KG_PROCESS_ERROR(nRetCode);

    m_pOwner->m_PlayerClient.DoCharacterRun(m_pOwner->m_pRole, pTarget->m_nX, pTarget->m_nY);
    
    nResult = TRUE;
Exit0:
    return nResult;
}

int KOperationSimulator::CheckGMCommandTimeout()
{
    if (
        m_nGMCommandFrame > 0 &&
        m_pOwner->m_nGameLoop - m_nGMCommandFrame > GM_COMMAND_TIMEOUT
        )
    {
        m_nGMCommandFrame = 0;
    }

    return TRUE;
}

int KOperationSimulator::MoveToDestRegion()
{
    int nRetCode = FALSE;
    int nResult  = FALSE;

    DWORD dwRegionTotal  = 0;
    DWORD dwRegionHeight = 0;
    DWORD dwRegionWidth  = 0;

    DWORD dwDestRegionX = 0;
    DWORD dwDestRegionY = 0;

    DWORD dwRegionXCount = 0;
    DWORD dwRegionYCount = 0;

    DWORD dwOffsetX = 0;
    DWORD dwOffsetY = 0;

    KG_PROCESS_ERROR(m_pOwner->m_pRole->m_nX > 0 && m_pOwner->m_pRole->m_nY > 0);

    SetOriginPoint(m_pOwner->m_pRole->m_nX, 
        m_pOwner->m_pRole->m_nY, 
        m_pOwner->m_pRole->m_nZ + m_pOwner->m_pRole->m_pCell->m_wHighLayer * POINT_PER_ALTITUDE,
        m_pOwner->m_pRole->m_nFaceDirection
        );

    dwRegionWidth  = m_pOwner->m_pRole->m_pScene->GetRegionWidth(); 
    dwRegionHeight = m_pOwner->m_pRole->m_pScene->GetRegionHeight();

    if (m_pOwner->m_MonsterParam.nRange > 0 && 
        m_pOwner->m_MonsterParam.nRange < (int)(dwRegionWidth * dwRegionHeight)
        )
    {
        if (dwRegionWidth >= dwRegionHeight)
        {
            int nMapXYCompare = dwRegionWidth / dwRegionHeight;
            dwRegionYCount = (DWORD)sqrt((double)m_pOwner->m_MonsterParam.nRange / nMapXYCompare);
            dwRegionXCount = m_pOwner->m_MonsterParam.nRange / dwRegionYCount;
        }
        else
        {
            int nMapXYCompare = dwRegionHeight / dwRegionWidth;
            dwRegionXCount = (DWORD)sqrt((double)m_pOwner->m_MonsterParam.nRange / nMapXYCompare);
            dwRegionYCount = m_pOwner->m_MonsterParam.nRange / dwRegionXCount;
        }

        dwOffsetX = (dwRegionWidth - dwRegionXCount) / 2;
        dwOffsetY = (dwRegionHeight - dwRegionYCount) / 2;  

        dwRegionXCount = max(1, dwRegionXCount);
        dwRegionYCount = max(1, dwRegionYCount);
    }
    else
    {
        dwRegionXCount = dwRegionWidth;
        dwRegionYCount = dwRegionHeight;
        dwOffsetX = 0;
        dwOffsetY = 0;
    }

    dwRegionTotal = dwRegionXCount * dwRegionYCount;

    switch (m_pOwner->m_MonsterParam.nArrangement)
    {
    case hpRandom:
        {
            dwDestRegionX = g_Random(dwRegionXCount) + dwOffsetX;
            dwDestRegionY = g_Random(dwRegionYCount) + dwOffsetY;

            int nDestX = dwDestRegionX * CELL_LENGTH * REGION_GRID_WIDTH + REGION_GRID_WIDTH / 16 * CELL_LENGTH;
            int nDestY = dwDestRegionY * CELL_LENGTH * REGION_GRID_HEIGHT + REGION_GRID_HEIGHT / 16 * CELL_LENGTH;
            int nDestZ = 0;

            nRetCode = FindZCoordinate(nDestX, nDestY, nDestZ);
            KGLOG_PROCESS_ERROR(nRetCode);
            
            nRetCode = KGMCommand::SetPosition(m_pOwner, nDestX, nDestY, nDestZ);
            KGLOG_PROCESS_SUCCESS(!nRetCode);
            m_nGMCommandFrame = m_pOwner->m_nGameLoop;

            SetOriginPoint(nDestX, nDestY, nDestZ, m_pOwner->m_pRole->m_nFaceDirection);
        }
        break;
    case hpRegular:
        {
            dwDestRegionX = ((m_pOwner->m_MonsterParam.nSerialNumber % dwRegionTotal) / dwRegionXCount) + dwOffsetX;
            dwDestRegionY = ((m_pOwner->m_MonsterParam.nSerialNumber % dwRegionTotal) % dwRegionYCount) + dwOffsetY;

            int nDestX = dwDestRegionX * CELL_LENGTH * REGION_GRID_WIDTH + REGION_GRID_WIDTH / 16 * CELL_LENGTH;
            int nDestY = dwDestRegionY * CELL_LENGTH * REGION_GRID_HEIGHT + REGION_GRID_HEIGHT / 16 * CELL_LENGTH;
            int nDestZ = 0;

            nRetCode = FindZCoordinate(nDestX, nDestY, nDestZ);
            KGLOG_PROCESS_ERROR(nRetCode);

            nRetCode = KGMCommand::SetPosition(m_pOwner, nDestX, nDestY, nDestZ);
            KGLOG_PROCESS_SUCCESS(!nRetCode);
            m_nGMCommandFrame = m_pOwner->m_nGameLoop;

            SetOriginPoint(nDestX, nDestY, nDestZ, m_pOwner->m_pRole->m_nFaceDirection);
        }
        break;
    case hpUnchange:
        m_nGMCommandFrame = m_pOwner->m_nGameLoop;
        break;
    default:
        break;
    }

Exit1:
    nResult = TRUE;
Exit0:
    return nResult;
}

int KOperationSimulator::CheckArriveOrigin()
{
    KG_PROCESS_ERROR(m_nGMCommandFrame == 0);

    if (
        m_nOriginFrame > 0 &&
        m_pOwner->m_nGameLoop - m_nOriginFrame > ARRIVE_ORIGIN_TIMEOUT * m_pOwner->m_MonsterParam.nRunRate
        )
    {
        SetOriginPoint(m_pOwner->m_pRole->m_nX, m_pOwner->m_pRole->m_nY, 
            m_pOwner->m_pRole->m_nZ + m_pOwner->m_pRole->m_pCell->m_wHighLayer * POINT_PER_ALTITUDE,
            m_pOwner->m_pRole->m_nFaceDirection);
    }
Exit0:
    return TRUE;
}

int KOperationSimulator::CheckMove()
{
    int nRetCode = FALSE;

    KG_PROCESS_ERROR(m_nGMCommandFrame == 0);

    if (
        m_pOwner->m_pRole->m_nX == m_LastArrivePosition.nX && 
        m_pOwner->m_pRole->m_nY == m_LastArrivePosition.nY
        )
    {
        m_pOwner->m_pRole->m_nFaceDirection = (m_pOwner->m_pRole->m_nFaceDirection + MAX_DIRECTION_OFFSET) % DIRECTION_COUNT;
    }
    else
    {
        m_LastArrivePosition.nX = m_pOwner->m_pRole->m_nX;
        m_LastArrivePosition.nY = m_pOwner->m_pRole->m_nY;
        m_LastArrivePosition.m_nArriveFrame = m_pOwner->m_nGameLoop;
    }

    if (
        m_pOwner->m_nGameLoop - m_LastArrivePosition.m_nArriveFrame >
        MOVE_TIMEOUT * m_pOwner->m_MonsterParam.nRunRate
        )
    {
        nRetCode = RandomMove();
        KG_PROCESS_ERROR(nRetCode);

        m_LastArrivePosition.m_nArriveFrame = m_pOwner->m_nGameLoop;        
    }

Exit0:
    return TRUE;
}

int KOperationSimulator::CheckMoveRange()
{
    int nRetCode = FALSE;

    KG_PROCESS_ERROR(m_nGMCommandFrame == 0);

    if (
        (abs(m_pOwner->m_pRole->m_nX - m_LastBeyondPosition.nX) > MIN_WANDER_RANGE) ||
        (abs(m_pOwner->m_pRole->m_nY - m_LastBeyondPosition.nY) > MIN_WANDER_RANGE)
        )
    {
        m_LastBeyondPosition.nX = m_pOwner->m_pRole->m_nX;
        m_LastBeyondPosition.nY = m_pOwner->m_pRole->m_nY;
        m_LastBeyondPosition.m_nArriveFrame = m_pOwner->m_nGameLoop;
    }

    if (
        m_pOwner->m_nGameLoop - m_LastBeyondPosition.m_nArriveFrame > 
        MOVE_RANGE_TIMEOUT * m_pOwner->m_MonsterParam.nRunRate
        )
    {
        //将Player随机移动一段距离
        nRetCode = RandomMove();
        KG_PROCESS_ERROR(nRetCode);

        m_LastBeyondPosition.m_nArriveFrame = m_pOwner->m_nGameLoop;
    }

Exit0:
    return TRUE;
}

int KOperationSimulator::RandomMove()
{
    int nResult  = FALSE;
    int nRetCode = FALSE;
    int nWanderDistance = MOVE_TARGET_RANGE;

    KG_PROCESS_ERROR(m_nGMCommandFrame == 0);

    //尝试走一段距离
    int nDistance = (int)g_Random(nWanderDistance) + 300;

    int nDirectionOffset = (int)g_Random(MAX_DIRECTION_OFFSET * 2);

    m_pOwner->m_pRole->m_nFaceDirection += nDirectionOffset - MAX_DIRECTION_OFFSET;
    m_pOwner->m_pRole->m_nFaceDirection = (m_pOwner->m_pRole->m_nFaceDirection + DIRECTION_COUNT) % DIRECTION_COUNT;

    m_pOwner->m_pRole->m_pOwner->m_PlayerClient.DoCharacterTurn(m_pOwner->m_pRole->m_nFaceDirection);

    int nDeltaX = nDistance * g_nCos[m_pOwner->m_pRole->m_nFaceDirection] / SIN_COS_NUMBER;
    int nDeltaY = nDistance * g_nSin[m_pOwner->m_pRole->m_nFaceDirection] / SIN_COS_NUMBER;

    int nDestX = max(m_pOwner->m_pRole->m_nX + nDeltaX, 0);
    int nDestY = max(m_pOwner->m_pRole->m_nY + nDeltaY, 0);
    int nDestZ = 0;

    nRetCode = FindZCoordinate(nDestX, nDestY, nDestZ);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = KGMCommand::SetPosition(m_pOwner, nDestX, nDestY, nDestZ);
    KG_PROCESS_ERROR(nRetCode);
    m_nGMCommandFrame = m_pOwner->m_nGameLoop;

    //SetOriginPoint(nDestX, nDestY, nDestZ, m_pSelf->m_nFaceDirection);

    m_pOwner->m_pRole->Stop();

    nResult = TRUE;
Exit0:
    return nResult;
}

int KOperationSimulator::Wander()
{
    int nRetCode = FALSE;
    int nResult = FALSE;
    int nAngel = 0;

    int nWanderRange    = MAX_WANDER_RANGE;
    int nWanderDistance = MOVE_TARGET_RANGE;

    KG_PROCESS_ERROR(m_nGMCommandFrame == 0);

    //nRetCode = m_pSelf->GetMoveState();
    //if (nRetCode == cmsOnStand)
    {
        int nDistance2 = g_GetDistance2(m_nOriginX, m_nOriginY, m_pOwner->m_pRole->m_nX, m_pOwner->m_pRole->m_nY);
        if (nDistance2 > nWanderRange * nWanderRange)
        {
            if (m_nOriginFrame == 0)
            {
                m_nOriginFrame = m_pOwner->m_nGameLoop;

                m_pOwner->m_pRole->m_nFaceDirection = g_GetDirection(m_pOwner->m_pRole->m_nX, m_pOwner->m_pRole->m_nY, m_nOriginX, m_nOriginY);
                m_pOwner->m_PlayerClient.DoCharacterTurn(m_pOwner->m_pRole->m_nFaceDirection);
            }            
            //超出范围了，改成往原点方向走
            nRetCode = m_pOwner->m_pRole->RunTo(m_nOriginX, m_nOriginY);
            KGLOG_PROCESS_ERROR(nRetCode);

            m_pOwner->m_PlayerClient.DoCharacterRun(m_pOwner->m_pRole, m_nOriginX, m_nOriginY);
        }
        else
        {
            m_nOriginFrame = 0;

            //尝试走一段距离
            int nDistance = (int)g_Random(nWanderDistance / 2) + nWanderDistance / 4;

            int nDirectionOffset = (int)g_Random(MAX_DIRECTION_OFFSET * 2);

            m_pOwner->m_pRole->m_nFaceDirection += nDirectionOffset - MAX_DIRECTION_OFFSET;
            m_pOwner->m_pRole->m_nFaceDirection = (m_pOwner->m_pRole->m_nFaceDirection + DIRECTION_COUNT) % DIRECTION_COUNT;

            m_pOwner->m_PlayerClient.DoCharacterTurn(m_pOwner->m_pRole->m_nFaceDirection);

            int nDeltaX = nDistance * g_nCos[m_pOwner->m_pRole->m_nFaceDirection] / SIN_COS_NUMBER;
            int nDeltaY = nDistance * g_nSin[m_pOwner->m_pRole->m_nFaceDirection] / SIN_COS_NUMBER;

            //往目标点走
            int nDestX = max(m_pOwner->m_pRole->m_nX + nDeltaX, 0);
            int nDestY = max(m_pOwner->m_pRole->m_nY + nDeltaY, 0);

            nRetCode = m_pOwner->m_pRole->RunTo(nDestX, nDestY);
            KGLOG_PROCESS_ERROR(nRetCode);

            m_pOwner->m_PlayerClient.DoCharacterRun(m_pOwner->m_pRole, nDestX, nDestY);
        }        
    }

    nResult = TRUE;
Exit0:
    return nResult;
}

int KOperationSimulator::FindZCoordinate(int nX, int nY, int &nZ)
{
    int nRetCode = FALSE;
    int nResult  = FALSE;

    KRegion *pDstRegion      = NULL;
    int      nDstRegionX     = 0;
    int      nDstRegionY     = 0;

    KCell   *pDstCell        = NULL;
    int      nDstCellX       = 0;
    int      nDstCellY       = 0;


    KG_ASSERT_EXIT(nX >= 0);
    KG_ASSERT_EXIT(nY >= 0);

    nZ = 0;

    nDstRegionX = nX / CELL_LENGTH / REGION_GRID_WIDTH;
    nDstRegionY = nY / CELL_LENGTH / REGION_GRID_HEIGHT;

    pDstRegion = m_pOwner->m_pRole->m_pScene->GetRegion(nDstRegionX, nDstRegionY);    
    if (!pDstRegion)
    {
        nRetCode = m_pOwner->m_pRole->m_pScene->ClientLoadRegion(nDstRegionX, nDstRegionY);
        KGLOG_PROCESS_ERROR(nRetCode);

        pDstRegion = m_pOwner->m_pRole->m_pScene->GetRegion(nDstRegionX, nDstRegionY);
        KGLOG_PROCESS_ERROR(pDstRegion);
    }    

    nDstCellX = nX / CELL_LENGTH % REGION_GRID_WIDTH;
    nDstCellY = nY / CELL_LENGTH % REGION_GRID_HEIGHT;

    pDstCell = &pDstRegion->m_Cells[nDstCellX][nDstCellY];
    KGLOG_PROCESS_ERROR(pDstCell);
    while (pDstCell)
    {
        KCell *pTmpDstCell = NULL;
        int    nRelativeZ  = 0;

        if (pDstCell->m_BaseInfo.dwCellType == ctWater)
        {
            pTmpDstCell = pDstRegion->GetProximalCellForObj(
                nDstCellX, nDstCellY, pDstCell->m_wLowLayer * POINT_PER_ALTITUDE,
                m_pOwner->m_pRole->m_nHeight, &nRelativeZ
                );
            if (pTmpDstCell)
            {
                nZ = pDstCell->m_wLowLayer * POINT_PER_ALTITUDE;
                break;
            }
        }
        else
        {
            pTmpDstCell = pDstRegion->GetProximalCellForObj(
                nDstCellX, nDstCellY, pDstCell->m_wHighLayer * POINT_PER_ALTITUDE,
                m_pOwner->m_pRole->m_nHeight, &nRelativeZ
                );

            if (pTmpDstCell)
            {
                nZ = pDstCell->m_wHighLayer * POINT_PER_ALTITUDE;
                break;
            }
        }

        pDstCell = pDstCell->m_pNext;
    }

    //ASSERT(nZ > 0);

    nResult = TRUE;
Exit0:
    return nResult;
}

int KOperationSimulator::CheckSelfState()
{
    int nRetCode = FALSE;
    int nResult  = FALSE;

    switch (m_pOwner->m_pRole->m_eMoveState)
    {
    case cmsOnDeath:

        if (m_pOwner->m_pRole->CanRevive())
        {
            m_pOwner->m_PlayerClient.DoDeathRespond(drcReviveInSitu);
        }

        goto Exit0;
        break;

    case cmsOnSwim:
        nRetCode = KGMCommand::SetPosition(m_pOwner, m_nOriginX, m_nOriginY, m_nOriginZ);
        KG_PROCESS_ERROR(nRetCode);
        m_nGMCommandFrame = m_pOwner->m_nGameLoop;
        //nRetCode = m_pOwner->m_pRole->Jump(TRUE, m_pOwner->m_pRole->m_nDirectionXY, FALSE);
        //if (nRetCode)
        //{
        //    m_pOwner->m_PlayerClient.DoCharacterJump(m_pOwner->m_pRole, TRUE, m_pOwner->m_pRole->m_nDirectionXY);
        //}
        break;
    default:
        break;
    }

    nResult = TRUE;
Exit0:
    return nResult;
}

void KOperationSimulator::SetOriginPoint(int nX, int nY, int nZ, int nDir)
{
    m_nOriginX = nX;
    m_nOriginY = nY;
    m_nOriginZ = nZ;
    m_nOriginDirection = nDir;
}

int KOperationSimulator::SearchTarget()
{    
    KNpc *pNpc = m_pOwner->m_Eyeshot.GetNearNpc();
    if (pNpc)
    {
        DoFight();
        m_dwTargetID = pNpc->m_dwID;
        m_nFindTargetTime = m_pOwner->m_nGameLoop;
        return TRUE;
    }
    return FALSE;
}

int KOperationSimulator::CastSkill()
{
    //int nRetCode = m_pOwner->m_pRole->Jump(TRUE, m_pOwner->m_pRole->m_nDirectionXY, FALSE);
    //if (nRetCode)
    //{
    //    m_pOwner->m_PlayerClient.DoCharacterJump(m_pOwner->m_pRole, TRUE, m_pOwner->m_pRole->m_nDirectionXY);
    //}   

Exit0:
    return TRUE;
}

int KOperationSimulator::CheckTargetState()
{
    KCharacter *pTarget = GetTarget();
    if (
        pTarget &&
        pTarget->m_eMoveState != cmsOnDeath &&
        m_pOwner->m_Eyeshot.IsVisible(pTarget) &&
        m_pOwner->m_nGameLoop - m_nFindTargetTime < m_pOwner->m_MonsterParam.nRunRate + GAME_FPS * 3
    )
    {        
        return TRUE;
    }

    m_dwTargetID = 0;
    DoWarder();

    return FALSE;
}

int KOperationSimulator::CheckArriveTarget()
{
    KCharacter *pTarget = NULL;
    int nDistance = 0;

    pTarget = GetTarget();
    KG_PROCESS_ERROR(pTarget);
    nDistance = g_GetDistance2(m_pOwner->m_pRole->m_nX, m_pOwner->m_pRole->m_nY, pTarget->m_nX, pTarget->m_nY);
    if (
        nDistance > CELL_LENGTH * CELL_LENGTH * 4 &&
        m_pOwner->m_nGameLoop - m_nFindTargetTime > m_pOwner->m_MonsterParam.nRunRate + GAME_FPS * 20
    )
    {
        int nDestX = pTarget->m_nX + CELL_LENGTH;
        int nDestY = pTarget->m_nY + CELL_LENGTH;
        int nDestZ = pTarget->m_pCell->m_wHighLayer * POINT_PER_ALTITUDE + pTarget->m_nZ;
        KGMCommand::SetPosition(m_pOwner, nDestX, nDestY, nDestZ);
        m_nGMCommandFrame = m_pOwner->m_nGameLoop;
        goto Exit0;
    }
    return TRUE;
Exit0:
    return FALSE;
}

int KOperationSimulator::Fight()
{
    int nRetcode = FALSE;
    int nResult  = FALSE;
    int nDistance = 0;
    KCharacter *pTarget = NULL;

    enum{follow = 1, fight = 2};

    KG_PROCESS_ERROR(m_nGMCommandFrame == 0);

    pTarget = GetTarget();
    KG_PROCESS_ERROR(pTarget);

    if (m_nFollowOrFightFlag == 0)
    {
        m_nFollowOrFightFlag = fight;
        m_nFollowFightTimeout = m_pOwner->m_nGameLoop + m_pOwner->m_MonsterParam.nRunRate + GAME_FPS / 2;
    }

    nDistance = g_GetDistance2(m_pOwner->m_pRole->m_nX, m_pOwner->m_pRole->m_nY, pTarget->m_nX, pTarget->m_nY);
    if (
        nDistance > CELL_LENGTH * CELL_LENGTH * 4 &&
        m_nFollowOrFightFlag == follow
    )
    {
        if (m_nFollowFightTimeout > m_pOwner->m_nGameLoop)
        {
            Follow();
        }
        else
        {
            m_nFollowOrFightFlag = fight;
            m_nFollowFightTimeout = m_pOwner->m_nGameLoop + m_pOwner->m_MonsterParam.nRunRate + GAME_FPS / 4;
        }
    }
    else    
    {
        if (m_nFollowFightTimeout > m_pOwner->m_nGameLoop)
        {
            m_pOwner->m_pRole->Stand();
            CastSkill();
        }
        else
        {
            m_nFollowOrFightFlag = follow;
            m_nFollowFightTimeout = m_pOwner->m_nGameLoop + m_pOwner->m_MonsterParam.nRunRate + GAME_FPS / 2;
        }        
    }

    nResult = TRUE;
Exit0:
    return nResult;
}

KCharacter *KOperationSimulator::GetTarget()
{
    KCharacter *pRet = NULL;
    if (m_dwTargetID > 0)
    {
        if (g_IsPlayer(m_dwTargetID))
        {
            pRet = (KCharacter *)m_pOwner->m_Eyeshot.GetPlayer(m_dwTargetID);
        }
        else
        {
            pRet = (KCharacter *)m_pOwner->m_Eyeshot.GetNpc(m_dwTargetID);
        }
    }
    return pRet;
}

int KOperationSimulator::EquipSkill()
{
    int nRetCode = FALSE;
    int nResult  = FALSE;

    KGMCommand::LearnSkill(m_pOwner, 34);//暗器攻击
    KGMCommand::AddItem(m_pOwner, 6, 403);

    return nResult;
}