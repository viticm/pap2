////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KTradingBox.h
//  Version     : 1.0
//  Creator     : Ye Bixuan
//  Create Date : 2006-12-18 
//  Comment     : 

////////////////////////////////////////////////////////////////////////////////

#ifndef _KTRADING_BOX_H_
#define _KTRADING_BOX_H_

#include "Global.h"

class KPlayer;
class KItem;
enum  TRADING_RESULT_CODE;

class KTradingBox
{
public:
    KTradingBox();
    ~KTradingBox();

    BOOL Init(KPlayer* pSelf, KPlayer* pOther);
    void UnInit();

    // dwGridIndex: 放进交易栏的哪个格子
    // dwBoxIndex:  物品在玩家的哪个背包里面
    // dwPosIndex:  物品在背包里面的什么地方
#ifdef _SERVER
    TRADING_RESULT_CODE AddItem(DWORD dwGridIndex, DWORD dwBoxIndex, DWORD dwPosIndex);
#endif

#ifdef _CLIENT
    BOOL        AddItem(DWORD dwGridIndex, KItem* pItem);
#endif

    BOOL        DelItem(DWORD dwGridIndex);
    KItem*      GetItem(DWORD dwGridIndex);

    BOOL        SetMoney(int nMoney);
    int         GetMoney();
    
    void        Confirm();
    void        Cancel();

    void        SetKey(int nKey);
    int         GetKey();

    KPlayer*    GetOtherSide();
    
#ifdef _SERVER
    BOOL        FinishTradingCheck();
    void        FinishTrading();
#endif

    void        ClearTradingInfo();
private:
    void        SetConfirmFlag(BOOL bConfirm);
    BOOL        GetConfirmFLag();
#ifdef _SERVER
    BOOL        CheckItemExistLimit(DWORD dwTabType, DWORD dwItemIndex, int nItemAmount);
#endif

    struct KITEM_GRID
    {
        DWORD   dwItemID;    
        KItem*  pItem;

        int     nStackNum;
        DWORD   dwBoxIndex;
        DWORD   dwPosIndex;
    };

    int             m_nMoney;
    KITEM_GRID      m_ItemGrids[MAX_TRADING_BOX_SIZE];
    int             m_nTradingItemCount;

    KPlayer*        m_pSelf;
    KPlayer*        m_pOtherSide;

    BOOL            m_bConfirm;
    int             m_nKey;  
};

#endif // _KTRADING_BOX_H_


// 建立TradingBox对象的时机: 
// 在交易双方都同意交易的时, 该对象才会分配给玩家

// 释放TradingBox对象的时机:
// 1. 在交易成功
// 2. 在交易被取消
// 3. 在客户端: 交易一方跑出了客户端玩家周围的9个Region, 在调用Player对象UnInit()时
// 4. 服务端: 在玩家退出游戏的时候, 在调用Player对象UnInit()时
