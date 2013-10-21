#ifndef _KGAME_CARD_MANAGER_H_
#define _KGAME_CARD_MANAGER_H_

#include "KGameCardDef.h"
#include "KGameCardOperation.h"
#include <map>
#include <list>

class KGameCardManager
{
public:
    KGameCardManager();
    ~KGameCardManager();

    BOOL Init();
    void UnInit();

    void Activate();

    BOOL PostRequestSell(
        DWORD dwPlayerID, int nCoin, int nGameTime, BYTE byType, int nPrice, int nDurationTime
    );
    BOOL PostRequestBuy(
        DWORD dwSellerID, int nCoin, int nGameTime, BYTE byType, int nPrice, DWORD dwBuyerID
    );
    BOOL PostRequestLookup(DWORD dwPlayerID, BYTE byCardType, int nStartIndex, BYTE byOrderType, BOOL bDesc);

    void OnDBBuyRespond(
        BYTE byCode, DWORD dwSellerID, int nCoin, int nGameTime, BYTE byType, int nPrice, DWORD dwBuyerID
    );
    void OnDBLookupRespond(DWORD dwPlayerID, int nTotalCount, int nCount, KGAME_CARD* pCardList);

    void OnPaysysFreezeCoinRespond(BOOL bSucceed, DWORD dwRequestID);
    void OnPaysysAccountExchangeRespond(BOOL bSucceed, DWORD dwRequestID);

    BOOL SendMoneyToSeller(KGAME_CARD_BUY_OPERATION* pOperation);

private:
    typedef std::map<DWORD, KGAME_CARD_SELL_OPERATION, std::less<DWORD>,
        KMemory::KAllocator<KGAME_CARD_SELL_OPERATION> > KGAME_CARD_SELL_TABLE;
    KGAME_CARD_SELL_TABLE   m_SellTable;

    typedef std::map<DWORD, KGAME_CARD_BUY_OPERATION, std::less<DWORD>,
        KMemory::KAllocator<KGAME_CARD_BUY_OPERATION> > KGAME_CARD_BUY_TABLE;
    KGAME_CARD_BUY_TABLE    m_BuyTable;

    typedef std::list<KGAME_CARD_LOOKUP_OPERATION*,
        KMemory::KAllocator<KGAME_CARD_LOOKUP_OPERATION*> > KGAME_CARD_LOOKUP_LIST;
    KGAME_CARD_LOOKUP_LIST  m_LookupList;
    BOOL                    m_bLookupBusy;

    time_t                  m_nNextCheckDurationTime;
    DWORD                   m_dwNextResquestID;
};
#endif
