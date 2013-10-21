#include "stdafx.h"
#include "KMath.h"
#include "SO3GlobalDef.h"
#include "KSceneObject.h"
#include "KScene.h"
#include "KRegion.h"
#include "KCharacter.h"
#include "KPlayer.h"
#include "KNpc.h"
#include "KAISearchTactics.h"
#include "KDoodad.h"

#define MAX_ALARM_RANGE_REVISE 1000

bool g_CompByDistance(const KSearchNode& left, const KSearchNode& right)
{
	return left.m_nDistance2 < right.m_nDistance2;
}

int g_GetLevelByTargetLife(int nLifeConversionLevel, double fPrecentLife)
{
    int nDeltaLevel = 0;

    if (fPrecentLife < 0.5)
        nDeltaLevel += nLifeConversionLevel;
    if (fPrecentLife < 0.1)
        nDeltaLevel += nLifeConversionLevel;

    return nDeltaLevel;
}

double g_GetRangeByLevelDiff(int nLevelDiff)
{
    double fRangePercent = 1.0;

    if (nLevelDiff > 8)
        nLevelDiff = 8;
    else if (nLevelDiff < -8)
        nLevelDiff = -8;

    fRangePercent -= 0.1 * (8 - nLevelDiff) / 2;

Exit0:
    return fRangePercent;
}

BOOL KSearchForAnyDoodad::operator ()(KDoodad* pDoodad)
{
	BOOL	bRetCode = false;
	int     nSrcZ       = 0;
	int     nDstZ       = 0;
	int		nDistance2	= 0;

	if (m_dwDoodadTemplateID && m_dwDoodadTemplateID != pDoodad->m_dwTemplateID)
		return true;

	bRetCode = g_InRange(m_pSelf, pDoodad, m_nDistance);
	if (bRetCode)
	{
		m_pResult = pDoodad;
		return false;
	}

	return true;
}

BOOL KSearchForTeamMember::operator ()(KPlayer* pPlayer)
{
    BOOL  bRetCode        = false;
    DWORD dwSelfTeamID    = m_pSelf->GetCurrentTeamID();
    DWORD dwPlayerTeamID  = pPlayer->GetCurrentTeamID();

    if (dwSelfTeamID == ERROR_ID)
        return false;

    if (dwPlayerTeamID != dwSelfTeamID)
        return true;

    bRetCode = g_InRange(m_pSelf, pPlayer, m_nDistance);
    if (bRetCode)
    {
        m_Result.push_back(pPlayer);
    }

    return true;
}

BOOL KSearchForAnyVisiblePlayer::operator()(KPlayer* pPlayer)
{
    BOOL     bRetCode   = false;
    int      nSrcZ      = 0;
    int      nDstZ      = 0;

    nSrcZ = ZPOINT_TO_ALTITUDE(m_pSelf->m_nZ + m_pSelf->m_nHeight + POINT_PER_ALTITUDE / 2);
    nDstZ = ZPOINT_TO_ALTITUDE(pPlayer->m_nZ + pPlayer->m_nHeight + POINT_PER_ALTITUDE / 2);

    bRetCode = m_pSelf->m_pScene->IsVisible(
        m_pSelf->m_nX, m_pSelf->m_nY, nSrcZ,
        pPlayer->m_nX, pPlayer->m_nY, nDstZ
    );
    if (bRetCode)
    {
        m_pResult = pPlayer;
        return false;
    }
    
    return true;
}

BOOL KSearchVisiblePlayerClosest::operator()(KPlayer* pPlayer)
{
    BOOL     bRetCode   = false;
    int      nSrcZ      = 0;
    int      nDstZ      = 0;
    int      nDistance2 = 0;

    nDistance2 = g_GetDistance2(m_pSelf->m_nX, m_pSelf->m_nY, pPlayer->m_nX, pPlayer->m_nY);
    if (nDistance2 > m_nDistance2)
        return true;

    nSrcZ = ZPOINT_TO_ALTITUDE(m_pSelf->m_nZ + m_pSelf->m_nHeight + POINT_PER_ALTITUDE / 2);
    nDstZ = ZPOINT_TO_ALTITUDE(pPlayer->m_nZ + pPlayer->m_nHeight + POINT_PER_ALTITUDE / 2);

    bRetCode = m_pSelf->m_pScene->IsVisible(
        m_pSelf->m_nX, m_pSelf->m_nY, nSrcZ,
        pPlayer->m_nX, pPlayer->m_nY, nDstZ
    );
    if (bRetCode)
    {
        m_pResult    = pPlayer;
        m_nDistance2 = nDistance2;
    }
    
    return true;
}

BOOL KSearchForCharacter::operator ()(KCharacter *pCharacter)
{
	BOOL	bRetCode	= false;
	int		nSrcZ		= 0;
	int		nDstZ		= 0;
	int		nDistance2	= 0;
	int		nAngle		= 0;
	int		nFaceTo		= 0;
	int		nCheckDistance2 = m_nDistance2;

	if (pCharacter == m_pSelf)
		return true;

	//临时代码，不遍历死亡的NPC
	if (pCharacter->m_eMoveState == cmsOnDeath)
		return true;

    //临时代码，忽略交通系统上的Character
    if (pCharacter->m_eMoveState == cmsOnAutoFly)
        return true;

	//都是NPC的情况下,忽视正在返回中的Npc
	if (IS_NPC(pCharacter->m_dwID) && IS_NPC(m_pSelf->m_dwID) && pCharacter->m_bSystemShield)
        return true;

    //战斗关系判断
    int nRelation = g_GetRelation(m_pSelf, pCharacter);
    if (!(nRelation & m_nRelation))
        return true;

	//判断距离
	nDistance2 = g_GetDistance2(m_pSelf->m_nX, m_pSelf->m_nY, pCharacter->m_nX, pCharacter->m_nY);

	//范围受等级差影响
	if (m_bAdjustRangeByLevelDiff)
	{
        int nSelfLevel = m_pSelf->m_nLevel;
        int nTargetLevel = pCharacter->m_nLevel;
        double fDistanceAdjustRate = 1.0;

        if (m_bAdjustRangeByTargetLife)
        {
            assert(pCharacter->m_nMaxLife > 0);

            nSelfLevel += g_GetLevelByTargetLife(m_nLifeConversionLevel, (double)pCharacter->m_nCurrentLife / (double)pCharacter->m_nMaxLife);
        }

		fDistanceAdjustRate = g_GetRangeByLevelDiff(nSelfLevel - nTargetLevel);
		nCheckDistance2 = (int)(nCheckDistance2 * fDistanceAdjustRate * fDistanceAdjustRate);
	}

	//范围受魔法属性影响,可能是某个减少警戒范围的BUFF,比如心灵安抚
	if (m_bAdjustByAttribute)
	{
		double fSelfAlarmRangeRevise = 0.0;
		double fOtherAlarmRangeRevise = 0.0;
			
		//是否忽视所有警戒的NPC
		if (pCharacter->m_nIgnoreOtherAlarmRange > 0)
		{
			return true;
		}

		fSelfAlarmRangeRevise = m_pSelf->m_nSelfAlarmRangeRevise / (double)MAX_ALARM_RANGE_REVISE;
		fOtherAlarmRangeRevise = pCharacter->m_nOtherAlarmRangeRevise / (double)MAX_ALARM_RANGE_REVISE;

		//修正范围
		if (fSelfAlarmRangeRevise < 0.0)
			fSelfAlarmRangeRevise = 0.0;
		if (fSelfAlarmRangeRevise > 1.0)
			fSelfAlarmRangeRevise = 1.0;
		if (fOtherAlarmRangeRevise < 0.0)
			fOtherAlarmRangeRevise = 0.0;
		if (fOtherAlarmRangeRevise > 1.0)
			fOtherAlarmRangeRevise = 1.0;

		fSelfAlarmRangeRevise = 1.0 - fSelfAlarmRangeRevise;
		fOtherAlarmRangeRevise = 1.0 - fOtherAlarmRangeRevise;

		nCheckDistance2 = (int)(nCheckDistance2 * fSelfAlarmRangeRevise * fSelfAlarmRangeRevise);
		nCheckDistance2 = (int)(nCheckDistance2 * fOtherAlarmRangeRevise * fOtherAlarmRangeRevise);
	}

	if (nDistance2 > nCheckDistance2)
		return true;

	//判断角度
	nFaceTo = m_pSelf->m_nFaceDirection;

	nAngle = g_GetDirection(m_pSelf->m_nX, m_pSelf->m_nY, pCharacter->m_nX, pCharacter->m_nY) - nFaceTo;
	if (nAngle < 0)
		nAngle = -nAngle;

	if (nAngle > DIRECTION_COUNT / 2)
		nAngle = DIRECTION_COUNT - nAngle;

	if (m_nAngle >= 0)
	{
		if (nAngle > m_nAngle / 2)
			return true;
	}
	else
	{
		if (nAngle <= -m_nAngle / 2)
			return true;
	}

	//判断可见性
    if (m_bAdjustByVisible)
    {
        nSrcZ = ZPOINT_TO_ALTITUDE(m_pSelf->m_nZ + m_pSelf->m_nHeight + POINT_PER_ALTITUDE / 2);
        nDstZ = ZPOINT_TO_ALTITUDE(pCharacter->m_nZ + pCharacter->m_nHeight + POINT_PER_ALTITUDE / 2);

        bRetCode = m_pSelf->m_pScene->IsVisible(
            m_pSelf->m_nX, m_pSelf->m_nY, nSrcZ,
            pCharacter->m_nX, pCharacter->m_nY, nDstZ
        );
    }

	if (bRetCode)
	{
		KSearchNode SearchNode;
		SearchNode.m_pCharacter = pCharacter;
		SearchNode.m_nDistance2 = nDistance2;

		m_Result.push_back(SearchNode);
	}

	return true;
}

BOOL SearchCharacter(
    KCharacter** ppResult, 
    KCharacter* pCharacter, KCharacter* pSelf, 
    int nSearchDistance2, int nSearchAngle, int nSearchRelation, int nLifeConversionLevel, 
    BOOL bAdjustRangeByLevelDiff, BOOL bAdjustByAttribute, BOOL bAdjustRangeByTargetLife, BOOL bAdjustByVisible
)
{
    BOOL    bResult     = false;
    BOOL	bRetCode    = false;
    int		nSrcZ       = 0;
    int		nDstZ       = 0;
    int		nDistance2  = 0;
    int		nAngle      = 0;
    int		nFaceTo     = 0;
    int     nRelation   = 0;

    assert(ppResult);
    assert(pCharacter);
    assert(pSelf);

    KG_PROCESS_ERROR(pCharacter != pSelf);
    KG_PROCESS_ERROR(pCharacter->m_eMoveState != cmsOnDeath);
    KG_PROCESS_ERROR(pCharacter->m_eMoveState != cmsOnAutoFly);

    //都是NPC的情况下,忽视正在返回中的Npc
    KG_PROCESS_ERROR(IS_PLAYER(pCharacter->m_dwID) || IS_PLAYER(pSelf->m_dwID) || !pCharacter->m_bSystemShield);

    //战斗关系判断
    nRelation = g_GetRelation(pSelf, pCharacter);
    KG_PROCESS_ERROR(nRelation & nSearchRelation);

    //判断距离
    nDistance2 = g_GetDistance2(pSelf->m_nX, pSelf->m_nY, pCharacter->m_nX, pCharacter->m_nY);

    //范围受等级差影响
    if (bAdjustRangeByLevelDiff)
    {
        int nSelfLevel = pSelf->m_nLevel;
        int nTargetLevel = pCharacter->m_nLevel;
        double fDistanceAdjustRate = 1.0;

        if (bAdjustRangeByTargetLife)
        {
            assert(pCharacter->m_nMaxLife > 0);

            nSelfLevel += g_GetLevelByTargetLife(nLifeConversionLevel, (double)pCharacter->m_nCurrentLife / (double)pCharacter->m_nMaxLife);
        }

        fDistanceAdjustRate = g_GetRangeByLevelDiff(nSelfLevel - nTargetLevel);
        nSearchDistance2 = (int)(nSearchDistance2 * fDistanceAdjustRate * fDistanceAdjustRate);
    }

    //范围受魔法属性影响,可能是某个减少警戒范围的BUFF,比如心灵安抚
    if (bAdjustByAttribute)
    {
        double fSelfAlarmRangeRevise = 0.0;
        double fOtherAlarmRangeRevise = 0.0;

        //是否忽视所有警戒的NPC
        KG_PROCESS_ERROR(!pCharacter->m_nIgnoreOtherAlarmRange);

        fSelfAlarmRangeRevise = pSelf->m_nSelfAlarmRangeRevise / (double)MAX_ALARM_RANGE_REVISE;
        fOtherAlarmRangeRevise = pCharacter->m_nOtherAlarmRangeRevise / (double)MAX_ALARM_RANGE_REVISE;

        //修正范围
        if (fSelfAlarmRangeRevise < 0.0)
            fSelfAlarmRangeRevise = 0.0;
        if (fSelfAlarmRangeRevise > 1.0)
            fSelfAlarmRangeRevise = 1.0;
        if (fOtherAlarmRangeRevise < 0.0)
            fOtherAlarmRangeRevise = 0.0;
        if (fOtherAlarmRangeRevise > 1.0)
            fOtherAlarmRangeRevise = 1.0;

        fSelfAlarmRangeRevise = 1.0 - fSelfAlarmRangeRevise;
        fOtherAlarmRangeRevise = 1.0 - fOtherAlarmRangeRevise;

        nSearchDistance2     = (int)(nSearchDistance2 * fSelfAlarmRangeRevise * fSelfAlarmRangeRevise);
        nSearchDistance2     = (int)(nSearchDistance2 * fOtherAlarmRangeRevise * fOtherAlarmRangeRevise);
    }

    KG_PROCESS_ERROR(nDistance2 < nSearchDistance2);

    //判断角度
    nFaceTo = pSelf->m_nFaceDirection;

    nAngle = g_GetDirection(pSelf->m_nX, pSelf->m_nY, pCharacter->m_nX, pCharacter->m_nY) - nFaceTo;
    if (nAngle < 0)
        nAngle = -nAngle;

    if (nAngle > DIRECTION_COUNT / 2)
        nAngle = DIRECTION_COUNT - nAngle;

    if (nSearchAngle >= 0)
        KG_PROCESS_ERROR(nAngle <= nSearchAngle / 2);
    else
        KG_PROCESS_ERROR(nAngle >= -nSearchAngle / 2);

    //判断可见性
    if (bAdjustByVisible)
    {
        nSrcZ = ZPOINT_TO_ALTITUDE(pSelf->m_nZ + pSelf->m_nHeight + POINT_PER_ALTITUDE / 2);
        nDstZ = ZPOINT_TO_ALTITUDE(pCharacter->m_nZ + pCharacter->m_nHeight + POINT_PER_ALTITUDE / 2);

        bRetCode = pSelf->m_pScene->IsVisible(pSelf->m_nX, pSelf->m_nY, nSrcZ, pCharacter->m_nX, pCharacter->m_nY, nDstZ);
        KG_PROCESS_ERROR(bRetCode);
    }

    *ppResult = pCharacter;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KSearchForAnyCharacter::operator ()(KCharacter* pCharacter)
{
    BOOL bRetCode = false;

    bRetCode = SearchCharacter(
        &m_pResult, 
        pCharacter, m_pSelf, 
        m_nDistance2, m_nAngle, m_nRelation, m_nLifeConversionLevel, 
        m_bAdjustRangeByLevelDiff, m_bAdjustByAttribute, m_bAdjustRangeByTargetLife, m_bAdjustByVisible
    );

    if (bRetCode)
        return false;

    return true;
}

BOOL KSearchForMultiCharacterAddThreatList::operator ()(KCharacter* pCharacter)
{
    BOOL bRetCode = false;

    bRetCode = SearchCharacter(
        &m_pResult, 
        pCharacter, m_pSelf, 
        m_nDistance2, m_nAngle, m_nRelation, m_nLifeConversionLevel, 
        m_bAdjustRangeByLevelDiff, m_bAdjustByAttribute, m_bAdjustRangeByTargetLife, m_bAdjustByVisible
    );

    if (bRetCode)
    {
        #ifdef _SERVER
        m_pSelf->m_SimpThreatList.ModifyThreat(thtSpecialThreat, pCharacter, 0);
        #endif

        m_nCharacterCounter--;

        if (m_nCharacterCounter <= 0)
            return false;
    }

    return true;
}

#ifdef _SERVER
BOOL _IsMalefactor(KCharacter* pCharacter)
{
    BOOL bResult    = false;

    assert(pCharacter);

    if (IS_PLAYER(pCharacter->m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)pCharacter;

        if (pPlayer->m_PK.IsOnSlay())
            bResult = true;
        
        if (pPlayer->m_nCurrentKillPoint > 300)
            bResult = true;
    }

    return bResult;
}

BOOL _IsGuardian(KCharacter* pGuarder, KCharacter* pTarget)
{
    BOOL bResult    = false;
    int  nCampMark  = 0;
    int  nForceMark = 0;

    assert(pTarget);
    assert(pGuarder);

    if (IS_NPC(pTarget->m_dwID))
    {
        bResult = pGuarder->m_dwForceID == pTarget->m_dwGuardForceID;
    }
    else
    {
        KPlayer* pPlayer = (KPlayer*)pTarget;

        nCampMark   = 1 << pPlayer->m_eCamp;
        nForceMark  = 1 << pPlayer->m_dwForceID;

        bResult = (pGuarder->m_AIData.dwProtectCampMark  & nCampMark) 
               && (pGuarder->m_AIData.dwProtectForceMark & nForceMark);
    }
    
    return bResult;
}

BOOL KSearchBustupCharacterList::operator ()(KCharacter* pCharacter)
{
    BOOL        bRetCode    = false;
    KCharacter* pResult     = NULL;

    bRetCode = SearchCharacter(
        &pResult, 
        pCharacter, m_pSelf, 
        m_nDistance2, m_nAngle, m_nRelation, m_nLifeConversionLevel, 
        m_bAdjustRangeByLevelDiff, m_bAdjustByAttribute, m_bAdjustRangeByTargetLife, m_bAdjustByVisible
    );

    if (bRetCode)
    {
        int                     nMaxThreatCount = 5;
        BOOL                    bProtectPlayer  = false;
        BOOL                    bProtectThreat  = false;
        KPlayer*                pPlayer         = (KPlayer*)pResult;
        const KSimpThreatNode*  cpThreatNode    = NULL;

        if (_IsMalefactor(pResult))
            m_Result.insert(pResult);

        // 切磋或非战斗不做考虑
        if (!pPlayer->m_bFightState || pPlayer->m_PK.IsDuel())
            return true;
        
        bProtectPlayer = _IsGuardian(m_pSelf, pResult);

        cpThreatNode = pResult->m_SimpThreatList.GetFirstThreat(thtMainThreat);
        while (--nMaxThreatCount && cpThreatNode)
        {
			assert(cpThreatNode->pCharacter);

            BOOL bIsPlayer = IS_PLAYER(cpThreatNode->pCharacter->m_dwID);

            bProtectThreat = _IsGuardian(m_pSelf, cpThreatNode->pCharacter);

            // 仇恨关系为Player->NPC
            if (!bIsPlayer && bProtectThreat)
                m_Result.insert(pResult);

            // 仇恨关系为Player->Player
            if (bIsPlayer)
            {
                KCharacter* pCandidate = NULL;

                if (_IsMalefactor(cpThreatNode->pCharacter))
                    m_Result.insert(cpThreatNode->pCharacter);
               
                if (bProtectPlayer == bProtectThreat)
                {
                    pCandidate = cpThreatNode->bPrimacord ? cpThreatNode->pCharacter : pResult;
                    m_Result.insert(pCandidate);
                }
                else
                {
                    if (bProtectPlayer && cpThreatNode->bPrimacord)
                        m_Result.insert(cpThreatNode->pCharacter);

                    if (bProtectThreat && !(cpThreatNode->bPrimacord))
                        m_Result.insert(pResult);
                }
            }

            cpThreatNode = pResult->m_SimpThreatList.GetNextThreat(thtMainThreat, cpThreatNode);
        }
    }

    return true;
}
#endif

void TestFunc()
{
    KPlayer Player;
    KSearchForAnyVisiblePlayer Search;

    Search.m_pSelf   = &Player;
    Search.m_pResult = NULL;

    AISearchPlayer(Search);
}
