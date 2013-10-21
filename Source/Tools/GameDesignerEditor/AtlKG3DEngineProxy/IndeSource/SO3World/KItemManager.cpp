#include "stdafx.h"
#include "KSO3World.h"
#include "KItemLib.h"
#include "Engine/KMemory.h"

KItemManager::KItemManager()
{
	m_pItemLib = NULL;
}

KItemManager::~KItemManager()
{
	assert(!m_pItemLib);
}

BOOL KItemManager::Init()
{
	BOOL bResult                = false;
	BOOL bRetCode               = false;
    BOOL bEnchantLibInitFlag    = false;
    int  nItemLibInitCount      = 0;

	m_pItemLib = new KItemLib[CURRENT_ITEM_VERSION + 1];
	KGLOG_PROCESS_ERROR(m_pItemLib);

	for (int i = 0; i < CURRENT_ITEM_VERSION + 1; i++)
	{
		bRetCode = m_pItemLib[i].Init(i);
		KGLOG_PROCESS_ERROR(bRetCode);
        ++nItemLibInitCount;
	}

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
        
        for (int i = 0; i < nItemLibInitCount; i++)
	    {
		    m_pItemLib[i].UnInit();
	    }

        KG_DELETE_ARRAY(m_pItemLib);
    }
	return bResult;
}

void KItemManager::UnInit()
{
    m_EnchantLib.UnInit();

	if (m_pItemLib)
	{
		for (int i = 0; i < CURRENT_ITEM_VERSION + 1; i++)
		{
			m_pItemLib[i].UnInit();
		}

		KG_DELETE_ARRAY(m_pItemLib);
	}
}

KItem* KItemManager::GenerateItem(
    int     nVersion, 
    DWORD   dwTabType, 
    DWORD   dwTabIndex, 
    DWORD   dwId/* = 0*/, 
    DWORD   dwRandSeed/* = 0 */, 
    int     nMagicNum/* = 0 */,
    int     nQuality/* = 0 */, 
    int     nMagic[]/* = NULL */, 
    time_t  nGenTime/* = 0*/
)
{
    KItem*  pResult         = NULL;
	BOOL    bRetCode        = false;
	KItem*  pItem           = NULL;
    BOOL    bRegisterFlag   = false;

	KGLOG_PROCESS_ERROR(nVersion <= CURRENT_ITEM_VERSION);

    pItem = KMemory::New<KItem>();
    KGLOG_PROCESS_ERROR(pItem);

    bRetCode = g_pSO3World->m_ItemSet.Register(pItem, dwId);
    KGLOG_PROCESS_ERROR(bRetCode);
    bRegisterFlag = true;

    bRetCode = GenerateItemTo(pItem, nVersion, dwTabType, dwTabIndex, dwId, dwRandSeed, nMagicNum, nQuality, nMagic, nGenTime);
	if (!bRetCode)
    {
        KGLogPrintf(KGLOG_INFO, "Failed to generate item(%d, %u, %u) !\n", nVersion, dwTabType, dwTabIndex);
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
    int     nVersion, 
    DWORD   dwTabType, 
    DWORD   dwTabIndex, 
    DWORD   dwId /* = ERROR_ID */, 
    DWORD   dwRandSeed /* = 0 */, 
    int     nMagicNum /* = 0 */, 
    int     nQuality /* = 0 */, 
    int     nMagic[] /* = NULL */, 
    time_t  nGenTime /* = 0 */
)
{
    BOOL bResult    = false;
	BOOL bRetCode   = false;
    BOOL bInitFlag  = false;

	assert(pDestItem);
	KGLOG_PROCESS_ERROR(nVersion >= 0 && nVersion <= CURRENT_ITEM_VERSION);

	bRetCode = pDestItem->Init();
	KGLOG_PROCESS_ERROR(bRetCode);
    bInitFlag = true;

	bRetCode = m_pItemLib[nVersion].GenerateItem(
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
	return NULL;
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

    pNewItem->m_pPermanentEnchant     = pItem->m_pPermanentEnchant;
    pNewItem->m_pTemporaryEnchant     = pItem->m_pTemporaryEnchant;
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

KItemInfo* KItemManager::GetItemInfo(int nVersion, DWORD dwTabType, DWORD dwIndex)
{   
    KItemInfo* pRetItemInfo = NULL;
    
	KGLOG_PROCESS_ERROR(nVersion >= 0 && nVersion <= CURRENT_ITEM_VERSION);

	pRetItemInfo = m_pItemLib[nVersion].GetItemInfo(dwTabType, dwIndex);
    
Exit0:
    if (pRetItemInfo == NULL)
    {
        KGLogPrintf(
            KGLOG_ERR, 
            "Get ItemInfo failed! nVersion : %d, dwTabType : %d, dwTabIndex : %d\n",
            nVersion, dwTabType, dwIndex
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

	pSetInfo = m_pItemLib[CURRENT_ITEM_VERSION].GetSetInfo(dwSetID);
	KGLOG_PROCESS_ERROR(pSetInfo);

	nAttribIndex = pSetInfo->dwAttribID[nSetNum - 2];	//数组下标要减1，然后两件套是第1项，还要减1
    
    KG_PROCESS_ERROR(nAttribIndex != 0);

	pAttrInfo = m_pItemLib[CURRENT_ITEM_VERSION].GetAttribInfo(nAttribIndex);
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
