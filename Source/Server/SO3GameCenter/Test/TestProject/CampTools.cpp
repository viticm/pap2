#include "StdAfx.h"
#include "CampTools.h"
#include "Relay_GS_Protocol.h"
#include "KGameServerSimulator.h"
#include "KGameCenterWrapper.h"
#include "Utility.h"
#include "TestDef.h"

bool CampTools::SetCamp(DWORD dwPlayerID, BYTE bCamp)
{
	S2R_APPLY_SET_CAMP_REQUEST Request;
	Request.wProtocolID = s2r_apply_set_camp_request;
	Request.dwPlayerID = dwPlayerID;
	Request.byNewCamp = bCamp;
	m_pGS->Send(&Request, sizeof(Request));
	SCOPEPROTOCOL(m_pGS, r2s_apply_set_camp_respond);
	BufferScopedPrt Buffer(m_pGS->ReceiveDataOnce(r2s_apply_set_camp_respond));
	if (!Buffer.get())
	{
		return false;
	}
	R2S_APPLY_SET_CAMP_RESPOND* pRespond = (R2S_APPLY_SET_CAMP_RESPOND*)Buffer->GetData();
	if ((pRespond->byResult == crtSuccess) &&
		(pRespond->dwPlayerID = dwPlayerID))
	{
		return true;
	}
	return false;
}

int CampTools::AddScore(int nScore)
{
	S2R_ADD_CAMP_SCORE Request;
	Request.wProtocolID = s2r_add_camp_score;
	Request.nAddCampScore = nScore;
	m_pGS->Send(&Request, sizeof(Request));

	SCOPEPROTOCOL(m_pGS, r2s_sync_camp_info);
	BufferScopedPrt Buffer(m_pGS->ReceiveDataOnce(r2s_sync_camp_info));
	if (!Buffer.get())
	{
		return -1;
	}
	R2S_SYNC_CAMP_INFO* pRespond = (R2S_SYNC_CAMP_INFO*)Buffer->GetData();
	return pRespond->nCampScore;
}

void CampTools::GetLevelScore(int* pArray )
{
	memcpy(pArray, GC->Center()->m_CampManager.m_nLevelScore, sizeof(GC->Center()->m_CampManager.m_nLevelScore));
}

int CampTools::GetLevel()
{
	SocpeLock(GC->Mutex());
	return GC->Center()->m_CampManager.m_nLevel;
}

int CampTools::GetScore()
{
	SocpeLock(GC->Mutex());
	return GC->Center()->m_CampManager.m_nScore;
}

void CampTools::SetLevel(int nLevel)
{
	SocpeLock(GC->Mutex());
	GC->Center()->m_CampManager.m_nLevel = nLevel;
}

void CampTools::SetScore(int nScore)
{
	SocpeLock(GC->Mutex());
	 GC->Center()->m_CampManager.m_nScore = nScore;
}