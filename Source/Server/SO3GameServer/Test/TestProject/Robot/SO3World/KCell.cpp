#include "stdafx.h"
#include "KMath.h"
#include "KCell.h"

#include "KSO3World.h"
#include "KNpc.h"
#include "KPlayer.h"
#include "KDoodad.h"

BOOL KCell::AddNpc(KNpc* pNpc)
{
	pNpc->m_pCell = this;

	return true;
}

BOOL KCell::AddDoodad(KDoodad* pDoodad)
{
	pDoodad->m_pCell = this;

	pDoodad->ApplyEffect();

	return true;
}

BOOL KCell::AddPlayer(KPlayer* pPlayer)
{
	pPlayer->m_pCell = this;

	return true;
}

BOOL KCell::RemoveNpc(KNpc* pNpc)
{

	pNpc->m_pCell = NULL;

	return true;
}

BOOL KCell::RemoveDoodad(KDoodad* pDoodad)
{
	pDoodad->UnApplyEffect();

	pDoodad->m_pCell = NULL;

	return true;
}

BOOL KCell::RemovePlayer(KPlayer* pPlayer)
{
	pPlayer->m_pCell = NULL;

	return true;
}
