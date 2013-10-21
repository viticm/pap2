#include "stdafx.h"
#include "KSO3World.h"
#include "KItemLib.h"
#include "Engine/KMemory.h"

KAttribute* CloneAttributeList(const KAttribute* pHead);

BOOL KItemManager::Init()
{
	BOOL bResult                = false;
	BOOL bRetCode               = false;
    BOOL bItemLibInitFlag       = false;
    BOOL bEnchantLibInitFlag    = false;
    
    bRetCode = m_ItemLib.Init();
	KGLOG_PROCESS_ERROR(bRetCode);
    bItemLibInitFlag = true;
    
    bRetCode = m_EnchantLib.Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bEnchantLibInitFlag = true;

	bResult = true;
Exit0:
    if (!bResult)
    {
        if (bEnchantLibInitFlag)
        {
            m_EnchantLib.UnInit();
            bEnchantLibInitFlag = false;
        }
        
        if (bItemLibInitFlag)
        {
            m_ItemLib.UnInit();
            bItemLibInitFlag = false;
        }
    }
	return bResult;
}

void KItemManager::UnInit()
{
    m_EnchantLib.UnInit();
    m_ItemLib.UnInit();
}

KItem* KItemManager::GenerateItem(
    DWORD   dwTabType, 
    DWORD   dwTabIndex, 
    time_t  nGenTime,
    DWORD   dwId/* = 0*/, 
    DWORD   dwRandSeed/* = 0 */, 
    int     nMagicNum/* = 0 */,
    int     nQuality/* = 0 */, 
    int     nMagic[]/* = NULL */
)
{
    KItem*  pResult         = NULL;
	BOOL    bRetCode        = false;
	KItem*  pItem           = NULL;
    BOOL    bRegisterFlag   = false;

    pItem = KMemory::New<KItem>();
    KGLOG_PROCESS_ERROR(pItem);

    bRetCode = g_pSO3World->m_ItemSet.Register(pItem, dwId);
    KGLOG_PROCESS_ERROR(bRetCode);
    bRegisterFlag = true;

    bRetCode = GenerateItemTo(
        pItem, dwTabType, dwTabIndex, nGenTime, dwId, dwRandSeed, nMagicNum, nQuality, nMagic
    );
	if (!bRetCode)
    {
        KGLogPrintf(KGLOG_INFO, "Failed to generate item(%u, %u) !\n", dwTabType, dwTabIndex);
        goto Exit0;
    }

	pResult = pItem;
Exit0:
    if (pResult == NULL)
    {
        if (bRegisterFlag)
        {
            g_pSO3World->m_ItemSet.Unregister(pItem);
            bRegisterFlag = false;
        }

	    if (pItem)
	    {
    	    KMemory::Delete(pItem);
            pItem = NULL;
	    }
    }
	return pResult;
}

BOOL KItemManager::GenerateItemTo(
    KItem*  pDestItem, 
    DWORD   dwTabType, 
    DWORD   dwTabIndex,
    time_t  nGenTime,
    DWORD   dwId /* = ERROR_ID */, 
    DWORD   dwRandSeed /* = 0 */, 
    int     nMagicNum /* = 0 */, 
    int     nQuality /* = 0 */, 
    int     nMagic[] /* = NULL */
)
{
    BOOL bResult    = false;
	BOOL bRetCode   = false;
    BOOL bInitFlag  = false;

	assert(pDestItem);

	bRetCode = pDestItem->Init();
	KGLOG_PROCESS_ERROR(bRetCode);
    bInitFlag = true;

	bRetCode = m_ItemLib.GenerateItem(
        pDestItem, dwTabType, dwTabIndex, dwId, dwRandSeed,
		nMagicNum, nQuality,  nMagic,     nGenTime
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
    if (!bResult)
    {
        if (bInitFlag)
        {
            pDestItem->UnInit();
            bInitFlag = false;
        }
    }
	return bResult;
}

KItem* KItemManager::GenerateItemFromBinaryData(DWORD dwItemID, void* pvBuffer, size_t uBufferSize)
{
    KItem*              pResult         = NULL;
    BOOL                bRetCode        = false;
    KItem*              pItem           = NULL;
    BOOL                bRegisterFlag   = false;
    KCOMMON_ITEM_DATA*  pComm           = (KCOMMON_ITEM_DATA*)pvBuffer;
    KENCHANT*           pEnchant        = NULL;

    KGLOG_PROCESS_ERROR(uBufferSize >= sizeof(KCOMMON_ITEM_DATA));

    pItem = KMemory::New<KItem>();
	KGLOG_PROCESS_ERROR(pItem);

    bRetCode = g_pSO3World->m_ItemSet.Register(pItem, dwItemID);
    KGLOG_PROCESS_ERROR(bRetCode);
    bRegisterFlag = true;

	switch (pComm->byTabType)
	{
    case ittOther:
		{
            KCOMMON_ITEM_DATA* pCid = (KCOMMON_ITEM_DATA*)pvBuffer;
			KGLOG_PROCESS_ERROR(uBufferSize == sizeof(KCOMMON_ITEM_DATA));
			
			bRetCode = GenerateItemTo(
                pItem, pCid->byTabType, pCid->wTabIndex, pCid->nGenTime,
                pItem->m_dwID, 0, 0, 0, NULL
            );
			KGLOG_PROCESS_ERROR(bRetCode);
            
            if (pItem->m_Common.bStack)
            {
                pItem->SetStackNum(pCid->wDurability);
            }
            else
            {
                pItem->m_nCurrentDurability = pCid->wDurability;
            }

            pItem->m_bBind  = pCid->byBind;
		}
		break;

    case ittCustArmor:
    case ittCustWeapon:
    case ittCustTrinket:
		{
            KCUSTOM_EQUI_DATA* pCed = (KCUSTOM_EQUI_DATA*)pvBuffer;
            KGLOG_PROCESS_ERROR(uBufferSize == sizeof(KCUSTOM_EQUI_DATA));
            
			bRetCode = GenerateItemTo(
                pItem, pCed->byTabType, pCed->wTabIndex, pCed->nGenTime,
                pItem->m_dwID, pCed->dwRandSeed, 0, 0, NULL
            );
			KGLOG_PROCESS_ERROR(bRetCode);

			pItem->m_nCurrentDurability = pCed->wDurability;
            pItem->m_bBind              = pCed->byBind;
            pItem->m_nColorID           = pCed->byColorID;

            pItem->m_dwEnchantID[eiPermanentEnchant]  = pCed->wEnchant[eiPermanentEnchant];
            pItem->m_dwEnchantID[eiTemporaryEnchant]  = 0;
            pItem->m_dwEnchantID[eiMount1] = pCed->wEnchant[eiMount1];
            pItem->m_dwEnchantID[eiMount2] = pCed->wEnchant[eiMount2];
            pItem->m_dwEnchantID[eiMount3] = pCed->wEnchant[eiMount3];
            pItem->m_dwEnchantID[eiMount4] = pCed->wEnchant[eiMount4];
            pItem->m_nEnchantTime          = 0;
            if (pCed->wLeftEnchantTime > 0)
            {
                pItem->m_dwEnchantID[eiTemporaryEnchant]  = pCed->wEnchant[eiTemporaryEnchant];
                pItem->m_nEnchantTime = pCed->wLeftEnchantTime + g_pSO3World->m_nCurrentTime;
            }
		}
		break;

	default:
        goto Exit0;
	}
    
    pResult = pItem;
Exit0:
    if (!pResult)
    {
        KGLogPrintf(KGLOG_ERR, "[ITEM] GenerateItemFromBinaryData failed, Tab: %d, Index: %d. \n", (int)pComm->byTabType, (int)pComm->wTabIndex);

        if (bRegisterFlag)
        {
            g_pSO3World->m_ItemSet.Unregister(pItem);
            bRegisterFlag = false;
        }

        if (pItem)
        {
            KMemory::Delete(pItem);
            pItem = NULL;
        }
    }
	return pResult;
}

KItem* KItemManager::CloneItem(const KItem* pItem)
{
    KItem*  pResult             = NULL;
	BOOL    bRetCode            = false;
	KItem*  pNewItem            = NULL;
    BOOL    bNewItemRegister    = false;
    BOOL    bNewItemInit        = false;

	assert(pItem);
    assert(g_pSO3World);

    pNewItem = KMemory::New<KItem>();
    KGLOG_PROCESS_ERROR(pNewItem);

	bRetCode = g_pSO3World->m_ItemSet.Register(pNewItem, ERROR_ID);
	KGLOG_PROCESS_ERROR(bRetCode);
	bNewItemRegister = true;

	bRetCode = pNewItem->Init();
	KGLOG_PROCESS_ERROR(bRetCode);
    bNewItemInit = true;
    
    pNewItem->m_pItemInfo             = pItem->m_pItemInfo;
    pNewItem->m_GenParam              = pItem->m_GenParam;
    pNewItem->m_Common                = pItem->m_Common;
    pNewItem->m_dwSkillID             = pItem->m_dwSkillID;
    pNewItem->m_dwSkillLevel          = pItem->m_dwSkillLevel;
    pNewItem->m_dwScriptID            = pItem->m_dwScriptID;
	pNewItem->m_nStackNum			  = pItem->m_nStackNum;		// union, 小心
	pNewItem->m_nMaxStackNum		  = pItem->m_nMaxStackNum;	// union, 小心
    pNewItem->m_bBind                 = pItem->m_bBind;
    pNewItem->m_nRepresentID          = pItem->m_nRepresentID;
    pNewItem->m_nColorID              = pItem->m_nColorID;
    pNewItem->m_bApplyed              = pItem->m_bApplyed;
    pNewItem->m_bEquiped              = pItem->m_bEquiped;

    pNewItem->m_pBaseAttr             = CloneAttributeList(pItem->m_pBaseAttr);
    pNewItem->m_pRequireAttr          = CloneAttributeList(pItem->m_pRequireAttr);
    pNewItem->m_pMagicAttr            = CloneAttributeList(pItem->m_pMagicAttr);

    memcpy(pNewItem->m_dwEnchantID, pItem->m_dwEnchantID, sizeof(pNewItem->m_dwEnchantID));
    pNewItem->m_nEnchantTime          = pItem->m_nEnchantTime;

#if defined(_CLIENT)
    pNewItem->m_nUiId                 = pItem->m_nUiId;
#endif
    
	pNewItem->m_GenParam.nGenTime     = g_pSO3World->m_nCurrentTime;

	pResult = pNewItem;
Exit0:
    if (pResult == NULL)
    {
		if (bNewItemInit)
		{
			pNewItem->UnInit();
			bNewItemInit = false;
		}

        if (bNewItemRegister)
        {
            g_pSO3World->m_ItemSet.Unregister(pNewItem);
            bNewItemRegister = false;
        }

	    if (pNewItem)
	    {
		    KMemory::Delete(pNewItem);
            pNewItem = NULL;
	    }
    }
	return pResult;
}

KItemInfo* KItemManager::GetItemInfo(DWORD dwTabType, DWORD dwIndex)
{   
    KItemInfo* pRetItemInfo = NULL;
    
	pRetItemInfo = m_ItemLib.GetItemInfo(dwTabType, dwIndex);
Exit0:
    if (pRetItemInfo == NULL)
    {
        KGLogPrintf(
            KGLOG_ERR, 
            "Get ItemInfo failed! dwTabType : %d, dwTabIndex : %d\n",
            dwTabType, dwIndex
        );
    }
	return pRetItemInfo;
}

BOOL KItemManager::GetSetAttrib(KAttribute& Attr, DWORD dwSetID, int nSetNum)
{
	BOOL            bResult         = false;
    KSetInfo*       pSetInfo        = NULL;
    int             nAttribIndex    = 0;
    KAttribInfo*    pAttrInfo      = NULL;

	KG_PROCESS_ERROR(nSetNum >= 2);							//至少是两件套才有属性加成

	KGLOG_PROCESS_ERROR(nSetNum <= MAX_SET_PART_NUM);

	pSetInfo = m_ItemLib.GetSetInfo(dwSetID);
	KGLOG_PROCESS_ERROR(pSetInfo);

	nAttribIndex = pSetInfo->dwAttribID[nSetNum - 2];	//数组下标要减1，然后两件套是第1项，还要减1
    
    KG_PROCESS_ERROR(nAttribIndex != 0);

	pAttrInfo = m_ItemLib.GetAttribInfo(nAttribIndex);
	KGLOG_PROCESS_ERROR(pAttrInfo);

	Attr.nKey       = pAttrInfo->nAttribID;
	Attr.nValue1    = pAttrInfo->Param[0].nMin;
	Attr.nValue2    = pAttrInfo->Param[1].nMin;
	Attr.pNext      = NULL;

	bResult = true;
Exit0:
	return bResult;
}

BOOL KItemManager::FreeItem(KItem* pItem)
{
	assert(pItem);

	pItem->UnInit();

	if (pItem->m_dwID != ERROR_ID)
	{
		g_pSO3World->m_ItemSet.Unregister(pItem);
	}

	KMemory::Delete(pItem);
    pItem = NULL;

	return true;
}

KAttribute* CloneAttributeList(const KAttribute* pHead)
{
    KAttribute* pNewHead = NULL;
    KAttribute* pNewTail = NULL;

    while (pHead)
    {
        KAttribute* pNewNode = KMemory::New<KAttribute>();
        KGLOG_PROCESS_ERROR(pNewNode);

        pNewNode->nKey      = pHead->nKey;
        pNewNode->nValue1   = pHead->nValue1;
        pNewNode->nValue2   = pHead->nValue2;
        pNewNode->pNext     = NULL;

        if (pNewHead == NULL)
        {
            pNewHead = pNewNode;
            pNewTail = pNewHead;
        }
        else
        {
            pNewTail->pNext = pNewNode;
            pNewTail = pNewTail->pNext;
        }

        pHead = pHead->pNext;
    }

Exit0:
    return pNewHead;
}
