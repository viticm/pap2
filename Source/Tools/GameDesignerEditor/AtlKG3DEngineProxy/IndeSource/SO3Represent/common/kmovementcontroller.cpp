#include "stdafx.h"
#include "./kmovementcontroller.h"

KMovementController::KMovementController()
{
	Reset();
}

KMovementController::~KMovementController()
{
}

void KMovementController::Reset()
{
	m_fYaw = 0.0f;
	m_fPitch = 0.0f;
	m_fRoll = 0.0f;

	m_bDrag = FALSE;
}

void KMovementController::OnBegin()
{
	m_bDrag = TRUE;
}

void KMovementController::OnMove(D3DXVECTOR3 vDelta)
{
	if (m_bDrag) 
	{ 
        if (vDelta.x != 0.0f)
		    SetYaw(m_fYaw - vDelta.x * (D3DX_PI * 2.0f));
        if (vDelta.y != 0.0f)
		    SetPitch(m_fPitch + vDelta.y * (D3DX_PI * 2.0f));
        if (vDelta.z != 0.0f)
            SetRoll(m_fRoll + vDelta.z * (D3DX_PI * 2.0f));
	}
}

void KMovementController::OnEnd()
{
	m_bDrag = FALSE;
}

BOOL KMovementController::IsBeingDragged() const
{
	return m_bDrag; 
}

void KMovementController::SetYaw(float fYaw)
{
	while (fYaw > 2.0f * D3DX_PI)
		fYaw -= 2.0f * D3DX_PI;
	while (fYaw < 0.0f)
		fYaw += 2.0f * D3DX_PI;

	ASSERT(fYaw >= 0.0f && fYaw <= 2.0f * D3DX_PI);
	m_fYaw = fYaw;
}

void KMovementController::SetPitch(float fPitch)
{
	fPitch = min(fPitch, D3DX_PI / 2.01f);
	fPitch = max(fPitch, -D3DX_PI / 2.01f);
	m_fPitch = fPitch;
}

void KMovementController::SetRoll(float fRoll)
{
	ASSERT(fRoll >= -D3DX_PI && fRoll <= D3DX_PI);
	m_fRoll = fRoll;
}

float KMovementController::GetYaw() const
{
	ASSERT(m_fYaw >= 0.0f && m_fYaw <= 2.0f * D3DX_PI);
	return m_fYaw;
}

float KMovementController::GetPitch() const
{
	ASSERT(m_fPitch >= -D3DX_PI && m_fPitch <= D3DX_PI);
	return m_fPitch;
}

float KMovementController::GetRoll() const
{
	ASSERT(m_fRoll >= -D3DX_PI && m_fRoll <= D3DX_PI);
	return m_fRoll;
}
