#include "stdafx.h"
#include <vector>
#include "Common/CRC32.h"
#include "KMath.h"
#include "KItem.h"
#include "StrMap.h"
#include "KCharacter.h"
#include "KSceneObject.h"
#include "KNpc.h"
#include "KSO3World.h"
#include "KShop.h"
#include "KItemLib.h"

using namespace std;

#define NPC_SHOP_CONFIG_FILE          "NpcShopList.tab"
#define REPUTATION_REBATE_CONFIG_FILE "ReputationRebate.tab"

#define NPC_SHOP_REFRESH_CYCLE    (10 * 60 * 60 * GAME_FPS)

BOOL KShopCenter::Init()
{
    BOOL        bResult     = false;
    int         nRetCode    = false;

#ifdef _SERVER
    nRetCode = LoadNpcShopTemplates();
    KGLOG_PROCESS_ERROR(nRetCode);

    m_nNextRefreshTime = g_pSO3World->m_nGameLoop + NPC_SHOP_REFRESH_CYCLE;
    m_dwAllocID = 1;
#endif

    m_ShopList.clear();
    m_ReputationRebateMap.clear();
    
    nRetCode = LoadReputationRebate();
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
#ifdef _SERVER
    if (!bResult)
    {
        m_NpcShopTemplateTable.clear();
    }
#endif
    return bResult;
}

void KShopCenter::UnInit()
{
    KSHOP_LIST::iterator ShopIterator;

    for (ShopIterator = m_ShopList.begin(); ShopIterator != m_ShopList.end(); ++ShopIterator)
    {
        KShop *pShop = &ShopIterator->second;
        KItem *pItem = NULL;
        
        for (int nPageIndex = 0; nPageIndex < MAX_SHOP_PAGES; ++nPageIndex)
        {
            for (int nPosIndex = 0; nPosIndex < MAX_SHOP_PAGE_ITEM_COUNT; ++nPosIndex)
            {
                pItem = pShop->m_ShopPages[nPageIndex].ShopItems[nPosIndex].pItem;
                if (!pItem)
                    continue;
                g_pSO3World->m_ItemManager.FreeItem(pItem);
                pShop->m_ShopPages[nPageIndex].ShopItems[nPosIndex].pItem = NULL;
            }
        }

        if (pShop->m_pNpc)
        {
            pShop->m_pNpc->m_pShop = NULL;
            pShop->m_pNpc = NULL;
            pShop->m_dwNpcID = 0;
        }
    }

    m_ReputationRebateMap.clear();
    m_ShopList.clear();

#ifdef _SERVER
    m_NpcShopTemplateTable.clear();
#endif
}

KShop* KShopCenter::GetShopInfo(DWORD dwShopID)
{
    KShop* pShop = NULL;
    KSHOP_LIST::iterator it;

    it = m_ShopList.find(dwShopID);
    KG_PROCESS_ERROR(it != m_ShopList.end());

    pShop = &it->second;
Exit0:
    return pShop;
}

#ifdef _SERVER
void KShopCenter::Activate()
{
    if (g_pSO3World->m_nGameLoop >= m_nNextRefreshTime) 
    {
        DoRefresh();
        m_nNextRefreshTime = g_pSO3World->m_nGameLoop + NPC_SHOP_REFRESH_CYCLE;
    }
}

BOOL KShopCenter::BindNpcShop(KNpc* pNpc, DWORD dwTemplateID)
{
    BOOL                bResult         = false;
    BOOL                bRetCode        = false;
    KNPC_SHOP_TEMPLATE* pShopTemplate   = NULL;
    int                 nPageIndex      = 0;
    int                 nPosIndex       = 0;
    KShop*              pNewShop        = NULL;
    KItem*              pItem           = NULL;
    KNPC_SHOP_TEMPLATE_TABLE::iterator it;
    pair<KSHOP_LIST::iterator, bool>   InsRet;

    assert(pNpc);
    KGLOG_PROCESS_ERROR(!pNpc->m_pShop);

    it = m_NpcShopTemplateTable.find(dwTemplateID);
    if (it == m_NpcShopTemplateTable.end())
    {
        KGLogPrintf(KGLOG_ERR, "[Shop] Invalid shop template %u, NPC: %s\n", dwTemplateID, pNpc->m_szName);
        goto Exit0;
    }

    pShopTemplate = &it->second;
    KGLOG_PROCESS_ERROR(pShopTemplate);
    
    InsRet = m_ShopList.insert(make_pair(++m_dwAllocID, KShop()));
    KGLOG_PROCESS_ERROR(InsRet.second);

    pNewShop = &(InsRet.first->second);
    pNewShop->m_dwShopID = InsRet.first->first;

    for (size_t i = 0; i < pShopTemplate->vecTemplateItems.size(); ++i)
    {
        KNPC_SHOP_TEMPLATE_ITEM* pTemplateItem  = NULL;
        KSHOP_ITEM*              pShopItem      = NULL;
        
        if (nPosIndex >= MAX_SHOP_PAGE_ITEM_COUNT)
        {
            nPosIndex = 0;
            ++nPageIndex;
        }

        if (nPageIndex >= MAX_SHOP_PAGES)
        {
            KGLogPrintf(KGLOG_ERR, "[Shop] Too many items in shop template %u !\n", dwTemplateID);
            goto Exit0;
        }
        
        pShopItem = &pNewShop->m_ShopPages[nPageIndex].ShopItems[nPosIndex++];
        
        memset(pShopItem, 0, sizeof(KSHOP_ITEM));

		pTemplateItem = &pShopTemplate->vecTemplateItems[i];

        pItem = g_pSO3World->m_ItemManager.GenerateItem(
            CURRENT_ITEM_VERSION, pTemplateItem->nType, pTemplateItem->nIndex, ERROR_ID, pTemplateItem->dwRandomSeed
        );
        if (!pItem)
        {
            KGLogPrintf(
                KGLOG_ERR, 
                "[Shop] Shop template ID %u, invalid item (nType = %d, nIndex = %d, dwRandomSeed = %u) !\n", 
                dwTemplateID, pTemplateItem->nType, pTemplateItem->nIndex, pTemplateItem->dwRandomSeed
            );
            continue;
        }
        
        pShopItem->pItem = pItem;
        pItem = NULL;
        
        pShopItem->nCount               = pTemplateItem->nLimit;
        pShopItem->nItemTemplateIndex   = (int)i;

        if (pTemplateItem->nDurability != -1)
        {
            KGLOG_PROCESS_ERROR(pTemplateItem->nDurability >= 0);
            if (pShopItem->pItem->m_nMaxDurability > pTemplateItem->nDurability)
            {
                pShopItem->pItem->m_nCurrentDurability = pTemplateItem->nDurability;
            }
        }
    }
    
    memcpy(pNewShop->m_szShopName, pShopTemplate->szShopName, sizeof(pNewShop->m_szShopName));
    pNewShop->m_szShopName[sizeof(pNewShop->m_szShopName) - 1] = '\0';

    pNewShop->m_dwTemplateID      = dwTemplateID;
    pNewShop->m_nShopType         = eShopType_NPC;
    pNewShop->m_dwValidPage       = nPageIndex + 1;
    pNewShop->m_dwNpcID           = pNpc->m_dwID;
    pNewShop->m_pNpc              = pNpc;
    pNewShop->m_bCanRepair        = pShopTemplate->bCanRepair;

    pNpc->m_pShop = pNewShop;
    
    bResult = true;
Exit0:
    if (!bResult)
    {
        for (int nPageIndex = 0; nPageIndex < MAX_SHOP_PAGES; ++nPageIndex)
        {
            for (int nPosIndex = 0; nPosIndex < MAX_SHOP_PAGE_ITEM_COUNT; ++nPosIndex)
            {
                KSHOP_ITEM* pShopItem = &pNewShop->m_ShopPages[nPageIndex].ShopItems[nPosIndex];
                if (pShopItem->pItem)
                {
                    g_pSO3World->m_ItemManager.FreeItem(pShopItem->pItem);
                    pShopItem->pItem = NULL;
                }
            }
        }

        m_ShopList.erase(pNewShop->m_dwShopID);

        if (pItem)
        {
            g_pSO3World->m_ItemManager.FreeItem(pItem);
            pItem = NULL;
        }
    }
    return bResult;
}

BOOL KShopCenter::UnbindNpcShop(KNpc* pNpc)
{
    BOOL                    bResult     = false;
    DWORD                   dwShopID    = 0;
    KShop                  *pShop       = NULL;
    KSHOP_LIST::iterator    it;

    assert(pNpc);
    KGLOG_PROCESS_ERROR(pNpc->m_pShop);

    pShop = pNpc->m_pShop;
    KG_PROCESS_ERROR(pShop);
    KG_ASSERT_EXIT((pShop->m_dwNpcID == pNpc->m_dwID) && (pNpc->m_pShop->m_dwShopID == pShop->m_dwShopID));
    
    for (int nPageIndex = 0; nPageIndex < MAX_SHOP_PAGES; ++nPageIndex)
    {
        for (int nPosIndex = 0; nPosIndex < MAX_SHOP_PAGE_ITEM_COUNT; ++nPosIndex)
        {
            KSHOP_ITEM* pShopItem = &pShop->m_ShopPages[nPageIndex].ShopItems[nPosIndex];
            if (pShopItem->pItem)
            {
                g_pSO3World->m_ItemManager.FreeItem(pShopItem->pItem);
                pShopItem->pItem = NULL;
            }
        }
    }

    dwShopID            = pShop->m_dwShopID;  
    pShop->m_pNpc       = NULL;
    pShop->m_dwNpcID    = 0;
    pNpc->m_pShop       = NULL;
 
    m_ShopList.erase(dwShopID);

    bResult = true;
Exit0:
    return bResult;
}

void KShopCenter::DoRefresh()
{
    DWORD                   dwShopID    = 0;
    KShop                  *pShop       = NULL;
    KSHOP_LIST::iterator    it;

    for (it = m_ShopList.begin(); it != m_ShopList.end(); ++it) 
    {
        dwShopID = it->first;
        pShop = &it->second;
        KG_ASSERT_EXIT(pShop);

        if (pShop->m_nShopType != eShopType_NPC)
            continue;

        pShop->Refresh();
    }

Exit0:
    return;
}

int KShopCenter::LoadNpcShopTemplates()
{
    BOOL        bResult                     = false;
    int         nRetCode                    = false;
    int         nHeight                     = 0;
    ITabFile*   piTabFile                   = NULL;
    char        szNpcShopConfigFile[MAX_PATH];
    char        szTemplateFileName[MAX_PATH];
    char        szConfigFilePath[MAX_PATH];
    KNPC_SHOP_TEMPLATE_TABLE::iterator it;

    snprintf(szNpcShopConfigFile, sizeof(szNpcShopConfigFile), "%s/shop/%s", SETTING_DIR, NPC_SHOP_CONFIG_FILE);
    szNpcShopConfigFile[sizeof(szNpcShopConfigFile) - 1] = '\0';

    piTabFile = g_OpenTabFile(szNpcShopConfigFile);
    KGLOG_PROCESS_ERROR(piTabFile);

    nHeight = piTabFile->GetHeight();

    for (int nLine = 2; nLine <= nHeight; ++nLine) 
    {
        int                                     nTemplateID         = 0;
        KNPC_SHOP_TEMPLATE*                     pNpcShopTemplate    = NULL;
        pair<KNPC_SHOP_TEMPLATE_TABLE::iterator, bool>    InsRet;
        
        nRetCode = piTabFile->GetInteger(nLine, "ShopTemplateID", 0, &nTemplateID);
        KGLOG_PROCESS_ERROR(nRetCode);
        
        InsRet = m_NpcShopTemplateTable.insert(make_pair(nTemplateID, KNPC_SHOP_TEMPLATE()));
        KGLOG_PROCESS_ERROR(InsRet.second);
        
        pNpcShopTemplate = &(InsRet.first->second);

        nRetCode = piTabFile->GetString(nLine, "ShopName", "", pNpcShopTemplate->szShopName, sizeof(pNpcShopTemplate->szShopName));
        KGLOG_PROCESS_ERROR(nRetCode);
        pNpcShopTemplate->szShopName[sizeof(pNpcShopTemplate->szShopName) - 1] = '\0';

        nRetCode = piTabFile->GetInteger(nLine, "CanRepair", 0, (int*)&pNpcShopTemplate->bCanRepair);    
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->GetString(nLine, "ConfigFile", "", szTemplateFileName, sizeof(szTemplateFileName));
        KGLOG_PROCESS_ERROR(nRetCode);
        szTemplateFileName[sizeof(szTemplateFileName) - 1] = '\0';

        snprintf(szConfigFilePath, sizeof(szConfigFilePath), "%s/shop/%s", SETTING_DIR, szTemplateFileName);
        szConfigFilePath[sizeof(szConfigFilePath) - 1] = '\0';

        nRetCode = LoadNpcShopTemplateItems(pNpcShopTemplate, szConfigFilePath);
        if (!nRetCode)
        {
            KGLogPrintf(KGLOG_ERR, "Load shop %s failed!", szTemplateFileName);
            goto Exit0;
        }
    }

    bResult = true;
Exit0:
    if (!bResult)
    {
        m_NpcShopTemplateTable.clear();
    }
    
    KG_COM_RELEASE(piTabFile);
    
    return bResult;
}
#endif

BOOL KShopCenter::LoadNpcShopTemplateItems(KNPC_SHOP_TEMPLATE* pShopTemplate, const char cszFileName[])
{
    BOOL         bResult            = false;
    int          nRetCode           = false;
    ITabFile*    pNPCShopConfigFile = NULL;
    int          nShopItemCount     = 0;
    KSHOP_LIST::iterator it;

    DECLARE_STRING_MAP_BEGIN(ITEM_TABLE_TYPE)
        REGISTER_STR_TO_VALUE("Other",          ittOther)
        REGISTER_STR_TO_VALUE("CustomWeapon",   ittCustWeapon)
        REGISTER_STR_TO_VALUE("CustomArmor",    ittCustArmor)
        REGISTER_STR_TO_VALUE("CustomTrinket",  ittCustTrinket)
    DECLARE_STRING_MAP_END(ITEM_TABLE_TYPE)

    assert(pShopTemplate);
    assert(cszFileName);

    pNPCShopConfigFile = g_OpenTabFile(cszFileName);
    KGLOG_PROCESS_ERROR(pNPCShopConfigFile);

    nShopItemCount = pNPCShopConfigFile->GetHeight() - 1;
    KGLOG_PROCESS_ERROR(nShopItemCount > 0 && nShopItemCount <= MAX_SHOP_PAGE_ITEM_COUNT * MAX_SHOP_PAGES);
    
    pShopTemplate->vecTemplateItems.resize(nShopItemCount);

    for (int i = 0; i < nShopItemCount; ++i)
    {
        int                         nTabType        = 0;
        KNPC_SHOP_TEMPLATE_ITEM*    pShopItem       = NULL;
        int                         nLine           = i + 2;
        char                        szValue[_NAME_LEN];
        
        pShopItem = &(pShopTemplate->vecTemplateItems[i]);

        nRetCode = pNPCShopConfigFile->GetString(nLine, "TabType", "-1", szValue, sizeof(szValue));
        KGLOG_PROCESS_ERROR(nRetCode);

        MAP_STRING_LOCAL(ITEM_TABLE_TYPE, szValue, nTabType);
        KGLOG_PROCESS_ERROR(nTabType != -1);

        pShopItem->nType = nTabType;

        nRetCode = pNPCShopConfigFile->GetInteger(nLine, "ItemIndex", -1, &pShopItem->nIndex);          
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(pShopItem->nIndex != -1);
        
        nRetCode = pNPCShopConfigFile->GetInteger(nLine, "RandSeed", -1, (int*)&pShopItem->dwRandomSeed);               
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(pShopItem->dwRandomSeed != -1);
        
        nRetCode = pNPCShopConfigFile->GetInteger(nLine, "CountLimit", -1, &pShopItem->nLimit);     
        KGLOG_PROCESS_ERROR(nRetCode);
        
        nRetCode = pNPCShopConfigFile->GetInteger(nLine, "Durability", -1, &pShopItem->nDurability);
        KGLOG_PROCESS_ERROR(nRetCode);
        
        nRetCode = pNPCShopConfigFile->GetInteger(nLine, "ReputeLevel", 0, &pShopItem->nReputeLevel);     
        KGLOG_PROCESS_ERROR(nRetCode);
        
        nRetCode = pNPCShopConfigFile->GetInteger(nLine, "Price", -1, &pShopItem->nPrice);     
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(pShopItem->nPrice > 0);

        nRetCode = pNPCShopConfigFile->GetInteger(nLine, "Prestige", -1, &pShopItem->nPrestige);     
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = pNPCShopConfigFile->GetInteger(nLine, "Contribution", -1, &pShopItem->nContribution);     
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = pNPCShopConfigFile->GetInteger(nLine, "RequireAchievementRecord", -1, &pShopItem->nRequireAchievementRecord);     
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = pNPCShopConfigFile->GetInteger(nLine, "AchievementPoint", -1, &pShopItem->nAchievementPoint);     
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    bResult = true;
Exit0:
    KG_COM_RELEASE(pNPCShopConfigFile);
    return bResult;
}


KNPC_SHOP_TEMPLATE_ITEM* KShopCenter::GetShopTemplateItem(DWORD dwShopTemplateID, int nItemTemplateIndex)
{
    KNPC_SHOP_TEMPLATE_ITEM*    pResult = NULL;
    KNPC_SHOP_TEMPLATE*         pShopTemplate   = NULL;
    KNPC_SHOP_TEMPLATE_TABLE::iterator it;

    it = m_NpcShopTemplateTable.find(dwShopTemplateID);
    KGLOG_PROCESS_ERROR(it != m_NpcShopTemplateTable.end());

    pShopTemplate = &it->second;
    KGLOG_PROCESS_ERROR(pShopTemplate);

    KGLOG_PROCESS_ERROR(nItemTemplateIndex < (int)pShopTemplate->vecTemplateItems.size());
    
    pResult = &pShopTemplate->vecTemplateItems[nItemTemplateIndex];
Exit0:
    return pResult;
}

#ifdef _CLIENT
BOOL KShopCenter::UpDateShopInfo(DWORD dwShopTemplateID, DWORD dwShopID, int nShopType, DWORD dwValidPage, BOOL bCanRepair)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    KShop*      pShop       = NULL;
    KSHOP_LIST::iterator it;
    KNPC_SHOP_TEMPLATE_TABLE::iterator itTemplate;

    it = m_ShopList.find(dwShopID);
    if (it != m_ShopList.end())
    {
        pShop = &it->second;
        pShop->m_nShopType    = nShopType;
        pShop->m_dwValidPage  = dwValidPage;        
        pShop->m_bCanRepair   = bCanRepair;
    }
    else
    {
        KShop Shop;
        pair<KSHOP_LIST::iterator, bool> IntResult;

        memset(&Shop, 0, sizeof(Shop));
        Shop.m_dwShopID       = dwShopID;
        Shop.m_nShopType      = nShopType;
        Shop.m_dwValidPage    = dwValidPage;
        Shop.m_bCanRepair     = bCanRepair;
        Shop.m_dwTemplateID   = dwShopTemplateID;

        IntResult = m_ShopList.insert(KSHOP_LIST::value_type(dwShopID, Shop));
        KGLOG_PROCESS_ERROR(IntResult.second);
        pShop = &IntResult.first->second;   
    }
    
    itTemplate = m_NpcShopTemplateTable.find(dwShopTemplateID);
    
    if (itTemplate == m_NpcShopTemplateTable.end())
    {
        bRetCode = LoadShopTemplate(dwShopTemplateID);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KShopCenter::LoadShopTemplate(DWORD dwShopTemplateID)
{
    BOOL        bResult                     = false;
    int         nRetCode                    = false;
    int         nHeight                     = 0;
    ITabFile*   piTabFile                   = NULL;
    char        szNpcShopConfigFile[MAX_PATH];
    char        szTemplateFileName[MAX_PATH];
    char        szConfigFilePath[MAX_PATH];
    KNPC_SHOP_TEMPLATE_TABLE::iterator it;

    snprintf(szNpcShopConfigFile, sizeof(szNpcShopConfigFile), "%s/shop/%s", SETTING_DIR, NPC_SHOP_CONFIG_FILE);
    szNpcShopConfigFile[sizeof(szNpcShopConfigFile) - 1] = '\0';

    piTabFile = g_OpenTabFile(szNpcShopConfigFile);
    KGLOG_PROCESS_ERROR(piTabFile);

    nHeight = piTabFile->GetHeight();

    for (int nLine = 2; nLine <= nHeight; ++nLine) 
    {
        int                                     nTemplateID         = 0;
        KNPC_SHOP_TEMPLATE*                     pNpcShopTemplate    = NULL;
        pair<KNPC_SHOP_TEMPLATE_TABLE::iterator, bool>    InsRet;
        
        nRetCode = piTabFile->GetInteger(nLine, "ShopTemplateID", 0, &nTemplateID);
        KGLOG_PROCESS_ERROR(nRetCode);

        if (nTemplateID != (int)dwShopTemplateID)
        {
            continue;
        }
        
        InsRet = m_NpcShopTemplateTable.insert(make_pair(nTemplateID, KNPC_SHOP_TEMPLATE()));
        KGLOG_PROCESS_ERROR(InsRet.second);
        
        pNpcShopTemplate = &(InsRet.first->second);

        nRetCode = piTabFile->GetString(nLine, "ShopName", "", pNpcShopTemplate->szShopName, sizeof(pNpcShopTemplate->szShopName));
        KGLOG_PROCESS_ERROR(nRetCode);
        pNpcShopTemplate->szShopName[sizeof(pNpcShopTemplate->szShopName) - 1] = '\0';

        nRetCode = piTabFile->GetInteger(nLine, "CanRepair", 0, (int*)&pNpcShopTemplate->bCanRepair);    
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->GetString(nLine, "ConfigFile", "", szTemplateFileName, sizeof(szTemplateFileName));
        KGLOG_PROCESS_ERROR(nRetCode);
        szTemplateFileName[sizeof(szTemplateFileName) - 1] = '\0';

        snprintf(szConfigFilePath, sizeof(szConfigFilePath), "%s/shop/%s", SETTING_DIR, szTemplateFileName);
        szConfigFilePath[sizeof(szConfigFilePath) - 1] = '\0';

        nRetCode = LoadNpcShopTemplateItems(pNpcShopTemplate, szConfigFilePath);
        if (!nRetCode)
        {
            KGLogPrintf(KGLOG_ERR, "Load shop %s failed!", szTemplateFileName);
            goto Exit0;
        }
		break;
    }

    bResult = true;
Exit0:
    if (!bResult)
    {
        m_NpcShopTemplateTable.clear();
    }
    
    KG_COM_RELEASE(piTabFile);
    return bResult;
}
#endif

BOOL KShopCenter::LoadReputationRebate()
{
    BOOL                bResult                             = false;
    int                 nRetCode                            = 0;
    DWORD               dwForceID                           = 0;
    int                 nReputeLevel                        = 0;
    ITabFile*           piTabFile                           = NULL;
    int                 nHeight                             = 0;
    char                szReputationRebateTab[MAX_PATH];
    REPUTATION_REBATE   ReputationRebate;


    snprintf(szReputationRebateTab, sizeof(szReputationRebateTab), "%s/shop/%s", SETTING_DIR, REPUTATION_REBATE_CONFIG_FILE);
    szReputationRebateTab[sizeof(szReputationRebateTab) - 1] = '\0';
    piTabFile = g_OpenTabFile(szReputationRebateTab);
    
    if (!piTabFile) 
    {
        KGLogPrintf(KGLOG_DEBUG, "[shop] can not find Reputation rebate config file %s\n", szReputationRebateTab);
        goto Exit0;
    }
    
    nHeight = piTabFile->GetHeight();
    KGLOG_PROCESS_ERROR(nHeight > 0);

    for (int nLine = 2; nLine <= nHeight; ++nLine)
    {
        uint64_t uKey = 0;
        REPUTATION_REBATE_MAP::iterator it;

        nRetCode = piTabFile->GetInteger(nLine, "ForceID", 0, (int*)&dwForceID);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->GetInteger(nLine, "ReputeLevel", 0, &nReputeLevel);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->GetInteger(nLine, "Rebate", 0, &ReputationRebate.nRebate);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->GetInteger(nLine, "MaxRebate", 0, &ReputationRebate.nMaxRebate);
        KGLOG_PROCESS_ERROR(nRetCode);

        uKey = MAKE_INT64(dwForceID, nReputeLevel);
        
        it = m_ReputationRebateMap.find(uKey);
        if (it != m_ReputationRebateMap.end())
        {
            KGLogPrintf(
              KGLOG_ERR, "[Shop] config file %s two line have same ForceID and ReputeLevel", szReputationRebateTab   
            );
            goto Exit0;
        }
        
        m_ReputationRebateMap[uKey] = ReputationRebate;
    }
    
    bResult = true;
Exit0:
    KG_COM_RELEASE(piTabFile);
    return bResult;
}

int KShopCenter::GetReputationRebate(DWORD dwForceID, int nReputeLevel)
{
    int         nResult = KILO_NUM;
    uint64_t    uKey    = 0;
    REPUTATION_REBATE_MAP::iterator  it;

    uKey    = MAKE_INT64(dwForceID, nReputeLevel);
    it      = m_ReputationRebateMap.find(uKey);
    KG_PROCESS_ERROR(it != m_ReputationRebateMap.end());
    
    nResult = it->second.nRebate;

Exit0:
    return nResult;
}

int KShopCenter::GetMaxRebate(DWORD dwForceID, int nReputeLevel)
{
    int         nResult = KILO_NUM;
    uint64_t    uKey = 0; 
    REPUTATION_REBATE_MAP::iterator it;

    uKey = MAKE_INT64(dwForceID, nReputeLevel);
    it = m_ReputationRebateMap.find(uKey);
    KG_PROCESS_ERROR(it != m_ReputationRebateMap.end());

    nResult = it->second.nMaxRebate;

Exit0:
    return nResult;
}
