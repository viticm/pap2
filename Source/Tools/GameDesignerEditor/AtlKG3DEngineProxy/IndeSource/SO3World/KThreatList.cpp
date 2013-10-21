#include "stdafx.h"
#include "Engine/KMemory.h"
#include "KThreatList.h"
#include "KSO3World.h"
#include "KPlayer.h"
#include "KPlayerServer.h"
#include "KMath.h"
#include "KScene.h"

#ifdef _SERVER

BOOL KThreatLink::AddMember(DWORD dwThreatLinkID, KCharacter* pCharacter)
{
    BOOL                    bResult = false;
    MEMBER_VECTOR&          rLink   = m_ThreatLinkMap[dwThreatLinkID];
    MEMBER_VECTOR::iterator it;

    KG_PROCESS_ERROR(dwThreatLinkID);
    assert(pCharacter);

    it = std::find(rLink.begin(), rLink.end(), pCharacter->m_dwID);
    KG_PROCESS_SUCCESS(it != rLink.end());

    rLink.push_back(pCharacter->m_dwID);

Exit1:
    bResult = true;
Exit0:
    return bResult;    
}

BOOL KThreatLink::DelMember(DWORD dwThreatLinkID, KCharacter* pCharacter)
{
    BOOL                    bResult = false;
    MEMBER_VECTOR&          rLink   = m_ThreatLinkMap[dwThreatLinkID];
    MEMBER_VECTOR::iterator it;

    KG_PROCESS_ERROR(dwThreatLinkID);
    assert(pCharacter);

    it = std::find(rLink.begin(), rLink.end(), pCharacter->m_dwID);
    KG_PROCESS_ERROR(it != rLink.end());

    rLink.erase(it);

Exit1:
    bResult = true;
Exit0:
    return bResult;    
}

BOOL KThreatLink::NewThreat(DWORD dwThreatLinkID, KCharacter* pCharacter)
{
    BOOL                    bResult = false;
    MEMBER_VECTOR&          rLink   = m_ThreatLinkMap[dwThreatLinkID];
    MEMBER_VECTOR::iterator itEnd   = rLink.end();
    MEMBER_VECTOR::iterator it;

    KG_PROCESS_ERROR(dwThreatLinkID);
    assert(pCharacter);

    for (it = rLink.begin(); it != itEnd; ++it)
    {
        DWORD       dwCharacterID   = *it;
        KCharacter* pDstCharacter   = NULL;

        if (IS_NPC(dwCharacterID))
            pDstCharacter = g_pSO3World->m_NpcSet.GetObj(dwCharacterID);
        else
            pDstCharacter = g_pSO3World->m_PlayerSet.GetObj(dwCharacterID);

        if (!pDstCharacter || pDstCharacter->m_eMoveState == cmsOnDeath)
            continue;

        pDstCharacter->m_SimpThreatList.ModifyThreat(thtSpecialThreat, pCharacter, 0);
    }

    bResult = true;
Exit0:
    return bResult;        
}

BOOL KThreatLink::SyncAllThreat(DWORD dwThreatLinkID, KCharacter* pDstCharacter)
{
    BOOL                    bResult = false;
    MEMBER_VECTOR&          rLink   = m_ThreatLinkMap[dwThreatLinkID];
    MEMBER_VECTOR::iterator itEnd   = rLink.end();
    MEMBER_VECTOR::iterator it;

    KG_PROCESS_ERROR(dwThreatLinkID);
    assert(pDstCharacter);

    for (it = rLink.begin(); it != itEnd; ++it)
    {
        DWORD                   dwCharacterID       = *it;
        KCharacter*             pCharacter          = NULL;
        const KSimpThreatNode*  cpSimpThreatNode    = NULL;

        if (IS_NPC(dwCharacterID))
            pCharacter = g_pSO3World->m_NpcSet.GetObj(dwCharacterID);
        else
            pCharacter = g_pSO3World->m_PlayerSet.GetObj(dwCharacterID);

        if (!pCharacter || pCharacter->m_eMoveState == cmsOnDeath)
            continue;

        cpSimpThreatNode = pCharacter->m_SimpThreatList.GetFirstThreat(thtMainThreat);
        while (cpSimpThreatNode)
        {
            if (cpSimpThreatNode->pCharacter)
                pDstCharacter->m_SimpThreatList.ModifyThreat(thtSpecialThreat, cpSimpThreatNode->pCharacter, 0);

            cpSimpThreatNode = pCharacter->m_SimpThreatList.GetNextThreat(thtMainThreat, cpSimpThreatNode);
        }
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KThreatLink::ClearAllMemberThreat(DWORD dwThreatLinkID)
{
    BOOL                    bResult     = false;
    BOOL                    bRetCode    = false;
    MEMBER_VECTOR&          rLink       = m_ThreatLinkMap[dwThreatLinkID];
    MEMBER_VECTOR::iterator itEnd       = rLink.end();
    MEMBER_VECTOR::iterator it;

    KG_PROCESS_ERROR(dwThreatLinkID);

    for (it = rLink.begin(); it != itEnd; ++it)
    {
        DWORD           dwCharacterID   = *it;
        KCharacter*     pCharacter      = NULL;

        if (IS_NPC(dwCharacterID))
            pCharacter = g_pSO3World->m_NpcSet.GetObj(dwCharacterID);
        else
            pCharacter = g_pSO3World->m_PlayerSet.GetObj(dwCharacterID);

        if (!pCharacter || pCharacter->m_eMoveState == cmsOnDeath)
            continue;

        bRetCode = pCharacter->m_SimpThreatList.ClearAllThreat();
        KGLOG_CHECK_ERROR(bRetCode);
    }

    bResult = true;
Exit0:
    return bResult;
}

#endif // _SERVER 

#ifdef _CLIENT

KThreatList::KThreatList(void)
{
	m_pSelf = NULL;
}

BOOL KThreatList::Init(KCharacter* pSelf)
{
    BOOL bResult = false;

	KGLOG_PROCESS_ERROR(pSelf);
	KGLOG_PROCESS_ERROR(m_ThreatList.IsEmpty());
	KGLOG_PROCESS_ERROR(m_ThreatenList.IsEmpty());

	m_pSelf = pSelf;

	bResult = true;
Exit0:
	return bResult;
}

void KThreatList::UnInit(void)
{	
	BOOL				bRetCode		= false;
	KThreatLiteNode*	pThreatNode		= NULL;
	KThreatLiteNode*	pThreatenNode	= NULL;

	pThreatNode = (KThreatLiteNode*)m_ThreatList.GetHead();
	while (pThreatNode)
	{
		KThreatLiteNode* pNext = (KThreatLiteNode*)pThreatNode->GetNext();

		bRetCode = ClearThreat(pThreatNode);
		KGLOG_CHECK_ERROR(bRetCode);

		pThreatNode = pNext;
	}

	pThreatenNode = (KThreatLiteNode*)m_ThreatenList.GetHead();
	while (pThreatenNode)
	{
		KThreatLiteNode* pNext = (KThreatLiteNode*)pThreatenNode->GetNext();

		bRetCode = ClearThreat(pThreatenNode);
		KGLOG_CHECK_ERROR(bRetCode);

		pThreatenNode = pNext;
	}

	return;
}

BOOL KThreatList::ModifyThreat(DWORD dwCharacterID)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;
	KThreatLiteNode* pNode = NULL;

	KGLOG_PROCESS_ERROR(dwCharacterID != ERROR_ID);

	bRetCode = IsInThreatList(dwCharacterID);

	if (!bRetCode)
	{
		pNode = KMemory::New<KThreatLiteNode>();
		pNode->dwCharacterID = dwCharacterID;
		m_ThreatList.AddTail(pNode);
	}

	bResult = true;
Exit0:
	return bResult;
}

BOOL KThreatList::ModifyThreaten(DWORD dwCharacterID)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;
	KThreatLiteNode* pNode = NULL;

	KGLOG_PROCESS_ERROR(dwCharacterID != ERROR_ID);

	bRetCode = IsInThreatenList(dwCharacterID);

	if (!bRetCode)
	{
		pNode = KMemory::New<KThreatLiteNode>();
		pNode->dwCharacterID = dwCharacterID;
		m_ThreatenList.AddTail(pNode);
	}

	bResult = true;
Exit0:
	return bResult;
}

BOOL KThreatList::ClearThreat(DWORD dwCharacterID)
{
    BOOL                bResult     = false;
	BOOL                bRetCode    = false;
	KThreatLiteNode*    pNode       = NULL;
    KThreatLiteNode*    pNext       = NULL;

	pNode = (KThreatLiteNode*)m_ThreatList.GetHead();
	while (pNode)
	{
        pNext = (KThreatLiteNode*)pNode->GetNext();

		if (pNode->dwCharacterID == dwCharacterID)
		{
			bRetCode = ClearThreat(pNode);
			KGLOG_PROCESS_ERROR(bRetCode);
			break;
		}

		pNode = pNext;
	}

	bResult = true;
Exit0:
	return bResult;
}

BOOL KThreatList::ClearThreaten(DWORD dwCharacterID)
{
    BOOL                bResult     = false;
	BOOL                bRetCode    = false;
	KThreatLiteNode*    pNode       = NULL;
    KThreatLiteNode*    pNext       = NULL;

	pNode = (KThreatLiteNode*)m_ThreatenList.GetHead();
	while (pNode)
	{
        pNext = (KThreatLiteNode*)pNode->GetNext();

		if (pNode->dwCharacterID == dwCharacterID)
		{
            bRetCode = ClearThreat(pNode);
			KGLOG_PROCESS_ERROR(bRetCode);
			break;
		}

		pNode = pNext;
	}

	bResult = true;
Exit0:
	return bResult;
}

const KThreatLiteNode* KThreatList::GetFirstThreat()
{
	KThreatLiteNode* pNode = NULL;

	pNode = (KThreatLiteNode*)m_ThreatList.GetHead();

	return pNode;
}

const KThreatLiteNode* KThreatList::GetFirstThreaten()
{
	KThreatLiteNode* pNode = NULL;

    pNode = (KThreatLiteNode*)m_ThreatenList.GetHead();   
    
    return pNode;
}

BOOL KThreatList::ClearThreat(KThreatLiteNode* pNode)
{
	KGLOG_PROCESS_ERROR(pNode);
	
	pNode->Remove();
    KMemory::Delete(pNode);
    pNode = NULL;

	return true;
Exit0:
	return false;
}

BOOL KThreatList::IsInThreatList(DWORD dwCharacterID)
{
	KThreatLiteNode *pNode = NULL;

	KGLOG_PROCESS_ERROR(dwCharacterID != ERROR_ID);

	pNode = (KThreatLiteNode*)m_ThreatList.GetHead();
	while (pNode)
	{
		if (pNode->dwCharacterID == dwCharacterID)
			return true;

		pNode = (KThreatLiteNode*)pNode->GetNext();
	}

Exit0:
	return false;
}

BOOL KThreatList::IsInThreatenList(DWORD dwCharacterID)
{
	KThreatLiteNode *pNode = NULL;

	KGLOG_PROCESS_ERROR(dwCharacterID != ERROR_ID);

	pNode = (KThreatLiteNode*)m_ThreatenList.GetHead();
	while (pNode)
	{
		if (pNode->dwCharacterID == dwCharacterID)
			return true;

		pNode = (KThreatLiteNode*)pNode->GetNext();
	}
Exit0:
	return false;
}

#endif // _CLIENT

//////////////////////////////////////////////////////////////////////////

#ifdef _SERVER

KSimpThreatNode::KSimpThreatNode()
{
    pCharacter              = NULL;
    pPairNode               = NULL;
    nInitFrame              = g_pSO3World->m_nGameLoop;
    nLastRefreshFrame       = nInitFrame;
    nLastInThreatFieldFrame = nInitFrame;
    bTopFlag                = false;
    
    for (int i = thtInvalid + 1; i < thtTotal; i++)
        nThreat[i] = 0;
}

KSimpThreatList::KSimpThreatList()
{
    m_pSelf             = NULL;
    m_nNextCheckLoop    = 0;
    m_nThreatPointX     = 0;
    m_nThreatPointY     = 0;
}

BOOL KSimpThreatList::Init(KCharacter* pSelf)
{
    assert(pSelf);

    m_pSelf = pSelf;

    return true;
}

void KSimpThreatList::UnInit()
{
    ClearAllThreat();
    ClearAllThreaten();

    m_pSelf = NULL;
}

BOOL KSimpThreatList::ModifyThreat(THREAT_TYPE eThreatType, KCharacter* pCharacter, int nThreat)
{
    BOOL                bResult         = false;
    KSimpThreatNode*    pSimpThreatNode = NULL;

    KGLOG_PROCESS_ERROR(eThreatType > thtInvalid);
    KGLOG_PROCESS_ERROR(eThreatType < thtTotal);
    KGLOG_PROCESS_ERROR(pCharacter);

    nThreat = pCharacter->AdjustThreat(eThreatType, nThreat);

    pSimpThreatNode = _FindThreat(pCharacter);
    if (!pSimpThreatNode)
    {
        KG_PROCESS_SUCCESS(nThreat < 0);

        pSimpThreatNode = KMemory::New<KSimpThreatNode>();
        assert(pSimpThreatNode);

        pSimpThreatNode->pCharacter         = pCharacter;
        pSimpThreatNode->nInitFrame         = g_pSO3World->m_nGameLoop;
        pSimpThreatNode->nLastRefreshFrame  = g_pSO3World->m_nGameLoop;

        for (int i = thtInvalid + 1; i < thtTotal; i++)
            m_SortedList[i].AddTail(&(pSimpThreatNode->SortedListNode[i]));

        pSimpThreatNode->pPairNode = KMemory::New<KSimpThreatenNode>();
        assert(pSimpThreatNode->pPairNode);

        pSimpThreatNode->pPairNode->pCharacter = m_pSelf;
        pSimpThreatNode->pPairNode->pPairNode = pSimpThreatNode;

        pCharacter->m_SimpThreatList.m_ThreatenList.AddTail(pSimpThreatNode->pPairNode);

        if (IS_NPC(m_pSelf->m_dwID))
        {
            KNpc* pNpc = (KNpc*)m_pSelf;

            if (pNpc->m_nNpcTeamID && pNpc->m_pScene)
                pNpc->m_pScene->m_ThreatLink.NewThreat(pNpc->m_nNpcTeamID, pCharacter);
        }

        if (IS_PLAYER(m_pSelf->m_dwID))
            g_PlayerServer.DoModifyThreat(((KPlayer*)m_pSelf)->m_nConnIndex, pCharacter->m_dwID);

        if (IS_PLAYER(pCharacter->m_dwID))
            g_PlayerServer.DoModifyThreaten(((KPlayer*)pCharacter)->m_nConnIndex, m_pSelf->m_dwID);

        if (IS_NPC(m_pSelf->m_dwID))
            OnCharacterEnterNpcThreatList(pCharacter);
    }

    pSimpThreatNode->nThreat[eThreatType] += nThreat;

    if (eThreatType != thtMainThreat)
        pSimpThreatNode->nThreat[thtMainThreat] += nThreat;

    pSimpThreatNode->nLastRefreshFrame = g_pSO3World->m_nGameLoop;

    _AdjustTheartList(pSimpThreatNode);

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KSimpThreatList::ModifyThreaten(THREAT_TYPE eThreatType, KCharacter* pCharacter, int nThreat)
{
    BOOL                bResult = false;
    KSimpThreatenNode*  pNode   = NULL;

    KGLOG_PROCESS_ERROR(eThreatType > thtInvalid);
    KGLOG_PROCESS_ERROR(eThreatType < thtTotal);
    KGLOG_PROCESS_ERROR(pCharacter);

    pNode = (KSimpThreatenNode*)m_ThreatenList.GetHead();
    while (pNode)
    {
        assert(pNode->pPairNode);
        assert(pNode->pCharacter);

        pNode->pCharacter->m_SimpThreatList.ModifyThreat(eThreatType, pCharacter, nThreat);

        pNode = (KSimpThreatenNode*)pNode->GetNext();
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KSimpThreatList::ClearThreat(KCharacter* pCharacter)
{
    BOOL                bResult         = false;
    BOOL                bRetCode        = false;
    KSimpThreatNode*    pSimpThreatNode = NULL;

    KGLOG_PROCESS_ERROR(pCharacter);

    pSimpThreatNode = _FindThreat(pCharacter);
    KG_PROCESS_ERROR(pSimpThreatNode);

    bRetCode = _ClearThreat(pSimpThreatNode);
    KGLOG_CHECK_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KSimpThreatList::ClearAllThreat()
{
    BOOL    bResult     = false;
    BOOL    bRetCode    = false;
    KNode*  pNext       = NULL;

    pNext = m_SortedList[thtMainThreat].GetHead();
    while (pNext)
    {
        KSimpThreatNode* pSimpThreatNode = KTHREAT_CONTAINING_RECORD(pNext, KSimpThreatNode, SortedListNode[thtMainThreat]);
        pNext = pNext->GetNext();

        bRetCode = _ClearThreat(pSimpThreatNode);
        KGLOG_CHECK_ERROR(bRetCode);
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KSimpThreatList::ClearAllThreaten()
{
    BOOL    bResult     = false;
    BOOL    bRetCode    = false;
    KNode*  pNext       = NULL;
    
    pNext = m_ThreatenList.GetHead();
    while (pNext)
    {
        KSimpThreatenNode* pSimpThreatenNode = (KSimpThreatenNode*)pNext;
        pNext = pNext->GetNext();

        bRetCode = pSimpThreatenNode->pCharacter->m_SimpThreatList._ClearThreat(pSimpThreatenNode->pPairNode);
        KGLOG_CHECK_ERROR(bRetCode);
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KSimpThreatList::StickThreat(KCharacter* pCharacter)
{
    BOOL                bResult = false;
    KSimpThreatNode*    pNode   = NULL;

    KGLOG_PROCESS_ERROR(pCharacter);

    pNode = _FindThreat(pCharacter);
    if (!pNode)
    {
        ModifyThreat(thtMainThreat, pCharacter, 0);
        pNode = _FindThreat(pCharacter);
    }
    KGLOG_PROCESS_ERROR(pNode);

    // 由于BUFF机制限制，保证同一时间只有一个仇恨置顶。
    UnstickThreat();

    pNode->bTopFlag = true;

    _AdjustTheartList(pNode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KSimpThreatList::UnstickThreat()
{
    BOOL                bResult = false;
    KSimpThreatNode*    pNode   = NULL;

    pNode = _GetFirstThreat(thtMainThreat);
    KG_PROCESS_ERROR(pNode);
    KG_PROCESS_ERROR(pNode->bTopFlag);

    pNode->bTopFlag = false;

    _AdjustTheartList(pNode);

    bResult = true;
Exit0:
    return bResult;
}

const KSimpThreatNode* KSimpThreatList::GetFirstThreat(THREAT_TYPE eThreatType)
{
    return _GetFirstThreat(eThreatType);
}

const KSimpThreatNode* KSimpThreatList::GetNextThreat(THREAT_TYPE eThreatType, const KSimpThreatNode* cpThreatNode)
{
    const KSimpThreatNode*  cpResult    = NULL;
    KNode*                  pNode       = NULL;

    KGLOG_PROCESS_ERROR(eThreatType > thtInvalid);
    KGLOG_PROCESS_ERROR(eThreatType < thtTotal);
    KGLOG_PROCESS_ERROR(cpThreatNode);

    pNode = ((KSimpThreatNode*)cpThreatNode)->SortedListNode[eThreatType].GetNext();    // What the fuck! The KNode doesn't accept a const pointer for GetNext()! 
    KG_PROCESS_ERROR(pNode);

    cpResult = KTHREAT_CONTAINING_RECORD(pNode, KSimpThreatNode, SortedListNode[eThreatType]);
Exit0:
    return cpResult;
}

const KSimpThreatNode* KSimpThreatList::GetNthThreat(THREAT_TYPE eThreatType, int n)
{
    const KSimpThreatNode*  cpResult    = NULL;
    KNode*                  pNode       = NULL;

    KGLOG_PROCESS_ERROR(eThreatType > thtInvalid);
    KGLOG_PROCESS_ERROR(eThreatType < thtTotal);
    KGLOG_PROCESS_ERROR(n > 0);

    pNode = m_SortedList[eThreatType].GetHead();
    KG_PROCESS_ERROR(pNode);

    for (int i = 2; i < n; i++)
    {
        pNode = pNode->GetNext();
        KG_PROCESS_ERROR(pNode);
    }

    cpResult = KTHREAT_CONTAINING_RECORD(pNode, KSimpThreatNode, SortedListNode[eThreatType]);
Exit0:
    return cpResult;
}

const KSimpThreatNode* KSimpThreatList::FindThreat(KCharacter* pCharacter)
{
    return _FindThreat(pCharacter);
}

const KSimpThreatenNode* KSimpThreatList::GetFirstThreaten()
{
    return (const KSimpThreatenNode*)m_ThreatenList.GetHead();
}

const KSimpThreatenNode* KSimpThreatList::GetNextThreaten(const KSimpThreatenNode* cpThreatenNode)
{
    const KSimpThreatenNode*    cpResult    = NULL;
    KNode*                      pNode       = NULL;

    KGLOG_PROCESS_ERROR(cpThreatenNode);

    pNode = ((KSimpThreatenNode*)cpThreatenNode)->GetNext();    // What the fuck! The KNode doesn't accept a const pointer for GetNext()! 
    KG_PROCESS_ERROR(pNode);

    cpResult = (const KSimpThreatenNode*)pNode;
Exit0:
    return cpResult;
}

BOOL KSimpThreatList::IsInThreatList(KCharacter* pCharacter)
{
    BOOL                bResult = false;
    KSimpThreatNode*    pNode   = NULL;

    pNode = _FindThreat(pCharacter);
    KG_PROCESS_ERROR(pNode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KSimpThreatList::IsInThreatenList(KCharacter* pCharacter)
{
    BOOL                bResult = false;
    KSimpThreatenNode*  pNode   = NULL;

    pNode = (KSimpThreatenNode*)m_ThreatenList.GetHead();
    while (pNode)
    {
        if (pNode->pCharacter == pCharacter)
            break;

        pNode = (KSimpThreatenNode*)pNode->GetNext();
    }

    KG_PROCESS_ERROR(pNode);

    bResult = true;
Exit0:
    return bResult;
}

void KSimpThreatList::UpdateKeepThreatField()
{
    assert(m_pSelf);

    m_nThreatPointX = m_pSelf->m_nX;
    m_nThreatPointY = m_pSelf->m_nY;

    return;
}

void KSimpThreatList::Activate()
{
    BOOL    bIsPlayer   = false;
    KNode*  pNext       = NULL;

    KG_PROCESS_ERROR(m_pSelf);
    KG_PROCESS_ERROR(m_nNextCheckLoop < g_pSO3World->m_nGameLoop);
    m_nNextCheckLoop = g_pSO3World->m_nGameLoop + CHECK_THREAT_INTERVAL;

    bIsPlayer = IS_PLAYER(m_pSelf->m_dwID);

    pNext = m_SortedList[thtMainThreat].GetHead();
    while (pNext)
    {
        KSimpThreatNode*    pNode               = KTHREAT_CONTAINING_RECORD(pNext, KSimpThreatNode, SortedListNode[thtMainThreat]);
        KCharacter*         pCharacter          = pNode->pCharacter;
        int                 nDistance2          = 0;
        int                 nKeepThreatRange    = 0;
        BOOL                bInRange            = false;

        assert(pNode->pPairNode);

        pNext = pNext->GetNext();

        if (
            pCharacter == NULL || 
            pCharacter->m_pScene == NULL || 
            pCharacter->m_pScene->m_dwMapID != m_pSelf->m_pScene->m_dwMapID || 
            pCharacter->m_pScene->m_nCopyIndex != m_pSelf->m_pScene->m_nCopyIndex
        )
        {
            _ClearThreat(pNode);
            continue;
        }

        if (pCharacter->m_eMoveState == cmsOnDeath)
        {
            m_pSelf->m_AIVM.FireEvent(aevOnTargetKilled, pCharacter->m_dwID, 0);

            _ClearThreat(pNode);
            continue;
        }

        // 检测仇恨消退时间,玩家为6秒规则,Npc的消退时间填在模板表里
        if (g_pSO3World->m_nGameLoop >= (pNode->nLastRefreshFrame + m_pSelf->m_nThreatTime))
        {
            _ClearThreat(pNode);
            continue;
        }

        // 后面的流程只针对NPC --------------------------------------------------------
        if (bIsPlayer)
            continue;

        nKeepThreatRange = g_pSO3World->m_Settings.m_NpcTemplateList.GetTemplate(0)->nPursuitRange;

        // NPC 模板定义的追击范围
        if (((KNpc*)m_pSelf)->m_pTemplate->nPursuitRange)
            nKeepThreatRange = ((KNpc*)m_pSelf)->m_pTemplate->nPursuitRange;

        bInRange = g_InRange(m_nThreatPointX, m_nThreatPointY, 0, pCharacter->m_nX, pCharacter->m_nY, 0, nKeepThreatRange);

        // 检测玩家是否从保持仇恨的范围外又回到了范围内
        if (bInRange || nKeepThreatRange == -1)
        {
            pNode->nLastInThreatFieldFrame = g_pSO3World->m_nGameLoop;
            continue;
        }

        // 玩家如果在保持仇恨范围外并且超时，则清除该仇恨
        if ((g_pSO3World->m_nGameLoop - pNode->nLastInThreatFieldFrame) >= CHECK_THREAT_INTERVAL + THREAT_FADE_FRAME)
        {
            _ClearThreat(pNode);
            continue;
        }
    }

Exit0:
    return;
}

BOOL KSimpThreatList::IsDropCandidate(KPlayer* pPlayer)
{
    BOOL bResult = false;

    assert(m_pSelf);
    assert(pPlayer);

    KG_PROCESS_ERROR(m_pSelf->m_pScene);
    KG_PROCESS_ERROR(pPlayer->m_eMoveState != cmsOnDeath);
    KG_PROCESS_ERROR(pPlayer->m_pScene == m_pSelf->m_pScene);

    bResult = true;
Exit0:
    return bResult;
}

void KSimpThreatList::OnCharacterEnterNpcThreatList(KCharacter* pEnterCharacter)
{
    KNpc*           pNpc        = (KNpc*)m_pSelf;

    assert(pEnterCharacter);
    assert(pNpc);

    KGLOG_PROCESS_ERROR(IS_NPC(m_pSelf->m_dwID));

    if (IS_PLAYER(pEnterCharacter->m_dwID) && pNpc->m_dwDropTargetPlayerID != ERROR_ID)
    {
        KPlayer* pEnterPlayer = (KPlayer*)pEnterCharacter;
        KPlayer* pDropTarget = g_pSO3World->m_PlayerSet.GetObj(pNpc->m_dwDropTargetPlayerID);
        if (
            pDropTarget && 
            (
                pEnterPlayer->m_dwTeamID == ERROR_ID || 
                pDropTarget->m_dwTeamID != pEnterPlayer->m_dwTeamID
            )
        )
        {
            pNpc->m_nAllAttackerLevel += pEnterPlayer->m_nLevel;
        }
    }
    else if (IS_NPC(pEnterCharacter->m_dwID))
    {
        KNpc* pEnterNpc = (KNpc*)pEnterCharacter;

        if (pEnterNpc->m_pTemplate && !pEnterNpc->m_pTemplate->bDropNotQuestItemFlag)
        {
            pNpc->m_bDropNotQuestItemFlag = false;
        }

        pNpc->m_nAllAttackerLevel += pEnterCharacter->m_nLevel;
    }

Exit0:
    return;
}

void KSimpThreatList::OnCharacterLeaveNpcThreatList(KCharacter* pLeaveCharacter)
{
    BOOL            bRetCode    = false;
    KNpc*           pNpc        = (KNpc*)m_pSelf;
    KPlayer*        pDropTarget = NULL;

    assert(pLeaveCharacter);
    assert(pNpc);

    KGLOG_PROCESS_ERROR(IS_NPC(m_pSelf->m_dwID));

    KG_PROCESS_ERROR(pNpc->m_dwDropTargetPlayerID == pLeaveCharacter->m_dwID);
    KG_PROCESS_ERROR(m_pSelf->m_eMoveState != cmsOnDeath);

    pDropTarget = g_pSO3World->m_PlayerSet.GetObj(pNpc->m_dwDropTargetPlayerID);
    KGLOG_PROCESS_ERROR(pDropTarget);

    pNpc->m_dwDropTargetPlayerID = ERROR_ID;

    if (pDropTarget->m_dwTeamID != ERROR_ID)
    {
        const KSimpThreatNode* cpThreatNode = NULL;

        cpThreatNode = GetFirstThreat(thtMainThreat);
        while (cpThreatNode)
        {
            assert(cpThreatNode->pCharacter);

            if (IS_PLAYER(cpThreatNode->pCharacter->m_dwID))
            {
                KPlayer* pThreatPlayer = (KPlayer*)cpThreatNode->pCharacter;
                if (
                    pThreatPlayer->m_dwTeamID == pDropTarget->m_dwTeamID &&
                    pThreatPlayer->m_dwID != pDropTarget->m_dwID
                )
                {
                    bRetCode = IsDropCandidate(pThreatPlayer);
                    if (bRetCode)
                    {
                        pNpc->m_dwDropTargetPlayerID = pThreatPlayer->m_dwID;
                        break;
                    }
                }
            }

            cpThreatNode = GetNextThreat(thtMainThreat, cpThreatNode);
        }
    }

    if (pNpc->m_dwDropTargetPlayerID == ERROR_ID)
    {
        // 如果已经清掉了掉落目标，同时还要清掉其他信息
        pNpc->m_bDropNotQuestItemFlag   = true;
        pNpc->m_nAllAttackerLevel       = 0;
    }

Exit0:
    return;
}

KSimpThreatNode* KSimpThreatList::_GetFirstThreat(THREAT_TYPE eThreatType)
{
    KSimpThreatNode*    pResult = NULL;
    KNode*              pNode   = NULL;

    KGLOG_PROCESS_ERROR(eThreatType > thtInvalid);
    KGLOG_PROCESS_ERROR(eThreatType < thtTotal);

    pNode = m_SortedList[eThreatType].GetHead();
    KG_PROCESS_ERROR(pNode);

    pResult = KTHREAT_CONTAINING_RECORD(pNode, KSimpThreatNode, SortedListNode[eThreatType]);
Exit0:
    return pResult;
}

KSimpThreatNode* KSimpThreatList::_FindThreat(KCharacter* pCharacter)
{
    KSimpThreatNode*    pResult = NULL;
    KNode*              pNext   = NULL;

    pNext = m_SortedList[thtMainThreat].GetHead();
    while (pNext)
    {
        KSimpThreatNode* pNode = KTHREAT_CONTAINING_RECORD(pNext, KSimpThreatNode, SortedListNode[thtMainThreat]);

        pNext = pNext->GetNext();

        if (pNode->pCharacter != pCharacter)
            continue;

        pResult = pNode;
        break;
    }

Exit0:
    return pResult;
}

void KSimpThreatList::_AdjustTheartList(KSimpThreatNode* pSimpThreatNode)
{
    assert(pSimpThreatNode);
    
    for (int i = thtInvalid + 1; i < thtTotal; i++)
    {
        KNode*  pPrev   = NULL;
        KNode*  pNext   = NULL;
        KNode*  pAdjust = NULL;

        pPrev = pSimpThreatNode->SortedListNode[i].GetPrev();
        while (pPrev)
        {
            KSimpThreatNode* pPrevSimpThreatNode = KTHREAT_CONTAINING_RECORD(pPrev, KSimpThreatNode, SortedListNode[i]);

            if (!pSimpThreatNode->bTopFlag && pPrevSimpThreatNode->bTopFlag)
                break;

            if (pSimpThreatNode->bTopFlag == pPrevSimpThreatNode->bTopFlag && pSimpThreatNode->nThreat[i] <= pPrevSimpThreatNode->nThreat[i])
                break;

            pAdjust = pPrev;
            pPrev = pPrev->GetPrev();
        }

        if (pAdjust)
        {
            pSimpThreatNode->SortedListNode[i].Remove();
            pAdjust->InsertBefore(&pSimpThreatNode->SortedListNode[i]);

            pAdjust = NULL;
        }

        pNext = pSimpThreatNode->SortedListNode[i].GetNext();
        while (pNext)
        {
            KSimpThreatNode* pNextSimpThreatNode = KTHREAT_CONTAINING_RECORD(pNext, KSimpThreatNode, SortedListNode[i]);

            if (pSimpThreatNode->bTopFlag && !pNextSimpThreatNode->bTopFlag)
                break;

            if (pSimpThreatNode->bTopFlag == pNextSimpThreatNode->bTopFlag && pSimpThreatNode->nThreat[i] >= pNextSimpThreatNode->nThreat[i])
                break;

            pAdjust = pNext;
            pNext = pNext->GetNext();
        }

        if (pAdjust)
        {
            pSimpThreatNode->SortedListNode[i].Remove();
            pAdjust->InsertAfter(&pSimpThreatNode->SortedListNode[i]);

            pAdjust = NULL;
        }
    }
}

BOOL KSimpThreatList::_ClearThreat(KSimpThreatNode *pSimpThreatNode)
{
    BOOL        bResult     = false;
    KCharacter* pThreatDst  = NULL;

    KGLOG_PROCESS_ERROR(pSimpThreatNode);

    pThreatDst = pSimpThreatNode->pCharacter;
    KGLOG_PROCESS_ERROR(pThreatDst);
    
    if (IS_NPC(m_pSelf->m_dwID))
        OnCharacterLeaveNpcThreatList(pThreatDst);

    if (IS_PLAYER(m_pSelf->m_dwID))
        g_PlayerServer.DoClearThreat(((KPlayer*)m_pSelf)->m_nConnIndex, pThreatDst->m_dwID);

    if (IS_PLAYER(pThreatDst->m_dwID))
    {
        g_PlayerServer.DoClearThreaten(((KPlayer*)pThreatDst)->m_nConnIndex, m_pSelf->m_dwID);

        //npc对玩家清除仇恨的同时,玩家也必定清除Npc的仇恨
        if (IS_NPC(m_pSelf->m_dwID))
            pThreatDst->m_SimpThreatList.ClearThreat(m_pSelf);
    }

    pSimpThreatNode->pPairNode->Remove();
    KMemory::Delete(pSimpThreatNode->pPairNode);
    pSimpThreatNode->pPairNode = NULL;

    for (int i = thtInvalid + 1; i < thtTotal; i++)
        pSimpThreatNode->SortedListNode[i].Remove();

    KMemory::Delete(pSimpThreatNode);

    pSimpThreatNode = NULL;

    bResult = true;
Exit0:
    return bResult;
}

#endif // _SERVER
