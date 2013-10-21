#ifndef _KGAME_CARD_OPERATION_H_
#define _KGAME_CARD_OPERATION_H_

#define GAME_CARD_TIMEOUT_TIME (15 * 60)

struct KGAME_CARD_OPERATION_HEAD 
{
    DWORD   dwRequestID;
    DWORD   dwPlayerID;
    time_t  nEndTime;
};

struct KGAME_CARD_SELL_OPERATION : KGAME_CARD_OPERATION_HEAD
{
    int     nCoin;
    int     nGameTime;
    BYTE    byType;
    int     nPrice;
    int     nDurationTime;
};

struct KGAME_CARD_BUY_OPERATION : KGAME_CARD_OPERATION_HEAD
{
    DWORD   dwSellerID;
    int     nCoin;
    int     nGameTime;
    BYTE    byType;
    int     nPrice;
};

struct KGAME_CARD_LOOKUP_OPERATION : KGAME_CARD_OPERATION_HEAD
{
    BYTE    byCardType;
    int     nStartIndex;
    BYTE    byOrderType;
    BOOL    bDesc;
};

#endif
