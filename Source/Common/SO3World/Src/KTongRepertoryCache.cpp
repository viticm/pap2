#include "stdafx.h"

#if defined(_CLIENT)
#include "KPlayer.h"
#include "KSO3World.h"
#include "KItemList.h"
#include "KPlayerClient.h"
#include "KTongRepertoryCache.h"
#include "SO3UI/SO3GameWorldUIHandler.h"

BOOL KTongRepertoryCache::Init()
{
    m_nMaxPageNum = 0;
    for (int i = 0 ; i < TONG_REPERTORY_PAGE_NUM; i++)
    {
        m_Pages[i].m_nLastUpdateCounter = -1;
        m_Pages[i].m_bGetRight = false;
        m_Pages[i].m_bPutRight = false;
        
        for (int j = 0 ; j < TONG_REPERTORY_PAGE_CAPACITY; j++)
        {
            m_Pages[i].m_pItems[j] = NULL;
        }
    }
    return true;
}

void KTongRepertoryCache::UnInit()
{
    for (int i = 0 ; i < TONG_REPERTORY_PAGE_NUM; i++)
    {
        for (int j = 0 ; j < TONG_REPERTORY_PAGE_CAPACITY; j++)
        {
            if (m_Pages[i].m_pItems[j] == NULL)
                continue;
            g_pSO3World->m_ItemManager.FreeItem(m_Pages[i].m_pItems[j]);
            m_Pages[i].m_pItems[j] = NULL;
        }
    }
}

void KTongRepertoryCache::Reset()
{
    for (int i = 0 ; i < TONG_REPERTORY_PAGE_NUM; i++)
    {
        m_Pages[i].m_nLastUpdateCounter = -1;
        m_Pages[i].m_bGetRight = false;
        m_Pages[i].m_bPutRight = false;
        
        for (int j = 0 ; j < TONG_REPERTORY_PAGE_CAPACITY; j++)
        {
            if (m_Pages[i].m_pItems[j] == NULL)
                continue;
            g_pSO3World->m_ItemManager.FreeItem(m_Pages[i].m_pItems[j]);
            m_Pages[i].m_pItems[j] = NULL;
        }
    }
}

BOOL KTongRepertoryCache::TakeItemToPos(int nPageIndex, int nPagePos, int nBoxIndex, int nBoxPos)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    KPlayer*    pPlayer     = NULL;
    KItem*      pItem       = NULL;
    int         nBoxSize    = 0;

    assert(nPageIndex >= 0 && nPageIndex < TONG_REPERTORY_PAGE_NUM);
    assert(nPagePos >= 0 && nPagePos < TONG_REPERTORY_PAGE_CAPACITY);
    assert(nBoxIndex >= ibPackage && nBoxIndex <= ibPackage4);
    assert(nBoxPos >= 0 && nBoxPos < MAX_ITEM_PACKAGE_SIZE);

    pItem = m_Pages[nPageIndex].m_pItems[nPagePos];
    KGLOG_PROCESS_ERROR(pItem);

    KGLOG_PROCESS_ERROR(nPageIndex >= 0 && nPageIndex < m_nMaxPageNum);

    bRetCode = CheckTakePageRight(nPageIndex);
    KG_PROCESS_ERROR(bRetCode);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    nBoxSize    = pPlayer->m_ItemList.GetBoxSize(nBoxIndex);
    KGLOG_PROCESS_ERROR(nBoxPos >= 0 && nBoxPos < nBoxSize);

    bRetCode = pPlayer->m_ItemList.CanAddItemToPos(pItem, nBoxIndex, nBoxPos);
    KGLOG_PROCESS_ERROR(bRetCode);

    g_PlayerClient.DoTakeTongRepertoryItemToPosRequest(nPageIndex, nPagePos, nBoxIndex, nBoxPos, m_Pages[nPageIndex].m_nLastUpdateCounter);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTongRepertoryCache::TakeItem(int nPageIndex, int nPagePos)
{
    BOOL                    bResult     = false;
    BOOL                    bRetCode    = false;
    ADD_ITEM_RESULT_CODE    eRetCode    = aircFailed;
    KPlayer*                pPlayer     = NULL;
    KItem*                  pItem       = NULL;

    assert(nPageIndex >= 0 && nPageIndex < TONG_REPERTORY_PAGE_NUM);
    assert(nPagePos >= 0 && nPagePos < TONG_REPERTORY_PAGE_CAPACITY);

    KGLOG_PROCESS_ERROR(nPageIndex >= 0 && nPageIndex < m_nMaxPageNum);

    bRetCode = CheckTakePageRight(nPageIndex);
    KG_PROCESS_ERROR(bRetCode);

    pItem = m_Pages[nPageIndex].m_pItems[nPagePos];
    KGLOG_PROCESS_ERROR(pItem);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    eRetCode = pPlayer->m_ItemList.CanAddItem(pItem);
    KG_PROCESS_ERROR(eRetCode == aircSuccess);

    g_PlayerClient.DoTakeTongRepertoryItemRequest(nPageIndex, nPagePos, m_Pages[nPageIndex].m_nLastUpdateCounter);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTongRepertoryCache::PutItemToPos(int nBoxIndex, int nBoxPos, int nPageIndex, int nPagePos)
{
    BOOL                        bResult     = false;
    BOOL                        bRetCode    = false;
    KItem*                      pItem       = NULL;       
    KPlayer*                    pPlayer     = NULL;
    KTongRepertoryCachePage*    pPage       = NULL;
    int                         nGenre      = 0;

    assert(nBoxIndex >= ibPackage && nBoxIndex <= ibPackage4);
    assert(nBoxPos >= 0 && nBoxPos < MAX_ITEM_PACKAGE_SIZE);
    assert(nPageIndex >= 0 && nPageIndex < TONG_REPERTORY_PAGE_NUM);
    assert(nPagePos >= 0 && nPagePos < TONG_REPERTORY_PAGE_CAPACITY);

    KGLOG_PROCESS_ERROR(nPageIndex >= 0 && nPageIndex < m_nMaxPageNum);

    bRetCode = CheckPutPageRight(nPageIndex);
    KG_PROCESS_ERROR(bRetCode);

    pPage = &m_Pages[nPageIndex];
    KGLOG_PROCESS_ERROR(pPage->m_pItems[nPagePos] == NULL);
    
    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    pItem = pPlayer->m_ItemList.GetItem(nBoxIndex, nBoxPos);
    KGLOG_PROCESS_ERROR(pItem);

    bRetCode = pItem->IsBind();
    KGLOG_PROCESS_ERROR(!bRetCode);

    nGenre = pItem->GetGenre();
    KGLOG_PROCESS_ERROR(nGenre != igTaskItem);

    g_PlayerClient.DoPutTongRepertoryItemToPosRequest(nBoxIndex, nBoxPos, nPageIndex, nPagePos);
    
    bResult = true;
Exit0:
    return bResult;
}

BOOL KTongRepertoryCache::PutItem(int nBoxIndex, int nBoxPos, int nPageIndex)
{
    BOOL                        bResult     = false;
    BOOL                        bRetCode    = false;
    KItem*                      pItem       = NULL;       
    KPlayer*                    pPlayer     = NULL;
    KTongRepertoryCachePage*    pPage       = NULL;
    int                         nGenre      = 0;

    assert(nBoxIndex >= ibPackage && nBoxIndex <= ibPackage4);
    assert(nBoxPos >= 0 && nBoxPos < MAX_ITEM_PACKAGE_SIZE);
    assert(nPageIndex >= 0 && nPageIndex < TONG_REPERTORY_PAGE_NUM);

    KGLOG_PROCESS_ERROR(nPageIndex >= 0 && nPageIndex < m_nMaxPageNum);

    bRetCode = CheckPutPageRight(nPageIndex);
    KG_PROCESS_ERROR(bRetCode);
    
    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    pItem = pPlayer->m_ItemList.GetItem(nBoxIndex, nBoxPos);
    KGLOG_PROCESS_ERROR(pItem);

    bRetCode = pItem->IsBind();
    KGLOG_PROCESS_ERROR(!bRetCode);

    nGenre = pItem->GetGenre();
    KGLOG_PROCESS_ERROR(nGenre != igTaskItem);

    g_PlayerClient.DoPutTongRepertoryItemRequest(nBoxIndex, nBoxPos, nPageIndex);
    
    bResult = true;
Exit0:
    return bResult;
}

BOOL KTongRepertoryCache::ExchangeItemPos(int nSrcPageIndex, int nSrcPos, int nDstPageIndex, int nDstPos)
{
    BOOL                        bResult     = false;
    BOOL                        bRetCode    = false;
    KTongRepertoryCachePage*    pSrcPage    = NULL;
    KTongRepertoryCachePage*    pDstPage    = NULL;
    KItem*                      pSrcItem    = NULL;
    KItem*                      pDstItem    = NULL;

    assert(nSrcPageIndex >= 0 && nSrcPageIndex < TONG_REPERTORY_PAGE_NUM);
    assert(nDstPageIndex >= 0 && nDstPageIndex < TONG_REPERTORY_PAGE_NUM);
    assert(nSrcPos >= 0 && nSrcPos < TONG_REPERTORY_PAGE_CAPACITY);
    assert(nDstPos >= 0 && nDstPos < TONG_REPERTORY_PAGE_CAPACITY);

    KGLOG_PROCESS_ERROR(nSrcPageIndex >= 0 && nSrcPageIndex < m_nMaxPageNum);
    KGLOG_PROCESS_ERROR(nDstPageIndex >= 0 && nDstPageIndex < m_nMaxPageNum);

    KGLOG_PROCESS_ERROR(nSrcPageIndex != nDstPageIndex || nSrcPos != nDstPos);

    pSrcPage = &m_Pages[nSrcPageIndex];
    pDstPage = &m_Pages[nDstPageIndex];

    pSrcItem = pSrcPage->m_pItems[nSrcPos];
    pDstItem = pDstPage->m_pItems[nDstPos];

    KGLOG_PROCESS_ERROR(pSrcItem || pDstItem);

    if (pSrcItem)
    {
        bRetCode = CheckTakePageRight(nSrcPageIndex);
        KG_PROCESS_ERROR(bRetCode);

        bRetCode = CheckPutPageRight(nDstPageIndex);
        KG_PROCESS_ERROR(bRetCode);
    }

    if (pDstItem)
    {
        bRetCode = CheckTakePageRight(nDstPageIndex);
        KG_PROCESS_ERROR(bRetCode);

        bRetCode = CheckPutPageRight(nSrcPageIndex);
        KG_PROCESS_ERROR(bRetCode);
    }

    g_PlayerClient.DoExchangeTongRepertoryItemPosRequest(
        nSrcPageIndex, nSrcPos, pSrcPage->m_nLastUpdateCounter, 
        nDstPageIndex, nDstPos, pDstPage->m_nLastUpdateCounter
    );

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTongRepertoryCache::StackItem(
    int nPageIndex, int nPagePos, int nBoxIndex, int  nBoxPos, int nStackNum
)
{
    BOOL        bResult         = false;
    BOOL        bRetCode        = false;
    int         nBoxSize        = 0;
    KPlayer*    pPlayer         = NULL;
    KItem*      pItem           = NULL;
    KItem*      pDstItem        = NULL;
    int         nDstStackNum    = 0;
    int         nMaxStackNum    = 0;
    int         nGenre          = 0;

    assert(nPageIndex >= 0 && nPageIndex < TONG_REPERTORY_PAGE_NUM);
    assert(nPagePos >= 0 && nPagePos < TONG_REPERTORY_PAGE_CAPACITY);
    assert(nBoxIndex >= ibPackage && nBoxIndex <= ibPackage4);
    assert(nBoxPos >= 0 && nBoxPos < MAX_ITEM_PACKAGE_SIZE);
    KGLOG_PROCESS_ERROR(nPageIndex >= 0 && nPageIndex < m_nMaxPageNum);

    bRetCode = CheckPutPageRight(nPageIndex);
    KG_PROCESS_ERROR(bRetCode);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    pItem = pPlayer->m_ItemList.GetItem(nBoxIndex, nBoxPos);
    KGLOG_PROCESS_ERROR(pItem);
    
    bRetCode = pItem->IsBind();
    KGLOG_PROCESS_ERROR(!bRetCode);

    nGenre = pItem->GetGenre();
    KG_PROCESS_ERROR(nGenre != igTaskItem);

    pDstItem = m_Pages[nPageIndex].m_pItems[nPagePos];
    KGLOG_PROCESS_ERROR(pDstItem);

    bRetCode = pDstItem->CanStack(pItem);
    KGLOG_PROCESS_ERROR(bRetCode);

    nMaxStackNum    = pItem->GetMaxStackNum();
    nDstStackNum    = pDstItem->GetStackNum();

    KGLOG_PROCESS_ERROR(nStackNum + nDstStackNum <= nMaxStackNum);

    g_PlayerClient.DoStackTongRepertoryItemRequest(
        nPageIndex, nPagePos, nBoxIndex, nBoxPos, nStackNum, 
        m_Pages[nPageIndex].m_nLastUpdateCounter
    );
    
    bResult = true;
Exit0:
    return bResult;
}

BOOL KTongRepertoryCache::CheckTakePageRight(int nPageIndex)
{
    BOOL bResult = false;

    assert(nPageIndex >= 0 && nPageIndex < TONG_REPERTORY_PAGE_NUM);
    assert(nPageIndex >= 0 && nPageIndex < m_nMaxPageNum);

    KG_PROCESS_ERROR(m_Pages[nPageIndex].m_bGetRight);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTongRepertoryCache::CheckPutPageRight(int nPageIndex)
{
    BOOL bResult = false;

    assert(nPageIndex >= 0 && nPageIndex < TONG_REPERTORY_PAGE_NUM);
    assert(nPageIndex >= 0 && nPageIndex < m_nMaxPageNum);

    KG_PROCESS_ERROR(m_Pages[nPageIndex].m_bPutRight);

    bResult = true;
Exit0:
    return bResult;
}
#endif // _CLIENT
