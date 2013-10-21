#include "stdafx.h"
#include ".\kphysicsforcefield.h"

KPhysicsForceField::KPhysicsForceField(void)
{
	m_dwType = 0;
}

KPhysicsForceField::~KPhysicsForceField(void)
{
}

HRESULT KPhysicsForceField::GetForce(D3DXVECTOR3* pForce,D3DXVECTOR3* pPosition)
{
	pForce->x = 0;
	pForce->y = 0;
	pForce->z = 0;

	return S_OK;
}

KPhysicsForceFieldManager::KPhysicsForceFieldManager(void)
{
}

KPhysicsForceFieldManager::~KPhysicsForceFieldManager(void)
{
}

HRESULT KPhysicsForceFieldManager::GetForce(D3DXVECTOR3* pForce,D3DXVECTOR3* pPosition)
{
	pForce->x = 0;
	pForce->y = 0;
	pForce->z = 0;

	return S_OK;
}