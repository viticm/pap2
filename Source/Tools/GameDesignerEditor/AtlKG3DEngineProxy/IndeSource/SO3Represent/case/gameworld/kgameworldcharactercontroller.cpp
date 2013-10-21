#include "stdafx.h"
#include "SO3World/IKGSO3WorldClient.h"
#include "./kgameworldcharactercontroller.h"
#include "../../common/kmathtools.h"
#include "../../SO3Represent.h"
#include "SO3World/IKGSO3WorldClient.h"

KGameWorldCharacterController::KGameWorldCharacterController()
: m_pCharacter(NULL)
, m_nLocalX(0)
, m_nLocalY(0)
, m_nLocalZ(0)
, m_nWalk(false)
, m_nTurn(0)
{
}

void KGameWorldCharacterController::Attach(KCharacter* pCharacter)
{
	m_pCharacter = pCharacter;

	UpdateMove();
}

void KGameWorldCharacterController::Turn(int nTurn)
{
    m_nTurn = nTurn;

	UpdateMove();
}

void KGameWorldCharacterController::SetYaw(int nYaw)
{
	if (m_pCharacter == NULL)
		return;

	g_pRL->m_pSO3WorldClient->TurnTo(m_pCharacter, nYaw);

	UpdateMove();
}

void KGameWorldCharacterController::SetPitch(int nPitch)
{
	if (m_pCharacter == NULL)
		return;

	g_pRL->m_pSO3WorldClient->PitchTo(m_pCharacter, nPitch);

	UpdateMove();
}

void KGameWorldCharacterController::Move(int nLocalX, int nLocalY, int nLocalZ)
{
	if (m_pCharacter == NULL)
		return;

    if (m_pCharacter->m_eMoveState == cmsOnSit && (m_nLocalX != 0 || m_nLocalY != 0))
    {
        g_pRL->m_pSO3WorldClient->Stand(m_pCharacter);
    }
    else
    {
        m_nLocalX = nLocalX;
        m_nLocalY = nLocalY;
        m_nLocalZ = nLocalZ;

        UpdateMove();
    }
}

void KGameWorldCharacterController::UpdateMove()
{
	if (m_pCharacter == NULL)
        return;

    int nMove = m_nLocalX != 0 || m_nLocalY != 0;
    int nDirection = g_pRL->m_pSO3WorldClient->GetLogicDirection(m_nLocalX, m_nLocalY) - DIRECTION_COUNT / 4;

    g_pRL->m_pSO3WorldClient->MoveCtrl(nMove, m_nWalk, nDirection, m_nTurn);
}

void KGameWorldCharacterController::Run()
{
    m_nWalk = false;

    UpdateMove();
}

void KGameWorldCharacterController::Walk()
{
    m_nWalk = true;

    UpdateMove();
}

void KGameWorldCharacterController::Jump(int nLocalX, int nLocalY)
{
    BOOL bStandJump = FALSE;
    int nFaceDirection = 0;

    KG_PROCESS_SUCCESS(m_pCharacter == NULL);

    nFaceDirection = m_pCharacter->m_nFaceDirection;

    if (nLocalX == 0 && nLocalY == 0)
        bStandJump = TRUE;
    else if (nLocalX != 0 && nLocalY == 0)
        nFaceDirection = nFaceDirection + nLocalX * -DIRECTION_COUNT / 4;
    else
    {   
        //左前45度跳跃角度1/8,右前45度跳跃角度-1/8,左后和右后的情况转化为+1/2之后的左前和右前的情况.
        if (nLocalY == -1)
            nFaceDirection = nFaceDirection + DIRECTION_COUNT / 2;
        nFaceDirection = nFaceDirection + nLocalX * nLocalY * -DIRECTION_COUNT / 8;
    }
    
	nFaceDirection = NormalizeDirection(nFaceDirection);
    
	g_pRL->m_pSO3WorldClient->Jump(m_pCharacter, bStandJump, nFaceDirection);

Exit1:
	return;
}

int KGameWorldCharacterController::GetMoveInfo(int *pnForward, int *pnStrafeRight, int *pnRotationRight)
{
    int nResult = false;
    
    KGLOG_PROCESS_ERROR(pnForward);
    KGLOG_PROCESS_ERROR(pnStrafeRight);
    KGLOG_PROCESS_ERROR(pnRotationRight);

    *pnForward = m_nLocalY;
    *pnStrafeRight = m_nLocalX;
    *pnRotationRight = m_nTurn;

    nResult = true;
Exit0:
    return nResult;
}

