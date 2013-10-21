#include "stdafx.h"
#include "KDesignation.h"
#include "KPlayerServer.h"
#include "KPlayerClient.h"
#include "KRelayClient.h"
#include "KPlayer.h"
#include "KDesignationList.h"

#pragma pack(1)
struct KDesignationDB
{
    BYTE    byCurrentPrefix;   // 前缀
    BYTE    byCurrentPostfix;  // 后缀
    BYTE    byGenerationIndex; // 辈分
    int     nBynameIndex;      // 绰号
    BYTE    byDisplayFlag;     // 是否显示辈分与绰号
};
#pragma pack()

KDesignation::KDesignation()
{
}

KDesignation::~KDesignation()
{
}

BOOL KDesignation::Init(KPlayer* pPlayer)
{
	assert(pPlayer);

	m_pPlayer = pPlayer;

    m_nCurrentPrefix    = 0;
    m_nCurrentPostfix   = 0;
    m_nGenerationIndex  = 0;
    m_nBynameIndex      = 0;
    m_bDisplayFlag      = false;

	return true;
}

void KDesignation::UnInit()
{
    m_pPlayer = NULL;
    return;
}

BOOL KDesignation::Save(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize)
{
    BOOL            bResult             = false;
    BOOL            bRetCode            = false;
    KDesignationDB* pDBHeader           = NULL;
    BYTE*           pbyOffset           = pbyBuffer;
    size_t          uLeftSize           = uBufferSize;
    size_t          uDataLen            = 0;

    assert(puUsedSize);
    assert(pbyBuffer);

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KDesignationDB));
    pDBHeader = (KDesignationDB*)pbyOffset;

    pDBHeader->byCurrentPrefix      = (BYTE)m_nCurrentPrefix;
    pDBHeader->byCurrentPostfix     = (BYTE)m_nCurrentPostfix;
    pDBHeader->byGenerationIndex    = (BYTE)m_nGenerationIndex;
    pDBHeader->nBynameIndex         = m_nBynameIndex;
    pDBHeader->byDisplayFlag        = (BYTE)m_bDisplayFlag;

    pbyOffset += sizeof(KDesignationDB);
    uLeftSize -= sizeof(KDesignationDB);

    bRetCode = m_AcquiredPrefix.Save(&uDataLen, pbyOffset, uLeftSize);
    KGLOG_PROCESS_ERROR(bRetCode);
    pbyOffset += uDataLen;
    uLeftSize -= uDataLen;

    bRetCode = m_AcquiredPostfix.Save(&uDataLen, pbyOffset, uLeftSize);
    KGLOG_PROCESS_ERROR(bRetCode);
    pbyOffset += uDataLen;
    uLeftSize -= uDataLen;

    *puUsedSize = uBufferSize - uLeftSize;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KDesignation::Load(BYTE* pbyData, size_t uDataLen)
{
    BOOL            bResult             = false;
    BOOL            bRetCode            = false;
    KDesignationDB* pDBHeader           = NULL;
    BYTE*           pbyOffset           = pbyData;
    size_t          uLeftSize           = uDataLen;

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KDesignationDB));
    pDBHeader = (KDesignationDB*)pbyOffset;

    m_nCurrentPrefix    = pDBHeader->byCurrentPrefix;
    m_nCurrentPostfix   = pDBHeader->byCurrentPostfix;
    m_nGenerationIndex  = pDBHeader->byGenerationIndex;
    m_nBynameIndex      = pDBHeader->nBynameIndex;
    m_bDisplayFlag      = pDBHeader->byDisplayFlag;

    pbyOffset += sizeof(KDesignationDB);
    uLeftSize -= sizeof(KDesignationDB);

    KGLOG_PROCESS_ERROR(uLeftSize >= DESIGNATIONFIX_DATA_SIZE);
    bRetCode = m_AcquiredPrefix.Load(pbyOffset, DESIGNATIONFIX_DATA_SIZE);
    KGLOG_PROCESS_ERROR(bRetCode);
    pbyOffset += DESIGNATIONFIX_DATA_SIZE;
    uLeftSize -= DESIGNATIONFIX_DATA_SIZE;

    KGLOG_PROCESS_ERROR(uLeftSize == DESIGNATIONFIX_DATA_SIZE);
    bRetCode = m_AcquiredPostfix.Load(pbyOffset, DESIGNATIONFIX_DATA_SIZE);
    KGLOG_PROCESS_ERROR(bRetCode);
    pbyOffset += DESIGNATIONFIX_DATA_SIZE;
    uLeftSize -= DESIGNATIONFIX_DATA_SIZE;
    
    bResult = true;
Exit0:
    return bResult;
}

// 改变玩家当前用的称号及显示
BOOL KDesignation::SetCurrentDesignation(int nPrefix, int nPostfix, BOOL bDisplayFlag)
{
    BOOL bResult            = false;
    BOOL bRetCode           = false;
    int  nGenerationIndex   = 0;

    if (nPrefix != 0)
    {
        bRetCode = IsPrefixAcquired(nPrefix);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    if (nPostfix != 0)
    {
        bRetCode = IsPostfixAcquired(nPostfix);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    m_nCurrentPrefix   = nPrefix;
    m_nCurrentPostfix  = nPostfix;
    m_bDisplayFlag     = bDisplayFlag;

#ifdef _SERVER
    g_PlayerServer.DoSyncPlayerDesignation(
        m_pPlayer, m_nCurrentPrefix, m_nCurrentPostfix, 
        m_nGenerationIndex, m_nBynameIndex, m_bDisplayFlag
    );
#endif

    bResult = true;
Exit0:
    return bResult;
}

BOOL KDesignation::AcquirePrefix(int nPrefix)
{
    BOOL            bResult     = false;
    BOOL            bRetCode    = false;
    KPrefixInfo*    pPrefixInfo = NULL;

    KGLOG_PROCESS_ERROR(nPrefix > 0 && nPrefix <= MAX_DESIGNATION_FIX_ID);

    bRetCode = m_AcquiredPrefix.SetBit(nPrefix, true);
    KGLOG_PROCESS_ERROR(bRetCode);

#ifdef _SERVER
    g_PlayerServer.DoAcquireDesignation(m_pPlayer, nPrefix, 0);

    pPrefixInfo = g_pSO3World->m_Settings.m_DesignationList.GetPrefixInfo(nPrefix);
    KGLOG_PROCESS_ERROR(pPrefixInfo);

    BroadcastDesignationAnnounce(nPrefix, 0, pPrefixInfo->byAnnounceType);

    g_pSO3World->m_StatDataServer.UpdateDesignationStat(true, nPrefix);
#endif
    
    bResult = true;
Exit0:
    return bResult;
}

BOOL KDesignation::AcquirePostfix(int nPostfix)
{
    BOOL            bResult      = false;
    BOOL            bRetCode     = false;
    KPostfixInfo*   pPostfixInfo = NULL;

    KGLOG_PROCESS_ERROR(nPostfix > 0 && nPostfix <= MAX_DESIGNATION_FIX_ID);

    bRetCode = m_AcquiredPostfix.SetBit(nPostfix, true);
    KGLOG_PROCESS_ERROR(bRetCode);

#ifdef _SERVER
    g_PlayerServer.DoAcquireDesignation(m_pPlayer, 0, nPostfix);

    pPostfixInfo = g_pSO3World->m_Settings.m_DesignationList.GetPostfixInfo(nPostfix);
    KGLOG_PROCESS_ERROR(pPostfixInfo);

    BroadcastDesignationAnnounce(0, nPostfix, pPostfixInfo->byAnnounceType);

    g_pSO3World->m_StatDataServer.UpdateDesignationStat(false, nPostfix);
#endif
    
    bResult = true;
Exit0:
    return bResult;
}

BOOL KDesignation::RemovePrefix(int nPrefix)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    KGLOG_PROCESS_ERROR(nPrefix > 0 && nPrefix <= MAX_DESIGNATION_FIX_ID);

    if (m_nCurrentPrefix == nPrefix)
    {
        SetCurrentDesignation(0, m_nCurrentPostfix, m_bDisplayFlag);
    }

    bRetCode = m_AcquiredPrefix.SetBit(nPrefix, false);
    KGLOG_PROCESS_ERROR(bRetCode);

#ifdef _SERVER
    g_PlayerServer.DoRemoveDesignation(m_pPlayer, nPrefix, 0);
#endif
    
    bResult = true;
Exit0:
    return bResult;
}

BOOL KDesignation::RemovePostfix(int nPostfix)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    KGLOG_PROCESS_ERROR(nPostfix > 0 && nPostfix <= MAX_DESIGNATION_FIX_ID);

    if (m_nCurrentPostfix == nPostfix)
    {
        SetCurrentDesignation(m_nCurrentPrefix, 0, m_bDisplayFlag);
    }

    bRetCode = m_AcquiredPostfix.SetBit(nPostfix, false);
    KGLOG_PROCESS_ERROR(bRetCode);

#ifdef _SERVER
    g_PlayerServer.DoRemoveDesignation(m_pPlayer, 0, nPostfix);
#endif
    
    bResult = true;
Exit0:
    return bResult;
}

// 判断玩家是否获得某个称号
BOOL KDesignation::IsPrefixAcquired(int nPrefix)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    KGLOG_PROCESS_ERROR(nPrefix > 0 && nPrefix <= MAX_DESIGNATION_FIX_ID);

    bRetCode = m_AcquiredPrefix.GetBit(nPrefix, &bResult);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return bResult;
}

BOOL KDesignation::IsPostfixAcquired(int nPostfix)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    KGLOG_PROCESS_ERROR(nPostfix > 0 && nPostfix <= MAX_DESIGNATION_FIX_ID);

    bRetCode = m_AcquiredPostfix.GetBit(nPostfix, &bResult);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return bResult;
}

// 设置玩家门派辈份(包括辈份号和在门派里的名字)
#ifdef _SERVER
BOOL KDesignation::SetGeneration(int nGenerationIndex)
{
    BOOL bResult = false;

    m_nGenerationIndex  = nGenerationIndex;
    m_nBynameIndex = g_Random(g_pSO3World->m_Settings.m_ConstList.nMaxDesignationBynameIndex) + 1;

    g_PlayerServer.DoSetGenerationNotify(m_pPlayer, nGenerationIndex, m_nBynameIndex);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KDesignation::BroadcastDesignationAnnounce(int nPrefix, int nPostfix, BYTE byType)
{
    BOOL bResult = false;

    assert(m_pPlayer);

    if (byType == datGlobal)
    {
        g_RelayClient.DoDesignationGlobalAnnounceRequest(m_pPlayer->m_dwID, nPrefix, nPostfix);
    }
    else
    {
        g_PlayerServer.BroadcastDesignationAnnounce(m_pPlayer->m_dwID, m_pPlayer->m_szName, nPrefix, nPostfix, byType);
    }

    bResult = true;
Exit0:
    return bResult;
}
#endif // _SERVER
