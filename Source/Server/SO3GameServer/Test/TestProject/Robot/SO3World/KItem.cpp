#include "stdafx.h"
#include "KItem.h"
#include "KPlayer.h"
#include "KObjectManager.h"
#include "KItemLib.h"

extern KItemManager g_ItemLib;

KItem::KItem(void)
{
	memset(&m_GenParam, 0, sizeof(m_GenParam));
	memset(&m_Common, 0, sizeof(m_Common));				

	m_nCurrentDurability = 0;
	m_nStackNum = 0;

	m_nMaxDurability = 0;
	m_nMaxStackNum = 0;

	m_bBound = 0;
	m_bApplyed = FALSE;
	m_bEquiped = FALSE;

	m_dwScriptID = 0;
	m_dwSkillID = INVALID_SKILL_ID;
	m_dwSkillLevel = 0;

	m_nRepresentId = ERROR_ID;
    m_pBaseAttr    = NULL;
    m_pRequireAttr = NULL;
    m_pMagicAttr   = NULL;
    m_pEnchant     = NULL;
    m_pTemporaryEnchant = NULL;
    m_nTemporaryEnchantTime = 0;

#ifdef _CLIENT
	m_nUiId = ERROR_ID;
#endif
}

KItem::~KItem(void)
{
	UnInit();
}

BOOL KItem::Init(void)
{
	memset(&m_GenParam, 0, sizeof(m_GenParam));
	memset(&m_Common, 0, sizeof(m_Common));				

	m_pBaseAttr = NULL;
	m_pRequireAttr = NULL;
	m_pMagicAttr = NULL;
	m_pEnchant = NULL;
	m_pTemporaryEnchant = NULL;
	m_nTemporaryEnchantTime = 0;

	m_dwScriptID = 0;
	m_dwSkillID = INVALID_SKILL_ID;
	m_dwSkillLevel = 0;

	m_nCurrentDurability = 0;
	m_nStackNum = 0;

	m_nMaxDurability = 0;
	m_nMaxStackNum = 0;

	m_bBound = 0;
	m_bApplyed = FALSE;
	m_bEquiped = FALSE;

	m_nRepresentId = ERROR_ID;
#ifdef _CLIENT
	m_nUiId = ERROR_ID;
#endif

	return TRUE;
}

BOOL KItem::UnInit(void)
{
	while (m_pBaseAttr)
	{
		KAttribute* pNext = m_pBaseAttr->pNext;
		g_ObjectManager.m_AttributeSet.Delete(m_pBaseAttr);
		m_pBaseAttr = pNext;
	}

	while (m_pRequireAttr)
	{
		KAttribute* pNext = m_pRequireAttr->pNext;
		g_ObjectManager.m_AttributeSet.Delete(m_pRequireAttr);
		m_pRequireAttr = pNext;
	}

	while (m_pMagicAttr)
	{
		KAttribute* pNext = m_pMagicAttr->pNext;
		g_ObjectManager.m_AttributeSet.Delete(m_pMagicAttr);
		m_pMagicAttr = pNext;
	}

	while (m_pEnchant)
	{
		KAttribute* pNext = m_pEnchant->pNext;
		g_ObjectManager.m_AttributeSet.Delete(m_pEnchant);
		m_pEnchant = pNext;
	}

	while (m_pTemporaryEnchant)
	{
		KAttribute* pNext = m_pTemporaryEnchant->pNext;
		g_ObjectManager.m_AttributeSet.Delete(m_pTemporaryEnchant);
		m_pTemporaryEnchant = pNext;
	}

	return TRUE;
}

BOOL KItem::SetStackNum(int nNum)
{
	BOOL bResult = FALSE;

	KG_ASSERT_EXIT(IsStackable());
	KGLOG_PROCESS_ERROR(nNum <= m_nMaxStackNum);

	m_nStackNum = nNum;
	bResult = TRUE;
Exit0:
	return bResult;
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
	int nResult = 0;
	int nType = 0;
	KItemInfo* pItemInfo = NULL;

	pItemInfo = g_ItemLib.GetItemInfo(CURRENT_ITEM_VERSION, m_GenParam.dwTabType, 
		m_GenParam.dwIndex, nType);
	KGLOG_PROCESS_ERROR(pItemInfo);

	nResult = pItemInfo->nMaxExistAmount;

Exit0:
	return nResult;
}

int KItem::GetMaxExistTime()
{
	int nResult = 0;
	int nType = 0;
	KItemInfo* pItemInfo = NULL;

	pItemInfo = g_ItemLib.GetItemInfo(CURRENT_ITEM_VERSION, m_GenParam.dwTabType, 
		m_GenParam.dwIndex, nType);
	KGLOG_PROCESS_ERROR(pItemInfo);

	nResult = pItemInfo->nMaxExistTime;

Exit0:
	return nResult;
}

BOOL KItem::IsStackable()
{
	BOOL bResult = FALSE;
	KG_PROCESS_ERROR(m_Common.bStack != FALSE);

	bResult = TRUE;
Exit0:
	return bResult;
}

BOOL KItem::IsRepairable()
{
	BOOL bResult = FALSE;

	KG_PROCESS_ERROR(m_Common.nGenre == igEquipment);
	KG_PROCESS_ERROR(m_Common.bStack == FALSE);
	KG_PROCESS_ERROR(m_nCurrentDurability < m_nMaxDurability);
    
    bResult = TRUE;
Exit0:
	return bResult;
}

BOOL KItem::CanStack(KItem* pDest)
{
	BOOL bResult = FALSE;

	KGLOG_PROCESS_ERROR(pDest);
	KG_PROCESS_ERROR(IsStackable() && pDest->IsStackable());
	KG_PROCESS_ERROR(m_nStackNum < m_nMaxStackNum);
	KG_PROCESS_ERROR(m_GenParam.dwTabType == pDest->m_GenParam.dwTabType);
	KG_PROCESS_ERROR(m_GenParam.dwIndex == pDest->m_GenParam.dwIndex);
	KG_PROCESS_ERROR(m_bBound == pDest->m_bBound);

	bResult = TRUE;
Exit0:
	return bResult;
}

int KItem::Stack(int nCount)
{
	int nResult = nCount;

	// 装备不能叠放
	KG_PROCESS_ERROR(IsStackable());

	if (nCount + m_nStackNum <= m_nMaxStackNum)
	{
		nResult = 0;		
		m_nStackNum += nCount;
	}
	else
	{
		nResult = nCount - m_nMaxStackNum + m_nStackNum;
		m_nStackNum = m_nMaxStackNum;
	}

Exit0:
	return nResult;
}

BOOL KItem::SetCurrentDurability(int nDurability)
{
    BOOL bResult = FALSE;

	KGLOG_PROCESS_ERROR(m_Common.nGenre == igEquipment);
	KGLOG_PROCESS_ERROR(m_Common.bStack == FALSE);
    KGLOG_PROCESS_ERROR(nDurability >= 0);
	KGLOG_PROCESS_ERROR(nDurability <= m_nMaxDurability);

    m_nCurrentDurability = nDurability;

    bResult = TRUE;
Exit0:
    return bResult;
}

// 返回-1代表该物品没有耐久度
int KItem::GetCurrentDurability()
{
    int nResult = -1;

    KGLOG_PROCESS_ERROR(m_Common.nGenre == igEquipment);
    KGLOG_PROCESS_ERROR(m_Common.bStack == FALSE);
	KG_PROCESS_ERROR(m_Common.nSub != estPackage);
	KG_PROCESS_ERROR(m_Common.nSub != estRing);
	KG_PROCESS_ERROR(m_Common.nSub != estAmulet);
	KG_PROCESS_ERROR(m_Common.nSub != estExtend);
	KG_PROCESS_ERROR(m_Common.nSub != estPendant);
    
    nResult = m_nCurrentDurability;

Exit0:
    return nResult;
}

// 返回-1代表该物品没有耐久度
int KItem::GetMaxDurability()
{
    int nResult = -1;

    KGLOG_PROCESS_ERROR(m_Common.nGenre == igEquipment);
    KGLOG_PROCESS_ERROR(m_Common.bStack == FALSE);
	KG_PROCESS_ERROR(m_Common.nSub != estPackage);
	KG_PROCESS_ERROR(m_Common.nSub != estRing);
	KG_PROCESS_ERROR(m_Common.nSub != estAmulet);
	KG_PROCESS_ERROR(m_Common.nSub != estExtend);
	KG_PROCESS_ERROR(m_Common.nSub != estPendant);

    nResult = m_nMaxDurability;

Exit0:
    return nResult;
}

int KItem::Repair()
{
	int nResult = 0;
	KGLOG_PROCESS_ERROR(IsRepairable());

	nResult = m_nMaxDurability - m_nCurrentDurability;
	m_nCurrentDurability = m_nMaxDurability;
Exit0:
	return nResult;
}

BOOL KItem::Abrade()
{
	BOOL bResult = FALSE;
	BOOL bRetCode = FALSE;

	// 是可修复的东西才会掉耐久
	KG_PROCESS_ERROR(m_Common.nGenre == igEquipment);
	KG_PROCESS_ERROR(m_Common.bStack == FALSE);

	// 有耐久的东西才会掉耐久
	KG_PROCESS_ERROR(GetCurrentDurability() > 0);

#define MAX_ABRADE_RATE	1024
	// 是否随机到磨损了
	bRetCode = ((int)g_Random(MAX_ABRADE_RATE) < m_Common.nAbradeRate);
	KG_PROCESS_ERROR(bRetCode);

	m_nCurrentDurability--;

	bResult = TRUE;
Exit0:
	return bResult;
}