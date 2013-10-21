#include "stdafx.h"
#include "KGPublic.h"
#include "KItem.h"
#include "KPlayer.h"
#include "KTradingBox.h"
#include "KPlayerServer.h"
#include "KSO3World.h"
#include "SO3UI/SO3GameWorldUIHandler.h"
#include "Engine/KMemory.h"
#include "KScene.h"

KTradingBox::KTradingBox()
{
    m_nMoney            = 0;    
    m_pSelf             = NULL;
    m_pOtherSide        = NULL;
    m_bConfirm          = false;   
    m_nKey              = 0;
}

KTradingBox::~KTradingBox()
{
    assert(!m_pSelf);
    assert(!m_pOtherSide);
}

BOOL KTradingBox::Init(KPlayer* pSelf, KPlayer* pOther)
{
    assert(!m_pSelf);
    assert(!m_pOtherSide);

    m_nTradingItemCount = 0;
    m_pSelf             = pSelf;
    m_pOtherSide        = pOther;
    m_bConfirm          = false;
    m_nMoney            = 0;
    m_nKey              = 0;

    memset(m_ItemGrids, 0, sizeof(m_ItemGrids));

    return true;
}

void KTradingBox::UnInit()
{
    assert(m_pSelf);
    assert(m_pOtherSide);

    for (DWORD dwIndex = 0; dwIndex < MAX_TRADING_BOX_SIZE; ++dwIndex)
    {
#ifdef _CLIENT
        if (m_ItemGrids[dwIndex].pItem && m_pSelf->m_dwID != g_pSO3World->m_dwClientPlayerID)
        {
            // 在客户端,如果这个TradingBox不是属于客户端玩家的话,那么,这里面的Item对象都是通过服务端传来的
            // 数据new出来的,所以要释放.
            // 但是,如果是客户端玩家的话,这里的Item实际上是指向了玩家背包里面的对象,所以不用释放.
            g_pSO3World->m_ItemManager.FreeItem(m_ItemGrids[dwIndex].pItem);
        }
#endif
        m_ItemGrids[dwIndex].dwItemID   = ERROR_ID;
        m_ItemGrids[dwIndex].pItem      = NULL;

        m_ItemGrids[dwIndex].dwBoxIndex = 0;
        m_ItemGrids[dwIndex].dwPosIndex = 0;
        m_ItemGrids[dwIndex].nStackNum  = 0;
    }       
    
    m_nMoney            = 0;
    m_nKey              = 0;
    m_nTradingItemCount = 0;
    m_pSelf             = NULL;
    m_pOtherSide        = NULL;
}

#ifdef _SERVER
TRADING_RESULT_CODE KTradingBox::AddItem(DWORD dwGridIndex, DWORD dwBoxIndex, DWORD dwPosIndex)
{
    TRADING_RESULT_CODE     nResult             = trcInvalid;
    BOOL                    bRetCode            = false;
    KItem*                  pItem               = NULL;
    DWORD                   dwTabType           = 0;
    int                     nVesion             = 0;
    int                     nBoxType            = 0;
    int                     nItemAmount         = 0;
    int                     nItemMaxExistAmount = 0;
    KItemInfo*              pItemInfo           = NULL;
    int                     nStackNum           = 0;

    assert(m_pSelf);
    assert(m_pOtherSide);

    KGLOG_PROCESS_ERROR(dwGridIndex < MAX_TRADING_BOX_SIZE);

    nBoxType = m_pSelf->m_ItemList.GetBoxType(dwBoxIndex);
    KGLOG_PROCESS_ERROR(nBoxType == ivtPackage);

    pItem = m_pSelf->m_ItemList.GetItem(dwBoxIndex, dwPosIndex);
    KGLOG_PROCESS_ERROR(pItem);
    
    if (pItem->m_Common.nBindType != ibtBindOnTimeLimitation)
    {
        bRetCode = pItem->IsBind();
        KG_PROCESS_ERROR_RET_CODE(!bRetCode, trcItemBeBound);
    }
    else
    {
        KGLOG_PROCESS_ERROR(m_pSelf->m_pScene);
        bRetCode = m_pSelf->m_pScene->m_TimeLimitationBindItemMgr.IsPlayerCanHaveTheItem(pItem->m_dwID, m_pOtherSide->m_dwID);
        KG_PROCESS_ERROR_RET_CODE(bRetCode, trcItemBeBound);
    }

    nStackNum = pItem->GetStackNum();
    bRetCode = CheckItemExistLimit(pItem->m_GenParam.dwTabType, pItem->m_GenParam.dwIndex, nStackNum);
    KG_PROCESS_ERROR_RET_CODE(bRetCode, trcItemExistAmountLimit);

    for (DWORD dwIndex = 0; dwIndex < MAX_TRADING_BOX_SIZE; ++dwIndex)
    {
        if (!m_ItemGrids[dwIndex].pItem)
            continue;

        KG_PROCESS_ERROR(m_ItemGrids[dwIndex].pItem->m_dwID != pItem->m_dwID);

        KG_PROCESS_ERROR(
            m_ItemGrids[dwIndex].dwBoxIndex != dwBoxIndex || m_ItemGrids[dwIndex].dwPosIndex != dwPosIndex
        );
    }

    m_ItemGrids[dwGridIndex].pItem      = pItem;
    m_ItemGrids[dwGridIndex].dwItemID   = pItem->m_dwID;
    m_ItemGrids[dwGridIndex].dwBoxIndex = dwBoxIndex;
    m_ItemGrids[dwGridIndex].dwPosIndex = dwPosIndex;
    m_ItemGrids[dwGridIndex].nStackNum  = pItem->GetStackNum();
    
    ++m_nTradingItemCount;
    
    ++m_nKey;

    assert(m_pOtherSide->m_pTradingBox);
    m_pOtherSide->m_pTradingBox->SetKey(m_nKey);
   
    g_PlayerServer.DoSyncTradingItemData(
        m_pSelf->m_nConnIndex, NULL, m_pSelf->m_dwID, 
        dwGridIndex, dwBoxIndex, dwPosIndex, m_nKey
    );

    g_PlayerServer.DoSyncTradingItemData(
        m_pOtherSide->m_nConnIndex, pItem, m_pSelf->m_dwID, 
        dwGridIndex, dwBoxIndex, dwPosIndex, m_nKey
    );

    // 交易内容该变了, 双方的交易面板都都应该处于tsTrading状态
    m_pOtherSide->m_pTradingBox->SetConfirmFlag(false);
    SetConfirmFlag(false);

    nResult = trcSuccess;
Exit0:
    return nResult;
}
#endif

#ifdef _CLIENT
BOOL KTradingBox::AddItem(DWORD dwGridIndex, KItem* pItem)
{
    BOOL bResult = false;

    assert(m_pSelf);
    assert(m_pOtherSide);

    assert(!m_ItemGrids[dwGridIndex].pItem);
    m_ItemGrids[dwGridIndex].pItem = pItem;

    // 交易内容该变了, 双方的交易面板都都应该解锁
    m_pOtherSide->m_pTradingBox->SetConfirmFlag(false);
    SetConfirmFlag(false);
    
    bResult = true;
Exit0:
    return bResult;
}
#endif

BOOL KTradingBox::DelItem(DWORD dwGridIndex)
{
    BOOL bResult = false;

    assert(m_pSelf);
    assert(m_pOtherSide);

    KG_PROCESS_ERROR(dwGridIndex < MAX_TRADING_BOX_SIZE);
    KG_PROCESS_SUCCESS(!m_ItemGrids[dwGridIndex].pItem);

#ifdef _SERVER
    if (m_ItemGrids[dwGridIndex].pItem)
    {
        assert(m_pOtherSide->m_pTradingBox);
        --m_nTradingItemCount;
        ++m_nKey;
        m_pOtherSide->m_pTradingBox->SetKey(m_nKey);
        g_PlayerServer.DoSyncTradingItemData(m_pSelf->m_nConnIndex, NULL, m_pSelf->m_dwID, dwGridIndex, ibInvalid, 0, m_nKey);
        g_PlayerServer.DoSyncTradingItemData(m_pOtherSide->m_nConnIndex, NULL, m_pSelf->m_dwID, dwGridIndex, ibInvalid, 0, m_nKey);
    }
#endif

#ifdef _CLIENT
    if (m_pSelf->m_dwID != g_pSO3World->m_dwClientPlayerID)
    {
        g_pSO3World->m_ItemManager.FreeItem(m_ItemGrids[dwGridIndex].pItem);
    }
#endif

    // 交易内容该变了, 双方的交易面板都都应该解锁
    m_pOtherSide->m_pTradingBox->SetConfirmFlag(false);
    SetConfirmFlag(false);

    m_ItemGrids[dwGridIndex].pItem      = NULL;
    m_ItemGrids[dwGridIndex].dwItemID   = ERROR_ID;
    m_ItemGrids[dwGridIndex].nStackNum  = 0;
  
Exit1:
    bResult = true;
Exit0:
    return bResult;
}

KItem* KTradingBox::GetItem(DWORD dwGridIndex)
{
    KItem* pItem = NULL;

    assert(m_pSelf);
    assert(m_pOtherSide);

    KGLOG_PROCESS_ERROR(dwGridIndex < MAX_TRADING_BOX_SIZE);

    pItem = m_ItemGrids[dwGridIndex].pItem;
    
Exit0:
    return pItem;
}

BOOL KTradingBox::SetMoney(int nMoney)
{
    BOOL bResult        = false;
    int  nTotalMoney    = 0;

    assert(m_pSelf);
    assert(m_pOtherSide);
    
    KG_PROCESS_ERROR(nMoney >= 0);

#ifdef _SERVER
    nTotalMoney = m_pSelf->m_ItemList.GetMoney();
    KG_PROCESS_ERROR(nMoney <= nTotalMoney);
#endif

    m_nMoney = nMoney;

#ifdef _SERVER
    assert(m_pOtherSide->m_pTradingBox);        
    
    ++m_nKey;
    m_pOtherSide->m_pTradingBox->SetKey(m_nKey);
    g_PlayerServer.DoSyncTradingMoney(m_pSelf->m_nConnIndex, m_pSelf->m_dwID, nMoney, m_nKey);
    g_PlayerServer.DoSyncTradingMoney(m_pOtherSide->m_nConnIndex, m_pSelf->m_dwID, nMoney, m_nKey);
#endif
    // 交易内容该变了, 双方的交易面板都应该解锁
    m_pOtherSide->m_pTradingBox->SetConfirmFlag(false);
    SetConfirmFlag(false);

    bResult = true;
Exit0:
    return bResult;
}

int KTradingBox::GetMoney()
{
    return m_nMoney;
}

void KTradingBox::Confirm()
{
    int     nResult         = 0;
    int     nRetCode        = 0;
    BOOL    bOtherConfirm   = false;

    assert(m_pSelf);
    assert(m_pOtherSide);

    if (m_bConfirm)
        return;

    SetConfirmFlag(true);

#ifdef _SERVER
    bOtherConfirm = m_pOtherSide->m_pTradingBox->GetConfirmFLag();
    KG_PROCESS_ERROR(bOtherConfirm);

    FinishTrading();
#endif

Exit0:
    return;
}

void KTradingBox::Cancel()
{
    KPlayer* pOtherSide = m_pOtherSide;
    
    assert(m_pSelf);
    assert(m_pOtherSide);

#ifdef _SERVER
    //g_PlayerServer.DoSyncTradingConfirm(m_pSelf->m_nConnIndex, m_pSelf->m_dwID, false);
    g_PlayerServer.DoSyncTradingConfirm(m_pOtherSide->m_nConnIndex, m_pSelf->m_dwID, false);
#endif

    ClearTradingInfo();
    pOtherSide->m_pTradingBox->ClearTradingInfo();
}

void KTradingBox::SetKey(int nKey)
{
    m_nKey = nKey;
}

int KTradingBox::GetKey()
{
    return m_nKey;
}

KPlayer* KTradingBox::GetOtherSide()
{
    return m_pOtherSide;
}

#ifdef _SERVER
BOOL KTradingBox::FinishTradingCheck()
{
    BOOL             bResult                = false;
    BOOL             bRetCode               = false;
    int              nSelfMoney             = 0;
    int              nSelfMoneyLimit        = 0;
    int              nOtherMoeny            = 0;
    int              nOtherMoneyLimit       = 0;
    int              nOtherTradingMoney     = 0;
    KTradingBox*     pOtherTradingBox       = NULL;
    KItem*           pItem                  = NULL;
    int              nStackNum              = 0;

    assert(m_pSelf);
    assert(m_pOtherSide && m_pOtherSide->m_pTradingBox);
    
    pOtherTradingBox = m_pOtherSide->m_pTradingBox;
    
    // 检查钱
    KGLOG_PROCESS_ERROR(m_nMoney >= 0);
    if (m_nMoney > 0)
    {
        nSelfMoney = m_pSelf->m_ItemList.GetMoney();
        KGLOG_PROCESS_ERROR(nSelfMoney >= m_nMoney);
        
        nOtherMoeny = m_pOtherSide->m_ItemList.GetMoney();
        nOtherMoneyLimit = m_pOtherSide->m_ItemList.GetMoneyLimit();
        KG_PROCESS_ERROR(nOtherMoneyLimit - m_nMoney >= nOtherMoeny);
    }
    
    nOtherTradingMoney = pOtherTradingBox->GetMoney();
    KGLOG_PROCESS_ERROR(nOtherTradingMoney >= 0);

    if (nOtherTradingMoney > 0)
    {
        nOtherMoeny = m_pOtherSide->m_ItemList.GetMoney();
        KGLOG_PROCESS_ERROR(nOtherMoeny >= nOtherTradingMoney);
        
        nSelfMoney = m_pSelf->m_ItemList.GetMoney();
        nSelfMoneyLimit = m_pSelf->m_ItemList.GetMoneyLimit();
        KG_PROCESS_ERROR(nSelfMoneyLimit - nOtherTradingMoney >= nSelfMoney);
    }    

    // 检查自己的交易物品数据是否正确
    for (DWORD dwIndex = 0; dwIndex < MAX_TRADING_BOX_SIZE; ++dwIndex)
    {
        if (!m_ItemGrids[dwIndex].pItem)
            continue;

        // 检测玩家包裹里物品位置是否有变化
        pItem = m_pSelf->m_ItemList.GetItem(m_ItemGrids[dwIndex].dwBoxIndex, m_ItemGrids[dwIndex].dwPosIndex); 
        KG_PROCESS_ERROR(pItem == m_ItemGrids[dwIndex].pItem);
        KG_PROCESS_ERROR(m_ItemGrids[dwIndex].pItem->m_dwID == m_ItemGrids[dwIndex].dwItemID);
        
        nStackNum = pItem->GetStackNum();
        KG_PROCESS_ERROR(nStackNum == m_ItemGrids[dwIndex].nStackNum);

        // 检查物品拥有限制
        bRetCode = CheckItemExistLimit(pItem->m_GenParam.dwTabType, pItem->m_GenParam.dwIndex, 0);
        KG_PROCESS_ERROR(bRetCode);
    }

    // 检查对方的交易物品数据是否正确
    for (DWORD dwIndex = 0; dwIndex < MAX_TRADING_BOX_SIZE; ++dwIndex)
    {
        if (!pOtherTradingBox->m_ItemGrids[dwIndex].pItem)
            continue;

        // 检测玩家包裹里物品位置是否有变化
        pItem = m_pOtherSide->m_ItemList.GetItem(
            pOtherTradingBox->m_ItemGrids[dwIndex].dwBoxIndex, 
            pOtherTradingBox->m_ItemGrids[dwIndex].dwPosIndex
        ); 
        KG_PROCESS_ERROR(pItem == pOtherTradingBox->m_ItemGrids[dwIndex].pItem);
        KG_PROCESS_ERROR(
            pOtherTradingBox->m_ItemGrids[dwIndex].pItem->m_dwID == 
            pOtherTradingBox->m_ItemGrids[dwIndex].dwItemID
        );
        
        nStackNum = pItem->GetStackNum();
        KG_PROCESS_ERROR(nStackNum == pOtherTradingBox->m_ItemGrids[dwIndex].nStackNum);

        // 检查物品拥有限制
        bRetCode = pOtherTradingBox->CheckItemExistLimit(
            pItem->m_GenParam.dwTabType, pItem->m_GenParam.dwIndex, 0
        );
        KG_PROCESS_ERROR(bRetCode);
    }
    
    bResult = true;
Exit0:
    return bResult;
}

enum FINISH_TRADING_STEP
{
    ftsBegin,
    ftsPickUpItem,
    ftsDstPlayerGetItem,
    ftsSrcPlayerGetItem,
    ftsEnd
};

struct KTRADING_ITEM
{
    KItem*          pItem;
    DWORD           dwPutBoxIndex;
    DWORD           dwPutPos;
    KTRADING_ITEM*  pNext;
};

void KTradingBox::FinishTrading()
{
    BOOL                 bResult                = false;
    BOOL                 bRetCode               = false;
    TRADING_RESULT_CODE  nSrcResult             = trcFailed;
    TRADING_RESULT_CODE  nDstResult             = trcFailed;
    FINISH_TRADING_STEP  eOperationStep         = ftsBegin;    
    KTradingBox*         pOtherTradingBox       = NULL;
    KItem*               pItem                  = NULL;
    KTRADING_ITEM*       pTradingItem           = NULL;
    int                  nSrcTradingItemCount   = 0;
    int                  nDstTradingItemCount   = 0;
    int                  nFreeRoom              = 0;
    DWORD                dwBoxIndex             = 0;
    DWORD                dwPos                  = 0;
    ADD_ITEM_RESULT_CODE eAddRetCode            = aircFailed;
    KTRADING_ITEM*       pSrcTradingItemList    = NULL;
    KTRADING_ITEM*       pDstTradingItemList    = NULL;

    assert(m_pSelf);
    assert(m_pOtherSide && m_pOtherSide->m_pTradingBox);
    
    pOtherTradingBox = m_pOtherSide->m_pTradingBox;
    
    bRetCode = FinishTradingCheck();
    KG_PROCESS_ERROR(bRetCode);

    eOperationStep = ftsPickUpItem;

    // 取出自己要交易的物品
    for (DWORD dwIndex = 0; dwIndex < MAX_TRADING_BOX_SIZE; ++dwIndex)
    {
        if (!m_ItemGrids[dwIndex].pItem)
            continue;

        pTradingItem = new KTRADING_ITEM;
        pTradingItem->pItem = m_pSelf->m_ItemList.PickUpItem(m_ItemGrids[dwIndex].dwBoxIndex, m_ItemGrids[dwIndex].dwPosIndex); 
        assert(pTradingItem->pItem);
        pTradingItem->dwPutBoxIndex = ibInvalid;
        pTradingItem->dwPutPos      = 0;
        pTradingItem->pNext         = pSrcTradingItemList;

        pSrcTradingItemList         = pTradingItem;
        pTradingItem                = NULL;

        ++nSrcTradingItemCount;
    }

    // 取出对方要交易的物品
    for (DWORD dwIndex = 0; dwIndex < MAX_TRADING_BOX_SIZE; ++dwIndex)
    {
        if (!pOtherTradingBox->m_ItemGrids[dwIndex].pItem)
            continue;

        pTradingItem = new KTRADING_ITEM;
        pTradingItem->pItem = m_pOtherSide->m_ItemList.PickUpItem(
            pOtherTradingBox->m_ItemGrids[dwIndex].dwBoxIndex, 
            pOtherTradingBox->m_ItemGrids[dwIndex].dwPosIndex
        ); 
        assert(pTradingItem->pItem);
        pTradingItem->dwPutBoxIndex = ibInvalid;
        pTradingItem->dwPutPos      = 0;
        pTradingItem->pNext         = pDstTradingItemList;

        pDstTradingItemList         = pTradingItem;
        pTradingItem                = NULL;

        ++nDstTradingItemCount;
    }
    
    // 检查包裹空间
    nFreeRoom = m_pSelf->m_ItemList.GetAllPackageFreeRoomSize();
    if (nFreeRoom < nDstTradingItemCount)
    {
        nSrcResult = trcNotEnoughFreeRoom;
        goto Exit0;
    }

    nFreeRoom = m_pOtherSide->m_ItemList.GetAllPackageFreeRoomSize();
    if (nFreeRoom < nSrcTradingItemCount)
    {
        nDstResult = trcNotEnoughFreeRoom;
        goto Exit0;
    }
    
    // 把我的东西给对方
    eOperationStep = ftsDstPlayerGetItem;

    pTradingItem = pSrcTradingItemList;
    while (pTradingItem)
    {
        assert(pTradingItem->pItem);
        
        bRetCode = m_pOtherSide->m_ItemList.FindFreeRoomInPackage(dwBoxIndex, dwPos);
        assert(bRetCode);

        eAddRetCode = m_pOtherSide->m_ItemList.AddItemToPos(pTradingItem->pItem, dwBoxIndex, dwPos);
        KG_PROCESS_ERROR(eAddRetCode == aircSuccess);

        if (pTradingItem->pItem->m_Common.nBindType == ibtBindOnTimeLimitation && m_pOtherSide->m_pScene)
        {
            m_pOtherSide->m_pScene->m_TimeLimitationBindItemMgr.SetOwner(pTradingItem->pItem->m_dwID, m_pOtherSide->m_dwID);
        }
        
        pTradingItem->dwPutBoxIndex = dwBoxIndex;
        pTradingItem->dwPutPos      = dwPos;

        pTradingItem = pTradingItem->pNext;
    }
    
    // 把对方的东西给我
    eOperationStep = ftsSrcPlayerGetItem;

    pTradingItem = pDstTradingItemList;
    while (pTradingItem)
    {
        assert(pTradingItem->pItem);
        
        bRetCode = m_pSelf->m_ItemList.FindFreeRoomInPackage(dwBoxIndex, dwPos);
        assert(bRetCode);

        eAddRetCode = m_pSelf->m_ItemList.AddItemToPos(pTradingItem->pItem, dwBoxIndex, dwPos);
        KG_PROCESS_ERROR(eAddRetCode == aircSuccess);

        if (pTradingItem->pItem->m_Common.nBindType == ibtBindOnTimeLimitation && m_pSelf->m_pScene)
        {
            m_pSelf->m_pScene->m_TimeLimitationBindItemMgr.SetOwner(pTradingItem->pItem->m_dwID, m_pSelf->m_dwID);
        }
        
        pTradingItem->dwPutBoxIndex = dwBoxIndex;
        pTradingItem->dwPutPos      = dwPos;

        pTradingItem = pTradingItem->pNext;
    }
    
    // 把我的钱给他
    bRetCode = m_pSelf->m_ItemList.AddMoney(-m_nMoney);
    assert(bRetCode);

    bRetCode = m_pOtherSide->m_ItemList.AddMoney(m_nMoney);
    assert(bRetCode);
    
    g_LogClient.LogPlayerMoneyChange(-m_nMoney, m_pSelf, "trading to other player : ", m_pOtherSide->m_szName);
    g_LogClient.LogPlayerMoneyChange(m_nMoney, m_pOtherSide, "trading from other player : ", m_pSelf->m_szName);

    // 把他的钱给我
    bRetCode = m_pOtherSide->m_ItemList.AddMoney(-pOtherTradingBox->m_nMoney);
    assert(bRetCode);

    bRetCode = m_pSelf->m_ItemList.AddMoney(pOtherTradingBox->m_nMoney);
    assert(bRetCode);
    
    g_LogClient.LogPlayerMoneyChange(-pOtherTradingBox->m_nMoney, m_pOtherSide, "trading to other player : ", m_pSelf->m_szName);
    g_LogClient.LogPlayerMoneyChange(pOtherTradingBox->m_nMoney, m_pSelf, "trading from other player : ", m_pOtherSide->m_szName);
    
    eOperationStep = ftsEnd;
    // 交易结束, 下面的代码不能有跳转失败的操作

    while (pSrcTradingItemList)
    {
        assert(pSrcTradingItemList->pItem);
        g_PlayerServer.DoAddItemNotify(
            m_pOtherSide, pSrcTradingItemList->pItem->m_dwID, pSrcTradingItemList->pItem->GetStackNum()
        );
        
        g_LogClient.LogPlayerItemChange(
            m_pSelf, ITEM_OPERATION_TYPE_OUT, pSrcTradingItemList->pItem, "sell to other player: ", m_pOtherSide->m_szName
        );

        g_LogClient.LogPlayerItemChange(
            m_pOtherSide, ITEM_OPERATION_TYPE_IN, pSrcTradingItemList->pItem, "buy from other player: ", m_pSelf->m_szName
        );

        pTradingItem = pSrcTradingItemList->pNext;
        KG_DELETE(pSrcTradingItemList);
        pSrcTradingItemList = pTradingItem;
    }

    while (pDstTradingItemList)
    {
        assert(pDstTradingItemList->pItem);
        g_PlayerServer.DoAddItemNotify(
            m_pSelf, pDstTradingItemList->pItem->m_dwID, pDstTradingItemList->pItem->GetStackNum()
        );
        
        g_LogClient.LogPlayerItemChange(
            m_pSelf, ITEM_OPERATION_TYPE_IN, pDstTradingItemList->pItem, "buy from other player: ", m_pOtherSide->m_szName
        );

        g_LogClient.LogPlayerItemChange(
            m_pOtherSide, ITEM_OPERATION_TYPE_OUT, pDstTradingItemList->pItem, "sell to other player: ", m_pSelf->m_szName
        );

        pTradingItem = pDstTradingItemList->pNext;
        KG_DELETE(pDstTradingItemList);
        pDstTradingItemList = pTradingItem;
    }

    bResult = true;
Exit0:
    if (!bResult)
    {
        // 回滚
        switch (eOperationStep)
        {
        case ftsSrcPlayerGetItem:
            {
                pTradingItem = pDstTradingItemList;
                for (;pTradingItem != NULL; pTradingItem = pTradingItem->pNext)
                {                    
                    if (pTradingItem->dwPutBoxIndex == ibInvalid)
                    {
                        continue;
                    }
                    
                    pItem = m_pSelf->m_ItemList.PickUpItem(
                        pTradingItem->dwPutBoxIndex, pTradingItem->dwPutPos
                    );
                    assert(pItem);
                    
                    pTradingItem->pItem = pItem;
                    pTradingItem->dwPutBoxIndex = ibInvalid;
                } 
            }
            // break; // 不用break,下面的回滚操作要继续
        case ftsDstPlayerGetItem:
            {
                pTradingItem = pSrcTradingItemList;
                for (;pTradingItem != NULL; pTradingItem = pTradingItem->pNext)
                {
                    if (pTradingItem->dwPutBoxIndex == ibInvalid)
                    {
                        continue;
                    }
                    
                    pItem = m_pOtherSide->m_ItemList.PickUpItem(
                        pTradingItem->dwPutBoxIndex, pTradingItem->dwPutPos
                    );
                    assert(pItem);
                    
                    pTradingItem->pItem = pItem;
                    pTradingItem->dwPutBoxIndex = ibInvalid;
                } 
            }
            // break; // 不用break,下面的回滚操作要继续
        case ftsPickUpItem:
            {
                while (pSrcTradingItemList)
                {
                    assert(pSrcTradingItemList->pItem);
                    eAddRetCode = m_pSelf->m_ItemList.AddItem(pSrcTradingItemList->pItem, false);
                    if (eAddRetCode != aircSuccess)
                    {
                        KGLogPrintf(KGLOG_ERR, "Trading rollback error: add Src item failed!\n");
                        g_pSO3World->m_ItemManager.FreeItem(pSrcTradingItemList->pItem);
                    }
                    pSrcTradingItemList->pItem   = NULL;

                    pTradingItem                 = pSrcTradingItemList->pNext;
                    KG_DELETE(pSrcTradingItemList);
                    pSrcTradingItemList          = pTradingItem;
                }

                while (pDstTradingItemList)
                {
                    assert(pDstTradingItemList->pItem);
                    eAddRetCode = m_pOtherSide->m_ItemList.AddItem(pDstTradingItemList->pItem, false);
                    if (eAddRetCode != aircSuccess)
                    {
                        KGLogPrintf(KGLOG_ERR, "Trading rollback error: add Dst item failed!\n");
                        g_pSO3World->m_ItemManager.FreeItem(pDstTradingItemList->pItem);
                    }
                    pDstTradingItemList->pItem   = NULL;

                    pTradingItem  = pDstTradingItemList->pNext;
                    KG_DELETE(pDstTradingItemList);
                    pDstTradingItemList = pTradingItem;
                }
            }
        	break;
        case ftsBegin:
            break;
        default:
            assert(false);
        }

        g_PlayerServer.DoMessageNotify(m_pSelf->m_nConnIndex, ectTradingErrorCode, nSrcResult);
        g_PlayerServer.DoMessageNotify(m_pOtherSide->m_nConnIndex, ectTradingErrorCode, nDstResult);
    }
    
    g_PlayerServer.DoTradingComplete(m_pSelf->m_nConnIndex, bResult);
    g_PlayerServer.DoTradingComplete(m_pOtherSide->m_nConnIndex, bResult);
    
    assert(pSrcTradingItemList == NULL);
    assert(pDstTradingItemList == NULL);

    ClearTradingInfo();
    pOtherTradingBox->ClearTradingInfo();

    return;
}
#endif

void KTradingBox::ClearTradingInfo()
{
    KTradingBox* pSelfTradingBox  = NULL;

    pSelfTradingBox               = m_pSelf->m_pTradingBox;
    m_pSelf->m_dwTradingInviteSrc = ERROR_ID;
    m_pSelf->m_dwTradingInviteDst = ERROR_ID;
    m_pSelf->m_pTradingBox        = NULL;

    pSelfTradingBox->UnInit();
    KMemory::Delete(pSelfTradingBox);
    pSelfTradingBox = NULL;
}

void KTradingBox::SetConfirmFlag(BOOL bConfirm)
{   
    assert(m_pSelf);
    assert(m_pOtherSide);
#ifdef _SERVER
    if (bConfirm && m_bConfirm != bConfirm)
    {
        //g_PlayerServer.DoSyncTradingConfirm(m_pSelf->m_nConnIndex, m_pSelf->m_dwID, true);
        g_PlayerServer.DoSyncTradingConfirm(m_pOtherSide->m_nConnIndex, m_pSelf->m_dwID, true);
    }
#endif

#ifdef _CLIENT
    if (m_bConfirm != bConfirm)
    {
        if (g_pGameWorldUIHandler)
        {
            KUIEventTradingUpdateConfirm UpdateConfirm;
            UpdateConfirm.dwCharacterID = m_pSelf->m_dwID;
            UpdateConfirm.bConfirm = bConfirm;
            g_pGameWorldUIHandler->OnTradingUpdateConfirm(UpdateConfirm);
        }
    }
#endif
    m_bConfirm = bConfirm;
}

BOOL KTradingBox::GetConfirmFLag()
{
    return m_bConfirm;
}

#ifdef _SERVER
BOOL KTradingBox::CheckItemExistLimit(DWORD dwTabType, DWORD dwItemIndex, int nItemAmount)
{
    BOOL        bResult             = false;
    KItemInfo*  pItemInfo           = NULL;
    int         nItemMaxExistAmount = 0;
    
    assert(m_pOtherSide);

    nItemAmount += m_pOtherSide->m_ItemList.GetItemTotalAmount(dwTabType, dwItemIndex);
    
    for (DWORD dwIndex = 0; dwIndex < MAX_TRADING_BOX_SIZE; ++dwIndex)
    {
        if (!m_ItemGrids[dwIndex].pItem)     
            continue;

        if (m_ItemGrids[dwIndex].pItem->m_GenParam.dwTabType == dwTabType && 
            m_ItemGrids[dwIndex].pItem->m_GenParam.dwIndex == dwItemIndex
        )
        {
            nItemAmount += m_ItemGrids[dwIndex].pItem->GetStackNum();
        }
    }

    pItemInfo = g_pSO3World->m_ItemManager.GetItemInfo(dwTabType, dwItemIndex);
    KGLOG_PROCESS_ERROR(pItemInfo);     

    nItemMaxExistAmount = pItemInfo->nMaxExistAmount;
    KG_PROCESS_ERROR(nItemMaxExistAmount == 0 || nItemMaxExistAmount >= nItemAmount);

    bResult = true;
Exit0:
    return bResult;
}
#endif

