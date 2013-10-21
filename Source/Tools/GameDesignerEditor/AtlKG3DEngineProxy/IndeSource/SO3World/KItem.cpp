#include "stdafx.h"
#include "KItem.h"
#include "KPlayer.h"
#include "KSO3World.h"
#include "KItemLib.h"
#include "Engine/KMemory.h"

#define MAX_ABRADE_RATE	1024

KItem::KItem()
{
    m_pBaseAttr             = NULL;
    m_pRequireAttr          = NULL;
    m_pMagicAttr            = NULL;
    m_pPermanentEnchant     = NULL;
    m_pTemporaryEnchant     = NULL;
}

KItem::~KItem()
{
    assert(m_pBaseAttr      == NULL);
    assert(m_pRequireAttr   == NULL);
    assert(m_pMagicAttr     == NULL);
}

BOOL KItem::Init()
{
	memset(&m_GenParam, 0, sizeof(m_GenParam));
	memset(&m_Common, 0, sizeof(m_Common));		

	m_pBaseAttr             = NULL;
	m_pRequireAttr          = NULL;
	m_pMagicAttr            = NULL;
    m_pPermanentEnchant     = NULL;
    m_pTemporaryEnchant     = NULL;
    m_nEnchantTime          = 0;

	m_dwScriptID            = 0;
	m_dwSkillID             = INVALID_SKILL_ID;
	m_dwSkillLevel          = 0;

	m_nCurrentDurability    = 0;
	m_nStackNum             = 0;

	m_nMaxDurability        = 0;
	m_nMaxStackNum          = 0;

	m_bBind                 = false;
    m_nRepresentID          = ERROR_ID;
    m_nColorID              = 0;
    m_bApplyed              = false;
	m_bEquiped              = false;

#ifdef _CLIENT
	m_nUiId                 = ERROR_ID;
#endif

    return true;
}

void KItem::UnInit()
{
	while (m_pBaseAttr)
	{
		KAttribute* pNext = m_pBaseAttr->pNext;
		KMemory::Delete(m_pBaseAttr);
		m_pBaseAttr = pNext;
	}

	while (m_pRequireAttr)
	{
		KAttribute* pNext = m_pRequireAttr->pNext;
		KMemory::Delete(m_pRequireAttr);
		m_pRequireAttr = pNext;
	}

	while (m_pMagicAttr)
	{
		KAttribute* pNext = m_pMagicAttr->pNext;
		KMemory::Delete(m_pMagicAttr);
		m_pMagicAttr = pNext;
	}

    m_pPermanentEnchant     = NULL;
    m_pTemporaryEnchant     = NULL;
    m_nEnchantTime          = 0;

	return;
}

BOOL KItem::SetStackNum(int nNum)
{
	assert(m_Common.bStack);

    if (nNum > m_nMaxStackNum)
    {
        KGLogPrintf(
            KGLOG_WARNING, "Set item(%u,%u) stack(%d) over max(%d)",
            m_GenParam.dwTabType, m_GenParam.dwIndex, nNum, m_nMaxStackNum
        );

        nNum = m_nMaxStackNum;
    }

	m_nStackNum = nNum;

	return true;
}

int KItem::GetStackNum()
{
	int nRet = 1;
	if (IsStackable())
	{
		nRet = m_nStackNum;
	}
	return nRet;
}

int KItem::GetMaxStackNum()
{
	int nRet = 1;
	if (IsStackable())
	{
		nRet = m_nMaxStackNum;
	}
	return nRet;
}

int KItem::GetMaxExistAmount()
{
    return m_Common.nMaxExistAmount;
}

int KItem::GetMaxExistTime()
{
	int         nResult     = 0;
	KItemInfo*  pItemInfo   = NULL;

	pItemInfo = GetItemInfo();
    KGLOG_PROCESS_ERROR(pItemInfo);
    
	nResult = pItemInfo->nMaxExistTime;
Exit0:
	return nResult;
}

BOOL KItem::IsStackable()
{	
    return m_Common.bStack;
}

BOOL KItem::IsRepairable()
{
	BOOL    bResult     = false;
    BOOL    bRetCode    = false;

	bRetCode = HaveDurability();
	KG_PROCESS_ERROR(bRetCode);

	KG_PROCESS_ERROR(m_nCurrentDurability < m_nMaxDurability);
    
    bResult = true;
Exit0:
	return bResult;
}

BOOL KItem::CanStack(KItem* pDest)
{
	BOOL bResult = false;

	assert(pDest);

	KG_PROCESS_ERROR(IsStackable() && pDest->IsStackable());
	KG_PROCESS_ERROR(m_nStackNum < m_nMaxStackNum);
	KG_PROCESS_ERROR(m_GenParam.dwTabType == pDest->m_GenParam.dwTabType);
	KG_PROCESS_ERROR(m_GenParam.dwIndex == pDest->m_GenParam.dwIndex);
	KG_PROCESS_ERROR(m_bBind == pDest->m_bBind);

	bResult = true;
Exit0:
	return bResult;
}

int KItem::Stack(int nCount)
{
	int nResult = nCount;

	KGLOG_PROCESS_ERROR(m_Common.bStack);

	if (nCount + m_nStackNum <= m_nMaxStackNum)
	{
		nResult     = 0;		
		m_nStackNum += nCount;
	}
	else
	{
		nResult     =  m_nStackNum + nCount - m_nMaxStackNum;
		m_nStackNum = m_nMaxStackNum;
	}

Exit0:
	return nResult;
}

BOOL KItem::SetCurrentDurability(int nDurability)
{
    BOOL    bResult     = false;
    BOOL    bRetCode    = false;

    KGLOG_PROCESS_ERROR(nDurability >= 0 && nDurability <= m_nMaxDurability);

	bRetCode = HaveDurability();
	KG_PROCESS_ERROR(bRetCode);

    m_nCurrentDurability = nDurability;

    bResult = true;
Exit0:
    return bResult;
}

// 返回-1代表该物品没有耐久度
int KItem::GetCurrentDurability()
{
    int     nResult     = -1;
    BOOL    bRetCode    = false;

    bRetCode = HaveDurability();
    KG_PROCESS_ERROR(bRetCode);

    nResult = m_nCurrentDurability;
Exit0:
    return nResult;
}

// 返回-1代表该物品没有耐久度
int KItem::GetMaxDurability()
{
    int     nResult     = -1;
    BOOL    bRetCode    = false;

    bRetCode = HaveDurability();
    KG_PROCESS_ERROR(bRetCode);

    nResult = m_nMaxDurability;
Exit0:
    return nResult;
}

BOOL KItem::Repair()
{
    BOOL bResult = false;

    KGLOG_PROCESS_ERROR(IsRepairable());

	m_nCurrentDurability = m_nMaxDurability;
    
    bResult = true;
Exit0:
	return bResult;
}

BOOL KItem::Abrade()
{
	BOOL bResult              = false;
	BOOL bRetCode             = false;
    int  nCurrentDurability   = 0;
    
    // 有耐久的东西才会掉耐久
    nCurrentDurability = GetCurrentDurability();
	KG_PROCESS_ERROR(nCurrentDurability > 0);

	bRetCode = ((int)g_Random(MAX_ABRADE_RATE) < m_Common.nAbradeRate);
	KG_PROCESS_ERROR(bRetCode);

	--m_nCurrentDurability;

	bResult = true;
Exit0:
	return bResult;
}

BOOL KItem::GetBinaryData(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize)
{
	BOOL    bResult     = false;
	return bResult;
}

BOOL KItem::CheckCoolDown(KPlayer* pPlayer)
{
    BOOL        bResult         = false;
    BOOL        bRetCode        = false;
    KItemInfo*  pItemInfo       = NULL;
    
    pItemInfo = GetItemInfo();
    KGLOG_PROCESS_ERROR(pItemInfo);
    
    if (pItemInfo->dwCoolDownID)
    {
        bRetCode = pPlayer->m_TimerList.CheckTimer(pItemInfo->dwCoolDownID);
        KG_PROCESS_ERROR(bRetCode);
    }
    
    bResult = true;
Exit0:
    return bResult;
}

BOOL KItem::HaveDurability()
{
    BOOL        bResult         = false;
    BOOL        bRetCode        = false;

    KG_PROCESS_ERROR(m_Common.nGenre == igEquipment);
    KG_PROCESS_ERROR(!m_Common.bStack);
	KG_PROCESS_ERROR(
        m_Common.nSub != estPackage && m_Common.nSub != estRing         && 
        m_Common.nSub != estAmulet  && m_Common.nSub != estWaistExtend  && 
        m_Common.nSub != estPendant && m_Common.nSub != estBackExtend   && 
        m_Common.nSub != estHorse
    );

    bResult = true;
Exit0:
    return bResult;
}

#ifdef _SERVER
BOOL KItem::ResetCoolDown(KPlayer* pPlayer)
{
    BOOL        bResult         = false;
    BOOL        bRetCode        = false;
    KItemInfo*  pItemInfo       = NULL;
    int         nDuration       = 0;

    pItemInfo = GetItemInfo();
    KGLOG_PROCESS_ERROR(pItemInfo);

    if (pItemInfo->dwCoolDownID)
    {
        nDuration = pPlayer->GetCDValue(pItemInfo->dwCoolDownID);

        pPlayer->m_TimerList.ResetTimer(pItemInfo->dwCoolDownID, nDuration);
    }

    bResult = true;
Exit0:
    return bResult;
}
#endif

int KItem::GetRequireLevel()
{
    int                 nResult             = 0;
    KItemInfo*          pItemInfo           = NULL;
    KCustomEquipInfo*   pCustomEquipInfo    = NULL;
    KOtherItemInfo*     pOtherItemInfo      = NULL;
    KAttribKey*         pAttribKey          = NULL;

    pItemInfo = GetItemInfo();
    KGLOG_PROCESS_ERROR(pItemInfo);

    switch (m_GenParam.dwTabType)
    {
    case ittCustWeapon:
    case ittCustArmor:
    case ittCustTrinket:
        pCustomEquipInfo = (KCustomEquipInfo*)pItemInfo;
        for (int i = 0; i < MAX_ITEM_REQUIRE_COUNT; i++)
        {
            if (pCustomEquipInfo->RequireAttrib[i].nAttribID == rqtLevel)
            {
                nResult = pCustomEquipInfo->RequireAttrib[i].nValue;
                break;
            }
        }
        break;

    case ittOther:
        pOtherItemInfo = (KOtherItemInfo*)pItemInfo;
        nResult = pOtherItemInfo->nRequireLevel;
        break;

    default:
        KGLOG_PROCESS_ERROR(false);
        break;
    }
    
    if (nResult < 1)
    {
        nResult = 1;
    }

Exit0:
    return nResult;
}

KItemInfo* KItem::GetItemInfo()
{
    KItemInfo* pItemInfo = NULL;
    return pItemInfo;
}
